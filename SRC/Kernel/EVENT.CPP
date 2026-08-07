/*
    Event.c
    Manejador de Eventos. Este modulo permite descargar en una cola de
    espera una serie de eventos, a medida que se van produciendo. Los
    eventos se definen de forma INDEPENDIENTE de la tarea a realizar, es
    decir, cada modulo debe proveer el manejador de eventos que necesite.

	Los eventos se descargan de una sola forma:

	   1) Automatica. Al arrancar el generador de eventos, este deja una
	  porcion de el mismo ejecutandose en background. Cuando detecta
	  un nuevo evento, el mismo se encarga de quitar el evento de la
	  cola y llamar al manejador requerido.
*/

#include <stdafx.h>

#define MAXEVENT 1000

// Optimiza el codigo
#ifdef POSXP
#include "GCStatic.h"
#endif

/*   Datos Globales   */
void (*manejador[MAXEVENT])(char *);
struct evnt {
       int clase;                       // Clase. Las clases se distribuyen
										// en el archivo 'evntclas.h'
       char param[32];                  // Parametro a pasar al manejador
} evento[MAXEVENT];
int e_in, e_out;
struct evnt e;

void SyncroLoop(void);
void EventLoop(void);

int initEvent()
{
	 e_in = e_out =0;                   //  No hay ninguno.
	 return 1;
}

bool PeekEvent(int clase, char *param)
{
	GCStatic::LockEvent("PeekEvent");
	for(int i = e_out; i != e_in; i++)
	{
		if(i >= MAXEVENT) i = 0;
		e = evento[i];
		if(e.clase == clase)
		{
			memmove(param, e.param, 32);
			while(i != e_in)
			{
				int nxt = i + 1;
				if(nxt >= MAXEVENT) nxt = 0;
				evento[i] = evento[nxt];
				i++;
				if(i >= MAXEVENT) i = 0;
			}
			e_in--;
			if(e_in < 0) e_in = MAXEVENT - 1;
			GCStatic::ReleaseLockEvent("PeekEvent");
			return true;
		}
	}

	GCStatic::ReleaseLockEvent("PeekEvent");
	return false;
}

// Despacha eventos. OJO: Pueden haber interrupciones en el medio, que pudran todo.
void eventDispatch()
{
	while(e_out != e_in)                   //  Si hay eventos, los atiende.
	{
		GCStatic::LockEvent("eventDispatch");
		e = evento[e_out++];           // Evento a atender
		if(e_out >= MAXEVENT) e_out=0;
		GCStatic::ReleaseLockEvent("eventDispatch");
		if(!manejador[e.clase])
			AbnormalEnd("Evento sin manejador instalado!!!");
		(*(manejador[e.clase]))(e.param);
	}
}

// Vacia la lista de eventos disponibles.
void FlushEvents()
{
	GCStatic::LockEvent("FlushEvents");
	e_out = e_in;
	GCStatic::ReleaseLockEvent("FlushEvents");
}

/*
**   Agrega un evento a la lista de eventos pendientes de ser atendidos.
**   OJO al piojo. Esta funcion puede ser llamada por interrupciones.
*/
void addEvent(int clase, char *param, unsigned sizeparam)
{
	struct evnt *e;
	char *p;
	int n_in;

	GCStatic::LockEvent("addEvent");
	e = &evento[e_in];                   // Evento a usar.
	n_in = e_in + 1;
	if(n_in >= MAXEVENT) n_in = 0;
	if(n_in == e_out)
	{
		GCStatic::ReleaseLockEvent("addEvent");
		return;		                     // Se saturo la queue de eventos.
	}
	e_in = n_in;
	e->clase = clase;
	if(sizeparam>32) sizeparam=32;
	for(p = e->param; sizeparam; sizeparam--)
		*p++ = *param++;
	GCStatic::ReleaseLockEvent("addEvent");
}

/*
 *   Imprime el estado del despachador de eventos.
*/

void EventStatus()
{
	sprintf(msg, "   Event Handler: e_in=%d, e_out=%d", e_in, e_out);
	WriteErr(msg);
}

/*
**   Define un evento.
*/

void defineEvent(int clase, void (*manej)(char *))
{
	 manejador[clase] = manej;
}

