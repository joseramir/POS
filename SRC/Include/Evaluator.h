#ifndef _EVALUATOR_INCLUDED_
#define _EVALUATOR_INCLUDED_

// Definir este miembro para trazar fugas de memoria en 'Value' no retornados al heap.
#define DEBUG_ALLOC

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <list>
#include <debug.h>
#include <ArrayList.h>

class Evaluator;
class EvError
{
	friend class Expression;
	friend class Evaluator;

	char *msg;
	inline EvError(char *message) { msg = STRDUP(message, "EvError:ctor"); }

public:
	inline ~EvError() { FREE(msg); }
	inline char *Message() { return msg; }
};

//extern char msg[];
void WLog(char *msg, ...);		// Log de error

// Lista de los objetos 'Value' que se crean y destruyen, a efectos de simplificar el manejo de memoria.
// Se usa void* para evitar dependencia circular con la clase Value.
typedef std::list<void*> ValueTraceList;
extern ValueTraceList valueTrace;


// Clase que encapsula un valor, tal como lo retorna el evaluador de expresiones. (Tambien, el evaluador de expresiones
// va creando 'Value' interno a medida que avanza la evaluacion).
//
// Manejo de Memoria: Es complejo el seguimiento de los objetos 'Value' que se crean, puesto que los crea el evaluador pero
// queda bajo la responsabilidad de la aplicacion el liberarlos. En algunos casos de objetos 'Value' de larga vida, se hace
// muy complicado seguir la pista de si estan en uso o no.
// Por eso, los objeto value tienen una propiedad '_permanent' que indica si son de caracter temporario o permanente (por
// ejemplo, un objeto permanente seria el valor tal cual lo almacena una variable). Los objetos temporarios son eliminados
// cuando se llama a 'CollectValueGarbage'. El POS llama a ese metodo cuando se cierra un ticket.
class Value
{
	char *_stringValue;
	char type;
	ValueTraceList::iterator _traceIt;

public:
	int    _intValue;
	double _doubleValue;
	bool   _persistent;			// Si no debe recogerla 'CollectValueGarbage()0

	void CommonInit() { valueTrace.push_back(this); _traceIt = --valueTrace.end(); _persistent = false; }

	inline Value(bool boolValue) { type = 'B'; _intValue = boolValue? 1: 0; CommonInit(); }
	inline Value(int intValue) { type = 'I'; _intValue = intValue; CommonInit(); }
	inline Value(double doubleValue) { type = 'D'; _doubleValue = doubleValue; CommonInit(); }
	inline Value(char *stringValue, int sLen) 
	{ 
		type = 'S'; 
		_stringValue = new char[sLen + 1]; 
		if(sLen)
			memmove(_stringValue, stringValue, sLen); 
		_stringValue[sLen] = 0;
		CommonInit();
	}
	inline Value(Value *v) 
	{ 
		type = v->Type(); 
		switch(type)
		{
		case 'S':
			_stringValue = new char[strlen(v->_stringValue) + 1];
			STRCPY(_stringValue, v->_stringValue);
			break;

		case 'D':
			_doubleValue = v->_doubleValue; 
			break;

		default:
			_intValue = v->_intValue; 
			break;
		}
		CommonInit();
	}

	inline ~Value()
	{
		if(type == 'S') delete _stringValue;
		valueTrace.erase(_traceIt);
	}

	inline char Type() { return type; }
	inline void SetPersistentFlag(bool v) { _persistent = v; }
	inline bool GetPersistentFlag() { return _persistent; }

	inline int ValueInteger() 
	{ 
		if(type == 'I') return _intValue;
		if(type == 'D') return (int)_doubleValue;
		throw "Error Interno: 'I' esperado";
		return 0;
	}

	inline double ValueDouble() 
	{ 
		if(type == 'D') return _doubleValue;
		if(type == 'I') return (double)_intValue;
		throw "Error Interno: 'D' esperado"; 
		return 0.0;
	}

	inline char *ValueString() 
	{ 
		if(type != 'S') 
			throw "Error Interno: 'S' esperado"; 
		return _stringValue; 
	}

	inline bool ValueBoolean()
	{
		if(type == 'B' || type == 'I') return (_intValue != 0);
		if(type == 'D') return ((int)_doubleValue) != 0;
		if(_stringValue == NULL || *_stringValue == 0) return false;
		return true;
	}
	void SetStringValue(char *newValue, int lenString);
	void SetInteger(int i);
	void SetDouble(double d);
	void SetType(char newType);
	char *ToString() { return ToString(NULL); }
	char *ToString(char *format);
};
typedef class Value* ValuePtr;

extern int nFuncs;
extern struct FuncDesc
{
	char Name[32];
	int ArgCount;
	char ArgType[16];
	ValuePtr (*Run)(ValuePtr *args);
} Functions[];

class Expression
{
	friend class Evaluator;

	EvError *error, *runError;
	char *compiled, *srcPtr;
	int cLen;

	Value **run;
	int runsp, runpc;

	//  Para compilar strings temporarias. Es un buffer que crece siempre
	char *tempString;
	int tempStringLen;

	void SkipBlank();
	void CompileOperand();
	void CompileNumber();
	void CompileString();
	void CompileName();

	void CompilePush(char code);
	void CompilePush(int item);
	void CompilePush(double dbl);
	void CompilePush(char *str);
	void CompilePushName(char token, char *name);

	void CompileSubExpression();

	inline char RunItem() { if(runpc >= cLen) throw "Error inesperado"; return compiled[runpc++]; }
	void RunFunctionCall(int funcnum);

public:
	Expression();
	~Expression();
	inline EvError *Error() { return error; }
	inline EvError *RunError() { return runError; }
	Value *(*NameResolver)(char *name);
	void Compile(char *exp);
	Value *Run();
};

extern ValuePtr (*GlobalNameResolver)(char *name);
extern ValuePtr (*FunctionsNameResolver)(char *name);

class Evaluator
{
	static ValuePtr InternalNameResolver(char *name);

public:
	static Expression *Compile(char *exp);
	static Value *Evaluate(char *exp);
};

#define TK_STARTEXP		'0'
#define TK_ENDEXP		'1'
#define TK_NAME			'2'
#define TK_INTERFUNC	'3'
#define TK_CUSTOMFUNC	'4'

#define TK_INTEGER		'I'
#define TK_DOUBLE		'D'
#define TK_STRING		'S'

char *GetTempString(void);
void CollectValueGarbage(void);
void IntegrateCustomFunctions(FuncDesc *, int cant);

#endif
