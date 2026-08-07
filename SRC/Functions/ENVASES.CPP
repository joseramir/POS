//
//   Envases.cpp
//   Manejo de los envases automaticos.
//

#include <stdafx.h>
#include "funcs.h"

char **EnvasLogo = (char **)new char*[5];
char **EnvasPie = (char **)new char*[5];
char ENVFN[] = "envases.dat";
int EnvasH, cEnvas;
struct env_ envase;
struct DDEnvas_ denv;
Choose *EnvH = NULL;
::ArrayList *envases = NULL;
bool envFormCancel = false;

// Referencias externas
extern char *usacod, *usades;

// Funcion que explora el ticket actual buscando articulos que tengan envase y armando la lista global
// 'envases' con los codigos/cantidad de envases que estan pendientes de ser registrados.
bool BuscarEnvases()
{
	int func, codEnvase, i, envCant, posi, cod;
	double cant;
	char tBuff[100];

	// Prepara la lista que contendra el resultado.
	if(envases == NULL) envases = new ::ArrayList(false, "BuscarEnvases");
	envases->Clear();

	// Explora el TRANS buscando transacciones con envases.
	GCStatic::AddLockToTrans("envases");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", tBuff);
		func = atoi(tBuff);
		tDbf.Get("anulado", tBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T')
		{
			if(func == DPlu)
			{
				tDbf.Get("envase", tBuff);
				codEnvase = atoi(tBuff);
				tDbf.Get("cant", tBuff);
				cant = atof(tBuff);
				if(codEnvase != 0)
				{
					// El articulo TIENE ENVASE ENGANCHADO. Contabiliza entonces los envases a controlar.
					posi = envases->FindIndex(codEnvase);
					if(posi < 0)
						posi = envases->Add(codEnvase, (void *)0);
					envCant = (int)envases->Item(posi);
					envCant += (int)cant;
					envases->SetItem(posi, (void *)envCant);
				}
				else
				{
					// El renglon ES LA VENTA del envase?
					tDbf.Get("cod", tBuff);
					cod = atoi(tBuff);
					posi = envases->FindIndex(cod);
					if(posi >= 0)
					{
						envCant = (int)envases->Item(posi);
						envCant -= (int)cant;
						envases->SetItem(posi, (void *)envCant);
					}
				}
			}
			else if(func == DEnvas)
			{
				tDbf.Get("flag", tBuff);
				if(tBuff[0] != 'T')				// Es operacion de 'perdonado' de envase
				{
					tDbf.Get("cod", tBuff);
					codEnvase = atoi(tBuff);
					tDbf.Get("cant", tBuff);
					cant = atof(tBuff);
					posi = envases->FindIndex(codEnvase);
					if(posi < 0)
						posi = envases->Add(codEnvase, (void *)0);
					envCant = (int)envases->Item(posi);
					envCant -= (int)cant;
					envases->SetItem(posi, (void *)envCant);
				}
			}
		}
	}
	GCStatic::ReleaseLockFromTrans("envases");

	// Elimina entradas en la lista que contengan solamente renglones en cero.
	for(i = envases->Count() - 1; i >= 0; i--)
	{
		envCant = (int) envases->Item(i);
		if(envCant <= 0)
		{
			envases->RemoveAt(i);
			i = envases->Count();		// Volvemos a empezar el examen de la lista... 
		}
	}
		
	// Retorna true si hay envases
	return (envases->Count() != 0);
}

//   Inicializa la tabla de envases.   
int InitEnvases()
{
    return 1;
}

//   Asienta el ticketeo de envases.
void ProcEnv(void *p_, int fwrite)
{
    struct DDEnvas_ *p;

    p = (struct DDEnvas_ *) p_;
	if(fwrite) WriteDump(p);
}

// Imprime lo relacionado con la registracion de envases.
void PrintEnvase(void *data)
{
	char tCod[10], tdes[100], pludes[100];

	struct DDEnvas_ *env = (struct DDEnvas_ *) data;
	if(env->flag != 0) return;

	sprintf(tCod, "%d", env->cod);
	dbplu.Setorder("mplucod");
	if(!dbplu.Seek(padl((double)env->cod, 6))) return;
	dbplu.Get("des", pludes);
	sprintf(tdes, "TKT ->> %s", pludes);

	WriteTickTmp(tCod, tdes, (double)env->cant, 1, 0, 0, 0);
}

//   Procesa la pulsacion de ENTER dentro de la ventana de seleccion del envase.
int EnvFunc(int cual)
{
	char *bText, *p, tMsg[100];
	int cant;

	cual--;									// Cual comienza en '1'
	bText = EnvH->GetText(cual);
	if(bText[0] == '*') return 1;			// Opcion de SALIR - Retorna con 1.
	p = strchr(bText, '[') + 1;
	cant = atoi(p + 6) - 1;
	if(cant < 0) cant =0;
	*p = 0;
	
	sprintf(tMsg, "%sCobrar %d]", bText, cant);
	if(cant == 0)
		EnvH->SetOption(cual, tMsg, false);
	else
		EnvH->SetOption(cual, tMsg, true);
	return 0;
}

//   Registra los tickets de envase que entrega el cliente.
void envas(int cual)
{
	int i, envCod, envCant;
	EditItem *items[20];

	// Mira si hay envases que se puedan procesar.
	envFormCancel = false;
	if(cual != 9998) BuscarEnvases();
	if(envases->Count() == 0) return;

	// Construye la form para editar los envases a cobrar.
	GenForm *gf = new GenForm("Envases a DESCONTARLE al Cliente");
	gf->CambiarFontSize(FONT_CHICO);
	dbplu.Setorder("mplucod");
	for(i = envases->Count() - 1; i >= 0; i--)
	{
		envCod = envases->Key(i);
		envCant = (int)envases->Item(i);
		if(envCant != 0)
		{
			if(!dbplu.Seek(padl((double)envCod, 6))) continue;
			dbplu.Get("des", usades);
			Trim(usades);

			EditItem *ei = new EditItem(usades);
			items[i] = ei;

			ei->CambiarTipo(TIPO_ENTERO);
			sprintf(tmsg_, "%d ** %s - (Hay registrado%s %d Vacio%s)", 
				envCod, 
				usades, 
				((envCant <= 1)? "": "s"),
				envCant,
				((envCant <= 1)? "": "s")
			);
			STRCPY(ei->help, tmsg_);
			gf->Items()->Add(ei);
		}
	}

	//  Anima la form y procesa en consecuencia.
	if(gf->Animar())
	{
		for(i = envases->Count() - 1; i >= 0; i--)
		{
			envCod = envases->Key(i);
			EditItem *ei = items[i];
			envCant = atoi(ei->dato);
			if(envCant > (int)envases->Item(i)) 
				envCant = (int)envases->Item(i);

			if(envCant > 0)
			{
				denv.func = DEnvas;
				denv.cod = envCod;
				denv.cant = envCant;
				denv.fecha = date();
				denv.hora = time__();
				denv.cajero = ckaNro;
				denv.flag = 0;					// Indica que es devuelto.
				ProcEnv(&denv, 1);
				PrintEnvase(&denv);
			}
		}
	}
	else
		envFormCancel = true;
}

//   Vende los envases que no se devolvieron.
void VenderEnvases()
{
	int i, j, pheader;

	pheader = 0;
	BuscarEnvases();
	for(i = envases->Count() - 1; i >= 0; i--)
	{
		// Obtiene la informacion
		int envCod = envases->Key(i);
		int envCant = (int)envases->Item(i);
		if(envCant != 0)
		{
			// Imprime un encabezamiento de la seccion de envases en el ticket.
			//if(!pheader)
			//{
			//	// Encabezamiento configurable.
			//	//Comentado para Nueva Impresora PR 250
			//	/*pf->PrintText("");
			//	pf->PrintText("");
			//	pf->PrintText("");
			//	pf->PrintText("");*/
			//	WriteTickTmp("", "**************** ENVASES ***************", 0, 0, 0, 0, 0);
			//	pheader = 1;
			//}
			pheader = 1;

			// Efectua la venta.
			xReg = (double) envCod;
			yReg = (double) envCant;
			plu(0);

			//  Mete un registro en el trans que describe la operacion de envases efectuada.
			denv.func = DEnvas;
			denv.cod = envCod;
			denv.cant = envCant;
			denv.fecha = date();
			denv.hora = time__();
			denv.cajero = ckaNro;
			denv.flag = 1;			// Indica que es cobrado.
			ProcEnv(&denv, 1);
		}
	}

	// Si imprimio encabezamiento, entonces tambien imprime un pie configurable.
	if(pheader)
	{
		for(j = 0; j < EnvasPieCnt; j++)
		{
			DispDeta(EnvasPie[j], "");
			//Comentado para Nueva Impresora PR 250
			//pf->PrintText(EnvasPie[j]);
		}
	}

	// Todos los envases fueron vendidos...
	envases->Clear();
}

//   La siguiente funcion es llamada al cerrar el ticket.
//   Analiza los envases pendientes y, si los hay, solicita a la cajera alguna accion.
//   De quedar envases pendientes, los imprime y fuerza la impresion del ticket.
int CheckEnvases()
{
	double p_xReg = xReg, p_yReg = yReg;

	// Mira si hay envases pendientes.
	if(!BuscarEnvases()) return 0;

	// Llama a la pantalla de confirmacion de envases. El parametro (9998) evita que se escanee
	// de nuevo el ticket buscando envases.
	envFormCancel = false;
	envas(9998);

	// Vende los envases que esten pendientes de ser registrados.
	if(!envFormCancel)
		VenderEnvases();

	// Recupera los registros del POS y retorna.
	xReg = p_xReg; 
	yReg = p_yReg;
	return envFormCancel? 1: 0;
}

