/*
 *  Alert.c
 *  Muestra ventanas de Alerta en la pantalla. Tambien, hace sonar la
 *  chicharra, etc.
*/

#include <stdafx.h>

#ifdef POSXP
#include "AlertForm.h"
using namespace pos;
#endif

#ifdef PANTACOLOR
short alrtesq[16] = {           // Esquema default de colores
	{ ROJO * 16 + BLANCO },     // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ ROJO * 16 + VERDE }       // Texto contenido.
};
#else
short alrtesq[16] = {           // Esquema default de colores
	{ BLANCO * 16 + NEGRO },    // Color del borde
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ BLANCO * 16 + NEGRO }     // Texto contenido.
};
#endif
int aHand, sHand, mHand;

// Saca una ventana conteniendo un par de mensajes de ALERTA.
void Alert(char *c1, char *c2, bool enableBeep)
{
#ifdef POSXP
	AlertForm ^af = gcnew AlertForm(c1, c2, false, true, enableBeep);
	af->ShowDialog();	
	System::Windows::Forms::Application::DoEvents();
	FlushAll();
#else
	int l1, l2;

	l1 = strlen(c1); l2 = strlen(c2);
	if(!aHand)
		aHand = wDefine(10, 10, 15, 70, BORDERSINGLE, SNGCURS, alrtesq,
				   " Atencion ", " Pulse <CLEAR> para seguir ");

	wActivate(aHand);
	wCls();
	wLocate(1, (l1 >= 60)? 1: (60-l1)/2);
	wPstr(c1);
	if(l2 > 0)
	{
		wLocate(2, (l2 >= 60)? 1: (60-l2)/2);
		wPstr(c2);
	}

	for( ;; )
		if(GetKey() == keycanc)
			break;

	wDeactivate(aHand, 0);
#endif
	if (inOper < 1)
		clear(1);
}

void Alert(char *c1, char *c2)
{
	Alert(c1, c2, true);
}


//   Abre una ventana con un par de mensajes (normalmente, mensajes de orientación)
//   y la deja abierta hasta que se llama a la función 'ClearMsg'
void SendMsg(char *c1, char *c2)
{
#ifdef POSXP
	StaticAlerts::af = gcnew AlertForm(c1, c2, false, false, false);
	StaticAlerts::af->Show();	
	System::Windows::Forms::Application::DoEvents();
#else
	int l1, l2;

	l1 = strlen(c1); l2 = strlen(c2);
	if(!mHand)
		mHand = wDefine(10, 10, 15, 70, BORDERSINGLE, SNGCURS, alrtesq,
				   " Atencion ", " Pulse <ESC> para seguir ");

	wActivate(mHand);
	wCls();
	wLocate(1, (l1 >= 60)? 1: (60-l1)/2);
	wPstr(c1);
	if(l2 > 0)
	{
		wLocate(2, (l2 >= 60)? 1: (60-l2)/2);
		wPstr(c2);
	}
#endif
}

void ClearMsg()
{
#ifdef POSXP
	if(StaticAlerts::af != nullptr)
	{
		StaticAlerts::af->AlertClose();
		StaticAlerts::af = nullptr;
		FlushAll();
	}
#else
	wDeactivate(mHand, 0);
	HClear();
#endif
}

int ClearMsg2()
{
	int estaCancelado = 1;
	if(StaticAlerts::af != nullptr)
	{
		StaticAlerts::af->AlertClose();
		if(StaticAlerts::af->Cancelo) 
			estaCancelado = 0;
		StaticAlerts::af = nullptr;
		FlushAll();
	}	
	return estaCancelado;
}


//   Abre una pequeña ventana solicitando una confirmación. Devuelve 1 si se eligió
//   SI y 0 en caso contrario.
int GetSiNo(char *c1, char *c2, bool enableBeep)
{
#ifdef POSXP
	AlertForm ^af = gcnew AlertForm(c1, c2, true, false, enableBeep);
	af->ShowDialog();
	//Application::Run(af);
	FlushAll();
	if(af->Cancelo) return 0;
	return 1;
#else
	int l1, l2;
	int ch;

	l1 = strlen(c1); l2 = strlen(c2);
	if(!sHand)
		sHand = wDefine(9, 10, 16, 70, BORDERSINGLE, SNGCURS, alrtesq,
				   " Escoja SI o NO ", "");

	wActivate(sHand);
	wCls();
	wLocate(1, (l1 >= 60)? 1: (60-l1)/2);
	wPstr(c1);
	if(l2 > 0)
	{
		wLocate(2, (l2 >= 60)? 1: (60-l2)/2);
		wPstr(c2);
	}
	wLocate(4, 6);
	wPstr("Pulse <ENTER> si es SI - Pulse <ESC> si es NO");

	for( ;; )
	{
		ch = GetKey();
		if(ch == keyenter || ch == keycanc)
			break;
	}

	wDeactivate(sHand, 0);
	clear(1);
	if(ch == keyenter) return 1;
	return 0;
#endif
}

int GetSiNo(char *c1, char *c2)
{
	return GetSiNo(c1, c2, true);
}

//   La siguiente funcion se utiliza cuando es requerida la autorizacion
//   del supervisor, para efectuar alguna funcion. Retorna 0 si esta habi-
//   litado, 1 si no lo esta.
//   Si estubiera en modo supervisor, retorna 1 sino retorna 0.
int RequestSuperv()
{
	if(rMode != 1)
	{
		Alert("Necesito AUTORIZACION para la operacion pedida !!!",
			  "Llame al supervisor por favor.");
		return 1;
	}
	return 0;
}

