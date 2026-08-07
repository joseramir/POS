//
//   Elige.c
//   Implementa una lista para elegir parametros.
//
#include <stdafx.h>

char tbuf[100];

#ifdef POSXP
#include "eligeform.h"
using namespace pos;
#else
short chooseEsq[16] = {         // Esquema de colores para choose.
	{ CELESTE * 16 + NEGRO },   // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ CELESTE * 16 + NEGRO },   // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno
	{ ROJO * 16 + BLANCO }      // Color cuando quiere resaltar el texto.
};
#endif

//   La siguiente funcion limpia el contenido de una estructura choose, 
//   sin desdefinir a la misma.
void Choose::Clear()
{
	this->maxAncho = -1;
	this->lines->Clear();
}

//   Constructor. Cada parametro significa:
//      tit,pie: Titulo y pie de la ventana.
//	    adicX, adicY: Espacios en blanco a reservar en eje X o Y. Donde se reservan depende del
//					  estilo: Si es LEFT se reservan a la derecha, si es RIGHT a la izquierda y si
//					  es CENTER de los dos lados.
//		style:  Indica donde se alinean las opciones.
//      AtnFunc: Funcion que se llamara cuando se pulse enterkey.
Choose::Choose(char *tit_, char *pie_, int adicX_, int adicY_, int estilo_,	int (*AtnFunc_)(int))
{
	// Crea la lista de las lineas contenidas.
	this->lines = new ::ArrayList(true, "ChooseCtor");

	// Inicializa las otras variables que dependen de la lista.
	this->AtnFunc = AtnFunc_;
	this->estilo = estilo_;
	this->adicX = adicX_; this->adicY = adicY_;
	this->maxAncho = -1;
	tit = (tit_? STRDUP(tit_, "Choose:ctor1"): NULL); pie = (pie_? STRDUP(pie_, "Choose:ctor2"): NULL);
	pAncho = pAlto = wHand = -1;
}

//   Destructor.
Choose::~Choose()
{
#ifndef POSXP
	if(wHand > 0) wRelease(wHand);
#endif

	delete this->lines;		// Cada item es borrado por el destructor.
	if(tit) FREE(tit);
	if(pie) FREE(pie);
}

//   Retorna el handle de la window utilizada.
#ifndef POSXP
int Choose::WindowHandle()
{
	return this->wHand;
}
#endif

// Retorna la totalidad de opciones definidas.
int Choose::Count()
{
	return this->lines->Count();
}

//   Agrega una linea a la lista de lineas a editar. Refresca la WINDOW si ya esta creada
int Choose::Add(char *msg)
{
	int index = this->lines->Add(STRDUP(msg, "Choose:Add"));
	this->maxAncho = -1;
	if(this->wHand >= 0) this->Refresh();
	return index;
}

//   Eimina una de las strings contenidas.
void Choose::Remove(int cual)
{
	if(cual < 1) return;
	this->lines->RemoveAt(cual - 1);
	this->maxAncho = -1;
	if(this->wHand >= 0) this->Refresh();
}

// Cambia el título de una opción.
void Choose::ChangeTit(int cual, char *newTit)
{
	if(cual < 1 || cual > this->lines->Count())
		return;
	this->lines->SetItem(cual - 1, STRDUP(newTit, "Choose:ChangeTit"));

#ifdef POSXP
	if(StaticChoose::ef != nullptr)
		StaticChoose::ef->SetTitulo(cual - 1, Strings::CharToString(newTit));
#endif
}

char *Choose::GetText(int index)
{
	static char tbuff[100];

#ifdef POSXP
	String ^text = StaticChoose::ef->GetButtonText(index);
	return Strings::StringToChar(text, tbuff, 98);
#else
	return lines->ItemString(index);
#endif
}

void Choose::SetOption(int index, char *t, bool enabled)
{
#ifdef POSXP
	StaticChoose::ef->ChangeButton(index, t, enabled);
#else
	lines->SetItem(index, t);
#endif
}

//   Funcion que anima el elige, displayando la form y todo lo que sea necesario. Retorna el nro
//   de opcion actualmente en display, si se eligio keyenter, sino, retorna 0.
int Choose::Anima()
{
	int ch = -1;

	this->actual = 1;				// Posicion actualmente seleccionada.
	this->Refresh();				// Construye la version visual.

#ifdef POSXP

	StaticChoose::ef = gcnew EligeForm(this->tit, this->pie, this->AtnFunc);
	this->Refresh();
	StaticChoose::ef->ShowDialog();
	int res = (StaticChoose::ef->DialogResult != System::Windows::Forms::DialogResult::OK);
	this->actual = StaticChoose::ef->Actual;
	if(res)
		return 0;

#else

	DisplayLine(this->actual);		// Muestra seleccionada la linea actual.
	while(true)
	{
		ch = GetKey();
		if(ch == keydown)
		{
			if(this->actual < this->pAlto)
			{
				int nActual = this->actual + 1;
				this->actual = -1;
				DisplayLine(nActual - 1);		// Pone color normal la anterior.
				this->actual = nActual;
				DisplayLine(this->actual);		// Resalta la nueva linea seleccionada.
			}
			continue;
		}
		if(ch == keyupper)
		{
			if(this->actual > 1)
			{
				int nActual = this->actual - 1;
				this->actual = -1;
				DisplayLine(nActual + 1);		// Pone color normal la anterior.
				this->actual = nActual;
				DisplayLine(this->actual);		// Resalta la nueva linea seleccionada.
			}
			continue;
		}
		if(ch == keyenter)
		{
			if(this->AtnFunc != NULL)
			{
				if((*this->AtnFunc)(this->actual) != 0)
					break;
			}
			else
				break;
			continue;
		}
		if(ch == keycanc)
			break;
	}
	wDeactivate(this->wHand, 0);
	if(ch == keycanc) return 0;
#endif

	return this->actual + 1;
}

#ifndef POSXP
//   Funcion que displaya una linea en la posicion que le toca. Si la linea a mostrar es la
//   'actual', se utiliza el esquema 'resaltado'.
void Choose::DisplayLine(int cual)
{
	int j, pcolor;
	char *p;

	if(cual > this->lines->Count())
	 {
		for(j = this->pAncho, p = tbuf; j; j--)
			*p++ = ' ';
		*p = 0;
	 }
	 else
		 STRCPY(tbuf, (char *) this->lines->Item(cual - 1));

	//  Alinea el texto.
	switch(this->estilo)
	{
		case CHOOSE_ALIGN_LEFT:         // Alineado a la izquierda.
			 while(strlen(tbuf) < (unsigned) this->pAncho)
				 STRCAT(tbuf, " ");
			 break;

		case CHOOSE_ALIGN_CENTER:       // Centrado.
			 for(j = (this->pAncho - strlen(tbuf)) / 2; j; j--)
			 {
				memmove(tbuf+1, tbuf, strlen(tbuf) + 1);
				tbuf[0] = ' ';
			 }
			 while(strlen(tbuf) < (unsigned) this->pAncho)
				STRCAT(tbuf, " ");
			 break;

		case CHOOSE_ALIGN_RIGHT:       // Alineado a la derecha.
			for(j = (this->pAncho - strlen(tbuf)) - 1; j; j--)
			{
				memmove(tbuf+1, tbuf, strlen(tbuf) + 1);
				tbuf[0] = ' ';
			}
			break;
	}

	wLocate(cual, 0);
	if(cual == this->actual)
	{
		pcolor = wSetColor(chooseEsq[5]);
		wPch('>');
	}
	else
		wPch(' ');
	
	wPstr(tbuf);
	if(cual == this->actual)
	{
		wPch('<');
		wSetColor(pcolor);
	}
	else
		wPch(' ');
}

//   Retorna el maximo ancho entre todas las opciones
int Choose::MaxAncho()
{
	if(this->maxAncho >= 0) return this->maxAncho;
	maxAncho = 0;
	for(int i=0; i<this->lines->Count(); i++)
		if(strlen(this->lines->ItemString(i)) > (unsigned) maxAncho)
			maxAncho = strlen(this->lines->ItemString(i));
	return maxAncho;
}
#endif

//   Refresca la ventana de un choose.
void Choose::Refresh()
{
#ifdef POSXP
	if(StaticChoose::ef != nullptr)
	{
		EligeForm ^ef = StaticChoose::ef;
		for(int i=0; i < this->lines->Count(); i++)
			ef->AgregarRenglon(this->lines->ItemString(i));
	}
#else
	int i;

	//   Calcula los nuevos altos y anchos para determinar la necesidad de redefinir la window.
	int uAlto = this->lines->Count(), uAncho = -1;
	for(i=0; i<uAlto; i++)
	{
		int tAncho = strlen((char *) this->lines->Item(i));
		if(tAncho > uAncho) uAncho = tAncho;
	}

	//   Si todavia no lo hizo, define (o redefine) la ventana a utilizar.
	if(this->wHand < 0 || this->pAncho != uAncho || this->pAlto != uAlto)
	{
		int uy, ux;
		if(this->wHand >= 0) { wRelease(this->wHand); }
		uy = (25 - (uAlto  + this->adicY + 2)) / 2;
		ux = (80 - (uAncho + this->adicX + 2)) / 2;
		this->wHand = wDefine(uy, ux, uy + uAlto + this->adicY + 3, ux + uAncho + this->adicX + 3,
							BORDERSINGLE, SNGCURS, chooseEsq, this->tit, this->pie);
		this->pAncho = uAncho; this->pAlto = uAlto;
		wActivate(this->wHand);
		if(this->AtnFunc != NULL) (*this->AtnFunc)(-1);
	}

	wActivate(this->wHand);
	for(i = 1; i <= this->pAlto; i++) 
		DisplayLine(i);
	if(this->AtnFunc != NULL) (*this->AtnFunc)(-2);
#endif
}
