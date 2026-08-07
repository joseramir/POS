#ifndef TEM_HANDLER_INCLUDED__
#define TEM_HANDLER_INCLUDED__

#ifndef NULL
#define NULL ((void *)0)
#endif

class Temp
{
public:
	static void SetTemporary(void *ptr);
	static void *Alloc(int size);
	static char *StrDup(char *str);
	static void *MemDup(void *src, int size);
	static void  Free(void *ptr);
};

#endif  TEM_HANDLER_INCLUDED__
