/*   Opc.h   - Prototipos de variables de opcion.   */
#pragma once

//#include "arraylist.h"
#include <STDAFX.H>

extern int pantalla;		// Tipo de FORM a mostrar - Solo windows XP
extern int opcdec;          // Decimales en importes.
extern int caja, suc;       // Geografia de la ubicacion.
extern int ponline;			// Activa el procesamiento online de transacciones.
extern int prndelay, autoapp, recuany, autonro, usegraf, modoMayor, precioBulto, precioX3Un, precioUnit;
extern int z_depto, z_cajero, z_mpago, z_horar, z_clrlog, z_vended;
extern int z_plus, z_muestra;
extern int wlog;		// Momento en el que graba el log.
extern int cMpag;
extern int keyupper, keydown, keyenter, keycanc, keyreg, keyx1, keyx2, keyz;
extern int keyback, keyleft, keyright, keyout;
extern int modescan, emax, printplucod, printsubt, forcesubt, nocieTke, autocierre, resuArqueo, pdetaArqueo, pdetaFinArqueo, talon;
extern int printCant, printReduce;
extern int balanPrec;
extern int tipoArqueo;
extern int chkXon, M_VerRng, M_VerPie, M_VerPie0, M_PrintAtPie, M_NroVoucher, M_Modo, M_ParamModo;
extern int beep;
extern char *beepFile;
extern int cko_rebo, cko_opnd, cko_enab;
extern int ven_enab, ven_enmed, ven_rebot, ven_obli;
extern int videoType;
extern int defFactuMode;
extern int listap, defListaPrec, confListaPrec;
extern int cdigEmax;
extern int UsaLlave;
extern int CopyInterlace, CopyPacketSize;
extern int SyncPrinterTot;
extern int GetPlusUsingDos;
extern int prnmudo;
extern int ccte_Edit, ccte_Onln;
extern int sc_DvEnBase, sc_DvEnScanner, sc_AppendDV;
extern int terminateSSaver, globalSpecialScan;
extern int cc_UsaListas, cc_HastaSaldo, cc_SoloInicio, cc_ForzarMP, cc_Medio, cc_PorceMax;
extern int pluMult;
extern int pingDelay;
extern int  dmp_Depto, dmp_Voucher;
extern int permitePesado, tjOnline;
extern int corriMode, retiCod, cajFlota, autoAnul, permiteSubir;
extern int logoDelay;

extern bool usaTouch;		// Indica si se usara pantalla touch screen.
extern bool enterEnabled;

extern long lastact;
extern long chPermi[];      // Passwords para cambios de modo.
extern long DelayEmax;
extern bool flagTope, forceClrLog;

extern double topeLocal;
extern double limTicket,	// Limites de registracion
	limRenglon, 
	limCantidad;
extern double limConsFinal;
extern int AplicarPercep;
extern int AplicarTissh;
extern double MinimoTissh;

////variables para Mayor
//extern int minArt, mfraccion, descBulto, descFraccion; 
//extern char *mcondiva;
extern int filtradepto[];
extern char *strconbd, *dvariac, *dirpuntual, *dirpromo;
extern char *fpromo, *fcanastas;
//Variables para Percepcion
extern bool baplipercep;
extern bool baplitissh;

//para promociones por medio de pago
extern int gNumMedioPago;
extern double gImporteSinDescto; //Valor de la promocion sin el descuento
extern double gPorcDescto; //Porcentaje de Descuento en promocion Medio de Pago
extern int gNumTarjeta;

extern char *host;
extern char *rango;
extern char *interfaz;
extern char *KEYBOARD;
extern char *ven_des;
extern char *p_Log;
extern char *ccte_Path;
extern char *sc_MskPesable, *sc_MskCajero, *sc_MskSuperv, *sc_MskLlaveX1, *sc_MskLlaveX2, 
            *sc_MskLlaveZ, *sc_MskMozo, *sc_MskCliente;

extern short STARTVIDEOMEM;

extern unsigned ENDKEY;

extern ::ArrayList *logos;


extern double gtotalpercep;
extern double gtotalextracc;
extern double minlimextrac;
extern double maxlimextrac;
extern bool EnAutocierre;
extern long numsecuencia;

extern int hasarEop;
extern bool bPromoCobra;
extern int permite_dun14;
extern char *crStr;
extern int cntItemArqueo;

extern int puede_pasar;
extern bool elige_pasar;
extern int scanartcobradora;
extern int imprimiopercep;
extern int imprimiotissh;

extern bool enApplyAction;
extern double cntApplyPromo;
extern double puApplyPromo;

extern int checkLimFisc;
extern int clerk_ant;


///VARIABLES PARA SAP
extern char sapcnlvta[3];
extern double sapimpiva;
extern char saptienda[7];

extern ::ArrayList tjonAutor;
extern bool tjontoprint;
extern bool reloadtjon;



//PARA tomar el peso de la balanza
extern float pesoLeido;
extern int TieneBalanza;
extern int deptopesables[];
extern char PesadoEnBalanza[12];

//Control de precios puntuales
extern bool variacionpp, applyvarpp, facturapp;

//Para bloqueo de venta por codigo interno
extern int VtaxCodInterno;


//para excluir la venta de deptos
extern int nofichadeptos[];

//para obtener las ordenes de compra del servidor
extern int portOC;
extern char ipServOC[];

//Para promocion de Colegios
extern int codcolsel;

//Para promocion de Cupones
extern int cantcuping;
extern double montocuping;

//Para Percepcion TISSH
extern double gtotaltissh;

//Para habilitar Venta En Factura B solo con CUIT
extern bool FactBconCuit;

//Para habilitar Sistema Autor TIPRE
//extern bool AutorPinPad;

//Para Descto de Tipre, solo Prueba falta Desglose
extern double desctoTipre;

//para ver codigo SOS Movil
extern int smcodmaterial;


//Para habilitar la pantalla de carga para Carga Voucher en pantalla de busqueda
extern bool habilitaCargaVoucher;

//para Nueva impresora P-250 Hasar requiere indicar el codigo interno
extern unsigned long imprecodigo;

//para distinguir Ecommerce
extern bool EsEcommerce;
extern long long NumPedEcommerce;
extern int NroVoucherEcommerce;

//Para Acumular los descuentos de Acciones Promocionales
extern double AcumDesctoPromo;

//Para Acumular cantidad Ingresada
extern int taCant;

//Para ingresar Numero de Vendedor
extern bool IngNroVdor;

extern bool EsRI;
//Para grabar el scan en la impresora nueva 250
extern char dscan[30];
//Para Numerar los Retiros
extern int NroRetiro;

//Para recuperar los reportes electronicos luego de ZETA Fiscal
extern bool bSaleRepElec;
extern int DesdeRepElec;
extern int HastaRepElec;


//Para Nota de Credito
extern bool EsNotaCredito;
extern int TipoCompNc;
extern int NumCompNc;
extern char codMotivoNc[10];

//Para Caja Libre
extern bool TieneFU;
extern int portCL;
extern char ipServCL[20];
extern int ComPortCL;
extern int SegsLuzCL;


extern double mpTotal;
extern char mpSeqOp[31];
extern int mpNumVoucherDev;
extern int mpRepCons;


//para Habilitar precio Mayorista pesables
extern bool mayPrecPesable;

//para Janis Ecommerce
extern bool janisEcom;

//Cliente Marcado;
extern int ClienteBenef;

//para importes de Janis
extern char xsPrecEcom[30];

extern bool xsJanisEsPesable;

//Para self Checkout
extern bool selfCheckout;
extern double acumPeso;
extern double limCantxTicket;
extern double pesoBalanControl;
extern bool tieneBalanzaControl;
extern int cajeroSelf;
extern int timeoutSelf;
extern int cntEsperaTouch;

extern int actMarcaCli;
extern double acumSaldoPromo;

//para excluir la venta de articulos
extern int nofichaarts[];

extern int tjPrisma;


//para testear si esta en pantalla de clientes en Selfcheckout
extern bool enSeleccionCliente;


//para almacenar el comprobante original sobre el que se realiza NC
extern char nroTicketOriginal[14];

//Para Verificar las notas de credito en la tabla
extern RegRelNotaCredito *ncControl;
//para asignar el medio de pago por defecto de la Nota Credito, por defecto sera 33
extern int NumeroMpNc;

//Para Manejar la cantidad de digitos decimales en la balanza
extern int CantDigDecBal;

//Agregado para Manejo de Percepcion IVA
extern bool bApliPerIva;
extern int AplicarPerIva;
extern bool imprimioPerIva;
extern double gTotalPercepIva;


extern double tresSinInteres;
extern double seisSinInteres;

//Agregado para Regimen Transparencia Fiscal
extern double AcumIVA;
extern double AcumImpuestosInternos;

///Para almacenar el vendedor Mayorista
extern int nroVendMay;

//para validar si el ticket fue pasado a la cobradora
extern bool pasadoACobradora;

//para almacenar el numero de supervisor que hace anulacion total
extern int numSuperv;

//para almacenar el numero de registro en el caso que se hace una venta con Vendedor Mayorista
extern long numRecFunc4;


extern int tjSmartPoint; //Agregado para Verifones Smart Point

//para almacenar el numero de tarjeta de mercadopago e imprimirlo
extern char mpSeqIdOrden[31];

//para almacenar e imprimir el motivo de Nota de credito
extern char motivoNotaCredito[51];
extern char supervNotaCredito[51];

//para almacenar e imprimir la leyenda de la promocion finde año sobre ruedas
extern char leyendaFinde[51];


//Agregado para Habilitar la carga de señas de Clientes
extern int CargaSenia; 
extern double gtotalsenia; //Total Señas

//para Recuperar Datos Estacionamiento
extern bool procesaParking;
extern int idParking;


//Para habilitar la integracion de PrismaEcr
extern bool EcrPrisma;

extern bool dummyHabil;

//Para habilitar la integracion de QR Cashout MercadoPago
extern bool mpCashout;
