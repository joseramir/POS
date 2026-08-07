//
//   MTask.c
//   Habilita MultiTasking en el sistema. (metodo cooperativo).
//

#include <stdafx.h>

#ifdef POSXP

/*   Utiliza las funciones de threading del .NET managed, si estuviera disponible    */
#include "StaticDump.h"

using namespace System;
using namespace System::Threading;

//  Inicializa el swapeador.
//  En .NET, no hace nada.
int initSwitcher()
{
	return 1;
}

//  Ejecuta el delegado del background worker
static void runfork(Object^ state)
{
	IntPtr^ iptr = (IntPtr^) state;
	Thread::CurrentThread->CurrentCulture = Dump::cInfo;
	Thread::CurrentThread->CurrentUICulture = Dump::cInfo;
	void (*func)() = (void (*)(void)) iptr->ToPointer();

	Dump::Forked->Add(Thread::CurrentThread);
	(*func)();
	Dump::Forked->Remove(Thread::CurrentThread);
}

//  Destruye todos los hilos que esten en ejecucion.
bool inkill = false;
void killall()
{
	if(inkill) return;
	inkill = true;
	while(Dump::Forked->Count != 0)
	{
		Thread ^th = Dump::Forked[0];
		if(th != Thread::CurrentThread)
			th->Abort();
		Dump::Forked->RemoveAt(0);
	}
}

//   Habilita una nueva tarea en background. Se le debe pasar como parametro un puntero a la funcion que se 
//   arrancara en background. El codigo vuelve inmediatamente (la funcion pasada como parametro se ejecutara 
//   posteriormente), con cero si todo salio ok o con otro  valor si algo fallo y no pudo lanzar la tarea.
int fork(void (*func)(void), char *procName)
{
	if(inkill) return 0;
	WaitCallback^ wcb = gcnew WaitCallback(&runfork);
	IntPtr^ execFunc = gcnew IntPtr((int) func);
	ThreadPool::QueueUserWorkItem(wcb, execFunc);
	return 0;
}

//    Incorpora, a la lista de funciones anunciantes, una nueva. Estas son
//    funciones que DEBEN ser llamadas, ante un Task-Switch, para anunciar
//    este evento a un modulo, que depende de la tarea que este activa. (Por
//    ejemplo, un Window Support).
//    NO SE USA
void AddAnun(void (*func)(int, int))
{
}


//  Remueve una funcion de anuncio, que no se utilizara mas.
//  NO SE USA
void RemoveAnun(void (*func)(int, int))
{
}

//    La siguiente funcion es la que conmuta de una tarea a otra.
//    Debe ser llamada periodicamente para permitir la ilusion de multitarea.
void tSwitch()
{
	Thread::Sleep(10);
}

//   La siguiente funcion devuelve TRUE si detecto la utilizacion de
//   un puntero nulo, sino, devuelve FALSE.
//   NO SE USA
int nullcheck()
{
	return 0;
}

//   La siguiente rutina se utiliza para verificar que no se descarriaron los punteros al stack de la maquina.
//   NO SE USA.
void StackProbe()
{
}

//     Imprime un estado del despachador de multitareas.
void MtaskStatus()
{
	printf(".NET Managed Threading Model.");
}



#else
/*   MSDOS utiliza un modulo propio para funciones de threading   */


#define NOTUSED  0          //  Posibles valores para 'estado' en 'tsk' 
#define DORMANT  1
#define RUNNING  2
#define SUSPEND  3
#define DEADING  4
#define STARTING 5
#define EMPTY    6

#define MAXTASK 32          //  Maximas tareas simultaneas
#define STKSIZE 0x1200		//  Tamaño default del stack de cada hilo.

struct tsk {
	char     fName[32];   		//  Permite trazar el origen del call.
	int      estado;			//  Ver los define's mas arriba
	unsigned long stack;		//  Area de Stack reservada para la tarea
	jmp_buf  registros;			//  Guarda el estado de los registros al susp.
	unsigned stack_low,			//  Para chequear que el stack no desborde, cada tanto.
			 stack_high;
	void (*ufunc)(void);		//  Funcion que se arrancara en background
} task[MAXTASK];

void (*cSwitchTsk[16])(int, int);
int  idxTask, cST;
unsigned ustack;
unsigned FreeStacks[MAXTASK];
int  FreeStackSP;
void (*xfunc)(void);        // Temporario para lanzar nuevas funciones.

//   Prototipos Necesarios
void tSwitch(void);
int  fork(void (*)(void));
void kill(void);
int  initSwitcher(void);
void AddAnun(void (*f)(int, int));
void RemoveAnun(void (*f)(int, int));
void StackProbe(void);

int subi;

//  Inicializa el swapeador. Simplemente, define a la tarea 1 como la
//  tarea padre.
//
//  Hay algo a discutir: Para cada tarea que se arranque, fork() le asigna-
//  ra STKSIZE bytes en el stack actual de la maquina. Este arreglo normalmente
//  es mas que suficiente, pero en casos extremos debera ser ajustado en
//  base al stack TOTAL disponible, al stack MAXIMO a asignar a cada tarea y
//  a la cantidad maxima de tareas que se pueden estar ejecutando simultaneamente.
//  Se podria tambien preveer un metodo para que en el momento de fork(),
//  la rutina que llama pueda informar de cuanto stack cree que sera necesa-
//  rio para la ejecucion de la tarea.
//
//  Tambien, preservo una copia de los registros SP (eSP) para tener un
//  'stack seguro' que pueda utilizar cuando el sistema haga crash.
int initSwitcher()
{
    unsigned long _sp;
	int stacksize = 0;
	subi = 0;

	//   Preserva un stack SEGURO para abortos instantaneos.
    #ifdef MODEL_FLAT
    _asm   mov _sp, esp
	#else
    _asm   mov ss:_sp, sp
	#endif

	// Inicializa el stack para tratar de minimizar las SEHExceptions durante el DEBUG
	//memset((void *)(_sp - 50000), 0, 49100);

	// Inicializa la tarea ROOT
	safe_sp = _sp;
    ustack = _sp - STKSIZE;
	task[0].estado = RUNNING;
    task[0].stack_high = safe_sp + 10;
    task[0].stack_low  = ustack - 10;
	STRCPY(task[0].fName, "PARENT");
	return 1;
}

//   Destruye la tarea que se esta ejecutando, mientras no sea la tarea padre.
void kill()
{
	StackProbe();
	if(!idxTask) return;			// La tarea raiz NO se puede matar

	FreeStacks[FreeStackSP++] = task[idxTask].stack;
	task[idxTask].estado = DEADING; // Cambia el estado a a 'muriendo'
	tSwitch();                      // En este switcheo se destruye.
}

//   Habilita una nueva tarea en background. Se le debe pasar como parametro un puntero a la funcion que se 
//   arrancara en background. El codigo vuelve inmediatamente (la funcion pasada como parametro se ejecutara 
//   posteriormente), con cero si todo salio ok o con otro  valor si algo fallo y no pudo lanzar la tarea.
struct tsk *t;
int i;
unsigned usa;

int fork(void (*func)(void), char *procName)
{
    #ifndef MODEL_FLAT
	static unsigned sp_;
    #endif

	//  Asigna  el stack para la nueva tarea.
	if(FreeStackSP)
		usa = FreeStacks[--FreeStackSP];
	else
	 {
		usa = ustack;
		ustack -= STKSIZE;
	 }

	StackProbe();

	//   Busca un handler de tarea que sera el que se utilice.
	for(i = 0, t = task; i<MAXTASK; i++, t++)
		if(t->estado == EMPTY || t->estado == NOTUSED)
		{
			if(strlen(procName) > 30) *(procName + 30) = 0;
			STRCPY(t->fName, procName);
			t->estado = STARTING;       // Para que no se reuse.
			t->stack = usa;
            t->stack_high = usa + 10;
            t->stack_low =  ustack - 10;
			t->ufunc = func;
            setjmp(t->registros);
			return 0;
		}
	return -1;                          //   No hay lugar para mas tareas.
}

//    Incorpora, a la lista de funciones anunciantes, una nueva. Estas son
//    funciones que DEBEN ser llamadas, ante un Task-Switch, para anunciar
//    este evento a un modulo, que depende de la tarea que este activa. (Por
//    ejemplo, un Window Support).
void AddAnun(void (*func)(int, int))
{
	int i;

	for(i = 0; i < cST; i--)
		if(!cSwitchTsk[i])
		 {
			cSwitchTsk[i] = func;
			return;
		 }

	if(cST == 16)       // Mira si hay lugar.
	   AbnormalEnd("Sin lugar para funciones de anuncio. Sistema Detenido.");

	cSwitchTsk[cST++] = func;
}


//   Remueve una funcion de anuncio, que no se utilizara mas.
void RemoveAnun(void (*func)(int, int))
{
	int i;

	for(i = 0; i < cST; i++)        // Busca la tarea de referencia.
		if(cSwitchTsk[i] == func)
		 {
			cSwitchTsk[i] = 0;
			return;
		 }
}

//    La siguiente funcion es la que conmuta de una tarea a otra.
//    Debe ser llamada periodicamente para permitir la ilusion de
//    multitarea.
void tSwitch()
{
	 static struct tsk *t, *tp;
	 static int svIdxTsk, i;
	 static unsigned sp_;

	 StackProbe();

	 //   Determina la tarea que se ejecutara a partir de ahora.
	 svIdxTsk = idxTask;
	 tp = &task[idxTask];               // Tarea saliente
	 do {
			if(++idxTask == MAXTASK)    // Roll-Over?
				idxTask = 0;
			t = &task[idxTask];         // Posible tarea a habilitar.
			if(t == tp) return;         // No hay ninguna que este durmiendo.
			if(t->estado == NOTUSED)	// Evita un loop por todos los hadlers.
				idxTask = -1;
	 } while(t->estado != DORMANT &&    // Hasta que encuentre una tarea durmiendo.
			 t->estado != STARTING);

	 if(setjmp(tp->registros))
	 {
		//  Tareas a ejecutar cuando una tarea se recomienza.
		//  (Recupero de Contexto).
		t=&task[idxTask];
		t->estado = RUNNING;            // La marca como ejecutandose
	 }
	 else
	 {
		//  Tareas a ejecutar cuando una tarea se detiene.
		// (Preservacion del contexto actual).
		for(i = cST; i; i--)            // Procesos que deban anunciarse
			if(cSwitchTsk[i])
				(*cSwitchTsk[i])(svIdxTsk, idxTask);
		if(tp->estado == DEADING)       // Si estaba muriendo
			tp->estado = EMPTY;
		 else
			tp->estado = DORMANT;       // Si continua activa

		//  Si la tarea tiene el estado 'starting', entonces NO tiene
		//  inicializada la estructura 'registros' por lo que se debe llenar
		//  esta estructura.
		if(t->estado == STARTING)
		{
			if(setjmp(t->registros))
			{
				t = &task[idxTask];
				t->estado = RUNNING;     // La marca como ejecutandose
				sp_ = t->stack;
				xfunc = t->ufunc;

                //  Segun el modelo de memoria, activa distinto el stack
                #ifdef MODEL_FLAT
				
				int nueva = sp_;
                _asm {
                    mov eax, nueva
                    mov esp, eax
                    mov ebp, eax
                }

				#else
				_asm {
						mov ax, ss:sp_
						mov sp, ax
                        mov bp, ax
				}
				#endif
				(*xfunc)();				 // Llama a la funci¢n.
				kill();                  // Destruye el thread
			}
		}
        longjmp(t->registros, 1);        // Salta a la nueva tarea.
	 }
}

//   La siguiente funcion devuelve TRUE si detecto la utilizacion de
//   un puntero nulo, sino, devuelve FALSE.
//
//   Se copio el codigo de deteccion desde la funcion _nullcheck de la
//   libreria de Microsoft, que no se utiliza pues esta da un mensaje de
//   error y aborta.
//
//   15/12/98 - Se borr¢ el c¢digo de protecci¢n contra punteros nulos,
//              pues ahora el programa se esta compilando con Visual C++ y
//              con Symantec C++ (Versi¢n MSDOS). Las rutinas de Runtime
//              son diferentes y complicadas y no se justifica usarlas, dada
//              la incorporaci¢n del chequeador \pos\krnl\mem.c y el excelen-
//              te chequeo de estos bugs que tiene Visual C++.
//   01/02/00 - Se elimino el modulo \pos\krnl\mem.c, pues complicaba la pro-
//              gramacion y, hasta ahora, no habia permitido detectar ningun
//              bug que no fuera igual de facil detectarlo con el debugger de
//              Visual C.
int nullcheck()
{
	return 0;
}

//   La siguiente rutina se utiliza para verificar que no se descarriaron
//   los punteros al stack de la maquina. Se puede llamar en cualquier
//   momento desde cualquier programa, excepto desde un interrupt handler.
//   Si detecta un error, coloca el stack a un valor seguro conocido,
//   imprime el error y detiene la maquina.
void StackProbe()
{
     struct tsk *tp;
	 unsigned sp_;
return;
     #ifdef MODEL_FLAT
	 _asm mov sp_, esp
	 #else
	 _asm mov ss:sp_, sp
	 #endif

	 tp = &task[idxTask];
     if(sp_ > tp->stack_high || sp_ < tp->stack_low)
	 {
		 FCls();
         printf("\nPOS: Error Interno!!!\n");
         printf("Mtask detecto un desborde en la Pila de la CPU.\n");
		 printf("\tProceso en Ejecucion: %d\n", idxTask);
		 #ifdef __FLAT__
		 printf("\tStack-Pointer permitido (eSP)=%08X al %08X. ",
				tp->stack_low, tp->stack_high);
		 printf("Encontrado: %08X\n", sp_);
         printf("Inicial...: %08X\n", safe_sp);
		 #else
		 printf("\tStack-Pointer permitido (SP)=%04X al %04X. ",
				tp->stack_low, tp->stack_high);
		 printf("Encontrado: %04X\n", sp_);
         printf("Inicial...: %04X\n", safe_sp);
		 #endif
         printf("\nHe detenido al Sistema para evitar da¤os.\n");
         printf("Pulse una tecla para salir de POS...");
		 _getch();
         printf("\n");
		 sp_ = safe_sp;
         #ifdef MODEL_FLAT
		 _asm mov esp, sp_
		 #else
         _asm mov sp, ss:sp_
		 #endif
		 exit(1);
	 }
}

//     Imprime un estado del despachador de multitareas.
void MtaskStatus()
{
	unsigned sp_;

    #ifdef MODEL_FLAT
	_asm mov sp_, esp
	#else
	_asm mov sp_, sp
	#endif

	printf("Despachador de MultiTask:");
    #ifdef MODEL_FLAT
	sprintf(msg, "      idxTask=%d, ustack=%08X, cST=%08X",
	   idxTask, ustack, cST);
	printf(msg);
	sprintf(msg, "      safe_sp=%08X, sp=%08X", safe_sp, sp_);

	#else
	sprintf(msg, "      idxTask=%d, ustack=%04X, cST=%04X",
	   idxTask, ustack, cST);
	printf(msg);
	sprintf(msg, "      safe_sp=%04X, sp=%04X", safe_sp, sp_);
	#endif
	printf(msg);
}
#endif
