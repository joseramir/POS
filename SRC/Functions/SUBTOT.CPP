//
//  SubTot.c
//  Imprime en pantalla subtotales de la operacion.
//

#include <stdafx.h>
#include "funcs.h"

char *izq = new char[100];
char *der = new char[100];
char vDesc[10][50];
char iDesc[10][50];
char sepAst[] = "****************************************";

// Detalle de medios de cobro resumidos.
extern ::ArrayList *mpconso;

//  Para utilizar el mecanismo de sincr7onizacion con el printer.
char *prot = new char[100];
double printerTot;
char *prot2 = new char[100];
bool forceClrLog = false;

//  Funcion que ordena los medios de cobro
::ArrayList *OrdenarMedios(void);

// Funcion que resume los medios de cobro del 'AcumCajero' que se le pasa, en un
// nuevo 'AcumCajero' que contiene solo los medios de cobro acumulados por su "linkecr".
AcumCajero *Resumir(AcumCajero *vtas)
{
	AcumCajero *res = new AcumCajero(vtas->CajeroNum());
	
	// Acumula los medios de cobro
	for(int i = vtas->VentaMedios()->Count(); i > 0; i--)
	{
		AcumMedio *orig = (AcumMedio *)vtas->VentaMedios()->Item(i - 1);
		struct mpag_ *mporig = BuscarMpag(orig->VerNumeroMedio());
		if(mporig != NULL)
		{
			AcumMedio *dest = res->VerAcumuladorMedio(mporig->linkecr);
			dest->AgregaCobro(orig->VerMontoCobros() - orig->VerMontoCobOnl() , orig->VerCantidadCobros() - orig->VerCantidadCobOnline());
			dest->AgregaVenta(orig->VerMontoVentas() - orig->VerMontoOnline(), orig->VerCantidadVentas() - orig->verCantidadVtasOnline());
			dest->AgregaDotacion(orig->VerMontoDotaciones(), orig->VerCantidadDotaciones());
			dest->AgregaRetiro(orig->VerMontoRetiros(), orig->VerCantidadRetiros());
			dest->AgregaExtraccion(orig->VerMontoExtracciones(), orig->VerCantidadExtracciones());

			if(orig->VerCantidadCobOnline() != 0 || orig->verCantidadVtasOnline() != 0)
			{
				dest->AgregaCobroOnline(orig->VerMontoCobOnl(), orig->VerCantidadCobOnline());
				dest->AgregaVentaOnline(orig->VerMontoOnline(), orig->verCantidadVtasOnline());
			}
		}
	}

	// Acumula los otros totalizadores.
	int cant = 0;
	double monto = 0;
	vtas->VerAnulacionRenglones(&cant, &monto);		// Anulaciones de renglon
	res->AnulaRenglon(cant, monto);
	vtas->VerAnulacionTotal(&cant, &monto);			// Anulaciones total de ticket
	res->AnulaTicket(cant, monto);
	vtas->VerSuspensiones(&cant, &monto);			// Tickets en espera
	res->Suspension(cant, monto);
	vtas->VerEnviadoCobradora(&cant, &monto);			// Tickets Enviado a cobradora
	res->EnviadoCobradora(cant, monto);
	vtas->VerPercepciones(&cant, &monto);			// Percepciones II BB RG 974
	res->Percepcion(cant, monto);
	vtas->VerTissh(&cant, &monto);			       // Percepciones TISSH Salta
	res->Tissh(cant, monto);

	vtas->VerPercepIva(&cant, &monto);		       // Percepciones IVA RG 5329
	res->PercepIva(cant, monto);

	vtas->VerAnulTarj(&cant, &monto);			   // Anulaciones Tarjetas Tipre
	res->AnulTarj(cant, monto);
	vtas->VerAnulSosMovil(&cant, &monto);		   // Anulaciones SOS Movil
	res->AnulSosMovil(cant, monto);
	vtas->VerExtracciones(&cant, &monto);			//Extracciones ExtraCash
	res->Extraccion(cant, monto);

	// Acumula retiros
	res->Retiro(vtas->VerCantidadRetiros(), vtas->VerMontoRetiros());

	// Acumula envases recuperados
	for(int i = vtas->Envases()->Count(); i > 0; i--)
	{
		Acum *orig = (Acum *)vtas->Envases()->Item(i - 1);
		Acum *dest = res->VerAcumuladorEnvases(orig->VerCodigo());
		dest->Agrega(orig->VerCantidad(), orig->VerMonto());
	}

	// Acumula envases vendidos
	for(int i = vtas->EnvasesVendidos()->Count(); i > 0; i--)
	{
		Acum *orig = (Acum *)vtas->EnvasesVendidos()->Item(i - 1);
		Acum *dest = res->VerAcumuladorEnvasesVendidos(orig->VerCodigo());
		dest->Agrega(orig->VerCantidad(), orig->VerMonto());
	}

	// Acumula practipagos
	for(int i = vtas->Cobranzas()->Count(); i > 0; i--)
	{
		Acum *orig = (Acum *)vtas->Cobranzas()->Item(i - 1);
		Acum *dest = res->VerAcumuladorCobros(orig->VerCodigo());
		dest->Agrega(orig->VerCantidad(), orig->VerMonto());
	}
	return res;
}

// Rutina comun de impresion de ventas de cajeros. Este cuerpo se imprime tanto en
// la "Y" del cajero como al pie del arqueo.
void PrintDatosUtiles(AcumCajero *vtas, bool enArqueo)
{
	char izq[50], der[50];
	int tmpCant;
	double tmpMonto;

	if(!enArqueo || retiCod <= 0)
	{
		DispDeta("", "");
		DispDeta("----------------------------------------", "");
		sprintf(izq, "Retiros:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadRetiros(), vtas->VerMontoRetiros());
		DispDeta(izq, der);

		sprintf(izq, "Extra Cash:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadExtracciones(), vtas->VerMontoExtracciones());
		DispDeta(izq, der);

		sprintf(izq, "Percepciones IIBB RG 974:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadPercepciones(), vtas->VerMontoPercepciones());
		DispDeta(izq, der);

		sprintf(izq, "Percepciones TISSH RG 060/08:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadTissh(), vtas->VerMontoTissh());
		DispDeta(izq, der);		

		sprintf(izq, "Percepciones IVA RG 5329:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadPercepIva(), vtas->VerMontoPercepIva());
		DispDeta(izq, der);		
	}

	sprintf(izq, "Voucher Descuento:");
	sprintf(der, "[%2d]", vtas->VerCantPromo());
	DispDeta(izq, der);

	sprintf(izq, "Señas Dinero:");
	sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadSenia(), vtas->VerMontoSenia());
	DispDeta(izq, der);		

	DispDeta("", "");
	DispDeta("************ ANULACIONES ***************", "");

	vtas->VerAnulacionRenglones(&tmpCant, &tmpMonto);
	sprintf(izq, "Anul. de Renglon:");
	sprintf(der, "[%2d]%12.2lf", tmpCant, tmpMonto);
	DispDeta(izq, der);

	vtas->VerAnulacionTotal(&tmpCant, &tmpMonto);
	sprintf(izq, "Anul. de Tickets:");
	sprintf(der, "[%2d]%12.2lf", tmpCant, tmpMonto);
	DispDeta(izq, der);

	vtas->VerSuspensiones(&tmpCant, &tmpMonto);
	sprintf(izq, "Ticket en Espera:");
	sprintf(der, "[%2d]%12.2lf", tmpCant, tmpMonto);
	DispDeta(izq, der);

	vtas->VerEnviadoCobradora(&tmpCant, &tmpMonto);
	sprintf(izq, "Ticket Env.Cobradora:");
	sprintf(der, "[%2d]%12.2lf", tmpCant, tmpMonto);
	DispDeta(izq, der);

	sprintf(izq, "Anul. Tarjetas:");
	sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadAnulTarj(), vtas->VerMontoAnulTarj());
	DispDeta(izq, der);

	sprintf(izq, "Anul. SOS Movil:");
	sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadAnulSosMovil(), vtas->VerMontoAnulSosMovil());
	DispDeta(izq, der);

	//Para los ticket acum por reinicio
	AcumTickCancelados(vtas->CajeroNum(), tmpCant, tmpMonto);
	sprintf(izq, "Ticket Corte Cor:");
	sprintf(der, "[%2d]%12.2lf", tmpCant, tmpMonto);
	DispDeta(izq, der);

	DispDeta("", "");
	DispDeta("**  RESUMEN DE COBRANZAS DE SERVICIOS **", "");
	if(vtas->Cobranzas()->Count() == 0)
		DispDeta("Ninguna", "");
	else
	{
		for(int i = vtas->Cobranzas()->Count() - 1; i >= 0; i--)
		{
			Acum *cob = (Acum *)vtas->Cobranzas()->Item(i);
			sprintf(izq, "%s", cob->VerNombre());
			sprintf(der, "[%2d]%12.2lf", cob->VerCantidad(), cob->VerMonto());
			DispDeta(izq, der);
		}
	}

	DispDeta("", "");
	DispDeta("***** CUPONES DE ENVASES RECIBIDOS *****", "");
	if(vtas->Envases()->Count() == 0)
		DispDeta("Ninguno", "");
	else
	{
		for(int i = vtas->Envases()->Count() - 1; i >= 0; i--)
		{
			Acum *env = (Acum *)vtas->Envases()->Item(i);
			sprintf(izq, "%d-%s", env->VerCodigo(), env->VerNombre());
			sprintf(der, "[%d]", env->VerCantidad());
			DispDeta(izq, der);
		}
	}

	DispDeta("", "");
	DispDeta("*****  RESUMEN DE ENVASES VENDIDOS *****", "");
	if(vtas->EnvasesVendidos()->Count() == 0)
		DispDeta("Ninguno", "");
	else
	{
		for(int i = vtas->EnvasesVendidos()->Count() - 1; i >= 0; i--)
		{
			Acum *env = (Acum *)vtas->EnvasesVendidos()->Item(i);
			sprintf(izq, "%d-%s", env->VerCodigo(), env->VerNombre());
			sprintf(der, "[%d]", env->VerCantidad());
			DispDeta(izq, der);
		}
	}

	DispDeta("", "");
	DispDeta("----------------------------------------", "");
}

//   Imprime el reporte de venta de los Cajeros.
void PrintZetaCajero()
{
	mpag_ *mp;
	int i, aClk;
	char uCod[10], mpdes[50], mptit[50];
	double importe, importeC, cantidad, tImporte, tImporteC, tCantidad;
	

	SendMsg("Imprimiendo 'Y' de Cajeros", "");
	dbclk.Use(F_Cajeros);
	::ArrayList *sMedios = OrdenarMedios();
	for(aClk = 0; aClk < 9999; aClk++)
	{
		//   Mira si hay zeta para un determinado cajero,
		dbclk.Go(aClk + 1);
		dbclk.Get("codigo", uCod);
		AcumCajero *vtas = (AcumCajero *)ZetaCajeros->Find(atoi(uCod));
		if(vtas == NULL) continue;

		//   Imprime el report con la Y del cajero
		PrintClerk();
		pf->OpenVoucher();
		DispDeta("", "");
		sprintf(tmsg, "@4Y=[%d]-%s", vtas->CajeroNum(), vtas->CajeroNombre());
		Trim(tmsg);
		DispDeta(tmsg, "");
		DispDeta("", "");

		// Imprime un reporte RESUMIDO por "grupo" de medio de cobro.
		AcumCajero *resumen = Resumir(vtas);
		DispDeta("&4********************", "");
		DispDeta("&4****** RESUMEN *****", "");
		DispDeta("", "");

		char tMsg[40], tMpDes[100];
		double tv = 0;
		for(i = 0; i < mpconso->Count(); i++)
		{
			mp = (mpag_ *)mpconso->Item(i);
			char *mpdes = mp->des;
			while(isdigit(*mpdes) || *mpdes == '-') mpdes++;
			AcumMedio *am = resumen->VerAcumuladorMedio(mp->nro);

			bool tieneOnline = (am->VerCantidadCobOnline() > 0) || (am->verCantidadVtasOnline() > 0);
			double monto = am->VerMontoVentas() + am->VerMontoCobros() - am->VerMontoCobOnl() - am->VerMontoOnline();
			int cant = am->VerCantidadVentas() + am->VerCantidadCobros() - am->VerCantidadCobOnline() - am->verCantidadVtasOnline();
			Trim(strcpy(tMpDes, mpdes));
			if(tieneOnline)
				strcat(tMpDes, " [MANUAL]");

			sprintf(tMsg, "[%3d]%12.2lf", cant, monto);
			DispDeta(tMpDes, tMsg);
			tv += monto;

			if(tieneOnline)
			{
				Trim(strcpy(tMpDes, mpdes));
				strcat(tMpDes, " [ONLINE]");
				monto = am->VerMontoOnline() + am->VerMontoCobOnl();
				cant = am->VerCantidadCobOnline() + am->verCantidadVtasOnline();
				sprintf(tMsg, "[%3d]%12.2lf", cant, monto);
				DispDeta(tMpDes, tMsg);
			    tv += monto;
			}
		}

		delete resumen;
		DispDeta("", "");
		sprintf(izq, "Suma Total:");
		sprintf(der, "%12.2lf", tv);
		DispDeta(izq, der);
		sprintf(izq, "Retiros:");
		sprintf(der, "[%2d]%12.2lf", vtas->VerCantidadRetiros(), vtas->VerMontoRetiros());
		DispDeta(izq, der);

		// Imprime el reporte DETALLADO
		DispDeta("", "");
		DispDeta("&4********************", "");
		DispDeta("&4*DETALLE POR MEDIO *", "");
		DispDeta("", "");

		/////////////////////////////////
		tCantidad = tImporte = tImporteC = 0.0;
		for(i = 0; i < cMpag; i++)
		{
			mp = (mpag_ *) sMedios->Item(i);
			AcumMedio *am = vtas->VerAcumuladorMedio(mp->nro);

			// Descripcion
			STRCPY(mpdes, mp->des);
			Trim(mpdes);
			STRCPY(mptit, mpdes);
			STRCAT(mptit, " (");
			STRCAT(mptit, mp->grptot);
			Trim(mptit);
			STRCAT(mptit, ")");

			// Totales del medio de cobro.
			cantidad = am->VerCantidadCobros() + am->VerCantidadVentas();
			importe = am->VerMontoVentas() + am->VerMontoDotaciones() - am->VerMontoRetiros();
			importeC = am->VerMontoCobros();

			if(cantidad != 0 || importe != 0 || importeC != 0)
			{
				// Imprime Header y Totales
				DispDeta(mptit, HRng(am->VerMontoVentas() + importeC));

				// Imprime detalle del medio de cobro
				if(am->verCantidadVtasOnline() != 0 || am->VerCantidadCobOnline() != 0)
				{
					sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadVentas()-am->verCantidadVtasOnline(), 
						am->VerMontoVentas() - am->VerMontoOnline());
					DispDeta("  Vtas Manual:", tMsg);
					sprintf(tMsg, "[%3d]%12.2lf", am->verCantidadVtasOnline(), am->VerMontoOnline());
					DispDeta("  Vtas Online:", tMsg);

					if(am->VerCantidadCobros() != 0)
					{
						sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadCobros()-am->VerCantidadCobOnline(), 
							am->VerMontoCobros() - am->VerMontoCobOnl());
						DispDeta("  Cob. Manual:", tMsg);
						sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadCobOnline(), am->VerMontoCobOnl());
						DispDeta("  Cob. Online:", tMsg);
					}
				}
				else
				{
					sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadVentas(), am->VerMontoVentas());
					DispDeta("  Vtas Manual:", tMsg);
					if(am->VerCantidadCobros() != 0)
					{
						sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadCobros(), am->VerMontoCobros());
						DispDeta("  Cobros:", tMsg);
					}
				}

				if(am->VerCantidadDotaciones() != 0)
				{
					sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadDotaciones(), am->VerMontoDotaciones());
					DispDeta("  Dotac.:", tMsg);
				}

				if(mp->nro == 1 || am->VerCantidadRetiros() != 0)
				{
					sprintf(tMsg, "[%3d]%12.2lf", am->VerCantidadRetiros(), am->VerMontoRetiros());
					DispDeta(" Retiros:", tMsg);

					double enCaja = importe + importeC;
					sprintf(izq, " %s en Caja:", mpdes);
					DispDeta(izq, HRng(enCaja));
				}				

				DispDeta("", "");
				sprintf(tmsg_, "%-40s", mp->des);

				tCantidad += cantidad;
				tImporte += am->VerMontoVentas();
				tImporteC += importeC;
			}
		}

		DispDeta("", "");
		DispDeta("&4********************", "");
		DispDeta("", "");
		DispDeta("TOTAL DEL CAJERO:", HRng(tImporte + tImporteC));
		DispDeta("       VENTAS:", HRng(tImporte));
		DispDeta("  COBROS SERV:", HRng(tImporteC - vtas->VerMontoCobroCC()));
		sprintf(tMsg, "[%3d]%12.2lf", vtas->VerCantidadCobroCC(), vtas->VerMontoCobroCC());
		DispDeta("  COBROS  C/C:", tMsg);

		// Imprime informacion estadistica y cierra el voucher
		PrintDatosUtiles(vtas, false);
		pf->CloseVoucher();
	}
	dbclk.Close();
	delete sMedios;
	ClearMsg();
}

void ConcatenaEImprime(char *plinea, char *cagregar, bool force)
{
	if (!force)
	{
		STRCAT(plinea, cagregar);
		STRCAT(plinea, " ");
	}
	if (force || strlen(plinea) > 100)
	{
		if(pf->GlobalVoucherFlag && strlen(plinea) > 0) 
			pf->PrintVoucher(plinea);
		plinea[0] = 0;
	}
}


//   Imprime el reporte de venta por departamentos.
void PrintZetaDepto()
{
	struct acum *x;
	int i;
	double tCant, tMonto;
	char depNro[40];

	SendMsg("Imprimiendo Venta por Departamentos", "");
	tCant = tMonto = 0.0;
	PrintClerk();
	pf->OpenVoucher();
	DispDeta("","");
	DispDeta(sepAst,"");
	DispDeta("** INFORME DE VENTAS POR DEPARTAMENTO **","");
	DispDeta(sepAst,"");
	for(x = zz_depto, i = zz_deptoQ; i; i--, x++)
	{
		STRCPY(izq, x->des);
		DispDeta(izq, "");
		sprintf(izq, "Cant: %.2lf", x->cant);
		sprintf(der, "$%.2lf", x->monto);
		DispDeta(izq, der);
		DispDeta("", "");
		DispDeta("", "");
		sprintf(depNro, "Dep:%-3d", x->nro);
		sprintf(tmsg_, "%-40s", x->des);

		tCant += x->cant;
		tMonto += x->monto;
	}
	if(tCant != 0.0 || tMonto != 0)
	{
		DispDeta("MONTO ACUMULADO:", "");
		sprintf(izq, "Cant: %.2lf", tCant);
		sprintf(der, "$%.2lf", tMonto);
		DispDeta(izq, der);
	}
	else
		DispDeta("            << Sin Venta >>", "");
	DispDeta(sepAst, "");
	DispDeta("", "");
	pf->CloseVoucher();
	ClearMsg();
}


//   Imprime el reporte de venta por medio de pago.
void PrintZetaMpag()
{
	struct acum *x, *xx;
	int i;
	double tCant, tMonto, tCobros;
	double impNotasCredito = 0;
	
	SendMsg("Imprimiendo Informe de Ventas", "por Medio de Cobro");
	tCant = tMonto = tCobros = 0.0;
	PrintClerk();
	pf->OpenVoucher();
	DispDeta("","");
	DispDeta(sepAst,"");
	DispDeta("**  INFORME TOTAL DE MEDIOS DE COBRO  **","");
	DispDeta(sepAst,"");
	
	bool printTarj = true;
	::ArrayList *sMedios = OrdenarMedios();
	for(int j = 0; j < sMedios->Count(); j++)
	{
		mpag_ *target = (mpag_ *)sMedios->Item(j);
		x = NULL;
		for(xx = zz_mpag, i = zz_mpagQ; i; i--, xx++)
		{
			if(xx->nro == target->nro)
			{
				x = xx;
				break;
			}
		}
	
		if(x == NULL) continue;

		//lo que esta comentado era una logica para impedir la impresion de medios marcados como tarjeta credito/debito
		/*if (strcmp(target->grptot, "2-TARJETAS      ") == 0 || strcmp(target->grptot, "4-TARJ.DEBITO   ") == 0){
			if (printTarj){

				printTarj = false;
			}
			continue;
		}*/


		STRCPY(izq, x->des);
		DispDeta(izq, "");
		sprintf(izq, "   Total: %.2lf", x->cant);
		sprintf(der, "$%.2lf", x->monto + x->cobros);
		sprintf(tmsg_, "%-40s", x->des);

		DispDeta(izq, der);
		sprintf(der, "$%.2lf", x->monto);
		DispDeta("  Ventas", der);
		sprintf(der, "$%.2lf", x->cobros);
		DispDeta("  Cobros", der);

		DispDeta("", "");
		DispDeta("", "");
		tCant += x->cant;
		tMonto += x->monto;
		tCobros += x->cobros;
	}
	if(tCant != 0.0 || tMonto != 0)
	{
		impNotasCredito = ventaFac[3] + ventaFac[4] + ventaFac[5];//Importe total de las Nc
		DispDeta(sepAst,"");
		DispDeta("RESUMEN DE LA JORNADA FISCAL:", "");
		sprintf(izq, "   Total %.2lf", tCant);
		sprintf(der, "$%.2lf", tMonto + tCobros);//sprintf(der, "$%.2lf", tMonto + tCobros - impNotasCredito);
		DispDeta(izq, der);		
		sprintf(der, "$%.2lf", tMonto + impNotasCredito);
		DispDeta("  Ventas", der);

		/*sprintf(der, "$%.2lf", tCobros);
		DispDeta("  Cobros", der);*/
		sprintf(der, "$%.2lf", impNotasCredito);
		DispDeta("  Notas Credito", der);
		
		sprintf(der, "$%.2lf", gtotalextracc);
		DispDeta("  Extra Cash:", der);
		sprintf(der, "$%.2lf", gtotalpercep);
		DispDeta("  Percepciones IIBB RG 974:", der);
		sprintf(der, "$%.2lf", gtotaltissh);
		DispDeta("  Percepciones TISSH RG 060/08:", der);
		sprintf(der, "$%.2lf", gTotalPercepIva);
		DispDeta("  Percepciones IVA RG 5329:", der);

		sprintf(der, "$%.2lf", gtotalsenia);
		DispDeta("  Señas Dinero:", der);
	}
	else
		DispDeta("            << Sin Venta >>", "");
	DispDeta(sepAst, "");
	delete sMedios;

	DispDeta("RESUMEN DE COMPROBANTES EMITIDOS:", "");
	sprintf(izq, "T/F -A-");
	sprintf(der, "[%3d]%12.2lf", countFac[1], ventaFac[1]);
	DispDeta(izq, der);
	if(countFac[1] != 0)
	{
		sprintf(izq, "  Desde: %d al %d", minNumFac[1], maxNumFac[1]);
		DispDeta(izq, "");
		for(int j=0; j<10; j++)
		{
			if(m_tasas[1][j] != 0)
			{
				sprintf(izq, "  IVA %.1lf%%", (m_tasas[1][j] - 1.0) * 100);
				sprintf(der, "[%.2lf]%12.2lf", m_ivas[1][j], m_netos[1][j]);
				DispDeta(izq, der);
			}
		}
	}
	DispDeta("", "");

	sprintf(izq, "T/F -B-");
	sprintf(der, "[%3d]%12.2lf", countFac[2], ventaFac[2]);
	DispDeta(izq, der);

	sprintf(izq, "Tickets a C/F");
	sprintf(der, "[%3d]%12.2lf", countFac[0], ventaFac[0]);
	DispDeta(izq, der);
	for(int j=0; j<10; j++)
	{
		if(m_tasas[0][j] != 0)
		{
			sprintf(izq, "  IVA %.1lf%%", (m_tasas[0][j] - 1.0) * 100);
			sprintf(der, "[%.2lf]%12.2lf", m_ivas[0][j], m_netos[0][j]);
			DispDeta(izq, der);
		}
	}

	/*********************/
	DispDeta("NOTAS DE CREDITO", "");
	sprintf(izq, "NC a C/F");
	sprintf(der, "[%3d]%12.2lf", countFac[3], ventaFac[3]);
	DispDeta(izq, der);
	if(countFac[3] != 0)
	{
		sprintf(izq, "  Desde: %d al %d", minNumFac[3], maxNumFac[3]);
		DispDeta(izq, "");
		/*for(int j=0; j<10; j++)
		{
			if(m_tasas[1][j] != 0)
			{
				sprintf(izq, "  IVA %.1lf%%", (m_tasas[1][j] - 1.0) * 100);
				sprintf(der, "[%.2lf]%12.2lf", m_ivas[1][j], m_netos[1][j]);
				DispDeta(izq, der);
			}
		}*/
	}
	DispDeta("", "");

	sprintf(izq, "NC -A-");
	sprintf(der, "[%3d]%12.2lf", countFac[4], ventaFac[4]);
	DispDeta(izq, der);
	if(countFac[4] != 0)
	{
		sprintf(izq, "  Desde: %d al %d", minNumFac[4], maxNumFac[4]);
		DispDeta(izq, "");
		/*for(int j=0; j<10; j++)
		{
			if(m_tasas[1][j] != 0)
			{
				sprintf(izq, "  IVA %.1lf%%", (m_tasas[1][j] - 1.0) * 100);
				sprintf(der, "[%.2lf]%12.2lf", m_ivas[1][j], m_netos[1][j]);
				DispDeta(izq, der);
			}
		}*/
	}
	DispDeta("", "");

	sprintf(izq, "NC -B-");
	sprintf(der, "[%3d]%12.2lf", countFac[5], ventaFac[5]);
	DispDeta(izq, der);
	if(countFac[5] != 0)
	{
		sprintf(izq, "  Desde: %d al %d", minNumFac[5], maxNumFac[5]);
		DispDeta(izq, "");
		/*for(int j=0; j<10; j++)
		{
			if(m_tasas[1][j] != 0)
			{
				sprintf(izq, "  IVA %.1lf%%", (m_tasas[1][j] - 1.0) * 100);
				sprintf(der, "[%.2lf]%12.2lf", m_ivas[1][j], m_netos[1][j]);
				DispDeta(izq, der);
			}
		}*/
	}
	DispDeta("", "");
	/*********************/

	pf->CloseVoucher();
	ClearMsg();
}

//   Imprime el reporte de venta por vendedor.
void PrintZetaVend(int VNro)
{
	static dbf dbven;
	acum *x;
	int aVen, codeVen, restan, tLen;
	char uCod[20], uNom[50];
	double importe, cantidad, tImporte, tCantidad;

	SendMsg("Imprimiendo Informe de Ventas por Vendedor (Mozo)", "");
	dbven.Use("pos\\mozos");
	tCantidad = tImporte = 0.0;
	for(aVen = 0; aVen < cVen_; aVen++)
	{

		//   Mira si hay zeta para un determinado cajero,
		dbven.Go(aVen + 1);
		dbven.Get("codigo", uCod);
		dbven.Get("nombre", uNom);
		codeVen = atoi(uCod);
		if(VNro != 0 && VNro != codeVen) continue;
		Trim(uNom);
		for(x = zz_vend, restan = zz_vendQ; restan; restan--, x++)
			if(x->nro == codeVen)
				break;
		if(!restan) continue;

		//   Imprime el report.
		PrintClerk();
		pf->OpenVoucher();
		DispDeta("","");
		DispDeta(sepAst,"");
		DispDeta("*** INFORME DE RECAUDACION POR MOZO  ***","");
		DispDeta(sepAst,"");

		DispDeta("VENTAS MOZO ", uNom);
		sprintf(msg, "Cod:%s", uCod);
		sprintf(tmsg_, " VENTAS MOZO '%s' ", uNom);
		tLen = (40 - strlen(tmsg_)) / 2;
		while(tLen--) STRCAT(tmsg_, "*");
		tLen = 40 - strlen(tmsg_);
		memmove(tmsg_ + tLen, tmsg_, strlen(tmsg_) + 1);
		memset(tmsg_, '*', tLen);

		cantidad = x->cant;
		importe = x->monto;

		// Imprime el renglon.
		sprintf(uNom, "%-12.0lf", x->cant);
		DispDeta(uNom, HRng(x->monto));
		DispDeta("", "");
		DispDeta("", "");

		tCantidad += cantidad;
		tImporte += importe;
	}
	if(VNro)
	{
		DispDeta("TOTAL DEL MOZO:", "");
		WriteTickTmp("", "*****************TOTAL******************",
			tCantidad, 0, 0, tImporte, 0);

		sprintf(uNom, "%-12.0lf", tCantidad);
		DispDeta(uNom, HRng(tImporte));
		DispDeta(sepAst, "");
	}
	DispDeta(sepAst, "");
	DispDeta("", "");
	pf->CloseVoucher();
	dbven.Close();
	ClearMsg();
}

//   Imprime el reporte de ventas por hora.
void PrintZetaHorar()
{
}

//   Imprime el reporte de ventas por artículo.
void PrintZetaPlus()
{
	double tCant, tMonto, uCant, uMonto;

	SendMsg("Imprimiendo Vtas por Producto", "Espere un momento por favor...");
	tCant = tMonto = 0.0;
	PrintClerk();
	pf->OpenVoucher();
	DispDeta("","");
	DispDeta(sepAst,"");
	DispDeta("* INFORME TOTAL DE VENTAS POR PRODUCTO *","");
	DispDeta(sepAst,"");
	rPlu.Go(DB_GO_TOP);
	while(!rPlu.Eof())
	{
		rPlu.Get("cod", tmsg_);
		sprintf(izq, "Cod:%ld", atol(tmsg_));
		rPlu.Get("des", tmsg_);
		DispDeta(izq, tmsg_);
		rPlu.Get("cant", tmsg_); uCant = atof_(tmsg_);
		rPlu.Get("imp", tmsg_);  uMonto = atof_(tmsg_);
		sprintf(izq, "Cant:%.2lf", uCant);
		sprintf(der, "$%.2lf", uMonto);
		DispDeta(izq, der);
		DispDeta("", "");
		tCant += uCant;
		tMonto += uMonto;
		rPlu.Skip(1);
	}
	if(tCant != 0.0 || tMonto != 0)
	{
		DispDeta("TOTAL PRODUCTOS:", "");
		sprintf(izq, "Cant:%.2lf", tCant);
		sprintf(der, "$%.2lf", tMonto);
		DispDeta(izq, der);
	}
	else
		DispDeta("            << Sin Venta >>", "");
	DispDeta(sepAst, "");
	DispDeta("", "");
	pf->CloseVoucher();
	ClearMsg();
	rPlu.Close();
}

//   Imprime un report x o z de la venta diaria registrada aqui.
void mkReport()
{
	int s_hor, s_dep, s_mpag, s_caj, s_vended, s_plus;
	struct DDzeta_ zet;

	WriteBootLog("Ingresando a MKREPORT", BM_MSG);

	//// Chequea que no queden mesas abiertas.
	//if((rMode == 3) && chkNoSusp(0) && !FiscalForceZeta) 	
	//{
	//	Alert("No puede emitir 'Z' en este momento.", "Existen ticket pendientes");
	//	return;
	//}
	borraSusp(0);

	s_hor    = z_horar;			// Salva los flags de listado.
	s_dep    = z_depto;
	s_mpag   = z_mpago;
	s_caj    = z_cajero;
	s_vended = z_vended;
	s_plus   = z_plus;

	z_clrlog = 1;

	if(xReg != 0.0)
	{
		z_cajero = z_mpago = z_depto = z_horar = z_vended = 0;
		z_plus = 0;

		if(     xReg == 111.0) z_cajero = 1;
		else if(xReg == 222.0) z_mpago = 1;
		else if(xReg == 333.0) z_depto = 1;
		else if(xReg == 444.0) z_horar = 1;
		else if(xReg == 555.0) z_vended = 1;
		else if(xReg == 666.0) z_plus = 1;
		else
		{
			Alert("Numero de Informe Desconocido", "");
			z_horar  = s_hor;
			z_depto  = s_dep;
			z_mpago  = s_mpag;
			z_cajero = s_caj;
			z_vended = s_vended;
			z_plus   = s_plus;
			return;
		}
	}

	if(z_horar || z_depto || z_mpago || z_cajero || z_vended || z_plus)
	{
		SendMsg("Espere. Preparando el Informe...","");
		CalculateExpandedZeta(true);
		ClearMsg();

		if(z_depto)
		{
			WriteBootLog("Impriendo zeta DEPTOS", BM_MSG);
			PrintZetaDepto();
		}
		if(z_cajero)
		{
			WriteBootLog("Imprimiendo zeta CAJERO", BM_MSG);
			/// Quitado pf->OpenDrwr();
			PrintZetaCajero();
		}
		if(z_mpago)
		{
			WriteBootLog("Imprimiendo zeta MPAGOS", BM_MSG);
			PrintZetaMpag();
		}
		if(z_horar)
		{
			WriteBootLog("Imprimiendo zeta HORARIA", BM_MSG);
			PrintZetaHorar();
		}

		if(z_vended && ven_enab)
		{
			WriteBootLog("Imprmiendo zeta mozos", BM_MSG);
			if(!z_cajero) pf->OpenDrwr();
			PrintZetaVend(0);
		}
		if(z_plus)
		{
			WriteBootLog("Ingresando zeta PLUS", BM_MSG);
			PrintZetaPlus();
		}

		WriteBootLog("Limpiando RAM usada para REPORTS", BM_MSG);
		if(zz_depto) { FREE(zz_depto); zz_depto = 0; }
		if(zz_mpag)  { FREE(zz_mpag);  zz_mpag = 0; }
		if(zz_vend)  { FREE(zz_vend);  zz_vend = 0; }
	}

	z_horar  = s_hor;
	z_depto  = s_dep;
	z_mpago  = s_mpag;
	z_cajero = s_caj;
	z_vended = s_vended;
	z_plus   = s_plus;

	WriteBootLog("Saliendo si es zeta puntual...", BM_MSG);        
	if(xReg != 0.0)	return;

	WriteBootLog("Imprimiendo ZETA Fiscal", BM_MSG);
	if(rMode > 2)
	{
		WriteBootLog("Borrando Tabla de Cancelados");
		BorraTickCancelados();
		
		SendMsg("Imprimiendo ZETA Fiscal", "");
		WriteBootLog("Imprimiendo ZETA FISCAL", BM_MSG);
		//pf->GeneraReporteAfip(190722,190728);
		NroRetiro = 0;
		WriteCmos(SEQRETIRO);
		zeta = pf->PrintZeta();   // Impresores Fiscales.
		ClearMsg();		

		/* 
		sprintf(msg, "** Reporte Diario (%s)", (rMode == 2)? "X" : "Z");
		sprintf(nro, "Nro: %ld **", zeta);

		DispDeta(sepAst, "");
		DispDeta(msg, nro);
		DispDeta(sepAst, "");

		sprintf(msg, "%12.2lf", tVta);
		DispDeta("Venta Bruta", msg);
		DispDeta("", "");

		for(i=0; i<cTasa; i++)
		{
		sprintf(msg, "%12.2lf", iva[i]);
		DispDeta(iDesc[i], msg);
		sprintf(msg, "%12.2lf", netos[i]);
		DispDeta(vDesc[i], msg);
		DispDeta("", "");
		}
		DispDeta("Recargo NO INSC.", HRng(sobret));

		DispDeta("------------- ANULACIONES --------------", "");
		sprintf(msg, "%ld", cAnula);
		sprintf(msg2, "%12.2lf", tAnula);
		DispDeta(msg, msg2);
		*/

		WriteBootLog("Grabando ZETA en LOG", BM_MSG);

		//  Graba la operacion en el log.
		zet.func = DZeta;
		zet.znro = zeta;
		zet.tnro = ticket;
		zet.fecha = date();
		zet.nro = 0;
		ProcZeta(&zet, 1);

		WriteBootLog("Abriendo Cajeros", BM_MSG);
		OpenAllClerks();
		ultCajero = 0;
		WriteCmos(ULTCAJERO);

		WriteBootLog("Retornando llave a REG", BM_MSG);
		if(!UsaLlave)
		{
			xReg = 0.0;
			superv(0);                                             // Vuelve a REGISTRO.
			if(ckaNro)
			{
				xReg = 0.0;
				clerk(1);											// Desactiva cajero.
			}
		}

		WriteBootLog("Enviando TRANS al SERVER", BM_MSG);
		bool procOk = true;
		if(wlog == 2 || wlog == 3) 
		{
			ForceRemoteDump();
			WriteBootLog("Borrando Tabla de Cancelados");
			BorraTickCancelados();
			procOk = ProcesarOnline();
		}
		if(procOk && (z_clrlog || forceClrLog))
		{
			WriteBootLog("Borrando TRANS actual", BM_MSG);
			ZeroDump();
		}
		else
			WriteBootLog("Salteado borrado de TRANS (opciones.ini)", BM_MSG);

		// Cierra los lotes de operaciones online X.25
		/*if (!AutorTipre)		
			CierreOnline();	*/
		if (tjPrisma)
		{		
			Alberdi::TJOCommon::Prisma::OperarTrxPrisma ^paCerrar = gcnew Alberdi::TJOCommon::Prisma::OperarTrxPrisma(COM_NLD);
			paCerrar->HacerCierre();
			paCerrar = nullptr;
		}

	}
	else
		ResetPOSAcum();

	// Desloguea al supervisor
	superv(0);
}

//   Procesa la zeta sobre el archivo de log.
void ProcZeta(void *zet_, int fwrite)
{
	int i;
	static struct DDzeta_ *zet;

	zet = (struct DDzeta_ *) zet_;
	if(fwrite)
	{
		WriteDump(zet);
		WriteEOPNoFiscal();
	}

	//  Espeja el archivo de transacciones en el server.

	//  Inicializa variables de operacion.
	cDecim = 0;
	inOper = 0;
	ticket++;
	WriteCmos(TICKET);
	tCobro = tVta = 0;
	WriteCmos(TOTVTA);
	for(i=0; i<4; i++)
	{
		netos[i] = 0;
		WriteCmos(NETO_1 + i);
		iva[i] = 0;
		WriteCmos(IVA_1 + i);
		tNetos[i] = tIva[i] = 0.0;
	}
	sobret = 0.0;
	WriteCmos(SOBRET);
	zeta++;
	WriteCmos(ZETA);
	tAnula = 0.0;
	WriteCmos(TANULA);
	cAnula = 0;
	WriteCmos(CANULA);
	HClear();
	xReg = yReg = xCnt  = 0.0;
	xTotal.Clear();
	dRepeat = pSubtot = pRepeat = 0;
	FiscalForceZeta = 0;
}

void subtot_(int);
void subtot(int cual)
{
	GCStatic::LockMainLoop("subtot");
	subtot_(cual);
	GCStatic::ReleaseLockMainLoop("subtot");
}

//   Procesa la funcion que subtotaliza la venta corriente en la pantalla.
void subtot_(int fg)
{
	double AddValue = 0;

	if(rMode > 1)
	{
		mkReport();
		return;
	}

	if(consmode) return;
	if(xTotal.VerTotal() == 0 || xReg + yReg != 0.0)
	{
		Alert("No puede pulsar TOTAL en este momento o", "Ingreso erroneamente un importe antes.");
		return;
	}

	if(!globalCobraCC)
	{
		AddValue = (inOper > 1)? 0: FactuReca(0);
		AddValue += (inOper > 1) ? 0 : FactuPercep(0, false);
		AddValue += (inOper > 1) ? 0 : FactuPercepTissh(0, false);
		AddValue += (inOper > 1) ? 0 : FactuPercepIva(0, false);
	}
	if(printsubt && inOper <= 1 && !globalCobraCC)
	{		
		pf->PrintSubtot("  *SubTotal", 'P');	// Deja PrinterTot - Flag = 'P' para imprimi		

		// Solo puede generarse 1 cvo de error de redondeo por RENGLON en el ticket.
		if((pf->PrinterCap & PRN_SYNC) && (printerTot > 0))
		{
			//   En los tickets factura, puede quedar un pequeño error de   redondeo...
			double tkTotal = xTotal.VerVenta().ToDouble();			// NO se incluyen los recargos
			if(fabs((tkTotal * 100.0) - printerTot) > (int)(xCnt * 2))
			{
				//   Envia un mensaje de LOG.
#ifdef POSDEBUG
				sprintf(dbgStr, "PRNTOT/NAK: Pos=%.3lf, Prn=%.3lf",	tkTotal*100, printerTot);
				DBG(DBGPrintProc, 5, dbgStr);
#endif

				DBG(DBGPrintProc, 5, "Reimprimiendo ticket...");
				//tTmp.Zap();				
				pf->PrintCancel();
				ReprintDump(false);
			}
		}
//		char *tPend = xTotal.VerPendiente().ToString(2);
//		DispDeta("  *SubTotal", tPend);
	}

	//   Imprime los valores del total (posiblemente corregidos).
	PrintTktTot();
//	DispCash(xTotal.VerPendiente().ToDouble(), opcdec);

	char *tTotal = xTotal.VerPendiente().ToString(2);
	CustDisp("Total", tTotal);
	HClear();
	pSubtot = 1;
}

//  Cuando se deprime una tecla de medio de pago, la siguiente funcion
//  se encarga de establecer el total definitivo de la operacion y de
//  imprimirlo.
void PrnTotal()
{
	//FactuReca(1);            // Agrega el NO inscripto
	FactuPercep(1, true);
	FactuPercepTissh(1, true);
	FactuPercepIva(1, true);
	//ProcVuelto();
	//DispDeta("----------------------------------------", "");
	//char *tPend = xTotal.VerPendiente().ToString(2);
	//DispDeta("** TOTAL", tPend);
	WriteTickTmp("", "*****************TOTAL******************", 0,	0, 0, xTotal.VerPendienteDouble(), 0);
}

