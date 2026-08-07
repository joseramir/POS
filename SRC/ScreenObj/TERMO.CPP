//
//   Termo.cpp
//   Soporte de termometros en pantalla.
//

#include <stdafx.h>

#ifdef POSXP
#include "MainForm.h"
#endif

//   Variables globales.
int TermoCienPerCien;		// Valor hasta el que llegara.

#ifdef POSXP
char lastMessage[200];
int lastValue;
#else
int TermoW;					// Handler de la ventana.
char TermoRepre[30];		// Representacion en pantalla.
int LastDisplayed;			// Para no actualizar la pantalla al vicio.
#endif

//   Abre la representacion del termometro en la pantalla.
void OpenTermo(char *msg, long CienPerCien)
{
	TermoCienPerCien = CienPerCien;

#ifdef POSXP
	MainForm::F->ActualizeTermo(msg, 0);
	STRCPY(lastMessage, msg);
	lastValue = 0;
#else
	TermoW = wDefine(21, 0, 24, 23, BORDERSINGLE, SNGCURS, 0, "", "");
	wActivate(TermoW);
	wLocate(0, 1);
	wPstr(msg);
#endif
}

//   Cierra la representacion del termometro.
void CloseTermo()
{
#ifdef POSXP
	MainForm::F->ActualizeTermo("", -1);
#else
	wRelease(TermoW);
#endif
}

//   Actualiza el termometro.
void AcTermo(long Percent)
{
#ifdef POSXP
	double calcPercent = ((double)Percent / (double)TermoCienPerCien) * 100.0;
	lastValue = (int)calcPercent;
	MainForm::F->ActualizeTermo(lastMessage, lastValue);
#else
	float inPercent;

	STRCPY(TermoRepre, "********************");
	if(Percent < TermoCienPerCien)
	{
		inPercent = (float) Percent / (float) TermoCienPerCien;
		TermoRepre[(int)(inPercent * 20)] = 0;
	}
	else
		inPercent = 1;
	if(LastDisplayed == (int)(inPercent * 100))
		return;
	wActivate(TermoW);
	wLocate(1, 1);
	wPstr(TermoRepre);
	LastDisplayed = (int)(inPercent * 100);
	sprintf(TermoRepre, "%3d%%", LastDisplayed);
	wLocate(1, 9);
	wPstr(TermoRepre);
#endif
}

//   Cambia el mensaje en la pantalla del termometro.
void AcTermoMsg(char *NewMsg)
{
#ifdef POSXP
	STRCPY(lastMessage, NewMsg);
	MainForm::F->ActualizeTermo(lastMessage, lastValue);
#else
	STRCPY(TermoRepre, "                      ");
	wActivate(TermoW);
	wLocate(0, 1);
	wPstr(TermoRepre);
	wLocate(0, 1);
	wPstr(NewMsg);
#endif
}
