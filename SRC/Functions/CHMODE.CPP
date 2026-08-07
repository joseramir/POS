//
//   Chmode.c
//   Cambia el modo actual de operacion.
//

#include <stdafx.h>
#include "funcs.h"

struct DDchmode_ chm;
char *modemsg[] = { "Reg", " X1", " X2", "  Z" };
long chPermi[10];
char *x1 = "X1", *x2 = "X2", *z = "Z";

void chgmode(int);

//   La siguiente funcion 'Registra' en trans.dat a la operacion de cambio
//   de modo.
void ProcChmode(void *d_, int fwrite)
{
    struct DDchmode_ *d;

    d = (struct DDchmode_ *) d_;
	if((d->nmode == rMode) || (d->nmode > 3)) return;
	consmode = 0;
	if(fwrite) WriteDump(d);
	rMode = d->nmode;
	AdicClear();			// Limpia la pantalla.
}

//   Esta funcion es llamada cada vez que se detecta un 'keystrok', con
//   el objeto de ver si es el preambulo de un cambio de modo.
int  ViewChMode(int ch)
{
	int res;
	short cha;

	if(ch != modescan) return 0;
	cha = xgetch();
    res = 0;												// Fuerza la vuelta a 'Reg'.
	if(cha == keyx1) res = 1;								// Valores en 'opciones.ini'
	if(cha == keyx2) res = 2;	
	if(cha == keyz)  res = 3;
	xReg = (double) res;
	chgmode(1);
	return 1;
}

void consulta_(int);
void consulta(int cual)
{
	GCStatic::LockMainLoop("consulta");
	consulta_(cual);
	GCStatic::ReleaseLockMainLoop("consulta");
}

//   La siguiente funcion PROCESA la funcion de activacion del modo de
//   consulta.
void consulta_(int dummy)
{
	 if(rMode)  return;
	 consmode = consmode? 0: 1;
	 AdicClear();
}

//  Funcion que cambia el modo. Es llamada por:
//     - Keystrok de cambio de modo (giro de la llave).
//     - Secuencia <nmodo> <tecla-de-cambio> (Si se define).
//     - ForceZeta, de la red.
//  Deteccion de scanners raros (plu.cpp)
void chgmode(int dummy)
{
	 int nFunc;

	 chm.func = DMode;
	 chm.cajero = ckaNro;
	 chm.supervisor = supervNro;
	 chm.fecha = date();
	 chm.hora = time__();
	 nFunc = (int) xReg;
	 if((nFunc < 0) || (nFunc > 3)) return;
	 chm.nmode = nFunc;
	 if(rMode == nFunc) chm.nmode = 0;
	 if((int) xReg == 0)
	 {
		ProcChmode(&chm, 1);
		HClear();
		return;
	 }

	 if(inOper && (chm.nmode > 1))
	 {
		Alert("No puede ir a X2 o Z cuando esta en el", "medio de una transaccion");
		return;
	 }

	 if(!autoDetected && !RegisterLocked)
	 {
		 int res = RequestPasswd((chm.nmode == 1)? x1: ((chm.nmode == 2)? x2: z));
		 if(!res) return;
	 }

	 ProcChmode(&chm, 1);
	 HClear();
}

