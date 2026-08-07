#ifndef TJONLINE_INCLUDED__
#define TJONLINE_INCLUDED__

#include "stdafx.h"
#include "arraylist.h"

//#include "StaticDump.h"

class TarjCuota
{
public:
	char *Nombre;
	int  Cuota;
	double Porcen;

	TarjCuota()
	{
		Nombre = new char[60];
		Nombre[0] = 0;
		Cuota = 0;
		Porcen = 0;
	}
	~TarjCuota() { delete Nombre; }
};

class TarjDef
{
public:
	int  Codigo;
	char *Nombre;
	int  Digitos;
	char *NumComercio;
	int  CodNumera;				// Solo usado al mandar un paquete, para identificar el "marca_id"
	int  Nodo;
	bool PideCvv;
	int  Voucher;
	int  MPago;
	char tipo;
	int longaut;
	bool EnvTrack1;
	bool CvvCero;
	char *TipreNom;
	int porDesctoCom;

	char *CodTarjPrisma;
	int CodPlanPrisma;

	::ArrayList *Cuotas;

	TarjDef()
	{
		Nombre = new char[60];
		NumComercio = new char[60];
		Codigo = Digitos = CodNumera = Nodo = 0;
		Nombre[0] = NumComercio[0] = 0;
		Cuotas = new ::ArrayList(true, "TarjDefCtor");
		tipo = 0;
		longaut = 6;
		EnvTrack1 = false;
		CvvCero = false;
		TipreNom = new char[30];
		porDesctoCom = 0;

		CodTarjPrisma = new char[3];
		CodPlanPrisma = 0;
	}

	~TarjDef()
	{
		delete Cuotas;
		delete Nombre;
		delete NumComercio;
		delete TipreNom;
	}
};

extern ::ArrayList defTarjetas;

class TarjRango
{
public:
	char	*Desde;
	char	*Hasta;
	int		LenDesde, LenHasta;
	TarjDef *Definicion;

	TarjRango()
	{
		Desde = new char[20];
		Hasta = new char[20];
		Desde[0] = Hasta[0] = 0;
		LenDesde = LenHasta = 0;
		Definicion = NULL;
	}
	~TarjRango() { delete Desde; delete Hasta; }
};

class TarjInfo
{
private:
	char   *vencFormatted;
	char   *dniFormatted;
	char   *tarjMasked;
	char   *tarjFormateada;
	char   *cuotasFormateadas;
	char   *cuotaNom;

public:
	char   *CrudeCardData;		// Datos encriptados crudos de la tarjeta
	char   *Tarjeta;			// Numero de Tarjeta.
	int    Vencim;				// Fecha de Vencimiento. (Año/Mes)
	char   *Nombre;				// Nombre del usuario
	double Monto;				// Monto para la imputacion
	double TasaRecargo;			// Tasa aplicada.
	double Recargo;				// Recargo aplicado
	int    Cuotas;				// Numero de cuotas
	int    Dni;					// Numero de documento
	int    Cvv;					// Codigo de Validacion
	bool   UsoLector;			// Si leyo la tarjeta usando lector de banda
	char   *DatosBanda;			// Datos de la banda magnetica
	int    ult4Digitos;			// Ultimos 4 digitos de la tarjeta
	int    Terminal;			// Numero de Terminal
	int    Cupon;				// Datos de aprobacion
	int    Lote;				// Lote en el cual se incluye la transaccion.
	bool   Aprobada;			// Si consiguio autorizacion.
	char	 Plan;				// Plan que utilizara la tarjeta
	int	 ticket;				//Numero de cupon
	char *Track_1;				// Datos del Track 1 de la tarjeta
	char *CodigoServicio;		//Service Code de la tarjeta

	double montoDescto;
	double tasaDescto;
	double montoIngresado;

	TarjDef *Definicion;		// Definicion de la tarjeta encontrada

	char *Marca()
	{
		if(Definicion == NULL) return "";
		return Definicion->Nombre;
	}

	TarjInfo *Clone()
	{
		TarjInfo *aux = new TarjInfo();
		STRCPY(aux->CrudeCardData, this->CrudeCardData);
		STRCPY(aux->Tarjeta, this->Tarjeta);
		aux->Vencim = this->Vencim;
		STRCPY(aux->Nombre, this->Nombre);
		aux->Monto = this->Monto;
		aux->TasaRecargo = this->TasaRecargo;
		aux->Recargo = this->Recargo;
		aux->Cuotas = this->Cuotas;
		aux->Dni = this->Dni;
		aux->Cvv = this->Cvv;
		aux->UsoLector = this->UsoLector;
		STRCPY(aux->DatosBanda, this->DatosBanda);
		aux->ult4Digitos = this->ult4Digitos;
		aux->Terminal = this->Terminal;
		aux->Cupon = this->Cupon;
		aux->Lote = this->Lote;
		aux->Aprobada = this->Aprobada;
		aux->Plan = this->Plan;
		aux->ticket = this->ticket;
		aux->Definicion = this->Definicion;
		STRCPY(aux->Track_1, this->Track_1);
		STRCPY(aux->CodigoServicio, this->CodigoServicio);
		return aux;
	}

	void Clear()
	{
		Tarjeta[0] = Nombre[0] = DatosBanda[0] = 0;
		Vencim = 1208;
		Monto = 0;
		Recargo = 0;
		TasaRecargo = 0;
		Cuotas = 1;
		Dni = Cvv = 0;
		UsoLector = true;
		Definicion = NULL;
		Cupon = 0;
		Lote = 0;
		Plan = '0';
		ticket = 0;
		Track_1[0] = 0;
		CodigoServicio[0] = 0;
	}

	TarjInfo()
	{
		vencFormatted = new char[6];
		dniFormatted = new char[11];
		tarjMasked = new char[25];
		tarjFormateada = new char[25];
		cuotasFormateadas = new char[200];
		cuotaNom = new char[30];
		CrudeCardData = new char[128];
		Tarjeta = new char[22];
		Nombre = new char[60];
		DatosBanda = new char[44];
		Plan = '0';
		ticket = 0;
		Track_1 = new char[80];
		CodigoServicio = new char[10];
		Clear();
		
	}

	~TarjInfo()
	{
		delete vencFormatted;
		delete dniFormatted;
		delete tarjMasked;
		delete tarjFormateada;
		delete cuotasFormateadas;
		delete cuotaNom;
		delete CrudeCardData;
		delete Tarjeta;
		delete Nombre;
		delete DatosBanda;
		delete Track_1;
		delete CodigoServicio;
	}

	char *CuotasFormateadas();
	char *NombreCuota();
	TarjCuota *DatosCuota();

	bool TarjetaValida();
	bool VencimValido();
	bool MontoValido();
	bool CuotasValidas();
	bool DniValido();
	bool CvvValido();

	bool DatosValidos()
	{
		bool ultDigsValidos = (LastDigits() == this->ult4Digitos);
		return 
			TarjetaValida()			&& 
			VencimValido()			&& 
			MontoValido()			&& 
			CuotasValidas()			&& 
			DniValido()				&& 
			CvvValido()				&& 
			(Definicion != NULL)	&& 
			ultDigsValidos;
	}

	TarjInfo(TarjInfo *orig)
	{
		STRCPY(Tarjeta, orig->Tarjeta);
		STRCPY(Nombre, orig->Nombre);
		STRCPY(DatosBanda, orig->DatosBanda);
		Vencim = orig->Vencim;
		Cuotas = orig->Cuotas;
		Dni = orig->Dni;
		Cvv = orig->Cvv;
		UsoLector = orig->UsoLector;
	}

	int LastDigits()
	{
		if(Tarjeta != NULL && strlen(Tarjeta) > 4)
		{
			char *posi = Tarjeta + strlen(Tarjeta) - 4;
			return atoi(posi);
		}
		return -1;
	}

	char *VencimFormateado()
	{
		sprintf(vencFormatted, "%02d/%02d", (Vencim / 100) % 100, Vencim % 100);
		return vencFormatted;
	}

	char *DniFormateado()
	{
		int tdni = Dni % 1000000000;
		int p1 = tdni / 1000000; tdni -= p1 * 1000000;
		int p2 = tdni / 1000; tdni -= p2 * 1000;

		sprintf(dniFormatted, "%02d.%03d.%03d", p1, p2, tdni);
		return dniFormatted;
	}

	char *TarjetaEnmascarada(bool corto)
	{
		STRCPY(tarjMasked, corto? "****-": "****-****-****-");
		if(strlen(Tarjeta) != 16)
			STRCAT(tarjMasked, "????");
		else
			STRCAT(tarjMasked, &Tarjeta[12]);
		return tarjMasked;
	}

	char *TarjetaEnmascarada2(bool corto)
	{
		STRCPY(tarjMasked, corto? "**": "****-****-****-");
		if(strlen(Tarjeta) != 16)
			STRCAT(tarjMasked, "????");
		else if (corto)
			STRCAT(tarjMasked, &Tarjeta[10]);
		else 
			STRCAT(tarjMasked, &Tarjeta[12]);
		return tarjMasked;
	}

	char *TarjetaFormateada()
	{
		if(strlen(Tarjeta) != 16)
			STRCPY(tarjFormateada, "");
		else
		{
			char *p = tarjFormateada, *s = Tarjeta;
			for(int i = 1; i <= 16; i++)
			{
				*p++ = *s++;
				if(i != 16 && (i% 4) == 0)
					*p++ = '-';
			}
			*p = 0;
		}
		return tarjFormateada;
	}
};

extern ::ArrayList infoTarjetas;

class OnlineResult
{
public:
	int  Codigo;
	char *Mensaje;
	int  Cupon;
	int  Lote;
	int  Terminal;
	int  Ticket;

	OnlineResult()
	{
		Codigo = Cupon = Lote = Terminal = Ticket = 0;
		Mensaje = new char[200];
	}

	~OnlineResult() { delete Mensaje; }
};

extern OnlineResult OLResult;

// Prototipos de funciones
double		TotalTarjetasOnline(bool soloRecargo);
double		PedirDatosTarjetas(double pendiente);
int			PrepareTjOnline(void);
int			CargaTjOnline(void);
TarjDef	   *BuscarDef(char *num);
TarjDef    *BuscarTipre(char *nombret);
bool		ProcesarTarjetasOnline();

double		TotalMpQrCashout(bool soloRecargo);

int	   BuscarTarjeta(char *num);






// Definicion de nodos que estan online
struct NodoOnline
{
	int id;
	char *nombre;
	int nii;
	int terminal;
	char *cierre;
	bool activo;
};

// Funciones Externas
#ifdef POSXP
int SendPing(System::String ^pHost);		// Sync.cpp
#endif

// Prototipos del driver
bool InvelOnline(TarjInfo *info);
void CierreOnline(void );

//protipos para Tipre
//bool IdentificaTarjeta(int cualIndex);

double		TotalTarjetasPrisma(bool soloRecargo);
void AnularTarjetaPrisma(int cual);

//void CompletarVouchers(char *tipooperacion, bool EsAnulacion);
int			GrabarAnulacionPrisma(int cual);
int			GrabarAnulacionPrismaEcr(int cual);

double		TotalTarjetasSmartPointMp(bool soloRecargo);
void AnularTarjetaSmartPoint(int cual);

double		TotalTarjetasPrismaECR(bool soloRecargo);
void AnularTarjetaPrismaEcr();
void CierreLotePrismaEcr();

bool SplitParam2(char *src, char *dst, int lenDst);
/*
double		TotalTarjetasOnline(bool soloRecargo);
double		PedirDatosTarjetas(double pendiente);
int			PrepareTjOnline(void);
int			CargaTjOnline(void);
TarjDef	   *BuscarDef(char *num);
bool		ProcesarTarjetasOnline();
int	   BuscarTarjeta(char *num);
*/

#endif  // TJONLINE_INCLUDED__