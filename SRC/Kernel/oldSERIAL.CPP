/*
 *   Serial.c
 *   Manejador de puertos com:
 *
 *   Para la version MSDOS, este modulo se aparea con el programa externo 'serdrv', que maneja el puerto serial.
 *   Ese debe ser un modulo externo puesto que utiliza interrupciones de hardware que solo son posibles en
 *   el modo REAL del PENTIUM. La interfaz con SERDRV se efectua a traves de la INT 17 (para el Port paralelo).
 *
 *   Para los programas compilados en windows, se utilizan directamente las funciones de port serial de windows,
 *   incluyendo la posibilidad de configurar el puerto a traves del mismo POSXP.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Datos comunes a los dos sistemas.
char *seccN[]=   { "COM1:", "COM2:", "COM3:", "COM4:" };
int  habil [4], preChar[4];
#ifdef POSXP
HANDLE hCom[4];
#endif
int hDebugSerial = -1, debugSerial;

////////////////////////////////////////////////////////////
//  Consulta el port para ver si hay caracteres
//  recibidos. Si los hubiera vuelve con TRUE.
int rxPoll(int handl)
{
#ifndef __VSC__

    union REGS ri, ro;

    if(!habil[handl]) return 1;
    ri.x.ax = 0x1203;
    ri.x.bx = handl;
    int86(0x17, &ri, &ro);
    return ro.x.ax;

#else
	char mBuff[2];
	unsigned long rd;

	if(preChar[handl] != -1) return 1;
	if(ReadFile(hCom[handl], mBuff, 1, &rd, NULL) && rd)
	{
		preChar[handl] = mBuff[0];
		return 1;
	}
	return 0;

#endif
}

#ifdef POSXP
//   Extrae una parte (token) del comando de configuracion del port serial.
static char *CommToken(char **ptr)
{
	char *p = *ptr;
	while(*p && isspace(*p)) p++;
	char *res = p;
    while(*p && (*p != ',')) p++;
	if(*p == ',') *p++ = 0;
	*ptr = p;
	Trim(res);
	return res;
}

//   Lee la string de configuracion del puerto y prepara su activacion. Valido solo para POSXP
void ConfigurePort(char *secc, DCB *conf)
{
	conf->DCBlength = sizeof(DCB);
	conf->BaudRate = 9600;
	conf->ByteSize = 8;
	conf->fAbortOnError = 0;
	conf->fOutxCtsFlow = 0;
	conf->fOutxDsrFlow = 0;
	conf->fDsrSensitivity = 0;
	conf->fOutX = 0;
	conf->fInX = 0;
	conf->fBinary = -1;
	conf->Parity = NOPARITY;
	conf->fParity = false;
	conf->StopBits = ONESTOPBIT;

	//  Por defecto, configura al port con '9600,N,8,1'
	char optBuff[200];
	Trim(STRCPY(optBuff, ExtractIniChar(secc, "config")));

	//  Velocidad de comunicacion.
	char *opt = optBuff;
	char *data = CommToken(&opt);
	if( _stricmp(data, "1200")==0 || _stricmp(data, "2400")==0 || _stricmp(data, "4800")==0 || _stricmp(data, "9600")==0 ||
		_stricmp(data, "14400")==0 || _stricmp(data, "19200")==0 || _stricmp(data, "38400")==0 || _stricmp(data, "57600")==0)
	{
		conf->BaudRate = atoi(data);
	}
	else if(strlen(data) != 0)
		WLog("ERROR en velocidad de comunicacion para '%s'", secc);

	//  Configura la paridad
	data = CommToken(&opt);
	if(data[0] == 'n' || data[0] == 'N')
		conf->Parity = NOPARITY;
	else if(data[0] == 'o' || data[0] == 'O')
		conf->fParity = ODDPARITY;
	else if(data[0] == 'e' || data[0] == 'E')
		conf->Parity = EVENPARITY;
	else if(data[0] == 'm' || data[0] == 'M')
		conf->fParity = MARKPARITY;
	else if(data[0] == 's' || data[0] == 'S')
		conf->fParity = SPACEPARITY;
	else if(strlen(data) != 0)
		WLog("ERROR en 'paridad' para '%s'", secc);

	// Cantidad de bits de datos
	data = CommToken(&opt);
	if(_stricmp(data, "7")==0 || _stricmp(data, "8")==0 || _stricmp(data, "9")==0)
		conf->ByteSize = atoi(data);
	else if(strlen(data) != 0)
		WLog("ERROR en la cantidad de bits de datos de '%s'", secc);

	// Cantidad de stop bits
	data = CommToken(&opt);
	if(_stricmp(data, "1")==0)
		conf->StopBits = ONESTOPBIT;
	else if(_stricmp(data, "1.5")==0)
		conf->StopBits = ONE5STOPBITS;
	else if(_stricmp(data, "2")==0)
		conf->StopBits = TWOSTOPBITS;
	else if(strlen(data) != 0)
		WLog("ERROR en la cantidad de bits de parada de '%s'", secc);
}
#endif

// Activa UN port serial.
void InstallSerial(int handl, char *secc)
{
    habil[handl] = ExtractIniSiNo(secc, "habilitado");
    #ifdef POSDEBUG
        sprintf(dbgStr, "InstallSerial: COM%d: (%s) %s.", handl + 1, secc,
                habil[handl]? "Habilitado": "Bloqueado");
        DBG(DBGPrintProc, 10, dbgStr);
    #endif
	if(!habil[handl]) return;

#ifdef POSXP

	DCB Config;
	hCom[handl] = CreateFile(secc, GENERIC_READ | GENERIC_WRITE,
					  0,					// Acceso exclusivo
					  NULL,					// Sin atributos de seguridad.
					  OPEN_EXISTING,
					  0,
					  NULL
					 );

	if(hCom[handl] == INVALID_HANDLE_VALUE) 
	{
		char msg[100];
		STRCAT(STRCPY(msg, "No puedo abrir el port serial "), secc);
		WriteBootLog(msg, BM_MSGZ);
		habil[handl] = 0;
		return;
	}

	//   Pone la configuracion.
	GetCommState(hCom[handl], &Config);
	ConfigurePort(secc, &Config);
	if(!SetCommState(hCom[handl], &Config))
	{
		STRCAT(STRCPY(msg, "No puedo setear los parametros de "), secc);
		WriteBootLog(msg, BM_MSGZ);
		habil[handl] = 0;
		return;
	}

	//   Especifica los timeouts que utilizara.
	COMMTIMEOUTS to;

	//  Lee los timeouts actuales.
	GetCommTimeouts(hCom[handl], &to);

	//  Indica que READ debe retornar inmediatamente, aunque no existan caracteres recibidos.
	to.ReadIntervalTimeout = MAXDWORD;
	to.ReadTotalTimeoutConstant = to.ReadTotalTimeoutMultiplier = 0;

	//  Indica que no se apliquen los timeouts a WRITE
	to.WriteTotalTimeoutConstant = to.WriteTotalTimeoutMultiplier = 5000;
	SetCommTimeouts(hCom[handl], &to);

	//  Indica que NO hay un caracter a retornar.
	preChar[handl] = -1;

#endif
}

//  Desactiva un port que no se usara mas.
void DeInitSerial(int handl)
{
#ifdef __VSC__
	CloseHandle(hCom[handl]);
	habil[handl] = 0;
#endif
}

//   Envia un caracter (ch) al puerto serial marcado por el handler.
//
void txChar(int handl, char ch)
{
#ifndef __VSC__

    union REGS ri, ro;

    if(!habil[handl]) return;
    ri.x.ax = 0x1202;
    ri.x.bx = handl;
    ri.x.dx = (int) ch;
    int86(0x17, &ri, &ro);
    #ifdef POSDEBUG
        //sprintf(dbgStr, "SERIALTXCHAR: %d='%c'", ch, ch);
        //DBG(DBGPrintProc, 10, dbgStr);
    #endif

#else

	unsigned long wrt;
	char tbuff[2];
	tbuff[0] = ch;
	tbuff[1] = 0;
	WriteFile(hCom[handl], tbuff, 1, &wrt, NULL);

#endif
}

//   Envia una string terminada en nulo, al puerto cuyo handler se pasa.
//
void txStr(int handl, char *str)
{
#ifndef __VSC__

    while(*str)
        txChar(handl, *str++);

#else

	unsigned long wrt;
	WriteFile(hCom[handl], str, strlen(str), &wrt, NULL);

#endif
}

/*
 *   Toma un caracter del buffer de un port serial.
*/
int rxChar(int handl)
{
	//   Espera que exista algun caracter listo.
    if(!habil[handl]) return 0;

	time_t rxTimer = time(NULL) + 5; // Ajustar si sale el mensaje de error muy seguido
	ErrorDuringRx = 0;
	while(preChar[handl] < 0 && !rxPoll(COM_PRN))
	{
		if(rxTimer <= time(NULL))
		{
			ErrorDuringRx = 1;
            sprintf(dbgStr, "SERIALPOLL: Timeout en COM%d: - Retornado '\r'", COM_PRN+1);
            DBG(DBGPrintProc, 10, dbgStr);
            return 13;
		}
	}

#ifndef __VSC__

    union REGS ri, ro;

    ri.x.ax = 0x1201;
    ri.x.bx = handl;
    int86(0x17, &ri, &ro);
    #ifdef POSDEBUG
        //sprintf(dbgStr, "SERIALRXCHAR: %d='%c'", ro.x.ax & 0xff,
        //                                         ro.x.ax & 0xff);
        //DBG(DBGPrintProc, 10, dbgStr);
    #endif

    return(ro.x.ax & 0xff);

#else
	char mBuff[2];
	unsigned long rd;

	//   Teoricamente, rxPoll ya dejo el caracter a retornar.
	if(preChar[handl] != -1)
	{
		mBuff[0] = (char)(preChar[handl] & 0xff);
		preChar[handl] = -1;
	}
	else
		ReadFile(hCom[handl], mBuff, 1, &rd, NULL);
	return mBuff[0];

#endif
}

/*
 * La siguiente funcion se utiliza para obtener el estado de las lineas
 * de modem desde un puerto serial.
 *
 * Utilizar las constantes MOD_CTS, MOD_DTR, MOD_OUT1 y MOD_OUT2 para
 * conocer el estado de cada uno de los bits que nos interesan.
*/

unsigned GetModemStat(int cual)
{
	return 0;
}

//  Vacia los buffers internos de comunicacion, eliminando cualquier
//  pendiente de leer o escribir.
void FlushComm(int handl)
{
#ifdef __VSC__

	FlushFileBuffers(hCom[handl]);
	while(rxPoll(handl))
		rxChar(handl);

#endif
}

/////////////////////////////////////////////////////////////////////
//Codigo en comun.
//   Esta rutina es llamado por el despachador de eventos, para avisar
//   a los modulos que los necesiten que se han producido novedades (inte-
//   rrupciones) en los puertos serie.
//   Por el momento, cincelarla a mano para que llame a las funciones nece-
//   sarias.
//
void reviseSerial()
{
	char param[3];
	int x;

	for(x = 0; x < 4; x++)
	{
		if(x != COM_PRN)
		{
            if(habil[x] && rxPoll(x))
			{
				param[0] = (char) x;
				param[1] = (char) rxChar(x);
				addEvent(E_SERIAL, param, 2);
			}
		}
	}
}

//   Utilizando la funcion anterior, la siguiente es la encargada de forzar
//   la exploracion de 'serial.ini' y el hardware instalado, tratando de
//   inicializar todos los puertos series que esten presentes.
//
int InitAllSerial()
{
	int x;

	if(OpenIni("serial"))
		AbnormalEnd("\nNo encuentro INI p/puertos serie.");
	for(x = 0; x < 4; x++)
		InstallSerial(x, seccN[x]);
	CloseIni();
	return 1;
}

//   Igual que la anterior, pero para volver a su valor original a los puertos
//   serie que hayan sido utilizados.
//
void DeInitAllSerial()
{
	int x;

	for(x = 0; x < 4; x++)
		DeInitSerial(x);
}

