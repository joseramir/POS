//
//   MyStr.cpp
//   Algunas funciones de strings compatibles foxpro.
//

#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif


//  Devuelve .T. si la cadena esta vacio o tiene solo espacios y/o TABS.
int Empty(char *s)
{
	while(*s)
	{
		if(!isspace(*s)) return 0;
		s++;
	}
	return 1;
}

//	Remueve espacios a la izquierda y derecha de una string.
char *Trim(char *s)
{
	char *p;

    for(p = s + strlen(s) - 1; (p >= s) && isspace(*p & 0x7f); p--)
        ;
	p++;
	*p = '\0';
    for(p = s; *p && isspace(*p & 0x7f); p++)
        ;
	if(s != p) STRCPY(s, p);
	return s;
}

//   Remueve espacios igual que 'Trim', pero operando sobre un buffer particular
//   (No toca la string original).
char *Trim(char *src, char *buffer)
{
	STRCPY(buffer, src);
	return Trim(buffer);
}

// Completa una string, a su izquierda, con el caracter que se indica.
// PRESUPONE que el buffer reservado para la string es lo suficientemente largo.
char *Padl(char *s, int len, char fill)
{
	int actLen;

	actLen = strlen(s);
	if(actLen >= len) return s;
	memmove(s + (len - actLen), s, actLen);
	memset(s, fill, (len - actLen));
	return s;
}

