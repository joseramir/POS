#include <stdafx.h>
#include "funcs.h"

// Variables locales
static GenForm *gf = NULL;

void cell_(int);
void cell(int cual)
{
	GCStatic::LockMainLoop("cell");
	cell_(cual);
	GCStatic::ReleaseLockMainLoop("cell");
}

// Procesa la registracion de carga de celulares
void cell_(int cual)
{
	// Validaciones de contexto.
	if(consmode) return;			// No funciona en consulta.
	if(rMode > 1) return;			// No funciona en X2 o Z
	if(chkClk()) return;			// Debe existir cajero loguineado.
	if(inOper)						// No debe haber ticket abierto.
	{
		Alert(
			"Las cargas de celulares solo se pueden efectuar en un",
			"ticket aparte. Reintente despues de cerrar el ticket");
		return;
	}

	// Vuelve a REG, si estaba en x1
	if(rMode)
	{
		double pxReg = xReg;
		xReg = 0.0;
		superv(0);
		xReg = pxReg;
	}

	// Construye la form para pedir los datos de la carga
	if(gf != NULL) delete gf;
	gf = new GenForm("Carga de Celulares");
	EditItem *celNum = new EditItem("Numero de Celular");			// Monto de la cobranza.
	celNum->CambiarTipo(TIPO_NUMREF);
	strcpy(celNum->help, "Ingrese el numero de celular completo, incluyendo el '15' y el codigo de area. Ejemplo: 0388155889900");
	gf->Items()->Add(celNum);
	EditItem *monto = new EditItem("Importe a Cargar");			// Monto de la cobranza.
	monto->CambiarTipo(TIPO_MONEDA);
	strcpy(monto->help, "Ingrese el monto a cargarle al celular, mayor de $10");
	gf->Items()->Add(monto);

	// Pide los datos y procesa la solicitud
	if(gf->Animar())
	{
	}
}
