//  winsup.cpp
//  Habilita el funcionamiento bajo windows 32.

#ifndef _CONSOLE
#include "stdafx.h"
#include "pos.rh"

#define MAX_LOADSTRING 100
#ifndef TCHAR
#define TCHAR char
#endif

// Variables Globales:
HINSTANCE hInst;						// Instancia actual.
TCHAR szTitle[MAX_LOADSTRING];          // Titulo de la ventana.
TCHAR szWindowClass[MAX_LOADSTRING];    // Nombre del programa.
TCHAR szVisorClass[] = "Visores";		// Clases para los visores.
HFONT VisorFont;						// Font para el visor.
HPEN VisorPen;							// Lapicera para el visor.
HWND hWnd,								// Handler a la ventana principal.
	 hVisor;							// Handler al visor principal.

// Funciones que utilizo mas adelante.
ATOM			 RegistroClases(HINSTANCE hInstance);
BOOL			 InicioInstancia(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AcercaDe(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK VisorProc(HWND, UINT, WPARAM, LPARAM);

//   Función principal.
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Cargo las strings globales.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_POS, szWindowClass, MAX_LOADSTRING);
	RegistroClases(hInstance);

	// Inicializo la aplicación.
	if (!InicioInstancia(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

    // Carga la tabla aceleradora de las teclas.
	hAccelTable = LoadAccelerators(hInstance, (LPSTR)IDC_POS);

	// Lazo principal de mensajes.
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//   Esta función y su uso son solo necesarias si se quiere que este có-
//   digo sea compatible con sistemas Win32 anteriores a la existencia de
//   la función 'RegisterClassEx', que aparece recién en Windows 95. Al
//   usar esta función se permite cargar y utilizar íconos pequeños. Esta
//   función no existía en Win32s (win 3.11) por lo que convertí el código
//   a la API anterior.
//
ATOM RegistroClases(HINSTANCE hInstance)
{
    WNDCLASS wc;
	int res;

	//   Clase para la ventana principal.
	wc.style		 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	 = (WNDPROC)WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
    wc.hIcon         = LoadIcon(hInstance, (LPSTR)IDI_POS);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName  = (LPSTR)IDC_POS;
	wc.lpszClassName = szWindowClass;
    res = RegisterClass(&wc);
	if(res)
	{
		wc.style = CS_PARENTDC | CS_NOCLOSE;
        wc.lpfnWndProc = VisorProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = szVisorClass;
		res = RegisterClass(&wc);
	}
	return res;
}

//   En esta función, salvo en una variable global el handle de la instan-
//   cia y creo y displayo la ventana principal.
//
BOOL InicioInstancia(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Grabo el nro de instancia.
   int x;

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
					   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
					   hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   hVisor = CreateWindow(szVisorClass, "x",		// Clase y Título
						 WS_BORDER | WS_CHILD,	// Estilo
	                     0, 0,					// x, y
	                     240, 60,				// Ancho, Alto.
						 hWnd,					// Window padre.
						 NULL,					// Menu atachado.
						 hInstance,				// Instancia.
						 NULL);					// Parámetros adicionales.
   x = GetLastError();
   ShowWindow(hVisor, SW_SHOWNORMAL);
   x = GetLastError();

   return TRUE;
}

//  Proceso de los mensajes del visor principal.
LRESULT CALLBACK VisorProc(HWND hWnd, UINT message, WPARAM wParam,
						   LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR mens[MAX_LOADSTRING];
	double xReg;
	RECT rt;
	HFONT PrevFont;
	HPEN PrevPen;

	switch(message)
	{
		case WM_PAINT:
			if(!VisorFont)
			{
				VisorFont = CreateFont(50, 0, 0, 0, 600, FALSE, FALSE,
					               FALSE,
				                   ANSI_CHARSET, OUT_DEFAULT_PRECIS,
								   CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
							       DEFAULT_PITCH | FF_ROMAN, "Arial");
				VisorPen = CreatePen(PS_SOLID, 1, 0xffffff);
			}
			xReg = 12345.67;
			sprintf(mens, "%-12.2lf", xReg);
			hdc = BeginPaint(hVisor, &ps);
			GetClientRect(hVisor, &rt);
			if(VisorFont)
				PrevFont = (HFONT) SelectObject(hdc, VisorFont);
			if(VisorPen)  
				PrevPen  = (HPEN) SelectObject(hdc, VisorPen);
			DrawText(hdc, mens, strlen(mens), &rt, 
				      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			SelectObject(hdc, PrevFont);
			SelectObject(hdc, PrevPen);
			EndPaint(hVisor, &ps);
			break;

		case WM_DESTROY:			// Se está destruyendo la ventana.
			if(VisorFont) DeleteObject(VisorFont);
			if(VisorPen)  DeleteObject(VisorPen);
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//  Proceso los mensajes para la ventana principal.
//		WM_COMMAND - proceso de mensajes del menú de la aplicación.
//		WM_PAINT   - Pinto la ventana principal.
//		WM_DESTROY - Despacho un mensaje de 'quit' y retorno.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
						 LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];

	sprintf(szHello, "Hola - Tamaño del Entero: %d.", sizeof(int));
	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 

			//   Opciones del menú.
			switch (wmId)
			{
				case IDM_ABOUT:		// Despacho la dialog 'Acerca de..'
                   DialogBox(hInst, (LPSTR) IDD_ABOUTBOX, hWnd,
					         (DLGPROC) AcercaDe);
				   break;

				case IDM_EXIT:		// Proceso la función de 'Salir'
				   DestroyWindow(hWnd);
				   break;

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_PAINT:				// Pinta la ventana.
			hdc = BeginPaint(hWnd, &ps);
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, 
				      DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:			// Se está destruyendo la ventana.
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Manejador de mensajes para la dialog 'Acerca de...'
LRESULT CALLBACK AcercaDe(HWND hDlg, UINT message, WPARAM wParam,
					   LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}
#endif
