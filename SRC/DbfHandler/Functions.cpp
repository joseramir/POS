#include "stdafx.h"
#include "funcs.h"


/*********************************************************************************************************************
 *   Funciones internas.
 *   RECORDAR QUE DEBEN ELIMINAR LOS ARGUMENTOS QUE NO RETORNAN
*/

// Funciones y variables externas
extern char *msg;
ValuePtr (*FunctionsNameResolver)(char *name);

/*********************************************************************************************************************
 *   Funciones internas.
 *   RECORDAR QUE DEBEN ELIMINAR LOS ARGUMENTOS QUE NO RETORNAN
*/

ValuePtr F_Iif(ValuePtr args[])
{
	bool res = args[0]->ValueBoolean();
	delete args[0];
	if(res)
	{
		delete args[2];
		return args[1];
	}
	else
	{
		delete args[1];
		return args[2];
	}
}

ValuePtr F_Str(ValuePtr args[])
{
	Value *arg = args[0];
	char *res = GetTempString();
	double dValue = arg->ValueDouble();
	int iValue = (int)dValue;
	if(fabs(dValue - iValue) > 0.0001)
		sprintf(res, "%.2lf", dValue);
	else
		sprintf(res, "%d", iValue);

	delete arg;
	return new Value(res, strlen(res));
}

ValuePtr F_DblVal(ValuePtr args[])
{
	Value *arg = args[0];
	double dValue = atof(arg->ValueString());
	int iValue = (int)dValue;
	delete arg;
	if(fabs(dValue - iValue) < 0.001)
		return new Value(iValue);
	return new Value(dValue);
}

ValuePtr F_Int(ValuePtr args[])
{
	Value *arg = args[0];
	double res;
	if(arg->Type() == 'S')
		res = atof(arg->ValueString());
	else
		res = arg->ValueDouble();
	delete arg;
	if (enApplyAction)
		cntApplyPromo = (int) res;
	return new Value((int)res);
}

ValuePtr F_Alltrim(ValuePtr args[])
{
	Value *arg = args[0];
	char *str = arg->ValueString(), *p = str;
	while(isspace(*p))
		p++;
	if(p != str)
		STRCPY(str, p);
	for(p = str + strlen(str) - 1; isspace(*p) && p >= str; p--)
		*p = 0;
	return arg;
}

ValuePtr F_Upper(ValuePtr args[])
{
	_strupr(args[0]->ValueString());
	return args[0];
}

//  Metodo auxiliar para ordenar las funciones del evaluador por su nombre.
int CompareFunctionsNames(const void *e1, const void *e2)
{
	const FuncDesc *f1 = (FuncDesc *)e1;
	const FuncDesc *f2 = (FuncDesc *)e2;
	return strcmp(f1->Name, f2->Name);
}

//  Funcion que integra una lista de funciones (descriptas con estructuras 'FuncDesc'), dentro de la estructura
//  misma del evaluador.
void IntegrateCustomFunctions(FuncDesc *funcs, int cant)
{
	int szElem = sizeof(FuncDesc);
	for(int i=0; i<cant; i++)
	{
		if(nFuncs >= 100) AbnormalEnd("No hay suficiente lugar para agregarle funciones CUSTOM al evaluador");
		memcpy(&Functions[nFuncs++], &funcs[i], szElem);
	}
	qsort(Functions, nFuncs, szElem, CompareFunctionsNames);
}

/*********************************************************************************************************************
 *   Estructura de datos para el evaluador
*/
struct FuncDesc Functions[100] =
{
	{ "ALLTRIM",			1, {'S'},					F_Alltrim },
	{ "IIF",				3, {'B', 'U', 'U'},			F_Iif },
	{ "INT",				1, {'D'},					F_Int },
	{ "UPPER",				1, {'S'},					F_Upper },
	{ "SI",                 3, {'B', 'U', 'U'},			F_Iif },   
	{ "STR",				1, {'D'},					F_Str },
	{ "VAL",				1, {'S'},					F_DblVal },
};

int nFuncs = 7;
