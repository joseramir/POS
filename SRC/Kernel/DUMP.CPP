//
//   Dump.c
//   Escribe en el archivo trans.dbf.
//

#include "stdafx.h"

#define IN_DUMP
#include "funcs.h"
#include <vector>
#include <utility>
using namespace System::Collections::Generic;
//#include "iostream"

//  Referencias externas
extern int maxMpluPrec;						// plu.cpp

char *p_Log;								// PATH para log remoto.
int wlog;
char FILE1[] = "trans.dbf", 
     FILE2[] = "transctl.dbf", 
	 FILE_TMP[] = "ticket.dbf",
	 FILE4[] = "tanul.dbf";

// Modo en el que graba el log remoto.
dbf tDbf, tCtl, /*tTmp,*/ tMonitor;				// Controladores
dbf tAnul;									//para almacenar los tickets anulados, por reinicio
char *tdes_ = new char[1500];				// Buffer temporario
int globalInitDump;							// Indica que está inicializando.
dbf rPlu;									// Para report de venta de plus.
extern dbf dbplu;
int talon;									// Tipo de talon a imprimir despues de cada ticket.
int ponline;								// Activa o no el procesamiento online de transacciones.
double limTicket, limCantidad, limRenglon;	// Montos maximos configurables.
double limConsFinal;
bool zapearTicketTemp = false;

#ifdef POSXP
#include "StaticDump.h"
#include "MainForm.h"
#else
xBrowse bDet;								// Estructura del browse.
#endif

//   String para crear las distintas bases de control y al 'trans.dbf'
char *crStr = 
"tienda c(6), caja n(3), func n(3), nprec n(2), mpag n(3), cajero n(4), imp c(13),\
	teorico c(13), nro n(11), flag L, uso_cant L, iva n(3), cant c(13),\
	que_anula n(4), posi n(6),cod n(14), uso_scan L, pesado L,\
	envase n(14), millas c(13), otrop n(3), u_vuelto L, u_pend L,\
	vuelto c(13), codcli c(12), numero c(13), fecha_c c(11),\
	nombre c(30), dni c(13), telefono c(12), direcc c(30),\
	barrio c(30), condiva c(2), local c(30), \
	tipo n(2), ticket n(8), zeta n(6),\
	fecha_z n(10), modo n(4), anulado L, fecha n(8), hora n(6), cnlvta c(3), impiva c(12)";
	//fecha_z n(10), modo n(4), anulado L, fecha n(8), hora n(6), caja n(3), cnlvta c(3), impiva c(7)";
//	caja n(3), cnlvta c(3)";//sproc n(1}";


char *crStrCtl = "act_trans n(8), l_sync n(8)";
char *crTick = "codigo c(7), nombre c(40), cant c(7), unitario c(8), total c(9), dump n(8), anulable L";
char *crPlu = "cod c(6), dep n(4), des c(40), cant n(16,3), imp n(16,2)";
char *crAnula = "cajero n(4), ticket n(7), imp c(13)";

// Mensaje a mostrar al cerrar el ticket. Usar strdup para copiar el mensaje aqui porque, despues de
// borrado, el programa elimina el mensaje con 'FREE'.
char *showMessage = NULL;

//   Variables de control de acceso al archivo local.
unsigned long ActualTrans, SyncTrans;
unsigned long DelCtrlTrans;

//  Prototipos de este programa.
void ReprocActualTran(void);
void PrintActualTran(void);
void ResetPOSAcum(void);
void ResetPOSAcumInternal(void);

int enviaTransSql(int);
void MostrarMayorEnPantalla();//en Factu.cpp esta el metodo

// Valida que una registracion no vaya a superar los limites permitidos por el sistema.
bool ValidarRegistracion(double cant, double monto)
{
	double oldTot = xTotal.VerVenta().ToDouble();
	double xtot = oldTot + (cant * monto);
	char msg1[100], msg2[100];
	if(xtot < 0)
	{
		STRCPY(msg1, "El ticket no puede hacerse negativo");
		STRCPY(msg2, "");
	}
	else if(fabs(cant) > limCantidad)
	{
		sprintf(msg2, "%.2lf Unidades", limCantidad);
		STRCPY(msg1, "La cantidad de items por renglon no puede superar");
	}
	else if(fabs(cant * monto) > limRenglon)
	{
		sprintf(msg2, "%.2lf Pesos", limRenglon);
		STRCPY(msg1, "Ningun renglon del ticket puede superar");
	}
	/*else if((cant + xCnt) > limCantxTicket)
	{
		sprintf(msg2, "%.2lf Unidades", limCantxTicket);
		STRCPY(msg1, "La cantidad de articulos del ticket no puede superar");
	}*/
	else if(limConsFinal < limTicket && xtot > limConsFinal && !GlobalTF && (strcmp(c_condiva, "CF") == 0 || strcmp(c_condiva, "") == 0))
	{
		sprintf(msg1, "El ticket Consumidor Final no puede superar %.2lf pesos", limConsFinal);
		STRCPY(msg2, "");
	}
	else if(xtot > limTicket)
	{
		if (puede_pasar)
		{
			if (!elige_pasar)
				elige_pasar = (bool)GetSiNo("El ticket supera monto fiscal.", "Cuando termine sera enviado a Cobradora. Confirma?", false);
			if (elige_pasar)
				return true;
			else
			{
				sprintf(msg1, "El ticket no puede superar %.2lf pesos", limTicket);
				STRCPY(msg2, "");
			}
		}
		else
		{		
			sprintf(msg1, "El ticket no puede superar %.2lf pesos", limTicket);
			STRCPY(msg2, "");
		}
	}

	else return true;

	// Falla la validacion...
	WLog("FALLO EN LIMITES");
	WLog("    Nuevo Renglon. Cant=%.3lf, Monto=%.3lf", cant, monto);
	WLog("    Nuevo Total. TotalVenta=%.3lf, Acumulado=%.3lf", oldTot, xtot);
	WLog("    Limites Configurados. limCantidad=%.3lf, limRenglon=%.3lf, limTicket=%.3lf, limConsFinal=%.3lf", limCantidad, limRenglon, limTicket, limConsFinal);
	WLog("    Mensaje1='%s'", msg1);
	WLog("    Mensaje2='%s'", msg2);

	Alert(msg1, msg2);
	return false;
}

//  Para el caso de una anulacion total de ticket, la siguiente funcion trunca (elimina) del 
//  archivo de dump, la transaccion que estaba en progreso.
void TruncTrans()
{
	GCStatic::AddLockToTrans("TruncTrans");
	if(!ActualTrans)
		tDbf.Zap();
	else
	{
		//tDbf.Trunc(ActualTrans-1);
		unsigned long idrecno = tDbf.Recno();
		int auxcont = 0;
		char *auxfunc = new char[16];
		do 
		{
			tDbf.Go(ActualTrans - auxcont);			
			if (!tDbf.Bof())
			{			
				tDbf.Get("func", auxfunc);	
				if (atoi(auxfunc) == 3)
					break;
				auxcont++;
			}
		} while (!tDbf.Bof());
		tDbf.Go(idrecno);
		tDbf.Trunc(ActualTrans - auxcont);
		//tDbf.Trunc((ActualTrans == DelCtrlTrans ? ActualTrans : ActualTrans-1));
	}
	GCStatic::ReleaseLockFromTrans("TruncTrans");

	FlushXPBuffers();
}

// Optimiza el codigo
//  A partir del contenido de las tablas, decodifica el valor de una estructura dump.
void trDecode(union Dump_ *d)
{
	struct dDbf_ *xd;
	struct dFld_ *xf;
	int i;
	char *p;

	GCStatic::AddLockToTrans("trDecode");
	tDbf.Get("func", tdes_);
	d->f.id = atoi(tdes_);
	for(i = sizeof(xxDump) / sizeof(dDbf_ *); i; i--)
	{
		xd = xxDump[i-1];
		if(xd->func == d->f.id)
		{
			p = ((char *) d) + 4;	// Saltea el ID.
			xf = &xd->dFld[0];
			while(xf->ancho)
			{
				if(!tDbf.Get(xf->fName, tdes_))
				{
					sprintf(tdes_, "Error %d Interno: GET trans.%s'.\n", view.dberr, xf->fName);
					GCStatic::ReleaseLockFromTrans("trDecode");
					AbnormalEnd(tdes_);
				}
				switch(xf->type)
				{
					case 'L':		// Dato tipo UNSIGNED LONG.
						*((unsigned long *) p) = atol(tdes_);
						break;

					case 'I':		// Dato tipo INT.
						*((int *) p) = atoi(tdes_);
						break;
					
					case 'T':		// Dato tipo CHAR. (BOOL)
						*p = (tdes_[0]  == 'T')? 1: 0;
						break;
					
					case 'C':		// Dato tipo STR. (CHAR *)
						Trim(tdes_);
						if(strlen(tdes_) > (size_t)(xf->ancho - 1))
							tdes_[xf->ancho - 1] = 0;
						STRCPY(p, tdes_);
						break;

					case 'X':		// Dato tipo UNSIGNED LONG.
						*((long long *) p) = _atoi64(tdes_);
						break;

					default:		// Dato no previsto!!!
						sprintf(tdes_, "Error Interno: 'TrDecode: Tipo '%c' no programado'.\n", xf->type);
						GCStatic::ReleaseLockFromTrans("trDecode");
						AbnormalEnd(tdes_);
				}
				p += xf->ancho;
				xf++;
			}
			break;
		}
	}
	if(!i)
	{
		d->f.id =0;
		tDbf.Delete();
	}
	GCStatic::ReleaseLockFromTrans("trDecode");
}

//  A partir del contenido de una estructura Dump_, codifica los datos en la tabla.
void trEncode(union Dump_ *d)
{
	struct dDbf_ *xd;
	struct dFld_ *xf;
	int i;
	char *p;
	char fhdata[10];

	GCStatic::AddLockToTrans("trEncode");
	for(i = sizeof(xxDump) / sizeof(dDbf_ *); i; i--)
	{
		xd = xxDump[i-1];
		if(xd->func == d->f.id)
		{
			_itoa(d->f.id, tdes_, 10);
			tDbf.Replace("func", tdes_);
			p = ((char *) d) + 4;	// Saltea el ID.
			xf = &xd->dFld[0];
			while(xf->ancho)
			{
				switch(xf->type)
				{
					case 'L':		// Dato tipo UNSIGNED LONG.
						_ltoa( *((unsigned long *) p), tdes_, 10);
						break;

					case 'I':		// Dato tipo INT.
						_itoa( *((int *) p), tdes_, 10);
						break;
					
					case 'T':		// Dato tipo CHAR. (BOOL)
						tdes_[0] = *p? 'T': 'F';
						tdes_[1] = 0;
						break;
					
					case 'C':		// Dato tipo STR. (CHAR *)
						Trim(p);
						STRCPY(tdes_, p);						
						break;

					case 'X':		// Dato tipo UNSIGNED LONG.
						_i64toa( *((long long *) p), tdes_, 10);
						break;

					default:		// Dato no previsto!!!
						sprintf(tdes_, "Error Interno: 'TrEncode: Tipo '%c' no programado'.\n", xf->type);
						GCStatic::ReleaseLockFromTrans("trEncode");
						AbnormalEnd(tdes_);
				}
				if(!tDbf.Replace(xf->fName, tdes_))
				{
					sprintf(tdes_, "Fallo (%d) interno: REPLACE trans.%s WITH '%s'.\n", view.dberr, xf->fName, tdes_);
					AbnormalEnd(tdes_);
				}
				p += xf->ancho;
				xf++;
			}
			break;
		}
	}	
	tDbf.Replace("tienda", saptienda);
	tDbf.Replace("cnlvta", sapcnlvta);
	sprintf(fhdata, "%d", date());
	tDbf.Replace("fecha", fhdata);
	sprintf(fhdata, "%d", time_());
	tDbf.Replace("hora", fhdata);
	_itoa(caja, tdes_, 10);
	tDbf.Replace("caja", tdes_);

	GCStatic::ReleaseLockFromTrans("trEncode");
	if(!i)
	{
		sprintf(tdes_, "Error Interno: 'TrEncode= Funcion %d no encontrada en tabla'.\n", d->f.id);
		AbnormalEnd(tdes_);
	}
}

//  Retorna el nro de registro actual que se esta escribiendo.
unsigned long trLastSeek()
{
	GCStatic::AddLockToTrans("TrLastSeek");
	long res = tDbf.RecCount();
	GCStatic::ReleaseLockFromTrans("TrLastSeek");
	return res;
}

//  La siguiente funcion escribe en los archivos de dumping, a efectos
//  de llevar un detalle de las operaciones confeccionadas.
void WriteDump(void *d)
{
	GCStatic::AddLockToTrans("WriteDump");
	tDbf.AppendBlank();
	trEncode((union Dump_ *) d);
	tDbf.Flush();
	GCStatic::ReleaseLockFromTrans("WriteDump");
	FlushXPBuffers();
    lastact = time__();
}

//   La siguiente funcion se utiliza para leer un renglon del dump, cuyo offset se pasa como 
//   parametro. Para devolver el resultado, utiliza una 'union Dump_' estatica cuyo
//   puntero se devuelve como 'void *'.
void *ReadDump(long donde)
{
	static union Dump_ ed;

	GCStatic::AddLockToTrans("ReadDump");
	tDbf.Go(donde);
	trDecode(&ed);
	GCStatic::ReleaseLockFromTrans("ReadDump");
	return &ed;
}

//   Al ser requerido desde la red, la siguiente funcion deja vacio el archivo de dumping.
void OpenTransFiles(void);
void ResetRemoteDump(void);
void ZeroDump()
{
	char t1[40], t2[40];
	int i;

	GCStatic::AddLockToTrans("ZeroDump");
    WriteBootLog("Cerrando TDBF");
	tDbf.Close();			// Cierra los archivos de transacciones.

    WriteBootLog("Cerrando TCTL");
	tCtl.Close();

    WriteBootLog("Eliminando 45");
	remove("transbak.45");
	for(i = 44; i > 0; i--)
	{
        WriteBootLog("Renombrando...");
		sprintf(t1, "transbak.%d", i + 1);
		sprintf(t2, "transbak.%d", i);		
		rename(t2, t1);
	}

    WriteBootLog("Renombrando TRANS.DBF a TRANS.001");
	rename(FILE1, t2);

    WriteBootLog("Eliminando TRANSCTL.DBF");
	remove(FILE2);	

    WriteBootLog("Ejecutando INITDUMP");
    OpenTransFiles();             // Lo crea nuevamente.

	WriteBootLog("Eliminado GAVETA.DBF");
	remove("gaveta.dbf");

	WriteBootLog("Ejecutando RESETPOSACUM");
	ResetPOSAcum();

	WriteBootLog("Eliminando DUMP Remoto");
	ResetRemoteDump();

	WriteBootLog("ZeroDump FINALIZADO");
	GCStatic::ReleaseLockFromTrans("ZeroDump");

	FlushXPBuffers();
}

//   Mueve el contenido del Dump para el ticket actual, al archivo que se le indica como parametro.
//   Si 'tickNro' es -1, el ticket que se mueve es el actual.
void MoveTicket(char *fn, int tickNro)
{
	static dbf mvTick;
	char tBuff[2], fBuff[10], tkBuff[30], tipoBuff[10];	
	int nrecno;

	if(!mvTick.Use(fn))
		if(!mvTick.creaTable(fn, crStr))
			return;
	mvTick.Zap();

	// Va al comienzo del ticket actual.
	GCStatic::AddLockToTrans("MoveTicket");
	tDbf.Go(ActualTrans);

	bool found = false;
	for(; !tDbf.Bof(); tDbf.Skip(-1))
	{
		tDbf.Get("anulado", tBuff);
		tDbf.Get("func", fBuff);
		tDbf.Get("ticket", tkBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T' && atoi(fBuff) != 6)
		{
			if(atoi(fBuff) == 3 && ((tickNro == -1)? true: (atoi(tkBuff) == tickNro)))
			{
				found = true;
				break;
			}
		}
	}

	// Si encontro el ticket pedido, lo suspende.
	if(found)
	{
		if(tickNro == -1)		
			tDbf.Skip(1);
		else
		{
			// Si se pide un ticket especifico, entonces ahora estamos AL FINAL del ticket. Caminamos hasta el comienzo...
			tDbf.Skip(-1);
			for(; !tDbf.Bof(); tDbf.Skip(-1))
			{
				tDbf.Get("anulado", tBuff);
				tDbf.Get("func", fBuff);
				if(atoi(fBuff) == 3 && tBuff[0] != 'T')
				{
					tDbf.Skip(1);
					break;
				}
			}
		}		
		nrecno = (int) tDbf.Recno() - 1;
		// Copia el contenido del ticket que nos interesa.
		for(; !tDbf.Eof(); tDbf.Skip(1))
		{
			tDbf.Get("func", fBuff);
			int func = atoi(fBuff);
			tDbf.Get("tipo", tipoBuff);
			int tipo = atoi(tipoBuff);
			if(!tDbf.Deleted() && func != 23)
			{
				mvTick.AppendBlank();
				mvTick.InsertFrom(tDbf);				
				if(func == 6)
				{
					mvTick.Get("posi", tkBuff);
					int posi = atoi(tkBuff) - nrecno;// - ActualTrans;
					sprintf(tkBuff, "%d", posi);
					mvTick.Replace("posi", tkBuff);
				}
			}
			if(!tDbf.Deleted() && (func == 3 && tipo == 0))
				break;
		}
	}

	mvTick.Close();
	tDbf.Go(DB_GO_BOTTOM);
	GCStatic::ReleaseLockFromTrans("MoveTicket");
	FlushXPBuffers();
    lastact = time__();
}

//   Mueve el contenido del Dump para el ticket actual, al archivo que se le indica como parametro.
//   Si 'tickNro' es -1, el ticket que se mueve es el actual.
bool MoveTicketNC(char *fn, int tipoDoc, int ptoVenta, int tickNro, int fecha, char *impfact)
{
	static dbf mvTick;
	char tBuff[2], fBuff[10], tkBuff[30], tipoBuff[10], fechaBuff[10], pvBuff[14], impBuff[14];	
	int nrecno;
	dbf searchTick;
	char transName[20];
	char fhdata[10];

	if(!mvTick.Use(fn))
		if(!mvTick.creaTable(fn, crStr))
			return false;
	mvTick.Zap();

	// Va al comienzo del ticket actual.
	GCStatic::AddLockToTrans("MoveTicket");
	tDbf.Go(ActualTrans);

	bool found = false;
	for(; !tDbf.Bof(); tDbf.Skip(-1))
	{
		tDbf.Get("anulado", tBuff);
		tDbf.Get("func", fBuff);
		tDbf.Get("ticket", tkBuff);
		tDbf.Get("posi", tipoBuff);
		tDbf.Get("fecha", fechaBuff);
		tDbf.Get("cod", pvBuff);
		if(!tDbf.Deleted() && tBuff[0] != 'T' && atoi(fBuff) != 6)
		{
			if(atoi(fBuff) == 3 && atoi(tkBuff) == tickNro &&  atoi(tipoBuff) == tipoDoc && atoi(fechaBuff) == fecha && atoi(pvBuff) == ptoVenta)
			{
				found = true;
				tDbf.Get("imp", impBuff);
				STRCPY(impfact, impBuff);
				break;
			}
		}
	}
	// Si encontro el ticket pedido, lo suspende.
	if(found)
	{
		/*if(tickNro == -1)		
			tDbf.Skip(1);
		else
		{*/
		// Si se pide un ticket especifico, entonces ahora estamos AL FINAL del ticket. Caminamos hasta el comienzo...
		tDbf.Skip(-1);
		for(; !tDbf.Bof(); tDbf.Skip(-1))
		{
			tDbf.Get("anulado", tBuff);
			tDbf.Get("func", fBuff);
			if(atoi(fBuff) == 3 && tBuff[0] != 'T')
			{
				tDbf.Skip(1);
				break;
			}
		}
		//}		
		nrecno = (int) tDbf.Recno() - 1;
		// Copia el contenido del ticket que nos interesa.
		for(; !tDbf.Eof(); tDbf.Skip(1))
		{
			tDbf.Get("func", fBuff);
			int func = atoi(fBuff);
			tDbf.Get("tipo", tipoBuff);
			int tipo = atoi(tipoBuff);
			if(!tDbf.Deleted() && func != 23)
			{
				mvTick.AppendBlank();
				mvTick.InsertFrom(tDbf);				
				if(func == 6)
				{
					mvTick.Get("posi", tkBuff);
					int posi = atoi(tkBuff) - nrecno;// - ActualTrans;
					sprintf(tkBuff, "%d", posi);
					mvTick.Replace("posi", tkBuff);
				}
			}
			if(!tDbf.Deleted() && (func == 3 && tipo == 0))
				break;
		}
	}
	//si no lo encuentra en el TRANS Actual busca en ARchivos TRANSBAK anteriores hasta 9 dias o 9 archivos transbak
	else{
		for(int kk = 1; kk < 10; kk++){
			sprintf(transName, "transbak.%d", kk);
			if (searchTick.Use(transName)){
				for( ; !searchTick.Eof(); searchTick.Skip(1)){
					searchTick.Get("anulado", tBuff);
					searchTick.Get("func", fBuff);
					searchTick.Get("ticket", tkBuff);
					searchTick.Get("posi", tipoBuff);
					searchTick.Get("fecha", fechaBuff);
					searchTick.Get("cod", pvBuff);
					searchTick.Get("imp", impBuff);//para almacenar el importe
					if(!searchTick.Deleted() && tBuff[0] != 'T' && atoi(fBuff) != 6)
					{
						if(atoi(fBuff) == 3 && atoi(tkBuff) == tickNro &&  atoi(tipoBuff) == tipoDoc && atoi(fechaBuff) == fecha && atoi(pvBuff) == ptoVenta)
						{
							found = true;
							STRCPY(impfact, impBuff);
							break;
						}
					}
				}
				if (!found)
					searchTick.Close();
				else
					break;
			}			
		}
		if (found){				
			// Si se pide un ticket especifico, entonces ahora estamos AL FINAL del ticket. Caminamos hasta el comienzo...
			searchTick.Skip(-1);
			for(; !searchTick.Bof(); searchTick.Skip(-1))
			{
				searchTick.Get("anulado", tBuff);
				searchTick.Get("func", fBuff);
				if(atoi(fBuff) == 3 && tBuff[0] != 'T')
				{
					searchTick.Skip(1);
					break;
				}
			}

			nrecno = (int) searchTick.Recno() - 1;
			// Copia el contenido del ticket que nos interesa.
			for(; !searchTick.Eof(); searchTick.Skip(1))
			{
				searchTick.Get("func", fBuff);
				int func = atoi(fBuff);
				searchTick.Get("tipo", tipoBuff);
				int tipo = atoi(tipoBuff);
				if(!searchTick.Deleted() && func != 23)
				{
					mvTick.AppendBlank();
					mvTick.InsertFrom(searchTick);				
					if(func == 6)
					{
						mvTick.Get("posi", tkBuff);
						int posi = atoi(tkBuff) - nrecno;// - ActualTrans;
						sprintf(tkBuff, "%d", posi);
						mvTick.Replace("posi", tkBuff);
					}
					sprintf(fhdata, "%d", date());
					mvTick.Replace("fecha", fhdata);
					sprintf(fhdata, "%d", time_());
					mvTick.Replace("hora", fhdata);
				}
				if(!searchTick.Deleted() && (func == 3 && tipo == 0))
					break;
			}
			searchTick.Close();
		}

	}	

	mvTick.Close();
	tDbf.Go(DB_GO_BOTTOM);
	GCStatic::ReleaseLockFromTrans("MoveTicket");
	FlushXPBuffers();
	lastact = time__();
	return found;
}

//Verifica si ya tiene hecha la Nota de Credito
bool BuscaTicketNcHecho(char *nroTicketOriginal, bool checkUsado)
{
	dbf mvTick;
	char tBuff[2],tkBuff[30];	
	
	bool found = false;
	if(!mvTick.Use("ticknot.DBF"))
		return found;
	
	for( ; !mvTick.Eof(); mvTick.Skip(1)){
		mvTick.Get("nrotick", tkBuff);
		mvTick.Get("usado", tBuff);		
		if(!mvTick.Deleted() && strcmp(nroTicketOriginal, tkBuff) == 0)
		{
			found = true;
			if (checkUsado)
				found = tkBuff[0] == 'T';			
			break;			
		}
	}

	mvTick.Close();
	return found;
}


//   La siguiente funcion incorpora un archivo de DUMP al ticket actualmente en curso.
void IncorporeTicket(char *fn)
{
	static dbf mvTick;
	char tStr[50];
	int nrecno; //ultimo numero de registro

	if(!mvTick.Use(fn)) 
	{
		WriteBootLog("no puedo utilizar el archivo ya que esta ocupado");
		return;
	}
	Inicializando = 1;			// Fuerza calculo de totales.
	GCStatic::AddLockToTrans("IncorporeTicket");
	tDbf.Go(DB_GO_BOTTOM); //tiene que ir al final del ticket
	nrecno = (int) tDbf.Recno();
	for(mvTick.Go(DB_GO_TOP); !mvTick.Eof(); mvTick.Skip(1))
	{
		tDbf.AppendBlank();
		tDbf.InsertFrom(mvTick);

		tDbf.Get("func", tStr);
		if(atoi(tStr) == 6)
		{
			tDbf.Get("posi", tStr);
			int posi = atoi(tStr) + nrecno; // + ActualTrans;
			sprintf(tStr, "%d", posi);
			tDbf.Replace("posi", tStr);
		}

		ReprocActualTran();			// Recalcula el renglon.
		PrintActualTran();			// Reimprime el renglon.
	}
	Inicializando = 0;
	GCStatic::ReleaseLockFromTrans("IncorporeTicket");
	PrintTktTot();					// Actualiza el total en pantalla
	mvTick.Close();
	FlushXPBuffers();
	lastact = time__();
}

//   La siguiente funcion incorpora un archivo de DUMP al ticket actualmente en curso.
void IncorporeTicketNC(char *fn, int tipoComp, long NroComp, int fechaOrig, char *pcodMotivoNc)
{
	static dbf mvTick;
	char tStr[50];
	int nrecno; //ultimo numero de registro
	

	if(!mvTick.Use(fn)) 
	{
		WriteBootLog("no puedo utilizar el archivo ya que esta ocupado");
		return;
	}
	Inicializando = 1;			// Fuerza calculo de totales.	
	GCStatic::AddLockToTrans("IncorporeTicketNC");
	tDbf.Go(DB_GO_BOTTOM); //tiene que ir al final del ticket
	nrecno = (int) tDbf.Recno();

	for(mvTick.Go(DB_GO_TOP); !mvTick.Eof(); mvTick.Skip(1))
	{		
		mvTick.Get("func", tStr);
		switch(atoi(tStr))
		{
			case 2:
				mvTick.Get("nro", tStr);
				//if (atoi(tStr) == 9999)//es Ticket factura
				//{
				//	mvTick.Replace("func", "34");
				//}
				//else
					mvTick.Replace("func", "37");			
				break;
			////Por ahora func = 3 no se habilita o sea quedará deshabilitado
			//case 3:
			//	break;
			case 4:
				mvTick.Replace("func", "35");
				break;
			case 28://Percepcion de IIBB Jujuy
				mvTick.Replace("func", "36");
				mvTick.Replace("nro", "1");
				break;
			case 31://Percepcion de TISSH Salta Capital
				mvTick.Replace("func", "36");
				mvTick.Replace("nro", "2");
				break;	
			case 38://Percepcion IVA RG 5329
				mvTick.Replace("func", "36");
				mvTick.Replace("nro", "3");
				break;	
			default:
				mvTick.Delete();
				break;
		}		
	}

	for(mvTick.Go(DB_GO_TOP); !mvTick.Eof(); mvTick.Skip(1))
	{
		if (!mvTick.Deleted())
		{
			EsNotaCredito = true;
			tDbf.AppendBlank();
			tDbf.InsertFrom(mvTick);		
			
			ReprocActualTran();			// Recalcula el renglon.
			PrintActualTran();			// Reimprime el renglon.
		}
	}
	Inicializando = 0;
	GCStatic::ReleaseLockFromTrans("IncorporeTicketNC");
	PrintTktTot();					// Actualiza el total en pantalla
	mvTick.Close();
	FlushXPBuffers();
	lastact = time__();
}



//   Busca la Nota Credito en la tabla
bool BuscaTicketNc(int tipoComp, long NroComp, int fechaOrig)
{
	dbf mvTickNc;
	char tStr[50];	

	bool buscaNc = false;

	if(!mvTickNc.Use("ticknot.DBF")) 
	{
		WriteBootLog("no puedo utilizar el archivo ya que esta ocupado");
		return buscaNc;
	}
	
	
	for(mvTickNc.Go(DB_GO_TOP); !mvTickNc.Eof(); mvTickNc.Skip(1))
	{		
		mvTickNc.Get("nrotick", tStr);
		if (strcmp(tStr, ncControl->NroTicket) == 0){
			mvTickNc.Get("nronc", ncControl->NroNotaCredito);
			mvTickNc.Get("fecha", tStr);
			ncControl->Fecha = atoi(tStr);
			mvTickNc.Get("hora", tStr);
			ncControl->Hora = atoi(tStr);
			mvTickNc.Get("mp", tStr);
			ncControl->Mp = atoi(tStr);
			mvTickNc.Get("saldo", tStr);
			ncControl->Saldo = atof(tStr);
			mvTickNc.Get("usado", tStr);
			ncControl->Usado = tStr[0] == 'T';
			buscaNc = true;
			break;
		}
	}
	return buscaNc;	
}

//   Escribe los datos de la nota de Credito realizada en un archivo con una relacion
void EscribeRelTicketNc()
{
	static dbf wrTickNc;
	char tStr[50];
	char ltpBuff[100];	

	if(!wrTickNc.Use("ticknot.DBF")) 
	{
		WriteBootLog("no puedo utilizar el archivo ya que esta ocupado");
		return;
	}
	
	GCStatic::AddLockToTrans("EscribeRelTicketNc");

	wrTickNc.AppendBlank();
	wrTickNc.Replace("nrotick", ncControl->NroTicket);
	wrTickNc.Replace("nronc", ncControl->NroNotaCredito);
	sprintf(tStr, "%d", ncControl->Fecha);
	wrTickNc.Replace("fecha", tStr);
	sprintf(tStr, "%d", ncControl->Hora);
	wrTickNc.Replace("hora", tStr);
	sprintf(tStr, "%d", ncControl->Mp);
	wrTickNc.Replace("mp", tStr);
	sprintf(tStr, "%.3lf", ncControl->Saldo);
	wrTickNc.Replace("saldo", tStr);
	wrTickNc.Replace("usado", (ncControl->Usado ? "T" : "F"));
	GCStatic::ReleaseLockFromTrans("EscribeRelTicketNc");
	wrTickNc.Close();
}

void WriteTicketWithDump(char *fld, char *value)
{
	//tTmp.Replace(fld, value);
	if(monitorMode != 0)
		tMonitor.Replace(fld, value);
}

//   Graba informacion en el Ticket Temporario, para que sea displayada en su momento.
void WriteTickTmp(char *cod, char *des, double cant, double bulto, double uni, double tot, int voided)
{
	// Si ticketTemp solo contiene basura, lo zapea
	String ^wtcant, ^wtunit, ^wttot;
	if(zapearTicketTemp)
	{
		zapearTicketTemp = false;
		//tTmp.Zap();
#ifdef POSXP
		//Dump::Browse->Refresh();
		if (Dump::cTicket != nullptr && Dump::cTicket->RecCount()>0)
			Dump::cTicket->LimpiarTicket();

#else
		bDet.dRefresh();
#endif
	}

	// Nuevo registro
	//tTmp.AppendBlank();
	if(monitorMode != 0)
	{
		tMonitor.AppendBlank();
		tMonitor.Lock(1);
	}

	// Codigo y nombre
	WriteTicketWithDump("codigo", cod);
	WriteTicketWithDump("nombre", des);
	tdes_[0] = voided? 'T': 'F';
	tdes_[1] = 0;
	WriteTicketWithDump("anulable", tdes_);

	// Cantidad (con unidad por bulto, si 'bulto' es distinto de cero)
	if(cant == 0.0)
		STRCPY(tdes_, "");
	else
	{		
		if(fabs(ceil(cant) - cant) >= 0.001)
			sprintf(tdes_, "%6.3lf", cant);
		else
			sprintf(tdes_, "%6.0lf", cant);

		// Agrega la descripcion del bulto, si tiene.
		if(bulto > 0)
		{
			char tbulto[20];
			sprintf(tbulto, " X %d", (int)bulto);
			while(tdes_[0] == ' ')
				memmove(tdes_, tdes_ + 1, strlen(tdes_));
			STRCAT(tdes_, tbulto);
		}
	}
	WriteTicketWithDump("cant", tdes_);
	wtcant = gcnew String(tdes_);
	
	if(uni == 0.0)
		STRCPY(tdes_, "");
	else
		sprintf(tdes_, "%8.2lf", uni);
	WriteTicketWithDump("unitario", tdes_);
	wtunit = gcnew String(tdes_);

	// Total
	if(tot == 0.0)
		STRCPY(tdes_, "");
	else
		sprintf(tdes_, "%9.2lf", tot);
	WriteTicketWithDump("total", tdes_);	
	wttot = gcnew String(tdes_);

	// Puntero al record en el trans que le corresponde a este registro.
	GCStatic::AddLockToTrans("WriteTickTmp-ReadRecno");
	long tRecno = tDbf.Recno();
	//validacion agregada por los vendedores Mayoristas
	if (numRecFunc4 > 0){
		tRecno = numRecFunc4;
		numRecFunc4 = 0;
	}
	Dump::cTicket->Agrega(gcnew String(cod), gcnew String(des), wtcant, wtunit, wttot, (int) tRecno, voided == 1);
	GCStatic::ReleaseLockFromTrans("WriteTickTmp-ReadRecno");

	_ltoa(tRecno, tdes_, 10);
	WriteTicketWithDump("dump", tdes_);

	// Vacia los datos a donde corresponda
#ifdef POSXP
	/*if(Dump::Browse != nullptr)
		Dump::Browse->Refresh();*/
	if (Dump::cTicket != nullptr)
		Dump::cTicket->Refresh();

#else
	bDet.dRefresh();
#endif
	if(monitorMode != 0)
	{
		tMonitor.Lock(0);
		tMonitor.Flush();
		FlushXPBuffers();
	}
	lastact = time__();
}

//   Reprocesa la transaccion actualmente apuntada en el trans.dbf
void ReprocActualTran()
{
	static int anulado;
	static union Dump_ d;

	GCStatic::AddLockToTrans("ReprocActualTran");
	tDbf.Get("anulado", tdes_);
	anulado = (tdes_[0] == 'T')? 1: 0;
	tDbf.Get("func", tdes_);
	if(anulado || (atoi(tdes_) == 0))
	{
		GCStatic::ReleaseLockFromTrans("ReprocActualTran");
		return;
	}

	trDecode(&d);				// Toma la informacion.
	switch(d.f.id)
	{
		case DSuspen:
			ProcSuspende(&d, 0);
			break;
		case DClk:
			ProcClerk(&d, 0);
			break;
		case DVen:
			ProcVen(&d, 0);
			break;
		case DDepto:
			ProcDepto(&d, 0);
			break;
		case DMpag:
			ProcMpag(&d, 0);
			break;
		case DPlu:
			ProcPlu(&d, 0);
			break;
		case DMode:
			ProcChmode(&d, 0);
			break;
		case DZeta:
			ProcZeta(&d, 0);
			break;
		case DFactu1:
			ProcFactu1(&d, 0);
			break;
		case DDOtrop:
			ProcOtrop(&d, 0);
			break;
		case DFactu2:
			ProcFactu2(&d, 0);
			break;
		case DCorri:
			//ProcCorri(&d, 0);
			break;
		case DMisc:
			ProcMisc(&d, 0);
			break;
		case DEnvas:
			ProcEnv(&d, 0);
			break;
		case DEop:
            GlobalTF = GlobalCCSelected = 0;
			break;
		case DReparto:
			ProcRepar(&d, 0);
			break;
        case DTickFac:
            ProcTickFac(&d, 0);
			break;
		case DCCOnln:
			ProcCCOnln(&d, 0);
			break;
		case DCobraCC:
			ProcCobraCC(&d, 0);
			break;
		case DSuperv:
			ProcSuperv(&d, 0);
			break;
		case DPromo:
			ProcPromo(&d, 0);
			break;
		case DFactu3:
			ProcFactu3(&d, 0);
			break;

		case DNcItem:
			ProcNcPlu(&d, 0);
			break;
		/*case DNcHead:
			ProcNcHead(&d, 0);*/
			break;
		case DNcPer:
			ProcNcPercep(&d, 0);
			break;
		case DNcPago:
			ProcNcMpag(&d, 0);
			break;
		case DVendMay:
			ProcVendedorMayor(&d, 0);
			break;

	}	// Del switch
	GCStatic::ReleaseLockFromTrans("ReprocActualTran");
	lastact = time__();
}

//   Abre los archivos de transacciones.
void OpenTransFiles()
{
	char tBuff[100];

	GCStatic::AddLockToTrans("OpenTransFiles");
	if(!tDbf.Opened())
	{
		if(!tDbf.Use(FILE1))
		{
			// Preserva el trans si el problema no es que no existe, sino que no lo puede abrir
			// porque esta corrupto
			if(Existe(FILE1))
			{
				rename("trcrash.9", "trcrash.10");
				rename("trcrash.8", "trcrash.9");
				rename("trcrash.7", "trcrash.8");
				rename("trcrash.6", "trcrash.7");
				rename("trcrash.5", "trcrash.6");
				rename("trcrash.4", "trcrash.5");
				rename("trcrash.3", "trcrash.4");
				rename("trcrash.2", "trcrash.3");
				rename("trcrash.1", "trcrash.2");
				rename(FILE1,       "trcrash.1");
			}			
			if(!tDbf.creaTable(FILE1, crStr))
				AbnormalEnd("No puedo crear a TRANS.DBF");			
		}
	}
	//abre la tabla de los aunlados por reinicio
	if (!tAnul.Opened())
	{
		if (!tAnul.Use(FILE4))
		{
			if (!Existe(FILE4))
				tAnul.creaTable(FILE4, crAnula);
		}
	}

	// El transctl no me interesa si esta corrupto. En este ultimo caso, simplemente lo
	// vuelve a crear correctamente.
	if(!tCtl.Opened())
		if(!tCtl.Use(FILE2))
			if(!tCtl.creaTable(FILE2, crStrCtl))
				AbnormalEnd("No puedo crear a TRANSCTL.DBF");

	// El transctl debe tener al menos un registro.
	if(tCtl.RecCount() < 1)
	{
		tCtl.AppendBlank();
		tCtl.Flush();
	}

	// Busca eliminar registros corruptos
	tDbf.Go(DB_GO_BOTTOM);
	int packear = 0;
	while(!tDbf.Bof())
	{
		bool ok = true;
		tDbf.Get("func", tBuff);
		int nFunc = atoi(tBuff);
		if(nFunc < 1 || nFunc > MAX_FUNC)
			ok = false;

		// Si no encuentra problemas, sale
		if(ok && !tDbf.Deleted()) 
			break;

		// Elimina el registro con problemas y busca otro.
		packear++;		
		tDbf.Delete();
		tDbf.Skip(-1);
	}

	if(packear != 0)
	{
		sprintf(tBuff, "ERROR - El 'Trans' estaba corrupto. Se eliminaron %d registros.", packear);
		WriteBootLog(tBuff);
		tDbf.Pack();
	}
	tDbf.Go(DB_GO_BOTTOM);

	// Detecta la ultima transaccion terminada correctamente
	tDbf.Go(DB_GO_BOTTOM);
	while(!tDbf.Bof())
	{
		tDbf.Get("func", tBuff);
		int nFunc = atoi(tBuff);
		if(nFunc == DEop)
		{
			tDbf.Skip(1);
			break;
		}
		tDbf.Skip(-1);
	}
	ActualTrans = tDbf.Recno();
	sprintf(tBuff, "%d", ActualTrans);
	tCtl.Replace("act_trans", tBuff);

	GCStatic::ReleaseLockFromTrans("OpenTransFiles");
	FlushXPBuffers();
	lastact = time__();
}

//   Este metodo llama a un plugin configurado para que reprocese las transacciones, de manera de
//   tener informacion online de lo que va sucediendo.
bool ProcesarOnline()
{
	bool res = true;

#if defined(POSXP) && defined(WFACT)
	char buff[20];

	tCtl.Get("l_sync", buff);
	int lastSync = atoi(buff);	

	if(lastSync < (int)tDbf.RecCount() && ponline && CheckNetwork())
	{
		tDbf.Close();
		try
		{
#if defined(DBMYSQL)
			int recafec = enviaTransSql(lastSync);			
			if (recafec > 0){
				itoa( lastSync + recafec, msg, 10);
				tCtl.Replace("l_sync", msg);
				//return res;
			}

#endif 
			fact::FactPluginClass ^plugin = GCStatic::GetFactPlugin();
			if(plugin != nullptr)
				res = plugin->SaveOnline("trans.dbf", lastSync);

		}
		catch(...) { res = false; }
		tDbf.Use(FILE1);

	//	// Asienta el nuevo puntero a los registros ya procesados.
		if(res)
		{
			itoa(tDbf.RecCount() + 1, msg, 10);
			tCtl.Replace("l_sync", msg);			// El flush lo hace WriteEOPFiscal
		}
	}
#endif

	return res;
}

//   Al comienzo, o despues de una zeta ordenada desde la red, la siguiente funcion inicializa 
//   el manejo del dumping de transacciones.
int InitDump()
{
	static int emitBrowse;
	char lcuit[20], lpv[10];

	//  Construye los archivos necesarios para el manejo de 'mayoristas'
	InitMayorHandling();

	//  Reabre los archivos.
	WriteBootLog("Iniciando TRANS.DBF y relacionados", BM_MSGZ);
	OpenTransFiles();

	//   Lee el nro de ticket actual.
	WriteBootLog("Obteniendo ultimo numero de Ticket", BM_MSGZ);
	ticket = pf->GetTickNro(GlobalTF);
	WriteBootLog("Obteniendo el punto de venta");
	pf->Owner(lcuit, lpv);

	//  Inicializa el archivo de ticket temporario.
	WriteBootLog("Iniciando ticket temporario", BM_MSGZ);
	emitBrowse = 0;
	/*if(!tTmp.Opened())
	{*/
		emitBrowse = 1;
		/*tTmp.creaTable(FILE_TMP, crTick);
	}*/

	//  Levanta las variables de control.
	WriteBootLog("Cargando variables de control", BM_MSGZ);
	tCtl.Go(DB_GO_TOP);
	tCtl.Get("act_trans", tdes_); 
	ActualTrans = atol(tdes_);
	DelCtrlTrans = ActualTrans;
	tCtl.Get("l_sync", tdes_); 
	SyncTrans = atol(tdes_);
	if(monitorMode != 0 && !tMonitor.Opened())
		ActivaMonitoreo(0);

	WriteBootLog("Reprocesando TRANS.DBF", BM_MSGZ);
	globalInitDump = 1;
	inOper = 0;
	xReg = yReg = xCnt = 0.0;
	xTotal.Clear();
	GCStatic::AddLockToTrans("InitDump");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
		ReprocActualTran();
	// El reproceso de arriba repuebla la tabla de promociones aplicadas via
	// ProcPromo(). Con eso ya se puede reponer acumSaldoPromo, que es de
	// proceso y se perdio en el reinicio: sin ella AlmacConsumo() no descuenta
	// el saldocaja del cliente al cerrar el ticket.
	ReconstruirAcumSaldoPromo();
	if (Inicializando)
		MostrarMayorEnPantalla();
	GCStatic::ReleaseLockFromTrans("InitDump");
	PrintTktTot();

	//  Muestra el total en el visor del cliente
	char *tTotal = xTotal.VerPendiente().ToString(2);
	if(stricmp(tTotal, "0.00"))
		CustDisp("Total", tTotal);

	//   Actualiza el browse en pantalla.
	WriteBootLog("Emitiendo BROWSE de ticket temporario", BM_MSGZ);
	if(emitBrowse)
	{
#ifdef POSXP
		///DbfBrowseColumn ^col;		
		//DbfBrowse ^browser = gcnew DbfBrowse;
		ListBrowse ^browser = gcnew ListBrowse();
		

		//col = gcnew DbfBrowseColumn(IntPtr(&tTmp), "codigo", "Codigo", 45);		
		/*col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);*/
		browser->AddColumn("Codigo", "Codigo", DataGridViewContentAlignment::MiddleLeft, 45);		
		
		/*col = gcnew DbfBrowseColumn(IntPtr(&tTmp), "nombre", "Nombre", 250);
		col->TextAlignment = DataGridViewContentAlignment::MiddleLeft;
		browser->Columnas->Add(col);*/
		browser->AddColumn("Nombre", "Nombre", DataGridViewContentAlignment::MiddleLeft, 250);

		/*col = gcnew DbfBrowseColumn(IntPtr(&tTmp), "cant", "Cantidad", 50);
		browser->Columnas->Add(col);*/
		browser->AddColumn("Cantidad", "Cantidad", DataGridViewContentAlignment::MiddleCenter, 50);

		/*col = gcnew DbfBrowseColumn(IntPtr(&tTmp), "unitario", "Unitario", 60);
		col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);*/
		browser->AddColumn("Unitario", "Unitario", DataGridViewContentAlignment::MiddleRight, 60);

		/*col = gcnew DbfBrowseColumn(IntPtr(&tTmp), "total", "Total", 70);
		col->TextAlignment = DataGridViewContentAlignment::MiddleRight;
		browser->Columnas->Add(col);*/
		browser->AddColumn("Total", "Total", DataGridViewContentAlignment::MiddleRight, 70);

		/*browser->Link(nullptr, MainForm::F->Grid, &tTmp);*/
		browser->Link(MainForm::F->Grid);
		/*Dump::Browse = browser;*/
		Dump::cTicket = browser;
#else
		bDet.xBrowseF(&tTmp,"codigo",   "Codigo");
		bDet.xBrowseF(&tTmp,"nombre",   "Nombre");
		bDet.xBrowseF(&tTmp,"cant",     "Cantidad");
		bDet.xBrowseF(&tTmp,"unitario", "Unitario");
		bDet.xBrowseF(&tTmp,"total",    "   Total");
		bDet.xBrowseAnimate(detalle, &tTmp, 0, 0, 1, 0);
#endif
	}
	globalInitDump = 0;
	showMessage = NULL;
	enEspera = chkNoSusp(0);

	FlushXPBuffers();
	lastact = time__();
    return 1;
}

//   Imprim los datos correspondientes a la transaccion actualmente apuntada en el trans.dbf
void PrintActualTran()
{
	union Dump_ d;
	int anulado;

	GCStatic::AddLockToTrans("PrintActualTrans");
	GlobalReprint = 1;
	tDbf.Get("anulado", tdes_);
	anulado = (tdes_[0] == 'T')? 1: 0;
	tDbf.Get("func", tdes_);
	if(anulado || (atoi(tdes_) == 0))
	{
		GCStatic::ReleaseLockFromTrans("PrintActualTrans");
		return;
	}

	trDecode(&d);					// Toma la informacion.
	if(d.f.id ==  DDepto)			// Venta x Departamento
		PrintDepto(&d);
	else if(d.f.id ==  DMpag)		// Medio de pago
		PrintMpag(&d);
	else if(d.f.id == DPlu)			// Venta x Articulo
		PrintPlu(&d);
	else if(d.f.id == DEnvas)		// Registro de envase
		PrintEnvase(&d);
	else if (d.f.id == DFactu3)	
		PrintFactu3(&d);
	else if (d.f.id == DPromo)
		PrintPromo(&d);
	else if (d.f.id == DFactu4)	
		PrintFactu4(&d);
	else if (d.f.id == DNcItem)
		PrintNcPlu(&d);
	else if (d.f.id == DNcPago)
		PrintNcMpag(&d);
	else if (d.f.id == DNcPer)
		PrintNcPer(&d);
	else if (d.f.id == DFactu5)	
		PrintFactu5(&d);
		
//	else if(d.f.id == DVen)			// Login de Mozo.
//		PrintVend(&d);
	GlobalReprint = 0;
	GCStatic::ReleaseLockFromTrans("PrintActualTrans");
}

//   En cualquier momento (basicamente, cuando el sistema se enciende y/o el impresor fiscal 
//   cancela la operacion), esta funcion se encarga de REIMPRIMIR el ticket.
void ReprintDump(bool inicial)
{
	DCompDanul_ *dcompAnula = new DCompDanul_();
    //   Ver como hacer lo que esta en la siguiente linea comentada, utilizando
    //   capacidades del impresor (El printer HASAR no cancela los tickets en cortes de corriente).
    //	if(Inicializando && (interfaz == 3)) return;
	GCStatic::AddLockToTrans("ReprintDump-GetRecCount");
	unsigned long trRecco = tDbf.RecCount();
	GCStatic::ReleaseLockFromTrans("ReprintDump-GetRecCount");
	if(ActualTrans > trRecco) 
	{
		//elimina si hay un documento abierto, 
		//no debiera darse este caso, si sucede es porque el trans esta vacio o no se posiciono correctamente
		if (pf->docFiscStat())
			pf->PrintCancel();
		return;
	}

	if(inicial || Inicializando)
	{
		zapearTicketTemp = true;
		clear(1);
		SendMsg("Reimprimiendo el Ticket",	"Espere unos segundos por favor...");
		WriteBootLog("cancela el ticket inicial");
		//Hay un documento fiscal abierto, lo cancelara
		if (pf->docFiscStat())
		{		
			int anumticket = 0;
			//double amontof = 0;
			//pf->ObtenerAbierto(anumticket, amontof);
			
			tAnul.AppendBlank();
			char auxdata[14];
			pf->PrintSubtot(auxdata, 0);
			sprintf(auxdata, "%d", ckaNro);
			dcompAnula->func = DCompAnul;
			dcompAnula->cajero = ckaNro;
			tAnul.Replace("cajero", auxdata);
			pf->GetTickNro(GlobalTF);
			sprintf(auxdata, "%d", pf->NumeroComprobante);
			dcompAnula->nticket = pf->NumeroComprobante;
			tAnul.Replace("ticket", auxdata);
			sprintf(auxdata, "%.2lf", pf->TotalActual);
			sprintf(dcompAnula->importe, "%.2lf", pf->TotalActual);
			dcompAnula->tipoDoc = pf->TipoComprobante;
			dcompAnula->ptovta = atoi(pf->PuntoDeVenta());
			dcompAnula->pescribir = true;
			tAnul.Replace("imp", auxdata);
			tAnul.Flush();
			pf->PrintCancel();
		}
		
		VendedPrinted = 0;
	}
	else
		SendMsg("Imprimiendo la Operacion", "Espere unos segundos por favor...");

	GCStatic::AddLockToTrans("ReprintDump");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
		if(!tDbf.Deleted())
			PrintActualTran();

	if (dcompAnula->pescribir)
		WriteDump(dcompAnula);
	GCStatic::ReleaseLockFromTrans("ReprintDump");
	ClearMsg();
	delete dcompAnula;
}

//   Si esta activo un reporte de zeta, mas allá de los fiscal, entonces corresponde calcular 
//   esos valores, ya que NO se calculan durante la operatoria normal de la maquina.
int cmp_(void const *key, void const *ka)
{
    return(*((int *) key) - ((struct acum *) ka)->nro);
}
int cmp__(void const *k1, void const *k2)
{
    return(((struct acum *) k1)->nro - ((struct acum *) k2)->nro);
}

void WriteToQuickFile(int h, int cant, double monto)
{
	write(h, &cant, sizeof(int));
	write(h, &monto, sizeof(double));
}

void ReadFromQuickFile(int h, int *cant, double *monto)
{
	read(h, cant, sizeof(int));
	read(h, monto, sizeof(double));
}

// Incorpora a la venta de UN cajero, la informacion del archivo que se le indica.
void IncorporeOneFile(char *fname)
{
	int h = sopen(fname, O_RDWR | O_BINARY, SH_DENYNO);
	if(h < 0)
	{
		char tErrMsg[300];
		sprintf(tErrMsg, "ERROR - IncorporeOneFile no pudo abrir '%s' para el arqueo remoto (errno=%d)", fname, errno);
		WLog(tErrMsg);
	}
	if(h >= 0)
	{
		AcumCajero *acClk = (AcumCajero *)ZetaCajeros->Find(ckaNro);
		if(acClk != NULL)
		{
			AcumMedio src, *dst;
			Acum acSrc, *acDst;

			// Agrega las ventas.
			int cant;
			read(h, &cant, sizeof(int));
			for(int j = 0; j < cant; j++)
			{
				read(h, &src, sizeof(AcumMedio));
				dst = acClk->VerAcumuladorMedio(src.VerNumeroMedio());
				dst->AgregaCobro(src.VerMontoCobros(), src.VerCantidadCobros());
				dst->AgregaDotacion(src.VerMontoDotaciones(), src.VerCantidadDotaciones());
				dst->AgregaRetiro(src.VerMontoRetiros(), src.VerCantidadRetiros());
				dst->AgregaVenta(src.VerMontoVentas(), src.VerCantidadVentas());
				dst->AgregaExtraccion(src.VerMontoExtracciones(), src.VerCantidadExtracciones());

				acClk->Retiro(src.VerCantidadRetiros(), src.VerMontoRetiros());
			}

			read(h, &cant, sizeof(int));
			for(int j = 0; j < cant; j++)
			{
				read(h, &acSrc, sizeof(Acum));
				acDst = acClk->VerAcumuladorCobros(acSrc.VerCodigo());
				acDst->Agrega(acSrc.VerCantidad(), acSrc.VerMonto());
			}

			double monto = 0;
			ReadFromQuickFile(h, &cant, &monto);
			acClk->AnulaRenglon(cant, monto);
			ReadFromQuickFile(h, &cant, &monto);
			acClk->AnulaTicket(cant, monto);
			ReadFromQuickFile(h, &cant, &monto);
			acClk->Suspension(cant, monto);
			ReadFromQuickFile(h, &cant, &monto);
			acClk->Percepcion(cant, monto);
			ReadFromQuickFile(h, &cant, &monto);
			acClk->Extraccion(cant, monto);

			// Agrega acumuladores de envases recuperados.
			read(h, &cant, sizeof(int));
			for(int j = 0; j < cant; j++)
			{
				read(h, &acSrc, sizeof(Acum));
				acDst = acClk->VerAcumuladorEnvases(acSrc.VerCodigo());
				acDst->Agrega(acSrc.VerCantidad(), acSrc.VerMonto());
			}

			// Agrega acumuladores de envases vendidos.
			read(h, &cant, sizeof(int));
			for(int j = 0; j < cant; j++)
			{
				read(h, &acSrc, sizeof(Acum));
				acDst = acClk->VerAcumuladorEnvasesVendidos(acSrc.VerCodigo());
				acDst->Agrega(acSrc.VerCantidad(), acSrc.VerMonto());
			}
		}

		close(h);
	}
}

// Calcula rapidamente la ventas y la graba en el archivo que se le indica.
// Se usa para:
//   - Dar soporte a la pantalla de ventas online.
//   - Dar soporte al cierre de ventas de cajero integrado por toda la linea.
void QuickVentas(char *tfile, int soloCajero)
{
	// Calcula las ventas solo por cajero
	int s_hor, s_dep, s_mpag, s_caj, s_vended, s_plus;
	s_hor    = z_horar;			// Salva los flags de listado.
	s_dep    = z_depto;
	s_mpag   = z_mpago;
	s_caj    = z_cajero;
	s_vended = z_vended;
	s_plus   = z_plus;

	z_horar = z_depto = z_mpago = z_vended = z_plus = 0;
	z_cajero = 1;
	CalculateExpandedZeta(false);
	z_horar = s_hor;
	z_depto = s_dep;
	z_mpago = s_mpag;
	z_cajero = s_caj;
	z_vended = s_vended;
	z_plus = s_plus;

	// Genera el archivo con las ventas. Se escribe primero a un temporal exclusivo de
	// este proceso y recien se publica con el nombre definitivo via MoveFileEx (rename
	// atomico), para que un lector remoto (arqueo de otra caja, IncorporeOneFile) nunca
	// vea el archivo a mitad de escribir ni compita por el handle con el escritor.
	char tMsg[200], tErrMsg[300], tTmpFile[130];
	sprintf(tTmpFile, "%s.tmp", tfile);
	int h = sopen(tTmpFile, O_CREAT | O_TRUNC | O_RDWR | O_BINARY, SH_DENYRW, S_IREAD | S_IWRITE);
	if(h < 0)
	{
		sprintf(tErrMsg, "ERROR - QuickVentas no pudo crear '%s' (errno=%d)", tTmpFile, errno);
		WLog(tErrMsg);
	}
	if(h >= 0)
	{
		if(soloCajero > 0)
		{
			AcumCajero *ac = (AcumCajero *)ZetaCajeros->Find(soloCajero);
			if(ac != NULL)
			{
				// Envia el detalle de venta por medio de cobro.
				int cant = ac->VentaMedios()->Count();
				double monto = 0;
				write(h, &cant, sizeof(int));
				for(int j = 0; j < cant; j++)
				{
					AcumMedio *am = (AcumMedio *)ac->VentaMedios()->Item(j);
					write(h, am, sizeof(AcumMedio));
				}

				// Agrega las cobranzas de servicios
				cant = ac->Cobranzas()->Count();
				write(h, &cant, sizeof(int));
				for(int j = 0; j < cant; j++)
				{
					Acum *acc = (Acum *)ac->Cobranzas()->Item(j);
					write(h, acc, sizeof(Acum));
				}

				// Envia el detalle de anulaciones.
				ac->VerAnulacionRenglones(&cant, &monto);
				WriteToQuickFile(h, cant, monto);
				ac->VerAnulacionTotal(&cant, &monto);
				WriteToQuickFile(h, cant, monto);
				ac->VerSuspensiones(&cant, &monto);
				WriteToQuickFile(h, cant, monto);
				ac->VerPercepciones(&cant, &monto);
				WriteToQuickFile(h, cant, monto);
				ac->VerExtracciones(&cant, &monto);
				WriteToQuickFile(h, cant, monto);

				// Envia detalle de cupones de envases recibidos
				cant = ac->Envases()->Count();
				write(h, &cant, sizeof(int));
				for(int j = 0; j < cant; j++)
				{
					Acum *ae = (Acum *)ac->Envases()->Item(j);
					write(h, ae, sizeof(Acum));
				}

				// Envia detalle de envases vendidos
				cant = ac->EnvasesVendidos()->Count();
				write(h, &cant, sizeof(int));
				for(int j = 0; j < cant; j++)
				{
					Acum *ae = (Acum *)ac->EnvasesVendidos()->Item(j);
					write(h, ae, sizeof(Acum));
				}
			}
		}
		else
		{
			// Graba la venta completa de todos los cajeros
			for(int i=0; i<ZetaCajeros->Count(); i++)
			{
				int cOperac = 0;
				double tVentas = 0, tCash = 0;
				AcumCajero *ac = (AcumCajero *)ZetaCajeros->Item(i);
				for(int j=ac->VentaMedios()->Count() - 1; j >= 0; j--)
				{
					AcumMedio *acum = (AcumMedio *)ac->VentaMedios()->Item(j);
					cOperac += acum->VerCantidadCobros() + acum->VerCantidadVentas();
					tVentas += acum->VerMontoCobros() + acum->VerMontoVentas();
					if(acum->VerNumeroMedio() == 1)
						tCash += acum->VerMontoCobros() + acum->VerMontoVentas();
				}

				int tCRetiros = ac->VerCantidadRetiros();
				double tRetiros = ac->VerMontoRetiros();
				double tCashInDrawer = tCash - tRetiros;
				int tUltHora = 0;

				sprintf(tMsg, "%d,%d,%.2lf,%.2lf,%.2lf,%.2lf,%d,%d,%.2lf\n",
					ac->CajeroNum(),
					cOperac,
					tVentas,
					tCash,
					tCashInDrawer,
					tUltHora,
					tCRetiros,
					tRetiros);

				write(h, tMsg, strlen(tMsg));
			}
		}

		// Cierra el archivo temporal y lo publica de forma atomica (reemplaza el
		// definitivo si ya existia) para que ningun lector lo vea a medio escribir.
		close(h);
		if(!MoveFileExA(tTmpFile, tfile, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			sprintf(tErrMsg, "ERROR - QuickVentas no pudo publicar '%s' desde '%s' (GetLastError=%lu)", tfile, tTmpFile, GetLastError());
			WLog(tErrMsg);
		}
	}

	// Elimina la estructura de acumulacion usada.
	FlushXPBuffers();
}

//Agregado los acumuladores para NC 3, 4, 5
int *countFac = (int *)new int[6];
int *minNumFac = (int *)new int[6];
int *maxNumFac = (int *)new int[6];
double *ventaFac = (double *)new double[6];
double m_tasas[2][10];
double m_netos[2][10];
double m_ivas[2][10];

void AcumMpag(mpag_ *mp, double monto, AcumCajero *acumCajero, bool globalCobro, bool esOnline)
{
	acum *x;

	if(monto == 0.0) return;
	if(z_mpago)
	{
		x = (acum *) bsearch(&mp->nro, zz_mpag, zz_mpagQ, sizeof(struct acum), cmp_);
		if(!x)
		{
			x = zz_mpag + (zz_mpagQ++);
			x->nro = mp->nro;
			qsort(zz_mpag, zz_mpagQ, sizeof(struct acum), cmp__);
			x = (acum *) bsearch(&mp->nro, zz_mpag, zz_mpagQ, sizeof(struct acum), cmp_);
			STRCPY(x->des, mp->des);
			x->cant = x->monto = x->cobros = 0.0;
		}
		
		if(monto > 0) x->cant++;
		if(globalCobro)
			x->cobros += monto;
		else
			x->monto += monto;
	}
	if(z_cajero)
	{
		AcumMedio *acMedio = acumCajero->VerAcumuladorMedio(mp->nro);
		if(esOnline)
		{
			if(globalCobro)
				acMedio->AgregaCobroOnline(monto);
			else
				acMedio->AgregaVentaOnline(monto);
		}
		else
		{
			if(globalCobro)
				acMedio->AgregaCobro(monto);
			else
				acMedio->AgregaVenta(monto);
		}
	}
}

void AcumMpag2(mpag_ *mp, double monto, AcumCajero *acumCajero, bool globalCobro, bool esOnline)
{
	acum *x;

	if(monto == 0.0) return;
	if(z_mpago)
	{
		x = (acum *) bsearch(&mp->nro, zz_mpag, zz_mpagQ, sizeof(struct acum), cmp_);
		if(!x)
		{
			x = zz_mpag + (zz_mpagQ++);
			x->nro = mp->nro;
			qsort(zz_mpag, zz_mpagQ, sizeof(struct acum), cmp__);
			x = (acum *) bsearch(&mp->nro, zz_mpag, zz_mpagQ, sizeof(struct acum), cmp_);
			STRCPY(x->des, mp->des);
			x->cant = x->monto = x->cobros = 0.0;
		}
		
		//if(monto > 0) x->cant++;
		if(globalCobro)
			x->cobros += monto;
		else
			x->monto += monto;
	}
	if(z_cajero)
	{
		AcumMedio *acMedio = acumCajero->VerAcumuladorMedio(mp->nro);
		if(esOnline)
		{
			if(globalCobro)
				acMedio->AgregaCobroOnline2(monto);
			else
				acMedio->AgregaVentaOnline2(monto);
		}
		else
		{
			if(globalCobro)
				acMedio->AgregaCobro2(monto);
			else
				acMedio->AgregaVenta2(monto);
		}
	}
}

void ObtenerPagosTicket(std::vector<std::pair<int, double>>& pagos)
{
	union Dump_ d;
	char buff[20];

	// caminar hacia atrás hasta inicio del ticket
	for (tDbf.Go(ActualTrans); !tDbf.Bof(); tDbf.Skip(-1))
	{
		tDbf.Get("func", buff);
		if (atoi(buff) == 3) // inicio ticket
			break;
	}

	// avanzar procesando pagos
	for (; !tDbf.Eof(); tDbf.Skip(1))
	{
		trDecode(&d);

		if (d.f.id == DMpag)
		{
			int mpag = d.mpag.nro;
			double monto = atof(d.mpag.importe);

			// Solo pago principal (sin vuelto)
			if (monto != 0)
				pagos.push_back(std::make_pair(mpag, monto));
		}

		if (d.f.id == DNcPago)
		{
			int mpag = d.pagoNc.nro;
			double monto = -atof(d.pagoNc.importe); // NC = negativo

			// Solo pago principal (sin vuelto)
			if (monto != 0)
				pagos.push_back(std::make_pair(mpag, monto));
		}

		if (d.f.id == DEop)
			break;
	}
}


void CalculateExpandedZeta(bool discrimOnline)
{
	int _nro = -1, _func = -1, clkAct = -1, anulado = -1, i, uNro = -1;
	union Dump_ d, corr;
	double _cant = 0, _monto = 0;
	struct acum *x, *xVend;
	char *_des;
	bool globalCobro = false, abiertoAqui = false;
	int globalTickFac = 0;
	double tkNetos[10], tkIvas[10], tkTasas[10];
	
	gtotalpercep = gtotalextracc = gtotaltissh = gtotalsenia = 0.0;

	// Abre la tabla de cajeros, si no esta abierta ya.
	if(!dbclk.Opened())
	{
		abiertoAqui = true;
		dbclk.Use(F_Cajeros);
	}

	// Inicializa variables intermedias de calculo
	countFac[0] = countFac[1] = countFac[2] = countFac[3] = countFac[4] = countFac[5] = 0;
	minNumFac[0] = minNumFac[1] = minNumFac[2] = minNumFac[3] = minNumFac[4] = minNumFac[5] = 99999999;
	maxNumFac[0] = maxNumFac[1] = maxNumFac[2] = maxNumFac[3] = maxNumFac[4] = maxNumFac[5] = 0;
	ventaFac[0] = ventaFac[1] = ventaFac[2] = ventaFac[3] = ventaFac[4] = ventaFac[5] = 0;

	// Inicializa ivas y tasas
	for(int i=0; i<10; i++)
	{
		tkNetos[i]= tkTasas[i] = tkIvas[i] = 0;
		m_tasas[0][i] = m_netos[0][i] = m_ivas[0][i] = 0;
		m_tasas[1][i] = m_netos[1][i] = m_ivas[1][i] = 0;
	}

	// Prepara estructuras de acumulacion
	zz_mpagQ = zz_deptoQ = zz_vendQ = 0;
    zz_mpag = (acum *) MALLOC(sizeof(acum) * (cMpag + 10), "CalculateExpandedZeta1");
    zz_depto = (acum *) MALLOC(sizeof(acum) * (cDep + 10), "CalculateExpandedZeta2");
	if(ZetaCajeros != NULL) { delete ZetaCajeros; ZetaCajeros = NULL; }
	ZetaCajeros = new ::ArrayList(true, "CalculateExpandedZeta-ZetaCajeros");
	if(z_vended && ven_enab)
        zz_vend = (acum *) MALLOC(sizeof(acum) * (cVen_ + 10), "CalculateExpandedZeta3");
	if(zz_depto == NULL ||
	   zz_mpag == NULL  ||
	   ZetaCajeros == NULL ||
	   (z_vended && ven_enab && (zz_vend == NULL))
	  )
	{
		Alert("No alcanza la memoria para Zetas Detalladas.", "Solo se emitiran Zetas Fiscales...");
        if(zz_mpag)  FREE(zz_mpag);
        if(zz_depto) FREE(zz_depto);
		if(ZetaCajeros != NULL) { delete ZetaCajeros; ZetaCajeros = NULL; }
        if(zz_vend)  FREE(zz_vend);
		return;
	}
	if(z_plus)
	{
		if(!rPlu.creaCursor("rPlu", crPlu))
			z_plus = 0;
		else
			rPlu.IndexOn("cod", "rPlu");
	}

	//  Busca el comienzo de las operaciones diarias (Hace marcha atras  en el archivo de dumping, 
	//  buscando la ultima zeta). En el caso que se esté emitiendo una X/Z manual de mozos,
	//  La variable globalVendedTop contendrá el nro de mozo a  buscar. En este caso, camina hacia 
	//  atras hasta la primera Zeta del mozo que encuentre.
	GCStatic::AddLockToTrans("CalculateExpandedZeta");
	for(; tDbf.Recno() > 1; tDbf.Skip(-1))
	{
		if(!tDbf.Deleted())
		{
			tDbf.Get("func", tdes_);
			i = atoi(tdes_);
			tDbf.Get("nro", tdes_);
			uNro = atoi(tdes_);
			if(i == DZeta)
			{
				tDbf.Skip(1);
				break;
			}
			if(globalVendedTop && (i == DZVended) && (!uNro || (uNro == globalVendedTop)))
			{
				tDbf.Skip(1);
				break;
			}
		}
		if(tDbf.Recno() == 1) break;
	}

	// Para acumular ventas por cajero.
	AcumCajero *acumCajero = NULL;
	int acumCajNum = -1;

	//  Camina por el archivo de transacciones, procesandolo.
	xVend = 0;
	//if (Dump::cierreSM == nullptr)
	//	Dump::cierreSM = gcnew Controles::CierreSosMovil();
	//Dump::cierreSM->Limpiar();
	for(; !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("anulado", tdes_);
		anulado = (tdes_[0] == 'T')? 1: 0;
		if(anulado || tDbf.Deleted()) continue;
		tDbf.Get("cajero", tdes_);
		int tmpCaje = atoi(tdes_);
		if(tmpCaje > 0) clkAct = tmpCaje;

		// Extrae los datos del cajero.
		trDecode(&d);

		// Determina la estructura por cajero donde guardar los datos.
		if(clkAct != acumCajNum && clkAct != 0)
		{
			acumCajNum = clkAct;
			acumCajero = (AcumCajero *)ZetaCajeros->Find(clkAct);
			if(acumCajero == NULL)
			{
				acumCajero = new AcumCajero(clkAct);
				ZetaCajeros->Add(clkAct, acumCajero);
			}
		}

		//  Procesa correcciones de error.
		if(d.f.id == DCorri && acumCajero != NULL)
		{
			//// Obtiene datos de lo anulado.
			//unsigned long aRec = tDbf.Recno();
			//tDbf.Go(d.corri.posi);
			//trDecode(&corr);
			//tDbf.Go(aRec);

			// Acumula en el cajero.
			double monto = 0;
			//if(d.corri.id == DPlu)
			monto =  atof(d.corri.monto);
			/*else if(corr.f.id == DDepto)
				monto = atof(corr.corri.monto);*/
			if (monto > 0)
				acumCajero->AnulaRenglon(1, monto);
		}

		if(d.f.id == DMisc && acumCajero != NULL)
		{
			AcumMedio *mpMisc = acumCajero->VerAcumuladorMedio(d.misc.mpag);
			if(d.misc.nro == 1)
			{
				double monto = atof(d.misc.imp);
				mpMisc->AgregaRetiro(monto);
				acumCajero->Retiro(-monto);
			}
			else
				mpMisc->AgregaDotacion(atof(d.misc.imp));
		}

		//  Procesa una registración de venta directa a departamento.
		if(d.f.id == DDepto)
		{
			ValuesDepto(&d, &_nro, &_des, &_cant, &_monto);
			if(_monto == 0.0) continue;
			if(z_depto)
			{
				x = (acum*) bsearch(&_nro, zz_depto, zz_deptoQ, sizeof(struct acum), cmp_);
				if(!x)
				{
					x = zz_depto + (zz_deptoQ++);
					x->nro = _nro;
					qsort(zz_depto, zz_deptoQ, sizeof(struct acum), cmp__);
					x = (acum *) bsearch(&_nro, zz_depto, zz_deptoQ, sizeof(struct acum), cmp_);
					sprintf(tdes_, "Dep:%d - %s", _nro, _des);
					if(strlen(tdes_) > 40) tdes_[39] = 0;
					STRCPY(x->des, tdes_);
					x->monto = x->cant = 0.0;
				}
				x->cant += _cant;
				x->monto += _monto;
			}
		}

		//   Procesa una registración de medio de cobro.
		if(d.f.id == DMpag && acumCajero != NULL)
		{
			if(d.mpag.nro == 9999)
			{
				globalTickFac = (d.mpag.civa[0] == 'R' && d.mpag.civa[1] == 'I')? 1: 2;
			}
			else
			{
				//if(!d.mpag.esextracash)
				//{				
					//bool isOnline = discrimOnline? (d.mpag.fecha[0] == '*' && (d.mpag.nro != 1)): false;
					bool isOnline = discrimOnline? (d.mpag.uOnline == 1 && (d.mpag.nro != 1)): false;
					struct mpag_ *mp = BuscarMpag(d.mpag.nro);
					if(mp == NULL) mp = BuscarMpag(1);
					if(mp != NULL)
					{						
						AcumMpag(mp, atof(d.mpag.importe), acumCajero, globalCobro, isOnline);
						if(d.mpag.uVuelto)
						{
							mp = BuscarMpag(mp->vltocod);
							AcumMpag(mp, -atof(d.mpag.vuelto), acumCajero, globalCobro, false);
						}						
					}
				//}
			}
		}

		//Setea los contadores de NC en caso de NC A o NC B

		if (d.f.id == DNcPago)
		{
			if ( d.pagoNc.nro == 9999 )
				globalTickFac = (d.pagoNc.civa[0] == 'R' && d.pagoNc.civa[1] == 'I') ? 4 : 5;
			else
			{
				bool isOnline = discrimOnline? (d.pagoNc.uOnline == 1 && (d.pagoNc.nro != 1)): false;
				struct mpag_ *mp = BuscarMpag(d.pagoNc.nro);
				if(mp == NULL) mp = BuscarMpag(1);
				if(mp != NULL)
				{						
					AcumMpag(mp, (-1) * atof(d.pagoNc.importe), acumCajero, globalCobro, isOnline);
					if(d.pagoNc.uVuelto)
					{
						mp = BuscarMpag(mp->vltocod);
						AcumMpag(mp, atof(d.pagoNc.vuelto), acumCajero, globalCobro, false);
					}						
				}
			}
		}

		//   Registra cobranzas de servicios
		if(d.f.id == DCobranza)
		{
			_monto = atof(d.ppago.monto);
			globalCobro = true;
			if(acumCajero != NULL)
			{
				Acum *pp = acumCajero->VerAcumuladorCobros(d.ppago.servicio);
				pp->Agrega(1, _monto);
			}
		}

		if(d.f.id == DSeniaMP && acumCajero != NULL)
		{
			gtotalsenia += atof(d.seniaMP.monto);
			acumCajero->AcumSenia(atof(d.seniaMP.monto));
		}

		//   Registra percepciones
		if(d.f.id == DFactu3 && acumCajero != NULL)
		{
			_monto = atof(d.fac3.percep);			
			if(acumCajero != NULL)
			{
				acumCajero->Percepcion(_monto);			
				gtotalpercep += _monto;
			}
		}

		//   Registra Extracciones de Dinero
		if(d.f.id == DExtraCash && acumCajero != NULL)
		{
			_monto = atof(d.dextra.monto);			
			if(acumCajero != NULL)
			{
				acumCajero->Extraccion(_monto);				

				struct mpag_ *mp = BuscarMpag(1);
				if(mp != NULL)
				{
					/*AcumMedio *acMedio = acumCajero->VerAcumuladorMedio(mp->nro);
					acMedio->AgregaExtraccion((-1) * _monto);*/
					AcumMpag2(mp, ((-1) * _monto), acumCajero, globalCobro, false);
					gtotalextracc += _monto;
					/*acMedio = acumCajero->VerAcumuladorMedio(d.dextra.nmpago);
					acMedio->AgregaExtraccion(_monto);*/
					mp = BuscarMpag(d.dextra.nmpago);
					if (mp != NULL)
						AcumMpag2(mp, _monto, acumCajero, globalCobro, false);					
				}
			}
		}

		
		//  Cobranza de cuentas corrientes
		if(d.f.id == DCobraCC)
		{
			_monto = atof(d.cobra.imp);
			globalCobro = true;
			if(acumCajero != NULL)
				acumCajero->CobranzaCC(_monto);
		}

		// Acumula los envases 'perdonados' y 'vendidos'
		if(d.f.id == DEnvas && acumCajero != NULL)
		{
			// Solo procesa los envases 'perdonados', no los vendidos.
			Acum *env = NULL;
			if(d.env.flag == 0)
				env = acumCajero->VerAcumuladorEnvases(d.env.cod);
			else
				env = acumCajero->VerAcumuladorEnvasesVendidos(d.env.cod);

			// Agrega al acumulador.
			env->Agrega(d.env.cant, 0);
		}

		//   Procesa una activacion/desactivacion de cajero.
		if(d.f.id == DClk)
		{
			if(!d.clk.logflg)
				clkAct = 0;
			else
				clkAct = d.clk.nro;
		}

		//   Procesa una registración de venta por plu.
		if(d.f.id == DPlu)
		{
			ValuesPlus(&d, &_nro, &_des, &_cant, &_monto);
			
			if(_monto == 0.0) continue;
			if(z_vended && xVend)
				xVend->monto += _monto;

			// Acumula para el iva
			BDecimal ivaTasa = (double)((d.plu.iva == 1)? 1.0: ((d.plu.iva == 2)? 1.21: 1.105));
			ivaTasa.ApplyRound(3);
			BDecimal gravado = _monto;
			gravado /= ivaTasa;
			gravado.ApplyRound(2);
			BDecimal iva = _monto;
			iva -= gravado;

			double dTasa = ivaTasa.ToDouble();
			int j;
			for(j = 0; j < 10; j++)
				if(tkTasas[j] == dTasa)
					break;
			if(j >= 10)
				for(j = 0; j < 10; j++)
					if(tkTasas[j] == 0)
						break;

			tkTasas[j] = dTasa;
			tkNetos[j] += gravado.ToDouble();
			tkIvas[j] += iva.ToDouble();

			if(z_plus)
			{
				DDplu_ *p;
				char ucod[20];

				p = (DDplu_ *) &d;
				STRCPY(ucod, padl(p->cod, 6));
				if(!rPlu.Seek(ucod))
				{
					dbplu.Seek(ucod);	
					rPlu.AppendBlank();
					rPlu.Replace("cod", ucod);
					dbplu.Get("des", tmsg_);
					rPlu.Replace("des", tmsg_);
					dbplu.Get("dep", tmsg_);
					rPlu.Replace("dep", tmsg_);
				}
				rPlu.Get("cant", tmsg_);
				sprintf(tmsg_, "%16.3lf", atof(tmsg_) + _cant);
				rPlu.Replace("cant", tmsg_);
				rPlu.Get("imp", tmsg_);
				sprintf(tmsg_, "%16.2lf", atof(tmsg_) + _monto);
				rPlu.Replace("imp", tmsg_);
			}
			if(z_depto)
			{
				x = (acum *) bsearch(&_nro, zz_depto, zz_deptoQ, sizeof(struct acum), cmp_);
				if(!x)
				{
					x = zz_depto + (zz_deptoQ++);
					x->nro = _nro;
					qsort(zz_depto, zz_deptoQ, sizeof(struct acum), cmp__);
					x = (acum *) bsearch(&_nro, zz_depto, zz_deptoQ, sizeof(struct acum), cmp_);
					sprintf(tdes_, "Dep: %d - %s", _nro, _des);
					if(strlen(tdes_) > 40) tdes_[39] = 0;
					STRCPY(x->des, tdes_);
					x->cant = x->monto = 0.0;
				}
				x->cant += _cant;
				x->monto += _monto;
			}
		}

		// Acumula tickets suspendidos.
		if(d.f.id == DSuspen && acumCajero != NULL)
		{
			if(d.susp.flag)
				acumCajero->Suspension(atof(d.susp.monto));
		}
		
		//Acumula Ticket
		if(d.f.id == DPromo && acumCajero != NULL && d.promo.tpremio == 3 &&
			(d.promo.codpromo == 20240603001701 || d.promo.codpromo == 20240603001702 || d.promo.codpromo == 20240603001703 
			|| d.promo.codpromo == 20240603001704) || d.promo.codpromo == 20240603001705)
		{
			acumCajero->IncrCantPromo();
		}

		// Acumula la venta del vendedor.
		if(d.f.id == DEop && acumCajero != NULL)
		{
			if(d.eop.anuFlag != 0){
				if (d.eop.enviadoCobradora == 0)
					acumCajero->AnulaTicket(atof(d.eop.montoAnul));
				else
					acumCajero->EnviadoCobradora(atof(d.eop.montoAnul));

			}
			else 
			{
				if(z_vended && xVend)
				{
					// Acumula en el mozo
					xVend->cant++;
					xVend->monto += atof(d.eop.imp);
					if(ven_rebot) xVend = 0;
				}

				// Acumula en los totales fiscales (Solo si es una operacion fiscal).
				if(d.eop.tipo == 0)
				{
					if (d.eop.tipoDoc == 110)
						globalTickFac = 3;

					countFac[globalTickFac]++;
					if((int)d.eop.ntick < minNumFac[globalTickFac])
						minNumFac[globalTickFac] = d.eop.ntick;
					if((int)d.eop.ntick > maxNumFac[globalTickFac])
						maxNumFac[globalTickFac] = d.eop.ntick;
					if (globalTickFac == 3 || globalTickFac == 4 || globalTickFac == 5)
						ventaFac[globalTickFac] += atof(d.eop.montoImpre);
					else
						ventaFac[globalTickFac] += atof(d.eop.imp);

					// Acumula para el iva ventas
					globalTickFac = (globalTickFac != 0)? 1: 0;
					for(int i=0; i<10; i++)
					{
						int j;
						for(j=0; j<10; j++)
							if(tkTasas[i] == m_tasas[globalTickFac][j])
								break;
						if(j >= 10)
							for(j = 0; j < 10; j++)
								if(m_tasas[globalTickFac][j] == 0)
									break;

						m_tasas[globalTickFac][j] =  tkTasas[i];
						m_netos[globalTickFac][j] += tkNetos[i];
						m_ivas [globalTickFac][j] += tkIvas[i];
					}
				}
			}

			// Inicializa variables del ticket.
			globalCobro = false;
			globalTickFac = 0;
			for(int i=0; i<10; i++)
				tkNetos[i]= tkTasas[i] = tkIvas[i] = 0;
		}

		//   Procesa una registración de vendedor activo.
		if(z_vended && (d.f.id == DVen))
		{
			ValuesVend(&d, &_nro, &_des, &_func);
			if(z_vended)
			{
				if(!_func)
					xVend = 0;
				 else
				 {
					x = (acum *) bsearch(&_nro, zz_vend, zz_vendQ,
								 sizeof(struct acum), cmp_);
					if(!x)
					{
						x = zz_vend + (zz_vendQ++);
						x->nro = _nro;
						qsort(zz_vend, zz_vendQ, sizeof(struct acum), cmp__);
						x = (acum *) bsearch(&_nro, zz_vend, zz_vendQ, sizeof(struct acum), cmp_);
						sprintf(tdes_, "Mozo: %d - %s", _nro, _des);
						if(strlen(tdes_) > 40) tdes_[39] = 0;
						STRCPY(x->des, tdes_);
						x->cant = x->monto = 0.0;
					}
					xVend = x;
				 }
			}
		}

		//   Registra percepciones TISSH
		if(d.f.id == DFactu4 && acumCajero != NULL)
		{
			_monto = atof(d.fac4.percep);			
			if(acumCajero != NULL)
			{
				acumCajero->Tissh(_monto);			
				gtotaltissh += _monto;
			}
		}

		//   Registra percepciones IVA RG 5329
		if(d.f.id == DFactu5 && acumCajero != NULL)
		{
			_monto = atof(d.fac5.percep);			
			if(acumCajero != NULL)
			{
				acumCajero->PercepIva(_monto);			
				gTotalPercepIva += _monto;
			}
		}

		//   Registra percepciones TISSH
		if(d.f.id == DAnul && acumCajero != NULL)
		{
			_monto = atof(d.fanul.importe);			
			if(acumCajero != NULL)
			{
				acumCajero->AnulTarj(_monto);			
				//gtotaltissh += _monto;
			}
		}
	}	

	GCStatic::ReleaseLockFromTrans("CalculateExpandedZeta");
	if(abiertoAqui) dbclk.Close();
}

// El siguiente metodo se usa para agregarle a la venta local de un cajero calculada por
// 'CalculateExpandedZeta', la venta generada por un cajero en otras cajas.
void IncorporaVentaRemota(int cajero)
{
	// Busca en la red todos los archivos que puedan ser ventas del cajero.
	char tFileMask[100], cajaNum[10];
	if(CheckNetwork())
	{
		// Determina los archivos de cajeros que se van a procesar.
		sprintf(cajaNum, ".%03d", caja);
		sprintf(tFileMask, "%s\\%02d%06d.*", p_Log, ckaNro, (date() % 1000000));
		::ArrayList *files = FindFiles(tFileMask);

		// Procesa cada uno
		for(int i=0; i<files->Count(); i++)
		{
			// Mira que no se vaya a reprocesar la informacion de esta misma caja.
			char fname[100];
			if(CheckNetwork())
			{
				sprintf(fname, "%s\\%s", p_Log, (char *)files->Item(i));
				char *pfile = fname + strlen(fname) - strlen(cajaNum);
				if(_stricmp(cajaNum, pfile))
					IncorporeOneFile(fname);
			}
		}
		delete files;
	}
}

//   Sincroniza el TRANS.DBF remoto. 'SyncRemoteDump' sincroniza el dump remoto, 
//   condicionado a que esta opcion se encuentre habilitada para los cierres de
//   ticket.
//   'ForceRemoteDump' se utiliza para forzar un descargue del dump en el dump
//   remoto. OJO que esto puede fallar, si el server no se encuentra disponible
//   o algo así. En este caso, retorna 1.
void SyncRemoteDump()
{
	dbf rDbf;
	if(wlog != 1 && wlog != 3) return;		// Opcion no habilitada.

	//  Efectúa algunas validaciones.
	if(SyncTrans >= ActualTrans) return;
	if(!CheckNetwork()) return;
	
	sprintf(msg, "%s\\oltrans.%03d", p_Log, caja);

	//  Anuncia la operación en la pantalla.
	int svExclu = view._exclusive;
	view._exclusive = 0;
    for(int OpenRetryCount = 50; OpenRetryCount; OpenRetryCount--)
    {
        if(!rDbf.Use(msg))
        {
            if(!Existe(msg))
            {
                if(!rDbf.creaTable(msg, crStr))
				{
					view._exclusive = svExclu;
                    return;
				}
            }
            else
			{
				view._exclusive = svExclu;
                return;
			}
        }
        else break;      // Abrio perfectamente.

        for(double DoubleRetry = 100.0; DoubleRetry != 0.0; DoubleRetry--)
            ;
    }
	view._exclusive = svExclu;

	//  Copia los registros que, SEGUN el archivo transctl, todavia no fueron copiados a la base remota.
	GCStatic::AddLockToTrans("SyncRemoteDump");
	for(tDbf.Go(SyncTrans + 1);
	    (tDbf.Recno() < ActualTrans) && !tDbf.Eof();
	    tDbf.Skip(1))
	{
		SyncTrans++;
		rDbf.AppendBlank();
		rDbf.InsertFrom(tDbf);
		_itoa(SyncTrans, tdes_, 10);
		tCtl.Replace("l_sync", tdes_);
		if((SyncTrans % 50) == 0)
		{
			rDbf.Flush();
			tCtl.Flush();
		}
	}

	GCStatic::ReleaseLockFromTrans("SyncRemoteDump");
	rDbf.Close();
}


// Verifica si el archivo que se le indica ya existe.
// retorna 1 si el archivo existe, 0 si no existe.
int ExistFile(char *fileName)
{
	int h = open(fileName, O_RDWR);
	if(h >= 0)
	{
		close(h);
		return 1;
	}
	return 0;
}

///elimina el archivo de dump remoto solo si la opcion estaba habilitada y existe conexion con el servidor
void ResetRemoteDump()
{
	dbf rDbf;
	if(wlog != 1 && wlog != 3) return;		// Opcion no habilitada.

	//  Efectúa algunas validaciones.
	
	if(!CheckNetwork()) return;
#if FACTSQL
	//invoca al procedimiento almacenado que se encarga de limpiar la informacion del trans
	TransSql::DatosSql::Cierre();
#endif
	if(SyncTrans >= ActualTrans) return;
	sprintf(msg, "%s\\oltrans.%03d", p_Log, caja);
	
	if (ExistFile(msg))
	{
		remove(msg);
		WriteBootLog("AVISO - DUMP Remoto Eliminado.");
	}

}


//   La siguiente funcion fuerza a que el archivo de log remoto se encuentre totalmente actualizado.
int ForceRemoteDump()
{
	dbf rDbf;

	//  Mira si esta habilitada la funcion de espejado.
	if(!strlen(p_Log)) return 1;

	//  Construye el nombre del log remoto.
	sprintf(msg, "%s\\trans.%03d", p_Log, caja);

	//  Si la opcion es grabar el log en la zeta, entonces se puede hacer un simple copy...
	if(wlog != 2 && wlog != 3) return 0;

	//   Asegura que la red esté conectada.
	if(!CheckNetwork()) { z_clrlog = 0; return 0; }

	GCStatic::AddLockToTrans("ForceRemoteDump");
	WriteBootLog("ForceRemoteDump: Cerrando y Preparando archivos");
	SendMsg("Enviando el archivo de transacciones al Servidor...", "");
	tCtl.Close();
	tDbf.Close();
	
	try
	{	
		//   Busca una extension para el trans que NO EXISTA en el server
		int exten = 0;
		for(exten = 0; exten < 10; exten++)
		{
			sprintf(msg, "%s\\trans.%03d", p_Log, caja + (exten * 100));
			if(!Existe(msg)) break;
		}
		if(exten > 9) exten = 9;

		for (int k = 0; k < 3; k++)
		{
			System::Threading::Thread ^hilo = System::Threading::Thread::CurrentThread;
			try
			{				
				if (!CheckNetwork())
				{
					WriteBootLog("Sin conexion de red, no se realiza copia del TRANS al Server");
					hilo->Sleep(5000);
					continue;
				}
				// Copia el archivo de transacciones
				sprintf(tmsg, "ForceRemoteDump: Copiando TRANS.DBF a '%s'", msg);
				WriteBootLog(tmsg);
				//xCopyFile("trans.dbf", msg, "Transacciones", false);	// No puede usar temporarios!!!		
				if (access(p_Log, 0) != 0)
				{
					WriteBootLog("No tengo acceso al directorio cajas");
					hilo->Sleep(5000);
					continue;
				}
				else
					System::IO::File::Copy("trans.dbf", gcnew System::String(msg), true);
				break;
			}
			catch (System::Exception^ excep)
			{		 
				System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");				
				writer->WriteLine(System::String::Format("{0} - Error capturado en funcion ForceRemoteDump, Intento {1}", System::DateTime::Now, k));		
				writer->WriteLine("Mensaje: " + excep->Message);
				writer->WriteLine("StackTrace: " + excep->StackTrace);
				writer->WriteLine("----------------------------------");
				writer->Close();
				WriteBootLog("Ocurrio un error al copiar la informacion de TRANS hacia el servidor, revisar errormain.txt");		 
				hilo->Sleep(5000);
			}
		}
		// Copia el archivo con info de tarjetas de credio
		if(Existe("cdata.bin"))
		{
			sprintf(msg, "%s\\cdata.%03d", p_Log, caja + (exten * 100));
			xCopyFile("cdata.bin", msg, "Audit Data", false);
			remove("cdata.bin");
		}
	}
	catch (System::Exception^ excep)
	 {		 
		 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
		 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ForceRemoteDump", System::DateTime::Now);
		 writer->WriteLine(fhdatos);		
		 writer->WriteLine("Mensaje: " + excep->Message);
		 writer->WriteLine("StackTrace: " + excep->StackTrace);
		 writer->WriteLine("----------------------------------");
		 writer->Close();
		 WriteBootLog("Ocurrio un error al copiar la informacion de TRANS hacia el servidor, revisar errormain.txt");		 
	 }	

	WriteBootLog("ForceRemoteDump: Reabriendo TRANS.DBF");
	OpenTransFiles();
	GCStatic::ReleaseLockFromTrans("ForceRemoteDump");
	ClearMsg();
	return 0;
}

////////////////////////////////////////////////////////////////////
//   Modulo de refacturacion.

unsigned long FactuPosi;

void ResetRengInfo()
{
	FactuPosi = ActualTrans;
	g_Total = g_TotalN = g_TotalT = g_TransporteN = g_TransporteT =
		g_Iva[0] = g_Iva[1] = g_Iva[2] = g_Iva[3] = g_Iva[4] = 0.0;
}

//   Posiciona en un renglon del trans.dat y extrae la informacion necesaria.
int GetRengInfo()
{
	static union Dump_ d;
	static int busca;

	//   Extrae la informacion del renglon.
	GCStatic::AddLockToTrans("GetRengInfo");
	for(busca = 1; busca;)
	{
		//   Verifica si hay mas renglones
		if(FactuPosi > tDbf.RecCount()) return 0;
		tDbf.Go(FactuPosi);
		trDecode(&d);
		if(d.f.id == DDepto)         // Venta x Departamento.
		{
			MakeDepFactu(&d, g_Codigo, g_Descrip, &g_Cantidad, &g_UnitarioN, &g_UnitarioT);
			busca = 0;
		}
		else if(d.f.id == DPlu)      // Venta x Articulo
		{
			MakePluFactu(&d, g_Codigo, g_Descrip, &g_Cantidad, &g_UnitarioN, &g_UnitarioT);
			busca = 0;
		}
		//   Avanza al siguiente renglon.
		FactuPosi++;
	}
	GCStatic::ReleaseLockFromTrans("GetRengInfo");

	//   Calcula los subtotales
	g_RtotalN = g_UnitarioN * g_Cantidad;
	g_RtotalT = g_UnitarioT * g_Cantidad;

	g_TransporteN = g_TransporteN + g_RtotalN;
	g_TransporteT = g_TransporteT + g_RtotalT;
	g_TotalN += g_RtotalN;
	g_TotalT += g_RtotalT;
	g_Iva[0] = g_TotalT - g_TotalN;
	g_Total  = g_Total + g_RtotalT;

	return 1;
}

//   La siguiente funcion scanea el trans, emitiendo como factura la transaccion actual.
void PrintComprob()
{
	static dbf dbcomp;
	static defc *c;

	//  Fuerza el ingreso de los datos, 
	ForceCC(1);

	//   Cierra lo utilizado.
	dbcomp.Use("pos\\comprob");
	sprintf(tdes_, "%2d", FactuFlag);
	if(dbcomp.Locate("nro", tdes_)) 
	{
		dbcomp.Get("definic", tdes_);
		c = LoadComprobDef(Trim(tdes_));
		if(c)
			c->Print();
	}
	dbcomp.Close();
}

// Resetea solamente las variables de acumulacion del POS, sin ningun otro efecto colateral.
void ResetPOSAcumInternal()
{
	cDecim = 0;
	inOper = 0;
	for(int i=0; i<4; i++)
		tNetos[i] = tIva[i] = tNpromo[i] = tNetosIva[i] = 0.0;

	xReg = yReg = xCnt = xTotAnul = TMiles = 0.0;
	xTotal.Clear();	
	dRepeat = pSubtot = pRepeat = 0;
	xCntAnul = 0;
	GlobalRngCount = 0;
	reparto_nro = 0;
	imprimiopercep = 0;
	imprimiotissh = 0;
	imprimioPerIva = false;
	cantcuping = 0;
	AcumDesctoPromo = 0;
	ClienteBenef = 0;
	acumPeso = 0;

	AcumImpuestosInternos = 0;
	xPrecEcom = 0;
	idParking = 0;
	

	ResetVouchers();
	promosCalc = false;
	WriteCmos(PROMOCALC);
	promosHechas = false;
	WriteCmos(PROMOSHECHAS);
	LimpiarPromosAplicadas();
	tVtaReal = 0;
	WriteCmos(TOTVTAREAL);
	numsecuencia = 0;
	WriteCmos(NSECUEN);

	// Limpia las variables porque son globales
	c_numero[0] = c_fecha[0] = c_nombre[0] = c_dni[0] = c_telef[0] = c_direcc[0] = 
		 c_localidad[0] = c_cod[0] = c_condiva[0] = 0;	
	leyendaFinde[0] = 0;

	baplipercep = false;
	baplitissh = false;
	bApliPerIva = false;

	validadoCierreEstacionamiento = false;

	//para borrar el motivo de NC
	motivoNotaCredito[0] = 0;
	supervNotaCredito[0] = 0;
	Dump::docActual = nullptr;

}

//   Resetea las variables internas del POS.
void ResetPOSAcum()
{	
	static int i;

	zapearTicketTemp = true;
	if(monitorMode != 0)
	{
		tMonitor.AppendBlank();
		tMonitor.Replace("dump", "99999999");
		tMonitor.Flush();
	}

	if(!PermFactuFlag) chgFactuMode(defFactuMode);	// Retorna al modo standard.
	listap = defListaPrec;
	if(listap < 1 || listap > maxMpluPrec) listap = 1;
    GlobalTF = GlobalCCSelected = 0;
	VendedPrinted = 0;
	CheckNetwork();
	if(defFactuMode) factura(1);
	if(prndelay) globalDelayPrint = 1;
	if(ven_rebot) venNro = 0;
	globalCobraCC = globalCCOnline = globalCheckPriceOverflow = 0;

	EsNotaCredito = false;
	NumCompNc = TipoCompNc = 0;
	*nroTicketOriginal = 0;
	pasadoACobradora = false;
	numSuperv = 0;

	ResetPOSAcumInternal();

	///Variables internas para Percepcion II BB
	c_percep[0]= c_fechapib[0] = c_alicuota[0] = c_leyenpercep[0] = 0;
	baplipercep = false;
	baplitissh = false;
	elige_pasar = false;
	NumPedEcommerce = 0;
	AcumDesctoPromo = 0;
	EsRI = false;
	numRecFunc4 = 0;

	actMarcaCli = 0;
	acumSaldoPromo = 0;

	tresSinInteres = 0;
	seisSinInteres = 0;
	
	codcolsel = 0;//codigo colegio seleccionado
	
	HClear();
	c_nombre[0] = 0;
	globalIgnoreSend = false;

	// Si se designo un mensaje, lo muestra en pantalla.
	if(showMessage != NULL && !bPromoCobra)
	{
		Alert(showMessage, "");
		FREE(showMessage);
		showMessage = NULL;
	}

	// Si hay demasiado dinero en gaveta, emite un mensaje.
	ResetMayor();
	FlushXPBuffers();
	ControlaGaveta();

	// Recarga la configuracion que sea aplicable
	if(forceReloadPromos) 
		ReloadPromos();	
	if (reloadPremioInst)
		ReloadPlanifInst();
	if (reloadtjon)
	{
		reloadtjon = false;
		CargaTjOnline();
	}

	// Libera estructuras del evaluador que puedan haber quedado huerfanas
	CollectValueGarbage();	
	ArenaDump();

	if(rebootear) 
	{
#ifdef POSXP
		MainForm::F->DoClose();
#else
		POSExit();
#endif
	}
}

//   Trunca el ticket actual. (Lo hace desaparecer del dump) a efecto de dejarlo suspendido.
void TruncTicket()
{
	//  Elimina el ticket en el trans.dbf
	GCStatic::AddLockToTrans("TruncTicket");
	TruncTrans();
	GCStatic::ReleaseLockFromTrans("TruncTicket");
	ticket--;

	//  Cancela el ticket en progreso, en el impresor fiscal.
	pf->PrintCancel();
	if(showMessage != NULL)
	{
		FREE(showMessage);
		showMessage = NULL;
	}
	ResetPOSAcum();
}

// Finaliza una operacion no fiscal.
void WriteEOPNoFiscal()
{
	struct DDeop_ e;

	// Registra el cierre en el dump
	GCStatic::AddLockToTrans("WriteEOPNoFiscal");
	memset(&e, 0, sizeof(DDeop_));
	e.func = DEop;
	e.tipo = 1;								// Operacion NO Fiscal.
	e.ntick = ticket;						// Numero de Ticket.
	e.fecha = (long) date();				// Fecha del ticket.
	e.hora = (long) time_();				// Hora del ticket
	e.cajero = ckaNro;						// Cajero cuando se genero la operacion
	e.ptovta = atoi(pf->PuntoDeVenta());	// Punto de Venta cuando se cerro la operacion
	*e.ticketOriginal = 0;
	e.enviadoCobradora = pasadoACobradora;
	e.superv = numSuperv;
	sprintf(e.montoImpre, "%.2lf", 0);
	WriteDump(&e);
	

	/*Dump::docActual->PuntoVenta = e.ptovta;
	Dump::docActual->TipoComprobante = pf->TipoComprobante;
	Dump::docActual->NumeroComprobante = e.ntick;
	Dump::docActual->Total = System::Convert::ToDecimal(gcnew System::String(e.imp));
	Dump::cliComprobante->PostComprobante(Dump::docActual);*/

#ifdef FACTSQL	
	if (!hasarEop && numsecuencia > 0)
	{
		TransSql::MDEop^ finop = gcnew TransSql::MDEop();
		finop->Func = e.func;
		finop->Tipo = e.func;
		finop->Ntick = e.ntick;
		finop->Imp = gcnew System::String(e.imp);
		finop->AnulFlag = e.anuFlag;
		finop->MontoAnul = gcnew System::String(e.montoAnul);
		finop->Ptovta = e.ptovta;
		finop->Tienda = gcnew System::String(saptienda);
		finop->Caja = caja;
		finop->Cajero = e.cajero;
		finop->Fecha = e.fecha;
		finop->Hora = e.hora;
		finop->CnlVta = gcnew System::String(sapcnlvta);
		finop->Towrite = true;
		Dump::sqltrans->Add(finop);
		TransSql::WriteSqlTrans::WriteTrans(numsecuencia, Dump::sqltrans);
		Dump::sqltrans->Clear();		
	}	
#endif

	//   Indica en el log que la operación esta cerrada.
	ProcesarOnline();
	ActualTrans = tDbf.RecCount() + 1;
	_itoa(ActualTrans, tdes_, 10);
	tCtl.Replace("act_trans", tdes_);
	tCtl.Flush();
	GCStatic::ReleaseLockFromTrans("WriteEOPNoFiscal");

	// Imprime vouchers
	PrintVouchers();
	ticket = pf->GetTickNro(GlobalTF);

	//   Actualiza la copia en el server.
	SyncRemoteDump();

	//  Inicializa variables de control de operacion.
	ticket++; if(ticket > 999999) ticket = 1;
	WriteCmos(TICKET);
	ResetPOSAcum();
	if (selfCheckout)
		cntEsperaTouch = timeoutSelf - 2;
}

//   Finaliza una operacion FISCAL.
void WriteEOPFiscal(int fullAnul)
{
	static struct DDeop_ e;	
	int i;
	char lsmens2[200];

	//fullAnul = hasarEop;
	//  Fuerza la activacion del vendedor, si corresponde.
	if(/*!fullAnul*/ !hasarEop && ven_enab && (rMode <= 1))
	{
		char *x;
		int len;

		if(inOper) ForceVen();

		x = venName();
        len = (40 - strlen(x)) / 2;
		for(i = 0; i < len; i++)
			msg[i] = ' ';
		msg[i] = 0;
		STRCAT(msg, x);
        while(strlen(msg) < 40) STRCAT(msg, " ");
        DispDeta(msg, "");
        DispDeta("", "");
        pf->PrintText("");
        pf->PrintText(msg);
		pf->PrintText("");
		if(!globalInitDump)
			WriteTickTmp("", msg, 0.0, 0.0, 0, 0.0, 0);
	}

	//  Imprime el ticket, si está en modo de impresion diferida.
	if(/*!fullAnul*/ !hasarEop && prndelay && interfaz)
	{
		globalDelayPrint = 0;
		ReprintDump(false);
	}

	//  Pide los datos para el reparto, si está en modo permanente de reparto.
	//  Muestra la cantidad de articulos tickeados.
	if(reparto_perm && (reparto_nro = 0))
		reparto(1);
	if(xCnt != 0.0)
		VoucherVar("@CANT_BULTOS@", xCnt);	

	//   Imprime en el pie, mensajes adicionales que correspondan (Promociones, Refacturacion, etc).
	if(!hasarEop)//(!fullAnul)
	{
		TotPremios();		
		//pf->PrintSubtot("  *SubTotal", 'P');///para obtener el subtotal
		//if (printerTot > 0)
		//{
		//	double varTot = xTotal.VerTotal().Round(2).ToDouble();
		//	double printTot = printerTot / 100;
		//	double redondeo = 0;
		//	if (printTot > varTot)//Descuento
		//	{
		//		redondeo = ceil((printTot - varTot) * 1000) / 1000;	
		//		if (redondeo > 0 && redondeo <= 0.03)
		//		pf->PrintDescto("Ajuste por redondeo", redondeo);
		//	}
		//	else if (printTot < varTot)//Percepcion
		//	{
		//		redondeo = ceil((varTot - printTot) * 1000) / 1000;
		//		if (redondeo > 0 && redondeo <= 0.03)
		//		pf->PrintPercep("Ajuste por redondeo", redondeo, 0);
		//	}			
		//}
		char *linedescto = new char[50];
		char spacedescto[41] = "                                        ";
		if (AcumDesctoPromo > 0)
		{
			sprintf(linedescto, "$%.2lf", AcumDesctoPromo);
			int largodescto = 20 - (6 + strlen(linedescto));
			spacedescto[largodescto] = 0;
			sprintf(linedescto, "&4AHORRO%s$%.2lf", spacedescto, AcumDesctoPromo);
			pf->SetPie(linedescto, 1);
		}
		delete linedescto;
#ifdef FACTADM
		if (strcmp(c_cod, "30716623080") == 0)
		{
			char *lingse = new char[120];
			sprintf(lingse, "FACTURA LIQUIDO PRODUCTO POR COMPRA EFECTUADA POR CUENTA Y ORDEN DE GSE S.A.S. - PERIODO: %s", periodofact());
			pf->SetPie(lingse, 2);
			delete lingse;
		}
		else
			pf->SetPie("", 2);
#endif	
		pf->PrintSubtot("", 0);		
		pf->PrintClose();

		//   Toma el nro de ticket actual.
		ticket = pf->GetTickNro(GlobalTF);
	}

	//   Imprime el cierre del ticket.   
	sprintf(msg, "Ticket: %-6ld              Hora: %s", ticket, hora());
	DispDeta("", "");
	DispDeta(msg, "");
	sprintf(msg, "Sucursal: %04d                Caja: %03d", suc, caja);
	DispDeta(msg, "");
	DispDeta("* * * * * * * * * * * * * * * * * * * * ", "");
	if(!hasarEop)//(!fullAnul)
	{
		if(FactuFlag > 1)
			PrintComprob();
		else
		{
			//PrintTktTot();
			pf->Cutter();
		}
	}

	//   Manda al monitor el numero de ticket
	if(monitorMode != 0)
	{
		tMonitor.AppendBlank();
		tMonitor.Lock(1);
		sprintf(msg, "** Tkt:%6ld, %s, Hora:%s **", ticket, clkName(), hora());
		WriteTicketWithDump("nombre", msg);
		WriteTicketWithDump("dump", "99999997");
		tMonitor.Lock(0);
		tMonitor.Flush();
	}

	//   Si estamos llevando el control de lo consumido por un cliente, registra entonces el
	//   nuevo cambio en la base de clientes.
	try
	{
		AlmacConsumo();
	}	
	catch (System::Exception ^mex)
	{
		WriteBootLog("Error al alamacenar consumo en Dump.cpp");
	}
	

	if (Dump::lstLeyenPromo !=nullptr && Dump::lstLeyenPromo->Count > 0)
	{
		for each(Controles::LeyenPromo ^aux in Dump::lstLeyenPromo)
		{
			Controles::OperarServidor::FinPromoGen(hasarEop == 1, aux->CodPromo, gcnew String(ipServOC), portOC, caja, ckaNro, ticket);
		}
		Dump::lstLeyenPromo->Clear();
		ReloadPromos();
	}
	
	//   Graba el log correspondiente al final del ticket.
	VoucherVar("@TICKET@", ticket);
	GCStatic::AddLockToTrans("WriteEOPFiscal");
	e.func = DEop;	
	e.tipo = 0;								// Operacion Fiscal.
	e.ntick = ticket;						// Numero de Ticket.
	e.fecha = (long) date();				// Fecha del ticket.
	e.hora = (long) time_();				// Hora del ticket
	e.cajero = ckaNro;						// Cajero que cerro la operacion
	e.ptovta = atoi(pf->PuntoDeVenta());	// Punto de venta cuando se cerro la operacion
    e.anuFlag = /*fullAnul*/hasarEop? 1: 0;				// Tipo de cierre.
	e.tipoDoc = pf->TipoComprobante;
	e.enviadoCobradora = pasadoACobradora;
	e.superv = numSuperv;
	sprintf(e.montoImpre, "%.2lf", pf->TotalActual);
	if (EsNotaCredito){
		STRCPY(e.ticketOriginal, nroTicketOriginal);
		STRCPY(e.codMotivoNc, codMotivoNc);
		STRCPY(ncControl->NroTicket, nroTicketOriginal);
		ncControl->AsignaNotaCred(pf->TipoComprobante, e.ptovta, ticket);		
		ncControl->Saldo = tVtaReal;
		ncControl->Mp = NumeroMpNc;
		ncControl->Fecha = e.fecha;
		ncControl->Hora = e.hora;
		if (Dump::docActual != nullptr){
			Dump::docActual->ReferenciaComprobante = gcnew String(nroTicketOriginal);
			Dump::docActual->MotivoNC = gcnew String(codMotivoNc);
			Dump::docActual->Total = System::Convert::ToDecimal(pf->TotalActual);			
		}
	}
	else
	{

		
		*e.ticketOriginal = 0;
		*e.codMotivoNc = 0;
		if (Dump::docActual != nullptr){
			Dump::docActual->Total = System::Convert::ToDecimal(tVtaReal);
			Dump::docActual->Anulado = e.anuFlag;
		}
	}
	if(hasarEop)//(fullAnul)
	{
		store(xTotal.VerTotal().ToDouble(), e.montoAnul);
		store(0.0, e.imp);
	}
	else
	{
		store(tVtaReal, e.imp);
		store(0, e.montoAnul);
	}	
	sprintf(lsmens2, "AVISO - tiene func = %d, ticket = %d, importe = %s, fullanull = %d, hasareop = %d", e.func, e.ntick, e.imp, fullAnul, hasarEop);
	WriteBootLog(lsmens2);
	WriteDump(&e);	
	

	if (janisEcom && Dump::PedidoJanis != nullptr && !hasarEop)
	{
		System::String^ respJanis = Controles::OperarServidor::JanisDatosTicket(Dump::PedidoJanis->SeqId, atoi(pf->PuntoDeVenta()), ticket, xTotal.VerTotal().ToDouble());
		if (respJanis->StartsWith("NOK"))
		{
			Strings::StringToChar(respJanis, lsmens2, 100);
			Alert(lsmens2, "Presione OK/Enter para continuar");
		}
	}

	
	
	e.writeOnline = 'F';
	if (Dump::docActual != nullptr){
		Dump::docActual->PuntoVenta = e.ptovta;
		Dump::docActual->TipoComprobante = pf->TipoComprobante;
		Dump::docActual->NumeroComprobante = e.ntick;

		e.writeOnline = Dump::cliComprobante->PostComprobante(Dump::docActual) ? 'T' : 'F';
		//Dump::cliComprobante->GuardaSqlServer(Dump::docActual);
	}

	if (Dump::docActual != nullptr && procesaParking && idParking > 0 && !hasarEop)
	{
		LibEntidades::Alberdi::Parking::ClienteParking^ cliParking = gcnew LibEntidades::Alberdi::Parking::ClienteParking();
		
		LibEntidades::Alberdi::Parking::ParkingPost ^parkingPost = gcnew LibEntidades::Alberdi::Parking::ParkingPost();

		parkingPost->id_estadia = idParking;
		parkingPost->caja = Dump::docActual->Caja;
		parkingPost->estado = false;
		parkingPost->pagos = Dump::docActual->Pagos;
		parkingPost->PuntoVenta = Dump::docActual->PuntoVenta;
		parkingPost->NumeroComprobante = Dump::docActual->NumeroComprobante;
		parkingPost->TipoComprobante = Dump::docActual->TipoComprobante;

		bool resultPostParking = cliParking->PostDatosParking(parkingPost);
		if (cliParking->ConError) {
			Alert("Ocurrio un error al procesar POST Estacionamiento", "");
		}
	}

#ifdef FACTSQL	
	if (!hasarEop && numsecuencia > 0)
	{
		TransSql::MDEop^ finop = gcnew TransSql::MDEop();
		finop->Func = e.func;
		finop->Tipo = e.tipo;
		finop->Ntick = e.ntick;
		finop->Imp = gcnew System::String(e.imp);
		finop->AnulFlag = e.anuFlag == 1;
		finop->MontoAnul = gcnew System::String(e.montoAnul);
		finop->Ptovta = e.ptovta;
		finop->Tienda = gcnew System::String(saptienda);
		finop->Caja = caja;
		finop->Cajero = e.cajero;
		finop->Fecha = e.fecha;
		finop->Hora = e.hora;
		finop->CnlVta = gcnew System::String(sapcnlvta);
		finop->Towrite = true;
		Dump::sqltrans->Add(finop);
		TransSql::WriteSqlTrans::WriteTrans(numsecuencia, Dump::sqltrans);
		Dump::sqltrans->Clear();				
	}	
#endif

	//   Asienta remotamnte la informacion (mientras el trans permanece trabado)
	ProcesarOnline();

	//   Indica en el log que la operación esta cerrada.
	ActualTrans = tDbf.RecCount() + 1;
	_itoa(ActualTrans, tdes_, 10);
	tCtl.Replace("act_trans", tdes_);
	tCtl.Flush();
	GCStatic::ReleaseLockFromTrans("WriteEOPFiscal");

	// Imprime los talones que correspondan.
	char *tdot = ".";
	switch(talon)
	{
		case 1:			// Talon de bar
			sprintf(msg, "%d", (ticket % 1000));
			for(int i = (20 - strlen(msg)) / 2; i > 0; i--)
			{
				memmove(msg + 1, msg, strlen(msg) + 1);
				msg[0] = ' ';
			}
			memmove(msg + 2, msg, strlen(msg) + 1);
			msg[0] = '&';
			msg[1] = '4';
			pf->FastVoucher(false, tdot, msg, tdot);
			break;	

		default:		// Sin talon
			break;
	}

	
	// Registra los vouchers especiales que correspondan.
	if(!hasarEop)//(!fullAnul)
	{
		if(globalCobraCC)
		{
			PrintCobroVoucher();
			tCobro += c_MontoCobro;
			WriteCmos(TCOBRO);
		}

		//   Imprime los vouchers que se hayan generado.
		//PrintVouchers();

		// Acumula la venta.
		tVta += xTotal.VerTotal().ToDouble();
		for(i=0; i<4; i++)
		{
			netos[i] += tNetos[i];
			WriteCmos(NETO_1 + i);
			iva[i] += tIva[i];
			WriteCmos(IVA_1 + i);
		}
		cAnula += xCntAnul;
		WriteCmos(CANULA);
		tAnula += xTotAnul;
		WriteCmos(TANULA);

		if((EsEcommerce || janisEcom) && NumPedEcommerce > 0 && !IngNroVdor)
		{
			char cnumped[20];
			sprintf(cnumped, "%lld", NumPedEcommerce);
			VoucherVar("@NUMPEDECOMM@", cnumped);
			StackVoucher(NroVoucherEcommerce);
		}

	}
	//Se cambio de lugar estaba mas arriba
	PrintVouchers();

	///System::Windows::Forms::MessageBox::Show("Paso la escritura de EOP");

	//   Actualiza la copia en el server.
	SyncRemoteDump();

	//  Inicializa variables de control de operacion.
	ticket++; if(ticket > 999999) ticket = 1;
	WriteCmos(TICKET);
	ResetPOSAcum();
	if (selfCheckout)
		cntEsperaTouch = timeoutSelf - 2;
	//System::Windows::Forms::MessageBox::Show("sale de EOP");
}

// Activacion / Desactivacion del modo de monitoreo remoto.
void ActivaMonitoreo(int forceRecreate)
{
	if(monitorMode != 0) return;
	
	char file[500], cajaNum[10];
	_itoa(caja, cajaNum, 10);
	STRCAT(STRCAT(STRCPY(file, p_Log), "\\monitor."), cajaNum);
	remove(file);
	if(forceRecreate)
	{
		tMonitor.creaTable(file, crTick);
		tMonitor.Close();
	}
	view._exclusive = 0;
	monitorMode = tMonitor.Use(file);
}

void DesactivaMonitoreo()
{
	if(monitorMode == 0) return;
	tMonitor.AppendBlank();
	tMonitor.Replace("Dump", "99999998");
	tMonitor.Close();
	monitorMode = 0;
}

// Mira si existen anulaciones que todavia no hayan sido autorizadas por un supervisor.
bool AnulacionesPendientes(void *_dest)
{
	bool result = false;
	char tBuff[50];
	dbf *dest = (dbf *) _dest;
	bool detallar = (dest != NULL);

	// Crea la tabla temporaria que contendra el resultado, si asi se pidio.
	if(detallar)
		dest->creaCursor("corri.tmp", crTick);

	GCStatic::AddLockToTrans("VerAnulaciones");
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", tBuff);
		int func = atoi(tBuff);
		if(func == DCorri)
		{
			tDbf.Get("cod", tBuff);
			if(atoi(tBuff) == 0)
			{
				result = true;
				if(detallar)
				{
					// Posiciona en el registro anulado.
					tDbf.Get("posi", tBuff);
					int posiAnu = atoi(tBuff);
					int aRec = tDbf.Recno();
					tDbf.Go(posiAnu);
					dest->AppendBlank();

					// Pasa datos desde el trans al archivo con el detalle de lo anulado.
					tDbf.Get("cod", tBuff);					// codigo del producto
					int cod = atoi(tBuff);
					sprintf(tBuff, "%-7d", cod);
					dest->Replace("codigo", tBuff);
					
					tDbf.Get("cant", tBuff);				// cantidad.
					double cant = atof(tBuff);
					if((cant - (int)cant) > 0)
						sprintf(tBuff, "%-4.3lf", cant);
					else
						sprintf(tBuff, "%-7.0lf", cant);
					dest->Replace("cant", tBuff);
					
					tDbf.Get("vuelto", tBuff);				// precio unitario
					double unitario = atof(tBuff) / 100;
					sprintf(tBuff, "%5.2lf", unitario);
					dest->Replace("unitario", tBuff);

					tDbf.Get("imp", tBuff);					// monto total
					double total = atof(tBuff);
					sprintf(tBuff, "%6.2lf", total);
					dest->Replace("total", tBuff);

					// Busca la descripion
					sprintf(tBuff, "%06d", cod);
		            dbplu.Setorder("mplucod");
					if(dbplu.Seek(tBuff))
					{
						dbplu.Get("des", tBuff);
						dest->Replace("nombre", tBuff);
					}

					// Vuelve al registro del trans donde estabamos
					tDbf.Go(aRec);
				}
				else
					break;
			}
		}
	}

	tDbf.Go(DB_GO_BOTTOM);
	GCStatic::ReleaseLockFromTrans("VerAnulaciones");
	return result;
}

// Marca todas las anulaciones en el TRANS como que estan autorizadas.
void ConfirmaAnulacionesEnTrans()
{
	char tBuff[50];

	GCStatic::AddLockToTrans("ConfirmaAnulaciones");
	int uCod = (supervNro == 0)? 1: supervNro;
	for(tDbf.Go(ActualTrans); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", tBuff);
		int func = atoi(tBuff);
		if(func == DCorri)
		{
			tDbf.Get("cod", tBuff);
			if(atoi(tBuff) == 0)
			{
				_itoa(uCod, tBuff, 10);
				tDbf.Replace("cod", tBuff);
			}
		}
	}

	tDbf.Go(DB_GO_BOTTOM);
	GCStatic::ReleaseLockFromTrans("ConfirmaAnulaciones");
}

void AcumTickCancelados(int conscajero, int &cantCancel, double &impCancel)
{
	char tBuff[50];
	cantCancel = 0;
	impCancel = 0;
	for( tAnul.Go(1); !tAnul.Eof(); tAnul.Skip(1))
	{
		tAnul.Get("cajero", tBuff);
		if (conscajero == atoi(tBuff))
		{		
			cantCancel++;
			tAnul.Get("imp", tBuff);
			impCancel += atof(tBuff);
		}
	}
}

//Borra el archivo de Tickets cancelados cuando se hace zeta
void BorraTickCancelados()
{
	if (!Existe(FILE4)){
		tAnul.Close();
		if (System::IO::File::Exists(gcnew System::String(FILE4)))
			System::IO::File::Delete(gcnew	System::String(FILE4));
	}
}



#ifdef DBMYSQL
int enviaTransSql(int rowdesde)
{
	int raffected = 0;	
	char tBuff2[30], tquery[1000], tqaux1[200], tqaux2[200], tqaux3[200];
	char tqaux4[200], tqaux5[200], tqaux6[200], tqaux7[200], tcaux[30];	
	int myfunc, mynprec, mympag, mycajero, mynro, myiva;
	int myqueanula, myposi, myotrop;
	int mytipo, myzeta, mymodo, myfecha, myhora;
	char myflag, myusocant, myusoscan, mypesado, myuvuelto, myupend, myanulado;
	char myimp[13], myteorico[13], mycant[13], mymillas[13], myvuelto[13];
	char mycodcli[6], mynumero[13], myfechac[11], mynombre[30], mydni[13];
	char my2telefono[12], mydirecc[30], mybarrio[30], mycondiva[2], mylocal[30];;
	double mycod, myenvase, myfechaz;
	long myticket;
	double tvaraux;
	tDbf.Use(FILE1);
	String ^cquery = String::Empty;
	StringBuilder ^squery = gcnew StringBuilder();
	String ^mycon = gcnew String(strconbd);	
	MySql::Data::MySqlClient::MySqlConnection ^mysqlcon = gcnew MySql::Data::MySqlClient::MySqlConnection(mycon);
	try{	
	MySql::Data::MySqlClient::MySqlCommand ^mysqlcmd = gcnew MySql::Data::MySqlClient::MySqlCommand();
	mysqlcmd->Connection = mysqlcon;
	mysqlcmd->CommandType = System::Data::CommandType::Text;
	mysqlcon->Open();

	squery->Append("INSERT INTO trans(caja,func,nprec,mpag,cajero,imp,teorico,nro,");
	squery->Append("flag,uso_cant,iva,cant,que_anula,posi,cod,uso_scan,pesado,envase,millas,");
	squery->Append("otrop,u_vuelto,u_pend,vuelto,codcli,numero,fecha_c,nombre,dni,telefono,");
	squery->Append("direcc,barrio,condiva,local,tipo,ticket,zeta,fecha_z,modo,anulado,fecha,");
	squery->Append("hora) VALUES");
	GCStatic::AddLockToTrans("enviaTransSql");	
	for(tDbf.Go(rowdesde); !tDbf.Eof(); tDbf.Skip(1))
	{
		tDbf.Get("func", tBuff2);
		myfunc = atoi(tBuff2);		
		tDbf.Get("nprec", tBuff2);
		mynprec = atoi(tBuff2);		
		tDbf.Get("mpag", tBuff2);
		mympag = atoi(tBuff2);		
		tDbf.Get("cajero", tBuff2);
		mycajero = atoi(tBuff2);		
		tDbf.Get("imp", tBuff2);
		tvaraux = atof(tBuff2);		
		sprintf(myimp, "%.3lf", tvaraux);
		tDbf.Get("teorico", tBuff2);				
		sprintf(myteorico, "%s", tBuff2);				
		tDbf.Get("nro", tBuff2);
		mynro = atoi(tBuff2);		
		tDbf.Get("flag", tBuff2);
		myflag = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');		
		tDbf.Get("uso_cant", tBuff2);
		myusocant = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("iva", tBuff2);
		myiva = atoi(tBuff2);
		tDbf.Get("cant", tBuff2);
		tvaraux = atof(tBuff2);
		sprintf_s(mycant, "%.3lf", tvaraux);		
		tDbf.Get("que_anula", tBuff2);
		myqueanula = atoi(tBuff2);
		tDbf.Get("posi", tBuff2);
		myposi = atoi(tBuff2);
		tDbf.Get("cod", tBuff2);
		mycod = atof(tBuff2);
		tDbf.Get("uso_scan", tBuff2);
		myusoscan = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("pesado", tBuff2);
		mypesado = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("envase", tBuff2);
		myenvase = atof(tBuff2);
		tDbf.Get("millas", tBuff2);
		tvaraux = atof(tBuff2);				
		sprintf(mymillas, "%.3lf", tvaraux);		
		tDbf.Get("otrop", tBuff2);
		myotrop = atoi(tBuff2);
		tDbf.Get("u_vuelto", tBuff2);
		myuvuelto = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("u_pend", tBuff2);
		myupend = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("vuelto", tBuff2);		
		strcpy(myvuelto, tBuff2);
		tDbf.Get("codcli", tBuff2);		
		sprintf(mycodcli, "%s", tBuff2);
		tDbf.Get("numero", tBuff2);		
		sprintf(mynumero, "%s", tBuff2);
		tDbf.Get("fecha_c", tBuff2);		
		sprintf(myfechac, "%s", tBuff2);
		tDbf.Get("nombre", tBuff2);				
		sprintf(mynombre, "%s", tBuff2);
		tDbf.Get("dni", tBuff2);		
		sprintf(mydni, "%s", tBuff2);
		tDbf.Get("telefono", tBuff2);		
		sprintf(my2telefono, "%s", tBuff2);
		tDbf.Get("direcc", tBuff2);		
		sprintf(mydirecc, "%s", tBuff2);
		tDbf.Get("barrio", tBuff2);				
		sprintf(mybarrio, "%s", tBuff2);
		tDbf.Get("condiva", tBuff2);		
		sprintf(mycondiva, "%s", tBuff2);
		tDbf.Get("local", tBuff2);		
		sprintf(mylocal, "%s", tBuff2);
		tDbf.Get("tipo", tBuff2);
		mytipo = atoi(tBuff2);
		tDbf.Get("ticket", tBuff2);
		myticket = atol(tBuff2);
		tDbf.Get("zeta", tBuff2);
		myzeta = atoi(tBuff2);
		tDbf.Get("fecha_z", tBuff2);
		myfechaz = atof(tBuff2);
		tDbf.Get("modo", tBuff2);
		mymodo = atoi(tBuff2);
		tDbf.Get("anulado", tBuff2);
		myanulado = strncmp(tBuff2, "T", 1) == 0 ? 'T' : (strncmp(tBuff2, "F", 1) == 0 ? 'F' : ' ');
		tDbf.Get("fecha", tBuff2);
		myfecha = atoi(tBuff2);
		tDbf.Get("hora", tBuff2);
		myhora = atoi(tBuff2);				
		*tquery = 0;
		*tqaux1 = 0;
		sprintf(tqaux1, " (%d,%d,%d,%d,%d,\'%s\',\'%s\',", caja, myfunc, mynprec, mympag, mycajero, myimp, myteorico);
		strcat(tquery, tqaux1);		
		*tqaux2 = 0;
		sprintf(tqaux2, "%d,'%c','%c',%d,\'%s\',%d,%d,", mynro, myflag, myusocant, myiva, mycant, myqueanula, myposi);
		strcat(tquery, tqaux2);		
		*tqaux3 = 0;
		sprintf(tqaux3, "%.3lf,'%c','%c',%.3lf,'%s',%d,", mycod, myusoscan, mypesado, myenvase, mymillas, myotrop);
		strcat(tquery, tqaux3);		
		*tqaux4 = 0;
		sprintf(tqaux4, "'%c','%c',\'%s\',\'%s\',\'%s\',\'%s\',", myuvuelto, myupend, myvuelto, mycodcli, mynumero, myfechac);
		strcat(tquery, tqaux4);		
		*tqaux5 = 0;		
		//sprintf(tqaux5, "\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',", mynombre, mydni, my2telefono, mydirecc, mybarrio);
		strcat(tqaux5, "'");strcat(tqaux5, mynombre);strcat(tqaux5, "','");strcat(tqaux5, mydni);
		strcat(tqaux5, "','");
		strncat(tqaux5, my2telefono, 12);
		strcat(tqaux5, "','");
		strcat(tqaux5, mydirecc);strcat(tqaux5, "','");strcat(tqaux5, mybarrio);strcat(tqaux5, "',");
		strcat(tquery, tqaux5);		
		*tqaux6 = 0;
		sprintf(tqaux6, "'%s','%s',%d,%d,%d,", mycondiva, mylocal, mytipo, myticket, myzeta);
		strcat(tquery, tqaux6);		
		*tqaux7 = 0;
		sprintf(tqaux7, "%.3lf,%d,'%c',%d,%d),", myfechaz, mymodo, myanulado, myfecha, myhora);
		strcat(tquery, tqaux7);		
		squery->Append(gcnew String(tquery));			
		raffected++;
	}	
	squery->Replace(",", ";", squery->Length - 1, 1);		
	mysqlcmd->CommandText = squery->ToString();
	mysqlcmd->ExecuteNonQuery();	
	tDbf.Go(DB_GO_BOTTOM);
	tDbf.Close();	
	delete mysqlcmd;
	GCStatic::ReleaseLockFromTrans("enviaTransSql");
	mysqlcon->Close();	
	}
	catch(System::Exception^ ex){
		raffected = 0;
		WriteBootLog("Ocurrio un error al tratar de insertar datos");
		if (mysqlcon->State == System::Data::ConnectionState::Open){
			WriteBootLog("La conexion estaba abierta!!!");
			mysqlcon->Close();
			tDbf.Close();
			GCStatic::ReleaseLockFromTrans("enviaTransSql");
			WriteBootLog(tquery);
		}
		raffected = rowdesde;
	}
	finally
	{
		 delete mysqlcon;
	}
	return raffected;
}

#endif
