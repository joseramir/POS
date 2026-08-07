/*
 *   PosDisp.c
 *   Establece al dispositivo logico CASHDISP. (Visor del Cajero).
*/

#include <stdafx.h>

#ifdef POSXP
#include "MainForm.h"
#endif

extern int networkAvailable;	// Sync.cpp - Para determinar si el server esta o no online.

short giantesq[16] = {          // Esquema default de colores
    { AMARILLO * 16 + NEGRO },  // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
    { AMARILLO * 16 + NEGRO },  // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color del fondo cuando usa relleno.
	{ ROJO * 16 + BLANCO },		// Color para titulos resaltados.
};

short detaesq[16] = {           // Esquema default de colores
    { CELESTE * 16 + BLANCO },  // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
    { CELESTE * 16 + BLANCO },  // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color del fondo cuando usa relleno.
	{ ROJO * 16 + BLANCO },		// Color para titulos resaltados.
};

char *GChar[14][4];
char *GCharBLKFONT[][4] = {

    { "00",            // '0'  128,129,160,161
      "00", },

    { "1 ",            // '1'  130,131,162,163
      "  ",  },

    { "2 ",            // '2'  132,133,164,165
      "  ",  },

    { "3 ",            // '3'  134,135,166,167
      "  ",  },

    { "4 ",            // '4'  136,137,168,169
      "  ",  },

    { "5 ",            // '5'  138,139,170,171
      "  ",  },

    { "6 ",            // '6'  140,141,172,173
      "  ",  },

    { "7 ",            // '7'  142,143,174,175
      "  ",  }, 

    { "8 ",            // '8'  148,149,184,181
      "  ",  },

    { "9 ",            // '9'  150,151,182,183
      "  ",  }, 

    { "  ",            // ' '
      "  "   },         

    { ". ",            // '.'   32, 32,152,153
      "  "   },        

    { "- ",            // '-'  154,155, 32, 32
      "  " } 
};

char *GCharDEBUG[][4] = {

    { "0 ",            // '0'  128,129,160,161
      "  ", },

    { "1 ",            // '1'  130,131,162,163
      "  ",  },

    { "2 ",            // '2'  132,133,164,165
      "  ",  },

    { "3 ",            // '3'  134,135,166,167
      "  ",  },

    { "4 ",            // '4'  136,137,168,169
      "  ",  },

    { "5 ",            // '5'  138,139,170,171
      "  ",  },

    { "6 ",            // '6'  140,141,172,173
      "  ",  },

    { "7 ",            // '7'  142,143,174,175
      "  ",  }, 

    { "8 ",            // '8'  148,149,184,181
      "  ",  },

    { "9 ",            // '9'  150,151,182,183
      "  ",  }, 

    { "  ",            // ' '
      "  ",  },         

    { ". ",            // '.'   32, 32,152,153
      "  ",  },        

    { "- ",            // '-'  154,155, 32, 32
      "  ", }       
};

char *GCharNFONT[][4] = {

    { "€",            // '0'  128,129,160,161
      " ¡"   },

    { "‚ƒ",            // '1'  130,131,162,163
      "¢£"   },

    { "„…",            // '2'  132,133,164,165
      "¤¥"   },

    { "†‡",            // '3'  134,135,166,167
      "¦§"   },

    { "ˆ‰",            // '4'  136,137,168,169
      "¨©"   },

    { "Š‹",            // '5'  138,139,170,171
      "ª«"   },

    { "Œ",            // '6'  140,141,172,173
      "¬­"   },

    { "Ž",            // '7'  142,143,174,175
      "®¯"   }, 

    { "”•",            // '8'  148,149,184,181
      "¸µ"   },

    { "–—",            // '9'  150,151,182,183
      "¶·"   }, 

    { "  ",            // ' '
      "  "   },

    { "  ",            // '.'   32, 32,152,153
      "˜™"   },        

    { "š›",            // '-'  154,155, 32, 32
      "  "   }     
};

#define DIG_ANCHO 2
#define DIG_ALTO  2

int visor, detalle, detlin, vistot;
double lTot;

//   Prototipos.
int InitCashDisp(void);
void DispCash(double, int);
void DispDeta(char *, register char *);
void DispDeta2(char *, register char *, char *);

//   Inicializa los visores.

int InitCashDisp()
{
	visor = wDefine(0, 40, 1+DIG_ALTO,40+13*(DIG_ANCHO+1), BORDERSINGLE, SNGCURS, giantesq, "", "");
    vistot = wDefine(0, 0, 3, 38, BORDERSINGLE, SNGCURS, 0, " Total ", "");
    detalle = wDefine(5, 1, 19, 78, BORDERSINGLE, SNGCURS, detaesq, "", "");
    adic = wDefine(21, 24, 24, 79, BORDERSINGLE, SNGCURS, 0, "", "");
	HostW = wDefine(21, 0, 24, 23, BORDERSINGLE, SNGCURS, 0, "", "");

	//  Selecciona el font a utilizar.
#ifdef __VSC__
    memmove(GChar, GCharDEBUG, 4*13*sizeof(char *));
#else
    if(videoType == 1)
        memmove(GChar, GCharNFONT, 4*13*sizeof(char *));
    else
	{
        if(videoType == 2)
            memmove(GChar, GCharBLKFONT, 4*13*sizeof(char *));
        else
            memmove(GChar, GCharDEBUG, 4*13*sizeof(char *));
	}
#endif

	wActivate(visor);
	DispCash(0, 0);
	return 1;
}

//   Muestra el total de la operacion, refrescando la grilla opcionalmente
void PrintTktTot(bool refreshGrid)
{
	char msgTotal[80], msgSal[80], msgCobrar[80], msgCC[80];

	msgTotal[0] = msgSal[0] = msgCobrar[0] = msgCC[0] = 0;
	char *strTotal = xTotal.VerPendiente().ToString(2);
	sprintf(msgTotal, "TOTAL=%s", strTotal);

	int ctaCod = atoi(c_cod);
	if(ctaCod != 0 && (globalCCOnline || globalCobraCC || GlobalCCSelected))
	{
		sprintf(msgCC, "C/C [%c]: %lld-%s",
			(_stricmp(c_condiva, "RI") == 0)? 'A': 'B',
			_atoi64(c_cod), 
			c_nombre);
		if(!globalCobraCC)
		{
			double saldo = atof_(c_saldo);
			double salmax = atof_(c_salmax);
			sprintf(msgSal, "SAL: %.2lf", salmax-saldo-xTotal.VerTotal().ToDouble());
		}
		else
			sprintf(msgCobrar, "COBRAR: %.2lf", c_MontoCobro);
	}
	else if(globalCheckPriceOverflow)
	{
		sprintf(msg, "MAX: %.2lf", c_PorceMax-c_Consumo-xTotal.VerTotal().ToDouble());;
		wLocate(0, 23);
		wPstr(msgSal);
		sprintf(msgCC, "C/C [%c]: %lld-%s",
			(_stricmp(c_condiva, "RI") == 0)? 'A': 'B',
			_atoi64(c_cod), 
			c_nombre);
		//WriteBootLog("Pasa por Cliente aqui1");
	}

#ifdef POSXP
	MainForm::F->SetTotDisplay(msgTotal, msgSal, msgCobrar, msgCC, (int)xCnt);	
	if(refreshGrid) MainForm::F->RefreshGrid();
#else
	wActivate(vistot);
	wCls();
	wLocate(0, 1);
	wPstr(msgTotal);
	wLocate(0, 14);
	wPstr(msgSal);
	wLocate(0, 20);
	wPstr(msgCobrar);
	wLocate(1, 1);
	wPstr(msgCC);
#endif
}

//   Muestra el total de la operacion, sin refrescar la grilla
void PrintTktTot() { PrintTktTot(false); }

//   Imprime una string en la ventana de detalle.
void DispDeta(char *l, char *r)
{
	static char detrng[45];
	register char *p;
	int x, lr;

//	CustDisp(l, r);
	STRCPY(detrng, "                                        ");
	memmove(detrng, l, strlen(l));        // Mensaje a la izquierda
	lr = strlen(r);
    for(p = detrng, x = 40; x > lr; x--, p++)
        ;
	while(*r)
		*p++ = *r++;
	if(pf->GlobalVoucherFlag) 
		pf->PrintVoucher(detrng);
}

//   Imprime una string en la ventana de detalle.
void DispDeta2(char *l, char *r, char *aux)
{
	char detrng[45];
	register char *p;
	int x, lr;

	//	CustDisp(l, r);
	STRCPY(detrng, "                                      ");
	memmove(detrng, l, strlen(l));        // Mensaje a la izquierda
	lr = strlen(r);
	for(p = detrng, x = 38; x > lr; x--, p++)
		;
	while(*r)
		*p++ = *r++;
	STRCPY(aux, detrng);	
}

//   Displaya un numero en caracteres gigantes.
void DispCash(double value, int dec)
{
	static char rng[24], frmt[24];

	//   Prepara la configuracion gigante.

	if(dec)
	 {
		sprintf(frmt, "%%%d.%dlf", 12, dec);
		sprintf(rng, frmt, value);
	 }
	 else
		sprintf(rng, "%12.0lf", value);

	//   Imprime los digitos gigantes.
#ifdef POSXP
	MainForm::F->SetNumDisplay(rng);
#else
	int ch, pasada;
	wActivate(visor);
	register char *x;
    for(pasada=0; pasada<DIG_ALTO; pasada++)
	{
        wLocate(pasada, 1);
		for(x = rng; *x; x++)
		{
			switch(*x)
			{
				case '-':
					ch = 12;
					break;
				case '.':
					ch = 11;
					break;
				case ' ':
					ch = 10;
					break;
				default:
					ch = *x - '0';
			}
			wPstr(GChar[ch][pasada]);
            wPch(' ');
		}
	}
#endif
}

//   Displaya en la ventana adicional de visor.
void DispAdic(int y, int x, char *msg)
{
#ifdef POSXP
	MainForm::F->SetMultiDisplay(msg);
#else
	int prvcol;

	wActivate(adic);
	wLocate(y, x);
	if(*msg == '\\')
	 {
		prvcol = wSetColor(*(msg + 1));
		wPstr(msg + 2);
		wSetColor(prvcol);
	 }
	 else
		wPstr(msg);
	wActivate(visor);
#endif
}

//   Refresca la ventana de datos adicionales.
void AdicClear()
{
#ifdef POSXP
	MainForm::F->SetMultiDisplay("");
	PrintTktTot();
	MainForm::F->SetCaptions(
		consmode,
		GlobalTF,
		rMode,
		NameLista(),
		cko_enab && ckaNro,
		clkName(),
		ven_enab && venNro,
		venName(),
		reparto_nro,
		networkAvailable);
#else
	wActivate(adic);
	wCls();
	if(consmode)
        DispAdic(0, 1, ">CONSULTA<");
	if(GlobalTF)
		DispAdic(0, 27, "TICK.FACTURA");
	else
		DispAdic(0, 27, "            "); //NameComp());
	DispAdic(0, 22, modemsg[rMode]);
	STRCPY(msg, NameLista());
    DispAdic(1, 44, Padl(msg, 7, ' '));
	if(cko_enab && ckaNro)
        DispAdic(1, 1, clkName());
    if(ven_enab && venNro)
	{
		wLocate(1, 20);
		wPch(' ');
           DispAdic(1, 21, venName());
	}
	if(reparto_nro)
	{
		sprintf(msg, "REPARTO:%d", reparto_nro);
		DispAdic(0, 12, msg);
	}
	if(!networkAvailable)
		DispAdic(0, 1, "OFFLINE");
	if(xCnt != 0)
	{
		sprintf(msg, "Articulos=%.0lf", xCnt);
		DispAdic(1, 30, msg);
	}

	wActivate(visor);
#endif
}

//   Al salir del pos, remueve todas las ventanas creadas y deja la pantalla limpia.
void ResetCashDisp()
{
	wRelease(visor);            // Remueve las ventanas de la pantalla.
	wRelease(detalle);
	wRelease(vistot);
	wRelease(adic);
	
	ucolor = 7;					// Borra la pantalla.
	FCls();
}
