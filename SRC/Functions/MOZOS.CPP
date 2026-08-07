/*
 *   Mozos.c
 *   Implementa la utilizacion de mozos (Vendedores).
*/

#include <stdafx.h>
#include "funcs.h"               // Carga la estructura ven_

struct ven_ *vended;
struct DDven_ ven;
char venNom[33];
dbf mozo;
static char uCod[5];
struct DDven_ vv;
int globalVendedTop, VendedPrinted;

int vencmp2(void const *k, void const *p)
{
    return *((int *) k) - ((struct ven_ *) p)->nro;
}

//   Al iniciar un ticket, imprime los datos del cajero en el impresor.
void PrintVen()
{
	struct ven_ *p;
	static char tNom[60], tNro[10], tMesa[60];
    int tLen;

	VendedPrinted = 1;
	if(!ven_enab) return;
	p = (ven_ *) bsearch(&venNro, vended, cVen_, sizeof(struct ven_),
		                 vencmp2);
	if(p)
	{
		STRCAT(STRCPY(tNom, vFuncName), _itoa(venNro, tNro, 10));
		STRCAT(STRCAT(tNom, " -"), p->Nom);
	}
	else
		sprintf(tNom, "%s: 0 -%s Invalido", vFuncName, vFuncName);

	if(actSusp)
		sprintf(tMesa, "Mesa: %d * ", actSusp);
	else
		tMesa[0] = 0;
	tLen = strlen(tMesa);
	memmove(&tNom[tLen], tNom, strlen(tNom) + 1);
	memmove(tNom, tMesa, tLen);
    DispDeta(tNom, "");
	pf->PrintText("******************************");
	pf->PrintText(tNom);
	pf->PrintText("******************************");
}

void PrintVend(void *p_)
{
	struct DDven_ *p;

	p = (struct DDven_ *) p_;
	if(!p->logflg) return;
    venNro = p->nro;
    PrintVen();
}

//  Retorna el nombre del vendedor actual, a quien lo necesite.
char *venName()
{
	static int venDispNro;
	struct ven_ *p;
	char *x;

	if(venNro == venDispNro) return venNom;
	p = (ven_ *) bsearch(&venNro, vended, cVen_, sizeof(struct ven_),
		                 vencmp2);
	if(p)
		sprintf(venNom, "Mozo: %d-%s",venNro, p->Nom);
	 else
		STRCPY(venNom, "Mozo: Invalido");
	for(x = venNom + (strlen(venNom) - 1); x >= venNom && isspace(*x); x--)
		*x = 0;
	return venNom;
}

//  Proceso de la funcion vendero.
void ProcVen(void *p_, int fwrite)
{
    struct DDven_ *p;

    p = (struct DDven_ *) p_;
	if(fwrite) 
	{
		WriteDump(p);
		WriteEOPNoFiscal();
	}

	if(p->logflg)
		venNro = p->nro;
	 else
		venNro = 0;
	AdicClear();
	HClear();
}

//   Imprime un report de ventas del vendedor / report total de vendedores.
void ReportVen()
{
	static int s_mpago, s_cajero, s_depto, s_vend, s_horar, s_plus;
	static struct DDzeta_ zet;

	if(!ven_enab) return;
	if(inOper || xTotal.VerTotal() != 0)
	{
		Alert("No puede usar esta funcion en este momento",
			  "Cierre el ticket antes !!!");
		return;
	
	}
	s_mpago = z_mpago; s_cajero = z_cajero;
	s_depto = z_depto; s_vend = z_vended;
	s_horar = z_horar; s_plus = z_plus;
	z_mpago = 0; z_cajero = 0; z_depto = 0; z_vended = 1;
	z_horar = 0; z_plus = 0;
	globalVendedTop = (int) xReg;
	CalculateExpandedZeta(true);
	PrintZetaVend((int) xReg);	// En subtot.cpp
    FREE(zz_vend);
	globalVendedTop = 0;
	z_mpago = s_mpago; z_cajero = s_cajero;
	z_depto = s_depto; z_vended = s_vend;
	z_plus = s_plus;   z_horar = s_horar;

	//   Registra la operación, si es de Zeta.
	if(rMode > 2)
	{
		//  Graba la operacion en el log.
		zet.func = DZVended;
		zet.znro = zeta;
		zet.tnro = ticket;
		zet.fecha = date();
		zet.nro = (int) xReg;
		ProcZeta(&zet, 1);
	}

	HClear();
}

//   A efectos de los reports, de una estructura de trans.dat retorna
//   alguna informaci¢n sobre la operaci¢n.
void ValuesVend(void *ven_, int *_cual, char **_des, int *_flag)
{
	int svNro;
    struct DDven_ *ven;

    ven = (struct DDven_ *) ven_;
    *_cual  = ven->nro;
	*_flag  = ven->logflg;
	svNro = venNro;
	venNro = ven->nro;
	*_des   = venName();
	venNro = svNro;
}

//   Funcion de procesamiento de la tecla de vendedor.
void vendedor(int cual)
{
	if(!ven_enab)
	{
		Alert("No puede usar esta funcion, pues",
			  "fue deshabilitada en 'opciones.ini'");
		return;
	}
	if(rMode > 1)                   // Reporte?
	{
		ReportVen();
		return;
	}

	//   Registro de vendedor. Simplemente, registra el evento en el
	//   trans.dat de la registradora.
	if(xReg == 0.0 && !venNro)
	{
		Alert("Ingrese el nro de mozo a activar",
			  "ANTES de pulsar la tecla 'mozo'.");
		return;
	}

	//   Procesa la conexion/desconexion del vendedor.
	if(venNro)
	 {
		vv.nro = venNro;                // Registra la DESCONEXION
		venNro = 0;
		vv.logflg = 0;
		vv.func = DVen;
		ProcVen(&vv, 1);
		AdicClear();
	 }
	if(((int) xReg) != 0)
	 {
		venNro = (int) xReg;            // Registra la CONEXION.
		vv.nro = venNro;
		vv.logflg = 1;
		vv.func = DVen;
		ProcVen(&vv, 1);
		AdicClear();
	 }
}

//  Si está activo el sistema de vendedors y es obligatorio un
//  vendedor por ticket, entonces la siguiente funcion fuerza
//  que el usuario ingrese el vendedor del ticket.
EditWindow *venWin = NULL;
int buscaVen2(char *cual, void *bag)
{
	static char uCod[10];

	Padl(Trim(STRCPY(uCod, cual)), 3, ' ');
	if(mozo.Locate("codigo", uCod))
	{
		venWin->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el mozo/vendedor '%s'.", cual);
	Alert(msg, "");
	return 0;
}

int whenDes(char *newValue, void *bag)
{
	return 0;
}

void ForceVen()
{
	int hWin;

	if(!ven_enab || !ven_obli || venNro) return;
	
	mozo.Use("pos\\mozos");
	hWin = wDefine(8, 18, 14, 62, BORDERSINGLE, SNGCURS, prograEsq, "Seleccione Mozo", "Pulse F8 p/Seguir");
	venWin = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Numero:");
	wLocate(2, 2);  wPstr("Nombre:");

	venWin->AddGet(1, 10, NUMBER_RIGHT_ALIGNED,  2, 0, uCod, NULL, NULL, buscaVen2);
	venWin->AddGet(2, 10, "nombre",     &mozo, NULL, &whenDes, NULL);
	while(1)
	{
		venWin->AnimaGets(1);
		Padl(Trim(uCod), 3, ' ');
		if(mozo.Locate("codigo", uCod)) break;
	}

	wRelease(hWin);
	delete venWin; venWin = NULL;

	//   Registra la conexion.
	venNro = atoi(uCod);
	vv.nro = venNro;
	vv.logflg = 1;
	vv.func = DVen;
	ProcVen(&vv, 1);

	//   Deja todo en orden.
	AdicClear();
	mozo.Close();
}

//  Carga, desde el archivo 'mozos.def' una definicion completa
//  de los vendedores.
int vencmp1(void const *d1, void const *d2)
{
    return ((struct ven_ *) d1)->nro - ((struct ven_ *) d2)->nro;
}

int LoadVen()
{
	 struct ven_ *vv;
	 char cod[4], des[25];

	 cVen_ = 0;
	 if(mozo.Use("pos\\mozos"))
	 {
		vended = new ven_[mozo.RecCount()];
		mozo.Go(DB_GO_TOP);
		while(!mozo.Eof())
		{
			mozo.Get("codigo", cod);
			mozo.Get("nombre", des);
			vv = &vended[cVen_++];
			vv->nro = atoi(cod);
			STRCPY(vv->Nom, Trim(des));
			mozo.Skip(1);
		}
		mozo.Close();
	 }
	 else
		 AbnormalEnd("No puedo cargar mozos/vendedores");

	 if(cVen_)
		qsort(vended, cVen_, sizeof(struct ven_), vencmp1);
     return 1;
}

//   Funcion de 'validacion' del codigo de mozo ingresado.
int buscaVen(char *cual, void *bag)
{
	static char uCod[10];
	int si;

	Padl(Trim(STRCPY(uCod, cual)), 3, ' ');
	if(mozo.Locate("codigo", uCod))
	{
		venWin->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el mozo/vendedor '%s'.", cual);
	si = GetSiNo(msg, "Lo incorporo como mozo/vendedor nuevo?");
	if(!si)
		return 0;
	mozo.AppendBlank();
	mozo.Replace("codigo", uCod);
	venWin->ShowGets();
	return 1;
}

//   Abre una ventana y permite editar el archivo de departamentos.
void PrograVended()
{
	int hWin;

	mozo.Use("pos\\mozos");
	hWin = wDefine(7, 18, 15, 62, BORDERSINGLE, SNGCURS, prograEsq, "Programacion de Mozos", "");
	venWin = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Numero:");
	wLocate(2, 2);  wPstr("Nombre:");

	venWin->AddGet(1, 10, NUMBER_RIGHT_ALIGNED,  2, 0, uCod, NULL, NULL, buscaVen);
	venWin->AddGet(2, 10, "nombre",     &mozo, NULL, NULL, NULL);

	venWin->AnimaGets(1);
	wRelease(hWin);
	delete venWin; venWin = NULL;
	mozo.Close();
    
	FREE(vended); LoadVen();
	HClear();
}	
