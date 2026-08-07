#include "STDAFX.H"
#include "MainForm.h"


extern int habil[];

char UltBalan[100];
char balanMsg[180];




void (*BalanzaRedirectedFunc)(char *);

int indBalan, AnularBalanza;

void ConsPluBalanza()
{
	char usacod[100];
	char usades[100];
	int found;
	double importe, x;

	if(xReg == 0.0) return;
	if (xReg > 999999)
	{
		Alert("Solo se consultan por Internos", "");
		return;
	}
	
	found = dbplu.Seek(padl(xReg, 6));	
	
	if(!found)
	{
		Alert("No encuentro al Articulo scaneado o tecleado", "");
		return;
	}
	dbplu.Get("cod", usacod);
	dbplu.Get("balanza", msg);
	if (msg[0] != 'T')
	{
		Alert("No es un articulo de Balanza", "");
		return;
	}
	
	dbplu.Get("des", usades);      // Toma los datos del articulo.
	//PatchRegionalChars(usades);
	switch(listap)
	{
		case 2:  dbplu.Get("precio2", msg);  break;
		case 3:  dbplu.Get("precio3", msg); break;
		case 4:  dbplu.Get("precio4", msg); break;		
		default: dbplu.Get("precio", msg); break;
	}

	x = atof_(msg);	
	importe = (pesoLeido > 0) ? (pesoLeido * x) : x;
	char tmsg[60];
	
	sprintf(tmsg, "Cod: %d", atoi(usacod));
	CustDisp3(usades, tmsg, HRng(importe));
	HClear();
	

}

int PesableEstaEnDep(char *dep){
	int depsearch = atoi(dep);
	int encontrado = 0;
	int todos = 0;	
	for(int i = 0; i < 50 ; i++){
		if (deptopesables[i] == depsearch){
			encontrado = 1;
			break;
		}
		else if (deptopesables[i] == -1)
			todos++;
	}
	//para que queden todos los departamentos habilitados. Es decir no tiene restricciones
	if (todos == 50)
		encontrado = 1;
	sprintf(msg, "Control de depto pesable -  Encontrado='%d', Deptoabuscar='%d'", encontrado, depsearch);
	WriteBootLog(msg);

	return encontrado;
}



void pesar_plu(int cual)
{
	int found;
	sprintf(PesadoEnBalanza, "%.3lf", 0.0);
	if (!TieneBalanza)
		return;	
	if (pesoLeido <= 0.001)
	{
		Alert("No se registra Peso. Colocar", "el articulo sobre la balanza");
		//WriteBootLog("AVISO - El codigo/scanner ingresado tiene parte decimal");
		return;
	}
	double locpeso = pesoLeido;

	char usacod[100];
	//char usades[100];
	char usaprec[50];
	char *mimssg = new char[1000];


	int antsuper = 0;
	
	if (EnAutocierre)
		return;	
	// Muestra un log de como se llamo a la funcion.
	sprintf(mimssg, "PLU-S1 - cual=%d, XReg=%lf, YReg=%lf, consmode=%d, pRepeat=%d, dRepeat=%d",
		cual, xReg, yReg, consmode, pRepeat, dRepeat);
	WriteBootLog(mimssg);

	double decpart, intpart;
	decpart = modf(xReg, &intpart);

	if ((decpart * 10000) > 0)
	{
		Alert("Error en CODIGO/SCANNER Ingresado", "Verificar CODIGO/SCANNER");
		WriteBootLog("AVISO - El codigo/scanner ingresado tiene parte decimal");
		return;
	}
	if (xReg <= 0 || xReg > 999999)
	{
		Alert("Error en CODIGO", "Verificar CODIGO, no pertenece a un articulo pesable");
		WriteBootLog("AVISO - El codigo pesable ingresado");
		return;
	}
			
	if(consmode) //FALTA EL MODO DE CONSULTA
	{
		ConsPluBalanza();
		pRepeat = dRepeat = 0;
		return;
	}
	if(inOper > 1 || globalCobraCC)     // Valida la fase de la operacion.
	{
		Alert("No puede vender Articulos Ahora.", "Termine la Cobranza Primero");
		return;
	}

	//   Valida otras condiciones previas para la funcion PLU.
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

	if(rMode)							// Estamos en X1? (Regresa a REG).
	{
		double tmp = xReg, tmp2 = yReg;
		xReg = 0.0;
		antsuper = supervNro;
		supervNro = 1;
		superv(0);
		xReg = tmp;
		yReg = tmp2;
	}		
		
	// Busca el PLU correspondiente. Esto lo hace SIEMPRE, dado que en xReg es esperable que SIEMPRE se
	// encuentre el codigo (o scanner) del articulo a buscar.
	
	dbplu.Setorder("mplucod");
	found = dbplu.Seek(padl(xReg, 6));
	//dplu.s_c_flag = 0;				// Indica que el codigo fue TECLEADO
	sprintf(mimssg, "PLU - Encontrado en PLU -  usacod='%s', xReg='%lf'", usacod, xReg);
	WriteBootLog(mimssg);	

	//   En este punto la base 'plu.dbf' DEBE encontrarse posicionada al articulo que se desea vender. La variable 'found' 
	//  es cero si el articulo NO esta definido.
	if(!found)
	{
		sprintf(mimssg, "[Codigo: %s]", usacod);
		Alert("No encuentro al Articulo scaneado o tecleado", mimssg);
		WriteBootLog("PLU - No encontrado");
		return;
	}	
	
	//   Obtiene del archivo el flag de si es o no articulo de venta al peso.
	dbplu.Get("cod", usacod);
	dbplu.Get("balanza", mimssg);
	if (mimssg[0] != 'T')
	{
		sprintf(mimssg, "[Codigo: %s]", usacod);
		Alert("El articulo tecleado no se vende por balanza", mimssg);
		sprintf(mimssg, "Codigo: %s, PLU no pesable, balanza = falso", usacod);
		WriteBootLog(mimssg);
		return;
	}

	dbplu.Get("dep", mimssg);
	if (!PesableEstaEnDep(mimssg))
	{
		int pdep = atoi(mimssg);
		sprintf(mimssg, "[Codigo: %s] -  depto invalido: %d", usacod, pdep);
		Alert("El articulo tecleado no se vende por balanza", mimssg);
		sprintf(mimssg, "Codigo: %s, PLU no pesable, no en lista depto. Depto buscado: %d", usacod, pdep);
		WriteBootLog(mimssg);
		return;
	}
	
	switch(listap)
	{
		case 2:  dbplu.Get("precio2", usaprec);  break;
		case 3:  dbplu.Get("precio3", usaprec); break;
		case 4:  dbplu.Get("precio4", usaprec); break;		
		default: dbplu.Get("precio", usaprec); break;
	}		
	System::Decimal rpeso = System::Convert::ToDecimal(locpeso);
	rpeso = System::Decimal::Round(rpeso, 3, System::MidpointRounding::AwayFromZero);
	System::Decimal rprecio = System::Convert::ToDecimal(atof(usaprec));
	rprecio = System::Decimal::Round(rprecio, 2, System::MidpointRounding::AwayFromZero);
	rprecio = rprecio * rpeso;
	rprecio = System::Decimal::Round(rprecio, 2, System::MidpointRounding::AwayFromZero);
	yReg = System::Convert::ToDouble(rprecio);//atof(usaprec) * pesoLeido;//System::Convert::ToDouble(formbalan->precioCalc);
	int auxpermpesado = permitePesado;
	permitePesado = 1;
	sprintf(PesadoEnBalanza, "%.3lf", locpeso);
	plu(1);
	permitePesado = auxpermpesado;	
	sprintf(PesadoEnBalanza, "%.3lf", 0.0);
	MainForm::F->BorrarPeso();
	delete mimssg;
}

// El siguiente es el puerto al que se PREDEFINE que esta conectado el scanner.
void BalanzaScann(char *p)
{
	char x;

	x = p[1];	
	if(scanDebug)
	{
		sprintf(msg, "SCAN: %d (%c)", (int) x, x);
		WriteBootLog(msg);
	}

	lastact = time__();
	if((int) p[0] == COM_BALAN)              // Balanza
	{
		if(indBalan != 0 && (x < '0' || x > '9'))
		{
			if(AnularBalanza)
			{
				WriteBootLog("SCanner Anulado - Transmitido NAK al scanner");
				//txChar(COM_SCAN, 0x15);
			}
			else
			{
				//txChar(COM_BALAN, 6);
				UltBalan[indBalan - 1] = 0;				
				
				if(!RegisterLocked)
				{					
					sprintf(balanMsg, "Termino de procesar Peso '%s' enviada a PLU(1)", UltBalan);
					pesoLeido = atof_(UltBalan)/1000;
					/*if (frmBalan::F != nullptr)
						frmBalan::F->ActualizarPeso();*/
					//plu(1);
					
					WriteBootLog(balanMsg);
				}
			}
			indBalan = AnularBalanza = 0;
		}
		else
			if(x >= '0' && x <= '9')
			{
				if(indBalan < 15 && x >= '0' && x <= '9')
					UltBalan[indBalan++] = x;
				else
					AnularBalanza = 1;
			}
			return;
	}
}

//   La siguiente funcion queda corriendo en background, esperando
//   que se complete un codigo de scanner para ingresarlo al teclado.
void BalanzaBackground(void)
{
	char args[50];
	try
	{
		for( ;; )
		{
			reviseSerial();
			//if(ScannerRedirectedFunc != NULL)
				if(PeekEvent(E_SERIAL, args))
					BalanzaScann(args);
			tSwitch();
		}
	}
	catch (...)
	{		 
		WriteBootLog("ERROR - en funcion ScannerBackground");		 
	}
}

// Vacia todo posible dato recibido de la balanza.
void FlushBalanza()
{
	if(habil[COM_BALAN])
	{
		while(rxPoll(COM_BALAN))
		{
			rxChar(COM_BALAN);
			tSwitch();
		}
	}
	indBalan = 0;
	FlushEvents();
}


//   Inicializa el manejo del scanner. Deja residente una porcion de ella
//   que se encarga de atender los eventos por ingreso de puerto serial
//   que se produzcan.
int InitBalanza()
{
	//ScannerRedirectedFunc = NULL;
	defineEvent(E_SERIAL, BalanzaScann);
	fork(BalanzaBackground, "BALANZA_ATT");
	return 1;
}



