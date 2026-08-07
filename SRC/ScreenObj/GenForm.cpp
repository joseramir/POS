#include "stdafx.h"

#ifdef POSXP
#include "GenericForm.h"
#else

//  esquema de colores para las forms genericas
short genformEsq[16] = {          // Esquema de colores para choose.
	{ BLANCO * 16 + NEGRO },      // Color del borde
	{ BLANCO * 16 + NEGRO },      // Titulo a ventana activa.
	{ AZUL   * 16 + BLANCO },     // Titulo a ventana inactiva.
	{ BLANCO * 16 + NEGRO },      // Texto contenido.
	{ NEGRO  * 16 + BLANCO },     // Color de Fondo cuando usa relleno
	{ ROJO   * 16 + BLANCO }      // Color cuando quiere resaltar el texto.
};

#endif

char *GenForm::VerTitulo() 
{ 
	if(this->titulo == NULL) return "";
	return this->titulo; 
}

void GenForm::CambiarTitulo(char *tit)
{
	if(titulo != NULL)
	{
		if(tit != NULL && (strlen(titulo) >= strlen(tit)))
		{
			STRCPY(titulo, tit);
			return;
		}
		FREE(titulo);
	}
	titulo = (tit == NULL)? NULL: STRDUP(tit, "GenForm:CambiarTitulo");
}

GenForm::GenForm(char *tit)
{
	titulo = (tit != NULL)? STRDUP(tit, "GenForm:ctor"): NULL;
	items = new ::ArrayList(true, "GenFormCtor");
	fontSize = 0;
}

GenForm::~GenForm()
{
	delete items;
	if(titulo != NULL) FREE(titulo);
}

char *EditItem::VerLabel()
{
	return label;
}

void EditItem::CambiarLabel(char *_label)
{
	STRCPY(label, _label);
}

EditItem::EditItem(char *_label)
{
	STRCPY(label, _label);
	Trim(label);
	STRCPY(dato, "");
	STRCPY(help, "");
	validFunc = NULL;
	dataType = TIPO_CUALQUIERA;
	valido = false;
	enabled = true;
	ancho = 10;
}

EditItem::~EditItem()
{
}

bool ValidateFormat(char *d, char *fmt)
{
	while(*d)
	{
		if(strchr(fmt, *d) == 0)
			return false;
		d++;
	}
	return true;
}

int EditItem::Medir()
{
	int lenLabel = strlen(this->label) + 2;
	return lenLabel + ancho;

}

char *EditItem::ValidarDatoIngresado(char *_nueDato, bool cambiar)
{
	char *res = NULL, *p;
	char tmsg[65];
	System::String^ aux;

	if(validFunc != NULL)
	{
		STRCPY(tmsg, _nueDato);
		res = validFunc(this, tmsg);
	}
	else
	{
		switch(dataType)
		{
			case TIPO_ENTERO:
				if(!ValidateFormat(_nueDato, "0123456789 "))
					res = "Esperaba un numero entero. Ingreselo de nuevo";
				else
					sprintf(tmsg, "%d", atol(_nueDato));
				break;

			case TIPO_MONEDA:				
				if(!ValidateFormat(_nueDato, "0123456789. "))
					res = "Esperaba un monto en pesos. Ingreselo de nuevo";
				else
				{
					aux = gcnew System::String(_nueDato);
					if (aux->Trim()->Length < 8)
						sprintf(tmsg, "%.2lf", atof(_nueDato));
					else
						res = "Monto muy grande o mal ingresado. Ingreselo de nuevo";
					delete aux;
				}
				break;

			case TIPO_NUMREF:
				if(!ValidateFormat(_nueDato, "0123456789 .-"))
					res = "Esperaba un numero de comprobante. Pruebe de nuevo";
				else
				{
					STRCPY(tmsg, _nueDato);
					Trim(tmsg);
					for(p = tmsg; *p; p++)
						if(*p == '.')
							*p = '-';
				}
				break;

			default:
				STRCPY(tmsg, _nueDato);
				Trim(tmsg);
				break;
		}
	}
	
	if(res == NULL)
	{
		this->valido = true;
		if(cambiar) STRCPY(dato, tmsg);
	}
	else
		this->valido = false;

	return res;
}

int ValidarMSDOS(char *nval, void *bag)
{
	EditItem *ei = (EditItem *)bag;
	char *errMsg = ei->ValidarDatoIngresado(nval, true);
	if(errMsg != NULL) Alert(errMsg, "");
	return (errMsg == NULL);
}

void GenForm::Refresh()
{
	ew->ShowGets();
}

bool GenForm::Animar()
{
#ifdef POSXP

	// Usa una form manejada.
	GenericForm ^gf = gcnew GenericForm(this);
	gf->CambiarFont("Tahoma",
		(fontSize == FONT_GRANDE)? 11 :
		(fontSize == FONT_MEDIANO)? 10 :
		9);
	gf->ShowDialog();
	return (gf->DialogResult == System::Windows::Forms::DialogResult::OK);

#else

	// Mide los elementos para determinar el ancho de ventana necesario.
	int ancho = strlen(this->VerTitulo()) + 1, xGet = 1;
	if(ancho < 32) ancho = 30;
	for(int i = this->items->Count() - 1; i >= 0; i--)
	{
		EditItem *ei = (EditItem *)this->items->Item(i); 
		int nxGet = strlen(ei->VerLabel());
		if(nxGet > xGet) xGet = nxGet;

		int nAncho = ei->VerAncho() + 5;
		if(nAncho > ancho) ancho = nAncho;
	}
	ancho += xGet;

	// Define la ventana a usar.
	ancho += 2;
	int alto = items->Count() + 5;
	int y = (25 - alto) / 2;
	int x = (80 - ancho) / 2;
	int hWin = wDefine(y, x, y + alto, x + ancho, BORDERSINGLE, SNGCURS, genformEsq, titulo, " ESC=Cancela, F8=Confirma ");
	wActivate(hWin);

	ew = new EditWindow(hWin);
	y = 1;
	for(int i = 0; i < items->Count(); i++)
	{
		EditItem *ei = (EditItem *)this->items->Item(i);
		int frmtFlag, boxLen = ei->VerAncho(), boxDec = 0;
		if(!ei->enabled)
			frmtFlag = TEXT_READ_ONLY;
		else
		{
			switch(ei->VerTipo())
			{
				case TIPO_NUMREF:
				case TIPO_ENTERO:
					frmtFlag = NUMBER_RIGHT_ALIGNED;
					break;

				case TIPO_MONEDA:
					frmtFlag = MONEDA_RIGHT_ALIGNED;
					boxDec = 2;
					break;

				default:
					frmtFlag = TEXT_LEFT_ALIGNED;
					break;
			}
		}

		y++;
		wLocate(y, 2 + xGet - strlen(ei->VerLabel())); 
		wSetColor(genformEsq[3]);
		wPstr(ei->VerLabel()); wPch(':');
		ew->AddGet(y, xGet + 4, frmtFlag, boxLen, boxDec, ei->dato, ei, NULL, ValidarMSDOS);
	}

	int res = ew->AnimaGets(1);

	wRelease(hWin);
	delete ew;
	return (res != 0);
#endif
}