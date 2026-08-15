//
//   Cajero.c
//   Implementa la utilizacion de cajeros en linea.
//

#include <stdafx.h>

#include "funcs.h"          // Carga la estructura clk_

char *ckaNom = new char[50];
char *supervNom = new char[50];
char *ckaCod = new char[7];
char *ckaFlag = new char[2];
char *tMsg = new char[200];
bool initGetsPtr = true;
char **value = (char **)new char[20][12];
int modoarq, ultCajero;
dbf dbclk;
int nocieTke, resuArqueo, pdeta, pdetaArqueo, pdetaFinArqueo, tipoArqueo;	
extern ::ArrayList *mpconso;
int tickVltoIndex;
::ArrayList *sortedMedios;
int supervDispNro = -1;
int ckaDispNro = -1;
Choose *opcArqu = NULL;
EditWindow *clkWin = NULL;
static char *uCod = new char[5];
void DisplayEstadoCaja();			// emax.cpp
#ifdef POSXP
#include "ArqueoTipo1.h"
#include "ArqueoTipo2.h"
#include "frmIngCodSuperv.h"
#endif

//   Prototipos
int FullArqueo(void);
double PrintAndDump(struct mpag_ *);
void PrintDatosUtiles(AcumCajero *acum, bool enArqueo);		// subtot.cpp
AcumCajero *Resumir(AcumCajero *vtas);			// mpago.cpp

// Elimina a un numero los caracteres de formateo.
char *ClearFormat(char *s)
{
	char *res = s, *d = s;
	while(*s)
	{
		if(*s == '.' || (*s >= '0' && *s <= '9'))
			*d++ = *s;
		s++;
	}
	*d = 0;
	return res;
}

//   Verifica si hay un cajero conectado.
//   De no ser asi, retorna TRUE.
int chkClk()
{
	if(ckaNro || !cko_enab) return 0;
	Alert("No puede operar la registradora todavia.", "No hay cajero habilitado.");
	return 1;
}

//   Al iniciar un ticket, imprime los datos del cajero en el impresor.
void PrintClerk()
{
	char tNom[60];

	if(!cko_enab) return;

	memset(tNom, 0, 60);
	STRCPY(tNom, "Caja: ");
	_itoa(caja, tNom + strlen(tNom), 10);
	if(rMode <= 1)
	{
		STRCAT(tNom, ", ");
		STRCAT(tNom, clkName()+4);
	}
	
	if(pf->GlobalVoucherFlag) 
		pf->PrintVoucher(tNom);
	while(strlen(tNom) < 40)
		STRCAT(tNom, "-");
	pf->SetHeader(tNom);

	VoucherVar("@CAJERO@", Trim(tNom));
	VoucherVar("@CAJA@", Trim(tNom));
}

char *supervName()
{
	int abiertoAqui;
	long aRec;
	char tempNom[50];

	if(supervNro == supervDispNro) return supervNom;
	supervDispNro = supervNro;
	if(supervNro <= 0)
	{
		STRCPY(supervNom, "[Ninguno]");
		return supervNom;
	}

	// Obtiene el nombre del supervisor de la base de cajeros.
	if(!dbclk.Opened())
	{
		abiertoAqui = 1;
		dbclk.Use(F_Cajeros);
	}
	else
	{
		abiertoAqui = 0;
		aRec = dbclk.Recno();
	}

	if(!dbclk.Opened()) AbnormalEnd("No puedo abrir CAJEROS.DBF o su equivalente");

	char supervCod[10];
    sprintf(supervCod, "%4d", (int) supervNro);
	if(dbclk.Locate("codigo", supervCod))
		dbclk.Get("nombre", tempNom);
	 else
		STRCAT(tempNom, "Desconocido");
	
	if(abiertoAqui)
		dbclk.Close();
	else
		dbclk.Go(aRec);

	sprintf(supervNom, "[%d]-%s", supervNro, tempNom);
	return Trim(supervNom);
}

char *clkName()
{
	int abiertoAqui;
	long aRec;
	char tempNom[50];

	if(ckaNro == ckaDispNro) return ckaNom;
	ckaDispNro = ckaNro;
	if(!dbclk.Opened())
	{
		abiertoAqui = 1;
		dbclk.Use(F_Cajeros);
	}
	else
	{
		abiertoAqui = 0;
		aRec = dbclk.Recno();
	}
	if(!dbclk.Opened()) AbnormalEnd("No puedo abrir CAJEROS.DBF o su equivalente");

	STRCPY(ckaNom, "Caj:");
    sprintf(ckaCod, "%4d", (int) ckaNro);
	if(dbclk.Locate("codigo", ckaCod))
		dbclk.Get("nombre", tempNom);
	 else
		STRCAT(tempNom, "Desconocido");
	
	if(abiertoAqui)
		dbclk.Close();
	else
		dbclk.Go(aRec);
	sprintf(ckaNom, "Caj:%d-%s", ckaNro, tempNom);
	return Trim(ckaNom);
}

void ProcSuperv(void *p_, int fwrite)
{
	struct DDSuperv_ *p;
	p = (struct DDSuperv_ *)p_;
	if(fwrite) 
		WriteDump(p);		// No escribir un WriteEOPNoFiscal() - El superv puede entrar en medio de un ticket.

	// Activa el supervisor actual.
	if(p->logflg){
		supervNro = p->nro;
		numSuperv = supervNro;
	}
	else
		supervNro = 0;
	
	supervName();

	// Graba en CMOS
	WriteCmos(SUPERVISOR);
}

void ProcClerk(void *p_, int fwrite)
{
    struct DDclk_ *p;
	char tFileName[100];

    p = (struct DDclk_ *) p_;
    if(fwrite) WriteDump(p);
	if(p->logflg)
		ultCajero = ckaNro = p->nro;
	else
	{
		// El cajero se esta cerrando. Grabamos la venta hasta ahora del cajero en el server.
		if(ckaNro != 0)
		{
			if(CheckNetwork())
			{
				//Imprime la venta remota de cajeros
				sprintf(tFileName, "%s\\%02d%06d.%03d", p_Log, ckaNro, (date() % 1000000), caja);							
				QuickVentas(tFileName, ckaNro);
				sprintf(tMsg, "Ventas Generadas. Cajero %d, Archivo: '%s'", ckaNro, tFileName);
				WLog(tMsg);				
			}
			else
			{
				Alert(
					"El Server NO ESTA Disponible en este momento.", 
					"Los arqueos a este cajero podrian verse afectados.");
				sprintf(tMsg, "ERROR - No puedo grabar Vtas de Cajero: ckaNro=%d, CheckNetwork()=%d",
					ckaNro, CheckNetwork());
				WLog(tMsg);
			}
		}

		// Si estamos usando el plugin, libera los archivos que esten en uso
		#ifdef WFACT
			try
			{
				fact::FactPluginClass ^plugin = GCStatic::GetFactPlugin();
				if(plugin != nullptr)
				{
					plugin->CloseAll();
				}
			}
			catch(...) {}
		#endif


		// Resetea el numero de cajero actual
		ckaNro = 0;
	}

	WriteCmos(CAJERO);
	WriteCmos(ULTCAJERO);
	if(fwrite) WriteEOPNoFiscal();

	AdicClear();
	HClear();
	if(rMode == 1) rMode = 0;
	autoDetected = false;
}

void DispDetaArqueo(char *m1, char *m2)
{
	if(z_muestra)
	{
		char *msg = m1;
		if(msg[0] == '&') msg += 2;
		double uCant = 0, uMonto = 0;
		int paso = 1;
		char *sv;
		for(char *p = m2 + strlen(m2) -1; p > m2; p--)
		{
			switch(paso)
			{
				case 1:				// Buscando el final del monto
					if(!isdigit(*p) && (*p != '.'))
					{
						uMonto = atof_(p);
						paso++;
					}
					break;

				case 2:				// Buscando el comienzo de la cantidad
					if(*p == ']')
					{
						sv = p;
						*sv = 0;
						paso++;
					}
					break;

				case 3:				// Buscando el final de la cantidad
					if(*p == '[')
					{
						uCant = atof(p + 1);
						paso++;
					}
					break;
			}
		}
		if(paso > 2) *sv = ']';
		WriteTickTmp("", msg, uCant, 0, 0, uMonto, false);
	}
	DispDeta(m1, m2);
}

//   Funcion de comparacion para los arqueos
int SortArqueoFunc(const void *_mp1, const void *_mp2)
{
	struct mpag_ 
		*mp1 = (struct mpag_ *) _mp1,
		*mp2 = (struct mpag_ *) _mp2;
	int res = _stricmp(mp1->grptot, mp2->grptot);
	if(res != 0) return res;
	return mp1->nro - mp2->nro;
}

//   Ordena los medios de pago para el arqueo
::ArrayList *OrdenarMedios()
{
	::ArrayList *res = new ::ArrayList(false, "OrdenarMedios");
	for(int i=0; i<cMpag; i++)
		res->Add(i, &mpagos[i]);
	res->Sort(SortArqueoFunc);
	return res;
}

//   Funcion que efectua el cierre del cajero.
void CierraCajero()
{
	int res, y;
	mpag_ *mp;
	struct DDecla_ decl;
	double importe, tImporte, teorico, tTeorico, tBolsin;
	int teoCant;
	char tMsg[50];

	importe = tImporte = teorico = tTeorico = tBolsin = 0.0;
	int tipoA = tipoArqueo;
	if(tipoArqueo < 1 || tipoArqueo > 2)
	{
		if(opcArqu == NULL)
		{
			opcArqu = new Choose("Tipo de Arqueo", "ESC=Cancela", 20, 0, CHOOSE_ALIGN_LEFT, NULL);
			opcArqu->Add("Arqueo de Control (Durante el Dia)");
			opcArqu->Add("Arqueo de Cierre del Cajero");
		}
		tipoA = opcArqu->Anima();
		if(tipoA == 0) return;
		tipoA--;
	}
	if((nocieTke || tipoA == 2) && chkNoSusp(0))
	{
		Alert("No puede cerrar todavia.", "Existen ticket en espera");
		if(nocieTke == 2 || tipoA == 2) return;
	}
	pdeta = (tipoA == 1)? pdetaArqueo: pdetaFinArqueo;

	CustDisp(closed[0], closed[1]);
	if (ckaNro != 0)
		pf->OpenDrwr();

	// Edita el arqueo
	res = FullArqueo();

	// Graba e imprime el resultado.
	int cantTickVlto = 0;
	double montoTickVlto = 0;
	if(res)
	{
		int P_mpago, P_depto, P_cajero, P_vended;

		// Calcula los teoricos extendidos, que incluyen ventas del cajero en otras cajas.
		P_mpago = z_mpago; P_depto = z_depto; P_cajero = z_cajero;
		P_vended = z_vended;
		z_mpago = z_depto = z_vended = 0; z_cajero = 1;
		CalculateExpandedZeta(false);
		if(cajFlota) IncorporaVentaRemota(ckaNro);
		z_mpago = P_mpago; z_depto = P_depto; z_cajero = P_cajero;
		z_vended = P_vended;

		// Marca el cajero como cerrado.
		if(tipoA == 2)
		{
			dbclk.Use(F_Cajeros);
			if(dbclk.Opened())
			{
				sprintf(ckaCod, "%d", (int) ckaNro);
				if(dbclk.Locate("codigo", ckaCod))
				{
					STRCPY(msg, "T");
					dbclk.Replace("cerrado", msg);
				}
				dbclk.Close();
			}
		}

		// Registra el cierre en el log.
		if(!z_muestra) SendMsg("Imprimiendo Arqueo de Cajero", "");
		pf->OpenVoucher();

		char *m1 = "&4-ARQUEO  DE CONTROL-",
			 *m2 = "&4- CIERRE DE CAJERO -",
			 *m3 = "";
		DispDetaArqueo(((tipoA == 1)? m1: m2), m3);
		
		DispDetaArqueo(Trim(clkName()), "");
		DispDeta("", "");
		tImporte = 0.0;

		AcumCajero *teoricos = (AcumCajero *)ZetaCajeros->Find(ckaNro);
		int cmp = cMpag;
		if(resuArqueo)
		{
			teoricos = Resumir(teoricos);
			cmp = mpconso->Count();
		}

		::ArrayList *arqSort = OrdenarMedios();
		for(y = 0; y < cmp; y++)
		{
			if(resuArqueo)
				mp = (mpag_ *) mpconso->Item(y);
			else
				mp = (mpag_ *) arqSort->Item(y);
			
			if(mp->nro == 0) continue;			// Saltea 'tarjetas online'
			teorico = 0.0;
			teoCant = 0;
			if(teoricos != NULL)
			{
				AcumMedio *am = teoricos->VerAcumuladorMedio(mp->nro);
				if(am != NULL)
				{
					teorico = am->VerMontoCobros() + am->VerMontoDotaciones() + am->VerMontoVentas() - am->VerMontoRetiros() + am->VerMontoExtracciones();
					teoCant = am->VerCantidadCobros() + am->VerCantidadVentas();
				}
				if(retiCod == mp->nro)
				{
					teoCant += teoricos->VerCantidadRetiros();
					teorico += teoricos->VerMontoRetiros();
				}
			}

			if(mp->tipo == 'Z') 
			{
				cantTickVlto += teoCant;
				montoTickVlto += teorico;
				continue;
			}

			taCant = 0;

			//if(modoarq > 1)
				importe = PrintAndDump(mp);
			//else
			//{
			//	importe = atof_(ClearFormat(value[y]));
			//	DispDeta(mp->des, "");
			//}
			if((importe != 0.0) || (teorico != 0.0))
			{
				double bolsin = (tipoA == 2)? 0: mp->bolsin;

				tImporte += importe;
				tTeorico += teorico;
				tBolsin += bolsin;

				decl.func = DDecla;
				decl.mpag = (y + 1);
		        decl.cajero = ckaNro;
				store(importe, decl.imp);
				store(teorico, decl.teorico);
				sprintf(&decl.nombregrupo, "%s", mp->des);
				WriteDump(&decl);
			
				if(importe == 0) // && ((y == 0 && pdetaArqueo == 0) || (y > 0 && pdetaArqueo == 1)))
				{
					sprintf(tMsg, "&4%s", mp->des);
					DispDetaArqueo(tMsg, "");
				}

				//DispDetaArqueo("   RECONTADO.:", HRng(importe));
				sprintf(tMsg, "[%3d]%12.2lf", taCant, importe);
				DispDetaArqueo("   RECONTADO.:", tMsg);
				sprintf(tMsg, "[%3d]%12.2lf", teoCant, teorico);
				DispDetaArqueo("   TEORICO...:", tMsg);
				if(bolsin > 0)
					DispDetaArqueo("    BOLSIN...:", HRng(bolsin));

				DispDetaArqueo("   DIFERENCIA:", HRng(importe - teorico - bolsin));
				DispDeta("", "");
			}
		}
		delete arqSort;

		if(tImporte || tTeorico)
		{
			decl.func = DDecla;
			decl.mpag = 999;
			decl.cajero = ckaNro;
			decl.control = (tipoA == 1);
			store(tImporte, decl.imp);
			store(tTeorico, decl.teorico);
			sprintf(&decl.nombregrupo, "TOTALES");
			WriteDump(&decl);
		}

		PrintDatosUtiles(teoricos, true);

		char tMsg[100];

		DispDeta("" , "");
		DispDetaArqueo("TOT RECAUDADO.:", HRng(tImporte));
		if(montoTickVlto > 0)
		{
			sprintf(tMsg, "[%3d]%12.2lf", cantTickVlto, montoTickVlto);
			DispDetaArqueo("TICKET VUELTO:", tMsg);
		}
		DispDetaArqueo("TOT TEORICO...:", HRng(tTeorico));
		if(tBolsin > 0)
			DispDetaArqueo("TOT  BOLSIN...:", HRng(tBolsin));
		DispDetaArqueo("DIFERENCIA FINAL:", HRng(tImporte + montoTickVlto - tTeorico - tBolsin));

		STRCPY(tMsg, ".");
		pf->PrintVoucher(tMsg);
		pf->PrintVoucher(tMsg);
		pf->PrintVoucher(tMsg);
		pf->PrintVoucher(tMsg);
		sprintf(tMsg, "Firma:  _________________________________");
		pf->PrintVoucher(tMsg);
		sprintf(tMsg, "        %s", clkName() + 4);
		pf->PrintVoucher(tMsg);
		sprintf(tMsg, "Superv: %s", supervName());
		pf->PrintVoucher(tMsg);
		STRCPY(tMsg, ".");
		pf->PrintVoucher(tMsg);
		pf->CloseVoucher();
		if(!z_muestra) ClearMsg();

		ultCajero = 0;
		WriteCmos(ULTCAJERO);
		WriteEOPNoFiscal();

		xReg = 0.0;					// Vuelve a registro y desactiva.
		chgmode(1);
		xReg = 0.0;
		if(tipoA == 2) clerk(0);	// Desactiva el cajero solo en arqueos de cierre.
		xReg = 0;
		superv(0);
	}
}

//   Despues de la zeta, rehabilita TODOS los cajeros.
void OpenAllClerks()
{
	dbclk.Use(F_Cajeros);
	if(dbclk.Opened())
	{
		dbclk.Go(DB_GO_TOP);
		STRCPY(msg, "F");
		while(!dbclk.Eof())
		{
			dbclk.Replace("cerrado", msg);
			dbclk.Skip(1);
		}
		dbclk.Close();
	}
}

//   Funcion de procesamiento de la tecla de supervisor.
void superv(int cual)
{
	struct DDSuperv_ superv;
	char tempStr[20];
	bool unlog = false;

	// Si se pide desloguinear el supervisor, pero no hay ningun loguineado
	if(xReg == 0.0 && supervNro == 0) return;
	if((int)xReg == supervNro) unlog = true;

	// Si hay supervisor loguineado actualmente, lo desloguinea.
	int actual = supervNro;
	if(supervNro != 0)
	{
		// Primero, regresa el modo a registro si estaba en otro modo.
		double saveXReg = xReg;
		xReg = 0;
		chgmode(0);
		xReg = saveXReg;

		// Ahora si efectua el desloguineo
		superv.func = DSuperv;
		superv.nro = supervNro;
		superv.logflg = 0;
		superv.cajero = ckaNro;
		superv.fecha = date();
		superv.hora = time__();
		ProcSuperv(&superv, true);
	}
	
	// Si solo estabamos desloguineando al supervisor.
	if(unlog) 
	{
		clear(0);
		return;
	}

	// Login del nuevo supervisor
	if(xReg != 0.0)
	{
		dbclk.Use(F_Cajeros);
		if(!dbclk.Opened()) AbnormalEnd("No puedo abrir CAJEROS.DBF o su equivalente");

        sprintf(tempStr, "%4d", (int) xReg);
		if(!dbclk.Locate("codigo", tempStr))
		{
			Alert("Codigo de Supervisor Invalido", "");
			dbclk.Close();
			autoDetected = false;
			return;
		}

		dbclk.Get("nivel", tempStr);
		int nivel = atoi(tempStr);
		if(nivel < 3)// reemplazado if(nivel < 1)
		{
			Alert("El codigo ingresado corresponde a un cajero, no a",
				  "un supervisor habilitado.\n");
			dbclk.Close();
			autoDetected = false;
			return;
		}

		char uPassword[20];
		char fechaclave[20];
		supervNro = (int) xReg;
		dbclk.Get("nombre", supervNom); // + 4);
		dbclk.Get("clave", uPassword);

		dbclk.Get("feclav", fechaclave);
		Trim(uPassword);
		bool validPassword = true;
		bool validVigencia = true;
		if(!autoDetected)
		{
			// Valid apor pantalla
			if(uPassword[0] && !CheckPassword(supervNom, uPassword))//!CheckPassword(ckaNom, uPassword))
			{
				Alert("La clave ingresada no es la correcta.","");
				validPassword = false;
			}
		}
		else
		{
			validPassword = (fabs(atof(uPassword) - zReg) < 0.1);
			if(!validPassword)
				Alert("La tarjeta usada esta desactualizada.","");
			else//la clave es valida controlo la validez de la fecha
			{
				//long vigclave = atol(fechaclave);
				if (atol(fechaclave) > 0 && strlen(fechaclave) == 8)
				{
					System::DateTime dt = System::DateTime::ParseExact(gcnew System::String(fechaclave), "yyyyMMdd", System::Globalization::CultureInfo::InvariantCulture);
					dt = dt.AddDays(15);
					validVigencia = dt >= System::DateTime::Now.Date;
					if (!validVigencia)
						Alert("La tarjeta usada esta vencida.","");
				}				
			}
		}

		// Si la clave no era valida.
		if(!validPassword || !validVigencia)
		{
			dbclk.Close();
			supervNro = 0;
			clear(1);
			autoDetected = false;
			return;
		}

		// Si NO se autodetecto el supervisor, entonces debemos forzar un cambio de modo por menu. (o directamente a X1,
		// si no fuera un supervisor FULL).
		// En caso que fuera autodetectado, el cambio de modo es directamente efectuado en la rutina que identifico al
		// supervisor y que llamo a esta funcion.
		int chgModeOpc = -1;
		if(!autoDetected)
		{
			if(nivel > 2)
			{
				Choose *chs = new Choose(" Modo ", "", 1, 1, CHOOSE_ALIGN_CENTER, NULL);
				chs->Add("X1");
				chs->Add("X2");
				chs->Add("ZETA");
				chgModeOpc = chs->Anima();
				delete chs;
			}
			else
				chgModeOpc = 2;
		}

		//   Activa el nuevo supervisor.
		dbclk.Close();
		autoDetected = false;

		superv.nro = (int)xReg;
		superv.logflg = 1;
		superv.func = DSuperv;
		superv.fecha = date();
		superv.hora = time__();
		superv.cajero = ckaNro;
	    ProcSuperv(&superv, true);

		if(chgModeOpc > 1)
		{
			xReg = chgModeOpc - 1;
			autoDetected = true;
			chgmode(0);
			autoDetected = false;
		}

		clear(0);
	}
}

//   Solicita en pantalla el codigo y clave de un supervisor para autorizar puntualmente
//   una accion sensible (sin loguinear al supervisor ni cambiar el modo de la caja, a
//   diferencia de 'superv()'). Hasta 3 intentos; cada intento (exito, clave invalida o
//   cancelado) queda registrado en el log. Si autoriza, copia el nombre del supervisor
//   en 'outNombreSuperv'. Devuelve 1 si un supervisor autorizo la operacion, 0 sino.
int AutorizaSupervisorPuntual(char *msgAccion, char *outNombreSuperv, int maxLen)
{
#ifdef POSXP
	char tempStr[20], nombre[50], clave[20];
	int intentos = 0;

	if(outNombreSuperv != NULL && maxLen > 0)
		outNombreSuperv[0] = 0;

	while(intentos < 3)
	{
		pos::frmIngCodSuperv ^fcod = gcnew pos::frmIngCodSuperv();
		if(fcod->ShowDialog() != System::Windows::Forms::DialogResult::OK)
		{
			WLog("AUTORIZ.SUPERV - Cancelado por el usuario. Accion='%s'", msgAccion);
			return 0;
		}

		int codigo = System::Convert::ToInt32(fcod->CodigoIng);

		if(!dbclk.Use(F_Cajeros))
		{
			WLog("AUTORIZ.SUPERV - No pude abrir CAJEROS.DBF. Accion='%s'", msgAccion);
			return 0;
		}

		sprintf(tempStr, "%4d", codigo);
		bool encontrado = dbclk.Locate("codigo", tempStr) != 0;
		int nivel = 0;
		if(encontrado)
		{
			dbclk.Get("nivel", tempStr);
			nivel = atoi(tempStr);
		}

		if(!encontrado || nivel < 3)
		{
			dbclk.Close();
			intentos++;
			WLog("AUTORIZ.SUPERV - Codigo invalido o sin nivel de supervisor. Codigo=%d, Intento=%d/3, Accion='%s'", codigo, intentos, msgAccion);
			Alert("Codigo de Supervisor Invalido", "");
			continue;
		}

		dbclk.Get("nombre", nombre);
		dbclk.Get("clave", clave);
		Trim(nombre);
		Trim(clave);
		dbclk.Close();

		if(clave[0] && !CheckPassword(nombre, clave))
		{
			intentos++;
			WLog("AUTORIZ.SUPERV - Clave incorrecta. Supervisor='%s' (Cod=%d), Intento=%d/3, Accion='%s'", nombre, codigo, intentos, msgAccion);
			continue;
		}

		WLog("AUTORIZ.SUPERV - Autorizado. Supervisor='%s' (Cod=%d), Cajero=%d, Accion='%s'", nombre, codigo, ckaNro, msgAccion);
		if(outNombreSuperv != NULL && maxLen > 0)
		{
			strncpy(outNombreSuperv, nombre, maxLen - 1);
			outNombreSuperv[maxLen - 1] = 0;
		}
		return 1;
	}

	WLog("AUTORIZ.SUPERV - Intentos agotados (3). Accion='%s'", msgAccion);
	Alert("Demasiados intentos fallidos.", "Operacion cancelada.");
	return 0;
#else
	return 0;
#endif
}

//   Funcion de procesamiento de la tecla de cajero.
void clerk_(int cual)
{
	struct DDclk_ ck;
	char uPassword[10];

	if(!cko_enab)
	{
		//Alert("No puede usar cajeros, pues esta funcion", "fue deshabilitada en 'opciones.ini'");
		autoDetected = false;
		return;
	}
	if(inOper || xTotal.VerTotal() != 0)
	{
		Alert("No puede cambiar el cajero en este momento", "Cierre el ticket antes !!!");
		autoDetected = false;
		return;
	}
	if(rMode > 1)
	{
		CierraCajero();
		superv(0);
		DisplayEstadoCaja();
		return;
	}

	// Si se pide cerrar y no hay cajero abierto...
	if(xReg == 0.0 && (ckaNro == 0))
	{
		autoDetected = false;
		return;
	}

	//   Si hay un cajero abierto, lo cierra.
	if(ckaNro)
	{
		if(chkNoSusp(0) && rMode < 1)
		{
			Alert("No puede desactivar un cajero mientras existan", "Tickets en espera. Llame al Supervisor...");
			autoDetected = false;
			return;
		}

		ck.nro = ckaNro;
		ck.logflg = 0;
	    ck.func = DClk;
		ck.fecha = date();
		ck.hora = time__();
	    ProcClerk(&ck, 1);
		return;
	}

	//   Si la orden es de abrir un nuevo cajero.
	if(xReg != 0.0)
	{
		if(ultCajero != 0 && (int)xReg != ultCajero && rMode < 1 && clerk_ant)
		{
			sprintf(msg, "Esta caja estaba siendo atendida por el cajero %d", ultCajero);
			Alert(msg, "Llame al supervisor si igual necesita usarla...");
			autoDetected = false;
			return;
		}

		dbclk.Use(F_Cajeros);
		if(!dbclk.Opened()) AbnormalEnd("No puedo abrir CAJEROS.DBF o su equivalente");

        sprintf(ckaCod, "%4d", (int) xReg);
		if(!dbclk.Locate("codigo", ckaCod))
		{
			Alert("Codigo de Cajero Invalido", "");
			dbclk.Close();
			autoDetected = false;
			return;
		}
		dbclk.Get("cerrado", ckaFlag);
		if(ckaFlag[0] == 'T')
		{
			Alert("El cajero YA cerro y, por lo tanto,", "no puede cobrar hasta la siguiente ZETA.\n");
			dbclk.Close();
			autoDetected = false;
			return;
		}
		ckaNro = (int) xReg;
		STRCPY(ckaNom, "Caj:");
		dbclk.Get("nombre", ckaNom + 4);

		dbclk.Get("clave", uPassword);
		Trim(uPassword);
		bool validPassword = true;
		if(!autoDetected)
		{
			if(uPassword[0] && !CheckPassword(ckaNom, uPassword))
			{
				Alert("La clave ingresada no es la correcta.","");
				validPassword = false;
			}
		}
		else
		{
			validPassword = (fabs(atof(uPassword) - zReg) < 0.1);
			if(!validPassword)
				Alert("La tarjeta del cajero esta desactualizada.", "");
		}
		if(!validPassword)
		{
			dbclk.Close();
			ckaNro = 0;
			clear(1);
			return;
		}

		//   Activa el nuevo cajero.
		autoDetected = false;
		dbclk.Close();

		ck.nro = ckaNro;
		ck.logflg = 1;
	    ck.func = DClk;
		ck.fecha = date();
		ck.hora = time__();
	    ProcClerk(&ck, 1);
	}
	clear(1);
}

void clerk(int cual)
{
	clerk_(cual);
	DisplayEstadoCaja();
}

//  Rutina llamada al inicializar, que carga y abre el archivo
//  con las descripciones.
int LoadClerk()
{
	dbclk.Use(F_Cajeros);
	if(!dbclk.Opened()) AbnormalEnd("No puedo abrir CAJEROS.DBF o su equivalente");
	dbclk.Close();
	return 1;
}

//   Funcion de 'validacion' del codigo de cajero encontrado.
int buscaClk(char *cual, void *bag)
{
	static char uCod[10];
	int si;

	Padl(Trim(STRCPY(uCod, cual)), 4, ' ');
	if(dbclk.Locate("codigo", uCod))
	{
		clkWin->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el cajero '%s'.", cual);
	si = GetSiNo(msg, "Lo incorporo como cajero?");
	if(!si)
		return 0;
	dbclk.AppendBlank();
	dbclk.Replace("codigo", uCod);
	clkWin->ShowGets();
	return 1;
}

//   Abre una ventana y permite editar el archivo de departamentos.
void PrograClk()
{
	int hWin;

	CustDisp(closed[0], closed[1]);
	dbclk.Use(F_Cajeros);
	if(!dbclk.Opened())
	{
		Alert("No puedo abrir 'CAJEROS.DBF' o su equivalente", "");
		return;
	}

	hWin = wDefine(7, 18, 15, 62, BORDERSINGLE, SNGCURS, prograEsq, "Programacion de Cajeros", "");
	clkWin = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Numero:");
	wLocate(2, 2);  wPstr("Nombre:");

	clkWin->AddGet(1, 10, NUMBER_RIGHT_ALIGNED,  2, 0, uCod, NULL, NULL, buscaClk);
	clkWin->AddGet(2, 10, "nombre",     &dbclk, NULL, NULL, NULL);

	clkWin->AnimaGets(1);
	wRelease(hWin);
	delete clkWin; clkWin = NULL;
	dbclk.Close();
	HClear();
}	

/////////////////////////////////////////////////////////////////////////////////////////
int *cuenta;
double *monto;
Choose *mArqueo = NULL;
char *frmtLine = "%4d %11s";
char tmsg[1000];
char tmonto[80];
short arqueoEsq[16] = {         // Esquema de colores para choose.
	{ CELESTE * 16 + NEGRO },   // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ CELESTE * 16 + NEGRO },   // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno
	{ ROJO * 16 + BLANCO }      // Color cuando quiere resaltar el texto.
};
char num[20][10];
char tnum[50];
::ArrayList *arqData;
int tXCol3, tCant, outOpc;
double tMonto;
char *dstFormula;
xBrowseField *dstFld;
dbf *tdb;

//   Funcion de validacion para cada entrada del modo 1.
int ValidArqu(char *buff, void *bag)
{
	//   Determina el item que estamos validando
	int item;
	for(item = 0; item < 20; item++)
		if(buff == num[item])
			break;

	double res = atof(ClearFormat(arqData->ItemString(item + 4))) * atof(ClearFormat(buff));
	sprintf(tmsg, "%9.2lf", res);
	wSetColor(arqueoEsq[0]);
	wLocate(3 + item, tXCol3);
	wPstr(tmsg);
	return 1;
}

//   Arqueo modo 1 - Pantalla con datos fijos.
void Arqueo1(mpag_ *mp, int item, ::ArrayList *arq, ::ArrayList *aData, bool print)
{
	//  Preserva datos para la funcion de validacion.
	arqData = arq;

	//  Determina las coordenadas X donde se pondran los elementos en la form.
	int tXCol2 = strlen(arq->ItemString(1)); if(tXCol2 < 16) tXCol2 = 16;
	tXCol3 = tXCol2 + strlen(arq->ItemString(2)); if(tXCol3 < 28) tXCol3 = 28;
	int tAncho = tXCol3 + strlen(arq->ItemString(3)); if(tAncho  < 42) tAncho = 42;
	int tAlto  = arq->Count() + 1;

	//  Determina coordenadas y titulo de la form y la crea.
	int ty = (25 - tAlto) / 2, tx = (80 - tAncho) / 2;

	//  Imprime el encabezamiento de los datos del arqueo.
	int wHand = 0;
	if(print)
	{
		STRCPY(tmsg, arq->ItemString(1)); while(strlen(tmsg)<(unsigned) tXCol2) STRCAT(tmsg, " ");
		STRCAT(tmsg, arq->ItemString(2)); while(strlen(tmsg)<(unsigned) tXCol3) STRCAT(tmsg, " ");
		STRCAT(tmsg, "    "); STRCAT(tmsg, arq->ItemString(3));
		if(pdeta >= 1) DispDeta(tmsg, "");
		if(pdeta >= 1) DispDeta("----------------------------------------", "");
	}
	else
	{
#ifndef POSXP
	    wHand = wDefine(ty, tx, ty + tAlto, tx + tAncho, BORDERSINGLE, SNGCURS, arqueoEsq, 
		                "", "Pulse ESC para Salir");
		wActivate(wHand);
		wLocate(1, 1);      wPstr(arq->ItemString(1));
		wLocate(1, tXCol2); wPstr(arq->ItemString(2));
		wLocate(1, tXCol3); wPstr("    "); wPstr(arq->ItemString(3));
		wLocate(2, 1);      wPstr("------------------------------------------------------------");
#endif
	}

	//   Inicializa el contenido inicial de los numeros.
    for(int i=0; i<20; i++) 
	{
		char *tData = aData->ItemString(i);
		if(tData == NULL)
			STRCPY(num[i], "0");
		else
			STRCPY(num[i], tData);
	}

	//   Determina el ancho maximo de la descripcion, para alinearlo.
	int maxAncho = 0; 
	for(int i=4; i<arq->Count(); i++)
		if(strlen(arq->ItemString(i)) > (unsigned) maxAncho)
			maxAncho = strlen(arq->ItemString(i));
	int offDes = (tXCol2 - maxAncho - 2) / 2;
	int nItems = arq->Count() - 4;

	//   Ubica en la form cada uno de los renglones, con su titulo y get.
#ifndef POSXP
	EditWindow *qWin = new EditWindow(wHand);
#endif
	struct DDeclaDT_ decl;
    for(int i=4; i<arq->Count(); i++)
	{
		char *des = arq->ItemString(i);
		if(print)
		{
			double cant = atof(num[i-4]), uni = atof(des), tot = cant * uni;
			if(cant != 0)
			{
				//   Muestra algo de informacion en pantalla
				tmsg[0] = 0; while(strlen(tmsg) < (unsigned) offDes) STRCAT(tmsg, " ");
				STRCAT(tmsg, "$ "); STRCAT(tmsg, des);
				//WriteTickTmp("", tmsg, cant, uni, tot, 0);

				//   Prepara el renglon a imprimir y enviar al server.
				while(strlen(tmsg)<(unsigned) tXCol2+1) STRCAT(tmsg, " ");
				sprintf(tnum, "%.0lf", cant); STRCAT(tmsg, tnum);
				while(strlen(tmsg) < (unsigned) tXCol3) STRCAT(tmsg, " ");
				sprintf(tnum, "%9.2lf", tot);
				STRCAT(tmsg, tnum);
				if(pdeta >= 1) DispDeta(tmsg, "");

				sprintf(decl.item, "%s,%.0lf", des, cant);
				decl.func = DDeclaDT;
				decl.mpag = item;
		        decl.cajero = ckaNro;
				store(tot, decl.imp);
				WriteDump(&decl);
			}
		}
		else
		{
#ifndef POSXP
			wSetColor(arqueoEsq[0]);
			wLocate(i-1, offDes); wPstr("$ "); wPstr(des);
			qWin->AddGet(i-1, tXCol2+1, NUMBER_RIGHT_ALIGNED, 6, 0, num[i-4], NULL, NULL, ValidArqu);
			ValidArqu(num[i - 4], NULL);
#endif
		}
	}

	//   Anima la FORM y graba los resultados obtenidos y elimina la memoria utilizada
	if(!print)
	{
#ifdef POSXP
		ArqueoTipo1 ^aq1 = gcnew ArqueoTipo1(Strings::CharToString(mp->des), arq, aData);
		aq1->ShowDialog();
		if(aq1->DialogResult != System::Windows::Forms::DialogResult::OK)
			return;

		sprintf(num[0], "%d", aq1->GetResult(1));
		sprintf(num[1], "%d", aq1->GetResult(2));
		sprintf(num[2], "%d", aq1->GetResult(3));
		sprintf(num[3], "%d", aq1->GetResult(4));
		sprintf(num[4], "%d", aq1->GetResult(5));
		sprintf(num[5], "%d", aq1->GetResult(6));
		sprintf(num[6], "%d", aq1->GetResult(7));
		sprintf(num[7], "%d", aq1->GetResult(8));
		sprintf(num[8], "%d", aq1->GetResult(9));
		sprintf(num[9], "%d", aq1->GetResult(10));
		sprintf(num[10], "%d", aq1->GetResult(11));
		sprintf(num[11], "%d", aq1->GetResult(12));
#else
		qWin->AnimaGets(true);
		wRelease(wHand);
		delete qWin;
#endif
	}

	tCant = 0; tMonto = 0;
	aData->Clear();
    for(int i=0; i<arq->Count()-4; i++)
	{
		if(!print) aData->Add(STRDUP(num[i], "Arqueo1"));
		double c = atof(ClearFormat(num[i]));
		if(c != 0)
		{
			tCant++;
			tMonto += c * atof(ClearFormat(arqData->ItemString(i + 4)));
		}
	}
	taCant = tCant;
}

//   Funcion de validacion del browse utilizada en Arqueo2 para evaluar la expresion de
//   validacion que se le indico.
bool ValidData(int useRow, char *fldName, char *newData)
{
	char tDataBuff[20];
	if(!_stricmp(fldName, "N1"))
		tdb->Get("N2", tDataBuff);
	else
		tdb->Get("N1", tDataBuff);
	
	double v1 = atof(ClearFormat(newData)), v2 = atof(ClearFormat(tDataBuff));
	sprintf(tDataBuff, "%10.2lf", v1 * v2);
	tdb->Replace("N3", tDataBuff);
	dstFld->RefreshFromDb(useRow, 1);
	return true;
}

//   Arqueo modo 2 - Pantalla con browse de datos variables.
void Arqueo2(mpag_ *mp, int item, ::ArrayList *arq, ::ArrayList *aData, bool print)
{
	static char fldname[20], tDataBuff[50];

	//  Preserva datos para la funcion de validacion.
	arqData = arq;

	//  Construye el cursor que contendra los datos para la edicion.
	tdb = new(dbf);
	tdb->creaCursor("tarqueo", "c1 c(20), c2 c(20), c3 c(20), c4 c(20), c5 c(20),n1 n(10,2), n2 n(10,2), n3 n(10,2), n4 n(10,2), n5 n(10,2)");
	//for(int i=0; i<40; i++) 
	for(int i=0; i< cntItemArqueo; i++) 
	{
#ifdef POSXP
		bool agregado = false;
#else
		tdb->AppendBlank();
#endif

		char *s = aData->ItemString(i);
		if(s != NULL && strlen(s) != 0)
		{
			int fldNum = 1;
			bool inChars = true;
			char *dptr = s;
			while(true)
			{
				if(inChars)
				{
					fldname[0] = 'C'; _itoa(fldNum, &fldname[1], 10);
					char *dst = tDataBuff;
					for(dptr++; *dptr != '\"'; dptr++)
						*dst++ = *dptr;
					dptr++; dptr++; *dst = 0;
#ifdef POSXP
					if(!agregado) { agregado = true; tdb->AppendBlank(); }
#endif
					tdb->Replace(fldname, tDataBuff);
					if(fldNum++ == 5) { fldNum = 1; inChars = false; }
				}
				else
				{
					fldname[0] = 'N'; _itoa(fldNum, &fldname[1], 10);
					char *dst = tDataBuff;
					while(*dptr != ',')
						*dst++ = *dptr++;
					dptr++; *dst =  0;
#ifdef POSXP
					if(!agregado) { agregado = true; tdb->AppendBlank(); }
#endif
					tdb->Replace(fldname, tDataBuff);
					if(fldNum++ == 5) break;
				}
			}
		}
	}
	tdb->Go(1);

	//  Construye el browse que se utilizara en la edicion.
	::ArrayList *dataRng = new ::ArrayList(false, "Arqueo2-datarng");
	int totColumn = 0;

#ifdef POSXP
	totColumn = 3;
	if(!print) 
	{
		ArqueoTipo2 ^aq2 = gcnew ArqueoTipo2(Strings::CharToString(mp->des), tdb);
		aq2->ShowDialog();
	}
	else
	{
		if(pdeta > 1) DispDeta("    Cantidad    Denomin.       Total", "");
		if(pdeta > 1) DispDeta("----------------------------------------", "");
		dataRng->Add("N1");
		dataRng->Add("N2");
		dataRng->Add("N3");
	}
#else
	xBrowse *brw = new(xBrowse);
	xString hdrRng; hdrRng = ""; 

	//  Procesa la lista de campos definda.
	int maxFields = arq->Count();
	if(maxFields > 5) maxFields = 5;
	int tAncho = 0;
    for(int i=1; i<maxFields; i++)
	{
		//   Parsea la string de entrada
		char *fStr = _strupr(arq->ItemString(i)), format = 'C', *autoFormat;
		int posi = strcspn(fStr, ":"), len = strlen(fStr), fPosi;
		fPosi = posi;
		fStr[posi] = 0;
		bool autoCalc = false;
		if(posi >= len)
			format = 'C';
		else
		{
			posi++;
			while(posi < len && isspace(fStr[posi])) posi++;
			if(fStr[posi] == 'N')	     format = 'N';
			else if(fStr[posi] == '$') { format = 'N'; totColumn = i; }
			else if(fStr[posi] == '(' && !autoCalc)
			{
				format = 'N';
				totColumn = i;
				autoCalc = true;
				autoFormat = &fStr[posi + 1];
			}
			else format = 'C';
		}

		fldname[0] = 0;
		_itoa(i, &fldname[1], 10);
		fldname[0] = format;
		xBrowseField *f = brw->xBrowseF(tdb, fldname, fStr, format);
		if(print)
		{
			if(strlen(fStr) < 12)
			{
				char spcs[] = "                   ";
				spcs[12 - strlen(fStr)] = 0;
				hdrRng = hdrRng + spcs;
			}
			hdrRng = hdrRng + fStr;
			dataRng->Add(fldname, true);
		}

		if(autoCalc)
		{
			f->ReadOnly = true;
			dstFld = f;
			dstFormula = autoFormat;
		}
		else
			f->ValidFunc = ValidData;
		fStr[fPosi] = ':';
	}

	//   Efectua la entrada de datos.
	if(!print) 
		brw->xBrowseAnimate(0, tdb, false, true, 0, NULL);
	else
	{
		DispDeta(hdrRng.Data(), "");
		DispDeta("----------------------------------------", "");
	}
#endif

	//   Preserva los datos recien editados mientra calcula totales
	tCant = 0; tMonto = 0;
	aData->Clear();
	tdb->Go(DB_GO_TOP);
	xString xs; 
	while(!tdb->Eof())
	{
		xs = "";
		bool empty = true;
		for(int i=1; i<=5; i++)
		{
			fldname[0] = 'C'; _itoa(i, &fldname[1], 10);
			tdb->Get(fldname, tDataBuff); Trim(tDataBuff);
			xs = xs + "\"";
			xs = xs + tDataBuff;
			xs = xs + "\",";
			if(strlen(tDataBuff) != 0) empty = false;
		}
        
        for(int i=1; i<=5; i++)
		{
			fldname[0] = 'N'; _itoa(i, &fldname[1], 10);
			tdb->Get(fldname, tDataBuff); Trim(tDataBuff);
			xs = xs + ClearFormat(tDataBuff);
			xs = xs + ",";
			if(strlen(tDataBuff) != 0) empty = false;
			if(i == totColumn)
				tMonto += atof(tDataBuff);
			else if(i == 1)
				tCant += atoi(tDataBuff);
		}
		if(!empty) aData->Add(STRDUP(xs.Data(), "Arqueo2"));
		if(print)
		{
			char spc[20], rng[100];
			double cant=0, uni=0;
			rng[0] = 0;
			for(int i=0; i<dataRng->Count(); i++)
			{
				char *fld = dataRng->ItemString(i);
				tdb->Get(fld, tDataBuff); Trim(tDataBuff);
				STRCPY(spc, "                ");
				if(strlen(tDataBuff) < 13)
					spc[12 - strlen(tDataBuff)] = 0;
				else
					spc[0] = 0;

				if(fld[0] == 'C')
				{
					STRCAT(rng, tDataBuff);
					STRCAT(rng, spc);
				}
				else
				{
					if(i == 0) cant = atof(tDataBuff); else if(i == 1) uni = atof(tDataBuff);
					STRCAT(rng, spc);
					STRCAT(rng, tDataBuff);
				}
			}

			struct DDeclaDT_ decl;
			if(cant != 0.0)
			{
				if(pdeta > 1) DispDeta(rng, "");
				sprintf(rng, "Cupones de %.2lf", uni);
				// WriteTickTmp("", rng, cant, uni, cant*uni, false);

				sprintf(decl.item, "%.2lf,%.0lf", uni, (double)cant);
				decl.func = DDeclaDT;
				decl.mpag = item;
		        decl.cajero = ckaNro;
				store(cant*uni, decl.imp);
				WriteDump(&decl);

			}
		}
		tdb->Skip(1);
	}
	tdb->Close();
	delete tdb;
	delete dataRng;
	taCant = tCant;
#ifndef POSXP
	delete brw;
#endif
}

//   Refresca las cantidades y montos
void RefreshCantAndMonto()
{
#ifndef POSXP
	int x = mArqueo->MaxAncho() + 3;
	wActivate(mArqueo->WindowHandle());
	for(int i = 0; i < outOpc - 1; i++)
	{
		sprintf(tmonto, "$ %.2f", monto[i]);
		sprintf(tmsg, frmtLine, cuenta[i], tmonto);
		wLocate(i+1, x);
		wPstr(tmsg);
	}
#endif
}

//   Funcion que es llamada cada vez que se edita un item de arqueo.
int ItemArqueo(int item)
{
	if(item == -1)			// Evento Refresh - se creo la window.
		RefreshCantAndMonto();
	else if(item == -2)		// Evento Refresh - Siempre
	{
	}
	else if(item == outOpc || item == 0)
		return 1;
	else if(item > 0 && item != (tickVltoIndex + 2))
	{
		mpag_ *mp;
		if(resuArqueo)
			mp = (mpag_ *)mpconso->Item(item - 1);
		else
			mp = (mpag_ *)sortedMedios->Item(item - 1);

		if(mp->arqueo == NULL)
			mp->arqueo = new ::ArrayList(true, "ItemArqueo");
		::ArrayList *arq =mp->arqueo;

		if(mp->aData == NULL)
			mp->aData = new ::ArrayList(true, "ItemArqueo2");
		::ArrayList *arqData = mp->aData;
		if(arq == NULL) return 1;
		
		if(!strcmp(arq->ItemString(0), "1"))
			Arqueo1(mp, item, arq, arqData, false);
		else if(!strcmp(arq->ItemString(0), "2"))
			Arqueo2(mp, item, arq, arqData, false);
		monto[item-1] = tMonto;
		cuenta[item-1] = tCant;

#ifdef POSXP
		if(tMonto == 0)
			sprintf(msg, "%s", mp->des);
		else
			sprintf(msg, "%s (%.2lf)", mp->des, tMonto);
		mArqueo->ChangeTit(item, msg);
#endif

		RefreshCantAndMonto();
	}
	return 0;
}

//   Form por la cual se edita el arqueo del cajero.
int FullArqueo()
{
	char tDes[100];
	if(!cko_enab)
	{
		Alert("La funcion de cajeros NO ESTA habilitada", "en el archivo de opciones del sistema.");
		return 0;
	}
	if(ckaNro == 0)
	{
		//Comentado esta seccion para que no de el mensaje en el cierre de la caja
		//Alert("No hay ningun cajero activo al cual cerrar.", "En modo REG, ingrese el numero de cajero a cerrar.");
		return 0;
	}

	if(mArqueo == NULL)
		mArqueo = new Choose("Cierre de Cajeros", "ESC=Cancela", 20, 0, CHOOSE_ALIGN_LEFT, ItemArqueo);
	else
		mArqueo->Clear();
	
	int cant = resuArqueo? mpconso->Count(): cMpag;
	cuenta = new int[cant]; // (int *) MALLOC(sizeof(int) * (cant + 10), "FullArqueo1");
	monto = new double[cant]; // (double *) MALLOC(sizeof(double) * (cant + 10), "FullArqueo2");
	sortedMedios = OrdenarMedios();
	for(int i = 0; i < cant; i++)
	{
		mpag_ *mp;
		if(resuArqueo)
			mp = (mpag_ *)mpconso->Item(i);
		else
			mp = (mpag_ *)sortedMedios->Item(i);
		STRCPY(tDes, mp->des);

		if(mp->tipo != 'Z') tickVltoIndex = i;
		mArqueo->Add(Trim(tDes));
		cuenta[i] = 0;
		monto[i] = 0;
	}

	mArqueo->Add("- Grabar y Salir -");
	outOpc = cant + 1;

	int xRes = mArqueo->Anima();
	delete sortedMedios;
	FREE(cuenta); FREE(monto);
	return xRes;
}

//   Impresion de los datos del arqueo.
double PrintAndDump(struct mpag_ *mp)
{
	tMonto = 0.0;
	char title[100]; 
	
	sprintf(title, "&4%s", mp->des);
	if(mp->aData == NULL) return tMonto;
	int item = (mp - mpagos) + 1;
	//int item = mp->nro;
	if(pdeta) DispDetaArqueo(title, "");
	if(!strcmp(mp->arqueo->ItemString(0), "1"))
	{
		Arqueo1(mp, item, mp->arqueo, mp->aData, true);
	}
	else if(!strcmp(mp->arqueo->ItemString(0), "2"))
		Arqueo2(mp, item, mp->arqueo, mp->aData, true);

	delete mp->aData;
	mp->aData = NULL;
	return tMonto;
}
