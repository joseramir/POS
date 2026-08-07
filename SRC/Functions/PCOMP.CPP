//
//   Pcomp.cpp
//   Imprime comprobantes de formato especial.
//

#include "stdafx.h"

//   Punteros a la listas de clases.
class defc     *dc_first;
class pHand    *ph_first;
struct const_  *co_first;

int    gCopia, gRenglon, gConst, gVari;
double g_Cantidad, g_UnitarioN, g_UnitarioT, g_RtotalN, g_RtotalT;
double g_TransporteN, g_TransporteT, g_TotalN, g_TotalT;
double g_Total, g_RecargosP, g_RecargosM, g_DescuentosP, g_DescuentosM;

char g_Codigo[15];
char g_Descrip[50];
char umsg[50];
char  *g_DIva[5] = {"Exento", "Iva 21%" };;
char *_dcopia[] = {"Original", "Duplicado", "Triplicado", "Cuadruplicado"};
char blank12[] = "            ";
double g_Iva[5];

/////////////////////////////////////////////////////////////////////////
///////////////////////// IMPLEMENTA CLASE pHand ////////////////////////

//  Envia una string de control.
void _SendCtrl(int h, char *s)
{
	int len;

	if(!s || !h) return;
	len = *((short *) s);
	if(len)
		write(h, s + 2, len);
}

//  Implementa constructor y destructor de la clase impresor.
pHand::pHand()
{
	memset(this, 0, sizeof(*this));
}
pHand::~pHand()
{
	if(h)       close(h);
        if(bold)    FREE(bold);
        if(_bold)   FREE(_bold);
        if(under)   FREE(under);
        if(_under)  FREE(_under);
        if(width)   FREE(width);
        if(_width)  FREE(_width);
        if(init)    FREE(init);
        if(release) FREE(release);
        if(port)    FREE(port);
        if(name)    FREE(name);

	// El campo next no lo toca. El proceso de destruccion va a ser
	// en masa y se puede pudrir todo si jodemos la cadena.
}

//  Imprime una string. 
void pHand::Print(char *s)
{
	if(!h) return;
	write(h, s, strlen(s));
	if(flags.nextprint)
	{
		flags.nextprint = 0;
		if(nextpCtrl & _BOLD_)   _SendCtrl(h, _bold);
		if(nextpCtrl & _UNDER_)  _SendCtrl(h, _under);
		if(nextpCtrl & _WIDTH_)  _SendCtrl(h, _width);
		if(nextpCtrl & _COND_)   _SendCtrl(h, _cond);
		nextpCtrl = 0;
	}
}

//  Imprime un comando de cambio de modo.
void pHand::SendCtrl(int Ctrl)
{
	if(Ctrl & _BOLD_)      _SendCtrl(h,  bold);
	if(Ctrl & _U_BOLD_)    _SendCtrl(h, _bold);
	if(Ctrl & _UNDER_)     _SendCtrl(h,  under);
	if(Ctrl & _U_UNDER_)   _SendCtrl(h, _under);
	if(Ctrl & _WIDTH_)     _SendCtrl(h,  width);
	if(Ctrl & _U_WIDTH_)   _SendCtrl(h, _width);
	if(Ctrl & _COND_)      _SendCtrl(h,  cond);
	if(Ctrl & _U_COND_)    _SendCtrl(h, _cond);
	if(Ctrl & _NEXT_PRINT)
	{
		flags.nextprint = 1;
		nextpCtrl |= Ctrl;
	}
}

//  Carga una definicion en memoria, la compila y devuelve un puntero
//  a la string ubicada. Si hay algun problema, devuelve nulo.
char *PrnStr(char *secc, char *cmd)
{
	char *s, *p;
	short len, dig;

	s = Trim(ExtractIniChar(secc, cmd));
	if(!s || !strlen(s)) return "";
	for(len = 0, p = &umsg[2]; *s; )
	{
		if(*s == '\\')
		{
			switch(*(++s))
			{
				case 'n':
					*p++ = '\n'; len++; s++;
					break;
				case 'r': 
					*p++ = '\r'; len++; s++;
					break;
				case 't':
					*p++ = '\t'; len++; s++;
					break;
				case 'f':
					*p++ = '\f'; len++; s++;
					break;
				case 'E':
					*p++ = '\x1b'; len++; s++;
					break;
				case 'x':
					if(*(++s) == '(')
					{
						for(dig = 0, s++; *s && isdigit(*s); )
							dig = dig * 10 + (short)((*s++) - '0');
						if(*s == ')') s++;
						*p++ = (char) dig; len++;
					}
					break;
				default:
					*p++ = '\\'; len++;
					if(*s)
					{
						*p++ = *s++;
						len++;
					}
			}
		}
		else
		{
			*p++ = *s++;
			len++;
		}
	}

	// Completa el resultado.
	*((short *) umsg) = len;
        p = (char *) MALLOC(len + 2, "PrnStr");
	memmove(p, umsg, len + 2);
	return p;
}

//  Implementa la funcion de carga de definicion. El parametro que se
//  pasa se utilizara como NOMBRE de la definicion. Esta funcion es de
//  uso interno dado que fuerza la carga de una definicion. Para evitar
//  Definiciones duplicadas, utilizar la funcion LoadDef que retorna un
//  puntero a la clase alocada. El parametro indica tambien el nombre
//  que tendra la seccion en 'printer.ini'. Las definiciones de formato
//  se cargan en demanda, es decir, no se cargan hasta que no se las u-
//  tiliza.
//  La funcion retorna TRUE si todo salio bien, sino, retorna FALSE.
//
int pHand::LoadDef(char *secc)
{
	if(OpenIni("printer")) return 0;
		name = (char *) MALLOC(strlen(secc) + 1, "pHand:LoadDef");
	_strupr(STRCPY(name, secc));

	//  Carga las definiciones.
	bold    = PrnStr(secc, "remarcado");
	_bold   = PrnStr(secc, "saca_remarcado");
	under   = PrnStr(secc, "subrayado");
	_under  = PrnStr(secc, "saca_subrayado");
	width   = PrnStr(secc, "dobleancho");
	_width  = PrnStr(secc, "saca_dobleancho");
	cond    = PrnStr(secc, "condensado");
	_cond   = PrnStr(secc, "saca_condensado");
	init    = PrnStr(secc, "al_empezar");
	release = PrnStr(secc, "al_finalizar");
	port    = Trim(ExtractIniStr(secc, "puerto"));

	//  Finaliza.
	CloseIni();
	next = ph_first;
	ph_first = this;
	if(port) return 1;		// DEBE existir el port.
	return 0;
}

//   La siguiente funcion es la que se utilizara para cargar una defi-
//   nicion. Primero, revisa si la definicion no esta ya creada. Si la
//   estuviere, retorna un puntero a la clase manejadora, sino, crea la
//   clase y carga la definicion. 
//   Retorna: Un puntero a clase definicion de impresora, si todo sale ok.
//            Un puntero NULO, si algo fallo. (Por ejemplo, no encontra-
//            ba la definicion de la clase). Este proceso NO falla, por
//            ejemplo, si la impresora esta fuera de linea. (Esto es par-
//            te del proceso de utilizacion.
//

pHand *LoadPrnDef(char *name)
{
	pHand *p;
	char *nm;

	if(!name) return 0;
    nm = (char *) MALLOC(strlen(name) + 1, "LoadPrnDef");
	_strupr(STRCPY(nm, name));
	for(p = ph_first; p; p = p->next)
	{
		if(!strcmp(p->name, nm))
		{
            FREE(nm);
			return p;
		}
	}

	//  Crea un nuevo manejador.
	p = new pHand;
	if(!p->LoadDef(name))
	{
		delete p;
		p = 0;
	}
	return p;
}

//   Prepara la impresion de un comprobante. Retorna TRUE si el impre-
//   sor esta disponible, sino, retorna FALSE.
int pHand::OpenPrint()
{
	_strupr(port);
	if(memcmp(port, "LPT", 3) && memcmp(port, "COM", 3))
	{
		remove(port);
		close(creat(port, S_IREAD | S_IWRITE));
	}
	h = open(port, O_WRONLY | O_BINARY);

	if(h < 0)
	{
		h = 0;
		return 0;
	}
	_SendCtrl(h, init);
	return 1;
}

//  Finaliza la impresion a traves de un port.
void pHand::ClosePrint()
{
	if(!h) return;
	_SendCtrl(h, release);
	close(h);
}

/////////////////////////////////////////////////////////////////////////
//////////////////////// IMPLEMENTA CLASE defc //////////////////////////

//   Constructor y Destructor.
defc::defc()
{
	memset(this, 0, sizeof(*this));
}
defc::~defc()
{
        if(printer) FREE(printer);
        if(name) FREE(name);
	printer = 0;
}

//   Carga la definicion de una seccion (funcion privada).
int defc::LoadSecc(char *secc, secc_ **s)
{
	static int _alto, conta, pass, us;
	static char treng[6], *p, *up, useSep, nextSep;
	secc_ sc, *bus, *pbus;

	//   Inicializa las estructuras.
	_alto = (int) ExtractIniLong(secc, "alto");

	//   Llena la estructura local 'sc' con los datos del renglon.
    for(conta = 1; conta < 9999; conta++)
	{
		//  Inicializa la seccion.
		memset(&sc, 0, sizeof(sc));
		us = 0;

		//   Extrae los datos de la seccion.
		sprintf(treng, "d%d", conta);
		p = ExtractIniChar(secc, treng);
		if(!p || !strlen(p)) break;

		//   Comienza la extraccion de campos.
		up = p; pass = 1;
		for(up = p; 1; p++)
		{
			switch(pass)
			{
				case 1:					// Extrayendo 'y'.
					if(!*p || *p == ',')
					{
						if(!*p) us = 1; else *p = 0;
						sc.y = atoi(up);
						up = p + 1;
						pass++;
						if(!us)	continue;
					}
					break;

				case 2:					// Extrayendo 'x'.
					if(!*p || *p == ',')
					{
						if(!*p) us = 1; else *p = 0;
						sc.x = atoi(up);
						up = p + 1;
						pass++;
						if(!us) continue;
					}
					break;

				case 3:					// Extrayendo campo.
					if(!*p || *p == ':' || *p == '=')
					{
						useSep = *p;
						if(!*p) us = 1; else *p = 0;
						Trim(up);
                        sc.dato = (char *) MALLOC(strlen(up) + 1, "LoadSecc1");
						_strupr(STRCPY(sc.dato, up));
						up = p + 1;
						pass++;
						if(!us) continue;
					}
					break;
				
				case 4:					// Funcion de control.
					if(!*p || *p == ':' || *p == '=')
					{
						nextSep = *p;
						if(!*p) us = 1; else *p = 0;
						if(useSep == '=')
							sc.len = atoi(up);
						else
						{
							_strlwr(Trim(up));
							if(!strcmp(up, "remarcado"))
								sc.atrib |= _BOLD_;
							else if(!strcmp(up, "condensado"))
								sc.atrib |= _COND_;
							else if(!strcmp(up, "ancho"))
								sc.atrib |= _WIDTH_;
							else if(!strcmp(up, "subrayado"))
								sc.atrib |= _UNDER_;
						}
						useSep = nextSep;
						up = p + 1;
						if(!us) continue;
					}
					break;
			}
			if(!*p) break;
		}

		//  Efectua algunas validaciones en los datos de la estructura.
		if(sc.y == 0) sc.y = 1;	if(sc.y > _alto) sc.y = alto;
		if(sc.x == 0) sc.x = 1;
		if(pass < 3) continue;

		//  Incorpora los datos obtenidos del renglon, a la lista de
		//  renglones del bloque.
		for(pbus = 0, bus = *s; bus;)
		{
			if(sc.y < bus->y || ((sc.y == bus->y) && (sc.x < bus->x)))
				break;
			pbus = bus;
			bus = bus->next;
		}
		if(!pbus)
		{
			if((*s) && (*s)->next) sc.next = (*s)->next;
				*s = new secc_(); // (secc_ *) MALLOC(sizeof(sc), "LoadSecc2");
			memmove(*s, &sc, sizeof(sc));
			continue;
		}
		else
		{
			sc.next = pbus->next;
            pbus->next = new secc_(); // (secc_ *) MALLOC(sizeof(sc), "LoadSecc3");
			memmove(pbus->next, &sc, sizeof(sc));
		}
	}

	return _alto;
}

//   Carga la definicion de un comprobante.
int defc::LoadDef(char *fn)
{
	if(OpenIni(fn)) return 0;
	printer =       Trim(ExtractIniStr( "general", "printer"));
    copias  = (int) ExtractIniLong("general", "copias");
	mcopias = (int) ExtractIniLong("general", "mcopias");
	alto    = (int) ExtractIniLong("general", "alto");
	if(!printer || !copias || !mcopias || !alto)
	{
		CloseIni();
		return 0;
	}

	//   Carga las secciones.
	a_titulo     = LoadSecc("titulo",     &titulo);
	a_encabezado = LoadSecc("encabezado", &encabezado);
	a_renglon    = LoadSecc("renglon",    &renglon);
	a_piecont    = LoadSecc("pie-cont",   &piecont);
	a_piefin     = LoadSecc("pie-fin",    &piefin);
	
	//   Engancha en la lista.
	next = dc_first;
	dc_first = this;

	//   La definicion del PRINTER la carga al IMPRIMIR!!!.
	CloseIni();
	return 1;
}

//  Imprime el contenido de una seccion.
void defc::PrintSecc(secc_ *u, int _alto)
{
    int a_y, a_x, i, uLen;
	secc_ *s;
	char *p;

	if(!u || !_alto) return;				// La seccion no esta definida.
	a_y = 1; a_x = 1;				// Posicion actual.
	for(s = u; s; s = s->next)
	{
		while(a_y < s->y)			// Empareja la linea. (coord y).
		{
			a_y++; a_x = 1;
			prn->Print("\r\n");
		}

		for(p = msg; a_x < s->x; a_x++)	// Empareja coord x.
			*p++ = ' ';
		if(p != msg)
		{
			*p = 0;
			prn->Print(msg);
		}
		switch(*s->dato)
		{
			case '@':						// Imprime variable.
				uLen = getVar(s->dato);
				break;
			case '#':						// Imprime constante.
				uLen = getConst(s->dato);
				break;
			default:						// Texto directo.
				uLen = strlen(s->dato);
				STRCPY(msg, s->dato);
		}
		i = strlen(msg);
		if(i < uLen)
		{
			memmove(msg + (i - uLen), msg, (i - uLen));
			memset(msg, ' ', (i - uLen));
		}
		if(uLen < (int) strlen(msg)) uLen = (int) strlen(msg);
		if(s->atrib) prn->SendCtrl(s->atrib | _NEXT_PRINT);
		prn->Print(msg);
		a_x += uLen;
	}
    do
    {
        a_y++;
        prn->Print("\r\n");

    }  while(a_y < _alto);
	gRenglon += _alto;
}

//  Imprime todas las copias de un comprobante.
int defc::Print()
{
	int lenRengSecc, primeraHoja;

	//   Carga la definicion del driver, si no se cargo todavia.
	if(!prn)
	{
		prn = LoadPrnDef(printer);
		if(!prn) return 0;
	}
	
	if(!prn->OpenPrint()) return 0;
	lenRengSecc = alto - a_piefin;
	for(gCopia = 1; gCopia <= copias; gCopia++)
	{
		ResetRengInfo();
		gRenglon = 9999;
		primeraHoja = 1;
		while(1)
		{
			if(!GetRengInfo())
			{
				while(gRenglon <= lenRengSecc)
				{
					prn->Print("\r\n");
					gRenglon++;
				}
				break;
			}
			if(gRenglon > lenRengSecc)
			{
				if(!primeraHoja)
				{
					PrintSecc(piecont, a_piecont);
					prn->Print("\x0c");
				}
				gRenglon = 1;
				if(primeraHoja)
				{
					if(titulo && a_titulo)
						PrintSecc(titulo, a_titulo);
					else
						PrintSecc(encabezado, a_encabezado);
				}
				else
					PrintSecc(encabezado, a_encabezado);
				primeraHoja = 0;

			}
			if(!renglon || !a_renglon)
				gRenglon++;
			else
				PrintSecc(renglon, a_renglon);
		}
		PrintSecc(piefin, a_piefin);
		prn->Print("\x0c");
	}
	prn->ClosePrint();
	return 1;
}

/////////////////////////////////////////////////////////////////////////
//////////////////   MANEJO DE LAS CONSTANTES   /////////////////////////

//   Carga el archivo de Constantes.
static void LoadConst()
{
	static char seccN[10], *p, *up;
	static const_ co;
	int i, pass, us;

	gConst = 1;
	if(OpenIni("const")) return;
	for(i = 1; i < 9999; i++)
	{
		us = 0;
		memset(&co, 0, sizeof(co));
		sprintf(seccN, "d%d", i);
		p = ExtractIniChar("constantes", seccN);
		if(!p || !strlen(p)) break;
		for(up = p, pass = 1; 1; p++)
		{
			switch(pass)
			{
				case 1:				// Extrayendo nombre de constante.
					if(!*p || *p == ',')
					{
						if(!*p) us = 1; else *p = 0;
						_strupr(Trim(up));
                        co.name = (char *) MALLOC(strlen(up) + 1, "LoadConst1");
						STRCPY(co.name, up);
						pass++;
						up = p + 1;
						if(!us) continue;
					}
					break;

				case 2:				// Extrayendo largo de la constante.
					if(!*p || *p == ',')
					{
						if(!*p) us = 1; else *p = 0;
						co.len = atoi(up);
						up = p + 1;
						pass++;
						if(!us) continue;
					}
					break;

				case 3:				// Extrayendo valor de la constante.
					if(!*p)
					{
						Trim(up);
                        co.value = (char *) MALLOC(strlen(up) + 1, "LoadConst2");
						STRCPY(co.value, up);
					}
					break;
			}
			if(!*p) break;
		}
		if(!co.name || !strlen(co.name) ||
		   !co.value || !strlen(co.value) ||
		   !co.len)
		   continue;
		co.next = co_first;
        co_first = new const_(); // (const_ *) MALLOC(sizeof(co), "LoadConst3");
		memmove(co_first, &co, sizeof(co));
	}
	CloseIni();
}

//   Devuelve el valor formateado de una constante.
int getConst(char *cName)
{
	const_ *co;
	int i;

	if(!gConst) LoadConst();
	cName++; // Saltea el #
	for(co = co_first; co; co = co->next)
		if(!strcmp(cName, co->name))
			break;
	msg[0] = 0;
	if(!co)
		return 0;
	if(co->value)
		STRCPY(msg, co->value);
	i = strlen(msg);
	while(i < co->len)
	{
		msg[i] = ' ';
		msg[++i] = 0;
	}
	return co->len;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////   MANEJO DE VARIABLES /////////////////////////////

//  Prototipos de las funciones que retornan datos de variables.

//  Estructura con la pista para ubicar los datos de las variables.

char *va_codcli(char *),      *va_nombre(char *),     *va_direccion(char *),
     *va_localidad(char *),   *va_codpos(char *),     *va_cuit(char *),
	 *va_fecha(char *),       *va_numero(char *),     *va_ncopia(char *),
	 *va_dcopia(char *),      *va_desprec(char *),    *va_codigo(char *),
	 *va_descrip(char *),     *va_cantidad(char *),   *va_cantidadz(char *),
	 *va_unitarion(char *),   *va_unitarionz(char *), *va_rtotaln(char *),
	 *va_unitariot(char *),   *va_unitariotz(char *),
	 *va_rtotalt(char *),     *va_transporten(char *), 
	 *va_transportet(char *), *va_totaln(char *),     *va_iva(char *),
	 *va_diva(char *),   	  *va_total(char *),
	 *va_recargosp(char *),   *va_recargosm(char *),
	 *va_descuentosp(char *), *va_descuentosm(char *),*va_condiva(char *);

struct variab_ {
		char *name;						// Nombre de la variable.
		char *(*func)(char *);			// Funcion que ejecuta.
} variab[] = {

	{"CODCLI", va_codcli},				// Datos de la cuenta corriente.
	{"NOMBRE", va_nombre},
	{"DIRECCION", va_direccion },
	{"LOCALIDAD", va_localidad},
	{"CODPOS", va_codpos},
	{"CONDIVA", va_condiva },
	{"CUIT", va_cuit},
	{"FECHA", va_fecha},				// Fecha de hoy.
	{"NUMERO", va_numero},
	{"NCOPIA", va_ncopia},				// Copia 1, 2, ...
	{"DCOPIA", va_dcopia},				// Original, Duplicado,...
	{"DESPREC", va_desprec},
	{"CODIGO", va_codigo},				// Codigo de PLU o DEPTO
	{"DESCRIP", va_descrip},	
	{"CANTIDAD", va_cantidad}, 
	{"CANTIDADZ", va_cantidadz},		// Si cantidad=1, ""...
	{"UNITARION", va_unitarion},
	{"UNITARIONZ", va_unitarionz},		// Si cantidad=1, ""...
	{"UNITARIOT", va_unitariot},
	{"UNITARIOTZ", va_unitariotz},
	{"RTOTALN", va_rtotaln},			// Total NETO del renglon
	{"RTOTALT", va_rtotalt},			// Total FINAL del renglon
	{"TRANSPORTEN", va_transporten},	// Monto de Transporte, NETO
	{"TRANSPORTET", va_transportet},	// Monto de Transporte, FINAL
	{"TOTALN", va_totaln},				// Total final NETO
	{"IVA1", va_iva}, {"IVA2", va_iva},	// Montos de impuestos.
	{"IVA3", va_iva}, {"IVA4", va_iva},
	{"IVA5", va_iva},
	{"DIVA1", va_diva}, {"DIVA2", va_diva}, // Descripciones de Impuestos
	{"DIVA3", va_diva}, {"DIVA4", va_diva},
	{"DIVA5", va_diva},
	{"TOTAL", va_total},				// Total final
	{"RECARGOSP", va_recargosp},		// Porcentaje de recargos.
	{"RECARGOSM", va_recargosm},		// Monto de Recargos
	{"DESCUENTOSP", va_descuentosp},	// Porcentaje de descuentos.
	{"DESCUENTOSM", va_descuentosm}		// Monto de descuentos.
};

//   Implementacion de las funciones que reportan datos.
char *va_codcli(char *v)
{
	sprintf(msg, "%-6s", c_cod);
	return msg;
}
char *va_nombre(char *v)
{
	sprintf(msg, "%-30s", c_nombre);
	return msg;
}
char *va_direccion(char *v)
{
	sprintf(msg, "%-30s", c_direcc);
	return msg;
}
char *va_localidad(char *v)
{
	sprintf(msg, "%-30s", c_localidad);
	return msg;
}
char *va_codpos(char *v)
{
	return "4400";
}
char *va_cuit(char *v)
{
    sprintf(msg, "%-13s", c_dni);
	return msg;
}
char *va_condiva(char *v)
{
	switch(atoi(c_condiva))
	{
		case 1: return "Resp. Inscrip.";
		case 2: return "Resp.NO Insc.";
		case 3: return "Cons. Final";
		case 4: return "Exento";
		case 5: return "Monotributo";
		default: return "No Categoriz.";
	}
	return "";
}
char *va_fecha(char *v)
{
	return fecha();
}
char *va_numero(char *v)
{
	return "91929394";
}
char *va_ncopia(char *v)
{
	sprintf(msg, "%2d", gCopia);
	return msg;
}
char *va_dcopia(char *v)
{
	if(gCopia <= 4)
		sprintf(msg, "%-14s", _dcopia[gCopia-1]);
	 else
		sprintf(msg, "     Copia: %02d", gCopia);
	return msg;
}
char *va_desprec(char *v)
{
	return "    PRECIO";
}
char *va_codigo(char *v)
{
	sprintf(msg, "%-12s", g_Codigo);
	if(strlen(msg) > 12) msg[12] = 0;
	return msg;
}
char *va_descrip(char *v)
{
	sprintf(msg, "%-40s", g_Descrip);
	if(strlen(msg) > 40) msg[40] = 0;
	return msg;
}
char *va_cantidad(char *v)
{
	if(ceil(g_Cantidad) == g_Cantidad)
		sprintf(msg, "%8.0lf    ", g_Cantidad);
	 else
		sprintf(msg, "%12.3lf", g_Cantidad);
 	if(strlen(msg) > 13) msg[13] = 0;
	return msg;
}
char *va_cantidadz(char *v)
{
	if(g_Cantidad == 1.0)
		return blank12;
	return va_cantidad(v);
}
char *va_unitarion(char *v)
{
	sprintf(msg, "%12.2lf", g_UnitarioN);
	return msg;
}
char *va_unitarionz(char *v)
{
	if(g_Cantidad == 1.0)
		return blank12;
	return va_unitarion(v);
}
char *va_rtotaln(char *v)
{
	sprintf(msg, "%12.2lf", g_RtotalN);
	return msg;
}
char *va_unitariot(char *v)
{
	sprintf(msg, "%12.2lf", g_UnitarioT);
	return msg;
}
char *va_unitariotz(char *v)
{
	if(g_Cantidad == 1.0)
		return blank12;
	return va_unitariot(v);
}
char *va_rtotalt(char *v)
{
	sprintf(msg, "%12.2lf", g_RtotalT);
	return msg;
}
char *va_transporten(char *v)
{
	sprintf(msg, "%12.2lf", g_TransporteN);
	return msg;
}
char *va_transportet(char *v)
{
	sprintf(msg, "%12.2lf", g_TransporteT);
	return msg;
}
char *va_totaln(char *v)
{
	sprintf(msg, "%12.2lf", g_TotalN);
	return msg;
}
char *va_iva(char *v)
{
	int uIva;

	uIva = atoi(v + 3) - 1;
	sprintf(msg, "%12.2lf", g_Iva[uIva]);
	return msg;
} 
char *va_diva(char *v)
{
	int uIva;

	uIva = atoi(v + 4) - 1;
	sprintf(msg, "%-12s", g_DIva[uIva]);
	return msg;
}
char *va_total(char *v)
{
	sprintf(msg, "%12.2lf", g_Total);
	return msg;
}
char *va_recargosp(char *v)
{
	sprintf(msg, "%12.2lf", g_RecargosP);
	return msg;
}
char *va_recargosm(char *v)
{
	sprintf(msg, "%12.2lf", g_RecargosM);
	return msg;
}
char *va_descuentosp(char *v)
{
	sprintf(msg, "%12.2lf", g_DescuentosP);
	return msg;
}
char *va_descuentosm(char *v)
{
	sprintf(msg, "%12.2lf", g_DescuentosM);
	return msg;
}

//  Rutina de comparacion para el qsort.
int va_comp1(const void *var1, const void *var2)
{
	return strcmp(((variab_ *) var1)->name, ((variab_ *) var2)->name);
}
//  Rutina de comparacion para el bsearch.
int va_comp2(const void *key, const void *var)
{
	return strcmp((char *) key, ((variab_ *) var)->name);
}

//   Funcion que devuelve el valor de una variable.
int getVar(char *vName)
{
	variab_ *va;
	char *res;
	
	if(!gVari)
	{
		gVari = 1;
		qsort(variab, sizeof(variab) / sizeof(variab_), sizeof(variab_),
			  va_comp1);
	}
	vName++;
	va = (variab_ *) bsearch(vName, variab, sizeof(variab) / sizeof(variab_),
		         sizeof(variab_), va_comp2);
	msg[0] = 0;
	if(!va)	return 0;
	res = (*va->func)(vName);
	if(res != msg)
		STRCPY(msg, res);
	return strlen(msg);
}


// Carga una definicion de comprobante
defc *LoadComprobDef(char *name)
{
	defc *p;
	char *nm;

	if(!name) return 0;
        nm = (char *) MALLOC(strlen(name) + 1, "LoadComprobDef1");
	_strupr(STRCPY(nm, name));
	for(p = dc_first; p; p = p->next)
	{
		if(!strcmp(p->name, nm))
		{
            FREE(nm);
			return p;
		}
	}

	//  Crea un nuevo manejador.
	p = new defc;
	p->name = nm;
	if(!p->LoadDef(nm))
	{
		delete p;
		p = 0;
	}
	return p;
}

