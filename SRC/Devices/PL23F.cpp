#include <STDAFX.H>



// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//using namespace std;


//extern "C" {
//	PrinterPL23F PL23F;
//};


// Retorna datos del propietario del impresor
void PrinterPL23F::Owner(char *cuit, char *pv)
{	
	SetHeader("");
	SetPie("");
	while(true)
	{
		xchgPkt("\x73");
		if(chkFiscStat()) continue;
		if(chkPrnStat()) continue;
		break;
	}

	// Extrae los datos que necesitamos
	pc = rxBuff + 1;
	getfld(&pc); //status de la impresora
	getfld(&pc); //status Fiscal
	STRCPY(cuit, getfld(&pc));
	if(strlen(cuit) >= 11)
	{
		getfld(&pc); //razon Social
		getfld(&pc); //Numero de Registro
		getfld(&pc); //Fecha de inicializacion
		STRCPY(pv, getfld(&pc));
		STRCPY(ptovta, pv);
	}
	else
	{
		cuit[0] = '2'; cuit[1] = '0';
		cuit[2] = '1'; cuit[3] = '2'; cuit[4] = '3'; cuit[5] = '4' ;cuit[6] = '5'; cuit[7] = '6'; cuit[8] = '7'; cuit[9] = '8';
		cuit[10] = '6'; cuit[11] = 0;
		pv[0] = pv[1] = pv[2] = '0'; pv[3] = '1'; pv[4] = 0;
	}
	return;
}

//  Manda al impresor algun comando de configuracion.
void PrinterPL23F::Configure(char *confParam)
{
	char tmpPkt[50];
	if (strlen(confParam) > 40)
		confParam[40] = 0;
	sprintf(tmpPkt, "\x64%c7%c%s", SEP, SEP, confParam);
	xchgPkt(tmpPkt);
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterPL23F::ChkPrnXonXoff()
{
	while(chk() == 1)
        ;
	return 0;
}

void PrinterPL23F::CheckStatus()
{
	xchgPkt("\x2A");
	chkFiscStat();
}

// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1 = ticket b/c factura b/c
//    tipo: 2 = ticket 'a' o factura 'a'
int PrinterPL23F::GetTickNro(int cual)
{
	// Envia el comando.
	ignoraError4 = 1;
	sprintf(txbuff, "\x2A", SEP);
	int result = xchgPkt(txbuff);
	ignoraError4 = 0;

	//   Desensambla los distintos campos del estado.
	if(result)
	{
		pc = rxBuff + 1;
		gethexa(&pc);					// prnstat
		gethexa(&pc);					// fiscstat
		_ultimoBcompleto = getdec(&pc);	// Nro tick B/C
		gethexa(&pc);					// Estado auxiliar
		_ultimoAcompleto = getdec(&pc);	// Nro tick A
	}

	// Retorna el numeo pedido
	if(cual == 1)					// GlobalTF -->  0=CF, 1=FA, 2=FB
		return _ultimoAcompleto;
	else
		return _ultimoBcompleto;
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispositivo presente 
// no se contempla en esta impresora.
int PrinterPL23F::OpenDrwr()
{
	//xchgPkt("\x7b");
	return 0;
}

// Completa la inicializacion que es especifica del modelo.
void PrinterPL23F::CompleteInitialization()
{
	// Envia el comando.	
	sprintf(txbuff, "\x96");
	int result = xchgPkt(txbuff);

	//   Desensambla los distintos campos del estado.
	if(result)
	{
		chkPrnStat();
		pc = rxBuff + 1;
		gethexa(&pc);					// prnstat
		gethexa(&pc);					// fiscstat
		gethexa(&pc);					// limite obligacion Datos Consumidor Final
		//limTicket = getdec(&pc);	// Limite Tique-Factura (No relevante)
		
	}	
}

char *PrinterPL23F::HasarBrand()
{
	return "Espere. Cargando el impresor HASAR SMH/PL-23F...";
}

//   Descarga en el printer todos los datos instalables.
int PrinterPL23F::PrinterUpLoad()
{
	int i;
	abierto = false;

    //  Activa las capacidades del impresor.
    PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY;
	solofact = true;
	prndelay = 1; //solo imprime al final

    //  Mensaje!!!
    SendMsg(HasarBrand(), "");
    ChkPrnXonXoff();

	//  Cancela cualquier ticket que el impresor tenga en memoria
	WriteBootLog("Cancelando documentos abiertos si hubiera");
	sprintf(txbuff, "\x98");
	xchgPkt(txbuff);

	sprintf(txbuff, "\x5F%c%d%c%s", SEP, 0, SEP, "\x7f");
	xchgPkt(txbuff);	
	
	//Configura la seccion Nombre de Fantasia, son dos lineas
	for(i=0; i<2; i++)
	{
		sprintf(txbuff, "\x5F%c%d%c%s", SEP, i+1, SEP, ChkDes(Trim(LogoLines[i]),0));
		xchgPkt(txbuff);
	}

	sprintf(txbuff, "\x5D%c%d%c%s", SEP, 0, SEP, "\x7f");
	xchgPkt(txbuff);
	//Primero, instala los encabezamientos. Son 5 lineas se corresponden con datos de Ing. Brutos (2 lineas) y propaganda (3 lineas)
	for(i=2; i<cLinesLogo; i++)
	{
        sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i-1, SEP, ChkDes(Trim(LogoLines[i]),0));
		xchgPkt(txbuff);
	}
	availHeaderLine = i;
	//while(i <= 11)              // Vacia las lineas no usadas.
	//{	
	//	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i-1, SEP, "\x7f");
	//	xchgPkt(txbuff);
	//	i++;
	//}

	//  Ahora, instala las definiciones de PIE de ticket.
	for(i = 0; i < 4; i++) //i<cLinesPie; i++) fijo con 4 lineas para el Trailer (pie) de factura
	{
        sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+11, SEP, ChkDesWithLen(LogoPie[i],0, 120));
		xchgPkt(txbuff);
	}
	
	// Programa la fecha y hora
	sprintf(txbuff, "\x58%c%06d%c%06d", SEP, date()-20000000, SEP, time__());
	xchgPkt(txbuff);

	CompleteInitialization();
    ClearMsg();
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPL23F::SetPie(char *msg)
{
	char tmsg[120];

	//ChkDes(msg, tmsg);
	ChkDesWithLen(msg, tmsg, 120);
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(tmsg) == 0)? " " : tmsg);
	xchgPkt(txbuff);
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPL23F::SetPie(char *msg, int linea)
{
	char tmsg[120];

	//ChkDes(msg, tmsg);
	ChkDesWithLen(msg, tmsg, 120);
	switch (linea)
	{
		case 1:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(tmsg) == 0)? " " : tmsg);
			break;
		case 2:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 13, SEP, (strlen(tmsg) == 0)? " " : tmsg);			
			break;
		case 3:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 14, SEP, (strlen(tmsg) == 0)? " " : tmsg);			
			break;
		default:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(tmsg) == 0)? " " : tmsg);
			break;
	}

	xchgPkt(txbuff);
	pieChanged = true;
	return 0;
}

//  Cambia el mensaje de la ultima linea del HEADER del ticket.
// En realidad esta cambiando el trailer
int PrinterPL23F::SetHeader(char *msg)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	if(availHeaderLine > 10) availHeaderLine = 10;	
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(tmsg) == 0)? " ": tmsg);	
	xchgPkt(txbuff);
	return 0;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
int PrinterPL23F::PrintEncab()
{
    char hdrPkt[400], *p;
	int validarCuit = 1;	

	TipoComprobante = EsNotaCredito ? 110 : 83;//int tipocomp = 83;//Ticket es el comprobante por defecto

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
    PrintClerk();               // Imprime los datos del cajero.
	SetPie("");
    if(GlobalTF)
    {        
		STRCPY(c_cuit, c_numero);
        if(!_stricmp(c_condiva, "RI"))
        {
            f_letra =  'A';
            f_respon = 'I';
			TipoComprobante = EsNotaCredito ? 112 : 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "NI"))
        {
            f_letra =  'A';
            f_respon = 'N';
			TipoComprobante = EsNotaCredito ? 112 : 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "EX"))
        {
            f_letra =  'B';
            f_respon = 'E';
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
        }
        else if(!_stricmp(c_condiva, "NR"))
        {
            STRCPY(c_cuit, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'A';
			validarCuit = 0;
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
        }
        else if(!_stricmp(c_condiva, "MO"))
        {
            f_letra =  'B';
            f_respon = 'M';
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
        }
        else        // Consumidor final.
        {
            //STRCPY(c_numero, "20-11111111-2");			
			if(strlen(c_cuit) == 0 || strlen(c_cuit) > 8)
				STRCPY(c_cuit, "11111111");
            f_letra =  'B';
            f_respon = 'C';
			validarCuit = 0;
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
        }

		//   Corrige la variable GlobalTF de acuerdo con el comprobante abierto
		GlobalTF = (f_letra == 'A')?1 : 2;

        //   Prepara el cuit.		
        Trim(c_cuit);
        for(p = c_cuit; *p; )
            if(*p == '-')
                STRCPY(p, p+1);
            else
                p++;
		/*Trim(c_numero);
		for(p = c_numero; *p; )
			if(*p == '-')
				STRCPY(p, p+1);
			else
				p++;*/
		if(validarCuit && !vCuit(c_numero))
		{
			Alert("El CUIT del cliente no es valido.", "Se emitira una Factura B...");
			GlobalTF = 2;
			STRCPY(c_cuit, "11111111");
			f_respon = 'C';
			PrepareCustomerData(hdrPkt);
			sprintf(hdrPkt, "\x40%cB%cT", SEP , SEP);
			xchgPkt(hdrPkt);           // Abre ticket fiscal comun
			STRCPY(GlobalComprob, "FB");
		}
		else
		{
			//   Prepara el resto de los campos.
			//STRCPY(ncliente, c_nombre);
			ChkDes(c_nombre, NULL);
			ChkDes(c_direcc, NULL);
			Trim(c_nombre);			
			Trim(c_direcc);
			Trim(c_localidad);
			Trim(c_telef);

			//   Manda los datos del cliente.
			PrepareCustomerData(hdrPkt);
			if((lstat.fiscstat & 0x8000) != 0)
			{
				sprintf(hdrPkt, "(%s) - DATOS NO VALIDOS DEL CLIENTE", _itoa(lstat.fiscstat, msg, 10));
				Alert(hdrPkt, "Se imprimira Factura B");
				STRCPY(c_cuit, "11111111");
				f_respon = 'C';
				PrepareCustomerData(hdrPkt);
				sprintf(hdrPkt, "\x40%cB%cT", SEP, SEP);
				STRCPY(GlobalComprob, "FB");
			}
			else
			{
				sprintf(hdrPkt, "\x40%c%c%cT", SEP, f_letra, SEP);
				GlobalComprob[0] = 'F';
				GlobalComprob[1] = f_letra;
				GlobalComprob[2] = 0;
			}
			xchgPkt(hdrPkt);			
		}
    }
    else
    {
        sprintf(hdrPkt, "\x40%cT%cT", SEP , SEP);
        xchgPkt(hdrPkt);           // Abre ticket fiscal comun
		STRCPY(GlobalComprob, "TI");
    }
	inOper = 1;
	abierto = true;
    return 0;
}

void PrinterPL23F::PrepareCustomerData(char *hdrPkt)
{
	char *ccliente = new char[80];
	char *cdirecc = new char[80];
	STRCPY(ccliente, c_numero);
	STRCAT(ccliente, "-");
	STRCAT(ccliente, strlen(c_nombre) == 0 ? "Sin Nombre": c_nombre);
	STRCPY(cdirecc, c_direcc);
	STRCAT(cdirecc, " - ");
	STRCAT(cdirecc, c_localidad);
	if(strlen(ccliente) > LENCUSDATA) 
		ccliente[LENCUSDATA - 1] = 0;	
	if (strlen(cdirecc) > LENCUSDATA)
		cdirecc[LENCUSDATA - 1] = 0;
	else if (strlen(cdirecc) == 3)
		STRCPY(cdirecc, "Sin Direccion");
	sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s" ,SEP, ccliente, SEP, c_cuit,	SEP, f_respon, SEP, f_respon=='C'? '2': 'C',SEP, cdirecc);
	//sprintf(hdrPkt,   "\x62%c%s%c%s%c%c%c%c" ,SEP, c_nombre, SEP, c_numero,	SEP, f_respon, SEP, vCuit(c_numero) ? '2': 'D');//f_respon=='C'? ' ': 'C');
	xchgPkt(hdrPkt);
	delete ccliente;
	delete cdirecc;
}

void PrinterPL23F::PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto)
{
	if(cajaBulto < 1) 
		cajaBulto = 1;
	if (strlen(tdes) > LENDESITEM)
		tdes[LENDESITEM - 1] = 0;
	if(qual == 'D')
	{
		sprintf(txbuff, "m%c%s%c%.2lf%c%.2lf%c%c%c0.00000%c0%cT%cB" ,
			SEP, tdes, SEP, monto, SEP, xtasa, SEP, 'm', SEP, SEP, SEP, SEP);
	}
	else
	{		
		sprintf(txbuff, "B%c%s%c%.6lf%c%.2lf%c%02.2lf%c%c%c0.0%c0%cT" ,
            SEP, tdes, SEP, mult * cajaBulto, SEP, monto, SEP, xtasa, SEP, qual, SEP, SEP, SEP);		
	}
}

//   Imprime un item de linea.
int PrinterPL23F::PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto, int variac, char *ades, 
						  double unitario, bool pesado, int cajaBulto)
{
    char tdes[100];	
    //unsigned maxLen;
	char multLine[60];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

    //maxLen = GlobalTF? 40: 20;	
    ChkDesWithLen(des, tdes, 50);	
    ChkDesWithLen(ades, NULL, 50);

	// Ajusta parametros
	double xtasa = ((double)tasa)/100;	
	//   Abre el comprobante, si corresponde.
	while (true)
	{
		if (!abierto)
			PrintEncab();
		if (abierto)
			break;
	}

	//   Envia el comando al impresor.
	for(int loop=0; loop <= 1; loop++)
	{  
		//   Imprime la descripcion adicional del articulo.
		if(strlen(ades) != 0)					
			strcat(tdes, ades);
		
		double lounitario = monto;		
		if ((fabs(mult - 1.000) > 0.001))
		{
			lounitario = unitario;
			if (pesado)			
				mult = monto / unitario;
		}			
		else if (qual == 'D' || cajaBulto > 1)
			lounitario = unitario;		

		//   Adapta el calificador de la operacion.		
		PrepareCommand(txbuff, tdes, mult, lounitario, xtasa, qual, cajaBulto);

		//	 Prepara el paquete a enviar.
		//   Envia los datos concretos del articulo. Verifica errores tipicos.
		xchgPkt(txbuff);
		if(chkPrnStat())  continue;
		if(chkFiscStat()) continue;
		
		break;
	}
    
	proteClose = time__();
	return 0;
}
//   Imprime y chequea el subtotal de la operacion.
int PrinterPL23F::PrintSubtot(char *msg, int pFlag)
{
	char tdes[30];

	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, tdes);

	sprintf(txbuff, "\x43%c%c%c%s%c0", SEP, pFlag, SEP, tdes, SEP);
	xchgPkt(txbuff);
	char *pc = rxBuff + 1;

	if(chkFiscStat()) 
	{
		printerTot = 0;
		return 0;
	}
	getfld(&pc);								// Estado de la impresora.
	getfld(&pc);								// Estado fiscal.
	getfld(&pc);								// Cuenta items de linea.
    printerTot = 100 * atof_(getfld(&pc));		// Total de mercaderia.
	return 0;
}

//   Envia un texto fiscal al ticket.
int PrinterPL23F::PrintText(char *msg)
{
	char tdes[50];

	ChkDes(msg, tdes);	
    if(!inOper) 
		PrintEncab();
	if (strlen(tdes) > LENDESITEM)
		tdes[LENDESITEM] = 0;

    sprintf(txbuff2, "\x41%c%s%c0", SEP, tdes, SEP);
	xchgPkt(txbuff2);
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPL23F::PrintMPago(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDesWithLen(msg, tdes, 50);
    sprintf(txbuff, "\x44%c%s%c%.2lf%cT%c0", SEP, tdes, SEP, monto, SEP, SEP);
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPL23F::PrintDescto(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(strcmp(msg, "Ajuste por redondeo") == 0)
		return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDesWithLen(msg, tdes, LENDESITEM);
	sprintf(txbuff, "\x54%c%s%c%.2lf%cm%c0%cC", SEP, tdes, SEP, monto, SEP, SEP, SEP);
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

int PrinterPL23F::PrintClose()
{
	if(proteClose == time__()) proteClose++;
	while(proteClose > time__())
		tSwitch();

	xchgPkt3("\x45");
	abierto = false;
	if (pieChanged)
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, LogoPie[1]);	
		xchgPkt(txbuff);
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 13, SEP, LogoPie[2]);
		xchgPkt(txbuff);	
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 14, SEP, LogoPie[3]);
		xchgPkt(txbuff);		
	}
	pieChanged = false;	
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterPL23F::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	/*sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'd', SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);*/
	sprintf(txbuff, "\x44%cCancela%c0.0%cC%c0", SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);
	abierto = false;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterPL23F::PrintZeta()
{
    sprintf(txbuff, "\x39%cZ", SEP);
	xchgPkt(txbuff);
	char *pc = rxBuff;
	getfld(&pc);
	getfld(&pc);
	getfld(&pc);
	return atoi(getfld(&pc));
}


//   Ordena al impresor que abra un ticket no fiscal.
int PrinterPL23F::OpenVoucher()
{
	xchgPkt("\x48");
	GlobalVoucherFlag = 1;
	return 0;
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterPL23F::OpenVoucher2()
{
	OpenVoucher();
	return 0;
}

//   Imprime un texto en un comprobante NO fiscal.
int PrinterPL23F::PrintVoucher(char *des)
{	
	char *itera = des;
	int i = 0; int j = 0;
	int tlargo = strlen(des);
	char aux[120];
	strcpy(aux, des);
	bool cambio = false;
	aux[0] = 0;
	i = j = 0;
	while (i < tlargo)// (itera[i] != '\0')
	{
		if (itera[i] == '&' && itera[i+1] == '4')
		{
			i++;
			i++;
			cambio = true;
			continue;
		}
		else		
		{
			aux[j] = itera[i];			
		}		
		i++; j++;
	}	
	if (cambio)
	{
		aux[j] = 0;
		ChkDesVoucher(aux, NULL);
		if (strlen(aux) == 0)
			sprintf(txbuff, "\x49%c%c%s%c0", SEP, '\xf4', " ", SEP);
		else
			sprintf(txbuff, "\x49%c%c%s%c0", SEP, '\xf4', aux, SEP);
	}else
	{
		ChkDesVoucher(des, NULL);
		if (strlen(aux) == 0)
			sprintf(txbuff, "\x49%c%s%c0", SEP, " ", SEP);
		else
			sprintf(txbuff, "\x49%c%s%c0", SEP, des, SEP);
	}
    
	xchgPkt(txbuff);
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPL23F::CloseVoucher()
{
	xchgPkt("\x4a");
    GlobalVoucherFlag = 0;
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPL23F::CloseVoucher2()
{
	CloseVoucher();
	return 0;
}

//   Chequea los bits de estado del controlador fiscal y muestra los mensajes
//   adecuados, de corresponder.
int PrinterPL23F::chkFiscStat()
{
	char *msg;

	if((lstat.fiscstat & 0x81) || (lstat.fiscstat & 0x80))
		msg = "Memoria Fiscal Llena";
	else if(lstat.fiscstat & 0x1)
		msg = "Error al Verificar la Memoria Fiscal";
	else if(lstat.fiscstat & 0x2)
		msg = "Error al Verificar la Memoria de Trabajo (CMOS)";
    else if(lstat.fiscstat & 0x8)
        msg = "Comando Desconocido";
	else if(lstat.fiscstat & 0x10)
		msg = "Datos no Validos en un campo";
	/*else if(lstat.fiscstat & 0x20)
		msg = "Comando Invalido en Estado Fiscal Actual";*/
    else if(lstat.fiscstat & 0x40)
		msg = "Monto Total excede lo permitido por el impresor";
    else if(lstat.fiscstat & 0x80)
        msg = "Memoria fiscal llena, bloqueada o dada de baja";
	else if(lstat.fiscstat & 0x100)
		msg = "Memoria fiscal CASI llena. Avise al Tecnico";
	else
	{		
		return 0;
	}
    #ifdef INPOS
	Alert("Error Fiscal en la Impresora", msg);
    #else
    printf("Error Fiscal en la Impresora!!!\n");
    #endif
	return 1;
}

//   Chequea los bits de estado del impresor fiscal y muestra los mensajes
//   orientativos que correspondieren.
int PrinterPL23F::chkPrnStat()
{
	char *msg;
	int adevolver = -1;
		
	if(lstat.prnstat & 0x4)
	{
		msg = "Error / Falla de Impresora"; //se ha interrumpido la conexion entre el controlador fiscal y la impresora
		adevolver = 1;
	}
    else if(lstat.prnstat & 0x08)
	{
		msg = "Impresora Fuera de Linea (OFFLINE)";//la impresora no ha logrado comunicarse dentro le periodo de tiempo establecido
		adevolver = 1;
	}
    else if(lstat.prnstat & 0x10)
		msg = "Falta Papel en la Cinta Testigo";//El sensor de papel diario ha detectado falta de papel
    else if(lstat.prnstat & 0x20)
		msg = "Falta Papel en el Ticket";//El sensor de papel de tiques ha detectado falta de papel
	else if(lstat.prnstat & 0x40)
	{
		msg = "Buffer de impresora lleno";
		adevolver = 1;
	}
    else if(lstat.prnstat & 0x100)
        msg = "Tapa del impresor abierta o mal cerrada";
	else adevolver = 0;
    if (adevolver != 0)
		Alert("Error en la Impresora", msg);
	if(adevolver < 0)
		adevolver = 0;
    
	return adevolver;
}


// Imprime la percepcion en la impresor fiscal
int PrinterPL23F::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
	int i = 0;
	int tverror = 0;
	char auxmonto[10];
	char tvaux[70];
	char hdrPkt[500];	
	char sep[2]; 
	int result = 0;

	if (permonto < 0.01)
		return result;
	if(strcmp(perdescrip, "Ajuste por redondeo") == 0)
		return result;
	if (!docFiscStat()) //si no existe un documento fiscal abierto no se imprimira la percepcion
		return result;
	if (strlen(perdescrip) > 20)
		perdescrip[20] = 0;
	ChkDesWithLen(perdescrip, NULL, 20);

	sprintf(auxmonto, "%.2lf", permonto);
	sep[0] = SEP;
	sep[1] = 0;
	while(true)
	{
		STRCPY(hdrPkt, "\x60");							STRCAT(hdrPkt, sep);
		STRCAT(hdrPkt, "**.**");						STRCAT(hdrPkt, sep);		// "S"
		STRCAT(hdrPkt, perdescrip);					STRCAT(hdrPkt, sep);		
		STRCAT(hdrPkt, auxmonto);

		xchgPkt(hdrPkt);
		bool fallo = (chkFiscStat2() != 0);
		if (!fallo)
		{
			result = 1;
			break;
		}
		else if (i < 3)
		{
			sprintf(tvaux, "ErrorPercep en impresion: %d", tverror);
			WriteBootLog(tvaux);
			i++;
		}else
			break;		
	}
	return result;
}

//   Chequea los bits de estado del controlador fiscal y muestra los mensajes
//   adecuados, de corresponder.
int PrinterPL23F::chkFiscStat2()
{
	char *msg;

	if((lstat.fiscstat & 0x81) || (lstat.fiscstat & 0x80))
		msg = "Memoria Fiscal Llena";
	else if(lstat.fiscstat & 0x1)
		msg = "Error al Verificar la Memoria Fiscal";
	else if(lstat.fiscstat & 0x2)
		msg = "Error al Verificar la Memoria de Trabajo (CMOS)";
    else if(lstat.fiscstat & 0x8)
        msg = "Comando Desconocido";
	else if(lstat.fiscstat & 0x10)
		msg = "Datos no Validos en un campo";
	else if(lstat.fiscstat & 0x20)
		msg = "Comando Invalido en Estado Fiscal Actual";
    else if(lstat.fiscstat & 0x40)
		msg = "Monto Total excede lo permitido por el impresor";
    else if(lstat.fiscstat & 0x80)
        msg = "Memoria fiscal llena, bloqueada o dada de baja";
	else if(lstat.fiscstat & 0x100)
		msg = "Memoria fiscal CASI llena. Avise al Tecnico";
	else
	{		
		return 0;
	}
    #ifdef INPOS
	Alert("Error Fiscal en la Impresora", msg);
    #else
    printf("Error Fiscal en la Impresora!!!\n");
    #endif
	return 1;
}


void PrinterPL23F::VoucherTJOnline(char *marca, char *tarjeta, char *nombre, int vencimiento, int lote,
					 int cupon, int autorizacion, double monto, int cuotas, char *numcomercio, int numterminal,
					 int tickfiscal, bool credito)
{
	char *aux = new char[10];
	char *inter = new char[10];
	char *atarjeta = new char[20];
	sprintf(aux, "%04d", vencimiento);
	STRCPY(atarjeta, tarjeta);
	for (int i = 0; i < 12; i++)
	{
		atarjeta[i] = '*';
	}
	inter[0] = aux[2], inter[1] = aux[3]; inter[2] = aux[0]; inter[3] = aux[1]; inter[4] = 0;
	sprintf(txbuff, "\x6a%c%s%c%c%s%c%c%c%s%c%s%c%c%c%02d", SEP, nombre, SEP, '\xf4', marca, SEP, 'C',SEP, atarjeta, SEP, inter, SEP, (credito ? 'C' : 'D'),SEP, cuotas);	
	xchgPkt(txbuff);
	sprintf(txbuff, "\x6b%c%s%c%d%c%d%c%d%c%c%c%c%c%d%c$ %.2lf%c%d%c%s", SEP, numcomercio, SEP, numterminal, SEP, lote ,SEP, cupon, SEP, ' ', SEP, 'N', SEP, autorizacion, SEP, monto,SEP,tickfiscal, SEP, clkName());	
	xchgPkt(txbuff);
	sprintf(txbuff, "\x6c%c%d", SEP, 2);
	xchgPkt(txbuff);	
	delete [] aux; delete [] inter; delete [] atarjeta;
}

char *PrinterPL23F::ChkDesWithLen(char *s, char *dst, int maxLen)
{
	int locallen = maxLen;
	if(dst == NULL && strlen(s) == 0) return s;
	if(dst == NULL) dst = s;
	char *ret = dst;
	//int maxLen = 41;
	while(*s && (maxLen-- != 0))
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			if( *(s + 1) == '4')
			{
				*dst++ = '\xf4';
				s++;
			}
		}
		else
		{
			if(*s < ' ' || *s > 'z' && (*s != '[' && *s != ']'))
				*dst++ = ' ';
			else
				*dst++ = *s;
		}
		s++;
	}

	if(strlen(ret) > locallen) *(ret + locallen) = 0;
	*dst = 0;
	return ret;
}

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
char *PrinterPL23F::ChkDesVoucher(char *s, char *dst)
{
	if(dst == NULL && strlen(s) == 0) return s;
	if(dst == NULL) dst = s;
	char *ret = dst;
	int maxLen = 120;
	while(*s && (maxLen-- != 0))
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			if( *(s + 1) == '4')
			{
				*dst++ = '\xf4';
				s++;
			}
		}
		else
		{
			if(*s < ' ' || *s > 'z' && (*s != '[' && *s != ']'))
				*dst++ = ' ';
			else
				*dst++ = *s;
		}
		s++;
	}

	if(strlen(ret) > 120) *(ret + 120) = 0;
	*dst = 0;
	return ret;
}

int PrinterPL23F::docFiscStat()
{
	int valreturn = 0;
	if (lstat.fiscstat & 0x1000) 			
		valreturn = 1;
	
	return valreturn;
}
