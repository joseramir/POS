/*
 *   Premios.c
 *   Manejo de Ofertas, premios y promociones.
*/

#include <stdafx.h>

char **PremLogoLine = (char **)new char*[5];
char **PremPieLine = (char **)new char*[5];
char *PremLine;
char *PremLineMed;
int M_PrintAtPie, M_NroVoucher;

//   La funcion que sigue se llama ante el final de CADA pie de ticket.
//   Se utiliza para verificar que, si hay millaje conseguido, se imprima
//   en el ticket el mensaje que corresponda haciendo la asignacion del
//   total de premios disponibles.
void TotPremios()
{
	int i;
	static int clearLastLine = 0, voucherLoaded = 0, voucherFlag = 0;

	if(clearLastLine) pf->SetPie("\x7f");
	clearLastLine = 0;
	switch(M_Modo)
	{
		//   Promociones desactivadas.
		case 0: return;

		//   Entrega x cupones por cada n artículos promocionados.
		case 2:
		case 1:
			VoucherVar("@TOT_PUNTOS@", (int) TMiles);
			int cCupones = ((int)(TMiles + 0.001)) / M_ParamModo;
			if(cCupones <= 0) return;
			VoucherVar("@CUPONES@", cCupones);
			if(M_NroVoucher)
			{
				//   Eligio imprimir un voucher...
				if(!voucherLoaded) 
				{
					voucherFlag = LoadVoucher(M_NroVoucher);
					voucherLoaded = 1;
				}
				if(voucherFlag) StackVoucher(M_NroVoucher);
			}
			else
			{
				//   No imprime voucher sino un conjunto de lineas al pie del ticket.
				if((TMiles > 0.0 && M_VerPie) || M_VerPie0)
				{
					if(M_PrintAtPie)
					{
						if(cCupones == 0)
						{
							if(M_VerPie0)
							{
								pf->SetPie("NO CORRESPONDEN CUPONES");
								clearLastLine = 1;
							}
						}
						else
						{
							sprintf(msg, PremLineMed, cCupones);
							pf->SetPie(msg);
							clearLastLine = 1;
						}
					}
					else
					{
						for(i = 0; i < PremiosLogo; i++)
							pf->PrintTick(PremLogoLine[i]);
						sprintf(msg, PremLineMed, (int)TMiles, cCupones);
						DispDeta(msg, "");
						for(i = 0; i < PremiosPie; i++)
							pf->PrintTick(PremPieLine[i]);
					}
				}
			}
			break;
	}
}

//   Verifica si hay millaje programado para el codigo, y devuelve
//   La cantidad de puntos que le asigna.
double ViewMiles(char *cod, double multi)
{
	static char umiles[10];
	double cant;

	switch(M_Modo)
	{
		case 0:		// Promociones desactivadas.
			return 0;

		case 1:		// Emisión de cupones.
			dbplu.Get("premio", umiles);
			cant = atoi(umiles);
			if(cant != 0) cant = (int) multi;	// Queda la cantidad de arts vendidos.
			return cant;

		case 2:		// Emision de cupones agrupada (P.Ej: 3 articulos hacen solo 1 cupone, 2 de otro hacen 1 y asi..)
			dbplu.Get("premio", umiles);
			cant = (double)atoi(umiles);
			if(cant == 0) return 0;
			return (multi * (1 / cant)) + 0.001;
	}
	return 0;
}
