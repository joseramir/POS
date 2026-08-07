//   Debug.h
//   Definicion de variables de control del proceso de debug online.

//   OJO: Hay que definir la variable POSDEBUG para el linker, para poder depurar.

//   Funciones de log. Estan en pos.cpp
#ifndef DEBUG_H_INCLUDED__
#define DEBUG_H_INCLUDED__

#define BM_INICIANDO 1
#define BM_FINALIZANDO 2
#define BM_MSG 3
#define BM_MSGZ 4

void BootLogInit(void);
void BootLogClose(void);
void WriteBootLog(char *);
void BootLogExit(void);

void DumpAddress(void *addr, char *msg);
void DumpArea(void *addr);
void AssertSanity(char *);

//   Funciones de depuracion.
extern int DBGRxPlus,		// Depuracion de la recepcion de articulos.
           DBGActiPlus,		// Depura el proceso de activacion de articulos.
		   DBGCopyFile,     // Depura el proceso de copia de archivos.
		   DBGPrintProc;	// Depura los procesos de impresion.

extern long startMark;		// Marca (de tiempo) de cuando finalizo la inicializacion.
extern bool debugStopper;	// Helper para producir condiciones de carrera (race conditions)

extern char dbgStr[];	// Para mensajes temporarios.
void WriteBootLog(char *msg, int flag);
#ifdef POSDEBUG
#define DBG(DBGVarControl, DBGLevel, DBGMsg) \
		if(DBGLevel<=DBGVarControl){WriteBootLog(DBGMsg,BM_MSGZ);}
#else
#define DBG(DBGVarControl, DBGLevel, DBGMsg)
#endif

// Funciones de ubicacion de memoria.
void ArenaDump();
void ValidateGuards();
#ifdef MEMDEBUG

void *DBG_Malloc(size_t x, char *invoker);
void  DBG_Free(void *ptr);
void *DBG_Realloc(void *ptr, size_t size, char *invoker);
char *DBG_StrDup(char *str, char *invoker);
char *DBG_StrCopy(char *dst, const char *src);
char *DBG_StrCat(char *dst, const char *src);

int ValidateArena();

#define MALLOC(x, n) DBG_Malloc(x, n)
#define FREE(x) DBG_Free(x)
#define REALLOC(x, sz, n) DBG_Realloc(x, sz, n)
#define STRDUP DBG_StrDup
#define STRCPY DBG_StrCopy
#define STRCAT DBG_StrCat

#else

void *PROD_Malloc(size_t x, char *invoker);
void *PROD_Realloc(void *ptr, size_t size, char *invoker);
char *PROD_StrDup(char *str, char *invoker);

#define MALLOC(x, n) PROD_Malloc(x, n)
#define FREE(x) free(x)
#define REALLOC(x, sz, n) PROD_Realloc(x, sz, n)
#define STRDUP(x, n) PROD_StrDup(x, n)
#define STRCPY strcpy
#define STRCAT strcat

#endif
#endif
