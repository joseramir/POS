#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Constantes
#define SIZE_EXPRESSION 2048
#define SIZE_RUNSTACK 32
#define MAX_TEMP_STRINGS 300

// Lista para trazar los 'Value' que se deben liberar.
ValueTraceList valueTrace;

char tempStrings[MAX_TEMP_STRINGS][200];
int tempStringPtr = 0;
char tempMsg[200];

char *E_TooComplex  = "Expresion muy compleja";
char *E_Tipos = "Tipos no coincidentes";

ValuePtr (*GlobalNameResolver)(char *name) = NULL;
ValuePtr (*CustomFunctionResolver)(char *name, ValuePtr *runStack, int *stackPointer);

//   Recupera memoria usada por estructuras 'Value' que quedaron huerfanas.
void CollectValueGarbage()
{
	for(ValueTraceList::iterator it = valueTrace.begin(); it != valueTrace.end(); )
	{
		Value *v = static_cast<Value*>(*it);
		++it;
		if(!v->GetPersistentFlag())
			delete v;
	}
}

//   Retorna una string temporaria, que se utiliza para evitar fugas de memoria
char *GetTempString()
{
	char *res = tempStrings[tempStringPtr++];
	if(tempStringPtr == MAX_TEMP_STRINGS)
		tempStringPtr = 0;
	return res;
}

// Las posibilidades son las siguientes:
// - Si 'newValue' != NULL && lenString >= 0
//	 Crea un nuevo buffer interno de lenString+1 bytes y copia 'newValue' al nuevo buffer.
// - Si 'newValue' == NULL && lenString >= 0
//   Crea un nuevo buffer interno de lenString+1 bytes y lo inicializa con 0.
// - Si 'newValue' != NULL && lenString < 0
//   Asigna simplemente el buffer interno a 'newValue' (usado para evitar duplicaciones innecesarias)
// - Si 'newValue' == NULL && lenString < 0
//   Combinacion de argumentos no permitidas.
void Value::SetStringValue(char *newValue, int lenString)
{
	if(type != 'S') 
		throw "Error Interno: 'S' esperado";
	if(_stringValue != NULL) delete _stringValue;

	// Crea un nuevo buffer interno, si se indico.
	if(lenString > 0)
		_stringValue = new char[lenString + 1];
	else
		_stringValue = NULL;

	if(newValue != NULL)
	{
		if(lenString < 0)
			_stringValue = newValue;
		else
		{
			memmove(_stringValue, newValue, lenString);
			_stringValue[lenString] = 0;
		}
	}
	else if(lenString >= 0)
		memset(_stringValue, 0, lenString + 1);
}

void Value::SetType(char newType)
{
	if(type != newType)
	{
		if(type == 'S') 
		{
			if(_stringValue != NULL) delete _stringValue;
		}

		type = newType;
		_stringValue = NULL;
	}
}

void Value::SetDouble(double d)
{
	SetType('D');
	_doubleValue = d;
}

void Value::SetInteger(int i)
{
	SetType('I');
	_intValue = i;
}

char *Value::ToString(char *format)
{
	char *buff = "";
	char frmtDig = (format == NULL || strlen(format) == 0)? ' ': format[0];
	switch(this->type)
	{
	case 'B':
		buff = GetTempString();
		sprintf(buff, "%s", ValueBoolean()? "TRUE": "FALSE");
		break;

	case 'S':
	case 'C': 
		buff = _stringValue;
		break;

	case 'D':
	case 'N':
		buff = GetTempString();
		switch(frmtDig)
		{
			// Formato con dos decimales siempre
		case 'N': case 'n':
			sprintf(buff, "%.2lf", ValueDouble());
			break;

			// Sin decimales (entero)
		case 'I': case 'i':
			sprintf(buff, "%.0lf", ValueDouble());
			break;

			// Imprime hasta 8 decimales, ignorando ceros a la derecha no validos.
		case 'D': case 'd':
		default:
			sprintf(buff, "%.8lf", ValueDouble());
			char *p;
			for(p = buff + strlen(buff) - 1; p > buff; p--)
			{
				if(*p == '.') { p--; break; }
				if(*p != '0') break;
			}
			*(++p) = 0;
			break;
		}

		break;

	case 'I':
		buff = GetTempString();
		sprintf(buff, "%d", _intValue);
		break;

	default:
		throw "No puede usar 'ToString' para estos valores";
	}
	return buff;
}

/*********************************************************************************************************************/

Value *Evaluator::InternalNameResolver(char *name)
{
	ValuePtr result = NULL;
	if(GlobalNameResolver != NULL) result = (*GlobalNameResolver)(name);
	if(result == NULL) throw "Nombre desconocido";
	return result;
}

Expression *Evaluator::Compile(char *exp)
{
	Expression *result = new Expression;
	result->Compile(exp);
	result->NameResolver = Evaluator::InternalNameResolver;
	return result;
}

Expression::Expression()
{
	memset(this, 0, sizeof(class Expression));
	compiled = new char[SIZE_EXPRESSION];
	run = (Value **)MALLOC(sizeof(Value *) * SIZE_RUNSTACK, "Expression:Ctor");
	tempStringLen = 0;
}

Expression::~Expression()
{
	if(error != NULL)		delete error;
	if(compiled != NULL)	delete compiled;
	if(run != NULL)			FREE(run);
	if(tempStringLen > 0)	delete tempString;
}

void Expression::CompilePush(char code)
{
	if(cLen + 1 > SIZE_EXPRESSION)
		throw E_TooComplex;
	compiled[cLen++] = code;
}

void Expression::CompilePush(int item)
{
	CompilePush(TK_INTEGER);
	if(cLen + sizeof(int) > SIZE_EXPRESSION)
		throw E_TooComplex;

	int *ptr = (int *)(compiled + cLen);
	*ptr = item;
	cLen += sizeof(int);
}

void Expression::CompilePush(double dbl)
{
	CompilePush(TK_DOUBLE);
	if(cLen + sizeof(double) > SIZE_EXPRESSION)
		throw E_TooComplex;

	double *ptr = (double *)(compiled + cLen);
	*ptr = dbl;
	cLen += sizeof(double);
}

void Expression::CompilePush(char *string)
{
	CompilePush(TK_STRING);
	int slen = strlen(string);
	if(cLen + sizeof(short) + slen > SIZE_EXPRESSION)
		throw E_TooComplex;

	short *ptr = (short *)(compiled + cLen);
	*ptr = slen;
	if(slen) memmove(compiled + cLen + sizeof(short), string, slen);
	cLen += sizeof(short) + slen;
}

void Expression::SkipBlank()
{
	while(*srcPtr == ' ' || *srcPtr == '\t')
		srcPtr++;
}

void Expression::CompileOperand()
{
	SkipBlank();
	if(*srcPtr == '(')
	{
		srcPtr++;
		CompileSubExpression();
		SkipBlank();
		if(*srcPtr != ')')
			throw "Falta parentesis de cierre ')'";
		srcPtr++;
		return;
	}

	if((*srcPtr >= '0' && *srcPtr <= '9') || *srcPtr == '-' || *srcPtr == '.')
	{
		CompileNumber();
		return;
	}

	if(*srcPtr == '\"')
	{
		CompileString();
		return;
	}

	if(isalpha(*srcPtr))
	{
		CompileName();
		return;
	}

	if(*srcPtr == 0)
		throw "Faltan operandos";
	else
		throw "Operando desconocido";
}

void Expression::CompileName()
{
	char *first = srcPtr;
	while(isalnum(*srcPtr) || *srcPtr == '_') srcPtr++;
	int nLen = srcPtr - first;

	char name[33]; 
	if(nLen > 32) nLen = 32;
	memmove(name, first, nLen);
	name[nLen] = 0;
	_strupr(name);

	SkipBlank();
	if(*srcPtr == '(')
	{
		// Si despues del nombre hay un  parentesis abierto, entonces se trata de la invocacion a una funcion y no
		// de la referencia a una variable por su nombre.
		int parCount = 0;
		srcPtr++;
		SkipBlank();
		while(true)
		{
			if(*srcPtr == ')') break;

			CompileSubExpression();
			parCount++;
			SkipBlank();
			if(*srcPtr == ',') srcPtr++;
			else if(*srcPtr != ')') throw "Esperaba ',' o ')'";
		}
		srcPtr++;

		// Ahora, busca en las tablas de que funcion se trata.
		int i = 0;
		for(; i<nFuncs; i++)
		{
			FuncDesc func = Functions[i];
			if(!strcmp(func.Name, name))
			{
				// Encontro la funcion. Mira si el numero y tipo de parametros ingresados son los esperados
				// por la funcion.
				int defArgCount = func.ArgCount;
				if(defArgCount == 1 && func.ArgType[0] == 'Z') defArgCount = 0;
				if(defArgCount != 0 && defArgCount != parCount) throw "Numero invalido de parametros para funcion";
				CompilePush(TK_INTERFUNC);
				short *ptr = (short *)(compiled + cLen);
				*ptr = i;
				cLen += sizeof(short);
				break;
			}
		}

		// Si no la encuentra en las funciones, entonces presupone que se trata de una funcion usuario.
		if(i >= nFuncs)
			CompilePushName(TK_CUSTOMFUNC, name);
	}
	else
		CompilePushName(TK_NAME, name);
}

void Expression::CompilePushName(char token, char *name)
{
	int nLen = strlen(name);
	if(cLen + nLen + sizeof(short) + 1 > SIZE_EXPRESSION)
		throw E_TooComplex;

	CompilePush(token);
	short *ptr = (short*)(compiled + cLen);
	*ptr = nLen;
	STRCPY(compiled + cLen + sizeof(short), name);
	cLen += nLen + sizeof(short) + 1;
}

void Expression::CompileNumber()
{
	bool isFloat = false;
	char temp[100];
	int tempPtr = 0;

	if(*srcPtr == '-') temp[tempPtr++] = *srcPtr++;
	while(*srcPtr)
	{
		if(tempPtr + 1 >= 100) throw "Numero de mas de 100 digitos";
		if(*srcPtr == '.')
		{
			if(isFloat) throw "Caracter '.' inesperado";
			isFloat = true;
		}
		else if(*srcPtr < '0' || *srcPtr > '9')
			break;

		temp[tempPtr++] = *srcPtr++;
	}

	temp[tempPtr] = 0;
	if(tempPtr > 8) isFloat = true;
	if(isFloat)
		CompilePush(atof(temp));
	else
		CompilePush(atoi(temp));
}

void Expression::CompileString()
{
	char comi = *srcPtr++;
	char *first = srcPtr;
	while(*srcPtr && *srcPtr != comi)
		srcPtr++;

	int slen = srcPtr - first;
	if(tempStringLen <= slen)
	{
		if(tempStringLen > 0) delete tempString;
		tempStringLen = (((slen + 16) / 32) + 1) * 32;	// Ubica 'chunks' de 32 bytes siempre
		tempString = new char[tempStringLen];
	}
	memmove(tempString, first, slen);
	tempString[slen] = 0;
	CompilePush(tempString);
	srcPtr++;
}

void Expression::CompileSubExpression()
{
	int priorities[100];
	char operators[100];
	int locsp = 0;

	while(true)
	{
		CompileOperand();
		SkipBlank();

		int priority;
		bool salir = false;
		switch(*srcPtr)
		{
			// Operadores logicos AND y OR
		case 'o': case 'O':
			if(*(srcPtr + 1) == 'R' || *(srcPtr + 1) == 'r')
				srcPtr++;
			priority = 600;
			*srcPtr = 'O';
			break;

		case 'a': case 'A':
			if(((*srcPtr + 1) == 'N' || *(srcPtr + 1) == 'n') &&
				((*srcPtr + 2) == 'D' || *(srcPtr + 2) == 'd'))
			{
				srcPtr += 2;
				priority = 500;
				*srcPtr = 'Y';
			}
			else
				salir = true;
			break;

		case 'y': case 'Y':
			priority = 500;
			*srcPtr = 'Y';
			break;

			// Operadores de comparacion
		case '#':
		case '=':
			priority = 400;
			break;
		case '<':
			priority = 400;
			if(*(srcPtr + 1) == '>')
			{
				srcPtr++;
				*srcPtr = '#';
			}
			else if(*(srcPtr + 1) == '=')
			{
				srcPtr++;
				*srcPtr = '$';
			}
			break;
		case '>':
			priority = 400;
			if(*(srcPtr + 1) == '=')
			{
				srcPtr++;
				*srcPtr = '&';
			}
			break;

			// Operadores binarios aritmeticos y de cadena.
		case '+': priority = 200; break;
		case '-': priority = 200; break;
		case '*': priority = 100; break;
		case '/': priority = 100; break;
		case '%': priority = 80; break;
		default: salir = true;
		}
		if(salir) break;

		// Manda al codigo compilado aquellos operadores con prioridad menor o igual a la del operador que
		// estamos compilando.
		while(locsp > 0 && priorities[locsp - 1] <= priority)
			CompilePush(operators[--locsp]);

		// Agrega a nuestro 'stack' de operadores, el operador recien detectado
		priorities[locsp] = priority;
		operators[locsp++] = *srcPtr++;
	}

	// Vacia en orden todos los operadores en el 'stack' que todavia no se descargaron al codigo compilado.
	while(--locsp >= 0)
		CompilePush(operators[locsp]);
}

void Expression::Compile(char *exp)
{
	cLen = 0;
	error = NULL;
	srcPtr = exp;
	try
	{
		CompilePush(TK_STARTEXP);
		CompileSubExpression();
		if(*srcPtr != 0) throw "Operador Invalido o hay basura al final de la expresion";
		CompilePush(TK_ENDEXP);
	}
	catch(char *msg)
	{
		error = new EvError(msg);
	}
}

Value *Expression::Run()
{
	if(error != NULL)
	{
		runError = new EvError("Hay errores de compilacion");
		return NULL;
	}

	runpc = runsp = 0;
	runError = NULL;
	try
	{
		if(RunItem() != TK_STARTEXP) throw "Falta marca de comienzo de expresion";

		// Variables del loop
		Value *right, *left;		// Los valores izq y der a utilizar con un operador binario
		char *buffer;				// Puntero temporario para extraer codigo del codigo compilado
		short slen;					// Largo de una string en el codigo compilado
		char *name;					// Puntero a un nombre extraido del codigo compilado
		int funcnum;				// El numero de funcion interna a ejecutar
		bool bResult;				// El resultado de la ejecucion de una funcion
		int compRes;				// El resultaod de la compilacion

		char item = RunItem();		// Extrae un item del codigo compilado
		while(item != TK_ENDEXP)
		{
			void *iPtr = (compiled + runpc);
			switch(item)
			{
			case TK_INTEGER:
				if(runpc + (int)sizeof(int) >= cLen) throw "Falta valor entero";
				run[runsp++] = new Value(*((int *)iPtr));
				runpc += sizeof(int);
				break;

			case TK_DOUBLE:
				if(runpc + (int)sizeof(double) >= cLen) throw "Falta valor double";
				run[runsp++] = new Value(*((double *)iPtr));
				runpc += sizeof(double);
				break;

			case TK_STRING:
				if(runpc + (int)sizeof(short) >= cLen) throw "Falta largo de la string";
				slen = *((short *)iPtr);
				if(runpc + (int)sizeof(short) + slen >= cLen) throw "La string no esta completa";
				run[runsp++] = new Value(compiled + runpc + (int)sizeof(short), slen);
				runpc += sizeof(short) + slen;
				break;

			case TK_CUSTOMFUNC:
			case TK_NAME:
				if(runpc + (int)sizeof(short) >= cLen) throw "Falta largo del nombre";
				slen = *((short *)iPtr);
				name = compiled + runpc + sizeof(short);
				runpc += slen + sizeof(short) + 1;
				if(item == TK_NAME)
				{
					if(NameResolver == NULL) throw "El resolvedor de nombres de variables no esta instalado";
					ValuePtr tres = (*NameResolver)(name);
					if(tres == NULL)
					{
						sprintf(tempMsg, "La variable '%s' no fue definida", name);
						throw tempMsg;
					}

					// Debe crear un 'Value' nuevo, porque el resolvedor PODRIA retornar directamente el valor de la variable o campo.
					// (Nuestro resolvedor 'standard' no hace eso, pero algun resolvedor 'custom' podria hacerlo...)
					// Aunque no eliminamos a 'tres', no existe peligro de fugas de memoria, si periodicamente se llama a 
					// 'CollectValueGarbage' para eliminar los 'Value' que quedaron sin uso.
					run[runsp++] = new Value(tres);
				}
				else
				{
					if(CustomFunctionResolver == NULL)
						throw "Funciones custom no estan soportadas";
					ValuePtr result = (*CustomFunctionResolver)(name, run, &runsp);
					run[runsp++] = new Value(result);
				}
				break;

			case TK_INTERFUNC:
				funcnum = *((short *)iPtr);
				runpc += sizeof(short);
				RunFunctionCall(funcnum);
				break;

			case 'O':
			case 'Y':
				right = run[--runsp];
				left = run[runsp - 1];
				bResult = (item == 'O')? 
					(left->ValueBoolean() || right->ValueBoolean()) :
				(left->ValueBoolean() && right->ValueBoolean());
				left->SetType('B');
				left->_intValue = bResult? 1: 0;
				delete right;
				break;

			case '=':
			case '#':
			case '>':
			case '&':		// >=
			case '<':
			case '$':		// <=
			case '+':
			case '-':
			case '*':
			case '/':
			case '%':
				right = run[--runsp];
				left = run[--runsp];
				if(left->Type() == 'S')
				{
					if(item != '+')
						throw "Operacion no permitida para cadenas";

					if(right->Type() != 'S') throw E_Tipos;
					char *s1 = left->ValueString(), *s2 = right->ValueString();
					if(item == '+')
					{
						buffer = new char[strlen(s1) + strlen(s2) + 1];
						STRCAT(STRCPY(buffer, s1), s2);
						left->SetStringValue(buffer, -1);	// -1 = Evita duplicaciones.
					}
					else
					{
						compRes = strcmp(s1, s2);
						switch(item)
						{
						case '=': bResult = (compRes == 0); break;
						case '#': bResult = (compRes != 0); break;
						case '>': bResult = (compRes > 0);  break;
						case '&': bResult = (compRes >= 0); break;	// >=
						case '<': bResult = (compRes < 0);  break;
						case '$': bResult = (compRes <= 0); break;	// <=
						default: bResult = false; break;
						}

						left->SetType('B');
						left->_intValue = bResult? 1: 0;
					}
				}
				else
				{
					if(left->Type() == 'D' || right->Type() == 'D')
					{
						if(left->Type() != 'D' && left->Type() != 'I') throw E_Tipos;
						if(right->Type() != 'D' && right->Type() != 'I') throw E_Tipos;
						double n1 = (left->Type() == 'D')? left->_doubleValue: (double)left->_intValue;
						double n2 = (right->Type() == 'D')? right->_doubleValue: (double)right->_intValue;
						left->SetType('D');
						switch(item)
						{
						case '+': left->_doubleValue = n1 + n2; break;
						case '-': left->_doubleValue = n1 - n2; break;
						case '*': left->_doubleValue = n1 * n2; break;
						case '/': if (n2 == 0)
								  {
									  this->error = new EvError("Division por cero");
									  this->runError = new EvError("Division por cero");
									  left->_doubleValue = 0;
								  }
								  else
									  left->_doubleValue = n1 / n2; 
							break;
						case '%': if (n2 == 0)
								  {
									  this->error = new EvError("Division por cero");
									  this->runError = new EvError("Division por cero");
									  left->_doubleValue = 0;
								  }
								  else
									  left->_doubleValue = (double)((int) n1 % (int) n2);
							break;
						default:
							switch(item)
							{
							case '=': bResult = (n1 == n2); break;
							case '#': bResult = (n1 != n2); break;
							case '<': bResult = (n1 <  n2); break;
							case '$': bResult = (n1 <= n2); break;
							case '>': bResult = (n1 >  n2); break;
							case '&': bResult = (n1 >= n2); break;
							}
							left->SetType('B');
							left->_intValue = bResult? 1: 0;
							break;
						}
					}
					else
					{
						if(left->Type() != 'I' || right->Type() != 'I') throw E_Tipos;
						switch(item)
						{
						case '+': left->_intValue += right->_intValue; break;
						case '-': left->_intValue -= right->_intValue; break;
						case '*': left->_intValue *= right->_intValue; break;
						case '/': 
							if (right->_intValue == 0)
							{
								this->error = new EvError("Division por cero");
								this->runError = new EvError("Division por cero");
								left->_intValue = 0;
							}
							else
								left->_intValue /= right->_intValue; 
							break;
						case '%': 
							if (right->_intValue == 0)
							{
								this->error = new EvError("Division por cero");
								this->runError = new EvError("Division por cero");
								left->_intValue = 0;
							}
							else
								left->_intValue %= right->_intValue; 
							break;
						default:
							switch(item)
							{
							case '=': bResult = (left->_intValue == right->_intValue); break;
							case '#': bResult = (left->_intValue != right->_intValue); break;
							case '<': bResult = (left->_intValue <  right->_intValue); break;
							case '$': bResult = (left->_intValue <= right->_intValue); break;
							case '>': bResult = (left->_intValue > right->_intValue); break;
							case '&': bResult = (left->_intValue >= right->_intValue); break;
							}
							left->SetType('B');
							left->_intValue = bResult? 1: 0;
							break;
						}
					}
				}

				delete right;
				run[runsp++] = left;
				break;

			default:
				throw "Token desconocido en runtime";
			}
			item = RunItem();
		}

		if(runsp == 0) throw "La expresion esta vacia";
		if(runsp != 1) throw "La expresion genera mas de 1 resultado";
		return run[0];
	}
	catch(char *msg)
	{
		runError = new EvError(msg);
		return NULL;
	}
}

void Expression::RunFunctionCall(int funcnum)
{
	FuncDesc func = Functions[funcnum];
	ValuePtr *args = &run[runsp - func.ArgCount];
	runsp -= func.ArgCount;

	int defArgCount = func.ArgCount;
	if(defArgCount = 1 && func.ArgType[0] == 'Z') defArgCount = 0;
	for(int i=0; i<defArgCount; i++)
	{
		ValuePtr arg = args[i];
		switch(func.ArgType[i])
		{
		case 'I':
			if(arg->Type() == 'D')
			{
				arg->SetType('I');
				arg->_intValue = (int)arg->_doubleValue;
			}
			else if(arg->Type() != 'I')
				throw "Esperaba un ARG entero";
			break;

		case 'D':
			if(arg->Type() == 'I')
			{
				arg->SetType('D');
				arg->_doubleValue = (double)arg->_intValue;
			}
			else if(arg->Type() != 'D')
				throw "Esperaba un ARG double";
			break;

		case 'C':
		case 'S':
		case 'B':
			if(arg->Type() != func.ArgType[i])
				throw "ARG incompatible";
			break;

		case 'U':		// No definido
			break;

		default:		// 'U'
			break;
		}
	}

	FunctionsNameResolver = this->NameResolver;
	ValuePtr result = (*func.Run)(args);
	run[runsp++] = result;
}

