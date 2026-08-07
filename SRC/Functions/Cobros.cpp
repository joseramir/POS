#include <stdafx.h>
#include "funcs.h"

// Estructuras globales
struct cobro_ *cobros;
DCobranza_ dump;
extern struct DDmpag_ mpag;
dbf    dbcobro;
int cCobros, elegido, numPago;
double acumPago;

// Menu para elegir el servico
Choose *ServMenuH = NULL;
static GenForm *gf = NULL;
struct cobro_ *cob;
char co_cod[10];
char co_des[25];
char co_ecr[10];
char co_voucher[10];
char co_habil[2];
char tmsg1[500];
char co_medios[100];
char co_datos[300];
char co_ayuda[300];

// Funcion llamada desde el menu cuando se elije una opcion.
int ServFunc(int cual)
{
	elegido = cual - 1;
	return 1;
}

// Prototipo de la funcion de cobranza
void mpago(int);

// Busca la definicion de una cobranza cuyo codigo se indica.
cobro_ *BuscarCobro(int cual)
{
	cobro_ *myc;
	int i;

	for(i = 0, myc = cobros; i < cCobros; i++, myc++)
		if(myc->nro == cual)
			return myc;
	return NULL;
}

void cobrox_(int);
void cobro(int cual)
{
	GCStatic::LockMainLoop("cobro");
	cobrox_(cual);
	GCStatic::ReleaseLockMainLoop("cobro");
}

// Procesa la registracion de cobranzas.
void cobrox_(int cual)
{
	int i;

	// Validaciones de contexto.
	if(consmode) return;			// No funciona en consulta.
	if(rMode > 1) return;			// No funciona en X2 o Z
	if(chkClk()) return;			// Debe existir cajero loguineado.
	if(inOper)						// No debe haber ticket abierto.
	{
		Alert(
			"Las cobranzas de servicios solo se pueden efectuar",
			"en un ticket aparte. Intente despues de cerrar el ticket");
		return;
	}

	// Vuelve a REG, si estaba en x1
	if(rMode)
	{
		double pxReg = xReg;
		xReg = 0.0;
		superv(0);
		xReg = pxReg;
	}

	// Solicita el servicio a cobrar, si no se especifico ninguno en la configuracion del teclado
	if(cual <= 0)
	{
		if(ServMenuH == NULL)
			ServMenuH = new Choose("Elija el Servicio", "", 0, 0, CHOOSE_ALIGN_LEFT, ServFunc);
		ServMenuH->Clear();

		for(i = 1, cob = cobros; i <= cCobros; i++, cob++)
		{
			sprintf(tmsg, "%d - %s", i, cob->nombre);
			ServMenuH->Add(tmsg);
		}
		if(!ServMenuH->Anima()) return;
		cob = &cobros[elegido];
	}
	else
	{
		// Se indico un servicio, lo busca.
		for(i = 0, cob = cobros; i < cCobros; i++, cob++)
		{
			if(cob->nro == cual)
			{
				cual = -1;
				break;
			}
		}
		if(cual >= 0) return;
	}

	// Tenemos en 'cob' el servicio elegido, dispone los otros datos.
	sprintf(tmsg, " Cobranza de '%s' ", Trim(cob->nombre));
	if(gf != NULL) delete gf;
	gf = new GenForm(tmsg);

	// Agrega los items necesesarios para la form.
	EditItem *cobItem = new EditItem("Importe");			// Monto de la cobranza.
	cobItem->CambiarTipo(TIPO_MONEDA);
	gf->Items()->Add(cobItem);
	
	// Agrega los items customs definidos que se le deben pedir al usuario.
	for(int i=0; i < 10; i++)
	{
		if(cob->datos[i] == NULL) break;
		EditItem *nroFacItem = new EditItem(cob->datos[i]);
		nroFacItem->CambiarTipo(cob->tdato[i]);
		STRCPY(nroFacItem->help, cob->helpdato[i]);
		gf->Items()->Add(nroFacItem);
	}

	// Efectua la edicion de los datos y asienta, si no se cancelo.
	if(gf->Animar())
	{
		// Valida que todo este ok.
		for(i = 0; i < gf->Items()->Count(); i++)
		{
			EditItem *ei = (EditItem *)gf->Items()->Item(i);
			if(!ei->valido)
			{
				sprintf(tmsg, "No se ingresaron datos para el '%s'", ei->VerLabel());
				Alert(tmsg, "o el valor ingresado NO es valido.");
				return;
			}
		}
		BDecimal *pTot = BDecimal::Parse(cobItem->dato);
		if(pTot == 0)
		{
			Alert("No se ingreso ningun monto a cobrar", "");
			return;
		}

		// Esta todo OK - Prepara para los siguientes pasos de definicion de la cobranza.
		inOper = 4;
		xTotal.AddVenta(*pTot);
		while(true)
		{
			// Ingresa y edita los medios de pago.
			availPagos = cob->medios;
			forceRebuild = true;
			mpago(-1);
			availPagos = NULL;
			forceRebuild = true;

			// Si las funciones de ingreso de pago funcionarion bien...
			if(gf == NULL) break;

			// No se ingresaron los pagos. Quiere cancelar??
			if(GetSiNo("Desea cancelar la operacion de Cobranza de", "servicios que esta en curso?"))
			{
				delete gf;
				gf = NULL;
				ResetPOSAcum();
				break;
			}

			// Trata de nuevo...
		}

		delete pTot;
	}
}

void cob_step2(struct mpag_ *mp, double montoPago)
{
	int i;
	char *p;
	double vuelto;

	// El monto viene entero.
	xReg /= 100;

	// Graba el trans, si corresponde.
	if(inOper == 4)
	{
		// Pone en el trans los datos adicionales agregados, separados en campos si hace falta.
		STRCPY(tmsg, "");
		for(i = 1; i < gf->Items()->Count(); i++)
		{
			EditItem *ei = (EditItem *)gf->Items()->Item(i);
			if(tmsg[0] != 0) STRCAT(tmsg, ",");
			STRCAT(tmsg, ei->dato);
		}
		
		STRCPY(dump.dato1, "");
		STRCPY(dump.dato2, "");
		STRCPY(dump.dato3, "");

		if(strlen(tmsg) <= 30)
			STRCPY(dump.dato1, tmsg);
		else
		{
			memmove(dump.dato1, tmsg, 30);
			dump.dato1[30] = 0;
			STRCPY(tmsg, tmsg + 30);
			if(strlen(tmsg) <= 30)
				STRCPY(dump.dato2, tmsg);
			else
			{
				memmove(dump.dato2, tmsg, 30);
				dump.dato2[30] = 0;
				STRCPY(tmsg, tmsg + 30);

				tmsg[30] = 0;
				STRCPY(dump.dato3, tmsg);
			}
		}
		
		// Graba en el trans.
		dump.func = DCobranza;
		dump.servicio = cob->nro;
		store(xTotal.VerVenta().ToDouble(), dump.monto);
		dump.fecha = (long) date();
		dump.hora = (long) time_();
		dump.cajero = ultCajero;
		WriteDump(&dump);

		// Construye la info de cobranza para el voucher
		sprintf(tmsg, "Cobranza: %s", cob->nombre);
		WriteTickTmp("", tmsg, 0, 0, 0, xTotal.VerVenta().ToDouble(), false);
		char *strTotal = xTotal.VerVenta().ToString(2);
		VoucherVar("@IMPORTE@", strTotal);
		sprintf(tmsg, "%d", cob->nro);	
		VoucherVar("@NUM_SERV@", tmsg);
		VoucherVar("@NOMBRE_SERV@", cob->nombre);
		
		// Manda al voucher los datos custom.
		for(i = 1; i < gf->Items()->Count(); i++)
		{
			EditItem *ei = (EditItem *)gf->Items()->Item(i);
			sprintf(tmsg, "@%s@", ei->VerLabel());
			_strupr(tmsg);
			for(p = tmsg; *p; p++) if(*p == ' ') *p = '_';
			VoucherVar(tmsg, ei->dato);
		}

		// Pone a cero las variables de voucher de montos de pago
		VoucherVar("@RENG_PAGO1@", "");
		VoucherVar("@RENG_PAGO2@", "");
		VoucherVar("@RENG_PAGO3@", "");
		VoucherVar("@RENG_PAGO4@", "");
		VoucherVar("@RENG_PAGO5@", "");

		// Fuerza a que a posteriori se asienten los medios de cobro pero no el trans.
		acumPago = 0.0;
		numPago = 1;
		inOper = 5;
	}

	sprintf(tmsg1, "@RENG_PAGO%d@", numPago);
	sprintf(tmsg, "%s = $%.2lf", mp->des, xReg);
	VoucherVar(tmsg1, tmsg);

	acumPago += xReg;
	vuelto = acumPago - xTotal.VerVenta().ToDouble();
	numPago++;

	// Graba un trans de medio de cobro e imprime algo en la pantalla.
	WriteTickTmp("", mp->des, 0, 0, 0, xReg, false);
	mpag.func = DMpag;
	mpag.nro = mp->nro;
	mpag.uVuelto = (char)(vuelto > 0.001);
	mpag.uPend = 0;
	mpag._fecha = (long) date();
	mpag._hora = (long) time_();
	mpag.cajero = ultCajero;
	store(0, mpag.mtodesc);
	store(xReg, mpag.importe);
	store(((vuelto > 0.001)? vuelto: 0), mpag.vuelto);
	WriteDump(&mpag);

	// Modifica el dinero en gaveta disponible.
	if(mp->vltocod == mp->nro)
		StoreGaveta(mp, xReg - vuelto);
	else
	{
		struct mpag_ *vmp = BuscarMpag(mp->vltocod);
		if(vmp == NULL) vmp = mp;
		StoreGaveta(vmp, -vuelto);
	}

	// Imprime el voucher
	if(acumPago >= xTotal.VerVenta().ToDouble())
	{
		// Abre la gaveta
		pf->OpenDrwr();

		// Muestra el vuelto.
		DispDeta("Vuelto", HRng(vuelto));
		WriteTickTmp("", "Vuelto", 0, 0, 0, vuelto, false);

		// Imprime los voucher...
		if(cob->voucher != 0)
		{
			PrintClerk();						// Imprime cajero
			sprintf(tmsg, "%.2lf", vuelto);		// Manda el vuelto a variable
			VoucherVar("@VUELTO@", tmsg);

			// Construye e imprime los vouchers
			VoucherVar("@DESTINO@", "******* ORIGINAL PARA EL CLIENTE *******");
			StackVoucher(cob->voucher);			// Original
			VoucherVar("@DESTINO@", "******* DUPLICADO PARA EL CAJERO *******");
			StackVoucher(cob->voucher);			// Duplicado
		}

		// Cierra la operacion en el trans.
		WriteEOPNoFiscal();

		// Limpia todo lo usado.
		HClear();
		delete gf;	
		gf = NULL;
	}
}

// Funcion auxiliar para ordenar los cobros por su codigo
int cobcmp(void const *d1, void const *d2)
{
    return ((struct cobro_ *) d1)->nro - ((struct cobro_ *) d2)->nro;
}

int ConvertTDato(char tdato)
{
	switch(tdato)
	{
		case 'E': return TIPO_ENTERO;
		case 'M': return TIPO_MONEDA;
		case 'R': return TIPO_NUMREF;
	}
	return TIPO_CUALQUIERA;
}

// Carga las definiciones de cobranzas al inicio
int LoadCobros()
{
	 char *p, *p2;

	 cCobros = 0;
	 if(dbcobro.Use("pos\\servicio"))
	 {
		 cobros = new cobro_[dbcobro.RecCount()];
		 dbcobro.Go(DB_GO_TOP);
		 while(!dbcobro.Eof())
		 {
			 dbcobro.Get("servicio", co_cod);
			 dbcobro.Get("nombre", co_des);
			 dbcobro.Get("ecr", co_ecr);
			 dbcobro.Get("voucher", co_voucher);
			 dbcobro.Get("habilitado", co_habil);
			 dbcobro.Get("lmedios", co_medios);
			 dbcobro.Get("datos", co_datos);
			 dbcobro.Get("helpdato", co_ayuda);

			 Trim(co_medios);
			 Trim(co_datos);
			 Trim(co_ayuda);

			 if(co_habil[0] == 'T')
			 {
				 cob = &cobros[cCobros++];
				 cob->nro = atoi(co_cod);
				 STRCPY(cob->nombre, co_des);
				 cob->ecr = atoi(co_ecr);
				 cob->voucher = atoi(co_voucher);
				 cob->habilitado = (co_habil[0] == 'T');

				 if(cob->voucher > 0)
					 if(!LoadVoucher(cob->voucher))
						 cob->voucher = 0;

				 // Parsea los medios de cobro disponibles.
				 int i = 0;
				 for(p2 = p = co_medios; ; p++)
				 {
					 if(*p == 0)
					 {
						 cob->medios[i] = atoi(p2);
						 break;
					 }
					 else if(*p == ',')
					 {
						 cob->medios[i++] = atoi(p2);
						 p2 = p + 1;
					 }
				 }

				 // Parsea los datos adicionales que el cajero debe cargar
				 i = 0;
				 for(p2 = p = co_datos; ; p++)
				 {
					 if(*p == 0)
					 {
						 cob->tdato[i] = ConvertTDato(*p2++);
						 cob->datos[i] = STRDUP(p2, "LoadCobrosDUP");
						 break;
					 }
					 else if(*p == ',')
					 {
						 *p = 0;
						 cob->tdato[i] = ConvertTDato(*p2++);
						 cob->datos[i++] = STRDUP(p2, "LoadCobrosDUP2");
						 p2 = p + 1;
					 }
				 }

				 // Parsea las ayudas para los datos adicionales. DEBE haber una para cada
				 // dato adicional definido.
				 i = 0;
				 for(p2 = p = co_ayuda; ; p++)
				 {
					 if(*p == 0)
					 {
						 cob->helpdato[i] = STRDUP(p2, "LoadCobrosDUP3");
						 break;
					 }
					 else if(*p == '|')
					 {
						 *p = 0;
						 cob->helpdato[i++] = STRDUP(p2, "LoadCobrosDUP4");
						 p2 = p + 1;
					 }
				 }
			 }

			 dbcobro.Skip(1);
		 }
		 dbcobro.Close();
	 }
	 else
		 AbnormalEnd("No puedo cargar descripcion de cobranzas de servicios.");

	 if(cCobros)
		qsort(cobros, cCobros, sizeof(struct cobro_), cobcmp);
     return 1;
}

