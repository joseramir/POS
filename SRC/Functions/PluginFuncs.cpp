//
//   PluginFunctions.cpp
//   Contiene las funciones que se agregan al evaluador de expresiones, para permitir 'customizar' el
//   codigo que se genere.
//

#include "stdafx.h"
#include "funcs.h"

// Definicion de canastas
class CanasDef
{
public:
	char cod[12];
	double cant;
	double venta;
	double ventaMonto;

	CanasDef()
	{
		cant = 0;
		venta = 0;
		ventaMonto = 0;
	}
};

/*********************************************************************************************************************
 *   Funciones internas.
 *   RECORDAR QUE DEBEN ELIMINAR LOS ARGUMENTOS QUE NO RETORNAN
*/

// Funciones y variables externas
extern int caja, suc;
extern double pTot, xCnt;
extern dbf tDbf, dbplu;
extern unsigned long ActualTrans;
extern char msg[];
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
		dbplu.Get("precio", bPrec);
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
		RegisterPrize(1, tmp, "", "");
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
					strcpy(pDst, resVal);
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
	RegisterPrize(2, tMsg, "", "");

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
	if(dbplu.Seek(ucod))
	{
		if(simular)
			xTotal.AddDescuento(BDecimal(monto));
		else
		{
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
			plu.precuni = (unsigned long)(monto * 100);
			plu.s_c_flag = 0;
			plu.uCant = 0;
			store(-monto, plu.xReg);
			store(cant, plu.yReg);
			strcpy(plu.origCode, ucod);

			dbplu.Get("iva", msg);
			plu.iva = atoi(msg);
			dbplu.Get("dep", msg);
			plu.dnro = atoi(msg);
			plu.esBulto = 0;
			plu.uni = 1;

			pf->PrintText(actualPromo->Descrip);
			ProcPlu(&plu, 1);
			char tmp[20];
			sprintf(tmp, "%d", codigo);
			RegisterPrize(3, tmp, plu.yReg, plu.xReg);
		}
	}
	else
	{
		sprintf(msg, "Error en PROMO (RESTAR_TOTAL) - El plu '%d' no existe", codigo);
		WriteBootLog(msg);
	}
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

	CommonDescto(codigo, monto * tasa / 100, 1, soloSimular);

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
	::ArrayList *canasDef = new ::ArrayList(true);

	// Carga la definicion de la canasta.
	char msg[100];
	dbf *canastas = new dbf();
	if(canastas->Use(F_Canastas))
	{
		// Levanta la definicion de la canasta
		for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
		{
			canastas->Get("canasta", msg);
			if(atoi(msg) == canasNum)
			{
				CanasDef *def = new CanasDef();
				canastas->Get("cod", msg);
				strcpy(def->cod, msg);
				def->cant = 0;
				canasDef->Add(atoi(def->cod), def);
			}
		}
		canastas->Close();
	}
	delete canastas;

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
			CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			if(def != NULL)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("vuelto", msg);
				double unitario = atof(msg) / 100;
				if(uCant > cant) uCant = cant;
				cant -= (int)uCant;
				def->cant += uCant;
				def->venta += unitario * uCant;
			}

			if(cant <= 0) break;
		}
	}
	GCStatic::ReleaseLockFromTrans("RegalarCanasta");

	// Aplica los descuentos
	for(int i=0; i<canasDef->Count(); i++)
	{
		CanasDef *def = (CanasDef *)canasDef->Item(i);
		if(def->cant > 0)
		{
			CommonDescto(atoi(def->cod), def->venta, 0, soloSimular);
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
	double vta = xTotal.VerTotal();
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
							case 2:
								flag = (inum == 0)? (nro != 0): (inum == nro);
								if(flag) result += cant;
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
							if(what == 5)
								tDbf.Get("cant", dblBuff);
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

/********************************************************************************************************
 * Manejo de Canastas
 */

ValuePtr CommonCanasta(ValuePtr *args, int tipo)
{
	// Obtiene el numero de canasta a procesar.
	ValuePtr res = args[0];
	int canasNum = res->ValueInteger();

	// Contendra la lista de codigos que forman la canasta.
	::ArrayList *canasDef = new ::ArrayList(true);

	// Carga la definicion de la canasta.
	char msg[100];
	dbf *canastas = new dbf();
	if(canastas->Use(F_Canastas))
	{
		// Levanta la definicion de la canasta
		for(canastas->Go(DB_GO_TOP); !canastas->Eof(); canastas->Skip(1))
		{
			canastas->Get("canasta", msg);
			if(atoi(msg) == canasNum)
			{
				CanasDef *def = new CanasDef();
				canastas->Get("cod", msg);
				strcpy(def->cod, msg);
				canastas->Get("cant", msg);
				def->cant = atof(msg);
				canasDef->Add(atoi(def->cod), def);
			}
		}
		canastas->Close();
	}
	delete canastas;

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
			CanasDef *def = (CanasDef *)canasDef->Find(atoi(msg));
			if(def != NULL)
			{
				tDbf.Get("cant", msg);
				double uCant = atof(msg);
				tDbf.Get("imp", msg);
				double uUnitario = atof(msg);
				def->venta += uCant;
				def->ventaMonto = uUnitario;
			}
		}
	}
	GCStatic::ReleaseLockFromTrans("CommonCanasta");

	// Cuenta las canastas. Este valor se usa en varios lugares en el switch.
	int cuentaCanasta = 999999;
	for(int ix = 0; ix < canasDef->Count(); ix++)
	{
		CanasDef *def = (CanasDef *)canasDef->Item(ix);
		int cCanas = (int)(def->venta / ((def->cant == 0)? 1: def->cant));
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
				CanasDef *def = (CanasDef *)canasDef->Item(ix);
				resultVenta += cuentaCanasta * def->cant * def->ventaMonto;
			}
			res->SetDouble(resultVenta);
			break;

		case 3:							// CONTAR_LISTA(...)
			cuentaLista = 0.0;
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				CanasDef *def = (CanasDef *)canasDef->Item(ix);
				cuentaLista += def->venta * def->cant;
			}
			res->SetDouble(cuentaLista);
			break;

		case 4:							// VENTA_LISTA(...)
			for(int ix = 0; ix < canasDef->Count(); ix++)
			{
				CanasDef *def = (CanasDef *)canasDef->Item(ix);
				resultVenta += def->venta * def->ventaMonto;
			}
			res->SetDouble(resultVenta);
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
};

int nApiFuncs = sizeof(ApiFunctions) / sizeof(struct FuncDesc);

int PreparePluginFunctions()
{
	IntegrateCustomFunctions(ApiFunctions, nApiFuncs);
	return 0;
}

