/*
 *   Scaner.c
 *   Manejador de lector de codigos de barra serial.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

char UltScann[100];
char scanMsg[180];
int  sc, AnularScanner, sc_AppendDV;
int  scanDebug = 0;
int  beep;
char *beepFile;

extern int habil[];
bool ignoreScan = false;
int appendDV;

// Funcion a la que esta redirigida la entrada de scanner.
void (*ScannerRedirectedFunc)(char *);

// Definido en plu.dbf
char dv(char *scan);

// El siguiente es el puerto al que se PREDEFINE que esta conectado el scanner.
void Scann(char *p)
{
	char x;

	x = p[1];
	if(ignoreScan) return;
	if(scanDebug)
	{
		sprintf(msg, "SCAN: %d (%c)", (int) x, x);
		WriteBootLog(msg);
	}

	lastact = time__();
	if((int) p[0] == COM_SCAN)              // Scanner
	{
        if(sc != 0 && (x < '0' || x > '9'))
        {
           if(AnularScanner)
		   {
			   WriteBootLog("SCanner Anulado - Transmitido NAK al scanner");
                txChar(COM_SCAN, 0x15);
		   }
            else
            {
                txChar(COM_SCAN, 6);
				/*if (sc_AppendDV == 1000)
					UltScann[sc + 1] = 0;
				else
				{*/
					//UltScann[sc - 1] = 0;
				if(sc_AppendDV)
				{
					UltScann[sc - 1] = 0;
					UltScann[sc - 1] = dv(UltScann);
				}
				UltScann[sc] = 0;
					

				//}

                ResetSSaver();
                if(!RegisterLocked)
                {
					 if(ScannerRedirectedFunc != NULL)
					 {
						 sprintf(scanMsg, "Termino de procesar Barra '%s' redirigida internamente.", UltScann);
						 (*ScannerRedirectedFunc)(UltScann);
					 }
					 else
					 {
						 sprintf(scanMsg, "Termino de procesar Barra '%s' enviada a PLU(1)", UltScann);
	                     xReg = atof_(UltScann);
						 plu(1);
					 }
					 WriteBootLog(scanMsg);
                }
            }
           sc = AnularScanner = 0;
        }
        else
           if(x >= '0' && x <= '9')
           {
                //if(sc < 15 && x >= '0' && x <= '9')
			   if(sc < 19 && x >= '0' && x <= '9')
                    UltScann[sc++] = x;
                 else
                    AnularScanner = 1;
           }
        return;
	}
}

//   La siguiente funcion queda corriendo en background, esperando
//   que se complete un codigo de scanner para ingresarlo al teclado.
void ScannerBackground(void)
{
	char args[50];
	try
	{
		 for( ;; )
		 {
			  reviseSerial();
			if(ScannerRedirectedFunc != NULL)
				if(PeekEvent(E_SERIAL, args))
					Scann(args);
			  tSwitch();
		 }
	}
	catch (...)
	 {		 
		 WriteBootLog("ERROR - en funcion ScannerBackground");		 
	 }
}

// Vacia todo posible dato recibido del scanner.
void FlushScanner()
{
	if(habil[COM_SCAN])
	{
		while(rxPoll(COM_SCAN))
		{
			rxChar(COM_SCAN);
			tSwitch();
		}
	}
	sc = 0;
	FlushEvents();
}

// Ejecuta un archivo de sonido
#ifdef POSXP
#pragma managed(on)

void LoadWav()
{
	// Ningun error al cargar 'ding.wav' debe detener al programa. Simplemente, no se debe reproducir el 'ding'
	try
	{
		// Crea e inicializa el soundplayer a usar.
		SoundPlayer ^sp = gcnew SoundPlayer();
		sp->Stream = System::IO::File::OpenRead(Strings::CharToString(beepFile));
		sp->Load();
		StaticAlerts::mp = sp;

		// El SoundPlayer termina la inicializacion al hacer la primera ejecucion
		PlayWav();
	}
	catch(...) {}
}

void PlayWav()
{
	if(StaticAlerts::mp != nullptr)
		StaticAlerts::mp->Play();
		
}
#pragma managed(off)
#else

void LoadWav()
{
}

void PlayWav()
{
}

#endif

// Envia un BELL al scanner para producir un sonido.
void BeepScanner()
{
	switch(beep)
	{
		case 1:
			txChar(COM_SCAN, 7);	// BELL
			break;

		case 2:
			PlayWav();
			break;
	}
}

//   Inicializa el manejo del scanner. Deja residente una porcion de ella
//   que se encarga de atender los eventos por ingreso de puerto serial
//   que se produzcan.
int InitScanner()
{
	ScannerRedirectedFunc = NULL;
	defineEvent(E_SERIAL, Scann);
    fork(ScannerBackground, "SCANNER_ATT");
    return 1;
}

