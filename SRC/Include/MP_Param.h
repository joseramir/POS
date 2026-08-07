//   MP_Param.h
//   Manejo de parametros para los medios de cobro.

#ifndef MP_PARAM_INCLUDED__
#define MP_PARAM_INCLUDED__

#include "pos.h"
#include "ArrayList.h"

// Lista que contiene TODOS los parametros cargados a TODOS los medios de cobro.
extern class ::ArrayList MPParam;

// Clase que define UN parametro.
struct MPParamValue
{
	char *Name;
	char *Value;
	EditItem *Editor;

	bool IsEmpty()
	{
		Trim(Value);
		return Value[0] == 0;
	}

	MPParamValue()
	{
		Name = new char[50];
		Value = new char[50];
		Name[0] = Value[0] = 0;
		Editor = NULL;
	}

	~MPParamValue() { delete Name; delete Value; }
};

// Clase que contiene TODOS los parametros que hacen a un determinado medio de pago.
class MPParamCollection
{
public:
	::ArrayList *Data;
	struct mpag_ *Medio;
	BDecimal Monto;

	MPParamValue *FindValue(char *n)
	{
		MPParamValue *v;
		for(int i = 0; i < Data->Count(); i++)
		{
			v = (MPParamValue *)Data->Item(i);
			if(!_stricmp(v->Name, n)) return v;
		}

		v = new MPParamValue();
		STRCPY(v->Name, n);
		Data->Add(v);
		return v;
	}

	MPParamCollection(int nro)
	{
		Data = new ::ArrayList(true, "MParamCollectionCtor");
		Medio = BuscarMpag(nro);
		MPParam.Add(this);
		Monto = 0;
	}
};

// Prototipos
bool MpagoPedirParametros(int nro, double monto);
bool MpagoUsaParametros(int nro);


#endif MP_PARAM_INCLUDED__