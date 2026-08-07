/*
 *   PPR5.c
 *   Modificaciones al driver de PR4 para la funcionalidad extendida del PR5 de hasar (615)
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//extern "C" {
//	PrinterPR5 P_PR5;
//};

//  Reemplaza caracteres ilegales en las descripciones
void ReplaceIllegalCharacters(char *src)
{
	char *dst = src;
	while(*src)
	{
		if(*src == '.' || *src == '-' || *src == ' ' ||
			(*src >= '0' && *src <= '9') ||
			(*src >= 'A' && *src <= 'Z') ||
			(*src >= 'a' && *src <= 'z'))
		{
			*dst++ = *src++;
		}
		else
			src++;
	}
	*dst = 0;
}

char *PrinterPR5::HasarBrand()
{
	return "Espere. Cargando el impresor HASAR 715/PR5F...";
}

void PrinterPR5::PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto)
{
	ReplaceIllegalCharacters(tdes);
	if(cajaBulto < 1) cajaBulto = 1;
	if(qual == 'D')
	{
		if(strlen(tdes) > 20) tdes[20] = 0;
		sprintf(txbuff, "m%c%s%c%.2lf%c%.2lf%c%c%c0.00000%c0%cT%cB" ,
			SEP, tdes, SEP, monto, SEP, xtasa, SEP, 'm', SEP, SEP, SEP, SEP);
	}
	else
	{
		if(strlen(tdes) > 30) tdes[30] = 0;
		sprintf(txbuff, "B%c%s%c%.3lf%c%.2lf%c%.2lf%c%c%c0.00000%c0%cT" ,
				SEP, tdes, SEP, mult * cajaBulto, SEP, monto, SEP, xtasa, SEP, qual, SEP, SEP, SEP);
	}
}

//  Los datos del cliente, para un ticket factura, tienen un formato diferente en el PR5
void PrinterPR5::PrepareCustomerData(char *hdrPkt)
{
	if(strlen(c_nombre) > 30)      c_nombre[30] = 0;
	if(strlen(c_direcc) > 40)      c_direcc[40] = 0;
    if(!_stricmp(c_condiva, "MO")) f_respon =  'M';

	ReplaceIllegalCharacters(c_nombre);
	ReplaceIllegalCharacters(c_direcc);

	char strippedCuit[20];
	int scPtr = 0;
	for(int i=0; i<(int)strlen(c_numero); i++)
	{
		char ch = c_numero[i];
		if(ch >= '0' && ch <= '9')
			strippedCuit[scPtr++] = ch;
	}
	strippedCuit[scPtr] = 0;

	sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s" ,SEP, c_nombre, SEP, strippedCuit, SEP, f_respon, SEP, f_respon=='C'? ' ': 'C', SEP, c_direcc);
	xchgPkt(hdrPkt);
}

