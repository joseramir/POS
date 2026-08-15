//Pos.h    -   Declaraciones generales del modulo POS completo  */

#pragma once
#include <time.h>
#include "ArrayList.h"
#include "BDecimal.h"
#include "TjOnline.h"

//   Declara la variable de control __FLAT__. Si esta variable est  decla-
//   rada, entonces implica que el programa se esta compilando con una
//   herramienta que genera ejecutables de 32 bits (Consola Windows 95/98 o
//   que usen Dos Extenders). En este caso, el modelo de memoria es plano
//   y se usa el 386 al mango (32 bits). Cambian entonces las subrutinas en
//   assembler y otras cosas. POS se puede tambien compilar en 16 bits,
//   pero hace rato que no se hace esto y deberian probarse bien los drivers
//   especialmente.

#define __FLAT__ 1
#ifndef __VSC__ 
	#define _outp outp
#endif

//   Manejo unificado del color
extern short prnesq[];			// Esquema de colores default.
extern int ForzarZeta;			// La linea de orden dice 'ZETA'

/////////////////////////////////////////////////////////////////////////////
//    Prototipos de las funciones internas a POS.

#ifdef _DEBUG
#define AbnormalEnd(msg) throw msg;
#else
void AbnormalEnd(char *msg);
#endif

void POSExit(void);									// pos.cpp - INTERNA - No usar.
//void RenameClr(System::String^ fnsource, System::String^ fndestino);
int  EmitirZeta(void);								// Emite una ZETA al inicio.

//   Termo.cpp
//   Manejo del termometro. Esta es una pequeña barra de progreso que aparece en una
//   ventana destinada al efecto. Se diseño de forma que no estorbe a las ventanas
//   normales cosa que se pueda seguir operando tranquilamente.
void OpenTermo(char *msg, long CienPerCien);		// Abre la ventana del termometro.
void CloseTermo(void);								// Destruye al termometro de la pantalla.
void AcTermo(long Percent);							// Actualiza la barra del termometro.
void AcTermoMsg(char *msg);							// Actualiza el mensaje de orientacion.

//	 Cobros.cpp
//	 Funciones del modulo de cobranzas.
int	 LoadCobros(void);								// Carga inicial de definiciones.
void cobro(int cual);								// Procesa la registracion de cobranzas.
void cob_step2(struct mpag_ *mp, double monto);		// Recibio una cobranza.
void ProcCobro(void *cob_, int fwrite);				// Procesa en el dump
struct cobro_ *BuscarCobro(int cod);					// Busca la definicion de un cobro

//   Sync.cpp
//   Se exportan algunas funciones utiles.
int  CheckNetwork(void);							// Analiza el estado de la red
int  xCopyFile(char *src, char *dst, char *tMsg);	// Copia un archivo usando func. internas
int  xCopyDosFile(char *src, char *dst, char *tMsg);// Copia un archivo usando a MSDOS.
void Syncronize(void);								// Ejecuta las funciones de la tabla sync_

//   Schedul.cpp
//   Aporta funciones que permiten programar eventos para que sucesan a determinada hora.
int  InitScheduler(void);							// Inicializa el scheduler (tabla sched_)
void CheckScheduler(void);							// Chequea el scheduler - NO USAR.

//   MyMath.cpp
//   La POS utiliza un punto decimal virtual que se puede configurar en opciones.ini, de
//   forma que no se ate a un determinado modelo de moneda. Se escribieron entonces algunas
//   funciones de utilidad que se utilizan para manejo de los tipos double que usa POS.
double atof_(char *src);							// Convierte ASCII a DOUBLE.
char *store(double src, char *dst);					// Convierte DOUBLE a ASCII.
char *ml(char *src);
void EnLetras(double num, char *postfix);			// Convierte DOUBLE a una frase que lo describe
extern char enlet_Reng1[], enlet_Reng2[], enlet_Reng35[], enlet[];

//   Enum.cpp
//   Permite utilizar archivos tipo '.def', para cargar definiciones y configuraciones
//   fijas. En los archivos tipo .def, el pos permite efectuar macrosustituciones. Un ejemplo
//   de archivo .def es el archivo 'keyboard.def' con la definicion del teclado.
char *SkipSpaces(char *msg);						// USO INTERNO - Saltea espacios en bco.
char *GetToken(void);								// Extrae un token de la linea actual.
int  StartFile(char *FileName, char LetterID);		// Abre el archivo .DEF indicado.
void CloseFile(char);								// Cierra el archivo .DEF que se esta usando.
char *GetPLine(void);								// Lee una linea y la macroprocesa.
char *GetPToken(void);								// Lee un token y lo macroprocesa.

//   DumpErr.cpp
//   Facilita el manejo del archivo de log del POS.
void WriteErr(char *msg);							// Escribe MSG en el archivo de log.
void DumpStatus(char *msg);							// Escribe MSG junto con informacion interesante.
int  ErrorInit(void);								// Inicializacion. Ver tabla 'loaded_'.

//   Event.cpp
//   Manejo de eventos internos.
int  initEvent(void);								// Inicializacion (tabla loaded_)
void eventDispatch(void);							// USO INTERNO - No usar.
void addEvent(int clase, char *param,				// Al ocurrir cada evento.
			  unsigned szParam);
void defineEvent(int clase, void (*hand)(char *));	// Define un nuevo evento.
void EventStatus(void);								// USO INTERNO - No usar.
void FlushEvents(void);

//   SSaver.cpp
//   Salva pantallas.
void CheckSSaver(void);								// USO INTERNO - No usar.
int  InitSSaver(void);								// Inicializacion (tabla loaded_)
void ResetSSaver(void);								// Evita que se lance el ScreenSaver.

//   Scanner.cpp
//   Manejo de lectores de codigo de barra conectados por port serial (y no como emulacion
//   de teclado). Se maneja con el POS utilizando eventos.
int  InitScanner(void);								// Inicializacion (tabla loaded_)
void Scann(char *ScanCod);							// Simula el registro del un scanner.
void BeepScanner(void);								// Emite un pitido en el scanner.
void FlushScanner(void);							// Vacia buffers de scanner
extern int scanDebug;
void LoadWav();										// Carga el archivo de sonido a reproducir como 'beep'
void PlayWav();										// Hace un 'beep'

//	Balanza.cppk
int InitBalanza(void);
void BalanzaScann(char *p);
void FlushBalanza(void);

//   Kbd.cpp
//   Manejo del teclado de POS.
int  initKbd(void);									// Inicializacion (tabla loaded_)
void resetKbd(void);								// Borra buffer de teclado.
void WaitClear(void);								// Fuerza tecla ESC antes de proxima tecla.
int  GetTransformedKey(void);
int  GetKey(void);									// Toma tecla TRASLADADA.
unsigned xgetch(void);								// Toma tecla SIN TRASLADAR.
int  KbHit(void);									// Indica si hay una tecla pulsada a nivel de driver
void FlushAll(void);								// Vacia todos los buffers de kbd, scan, etc.
#ifdef POSXP
void putkey(unsigned);
#endif

//   MTask.cpp
//   Permite la simulacion de multiples tareas ejecutandose simultaneamente.
int  initSwitcher(void);							// Inicializacion (tabla loaded_)
void tSwitch(void);									// Comparte tiempo de CPU. 
void StackProbe(void);								// Prueba que todo este en orden.
void MtaskStatus(void);								// USO INTERNO - No usar.
int  fork(void (*func)(void), char *name);			// Lanza un nuevo hilo de ejecucion.
void killall(void);									// Destruye todos los hilos en uso.
void AddAnun(void (*f)(int oldt, int newt));		// Incorpora una funcion de anuncio de task switch.
void RemoveAnun(void (*f)(int oldt, int newt));		// Remueve una funcion de anuncio de task switch.

//   PosDisp.cpp
//   Manejo de visor estilo caja registradora.
int  InitCashDisp(void);							// Inicializacion (tabla loaded_).
void ResetCashDisp(void);							// Limpia todos los visores.
void DispCash(double n, int dec);					// Muestra en nros gigantes.
void DispDeta(char *lft, char *rgt);				// Muestra en el browse de detalle.
void DispDeta2(char *lft, char *rgt, char *aux);				// Muestra en el browse de detalle, para PL-23F
void DispAdic(int y, int x, char *msg);				// Muestra en el visor adicional.
void AdicClear(void);								// Simula pulsacion ESC.
void PrintTktTot(void);								// Imprime el subtotal del ticket sin refrescar la grilla.
void PrintTktTot(bool refreshGrid);					// Imprime el subtotal del ticket refrescando la grilla opcionalmente

//   Emax.CPP
//   Maneja el visor de cliente (dispositivo especial).
int  InitEmax(void);								// Inicializacion (tabla loaded_)
void DeInitEmax(void);								// Apaga (tabla loaded_)
void CustDisp(char *arriba, char *abajo);			// Muestra dos mensajes en el visor.
void CustDisp3(char *arriba, char *izq, char *der);	// Muestra tres mensajes en el visor.
void CustDisp4(char *arribaIzq, char *arribaDer,	// Muestra cuatro mensajes en el visor
			   char *abajoIzq, char *abajoDer);
void CustRott(char *msg);							// USO INTERNO - No usar.

//   Premios.cpp
void TotPremios(void);										// Imprime el total de premios al pie del ticket.
double ViewMiles(char *cod, double multi);					// Mensaje de premios, renglon por renglon.

//   MyStr.cpp
//   Algunas funciones utilitarias para manejos de string.
char *Trim(char *str);										// Remueve espacios antes y despues de la string.
char *Trim(char *str, char *buffer);						// Igual que 'Trim', pero sobre un buffer.
char *Padl(char *cual, int len, char fill);					// Rellena a la izquierda (dezplazando).
int  Empty(char *s);										// Mira si solo contiene espacios.

//   Inic.cpp
//   Manejo de archivos INI (como opciones.ini) donde las opciones se separan en grupos.
void CloseIni(void);										// Cierra un .ini abierto.
int CheckExisteIni(char *fn);								// Controla que exista un INI
int  OpenIni(char *fileName);								// Abre un nuevo ini.
int  ExtractIniSiNo(char *group, char *key, int def);		// Extrae una clave como 1(SI) o 0(NO).
int  ExtractIniSiNo(char *group, char *key);				// Extrae una clave como 1(SI) o 0(NO). - Default = No
long ExtractIniLong(char *group, char *key);				// Extrae una clave numerica.
long ExtractIniLong(char *group, char *key, long def);		// Extrae una clave numerica, con un valor default
char *ExtractIniChar(char *group, char *key);				// Extrae una clave alfanumerica en buffer estatico.
char *ExtractIniStr(char *group, char *key);				// Extrae una clave alfanumerica en buffer dinamico.
unsigned ExtractIniHexa(char *group, char *key);			// Extrae una clave hexadecimal (convertida a bin)
char *ExtractIni(char *group, char *key);					// Extrae una clave sin ningún postprocesamiento.
int  CheckSecc(char *group);								// Chequea que un grupo esté definido en el .ini.

//   Digits.cpp
//   Funciones de uso interno.
void HDecim(void);          
char *HRng(double value);
void HClear(void);


/////////////////////////////////////////////////////////////////////////////////////////////////////
//   Envases.cpp
//   Funcion de ENVASES.
int  InitEnvases(void);								// Carga la tabla (al boot).
int  CheckEnvases(void);							// Verifica envases al cierre de ticket.
void ProcEnv(void *track, int soloPrn);				// Vende envases, cuando corresponde.
void PrintEnvase(void *data);						// Imprime informacion relacionada con envases

//   Misc.cpp
//   Funcion de MISCELANEAS (Retiros, dotaciones, etc).
int  LoadMisc(void);								// Carga la tabla (al boot).
void ProcMisc(void *track, int soloPrn);			// Asienta una operacion miscelanea.
void ProcExtraCash(void *track, int soloPrn);			// Asienta una operacion Extraccion Efectivo.

//   Suspend.cpp
//   Manejo de SUSPENSIONEs (Tickets en espera, mesas suspendidas).
int  chkNoSusp(int cual);							// Chequea no queden suspensiones (zeta).
void borraSusp(int cual);							// Borra los tickets in supenso
extern int enEspera;
void ProcSuspende(void *d_, int fwrite);

//   ChMode.cpp
//   Maneja el cambio de modo de operacion (Registro, X1, etc).
void ProcChmode(void *tack, int soloPrn);			// Asienta un cambio de modo.
void chgmode(int newMode);							// Nuevo modo (0=Reg, ...)

//   Deptos.cpp
//   Manejo de DEPARTAMENTOS.
int  LoadDep(void);									// Carga la lista de deptos (al boot).        
void ProcDepto(void *track, int soloPrn);			// Asienta una operacion de depto.
void PrintDepto(void *track);						// Imprime una operacion de depto.
void ValuesDepto(void *track, int *cual, char **_d,	// Usada por la funcion de anulacion.
				 double *_cant, double *_monto);
char *GetDeptoDes(int nro);							// Usada por la funcion de impresion.
void PrograDeptos(void);							// Usada en modo PROGRAMA.

//   Mozos.cpp
//   Manejo de la funcion de vendedores (o mozos).
void ValuesVend(void *track, int *cual, char **d, int *flag);
char *venName(void);
void PrograVended(void);
void ForceVen(void);
void PrintVend(void *p);
void PrintVen(void);

// Host.cpp
int  InitializeHost(void);
void DeInitHost(void);
void ViewNetHandler(void *, char *);

// NetProto.h
int  LockRegister(int);
void ZeroLog(void);
void ForceZeta(void);

char *InfoReporteElectronico(int desde, int hasta);
char *InfoReporteElectronicoMes(int desde, int hasta);
char *CopiaDocumentoFiscal(int tipoDocumento, int nroDocumento);

// Dump.cpp
bool ProcesarOnline();
bool AnulacionesPendientes(void *dest);
void ConfirmaAnulacionesEnTrans(void);
bool ValidarRegistracion(double cant, double monto);
int  InitDump(void);
void WriteDump(void *);
void TruncTrans(void);
void WriteEOPFiscal(int);
void WriteEOPNoFiscal(void);
void *ReadDump(long);
void ZeroDump(void);
void ReprintDump(bool inicial);
void MoveTicket(char *fn, int tickNro);
bool MoveTicketNC(char *fn, int tipoDoc, int ptoVenta, int tickNro, int fecha, char *impTick);
void IncorporeTicket(char *fn);
void IncorporeTicketNC(char *fn, int tipoComp, long NroComp, int fechaOrig, char *pcodMotivoNc);
bool BuscaTicketNc(int tipoComp, long NroComp, int fechaOrig);
bool BuscaTicketNcHecho(char *nroTicketOriginal, bool checkUsado);
void EscribeRelTicketNc(void);
void TruncTicket(void);
void QuickVentas(char *file, int soloCajero);			// Ventas rapidas para informes al server.
void CalculateExpandedZeta(bool discrimOnline);
void IncorporaVentaRemota(int cajero);	
void MakeDepFactu(void *depx_, char *g_Codigo, char *g_Descrip,
				  double *g_Cantidad,double *g_UnitarioN, double *g_UnitarioT);
void MakePluFactu(void *plux_, char *g_Codigo, char *g_Descrip,
				  double *g_Cantidad,double *g_UnitarioN, double *g_UnitarioT);
void MakeCorFactu(void *corrx_, char *g_Codigo, char *g_Descrip,
				  double *g_Cantidad,double *g_UnitarioN, double *g_UnitarioT);
unsigned long trLastSeek(void);
void WriteTickTmp(char *cod, char *des, double cant, double bulto, double uni,
				  double tot, int anulable);
int ForceRemoteDump();
void ActivaMonitoreo(int);
void DesactivaMonitoreo();
extern char *showMessage;		// Mensaje a mostrar al cerrar el ticket. (usar strdup)
void AcumTickCancelados(int conscajero, int &cantCancel, double &impCancel);
void BorraTickCancelados();

// Cmos.cpp
int  InitCmos(void);
void WriteCmos(int);

// Mpago.cpp
int  LoadMpag(void);
void ProcMpag(void *, int);
void PrintMpag(void *);


void ProcNcMpag(void *, int);
void PrintNcMpag(void *);

struct mpag_ *BuscarMpag(int cod);
void PrograMpag(void);
int  CompleteCC(struct mpag_ *refe, double vtaTot);
int  vCuit(char *cuit);
void ProcCCOnln(void *, int);
void ProcCobraCC(void *, int);
void CheckPriceOverflow(void);
void AlmacConsumo(void);
void LeeSaldoConsumo(void);
void PrintCobroVoucher(void);
extern int *availPagos;
extern bool forceRebuild;
void ControlaGaveta();								// Alerta si se debe hacer retiro
void StoreGaveta(struct mpag_ *mp, double monto);	// Modifica acumulador de dinero en gaveta.
void mpago(int, TarjInfo *);

extern int globalCCOnline, globalCheckPriceOverflow, globalCobraCC;
extern int c_lista;
extern double c_PorceMax, c_Consumo, c_MontoCobro, tCobro;
extern BDecimal recaMedioPago;

#define TIPORECA_FINANCIERO 0
#define TIPORECA_RECUPERO	 1
extern bool globalIgnoreSend;
void FacturarRecargo(int tipo, BDecimal recargo);
void FacturarMpagoRecargo(int tipo, BDecimal recargo);

int  LoadClk(void);                   // Cajero.cpp
void ProcClk(void *, int);
void PrograClk(void);
void OpenAllClerks(void);
extern int ultCajero;
extern char tmsg[];

void ProcTickFac(void *, int);        // TickFac.cpp

int  LoadVen(void);                   // Cajero.c
void ProcVen(void *, int);

int  LoadOpc(void);                   // opc.c

int  LoadIvas(void);                  // ivas.c
void CalcIva(double, int, int);
void CalcIvaSap(void *);
void CalcIvaSapNc(void *p_);
double RetSobreTasa(void);
int ivatasa(int);
double ivadisc(int);
double CalculoPercep(void);
void CalcIvaPromo(double, int, int);
void ReinitIvaPromo(void);
double ValueIvaPromo(void);
double CalcNetoIva(double, int, int);
void ReinitNetoIva(void);
double ValueIvas(void);

int  InitAllSerial(void);             // Serial.c
void Scann(char *);
void DeInitAllSerial(void);

extern int hDebugSerial, debugSerial;

void subtot(int);                     // subtot.cpp
void mkReport(void);
void ProcZeta(void *, int);
void PrnTotal(void);
void PrintZetaVend(int cual);
void ResetPOSAcum(void);

int  LoadPlus(void);                  // plu.cpp
void ProcPlu(void *, int);
void PrintPlu(void *);
void UnLoadPlus(void);
void plu(int);
void CheckNewPlu(void);
int  Existe(char *);
int  ExisteClr(char *);
int  checkerr(int);
char *padl(double cual, int cuanto);
void PrograPlu(void);
void ForcePluReindex(void);

void ProcNcPlu(void *, int);
void PrintNcPlu(void *);


extern bool autoDetected;

void ValuesPlus(void *dep, int *cual, char **_des, double *_cant,
				 double *_monto);

double ViewMiles(char *, double);		// Premios.c
void TotPremios(void);

int  LoadMessages(void);				// Messg.c

void Alert(char *, char *);				// Alert.c
void Alert(char *, char *, bool);		// Alert.c
int GetSiNo(char *, char *);
int GetSiNo(char *, char *, bool);
void sleep_(int);
int RequestSuperv(void);
int AutorizaSupervisorPuntual(char *msgAccion, char *outNombreSuperv, int maxLen);	// Cajero.cpp
void SendMsg(char *, char *);
void SendMsg(char *, char *, bool);
void ClearMsg(void);
int ClearMsg2(void);
extern char LastPassword[12];
int CheckPassword(char *msg, char *rPassw);

void factura(int);						// Factu.cpp
void reparto(int);
void prnFactu(void);
double FactuReca(int);
double FactuPercep(int, bool);			//Agregado Percepcion IIBB Jujuy 974
double FactuPercepTissh(int, bool);		//Agregado Percepcion TISSH Salta
double FactuPercepIva(int, bool);		//Agregado Percepcion IVA
void ProcFactu1(void *, int);
void ProcFactu2(void *, int);
void ProcFactu3(void *, int);
void ProcFactu4(void *, int);
void ProcFactu5(void *, int);		//Para percepcion IVA
void ProcSenia(void *, int);		//Para procesar la senia
void NotaCredito(int);
void CargaSeniaPago(int);
void extracash(int, double);
void ProcOtrop(void *, int);
void ProcRepar(void *, int);
void ProcNcPercep(void *, int );
void PrintFactu3(void *);
void PrintFactu4(void *);
void PrintFactu5(void *);
void PrintSenia(void *);			//Par realizar la impresion de la Senia
void PrintNcPer(void *);
char *NameComp(void);
void chgFactuMode(int);
char *NameLista(void);
int  chkItemCount(void);
extern int clavePrecios;
void InitMayorHandling();				// Manejo mayorista
void ResetMayor(void);
extern int switchPrec;
extern bool globalRedraw;

void ProcVendedorMayor(void *, int);	//Agregado para escribir en TRANS el registro del Vendedor Mayorista

#ifndef SKIP_BDECIM
void ExecOtroPrec(int nPrec, int permFlag);
bool PidePrecio(char *codNom, BDecimal *prec);
#endif


char *hora(void);                    // HoraFec.cpp
char *fecha(void);
char *periodofact(void);
unsigned long date(void);
int ComparaTm(struct tm *t1, struct tm *t2);
bool HayQueCerrar(long ptime, long intervalo);

int dow();
int time_();
long time__();
long seconds_();

int  LoadClerk(void);                // cajero.c
void ProcClerk(void *, int);
void ProcSuperv(void *, int);
void PrintClerk(void);
int  chkClk(void);
void clerk(int);
void superv(int);
char *clkName(void);
char *supervName(void);

int  ViewChMode(int);                // chmode.c
void clear(int);

void ProcPromo(void *, int);		// Promos.c
void RegisterPrize(int premio, char *p1, char *p2, char *p3, bool imprime, int linea);
extern bool promosHechas;

void Acomoda(char *, char *, char *);	// Corri.cpp
void ProcCorri(void *, int);

/*   Lista de eventos predefinidos   */

#define E_KEYB   1
#define E_SERIAL 2

extern char SERVER[];               // Host.c

extern int ssav_delay;              //  SSaver.cpp

extern int globalCorri;
extern double xReg, yReg, zReg, xCnt, xTotAnul, xPrecEcom;          // Digits.h
extern int cDecim, rMode, borrar, inOper, cTasa;
extern char *msg, *SI, iDesc[][50], vDesc[][50];
extern char *msg1, *msg2;
extern double *tNetos, *tIva, *tNpromo, *tNetosIva;
extern long xCntAnul;

extern int  cLinesLogo, cLinesPie, cLinesEpson;          // Messg.c
extern char **LogoLines, **LogoPie, **LogoEpson, **saludos, **closed;

extern int pRepeat, dRepeat, pSubtot;       // Dump.c
extern unsigned long ActualTrans;
extern unsigned safe_ss, safe_sp;
extern int GlobalReprint;
#ifndef POSXP
extern class xBrowse bDet;					// Estructura del browse.
#endif

extern int cDep;                            // deptos.c

extern char 
	NEW_PLU[], NEW_SCAN[],					// plu.c
     NEW_PLUIDX1[], NEW_PLUIDX2[], NEW_PLUIDX3[],
	 NEW_SCANIDX1[], NEW_SCANIDX2[];
extern int GlobalGetPrec;

extern int PromoActi;                       // Premios.c
extern double TMiles;

extern int consmode, rMode;                 // chmode.c
extern char *modemsg[];

extern int Resetear;                        // net\ionet.c

extern int ActiveDump;                      // DumpErr.c

extern int adic;                            // Handler a la ventana adicional.

extern int RegisterLocked;                  // Registradora bloqueada desde la red.

extern int ckaNro, supervNro;				// cajero.c
extern char *ckaNom, *supervNom;

//   Mozos.cpp
extern int venNro, cVen_, VendedPrinted;
extern char *vFuncName;
extern int globalVendedTop;

extern char *f_des, f_qual, *f_adicdes;
extern double f_mult, f_monto;
extern int f_tasa, f_bulto, f_variac;

extern int FactuFlag, PermFactuFlag;		// Factu.cpp
extern int listap_perm;
extern int reparto_nro, reparto_perm;

//  Subtot.c
extern struct acum *zz_depto, *zz_mpag, *zz_vend;
extern int zz_deptoQ, zz_mpagQ, zz_vendQ;
extern class ::ArrayList *ZetaCajeros;

//  Mtask.c
#define restore_safe_stack

//  Envases.c
extern int EnvasLogoCnt, EnvasPieCnt;

//  Premios.c
extern int PremiosLogo, PremiosPie;

//  Variab.cpp
extern long ticket, zeta, cAnula;
extern double *netos, *iva, tVtaReal, tVta, tAnula, sobret;
extern char *tmsg_;
extern int monitorMode;

//  esquema de colores para la programacion.
extern short prograEsq[];

//  mpagos.c
extern struct mpag_ *mpagos;
extern int GlobalCCSelected, chkMaxSal;
extern char *c_nombre, *c_numero, *c_dni, *c_direcc, *c_telef, *c_cod, *c_localidad,
	*c_condiva, *c_saldo, *c_salmax, *c_savecod, *c_dnicompr, *c_nomcompr, *c_fecha, *ccte_Path;
extern char *c_codpostal, *c_saldopromo, *c_repadescrip;
///Agregado para percepcion
extern char *c_percep, *c_fechapib, *c_alicuota, *c_leyenpercep;

//Agregado para Percepcion Tissh Salta
extern char *c_tisshalic, *c_tisshpercep;


//   Pos.cpp
extern int Inicializando, GlobalRngCount, RngLimit, errorCode;
extern char *posVersion;

//   Corri.cpp
extern int globalCorri;

//   Subtot.c
extern double printerTot;

//   Dump.cpp
extern char *tdes_;				// Temporar. P/cualquier uso.
extern class dbf tDbf;//,tTmp;		// TRANS.DBF
extern int globalInitDump;		// Indica que esta inicializando.
extern dbf rPlu;				// Para el reporte de venta de plus.
extern int globalDelayPrint;	// Para impresiones diferidas.

//   Sync.cpp
extern int HostW, rebootear;

//   PosDisp.cpp
extern int detalle;				// Handler a la window.

//   Suspend.cpp
extern int actSusp;				// Si está en progreso una suspension.

//   Messg.cpp
int LoadVoucher(int nro);		// Carga una definicion de voucher.

//   Security.cpp
int RequestPasswd(char *k);		// Requiere un password antes de seguir.

//   TickFac.cpp
extern int GlobalTF;
//extern char f_respon, f_letra;

//  Manejo de logos
void LoadLogos(void);
extern bool reloadLogos;




int ForceCC(int cual);
extern int FiscalForceZeta;
int InitPrinter(void);
extern struct lstat_
{
	long prnstat;
	long fiscstat;
	long lastcomp;
	long fecop;
	long horaop;
	long lastcie;
	long archi1;
	long archi2;
    char archi3[12];
    char archi4[3];
} lstat;

///////////////////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_POSAPI
#define INCLUDED_POSAPI

//   Tabla con la lista (y orden) de las funciones que se bootearan al arranque.
extern struct BootList_ {
	int  (*InitFunc)(void);		// Funcion que se usara para inicializar (puede ser NULL)
	void (*ResetFunc)(void);	// Funcion que se usara para apagar (puede ser NULL).
	char *InitMsg;				// Mensaje que se displayara durante el proceso.
	int  Initialized;			// Reservado - Uso interno.
} BootList[];

extern int cInit;

//   Lista de servicios que se deben ejecutar en forma periodica.
extern struct PeriodicList_
{
	void (*PeriodicFunc)(void);	// Función que se ejecutará.
	char *name;					// Nombre de la función.
	int  Slice;					// Slices que salteará.
	int  SliceActual;			// USO INTERNO - No tocar.
} PeriodicList[];
extern int cPeriod;

//   Interfaz estandar para manejar impresores fiscales.
#define PRN_FISCAL  1			/*   Capacidades del impresor   */
#define PRN_TICKFAC 2
#define PRN_SYNC    4
#define PRN_IDENTIFY 8			/* Soporta devolver CUIT y PV del dueño  */
#define PRN_CANLOGO  16			/* Soporta logos graficos  */
#define PRN_CANVOUCHER_TJ 32	/* Tiene un voucher custom para tarjetas */

#define SEP     '\x1c'
// extern char *pc;

extern char rxBuff[], txbuff[], txbuff2[], mdBuff[];

class PrinterItemPago
{
public:

	char   *DescripAdic;
	char   *Descrip;
	double Monto;

	PrinterItemPago(char *descrip, char *descripAdic, double importe)
	{
		DescripAdic = new char[50];
		Descrip = new char[50];
		Monto = importe;
		STRCPY(DescripAdic, descripAdic);
		STRCPY(Descrip, descrip);
	}
	~PrinterItemPago() { delete DescripAdic; delete Descrip; }
};

extern class PrinterFiscal_ 
{
protected:
	bool seqHigh;
	int lastResult;
	//char ptovta[10];
	char mpagAdicDesc[80];
	::ArrayList *stackPagos;

	char *FormatPkt(char *pkt, int len);
	char f_respon, f_letra;

public:	
	char ptovta[10];				//Cambiado de lugar
	int globalDelayPrint;			// El ticket se imprime solo al final.
	int GlobalVoucherFlag;
	int prnmudo, PrinterCap;        // Capacidades disponibles.
	int SyncPrinterTot;             // Opcion 'sincroniza'
	int xon930;						// Para control de flujo.
	int PrintTextAllowed;           // Algunos impresores fiscales limitan el
									// uso de la funcion 'PrintText'
	int pHandler;					// Para manejo de alertas.
	int ignoraError4;
	struct lstat_ lstat;			// Para almacenamiento del estado del impresor.
	char pktbuff[1500];				// Para almacenamiento temporario de paquetes.
	int debugMode;					// 0=No debug, 1=Dump RX, 2=Dump ALL
	bool solofact;					// Indica si el impresor solo imprime Facturas.	
	int TipoComprobante;
	long NumeroComprobante;

	int frepdesde, frephasta, frepsig;

	double TotalActual;

	//  Metodos para manejo de los comprobantes no fiscales.
	PrinterFiscal_();						// Constructor estatico.
    virtual int OpenVoucher(void);          // Abre un CNF
	virtual int PrintVoucher(char *des);	// Imprime texto en un CNF
	virtual int CloseVoucher(void);			// Cierra un CNF.

	virtual int OpenVoucher2(void);          // Abre un CNF sin encabezado ni colas	
	virtual int CloseVoucher2(void);			// Cierra un CNF.

	//  Metodos para manejo de los comprobantes fiscales.
	virtual int PrintItem(char *des, double mult, double monto,	// Imprime un item 
		          int tasa, char qual, int bulto, int variac, 	// (y abre un CF si no estaba abierto)
				  char *ades, double unitario, bool forzarCant,
				  int cajaUni);
	virtual int PrintSubtot(char *msg, int pFlag);				// Imprime (o calcula) el subtotal.
	virtual void MPagoAdicDesc(char *msg);						// Mensaje adicional para el siguiente medio de pago.
	virtual int PrintMPago(char *msg, double monto);			// Registra un medio de cobro.
	virtual int PrintClose();									// Cierra un ticket
	virtual int PrintDescto(char *msg, double monto);			// Imprime un descuento por el total.
	virtual int PrintText(char *msg);							// Imprime un texto cualquiera.
	virtual int PrintCancel(void);								// Cancela el ticket en curso.
	virtual int PrintZeta(void);								// Imprime el cierre diario.
	virtual void AjustarRedondeo(double redondeo);				// Ajusta el redondeo en el impresor, si corresonde.
	virtual int PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp); //Imprime la Percepcion
	virtual int PrintRecargo(char *msg, double monto);			// Imprime un recargo por el total.
	virtual void ObtenerAbierto(int &numticket, double &monto);	// Cancelara el primer ticket/Factura al iniciar POS

	//  Funciones obsoletas. Tratar de no utilizarlas mas.
	virtual int ChkPrnXonXoff(void);							// Chequea si el impresor está listo.
	virtual int PrintJournal(char *msg);						// Imprime en la cinta testigo.
	virtual int PrintTick(char *msg);							// Imprime en el ticket.
	virtual int PrintAmbos(char *msg);							// Imprime en ticket y CT
	virtual int Cutter(void);									// Corta el ticket.

	//  Funciones de utilidad variada.
	virtual int PrinterUpLoad(void);							// Carga los encabezamientos y pie.
	virtual int SetPie(char *msg);								// Cambia la primera linea del pie.
	virtual int SetPie(char *msg, int linea);					// Cambia la linea N del pie.
	virtual int SetPie2(char *msg);								// Cambia la segunda linea del pie.
	virtual int SetHeader(char *msg);							// Cambia la ultima linea del header.
	virtual int OpenDrwr(void);									// Abre la gaveta.
	virtual int FiscalInit(char *cuit, char *rs, char *regnro,
		char *fec, char*ptovta, char *inicAct, char *ibrut,
		char *respon);
	virtual void Owner(char *cuit, char *pv){}					// Datos del dueño del impresor.
	virtual void InstallLogo(char *filename);					// Instala logo grafico
	virtual void VoucherTJOnline(								// Voucher de tarjeta online
		char *marca,
		char *tarjeta,
		char *nombre,
		int  vencimiento,
		int  lote,
		int  cupon,
		int  autorizacion, 
		double monto,
		int cuotas,
		char *numcomercio,
		int numterminal,
		int tickfiscal,
		bool credito);
	virtual int docFiscStat();					//controla si hay un documento fiscal abierto

	virtual int GeneraReporteAfip(int fdesde, int fhasta);
	virtual int CopiaDocumento(int tipoDocumento, int nroDocumento);
	virtual int GeneraReporteAfipNew(int fdesde, int fhasta);
	virtual void ObtieneRangosReportes(int pdesde);
	virtual void ObtieneRangosReportesMes(int pdesde);


	//  Funciones auxiliares de comunicacion.
	virtual void txStrP(char *s);				// Transmite una string.
	virtual void txCharP(char c);				// Transmite un unico caracter.
	virtual char rxCharP(void);					// Recibe un unico caracter.
	virtual void txPkt(char *pkt, int len);		// Transmite un paquete desde el impresor.
	virtual int  rxPkt(void);					// Recibe un paquete desde el impresor.
	virtual int  xchgPkt(char *pkt);			// Intercambia un paquete con el impresor.
	virtual int  xchgPkt2(char *pkt, int len);	// Intercambia paquete BINARIO con el impresor.
	virtual int  chk(void);						// Verifica el estado del impresor.

	virtual void txPkt2(char *pkt, int len);		// Transmite un paquete desde el impresor.
	virtual int  rxPkt2(void);					// Recibe un paquete desde el impresor.
	virtual int  xchgPkt3(char *pkt);			// Intercambia un paquete con el impresor.


	//   Rutinas auxiliares.
	virtual char *getfld(char **pc);			// Devuelve un campo, desde un paquete.
	virtual unsigned long gethexa(char **pc);	// Devuelve un campo, interpretado como hexadecimal.
	virtual unsigned long getdec(char **);		// Devuelve un campo, interpretado como decimal.
	virtual int  chkFiscStat(void);				// Chequea los bits de estado fiscal.
	virtual int  chkPrnStat(void);				// Chequea los bits de estado del hardware
	virtual char *ChkDes(char *src, char *dst);	// Chequea la validez de descripciones.
	virtual void Debug(void);					// Funciones de depuracion.
	virtual int  GetTickNro(int);				// Devuelve el nro de ticket (0=Ticket B, 1=Ticket A)
	virtual void CheckStatus();					// Valida el estado del impresor
	virtual void Configure(char *confParam);	// Configura el impresor.
	virtual char *PuntoDeVenta();				// Retorna el punto de venta actual
	virtual void FastVoucher(bool pHeader,		// Voucher rapido (si el impresor lo permite, sino redirige a vouchers lentos)
		char *l1, char *l2, char *l3);
} *pf;

//   Lista de 'device drivers' utilizables.
#define DV_PRINTER 'P'
//extern struct DeviceList_	{
//	void *ClassUsed;		// Clase que maneja al dispositivo.
//	char *name;				// Nombre que se utiliza para ubicar la definicion.
//	int  clase;				// Clase a la que corresponde el dispositivo.
//} DeviceList[];
//extern int cDev;			// Cantidad de dispositivos a utilizar.

extern int    rxCount, cue;
//extern char   rxBuff[], txbuff[], txbuff2[], mdBuff[];
extern time_t rxTimer;
extern int    ErrorDuringRx;
extern int    PrinterCap;
extern char   GlobalComprob[];
extern char  *ptrFields[];
extern int    cFields;
extern char  *logoFile;
extern char  *pluAdicText;
#endif


//  Definicion de los posibles vouchers. (Voucher.cpp)
extern struct voucher
{
	char **msg;								// Lineas del voucher.
	int cLines;								// Cantidad de lineas validas.
	bool sinencycola;						// indica si el voucher imprimira el encabezado y cola

	voucher() {	msg = new char*[50]; }
	~voucher() { delete msg; }

} *voucherList[];
extern int voucherLoadFlags[];				// Para saber si ya se cargo el voucher.

void StackVoucher(int nro);					// Apila vouchers que se deben imprimir.
void PrintVouchers();						// Imprime los vouchers apilados.
void VoucherVar(char *name, char *value);	// Variables para voucher.
void VoucherVar(char *name, int value);
void VoucherVar(char *name, double monto);
void VoucherVar(char *name, double monto, bool esMoneda);
void ResetVouchers(void);					// Para eliminar los vouchers generados.

//  Promo.cpp
#include "promo.h"

//  Dump.cpp - Acumuladores de comprobantes fiscales.
extern int *countFac, *minNumFac, *maxNumFac;
extern double *ventaFac;
extern double m_tasas[][10], m_netos[][10], m_ivas[][10];

//  sync.cpp - Acumuladores para hacer el PINGVTA al server
extern double pingVenta, pingEfectivo;
extern int pinTickets, pingPtoVta;


class RegRelNotaCredito
{
public:
	char *NroTicket;
	char *NroNotaCredito;
	int  Fecha;
	int  Hora;
	int  Mp;
	double Saldo;
	bool Usado;

	RegRelNotaCredito()
	{
		NroTicket = new char[15];
		NroNotaCredito = new char[15];
		Fecha = 0;
		Hora = 0;
		Mp = 0;
		Saldo = 0;
		Usado = false;
	}

	~RegRelNotaCredito() { 
		delete NroTicket; 
		delete NroNotaCredito;
	}

	void Reset(){
		*NroTicket = 0;
		*NroNotaCredito = 0;
		Fecha = 0;
		Hora = 0;
		Mp = 0;
		Saldo = 0;
		Usado = false;
	}

	void AsignaTicket(int TipoCompTi, int PtoVta, int NumCompTi){
		char unaLetra = '-';
		if (TipoCompTi == 82)
			unaLetra = 'B';
		else if (TipoCompTi == 81)
			unaLetra = 'A';
		sprintf(NroTicket, "%04d%c%08d", PtoVta, unaLetra, NumCompTi);
	}

	void AsignaNotaCred(int TipoCompNc, int PtoVta, int NumCompNc){
		char unaLetra = '-';
		if (TipoCompNc == 113)
			unaLetra = 'B';
		else if (TipoCompNc == 112)
			unaLetra = 'A';
		sprintf(NroNotaCredito, "%04d%c%08d", PtoVta, unaLetra, NumCompNc);
	}
};
