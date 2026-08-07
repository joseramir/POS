//  Idx.cpp
//  Implementación de la utilización de indices .idx
//

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Variables locales y Mensajes de error.
char pbuff[550];
char tkey[550];
char lastK[550];
char newKey[550];
char outKey[550];
char *changedField;
ValuePtr (*saveResolver)(char *);
bool revisar;


// Lee un nodo.
void inline loadNode(struct idx *x, unsigned long posi)
{
	x->fh.xLseek(posi, SEEK_SET);
	x->fh.xRead(&x->node, SZ_NODE);
}

// Escribe un nodo. A efectos de poder analizar y depurar el programa,
// escribe bytes 'EE' en el espacio de datos que no se ocupa.
// #define WRITEEE
void inline writeNode(struct idx *x, long posi)
{
	#ifdef WRITEEE	// OJO - Solo funca con el archivo de prueba
	memset(x->node.data + (x->node.nkey * 10), '\xEE',
		   500 - (x->node.nkey * 10));
	#endif
	x->fh.xLseek(posi, SEEK_SET);
	x->fh.xWrite(&x->node, SZ_NODE);	// OJO - Struct node tiene mas de SZ_NODE
}

//   De acuerdo con las especificaciones, lee de los nodos un puntero
//   'intraindex', en caso que no sea nodo extremo, o el nro de record de
//   la dbf, en caso que sea nodo extremo.
unsigned long gethex(char *p)
{
	unsigned long res;
	register int i;

	for(res = 0l, i = 4; i; i--)
		res = (res * 256) + (((unsigned long) *p++) & 0xffl);
	return res;
}

// Igual que la anterior, pero al revés (long)--->(hexa)
char *sethex(unsigned long value)
{
	static char vl[5];
	char *p1, *p2;
	int i;

	for(p1 = &vl[3], p2 = (char *) &value, i = 4; i; i--)
		*p1-- = *p2++;
	return vl;
}

//   Homologa  la clave. Esta se pasa en asciz. El tipo de la misma se de-
//   duce del tipo de la clave del indice. La clave homologada se coloca
//   en el lugar reservado para la misma en la estructura del indice.
void homologa(char *k, struct idx *id)
{
	 char *tb, *p, *p1;
	 double nro;
	 int i;

	 tb = id->tempKey;              // Clave temporaria  
	 switch(id->keyType)
	 {
		  case 'C':                 // Caracteres ya estan listos   
			   for(i = id->hd.LenKey; i; i--)
				   if(*k)
						*tb++ = *k++;
					else
						*tb++ = ' ';
			   break;

		  case 'N':                 // Numero (double float)  
			  nro = atof(k);
			  for(p1 = tb, p = ((char *) &nro) + 7; p >= ((char *) &nro);)
				  *p1++ = *p--;
			  if(nro < 0)
			  {
				 for(p = tb + 7; p >= tb; p++)
					 *p = (char)(~*p);
			  } else
			  {
				 tb[0] ^= '\x80';
			  }
			  break;
	 }
	 return;
}

//   La siguiente función va al TOP/BOTTOM de un índice, posicionando 
//   como corresponda el recno de la base maestra.
int dbf::idxGo(int offset, struct idx *x)
{
	int lk;

	loadNode(x, x->hd.RootNode);
	lk = x->hd.LenKey;
	if(offset == DB_GO_TOP)
	{
		while( !(x->node.attrib & ATT_LEAF))
			loadNode(x, gethex(x->node.data + lk));
		return dbGo(gethex(x->node.data + lk));
	}

	// Entonces, es DB_GO_BOTTOM.
	while( !(x->node.attrib & ATT_LEAF))
		loadNode(x, gethex(x->node.data + (lk+4) * (x->node.nkey-1) + lk));
	return dbGo(gethex(x->node.data + (lk+4) * (x->node.nkey-1) + lk));
}

//   La siguiente función vacía un índice.
void dbf::idxZap(struct idx *x)
{
	x->node.attrib = ATT_LEAF | ATT_ROOT;
	x->node.nkey = 0;
	x->node.nleft = x->node.nright = -1l;
	memset(x->node.data, 0, 500);

	x->hd.RootNode = 512l;
	x->hd.EofNode = 1024l;
	x->hd.FreeNode = -1l;
	x->hd.signature = 1;

	x->fh.xChsize(1024l);
	x->fh.xLseek(0l, SEEK_SET);
	x->fh.xWrite(&x->hd, SZ_NODE);
	x->fh.xWrite(&x->node, SZ_NODE);

	x->lFile = 1024l;
	x->NodeTreeCount = 0l;
}

//   La siguiente función agrega una clave en blanco en los índices, para
//   cada 'AppendBlank' que se realiza en la base.
void dbf::InsertBlankKey()
{
	struct idx *x;
	static char urecC[6], uKey[3];

	memmove(urecC, sethex(recno), 4);
	STRCPY(uKey, "");
	for(x = idxlist; x; x = x->next)
	{
		idxSearch(x, uKey);
		InsertKey(urecC, x->tempKey, x);
	}
}

ValuePtr CheckerResolver(char *name)
{
	if(!strcmp(name, changedField)) revisar = true;
	return (*saveResolver)(name);
}

//   La siguiente función es llamada después de reemplazar una campo en una
//   base. Verifica si el cambio debe actualizar un índice y, si corresponde,
//   efectúa la modificación.
void dbf::CheckIndexUpdate(char *field, char *previo, char *nuevo, int len)
{
	struct idx *x;
	static char urecC[6], nFld[20];

	if(!len || !memcmp(previo, nuevo, len)) return;
	
	saveResolver = ::GlobalNameResolver;
	::GlobalNameResolver = CheckerResolver;
	changedField = field;

	memmove(urecC, sethex(recno), 4);			// Salva registro actual
	for(x = idxlist; x; x = x->next)
	{
		revisar = false;
		SetTemporarySelected(this);
		ValuePtr res = x->keyexpr->Run();
		RestoreTemporarySelected();
		delete res;

		//   Busca el indice que es actualizable.
		if(revisar)
		{
			idxReseek(x);						// Reengancha el registro.
			DeleteKey(recno, x);				// Borra la clave vieja.
			idxSearch(x, nuevo);				// Ubica a la nueva clave.
			InsertKey(urecC, nuevo, x);			// La Inserta en el índice.
		}
	}
	::GlobalNameResolver = saveResolver;
}

//   La siguiente función 'reseekea' la clave que le corresponde al record
//   actual. Es utilizada por la función de actualización de índices, skip,
//   etc.
//
//   OJO - OJO: Parece que no esta andando muy bien. Un scaneo completo de un
//       indice se saltea algunas claves, y el agregado de claves a veces 
//       corrompe al indice. Parece ser que el BUG esta aca...
int dbf::idxReseek(struct idx *x)
{
	static long xrec, anode;
	static int cnt, st;
	char *n;

	// Evalua la expresion de indice y efectua el seek
	xrec = recno;
	SetTemporarySelected(this);
	Value *v = x->keyexpr->Run();
	RestoreTemporarySelected();
	STRCPY(x->reKey, v->ToString());
	delete v;

	if(!idxSearch(x, x->reKey))
	{
		x->KeyFound = x->node.data;
		return 0;
	}

	// La clave a la que apunta el resultado de 'idxsearch' no es necesa-
	// riamente la que buscamos, en el caso que el índice tenga claves du-
	// plicadas. Recordar que, en este caso, la que devuelve 'IdxSearch'
	// el la PRIMERA que encuentra. La clave que nos interesa, incluso,
	// puede estar en algún nodo más a la derecha.
	memmove(x->reKey + x->hd.LenKey, sethex(xrec), 4);

	// Ahora, levanta el nodo donde se encuentra la clave seekeada.
	anode = x->NodeTree[x->NodeTreeCount - 1];
	do
	{
		loadNode(x, anode);
		anode = x->node.nright;
		for(cnt = x->node.nkey, n = x->node.data;
			cnt > 0 && ((st = memcmp(x->reKey, n, x->hd.LenKey + 4)) > 0);
			n += x->hd.LenKey + 4, cnt--)
			;
	} while(st && (anode != -1l));
	x->KeyFound = n;
	return cnt;
}

// Retorna TODOS los recnos de todas las entradas para el indice actual.
int *dbf::AllIndexEntries()
{
	// Valida la operacion
	if(!idxmaster) return NULL;

	// Variables a usar.
	int *result = new int[dbhd.RecCount];
	int resIdx = 0;

	// Va a la primera entrada del indice.
	idxGo(DB_GO_TOP, idxmaster);

	// Scanea todos los nodos LEAF levantando los recnos que corresponden.
	int lk = idxmaster->hd.LenKey;
	while(true)
	{
		// Levanta todos los recnos del nodo actual.
		char *n = idxmaster->node.data;
		for(int i=idxmaster->node.nkey; i>0; i--)
		{
			result[resIdx++] = gethex(n + lk);
			n += lk + 4;
		}

		// Se mueve al nodo inmediatamente a la derecha.
		if(idxmaster->node.nright < 0) break;
		loadNode(idxmaster, idxmaster->node.nright);
	}

	// Padea el resultado con records invalidos.
	while(resIdx < (int)dbhd.RecCount)
		result[resIdx] = dbhd.RecCount + 1;
	return result;
}

//   La siguiente función, a partir de la posición actual del índice 
//   busca la clave inmediata siguiente/anterior.
void dbf::idxSkip(int offset, struct idx *x)
{
	int lk, cnt;
	char *n;

	if(offset == 0) return;

	// Inicializa algunas variables rápidas de control
	lk = x->hd.LenKey;

	//  Apunta al registro actual.
	cnt = idxReseek(x);

	//  Avanza un registro.
	n = x->KeyFound;
	if(offset > 0)
	{
		if(cnt > 1)			// Quedan claves en el nodo!!!
			n += (lk + 4);
		else
		{
			n = NULL;
			while(x->node.nright != -1l)
			{
				loadNode(x, x->node.nright);
				if(x->node.nkey > 0)
				{
					n = x->node.data;
					break;
				}
			}

			// No hay mas claves???
			if(n == NULL)
			{
				dbGo(DB_GO_BOTTOM);
				flags.eof = 1;
				return;
			}
		}

		// 'n' apunta a la siguiente clave a utilizar.
		dbGo(gethex(n + lk));
		return;
	}

	// Retrocede un lugar.
	if(n > x->node.data)	// Quedan claves en el nodo!!!
		n -= (lk + 4);
	else
		if(x->node.nleft != -1l)
		{
			loadNode(x, x->node.nleft);
			n = x->node.data + (x->node.nkey - 1) * (lk + 4);
		}
		else
		{
			dbGo(DB_GO_TOP);
			flags.bof = 1;
			return;
		}
		
	// 'n' apunta a la siguiente clave a utilizar.
	dbGo(gethex(n + lk));
	return;
}

//   La siguiente rutina BUSCA a traves del arbol del indice, la clave que se le pasa como parametro.
//   El TIPO de la clave, surge de la evaluacion de la expresion clave. La clave que se pasa para la busqueda, 
//   esta colocada en el formato STANDARD del almacenamiento de claves en el indice.
//
//   Por necesidad de las funciones que agregan y suprimen claves en un indice, a medida que se efectua la 
//   busqueda se va guardando una traza de los nodos consultados. Para esto, cada estructura de indice tiene
//   lugar reservados para hasta 100 nodos. Si esto no alcanza, se da un error pues lo mas probable es una 
//   destruccion total del indice.
//
//   Retorna: 0     - Si la busqueda no fue satisfactoria.
//            recno - Si la busqueda tuvo exito.
//
unsigned long dbf::idxSearch(struct idx *id, char *skey)
{
     char *n, *u;
	 static int lk, st, cnt;
	 static unsigned long nxt;

	 homologa((char *) skey, id);
	 u = id->tempKey;
	 id->NodeTree[0] = id->hd.RootNode;
	 id->NodeTreeCount = 1;
	 loadNode(id, id->hd.RootNode);
	 lk = id->hd.LenKey;
	 for( ;; )
	 {
		if(id->node.attrib > 3)         // Corrupcion Interna.
			return 0l;
		for(cnt = id->node.nkey, n = id->node.data;
			cnt>0 && ((st = memcmp(u, n, lk)) > 0);
            n += 4 + lk, cnt--)
            ;           // NADA!!
		
		// cnt == 0, indica que la clave que se está buscando es más grande
		// que cualquier clave en el índice. También, se debería de-
		// tectar esta situación solo en el nodo raíz. Dado que los
		// resultados del 'seek' son usados posteriormente para insertar
		// y borrar claves, entonces, en éste caso, camina hasta el nodo
		// extremo right.
		if(!cnt &&  (id->node.attrib & ATT_ROOT))
		{
			while( !(id->node.attrib & ATT_LEAF))
			{
			    n = id->node.data + (id->node.nkey - 1) * (lk + 4);
				nxt = id->NodeTree[id->NodeTreeCount++] = gethex(n + lk);
				loadNode(id, nxt);
			}
			n = id->node.data + (id->node.nkey - 1) * (lk + 4);
			view.recno_0 = gethex(n + lk);
			return 0l;
		}

		//  Si llegó a un nodo extremo, toma el recno_0 y retorna.
		if(id->node.attrib & ATT_LEAF)
		{
			view.recno_0 = gethex(n + lk);
			return !st? view.recno_0 : 0l;
		}
		nxt = id->NodeTree[id->NodeTreeCount++] = gethex(n + lk);
		if(nxt + 512l > id->lFile)         // Corrupcion detectada
		{
			id->NodeTreeCount--;
			return 0;
		}
		loadNode(id, nxt);
	 }
	 return 0l;
}

//  Cierra toda la lista de indices relacionadas. SOLO puede usarse internamente.
void dbf::closeIndex()
{
	struct idx *i, *nxt;

	if(!flags.in_use) return;
	for(i = idxlist; i; i = nxt)
	{
		i->fh.xClose();
		nxt = i->next;
		FREE(i->tempKey);
		FREE(i->reKey);
		FREE(i->idName);
		FREE(i->ixNic);
		FREE(i);
	}
	idxlist = idxmaster = 0;
}

//   Abre un indice y lo asocia a la db en uso.
int dbf::SetIndex(char *fn)
{
	struct idx *id, *x;

	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }

	//  Prepara la estructura que contendra los parametros del indice.
	id = new(idx);
    if(!id)
    {
        view.dberr = DB_NO_MEMORY;
        return !view.dberr;
    }
	memset(id, 0, sizeof(idx));

	id->idName = (char *) MALLOC(strlen(fn) + 10, "SetIndex1");
	STRCPY(id->idName, fn);
	ReplaceExtension(id->idName, ".idx", false);
	int shFlag = view._exclusive? SH_DENYRW: SH_DENYNO;
	if(id->fh.xOpen(id->idName, O_BINARY | O_RDWR, shFlag) < 0)
	{
		FREE(id->idName);
		FREE(id);
		if(errno == EACCES)
        {
            view.dberr = DB_IDX_EXCLU;
            return !view.dberr;
        }
        else
        {
            view.dberr = DB_IDX_NOTFOUND;
            return !view.dberr;
        }
	}

	//   Lee la descripción del indice.
	id->fh.xRead(&id->hd, sizeof(struct idxHeader));

	// Compila la expresion para ver que no tenga errores de sintaxis
	id->keyexpr = Evaluator::Compile(id->hd.key);
	if(id->keyexpr->Error() != NULL) 
	{
		view.dberr = DB_NO_DBOPEN;
		RestoreTemporarySelected();
		return view.dberr;
	}

	//   Prepara el largo de la clave
	SetTemporarySelected(this);
	ValuePtr sample = id->keyexpr->Run();
	RestoreTemporarySelected();
	id->keyType = (sample->Type() == 'S')? 'C': ((sample->Type() == 'D')? 'N': 'U');
	//id->hd.LenKey = (sample->Type() == 'D')? strlen(sample->ValueString());
	delete sample;

	if(id->keyType == 'U' || id->hd.LenKey <= 0)
	{
		id->fh.xClose();
		FREE(id->idName);
		FREE(id);
		return DB_BAD_IDXKEY;
	}

	//  Establece la clave temporaria.
	id->tempKey = (char *) MALLOC(id->hd.LenKey + 6, "SetIndex2");
	id->reKey = (char *) MALLOC(id->hd.LenKey + 6, "SetIndex3");
	if(!id->tempKey || !id->reKey)
	{
		if(id->tempKey) FREE(id->tempKey);
		if(id->reKey) FREE(id->reKey);
		id->fh.xClose();
		FREE(id->idName);
		FREE(id);
		return DB_NO_MEMORY;
	}
	id->lFile = id->fh.xFilelen();

	//   Engancha el nuevo indice en la lista de indices activos.
	id->next = NULL;
	if(!idxlist)
		idxlist = id;
	else
	{
		for(x = idxlist; x && x->next; x = x->next) 
			;		// Nada!!
		x->next = id;
	}

	//   Prepara el NIC
	char *pNic;
	for(pNic = id->idName + strlen(id->idName) - 1; pNic >= id->idName; pNic--)
		if(*pNic == '\\')
			break;
	id->ixNic = STRDUP(++pNic, "dbf:SetNic");
	for(pNic = id->ixNic; *pNic; pNic++)
	{
		if(*pNic == '.')
		{
			*pNic = 0;
			break;
		}
	}
	_strupr(pNic);

	idxmaster = id;
	activeKey = id->hd.key;
	if(flags.iscursor) id->fh.xCursor();
	return 1;
}

// Devuelve la clave actual del indice.
const char *dbf::GetOrder()
{
	if(activeKey == 0) return "";
	return (const char *)activeKey;
}

//   Activa un indice como nuevo controlador.
void dbf::Setorder(char *nwTag)
{
	idx *x;

	if(activeKey && !strcmp(activeKey, nwTag)) return;
	for(x = idxlist; x; x = x->next)
	{
		if(!_stricmp(x->ixNic, nwTag))
		{
			idxmaster = x;
			activeKey = _strupr(x->hd.key);
			return;
		}
	}
	idxmaster = 0;
	activeKey = 0;
}

//   Agrega un nodo vacío al índice.
unsigned long getFreeNode(struct idx *id)
{
	unsigned long npos;

	npos = id->hd.EofNode;
	id->hd.EofNode += SZ_NODE;
	id->lFile += SZ_NODE;
	id->fh.xLseek(0l, SEEK_SET);
	id->fh.xWrite(&id->hd, sizeof(struct idxHeader));
	return npos;
}

//   La siguiente función INSERTA una clave en el indice que se indica..
//	 OJO - La secuencia de nodos donde se inserta la clave, YA ESTA EN
//		   EL ARRAY NodeTree. El parámetro 'rec' contiene el valor a
//		   agregar a la clave. 'key' contiene la clave.

void dbf::InsertKey(void *rec, char *key, struct idx *id)
{
	short maxKeyInNode;
	int lk, cnt, st, expTree, emptyNode;
	char *n, tRec[6];
	long npos, pRight, anode;

	if(!id->NodeTreeCount) return;
	lk = id->hd.LenKey; 
	maxKeyInNode = 500 / (lk + 4);
	anode = id->NodeTree[id->NodeTreeCount - 1];
	loadNode(id, anode);

	//   Preserva la última key, pues si cambia, debe alterar la estructura del árbol.
	if(id->node.nkey)
	{
		if(id->node.nkey > maxKeyInNode)
			emptyNode = 1;
		emptyNode = 0;
		memmove(lastK, id->node.data + ((lk + 4) * (id->node.nkey-1)), lk + 4);
	}
    else
		emptyNode = 1;
	
	//  Prepara la clave a usar. (La clave se compone de la clave en si misma y del recno del registro).
	//  El siguiente 'if' es porque InsertKey se llama recursivamente y en ese caso, el parámetro key
	//  es tkey, y con el 'if' se evita un movimiento innecesario de la clave.
	if(tkey != key)	memmove(tkey, key, lk);
	memmove(tkey + lk, (char *) rec, 4);
	for(cnt = id->node.nkey, n = id->node.data;
		cnt>0 && ((st = memcmp(tkey, n, lk + 4)) > 0);
		n += 4 + lk, cnt--)
		;				// Nada

	//   Mueve las claves necesarias e inserta la nueva.
	//   OJO - La clave que se inserta NUNCA puede ser igual a la última puesto que el numero de recno
	//         es parte de la key. Todo deberia manejarse bien con el siguiente codigo...
	if(cnt)
		memmove(n + lk + 4, n, (lk + 4) * cnt);
	memmove(n, tkey, lk + 4);		// Inserta la clave
	expTree = 0;
	if(id->node.nkey + 1 > maxKeyInNode)
	{
		expTree++;
		memmove(outKey, id->node.data + (lk + 4) * id->node.nkey, lk + 4);
	}
	else
		id->node.nkey++;
    writeNode(id, anode);

	//  Si cambió la estructura de la última clave del nodo, entonces 'camina' hacia arriba actualizando
	//  los nodos índices. De la forma que procesa en el bloque anterior, id->node.nkey
	//  apunta SIEMPRE a la última clave disponible en el nodo actual.
	if(!emptyNode && memcmp(lastK,
		id->node.data + ((lk+4)*(id->node.nkey-1)), lk))
	{
		int treeSP;

		memmove(newKey, id->node.data+((lk + 4) * (id->node.nkey - 1)),lk);
		for(treeSP = id->NodeTreeCount-2; treeSP >= 0; treeSP--)
		{
			memmove(lastK + lk, sethex(id->NodeTree[treeSP + 1]), 4);
			loadNode(id, id->NodeTree[treeSP]);
			for(cnt = id->node.nkey, n = id->node.data;
				cnt > 0 && ((st = memcmp(lastK, n, lk + 4)) > 0);
				n += 4 + lk, cnt--)
				;				// Nada
			if(!st)
			{
				memmove(n, newKey, lk);
	            writeNode(id, id->NodeTree[treeSP]);
				if(cnt > 1) break;
			}
			else
				break;	// No encontró ninguno. Error?
		}
	}

	// Trata de insertar a la clave que se 'cayó', como primera/ clave del nodo que está inmediatamente
	// a la derecha, si hay alguno. Si lo consigue, desactiva expTree para que no se cree un nuevo nodo. 
	// Esto mantiene balanceados a los nodos en la medida de lo posible.
	if(expTree)
	{
		loadNode(id, anode);
		npos = id->node.nright;
		if(npos > 0)
		{
			loadNode(id, npos);
			if(id->node.nkey < maxKeyInNode)
			{
				if(id->node.nkey)
					memmove(id->node.data + lk + 4, id->node.data,
							id->node.nkey * (lk + 4));
				memmove(id->node.data, outKey, lk + 4);
				id->node.nkey++;
				writeNode(id, npos);
				expTree = 0;
			}
		}
	}

	// Verifica si el nodo al que se le 'cayó' una clave, era el nodo raíz. De ser así, esto tiene un 
	// tratamiento especial ya que el viejo nodo raíz (mas la clave que se 'cayó') se parte en dos, 
	// y estos dos nodos pasan a ser nodos 'intraindex' o 'leaf', según corresponda, y se crea un nuevo
	// nodo que será el nuevo raíz. Se deberia podr hacer algo para balancear el nodo raiz...
	if(expTree)
	{
		loadNode(id, anode);
		if(id->node.attrib & 1)
		{
			int k1, k2;

			npos = getFreeNode(id);
			memmove(id->node.data + ((lk + 4) * id->node.nkey), outKey, lk + 4);
			k1 = id->node.nkey / 2;
			k2 = id->node.nkey - k1 + 1;
			
			// Crea el nuevo nodo izquierdo.
			memmove(pbuff, id->node.data + ((lk + 4) * k1), (lk + 4) * k2);
			memmove(newKey, id->node.data + ((lk + 4) * (k1 - 1)), lk);
			id->node.nkey = k1;
			id->node.attrib &= ~ATT_ROOT;
			id->node.nleft = -1l;
			id->node.nright = npos;
			writeNode(id, anode);

			// Crea el nuevo nodo derecho.
			memmove(id->node.data, pbuff, ((lk + 4) * k2));
			memmove(outKey, id->node.data + ((lk + 4) * (k2 - 1)), lk);
			id->node.nkey = k2;
			id->node.nleft = anode;
			id->node.nright = -1l;
			writeNode(id, npos);
			
			// Crea el nuevo nodo raíz.
			memmove(id->node.data, newKey, lk);	// Ptro nodo izquierdo
			memmove(id->node.data + lk, sethex(anode), 4);
			memmove(id->node.data + lk + 4, outKey, lk);	// Derecho
			memmove(id->node.data + lk + lk + 4, sethex(npos), 4);
			id->node.nkey = 2;
			id->node.nleft = id->node.nright = -1l;
			id->node.attrib = ATT_ROOT;
			npos = getFreeNode(id);
			writeNode(id, npos);

			// Indica en el header el nuevo nodo raíz.
			id->hd.RootNode = npos;
			id->fh.xLseek(0l, SEEK_SET);
			id->fh.xWrite(&id->hd, sizeof(struct idxHeader));

			//  Indica que la operación está finalizada.
			expTree = 0;
		}
	}

	// Obtiene la posición del nuevo nodo, sacándolo de la lista
	// de nodos liberados, o creando uno nuevo si corresponde.
	if(expTree)
	{
		loadNode(id, anode);
		npos = getFreeNode(id);
		pRight = id->node.nright;
		id->node.nright = npos;
		writeNode(id, anode);
		
		//  Construye el nuevo nodo. (El atributo se hereda).
		id->node.nkey = 1;
		id->node.nright = pRight;
		id->node.nleft = anode;
		memmove(id->node.data, outKey, lk + 4);
		writeNode(id, npos);

		//  Inserta la referencia en el nodo superior.
		id->NodeTreeCount--;
		memmove(tkey, outKey, lk);
		memmove(tRec, sethex(npos), 4);
		InsertKey(tRec, tkey, id);
	}
}

//   La siguiente función remueve una clave (identificada por el db->recno)
//   del indice que se indica.
//   OJO - La secuencia de nodos donde está la clave YA ESTA EN EL ARRAY
//         NodeTree. El parámetro 'rec' solo sirve para identificar cla-
//         ves DENTRO del nodo actualmente apuntado.
void dbf::DeleteKey(unsigned long rec, struct idx *id)
{
	 unsigned long nleft, nright;
	 int cnt, lk;
	 char *n;

	 // Si está en el root node - finaliza.
	 if(!id->NodeTreeCount) return;

	 // Busco la clave cuyo NODE PTR sea el que se quiere borrar.
	 loadNode(id, id->NodeTree[id->NodeTreeCount - 1]);
	 lk = id->hd.LenKey;
	 for(cnt = id->node.nkey, n = id->node.data; cnt;
	     n += 4 + lk, cnt--)
		 if(rec == gethex(n + lk))
			break;
	 if(!cnt) return;       //   No la encontro?????
	 id->node.nkey--;       //   Una clave menos
	 if(--cnt)				//   Mueve las restantes un lugar atrás.
		memmove(n, n + lk + 4, cnt * (lk + 4));

	 //   Si quedo por lo menos una clave en el nodo, reescribo el nuevo
	 //   nodo, sino, inicio los pasos de supresion de nodos vacios.
	 //   OJO - Si el nodo que se está deleteando NO tiene nodo izquier-
	 //         do ni derecho, entonces se está tratando de suprimir un
	 //         nivel. En este caso, el nodo se escribe vacío porque no
	 //			está prevista la supresión de niveles.
	 nleft = id->node.nleft;
	 nright = id->node.nright;
	 if(id->node.nkey || ((nleft == -1l) && (nright == -1l)))
		writeNode(id, id->NodeTree[id->NodeTreeCount - 1]);
	  else
	  {
		//  Los siguientes pasos remueven el nodo, interconectando entre
		//  si a los nodos que antes lo tenian a este como su nodo de-
		//  recho e izquierdo.
		if(nleft != -1l)
		{
			 loadNode(id, nleft);
			 id->node.nright = nright;
			 writeNode(id, nleft);
		}
		if(nright != -1l)
		{
			 loadNode(id, nright);
			 id->node.nleft = nleft;
			 writeNode(id, nright);
		}
		rec = id->NodeTree[--id->NodeTreeCount];
		DeleteKey(rec, id);
	  }
}

#ifdef TESTIDX
char xKey[10];
int CheckNode(struct idx *x, long n)
{
	int cnt, lk, maxKeyInNode;
	char *p, pKey[10];
	long nxtNode;


	// El largo de la key solo puede ser 6 (Archivo de prueba).
	lk = x->hd.LenKey;
	if(lk != 6)
	{
		printf("Error - Largo de key != 6 (%d).\n", lk);
		return 0;
	}

	// Carga el nodo en memoria.
	loadNode(x, n);

	// Dado que solo estamos insertando, no puede haber nodos vacíos.
	if(x->node.nkey == 0)
	{
		printf("Nodo: %ld con 0 keys.\n", n);
		return 0;
	}
	
	// Verifica la identidad del nodo.

	maxKeyInNode = (500 / (lk + 4));
	if(x->node.nkey > maxKeyInNode)
	{
		printf("El nodo %lX tienen nro inválido de entradas. (%d - Max:%d).\n",
			    n, x->node.nkey, maxKeyInNode);
		return 0;
	}

	// El atributo puede ir de 0 a 3 exclusivamente.
	if(x->node.attrib < 0 || x->node.attrib > 3)
	{
		printf("Atributo %d inválido, nodo %lX\n", x->node.attrib, n);
		return 0;
	}

	// Parametro para chequear correlatividades.
	STRCPY(pKey, "      ");
	for(cnt = x->node.nkey, p = x->node.data; cnt; cnt--,p += lk+4)
	{
		// Detecta el error de claves no correlativas.
		if(memcmp(pKey, p, lk) > 0)
		{
			printf("Claves no consecutivas. Nodo:%lX, nkey:%d.\n",
				    n, x->node.nkey);
			return 0;
		}
		memmove(pKey, p, lk);

		//  Ahora, corresponde que chequee el sub-tree.
		if( !(x->node.attrib & ATT_LEAF))
		{
			nxtNode = gethex(p + lk);
            if(!CheckNode(x, nxtNode))
			{
				printf("CheckNode(x, %lX) dió error.\n", nxtNode);
				printf("Se lo llamó desde %lX, key '%s'.\n",
					    n, pKey);
				return 0;
			}

			//  Verifica la referencia entre ramas del árbol.
			if(memcmp(pKey, xKey, lk))
			{
				pKey[lk] = xKey[lk] = 0;
				printf("La última clave de %lX (%s)\n", nxtNode, pKey);
				printf("NO es igual al marcador de %lX (%s)\n", n, xKey);
				return 0;
			}

			//  Recargo el nodo que estaba chequeando.
			loadNode(x, n);
		}
	}
	memmove(xKey, x->node.data + ((lk+4) * (x->node.nkey-1)), lk);
	return 1;
}

int dbf::CheckIdx()
{
	struct idx *x;
	long actNode, prvNode;
	char pKey[20];
    int lk;

	//  Inicializa algunas variables.
	x = idxmaster;
    lk = x->hd.LenKey;

	//  Primero, verifica que el contenido de los nodos no sea mierda.
	if(!CheckNode(x,x->hd.RootNode)) return 0;

	//  Ahora, verificaremos que los niveles sean coherentes.
	actNode = x->hd.RootNode;
	loadNode(x, actNode);
	for( ;; )
	{
		// Camina hasta el extremo izquierdo.
		while(x->node.nleft != -1l)
		{
			actNode = x->node.nleft;
			loadNode(x, actNode);
		}
		
		// Compara la string de referencia con la primera del nodo.
		STRCPY(pKey, "      ");
		prvNode = -1l;
		for( ;; )
		{
			if(memcmp(pKey, x->node.data, lk) > 0)
			{
				printf("Intra-Nodes fuera de secuencia: %lX<->%lX\n",
					actNode, prvNode);
				return 0;
			}
			memmove(pKey, x->node.data + (lk+4) * (x->node.nkey-1), lk);
			prvNode = actNode;
			actNode = x->node.nright;
			if(actNode == -1l)
				break;
			loadNode(x, actNode);
		}
		if(x->node.attrib & ATT_LEAF)
			break;
		actNode = gethex(x->node.data + lk);
		loadNode(x, actNode);
	}
	return 1;
}
#endif
