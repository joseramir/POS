//
//   ArrayList.cpp
//   Implementacion de la clase ArrayList que permite mantener arrays dinamicos de objetos.
//

#include <stdafx.h>

#define ARENASIZE	1000				// Incrementos en el area de arena

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

class IndexedEntry						// Para entradas indexadas.
{
public:
	int key;
	void *obj;

	IndexedEntry(int _key, void *_obj)
	{
		key = _key;
		obj = _obj;
	}
};

// Funcion a usar para comparar entradas indexadas.
int (*useCompareFunc)(const void *, const void *);

// Cuando la lista contiene elementos indexados, para ordenarlos debemos acceder
// al puntero en la entry.
int InternalCompare(const void *d1, const void *d2)
{
	IndexedEntry 
		**e1 = (IndexedEntry **)d1, 
		**e2 = (IndexedEntry **)d2;
	return (*useCompareFunc)((*e1)->obj, (*e2)->obj);
}

//   Ordena la lista usando la funcion de comparacion que se pasa como parametro.
void ::ArrayList::Sort(int (*compareFunc)(const void *, const void *))
{
	if(indexed)
	{
		useCompareFunc = compareFunc;
		qsort((void *)arena, (size_t)usedCount, (size_t)sizeof(void *), InternalCompare);
	}
	else
		qsort((void *)arena, (size_t)usedCount, (size_t)sizeof(void *), compareFunc);
}

//   Constructor.
::ArrayList::ArrayList(bool dstroy, char *own)
{
	owner = STRDUP(own, "ArrayListCtor");
	arena = NULL;						// Inicializa las areas internas.
	arenaLen = usedCount = 0;
	destroy = dstroy;
	indexed = false;
}

//   Constructor - La string recibida la parsea y agrega como elementos sueltos.
::ArrayList::ArrayList(char *str, char *own)
{
	owner = STRDUP(own, "ArrayListCtor");
	arena = NULL;						// Construccion estandard.
	arenaLen = usedCount = 0;
	destroy = true;
	indexed = false;
	this->AddSplitted(str, ',');
}

//   Destructor.
::ArrayList::~ArrayList()
{
	Clear();
	if(arenaLen > 0) { FREE(arena); arena = NULL; }
	FREE(owner); owner = NULL;
}

//   Retorna la cuenta de los elementos activos.
int ::ArrayList::Count()
{
	return usedCount;
}

// Si el contenido es una lista indexada, entonces retorna la clave de del item indicado.
int ::ArrayList::Key(int index)
{
	if(usedCount <= index || !indexed) return -1;
	IndexedEntry *entry = (IndexedEntry *)arena[index];
	return entry->key;
}

//   Retorna el puntero a un elemento cualquiera. Si no estuviera en la lista, retorna NULL.
void *::ArrayList::Item(int index)
{
	if(usedCount <= index) return NULL;
	void *res = arena[index];
	if(indexed)
		res = ((IndexedEntry *)res)->obj;
	return res;
}

//   Retorna el puntero a un elemento cualquiera, convertido en string.
char *::ArrayList::ItemString(int index)
{
	return (char *) Item(index);
}

//   Cambia el valor de un elemento cualquiera.
void ::ArrayList::SetItem(int index, void *obj)
{
	if(usedCount <= index) throw "ArrayList::SetItem";
	if(indexed)
	{
		IndexedEntry *entry = (IndexedEntry *)arena[index];
		if(destroy && (entry->obj != NULL)) { delete entry->obj; entry->obj = NULL; }
		entry->obj = obj;
	}
	else
	{
		if(destroy && (arena[index] != NULL)) { delete arena[index]; arena[index] = NULL; }
		arena[index] = obj;
	}
}

//   Agrega un elemento al pie de la lista.
int ::ArrayList::Add(void *obj)
{
	if(arenaLen <= usedCount)
		RemapArena(arenaLen + ARENASIZE);
	arena[usedCount] = obj;
	return usedCount++;
}

//   Corta una string utilizando como separador el caracter que se indica y agrega cada
//   uno de los elementos.
void ::ArrayList::AddSplitted(char *str, char sep)
{
	char temp[100];				// Parseo de la string.
	char *p = temp;
	bool inComi = false;
	while(*str)
	{
		if(*str == '\"') 
		{
			inComi = !inComi;
			str++;
		}
		else if(!inComi && *str == ',')
		{
			str++;
			*p = 0;
			this->Add(temp, true);
			p = temp;
		}
		else *p++ = *str++;
	}
	*p = 0;
	this->Add(temp, true);
}

//   Agrega una string previo removerle los posibles espacios en blanco que tenga.
int ::ArrayList::Add(char *s, bool trim)
{
#if MEMDEBUG
	char debname[100];
	sprintf(debname, "ArrayList:Add - %s", owner);
	char *str = STRDUP(s, debname);
#else
	char *str = STRDUP(s, "ArrayList:Add");
#endif

	if(trim)
	{
		while(isspace(str[0])) STRCPY(str, str+1);
		while(strlen(str)>0 && isspace(str[strlen(str)-1])) str[strlen(str)-1] = 0;
	}
	return this->Add(str);
}

//   Elimina un elemento dado su indice.
void ::ArrayList::RemoveAt(int index)
{
	if(usedCount <= index) return;
	int toMove = (usedCount - index - 1) * sizeof(void *);
	if(destroy || indexed)
	{
		if(indexed && destroy)
		{
			IndexedEntry *entry = (IndexedEntry *)arena[index];
			delete entry->obj;
		}
		delete arena[index];
	}
	if(toMove)
		memmove(&arena[index], &arena[index + 1], toMove);
	usedCount--;
}

//   Reasigna el area de memoria.
void ::ArrayList::RemapArena(int newLen)
{
#ifdef MEMDEBUG
	char debname[100];
	sprintf(debname, "RA-%s", owner);
#else
	char *debname = "ArrayList:RemapArena";
#endif

	void **newArena = (void **) MALLOC(newLen * sizeof(void *), debname);

	if(newArena == NULL)	AbnormalEnd("Sorry. No memory for ArrayList");
	if(newLen < usedCount)	usedCount = newLen;
	if(newLen > 0)          memset(newArena, 0, newLen * sizeof(void *));
	if(usedCount)			memmove(newArena, arena, usedCount * sizeof(void *));
	if(arenaLen)			FREE(arena);

	arena = newArena;
	arenaLen = newLen;
}

//   Elimina el contenido de la lista.
void ::ArrayList::Clear()
{
	if(destroy || indexed)
	{
		for(int i = 0; i < usedCount; i++)
		{
			void *entry = arena[i];
			if(destroy && indexed) delete ((IndexedEntry *)entry)->obj;
			delete entry;
		}
	}

	usedCount = 0;
	indexed = false;
}

// Agrega un elemento indexado
int ::ArrayList::Add(int key, void *obj)
{
	if(!indexed && (usedCount > 0)) throw "No puede mezclar items indexados y no indexados";
	IndexedEntry *entry = new IndexedEntry(key, obj);
	indexed = true;
	return Add(entry);
}

// Busca un elemento indexado.
void *::ArrayList::Find(int key)
{
	if(!indexed && usedCount != 0) throw "La lista con contiene items indexados";
	for(int i = usedCount - 1; i >= 0; i--)
	{
		IndexedEntry *entry = (IndexedEntry *)arena[i];
		if(entry->key == key)
			return entry->obj;
	}
	return NULL;
}

// Busca la posicion de un determinado item indexado.
int ::ArrayList::FindIndex(int key)
{
	if(!indexed && usedCount != 0) throw "La lista con contiene items indexados";
	for(int i = usedCount - 1; i >= 0; i--)
	{
		IndexedEntry *entry = (IndexedEntry *)arena[i];
		if(entry->key == key)
			return i;
	}
	return -1;
}