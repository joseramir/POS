//
//  Create.cpp
//  Instruccion Create Table.
//

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Instruccion que crea una base cursor. (se borra al cerrarla).
int dbf::creaCursor(char *fn, char *fld)
{
	if(!creaTable(fn, fld)) return 0;
	fh.xCursor();
	flags.iscursor = 1;
	return 1;
}

//   Instruccion que crea una tabla. Mucho del codigo, es similar
//   al de la instruccion USE.
int dbf::creaTable(char *fn, char *fld)
{
	static unsigned dbNameLen, i;
	static int addExt, lstCount, sigue, tLen, tDec, DataOffset;
	struct dbField *f, *flist;
	char *pn;
	static char tName[11], tField;

	Close();
	view.dberr = DB_NO_ERROR;

	//   Compila la lista de campos.
    flist = new dbField[MAXFIELDS]; // (dbField *) MALLOC(sizeof(dbField) * (MAXFIELDS + 2), "creaTable1");
	dbhd.LenRecord = cFlds = lstCount = 0;
	DataOffset = 1;
	for(sigue = 1; sigue; )
	{
		//   Inicializa las variables.
		tLen = tDec = 0;
		tName[0] = 0;

		//   Compila el nombre del campo.
		while(isspace(*fld)) fld++;
		pn = fld;
		for(i = 0; *fld && !isspace(*fld); fld++) i++;
		if(i > 10) i = 10;
		memmove(tName, pn, i); tName[i] = 0; _strupr(tName);

		//   Compila el tipo de campo.
		while(isspace(*fld)) fld++;
		switch(*fld)
		{
			case 'n': case 'N':
				tField = 'N';
				break;
			case 'c': case 'C':
				tField = 'C';
				break;
			case 'd': case 'D':
				tField = 'D';
				break;
			case 'l': case 'L':
				tField = 'L';
				break;
			default:
				sigue = 0;
				continue;
		}

		//  Compila el parentesis.
		fld++; while(isspace(*fld)) fld++;
		if(*fld == '(')
		{
			//  Compila el largo del campo.
			fld++; while(isspace(*fld)) fld++;
			tLen = atoi(fld); 
			while(isdigit(*fld)) fld++;

			//  Compila la coma separadora de parametros.
			while(isspace(*fld)) fld++;
			if(*fld == ',')
			{
				//   Compila el largo de la parte decimal.
				fld++; while(isspace(*fld)) fld++;
				tDec = atoi(fld);
				while(isdigit(*fld)) fld++;
			}

			//  Compila el parentesis que cierra.
			while(isspace(*fld)) fld++;
			if(*fld != ')') { sigue = 0; continue; }
			fld++;
		}

		//   Chequea las variables generadas y agrega la infor-
		//   macion a la lista de campos creados.

		if(tField == 'D') { tLen = 8; tDec = 0; } 
		else if(tField == 'L') { tLen = 1; tDec = 0; }
		if(tField != 'N') { tDec = 0; }
		if(tLen > 255) tLen = 255;
		if(tDec > tLen) tDec = tLen - 1;
		if(tDec < 0) tDec = 0;
		f = (flist + lstCount); lstCount++;
		STRCPY(f->name, tName);
		f->DataType = tField;
		f->FieldLen = (char) tLen;
		f->FieldDec = (char) tDec;
		f->OffsetInRec = DataOffset;
		DataOffset += tLen;
		cFlds++;

		//   Mira si quedan mas definiciones de campos.
		while(isspace(*fld)) fld++;
		if(*fld != ',') { sigue = 0; continue; }
		fld++;
	}
	
	//  Incorpora la lista de campos a la definicion de la base.
	if(!cFlds)
	{
        FREE(flist);
        view.dberr = DB_CREATE_SINTAX;
        return !view.dberr;
	}
    Flds = new dbField[cFlds]; // (dbField *) MALLOC(sizeof(dbField) * cFlds, "creaTable2");
	memmove(Flds, flist, cFlds * sizeof(dbField));
    FREE(flist);

	//  Establece el nombre del archivo y lo crea.
	dbNameLen = strlen(fn);
    addExt = !strchr(fn, '.');
    if(addExt)
		dbNameLen += 4;
        dbName = (char *) MALLOC(dbNameLen + 1, "creaTable3");
	STRCPY(dbName, fn);
	if(addExt) STRCAT(dbName, ".dbf");
	remove(dbName);
	fh.xCreate(dbName, S_IREAD | S_IWRITE);
	fh.xClose();
	if(fh.xOpen(dbName, O_BINARY | O_RDWR, _SH_DENYRW) < 0)
	{
        FREE(Flds);
        FREE(dbName);
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

	//   Construye y escribe el encabezamiento.
	dbhd.dbID = NOMEMO;
	dbhd.FirstDataByte = (cFlds + 1) * 32;
	dbhd.LenRecord = DataOffset;
	dbhd.RecCount = 0l;
	fh.xLseek(0l, SEEK_SET);
	fh.xWrite(&dbhd, sizeof(struct dbHeader));
	for(f = Flds, i = 0; i < cFlds; i++)
	{
		fh.xLseek((i + 1) * 32, SEEK_SET);
		fh.xWrite(f++, sizeof(dbField));
	}

	//   Escribe el primer byte, para que la base quede con el
	//   header consistente.
	fh.xLseek(dbhd.FirstDataByte, SEEK_SET);
	fh.xWrite("\r", 1);

	//   Crea el area para almacenar copia del record actual.
    uRec = (unsigned char *) MALLOC(dbhd.LenRecord, "creaTable4");
	if(!uRec)
	{
        FREE(dbName);
        if(uRec) FREE(uRec);
        if(Flds) FREE(Flds);
        view.dberr = DB_NO_MEMORY;
        return !view.dberr;
	}

	//   Descarga lo necesario al disco.
	fh.xFlush();

	//   Inicializa algunas otras variables de control.
	rel = 0;
	idxlist = idxmaster = 0;
	activeKey = 0;

	//   Establece los flags finales.
	flags.eof = flags.bof = flags.wri = flags.lock = flags.rdy = 0;
	flags.excl = view._exclusive? 1 : 0;
	flags.wOnce = flags.iscursor = 0;
	flags.in_use = 1;
	recno = 2; dbGo(DB_GO_TOP);
	return 1;
}
