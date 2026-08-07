/*
 *   Opc.c
 *   Opciones configurables.
*/

#include <stdafx.h>

// Secciones disponibles en 'opciones.ini'
char SECURITY[] = "seguridad";
char GRAL[]     = "gral";
char KEYS[]     = "teclas";
char LAYOUT[]   = "layout";
char COMPAT[]   = "hardware";
char MILLAS[]   = "millas";
char CLK[]      = "cajeros";
char INFOR[]    = "informes";
char VEN[]      = "vended";
char PATHS[]    = "paths";
char SUSP[]     = "suspender";
char PERF[]		= "perfomance";
char PING[]     = "ping";
char DEBUG[]    = "debug";
char CTACTE[]   = "ctacte";
char SCANNER[]	= "scanner";
char OPTMAY[]	= "opciones";

// Archivos de configuracion
char *F_Opciones = "opciones", *F_Scanner = "scanner", *F_Mensajes = "mensajes", 
     *F_Cabecera = "cabecera", *F_Logos = "logos", *F_Net = "net",
	 *F_Online = "online", *F_Promo = "promo";

char *F_Cajeros = "cajeros", *F_Canastas = "canastas", *F_Claves = "claves", 
     *F_Rangos = "rangos", *F_TjOnline = "tjonline", *F_CuoTar = "cuotar",
	 *F_Mpagos = "mpagos", *F_Mayor = "mayor";

void GeneraFiltroDepto(char *cadena, int *arrfiltro); //opc.cpp

//   Hay opciones que son PATHS. Los chequea y valida.
void ValidatePath(char *p)
{
	while(strlen(p) > 0 && isspace(*(p + strlen(p) - 1)))
		*(p + strlen(p) - 1) = 0;
	while(strlen(p) > 0 && *(p + strlen(p) - 1) == '\\')
		*(p + strlen(p) - 1) = 0;
	while(isspace(*p))
		STRCPY(p, p + 1);
}

void LoadLogos()
{
	char opcname[30];
	if(logos == NULL) logos = new ::ArrayList(true, "LoadLogos");
	if(!OpenIni(F_Logos))
	{
		logoDelay = ExtractIniLong("logos", "delay");
		logos->Clear();
		for(int i=1; i<1000; i++)
		{
			sprintf(opcname, "main%d", i);
			char *s = ExtractIniStr("logos", opcname);
			if((s == NULL || (s[0] == 0)) && i == 1)
				s = ExtractIniStr("logos", "main");
			if(s == NULL || (s[0] == 0)) break;
			logos->Add(s);
		}

		reloadLogos = false;
		CloseIni();
	}
}

// Mira si se indico la opcion de remapeo del archivo de opcion indicado. Si se indico,
// reemplaza entonces el nombre del archivo a usar.
static void ReplaceOptionalPath(char **path, char *opt, bool forcePosFolder)
{
	char *patch	= Trim(ExtractIniStr (GRAL, opt));
	if(strlen(patch) != 0) *path = patch;
	if(forcePosFolder && (strchr(*path, '\\') == NULL))
	{
		char *tmp = (char *)MALLOC(strlen(*path) + 5, "ReplaceOptionalPath");
		STRCAT(STRCPY(tmp, "pos\\"), *path);
		*path = tmp;
	}
}

//   Carga las opciones.
int LoadOpc()
{
	//Inicializo arrays de deptos pesables, excluidos e incluidos
	for (int k = 0; k < 50; k++)
	{
		nofichadeptos[k] = -1;
		deptopesables[k] = -1;
		filtradepto[k] = -1;
	}

	if(!OpenIni("datgral"))
	{
		caja			= (int)      ExtractIniLong(GRAL, "caja" );
		suc				= (int)      ExtractIniLong(GRAL, "sucursal");

		ReplaceOptionalPath(&F_Opciones, "opciones", false);
		ReplaceOptionalPath(&F_Scanner,  "scanner", false);
		ReplaceOptionalPath(&F_Mensajes, "mensajes", false);
		ReplaceOptionalPath(&F_Cabecera, "cabecera", false);
		ReplaceOptionalPath(&F_Logos,    "logos", false);
		ReplaceOptionalPath(&F_Net,      "net", false);
		ReplaceOptionalPath(&F_Online,   "online", false);
		ReplaceOptionalPath(&F_Promo,    "promo", false);
		
		ReplaceOptionalPath(&F_Cajeros,  "cajeros", true);
		ReplaceOptionalPath(&F_Canastas, "canastas", true);
		ReplaceOptionalPath(&F_Claves,   "claves", true);
		ReplaceOptionalPath(&F_Rangos,   "rangos", true);
		ReplaceOptionalPath(&F_TjOnline, "tjonline", true);
		ReplaceOptionalPath(&F_CuoTar,   "cuotar", true);
		ReplaceOptionalPath(&F_Mpagos,   "mpagos", true);

		CloseIni();
	}
	else
		AbnormalEnd("No encuentro DATGRAL.INI");
	
	reloadLogos = true;
	if(!OpenIni(F_Net))
	{
		CopyPacketSize = (int)       ExtractIniLong(PERF, "copypacketsize");
		CopyInterlace  = (int)       ExtractIniLong(PERF, "copyinterlace");

		if(CopyPacketSize <= 0) CopyPacketSize = 1000;
		if(CopyInterlace <= 0) CopyInterlace = 1;

		STRCPY(host,                 ExtractIniStr(PING, "Host"));
		pingDelay = (int)            ExtractIniLong(PING, "Delay");

		Trim(host);
		if(pingDelay < 5) pingDelay = 5;
		if(strlen(host) == 0) pingDelay = 0;

		CloseIni();
	}
	else
		AbnormalEnd("No encuentro NET.INI o el equivalente");
	
	if(!OpenIni(F_Scanner))
	{
		sc_DvEnBase    =             ExtractIniSiNo(SCANNER, "dvenbase");
		sc_DvEnScanner =             ExtractIniSiNo(SCANNER, "dvenscanner");
		sc_AppendDV    =             ExtractIniSiNo(SCANNER, "appenddv");
		sc_MskPesable  =             ExtractIniStr (SCANNER, "pesablemsk");
		sc_MskCajero   =             ExtractIniStr (SCANNER, "cajeromsk");
		sc_MskMozo     =             ExtractIniStr (SCANNER, "mozomsk");
		sc_MskSuperv   =             ExtractIniStr (SCANNER, "supervmsk");
		sc_MskLlaveX1  =             ExtractIniStr (SCANNER, "llavex1");
		sc_MskLlaveX2  =             ExtractIniStr (SCANNER, "llavex2");
		sc_MskLlaveZ   =             ExtractIniStr (SCANNER, "llavez");
		sc_MskCliente  =             ExtractIniStr (SCANNER, "cltmsk");
		scanDebug	   =             ExtractIniSiNo(SCANNER, "debug");
		CloseIni();
	}
	else
		AbnormalEnd("No encuentro SCANNER.INI o el equivalente");
	
	if(!OpenIni("ctasctes"))
	{
		cc_UsaListas = (int)         ExtractIniLong(CTACTE, "UsaListas");
		cc_HastaSaldo =              ExtractIniSiNo(CTACTE, "HastaSaldo");
		cc_SoloInicio =              ExtractIniSiNo(CTACTE, "SoloInicio");
		cc_ForzarMP =                ExtractIniSiNo(CTACTE, "ForzarMP");
		cc_Medio = (int)             ExtractIniLong(CTACTE, "CtaMedio");
		cc_PorceMax =                ExtractIniSiNo(CTACTE, "UsaPorceMax");
		CloseIni();
	}
	else
		AbnormalEnd("No encuentro CTASCTES.INI");	
	#ifdef POSDEBUG
	DBGRxPlus = DBGCopyFile = DBGActiPlus = 0;
	if(!OpenIni("debug"))
	{
		DBGRxPlus =   (int)          ExtractIniLong(DEBUG, "RecepcionPrecios");	
		DBGActiPlus = (int)          ExtractIniLong(DEBUG, "ActivacionPrecios");
		DBGCopyFile = (int)          ExtractIniLong(DEBUG, "CopiarArchivos");
		DBGPrintProc = (int)         ExtractIniLong(DEBUG, "PrinterProc");

#ifdef POSXP
		int kblog =					 ExtractIniSiNo(DEBUG, "KbLog");
		Pos::Desktop::KbdLogger::Enabled = (kblog != 0);
#endif
		CloseIni();
	}
	#endif

	if(!OpenIni(F_Opciones))
	 {
		p_Log			=       Trim(ExtractIniStr (PATHS, "log"));
		wlog			= (int)      ExtractIniLong(PATHS, "wlog");
		forceClrLog		=            ExtractIniSiNo(PATHS, "force_clear");

		dvariac			=			ExtractIniStr(PATHS, "dvariac");
		dirpuntual		=			ExtractIniStr(PATHS, "dirpuntual");
		dirpromo		=			ExtractIniStr(PATHS, "dirpromo");
		fpromo			=			ExtractIniStr(PATHS, "fpromo");
		fcanastas		=			ExtractIniStr(PATHS, "fcanastas");

		prndelay		=            ExtractIniSiNo(SUSP,  "prndelay");
		mayPrecPesable	=            ExtractIniSiNo(SUSP,  "mayprecpesable", 0);
		topeLocal		=      atof_(ExtractIniChar(SUSP,  "topelocal"));
		autoapp			=            ExtractIniSiNo(SUSP,  "autoapp");
		recuany			=            ExtractIniSiNo(SUSP,  "recuany");
		autonro			=            ExtractIniSiNo(SUSP,  "autonro");
		rango			=            ExtractIniStr (SUSP,  "rango");
		usegraf			=            ExtractIniSiNo(SUSP,  "usegraf");
		ponline         =            ExtractIniSiNo(SUSP,  "ponline", 0);
		modoMayor		= (int)		 ExtractIniLong(SUSP,  "modo-mayor", 0);
		precioBulto		= (int)		 ExtractIniLong(SUSP,  "precio-bulto", 2);
		precioX3Un		= (int)		 ExtractIniLong(SUSP,  "precio-por3", 4);
		precioUnit		= (int)		 ExtractIniLong(SUSP,  "precio-unit", 1);
		strconbd		=			 ExtractIniStr(SUSP,	"strconbd");
#ifdef FACTSQL
		TransSql::DatosSql::SetCadenaConexion(gcnew String(strconbd));
#endif

		z_depto			=            ExtractIniSiNo(INFOR, "zeta-depto");
		z_cajero		=            ExtractIniSiNo(INFOR, "zeta-cajero");
		z_mpago			=            ExtractIniSiNo(INFOR, "zeta-mpago");
		z_horar			=            ExtractIniSiNo(INFOR, "zeta-horar");
		z_clrlog		=            ExtractIniSiNo(INFOR, "borra-log");
		z_vended		=            ExtractIniSiNo(INFOR, "zeta-vend");
		z_plus			=            ExtractIniSiNo(INFOR, "zeta-plus");
		z_muestra       =            ExtractIniSiNo(INFOR, "muestra-arqueo");

		limTicket		= (double)   ExtractIniLong(GRAL, "lim-ticket", 10000);
		limRenglon		= (double)   ExtractIniLong(GRAL, "lim-renglon", 10000);
		limCantidad		= (double)   ExtractIniLong(GRAL, "lim-cantidad", 10000);
//		limCantxTicket	= (double)   ExtractIniLong(GRAL, "lim-cantxticket", 99999999);
		minlimextrac	= (double)	 ExtractIniLong(GRAL, "lim-minextrac", 50);
		maxlimextrac	= (double)	 ExtractIniLong(GRAL, "lim-maxextrac", 200);
		limConsFinal	= (double)	 ExtractIniLong(GRAL, "lim-consfinal", 1000);
		AplicarPercep	=				 ExtractIniSiNo(GRAL, "aplicar-percepcion", 1);		
		AplicarTissh	=				 ExtractIniSiNo(GRAL, "aplicar-tissh", 0);		
		AplicarPerIva	=				 ExtractIniSiNo(GRAL, "aplicar-periva", 0);		
		EsEcommerce	=				 ExtractIniSiNo(GRAL, "ecommerce", 0);
		janisEcom	=				 ExtractIniSiNo(GRAL, "janisEcom", 0);
		procesaParking =			 ExtractIniSiNo(GRAL, "procesaParking", 0);
		IngNroVdor	=				 ExtractIniSiNo(GRAL, "ing-vendedor", 0);
		CargaSenia	=				 ExtractIniSiNo(GRAL, "carga-senia", 0);
		if (IngNroVdor)
			EsEcommerce = true;
		NroVoucherEcommerce		= (int)      ExtractIniLong(GRAL, "nrovou-ecomm", 6);

		MinimoTissh		= (double)	 ExtractIniLong(GRAL, "minimo-tissh", 48750);
		MinimoTissh /=100;
		FactBconCuit	=				 ExtractIniSiNo(GRAL, "FactB-conCuit", 0);
		opcdec			= (int)      ExtractIniLong(GRAL, "decimales");
		RngLimit		= (int)      ExtractIniLong(GRAL, "rengFact");
		char *t			=       Trim(ExtractIniStr( GRAL, "plumult"));
		checkLimFisc	=			ExtractIniSiNo(GRAL, "controla-limfiscal", 1);

		mpNumVoucherDev			= (int)      ExtractIniLong(GRAL, "voucher-devmerpago", 8);
		mpRepCons			= (int)      ExtractIniLong(GRAL, "repite-consmerpago", 60);
		
		pluMult			= (_stricmp(t, "monto") != 0);	// 0=Monto, 1=Cantidad
		if(RngLimit == 0) RngLimit = 55;

		chPermi[1]		=            ExtractIniLong(SECURITY, "x1");
		chPermi[2]		=            ExtractIniLong(SECURITY, "x2");
		chPermi[3]		=            ExtractIniLong(SECURITY, "z");
        chPermi[4]		=            ExtractIniLong(SECURITY, "prog");

		keyupper		= (int)      ExtractIniLong(KEYS, "sube");
		keydown			= (int)      ExtractIniLong(KEYS, "baja");
		keyenter		= (int)      ExtractIniLong(KEYS, "acepta");
		keycanc			= (int)      ExtractIniLong(KEYS, "cancela");
		keyback			= (int)      ExtractIniLong(KEYS, "borra");
		keyleft			= (int)      ExtractIniLong(KEYS, "izquierda");
		keyright		= (int)      ExtractIniLong(KEYS, "derecha");
	    keyout			= (int)      ExtractIniLong(KEYS, "sale");
		
		keyreg			= (int)      ExtractIniLong(KEYS, "modo-reg");
		keyx1			= (int)      ExtractIniLong(KEYS, "modo-x1");
		keyx2			= (int)      ExtractIniLong(KEYS, "modo-x2");
		keyz			= (int)      ExtractIniLong(KEYS, "modo-z");
		modescan		= (int)      ExtractIniLong(KEYS, "previo");

		emax			= (int)      ExtractIniLong(LAYOUT, "custdisp");
		printsubt		=            ExtractIniSiNo(LAYOUT, "p-subtotal");
		forcesubt		=            ExtractIniSiNo(LAYOUT, "fuerza-subtot");
		printplucod		=            ExtractIniSiNo(LAYOUT, "p-plucod");
        cdigEmax		= (int)      ExtractIniLong(LAYOUT, "digitos-emax");
        ssav_delay		= (int)      ExtractIniLong(LAYOUT, "demora-ssaver");
		permitePesado	=            ExtractIniSiNo(LAYOUT, "permite-plu-pesado");
		tjOnline		=            ExtractIniSiNo(LAYOUT, "tjonline");
		tjPrisma		=            ExtractIniSiNo(LAYOUT, "tjprisma");
		tjSmartPoint	=            ExtractIniSiNo(LAYOUT, "tjsmartpoint");
		EcrPrisma		=            ExtractIniSiNo(LAYOUT, "ecrprisma");
		mpCashout		=            ExtractIniSiNo(LAYOUT, "mpcashout");
		corriMode		= (int)      ExtractIniLong(LAYOUT, "corri-mode");
		logoFile		=       Trim(ExtractIniStr (LAYOUT, "logo"));
		clavePrecios	= (int)      ExtractIniLong(LAYOUT, "clave-precios");
		nocieTke		=            ExtractIniSiNo(LAYOUT, "nocierra_tke");
		autocierre		= (int)      ExtractIniLong(LAYOUT, "autocierre");
		tipoArqueo		= (int)      ExtractIniLong(LAYOUT, "tipo-arqueo");
		resuArqueo		=            ExtractIniSiNo(LAYOUT, "resu-arqueo");
		pdetaArqueo		= (int)      ExtractIniLong(LAYOUT, "pdeta-arqueo");
		pdetaFinArqueo	= (int)      ExtractIniLong(LAYOUT, "pdetafin-arqueo");
		talon			= (int)      ExtractIniLong(LAYOUT, "talon");
		retiCod			= (int)      ExtractIniLong(LAYOUT, "reticod-arqueo");
		cajFlota        =            ExtractIniSiNo(LAYOUT, "caj-flotantes", 1);
		autoAnul        =            ExtractIniSiNo(LAYOUT, "auto-anul", 0);
		permiteSubir    =            ExtractIniSiNo(LAYOUT, "pesado-mayor-solo-baja", 0);
	    printCant       = (int)      ExtractIniSiNo(LAYOUT, "print-cant", 0);
	    printReduce     = (int)      ExtractIniSiNo(LAYOUT, "print-reduce", 1);
		balanPrec       = (int)      ExtractIniLong(LAYOUT, "balan-prec", 1);
		enterEnabled    =            ExtractIniSiNo(LAYOUT, "buscar-usa-enter", 1);
		permite_dun14   = (int)      ExtractIniLong(LAYOUT, "permite-dun14", 1);
		cntItemArqueo	= (int)		 ExtractIniLong(LAYOUT, "cant-item-arqueo", 100);
		puede_pasar		= (int)		 ExtractIniLong(LAYOUT, "puede-pasar", 0);
		scanartcobradora = (int)	 ExtractIniLong(LAYOUT, "articulos-por-cobradora", 1);

        if(ssav_delay == 0)    ssav_delay = 120;
        if(cdigEmax == 0)      cdigEmax = 20;
		if(emax < 1 || emax > 3) emax=1;

        debugSerial		=            ExtractIniSiNo(COMPAT, "debugserial");
        videoType		= (int)      ExtractIniLong(COMPAT, "video-mem");
        KEYBOARD		=       Trim(ExtractIniStr (COMPAT, "teclado"));
		ENDKEY			= (unsigned) ExtractIniLong(COMPAT, "finaliza");
		COM_PRN			= (unsigned) ExtractIniLong(COMPAT, "impresor");
		COM_EMAX		= (unsigned) ExtractIniLong(COMPAT, "visor-emax");
		COM_SCAN		= (unsigned) ExtractIniLong(COMPAT, "scanner");
		COM_BALAN		= (unsigned) ExtractIniLong(COMPAT, "balanza");
		chkXon			=		     ExtractIniSiNo(COMPAT, "check-prn");
		interfaz		=    _strupr(ExtractIniStr (COMPAT, "prnprot"));
        defFactuMode	=            ExtractIniSiNo(COMPAT, "comprob");
		defListaPrec	= (int)      ExtractIniLong(COMPAT, "lista");
        DelayEmax		=            ExtractIniLong(COMPAT, "del-emax");
		UsaLlave		=            ExtractIniSiNo(COMPAT, "usa-llave");
		SyncPrinterTot  =            ExtractIniSiNo(COMPAT, "sincroniza");
		GetPlusUsingDos =            ExtractIniSiNo(COMPAT, "pluswdos");
        prnmudo         =            ExtractIniSiNo(COMPAT, "prnmudo");
		TieneBalanza    = (int)      ExtractIniLong(COMPAT, "tienebalanza");
		//hace esto para validar el modelo actual que dice tienebalanza = "si"
		if (TieneBalanza == 0)
			TieneBalanza =  ExtractIniSiNo(COMPAT, "tienebalanza");
		timeoutSelf		= (int)		 ExtractIniLong(COMPAT, "timeoutSelf", 30);

		tieneBalanzaControl    =            ExtractIniSiNo(COMPAT, "tienebalcontrol");
		COM_BCL			=(unsigned) ExtractIniLong(COMPAT, "balcontrol");

		COM_NLD			=(unsigned) ExtractIniLong(COMPAT, "comnewland");

		TieneFU			=            ExtractIniSiNo(COMPAT, "tiene-fu", 0);
		ComPortCL		= (int)      ExtractIniLong(COMPAT, "com-fu");
		SegsLuzCL		= (int)      ExtractIniLong(COMPAT, "segs-fu");

		beepFile        =       Trim(ExtractIniStr (COMPAT, "beep"));		
		if(defListaPrec < 1||defListaPrec>5) defListaPrec = 1;
		if(beepFile==NULL || strlen(beepFile)==0)
			beep = 1;
		else
		{
			beep = atoi(beepFile);
			if(beep == 0 && !Existe(beepFile))
				beep = 0;
			else
				beep = 2;
		}

		M_VerRng		= (int)      ExtractIniLong(MILLAS, "VerReng");
		M_VerPie		=            ExtractIniSiNo(MILLAS, "VerPie");
		M_VerPie0		=            ExtractIniSiNo(MILLAS, "VerPie0");
		M_PrintAtPie	=            ExtractIniSiNo(MILLAS, "UsaPieTick");
		M_NroVoucher	= (int)      ExtractIniLong(MILLAS, "UsaVoucher");
		M_Modo			= (int)      ExtractIniLong(MILLAS, "Modo");
		M_ParamModo		= (int)      ExtractIniLong(MILLAS, "ParamModo");

		cko_rebo		=            ExtractIniSiNo(CLK, "Rebotable");
		cko_opnd		=            ExtractIniSiNo(CLK, "AbreGav");
		cko_enab		=            ExtractIniSiNo(CLK, "UsaCajero");
		clerk_ant		=			 ExtractIniLong(CLK, "controla-ant", 1);
		if (pantalla == 7)
			cajeroSelf	=			 ExtractIniLong(CLK, "cajeSelf", 100);

		ven_enab		=            ExtractIniSiNo(VEN, "UsaVended");
		ven_enmed		=            ExtractIniSiNo(VEN, "EnMedio");
		ven_rebot		=            ExtractIniSiNo(VEN, "Rebotable");
		ven_obli		=            ExtractIniSiNo(VEN, "Obligat");
		ven_des			=            ExtractIniStr (VEN, "Venddes");

		ccte_Path		=            ExtractIniStr (CTACTE, "ArchComp");
		ccte_Edit		=            ExtractIniSiNo(CTACTE, "EditCta");
		ccte_Onln		=            ExtractIniSiNo(CTACTE, "OnLine");

		ValidatePath(ccte_Path);
		ValidatePath(p_Log);
		CloseIni();
	}
	else
		AbnormalEnd("No encuentro 'OPCIONES.INI' o su equivalente");
	
	// Variables opcionales que pisan opciones jodidas del 'opciones.ini'
	if(!OpenIni("datgral"))
	{
		int tLista		= (int)      ExtractIniLong(GRAL, "lista");
		if(tLista != 0)	defListaPrec = tLista;

		int tTalon		= (int)      ExtractIniLong(GRAL, "talon");
		if(tTalon != 0) talon = tTalon;

		double loctope	=      atof_(ExtractIniChar(GRAL, "topelocal"));
		if(loctope > 0.01) topeLocal = loctope;
        
		char *patchKeyb	=       Trim(ExtractIniStr (GRAL, "teclado"));
		if(strlen(patchKeyb) != 0) KEYBOARD = patchKeyb;

		char *patchPrn  =    _strupr(ExtractIniStr (GRAL, "prnprot"));
		if(strlen(patchPrn) != 0) interfaz = patchPrn;
		char *extractdeptos =		ExtractIniStr(GRAL, "vende-deptos");
		if (strlen(extractdeptos) != 0)
			GeneraFiltroDepto(extractdeptos, filtradepto);				

		char *deptosbalan =		ExtractIniStr(GRAL, "deptos-balanza");
		if (strlen(deptosbalan) != 0)
			GeneraFiltroDepto(deptosbalan, deptopesables);			

		char *noextractdeptos =		ExtractIniStr(GRAL, "no-vende-deptos");
		if (strlen(noextractdeptos) != 0)
			GeneraFiltroDepto(noextractdeptos, nofichadeptos);		

		char *noextractarts =		ExtractIniStr(GRAL, "no-vende-arts");
		if (strlen(noextractarts) != 0)
			GeneraFiltroDepto(noextractarts, nofichaarts);		

		char *auxcnlvta = _strupr(ExtractIniStr(GRAL, "canalventa"));
		if (strlen(auxcnlvta) > 0) 
			STRCPY(sapcnlvta, auxcnlvta);
		else 
			STRCPY(sapcnlvta, "MI");
		char *auxtienda = _strupr(ExtractIniStr(GRAL, "tienda"));
		if (strlen(auxtienda) > 0) 
			STRCPY(saptienda, auxtienda);
		else 
			STRCPY(saptienda, "CO001");
		//AutorPinPad =	ExtractIniSiNo(GRAL, "Autoriza-PinPad", 0);
		habilitaCargaVoucher = ExtractIniSiNo(GRAL, "Habil-CargaVoucher", 0);
		VtaxCodInterno    =   ExtractIniSiNo(GRAL, "venta-codinterno", 1);

		NumeroMpNc	= (int)		 ExtractIniLong(GRAL, "mp-notacredito", 33);
		CantDigDecBal	= (int)		 ExtractIniLong(GRAL, "CantDigDecBal", 2);		
		char *auxipServOC = ExtractIniStr(GRAL, "ipservidor-oc");
		if (strlen(auxipServOC) > 0)
			STRCPY(ipServOC, auxipServOC);
		portOC = (int) ExtractIniLong(GRAL, "puertoservidor-oc");		

		auxipServOC = ExtractIniStr(GRAL, "ipservidor-fu");
		if (strlen(auxipServOC) > 0)
			STRCPY(ipServCL, auxipServOC);
		portCL = (int) ExtractIniLong(GRAL, "puertoservidor-fu");	

	    CloseIni();
	}

	Int32::TryParse(System::Configuration::ConfigurationManager::AppSettings["smcodmaterial"], smcodmaterial);
	
	confListaPrec = defListaPrec;	

#ifndef POSXP
	topeLocal = 99999999.99;
#endif

	flagTope = (topeLocal > 0.01);
	return 1;
}

void GeneraFiltroDepto(char *cadena, int *arrfiltro){
	char mdep[7];
	int i = 0;

	for (int i = 0; i < 50; i++) 
		deptopesables[i] = -1;

	memset(mdep, 0, sizeof(mdep));

	while (*cadena) {
		if (*cadena == ',') {
			if (i > 0) {
				mdep[i] = '\0';
				*arrfiltro++ = atoi(mdep);
				memset(mdep, 0, sizeof(mdep));
				i = 0;
			}
		} else if (*cadena >= '0' && *cadena <= '9') {
			if (i < 6) {          // guarda contra overflow
				mdep[i++] = *cadena;
			}
		}
		cadena++;
	}
	// último token sin coma final
	if (i > 0) {
		mdep[i] = '\0';
		*arrfiltro++ = atoi(mdep);
	}
	*arrfiltro = -1; // centinela al final
}
