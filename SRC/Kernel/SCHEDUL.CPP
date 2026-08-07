//
//   Schedul.cpp
//   Maneja la administracion de las tareas preprogramadas.
//

#include <stdafx.h>
#include "funcs.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif


//   Estructura para almacenar la lista de tareas.
struct pTask_ {
	unsigned long fecha_d, fecha_h;	// Rango de fechas.
	short hora_d, hora_h;			// Rango de horas.
	short dia;						// Dia de la semana asignado.
	short tarea;					// Codigo de tarea.
	char  *param;					// Parametro.
	struct flags_ {
		int uFec: 1;				// Utiliza la fecha desde/hasta.
		int uHor: 1;				// Utiliza la hora desde/hasta.
		int uDia: 1;				// Utiliza el dia de la semana.
	} flags;
} *pTask;
int cpTask;

//   Carga la tabla de tareas preprogramadas.
int InitScheduler()
{
	dbf sch;
	pTask_ *p;

	sch.Use("pos\\tareas");
	sch.Go(DB_GO_TOP);
	cpTask = sch.RecCount();
    pTask = new pTask_[sch.RecCount()];
	for(p = pTask; !sch.Eof(); p++)
	{
		sch.Get("usa_fec",   tmsg_);
		p->flags.uFec = (tmsg_[0] == 'T')? 1: 0;
		sch.Get("usa_dia",   tmsg_);
		p->flags.uDia = (tmsg_[0] == 'T')? 1: 0;
		sch.Get("usa_hora",  tmsg_);
		p->flags.uHor = (tmsg_[0] == 'T')? 1: 0;
		sch.Get("f_desde",   tmsg_);
		p->fecha_d = atol(tmsg_);
		sch.Get("f_hasta",   tmsg_);
		p->fecha_h = atol(tmsg_);
		sch.Get("dia",       tmsg_);
		p->dia = (short) atoi(tmsg_);
		sch.Get("h_desde",   tmsg_);
		p->hora_d = (short) atoi(tmsg_);
		sch.Get("h_hasta",   tmsg_);
		p->hora_h = (short) atoi(tmsg_);
		sch.Get("tarea",     tmsg_);
		p->tarea = (short) atoi(tmsg_);
		sch.Get("parametro", tmsg_);
		p->param = STRDUP(tmsg_, "InitScheduler2");
		sch.Skip(1);
	}
	sch.Close();
	CheckScheduler();	// Ejecucion Inicial.
	return 0;
}

//   Rutina que periodicamente verifica la ejecucion de las tareas preprogramadas.
void CheckScheduler()
{
	pTask_ *p;
	int i, proc1, proc2, proc3;
	static struct DPTarea_ dpt;

	for(p = pTask, i = cpTask; i; i--, p++)
	{
		// Determina si corresponde ejecutar la actualizacion.
		proc1 = proc2 = proc3 = 0;
		if(p->flags.uDia)
		{
			if(dow() == p->dia) proc1 = 1;
		} else proc1 = 1;
		if(p->flags.uFec)
		{
			if(date() >= p->fecha_d && 
			  ((p->fecha_h != 0) || (date() <= p->fecha_h))) proc2 = 1;
		} else proc2 = 1;
		if(p->flags.uHor)
		{
			int hora = time_() / 100;
			if(hora >= p->hora_d && 
			  ((p->hora_h != 0) || (hora <= p->hora_h))) proc3 = 1;
		} else proc3 = 1;

		if(proc1 && proc2 && proc3)
		{
			//   Registra el log.
			dpt.func = DPTarea;
			dpt.nRec = cpTask - i;
			dpt.tFunc = p->tarea;
			WriteDump(&dpt);

			//   Ejecuta la tarea.
			switch(p->tarea)
			{
				case 1:		// Cambia la lista default.
					ExecOtroPrec(atoi(p->param), 1);
					break;
			}
		}
	}
}
