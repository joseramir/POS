//
//   Plu.cpp
//   Manejo de la venta de articulos con precio fijo.
//
//   Bateria de pruebas para la funcion plu:
//
//       0) Programar el modo de barra pesada en MONTO y el modo de multiplicacion al peso en MONTO.
//		 1) Digitar directamente el codigo de un plu normal. Ver que lo venda bien.
//		 2) Pulsar PLU de nuevo y ver que el codigo vendido en (1) se venda de nuevo, por repeticion.
//		 3) Probar la venta por multiplicacion de un codigo normal. Ver que lo venda bien.
//		 4) Pulsar PLU de nuevo y ver que el codigo vendido en (3) NO se venda de nuevo (no puede repetir)
//		 5) Ingresar una cantidad con decimales y multiplicarla por un plu normal. Ver que salga error.
//		 6) Efectuar todas las pruebas de (1) a (5), con el codigo de barras de un plu normal.
//		 7) Digitar el codigo de un producto pesado y ver que salga un error.
//		 8) Digitar un monto y multiplicarlo por un codigo pesado. Ver que lo venda bien.
//		 9) Digitar una barra de codigo pesado, y ver que la venda bien.
//	    10) Programar el modo de barra pesado en CANTIDAD y el modo de multiplicacion al peso en MONTO.
//		11) Repetir las pruebas, desde (1) a (9)
//		12) Programar el modo de barra pesado en MONTO y el modo de multiplicacion al peso en CANTIDAD.
//		13) Repetir las pruebas, desde (1) a (9)
//		14) Programar el modo de barra pesado en CANTIDAD y el modo de multiplicacion al peso en CANTIDAD.
//		15) Repetir las pruebas, desde (1) a (9)

#include <stdafx.h>
#include "funcs.h"

#ifdef POSXP
#include "DbfBrowse.h"
#include "GCStatic.h"
#include "StaticDump.h"
#include "frmIngNumPedido.h"
#endif

//   Variables de depuracion locales.
#ifdef POSDEBUG
int DBGActiPlus;
#endif

// Prototipos externos
void ReprocTransMayor(DDplu_ *vta, bool anulFlag);
int PrecioMayor(unsigned long cod);

//   Referencias externas
extern char *PremLine;

//   sc_ModoBarra: Se utiliza para conocer el tipo de codigo de barras decodificado, dado que puede
//   ser que la barra indique CANTIDAD o MONTO a cobrar. Los posibles valores son: 
//		0- No se detecto un codigo de barras decodificable (Puede ser un unitario)
//		1- yReg es CANTIDAD
//		2- yReg es MONTO
int sc_ModoBarra, permiteSubir, printCant, printReduce, precioBulto, precioX3Un, precioUnit;

//   El siguiente flag es usado para indicar que un codigo especial (por ejemplo, el codigo de un
//   cajero) se detecto a traves de una tarjeta. En este caso, se saltea el pedirle al cajero/supervisor
//   el 'password' de acceso.
bool autoDetected = false;

//   La siguiente es una opcion configurable que permite que el cajero digite "monto x plu" para
//   articulos pesados. Cuando esta bloqueada, la unica forma de vender articulos pesados sera
//   escaneando la barra.
int permitePesado;

//   Obtiene, por configuracion, el precio al que estan etiquetando las balanzas.
int balanPrec;

//   La tabla 'MPLU' tiene un minimo de 3 precios, pero podria tener mas. Cuando se carga el MPLU, coloca
//   en esta variable el maximo precio existente. (la columna prec)
int maxMpluPrec;
bool agregaIvaRi;

//   Texto adicional que se imprimira con el siguiente plu
char *pluAdicText;

//   Variables Globales.
char *usacod = new char[100];
char *usades = new char[100];
void *idxplu;
void *idxscan;
struct DDplu_ dplu;
int globalProgramPlu, GlobalGetPrec, globalSpecialScan;
int sc_DvEnScanner, sc_DvEnBase, pluMult;
char *sc_MskPesable;
char *sc_MskCajero;
char *sc_MskSuperv;
char *sc_MskLlaveX1;
char *sc_MskLlaveX2;
char *sc_MskLlaveZ;
char *sc_MskMozo;
char *sc_MskCliente;
int hWin = 0;
EditWindow *pManualEdit;
BDecimal *basePrec = NULL;
BDecimal *precio1 = NULL;
char PLU_TBL[]      = "pos\\mplu.dbf";
char PLU_IDX1[]     = "pos\\mplucod.idx";
char PLU_IDX2[]     = "pos\\mpludes.idx";
char PLU_IDX3[]     = "pos\\mplumarc.idx";
char SCAN_TBL[]     = "pos\\mscan.dbf";
char SCAN_IDX1[]    = "pos\\mscancod.idx";
char SCAN_IDX2[]    = "pos\\mscansc.idx";
char NEW_PLU[]      = "pos\\mplu.new";
char NEW_SCAN[]     = "pos\\mscan.new";
char NEW_PLUIDX1[]  = "pos\\mplucod.new";
char NEW_PLUIDX2[]  = "pos\\mpludes.new";
char NEW_PLUIDX3[]  = "pos\\mplumarc.new";
char NEW_SCANIDX1[] = "pos\\mscancod.new";
char NEW_SCANIDX2[] = "pos\\mscansc.new";
dbf chknew;
static char *uCod = new char[30];
EditWindow *prograEdit = NULL;

int pruebaCondiva();
int EstaEnDep(char *dep);
int EstaEnDepExclu(char *dep);
int EstaEnArtExclu(char *art);

// Corrige en una descripcion los caracteres que tienen distinta representacion regional, 
// como las vocales asentuadas, enies, etc.
void PatchRegionalChars(char *des)
{
	unsigned char *p = (unsigned char *)des;
	while(*p) 
	{
		switch(*p)
		{
			case 242: *p = 'N'; break;
			case 241: *p = 'n'; break;
		}
		p++;
	}
}

//   La siguiente funcion es utilizada durante la iniciacion del programa,
//   para verificar el error-code retornado por las funciones db.
//   Si algo no sale bien, entonces aborta la ejecucion del programa
//   pos usando AbEnd.
int checkerr(int res)                   // Chequea errores de instalacion.
{
	if(res) return 0;
	sprintf(msg, "Error %d abriendo archivos de plus.\n", res);
	AbnormalEnd(msg);
	return 1;
}

//   En las bases de plus, los campos de codigos son strings completadas a
//   a la izquierda con ceros. Esta funcion prepara los codigos para ser
//   utilizados en los seek de las bases de datos.
char *padl(double cual, int cuanto)
{
	char frmt[100];
	
	sprintf(frmt, "%%0%d.0lf", cuanto);
	sprintf(usacod, frmt, cual);
	return usacod;
}

//   Genera un digito verificador, para la string que se le pasa. Los digitos verificadores que 
//   genera son los que utiliza la codificacion EAN.                                              **
//
//   El algoritmo (compatible EAN y DUN14) es:
//       - Completar a un numero par de digitos (rellenando con ceros a la izquierda).
//       - Sumar los digitos impares por un lado y los pares por otro.
//       - Sumar la (suma de pares * 3) mas la (suma de impares).
//       - El digito verificador es lo que hay que sumar al nro recien
//         calculado, para llegar a un multiplo de 10.
char dv(char *ucod)
{
	int pares = 0, impar = 0;
	char tcod[30];
	
	STRCPY(tcod, ucod);
	while((strlen(tcod) % 2) != 0)
	{
		memmove(tcod + 1, tcod, strlen(tcod)+1);
		tcod[0] = '0';
	}

	int lenCod = strlen(tcod);
	for(int index = 0; index < lenCod; index += 2)
	{
		impar += (tcod[index] - '0');
		pares += (tcod[index + 1] - '0');
	}

	return ((10 - (((pares * 3) + impar) % 10)) % 10) + '0';
}

//   Flag que se activa cuando se detecto un DUN14
bool dun14;

//   Valida que el DV embebido en el scanner pasado sea valido.
bool ValidateDV(char *tcod)
{
	if(sc_DvEnScanner)
	{
		int len = strlen(tcod);
		char xdv = tcod[len - 1];
		tcod[len - 1] = 0;
		char calcDv = dv(tcod);
		tcod[len - 1] = xdv;
		if(xdv != calcDv) return false;
	}
	return true;
}

int TestDUN14(double ucod)
{
	char frmt2[20];

	sprintf(frmt2, "%014.0lf", ucod);
	
	//   Prepara el scanner...
	dun14 = false;	

	int realLen = strlen(frmt2);
	for(char *p = frmt2; *p && *p == '0'; p++)
		realLen--;

	if (realLen == 14)
	{
		return 14;
	}
	return 0;
}

//   La siguiente funcion, segun la configuracion de 'opciones.ini', genera el
//   codigo apropiado de busqueda de una string en el scanner. Retorna TRUE si
//   lo encontro, sino, false.
int SeekInMscan(double ucod)
{
	char *tScanner;

	//   Prepara el scanner...
	dun14 = false;
	
	tScanner = padl(xReg, 14);
	sprintf(tmsg, "SeekMS - tScanner='%s'", tScanner);
	WriteBootLog(tmsg);

	int realLen = strlen(tScanner);
	for(char *p = tScanner; *p && *p == '0'; p++)
		realLen--;
	
	//   Mira si estamos ante un codigo DUN14
	if((realLen == 13 && !sc_DvEnScanner) || (realLen == 14 && sc_DvEnScanner))
	{
		// Valida el digito verificador
		WriteBootLog("SeekMS - Parece DUN14");
		if(!ValidateDV(tScanner)) { WriteBootLog("SeekMS - Fallo DV 1"); return 0; }

		// Activa el codigo DUN14
		dun14 = true;
		STRCPY(tScanner, tScanner + 1);				// Remueve el flag logistico
		if(sc_DvEnScanner)							// Recalcula el DV
		{
			int tlen = strlen(tScanner);
			tScanner[tlen - 1] = 0;
			tScanner[tlen - 1] = dv(tScanner);
		}
	}
	else
		STRCPY(tScanner, tScanner+1);

	// Si el scanner tiene DV, entonces lo valida.
	if(!ValidateDV(tScanner)) { WriteBootLog("SeekMS - Fallo DV 2"); return 0; }

	//   Adapta el codigo a LO QUE ESTE EN LA BASE DE SCANNERS...
	if(sc_DvEnBase)
	{
		if(!sc_DvEnScanner)
		{
			//   Debe agregar el digito verificador antes de buscar.
			memmove(tScanner, tScanner+1, 12);
			tScanner[12] = dv(tScanner);
			WriteBootLog("SeekMS - Agrego DV");
		}
	}
	else
	{
		if(sc_DvEnScanner)
		{
			//   Debe quitar el digito verificador a lo leido por la pistola.
			memmove(tScanner+1, tScanner, 12);
			tScanner[0] = '0';
			WriteBootLog("SeekMS - Quito DV");
		}
	}

	// Busca en la tabla.
	sprintf(tmsg, "SeekMS - Scanner que se busca='%s'", tScanner);
	WriteBootLog(tmsg);

	if(!dbscan.Seek(tScanner))
    {
		WriteBootLog("SeekMS - Fallo 1 - Trata 2");
        STRCPY(tScanner, tScanner+1);
        return dbscan.Seek(tScanner);
    }
	WriteBootLog("SeekMS - Ok 1");
    return 1;
}

//   En modo de consulta, los procesos de plu's se dirigen a la siguiente
//   funcion para ser procesadas.
void ConsPlu()
{
	int found;
	double importe, x;

	if(xReg == 0.0) return;
	if(SeekInMscan(xReg))
	 {
		 dbscan.Get("cod", usacod);
		 found = dbplu.Seek(usacod);
		 dplu.s_c_flag = 1;
	 }
	 else
	 {
		 found = dbplu.Seek(padl(xReg, 6));
		 dplu.s_c_flag = 0;
	 }
	 if(!found)
	 {
		 Alert("No encuentro al Articulo scaneado o tecleado", "");
		 return;
	 }

	 dplu.cod = atol(usacod);
	 dbplu.Get("des", usades);      // Toma los datos del articulo.
	 //PatchRegionalChars(usades);
	 switch(listap)
	 {
		case 2:  dbplu.Get("precio2", msg);  break;
		case 3:  dbplu.Get("precio3", msg); break;
		case 4:  dbplu.Get("precio4", msg); break;
		case 5:  STRCPY(msg, "0"); break;
		default: dbplu.Get("precio", msg); break;
	 }

	 x = atof_(msg);
	 store(x, dplu.xReg);
	 dbplu.Get("dep", msg);
	 dplu.dnro = atoi(msg);
	 dbplu.Get("al_peso", msg);
	 dplu.pesado = (msg[0] == 'T')? 1 : 0;
	 dbplu.Get("iva", usacod); //dbplu.Get(EsRI ? "ivari" : "iva", usacod);
	 dplu.iva = atoi(usacod);
     store(yReg = 1.0, dplu.yReg);

#ifndef POSXP
	 wActivate(adic);
	 wLocate(3, 1); wPstr("Codigo: "); sprintf(msg, "%8ld", dplu.cod);
					wPstr(msg);
	 wLocate(2, 1); wPstr(usades);
	 wLocate(4, 1); wPstr("Precio: "); sprintf(msg, "%.2lf", x);
					wPstr(msg);

	 importe = ((dplu.pesado)? (double) 1: yReg) * x;
	 DispCash(importe, 2);
	 CustDisp3(usades, "Consulta", HRng(importe));
#else
	 importe = ((dplu.pesado)? (double) 1: yReg) * x;
	 char tmsg[60];
	 if(dun14)
	 {
		dbplu.Get("uni", msg);
		int uni = atoi(msg);
		sprintf(tmsg, "Cod: %d (x %d)", dplu.cod, uni);
	 }
	 else
		 sprintf(tmsg, "Cod: %d", dplu.cod);

	 CustDisp3(usades, tmsg, HRng(importe));
#endif
	 HClear();
}

//   Para ventas a 'precio manual', pide el precio de una venta.
void GetManPrec()
{
	if(!hWin)
	{
		hWin = wDefine(6, 13, 14, 67, BORDERSINGLE, SNGCURS, prograEsq, "Precio Manual", "");
		pManualEdit = new EditWindow(hWin);
	}

	wActivate(hWin);
	wCls();
	wLocate(1, 2);  wPstr("Codigo:");
	wLocate(2, 2);  wPstr("Nombre:");
	wLocate(4, 2);  wPstr("Precio Manual:");
	sprintf(msg, "%ld", dplu.cod);
	wLocate(1, 10); wPstr(msg);
	wLocate(2, 10); wPstr(usades);
	msg[0] = 0;
	switch(defListaPrec)
	{
		case 1:  dbplu.Get("precio",  msg);  break;
		case 2:  dbplu.Get("precio2", msg);  break;
		case 3:  dbplu.Get("precio3", msg);  break;
		case 4:  dbplu.Get("precio4", msg);  break;
		default: dbplu.Get("precio",  msg);  break;
	}
	
	Trim(msg);
	pManualEdit->ClearGets();
	pManualEdit->AddGet(4,17, NUMBER_RIGHT_ALIGNED, 8, 0, msg, NULL, NULL, NULL);
	pManualEdit->AnimaGets(0);
	wDeactivate(hWin, 0);
	HClear();
}


void FacturarRecargo(int tipo, BDecimal recargo)
{
	if(recargo.Abs() < 0.01) return;
	if(tipo == TIPORECA_FINANCIERO)
		dplu.cod = 99997;
	else
		throw "Error Interno - TIPORECA no definido";

	dbplu.Setorder("mplucod");
	bool found = dbplu.Seek(padl((double)dplu.cod, 6));
	if(!found)
	{
		sprintf(tmsg, "El PLU de recargo '%ld' no esta definido", dplu.cod);
		Alert(tmsg, "No se cobrara recargo!!!");
		return;
	}
	char *auxmonto = recargo.ToString();
	Decimal auxdec = System::Convert::ToDecimal(gcnew System::String(auxmonto));	
	
	long precuni = System::Convert::ToInt64(auxdec * 100);
	double importe = ((double)precuni) / 100;
	double cantidad = 1.0;
	xTotal.AddRecFinan(importe);

	dplu.s_c_flag = 0;					// Indica que el codigo fue TECLEADO
	dplu.func = DPlu;					// ID de la funcion
	dplu.cod = atol(usacod);			// Codigo del articulo.
	dplu.envas = 0;
	dbplu.Get("des", usades);			// Descripcion.
	//PatchRegionalChars(usades);
	dbplu.Get("dep", msg);				// Departamento relacionado
	dplu.dnro = atoi(msg);
	dbplu.Get("subdep", msg);
	dplu.sdnro = atoi(msg);
	dplu.uni = 1;
	dplu.esBulto = false;
	dbplu.Get("iva", usacod); //dbplu.Get(EsRI ? "ivari" : "iva", usacod); 
	dplu.iva = atoi(usacod);
	dplu.pesado = 0;
	dplu.anulado = 0;
	dplu.otroP = 1;
	dplu.precuni = precuni;
	dplu.fecha = (long) date();							// Fecha, hora y cajero de la registracion
	dplu.hora = (long) time_();
	dplu.cajero = ckaNro;
	dplu.ppuntual = 0;
	sprintf(dplu.pesoenbalanza, "%.3lf", 0.0);
	dbplu.Get("premio", msg);
	dplu.umillas = atoi(msg);
	store(importe, dplu.xReg);							// Importe total a cobrar
	store(cantidad, dplu.yReg);							// Cantidad para la multiplicacion
	store((double)dplu.cod, tmsg);						// Codigo originalmente tipeado o scaneado
	if(strlen(tmsg) > 13) tmsg[13] = 0;
	STRCPY(dplu.origCode, tmsg);
	STRCPY(dscan, "7790001001047");
	ProcPlu(&dplu, 1);                                  // GRABA!!!
}

//Agregado para recupero de gastos
void FacturarMpagoRecargo(int tipo, BDecimal recargo)
{
	if(recargo.Abs() < 0.01) return;
	if(tipo == TIPORECA_RECUPERO)
		dplu.cod = 50545;
	else
		throw "Error Interno - TIPORECAMPAGO no definido";

	dbplu.Setorder("mplucod");
	bool found = dbplu.Seek(padl((double)dplu.cod, 6));
	if(!found)
	{
		sprintf(tmsg, "El PLU de recargo '%ld' no esta definido", dplu.cod);
		Alert(tmsg, "No se cobrara Recupero Gastos!!!");
		return;
	}
	char *auxmonto = recargo.ToString();
	Decimal auxdec = System::Convert::ToDecimal(gcnew System::String(auxmonto));	

	long precuni = System::Convert::ToInt64(auxdec * 100);
	double importe = ((double)precuni) / 100;
	double cantidad = 1.0;
	xTotal.AddRecFinan(importe);

	dplu.s_c_flag = 0;					// Indica que el codigo fue TECLEADO
	dplu.func = DPlu;					// ID de la funcion
	dplu.cod = atol(usacod);			// Codigo del articulo.
	dplu.envas = 0;
	dbplu.Get("des", usades);			// Descripcion.
	//PatchRegionalChars(usades);
	dbplu.Get("dep", msg);				// Departamento relacionado
	dplu.dnro = atoi(msg);
	dbplu.Get("subdep", msg);
	dplu.sdnro = atoi(msg);
	dplu.uni = 1;
	dplu.esBulto = false;
	dbplu.Get("iva", usacod); //dbplu.Get(EsRI ? "ivari" : "iva", usacod); 
	dplu.iva = atoi(usacod);
	dplu.pesado = 0;
	dplu.anulado = 0;
	dplu.otroP = 1;
	dplu.precuni = precuni;
	dplu.fecha = (long) date();							// Fecha, hora y cajero de la registracion
	dplu.hora = (long) time_();
	dplu.cajero = ckaNro;
	dplu.ppuntual = 0;
	sprintf(dplu.pesoenbalanza, "%.3lf", 0.0);
	dbplu.Get("premio", msg);
	dplu.umillas = atoi(msg);
	store(importe, dplu.xReg);							// Importe total a cobrar
	store(cantidad, dplu.yReg);							// Cantidad para la multiplicacion
	store((double)dplu.cod, tmsg);						// Codigo originalmente tipeado o scaneado
	if(strlen(tmsg) > 13) tmsg[13] = 0;
	STRCPY(dplu.origCode, tmsg);
	STRCPY(dscan, "7790001001047");
	ProcPlu(&dplu, 1);                                  // GRABA!!!
}

//   Prototipo forward.
int ChkSpecialScan(void);

//   La siguiente funcion procesa los pedidos de ventas de plus. A esta
//   funcion se llega desde el despachador de eventos de teclados o el
//   despachador de eventos de scanner.
void plu_(int);
void plu(int cual)
{
	GCStatic::LockMainLoop("plu");
	CheckNewPlu();
	plu_(cual);
	GCStatic::ReleaseLockMainLoop("plu");
}

void plu_(int cual)
{
	int found;
	double importe, ForcePrice, origCode;
	
	bool EsEscaneado = false;
	bool EstaRepitiendo = false;
	
	double tmuni = 0;
	sapimpiva = 0;
	agregaIvaRi = false;
	if (EnAutocierre)
		return;	
	
	// Muestra un log de como se llamo a la funcion.
	sprintf(msg, "PLU-S1 - cual=%d, XReg=%lf, YReg=%lf, consmode=%d, pRepeat=%d, dRepeat=%d",
		cual, xReg, yReg, consmode, pRepeat, dRepeat);
	WriteBootLog(msg);
	
	double decpart, intpart;
	decpart = modf(xReg, &intpart);
	
	if ((decpart * 10000) > 0)
	{
		Alert("Error en CODIGO/SCANNER Ingresado", "Verificar CODIGO/SCANNER");
		WriteBootLog("AVISO - El codigo/scanner ingresado tiene parte decimal");
		return;
	}
	// Preserva el codigo original
	origCode = xReg;

	// Valida codigos de barra extraños (que no este validando el digito verificador)
	char *tScanner = padl(xReg, 13);
	int lenScanner = strlen(tScanner);
	char origscanner[30];
	float origyReg = 0;
	origyReg = yReg;
	STRCPY(origscanner, usacod);
	if(sc_DvEnScanner && lenScanner > 10 && tScanner[0] == '2')
	{
		char xdv = tScanner[lenScanner - 1];
		tScanner[lenScanner - 1] = 0;
		char calcDv = dv(tScanner);
		if(xdv != calcDv)
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No encuentro al Articulo scaneado o tecleado", msg);
			return;
		}
		STRCPY(usacod, origscanner);
	}
	
	//Porcion de Codigo agregada para los scanners CHINOS que coinciden con los pesables
	//mas adelante continua la parte del codigo para estos scanneres con la variable cbEnLista
	bool cbEnLista = false;
	if (strlen(origscanner) == 13){
		dbscan.Setorder("mscansc");		 
		if(dbscan.Seek(origscanner))
		{
			WriteBootLog("SeekMS - Encontro el scanner posiblemente CHINO");
			cbEnLista = true;
		}
	}
	//   La funcion PLU se utiliza para registrar la entrada de codigos de barra. Como tal,
	//   hay codigos de barra que son de control. Si este es el caso, entonces ejecuta esas
	//   funciones de control.
	//   Si, al analizar la barra, se encuentra una barra con un codigo PESABLE (emitida por balanza),
	//   esta funcion puso: xReg=Codigo del Plu, yReg=Monto/Cantidad, sc_ModoBarra=1/2
	if(ChkSpecialScan()) 
	{ 
		WriteBootLog("PLU - Detectado scan especial"); 
		return; 
	}
	WriteBootLog("PLU - No detecto scanner especial");		

	//   AQUI ya estamos en el codigo propio de la funcion PLU!!!
	pSubtot = 0;		// Indica que se debe pulsar subtotal de nuevo.
	if(consmode)
	{
		ConsPlu();
		pRepeat = dRepeat = 0;
		return;
	}
	if(inOper > 1 || globalCobraCC)     // Valida la fase de la operacion.
	{
		Alert("No puede vender Articulos Ahora.", "Termine la Cobranza Primero");
		return;
	}

	//   Valida otras condiciones previas para la funcion PLU.
	if (selfCheckout)
		ckaNro = cajeroSelf;
	if(chkClk()) return;				// Hay cajero habilitado?
	if(chkItemCount()) return;			// Se supero el nro maximo de items permitidos.

	
	if (xReg != 99997 && !scanartcobradora)//
	{
		Alert("No se permite facturar articulos", "En este modelo de impresoras");
		return;
	}
	if (!GlobalTF && pf->solofact)
	{
		Alert("El impresor solo permite Factura/TiqueFactura", "Elija un cliente");
		return;
	}

	if (EsEcommerce && NumPedEcommerce <= 0)
	{
		frmIngNumPedido ^fnumpedido = gcnew frmIngNumPedido(false);
		fnumpedido->ShowDialog();
		NumPedEcommerce = fnumpedido->NumPedido == "" ? 0 : System::Convert::ToInt64(fnumpedido->NumPedido);
		delete fnumpedido;
	}

	if(rMode)							// Estamos en X1? (Regresa a REG).
	{
		double tmp = xReg, tmp2 = yReg;
		xReg = 0.0;
		supervNro = 1;
		superv(0);
		xReg = tmp;
		yReg = tmp2;
	}

	// Mas informacion
	sprintf(msg, "PLU-S2 - cual=%d, XReg=%lf, YReg=%lf, consmode=%d, pRepeat=%d, dRepeat=%d",
		cual, xReg, yReg, consmode, pRepeat, dRepeat);
	WriteBootLog(msg);

	//   Incializa variables de calculo.
	ForcePrice = 0;             // Para articulos al peso, el total a cobrar.

	//   Si se pulsa <PLU> sin haber ingresado antes un numero (o scaneado), entonces presupongo que lo que 
	//   se intenta hacer es REPETIR la ultima fichada de articulos.
	if(xReg == 0.0)
	 {
		//   pRepeat sera 1 si la ultima operacion registrada fue la de plu, sino, sera cero y la operacion
		//   de repeticion estara deshabilitada
		if(!pRepeat)
		{
			Alert("Digite el NRO de CODIGO del Articulo", "Antes de pulsar la tecla de <PLU>");
			return;
		}
		if (modoMayor)
		{
			char *ms2g = padl(dplu.cod, 6);
			dbplu.Setorder("mplucod");
			dbplu.Seek(ms2g);
			dbplu.Get("uni", ms2g);
			tmuni = atoi(ms2g);
		}

		if(basePrec != NULL) FREE(basePrec);
		basePrec = BDecimal::Parse(dplu.xReg); //  xReg de la ultima fichada  
		if(yReg != 0) 
		{
			Alert("No ingreso el codigo del articulo a vender", "");
			return;
		}
		yReg = 1;         //  De la ultima fichada 
		EstaRepitiendo = true;
		WriteBootLog("PLU - Repite anterior...");
	 }
	 else
	 {
		 if (TestDUN14(atof_(usacod)) == 14 && !permite_dun14)
		 {
			 Alert("No ingrese el codigo de barra del Bulto", "");
			 return;
		 }

		 if (cbEnLista)
		 {
			 STRCPY(usacod, origscanner);
			 xReg = atof(usacod);
			 yReg = origyReg;
			 sc_ModoBarra = 0;
		 }
		 // Busca el PLU correspondiente. Esto lo hace SIEMPRE, dado que en xReg es esperable que SIEMPRE se
		 // encuentre el codigo (o scanner) del articulo a buscar.
		 dbscan.Setorder("mscansc");		 
		 if(SeekInMscan(atof_(usacod)))
		 {
		 	dbscan.Get("cod", usacod);
			dbplu.Setorder("mplucod");
			found = dbplu.Seek(usacod);
			EsEscaneado = true;
			dplu.s_c_flag = 1;				// Indica que el codigo fue SCANEADO
			sprintf(msg, "PLU - Encontrado en SCAN: %s", usacod);
			WriteBootLog(msg);
		 }
		 else
		 {			
			dbplu.Setorder("mplucod");
			found = dbplu.Seek(padl(xReg, 6));
			dplu.s_c_flag = 0;				// Indica que el codigo fue TECLEADO
			sprintf(msg, "PLU - Encontrado en PLU -  usacod='%s', xReg='%lf'", usacod, xReg);
			WriteBootLog(msg);
		 }

		 //   En este punto la base 'plu.dbf' DEBE encontrarse posicionada al articulo que se desea vender. La variable 'found' 
		 //  es cero si el articulo NO esta definido.
		if(!found)
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No encuentro al Articulo scaneado o tecleado", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}

		if (found && !VtaxCodInterno && !EsEscaneado)
		{
			dbplu.Get("vta_codigo", msg);
			if (msg[0] == 'F')
			{
				sprintf(msg, "Ingrese codigo barra de [Codigo: %s]", usacod);
				Alert("No se permite la venta Ingresando Codigo", msg);
				WriteBootLog(msg);
				return;
			}
		}

		dbplu.Get("dep", msg);
		if (!EstaEnDep(msg))
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No encuentro al Articulo scaneado o tecleado", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}

		if (EstaEnDepExclu(msg))
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No se permite fichar el articulo", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}
		dbplu.Get("cod", usacod);
		if (atoi(usacod) == smcodmaterial)
		{
			sprintf(msg, "[Codigo: %d]", smcodmaterial);
			Alert("No se permite fichar el articulo", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}

		if (EstaEnArtExclu(usacod))
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No se permite fichar el articulo", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}

		dbplu.Get("marca", msg);

		//Rutina agregada para que saque los espacios finales para comparar articulos de descarga
		char* end = msg + strlen(msg);
		// find the last non-whitespace character
		while ((end != msg) && isspace( *(end-1))) {
			--end;
		}

		// at this point either (end == s) and s is either empty or all whitespace
		//      so it needs to be made empty, or
		//      end points just past the last non-whitespace character (it might point
		//      at the '\0' terminator, in which case there's no problem writing
		//      another there).    
		*end = '\0';

		//   Con el estacionamiento habilitado, no se permite facturar mientras la aplicacion de
		//   Parking no tenga hecho su cierre: 'autorizado' viene en true recien cuando el cierre
		//   ya esta realizado. Si no se puede consultar el endpoint se deja facturar igual (para
		//   no dejar la caja parada por una falla del servicio), pero queda registrado en el log.
		if (procesaParking && !validadoCierreEstacionamiento)
		{
			LibEntidades::Alberdi::Parking::ClienteParking^ cliParking = gcnew LibEntidades::Alberdi::Parking::ClienteParking();
			LibEntidades::Alberdi::Parking::AutorizacionCierre^ consuCierre = cliParking->ConsultaCierreRealizado();
			bool cierreRealizado = true;
			char msgConsu[100];

			msgConsu[0] = '\0';
			if (consuCierre == nullptr)
				WLog("PARKING - No se pudo consultar el cierre de Estacionamiento; se permite facturar");
			else if (!consuCierre->autorizado)
			{
				// Falta el cierre en la aplicacion de Parking: no se puede facturar.
				cierreRealizado = false;
				if (consuCierre->mensaje != nullptr)
					Strings::StringToChar(consuCierre->mensaje, msgConsu, 100);
				else
					strcpy(msgConsu, "Falta realizar el cierre en Estacionamiento");
				WLog("PARKING - Cierre de Estacionamiento sin realizar, no se factura. Msg='%s'", msgConsu);
			}
			else
				validadoCierreEstacionamiento = true;		// Ya verificado, no reconsultar en este ticket.

			delete consuCierre;

			if (!cierreRealizado)
			{
				Alert(msgConsu, "");
				return;
			}
		}

		if (!(janisEcom || procesaParking) && !stricmp("descarga", msg))
		{
			sprintf(msg, "[Codigo: %s]", usacod);
			Alert("No se permite fichar el articulo", msg);
			WriteBootLog("PLU - No encontrado");
			return;
		}

		//   Obtiene del archivo el flag de si es o no articulo de venta al peso.
		dbplu.Get("cod", usacod);
		dplu.cod = atol(usacod);
		
		dbplu.Get("al_peso", msg);
		dplu.pesado = (msg[0] == 'T')? 1 : 0;
		if ((janisEcom || (procesaParking && idParking > 0)) && dplu.pesado && !xsJanisEsPesable)
			//dplu.pesado = 0;
			yReg = yReg * xPrecEcom;
		
		// Determina si el articulo de venta al peso fue scaneado
		bool flag = (sc_ModoBarra != 0);

		//   Fija si se uso precio manual y, de ser asi, si esta permitido hacerlo
		//if( false && fabs(yReg) > 0.001 && dplu.pesado && !permitePesado && !flag)
		if(fabs(yReg) > 0.001 && dplu.pesado && !permitePesado && !flag)
		{
			Alert("No puede 'multiplicar' articulos de venta al peso.", "Esta opcion ha sido bloqueada.");
			return;
		}

		if(!flag && (((int)(yReg * 1000)) % 1000) != 0) flag = true;
		if(flag && !dplu.pesado)
		{
			Alert("El articulo es UNITARIO y NO se puede vender pesado", "o fraccionado");
			return;
		}
		if(dplu.pesado && (fabs(yReg) < 0.001))
		{
			sprintf(msg, "Scanee una barra o INGRESE %s", pluMult? "un monto": "la cantidad");
			Alert("El articulo es de venta al peso.", msg);
			return;
		}

		//   Continua completando la estructura dplu.
		dplu.func = DPlu;					// ID de la funcion
		dplu.cod = atol(usacod);			// Codigo del articulo.
		dbplu.Get("envase", usades);		// Codigo de envase relacionado
		dplu.envas = atol(usades);
		dbplu.Get("des", usades);			// Descripcion.
		//PatchRegionalChars(usades);
		dbplu.Get("dep", msg);				// Departamento relacionado
		dplu.dnro = atoi(msg);
		dbplu.Get("subdep", msg);
		dplu.sdnro = atoi(msg);

		dbplu.Get("cf3", msg);
		dplu.apliPerIva = atof(msg) == 1 ? 0 : 1;

		//Obtiene el numero de pedido/seq, este dato solo viene cuando es precio puntual enviado por sistema
		dbplu.Get("seq", msg);
		dplu.precpuntual = atol(msg);		
		if (NumPedEcommerce > 0)
			dplu.ppuntual = NumPedEcommerce;

		//  Agrega a la estructura dplu la info sobre codigos DUN14
		dplu.uni = 1;
		dplu.esBulto = false;
		int usaPrec = listap;
		if(dun14)
		{
			dbplu.Get("uni", msg);
			dplu.uni = atoi(msg);
			tmuni = atof(msg);
			dplu.esBulto = true;
			if(dplu.uni < 1) dplu.uni = 1;
			if(precioBulto > 0 && precioBulto <= maxMpluPrec)
				usaPrec = precioBulto;
			dplu.dun14 = dun14;
		}
		else
		{	dbplu.Get("uni", msg);
			tmuni = atof(msg);		
			if(modoMayor != 0 && modoMayor != usaPrec)
				usaPrec = PrecioMayor(dplu.cod);
		}

		//  Determina el precio unitario a utilizar.
		//  Los articulos de venta al peso SIEMPRE se facturan a precio1. Deberia reducir los errores
		//  de stock por venta de productos pesados en cajas mayoristas o de bar
		//
		//  IMPORTANTE: el precio se lee y parsea SIEMPRE en un buffer LOCAL (precBuf) y nunca en el
		//  buffer global 'msg'. El buffer global lo comparten otros contextos (logging, y el evento
		//  serial de la balanza 'spBalan_DataReceived' que corre en un hilo de fondo de .NET); si una
		//  trama de peso llega entre el Get del precio y el Parse, 'msg' se sobrescribe y basePrec
		//  termina valiendo $1.00 (item pesado tecleado -> precio unitario erroneo en el ticket).
		char *tPrec;
		char precBuf[128];
		precBuf[0] = '\0';
		if(usaPrec == 1)
			dbplu.Get("precio", precBuf);
		else if(usaPrec > maxMpluPrec)
		{
			// Precio manual
			dbplu.Get("precio", precBuf);
			if(basePrec != NULL) FREE(basePrec);
			basePrec = BDecimal::Parse(precBuf);
			if(!PidePrecio(usades, basePrec)) return;
			tPrec = basePrec->ToString(2);
			STRCPY(precBuf, tPrec);
		}
		else
		{
			char nomPrec[16];
			sprintf(nomPrec, "precio%d", usaPrec);
			dbplu.Get(nomPrec, precBuf);
		}

		if ((janisEcom || procesaParking) && xPrecEcom != 0)
			//sprintf(precBuf, "%.2lf", xPrecEcom);
			STRCPY(precBuf, xsPrecEcom);

		// Las balanzas etiquetan SIEMPRE a un mismo precio. Determina a continuacion el precio con el
		// que se etiqueto el articulo, para corregirlo si corresponde.
		if(basePrec != NULL) FREE(basePrec);
		if (dplu.pesado == 1 && CantDigDecBal == 1 && strlen(precBuf) > 0)
			precBuf[strlen(precBuf) - 1] = '\0';
		basePrec = BDecimal::Parse(precBuf);
		switch(balanPrec)
		{
			case 2:  dbplu.Get("precio2", precBuf);  break;
			case 3:  dbplu.Get("precio3", precBuf);  break;
			case 4:  dbplu.Get("precio4", precBuf);  break;
			case 5:  dbplu.Get("precio5", precBuf);  break;
			case 6:  dbplu.Get("precio6", precBuf);  break;
			default:
				dbplu.Get("precio",  precBuf);
				break;
		}

		if(precio1 != NULL)
			//FREE(precio1);
			delete precio1;
		if (dplu.pesado == 1 && CantDigDecBal == 1 && strlen(precBuf) > 0)
			precBuf[strlen(precBuf) - 1] = '\0';
		precio1 = BDecimal::Parse(precBuf);
		tPrec = basePrec->ToString(2);
		
		//ivatasa(p->iva-1)
		//dbplu.Get("addivari", usacod);		
		//agregaIvaRi = (usacod[0] == 'T')? 1 : 0;

		dbplu.Get("iva", usacod); //dbplu.Get(EsRI ? "ivari" : "iva", usacod); 
		dplu.iva = atoi(usacod);	

		//if (EsRI && agregaIvaRi)
		//{					
		//	//double auxdiva = (((double)ivatasa(dplu.iva - 1))/10000) + 1;
		//	double ivadec = basePrec->ToDouble() * ((((double)ivatasa(dplu.iva - 1))/10000) + 1);
		//	basePrec = new BDecimal(ivadec);
		//}

		if (modoMayor > 0 && dplu.pesado && mayPrecPesable){
			STRCPY(dplu.xReg, precio1->ToString(2));
			basePrec = precio1;					
		}
		else
			STRCPY(dplu.xReg, tPrec);

		//   Si el articulo fuera scaneado y el scanner trae el monto en la barra.
		if(sc_ModoBarra == 1 || (sc_ModoBarra == 0 && dplu.pesado && (pluMult == 0)))
		{
			bool ajustar = false;
			if(sc_ModoBarra == 1) 
			{ 
				yReg = yReg / 100; 
				ajustar = true; 
			}
			if(listap == balanPrec || balanPrec == 0) 
				ajustar = false;
			if(ajustar)
			{
				// cantidad
				double save_yReg = yReg;
				//yReg = ceil(yReg / precio1->ToDouble() * 1000) / 1000;
				//yReg = ceil(yReg / precio1->ToDouble() * 1000000) / 1000000;
				yReg = (floor(yReg / precio1->ToDouble() * 1000000) / 1000000);

				// monto
				//ForcePrice = yReg * basePrec->ToDouble(); original
				//ForcePrice = (modoMayor && mayPrecPesable) ? (yReg * precio1->ToDouble()) : (yReg * basePrec->ToDouble());
				ForcePrice = (modoMayor && mayPrecPesable) ? (yReg * precio1->ToDouble()) : (yReg * basePrec->ToDouble());
				double avalue = (int)(ForcePrice * 100 + .5);
				ForcePrice = (double)avalue / 100;
				if(permiteSubir && save_yReg < ForcePrice)
				{
					yReg = save_yReg;
					if(basePrec != NULL) FREE(basePrec);
					basePrec = new BDecimal(precio1->ToDouble());
					ajustar = false;
				}
			}
			if(!ajustar)
			{
				// El articulo fue mutiplicado, por lo que no ajusta el precio de venta.
				ForcePrice = yReg;			
				yReg = ceil(ForcePrice / basePrec->ToDouble() * 100000) / 100000;				
			}
			dplu.uCant = 1;
		}
		else
		{
			//  El siguiente codigo funciona igual si la cantidad se tecleo (usando multiplicacion) o se decodifico
			//  de la barra que le corresponde.

			//  Si la cantidad se obtuvo por decodificacion de una barra, corresponde dividirla por mil porque no
			//  tiene representados los decimales.
			if(sc_ModoBarra == 2) 
			{
				dbplu.Get("balanza", msg);
				if (msg[0] == 'T')				
					yReg /= 1000;
			}

			//  Si la digitacion del codigo (o scaneado) NO INCLUYE CANTIDAD, usa '1' como cantidad. Tambien, ajusta
			//  el valor de 'dplu.uCant' que indica si se tipeo o no cantidad.
			if(fabs(yReg) <= .001)
			{
				yReg = 1.0;
				dplu.uCant = 0;
			}
			else
				dplu.uCant = 1;
		}
		store(yReg, dplu.yReg);		

		// Determina la tasa de iva que se le aplica al articulo.
		dbplu.Get("iva", usacod); //dbplu.Get(EsRI ? "ivari" : "iva", usacod); 
		dplu.iva = atoi(usacod);
	}

	//Obtiene el monto del impuesto interno del MPLU
	dbplu.Get("impint", msg);
	sprintf(dplu.montoImpInterno, "%.2lf", (atof(msg) * atof(dplu.yReg)));

	//************************************************************************************************
	//**  Aqui, comienza el proceso REAL de impresion y venta del articulo que se esta procesando.  **
	//************************************************************************************************
	dplu.dun14 = dun14;
	if(dplu.uCant || dun14)								// Imprime la multiplicacion.
	{
		//  Corrige la cantidad, si estamos ante un dun14
		if(!dplu.uCant) yReg = 1.0;
		yReg = yReg * dplu.uni;
		dplu.uCant = 1;

		//  Prepara el mensaje de repeticion para la pantalla.
		if(ceil(yReg) == yReg)
			sprintf(usacod, "%6.0lf X", yReg);
		 else
			sprintf(usacod, "%6.3lf X", yReg);
		Acomoda(msg1, usacod, HRng(basePrec->ToDouble()));	// Mensaje para CORRECCIONES
		pRepeat = 0;							// Multiplicaciones NO repito
	}
	else
	{
		pRepeat = 1;
		STRCPY(msg1, "");						// Mensaje para CORRECCIONES
	}

	//   Calcula los distintos totales, chequea la validez de la operacion y la imprime.
	if(dplu.pesado && (ForcePrice > 0.001))
		importe = ForcePrice;
	else{
		//se agrega para el caso de que son menos digitos decimales
		if (dplu.pesado && CantDigDecBal == 1)
			importe = BDecimal(basePrec->ToDouble() * yReg).ApplyRound(1).ToDouble();
		else
			importe = BDecimal(basePrec->ToDouble() * yReg).ApplyRound(2).ToDouble();
	}

///////////////////////////////////////////////////////////////////// estaba lo de modoMayor
	
/////////////////////////////////////////////////////////////////////
	if(globalCCOnline && cc_HastaSaldo)
	{
		double saldo = atof_(c_saldo);
		double salmax = atof_(c_salmax);
		if(salmax - saldo < xTotal.VerPendiente().ToDouble() + importe)
		{
			Alert("Con el ultimo articulo SUPERA el monto de credito",
				  "disponible. Cierre el ticket por favor...");
			return;
		}
	}
	if(!ValidarRegistracion(yReg, basePrec->ToDouble()))
		return;

	if(yReg >= 0) 
	{
		CustDisp3(usades, msg1, HRng(importe));             // Visor Cliente
		DispCash(importe, 2);                               // Visor del Cajero
	}
	Acomoda(msg2, usades, HRng(importe));
	dbplu.Get("premio", msg);
	dplu.umillas = atoi(msg);

	//  Acumula la venta para las promociones que la usen.
	store(ViewMiles(padl((double) dplu.cod, 6), (dplu.pesado)? (double) 1: yReg), dplu.miles);

	// Termina de guardar cosas en dplu.
	store(importe, dplu.xReg);							// Importe total a cobrar
	sprintf(tmsg, "%.0lf", origCode);// store(origCode, tmsg);								// Codigo originalmente tipeado o scaneado
	if(strlen(tmsg) > 13) tmsg[13] = 0;
	if (!EstaRepitiendo)
		STRCPY(dplu.origCode, tmsg);

	dplu.otroP = listap;								// Lista de precios usada	
	if ((janisEcom || procesaParking))
		dplu.precuni = (int)(basePrec->ToDouble() * 100);
	else
		dplu.precuni = (int)((basePrec->ToDouble() + 1e-10) * 100);
	dplu.fecha = (long) date();							// Fecha, hora y cajero de la registracion
	dplu.hora = (long) time_();
	dplu.cajero = ckaNro;
	if (dplu.pesado)
	{
		//acumPeso += atof(PesadoEnBalanza);
		//para el caso de un material que pesa en balanza
		if (atof(PesadoEnBalanza) > 0.001)
			STRCPY(dplu.pesoenbalanza, PesadoEnBalanza);
		else ///para el caso de un material de venta por unidad
			sprintf(dplu.pesoenbalanza, "%.3lf", 1);
	}
	else
	{
		dbplu.Get("peso", msg);
		sprintf(dplu.pesoenbalanza, "%.3lf",  yReg * atof(msg));
		//acumPeso += yReg * atof(msg);
	}
	dplu.precmay[0] = 0;
	//CalcIvaSap(&dplu);
	ProcPlu(&dplu, 1);                                  // GRABA!!!

	dRepeat = 0;                                        // No repite deptos.
	yReg = 0.0;                                         // Ni multiplicaciones
	PrintTktTot();
	CheckPriceOverflow();								// Si debe volver a precio comum
	HClear();
}

/// AGREGADO
#pragma region Agregado

//	valida la condicion de IVA del mayor.ini con la que tiene el ticket actual
// para aplicar ver si corresponde aplicar descuentos.
int pruebaCondiva()
{
	int aux = 0;
	/*int condic = strcmp( mcondiva, "si");
	if (!condic)
		if (*c_condiva != 0)
			aux = 1;
		else
			aux = 0;
	else
		aux = 1;*/
	return aux;
}

//Valida si el articulo se encuentra en el depto filtro
//si no hay un listado definido devuelve 1 (true), no se esta filtrando
int EstaEnDep(char *dep){
	int depsearch = atoi(dep);
	int encontrado = 0;
	int todos = 0;	
	for(int i = 0; i < 50 ; i++){
		if (filtradepto[i] == depsearch)
			encontrado = 1;
		else if (filtradepto[i] == -1)
			todos++;
	}
	if (todos == 50)
		encontrado = 1;
	return encontrado;
}

//Valida si el articulo se encuentra en el depto filtro
//si no hay un listado definido devuelve 1 (true), no se esta filtrando
int EstaEnDepExclu(char *dep){
	int depsearch = atoi(dep);
	int encontrado = 0;	
	for(int i = 0; i < 50 ; i++){
		if (nofichadeptos[i] == depsearch)
		{
			encontrado = 1;
			break;
		}		
	}	
	return encontrado;
}


//Valida si el articulo se encuentra en el depto filtro
//si no hay un listado definido devuelve 1 (true), no se esta filtrando
int EstaEnArtExclu(char *art){
	int artsearch = atoi(art);
	int encontrado = 0;	
	for(int i = 0; i < 100 ; i++){
		if (nofichaarts[i] == artsearch)
		{
			encontrado = 1;
			break;
		}		
	}	
	return encontrado;
}


#pragma endregion Agregado

//   La siguiente funcion asienta el proceso del plu.
void ProcPlu(void *p_, int fwrite)
{
	double y, x, signo;
    struct DDplu_ *p;
	int antNroVend = 0;

	struct DVendMay_ vendMay;

    p = (struct DDplu_ *) p_;
    y = atof_(p->yReg);
	x = atof_(p->xReg);
	signo = (x < 0.0 || y < 0.0)? -1.0: 1.0;
	if (p->ppuntual > 0)
		NumPedEcommerce = p->ppuntual;
	CalcIvaSap(p);

	if (!p->anulado)
		acumPeso += atof(p->yReg) * atof(p->pesoenbalanza);

	if(fwrite) {
		WriteDump(p);
		if (nroVendMay > 0){
			numRecFunc4 = tDbf.Recno();
			vendMay.func = DVendMay;
			vendMay.cajero = ckaNro;
			vendMay.nro = nroVendMay;
			antNroVend = nroVendMay;
			ProcVendedorMayor(&vendMay, 1);
			nroVendMay = 0;
		}
	}
#ifdef FACTSQL
	if (numsecuencia > 0 && fwrite)
	{	
		TransSql::MDPlu^ unplu = gcnew TransSql::MDPlu();
		unplu->Func = p->func;
		unplu->Tienda = gcnew System::String(saptienda);
		unplu->Caja = caja;
		unplu->Cajero = p->cajero;
		unplu->Cod = p->cod;
		unplu->S_c_flag = p->s_c_flag;
		unplu->UCant = p->uCant;
		unplu->Iva = p->iva;
		unplu->Pesado = p->pesado;
		unplu->Dnro = p->dnro;
		unplu->Envas = p->envas;
		unplu->YReg = gcnew System::String(p->yReg);
		unplu->XReg = gcnew System::String(p->xReg);
		unplu->Miles = gcnew System::String(p->miles);
		unplu->OrigCode = gcnew System::String(p->origCode);
		unplu->OtroP = p->otroP;
		unplu->Anulado = p->anulado;
		unplu->Precuni = p->precuni;
		unplu->Uni = p->uni;
		unplu->EsBulto = p->esBulto;
		unplu->Fecha = p->fecha;
		unplu->Hora = p->hora;
		unplu->Dun14 = p->dun14;
		unplu->Sdnro = p->sdnro;
		unplu->Impiva = gcnew System::String(p->impiva);
		unplu->Precmay = gcnew System::String(p->precmay);
		unplu->Ppuntual = p->ppuntual;
		unplu->Umillas = p->umillas;
		unplu->Towrite = true;
		Dump::sqltrans->Add(unplu);
	}
#endif

	if(Inicializando || globalCorri || fwrite || globalRedraw)
	{
		xTotal.AddVenta(BDecimal(fabs(x) * signo));
		xCnt += signo;
		double miles = atof_(p->miles);
		CalcIva(x * signo, p->iva-1, signo);
		//agregado para los impuestos internos
		AcumImpuestosInternos += (atof(p->montoImpInterno) * signo);
	}

	if(fwrite || globalCorri) PrintPlu(p);
	if(globalCorri) GlobalRngCount--;
	else			GlobalRngCount++;
	inOper = 1;

	//control de montos
	if (x >= 0.01)
		ReprocTransMayor(p, false);

	//Agregado para centralizar
	if (!p->anulado){
		if (Dump::docActual == nullptr){
			Dump::docActual = gcnew LibEntidades::Alberdi::HeaderDoc();
			Dump::docActual->Caja = caja;
			Dump::docActual->Cajero = ckaNro;
			Dump::docActual->Tienda->Tienda = gcnew String(saptienda);
			Dump::docActual->Tienda->IdTienda = suc;
			Dump::docActual->CanalVenta = gcnew String(sapcnlvta);
			Dump::docActual->NroVendedor = antNroVend;		
		}
		Dump::docActual->FechaHora = System::DateTime::Now;

		LibEntidades::Alberdi::DetalleDoc ^detaItem = gcnew LibEntidades::Alberdi::DetalleDoc();
		detaItem->Codigo = p->cod;
		detaItem->Departamento = p->dnro;
		detaItem->Subdepartamento = p->sdnro;
		detaItem->Scan = gcnew String(p->origCode);
		detaItem->IVA = p->iva;
		detaItem->Importe = System::Convert::ToDecimal(gcnew String(p->xReg));
		detaItem->Cantidad = System::Convert::ToDecimal(gcnew String(p->yReg));
		detaItem->Precio = System::Convert::ToDecimal(p->precuni/100.0);
		detaItem->Milla = p->umillas;
		detaItem->Escaneado = p->s_c_flag == 1;
		detaItem->Hora = System::DateTime::Now;
		detaItem->NroReg = tDbf.Recno();
		Dump::docActual->Detalle->Add(detaItem);
	}
}

//   La siguiente funcion asienta el proceso del plu.
void ProcNcPlu(void *p_, int fwrite)
{
	double y, x, signo;
	struct DDNcItem_ *p;

	p = (struct DDNcItem_ *) p_;
	y = atof_(p->yReg);
	x = atof_(p->xReg);
	signo = (x < 0.0 || y < 0.0)? -1.0: 1.0;
	
	EsNotaCredito = true;
	CalcIvaSapNc(p);
	if(fwrite) 
		WriteDump(p);

	if(Inicializando || globalCorri || fwrite || globalRedraw)
	{
		xTotal.AddVenta(BDecimal(fabs(x) * signo));
		xCnt += signo;
		double miles = atof_(p->miles);
		CalcIva(x * signo, p->iva-1, signo);
		//agregado para los impuestos internos
		AcumImpuestosInternos += (atof(p->montoImpInterno) * signo);
	}

	if(fwrite || globalCorri) 
		PrintNcPlu(p);
	if(globalCorri) 
		GlobalRngCount--;
	else
		GlobalRngCount++;
	inOper = 1;

	//ReprocTransMayor(p, false);
	//Agregado para centralizar
	if (Dump::docActual == nullptr){
		Dump::docActual = gcnew LibEntidades::Alberdi::HeaderDoc();
		Dump::docActual->Caja = caja;
		Dump::docActual->Cajero = ckaNro;
		Dump::docActual->Tienda->Tienda = gcnew String(saptienda);
		Dump::docActual->Tienda->IdTienda = suc;
		Dump::docActual->CanalVenta = gcnew String(sapcnlvta);		
	}
	Dump::docActual->FechaHora = System::DateTime::Now;

	LibEntidades::Alberdi::DetalleDoc ^detaItem = gcnew LibEntidades::Alberdi::DetalleDoc();
	detaItem->Codigo = p->cod;
	detaItem->Departamento = p->dnro;
	detaItem->Subdepartamento = p->sdnro;
	detaItem->Scan = gcnew String(p->origCode);
	detaItem->IVA = p->iva;
	detaItem->Importe = System::Convert::ToDecimal(gcnew String(p->xReg));
	detaItem->Cantidad = System::Convert::ToDecimal(gcnew String(p->yReg));
	detaItem->Precio = System::Convert::ToDecimal(p->precuni/100.0);
	detaItem->Milla = p->umillas;
	detaItem->Escaneado = p->s_c_flag == 1;
	detaItem->Hora = System::DateTime::Now;
	detaItem->NroReg = tDbf.Recno();
	Dump::docActual->Detalle->Add(detaItem);
}

//   Genera la informacion para la refacturacion.
void MakePluFactu(void *p_, char *g_Codigo, char *g_Descrip, double *g_Cantidad,double *g_UnitarioN, double *g_UnitarioT)
{
    struct DDplu_ *p;

    p = (struct DDplu_ *) p_;
	dbplu.Seek(padl(p->cod, 6));
	dbplu.Get("des", g_Descrip);
	//PatchRegionalChars(g_Descrip);
	sprintf(g_Codigo, "%-6ld", p->cod);
	*g_Cantidad = atof_(p->yReg);
	*g_UnitarioT = atof_(p->xReg);
	*g_UnitarioN = *g_UnitarioT / ivadisc(p->iva);
}

//   Imprime un renglon de venta.
void PrintPlu(void *p_)
{
	double x, y, pUnitario;
    struct DDplu_ *p;
	char calific;

    p = (struct DDplu_ *) p_;
	imprecodigo = 0;
	if(dbplu.Seek(padl(p->cod, 6)))
		dbplu.Get("des", usades);
	else
		sprintf(usades, "ARTICULO %ld (no encontrado)", p->cod);	// articulo ausente en el mplu local
	if (p->precpuntual > 0 && p->cod == 99029)
		sprintf(usades, "%s", "Gaseosa x 3 Lts");

	Trim(usades);
#ifndef FACTSQL
	// Defensa: solo recorta el sufijo "x 1 Un"/"x 1 Kg" si la descripcion tiene al menos 6
	// caracteres. Sin este control, con una descripcion corta (p. ej. "Pan") o vacia (articulo
	// no encontrado en el mplu local, caso tipico al reprocesar un ticket pendiente con codigos
	// de otra caja), 'usades + strlen(usades) - 6' apunta ANTES del buffer y el _stricmp lee/escribe
	// fuera de rango -> violacion de acceso intermitente al reimprimir lineas func=4 (DPlu/NC).
	size_t lenDes = strlen(usades);
	if(lenDes >= 6)
	{
		char *pTemp = usades + lenDes - 6;
		if(_stricmp(pTemp, "x 1 Un") == 0 || _stricmp(pTemp, "x 1 Kg") == 0) *pTemp = 0;
	}
#endif
	Trim(usades);
	//PatchRegionalChars(usades);

	x = atof_(p->xReg);
	y = atof_(p->yReg);	

	if (p->pesado || atof_(p->pesoenbalanza) > 0)
		STRCPY(dscan, "");
	else if (p->s_c_flag == 1)
		sprintf(dscan, "%s", p->origCode);
	else if (p->cod != 99997)
		STRCPY(dscan, "");

	pUnitario = ((double)p->precuni) / 100;	

	if(atof(p->miles) >= 0.01)
	{
		if(M_VerRng == 1 && PremLine)	
		{
			sprintf(msg, PremLine, dplu.miles, dplu.miles, dplu.miles);
			DispDeta(msg, " ");
		}
		else
		{
			if (p->umillas == 1)
			{			
				memmove(usades+1, usades, strlen(usades));
				usades[0] = '#';
			}
		}
	}

	if(p->anulado)
	{
		calific = 'm';
		y = y * (-1.0);
	}
	else if(x < 0.0 || y < 0.0)
		calific = 'D';
	else
		calific = 'M';

	sprintf(msg1, "%ld", p->cod);
	DispDeta(usades, HRng(x));

	double bulto = 0;
	if(p->esBulto) bulto = p->uni;
	if((prndelay && !globalCorri && globalDelayPrint) || (!prndelay && !globalCorri))
		WriteTickTmp(msg1, usades, y, bulto, pUnitario, x, 1);

	if(globalDelayPrint) return;
	if(pluAdicText != NULL && strlen(pluAdicText) != 0) 
		pf->PrintText(pluAdicText);

	if (p->precpuntual > 0 && p->cod == 99029){
		sprintf(msg1, "%s", "Gaseosa x 3 Lts");
		usades[0] = 0;
	}else{
		// Corrige la descripcion para ganar espacio en el ticket, si se puede
		if(printReduce && (strlen(msg1) + 3 + strlen(usades) <= 34))
		{
			sprintf(tmsg, "[%s] %s", msg1, usades);
			STRCPY(msg1, tmsg);
			usades[0] = 0;
		}
		else
			sprintf(msg1, "[%ld]", p->cod);
	}

	// Corrige la cantidad solo en la impresion, para aquellos articulos que se resten del ticket
	// y que deban afectar al stock
	if(fabs(y) < 0.001) y = 1;
	/*if (janisEcom && x < 0)
	{
		x *= -1;
		pUnitario *= -1;
	}*/
	imprecodigo = p->cod;
	pf->PrintItem(
			msg1,									// Codigo 
			(double) y,								// Cantidad
			(double) x,								// Monto Total
			ivatasa(p->iva-1),						// Tasa de Iva
			calific,								// Calificador
			(int)(p->pesado? 1.0: y + 0.0001),		// Bultos
			0,										// Impuestos Internos
			usades,									// Descripcion adicional.
			pUnitario,								// Precio Unitario
			p->pesado? 1: 0	,						// Siempre imprime la cantidad
			(int)bulto								// Cantidad de unidades en el bulto cerrado
			);			
}


//   Imprime un renglon de venta.
void PrintNcPlu(void *p_)
{
	double x, y, pUnitario;
	struct DDNcItem_ *p;
	char calific;

	p = (struct DDNcItem_ *) p_;
	imprecodigo = 0;
	if(dbplu.Seek(padl(p->cod, 6)))
		dbplu.Get("des", usades);
	else
		sprintf(usades, "ARTICULO %ld (no encontrado)", p->cod);	// articulo ausente en el mplu local
	Trim(usades);
#ifndef FACTSQL
	// Defensa: solo recorta el sufijo "x 1 Un"/"x 1 Kg" si la descripcion tiene al menos 6
	// caracteres. Sin este control, con una descripcion corta (p. ej. "Pan") o vacia (articulo
	// no encontrado en el mplu local, caso tipico al reprocesar un ticket pendiente con codigos
	// de otra caja), 'usades + strlen(usades) - 6' apunta ANTES del buffer y el _stricmp lee/escribe
	// fuera de rango -> violacion de acceso intermitente al reimprimir lineas func=4 (DPlu/NC).
	size_t lenDes = strlen(usades);
	if(lenDes >= 6)
	{
		char *pTemp = usades + lenDes - 6;
		if(_stricmp(pTemp, "x 1 Un") == 0 || _stricmp(pTemp, "x 1 Kg") == 0) *pTemp = 0;
	}
#endif
	Trim(usades);
	//PatchRegionalChars(usades);

	x = atof_(p->xReg);
	y = atof_(p->yReg);	

	if (p->pesado || atof_(p->pesoenbalanza) > 0)
		STRCPY(dscan, "");
	else if (p->s_c_flag == 1)
		sprintf(dscan, "%s", p->origCode);
	else if (p->cod != 99997)
		STRCPY(dscan, "");

	pUnitario = ((double)p->precuni) / 100;		

	if(p->anulado)
	{
		calific = 'm';
		y = y * (-1.0);
	}
	else if(x < 0.0 || y < 0.0)
		calific = 'D';
	else
		calific = 'M';

	sprintf(msg1, "%ld", p->cod);
	DispDeta(usades, HRng(x));

	double bulto = 0;
	if(p->esBulto) bulto = p->uni;
	if((prndelay && !globalCorri && globalDelayPrint) || (!prndelay && !globalCorri))
		WriteTickTmp(msg1, usades, y, bulto, pUnitario, x, 1);

	if(globalDelayPrint) return;
	if(pluAdicText != NULL && strlen(pluAdicText) != 0) 
		pf->PrintText(pluAdicText);

	// Corrige la descripcion para ganar espacio en el ticket, si se puede
	if(printReduce && (strlen(msg1) + 3 + strlen(usades) <= 20))
	{
		sprintf(tmsg, "[%s] %s", msg1, usades);
		STRCPY(msg1, tmsg);
		usades[0] = 0;
	}
	else
		sprintf(msg1, "[%ld]", p->cod);

	// Corrige la cantidad solo en la impresion, para aquellos articulos que se resten del ticket
	// y que deban afectar al stock
	if(fabs(y) < 0.001) y = 1;
	imprecodigo = p->cod;
	pf->PrintItem(
		msg1,									// Codigo 
		(double) y,								// Cantidad
		(double) x,								// Monto Total
		ivatasa(p->iva-1),						// Tasa de Iva
		calific,								// Calificador
		(int)(p->pesado? 1.0: y + 0.0001),		// Bultos
		0,										// Impuestos Internos
		usades,									// Descripcion adicional.
		pUnitario,								// Precio Unitario
		p->pesado? 1: 0	,						// Siempre imprime la cantidad
		(int)bulto								// Cantidad de unidades en el bulto cerrado
		);			
}

//   Para la zeta extendida, retorna el monto a adicionar al departamento.
void ValuesPlus(void *plu_, int *_cual, char **_des, double *_cant, double *_monto)
{
    struct DDplu_ *plu;

    plu = (struct DDplu_ *) plu_;
	*_cual = plu->dnro;
	*_des = GetDeptoDes(*_cual);
	*_cant = atof_(plu->yReg);
	*_monto = atof_(plu->xReg);
}

//   A la inicializacion, la siguiente funcion abre las tablas con la informacion de los plus.
int LoadPlus()
{
	int msgFlag;

	msgFlag = 0;
	checkerr(dbplu.Use(PLU_TBL));			// Archivo de plus.

	WriteBootLog(PLU_IDX3);
	if(!dbplu.SetIndex(PLU_IDX3))
	{
		SendMsg("Espere un momento.", "Construyendo Indices");
		msgFlag = 1;
		checkerr(dbplu.IndexOn("upper(marca)+upper(des)", PLU_IDX3));
	}

	WriteBootLog(PLU_IDX2);
	if(!dbplu.SetIndex(PLU_IDX2))
	{
		if(!msgFlag)
			SendMsg("Espere un momento.", "Construyendo Indices");
		msgFlag = 1;
		checkerr(dbplu.IndexOn("upper(des)", PLU_IDX2));
	}

	WriteBootLog(PLU_IDX1);
	if(!dbplu.SetIndex(PLU_IDX1))
	{
		if(!msgFlag)
			SendMsg("Espere un momento.", "Construyendo Indices");
		msgFlag = 1;
		checkerr(dbplu.IndexOn("cod", PLU_IDX1));
	}

	checkerr(dbscan.Use(SCAN_TBL));			// Archivo de scanners.
	WriteBootLog(SCAN_IDX1);
	if(!dbscan.SetIndex(SCAN_IDX1))
	{
		if(!msgFlag)
			SendMsg("Espere un momento.", "Construyendo Indices");
		msgFlag = 1;
		checkerr(dbscan.IndexOn("cod", SCAN_IDX1));
	}

	WriteBootLog(SCAN_IDX2);
	if(!dbscan.SetIndex(SCAN_IDX2))
	{
		if(!msgFlag)
			SendMsg("Espere un momento.", "Construyendo Indices");
		msgFlag = 1;
		checkerr(dbscan.IndexOn("scan", SCAN_IDX2));
	}
	if (variacionpp)
	{
		applyvarpp = true;
		WriteCmos(APPLYVARPP);
	}
	for(maxMpluPrec=4; maxMpluPrec<99; maxMpluPrec++)
	{
		char nomPrec[16];
		sprintf(nomPrec, "precio%d", maxMpluPrec);
		dbplu.Get(nomPrec, msg);
		if(strlen(msg) == 0) { maxMpluPrec--; break; }
	}

	if(msgFlag)	ClearMsg();
    return 1;
}

//   Al final del programa o cuando se necesita, la siguiente funcion cierra los archivos de artículos.
void UnLoadPlus()
{
	dbplu.Close();
	dbscan.Close();
}

//   La siguiente funcion se utiliza para, en casos de problemas, forzar
//   a que el sistema efectue una reconstruccion de indices de las bases
//   de articulos.
void ForcePluReindex()
{
	UnLoadPlus();
	remove(PLU_IDX1); 	remove(PLU_IDX2);    remove(PLU_IDX3);
	remove(SCAN_IDX1);	remove(SCAN_IDX2);
	LoadPlus();
}

//   La siguiente funcion se utiliza para chequear si hay modificaciones
//   de precios que se deban asentar. Si las hay, las procesa.
void CheckNewPlu()
{
	int t1, t2, t3, t4, t5, t6, t7, ErrorInTx;

	if(inOper || !GlobalGetPrec) return;		// Solo cuando esté activo el flag y no hay ticket iniciado

	/*t1 = Existe(NEW_PLU);
	t2 = Existe(NEW_SCAN);
	t3 = Existe(NEW_PLUIDX1);
	t4 = Existe(NEW_PLUIDX2);
	t5 = Existe(NEW_PLUIDX3);
	t6 = Existe(NEW_SCANIDX1);
	t7 = Existe(NEW_SCANIDX2);*/

	t1 = ExisteClr(NEW_PLU);
	t2 = ExisteClr(NEW_SCAN);
	t3 = ExisteClr(NEW_PLUIDX1);
	t4 = ExisteClr(NEW_PLUIDX2);
	t5 = ExisteClr(NEW_PLUIDX3);
	t6 = ExisteClr(NEW_SCANIDX1);
	t7 = ExisteClr(NEW_SCANIDX2);

	ErrorInTx = 1;
	//if(t1 && t2)
	if(t1 && t2 && t3 && t4 && t5 && t6 && t7)
	{
		//   Verifica que las bases que vienen tengan una estructura coherente.
		t1 = chknew.Use(NEW_PLU);
		chknew.Close();
		t2 = chknew.Use(NEW_SCAN);
		chknew.Close();
		if(t1 && t2)
		{
			//   Marca como correctamente transmitido.
			ErrorInTx = 0;

			//   Cierra los archivos de articulos actualmente en uso, y los borra.
			UnLoadPlus();           // Cierra archivo de articulos.
			remove(PLU_TBL);
			remove(PLU_IDX1);
			remove(PLU_IDX2);
			remove(PLU_IDX3);
			remove(SCAN_TBL);
			remove(SCAN_IDX1);
			remove(SCAN_IDX2);

			//   Activa los nuevos archivos.
			rename(NEW_PLU, PLU_TBL);
			rename(NEW_SCAN, SCAN_TBL);
			if(t3) rename(NEW_PLUIDX1, PLU_IDX1);
			if(t4) rename(NEW_PLUIDX2, PLU_IDX2);
			if(t5) rename(NEW_PLUIDX3, PLU_IDX3);
			if(t6) rename(NEW_SCANIDX1, SCAN_IDX1);
			if(t7) rename(NEW_SCANIDX2, SCAN_IDX2);

			//   Los activa.
			LoadPlus();

#ifdef POSXP
			// Si la siguiente instruccion se ejecuta MIENTRAS esta presente alguna de las forms
			// de busqueda de articulos, seguro que se producira una corrupcion de la memoria. Ver
			// esto...
			// (FEDE-15/09/2009) - Ahora, CheckNewPlu() es invocada desde _plu() y no desde una
			// tarea en background - Deberia ser seguro invocar a 'ClearImages' en este momento...
			DbfBrowse::ClearImages();
#endif

		}
	}

	//  Si algo no vino bien, saca un mensaje en pantalla.
	if(ErrorInTx)
	{
		Alert("Encontre actualizaciones de Articulos pero los",
			  "archivos estaban en mal estado. Transmita de Nuevo!!!");
		
		
		remove(NEW_PLU);
		remove(NEW_SCAN);
		remove(NEW_PLUIDX1);
		remove(NEW_PLUIDX2);
		remove(NEW_PLUIDX3);
		remove(NEW_SCANIDX1);
		remove(NEW_SCANIDX2);
		
	}

	
	//   Indica que los precios ya están asentados.
	GlobalGetPrec = 0;
	WriteCmos(FLAGPREC);
}

//   Funcion de 'validacion' del codigo de plu encontrado.
void buscaplu(int como);
int buscaPlu(char *cual, void *_bag)
{
	int si;

	Padl(Trim(STRCPY(uCod, cual)), 6, '0');
	if(!strcmp(uCod, "000000"))
	{
		globalProgramPlu = 1;
		buscaplu(3);
		globalProgramPlu = 0;
		dbplu.Get("cod", uCod);
	}
	if(dbplu.Seek(uCod))
	{
		prograEdit->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el articulo '%s'.", cual);
	si = GetSiNo(msg, "Lo incorporo como articulo nuevo?");
	if(!si)
		return 0;
	dbplu.AppendBlank();
	dbplu.Replace("cod", uCod);
	prograEdit->ShowGets();
	return 1;
}

//   Abre una ventana y permite editar el archivo de articulos.
void PrograPlu()
{
	int hWin;

	hWin = wDefine(5, 18, 18, 62, BORDERSINGLE, SNGCURS, prograEsq, "Programacion de Articulos", "");
	prograEdit = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Codigo:");
	wLocate(2, 2);  wPstr("Nombre:");
	wLocate(3, 2);  wPstr("Prec.1:");
	wLocate(4, 2);  wPstr("Prec.2:");
	wLocate(5, 2);  wPstr("Prec.3:");
	wLocate(6, 2);  wPstr("Prec.4:");
	wLocate(7, 2);  wPstr("Depto :");
	wLocate(8, 2);  wPstr("Iva ..:");
	wLocate(9, 2);  wPstr("Pesado:");
	wLocate(10, 2); wPstr("Puntos:");
	wLocate(11, 2); wPstr("Envase:");
	wLocate(8, 13); wPstr("0=Exento, 1...=Gravado");
	wLocate(1, 18); wPstr("0=Busca");

	prograEdit->AddGet( 1, 10, TEXT_LEFT_ALIGNED,  6, 0, uCod, NULL, NULL, buscaPlu);
	prograEdit->AddGet( 2, 10, "des",     &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 3, 10, "precio",  &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 4, 10, "precio2", &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 5, 10, "precio3", &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 6, 10, "precio4", &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 7, 10, "dep",     &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 8, 10, "iva",     &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet( 9, 10, "al_peso", &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet(10, 10, "premio",  &dbplu, NULL, NULL, NULL);
	prograEdit->AddGet(11, 10, "envase",  &dbplu, NULL, NULL, NULL);

	prograEdit->AnimaGets(1);
	wRelease(hWin);
	delete prograEdit;

#ifdef POSXP
	DbfBrowse::ClearImages();
#endif

	HClear();
}	

//   Verifica si 'usacod' valida el patron que se le pasa como parametro.
int FitPattern(char *patt)
{
	char *u, v1[17], v2[17], v3[17];
	int res = 0;

	//   Si el patron es vacio, NO VALIDA.
	while(isspace(*patt)) patt++;
	if(!*patt) return 0;

	//   Inicializa los valores temporarios.
	STRCPY(v1, "                ");
	STRCPY(v2, "                ");
	STRCPY(v3, "                ");
	
	//   Scanea el patron, comparando.
	for(u = usacod; *u; u++, patt++)
	{
		//   Si finalizo el patron..
		if(!*patt || isspace(*patt) || (*patt == 'v')) 
		{
			res = 1;
			break;
		}

		//   Descarga, segun cual sea el digito especificado en el patron.
		switch(*patt)
		{
			case 'd':							// Dni/Codigo de cliente.
			case 'k':							// Codigo de cajero.
			case 'c': case 'C':					// Codigo de plu.
				memmove(v1, v1+1, 15);			// Hace un lugar.
				v1[15] = *u;					// Coloca el nuevo digito.
				break;

			case 'p': case 'P':					// Cantidad en plu
				sc_ModoBarra = 2;
				memmove(v2, v2+1, 15);
				v2[15] = *u;
				break;

			case 'f':							// Codigo de funcion
			case 'm': case 'M':					// Monto de un plu.
				sc_ModoBarra = 1;
				memmove(v2, v2+1, 15);
				v2[15] = *u;
				break;

			case 'w':							// Password encriptada
				memmove(v3, v3+1, 15);
				v3[15] = *u;
				break;

			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': case '0':
				if(*patt != *u) return 0;
				break;

			default:							// Patron invalido.
				return 0;
		}
	}

	if(*patt && !isspace(*patt) && !(*patt == 'v')) return 0;
	xReg = atof_(v1);
	yReg = atof_(v2);
	zReg = atof_(v3);
	autoDetected = true;
	return res;
}

//   Prototipo.
void cltonl(int);

//   Verifica si el scanner ingresado es un codigo de scanner especial. Si es asi, lo procesa
//   y retorna true indicando esta situacion.
int ChkSpecialScan()
{
	//   Mira si la entrada es codigo de barra o codigo comun.
	globalSpecialScan = 0;					// Por ahora, NADA.
	sc_ModoBarra = 0;						// No hay PLU decodificado todavia.
	int eanLen = sc_DvEnScanner? 13: 12;
	if(eanLen == 12)
	{
		memmove(usacod, usacod + 1, 12);
		usacod[12] = dv(usacod);
	}
	char vaux[100];
	strcpy(vaux, usacod);
	sprintf(msg, "PLU - ChkSpecialScan-usacod: '%s'", vaux);
	*usacod = 0;
	strcpy(usacod, vaux);
	WriteBootLog(msg);
	if(usacod[0] == '0') return 0;
	
	AssertSanity("Antes de 'FitPattern' en plu::ChkSpecialScan");

	//   Mira si se trata del patron asignado a los codigos pesados...
	if(FitPattern(sc_MskPesable))
	{
		sprintf(msg, "PLU - Detectado Pesable. usacod='%s', mascara='%s'", usacod, sc_MskPesable);
		WriteBootLog(msg);
		autoDetected = false;
		return 0;		// Retorna '0' porque no debe salir de la funcion 'plu', que es desde donde se la llamo...
	}

	//   Mira si se trata del patron asignado para codigos de cajero...
	if(FitPattern(sc_MskCajero))
	{
		sprintf(msg, "PLU - Detectado Cajero. usacod='%s', mascara='%s'", usacod, sc_MskCajero);
		WriteBootLog(msg);
		clerk(1);					// El codigo ya esta en el xreg
		autoDetected = false;
		return 1;					// Retorna indicando que ya lo proceso...
	}

	//   Posibles cambio de llave.
	if(FitPattern(sc_MskLlaveX1))
	{
		sprintf(msg, "PLU - Detectado X1. usacod='%s', mascara='%s'", usacod, sc_MskLlaveX1);
		WriteBootLog(msg);
		globalSpecialScan = 2;
		xReg = 9999;
		superv(1);
		if(supervNro > 0)
		{
			xReg = rMode? 0: 1;
			autoDetected = true;
			chgmode(1);
		}
		autoDetected = false;
		return 1;
	}
	if(FitPattern(sc_MskLlaveX2))
	{
		sprintf(msg, "PLU - Detectado X2. usacod='%s', mascara='%s'", usacod, sc_MskLlaveX2);
		WriteBootLog(msg);
		globalSpecialScan = 2;
		xReg = 9999;
		superv(1);
		if(supervNro > 0)
		{
			xReg = rMode? 0: 2;
			autoDetected = true;
			chgmode(1);
		}
		autoDetected = false;
		return 1;
	}
	if(FitPattern(sc_MskLlaveZ))
	{
		sprintf(msg, "PLU - Detectado Z. usacod='%s', mascara='%s'", usacod, sc_MskLlaveZ);
		WriteBootLog(msg);
		globalSpecialScan = 2;
		xReg = 9999;
		superv(1);
		if(supervNro > 0)
		{
			xReg = rMode? 0: 3;
			autoDetected = true;
			chgmode(1);
		}
		autoDetected = false;
		return 1;
	}

	//   Analiza el patron de tarjetas de cuentas corrientes.
	if(FitPattern(sc_MskCliente))
	{
		sprintf(msg, "PLU - Detectado Cliente. usacod='%s', mascara='%s'", usacod, sc_MskCliente);
		WriteBootLog(msg);
		cltonl(1);
		autoDetected = false;
		return 1;
	}

	//   Analiza el patron de tarjetas de supervisores.
	if(FitPattern(sc_MskSuperv))
	{
		int _func = (int)yReg;
		sprintf(msg, "PLU - Detectado SUPERVISOR. Codigo='%.0lf', Funcion='%d', Mascara='%s'", xReg, _func, sc_MskSuperv);
		WriteBootLog(msg);
		superv(1);
		xReg = (supervNro != 0)? _func: 0;
		autoDetected = true;
		chgmode(1);
		autoDetected = false;
		return 1;
	}

	//   Patrones previstos en scanner.ini pero todavia no implementados...
	if(FitPattern(sc_MskMozo))
	{
		Alert("El tipo de tarjeta scaneado todavia no esta soportado",
			  "en esta version del POS. Lo siento.");
		autoDetected = false;
		return 1;			// Indica que ya esta procesado.
	}
	return 0;
}

