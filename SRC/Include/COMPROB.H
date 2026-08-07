//
//   Comprob.h
//   Definicion de variables y clases para imprimir comprobantes.
//

#ifndef COMPROB_INCLUDED_
#define COMPROB_INCLUDED_

//   Variables generales de la disposicion del listado.
extern int    gCopia, gRenglon, gConst, gVari;
extern char   blank12[];
extern char   g_Codigo[], g_Descrip[];
extern char  *g_DIva[];
extern double g_Cantidad, g_UnitarioN, g_UnitarioT, g_RtotalN, g_RtotalT;
extern double g_TransporteN, g_TransporteT, g_TotalN, g_TotalT, g_Iva[];
extern double g_Total, g_RecargosP, g_RecargosM, g_DescuentosP, g_DescuentosM;

//   Prototipo de la funcion que EXTRAERA los datos de un renglon.
//   Volvera con 0 si NO PUDO extraer mas datos, probablemente porque
//   no quedan mas renglones. Volvera con 1 si se trajo correctamente
//   un renglon.
//
void ResetRengInfo(void);
int GetRengInfo(void);
int getVar(char *);
int getConst(char *);

//   Estructura para almacenar las constantes que se cargan desde
//   el archivo 'const.ini'
struct const_ {
	char *name;					// Nombre.
	int  len;					// Largo definido.
	char *value;				// Valor de la constante.
	const_ *next;				// Siguiente en la lista.
};

//   Clase para manejar el impresor.
class pHand {
private:
	int  h;						// Handler utilizado con el DOS.
	char *bold,  *_bold;		// Secuencia para letra remarcada.
	char *under, *_under;		// Secuencia para letra subrayada.
	char *width, *_width;		// Secuencia para letra ancha.
	char *cond,  *_cond;        // Secuencia para letra condensada.
	char *init,  *release;		// Secuencia de inicializacion, reposicion.
	char *port;					// Puerto a utilizar.
	int  nextpCtrl;				// Comando a desactivar.
	struct {
		int online: 1;			// A TRUE si todo va OK todavia.
		int nextprint: 1;		// Esta activo el netpCtrl.
	} flags;

public:
	pHand *next;				// Siguiente definicion.
	char *name;					// 'nombre' del impresor.

	pHand();
	~pHand();
	
	int  LoadDef(char *secc);	// Carga la definicion de una seccion.
	int  OpenPrint(void);		// Abre y prepara la impresora.
	void ClosePrint(void);		// Cierra la impresion.
	void Print(char *);			// Imprime una secuencia.
	void SendCtrl(int);			// Cambia el estado (ver define's).
};

//  Defines de los posibles controles para la impresora.
#define   _COND_    0x0040
#define _U_COND_    0x0080
#define   _BOLD_	0x0001
#define _U_BOLD_	0x0002
#define   _UNDER_	0x0004
#define _U_UNDER_	0x0008
#define   _WIDTH_	0x0010
#define _U_WIDTH_	0x0020
#define _NEXT_PRINT 0x8000		// Control valido solo para la siguiente.

struct secc_ {
	int y, x;					// Coordenadas.
	char *dato;					// Variable, constante o texto a imprimir.
	int  atrib;					// Atributo a colocar.
	int  len;					// Len sobreescrito.
	secc_ *next;				// Siguiente definicion en la seccion.
};

class defc {
private:
	char *printer;				// NOMBRE del printer.
	pHand *prn;					// pHand del impresor, una vez iniciado.
	int  copias;				// Cantidad de copias DEFAULT.
	int  mcopias;				// Cantidad minima de copias (999 no cambia).
	int  alto;					// Cantidad de renglones de una hoja.

	//   Secciones del informe.
	secc_ *titulo;
	secc_ *encabezado;
	secc_ *renglon;
	secc_ *piecont;
	secc_ *piefin;
	int a_titulo, a_encabezado, a_renglon, a_piecont, a_piefin;

	//   Funciones aplicadas (privadas).
	int LoadSecc(char *, secc_**);	// Carga una seccion en memoria.
	void PrintSecc(secc_ *secc, int alto);

public:
	char *name;
	defc *next;					// Para enganchar las listas.

	defc();
	~defc();
	int LoadDef(char *fn);		// Carga la definicion de un comprob.
	int Print(void);			// Imprime un comprobante.
};

pHand *LoadPrnDef(char *name);
defc *LoadComprobDef(char *name);

#endif COMPROB_INCLUDED_
