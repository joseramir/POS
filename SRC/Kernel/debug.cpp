#include <stdafx.h>

extern char *sc_MskPesable;
long startMark;
bool debugStopper;			// Helper para asegurar que se producen condiciones de carrera (race conditions)

// Estructura para proteger el encabezamiento de un bloque de memoria
struct GuardHeader
{
	char fg1, fg2;
	size_t size;
	char invoker[40];
	long time;
	short generation;
	int caller;
	char fg3, fg4;
};

// Estructura para proteger el pie de un bloque de memoria
struct GuardFooter
{
	char fg1, fg2;
	char fg3, fg4;
};


void AssertSanity(char *xmsg)
{
/*	if(strcmp(sc_MskPesable, " 20cccccmmmmmv"))
	{
		WriteBootLog(xmsg);
		DumpAddress(sc_MskPesable, "sc_MskPesable");
		DumpArea(sc_MskPesable);
		AbEnd("Se corrompio el area de articulos");
	}*/
}

char *FormatAddress(void *addr)
{
	static char res[20];
	sprintf(res, "%04X:%04X", (unsigned)(((unsigned long)addr) / 0x10000), (unsigned)(((unsigned long)addr) % 0x10000));
	return res;
}

void DumpAddress(void *addr, char *label)
{
	sprintf(msg, "DUMP - %s - Direccion: %s", label, FormatAddress(addr));
	WriteBootLog(msg);
}

void DumpArea(void *addr, int size)
{
	static char tmsg[5], tascii[32];
	char *x = (char *) (((unsigned long)addr) & 0xFFFFFFF0);
	strcpy(msg, "");
	for(int i=0; i <= size; i++)
	{
		if((i % 16) == 0)
		{
			if(strlen(msg) != 0)
			{
				strcat(strcat(msg, " "), tascii);
				WriteBootLog(msg);
			}
			strcat(strcpy(msg, FormatAddress(x)), "  ");
			tascii[0] = 0;
		}

		sprintf(tmsg, "%02X ", *x & 0xFF);
		strcat(msg, tmsg);
		if(*x >= ' ' && *x <= 'z')
			tmsg[0] = *x;
		else
			tmsg[0] = '.';
		tmsg[1] = 0;
		strcat(tascii, tmsg);

		x++;
	}
}

#ifdef MEMDEBUG

#define PRESURE 200
void **arena;
int arenaCount = 0;
short generacion = 1;

#include <assert.h>

CRITICAL_SECTION lockAlloc;
bool lockAllocInitialized;

void GetLockAlloc()
{
	if(!lockAllocInitialized) 
	{ 
		InitializeCriticalSection(&lockAlloc); 
		lockAllocInitialized = true; 
	}
	EnterCriticalSection(&lockAlloc);
}

void ReleaseLockAlloc()
{
	LeaveCriticalSection(&lockAlloc);
}

int ValidateArena(bool forceError)
{
	int usedCount = 0;
	GetLockAlloc();
	for(int arenaPtr = 0; arenaPtr < arenaCount; arenaPtr++)
	{
		char *ptr = (char *)arena[arenaPtr];
		if(ptr != NULL)
		{
			GuardHeader *hdr = (GuardHeader *)ptr;
			GuardFooter *ftr = (GuardFooter *)(ptr + hdr->size + 64);

			usedCount++;
			bool res = (hdr->fg1 == 'F');
			res = res & (hdr->fg2 == 'E');
			res = res & (hdr->fg3 == '\xaa');
			res = res & (hdr->fg4 == '\x55');
			res = res & (ftr->fg1 == 'F');
			res = res & (ftr->fg2 == 'E');
			res = res & (ftr->fg3 == '\x55');
			res = res & (ftr->fg4 == '\xaa');

			res = res & (hdr->generation <= generacion);

			if(!res)
			{
				DumpArea(ptr, hdr->size + sizeof(GuardHeader) + sizeof(GuardFooter));
				WLog("Fallo al validar la 'arena'. Bloque Corrupto: '%s'", hdr->invoker);
				assert(res);
			}
		}
	}

	assert(!forceError);
	ReleaseLockAlloc();
	return usedCount;
}

int ValidateArena()
{
	return ValidateArena(false);
}

void Intercept(GuardHeader *hdr)
{
//	if(hdr->size == 50 && hdr->caller != 520284 && hdr->caller != 12167678 && hdr->caller != 519520
//		 && hdr->caller != 74442616 && hdr->caller != 62908280)
//		assert(false);
}

void *DBG_Malloc(size_t sz, char *invoker)
{
	// Valida que ningun bloque este destruido
	ValidateArena();

	// Traba en multitarea
	GetLockAlloc();

	// Busca la entrada en la 'arena' que utilizara
	int arenaPtr = 0;
	while(arenaPtr < arenaCount && arena[arenaPtr] != NULL) arenaPtr++;

	// Si la 'arena' se lleno, entonces la agranda
	if(arenaPtr >= arenaCount)
	{
		int nCount = arenaCount + PRESURE;
		void **nptr = (void **)malloc(nCount * sizeof(void *));
		assert(nptr != NULL);
		memset(nptr, 0, nCount * sizeof(void *));
		if(arenaCount > 0)
			memcpy(nptr, arena, arenaCount * sizeof(void *));
		free(arena);
		arena = nptr;
		arenaCount = nCount;
	}

	// Reserva la memoria pedida, mas 64 bytes que se utilizan para marcar y proteger el bloque.
	char *ptr = (char *) malloc(sz + 128);
	assert(ptr != NULL);
	memset(ptr, 0, sz + 128);

	// Llena el encabezamiento
	GuardHeader *hdr = (GuardHeader *)ptr;
	hdr->fg1 = 'F'; hdr->fg2 = 'E'; hdr->fg3 = '\xAA'; hdr->fg4 = '\x55';
	hdr->size = sz;
	assert(strlen(invoker) < 39);
	STRCPY(hdr->invoker, invoker);
	hdr->time = time__();
	hdr->generation = generacion;

	// Llena el pie del bloque
	GuardFooter *ftr = (GuardFooter *)(ptr + 64 + sz);
	ftr->fg1 = 'F'; ftr->fg2 = 'E'; ftr->fg3 = '\x55'; ftr->fg4 = '\xaa';

	// Identifica el caller
	int caller; 
	_asm 
	{ 
		// Obtiene del stack la direccion de retorno
		mov eax, [ebp + 0] 
		mov [caller], eax
	}
	hdr->caller = caller;

	// Listo
	Intercept(hdr);
	arena[arenaPtr] = ptr;
	ReleaseLockAlloc();
	return ptr + sizeof(GuardHeader);
}

void *DBG_Realloc(void *handler, size_t newsz, char *invoker)
{
	GetLockAlloc();
	char *ptr = ((char *)handler) - 8;
	int oldsz = *((int *) &ptr[2]);
	
	void *res = DBG_Malloc(newsz, invoker);
	if(oldsz > (int)newsz) oldsz = (int)newsz;
	if(oldsz > 0) memcpy(res, handler, oldsz);
	DBG_Free(handler);
	ReleaseLockAlloc();
	return res;
}

void DBG_Free(void *handler)
{
	ValidateArena();

	GetLockAlloc();

	int arenaPtr = 0;
	char *ptr = ((char *)handler) - 64;
	while(arenaPtr < arenaCount && arena[arenaPtr] != ptr) arenaPtr++;
	if(arenaPtr >= arenaCount) { throw "Unallocated handler in FREE"; }
	arena[arenaPtr] = NULL;

	GuardHeader *hdr = (GuardHeader *)ptr;
	assert(hdr->fg1 == 'F');
	assert(hdr->fg2 == 'E');
	assert(hdr->fg3 == '\xaa');
	assert(hdr->fg4 == '\x55');
	assert(hdr->generation = generacion);

	GuardFooter *ftr = (GuardFooter *)(ptr + hdr->size + 64);
	assert(ftr->fg1 == 'F');
	assert(ftr->fg2 == 'E');
	assert(ftr->fg3 == '\x55');
	assert(ftr->fg4 == '\xaa');

	free(ptr);
	ReleaseLockAlloc();
}

char *DBG_StrCopy(char *d, const char *s)
{
	GuardHeader *hdr = (GuardHeader *)(d - 64);
	bool isLegalBlock = false;
	
	// El siguiente codigo podria generar un error cuando la string original (s) no fue ubicada con nuestras rutinas
	// de debug sino directamente con 'malloc' o 'strdup'. Entonces, al movernos 64 bytes hacia atras del puntero que
	// recibimos, podriamos estar accediendo memoria protegida.
	// Por el momento, solo ignoramos la exception aunque quizas deberiamos generar un error interno para que se nos
	// obligue a depurar el programa y reemplazar el 'malloc' o 'strdup' por su correspondiente macro de depuracion. 
	// De todas formas, esto no siempre sera posible para las rutinas de libreria...
	try
	{
		isLegalBlock = (hdr->fg1 == 'F' && hdr->fg2 == 'E' && hdr->fg3 == '\xaa' && hdr->fg4 == '\x55'); 
	}
	catch(...)
	{
		isLegalBlock = false;
	}

	char *res = strcpy(d, s);
	if(isLegalBlock)
	{
		GuardFooter *ftr = (GuardFooter *)(d + hdr->size);
		bool invalid = (hdr->fg1 != 'F' || hdr->fg2 != 'E' || hdr->fg3 != '\xaa' || hdr->fg4 != '\x55'); 
		invalid |= (ftr->fg1 != 'F' || ftr->fg2 != 'E' || ftr->fg3 != '\x55' || ftr->fg4 != '\xaa'); 
		if(invalid)	ValidateArena(true);
	}
	else
		ValidateArena();
	return res;
}

char *DBG_StrCat(char *d, const char *s)
{
	GuardHeader *hdr = (GuardHeader *)(d - 64);

	// El siguiente codigo podria fallar si la string no fue ubicada usando las debug-macro. Ver mas arriba.
	bool isLegalBlock = false;
	try
	{
		isLegalBlock = (hdr->fg1 == 'F' && hdr->fg2 == 'E' && hdr->fg3 == '\xaa' && hdr->fg4 == '\x55'); 
	}
	catch(...)
	{
		isLegalBlock = false;
	}

	char *res = strcat(d, s);
	if(isLegalBlock)
	{
		GuardFooter *ftr = (GuardFooter *)(d + hdr->size);
		bool invalid = (hdr->fg1 != 'F' || hdr->fg2 != 'E' || hdr->fg3 != '\xaa' || hdr->fg4 != '\x55'); 
		invalid |= (ftr->fg1 != 'F' || ftr->fg2 != 'E' || ftr->fg3 != '\x55' || ftr->fg4 != '\xaa'); 
		if(invalid)	ValidateArena(true);
	}
	else
		ValidateArena();
	return res;
}

char *DBG_StrDup(char *s, char *invoker)
{
	int slen = strlen(s) + 1;
	char *dst = (char *)DBG_Malloc(slen, invoker);
	memcpy(dst, s, slen);
	return dst;
}

int cPass = 0;

// Reemplaza los operadores globales 'new' y 'delete'
void operator delete(void *handler) { DBG_Free(handler); }
void *operator new(size_t size) 
{ 
	// Reemplaza el 'caller' para saber exactamente de donde viene el new...
	GetLockAlloc();
	void *res = (char *)DBG_Malloc(size, "new"); 

	// Corrige el caller
	GuardHeader *hdr = (GuardHeader *)(((char *)res) - 64);
	int caller; 
	_asm 
	{ 
		mov eax, [ebp+4] 
		mov [caller], eax
	}
	hdr->caller = caller;
	Intercept(hdr);
	ReleaseLockAlloc();
	return res;
}
#else

void *PROD_Malloc(size_t size, char *invoker)
{
	void *res = malloc(size);
	if(res == NULL)
	{
		WLog("FATAL - Sin memoria (MALLOC) para '%s'", invoker);
		exit(1);
	}
	memset(res, 0, size);		// Para ser compatibles con el comportamiento de 'DBG_Malloc'
	return res;
}

void *PROD_Realloc(void *ptr, size_t size, char *invoker)
{
	void *res = realloc(ptr, size);
	if(res == NULL)
	{
		WLog("FATAL - Sin memoria (REALLOC) para '%s'", invoker);
		exit(1);
	}
	return res;
}

char *PROD_StrDup(char *s, char *invoker)
{
	char *res = (char *)PROD_Malloc(strlen(s) + 1, invoker);
	strcpy(res, s);
	return res;
}

void *operator new(size_t size)
{
	void *res = malloc(size);
	memset(res, 0, size);
	return res;
}

#endif

// Vacia el detalle de la arena ocupada.
void ArenaDump()
{
#ifdef MEMDEBUG
	int h = creat("arena.dmp", S_IREAD | S_IWRITE);
	if(h < 0) return;
	char tstr[100];
	int count = 0;
	sprintf(tstr, "Generacion\tValidez\tTamaño\tCaller\tTimeStamp\tInvoker\n\n");
	write(h, tstr, strlen(tstr));
	for(int arenaPtr = 0; arenaPtr < arenaCount; arenaPtr++)
	{
		char *ptr = (char *)arena[arenaPtr];
		if(ptr != NULL)
		{
			GuardHeader *hdr = (GuardHeader *)ptr;
			GuardFooter *ftr = (GuardFooter *)(ptr + 64 + hdr->size);
		
			count++;
			int validBlk = 0;

			validBlk += (hdr->fg1 != 'F');
			validBlk += (hdr->fg2 != 'E');
			validBlk += (hdr->fg3 != '\xaa');
			validBlk += (hdr->fg4 != '\x55');
			validBlk += (ftr->fg1 != 'F');
			validBlk += (ftr->fg2 != 'E');
			validBlk += (ftr->fg3 != '\x55');
			validBlk += (ftr->fg4 != '\xAA');

			if(hdr->time >= startMark || (validBlk != 0))
			{
				sprintf(tstr, "%d\t%s\t%8d\t%8d\t%8d\t\"%s\"\n", 
					hdr->generation,
					((validBlk == 0)? "  VALID" : "INVALID"),
					hdr->size,
					hdr->caller,
					(int)hdr->time,
					hdr->invoker);
				write(h, tstr, strlen(tstr));
			}
		}
	}

	sprintf(tstr, "Bloques = %d, Items en Arena:%d\n", count, arenaCount);
	write(h, tstr, strlen(tstr));
	close(h);
	generacion++;

#endif
}

