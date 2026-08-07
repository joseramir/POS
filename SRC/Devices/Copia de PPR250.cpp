/*
 *   PPR4.c
 *   Driver del impresor HASAR PR4F Fiscal.
*/

#include <stdafx.h>
// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif




/*   Datos de este solo modulo   */
//static struct lstat_ lstatPR4;
//static char *pc;                // Para explorar respuestas.
////static int inTickPR4;           // Indica que el PR4 tiene ticket abierto.
//long _ultimoBcompleto, _ultimoAcompleto;

//extern "C" {
//	PrinterPR250 P_PR4;
//};
char mfld[10000];

char *PrinterPR250::splitChar(char **text)
{
	/*char *temp = *text;
	while (*temp != '\0' && *temp != SEP) 
	{
		temp++;
	}
	*temp = 0;
	return temp;*/


	char *r;
	int ki = 0;

	if(**text == SEP) 
		(*text)++; 
	if(**text == 0 ) 
		return "";
	for(r = mfld; **text != SEP || **text == 0; )
	{
		if (**text == 0)
			break;
		else
		{
			*r++ = **text;
			(*text)++;
			ki++;
		}
	}
	*r = 0;
	return mfld;
}

//   Extrae un campo y lo procesa como una constante hexadecimal.
unsigned long PrinterPR250::sgethexa(char **pc)    
{
	unsigned long res;
	char *p;

	for(p = splitChar(pc), res = 0l; isxdigit(*p); p++)
	{
		res *= 16;
		res += (*p <= '9')? (*p - '0') : (
			(*p <= 'F')? (*p - 'A' + 10): (*p - 'a' + 10));
	}
	return res;
}

//   Extrae un campo y lo procesa como una constante decimal.
unsigned long PrinterPR250::sgetdec(char **pc)     // Lee un nro expresado decimal
{
	unsigned long res;
	char *p;

	for(p = splitChar(pc), res = 0l; isdigit(*p); p++)
	{
		res *= 10;
		res += *p - '0';
	}
	return res;
}



// Retorna datos del propietario del impresor
void PrinterPR250::Owner(char *cuit, char *pv)
{
	SetHeader("");
	SetPie("");
	while(true)
	{
		xchgPkt("\x73"); // xchgPkt("s");
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
		getfld(&pc); //Razon Social
		getfld(&pc); //Registro
		//getfld(&pc);
		STRCPY(pv, getfld(&pc)); //Numero de POS
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
void PrinterPR250::Configure(char *confParam)
{
	char tmpPkt[50];
	if (strlen(confParam) > 40)
		confParam[40] = 0;
	sprintf(tmpPkt, "d%c7%c%s", SEP, SEP, confParam);
	xchgPkt(tmpPkt);
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterPR250::ChkPrnXonXoff()
{
	int rtachk = 0;
	{
		//GCStatic::impre2Gen->Conectar("127.0.0.1");		
		handImpre = OpenComFiscal(18, 0);
		MandaPaqueteFiscal(handImpre, "\x2A");
		UltimaRespuesta(handImpre, bufImpre);
		//xchgPkt("\x2A"); 
		char *pc = bufImpre;// rxBuff + 1;

		//   Descompila los distintos campos del estado.		
		lstat.prnstat = sgethexa(&pc);// gethexa(&pc);
		lstat.fiscstat = sgethexa(&pc);

		this->EstadoAuxiliar = sgethexa(&pc);
		this->EstadoInterno = sgetdec(&pc);

		lstat.lastcomp = sgetdec(&pc);
		/*lstat.fecop = getdec(&pc);
		lstat.horaop = getdec(&pc);*/
		
		rtachk = 0;
		//   Analiza por posibles situaciones de error.
		if(chkPrnStat())
			rtachk = 1;
		if(chkFiscStat()) 
			rtachk = 2;
	}while (rtachk == 1)
	/*while(chk() == 1)
        ;*/
	return 0;
}

void PrinterPR250::CheckStatus()
{
	xchgPkt("*");
	chkFiscStat();
}

// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1 = ticket b/c factura b/c
//    tipo: 2 = ticket 'a' o factura 'a'
int PrinterPR250::GetTickNro(int cual)
{
	//codigos de comprobante 
	//81 Tique FActura A
	//82 Tique Factura B
	//83 Tique

	// Envia el comando.
	ignoraError4 = 1;
	sprintf(txbuff, "*%c%d", SEP, TipoComprobante); //(cual == 2 ? 81 : 82));
	int result = xchgPkt(txbuff);
	ignoraError4 = 0;
	

	//   Desensambla los distintos campos del estado.
	if(result)
	{
		pc = rxBuff + 1;
		gethexa(&pc);					// Estado de la impresora
		gethexa(&pc);					// Estado Fiscal

		getdec(&pc);					// Estado auxiliar
		getdec(&pc);					// Estado Interno
		getdec(&pc);					// Codigo comprobante en curso
		getdec(&pc);					// Codigo comprobante informado
		//if (cual == 2)
		//	_ultimoAcompleto = getdec(&pc);	// Nro tick A
		//else
		//	_ultimoBcompleto = getdec(&pc);	// Nro tick B/C
		NumeroComprobante = getdec(&pc);	//Numero de Comprobante
		//gethexa(&pc);					// Estado auxiliar
		
	}

	// Retorna el numeo pedido
	//if(cual == 1)					// GlobalTF -->  0=CF, 1=FA, 2=FB
	//	return _ultimoAcompleto;
	//else
	//	return _ultimoBcompleto;
	return NumeroComprobante;
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispositivo presente 
//   en el impresor PR4 para activarla.
int PrinterPR250::OpenDrwr()
{
	xchgPkt("\x7b");
	return 0;
}

// Completa la inicializacion que es especifica del modelo.
void PrinterPR250::CompleteInitialization()
{
	double llimfiscal = 0;
	// Envia el comando.	
	sprintf(txbuff, "\xae%c%c", SEP, 'B');			
	int result = xchgPkt(txbuff);

	//   Desensambla los distintos campos del estado.
	if(result)
	{
		pc = rxBuff + 1;
		gethexa(&pc);					// prnstat
		gethexa(&pc);					// fiscstat
		//gethexa(&pc);					// limite obligacion Datos Consumidor Final
		llimfiscal = getdec(&pc);	// Limite Tique-Factura
		sprintf(txbuff, "Limite Fiscal: %.2lf", llimfiscal);
		WriteBootLog(txbuff);
		if (llimfiscal == 0) //No tiene limite fiscal se coloca el que esta configurado
			llimfiscal = limTicket;
		if (checkLimFisc)	
			limTicket = llimfiscal;
		else
		{
			if (limTicket > llimfiscal)
				limTicket = llimfiscal;
		}
		txbuff[0] = 0;
	}
	TipoComprobante = 83; //Setea el tipo de Comprobante por defecto a Tique
}

char *PrinterPR250::HasarBrand()
{
	return "Espere. Cargando el impresor HASAR PR250...";
}

//   Descarga en el printer todos los datos instalables.
int PrinterPR250::PrinterUpLoad()
{
	int i;
	abierto = false;

    //  Activa las capacidades del impresor.
    PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY;	

    //  Mensaje!!!
    SendMsg(HasarBrand(), "");
    ChkPrnXonXoff();

	
	WriteBootLog("Cancelando documentos abiertos si hubiera");
	sprintf(txbuff, "\x98");
	xchgPkt(txbuff);

	sprintf(txbuff, "\x9E%c1%c2%c%s%cD%cF", SEP, SEP, SEP, ChkDes(Trim(LogoLines[0]),0),SEP, SEP);
	xchgPkt(txbuff);
	sprintf(txbuff, "\x9E%c2%c1%c%s%cD%cF", SEP, SEP, SEP, "\x7f",SEP, SEP);
	xchgPkt(txbuff);

	//  Primero, instala los encabezamientos.
	for(i=1; i<5; i++)
	{
        sprintf(txbuff, "\x9E%c%d%c0%c%s%cD%cO", SEP, i, SEP, SEP, ChkDes(Trim(LogoLines[i]),0), SEP, SEP);
		xchgPkt(txbuff);
	}
	availHeaderLine = i;
	i = 1;
	while(i <= 3)              // Vacia las lineas no usadas.
	{	
		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cH", SEP, i, SEP, SEP, "\x7f",SEP, SEP);
		xchgPkt(txbuff);

		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%ch", SEP, i, SEP, SEP, "\x7f",SEP, SEP);
		xchgPkt(txbuff);
		i++;
	}

	//  Ahora, instala las definiciones de PIE de ticket.
	for(i=0; i<cLinesPie; i++)
	{
        sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, i+1, SEP, SEP, ChkDes(LogoPie[i],0), SEP, SEP);		
		xchgPkt(txbuff);
	}
	i = 1;
	while(i <= 6)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%ct", SEP, i, SEP, SEP, "\x7f",SEP, SEP);
		xchgPkt(txbuff);
		i++;
	}

	// Programa la fecha y hora
	//sprintf(txbuff, "X%c%06d%c%06d", SEP, date()-20000000, SEP, time__());
	sprintf(txbuff, "\x58%c%06d%c%06d", SEP, date()-20000000, SEP, time__());
	xchgPkt(txbuff);

	CompleteInitialization();
    ClearMsg();
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPR250::SetPie(char *msg)
{
	char tmsg[80];

	ChkDes(msg, tmsg);	
	sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 2, SEP, SEP, (strlen(tmsg) == 0)? " " : tmsg, SEP, SEP);	
	xchgPkt(txbuff);	
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterPR250::SetPie(char *msg, int linea)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	switch (linea)
	{
		case 1:			
			sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 1, SEP, SEP, (strlen(tmsg) == 0)? " " : tmsg, SEP, SEP);	
			break;
		case 2:			
			sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 2, SEP, SEP, (strlen(tmsg) == 0)? " " : tmsg, SEP, SEP);	
			break;
		case 3:			
			sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 3, SEP, SEP, (strlen(tmsg) == 0)? " " : tmsg, SEP, SEP);	
			break;
		default:			
			sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 1, SEP, SEP, (strlen(tmsg) == 0)? " " : tmsg, SEP, SEP);	
			break;
	}
	
	xchgPkt(txbuff);
	pieChanged = true;
	return 0;
}


//  Cambia el mensaje de la ultima linea del HEADER del ticket.
int PrinterPR250::SetHeader(char *msg)
{
	char tmsg[80];

	ChkDes(msg, tmsg);
	if(availHeaderLine > 10) availHeaderLine = 10;
	//sprintf(txbuff, "\x5D%c%02d%c%s", SEP, availHeaderLine, SEP, (strlen(tmsg) == 0)? " ": tmsg);
	sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cH", SEP, 1, SEP, SEP, (strlen(tmsg) == 0)? " ": tmsg,SEP, SEP);
	xchgPkt(txbuff);
	return 0;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
int PrinterPR250::PrintEncab()
{
    char hdrPkt[400], *p;
	int validarCuit = 1;
	TipoComprobante = 83;//int tipocomp = 83;//Ticket es el comprobante por defecto
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
			TipoComprobante = 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "NI"))
        {
            f_letra =  'A';
            f_respon = 'T';
			TipoComprobante = 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "EX"))
        {
            f_letra =  'B';
            f_respon = 'E';
			TipoComprobante = 82; //tipocomp = 82;
        }
        else if(!_stricmp(c_condiva, "NR"))
        {
            STRCPY(c_numero, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'A';
			TipoComprobante = 82; //tipocomp = 82;
			validarCuit = 0;
        }
        else if(!_stricmp(c_condiva, "MO"))
        {
            f_letra =  'B';
            f_respon = 'M';
			TipoComprobante = 82; //tipocomp = 82;
        }
        else        // Consumidor final.
        {
            STRCPY(c_numero, "20-11111111-2");			
			/*if(strlen(c_cuit) == 0)/// || strlen(c_cuit) > 8)
				STRCPY(c_cuit, "11111111");*/
            f_letra =  'B';
            f_respon = 'C';
			TipoComprobante = 82; //tipocomp = 82;
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
		
		if(validarCuit && !vCuit(c_numero))
		{
			Alert("El CUIT del cliente no es valido.", "Se emitira un ticket comun...");
			GlobalTF = 0;
			sprintf(hdrPkt, "\x40%c%d", SEP, 83);
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
				sprintf(hdrPkt, "\x40%c%d", SEP, 83);
			}
			else
				sprintf(hdrPkt, "\x40%c%d", SEP, TipoComprobante);// tipocomp);				

			xchgPkt(hdrPkt);
			GlobalComprob[0] = 'F';
			GlobalComprob[1] = f_letra;
			GlobalComprob[2] = 0;
		}
    }
    else
    {
        sprintf(hdrPkt, "\x40%c%d", SEP , TipoComprobante); // tipocomp);
        xchgPkt(hdrPkt);           // Abre ticket fiscal comun
		if (chkFiscStat() == 0)
		STRCPY(GlobalComprob, "TI");
    }
	inOper = 1;
	abierto = true;
    return 0;
}

void PrinterPR250::PrepareCustomerData(char *hdrPkt)
{
	if(strlen(c_nombre) > 30) c_nombre[30] = 0;
		sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s%c%s%c%s%c%s" ,SEP, c_nombre, SEP, c_cuit, SEP, f_respon, SEP, f_respon=='C'? ' ': 'C',
			SEP, c_direcc, SEP, "", SEP, "", SEP, "");		
	xchgPkt(hdrPkt);
}

void PrinterPR250::PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto)
{
	if(cajaBulto < 1) 
		cajaBulto = 1;
	
	if(qual == 'D')			
		sprintf(txbuff, "m%c%s%c%.2lf%c%d%c%.2lf%c%c%c%d%c%d%c%c%c%s%c%c" ,
			SEP, tdes, SEP, monto, SEP, 7, SEP, xtasa, SEP, '+', SEP, 0, SEP, 0, SEP, 'T', SEP, "7790001001030", SEP, 'B');	
	else
	{		
		/*if (imprecodigo == 99997)
			sprintf(txbuff, "m%c%s%c%.2lf%c%d%c%.2lf%c%c%c%d%c%d%c%c%c%s%c%c" ,
			SEP, tdes, SEP, monto, SEP, 7, SEP, xtasa, SEP, '+', SEP, 0, SEP, 0, SEP, 'T', SEP, "7790001001047", SEP, 'R');	
		else*/
		sprintf(txbuff, "\x42%c%s%c%.6lf%c%.2lf%c%d%c%02.2lf%c%c%c%c%c%.2lf%c%d%c%c%c%d%c%s%c%d%c%d" ,
            SEP, tdes, SEP, mult * cajaBulto, SEP, monto, SEP, 7, SEP, xtasa, SEP, qual, SEP, '+', 
			SEP, 0.0, SEP, 0, SEP, 'T', SEP, 1, SEP, "", SEP, imprecodigo, SEP, 7);		
	}
}

//   Imprime un item de linea.
int PrinterPR250::PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto, int variac, char *ades, 
						  double unitario, bool pesado, int cajaBulto)
{
    char tdes[100];	
    unsigned maxLen;
	char multLine[60];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	TipoComprobante = 83;

    maxLen = GlobalTF? 40: 20;
    ChkDes(des, tdes);	
    ChkDes(ades, NULL);

	// Ajusta parametros
	double xtasa = ((double)tasa)/100;	

	//   Envia el comando al impresor.
	for(int loop=0; loop <= 1; loop++)
	{
        //   Abre el comprobante, si corresponde.
		if(!abierto)//(!inOper) 
			PrintEncab();		

		//   Imprime la descripcion adicional del articulo.
		if(strlen(ades) != 0)
		{
			if(qual != 'D') 				
				PrintText(ades);			
			else
			{				
				strcat(tdes, ades);
				if (strlen(tdes) > 20)
					tdes[19] = 0;								
			}
		}
		double lounitario = monto;		
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
int PrinterPR250::PrintSubtot(char *msg, int pFlag)
{
	char tdes[30];

	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, tdes);
	//msg no va en esta impresora
	sprintf(txbuff, "\x43%c%c%c%c", SEP, (pFlag == 1 ? 'P' : 'N'), SEP, '0');
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
int PrinterPR250::PrintText(char *msg)
{
	char tdes[50];

	ChkDes(msg, tdes);
	if (strlen(tdes) > 30)
		tdes[30] = 0;
    if(!inOper) PrintEncab();

    sprintf(txbuff2, "\x41%c%s%c%s%c0", SEP, "", SEP, tdes, SEP);
	xchgPkt(txbuff2);
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPR250::PrintMPago(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDes(msg, tdes);
    sprintf(txbuff, "\x44%c%s%c%.2lf%c%c%c%c", SEP, tdes, SEP, monto, SEP, 'T', SEP, '0');
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterPR250::PrintDescto(char *msg, double monto)
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
	sprintf(txbuff, "\x54%c%s%c%.2lf%c%c%c%c%c%s%c%c", SEP, tdes, SEP, monto, SEP, '0', SEP, 'T', SEP, "7790001001030", SEP, 'B'); //(monto >= 0 ? 'R' : 'D'));
	xchgPkt(txbuff);
	proteClose = time__();
	return 0;
}

int PrinterPR250::PrintClose()
{
	if(proteClose == time__()) proteClose++;
	while(proteClose > time__())
		tSwitch();

	sprintf(txbuff, "\x45%c%c%c%s", SEP, '0', SEP, "");
	xchgPkt(txbuff);
	abierto = false;
	if (pieChanged)
	{
		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 1, SEP, SEP, LogoPie[0], SEP, SEP);				
		xchgPkt(txbuff);
		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 2, SEP, SEP, LogoPie[1], SEP, SEP);			
		xchgPkt(txbuff);	
		sprintf(txbuff, "\x9E%c%d%c1%c%s%cD%cT", SEP, 3, SEP, SEP, LogoPie[2], SEP, SEP);			
		xchgPkt(txbuff);		
	}
	pieChanged = false;
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterPR250::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	
	//sprintf(txbuff, "%c%cCancela%c0.0%cC%c0", 'D', SEP, SEP, SEP, SEP);
	xchgPkt("\x98");
	abierto = false;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterPR250::PrintZeta()
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
int PrinterPR250::OpenVoucher()
{
	sprintf(txbuff, "\x40%c%d", SEP , 910);
	xchgPkt(txbuff); 
	//xchgPkt("\x48");
	GlobalVoucherFlag = 1;
	return 0;
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterPR250::OpenVoucher2()
{
	//borra el encabezado y cola
	/*sprintf(txbuff, "\x5d%c0%c ",SEP,SEP);
	xchgPkt(txbuff);*/
	//abre el voucher	
	sprintf(txbuff, "\x40%c%d", SEP , 910);
	xchgPkt(txbuff); 
	GlobalVoucherFlag = 1;
	return 0;
}

//   Imprime un texto en un comprobante NO fiscal.
int PrinterPR250::PrintVoucher(char *des)
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
	int aformato = 0;
	if (cambio)
	{
		aux[j] = 0;
		aformato = 8;		
	}
	ChkDes(aux, NULL);		
	sprintf(txbuff, "\x49%c%d%c%s%c0", SEP, aformato, SEP, aux, SEP);    
	xchgPkt(txbuff);
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPR250::CloseVoucher()
{
	sprintf(txbuff, "\x45%c%d%c%s", SEP, 1, SEP, "");   
	xchgPkt(txbuff);
    GlobalVoucherFlag = 0;
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterPR250::CloseVoucher2()
{
	int i;

	sprintf(txbuff, "\x45%c%d%c%s", SEP, 1, SEP, "");   
	xchgPkt(txbuff);
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
int PrinterPR250::chkFiscStat()
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
    else if(lstat.fiscstat & 0x20)	
        msg = "Campo no valido para el estado fiscal actual";
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
	ConsultaUltimoError();
    #else
    printf("Error Fiscal en la Impresora!!!\n");
    #endif
	return 1;
}

//   Chequea los bits de estado del impresor fiscal y muestra los mensajes
//   orientativos que correspondieren.
int PrinterPR250::chkPrnStat()
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
int PrinterPR250::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
//	int intMonto = (int)(permonto * 100);
	int i = 0;
	int tverror = 0;
	//char auxmonto[10];
	char tvaux[70];
	char hdrPkt[500];	
	//char sep[2]; 
	int result = 0;

	if (permonto < 0.01)
		return result;
	if(strcmp(perdescrip, "Ajuste por redondeo") == 0)
		return result;
	if (!docFiscStat()) //si no existe un documento fiscal abierto no se imprimira la percepcion
		return result;
	int atipoper = 9;
	if (gtotalpercep > 0)
		atipoper = 7;
	else if (gtotaltissh > 0)
		atipoper = 8;
	//double mbase = permonto / (((double)pertasa)/100);

	/*sprintf(auxmonto, "%.2lf", permonto);
	sep[0] = SEP;
	sep[1] = 0;*/
	while(true)
	{
		sprintf(hdrPkt, "\x61%c%d%c%s%c%.2lf%c%.2lf", SEP, atipoper, SEP, perdescrip, SEP, baseImp, SEP, permonto);							
		//STRCAT(hdrPkt, sep);
		//STRCAT(hdrPkt, "**.**");						STRCAT(hdrPkt, sep);		// "S"
		//STRCAT(hdrPkt, perdescrip);					STRCAT(hdrPkt, sep);		
		//STRCAT(hdrPkt, auxmonto);

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

int PrinterPR250::docFiscStat()
{
	if (lstat.fiscstat & 0x1000) 
	{		
		return 1;
	}
	return 0;
}

int PrinterPR250::GeneraReporteAfip(int fdesde, int fhasta)
{
	sprintf(txbuff, "\x76%c%d%c%d%c%c", SEP, fdesde, SEP, fhasta, SEP, 'P');
	xchgPkt(txbuff);

	char *pc = rxBuff;
	FILE *fpw;
	std::ofstream myfile;
	myfile.open("rep.txt");
	//fpw = fopen("rep.txt", "w");

	getfld(&pc);
	getfld(&pc);
	getfld(&pc);
	if (getdec(&pc) == 1)
	{
		char *cadafip = new char[20000];
		STRCPY(cadafip, getfld(&pc));
		myfile << cadafip;
		myfile.flush();
		//fputs(cadafip, fpw);	
		ConsultaUltimoError();
		bool sigue = true;
		//GrabaRepLog = true;
		while(sigue)
		{
			cadafip = "";
			sprintf(txbuff, "\x77");
			xchgPkt(txbuff);

			char *pvc = rxBuff;
			getfld(&pvc);
			getfld(&pvc);
			getfld(&pvc);
			if (atoi(getfld(&pvc)) == 1)
			{
				STRCPY(cadafip, getfld(&pc));
				myfile << cadafip; // getfld(&pvc);
				myfile.flush();
				//cadafip = NULL;
				STRCPY(cadafip, getfld(&pvc));
				//fputs(cadafip, fpw);
			}
			else
				sigue = false;
			delete pvc;
		}
		cadafip = NULL;
		delete cadafip;
		GrabaRepLog = false;
	}
	//fclose(fpw);
	myfile.close();	
	return 1;
}

void PrinterPR250::ConsultaUltimoError()
{
	sprintf(txbuff, "\x2C");
	xchgPkt(txbuff);
	char *pvc = rxBuff;
	getfld(&pvc);
	getfld(&pvc);
	getfld(&pvc);
	char *ultMsjError = new char[10000];
	char *ultCodError = new char[100];
	STRCPY(ultCodError, getfld(&pvc));
	getfld(&pvc);
	STRCPY(ultMsjError, getfld(&pvc));
	Alert("Error Fiscal en la Impresora", ultMsjError);
	if (strcmp(ultCodError, "POS_DOCUMENT_BEYOND_FISCAL_DAY") == 0)
	{
		PrintZeta();
		exit(0);
	}
	delete ultMsjError;
	delete ultCodError;
}