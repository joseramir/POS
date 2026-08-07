//
//   xFile.c
//   Implementación del i/o a archivos.
//

#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Mantenemos una lista de los archivos abiertos, para no 'reabrir' los
//   mismos y para que rutinas externas puedan CERRAR archivos que necesitan
//   usar (archivos que se 'reabriran' la siguiente vez que se necesiten.
class xFile *fList;
char  tFileName[304];

#ifdef POSXP

// Estados en los que puede estar la thread que vacia buffers a disco.
#define FT_STOPPED  0
#define FT_WAITING  1
#define FT_WORKING  2
#define FT_ENDREQ   3

int flushThreadControl = FT_STOPPED;
HANDLE hEvent;
#endif


/////////////////////////////////////////////////////////////////////////
//   La inicialización del sistema transaccional DEBE hacerse sí o sí,
//   para que se puedan recuperar transacciones abortadas por cortes de
//   luz, etc. También, debería poder interceptarse las salidas a lo loco
//   del sistema, para deshacer transacciones que estén pendientes. Para
//   lograr esto, creo un objeto cuya construcción/destrucción por parte
//   del programa, cuando se carga, hace todas esas cosas.

class TT_txn
{
public:
	TT_txn::TT_txn();			// Inicializador.
	TT_txn::~TT_txn();			// DesInicializador.
};

//   Rutina de inicialización (constructor).
TT_txn::TT_txn()
{
}

// Apagado del programa.
TT_txn::~TT_txn()
{
}

//  Comienza un nivel más de transacción.
int xTTStart()
{
	return 1;
}

//  Cierra un nivel de transacciones.
int xTTCommit()
{
	return 1;
}

//  Deshace un nivel de transacciones.
int xTTRollback()
{
	return 1;
}

//  Función que se llama mientras se espera al sistema transaccional.
void xTTBusy()
{
}

/////////////////////////////////////////////////////////////////////////
//   Clase de manejo y cache de archivos. Constructor.
xFile::xFile()
{
	memset(this, 0, sizeof(xFile));
	next = fList;
	fList = this;
}

//   Destructor de archivo.
xFile::~xFile()
{
	class xFile *f;

	//   Cierra el archivo, si estaba abierto.
	if(flags.opened) xClose();
	
	//   Lo quita de la lista de estructuras creadas.
	if(fList == this)
		fList = next;
	else
	{
		for(f = fList; f && f != (xFile *) 0xfeeefeee; f = f->next)
		{
			if(f->next && (f->next == this))
			{
				f->next = next;
				break;
			}
		}
	}
}

//   Devuelve el flag de archivo abierto/cerrado.
//   No se fija en el flag de 'reopen', porque ese es para manipuleo interno.
int xFile::xOpened()
{
	if(flags.opened) return 1;
	return 0;
}

//   Activa el flag de cursor de un archivo.
void xFile::xCursor()
{
	flags.iscursor = 1;
	xCache(CACHE_LOAD_FILE);
}

//   Abre un archivo.
int  xFile::xOpen(char *fn, int flags1, int flags2)
{
	char *tChar;

	if(flags.opened) xClose();
	errno = 0;
	h = sopen(fn, flags1, flags2);

	flags.loaded = flags.writePend = flags.opened = flags.iscursor = flags.reopen = 0;
	Error = "Ok";
	if(h >= 0)
	{
		OpenFlags1 = flags1;
		OpenFlags2 = flags2;
		tChar = _fullpath(tFileName, fn, 300);
		STRCPY(lfn, tChar? tChar: fn);
		flags.opened = 1;
	}
	else
	{
		switch(errno)
		{
			case EACCES:
				Error = "EACCES";
				break;

			case EEXIST:
				Error = "EEXIST";
				break;

			case EINVAL:
				Error = strerror(_doserrno);
				break;

			case EMFILE:
				Error = "EMFILE";
				break;

			case ENOENT:
				Error = "ENOENT";
				break;

			default:
				Error = "UNKNOWN";
		}
	}
	return h;
}

//   Reabre un archivo que fue temporariamente cerrado.
void xFile::xReopen()
{
	if(!flags.opened || !flags.reopen) return;
	h = sopen(lfn, OpenFlags1, OpenFlags2);
	if(h < 0)
	{
		sprintf(tFileName, "No puedo REABRIR a '%s'. Abortando...\n", lfn);
		AbnormalEnd(tFileName);
	}
	flags.reopen = flags.loaded = 0;
	lseek(h, chPos, SEEK_SET);
}

//   Cierra temporariamente un archivo.
void xFile::xTmpClose()
{
	if(!flags.opened || flags.reopen) return;
	if(flags.iscursor && flags.loaded) return;
	if(flags.loaded && flags.writePend)
	{
		lseek(h, 0l, SEEK_SET);
		write(h, chFile, szMem);
		flags.writePend = 0;
	}
	close(h);
	flags.reopen = 1;
}

//   Vacia los buffers de cache.
void xFile::xFlush()
{
	if(!flags.opened) return;
	if(flags.reopen) xReopen();
	if(flags.iscursor && flags.loaded) return;
	if(flags.loaded && flags.writePend)
	{
		lseek(h, 0l, SEEK_SET);
		write(h, chFile, szMem);
		flags.writePend = 0;
	}
	close(h);
	h = sopen(lfn, OpenFlags1, OpenFlags2);
}

//   Crea un nuevo archivo.
int  xFile::xCreate(char *fn, int flags1)
{
	char *tChar;

	if(flags.opened) xClose();
	flags.iscursor = flags.opened = flags.loaded = flags.writePend = 0;
	h = creat(fn, flags1);
	if(h >= 0)
	{
		OpenFlags2 = _SH_DENYRW;
		OpenFlags1 = O_RDWR | O_BINARY;
		tChar = _fullpath(tFileName, fn, 300);
		STRCPY(lfn, tChar? tChar: fn);
		flags.opened = 1;
	}
	return h;
}

//   Especifica el esquema de cache a utilizar.
int  xFile::xCache(int cMode)
{
	switch(cMode)
	{
		case CACHE_LOAD_FILE:
			if(!flags.loaded)
			{
				if(flags.reopen) xReopen();
				szMem = xFilelen();
				chFile = (char *) MALLOC(szMem + 10, "xFile:xCache1");
				if(!chFile) return 0;
				chPos = tell(h);
				if(szMem)
				{
					lseek(h, 0l, SEEK_SET);
					read(h, chFile, szMem);
				}
				flags.loaded = 1;
				flags.writePend = 0;
				return 1;
			}
			break;
	}
	return 0;
}

//   Cierra un archivo.
int  xFile::xClose()
{
	int x;

	// Si estaba cacheado y hay escrituras pendientes, las graba.
	if(flags.loaded && flags.writePend)
	{
		if(!flags.iscursor)
		{
			if(flags.reopen) xReopen();
			lseek(h, 0l, SEEK_SET);
			write(h, chFile, szMem);
		}
		flags.writePend = 0;
	}
	
	// Cierra el handler.
	x = 0;
	if(flags.opened)
	{
		flags.opened = 0;
		if(!flags.reopen) x = close(h);
		if(flags.iscursor) remove(lfn);
		flags.reopen = 0;
	}

	//   Libera la memoria del cache, si corresponde.
    if(flags.loaded) FREE(chFile);
	flags.loaded = 0;
	return x;
}

//   Lee desde un archivo.
int  xFile::xRead(void *buff, int cant)
{
	int cRead;

	if(!flags.opened) return 0;
	if(flags.loaded)
	{
		cRead = szMem - chPos;
		if(cRead >= cant) cRead = cant; else cant = cRead;
		if(cRead <= 0) return 0;
		memmove(buff, chFile + chPos, cRead);
		chPos += cRead;
		return cRead;
	}

	if(flags.reopen) xReopen();
	cRead = read(h, buff, cant);
	chPos = tell(h);
	return cRead;
}

//   Escribe a un archivo.
int  xFile::xWrite(void *buff, int cant)
{
	int xRet;

	if(!flags.opened) return 0;
	if(flags.loaded)
	{
		if(chPos + cant > szMem)	//  Cambia el tamaño de la memoria.
			xChsize(chPos + cant);
		if(flags.loaded)			//  Si continua en memoria.
		{
			memmove(chFile + chPos, buff, cant);
			flags.writePend = 1;
			chPos += cant;
			return cant;
		}
	}

	if(flags.reopen) xReopen();
	xRet = write(h, buff, cant);
	chPos = tell(h);
	return xRet;
}

//   Ubica el puntero de un archivo.
long xFile::xLseek(long pos, int flag)
{
	if(!flags.opened) return 0l;
	if(flags.loaded)
	{
		switch(flag)
		{
			case SEEK_CUR:
				chPos += pos;
				break;
			case SEEK_END:
				chPos = szMem + pos;
				break;
			default:
				chPos = pos;
				break;
		}
		if(chPos < 0l) chPos = 0l;
		return chPos;
	}

	if(flags.reopen) xReopen();
	return (chPos = lseek(h, pos, flag));
}

//   Traba/Destraba una secuencia de bytes en el archivo.
int  xFile::xLocking(int lockflg, long cBytes)
{
	if(!flags.opened) return 0;
	if(flags.reopen) xReopen();
	if(flags.loaded)
		lseek(h, chPos, SEEK_SET);
    return locking(h, (lockflg? LK_NBLCK: LK_UNLCK), cBytes);
}

//   Retorna la posición actual del puntero de un archivo.
long xFile::xTell()
{
	if(!flags.opened) return 0l;
	return chPos;
}

//   Cambia el tamaño de un archivo.
int xFile::xChsize(long nSize)
{
	char *nPos;

	//  Si es un archivo regular.
	if(flags.reopen) xReopen();
	if(!flags.loaded)
	{
		chsize(h, nSize);
		return nSize;
	}
	
	//  Descarga las modificaciones de archivos de memoria.
	if(flags.writePend)
	{
		if(!flags.iscursor)
		{
			lseek(h, 0l, SEEK_SET);
			write(h, chFile, szMem);
		}
		flags.writePend = 0;
	}
	if(nSize != szMem)
	{
		nPos = (char *) REALLOC(chFile, nSize, "xFile:xChSize");
		if(!nPos)
		{
			//   No hay mas memoria, asi que convierte el archivo en uno regular.
			if(flags.iscursor)
			{
				lseek(h, 0l, SEEK_SET);
				write(h, chFile, szMem);
			}
            FREE(chFile);
			flags.loaded = 0;
			return nSize;
		}
		chFile = nPos;
		szMem = nSize;
	}
	return nSize;
}

//   Retorna el largo (cantidad de bytes) de un archivo.
long xFile::xFilelen()
{
	if(flags.loaded) return szMem;
	if(flags.reopen) xReopen();
	return filelength(h);
}

//   Retorna TRUE si esta al final del archivo.
int xFile::xEof()
{
	if(flags.loaded) return (szMem >= chPos)? 0: 1;
    if(xFilelen() <= xTell()) return 1;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////   RUTINAS GLOBALES /////////////////////////////////
void TemporaryClose(char *fn)
{
	class xFile *f;
	char *tChar;

	tChar = _fullpath(tFileName, Trim(fn), 300);
	if(!tChar) tChar = fn;
	for(f = fList; f; f = f->next)
	{
		if(f->lfn && !_stricmp(f->lfn, tChar))
		{
			f->xTmpClose();
			break;
		}
	}
}

// Cambia la extension de un archivo.
void ReplaceExtension(char *fn, char *ext, bool always)
{
	int len = strlen(fn);
	for(int i = strlen(fn) - 1; i >= 0; i--)
	{
		if(fn[i] == '.')
		{
			if(always)
				STRCPY(&fn[i], ext);
			return;
		}
		if(fn[i] == '\\') break;
	}
	STRCAT(fn, ext);
}

// Busca todos los archivos que cumplan con una determinada mascara.
// ATENCION: El llamador debe destruir el arraylist que se retorna, cuando ya no lo usa.
::ArrayList *FindFiles(char *mask)
{
	::ArrayList *result = new ::ArrayList(true, "FindFiles");

#ifdef WIN32

	struct _finddata_t fd;
	intptr_t hFile = _findfirst(mask, &fd);
	if(hFile >= 0)
	{
		do {
			result->Add(fd.name, true);
		} while(_findnext(hFile, &fd) == 0);
		_findclose(hFile);
	}

#else

	FIND *find = findfirst(mask, _A_ARCH);
	while(find != NULL)
	{
		result->Add(find->name, true);
		find = findnext();
	}

#endif

	return result;
}

// Retorna la fecha y hora de la ultima vez que se escribio en el archivo.
struct tm *LastAccessFile(char *fn)
{
#ifdef WIN32

	//struct _finddata_t fd;
	struct _finddata32_t fd;
	//intptr_t hFile = _findfirst(fn, &fd);
	intptr_t hFile = _findfirst32(fn, &fd);
	if(hFile >= 0)
	{
		struct tm *res = _localtime32(&fd.time_write);
		_findclose(hFile);
		return res;
	}
	return NULL;

#else

	struct time_format
	{   
		unsigned two_seconds: 5;
		unsigned minutes: 6; 
		unsigned hours: 5;
	} time; 
	struct date_format
	{   
		unsigned day: 5;
		unsigned month: 4; 
		unsigned year: 7;
	} date; 

	FIND *find = findfirst(fn, _A_ARCH);
	if(find != NULL)
	{
		static struct tm res;
		time = *(struct time_format *)& find->time; 
		date = *(struct date_format *)& find->date;

		// convierte a la estructura tm usada por esta funcion.
		res.tm_sec = time.two_seconds;
		res.tm_min = time.minutes;
		res.tm_hour = time.hours;
		res.tm_mday = date.day;
		res.tm_mon = date.month;
		res.tm_year = 1980 + date.year;

		return &res;
	}
	return NULL;

#endif
}

#ifdef POSXP

// Estados en los que puede estar la thread que vacia buffers a disco.
#define FT_STOPPED  0
#define FT_WAITING  1
#define FT_WORKING  2
#define FT_ENDREQ   3

// Metodo que, en un hilo separado de trabajo, se encarga de vaciar los buffers de XP cuando corresponden.
void DoFlush(void *pParams)
{
	flushThreadControl = FT_WAITING;
	while(flushThreadControl != FT_ENDREQ)
	{
		// Espera que se le señalice la necesidad de hacer el flush.
		WaitForSingleObject(hEvent, INFINITE);
		flushThreadControl = FT_WORKING;

		// Efectua UN flush al archivo.
		LPCSTR szVolume="\\\\.\\C:";				// 'nombre' del disco c
		HANDLE hFile=CreateFile(
			szVolume,								// Nombre
			GENERIC_READ | GENERIC_WRITE,			// Permisos
			FILE_SHARE_READ | FILE_SHARE_WRITE,		// Modo
			0,										// 
			OPEN_EXISTING,							// No trata de crearlo si no existe. Es un disco!!
			0,										//
			0); 

		if (hFile!=INVALID_HANDLE_VALUE) 
		{
			int res = FlushFileBuffers(hFile);
			if(!res)
				WriteErr("Fallo al vaciar buffers de XP");
			CloseHandle(hFile);
		}

		flushThreadControl = FT_WAITING;
	}

	flushThreadControl = FT_STOPPED;
}
#endif

// Vacia todos los buffers de escritura de windows XP
void FlushXPBuffers()
{
#ifdef POSXP

	// Crea la thread que atiende el vaciado de buffers, si no se creo todavia
	if(flushThreadControl == FT_STOPPED)
	{
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread(DoFlush, 0, NULL);
		while(flushThreadControl != FT_WAITING)
			Sleep(10);
	}

	// Envia a la thread la señal para que lance un vaciado de buffers
	if(flushThreadControl == FT_WORKING) return;
	SetEvent(hEvent);

#endif
}