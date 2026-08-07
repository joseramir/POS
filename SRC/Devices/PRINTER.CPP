/*
 *   Printer.c    (v  2.005)
 *   Manejo del impresor fiscal atachado al sistema. Implementación del device driver
 *   como una clase.
 *   Implementa tambien el tipo de impresor 'nada'.
*/

#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif



PrinterFiscal_ *pf;
char GlobalComprob[30];			// Codigo del comprobante que se esta emitiendo.

// Posibles caracteres de control usados en el protocolo.
#define STX     '\x02'
#define ETX     '\x03'
#define ACK     '\x06'
#define NAK     '\x15'
#define DC2     '\x12'
#define DC4     '\x14'
#define ESC     '\x1b'
#define STATPRN	'\Xa1'

//   Variable que indica si la configuracion decide que se debe verificar
//   la disponibilidad del impresor cada tanto.
int chkXon;


//extern "C" {
//	PrinterDummy P_DUMMY;
//};

//   Variables para pasar datos a las subrutinas de cada impresor.
char *f_adicdes;
int DBGPrintProc;
int (*prnfunc[40])(void);
int globalDelayPrint;			// Se imprime solo al final.
int GlobalVoucherFlag;
int PrintTextAllowed;           // Algunos impresores fiscales limitan el
                                // uso de la funcion 'PrintText'
int FiscalForceZeta;			// Cuando el printer OBLIGA a ZETA.
int prnmudo, PrinterCap;        // Capacidades disponibles.
int SyncPrinterTot;             // Opcion 'sincroniza'

//***********************************************************************
//   FUNCIONES NO FISCALES
int PrinterFiscal_::OpenVoucher()
{
	GlobalVoucherFlag = 1;
	return 0;
}

int PrinterFiscal_::OpenVoucher2()
{
	this->OpenVoucher();
	return 0;
}

// Voucher rapido
void PrinterFiscal_::FastVoucher(bool pHeader, char *l1, char *l2, char *l3)
{
}

// Configura el impresor
void PrinterFiscal_::Configure(char *confParam)
{
}

void PrinterFiscal_::AjustarRedondeo(double redondeo)
{
}

int PrinterFiscal_::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
	return 1;
}

int PrinterFiscal_::PrintRecargo(char *msg, double monto)
{
	return 0;
}


//    Imprime un texto en un ticket no fiscal.
int PrinterFiscal_::PrintVoucher(char *des)
{
	if(!GlobalVoucherFlag) return 1;
	return 0;
}

int PrinterFiscal_::CloseVoucher()
{
	GlobalVoucherFlag = 0;
	return 0;
}

int PrinterFiscal_::CloseVoucher2()
{
	CloseVoucher();
	return 0;
}

#define HEXDIG(x) (((x) > 9)? ((x) - 10 + 'A') : (x) + '0')

char tempPkt[4000];

// Rutina que formatea los caracteres no imprimibles de un paquete, para visualizarlo correctamente en un log.
char *PrinterFiscal_::FormatPkt(char *pkt, int len)
{
	char *res = tempPkt;
	int avail = sizeof(tempPkt);
	int mcnt = 0;
	while(len--)
	{
		if(*pkt == SEP)
			*res++ = '|';
		else if(*pkt >= ' ' && *pkt <= 'z')// && mcnt > 1)
			*res++ = *pkt;
		else
		{
			int dig = *((unsigned char *)pkt);
			*res++ = '{';
			*res++ = (char)HEXDIG(dig / 16);
			dig &= 0x0f;
			*res++ = (char)HEXDIG(dig);
			*res++ = '}';
			avail -= 3;
		}
		mcnt++;

		pkt++;
		avail--;
		if(avail <= 0)
			AbnormalEnd("Buffer insuficiente en 'Printer.cpp::FormatPkt(char *pkt)'");
	}
	*res = 0;
	return tempPkt;
}

//***********************************************************************
//   FUNCIONES FISCALES.
int PrinterFiscal_::PrintItem(char *des, double mult, double monto, int tasa,
                              char qual, int bulto, int variac, char *ades, 
							  double unitario, bool forzarCant, int cajaBulto)
{	
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	STRCPY(GlobalComprob, "");
	return 0;
}

int PrinterFiscal_::PrintSubtot(char *msg, int pFlag)
{
	if(FactuFlag > 1) return 1;
	printerTot = -1.0;
	if(globalDelayPrint) return 0;
	return 0;
}

int PrinterFiscal_::PrintMPago(char *msg, double monto)
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	PrinterItemPago *pi = new PrinterItemPago(msg, mpagAdicDesc, monto);
	STRCPY(mpagAdicDesc, "");
	stackPagos->Add(pi);
	return 0;
}

void PrinterFiscal_::MPagoAdicDesc(char *msg)
{
	STRCPY(mpagAdicDesc, msg);
}

int PrinterFiscal_::PrintClose()
{
	return 0;
}

int PrinterFiscal_::PrintDescto(char *msg, double monto)
{
	return 0;
}

int PrinterFiscal_::PrintText(char *msg)
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	return 0;
}

int PrinterFiscal_::PrintCancel(void)
{
	if(FactuFlag > 1) return 1;
	return 0;
}

int PrinterFiscal_::PrintZeta(void)
{
	return 0;
}

int PrinterFiscal_::FiscalInit(char *cuit, char *rs, char *regnro, char *fec, char *ptovta,
							   char *initAct, char *ibrut, char *respon)
{
	return 0;
}

int PrinterFiscal_::SetPie(char *msg)
{
	return 0;
}

int PrinterFiscal_::SetPie(char *msg, int linea)
{
	return 0;
}

int PrinterFiscal_::SetPie2(char *msg)
{
	return 0;
}

int PrinterFiscal_::SetHeader(char *msg)
{
	return 0;
}

/***********************************************************************
 *   FUNCIONES DE COMPATIBILIDAD.
 *   Las utilizaban los impresores que no eran fiscales. Quedaron...   (y se usan) */
 
//   Chequea si el impresor esta listo para operar
int PrinterFiscal_::ChkPrnXonXoff()
{
	int x;
	if(globalDelayPrint) return 0;

	x = GetModemStat(COM_PRN);      // Toma el estado del port
	if(!chkXon) x |= MOD_CTS;
	if(x & MOD_CTS)
		xon930 = 1;
	 else
		xon930 = 0;
	return 0;

	return 0;
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispo-
//   sitivo presente en el impresor TM930 para activarla.
int PrinterFiscal_::OpenDrwr()
{
	if(globalDelayPrint) return 0;
	return 0;
}

//   Imprime una linea en la Cinta Testigo.
int PrinterFiscal_::PrintJournal(char *s)
{
	if(globalDelayPrint) return 0;
	return 0;
}

//   Imprime una linea en el ticket y en la cinta testigo.
int PrinterFiscal_::PrintAmbos(char *s)
{
	if(globalDelayPrint) return 0;
	return 0;
}

//   Imprime una linea en el ticket solamente.
int PrinterFiscal_::PrintTick(char *s)
{
	if(globalDelayPrint) return 0;
	return 0;
}

//   Corta el ticket y avanza al siguiente renglon.
int PrinterFiscal_::Cutter()
{
	return 0;
}

//   Descarga en el impresor los encabezamientos y pie de ticket.
int PrinterFiscal_::PrinterUpLoad()
{
	xon930 = 1;
	return 0;
}

//  Constructor del impresor virtual. Solamente, activa la utilizacion de
//  texto fiscal.
PrinterFiscal_::PrinterFiscal_()
{
	PrintTextAllowed = 1;
	ignoraError4 = 0;
	seqHigh = false;
    if(!SyncPrinterTot) PrinterCap &= ~PRN_SYNC;	// Desactiva la capacidad de sincronizar.
	STRCPY(ptovta, "");
	STRCPY(mpagAdicDesc, "");
	stackPagos = new ::ArrayList(true, "PrinterFiscal_Ctor");
	solofact = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
//  La siguiente función es llamada por el inicializador de kernel para activar
//  la clase que se utilizara para el booteo del POS. Elige en la tabla de
//  dispositivos el que mejor se le parezca y, si no lo encuentra, entonces
//  activa el printer dummy (también, podría abortar de manera fatal. SERIA VALIDO)
int InitPrinter()
{
	//DeviceList_ *d;
	//int i;

	//pf = (PrinterFiscal_ *) &P_DUMMY;				// Con impresor virtual.
	Trim(interfaz);
	/*for(d = DeviceList, i = cDev; i; i--, d++)
		if(d->clase == DV_PRINTER && !_stricmp(d->name, interfaz))
		{
			pf = (PrinterFiscal_ *) d->ClassUsed;
			break;
		}
	*/
	bool dummy = false;
	if (!_stricmp("EpsonTM220F", interfaz))
		pf = (PrinterFiscal_ *) new PrinterTM220();
	/*else if (!_stricmp("HasarPR4", interfaz))
		pf = (PrinterFiscal_ *) new PrinterPR4();
	else if (!_stricmp("HasarPR5", interfaz))
		pf = (PrinterFiscal_ *) new PrinterPR5();*/	
	else if (!_stricmp("HasarPR250", interfaz))
		pf = (PrinterFiscal_ *) new PrinterPR250();
	else if (!_stricmp("PTermica", interfaz))
		pf = (PrinterFiscal_ *) new PrinterTermica();
	else 
	{
		pf = (PrinterFiscal_ *) new PrinterDummy();
		dummy = true;
	}

		//"Epson930",		DV_PRINTER	},	// Implementado en P930.cpp
		//	{   &P_EPSON930,	"Epson950",		DV_PRINTER	},
		//	{	&P_TM300,		"EpsonTM300F",  DV_PRINTER	},	// Impresores fiscales EPSON
		//	{	&P_TM300,		"EpsonTM950F",	DV_PRINTER	},
		//	{   &P_TM220,       "EpsonTM220F",  DV_PRINTER  },
		//	{   &P_PR4,         "HasarPR4",     DV_PRINTER  },  // Impresores fiscales HASAR
		//	{   &P_PR5,         "HasarPR5",     DV_PRINTER  },
		//	{   &P_BEMA,        "BematechMP35", DV_PRINTER  },  // Impresores fiscales BEMATECH
		//	{   &PL23F,			"HasarPL23F",	DV_PRINTER  },  // Impresores fiscales BEMATECH

	//   Loguea un mensaje, si no encontro la definicion del impresor.
	//if(!i)
	if (dummy)
		sprintf(dbgStr, "ERROR: '%s' no esta como DV_PRINTER en 'DeviceList'. Usare 'nada'\n",
			    interfaz);
	else
		sprintf(dbgStr, "Se usara el impresor tipo '%s'.", interfaz);
	WLog(dbgStr, BM_MSGZ);
	STRCPY(GlobalComprob, "TI");
	if(ForzarZeta) return 0;
	return pf->PrinterUpLoad();
}

//   Transmite una string al impresor.
void PrinterFiscal_::txStrP(char *s)
{
	txStr(COM_PRN, s);
}

//   Emite un caracter al impresor. Si no esta en linea saca un aviso.
void PrinterFiscal_::txCharP(char c)
{
	// La transmision se debe suspender por 2 segundos 
	if(!seqHigh)
	{
		while(true)
		{
			if(!rxPoll(COM_PRN)) break;
			int ch = rxChar(COM_PRN);
			if(ch != DC2 && ch != DC4)
			{
				sprintf(mdBuff, "BAD Char waiting ASSERTION in  txCharP: %02X", ch & 0xff);
				WriteBootLog(mdBuff);
				break;
			}

			// Si recibimos DC2/DC4, el siguiente no vendrá antes de 1/2 segundo. Debemos
			// esperar un segundo antes de chequear si recibimos otro DC2/DC4, o si el canal
			// ya está liberado para proceder.
			rxTimer = time(NULL) + 2;
			while(!rxPoll(COM_PRN))
				if(rxTimer <= time(NULL))
					break;
		}
	}
	txChar(COM_PRN, c);
}

// Recibe un caracter desde el impresor. Valida RTS-CTS, XON-XOFF, etc 
char PrinterFiscal_::rxCharP()
{
	rxTimer = time(NULL) + 5; // Ajustar si sale el mensaje de error muy seguido
	ErrorDuringRx = 0;
	while(!rxPoll(COM_PRN))
	{
		if(rxTimer <= time(NULL))
		{
			ErrorDuringRx = 1;
            #ifdef POSDEBUG
                sprintf(dbgStr, "SERIALPOLL: Timeout en COM%d:", COM_PRN+1);
                DBG(DBGPrintProc, 10, dbgStr);
            #endif

            return 0;
		}
	}
	cue++;
    return (char) rxChar(COM_PRN);
}

/*   Funciones de depuracion  */
int hDump = -1;
void PrinterFiscal_::Debug()
{
	remove("dump.bin");
	close(creat("dump.bin", S_IREAD | S_IWRITE));
	hDump = open("dump.bin", O_RDWR | O_BINARY);
	if(hDump >= 0)
	{
		this->debugMode = 1;
		sprintf(txbuff, "\x3B%c%04d%c%04d%c%c", SEP, 1, SEP, 2, SEP, 'T');
		xchgPkt(txbuff);
		close(hDump);
		this->debugMode = 0;
	}
}

void DumpOutput(char ch, int source)
{
	char data[2];
	if(hDump >= 0)
	{
		data[0] = ch;
		write(hDump, data, 1);
	}
}

/*   Variables de este modulo   */
#define SIZE300BUFF 15000 //15000

unsigned char seq = ' ';
int rxCount, cue;
char rxBuff[SIZE300BUFF];
char txbuff[SIZE300BUFF];
char txbuff2[SIZE300BUFF];
char mdBuff[SIZE300BUFF];
time_t rxTimer;
int ErrorDuringRx;

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
char *PrinterFiscal_::ChkDes(char *s, char *dst)
{
	if(dst == NULL && strlen(s) == 0) return s;
	if(dst == NULL) dst = s;
	char *ret = dst;
	int maxLen = 41;
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

	if(strlen(ret) > 40) *(ret + 40) = 0;
	*dst = 0;
	return ret;
}

//   Envia un paquete a la registradora   
void PrinterFiscal_::txPkt(char *pkt, int len)
{
    long chksum, pktLen;
    char chk[6], *p, ch;

	// Valida la secuencia
	if(seqHigh)
	{
		if(seq >= 0xff || seq < 0x81) 
			seq = 0x81;
	}
	else
	{
		if(seq > 0x7f) 
			seq = ' ';
	}

    p = pktbuff;
	chksum =  (unsigned)(*p++ = STX);	pktLen = 1;
	*p++ = seq;							pktLen++;
	chksum += seq;
	while(len--)
	{
		if(seqHigh &&
			(*pkt == 0x2 || *pkt == 0x3 || *pkt == 0x1a || *pkt == 0x1b || *pkt == 0x1d || *pkt == 0x1e || *pkt == 0x1f))
		{
			*p++ = ESC;					pktLen++;
			chksum += ESC;
		}

		*p++ = *pkt;					pktLen++;
        chksum += ((unsigned char)*pkt++);
	}
	chksum += (unsigned)(*p++ = ETX);	pktLen++;
	sprintf(chk, "%04X", chksum);
	STRCPY(p, chk); p += 4;				pktLen += 4;

	// Envia el paquete. Espera un ACK o NAK de respuesta.
	while(true)
	{
		int i = pktLen;
		for(p = pktbuff; i; p++, i--)
			txCharP(*p);
		if(!seqHigh) break;					// No todos los impresores confirman el paquete con ACK

		ch = rxCharP();
		if(!ErrorDuringRx && (ch == ACK)) break;
		if(ch != NAK)
		{
			sprintf(mdBuff, "Bad ACK/NAK despues de TkPkt: %02X", ch & 0xff);
			WriteBootLog(mdBuff);
		}
	}
}

//   Recibe un paquete    
int PrinterFiscal_::rxPkt()
{
    long chksum, rcvchks;
    char *rx, ch;
	int i;
    unsigned char rxseq;

	//   Espera el STX que indica el comienzo de un paquete.
	while(true)
	{
		rxCount = 0;
		while(true)
		{
			ch = rxCharP();
			if(ErrorDuringRx) return 4;
			
			if(ch == STX) break;
			if(!seqHigh && (ch == ACK)) continue;
			if(!seqHigh && (ch == NAK)) return 1;
			if(ch != DC2 && ch != DC4)
			{
				sprintf(mdBuff, "Bad CHAR waiting STX in rxPkt: %02X", ch & 0xff);
				WriteBootLog(mdBuff);
			}
		}

		//   Continua recibiendo hasta que recibe el ETX
		rx = rxBuff;
		int avail = SIZE300BUFF;
		chksum = STX;
		rxseq = rxCharP();				// Largo del paquete.
		if(ErrorDuringRx) return 4;
		chksum += rxseq;
		bool escaped = false;
		while(true)
		{
			escaped = false;
			ch = rxCharP();
			if(ErrorDuringRx) return 4;
			chksum += (unsigned char)ch;
			if(seqHigh && (ch == ESC))
			{
				ch = rxCharP();
				if(ErrorDuringRx) return 4;
				chksum += (unsigned char)ch;
				escaped = true;
			}

			*rx++ = ch;
			rxCount++;
			if(--avail <= 0)
				AbnormalEnd("Insuficiente rxbuff en 'Printer.cpp::rxPkt'");
			if(!escaped && ch == ETX) 
				break;
		}
		rxCount--;
		*(--rx) = 0;

		//   Recibe el CRC y lo chequea.
		for(rcvchks = 0, i=4; i; i--)
		{
			ch = rxCharP();
			if(ErrorDuringRx) return 4;
			rcvchks *= 16;
			rcvchks +=  (ch <= '9')?(ch - '0'):
				   ((ch <= 'F')?(ch - 'A' + 10) : (ch - 'a' + 10));
		}
		if(rcvchks != chksum)
		{
			txCharP(NAK);
			return rxPkt();
		}
		txCharP(ACK);

		//   Si no es la secuencia correcta, recibe otro.
		if(rxseq == seq) break;
	}
	return 0;
}

void PrinterFiscal_::CheckStatus()
{
	FiscalForceZeta = 0;
}

char *PrinterFiscal_::PuntoDeVenta()
{
	return ptovta;
}

char *ptrFields[100];
int cFields;

int PrinterFiscal_::xchgPkt2(char *pkt, int len)
{
	int flag;

	//   Intercambia el paquete.
	FlushComm(COM_PRN);
	do {
		//   Emite el paquete al archivo de depuracion.
		#ifdef POSDEBUG
			if(len < 256)
			{
				sprintf(dbgStr, "PC==>PRN: '%s'", FormatPkt(pkt, len));
				DBG(DBGPrintProc, 10, dbgStr);
			}
		#endif

		//   Continua normalmente.
		txPkt(pkt, len);
		do {
			flag = rxPkt();
			#ifdef POSDEBUG
			    if(rxCount < 64)
				{
					sprintf(dbgStr, "PRN==>PC: (%d) '%s'", flag, FormatPkt(rxBuff, rxCount));
					DBG(DBGPrintProc, 10, dbgStr);
				}
			#endif
		} while(flag == 2);
		if(flag == 3)
			AbnormalEnd("Error en el protocolo con el impresor.");
		if(flag == 4)
		{
			if(ignoraError4==0)
			{
				#ifdef INPOS
				Alert("Hay problemas con el PAPEL en el impresor.",
					  "O puede pasar que el impresor este apagado.");
				#else
				printf("No puedo comunicar con el TM300. Revise!!!\n");
				#endif
			}
			else
				return 0;
		}
	} while(flag != 0);

	// Nueva secuencia. Se ajusta ANTES de usarla.
	++seq;

	/*   Descomprime el resultado del intercambio   */
	char *pc = rxBuff;
	lstat.prnstat = *((unsigned short *)pc);  pc += 3;
	lstat.fiscstat = *((unsigned short *)pc);  pc += 3;
	rxCount -= 6;

	// Saltea reservado 1
	while(rxCount > 0)
	{
		if(*pc++ == SEP) break;
		rxCount--;
	}

	int result = 0;
	while(rxCount > 0 && *pc != SEP)
	{
		result = result * 256 + *((unsigned char *)pc++);
		rxCount--;
	}
	pc++;

	// Saltea reservado 2
	while(rxCount > 0)
	{
		if(*pc++ == SEP) break;
		rxCount--;
	}

	cFields = 0;
	char *oldPc = pc;
	while(rxCount > 0)
	{
		if(*pc == SEP)
		{
			*pc++ = 0;
			ptrFields[cFields++] = oldPc;
			oldPc = pc;
		}
		else
			pc++;
		rxCount--;
	}

	ptrFields[cFields++] = oldPc;
	ptrFields[cFields] = NULL;

	lastResult = result;
	if((result & 0x0800) != 0 || (result & 0x0900) != 0)
		return result;

	if(result != 0 && (result != 0x0203))//quitado && (result != 0x0102))
	{
		// Los errores que empiezan con "00" son errores de la placa fiscal...
		if((result / 256) == 0)
		{
			sprintf(tmsg, "ERROR - PRINTER CON ERROR: %04X", result);
			Alert(
				"La impresora fiscal tiene problemas. Por favor, apaguela y",
				"vuelvala a encender. Si no, llame al Serv. Tecnico");
		}
		else
			sprintf(tmsg, "ERROR - Result Inesperado: %04X", result);

		// Escribe el error recibido al POS. Mostramos el paquete enviado por si POSDEBUG no se activo.
		WriteBootLog(tmsg);
		sprintf(tmsg, "PKT SENT: >>%s<<", FormatPkt(pkt, len));
		WriteBootLog(tmsg);

		// Indica que debe rebootear al POS y vuelve indicando que NO HUBO error
		// Aparentemente, estos errores pasan por corrupcion de los buffers internos
		// dado que el log muestra que estan ocurriendo cosas raras...
		errorCode = 1; 
//		rebootear = 1;
		return 1;
	}
	return 1;
}

//  El paquete que se recibe como parametro lo envia al impresor. Posteriormente, queda
//  en espera de la respuesta y, cuando la recibe, chequea los resultados.
int PrinterFiscal_::xchgPkt(char *pkt)
{
	int flag, len;

	//   Intercambia el paquete.
	len = strlen(pkt);
	FlushComm(COM_PRN);
	do
	{
		//   Emite el paquete al archivo de depuracion.
		#ifdef POSDEBUG
			sprintf(dbgStr, "PC==>PRN: '%s'", FormatPkt(pkt, len));
			DBG(DBGPrintProc, 10, dbgStr);
		#endif

		//   Continua normalmente.
		txPkt(pkt, len);
		do {
			flag = rxPkt();
			#ifdef POSDEBUG
				sprintf(dbgStr, "PRN==>PC: (%d) '%s'", flag, FormatPkt(rxBuff, rxCount));
				DBG(DBGPrintProc, 10, dbgStr);
			#endif
		} while(flag == 2);
		if(flag == 3)
			AbnormalEnd("Error en el protocolo con el impresor.");
		if(flag == 4)
		{
			if(ignoraError4==0)
			{
				#ifdef INPOS
				Alert("Hay problemas con el PAPEL en el impresor.",
					  "O puede pasar que el impresor este apagado.");
				#else
				printf("No puedo comunicar con el TM300. Revise!!!\n");
				#endif
			}
			else
				return 0;
		}
	} while(flag != 0);

	++seq;

	/*   Descomprime el resultado del intercambio   */
	char *pc = rxBuff + 1;
	lstat.prnstat  = gethexa(&pc);
	lstat.fiscstat = gethexa(&pc);
	return 1;
}

//char fld[60];
char fld[30000];
//   Extrae un campo (detectado por la presencia del caracter separador SEP).
char *PrinterFiscal_::getfld(char **pc)    // Lee una string.
{
    char *r;
	int ki = 0;

	if(**pc == SEP) 
		(*pc)++; 
	if(*pc >= rxBuff + SIZE300BUFF) 
		return "";
	for(r = fld; **pc != SEP; )
	{
		if(*pc >= (rxBuff + SIZE300BUFF) || r >= (fld + 30000)) //(fld + 60))
			return "";
		else if (**pc == 0)
			break;
		 else
		 {
			*r++ = **pc;
			(*pc)++;
			ki++;
		 }
	}
	*r = 0;
	return fld;
}

//   Extrae un campo y lo procesa como una constante hexadecimal.
unsigned long PrinterFiscal_::gethexa(char **pc)    
{
	unsigned long res;
    char *p;

	for(p = getfld(pc), res = 0l; isxdigit(*p); p++)
	{
		res *= 16;
		res += (*p <= '9')? (*p - '0') : (
		       (*p <= 'F')? (*p - 'A' + 10): (*p - 'a' + 10));
	}
	return res;
}

//   Extrae un campo y lo procesa como una constante decimal.
unsigned long PrinterFiscal_::getdec(char **pc)     // Lee un nro expresado decimal
{
	unsigned long res;
    char *p;

	for(p = getfld(pc), res = 0l; isdigit(*p); p++)
	{
		res *= 10;
		res += *p - '0';
	}
	return res;
}

//   Chequea los bits de estado del controlador fiscal y muestra los mensajes
//   adecuados, de corresponder.
int PrinterFiscal_::chkFiscStat()
{
	char *msg;

	int modo = lstat.fiscstat / 0x4000;
	int fisc = (lstat.fiscstat & 0x0C00) / 0x0400;

	if((lstat.fiscstat & 0x81) || (lstat.fiscstat & 0x80))
		msg = "Memoria Fiscal Llena";
	else if(lstat.fiscstat & 0x1)
		msg = "Error al Verificar la Memoria Fiscal";
	else if(lstat.fiscstat & 0x2)
		msg = "Error al Verificar la Memoria de Trabajo (CMOS)";
	else if(lstat.fiscstat & 0x4)
		msg = "Haga reemplazar la bateria del impresor fiscal";
	else if(lstat.fiscstat & 0x40)
		msg = "Monto Total excede lo permitido por el impresor";
	else if(lstat.fiscstat & 0x100)
		msg = "Memoria fiscal CASI llena. Avise al Tecnico";
	else if((lstat.fiscstat & 0x800) && (lstat.fiscstat & 0x2000))
		msg = "Cierre el Ticket - El impresor no admite mas renglones";
	else if((lstat.fiscstat & 0x800) && !(lstat.fiscstat & 0x2000))
	{
		msg = "Emitiré una Zeta. El impresor asi lo requiere...";
		FiscalForceZeta = 1;
	}
	else return 0;
    #ifdef INPOS
	Alert("Error Fiscal en la Impresora", msg);
    #else
    printf("Error Fiscal en la Impresora!!!\n");
    #endif
	return 1;
}

//   Chequea los bits de estado del impresor fiscal y muestra los mensajes
//   orientativos que correspondieren.
int PrinterFiscal_::chkPrnStat()
{
	char *msg;

	if(lstat.prnstat & 0x4)
		msg = "Error / Falla de Impresora";
	else if(lstat.prnstat & 8)
		msg = "Impresora Fuera de Linea (OFFLINE)";
	else if(lstat.prnstat & 10)
		msg = "Falta Papel en la Cinta Testigo";
	else if(lstat.prnstat & 20)
		msg = "Falta Papel en el Ticket";
	else return 0;
    #ifdef INPOS
	Alert("Error en la Impresora", msg);
    #else
    printf("Error '%s' en el TM300 !!!", msg);
    #endif
	return 1;
}

//   Engloba todas las funciones de chequeo del estado.
int PrinterFiscal_::chk()
{
	//   Envia el comando de requerimiento de estado.
	xchgPkt("*");
	char *pc = rxBuff + 1;

	//   Descompila los distintos campos del estado.
	lstat.prnstat = gethexa(&pc);
	lstat.fiscstat = gethexa(&pc);
	lstat.lastcomp = getdec(&pc);
	lstat.fecop = getdec(&pc);
	lstat.horaop = getdec(&pc);
	//lstat.lastcie = getdec(&pc);
	//lstat.archi1 = getdec(&pc);
	//lstat.archi2 = getdec(&pc);
	//STRCPY(lstat.archi3, getfld(&pc));
	//STRCPY(lstat.archi4, getfld(&pc));

	//   Analiza por posibles situaciones de error.
	if(chkPrnStat()) return 1;
	if(chkFiscStat()) return 2;
	return 0;
}

//   Devuelve el nro de ticket actual.
//   ATENTI - Ignora el parametro retornando siempre el ultimo ticket B - Si el impresor
//       soporta tickets A, entonces debe sobreescribirse en el driver del impresor, porque obtener
//       el numero de ticket A no es una operacion estandard.
int PrinterFiscal_::GetTickNro(int cual)
{
	return 9999;
}

// Instala logo grafico.
void PrinterFiscal_::InstallLogo(char *filename)
{
}

// Voucher de tarjeta de credito online.
void PrinterFiscal_::VoucherTJOnline(
		char *marca,
		char *tarjeta,
		char *nombre,
		int  vencimiento,
		int  lote,
		int  cupon,
		int  autorizacion, 
		double monto,
		int cuotas,
		char *numcomercio,
		int numterminal,
		int tickfiscal,
		bool credito)
{
}

int PrinterFiscal_::GeneraReporteAfip(int fdesde, int fhasta)
{
	return 1;
}

void PrinterFiscal_::ObtieneRangosReportes(int pdesde)
{
	frepdesde = frephasta = frepsig = 0;

	int anio = pdesde / 10000;
	int mes = pdesde % 10000;
	mes = mes / 100;
	int dia = pdesde % 100;

	int sigDia, sigMes, sigAnio;
	int hasDia, hasMes, hasAnio;

	long sysfecha = date();
	int auxAnio = sysfecha / 10000;

	if (dia <= 31 && mes <= 12 && (anio + 2000) <= auxAnio )
	{
		sigAnio = hasAnio = anio;
		sigMes = hasMes = mes;
		sigDia = hasDia = dia;  
		if (dia >= 1 && dia <= 7)
		{
			dia = 1;
			hasDia = 7;
			sigDia = 8;
		}
		else if (dia >= 8 && dia <= 14)
		{
			dia = 8;
			hasDia = 14;
			sigDia = 15;
		}
		else if (dia >= 15 && dia <= 21)
		{
			dia = 15;
			hasDia = 21;
			sigDia = 22;
		}
		else if (dia >= 22)
		{
			dia = 22;
			if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12)
				hasDia = 31;
			else if (mes == 4 || mes == 6 || mes == 9 || mes == 11 || mes == 8 || mes == 10 || mes == 12)
				hasDia = 30;
			else if (mes == 2)
			{
				if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0))
					hasDia = 29;
				else
					hasDia = 28;
			}
			sigDia = 1;
			if (mes  >= 12)
			{
				sigMes = 1;
				sigAnio = anio + 1;
			}
			else                     
				sigMes = mes + 1;                    
		}

		frepdesde = anio * 10000 + mes * 100 + dia;
		frephasta = hasAnio * 10000 + hasMes * 100 + hasDia;
		frepsig = sigAnio * 10000 + sigMes * 100 + sigDia;
	}
	else
		frepdesde = frephasta = frepsig = 0;

}

void PrinterFiscal_::ObtieneRangosReportesMes(int pdesde)
{
	frepdesde = frephasta = frepsig = 0;

	int anio = pdesde / 10000;
	int mes = pdesde % 10000;
	mes = mes / 100;
	int dia = pdesde % 100;

	int sigDia, sigMes, sigAnio;
	int hasDia, hasMes, hasAnio;

	long sysfecha = date();
	int auxAnio = sysfecha / 10000;

	if (dia <= 31 && mes <= 12 && (anio + 2000) <= auxAnio )
	{
		sigAnio = hasAnio = anio;
		sigMes = hasMes = mes;
		sigDia = hasDia = dia;  
		
		
		dia = 1;
		if (mes == 1 || mes == 3 || mes == 5 || mes == 7 || mes == 8 || mes == 10 || mes == 12)
			hasDia = 31;
		else if (mes == 4 || mes == 6 || mes == 9 || mes == 11 || mes == 8 || mes == 10 || mes == 12)
			hasDia = 30;
		else if (mes == 2)
		{
			if ((anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0))
				hasDia = 29;
			else
				hasDia = 28;
		}
		sigDia = 1;
		if (mes  >= 12)
		{
			sigMes = 1;
			sigAnio = anio + 1;
		}
		else                     
			sigMes = mes + 1;                    
		

		frepdesde = anio * 10000 + mes * 100 + dia;
		frephasta = hasAnio * 10000 + hasMes * 100 + hasDia;
		frepsig = sigAnio * 10000 + sigMes * 100 + sigDia;
	}
	else
		frepdesde = frephasta = frepsig = 0;

}

int PrinterFiscal_::GeneraReporteAfipNew(int fdesde, int fhasta)
{
	return 1;
}

int PrinterFiscal_::CopiaDocumento(int tipoDocumento, int nroDocumento)
{
	return 1;
}

void PrinterFiscal_::ObtenerAbierto(int &numticket, double &monto)
{
	numticket = 0;
	monto = 0;
}

int PrinterFiscal_::docFiscStat()
{
	return 0;
}










//   Envia un paquete a la registradora   
void PrinterFiscal_::txPkt2(char *pkt, int len)
{
	long chksum, pktLen;
	char chk[6], *p, ch;

	// Valida la secuencia
	if(seqHigh)
	{
		if(seq >= 0xff || seq < 0x81) 
			seq = 0x81;
	}
	else
	{
		if(seq > 0x7f) 
			seq = ' ';
	}

	p = pktbuff;
	chksum =  (unsigned)(*p++ = STX);	pktLen = 1;
	*p++ = seq;							pktLen++;
	chksum += seq;
	while(len--)
	{
		if(seqHigh &&
			(*pkt == 0x2 || *pkt == 0x3 || *pkt == 0x1a || *pkt == 0x1b || *pkt == 0x1d || *pkt == 0x1e || *pkt == 0x1f))
		{
			*p++ = ESC;					pktLen++;
			chksum += ESC;
		}

		*p++ = *pkt;					pktLen++;
		chksum += ((unsigned char)*pkt++);
	}
	chksum += (unsigned)(*p++ = ETX);	pktLen++;
	sprintf(chk, "%04X", chksum);
	STRCPY(p, chk); p += 4;				pktLen += 4;

	// Envia el paquete. Espera un ACK o NAK de respuesta.
	while(true)
	{
		int i = pktLen;
		for(p = pktbuff; i; p++, i--)
			txCharP(*p);
		if(!seqHigh) break;					// No todos los impresores confirman el paquete con ACK

		ch = rxCharP();
		if(!ErrorDuringRx && (ch == ACK)) break;
		if(ch != NAK)
		{
			sprintf(mdBuff, "Bad ACK/NAK despues de TkPkt: %02X", ch & 0xff);
			WriteBootLog(mdBuff);
		}
	}
}

//   Recibe un paquete    
int PrinterFiscal_::rxPkt2()
{
	long chksum, rcvchks;
	char *rx, ch;
	int i;
	unsigned char rxseq;

	//   Espera el STX que indica el comienzo de un paquete.
	while(true)
	{
		rxCount = 0;
		while(true)
		{
			ch = rxCharP();
			if(ErrorDuringRx) return 4;

			if(ch == STX) break;
			if(!seqHigh && (ch == ACK)) continue;
			if(!seqHigh && (ch == NAK)) return 1;
			/*if(ch != DC2 && ch != DC4)
			{
				sprintf(mdBuff, "Bad CHAR waiting STX in rxPkt: %02X", ch & 0xff);
				WriteBootLog(mdBuff);
			}*/
			//if (ch == DC2)
			//{
			//	sprintf(mdBuff, "Recibio DC2 en rxPkt2: %02X", ch & 0xff);
			//	WriteBootLog(mdBuff);
			//	for(int k = 0; k < 400; k++)//espera por 400 miliseg.
			//		;
			//}
			if (ch == DC4)
			{
				sprintf(mdBuff, "Recibio DC4 en rxPkt2: %02X", ch & 0xff);
				WriteBootLog(mdBuff);
			}
			if (ch == STATPRN)
			{
				sprintf(mdBuff, "Recibio STATPRN en rxPkt2: %02X", ch & 0xff);
				WriteBootLog(mdBuff);
			}
		}

		//   Continua recibiendo hasta que recibe el ETX
		rx = rxBuff;
		int avail = SIZE300BUFF;
		chksum = STX;
		rxseq = rxCharP();				// Largo del paquete.
		if(ErrorDuringRx) return 4;
		chksum += rxseq;
		bool escaped = false;
		while(true)
		{
			escaped = false;
			ch = rxCharP();
			if(ErrorDuringRx) return 4;
			chksum += (unsigned char)ch;
			if(seqHigh && (ch == ESC))
			{
				ch = rxCharP();
				if(ErrorDuringRx) return 4;
				chksum += (unsigned char)ch;
				escaped = true;
			}

			*rx++ = ch;
			rxCount++;
			if(--avail <= 0)
				AbnormalEnd("Insuficiente rxbuff en 'Printer.cpp::rxPkt'");
			if(!escaped && ch == ETX) 
				break;
		}
		rxCount--;
		*(--rx) = 0;

		//   Recibe el CRC y lo chequea.
		for(rcvchks = 0, i=4; i; i--)
		{
			ch = rxCharP();
			if(ErrorDuringRx) return 4;
			rcvchks *= 16;
			rcvchks +=  (ch <= '9')?(ch - '0'):
				((ch <= 'F')?(ch - 'A' + 10) : (ch - 'a' + 10));
		}
		if(rcvchks != chksum)
		{
			txCharP(NAK);
			return rxPkt();
		}
		txCharP(ACK);

		//   Si no es la secuencia correcta, recibe otro.
		if(rxseq == seq) break;
	}
	return 0;
}


//  El paquete que se recibe como parametro lo envia al impresor. Posteriormente, queda
//  en espera de la respuesta y, cuando la recibe, chequea los resultados.
int PrinterFiscal_::xchgPkt3(char *pkt)
{
	int flag, len;

	//   Intercambia el paquete.
	len = strlen(pkt);
	FlushComm(COM_PRN);
	do
	{
		//   Emite el paquete al archivo de depuracion.
#ifdef POSDEBUG
		sprintf(dbgStr, "PC==>PRN: '%s'", FormatPkt(pkt, len));
		DBG(DBGPrintProc, 10, dbgStr);
#endif

		//   Continua normalmente.
		txPkt2(pkt, len);
		do {
			flag = rxPkt2();
#ifdef POSDEBUG
			sprintf(dbgStr, "PRN==>PC: (%d) '%s'", flag, FormatPkt(rxBuff, rxCount));
			DBG(DBGPrintProc, 10, dbgStr);
#endif
		} while(flag == 2);
		if(flag == 3)
			AbnormalEnd("Error en el protocolo con el impresor.");
		if(flag == 4)
		{
			if(ignoraError4==0)
			{
#ifdef INPOS
				Alert("Hay problemas con el PAPEL en el impresor.",
					"O puede pasar que el impresor este apagado.");
#else
				printf("No puedo comunicar con el TM300. Revise!!!\n");
#endif
			}
			else
				return 0;
		}
	} while(flag != 0);

	++seq;

	/*   Descomprime el resultado del intercambio   */
	char *pc = rxBuff + 1;
	lstat.prnstat  = gethexa(&pc);
	lstat.fiscstat = gethexa(&pc);
	return 1;
}
