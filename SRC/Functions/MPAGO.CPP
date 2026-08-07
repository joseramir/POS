/*
 *   MPago.c
 *   Programa la funcionalidad de medios de pago.
*/

#include <stdafx.h>
#include "funcs.h"          // Carga la estructura mpag_


// Compatibilidad
#ifdef __VSC__
#define stricmp _stricmp
#endif

#ifdef POSXP
#include "PagoForm.h"
#include "MpagCliente.h"
#include "Strings.h"
#include "GCStatic.h"

#include "TarjOnlineTouch.h"

using namespace pos;
using namespace Bitnet::TJOCommon;
using namespace Alberdi::TJOCommon::VerifoneCommon;
#else
#include "pos.h"
#endif

void CommonDescto(int codigo, double monto, double cant, bool simular);

bool clrCustomerDisplay = false;
static char *tempTot = new char[30];

//  El siguiente array de enteros puede llegar a contener la lista de codigos
//  de medio de cobro disponibles. Si el puntero es NULL, todos los medios de
//  cobros habilitados estaran disponible.s
int *availPagos = NULL;
bool forceRebuild = false;
static int tickVltoNro = 0;

//   Variables para manejo de la funcion de enviar la operacion a cobradora externa.
bool flagTope;
double topeLocal;

//   Codigo del medio de pago usado para arquear retiros
int retiCod = -1;
int cajFlota = 1;

//   Recargos calculados por medio de pago
BDecimal recaMedioPago;

//   Fuerza ignorar el tope local
bool globalIgnoreSend = false;

//   Variables y estructuras para manejar las promociones por medios de pago
int  dmp_Depto, dmp_Voucher;
::ArrayList *stackPagos = NULL;
struct RngPago
{
	char des[100];
	double monto;
	double descto;
};
void depto(int cual);
double totalAntesDcto;
double gextracc;

///
struct mpag_ *mpagos;
::ArrayList *mpconso, *mpreti;
struct DDmpag_ mpag;
 dbf    dbmpag;
dbf dbcta;
dbf dbgaveta;

dbf dbvuelto;

int GlobalCCSelected, chkMaxSal=1, ccte_Onln, ccte_Edit, globalCobraCC;
char *c_nombre = new char[200];
char *c_numero = new char[200];
char *c_dni = new char[50];
char *c_direcc = new char[200];
char *c_telef = new char[200];
char *c_cod = new char[50];
char *c_localidad = new char[200];
char *c_condiva = new char[50];
char *c_saldo = new char[70];
char *c_salmax = new char[70];
char *c_savecod = new char[50];
char *c_dnicompr = new char[50];
char *c_nomcompr = new char[200];
char *c_fecha = new char[50];
char *ccte_Path;
char *fmtCard = new char[300];

char *c_codpostal = new char[12];

char *c_saldopromo = new char[70];

///Agregados para Percepcion de Ingresos Brutos Jujuy
char *c_percep = new char[2];
char *c_fechapib = new char[8];
char *c_alicuota = new char[8];
char *c_leyenpercep = new char[40];

// Agregados pra Percepcion TISSH Salta
char *c_tisshpercep = new char[2];
char *c_tisshalic = new char[8];

//Agregado para implementar IVA por cliente
char *c_pyme = new char[2];

//Agregado para implementar percepcion IVA a partir de 01/04/2023
char *c_leyenIvapercep = new char[40];

//Agregado para mantener la descripcion de la reprticion desde SQL
char *c_repadescrip = new char[50];

int  c_lista;
 double c_Consumo, c_PorceMax, c_MontoCobro, tCobro;
int  ctaFull=0;

int  digicuit[] = { 5, 4, 3, 2, 7, 6, 5, 4, 3, 2 };
int  comprador, globalCCOnline, globalCheckPriceOverflow;
int  cc_UsaListas, cc_HastaSaldo, cc_SoloInicio, cc_ForzarMP, CC_NotUseUI = 0, cc_Medio, cc_PorceMax;
char *usaPath;

char *mp_cod = new char[4];
char *mp_des = new char[25];
char *mp_tipo = new char[2];
char *mp_linkcc = new char[7];
char *mp_max = new char[20];
char *mp_voucher = new char[8];
char *mp_arqueo = new char[200];
char *mp_descto = new char[20];
char *mp_grptot = new char[20];
char *mp_maxvlto = new char[20];
char *mp_vltocod = new char[10];
char *mp_limgav = new char[20];
char *mp_bolsin = new char[20];
char *mp_linkecr = new char[20];
char *mp_retiro = new char[5];
char *mp_extraible = new char[5];
char *mp_desctomp = new char[5];
char *mp_minextracc = new char[10];
char *mp_maxextracc = new char[10];
//char *mp_esvuelto = new char[5];
//char *mp_recargo = new char[10];

char *mp_mlimreca = new char[20];
char *mp_alirecargo= new char[20];

bool avisoGaveta;
EditWindow *totWin = NULL;
extern int xKey;
EditWindow *mpagWin = NULL;
static char *uCod = new char[5];
static EditItem *cod;
static EditItem *nom;
static EditItem *monto;
static GenForm *gfcc;

//   Esquema de colores para el browse del buscador.
#ifdef PANTACOLOR
short bcliesq[16] = {           // Esquema default de colores
    { VERDE * 16 + BLANCO },    // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
    { VERDE * 16 + BLANCO },    // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color del fondo cuando usa relleno.
	{ ROJO * 16 + BLANCO },		// Color para titulos resaltados.
};
#else
short bcliesq[16] = {            // Esquema default de colores
	{ NEGRO * 16 + BLANCO },    // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ NEGRO * 16 + BLANCO },    // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno.
	{ BLANCO * 16 + NEGRO },
};
#endif

class PagoExtendido *rootPagos = NULL;
::ArrayList MPParam = ::ArrayList(true, "MPParamStaticCtor");

// Trata de ubicar aunque mas no sea parte de 'dst' en 'src', ocupando no mas de 'lenDst' posiciones.
// - Si 'dst' no fuera una string vacia, no ocupa nada y retorna true.
// - Si no todos los caracteres de 'src' se pudieran ubicar en 'dst', ubica los que puede, los remueve
//   de 'src' y retorna true
// - Si todos los caracteres de 'src' se pudieron pasar a 'dst', retorna false. (no hace falta removerlos de 'src')
bool SplitParam(char *src, char *dst, int lenDst)
{
	if(dst[0] != 0) return true;
	int lenSrc = strlen(src);
	if(lenSrc <= lenDst)
	{
		STRCPY(dst, src);
		return false;
	}

	memcpy(dst, src, lenDst);
	memcpy(src, src + lenDst, (lenSrc - lenDst) + 1);
	return true;
}

bool MandarACobradora()
{
	// Si es operacion A/B, entonces pide una confirmacion simple
	if(GlobalTF)
		return (bool)GetSiNo("El ticket debe ser enviado a caja cobradora.", "Confirma?", false);

	// Indica que mandara a cobradora. Pide como referencia el nombre del cliente
	EditItem *ei = new EditItem("Cliente");
	ei->CambiarTipo(TIPO_CUALQUIERA);
	STRCPY(ei->dato, c_nombre);
	STRCPY(ei->help, "Nombre del Cliente, para identificar la operacion en la caja cobradora.");

	GenForm *gf = new GenForm("Envio a Caja Cobradora");
	gf->Items()->Add(ei);
	if(!gf->Animar()) { delete gf; return false; }

	STRCPY(c_nombre, ei->dato);
	delete gf;
	return true;
}

bool CompleteParams(MPParamCollection *mpData)
{
	MPParamValue *v, *v2, *v3;

	// Primero, mira que todos los parametros necesarios esten definidos en el medio de cobro.
	switch(mpData->Medio->tipo)
	{
		case 'T':		// Tarjeta
		case 'U':		// Tarjeta varia
			v = mpData->FindValue("Cupon");
			v2 = mpData->FindValue("DNI");
			v3 = mpData->FindValue("Cuotas");
			return !(v->IsEmpty() || v2->IsEmpty() || v3->IsEmpty());

		case 'K':		// Ticket
		case 'L':		// Ticket vario
			return true;

		case 'M':		// Mutuales
			v = mpData->FindValue("Nro de Orden");
			return !v->IsEmpty();

		case 'V':		// Varios
			v = mpData->FindValue("Referencia");
			return !v->IsEmpty();

		case 'H':		// Cheques
			v = mpData->FindValue("Fecha");
			v2 = mpData->FindValue("Firmante");
			return !(v->IsEmpty() || v2->IsEmpty());
				
		default:		// Otro no soporta parametros.
			return true;
	}
	return false;
}

// Examina la definicion del medio de pago y a la lista 'mpagParam' para ver si el 
// medio de pago necesita parametros y no se le dieron todavia. De ser asi, solicita los
// parametros que el medio de pago requiera.
// Vuelve con true si todo salio ok, o false si el usuario cancelo el ingreso de los parametros.
bool MpagoPedirParametros(int nro, double monto)
{
	int i;
	struct mpag_ *mp = BuscarMpag(nro);
	MPParamCollection *mpParam;
	bool found = false;

	if(mp == NULL) return true;
	if(!MpagoUsaParametros(nro)) return true;

	// Mira si ya esta creada la estructura de parametros
	for(i = 0; i < MPParam.Count(); i++)
	{
		mpParam = (MPParamCollection *)MPParam.Item(i);
		if(mpParam->Medio->nro == nro)
		{
			found = true;
			break;
		}
	}

	// Crea la estructura, si hacia falta.
	if(!found)
		mpParam = new MPParamCollection(nro);

	// Completa los parametros. Si no hay parametros necesarios, no hace nada.
	if(monto > 0.001) mpParam->Monto = monto;
	mpParam->Monto.ApplyRound(2);
	bool faltan = CompleteParams(mpParam);
	if(faltan && mpParam->Monto != 0) return true;
	
	// Falta algun dato... Completa los parametros.
	GenForm *gf = new GenForm(mp->des);
	EditItem *ei = new EditItem("Monto");
	ei->CambiarTipo(TIPO_MONEDA);

	char *tMonto = mpParam->Monto.ToString(2);
	STRCPY(ei->dato, tMonto);
	if(mpParam->Monto != 0)	ei->enabled = false;
	gf->Items()->Add(ei);

	for(int j=0; j < mpParam->Data->Count(); j++)
	{
		MPParamValue *v = (MPParamValue *)mpParam->Data->Item(j);
		v->Editor = new EditItem(v->Name);
		gf->Items()->Add(v->Editor);
	}

	// Anima la ventana.
	if(!gf->Animar()) return false;

	// Junta el valor obtenido de los parametros en una gran string
	fmtCard[0] = 0;
	int copyParam = 0;
	for(int j=0; j < mpParam->Data->Count(); j++)
	{
		MPParamValue *v = (MPParamValue *)mpParam->Data->Item(j);
		STRCPY(v->Value, Trim(v->Editor->dato));
		if(fmtCard[0] != 0) STRCAT(fmtCard, ",");
		STRCAT(fmtCard, v->Value);
		if(j == copyParam)
			SplitParam(v->Value, c_numero, 12);
	}
	
	return true;

}

// Retorna un valor que indica si el medio de pago usa o no parametros.
bool MpagoUsaParametros(int nro)
{
	return false;
	struct mpag_ *mp = BuscarMpag(nro);

	if(mp == NULL) return false;
	switch(mp->tipo)
	{
		case 'T':		//Tarjeta
		case 'U':		// Tarjeta varia
		case 'K':		// Ticket
		case 'L':		// Ticket vario
		case 'M':		// Mutuales
		case 'V':		// Varios
		case 'H':		// Cheques
			return true;
				
		default:		// Otro no soporta parametros.
			return false;
	}
	return false;
}

// Funcion usada recursivamente para llenar la lista solamente con los resultados relevantes.
void IncorporarResultado(PagoExtendido *container, ::ArrayList *result)
{
	if(container->subItems == 0)
	{
		if(container->monto > 0.0001)
			result->Add(container);
		return;
	}
	for(int i=0; i<container->subItems; i++)
		IncorporarResultado(container->items[i], result);
}

// Funcion que compara dos medios extendidos, para ordenarlos.
int OrdenaPagos(const void *_p1, const void *_p2)
{
	PagoExtendido *p1 = *((PagoExtendido **) _p1), *p2 = *((PagoExtendido **) _p2);
	int res = stricmp(p1->mp->grptot, p2->mp->grptot);
	if(res == 0) return p1->mp->nro - p2->mp->nro;
	return res;
}

// A medida que se ingresan datos en el total, va mostrando como queda la operacion.
double tickIngre;
void DisplayOne(int y, char *title, double monto)
{
	wEsqSetColor(5);
	wLocate(y, 5); wPstr("  ");
	wPstr(title);
	sprintf(msg, "%.2lf", monto);
	while(strlen(msg) < 14)
	{
		memmove(msg + 1, msg, strlen(msg) + 1);
		msg[0] = ' ';
	}
	wPstr(msg);
}

void DisplayTickTotal()
{
	double tkTot = xTotal.VerPendiente().ToDouble();
	DisplayOne(1, "TOTAL.: ", tkTot);
	DisplayOne(2, "INGRE.: ", tickIngre);
	DisplayOne(3, (char *)((tkTot <= tickIngre)? "VUELTO: ": "PEND..: "), fabs(tickIngre) - tkTot);
}

//  Rutina de validacion de los textboxes principales.
int NuevoMonto(char *dato, void *bag)
{
	tickIngre = 0;
	for(int i=0; i<rootPagos->subItems; i++)
	{
		PagoExtendido *pe = rootPagos->items[i];
		if(pe->getbuff == dato)
		{
			pe->monto = atof_(dato) * 100;
			if(abs(pe->monto) < .00001)
				STRCPY(dato, "");
			else
				sprintf(dato, "%13.2lf", pe->monto/100);
		}

		tickIngre += (pe->monto / 100);
	}

	DisplayTickTotal();
	return 1;
}

int CopiaMonto(char *dato, void *bag)
{
	double monto = atof(dato);
	if(monto == 0)
		STRCPY(dato, "");
	else
		sprintf(dato, "%13.2lf", monto);
	return 1;
}

int EditaPagoExtendido(PagoExtendido *pe, bool esRoot);

int SubVentana(char *dato, void *bag)
{
	tickIngre = 0;
	for(int i=0; i<rootPagos->subItems; i++)
	{
		PagoExtendido *pe = rootPagos->items[i];
		if(pe->getbuff == dato)
		{
			EditaPagoExtendido(pe, false);
			pe->monto = 0;
			for(int j=0; j<pe->subItems; j++)
			{
				PagoExtendido *pe2 = pe->items[j];
				pe2->monto = atof_(pe2->getbuff) * 100;
				pe->monto += pe2->monto;
			}
			if(abs(pe->monto) < 0.0001)
				STRCPY(pe->getbuff, "");
			else
				sprintf(pe->getbuff, "%13.2lf", pe->monto / 100);
		}
		tickIngre += (pe->monto / 100);
	}

	totWin->ShowGets();
	DisplayTickTotal();
	xKey = keydown;				// Evita que la tecla se propague a la ventana anterior.
	return 1;
}

int pxWin = -1;
int EditaPagoExtendido(PagoExtendido *pex, bool esRoot)
{
#ifdef POSXP
	int vbool = 0;
	try
	{	
		double tkTot = xTotal.VerPendiente().ToDouble();
		//double tkpiibb = xTotal.VerRecargo(10002).ToDouble();
		GCStatic::tjPosnet->Clear();
		Decimal totEnviado = Decimal (tkTot);		
		
		if (selfCheckout)
		{
			//Para Control de Peso igual dentro de 10 gramos de tolerancia
			/*if ((acumPeso - pesoBalanControl) > -0.01 && (acumPeso - pesoBalanControl) < 0.01 )
			{		*/	
				//PagoForm ^pf = gcnew PagoForm(pex, totEnviado, true, Decimal(tkpiibb));
				TarjOnlineTouch ^pf = gcnew TarjOnlineTouch(System::Convert::ToDouble(totEnviado));				
				pf->ShowDialog();
				vbool = (pf->DialogResult == System::Windows::Forms::DialogResult::OK);
				if (!vbool)
				{
					if (baplipercep)						
						xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
					if (baplitissh)
						xTotal.AddRecargo(10003, BDecimal::BDecimal(0));
					if (bApliPerIva)
						xTotal.AddRecargo(10004, BDecimal::BDecimal(0));
					xTotal.AddRecargo(10005, BDecimal::BDecimal(0));
					baplipercep = false;
					baplitissh = false;
					bApliPerIva = false;
					//Dump::ListaVueltos->Clear();
				}		
				delete pf;
			/*}
			else
				Alert("No coinciden los Pesos", "Colocar los articulos en Balanza");*/
		}else
		{
			//PagoForm ^pf = gcnew PagoForm(pex, totEnviado, true, Decimal(tkpiibb));
			PagoForm ^pf = gcnew PagoForm(pex, totEnviado, true);
			//Application::Run(pf);
			pf->ShowDialog();
			vbool = (pf->DialogResult == System::Windows::Forms::DialogResult::OK);
			if (!vbool)
			{
				if (baplipercep)						
					xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
				if (baplitissh)
					xTotal.AddRecargo(10003, BDecimal::BDecimal(0));
				if (bApliPerIva)
					xTotal.AddRecargo(10004, BDecimal::BDecimal(0));
				xTotal.AddRecargo(10005, BDecimal::BDecimal(0));
				baplipercep = false;
				baplitissh = false;
				bApliPerIva = false;
				//Dump::ListaVueltos->Clear();
			}		
			delete pf;
		}
	}
	 catch (System::Exception^ excep)
	 {
		 vbool = 0;
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion EditaPagoExtendido", System::DateTime::Now);
		 writer->WriteLine(fhdatos);		
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 /*ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");*/
		 throw "Error";
	 }
	 catch(...)
	 {
		 WriteBootLog("ERROR - No manejado en EditaPagoExtendido");		 
		 throw "Error";
	 }
	return vbool;
	//return (pf->DialogResult == System::Windows::Forms::DialogResult::OK);		


#endif
}

// Funcion que pide el total del ticket, usando un browse para la edicion.
//   datos=ArrayList que debe ser llenado con los objetos 'PagoExtendido' que definen que
//         es lo que ingreso el cajero y se va a registrar.
bool TotalExtendido(::ArrayList *datos)
{
	// Inicializa las definiciones, si corresponde.
	if(forceRebuild)
	{
		delete rootPagos;
		rootPagos = NULL;
		forceRebuild = false;
	}
	if(rootPagos == NULL)
	{
		rootPagos = new PagoExtendido(true);
		for(int i=0; i<cMpag; i++)
		{
			// Determina el medio de pago y si es valido para la transaccion actual.
			struct mpag_ *mp = &mpagos[i];
			bool found = true;
			if(retiCod == mp->nro)
				found = false;

			if(found && (availPagos != NULL))
			{
				found = false;
				for(int *pp = availPagos; *pp; pp++)
				{
					if(*pp == mp->nro)
					{
						found = true;
						break;
					}
				}
			}
			if(found)
			{
				found = false;
				for(int j=0; j<rootPagos->subItems; j++)
				{
					PagoExtendido *pe = rootPagos->items[j];
					if(!stricmp(pe->mp->grptot, mp->grptot))
					{
						// Si no tenia subitems, convierte un item ROOT en un item CHILD
						if(pe->subItems == 0)
							pe->Add(new PagoExtendido(pe->mp));

						PagoExtendido *xpe = new PagoExtendido(mp);
						pe->Add(xpe);
						found = true;
						break;
					}
				}
				if(!found)
				{
					PagoExtendido *npe = new PagoExtendido(mp);
					rootPagos->Add(npe);
				}
			}
		}

		// Ordena los medios ascendente por grupo
		qsort(rootPagos->items, rootPagos->subItems, sizeof(PagoExtendido *), OrdenaPagos);
	}

	// Limpia totales anteriores.
	for(int i=0; i<rootPagos->subItems; i++)
	{
		PagoExtendido *pe = rootPagos->items[i];
		STRCPY(pe->getbuff, "             ");
		pe->monto = 0;
		pe->variacion = 0;
		for(int j=0; j<pe->subItems; j++)
		{
			PagoExtendido *pex = pe->items[j];
			STRCPY(pex->getbuff, "             ");
			pex->monto = 0;
		}
	}

	// Efectua la edicion de los datos.
	int res = EditaPagoExtendido(rootPagos, true);

	// Pasa los resultados de una forma plana.
	datos->Clear();
	if(res)
	{
		IncorporarResultado(rootPagos, datos);
		WriteBootLog("AVISO - Termino de procesar los pagos con OK");
		/*bool (hayvisdebito = false)
		for(int i=res->Count()-1; i>=0; i--)
		{
			PagoExtendido *pe = (PagoExtendido *)res->Item(i);
			if(pe->monto > 0.005 && pe->mp != NULL)
			{
				double usePend = xTotal.VerPendienteDouble();
		if(GlobalTF)
		return (bool)GetSiNo("El ticket debe ser enviado a caja cobradora.", "Confirma?", false);*/

		return true;
	}
	WriteBootLog("AVISO - Termino de procesar los pagos con CANCEL");
	return false;
}


//   Lee los archivos de datos de cuenta corriente y los carga en las variables internas
void LeeCCDatos()
{
	char temp[200];
	System::String ^auxtmp = "";
	auxtmp = System::String::Format("{0}", Dump::actCliente->Cod);
	Strings::StringToChar(auxtmp, c_cod, 15);
	Strings::StringToChar(auxtmp, c_dni, 15);
	Strings::StringToChar(Dump::actCliente->Nombre, c_nombre, 50);
	Strings::StringToChar(Dump::actCliente->Cuit, c_numero, 50);
	auxtmp = System::String::Format("{0}", Dump::actCliente->Civa);
	Strings::StringToChar(auxtmp, c_condiva, 50);

	auxtmp = System::String::Format("{0}", Dump::actCliente->SalMax.ToString("F2"));
	Strings::StringToChar(auxtmp, c_salmax, 20);
	auxtmp = System::String::Format("{0}", Dump::actCliente->Saldo.ToString("F2"));
	Strings::StringToChar(auxtmp, c_saldo, 20);
	Strings::StringToChar(Dump::actCliente->Direccion, c_direcc, 50);
	Strings::StringToChar(Dump::actCliente->Telefono, c_telef, 50);
	Strings::StringToChar(Dump::actCliente->Localidad, c_localidad, 50);
	auxtmp = System::String::Format("{0}", Dump::actCliente->CodPostal);
	Strings::StringToChar(auxtmp, c_codpostal, 20);
	
	auxtmp = System::String::Format("{0}", Dump::actCliente->Lista);
	Strings::StringToChar(auxtmp, temp, 15);

	auxtmp = System::String::Format("{0}", Dump::actCliente->Percepcion);
	Strings::StringToChar(auxtmp, c_percep, 15);

	auxtmp = System::String::Format("{0}", Dump::actCliente->Fecha_pib.ToString("dd/MM/yyyy"));
	Strings::StringToChar(auxtmp, c_fechapib, 15);

	auxtmp = System::String::Format("{0}", Dump::actCliente->Alicuota.ToString("F2"));
	Strings::StringToChar(auxtmp, c_alicuota, 20);

	auxtmp = System::String::Format("{0}", Dump::actCliente->PerTissh);
	Strings::StringToChar(auxtmp, c_tisshpercep, 15);

	auxtmp = System::String::Format("{0}", Dump::actCliente->AliTissh.ToString("F2"));
	Strings::StringToChar(auxtmp, c_tisshalic, 20);

	auxtmp = System::String::Format("{0}", Dump::actCliente->SaldoCaja.ToString("F2"));
	Strings::StringToChar(auxtmp, c_saldopromo, 20);	

	Strings::StringToChar(Dump::actCliente->RepaDes, c_repadescrip, 50);

	c_lista = atoi(temp);
	if(c_lista < 0 || c_lista > 5)	c_lista = 0;
	c_nombre[30] = c_direcc[30] = c_localidad[30] = c_numero[13] = 0;

	
	STRCPY(c_dnicompr, c_dni); //dHost->Get("dni", c_dnicompr);
	STRCPY(c_nomcompr, c_nombre); //dHost->Get("rs", c_nomcompr);
	

	
	auxtmp = System::String::Format("{0}", Dump::actCliente->Pyme);
	Strings::StringToChar(auxtmp, c_pyme, 15);
	
	ClienteBenef = Dump::actCliente->Repa; //atoi(temp);

	//   Convierte la condicion ante el iva...
	STRCPY(c_savecod, c_cod);
	switch(atoi(c_condiva))
	{
		case 2:	 STRCPY(c_condiva, "RI"); 			
			EsRI = true; 			
			break;
		case 3:	 STRCPY(c_condiva, "NI"); break;
		case 4:	 STRCPY(c_condiva, "MO"); break;
			/*if (strlen(c_pyme) == 0)
				EsRI = false; 
			else
				EsRI = c_pyme[0] == 'T';
			break;*/
		case 5:	 STRCPY(c_condiva, "EX"); break;
		case 6:	 STRCPY(c_condiva, "NR"); break;
		default: STRCPY(c_condiva, "CF"); break;
	}	
	if (EsRI && strlen(c_pyme) > 0 && c_pyme[0] == 'S')
		EsRI = false;

	//   Extrae otros datos...
	if(ctaFull)
	{
		//dHost->Get("consumo", temp);
		c_Consumo = System::Convert::ToDouble(Dump::actCliente->Consumo); // atof_(temp);
		//dHost->Get("porcemax", temp);
		c_PorceMax = System::Convert::ToDouble(Dump::actCliente->Porcemax); // atof_(temp);
	}
	else
	{
		c_Consumo = 0;
		c_PorceMax = 100;
	}

	//   Pone todos los datos del cliente en variables de vouchers, para que puedan
	//   ser utilizados a discrecion.
	char tempBuff[500], *t;
	t = Trim(c_cod, tempBuff);       VoucherVar("@CC_COD@",     t);
	t = Trim(c_dni, tempBuff);       VoucherVar("@CC_DNI@",     t);
	t = Trim(c_nombre, tempBuff);    VoucherVar("@CC_NOM@",     t);
	t = Trim(c_numero, tempBuff);    VoucherVar("@CC_CUIT@",    t);
	t = Trim(c_condiva, tempBuff);   VoucherVar("@CC_COND@",    t);
	t = Trim(c_salmax, tempBuff);    VoucherVar("@CC_SALMAX@",  t);
	t = Trim(c_saldo, tempBuff);     VoucherVar("@CC_SALDO@",   t);
	t = Trim(c_direcc, tempBuff);    VoucherVar("@CC_DIREC@",   t);
	t = Trim(c_telef, tempBuff);     VoucherVar("@CC_TELEF@",   t);
	t = Trim(c_localidad, tempBuff); VoucherVar("@CC_LOCAL@",   t);
	t = Trim(c_dnicompr, tempBuff);  VoucherVar("@CC_DNICOMP@", t);
	t = Trim(c_nomcompr, tempBuff);  VoucherVar("@CC_NOMCOMP@", t);

	if (Dump::docActual == nullptr)
		Dump::docActual = gcnew LibEntidades::Alberdi::HeaderDoc();
	Dump::docActual->CodCli = System::Convert::ToString(Dump::actCliente->Cod);
	Dump::docActual->Nombre = Dump::actCliente->Nombre;
	Dump::docActual->Direccion = Dump::actCliente->Direccion;
	Dump::docActual->mReparticion->Descripcion = Dump::actCliente->RepaDes;
	Dump::docActual->mReparticion->ReparticionID = Dump::actCliente->Repa;
	Dump::docActual->FechaHora = System::DateTime::Now;

	Dump::docActual->Caja = caja;
	Dump::docActual->Cajero = ckaNro;
	Dump::docActual->Tienda->Tienda = gcnew String(saptienda);
	Dump::docActual->Tienda->IdTienda = suc;
	Dump::docActual->CanalVenta = gcnew String(sapcnlvta);
	
}

#ifdef POSXP
int vCCcod(char *, void *bag);		// Fordward

ref class MpagoStatic
{
public:

	static int ValidaCodCuenta(String ^cod)
	{
		char mycod[20];
		return vCCcod(Strings::StringToChar(cod, mycod, 12), NULL);
	}
};
#else
EditWindow *ccWin = NULL;
#endif

//   Valida el codigo de cuenta ingresado.
int vCCcod(char *p, void *bag)
{
	static char mycod[20], lastcod[20];	
	
	if(Empty(p)) 
		return 1;
	_strupr(Padl(Trim(STRCPY(mycod, p)), 12, ' '));
	Dump::actCliente = Controles::FuncClientes::BuscaClientePorCod(gcnew String(mycod));
	if (Dump::actCliente == nullptr){
		Alert("La cuenta indicada NO EXISTE", "Pulse ESC para seguir...");
		return 0;
	}

	LeeCCDatos();

	char *mmsg = new char[100];
	sprintf(mmsg, "Codigo de cliente seleccionado %s - reparticion: %d",  mycod, ClienteBenef);
	WriteBootLog(mmsg);
	delete mmsg;
	STRCPY(lastcod, c_cod);

	//if(!ccte_Edit && !CC_NotUseUI) RefreshWhenSay();
#ifndef POSXP
	if(!CC_NotUseUI) ccWin->ShowGets();
#endif
	return 1;
}



//   Valida la condicion del cliente.
int vCndIva(char *p, void *bag)
{
    _strupr(p);
    if(!strcmp(p, "RI") || !strcmp(p, "NI") || !strcmp(p, "NR") || !strcmp(p, "EX") || !strcmp(p, "CF") || !strcmp(p, "MO"))
       return 1;
    Alert("Ingreso MAL el codigo de condicion del cliente, p/ la DGI.", "Los codigos validos son RI, NI, NR, EX, CF o MO !!!");
    return 0;
}

//   Valida un CUIT.
int vCuit(char *p)
{
	int i, j, suma;
	char tcuit[20];

	//   Valida el largo del cuit.
	i = strlen(Trim(p)); 
	if(i < 11 || i > 13) return 0;
    STRCPY(tcuit, p);
	
	//   Remueve posibles guiones.
    for(p = tcuit; *p; p)
		if(*p == '-')
			STRCPY(p, p+1);
		else
			p++;
	if(strlen(tcuit) != 11) return 0;

	//   Calcula cada digito, controlandolo!!!
	for(i = suma = 0, p=tcuit; i < 10; p++, i++)
	{
		if(*p < '0' || *p > '9') return 0;
		j = *p - '0';
		suma += j * digicuit[i];
	}

	j = (11-(suma % 11)) % 11;
	if(j != (*p - '0')) return 0;
	return 1;
}

//   Muestra una pantalla indicando si el codigo de cuit es correcto.
int avCuit(char *p, void *bag)
{

	if(!_stricmp(c_condiva, "NR") || !_stricmp(c_condiva, "CF")) return 1;
	if(!vCuit(p))
	{
		Alert("El CUIT ingresado NO es valido!!!", "Ingreselo nuevamente...");
		return 0;
	}
	return 1;
}

//   Transforma la estructura del archivo de clientes a la nueva estructura.
void TransformDbf()
{
        char cBuff[200];

        if(Existe("pos\\clientes.dbf")) return;
        dbf *nw = new dbf, *old = new dbf;
        if(!old->Use("pos\\cctes.dbf")) return;
        nw->creaTable("pos\\clientes.dbf", "cod n(4), dni c(8), rs c(35), direcc c(40), localidad c(35), telefono c(11), civa n(1), cuit c(13), salmax n(16,2), saldo n(16,2), grupo n(4)");
        while(!old->Eof())
        {
                // Nuevo Registro
                nw->AppendBlank();

                // Reemplazos directos
                old->Get("ctacod", cBuff);    Trim(cBuff); nw->Replace("cod", cBuff);
                old->Get("nom", cBuff);       Trim(cBuff); nw->Replace("rs", cBuff);
                old->Get("direc", cBuff);     Trim(cBuff); nw->Replace("direcc", cBuff);
                old->Get("localidad", cBuff); Trim(cBuff); nw->Replace("localidad", cBuff);
                old->Get("telef", cBuff);     Trim(cBuff); nw->Replace("telefono", cBuff);
                old->Get("nrodoc", cBuff);    Trim(cBuff); nw->Replace("cuit", cBuff);
                old->Get("maxcred", cBuff);   Trim(cBuff); nw->Replace("salmax", cBuff);
                old->Get("saldo", cBuff);     Trim(cBuff); nw->Replace("saldo", cBuff);

                // Campos que se deben convertir
                old->Get("cndiva", cBuff); Trim(cBuff);
                if(!_stricmp(cBuff, "RI")) STRCPY(cBuff, "2");
                else if(!_stricmp(cBuff, "NI")) STRCPY(cBuff, "3");
                else if(!_stricmp(cBuff, "MO")) STRCPY(cBuff, "4");
                else if(!_stricmp(cBuff, "EX")) STRCPY(cBuff, "5");
                else STRCPY(cBuff, "1");
                nw->Replace("civa", cBuff);

                // Campos nuevos que se inicializan vacios
                cBuff[0] = 0; nw->Replace("dni", cBuff);
                STRCPY(cBuff, "0"); nw->Replace("grupo", cBuff);

                // Siguiente...
                old->Skip(1);
        }

        nw->Close(); delete nw;
        old->Close(); delete old;
        remove("pos\\ctacod.idx");
        remove("pos\\ctanom.idx");
        remove("pos\\ctadni.idx");
}

//   Abre los archivos de cuentas corrientes.
int OpenCCFiles()
{
	char tFileName[50];
    
	if(ccte_Onln)
	{
		usaPath = ccte_Path;
		if(usaPath[strlen(usaPath) - 1] != '\\')
			STRCAT(usaPath, "\\");
		view._exclusive = 0;
	}
	else
    {
		usaPath = "pos\\";
        TransformDbf();
    }

	//   Abre la DBF...	
	STRCAT(STRCPY(tFileName, usaPath), "clientes");
	if(!dbcta.Use(tFileName)) 
		if(!ccte_Onln)
			return 0;		
		else
		{
			Alert("El servidor no esta disponible.", "No puede hacer transacciones de cta/cte");
			WLog("Error al abrir: '%s'", tFileName);
			view._exclusive = 1;
			return 0;
		}
	
	//  Identifica si estamos con la estructura ampliada de base de datos o con la restringida.
	ctaFull = dbcta.FieldLen("dni2");	

	//   Abre (O crea, si no existen) los archivos de indice.
	STRCAT(STRCPY(tFileName, usaPath), "ctacod");
	if(!dbcta.SetIndex(tFileName))
		if(!dbcta.IndexOn("cod", tFileName))
		{
			view._exclusive = 1;
			dbcta.Close();
			return 0;
		}

	STRCAT(STRCPY(tFileName, usaPath), "ctanom");
	if(!dbcta.SetIndex(tFileName))
		if(!dbcta.IndexOn("upper(rs)", tFileName))
		{
			view._exclusive = 1;
			dbcta.Close();
			return 0;
		}

	STRCAT(STRCPY(tFileName, usaPath), "ctadni");
	if(!dbcta.SetIndex(tFileName))
		if(!dbcta.IndexOn("dni", tFileName))
		{
			view._exclusive = 1;
			dbcta.Close();
			return 0;
		}

	//   Los siguientes indices los abre (o crea) solamente si estamos con la estructura a FULL.
	if(ctaFull)
	{
		STRCAT(STRCPY(tFileName, usaPath), "ctadni2");
		if(!dbcta.SetIndex(tFileName))
			if(!dbcta.IndexOn("dni2", tFileName))
			{
				view._exclusive = 1;
				dbcta.Close();
				return 0;
			}
		
		STRCAT(STRCPY(tFileName, usaPath), "ctadni3");
		if(!dbcta.SetIndex(tFileName))
			if(!dbcta.IndexOn("dni3", tFileName))
			{
				view._exclusive = 1;
				dbcta.Close();
				return 0;
			}
	}
	
	//   Deja el flag de exclusive puesto.
	view._exclusive = 1;
	return 1;
}

//   Funcion que se utiliza para pedir en pantalla los datos de clientes en c/c.
int CompleteCC(struct mpag_ *p, double vtaTot)
{
	int res;
	double saldo_maximo, saldo_actual;
	char tFileName[50];

	if (!Controles::FuncClientes::AbrirClientesSql())
	{
		Alert("No se puede realizar Facturas", "Verficar conexion con servidor");
		return 0;
	}

	if(p != NULL && !Empty(p->linkcc))
		LeeCCDatos();

	//   Edita los datos obtenidos. Solamente si no estamos emitiendo ticket factura.
	if(consmode || (!GlobalTF && !globalCCOnline))
	{
#ifdef POSXP
		enSeleccionCliente = true;
		res = 0;
		MpagCliente ^cltForm = gcnew MpagCliente();
		cltForm->ValidateCod = gcnew MpagCliente::ValidateCodDelegate(&MpagoStatic::ValidaCodCuenta);
		cltForm->ShowDialog();
		res = (cltForm->DialogResult == System::Windows::Forms::DialogResult::OK);
		enSeleccionCliente = false;

#endif

		if(!res)
		{			
			EsRI = false;
			return 0;
		}		
	}

	// Si estamos en modo de consulta, salimos directamente.
	if(consmode)
	{		
		return 0;
	}

	//   Verifica que la cuenta este habilitada
	char tHabil[10]; tHabil[0] = 'F';
	/*if(ctaFull) dbcta.Get("bloqueado", tHabil);
	if(tHabil[0] == 'T')
	if (Dump::actCliente->)
	{
		Alert("Esta cuenta corriente esta bloqueada y", "no puede operar con ella...");		
		return 0;
	}*/

	//  Si corresponde, verifica que no se pase del limite permitido de consumo.
	if(chkMaxSal && fabs(vtaTot) > 0.01)
	{
		saldo_maximo = atof_(c_salmax);
		saldo_actual = atof_(c_saldo);
		if(saldo_actual + vtaTot > saldo_maximo)
		{
			sprintf(msg, "El maximo disponible es de $%.2lf.", 
					saldo_maximo - saldo_actual);
			Alert("Esta superando el saldo maximo permitido", msg);
			//dbcta.Close();
			return 0;
		}

		//  Deja algunos datos disponibles para los vouchers.
		VoucherVar("@CRED_SALMAX@", saldo_maximo);
		VoucherVar("@CRED_SALDO@", saldo_actual + vtaTot);
		VoucherVar("@CRED_DISPO@", saldo_maximo - (saldo_actual + vtaTot));
		VoucherVar("@CRED_DNICOMP@", c_dnicompr);
		VoucherVar("@CRED_NOMCOMP@", c_nomcompr);
		comprador = 1;

		//  Asienta en los archivos de cuenta corriente.
		//if(dbcta.Lock(1))
		//{
		//	//  Primero, actualiza el saldo del cliente.
		//	/*dbcta.Get("saldo", c_saldo);
		//    sprintf(tFileName, "%.2lf", vtaTot + atof_(c_saldo));
		//	dbcta.Replace("saldo", tFileName);		*/

		//	//rutina usada para registrar los datos en otra tabla CCMOVI.dbf en el servidor, no en todas las sucursales se realizaba
		//	////  Ahora, registra el movimiento, solamente si estamos online.
		//	//if(ctaFull && ccte_Onln)
		//	//{
		//	//	dbf *ccm = new dbf;
		//	//	STRCAT(STRCPY(tFileName, usaPath), "ccmovi");
		//	//	view._exclusive = 0;
		//	//	if(ccm->Use(tFileName))
		//	//	{
		//	//		//  Prepara la fecha
		//	//		char *fec = fecha();
		//	//		memmove(tFileName + 6, fec, 2); fec += 3;
		//	//		memmove(tFileName + 4, fec, 2); fec += 3;
		//	//		memmove(tFileName + 0, fec, 4);
		//	//		tFileName[8] = 0;

		//	//		//  Prepara el codigo de la cuenta.
		//	//		char tCod[10];
		//	//		STRCPY(tCod, c_savecod);
		//	//		Padl(Trim(tCod), 4, ' ');

		//	//		//  Asentar el movimiento.
		//	//		ccm->AppendBlank();
		//	//		ccm->Replace("tcom", GlobalComprob);
		//	//		ccm->Replace("fecha", tFileName);
		//	//		ccm->Replace("codcli", tCod);
		//	//		ccm->Replace("cuota", "1");
		//	//		ccm->Replace("fchvnc", tFileName);
		//	//		sprintf(tFileName, "%.2lf", vtaTot);
		//	//		ccm->Replace("monto", tFileName);
		//	//		ccm->Replace("aplicado", "0");

		//	//		sprintf(tFileName, "%d", ticket);
		//	//		ccm->Replace("comnro", tFileName);

		//	//		//  Cierra la base de movimientos
		//	//		ccm->Close();
		//	//	}

		//	//	view._exclusive = 1;
		//	//	dbcta.Lock(0);
		//	//	dbcta.Close();
		//	//}
		//}
		//else
		//{
		//	Alert("Esta CTA CTE esta siendo usada por otra pantalla",
		//	      "Por favor, pida que la desocupen para poder seguir...");
		//	//dbcta.Close();
		//	return 0;
		//}

		//dbcta.Close();
		return 1;
	}

	//   El manejo de clientes influye al submodulo de facturacion - ticket/factura.
	//   Deja las variables de ese modulo sincronizadas.
	if(FactuFlag <= 1) 
	{
		//chgFactuMode(c_condiva);
	}

	//dbcta.Close();
	GlobalCCSelected = 1;
	HClear();
	AdicClear();
	return 1;
}

//   Funcion de utilidad para cuando hay que facturar. 
//   FUERZA que un cliente este seleccionado.
int ForceCC(int canAbort)
{
	static struct mpag_ refe;
	int res;

	//   Selecciona un cliente.
	STRCPY(refe.des, "");
	STRCPY(refe.linkcc, "");
	refe.tipo = 'C';
	refe.nro = 9999;
	refe.maxsal = 0;
	while(1)
	{
		res = CompleteCC(&refe, 0);
		if(!res && canAbort) return 0;
		if(res) break;
	}

	 //  Graba los Resultados.
	mpag.func = DMpag;
	mpag.nro = 9999;
	mpag.uVuelto = 0;
	mpag.uPend = 0;
	mpag.cajero = ckaNro;
	store(0, mpag.importe);
	store(0, mpag.vuelto);
	
	//   Chequea que no se sobrepasen los maximos en los campos string.
	Trim(c_numero); Trim(c_fecha);  Trim(c_nombre); Trim(c_dni);
	Trim(c_telef);  Trim(c_direcc); Trim(c_condiva);
	if(strlen(c_cod) > 11)		    c_cod[11] = 0;
	if(strlen(c_numero) > 13)		c_numero[13] = 0;
	if(strlen(c_fecha) > 11)		c_fecha[11] = 0;
	if(strlen(c_nombre) > 30)		c_nombre[30] = 0;
	if(strlen(c_dni) > 15)			c_dni[15] = 0;
	if(strlen(c_telef) > 12)		c_telef[12] = 0;
	if(strlen(c_direcc) > 30)		c_direcc[30] = 0;
	if(strlen(c_condiva) > 2)       c_condiva[2] = 0;
	if(strlen(c_localidad) > 30)	c_localidad[30] = 0;

	//   Graba los datos en la estructura de medio de pago.
	STRCPY(mpag.telef, c_telef);
	STRCPY(mpag.direc, c_direcc);
	STRCPY(mpag.cc, c_cod);
	STRCPY(mpag.numero, c_numero);
	STRCPY(mpag.civa, c_condiva);
	STRCPY(mpag.fecha, c_fecha);
	STRCPY(mpag.nombre, c_nombre);
	STRCPY(mpag.local, c_localidad);
	STRCPY(mpag.dni, c_dni);
	mpag.alipercep = (int) (atof(c_alicuota) * 100);

	DCCOnln_ onln;
	onln.func = DCCOnln;
	onln.ctacte = _atoi64(c_cod);
	onln.flagPrec = 1;
	mpag.uOnline = 0;
	
	mpag.codpostal = atoi(c_codpostal);
	mpag.reparticion = ClienteBenef;

	ProcCCOnln(&onln, 1);

	//   Graba en el dump.
	ProcMpag(&mpag, 1);
	return 1;
}

//   La siguiente funcion se utiliza para registrar la venta a un determinado
//   medio de pago, cuya tecla se pulso.
//
int mpgcmp2(void const *k, void const *p)
{
    return *((int *) k) - ((struct mpag_ *) p)->nro;
}

// Envia el ticket a una cobradora externa.
bool EnviarACobradora()
{
#ifdef FACTSQL
	char tBuff[5], fBuff[5];
	bool addedpercep = false;
	if(!GlobalTF)
	{
		// Debemos indicar que se le facture a un consumidor final
		DDFactu1_ fact;
		fact.func = DFactu1;
		if(strlen(c_nombre) >= 30) c_nombre[29] = 0;
		STRCPY(fact.nombre, c_nombre);
		WriteDump(&fact);
	}
	try
	{
		remove("resu.tk");		
		//MoveTicket("resu.tk", -1);
		ReinitIvaPromo();		
		ApliPromoCobra();							
		bPromoCobra = true;		
		MoveTicket("resu.tk", -1);

		TransSql::Exporta::ToBdSql("resu.tk", gcnew String(saptienda), caja, ckaNro, date(), time_(), gcnew String(sapcnlvta));
		pasadoACobradora = true;

		// La operacion se envio satisfactoriamente al plugin. La elimina del trans.
		WriteTickTmp("", "Ticket Enviado a caja cobradora...", 0, 0, 0, 0, false);
		superv(0);
		TruncTrans();
		hasarEop = 1;
		WriteEOPFiscal(1);				
		elige_pasar = false;
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		bPromoCobra = false;
		ClearMsg();
		return true;		
	} 	
	catch (System::Exception^ ex)
	{
		ClearMsg();		
		bool aborrar = false;
		for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
		{
			tDbf.Get("func", fBuff);
			int func = atoi(fBuff);				
			if(!aborrar && (func == 8 || func == 28 || func == 26 || 
				func == 31 || func == 38 || func == 40))
				aborrar = true;
			if (aborrar)
				tDbf.Delete();				
		}
		if (aborrar)
		{
			tDbf.Pack();			
			Dump::cTicket->LimpiarNoAnulables();
		}
		//xTotal.ClearDescuento();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		bPromoCobra = false;
		Alert("Hay problemas para enviar el Ticket a Caja Cobradora...", "");
		return false;		
	}
#endif
#ifdef WFACT
	char tBuff[5], fBuff[5];	
	if(!GlobalTF)
	{
		// Debemos indicar que se le facture a un consumidor final
		DDFactu1_ fact;
		fact.func = DFactu1;
		if(strlen(c_nombre) >= 30) c_nombre[29] = 0;
		STRCPY(fact.nombre, c_nombre);
		WriteDump(&fact);
	}
	try
	{
		SendMsg("Enviando Ticket a Oficina de Cobranzas", "");
		fact::FactPluginClass ^plugin = GCStatic::GetFactPlugin();
		if(plugin == nullptr)
		{
			ClearMsg();
			Alert("Error de Configuracion en el sistema de Facturacion", "Avise a Sistemas");
			return false;
		}

		remove("resu.tk");		
		ReinitIvaPromo();		
		ApliPromoCobra();							
		bPromoCobra = true;		
		MoveTicket("resu.tk", -1);
		if(!plugin->SendToFact("resu.tk"))
		{			
			bool aborrar = false;
			for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
			{
				tDbf.Get("func", fBuff);
				int func = atoi(fBuff);				
				if(!aborrar && (func == 8 || func == 28 || func == 26))
					aborrar = true;
				if (aborrar)					
					tDbf.Delete();								
			}
			if (aborrar)
			{
				tDbf.Pack();			
				Dump::cTicket->LimpiarNoAnulables();
			}
			//xTotal.ClearDescuento();
			xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
			ClearMsg();			
			Alert("El ticket no pudo ser enviado a la caja cobradora.", "");
			bPromoCobra = false;
			return false;
		}	

		// La operacion se envio satisfactoriamente al plugin. La elimina del trans.
		WriteTickTmp("", "Ticket Enviado a caja cobradora...", 0, 0, 0, 0, false);
		superv(0);
		TruncTrans();
		hasarEop = 1;
		WriteEOPFiscal(1);		
		//xTotal.ClearDescuento();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		bPromoCobra = false;
		ClearMsg();
		return true;
	} 
	//catch(...)
	catch (System::Exception^ ex)
	{
		ClearMsg();
		bool aborrar = false;
		for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
		{
			tDbf.Get("func", fBuff);
			int func = atoi(fBuff);				
			if(!aborrar && (func == 8 || func == 28 || func == 26))
				aborrar = true;
			if (aborrar)					
				tDbf.Delete();								
		}
		if (aborrar)
		{
			tDbf.Pack();			
			Dump::cTicket->LimpiarNoAnulables();
		}
		//xTotal.ClearDescuento();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		bPromoCobra = false;
		Alert("Hay problemas para enviar el Ticket a Caja Cobradora...", "");
		return false;
	}
#else
	return false;
#endif
}


void CancelOperacion()
{
	/*double tickTot = xTotal.VerTotal().ToDouble();
	char tickMsg[40], tickAdic[40]; 
	STRCPY(tickAdic, "&4TKT CANCELADO");
	STRCPY(tickMsg, "SUBTOTAL");
	pf->PrintItem(tickMsg, 1, tickTot, 0, 'M', 1, 0, tickAdic, tickTot, false, 0);

	superv(0);*/
	TruncTrans();

	CustDisp("Ticket CANCELADO", "");
	WriteTickTmp("", "************ TICKET CANCELADO ************", 0, 0, 0, 0, 0);
	ticket = pf->GetTickNro(GlobalTF);
	//pf->PrintCancel();
	hasarEop = 1;
	WriteEOPFiscal(1);

	// Limpia todo.
	//ticket--;
	xTotal.Clear();
	xTotAnul = xReg = yReg = xCnt = 0;
	xCntAnul = 0;
	for(int i = 0; i < 4; i++)
		tNetos[i] = tIva[i] = tNpromo[i] = 0.0;
	HClear();
}


// Busca un medio de pago.
struct mpag_ *BuscarMpag(int cual)
{
	 return (mpag_ *) bsearch(&cual, mpagos, cMpag, sizeof(struct mpag_), mpgcmp2);
}

void mpago(int cual)
{
	mpago(cual, NULL);
}

void mpago_(int, TarjInfo *);
void mpago_2(int cual, CPosnet ^paramlapos);
void mpago_3(int cual, TransSmartPointMp ^paramSmart);

void mpago(int cual, TarjInfo *info)
{
	GCStatic::LockMainLoop("mpago");
	try
	{
		mpago_(cual, info);
	}
	catch (...)
	{		 
		WriteBootLog("ERROR - en funcion mpago, de tipo c++");
		ClearMsg();
		Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");
		POSExit();
	}
	GCStatic::ReleaseLockMainLoop("mpago");
}

void mandacob_(int cual)
{
	// Valida el estado
	 if(consmode) return;
	 if(inOper != 1) return;
	 if(CheckEnvases()) return;
	 if(corriMode == 1 && AnulacionesPendientes(NULL))
	 {
		 Alert("Hay ANULACIONES pendientes de Autorizar", "Llame al Supervisor");
		 return;
	 }
	 if (FactBconCuit && _atoi64(c_cod) < 999999)
	 {
		 Alert("Solo se permiten enviar Facturas", "A la cobradora");
		 return;
	 }

	 int res = MandarACobradora(); 
	 if(res == 0) return;
	 EnviarACobradora();
}

void mandacob(int cual)
{
	GCStatic::LockMainLoop("mandacob");
	mandacob_(cual);
	GCStatic::ReleaseLockMainLoop("mandacob");
}

void mpago_(int cual, TarjInfo *info)
{
	 struct mpag_ *p;
	 int uVuelto, uPend;
	 double importe, vuelto;

	 int iNroVuelto = 0;
	 try
	 {

	 //   La tecla no funciona en modo consulta
	 if(consmode) return;	 

	 if (rMode == 0 && cual == -1 && xTotal.VerVentaDouble() < 0.12)
	 {
		 Alert("Monto de venta no permitido","");
		 return;
	 }

	 //   Si estamos en modo global y la cuenta corriente es obligatoria, transforma el medio.
	 if(cc_ForzarMP && globalCCOnline) cual = cc_Medio;

	 //  Chequea envases pendientes.
	 if(inOper == 1)
	 {		 
		 // Validaciones antes de cerrar el ticket
		 if(CheckEnvases()) return;
		 if(corriMode == 1 && AnulacionesPendientes(NULL))
		 {
			 Alert("Hay ANULACIONES pendientes de Autorizar", "Llame al Supervisor");
			 return;
		 }
		 //
		 if (puede_pasar && xTotal.VerTotal().ToDouble() > limTicket)
			 elige_pasar = true;
		 if (puede_pasar && elige_pasar)		 
		 {
			 mandacob(0);
			 return;
		 }
		 
 
		//  Calcula las promociones que aplican al ticket.
		 if (!promosHechas){
			promosCalc = false;
			ReinitIvaPromo();
		 }
		if (cual == -1)
			Dump::promosmp->Clear();

		if (janisEcom)
			CalcularPromoJanisAntesMP();
		else
			CalcularPromosAntesMP();

		// Mira si el ticket va a ser cobrado externamente en caja cobradora.
		if(flagTope && !globalIgnoreSend && xTotal.VerPendiente() > topeLocal)
		{
			int res = MandarACobradora(); 
			if(res == 0) return;
			if(EnviarACobradora()) return; // Si hay error, sigue con la cobranza en modo local...
		}
	 }

	 //   Si el medio de pago es -1, entonces llama a la pantalla grafica de edicion de medios
	 //   y procesa el resultado.
	 if(cual == -1)
	 {		 
		 // Necesario, para sacar X o Zetas
		if(rMode > 1)
		{
			mkReport();
			return;
		}

		if(inOper < 1  || yReg != 0 || xReg != 0)
		{
			Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
			return;
		}

		// Fuerza un subtotal para sincronizar los impresores
		if(inOper == 1)	subtot(0);

		::ArrayList *res = new ::ArrayList(false, "mpago_");
		bool bandera = TotalExtendido(res);
		if(bandera)
		{
			iNroVuelto = 0;
			/////////////////////////////////////////
			for(int i=res->Count()-1; i>=0; i--)
			{
				PagoExtendido *tmpe = (PagoExtendido *)res->Item(i);
				double montodescto = Dump::promosmp->GetMontoDescSinConfirmarDouble(tmpe->mp->nro);
				if (tmpe->monto >= 0.01 && montodescto > 0)
				{
					Dump::promosmp->SetConfirmaDescto(tmpe->mp->nro, tmpe->monto);
				}
			}
			/////////////////////////////////////////

			double recaMPago = xTotal.VerRecargo(10005).ToDouble();
			if (recaMPago > 0)
			{
				FacturarMpagoRecargo(TIPORECA_RECUPERO, xTotal.VerRecargo(10005));
				xTotal.AddRecargo(10005, BDecimal::BDecimal(0));
			}

			//if(Dump::PinPadTarjTipre->Count == 0)
			{
				recaMedioPago = 0;
				for each(CPosnet ^posnet in GCStatic::tjPosnet)
				{
					if (posnet->m_recargo > 0)
						recaMedioPago += posnet->m_recargo;
					double montodescto = Dump::promosmp->GetMontoDescSinConfirmarDouble(posnet->m_mpago, posnet->m_codtarjeta);
					if (posnet->m_monto >= 0.01 && montodescto > 0)
						Dump::promosmp->SetConfirmaDescto(posnet->m_mpago, posnet->m_codtarjeta);
				}
				FacturarRecargo(TIPORECA_FINANCIERO, recaMedioPago);
				PrintTktTot(false);
				
				//agregado para las tarjetas Prisma
				recaMedioPago = 0;
				for each(TransVerifone ^posnet in Dump::TarjPrisma)
				{
					if (posnet->ImporteRecargo > 0)					
						recaMedioPago += Convert::ToDouble(posnet->ImporteRecargo);
					double montodescto = Dump::promosmp->GetMontoDescSinConfirmarDouble(posnet->AlbNroMpago, posnet->AlbNroTarjeta);
					if (posnet->ImporteIngresado > 0 && montodescto > 0)
						Dump::promosmp->SetConfirmaDescto(posnet->AlbNroMpago, posnet->AlbNroTarjeta);
				}
				FacturarRecargo(TIPORECA_FINANCIERO, recaMedioPago);
				PrintTktTot(false);

				recaMedioPago = 0;
				for each(TransSmartPointMp ^posnet in Dump::TarjSmartPointMp)
				{
					if (posnet->ImporteRecargo > 0)					
						recaMedioPago += Convert::ToDouble(posnet->ImporteRecargo);
					double montodescto = Dump::promosmp->GetMontoDescSinConfirmarDouble(posnet->AlbNroMpago, posnet->AlbNroTarjeta);
					if (posnet->ImporteIngresado > 0 && montodescto > 0)
						Dump::promosmp->SetConfirmaDescto(posnet->AlbNroMpago, posnet->AlbNroTarjeta);
				}
				FacturarRecargo(TIPORECA_FINANCIERO, recaMedioPago);
				PrintTktTot(false);
			}			

			
			// Primero, procesa las tarjetas online. Si falla, no procesa mas...
			//bool rtaAutor = AutorPinPad ? AutorizarTarjetasPinPad() : AutorizarTarjetasTipre();									
			for each(OrdenCashout ^pcashout in Dump::listCashout){
				if (pcashout->Cashout > 0)
				{
					extracash(1, System::Convert::ToDouble(pcashout->Cashout));
				}
			}
			
			for each(CPosnet ^posnet in GCStatic::tjPosnet)
			{				
				xReg = (posnet->m_monto + posnet->m_recargo) * 100;
				yReg = 0;
				cDecim = 0;
				pSubtot = true;
				for(int i=res->Count()-1; i>=0; i--)
				{
					PagoExtendido *tmpe = (PagoExtendido *)res->Item(i);
					if (tmpe->mp->nro == posnet->m_mpago)
					{
						tmpe->monto = 0;
						tmpe->variacion = 0;
						if (posnet->m_extraccion > 0)
						{
							extracash(tmpe->mp->nro, posnet->m_extraccion);
							//mpago_3(tmpe->mp->nro, posnet);
						}
					}
				}

				LibEntidades::Alberdi::PagoDoc ^itemPago = gcnew LibEntidades::Alberdi::PagoDoc();
				itemPago->MedioPago = posnet->m_mpago;
				itemPago->Monto = System::Convert::ToDecimal(posnet->m_monto);
				itemPago->CodTarjeta = posnet->m_codtarjeta;
				itemPago->Cuotas = posnet->m_cuotas;
				itemPago->Lote = posnet->m_lote;
				itemPago->Cupon = posnet->m_cupon;
				itemPago->Autorizacion = posnet->m_autorizacion;
				itemPago->Integracion = 0;
				itemPago->Terminal = System::Convert::ToInt32(posnet->m_terminal);				
				itemPago->QR = posnet->m_esqr;
				itemPago->Extracash = System::Convert::ToDecimal(posnet->m_extraccion);
				Dump::docActual->Pagos->Add(itemPago);

				mpago_2(posnet->m_mpago, posnet);

			}

			for each(TransVerifone ^pprisma in Dump::TarjPrisma){
				xReg = System::Convert::ToDouble(pprisma->Importe) * 100;
				yReg = 0;
				cDecim = 0;
				pSubtot = true;

				LibEntidades::Alberdi::PagoDoc ^itemPago = gcnew LibEntidades::Alberdi::PagoDoc();
				itemPago->MedioPago = pprisma->AlbNroMpago;
				itemPago->Monto = pprisma->Importe;
				itemPago->CodTarjeta = pprisma->AlbNroTarjeta;
				itemPago->Cuotas = pprisma->AlbNumCuota;
				itemPago->Lote = pprisma->NroLote;
				itemPago->Cupon = pprisma->NroCupon;
				itemPago->Autorizacion = pprisma->NroAutorizacion;
				itemPago->Integracion = 1;
				itemPago->Terminal = System::Convert::ToInt32(pprisma->NroTerminal);				
				itemPago->QR = pprisma->QrModo;
				itemPago->Extracash = pprisma->ImporteCashBack;
				Dump::docActual->Pagos->Add(itemPago);

				CPosnet ^posmpago = gcnew CPosnet();
				posmpago->m_monto = Convert::ToDouble(pprisma->Importe);
				posmpago->m_cuotas = pprisma->AlbNumCuota;
				posmpago->m_extraccion = Convert::ToDouble(pprisma->ImporteCashBack);
				posmpago->m_recargo = Convert::ToDouble(pprisma->ImporteRecargo);
				posmpago->m_mpago = pprisma->AlbNroMpago;
				posmpago->m_codtarjeta = pprisma->AlbNroTarjeta;
				posmpago->m_nrotarjeta = pprisma->GetFormaNroTarjeta();
				posmpago->m_autorizacion = pprisma->NroAutorizacion;
				posmpago->m_lote = pprisma->NroLote;
				posmpago->m_cupon = pprisma->NroCupon;
				posmpago->m_credito = pprisma->TarjCredito;
				posmpago->longaut = pprisma->longaut;
				posmpago->m_tipoTerminal = 2;
				posmpago->m_terminal = pprisma->NroTerminal;
				posmpago->m_esqr = (pprisma->QrModo ? 1: 0);

				if (posmpago->m_extraccion > 0)
					extracash(posmpago->m_mpago, posmpago->m_extraccion);
				
				mpago_2(posmpago->m_mpago, posmpago);
			}

			//borra las operaciones que tiene cargadas en memoria
			Dump::TarjPrisma->Clear();

			///////////////////////////////////////////////////////////////////
			for each(TransPrismaECR ^pprisma in Dump::TarjPrismaECR){
				xReg = System::Convert::ToDouble(pprisma->Importe) * 100;
				yReg = 0;
				cDecim = 0;
				pSubtot = true;

				LibEntidades::Alberdi::PagoDoc ^itemPago = gcnew LibEntidades::Alberdi::PagoDoc();
				itemPago->MedioPago = pprisma->AlbNroMpago;
				itemPago->Monto = pprisma->Importe;
				itemPago->CodTarjeta = pprisma->AlbNroTarjeta;
				itemPago->Cuotas = pprisma->AlbNumCuota;
				itemPago->Lote = pprisma->NroLote;
				itemPago->Cupon = pprisma->NroCupon;
				itemPago->Autorizacion = pprisma->NroAutorizacion;
				itemPago->Integracion = 1;
				itemPago->Terminal = System::Convert::ToInt32(pprisma->NroTerminal);				
				itemPago->QR = pprisma->QrModo;
				itemPago->Extracash = pprisma->ImporteCashBack;
				Dump::docActual->Pagos->Add(itemPago);

				CPosnet ^posmpago = gcnew CPosnet();
				posmpago->m_monto = Convert::ToDouble(pprisma->Importe);
				posmpago->m_cuotas = pprisma->AlbNumCuota;
				posmpago->m_extraccion = Convert::ToDouble(pprisma->ImporteCashBack);
				posmpago->m_recargo = Convert::ToDouble(pprisma->ImporteRecargo);
				posmpago->m_mpago = pprisma->AlbNroMpago;
				posmpago->m_codtarjeta = pprisma->AlbNroTarjeta;
				posmpago->m_nrotarjeta = pprisma->GetFormaNroTarjeta();
				posmpago->m_autorizacion = pprisma->NroAutorizacion;
				posmpago->m_lote = pprisma->NroLote;
				posmpago->m_cupon = pprisma->NroCupon;
				posmpago->m_credito = pprisma->TarjCredito;
				posmpago->longaut = pprisma->longaut;
				posmpago->m_tipoTerminal = 2;
				posmpago->m_terminal = pprisma->NroTerminal;
				posmpago->m_esqr = (pprisma->QrModo ? 1: 0);

				if (posmpago->m_extraccion > 0)
					extracash(posmpago->m_mpago, posmpago->m_extraccion);

				mpago_2(posmpago->m_mpago, posmpago);
			}

			//borra las operaciones que tiene cargadas en memoria
			Dump::TarjPrismaECR->Clear();
			///////////////////////////////////////////////////////////////////



			for each(TransSmartPointMp ^psmartpoint in Dump::TarjSmartPointMp){
				xReg = System::Convert::ToDouble(psmartpoint->Importe) * 100;
				yReg = 0;
				cDecim = 0;
				pSubtot = true;
				
				//mpago_3(psmartpoint->AlbNroMpago, psmartpoint);
				mpSeqIdOrden[0] = 0;
				//Antes estaba mpago_3(49, psmartpoint);
				mpago_3(32, psmartpoint);
			}

			//borra las operaciones que tiene cargadas en memoria
			if (Dump::TarjSmartPointMp->Count > 0){
				Dump::TarjSmartPointMp->Clear();
				//return;
			}

			// Ahora, procesa cada uno de los medios de cobro comunes
			globalIgnoreSend = true;
			int auxk = 1;
			char nomMedPag[50];
			char nomPagoMonto[50];
			for(int i=res->Count()-1; i>=0; i--)
			{
				PagoExtendido *pe = (PagoExtendido *)res->Item(i);
				if(pe->monto > 0.005 && pe->mp != NULL)
				{
					double usePend = xTotal.VerPendienteDouble();
					if(usePend <= 0.001) 
						break;
					mpSeqOp[0] = 0;		
					//Medio de pago SmartPoint se saltea en esta seccion, se trato anteriormente.
					if (pe->mp->nro == 49)
						continue;
					if (pe->mp->nro == 32)//Controla Mercado Pago
					{
						if (mpCashout)
						{
							for each(OrdenCashout ^pcashout in Dump::listCashout){
								xReg = System::Convert::ToDouble(pcashout->Monto) * 100;
								yReg = 0;
								cDecim = 0;
								pSubtot = true;

								if (pcashout->Cashout > 0)
									Strings::StringToChar(String::Format("{0} Ext: {1}",pcashout->Monto.ToString("F2"), pcashout->Cashout.ToString("F2")), nomMedPag, 40);
								else
									Strings::StringToChar(pcashout->Monto.ToString("F2"), nomMedPag, 40);
								VoucherVar("@MONTOMP@", nomMedPag);			
								WriteBootLog("convierte el monto de pago");
								Strings::StringToChar(("ID: " + pcashout->ExternalReference), nomMedPag, 40);
								VoucherVar("@IDOPERMP@", nomMedPag);
								WriteBootLog("convierte el id OP interno");
								Strings::StringToChar(System::Convert::ToString(pcashout->Reference), nomMedPag, 30);
								WriteBootLog(nomMedPag);
								VoucherVar("@IDOPERMPAPP@", nomMedPag);
								WriteBootLog("convierte el id Operacion de MercadoPago");
								//graba en variable para luego escribir en TRANS
								Strings::StringToChar(String::Format("{0},{1}", pcashout->ExternalReference, pcashout->Reference), mpSeqOp, 31);								
								StackVoucher(mpNumVoucherDev - 1);
								mpago_(32, nullptr);								
							}

							//borra las operaciones que tiene cargadas en memoria
							if (Dump::listCashout->Count > 0){
								Dump::listCashout->Clear();
								continue;
							}
						}
						else{
							Controles::PeticionCaja ^orden = gcnew Controles::PeticionCaja();						
							orden->numcaja = caja;
							orden->tienda = gcnew String(saptienda);
							orden->ptovta = atoi(pf->PuntoDeVenta());
							orden->tipocomp = "TI";
							orden->numcomp = pf->GetTickNro(GlobalTF);
							orden->monto = Convert::ToDecimal(pe->monto);
							orden->fhinicio = DateTime::Now;
							orden->fhactua = DateTime::Now;
							orden->idmp = "";
							orden->idoper = "";
							orden->estado = 0;
							orden->payment_id = 0;

							Controles::MsgAlert ^otromsg = gcnew Controles::MsgAlert();
							otromsg->Intentos = mpRepCons;
							otromsg->mipet = orden;
							otromsg->ShowMsgCrea("MERCADOPAGO...", "Creando Orden de pago");
																			
							if (orden->estado == 7)//
							{
								try
								{							
									Strings::StringToChar(orden->monto.ToString("F2"), nomMedPag, 40);
									VoucherVar("@MONTOMP@", nomMedPag);			
									WriteBootLog("convierte el monto de pago");
									Strings::StringToChar(("ID: " + orden->idoper), nomMedPag, 40);
									VoucherVar("@IDOPERMP@", nomMedPag);
									WriteBootLog("convierte el id OP interno");
									Strings::StringToChar(System::Convert::ToString(orden->payment_id), nomMedPag, 30);
									WriteBootLog(nomMedPag);
									VoucherVar("@IDOPERMPAPP@", nomMedPag);
									WriteBootLog("convierte el id Operacion de MercadoPago");
									//graba en variable para luego escribir en TRANS
									Strings::StringToChar(String::Format("{0},{1}", orden->idoper, orden->payment_id), mpSeqOp, 31);								
								}							
								catch (...)
								{
									WriteBootLog("Error al procesar MercadoPago");
								}
								StackVoucher(mpNumVoucherDev - 1);
								
							}
							else
							{
								if (orden->estado == 0)
									Alert("No se puede Crear Orden MercadoPago", "Servicio No Disponible");
								return;
							}
							otromsg = nullptr;
						}
					}
					sprintf(nomMedPag, "@MEDIO_PAGO%d@", auxk);
					sprintf(nomPagoMonto, "%s  $ %.2lf", pe->mp->des, pe->monto);
					VoucherVar(nomMedPag, nomPagoMonto);
					auxk++;
					if(tickVltoNro > 0 && usePend < pe->monto && pe->mp->maxvlto < 0)
					{
						double tickVlto = pe->monto - usePend;

#ifdef POSXP
					    xReg = -tickVlto * 100;
#else
					    xReg = -tickVlto;
#endif
						yReg = 0;
						cDecim = 0;
						mpago(tickVltoNro);

#ifdef POSXP
						xReg = pe->monto * 100;
#else
					    xReg = pe->monto;
#endif
						yReg = 0;
						cDecim = 0;						
						mpago(pe->mp->nro);
					}
					else
					{
#ifdef POSXP
						xReg = pe->monto * 100;
#else
						xReg = pe->monto;
#endif
						yReg = 0;
						cDecim = 0;
						pSubtot = true;
						gextracc = pe->extraccion;
						mpago(pe->mp->nro);
					}
					if(inOper < 2) break;
				}
			}
			globalIgnoreSend = false;
		}
		else		
			xTotal.ClearDescuento();
		
		delete res;
		return;
	 }

	 //   Apunta a los datos de definicion del medio y efectua algunas validaciones.+
	 p = BuscarMpag(cual);
	 if(inOper == 4 || inOper == 5)
	 {
		 if(p) cob_step2(p, xReg);
		 return;
	 }	 

	 if(!p || inOper < 1  || yReg != 0)
	 {
		Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
		return;
	 }
	 if(inOper == 1 && !pSubtot)
	 {
		 if(forcesubt)
		 {
			 Alert("Pulse SUBTOTAL antes de tratar de ingresar", "Algun medio de pago.");
			 return;
		 }
		 else
			 subtot(0);
	 }

	 //  Efectua los calculos.
	 if(xTotal.VerTotal() == 0 && xReg != 0.0) return;
	 if(inOper == 1) 
	 {
		 totalAntesDcto = xTotal.VerTotal().ToDouble();
		 char *tTotal = xTotal.VerTotal().ToString(2);
		 VoucherVar("@TOTAL_A_DCTO@", tTotal);
	 }	 

	 if (janisEcom && inOper == 1)	 
		 AplicaPromoJanis();		 
	 else
	 {
		 if (promosCalc && inOper == 1)
			//  Aplica las promociones.
 			AplicarPromociones();
	 }

	 //  Si no se indico TOTAL, lo procesa
	 if(xReg == 0.0)
		 importe = xTotal.VerPendiente().ToDouble();
     else
	 {
		 HDecim();
		 importe = xReg;
	 }

 	 //  Almacena el total del ticket
	 double tkTot = xTotal.VerPendiente().ToDouble();
	 if(inOper == 1)
	 {		 
		 tVtaReal = tkTot;
		 WriteCmos(TOTVTAREAL);
		 char *tTotal = xTotal.VerTotal().ToString(2);
		 VoucherVar("@TOTAL_TICKET@", xTotal.VerTotal().ToString(2));
		 PrnTotal();
		 STRCPY(tempTot, tTotal);
	 }

	 //  Determina si puede dar vuelto.
	 double uMaxVlto = p->maxvlto;
	 if(fabs(uMaxVlto) < 0.001) uMaxVlto = 0.01;
	 if(uMaxVlto >= 0 && importe > tkTot && (importe - tkTot) - uMaxVlto >= 0.01)
	 {
		 sprintf(msg, "Maximo vuelto para '%s' es %.2lf", p->des, p->maxvlto);
		 Alert(msg, "Pruebe de nuevo");
		 return;
	 }

	 // Algunas validaciones, si es cuenta corriente.
	 if(p->tipo == 'C')
	 {
		if(globalCobraCC)
		{
			Alert("No puede cancelar una C/C con otra C/C", "");
			return;
		}
		if(!CompleteCC(p, importe)) return;
	 }

	 // Pide los datos adionales, segun que medio de cobro se trate.
	 // c_nombre[0] = c_numero[0] = c_fecha[0] = c_dni[0] = 
     // c_direcc[0] = c_telef[0] = c_cod[0] = fmtCard[0] = 0;
	 if(!MpagoPedirParametros(p->nro, importe)) 
		 return;

	 // Todas las validaciones se efectuaron. Procesa...
	 uPend = uVuelto = 0;
	 vuelto = 0.0;
     if((importe - tkTot) >= 0)
	 {
		 clrCustomerDisplay = true;
         vuelto = importe - tkTot;
         uVuelto = (vuelto >= 0.01)? 1: 0;
	 }
     else
     {
         vuelto = tkTot - importe;
         uPend = (fabs(vuelto) >= 0.01)? 1: 0;
     }

	 if (cual == 34 && Dump::actCliente != nullptr)//mutual comodin
	 {
		 long mcticket = pf->GetTickNro(GlobalTF);
		 VoucherVar("@TICKET@", mcticket);	
		 VoucherVar("@IMPORTEMC@",importe);
			 //StackVoucher(27);
	 }

	 //  Imprime los resultados.
	 if(uVuelto)
	 {
		 CustDisp4("Total", tempTot, "Vuelto", HRng(vuelto));
		 DispCash(vuelto, opcdec);
	 }
	 else
	 {
		if(uPend)
		{
			CustDisp("Pendiente", HRng(vuelto));
			DispCash(vuelto, opcdec);
		}
		else
		{
			DispCash(0.0, opcdec);
		}
	 }

	 //  Copia datos de targetas online, si corresponden.
	 bool esOnline = false;
	 char *cl_nombre = new char[200];
	char *cl_dni = new char[50];
	char *cl_direcc = new char[200];
	char *cl_telef = new char[200];
	char *cl_localidad = new char[200];
	char *cl_condiva = new char[50];
	char *cl_fecha = new char[50];
	 ///c_fecha[0] = c_nombre[0] = c_dni[0] = c_telef[0] = c_direcc[0] = c_localidad[0] = 0;
	 fmtCard[0] = 0;
	 mpag.civa[0] = 0;

	 if(info != NULL)
	 {
		 char *alote = new char[6];
		 char *vlote = new char[6];
		 itoa(info->Lote, alote, 10);
		 if (strlen(alote) > 3)
		 {
			 int indlote = strlen(alote) - 3;
			 for(int km = 0; km < 3; km++)
				 vlote[km] = alote[indlote++];
			 vlote[3] = '\0';
			 //strncpy(vlote, alote, 3);
		 }
		 else
			 STRCPY(vlote, alote);		 
		 String ^cadticket = Convert::ToString(info->ticket);
		 info->ticket = cadticket->Length > 4 ? Convert::ToInt32(cadticket->Substring(cadticket->Length - 4)) : info->ticket;
		 String ^cadaut = Convert::ToString(info->Cupon);
		 info->Cupon = cadaut->Length > info->Definicion->longaut ? Convert::ToUInt32(cadaut->Substring(cadaut->Length - info->Definicion->longaut)) : info->Cupon;
		 int ivlote = atoi(vlote);
		 char *frmtaut = new char[10];
		 char *auxaut = new char[10];
		 sprintf(frmtaut, "%%0%dd", info->Definicion->longaut);
		 sprintf(auxaut, frmtaut, info->Cupon);
		 //sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%06d,%04d,%08d",
		 sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%s,%04d,%08d",
			 info->TarjetaEnmascarada2(true),
			 info->Vencim,
			 info->UsoLector? "S": "N",
			 info->Cuotas, 
			 0.0, //info->TasaRecargo,
			 0.0, //info->Recargo,
			 ivlote,//alote, //IntToStringFormat(info->Lote, 3),			 
			 auxaut,//info->Cupon,
			 (info->ticket >= 0 ? info->ticket : (-1) * info->ticket),//acupon, //IntToStringFormat(0, 4),			 
			 info->Dni);		 
		 esOnline = true;
		 char *ntarj = new char[7];
		 for(int k = 0, ik = 5; ik >= 0; k++, ik--)
			 ntarj[k] = info->Tarjeta[ik];		 
		 ntarj[6] = 0;
		 sprintf(mpag.dtarjeta, "%02d%s", info->Definicion->Codigo, ntarj);		 		 
		 delete [] alote; delete [] vlote; delete [] ntarj;
		 delete [] frmtaut; delete [] auxaut;
		 delete cadticket;
	 }

	 //  Prepara los datos adicionales a grabar, si existen
	 if(fmtCard[0] != 0)
	 {
		 if(SplitParam(fmtCard, cl_fecha, 11))
			 if(SplitParam(fmtCard, cl_nombre, 30))
				 if(SplitParam(fmtCard, cl_dni, 15))
					 if(SplitParam(fmtCard, cl_telef, 12))
						 if(SplitParam(fmtCard, cl_direcc, 30))
							 SplitParam(fmtCard, cl_localidad, 30);
	 }

	 //  Graba los Resultados.
	 mpag.func = DMpag;
	 mpag.nro = p->nro;
	 mpag.uVuelto = uVuelto;
	 mpag.uPend = uPend;
	 mpag._fecha = (long) date();
	 mpag._hora = (long) time_();
	 mpag.cajero = ckaNro;
	 store(importe, mpag.importe);
	 store(vuelto, mpag.vuelto);
	 Trim(c_numero); Trim(c_fecha); Trim(c_nombre); Trim(c_dni);
	 Trim(c_telef);  Trim(c_direcc);
	 if(strlen(c_numero) > 13) c_numero[13] = 0;
	 if(strlen(cl_fecha) > 11) cl_fecha[11] = 0;
	 if(strlen(cl_nombre) > 30) cl_nombre[30] = 0;
	 if(strlen(cl_dni) > 15) cl_dni[15] = 0;
	 if(strlen(cl_telef) > 12) cl_telef[12] = 0;
	 if(strlen(cl_direcc) > 30) cl_direcc[30] = 0;
	 if(strlen(cl_localidad) > 30) cl_localidad[30] = 0;
	 if(strlen(fmtCard) > 99) fmtCard[99] = 0;

	 STRCPY(mpag.telef, cl_telef);
	 STRCPY(mpag.direc, cl_direcc);
	 STRCPY(mpag.local, cl_localidad);
	 STRCPY(mpag.cc, c_cod);
	 STRCPY(mpag.numero, c_numero);
	 STRCPY(mpag.fecha, cl_fecha);
	 STRCPY(mpag.nombre, cl_nombre);
	 STRCPY(mpag.dni, cl_dni);

	 LibEntidades::Alberdi::PagoDoc ^itemPago = gcnew LibEntidades::Alberdi::PagoDoc();
	 itemPago->MedioPago = mpag.nro;
	 itemPago->Monto = System::Convert::ToDecimal(gcnew String(mpag.importe));
	 itemPago->Vuelto = System::Convert::ToDecimal(gcnew String(mpag.vuelto));
	 Dump::docActual->Pagos->Add(itemPago);

	 if (strlen(mpSeqOp) > 0)
		STRCPY(mpag.local, mpSeqOp);
	 mpag.uOnline = esOnline? 1: 0;
	 mpag.alipercep = 0;

	 dRepeat = pRepeat = 0;
	 mpag.esextracash = 0;
	 mpag.nticket = NumPedEcommerce;	 
	 ProcMpag(&mpag, 1);
	 *mpag.dtarjeta = 0;
	 mpag.nticket = 0;	 
	 //if(actSusp) SetTableStatus(actSusp, TABLE_EMPTY); MESAS
	 actSusp = 0;

	 delete [] cl_nombre; delete [] cl_dni; delete [] cl_direcc; delete [] cl_telef;
	 delete [] cl_localidad; delete [] cl_condiva; delete [] cl_fecha;
	 }
	 catch (System::Exception^ excep)
	 {		 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion mpago_", System::DateTime::Now);
		 writer->WriteLine(fhdatos);
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 throw "Error";
	 }
	 catch(...)
	 {
		 WriteBootLog("ERROR - No manejado en ProcMpag");		 
		 throw "Error";
	 }
}

void mpago_2(int cual, CPosnet ^paramlapos)
{
	//   Apunta a los datos de definicion del medio y efectua algunas validaciones.
	struct mpag_ *p;
	int uVuelto, uPend;
	double importe, vuelto;
	GCStatic::LockMainLoop("mpago_2");

	p = BuscarMpag(cual);
	if(inOper == 4 || inOper == 5)
	{
		if(p) cob_step2(p, xReg);
		return;
	}

	if(!p || inOper < 1  || yReg != 0)
	{
		Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
		return;
	}
	if(inOper == 1 && !pSubtot)
	{
		if(forcesubt)
		{
			Alert("Pulse SUBTOTAL antes de tratar de ingresar", "Algun medio de pago.");
			return;
		}
		else
			subtot(0);
	}

	//  Efectua los calculos.
	if(xTotal.VerTotal() == 0 && xReg != 0.0) return;
	if(inOper == 1) 
	{
		totalAntesDcto = xTotal.VerTotal().ToDouble();
		char *tTotal = xTotal.VerTotal().ToString(2);
		VoucherVar("@TOTAL_A_DCTO@", tTotal);
	}

	if (janisEcom && inOper == 1)	
		AplicaPromoJanis();		
	else
	{
		if (promosCalc && inOper == 1)
			//  Aplica las promociones.
			AplicarPromociones();
	}

	//  Si no se indico TOTAL, lo procesa
	if(xReg == 0.0)
		importe = xTotal.VerPendiente().ToDouble();
	else
	{
		HDecim();
		importe = xReg;
	}

	//  Almacena el total del ticket
	double tkTot = xTotal.VerPendiente().ToDouble();
	if(inOper == 1)
	{
		tVtaReal = tkTot;
		WriteCmos(TOTVTAREAL);
		char *tTotal = xTotal.VerTotal().ToString(2);
		VoucherVar("@TOTAL_TICKET@", xTotal.VerTotal().ToString(2));
		PrnTotal();
		STRCPY(tempTot, tTotal);
	}

	//  Determina si puede dar vuelto.
	double uMaxVlto = p->maxvlto;
	if(fabs(uMaxVlto) < 0.001) uMaxVlto = 0.01;
	if(uMaxVlto >= 0 && importe > tkTot && (importe - tkTot) - uMaxVlto >= 0.01)
	{
		sprintf(msg, "Maximo vuelto para '%s' es %.2lf", p->des, p->maxvlto);
		Alert(msg, "Pruebe de nuevo");
		return;
	}

	// Algunas validaciones, si es cuenta corriente.
	if(p->tipo == 'C')
	{
		if(globalCobraCC)
		{
			Alert("No puede cancelar una C/C con otra C/C", "");
			return;
		}
		if(!CompleteCC(p, importe)) return;
	}

	// Pide los datos adionales, segun que medio de cobro se trate.
	// c_nombre[0] = c_numero[0] = c_fecha[0] = c_dni[0] = 
	// c_direcc[0] = c_telef[0] = c_cod[0] = fmtCard[0] = 0;
	if(!MpagoPedirParametros(p->nro, importe)) 
		return;

	// Todas las validaciones se efectuaron. Procesa...
	uPend = uVuelto = 0;
	vuelto = 0.0;
	if((importe - tkTot) >= 0)
	{
		clrCustomerDisplay = true;
		vuelto = importe - tkTot;
		uVuelto = (vuelto >= 0.0)? 1: 0;
	}
	else
	{
		vuelto = tkTot - importe;
		uPend = (fabs(vuelto) >= 0.01)? 1: 0;
	}

	//  Imprime los resultados.
	if(uVuelto)
	{
		CustDisp4("Total", tempTot, "Vuelto", HRng(vuelto));
		DispCash(vuelto, opcdec);
	}
	else
	{
		if(uPend)
		{
			CustDisp("Pendiente", HRng(vuelto));
			DispCash(vuelto, opcdec);
		}
		else
		{
			DispCash(0.0, opcdec);
		}
	}

	//  Copia datos de targetas online, si corresponden.
	bool esOnline = false;
	char *cl_nombre = new char[200];
	char *cl_dni = new char[50];
	char *cl_direcc = new char[200];
	char *cl_telef = new char[200];
	char *cl_localidad = new char[200];
	char *cl_condiva = new char[50];
	char *cl_fecha = new char[50];
	//c_fecha[0] = c_nombre[0] = c_dni[0] = c_telef[0] = c_direcc[0] = c_localidad[0] = 0;
	*fmtCard = 0;
	mpag.civa[0] = 0;

	char ntarj[7];

	int ik = 0;
	int k = 0;		 
	for(k = 0, ik = 5; ik >= 0; k++, ik--)
		ntarj[k] = (char) paramlapos->m_nrotarjeta[ik];		 
	ntarj[6] = 0;

	sprintf(mpag.dtarjeta, "%02d%s", paramlapos->m_codtarjeta, ntarj);
	mpag.nticket = NumPedEcommerce;

	String ^dertarjet = paramlapos->m_nrotarjeta->Trim();
	ik = 2;
	k = dertarjet->Length > 15 ? 10 : 0;
	paramlapos->m_nrotarjeta= "**" + (k != 10 ? "000000" : dertarjet->Substring(dertarjet->Length - 6));		 
	
	String ^cadaut = Convert::ToString(paramlapos->m_autorizacion);
	//paramlapos->m_autorizacion = cadaut->Length > paramlapos->longaut ? Convert::ToUInt32(cadaut->Substring(cadaut->Length - paramlapos->longaut)) : paramlapos->m_autorizacion;	
	paramlapos->m_autorizacion = cadaut->Length > paramlapos->longaut ? cadaut->Substring(cadaut->Length - paramlapos->longaut) : paramlapos->m_autorizacion;	
	
	char *auxaut = new char[10];
	char *auxterm = new char[20];
	Strings::StringToChar(paramlapos->m_terminal, auxterm, 20);
	
	sprintf(auxaut, "%s", paramlapos->m_autorizacion);

	//sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%06d,%04d,%08d",
	sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%s,%04d,%08d,%d,%s,%d",
		paramlapos->m_nrotarjeta,
		paramlapos->m_vencim,
		"N",
		paramlapos->m_cuotas, 
		0.0, //paramlapos->m_tasarecargo,
		0.0, //paramlapos->m_recargo,
		paramlapos->m_lote, 
		auxaut, //paramlapos->m_autorizacion,
		paramlapos->m_cupon, 
		0, paramlapos->m_tipoTerminal, auxterm, paramlapos->m_esqr);

	esOnline = false;		 

	//  Prepara los datos adicionales a grabar, si existen
	if(fmtCard[0] != 0)
	{
		if(SplitParam(fmtCard, cl_fecha, 11))
			if(SplitParam(fmtCard, cl_nombre, 30))
				if(SplitParam(fmtCard, cl_dni, 13))
					if(SplitParam(fmtCard, cl_telef, 12))
						if(SplitParam(fmtCard, cl_direcc, 30))
							SplitParam(fmtCard, cl_localidad, 30);
	}		 

	//  Graba los Resultados.
	mpag.func = DMpag;
	mpag.nro = p->nro;
	mpag.uVuelto = uVuelto;
	mpag.uPend = uPend;
	mpag._fecha = (long) date();
	mpag._hora = (long) time_();
	mpag.cajero = ckaNro;
	store(importe, mpag.importe);
	store(vuelto, mpag.vuelto);
	Trim(c_numero); Trim(c_fecha); Trim(c_nombre); Trim(c_dni);
	Trim(c_telef);  Trim(c_direcc);
	if(strlen(c_numero) > 13) c_numero[13] = 0;
	if(strlen(cl_fecha) > 11) cl_fecha[11] = 0;
	if(strlen(cl_nombre) > 30) cl_nombre[30] = 0;
	if(strlen(cl_dni) > 13) cl_dni[13] = 0;
	if(strlen(cl_telef) > 12) cl_telef[12] = 0;
	if(strlen(cl_direcc) > 30) cl_direcc[30] = 0;
	if(strlen(cl_localidad) > 30) cl_localidad[30] = 0;
	if(strlen(fmtCard) > 99) fmtCard[99] = 0;

	STRCPY(mpag.telef, cl_telef);
	STRCPY(mpag.direc, cl_direcc);
	STRCPY(mpag.local, cl_localidad);
	STRCPY(mpag.cc, c_cod);
	STRCPY(mpag.numero, c_numero);
	STRCPY(mpag.fecha, cl_fecha);
	STRCPY(mpag.nombre, cl_nombre);
	STRCPY(mpag.dni, cl_dni);

	mpag.uOnline = 0;	 	 
	mpag.escredito = paramlapos->m_credito ? 1: 0;
	//STRCPY(mpag.dtarjeta, paramlapos->m_nrotarjeta);
	mpag.esposnet = 1;
	mpag.esextracash = paramlapos->m_extraccion > 0 ? 1: 0;	
	sprintf(mpag.montoextrac, "%.2lf", paramlapos->m_extraccion);
	dRepeat = pRepeat = 0;
	mpag.nticket = NumPedEcommerce;
	ProcMpag(&mpag, 1);
	//if(actSusp) SetTableStatus(actSusp, TABLE_EMPTY); MESAS
	actSusp = 0;
	GCStatic::ReleaseLockMainLoop("mpago_2");	 
	mpag.escredito = 0;	 	 
	mpag.esposnet = 0;	
	*mpag.dtarjeta = 0;
	mpag.nticket = 0;
	*mpag.barrio = 0;
	
	delete [] cl_nombre;		delete [] cl_dni;			delete [] cl_direcc;		delete [] cl_telef;
	delete [] cl_localidad;	delete [] cl_condiva;	delete [] cl_fecha;
	delete [] auxaut; delete[] auxterm;
}


void mpago_3(int cual, TransSmartPointMp ^paramlapos)
{
	//   Apunta a los datos de definicion del medio y efectua algunas validaciones.
	struct mpag_ *p;
	int uVuelto, uPend;
	double importe, vuelto;
	GCStatic::LockMainLoop("mpago_2");

	p = BuscarMpag(cual);
	if(inOper == 4 || inOper == 5)
	{
		if(p) cob_step2(p, xReg);
		return;
	}

	if(!p || inOper < 1  || yReg != 0)
	{
		Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
		return;
	}
	if(inOper == 1 && !pSubtot)
	{
		if(forcesubt)
		{
			Alert("Pulse SUBTOTAL antes de tratar de ingresar", "Algun medio de pago.");
			return;
		}
		else
			subtot(0);
	}

	//  Efectua los calculos.
	if(xTotal.VerTotal() == 0 && xReg != 0.0) return;
	if(inOper == 1) 
	{
		totalAntesDcto = xTotal.VerTotal().ToDouble();
		char *tTotal = xTotal.VerTotal().ToString(2);
		VoucherVar("@TOTAL_A_DCTO@", tTotal);
	}

	if (janisEcom && inOper == 1)	
		AplicaPromoJanis();		
	else
	{
		if (promosCalc && inOper == 1)
			//  Aplica las promociones.
			AplicarPromociones();
	}

	//  Si no se indico TOTAL, lo procesa
	if(xReg == 0.0)
		importe = xTotal.VerPendiente().ToDouble();
	else
	{
		HDecim();
		importe = xReg;
	}

	//  Almacena el total del ticket
	double tkTot = xTotal.VerPendiente().ToDouble();
	if(inOper == 1)
	{
		tVtaReal = tkTot;
		WriteCmos(TOTVTAREAL);
		char *tTotal = xTotal.VerTotal().ToString(2);
		VoucherVar("@TOTAL_TICKET@", xTotal.VerTotal().ToString(2));
		PrnTotal();
		STRCPY(tempTot, tTotal);
	}

	//  Determina si puede dar vuelto.
	double uMaxVlto = p->maxvlto;
	if(fabs(uMaxVlto) < 0.001) uMaxVlto = 0.01;
	if(uMaxVlto >= 0 && importe > tkTot && (importe - tkTot) - uMaxVlto >= 0.01)
	{
		sprintf(msg, "Maximo vuelto para '%s' es %.2lf", p->des, p->maxvlto);
		Alert(msg, "Pruebe de nuevo");
		return;
	}

	// Algunas validaciones, si es cuenta corriente.
	if(p->tipo == 'C')
	{
		if(globalCobraCC)
		{
			Alert("No puede cancelar una C/C con otra C/C", "");
			return;
		}
		if(!CompleteCC(p, importe)) return;
	}

	// Pide los datos adionales, segun que medio de cobro se trate.
	// c_nombre[0] = c_numero[0] = c_fecha[0] = c_dni[0] = 
	// c_direcc[0] = c_telef[0] = c_cod[0] = fmtCard[0] = 0;
	if(!MpagoPedirParametros(p->nro, importe)) 
		return;

	// Todas las validaciones se efectuaron. Procesa...
	uPend = uVuelto = 0;
	vuelto = 0.0;
	if((importe - tkTot) >= 0)
	{
		clrCustomerDisplay = true;
		vuelto = importe - tkTot;
		uVuelto = (vuelto >= 0.0)? 1: 0;
	}
	else
	{
		vuelto = tkTot - importe;
		uPend = (fabs(vuelto) >= 0.01)? 1: 0;
	}

	//  Imprime los resultados.
	if(uVuelto)
	{
		CustDisp4("Total", tempTot, "Vuelto", HRng(vuelto));
		DispCash(vuelto, opcdec);
	}
	else
	{
		if(uPend)
		{
			CustDisp("Pendiente", HRng(vuelto));
			DispCash(vuelto, opcdec);
		}
		else
		{
			DispCash(0.0, opcdec);
		}
	}

	//  Copia datos de targetas online, si corresponden.
	bool esOnline = false;
	char *cl_nombre = new char[200];
	char *cl_dni = new char[50];
	char *cl_direcc = new char[200];
	char *cl_telef = new char[200];
	char *cl_localidad = new char[200];
	char *cl_condiva = new char[50];
	char *cl_fecha = new char[50];

	char *cl_smartPointMP = new char[300];
	//String ^cadena = System::String::Format("{0},{1},{2}", paramlapos->ExternalReference, paramlapos->IdOrdenMercadoPago, paramlapos->IdPayment);
	String ^cadena = System::String::Format("{0},{1}", paramlapos->ExternalReference, paramlapos->IdPayment);
	*cl_smartPointMP = 0;
	Strings::StringToChar(cadena, cl_smartPointMP, 300);
	Strings::StringToChar(paramlapos->ExternalReference, mpSeqIdOrden, 30);
	////c_fecha[0] = c_nombre[0] = c_dni[0] = c_telef[0] = c_direcc[0] = c_localidad[0] = 0;
	//*fmtCard = 0;
	//mpag.civa[0] = 0;

	//char ntarj[7];

	//int ik = 0;
	//int k = 0;		 
	//for(k = 0, ik = 5; ik >= 0; k++, ik--)
	//	ntarj[k] = (char) paramlapos->m_nrotarjeta[ik];		 
	//ntarj[6] = 0;

	//sprintf(mpag.dtarjeta, "%02d%s", paramlapos->m_codtarjeta, ntarj);
	//mpag.nticket = NumPedEcommerce;

	//String ^dertarjet = paramlapos->m_nrotarjeta->Trim();
	//ik = 2;
	//k = dertarjet->Length > 15 ? 10 : 0;
	//paramlapos->m_nrotarjeta= "**" + (k != 10 ? "000000" : dertarjet->Substring(dertarjet->Length - 6));		 

	//String ^cadaut = Convert::ToString(paramlapos->m_autorizacion);
	////paramlapos->m_autorizacion = cadaut->Length > paramlapos->longaut ? Convert::ToUInt32(cadaut->Substring(cadaut->Length - paramlapos->longaut)) : paramlapos->m_autorizacion;	
	//paramlapos->m_autorizacion = cadaut->Length > paramlapos->longaut ? cadaut->Substring(cadaut->Length - paramlapos->longaut) : paramlapos->m_autorizacion;	

	//char *auxaut = new char[10];
	//char *auxterm = new char[20];
	//Strings::StringToChar(paramlapos->m_terminal, auxterm, 20);

	//sprintf(auxaut, "%s", paramlapos->m_autorizacion);

	////sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%06d,%04d,%08d",
	//sprintf(fmtCard, "%s,%04d,%s,%02d,%.3lf,%.2lf,%03d,%s,%04d,%08d,%d,%s,%d",
	//	paramlapos->m_nrotarjeta,
	//	paramlapos->m_vencim,
	//	"N",
	//	paramlapos->m_cuotas, 
	//	0.0, //paramlapos->m_tasarecargo,
	//	0.0, //paramlapos->m_recargo,
	//	paramlapos->m_lote, 
	//	auxaut, //paramlapos->m_autorizacion,
	//	paramlapos->m_cupon, 
	//	0, paramlapos->m_tipoTerminal, auxterm, paramlapos->m_esqr);

	//esOnline = false;		 

	//  Prepara los datos adicionales a grabar, si existen
	if(cl_smartPointMP[0] != 0)
	{
		if(SplitParam(cl_smartPointMP, cl_fecha, 11))
			if(SplitParam(cl_smartPointMP, cl_nombre, 30))
				if(SplitParam(cl_smartPointMP, cl_dni, 13))
					if(SplitParam(cl_smartPointMP, cl_telef, 12))
						if(SplitParam(cl_smartPointMP, cl_direcc, 30))
							SplitParam(cl_smartPointMP, cl_localidad, 30);
	}		 

	//  Graba los Resultados.
	mpag.func = DMpag;
	mpag.nro = p->nro;
	mpag.uVuelto = uVuelto;
	mpag.uPend = uPend;
	mpag._fecha = (long) date();
	mpag._hora = (long) time_();
	mpag.cajero = ckaNro;
	store(importe, mpag.importe);
	store(vuelto, mpag.vuelto);
	Trim(c_numero); Trim(c_fecha); Trim(c_nombre); Trim(c_dni);
	Trim(c_telef);  Trim(c_direcc);
	if(strlen(c_numero) > 13) c_numero[13] = 0;
	if(strlen(cl_fecha) > 11) cl_fecha[11] = 0;
	if(strlen(cl_nombre) > 30) cl_nombre[30] = 0;
	if(strlen(cl_dni) > 13) cl_dni[13] = 0;
	if(strlen(cl_telef) > 12) cl_telef[12] = 0;
	if(strlen(cl_direcc) > 30) cl_direcc[30] = 0;
	if(strlen(cl_localidad) > 30) cl_localidad[30] = 0;
	if(strlen(fmtCard) > 99) fmtCard[99] = 0;

	STRCPY(mpag.telef, cl_telef);
	STRCPY(mpag.direc, cl_direcc);
	STRCPY(mpag.local, cl_localidad);
	STRCPY(mpag.cc, c_cod);
	STRCPY(mpag.numero, c_numero);
	STRCPY(mpag.fecha, cl_fecha);
	STRCPY(mpag.nombre, cl_nombre);
	STRCPY(mpag.dni, cl_dni);

	mpag.uOnline = 0;	 	 
	mpag.escredito = paramlapos->TarjCredito ? 1: 0;
	mpag.esposnet = 1;
	mpag.esextracash = 0; //paramlapos->m_extraccion > 0 ? 1: 0;	
	sprintf(mpag.montoextrac, "%.2lf", 0);
	dRepeat = pRepeat = 0;
	mpag.nticket = NumPedEcommerce;
	ProcMpag(&mpag, 1);
	//if(actSusp) SetTableStatus(actSusp, TABLE_EMPTY); MESAS
	actSusp = 0;
	GCStatic::ReleaseLockMainLoop("mpago_3");	 
	mpag.escredito = 0;	 	 
	mpag.esposnet = 0;	
	*mpag.dtarjeta = 0;
	mpag.nticket = 0;
	*mpag.barrio = 0;

	delete [] cl_nombre;		delete [] cl_dni;			delete [] cl_direcc;		delete [] cl_telef;
	delete [] cl_localidad;	delete [] cl_condiva;	delete [] cl_fecha;	
	delete[] cl_smartPointMP;
}


void ProcMpag(void *mpg_, int fwrite)
{
	double import;
    struct DDmpag_ *mpg;
	 mpg = (struct DDmpag_ *) mpg_;
	 WriteBootLog("AVISO - Entrando a ProcMpag");
	 try
	 {		 
		 if(fwrite) WriteDump(mpg);
#ifdef FACTSQL
		 if (numsecuencia > 0 && fwrite)
		 {	
			 TransSql::MDMpag^ unpago = gcnew TransSql::MDMpag();
			 unpago->Tienda = gcnew System::String(saptienda);
			 unpago->Caja = caja;
			 unpago->Func = mpg->func;
			 unpago->Nro = mpg->nro;
			 unpago->UVuelto = mpg->uVuelto;
			 unpago->UPend = mpg->uPend;
			 unpago->Importe = gcnew System::String(mpg->importe);
			 unpago->Vuelto = gcnew System::String(mpg->vuelto);
			 unpago->Cc = gcnew System::String(mpg->cc);
			 unpago->Numero = gcnew System::String(mpg->numero);
			 unpago->Fecha_c = gcnew System::String(mpg->fecha);
			 unpago->Nombre = gcnew System::String(mpg->nombre);
			 unpago->Dni = gcnew System::String(mpg->dni);
			 unpago->Telef = gcnew System::String(mpg->telef);
			 unpago->Direc = gcnew System::String(mpg->direc);
			 unpago->Local = gcnew System::String(mpg->local);
			 unpago->Civa = gcnew System::String(mpg->civa);
			 unpago->Barrio = gcnew System::String(mpg->barrio);
			 unpago->Mtodesc = gcnew System::String(mpg->mtodesc);
			 unpago->UOnline = mpg->uOnline;
			 unpago->Fecha = mpg->_fecha;
			 unpago->Hora = mpg->_hora;
			 unpago->Cajero = mpg->cajero;
			 unpago->Dtarjeta = gcnew System::String(mpg->dtarjeta);
			 unpago->Nticket = mpg->nticket;
			 unpago->Alipercep = mpg->alipercep;
			 unpago->Escredito = mpg->escredito;
			 unpago->Esposnet = mpg->esposnet;
			 unpago->CnlVta = gcnew System::String(sapcnlvta);
			 unpago->Towrite = true;			 
			 Dump::sqltrans->Add(unpago);
		 }
#endif

		 if(mpg->nro == 9999)
		 {
			STRCPY(c_telef, mpg->telef);
			STRCPY(c_direcc, mpg->direc);
			STRCPY(c_cod, mpg->cc);
			STRCPY(c_numero, mpg->numero);
			STRCPY(c_fecha, mpg->fecha);
			STRCPY(c_nombre, mpg->nombre);
			STRCPY(c_dni, mpg->dni);
			STRCPY(c_localidad, mpg->local);
			STRCPY(c_condiva, mpg->civa);
			float auxper = mpg->alipercep / 100.0;
			sprintf(c_alicuota, "%.2lf", auxper);
			STRCPY(c_percep, (auxper > 0.01 ? "S" : " "));

			/*mpg->codpostal = atoi(c_codpostal);
			mpg->reparticion = ClienteBenef;*/

			GlobalCCSelected = 1;
			mpg->uOnline = 0;

			if(!_stricmp(c_condiva, "RI") || !_stricmp(c_condiva, "NI")){
				pf->TipoComprobante = 81;
				GlobalTF = 1;
			}
			else if(!_stricmp(c_condiva, "CF") || !_stricmp(c_condiva, "EX") || !_stricmp(c_condiva, "NR") || !_stricmp(c_condiva, "MO")){
				pf->TipoComprobante = 82;
				GlobalTF = 2;
			}

			if (Dump::docActual == nullptr)
				Dump::docActual = gcnew LibEntidades::Alberdi::HeaderDoc();
			Dump::docActual->CodCli = System::Convert::ToString(Dump::actCliente->Cod);
			Dump::docActual->Nombre = Dump::actCliente->Nombre;
			Dump::docActual->Direccion = Dump::actCliente->Direccion;
			Dump::docActual->mReparticion->Descripcion = Dump::actCliente->RepaDes;
			Dump::docActual->mReparticion->ReparticionID = Dump::actCliente->Repa;

			Dump::docActual->Caja = caja;
			Dump::docActual->Cajero = ckaNro;
			Dump::docActual->Tienda->Tienda = gcnew String(saptienda);
			Dump::docActual->Tienda->IdTienda = suc;
			Dump::docActual->CanalVenta = gcnew String(sapcnlvta);
			Dump::docActual->TipoComprobante = pf->TipoComprobante;
						
		 }
		 else
		 {
			 //if (!mpag.esextracash)
			 //{			 
				double tkTot = xTotal.VerPendiente().ToDouble();
				printerTot = 0;
				if(inOper == 1)
				{
					// Abre la gaveta
					pf->OpenDrwr();
					//WriteBootLog("AVISO - Paso de Abrir Gaveta");

					// Corrige redondeos si el impresor no esta en modo delayed			
					if(globalDelayPrint)
					{
						double redondeo = tkTot;
						redondeo += FactuReca(0);
						redondeo -= (printerTot / 100);				
						if(redondeo > 0.01)
						{
							sprintf(tmsg, "ERROR DE REDONDEO: tkTotal=%.5lf, printerTot=%.5lf, redondeo=%.5lf",
								tkTot, printerTot, redondeo);
							pf->AjustarRedondeo(redondeo);
						}
					}
				}

				inOper = 2;
				import = atof_(mpg->importe);
				if(fwrite)
				{			
					// Imprime el medio de pago.
						PrintMpag(mpg);	

					// Actualiza la tabla de acumulados en gaveta.
					struct mpag_ *mp = BuscarMpag(mpg->nro);
					if(mpg->uVuelto)
					{
						// Si tiene vuelto, optimiza para el caso que el codigo de vuelto sea el mismo del medio de pago.
						double vlto = atof(mpg->vuelto);
						if(mp->vltocod == mp->nro)
							StoreGaveta(mp, import - vlto);
						else
						{
							// No es el mismo codigo. Graba el ingreso por un lado y el vuelto por otro.
							StoreGaveta(mp, import);
							struct mpag_ *vmp = BuscarMpag(mp->vltocod);
							if(vmp == NULL) vmp = mp;
							StoreGaveta(vmp, -vlto);
						}
					}
					else
						StoreGaveta(mp, import);
				}
				if(!mpg->uPend)                   // Cerro la Operacion.
				{
					hasarEop = 0;
					AcumIVA = ValueIvas();
					WriteEOPFiscal(0);
					///if(actSusp) SetTableStatus(actSusp, TABLE_EMPTY);
					//System::Windows::Forms::MessageBox::Show("vuelve de WriteEOPFiscal");
				}
				else
				{
					xTotal.AddPago(BDecimal(import));
					HClear();
					yReg = xReg = 0.0;
				}
			//}
			 
		 }
		 WriteBootLog("AVISO - Sale de ProcMpag");
	 }
	 catch (System::Exception^ excep)
	 {		 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ProcMpag", System::DateTime::Now);
		 writer->WriteLine(fhdatos);		
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 System::String^ vmpago = System::String::Format("nro={0}, importe={1}, vuelto={2}, uVuelto={3}, uPend={4}, uOnline={5}, mtodesc={6}", mpg->nro, gcnew System::String(mpg->importe), gcnew System::String(mpg->vuelto), (int)mpg->uVuelto, (int)mpg->uPend, (int)mpg->uOnline, gcnew System::String(mpg->mtodesc));
		 writer->WriteLine(vmpago);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 /*ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");*/
		 throw "Error";
	 }	 
}


void ProcNcMpag(void *mpg_, int fwrite)
{
	double import;
    struct DDNcPago_ *mpg;
	 mpg = (struct DDNcPago_ *) mpg_;
	 WriteBootLog("AVISO - Entrando a ProcMpag");
	 try
	 {		 
		 if(fwrite) WriteDump(mpg);

		 if(mpg->nro == 9999)
		 {
			STRCPY(c_telef, mpg->telef);
			STRCPY(c_direcc, mpg->direc);
			STRCPY(c_cod, mpg->cc);
			STRCPY(c_numero, mpg->numero);
			STRCPY(c_fecha, mpg->fecha);
			STRCPY(c_nombre, mpg->nombre);
			STRCPY(c_dni, mpg->dni);
			STRCPY(c_localidad, mpg->local);
			STRCPY(c_condiva, mpg->civa);
			float auxper = mpg->alipercep / 100.0;
			sprintf(c_alicuota, "%.2lf", auxper);
			STRCPY(c_percep, (auxper > 0.01 ? "S" : " "));
			
			GlobalCCSelected = 1;			
			mpg->uOnline = 0;

			if(!_stricmp(c_condiva, "RI") || !_stricmp(c_condiva, "NI")){
				pf->TipoComprobante = 112;
				GlobalTF = 1;
			}
			else if(!_stricmp(c_condiva, "CF") || !_stricmp(c_condiva, "EX") || !_stricmp(c_condiva, "NR") || !_stricmp(c_condiva, "MO")){
				pf->TipoComprobante = 113;
				GlobalTF = 2;
			}
						
		 }
		 else
		 {
			 //if (!mpag.esextracash)
			 //{			 
				double tkTot = xTotal.VerPendiente().ToDouble();
				printerTot = 0;
				if(inOper == 1)
				{
					// Abre la gaveta
					pf->OpenDrwr();
					//WriteBootLog("AVISO - Paso de Abrir Gaveta");

					// Corrige redondeos si el impresor no esta en modo delayed			
					if(globalDelayPrint)
					{
						double redondeo = tkTot;
						redondeo += FactuReca(0);
						redondeo -= (printerTot / 100);				
						if(redondeo > 0.01)
						{
							sprintf(tmsg, "ERROR DE REDONDEO: tkTotal=%.5lf, printerTot=%.5lf, redondeo=%.5lf",
								tkTot, printerTot, redondeo);
							pf->AjustarRedondeo(redondeo);
						}
					}
				}

				inOper = 2;
				import = atof_(mpg->importe);

				// Imprime el medio de pago.
				PrintMpag(mpg);	

				// Actualiza la tabla de acumulados en gaveta.
				struct mpag_ *mp = BuscarMpag(mpg->nro);
				if(mpg->uVuelto)
				{
					// Si tiene vuelto, optimiza para el caso que el codigo de vuelto sea el mismo del medio de pago.
					double vlto = atof(mpg->vuelto);
					if(mp->vltocod == mp->nro)
						StoreGaveta(mp, import - vlto);
					else
					{
						// No es el mismo codigo. Graba el ingreso por un lado y el vuelto por otro.
						StoreGaveta(mp, import);
						struct mpag_ *vmp = BuscarMpag(mp->vltocod);
						if(vmp == NULL) vmp = mp;
						StoreGaveta(vmp, -vlto);
					}
				}
				else
					StoreGaveta(mp, import);

				if(!mpg->uPend)                   // Cerro la Operacion.
				{
					hasarEop = 0;
					WriteEOPFiscal(0);				
				}			
			//}
			 
		 }
		 WriteBootLog("AVISO - Sale de ProcMpag");
	 }
	 catch (System::Exception^ excep)
	 {		 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ProcMpag", System::DateTime::Now);
		 writer->WriteLine(fhdatos);		
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 System::String^ vmpago = System::String::Format("nro={0}, importe={1}, vuelto={2}, uVuelto={3}, uPend={4}, uOnline={5}, mtodesc={6}", mpg->nro, gcnew System::String(mpg->importe), gcnew System::String(mpg->vuelto), (int)mpg->uVuelto, (int)mpg->uPend, (int)mpg->uOnline, gcnew System::String(mpg->mtodesc));
		 writer->WriteLine(vmpago);
		 writer->WriteLine("----------------------------------");
       writer->Close();
		 /*ClearMsg();
		 Alert("Ha ocurrido un error Interno del POS", "La aplicación se cerrara");*/
		 throw "Error";
	 }
	 
}


//
//   Imprime los descuentos (promociones) por medio de pago, resumidos, si los hay.
//   Usa las variables:
//       dmp_Depto, dmp_CntHeader, dmp_CntTrailer (int)
//		 dmp_Header[5], dmp_Trailer[5] (char *)
//
void PrintDescuento()
{
	RngPago *rng;
	double descuento = 0.0;

	for(int i=0; i<stackPagos->Count(); i++)
	{
		rng = (RngPago *) stackPagos->Item(i);
		descuento += rng->descto;
	}

	VoucherVar("@DESCTO_MPAGO@", descuento);
	VoucherVar("@TOTAL_TICKET@", totalAntesDcto - descuento);

	if(descuento < 0.01) return;

	if(dmp_Voucher) StackVoucher(dmp_Voucher);

	//   Imprime el header
	/*for(int i=0; i<dmp_CntHeader; i++)
		pf->PrintText(dmp_Header[i]);*/
	
	//   Registra la venta en el departamento
	inOper = 1;
	yReg = -descuento * 100;
	xReg = dmp_Depto;
	plu(1);
	inOper = 2;

	/*inOper = 1;
	xReg = -descuento;
	yReg = 1;
	double sTot = pTot;
	depto(dmp_Depto);
	inOper = 2;
	pTot = sTot;*/

	//  Imprime el trailer
	/*for(int i=0; i<dmp_CntTrailer; i++)
		pf->PrintText(dmp_Trailer[i]);*/
}

// Para evitar casos extraños de reimpresion del medio de pago
bool enReproceso = false;

//   Imprime los datos de un medio de pago.
void PrintMpag(void *mpg_)
{
	struct mpag_ *p;
	double import, descto;
    struct DDmpag_ *mpg;
	 //char *lsmens = new char[100];
   
	WriteBootLog("AVISO - Entrando a PrintMpag");
	//   Si hay vendedor activo, imprime sus datos.
	if(!VendedPrinted) PrintVen();

	//   Busca los datos fijos del medio de pago usado (configuracion).
	mpg = (struct DDmpag_ *) mpg_;
	//if (!mpg->esextracash)
	//{	
		p = (mpag_ *) bsearch(&mpg->nro, mpagos, cMpag, sizeof(struct mpag_), mpgcmp2);
		if(!p) return;
		import = atof_(mpg->importe);
		descto = atof_(mpg->mtodesc);

		if(descto >= 0.01)
		{
			WriteTickTmp("", "Descuento", 0,	0, 0, descto, 0);
			//DispDeta("Descuento", HRng(descto));
			//if(!prndelay || globalDelayPrint) WriteTickTmp("", "Descuento", 0, 0, 0, descto, 0);
		}

 		if(import)
		{
	//		DispDeta("", "");
			//DispDeta(p->des, HRng(import));
			if(!prndelay || globalDelayPrint) WriteTickTmp("", p->des, 0, 0, 0, import, 0);
		}
		
		//   Envía información genérica para posibles vouchers.
		EnLetras(fabs(import), "MONTO_PAGO");
		EnLetras(fabs(atof_(mpag.vuelto)), "VUELTO");	
		EnLetras(fabs(import)-fabs(atof_(mpag.vuelto)), "NETO_PAGO");	
		VoucherVar("@MEDIO_PAGO@", p->des);
		
		//   Envía información específica de cada medio de pago, para los posibles vouchers.
		char *nada = "";
		switch(p->tipo)
		{
			case 'C':		// Credito en cuentas corrientes.
				VoucherVar("@CRED_NOMBRE@", mpag.nombre);
				VoucherVar("@CRED_DNI@", mpag.dni);
				VoucherVar("@CRED_CODI@", mpag.cc);
				VoucherVar("@CRED_FECHA@", mpag.fecha);
				break;
				
			default:		// Limpia lo que no se usa
				VoucherVar("@CRED_NOMBRE@", nada);
				VoucherVar("@CRED_DNI@", nada);
				VoucherVar("@CRED_CODI@", nada);
				VoucherVar("@CRED_FECHA@", nada);
				break;
		}

		if(mpg->uVuelto)
		{
			double vlto;

			vlto = atof_(mpag.vuelto);
			if(vlto != 0.0)
			{
	//			DispDeta("Vuelto", HRng(vlto));
				if(!prndelay || globalDelayPrint) WriteTickTmp("", "VUELTO", 0, 0, 0, vlto, 0);
				EnLetras(vlto, "@VUELTO@");						
			}		
		}	   
		// Si corresponde, prepara la impresion del voucher
		if(p->voucher != 0) 
		{
			//WriteBootLog("AVISO - Stack voucher");
			StackVoucher(p->voucher);
		}

		// Cierra la impresion.
		if(import != 0)
		{
			RngPago *rng = new RngPago;
			STRCPY(rng->des, p->des);
			/*if(Trim(mpg->numero)[0] != 0 && !pf->solofact)
			{
				STRCAT(rng->des, " (");
				STRCAT(rng->des, mpg->numero);
				STRCAT(rng->des, ")");
			}*/
			if (mpg->esextracash)
			{
				STRCAT(rng->des, " - EXT ");
				STRCAT(rng->des, mpg->montoextrac);
			}
			if (strlen(mpSeqIdOrden) > 1){
				STRCAT(rng->des, "  ");
				STRCAT(rng->des, mpSeqIdOrden);
				mpSeqIdOrden[0] = 0;
			}

			if (mpg->uVuelto && atof(mpg->vuelto) > 0.01 && pf->solofact)
			{
				sprintf(rng->des, "%s %.2lf, Vlto: %.2lf", Trim(p->des), atof(mpg->importe), atof(mpg->vuelto));			
			}

			rng->monto = import;
			rng->descto = descto;
			stackPagos->Add(rng);
			
			//  Si no deja pendiente, entonces esta cerrando el ticket...
			if(!mpg->uPend)
			{
				if(!enReproceso)
				{
					enReproceso = true;
					if(!globalDelayPrint) PrintDescuento();
					for(int i=0; i<stackPagos->Count(); i++)
					{
						rng = (RngPago *) stackPagos->Item(i);
						if(!globalDelayPrint) 
							pf->PrintMPago(rng->des, rng->monto); // (i+1 == stackPagos->Count()));
					}
					stackPagos->Clear();
				}
				stackPagos->Clear();
			}
		}
		enReproceso = false;
	//}
	WriteBootLog("AVISO - Saliendo de PrintMpag");
}

//   Imprime los datos de un medio de pago.
void PrintNcMpag(void *mpg_)
{
	struct mpag_ *p;
	double import, descto;
	struct DDNcPago_ *mpg;	

	WriteBootLog("AVISO - Entrando a PrintNcMpag");
	
	//   Busca los datos fijos del medio de pago usado (configuracion).
	mpg = (struct DDNcPago_ *) mpg_;
	if (!mpg->esextracash)
	{	
		p = (mpag_ *) bsearch(&mpg->nro, mpagos, cMpag, sizeof(struct mpag_), mpgcmp2);
		if(!p) return;
		import = atof_(mpg->importe);
		descto = atof_(mpg->mtodesc);

		if(descto >= 0.01)
		{
			WriteTickTmp("", "Descuento", 0,	0, 0, descto, 0);			
		}

		if(import)
		{
			if(!prndelay || globalDelayPrint) 
				WriteTickTmp("", p->des, 0, 0, 0, import, 0);
		}

		VoucherVar("@MEDIO_PAGO@", p->des);		
		

		if(mpg->uVuelto)
		{
			double vlto;

			vlto = atof_(mpag.vuelto);
			if(vlto != 0.0)
			{
				//			DispDeta("Vuelto", HRng(vlto));
				if(!prndelay || globalDelayPrint) 
					WriteTickTmp("", "VUELTO", 0, 0, 0, vlto, 0);
				EnLetras(vlto, "@VUELTO@");						
			}		
		}	   
		// Si corresponde, prepara la impresion del voucher
		if(p->voucher != 0) 
		{
			//WriteBootLog("AVISO - Stack voucher");
			StackVoucher(p->voucher);
		}

		// Cierra la impresion.
		if(import != 0)
		{
			RngPago *rng = new RngPago;
			STRCPY(rng->des, p->des);
			if(Trim(mpg->numero)[0] != 0 && !pf->solofact)
			{
				STRCAT(rng->des, " (");
				STRCAT(rng->des, mpg->numero);
				STRCAT(rng->des, ")");
			}
			if (mpg->uVuelto && atof(mpg->vuelto) > 0.01 && pf->solofact)
			{
				sprintf(rng->des, "%s %.2lf, Vlto: %.2lf", Trim(p->des), atof(mpg->importe), atof(mpg->vuelto));			
			}

			rng->monto = import;
			rng->descto = descto;
			stackPagos->Add(rng);

			//  Si no deja pendiente, entonces esta cerrando el ticket...
			if(!mpg->uPend)
			{
				if(!enReproceso)
				{
					enReproceso = true;
					if(!globalDelayPrint) PrintDescuento();
					for(int i=0; i<stackPagos->Count(); i++)
					{
						rng = (RngPago *) stackPagos->Item(i);
						if(!globalDelayPrint) 
							pf->PrintMPago(rng->des, rng->monto); // (i+1 == stackPagos->Count()));
					}
					stackPagos->Clear();
				}
				stackPagos->Clear();
			}
		}
		enReproceso = false;
	}
	WriteBootLog("AVISO - Saliendo de PrintMpag");
}


//   Carga, desde el archivo 'mpagos.dbf', la definicion de los Medios de Pago.
int mpgcmp1(void const *d1, void const *d2)
{
    return ((struct mpag_ *) d1)->nro - ((struct mpag_ *) d2)->nro;
}

int mpgcmp3(void const *d1, void const *d2)
{
	struct mpag_ 
		*mp1 = (struct mpag_ *) d1, 
		*mp2 = (struct mpag_ *) d2;
	return stricmp(mp1->des, mp2->des);
}

//void LoadVueltos()
//{
//	char *varcodigo = new char[7];
//	char *vardescrip = new char[50];
//	char *varobliga = new char[2];
//
//	if (dbvuelto.Use("pos\\vueltos"))
//	{
//		Dump::ListaVueltos->Clear();
//		dbvuelto.Go(DB_GO_TOP);
//		while (!dbvuelto.Eof())
//		{
//			dbvuelto.Get("codigo", varcodigo);
//			dbvuelto.Get("descrip", vardescrip);
//			dbvuelto.Get("crgdts", varobliga);
//			Controles::TVuelto ^unvuelto = gcnew Controles::TVuelto();
//			unvuelto->Codigo = gcnew System::String(varcodigo);
//			unvuelto->Descrip = gcnew System::String(vardescrip);
//			unvuelto->Obligatorio = varobliga[0] == 'T';
//			Dump::ListaVueltos->Add(unvuelto);
//			dbvuelto.Skip(1);
//		}
//
//	}
//
//	delete varcodigo;
//	delete vardescrip;
//	delete varobliga;
//}

int LoadMpag()
{
	 struct mpag_ *mpg, *mpacu;

	 cMpag = 0;
	 mpconso = new ::ArrayList(false, "LoadMpag - mpconso");
	 mpreti = new ::ArrayList(true, "LoadMpag - mpreti");

	 if(dbmpag.Use(F_Mpagos))
	 {
		 mpagos = new mpag_[dbmpag.RecCount()];
		 bool onlineAdded = false;
		 dbmpag.Go(DB_GO_TOP);
		 //while(!dbmpag.Eof())
		 while(!dbmpag.Eof() && !dbmpag.Deleted())
		 {
			 dbmpag.Get("codigo", mp_cod);
			 dbmpag.Get("des", mp_des);
			 dbmpag.Get("tipo", mp_tipo);
			 dbmpag.Get("grptot", mp_grptot);
			 dbmpag.Get("linkcc", mp_linkcc);
			 dbmpag.Get("maxsal", mp_max);
			 dbmpag.Get("voucher", mp_voucher);
			 dbmpag.Get("arqueable", mp_arqueo);
			 dbmpag.Get("descto", mp_descto);
			 dbmpag.Get("maxvlto", mp_maxvlto);
			 dbmpag.Get("vltocod", mp_vltocod);
			 dbmpag.Get("limgav", mp_limgav);
			 dbmpag.Get("bolsin", mp_bolsin);
			 dbmpag.Get("linkecr", mp_linkecr);
			 dbmpag.Get("autoretiro", mp_retiro);
			 dbmpag.Get("extraible", mp_extraible);
			 dbmpag.Get("minextrac", mp_minextracc);
			 dbmpag.Get("maxextrac", mp_maxextracc);
			 dbmpag.Get("desctomp", mp_desctomp);
			 
			 dbmpag.Get("mlimreca", mp_mlimreca);
			 dbmpag.Get("porreca", mp_alirecargo);
			 


			 mpg = &mpagos[cMpag++];
			 mpg->nro = atoi(mp_cod);
			 mpg->tipo = mp_tipo[0];
			 STRCPY(mpg->grptot, mp_grptot);
			 mpg->maxsal = atof_(mp_max);
			 mpg->descto = atof_(mp_descto);
			 mpg->voucher = atoi(mp_voucher);
			 mpg->vltocod = atoi(mp_vltocod);
			 mpg->maxvlto = atof_(mp_maxvlto);
			 mpg->limgav = atof(mp_limgav);
			 mpg->bolsin = atof(mp_bolsin);
			 mpg->linkecr = atoi(mp_linkecr);
			 mpg->extraible = mp_extraible[0] == 'T';
			 mpg->minextracc = atof(mp_minextracc);
			 mpg->maxextracc = atof(mp_maxextracc);
			 mpg->desctomp = mp_desctomp[0] == 'T';	

			 mpg->mlimreca = atof(mp_mlimreca);
			 mpg->alirecargo = atof(mp_alirecargo);

			 if(mpg->tipo == 'Z')
				 tickVltoNro = mpg->nro;

			 if(mpg->voucher > 0)
				 if(!LoadVoucher(mpg->voucher))
					 mpg->voucher = 0;

			 STRCPY(mpg->linkcc, mp_linkcc);
			 STRCPY(mpg->des, Trim(mp_des));
			 if(mp_arqueo != NULL && strlen(mp_arqueo) != 0)
				 mpg->arqueo = new ::ArrayList(mp_arqueo, "mpg-arqueo-item");
			 else
				 mpg->arqueo = NULL;
			 mpg->aData = NULL;

			 // Agrega el MP a la lista de medios de pago 'retirables'
			 if(mpg->nro == 1 || mp_retiro[0] == 'T')
			 {
				 char tempstr[200];
				 sprintf(tempstr, "    %d - %s    ", mpg->nro, mpg->des);
				 mpreti->Add(tempstr, true);
			 }


			 // Agrega a la lista de medios de pago acumuladores, si no se agrego ya.
			 mpacu = (struct mpag_ *)mpconso->Find(mpg->linkecr);
			 if(mpacu == NULL)
			 {
				 mpacu = new mpag_();
				 mpacu->nro = mpg->linkecr;
				 mpacu->linkecr = mpg->linkecr;
				 mpacu->tipo = mpg->tipo;
				 STRCPY(mpacu->des, mpg->grptot);
				 mpacu->arqueo = new ::ArrayList(mp_arqueo, "mpacu-arqueo");
				 mpacu->aData = NULL;
				 mpacu->bolsin += mpg->bolsin;
				 mpconso->Add(mpg->linkecr, mpacu);

				 if(mpacu->tipo == 'T' && tjOnline && !onlineAdded)
				 {
					 onlineAdded = true;

					 // Si estan habilitadas tarjetas online, entonces agrega un consolidador mas.
					 mpacu = new mpag_();
					 mpacu->nro = 0;
					 mpacu->linkecr = 9999;
					 mpacu->tipo = 'T';
					 STRCPY(mpacu->des, "TARJ.ONLINE");
					 mpacu->arqueo = new ::ArrayList(mp_arqueo, "mpacu-arqueo2");
					 mpacu->aData = NULL;
					 mpacu->bolsin = 0;
					 mpconso->Add(mpacu->linkecr, mpacu);
				 }
			 }
			 dbmpag.Skip(1);
		 }
		 dbmpag.Close();
	 }
	 else
		 AbnormalEnd("No puedo cargar medios de pago");

	 
	 //LoadVueltos();
	 if(cMpag > 0)
		qsort(mpagos, cMpag, sizeof(struct mpag_), mpgcmp1);
	 mpconso->Sort(mpgcmp3);
	 stackPagos = new ::ArrayList(true, "LoadMpag-stackPagos");
     return 1;
}

//   Funcion de 'validacion' del codigo de medio de cobro encontrado.
int buscaMpag(char *cual, void *bag)
{
	static char uCod[10];
	int si;

	Padl(Trim(STRCPY(uCod, cual)), 3, ' ');
	if(dbmpag.Locate("codigo", uCod))
	{
		mpagWin->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el medio de pago '%s'.", cual);
	si = GetSiNo(msg, "Lo incorporo como medio de pago nuevo?");
	if(!si)
		return 0;
	dbmpag.AppendBlank();
	dbmpag.Replace("codigo", uCod);
	mpagWin->ShowGets();
	return 1;
}


//   Abre una ventana y permite editar el archivo de departamentos.
void PrograMpag()
{
	int hWin;

	dbmpag.Use(F_Mpagos);
	hWin = wDefine(7, 18, 15, 62, BORDERSINGLE, SNGCURS, prograEsq, "Programacion de Medios", "");
	mpagWin = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Numero...:");
	wLocate(2, 2);  wPstr("Nombre...:");
	wLocate(3, 2);  wPstr("Tipo ....:");
	wLocate(4, 2);  wPstr("CtaCte...:");
	wLocate(5, 2);  wPstr("Saldo Max:");

	mpagWin->AddGet(1, 13, NUMBER_RIGHT_ALIGNED,  2, 0, uCod, NULL, NULL, buscaMpag);
	mpagWin->AddGet(2, 13, "des",     &dbmpag, NULL, NULL, NULL);
	mpagWin->AddGet(3, 13, "tipo",    &dbmpag, NULL, NULL, NULL);
	mpagWin->AddGet(4, 13, "linkcc",  &dbmpag, NULL, NULL, NULL);
	mpagWin->AddGet(5, 13, "maxsal",  &dbmpag, NULL, NULL, NULL);

	mpagWin->AnimaGets(1);
	wRelease(hWin);
	delete mpagWin; mpagWin = NULL;
	dbmpag.Close();
    FREE(mpagos); LoadMpag();
	HClear();
}	

char *ValidCod(EditItem *ei, char *dato)
{
	CC_NotUseUI = 1;
	bool res = vCCcod(dato, NULL);
	CC_NotUseUI = 0;
	if(!res)
	{
		char *msg = "La cuenta indicada no existe";
		Alert(msg, "");
		STRCPY(ei->dato, "");
		return msg;
	}

	STRCPY(cod->dato, c_cod);		Trim(cod->dato);
	STRCPY(nom->dato, c_nombre);	Trim(nom->dato);
	gfcc->Refresh();
	return NULL;
}

//   La siguiente funcion se utiliza para efectuar la cobranza de una cuenta corriente (que,
//   al ingresar a esta funcion, esta en el xReg).
//   Se puede utilizar solo al inicio de ticket y, una vez utilizada, solo se puede continuar
//   digitando medios de pago.
void cobracc(int dummy)
{
	DCobraCC_ cobra;

	//  Efectua primer algunas validaciones de el momento donde se llama a la tecla.
	if(globalCobraCC || inOper >1 || yReg != 0 || xReg != 0.0 || consmode) 
	{
		Alert("No puede hacer cobranzas en este momento", "");
		return;
	}
	//if(!OpenCCFiles())
	if (!Controles::FuncClientes::AbrirClientesSql())
	{
		Alert("El servidor no esta disponible", "Reintente en unos minutos");
		return;
	}

	gfcc = new GenForm(" Cobranza de Cuenta Corriente ");
	gfcc->CambiarFontSize(FONT_CHICO);

	cod = new EditItem("Cuenta Corriente (B=Busca)");
	cod->validFunc = ValidCod;
	gfcc->Items()->Add(cod);

	nom = new EditItem("Nombre Cliente");
	nom->CambiarAncho(30);
	nom->enabled = false;
	gfcc->Items()->Add(nom);

	monto = new EditItem("Importe");
	monto->CambiarTipo(TIPO_MONEDA);

	gfcc->Items()->Add(monto);

	if(gfcc->Animar())
	{
		//   Valida los datos ingresados.
		double uImpo = atof_(monto->dato);
		int codi = atoi(c_cod);
		if(uImpo > 0 && codi != 0)
		{
			//   Registra la operación y la graba.
			cobra.func = DCobraCC;
			cobra.ctacte = codi;
			store(uImpo, cobra.imp);
			ProcCobraCC(&cobra, 1);

			//  Llama a la pantalla de medios de pago.
			mpago_(-1, NULL);
		}
	}

	HClear();
	dbcta.Close();
	delete gfcc;
}

//   Asienta el movimiento de cobranzas en el archivo de cuentas corrientes.
int AsieCobrInCtaCte(DCobraCC_ *cob, int fWrite)
{
	char tFileName[100];

	//  Recupera el total cobrado.
	double uCobMonto = atof_(cob->imp);

	//  Asienta en los archivos de cuenta corriente.
	//if(!fWrite || dbcta.Lock(1))
	if(!fWrite)
	{
		//  Primero, actualiza el saldo del cliente.
		System::String ^csaldo = Controles::FuncClientes::LeeSaldoCli(Dump::actCliente->Cod);
		Strings::StringToChar(csaldo, c_saldo, 20);

		//dbcta.Get("saldo", c_saldo);
		double saldo = atof_(c_saldo);
		sprintf(tFileName, "%.2lf", saldo);
		VoucherVar("@COBCC_SALANT@", tFileName);
		sprintf(tFileName, "%.2lf", saldo - uCobMonto);
		if(fWrite) {
			//dbcta.Replace("saldo", tFileName);
			Controles::FuncClientes::ActualizaSaldoCli(Dump::actCliente, gcnew String(tFileName));
		}
		VoucherVar("@COBCC_SALNUE@", tFileName);
		sprintf(tFileName, "%.2lf", uCobMonto);
		VoucherVar("@COBCC_MONTO@", tFileName);

		////  Ahora, registra el movimiento...
		//dbf *ccm = new dbf;
		//STRCAT(STRCPY(tFileName, usaPath), "ccmovi");
		//view._exclusive = 0;
		//if(ccm->Use(tFileName))
		//{
		//	//  Prepara la fecha
		//	char *fec = fecha();
		//	VoucherVar("@COBCC_FECHA@", fec);
		//	memmove(tFileName + 6, fec, 2); fec += 3;
		//	memmove(tFileName + 4, fec, 2); fec += 3;
		//	memmove(tFileName + 0, fec, 4);
		//	tFileName[8] = 0;

		//	//  Prepara el codigo de la cuenta.
		//	char tCod[10];
		//	_itoa(cob->ctacte, tCod, 10);
		//	Padl(Trim(tCod), 4, ' ');

		//	//  Numero de operacion que le corresponde.
		//	char tickNro[20];
		//	_itoa(ticket, tickNro, 10);
		//	VoucherVar("@COBCC_COMPNRO@", tickNro);

		//	//  Asentar el movimiento.
		//	if(fWrite)
		//	{
		//		ccm->AppendBlank();
		//		ccm->Replace("tcom", "RC");
		//		ccm->Replace("fecha", tFileName);
		//		ccm->Replace("codcli", tCod);
		//		ccm->Replace("cuota", "0");
		//		ccm->Replace("fchvnc", tFileName);
		//		sprintf(tFileName, "%.2lf", -uCobMonto);
		//		ccm->Replace("monto", tFileName);
		//		ccm->Replace("aplicado", "0");
		//		ccm->Replace("comnro", tickNro);
		//	}

		//	//  Cierra la base de movimientos
		//	ccm->Close();
		//}

		//view._exclusive = 1;
		//if(fWrite) dbcta.Lock(0);
		//dbcta.Close();
	}
	else
	{
		Alert("Esta CTA CTE esta siendo usada por otra pantalla",
				"Por favor, pida que la desocupen para poder seguir...");
		//dbcta.Close();
		return 0;
	}
	return 1;
}

//   Imprime el voucher y asienta en las variables de memoria
int loadCobroVoucher = 0;
void PrintCobroVoucher()
{
	if(!loadCobroVoucher)
		if(LoadVoucher(20))
			loadCobroVoucher = 1;
	if(loadCobroVoucher)
		StackVoucher(20);
}

//   Asienta el comienzo de la cobranza de una cuenta corriente.
void ProcCobraCC(void *cobra_, int fwrite)
{
    struct DCobraCC_ *cobra;
	char tCod[20];

	//   Reseekea los datos del cliente.
	cobra = (DCobraCC_ *) cobra_;
	//if(!OpenCCFiles()) return;
	if (!Controles::FuncClientes::AbrirClientesSql()) return;
	sprintf(tCod, "%d", cobra->ctacte);
	CC_NotUseUI = 1;
	if(!vCCcod(tCod, NULL))
	{
		CC_NotUseUI = 0;
		//dbcta.Close();
		return;
	}
	CC_NotUseUI = 0;

	//   Registra el proceso si corresponde.
	if(fwrite) WriteDump(cobra);
	while(!AsieCobrInCtaCte(cobra, fwrite))
		;

	//   Imprime la operacion.
	double uImport = atof_(cobra->imp);
	WriteTickTmp("", "Cobranza C/C", 0, 0, 0, uImport, 0);

	//   Activa el flag y registra la cobranza.
	inOper = globalCobraCC = 1;
	xTotal.AddVenta(BDecimal(uImport));
	c_MontoCobro = uImport;
	PrintTktTot();
}

//   La siguiente funcion se utiliza para activar una cuenta corriente dado su codigo
//   (que, al ingreso a esta funcion, esta en el xReg).
//   Se puede utilizar solo al inicio de un ticket y una vez utilizada empieza a
//   obligar a que se lleve todo en cuenta corriente.
//   Es decir:
//		- Despues de cada item del ticket, se verificara que el SALDO y se podria impedir
//		  que el cliente siga llevando.
//		- Podria forzar la utilizacion de una lista de precios especifica.
//		- Podria obligar a que el medio de pago sea efectivo.
//
//   Esta funcion puede ser llamada desde el manejador de teclado o desde el procesador de
//   patrones de codigo de barra.
void cltonl(int dummy)
{
	char tCod[20];
	DCCOnln_ onln;

	//  Efectua primer algunas validaciones de el momento donde se llama a la tecla.
	if(inOper >1 || yReg != 0 || consmode) return;

	//  Procesa una posible instruccion de DESACTIVACION.
	if(globalCCOnline)
	{
		if(cc_UsaListas && (xTotal.VerVenta() != 0))
		{
			Alert("No puede DESACTIVAR la cuenta porque estuvo",
				  "vendiendo a OTRO PRECIO. Cancele el ticket...");
			return;
		}

		//  Desactiva la cuenta corriente.
		onln.func = DCCOnln;
		onln.ctacte = 0;
		onln.flagPrec = 0;
		ProcCCOnln(&onln, 1);
		HClear();
		return;
	}
	else
		if(xReg == 0.0)	return;		// DEBE venir indicado un codigo de cuenta corriente.

	//  Antes de hacer nada, trata de ubicar la cuenta corriente que le estan pidiendo...
	//if(!OpenCCFiles()) return;
	if (!Controles::FuncClientes::AbrirClientesSql()) return;
	sprintf(tCod, "%.0lf", xReg);
	CC_NotUseUI = 1;
	if(!vCCcod(tCod, NULL))
	{
		CC_NotUseUI = 0;
		//dbcta.Close();
		return;
	}
	CC_NotUseUI = 0;
	//dbcta.Close();

	//  La cuenta existe y todas las variable ya estan cargadas. Efectua validaciones segun
	//  los modos operativos elegidos...
	double salmax = atof_(c_salmax);
	double saldo = atof_(c_saldo);
	if(cc_SoloInicio && xTotal.VerVenta() != 0)
	{
		//   Chequea que el disponible alcance para el ticket ya abierto.
		if(xTotal.VerVenta() > salmax-saldo)
		{
			Alert("No se puede activar la  cuenta porque el total actual",
				  "del ticket SUPERA el disponible de compra del cliente");
			return;
		}

		//   Chequea que no corresponda cambiar el precio porque, de ser así, hay artículos
		//   ya tickeados que deberian ir a otro precio.
		if(cc_UsaListas && (xTotal.VerVenta() != 0))
		{
			Alert("No puede activar un cliente ahora porque ya hay",
				  "venta y este cliente usa otro precio");
			return;
		}
	}
	
	if(salmax-saldo <= 0.01)
	{
		if((c_PorceMax-c_Consumo)<0)
		{
			Alert("La cuenta NO TIENE disponible ni de compra",
				  "ni de precio diferencial...");
			return;
		}
		onln.flagPrec = 1;
	}
	else
		onln.flagPrec = 0;

	//  Activa la visualización de la cuenta corriente.
	onln.func = DCCOnln;
	onln.ctacte = _atoi64(c_cod);
	ProcCCOnln(&onln, 1);
	HClear();
}

//   Activa un cambio de modo de cliente con seguimiento.
void ProcCCOnln(void *onln_, int fwrite)
{
    struct DCCOnln_ *onln;
	char tCod[30];

    onln = (struct DCCOnln_ *) onln_;
	if(fwrite) WriteDump(onln);
	globalCCOnline = 0;
	if(onln->ctacte != 0)
	{
		//   Primero, activa la cuenta corriente.
		//if(!OpenCCFiles()) return;
		if (!Controles::FuncClientes::AbrirClientesSql()) return;
		sprintf(tCod, "%lld", onln->ctacte);
		CC_NotUseUI = 1;
		if(!vCCcod(tCod, NULL))
		{
			CC_NotUseUI = 0;
			//dbcta.Close();
			return;
		}
		CC_NotUseUI = 0;
		//dbcta.Close();
		globalCheckPriceOverflow = 1;
		if(!onln->flagPrec)
			globalCCOnline = 1;
//		else
//			Alert("La cuenta solo se recibe para PRECIO DIFERENCIAL.",
//				  "(No tiene margen para Cuenta Corriente)...");

		//   Ahora, cambia la lista de precios si corresponde.
		int usarListas = 0;
		if(cc_PorceMax)
		{
			if(xTotal.VerVenta() > (c_PorceMax - c_Consumo))
				usarListas = 1;
		}
		else
			usarListas = 1;

		if(usarListas)
		{
			switch(cc_UsaListas)
			{
				case 1:					// Fuerza la lista nro 1.
				case 2:					// Fuerza la lista nro 2.
				case 3:
				case 4:
					ExecOtroPrec(cc_UsaListas, 1);
					break;

				case 99:				// Usa la lista que especifica el cliente.
					switch(c_lista)
					{
						case 0:
						case 1:			// El cliente usa la lista que este programada como default
							break;

						case 2:			// Fuerza un determinado precio
						case 3:
						case 4:
						case 5:
							ExecOtroPrec(c_lista + 1, fwrite);
							break;
					}
					break;
			}
		}
		else
			ExecOtroPrec(defListaPrec + 1, fwrite);
	}
	else
		ExecOtroPrec(defListaPrec + 1, fwrite);

	PrintTktTot();
}

//   La siguiente funcion verifica si debe cambiar al vuelo la lista de precio, dado que el
//   cliente supero el consumo permitido a precio diferencial.
void CheckPriceOverflow()
{
	//   Verifica que la funcion este activa.
	if(!globalCheckPriceOverflow || !cc_PorceMax || listap == 1) return;
	if(xTotal.VerVenta() > (c_PorceMax-c_Consumo))
	{
		Alert("Supero el maximo permitido a precio diferencial",
			  "Se cambiara a precio standard...");
		AlmacConsumo();
		ExecOtroPrec(defListaPrec, 1);
		globalCheckPriceOverflow = 0;
	}
}

//   La siguiente funcion actualiza el acumulador de 'consumido' por un cliente, en el archivo
//   de cuentas corrientes...
void AlmacConsumo()
{
	int msgShowed = 0;
	char lmsg[100];

	try
	{		
		if(!globalCheckPriceOverflow || !Controles::FuncClientes::AbrirClientesSql()) 
			return;
		
		if (acumSaldoPromo > 0 || Dump::actCliente->UsadoMutual)
			Controles::FuncClientes::ActualizaConsumoCli(Dump::actCliente, xTotal.VerVenta().ToDouble(), acumSaldoPromo);
		else
			Controles::FuncClientes::ActualizaConsumo(Dump::actCliente->Cod, xTotal.VerVenta().ToDouble());
	}	
	catch (...)
	{
		sprintf(lmsg, "Error al almacenar consumo del cliente %s", c_savecod);
		WriteBootLog(lmsg);
	}
}


void LeeSaldoConsumo()
{
	int msgShowed = 0;
	char lmsg[100];

	STRCPY(c_saldopromo, "0.00");
	try
	{
		if (!Controles::FuncClientes::AbrirClientesSql())
		{			
			WriteBootLog("No puedo abrir archivos de CC para leer Saldo Promo");
			return;
		}
		System::String ^csaldopromo = Controles::FuncClientes::LeeSaldoPromoCli(Dump::actCliente->Cod);
		Strings::StringToChar(csaldopromo, c_saldopromo, 20);

		sprintf(lmsg, "Saldo Promo del cliente %s", c_saldopromo);
		WriteBootLog(lmsg);
	}	
	catch (...)
	{
		STRCPY(c_saldopromo, "0.00");
		sprintf(lmsg, "Error al leer Saldo Promo del cliente %s", c_savecod);
		WriteBootLog(lmsg);
	}
}

// Mira si corresponde emitir el mensaje de que hay demasiado dinero en gaveta.
void ControlaGaveta()
{
	char m1[50], m2[50], tclk[20];

	if(ckaNro == 0 || !Existe("gaveta.dbf") || rMode) return;
	if(dbgaveta.Use("gaveta"))
	{
		bool avisar = false;
		for(int i = dbgaveta.RecCount(); i > 0; i--)
		{
			dbgaveta.Go(i);
			dbgaveta.Get("cajero", tclk);
			int ccaje = atoi(tclk);
			if(ccaje == ckaNro)
			{
				dbgaveta.Get("monto", m1);
				dbgaveta.Get("maxim", m2);
				if(atof(m1) >= atof(m2) && atof(m2) > 0)
				{
					avisar = true;
					break;
				}
			}
		}
		dbgaveta.Close();

		if(avisar)
			Alert("Solicite al Supervisor que le haga un Retiro", "");
	}
}

// Almacena lo recibido en un medio de pago en el acumulador de dinero en gaveta.
void StoreGaveta(struct mpag_ *mp, double monto)
{
	char tdata[50];

	// Mira si corresponde hacerlo.
	if(mp->maxvlto <= 0) return;

	// Abre el archivo de acumulados.
	bool ok = false;
	if(Existe("gaveta.dbf"))
		ok = dbgaveta.Use("gaveta");
	else
		ok = dbgaveta.creaTable("gaveta", "cajero n(3), cod n(3), monto n(16,2), maxim n(16, 2)");
	if(!ok) return;

	// Busca el registro que nos toca modificar.
	bool found = false;
	for(int i = dbgaveta.RecCount(); i > 0; i--)
	{
		dbgaveta.Go(i);
		dbgaveta.Get("cod", tdata);
		int ccod = atoi(tdata);
		dbgaveta.Get("cajero", tdata);
		int ccaje = atoi(tdata);
		if(ccod == mp->nro && ccaje == ckaNro)
		{
			found = true;
			break;
		}
	}
		
	// Si no encontro registro, lo crea.
	if(!found)
	{
		dbgaveta.AppendBlank();
		_itoa(mp->nro, tdata, 10);
		dbgaveta.Replace("cod", tdata);
		_itoa(ckaNro, tdata, 10);
		dbgaveta.Replace("cajero", tdata);
		sprintf(tdata, "%.2lf", mp->limgav);
		dbgaveta.Replace("maxim", tdata);
	}

	// Calcula y reemplaza el acumulado con el nuevo monto.
	dbgaveta.Get("monto", tdata);
	double aMonto = atof(tdata);
	aMonto += monto;
	sprintf(tdata, "%.2lf", aMonto);
	dbgaveta.Replace("monto", tdata);
	dbgaveta.Close();
}
