/*
 **   Kbd.c
 **   Manejador del teclado. Coloca en la queue de eventos el codigo
 **   de funcion y subfuncion de la tecla deprimida, si estuviere definida.
*/

#include <stdafx.h>

#ifdef POSXP
#include "StaticDump.h"
#include "AlertForm.h"

// Habilita dump de teclas.
bool kbDump = true;
#endif

void Cerrar(void);

void SetKbMutex() { GCStatic::LockKeyboard("kbdhandler"); }
void ReleaseKbMutex() { GCStatic::ReleaseKeyboard("kbdhandler"); }


// #define LOGK
#ifdef _MSC_VER_

#if _MSC_VER >= 1000
	#include <windows.h>
	#include <wincon.h>				// Funciones de consola.
	#define _W98_					// Para el resto del programa.	
	HANDLE hConIn;
#endif
#endif

unsigned ENDKEY;
unsigned managed_kbuff[1000];		// Buffer para teclas
int kbuff_in = 0, kbuff_out = 0;
int UsaLlave = 0, transform[200];
char *KEYBOARD;

/*   Prototipos   */
int initKbd(void);
void kbattn(char *);
#ifdef __SC__
extern "C" {
int _kbhit(void);
}
#endif

/*   Datos globales   */
struct kd {
	unsigned scan;      // Scan del teclado
	int func;           // Funcion principal
	int subfunc;        // SubFuncion.
	char *chklevel;		// Permiso para utilizar la funcion.
	double xReg;		// Registro X.
	double yReg;		// Registro Y.
	int cntMacro;		// Flag para indicar macro y orden de ejecuc.
	char *wTitle;		// Titulo de la ventana que debe estar activa.
} kdef[500];

volatile int kcount, LastCh, ClearKey;
volatile int kbuff[30], kSp;

void CheckKbd(void);

//   Prototipos de las funciones de teclado.
void digitos(int);
void clear(int);
void decpt(int);
void dblzr(int);
void depto(int);
void multipli(int);
void subtot(int);
void mpago(int);
void plu(int);
void chgmode(int);
void corrige(int);
void corriall(int);
void consulta(int);
void factura(int);
void misc(int);
void clerl(int);
void envas(int);
void vendedor(int);
void suspende(int);
void programa(int);
void otroprec(int);
void buscaplu(int);
void recupera(int);
void viewmap(int);
void reparto(int);
void gaveta(int);
void tickfac(int);
void cltonl(int);
void cobracc(int);
void cobro(int);
void superv(int);
void mandacob(int);
void cell(int);
void recupera_sql(int);
void pesar_plu(int);
void menu_vacio1(int);//agregado para reemplazar itrackq
void ing_codpromo(int);

void infCajaLibre(int);
void ObtenerPedJanis(int cual);
void IngVendedorMayor(int cual);

void IngIdParking(int cual);

//  Tabla de las funciones de teclado. Todo el codigo principal de las  funciones reemplazables debe ser declarado en esta tabla. 
//  Antes, se prototipearon estas funciones.

void (*kFunc[])(int) = {
	digitos, clear, decpt, dblzr, multipli, depto, subtot, mpago, plu,
	chgmode, corrige, corriall, consulta, tickfac, misc, clerk, envas,
	vendedor, suspende, programa, otroprec, buscaplu, recupera, viewmap,
	reparto, gaveta, cltonl, cobracc, cobro, superv, mandacob, recupera_sql, 
	pesar_plu, menu_vacio1, ing_codpromo, infCajaLibre, ObtenerPedJanis, IngVendedorMayor, IngIdParking, cell,
	
	factura		// OJO - 'factura' debe ser la ultima 
	//  (Porque ya no se usa, dado que quedo obsoleta con el ticket factura...)
};

#define CFUNC (sizeof(kFunc) / sizeof(void (*)()))

void gaveta_(int);
void gaveta(int cual)
{
	GCStatic::LockMainLoop("gaveta");
	gaveta_(cual);
	GCStatic::ReleaseLockMainLoop("gaveta");
}

//   La siguiente funcion simplemente mapea a OpenDrwr.
void gaveta_(int dummy)
{
	if(chkClk()) return;				// Hay cajero habilitado?
	pf->OpenDrwr();
	if(supervNro != 0) superv(0);
}

//   Pide al driver de teclado el scan de una tecla.
extern int flag;          // Para DEBUG. Permite finalizar a 'POS.EXE'
int wcl;

// Si hay tecla bufferizada, la quita del buffer y la retorna. Sino, retorna 0 indicando que no hay tecla bufferizada
unsigned GetBufferedKey()
{
	if(kSp)
	{
		// Remueve una tecla del buffer
		unsigned lastCh = kbuff[0];
		kSp--;
		for(int i = 0; i < kSp; i++)
			kbuff[i] = kbuff[i + 1];
		ReleaseKbMutex();
		lastact = time__();
		return lastCh;
	}
	return 0;
}

#ifdef POSXP

// Driver para WinXP
unsigned xgetch(void)
{
	unsigned lastCh;

	// Queda esperando teclas, para siempre
	while(true)
	{
		// Si hay teclas en el buffer, entonces emite primero esas teclas antes de consultar el teclado.
		SetKbMutex();
		lastCh = GetBufferedKey();

		// Si no habia tecla bufferizada, entonces espera que entren teclas desde el driver.
		if(lastCh == 0 && kbuff_in != kbuff_out)
		{
			// Quita nuestra tecla.
			lastCh = managed_kbuff[kbuff_out];
			kbuff_out++;
			if(kbuff_out >= 900) kbuff_out = 0;
		}

		// No necesitamos mas la exclusividad
		ReleaseKbMutex();

		// Si se pulso tecla, retorna.
		if(lastCh > 0)
		{
			lastact = time__();
			return lastCh;
		}

		// No hay tecla disponible. Hace otras cosas...
		tSwitch();
	}
}

#else

// Driver para MSDOS
#ifdef _W98_
static INPUT_RECORD kbuff;
#endif
unsigned xgetch(void)
{
	while(true)
	{
		// En uso exclusivo, mira si hay tecla disponible.
		SetKbMutex();
		unsigned cha = GetBufferedKey();
		if(cha == 0 && KbHit())
		{
			cha = _getch();
			if(!cha || cha == 0xe0)
				cha = _getch() * 256;
			if((char) cha >= 'a' && (char) cha <= 'z')
				cha = cha - 'a' + 'A';
		}
		ReleaseKbMutex();

		// Si hubiera detectado tecla, la retorna.
		if(cha != 0)
		{
			ResetSSaver();
			lastact = time__();
			if(cha == 248) return xgetch();
			return cha;
		}
	}
}
#endif

// Indica si hay alguna tecla pulsada.
int KbHit()
{
#ifdef POSXP
	int result = (kbuff_in != kbuff_out);
	return result;
#else
	return _kbhit();
#endif
}

bool waitQ;

void putkey(unsigned code)
{
	// Activa el mutex
	SetKbMutex();

	// Agrega la tecla y retorna
	bool addKey = true;
	if(waitQ && code == 81)
	{
		rebootear = 0;
		errorCode = 0;
		waitQ = addKey = false;
		ReleaseKbMutex();
		Cerrar();					// No deberia volver de 'Cerrar()'...
	}
	else if(code == keyout)// || code == 15)
	{
		waitQ = true;
		addKey = false;
	}

	if(addKey)
	{
		if(code < 65000) waitQ = false;
		managed_kbuff[kbuff_in] = code;
		kbuff_in++;		// OJO con querer optimizarlo. No funcionara....
		if(kbuff_in >= 900) kbuff_in = 0;
	}
	ReleaseKbMutex();
}

//   Ejecuta una macro que consiste en un 'keyscan'.
//   Debe estar activa la ventana indicada en *wTit, para que se emita la macro. Si *wTit es 0, entonces el keyscan se
//   emite siempre.
void M_keyscan(int kscan, char *wTit)
{
	kbattn((char *) &kscan);
}

//   Ejecuta la funcion solicitada.
void ExecuteFunc(kd *k)
{
	kd *kk;
	int i, j;

	lastact = time__();
	if(fabs(xReg) < 0.001) xReg = 0;
	if(fabs(yReg) < 0.001) yReg = 0;
	if(k->cntMacro)
	{
		if(!k->wTitle || !strlen(k->wTitle) || !strcmp(wTitle(), k->wTitle))
		{
			for(j=1; ; j++)
			{
				for(kk=kdef, i=kcount; i; i--, kk++)
				{
					if((kk->scan == k->scan) && (kk->cntMacro == j))
					{
						if(fabs(kk->xReg) > 0.001)
							xReg = kk->xReg;
						if(fabs(kk->yReg) > 0.001)
							yReg = kk->yReg;
						if(kk->func <= CFUNC)
							(*kFunc[kk->func - 1])(kk->subfunc);
						else if(kk->func == 9998)
							M_keyscan(kk->subfunc, kk->wTitle);
						break;
					}
				}
				if(!i) break;
			}
		}
	}
	else
	{
		if(k->func == 9998)
		{
			if(!k->wTitle || !strcmp(wTitle(), k->wTitle))
				M_keyscan(k->subfunc, k->wTitle);
		}
		else if(k->func <= CFUNC)
			(*kFunc[k->func - 1])(k->subfunc);
	}
}

//   La siguiente funcion es llamada por el despachador de eventos, cada vez que se detecta un evento de tecla pulsada.
void kbattn(char *p)
{
	 unsigned  cha, i;
	 struct kd *k;

	 cha = *((int *) p);

	 lastact = time__();
	 if(RegisterLocked) return;
	 if(ViewChMode(cha)) return;			// Si era la llave de modo
	 if(FiscalForceZeta && (rMode != 3))
	 {
		 Alert("No puede continuar hasta que efectue", "la ZETA que requiere el impresor !!!");
		 return;
	 }

	 char kmsg[200];
	 sprintf(kmsg, "Tecla: %u", cha);
	 WriteBootLog(kmsg);

	 for(i = kcount, k = kdef; i; i--, k++)
	 {
		if(k->scan == cha)
		{
#ifdef POSXP
			Pos::Desktop::KbdLogger::Log(String::Format("** Detectada: SCAN={0}, FUNC={1}, SUBFUNC={2}\n", k->scan, k->func, k->subfunc));
#endif
			if(borrar) clear(1);			// Borra las pantallas.
			if(strlen(k->chklevel) != 0)
			{
				//   La funcion requiere un 'password' antes de poder ser utilizada. (o sino modo X1).
				int ok = 0;
				if(strcmp(k->chklevel, "1") == 0)
					ok = !RequestSuperv();
				else
					ok = RequestPasswd(k->chklevel);
				if(ok) ExecuteFunc(k);
			}
			else
				ExecuteFunc(k);
			break;
		}
	 }

	//  Si no encontro tecla de funcion asociada, puede tratarse
	//  de una de las teclas de manejo remoto del browse.
#ifdef POSXP
	if(!i)
		//Dump::Browse->RemoteKey(cha);
		Dump::cTicket->RemoteKey(cha);
#else
	if(!i)
		bDet.xBrowseRemoteKey(cha);
#endif
	return;
}

//   Espera se pulse cualquier tecla y retorna el 'keystrok' que le corresponda a la misma.
int GetTransformedKey()
{
	int cha = GetKey();
	int res = cha;
	if(cha == transform[10]) res = '.';
	if(cha == transform[11])
	{
		kbuff[0] = '0';
		kSp = 1;
		res = '0';
	}
	for(int i=0; i<10; i++)
	{
		if(cha == transform[i])
		{
			res = i + '0';
		}
	}

#ifdef LOGK
	sprintf(msg, "GETTRKEY: cha=%d, res=%d (%c)", cha, res, (char)res);
	WriteBootLog(msg);
#endif
	return res;
}

int GetKey()
{
	struct kd *k, *kk;
	int i, j, l;

	// Obtiene una tecla del teclado o de los buffers
	LastCh = xgetch();

	//   Dado que esta funcion es utilizada por aquellas que no utilizan el esquema de teclas por eventos, entonces verifico
	//   si el scancode de la tecla pulsada se corresponde con alguna macro. De ser asi, EJECUTA LA MACRO.
    for(l = kcount, k = kdef; l; l--, k++)
	{
		if(k->scan == (unsigned) LastCh && k->cntMacro)
		{
			if(!k->wTitle || !strlen(k->wTitle) || !strcmp(wTitle(), k->wTitle))
			{
				for(j = 1; ; j++)
				{
					for(kk = kdef, i = kcount; i; i--, kk++)
					{
						if((kk->scan == k->scan) && 
						   (kk->cntMacro == j) &&
						   (kk->func == 9998))
						{
							SetKbMutex();
							kbuff[kSp++] = kk->subfunc;
							ReleaseKbMutex();
							break;
						}
					}
					if(!i) break;
				}

				// Hemos detectado una macro y hemos agregado a la misma en el buffer. Entonces, no corresponde que regresemos
				// la tecla que nos devolvio el driver del teclado, sino la macro en si misma.
				return xgetch();
			}
		}
	}

	// Retorna la tecla devuelta por el driver del teclado.
	return LastCh;
}

//  Queda haciendo polling del teclado. Cuando detecta la pulsaci¢n de una tecla, entonces genera un evento para que, 
//  cuando llegue el momento, se atienda.
void newkbd(void)
{
	try
	{
		while(true)
		{
			// Utiliza el driver fisico para ver si hay tecla y, si la hay, poner su codigo en la queue de eventos
			  if(kSp != 0 || KbHit())
			{
				int pCode = xgetch();
				addEvent(E_KEYB, (char *) &pCode, sizeof(int));
			}

			// Va a otra tarea
			tSwitch();
		}
	}
	catch (System::Threading::ThreadAbortException^ exnada)
	{

	}
	catch (System::Exception^ excep)
	 {			 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errorkeyb.txt");		 
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion newkbd", System::DateTime::Now);
		 writer->WriteLine(fhdatos);
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 WriteBootLog("AVISO - PARA DIEGO");
		 /*ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");*/		 
	 }
}

// Vacia todos los eventos pendientes.
void FlushAll()
{
	// Vacia buffer de scanner
	FlushScanner();

	// Vacia buffer de teclado
	SetKbMutex();
	kbuff_out = kbuff_in;
	kSp = 0;
	ReleaseKbMutex();

	// Vacia buffer de eventos
	FlushEvents();
}

// Inicializacion del manejador de teclado.
// Desvia la interrupcion de teclado para que la pulsacion de una tecla
// genere un evento.
// Carga la definicion de teclas posibles y sus funciones/subfunciones.

int initKbd()
{
	 kd *k;

     if(KEYBOARD && !StartFile(KEYBOARD, 'k'))
	 {
		for(int i=0; i<12; i++) transform[i] = 0;
		while(GetPLine())
		{
            k=&kdef[kcount];   
            k->scan = atoi(GetPToken());
            k->func = atoi(GetPToken());
            if(k->func == 2)
				ClearKey = k->scan;
			if(k->func != 0)
			{
				if(k->func != 9999)
				{
					//   Tecla normal.
					k->subfunc = atoi(GetPToken());
					k->chklevel = STRDUP(GetPToken(), "InitKbdDUP");

					if(k->func == 1 && k->subfunc < 10)
						transform[k->subfunc] = k->scan;
					else if(k->func == 3)
						transform[10] = k->scan;
					else if(k->func == 4)
						transform[11] = k->scan;
				}
				else
				{
					//   Secuencia de macro.
					kd *kk;
					int i, j;
					char *wt;

					k->chklevel = STRDUP(GetPToken(), "InitKbdDUP2");
					k->func = atoi(GetPToken());
					k->subfunc = atoi(GetPToken());
					k->xReg = atof_(GetPToken());
					k->yReg = atof_(GetPToken());
					wt = GetToken();
					if(wt) k->wTitle = _strupr(STRDUP(wt, "initKbdDUP3"));
					j = 1;
					for(kk = kdef, i = 0; i < kcount; i++, kk++)
						if((kk->scan == k->scan) && kk->cntMacro)
							j++;
					k->cntMacro = (char) j;
				}
				kcount++;
			}
			else
			    k->subfunc=0;
        }
		CloseFile('k');
     }
     else
        AbnormalEnd("No puedo cargar definicion de teclado.");

	 defineEvent(E_KEYB, kbattn);
	 lastact = time__();
     if(UsaLlave) _outp(0x60, 0xe7);                // Pide posicion de la llave.
	 fork(newkbd, "KBD_ATTN");
	 #ifdef _W98_
	 hConIn = GetStdHandle(STD_INPUT_HANDLE);
	 SetConsoleMode(hConIn, 0);
	 #endif
	 return 1;
}


///
void menu_vacio1(int cual)
{

}
