//
//   Aget.cpp
//   Implementa la funcion arroba get...
//

#include "stdafx.h"

// Constructor de la ventana de edicion.
EditWindow::EditWindow(int hWin)
{
	animaWind = hWin;
	first = focus = NULL;
}

// Destructor de una ventana de edicion.
EditWindow::~EditWindow()
{
	ClearGets();
}

//  Instala un nuevo objeto, linkeado con un campo.
aget *EditWindow::AddGet(int _y, int _x, char *_fld, dbf *_alias, void *_bag, int (*_when)(char *, void *), int (*_valid)(char *, void *))
{
	// Inicial
	memset(this, 0, sizeof(*this));
	aget *xget = new aget(this, _y, _x, _fld, _alias, _bag, _when, _valid);

	//   Engancha en la estructura de campos.
	aget *busca;
	for(busca = first; busca && busca->next; busca = busca->next)
		;
	if(busca)
		busca->next = xget;
	else
		first = xget;
	return xget;
}

//  Instala un nuevo objeto normal, linkeado a un buffer en memoria.
aget *EditWindow::AddGet(int _y, int _x, int _frmtflag, int _len, int _dec, char *_buff, 
						void *_bag,
						int (*_when)(char *, void *), int (*_valid)(char *, void *))
{
	// Inicial
	aget *xget = new aget(this, _y, _x, _frmtflag, _len, _dec, _buff, _bag, _when, _valid);

	//   Engancha en la estructura de campos.
	aget *busca;
	for(busca = first; busca && busca->next; busca = busca->next)
		;
	if(busca)
		busca->next = xget;
	else
		first = xget;
	return xget;
}

//   Anima los gets colocados en la pantalla.
int EditWindow::AnimaGets(int InfiniteLoop)
{
	int xKey;
	aget *busca;

	if(!first) return 0;			// DEBE haber elementos definidos.
	animaWind = wact;				// Ventana en la que edita.
	focus = first;
	while(1)
	{
		xKey = focus->anima();
		if(xKey == 0) continue;
		if(xKey == keycanc || xKey == keyout)
			break;

		if(!xKey || xKey == keyenter || xKey == keydown)
		{
			while(true)
			{
				if(focus->next)
					focus =  focus->next;
				else
				{
					if(InfiniteLoop)
						focus = first;
					else
						break;
				}
				if(focus->flags.enabled || focus->flags.isButton) break;
			}
			continue;
		}
		if(xKey == keyupper)
		{
			while(true)
			{
				if(first == focus)
					for(busca = first; busca->next; busca = busca->next)
						;
				else
					for(busca = first; busca->next != focus; busca=busca->next)
						;
				focus = busca;
				if(focus->flags.enabled || focus->flags.isButton) break;
			}
			continue;
		}
	}
	animaWind = 0;
	ClearGets();
	return (xKey == keycanc)? 0: 1;
}

//   Reactualiza la representacion de TODOS los campos en pantalla.
void EditWindow::ShowGets()
{
	aget *svFocus, *busca;

	svFocus = focus; focus = 0;
	for(busca = first; busca; busca = busca->next)
		busca->show();
	focus = svFocus;
}

//   Limpia todas las estructuras de pantalla.
void EditWindow::ClearGets()
{
	aget *busca, *nxt;

	for(busca = first; busca; busca = nxt)
	{
		nxt = busca->next;
		delete busca;
	}
	focus = first = NULL;
}






//  El destructor remueve la estructura si corresponde.
aget::~aget()
{
	if(flags.BuffFree)
	{
		FREE(buff);
		FREE(tbuff);
	}
}

// Constructor de un get conectado con un campo en base de datos.
aget::aget(EditWindow *_container, int _y, int _x, char *_fld, dbf *_alias, 
		   void *_bag, 
		   int (*_when)(char *, void *), 
		   int (*_valid)(char *, void *))
{
	// Limpia toda la posible memoria en uso.
	memset(this, 0, sizeof(*this));

	y = _y; 
	x = _x; 
	bag = _bag;
	when = _when; 
	valid = _valid;
	alias = _alias; 
	fld = _fld;
	len = alias->FieldLen(fld); dec = 0;
	container = _container;
	buff = (char *) MALLOC(len + 5, "aget:ctor1");
	tbuff = (char *) MALLOC(len + 5, "aget:ctor2");
	switch(alias->FieldType(fld))
	{
		case 'C':
			frmtflag = TEXT_LEFT_ALIGNED;
			break;
		case 'N':
			frmtflag = NUMBER_RIGHT_ALIGNED;
			break;
		case 'L':
			frmtflag = LOGICAL_VALUE;
			break;
	}


	// Actualiza la presentacion.
	next = 0;
	flags.BuffFree = flags.fldlnk = true;
	show();
}

// Constructor de un get conectado con un buffer comun.
aget::aget(EditWindow *_container, int _y, int _x, int _frmtflag, int _len, int _dec, char *_buff, 
		   void *_bag, 
		   int (*_when)(char *, void *), 
		   int (*_valid)(char *, void *))
{
	//   Inicializa los miembros.
	memset(this, 0, sizeof(*this));
	y = _y; x = _x; frmtflag = _frmtflag; len = _len; dec = _dec;
	buff = _buff; when = _when; valid = _valid;
	bag = _bag;
	container = _container;
	flags.enabled = (_frmtflag != TEXT_READ_ONLY);

	// Actualiza algunas variables, si se trata de un color picker.
	flags.cPicker = 0;
	if(_frmtflag == MAKE_COLOR_PICKER)
	{
		flags.cPicker = 1;
		pickColor = atoi(buff);
	}

	// Actualiza la presentacion en pantalla.
	next = 0;
	flags.BuffFree = flags.fldlnk = 0;
	show();
}


//   Imprime una string como una mascara para password.
void wPstrPasswd(char *buff)
{
	int iMask, len;

	len = strlen(buff);
	for(iMask = 0; iMask < len; iMask++)
		if(buff[iMask] == ' ')
			wPch(' ');
		else
			wPch('*');
}

//  Displaya o actualiza la visualizacion de un objeto.
void aget::show()
{
	int i;
	short prev;

	//   Si esta enganchado a campo, trae la informacion.
	if(flags.fldlnk)
	{
		alias->Get(fld, buff);
		buff[len] = 0;
		if(frmtflag == LOGICAL_VALUE)
			buff[0] = (buff[0] == 'T')? 'S': 'N';
	}

	//   Prepara el buffer para editarlo.
	wActivate(container->Window());
	switch(frmtflag)
	{
		case MAKE_COLOR_PICKER:
			wLocate(y, x);
			wEsqSetColor((container->Focus() == this)? COLFOCUS: COLNOFOCUS);
			wPch('[');
			prev = wSetColor(pickColor);
			wPch('X');
			wSetColor(prev);
			wPch(']');
			return;

		case PASSWORD_MASK:
		case NUMBER_LEFT_ALIGNED:
		case TEXT_LEFT_ALIGNED:
		case TEXT_READ_ONLY:
			while((int) strlen(buff) < len)
				STRCAT(buff, " ");
			buff[len] = 0;
			break;

		case MONEDA_RIGHT_ALIGNED:
		case NUMBER_RIGHT_ALIGNED:
			if(frmtflag == MONEDA_RIGHT_ALIGNED) 
				sprintf(buff, "%.2lf", atof_(buff));
			
			Trim(buff);
			i = len - strlen(buff);
			if(i > 0)
			{
				if(len - i) memmove(buff + i, buff, len - i + 1);
				memset(buff, ' ', i);
				buff[len] = 0;
			}
			else if(i < 0)
				buff[len] = 0;
			break;
	}

	//   Salva el buffer, si era campo.
	if(flags.fldlnk)
		STRCPY(tbuff, buff);

	//   Displaya el contenido del buffer.
	wLocate(y, x);
	wEsqSetColor((container->Focus() == this)? COLFOCUS: COLNOFOCUS);
	if(frmtflag == PASSWORD_MASK)
		wPstrPasswd(buff);
	else
		wPstr(buff);
	wLocate(y, x);
}

//   Anima la edicion de un campo en particular.
int xKey;
int aget::anima()
{
	int e_x, e_pos, ValidRes, res, i;
	aget *pFocus;
	char *p;

	if(when)
	{
		res = (*when)(buff, NULL);
		wActivate(container->Window());
		if(!res) return 0;
	}

#ifdef AGETDB
	sprintf(msg, "ANIMA - keyenter:%d, keycanc:%d, keyupper:%d, keydown:%d, keyout:%d",
		keycanc, keyupper, keydown, keyout);
	WriteBootLog(msg);
	sprintf(msg, "      - keyback:%d, keyright:%d, keyleft:%d", keyback, keyright, keyleft);
#endif

	show();								// Redisplaya en otro color.
	e_x = x; e_pos = 0;					// Posicion dentro de la edicion.
	flags.clrNext = 1;					// Selecciona todo el campo.
	while(1)
	{
		xKey = GetTransformedKey();

		//   Se trata de un 'color picker' y se pulso enter?
		if(xKey == keyenter && flags.cPicker)
			pickColor = MakeColorPicker(pickColor);

		// Sale o cancela??
		wActivate(container->Window());
		if(xKey == keycanc  || xKey == keyenter ||
		   xKey == keyupper || xKey == keydown  ||
		   xKey == keyout   || xKey == 0x0d)
		{
			// Determina si corresponde efectuar la validacion
			bool validar = (valid != NULL);
			if(flags.isButton)
			{
				if(xKey != keyenter) validar = false;
			}
			else if(xKey == keycanc) validar = false;

			// Efectua la validacion.
			ValidRes = 1;
			if(validar)
			{
				ValidRes = (*valid)(buff, bag);
				if(frmtflag == TEXT_READ_ONLY)
				{
					if(ValidRes && xKey == keyenter) xKey = keydown;
				}
				wActivate(container->Window());
			}

			// Si la validacion salio OK y estamos enganchados a una tabla, actualizamos el campo de la tabla.
			if(ValidRes && flags.fldlnk)
			{
				if(strcmp(buff, tbuff))
				{
					if(frmtflag == LOGICAL_VALUE)
						buff[0] = (buff[0] == 'S')? 'T': 'F';
					else
					{
						if(frmtflag == NUMBER_RIGHT_ALIGNED || frmtflag == NUMBER_LEFT_ALIGNED)
						{
							for(p = buff; *p && *p == ' '; p++)
								;
							for(; *p && *p != ' '; p++)
								;
							*p = 0;
						}
						if(frmtflag == NUMBER_RIGHT_ALIGNED)
						{
							Trim(buff);
							i = len - strlen(buff);
							if(i > 0)
							{
								if(len - i)
									memmove(buff + i, buff, len - i);
								memset(buff, ' ', i);
							}
							else if(i < 0)
								buff[len] = 0;
						}
					}
					alias->Replace(fld, buff);
				}
			}

			// Si la validacion salio OK y somos un "color picker", obtenemos el color
			if(ValidRes && flags.cPicker)
			{
				if(xKey != keycanc)
					_itoa(pickColor, buff, 10);
				 else
					pickColor = atoi(buff);
			}

			// Si el resultado de la validacion salio OK y la tecla es ESC o F8, retornamos.
			if(ValidRes || (xKey == keycanc))
			{
				pFocus = container->Focus(); 
				container->SetFocus(NULL);
				show();
				container->SetFocus(pFocus);
				return xKey;
			}
		}

		// Tecla de borrado?
		if(!flags.cPicker && (xKey == keyback))
		{
			if(e_pos > 0)
			{
				memmove(&buff[e_pos - 1], &buff[e_pos], len - e_pos);
				buff[len-1] = ' ';
				wLocate(y, x); 
				if(frmtflag == PASSWORD_MASK)
					wPstrPasswd(buff);
				else
					wPstr(buff);
				wLocate(y, --e_x);
				e_pos--;
			}
			continue;
		}

		//  Mueve a la derecha?
		if(!flags.cPicker && (xKey == keyright))
		{
			if(e_pos < len)
			{
				e_pos++;
				wLocate(y, ++e_x);
			}
			flags.clrNext = 0;
			continue;
		}

		//  Mueve a la izquierda?
		if(!flags.cPicker && (xKey == keyleft))
		{
			if(e_pos)
			{
				e_pos--;
				wLocate(y, --e_x);
			}
			flags.clrNext = 0;
			continue;
		}
		
		//  Esta activado el borrado inicial?
		if(!flags.cPicker && (flags.clrNext))
		{
			memset(buff, ' ', len);
			e_pos = 0;
			e_x = x;
		    wEsqSetColor(COLFOCUS);
			wLocate(y, x); 
			wPstr(buff);
		}

		//  Hay lugar para editar?
		if(!flags.cPicker && (e_pos >= len)) continue;

		//  Valida el caracter ingresado.
		if(!flags.cPicker)
		{
			switch(frmtflag)
			{
				case TEXT_READ_ONLY:
					continue;

				case TEXT_LEFT_ALIGNED:
					break;

				case NUMBER_RIGHT_ALIGNED:
				case NUMBER_LEFT_ALIGNED:
					if(xKey != ' ' && xKey != '.' && (xKey < '0' || xKey > '9'))
						continue;
					break;

				case LOGICAL_VALUE:
					if(xKey != 'S' && xKey != 'N')
						continue;
					break;
			}

			buff[e_pos++] = xKey;
			wEsqSetColor(COLFOCUS);
			wLocate(y, e_x++); 
			if(frmtflag == PASSWORD_MASK)
				wPch('*');
			else
				wPch(xKey);
			flags.clrNext = 0;
		}
	}
}

