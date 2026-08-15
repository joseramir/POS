#include "stdafx.h"
#include "funcs.h"
#include "IniReader.h"
#include <vector>

#ifdef POSXP
#include "StaticDump.h"
#endif

// Tabla de promociones ya aplicadas (con renglon DPromo grabado) en el ticket
// actualmente en curso. Se llena en ProcPromo(), tanto al reprocesar el ticket
// pendiente al iniciar la caja (fwrite=0) como al aplicarse en vivo (fwrite=1).
// Sirve para no recalcular ni reaplicar una promo que ya quedo grabada, sin
// tener que volver a leer trans.dbf.
//
// Detalle por accion. Cada renglon DPromo lleva el indice 1-based de la accion
// que lo genero (campo accionSeq, que viaja en una columna libre del DBF), asi
// un ticket interrumpido por un reinicio se retoma en la accion exacta donde
// quedo, en vez de saltear o repetir la promo entera.
struct AccionAplicadaInfo
{
	int seq;
	double montoAplicado;
	int descarga;
};

struct PromoAplicadaInfo
{
	long long codPromo;
	double montoAplicado;
	int descarga;
	bool aplicado;
	bool legacy;					// Trae renglones sin accionSeq (grabados por la version anterior).
	std::vector<AccionAplicadaInfo> acciones;
};
static std::vector<PromoAplicadaInfo> tablaPromosAplicadas;

// Busca una promo en la tabla de aplicadas. Devuelve NULL si no esta.
static PromoAplicadaInfo *BuscaPromoAplicada(long long codPromo)
{
	for(size_t i=0; i<tablaPromosAplicadas.size(); i++)
	{
		if(tablaPromosAplicadas[i].codPromo == codPromo)
			return &tablaPromosAplicadas[i];
	}
	return NULL;
}

// Devuelve true si la accion 'seq' (1-based) de la promo ya quedo grabada en
// trans.dbf. En un ticket grabado por la version anterior no hay indice de
// accion, asi que se cae al criterio viejo: la promo entera cuenta como hecha
// si llego a grabar su renglon de descuento/entrega (premio==3).
static bool AccionYaAplicada(long long codPromo, int seq)
{
	PromoAplicadaInfo *info = BuscaPromoAplicada(codPromo);
	if(info == NULL) return false;
	if(info->legacy) return info->aplicado;
	for(size_t i=0; i<info->acciones.size(); i++)
	{
		if(info->acciones[i].seq == seq)
			return true;
	}
	return false;
}

// Corte grueso: solo saltea la promo si TODAS sus acciones ya quedaron
// grabadas. Si queda alguna pendiente la promo vuelve a entrar, y el filtrado
// fino accion por accion lo hace PromoDef::ApplyAction().
static bool PromoTotalmenteAplicada(PromoDef *promo)
{
	PromoAplicadaInfo *info = BuscaPromoAplicada(promo->CodPromo);
	if(info == NULL) return false;
	if(info->legacy) return info->aplicado;

	int total = promo->ActionCount();
	if(total <= 0) return info->aplicado;
	for(int i=1; i<=total; i++)
	{
		if(!AccionYaAplicada(promo->CodPromo, i))
			return false;
	}
	return true;
}

// Vacia la tabla. Se llama al empezar un ticket nuevo (ResetPOSAcumInternal).
void LimpiarPromosAplicadas()
{
	tablaPromosAplicadas.clear();
}
::ArrayList *promos = NULL;
PromoDef *actualPromo;			// Promocion actual bajo evaluacion, para el name-resolver
int accionActualSeq = 0;		// Accion (1-based) en ejecucion, para sellar el renglon DPromo.
bool promosCalc = false;		// Para evitar el calculo de las promociones mas de una vez.
bool promosAplicadas = false;	// Para evitar aplicar promociones mas de una vez.
bool forceReloadPromos = false;	// Indica que se deben recargar las promociones antes de usarlas.
dbf canastas;					// Base con la definicion de las canastas disponibles.
bool globalPromos;				// Activacion o desactivacion global de este subsitema.
bool soloSimular;				// Se usa para evaluar las acciones pero no ejecutarlas.
bool promosHechas;				// Evita que, por errores internos, las promos se apliquen mas de una vez en el mismo ticket.
static char *si = "Si";
static char *no = "No";

bool reloadPremioInst = false;  // Para indicar que se recibio una nueva planificacion de premios


// Dummy que redirecciona la resolucion de nombres a la promo actualmente bajo evaluacion.
Value *PromoNameResolver(char *name)
{
	return actualPromo->ResolveName(name);
}

int InitPromos()
{
	// Crea la lista que contendra las definiciones cargadas.
	if(promos == NULL)
		promos = new ::ArrayList(false, "InitPromos");
	return ReloadPromos();
}

// Carga (nuevamente) la lista de promociones disponibles.
int ReloadPromos()
{
	char secc[10], lnum[10], *line;
	PromoDef *pd;
	int i, j;

	if (forceReloadPromos)
	{	
		if (access("pos\\promo.new", 6) == 0)
		{
			remove("pos\\promo.ini");
			rename("pos\\promo.new", "pos\\promo.ini");
			WriteBootLog("Cambio el nombre del promo.new a promo.ini");
		}
		if (access("pos\\canastas.new", 6) == 0)
		{
			remove("pos\\canastas.dbf");
			rename("pos\\canastas.new", "pos\\canastas.dbf");
			WriteBootLog("Cambio el nombre del canastas.new a canastas.dbf");
		}
	}
	forceReloadPromos = false;
	WriteCmos(FRELPROMOS);
	promos->Clear();
	globalPromos = false;
	if(!canastas.Use(F_Canastas))
	{
		WriteBootLog("No encuentro CANASTAS.DBF o su equivalente. Desactivando promos...");
		return 0;
	}
	canastas.Close();

	// Carga las definiciones de promociones
	//if(!OpenIni(F_Promo))
	if (!CheckExisteIni(F_Promo))
	{
		//CloseIni();
		CIniReader iniReader("pos\\promo.ini");

		for(i=1; i<999; i++)
		{
			sprintf(secc, "promo%d", i);
			char *szactiva = iniReader.ReadString(secc, "activa", "no");
			//if(ExtractIniSiNo(secc, "activa"))
			if (szactiva[0] == 's' || szactiva [0] == 'S')
			{
				int lstpromo = iniReader.ReadInteger(secc, "listaprec", 0);
				if (lstpromo > 0 && lstpromo != defListaPrec)
					continue;
				// Crea la promocion.
				pd = new PromoDef(i);

				// Descripcion y codigo.
				pd->Descrip = iniReader.ReadString(secc, "desc", ""); // ExtractIniStr(secc, "desc");
				pd->CodPlu = atol(iniReader.ReadString(secc, "codigo", "")); //ExtractIniLong(secc, "codigo");
				pd->CodPromo = _atoi64(iniReader.ReadString(secc, "codpromo", "")); //ExtractIniLong(secc, "codigo");
				pd->CodMP = atol(iniReader.ReadString(secc, "mediopago", ""));
				pd->NumTarjeta = atol(iniReader.ReadString(secc, "numtarjeta", ""));
				pd->MarcaCli = atoi(iniReader.ReadString(secc, "marcacli", ""));
				pd->ExcluCli = atoi(iniReader.ReadString(secc, "exclucli", ""));

				// Agrega las expresiones a evaluar.
				line = iniReader.ReadString(secc, "eval", ""); // ExtractIniChar(secc, "eval");
				if(line != NULL && *line) pd->AddExpression(line);
				line = iniReader.ReadString(secc, "eval1", ""); // ExtractIniChar(secc, "eval1");
				if(line != NULL && *line) pd->AddExpression(line);
				for(j=2; j<9999; j++)
				{
					sprintf(lnum, "eval%d", j);
					line = iniReader.ReadString(secc, lnum, ""); // ExtractIniChar(secc, lnum);
					if(line == NULL || (*line ==0)) break;
					pd->AddExpression(line);
				}

				// Agrega la condicion de filtro.
				line = iniReader.ReadString(secc, "filtro", ""); // ExtractIniChar(secc, "filtro");
				pd->SetFilter(line);

				// Agrega las acciones a efectuar.
				line = iniReader.ReadString(secc, "accion", ""); // ExtractIniChar(secc, "accion");
				if(line != NULL && *line) pd->AddAction(line);
				line = iniReader.ReadString(secc, "accion1", ""); // ExtractIniChar(secc, "accion1");
				if(line != NULL && *line) pd->AddAction(line);
				for(j=2; j < 9999; j++)
				{
					sprintf(lnum, "accion%d", j);
					line = iniReader.ReadString(secc, lnum, ""); // ExtractIniChar(secc, lnum);
					if(line == NULL || (*line == 0)) break;
					pd->AddAction(line);
				}

				line = iniReader.ReadString(secc, "cantidad", ""); // ExtractIniChar(secc, "eval1");
				if(line != NULL && *line) 
					pd->AddCantVta(line);

				// Si la definicion es valida, entonces agrega a la lista de promociones definidas.
				if(pd->ValidDef())
					promos->Add(pd);
				else
					delete pd;		// Recupera la memoria consumida.				
			}
		}

		if (Dump::lascanastas == nullptr)
			Dump::lascanastas = gcnew Controles::MCanasta();

		else 
			Dump::lascanastas->Limpiar();
		dbf *canastas = new dbf();
		if(canastas->Use(F_Canastas))
		{
			// Levanta la definicion de la canasta
			int pid, pcod;
			double pcant;

			for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
			{
				canastas->Get("canasta", msg);
				pid = atoi(msg);				
				canastas->Get("cod", msg);
				pcod = atoi(msg);
				canastas->Get("cant", msg);
				pcant = atof(msg);									
				Dump::lascanastas->Agregar(pid, pcod, pcant);
			}
			canastas->Close();
		}
		delete canastas;

		//CloseIni();
		globalPromos = true;		// Activa el modulo de promociones
	}
	else
		WriteBootLog("No encuentro 'PROMOS.INI' o su equivalente. Desactivando promos...");

	return 0;
}

int CargaPromosExtra()
{
	char line[1000];
	PromoDef *pd;
	int i;

	if (Dump::promosfiel == nullptr)
		return 0;	
	////anulo las promociones actuales
	//promos->Clear();
	i = promos->Count();
	//globalPromos = false;

	// Carga las definiciones de promociones		
	for each(Controles::PromoExtra ^unapromo in Dump::promosfiel)
	{		
		// Crea la promocion.
		pd = new PromoDef(i);
		// Descripcion y codigo.
		pd->CodPromo = System::Convert::ToInt64(unapromo->Codpromo); //ExtractIniLong(secc, "codigo");
		if (unapromo->MarcaCli > 0)
			pd->MarcaCli = unapromo->MarcaCli;
		if (unapromo->MedioPago > 0)
			pd->CodMP = unapromo->MedioPago;
		if (unapromo->NumTarjeta > 0)
			pd->NumTarjeta = unapromo->NumTarjeta;

		// Agrega las expresiones a evaluar.
		for each(System::String ^uneval in unapromo->Evals)
		{			
			Strings::StringToChar(uneval, line, 1000);
			if(line != NULL && *line) 
				pd->AddExpression(line);
		}
		// Agrega la condicion de filtro.
		Strings::StringToChar(unapromo->Filtro, line, 1000);
		pd->SetFilter(line);
		// Agrega las acciones a efectuar.
		for each(System::String ^unaccion in unapromo->Acciones)
		{
			Strings::StringToChar(unaccion, line, 1000);			
			if(line != NULL && *line) 
				pd->AddAction(line);
		}

		Strings::StringToChar(unapromo->Cantidad, line, 1000);
		if(line != NULL && *line) 
			pd->AddCantVta(line);

		// Si la definicion es valida, entonces agrega a la lista de promociones definidas.
		if(pd->ValidDef())
			promos->Add(pd);
		else
		{
			delete pd;		// Recupera la memoria consumida.
			continue;
		}
		///comentado de la version anterior
		/*if (Dump::lascanastas == nullptr)
			Dump::lascanastas = gcnew Controles::MCanasta();
		for each(Controles::MCanasDef ^unacan in unapromo->Canastas)
		{
			Dump::lascanastas->Agregar(unacan->Id, unacan->Cod, unacan->Cant);
		}*/
		i++;
	}

	globalPromos = true;		// Activa el modulo de promociones	

	return 0;
}


// Calcula las promociones que aplican. Este metodo es llamado inmediatamente antes de entrar a la fase
// de ingreso de los medios de pago.
void CalcularPromosAntesMP()
{
	xTotal.ClearDescuento();	
	tresSinInteres = 0;
	seisSinInteres = 0;
	/*double tkpiibb = xTotal.VerRecargo(10002).ToDouble();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));*/
	if(globalPromos && !promosCalc)
	{
		double tkpiibb = xTotal.VerRecargo(10002).ToDouble();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		// Se asegura que, al aplicar las acciones, estas solo se simulen.
		soloSimular = true;

		// Examina todas las promociones.
		for(int i=0; i<promos->Count(); i++)
		{
			PromoDef *promo = (PromoDef *)promos->Item(i);
			// Si esta promo ya quedo aplicada y grabada en el ticket actual, no
			// hace falta volver a simularla: su descuento ya esta reflejado en
			// el total real.
			if(PromoTotalmenteAplicada(promo))
				continue;

			//para promociones por medio de pago
			gNumMedioPago = promo->CodMP;
			gImporteSinDescto = 0;
			gNumTarjeta = promo->NumTarjeta;
			actMarcaCli = 0;
			if(!promo->HasRuntimeErrors())
			{
				if (promo->MarcaCli > 0){
					actMarcaCli = promo->MarcaCli;
					if (ClienteBenef > 1){
						//para excluir los clientes, en principio exclucli = 9 empleados puede venir otro valor
						if (promo->ExcluCli > 0 && promo->ExcluCli == ClienteBenef)
							continue;
						if (promo->MarcaCli == ClienteBenef){
							promo->Evaluate();
							promo->ApplyAction();
						}else if ((promo->MarcaCli == 10 || promo->MarcaCli == 14) && (ClienteBenef >= 1 && ClienteBenef <= 14)){
							promo->Evaluate();
							promo->ApplyAction();
						}else
							continue;						
					}
					else
						continue;
				}
				else
				{
					promo->Evaluate();
					promo->ApplyAction();
				}
			}
		}
		xTotal.AddRecargo(10002, BDecimal::BDecimal(tkpiibb));

		promosCalc = true;			// Evita que esta funcion se llame varias veces.
		promosAplicadas = false;	// Permite que se apliquen las promociones.
		WriteCmos(PROMOCALC);
	}
}

void CalcularPromoJanisAntesMP(){
	xTotal.ClearDescuento();
	if (Dump::PedidoJanis != nullptr)
	{
		for (int i = 0; i < Dump::PedidoJanis->items->Count; i++)
		{
			Controles::ItemPos^ item = Dump::PedidoJanis->items[i];
			//LibEntidades::Janis::ItemPos^ item = Dump::PedidoJanis->items[i];
			if (item->Precio < 0)
				xTotal.AddDescuento(BDecimal::BDecimal(System::Convert::ToDouble(item->Precio ) * (-1)));
		}
	}
}

//  Aplica todas las promociones.
void AplicarPromociones()
{
	if(globalPromos && !promosAplicadas && !promosHechas)
	{
		// Se asegura que, al aplicar las acciones, estas se ejecuten.
		soloSimular = false;

		// Examina las promociones activas y dispara las acciones programadas.
		for(int i=0; i<promos->Count(); i++)
		{
			PromoDef *promo = (PromoDef *)promos->Item(i);

			// Solo se saltea la promo si TODAS sus acciones ya quedaron grabadas
			// en trans.dbf. Si el reinicio la corto por la mitad, vuelve a entrar
			// y ApplyAction() retoma unicamente las acciones que faltan.
			if(PromoTotalmenteAplicada(promo))
				continue;

			gNumMedioPago = promo->CodMP;
			gImporteSinDescto = 0;
			gNumTarjeta = promo->NumTarjeta;
			if(!promo->HasRuntimeErrors())
			{
				if (promo->MarcaCli > 0){
					actMarcaCli = promo->MarcaCli;
					if (ClienteBenef > 1){
						//para excluir los clientes, en principio exclucli = 9 empleados puede venir otro valor
						if (promo->ExcluCli > 0 && promo->ExcluCli == ClienteBenef)
							continue;
						if (promo->MarcaCli == ClienteBenef){
							pluAdicText = promo->Descrip;
							promo->ApplyAction();
							pluAdicText = NULL;

						}else if ((promo->MarcaCli == 10 || promo->MarcaCli == 14) && (ClienteBenef >= 1 && ClienteBenef <= 14)){
							pluAdicText = promo->Descrip;
							promo->ApplyAction();
							pluAdicText = NULL;
						}else
							continue;						
					}
					else
						continue;
				}
				else
				{
					pluAdicText = promo->Descrip;
					promo->ApplyAction();
					pluAdicText = NULL;
				}
			}
		}

		// Evita que esta operacion se repita
		promosAplicadas = true;
		promosHechas = true;		
		WriteCmos(PROMOSHECHAS);

		xTotal.ClearDescuento();
	}
}

void AplicaPromoJanis(){
	if (Dump::PedidoJanis != nullptr)
	{	
		for (int i = 0; i < Dump::PedidoJanis->items->Count; i++)
		{
			Controles::ItemPos^ item = Dump::PedidoJanis->items[i];
			//LibEntidades::Janis::ItemPos^ item = Dump::PedidoJanis->items[i];
			if (item->Precio < 0)
			{
				/*xReg = System::Convert::ToDouble(item->Cod);
				if (item->EsPesable)
					yReg = System::Convert::ToDouble(item->Cantidad * item->Precio);
				else
					yReg = System::Convert::ToDouble(item->Cantidad);
				xPrecEcom = System::Convert::ToDouble(item->Precio);*/
				//plu(0);

				// Busca el articulo a promocionar.
				char *ucod = padl(System::Convert::ToDouble(item->Cod), 6);
				dbplu.Setorder("mplucod");				
				double auxmonto = Decimal::ToDouble(item->Precio * (-1));
				if(dbplu.Seek(ucod))
				{						
					struct DDplu_ plu;
					plu.anulado = 0;
					plu.cod = item->Cod;
					plu.envas = 0;
					plu.fecha = date();
					plu.hora = time_();
					plu.cajero = ckaNro;
					plu.func = 4;
					plu.miles[0] = 0;
					plu.otroP = listap;
					plu.pesado = 0;
					plu.precuni = (unsigned long)((auxmonto + 1e-10) * 100);
					plu.s_c_flag = 0;
					plu.uCant = 0;
					store(-auxmonto, plu.xReg);
					store(1, plu.yReg);//antes iba cant en lugar de 1
					STRCPY(plu.origCode, ucod);

					STRCPY(plu.montoImpInterno, "0.00");

					dbplu.Get("iva", msg); 
					plu.iva = atoi(msg);
					dbplu.Get("dep", msg);
					plu.dnro = atoi(msg);			
					dbplu.Get("subdep", msg);
					plu.sdnro = atoi(msg);
					plu.esBulto = 0;
					plu.uni = 1;
					*plu.precmay = 0;
					plu.ppuntual = 0;
					plu.umillas = 0;
					sprintf(plu.pesoenbalanza, "%.3lf", 0.0);

					AcumDesctoPromo += (auxmonto + 1e-10);

					pluAdicText = NULL;
					ProcPlu(&plu, 1);
					/*char tmp[20];
					sprintf(tmp, "%d", item->Cod);
					RegisterPrize(3, tmp, plu.yReg, plu.xReg, false, 0);	*/								
					
				}				
			}
		}
	}
	xTotal.ClearDescuento();
}

void ProcPromo(void *d, int fwrite)
{
	struct DPromo_ *promo = (DPromo_ *)d;
	String ^auxcodigo = gcnew String(promo->p1);
	// Registra/actualiza la tabla de promociones ya aplicadas en este ticket,
	// tanto si el renglon se esta escribiendo ahora (fwrite=1) como si se esta
	// reprocesando desde trans.dbf al iniciar la caja (fwrite=0). No depende
	// de Dump::docActual ni de fwrite, a diferencia del registro .NET de abajo.
	PromoAplicadaInfo *infoAplic = BuscaPromoAplicada(promo->codpromo);
	if(infoAplic == NULL)
	{
		PromoAplicadaInfo nueva;
		nueva.codPromo = promo->codpromo;
		nueva.aplicado = false;
		nueva.legacy = false;
		tablaPromosAplicadas.push_back(nueva);
		infoAplic = &tablaPromosAplicadas.back();
	}
	System::Decimal decMontoTabla = 0;
	System::Decimal::TryParse(gcnew String(promo->preciouni) == "0.000" ? gcnew String(promo->p3) : gcnew String(promo->preciouni), decMontoTabla);
	infoAplic->montoAplicado = System::Decimal::ToDouble(decMontoTabla);
	infoAplic->descarga = 0;
	if (auxcodigo != "")
	{
		System::Int32 auxIntCodTabla = 0;
		System::Int32::TryParse(auxcodigo, auxIntCodTabla);
		infoAplic->descarga = auxIntCodTabla;
	}
	// Solo la accion de descuento/entrega real (premio==3) cierra la promo.
	// Un renglon informativo (premio 1 o 2) por si solo no debe bloquear que,
	// tras un reinicio, se reintente la accion de descuento pendiente.
	if(promo->tpremio == 3)
		infoAplic->aplicado = true;

	// Asienta la accion concreta que grabo este renglon. accionSeq==0 significa
	// que viene de un trans.dbf grabado por la version anterior: ahi no hay
	// resume fino posible y la promo se marca como legacy.
	if(promo->accionSeq > 0)
	{
		bool yaEsta = false;
		for(size_t iac=0; iac<infoAplic->acciones.size(); iac++)
		{
			if(infoAplic->acciones[iac].seq == promo->accionSeq)
			{
				infoAplic->acciones[iac].montoAplicado = infoAplic->montoAplicado;
				infoAplic->acciones[iac].descarga = infoAplic->descarga;
				yaEsta = true;
				break;
			}
		}
		if(!yaEsta)
		{
			AccionAplicadaInfo nuevaAc;
			nuevaAc.seq = promo->accionSeq;
			nuevaAc.montoAplicado = infoAplic->montoAplicado;
			nuevaAc.descarga = infoAplic->descarga;
			infoAplic->acciones.push_back(nuevaAc);
		}
	}
	else
		infoAplic->legacy = true;

	if(fwrite) WriteDump(promo);
	if (fwrite && Dump::docActual != nullptr)
	{
		LibEntidades::Alberdi::PromoAplicada ^miPromo = gcnew LibEntidades::Alberdi::PromoAplicada();
		System::Decimal decValue = 0;
		System::Decimal::TryParse(gcnew String(promo->cantpromo) == "0.000" ? gcnew String(promo->p2) :  gcnew String(promo->cantpromo), decValue);
		miPromo->Cantidad = decValue;
		decValue = 0;
		System::Decimal::TryParse(gcnew String(promo->preciouni) == "0.000" ? gcnew String(promo->p3) : gcnew String(promo->preciouni), decValue);
		miPromo->Monto = decValue;
		miPromo->CodPromo = promo->codpromo;
		if (auxcodigo != ""){
			System::Int32 auxIntCod = 0;
			System::Int32::TryParse(auxcodigo, auxIntCod);
			miPromo->Descarga = auxIntCod;
		}
		miPromo->Hora = System::DateTime::Now;
		Dump::docActual->Promociones->Add(miPromo);
	}

#ifdef FACTSQL
	if (numsecuencia > 0 && fwrite)
	{
		TransSql::MDPromo^ unaprom = gcnew TransSql::MDPromo();
		unaprom->Func = promo->func;
		unaprom->Nro = promo->nro;
		unaprom->Codpromo = promo->codpromo;
		unaprom->Tpremio = promo->tpremio;
		unaprom->Imprime = promo->imprime;
		unaprom->Linea = promo->linea;
		unaprom->P1 = gcnew System::String(promo->p1);
		unaprom->P2 = gcnew System::String(promo->p2);
		unaprom->P3 = gcnew System::String(promo->p3);
		unaprom->Cantpromo = gcnew System::String(promo->cantpromo);
		unaprom->Preciouni = gcnew System::String(promo->preciouni);
		unaprom->Fecha = promo->fecha;
		unaprom->Hora = promo->hora;
		unaprom->Cajero = promo->cajero;
		unaprom->Tienda = gcnew System::String(saptienda);
		unaprom->Caja = caja;
		unaprom->CnlVta = gcnew System::String(sapcnlvta);
		unaprom->Towrite = true;
		Dump::sqltrans->Add(unaprom);
	}
#endif
}

// Busca la definicion de una promo por su codigo.
static PromoDef *BuscaPromoDef(long long codPromo)
{
	if(promos == NULL) return NULL;
	for(int i=0; i<promos->Count(); i++)
	{
		PromoDef *promo = (PromoDef *)promos->Item(i);
		if(promo->CodPromo == codPromo)
			return promo;
	}
	return NULL;
}

// Repone acumSaldoPromo despues de reprocesar un ticket pendiente al iniciar
// la caja. Es una variable de proceso y se pierde en el reinicio; sin ella
// AlmacConsumo() (MPAGO.CPP) llama a ActualizaConsumo() en vez de
// ActualizaConsumoCli(), y el saldocaja del cliente nunca se descuenta en la
// BD aunque el ticket ya le haya dado el descuento.
//
// Ojo con la semantica: F_DescTotalIva2() ASIGNA acumSaldoPromo (y la pone en
// 0 antes), no la acumula. O sea que vale el ultimo descuento aplicado, no la
// suma de todos. Reconstruirla sumando descontaria de mas.
void ReconstruirAcumSaldoPromo()
{
	for(size_t i=0; i<tablaPromosAplicadas.size(); i++)
	{
		PromoAplicadaInfo *info = &tablaPromosAplicadas[i];
		PromoDef *promo = BuscaPromoDef(info->codPromo);
		if(promo == NULL) continue;

		for(size_t j=0; j<info->acciones.size(); j++)
		{
			if(!promo->AccionUsaSaldoCaja(info->acciones[j].seq))
				continue;
			double monto = info->acciones[j].montoAplicado;
			acumSaldoPromo = monto < 0 ? -monto : monto;
		}
	}
}

void ApliPromoCobra()
{
	xTotal.ClearDescuento();
	promosCalc = false;
	if(globalPromos && !promosCalc)
	{
		double tkpiibb = xTotal.VerRecargo(10002).ToDouble();
		xTotal.AddRecargo(10002, BDecimal::BDecimal(0));
		// Se asegura que, al aplicar las acciones, estas solo se simulen.
		soloSimular = true;		

		// Examina todas las promociones.
		for(int i=0; i<promos->Count(); i++)
		{
			PromoDef *promo = (PromoDef *)promos->Item(i);
			// Si esta promo ya quedo integramente aplicada y grabada en un intento
			// anterior de enviar este mismo ticket a caja cobradora, no hay que
			// volver a simularla ni reaplicarla (ver CHANGELOG 2026-08-06).
			if(PromoTotalmenteAplicada(promo))
				continue;
			gNumMedioPago = promo->CodMP;			
			gNumTarjeta = promo->NumTarjeta;
			if (gNumMedioPago == 0 && gNumTarjeta == 0)
			{
				if (promo->CodMP > 0 || promo->MarcaCli > 0)
					continue;
				if(!promo->HasRuntimeErrors())
				{
					promo->Evaluate();
					promo->ApplyAction();
				}
			}
		}
		xTotal.AddRecargo(10002, BDecimal::BDecimal(tkpiibb));

		//promosCalc = true;			// Evita que esta funcion se llame varias veces.
		promosAplicadas = false;	// Permite que se apliquen las promociones.
		//WriteCmos(PROMOCALC);
	}
	FactuPercep(0, false);		
	FactuPercepTissh(0, false);
	FactuPercepIva(0, false);

	if(globalPromos && !promosAplicadas) //&& !promosHechas)
	{
		// Se asegura que, al aplicar las acciones, estas se ejecuten.		
		soloSimular = false;

		// Examina las promociones activas y dispara las acciones programadas.
		for(int i=0; i<promos->Count(); i++)
		{
			PromoDef *promo = (PromoDef *)promos->Item(i);
			// Idem nota de arriba: si un reintento anterior ya dejo esta promo
			// aplicada (descuento/entrega confirmado), no volver a dispararla.
			if(PromoTotalmenteAplicada(promo))
				continue;
			if (promo->CodMP > 0 || promo->MarcaCli > 0)
				continue;

			gNumMedioPago = promo->CodMP;			
			gNumTarjeta = promo->NumTarjeta;
			if (gNumMedioPago == 0 && gNumTarjeta == 0)
			{				
				if(!promo->HasRuntimeErrors())
				{
					pluAdicText = promo->Descrip;
					promo->ApplyAction();
					pluAdicText = NULL;
				}
			}
		}

		// Evita que esta operacion se repita
		//promosAplicadas = true;
		//promosHechas = true;
		//WriteCmos(PROMOSHECHAS);

		xTotal.ClearDescuento();
	}
	FactuPercep(1, false);
	FactuPercepTissh(1, false);
	FactuPercepIva(1, false);
}


void RegisterPrize(int premio, char *p1, char *p2, char *p3, bool imprime, int linea)
{
	struct DPromo_ dp;

	// No hace nada, si estamos en modo de simulacion.
	if(soloSimular) return;

	dp.func = DPromo;
	// dp no se memsetea: todo campo nuevo hay que inicializarlo aca a mano.
	dp.accionSeq = accionActualSeq;
	dp.nro = actualPromo->num;
	dp.tpremio = premio;
	dp.cod = actualPromo->CodPlu;
	dp.codpromo = actualPromo->CodPromo;
	dp.cajero = ckaNro;
	dp.imprime = imprime ? 1 : 0;
	dp.linea = linea;
	if (premio == 3)
	{
		store(cntApplyPromo, dp.cantpromo);
		store(puApplyPromo, dp.preciouni);
	}
	else
	{
		strcpy(dp.cantpromo, "");
		strcpy(dp.preciouni, "");
	}
	dp.fecha = (long)date();
	dp.hora = (long)time__();
	if (actualPromo->CodPromo >= 99999999999001 && actualPromo->CodPromo <= 99999999999999)
		strcpy(leyendaFinde, "TENES UN CUPON! VALIDALO EN CLUBCOMODIN.COM.AR");

	if(strlen(p1) > 29) p1[29] = 0;
	if(strlen(p2) > 29) p2[29] = 0;
	if(strlen(p3) > 29) p3[29] = 0;
	STRCPY(dp.p1, p1);
	STRCPY(dp.p2, p2);
	STRCPY(dp.p3, p3);

	ProcPromo(&dp, 1);
}

//Recarga la planificacion de premios instantaneos
void ReloadPlanifInst()
{
	try
	{
		if (!ReloadPlanifInst)
			return;
		reloadPremioInst = false;
		WriteCmos(FRELPREMIN);
		if (access("pos\\tipoprem.new", 6) != 0)
			return;
		if (access("pos\\premioin.new", 6) != 0)
			return;
		bool encontrado = false;
		if (access("pos\\premioin.dbf", 6) != 0 || access("pos\\tipoprem.dbf", 6) != 0)
		{
			rename("pos\\premioin.new", "pos\\premioin.dbf");
			rename("pos\\tipoprem.new", "pos\\tipoprem.dbf");
			WriteBootLog("Recibio los archivos de promocion, 1º vez");
			return;
		}
		//int ltpremio, cmptpremio;
		long lfecha, lfactual;
		int lhinicio, lhfin, ltpremio;
		int cmpfecha, cmphinicio, cmphfin, cmptpremio;

		char buffprem[50];
		dbf *ttpremionew = new dbf();
		if (ttpremionew->Use("pos\\tipoprem.new"))
		{		
			dbf *ttpremio = new dbf();
			ttpremio->Use("pos\\tipoprem.dbf");		
			for(ttpremionew->Go(DB_GO_TOP); !ttpremionew->Eof(); ttpremionew->Skip(1))
			{
				ttpremionew->Get("idtpremio", buffprem);
				ltpremio = atoi(buffprem);
				encontrado = false;
				for(ttpremio->Go(DB_GO_TOP); !ttpremio->Eof(); ttpremio->Skip(1))
				{
					ttpremio->Get("idtpremio", buffprem);
					cmptpremio = atoi(buffprem);
					if (ltpremio == cmptpremio)
					{
						encontrado = true;
						break;
					}
				}
				if (!encontrado)
				{
					ttpremio->AppendBlank();
					sprintf(buffprem, "%06d", ltpremio);
					ttpremio->Replace("idtpremio", buffprem);
				}
				ttpremionew->Get("des", buffprem);
				ttpremio->Replace("des", buffprem);			
			}
			ttpremio->Close();
			ttpremionew->Close();
			delete ttpremio;
		}
		
		dbf *premioinnew = new dbf();
		if (premioinnew->Use("pos\\premioin.new"))
		{		
			dbf *premioin = new dbf();
			premioin->Use("pos\\premioin.dbf");
			//borra la planificacion distinta al dia actual
			lfactual = date();
			for(premioin->Go(DB_GO_TOP); !premioin->Eof(); premioin->Skip(1))
			{
				premioin->Get("fecha", buffprem);
				lfecha = atol(buffprem);
				if (lfecha != lfactual)
					premioin->Delete();
			}
			premioin->Pack();
			premioin->Close();
			premioin->Use("pos\\premioin.dbf");
			for(premioinnew->Go(DB_GO_TOP); !premioinnew->Eof(); premioinnew->Skip(1))
			{
				premioinnew->Get("fecha", buffprem);
				lfecha = atol(buffprem);
				if (lfecha < lfactual) //si la planificacion es anterior al dia de la fecha
					continue; // ignora las planificaciones anteriores
				else if (lfecha == lfactual) //si la fecha es la actual reemplazo lo actual
				{						
					premioinnew->Get("hinicio", buffprem);
					lhinicio = atoi(buffprem);
					premioinnew->Get("hfin", buffprem);
					lhfin = atoi(buffprem);
					premioinnew->Get("tpremio", buffprem);
					ltpremio = atoi(buffprem);					
					for(premioin->Go(DB_GO_TOP); !premioin->Eof(); premioin->Skip(1))
					{
						premioin->Get("fecha", buffprem);
						cmpfecha = atol(buffprem);
						premioin->Get("hinicio", buffprem);
						cmphinicio = atoi(buffprem);
						premioin->Get("hfin", buffprem);
						cmphfin = atoi(buffprem);
						premioin->Get("tpremio", buffprem);
						cmptpremio = atoi(buffprem);
						if (lfecha == cmpfecha && lhinicio == cmphinicio && lhfin == cmphfin && ltpremio == cmptpremio)
						{
							premioinnew->Get("idplanif", buffprem);
							premioin->Replace("idplanif", buffprem);
							premioinnew->Get("stock", buffprem);
							premioin->Replace("stock", buffprem);							
						}
					}
				}
				else // la planificacion es posterior al dia actual, lo agrega
				{					
					premioin->AppendBlank();
					premioinnew->Get("idplanif", buffprem);
					premioin->Replace("idplanif", buffprem);					
					premioinnew->Get("fecha", buffprem);
					premioin->Replace("fecha", buffprem);
					premioinnew->Get("hinicio", buffprem);
					premioin->Replace("hinicio", buffprem);
					premioinnew->Get("hfin", buffprem);
					premioin->Replace("hfin", buffprem);
					premioinnew->Get("tpremio", buffprem);
					premioin->Replace("tpremio", buffprem);
					premioinnew->Get("stock", buffprem);
					premioin->Replace("stock", buffprem);
					premioinnew->Get("reserv", buffprem);
					premioin->Replace("reserv", buffprem);
					premioinnew->Get("entrego", buffprem);
					premioin->Replace("entrego", buffprem);
				}
			}
			premioin->Close();
			premioinnew->Close();
			delete premioin;
		}
		delete premioinnew;
		delete ttpremionew;
		remove("pos\\premioin.new");
		remove("pos\\tipoprem.new");	
		WriteBootLog("Recibio Actualizacion de archivos de promoción.");
	}
	catch (Exception ^excep)
	{
		System::IO::StreamWriter^ writer = System::IO::File::AppendText("errordbf.txt");
		System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ReloadPlanifInst", System::DateTime::Now);
		writer->WriteLine(fhdatos);		
		writer->WriteLine("Mensaje: " + excep->Message);
		writer->WriteLine("StackTrace: " + excep->StackTrace);
		writer->WriteLine("----------------------------------");
		writer->Close();
		WriteBootLog("ReloadPlanifInst - Error al actualizar los archivos de Premios instantaneos");
	}	
}

// -------------- Implementacion de las clases de promo ----------------------------------------------
PromoDef::PromoDef(int promoNum)
{
	num = promoNum;
	evalRengs = new ::ArrayList(false, "PromoDef-EvalRengs");
	actionExps = new ::ArrayList(false, "PromoDef-ActionExps");
	cantidad = new ::ArrayList(false, "PromoDef-Cantidad");	
	filterExp = NULL;
	filterResult = NULL;			// ApplyAction() lo dereferencia: sin esto queda indeterminado.
	accionesSaldoCajaMask = 0;
	CodMP = 0;
}

PromoDef::~PromoDef()
{
	for(int i=0; i<evalRengs->Count(); i++)
		delete evalRengs->Item(i);
	delete evalRengs;

	for(int i=0; i<actionExps->Count(); i++)
		delete actionExps->Item(i);
	delete actionExps;

	for(int i=0; i<cantidad->Count(); i++)
		delete cantidad->Item(i);
	delete cantidad;

	if(filterExp != NULL)
		delete filterExp;
}

void PromoDef::AddAction(char *act)
{
	Expression *action = new Expression();
	action->Compile(act);
	EvError *error = action->Error();
	if(error != NULL)
	{
		sprintf(msg, "Error Def Promo: %d\n\tAccion incorrecta: '%s'\n\tError: '%s'", num, act, error->Message());
		WriteBootLog(msg);
		delete action;
		return;
	}
	action->NameResolver = PromoNameResolver;
	actionExps->Add(action);

	// Marca las acciones que descuentan contra el saldocaja del cliente. La
	// unica que lo hace es DESC_TOTIVACC (F_DescTotalIva2), que ademas es la
	// unica que asigna acumSaldoPromo. Se detecta sobre el texto crudo de la
	// accion: MarcaCli NO sirve como indicador, hay promos con MarcaCli>0 que
	// no tocan saldocaja. El bit se prende despues del Add() para que el indice
	// coincida con la posicion real en actionExps (arriba hay un return por
	// error de compilacion que no agrega la accion).
	int idxAccion = actionExps->Count();		// 1-based
	if(idxAccion >= 1 && idxAccion <= 32 && strstr(act, "DESC_TOTIVACC") != NULL)
		accionesSaldoCajaMask |= (1L << (idxAccion - 1));
}

void PromoDef::AddExpression(char *exp)
{
	// Separa el nombre de la variable.
	char *foxExp = strchr(exp, '=');
	if(foxExp == NULL || *foxExp == 0)
	{
		sprintf(msg, "Error Def Promo: %d:\n\tFalta el '=' en '%s'", num, exp);
		WriteBootLog(msg);
		return;
	}
	
	*foxExp = 0;
	foxExp++;
	Trim(exp);

	if(*exp == 0)
	{
		sprintf(msg, "Error Def Promo: %d\n\tFalta nombre de variable en '%s'", num, foxExp);
		WriteBootLog(msg);
		return;
	}

	// Crea y compila la expresion.
	Expression *e = new Expression();
	e->Compile(foxExp);
	EvError *err = e->Error();
	if(err != NULL)
	{
		sprintf(msg, "Error Def Promo %d:\n\t'%s'\n\tError: '%s'", num, foxExp, err->Message());
		WriteBootLog(msg);
		delete e;
		return;
	}

	// Completa la inicializacion de la expresion y la agrega a la definicion de la promo.
	e->NameResolver = PromoNameResolver;
	evalRengs->Add(new EvalReng(exp, e));
}

void PromoDef::AddCantVta(char *exp)
{
	// Separa el nombre de la variable.
	char *foxExp = strchr(exp, '=');
	if(foxExp == NULL || *foxExp == 0)
	{
		sprintf(msg, "Error Def Promo: %d:\n\tFalta el '=' en '%s'", num, exp);
		WriteBootLog(msg);
		return;
	}

	*foxExp = 0;
	foxExp++;
	Trim(exp);

	if(*exp == 0)
	{
		sprintf(msg, "Error Def Promo: %d\n\tFalta nombre de variable en '%s'", num, foxExp);
		WriteBootLog(msg);
		return;
	}

	// Crea y compila la expresion.
	Expression *e = new Expression();
	e->Compile(foxExp);
	EvError *err = e->Error();
	if(err != NULL)
	{
		sprintf(msg, "Error Def Promo %d:\n\t'%s'\n\tError: '%s'", num, foxExp, err->Message());
		WriteBootLog(msg);
		delete e;
		return;
	}

	// Completa la inicializacion de la expresion y la agrega a la definicion de la promo.
	e->NameResolver = PromoNameResolver;
	cantidad->Add(new EvalReng(exp, e));
}


void PromoDef::SetFilter(char *filt)
{
	Expression *e = new Expression();
	e->Compile(filt);
	EvError *err = e->Error();
	if(err != NULL)
	{
		sprintf(msg, "Error Def Promo: %d\n\t filtro '%s' invalido\n\tError: '%s'", num, filt, err->Message());
		WriteBootLog(msg);
		delete e;
		return;
	}

	e->NameResolver = PromoNameResolver;
	filterExp = e;
}

bool PromoDef::ValidDef()
{
	if(filterExp == NULL) return false;
	if(actionExps->Count() == 0) return false;
	hasErrors = false;
	return true;
}

// Evalua un renglon y asigna el resultado a la variable indicada.
bool EvalReng::Evaluate()
{
	Reset();
	Value *tempResult = evalExp->Run();
	runError = evalExp->RunError();
	if(runError == NULL)
		result = new Value(tempResult);
	delete tempResult;
	return (runError == NULL);
}

// Elimina todos los posibles resultados de la evaluacion, incluyendo el limpiado de
// las variables de voucher.
void PromoDef::Reset()
{
	if(hasErrors) return;
	for(int i=0; i<evalRengs->Count(); i++)
	{
		EvalReng *reng = (EvalReng *)evalRengs->Item(i);
		reng->Reset();
	}
}

// Evalua la promocion calculando los posibles resultados.
void PromoDef::Evaluate()
{
	EvError *error;

	Reset();
	if(hasErrors) return;

	// Calcula las distintas expresiones de evaluacion.
	actualPromo = this;
	for(int i=0; i<evalRengs->Count(); i++)
	{
		EvalReng *reng = (EvalReng *)evalRengs->Item(i);
		if(!reng->Evaluate())
		{
			error = reng->Error();
			sprintf(msg, "Error al evaluar promocion nro %d (eval):\n'%s' - Promo Desactivada", 
				num, error->Message());
			WriteBootLog(msg);
			Reset();
			hasErrors = true;
			return;
		}
	}

	// Evalua el filtro.
	EvalReng *ver = (EvalReng *)evalRengs->Item(0);
	filterResult = filterExp->Run();
	error = filterExp->RunError();
	if(error != NULL)
	{
		sprintf(msg, "Error al evaluar promocion nro %d (filtro):\n'%s' - Promo Desactivada", 
			num, error->Message());
		WriteBootLog(msg);
		Reset();
		hasErrors = true;
		return;
	}

	if (cantidad->Count()>0)
	{
		EvalReng *rengcant = (EvalReng *)cantidad->Item(0);
		if(!rengcant->Evaluate())
		{
			error = rengcant->Error();
			sprintf(msg, "Error al evaluar promocion nro %d (eval):\n'%s' - Promo Desactivada", 
				num, error->Message());
			WriteBootLog(msg);
			Reset();
			hasErrors = true;
			return;
		}
	}
}

Value *PromoDef::ResolveName(char *name)
{
	for(int i=0; i<evalRengs->Count(); i++)
	{
		EvalReng *reng = (EvalReng *)evalRengs->Item(i);
		if(!_stricmp(name, reng->VarName()))
			return new Value(reng->Result());
	}
	return NULL;
}

void PromoDef::ApplyAction()
{
	char varName[100];

	if(hasErrors) return;
	// filterResult lo produce Evaluate(). AplicarPromociones() no llama a
	// Evaluate(): depende de que CalcularPromosAntesMP() haya corrido antes en
	// el mismo proceso. Si no corrio, esto era una lectura de memoria basura.
	if(filterResult == NULL) return;
	EvalReng *ver = (EvalReng *)evalRengs->Item(0);
	actualPromo = this;
	if(filterResult->ValueBoolean())
	{
		// Envia como variable de voucher las distintas variables construidas.
		for(int i=0; i<evalRengs->Count(); i++)
		{
			EvalReng *reng = (EvalReng *)evalRengs->Item(i);
			Value *result = reng->Result();
			sprintf(varName, "@%s@", reng->VarName());
			switch(result->Type())
			{
				case 'I':
				case 'i':
					VoucherVar(varName, result->ValueInteger()); 
					break;

				case 'D':	
				case 'd':
					VoucherVar(varName, result->ValueDouble()); 
					break;

				case 'B':	
				case 'b':
					VoucherVar(varName, result->ValueBoolean()? si: no); 
					break;

				case 'S': 
				case 'C':
				case 's':
				case 'c':
					VoucherVar(varName, result->ValueString()); 
					break;
			}
		}

		if (cantidad->Count()>0)
		{
			EvalReng *rengcant = (EvalReng *)cantidad->Item(0);
			Value *rescant = rengcant->Result();
			cntApplyPromo = (int)rescant->ValueDouble();
		}

		enApplyAction = true;

		// Ejecuta las acciones y analiza el resultado.
		for(int i=0; i<actionExps->Count(); i++)
		{
			// Si esta accion ya quedo grabada en trans.dbf (ticket interrumpido
			// por un reinicio de caja) no hay que repetirla: repetirla duplica el
			// descuento y vuelve a llamar a los servicios externos que dispare
			// (vouchers, stock de premios).
			if(AccionYaAplicada(CodPromo, i + 1))
				continue;

			accionActualSeq = i + 1;
			Expression *action = (Expression *)actionExps->Item(i);
			Value *result = action->Run();
			delete result;
			if(action->RunError() != NULL)
			{
				sprintf(msg, "Error al ejecutar la accion de la promo nro %d\n\t'%s' - Promo Desactivada",
					num, action->RunError()->Message());
				WriteBootLog(msg);
				hasErrors = true;
				break;
			}
		}
		accionActualSeq = 0;
		enApplyAction = false;
	}
}

void PrintPromo(void *promo_)
{
	struct DPromo_ *promocion;
	promocion = (struct DPromo_ *) promo_;	 
	if (promocion->imprime)
		pf->SetPie(promocion->p1, promocion->linea);
}
