/*
 *   DumpErr.c
 *   Graba en un archivo los mensajes que se consideren interesantes o para
 *   rastreo del programa.
*/

#include <stdafx.h>

char ERRDUMP[] = "error.log";

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   AGREGARLE PARA QUUE IMPRIMA FECHA Y HORA DEL MENSAJE.
//   (La implementaci¢n de Fecha y Hora usada con QC no es portable y daba
//   mil errores al cambiar de plataforma, as¡ que la saque).
void WriteErr(char *msg)
{
	static FILE *h;

	if(!ActiveDump) return;
    h = fopen(ERRDUMP, "a");
    if(!h)
		return;
    fprintf(h, "%s-%s ERROR DUMP: %s\n", fecha(), hora(), msg);	 
	fclose(h);
}

/*
 *   Inicializa el dumping de errores.
*/

int ErrorInit()
{
	static FILE *h;

	ActiveDump = 0;
    h = fopen(ERRDUMP, "r");
    if(!h)
		return 1;
	fclose(h);
	remove("error.3");
	rename("error.2", "error.3");
	rename("error.1", "error.2");
	rename("error.log", "error.1");
	return 0;
}

/*
 *   La siguiente funcion se utiliza para bajar un estado del sistema al
 *   archivo de Control.
*/

void DumpStatus(char *quien)
{
	if(ActiveDump)
	{
		WriteErr("Final del Trazado de Programa.");
		ActiveDump = 0;
		return;
	}

	ActiveDump = 1;
	StackProbe();               // Verifica que este todo en orden
	sprintf(msg, "Dumping de Estado solicitado por '%s'.", quien);
	WriteErr(msg);
	EventStatus();
	MtaskStatus();
	WriteErr("Final del Dumping --------------------------");
}

