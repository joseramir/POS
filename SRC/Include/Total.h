//   Definiciones para seguirle la pista al total del ticket.

#ifndef TOTAL_DEFINED__
#define TOTAL_DEFINED__

#include "BDecimal.h"
#include "ArrayList.h"
#include <malloc.h>

// Clase para guardar, en el total del ticket, un recargo que hace al total del ticket.
class tagRecargo
{
	int pluCod;
	BDecimal *monto;

public:
	int VerCodigo() { 
		return pluCod; 
	}

	BDecimal &Monto() { 
		return *monto; 
	}

	tagRecargo(int cod) { 
		pluCod = cod; monto = new BDecimal(0); 
	}

	~tagRecargo() { 
		delete monto; 
	}
};

class tagDescMP
{
	int CodMP;
	BDecimal *monto;
public:
	int VerMP(){
		return CodMP;
	}	

	BDecimal &MontoDesc(){
		return *monto;
	}

	tagDescMP(int codigo){
		CodMP = codigo;
		monto = new BDecimal(0);
	}

	~tagDescMP(){
		delete monto;
	}

};

class tagTotal
{
	BDecimal *venta, *temp, *pagos, *descuentos, *recfinan;
	::ArrayList *recargos;
	::ArrayList *descMedioPago;


public:
	tagTotal() { 
		venta = new BDecimal(); 
		temp = new BDecimal(); 
		pagos = new BDecimal(); 
		recfinan = new BDecimal(); 
		recargos = new ::ArrayList(true, "tagTotalCtor"); 
		descMedioPago = new ::ArrayList(true, "tagTotalCtor2"); 
		descuentos = new BDecimal(); 
	}
	~tagTotal() { 
		delete venta; 
		delete temp; 
		delete pagos; 
		delete recargos; 
		delete descuentos;
		delete descMedioPago;
		delete recfinan;
	}

	void AddVenta(BDecimal &vta) { 
		venta->operator +=(vta); 
	}

	void AddRecFinan( double vta) { 
		recfinan->operator +=(vta); 
	}

	void AddPago(BDecimal &monto) { 
		pagos->operator +=(monto); 
	}

	void AddDescuento(BDecimal &monto) { 
		descuentos->operator +=(monto); 
	}

	void ClearDescuento() { 
		descuentos->operator =(0); 
	}

	BDecimal &VerVenta() { 
		return *venta; 
	}

	double VerVentaDouble()
	{
		return venta->ToDouble();
	}

	BDecimal &VerPendiente() 
	{
		double tv;
		VerTotal();
		tv = temp->ToDouble();
		temp->operator -= (*pagos);
		tv = temp->ToDouble();
		return *temp;
	}
	
	double VerPendienteDouble()
	{
		VerTotal();
		return temp->ToDouble();
	}

	double VerRecFinan()
	{
		
		return this->recfinan->ToDouble();
	}

	// Limpia el total
	void Clear()
	{
		recargos->Clear();
		descMedioPago->Clear();
		venta->operator = (0);
		pagos->operator = (0);
		recfinan->operator = (0);
	}

	// Retorna el total de la operacion.
	BDecimal &VerTotal()
	{
		double tv;
		temp->operator =(*venta);
		tv = temp->ToDouble();
		temp->operator -=(*descuentos);
		for(int i=0; i<recargos->Count(); i++)
		{
			tagRecargo *rec = (tagRecargo *)recargos->Item(i);
			temp->operator +=(rec->Monto());
			tv = temp->ToDouble();
		}
		return *temp;
	}

	// Manejo de recargos.
	void AddRecargo(int pluCod, BDecimal &monto)
	{
		tagRecargo *rec;
		for(int i=0; i<recargos->Count(); i++)
		{
			rec = (tagRecargo *)recargos->Item(i);
			if(rec->VerCodigo() == pluCod)
			{
				rec->Monto() = monto;
				return;
			}
		}

		rec = new tagRecargo(pluCod);
		rec->Monto() = monto;
		recargos->Add(rec);
	}

	// Manejo de Descuentos Medios de pago
	void AddDescMedioPago(int mediopago, BDecimal &monto)
	{
		tagDescMP *desc;
		for(int i=0; i<descMedioPago->Count(); i++)
		{
			desc = (tagDescMP *)descMedioPago->Item(i);
			if(desc->VerMP() == mediopago)
			{
				desc->MontoDesc() = monto;
				return;
			}
		}

		desc = new tagDescMP(mediopago);
		desc->MontoDesc() = monto;
		descMedioPago->Add(desc);
	}

	// Retorna el total de la operacion.
	BDecimal &VerRecargo(int pluCod)
	{
		double tv;
		temp->operator =(0);
		tv = temp->ToDouble();		
		for(int i=0; i<recargos->Count(); i++)
		{
			tagRecargo *rec = (tagRecargo *)recargos->Item(i);
			if (rec->VerCodigo() == pluCod)
			{
				temp->operator +=(rec->Monto());
				tv = temp->ToDouble();
			}
		}
		return *temp;
	}

	// Retorna el total de la operacion.
	BDecimal &VerDescMedioPago(int mediopago)
	{
		double tv;
		temp->operator =(0);
		tv = temp->ToDouble();		
		for(int i=0; i<descMedioPago->Count(); i++)
		{
			tagDescMP *desc = (tagDescMP *)descMedioPago->Item(i);
			if (desc->VerMP() == mediopago)
			{
				temp->operator +=(desc->MontoDesc());
				tv = temp->ToDouble();
			}
		}
		return *temp;
	}

	::ArrayList *VerRecargos() { 
		return recargos; 
	}
};

extern class tagTotal xTotal;
extern bool soloSimular;

#endif