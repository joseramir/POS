//
//   Implementa los prototipos y variables globales de distintos
//   objetos que se colocan en pantalla.
//

#pragma once
#include "db.h"
#include "xString.h"
#include "arraylist.h"

//////////////////////////////////////////////////////////////////
// aget.cpp
class aget 
{
private:
	int y, x;								// Coordenadas donde se ubica.
	int frmtflag;							// Flag para filtro de edicion.
	int len, dec;							// Largo de la parte entera y decimal.
	int pickColor;							// Si es un color picker.
	char *buff, *tbuff;						// Buffer que se esta editando.
	int (*when)(char *nval, void *bag);		// Funcion de validacion a la entrada.
	int (*valid)(char *nval, void *bag);	// Funcion de validacion a la salida.
	dbf *alias;								// Base de referencia.
	char *fld;								// Nombre del campo.

public:
	struct flags_							// Banderas de edicion.
	{
		int BuffFree:1;						// El buffer fue ubicado y DEBE liberarse.
		int used:1;							// Estructura usada. DEBE liberarse.
		int clrNext:1;						// Todo esta seleccionado.
		int fldlnk:1;						// Enganchado a campo.
		int cPicker:1;						// Es un color picker.
		int enabled:1;						// Si esta habilitado para edicion o es solo readonly
		int isButton:1;						// Si funciona como un boton
	} flags;

	aget(class EditWindow *_container, int _y, int _x, char *_fld, dbf *_alias, 
		void *bag,
		int (*_when)(char *nval, void *bag), 
		int (*_valid)(char *nval, void *bag));
	aget(class EditWindow *_container, int _y, int _x, int _frmtflag, int _len, int _dec, char *_buff, 
		void *bag,
		int (*_when)(char *nval, void *bag), 
		int (*_valid)(char *nval, void *bag));
	~aget();

	aget *next;								// Siguiente en la lista.
	void *bag;								// Contenedor de datos para el usuario
	class EditWindow *container;
	void get(int _y, int _x, int _frmtflag, int _len,		// Activa un elemento.
		void *_bag,											// Valor del usuario para las funciones
		int _dec, char *_buff, 
		int (*_when)(char *nval, void *bag),				// Funcion llamada ANTES de editar el campo
		int (*_valid)(char *nval, void *bag));				// Funcion llamada DESPUES de editar el campo

	void getf(int _y, int _x, char *fldName, dbf *alias,	// Activa un campo.
		void *_bag,											// Valor del usuario
		int (*_when)(char *nval, void *bag), 
		int (*_valid)(char *nval, void *bag));
	
	void show(void);										// Actualiza Control.
	void clear(void);
	int  anima(void);
};

class EditWindow
{
	// Primer GET de la lista y puntero al GET que tiene el foco.
	aget *first, *focus;
	
	// Ventana en la que se anima el edit.
	int animaWind;

public:
	// Agrega un get linkeado con un campo en una tabla.
	EditWindow(int hWin);
	~EditWindow();
	inline int Window() { return animaWind; }
	inline aget *Focus() { return focus; }
	inline void SetFocus(aget *newFocus) { focus = newFocus; }
	aget *AddGet(int _y, int _x, char *_fld, dbf *_alias, void *_bag, int (*_when)(char *, void *), int (*_valid)(char *, void *));
	aget *AddGet(int _y, int _x, int _frmtflag, int _len, int _dec, char *_buff, void *_bag, int (*_when)(char *, void *), int (*_valid)(char *, void *));
	int AnimaGets(int);					// Lanza la edicion.
	void ShowGets(void);				// Refresca toda la pantalla.
	void ClearGets(void);				// Limpia la lista de 'gets' animados.
};

#define TEXT_LEFT_ALIGNED    0			// Texto alineado.
#define NUMBER_LEFT_ALIGNED  1			// Nro alineado.
#define LOGICAL_VALUE        2			// Valor logico.
#define NUMBER_RIGHT_ALIGNED 3			// Nro alineado.
#define MONEDA_RIGHT_ALIGNED 4			// Moneda (con decimales)
#define MAKE_COLOR_PICKER    5			// Construye un color picker.
#define PASSWORD_MASK		 6			// Enmascara para ingresar passwords.
#define TEXT_READ_ONLY		 7			// Un texto readonly. La funcion VLD es llamada si se pulsa ENTER en el textbox.

/////////////////////////////////////////////////////////////////////////////
//  xBrowse.cpp

class xBrowseField {
private:
	xString fldName;						// Nombre del campo.
	xString tit;							// Titulo a displayar.
	int lenTit, lenFld, decFld;				// Largos predeterminados
	dbf *db;								// Alias para la variable.	
	xString **brwData;						// Array de puntero a los datos.
	int cRows;								// Cantidad de filas de datos.
	char *editBuff;							// Buffer con los datos a editar.

public:
	size_t cLen, dLen;						// Ancho de la columna.
											// Ojo - Si se toca cLen, usar Refresh.
	int x;									// Posicion del displayeo.
	class xBrowse *brw;						// Referencia al contenedor.	

	xBrowseField();
	~xBrowseField();
	char dataType;							// 'C' o 'N' - Tipo de datos validos.
	bool (*ValidFunc)(int, char *, char *);	// Funcion de validacion.
	bool ReadOnly;							// No deja que el campo se edite.
	void SetField(dbf *udb, char *fldName);	// Cambia el campo relacionado.
	void SetTitle(char *fldName);			// Cambia el titulo de la columna.
	char *GetFieldName(void);				// Retorna el nombre del campo
	void RefreshFromDb(int row, int flag);	// Refresca los datos desde la DB
	void sRefresh(void);					// Refresca el displayeo de la columna.
	void resetDataPtr(int nSize);			// Cambia el largo del array.
	void setData(int n);					// Establece los datos para una fila.
	void DisplayData(int n, int att);		// Displaya un campo.
	int  Edit(int n);						// Edita los datos del campo.
};

class xBrowse :public xList {
	friend class xBrowseField;

private:
	int hWnd;						// Handler a la window.
	dbf *db;						// Base relacionada.
	int WndRows;					// Filas que contiene una hoja.
	unsigned long aRec;				// Record de la primera fila.
	unsigned long xRecnos[50];		// Array con los recnos()'s asigandos.
	int useRow;						// Fila en pantalla que se esta editando.
	int topRecord, bottomRecord;	// Para que el browse no se desborde
	char FastFind[80];				// Para búsqueda rápida.
	struct flags_ {
		int useLocator: 1;			// Es 'locator' y no 'browse'.
		int IDefTheWindow: 1;		// Si yo defini la ventana.
		int CursorAtBottom: 1;		// El record cursor va al final.
	} flags;

public:
	xBrowseField *xBrowseF(dbf *udb, char *fldName, char *fldTit);
	xBrowseField *xBrowseF(dbf *udb, char *fldName, char *fldTit, bool readOnly);
	xBrowseField *xBrowseF(dbf *udb, char *flcName, char *fldTit, char dataType);
	xBrowseField *xBrowseF(dbf *udb, char *fldName, char *fldTit, char dataType, 
		bool (*Valid)(int row, char *fldName, char *nData));
	int  xBrowseAnimate(int hWnd, dbf *udb, int flgLocator, int Wait, int curPos, short *eCol);
	int  xBrowseRemoteKey(int ch);
	void xBrowseMovePtr(int);
	void DisplayRecord(int row, int flag);
	void sRefresh(void);
	void dRefresh(void);
	void RefreshDbPointers();
	xBrowse();
	~xBrowse();
};

///////////////////////////////////////////////////////////////////////
// color.cpp
short MakeColorPicker(short aColor);

/////////////////////////////////////////////////////////////////////////////
// elige.cpp
#define CHOOSE_ALIGN_LEFT	0
#define CHOOSE_ALIGN_CENTER 1
#define CHOOSE_ALIGN_RIGHT  2

#ifdef POSXP
#include "eligeform.h"
using namespace pos;
ref class StaticChoose { public: static EligeForm ^ef; };
#endif

class Choose 
{
private:
	::ArrayList *lines;
	int estilo, adicX, adicY, maxAncho;
	int (*AtnFunc)(int);
	char *tit, *pie;
	int wHand, pAncho, pAlto, actual;

#ifndef POSXP
	void DisplayLine(int cual);	// Displaya una de las lineas.
#endif

public:
	//   Constructor.
	Choose(char *tit, char *pie, int adicX, int adicY, int estilo, int (*AtnFunc)(int));
	
	~Choose(void);										// Destructor.
	void Refresh(void);									// Refresca o muestra los datos en la window.
	void Clear(void);									// Elimina toda la lista contenida.
	int  Add(char *msg);								// Agrega una linea
	void Remove(int cual);								// Elimina una linea
	int  Anima(void);									// Anima el elige
	void ChangeTit(int, char *);						// Cambia el titulo de la opción.
	char *GetText(int index);							// Devuelve el texto de una opcion dada.
	void SetOption(int index, char *t, bool enabled);	// Cambia el texto de una opcion dada.
	int  Count(void);									// Cantidad de opciones definidas.

#ifndef POSXP
	int  WindowHandle(void);	// Retorna el handler de la window utilizada
	int  MaxAncho(void);		// Retorna el maximo ancho entre todas las opciones
#endif
};

// Posibles tipos de datos para EditItem
#define TIPO_CUALQUIERA 0
#define TIPO_ENTERO		1
#define TIPO_MONEDA		2
#define TIPO_NUMREF		3

// Clase generica para editar items en pantalla de edicion general.
class EditItem
{
	char label[60];
	int dataType;
	int ancho;

public:
	char dato[60];
	char help[200];
	bool valido;
	bool enabled;
	char *(*validFunc)(EditItem *item, char *nueDato);

	char *VerLabel();
	int VerTipo() { return dataType; }
	int VerAncho() { return ancho; }
	void CambiarAncho(int v) { ancho = v; }
	void CambiarDatoIngresado(char *_dato) { STRCPY(dato, _dato); }
	void CambiarTipo(int nuevoTipo) { dataType = nuevoTipo; }
	char *ValidarDatoIngresado(char *_nueDato, bool cambiar);
	void CambiarLabel(char *_label);
	int  Medir();

	EditItem(char *_label);
	~EditItem();
};

#define FONT_GRANDE 0
#define FONT_MEDIANO 1
#define FONT_CHICO 2

// Clase que describe una pantalla generica de entrada de informacion
class GenForm
{
	char *titulo;
	::ArrayList *items;
	int fontSize;
	EditWindow *ew;

public:
	char *VerTitulo();
	void CambiarTitulo(char *tit);
	void CambiarFontSize(int newSize) { fontSize = (newSize < 0 || newSize > 2)? 0: newSize; }
	::ArrayList *Items() { return items; }
	bool Animar();
	void Refresh();

	GenForm(char *tit);
	~GenForm();
};