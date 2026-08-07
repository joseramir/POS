//
//  Variab.c
//  Definición de las variables globales.
//

//#include "stdafx.h"
#include <STDAFX.H>

char *tmsg_ = new char[200];
char *my_Linea, *my_Usa, *comando, *cajas, *interfaz, *ven_des, *vFuncName, *f_des;

char TxInProgress, f_qual;

unsigned uQueue, uDest, safe_ss, safe_sp;

int COM_PRN, COM_EMAX, COM_SCAN, PANTACOLOR, COM_BALAN, COM_BCL, COM_NLD;
int net_finalizar, net_finalizad, my_Nro, my_Lock, my_PCNro, my_Broad, my_Testigo, IamTest, my_Delay;
int ConnectSource, opcdec, caja, suc;                  
int z_depto, z_cajero, z_mpago, z_horar, z_clrlog, z_vended, z_plus, z_muestra;
int keyupper, keydown, keyenter, keycanc, keyreg, keyx1, keyx2, keyz;
int keyback, keyleft, keyright, keyout;
int modescan, emax, printplucod, printsubt, forcesubt;
int M_VerRng, M_VerPie, M_VerPie0, M_Habil, M_Modo, M_ParamModo;
int cko_rebo, cko_opnd, cko_enab;
int ven_enab, ven_enmed, ven_rebot, ven_obli;
int cLinesLogo, cLinesPie, cLinesEpson;         
int pRepeat, dRepeat, pSubtot;      
int GlobalReprint;
int cMpag, cDep;                            
int ActivaNewPlu, PromoActi, consmode, Resetear, ActiveDump, adic, RegisterLocked, ckaNro, supervNro, venNro, cVen_;                
int f_tasa, f_bulto, f_variac, FactuFlag, zz_deptoQ, zz_mpagQ, zz_vendQ, zz_cajeQ, EnvasLogoCnt, EnvasPieCnt;
int PremiosLogo, PremiosPie, rMode, inOper, cDecim, borrar, cTasa, defFactuMode, listap, defListaPrec, listap_perm, confListaPrec;
int GlobalRngCount, GlobalNetAvail, monitorMode, corriMode, logoDelay;
/////Agregado manejo mayorista
//int minArt, mfraccion, descBulto, descFraccion;
//char *mcondiva;
int filtradepto[50] = {0};
char *strconbd, *dvariac, *dirpuntual, *dirpromo;
char *fpromo, *fcanastas;
///Agregado manejo percepciones
bool baplipercep = false;
bool baplitissh = false;

int checkLimFisc; //Validara si controla el limite fiscal de la impresora

long ticket, zeta, cAnula, xCntAnul;

double xReg, yReg, zReg, pTot, xTot, xCnt, xTotAnul, xPrecEcom;       
double f_mult, f_monto, tVta, tVtaReal, tAnula, sobret, TMiles;
double *netos = new double[4];
double *iva = new double[4];

bool enterEnabled, reloadLogos;

view_ view;

::ArrayList *ZetaCajeros = NULL;
::ArrayList *logos = NULL;

acum *zz_depto;
acum *zz_mpag;
acum *zz_vend;

dbf dbplu;
dbf dbscan;


double gtotalpercep; //Total Percepciones
double gtotalextracc; 
double minlimextrac;
double maxlimextrac;
bool EnAutocierre = false;
long numsecuencia;

int hasarEop;
bool bPromoCobra;
int permite_dun14; //Habilita los DUN14 en caja
int cntItemArqueo; //Cantidad de items para arquear
int puede_pasar; //para permitir pasar un ticket que excede el monto fiscal
bool elige_pasar; //Para preguntar si el ticket se pasa a cobradora, solo si puede_pasar = true
int scanartcobradora; //para permitir que scanee articulos en las cobradoras
int imprimiopercep;		//para controlar que la percepcion de IIBB se imprima solo una vez
int imprimiotissh;

//Agregadas para manejo de promociones
bool enApplyAction;
double cntApplyPromo;
double puApplyPromo;

//para promociones por medio de pago
int gNumMedioPago;
double gImporteSinDescto; //Valor de la promocion sin el descuento
double gPorcDescto; //Porcentaje de Descuento en promocion Medio de Pago
int gNumTarjeta;


int clerk_ant;
///VARIABLES SAP
char sapcnlvta[3];
double sapimpiva;
char saptienda[7];


::ArrayList tjonAutor = ::ArrayList(true, "InfoTarjetasStaticCtor2"); //para tarjetas online autorizadas
bool tjontoprint; //para validar si hay que imprimir voucher de tarjetas
bool reloadtjon; //para recargar las tarjetas online en caliente (al finalizar un ticket)

//PARA tomar el peso de la balanza
float pesoLeido;
int TieneBalanza;
int deptopesables[50];
char PesadoEnBalanza[12];

//Control de precios puntuales
bool variacionpp, applyvarpp, facturapp;

//Para bloqueo de venta por codigo interno
int VtaxCodInterno;


//para excluir la venta de deptos
int nofichadeptos[50] = {-1};

//para obtener las ordenes de compra del servidor
int portOC;
char ipServOC[20];

//Para promocion de Colegios
int codcolsel;

//Para promocion de Cupones
int cantcuping;
double montocuping;

//Para Percepcion TISSH
double gtotaltissh;

//Para habilitar Venta En Factura B solo con CUIT
bool FactBconCuit;

//Para habilitar Sistema Autor TIPRE
//bool AutorPinPad;

//Para Descto de Tipre, solo Prueba falta Desglose
double desctoTipre;

//para ver codigo SOS Movil
int smcodmaterial;


//Para habilitar la pantalla de carga para Carga Voucher en pantalla de busqueda
bool habilitaCargaVoucher;

//para Nueva impresora P-250 Hasar requiere indicar el codigo interno
unsigned long imprecodigo;


//para distinguir Ecommerce
bool EsEcommerce;
long long NumPedEcommerce;
int NroVoucherEcommerce;

//Para Acumular los descuentos de Acciones Promocionales
double AcumDesctoPromo;

//Para Acumular cantidad Ingresada
int taCant;

//Para ingresar Numero de Vendedor
bool IngNroVdor;


bool EsRI;
//Para grabar el scan en la impresora nueva 250
char dscan[30];
//Para Numerar los Retiros
int NroRetiro;

//Para recuperar los reportes electronicos luego de ZETA Fiscal
bool bSaleRepElec;
int DesdeRepElec;
int HastaRepElec;

//Para Nota de Credito
bool EsNotaCredito;
int TipoCompNc;
int NumCompNc;
char codMotivoNc[10];

//Para Caja Libre
bool TieneFU;
int portCL;
char ipServCL[20];
int ComPortCL;
int SegsLuzCL;


double mpTotal;
char mpSeqOp[31];
int mpNumVoucherDev;
int mpRepCons;


//para Habilitar precio Mayorista pesables
bool mayPrecPesable;

//para Janis Ecommerce
bool janisEcom;

//Cliente Marcado;
int ClienteBenef;

//para importes de Janis
char xsPrecEcom[30];

bool xsJanisEsPesable;

//para Self Checkout
bool selfCheckout;
double acumPeso;
double limCantxTicket;
double pesoBalanControl;
bool tieneBalanzaControl;
int cajeroSelf;
int timeoutSelf;
int cntEsperaTouch;

int actMarcaCli;
double acumSaldoPromo;


//para excluir la venta de articulos
int nofichaarts[100] = {-1};

//para testear si esta en pantalla de clientes en Selfcheckout
bool enSeleccionCliente;

//para almacenar el comprobante original sobre el que se realiza NC
char nroTicketOriginal[14];

//Para Verificar las notas de credito en la tabla
RegRelNotaCredito *ncControl = new RegRelNotaCredito();
//para asignar el medio de pago por defecto de la Nota Credito, por defecto sera 33
int NumeroMpNc;

//Para Manejar la cantidad de digitos decimales en la balanza
int CantDigDecBal;

//Agregado para Manejo de Percepcion IVA
bool bApliPerIva = false;
bool imprimioPerIva = false;
double gTotalPercepIva = 0;

double tresSinInteres = 0;
double seisSinInteres = 0;

//Agregado para Regimen Transparencia Fiscal
double AcumIVA = 0;
double AcumImpuestosInternos = 0;

///Para almacenar el vendedor Mayorista
int nroVendMay = 0;

//para validar si el ticket fue pasado a la cobradora
bool pasadoACobradora;

//para almacenar el numero de supervisor que hace anulacion total
int numSuperv;

//para almacenar el numero de registro en el caso que se hace una venta con Vendedor Mayorista
long numRecFunc4;

//para almacenar el numero de tarjeta de mercadopago e imprimirlo
char mpSeqIdOrden[31];

//para almacenar e imprimir el motivo de Nota de credito
char motivoNotaCredito[51];
char supervNotaCredito[51];

//para almacenar e imprimir la leyenda de la promocion finde año sobre ruedas
char leyendaFinde[51];


//Agregado para Habilitar la carga de señas de Clientes
int CargaSenia; 
double gtotalsenia; //Total Señas

//para Recuperar Datos Estacionamiento
bool procesaParking;
int idParking;


//Para habilitar la integracion de PrismaEcr
bool EcrPrisma;

bool dummyHabil;

//Para habilitar la integracion de QR Cashout MercadoPago
bool mpCashout;