/*
 *   Corri.c
 *   Efectua la correccion de un renglon.
*/

#include <stdafx.h>
#include "funcs.h"

#ifdef POSXP
#include "StaticDump.h"
#include "ConfCorri.h"
#include "GCStatic.h"
#include "frmAdicPrisma.h"
#else
extern short passesq[], bpluesq[];
#endif

// Prototipos externos
void ReprocTransMayor(DDplu_ *vta, bool anulFlag);

//   Datos Globales.
int    globalCorri;			// Flag para indicar que está corrigiendo.
union  Dump_ act;
struct DDcorri_ d;          // Para dumpear la funcion.
int    autoAnul;			// Indica si le permite a un cajero 'vaciar' el ticket.

//   Efectua la anulacion total del ticket
void DoCorriAll()
{
	double tickTot = xTotal.VerTotal().ToDouble();
	char tickMsg[40], tickAdic[40]; 	
	
	//no imprime ticket cuando sea selfckout
	//if (!selfCheckout) //No es SelfCheckout
	//Va imprimir el ticket incluso en selfCheckout, pedido 16/05/2025 Ana Cabana
	//{
		//Agregado para que imprima los items del ticket cancelado en los mayoristas
		if (prndelay)
		{
			globalDelayPrint = 0;
			ReprintDump(false);
			globalDelayPrint = 1;
		}

		pf->PrintSubtot("", 0);//agregado para impresion del subtotal
		//STRCPY(tickAdic, "&4TKT CANCELADO");
		sprintf(tickAdic, "&4TKT CANCELADO - %s", supervNom);
		STRCPY(tickMsg, "SUBTOTAL");
		pf->PrintItem(tickMsg, 1, tickTot, 0, 'M', 1, 0, tickAdic, tickTot, false, 0);
	//}
	superv(0);
	TruncTrans();

	CustDisp("Ticket ANULADO", "");
	WriteTickTmp("", "************ TICKET ANULADO ************", 0, 0, 0, 0, 0);
	//No imprime ticket cuando es SelfCheckout
	//if (!selfCheckout)//No es selfCheckout
	//Va imprimir el ticket incluso en selfCheckout, pedido 16/05/2025 Ana Cabana
	//{	
		ticket = pf->GetTickNro(GlobalTF);		
		pf->PrintCancel();
	//}
	hasarEop = 1;
	acumSaldoPromo = 0;
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

//   Asienta la correccion (o la reprocesa).
void ProcCorri(void *c_, int fwrite)
{
	Dump_ *que;
    struct DDcorri_ *c;
	unsigned long svrec;
	char corriCodi[50], corriRng[100], tempTotal[30], tempNum[30];

	STRCPY(corriRng, "(Anula) ");
	corriCodi[0] = 0;

    c = (struct DDcorri_ *) c_;
	if(fwrite) WriteDump(c);
    if(!globalInitDump && monitorMode != 0)
	{
		tMonitor.AppendBlank();
		tMonitor.Lock(1);
	}

	GCStatic::AddLockToTrans("CorreccionDePlu");
	svrec = tDbf.Recno();
	globalCorri = 1;
	if(!globalInitDump) que = (Dump_ *) ReadDump(c->posi);
	if(!globalInitDump && (que->f.id == DDepto))
	{		
		 store(atof_(que->depto.xReg) * (-1.0), que->depto.xReg);
		 ProcDepto(&que->depto, 0);
		 xCntAnul++;
		 xTotAnul += atof_(que->depto.xReg) * atof_(que->depto.yReg);

		 if(monitorMode)
		 {
			 sprintf(corriCodi, "Dep:%d", que->depto.nro);
			 dep_ *d = (dep_ *) bsearch(&que->depto.nro, deptos, cDep, sizeof(struct dep_), depcmp2);
			 STRCAT(corriRng, d->des);
			 double x = atof_(que->depto.xReg), y = atof_(que->depto.yReg);
			 store(x * y, tempTotal);
 	 		 tMonitor.Replace("cant", Trim(que->depto.yReg, tempNum));
			 tMonitor.Replace("unitario", Trim(que->depto.xReg, tempNum));
			 tMonitor.Replace("total", Trim(tempTotal));
		 }
	}
	if(!globalInitDump && (que->f.id == DPlu))
	{
		//Agregado para centralizar
		if (Dump::docActual != nullptr){
			Dump::docActual->AnularItem(c->posi);
		}

		 store(atof_(que->plu.yReg) * (-1.0), que->plu.yReg);
		 ProcPlu(&que->plu, 0);
		 xCntAnul++;
		 xTotAnul += atof_(que->plu.xReg) * atof_(que->plu.yReg);

		 acumPeso -= atof(que->plu.pesoenbalanza);

		 ReprocTransMayor(&que->plu, true);

		 if(monitorMode)
		 {
			 STRCPY(corriCodi, padl(que->plu.cod, 6));
		 	 dbplu.Seek(corriCodi);
			 dbplu.Get("des", corriRng + 8);

			 double x = atof_(que->plu.xReg), y = atof_(que->plu.yReg);
			 store(x * y, tempTotal);
 	 		 tMonitor.Replace("cant", Trim(que->plu.yReg, tempNum));
			 tMonitor.Replace("unitario", Trim(que->plu.xReg, tempNum));
			 tMonitor.Replace("total", Trim(tempTotal));
		 }
	}
	globalCorri = 0;
	tDbf.Go(svrec);
	GCStatic::ReleaseLockFromTrans("CorreccionDePlu");

    if(!globalInitDump && monitorMode != 0)
	{
		tMonitor.Replace("codigo", corriCodi);
		tMonitor.Replace("nombre", corriRng);
		tMonitor.Replace("dump", "99999991");
		tMonitor.Lock(0);
		tMonitor.Flush();
	}
}

void corriall_(int);
void corriall(int cual)
{
	GCStatic::LockMainLoop("corriall");
	corriall_(cual);
	GCStatic::ReleaseLockMainLoop("corriall");
}

//   La siguiente funcion procesa la funcion de correccion total, que se
//   utiliza para anular TODA la opeacion en curso.
void corriall_(int dummy)
{
	if(consmode )
		return;
	if (rMode == 1 && inOper == 0)
	{
		Choose *elanul = new Choose("Operaciones de Anulacion", "ESC=Cancela", 20, 0, CHOOSE_ALIGN_LEFT, NULL);
		elanul->Add("Nota de Credito");
		elanul->Add("Anula Tarjeta Newland");
		elanul->Add("Opciones Adic. Newland");
		elanul->Add("Anula Tarjeta SMARTPOINT");
		elanul->Add("Carga Seña de Pago");
		elanul->Add("Anula Tarjeta PrismaECR");
		elanul->Add("Cierre de Lotes PrismaECR");
		//elanul->Add("Hacer Ticket/Factura de Nota Credito");
		//elanul->Add("Anula MercadoPago");
		int rtaanul = elanul->Anima();
		switch(rtaanul)
		{	
			case 2:
				//Alert("Opcion no habilitada.", "");
				NotaCredito(0);
				break;
			case 3:
				if (tjPrisma)
					AnularTarjetaPrisma(1);				
				else
					Alert("Opcion no habilitada.", "");
				break;

			case 4:
				if (tjPrisma){
					frmAdicPrisma ^fadic = gcnew frmAdicPrisma();
					fadic->ShowDialog();
					fadic = nullptr;
				}
				else
					Alert("Opcion no habilitada.", "");
				break;
			case 5:
				if (tjSmartPoint)
					AnularTarjetaSmartPoint(1);
				else
					Alert("Opcion no habilitada.", "");
				break;
			case 6:
				if (CargaSenia)
					CargaSeniaPago(1);
				else
					Alert("Opcion no habilitada.", "");
				break;

			case 7:
				if (EcrPrisma)
					AnularTarjetaPrismaEcr();
				else
					Alert("Opcion no habilitada.", "");
				break;
			case 8:
				if (EcrPrisma)
					CierreLotePrismaEcr();
				else
					Alert("Opcion no habilitada.", "");
				break;
			
		}
		superv(0);
		elanul->Clear();
		elanul = NULL;
		delete elanul;

		acumPeso = 0;

		WriteEOPNoFiscal();
		return;
	}
	
	if (inOper != 1) 
		return;

	/*if (!scanartcobradora)
	{
		Alert("Solo se realizan Cobranzas", "Devuelva la Factura!!!");
		return;
	}	*/

	if (!selfCheckout)
		if(rMode != 1)
		{
			Alert("Necesito AUTORIZACION para anular !!!",
				  "Llame al supervisor.");
			return;
		}

	if(GetSiNo("Esta SEGURO que quiere borrar TODO", "el ticket que esta en progreso?"))
	{
		// Elimina el ticket actual del TRANS.
		DoCorriAll();
	}

	superv(0);
}

// Funcion utilizada para confirmar anulaciones, cuando se eligio el modo de confirmacion
// por el supervisor.
bool ConfirmaCorri()
{
	bool ok = true;

	// Trae las anulaciones pendientes a una tabla y valida que haya algo para anular.
	dbf *corrDb = new dbf();
	AnulacionesPendientes(corrDb);
	if(corrDb->RecCount() > 0)
	{

#ifdef POSXP
		// Prepara los datos para bindearse a la grilla.
		DbfBrowseColumn ^col;
		DbfBrowse ^browser = gcnew DbfBrowse;

		col = gcnew DbfBrowseColumn(IntPtr(corrDb), "codigo", "Codigo", 60);
		col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);
			
		col = gcnew DbfBrowseColumn(IntPtr(corrDb), "nombre", "Nombre", 250);
		col->TextAlignment = DataGridViewContentAlignment::MiddleLeft;
		browser->Columnas->Add(col);

		col = gcnew DbfBrowseColumn(IntPtr(corrDb), "cant", "Cantidad", 60);
		browser->Columnas->Add(col);

		col = gcnew DbfBrowseColumn(IntPtr(corrDb), "unitario", "Unitario", 70);
		col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);
			
		col = gcnew DbfBrowseColumn(IntPtr(corrDb), "total", "Total", 80);
		col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);


		// Muestra la form de confirmacion
		ConfCorri ^f = gcnew ConfCorri(browser, corrDb);
		f->ShowDialog();
		ok = (f->DialogResult == System::Windows::Forms::DialogResult::OK);
#else

		// Codigo para MSDOS
		xBrowse *confCorri = new xBrowse;

		confCorri->xBrowseF(corrDb, "codigo", "Codigo", true)->dataType = 'N';
		confCorri->xBrowseF(corrDb, "nombre", "Nombre", true);
		confCorri->xBrowseF(corrDb, "cant", "Cantidad", true)->dataType = 'N';

		int hWin = wDefine(3, 11, 14, 68, BORDERSINGLE, SNGCURS, passesq, " Confirmando Anulaciones ", " ESC=Cancela, F8=Confirma ");
		int tecla = confCorri->xBrowseAnimate(hWin, corrDb, 0, 1, 1, bpluesq);
		wRelease(hWin);
		delete confCorri;
		ok = (tecla != keycanc);

#endif

	}
	else
	{
		ok = false;
		Alert("No hay anulaciones que esten pendientes de", "ser autorizadas...");
	}

	// Cierra y destruye la tabla con las anulaciones detectadas.
	corrDb->Close();
	delete corrDb;

	// Confirma si corresponde. Al salir, siempre desactiva el modo X1.
	if(ok) ConfirmaAnulacionesEnTrans();
	chgmode(0);
	superv(0);
	return ok;
}

void corrige_(int);
void corrige(int cual)
{
	GCStatic::LockMainLoop("corrige");
	corrige_(cual);
	GCStatic::ReleaseLockMainLoop("corrige");
}

//   La siguiente funcion se utiliza para anular un solo renglon del
//   ticket en progreso.
void corrige_(int dummy)
{
	char tfld[20];
	
	// En consulta no hacemos nada.
	if(consmode) return;

	if (!scanartcobradora)
	{
		Alert("Solo se realizan Cobranzas, No se corrigen art.", "Devuelva la Factura!!!");
		return;
	}

	//  Si estamos en el modo X1 y en el modo de confirmacion, cambia de ruta.
	if(rMode == 1 && corriMode == 1)
	{
		// Si se confirman las correcciones y el total del ticket va a cero, lo anula.
		if(ConfirmaCorri() && xTotal.VerTotal().ToDouble() == 0)
			DoCorriAll();
		return;
	}

	// Si 'inOper' no esta en uno, entonces no hay nada que anular.
	if(inOper != 1) return;

	TicketItem^ titem = Dump::cTicket->Item();

	//  Determina si el renglon actual es anulable o no.
	/*tTmp.Get("anulable", tfld);
	if(tTmp.Eof() || tTmp.Deleted() || tfld[0] != 'T')*/
	if (titem != nullptr && !titem->Anulable)
	{
		Alert("No se puede anular este renglon !!!", "");
		return;
	}

	//if(tTmp.RecCount() == 1 && autoAnul == 0)
	if(Dump::cTicket->RecCount() == 1 && autoAnul == 0)
	{
		Alert("El ticket NO puede quedar vacio.", "Llame al Supervisor y que le Anule el Ticket");
		return;
	}

	//  Extrae desde el 'dump' el valor a anular.
	//tTmp.Get("dump", tfld);
	d.func = DCorri;
	//d.posi = atol(tfld);
	d.posi = titem->Dump;
	d.cajero = ckaNro;
	d.supervisor = (corriMode == 1)?0: supervNro;
	d.fecha = date();
	d.hora = time__();
	Strings::StringToChar(titem->Total, d.monto, 14);
	
	//  Marca la funcion que se esta anulando y anula el registro en el 'dump'.
	GCStatic::AddLockToTrans("CorreccionDePlu-corri");
	tDbf.Go(d.posi);
	tDbf.Get("func", tfld);
	d.anula = atoi(tfld);
	STRCPY(tfld, "T");
	tDbf.Replace("anulado", tfld);
	GCStatic::ReleaseLockFromTrans("CorreccionDePlu-corri");

	/*tTmp.Delete();
	tTmp.Pack();
	tTmp.Go(DB_GO_BOTTOM);*/
#ifdef POSXP
	//Dump::Browse->Refresh();
	Dump::cTicket->EliminaItem(titem->Dump);
#else
	bDet.dRefresh();
#endif

	//   Registra la anulacion en el dump.
	DispDeta("--------------- Anulado ---------------", "");
	ProcCorri(&d, 1);
	DispDeta("---------------------------------------", "");
	inOper = 1;
	superv(0);
	clear(1);

	//   Si el ticket quedo TOTALMENTE ANULADO, entonces resetea al pos. El ticket temporario tiene
	//   que haber quedado con 0 registros.
	//if(tTmp.RecCount() == 0) 
	if (Dump::cTicket != nullptr && Dump::cTicket->RecCount() == 0)
		DoCorriAll();
	else
		PrintTktTot();
}

//   Esta funcion es usada desde plu.cpp y depto.cpp
void Acomoda(char *dst, char *izq, char *der)
{
	 int l;

	 STRCPY(dst, Trim(izq));
	 while(strlen(dst) < 39) STRCAT(dst, " ");
	 l = strlen(Trim(der));
	 STRCPY(dst + (39 - l), der);
}

