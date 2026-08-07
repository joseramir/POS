#include "stdafx.h"

#define TEMP_POOL 3000

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

void *tempPtrs[TEMP_POOL + 10];
int nextPtr;

void *Temp::Alloc(int size)
{
	// Libera cualquier bloque previo
	try
	{
		// Aunque aqui tengamos un puntero valido, el objeto podria ya haber sido liberado sin haber informado a esta clase).
		if(tempPtrs[nextPtr] != NULL)
			delete tempPtrs[nextPtr];
	}
	catch(...) {}

	// Ubica el buffer pedido
	void *result = new char[size];
	tempPtrs[nextPtr++] = result;
	if(nextPtr >= TEMP_POOL) nextPtr = 0;
	return result;
}

void Temp::SetTemporary(void *ptr)
{
	// Libera cualquier bloque previo
	try
	{
		// Aunque aqui tengamos un puntero valido, el objeto podria ya haber sido liberado sin haber informado a esta clase).
		if(tempPtrs[nextPtr] != NULL)
			delete tempPtrs[nextPtr];
	}
	catch(...) {}

	// Ubica el buffer pedido
	tempPtrs[nextPtr++] = ptr;
	if(nextPtr >= TEMP_POOL) nextPtr = 0;
}

char *Temp::StrDup(char *str)
{
	int size = strlen(str) + 1;
	return (char *) Temp::MemDup(str, size);
}

void *Temp::MemDup(void *src, int size)
{
	void *result = Alloc(size);
	memmove(result, src, size);
	return result;
}

void  Temp::Free(void *ptr)
{
	for(int i=0; i<TEMP_POOL; i++)
	{
		if(tempPtrs[i] == ptr)
		{
			tempPtrs[i] = NULL;
			break;
		}
	}

	// Siempre deletea, aunque ignora errores (que se produciran si el objeto ya fue liberado antes)
	try
	{
		delete ptr;
	}
	catch(...) {}
}

#ifdef POSXP
#pragma managed(pop)
#endif
