//
// xBrowse.cpp
// Implementación de la clase xBrowse.
//

#include <stdafx.h>

extern char *tBuff;

xBrowseField::xBrowseField()
{
	memset(this, 0, sizeof(*this));
	x = -1;
}

xBrowseField::~xBrowseField()
{
	resetDataPtr(0);
	if(editBuff) FREE(editBuff);
}

void xBrowseField::resetDataPtr(int nSize)
{
	int i;

	if(brwData != NULL)
	{
		for(i = 0; i < cRows; i++)
			if(brwData[i])
				delete brwData[i];
		FREE(brwData);
	}
	cRows = nSize;
	if(nSize)
	{
		brwData = (xString **) MALLOC(nSize * sizeof(xString *), "xBrowseField:resetDataPtr");
		for(i = 0; i < cRows; i++)
			brwData[i] = new(xString);
	}
	else
		brwData = (xString **) 0xcdcdcdcd;		// Para que SALTE un error grave si no se inicializa.
}

void xBrowseField::setData(int row)
{
	db->Get(fldName.Data(), tBuff);
	if(!tBuff[0])
	{
		memset(tBuff, ' ', cLen);
		tBuff[cLen] = 0;
	}
	*(brwData[row]) = tBuff;
}

char *xBrowseField::GetFieldName()
{
	return this->fldName.Data();
}

void xBrowseField::RefreshFromDb(int row, int flag)
{
	char tData[100];
	this->db->Get(this->fldName.Data(), tData);
	*brwData[row] = tData;
	DisplayData(row, flag);
}

void xBrowseField::DisplayData(int row, int flag)
{
	wEsqSetColor((flag == 1)? 5: 3);
	wLocate(row + 2, x);
	if(flag < 0)
		memset(tBuff, ' ', cLen);
	else
	{
		STRCPY(tBuff, (char *) brwData[row]->Data());
		while(strlen(tBuff) < cLen)
			STRCAT(tBuff, " ");
	}

	tBuff[cLen] = 0;
	wPstr(tBuff);
}

//   Efectua la edicion de los datos del campo. Se presupone que previo a esto 'DisplayData'
//   ya armo los buffers necesarios.
int xBrowseField::Edit(int row)
{
	if(ReadOnly) return GetKey();

	//  Toma los datos a editar.
	if(editBuff == NULL)
		editBuff = (char *) MALLOC(cLen + 10, "xBrowseField:Edit");
	STRCPY(editBuff, brwData[row]->Data());
	Trim(editBuff);
	while(strlen(editBuff) < (size_t)lenFld)
		STRCAT(editBuff, " ");

	//  Determina la posicion inicial de edicion.
	int ex = x + lenFld - 1;
	while(ex >= x && isspace(editBuff[ex - x]))
		ex--;
	ex++;

	//  Efectua la edicion en loop
	int ch = -1;
	while(true)
	{
		wLocate(row + 2, ex);
		ch = GetTransformedKey();
		if(ch == '\r' || ch == keyenter) { ch = keyenter; break; }
		if(ch == keycanc || ch == keydown || ch == keyupper || ch == keyleft || ch == keyright)
		   break;

		if(ch != keyback)
		{
			if(ex - x < lenFld)
			{
				wEsqSetColor(5);
				bool proc = false;
				switch(dataType)
				{
					case 'N': if((ch>='0' && ch<='9') || ch=='.') proc = true; break;
					default:  proc = true; break;
				}
				if(proc)
				{
					editBuff[ex - x] = ch;
					wPch(ch);
					ex++;
				}
			}
		}
		else if(x != ex)
		{
			STRCAT(STRCPY(&editBuff[ex - x - 1], &editBuff[ex - x]), " ");
			ex--;
			wLocate(row+2, ex);
			wPch(' ');
		}
	}

	//  Llama a la rutina de validacion, si esta activada.
	if(ValidFunc)
	{
		if(!(*ValidFunc)(row, fldName.Data(), editBuff)) return -1;
	}

	//  Graba los datos en la tabla subyacente.
	*brwData[row] = editBuff;
	this->db->Replace(this->fldName.Data(), editBuff);
	this->DisplayData(row, 1);
	return ch;
}

void xBrowseField::sRefresh()
{
    int spc, tFlag, i, WndRows;
	char *p;

    WndRows = wClientHeigth() - 1;
	
	if(x >= 0)
	{
		//  Nombre del campo, alineado a la izquierda.
        tFlag = ((class xList *) brw)->MoreInList();
		wLocate(0, x);
		wEsqSetColor(5);
		for(p = tit.Data(), spc = cLen; spc && *p; spc--)
			wPch(*p++);

		while(spc-- > 0) wPch(' ');
		wEsqSetColor(6);
        if(tFlag) wPch('\xb3');
	
		//  Separador horizontal.
		if(x)
		{
			wSetBorder(0, x, '\xc2');
			wSetBorder(999, x, '\xc1');
		}
		wLocate(1, x);
		wEsqSetColor(6);
		for(spc = cLen; spc; spc--)
			wPch('\xc4');
        if(tFlag)
			wPch('\xc5');
		for(i = 0; i < WndRows; i++)
		{
			if(x-1)
			{
				wLocate(i + 2, x-1);
				wPch('\xb3');
			}
		}
	}
}

void xBrowseField::SetField(dbf *udb, char *newName)
{
	fldName = newName;
	db = udb;
	if(db)
	{
		lenFld = db->FieldLen(newName);
		decFld = db->FieldDec(newName);
	}
	else
	{
		lenFld = 10;
		decFld = 0;
	}
	cLen = (lenFld > lenTit)? lenFld: lenTit;
	dLen = decFld;
	sRefresh();
}

void xBrowseField::SetTitle(char *newTit)
{
	tit = newTit;
	lenTit = tit.Len();
	cLen = (lenFld > lenTit)? lenFld: lenTit;
	sRefresh();
}

///////////////////////////// class xBrowse ///////////////////////////////
//   Constructor - Destructor.

xBrowse::xBrowse()
{
	memset(this, 0, sizeof(*this));
	hWnd = -1;
}
xBrowse::~xBrowse()
{
	xBrowseField *f;

	GoTop();
	while((f = (xBrowseField *) GetPtr()))
	{
		delete f;
		Skip();
	}
	if(flags.IDefTheWindow && hWnd >= 0)
		wRelease(hWnd);
}


//   Metodos sobrecargados para crear un campo
xBrowseField *xBrowse::xBrowseF(dbf *udb, char *fldName, char *fldTit, char dType)
{
	return xBrowseF(udb, fldName, fldTit, dType, NULL);
}

xBrowseField *xBrowse::xBrowseF(dbf *udb, char *fldName, char *fldTit)
{
	return xBrowseF(udb, fldName, fldTit, 'C', NULL);
}

xBrowseField *xBrowse::xBrowseF(dbf *udb, char *fldName, char *fldTit, bool readOnly)
{
	xBrowseField *fld = xBrowseF(udb, fldName, fldTit, 'C', NULL);
	fld->ReadOnly = readOnly;
	return fld;
}

//   Agrega una columna a la definición.
xBrowseField *xBrowse::xBrowseF(dbf *udb, char *newName, char *fldTit, char dType, 
			bool (*Vld)(int, char *, char *))
{
	xBrowseField *f;

	//   Crea e inicializa la nueva estructura.
	f = new(xBrowseField);
	AddBottom(f);
	f->brw = this;

	f->SetField(udb, newName);
	f->SetTitle(fldTit);
	f->ValidFunc = Vld;
	f->dataType = dType;
	f->ReadOnly = false;
	return f;
}

//   Refresca el display.
void xBrowse::sRefresh()
{
	xBrowseField *f;
	int x;

	wActivate(hWnd);
	wSetBorder(2, 0, '\xc3');
	wSetBorder(2, 999, '\xb4');
	for(x = 0, GoTop(); f = (xBrowseField *) GetPtr(); Skip())
	{
		f->x = x;
		x += f->cLen + 1;
		f->sRefresh();
	}
}

//   Displaya un renglon.
void xBrowse::DisplayRecord(int row, int flag)
{
	xBrowseField *f;

	wActivate(hWnd);
	for(GoTop(); f = (xBrowseField *) GetPtr(); Skip())
		f->DisplayData(row, flag);
}

//   Refresca la estructura de datos.
void xBrowse::dRefresh()
{
	xBrowseField *f;
	int nRows, i;

	if(!db) return;
	RefreshDbPointers();

	wActivate(hWnd);
	nRows = wClientHeigth() - (1 + (flags.useLocator? 2: 0));
	if(nRows != WndRows)
	{
		WndRows = nRows;
		for(GoTop(); f = (xBrowseField *) GetPtr(); Skip())
			f->resetDataPtr(nRows);
	}

	aRec = db->Recno();
	i = (WndRows / (flags.CursorAtBottom? 1: 2));
	if(flags.CursorAtBottom) i--;

	// Trata de posicionar el registro elegido en el medio de la lista.
	while(i-- > 0)
	{
		db->Skip(-1);
		if(db->Bof())
		{
			db->Go(DB_GO_TOP);
			break;
		}
	}

	//   Dibuja el area del ubicador.
	if(flags.useLocator)
	{
		int x;

		wPrintSep(WndRows + 2);
	    wEsqSetColor(3);
		for(x = 0, GoTop(); f = (xBrowseField *) GetPtr(); Skip())
		{
			if(x)
			{
				wSetBorder(999, x, '\xc4');
				wLocate(WndRows + 2, x-1);
				wPch('\xc1');
			}
			x += f->cLen + 1;
		}

		wLocate(WndRows + 3, 1);
		if(strlen(FastFind))
		{
			wPstr("Dato Buscado: ");
			wPstr(FastFind);
		}
        wPstr("                                                            ");
	}
	//   Carga los datos a displayar y los nros de records asignados.
	memset(xRecnos, 0, 48 * sizeof(unsigned long));
	for(i = 0, nRows = WndRows; nRows && !db->Eof(); nRows--, i++)
	{
		xRecnos[i] = db->Recno();
		for(GoTop(); f = (xBrowseField *) GetPtr(); Skip())
			f->setData(i);
		db->Skip(1);
	}
	for( ; nRows; nRows--, i++)	xRecnos[i] = 0;

	//  Displaya la información del browse.
	for(i = 0, nRows = WndRows; nRows; nRows--, i++)
	{
		if(xRecnos[i])
			DisplayRecord(i, (xRecnos[i] == aRec)? 1: 0);
		else
			DisplayRecord(i, -1);
	}

	db->Go(aRec);
    for(i=0; xRecnos[i] && i < 48; i++)
	{
		if(aRec == xRecnos[i])
		{
			useRow = i;
			break;
		}
	}
}

//   Mueve el puntero del browse a otro lugar.
void xBrowse::xBrowseMovePtr(int sent)
{
	int actRow, i;
	unsigned long nextRecord;

	//   Chequea que no nos estemos saliendo de la base de datos.
	int tempRecord = db->Recno();
	if((sent == 1 && tempRecord == bottomRecord) || (sent == -1 && tempRecord == topRecord))
		return;

	//   Mueve el puntero de la base subyacente, si puede. Si no puede vuelve inmediatamente
	db->Skip(sent);
	nextRecord = db->Recno();

    //   Busca en la lista del browse las filas del record actual y del siguiente.
	useRow = actRow = -1;
	for(i = 0; xRecnos[i] && (i < 48); i++)
	{
		if(xRecnos[i] == aRec)			actRow = i;
		if(xRecnos[i] == nextRecord)	useRow = i;
		if(actRow > 0 && useRow > 0)	break;
	}
	if((useRow < 0) || (actRow < 0))
		dRefresh();
	else
	{
		if(useRow == actRow)
			return;
		else
		{
			DisplayRecord(actRow, 0);
			DisplayRecord(useRow, 1);
		}
	}
	aRec = nextRecord;
}

void xBrowse::RefreshDbPointers()
{
	aRec = db->Recno();
	db->Go(DB_GO_TOP); topRecord = db->Recno();
	db->Go(DB_GO_BOTTOM); bottomRecord = db->Recno();
	db->Go(aRec);
}

//   Anima un browse.
int xBrowse::xBrowseAnimate(int uWnd, dbf *udb, int LocFlag, int WaitInput, int curPos, short *eCol)
{
	xBrowseField *x;
	int ch = 0, continuar, tLen;

	flags.useLocator = LocFlag? 1: 0;
	if(uWnd)
		hWnd = uWnd;
	else
	{
		int uAncho, _x;

		for(uAncho = 0, GoTop(); x = (xBrowseField *) GetPtr(); Skip())
			uAncho += x->cLen + 1;
		_x = (80 - uAncho) / 2;
		hWnd = wDefine(2, _x, 23, _x + uAncho, BORDERSINGLE, SNGCURS,
			           eCol, "", "");
		flags.IDefTheWindow = 1;
	}

	//  Inicializa el acceso a datos.
	db = udb;
	
	//   Inicializa
	flags.CursorAtBottom = curPos? 1: 0;
	FastFind[0] = 0;								// Vacia la string del buscador.
	sRefresh();										// Refresca estructura.
	dRefresh();										// Refresca datos.
	int editIndex = 0;								// Indice del item que se esta editando
	for(continuar = WaitInput; continuar; )
	{
		if(flags.useLocator)
			ch = GetKey();
		else
			ch = ((xBrowseField *) GetItem(editIndex))->Edit(useRow);

		if(ch == keycanc || ch == keyout || (ch == keyenter && flags.useLocator)) continuar = 0;
		else if(ch == keydown)  xBrowseMovePtr(1);
		else if(ch == keyupper) xBrowseMovePtr(-1);
		else if(ch == keyenter)
		{
			if(!flags.useLocator && editIndex < ItemsCount() - 1) 
				editIndex++;
			else
			{
				if(flags.useLocator)
					continuar = false;
				else
				{
					xBrowseMovePtr(1);
					editIndex = 0;
				}
			}
		}
		else if(ch == keyleft)
		{
			if(!flags.useLocator)
			{
				if(editIndex-- == 0) editIndex = ItemsCount() - 1;
			}
			else
			{
				tLen = strlen(FastFind);
				if(tLen)
				{
					FastFind[tLen - 1] = 0;
					if(!db->Seek(FastFind))
						db->Go(view.recno_0);
					dRefresh();
				}
			}
		} 
		else if(ch == keyright)
		{
			if(!flags.useLocator) if(editIndex++ == ItemsCount() - 1) editIndex = 0;
		}
		else if(flags.useLocator && 
				   ((ch >= '0' && ch <= '9') ||
				    (ch >= 'a' && ch <= 'z') ||
					(ch >= 'A' && ch <= 'Z') ||
					(ch == ' ')))
			{
				tLen = strlen(FastFind);
				if(tLen < 78)
				{
					FastFind[tLen] = ch;
					FastFind[tLen + 1] = 0;
					if(!db->Seek(FastFind))
						db->Go(view.recno_0);
					dRefresh();
				}
			}
	}
	if(flags.IDefTheWindow) wRelease(hWnd);
	return ch;
}

//   Para cuando el Animate se emitio en el modo nowait, la siguiente
//   funcion se puede utilizar para hacer que el browse procese
//   teclas (tipo control remoto). Retorna 1 si encontro alguna
//   tecla procesable, sino, 0.
//
int xBrowse::xBrowseRemoteKey(int ch)
{
	int tLen;

	if(ch == keydown) { xBrowseMovePtr(1); return 1; }
	else if(ch == keyupper) { xBrowseMovePtr(-1); return 1; }
	else if(ch == keyleft)
	{
		if(flags.useLocator)
		{
			tLen = strlen(FastFind);
			if(tLen)
			{
				FastFind[tLen - 1] = 0;
				if(!db->Seek(FastFind))
					db->Go(view.recno_0);
				dRefresh();
			}
			return 1;
		}
	} 
	else if(flags.useLocator && 
		   ((ch >= '0' && ch <= '9') ||
		    (ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch == ' ')))
	{
		tLen = strlen(FastFind);
		if(tLen < 78)
		{
			FastFind[tLen] = ch;
			FastFind[tLen + 1] = 0;
			if(!db->Seek(FastFind))
				db->Go(view.recno_0);
			dRefresh();
		}
		return 1;
	}
	return 0;			// No proceso nada.
}

