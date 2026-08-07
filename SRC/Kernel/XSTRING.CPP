//   xString.cpp
//   Implementa la clase xString.
//
#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Constructor vacio. Destructor.
xString::xString()
{
	memset(this, 0, sizeof(this));
}

xString::~xString()
{
	if(aLen) FREE(pos);
}

//   Retorna un puntero a la string ubicada. Si no la hubiere,
//   Retorna una string vacia (nunca el valor NULL).

char *xString::Data()
{
	if(!pos) return "";
	return pos;
}

//   Copia una cString a una xString.
xString& xString::operator= (char *s)
{
	if((aLen < strlen(s)) || !aLen)
	{
		if(aLen) FREE(pos);
		aLen = strlen(s);
		pos = (char *) MALLOC(aLen + 1, "xstring:op_eq");
	}
	STRCPY(pos, s);
	return *this;
}

//   Copia una xString a otra.
xString& xString::operator= (xString *s)
{
	if(aLen < strlen(s->pos))
	{
		if(aLen) FREE(pos);
		aLen = strlen(s->pos);
		pos = (char *) MALLOC(aLen + 1, "xstring:op_eq2");
	}
	STRCPY(pos, s->pos);
	return *this;
}

//  Agrega una cString a una xString.
xString& xString::operator+ (char *s)
{
	size_t tLen;
	char *tPos;

	tLen = (pos? strlen(pos): 0) + strlen(s);
	if(aLen < tLen)
	{
		tPos = pos;
		aLen = tLen;
		pos = (char *) MALLOC(aLen + 1, "xString:op_add");
		if(tPos)
		{
			STRCPY(pos, tPos);
			FREE(tPos);
		}
		else
			STRCPY(pos, "");
	}
	STRCAT(pos, s);
	return *this;
}

//  Agrega una xString a otra.
xString& xString::operator+ (xString *s)
{
	size_t tLen;
	char *tPos;
	
	tLen = (pos? strlen(pos): 0) + (s->pos? strlen(s->pos): 0);
	if(aLen < tLen)
	{
		tPos = pos; 
		aLen = tLen;
		pos = (char *) MALLOC(aLen + 1, "xstring:op_ad2");
		if(tPos)
		{
			STRCPY(pos, tPos);
			FREE(tPos);
		}
		else
			STRCPY(pos, "" );
	}
	if(s->pos)
		STRCAT(pos, s->pos);
	return *this;
}

//  Pone una xString en mayusculas/minusculas.
xString& xString::Lower()
{
	if(!pos)
	{
		aLen = 0;
		pos = (char *) MALLOC(1, "xstring:lower");
		STRCPY(pos, "");
	}
	_strlwr(pos);
	return *this;
}

xString& xString::Upper()
{
	if(!pos)
	{
		aLen = 0;
		pos = (char *) MALLOC(1, "xstring:upper");
		STRCPY(pos, "");
	}
	_strupr(pos);
	return *this;
}

//   Retorna el largo de la string almacenada.
size_t xString::Len()
{
	if(!pos) return 0;
	return strlen(pos);
}
/////////////////////////////  class xList ////////////////////////////////
//   Constructor / Destructor.
xList::xList()
{
	memset(this, 0, sizeof(*this));
}

xList::~xList()
{
	xListElem *x, *n;

	for(x = first; x; x = n)
	{
		n = x->next;
		Remove(x);
	}
}

//   Agrega elementos al comienzo de la lista.
void *xList::AddTop(void *s)
{
	xListElem *e = new xListElem();

	//e = (xListElem *) MALLOC(sizeof(xListElem), "xList:AddTop");
	e->data = s;
	e->next = first;
	e->father = this;
	first = e;
	e->IAlloced = 0;
	walkAct = e;
	return e->data;
}

void *xList::AddTop(size_t datalen)
{
	char *x;
	void *ret;

	x = (char *) MALLOC(datalen, "xList:AddTop2");
	memset(x, 0, datalen);
	ret = AddTop(x);
	walkAct->IAlloced = 1;
	return ret;
}

//   Agrega al pie de la lista.
void *xList::AddBottom(void *s)
{
	struct xListElem *e, *x;

	e = (xListElem *) MALLOC(sizeof(xListElem), "xList:AddBottom1");
	e->data = s;
	e->next = 0;
	e->IAlloced = 0;
	e->father = this;
	walkAct = e;
	if(!first)
		first = e;
	else
	{
		for(x = first; x->next; x = x->next)
			;
		x->next = e;
	}
	return e->data;
}

void *xList::AddBottom(size_t datalen)
{
	char *x;
	void *ret;

	x = (char *) MALLOC(datalen, "xList:AddBottom2");
	memset(x, 0, datalen);
	ret = AddBottom(x);
	walkAct->IAlloced = 1;
	return ret;
}

void xList::Remove(void *xdata)
{
	xListElem *x, *y;

	for(x = first; x && x->data && (x->data != xdata); x = x->next)
		;
	if(x)
	{
		if(x->IAlloced) FREE(x->data);
		if(first == x)
			first = x->next;
		else
		{
			for(y = first; y && (y->next != x); y = y->next)
				;
			y->next = x->next;
			FREE(x);
		}
	}
}

void xList::GoTop()
{
	walkAct = first;
	sent = 1;
}

void xList::GoBottom()
{
	xListElem *x;

	for(x = first; x; x = x->next)
		;
	walkAct = x;
	sent = 0;
}

int xList::MoreInList()
{
	if(walkAct->next) return 1;
	return 0;
}

int xList::IsFirst()
{
	if(walkAct == first) return 1;
	return 0;
}

void *xList::GetPtr()
{
	if(!walkAct) return 0;
	return walkAct->data;
}

void *xList::GetItem(int index)
{
	int i = 0;
	for(walkAct = first; walkAct != NULL; walkAct = walkAct->next)
		if(i++ == index)
			return walkAct->data;
	GoTop();
	return (walkAct != NULL)? walkAct->data: NULL;
}

void xList::Skip()
{
	xListElem *x;

	if(sent)			// Top-->Bottom
	{
		if(walkAct) walkAct = walkAct->next;
	}
	else				// Bottom-->Top
	{
		if(walkAct == first)
			walkAct = 0;
		else
		{
			for(x = first; x && (x->next != walkAct); x = x->next)
				;
			walkAct = x;
		}
	}
}

int xList::ItemsCount()
{
	int c = 0;
	for(xListElem *x = first; x != NULL; x = x->next)
		c++;
	return c;
}

