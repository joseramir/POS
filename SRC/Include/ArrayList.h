//   
//   ArrayList.h
//   Encabezamiento y definicion de clase para ArrayList.
//

#ifndef ARRAYLIST_DEFINED
#define ARRAYLIST_DEFINED

class ArrayList
{
private:
	void **arena;									// Lugar actualmente ubicado.
	int arenaLen, usedCount;						// Punteros internos.
	bool destroy;									// Si debe liberar los ptros contenidos
	bool indexed;									// Si las entradas son indexadas
	void RemapArena(int newLen);					// Reubica el area de arena.
	char *owner;

public:
	ArrayList(bool dstroy, char *own);				// Constructor
	ArrayList(char *str, char *own);				// Separa y agrega
	~ArrayList(void);								// Destructor
	int Count(void);								// Cantidad de elementos contenidos.
	int Add(void *obj);								// Agrega un elemento cualquiera, por su ptr.
	int Add(char *s, bool trim);					// Agrega una unica string
	void AddSplitted(char *s, char c);				// Rompe una string y la agrega
	void RemoveAt(int index);						// Quita un elemento dado su indice.
	void *Item(int index);							// Devuelve el ptr a un elemento dado su indice.
	int Key(int index);								// Si la lista es indexada, devuelve la clave.
	char *ItemString(int index);					// Devuelve un ptr como una string
	void SetItem(int index, void *obj);				// Cambia el contenido de un item.
	void Clear(void);								// Elimina el contenido (sin eliminar la arena)

	int  Add(int key, void *obj);					// Agrega un elemento dando una clave para el elemento
	void *Find(int key);							// Busca el elemento dada la clave del mismo.
	int  FindIndex(int key);						// Busca la posicion del item cuya key se pasa
	void Sort(										// Ordena la lista
		int (*func)(const void *e1, const void *e2));
};

#endif	// ARRAYLIS_DEFINED