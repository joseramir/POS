/*
 *   HoraFec.c
 *   Permite obtener la fecha y la hora desde el sistema.
*/

#include <stdafx.h>

char stattime[16];
char statdate[16];
time_t TNULL;

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Devuelve la hora actual, en el formato hh:mm
char *hora()
{
    static struct tm *x;

    time(&TNULL);
    x = localtime(&TNULL);
    sprintf(stattime, "%02d:%02d", x->tm_hour, x->tm_min);
	return stattime;
}

//   Devuelve la hora actual, como el short hhmm
int time_()
{
	static struct tm *x;

	time(&TNULL);
	x = localtime(&TNULL);
	return x->tm_hour * 10000 + x->tm_min * 100 + x->tm_sec;
}

//   Devuelve la hora actual, como el long hhmmss
long time__()
{
	static struct tm *x;	

	time(&TNULL);
	x = localtime(&TNULL);
	return x->tm_hour * 10000 + x->tm_min * 100 + x->tm_sec;
}

// Devuelve los segundos de la hora actual.
long seconds_()
{
	static struct tm *x;

	time(&TNULL);
	x = localtime(&TNULL);
	return x->tm_sec + 
		(x->tm_min * 60) +
		(x->tm_hour * 60 * 60) +
		(x->tm_mday * 60 * 60 * 24);
}

//   Devuelve la fecha actual, en el formato dd-mm-aaaa
char *fecha()
{
    static struct tm *x;
	int aio;

    time(&TNULL);
    x = localtime(&TNULL);
    aio = x->tm_year + 1900;
    sprintf(statdate, "%02d/%02d/%04d", x->tm_mday, x->tm_mon+1,
                                        aio);
	return statdate;
}


//   Devuelve la fecha actual, en el formato dd-mm-aaaa
char *periodofact()
{
	static struct tm *x;
	int aio;
	char nommes[20];

	time(&TNULL);
	x = localtime(&TNULL);
	aio = x->tm_year + 1900;
	switch (x->tm_mon + 1)
	{
		case 1:
			strcpy(nommes, "ENERO");
			break;
		case 2:
			strcpy(nommes, "FEBRERO");
			break;
		case 3:
			strcpy(nommes, "MARZO");
			break;
		case 4:
			strcpy(nommes, "ABRIL");
			break;
		case 5:
			strcpy(nommes, "MAYO");
			break;
		case 6:
			strcpy(nommes, "JUNIO");
			break;
		case 7:
			strcpy(nommes, "JULIO");
			break;
		case 8:
			strcpy(nommes, "AGOSTO");
			break;
		case 9:
			strcpy(nommes, "SEPTIEMBRE");
			break;
		case 10:
			strcpy(nommes, "OCTUBRE");
			break;
		case 11:
			strcpy(nommes, "NOVIEMBRE");
			break;
		case 12:
			strcpy(nommes, "DICIEMBRE");
			break;
	}
	sprintf(statdate, "%s %04d", nommes, aio);
	return statdate;
}


//   Devuelve el dia de la semana (1=domingo, ...)
int dow()
{
	static struct tm *x;
	
	time(&TNULL);
	x = localtime(&TNULL);
	return x->tm_wday;
	
}

//   Devuelve la fecha actual, como el long aaaammdd
unsigned long date()
{
    static struct tm *x;
	static int aio;

    time(&TNULL);
    x = localtime(&TNULL);
    aio = x->tm_year + 1900;
    return aio * 10000 + (x->tm_mon + 1) * 100 + x->tm_mday;
}

int ComparaTm(struct tm *t1, struct tm *t2)
{
	if(t1->tm_year != t2->tm_year) return t1->tm_year - t2->tm_year;
	if(t1->tm_mon != t2->tm_mon) return t1->tm_mon - t2->tm_mon;
	if(t1->tm_mday != t2->tm_mday) return t1->tm_mday - t2->tm_mday;
	if(t1->tm_hour != t2->tm_hour) return t1->tm_hour - t2->tm_hour;
	if(t1->tm_min != t2->tm_min) return t1->tm_min - t2->tm_min;
	return t1->tm_sec - t2->tm_sec;
}

bool HayQueCerrar(long ptime, long intervalo)
{
	struct tm *newtime, *oldtime;

	time_t result;
	time_t long_time;
	double elapsed_time;
	long auxtime = ptime;
	bool auxret = false;

	time( &long_time );

	newtime = localtime( &long_time );
	oldtime = localtime( &long_time );

	auxtime = ptime % 10000;
	newtime->tm_hour = (int) ptime /10000;
	newtime->tm_min = (int) auxtime / 100;
	auxtime = auxtime % 100;
	newtime->tm_sec = (int) auxtime;	

	result = mktime(newtime);
	if ( result == (time_t)-1)
	{				
	}
	else
	{
		elapsed_time = difftime(long_time, result );
		if(elapsed_time > intervalo)
			auxret = true;
	}
//	delete newtime; delete oldtime;
	return auxret;
}