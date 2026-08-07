//
//   Index_on.cpp
//   Soporta la creacion y/o reconstruccion de indices .idx.
//

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Funciones en 'idx.cpp'
void homologa(char *, struct idx *);
char *sethex(unsigned long);

// Cantidad de claves contenidas en un chunk.
#define SZ_CHUNK 500

// Variables globales.
int chkSize;

//   Reconstruye TODOS los indices de una base.
void dbf::Reindex()
{
	idx *x;

	for(x = idxlist; x; x = x->next)
		lReindex(x, 0);
}

//  Busca el punto de inserccion en el pool
char *InsPoint(char *buff, char *key, int nsize, int count)
{
	int izq, der, point, npoint, res;
	char *uPtr;

	izq = 0;
	der = count - 1;
	point = 0;

	while(1)
	{
		npoint = izq + ((der - izq) / 2);
		if(point == npoint) 
		{
            point = der;
			do {
					uPtr = buff + (point * nsize);
					if(memcmp(key, uPtr, nsize) > 0)
						return(uPtr + nsize);
			} while(point-- > 0);
			return uPtr;
		}
		point = npoint;
		uPtr = buff + (point * nsize);
		res = memcmp(key, uPtr, nsize);
		if(res == 0)
		{
			do {
					uPtr = buff + (point * nsize);
					if(memcmp(key, uPtr, nsize) > 0)
						return(uPtr + nsize);
			} while(point-- > 0);
			return uPtr;
		}
		if(res < 0) 
			der = point - 1;
		else
			izq = point + 1;
	}
    return buff;
}

//  Funcion de comparacion de claves en un chunk, para el qsort.
int chunkCmp(const void *e1, const void *e2)
{
	unsigned char *p1 = (unsigned char *)e1, *p2 = (unsigned char *)e2;
	for(int i = chkSize; i>0; i--, p1++, p2++)
		if(*p1 != *p2)
			return (*p1 > *p2)? 1: -1;
	return 0;
}

//   Actualiza el indicador en pantalla.
void dbf::DefaultTally(int cnt, char *msg)
{
     if(cnt < 0)
        printf("%ld %s         \n", dbhd.RecCount, msg);
     else
        if((cnt % 100) == 0)
            printf("%ld %s         \r", cnt, msg);
}

//   Reconstruye un unico indice.
void dbf::lReindex(idx *x, void (*TallyFunc)(unsigned long))
{
	static struct idxNodeHd nh;
    static unsigned long nRec, pos, wrtPos;
    static int globCount, nLevCount, rcCount, actuCount, pendWrite, wCount, fillPool;
    static int i, chunkCount, poolCount, mb, mc, poolInsSize, yetMoved;
	static int tmpPoolCount, fillCount;
	static struct dbField *f;
    static char uName[64], *chunk, *chunkPtr, *pool, *wKey, *nlKey;
	static char *inPool, *inChunk, *p1, *p2;
		 
	//  Inicializa algunas areas de memoria y control de temporarios.
	chkSize = x->hd.LenKey + 4;
	chunk   = (char *) MALLOC((SZ_CHUNK + 10)      * chkSize, "lReindex1");
    pool    = (char *) MALLOC((dbhd.RecCount + 10) * chkSize, "lReindex2");
    if(!chunk || !pool)
    {
		if(chunk) FREE(chunk);
		if(pool) FREE(pool);
        printf("Sin memoria suficiente para reconstruir indice!!!\n" );
        return;
    }
	chunkPtr = chunk; chunkCount = 0;
	poolCount = 0;

	/////////////////////////////////////////////////////////////////////
	//   Atraviesa la base de datos, levantando la informacion sobre la 
	//   clave y armando el array 'pool'.
	if(TallyFunc)
		(*TallyFunc)(0);
	else
		DefaultTally(0, "claves generadas");

	for(nRec = 1l; nRec <= dbhd.RecCount; nRec++)
	{
		//   Actualiza la representacion en pantalla.
		Go(nRec);
		if(TallyFunc)
			(*TallyFunc)(nRec);
		else
			DefaultTally(nRec, "claves generadas");

		//   Levanta la información de los campos directamente sobre el chunk.
		SetTemporarySelected(this);
		ValuePtr keydata = x->keyexpr->Run();
		RestoreTemporarySelected();
		if(x->keyexpr->RunError() != NULL)
			throw x->keyexpr->RunError()->Message();
		int slen = strlen(keydata->ValueString());
		if(x->hd.LenKey !=  slen)
			throw "Cambio el tamaño de la clave";
		memmove(chunkPtr, keydata->ValueString(), slen);
		chunkPtr[slen] = 0;
		delete keydata;

		homologa(chunkPtr, x);
		memmove(chunkPtr, x->tempKey, x->hd.LenKey);
		memmove(chunkPtr + x->hd.LenKey, sethex(nRec), 4);
		chunkPtr += chkSize;
		chunkCount++;

		//   Manda el chunk al pool, si corresponde.
		if(chunkCount >= SZ_CHUNK || (nRec + 1 > dbhd.RecCount))
		{
			//   Ordena las claves en el chunk
			qsort(chunk, chunkCount, chkSize, chunkCmp);

			//   Si el pool está vacío, simplemente mueve el chunk.
			if(poolCount == 0)
			{
				memmove(pool, chunk, chkSize * chunkCount);
				poolCount += chunkCount;
			}
			else
			{
				//   Agrega 'chunkCount' claves al final del pool.
				memset(pool + (poolCount * chkSize), '\xff', chunkCount * chkSize);
				tmpPoolCount = poolCount + chunkCount;

				//   inChunk apunta a la ultima clave.
				yetMoved = 0;
				do {
					  //   Inicializa variables de posicionamiento.
					  inChunk = chunk + (chkSize * (chunkCount - 1));
				      poolInsSize = chunkCount * chkSize;

					  //  inPool apunta al lugar en el pool donde insertara la clave.
					  inPool = InsPoint(pool, inChunk, chkSize, tmpPoolCount);
                      if(inPool < pool) inPool = pool;

					  //  Si el punto de inserccion es el comienzo del pool, entonces
					  //  inserta el chunk completo. El siguiente codigo deja:
					  //	mc=Cantidad de claves a mover, desde el chunk al pool.
					  //    inChunk=Lugar en el chunk donde comienzan las claves a mover.
					  //    inPool=Lugar en el pool donde se insertaran las nuevas claves.
					  if(inPool == pool)
					  {
						  mc = chunkCount; 
						  inChunk = chunk;
					  }
					  else
					  {
						  //   Para hacer mas eficiente, controla si en lugar de una se
						  //   pueden insertar VARIAS claves.
						  for(mc = 1, p1 = (inPool - chkSize); inChunk >= chunk;
						      inChunk -= chkSize, mc++)
							  if(memcmp(inChunk, p1, chkSize) < 0)
							  {
								  inChunk += chkSize;
								  mc--;
								  break;
							  }
						   if(inChunk < chunk)
						   {
							   inChunk += chkSize;
							   mc--;
						   }
					  }

					  //   El siguiente codigo abre lugar en el pool.
					  mb = ((tmpPoolCount-((int)(inPool - pool) / chkSize)-(poolInsSize / chkSize))
						    * chkSize) - yetMoved;
					  if(mb > 0)
						  memmove(inPool + poolInsSize, inPool, mb);

					  //   Rellena los lugares a la izquierda con una copia de la variable
					  //   extrema izquierda. Esto para que no falle 'insPoint'. Estos datos,
					  //   finalmente, se sobreescribiran en la siguiente pasada del loop.
					  for(p2 = inPool, fillPool = poolInsSize - (chkSize * mc); fillPool > 0;
					      fillPool -= chkSize, p2 += chkSize)
						  memmove(p2, inChunk, chkSize);

					  //   Mueve al pool las claves desde el chunk.
                      memmove(p2, inChunk, chkSize * mc);

					  //   Actualiza las variables del loop.
					  yetMoved   += (mc * chkSize) + mb;
				      chunkCount -= mc;
				      poolCount  += mc;

                } while(chunkCount > 0);
				if(poolCount != tmpPoolCount)
					printf("x");
			}
			chunkPtr = chunk;
			chunkCount = 0;
		}
	}
	if(TallyFunc)
		(*TallyFunc)(-1);
	else
		DefaultTally(-1, "claves generadas");

	///////////////////////////////////////////////////////////////////////
	//   El array 'pool' ya esta construido. Ahora, baja 'nodos' al
	//   archivo directamente.

	nh.attrib = ATT_LEAF;			// Nodo con  nros de record.
	nh.nleft = -1l;
	wrtPos = SZ_NODE;				// Donde comienza a escribir.
	globCount = poolCount;
	nlKey = wKey = pool;
	actuCount = 1;
	rcCount = nLevCount = 0;
	x->fh.xLseek(0, SEEK_SET);
	if(SZ_NODE != x->fh.xRead(&x->node, SZ_NODE))
	{
		memset(&x->node, 0, SZ_NODE);
		x->fh.xLseek(0, SEEK_SET);
		x->fh.xWrite(&x->node, SZ_NODE);
	}
	if(TallyFunc)
		(*TallyFunc)(0);
	else
		DefaultTally(0, "registros indexados");

	do {
			//   Completa los datos del encabezamiento y graba los datos.
			wCount = 500 / chkSize;
			if((wCount - 1) > 1) wCount--;
			if(globCount <= wCount) 
			{
				wCount = globCount;
				nh.nright = -1l;
			}
			else
				nh.nright = wrtPos + SZ_NODE;
			pendWrite = 500 - (wCount * chkSize);

			//   Actualiza la presentacion en pantalla.
			if(actuCount)
			{
				rcCount += wCount;
				if(TallyFunc)
					(*TallyFunc)(rcCount);
				else
					DefaultTally(rcCount, "registros indexados");
			}
			
			//   Flaguea el nodo, si es el nodo raiz.
			if(poolCount <= wCount)
				nh.attrib |= ATT_ROOT;

			//   Escribe las cosas en el disco.
			nh.nkey = wCount;
			x->fh.xWrite(&nh, sizeof(struct idxNodeHd));
			x->fh.xWrite(wKey, chkSize * wCount);
			if(pendWrite)
			{
				memset(&x->node, 0xcc, pendWrite);
				x->fh.xWrite(&x->node, pendWrite);
			}

			//   La ultima clave del nodo que acabamos de escribir, se copia
			//   para ir generando el siguiente nivel.
			if( !(nh.attrib & ATT_ROOT))
			{
				memmove(nlKey, wKey + (chkSize * (wCount - 1)), x->hd.LenKey);
				memmove(nlKey + x->hd.LenKey, sethex(wrtPos), sizeof(long));
				nlKey += chkSize;
				nLevCount++;
			}

			//   Actualiza las variables de control.
			globCount -= wCount;
			wKey += (chkSize * wCount);
			nh.nleft = wrtPos;
			wrtPos += SZ_NODE;

			//   Genera el siguiente nivel, si corresponde.
			if(globCount <= 0)
			{
				globCount = poolCount = nLevCount;
				nlKey = wKey = pool;
				nh.nleft = -1l;
				nLevCount = 0;
				nh.attrib &= ~ATT_LEAF;
				actuCount = 0;
			}

	} while( !(nh.attrib & ATT_ROOT));
	if(TallyFunc)
		(*TallyFunc)(-1);
	else
		DefaultTally(-1, "registros indexados");

	///////////////////////////////////////////////////////////////////////////////////
	//   Completa los datos del encabezamiento y finaliza la reconstruccion del indice.
	//   Deja las variables de control en condiciones, a fin de poder acceder correc-
	//   tamente en posteriores accesos.
	x->hd.RootNode = wrtPos - SZ_NODE;
	x->hd.EofNode = wrtPos;
	x->hd.FreeNode = -1l;
	x->hd.signature = 1;
	x->fh.xLseek(0l, SEEK_SET);
	x->fh.xWrite(&x->hd, sizeof(x->hd));
	x->fh.xChsize(x->hd.EofNode);
	x->fh.xFlush();
	x->lFile = wrtPos;
	x->NodeTreeCount = 0l;

	//   Libera los recursos utilizados.
	FREE(chunk);
	FREE(pool);
}

//   PlaceHolder para la funcion de tally.
void IndexTally(unsigned long cRec)
{
}

//   Construye un unico indice.
int dbf::IndexOn(char *k, char *fn)
{
	unsigned idNameLen;
	int addExt;
	struct idx *id, *x;
	static char tKey[30];

	//  Chequea que esté abierta la bases.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }

	//  Prepara una estructura virtual idx para construirla.
	id = new(idx);
    if(!id)
    {
        view.dberr = DB_NO_MEMORY;
        return !view.dberr;
    }
	memset(id, 0, sizeof(idx));

	// Compila la expresion para ver que no tenga errores de sintaxis
	id->keyexpr = Evaluator::Compile(k);
	if(id->keyexpr->Error() != NULL) 
	{
		view.dberr = DB_NO_DBOPEN;
		return view.dberr;
	}

	//   Prepara el largo de la clave
	SetTemporarySelected(this);
	ValuePtr sample = id->keyexpr->Run();
	RestoreTemporarySelected();
	id->keyType = (sample->Type() == 'C')? 'C': 'N';
	id->hd.LenKey = strlen(sample->ToString());
	delete sample;
	if((id->keyType != 'C' && id->keyType != 'N') || id->hd.LenKey <= 0)
	{
		FREE(id);
		return DB_BAD_IDXKEY;
	}

	// Prepara el nombre del archivo.
	idNameLen = strlen(fn);
    addExt = !strchr(fn, '.');
    if(addExt) idNameLen += 4;
	id->idName = (char *) MALLOC(idNameLen + 1, "IndexOn1");
	STRCPY(id->idName, fn);
	if(addExt) STRCAT(id->idName, ".idx");
	remove(id->idName);
	id->fh.xCreate(id->idName, S_IREAD | S_IWRITE);
	id->fh.xClose();
	if(id->fh.xOpen(id->idName, O_BINARY | O_RDWR, _SH_DENYNO) < 0)
	{
		FREE(id->idName);
		FREE(id);
		RestoreTemporarySelected();
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
	
	//   Si la base principal es cursor, entonces este indice tambien lo sera.
	if(flags.iscursor)
		id->fh.xCursor();

	//   Construye el encabezamiento.
	id->hd.forc[0] = 0;
	id->hd.FreeNode = -1l;
	STRCPY(id->hd.key, k); 
	_strupr(id->hd.key);
	id->hd.idxFlag = 0;

	//  Establece la clave temporaria.
	id->tempKey = (char *) MALLOC(id->hd.LenKey + 6, "IndexOn2");
	id->reKey = (char *) MALLOC(id->hd.LenKey + 6, "IndexOn3");
	if(!id->tempKey)
	{
		id->fh.xClose();
		FREE(id->idName);
		FREE(id);
		return DB_NO_MEMORY;
	}
	id->lFile = 512l;

	//   Construye el indice efectivamente.
	id->fh.xLseek(0l, SEEK_SET);
	id->fh.xWrite(&id->hd, SZ_NODE);
	lReindex(id, IndexTally);

	//   Prepara el NIC
	char *pNic;
	for(pNic = id->idName + strlen(id->idName) - 1; pNic >= id->idName; pNic--)
		if(*pNic == '\\')
			break;
	id->ixNic = STRDUP(++pNic, "dbf:IndexOn2");
	for(pNic = id->ixNic; *pNic; pNic++)
	{
		if(*pNic == '.')
		{
			*pNic = 0;
			break;
		}
	}
	_strupr(pNic);

	//   Engancha el nuevo indice al pie de la lista de indices activos.
	id->next = NULL;
	if(!idxlist)
		idxlist = id;
	else
	{
		for(x = idxlist; x && x->next; x = x->next) 
			;		// Nada!!
		x->next = id;
	}
	if(flags.iscursor) id->fh.xCursor();
	idxmaster = id;
	activeKey = id->hd.key;
	return 1;
}
