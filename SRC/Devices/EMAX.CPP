/*
 *   Emax.C
 *   Maneja el visor de Cliente.
*/

#include <stdafx.h>

#ifdef POSXP
#include "MainForm.h"
#endif

//   Distintos tipos de visor de cliente definidos.
#define EMAX_VGA    1
#define EMAX_SERIAL 2
#define EMAX_NONE   3

int cliente;             // Handler de la ventana.
char l1[50];
char l2[50];
char l3[50];
int cdigEmax = 20;
long DelayEmax;

//   Algunas string de control.
char emax_cls[] = "\x0e\x0c\x16";   // 0E=Cls, 0C=Cursor HOME, 16=Cursor Invisible
char emax_crlf[] = "\r\n";

//   La siguiente funcion emite una string a un visor emax.
void WriteEmaxChar(char ch)
{
    double d;

    txChar(COM_EMAX, ch);
    for(d = (double) DelayEmax; d > 0;  d -= 1)
        ;
}
void WriteEmaxStr(char *s)
{
    while(*s)
        WriteEmaxChar(*s++);
}

void PatchLine(char *dst, char *izq, char *der)
{
	char tIzq[100], tDer[100];

	STRCPY(dst, "                                        ");
	dst[cdigEmax] = 0;
	Trim(STRCPY(tIzq, izq));
	Trim(STRCPY(tDer, der));
	if((int)strlen(tIzq) > cdigEmax) tIzq[cdigEmax] = 0;
	if((int)strlen(tDer) > cdigEmax) tDer[cdigEmax] = 0;

	memcpy(dst, tIzq, strlen(tIzq));
	memcpy(dst + cdigEmax - strlen(tDer), tDer, strlen(tDer));
}

void WriteToEmax(char *tl1, char *tl2)
{
	switch(emax)
	{
		case EMAX_VGA:
#ifndef POSXP
			 wActivate(cliente);
			 wCls();
             wLocate(0, 1);
			 wPstr(tl1);
             wLocate(1, 1);
			 wPstr(tl2);
#endif
			 break;

		case EMAX_SERIAL:
             WriteEmaxStr("\x1bQA");
			 WriteEmaxStr(tl1);
			 WriteEmaxChar('\r');

             WriteEmaxStr("\x1bQB");
			 WriteEmaxStr(tl2);
			 WriteEmaxChar('\r');
			 break;

         case EMAX_NONE:
             break;
	}
}

void CustDisp4(char *ul, char *ur, char *dl, char *dr)
{
	char tl1[100], tl2[100];

	PatchLine(tl1, ul, ur);
	PatchLine(tl2, dl, dr);

	WriteToEmax(tl1, tl2);

#ifdef POSXP
	MainForm::F->SetMultiDisplay(dl);
	MainForm::F->SetNumDisplay(dr);
//	if(strlen(ul) != 0)
//		MainForm::F->SetTotDisplay(ul, "", "", "", 0);
#endif
}

//   La siguiente funcion displayea dos strings en el visor de cliente.
void CustDisp(char *Ul1, char *Ul2)
{
	char tl1[100], tl2[100];
	PatchLine(tl1, Ul1, "");
	PatchLine(tl2, "", Ul2);

	WriteToEmax(tl1, tl2);

#ifdef POSXP
	MainForm::F->SetMultiDisplay(Ul1);
	MainForm::F->SetNumDisplay(Ul2);
#endif
}

/*
 *   Combina 3 mensajes en el visor del cliente.
*/

void CustDisp3(char *Ul1, char *Ul2, char *Ul3)
{
	char tl1[100], tl2[100];

	PatchLine(tl1, Ul1, "");
	PatchLine(tl2, Ul2, Ul3);

	WriteToEmax(tl1, tl2);

#ifdef POSXP
	MainForm::F->SetMultiDisplay(Ul1);
	MainForm::F->SetNumDisplay(Ul3);
	if(strlen(Ul2) != 0)
		MainForm::F->SetTotDisplay(Ul2, "", "", "", 0);
#endif
}

void DisplayEstadoCaja()
{
	 char *s1, *s2;
	 if(ckaNro || !cko_enab)
	 {
		 s1 = "Bienvenido";
		 s2 = "Caja Habilitada";
	 }
	 else
	 {
		 s1 = "Caja Cerrada";
		 s2 = "";
	 }
	 CustDisp(s1, s2);
}

/*
 *   Rutina de inicializacion del visor
*/

int InitEmax()
{
	switch(emax)
	{
		case EMAX_VGA:
			cliente = wDefine(21, 0, 24, 24, BORDERSINGLE, SNGCURS, 0, " Cliente ", "");
			break;
	}
	DisplayEstadoCaja();
	return 1;
}

/*
 *   Libera el visor.
*/

void DeInitEmax()
{
	switch(emax)
	{
		case EMAX_VGA:
 			 wDeactivate(cliente, 0);
			 break;

		case EMAX_SERIAL:
			 CustDisp("Caja Cerrada", "");
			 break;

        case EMAX_NONE:
             break;
	}
}
