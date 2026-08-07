/*   Wsup.h    - Soporte de Video y Ventanas   */
#pragma once
#define NEGRO     0                 // Codigos de colores normales.
#define AZUL      1
#define VERDE     2
#define CELESTE   3
#define ROJO      4
#define VIOLETA   5
#define MARRON    6
#define GRISCLARO 7
#define GRIS      8
#define AZULF     9
#define VERDEF   10
#define CELESTEF 11
#define ROJOF    12
#define VIOLETAF 13
#define AMARILLO 14
#define BLANCO   15

#define NOBORDER  0             // Ventana sin borde.
#define BORDERSINGLE 1          // Borde de linea simple.
#define BORDERDOUBLE 2          // Borde de lina doble.
#define BORDEPANEL 3            // Borde grueso de linea llena.

//  'shape'

#define NOCURSOR   0            // Sin cursor en pantalla.
#define SNGCURS    1            // Cursor de linea subrayada.
#define BLKCURS    2            // Cursor de bloque lleno.

#define COLBORDER  0            // En el esquema, color del borde.
#define COLTITLE1  1            // Color del titulo, cuando la wind esta act
#define COLTITLE2  2            // Color del titulo, cuando la wind NO esta
#define COLMAIN    3            // Color principal, cuando no hay otro.
#define COLFOCUS   4            // Get con el foco.
#define COLNOFOCUS 5			// Get sin el foco.

struct rect {                   // Utilizada para especificar un rectangulo.
	int sy, sx, ey, ex;         // Coord superior izq e inferior derecha.
};

extern int wSupEnabled, wact;
extern unsigned short ucolor;
extern int UseVideoCopy;

/*   Prototipos  */
void wRelease(int);
int  InitializeWSup(void);
void ReleaseWSup(void);
void LoadActualScreen(unsigned short *);
void wLocate(int y, int x);
void wChgShape(int shape);
void wPstr(char *s);
int  wPch(char c);
void wCls(void);
void wCls(char relleno, short color);
int  wDefine(int StartY, int StartX, int EndY, int EndX, int Border,
             int Cursor, short *Esquema, char *Title, char *Footer);
void wScroll(void);
void wDeactivate(int, int);
void wDrawBox(struct rect *r, int style);
void wPutFront(int wh);
void wPrintSep(int);
int  wSetColor(int color);
int  wEsqSetColor(int esqColor);
void RefreshScreen(void);
void wActivate(int);
int  wClientWidth(void);
int  wClientHeigth(void);
char *wTitle(void);
void wSetBorder(int y, int x, int ch);
void FCls(void);				// Acceso directo
void FCursorShape(int);
void RefreshScreen(void);
void FCursorShape(int);
void FCursorPos(int, int);
void FWrtChar(unsigned short);
void FCursorAdvance(void);
