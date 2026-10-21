#include "stdafx.h"
//#include "TjOnline.h"

#ifdef POSXP
#include "TarjOnlineForm.h"
#include "GCStatic.h"
#include "StaticDump.h"
#include "TarjOnlineTouch.h"

#include "frmAnulaPrisma.h"
#include "frmAnulaSmartPoint.h"
#include "frmAnulaPrismaECR.h"
#include "frmAnulaCloverECR.h"
#include "FUNCS.H"

using namespace pos;
using namespace Alberdi::TJOCommon::Prisma;
using namespace Alberdi::TJOCommon::VerifoneCommon;
using namespace Alberdi::TJOCommon::ComunicaMercadoPago;
using namespace Alberdi::TJOCommon::PrismaECR;
using namespace Alberdi::TJOCommon::CloverECR;

#else
extern int caja;
#endif

void WriteBootLog(char *msg);

// Flag que activa o desactiva, de forma global, la funcionalidad de tarjetas online.
int tjOnline;
int tjPrisma; //Agregado para Verifones Prisma
int tjSmartPoint; //Agregado para Verifones Smart Point de MercadoPago

// Lista con las tarjetas que está usando el ticket actual.
::ArrayList infoTarjetas = ::ArrayList(true, "InfoTarjetasStaticCtor");

// Lista de definicion de las tarjetas online.
::ArrayList defTarjetas = ::ArrayList(true, "DefTarjetasStaticCtor");

// Rangos de tarjetas cargados
::ArrayList rangos = ::ArrayList(true, "RangosStaticCtor");

/////////////// Datos para la interface con JAuton   /////////////////////////////////
int version;			// Version del autorizador - 1=Msdos, 2=Jauton
char *server;			// IP del server JAuton
int port;				// Port del JAuton
int timeout;			// Timeout de lectura
int offset;				// Para dezplazar el numero de caja y proxiar las mismas
::ArrayList *nodos;		// Nodos definidos online

// Resultado de la ultima operacion online.
OnlineResult OLResult;

// Paquetes de datos.
extern struct DatosTerminal datoCaja;
extern struct RespuestaTerminal respCaja;

void SVoucherVar(TarjInfo *tarj, char *vName, char *msg)
{
	char vname[50];
	sprintf(vname, "@%s@", vName);
	VoucherVar(vname, msg);
}

void SVoucherVar(TarjInfo *tarj, char *vName, bool v)
{
	SVoucherVar(tarj, vName, (v? "Si": "No"));
}

void SVoucherVar(TarjInfo *tarj, char *vName, double d)
{
	static char tbuff[30];
	sprintf(tbuff, "%.2lf", d);
	SVoucherVar(tarj, vName, tbuff);
}

void SVoucherVar(TarjInfo *tarj, char *vName, int d)
{
	static char tbuff[30];
	sprintf(tbuff, "%d", d);
	SVoucherVar(tarj, vName, tbuff);
}

void SaveAuditInfo(TarjInfo *info);

// Procesa las tarjetas. Retorna false si algo fallo durante el proceso.
bool ProcesarTarjetasOnline()
{
	char msg[500], msg2[500];

	// No hace nada si no hay tarjetas online
	if(infoTarjetas.Count() == 0) return true;

	// Efectua el proceso ONLINE de cada tarjeta.
	bool todoOk = true;
	
	//todoOk = false;
	//TarjInfo *t = (TarjInfo *) infoTarjetas.Item(0);
	//t->Cupon = 1234;
	//t->Lote = 9876;
	//t->Terminal = 7272727;
	//
    //OJO.....: Poner tambien 'todoOk = true;' despues del loop

	for(int i=0; todoOk && i<infoTarjetas.Count(); i++)
	{
		TarjInfo *info = (TarjInfo *) infoTarjetas.Item(i);
		if(info->DatosValidos())
		{
			sprintf(msg, "$%.2lf en %s %s", info->Monto, info->Definicion->Nombre, info->TarjetaEnmascarada(true));
			SendMsg("Procesando Autorizacion Online...", msg);
			info->Aprobada = InvelOnline(info);
			ClearMsg();
			if(!info->Aprobada)
			{
				sprintf(msg, "Fallo %s %s", info->Definicion->Nombre, info->TarjetaEnmascarada(true));
				sprintf(msg2, "'%s' (%d)",  OLResult.Mensaje, OLResult.Codigo);
				Alert(msg, msg2);
				todoOk = false;
			}
			else
			{
				info->Cupon = OLResult.Cupon;
				info->Lote = OLResult.Lote;
				info->Terminal = OLResult.Terminal;
				info->ticket = OLResult.Ticket;
			}
		}
	}

	//todoOk = true;	

	// Si alguna tarjeta fallo, deshace las autorizaciones efectuadas y anula el proceso de pago
	if(!todoOk)
	{
		// TODO: Deshacer las autorizaciones obtenidas.
		infoTarjetas.Clear();

		// Retornando false el modulo de 'mpago' deshace todos los pagos previos.
		return false;
	}	
	///factura en el ticket los recargos financieros de los cupones POSNET
	recaMedioPago = 0;
	for each(CPosnet ^posnet in GCStatic::tjPosnet)
	{
		if (posnet->m_recargo > 0)
			recaMedioPago += posnet->m_recargo;
	}
	FacturarRecargo(TIPORECA_FINANCIERO, recaMedioPago);

	// Todas las tarjetas se aprobaron. Calcula los recargos y los imprime.
	// De paso, apila todos los vouchers de tarjetas.
	BDecimal tRecargo = 0;	
	tjonAutor.Clear();
	tjontoprint = false;
	for(int i=0; i<infoTarjetas.Count(); i++)
	{
		TarjInfo *info = (TarjInfo *) infoTarjetas.Item(i);
		/*std::ofstream fout("tarjetas.dat", std::ios::binary);
		if (fout)
		{
			fout.write((char*) &info, sizeof info);
			fout.close();
		}*/

		if(info->Aprobada && info->Cupon != 0)
		{
			// Acumula el recargo
			tRecargo += info->Recargo;
			SVoucherVar(info, "CUOTAS", info->Cuotas);
			SVoucherVar(info, "CUPON", info->Cupon); 
			SVoucherVar(info, "DNI", info->DniFormateado());
			SVoucherVar(info, "LOTE", info->Lote);
			SVoucherVar(info, "MARCA", info->Marca());
			SVoucherVar(info, "MONTO", info->Monto);
			SVoucherVar(info, "NOMBRE", info->Nombre);
			SVoucherVar(info, "NOM_CUOTA", info->NombreCuota());
			SVoucherVar(info, "RECARGO", info->Recargo);
			SVoucherVar(info, "TASA", info->TasaRecargo);
			SVoucherVar(info, "TARJETA", info->TarjetaEnmascarada(false));
			SVoucherVar(info, "USO_LECTOR", info->UsoLector? "Leida con Lector B.M.": "Leida por Teclado Manual" );
			SVoucherVar(info, "VENCIM", info->VencimFormateado());
			SVoucherVar(info, "CAJA", caja);
			SVoucherVar(info, "PVTA", pf->PuntoDeVenta());
			SVoucherVar(info, "CAJERO", clkName());
			SVoucherVar(info, "TICKET", pf->GetTickNro(GlobalTF));
			SVoucherVar(info, "COMER", info->Definicion->NumComercio);
			SVoucherVar(info, "TERMINAL", info->Terminal);
			SVoucherVar(info, "MONEDA", "$ (Pesos)");
			SVoucherVar(info, "NUMCUPON", info->ticket);

			SVoucherVar(info, "IDTRXTIPRE", 0);					
			SVoucherVar(info, "TIPOOPER", "COMPRA ONLINE");		

			// Apila el voucher si corresponde.
			if(info->Definicion->Voucher != 0)
			{							
				StackVoucher(info->Definicion->Voucher);
				StackVoucher(info->Definicion->Voucher + 1);				
			}
			
		}
	}	

	// Imprime el recargo.
	FacturarRecargo(TIPORECA_FINANCIERO, tRecargo);

	// Procesa e imprime los medios de cobro
	for(int i=0; i<infoTarjetas.Count(); i++)
	{
		TarjInfo *info = (TarjInfo *) infoTarjetas.Item(i);
		if(info->Aprobada && info->Cupon != 0 && info->Monto > 0.005)
		{
#ifdef POSXP
			xReg = info->Monto * 100;
#else
			xReg = info->Monto;
#endif
			yReg = 0;
			cDecim = 0;
			pSubtot = true;
			pf->MPagoAdicDesc(info->Marca());
			mpago(info->Definicion->MPago, info);

			// Graba datos de auditoria
			SaveAuditInfo(info);
		}
	}

	// Limpia el registro de tarjetas y vuelve
	infoTarjetas.Clear();
	return todoOk;
}

// Compara dos rangos, para la funcion de ordenacion de los rangos cargados.
// Las ordena de forma que los rangos mas largos (mas especificos) se examinen antes que los
// rangos mas cortos y, cuando dos rangos tienen igual largo, entonces por el valor absoluto
// de los rangos.
int ComparaRangos(const void *e1, const void *e2)
{
	TarjRango *r1 = *((TarjRango **)e1);
	TarjRango *r2 = *((TarjRango **)e2);
	if(r1->LenDesde != r2->LenDesde) return r1->LenDesde - r2->LenDesde;
	if(r1->LenHasta != r2->LenHasta) return r1->LenHasta - r2->LenHasta;
	int res = _stricmp(r1->Desde, r2->Desde);
	if(res != 0) return res;
	return _stricmp(r1->Hasta, r2->Hasta);
}

// Prepara e inicializa el funcionamiento de tarjetas online.
int PrepareTjOnline()
{
	//char tbuff[50];

	if(!tjOnline) return 1;

	if (CargaTjOnline())
		return 1;
	else 
		return 0;
}

// Carga los datos de las tarjetas, separado de PrepareTjOnline para que siempre tenga los datos de las tarjetas
int CargaTjOnline()
{
	char tbuff[50];

//	if(!tjOnline) return 1;

	// Lee la configuracion
	if(OpenIni(F_Online))
	{
		WriteBootLog("No se activan las funciones de tarjetas online porque no se encuentra ONLINE.INI o su equivalente");
		tjOnline = false;
		return 1;
	}

	version = (int)ExtractIniLong("general", "version");
	server = ExtractIniStr("general", "server");
	port = (int)ExtractIniLong("general", "port");
	timeout = (int)ExtractIniLong("general", "timeout");
	offset = (int)ExtractIniLong("general", "offset");
	int cNodos = (int)ExtractIniLong("general", "nodos");
	if (nodos == NULL)
		nodos = new ::ArrayList(true, "PrepareTjOnline-Nodos");
	else
		nodos->Clear();
	
	if(version < 0 || version > 2) version = 2;
	for(int iNodo = 1; iNodo <= cNodos; iNodo++)
	{
		sprintf(tbuff, "nodo%d", iNodo);
		NodoOnline *nodo = new NodoOnline();
		nodo->id = iNodo;
		nodo->nombre = ExtractIniStr(tbuff, "nombre");
		nodo->terminal = (int)ExtractIniLong(tbuff, "terminal");
		nodo->nii = (int)ExtractIniLong(tbuff, "nii");
		nodo->cierre = ExtractIniStr(tbuff, "cierre");
		nodo->activo = (bool)ExtractIniSiNo(tbuff, "activo");
		nodos->Add(nodo);
	}
	CloseIni();

#ifdef POSXP
	// Construye el cliente a usar.
	GCStatic::X25Client = gcnew Bitnet::TJOCommon::TJOClient(
		version, Strings::CharToString(server), port, timeout, nullptr);
	char logMsg[100];
	sprintf(logMsg, "Inicializacion X25 Online: %s", GCStatic::X25Client->Valido? "Ok": 
		Strings::StringToChar(GCStatic::X25Client->ResultMessage, tbuff, 50));
	WriteBootLog(logMsg);
#else
	tjOnline = false;
#endif


	// Abre definiciones de tarjetas online
	dbf *tjdb = new dbf();
	if(!tjdb->Use(F_TjOnline))
	{
		delete tjdb;
		tjOnline = false;
		return 0;
	}

	// Abre definiciones de rangos
	dbf *trang = new dbf();
	if(!trang->Use(F_Rangos))
	{
		tjdb->Close();
		delete tjdb;
		delete trang;
		tjOnline = false;
		return 0;
	}

	// abre definicions de cuotas
	dbf *tcuota = new dbf();
	if(!tcuota->Use(F_CuoTar))
	{
		tjdb->Close();
		trang->Close();
		delete tcuota;
		delete tjdb;
		delete trang;
		tjOnline = false;
		return 0;
	}

	defTarjetas.Clear();
	// Carga las definiciones.
	tjdb->Go(DB_GO_TOP);
	while(!tjdb->Eof())
	{
		TarjDef *def = new TarjDef();

		// Lee los datos de la definicion
		tjdb->Get("Cod", tbuff);			def->Codigo = atoi(tbuff);
		tjdb->Get("Nombre", tbuff);			STRCPY(def->Nombre, Trim(tbuff));
		tjdb->Get("Digitos", tbuff);		def->Digitos = atoi(tbuff);
		tjdb->Get("Nro_Comerc", tbuff);		STRCPY(def->NumComercio, Trim(tbuff));
		tjdb->Get("Cod_Numera", tbuff);		def->CodNumera = atoi(tbuff);
		tjdb->Get("Nodo", tbuff);			def->Nodo = atoi(tbuff);
		tjdb->Get("Pide_CVV", tbuff);		def->PideCvv = (tbuff[0] == 'T');
		tjdb->Get("Voucher", tbuff);		def->Voucher = atoi(tbuff);
		tjdb->Get("Cvv_Cero", tbuff);		def->CvvCero = (tbuff[0] == 'T');
		tjdb->Get("Mpago", tbuff);			def->MPago = atoi(tbuff);
		tjdb->Get("tipo", tbuff);			def->tipo = tbuff[0];
		tjdb->Get("tiprenom", tbuff);			STRCPY(def->TipreNom, Trim(tbuff));
		tjdb->Get("desctoCom", tbuff);			def->porDesctoCom = atoi(tbuff);

		tjdb->Get("codtarjpri", tbuff);			STRCPY(def->CodTarjPrisma, Trim(tbuff));
		tjdb->Get("codplanpr", tbuff);			def->CodPlanPrisma = atoi(tbuff);

		///debe ir separado
		tjdb->Get("longaut", tbuff);				
		if (atoi(tbuff) > 0 && atoi(tbuff) < 6)
			def->longaut = atoi(tbuff);
		tjdb->Get("EnvTrack1", tbuff);		def->EnvTrack1 = (tbuff[0] == 'T');
		tjdb->Get("camdia", tbuff);
		char *promdias = new char[7];
		if (strlen(tbuff) == 7)
			strcpy(promdias, tbuff);
		long vcambio = atol(tbuff);		
		long vdiahoy = (long) System::DateTime::Now.DayOfWeek;
		bool cencontro = false;
		if ((strlen(promdias) == 7) && (promdias[vdiahoy] > '0' && promdias[vdiahoy] <= '9'))
		{			
			dbf *tcuota2 = new dbf();
			if(!tcuota2->Use("pos\\cuotarpr"))
			{
				//tjdb->Close();
				//trang->Close();
				delete tcuota2;				
			}
			else
			{
				tcuota2->Go(DB_GO_TOP);				
				while (!tcuota2->Eof())
				{
					tcuota2->Get("Cod_Tarjet", tbuff);
					int ncodTarj = atoi(tbuff);
					tcuota2->Get("codcuo", tbuff);
					int ncodcuo = atoi(tbuff);
					tcuota2->Get("finicio", tbuff);
					long nfinicio = atol(tbuff);
					tcuota2->Get("ffin", tbuff);
					long nffin = atol(tbuff);
					long nfhoy = date();
					int ncodbus = ((int)promdias[vdiahoy] - 48);
					if (ncodTarj == def->Codigo && ncodbus == ncodcuo && nfhoy >= nfinicio && nfhoy <= nffin)
					{
						TarjCuota *ncuota = new TarjCuota();
						tcuota2->Get("des", ncuota->Nombre);
						tcuota2->Get("nro_cuota", tbuff);	ncuota->Cuota = atoi(tbuff);
						tcuota2->Get("recargo", tbuff);		ncuota->Porcen = atof(tbuff);
						Trim(ncuota->Nombre);
						def->Cuotas->Add(ncuota);
						cencontro = true;
					}
					tcuota2->Skip(1);
				}
				tcuota2->Close(); delete tcuota2;
			}
		}
		if (!cencontro)
		{
			// Lee las cuotas
			tcuota->Go(DB_GO_TOP);
			while(!tcuota->Eof())
			{
				tcuota->Get("Cod_Tarjet", tbuff);
				int codTarj = atoi(tbuff);
				if(codTarj == def->Codigo)
				{
					TarjCuota *cuota = new TarjCuota();
					tcuota->Get("des", cuota->Nombre);
					tcuota->Get("nro_cuota", tbuff);		cuota->Cuota = atoi(tbuff);
					tcuota->Get("recargo", tbuff);			cuota->Porcen = atof(tbuff);
					
					Trim(cuota->Nombre);
					
					def->Cuotas->Add(cuota);
				}
				tcuota->Skip(1);
			}
		}

		// Agrega la definicion a la lista y busca otra
		defTarjetas.Add(def);
		tjdb->Skip(1);
	}

	rangos.Clear();
	// Lee los rangos
	trang->Go(DB_GO_TOP);
	while(!trang->Eof())
	{
		// Busca la definicion de tarjeta que le toca
		trang->Get("Cod_Tarjet", tbuff);
		int codTarj = atoi(tbuff);
		TarjDef *def = NULL;
		for(int i = defTarjetas.Count() - 1; i >= 0; i--)
		{
			def = (TarjDef *)defTarjetas.Item(i);
			if(def->Codigo == codTarj)
			{
				TarjRango *rango = new TarjRango();
				trang->Get("Cod_Desde", rango->Desde);
				trang->Get("Cod_Hasta", rango->Hasta);
				Trim(rango->Desde);
				Trim(rango->Hasta);
				rango->LenDesde = strlen(rango->Desde);				
				rango->LenHasta = strlen(rango->Hasta);
				rango->Definicion = def;

				rangos.Add(rango);
				break;
			}
		}
		trang->Skip(1);
	}

	// Los rangos deben estar ordenados desde el mas grande al mas corto para que, al buscar una
	// tarjeta en un rango los mas largos (mas especificos) se vean ANTES que los mas cortos (mas
	// generales).
	rangos.Sort(ComparaRangos);

	// Cierra todo
	tjdb->Close();		delete tjdb;
	trang->Close();		delete trang;
	tcuota->Close();	delete tcuota;
	return 1;
}

#ifdef POSXP
ref class TjStatic { public: static TarjOnlineForm ^tForm = nullptr; };


ref class TjStTouch { public: static TarjOnlineTouch ^tForm = nullptr; };

// Funcion que es llamada cuando el scanner detecta una barra, y se utiliza para conocer
// si el supervisor esta habilitando el digitado manual del numero de tarjeta.
void EnableManualCard(char *sc)
{
	//   Posibles cambio de llave.
	if(FitPattern(sc_MskLlaveX1) || FitPattern(sc_MskSuperv))
		TjStatic::tForm->AcceptSupervisorCode();
	autoDetected = false;
}
#endif

// Funcion que es llamada cuando se desea pedir datos de las tarjetas de credito online que usa el cliente.
// Retorna el MONTO TOTAL en tarjetas online. El detalle va en el array 'infoTarjetas'
double PedirDatosTarjetas(double pendiente)
{
	if(!tjOnline) return 0;

	// Asegura que exista por lo menos una entrada en la lista.
	if(infoTarjetas.Count() < 1)
	{
		TarjInfo *info = new TarjInfo();
		info->Monto = pendiente;
		infoTarjetas.Add(info);
	}

#ifdef POSXP
	
	// Pide los datos de las tarjetas a usar
	ScannerRedirectedFunc = EnableManualCard;
	TjStatic::tForm = gcnew TarjOnlineForm(pendiente);
	TjStatic::tForm->ShowDialog();
	if(TjStatic::tForm->DialogResult == DialogResult::Cancel)
		infoTarjetas.Clear();
	TjStatic::tForm = nullptr;
	ScannerRedirectedFunc = NULL; 

#endif

	for(int i=0; i<infoTarjetas.Count(); )
	{
		TarjInfo *info = (TarjInfo *)infoTarjetas.Item(i);
		if(!info->DatosValidos())
			infoTarjetas.RemoveAt(i);
		else
			i++;
	}
	return TotalTarjetasOnline(false);
}

// Calcula el total pagado en tarjetas online
double TotalTarjetasOnline(bool soloRecargo)
{
	BDecimal tRecargo, tMonto, res = 0;
	for(int i = infoTarjetas.Count() - 1; i >= 0; i--)
	{
		TarjInfo *t = (TarjInfo *)infoTarjetas.Item(i);
		tRecargo = t->Recargo;
		tRecargo.ApplyRound(2);
		tMonto = t->Monto;
		tMonto.ApplyRound(2);
		res += soloRecargo? tRecargo: tMonto;
	}
	return res.ToDouble();
}

// Busca la definicion de la tarjeta que coincida con el numero de tarjeta que se le pasa.
TarjDef *BuscarDef(char *num)
{
	/*System::String ^filename = "rangos.txt";
	System::IO::StreamWriter ^writer = System::IO::File::AppendText(filename);
	for(int i = rangos.Count() - 1; i >= 0; i--)
	{
		TarjRango *rango = (TarjRango *)rangos.Item(i);
		writer->WriteLine(System::String::Format("{0}\t\t{1}\t{2}", rango->Definicion->Codigo, gcnew String(rango->Desde), gcnew String(rango->Hasta)));		
	}
	writer->Close();*/

	int numLen = strlen(num);
	for(int i = rangos.Count() - 1; i >= 0; i--)
	{
		TarjRango *rango = (TarjRango *)rangos.Item(i);
		if(numLen >= rango->LenDesde && numLen >= rango->LenHasta)
		{
			if(
				memcmp(num, rango->Desde, rango->LenDesde) >= 0 &&
				memcmp(num, rango->Hasta, rango->LenHasta) <= 0)
			{
				return rango->Definicion;
			}
		}
	}
	return NULL;
}


TarjDef *BuscarTipre(char *nombret)
{
	/*System::String ^filename = "rangos.txt";
	System::IO::StreamWriter ^writer = System::IO::File::AppendText(filename);
	for(int i = rangos.Count() - 1; i >= 0; i--)
	{
		TarjRango *rango = (TarjRango *)rangos.Item(i);
		writer->WriteLine(System::String::Format("{0}\t\t{1}\t{2}", rango->Definicion->Codigo, gcnew String(rango->Desde), gcnew String(rango->Hasta)));		
	}
	writer->Close();*/

	//int numLen = strlen(num);
	for(int i = defTarjetas.Count() - 1; i >= 0; i--)
	{
		TarjDef *rango = (TarjDef *)defTarjetas.Item(i);
		if(strcmp(nombret, rango->TipreNom) == 0)
		{			
			return rango;			
		}
	}
	return NULL;
}

///Agregado 10/11/2011
///Devuelve el codigo de la tarjeta si lo encuentra, busca entre los rangos
int	 BuscarTarjeta(char *num)
{
	int numLen = 6;//strlen(num);	El BIN por defecto son los primeros 6 digitos
	int codtarjeta = 0;
	/*if (tjOnline)
	{*/
		for(int i = rangos.Count() - 1; i >= 0; i--)
		{
			TarjRango *rango = (TarjRango *)rangos.Item(i);
			if(numLen >= rango->LenDesde && numLen >= rango->LenHasta)
			{
				if(
					memcmp(num, rango->Desde, numLen) >= 0 &&
					memcmp(num, rango->Hasta, numLen) <= 0)
				{
					codtarjeta = rango->Definicion->Codigo;
					break;
				}
			}
		}
	//}
	return codtarjeta;
}


TarjCuota *TarjInfo::DatosCuota()
{
	if(Definicion == NULL) return NULL;
	for(int i=0; i < Definicion->Cuotas->Count(); i++)
	{
		TarjCuota *tc = (TarjCuota *) Definicion->Cuotas->Item(i);
		if(Cuotas == tc->Cuota) return tc;
	}
	return NULL;
}

// Implementaciones de TarjInfo...
char *TarjInfo::NombreCuota()
{
	if(Definicion == NULL) return "";
	for(int i=0; i < Definicion->Cuotas->Count(); i++)
	{
		TarjCuota *tc = (TarjCuota *) Definicion->Cuotas->Item(i);
		if(Cuotas == tc->Cuota)
		{
			if(strlen(tc->Nombre) != 0)
				sprintf(cuotaNom, "%d=%s (%.2lf%%)", Cuotas, tc->Nombre, tc->Porcen);
			else
				sprintf(cuotaNom, "%d Pago%s (%.2lf%%)", Cuotas, ((Cuotas == 1)? "": "s"), tc->Porcen);
			
			return cuotaNom;
		}
	}
	return "";
}

char *TarjInfo::CuotasFormateadas()
{
	char temp[20];
	cuotasFormateadas[0] = 0;
	bool listo = false;

	if(Definicion == NULL) return "";
	int dRango = 0, hRango = 0;
	for(int i=0; i < Definicion->Cuotas->Count(); i++)
	{
		TarjCuota *tc = (TarjCuota *) Definicion->Cuotas->Item(i);

		bool flushear = hRango != 0 && tc->Cuota != hRango;
		if(!flushear && (strlen(tc->Nombre) != 0))
			flushear = true;

		if(flushear)
		{
			// Determina el mensaje a agregar
			if(hRango != 0)
				sprintf(temp, "%d a %d", dRango, hRango - 1);
			else
				sprintf(temp, "%d", dRango);

			// Agrega la parte al mensaje en cuestion
			if(!listo && (dRango != 0))
			{
				if(cuotasFormateadas[0]) STRCAT(cuotasFormateadas, ", ");
				if(strlen(cuotasFormateadas) + strlen(temp) > 194)
				{
					STRCAT(cuotasFormateadas, "...");
					listo = true;
				}
				else
					STRCAT(cuotasFormateadas, temp);
			}

			// Libera las variables usadas
			dRango = hRango = 0;
		}
		
		// Ahora, agrega el item en cuestion.
		if((strlen(tc->Nombre) != 0) && !listo)
		{
			sprintf(temp, "%d=%s", tc->Cuota, tc->Nombre);
			if(cuotasFormateadas[0]) STRCAT(cuotasFormateadas, ", ");
			if(strlen(cuotasFormateadas) + strlen(temp) > 194)
			{
				STRCAT(cuotasFormateadas, "...");
				listo = true;
			}
			else
				STRCAT(cuotasFormateadas, temp);
		}
		else
		{
			if(dRango == 0) dRango = tc->Cuota; else hRango = tc->Cuota + 1;
		}
	}

	// Agrega el ultimo item
	if(!listo && ((dRango != 0) || (hRango != 0)))
	{
		// Determina el mensaje a agregar
		if(hRango != 0)
			sprintf(temp, "%d a %d", dRango, hRango - 1);
		else
			sprintf(temp, "%d", dRango);

		// Agrega la parte al mensaje en cuestion
		if(cuotasFormateadas[0]) STRCAT(cuotasFormateadas, ", ");
		if(strlen(cuotasFormateadas) + strlen(temp) > 194)
			STRCAT(cuotasFormateadas, "...");
		else
			STRCAT(cuotasFormateadas, temp);
	}

	return cuotasFormateadas;
}

bool TarjInfo::TarjetaValida()
{
	return strlen(Tarjeta) == 16;
}

bool TarjInfo::VencimValido()
{
	if(this->Vencim == 0) return false;

	// Obtiene el año y mes minimos.
	int minFecha = date() / 100;
	int minMes = minFecha % 100;
	int minAio = (minFecha / 100) - 2000;

	// Formatea como corresponde.
	int mes = Vencim / 100, aio = Vencim % 100;
	if(mes == 0 && aio == 0)
		return false;
	else if((mes < 1 || mes > 12) || (aio < 8 && aio > 20))
		return false;
	else if(aio < minAio || (aio == minAio && mes < minMes))
		return false;
	return true;
}

bool TarjInfo::MontoValido()
{
	return Monto > 0;
}

bool TarjInfo::CuotasValidas()
{
	char *cuoNom = this->NombreCuota();
	return strlen(cuoNom) != 0;
}

bool TarjInfo::DniValido()
{
	return Dni > 99999 && Dni < 99999999;
}

bool TarjInfo::CvvValido()
{
	if(Definicion != NULL && !Definicion->PideCvv) return true;
	if (Definicion != NULL && Definicion->CvvCero) return true;
	return (Cvv > 0 && Cvv < 1000);
}



// ******  Proceso Nativo de INVEL *********************************************************
void CierreOnline()
{
#ifdef POSXP
	char cieMsg[100];
	if(!tjOnline) return;
	for(int i=0; i < nodos->Count(); i++)
	{
		NodoOnline *nodo = (NodoOnline *)nodos->Item(i);
		if(nodo->activo)
		{
			sprintf(cieMsg, "Nodo: '%s' (%d) - Comercio: '%s'", nodo->nombre, nodo->id, nodo->cierre);
			SendMsg("Cerrando Lotes de Operaciones Online", cieMsg);

			// Efectua el proceso del paquete.
			bool res = GCStatic::X25Client->CierreLote(caja + offset, nodo->id);
			sprintf(cieMsg, "Cierre Online - Nodo '%s' (%d -> %d) - Resultado: %s",
				nodo->nombre,
				nodo->id,
				nodo->terminal - 1 + caja /*+ offset*/,
				res? "Ok": "Fallo");
			WriteBootLog(cieMsg);
			if(!res)
			{
				Strings::StringToChar(GCStatic::X25Client->ResultMessage, cieMsg, 98);
				WriteBootLog(cieMsg);
			}
			ClearMsg();
		}
	}
#endif
}

bool InvelOnline(TarjInfo *info)
{
#ifdef POSXP
	// Codigo inicial: fallo
	OLResult.Codigo = -1;	
	STRCPY(OLResult.Mensaje, "Fallo General");

	// Verifica que el procesador este listo.
	if(!GCStatic::X25Client->Valido)
	{
		STRCPY(OLResult.Mensaje, "El sistema ONLINE no esta bien configurado");
		return false;
	}

	// Determina el nodo a usar.
	NodoOnline *nodo = NULL;
	for(int i=0; i<nodos->Count(); i++)
	{
		NodoOnline *ver = (NodoOnline *)nodos->Item(i);
		if(ver->id == info->Definicion->Nodo)
		{
			nodo = ver;
			break;
		}
	}

	// Valida el nodo
	if(nodo == NULL)
	{
		sprintf(OLResult.Mensaje, "El nodo %d no esta definido", info->Definicion->Nodo);
		return false;
	}
	if(!nodo->activo)
	{
		sprintf(OLResult.Mensaje, "El nodo %s (%d) no esta activo", nodo->nombre, info->Definicion->Nodo);
		return false;
	}

	try
	{

		// Arma el request usado por la interface online.
		X25Request ^req = gcnew X25Request();

		req->nro_cta_tarjeta = Strings::CharToString(info->Tarjeta);
		req->track_1 = (info->Definicion->EnvTrack1) ? Strings::CharToString(info->Track_1) : "";
		req->track_2 = Strings::CharToString(info->DatosBanda);
		req->fecha_expiracion = info->Vencim.ToString()->PadLeft(4, '0');
		req->marca_id = info->Definicion->CodNumera;
		req->numero_de_comercio = Strings::CharToString(info->Definicion->NumComercio);
		req->nodo_id = info->Definicion->Nodo;
		req->numero_de_terminal = nodo->terminal -1 + caja;// + offset;
		req->terminal_id = caja + offset;
		if(info->Definicion->PideCvv) req->cvv = info->Cvv;
		
		req->ticket = (short)(ticket + 1);
		req->tipo_de_mensaje = 200;
		req->bloque_version = 1;
		req->bloque_id_clave = 1;
		req->fecha = DateTime::Now;
		req->fecha_local = DateTime::Now;
		req->hora_local = DateTime::Now;

		req->modo_de_ingreso = info->UsoLector? 1: 0;
		req->importe = (float)info->Monto;
		req->cuotas = info->Cuotas;
		req->plan = info->Plan; //'0';

		// Efectua el proceso del paquete.
		bool res = GCStatic::X25Client->ProcessRequest(req);
		if(!res || !GCStatic::X25Client->ResultAprobada)
		{
			Strings::StringToChar(GCStatic::X25Client->ResultMessage, OLResult.Mensaje, 190);
			WriteBootLog("Tarjetas, no se autorizo la tarjeta.");
			WriteBootLog(OLResult.Mensaje);
			return false;
		}

		// Copia los datos que interesan.
		OLResult.Codigo = 0;
		OLResult.Cupon = req->autorizacion;
		OLResult.Lote = req->batch;
		OLResult.Terminal = req->numero_de_terminal;
		OLResult.Ticket = req->ticket;
		return true;
	}
	catch (System::Exception ^ex)
	{
		WriteBootLog("AVISO - Excepcion al procesar tarjeta online.");
		return false;
	}
#else
	return false;
#endif
}

//Funciones Implementadas de Tipre


double		TotalTarjetasPrisma(bool soloRecargo)
{
	System::Decimal tRecargo = 0, tMonto = 0, res = 0;
	desctoTipre = 0;
	for (int k = 0; k < Dump::TarjPrisma->Count; k++)
	{
		TransVerifone ^tmpPrisma = Dump::TarjPrisma[k];
		tMonto = tmpPrisma->Importe;
		tRecargo = tmpPrisma->ImporteRecargo;
		desctoTipre += Convert::ToDouble(tmpPrisma->ImporteDescuento);
		desctoTipre += Convert::ToDouble(tmpPrisma->AlbMontoDescto);
		res += soloRecargo ? tRecargo : tMonto;
	}
	return System::Convert::ToDouble(res);
}

double		TotalTarjetasPrismaECR(bool soloRecargo)
{
	System::Decimal tRecargo = 0, tMonto = 0, res = 0;
	desctoTipre = 0;
	for (int k = 0; k < Dump::TarjPrismaECR->Count; k++)
	{
		TransPrismaECR ^tmpPrisma = Dump::TarjPrismaECR[k];
		tMonto = tmpPrisma->Importe;
		tRecargo = tmpPrisma->ImporteRecargo;
		desctoTipre += Convert::ToDouble(tmpPrisma->ImporteDescuento);
		desctoTipre += Convert::ToDouble(tmpPrisma->AlbMontoDescto);
		res += soloRecargo ? tRecargo : tMonto;
	}
	return System::Convert::ToDouble(res);
}


void AnularTarjetaPrisma(int cual)
{	
	char *msgrta = new char[60];
	Dump::TarjPrisma->Clear();

	frmAnulaPrisma ^frmIng = gcnew frmAnulaPrisma(cual);	

	if (frmIng->ShowDialog() == System::Windows::Forms::DialogResult::OK)			
	{			
		if (Dump::TarjPrisma->Count > 0)//significa que se ingresaron los datos de la banda
		{	
			TransVerifone ^unReq = Dump::TarjPrisma[0];
			if (unReq->TipoVerifone == 1) {
				TrxPrisma^ trx = gcnew TrxPrisma();
				RtaVenta^ rta = gcnew RtaVenta();
				rta->NumeroLote = unReq->NroLote;
				rta->NumeroCupon = unReq->NroCupon;
				rta->CodigoAutorizacion = unReq->NroAutorizacion;
				SendMsg("Buscando para Anular", "");
				OperarTrxPrisma^ hacer = gcnew OperarTrxPrisma(COM_NLD);
				hacer->HacerDevolAnul(trx, rta);
				ClearMsg();
				if (!hacer->ConError)
				{
					if (rta->CodigoRespuesta == 0)
					{
						Alert("Se Anulo la operacion", "");
						GrabarAnulacionPrisma(0);
					}
					else
					{
						Strings::StringToChar(rta->MensajeRespuesta, msgrta, 60);
						Alert("No se pudo Anular", msgrta);
					}
				}
				delete hacer;
			}
			else if (unReq->TipoVerifone == 2)
			{
				/*SendMsg("Buscando para Anular", "");
				OperarFiserv^ phacer = gcnew OperarFiserv();
				phacer->HacerDevolAnul(unReq);
				ClearMsg();
				if (!phacer->ConError) {
					if (unReq->CodigoRespuesta == "0") {
						Alert("Se Anulo la operacion", "");
						GrabarAnulacionPrisma(0);
					}
					else {
						Strings::StringToChar(unReq->MensajeRespuesta, msgrta, 60);
						Alert("No se pudo Anular", msgrta);
					}
				}
				delete phacer;*/
			}
		}				
	}

	delete frmIng;
	delete msgrta;
	Dump::TarjPrisma->Clear();
}

void AnularTarjetaPrismaEcr()
{	
	char *msgrta = new char[60];
	Dump::TarjPrismaECR->Clear();

	frmAnulaPrismaECR ^frmIng = gcnew frmAnulaPrismaECR();	

	if (frmIng->ShowDialog() == System::Windows::Forms::DialogResult::OK)			
	{			
		if (Dump::TarjPrismaECR->Count > 0)//significa que se ingresaron los datos de la banda
		{	
			TransPrismaECR ^unReq = Dump::TarjPrismaECR[0];
			if (unReq->TipoVerifone == 3) {
				String ^claveBusq = System::String::Format("{0:000}{1:0000}{2}", unReq->NroLote, unReq->NroCupon, unReq->NroAutorizacion->ToUpper()->PadLeft(6, '0'));				
				ClearMsg();
				MsgAlertPrismaECR ^hacer = gcnew MsgAlertPrismaECR();
				hacer->ShowMsgProcesaAnulacion("Procesando Anulación", "...", claveBusq);
				PosAnulacionResponse ^rtaAnul = hacer->RespuestaAnulacion;				
				if (!hacer->ConError)
				{
					if (rtaAnul->Estado == "ANULADA")
					{
						Alert("Se Anulo la operacion", "");
						GrabarAnulacionPrismaEcr(0);
					}
					else if (rtaAnul->Estado == "ERROR")
					{
						// PrismaECR no pudo confirmar el resultado con Payway: no se sabe si se anulo
						Alert("No se sabe si se anulo", "Verifiquelo antes de reintentar");
					}
					else
					{
						if (rtaAnul->Mensaje != nullptr){
							Strings::StringToChar(rtaAnul->Mensaje, msgrta, 60);
							Alert("No se pudo Anular", msgrta);
						}else
							Alert("No se pudo Anular", "");
					}
				}
				else{
					Strings::StringToChar(hacer->MensajeError, msgrta, 60);
					Alert("No se pudo Anular", msgrta);
				}
				delete hacer;
			}			
		}				
	}

	delete frmIng;
	delete msgrta;
	Dump::TarjPrismaECR->Clear();
}


void AnularTarjetaSmartPoint(int cual)
{	
	char *msgrta = new char[60];
	Dump::TarjPrisma->Clear();

	frmAnulaSmartPoint ^frmIng = gcnew frmAnulaSmartPoint(cual);	

	if (frmIng->ShowDialog() == System::Windows::Forms::DialogResult::OK)			
	{
		if (frmIng->ExternalReference != String::Empty){
			MsgAlertSmartPointMp ^formMsg = gcnew MsgAlertSmartPointMp();
			formMsg->ShowMsgDevuelveVenta("Procesando devolucion...", "Orden: " + frmIng->ExternalReference, frmIng->ExternalReference);
			RtaOrdenCaja ^respuesta = formMsg->RespuestaVenta;
			ClearMsg();
			if (respuesta != nullptr){
				Alert("Se anulo la orden de MercadoPago", "");
			}
			else
				Alert("No se pudo anular la orden de MercadoPago", "");

		}
		ClearMsg();
		//if (Dump::TarjPrisma->Count > 0)//significa que se ingresaron los datos de la banda
		//{	
		//	TransVerifone ^unReq = Dump::TarjPrisma[0];
		//	if (unReq->TipoVerifone == 1) {
		//		TrxPrisma^ trx = gcnew TrxPrisma();
		//		RtaVenta^ rta = gcnew RtaVenta();
		//		rta->NumeroLote = unReq->NroLote;
		//		rta->NumeroCupon = unReq->NroCupon;
		//		rta->CodigoAutorizacion = unReq->NroAutorizacion;
		//		SendMsg("Buscando para Anular", "");
		//		OperarTrxPrisma^ hacer = gcnew OperarTrxPrisma(COM_NLD);
		//		hacer->HacerDevolAnul(trx, rta);
		//		ClearMsg();
		//		if (!hacer->ConError)
		//		{
		//			if (rta->CodigoRespuesta == 0)
		//			{
		//				Alert("Se Anulo la operacion", "");
		//				GrabarAnulacionPrisma(0);
		//			}
		//			else
		//			{
		//				Strings::StringToChar(rta->MensajeRespuesta, msgrta, 60);
		//				Alert("No se pudo Anular", msgrta);
		//			}
		//		}
		//		delete hacer;
		//	}			
		//}				
	}

	delete frmIng;
	delete msgrta;
	Dump::TarjPrisma->Clear();
}

void CierreLotePrismaEcr(){
	PosCierreResponse ^mrta = nullptr;
	MsgAlertPrismaECR ^otroMsg = gcnew MsgAlertPrismaECR();
	otroMsg->ShowMsgProcesaCierre("Realizando Cierre de Lotes", "Aguarde un instante", suc, caja);
	mrta = otroMsg->RespuestaCierre;
	if (!otroMsg->ConError){

	}
}

int			GrabarAnulacionPrisma(int cual)
{
	//  Copia datos de targetas online, si corresponden.
	struct DDanul_ *fanul = new struct DDanul_();	
	TransVerifone ^acttipre = Dump::TarjPrisma[0];
	

	bool esOnline = false;
	char *cl_nombre = new char[200];
	char *cl_dni = new char[50];
	char *cl_direcc = new char[200];
	char *cl_telef = new char[200];
	char *cl_barrio = new char[200];
	char *cl_condiva = new char[50];
	char *cl_fecha = new char[50];	
	char *fmtCard = new char[300];	

	fmtCard[0] = 0;		
		
	/*char *numtarje = new char[30];
	Strings::StringToChar(acttipre->Nro_Tarjeta, numtarje, 20);
	char *ntarj = new char[7];
	for(int k = 0, ik = 5; ik >= 0; k++, ik--)
		ntarj[k] = numtarje[ik];		 
	ntarj[6] = 0;*/
	Strings::StringToChar(acttipre->ParaTrans(), fmtCard, 100);
	sprintf(fanul->dtarjeta, "%02d%s",acttipre->AlbNroTarjeta, "000000");		 		 
	//delete [] numtarje, delete [] ntarj;		

	//  Prepara los datos adicionales a grabar, si existen
	if(fmtCard[0] != 0)
	{
		if(SplitParam2(fmtCard, cl_fecha, 11))
			if(SplitParam2(fmtCard, cl_nombre, 30))
				if(SplitParam2(fmtCard, cl_dni, 13))
					if(SplitParam2(fmtCard, cl_telef, 12))
						if(SplitParam2(fmtCard, cl_direcc, 30))
							SplitParam2(fmtCard, cl_barrio, 30);
	}

	//  Graba los Resultados.
	fanul->func = DAnul;	
	fanul->nro = acttipre->AlbNroMpago;
	
	fanul->_fecha = (long) date();
	fanul->_hora = (long) time_();
	fanul->cajero = ckaNro;
	store(Convert::ToDouble(acttipre->Importe), fanul->importe);
	Trim(c_numero); Trim(c_fecha); Trim(c_nombre); Trim(c_dni);
	Trim(c_telef);  Trim(c_direcc);
	if(strlen(c_numero) > 13) c_numero[13] = 0;
	if(strlen(cl_fecha) > 11) cl_fecha[11] = 0;
	if(strlen(cl_nombre) > 30) cl_nombre[30] = 0;
	if(strlen(cl_dni) > 13) cl_dni[13] = 0;
	if(strlen(cl_telef) > 12) cl_telef[12] = 0;
	if(strlen(cl_direcc) > 30) cl_direcc[30] = 0;
	if(strlen(cl_barrio) > 30) cl_barrio[30] = 0;
	if(strlen(fmtCard) > 99) fmtCard[99] = 0;

	STRCPY(fanul->telef, cl_telef);
	STRCPY(fanul->direc, cl_direcc);
	STRCPY(fanul->barrio, cl_barrio);
	STRCPY(fanul->fecha, cl_fecha);
	STRCPY(fanul->nombre, cl_nombre);
	STRCPY(fanul->dni, cl_dni);
	WriteDump(fanul);

	delete [] cl_nombre; delete [] cl_dni; delete [] cl_direcc; delete [] cl_telef;
	delete [] cl_barrio; delete [] cl_condiva; delete [] cl_fecha;
	delete fanul;
	return 0;
}


int			GrabarAnulacionPrismaEcr(int cual)
{
	//  Copia datos de targetas online, si corresponden.
	struct DDanul_ *fanul = new struct DDanul_();	
	TransPrismaECR ^acttipre = Dump::TarjPrismaECR[0];
	

	bool esOnline = false;
	char *cl_nombre = new char[200];
	char *cl_dni = new char[50];
	char *cl_direcc = new char[200];
	char *cl_telef = new char[200];
	char *cl_barrio = new char[200];
	char *cl_condiva = new char[50];
	char *cl_fecha = new char[50];	
	char *fmtCard = new char[300];	

	fmtCard[0] = 0;		
		
	/*char *numtarje = new char[30];
	Strings::StringToChar(acttipre->Nro_Tarjeta, numtarje, 20);
	char *ntarj = new char[7];
	for(int k = 0, ik = 5; ik >= 0; k++, ik--)
		ntarj[k] = numtarje[ik];		 
	ntarj[6] = 0;*/
	Strings::StringToChar(acttipre->ParaTrans(), fmtCard, 100);
	sprintf(fanul->dtarjeta, "%02d%s",acttipre->AlbNroTarjeta, "000000");		 		 
	//delete [] numtarje, delete [] ntarj;		

	//  Prepara los datos adicionales a grabar, si existen
	if(fmtCard[0] != 0)
	{
		if(SplitParam2(fmtCard, cl_fecha, 11))
			if(SplitParam2(fmtCard, cl_nombre, 30))
				if(SplitParam2(fmtCard, cl_dni, 13))
					if(SplitParam2(fmtCard, cl_telef, 12))
						if(SplitParam2(fmtCard, cl_direcc, 30))
							SplitParam2(fmtCard, cl_barrio, 30);
	}

	//  Graba los Resultados.
	fanul->func = DAnul;	
	fanul->nro = acttipre->AlbNroMpago;
	
	fanul->_fecha = (long) date();
	fanul->_hora = (long) time_();
	fanul->cajero = ckaNro;
	store(Convert::ToDouble(acttipre->Importe), fanul->importe);
	Trim(c_numero); Trim(c_fecha); Trim(c_nombre); Trim(c_dni);
	Trim(c_telef);  Trim(c_direcc);
	if(strlen(c_numero) > 13) c_numero[13] = 0;
	if(strlen(cl_fecha) > 11) cl_fecha[11] = 0;
	if(strlen(cl_nombre) > 30) cl_nombre[30] = 0;
	if(strlen(cl_dni) > 13) cl_dni[13] = 0;
	if(strlen(cl_telef) > 12) cl_telef[12] = 0;
	if(strlen(cl_direcc) > 30) cl_direcc[30] = 0;
	if(strlen(cl_barrio) > 30) cl_barrio[30] = 0;
	if(strlen(fmtCard) > 99) fmtCard[99] = 0;

	STRCPY(fanul->telef, cl_telef);
	STRCPY(fanul->direc, cl_direcc);
	STRCPY(fanul->barrio, cl_barrio);
	STRCPY(fanul->fecha, cl_fecha);
	STRCPY(fanul->nombre, cl_nombre);
	STRCPY(fanul->dni, cl_dni);
	WriteDump(fanul);

	delete [] cl_nombre; delete [] cl_dni; delete [] cl_direcc; delete [] cl_telef;
	delete [] cl_barrio; delete [] cl_condiva; delete [] cl_fecha;
	delete fanul;
	return 0;
}


// ----------------------------------------------------------------------------------------------
// Clover (CloverECR). Mismo esquema que PrismaECR: el POS habla con la WebAPI CloverECR, que habla
// con Clover y con el Flex. No hay cierre de lote (Clover lo cierra solo): se consulta el resumen del dia.

double		TotalTarjetasCloverECR(bool soloRecargo)
{
	System::Decimal tRecargo = 0, tMonto = 0, res = 0;
	// Cada TotalTarjetas* pisa desctoTipre: aca solo se toca si hay cupones Clover, para no borrar el de los otros
	if (Dump::TarjCloverECR->Count > 0)
		desctoTipre = 0;
	for (int k = 0; k < Dump::TarjCloverECR->Count; k++)
	{
		TransCloverECR ^tmpClover = Dump::TarjCloverECR[k];
		tMonto = tmpClover->Importe;
		tRecargo = tmpClover->ImporteRecargo;
		desctoTipre += Convert::ToDouble(tmpClover->ImporteDescuento);
		desctoTipre += Convert::ToDouble(tmpClover->AlbMontoDescto);
		res += soloRecargo ? tRecargo : tMonto;
	}
	return System::Convert::ToDouble(res);
}

// Alert con un mensaje de Clover (String) en la segunda linea
static void AlertClover(char *titulo, String ^mensaje)
{
	char *linea = new char[61];
	linea[0] = 0;
	if (mensaje != nullptr)
		Strings::StringToChar(mensaje, linea, 60);
	Alert(titulo, linea);
	delete [] linea;
}

// Medio de pago y tarjeta del POS para una marca de Clover (VI, MC, ...; RQ = QR). Es la misma tabla
// de tarjetas que usa PrismaECR (CodTarjPrisma = "0VI", "QRQ", ...).
static void BuscaTarjetaClover(String ^marca, String ^medioPago, int &mpago, int &codtarj)
{
	mpago = codtarj = 0;
	if (!tjOnline && defTarjetas.Count() <= 0)
		CargaTjOnline();
	String ^buscar = marca;
	if (String::IsNullOrEmpty(buscar))
		buscar = (medioPago == "QR") ? gcnew String("RQ") : String::Empty;
	if (buscar->Length != 2)
		return;
	char cod[3];
	Strings::StringToChar(buscar, cod, 2);
	for (int k = 0; k < defTarjetas.Count(); k++)
	{
		TarjDef *def = (TarjDef *) defTarjetas.Item(k);
		if (def->CodTarjPrisma != NULL && def->CodTarjPrisma[0] != 0 && strncmp(def->CodTarjPrisma + 1, cod, 2) == 0)
		{
			mpago = def->MPago;
			codtarj = def->Codigo;
			return;
		}
	}
}

// Graba la anulacion/devolucion en el trans, como GrabarAnulacionPrismaEcr, pero con el importe revertido
// y el medio de pago de la tarjeta de la venta.
static int	GrabarAnulacionCloverEcr(VentaParaReversion ^venta, EstadoOperacion ^rta)
{
	struct DDanul_ *fanul = new struct DDanul_();
	int mpago = 0, codtarj = 0;
	BuscaTarjetaClover(venta->MarcaTarjeta, venta->MedioPago, mpago, codtarj);
	// Importe revertido: el de la devolucion, o el de la venta si fue una anulacion
	System::Decimal importe = (rta->Monto > 0) ? rta->Monto : venta->Monto;

	fanul->func = DAnul;
	fanul->nro = mpago;
	sprintf(fanul->dtarjeta, "%02d%s", codtarj, "000000");
	fanul->_fecha = (long) date();
	fanul->_hora = (long) time_();
	fanul->cajero = ckaNro;
	store(Convert::ToDouble(importe), fanul->importe);
	WriteDump(fanul);

	CloverClient::EscribeLog(rta->TipoOperacion + " grabada en el trans: clave=" + venta->ClaveBusq + " importe=" + importe.ToString("F2") +
		" mpago=" + mpago.ToString() + " tarjeta=" + codtarj.ToString() + " estadoVenta=" + rta->EstadoVenta, "CAJA");
	delete fanul;
	return 0;
}

// Anulacion (modo 0), devolucion parcial o total (modo 1) o reimpresion del recibo (modo 2) de una venta Clover.
// La venta se busca por lote + cupon + autorizacion en toda la sucursal; la operacion se hace en el Flex de esta caja.
void AnularTarjetaCloverEcr(int modo)
{
	frmAnulaCloverECR ^frmIng = gcnew frmAnulaCloverECR(modo);
	bool aceptado = (frmIng->ShowDialog() == System::Windows::Forms::DialogResult::OK);
	String ^claveBusq = frmIng->ClaveBusq;
	System::Decimal montoPedido = frmIng->Monto;
	delete frmIng;
	if (!aceptado || String::IsNullOrEmpty(claveBusq))
		return;

	MsgAlertCloverECR ^busca = gcnew MsgAlertCloverECR();
	busca->ShowMsgBuscaVenta("Buscando la venta Clover", claveBusq, claveBusq);
	VentaParaReversion ^venta = busca->RespuestaBusqueda;
	if (busca->ConError || venta == nullptr)
	{
		AlertClover("No se pudo buscar la venta", busca->MensajeError);
		return;
	}

	if (modo == 2)
	{
		// La reimpresion vale tambien para una venta ya anulada o devuelta (409)
		if ((venta->HttpStatus != 200 && venta->HttpStatus != 409) || String::IsNullOrEmpty(venta->IdOperacion))
		{
			AlertClover("No se encontro la venta", venta->Mensaje);
			return;
		}
		MsgAlertCloverECR ^reimp = gcnew MsgAlertCloverECR();
		reimp->ShowMsgReimprimir("Reimprimiendo el recibo", "en el Flex de esta caja", venta->IdOperacion);
		RespuestaPos ^r = reimp->RespuestaReimpresion;
		if (reimp->ConError || r == nullptr)
			AlertClover("No se pudo reimprimir", reimp->MensajeError);
		else if (r->Aceptada)
			Alert("Reimpresion enviada al Flex", "");
		else
			AlertClover("No se pudo reimprimir", r->Mensaje);
		return;
	}

	// 409 = ya ANULADA o DEVUELTA, 404 = no existe en la sucursal
	if (venta->HttpStatus != 200)
	{
		AlertClover("La venta no se puede revertir", venta->Mensaje);
		return;
	}
	if (modo == 0 && venta->LoteCerrado)
	{
		Alert("El lote de la venta ya cerro", "Solo se puede hacer una DEVOLUCION");
		return;
	}
	if (modo == 0 && venta->MontoDevuelto > 0)
	{
		Alert("La venta tiene devoluciones", "Solo se puede DEVOLVER el resto");
		return;
	}
	if (modo == 1 && montoPedido > venta->MontoDisponible)
	{
		AlertClover("El monto supera lo disponible", "Disponible $ " + venta->MontoDisponible.ToString("F2"));
		return;
	}

	// Confirmacion con los datos de la venta encontrada
	System::Decimal aRevertir = (modo == 1 && montoPedido > 0) ? montoPedido : venta->MontoDisponible;
	String ^s1 = String::Format("{0} {1} ****{2} $ {3}", venta->MedioPago, venta->MarcaTarjeta, venta->Ult4Digitos, venta->Monto.ToString("F2"));
	String ^s2 = gcnew String((modo == 1) ? "Devolver $ " : "Anular $ ");
	s2 = s2 + aRevertir.ToString("F2") + ". Confirma?";
	char *l1 = new char[61];
	char *l2 = new char[61];
	Strings::StringToChar(s1, l1, 60);
	Strings::StringToChar(s2, l2, 60);
	int confirma = GetSiNo(l1, l2);
	delete [] l1;
	delete [] l2;
	if (!confirma)
		return;

	String ^cajero = System::Convert::ToString(ckaNro);
	MsgAlertCloverECR ^hacer = gcnew MsgAlertCloverECR();
	if (modo == 1)
	{
		// Sin monto = todo lo disponible. Total con el lote abierto: CloverECR la hace como anulacion
		System::Nullable<System::Decimal> monto;
		if (montoPedido > 0)
			monto = System::Nullable<System::Decimal>(montoPedido);
		hacer->ShowMsgProcesaDevolucion("Procesando Devolucion Clover", "Siga las indicaciones del Flex", claveBusq, monto, cajero);
	}
	else
		hacer->ShowMsgProcesaAnulacion("Procesando Anulacion Clover", "Siga las indicaciones del Flex", claveBusq, cajero);

	EstadoOperacion ^rta = hacer->RespuestaOperacion;
	if (hacer->ConError || rta == nullptr)
		AlertClover("No se pudo completar la operacion", hacer->MensajeError);
	else if (rta->Estado == Estados::Aprobada)
	{
		if (rta->TipoOperacion == "DEVOLUCION")
			Alert("Se realizo la devolucion", "");
		else
			Alert("Se anulo la operacion", "");
		GrabarAnulacionCloverEcr(venta, rta);
	}
	else if (!Estados::EsFinal(rta->Estado))
		Alert("La operacion sigue sin resultado", "Verifiquela en el backoffice antes de reintentar");
	else
		AlertClover("No se pudo completar la operacion", rta->Mensaje);
}

// Totales del dia de la caja en Clover (reemplaza al cierre de lotes de PrismaECR)
void ResumenCloverEcr()
{
	MsgAlertCloverECR ^consulta = gcnew MsgAlertCloverECR();
	consulta->ShowMsgResumen("Consultando los totales Clover", "del dia", System::DateTime::Today);
	Alberdi::TJOCommon::CloverECR::Resumen ^r = consulta->RespuestaResumen;
	if (consulta->ConError || r == nullptr)
	{
		AlertClover("No se pudo leer el resumen", consulta->MensajeError);
		return;
	}
	if (r->HttpStatus != 200)
	{
		AlertClover("No se pudo leer el resumen", r->Mensaje);
		return;
	}

	System::Text::StringBuilder ^sb = gcnew System::Text::StringBuilder();
	sb->AppendLine(String::Format("Sucursal {0} - Caja {1}", r->NumeroSucursal, r->NumeroCaja));
	if (r->Fuente == "CLOVERECR")
		sb->AppendLine("ATENCION: Clover no respondio, totales calculados con lo registrado en CloverECR.");
	sb->AppendLine();
	if (r->Grupos != nullptr)
	{
		for each (Alberdi::TJOCommon::CloverECR::ResumenGrupo ^g in r->Grupos)
			sb->AppendLine(String::Format("Lote {0}  {1} {2}  cant {3}  ventas $ {4}  devol $ {5}  neto $ {6}",
				g->NumeroLote, g->MedioPago, g->Marca, g->Cantidad, g->Ventas.ToString("F2"), g->Devoluciones.ToString("F2"), g->Neto.ToString("F2")));
	}
	if (r->Total != nullptr)
	{
		sb->AppendLine();
		sb->AppendLine(String::Format("TOTAL  cant {0}  ventas $ {1}  devol $ {2}  neto $ {3}",
			r->Total->Cantidad, r->Total->Ventas.ToString("F2"), r->Total->Devoluciones.ToString("F2"), r->Total->Neto.ToString("F2")));
	}
	sb->AppendLine(String::Format("Anulados: {0} por $ {1}", r->CantidadAnulados, r->MontoAnulados.ToString("F2")));
	System::Windows::Forms::MessageBox::Show(sb->ToString(), "Resumen Clover del " + System::DateTime::Today.ToString("dd/MM/yyyy"));
}

double		TotalTarjetasSmartPointMp(bool soloRecargo)
{
	System::Decimal tRecargo = 0, tMonto = 0, res = 0;
	desctoTipre = 0;
	for (int k = 0; k < Dump::TarjSmartPointMp->Count; k++)
	{
		TransSmartPointMp ^tmpSmartPoint = Dump::TarjSmartPointMp[k];
		tMonto = tmpSmartPoint->Importe;
		tRecargo = tmpSmartPoint->ImporteRecargo;
		desctoTipre += Convert::ToDouble(tmpSmartPoint->ImporteDescuento);
		desctoTipre += Convert::ToDouble(tmpSmartPoint->AlbMontoDescto);
		res += soloRecargo ? tRecargo : tMonto;
	}
	return System::Convert::ToDouble(res);
}


double		TotalMpQrCashout(bool soloRecargo)
{
	System::Decimal tRecargo = 0, tMonto = 0, res = 0;
	desctoTipre = 0;
	for (int k = 0; k < Dump::listCashout->Count; k++)
	{
		OrdenCashout ^tmpSmartPoint = Dump::listCashout[k];
		tMonto = tmpSmartPoint->Monto;
		tRecargo = 0;
		desctoTipre += 0;
		desctoTipre += 0;
		res += soloRecargo ? tRecargo : tMonto;
	}
	return System::Convert::ToDouble(res);
}


bool SplitParam2(char *src, char *dst, int lenDst)
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
