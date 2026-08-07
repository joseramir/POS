//   
//  Promo.h
//  Declaraciones para el modulo de promociones.
//

#ifndef PROMO_H_INCLUDED
#define PROMO_H_INCLUDED

#include "ArrayList.h"
#include "Evaluator.h"

// Clase que contiene un renglon de datos de la definicion de una promo.
class EvalReng
{
	char *vName;
	Expression *evalExp;
	Value *result;
	EvError *runError;

public:
	inline EvalReng(char *varName, Expression *exp)
	{
		vName = STRDUP(varName, "EvalReng:ctor");
		evalExp = exp;
		result = NULL;
		runError = NULL;
	}

	inline ~EvalReng()
	{
		FREE(vName);
		delete evalExp;
		if(result != NULL) delete result;
		if(runError != NULL) delete runError;
	}

	// Accessors
	inline char *VarName() { return vName; }
	inline Value *Result() { return result; }

	// Acciones.
	inline void Reset(void) 
	{ 
		if(result != NULL) { result = NULL; }							// Del 'DELETE' se encarga el GC
		if(runError != NULL) { delete runError; runError = NULL; }		// No asi del 'runError'. Lo eliminamos aqui.
	}

	inline EvError *Error() { return runError; }
	bool Evaluate(void);
};

// Clase que describe la definicion de una promo.
class PromoDef
{
	::ArrayList *evalRengs;
	Expression *filterExp;
	::ArrayList *actionExps;
	Value *filterResult;
	::ArrayList *cantidad;	
	bool hasErrors;

public:
	// Constructor y Destructor
	PromoDef(int promoNum);
	~PromoDef();

	// Descripcion
	char *Descrip;

	// Codigo (PLU) en el que reportar la activacion.
	long CodPlu;
	long long CodPromo;
	long CodMP;
	int NumTarjeta;
	int MarcaCli;
	int ExcluCli;

	// Numero de promocion.
	int num;

	// Rutinas usadas durante la configuracion.
	void AddExpression(char *exp);
	void SetFilter(char *exp);
	void AddAction(char *act);
	bool ValidDef(void);

	void AddCantVta(char *exp);	

	// Rutinas usadas durante la ejecucion.
	inline bool HasRuntimeErrors() { return hasErrors; }
	void Reset(void);
	void Evaluate(void);
	void ApplyAction(void);
	Value *ResolveName(char *name);
};


// Prototipos de funciones globales
int  InitPromos(void);			// Inicializa el subsistema de promociones
int  ReloadPromos(void);		// Recarga la lista de promociones.
int CargaPromosExtra(void);		// Carga las promociones para extras del servidor
void CalcularPromosAntesMP();	// Calcula las promociones que aplican ANTES de los Medios de Pago
void CalcularPromosDespuesMP();	// Calcula las promociones DESPUES de los Medios de Pago
void AplicarPromociones();		// Aplica las promociones precalculadas.
void LimpiarPromosAplicadas();	// Vacia la tabla de promociones ya aplicadas del ticket actual.
void ApliPromoCobra();
void PrintPromo(void *);

void ReloadPlanifInst();		//recarga la planificacion de premios instantaneos;

void CalcularPromoJanisAntesMP();
void AplicaPromoJanis();

// Variables globales.
extern bool promosCalc;			// Indica que las promos ya se calcularon.
extern bool forceReloadPromos;	// Indica que se deben recargar las promociones antes de usarlas.
extern bool reloadPremioInst;  // Para indicar que se recibio una nueva planificacion de premios

#endif	// PROMO_H_INCLUDED