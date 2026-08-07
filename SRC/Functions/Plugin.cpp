//
//   PluginFunctions.cpp
//   Contiene las funciones que se agregan al evaluador de expresiones, para permitir 'customizar' el
//   codigo que se genere.
//

#include "stdafx.h"
#include "funcs.h"
#include "StaticDump.h"
#include "frmSelColegio.h"
#include "frmIngNroDoc.h"
//#include "frmIngresaCod.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::Data::Odbc;

// Definicion de canastas
class CanasDef
{
public:
	char cod[12];
	double cant;
	double venta;
	double ventaMonto;
	double precuni;
	

	CanasDef()
	{
		cant = 0;
		venta = 0;
		ventaMonto = 0;		
		precuni = 0;
	}
};

ref class TfSelColegio { public: static frmSelColegio ^tForm = nullptr; };

/*********************************************************************************************************************
 *   Funciones internas.
 *   RECORDAR QUE DEBEN ELIMINAR LOS ARGUMENTOS QUE NO RETORNAN
*/

// Funciones y variables externas
extern int caja, suc, defListaPrec; 
extern double pTot, xCnt;
extern dbf tDbf, dbplu;
extern unsigned long ActualTrans;
extern char *msg;
extern PromoDef *actualPromo;

void StackVoucher(int);										// Voucher.cpp
unsigned long date(void);									// HoraFec.cpp
int time_(void);											// HoraFec.cpp
int dow(void);												// HoraFec.cpp
char *padl(double cual, int cuanto);
void ProcPlu(void *p_, int fwrite);							// plu.cpp
char *store(double src, char *dst);							// Convierte DOUBLE a ASCII.
void WriteBootLog(char *msg);								// pos.cpp

/*   Prototipos  */
ValuePtr F_Str(ValuePtr args[]);
double CommonTicketExplorer(int inum, int what);


bool ExcluidosDePromo(int codplu)
{
	bool aux = false;
	if (codplu == 99997)
		aux = true;
	return aux;
}
/*********************************************************************************************************************
 *   Funciones internas.
 *   RECORDAR QUE DEBEN ELIMINAR LOS ARGUMENTOS QUE NO RETORNAN
*/

//   Funcion que espera como argumento el codigo de un articulo, y retorna el precio 1 (de venta al publico)
ValuePtr F_Precio(ValuePtr args[])
{
	double dcod = args[0]->ValueDouble();
	delete args[0];
	dbplu.Setorder("mplucod");
	double prec = 0;
	if(dbplu.Seek(padl(dcod, 6)))
	{
		char bPrec[20];
		switch (defListaPrec){
		case 1 :
				dbplu.Get("precio", bPrec);
				break;
		case 2:
				dbplu.Get("precio2", bPrec);
				break;
		case 3:
				dbplu.Get("precio3", bPrec);
				break;
		case 4:
				dbplu.Get("precio4", bPrec);
				break;
		}
		prec = atof(bPrec);
	}
	if (enApplyAction)
		puApplyPromo = prec;

	return new Value(prec);
}

//   Funcion que espera como argumento el codigo de un articulo, y retorna el precio 1 (de venta al publico)
ValuePtr F_Precio_Pvm(ValuePtr args[])
{
	double dcod = args[0]->ValueDouble(); // Codigo del articulo
	double dcant = args[1]->ValueDouble(); // cantidad final que cumple con la promocion
	delete args[0];
	delete args[1];
	dbplu.Setorder("mplucod");
	double prec = 0;
	if(dbplu.Seek(padl(dcod, 6)))
	{
		char bPrec[20];
		dbplu.Get("pvm", bPrec);
		if (bPrec[0] == '1' && modoMayor)
		{
			dbplu.Get("uni", bPrec);
			double cantuni = atof(bPrec);
			int i1 = 3;
			char campo[6];
			bool bencontrado = false;
			int cbultos = (int) dcant/cantuni;
			if(cbultos > 0)
			{			
				while (i1 >= 1)
				{
					sprintf(campo, "cb%d", i1);
					dbplu.Get(campo, msg);
					int condbultos = atoi(msg);
					if (condbultos > 0 && cbultos >= condbultos)
					{
						sprintf(campo, "B%d", i1);
						dbplu.Get(campo, msg);
						prec = atof(msg)/cantuni;												
						bencontrado = true;
						break;
					}				
					i1--;
				}
			}
			if (!bencontrado) //sigue buscando
			{
				i1 = 2;
				while (i1 >= 1)
				{
					sprintf(campo, "cf%d", i1);
					dbplu.Get(campo, msg);
					int conduni = atoi(msg);
					if (conduni > 0 && dcant >= conduni)
					{
						sprintf(campo, "F%d", i1);
						dbplu.Get(campo, msg);
						prec = atof(msg);												
						bencontrado = true;
						break;
					}				
					i1--;
				}
			}
		}
		else
		{		
			switch (defListaPrec)
			{
				case 1 :
					dbplu.Get("precio", bPrec);
					break;
				case 2:
					dbplu.Get("precio2", bPrec);
					break;
				case 3:
					dbplu.Get("precio3", bPrec);
					break;
				case 4:
					dbplu.Get("precio4", bPrec);
					break;
			}
			prec = atof(bPrec);
		}
	}
	if (enApplyAction)
		puApplyPromo = prec;

	return new Value(prec);
}


//   Funcion que espera como argumento el codigo de un articulo, y retorna el precio 1 (de venta al publico)
ValuePtr F_CantUnidades(ValuePtr args[])
{
	double dcod = args[0]->ValueDouble();
	delete args[0];
	dbplu.Setorder("mplucod");
	double prec = 0;
	if(dbplu.Seek(padl(dcod, 6)))
	{
		char bPrec[20];
		dbplu.Get("uni", bPrec);		
		prec = atof(bPrec);
	}

	return new Value(prec);
}


// **********  FUNCIONES DE PROMO   ************************************************
// Accion de PROMO que permite stackear un voucher.
// ATENTI - Todas las acciones devuelven 'T', para que se puedan combinar en la definicion
//          tanto con 'AND' como con 'OR'.
ValuePtr F_VoucherC(ValuePtr args[])
{
	char tmp[20];

	int voucherNum = args[0]->ValueInteger();
	int voucherCant = args[1]->ValueInteger();
	delete args[1];

	if(voucherCant > 0)
	{
		for(int idup = 1; idup <= voucherCant; idup++)
		{
			System::DateTime ahora = System::DateTime::Now;
			System::String^ diahoy = ahora.ToString("dd/MM/yyyy");
			Strings::StringToChar(diahoy, tmp, 20);
			VoucherVar("@FECHAHOY@", tmp);

			VoucherVar("@COPIA@", idup);
			switch(idup)
			{
				case 1: VoucherVar("@COPIA_NOM@", "Original"); break;
				case 2: VoucherVar("@COPIA_NOM@", "Duplicado"); break;
				case 3: VoucherVar("@COPIA_NOM@", "Triplicado"); break;
				case 4: VoucherVar("@COPIA_NOM@", "Cuadruplicado"); break;
				default:
					sprintf(tmp, "Copia %d", idup);
					break;
			}

			if(!soloSimular)
				StackVoucher(voucherNum);
		}

		sprintf(tmp, "%d", voucherNum);
		RegisterPrize(1, tmp, "", "", false, 0);
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

//  Funcion simplificada de emision de vouchers, que emite UN SOLO voucher.
ValuePtr F_Voucher(ValuePtr args[])
{
	ValuePtr nargs[2];
	nargs[0] = args[0];
	nargs[1] = new Value(1);
	return F_VoucherC(nargs);
}

// Muestra (y/o imprime) un mensaje.
ValuePtr F_Mensaje(ValuePtr args[])
{
	char tMsg[200], *pSrc, *pDst, varName[200], *pVar = NULL;
	char *msg = args[0]->ToString();
	int flags = args[1]->ValueInteger();

	// Reemplaza en el mensaje aquellas variables que esten embebidas.
	bool inVarName = false;
	bool hayqueaplicar = true;
	if (gNumMedioPago > 0)
	{
		hayqueaplicar = false;
		/*if (Dump::promosmp->GetMontoDescConfirmadoDouble(gNumMedioPago) > 0)
			hayqueaplicar = true;*/
	}
	if (!hayqueaplicar)
	{
		// Retorna un valor logico de TRUE
		args[1]->SetType('B');
		args[1]->_intValue = 1;
		delete args[0];
		return args[1];
	}
		
	for(pSrc = msg, pDst = tMsg; *pSrc; pSrc++)
	{
		if(*pSrc == '@')
		{
			if(inVarName)
			{
				*pVar = 0;
				ValuePtr vp = (*FunctionsNameResolver)(varName);
				if(vp != NULL)
				{
					char *resVal = vp->ToString();
					STRCPY(pDst, resVal);
					pDst += strlen(resVal);
				}
				inVarName = false;

			}
			else
			{
				inVarName = true;
				pVar = varName;
			}
		}
		else
		{
			if(inVarName)
				*pVar++ = *pSrc;
			else
				*pDst++ = *pSrc;
		}
	}
	*pDst = 0;

	// Manda el mensaje a la pantalla, si corresponde.
	if(!soloSimular && (flags == 0 || flags == 1 || flags == 3))
	{
		char *t = &tMsg[(tMsg[0] == '&')? 2: 0];
		showMessage = STRDUP(t, "F_Mensaje");
	}

	// Manda el mensaje al pie del impresor.
	if(!soloSimular && (flags == 2 || flags == 3))
		pf->SetPie(tMsg);

	// Registra en el TRANS la operacion.
	RegisterPrize(2, tMsg, "", "", (flags == 2 || flags == 3), 1);

	// Retorna un valor logico de TRUE
	args[1]->SetType('B');
	args[1]->_intValue = 1;
	delete args[0];
	return args[1];
}

// Muestra (y/o imprime) un mensaje.
ValuePtr F_Mensaje2(ValuePtr args[])
{
	char tMsg[200], *pSrc, *pDst, varName[200], *pVar = NULL;
	char *msg = args[0]->ToString();
	int flags = args[1]->ValueInteger();
	char valcount[30];

	bool inVarName = false;
	bool hayqueaplicar = true;
	if (gNumMedioPago > 0)
	{
		hayqueaplicar = false;
		/*if (Dump::promosmp->GetMontoDescConfirmadoDouble(gNumMedioPago) > 0)
			hayqueaplicar = true;*/
	}
	if (!hayqueaplicar)
	{
		// Retorna un valor logico de TRUE
		args[1]->SetType('B');
		args[1]->_intValue = 1;
		delete args[0];
		return args[1];
	}

	// Reemplaza en el mensaje aquellas variables que esten embebidas.
	STRCPY(valcount, "");
	for(pSrc = msg, pDst = tMsg; *pSrc; pSrc++)
	{
		if(*pSrc == '@')
		{
			if(inVarName)
			{
				*pVar = 0;
				ValuePtr vp = (*FunctionsNameResolver)(varName);
				if(vp != NULL)
				{
					char *resVal = vp->ToString();					
					STRCPY(pDst, resVal);
					pDst += strlen(resVal);
					STRCPY(valcount, resVal);
				}
				inVarName = false;

			}
			else
			{
				inVarName = true;
				pVar = varName;
			}
		}
		else
		{
			if(inVarName)
				*pVar++ = *pSrc;
			else
				*pDst++ = *pSrc;
		}
	}
	*pDst = 0;

	// Manda el mensaje a la pantalla, si corresponde.
	if(!soloSimular && (flags == 0 || flags == 1 || flags == 3))
	{
		char *t = &tMsg[(tMsg[0] == '&')? 2: 0];
		showMessage = STRDUP(t, "F_Mensaje2");
	}

	// Manda el mensaje al pie del impresor.
	if(!soloSimular && (flags == 1 || flags == 2 || flags == 3))
		pf->SetPie(tMsg, flags);

	// Registra en el TRANS la operacion.
	//RegisterPrize(2, tMsg, "", "");
	RegisterPrize(2, tMsg, "", valcount, (flags == 1 || flags == 2 || flags == 3), flags);

	// Retorna un valor logico de TRUE
	args[1]->SetType('B');
	args[1]->_intValue = 1;
	delete args[0];
	return args[1];
}


//  Funcion comun para todas aquellas promociones que deben hacer un descuento al final del ticket.
//     codigo: Codigo de PLU donde se registrara el descueto.
//      monto: El monto total (en pesos) a descontar.
//       cant: Cantidad de articulos que le imputaran al descuento.
//    simular: Si esta en TRUE, entonces solo simula la ocurrencia del descuento y no imprime nada.
void CommonDescto(int codigo, double monto, double cant, bool simular)
{
	// Busca el articulo a promocionar.
	char *ucod = padl((double)codigo, 6);
	dbplu.Setorder("mplucod");
	System::Decimal valor = System::Decimal(monto);
	valor = Decimal::Round(valor, 2);	
	double auxmonto = Decimal::ToDouble(valor);
	if(dbplu.Seek(ucod))
	{
		if(simular)
		{
			dbplu.Get("iva", msg); //dbplu.Get(EsRI ? "ivari" : "iva", msg);
			CalcIvaPromo(auxmonto, atoi(msg), -1);
			if (gNumMedioPago == 0) //es una promocion normal
				xTotal.AddDescuento(BDecimal(auxmonto));		
			else if (gNumMedioPago > 0)
			{
				Controles::PromoMedioPago ^itemMp = gcnew Controles::PromoMedioPago();
				itemMp->NumMpago = gNumMedioPago;
				itemMp->Monto = System::Decimal(auxmonto);
				itemMp->ImporteSinDescto = System::Decimal(gImporteSinDescto);
				itemMp->Descarga = codigo;
				itemMp->NumTarjeta = gNumTarjeta;
				//itemMp->AplicaInic = itemMp->NumTarjeta != 0;
				itemMp->PorcDescto = gPorcDescto/100;
				Dump::promosmp->Agregar(itemMp);
				xTotal.AddDescMedioPago(gNumMedioPago, BDecimal(auxmonto));
				
			}
		}
		else
		{
			bool hayqueaplicar = true;
			if (gNumMedioPago > 0)
			{
				hayqueaplicar = false;
				auxmonto = Dump::promosmp->GetMontoDescConfirmadoDoublePorDesc(gNumMedioPago, codigo);
				if ( auxmonto>= 0.01)
				{
					hayqueaplicar = true;
					xTotal.AddDescMedioPago(gNumMedioPago, BDecimal(auxmonto));
				}
			}
			if (hayqueaplicar)
			{			
				char *mpmsg = new char[100];
				sprintf(mpmsg, "Para aplicar importe descto $%.2lf", monto);
				WriteBootLog(mpmsg);
				delete mpmsg;

				struct DDplu_ plu;
				plu.anulado = 0;
				plu.cod = codigo;
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
				store(cant, plu.yReg);
				STRCPY(plu.origCode, ucod);

				dbplu.Get("iva", msg); //dbplu.Get(EsRI ? "ivari" : "iva", msg);
				plu.iva = atoi(msg);
				dbplu.Get("dep", msg);
				plu.dnro = atoi(msg);			
				dbplu.Get("subdep", msg);
				plu.sdnro = atoi(msg);
				STRCPY(plu.montoImpInterno, "0.00");
				plu.esBulto = 0;
				plu.uni = 1;
				*plu.precmay = 0;
				plu.ppuntual = 0;
				plu.umillas = 0;
				sprintf(plu.pesoenbalanza, "%.3lf", 0.0);

				AcumDesctoPromo += (auxmonto + 1e-10);

				pluAdicText = NULL;
				ProcPlu(&plu, 1);
				char tmp[20];
				sprintf(tmp, "%d", codigo);
				RegisterPrize(3, tmp, plu.yReg, plu.xReg, false, 0);			
			}
		}
	}
	else
	{
		sprintf(msg, "Error en PROMO (RESTAR_TOTAL) - El plu '%d' no existe", codigo);
		WriteBootLog(msg);
	}
	cntApplyPromo = puApplyPromo = 0;
}
// Accion que permite restar un monto fijo
ValuePtr F_RestaTotal(ValuePtr args[])
{
	// Obtiene los parametro de la funcion.
	double monto = args[0]->ValueDouble();
	int codigo = args[1]->ValueInteger();
	delete args[1];

	CommonDescto(codigo, monto / 100, 1, soloSimular);

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Accion que permite restar un monto fijo
ValuePtr F_SeleccionarColegio(ValuePtr args[])
{
	int cantptoscol = args[0]->ValueInteger();
	if (soloSimular && codcolsel == 0)
	{	
		TfSelColegio::tForm = gcnew frmSelColegio();
		TfSelColegio::tForm->ShowDialog();
		codcolsel = System::Convert::ToInt32(TfSelColegio::tForm->codigosel);
		TfSelColegio::tForm = nullptr;
	}
	else if (!soloSimular && codcolsel != 0)
	{
		// Busca el articulo a promocionar.
		char *ucod = padl((double)codcolsel, 6);
		dbplu.Setorder("mplucod");			
		double auxmonto = 0;//Precio $ 0.00
		if(dbplu.Seek(ucod))
		{						
			struct DDplu_ plu;
			plu.anulado = 0;
			plu.cod = codcolsel;
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
			store(auxmonto, plu.xReg);
			store(cantptoscol, plu.yReg);
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

			pluAdicText = NULL;
			ProcPlu(&plu, 1);
			char tmp[20];
			sprintf(tmp, "%d", codcolsel);
			RegisterPrize(3, tmp, plu.yReg, plu.xReg, false, 0);	
			codcolsel = 0;
		}		
	}
	ValuePtr result = new Value(true);	
	return result;
}

// Accion que permite restar un monto fijo
ValuePtr F_IngresarSerie(ValuePtr args[])
{
	int cantaing = args[0]->ValueInteger();	
	if (soloSimular)
	{	
		cantcuping = cantaing;
		/*frmIngresaCod ^tForm = gcnew frmIngresaCod(cantaing);
		if (tForm->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{		
			cantcuping = System::Convert::ToInt32(tForm->cantcuping);
			montocuping = System::Convert::ToDouble(tForm->montocuping);
		}
		tForm = nullptr;
		delete tForm;*/			 
	}
	else if (!soloSimular && cantcuping > 0)
	{
		cantcuping = 0;
		montocuping = 0;
		// Busca el articulo a promocionar.
		//char *ucod = padl((double)lcdescarga, 6);
		//dbplu.Setorder("mplucod");			
		//double auxmonto = montocuping;//Precio $ 0.00
		//if(dbplu.Seek(ucod))
		//{						
		//	struct DDplu_ plu;
		//	plu.anulado = 0;
		//	plu.cod = lcdescarga;
		//	plu.envas = 0;
		//	plu.fecha = date();
		//	plu.hora = time_();
		//	plu.cajero = ckaNro;
		//	plu.func = 4;
		//	plu.miles[0] = 0;
		//	plu.otroP = listap;
		//	plu.pesado = 0;
		//	plu.precuni = (unsigned long)((auxmonto + 1e-10) * 100);
		//	plu.s_c_flag = 0;
		//	plu.uCant = 0;
		//	store(auxmonto, plu.xReg);
		//	store(cantcuping, plu.yReg);
		//	STRCPY(plu.origCode, ucod);

		//	dbplu.Get("iva", msg);
		//	plu.iva = atoi(msg);
		//	dbplu.Get("dep", msg);
		//	plu.dnro = atoi(msg);			
		//	dbplu.Get("subdep", msg);
		//	plu.sdnro = atoi(msg);
		//	plu.esBulto = 0;
		//	plu.uni = 1;
		//	*plu.precmay = 0;
		//	plu.ppuntual = 0;
		//	plu.umillas = 0;
		//	sprintf(plu.pesoenbalanza, "%.3lf", 0.0);

		//	pluAdicText = NULL;
		//	ProcPlu(&plu, 1);
		//	char tmp[20];
		//	sprintf(tmp, "%d", codcolsel);
		//	RegisterPrize(3, tmp, plu.yReg, plu.xReg, false, 0);	
		//	codcolsel = 0;
		//}		
	}
	ValuePtr result = new Value(true);	
	return result;
}


// Accion que permite restar un monto fijo
ValuePtr F_RestaTotal2(ValuePtr args[])
{
	// Obtiene los parametro de la funcion.
	double cant = args[0]->ValueDouble();
	double monto = args[1]->ValueDouble();
	int codigo = args[2]->ValueInteger();
	delete args[1];
	delete args[2];

	CommonDescto(codigo, monto / 100, cant, soloSimular);

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Accion que permite aplicarle un porciento a un monto y restarlo
ValuePtr F_DescTotal(ValuePtr *args)
{
	// Obtiene los parametro de la funcion.
	double tasa = args[0]->ValueDouble() / 100;
	double monto = args[1]->ValueDouble();
	int codigo = args[2]->ValueInteger();
	delete args[1];
	delete args[2];

	gImporteSinDescto = monto;
	gPorcDescto = tasa;
	CommonDescto(codigo, monto * tasa / 100, 1, soloSimular);

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}


// Accion que permite aplicarle un porciento a un monto y restarlo
ValuePtr F_DescTotalIva(ValuePtr *args)
{
	// Obtiene los parametro de la funcion.
	double tasa = args[0]->ValueDouble() / 100;
	double vta10 = args[1]->ValueDouble();	
	int codigo10 = args[2]->ValueInteger();
	double vta21 = args[3]->ValueDouble();	
	int codigo21 = args[4]->ValueInteger();
	double vtatope = args[5]->ValueDouble();
	double vtasuma = vta10 + vta21;
	delete args[1];
	delete args[2];
	delete args[3];
	delete args[4];
	delete args[5];

	
	double mayor = vta10 > vta21 ? vta10 : vta21;
	int codigosel = vta10 > vta21 ? codigo10 : codigo21;
	double tope = 0;
	if (vtatope > 0)
		tope = vtasuma > vtatope ? vtatope : vtasuma;
	else
		tope = vtasuma;	

	gPorcDescto = tasa;
	gImporteSinDescto = vtasuma;

	double valdescto = tope * tasa;
	
	CommonDescto(codigosel, valdescto / 100, 1, soloSimular);	

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Accion que permite aplicarle un porciento a un monto y restarlo, considera ademas un tope por cliente
ValuePtr F_DescTotalIva2(ValuePtr *args)
{
	// Obtiene los parametro de la funcion.
	double tasa = args[0]->ValueDouble() / 100;
	double vta10 = args[1]->ValueDouble();	
	int codigo10 = args[2]->ValueInteger();
	double vta21 = args[3]->ValueDouble();	
	int codigo21 = args[4]->ValueInteger();
	//double vtatope = args[5]->ValueDouble();
	double vtasuma = vta10 + vta21;
	delete args[1];
	delete args[2];
	delete args[3];
	delete args[4];
	delete args[5];


	double mayor = vta10 > vta21 ? vta10 : vta21;
	int codigosel = vta10 > vta21 ? codigo10 : codigo21;
	double tope = 0;
	/*if (vtatope > 0)
		tope = vtasuma > vtatope ? vtatope : vtasuma;
	else*/
		tope = vtasuma;	

	gPorcDescto = tasa;
	gImporteSinDescto = vtasuma;

	double valdescto = (tope * tasa) / 100;
	//Promocion para clientes de Fan Club Comodin
	bool paraApliCli = false;
	//Promocion actual es para clientes fan club
	if (actMarcaCli == 10)
		paraApliCli = (ClienteBenef >= 2 && ClienteBenef <= 9) || ClienteBenef == 11;

	if (ClienteBenef > 0 && (actMarcaCli == ClienteBenef || paraApliCli))
	{
		acumSaldoPromo = 0;
		LeeSaldoConsumo();
		if (atof(c_saldopromo) > 0)
		{		
			char *ucod10 = new char[10];
			char *ucod21 = new char[10];
			sprintf(ucod10, "%06.0lf", (double) codigo10);
			sprintf(ucod21, "%06.0lf", (double) codigo21);
			dbplu.Setorder("mplucod");
			if(dbplu.Seek(ucod10) && dbplu.Seek(ucod21))
			{
				if (atof(c_saldopromo) >= (valdescto))
					acumSaldoPromo = (valdescto);
				else
				{
					acumSaldoPromo = atof(c_saldopromo);
					valdescto = acumSaldoPromo;
				}
			}
			else
				valdescto = 0;
			delete ucod10;
			delete ucod21;
		}
		else
			valdescto = 0;
	}

	if (valdescto >= 0.01)
		CommonDescto(codigosel, valdescto, 1, soloSimular);	

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Accion que permite restar un monto fijo
ValuePtr F_CuotaSinInteres(ValuePtr args[])
{
	// Obtiene los parametro de la funcion.
	
	double monto = args[0]->ValueDouble();
	double nrocuota = args[1]->ValueDouble();
	
	delete args[1];

	//tresSinInteres = 0;
	//seisSinInteres = 0;

	if (nrocuota == 13)
		tresSinInteres = monto;
	else if (nrocuota == 16)
		seisSinInteres = monto;

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Accion que permite regalarle al cliente un articulo adicional (que nunca se tickeo)
ValuePtr F_RegalarMas(ValuePtr *args)
{
	int codigo = args[0]->ValueInteger();
	double cant = args[1]->ValueDouble();
	int flag = args[2]->ValueInteger();

	delete args[1];
	delete args[2];

	if(flag)
	{
		int vcant = (int)CommonTicketExplorer(codigo, 5);
		if(cant > vcant) cant = vcant;
		if(cant > 0)
		{
			dbplu.Setorder("mplucod");
			if(dbplu.Seek(padl((double)codigo, 6)))
			{
				char tprec[20];
				switch(listap)
				{
					case 2: dbplu.Get("precio2", tprec); break;
					case 3: dbplu.Get("precio3", tprec); break;
					case 4: dbplu.Get("precio4", tprec); break;
					default: dbplu.Get("precio", tprec); break;
				}
				CommonDescto(codigo, atof_(tprec) * cant, 0, soloSimular);
			}
		}
	}
	else
		CommonDescto(codigo, 0, cant, soloSimular);

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

//  Accion que permite regalar un articulo cualquiera de la cansta, asta llegar al monto requerido.
ValuePtr F_RegalarCanasta(ValuePtr *args)
{
	int canasNum = args[0]->ValueInteger();
	int cant = args[1]->ValueInteger();
	int flag = args[2]->ValueInteger();

	delete args[1];
	delete args[2];

	// Contendra la lista de codigos que forman la canasta.
	//::ArrayList *canasDef = new ::ArrayList(true, "RegalarCanasta");

	// Carga la definicion de la canasta.
	char msg[100];
	//dbf *canastas = new dbf();
	//if(canastas->Use(F_Canastas))
	//{
	//	// Levanta la definicion de la canasta
	//	for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
	//	{
	//		canastas->Get("canasta", msg);
	//		if(atoi(msg) == canasNum)
	//		{
	//			CanasDef *def = new CanasDef();
	//			canastas->Get("cod", msg);
	//			STRCPY(def->cod, msg);
	//			def->cant = 0;
	//			canasDef->Add(atoi(def->cod), def);
	//		}
	//	}
	//	canastas->Close();
	//}
	//delete canastas;
	Dump::lascanastas->Reset();
	Controles::MCanasta ^canasDef = Dump::lascanastas->GetCanasta(canasNum);

	// Scanea el ticket levantando los datos de los articulos que se vendieron
	//GCStatic::AddLockToTrans("RegalarCanasta");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", msg);
		int func = atoi(msg);
		tDbf.Get("anulado", msg);
		if(!tDbf.Deleted() && msg[0] != 'T' && func == 4)
		{
			// Obtiene y busca el articulo en cuestion.
			tDbf.Get("cod", msg);
			//CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			Controles::MCanasDef ^def = canasDef->Encontrar(atoi(msg));
			//if(def != NULL)
			if (def != nullptr)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("vuelto", msg);
				double unitario = atof(msg) / 100;
				if(uCant > cant) uCant = cant;
				cant -= (int)uCant;
				def->Cant += uCant;  //def->cant += uCant;
				def->Venta += unitario * uCant;  //def->venta += unitario * uCant;
			}

			if(cant <= 0) break;
		}
	}

	// Scanea el ticket levantando los datos de los articulos que se vendieron
	GCStatic::AddLockToTrans("RegalarCanasta");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", msg);
		int func = atoi(msg);
		tDbf.Get("anulado", msg);
		if(!tDbf.Deleted() && msg[0] != 'T' && func == 4)
		{
			// Obtiene y busca el articulo en cuestion.
			tDbf.Get("cod", msg);
			//CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			Controles::MCanasDef ^def = canasDef->Encontrar(atoi(msg));
			//if(def != NULL)
			if (def != nullptr)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("vuelto", msg);
				double unitario = atof(msg) / 100;
				if(uCant > cant) uCant = cant;
				cant -= (int)uCant;
				def->Cant += uCant;//def->cant += uCant;
				def->Venta += unitario * uCant;//def->venta += unitario * uCant;
			}

			if(cant <= 0) break;
		}
	}
	GCStatic::ReleaseLockFromTrans("RegalarCanasta");

	// Aplica los descuentos
	for(int i=0; i<canasDef->Count(); i++)
	{
		//CanasDef *def = (CanasDef *)canasDef->Item(i);
		Controles::MCanasDef ^def = canasDef[i];
		if(def->Cant > 0)  //if(def->cant > 0)
		{
			CommonDescto(def->Cod, def->Venta, 0, soloSimular);//CommonDescto(atoi(def->cod), def->venta, 0, soloSimular);
		}
	}

	delete canasDef;
	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

// Retorna la fecha como un numero.
ValuePtr F_Fecha(ValuePtr *args)
{
	Value *result = new Value((int)date());
	return result;
}

// Retorna un TRUE si la fecha actual esta en el rango que se indica.
ValuePtr F_EnFecha(ValuePtr *args)
{
	ValuePtr desde = args[0], hasta = args[1];
	int ahora = (int)date();
	bool result = (desde->ValueInteger() <= ahora) && (hasta->ValueInteger() >= ahora);
	desde->SetType('B');
	desde->_intValue = result? 1: 0;
	delete hasta;
	return desde;
}

// Retorna la hora como un numero.
ValuePtr F_Hora(ValuePtr *args)
{
	Value *result = new Value(((int)time_()) / 100);
	return result;
}

// Retorna TRUE si la hora actual esta en el rango que se indica.
ValuePtr F_EnHora(ValuePtr *args)
{
	ValuePtr desde = args[0], hasta = args[1];
	int ahora = ((int)time_()) / 100;
	bool result = (desde->ValueInteger() <= ahora) && (hasta->ValueInteger() >= ahora);
	desde->SetType('B');
	desde->_intValue = result? 1: 0;
	delete hasta;
	return desde;
}

// Retorna TRUE si el dia de la semana requerido es el dia de la semana de hoy
ValuePtr F_EsDia(ValuePtr *args)
{
	ValuePtr reqDia = args[0];
	bool result = (reqDia->ValueInteger() == dow());
	reqDia->SetType('B');
	reqDia->_intValue = result? 1: 0;
	return reqDia;
}

// Retorna el numero de caja..
ValuePtr F_Caja(ValuePtr *args)
{
	ValuePtr result = new Value(caja);
	return result;
}

// Retorna el numero de sucursal..
ValuePtr F_Sucursal(ValuePtr *args)
{
	ValuePtr result = new Value(suc);
	return result;
}

// Retorna el total de venta efectuado en el ticket.
ValuePtr F_Venta(ValuePtr *args)
{
	double vta = xTotal.VerTotal().ToDouble();
	double recfinaciero = xTotal.VerRecFinan();
	vta = vta-recfinaciero;
	return new Value(vta);
}

// Retorna el total de unidades vendidos en el ticket.
ValuePtr F_Unidades(ValuePtr *args)
{
	return new Value(CommonTicketExplorer(0, 7));
}

// Funcion comun para extraer datos simples del ticket.
double CommonTicketExplorer(int inum, int what)
{
	char tBuff[5], fBuff[5], dblBuff[20], nroBuff[10];
	int nro = 0, cod;
	double cant;
	bool flag;

	GCStatic::AddLockToTrans("CommonTicketExplorer");
	double result = 0;
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", fBuff);
		int func = atoi(fBuff);
		tDbf.Get("anulado", tBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T')
		{
			switch(what)
			{
				case 0: case 1:			// Venta o CANT de departamento.
					tDbf.Get("nro", nroBuff);
					nro = atoi(nroBuff);
					if((func == 4 || func == 1)	&& (nro == inum))
					{
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						if(what == 0)
							result += cant;
						else if(what == 1)
						{
							tDbf.Get("imp", dblBuff);
							result += atof(dblBuff);
						}
					}
					break;

				case 2:					// Cuenta las millas
				case 4:					// Venta de millas
					if(func == 4)
					{
						tDbf.Get("cod", dblBuff);
						dbplu.Setorder("mplucod");
						dbplu.Seek(padl(atof(dblBuff), 6));
						dbplu.Get("premio", nroBuff);
						nro = atoi(nroBuff);
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						switch(what)
						{
							case 2://contar Millas
								flag = (inum == 0)? (nro != 0): (inum == nro);
								if(flag) 
								{
									dbplu.Get("al_peso", nroBuff);
									//setea la promocion en el caso de promociones con milla = 1, cuando es pesable y cantidad < 1
									//para que tome un item del ticket como cantidad = 1 si la cantidad < 1 KG
									if (nroBuff[0] == 'T' && nro == 1 && cant < 1)
										cant = 1;
									result += cant;
								}
								break;

							case 4:
								flag = (inum == 0)? (nro != 0): (inum == nro);
								if(flag)
								{
									tDbf.Get("imp", dblBuff);
									result += atof(dblBuff);
								}
								break;
						}
					}
					break;

				case 5:							// Contar plus
				case 6:							// Sumar plus
					if(func == 4)
					{
						tDbf.Get("cod", dblBuff);						
						cod = atoi(dblBuff);
						if(cod == inum)
						{
							if(what == 5){
								//Begin Agregado
								char mns[42];
								dbplu.Setorder("mplucod");
								char *vcod = padl(atof(dblBuff), 6);
								dbplu.Seek(vcod);
								dbplu.Get("des", mns);
								VoucherVar("@CRED_NOMBRE@", mns);
								//End Agregado
								tDbf.Get("cant", dblBuff);
							}
							else
								tDbf.Get("imp", dblBuff);

							result += atof(dblBuff);
						}
					}
					break;

				case 7:							// Suma TOTAL de unidades vendidas
					if(func == 4 || func == 1)
					{
						tDbf.Get("cant", dblBuff);
						result += atof(dblBuff);
					}
					break;

				case 8: 
				case 9:			// Venta o CANT de subdepartamento.
					tDbf.Get("posi", nroBuff);
					nro = atoi(nroBuff);
					if((func == 4 || func == 1)	&& (nro == inum))
					{
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						if(what == 8)
							result += cant;
						else if(what == 1)
						{
							tDbf.Get("imp", dblBuff);
							result += atof(dblBuff);
						}
					}
					break;
				case 10:
					tDbf.Get("iva", dblBuff);
					nro = atoi(dblBuff);
					tDbf.Get("cod", dblBuff);
					if ((func == 4 || func == 1) && (inum == nro && !ExcluidosDePromo(atoi(dblBuff))))// != 99997))
					{					
						tDbf.Get("imp", dblBuff);
						result += atof(dblBuff);
					}
					break;
				case 11:					// Venta de millas
					if(func == 4)
					{
						tDbf.Get("iva", nroBuff);						
						nro = atoi(nroBuff);
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						//switch(what)
						//{
						//case 2://contar Millas
						//	flag = (inum == 0)? (nro != 0): (inum == nro);
						//	if(flag) 
						//	{
						//		dbplu.Get("al_peso", nroBuff);
						//		//setea la promocion en el caso de promociones con milla = 1, cuando es pesable y cantidad < 1
						//		//para que tome un item del ticket como cantidad = 1 si la cantidad < 1 KG
						//		if (nroBuff[0] == 'T' && nro == 1 && cant < 1)
						//			cant = 1;
						//		result += cant;
						//	}
						//	break;

						//case 4:
							flag = (inum == 0)? (nro != 0): (inum == nro);
							if(flag)
							{
								tDbf.Get("imp", dblBuff);			

								result += CalcNetoIva(atof(dblBuff), nro, 1);
							}
						/*	break;
						}*/
					}
					break;
			}
		}
	}

	GCStatic::ReleaseLockFromTrans("CommonTicketExplorer");
	return result;
}

// Retorna el total de unidades vendidas a un determinado departamento
ValuePtr F_CantDepto(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 0));
	return args[0];
}

// Retorna el total de ventas efectuadas a un determinado departamento
ValuePtr F_VentaDepto(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 1));
	return args[0];
}

ValuePtr F_ContarMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 2));
	return args[0];
}

ValuePtr F_VtaMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 4));
	return args[0];
}

ValuePtr F_ContarPlu(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 5));
	return args[0];
}

ValuePtr F_VentaPlu(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 6));
	return args[0];
}


ValuePtr F_VentaIva(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 10));
	return args[0];
}

ValuePtr F_VtaNetoMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonTicketExplorer(args[0]->ValueInteger(), 11));
	return args[0];
}


// Funcion comun para extraer datos simples del ticket.
double CommonSubTicketExplorer(int idep, int isubdep, int what)
{
	char tBuff[5], fBuff[5], dblBuff[20], nroBuff[16];
	int nro = 0, cod;
	double cant;
	bool flag;
	int ldep, niva, nmilla;
	double axcant = 0;

	GCStatic::AddLockToTrans("CommonTicketExplorer");
	double result = 0;
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", fBuff);
		int func = atoi(fBuff);
		tDbf.Get("anulado", tBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T')
		{
			switch(what)
			{				
				case 0: 
				case 1:			// Venta o CANT de subdepartamento.
					tDbf.Get("nro", nroBuff);
					ldep = atoi(nroBuff);
					tDbf.Get("posi", nroBuff);
					nro = atoi(nroBuff);
					if((func == 4 || func == 1)	&& (ldep == idep && nro == isubdep))
					{
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						if(what == 0)
							result += cant;
						else if(what == 1)
						{
							tDbf.Get("imp", dblBuff);
							result += atof(dblBuff);
						}
					}
					break;
				case 2:			// Cantidad de articulos marcados con una milla, discrimina iva
				case 3:			// Venta de articulos marcados con una milla, discrimina iva
					//idep = milla, isubdep = Nro iva					
					tDbf.Get("iva", nroBuff);
					niva = atoi(nroBuff);
					tDbf.Get("modo", nroBuff);
					nmilla = atoi(nroBuff);
					tDbf.Get("cod", nroBuff);
					if((func == 4 || func == 1) && (idep == nmilla && isubdep == niva && !ExcluidosDePromo(atoi(nroBuff))))// != 99997))
					{
						tDbf.Get("cant", dblBuff);
						cant = atof(dblBuff);
						if(what == 2)
							result += cant;
						else if(what == 3)
						{
							tDbf.Get("imp", dblBuff);
							result += atof(dblBuff);
						}
					}
					break;
				case 4:
					tDbf.Get("modo", nroBuff);
					nmilla = atoi(nroBuff);
					tDbf.Get("cod", nroBuff);
					if((func == 4 || func == 1) && (idep == nmilla && !ExcluidosDePromo(atoi(nroBuff))))// != 99997))
					{
						tDbf.Get("cant", nroBuff);
						axcant = atof(nroBuff);
						tDbf.Get("pesado", nroBuff);
						if (nroBuff[0]== 'T' && axcant < 1)							
							axcant = 1;						
						result += axcant;
					}
					break;
			}
		}
	}

	GCStatic::ReleaseLockFromTrans("CommonTicketExplorer");
	return result;
}


// Funcion comun para el calculo de promociones por depto, 2º articulo igual x% Desc
double CommonSubTicketExplorer2(int idep, int cantrep, int what, int smillas, int aejecutar)
{
	char tBuff[5], fBuff[5], dblBuff[20], nroBuff[10];
	int nro = 0, cod;
	double cant, auxmonto;
	bool flag;
	System::Collections::Generic::List<CPromRep ^> ^unalista = gcnew System::Collections::Generic::List<CPromRep ^>();
	bool cencontro = false;

	GCStatic::AddLockToTrans("CommonSubTicketExplorer2");
	double result = 0;
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", fBuff);
		int func = atoi(fBuff);
		tDbf.Get("anulado", tBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T')
		{
			tDbf.Get("nro", nroBuff);
			int ldep = atoi(nroBuff);
			tDbf.Get("posi", nroBuff);
			nro = atoi(nroBuff);
			if((func == 4 || func == 1)	&& (ldep == idep))// && nro == isubdep))
			{
				cencontro = false;
				tDbf.Get("Cod", dblBuff);
				String^ lccod = gcnew String(dblBuff);
				CPromRep ^unob;
				tDbf.Get("cant", dblBuff);
				cant = atof(dblBuff);
				tDbf.Get("imp", dblBuff);				
				auxmonto = atof(dblBuff);
				tDbf.Get("pesado", nroBuff);
				if (nroBuff[0] == 'T')
					continue;
				for (int kk = 0; kk < unalista->Count; kk++)
				{
					unob = unalista[kk];
					if (lccod->Trim() == unob->m_cod)
					{
						cencontro = true;
						unob->m_cant += cant;
						unob->m_monto += Decimal(auxmonto);
					}
				}
				if (!cencontro)
				{
					tDbf.Get("modo", nroBuff);//leo las millas del articulo					
					unob = gcnew CPromRep();
					unob->m_cod = lccod->Trim();
					unob->m_cant = cant;
					unob->m_monto = Decimal(auxmonto);
					unob->m_millas = atoi(nroBuff);
					unalista->Add(unob);
				}					
			}			
		}		
	}

	if (unalista->Count > 0)
	{
		for each(CPromRep^ otroobj in unalista)
		{
			int auxint = otroobj->m_cant / cantrep;
			if (auxint > 0)
			{
				Decimal auxpu = otroobj->m_monto / otroobj->m_cant;			
				switch(what)
				{
					case 0: 						
						result += Decimal::ToDouble(auxpu * auxint);						
						break;
					case 1:
						if (otroobj->m_millas == smillas)							
							result += Decimal::ToDouble(auxpu * auxint);						
						break;
					case 2:
						result += auxint;					
						break;
					case 3:
						if (otroobj->m_millas == smillas)						
							result += auxint;					
						break;					
					case 4:
						aejecutar = auxint > aejecutar ? aejecutar : auxint;
						if (smillas == 0 || (smillas > 0 && otroobj->m_millas != smillas))
							result += Decimal::ToDouble(auxpu * aejecutar);						
						break;					
					case 5:
						aejecutar = auxint > aejecutar ? aejecutar : auxint;
						if (smillas == 0 || (smillas > 0 && otroobj->m_millas != smillas))
							result += aejecutar;					
						break;

				}
			}
		}
		
	}

	unalista->Clear();
	delete unalista;

	GCStatic::ReleaseLockFromTrans("CommonSubTicketExplorer2");
	return result;
}

// Funcion comun para el calculo de promociones por marcado de millas
//del tipo 2do articulo igual X% desc, 3ro igual resta 1
double CommonSubTicketExplorer3(int pmillas, int cantrep, int aejecutar, int what)
{
	char tBuff[5], fBuff[5], dblBuff[20], nroBuff[10];
	int cod;
	double cant, auxmonto;
	bool flag;
	System::Collections::Generic::List<CPromRep ^> ^unalista = gcnew System::Collections::Generic::List<CPromRep ^>();
	bool cencontro = false;

	GCStatic::AddLockToTrans("CommonSubTicketExplorer3");
	double result = 0;
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", fBuff);
		int func = atoi(fBuff);
		tDbf.Get("anulado", tBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T')
		{
			tDbf.Get("modo", nroBuff);//leo las millas del articulo
			int tmillas = atoi(nroBuff);
			
			if((func == 4 || func == 1)	&& (tmillas == pmillas))
			{
				cencontro = false;
				tDbf.Get("Cod", dblBuff);
				String^ lccod = gcnew String(dblBuff);
				CPromRep ^unob;
				tDbf.Get("cant", dblBuff);
				cant = atof(dblBuff);
				tDbf.Get("imp", dblBuff);				
				auxmonto = atof(dblBuff);
				tDbf.Get("pesado", nroBuff);
				if (nroBuff[0] == 'T')
					continue;
				for (int kk = 0; kk < unalista->Count; kk++)
				{
					unob = unalista[kk];
					if (lccod->Trim() == unob->m_cod)
					{
						cencontro = true;
						unob->m_cant += cant;
						unob->m_monto += Decimal(auxmonto);
					}
				}
				if (!cencontro)
				{					
					unob = gcnew CPromRep();
					unob->m_cod = lccod->Trim();
					unob->m_cant = cant;
					unob->m_monto = Decimal(auxmonto);
					unob->m_millas = tmillas;//atoi(nroBuff);
					unalista->Add(unob);
				}					
			}			
		}		
	}

	if (unalista->Count > 0)
	{
		for each(CPromRep^ otroobj in unalista)
		{
			int auxint = otroobj->m_cant / cantrep;
			if (auxint > 0)
			{
				Decimal auxpu = 0;
				switch(what)
				{
					case 0: 						
						auxpu = otroobj->m_monto / otroobj->m_cant;
						result += Decimal::ToDouble(auxpu * auxint);						
						break;
					case 1:						
						result += auxint;					
						break;
					case 2:
						auxint = auxint > aejecutar ? aejecutar: auxint;
						auxpu = otroobj->m_monto / otroobj->m_cant;
						result += Decimal::ToDouble(auxpu * auxint);						
						break;
					case 3:
						auxint = auxint > aejecutar ? aejecutar: auxint;
						result += auxint;					
						break;
				}
			}
		}

		if (what == 4 && unalista->Count>0)
		{
			if (unalista->Count == 1)
				result = System::Convert::ToUInt32(unalista[0]->m_cod);
			else
			{
				System::Random ^rnd = gcnew System::Random();
				int indicd = rnd->Next(0, unalista->Count);
				result = System::Convert::ToUInt32(unalista[indicd]->m_cod);
			}

		}
	}

	unalista->Clear();
	delete unalista;

	GCStatic::ReleaseLockFromTrans("CommonSubTicketExplorer3");
	return result;
}


ValuePtr F_CantSubDepto(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer(args[0]->ValueInteger(), args[1]->ValueInteger(), 0));
	return args[0];
}

// Retorna el total de ventas efectuadas a un determinado departamento
ValuePtr F_VentaSubDepto(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer(args[0]->ValueInteger(), args[1]->ValueInteger(), 1));
	return args[0];
}

ValuePtr F_CantIvaPorMilla(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer(args[0]->ValueInteger(), args[1]->ValueInteger(), 2));
	return args[0];
}

ValuePtr F_VtaIvaPorMilla(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer(args[0]->ValueInteger(), args[1]->ValueInteger(), 3));
	return args[0];
}

ValuePtr F_ContMillaPromo(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer(args[0]->ValueInteger(), 0, 4));
	return args[0];
}

ValuePtr F_VentaDeptoRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 0, 0, 0));
	return args[0];
}

ValuePtr F_VentaDeptoRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 1, args[2]->ValueInteger(), 0));
	return args[0];
}

ValuePtr F_CantDeptoRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 2, 0, 0));
	return args[0];
}

ValuePtr F_CantDeptoRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 3, 
		args[2]->ValueInteger(), 0));
	return args[0];
}

ValuePtr F_VtaDeptoRepLimXCantMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 4, 
		args[2]->ValueInteger(), args[3]->ValueInteger()));
	return args[0];
}

ValuePtr F_CantDeptoRepLimXCantMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer2(args[0]->ValueInteger(), args[1]->ValueInteger(), 5, 
		args[2]->ValueInteger(), args[3]->ValueInteger()));
	return args[0];
}

ValuePtr F_VentaArtRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer3(args[0]->ValueInteger(), args[1]->ValueInteger(), 0, 0));
	return args[0];
}

ValuePtr F_CantArtRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer3(args[0]->ValueInteger(), args[1]->ValueInteger(), 0, 1));
	return args[0];
}

ValuePtr F_VentaLimArtRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer3(args[0]->ValueInteger(), args[1]->ValueInteger(), 
		args[2]->ValueInteger(), 2));
	return args[0];
}

ValuePtr F_CantLimArtRepMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer3(args[0]->ValueInteger(), args[1]->ValueInteger(), 
		args[2]->ValueInteger(), 3));
	return args[0];
}

ValuePtr F_ElijeCodAleaMillas(ValuePtr *args)
{
	args[0]->SetDouble(CommonSubTicketExplorer3(args[0]->ValueInteger(), 1, 0, 4));
	return args[0];
}


/********************************************************************************************************
 * Manejo de Canastas
 */

ValuePtr CommonCanasta(ValuePtr *args, int tipo)
{
	// Obtiene el numero de canasta a procesar.
	ValuePtr res = args[0];
	int canasNum = res->ValueInteger();

	// Contendra la lista de codigos que forman la canasta.
	//::ArrayList *canasDef = new ::ArrayList(true, "CommonCanasta");

	// Carga la definicion de la canasta.
	char msg[100];
	//dbf *canastas = new dbf();
	//if(canastas->Use(F_Canastas))
	//{
	//	// Levanta la definicion de la canasta
	//	for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
	//	{
	//		canastas->Get("canasta", msg);
	//		if(atoi(msg) == canasNum)
	//		{
	//			CanasDef *def = new CanasDef();
	//			canastas->Get("cod", msg);
	//			STRCPY(def->cod, msg);
	//			canastas->Get("cant", msg);
	//			def->cant = atof(msg);				
	//			canasDef->Add(atoi(def->cod), def);				
	//		}
	//	}
	//	canastas->Close();
	//}
	//delete canastas;

	// Scanea el ticket levantando los datos de los articulos que se vendieron
	//GCStatic::AddLockToTrans("CommonCanasta");

	Dump::lascanastas->Reset();
	Controles::MCanasta ^canasDef = Dump::lascanastas->GetCanasta(canasNum);
	// Scanea el ticket levantando los datos de los articulos que se vendieron
	GCStatic::AddLockToTrans("CommonCanasta");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", msg);
		int func = atoi(msg);
		tDbf.Get("anulado", msg);
		if(!tDbf.Deleted() && msg[0] != 'T' && func == 4)
		{
			// Obtiene y busca el articulo en cuestion.
			tDbf.Get("cod", msg);
			//CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			Controles::MCanasDef ^def = canasDef->Encontrar(atoi(msg));
			//if(def != NULL)
			if (def != nullptr)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("imp", msg);
				double uUnitario = atof(msg);
				tDbf.Get("vuelto", msg);
				def->PrecUni = atof(msg) / 100; //def->precuni = atof(msg) / 100;
				def->Venta += uCant; //def->venta += uCant;
				def->VentaMonto = uUnitario; //def->ventaMonto = uUnitario;
			}
		}
	}
	GCStatic::ReleaseLockFromTrans("CommonCanasta");	

	// Cuenta las canastas. Este valor se usa en varios lugares en el switch.
	int cuentaCanasta = 999999;
	for(int ix = 0; ix < canasDef->Count(); ix++)
	{
		//CanasDef *def = (CanasDef *)canasDef->Item(ix);
		Controles::MCanasDef ^def = canasDef[ix];
		//int cCanas = (int)(def->venta / ((def->cant == 0)? 1: def->cant));
		int cCanas = (int)(def->Venta / ((def->Cant == 0)? 1: def->Cant));
		if(cCanas < cuentaCanasta) cuentaCanasta = cCanas;
	}
	if(cuentaCanasta == 999999) cuentaCanasta = 0;

	// Determina el resultado y elimina la definicion de la canasta usada
	double resultVenta = 0;
	double cuentaLista = 0.0;
	switch(tipo)
	{
		case 1:							// CONTAR_CANASTA(...)
			res->SetInteger(cuentaCanasta);
			break;

		case 2:							// VENTA_CANASTA(...)
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				resultVenta += cuentaCanasta * def->Cant * def->VentaMonto;
			}
			res->SetDouble(resultVenta);
			break;

		case 3:							// CONTAR_LISTA(...)
			cuentaLista = 0.0;
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				cuentaLista += def->Venta * def->Cant;
			}
			res->SetDouble(cuentaLista);
			break;

		case 4:							// VENTA_LISTA(...)
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				resultVenta += def->Venta * def->PrecUni; //resultVenta += def->venta * def->precuni;
				//resultVenta += def->venta * def->ventaMonto;
			}
			res->SetDouble(resultVenta);
			break;
		case 5:							// PRECMIN_LISTA(...)
			resultVenta = 9999999;
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				//if (def->venta >= 1 && def->precuni < resultVenta)
				if (def->Venta >= 1 && def->PrecUni < resultVenta)
					resultVenta = def->PrecUni; //resultVenta = def->precuni;				
			}
			if (resultVenta == 9999999)
				resultVenta = 0;
			res->SetDouble(resultVenta);
			break;

	}

	// Limpia lo usado y retorna
	delete canasDef;
	return res;
}

//
//Similar al metodo anterior salvo que solo considera listas
//
double CommonCanasta2(int canasNum, int aejecutar, int tipo)
{
	double res = 0;
	// Contendra la lista de codigos que forman la canasta.
	//::ArrayList *canasDef = new ::ArrayList(true, "CommonCanasta2");

	// Carga la definicion de la canasta.
	char msg[100];
	//dbf *canastas = new dbf();
	//if(canastas->Use(F_Canastas))
	//{
	//	// Levanta la definicion de la canasta
	//	for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
	//	{
	//		canastas->Get("canasta", msg);
	//		if(atoi(msg) == canasNum)
	//		{
	//			CanasDef *def = new CanasDef();
	//			canastas->Get("cod", msg);
	//			STRCPY(def->cod, msg);
	//			canastas->Get("cant", msg);
	//			def->cant = atof(msg);				
	//			canasDef->Add(atoi(def->cod), def);				
	//		}
	//	}
	//	canastas->Close();
	//}
	//delete canastas;

	// Scanea el ticket levantando los datos de los articulos que se vendieron
	//GCStatic::AddLockToTrans("CommonCanasta2");
	Dump::lascanastas->Reset();
	Controles::MCanasta ^canasDef = Dump::lascanastas->GetCanasta(canasNum);

	// Scanea el ticket levantando los datos de los articulos que se vendieron
	GCStatic::AddLockToTrans("CommonCanasta2");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", msg);
		int func = atoi(msg);
		tDbf.Get("anulado", msg);
		if(!tDbf.Deleted() && msg[0] != 'T' && func == 4)
		{
			// Obtiene y busca el articulo en cuestion.
			tDbf.Get("cod", msg);
			//CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			Controles::MCanasDef ^def = canasDef->Encontrar(atoi(msg));
			//if(def != NULL)
			if (def != nullptr)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("imp", msg);
				double uUnitario = atof(msg);
				tDbf.Get("vuelto", msg);
				def->PrecUni = atof(msg) / 100; //def->precuni = atof(msg) / 100;
				def->Venta += uCant; //def->venta += uCant;
				def->VentaMonto = uUnitario; //def->ventaMonto = uUnitario;
			}
		}
	}
	GCStatic::ReleaseLockFromTrans("CommonCanasta2");
	
	// Determina el resultado y elimina la definicion de la canasta usada
	double resultVenta = 0;
	double cuentaLista = 0.0;
	switch(tipo)
	{
		case 0:							// 
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				if (def->Venta > 0 && def->Venta >= def->Cant)//	if (def->venta > 0 && def->venta >= def->cant)//
					resultVenta += ((int) (def->Venta / def->Cant)) * def->PrecUni; //resultVenta += ((int) (def->venta / def->cant)) * def->precuni;
				
			}
			res =resultVenta;
			break;

		case 1:							// 
			cuentaLista = 0.0;
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				Controles::MCanasDef ^def = canasDef[ix];//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				if (def->Venta > 0 && def->Venta >= def->Cant)//if (def->venta > 0 && def->venta >= def->cant)			
					cuentaLista += ((int) (def->Venta / def->Cant)); //cuentaLista += ((int) (def->venta / def->cant));
			}
			res = cuentaLista;
			break;

		case 2:				
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				Controles::MCanasDef ^def = canasDef[ix];//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				if (def->Venta > 0 && def->Venta >= def->Cant)	//if (def->venta > 0 && def->venta >= def->cant)
				{
					int cumplen = ((int) (def->Venta / def->Cant)); //int cumplen = ((int) (def->venta / def->cant));
					if (cumplen > aejecutar)
						cumplen = aejecutar;
					resultVenta += cumplen * def->PrecUni;//resultVenta += cumplen * def->precuni;
				}
			}
			res = resultVenta;
			break;
		case 3:		
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				Controles::MCanasDef ^def = canasDef[ix];//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				if (def->Venta > 0 && def->Venta >= def->Cant)//if (def->venta > 0 && def->venta >= def->cant)	
				{
					int cumplen = ((int) (def->Venta / def->Cant));//int cumplen = ((int) (def->venta / def->cant));
					if (cumplen > aejecutar)
						cumplen = aejecutar;
					cuentaLista += cumplen;
				}					
			}
			res = cuentaLista;
			break;

		case 4:							// 
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				Controles::MCanasDef ^def = canasDef[ix];
				if (def->Venta > 0 && def->Venta >= def->Cant)//	if (def->venta > 0 && def->venta >= def->cant)//
					resultVenta += ((int) (def->Venta)) * def->PrecUni; //resultVenta += ((int) (def->venta / def->cant)) * def->precuni;

			}
			res =resultVenta;
			break;

		case 5:							// 
			cuentaLista = 0.0;
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				Controles::MCanasDef ^def = canasDef[ix];//CanasDef *def = (CanasDef *)canasDef->Item(ix);
				if (def->Venta > 0 && def->Venta >= def->Cant)//if (def->venta > 0 && def->venta >= def->cant)			
					cuentaLista += ((int) (def->Venta)); //cuentaLista += ((int) (def->venta / def->cant));
			}
			res = cuentaLista;
			break;

	}

	// Limpia lo usado y retorna
	delete canasDef;
	return res;
}

ValuePtr F_CuentaCanasta(ValuePtr *args)
{
	return CommonCanasta(args, 1);
}

ValuePtr F_VentaCanasta(ValuePtr *args)
{
	return CommonCanasta(args, 2);
}

ValuePtr F_CuentaLista(ValuePtr *args)
{
	return CommonCanasta(args, 3);
}

ValuePtr F_VentaLista(ValuePtr *args)
{
	return CommonCanasta(args, 4);
}

ValuePtr F_PrecMinLista(ValuePtr *args)
{
	return CommonCanasta(args, 5);
}


ValuePtr F_VtaListaRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), 0, 0));
	return args[0];
}

ValuePtr F_CtaListaRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), 0, 1));
	return args[0];
}

ValuePtr F_VtaListaLimRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), args[1]->ValueInteger(), 2));
	return args[0];
}

ValuePtr F_CtaListaLimRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), args[1]->ValueInteger(), 3));
	return args[0];
}

ValuePtr F_VtaListaxCantRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), 0, 4));
	return args[0];
}

ValuePtr F_CtaListaxCantRep(ValuePtr *args)
{
	args[0]->SetDouble(CommonCanasta2(args[0]->ValueInteger(), 0, 5));
	return args[0];
}

ValuePtr F_TraeStockPremio(ValuePtr args[])
{
	
	int codrelo = args[0]->ValueInteger(); //Codigo regalo
	int aregalar = args[1]->ValueInteger(); //cantidad a regalar	
	

	int retvalue = 0;

	try 
	{
		if (aregalar > 0)
			//retvalue = Controles::PromoOnLine::GetPremios(gcnew String(p_Log), codrelo, caja, aregalar, soloSimular);
			retvalue = Controles::OperarServidor::GetPremioStock(gcnew String(ipServOC), portOC, codrelo, aregalar, caja, ckaNro, ticket,soloSimular);
	}
	catch (Exception ^excep)
	{
		System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion F_TraeEntradas", System::DateTime::Now);
		writer->WriteLine(fhdatos);		
		writer->WriteLine("Mensaje: " + excep->Message);
		writer->WriteLine("StackTrace: " + excep->StackTrace);
		writer->WriteLine("----------------------------------");
		writer->Close();
		retvalue = 0;
	}

	return new Value(retvalue);
}

ValuePtr F_VoucherC2(ValuePtr args[])
{
	char tmp[20];

	int codrelo = args[0]->ValueInteger(); //Codigo regalo
	int aregalar = args[1]->ValueInteger(); //cantidad a regalar	
	int voucherNum = args[2]->ValueInteger();
	int voucherCant = 0;
	
	delete args[2];
	delete args[1];

	voucherCant = Controles::OperarServidor::GetPremioStock(gcnew String(ipServOC), portOC, codrelo, aregalar, caja, ckaNro, ticket,soloSimular);

	if(voucherCant > 0)
	{
		for(int idup = 1; idup <= voucherCant; idup++)
		{
			VoucherVar("@COPIA@", idup);
			switch(idup)
			{
			case 1: VoucherVar("@COPIA_NOM@", "Original"); break;
			case 2: VoucherVar("@COPIA_NOM@", "Duplicado"); break;
			case 3: VoucherVar("@COPIA_NOM@", "Triplicado"); break;
			case 4: VoucherVar("@COPIA_NOM@", "Cuadruplicado"); break;
			default:
				sprintf(tmp, "Copia %d", idup);
				break;
			}

			if(!soloSimular)
				StackVoucher(voucherNum);
		}

		sprintf(tmp, "%d", voucherNum);
		RegisterPrize(1, tmp, "", "", false, 0);
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

ValuePtr F_VoucherC3(ValuePtr args[])
{
	char tmp[20];

	int voucherNum = args[0]->ValueInteger();
	/*int voucherCant = args[1]->ValueInteger();
	delete args[1];*/

	if (!soloSimular)
	{
		String ^rtaServ = "";

		rtaServ = Controles::OperarServidor::GetNumeroCupon(gcnew String(ipServOC), portOC, caja, ckaNro, pf->GetTickNro(GlobalTF));
		array<String ^> ^arrrta = rtaServ->Split(',');
		if (arrrta[0] == "OK")
		{
			Strings::StringToChar(arrrta[1], tmp,20);
			VoucherVar("@NUM_CUPON@", tmp); 		
			StackVoucher(voucherNum);		

			sprintf(tmp, "%d", voucherNum);
			RegisterPrize(1, tmp, "", "", false, 0);

			delete rtaServ;
		}
		/*else
			VoucherVar("@NUM_CUPON@", "9961F9E1"); 		*/		
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}


ValuePtr F_VoucherC4(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	delete args[1];

	if (!soloSimular)
	{
		String ^codpromo = "";
		String ^leyenda1 = "";
		String ^leyenda2 = "";
		String ^leyenda3 = "";
		String ^vdesde = "";
		String ^vhasta = "";

		Controles::OperarServidor::GetGeneraPremioStock(gcnew String(ipServOC), portOC,lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF),
			codpromo, leyenda1, leyenda2, leyenda3, vdesde, vhasta);
		
		if (codpromo != "")
		{
			Strings::StringToChar(codpromo, tmp,200);
			VoucherVar("@PS_CODPROMO@", tmp); 					

			Strings::StringToChar(leyenda1, tmp,200);
			VoucherVar("@PS_LEYENDA1@", tmp); 					

			Strings::StringToChar(leyenda2, tmp,200);
			VoucherVar("@PS_LEYENDA2@", tmp); 					

			Strings::StringToChar(leyenda3, tmp,200);
			VoucherVar("@PS_LEYENDA3@", tmp); 					
			
			Strings::StringToChar(vdesde, tmp,200);
			VoucherVar("@PS_VDESDE@", tmp); 					
			
			Strings::StringToChar(vhasta, tmp,200);
			VoucherVar("@PS_VHASTA@", tmp); 		

			StackVoucher(voucherNum);

			sprintf(tmp, "%d", voucherNum);
			RegisterPrize(1, tmp, "", "", false, 0);

			//delete codpromo;
			
		}
		delete codpromo;
		delete leyenda1;
		delete leyenda2;
		delete leyenda3;
		delete vdesde;
		delete vhasta;
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

ValuePtr F_VoucherC5(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	double mtodescto = args[2]->ValueDouble();

	delete args[1];

	if (!soloSimular)
	{
		String ^codpromo = "";
		String ^leyenda1 = "";
		String ^leyenda2 = "";
		String ^leyenda3 = "";
		String ^vdesde = "";
		String ^vhasta = "";

		Controles::OperarServidor::GetGeneraPremioStock(gcnew String(ipServOC), portOC,lpmilla, mtodescto, caja, ckaNro, pf->GetTickNro(GlobalTF),
			codpromo, leyenda1, leyenda2, leyenda3, vdesde, vhasta);

		if (codpromo != "")
		{
			frmIngNroDoc ^fingdoc = gcnew frmIngNroDoc();
			fingdoc->ShowDialog();
			Strings::StringToChar(fingdoc->NroDocumentoIng, tmp,200);
			VoucherVar("@DNI@", tmp); 					
			delete fingdoc;

			Strings::StringToChar(codpromo, tmp,200);
			VoucherVar("@PS_CODPROMO@", tmp); 					

			Strings::StringToChar(leyenda1, tmp,200);
			VoucherVar("@PS_LEYENDA1@", tmp); 					

			Strings::StringToChar(leyenda2, tmp,200);
			VoucherVar("@PS_LEYENDA2@", tmp); 					

			Strings::StringToChar(leyenda3, tmp,200);
			VoucherVar("@PS_LEYENDA3@", tmp); 					

			Strings::StringToChar(vdesde, tmp,200);
			VoucherVar("@PS_VDESDE@", tmp); 					

			Strings::StringToChar(vhasta, tmp,200);
			VoucherVar("@PS_VHASTA@", tmp); 		

			Strings::StringToChar(vhasta, tmp,200);
			VoucherVar("@PS_VHASTA@", tmp); 			

			StackVoucher(voucherNum);


			sprintf(tmp, "%d", voucherNum);
			RegisterPrize(1, tmp, "", "", false, 0);

			//delete codpromo;

		}
		delete codpromo;
		delete leyenda1;
		delete leyenda2;
		delete leyenda3;
		delete vdesde;
		delete vhasta;
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}


ValuePtr F_VoucherC6(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	//double mtodescto = args[2]->ValueDouble();

	delete args[1];

	if (!soloSimular)
	{
		String ^codpromo = "";
		String ^leyenda1 = "";
		String ^leyenda2 = "";
		String ^leyenda3 = "";
		
		try{
			Controles::OperarServidor::GetGeneraPremioStock(gcnew String(ipServOC), portOC,lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF),
				codpromo, leyenda1, leyenda2, leyenda3);

			if (codpromo != "")
			{
				Strings::StringToChar(codpromo, tmp,200);
				VoucherVar("@PS_CODPREMIO@", tmp); 					

				Strings::StringToChar(leyenda1, tmp,200);
				VoucherVar("@PS_PRLEYENDA1@", tmp); 					

				Strings::StringToChar(leyenda2, tmp,200);
				VoucherVar("@PS_PRLEYENDA2@", tmp); 					

				Strings::StringToChar(leyenda3, tmp,200);
				VoucherVar("@PS_PRLEYENDA3@", tmp);			

				StackVoucher(voucherNum);

				sprintf(tmp, "%d", voucherNum);
				RegisterPrize(1, tmp, "", "", false, 0);

				//delete codpromo;

			}
		}catch(System::Exception ^mex)
		{
			WriteBootLog("Error en Funcion F_Voucher6");
		}
		delete codpromo;
		delete leyenda1;
		delete leyenda2;
		delete leyenda3;		
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

ValuePtr F_VoucherC7(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	double mtovta = args[2]->ValueDouble();

	delete args[1];

	if (!soloSimular)
	{
		String ^codpromo = "";
		String ^leyenda1 = "";
		String ^leyenda2 = "";
		String ^leyenda3 = "";
		String ^vdesde = "";
		String ^vhasta = "";

		try{
		/*Controles::OperarServidor::GetGeneraVoucherStock22(gcnew String(ipServOC), portOC,lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF), mtovta,
			codpromo, leyenda1, leyenda2, leyenda3, vdesde, vhasta);*/
			System::Int64 vaCod = 0;
			System::Int64::TryParse(gcnew String(c_cod), vaCod);
			if (strlen(c_cod) >= 6 && strlen(c_cod) <= 11 && vaCod > 0)
			{		
				Controles::OperarServidor::GetGeneraVoucherStock33(gcnew String(ipServOC), portOC, lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF), mtovta, 
					vaCod, codpromo, leyenda1, leyenda2, leyenda3, vdesde, vhasta);

				if (codpromo != "")
				{
					Strings::StringToChar(codpromo, tmp,200);
					VoucherVar("@PS_CODPROMO@", tmp); 					

					Strings::StringToChar(leyenda1, tmp,200);
					VoucherVar("@PS_LEYENDA1@", tmp); 					

					Strings::StringToChar(leyenda2, tmp,200);
					VoucherVar("@PS_LEYENDA2@", tmp); 					

					Strings::StringToChar(leyenda3, tmp,200);
					VoucherVar("@PS_LEYENDA3@", tmp); 					

					Strings::StringToChar(vdesde, tmp,200);
					VoucherVar("@PS_VDESDE@", tmp); 					

					Strings::StringToChar(vhasta, tmp,200);
					VoucherVar("@PS_VHASTA@", tmp);

					VoucherVar("@PS_DNI@", c_cod); 

					StackVoucher(voucherNum);

					sprintf(tmp, "%d", voucherNum);
					RegisterPrize(1, tmp, "", "", false, 0);

					//delete codpromo;						

					pf->SetPie("TIENE UN VOUCHER CON PREMIO", 3);

				}
			}
		}catch(System::Exception ^mex)
		{
			WriteBootLog("Error en Funcion F_Voucher7");
		}

		delete codpromo;
		delete leyenda1;
		delete vdesde;
		delete vhasta;		
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

ValuePtr F_VoucherC8(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	double CodigoRegalo = args[2]->ValueDouble();

	delete args[1];

	if (!soloSimular)
	{
		String ^RtaRegalo = "";		

		Controles::OperarServidor::GetGeneraValeOtro(gcnew String(ipServOC), portOC,lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF), CodigoRegalo, RtaRegalo);

		if (RtaRegalo == "OK")
		{
			char *ucod = padl(CodigoRegalo, 6);
			dbplu.Setorder("mplucod");
			if(dbplu.Seek(ucod))
			{
				VoucherVar("@PS_CODVALEO@", ucod);

				dbplu.Get("des", tmp);
				VoucherVar("@PS_LEYVALEO@", tmp); 					

				StackVoucher(voucherNum);

				sprintf(tmp, "%d", voucherNum);
				RegisterPrize(1, tmp, "", "", false, 0);
			}

			//delete codpromo;
			pf->SetPie("TIENE UN VOUCHER CON PREMIO", 3);

		}
		delete RtaRegalo;		
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

ValuePtr F_VoucherC9(ValuePtr args[])
{
	char tmp[200];

	int voucherNum = args[0]->ValueInteger();
	int lpmilla = args[1]->ValueInteger();
	double mtovta = args[2]->ValueDouble();

	delete args[1];

	if (!soloSimular)
	{
		String ^codpromo = "";
		String ^leyenda1 = "";
		String ^leyenda2 = "";
		String ^leyenda3 = "";
		String ^vdesde = "";
		String ^vhasta = "";

		try{		
			System::Int64 vaCod = 0;
			System::Int64::TryParse(gcnew String(c_cod), vaCod);
			//Validacion agregada para diferenciar los clientes 
			//los que no son Fan club se les pondra el numero 77777777
			if (vaCod < 99999)
				vaCod = 77777777;
			if ((!(ClienteBenef >= 2 && ClienteBenef <= 9) || ClienteBenef == 11))
				vaCod = 77777777;
			if (vaCod > 0)
			{		
				Controles::OperarServidor::GetGeneraVoucherStock33(gcnew String(ipServOC), portOC, lpmilla, caja, ckaNro, pf->GetTickNro(GlobalTF), mtovta, 
					vaCod, codpromo, leyenda1, leyenda2, leyenda3, vdesde, vhasta);

				if (codpromo != "")
				{
					Strings::StringToChar(codpromo, tmp,200);
					VoucherVar("@PS_CODPROMO@", tmp); 					

					Strings::StringToChar(leyenda1, tmp,200);
					VoucherVar("@PS_LEYENDA1@", tmp); 					

					Strings::StringToChar(leyenda2, tmp,200);
					VoucherVar("@PS_LEYENDA2@", tmp); 					

					Strings::StringToChar(leyenda3, tmp,200);
					VoucherVar("@PS_LEYENDA3@", tmp); 					

					Strings::StringToChar(vdesde, tmp,200);
					VoucherVar("@PS_VDESDE@", tmp); 					

					Strings::StringToChar(vhasta, tmp,200);
					VoucherVar("@PS_VHASTA@", tmp);

					if (vaCod != 77777777)
						VoucherVar("@PS_DNI@", c_cod); 
					else
						VoucherVar("@PS_DNI@", " "); 

					StackVoucher(voucherNum);

					sprintf(tmp, "%d", voucherNum);
					RegisterPrize(1, tmp, "", "", false, 0);

					pf->SetPie("TIENE UN VOUCHER CON PREMIO", 3);

				}
			}
		}catch(System::Exception ^mex)
		{
			WriteBootLog("Error en Funcion F_Voucher7");
		}

		delete codpromo;
		delete leyenda1;
		delete vdesde;
		delete vhasta;		
	}

	args[0]->SetType('B');
	args[0]->_intValue = 1;
	return args[0];
}

/*********************************************************************************************************************
 *   Estructura de datos para el evaluador
*/
struct FuncDesc ApiFunctions[] =
{
	{ "CAJA",				0, {'Z'},					F_Caja },
	{ "CONTAR_CANASTA",		1, {'I'},					F_CuentaCanasta },
	{ "CONTAR_LISTA",		1, {'I'},					F_CuentaLista },
	{ "CONTAR_MILLAS",		1, {'I'},					F_ContarMillas },
	{ "CONTAR_PLU",			1, {'I'},					F_ContarPlu },
	{ "DESC_TOTAL",			3, {'D', 'D', 'I'},			F_DescTotal },
	{ "EN_FECHA",			2, {'I', 'I'},				F_EnFecha },
	{ "EN_HORA",			2, {'I', 'I'},				F_EnHora },
	{ "ES_DIA",				1, {'I'},					F_EsDia },
	{ "FECHA",				0, {'Z'},					F_Fecha },
	{ "HORA",				0, {'Z'},					F_Hora },
	{ "MENSAJE",			2, {'S', 'I'},				F_Mensaje },
	{ "PRECIO",             1, {'D'},					F_Precio },
	{ "REGALAR",			3, {'I', 'D', 'I'},	        F_RegalarMas },
	{ "REGALAR_CANASTA",    3, {'I', 'I', 'I'},         F_RegalarCanasta },
	{ "RESTAR_TOTAL",		2, {'D', 'I'},				F_RestaTotal },
	{ "RESTAR_TOTAL2",		3, {'D', 'D', 'I'},			F_RestaTotal2 },
	{ "SUCURSAL",			0, {'Z'},					F_Sucursal },
	{ "UNIDADES",			0, {'Z'},					F_Unidades },
	{ "VENTA",				0, {'Z'},					F_Venta },
	{ "VENTA_CANASTA",		1, {'I'},					F_VentaCanasta },
	{ "VENTA_DEP_CANT",		1, {'I'},					F_CantDepto },
	{ "VENTA_DEP_MONTO",	1, {'I'},					F_VentaDepto },
	{ "VENTA_LISTA",		1, {'I'},					F_VentaLista },
	{ "VENTA_MILLAS",		1, {'I'},					F_VtaMillas },
	{ "VENTA_PLU",			1, {'I'},					F_VentaPlu },
	{ "VOUCHER",			1, {'I'},					F_Voucher },
	{ "VOUCHERC",			2, {'I', 'I'},				F_VoucherC },
	{ "VENTA_SUBDEP_CANT",		2, {'I', 'I'},			F_CantSubDepto },
	{ "VENTA_SUBDEP_MONTO",	2, {'I', 'I'},				F_VentaSubDepto },
	{ "VENTA_DEP_MONTO_REP",	2, {'I', 'I'},			F_VentaDeptoRep },
	{ "VENTA_DEP_MONTO_REPMILLAS",	3, {'I', 'I','I'},	F_VentaDeptoRepMillas },
	{ "PRECIO_PVM",         2, {'D', 'D'},				F_Precio_Pvm },
	{ "UNIXBULTO",			1, {'D'},					F_CantUnidades },
	{ "VENTA_DEP_CANT_REP",	2, {'I', 'I'},				F_CantDeptoRep },
	{ "VENTA_DEP_CANT_REPMILLAS",	3, {'I', 'I','I'},	F_CantDeptoRepMillas },
	{ "VENTA_MILLAS_MONTO_XCANT",	2, {'I', 'I'},		F_VentaArtRepMillas },
	{ "VENTA_MILLAS_CANT_XCANT",	2, {'I', 'I'},		F_CantArtRepMillas },
	{ "VENTA_LISTA_REP",			1, {'I'},		F_VtaListaRep },
	{ "CONTAR_LISTA_REP",			1, {'I'},		F_CtaListaRep },
	{ "VENTA_LISTA_LIMREP",			2, {'I','I'},		F_VtaListaLimRep },
	{ "CONTAR_LISTA_LIMREP",		2, {'I','I'},		F_CtaListaLimRep },
	{ "VENTA_MILLAS_REPLIMXCANT",	3, {'I','I','I'},	F_VentaLimArtRepMillas },
	{ "CONTAR_MILLAS_REPLIMXCANT",	3, {'I','I','I'},	F_CantLimArtRepMillas },
	{ "VENTA_DEP_REPLIMXCANTMILLAS",	4, {'I','I','I','I'},	F_VtaDeptoRepLimXCantMillas },
	{ "CONTAR_DEP_REPLIMXCANTMILLAS",	4, {'I','I','I','I'},	F_CantDeptoRepLimXCantMillas },
	{ "PRECMIN_LISTA",		1, {'I'},					F_PrecMinLista },
	{ "VENTA_IVA",		1, {'I'},					F_VentaIva },
	{ "MENSAJE2",			2, {'S', 'I'},				F_Mensaje2 },
	{ "VTA_IVAXMILLA",		2, {'I', 'I'},			F_VtaIvaPorMilla },
	{ "CANT_IVAXMILLA",		2, {'I', 'I'},			F_CantIvaPorMilla },
	{ "CONTAR_PROMO",		1, {'I'},			F_ContMillaPromo },	
	{ "TRAER_PREMIOSTOCK",		2, {'I', 'I'},			F_TraeStockPremio },	
	{ "VOUCHER_STOCK",			3, {'I', 'I', 'I'},				F_VoucherC2 },
	{ "ELEGIRCOLEGIO",		1, {'I'},					F_SeleccionarColegio },
	{ "INGRESARSERIE",		1, {'I'},					F_IngresarSerie },
	{ "VOUCHER_CUPON",			1, {'I'},				F_VoucherC3 },
	{ "VOUCHER_PROMO",		2, {'I','I'},				F_VoucherC4 },
	{ "VOUCHER_DESCTO",		3, {'I','I','D'},				F_VoucherC5 },
	{ "VOUCHER_PINST",		2, {'I','I'},				F_VoucherC6 },
	{ "DESC_TOTIVA",		6, {'D','D','I','D','I','D'},				F_DescTotalIva },
	{ "VENTA_LISXCANT_REP",			1, {'I'},		F_VtaListaxCantRep },
	{ "CONTAR_LISXCANT_REP",			1, {'I'},		F_CtaListaxCantRep },
	{ "VENTA_NETO_MILLAS",		1, {'I'},					F_VtaNetoMillas },
	{ "VOUCHER_GENVOC",		3, {'I','I','D'},				F_VoucherC7 },
	{ "DESC_TOTIVACC",		5, {'D','D','I','D','I'},				F_DescTotalIva2 },
	{ "ELEGIRCODIGO",		1, {'I'},					F_ElijeCodAleaMillas },
	{ "VOUCHER_ROTRO",		3, {'I','I','I'},				F_VoucherC8 },
	{ "VOUCHER_GENVOCSC",		3, {'I','I','D'},				F_VoucherC9 },
	{ "CUOTA_SININT",		2, {'D','I'},				F_CuotaSinInteres },
};


int nApiFuncs = sizeof(ApiFunctions) / sizeof(struct FuncDesc);

int PreparePluginFunctions()
{
	IntegrateCustomFunctions(ApiFunctions, nApiFuncs);
	return 0;
}

