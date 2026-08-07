/*
 *   P300.c
 *   Driver del impresor EPSON TM300F / TM950 Fiscal.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//  Variables de estado.
long prnstat, fiscstat, ultimaZeta, ultimoBcompleto, ultimoB, ultimoAcompleto, ultimoA,
	ultimoDNF, ultimoNFH, numeroRef;

//   Datos de este solo modulo   
char *pc,                // Para explorar respuestas.
     f_letra,           //  Para los tickets fiscales.
	 f_respon,			//  Tipo de responsabilidad del cliente.
	 tdes[200];

//   Clase que encapsula el manejo de este impresor.

//extern "C" {
//	PrinterTM300 P_TM300;
//};

//  Flag usado para mostrar algunos mensajes una sola vez.
int flagMsg = 0;
#define FLAG_MSG_1	1	/* Para mostrar 1 vez el msg "No puede abrir ticket fiscal, ..."  */

//  Constructor
PrinterTM300::PrinterTM300()
{
	STRCPY(vendor, "EPSON TM300/950F");
	hdrChanged = pieChanged = false;
}

// Retorna datos del propietario del impresor
void PrinterTM300::Owner(char *cuit, char *pv)
{
	double llimfiscal = 0;
	SetPie("");
	SetHeader("");
	hdrChanged = pieChanged = false;
	sprintf(msg, "*%cC", SEP);
	while(true)
	{
		xchgPkt(msg);
		if(chkFiscStat()) continue;
		if(chkPrnStat()) continue;
		break;
	}

	char *pc = rxBuff + 1;
	getfld(&pc);				// Estado fiscal
	getfld(&pc);				// Estado de impresora
	STRCPY(cuit, getfld(&pc));	// CUIT
	STRCPY(pv, getfld(&pc));	// Punto de Venta
	STRCPY(ptovta, pv);
	getfld(&pc);
	getfld(&pc);
	llimfiscal = atof(getfld(&pc));
	sprintf(msg, "Limite Fiscal: %.2lf", llimfiscal);
	WriteBootLog(msg);
	if (checkLimFisc)	
		limTicket = llimfiscal;
	else
	{
		if (limTicket > llimfiscal)
			limTicket = llimfiscal;
	}	
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterTM300::ChkPrnXonXoff()
{
	while(chk() == 1)
		if(FiscalForceZeta)
			PrintZeta();
	return 0;
}

void PrinterTM300::CheckStatus()
{
	xchgPkt("*");
	chkFiscStat();
}

// Ajustamos el redondeo en los Ticket A
void PrinterTM300::AjustarRedondeo()
{
	char msg[60], msg2[10];
	STRCPY(msg, "R");
	STRCPY(msg2, "");
	for(int index=0; index<10; index++)
	{
		double red = ceil(redondeo[index]); //(double)((int)redondeo[index]);
		if(red > 0.1)
		{
			PrintItem(msg, 1, red /100, /*(int)(rtasa[index] * 10000)*/0, 'M', 0, 0, msg2, 1, 0, 0);
		}
	}
}

// Imprime la percepcion en la impresor fiscal
int PrinterTM300::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
	int intMonto = (int)(permonto * 100);
	int i = 0;
	int tverror = 0;
	char auxmonto[10];
	char tvaux[70];
	char hdrPkt[500];

	int result = 0;
	if (permonto < 0.01)
		return result;
	BDecimal xtasa = (((double) pertasa) / 10000);
	xtasa.ApplyRound(5);
	xtasa *= 10000;
	sprintf(auxmonto, "%d", intMonto);
	int intTasa = xtasa.ToInt32();
	char sep[2]; 
	sep[0] = SEP;
	sep[1] = 0;
	while(true)
	{
		STRCPY(hdrPkt, "\x66");							STRCAT(hdrPkt, sep);
		STRCAT(hdrPkt, perdescrip);					STRCAT(hdrPkt, sep);
		STRCAT(hdrPkt, "I");								STRCAT(hdrPkt, sep);		// "S"
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


// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1=ticket b/c/ factura b/c
//    tipo: 2=tikect a factura a
int PrinterTM300::GetTickNro(int cual)
{
	// Envia el comando.
	ignoraError4 = 1;
	sprintf(txbuff, "*%cA", SEP);
	int result = xchgPkt(txbuff);
	ignoraError4 = 0;


	//   Desensambla los distintos campos del estado.
	if(result)
	{
		pc = rxBuff + 1;
		prnstat = gethexa(&pc);
		fiscstat = gethexa(&pc);
		ultimaZeta = getdec(&pc);
		ultimoBcompleto = getdec(&pc);
		ultimoB = getdec(&pc);
		ultimoAcompleto = getdec(&pc);
		ultimoA = getdec(&pc);
		ultimoDNF = getdec(&pc);
		ultimoNFH = getdec(&pc);
		numeroRef = getdec(&pc);
	}

	// Retorna el numeo pedido
	if(cual == 1)					// GlobalTF --> 0=CF, 1=A, 2=B
		return ultimoAcompleto;
	else
		return ultimoBcompleto;
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispo-
//   sitivo presente en el impresor TM300 para activarla.
int PrinterTM300::OpenDrwr()
{
	xchgPkt("\x7b");
	return 0;
}

void PrinterTM300::PrepararTexto(char *texto, bool centrar)
{
	int largo = strlen(texto);
	bool hayDobleAncho = false;
	if(largo == 0) return;
	for(int i = 0; i < largo; i++)
	{
		if((texto[i] & 0xff) == 241)
			texto[i] = 'n';
		else if((largo - i > 2 ) && (texto[i] == '@' || texto[i] == '&'))
		{
			hayDobleAncho = true;
			texto[i++] = '\xf4';
			largo--;
			if(largo > 0)
				memmove(&texto[i], &texto[i + 1], largo - i + 1);
		}
	}

	if(centrar)
	{
		int agregar = ((hayDobleAncho? 23: 41) - strlen(texto)) / 2;
		int offset = hayDobleAncho? 1: 0;
		if(agregar > 0)
		{
			memmove(texto + offset + agregar, texto + offset, strlen(texto) + 1);
			while(agregar > 0)
			{
				texto[offset + agregar - 1] = ' ';
				agregar--;
			}
		}
	}
}

//   Descarga en el printer todos los datos instalables.
int PrinterTM300::PrinterUpLoad()
{
	int i;

    //  Activa las capacidades del impresor.
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
    PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY;
	GlobalVoucherFlag = 1;		// Puede emitir vouchers

	for(int index=0; index<10; index++)
		rtasa[index] = 0;

    //  Mensaje!!!
	sprintf(msg, "Espere. Preparando impresor %s...", vendor);
    SendMsg(msg, "");

    //  Primero, instala los encabezamientos.
	for(i=0; i<cLinesLogo; i++)
	{
		if(strlen(LogoLines[i]) == 0) LogoLines[i] = " ";
		PrepararTexto(LogoLines[i]);
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i + 1), SEP, LogoLines[i]);		// Modificado p/950F (i+1) <<-- (i)
		xchgPkt(txbuff);
	}
	availHeaderLine = i + 1;
	while(i < 10)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i + 1), SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}

	//  Ahora, instala las definiciones de PIE de ticket.
	for(i=0; i<cLinesPie; i++)
	{
		if(strlen(LogoPie[i]) == 0) LogoPie[i] = " ";
		PrepararTexto(LogoPie[i]);
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i+11), SEP, LogoPie[i]);
		xchgPkt(txbuff);
	}
	while(i < 10)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i+11), SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}

	// Ahora los mensajes para el ticket factura
	for(i=0; i<cLinesEpson; i++)
	{
		if(strlen(LogoEpson[i]) == 0) LogoEpson[i] = ".";
		PrepararTexto(LogoEpson[i]);
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i+50), SEP, LogoEpson[i]);
		xchgPkt(txbuff);
	}
	while(i < 15)              // Vacia las lineas no usadas.
	{
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, (i+50), SEP, "\x7f");
		xchgPkt(txbuff);
		i++;
	}

	// Programa la fecha y hora
	sprintf(txbuff, "X%c%06d%c%06d", SEP, date()-20000000, SEP, time__());
	xchgPkt(txbuff);

	// Efectua operaciones custom si el impresor no es realmente un epson (como el bematech)
	CompleteInitialization();

    ClearMsg();
	return 0;
}

//  Completa la inicializacion. Placeholder para extensiones de la inicializacion que puedan ser necesarias
//  por impresores que extienden esta clase.

void PrinterTM300::CompleteInitialization()
{
}

//  Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterTM300::SetPie(char *msg)
{
	PrepararTexto(msg, true);
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(msg) == 0)? " ": msg);
	xchgPkt(txbuff);
	pieChanged = true;
	return 0;
}

int PrinterTM300::SetPie(char *msg, int linea)
{
	PrepararTexto(msg, true);
	switch (linea)
	{
	case 1:
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(msg) == 0)? " ": msg);
		break;
	case 2:
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 12, SEP, (strlen(msg) == 0)? " ": msg);
		break;
	case 3:
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 13, SEP, (strlen(msg) == 0)? " ": msg);
		break;
	default:
		sprintf(txbuff, "\x5D%c%02d%c%s", SEP, 11, SEP, (strlen(msg) == 0)? " ": msg);
		break;
	}
	xchgPkt(txbuff);
	pieChanged = true;
	return 0;
}

//  Cambia el mensaje de la ultima linea del HEADER del ticket.
int PrinterTM300::SetHeader(char *msg)
{
	if(availHeaderLine > 10) availHeaderLine = 10;
	sprintf(txbuff, "\x5D%c%02d%c%s", SEP, availHeaderLine, SEP, (strlen(msg) == 0)? " ": msg);
	xchgPkt(txbuff);
	hdrChanged = true;
	return 0;
}

//   Inicializa el impresor fiscal con los datos que se le indican...
static char ibuff[1500];
int PrinterTM300::FiscalInit(char *cuit, char *rs, char *regnro, char *fec, char *ptovta,
							   char *initAct, char *ibrut, char *respon)
{
	if(strlen(cuit) != 11)
	{
		Alert("Inicializacion de Printer", "El CUIT no tiene 11 caracteres");
		return 1;
	}
	if(strlen(regnro) != 10)
	{
		Alert("Inicializacion de Printer", "El REGISTRO no tiene 10 caracteres");
		return 1;
	}
	else
		_strupr(regnro);
	if(strlen(fec) != 6)
	{
		Alert("Inicializacion de Printer", "La fecha de HOY no tiene 6 caracters (AAMMDD)");
		return 1;
	}
	if(strlen(ptovta) != 4)
	{
		Alert("Inicializacion de Printer", "El PUNTO DE VENTA no tiene 4 caracteres");
		return 1;
	}
	if(strlen(initAct) != 6)
	{
		Alert("Inicializacion de Printer", "La FECHA DE INIC.ACT no tiene 6 caracteres (AAMMDD)");
		return 1;
	}
	if(strlen(respon) != 1)
	{
		Alert("Inicializacion de Printer", "El campo RESPON no tiene 1 caracter");
		return 1;
	}
	else
		_strupr(respon);

	sprintf(ibuff, "\x63%c%s%c%-50s%c%s%c%s%c%s%c%s%c%-30s%c%s",
		SEP,cuit,SEP,rs,SEP,regnro,SEP,fec,SEP,ptovta,SEP,initAct,SEP,ibrut,SEP,respon);
	return 0;
}

char tCompleta[60];
bool abierto = false;
char *Completa(char *s, int len, bool addEOL)
{
	addEOL = false;
	int mLen = strlen(s);
	memset(tCompleta, ' ', 49);
	memcpy(tCompleta, s, mLen);
	if(addEOL)
	{
		tCompleta[mLen] = '\r';
		tCompleta[mLen + 1] = '\n';
	}
	tCompleta[len] = 0;
	
	for(int i = 0; i < len; i++)
		if(tCompleta[i] >= 'a' && tCompleta[i] <= 'z')
			tCompleta[i] = tCompleta[i] + 'A' - 'a';

	return tCompleta;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
int PrinterTM300::PrintEncab()
{
    char *p, validarCuit = 1;
	abierto = true;
	static char hdrPkt[1000], tDoc[100], c_cuit[50];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	for(int index=0;index<10; index++)
		redondeo[index] = 0;
	
    PrintClerk();               // Imprime los datos del cajero.
	SetPie("");

	if(GlobalTF)
    {
        STRCPY(tDoc, "CUIT");
		STRCPY(c_cuit, c_numero);
        if(!_stricmp(c_condiva, "RI"))
        {
            f_letra =  'A';
            f_respon = 'I';
        }
        else if(!_stricmp(c_condiva, "NI"))
        {
            f_letra =  'A';
            f_respon = 'R';
        }
        else if(!_stricmp(c_condiva, "EX"))
        {
            f_letra =  'B';
            f_respon = 'E';
        }
        else if(!_stricmp(c_condiva, "NR"))
        {
            STRCPY(c_cuit, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'N';
			validarCuit = 0;
        }
        else if(!_stricmp(c_condiva, "MO"))
        {
            f_letra =  'B';
            f_respon = 'M';
        }
        else        // Consumidor final.
        {
            STRCPY(c_cuit, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'F';
			validarCuit = 0;
        }

		//   Corrige la variable global usada para analizar el comprobante en curso.
		GlobalTF = (f_letra == 'A'? 1: 2);

        //   Prepara el cuit.
        Trim(c_cuit);
        for(p = c_cuit; *p; )
            if(*p == '-')
                STRCPY(p, p+1);
            else
                p++;

		//   Si el cuit NO es valido, entonces desactiva el ticket factura y emite simplemente
		//   un ticket comun y corriente.
        if(validarCuit && !vCuit(c_cuit))
		{
			Alert("El CUIT que tiene cargado el cliente no esta bien.", "Se emitira ticket comun...");
			GlobalTF = 0;
	        xchgPkt("@");           // Abre ticket fiscal comun
			STRCPY(GlobalComprob, "TI");
		}
		else
		{
	       //   Prepara el resto de los campos.
			Trim(c_nombre);
			Trim(c_direcc);
			Trim(c_localidad);
			Trim(c_telef);
			ChkDes(c_nombre, NULL);
			ChkDes(c_localidad, NULL);
			ChkDes(c_direcc, NULL);
			ChkDes(c_telef, NULL);

			//   No pueden quedar campos vacios.
			if(strlen(c_nombre) == 0)		STRCPY(c_nombre, "x");
			if(strlen(c_direcc) == 0)		STRCPY(c_direcc, "x");
			if(strlen(c_localidad) == 0)	STRCPY(c_localidad, "x");
			if(strlen(c_telef) == 0)		STRCPY(c_telef, "x");
			c_nombre[35] = c_direcc[35] = c_localidad[35] = c_telef[35] = 0;

			//   Prepara el paquete a enviar a la impresora.
			char sep[2], letra[2], respon[2]; 
			sep[0] = SEP; letra[0] = f_letra; respon[0] = f_respon;
			sep[1] = letra[1] = respon[1] = 0;
			while(1)
			{
				STRCPY(hdrPkt, "\x60");								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "T");								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "C");								STRCAT(hdrPkt, sep);		// "S"
				STRCAT(hdrPkt, letra);								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "1");								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "P");								STRCAT(hdrPkt, sep);		// "P"
				STRCAT(hdrPkt, "17");								STRCAT(hdrPkt, sep);		// "17"
				STRCAT(hdrPkt, "I");								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, respon);								STRCAT(hdrPkt, sep);		
				STRCAT(hdrPkt, Completa(c_nombre, 19, true));		STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(".", 40, false));			STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, tDoc);								STRCAT(hdrPkt, sep);		// "CUIT"
				STRCAT(hdrPkt, c_cuit);								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "N");								STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(c_direcc, 35, true));		STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(c_localidad, 40, false));	STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(c_telef, 31, false));		STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(".", 40, true));			STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, Completa(".", 28, true));			STRCAT(hdrPkt, sep);
				STRCAT(hdrPkt, "C");		

				xchgPkt(hdrPkt);
				if((lstat.fiscstat & 0x8000) != 0)
				{
					if((flagMsg & FLAG_MSG_1) == 0)
					{
						Alert("No se puede abrir ticket fiscal.", "Se emitira ticket comun...");
						flagMsg |= FLAG_MSG_1;
					}

					GlobalTF = 0;
					xchgPkt("@");           // Abre ticket fiscal comun
					STRCPY(GlobalComprob, "TI");
					return 0;
				}

				if(FiscalForceZeta) PrintZeta();
				if(chkPrnStat() == 0) break;
			}

			GlobalComprob[0] = 'F';
			GlobalComprob[1] = f_letra;
			GlobalComprob[2] = 0;
		}
    }
    else
	{
		while(1)
		{
			xchgPkt("@");           // Abre ticket fiscal comun
			if(chkPrnStat() == 0) break;
			if(FiscalForceZeta) PrintZeta();
		}
		STRCPY(GlobalComprob, "TI");
	}

    return 0;
}

//   Imprime un item de linea.
int PrinterTM300::PrintItem(char *des, double mult, double monto, int tasa, char qual,
							int bulto, int variac, char *ades, double unitario, bool pesado, int cajaBulto)
{
    unsigned maxLen;
	char multLine[100];

    if(FactuFlag > 1) return 1;
    if(globalDelayPrint) return 0;

    maxLen = GlobalTF? 40: 20;
    ChkDes(des, tdes);
    ChkDes(ades, NULL);
	Trim(des);
	Trim(ades);

	//   Imprime la descripcion adicional del articulo.
	if(!abierto) PrintEncab();

	//   Envia el comando al impresor.
	if(qual == 'D')
	{
		qual = 'R';
		monto = fabs(monto);
	}

	//   Verifica que el impresor este en condiciones de imprimir
	while(true)
	{
		xchgPkt("*");
		if(chkPrnStat()) continue;
		if(chkFiscStat()) continue;
		break;
	}

	while(1)
	{
		// Imprime datos de la multiplicacion efectuada, si corresponde.
		if((fabs(mult - 1.000) > 0.001) || pesado || printCant)
		{
			if(cajaBulto > 0)
				sprintf(multLine, "%.0lf X %d X $%.2lf", mult, cajaBulto, unitario);
			else if(pesado)
				sprintf(multLine, "%.3lfKg X $%.2lf", mult, unitario);
			else
				sprintf(multLine, "%.0lf X $%.2lf", mult, unitario);
		}
		else
			multLine[0] = 0;

		// Prepara el paquete a enviar. Varia si estamos en ticket factura o no.
		if(GlobalTF)
		{
			double xtasa = (f_letra == 'A')? (double) tasa / 10000: 0;
			double dblUnitario =      (monto / (1 + xtasa) * 100);
			int intUnitario = (int)dblUnitario;

			// Busca la tasa a utilizar.
			int tasaIndex = 0;
			for(; tasaIndex < 10; tasaIndex++)
			{
				if(rtasa[tasaIndex] == 0)		// No la encontro, crea una nueva...
					rtasa[tasaIndex] = xtasa;
				if(rtasa[tasaIndex] == xtasa)	// Si la encontro.
					break;
			}

			// Almacena el redondeo acumulado en la tasa.
			double esteRedondeo = dblUnitario - intUnitario;
			if(qual == 'M')
				redondeo[tasaIndex] += esteRedondeo + (esteRedondeo * xtasa);
			else
				redondeo[tasaIndex] -= esteRedondeo + (esteRedondeo * xtasa);

			char *lin = ades;
			if(multLine[0] != 0)
			{
				PrintText(ades);
				lin = multLine;
			}
            sprintf(txbuff, "b%s%08.0lf%09d%04d%c%05d%d%s%s%s%04d%d",
                des, 
				1000.0,
				intUnitario,
				tasa, 
				qual, 
				bulto, 
				variac,
                lin, 
				otradesc, 
				otradesc2, 
				((f_respon=='R')? tasa / 2: 0), 
				0); 


		}
		else
		{
			// Debemos imprimir los textos antes.
			PrintText(ades);
			if(multLine[0] != 0) PrintText(multLine);
			ImprimirTextoDeVerdad();
				
			sprintf(txbuff, "B%c%s%c%08.0lf%c%09.0lf%c%04d%c%c%c%05d%c%d" ,
                SEP, des, 
				SEP, 1000.0,			// No mandamos cantidad NUNCA
				SEP, monto * 100,		// Mandamos el TOTAL a COBRAR SIEMPRE
				SEP, tasa, 
				SEP, qual, 
				SEP, bulto, 
				SEP, variac);
		}

		//   Envia los datos concretos del articulo. Verifica errores tipicos.
		xchgPkt(txbuff);
		bool fallo = (chkPrnStat() != 0);
		chkFiscStat();
		if(FiscalForceZeta) { PrintZeta(); continue; }

		//   Si el ticket no estaba abierto, lo abre.
		if(lstat.fiscstat & 0x20 || fallo)
		{
			ReprintDump(true);
            PrintEncab();
		}
		break;
	}

	// Elimina las descripciones usadas.
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
	return 0;
}

//   Imprime y chequea el subtotal de la operacion.
int PrinterTM300::PrintSubtot(char *msg, int pFlag)
{
	char sep[2], pf[2];

	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, tdes);
	pf[0] = GlobalTF? 'P': 0;
	sep[0] = SEP;
	pf[1] = sep[1] = 0;
	while(1)
	{
		STRCPY(txbuff, GlobalTF? "c": "C");		STRCAT(txbuff, sep);
		STRCAT(txbuff, "P");						STRCAT(txbuff, sep);
		STRCAT(txbuff, tdes);

		//sprintf(txbuff, "%c%c%c%c%s", (GlobalTF? 'c': 'C'), SEP, pFlag, SEP, tdes);
		xchgPkt(txbuff);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) break;
	}

	char *pc = rxBuff + 1;
	
	getfld(&pc);						// Estado de la impresora.
	getfld(&pc);						// Estado fiscal.
	getfld(&pc);						// No se usa.
	getfld(&pc);						// Cuenta items de linea.
	if(0 && GlobalTF)
	{
		getfld(&pc);					// Total de mercaderia.
		getfld(&pc);					// Total de IVA.
		getfld(&pc);					// Total pagado hasta el momento.
		getfld(&pc);					// Total impuestos internos porcentuales.
		getfld(&pc);					// Total impuestos internos fijos.
	}

	printerTot = atof_(getfld(&pc));
	for(int index=0; index<10; index++)
		printerTot += redondeo[index];	
	printerTot = ceil(printerTot);//(double)((int)printerTot);
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterTM300::PrintMPago(char *msg, double monto)
{
	char tMpagoDes[200];
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, tMpagoDes);

	while(true)
	{
		sprintf(txbuff, "%c%s%09.0lfT", (GlobalTF? 'd': 'D'), tMpagoDes, monto * 100);
		xchgPkt(txbuff);
		if(chkPrnStat() != 0) continue;
		if(chkFiscStat() != 0 || (lstat.fiscstat & 0x20) != 0)
		{
			ReprintDump(true);
			continue;
		}
		break;
	}


	return 0;
}

//   Cierra un ticket fiscal.
int PrinterTM300::PrintClose()
{
	this->ignoraError4 = 1;
	abierto = false;
    if(GlobalTF)
    {
		while(1)
		{
			sprintf(txbuff, "eT%c%s", f_letra, "*");
			xchgPkt(txbuff);
			if(chkPrnStat() == 0) break;
			if(FiscalForceZeta)
				PrintZeta();
		}
    }
    else
	{
		while(1)
		{
			xchgPkt("E");
			if(chkPrnStat() == 0) break;
			if(FiscalForceZeta)
				PrintZeta();
		}
	}
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
	this->ignoraError4 = 0;
	return 0;
}

int PrinterTM300::PrintDescto(char *msg, double monto)
{
	if(globalDelayPrint) return 0;

	ChkDes(msg, tdes);
    sprintf(txbuff, "%c%s%09.0lfD", 'D', tdes, monto * 100);
	xchgPkt(txbuff);
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterTM300::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	ignoraError4 = 1;
    sprintf(txbuff, "%c 000000000C", GlobalTF? 'd': 'D');
	xchgPkt(txbuff);
	abierto = false;
	ignoraError4 = 0;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterTM300::PrintZeta()
{
	char sep[2];
	sep[0] = SEP;
	sep[1] = 0;
	while(1)
	{
		STRCPY(txbuff, "9");		STRCAT(txbuff, sep);
		STRCAT(txbuff, "Z");		STRCAT(txbuff, sep);
		STRCAT(txbuff, "P");
		xchgPkt(txbuff);
		if(chkPrnStat() != 0)
		{
			xchgPkt(txbuff);
			if(chkPrnStat() == 0) break;
		}
		else
			break;
	}

	FiscalForceZeta = 0;
	char *pc = rxBuff;
	getfld(&pc);
	getfld(&pc);
	getfld(&pc);
	return atoi(getfld(&pc));
}

//   Envia un texto fiscal al ticket. En este impresor, solo manda los textos a un cache desde
//   el cual se imprime posteriormente.
int PrinterTM300::PrintText(char *msg)
{
	ChkDes(msg, tdes);
	if(strlen(otradesc) == 0)
		STRCPY(otradesc, tdes);
	else
		STRCPY(otradesc2, tdes);
	return 0;
}

//  Este es el metodo que se llama cuando se quieren imprimir los textos cacheados.
void PrinterTM300::ImprimirTextoDeVerdad()
{
	ImprimeAuxiliar(otradesc);
	ImprimeAuxiliar(otradesc2);
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
}

/// Metodo auxiliar para imprimir UN texto cachedo, si tiene algo valido.
void PrinterTM300::ImprimeAuxiliar(char *msg)
{
	if(strlen(msg) == 0) return;
	while(1)
	{
		if(strlen(msg) > 26) msg[26] = 0;
		sprintf(txbuff2, "A%c%s", SEP, msg);
		xchgPkt(txbuff2);
		if(chkPrnStat() != 0)
		{
			xchgPkt(txbuff2);
			if(chkPrnStat() == 0) break;
		}
		else
			break;

		if(FiscalForceZeta)
			PrintZeta();
	}
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterTM300::OpenVoucher()
{
	abierto = true;
	while(1)
	{
		xchgPkt("\x48");
		if(chkPrnStat() != 0)
		{
			//   Reintenta una vez...
			for(double d = 300; d > 0; d--)
				;
			xchgPkt("\x48");
			if(chkPrnStat() == 0) break;
		}
		else
			break;

		if(FiscalForceZeta) PrintZeta();
	}
	return 0;
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterTM300::OpenVoucher2()
{
	OpenVoucher();
	return 0;
}

//   Imprime un texto en un comprobante NO fiscal.
int PrinterTM300::PrintVoucher(char *des)
{
	if(!abierto) return 0;
	while(1)
	{
		ChkDes(des, NULL);
		sprintf(txbuff, "\x49%c%s", SEP, des);
		xchgPkt(txbuff);
		if(chkPrnStat() != 0)
		{
			for(double d = 300; d > 0; d--)
				;
			xchgPkt(txbuff);
			if(chkPrnStat() == 0) break;
		}
		else
			break;

		if(FiscalForceZeta) PrintZeta();
	}
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterTM300::CloseVoucher()
{
	while(1)
	{
		xchgPkt("\x4a");
		if(chkPrnStat() != 0)
		{
			for(double d = 300; d > 0; d--)
				;
			xchgPkt("\x4a");
			if(chkPrnStat() == 0) break;
		}
		else
			break;

		if(FiscalForceZeta) PrintZeta();
	}
	abierto = false;
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterTM300::CloseVoucher2()
{
	CloseVoucher();
	return 0;
}
