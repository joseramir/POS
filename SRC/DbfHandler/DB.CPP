//  Db.cpp
//  Implementación del acceso a bases de datos.
//

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Lista de mensajes de error, del sistema de base de datos.
char *dberrMSG[] = {
	"",							 "Base de Datos No Encontrada",
    "No es archivo tipo Foxpro", "Lista vacía de campos",
	"No alcanza la memoria",	 "No hay archivo abierto",
	"Nro inválido de registro",	 "Registro bloqueado por otro usuario",
	"Campo no encontrado",		 "No puede reemplazar registro no trabado",
	"Archivo en uso exclusivo",	 "Fin de archivo encontrado",
	"Inicio de archivo encontrado","Clave de Indice no permitida",
	"Acceso negado a un indice",  "Indice no encontrado"};

// Agrega a la resolucion de nombres del evaluador de expresiones, el resolver nombres
// a partir de la tabla actualmente seleccionada.
dbf *selected = NULL;
dbf *selstack[32];
int selsp;

ValuePtr ResolveFieldNames(char *name)
{
	ValuePtr result = NULL;
	if(selected == NULL) return result;
	int fType = selected->FieldType(name);
	if(fType == 'N')
	{
		char tmp[32];
		selected->Get(name, tmp);
		double v = atof(tmp);
		result = new Value(v);
	}
	else
	{
		int fLen = selected->FieldLen(name);
		if(fLen > 0)
		{
			result = new Value(0);
			result->SetType('S');
			result->SetStringValue(NULL, fLen);
			selected->Get(name, result->ValueString());
			result->ValueString()[fLen] = 0;
		}
	}
	return result;
}
	
void SetTemporarySelected(dbf *_dbf)
{
	if(selsp >= 31) throw "Muchas DBF seleccionadas";
	selstack[selsp++] = selected;
	selected = _dbf;
}

void RestoreTemporarySelected()
{
	if(selsp <= 0) throw "Mal balanceado";
	selected = selstack[--selsp];
}

//   Funciones de implementación de la clase 'dbf'
dbf::dbf()
{
	memset(this, 0, sizeof(class dbf));
}

dbf::~dbf()
{
	Close();
}

//  Cierra un archivo de base de datos, si estuviere abierto.
void dbf::Close()
{
	if(!flags.in_use) return;
	ClearRelation(0);
	closeIndex();

	//   Vacía datos al archivo, destraba y cierra.
	flushWrite();
	Lock(DB_UNLOCK);
	fh.xClose();

	//   Libera buffers.
	if(dbName != NULL) FREE(dbName);		
	if(uRec != NULL) FREE(uRec);
	if(Flds != NULL) FREE(Flds);
	
	//   Marca el área como inactiva.
	flags.in_use = 0;
}

//   Descarga todos los buffers y escrituras pendientes en el
//   archivo fisico de disco.
void dbf::Flush()
{
	if(!flags.in_use) return;
	flushWrite();			// Descarga escrituras pendientes.
	fh.xFlush();			// Descarga caches de disco.
}

//   Copia TODO el contenido de una base de datos, en otra, manteniendo
//   una relacion por campo.
void dbf::InsertFrom(dbf& src)
{
	struct dbField *f;
	int i;
	char *tBuff;

    tBuff = (char *) MALLOC(1000, "InsertFrom");
	for(i = src.cFlds, f = Flds; i; i--, f++)
	{
		src.Get(f->name, tBuff);
		Replace(f->name, tBuff);
	}
    FREE(tBuff);
}

//   Abre un archivo de base de datos. Recibe como parámetro el nombre
//   del archivo. Si el nombre recibido no tiene extensión, entonces
//   le agrega la extensión '.dbf'.
int dbf::Use(char *fn)
{
	unsigned i;
	unsigned long LastOffset;
	struct dbField *f;

	Close();
	view.dberr = DB_NO_ERROR;
	::GlobalNameResolver = ResolveFieldNames;

	// Establece el nombre del archivo y lo abre.
	Error = "Ok";
	dbName = (char *) MALLOC(strlen(fn) + 10, "Use1");
	if(dbName == NULL) return 0;
	STRCPY(dbName, fn);
	ReplaceExtension(dbName, ".dbf", false);
	int shFlag = view._exclusive? SH_DENYRW: SH_DENYNO;
	if(fh.xOpen(dbName, O_BINARY | O_RDWR, shFlag) < 0)
	{
		FREE(dbName);
		Error = fh.Error;
		if(errno == EACCES)
        {
            view.dberr = DB_FILE_EXCLU;
            return !view.dberr;
        }
        else
        {
            view.dberr = DB_FILE_NOT_FOUND;
            return !view.dberr;
        }
	}

	//   Lee el encabezamiento de la base y chequea el tipo de la misma.
	fh.xRead(&dbhd, sizeof(struct dbHeader));
	switch(dbhd.dbID)
	{
		case NOMEMO:		//   FoxBase sin memo.
			break;

		default:
			FREE(dbName);
            view.dberr = DB_NOT_DB;
            return !view.dberr;
	}

	//   Realiza las ultimas comprobaciones.
	cFlds = (dbhd.FirstDataByte / 32) - 1;
	if(!cFlds)
	{
		FREE(dbName);
        view.dberr = DB_FILE_EMPTY;
        return !view.dberr;
	}

	//   Ubica memoria para contener la lista de definicion de campos.
	Flds = new dbField[cFlds]; // (struct dbField *)  MALLOC(sizeof(struct dbField) * cFlds, "Use2");

	//   Ubica memoria para contener los datos del record actual.
	uRec = (unsigned char *) MALLOC(dbhd.LenRecord, "Use3");
	if(!Flds || !uRec)
	{
		FREE(dbName);
		if(uRec) FREE(uRec);
		if(Flds) FREE(Flds);
        view.dberr = DB_NO_MEMORY;
        return !view.dberr;
	}

	//   Carga la lista de campos.
	LastOffset = 1;
	for(f = Flds, i = 0; i < cFlds; i++)
	{
		fh.xLseek((i + 1) * 32, SEEK_SET);
		fh.xRead(f, sizeof(struct dbField));
		f->OffsetInRec = LastOffset;
		LastOffset += f->FieldLen;
		f++;
	}

	//   Inicializa algunas otras variables de control.
	rel = 0;
	idxlist = idxmaster = 0;
	activeKey = 0;

	//   Establece los flags finales.
	flags.eof = flags.bof = flags.wri = flags.lock = flags.rdy = 0;
	flags.excl = view._exclusive? 1 : 0;
	flags.in_use = 1;
	recno = 2; dbGo(DB_GO_TOP);
	return 1;
}

//  Establece una relación con otra base.
void dbf::SetRelation(dbf *target_, char *field_)
{
	dbRelation *p;

	if(!flags.in_use) return;
	for(p = rel; p; p = p->next)
        if((dbf *) p->target == target_)
		{
			FREE(p->field);
			break;
		}
	if(!p)
	{
		p = new dbRelation(); // (dbRelation *) MALLOC(sizeof(dbRelation), "SetRelation1");
		p->next = rel;
		rel = p;
	}
    #ifdef __SCX__
    p->target = (dbRelation::dbf *) target_;
	#else
    p->target = target_;
	#endif
	p->field = (char *) MALLOC(strlen(field_) + 1, "SetRelation2");
	STRCPY(p->field, field_);
	GoRelation();
}

//  Elimina relaciones.
void dbf::ClearRelation(dbf *target_)
{
	dbRelation *p, *p2;

	if(!target_)
	{
		for(p = rel; p; p = p2)
		{
			p2 = p->next;
			FREE(p->field);
			FREE(p);
		}
		rel = 0;
	}
	else
	{
		for(p = rel; p; p = p->next)
		{
            #ifdef __SCX__
            if(p->target == (dbRelation::dbf *) target_) break;
			#else
            if(p->target == target_) break;
			#endif
		}
		if(p)
		{
			if(p == rel)
				rel = p->next;
			else
			{
				for(p2 = rel; p2->next != p; p2 = p2->next)
					;
				p2->next = p->next;
			}
			FREE(p->field);
			FREE(p);
		}
	}
}

//  Busca secuencialmente en un archivo.
int dbf::Locate(char *kfld, char *value)
{
	int uLen;
	char *ubuff;

	uLen = FieldLen(kfld);
	char ft = FieldType(kfld);
	if(uLen == 0) return 0;
    ubuff = (char *) MALLOC(uLen + 1, "Locate");
	if((int) strlen(value) < uLen) uLen = (int) strlen(value);
	Go(DB_GO_TOP);
	while(!Eof())
	{
		Get(kfld, ubuff);
		int res = 1;
		switch(ft)
		{
			case 'C':
				res = !memcmp(ubuff, value, uLen);
				break;

			case 'N':
				res = fabs(atof(ubuff) - atof(value)) < 0.01;
				break;

			default:
				throw "Tipo de campo no soportado para LOCATE";
		}
		if(res)
		{
            FREE(ubuff);
			return 1;
		}
		Skip(1);
	}
    FREE(ubuff);
	return 0;
}

//  Vacía las escrituras que pudieran estar pendientes.
void dbf::flushWrite()
{
	unsigned long pos;

	//  Valida el estado, para evitar hacer operaciones inútiles.
	if(!flags.in_use || !flags.wri) return;
	
	//  Determina la posición inicial del registro a escribir, en el archivo.
	pos = ((recno - 1) * dbhd.LenRecord) + dbhd.FirstDataByte;
	
	//  Bloquea el registro físico (El lógico ya está bloqueado).
	if(!flags.excl)
	{
		fh.xLseek(pos, SEEK_SET);
		fh.xLocking(LK_LOCK, dbhd.LenRecord);
	}

	//  Efectúa la escritura.
	fh.xLseek(pos, SEEK_SET);
	fh.xWrite(uRec, dbhd.LenRecord);

	//  Desbloquea el registro físico (El lógico permanece bloqueado).
	if(!flags.excl)
	{
		fh.xLseek(pos, SEEK_SET);
		fh.xLocking(LK_UNLCK, dbhd.LenRecord);
	}

	//   Acomoda los flags.
	flags.wri = 0;
}

int dbf::Pack()
{
	//   Solo se puede hacer si el archivo se abrio en forma exclusiva.
	if(!flags.excl) { view.dberr = DB_FILE_EXCLU; return !view.dberr; }

	//   Flushea escrituras pendientes y salva variables que nos interesa.
	this->Flush();
	int aRec = recno;

	//   Mueve los registros dejando solamente los deleteados.
	int src = dbhd.FirstDataByte , dst = dbhd.FirstDataByte;
	int wrtRec = 0;
	for(unsigned xRec = 0; xRec < dbhd.RecCount; xRec++)
	{
		//   Lee un registro y revisa su marca de eliminado
		fh.xLseek(src, SEEK_SET);
		fh.xRead(uRec, dbhd.LenRecord);
		if(*uRec == ' ')
		{
			//   Graba el registro en su nueva posicion (solo si no es la misma)
			if(dst != src)
			{
				fh.xLseek(dst, SEEK_SET);
				fh.xWrite(uRec, dbhd.LenRecord);
			}

			//   Incrementa el puntero del siguiente registro y el contador de registros escritos.
			dst += dbhd.LenRecord;
			wrtRec++;
		}
		else if(aRec > wrtRec) aRec--;

		//   Apunta al siguiente registro a examinar.
		src += dbhd.LenRecord;
	}

	//   Elimina el espacio fisico que pueda estar sin usar y reconstruye indices.
	if(dbhd.RecCount != wrtRec)
		this->Trunc(wrtRec);

	//   Deja buffers y punteros internos en orden.
	this->Go((aRec >= 1)? aRec: DB_GO_TOP);
        return 1;
}

//   rLock: Coloca cerrojos a nivel registro, en ambientes multiusuario.
//
//   Parametro: DB_UNLOCK para destrabar, DB_LOCK para trabar.
//
//   Atencion:
//           Cuando la accion es trabar, y el registro NO estaba previa-
//           mente trabado, pero SI leido, esta funcion fuerza una RELEC-
//           TURA de los datos del registro cambiando el flag 'rdy' a cero.
//           Esto se hace asi porque es la unica forma de asegurarse que
//           los datos cerrados son los mas actuales.
//
//           Los resultados son impredecibles si se llama a esta funcion
//           DESPUES de haber posicionado un  registro y leido o escrito
//           datos, ya que estos pueden no ser actuales, o se puede des-
//           truir la integridad de las escrituras.
int dbf::Lock(int fg)
{
	unsigned long pos;

	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }
	
	//   Algunas validaciones, para evitar procesamiento inútil.
	if(flags.excl) return 1;			// Si está abierto en modo exclusivo.
	if(fg && flags.lock) return 1;		// Si ya está cerrado y está pidiendo cerrar.
	if(!fg && !flags.lock) return 1;	// Si no está cerrado y estpa pidiendo destrabar.
	
	if(!fg) flushWrite();				// Si la orden es destrabar, descarga escrituras.

	//   Esta función solo opera sobre registros LOGICOS, a fin de mantener destrabados
	//   y abiertos a los registros fisicos correspondientes.
	if(!flags.wCdx)
		pos = 0x40000000 + (((recno - 1) * dbhd.LenRecord) + dbhd.FirstDataByte);
	else
		pos = 0x7ffffffe - recno;
	fh.xLseek(pos, SEEK_SET);
	if(!fg)                        
	{
		fh.xLocking(LK_UNLCK, 1);
		flags.lock = 0;					// Indica registro destrabado.
	} 
	else
	{
		if(!fh.xLocking(LK_LOCK, 1))
		{
			flags.lock = 1;				// Indica registro trabado.
			flags.rdy = 0;				// Fuerza relectura del registro.
		} 
		else
        {
            view.dberr = DB_REC_LOCKED;	// Retorna error de registro ocupado por otro.
            return !view.dberr;
        }
	}
	return 1;
}

//   Rutina de uso interno, utilizada por aquellas funciones que cambian
//   el valor de recno(), para efectuar el enganche en bases automática-
//   mente relacionadas con dbf::SetRelation.
void dbf::GoRelation()
{
	dbRelation *p;
    dbf *bp;
	char *tBuff;
	int LastLen, uLen;

	tBuff = 0;
	for(p = rel; p; p = p->next)
	{
		//  ´tBuff´ se utiliza para leer los datos del campo por el cuál se
		//  efectúa la relación.
		if(!tBuff)
		{
			LastLen = 100; 
            tBuff = (char *) MALLOC(LastLen, "GoRelation");
		}

		uLen = FieldLen(p->field);		// Valida el campo de la relación.
		if(uLen)
		{
			//   Si el largo del buffer no alcanza, lo redimensiona.
			if(LastLen <= uLen)
			{
				LastLen = uLen + 100;
				tBuff = (char *) REALLOC(tBuff, LastLen, "dbf:GoRelation");
			}

			//   Toma los datos del campo y engancha la base remota.
			Get(p->field, tBuff);
            bp = (dbf *) p->target;
            if(!bp->Seek(tBuff))
			{
                bp->Go(DB_GO_BOTTOM);	// Si no lo encuentra, queda en EOF
                if(!bp->Eof())
                    bp->Skip(1);
			}
		}
	}

	//  Libera el buffer utilizado.
    if(tBuff) FREE(tBuff);
}

//   Va a determinado registro. Si 'nrec' es DB_GO_TOP ó DB_GO_BOTTOM
//   y hay índice abierto, lo utiliza para 'seekear' el primer/último
//   registro. Sino, va a dbf::dbGo directamente.
int dbf::Go(unsigned long nrec)
{
	int retValue;

	if(idxmaster && (nrec == DB_GO_BOTTOM || nrec == DB_GO_TOP))
	{
		if(nrec == DB_GO_BOTTOM)
		{
			retValue = idxGo(DB_GO_BOTTOM, idxmaster);
			return retValue;
		}
		else
		{
			retValue = idxGo(DB_GO_TOP, idxmaster);
			return retValue;
		}
	}
	retValue = dbGo(nrec);
	return retValue;
}

//   Se va a determinado registro, indicado por el nro. RUTINA DE USO
//   INTERNO. La implementación usa dbf::Go...
int dbf::dbGo(unsigned long nrec)
{
	int retValue, recno_0;

	//   Valida el area.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }
	
	// Evita recargas innecesarias del registro.
	if(nrec == recno && !flags.eof && !flags.bof) 
		return 1;

	flushWrite();							// Descarga escrituras pendientes.
	Lock(DB_UNLOCK);						// Desbloquea registros trabados.
	flags.eof = flags.bof = flags.rdy = 0;	// Fuerza algunos estados.
	recno_0 = view.recno_0;

	//   Si la orden es GO TOP
	if(nrec == DB_GO_TOP)
	{
		if(!Go(1l))
		{
			flags.bof = 1;
			recno = 1;
			GoRelation();
            view.dberr = DB_BAD_RECNUM;
			view.recno_0 = recno_0;
            return !view.dberr;
		}
		GoRelation();
		view.recno_0 = recno_0;
		return 1;
	}
	
	//  Si la orden es GO BOTTOM.
	if(nrec == DB_GO_BOTTOM)
	{
		retValue = Go(dbhd.RecCount);
		GoRelation();
		view.recno_0 = recno_0;
		return retValue;
	}

	//   Si la orden es GO <record>, valida el rango del record solicitado.
	if(nrec > dbhd.RecCount)
	{
		recno = dbhd.RecCount + 1;
		flags.eof = 1;
        view.dberr = DB_BAD_RECNUM;
		view.recno_0 = recno_0;
        return !view.dberr;
	}

	//   Efectua el GO pedido.
	if(nrec < 1)
	{
		recno = 1;
		flags.bof = 1;
        view.dberr = DB_BAD_RECNUM;
		view.recno_0 = recno_0;
        return !view.dberr;
	}

	recno = nrec;		// Es un record cualquiera
	GoRelation();
	view.recno_0 = recno_0;
	return 1;
}

// Busca la definicion de una columna y la retorna.
struct dbField *dbf::getField(char *name)
{
	struct dbField *f;
	int i;
	char uName[32];

	//   Valida el area.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return NULL;
    }
	
	_strupr(STRCPY(uName, name));
	for(i = cFlds, f = Flds; i; i--, f++)
	{
	   if(!_stricmp(f->name, uName)) return f;
	}
	return NULL;
}

//   Busca un campo, levanta la información del registro y retorna en
//   el buffer el contenido del registro.
int dbf::Get(char *nm, char *buff)
{
	struct dbField *f;
	int i;
	char uName[32];

	//   Valida el area.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }
	
	_strupr(STRCPY(uName, nm));
	for(i = cFlds, f = Flds; i; i--, f++)
	{
	   if(!_stricmp(f->name, uName))
	   {
		   //   Si el puntero esta en EOF, vuelve vacio siempre.
		   if(flags.eof)
		   {
			   memset(buff, ' ', f->FieldLen);
			   buff[f->FieldLen] = '\0';
		   }
		   else
		   {
			  //   Fuerza lectura del registro, si todavia no se leyó.
			  getActualRecord();

			  //   Extrae los datos desde el buffer de registro.
			  memmove(buff, uRec + f->OffsetInRec, f->FieldLen);
			  buff[ f->FieldLen ] = '\0';
		   }
		   return 1;
	   }
	}
	buff[0] = '\0';			// Campo no encontrado.
    view.dberr = DB_FLD_NOTFOUND;
    return !view.dberr;
}

//	La siguiente función retorna 1 si el registro actual está borrado,
//  sino, retorna 0.
int dbf::Deleted()
{
	unsigned long pos;

	//   Efectua algunas validaciones.
	if(!flags.in_use || flags.eof) return 0;
    
	//   Si el registro no se leyó todavía, lo lee.
	if(!flags.rdy)
	{
		pos = ((recno - 1) * dbhd.LenRecord) + dbhd.FirstDataByte;
		fh.xLseek(pos, SEEK_SET);
		fh.xRead(uRec, dbhd.LenRecord);
		flags.rdy = 1;
	}
	return (uRec[0] == ' ')? 0: 1;
}

// Deletea (borra) el registro actual.
void dbf::Delete()
{
	//   Efectúa algunas validaciones.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return;
    }

	//   Fuerza que el registro a deletear esté bloqueado.
	Lock(DB_LOCK);
	if(!flags.excl && !flags.lock)
    {
        view.dberr = DB_REC_NO_LOCKED;
        return;
    }

	//   Lee el registro, si no está leído
	getActualRecord();
	
	//   Efectúa la operación de borrado.
	if(!flags.eof)
	{
		uRec[0] = '*';		// Marca el registro.
		flags.wri = 1;
	}
	return;
}

//   Fuerza la lectura del registro actualmente logineado, desde el disco
//   al buffer reservado al efecto.
void dbf::getActualRecord()
{
	unsigned long pos;

	if(!flags.rdy)
	{
		pos = ((recno - 1) * dbhd.LenRecord) + dbhd.FirstDataByte;
		fh.xLseek(pos, SEEK_SET);
		fh.xRead(uRec, dbhd.LenRecord);
		flags.rdy = 1;
	}
}

// Escribe los datos que se pasan, en el campo que se indica.
int dbf::Replace(char *nm, char *buff)
{
	struct dbField *f;
	int i;
	static char uName[32];

	//   Efectúa algunas validaciones.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }

	//   Busca el campo a reemplazar.
	_strupr(STRCPY(uName, nm));
	for(i = cFlds, f = Flds; i; i--, f++)
	{
	   if(!strcmp(f->name, uName))
	   {
		  //   Fuerza que el registro esté bloqueado.
		  Lock(DB_LOCK);
		  if(!flags.excl && !flags.lock)
          {
             view.dberr = DB_REC_NO_LOCKED;
             return !view.dberr;
          }

		  //   Fuerza la lectura de los datos del registro.
		  getActualRecord();

		  //   Efectúa el reemplazo, si no estamos en el EOF.
		  if(!flags.eof)
		  {
			 int c;
			 char *InDataPtr;
			
			 //  En el código siguiente, 'c' queda con la cantidad de caracteres
			 //  que se deben poner al comienzo del registro, para completar un
			 //  campo. También, si los datos vienen mas extensos que el ancho del
			 //  campo, trunca los datos.
			 c = f->FieldLen - strlen(buff);
			 if(c < 0)
			 {
				c = 0;
				buff[f->FieldLen] = '\0';
			 }
			 
			 //   Arma en 'pbuff'  el contenido del nuevo campo.
			 if(f->DataType == 'C')
			 {
				 // Caracteres, alineados a la izquierda en el campo.
				 STRCPY(pbuff, buff);
				 for(InDataPtr = pbuff + strlen(pbuff); c; c--)
					 *InDataPtr++ = ' ';
				 *InDataPtr = 0;
			 }
			 else
			 {
				 // El resto, alineados a la derecha.
				 if(c) memset(pbuff, ' ', c);
				 memmove(pbuff + c, buff, strlen(buff));
			 }
			 
			 //   Chequea si este campo afecta a un indice actualmente abierto y, 
			 //   de ser así, lo actualiza.
			 InDataPtr = (char *)(uRec + f->OffsetInRec);
			 CheckIndexUpdate(uName, InDataPtr, pbuff, f->FieldLen);
			 
			 //   Finalmente, fectúa los cambios en el contenido del registro.
			 memmove(InDataPtr, pbuff , f->FieldLen);
			 flags.wri = 1;
		  }
		  return 1;
	   }
	}
    view.dberr = DB_FLD_NOTFOUND;
    return !view.dberr;
}

//    Agrega un registro en blanco al final de la base de datos.
int dbf::AppendBlank()
{
	unsigned short sz;
	unsigned long pos, svRecno;

	//  Efectúa algunas validaciones.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }
	
	// Vacía escrituras pendientes y destraba el registro.
	Lock(DB_UNLOCK);
	flushWrite();

	//   Bloquea el registro logico.
	svRecno = recno; recno = 0l;
	while(!Lock(DB_LOCK))
		;
	recno = svRecno;

	//   ReLee el encabezamiento previo bloqueo multiusuario del mismo.
	sz = sizeof(struct dbHeader);
	if(!flags.excl)
	   do                    
		   fh.xLseek(0l, SEEK_SET);
	   while(fh.xLocking(LK_LOCK, sz));

	fh.xLseek(0l, SEEK_SET);
	fh.xRead(&dbhd, sz);

	//   Agrega el nuevo registro.
	dbhd.RecCount++;      

	//   Escribe un nuevo registro en blanco.
	pos = ((dbhd.RecCount - 1) * dbhd.LenRecord) + dbhd.FirstDataByte;
	fh.xLseek(pos, SEEK_SET);
	memset(uRec, ' ', dbhd.LenRecord);
	fh.xWrite(uRec, dbhd.LenRecord);

	//   Reescribe el encabezamiento y lo desbloquea.
	fh.xLseek(0l, SEEK_SET);
	fh.xWrite(&dbhd, sz);
	if(!flags.excl)
	{
		fh.xLseek(0l, SEEK_SET);
		fh.xLocking(LK_UNLCK, sz);
	}

	//   Desbloquea el registro logico.
	recno = 0l;
	Lock(DB_UNLOCK);

	//   Actualiza algunas variables y banderas.
	recno = dbhd.RecCount;
	flags.eof = flags.bof = flags.wri = 0;
	flags.rdy = 1;

	//   Inserta la clave en blanco, en los índices.
	InsertBlankKey();
	return 0;
}

//   Busca en un archivo, utilizando un indice.
int dbf::Seek(char *k)
{
	unsigned long n, recno_0;;

	//   Efectua algunas validaciones.
	if(!flags.in_use || !idxmaster)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }
	
	//   Efectúa la búsqueda en el índice (idxSearch retorna el recno).
	n = idxSearch(idxmaster, k);
	if(n)
        return dbGo(n);

	//   No lo encontró, posiciona en el EOF.
	dbGo(DB_GO_BOTTOM);
	recno_0 = view.recno_0;
	if(!flags.eof) Skip(1);
	view.recno_0 = recno_0;
	return 0;
}

//   Salta registros, adelanto o atrás.
int dbf::Skip(long cant)
{
	//   Efectúa algunas validaciones.
	if(!flags.in_use)
    {
        view.dberr = DB_NO_DBOPEN;
        return !view.dberr;
    }

	//   Los procesos son distintos, si el skip es hacia adelante o atrás.
	if(cant > 0)
	{
		while(cant > 0)
		{
			//   El proceso es distinto, según si hay o nó un índice activo.
			if(!idxmaster)
				Go(recno + 1);
			else
				idxSkip(1, idxmaster);

			//   Si se acabó el archivo, finaliza.
			if(flags.eof)
            {
                view.dberr = DB_EOF_ENCOUNT;
                return !view.dberr;
            }

			//   Ignora registros deleteados.
			if(!view._deleted || !Deleted())
				cant--;
		}
	}
	else
	{
		while(cant < 0)
		{
			//   Efectúa el movimiento físico del registro.
			if(!idxmaster)
				Go(recno - 1);
			else
				idxSkip(-1, idxmaster);

			//   Si no pudo avanzar hacia atras mas de lo pedido.
			if(flags.bof) 
			{
				view.dberr = DB_BOF_ENCOUNT;
				return !view.dberr;
			}

			//   Saltea registros deleteados, si corresponde.
			if(!view._deleted || !Deleted())
				cant++;
		}
	}

	//   Saltea registros deleteados si el actual está deleteado.
	while(view._deleted && Deleted() && !Eof())
		if(!idxmaster)
			Go(recno + 1);
		else
			idxSkip(1, idxmaster);
	return 1;
}

////////////////// Retornan información de control ////////////////////
//   Retorna el largo de un campo, en bytes, dado su nombre
int dbf::FieldLen(char *nm)
{
	int i;
	struct dbField *f;
	static char uName[32];

	//   Efectúa algunas validaciones.
	if(!flags.in_use) return 0;
	_strupr(STRCPY(uName, nm));                       

	//   Busca el campo del que se quiere saber el largo.
	for(i = cFlds, f=Flds; i; i--, f++)
	   if(!strcmp(f->name, uName))
			return f->FieldLen;
	return 0;
}

//   Retorna los digitos decimales de un campo, en bytes, dado su nombre.
int dbf::FieldDec(char *nm)
{
	int i;
	struct dbField *f;
	static char uName[32];

	//   Efectúa algunas validaciones.
	if(!flags.in_use) return 0;
	_strupr(STRCPY(uName, nm));                       

	//   Busca el campo del que se quiere saber el largo decimal.
	for(i = cFlds, f=Flds; i; i--, f++)
	   if(!strcmp(f->name, uName))
			return f->FieldDec;
	return 0;
}

//  Retorna como un caracter el tipo de un campo, dado su nombre.
char dbf::FieldType(char *nm)
{
	int i;
	struct dbField *f;
	static char uName[32];

	//   Efectúa algunas validaciones.
	if(!flags.in_use) return 'U';
	_strupr(STRCPY(uName, nm));

	//   Busca el campo del que se quiere saber el tipo.
	for(i = cFlds, f=Flds; i; i--, f++)
	   if(!strcmp(f->name, uName))
			return f->DataType;
	return 'U';
}

//   Retorna el valor del indicador del final de archivo (EOF).
int dbf::Eof()
{
	//   Efectúa algunas validaciones.
	if(flags.in_use && flags.eof) return 1;
	
	//   Si la base no tiene registros, entonces está en el EOF.
	if(dbhd.RecCount == 0) return 1;
	
	//   Si el registro actual está mas allá del reccount, está en EOF.
	if(recno > dbhd.RecCount) return 1;
	
	//   No está en el EOF.
	return 0;
}

//   Retorna el valor del indicador de comienzo de archivo (BOF).
int dbf::Bof()
{
	//   Efectúa algunas validaciones.
	if(flags.in_use && flags.bof) return 1;
	
	//   Si la base no tiene registros, entonces está en el BOF.
	if(dbhd.RecCount == 0) return 1;
	
	//   Si el recno es menor que uno, está en el BOF.
	if(recno <= 0) return 1;
	
	//   No está en el BOF.
	return 0;
}

//  Retorna la cantidad de registros actuales en la tabla.
unsigned long dbf::RecCount()
{
	if(!flags.in_use) return 0l;
	return dbhd.RecCount;
}

//  Retorna el nro de registro actual en la tabla.
unsigned long dbf::Recno()
{
	//   Efectúa algunas validaciones.
	if(!flags.in_use) return 0l;
	
	//   Si está en el final de archivo, retorna el reccount.
	if(flags.eof) return dbhd.RecCount;
	
	//   Retorna el nro de record actual.
	return recno;
}

//   Trunca una base de datos en determinado nro de registro.
void dbf::Trunc(unsigned long nTrunc)
{
	unsigned sz = 32;

	//   Valida algunos datos.
	if(!flags.in_use)
	{
		view.dberr = DB_NO_DBOPEN;
		return;
	}

	//   Si se la trunca a 0 records, en realidad, se esta zapeando.
	if(nTrunc == 0)
	{
		Zap();
		return;
	}
	
	//  Graba los cambios que están pendientes.
	flushWrite();
	Lock(DB_UNLOCK);

	//   Bloquea el encabezamiento en una red.
	if(!flags.excl)
	{
	   do                    
		   fh.xLseek(0l, SEEK_SET);
	   while(fh.xLocking(LK_LOCK, sz));
	}

	//   Lee el encabezamiento.
	fh.xLseek(0l, SEEK_SET);
	fh.xRead(&dbhd, sz);   

	//   Efectúa la actualización del número de registros  en el header.
	dbhd.RecCount = nTrunc;
	
	//   Cambia el tamaño físico del archivo.
	if(recno > dbhd.RecCount) recno = dbhd.RecCount;
	fh.xChsize(dbhd.FirstDataByte + (dbhd.LenRecord * dbhd.RecCount));
	
	//   Graba el nuevo encabezamiento y lo desbloquea.
	fh.xLseek(0l, SEEK_SET);
	fh.xWrite(&dbhd, sz);
	if(!flags.excl)
	{
		fh.xLseek(0l, SEEK_SET);
		fh.xLocking(LK_UNLCK, sz);
	}

	//   Si tiene indices relacionados, los reconstruye.
	if(idxlist) Reindex();
}

//   Devuelve el flag de area ocupada o no.
int dbf::Opened()
{
	if(flags.in_use || fh.xOpened()) return 1;
	return 0;
}

//   Vacía el archivo de TODOS los registros que contenga.
void dbf::Zap()
{
	struct idx *x;
	unsigned sz = 32;

	//   Valida algunos archivos.
	if(!flags.in_use)
	{
		view.dberr = DB_NO_DBOPEN;
		return;
	}

	//  Primero, vacía la dbf.
	flushWrite();				// No hace falta, pero acomoda banderas.
	Lock(DB_UNLOCK);

	//  Bloquea el encabezamiento.
	if(!flags.excl)
	{
	   do                    
		   fh.xLseek(0l, SEEK_SET);
	   while(fh.xLocking(LK_LOCK, sz));
	}

	//   Lee el encabezamiento.
	fh.xLseek(0l, SEEK_SET);
	fh.xRead(&dbhd, sz);   

	//   Actualiza los datos del encabezamiento y cambia el tamaño físico del archivo.
	dbhd.RecCount = 0l;
	fh.xChsize(dbhd.FirstDataByte);
	fh.xLseek(0l, SEEK_SET);
	fh.xWrite(&dbhd, sz);
	fh.xLseek(dbhd.FirstDataByte, SEEK_SET);
	fh.xWrite("\0", 1);

    //  Desbloquea el encabezamiento.
	if(!flags.excl)
	{
		fh.xLseek(0l, SEEK_SET);
		fh.xLocking(LK_UNLCK, sz);
	}

	//  Ahora, vacía los índices.
	for(x = idxlist; x; x = x->next)
		idxZap(x);

	//  Ahora, deja las variables y flags para que los programas accedan bien.
	recno = 1l;
	flags.bof = flags.eof = 1;
	flags.rdy = flags.wri = 0;
	GoRelation();
}

///////////////////////////////////////////////////////////////////////
//   Chequea el resultado de una operación.
//   Si detecta un error, imprime el error y detiene el programa.
void chkE(int eFlag)
{
	if(eFlag) return;
	printf("\n\ndbERROR - '%s'.\n", dberrMSG[view.dberr]);
	exit(1);
}

