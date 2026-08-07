/*
**   Wsup.c
**   Modulo Principal - Soporte de ventanas.
**
**   El manejo principal se efectua utilizando el array 'scrmap', en el que
**   cada byte se corresponde con un par caracter/atributo en la pantalla.
**   En 'scrmap' se indica a que ventana le corresponde la posicion de pan-
**   talla en cuestion.
**
**   Correspondientemente, con cada ventana creada, se reserva un area de
**   memoria donde se almacena una copia exacta de lo que se va displayan-
**   do EN LA VENTANA. Esta copia se utiliza para refrescar la ventana prin-
**   cipal. En esta copia se contienen los bordes, si la ventana los posee.
**
**   Todas las coordenadas se expresan a partir de 0. En caso de ventanas
**   con borde, las coordenadas se expresan a partir de 0 y se corresponden
**   con el area en blanco dejada para edicion. (area cliente)
**
**   Se modifico para funcionar en 32 bits modo plano. Actualmente,
**   solo funciona como una aplicacion 'DPMI Client Compliant'.
**   A partir de este momento, la variable 'VideoSel' pasa a funcionar como
**   un selector LDT para acceso directo a la ram de video.
**
**   De acuerdo con lo descripto en el punto anterior, algunas funciones se 
**   reescribieron en assembler a fin de poder evitar tener que cargar un re-
**   gistro de segmento con el valor del selector, cada vez que se desea acceso
**   al video. La operación de carga de selectores en los registros de segmen-
**   tos DEMORA MUCHISIMO y se desconoce como operan los distintos procesadores 
**   cuando se carga un segmento con el mismo valor que ya tenia. Dado que el 
**   programa corre en el modo 'flat', es bastante probable que el valor del 
**   registro ES no se toque para nada (a menos que se llame a alguna funcion
**   de MSDOS en el medio). Por esto, antes de instalar el selector de video 
**   en ES primero compara, y si ya esta cargado y si es asi, entonces, no lo 
**   vuelve a cargar.
*/

#include "stdafx.h"

//   Estamos compilando para windows 98, con Visual C++ 6.0?
#ifdef _MSC_VER
#if _MSC_VER >= 1000
	#include <windows.h>
	#include <wincon.h>				// Funciones de consola.
	#define _W98_					// Para el resto del programa.	
#endif
#endif

//   Los codigos de colores se sacaron de aqui, ya que se utilizan
//   los definidos en 'wsup.h'.
#define MAXWINDS 32             // Maxima WINDOWS definidas.

#define NOBORDER  0             // Ventana sin borde.
#define BORDERSINGLE 1          // Borde de linea simple.
#define BORDERDOUBLE 2          // Borde de lina doble.
#define BORDEPANEL 3            // Borde grueso de linea llena.

#define NOCURSOR   0            // Sin cursor en pantalla.
#define SNGCURS    1            // Cursor de linea subrayada.
#define BLKCURS    2            // Cursor de bloque lleno.

#define COLBORDER  0            // En el esquema, color del borde.
#define COLTITLE1  1            // Color del titulo, cuando la wind esta act
#define COLTITLE2  2            // Color del titulo, cuando la wind NO esta
#define COLMAIN    3            // Color principal, cuando no hay otro.

char tstr[100];                 // Para hacer CLS
int wSupEnabled, UseVideoCopy;
short STARTVIDEOMEM = (short) 0xb800;
int videoType = 1;

#pragma pack(1)
struct wnd {
	struct rect wlim;           // Posiciones de los bordes.        NO MOVER
	int maxX, maxY;             // Extremos rapidos.                NO MOVER
    unsigned short *image;      // Copia del contenido.             NO MOVER
	short colesq[16];           // Esquema de colores disponibles.
	int curX, curY;             // Posicion actual del cursor.
	unsigned short ucolor;      // Color actual.
	int border;                 // Tipo de border
	int cursor;                 // Modo del cursor.
	int cliX, cliY;				// Ancho y alto del area cliente.
	char title[80], footer[80]; // Titulo y pie de la ventana.
	struct {
				int EnUso:1;    // Si el handler no esta disponible.
				int NoShow:1;   // Si NO debe actualizar el desktop
				int Drawed:1;   // Si necesita ser redibujada.
				int Skipbrd:1;  // Evita se destrocen los bordes.
	} fg;

};
wnd wnds[MAXWINDS];
wnd *words[MAXWINDS];
#pragma pack()

int wstack;         // Indica las ventanas presentes en pantalla

#ifdef PANTACOLOR
short defesq[16] = {            // Esquema default de colores
	{ AZUL * 16 + MARRON },     // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ AZUL * 16 + BLANCO },     // Texto contenido.
    { NEGRO * 16 + MARRON },    // Color del fondo cuando usa relleno.
	{ ROJO * 16 + BLANCO },		// Color para titulos resaltados.
};
#else
short defesq[16] = {            // Esquema default de colores
	{ NEGRO * 16 + BLANCO },    // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ NEGRO * 16 + BLANCO },    // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno.
	{ BLANCO * 16 + NEGRO },
};
#endif

/*   Variables globales a todo el modulo  */

char *scrmap;                   // Mapa de distribucion de ventanas.
int  wdefs, wact;               // Cantidad de ventanas definidas.
struct wnd *w;                  // Ventana actualmente activa.
int ActCursorShape;             // Figura actual del cursor.
int ActCursorX, ActCursorY;     // Posicion actual del cursor.
unsigned short CRTCtrl, CRTData;// Registros del controlador de CRT.
unsigned short ucolor;          // Color actual para escribir.
short fillchar = '°';

#ifdef _W98_					// Variables para las funciones consola.
HANDLE hCon;
CONSOLE_SCREEN_BUFFER_INFO ConBuff;	// Dimensiones de la ventana.
COORD CeroCoord;					// {0, 0}
unsigned short xUcolor;				// Ultimo color seteado.
char *scrstart;					// Pantalla virtual. (caracteres).
WORD *attstart;					// Pantalla virtual. (atributos).
#else
short VideoSel;                 // Contendra el selector al video-buff.
unsigned short *scrstart;
extern "C" short _get_video_selector(short segm);
#endif

//   No existe en la libreria. Setea las primeras 'count' words del buffer
//   'buff' al valor entero 'value'.
short *memwset(short *buff, short value, int count)
{
	short *retV;

	retV = buff;
	while(count--)
			*buff++ = value;
	return retV;
}
	
//   Remueve la memoria ocupada por este modulo  
void ReleaseWSup()
{
	register struct wnd *w;
	register int i;

	for(w = wnds, i=0; i<MAXWINDS; i++)
		if(w->fg.EnUso)
			wRelease(i);
    FREE(scrmap);
}

//   Inicializa el módulo.
//   Bajo Windows, inicializa las funciones de consola.
//   Bajo modo X, inicializa los selectores para acceso al video buffer.
int InitializeWSup()
{
     #ifndef _W98_
     short error;
     #endif

	 switch(videoType)
	 {
			case 1:                 // VGA Color
                 STARTVIDEOMEM = (short) 0xb800;
			     CRTCtrl = 0x3d4;           // Registros de control de CRT
	             CRTData = 0x3d5;
				 break;
			case 2:                 // VGA Mono
                 STARTVIDEOMEM = (short) 0xb000;
				 CRTCtrl = 0x3d4;           // Registros de control de CRT
			     CRTData = 0x3d5;
				 break;
			case 3:                 // CGA
                 STARTVIDEOMEM = (short) 0xa000;
				 CRTCtrl = 0x3b4;           // Registros de control de CRT
				 CRTData = 0x3b5;
				 break;
			default:                // Hercules
                 STARTVIDEOMEM = (short) 0xb800;
				 CRTCtrl = 0x3b4;           // Registros de control de CRT
				 CRTData = 0x3b5;

		}

     scrmap = (char *) MALLOC(25 * 80, "InitializeWSup1");          // Memoria para el mapa.
     if(!scrmap)
		AbnormalEnd("Sin memoria para ventanas");
	 memset(scrmap, 0, 25 * 80);            // Inicializa el area.

	 // Las siguientes variables se definen para acceso directo al video
	 // VGA.
	 #ifdef _W98_
     scrstart = (char *) MALLOC(25 * 80, "InitializeWSup2");
     attstart = (WORD *) MALLOC(25 * 80 * sizeof(WORD), "InitializeWSup3");
	 if(!scrstart || !attstart)
		 AbnormalEnd("Sin memoria para pantalla virtual.");
	 #endif

	 //   Las siguientes variables afectan al hardware pero son portables.
	 ActCursorShape = 0xff;                 // Fuerza ajuste de forma.
	 ActCursorX = ActCursorY = 0xff;        // Fuerza ajuste de posicion.
	 ucolor = (MARRON * 16 + CELESTE) * 256;  // Color Actual.

	 //  En windows 98, prepara el acceso a la consola.
	 #ifdef _W98_
	 hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	 if( !GetConsoleScreenBufferInfo(hCon, &ConBuff))
		AbnormalEnd("Programa compilado para correr bajo Windows 98/NT.\n");
	 SetConsoleTitle("POS - Versión 3.0 - Sistema de Registración.");
     #else
     #ifdef _CONSOLE
     _asm {
                mov ax, word ptr STARTVIDEOMEM
                call _get_video_selector
                mov VideoSel, ax
     }
     #endif
	 scrstart = 0;
     #endif

	 //  Define la ventana principal (desktop).   
	 w = words[0] = &wnds[0];   // Indica activa para salida.
	 wact = 0;                  // Handler de la ventana activa.
	 wstack = 1;                // Hay una ventana en pantalla (minimo).
	 STRCPY(w->title, "POS - Main Window Title");
	 STRCPY(w->footer, "POS - Main Window Footer");
	 w->fg.EnUso = 1;           // Indica handler no disponible
	 w->fg.NoShow = 0;          // Indica que SI actualice el desktop.
	 w->fg.Drawed = 1;          // No necesita ser redibujada.
	 w->fg.Skipbrd = 0;         // No tiene bordes
	 w->wlim.sx = 0;            // Posicion de la ventana.
	 w->wlim.sy = 0;
	 w->wlim.ex = 79;
	 w->wlim.ey = 24;
	 w->cliX = 80;				// Ancho y Alto area cliente.
	 w->cliY = 25;
	 w->border = NOBORDER;      // Sin borde visible
	 w->maxX = 79;              // Para chequeo rapido de overflows.
	 w->maxY = 24;
	 w->curX = 0;               // Posicion inicial del cursor.
	 w->curY = 0;
	 w->cursor = SNGCURS;       // Cursor de raya simple.
     w->image = (unsigned short *) MALLOC(25 * 80 * 2, "InitializeWSup4");
     if(!w->image)
		AbnormalEnd("No hay NADA de memoria para 'wsup'.");
	 if(UseVideoCopy)
		LoadActualScreen(w->image);
	 else
		memwset((short *) w->image, (short)((defesq[4] * 256) + fillchar),
				 25 * 80);
	 memmove(w->colesq, defesq, 16 * sizeof(short));
     ActCursorY = ActCursorX = 0;
     wSupEnabled = 1;
	 return 0;
}

//   La siguiente funcion devuelve el nombre de la ventana que se
//   encuentra activa y con el foco. Este titulo de ventana se de-
//   vuelve en un buffer estatico y convertido en mayusculas.
char *wTitle()
{
	static char tit[100];

	STRCPY(tit, w->title);
	return _strupr(tit);
}

//   Imprime una linea separadora.
void wPrintSep(int donde)
{
	int i, prev;

	prev = wSetColor(w->colesq[0]);
	wLocate(donde, 0);
	for(wLocate(donde, 0), i=0; w->curX < (w->maxX - (w->fg.Skipbrd? 2: 0)); i++)
		wPch('Ä');
	wPch('Ä');
	wSetBorder(donde + 1, 0, '\xc3');
	wSetBorder(donde + 1, 999, '\xb4');
	wSetColor(prev);
}

// Refresca la ventana principal. Utiliza acceso fisico directo asi que
// revisar cuando se incorporen nuevos controladores de video que no
// sean VGA (o soporte para nuevos modos de video, ej VGA50).
// Adaptada para funcionar bajo el modo DOS protegido y/o como aplicacion
// consola de Windows de 32 bits (W95/98/NT).
void LoadActualScreen(unsigned short *x)
{
	unsigned short *iptr, xVal;
	unsigned count;
	#ifdef _W98_
	WORD *att;
	unsigned long cant;
	char *scr;
	#else
	unsigned short *scr;
	#endif

	#ifdef _W98_
	ReadConsoleOutputAttribute(hCon, attstart, 25 * 80, CeroCoord, &cant);
	ReadConsoleOutputCharacter(hCon, scrstart, 25 * 80, CeroCoord, &cant);
	att = attstart;
	#endif
    for(iptr = x, scr=scrstart, count=25*80; count; count--)
	{
		#ifdef _W98_
		xVal = ((*scr++) & 0xff) + (((*att++) * 256) & 0xff00);
		*iptr++ = xVal;
		#else
        // *iptr++ = *scr++;
		_asm {
					mov ax, word ptr [VideoSel]
					mov dx, es
					cmp dx, ax
					je  NoReload
					mov es, ax
		NoReload:	mov ebx, dword ptr [scr]
					mov word ptr cx, es:[ebx]
					inc ebx
					inc ebx
					mov dword ptr [scr], ebx
					mov ebx, dword ptr [iptr]
					mov word ptr [ebx], cx
					inc ebx
					inc ebx
					mov dword ptr [iptr], ebx
		}
		#endif
	}
	return;
}

//   Refresca la ventana principal. Utiliza acceso fisico directo asi que
//   revisar cuando se incorporen nuevos controladores de video que no
//   sean VGA (o soporte para nuevos modos de video, ej VGA50).
//   Adaptada para funcionar bajo el modo DOS protegido y/o como aplicacion
//   consola de Windows de 32 bits (W95/98/NT).
void RefreshScreen()
{
	unsigned short *iptr;
	unsigned count, offset;
	char *map;
	int AbsY, AbsX;
	#ifdef _W98_
	WORD *att;
	unsigned long cant;
	char *scr;
	#else
	unsigned short *scr;
	#endif

	#ifdef _W98_
	att = attstart;
	#endif
    for(map=scrmap, scr=scrstart, count=25*80, AbsY = AbsX = 0;
		count;
		count--, map++)
	{
		struct wnd *w = &wnds[*map];            // Apunta a la ventana recipiente.
		offset = ((AbsY - w->wlim.sy) * (1 + w->maxX)) + (AbsX - w->wlim.sx);
		iptr = w->image + offset;
		#ifdef _W98_
		*scr++ = (char)(*iptr & 0xff);
		*att++ = (WORD)(*iptr / 0xff);
		#else
        // *scr++ = *iptr;
        _asm {
                    mov ebx, dword ptr [iptr]
                    mov cx, word ptr [ebx]
                    mov ebx, dword ptr [scr]
                    mov ax, word ptr [VideoSel]
                    mov dx, es
                    cmp dx, ax
                    je  NoReload
                    mov es, ax
          NoReload: mov word ptr es:[ebx], cx
                    inc ebx
                    inc ebx
                    mov dword ptr [scr], ebx
        }
		#endif
		if(++AbsX >= 80)            // Actualiza coordenadas absolutas
		{
			AbsY++;                 // No puede haber overflow
			AbsX = 0;
		}
	}
	#ifdef _W98_
	WriteConsoleOutputAttribute(hCon, attstart, 25 * 80, CeroCoord, &cant);
	WriteConsoleOutputCharacter(hCon, scrstart, 25 * 80, CeroCoord, &cant);
	#endif
	return;
}

//   La siguiente funcion se utiliza para tratar de conocer si le esta
//   permitido a un programa actualizar directamente el video.
int ChkWindowFront(int whand, int y, int x)
{
	return whand == (int)(*(scrmap + (80 * y)  + x));
}

//  Cambia la forma del cursor, para la ventana actual.
void wChgShape(int shape)
{
	w->cursor = shape;                  // Actualiza estructura de control.
	if(!w->fg.NoShow)                   // Actualiza en pantalla.
		if(ChkWindowFront(wact, w->curY + w->wlim.sy, w->curX + w->wlim.sx))
			FCursorShape(shape);
}

//  Ubica el cursor, en la posicion virtual dentro de la pantalla.
//  El cursor DEBE apuntar a un area visible, sino, vuelve con 1 (error).
void wLocate(int y, int x)
{
	int addbrd;

	addbrd = w->fg.Skipbrd? 1: 0;
	if(w->maxX < x + (2 * addbrd) || w->maxY < y + (2 * addbrd))
			return;
	w->curX = x;
	w->curY = y;
	if(!w->fg.NoShow && w->cursor)        // Si hay que moverlo fisicamente
		if(ChkWindowFront(wact, w->wlim.sy + y, w->wlim.sx + x))
		 {
			FCursorPos(w->wlim.sy + y + addbrd, w->wlim.sx + x + addbrd);
			FCursorShape(w->cursor);      // Cambia la forma si corresponde.
		 } else
		 {
			FCursorShape(NOCURSOR);       // Borra el cursor.
		 }
}

//   Imprime en la ventana actual, en la posicion actual, con el color ac-
//   tual, una cadena terminada en nulo.
void wPstr(char *s)
{
	while(*s)
		if(wPch(*s++))
			return;
}

//   Imprime un caracter ascii. Retorna error si no hay mas lugar en la
//   ventana para imprimir nada.
int wPch(char ch)
{
	unsigned short *pos, put;
	int actCursor, addbrd;

	addbrd = w->fg.Skipbrd? 1: 0;
	put = (ch &  0xff) + ucolor;
	pos = w->image + ((w->curY + addbrd) * (1 + w->maxX)) + (w->curX+addbrd);
	*pos = put;
	actCursor = 0;
	if(!w->fg.NoShow)
		if(ChkWindowFront(wact, w->curY + w->wlim.sy, w->curX + w->wlim.sx))
		{
			actCursor = 1;
			FWrtChar(put);
		}
	if(w->curX >= w->maxX - (2 * addbrd))
	{
		wLocate(w->curY, w->curX);
		return 1;
	}
	w->curX++;
	#ifndef _W98_				// En W98 ya avanza FWrtChar.
	if(actCursor)
		FCursorAdvance();
	#endif
	return 0;
}

//   Enrolla el contenido de la ventana una linea hacia arriba.
void wScroll()
{
    unsigned short *desde, *hasta;
	int cuanto, addbrd;

	addbrd = w->fg.Skipbrd? 1: 0;
	hasta = w->image + (addbrd * (1 + w->maxX)) + addbrd;
	desde = w->image + ((1 + addbrd) * (1 + w->maxX)) + addbrd;
	cuanto = ((w->maxX + addbrd) * 2) * (w->maxY - (2 * addbrd)) -
			 (2 * addbrd);
    memmove(hasta, desde, cuanto);
	RefreshScreen();
	wLocate(w->maxY - 2 * addbrd, 0);
	for(cuanto = 80, addbrd = 0; !addbrd && cuanto; cuanto--)
		addbrd = wPch(' ');
}

//   Retorna las variables del area cliente.
int wClientWidth()		// Ancho
{
	return w->cliX;
}
int wClientHeigth()		// Alto
{
	return w->cliY;
}

//   Cambia el color con el que imprimirá posteriormente en la ventana.
int wSetColor(int newcolor)
{
	int prevcolor;

	prevcolor = w->ucolor;
	w->ucolor = ucolor = (unsigned)((newcolor & 0xff) * 256);
	return prevcolor / 256;
}

//   Cambia el color a uno de los colores del esquema.
int wEsqSetColor(int esqElem)
{
	int uEsq;

	uEsq = esqElem;
	if(uEsq < 0 || uEsq > 15) uEsq = 0;
	if(!w->colesq[uEsq]) uEsq = 0;
	return wSetColor(w->colesq[uEsq]);
}

//   Borra la ventana actual, repone el atributo al color que se especi-
//   fica, y coloca el cursor en la posicion (0,0).
void wCls(void)
{
    wCls(' ', 0);
}

void wCls(char t, short color)
{
	int y;

	memset(tstr, t, 90);
	tstr[w->maxX + (w->fg.Skipbrd? 0: 1)] = '\0';
    if(color) wSetColor(color);
	for(y = 0; y < w->maxY + (w->fg.Skipbrd? 0: 1); y++)
	{
		wLocate(y, 0);
		wPstr(tstr);
	}
}


//   Dibuja un rectangulo en la ventana actual.
static char *boxC[3] = { "Ú¿ÀÙÄ³", "É»È¼Íº", "ÛÛÛÛÛÛ" };

void wDrawBox(struct rect *r, int style)
{
	 char *bx;
	 int y, x;
	 
	 if(style < 1 || style > 3) return;
	 bx = boxC[style - 1];          // Prepara el estilo del recuadro

	 wLocate(r->sy, r->sx);         // Linea superior.
	 wPch(*bx);
	 for(x = r->sx + 1; x < r->ex; x++)
		wPch(*(bx + 4));
	 wPch(*(bx + 1));

	 for(y = r->sy + 1; y < r->ey; y++) // Lineas verticales
	 {
		wLocate(y, r->sx);
		wPch(*(bx + 5));
		wLocate(y, r->ex);
		wPch(*(bx + 5));
	 }

	 wLocate(r->ey, r->sx);           // Linea inferior
	 wPch(*(bx + 2));
	 for(x = r->sx + 1; x < r->ex; x++)
		wPch(*(bx + 4));
	 wPch(*(bx + 3));
}

//   Coloca una ventana en el tope de la lista de ventanas presentes en
//   pantalla, o la remueve de ahi.
void WindowsInScreen(struct wnd *wp, int acc)
{
	int i, j;

	if(!acc)                            // Si esta removiendo la ventana.
	{
		for(i = j = 0; i < MAXWINDS; i++, j++)
		{
			if(words[i] == wp)
			{
				j++;
				wstack--;
			}
			if(i != j && j < MAXWINDS)
				words[i] = words[j];
		}
		return;
	}

	//   Esta agregando una ventana al tope.
	WindowsInScreen(wp, 0);         // Primero, remueve cualquier referencia
	memmove(&words[1], &words[0], wstack * sizeof(struct wnd *));
	words[0] = wp;
	wstack++;
}

//   Refresca el mapa de asignación de la pantalla, de acuerdo con las
//   ventanas que esten activas.
void RefreshScrMap()
{
	 struct wnd *wp;
	 int i, j;
	 char mywind, *ptr;
	 int start;

	 for(wp = words[(i = wstack) - 1]; i; i--, wp = words[i - 1])
	 {
		mywind = (char)(wp - wnds);
		for(start = wp->wlim.sy * 80; start <= 80 * wp->wlim.ey; start += 80)
			for(j = wp->wlim.sx, ptr = scrmap + start + j;
				   j <= wp->wlim.ex; j++)
				*ptr++ = mywind;
	 }
}

//   Define una ventana. Para poder sacar informacion a la misma se debe
//   utilizar wActivate()
int wDefine(int sy, int sx, int ey, int ex,     // Coordenadas
			int border, int cursor,             // Forma del borde y cursor
            short *esquema,                     // Esq de colores o cero.
			char *title, char *footer)          // Titulo / Pie
{
	int addb, wsc, cmem;
	struct wnd *ws;

	addb = border? 1 : 0;
	if((sy + border + 1 > ey) || (sx + border + 1 > ex))
		return -1;
	for(wsc = 1; wsc < MAXWINDS; wsc++)
		if(!((ws = &wnds[wsc])->fg.EnUso))
			break;
	if(wsc == MAXWINDS) return -1;     // Sin lugar.
	ws->fg.EnUso = 1;                  // Marca en uso
	ws->fg.NoShow = 1;                 // No la muestra.
	ws->fg.Drawed = 0;				   // La dibuja ya que no estaba.
	ws->fg.Skipbrd = 0;
	ws->wlim.sx = sx;                  // Actualiza variables
	ws->wlim.sy = sy;
	ws->wlim.ex = ex;
	ws->wlim.ey = ey;
	ws->cliX = (ws->wlim.ex - ws->wlim.sx) - (border? 2: 0);
	ws->cliY = (ws->wlim.ey - ws->wlim.sy) - (border? 2: 0);

	ws->border = border;               // Borde predefinido.
	ws->maxX = ex - sx;                // Para chequeo rapido de overflows.
	ws->maxY = ey - sy;
	ws->curX = 0;                      // Posicion inicial del cursor.
	ws->curY = 0;
	ws->cursor = cursor;               // Cursor a utilizar.
	if(esquema)
		memmove(ws->colesq, esquema, 16);
	 else
		memmove(ws->colesq, defesq, 16);
	cmem = ((1 + ws->maxY) * (1 + ws->maxX)) * 2;
    ws->image = (unsigned short *) MALLOC(cmem, "wDefine");
    if(!ws->image)
		AbnormalEnd("Sin memoria para mas ventanas");
    memwset((short *) ws->image, (short)((ws->colesq[3] * 256) + ' '),
            cmem / 2);
	if(strlen(title) < 80)
		STRCPY(ws->title, title);
	 else
	 {
		memmove(ws->title, title, 79);
		ws->title[79] = '\0';
	 }
	if(strlen(footer) < 80)
		STRCPY(ws->footer, footer);
	 else
	 {
		memmove(ws->footer, footer, 79);
		ws->footer[79] = '\0';
	 }
	return wsc;                                 // Handle para uso poster.
}

//   La siguiente funcion, permite colocar un caracter en el
//   marco de una ventana. Es necesaria dado que las funciones
//   de impresion solo permiten el acceso al area cliente de la
//   ventana.
void wSetBorder(int y, int x, int ch)
{
	struct wnd *mw;
	int saveSkip, saveColor;

	mw = &wnds[wact];
	saveSkip = mw->fg.Skipbrd;
	mw->fg.Skipbrd = 0;
	if(y == 999) y = mw->maxY;
	if(x == 999) x = mw->maxX;
	wLocate(y, x);
	saveColor = wSetColor(mw->colesq[0]);
	wPch(ch);
	mw->fg.Skipbrd = saveSkip;
	wSetColor(saveColor);
}

//   Activa una ventana para su uso.
//   Los parametros son:
//      - whand: Manejador de ventana, tal como lo devuelve wDefine().
//      - showfg: a Uno si activa la visualizacion de la ventana.
//      - samefg: a Uno si NO la coloca sobre las otras ventanas.
void wActivate(int whand)
{
	 int l, showfg;
	 struct rect b;

	 if(whand == wact) return;			// Ya esta activa.
	 showfg = 1;                        // (Deberia ir como parametro).
	if(whand<0 || whand>=MAXWINDS)
		AbnormalEnd("Invalid window handle");
	
	struct wnd *tmpw = &wnds[whand];
	 if(!tmpw->fg.EnUso)  return;

	 w = tmpw;                           // La  activa como actual.
	 wact = whand;
	 if(!w->fg.Drawed)                // La dibuja, si no estaba.
	 {
		wSetColor(w->colesq[3]);      // Limpia el contenido
		wCls();
		w->fg.Skipbrd = 0;
		if(w->border)                 // Si tiene borde, lo dibuja.
		{
			wSetColor(w->colesq[0]);
			b = w->wlim;
			b.ey -= b.sy; b.ex -= b.sx;
			b.sy = b.sx = 0;
			wDrawBox(&b, w->border);
            l = strlen(w->title);
            if(l)
			{
				wSetColor(w->colesq[1]);
				wLocate(0, (w->maxX - l) / 2);
				wPstr(w->title);
			}
            l = strlen(w->footer);
            if(l)
			{
				wSetColor(w->colesq[1]);
				wLocate(w->maxY, (w->maxX - l) / 2);
				wPstr(w->footer);
			}
		}
		w->fg.Drawed = 1;             // Evita un redibujo.
		w->fg.Skipbrd = 1;            // Evita se masacren los bordes.
		w->curY = w->curX = 0;
	 }
	 if(showfg)
	 {
		WindowsInScreen(w, 1);        // Coloca en la lista.
		w->fg.NoShow = 0;
	 } else
	 {
		WindowsInScreen(w, 0);        // Sino, la quita.
		w->fg.NoShow = 1;
	 }
	 RefreshScrMap();                  // Actualiza la asignacion de pantalla
	 RefreshScreen();                  // Redibuja el desktop
	 wChgShape(w->cursor);            // Actualiza forma del cursor
	 wLocate(w->curY, w->curX);        // Actualiza posicion del cursor
	 wSetColor(w->colesq[3]);          // Color normal de operacion.
}

//   Pasa una ventana al frente de todas. (Pero sin activarla).
void wPutFront(int wh)
{
	struct wnd *mw;

	mw = &wnds[wh];                             // Manejador
	if(!mw->fg.EnUso || !mw->fg.Drawed) return; // si no esta inicializada
	WindowsInScreen(mw, 1);
	/*w = mw;
	wact = w - wnds;*/
	RefreshScrMap();
	RefreshScreen();
}

//   Desactiva la window actual, si se puede, cediendole el control a la
//   inmediatamente inferior. Si el parametro que se le pasa esta a uno,
//   entonces NO esconde la ventana sino que, en pantalla, es como si no
//   pasara nada.
void wDeactivate(int wh, int nohide)
{
	struct wnd *mw;

	wActivate(wh);
	mw = &wnds[wh];
	if(!mw->fg.EnUso || wh == 0 || w != mw)
		return;

	WindowsInScreen(mw, 0);
	RefreshScrMap();
	RefreshScreen();
	wChgShape(w->cursor);       // Actualiza la salida streaming
	wLocate(w->curY, w->curX);
	wSetColor(w->ucolor);
}

//   Remueve una ventana, desactivandola si corresponde y liberando toda
//   la memoria que esta haya estado utilizando.
void wRelease(int wh)
{
	struct wnd *mw;
	int ActScr;

	wDeactivate(wh, 0);                     // La desactiva.
	mw = &wnds[wh];                         // Obtiene el puntero.
	if(!mw->fg.EnUso || wh == 0) return;    // si no se puede remover.
	ActScr = 0;
	if(w == mw)                             // Si era la ventana activa
	{
		w = words[1];
		wact = (int)(w - wnds);
		ActScr = 1;
	}
	WindowsInScreen(mw, 0);                 // Por si estaba visible
	if(ActScr && w)
	{
		wChgShape(w->cursor);               // Esta reactivando otra
		wLocate(w->curY, w->curX);
		wSetColor(w->ucolor);
	}
    FREE(mw->image);                    // Memoria utilizada
	mw->fg.EnUso = 0;                       // Libera el handler
	RefreshScrMap();
	RefreshScreen();
}

/************************************************************************/
/*   Las siguientes funciones se utilizan para implementar el acceso  ***/
/*   FISICO al hardware de video.                                     ***/
/************************************************************************/

//   Cambia la forma del cursor, y lo activa/desactiva.
void FCursorShape(int curs)
{
	#ifdef _W98_
/*	CONSOLE_CURSOR_INFO cursor;

	switch(ActCursorShape = curs)
	{
		case NOCURSOR:
			cursor.bVisible = FALSE;
			cursor.dwSize = 100;
			break;
		case SNGCURS:
			cursor.bVisible = TRUE;
			cursor.dwSize = 10;
			break;
		case BLKCURS:
			cursor.bVisible = TRUE;
			cursor.dwSize = 0;
			break;
	}
	SetConsoleCursorInfo(hCon, &cursor);*/
	#else
	//  Acceso directo al hardware, para modos msdos puros.
	int sc, ec;

	if(ActCursorShape != curs)      // Solo si cambia
	{
		switch(ActCursorShape = curs)
		{
			case NOCURSOR:          // Apaga el cursor
				 sc = 12; ec = 11;  // Posiciones imposibles
				 break;

			case SNGCURS :          // Cursor de raya simple
				 sc = 10; ec = 12;
				 break;

			case BLKCURS :          // Cursor de bloque
				 sc = 0; ec = 12;
				 break;
		}

		_outp(CRTCtrl, 10);         // Registro de arranque de cursor
		_outp(CRTData, sc);
		_outp(CRTCtrl, 11);         // Registro de fin de cursor.
		_outp(CRTData, ec);
	}
	#endif
}

// Cambia la posición FISICA del cursor en la pantalla.
void FCursorPos(int y, int x)
{
	#ifdef _W98_
	COORD posic;
	#else
	unsigned offset;
	#endif

	ActCursorY = y; ActCursorX = x; // Actualiza coordenadas fisicas.

	#ifdef _W98_
	posic.Y = y; posic.X = x;
	SetConsoleCursorPosition(hCon, posic);
	#else
	offset = y * 80 + x;
	_outp(CRTCtrl, 14);             // Actualiza el CRT
	_outp(CRTData, (offset & 0xff00) >> 8);
	_outp(CRTCtrl, 15);
	_outp(CRTData, offset & 0xff);
	#endif
}

//  Avanza el cursor a la siguiente posición de la pantalla.
void FCursorAdvance()
{
	if(ActCursorX++ >= 80)
	{
		ActCursorX = 0;
		if(ActCursorY++ >= 25)
			ActCursorY = 24;
	}
	FCursorPos(ActCursorY, ActCursorX);
}

//  Escribe un caracter en la memoria de video.
//  OJO - El high-byte de 'ch' puede contener el atributo a usar.
//        Si fuera cero, se imprimirá con el atributo actual de ucolor'.
//  Adaptada para funcionar bajo modo protegido de MSDOS y como aplicacion
//  consola de Windows de 32 bits (W95/98/NT).
void FWrtChar(unsigned short car)
{
	#ifdef _W98_
	char cBuff[2];
	unsigned long cant, reserved;
		
	if(car & 0xff00) ucolor = car & 0xff00;		// Si venía con atributo.
	if(xUcolor != ucolor)
	{
		xUcolor = ucolor;
		SetConsoleTextAttribute(hCon, ucolor / 256);
	}
    cBuff[0] = (char) car; cBuff[1] = 0;
	WriteConsole(hCon, cBuff, 1, &cant, (void *) &reserved);

	// 'WriteConsole' avanza el cursor, algo que deja descontroladas las
	//  variables locales que llevan la posición del cursor. Así, las
	//  actualiza. No hay un 'GetConsoleCursorPosition', así que...
	FCursorAdvance();

	#else
	// Acceso directo al hardware de video. (msdos puro).
	unsigned short *memscr;

    memscr = scrstart + (long) (((ActCursorY * 80) + ActCursorX) & 0xffffl);
    _asm {
                mov ebx, dword ptr [memscr]
                mov cx, car
                mov ax, word ptr VideoSel            
                mov dx, es
                cmp ax, dx
                je  NoReload
                mov es, ax
    NoReload:   mov word ptr es:[ebx], cx
    }
    //*memscr = ch;
    #endif
}

//  Borra toda la pantalla, reseteando también el atributo que se usará.
void FCls()
{
	unsigned long cant;

	#ifdef _W98_
	FillConsoleOutputCharacter(hCon, ' ', 25 * 80, CeroCoord, &cant);
	FillConsoleOutputAttribute(hCon, ucolor / 256, 25 * 80, CeroCoord, &cant);
	#else
	_asm {
				mov ax, 0600h
				mov bh, 07h
				mov cx, 0
				mov dx, 25 * 256 + 80
				int 10h
		 }
	#endif

	ActCursorY = ActCursorX = ActCursorShape = 9999;
	FCursorPos(0, 0);
	FCursorShape(SNGCURS);
}

