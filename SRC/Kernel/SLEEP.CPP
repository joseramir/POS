/*
 *   sleep.c
 *   'Duerme'  durante la cantidad de segundos que se le especifica.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

void sleep_(int cuanto)
{
	time_t esperar;

	esperar = time(0) + cuanto;
	while(esperar > time(0)) 
		tSwitch();
}

