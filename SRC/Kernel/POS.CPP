//
//   Pos.c
//   Modulo principal del programa de Registracion POS.EXE
//

#include <stdafx.h>

// Tipo de pantalla a mostrar.
int pantalla = 0;
bool usaTouch = false;

// El codigo de error que la aplicacion devolvera al salir. 0=Ok
int errorCode = 0;

// El mensaje que indica la version del POS
char *posVersion = new char[100];

// Total de la operacion en curso.
tagTotal xTotal = tagTotal();

// Datos encriptados.
char licFile[] = {
	'b' ^ 'C',
	'i' ^ 'A',
	't' ^ 'C',
	'n' ^ 'A',
	'e' ^ ' ',
	't' ^ 'D',
	'.' ^ 'E',
	'l' ^ ' ',
	'i' ^ 'G',
	'c' ^ 'I',
	0 
};

char licError[] = {
	'F' ^ 'C',
	'a' ^ 'A',
	'l' ^ 'C',
	'l' ^ 'A',
	'o' ^ ' ',
	' ' ^ 'D',
	'e' ^ 'E',
	'n' ^ ' ',
	' ' ^ 'G',
	'l' ^ 'I',
	'i' ^ 'Z',
	'c' ^ 'M',
	'e' ^ 'O',
	'n' ^ 'C',
	'c' ^ 'A',
	'i' ^ 'C',
	'a' ^ 'A',
	'(' ^ ' ',
	'%' ^ 'D',
	's' ^ 'E',
	',' ^ ' ',
	' ' ^ 'G',
	'%' ^ 'I',
	's' ^ 'Z',
	')' ^ 'M',
	0
};

char licFileError[] =
{
	'M' ^ 'C',
	'a' ^ 'A',
	'n' ^ 'C',
	'o' ^ 'A',
	's' ^ ' ',
	'e' ^ 'D',
	'a' ^ 'E',
	'd' ^ ' ',
	'o' ^ 'G',
	0
};

// El impresor DUMMY existe siempre
//extern "C" {
//	extern PrinterFiscal_ P_DUMMY;
//};

#ifdef POSXP
#include "MainForm.h"
#include "StaticDump.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Globalization;
using namespace System::Threading;
using namespace pos;
using namespace System::IO;
using namespace System::Diagnostics;
using namespace Microsoft::Win32;

#endif

int flag, DelayTime, iHand, Inicializando, ForzarZeta;
char *msg = new char[10000];	// Array de propósitos generales.

#ifdef PANTACOLOR
short initesq[16] = {           // Esquema default de colores
	{ ROJO * 16 + BLANCO },     // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ ROJO * 16 + VERDE }       // Texto contenido.
};
#else
short initesq[16] = {           // Esquema default de colores
	{ BLANCO * 16 + NEGRO },    // Color del borde
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ BLANCO * 16 + NEGRO }     // Texto contenido.
};
#endif

//   Esquema default de colores para algunas ventanas.
short prnesq[16];

//   La siguiente variable hace que cstartup inicialice un determinado stack.
unsigned _stack = 50000;

//   En el siguiente array se contienen los valores de proteccion.
char ProtArray[15] = "srqt";

//   Funciones de inicializacion/Remocion de sub-modulos.

//   Variables para control del stack local.
#ifndef __VSC__
unsigned xDosStackSS, xDosStackSP;      // Stack con el que entra.
unsigned xMyStackSS, xMyStackSP;        // Stack de este programa.
unsigned xMyStackSPT;                   // Tope de Stack para este programa.
char *xMyStack;                         // Puntero a mi stack.
#endif

char *cpu[] = {"8086", "80286", "80386", "80486", "Pentium(r)" };

//   Variables que solo usa main.
static int i, xargc;
BootList_ *s;
char *logoFile;			// Archivo con logo para la impresora.

void RenameClr(String^ fnsource, String^ fndestino)
{
	if (File::Exists(fndestino) && File::Exists(fnsource))
	{
		File::Delete(fndestino);
		File::Move(fnsource, fndestino);
	}
	else if (File::Exists(fnsource))
	{
		File::Move(fnsource, fndestino);
	}
}

//   Ejecuta aquellas funciones que se hayan definido como periodicas en la
//   tabla 'PeriodicList'.
void LoopPeriodic()
{
	PeriodicList_ *p;
	int i;

	try
	{
		//   Queda en loop para siempre.
		for( ;; )
		{
			for(p = PeriodicList, i = cPeriod; i; i--, p++)
			{
				if(p->SliceActual-- <= 0)
				{
					(*p->PeriodicFunc)();
					p->SliceActual = p->Slice;
				}
			}
			tSwitch();
		}
	}
	catch (System::Threading::ThreadAbortException^ exnada)
	{

	}
	catch (System::Exception^ excep)
	 {	
		 ClrLog::escribeExcepcion("errorperiod.txt", "LoopPeriodic", excep);
		 /*System::IO::StreamWriter^ writer = System::IO::File::AppendText("errorperiod.txt");		 
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion LoopPeriodic", System::DateTime::Now);
		 writer->WriteLine(fhdatos);
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);		 
		 writer->WriteLine("----------------------------------");
         writer->Close();*/
		 /*ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");*/		 
	 }
}

// Ejecuta el loop de sincronizacion.
void LoopSynchro()
{
	int slice = 0;
	try
	{
		for( ;; )
		{
			if(slice <= 0)
			{
				Syncronize();
				slice = 30;
			}
			slice--;
			tSwitch();
		}
	}
	catch (System::Threading::ThreadAbortException^ exnada)
	{

	}
	catch (System::Exception^ excep)
	 {	
 		 ClrLog::escribeExcepcion("errorsynchro.txt", "LoopSynchro", excep);
		 /*System::IO::StreamWriter^ writer = System::IO::File::AppendText("errorsynchro.txt");		 
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion LoopSynchro", System::DateTime::Now);
		 writer->WriteLine(fhdatos);
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
         writer->Close();*/
		 
	 }
}



void LoopSyncTicket()
{
	int slice = 0;
	LibEntidades::Alberdi::SyncWorker ^syncTicket = gcnew LibEntidades::Alberdi::SyncWorker();
	try
	{
		syncTicket->Iniciar();		
	}
	catch (System::Threading::ThreadAbortException^ exnada)
	{

	}
	catch (System::Exception^ excep)
	 {	
 		 ClrLog::escribeExcepcion("errorsynchro.txt", "LoopSyncTicket", excep);
		 /*System::IO::StreamWriter^ writer = System::IO::File::AppendText("errorsynchro.txt");		 
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion LoopSynchro", System::DateTime::Now);
		 writer->WriteLine(fhdatos);
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
         writer->Close();*/
		 
	 }
}

//  Cuando el pos se inicia con la opcion /prninit, trata de enviar un comando de inicializacion
//  al pos fiscal.
void InicioFiscal()
{
	char *fis_Cuit, *fis_Nombre, *fis_Registro, *fis_Fecha, *fis_PtoVta,
		 *fis_Inicio, *fis_IBrut, *fis_Respon;
	char *FISCAL = "fiscal";

	if(!OpenIni("fiscal"))
	{
		fis_Cuit     = Trim(ExtractIniStr(FISCAL, "cuit")); 
		fis_Nombre   = Trim(ExtractIniStr(FISCAL, "nombre"));
		fis_Registro = Trim(ExtractIniStr(FISCAL, "registro"));
		fis_Fecha    = Trim(ExtractIniStr(FISCAL, "fecha"));
		fis_PtoVta   = Trim(ExtractIniStr(FISCAL, "ptovta"));
		fis_Inicio   = Trim(ExtractIniStr(FISCAL, "iniact"));
		fis_IBrut    = Trim(ExtractIniStr(FISCAL, "ibrut"));
		fis_Respon   = Trim(ExtractIniStr(FISCAL, "respon"));
		pf->FiscalInit(
			fis_Cuit, 
			fis_Nombre, 
			fis_Registro, 
			fis_Fecha, 
			fis_PtoVta, 
			fis_Inicio,
			fis_IBrut, 
			fis_Respon);
		CloseIni();
	}
}

#include "Evaluator.h"

//  ****************************** MAIN **********************************
int reqStat(int);
void pos_main(int argc, char *argv[])
{
	EnLetras(147289374.33, "x");
	EnLetras(1, "X");

	printf("Sistema de Registracion Computarizado (POS).\n");
    printf("Version 3.1 - Copyright (c) 1996-2014 * Alberdi SA.\n\n");
	printf("Inicializando Servicios y Cargando Archivos:\n\n");
	memmove(prnesq, initesq, sizeof(prnesq));

    //  Inicializa el manejador de DPMI
    #ifdef _UTILIZE_DPMI_
    InitializeDPMI();
    #endif

    //  Las variables que el compilador busca a partir del EBP, van a parar a  cualquier parte...
    xargc = argc;

	//   Inicializa TODOS los modulos que lo requieran.
	Inicializando = 1;
    BootLogInit();

	for(s = BootList, i = cInit; i; i--, s++)
	{
		if(s->InitFunc)
		{
			//   Imprime un mensaje del progreso del arranque.
			if(wSupEnabled)
			 {
				if(!iHand)
					iHand = wDefine(7, 10, 18, 70, BORDERSINGLE, SNGCURS, initesq, " Inicializando ", "");

				wActivate(iHand);
				wScroll();
				wLocate(9, 1);
				wPstr(s->InitMsg);
			 }
			 else
				printf("\t%s - ", s->InitMsg);
            WriteBootLog(s->InitMsg, BM_INICIANDO);

			//   Llama a la funcion de inicializacion.
			if((s->Initialized = (*s->InitFunc)()) == 2)
				break;

			if(wSupEnabled)
				wPstr(" - Ok.");
			 else
				printf(" Ok.\n");
            WriteBootLog("OK", BM_FINALIZANDO);
		} else
			s->Initialized = 1;
	}

	StackProbe();
	wRelease(iHand);
	Inicializando = 0;

	//   Inicializacion Adicional
    WriteBootLog("Definiendo Evento de Scanner", BM_MSG);
	defineEvent(E_SERIAL, Scann);
	chgFactuMode(defFactuMode);
    WriteBootLog("OK", BM_FINALIZANDO);
	WriteBootLog("Cargando sonido para 'beep'", BM_MSG);
	LoadWav();
	WriteBootLog("OK", BM_FINALIZANDO);

	//   Si se indico como opcion, inicializar el impresor!!!
	if(argc == 2 && _stricmp(argv[1], "PRNINIT") == 0)
		InicioFiscal();

	//   Revisa el comando ZETA de la linea de ORDEN.
	ForzarZeta = 0;
	if((argc == 2 && _stricmp(argv[1], "ZETA") == 0))
		ForzarZeta = 1;

	// Fuerza una consulta de estado.
	pf->CheckStatus();
	if(FiscalForceZeta)
	{
		WriteBootLog("Se emite ZETA porque la impresora esta bloqueada");
		pf->PrintZeta();
	}

	//   Modulo principal.
	if(!i)
	{
		// Controla la licencia del modulo.
		if(pf->PrinterCap | PRN_IDENTIFY)
		{
			char cuit[20], pv[10], *tmp, *tmp2;

			pf->Owner(cuit, pv);
			Lic *lic = new Lic();
			tmp = Decrypt(licFile);
			bool res = lic->LoadLic(tmp, BITNET_SALT);
			DecryptClear(tmp);
			if(res)
			{
				res = lic->ValidatePos(cuit, pv, fecha());
				if(!res)
				{
					tmp = Decrypt(licError);
					sprintf(msg, tmp, cuit, pv);
					DecryptClear(tmp);
					WriteBootLog(msg);
					memset(msg, 0, strlen(msg));
					pf = (PrinterFiscal_ *) new PrinterDummy();// &P_DUMMY;
				}
			}
			else
			{
				tmp = Decrypt(licFileError);
				tmp2 = Decrypt(licError);
				sprintf(msg, tmp2, tmp, tmp);
				DecryptClear(tmp);
				DecryptClear(tmp2);
				WriteBootLog(msg);
				memset(msg, 0, strlen(msg));
				pf = (PrinterFiscal_ *) new PrinterDummy();// &P_DUMMY;
			}

			// Libera memoria usada y elimina rastros de la operacion efectuada.
			delete lic;
			memset(cuit, 0, 20);
			memset(pv, 0, 10);
		}

		// Si el impresor soporta logos
		/*if(pf->PrinterCap | PRN_CANLOGO)
		{
			sprintf(tmsg, "img\\%s", logoFile);
			if(Existe(tmsg))
			{
				bool actualizar = false;
				struct tm tmLogo = *LastAccessFile(tmsg);
				ReplaceExtension(tmsg, ".ctl", true);
				if(!Existe(tmsg))
					actualizar = true;
				else
				{
					struct tm *tmCtl = LastAccessFile(tmsg);
					if(ComparaTm(&tmLogo, tmCtl) > 0)
						actualizar = true;
				}

				if(actualizar)
				{
					sprintf(tmsg, "img\\%s", logoFile);
					pf->InstallLogo(tmsg);
					ReplaceExtension(tmsg, ".ctl", true);

					// Crea el archivo de control para no recargar el logo.
					remove(tmsg);
					int h = creat(tmsg, S_IREAD | S_IWRITE);
					write(h, "x", 1);
					close(h);
				}
			}
		}
		*/

		//  Cierra las operaciones de arranque del pos.
		#ifdef POSDEBUG
        WriteBootLog("POSDEBUG - Se activaron las siguientes funciones:", BM_MSGZ);
		sprintf(msg, "DBGRxPlus=%d, DBGCopyFile=%d, DBGActiPlus=%d",
			    DBGRxPlus, DBGCopyFile, DBGActiPlus);
		WriteBootLog(msg, BM_MSGZ);
		#endif
		BootLogClose();

		//   Lazo principal. Solo sale para finalizar el POS.
		fork(LoopPeriodic,  "Periodic Shell");
		fork(LoopSynchro, "Sincronizacion Remota");
		fork(LoopSyncTicket, "Sincronizacion de tickets");

		// Recupera ticket pendiente.
		WriteBootLog("Recuperando ticket pendiente, desde corte de corriente", BM_INICIANDO);
		if(prndelay) 
			globalDelayPrint = 1;
		ReprintDump(true);

		WriteBootLog("OK", BM_FINALIZANDO);
		DispAdic(0, 1, saludos[2]);
		///Para llamar a la modificacion de estructura de BD
		if (tjPrisma)
			Alberdi::TJOCommon::Prisma::AccesoDb::ModificacionEstructura();

		// Marca la finalizacion de la inicializacion y el comienzo de las funciones normales
		startMark = time__();

#ifndef POSXP
		for(flag = 1; flag; )
		{
			StackProbe();
			tSwitch();
		}
	}
	POSExit();
#else
	}
#endif
}

#ifdef POSXP

// xfile.cpp
extern int flushThreadControl;
#define FT_ENDREQ   3

#endif

//   Finaliza con el POS.
void POSExit()
{
	int i;

	BootLogExit();
	for(s = &BootList[(i=cInit) - 1]; i; i--, s--)
	{
		printf("%3d\r", i - 1);
		if(s->Initialized == 1 && s->ResetFunc)
			(*s->ResetFunc)();
		StackProbe();
	}

#ifdef POSXP

	// Detiene la thread que vacia buffers a disco
	flushThreadControl = FT_ENDREQ;

#endif

    FCls();
	exit(errorCode);
}

//  Imprime un mensaje de error y aborta.
#ifndef _DEBUG
char abendmsg[80];
void AbnormalEnd(char *emsg)
{
	 static int abortando;

     BootLogExit();
     ReleaseWSup(); FCls();
	 STRCPY(abendmsg, emsg);
	 restore_safe_stack ;
	 DumpStatus("Program AbEnd");
	 if(abortando)
		printf("Mas Errores: '%s'.\n", abendmsg);
	  else
	  {
        printf("\n\n\n");
		printf("Finalizacion Abrupta.\n Error: '%s'.\n", abendmsg);
		abortando = 1;
		if(strlen(abendmsg) > 78)
			abendmsg[79] = 0;
		sprintf(msg, "AbEnd: '%s'.", abendmsg);
		WriteErr(msg);
	  }

	printf("Regresando al Sistema Operativo...\n");
	exit(1);
}
#endif

//  Rastreo del BOOT-LOG.
FILE *h;
char BOOTFILE[] = "log.pos";
char dbgStr[2000];
int start_mem;
void BootLogInit()
{
	// Obtiene la fecha de la ultima vez que se modifico el log.
	struct stat st;
	stat(BOOTFILE, &st);
	struct tm *t = gmtime(&st.st_mtime);
	//int fileTime = (t->tm_year + 1900) * 10000 + (t->tm_mon + 1) * 100 + t->tm_mday;
	System::DateTime^ fileTime = System::IO::File::GetLastWriteTime("log.pos");

	// Si la fecha no es la de hoy, entonces comienza al log de cero haciendo backup de los log	
	//if(fileTime != date())
	try
	{
	if (fileTime->Date != System::DateTime::Now.Date)
	{		
		RenameClr("log.8", "log.9");//rename("log.8", "log.9");
		RenameClr("log.7", "log.8");//rename("log.7", "log.8");
		RenameClr("log.6", "log.7");//rename("log.6", "log.7");
		RenameClr("log.5", "log.6");//rename("log.5", "log.6");
		RenameClr("log.4", "log.5");//rename("log.4", "log.5");
		RenameClr("log.3", "log.4");//rename("log.3", "log.4");
		RenameClr("log.2", "log.3");//rename("log.2", "log.3");
		RenameClr("log.1", "log.2");//rename("log.1", "log.2");
		RenameClr("log.pos", "log.1");//rename(BOOTFILE, "log.1");

		RenameClr("LogPrisma.9", "LogPrisma.10");
		RenameClr("LogPrisma.8", "LogPrisma.9");
		RenameClr("LogPrisma.7", "LogPrisma.8");
		RenameClr("LogPrisma.6", "LogPrisma.7");
		RenameClr("LogPrisma.5", "LogPrisma.6");
		RenameClr("LogPrisma.4", "LogPrisma.5");
		RenameClr("LogPrisma.3", "LogPrisma.4");
		RenameClr("LogPrisma.2", "LogPrisma.3");
		RenameClr("LogPrisma.1", "LogPrisma.2");
		RenameClr("LogPrisma.txt", "LogPrisma.1");

		RenameClr("LogSmartPoint.9", "LogSmartPoint.10");
		RenameClr("LogSmartPoint.8", "LogSmartPoint.9");
		RenameClr("LogSmartPoint.7", "LogSmartPoint.8");
		RenameClr("LogSmartPoint.6", "LogSmartPoint.7");
		RenameClr("LogSmartPoint.5", "LogSmartPoint.6");
		RenameClr("LogSmartPoint.4", "LogSmartPoint.5");
		RenameClr("LogSmartPoint.3", "LogSmartPoint.4");
		RenameClr("LogSmartPoint.2", "LogSmartPoint.3");
		RenameClr("LogSmartPoint.1", "LogSmartPoint.2");
		RenameClr("LogSmartPoint.txt", "LogSmartPoint.1");

		RenameClr("LogParking.9", "LogParking.10");
		RenameClr("LogParking.8", "LogParking.9");
		RenameClr("LogParking.7", "LogParking.8");
		RenameClr("LogParking.6", "LogParking.7");
		RenameClr("LogParking.5", "LogParking.6");
		RenameClr("LogParking.4", "LogParking.5");
		RenameClr("LogParking.3", "LogParking.4");
		RenameClr("LogParking.2", "LogParking.3");
		RenameClr("LogParking.1", "LogParking.2");
		RenameClr("LogParking.txt", "LogParking.1");

		RenameClr("LogPrismaECR.9", "LogPrismaECR.10");
		RenameClr("LogPrismaECR.8", "LogPrismaECR.9");
		RenameClr("LogPrismaECR.7", "LogPrismaECR.8");
		RenameClr("LogPrismaECR.6", "LogPrismaECR.7");
		RenameClr("LogPrismaECR.5", "LogPrismaECR.6");
		RenameClr("LogPrismaECR.4", "LogPrismaECR.5");
		RenameClr("LogPrismaECR.3", "LogPrismaECR.4");
		RenameClr("LogPrismaECR.2", "LogPrismaECR.3");
		RenameClr("LogPrismaECR.1", "LogPrismaECR.2");
		RenameClr("LogPrismaECR.txt", "LogPrismaECR.1");

		RenameClr("LogAPIVentas.9", "LogAPIVentas.10");
		RenameClr("LogAPIVentas.8", "LogAPIVentas.9");
		RenameClr("LogAPIVentas.7", "LogAPIVentas.8");
		RenameClr("LogAPIVentas.6", "LogAPIVentas.7");
		RenameClr("LogAPIVentas.5", "LogAPIVentas.6");
		RenameClr("LogAPIVentas.4", "LogAPIVentas.5");
		RenameClr("LogAPIVentas.3", "LogAPIVentas.4");
		RenameClr("LogAPIVentas.2", "LogAPIVentas.3");
		RenameClr("LogAPIVentas.1", "LogAPIVentas.2");
		RenameClr("LogAPIVentas.txt", "LogAPIVentas.1");

		RenameClr("LogQrCashout.9", "LogQrCashout.10");
		RenameClr("LogQrCashout.8", "LogQrCashout.9");
		RenameClr("LogQrCashout.7", "LogQrCashout.8");
		RenameClr("LogQrCashout.6", "LogQrCashout.7");
		RenameClr("LogQrCashout.5", "LogQrCashout.6");
		RenameClr("LogQrCashout.4", "LogQrCashout.5");
		RenameClr("LogQrCashout.3", "LogQrCashout.4");
		RenameClr("LogQrCashout.2", "LogQrCashout.3");
		RenameClr("LogQrCashout.1", "LogQrCashout.2");
		RenameClr("LogQrCashout.txt", "LogQrCashout.1");

		h = fopen(BOOTFILE, "w");
	    if(!h) return;
	    fprintf(h, "LOG del POS BITNET %s.\n", posVersion);
		fprintf(h, "FECHA=%d\n", date());
	    fprintf(h, "======================================\n\n");
	    fclose(h);
		 return;
	}
	h = fopen(BOOTFILE, "a");
	//fprintf(h, "Iniciando - Fecha del archivo = %d, fecha actual = %d\n", fileTime, date());
	fclose(h);
	}
	catch (System::Exception^ excep)
	 {		 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion BootLogInit", System::DateTime::Now);
		 writer->WriteLine(fhdatos);		
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 //System::String^ vmpago = System::String::Format("nro={0}, importe={1}, vuelto={2}, uVuelto={3}, uPend={4}, uOnline={5}, mtodesc={6}", mpg->nro, gcnew System::String(mpg->importe), gcnew System::String(mpg->vuelto), (int)mpg->uVuelto, (int)mpg->uPend, (int)mpg->uOnline, gcnew System::String(mpg->mtodesc));
		 //writer->WriteLine(vmpago);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");
		 POSExit();
	 }
}

void BootLogExit()
{
    h = fopen(BOOTFILE, "a");
    fprintf(h, "-------------------------------------------------\n");
	#ifndef __VSC__
    #ifdef MODEL_FLAT
    fprintf(h, "Finalizando el POS. Memoria total ocupada: %d\n", start_mem - _memavl());
    #endif
	#endif
    fclose(h);
}

void BootLogClose()
{
    h = fopen(BOOTFILE, "a");
    fprintf(h, "-------------------------------------------------\n");
#ifndef __VSC__
#ifdef MODEL_FLAT
    fprintf(h, "Memoria consumida para arrancar: %d\n", start_mem - _memavl());
#endif
#endif
    fprintf(h, "Continua con las funciones normales...\n");
    fclose(h);
}

// Buffer para el mensaje a pasarle al log.
char logMsg[1000];

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

void WLog(char *msg, ...)
{
	va_list argList;
	va_start(argList, msg);
	vsprintf(logMsg, msg, argList);
	WriteBootLog(logMsg, BM_MSGZ);
}

void WriteBootLog(char *msg)
{
	WriteBootLog(msg, BM_MSGZ);
}

void WriteBootLog(char *msg, int flag)
{
    h = fopen(BOOTFILE, "a");
    if(!h)
    {
        AbnormalEnd("No puedo registrar en BOOT.LOG");
        return;
    }

    switch(flag)
    {
        case BM_INICIANDO:
             fprintf(h, "Iniciando: '%s'\n", msg);
             break;
        case BM_FINALIZANDO:
             fprintf(h, "Finalizado. Estado resultado: '%s'.\n", msg);
			 #ifndef __VSC__
             #ifdef MODEL_FLAT
             fprintf(h, "coreleft: %d, memavl: %d\n\n", _x386_coreleft(), _memavl());
             #endif
			 #endif
             break;
        case BM_MSG:
             fprintf(h, "Ejecutando funcion: '%s'\n\n", msg);
             break;
		case BM_MSGZ:
			 fprintf(h, "%s-%s= %s\n", fecha(), hora(), msg);
			 break;
    }
    fclose(h);
}

// Optimiza el codigo
#ifdef POSXP
#pragma managed(pop)
#endif


//  Cuando se compila en modo consola, la siguiente funcion
//  llama al pos_main. Sino, el pos_main es llamado desde winsup.cpp.
#ifdef _CONSOLE
#ifdef POSXP

#include "GCStatic.h"

static array<String ^> ^CreateStringArray(const char *data)
{
	List<String ^> ^res = gcnew List<String ^>();
	StringBuilder ^sb = gcnew StringBuilder();
	while(true)
	{
		sb->Length = 0;
		while(*data && (*data != '%'))
			sb->Append((wchar_t)*data++);
		data++;
		if(sb->Length == 0) break;
		res->Add(sb->ToString());
	}
	return res->ToArray();
}

/// <summary>
/// Establece la configuracion regional utilizada por la aplicacion.
/// </summary>
static void PonerConfiguracionRegional()
{
	CultureInfo ^cInfo = (CultureInfo ^)CultureInfo::GetCultureInfo("es-es")->Clone();
    cInfo->NumberFormat->CurrencyDecimalSeparator = ".";
    cInfo->NumberFormat->CurrencyGroupSeparator = ",";
    cInfo->NumberFormat->CurrencySymbol = "$";
    cInfo->NumberFormat->CurrencyNegativePattern = 2;
    cInfo->NumberFormat->CurrencyPositivePattern = 2;

    cInfo->NumberFormat->NumberDecimalSeparator = ".";
    cInfo->NumberFormat->NumberGroupSeparator = ",";
    cInfo->NumberFormat->NumberNegativePattern = 2;

    cInfo->NumberFormat->PercentDecimalDigits = 2;
    cInfo->NumberFormat->PercentDecimalSeparator = ".";
    cInfo->NumberFormat->PercentGroupSeparator = ",";

    cInfo->DateTimeFormat->AbbreviatedDayNames = CreateStringArray("Do%Lu%Ma%Mi%Ju%Vi%Sa%");
    cInfo->DateTimeFormat->AbbreviatedMonthNames = CreateStringArray("Ene%Feb%Mar%Abr%May%Jun%Jul%Ago%Set%Oct%Nov%Dic% ");
	cInfo->DateTimeFormat->CalendarWeekRule = CalendarWeekRule::FirstDay;
    cInfo->DateTimeFormat->DateSeparator = "/";
    cInfo->DateTimeFormat->DayNames = CreateStringArray("Domingo%Lunes%Martes%Miércoles%Jueves%Viernes%Sábado");
	cInfo->DateTimeFormat->FirstDayOfWeek = DayOfWeek::Sunday;
    cInfo->DateTimeFormat->FullDateTimePattern = "dddd, dd MMMM yyyy HH:mm:ss";
    cInfo->DateTimeFormat->LongDatePattern = "dddd, dd MMMM yyyy";
    cInfo->DateTimeFormat->LongTimePattern = "HH:mm:ss";
    cInfo->DateTimeFormat->MonthDayPattern = "MMMM dd";
    cInfo->DateTimeFormat->MonthNames = CreateStringArray("Enero%Febrero%Marzo%Abril%Mayo%Junio%Julio%Agosto%Setiembre%Octubre%Noviembre%Diciembre% ");
    cInfo->DateTimeFormat->ShortDatePattern = "dd/MM/yyyy";
    cInfo->DateTimeFormat->ShortestDayNames = CreateStringArray("Do%Lu%Ma%Mi%Ju%Vi%Sa%");
    cInfo->DateTimeFormat->ShortTimePattern = "HH:mm";
    cInfo->DateTimeFormat->TimeSeparator = ":";

	Thread::CurrentThread->CurrentUICulture = cInfo;
	Thread::CurrentThread->CurrentCulture = cInfo;
	Dump::cInfo = cInfo;
}

// Arranque para .NET
char titBuff[200];

// Captura de excepciones NO controladas que no pasan por el try/catch del hilo principal:
// - AppDomain::UnhandledException: dispara para cualquier hilo (tareas 'fork', eventos serie
//   SerialPort.DataReceived, finalizadores) justo antes de que el proceso termine. En .NET 2.0
//   tambien recibe las violaciones de acceso nativas como excepcion administrada.
// - Application::ThreadException: excepciones en el lazo de mensajes de la UI.
// Ambas vuelcan el detalle a crash.dmp con el mismo formato que usa main(), para poder
// diagnosticar caidas aleatorias que hoy son "mudas".
ref class CrashLogger
{
public:
	static void Dump(System::String ^origen, System::Object ^exObj)
	{
		try
		{
			System::IO::StreamWriter^ sw = System::IO::File::AppendText("crash.dmp");
			sw->WriteLine(System::String::Format("Fecha: {0} ----------------- [{1}]", System::DateTime::Now, origen));
			System::Exception ^ex = dynamic_cast<System::Exception ^>(exObj);
			if(ex != nullptr)
			{
				sw->WriteLine(ex->Message);
				sw->WriteLine(ex->StackTrace);
				for(System::Exception ^inner = ex->InnerException; inner != nullptr; inner = inner->InnerException)
				{
					sw->WriteLine("-- Inner --");
					sw->WriteLine(inner->Message);
					sw->WriteLine(inner->StackTrace);
				}
			}
			else
			{
				sw->WriteLine(exObj != nullptr ? exObj->ToString() : "Excepcion no-CLS (ExceptionObject nulo)");
			}
			sw->WriteLine("-- Fin del Dump --------------------------");
			sw->Close();
		}
		catch(...) { /* nunca dejar que el propio logger tire una excepcion */ }
	}

	static void OnUnhandledException(System::Object ^sender, System::UnhandledExceptionEventArgs ^e)
	{
		Dump(e->IsTerminating ? "AppDomain.UnhandledException (TERMINA EL PROCESO)" : "AppDomain.UnhandledException", e->ExceptionObject);
	}

	static void OnThreadException(System::Object ^sender, System::Threading::ThreadExceptionEventArgs ^e)
	{
		Dump("Application.ThreadException", e->Exception);
	}
};

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Registra la captura de excepciones no controladas ANTES de cualquier otra cosa, para que
	// cubra tambien la inicializacion (pos_main) y las tareas/eventos de fondo.
	AppDomain::CurrentDomain->UnhandledException += gcnew UnhandledExceptionEventHandler(&CrashLogger::OnUnhandledException);
	Application::ThreadException += gcnew System::Threading::ThreadExceptionEventHandler(&CrashLogger::OnThreadException);

	// Asegura una configuracion regional coherente.
	PonerConfiguracionRegional();
	remove("kb.log");

	// Habilitar los efectos visuales de Windows XP antes de crear ningún control
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Pone la version del pos en un buffer no manejado
	Assembly ^assembly = Assembly::GetExecutingAssembly();
	AssemblyName ^name = assembly->GetName();
	String ^version = String::Format("{0}.{1}.{2}", name->Version->Major, name->Version->Minor, name->Version->Build);
	Strings::StringToChar("(Version " + version + ")", posVersion, 99);

	// Lee la definicion de pantalla. Todavia no cargamos opciones...
	if(!OpenIni("datgral"))
	{
		char *pantData = ExtractIni("gral", "pantalla");
		if(strlen(pantData) != 0)
			pantalla = atoi(pantData);
		else
			pantalla = 1;
	    CloseIni();
	}

	// 0 = Touch   800*600
	// 1 = Normal 1024*768
	// 2 = Touch  1024*768
	usaTouch = (pantalla == 0) || (pantalla == 2) || (pantalla == 6) || (pantalla == 7);
	selfCheckout = pantalla == 7;
	Controles::BaseDialog::TipoPantalla = pantalla;
	try
	{
		
		// Lanza el codigo principal del POS
		MainForm^ mainForm = gcnew MainForm(pantalla);
		pos_main(1, NULL);

		// Esconde la pantalla msdos donde se registro el boot
		SetConsoleTitle("POSXP - 0123456789");
		HWND conHandle = FindWindow(NULL, "POSXP - 0123456789");
		ShowWindow(conHandle, 0);
		DestroyWindow(conHandle);		

		// Crea el loop de mensajes de windows
		Application::Run(mainForm);
	}
	catch(Exception ^excep)
	{
		//FileStream ^fs = File::Open("crash.dmp", FileMode::OpenOrCreate, FileAccess::ReadWrite);
		//StreamWriter ^sw = gcnew StreamWriter(fs);
		System::IO::StreamWriter^ sw = System::IO::File::AppendText("crash.dmp");
		sw->WriteLine(String::Format("Fecha: {0} -----------------", DateTime::Now));
		/*StackTrace ^st = gcnew StackTrace(true);
		for each (StackFrame ^frame in st->GetFrames())
		{
			String ^s = frame->ToString();
			sw->WriteLine(s);
		}*/
		sw->WriteLine(excep->Message);
		sw->WriteLine(excep->StackTrace);
		sw->WriteLine("-- Fin del Dump --------------------------");
		sw->Close();
		//fs->Close();
		errorCode = 1;
	}

	// Cierra la aplicacion.
	POSExit();
	return 0;
}
#else

// Arranque para MSDOS
void main(int argc, char *argv[])
{
	pos_main(argc, argv);
}
#endif
#endif
