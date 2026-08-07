/*
 *   Factu.c
 *   Funciones relacionadas con facturacion, precios alternativos y mayoristas.
*/

#include <stdafx.h>
#include "funcs.h"

#ifdef POSXP
#include "PrecManual.h"
#include "StaticDump.h"
#include "frmNroNotaCredito.h"
#include "frmSeniaMP.h"
#include "MpagCliente.h"
#endif

// Prototipos
void ReprocActualTran();			// dump.cpp
void ResetPOSAcumInternal(void);	// dump.cpp

// Variables del modulo
int PermFactuFlag, reparto_nro,  reparto_perm, modoMayor;
dbf dbcomp, *dbmayor;
bool globalRedraw = false;
char *ComprobName = new char[22];
int RngLimit, clavePrecios;
static Choose *hChooseFactu = NULL;
static Choose *hChoosePrec = NULL;
::ArrayList *mayorTkt;

int AplicarPercep;
int AplicarTissh;
int AplicarPerIva;
double MinimoTissh;
//   Al inicio del POS, si esta habilitado el modo de proceso mayorista, construye las tablas
//   de control que necesitara.
void InitMayorHandling()
{
	// Valida el modo mayorista
	if(prndelay == 0 || modoMayor == defListaPrec) modoMayor = 0;
	if(!modoMayor) return;

	// Crea la tabla de control que usaremos
	mayorTkt = new ::ArrayList(true, "InitMayorHandling");
	globalRedraw = false;
}

//   Al FINAL de cada ticket, elimina todos los calculos intermedios y vacia las tablas de control usadas.
void ResetMayor()
{
	if(!modoMayor) return;
	mayorTkt->Clear();
	globalRedraw = false;
}

class TktItemResu
{
public:
	unsigned long cod;
	double cant;
	double cant1;//agregado para cantidad por 3 unidades
	//double cant2;	
	double uni;
	bool alpeso;
	bool porBalanza;
	bool tienePVM;
	int prec;
};
//class TktItemResu
//{
//public:
//	unsigned long cod;
//	double cant;
//	double cant2;
//	double uni;
//	int prec;
//};

// Determina el precio a usar, para el articulo en cuestion.
int PrecioMayor(unsigned long cod)
{
	TktItemResu *res = (TktItemResu *)mayorTkt->Find(cod);
	if(res == NULL) return listap;
	return res->prec;
}

////   Despues de concretada una venta, reprocesa el trans para ver si aplican algunos de los beneficios.
//void ReprocTransMayor(struct DDplu_ *vta, bool anulFlag)
//{
//	bool redoTicket = false, redoTicket2 = false;
//	if(modoMayor == 0 || globalRedraw) return;
//	if(precioBulto <= 0)
//	{
//		if(!modoMayor || vta->dun14) return;
//	}
//	if(!anulFlag && (atof(vta->yReg) < 0)) return;
//
//	// Busca el articulo en el resumen del ticket que llevamos.
//	TktItemResu *res = (TktItemResu *)mayorTkt->Find(vta->cod);
//	if(res == NULL)
//	{
//		// Posiciona en el PLU que corresponde.
//		sprintf(msg, "%06d", vta->cod);
//		dbplu.Seek(msg);
//		dbplu.Get("cant2", msg);
//		int cant2 = atoi(msg);
//		if(cant2 <= 0) cant2 = 99999999;
//        dbplu.Get("uni", msg);
//		int uni = atoi(msg);
//		if(uni <= 1) uni = 1;
//
//		// Rellena la estructura de control
//		res = new TktItemResu();
//		res->cod = vta->cod;
//		res->cant2 = cant2;
//		res->uni = uni;
//		res->prec = defListaPrec;
//		mayorTkt->Add((int)vta->cod, res);
//	}
//
//	// Detecta si hay cambios de importancia en el ticket
//	double finalCantToDisplay = atof(vta->yReg);
//	if(anulFlag)
//	{
//		// (La cantidad en vta->yReg esta en negativo...)
//		redoTicket = (modoMayor > 0) && (res->cant + atof(vta->yReg) < res->cant2) && (res->cant >= res->cant2);
//		redoTicket2 = (precioBulto > 0) && (res->cant + atof(vta->yReg) < res->uni) && (res->cant >= res->uni);
//		res->cant += atof(vta->yReg);
//		if(redoTicket2)
//		{
//			if((res->cant + atof(vta->yReg) >= res->cant2))
//				res->prec = modoMayor;
//			else
//				res->prec = listap;
//		}
//		else if(redoTicket) 
//			res->prec = listap;
//		finalCantToDisplay *= (-1);
//	}
//	else
//	{
//		redoTicket = (modoMayor > 0) && (res->cant < res->cant2) && (res->cant + atof(vta->yReg) >= res->cant2);		
//		redoTicket2 = (precioBulto > 0) && (res->uni > 1) && (res->cant < res->uni) && (res->cant + atof(vta->yReg) >= res->uni);
//		
//		res->cant += atof(vta->yReg);
//		if(redoTicket2) 
//			res->prec = precioBulto;
//		else if(redoTicket) 
//			res->prec = modoMayor;
//	}
//	return;
//	if(!redoTicket && !redoTicket2) return;
//
//	// Determina el nuevo precio del articulo, a fin de parchar las entradas que correspondan en el trans.
//	char _cod[30];
//	sprintf(_cod, "%06d", vta->cod);
//	dbplu.Seek(_cod);
//	if(res->prec == 1)
//		dbplu.Get("precio", msg);
//	else
//	{
//		sprintf(_cod, "precio%d", res->prec);
//		dbplu.Get(_cod, msg);
//	}
//	double uPrec = atof(msg);
//
//	// Reprocesa el trans para recalcular los totales y redibujar los renglones.
//	GCStatic::AddLockToTrans("PrecioMayor");
//	globalRedraw = true;
//	ResetPOSAcumInternal();
//	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
//	{
//		if(!tDbf.Deleted())
//		{
//			// Lee info dsde el trans
//			char _func[10], _cod[20], _anul[5], _cant[20], _dun14[5];
//			tDbf.Get("func", _func); tDbf.Get("cod", _cod); tDbf.Get("anulado", _anul); tDbf.Get("flag", _dun14);
//			if(atoi(_func) == 4 && atoi(_cod) == vta->cod && _anul[0] != 'T' && _dun14[0] != 'T')
//			{
//				// Trae los campos que faltan.
//				tDbf.Get("cant", _cant);
//
//				// Reemplaza entradas en el trans
//				sprintf(msg, "%d", (int)(uPrec * 100));
//				tDbf.Replace("vuelto", msg);
//				sprintf(msg, "%.3lf", uPrec * atof(_cant));
//				tDbf.Replace("imp", msg);
//
//				// Reemplaza entradas en el ticket temporario
//				/*if(tTmp.Opened())
//				{*/
//					sprintf(msg, "%d", tDbf.Recno());
//					int ndump = atoi(msg);
//					sprintf(msg, "%8.2lf", uPrec);
//					String^ nunit = gcnew String(msg);
//					sprintf(msg, "%9.2lf", uPrec * atof(_cant));
//					String^ ntot = gcnew String(msg);
//					Dump::cTicket->ReimprimeItem(ndump, nunit, ntot);
//					/*if(tTmp.Locate("dump", msg))
//					{
//						sprintf(msg, "%8.2lf", uPrec);
//						tTmp.Replace("unitario", msg);
//						sprintf(msg, "%9.2lf", uPrec * atof(_cant));
//						tTmp.Replace("total", msg);
//					}*/
//				//}
//			}
//		}
//
//		// Reprocesa el renglon.
//		ReprocActualTran();
//	}
//
//	globalRedraw = false;
//	GCStatic::ReleaseLockFromTrans("PrecioMayor");
//	PrintTktTot(true);
//	DispCash(finalCantToDisplay * uPrec, 2);
//}

//   Despues de concretada una venta, reprocesa el trans para ver si aplican algunos de los beneficios.
void ReprocTransMayor(struct DDplu_ *vta, bool anulFlag)
{
	//bool redoTicket = false, redoTicket1 = false, redoTicket2 = false, redoPesable = false;
	bool redoTicket = false, redoTicket1 = false, redoPesable = false, polVenMay = false;
	if(modoMayor == 0 || globalRedraw) return;
	/*if(precioBulto <= 0)
	{
		if(!modoMayor || vta->dun14) return;
	}*/
	if(!anulFlag && (atof(vta->yReg) < 0)) return;

	// Busca el articulo en el resumen del ticket que llevamos.
	TktItemResu *res = (TktItemResu *)mayorTkt->Find(vta->cod);
	if(res == NULL)
	{
		// Posiciona en el PLU que corresponde.
		sprintf(msg, "%06d", vta->cod);
		dbplu.Seek(msg);
		dbplu.Get("cf2", msg);
		double cant1 = atof(msg);
		if (cant1 <= 0) 
			cant1 = 99999999;		
		
        dbplu.Get("uni", msg);
		int uni = atoi(msg);
		if(uni <= 1) uni = 1;
		//actualizo cb2		
		dbplu.Get("al_peso", msg);
		bool alpeso = (msg[0] == 'T')? 1 : 0;

		dbplu.Get("balanza", msg);
		bool porBalanza = (msg[0] == 'T')? 1 : 0;

		dbplu.Get("pvm", msg);
		polVenMay = atoi(msg) == 0;
		if (modoMayor && polVenMay){
			cant1 = 99999999;
		}

		// Rellena la estructura de control
		res = new TktItemResu();
		res->tienePVM = !polVenMay;
		res->cod = vta->cod;
		res->cant1 = cant1;		
		res->uni = uni;
		res->alpeso = alpeso;		
		res->porBalanza = porBalanza;
		res->prec = defListaPrec;
		mayorTkt->Add((int)vta->cod, res);
	}

	// Detecta si hay cambios de importancia en el ticket
	double finalCantToDisplay = atof(vta->yReg);
	if (res->tienePVM){
		if(anulFlag)
		{			
			// (La cantidad en vta->yReg esta en negativo...)
			redoTicket = (modoMayor > 0) && ((res->cant + atof(vta->yReg)) < res->cant1) && (res->cant >= res->cant1);		
			//redoTicket1 = (modoMayor > 0 && precioX3Un > 0) && (res->cant + atof(vta->yReg) < res->cant2) && (res->cant >= res->cant2) && (res->cant2 != 99999999);		
			redoTicket1 = (modoMayor > 0 && precioX3Un > 0) && ((res->cant + atof(vta->yReg)) >= res->cant1) && (res->cant1 != 99999999);		
			//redoTicket2 = (modoMayor > 0 && precioBulto > 0) && ((res->cant + atof(vta->yReg)) >= res->cant2) && (res->cant2 != 99999999);
			res->cant += atof(vta->yReg);

			if (redoTicket1)
				res->prec = precioBulto; //precio2 x 3 unidades
			else if(redoTicket) 
				res->prec = precioX3Un; //precio4 x 1 unidad
			//setea por demas el precio en los de balanza
			if (mayPrecPesable && res->alpeso){
				if (res->cod == 32786){
					if (res->cant >= res->cant1)
						res->prec = precioBulto;
					else 
						res->prec = precioX3Un;				
				}
				else if (!res->porBalanza){
					if (res->cant >= res->cant1)
						res->prec = precioBulto;
					else 
						res->prec = precioX3Un;
				}
				else{
					if (res->cant >= 1){
						if (precioBulto > 0)
							res->prec = precioBulto;
						else 
							res->prec = precioX3Un;				
					}
					else
						res->prec = precioX3Un;
				}
				redoPesable = true;
			}

			///	res->prec = 2;
			finalCantToDisplay *= (-1);
		}
		else
		{
			redoTicket = (modoMayor > 0) && (res->cant < res->cant1) && ((res->cant + atof(vta->yReg)) < res->cant1);		
			redoTicket1 = (modoMayor > 0) && ((res->cant + atof(vta->yReg)) >= res->cant1) && (res->cant1 != 99999999);			
			
			res->cant += atof(vta->yReg);		
			if (redoTicket1)
				res->prec = precioBulto; //precio2 x 3 unidades
			else if(redoTicket) 
				res->prec = precioX3Un; //precio4 x 1 unidad
			//setea por demas el precio en los de balanza
			if (mayPrecPesable && res->alpeso)
			{
				if (res->cod == 32786){
					if (res->cant >= res->cant1)
						res->prec = precioBulto;
					else 
						res->prec = precioX3Un;				
				}
				else if (!res->porBalanza){
					if (res->cant >= res->cant1)
						res->prec = precioBulto;
					else 
						res->prec = precioX3Un;
				}
				else{
					if (res->cant >= 1){
						if (precioBulto > 0)
							res->prec = precioBulto;
						else 
							res->prec = precioX3Un;				
					}
					else
						res->prec = precioX3Un;
				}
				redoPesable = true;
			}
		}
	}
	//return;
	if(!redoTicket && !redoTicket1 && !redoPesable) return;
	if (Inicializando) return;

	// Determina el nuevo precio del articulo, a fin de parchar las entradas que correspondan en el trans.
	char _cod[30];
	sprintf(_cod, "%06d", vta->cod);
	dbplu.Seek(_cod);
	if(res->prec == 1)
		dbplu.Get("precio", msg);
	else
	{
		sprintf(_cod, "precio%d", res->prec);
		dbplu.Get(_cod, msg);
	}
	double uPrec = atof(msg);
	System::Decimal decprec = 0, decimp = 0, deccant = 0;
	decprec = System::Convert::ToDecimal(uPrec);

	// Reprocesa el trans para recalcular los totales y redibujar los renglones.
	GCStatic::AddLockToTrans("PrecioMayor");
	globalRedraw = true;
	ResetPOSAcumInternal();
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		if(!tDbf.Deleted())
		{
			// Lee info dsde el trans
			char _func[10], _cod[20], _anul[5], _cant[20], _dun14[5], _iva[5];
			tDbf.Get("func", _func); tDbf.Get("cod", _cod); tDbf.Get("anulado", _anul); tDbf.Get("flag", _dun14);
			if(atoi(_func) == 4 && atoi(_cod) == vta->cod && _anul[0] != 'T' && _dun14[0] != 'T')
			{
				// Trae los campos que faltan.
				tDbf.Get("cant", _cant);

				// Reemplaza entradas en el trans
				//sprintf(msg, "%d", (int)(uPrec * 100));
				sprintf(msg, "%d", (int)(decprec * 100));
				tDbf.Replace("vuelto", msg);
				deccant = System::Convert::ToDecimal(atof(_cant));
				decimp = System::Decimal::Round(decprec * deccant, 2);

				//sprintf(msg, "%.3lf", uPrec * atof(_cant));
				sprintf(msg, "%.3lf", System::Convert::ToDouble(decimp));
				tDbf.Replace("imp", msg);				

				struct DDplu_ paux;
				tDbf.Get("cant", paux.yReg);
				tDbf.Get("imp", paux.xReg);
				tDbf.Get("iva", _iva);
				paux.iva = atoi(_iva);
				paux.apliPerIva = 0;
				CalcIvaSap(&paux);
				tDbf.Replace("impiva", paux.impiva);

				// Reemplaza entradas en el ticket temporario
				sprintf(msg, "%d", tDbf.Recno());
				int ndump = atoi(msg);
				sprintf(msg, "%8.2lf", uPrec);
				String^ nunit = gcnew String(msg);
				sprintf(msg, "%9.2lf", uPrec * atof(_cant));
				String^ ntot = gcnew String(msg);
				if (Dump::cTicket != nullptr)
					Dump::cTicket->ReimprimeItem(ndump, nunit, ntot);
			}
		}

		// Reprocesa el renglon.
		ReprocActualTran();
	}

	/*if(!globalCobraCC && inOper <= 1)
	{
		FactuReca(0);
		FactuPercep(0, false);
		FactuPercepTissh(0, false);
		FactuPercepIva(0, false);
	}*/

	globalRedraw = false;
	GCStatic::ReleaseLockFromTrans("PrecioMayor");
	PrintTktTot(true);
	DispCash(finalCantToDisplay * uPrec, 2);
}

void MostrarMayorEnPantalla(){
	// Determina el nuevo precio del articulo, a fin de parchar las entradas que correspondan en el trans.
	

	if (modoMayor == 0)
		return ;

	double finalCantToDisplay = 0;
	char _cod[30];

	// Reprocesa el trans para recalcular los totales y redibujar los renglones.
	GCStatic::AddLockToTrans("PrecioMayor");
	globalRedraw = true;
	ResetPOSAcumInternal();

	// Examina todas las promociones.
	for(int i=0; i<mayorTkt->Count(); i++)
	{
		TktItemResu *res = (TktItemResu *) mayorTkt->Item(i);
		sprintf(_cod, "%06d", res->cod);
		dbplu.Seek(_cod);
		if(res->prec == 1)
			dbplu.Get("precio", msg);
		else
		{
			sprintf(_cod, "precio%d", res->prec);
			dbplu.Get(_cod, msg);
		}
		double uPrec = atof(msg);
		System::Decimal decprec = 0, decimp = 0, deccant = 0;
		decprec = System::Convert::ToDecimal(uPrec);

		for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
		{
			if(!tDbf.Deleted())
			{
				// Lee info dsde el trans
				char _func[10], _cod[20], _anul[5], _cant[20], _dun14[5], _iva[5];
				tDbf.Get("func", _func); tDbf.Get("cod", _cod); tDbf.Get("anulado", _anul); tDbf.Get("flag", _dun14);
				if(atoi(_func) == 4 && atoi(_cod) == res->cod && _anul[0] != 'T' && _dun14[0] != 'T')
				{
					// Trae los campos que faltan.
					tDbf.Get("cant", _cant);

					// Reemplaza entradas en el trans
					//sprintf(msg, "%d", (int)(uPrec * 100));
					sprintf(msg, "%d", (int)(decprec * 100));
					tDbf.Replace("vuelto", msg);
					deccant = System::Convert::ToDecimal(atof(_cant));
					decimp = System::Decimal::Round(decprec * deccant, 2);

					//sprintf(msg, "%.3lf", uPrec * atof(_cant));
					sprintf(msg, "%.3lf", System::Convert::ToDouble(decimp));
					tDbf.Replace("imp", msg);				

					struct DDplu_ paux;
					tDbf.Get("cant", paux.yReg);
					tDbf.Get("imp", paux.xReg);
					tDbf.Get("iva", _iva);
					paux.iva = atoi(_iva);
					paux.apliPerIva = 0;
					CalcIvaSap(&paux);
					tDbf.Replace("impiva", paux.impiva);

					// Reemplaza entradas en el ticket temporario
					sprintf(msg, "%d", tDbf.Recno());
					int ndump = atoi(msg);
					sprintf(msg, "%8.2lf", uPrec);
					String^ nunit = gcnew String(msg);
					sprintf(msg, "%9.2lf", uPrec * atof(_cant));
					String^ ntot = gcnew String(msg);
					if (Dump::cTicket != nullptr)
						Dump::cTicket->ReimprimeItem(ndump, nunit, ntot);
				}
				// Reprocesa el renglon.
				//ReprocActualTran();
			}
		}
	}

	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		if(!tDbf.Deleted())
		{					
			// Reprocesa el renglon.
			ReprocActualTran();
		}
	}
			
	/*if(!globalCobraCC && inOper <= 1)
	{
		FactuReca(0);
		FactuPercep(0, false);
		FactuPercepTissh(0, false);
		FactuPercepIva(0, false);
	}*/

	globalRedraw = false;
	GCStatic::ReleaseLockFromTrans("PrecioMayor");
	PrintTktTot(true);
	//DispCash(finalCantToDisplay * uPrec, 2);
}

//   Retorna el nombre del comprobante.
char *NameComp()
{
	return ComprobName;
}

//   Retorna el nombre de la lista de precios actual.
char *NameLista()
{
	switch(listap)
	{
		case 1: return "Precio 1";
		case 2: return "Precio 2";
		case 3: return "Precio 3";
		case 4: return "Precio 4";
		case 5: return "Prec.Manual";
	}

	listap = 1;
	return "Precio 1";
}

//   La siguiente funcion es la que se llama al deprimirse la tecla de  FACTURA.
void factura(int dummy)
{
	 static struct DDFactu1_ f1;
	 int sel, i;
	 static char f_nro[4], f_des[24];

	 // Muestra el menú donde se eligirá el tipo de comprobante.
	 dbcomp.Use("pos\\comprob");
	 i = dbcomp.RecCount();
	 if(i>20) i = 20;
	 if(!hChooseFactu)
		hChooseFactu = new Choose("Comprobantes", "", 0, 0, CHOOSE_ALIGN_CENTER, NULL);
	 else
		hChooseFactu->Clear();
	 
	 // Llena el choose.
	 dbcomp.Go(DB_GO_TOP);
	 while(!dbcomp.Eof())
	 {
		 dbcomp.Get("des", f_des);
		 hChooseFactu->Add(f_des);
		 dbcomp.Skip(1);
	 }

	 sel = hChooseFactu->Anima();
	 if(!sel)
	 {
		 dbcomp.Close();
		 return;
	 }
	 dbcomp.Go(DB_GO_TOP);
	 while(!dbcomp.Eof())
	 {
		if((int) dbcomp.Recno() == sel)
			break;
		dbcomp.Skip(1);
	 }
	 dbcomp.Get("nro", f_nro);
	 dbcomp.Get("des", f_des);
	 dbcomp.Close();

	 if(sel > 1 && (GlobalRngCount >= RngLimit))
	 {
		 sprintf(msg, "No puede tener mas de %d Renglones", RngLimit);
		 Alert("NO puede pasar a imprimir factura",
			    msg);
		 return;
	 }
	 if(sel == FactuFlag)			// Ya estaba - Elige para siempre?
	 {
		 sprintf(msg, "Ya esta en modo '%s'", ComprobName);
		 if(GetSiNo(msg, "Desea activarlo en forma permanente?"))
		 {
			 f1.func = DFactu1;
			 f1.modo = 999;
			 ProcFactu1(&f1, 1);
		 }
	 }
	 else
	 {
		f1.func = DFactu1;
		f1.modo = sel;
		ProcFactu1(&f1, 1);
	 }
 	 STRCPY(ComprobName, f_des);
}

//   Verifica que no se pase de la cantidad de renglones.
int chkItemCount()
{
	if(FactuFlag < 1) return 0;
	if(GlobalRngCount >= RngLimit)
	{
		sprintf(msg, "No puede tickear masde %d renglones.", RngLimit);
		Alert("DEBE cerrar el comprobante", msg);
		return 1;
	}
	return 0;
}

//   Procesa el cambio de Tipo de precio.
void ProcOtrop(void *op_, int fwrite)
{
    struct DOtrop_ *op;

    op = (struct DOtrop_ *) op_;
    if(fwrite) WriteDump(op);
	listap = op->nprec;
	listap_perm = (int) op->permFlag;
	AdicClear();
}

//   Procesa el cambio de modo de reparto.
void ProcRepar(void *re_, int fwrite)
{
    struct DRepar_ *re;

    re = (struct DRepar_ *) re_;
    if(fwrite) WriteDump(re);
	reparto_nro = re->nro;
	reparto_perm = re->permFlag;
	AdicClear();
}

//   La siguiente funcion es usada internamente cuando se quiere for-
//   zar el cambio de lista de precios.
void ExecOtroPrec(int nPrec, int permFlag)
{
	 static struct DOtrop_ op;

	 nPrec--;
	 if((nPrec < 1 || nPrec > 5) || (nPrec == listap)) return;
	 if(nPrec != defListaPrec)
	 {
		 switch(clavePrecios)
		 {
			 // Solicitar clave de gerente para CUALQUIER precio
			case 1:
				if(!RequestPasswd("pman")) return;
				break;

			// Solicitar clave de gerente solo para precio manual
			case 2:
				if(nPrec == 5)
				{
					if(!RequestPasswd("pman")) return;
				}
				break;
		 }
	 }

	 op.func = DDOtrop;
	 op.nprec = nPrec;
	 op.permFlag = permFlag? 1: 0;
	 ProcOtrop(&op, 1);
}

//   La siguiente funcion es la que se llama al deprimirse la tecla de OTRO PRECIO.
void otroprec(int permFlag)
{
	 int sel;

	 if(rMode > 1)  // Valida que sea posible
	 {
		Alert("No puede cambiar de lista en este momento.","");
		return;
	 }

	 if(hChoosePrec == NULL)
	 {
		hChoosePrec = new Choose("Precios", "", 0, 0, CHOOSE_ALIGN_CENTER, NULL);
		hChoosePrec->Add("Precio 1");
		hChoosePrec->Add("Precio 2");
		hChoosePrec->Add("Precio 3");
		hChoosePrec->Add("Precio 4");
		hChoosePrec->Add("Precio Manual");
	 }
	 
	 sel = hChoosePrec->Anima();
	 if(!sel) return;			// No eligio nada.
	 ExecOtroPrec(sel, permFlag);
}


//   La siguiente funcion es la que se llama, cuando se pulsa la
//   tecla de reparto
void reparto(int permFlag)
{
	static DRepar_ re;

	//   Pide los datos del cliente para la entrega.
	if(!GlobalCCSelected)
	{
		chkMaxSal = 0;
		if(!ForceCC(1))
		{
			chkMaxSal = 1;
			return;
		}
		chkMaxSal = 1;
	}

	//   Procesa el requerimiento.
	re.func = DReparto;
	re.nro = 1;
	re.permFlag = permFlag;
	ProcRepar(&re, 1);
}

//   Cambia el modo de facturacion.
void chgFactuMode(int nMode)
{
	//   ARREGLAR ESTE CODIGO PARA QUE FUNCIONE CON LOS CAMBIOS DE TICKET FACTURA...
	//   RECIBIA EL PARAMETRO 'nMode' CON EL TIPO DE COMPROBANTE NUMERICO...

	/*
	if(FactuFlag == nMode) return;
	dbcomp.Use("pos\\comprob");
	if(!dbcomp.Locate("nro", a_nro))
	{
		dbcomp.Close();
		return;
	}
	if((FactuFlag == 0 || FactuFlag == 1) && nMode > 1)
		pf->PrintCancel();
	if(FactuFlag > 1 && nMode <= 1)
	{
		FactuFlag = nMode;
		if(FactuFlag == 1) FactuFlag = 0;
		ReprintDump();
	}
	FactuFlag = nMode; if(FactuFlag == 1) FactuFlag = 0;
	dbcomp.Get("des", ComprobName);
	dbcomp.Close();
	*/
}

/*
 *   Procesa el cambio de Modo de Facturacion.
*/
void ProcFactu1(void *fac1_, int fwrite)
{
    struct DDFactu1_ *fac1;

    fac1 = (struct DDFactu1_ *) fac1_;
    if(fwrite) WriteDump(fac1);
	if(fac1->modo == 999)
		PermFactuFlag = 1;
	else
	{
		chgFactuMode(fac1->modo);
		PermFactuFlag = 0;
	}
	if(fwrite) AdicClear();
}

/*
 *   En caso que se haya decidido por un cliente NO inscripto, la siguiente
 *   funcion devuelve el importe que se debe ADICIONAR al total del ticket,
 *   para cobrarle al cliente.
*/

double FactuReca(int impri)
{
	double vl;
	static struct DDFactu2_ fac2;

	if(FactuFlag != 2 && !GlobalTF) return 0.0;
	//if(GlobalTF && f_respon != 'R') return 0.0; c_condiva, "NI"
	if(GlobalTF && _stricmp(c_condiva, "NI") ) return 0.0; 
	vl = RetSobreTasa();

	if(impri)
	{
		sobret += vl;
		WriteCmos(SOBRET);
		DispDeta("RECARGO N.I.", HRng(vl));
		pf->PrintTick("");

		// Hace un registro en el dump, que es solo readonly.

		fac2.func = DFactu2;
		store(vl, fac2.ni);
		ProcFactu2(&fac2, 1);
	}
	return vl;
}

double FactuPercep(int impri, bool enpantalla)
{
	BDecimal vl;
	static struct DDFactu3_ fac3;
	if (!AplicarPercep)	return 0.0;
	if (!GlobalTF || strcmp(c_condiva, "CF") == 0) return 0.0;// && ((strcmp(c_condiva, "RI") != 0 && strcmp(c_condiva, "MO") != 0 ))) return 0.0;
	
	if (strcmp(c_percep, "S") != 0) return 0.0;	

	if (FactuFlag != 2 && !GlobalTF) return 0.0;
	if (atof(c_alicuota) <= 0.0) return 0.0;
	
	long fhoy = (long) date();
	long lfpercep = atol(c_fechapib);
	if (lfpercep > 0 && fhoy < lfpercep) return 0.0;	

	double lpercep = atof(c_alicuota) / 100;
	static double montosinpercep;
	if (!baplipercep)
	{
		montosinpercep = 0;
		if (strcmp(c_condiva, "RI") == 0)
		{
			double lpnetos = 0;
			for(int i=0; i<4; i++)
			{
				lpnetos += tNetos[i];				
			}
			lpnetos = floor(lpnetos * 100) / 100;
			lpnetos += ValueIvaPromo();
			montosinpercep = lpnetos;
			vl = lpnetos * lpercep;
		}else
		{
			montosinpercep = xTotal.VerPendienteDouble();
			vl = xTotal.VerPendienteDouble() * (lpercep);
		}
	
		vl = vl.Round(2);
		xTotal.AddRecargo(10002, vl);
		baplipercep = true;
	}
	if(impri)
	{
		sprintf(fac3.montoneto, "%.3lf", montosinpercep);
		vl = xTotal.VerRecargo(10002);		
		vl = vl.Round(2);
		baplipercep = false;		

		fac3.func = DFactu3;
		store(vl.ToDouble(), fac3.percep);
		*fac3.alicuota = 0;
		fac3.cajero = ckaNro;
		sprintf(fac3.alicuota, "%s", c_alicuota);
		ProcFactu3(&fac3, 1);
		if (enpantalla)
		{
			sprintf(c_leyenpercep, "IIBB RG 974 Jujuy   %s", c_alicuota);		
			WriteTickTmp("", c_leyenpercep, 0,	0, 0, vl.ToDouble(), 0);		
			if(!globalDelayPrint)		
				PrintFactu3(&fac3);		
		}
	}
	return vl.ToDouble();
}

void PrintFactu3(void *fac3_)
{
	struct DDFactu3_ *fac3;

   fac3 = (struct DDFactu3_ *) fac3_;	 
	*c_leyenpercep = 0;
	strcat(c_leyenpercep, "IIBB RG 974 Jujuy"); //strcat(c_leyenpercep, "IIBB RG 974 Jujuy   ");
	//strcat(c_leyenpercep, fac3->alicuota);
	double imppercep = atof(fac3->percep);
	if (!imprimiopercep)
		imprimiopercep = pf->PrintPercep(c_leyenpercep, imppercep, atoi(fac3->alicuota), atof(fac3->montoneto)); // 0);	 
}

//Percepcion TISSH Salta
double FactuPercepTissh(int impri, bool enpantalla)
{
	BDecimal vl;
	static struct DDFactu4_ fac4;
	//deshabilita la percepcion
	//return 0.00;
	if (!AplicarTissh)	return 0.0;
	if (!GlobalTF || ((strcmp(c_condiva, "RI") != 0 && strcmp(c_condiva, "MO") != 0 ))) 
		return 0.0;
	double totOper = xTotal.VerTotal().ToDouble();

	if (totOper < MinimoTissh)
		return 0.0;

	if (strcmp(c_tisshpercep, "S") != 0 && strcmp(c_codpostal, "4400") != 0 ) return 0.0;	

	if (FactuFlag != 2 && !GlobalTF) return 0.0;
	if (atof(c_tisshalic) <= 0.0) return 0.0;
	
	double lpercep = atof(c_tisshalic) / 100;
	static double montosintissh;
	if (!baplitissh)
	{
		montosintissh = 0;
		
		double lpnetos = 0;
		for(int i=0; i<4; i++)
		{
			lpnetos += tNetos[i];				
		}
		lpnetos = floor(lpnetos * 100) / 100;
		lpnetos += ValueIvaPromo();
		montosintissh = lpnetos;
		vl = lpnetos * lpercep;
		

		vl = vl.Round(2);
		xTotal.AddRecargo(10003, vl);
		baplitissh = true;
	}
	if(impri)
	{
		sprintf(fac4.montoneto, "%.3lf", montosintissh);
		vl = xTotal.VerRecargo(10003);		
		vl = vl.Round(2);
		baplitissh = false;		

		fac4.func = DFactu4;
		store(vl.ToDouble(), fac4.percep);
		*fac4.alicuota = 0;
		fac4.cajero = ckaNro;
		sprintf(fac4.alicuota, "%s", c_tisshalic);
		ProcFactu4(&fac4, 1);
		if (enpantalla)
		{
			sprintf(c_leyenpercep, "TISSH Salta  %s", c_tisshalic);		
			WriteTickTmp("", c_leyenpercep, 0,	0, 0, vl.ToDouble(), 0);		
			if(!globalDelayPrint)		
				PrintFactu4(&fac4);		
		}
	}
	return vl.ToDouble();
}

void PrintFactu4(void *fac4_)
{
	struct DDFactu4_ *fac4;

	fac4 = (struct DDFactu4_ *) fac4_;	 
	*c_leyenpercep = 0;
	strcat(c_leyenpercep, "TISSH Salta   ");
	strcat(c_leyenpercep, fac4->alicuota);
	double imppercep = atof(fac4->percep);
	if (!imprimiotissh)
		imprimiotissh = pf->PrintPercep(c_leyenpercep, imppercep, 0, 0);	 
}

//Percepcion IVA solo RI a partir de 01/04/2023
double FactuPercepIva(int impri, bool enpantalla)
{
	BDecimal vl;
	static struct DDFactu5_ fac5;
	//deshabilita la percepcion
	//return 0.00;
	if (!AplicarPerIva)	return 0.0;
	if (!GlobalTF || ((strcmp(c_condiva, "RI") != 0))) 
		return 0.0;	

	//El cliente esta exento para la aplicacion del impuesto
	if (Dump::actCliente == nullptr)
		return 0.0;
	if (Dump::actCliente->NoPercepIVA)
		return 0.0;

	//if (strcmp(c_tisshpercep, "S") != 0 && strcmp(c_codpostal, "4400") != 0 ) return 0.0;	

	//if (FactuFlag != 2 && !GlobalTF) return 0.0;
	//if (atof(c_tisshalic) <= 0.0) return 0.0;

	//double lpercep = atof(c_alicuota) / 100;
	static double montosinpercep;
	if (!bApliPerIva)
	{
		montosinpercep = 0;
		
		double lpnetos = 0;
		
		lpnetos += tNetosIva[1] * 0.03; //IVA 21%
		lpnetos += tNetosIva[2] * 0.015; //IVA 10.5%
		//lpnetos += ValueIvaPromo();
		lpnetos += tNpromo[1] * 0.03; //IVA 21%
		lpnetos += tNpromo[2] * 0.015; //IVA 10.5%
		montosinpercep = tNetosIva[1] + tNetosIva[2] + tNpromo[1] + tNpromo[2];
		vl = lpnetos;		
		if (lpnetos > 3000.01){
			vl = vl.Round(2);		
			xTotal.AddRecargo(10004, vl);
			bApliPerIva = true;
		}
	}
	if(impri && bApliPerIva)
	{
		sprintf(fac5.montoneto, "%.3lf", montosinpercep);
		vl = xTotal.VerRecargo(10004);		
		vl = vl.Round(2);
		bApliPerIva = false;		

		fac5.func = DFactu5;
		store(vl.ToDouble(), fac5.percep);
		*fac5.alicuota = 0;
		fac5.cajero = ckaNro;		
		sprintf(fac5.alicuota, "%s", "0.00");
		ProcFactu5(&fac5, 1);
		if (enpantalla)
		{
			//sprintf(c_leyenpercep, "Percepcion IVA", c_alicuota);		
			WriteTickTmp("", "Percepcion IVA RG 5329", 0,	0, 0, vl.ToDouble(), 0);		
			if(!globalDelayPrint)		
				PrintFactu5(&fac5);		
		}
	}
	return vl.ToDouble();
}

void PrintFactu5(void *fac5_)
{
	struct DDFactu5_ *fac5;

	fac5 = (struct DDFactu5_ *) fac5_;	 
	*c_leyenpercep = 0;
	strcat(c_leyenpercep, "Percepcion IVA RG 5329");	
	double imppercep = atof(fac5->percep);
	if (!imprimioPerIva)
		imprimioPerIva = pf->PrintPercep(c_leyenpercep, imppercep, 0, atof(fac5->montoneto));	 
}


void PrintNcPer(void *fac4_)
{
	struct DDNcPercep_ *facnc;

	facnc = (struct DDNcPercep_ *) fac4_;	 
	*c_leyenpercep = 0;
	if (facnc->tipo == 1)
	{
		strcat(c_leyenpercep, "IIBB RG 974 Jujuy");
		AplicarPercep = 1;
	}
	else if (facnc->tipo == 2)
	{
		strcat(c_leyenpercep, "TISSH Salta   ");
		AplicarTissh = 1;
	}
	else if (facnc->tipo == 3)
	{
		strcat(c_leyenpercep, "Percepcion IVA RG 5329");
		AplicarTissh = 1;
	}
	//strcat(c_leyenpercep, facnc->alicuota);
	double imppercep = atof(facnc->percep);
	pf->PrintPercep(c_leyenpercep, imppercep, 0, 0);	 
}


void ProcFactu2(void *fac2_, int fwrite)
{
    struct DDFactu2_ *fac2;

    fac2 = (struct DDFactu2_ *) fac2_;
	if(fwrite) WriteDump(fac2);
	xTotal.AddRecargo(10001, *BDecimal::Parse(fac2->ni));
}

void ProcFactu3(void *fac3_, int fwrite)
{
    struct DDFactu3_ *fac3;

    fac3 = (struct DDFactu3_ *) fac3_;
	 if (fwrite)	 
		 WriteDump(fac3);

	 LibEntidades::Alberdi::ImpuestoAplicado ^impAplicado = gcnew LibEntidades::Alberdi::ImpuestoAplicado();	
	 impAplicado->Alicuota = System::Convert::ToDecimal(gcnew String(fac3->alicuota));
	 impAplicado->Hora = System::DateTime::Now;
	 impAplicado->Monto = System::Convert::ToDecimal(gcnew System::String(fac3->percep));
	 impAplicado->TipoImpuesto = 1;
	 Dump::docActual->Impuestos->Add(impAplicado);

#ifdef FACTSQL
	 if (numsecuencia > 0 && fwrite)
	 {	 
		TransSql::MDPercep^ percep = gcnew TransSql::MDPercep();
		percep->Func = fac3->func;
		percep->Percep = gcnew System::String(fac3->percep);
		percep->Montoneto = gcnew System::String(fac3->montoneto);
		percep->Alicuota = gcnew System::String(fac3->alicuota);
		percep->Cajero = fac3->cajero;
		percep->Tienda = gcnew System::String(saptienda);
		percep->CnlVta = gcnew System::String(sapcnlvta);
		percep->Caja = caja;
		percep->Fecha = date();
		percep->Hora = time__();
		percep->Towrite = true;
		Dump::sqltrans->Add(percep);
	 }
#endif	
}

void ProcFactu4(void *fac4_, int fwrite)
{
	struct DDFactu4_ *fac4;

	fac4 = (struct DDFactu4_ *) fac4_;
	if (fwrite)	 
		WriteDump(fac4);
	
	LibEntidades::Alberdi::ImpuestoAplicado ^impAplicado = gcnew LibEntidades::Alberdi::ImpuestoAplicado();	
	impAplicado->Alicuota = System::Convert::ToDecimal(gcnew String(fac4->alicuota));
	impAplicado->Hora = System::DateTime::Now;
	impAplicado->Monto = System::Convert::ToDecimal(gcnew System::String(fac4->percep));
	impAplicado->TipoImpuesto = 2;
	Dump::docActual->Impuestos->Add(impAplicado);

#ifdef FACTSQL
	if (numsecuencia > 0 && fwrite)
	{	 
		TransSql::MDPerTissh^ percep = gcnew TransSql::MDPerTissh();
		percep->Func = fac4->func;
		percep->Percep = gcnew System::String(fac4->percep);
		percep->Montoneto = gcnew System::String(fac4->montoneto);
		percep->Alicuota = gcnew System::String(fac4->alicuota);
		percep->Cajero = fac4->cajero;
		percep->Tienda = gcnew System::String(saptienda);
		percep->CnlVta = gcnew System::String(sapcnlvta);
		percep->Caja = caja;
		percep->Fecha = date();
		percep->Hora = time__();
		percep->Towrite = true;
		Dump::sqltrans->Add(percep);
	}
#endif	
}

void ProcFactu5(void *fac5_, int fwrite)
{
	struct DDFactu5_ *fac5;

	fac5 = (struct DDFactu5_ *) fac5_;
	if (fwrite)	 
		WriteDump(fac5);

	LibEntidades::Alberdi::ImpuestoAplicado ^impAplicado = gcnew LibEntidades::Alberdi::ImpuestoAplicado();	
	impAplicado->Alicuota = System::Convert::ToDecimal(gcnew String(fac5->alicuota));
	impAplicado->Hora = System::DateTime::Now;
	impAplicado->Monto = System::Convert::ToDecimal(gcnew System::String(fac5->percep));
	impAplicado->TipoImpuesto = 3;
	Dump::docActual->Impuestos->Add(impAplicado);

#ifdef FACTSQL
	if (numsecuencia > 0 && fwrite)
	{	 
		TransSql::MDPerIva^ percep = gcnew TransSql::MDPerIva();
		percep->Func = fac5->func;
		percep->Percep = gcnew System::String(fac5->percep);
		percep->Montoneto = gcnew System::String(fac5->montoneto);
		percep->Alicuota = gcnew System::String(fac5->alicuota);
		percep->Cajero = fac5->cajero;
		percep->Tienda = gcnew System::String(saptienda);
		percep->CnlVta = gcnew System::String(sapcnlvta);
		percep->Caja = caja;
		percep->Fecha = date();
		percep->Hora = time__();
		percep->Towrite = true;
		Dump::sqltrans->Add(percep);
	}
#endif	
}

void ProcNcPercep(void *fac4Nc_, int fwrite)
{
	BDecimal vl;
	struct DDNcPercep_ *facNc;

	facNc = (struct DDNcPercep_ *) fac4Nc_;
	if (fwrite)	 
		WriteDump(facNc);	
	
	vl = atof(facNc->percep);
	vl = vl.Round(2);
	xTotal.AddRecargo(10003, vl);	

	LibEntidades::Alberdi::ImpuestoAplicado ^impAplicado = gcnew LibEntidades::Alberdi::ImpuestoAplicado();	
	impAplicado->Alicuota = System::Convert::ToDecimal(gcnew String(facNc->alicuota));
	impAplicado->Hora = System::DateTime::Now;
	impAplicado->Monto = System::Convert::ToDecimal(vl.ToDouble());

	if (facNc->tipo == 1){
		sprintf(c_leyenpercep, "IIBB RG 974 Jujuy   %s", facNc->alicuota);		
		impAplicado->TipoImpuesto = 1;
	}
	else if (facNc->tipo == 2){
		sprintf(c_leyenpercep, "TISSH Salta  %s", facNc->alicuota);		
		impAplicado->TipoImpuesto = 2;
	}
	else if (facNc->tipo == 3){
		sprintf(c_leyenpercep, "Percepcion IVA RG 5329");
		impAplicado->TipoImpuesto = 3;
	}

	Dump::docActual->Impuestos->Add(impAplicado);
	

	WriteTickTmp("", c_leyenpercep, 0,	0, 0, vl.ToDouble(), 0);		
	
	
#ifdef FACTSQL
	if (numsecuencia > 0 && fwrite)
	{	 
		TransSql::MDPercep^ percep = gcnew TransSql::MDPercep();
		percep->Func = facNc->func;
		percep->Percep = gcnew System::String(facNc->percep);
		percep->Montoneto = gcnew System::String(facNc->montoneto);
		percep->Alicuota = gcnew System::String(facNc->alicuota);
		percep->Cajero = facNc->cajero;
		percep->Tienda = gcnew System::String(saptienda);
		percep->CnlVta = gcnew System::String(sapcnlvta);
		percep->Caja = caja;
		percep->Fecha = date();
		percep->Hora = time__();
		percep->Towrite = true;
		Dump::sqltrans->Add(percep);
	}
#endif	
}


void NotaCredito(int cual)
{
	bool esticket = false;
	int afecha = 0;
	int ptoVenta = 0;
	char pcodMotivoNc[10], imptTick[14];

	Inicializando = 0;
	
	if (!selfCheckout && ckaNro <= 0){
		Alert("ATENCION. Necesita habilitar un cajero.", "para realizar la Nota de Crédito.");
		return;
	}

	/*MpagCliente ^cltForm = gcnew MpagCliente();
	cltForm->ValidateCod = gcnew MpagCliente::ValidateCodDelegate(&MpagoStatic::ValidaCodCuenta);
	cltForm->ShowDialog();
	System::Windows::Forms::DialogResult resrta = (cltForm->DialogResult == System::Windows::Forms::DialogResult::OK);*/

	frmNroNotaCredito ^fnc = gcnew frmNroNotaCredito();
	if (fnc->ShowDialog() == DialogResult::OK)
	{
		switch (fnc->pTipoComp)
		{
			case 0:
				TipoCompNc = 83;
				esticket = true;
				break;
			case 1:
				TipoCompNc = 81;
				break;
			case 2:
				TipoCompNc = 82;
				break;
			default:
				TipoCompNc = 83;
				break;
		}

		NumCompNc = fnc->pNroComp;
		ptoVenta = fnc->pPtoVta;
		Strings::StringToChar(fnc->motivoActual->CodigoMotivo, codMotivoNc, 10);

		char unaLetra = '-';
		if (TipoCompNc == 82)
			unaLetra = 'B';
		else if (TipoCompNc == 81)
			unaLetra = 'A';
		sprintf(nroTicketOriginal, "%04d%c%08d", atoi(pf->ptovta), unaLetra, NumCompNc);

		if (BuscaTicketNcHecho(nroTicketOriginal, false)){
			Alert("Ya se realizo Nota de Credito del Ticket/Factura", "");
			return;
		}
		
		afecha = System::Convert::ToInt32(fnc->pFechaComp.ToString("yyyyMMdd"));
		remove("tknc.dbf");
		if (MoveTicketNC("tknc.dbf", TipoCompNc, ptoVenta, NumCompNc, afecha, imptTick))
		{
			char msgConf[50];
			sprintf(msgConf, "El ticket/factura tiene importe de $ %.2lf", atof(imptTick));
			int respuesta = GetSiNo(msgConf, "¿Desea continuar con la Nota de Credito?");
			if (respuesta)
				IncorporeTicketNC("tknc.dbf", TipoCompNc, NumCompNc, 000, codMotivoNc);
			else
				return;
			//}
			//else
			//{
			//	EsNotaCredito = true;
			//	Dump::actNotaCredito = gcnew Controles::NCredito();
			//	LeeTicketParaNC("tknc.dbf");
			//	Dump::actNotaCredito->FechaOr = fnc->pFechaComp;
			//	Dump::actNotaCredito->NroDocOr = fnc->pNroComp;
			//	Dump::actNotaCredito->TipoDocOr = TipoCompNc;
			//	Dump::actNotaCredito->PtoVta = System::Convert::ToInt32(pf->ptovta);
			//	frmCargaNotaCredito ^fncarga = gcnew frmCargaNotaCredito();
			//	if (fncarga->ShowDialog() == DialogResult::OK)
			//	{
			//		for each(Controles::NCDeta ^detanc in Dump::actNotaCredito->Detalle)
			//		{
			//			struct DDNcItem_ ncplu;
			//			ncplu.func = DNcItem;
			//			double auxmonto = System::Convert::ToDouble(detanc->Precio);
			//			ncplu.anulado = 0;
			//			ncplu.cod = System::Convert::ToInt64(detanc->Codigo);
			//			ncplu.envas = 0;
			//			ncplu.fecha = date();
			//			ncplu.hora = time_();
			//			ncplu.cajero = ckaNro;					
			//			ncplu.miles[0] = 0;
			//			ncplu.otroP = listap;
			//			ncplu.pesado = 0;
			//			ncplu.precuni = (unsigned long)((auxmonto + 1e-10) * 100);
			//			ncplu.s_c_flag = 0;
			//			ncplu.uCant = 0;
			//			store(auxmonto, ncplu.xReg);
			//			store(System::Convert::ToDouble(detanc->Cantidad), ncplu.yReg);
			//			//STRCPY(ncplu.origCode, ucod);
			//			Strings::StringToChar(detanc->Codigo, ncplu.origCode, 6);
			//			
			//			ncplu.iva = detanc->Iva;					
			//			ncplu.dnro = detanc->Dep;					
			//			ncplu.sdnro = detanc->SubDep;
			//			ncplu.esBulto = 0;
			//			ncplu.uni = 1;
			//			*ncplu.precmay = 0;
			//			ncplu.ppuntual = 0;
			//			ncplu.umillas = 0;
			//			sprintf(ncplu.pesoenbalanza, "%.3lf", 0.0);
			//			ProcNcPlu(&ncplu, 1);
			//		}				
			//		
			//		struct DDNcPago_ ncpago;
			//		ncpago.func = DNcPago;
			//		ncpago.cajero = ckaNro;
			//		ncpago._fecha = date();
			//		ncpago._hora = time_();

			//		ncpago.nro = 1;//Efectivo
			//		STRCPY(ncpago.dtarjeta, "");
			//		STRCPY(ncpago.fecha, "");
			//		STRCPY(ncpago.numero, "");
			//		STRCPY(ncpago.telef, "");
			//		STRCPY(ncpago.nombre, "");
			//		STRCPY(ncpago.direc, "");
			//		ncpago.uVuelto = 0;
			//		ncpago.uOnline = 0;
			//		ncpago.escredito = 0;
			//		ncpago.esextracash = 0;
			//		ncpago.esposnet = 0;
			//		Strings::StringToChar(Dump::actNotaCredito->CalculaTotal(), ncpago.importe, 14);
			//		sprintf(ncpago.vuelto, "%.3lf", 0.0);
			//		ncpago.uPend = 0;
			//		tVtaReal = System::Convert::ToDouble(Dump::actNotaCredito->CalculaTotal());
			//		Strings::StringToChar(Dump::actNotaCredito->GetFormatCompOriginal(), nroTicketOriginal, 15);
			//		ProcNcMpag(&ncpago, 1);
			//		EscribeRelTicketNc();
			//	}
			//}
		}
		else
			Alert("No se encontro Ticket/Factura", "Verificar los datos ingresados");
	}
	
}

void CargaSeniaPago(int cual)
{
	char seniamonto[20];
	if (ckaNro <= 0){
		Alert("ATENCION. Necesita habilitar un cajero.", "para Cargar la Seña de Pago.");
		return;
	}
	frmSeniaMP ^fseniapago = gcnew frmSeniaMP();
	fseniapago->ShowDialog();
	if (fseniapago->DialogResult == System::Windows::Forms::DialogResult::OK)
	{
		struct DSenia_ psenia;
		psenia.func = DSeniaMP;
		psenia.cajero = ckaNro;
		psenia.supervisor = supervNro;
		psenia.mediopago = fseniapago->NroMedioPago;
		Strings::StringToChar(fseniapago->SMontoIngresado, psenia.monto, 14);
		psenia.hora = time_();
		psenia.fecha = date();

		//psenia.monto
		ProcSenia(&psenia, 1);
	}
}

void ProcSenia(void *fac5_, int fwrite)
{
	struct DSenia_ *fac5;

	fac5 = (struct DSenia_ *) fac5_;
	if (fwrite){ 
		WriteDump(fac5);
		PrintSenia(fac5);
	}
	
//#ifdef FACTSQL
//	if (numsecuencia > 0 && fwrite)
//	{	 
//		TransSql::MDPerIva^ percep = gcnew TransSql::MDPerIva();
//		percep->Func = fac5->func;
//		percep->Percep = gcnew System::String(fac5->percep);
//		percep->Montoneto = gcnew System::String(fac5->montoneto);
//		percep->Alicuota = gcnew System::String(fac5->alicuota);
//		percep->Cajero = fac5->cajero;
//		percep->Tienda = gcnew System::String(saptienda);
//		percep->CnlVta = gcnew System::String(sapcnlvta);
//		percep->Caja = caja;
//		percep->Fecha = date();
//		percep->Hora = time__();
//		percep->Towrite = true;
//		Dump::sqltrans->Add(percep);
//	}
//#endif	
}


void PrintSenia(void *fac5_)
{
	struct DSenia_ *fac5;

	fac5 = (struct DSenia_ *) fac5_;	
	VoucherVar("@CAJA@", caja);				// Variables de voucher...
	VoucherVar("@CAJERO@", clkName());
	VoucherVar("@SUPERV@", supervName());
	VoucherVar("@IMPORTE@", fac5->monto);
	if (fac5->mediopago == 1)
		VoucherVar("@MEDIO_PAGO@", "EFECTIVO");	
	else
		VoucherVar("@MEDIO_PAGO@", "TRANSFERENCIA");	
	StackVoucher(16);
	StackVoucher(17);
	// Cierra la operacion en el trans.
	WriteEOPNoFiscal();
}



bool PidePrecio(char *codNom, BDecimal *prec)
{
#ifdef POSXP
	PrecManual ^form = gcnew PrecManual(Strings::CharToString(codNom), prec->ToDouble());
	form->ShowDialog();
	if(form->DialogResult != System::Windows::Forms::DialogResult::OK)
		return false;
	if(form->Precio <= 0)
	{
		Alert("El precio indicado no es valido", "");
		return false;
	}
	*prec = BDecimal(form->Precio);
	return true;
#else
//   #error Hacer Precio Manual para MSDOS - Adaptar PLU::GetManPrec()
	return false;
#endif
}


////   Despues de concretada una venta, reprocesa el trans para ver si aplican algunos de los beneficios.
//void ReprocTransMayor(struct DDplu_ *vta, bool anulFlag)
//{
//	bool redoTicket = false, redoTicket2 = false;
//	if(modoMayor == 0 || globalRedraw) return;
//	if(precioBulto <= 0)
//	{
//		if(!modoMayor || vta->dun14) return;
//	}
//	if(!anulFlag && (atof(vta->yReg) < 0)) return;
//
//	// Busca el articulo en el resumen del ticket que llevamos.
//	TktItemResu *res = (TktItemResu *)mayorTkt->Find(vta->cod);
//	if(res == NULL)
//	{
//		// Posiciona en el PLU que corresponde.
//		sprintf(msg, "%06d", vta->cod);
//		dbplu.Seek(msg);
//		dbplu.Get("cant2", msg);
//		int cant2 = atoi(msg);
//		if(cant2 <= 0) cant2 = 99999999;
//        dbplu.Get("uni", msg);
//		int uni = atoi(msg);
//		if(uni <= 1) uni = 1;
//
//		// Rellena la estructura de control
//		res = new TktItemResu();
//		res->cod = vta->cod;
//		res->cant2 = cant2;
//		res->uni = uni;
//		res->prec = defListaPrec;
//		mayorTkt->Add((int)vta->cod, res);
//	}
//
//	// Detecta si hay cambios de importancia en el ticket
//	double finalCantToDisplay = atof(vta->yReg);
//	if(anulFlag)
//	{
//		// (La cantidad en vta->yReg esta en negativo...)
//		redoTicket = (modoMayor > 0) && (res->cant + atof(vta->yReg) < res->cant2) && (res->cant >= res->cant2);
//		redoTicket2 = (precioBulto > 0) && (res->cant + atof(vta->yReg) < res->uni) && (res->cant >= res->uni);
//		res->cant += atof(vta->yReg);
//		if(redoTicket2)
//		{
//			if((res->cant + atof(vta->yReg) >= res->cant2))
//				res->prec = modoMayor;
//			else
//				res->prec = listap;
//		}
//		else if(redoTicket) 
//			res->prec = listap;
//		finalCantToDisplay *= (-1);
//	}
//	else
//	{
//		redoTicket = (modoMayor > 0) && (res->cant < res->cant2) && (res->cant + atof(vta->yReg) >= res->cant2);		
//		redoTicket2 = (precioBulto > 0) && (res->uni > 1) && (res->cant < res->uni) && (res->cant + atof(vta->yReg) >= res->uni);
//		
//		res->cant += atof(vta->yReg);
//		if(redoTicket2) 
//			res->prec = precioBulto;
//		else if(redoTicket) 
//			res->prec = modoMayor;
//	}
//	if(!redoTicket && !redoTicket2) return;
//
//	// Determina el nuevo precio del articulo, a fin de parchar las entradas que correspondan en el trans.
//	char _cod[30];
//	sprintf(_cod, "%06d", vta->cod);
//	dbplu.Seek(_cod);
//	if(res->prec == 1)
//		dbplu.Get("precio", msg);
//	else
//	{
//		sprintf(_cod, "precio%d", res->prec);
//		dbplu.Get(_cod, msg);
//	}
//	double uPrec = atof(msg);
//
//	// Reprocesa el trans para recalcular los totales y redibujar los renglones.
//	GCStatic::AddLockToTrans("PrecioMayor");
//	globalRedraw = true;
//	ResetPOSAcumInternal();
//	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
//	{
//		if(!tDbf.Deleted())
//		{
//			// Lee info dsde el trans
//			char _func[10], _cod[20], _anul[5], _cant[20], _dun14[5];
//			tDbf.Get("func", _func); tDbf.Get("cod", _cod); tDbf.Get("anulado", _anul); tDbf.Get("flag", _dun14);
//			if(atoi(_func) == 4 && atoi(_cod) == vta->cod && _anul[0] != 'T' && _dun14[0] != 'T')
//			{
//				// Trae los campos que faltan.
//				tDbf.Get("cant", _cant);
//
//				// Reemplaza entradas en el trans
//				sprintf(msg, "%d", (int)(uPrec * 100));
//				tDbf.Replace("vuelto", msg);
//				sprintf(msg, "%.3lf", uPrec * atof(_cant));
//				tDbf.Replace("imp", msg);
//
//				// Reemplaza entradas en el ticket temporario
//				/*if(tTmp.Opened())
//				{*/
//					sprintf(msg, "%d", tDbf.Recno());
//					int ndump = atoi(msg);
//					sprintf(msg, "%8.2lf", uPrec);
//					String^ nunit = gcnew String(msg);
//					sprintf(msg, "%9.2lf", uPrec * atof(_cant));
//					String^ ntot = gcnew String(msg);
//					Dump::cTicket->ReimprimeItem(ndump, nunit, ntot);
//					/*if(tTmp.Locate("dump", msg))
//					{
//						sprintf(msg, "%8.2lf", uPrec);
//						tTmp.Replace("unitario", msg);
//						sprintf(msg, "%9.2lf", uPrec * atof(_cant));
//						tTmp.Replace("total", msg);
//					}*/
//				//}
//			}
//		}
//
//		// Reprocesa el renglon.
//		ReprocActualTran();
//	}
//
//	globalRedraw = false;
//	GCStatic::ReleaseLockFromTrans("PrecioMayor");
//	PrintTktTot(true);
//	DispCash(finalCantToDisplay * uPrec, 2);
//}
