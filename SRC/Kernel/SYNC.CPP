//
//    Sync.cpp
//    Programa de sincronizacion y ejecucion de ordenes.
//

#include "stdafx.h"
#include <exception>

#ifdef POSXP
#include "MainForm.h"
using namespace System::Net::NetworkInformation;
#endif

//   Variables de depuracion locales
#ifdef POSDEBUG
int DBGRxPlus, DBGCopyFile;
#endif

//   Datos globales.
int HostW, CopyInterlace, CopyPacketSize, GetPlusUsingDos;
int networkAvailable = 0, pingDelay, rebootear, autocierre;
long lastact;
time_t nextPingTry = 0;
char *tBuff = new char[1500];
char *host = new char[100];
static char *CopyBuff;

//   Verifica la existencia (y disponibilidad) de un archivo.
int Existe(char *s)
{	
	//int h = sopen(s, O_RDWR, SH_DENYNO);
	int h = _sopen(s, _O_RDWR, _SH_DENYNO);
	if(h >= 0) { close(h); return 1; }
	return 0;
}

int ExisteClr(char *s)
{
	int rta = 0;
	try
	{
		System::IO::FileStream ^unarc = System::IO::File::Open(gcnew String(s), System::IO::FileMode::Open, System::IO::FileAccess::ReadWrite, System::IO::FileShare::ReadWrite);
		rta = (unarc != nullptr ? 1 : 0);
		unarc->Close();
	}
	catch (System::Exception^ excep)
	{

	}
	return rta;
}

//   Reconecta a la red.
#ifndef _P_WAIT
   #define _P_WAIT P_WAIT
#endif

// Emite un PING de TCP/IP al server, para ver si la red esta activa. Si lo esta, retorna 1, sino, retorna 0
#ifdef POSXP
int SendPing(String ^pHost)
{
	Ping ^ping = gcnew Ping();
	PingOptions ^pingOptions = gcnew PingOptions(128, true);
	cli::array<System::Byte> ^buffer = System::Text::Encoding::ASCII->GetBytes("bitnet ping");
	if(pHost->StartsWith("\\\\")) pHost = pHost->Substring(2);
	int index = pHost->IndexOf('\\'); 
	if(index > 0) pHost = pHost->Substring(0, index);
	try
	{
		PingReply ^ans = ping->Send(pHost, 120, buffer, pingOptions);
		return (ans->Status == IPStatus::Success)? 1: 0;
	}
	catch(...) { return 0; }

	/*
	HANDLE h = CreateFile(host, 
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,		// Acceso compartido
				NULL,									// Sin atributos de seguridad.
				OPEN_ALWAYS,							// Lo crea si no existe
				FILE_ATTRIBUTE_NORMAL,					// Si lo crea, lo crea normal
				NULL);

	if(h != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(h);
		return 1;
	}
	return 0;*/
}
#endif

int SendPing(char* host)
{
#ifdef POSXP
	return SendPing(Strings::CharToString(host));
#else
	return 1;
#endif
}

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Verifica si la red esta disponible. Si lo estuviera, retorna 1, sino, retorna 0.
int CheckNetwork()
{
	// Si no se activo la opcion, indica que la red SIEMPRE esta disponible
	if(pingDelay <= 5) { networkAvailable = 1; return 1; }

	// Reintenta la conexion cada 30 segundos
	time_t actual = time(NULL);
	if(actual < nextPingTry) return networkAvailable;
	nextPingTry = time(NULL) + pingDelay;

	// Si la red NO estaba disponible, mira si corresponde emitir un PING para ver si lo esta.
	if(!networkAvailable)
	{
		// Si el ping funciona, nos reconectamos...
		if(SendPing(host))
		{
			if(Existe("connect.bat"))
				_spawnl(_P_WAIT, "c:\\command.com", "command", "/C", "connect.bat", ">nul", NULL);
			
			networkAvailable = 1;
			AdicClear();		// Refresca el msg 'Online/Offline' en la pantalla.
			return 1;
		}

		// La red sigue abajo, establece la cuenta de espera de nuevo
		nextPingTry = actual + pingDelay;
		return 0;
	}

	// La red SI estaba disponible. Emite un PING para ver si SIGUE disponible.
	if(SendPing(host)) return 1;
	networkAvailable = 0;
	nextPingTry = time(NULL) + pingDelay;
	AdicClear();
	return 0;
}

// Verifica si el archivo recibido es el archivo indicado.
bool CheckReceivedFile(char *dst, char *fn)
{
	int lFn = strlen(fn);
	int lDst = strlen(dst);
	if(lFn <= lDst)
	{
		char *pos = dst + (lDst - lFn);
		return (_stricmp(pos, fn) == 0);
	}
	return false;
}

//   Copia archivos.
int xCopyFile(char *src, char *dst, char *tMsg, bool usaTemp)
{
	int sh, dh, copied, tCopied, Interlace, writted;
	char *buff, *tdst;

	//   Establece el buffer a usar.
    if(CopyBuff == NULL)
		CopyBuff = (char *) MALLOC(CopyPacketSize + 10, "xCopyFile");
	if(CopyBuff == NULL)
	{
		DBG(DBGCopyFile, 1, "DBGCopyFile: No consigo ubicar buffer. No copio nada...");
		return 0;
	}
	buff = CopyBuff;

	//   Abre los archivos.
	sh = sopen(src, O_RDONLY | O_BINARY, SH_DENYNO);
	if(sh < 0)
	{
		DBG(DBGCopyFile, 1, "DBGCopyFile: No encuentro el origen para copiarlo:");
		DBG(DBGCopyFile, 1, src);
		return 0; 
	}

	tdst = (char *)MALLOC(strlen(dst) + 100, "xCopyFile::tdst");
	STRCPY(tdst, dst); 
	if(usaTemp) ReplaceExtension(tdst, ".tmp", true);
	remove(tdst);
	dh = open(tdst, O_RDWR | O_BINARY);
	if(dh < 0)
	{
		int h = creat(tdst, S_IREAD | S_IWRITE);
		if(h >= 0) close(h);
	    dh = open(tdst, O_RDWR | O_BINARY);
		if(dh < 0)
		{
			DBG(DBGCopyFile, 1, "DBGCopyFile: No pudo crear el archivo destino:");
			DBG(DBGCopyFile, 1, dst);
			close(sh);
			FREE(tdst);
			return 0;
		}
	}
	
	OpenTermo(tMsg, filelength(sh));
	tCopied = 0;
	Interlace = CopyInterlace;
	do {
			if(--Interlace <= 0)
			{
				Interlace = CopyInterlace;
			    copied = read(sh, buff, CopyPacketSize);
			    if(copied > 0)
				{
				   tSwitch();
				   writted = write(dh, buff, copied);
				   tCopied += copied;
				   AcTermo(tCopied);
				}
			}
			else
				copied = 1;		// Sino, sale del loop a partir del 2do archivo!!!

			tSwitch();
	} while(copied > 0);
	
	if(filelength(dh) != filelength(sh))
	{
		#ifdef POSDEBUG
		DBG(DBGCopyFile, 1, "DBGCopyFile: No coinciden los tamaños.");
        sprintf(tdes_, "Fuente: %d, Destino: %d.", filelength(sh), filelength(dh));
	    DBG(DBGCopyFile, 1, tdes_);
		#endif
		close(sh); close(dh);
		FREE(tdst);
		remove(dst);
		return 0;
	}
	
	#ifdef POSDEBUG
	sprintf(tdes_, "DBGCopyFile: Copia finalizada. Copie %d bytes.", tCopied);
	DBG(DBGCopyFile, 1, tdes_);
	#endif
	close(sh);
	close(dh);
	CloseTermo();

	// Si uso un temporario para la copia, entonces lo renombra definitivamente.
	if(usaTemp)
	{
		if(remove(dst))
			DBG(DBGCopyFile, 1, "WARNING: No pudo quitar el archivo destino viejo.");

		if(rename(tdst, dst))
		{
			char *tmp = dst + strlen(dst - 1);
			while(tmp > dst)
			{
				if(*tmp == '\\')
				{
					tmp++;
					break;
				}
				tmp--;
			}
			if(rename(tdst, dst))
				DBG(DBGCopyFile, 10, "FATAL: No pudo renombrar el archivo recien recibido.");
		}
	}

	FREE(tdst);

	// Si llego nueva configuracion de logos
	if(CheckReceivedFile(dst, "logos.ini")) 
		reloadLogos = true;

	// Llegaron promos...
	if(CheckReceivedFile(dst, "promo.ini") || CheckReceivedFile(dst, "canastas.dbf"))
		forceReloadPromos = true;	

	return 1;
}

// Por compatibilidad, esta funcion usa a la anterior creando temporarios.
int xCopyFile(char *src, char *dst, char *tMsg)
{
	return xCopyFile(src, dst, tMsg, true);
}

//   Copia un archivo utilizando directamente al COMMAND.COM
int xCopyDosFile(char *src, char *dst, char *tMsg)
{
	int sh, dh, xRet;

	//   Abre los archivos.
	DBG(DBGCopyFile, 1, "DBGCopyFile: Copia con MsDos. Fuente: ");
	DBG(DBGCopyFile, 1, src);
	sh = sopen(src, O_RDWR | O_BINARY, SH_DENYNO);
	if(sh < 0) 
	{
		DBG(DBGCopyFile, 1, "No encuentro el origen.");
		return 0; 
	}
	close(sh);
    remove(dst);

	//   Efectua el volcado.
	xRet = (int)_spawnl(_P_WAIT, "c:\\command.com", "command", "/C", "copy", src, dst, ">nul", NULL);
	#ifdef POSDEBUG
	sprintf(tdes_, "DBGCopyFile: El _spawnl de COPY retorno %d - errno: %d.", xRet, errno);
	DBG(DBGCopyFile, 2, tdes_);
	#endif

	//   Verifica que la copia haya tenido exito.
	sh = sopen(src, O_RDWR | O_BINARY, SH_DENYNO);
	dh = sopen(dst, O_RDWR | O_BINARY, SH_DENYNO);
	if(sh < 0 || dh < 0 || filelength(dh) != filelength(sh))
	{
		DBG(DBGCopyFile, 1, "DBGCopyFile: Fallo la copia.");
		close(sh); close(dh);
		remove(dst);
		return 0;
	}
	DBG(DBGCopyFile, 1, "Copia finalizada con exito.");
	close(sh);
	close(dh);
	return 1;
}

//   Recibe actualizaciones de articulos.
void RecibePlus()
{
	char *pEnd;

	GlobalGetPrec = 0;
	WriteCmos(FLAGPREC);
	if(GetPlusUsingDos)
	{
		SendMsg("Espere un momento.", "Recibiendo actualizaciones...");
		DBG(DBGRxPlus, 1, "DBGRxPlus: Se recibiran usando MsDos. Despachando el batch...");
		_spawnl(_P_WAIT, "c:\\command.com", "command", "/C", "copyplus.bat", ">nul", NULL);
		DBG(DBGRxPlus, 1, "DBGRxPlus: Regresó del Batch.");
		ClearMsg();
	}
	else
	{
		if (*dvariac == 0)
			STRCAT(STRCPY(tBuff, p_Log), "\\datos\\");
		else 
			STRCAT(STRCPY(tBuff, p_Log), dvariac);
		if (access(tBuff, 0) != 0){
			WriteBootLog("Variable \"dvariac\", no es un directorio valido!!");
			return;
		}
		pEnd = tBuff + strlen(tBuff);
		STRCAT(tBuff, "mplu.dbf");
		remove(NEW_PLU);
		xCopyFile(tBuff, NEW_PLU, "Articulos. Paso 1/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mscan.dbf");
		remove(NEW_SCAN);
		xCopyFile(tBuff, NEW_SCAN, "Articulos. Paso 2/7...");
	
		*pEnd = 0;
		STRCAT(tBuff, "mscancod.idx");
		remove(NEW_SCANIDX1);
		xCopyFile(tBuff, NEW_SCANIDX1, "Articulos. Paso 3/7...");
	
		*pEnd = 0;
		STRCAT(tBuff, "mscansc.idx");
		remove(NEW_SCANIDX2);
		xCopyFile(tBuff, NEW_SCANIDX2, "Articulos. Paso 4/7...");
	
		*pEnd = 0;
		STRCAT(tBuff, "mplucod.idx");
		remove(NEW_PLUIDX1);
		xCopyFile(tBuff, NEW_PLUIDX1, "Articulos. Paso 5/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mpludes.idx");
		remove(NEW_PLUIDX2);
		xCopyFile(tBuff, NEW_PLUIDX2, "Articulos. Paso 6/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mplumarc.idx");
		remove(NEW_PLUIDX3);
		xCopyFile(tBuff, NEW_PLUIDX3, "Articulos. Paso 7/7...");
		variacionpp = false;
		WriteCmos(VARCONPP);
	}

	//   Pone la marca para que asienten al final del ticket.
	GlobalGetPrec = 1;
	WriteCmos(FLAGPREC);
	DBG(DBGRxPlus, 1, "DBGRxPlus: Marcado en CMOS los nuevos articulos.");
}


//   Recibe actualizaciones de articulos para precios puntuales.
void RecibePlusPuntual()
{
	char *pEnd;

	GlobalGetPrec = 0;
	WriteCmos(FLAGPREC);
	if(GetPlusUsingDos)
	{
		SendMsg("Espere un momento.", "Recibiendo actualizaciones...");
		DBG(DBGRxPlus, 1, "DBGRxPlus: Se recibiran usando MsDos. Despachando el batch...");
		_spawnl(_P_WAIT, "c:\\command.com", "command", "/C", "copyplus.bat", ">nul", NULL);
		DBG(DBGRxPlus, 1, "DBGRxPlus: Regresó del Batch.");
		ClearMsg();
	}
	else
	{
		if (*dirpuntual == 0)
			STRCAT(STRCPY(tBuff, p_Log), "\\puntuales\\");
		else 
			STRCAT(STRCPY(tBuff, p_Log), dirpuntual);
		if (access(tBuff, 0) != 0){
			WriteBootLog("Variable \"dirpuntual\", no es un directorio valido!!");
			return;
		}
		pEnd = tBuff + strlen(tBuff);
		STRCAT(tBuff, "mplu.dbf");
		remove(NEW_PLU);
		xCopyFile(tBuff, NEW_PLU, "Articulos. Paso 1/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mscan.dbf");
		remove(NEW_SCAN);
		xCopyFile(tBuff, NEW_SCAN, "Articulos. Paso 2/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mscancod.idx");
		remove(NEW_SCANIDX1);
		xCopyFile(tBuff, NEW_SCANIDX1, "Articulos. Paso 3/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mscansc.idx");
		remove(NEW_SCANIDX2);
		xCopyFile(tBuff, NEW_SCANIDX2, "Articulos. Paso 4/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mplucod.idx");
		remove(NEW_PLUIDX1);
		xCopyFile(tBuff, NEW_PLUIDX1, "Articulos. Paso 5/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mpludes.idx");
		remove(NEW_PLUIDX2);
		xCopyFile(tBuff, NEW_PLUIDX2, "Articulos. Paso 6/7...");

		*pEnd = 0;
		STRCAT(tBuff, "mplumarc.idx");
		remove(NEW_PLUIDX3);
		xCopyFile(tBuff, NEW_PLUIDX3, "Articulos. Paso 7/7...");
		variacionpp = true;
		WriteCmos(VARCONPP);		
		WriteCmos(APPLYVARPP);
		
	}

	//   Pone la marca para que asienten al final del ticket.
	GlobalGetPrec = 1;
	WriteCmos(FLAGPREC);
	DBG(DBGRxPlus, 1, "DBGRxPlus: Marcado en CMOS los nuevos articulos.");
}


//   Recibe las promociones archivos promo.ini y canastas.dbf
int RecibePromos()
{
	
	char dwork[100];

	forceReloadPromos = false;
	WriteCmos(FRELPROMOS);
	
		if (*dirpromo == 0)
			STRCAT(STRCPY(tBuff, p_Log), "\\diego\\");
		else 
			STRCAT(STRCPY(tBuff, p_Log), dirpromo);
		STRCPY(dwork, tBuff);
		if (*fpromo == 0)
			STRCPY(fpromo, "promo.ini");
		if (*fcanastas == 0)
			STRCPY(fcanastas, "canastas.dbf");
		if (access(tBuff, 0) != 0)
		{
			WriteBootLog("Variable \"dirpromo\", no es un directorio valido!!");
			return 0;
		}
		STRCAT(tBuff, fpromo);
		if (access(tBuff, 0) != 0)//Verifica la existencia del archivo
		{
			WriteBootLog("RecibePromos - No encuentro archivo de promociones");
			return 0;
		}
		STRCPY(tBuff, dwork);
		STRCAT(tBuff, fcanastas);
		if (access(tBuff, 0) != 0)
		{
			WriteBootLog("RecibePlus - No encuentro archivo de canastas");
			return 0;
		}
		STRCPY(tBuff, dwork);
		
		STRCAT(tBuff, fpromo);
		remove("pos\\promo.new");
		xCopyFile(tBuff, "pos\\promo.new", "");
		
		STRCPY(tBuff, dwork);
		STRCAT(tBuff, fcanastas);
		remove("pos\\canastas.new");
		xCopyFile(tBuff, "pos\\canastas.new", "");
		
		STRCPY(tBuff, dwork);	

	//   Pone la marca para que asienten al final del ticket.
	/*GlobalGetPrec = 1;
	WriteCmos(FLAGPREC);
	DBG(DBGRxPlus, 1, "DBGRxPlus: Marcado en CMOS los nuevos articulos.");*/
	forceReloadPromos = true; //recarga las promociones
	WriteCmos(FRELPROMOS);
	//Agregado para recargar las promociones solo si no esta cobrando
	if (inOper == 0)
		ReloadPromos();
	return 1;
}



int RecibePremioInst()
{
	
	char dwork[100];
	char ltpremio[30], lpremioin[30];

	reloadPremioInst = false;
	WriteCmos(FRELPREMIN);
	
		if (*dirpromo == 0)
			STRCAT(STRCPY(tBuff, p_Log), "\\PremioInst\\");
		else 
			STRCAT(STRCPY(tBuff, p_Log), dirpromo);
		STRCPY(dwork, tBuff);
		sprintf(ltpremio, "tipoprem.%03d", caja);
		sprintf(lpremioin, "premioin.%03d", caja);
		if (access(tBuff, 0) != 0)
		{
			WriteBootLog("Variable \"dirpromo\", no es un directorio valido!!");
			return 0;
		}
		STRCAT(tBuff, ltpremio);
		if (access(tBuff, 0) != 0)//Verifica la existencia del archivo
		{
			WriteBootLog("RecibePromos - No encuentro archivo de Tipo de premios Instantaneos Tipoprem.dbf");
			return 0;
		}
		STRCPY(tBuff, dwork);
		STRCAT(tBuff, lpremioin);
		if (access(tBuff, 0) != 0)
		{
			WriteBootLog("RecibePlus - No encuentro archivo de Distribucion Premioin.dbf");
			return 0;
		}
		STRCPY(tBuff, dwork);
		
		STRCAT(tBuff, ltpremio);
		remove("pos\\tipoprem.new");
		xCopyFile(tBuff, "pos\\tipoprem.new", "");
		
		STRCPY(tBuff, dwork);
		STRCAT(tBuff, lpremioin);
		remove("pos\\premioin.new");
		xCopyFile(tBuff, "pos\\premioin.new", "");
		
		STRCPY(tBuff, dwork);	

	reloadPremioInst = true;
	WriteCmos(FRELPREMIN);
	return 1;
}


//   La siguiente funcion envia un comando al servidor.
void SendCmdToServer(int cmd, char *cParam)
{
	static dbf xAcc;
	int pExclusive, salir;

	STRCAT(STRCPY(tBuff, p_Log), "\\comandos.dbf");
	pExclusive = view._exclusive;
	view._exclusive = 0;
	salir = 0;
	if(Existe(tBuff) &&  xAcc.Use(tBuff))
	{
		while(!xAcc.Eof())
		{
			xAcc.Get("estado", tBuff);
			if(atoi(tBuff) == 3)
				break;
			xAcc.Skip(1);
		}
		if(xAcc.Eof())
		{
			xAcc.AppendBlank();
			xAcc.Replace("estado", "4");
		}

		xAcc.Replace("caja", "0");

		/////   SEGUIR DESDE AQUI    ////////
		xAcc.Close();
	}
}

// Optimiza el codigo
#ifdef POSXP
#pragma managed(pop)
#endif

// Llama a la funcion manejada que cierra el pos
void Cerrar()
{
#ifdef POSXP
	MainForm::F->DoClose();
#else
	POSExit();
#endif
}

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Funcion principal de sincronizacion.
time_t lastSyncro;
int inSyncro;
dbf xAcc;

void Syncronize()
{
	char retValue[40], *p1, *p2;
	
	int pExclusive, comando, tickNro;
	time_t actSyncro;
	bool salir = false;

	if(!CheckNetwork()) return;

	time(&actSyncro);
	if(inSyncro || (actSyncro <= lastSyncro)) return;

	inSyncro++;
	lastSyncro = time(&actSyncro)  + 5;
	STRCAT(STRCPY(tBuff, p_Log), "\\comandos.dbf");
	pExclusive = view._exclusive;
	view._exclusive = 0;
	if(Existe(tBuff) &&  xAcc.Use(tBuff))
	{
		char *tfile = new char[200], *tdst = new char[200];
		char *msgparam = new char[200];
		while(!xAcc.Eof())
		{
			xAcc.Get("caja", tBuff);
			if(atoi(tBuff) == caja)
			{
				xAcc.Get("estado", tBuff);
				if(atoi(tBuff) < 3)
				{
					//   Termina el Screen Saver, si esta ejecutandose.
					terminateSSaver = 1; tSwitch();

					//   Indica que el comando esta siendo procesado				
					while(!xAcc.Lock(DB_LOCK)) tSwitch();
					xAcc.Replace("estado", "2");
					xAcc.Lock(DB_UNLOCK);
					xAcc.Flush();

					// Obtiene el comando a procesar
					xAcc.Get("comando", tBuff);
					STRCPY(retValue, "3");				// Valor final por default
					comando = atoi(tBuff);
					switch(comando)
					{
						case 1:				// Recibe archivo de artículos.
							DBG(DBGRxPlus, 10, "DBGRxPlus: Comando detectado. Llamando a RecibePlus");
							RecibePlus();
							break;

						case 2:				// Traba la registradora (no deja vender).
							if(!LockRegister(1))
								STRCPY(retValue, "2");
							break;

						case 3:				// Destraba la registradora.
							if(!LockRegister(0))
								STRCPY(retValue, "2");
							break;

						case 4:				// Vacía el archivo de log.
							//ZeroDump();
							//salir = 1;
							break;

						case 5:				// Saca una zeta.
							z_clrlog = 1;
							ForceZeta();
							salir = true;

							if(bSaleRepElec &&  DesdeRepElec > 0 && HastaRepElec > 0 && DesdeRepElec <= HastaRepElec)
								sprintf(tfile, "%s", InfoReporteElectronico(DesdeRepElec, HastaRepElec));
							bSaleRepElec = false;
							WriteCmos(SACAREPEL);
							DesdeRepElec = 0;
							WriteCmos(DESDEREPEL);
							HastaRepElec = 0;
							WriteCmos(HASTAREPEL);							
							break;

						case 6:				// Copia un archivo a/desde el server.
							xAcc.Get("param", tBuff);

							//  Del parametro, separa en 'p1' y 'p2' los nombres de los archivos fuente y destino, respectivamente.
							for(p1 = tBuff; *p1 && isspace(*p1); p1++)
								;
							for(p2 = p1; *p2 && !isspace(*p2); p2++)
								;
							*p2++ = 0;
							while(*p2 && isspace(*p2))
								p2++;
							if(Existe(p1))
							{
								TemporaryClose(p2);
								sprintf(tfile, "%s --> %s * ", p1, p2);
								if(xCopyFile(p1, p2, "Recibiendo Archivo..."))
									STRCAT(tfile, "Ok");
								else
									STRCAT(tfile, "Fallo");
							}
							else
								sprintf(tfile, "%s : No existe", p1);
							xAcc.Replace("param", tfile);
							break;

						case 7:				// Actualiza una cuenta corriente.
							xAcc.Get("param", tBuff);
							Trim(tBuff);
							//ActualizeCC(tBuff);
							break;

						case 8:				// Activa el monitoreo de tickets.
							ActivaMonitoreo(1);
							break;

						case 9:				// Desactiva el monitoreo e tickets
							DesactivaMonitoreo();
							break;

						case 10:			// Manda ventas 'rapidas' al servidor
							sprintf(tfile, "qventas.%03d", caja);
							QuickVentas(tfile, -1);
							STRCAT(STRCAT(STRCPY(tdst, p_Log), "\\"), tfile);
							xCopyFile(tfile, tdst, "Ventas", false);
							break;

						case 11:			// Apaga o rebootea al pos
							xAcc.Get("param", tBuff);
							Trim(tBuff);
							errorCode = atoi(tBuff);
							rebootear = 1;
							if(inOper == 0) salir = 1;
							break;

						case 12:			// Envia un ticket al server
							xAcc.Get("param", tBuff);
							Trim(tBuff);
							tickNro = atoi(tBuff);
							remove("tkdat.dbf");
							MoveTicket("tkdat.dbf", tickNro);
							STRCAT(STRCPY(tBuff, p_Log), "\\tkdat.dbf");
							xCopyFile("tkdat.dbf", tBuff, "Ticket Solicitado", false);
							break;

						case 13:			// Cambia la configuracion del impresor
							xAcc.Get("param", tBuff);
							Trim(tBuff);
							pf->Configure(tBuff);
							break;

						case 14:				// Recibe archivo de artículos para precios puntuales
							DBG(DBGRxPlus, 10, "DBGRxPlus: Comando detectado. Llamando a RecibePlusPuntual");
							RecibePlusPuntual();
							break;

						case 15:				// Recibe archivo de promociones
							//DBG(DBGRxPlus, 10, "DBGRxPlus: Comando detectado. Llamando a RecibePlus");
							if (RecibePromos())
								STRCPY(msgparam, "Promociones copiadas");
							else 
								STRCPY(msgparam, "No se pudo realizar copia");
							xAcc.Replace("param", msgparam);
							break;

						case 16:				// Recibe archivo de premios instantaneos							
							if (RecibePremioInst())
								STRCPY(msgparam, "Archivos copiados");
							else 
								STRCPY(msgparam, "No se pudo realizar copia");
							xAcc.Replace("param", msgparam);
							break; 
						case 17:				// Devuelve la version de la linea de cajas
							sprintf(msgparam, "%s - %s - %s", posVersion, fecha(), hora());
							xAcc.Replace("param", msgparam);
							break;
						case 18:
							STRCPY(msgparam, "Las tarjetas se recargaran despues de cerrar un ticket");
							reloadtjon = true; //Recarga las tarjetas online al cerrar el ticket (NO medios de pago)
							xAcc.Replace("param", msgparam);
							break;
						case 19:
							xAcc.Get("param", tBuff);

							//  Del parametro, separa en 'p1' y 'p2' los nombres de los archivos fuente y destino, respectivamente.
							for(p1 = tBuff; *p1 && isspace(*p1); p1++)
								;
							for(p2 = p1; *p2 && !isspace(*p2); p2++)
								;
							*p2++ = 0;
							while(*p2 && isspace(*p2))
								p2++;
							
							if (inOper == 0)														
								sprintf(tfile, "%s", InfoReporteElectronico(atoi(p1), atoi(p2)));
							else							
								sprintf(tfile, "%s : Caja ocupada en ticket", p1);							
							xAcc.Replace("param", tfile);							
							break;
						case 20:
							xAcc.Get("param", tBuff);

							//  Del parametro, separa en 'p1' y 'p2' los nombres de los archivos fuente y destino, respectivamente.
							for(p1 = tBuff; *p1 && isspace(*p1); p1++)
								;
							for(p2 = p1; *p2 && !isspace(*p2); p2++)
								;
							*p2++ = 0;
							while(*p2 && isspace(*p2))
								p2++;
							bSaleRepElec = true;
							WriteCmos(SACAREPEL);
							DesdeRepElec = atoi(p1);
							WriteCmos(DESDEREPEL);
							HastaRepElec = atoi(p2);
							WriteCmos(HASTAREPEL);

							sprintf(tfile, "%s : Comando recibido", p1);
							
							xAcc.Replace("param", tfile);							
							break;
						case 21:
							xAcc.Get("param", tBuff);

							//  Del parametro, separa en 'p1' y 'p2' los nombres de los archivos fuente y destino, respectivamente.
							for(p1 = tBuff; *p1 && isspace(*p1); p1++)
								;
							for(p2 = p1; *p2 && !isspace(*p2); p2++)
								;
							*p2++ = 0;
							while(*p2 && isspace(*p2))
								p2++;

							if (inOper == 0)														
								sprintf(tfile, "%s", CopiaDocumentoFiscal(atoi(p1), atoi(p2)));
							else							
								sprintf(tfile, "%s : Caja ocupada en ticket", p1);							
							xAcc.Replace("param", tfile);							
							break;
						case 22:
							xAcc.Get("param", tBuff);

							//  Del parametro, separa en 'p1' y 'p2' los nombres de los archivos fuente y destino, respectivamente.
							for(p1 = tBuff; *p1 && isspace(*p1); p1++)
								;
							for(p2 = p1; *p2 && !isspace(*p2); p2++)
								;
							*p2++ = 0;
							while(*p2 && isspace(*p2))
								p2++;
							bSaleRepElec = true;
							WriteCmos(SACAREPEL);
							DesdeRepElec = atoi(p1);
							WriteCmos(DESDEREPEL);
							HastaRepElec = atoi(p2);
							WriteCmos(HASTAREPEL);

							sprintf(tfile, "%s : Comando recibido", p1);

							xAcc.Replace("param", tfile);							
							break;

					}
					xAcc.Replace("estado", retValue);
					xAcc.Flush();
				}
			}

			xAcc.Skip(1);
		}

		xAcc.Close();
		delete tfile; delete tdst;
		delete msgparam;
	}
	view._exclusive = pExclusive;
	inSyncro = 0;

	time_t untmp;
	time(&untmp);

	// Mira si hizo timeout la seccion
	if(autocierre > 0 && HayQueCerrar(lastact, autocierre))
	//if(autocierre > 0 && (autocierre + lastact) <= time__())
	{
		if(ckaNro != 0 && rMode < 1 && inOper == 0 && !chkNoSusp(0))
		{
			EnAutocierre = true;
			clerk(0);
			EnAutocierre = false;
		}
		lastact = time__();
	}

	// Si corresponde cerrar el pos
	if(salir) 		
		Cerrar();
}



////////////////////////////// COMANDOS POS /////////////////////////////
//  Fuerza que la registradora se trabe y que no permita ser operada.
int LockRegister(int que)
{
	if(que)
	{
		if(RegisterLocked) return 1;
		if(inOper)
		{
			Alert(
				"Se ordeno una ZETA desde el servidor",
				"Por favor, finalice el ticket actual para poder emitirla");

			// Espera que se termine la operacion en curso
			while(inOper) tSwitch();
		}

		if(chkNoSusp(0))
		{
			Alert(
				"Se ordeno una ZETA desde el servidor, pero hay tickets",
				"en espera. Por favor, resuelva el problema...");

			// Espera que se resuelva el problema
			while(inOper || chkNoSusp(0)) tSwitch();
		}

		RegisterLocked++;
		DispAdic(2, 1, "Caja Cerrada!!!");
		CustDisp("Caja Cerrada", "");
		return 1;
	}
	else
	{
		if(!RegisterLocked) return 1;
		RegisterLocked = 0;
		DispAdic(2, 2, "             ");
		CustDisp(saludos[0], saludos[1]);
		DispAdic(2, 1, "Caja Habilitada");
		return 1;
	}
	return 1;
}

//  Fuerza la emision de un reporte ZETA.
void ForceZeta()
{
    WriteBootLog("Ordenada ZETA REMOTA. LOCK(1)", BM_MSG);
    LockRegister(1);                        // Traba la registradora.

    WriteBootLog("ZETA REMOTA. clerk(0)", BM_MSG);
	xReg = 0.0;	clerk(1);					// Desactiva cajero.

    WriteBootLog("ZETA REMOTA: Cambiando a Z", BM_MSG);
	xReg = 3.0; chgmode(1);                 // Pasa a modo zeta.

    WriteBootLog("ZETA REMOTA: MkReport", BM_MSG);
	xReg = 0.0; mkReport();                 // Emite la zeta.

    WriteBootLog("ZETA REMOTA: Cambiando a REG", BM_MSG);
	xReg = 0.0; chgmode(1);                 // Vuelve a registro.

//  El 'ZeroDump()' ya es invocado al final de 'mkReport()'
//    WriteBootLog("ZETA REMOTA: ZeroDump", BM_MSG);
//	ZeroDump();								// Vacia el archivo de dump.

    WriteBootLog("ZETA REMOTA: Lock(0)", BM_MSG);
	LockRegister(0);						// Desbloquea la registradora.
}

char *InfoReporteElectronico(int desde, int hasta)
{
	char rtaParam[200];
	char paraudit[200];

	LockRegister(1);                        // Traba la registradora.	
	//if(pf->GeneraReporteAfip(desde,hasta))
	pf->ObtieneRangosReportes(desde);
	while (pf->frepdesde < hasta){
		if(pf->GeneraReporteAfipNew(pf->frepdesde, pf->frephasta))
		{
			//TemporaryClose(p2);
			sprintf(rtaParam, "%s --> audi%d.%s.%03d * ", "rep.txt", pf->frepdesde, pf->ptovta, caja);
			sprintf(paraudit, "%s\\audi%d.%d.%s.%03d", p_Log, pf->frepdesde, pf->frephasta, pf->ptovta, caja);
			if(xCopyFile("rep.txt", paraudit, "Recibiendo Archivo..."))
				STRCAT(rtaParam, "Ok");
			else
				STRCAT(rtaParam, "Fallo");
		}
		else
			sprintf(rtaParam, "%d : No Hay", pf->frepdesde);
		pf->ObtieneRangosReportes(pf->frepsig);
	}
	LockRegister(0);						// Desbloquea la registradora.
	return rtaParam;
}

char *InfoReporteElectronicoMes(int desde, int hasta)
{
	char rtaParam[200];
	char paraudit[200];

	LockRegister(1);                        // Traba la registradora.	
	//if(pf->GeneraReporteAfip(desde,hasta))
	pf->ObtieneRangosReportesMes(desde);
	while (pf->frepdesde < hasta){
		if(pf->GeneraReporteAfipNew(pf->frepdesde, pf->frephasta))
		{
			//TemporaryClose(p2);
			sprintf(rtaParam, "%s --> audi%d.%s.%03d * ", "rep.txt", pf->frepdesde, pf->ptovta, caja);
			sprintf(paraudit, "%s\\audi%d.%d.%s.%03d", p_Log, pf->frepdesde, pf->frephasta, pf->ptovta, caja);
			if(xCopyFile("rep.txt", paraudit, "Recibiendo Archivo..."))
				STRCAT(rtaParam, "Ok");
			else
				STRCAT(rtaParam, "Fallo");
		}
		else
			sprintf(rtaParam, "%d : No Hay", pf->frepdesde);
		pf->ObtieneRangosReportesMes(pf->frepsig);
	}
	LockRegister(0);						// Desbloquea la registradora.
	return rtaParam;
}

//Imprime una copia del documento fiscal indicado, al momento funciona en Hasar 250
char *CopiaDocumentoFiscal(int tipoDocumento, int nroDocumento)
{
	char rtaParam[200];
	char paraudit[200];

	LockRegister(1);                        // Traba la registradora.	
	if(pf->CopiaDocumento(tipoDocumento, nroDocumento))	
		sprintf(rtaParam, "Ok");	
	else
		sprintf(rtaParam, "%d : No Hay", nroDocumento);
	LockRegister(0);						// Desbloquea la registradora.
	return rtaParam;
}
