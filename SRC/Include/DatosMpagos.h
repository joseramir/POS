#pragma once

#include <string.h>
#include "pos.h"
#include "scrobj.h"

extern class PagoExtendido
{
public:
	double monto;
	double variacion;
	class PagoExtendido *items[50];
	int subItems;
	int dynTotalIndex;
	mpag_ *mp;
	//aget *getobj;
	char *getbuff;
	double extraccion;

	inline void Add(PagoExtendido *item) { items[subItems++] = item; }

	PagoExtendido(bool root)
	{
		if(!root) throw "Solo se permite root=true";
		getbuff = new char[50];
		mp = NULL;
		monto = 0.0;
		subItems = 0;
		variacion = 0;
		dynTotalIndex = 0;
		//getobj = NULL;
		extraccion = 0.0;		
	}

	PagoExtendido(mpag_ *medio)
	{
		getbuff = new char[50];
		mp = medio;
		monto = 0.0;
		subItems = 0;
		variacion = 0;
		dynTotalIndex = 0;
		//getobj = NULL;
		extraccion = 0.0;
	}

	~PagoExtendido() 
	{ 
		delete getbuff; 
		for(int i=0; i<subItems; i++)
		{
			if(items[i] != NULL)
			{
				delete items[i];
				items[i] = NULL;
			}
		}
	}

} *rootPagos;


#ifdef POSXP
#include "Strings.h"
#include "pos.h"
using namespace System;

ref class CuentaCorriente
{
public:
	static property String ^Codigo { String ^get() { return Strings::CharToString(c_cod); }};
	static property String ^Dni { String ^get() { return Strings::CharToString(c_dni); }};
	static property String ^Nombre { String ^get() { return Strings::CharToString(c_nombre); }};
	static property String ^Cuit { String ^get() { return Strings::CharToString(c_numero); }};
	static property String ^CondIva { String ^get() { return Strings::CharToString(c_condiva); }};
	static property String ^SaldoMaximo { String ^get() { return Strings::CharToString(c_salmax); }};
	static property String ^Saldo { String ^get() { return Strings::CharToString(c_saldo); }};
	static property String ^Direccion { String ^get() { return Strings::CharToString(c_direcc); }};
	static property String ^Telefono { String ^get() { return Strings::CharToString(c_telef); }};
	static property String ^Localidad { String ^get() { return Strings::CharToString(c_localidad); }};
	static property String ^DniComprador { String ^get() { return Strings::CharToString(c_localidad); }};
	static property String ^Comprador { String ^get() { return Strings::CharToString(c_nomcompr); }};
	static property String ^Percepcion { String ^get() { return Strings::CharToString((strcmp(c_percep, "S") == 0) ? "Si" : "No"); }};
	static property String ^FechaPib { String ^get() { return Strings::CharToString(c_fechapib); }};
//	static property String ^Categoria { String ^get() { return Strings::CharToString(c_categpercep); }};
	static property String ^Alicuota { String ^get() { return Strings::CharToString(c_alicuota); }};

	static property String ^RepaDes { String ^get() { return Strings::CharToString(c_repadescrip); }};
};

ref class MP 
{
public:
	property PagoExtendido *PagoExt;
	property Decimal Automatico;
	property String ^Nombre;
	property Decimal DesctoMp;
	property Decimal ImpSinDesctoMp;
	property Decimal Monto
	{
		Decimal get() { return Decimal(PagoExt->monto); }
		void set(Decimal v) { PagoExt->monto = (double)v; }
	};
		
	System::Windows::Forms::Control ^Handler;
	property bool Borrar
	{
		bool get() { return false; }
		void set(bool valor) {}
	}
};


public ref class MDatosTarj
{
public:
	int cod;
	String ^nombre;
	int mpago;
	bool extraible;
	int longaut;

	MDatosTarj()
	{
		cod = 0;
		nombre = String::Empty;
		mpago = 0;
		longaut = 0;
		extraible = false;
	}	

	MDatosTarj(int codigo, String^ name, int mediop, bool esextraible, int plongaut)
	{
		cod = codigo;
		nombre = name;
		mpago = mediop;
		extraible = esextraible;
		longaut = plongaut;
	}

	~MDatosTarj()
	{
		cod = 0;
		nombre = nullptr;
		mpago = 0;
		longaut = 0;
	}

	virtual String ^ToString() override
	{
		return nombre;
	}

};


///Clase creada para los cupones de Tarjetas POSNET
ref class CPosnet
{
public:
	//char m_nrotarjeta[17];	
	String ^m_nrotarjeta;
	double m_monto;	
	int m_cuotas;	
	bool m_credito;	
	int m_codtarjeta;
	int m_lote;
	int m_cupon;
	//char m_nrodni[9];
	double m_recargo;
	double m_tasarecargo;
	int m_vencim;
	//int m_autorizacion;
	String ^m_autorizacion;
	int m_mpago;
	String ^m_nomtarjeta;
	int indice;
	double m_extraccion;
	bool extraible;
	int longaut;	///longitud que tomara la autorización

	double m_desctoMp;
	double m_montoIngresado;

	String ^m_terminal;
	int m_tipoTerminal;
	int m_esqr;

	CPosnet()
	{
		/*m_nrotarjeta[0] = '0';
		m_nrotarjeta[1] = 0;		*/
		m_nrotarjeta = gcnew String("0");
		m_monto = 0;
		m_cupon = 0;
		m_cuotas = 0;		
		m_credito = false;		
		m_codtarjeta = 0;
		m_lote = 0;
		m_cupon = 0;
		//*m_nrodni = 0;		
		m_recargo = 0;
		m_tasarecargo = 0;
		m_vencim = 0;
		//m_autorizacion = 0;
		m_autorizacion = "0";
		m_mpago = 0;
		m_nomtarjeta = String::Empty;
		indice = 0;
		m_extraccion = 0;
		extraible = false;
		longaut = 0;

		m_desctoMp = 0;
		m_montoIngresado = 0;
		m_terminal = "00000000";
		m_tipoTerminal = 1;
		m_esqr = 0;
	}

	~CPosnet()
	{
		delete m_nrotarjeta;
		delete m_nomtarjeta;
	}
};

ref class CPromRep
{
public:		
	String ^m_cod;
	Decimal m_monto;	
	int m_cant;
	int m_millas;

	CPromRep()
	{
		m_cod = String::Empty;
		m_monto = 0;
		m_cant = 0;
		m_millas = 0;
	}

	~CPromRep()
	{
		delete m_cod;
	}
};


#endif