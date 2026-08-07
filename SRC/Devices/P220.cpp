/*
 *   P220.c
 *   Driver del impresor EPSON TM220F - Nuevo protocolo
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif


//  Helpers para preparar comando.
static int cmdLen;

static void AppendToCmd(char ch)
{
	txbuff[cmdLen++] = SEP;
	txbuff[cmdLen++] = ch;
}

static void AppendToCmd(int pInt, int places)
{
	char tbuff[40], fmt[40];
	sprintf(fmt, "%%0%dd", places);
	sprintf(tbuff, fmt, pInt);
	tbuff[places] = 0;
	int size = strlen(tbuff);

	txbuff[cmdLen++] = SEP;
	memmove(&txbuff[cmdLen], tbuff, size);
	cmdLen += size;
}

static void AppendToCmd(int pInt)
{
	char tbuff[40];
	sprintf(tbuff, "%d", pInt);
	int size = strlen(tbuff);

	txbuff[cmdLen++] = SEP;
	memmove(&txbuff[cmdLen], tbuff, size);
	cmdLen += size;
}

static void AppendToCmd(char *str)
{
	txbuff[cmdLen++] = SEP;
	int size = strlen(str);
	memmove(&txbuff[cmdLen], str, size);
	cmdLen += size;
}

static void AppendCmdBinary(int binValue)
{
	if(cmdLen != 0) txbuff[cmdLen++] = SEP;
	txbuff[cmdLen++] = (char)(binValue / 256);
	txbuff[cmdLen++] = (char)(binValue % 256);
}

static void PrepareCmd(int cmd, int exten)
{
	cmdLen = 0;
	AppendCmdBinary(cmd);
	AppendCmdBinary(exten);
}

//  Variables de estado.
//extern long prnstat, fiscstat, ultimaZeta, ultimoBcompleto, ultimoB, ultimoAcompleto, ultimoA,
//	ultimoDNF, ultimoNFH, numeroRef;
long prnstat, fiscstat, ultimaZeta, ultimoBcompleto, ultimoB, ultimoAcompleto, ultimoA,
	ultimoDNF, ultimoNFH, numeroRef;

//   Datos de este solo modulo   

//extern char *pc,                // Para explorar respuestas.
//     f_letra,           //  Para los tickets fiscales.
//	 f_respon,			//  Tipo de responsabilidad del cliente.
//	 tdes[];
// char tdes[];

//   Clase que encapsula el manejo de este impresor.

//extern "C" {
//PrinterTM220 P_TM220;
//};

//  Flag usado para mostrar algunos mensajes una sola vez.
int flagMsg;//extern int flagMsg;
#define FLAG_MSG_1	1	/* Para mostrar 1 vez el msg "No puede abrir ticket fiscal, ..."  */

extern char tCompleta[];
bool abierto; //extern bool abierto;
char *Completa(char *s, int len, bool addEOL);

//  Constructor
PrinterTM220::PrinterTM220()
{
    //  Activa las capacidades del impresor.
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
    PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY | PRN_CANLOGO | PRN_CANVOUCHER_TJ;
	GlobalVoucherFlag = 1;		// Puede emitir vouchers

	STRCPY(vendor, "EPSON 220AF");
	hdrChanged = false;
	seqHigh = true;
	inVoucher = false;
	pieChanged = false;	
	pie[0] = 0;
	pie2[0] = 0;
	pie3[0] = 0;
}

// Retorna datos del propietario del impresor
void PrinterTM220::Owner(char *cuit, char *pv)
{
	double llimfiscal = 0;
	SetPie("");
	SetHeader("");
	hdrChanged = false;

	while(true)
	{
		PrepareCmd(0x0507, 0x0000);
		xchgPkt2(txbuff, cmdLen);
		if(chkFiscStat()) continue;
		if(chkPrnStat()) continue;
		break;
	}

	STRCPY(cuit, ptrFields[1]);
	STRCPY(pv, ptrFields[2]);
	STRCPY(ptovta, pv);
	WriteBootLog("Limite Fiscal: x 100");
	WriteBootLog(ptrFields[12]);
	llimfiscal = atof(ptrFields[12])/100;//Agregado para obtener el limite fiscal del impresor	
	if (checkLimFisc)	
		limTicket = llimfiscal;
	else
	{
		if (limTicket > llimfiscal)
			limTicket = llimfiscal;
	}
	
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterTM220::ChkPrnXonXoff()
{
	while(chk() == 1)
		if(FiscalForceZeta)
			PrintZeta();
	return 0;
}

void PrinterTM220::CheckStatus()
{
	PrepareCmd(0x0001, 0x0000);
	xchgPkt2(txbuff, cmdLen);
	chkFiscStat();
}

// Ajustamos el redondeo en los Ticket A
void PrinterTM220::AjustarRedondeo(double redondeo)
{
	int intUnitario = (int)(redondeo * 10000);
	PrepareCmd(
		GlobalTF? 0x0B02: 0x0A02,
		GlobalTF? 0x0018: 0x0010
	);
	AppendToCmd("");				// Linea 1 de descripcion
	AppendToCmd("");				// Linea 2 de descripcion
	AppendToCmd("");				// Linea 3 de descripcion
	AppendToCmd("");				// Linea 4 de descripcion
	AppendToCmd("Redondeo");		// Descripcion del item (no opcional)
	AppendToCmd(10000);				// Cantidad
	AppendToCmd(intUnitario);		// Precio Unitario
	AppendToCmd(0);					// Tasa de impuestos
	AppendToCmd("");				// Impuestos internos
	AppendToCmd("");				// Coeficientes
	//xchgPkt2(txbuff, cmdLen);
}
// Imprime la percepcion en la impresor fiscal
int PrinterTM220::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
	int intMonto = (int)(permonto * 100);
	int i = 0;
	int tverror = 0;
	char tvaux[70];
	int result = 0;
	BDecimal xtasa = (((double) pertasa) / 10000);
	xtasa.ApplyRound(5);
	xtasa *= 10000;
	int intTasa = xtasa.ToInt32();
	if (permonto < 0.01)
		return result;
	while(true)
	{
		PrepareCmd(0x0B20, 0x0200);// 0x0200); //PrepareCmd(0x0B20,(pertasa == 0 ? 0x0200 : 0x0100));
		AppendToCmd(perdescrip);
		AppendToCmd(intMonto);
		AppendToCmd(0);//intTasa);
		//AppendToCmd("T");
		xchgPkt2(txbuff, cmdLen);
		tverror = chkPrnStat();
		if(tverror == 0)
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

int PrinterTM220::PrintRecargo(char *msg, double monto)
{
	if(globalDelayPrint) return 0;
	ChkDes(msg, tdes);

	PrepareCmd(GlobalTF? 0x0B04: 0x0A04, 0x0001);		// 1 en la exten para hacer recargo
	AppendToCmd(msg);
	AppendToCmd((int)(monto * 100));
	xchgPkt2(txbuff, cmdLen);
	return 0;
}

// Imprime un voucher rapido con 3 lineas de mensajes (las ultimas 2 opcionales).
void PrinterTM220::FastVoucher(bool pHeader, char *l1, char *l2, char *l3)
{
	PrepareCmd(0x0E30, 0x0004 + (pHeader? 0x0000: 0x0400)); 
	AppendToCmd((char *)((l1 != NULL)? PrepararTexto(l1): ""));
	AppendToCmd((char *)((l2 != NULL)? PrepararTexto(l2): ""));
	AppendToCmd((char *)((l3 != NULL)? PrepararTexto(l3): ""));
	for(int i=0; i<27; i++)
		AppendToCmd("");

	AppendToCmd(0);			// Nro de linea de reemplazo de pie
	AppendToCmd("");
	AppendToCmd(0);			// Nro de linea de reemplazo de pie
	AppendToCmd("");
	AppendToCmd(0);			// Nro de linea de reemplazo de pie
	AppendToCmd("");

	xchgPkt2(txbuff, cmdLen);
 }

// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1=ticket b/c/ factura b/c
//    tipo: 2=tikect a factura a
int PrinterTM220::GetTickNro(int cual)
{
	// Envia el comando.
	ignoraError4 = 1;
	PrepareCmd(0x080A, 0x0000);
	int result = xchgPkt2(txbuff, cmdLen);
	ignoraError4 = 0;

	//   Desensambla los distintos campos del estado.
	if(result == 1)
	{
		ultimaZeta = atoi(ptrFields[2]);
		ultimoA = atoi(ptrFields[3]);
		ultimoB = atoi(ptrFields[4]);
		ultimoDNF = atoi(ptrFields[7]);
		ultimoNFH = atoi(ptrFields[8]);
		numeroRef = ultimoB;
	}

	// Retorna el numeo pedido
	if(cual == 1)					// GlobalTF --> 0=CF, 1=A, 2=B
		return ultimoA;
	else if(cual == 10)
		return ultimoDNF;
	else
		return ultimoB;
}
//Se ejecuta cuando se Inicia el POS
//
void PrinterTM220::ObtenerAbierto(int &numticket, double &monto)
{
	// Envia el comando.
	ignoraError4 = 1;
	PrepareCmd(0x0A0A, 0x0000);
	int result = xchgPkt2(txbuff, cmdLen);
	ignoraError4 = 0;

	numticket = 0;
	monto = 0;
	//   Desensambla los distintos campos del estado.
	if(result == 1)
	{
		numticket = atoi(ptrFields[0]);
		monto = atof(ptrFields[1])/100;
	}
	else
	{
		ignoraError4 = 1;
		PrepareCmd(0x0B0A, 0x0000);
		result = 0;
		result = xchgPkt2(txbuff, cmdLen);
		ignoraError4 = 0;
		if (result == 1)
		{
			numticket = atoi(ptrFields[0]);
			monto = atof(ptrFields[1])/100;
		}
	}
	//cancela el ticket abierto
	//this->PrintCancel();
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispo-
//   sitivo presente en el impresor TM300 para activarla.
int PrinterTM220::OpenDrwr()
{
	PrepareCmd(0x0707, 0x0001);
	xchgPkt2(txbuff, cmdLen);
	return 0;
}

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
char *PrinterTM220::ChkDes(char *s, char *dst)
{
	if(dst == NULL && strlen(s) == 0) return s;
	if(dst == NULL) dst = s;
	char *ret = dst;
	while(*s)
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			int flag = 0;
			switch(*++s)
			{
				case '4': flag |= 4; break;
			}
			*dst++ = '\x1b';
			*dst++ = (char) flag;
		}
		else
		{
			if(*s < ' ' || *s > 'z')
				*dst++ = ' ';
			else
				*dst++ = *s;
		}
		s++;
	}

	if(strlen(ret) > 40) *(ret + 40) = 0;
	*dst = 0;
	return ret;
}

char *PrinterTM220::PrepararTexto(char *texto, bool centrar)
{
	int largo = strlen(texto);
	char *res = texto;
	bool hayDobleAncho = false;
	for(int i = 0; i < largo; i++)
	{
		if((texto[i] & 0xff) == 241)
			texto[i] = 'n';
		else if((largo - i > 2 ) && (texto[i] == '@' || texto[i] == '&'))
		{
			texto[i++] = 27;
			switch(texto[i])
			{
				case '4': texto[i] = 0x04; hayDobleAncho = true; break;
				default: texto[i] = 0;
			}
		}
	}

	if(centrar)
	{
		largo = strlen(texto);
		int agregar = ((hayDobleAncho? 23: 41) - largo) / 2;
		//int agregar = ((hayDobleAncho? 23: 41) - strlen(texto)) / 2;
		int offset = hayDobleAncho? 2: 0;
		if(agregar > 0)
		{
			memmove(texto + offset + agregar, texto + offset, largo + 1);
			//memmove(texto + offset + agregar, texto + offset, strlen(texto) + 1);
			while(agregar > 0)
			{
				texto[offset + agregar - 1] = ' ';
				agregar--;
			}
		}
	}
	return res;
}

//   Descarga en el printer todos los datos instalables.
int PrinterTM220::PrinterUpLoad()
{
	int i;

    //  Mensaje!!!
	sprintf(msg, "Espere. Preparando impresor %s...", vendor);
    SendMsg(msg, "");

    //  Primero, instala los encabezamientos. La ultima linea del encabezado se reserva
	//  Para el nombre del cajero
	if(cLinesLogo >= 8) cLinesLogo = 8;
	for(i=0; i<cLinesLogo; i++)
	{
		if(strlen(LogoLines[i]) == 0) LogoLines[i] = " ";
		PrepararTexto(LogoLines[i]);
		PrepareCmd(0x0508, 0x0000);
		AppendToCmd(i + 1);			// Numero de linea
		AppendToCmd(LogoLines[i]);	// Texto del pie
		xchgPkt2(txbuff, cmdLen);		
	}
	availHeaderLine = i + 1;
	while(i < 8)              // Vacia las lineas no usadas.
	{
		PrepareCmd(0x0508, 0x0000);
		AppendToCmd(i + 1);
		AppendToCmd("");
		xchgPkt2(txbuff, cmdLen);
		i++;
	}

	// Vaciamos la linea 9, que la usamos para separar datos del caja/cajero del resto
	// del encabezamiento.
	PrepareCmd(0x0508, 0x0000);
	AppendToCmd(9);
	AppendToCmd(" ");
	xchgPkt2(txbuff, cmdLen);

	// Programa el pie del ticket.
	if(cLinesPie >= 10) cLinesPie = 9;
	for(i=0; i<cLinesPie; i++)
	{
		if(strlen(LogoPie[i]) == 0) LogoPie[i] = " ";
		PrepararTexto(LogoPie[i]);
		PrepareCmd(0x050A, 0x0000);
		AppendToCmd(i + 1);			// Numero de linea
		AppendToCmd(LogoPie[i]);	// Texto del pie
		xchgPkt2(txbuff, cmdLen);
		if ((i + 1) == 2)		
			STRCPY(contpie2, LogoPie[i]); //almacena la linea2
		else if ((i + 1) == 3)
			STRCPY(contpie3, LogoPie[i]); //almacena la linea3
	}
	while(i < 10)              // Vacia las lineas no usadas.
	{
		PrepareCmd(0x050A, 0x0000);
		AppendToCmd(i + 1);
		AppendToCmd("");
		xchgPkt2(txbuff, cmdLen);
		i++;
	}

	// Ahora los mensajes para el ticket factura. La primera linea (ex linea 50) DEBE tener algo
	if(LogoEpson[0][0] = ' ' || LogoEpson[0][0] == 0)
	{
		LogoEpson[0][0] = '.';
		LogoEpson[0][1] = 0;
	}
	for(i=0; i<cLinesEpson; i++)
	{
		PrepareCmd(0x050E, i);			// La extension del comando es el nro de linea.
		PrepararTexto(LogoEpson[i]);
		AppendToCmd(LogoEpson[i]);
		xchgPkt2(txbuff, cmdLen);
	}
	while(i < 10)              // Vacia las lineas no usadas.
	{
		PrepareCmd(0x050E, i);
		AppendToCmd("");
		xchgPkt2(txbuff, cmdLen);
		i++;
	}

	// Programa la fecha y hora. La fecha la obtenemos aaaa/mm/dd pero debemos pasarla como dd/mm/aa...
	int ufecha = date() - 20000000;
	int dia = ufecha % 100; ufecha /= 100;
	int mes = ufecha % 100; ufecha /= 100;
	ufecha = dia * 10000 + mes * 100 + ufecha;
	PrepareCmd(0x0501, 0x0000);
	AppendToCmd(ufecha, 6);
	AppendToCmd(time__(), 6);
	xchgPkt2(txbuff, cmdLen);

	// Efectua operaciones custom si el impresor no es realmente un epson (como el bematech)
	CompleteInitialization();

    ClearMsg();
	return 0;
}

//  Completa la inicializacion. Placeholder para extensiones de la inicializacion que puedan ser necesarias
//  por impresores que extienden esta clase.

void PrinterTM220::CompleteInitialization()
{
	PrepareCmd(0x0A08, 0x0Cff);
	AppendToCmd("PAGO");
	xchgPkt2(txbuff, cmdLen);

	PrepareCmd(0x0B08, 0x0FC7);
	AppendToCmd("PAGO");
	xchgPkt2(txbuff, cmdLen);	
}

//  Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterTM220::SetPie(char *msg)
{
	STRCPY(pie, msg);
	PrepararTexto(pie, true);
	return 0;
}

//  Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterTM220::SetPie(char *msg, int linea)
{
	switch (linea)
	{
		case 1:
			STRCPY(pie, msg);
			PrepararTexto(pie, true);
			break;
		case 2:
			STRCPY(pie2, msg);
			PrepararTexto(pie2, true);
			break;
		case 3:
			STRCPY(pie3, msg);
			PrepararTexto(pie3, true);
			break;
		default:
			STRCPY(pie, msg);
			PrepararTexto(pie, true);
			break;
	}
	pieChanged = true;
	return 0;
}
////  Cambia el mensaje de la ultima linea del PIE del ticket.
//int PrinterTM220::SetPie2(char *msg)
//{
//	STRCPY(pie2, msg);
//	PrepararTexto(pie2, true);
//	return 0;
//}

//  Cambia el mensaje de la ultima linea del HEADER del ticket.
int PrinterTM220::SetHeader(char *msg)
{
	if(availHeaderLine > 8) availHeaderLine = 8;
	PrepareCmd(0x0508, 0x0000);
	AppendToCmd(availHeaderLine);
	AppendToCmd(msg);
	xchgPkt2(txbuff, cmdLen);
	hdrChanged = true;
	return 0;
}

//   Inicializa el impresor fiscal con los datos que se le indican...
int PrinterTM220::FiscalInit(char *cuit, char *rs, char *regnro, char *fec, char *ptovta,
							   char *initAct, char *ibrut, char *respon)
{
	return 0;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
int PrinterTM220::PrintEncab()
{
    char *p, validarCuit = 1, f_TipoDoc;
	 int reintentos = 3;
	abierto = true;
	static char hdrPkt[1000], tDoc[100], c_cuit[50];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	
    PrintClerk();               // Imprime los datos del cajero.
	SetPie("");

	if(GlobalTF)
    {
		f_TipoDoc = 'T';
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
			f_TipoDoc = 'D';
            f_letra =  'B';
            f_respon = 'F';
			Trim(c_cuit);
			if(strlen(c_cuit) == 0)/// || strlen(c_cuit) > 8)
				STRCPY(c_cuit, "11111111");
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
			PrepareCmd(0x0A01, 0x0000);
	        xchgPkt2(txbuff, cmdLen);           // Abre ticket fiscal comun
			STRCPY(GlobalComprob, "TI");
		}
		else
		{
	       //   Prepara el resto de los campos.
			Trim(c_nombre);
			Trim(c_direcc);
			Trim(c_localidad);
			Trim(c_telef);

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
				PrepareCmd(0x0B01, 0x0000);
				AppendToCmd(c_nombre);				// Nombre 1 del comprador (No opcional)
				AppendToCmd("");					// Nombre 2 del comprador
				AppendToCmd(c_direcc);				// Domicilio 1 del comprador (No opcional)
				AppendToCmd(c_localidad);			// Domicilio 2 del comprador
				AppendToCmd(c_telef);				// Domicilio 3 del comprador
				AppendToCmd(f_TipoDoc);				// 'T' = cuit, 'D' = DNI, otros no se soportan
				AppendToCmd(c_cuit);				// Numero de uit o documento.
				AppendToCmd(f_respon);				// Reponsabilidad fiscal.
				AppendToCmd("Ninguno");				// Linea 1 remitos asociados (No opcional)
				AppendToCmd("");					// Linea 2 remitos asociados
				AppendToCmd("");					// Linea 3 remitos asociados.

				int result = xchgPkt2(txbuff, cmdLen);
				if(result != 1)
				{
					if((flagMsg & FLAG_MSG_1) == 0)
					{
						Alert("No se puede abrir ticket fiscal.", "Se emitira ticket comun...");
						flagMsg |= FLAG_MSG_1;
					}

					GlobalTF = 0;
					PrepareCmd(0x0A01, 0x0000);
				   int result = xchgPkt2(txbuff, cmdLen);           // Abre ticket fiscal comun
					STRCPY(GlobalComprob, "TI");
					if((chkPrnStat() == 0) && (chkRta(result) == 0)) break;
					if(FiscalForceZeta) PrintZeta();

					if (chkRta(result) > 0 && reintentos < 1)
					{
						Alert(
							"La impresora fiscal tiene problemas. Por favor, apaguela y",
							"vuelvala a encender. No puedo abrir Ticket.");
						break;
					}
					for(double d = 300; d > 0; d--)
						;
					reintentos--;
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
			PrepareCmd(0x0A01, 0x0000);
	      int result = xchgPkt2(txbuff, cmdLen);           // Abre ticket fiscal comun
			

			if((chkPrnStat() == 0) && (chkRta(result) == 0)) break;
			if(FiscalForceZeta) PrintZeta();
			
			if (chkRta(result) > 0 && reintentos < 1)
			{
				Alert(
					"La impresora fiscal tiene problemas. Por favor, apaguela y",
					"vuelvala a encender. No puedo abrir ticket.");
				break;
			}
			for(double d = 300; d > 0; d--)
				;
			reintentos--;
			
		}
		STRCPY(GlobalComprob, "TI");
	}

    return 0;
}

//   Imprime un item de linea.
int PrinterTM220::PrintItem(char *des, double mult, double monto, int tasa, char qual,
							int bulto, int variac, char *ades, double unitario, bool pesado, int cajaBulto)
{
    unsigned maxLen;
	char multLine[60];
	
    if(FactuFlag > 1) return 1;
    if(globalDelayPrint) return 0;

    maxLen = GlobalTF? 40: 20;
    ChkDes(des, NULL);
    ChkDes(ades, NULL);
	Trim(des);
	if(strlen(ades) != 0) Trim(ades);

	//   Imprime la descripcion adicional del articulo.
	if(!abierto) 
	{
		while(true)
		{
			PrintEncab();
			if(chkPrnStat() == 0) break;
		}
	}

	//   Envia el comando al impresor.
	while(1)
	{
		// Imprime datos de la multiplicacion efectuada, si corresponde.
		if((fabs(mult - 1.000) > 0.001) || pesado || cajaBulto > 0 || printCant)
		{
			if(cajaBulto > 0)
				sprintf(multLine, "%.0lf X %d X $%.2lf", mult, cajaBulto, unitario);
			else if(pesado)
				sprintf(multLine, "%.3lfKg X $%.2lf", mult, unitario);
			else
				sprintf(multLine, "%.0lf X $%.2lf", mult, unitario);
		}
		else
			STRCPY(multLine, "");

		BDecimal xtasa = (((double) tasa) / 10000);
		xtasa.ApplyRound(5);
		xtasa *= 10000;
		int intTasa = xtasa.ToInt32();
		//int intUnitario = (int)(fabs(monto) * 10000);
		char intUnitario[20];
		sprintf(intUnitario, "%.0lf", (fabs(monto) * 10000));

		int exten = GlobalTF? 0x0018: 0x0010;
		if(qual == 'm') exten += 1;
		if(qual == 'D') 
		{
			if(GlobalTF)
				exten = 0x0C;
			else
				exten += 4;
		}

		PrepareCmd(GlobalTF? 0x0B02: 0x0A02, exten);

		Trim(otradesc);
		if(memcmp(otradesc2, "------------", 12) == 0)
			otradesc2[0] = 0;

		char *l1 = des, *l2 = "", *l3 = "";
		if(strlen(multLine) != 0)
		{
			l2 = l1;
			l1 = multLine;
		}
		if(strlen(ades) != 0)
		{
			if(l2[0] != 0) l3 = l2;
			l2 = l1;
			l1 = ades;
		}

		AppendToCmd(otradesc);			// Linea 2 de descripcion
		AppendToCmd(otradesc2);			// Linea 3 de descripcion
		AppendToCmd(l2);				// Linea 4 de descripcion
		AppendToCmd(l3);				// Linea 5 de descripcion
		AppendToCmd(l1);				// Linea 1 de descripcion del item (no opcional)
		AppendToCmd(10000);				// Cantidad
		AppendToCmd(intUnitario);		// Precio Unitario
		AppendToCmd(intTasa);			// Tasa de impuestos
		AppendToCmd("");				// Impuestos internos
		AppendToCmd("");				// Coeficientes

		//   Envia los datos concretos del articulo. Verifica errores tipicos.
		xchgPkt2(txbuff, cmdLen);
		if(chkPrnStat())  continue;
		if(chkFiscStat()) continue;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		break;
	}

	// Elimina las descripciones usadas.
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
	return 0;
}

//   Imprime y chequea el subtotal de la operacion.
int PrinterTM220::PrintSubtot(char *msg, int pFlag)
{
	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	pFlag = pFlag? 1: 0;
	while(1)
	{
		PrepareCmd(
			GlobalTF? 0x0B03: 0x0A03,
			GlobalTF? pFlag + 0x0008: pFlag);		

		xchgPkt2(txbuff, cmdLen);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) break;
	}

	printerTot = atof_(ptrFields[0]);
	return 1;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterTM220::PrintMPago(char *msg, double monto)
{
	char t1[50], t2[50];
	

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	ChkDes(msg, t1);		
	ChkDes(mpagAdicDesc, t2);

	PrepareCmd((GlobalTF? 0x0B05: 0x0A05), 0x0000);
	AppendToCmd(t2);		
	AppendToCmd(t1);
	BDecimal vmonto(monto, 2);	
	vmonto *=100;
	//AppendToCmd((int)(monto * 100));
	AppendToCmd(vmonto.ToInt32());
	xchgPkt2(txbuff, cmdLen);	
	STRCPY(mpagAdicDesc, "");	
	return 0;
}

//   Cierra un ticket fiscal.
int PrinterTM220::PrintClose()
{
	////char t1[50], t2[50];
	int reintentos = 3;	
	
	if (!docFiscStat()) //no hay un documento fiscal abierto
	{
		abierto = false;
		return 0;
	}
	
	////// Primero envia al impresor los medios de pago.
	////for(int i=0; i<stackPagos->Count(); i++)
	////{
	////	PrinterItemPago *pi = (PrinterItemPago *)stackPagos->Item(i);
	////	ChkDes(pi->Descrip, t1);		
	////	ChkDes(pi->DescripAdic, t2);

	////	PrepareCmd((GlobalTF? 0x0B05: 0x0A05), 0x0000);
	////	AppendToCmd(t2);		
	////	AppendToCmd(t1);
	////	AppendToCmd((int)(pi->Monto * 100));
	////    xchgPkt2(txbuff, cmdLen);
	////}	
	////stackPagos->Clear();	
	// Ahora ciera el ticket
	this->ignoraError4 = 1;
	abierto = false;
	if(pie[0] == 0) STRCPY(pie, " ");	
	while(true)
	{
		if (pieChanged) //cambio alguno de los pies, en el nuevo metodo
		{		
			PrepareCmd((GlobalTF? 0x0B06: 0x0A06), 0x0001);
			AppendToCmd(1);			// Nro de linea de reemplazo 1
			AppendToCmd(pie);		// Texto de reemplazo de pie 1
			AppendToCmd(2);		// Nro de linea de reemplazo 2
			AppendToCmd(strlen(pie2)== 0 ? contpie2	: pie2);		// Texto de reemplazo de pie 2
			AppendToCmd(3);		// Nro de linea de reemplazo 3
			AppendToCmd(strlen(pie3)== 0 ? contpie3	: pie3);		// Texto de reemplazo de pie 3
		}
		else
		{
			PrepareCmd((GlobalTF? 0x0B06: 0x0A06), 0x0001);
			AppendToCmd(1);			// Nro de linea de reemplazo 1
			AppendToCmd(pie);		// Texto de reemplazo de pie 1
			AppendToCmd("");		// Nro de linea de reemplazo 2
			AppendToCmd("");		// Texto de reemplazo de pie 2
			AppendToCmd("");		// Nro de linea de reemplazo 3
			AppendToCmd("");		// Texto de reemplazo de pie 4
		}

		int result = xchgPkt2(txbuff, cmdLen);		
		if((chkPrnStat() == 0) && (chkRta(result) == 0))
			break;
		if (reintentos < 1)
		{
			ClearMsg();
			Alert(
				"La impresora fiscal tiene problemas. Por favor, apaguela y",
				"vuelvala a encender. No puedo Cerrar ticket.");
			break;
		}
		for(double d = 300; d > 0; d--)
			;
		reintentos--;
	}
	if (pieChanged) //vuelve a la programacion por defecto la inicial
	{
		for (int i = 0; i < 3; i++)
		{		
			PrepareCmd(0x050A, 0x0000);
			AppendToCmd(i + 1);			// Numero de linea
			AppendToCmd(LogoPie[i]);	// Texto del pie
			xchgPkt2(txbuff, cmdLen);
		}
	}
	pie[0] = 0;	
	pie2[0] = 0;
	pie3[0] = 0;
	
	pieChanged = false;	
	this->ignoraError4 = 0;
	return 0;
}

int PrinterTM220::PrintDescto(char *msg, double monto)
{
	if(globalDelayPrint) return 0;
	ChkDes(msg, tdes);

	PrepareCmd(GlobalTF? 0x0B04: 0x0A04, 0x0000);		// 1 en la exten para hacer recargo
	AppendToCmd(msg);
	AppendToCmd((int)(monto * 100));
	xchgPkt2(txbuff, cmdLen);
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterTM220::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	ignoraError4 = 1;
	PrepareCmd(0x0B07, 0x0000);				// Cancela ticket fiscal A
	xchgPkt2(txbuff, cmdLen);
	PrepareCmd(0x0A07, 0x0000);				// Cancela ticket fiscal B o ticket comun
	xchgPkt2(txbuff, cmdLen);
	CloseVoucher();							// Cancela doc no fiscal en curso.
	abierto = false;
	ignoraError4 = 0;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterTM220::PrintZeta()
{
	PrepareCmd(0x0801, 0x0400);
	xchgPkt2(txbuff, cmdLen);
	return atoi(ptrFields[0]);
}

//   Envia un texto fiscal al ticket. En este impresor, solo manda los textos a un cache desde
//   el cual se imprime posteriormente.
int PrinterTM220::PrintText(char *msg)
{	
	ChkDes(msg, tdes);
	if(strlen(otradesc) == 0)
		STRCPY(otradesc, tdes);
	else
		STRCPY(otradesc2, tdes);
	return 0;
}

//  Este es el metodo que se llama cuando se quieren imprimir los textos cacheados.
void PrinterTM220::ImprimirTextoDeVerdad()
{
	ImprimeAuxiliar(otradesc);
	ImprimeAuxiliar(otradesc2);
	STRCPY(otradesc, "");
	STRCPY(otradesc2, "");
}

/// Metodo auxiliar para imprimir UN texto cachedo, si tiene algo valido.
void PrinterTM220::ImprimeAuxiliar(char *msg)
{
	return;
	if(strlen(msg) == 0) return;
	int reintentos = 2;
	while(1)
	{
		sprintf(txbuff2, "A%c%s", SEP, msg);
		xchgPkt(txbuff2);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterTM220::OpenVoucher()
{
	abierto = true;
	int reintentos = 2;
	while(true)
	{
		PrepareCmd(0x0E01, 0x0000);
		xchgPkt2(txbuff, cmdLen);

		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
	inVoucher = true;
	return 0;
}

int PrinterTM220::OpenVoucher2()
{
	abierto = true;
	int reintentos = 2;
	while(true)
	{
		PrepareCmd(0x0E01, 0x0400);
		xchgPkt2(txbuff, cmdLen);

		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
	inVoucher = true;
	return 0;
}


//   Imprime un texto en un comprobante NO fiscal.
int PrinterTM220::PrintVoucher(char *des)
{
	if(!abierto || !inVoucher) return 0;
	ChkDes(des, NULL);
	int reintentos = 2;
	while(true)
	{
		PrepareCmd(0x0E02, 0x0000);
		AppendToCmd(des);
		xchgPkt2(txbuff, cmdLen);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterTM220::CloseVoucher()
{
	int reintentos = 2;
	if(!inVoucher) return 0;
	while(true)
	{
		PrepareCmd(0x0E06, 0x0001);
		AppendToCmd("");				// # de linea a reemplazar en la cola 1
		AppendToCmd("");				// Texto para la cola 1.
		AppendToCmd("");				// # de linea a reemplazar en la cola 2
		AppendToCmd("");				// Texto para la cola 2.
		AppendToCmd("");				// # de linea a reemplazar en la cola 2
		AppendToCmd("");				// Texto para la cola 2.

		xchgPkt2(txbuff, cmdLen);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
	inVoucher = abierto = false;
	return 0;
}

int PrinterTM220::CloseVoucher2()
{
	int i;
	int reintentos = 2;
	if(!inVoucher) return 0;
	while(true)
	{
		PrepareCmd(0x0E06, 0x0401);
		AppendToCmd("");				// # de linea a reemplazar en la cola 1
		AppendToCmd("");				// Texto para la cola 1.
		AppendToCmd("");				// # de linea a reemplazar en la cola 2
		AppendToCmd("");				// Texto para la cola 2.
		AppendToCmd("");				// # de linea a reemplazar en la cola 2
		AppendToCmd("");				// Texto para la cola 2.

		xchgPkt2(txbuff, cmdLen);
		if(chkPrnStat() == 0) break;
		if(FiscalForceZeta) { PrintZeta(); continue; }
		if(reintentos-- <= 0) { abierto = false; break; }		// Solo reintenta 3 veces
		for(double d = 300; d > 0; d--)
			;
	}
	inVoucher = abierto = false;
	return 0;
}


// Se sobreescribe porque el estado se codifica distinto.
int PrinterTM220::chkFiscStat()
{
	char *msg;

	int fisc = (lstat.fiscstat & 0x0C00) / 0x0400;

	switch(fisc)
	{
		case 1: 
			msg = "Memoria Fiscal a punto de llenarse";
			break;

		case 2:
			msg = "Memoria Fiscal Llena";
			break;

		case 3:
			msg = "Memoria Fiscal en mal Estado";
			break;

		default:
			msg = "";
	}

	// Como detectamos que hace falta una zeta??
	FiscalForceZeta = 0;
	if(strlen(msg) != 0)
	{
		Alert("Error Fiscal en la Impresora", msg);
		return 1;
	}
	return 0;
}

//   Chequea los bits de estado del impresor fiscal y muestra los mensajes
//   orientativos que correspondieren.
int PrinterTM220::chkPrnStat()
{
	char *msg;

	if(lstat.prnstat & 0x40)
		msg = "Mecanismo de Impresora Trabado";
	else if(lstat.prnstat & 0x20)
		msg = "La tapa del impresor esta abierta";
	else if((lstat.prnstat & 0x0C) == 0x08)
		msg = "Sin papel en cinta testigo";
	else if((lstat.prnstat & 0x03) == 0x02)
		msg = "Sin papel en cinta testigo";
	else if(lstat.prnstat & 0x80)
		msg = "Impresora Fuera de Linea (OFFLINE)";
	else
		msg = "";

	if(strlen(msg) != 0)
	{
		Alert("Error en Impresora", msg);
		return 1;
	}
	return 0;
}

void PrinterTM220::InstallLogo(char *filename)
{
	SendMsg("Cargando Logo de Ticket", "");
	PrepareCmd(0x0530, 0x0000);
	int width = 200, height = 100;

	AppendCmdBinary(width / 8);
	AppendCmdBinary(height / 8);
	txbuff[cmdLen++] = SEP;
	txbuff[cmdLen++] = 1;
//	AppendToCmd(1);

	xchgPkt2(txbuff, cmdLen);

	bool alter = false;
	cmdLen = -1;
	for(int i=width/8*height; i>0;i--)
	{
		if(cmdLen > 512)
		{
			xchgPkt2(txbuff, cmdLen);
			cmdLen = -1;
		}
		if(cmdLen < 0)
		{
			PrepareCmd(0x0531, 0x0000);
			txbuff[cmdLen++] = SEP;
		}

		int byte = alter? 0xff: 0x00;
		alter = !alter;
		txbuff[cmdLen++] = (char)byte;
	}

	if(cmdLen > 0) xchgPkt2(txbuff, cmdLen);
	PrepareCmd(0x0532, 0x00);
	xchgPkt2(txbuff, cmdLen);
	ClearMsg();
}

//void PrinterTM220::VoucherTJOnline(
//	char *marca,
//	char *tarjeta,
//	char *nombre,
//	char *vencimiento,
//	int  lote,
//	int  cupon,
//	int  autorizacion, 
//	double monto,
//	int cuotas)
//{
//}

// Se sobreescribe porque el estado se codifica distinto.
int PrinterTM220::docFiscStat()
{
	int valreturn = 0;
	if ((lstat.fiscstat & 0x0100) || (lstat.fiscstat & 0x0200))			
		valreturn = 1;	
	return valreturn;
}


int PrinterTM220::chkRta(int rta)
{
	int aux = 0;
	switch (rta)
	{
	case 0x0102:
		sprintf(tmsg, "ERROR - Comando invalido para el documento actual: %04X", rta);
		aux = rta;
		break;
	case 0x0301:
		sprintf(tmsg, "ERROR - de Hardware: %04X", rta);
		aux = rta;
		break;
	case 0x0302:
		sprintf(tmsg, "ERROR - Impresora fuera de linea: %04X", rta);
		aux = rta;
		break;
	case 0x0303:
		sprintf(tmsg, "ERROR - Error de impresion: %04X", rta);
		aux = rta;
		break;
	case 0x0304:
		sprintf(tmsg, "ERROR - Problemas de papel: %04X", rta);
		break;
	case 0x0305:
		sprintf(tmsg, "ERROR - Poco papel disponible: %04X", rta);
		break;
	case 0x0306:
		sprintf(tmsg, "ERROR - en carga o expulsion de papel: %04X", rta);
		break;
	}

	if (aux)
		WriteBootLog(tmsg);

	return aux;
}

