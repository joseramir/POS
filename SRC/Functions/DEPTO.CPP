/*
 *   Depto.c
 *   Programa la funcionalidad de 'departamento', para registracion
 *   manual de ventas a Departamentos directos o indirectos.
*/

#include <stdafx.h>
#include "funcs.h"          // Carga la estructura dep_

struct dep_ *deptos;
struct DDdep_ dep;
dbf dbdep;
double LastDep;
EditWindow *depWin = NULL;
char uCod[5];

/*
 *  La siguiente funcion se utiliza para registrar la venta a un departa-
 *  mento, cuando se presiona una tecla programada para tal fin.
*/
int depcmp2(void const *k, void const *d)
{
    return *((int *) k) - ((struct dep_ *) d)->nro;
}

void depto_(int);
void depto(int cual)
{
	GCStatic::LockMainLoop("depto");
	depto_(cual);
	GCStatic::ReleaseLockMainLoop("depto");
}

void depto_(int cual)
{
	 dep_ *d;
	 int uCant;
	 double pxReg;
	 
	 if(consmode) return;
	 if(chkClk()) return;
	 if(chkItemCount()) return;
	 pSubtot = 0;
	 d = (dep_ *) bsearch(&cual, deptos, cDep, sizeof(struct dep_), depcmp2);
	 if(!d || inOper > 1 || globalCobraCC)  // Valida que sea posible
	 {
		Alert("No puede utilizar esta tecla ahora",
			  "Termine la cobranza primero");
		return;
	 }
	 if(rMode)
	 {
		 pxReg = xReg;
		 xReg = 0.0;
		 superv(0);
		 xReg = pxReg;
	 }

	 /*   Procesa Repeticion  */

	 if(xReg == 0.0)
	 {
		if(dRepeat)
		 {
			xReg = LastDep;
			yReg = 0.0;
		 }
		 else
		 {
			Alert("Ingrese un importe ANTES de",
				  "Pulsar la tecla de Departamento");
			return;
		 }
	 }

	 /*   Procesa segun los datos ingresados.   */

	 if(yReg == 0.0)
	 {
		yReg = 1;
		uCant = 0;
		LastDep = xReg;
		dRepeat = 1;
	 }
	 else
	 {
		uCant = 1;
		LastDep = 0.0;
		dRepeat = 0;
	 }

	 HDecim();

	 //  Calcula el total final y verifica si corresponde la operacion en cuentas chequeadas.
	 if(globalCCOnline && cc_HastaSaldo)
	 {
		double saldo = atof_(c_saldo);
		double salmax = atof_(c_salmax);
		if(salmax - saldo < xTotal.VerVenta().ToDouble() + (yReg * xReg))
		{
			Alert("Con la ultima registracion SUPERA el monto de credito",
				  "disponible. Cierre el ticket por favor...");
			return;
		}
	 } 
	 if(!ValidarRegistracion(yReg, xReg))
		 return;

	 //   Imprime en los visores.
	 if(uCant)
	 {
		  if(ceil(yReg) == yReg)
				sprintf(msg, "%6.0lf X ", yReg);
		   else
				sprintf(msg, "%6.3lf X ", yReg);
		  Acomoda(msg1, msg, HRng(xReg));
	 }
	 else
		  STRCPY(msg1, "");

	 CustDisp3(d->des, msg1, HRng(xReg * yReg));
	 DispCash(xReg * yReg, cDecim);
	 Acomoda(msg2, d->des, HRng(xReg * yReg));
	 HClear();

	 /*   Actualiza los calculos   */
	 dep.func = DDepto;				// Id de la operacion.
	 dep.nro = d->nro;				// Nro de Departamento.
	 store(yReg, dep.yReg);
	 store(xReg, dep.xReg);
	 dep.uCant = uCant;
	 dep.iva = d->iva;
	 dep.cajero = ckaNro;
	 pRepeat = 0;
	 dep.fecha = (long) date();
	 dep.hora = (long) time_();
	 ProcDepto(&dep, 1);			// Procesa la venta
	 yReg = 0.0;
	 PrintTktTot();
	 CheckPriceOverflow();
}

void ProcDepto(void *dep_, int fwrite)
{
	double x, y;
    struct DDdep_ *dep;

    dep = (struct DDdep_ *) dep_;
    if(fwrite) WriteDump(dep);
	x = atof_(dep->xReg);
	y = atof_(dep->yReg);
	if(Inicializando || globalCorri || fwrite)
	{
		xTotal.AddVenta(BDecimal(x * y));
		xCnt += y;
		CalcIva(x * y, dep->iva, 1);
	}
	if(fwrite || globalCorri) PrintDepto(dep);
	if(globalCorri) GlobalRngCount--;
	else 			GlobalRngCount++;
	inOper = 1;
}

//   Genera la informacion para la refacturacion.
void MakeDepFactu(void *depx_, char *g_Codigo, char *g_Descrip,
				  double *g_Cantidad,double *g_UnitarioN, double *g_UnitarioT)
{
	dep_ *d;
    struct DDdep_ *dep;

    dep = (struct DDdep_ *) depx_;
    d = (dep_ *) bsearch(&dep->nro, deptos, cDep, sizeof(struct dep_), depcmp2);

	if(!d)
	{
		g_Codigo[0] = g_Descrip[0] = 0;
		*g_Cantidad = *g_UnitarioT = *g_UnitarioN = 0;
		return;
	}
	
	//   Codigo normal
	STRCPY(g_Descrip, d->des);
	sprintf(g_Codigo, "%-6d", d->nro);
	*g_Cantidad = atof_(dep->yReg);
	*g_UnitarioT = atof_(dep->xReg);
	*g_UnitarioN = *g_UnitarioT / ivadisc(dep->iva);
}
 
/*
 *   Imprime una registraci¢n de un departamento.
*/
void PrintDepto(void *depx_)
{
	double x, y;
	dep_ *d;
    struct DDdep_ *dep;
	char calific;
	char adicdes[30];

    dep = (struct DDdep_ *) depx_;
    d = (dep_ *) bsearch(&dep->nro, deptos, cDep, sizeof(struct dep_), depcmp2);
	x = atof_(dep->xReg);
	y = atof_(dep->yReg);
	if(dep->uCant)
	{
		if(ceil(yReg) == yReg)
			sprintf(msg, "%6.0lf X ", y);
		else
			sprintf(msg, "%6.3lf X ", y);
		DispDeta(msg, ml(HRng(x)));
	}
	DispDeta(d->des, HRng(x * y));

	if(x < 0.0)
	{
		calific = 'm';
		x = x * (-1.0);
	}
	else
		calific = 'M';
	if((prndelay && !globalCorri && globalDelayPrint) || (!prndelay && !globalCorri))
	{
		sprintf(msg1, "Dep:%d", dep->nro);
		sprintf(msg, "%-40s", d->des);
		WriteTickTmp(msg1, msg, (double) y, (double) x, 0, (double)(x * y), 1);
	}

	double pUnitario = x / y;
	if(globalDelayPrint) return;
	sprintf(adicdes, "Dep:%d", dep->nro);
	if(calific == 'R')
		pf->PrintDescto(d->des, (double) y * (double) x);
	else
		pf->PrintItem(
				adicdes,							// Descripcion
				(double)(y),						// Cantidad
				(double)(x * y),					// Monto Total
				ivatasa(dep->iva-1),				// Tasa de Iva
				calific,							// Calificador
				(int)(dep->uCant? y + 0.0001: 1),   // Bultos
				0,									// Impuestos Internos
				d->des,								// Descripcion adicional
				pUnitario,							// El precio unitario del producto vendido 
				dep->uCant,							// Si debe forzar la impresion de la cantidad
				0									// La cantidad de unidades en la caja (ninguna siempre)	
				);
}

/*
 *  A efectos de la generaci¢n de la Zeta, retorna MONTO y CANTIDAD,
 *  de la venta efectuada a un departamento.
*/
void ValuesDepto(void *depx_,
				 int *_cual, char **_des, double *_cant, double *_monto)
{
	dep_ *d;
    struct DDdep_ *dep;

    dep = (struct DDdep_ *) depx_;
	d = (dep_ *) bsearch(&dep->nro, deptos, cDep, sizeof(struct dep_), depcmp2);
	*_cual  = dep->nro;
	*_cant  = atof_(dep->yReg);
	*_monto = atof_(dep->xReg);
	if(!d)
		*_des = "<Depto Indefinido>";
	else
		*_des = d->des;
}

/*
 *  Retorna la descripci¢n del departamento.
*/
char *GetDeptoDes(int nro)
{
	dep_ *d;

	d = (dep_ *) bsearch(&nro, deptos, cDep, sizeof(struct dep_), depcmp2);
	if(!d)
		return "<Depto Indefinido>";
	else
		return d->des;
}

//   Carga, desde el archivo 'deptos.def', la definicion de los Departa-
//   mentos.
//
int depcmp1(const void *d1, const void *d2)
{
    return ((struct dep_ *) d1)->nro - ((struct dep_ *) d2)->nro;
}

int LoadDep()
{
	 struct dep_ *dep;
	 char cod[4], des[25], habil[2], iva[4];

	 cDep = 0;
	 if(dbdep.Use("pos\\deptos"))
	 {
		 deptos = new dep_[dbdep.RecCount()];
		 dbdep.Go(DB_GO_TOP);
		 while(!dbdep.Eof())
		 {
			 dbdep.Get("codigo", cod);
			 dbdep.Get("des", des);
			 dbdep.Get("habilitado", habil);
			 dbdep.Get("iva", iva); //dbdep.Get(EsRI ? "iva" : "ivari", iva);
			 dep = &deptos[cDep++];
			 dep->nro = atoi(cod);
			 dep->flags.habilitado = (habil[0] == 'F')? 0: 1;
			 dep->iva = atoi(iva);
			 STRCPY(dep->des, Trim(des));
			 dbdep.Skip(1);
		 }
		 dbdep.Close();
	 }
	 else
		 AbnormalEnd("No puedo cargar los departamentos");
	 if(cDep)
		qsort(deptos, cDep, sizeof(struct dep_), depcmp1);
     return 1;
}

//   Funcion de 'validacion' del codigo de depto encontrado.
int buscaDep(char *cual, void *bag)
{
	static char uCod[10];
	int si;

	Padl(Trim(STRCPY(uCod, cual)), 2, ' ');
	if(dbdep.Locate("codigo", uCod))
	{
		depWin->ShowGets();
		return 1;
	}
	sprintf(msg, "No encuentro el departamento '%s'.", cual);
	si = GetSiNo(msg, "Lo incorporo como departamento nuevo?");
	if(!si)
		return 0;
	dbdep.AppendBlank();
	dbdep.Replace("codigo", uCod);
	depWin->ShowGets();
	return 1;
}

//   Abre una ventana y permite editar el archivo de departamentos.
void PrograDeptos()
{
	int hWin;

	dbdep.Use("pos\\deptos");
	hWin = wDefine(7, 18, 15, 62, BORDERSINGLE, SNGCURS, prograEsq, "Programacion de Departamentos", "");
	depWin = new EditWindow(hWin);

	wActivate(hWin);
	wLocate(1, 2);  wPstr("Numero:");
	wLocate(2, 2);  wPstr("Nombre:");
	wLocate(3, 2);  wPstr("Habil.:");
	wLocate(4, 2);  wPstr("Iva ..:");
	wLocate(4, 13); wPstr("0=Exento, 1...=Gravado");

	depWin->AddGet(1, 10, NUMBER_RIGHT_ALIGNED,  2, 0, uCod, NULL, NULL, buscaDep);
	depWin->AddGet(2, 10, "des",        &dbdep, NULL, NULL, NULL);
	depWin->AddGet(3, 10, "habilitado", &dbdep, NULL, NULL, NULL);
	depWin->AddGet(4, 10, "iva",        &dbdep, NULL, NULL, NULL);//depWin->AddGet(4, 10, EsRI ? "ivari" : "iva",        &dbdep, NULL, NULL, NULL);

	depWin->AnimaGets(1);
	wRelease(hWin);
	delete depWin; depWin = NULL;
	dbdep.Close();
    
	FREE(deptos); LoadDep();
	HClear();
}	
