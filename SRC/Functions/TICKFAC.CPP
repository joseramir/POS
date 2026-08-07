/*
 *   TickFac.cpp
 *   Emite ticket/factura en los nuevos controladores fiscales.
*/

#include <stdafx.h>
#include "funcs.h"          // Carga la estructura clk_

int GlobalTF;
struct DTickFac_ TickFac;

void tickfac_(int);
void tickfac(int cual)
{
	GCStatic::LockMainLoop("tickfac");
	tickfac_(cual);
	GCStatic::ReleaseLockMainLoop("tickfac");
}

//   Rutina de atencion del teclado.
void tickfac_(int flag)
{
    //   Revisa si el impresor fiscal dispone de la funcion.
#ifndef POSDEBUG
    if(!(pf->PrinterCap & PRN_TICKFAC))
    {
        Alert("La funcion de Ticket Factura no esta disponible",
              "Debe disponer de OTRO TIPO de impresor...");
        return;
    }
#endif

	char *errMsg = NULL;
	if(consmode)
	{
		CompleteCC(NULL, 0);
		ClienteBenef = 0;
		return;
	}
	else if(chkClk())
		errMsg = "No hay cajero habilitado todavia!!!";
	else if(inOper > 1)
		errMsg = "No puede usar la tecla en X2 o ZETA";
	else if (!scanartcobradora)
		errMsg = "Solo se realizan Cobranzas";

	if(errMsg != NULL)
	{
		Alert(errMsg, "");
		return;
	}	
    //   Revisa si ya se activo la funcion.
    if(GlobalCCSelected)
    {
		if(inOper)
		{
			Alert("Los datos del cliente ya se imprimieron!!!",
				  "Si debe corregirlos, CANCELE el ticket y hagalo de nuevo.");
			return;
		}
		else
		{
			TickFac.func = DTickFac;
			TickFac.cajero = ckaNro;
			TickFac.flag = 0;
			TickFac.fecha = (long) date();
			TickFac.hora = (long) time_();
			ProcTickFac(&TickFac, 1);
		
			DCCOnln_ onln;
			onln.func = DCCOnln;
			onln.ctacte = 0;
			onln.flagPrec = 0;
			ProcCCOnln(&onln, 1);

			TruncTicket();
			clear(1);
		}
    }
	else
	{
		//   Revisa que este el ticket cerrado.
		if(inOper)
		{
			Alert("Hay una ticket a medias, por lo que para cambiar a",
				  "ticket factura, debe CANCELARLO antes !!!");
			return;
		}

		// Selecciona la C/C (Cliente) sobre el que se efectuara la facturacion.
		if(!ForceCC(1)) return;

		// Asienta la operacion.
		TickFac.func = DTickFac;
		TickFac.cajero = ckaNro;
		TickFac.flag = 1;
		TickFac.fecha = (long) date();
		TickFac.hora = (long) time_();
		ProcTickFac(&TickFac, 1);
	}

	AdicClear();
	PrintTktTot();
}

//   Procesa y graba un ticket factura
void ProcTickFac(void *p_, int fwrite)
{
    struct DTickFac_ *p;

    p = (struct DTickFac_ *) p_;
    if(fwrite) WriteDump(p);
	if(p->flag)
		GlobalTF = 1;
	else
	{
		GlobalTF = 0;
		GlobalCCSelected = false;
	}
}

