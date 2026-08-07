#include "stdafx.h"

#ifndef POSXP

// (Codigo valido solamente para la version de MSDOS)
GCLockBag kbLock, eventLock, transLock, posLock;
extern int idxTask;

void GCStatic::DoLock(GCLockBag *bag)
{
	// Las operaciones que se hagan sobre los GCLockBag deben ser atomicas. Por eso, deshabilitamos las interrupciones
	// mientras manipulamos los bloqueos y las volvemos a activar justo antes de interactuar con el resto del POS.
	while(true)
	{
		// Trata de bloquear.
		_asm { _CLI_ }
		if(bag->lockCount++ == 0) 
		{ 
			bag->ownerThread = idxTask;
			_asm { _STI_ } 
			return; 
		}

		// No pudo bloquear. Si estamos en la misma thread que adquirio el lock, no esperamos que se libere el bloqueo
		// sino que, por el contrario, lo damos por bueno. (La semantica es que el mismo objeto no puede ser bloqueado mas
		// de una vez por la misma thread).
		// (De todas formas, el contador permanece con el nuevo incremento, por lo que sigue siendo valido que hay
		//  que llamar a UnLock tantas veces como se llamo a Lock para liberar el bloqueo al objeto).
		if(bag->ownerThread == idxTask)
		{
			_asm { _STI_ }
			return;
		}

		// Otra thread tiene al objeto bloqueado. Esperamos...
		bag->lockCount--;
		_asm { _STI_ }
		tSwitch();
	}
}

void GCStatic::DoUnlock(GCLockBag *bag)
{
	_asm { _CLI_ }
	if(--bag->lockCount <= 0) { bag->lockCount = 0; }
	_asm { _STI_ }
}

#endif