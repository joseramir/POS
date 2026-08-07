/*
 *   PPR4.c
 *   Driver del impresor HASAR PR4F Fiscal.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

using namespace std;

/*   Datos de este solo modulo   */
//static struct lstat_ lstatPR4;
//static char *pc;                // Para explorar respuestas.
////static int inTickPR4;           // Indica que el PR4 tiene ticket abierto.
//long _ultimoBcompleto, _ultimoAcompleto;

//extern "C" {
//	PrinterPR4 P_PR4;
//};


// Retorna datos del propietario del impresor
void PrinterPR4::Owner(char *cuit, char *pv)
{
	SetHeader("");
	SetPie("");
	while(true)
	{
		xchgPkt("s");
		if(chkFiscStat()) continue;
		if(chkPrnStat()) continue;
		break;
	}

	// Extrae los datos que necesitamos
	pc = rxBuff + 1;
	getfld(&pc);
	getfld(&pc);
	STRCPY(cuit, getfld(&pc));
	if(strlen(cuit) >= 11)
	{
		getfld(&pc);
		getfld(&pc);
		getfld(&pc);
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
void PrinterPR4::Configure(char *confParam)
{
	char tmpPkt[50];
	if (strlen(confParam) > 40)
		confParam[40] = 0;
	sprintf(tmpPkt, "d%c7%c%s", SEP, SEP, confParam);
	xchgPkt(tmpPkt);
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterPR4::ChkPrnXonXoff()
{
	while(chk() == 1)
        ;
	return 0;
}

void PrinterPR4::CheckStatus()
{
	xchgPkt("*");
	chkFiscStat();
}

// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1 = ticket b/c factura b/c
//    tipo: 2 = ticket 'a' o factura 'a'
int PrinterPR4::GetTickNro(int cual)
{
	// Envia el comando.
	ignoraError4 = 1;
	sprintf(txbuff, "*", SEP);
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
//   en el impresor PR4 para activarla.
int PrinterPR4::OpenDrwr()
{
	xchgPkt("\x7b");
	return 0;
}

// Completa la inicializacion que es especifica del modelo.
void PrinterPR4::CompleteInitialization()
{
	double llimfiscal = 0;
	// Envia el comando.	
	sprintf(txbuff, "\x66");
	int result = xchgPkt(txbuff);

	//   Desensambla los distintos campos del estado.
	if(result)
	{
		pc = rxBuff + 1;
		gethexa(&pc);					// prnstat
		gethexa(&pc);					// fiscstat
		gethexa(&pc);					// limite obligacion Datos Consumidor Final
		llimfiscal = getdec(&pc);	// Limite Tique-Factura
		sprintf(txbuff, "Limite Fiscal: %.2lf", llimfiscal);
		WriteBootLog(txbuff);
		if (checkLimFisc)	
			limTicket = llimfiscal;
		else
		{
			if (limTicket > llimfiscal)
				limTicket = llimfiscal;
		}
		txbuff[0] = 0;
	}	
}

char *PrinterPR4::HasarBrand()
{
	return "Espere. Cargando el impresor HASAR 615/272F/PR4...";
}

//   Descarga en el printer todos los datos instalables.
int PrinterPR4::PrinterUpLoad()
{
	int i;
	abierto = false;

    //  Activa las capacidades del impresor.
    PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY;	

    //  Mensaje!!!
    SendMsg(HasarBrand(), "");
    ChkPrnXonXoff();

	//  Cancela cualquier ticket que el impresor tenga en memoria
	/*sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'd', SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);
	sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'D', SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);*/
	WriteBootLog("Cancelando documentos abiertos si hubiera");
	sprintf(txbuff, "\x98");
	xchgPkt(txbuff);

	//  Primero, instala los encabezamientos.
	for(i=0; i<cLinesLogo; i++)
	{
        sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+1, SEP, ChkDes(Trim(LogoLines[i]),0));
		xchgPkt(txbuff);
	}
	availHeaderLine = i;
	while(i <= 11)              // Vacia las lineas no usadas.
	{	
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+1, SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}

	//  Ahora, instala las definiciones de PIE de ticket.
	for(i=0; i<cLinesPie; i++)
	{
        sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+11, SEP, ChkDes(LogoPie[i],0));		
		xchgPkt(txbuff);
	}
	while(i <= 8)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+11, SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}

	// Programa la fecha y hora
	sprintf(txbuff, "X%c%06d%c%06d", SEP, date()-20000000, SEP, time__());
	xchgPkt(txbuff);

	CompleteInitialization();
    ClearMsg();
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPR4::SetPie(char *msg)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(tmsg) == 0)? " " : tmsg);
	xchgPkt(txbuff);	
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPR4::SetPie(char *msg, int linea)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	switch (linea)
	{
		case 1:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(tmsg) == 0)? " " : tmsg);
			break;
		case 2:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(tmsg) == 0)? " " : tmsg);			
			break;
		case 3:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 13, SEP, (strlen(tmsg) == 0)? " " : tmsg);			
			break;
		default:
			sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(tmsg) == 0)? " " : tmsg);
			break;
	}
	
	xchgPkt(txbuff);
	pieChanged = true;
	return 0;
}


//  Cambia el mensaje de la ultima linea del HEADER del ticket.
int PrinterPR4::SetHeader(char *msg)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	if(availHeaderLine > 10) availHeaderLine = 10;
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, availHeaderLine, SEP, (strlen(tmsg) == 0)? " ": tmsg);
	xchgPkt(txbuff);
	return 0;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
int PrinterPR4::PrintEncab()
{
    char hdrPkt[400], *p;
	int validarCuit = 1;
	//char ncliente[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
    PrintClerk();               // Imprime los datos del cajero.
	SetPie("");
    if(GlobalTF)
    {
        //STRCPY(tDoc, "CUIT");
		  STRCPY(c_cuit, c_numero);
        if(!_stricmp(c_condiva, "RI"))
        {
            f_letra =  'A';
            f_respon = 'I';
        }
        else if(!_stricmp(c_condiva, "NI"))
        {
            f_letra =  'A';
            f_respon = 'N';
        }
        else if(!_stricmp(c_condiva, "EX"))
        {
            f_letra =  'B';
            f_respon = 'E';
        }
        else if(!_stricmp(c_condiva, "NR"))
        {
            STRCPY(c_numero, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'A';
			validarCuit = 0;
        }
        else if(!_stricmp(c_condiva, "MO"))
        {
            f_letra =  'B';
            f_respon = 'M';
        }
        else        // Consumidor final.
        {
            STRCPY(c_numero, "20-11111111-2");			
			/*if(strlen(c_cuit) == 0)/// || strlen(c_cuit) > 8)
				STRCPY(c_cuit, "11111111");*/
            f_letra =  'B';
            f_respon = 'C';
			validarCuit = 0;
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
			Alert("El CUIT del cliente no es valido.", "Se emitira un ticket comun...");
			GlobalTF = 0;
			sprintf(hdrPkt, "@%cT%cT", SEP , SEP);
			xchgPkt(hdrPkt);           // Abre ticket fiscal comun
			STRCPY(GlobalComprob, "TI");
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
				sprintf(hdrPkt, "(%s) - La CONDICION ante el IVA del cliente es INVALIDA", _itoa(lstat.fiscstat, msg, 10));
				Alert(hdrPkt, "Se imprimira un ticket comun");
				sprintf(hdrPkt, "@%cT%cT", SEP, SEP);
			}
			else
				sprintf(hdrPkt, "@%c%c%cT", SEP, f_letra, SEP);

			xchgPkt(hdrPkt);
			GlobalComprob[0] = 'F';
			GlobalComprob[1] = f_letra;
			GlobalComprob[2] = 0;
		}
    }
    else
    {
        sprintf(hdrPkt, "@%cT%cT", SEP , SEP);
        xchgPkt(hdrPkt);           // Abre ticket fiscal comun
		STRCPY(GlobalComprob, "TI");
    }
	inOper = 1;
	abierto = true;
    return 0;
}

void PrinterPR4::PrepareCustomerData(char *hdrPkt)
{
	if(strlen(c_nombre) > 30) c_nombre[30] = 0;
	sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s" ,SEP, c_nombre, SEP, c_cuit,	SEP, f_respon, SEP, f_respon=='C'? ' ': 'C',SEP, c_direcc);
	//sprintf(hdrPkt,   "\x62%c%s%c%s%c%c%c%c" ,SEP, c_nombre, SEP, c_numero,	SEP, f_respon, SEP, vCuit(c_numero) ? '2': 'D');//f_respon=='C'? ' ': 'C');
	xchgPkt(hdrPkt);
}

void PrinterPR4::PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto)
{
	if(cajaBulto < 1) cajaBulto = 1;
	
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
int PrinterPR4::PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto, int variac, char *ades, 
						  double unitario, bool pesado, int cajaBulto)
{
    char tdes[100];	
    unsigned maxLen;
	char multLine[60];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

    maxLen = GlobalTF? 40: 20;
    ChkDes(des, tdes);	
    ChkDes(ades, NULL);

	// Ajusta parametros
	double xtasa = ((double)tasa)/100;
	//if(qual != 'M') qual = 'm'; 

	//   Envia el comando al impresor.
	for(int loop=0; loop <= 1; loop++)
	{
        //   Abre el comprobante, si corresponde.
		if(!abierto)//(!inOper) 
			PrintEncab();
		
		//// Imprime datos de la multiplicacion efectuada, si corresponde.
		//if((fabs(mult - 1.000) > 0.001) || pesado || cajaBulto > 0 || printCant)
		//{
		//	if(cajaBulto > 0)
		//		sprintf(multLine, "%.0lf X %d X $%.2lf", mult, cajaBulto, unitario);
		//	else if(pesado)
		//		sprintf(multLine, "%.3lfKg X $%.2lf", mult, unitario);
		//	else
		//		sprintf(multLine, "%.0lf X $%.2lf", mult, unitario);
		//}
		//else
		//	STRCPY(multLine, "");

		//char *l1 = des, *l2 = "", *l3 = "";
		//if(strlen(multLine) != 0)
		//{
		//	l2 = l1;
		//	l1 = multLine;
		//}
		//if(strlen(ades) != 0)
		//{
		//	if(l2[0] != 0) l3 = l2;
		//	l2 = l1;
		//	l1 = ades;
		//}

		//   Imprime la descripcion adicional del articulo.
		if(strlen(ades) != 0)
		{
			if(qual != 'D') 
			{
				//if (strlen(l1) > 0)
				//	PrintText(l1);
				//if (strlen(l2) > 0)
				//	PrintText(l2);
				//if (strlen(l3) > 0)
				//	STRCPY(tdes, l3);
				PrintText(ades);
			}
			else
			{
				//STRCPY(dscp, tdes);
				//strcat(dscp, ades);
				strcat(tdes, ades);
				if (strlen(tdes) > 20)
					tdes[19] = 0;
				//STRCPY(tdes, ades);					
			}
		}
		double lounitario = monto;
		//double lounitario = unitario;
		if ((fabs(mult - 1.000) > 0.001))
		{
			lounitario = unitario;
			if (pesado)			
				mult = monto / unitario;
		}			
		else if (qual == 'D')
			lounitario = unitario;
		else if (cajaBulto > 1)
			lounitario = unitario;

		//   Adapta el calificador de la operacion.
		//PrepareCommand(txbuff, tdes, mult, (pesado ? monto : unitario), xtasa, qual, cajaBulto);
		PrepareCommand(txbuff, tdes, mult, lounitario, xtasa, qual, cajaBulto);

		//	 Prepara el paquete a enviar.
		//   Envia los datos concretos del articulo. Verifica errores tipicos.
		xchgPkt(txbuff);
		if(chkPrnStat())  continue;
		if(chkFiscStat()) continue;

		//   Si el ticket no estaba abierto, lo abre.
        if(lstat.fiscstat & 0x30)
		{
			if(GlobalReprint || inOper == 0)
                PrintEncab();
			continue;
		}
		break;
	}
    
	proteClose = time__();
	return 0;
}
//   Imprime y chequea el subtotal de la operacion.
int PrinterPR4::PrintSubtot(char *msg, int pFlag)
{
	char tdes[30];

	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, tdes);

    //sprintf(txbuff, "%c%c%c%c%s%c0", (GlobalTF? 'c': 'C'), SEP, pFlag, SEP, tdes, SEP);
	sprintf(txbuff, "%c%c%c%c%s%c0", 'C', SEP, pFlag, SEP, tdes, SEP);
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
int PrinterPR4::PrintText(char *msg)
{
	char tdes[50];

	ChkDes(msg, tdes);
	if (strlen(tdes) > 30)
		tdes[30] = 0;
    if(!inOper) PrintEncab();

    sprintf(txbuff2, "A%c%s%c0", SEP, tdes, SEP);
	xchgPkt(txbuff2);
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPR4::PrintMPago(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDes(msg, tdes);
    sprintf(txbuff, "%c%c%s%c%.2lf%cT%c0", 'D', SEP, tdes, SEP, monto, SEP, SEP);
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPR4::PrintDescto(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(strcmp(msg, "Ajuste por redondeo") == 0)
		return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDes(msg, tdes);
	sprintf(txbuff, "%c%c%s%c%.2lf%cm%c0%cC", 'T', SEP, tdes, SEP, monto, SEP, SEP, SEP);
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

int PrinterPR4::PrintClose()
{
	if(proteClose == time__()) proteClose++;
	while(proteClose > time__())
		tSwitch();

	xchgPkt("E");
	abierto = false;
	if (pieChanged)
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, LogoPie[0]);	
		xchgPkt(txbuff);
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, LogoPie[1]);
		xchgPkt(txbuff);	
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 13, SEP, LogoPie[2]);
		xchgPkt(txbuff);		
	}
	pieChanged = false;
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterPR4::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	/*sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'd', SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);*/
	sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'D', SEP, SEP, SEP, SEP);
	xchgPkt(txbuff);
	abierto = false;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterPR4::PrintZeta()
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
int PrinterPR4::OpenVoucher()
{
	xchgPkt("\x48");
	GlobalVoucherFlag = 1;
	return 0;
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterPR4::OpenVoucher2()
{
	//borra el encabezado y cola
	sprintf(txbuff, "\x5d%c0%c ",SEP,SEP);
	xchgPkt(txbuff);
	//abre el voucher
	xchgPkt("\x48");
	GlobalVoucherFlag = 1;
	return 0;
}

//   Imprime un texto en un comprobante NO fiscal.
int PrinterPR4::PrintVoucher(char *des)
{	
	char *itera = des;
	int i = 0; int j = 0;
	char aux[100];
	strcpy(aux, des);
	bool cambio = false;
	aux[0] = 0;
	while (itera[i] != '\0')
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
		ChkDes(aux, NULL);		
		sprintf(txbuff, "\x49%c%c%s%c0", SEP, '\xf4', aux, SEP);
	}else
	{
		ChkDes(des, NULL);
		sprintf(txbuff, "\x49%c%s%c0", SEP, des, SEP);
	}
    
	xchgPkt(txbuff);
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPR4::CloseVoucher()
{
	xchgPkt("\x4a");
    GlobalVoucherFlag = 0;
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPR4::CloseVoucher2()
{
	int i;

	xchgPkt("\x4a");
	GlobalVoucherFlag = 0;

	//vuelve poner los encabezados y colas

	//  Primero, instala los encabezamientos.
	for(i=0; i<cLinesLogo; i++)
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+1, SEP, ChkDes(Trim(LogoLines[i]),0));
		xchgPkt(txbuff);
	}
	availHeaderLine = i;
	while(i <= 11)              // Vacia las lineas no usadas.
	{	
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+1, SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}
	//  Ahora, instala las definiciones de PIE de ticket.
	for(i=0; i<cLinesPie; i++)
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+11, SEP, ChkDes(LogoPie[i],0));		
		xchgPkt(txbuff);
	}
	while(i <= 8)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, i+11, SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}
	return 0;
}

//   Chequea los bits de estado del controlador fiscal y muestra los mensajes
//   adecuados, de corresponder.
int PrinterPR4::chkFiscStat()
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
    //else if(lstat.fiscstat & 0x10)
    //    msg = "No coinciden los totales del ticket y pantalla.";
    //else if(lstat.fiscstat & 0x20)
    //    msg = "Campo no valido para el estado fiscal actual";
	else if(lstat.fiscstat & 0x40)
		msg = "Monto Total excede lo permitido por el impresor";
    else if(lstat.fiscstat & 0x80)
        msg = "Memoria fiscal llena, bloqueada o dada de baja";
	else if(lstat.fiscstat & 0x100)
		msg = "Memoria fiscal CASI llena. Avise al Tecnico";
	else
	{
		if(lstat.fiscstat & 0x10) return 1;
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
int PrinterPR4::chkPrnStat()
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
int PrinterPR4::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
//	int intMonto = (int)(permonto * 100);
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
		bool fallo = (chkPrnStat() != 0);
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

int PrinterPR4::docFiscStat()
{
	if (lstat.fiscstat & 0x1000) 
	{		
		return 1;
	}
	return 0;
}
