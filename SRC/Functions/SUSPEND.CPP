//
//   Suspend.cpp
//   Suspende / Recupera tickets de la memoria.
//

#include "stdafx.h"
#include "funcs.h"
#ifdef FACTSQL
#include "frmFactSql.h"
#include "StaticDump.h"
using namespace TransSql;
#endif

//  Maximo nro de suspensiones permitidas.
#define MAXSUSP	800



//  Opciones configurables en 'opciones.ini'
int prndelay, autoapp, recuany, autonro, usegraf, actSusp, enEspera;
char *rango;
char initBM;
char *suspMsg1;
char *suspMsg2;
char *suspFn = new char[20];
BitMap *srBm;

//tbl_ *tbl; MESAS

//  Funcion que compila en un bitmap el rango permitido para el nro de suspension.
void LoadSuspRange()
{
	char *p;
	unsigned n1, n2, n3;

	srBm = new BitMap(MAXSUSP);
	if(srBm == NULL) AbnormalEnd("No puedo inicializar mapa de Susp");
	if(!rango || Empty(rango))
	{
		srBm->SetALLBits();
		return;
	}
	
	//   Compila el rango.
	for(p = rango; *p; )
	{
		while(isspace(*p)) p++;
		for(n1 = 0; isdigit(*p); p++) n1 = 10 * n1 + (*p-'0');
		if(n1 < 1 || n1 > MAXSUSP) break;
		{
			srBm->SetBit(n1);
			sprintf(suspFn, "TICK%d.SUS", n1);
			//if(Existe(suspFn)) MESAS
			//	SetTableStatus(n1, TABLE_BUSY);
		}

		while(isspace(*p)) p++;
		if(*p == ',') 
		{
			p++;
			continue;
		}
		if(*p != '-') break;
		p++; while(isspace(*p)) p++;
		for(n2 = 0; isdigit(*p); p++) n2 = 10 * n2+ (*p-'0');
		if(n2 < 0 || n2 > MAXSUSP) break;
		if(n2 < n1) { n3 = n2; n2 = n1; n1 = n3; }
		for(n3 = n1; n3 <= n2; n3++)
		{
			srBm->SetBit(n3);
			sprintf(suspFn, "TICK%d.SUS", n3);
			//if(Existe(suspFn)) MESAS
			//	SetTableStatus(n3, TABLE_BUSY);
		}
		while(isspace(*p)) p++;
		if(*p == ',')
		{
			p++;
			continue;
		}
		break;
	}
}

//  Funcion que retorna true, si queda alguna mesa sin cerrar.
int chkNoSusp(int cual)
{
//	int i;

//	if(!initBM) LoadSuspRange();
//	for(i = firstBit(srBm); i; i = nextBit(srBm))
//		if(GetTableStatus(i))
//			return 1;
//		else
//		{
	/*sprintf(suspFn, "TICK%d.SUS", (cual == 0)? 1: cual);
			if(Existe(suspFn))
				return 1;*/
//		}

	int i, k;
	k = 0;
	if(!initBM) LoadSuspRange();

	for(i = srBm->FirstBit(); i; i = srBm->NextBit())
	{
		sprintf(suspFn, "TICK%d.SUS", i);
		if(Existe(suspFn)) 				
		{
			k = 1;
			break;
		}
			
	}
	return k;
}

void borraSusp(int cual)
{
	int i;

	if(!initBM) LoadSuspRange();

	for(i = srBm->FirstBit(); i; i = srBm->NextBit())
	{
		sprintf(suspFn, "TICK%d.SUS", i);
		if(Existe(suspFn)) 				
			remove(suspFn);			
	}
}

void recupera_(int);
void recupera(int cual)
{
	GCStatic::LockMainLoop("recupera");
	recupera_(cual);
	GCStatic::ReleaseLockMainLoop("recupera");
}

//  Funcion que recupera un ticket previamente suspendido.
void recupera_(int dummy)
{
	int i;
	char tmsg[8];
	struct DSuspende_ susp;

	//   Hace algunas verificaciones.
	CustDisp("",  "");
	if(!initBM) LoadSuspRange();
	if(inOper > 1)
	{
		Alert("Ya ingresó en la fase de pago de un ticket",
			  "No puede recuperar una operacion suspendida ahora");
		return;
	}
	if(xReg == 0.0)
	{
		if(!autonro && !usegraf)
		{
			Alert("Ingrese el nro a recuperar",
				"Antes de pulsar la tecla RECUPERA");
			return;
		}
		/*if(usegraf) MESAS
		{
			i = AnimateGraf(actSusp);
			if(!i)
			{
				clear(1);
				return;
			}
		}
		else*/
		if (!usegraf) // MESAS agregado en lugar del ELSE
		{
			bool haypararecup = false;
			int tickposic[30] = {0};
			int kpos = 0;
			Choose *mpch = new Choose(" Recupera Tickets Suspendidos ", "ESC=Cancela", 0, 5, CHOOSE_ALIGN_LEFT, NULL);			

 			for(i = srBm->FirstBit(); i; i = srBm->NextBit())
			{
				sprintf(suspFn, "TICK%d.SUS", i);
				if(Existe(suspFn)) 
				//	break;
				{
					mpch->Add(suspFn);
					haypararecup = true;
					tickposic[kpos] = i;
					kpos++;
				}
			}
			i = 0;
			if (haypararecup)
			{
				if (kpos == 1)//Solo hay uno para recuperar
					i = tickposic[0];
				else
				{				
					i = mpch->Anima();		
					i = tickposic[i-2];
				}
			}
			delete mpch;
			if(!i)
			{
				Alert("No existe ningun ticket suspendido que pueda",
					"ser recuperado ahora...");
				clear(1);
				return;
			}
		}
		xReg = (double) i;
	}
	sprintf(suspFn, "TICK%d.SUS", (int) xReg);
	if(!srBm->TestBit((unsigned) xReg))
	{
		Alert("El Nro ingresado no esta permitido !!!", "");
		clear(1);
		return;
	}
	if(!Existe(suspFn))
	{
		sprintf(tmsg_, "No existe la suspension nro %d !!!", (int) xReg);
		Alert(tmsg_, "");
		return;
	}
	if(inOper && !recuany)
	{
		if(!GetSiNo("Tiene un ticket a medio confeccionar ...",
			        "Desea que LE AGREGUE la operacion suspendida? "))
		{
			clear(1);
			return;
		}
	}

	//   Finalmente, efectua la recuperacion.
	actSusp = (int) xReg;
    _itoa(actSusp, tmsg, 10);
    CustDisp3("Ticket Recuperado", "Nro", tmsg);
	//SetTableStatus(actSusp, TABLE_INCURSE); MESAS
	SendMsg("Recuperando el ticket Suspendido...", "");
	IncorporeTicket(suspFn);
	remove(suspFn);
	ClearMsg();
	enEspera = chkNoSusp(0);

	// Escribe la recuperacion.
	susp.func = DSuspen;
	susp.comprob = GlobalTF;
	susp.ticknum = pf->GetTickNro(GlobalTF) + 1;
	susp.flag = 0;
	susp.fecha = (long) date();
	susp.hora = (long) time_();
	susp.cajero = ultCajero;
	store(xTotal.VerTotal().ToDouble(), susp.monto);
	WriteDump(&susp);

	clear(1);
}

void suspende_(int);
void suspende(int cual)
{
	GCStatic::LockMainLoop("suspende");
	suspende_(cual);
	GCStatic::ReleaseLockMainLoop("suspende");
}

//  Funcion de proceso de la tecla de suspension.
void suspende_(int dummy)
{
	char tmsg[8];
	struct DSuspende_ susp;

	CustDisp("", "");
	if (!scanartcobradora)
	{
		Alert("No se permite suspender en Cobradora", "Devuelva la Factura!!!");
		return;
	}

	if(!initBM) LoadSuspRange();
	if(inOper != 1)
	{
		if(autonro)
			recupera(dummy);
		else
			Alert("No puede utilizar la tecla pulsada", "En este momento de la operacion !!!");
		return;
	}	

	//  Determina el nro de secuencia/nombre de archivo.
	if(xReg == 0.0 && actSusp != 0)
	{
		xReg = (double) actSusp;
		actSusp = 0;
	}
	if(xReg == 0.0)
	{
		int i;

		if(!autonro && !usegraf)
		{
			Alert("Ingrese el nro a suspender...", "");
			return;
		}
		/*if(usegraf) MESAS
		{
			i = AnimateGraf(actSusp);
			if(!i)
			{
				clear(1);
				return;
			}
		}
		else*/
		if (!usegraf) //MESAS agregado en lugar del ELSE
		{
			for(i = srBm->FirstBit(); i; i = srBm->NextBit())
			{
				sprintf(suspFn, "TICK%d.SUS", i);
				if(!Existe(suspFn))
					break;
			}
			if(!i)
			{
				Alert("Todas las opciones de suspension estan cubiertas.",
					"No puede suspender hasta que recupere un ticket !!!");
				clear(1);
				return;
			}
		}
		xReg = (double) i;
	}
	if(!srBm->TestBit((unsigned) xReg))
	{
		Alert("El Nro ingresado no esta permitido !!!", "");
		clear(1);
		return;
	}

	sprintf(suspFn, "TICK%d.SUS", (int) xReg);
	if(!autoapp && Existe(suspFn))
	{
		sprintf(tmsg_,"Ya existe un ticket suspendido con el nro %d", xReg);
		if(!GetSiNo(tmsg_, "Desea AGREGAR este al anteriormente supendido?"))
		{
			clear(1);
			return;
		}
	}

	//  Imprime un subtotal
	double tickTot = xTotal.VerTotal().ToDouble();
	char tickMsg[40], tickAdic[40]; 
	STRCPY(tickAdic, "&4TKT EN ESPERA");
	STRCPY(tickMsg, "SUBTOTAL");
	pf->PrintItem(tickMsg, 1, tickTot, 0, 'M', 1, 0, tickAdic, tickTot, false, 0);

	//  Finalmente, suspende el ticket.
	//if(actSusp) SetTableStatus(actSusp, TABLE_EMPTY); MESAS
    actSusp = (int) xReg;
	_itoa(actSusp, tmsg, 10);
	//CustDisp3("Ticket Suspendido", "Nro", tmsg);
	//SetTableStatus((int) xReg, TABLE_BUSY); MESAS
	actSusp = 0;
	SendMsg("Espere un momento,", "Suspendiendo el ticket actual...");
	enEspera = 1;
	MoveTicket(suspFn, -1);
	DispDeta(suspMsg1, "");
	sprintf(tmsg_, suspMsg2, (int) xReg);
	DispDeta(tmsg_, "");
	WriteTickTmp("", tmsg_, 0, 0, 0, 0, 0);

	// Elimina el ticket y graba el log.
	susp.func = DSuspen;
	susp.comprob = GlobalTF;
	susp.ticknum = pf->GetTickNro(GlobalTF) + 1;
	susp.flag = 1;
	susp.fecha = (long) date();
	susp.hora = (long) time_();
	susp.cajero = ultCajero;
	store(xTotal.VerTotal().ToDouble(), susp.monto);

	TruncTicket();
	WriteDump(&susp);
	WriteEOPNoFiscal();

	ClearMsg();
}

//   Displaya, para consulta simplemente, el mapa de mesas
void viewmap(int dummy)
{
#ifndef POSXP
	if(!initBM) LoadSuspRange();
	AnimateGraf(actSusp);
#endif
}

void ProcSuspende(void *data, int storeFlag)
{
}

//Se encarga de crear el archivo temporal que se utilizara para guardar la info extraida del SQL
//fname es el nombre del archivo a crear
void CrearTempDbfParaSql(char *fname)
{
#ifdef FACTSQL	
	bool paracrear = false;
	System::String^ mfname = System::String::Empty;
	for (int i = 0; i < 1000; i++)
	{
		sprintf(fname, "fromsql%03d.dbf", i);
		mfname = gcnew System::String(fname);
		if (System::IO::File::Exists(mfname))
		{
			if (TransSql::Importa::TryOpen(mfname))
			{
				System::IO::File::Delete(mfname);
				paracrear = true;
			}
			else
				delete mfname;
		}
		else
			paracrear = true;
		if (paracrear)
		{
			dbf *desql = new dbf();
			int rcrea = desql->creaTable(fname, crStr);
			desql->Close();
			delete desql;
			if (!rcrea)
			{
				if (System::IO::File::Exists(mfname))
					System::IO::File::Delete(mfname);
				System::IO::File::Copy("fromsql.bak", mfname);
			}
			else
				break;
		}
	}	
	delete mfname;
#endif
}

//Realiza el proceso de eleccion de la Factura a cobrar.
void recupera_sql(int cual)
{
#ifdef FACTSQL	

	
	int i;
	static bool inrecusql = false;
	char tmsg[8];
	struct DSuspende_ susp;	

	if (inrecusql)
		return;	
	//   Hace algunas verificaciones.
	CustDisp("",  "");	
	
	if(inOper > 1)
	{
		Alert("Ya ingresó en la fase de pago de un ticket",
			  "No puede recuperar una operacion suspendida ahora");
		return;
	}
	if(chkClk())
	{
		Alert("No hay cajero habilitado todavia!!!", "");
		return;
	}
	if (inOper == 1 && numsecuencia == 0)
	{
		Alert("Existe un ticket en proceso.", "Termine su cobranza");
		return;
	}
	inrecusql = true;
	GCStatic::LockMainLoop("recupera_sql");
	char *archivotmp = new char[30];
	try
	{	
		if (inOper == 1 && numsecuencia > 0)// Si ya tiene una factura para cobrar la devuelve
		{
			if(rMode != 1)//Tiene que haber un supervisor para realizar la devolucion
			{
				Alert("Necesito AUTORIZACION para Devolver Ticket!!!",
					"Llame al supervisor.");
				inrecusql = false;
				return;
			}
			//Cambia el estado del Ticket en SQL para que este disponible para cobrar
			TransSql::Exporta::DevolverTicket(numsecuencia);
			WriteTickTmp("", "Ticket Devuelto para cobrar...", 0, 0, 0, 0, false);
			superv(0);
			TruncTrans();
			hasarEop = 1;
			WriteEOPFiscal(1);		
			ClearMsg();
			elige_pasar = false;
			//superv(0);
		}
		else
		{
			//Cambia a la variacion actual
			CheckNewPlu();
			//Presenta el form para que el cajero elija una factura
			frmFactSql ^formu = gcnew frmFactSql();
			formu->ShowDialog();
			if (formu->DialogResult == System::Windows::Forms::DialogResult::OK && formu->numeroseq > 0)
			{				
				SendMsg("Recuperando el ticket del servidor...", "");
				CrearTempDbfParaSql(archivotmp);			
				//llama al metodo que recupera el Ticket del SQL
				Dump::sqltrans = TransSql::Importa::ToDbf(gcnew System::String(archivotmp), formu->numeroseq, gcnew System::String(saptienda), ckaNro , caja, pf->solofact);
				numsecuencia = formu->numeroseq;
				WriteCmos(NSECUEN);
				//Agrega el ticket al Trans
				IncorporeTicket(archivotmp);		
				ClearMsg();			
			}			
			delete formu;
		}	
	}	
	catch (System::Exception^ excep)
	{
		ClearMsg();
		TransSql::DatosSql::WriteLog(excep, "recupera_sql");
		Alert("Ocurrio un Error al recuperar la Factura", "");
		WriteBootLog("Recupera_Sql Error al recuperar la factura");
	}
	finally
	{	
		GCStatic::ReleaseLockMainLoop("recupera_sql");
		inrecusql = false;
		delete archivotmp;
	}
#endif
}