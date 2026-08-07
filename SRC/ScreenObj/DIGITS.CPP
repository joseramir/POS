/*
 *   Digits.c
 *   Procesa entradas general de teclado.
 *   Las funciones de este modulo son despachadas desde 'kbd.c'.
*/

#include <stdafx.h>

#ifdef POSXP
#include "MainForm.h"
#include "StaticDump.h"
#endif

int usaDec = 2;
double maxDisplay;
extern bool zapearTicketTemp;			// Dump.cpp
extern bool clrCustomerDisplay;			// Mpago.cpp

//   Algunos Prototipos.
void HDecim(void);
char *HRng(double);

//   Al deprimir la tecla de multiplicacion, mueve xReg a yReg.
void multipli(int dummy)
{
	if(yReg != 0.0 || xReg == 0.0)
		return;

	usaDec = 3;             // Default en cantidades.
	if(!cDecim) cDecim=usaDec;
	HDecim();               // Homologa a xReg.
	yReg = xReg;

	usaDec = opcdec;        // Decimales standard para importes.
	xReg = 0;
	cDecim = 0;

	sprintf(msg, "Cantidad= %s", Trim(HRng(yReg)));
#ifdef POSXP
	MainForm::F->SetMultiDisplay(msg);
#else
	DispAdic(1, 1, msg);
#endif
}

//   Procesa un digito numerico comun.
void digitos(int cual)
{
	double pote;
	int i;

	if(cual == -1)				// Borra un digito!!1
	{
		char fmt[50], xStr[50];
		if(cDecim)
		{
			sprintf(fmt, "%%.%dlf", cDecim - 1);
			cDecim--;
		}
		else
			STRCPY(fmt, "%.0lf");

		sprintf(xStr, fmt, xReg);
		xStr[strlen(xStr)-1] = 0;
		xReg = atof(xStr);
		DispCash(xReg, (cDecim > 0)? cDecim - 1: 0);
	}
	else
	{
		if(maxDisplay == 0.0)
		{
			for(maxDisplay = 9.0, i = 12; i; i--)
				maxDisplay = maxDisplay * 10 + 9;
		}

		for(pote=1.0, i=cDecim; i; i--, pote *= 10.0)
					;
		if((xReg * pote) < maxDisplay)
		{
			if(cDecim)
				xReg = xReg + ((double) cual / pote);
			else
				xReg = xReg * 10.0 + (double) cual;
			DispCash(xReg, cDecim);
			if(cDecim) cDecim++;
	    }
	}
}

//   Borra toda entrada y limpia la maquina.
void clear(int dummy)
{
	if(zapearTicketTemp)
	{
		zapearTicketTemp = false;
		//tTmp.Zap();
#ifdef POSXP
		//Dump::Browse->Refresh();
		if (Dump::cTicket != nullptr && Dump::cTicket->RecCount()>0)
			Dump::cTicket->LimpiarTicket();
#else
		bDet.dRefresh();
#endif
	}

	yReg = xReg = 0.0;
	cDecim = 0;
	borrar = 0;
	DispCash(xReg, usaDec);
	AdicClear();
	if(clrCustomerDisplay)
	{
		clrCustomerDisplay = false;
		CustDisp("Bienvenido", "Caja Habilitada");
	}

}

//   Procesa la tecla de doble cero.
void dblzr(int dummy)
{
	digitos(0);             // Manda dos tecleadas
	digitos(0);
}

//   Procesa la tecla de punto decimal.
void decpt(int dummy)
{
	if(!cDecim)	cDecim++;
}

/*******************  Rutinas de Homologacion de Valores  ************/
void HDecim()               // Homologa decimales en xReg
{
	if(cDecim)  return;     // Hay decimales manuales
	for(; cDecim < usaDec; cDecim++)
		xReg /= 10;
}

//  Escribe el nro double que se le pasa, como un valor estandard con los
//  decimales en uso.
char *HRng(double val)
{
	static char frmt[24];
	static char rng[24];

	sprintf(frmt, "%%12.%dlf", usaDec);
	sprintf(rng, frmt, val);
	return rng;
}

//   Prepara todo para que, a la siguiente tecla que se apriete, se simule
//   la depresion de un 'clear' previo. Esto lo maneja directamente 'kbd.cpp'
void HClear()
{
	borrar = 1;
}

