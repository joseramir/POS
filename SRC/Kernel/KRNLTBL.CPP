//
//   KrnlTbl.cpp
//   Tablas para configuracion del kernel del POS.
//

#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Lista de 'device drivers' utilizables.
//   En un dispositivo, la class que se utilice para indicar el interfaz a usar debe
//   estar en CUIDADOSA correspondencia con lo que se indique como 'clase' del dispositivo.
//   Si no se cuida este detalle, POS fallecera miserablemente.
//   La siguiente tabla define las 'clases' de dispositivos disponibles, la 'class' que
//   se debe usar como base del interfaz y el puntero estandard para comunicacion con el 
//   device. Recordar que si no se define una clase derivada, entonces la implementacion
//   de la clase base toma el control con una función default. De esta forma, en la clase
//   derivada solo es necesario implementar aquellas funciones que se necesiten adaptar.
//
//    Clase         ClassBase       StdDevice	Observaciones
//   ------------------------------------------------------------------------------------
//    DV_PRINTER    PrinterFiscal_  pf			Seleccionar en [prnprot] de opciones.ini
//	  DV_MAINDISP	MainDisplay_	md			Se usara EL PRIMERO que encuentre.
//	  DV_CUSTDISP	MainDisplay_	cd			Se usara EL PRIMERO que encuentre.
//	  DV_KEYBOARD	Keyboard_		kb			Seleccionar en [teclado] de opciones.ini
//	  DV_SCANNER	Scanner_		sc			Se usara EL PRIMERO que encuentre.

//   Prototipos.
int PreparePluginFunctions(void);
//extern "C" {
//	extern PrinterFiscal_ P_EPSON930, P_DUMMY, P_TM300, P_PR4, P_PR5, P_BEMA, P_TM220, PL23F;
//};
//
////   Lista de dispositivos.
//DeviceList_ DeviceList[] = {
//	{	&P_DUMMY,		"nada",			DV_PRINTER	},	// No utiliza impresor (printer.cpp)
//	{	&P_EPSON930,	"Epson930",		DV_PRINTER	},	// Implementado en P930.cpp
//	{   &P_EPSON930,	"Epson950",		DV_PRINTER	},
//	{	&P_TM300,		"EpsonTM300F",  DV_PRINTER	},	// Impresores fiscales EPSON
//	{	&P_TM300,		"EpsonTM950F",	DV_PRINTER	},
//	{   &P_TM220,       "EpsonTM220F",  DV_PRINTER  },
//	{   &P_PR4,         "HasarPR4",     DV_PRINTER  },  // Impresores fiscales HASAR
//	{   &P_PR5,         "HasarPR5",     DV_PRINTER  },
//	{   &P_BEMA,        "BematechMP35", DV_PRINTER  },  // Impresores fiscales BEMATECH
//
//};
//int cDev = sizeof(DeviceList) / sizeof(DeviceList_);

//   Lista de funciones disponibles.

//   Lista de servicios que se deben ejecutar en forma periodica.
//   PRECAUCION: No tocar nada de la configuración de las rutinas periódicas 
//               nativas del POS.
PeriodicList_ PeriodicList[] = {
	{	eventDispatch,	"Event Dispatcher", 1},	// NATIVA=Despachador de eventos.
	{	CheckSSaver,	"C-ScreenSaver", 10 },	// NATIVA=Protector de pantalla.
};
int cPeriod = sizeof(PeriodicList) / sizeof(PeriodicList_);


//   Servicios que se deben arrancar al inicio y que se deben apagar al finalizar.
//   CUIDADO: No altere la configuración de los servicios comentados con POS INTERNO,
//            el POS podría dejar de funcionar.
//		      También, NO cambiar EL ORDEN en que arrancan los diversos servicios POS INTERNO,
//			  aunque, si se pueden insertar otros servicios entre ellos.
BootList_ BootList[] = {
	{   initSwitcher,                0,  
	   "Iniciando Servicios de MultiTasking" },						// POS INTERNO

	{   LoadOpc,                     0, 
	   "Cargando Configuracion de Opciones Fijas" },				// POS INTERNO

	{   InitializeWSup,    ReleaseWSup,  
	   "Iniciando Soporte de Video VGA y Ventanas" },				// POS INTERNO

	{   ErrorInit,                   0,  
	   "Iniciando la Retoma y Captura de Errores y Rastreo" },		// POS INTERNO

	{   PreparePluginFunctions,      0,      
	    "Agregando funciones de PLUGIN" },							// ApiFunctions.cpp

	{   LoadMessages,                0,  
	   "Cargando archivo de Mensajes Predefinidos" },				// POS INTERNO

	{   initEvent,                   0,  
	   "Iniciando Servicios del Despachador de Eventos" },			// POS INTERNO

	{   InitAllSerial, DeInitAllSerial,  
	   "Inicializando Dispositivos de Comunicacion Serie" },		// POS INTERNO

	{   InitCmos,                    0,  
	   "Iniciando Servicios de Lecto-Escritura en CMOS-RAM" },		// POS INTERNO

	{   initKbd,                     0,  
	   "Iniciando Controlador Logico de Teclado P&P" },				// POS INTERNO

	{   InitScanner,                 0,  
	   "Iniciando Controlador Logico del Scanner Serial" },			// POS INTERNO

	/*{   InitBalanza,                 0,  
	"Iniciando Controlador Logico de la Balanza Serial" },			// POS INTERNO*/

	{   LoadDep,                     0,  
	   "Cargando Descripcion de Departamentos" },					// POS INTERNO

	{   LoadClerk,                   0,  
	   "Cargando Descripcion de Cajeros" },							// POS INTERNO

    {   LoadVen,                     0,  
       "Cargando Descripcion de Mozos" },							// POS INTERNO

	{	PrepareTjOnline,			0,
		"Cargando Definicion de Tarjetas Online" },					// POS INTERNO

	{   LoadMpag,                    0,	
	   "Cargando Descripcion de Medios de Pago" },					// POS INTERNO

	{   LoadMisc,                    0,  
	   "Cargando Definicion Teclas Varias" },						// POS INTERNO

	{   LoadCobros,					 0,
	   "Cargando Definicion de PractiPagos" },						// POS INTERNO

	{   LoadIvas,                    0,  
	   "Cargando Tasas de Iva e Impuestos" },						// POS INTERNO

	{   LoadPlus,           UnLoadPlus,  
	   "Abriendo Base de Descripcion de Articulos" },				// POS INTERNO

	{   InitEnvases,                 0,  
	   "Inicializando Manejo de Envases"},							// POS INTERNO

	{   InitCashDisp,    ResetCashDisp,  
	   "Cargando Soporte para Visor de Cajero" },					// POS INTERNO

	{   InitEmax,           DeInitEmax,  
	   "Iniciando Controlador Logico de Visor EMAX" },				// POS INTERNO

	{   InitPrinter,                 0,  
	   "Iniciando Controlador Logico del Impresor" },				// POS INTERNO

	{	InitPromos,					0,	
	   "Preparando Promociones" },									// POS INTERNO

	{   InitScheduler,               0,  
	   "Iniciando Atencion de Tareas Preprogramadas" },				// POS INTERNO

    {   InitSSaver,                  0,  
       "Inicializando el Salva-pantalla" },							// POS INTERNO

	{   InitDump,                    0,  
	   "Iniciando Servicios de Dumping de Transacciones" },			// POS INTERNO
};
int cInit  = (sizeof(BootList)/sizeof(struct BootList_));
