//   xString.h

#ifndef XSTRING_INCLUDED
#define XSTRING_INCLUDED
class xString {
private:
	size_t aLen;	// Largo actual del BUFFER (NO de la string).
	char *pos;		// Buffer asignado.
	
public:
	xString();
	~xString();
	xString& operator= (char *);
	xString& operator= (xString *);
	xString& operator+ (char *);
	xString& operator+ (xString *);
	xString& Lower(void);
	xString& Upper(void);
	char *Data(void);
	size_t   Len(void);
};

struct xListElem {
	xListElem *next;
	class xList *father;
	int IAlloced;
	void *data;
};

//   
class xList {
private:
	xListElem *first, *walkAct;
	int sent;

public:
	xList();	//   Constructor / Destructor de la lista.
	~xList();
	
	//   Funciones para crear/borrar elmentos de la lista.
	void *AddBottom(size_t datalen); // Incopora al Final
	void *AddBottom(void *data);
	void *AddTop(size_t datalen);	 // Incorpora al principio.
	void *AddTop(void *data);
	void Remove(void *data);		 // Remueve un elemento.

	//   Funciones para caminar por la lista.
	void GoTop(void);				//  Prepara p/caminar de top a bottom
	void GoBottom(void);			//  Prepara p/caminar de bottom a top
	void Skip(void);				//  Busca el siguiente elemento.
	void *GetPtr(void);				//  Devuelve ptr al actual (0 si no).
	int  MoreInList(void);			//  TRUE si hay mas despues del actual.
	int  IsFirst();					//  TRUE si es el primero de la lista.
	void *GetItem(int index);		//  Devuelve el ptr de un item dado.
	int  ItemsCount();				//  Retorna la cantidad de items contenidos en la lista.
};
#endif
