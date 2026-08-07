/*
 *   Ivas.C
 *   Manejo de las Tasas de Iva.
*/

#include <stdafx.h>
#include "funcs.h"

double *iTasa = (double *)new double[8];
double *tNetos = (double *)new double[8];
double *tIva = (double *)new double[8];

double *tNpromo = (double *)new double[8];
//Para acumular valores de Percepcion IVA
double *tNetosIva = (double *)new double[8];

int LoadIvas()
{
	 char *c;

     if(!StartFile("ivas.def", 'i'))
     {
        while(GetPLine())
        {
            c = GetPToken();
            if(strlen(c) >= 16)
                *(c + 16) = 0;
            STRCPY(iDesc[cTasa], c);
			c = GetPToken();
			if(strlen(c) >= 16)
				*(c + 16) = 0;
			STRCPY(vDesc[cTasa], c);
            iTasa[cTasa] = atof(GetPToken());
            if(++cTasa > 8) break;
        }
        CloseFile('d');
     }
    return 1;
}

/*
 *   Calcula el iva a aplicar por una operacion NORMAL, y actualiza los
 *   totalizadores de iva del sistema.
*/

void CalcIva(double value, int tasa, int signo)
{
	double ovalue, uvalue;
	static char tmp[30];

	ovalue = value;
	tasa = (tasa < 0 || tasa > cTasa)? 0 : tasa;
	uvalue = 1 + (iTasa[tasa] / 100);
	value /= uvalue;
	sprintf(tmp, "%.2lf", value);
	value=atof(tmp);	
    tNetos[tasa] += (double) signo * fabs(value);
	sapimpiva = (double) signo * fabs(ovalue - value);
	tIva[tasa] += (double) signo * fabs(ovalue - value);
}

//calcula el iva para SAP
//void CalcIvaSap(double value, int tasa, int signo)
void CalcIvaSap(void *p_)
{
	struct DDplu_ *p;
	p = (struct DDplu_ *) p_;
   double ly = atof_(p->yReg);
	double lx = atof_(p->xReg);
	int signo = (lx < 0.0 || ly < 0.0)? -1.0: 1.0;
	double value = lx * signo;
	int tasa = p->iva - 1;

	double ovalue, uvalue;
	static char tmp[30];

	ovalue = value;
	tasa = (tasa < 0 || tasa > cTasa)? 0 : tasa;
	uvalue = 1 + (iTasa[tasa] / 100);
	value /= uvalue;
	sprintf(tmp, "%.2lf", value);
	value=atof(tmp);
	//para percepcion IVA solo se acumulan determinados materiales 
	if (p->apliPerIva)
		tNetosIva[tasa] += (double) signo * fabs(value);
    sapimpiva = (double) signo * fabs(ovalue - value);
	sprintf(p->impiva, "%.2lf", sapimpiva);	
}


//calcula el iva para SAP
//void CalcIvaSap(double value, int tasa, int signo)
void CalcIvaSapNc(void *p_)
{
	struct DDNcItem_ *p;
	p = (struct DDNcItem_ *) p_;
	double ly = atof_(p->yReg);
	double lx = atof_(p->xReg);
	int signo = (lx < 0.0 || ly < 0.0)? -1.0: 1.0;
	double value = lx * signo;
	int tasa = p->iva - 1;

	double ovalue, uvalue;
	static char tmp[30];

	ovalue = value;
	tasa = (tasa < 0 || tasa > cTasa)? 0 : tasa;
	uvalue = 1 + (iTasa[tasa] / 100);
	value /= uvalue;
	sprintf(tmp, "%.2lf", value);
	value=atof(tmp);	
	//para percepcion IVA solo se acumulan determinados materiales 
	if (p->apliPerIva)
		tNetosIva[tasa] += (double) signo * fabs(value);
	sapimpiva = (double) signo * fabs(ovalue - value);
	sprintf(p->impiva, "%.2lf", sapimpiva);	
}


/*
 *   El sistema permite cobrar una sobre-tasa por no inscripto.
 *   En este caso, la siguiente funcion devuelve el valor que le corres-
 *   ponderia a esta sobretasa, en este momento.
*/

double RetSobreTasa()
{
	double tgrav;
	int i;
	static char tmp[30];

	for(tgrav = 0.0, i = 0; i < cTasa; i++)
		if(iTasa[i])
			tgrav += tNetos[i] * iTasa[i] / 200;
	tgrav = sprintf(tmp, "%.2lf", tgrav);
	return atof(tmp);
}

/*
 *   A efectos de la impresion y control de los impresores fiscales,
 *   devuelve el VALOR de la tasa que se indica.
*/

int ivatasa(int cual)
{
	return (int)(iTasa[cual] * 100);
}

double ivadisc(int cual)
{
	return (iTasa[cual] / 100) + 1;
}


double CalculoPercep()
{
	double tgrav = 0;
	int i;
	static char tmp[30];

	for(tgrav = 0.0, i = 0; i < cTasa; i++)
		if(iTasa[i])
			tgrav += (tNetos[i] * 0.21);
	//tgrav = tgrav * 1.21;
	tgrav = sprintf(tmp, "%.2lf", tgrav);
	return atof(tmp);
}


void CalcIvaPromo(double value, int tasa, int signo)
{
	double ovalue, uvalue;
	static char tmp[30];

	ovalue = value;
	tasa = (tasa < 0 || tasa > cTasa)? 0 : tasa;
	uvalue = 1 + (iTasa[tasa] / 100);
	value /= uvalue;
	sprintf(tmp, "%.2lf", value);
	value=atof(tmp);	
    tNpromo[tasa] += (double) signo * fabs(value);
	/*sapimpiva = (double) signo * fabs(ovalue - value);
	tIva[tasa] += (double) signo * fabs(ovalue - value);*/
}

void ReinitIvaPromo()
{
	for(int i = 0; i < 4; i++)
		tNpromo[i] = 0.0;
}

//void ReinitNetoIva()
//{
//	for(int i = 0; i < 4; i++)
//		tNetosIva[i] = 0.0;
//}

double ValueIvaPromo()
{
	double lrvalue = 0.0;
	for(int i = 0; i < 4; i++)
		lrvalue += tNpromo[i];
	return lrvalue;
}

double ValueNetoIva()
{
	double lrvalue = 0.0;
	for(int i = 0; i < 4; i++)
		lrvalue += tNetosIva[i];
	return lrvalue;
}

double ValueIvas()
{
	double lrvalue = 0.0;
	for(int i = 0; i < 4; i++)
		lrvalue += tIva[i];
	return lrvalue;
}

double CalcNetoIva(double value, int tasa, int signo)
{
	double ovalue, uvalue;
	static char tmp[30];

	ovalue = value;
	tasa = (tasa < 0 || tasa > cTasa)? 0 : tasa;
	uvalue = 1 + (iTasa[tasa] / 100);
	value /= uvalue;
	sprintf(tmp, "%.2lf", value);
	value=atof(tmp);	
	return ((double) signo * fabs(value));
	//tNetos[tasa] += (double) signo * fabs(value);
	//sapimpiva = (double) signo * fabs(ovalue - value);
	//tIva[tasa] += (double) signo * fabs(ovalue - value);
}