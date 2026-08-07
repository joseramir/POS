#include "stdafx.h"

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Constructor
Lic::Lic()
{
	Source = NULL;
	PLicenses = NULL;
	errMessage = new char[300];
	firma = (unsigned *)new unsigned[5];
	Reset();
}

//   Destructor
Lic::~Lic()
{
	Reset();
	delete firma;
	delete errMessage;
	memset(this, 0, sizeof(Lic));
}

//   Resetea al manejador.
void Lic::Reset()
{
	Loaded = false;
	CorrectlySigned = false;
	errMessage[0] = 0;
	
	FreeSource();
	FreePosLicenses();
}

// Libera la memoria usada por el source.
void Lic::FreeSource()
{
	while(Source)
	{
		SourceLine *next = Source->Next;
		delete Source->Source;
		delete Source;
		Source = next;
	}

	Source = NULL;
}

// Libera la memoria usada por las licencias de POS
void Lic::FreePosLicenses()
{
	while(PLicenses)
	{
		PosLicense *next = PLicenses->Next;
		delete PLicenses;
		PLicenses = next;
	}

	PLicenses = NULL;
}

//   Carga un archivo de licencias
bool Lic::LoadLic(char *filename, int initial)
{
	Reset();
	if(this->LoadLicenseFile(filename, false))
	{
		bool res = this->ValidateSign(initial);
		if(res) return true;
	}

	Reset();
	return false;
}



// Carga un archivo de licencias. Durante la carga, valida la sintaxis y si encuentra
// alguna linea incorrecta, detiene la carga. El posible error de carga se obtiene con
// el metodo Error()
bool Lic::LoadLicenseFile(char *filename, bool storeSrc)
{
	int handle = open(filename, O_RDWR | O_TEXT);
	char buffer[3], line[200], *linePtr;
	bool flag = false;
	if(handle >= 0)
	{
		linePtr = line;
		flag = true;
		while(flag && read(handle, buffer, 1) > 0)
		{
			if(buffer[0] == '\n')
			{
				*linePtr = 0;
				flag = ProcessLine(line, storeSrc);
				linePtr = line;
			}
			else
				*linePtr++ = buffer[0];
		}
		close(handle);

		*linePtr = 0;
		flag = ProcessLine(line, storeSrc);
	}

	Loaded = flag;
	return flag;
}

int ValidaParteFecha(char *parte, char sep, char wsep)
{
	if(wsep != ' ' && (wsep != sep)) return 0;
	return atoi(parte);
}

bool Lic::ProcessLine(char *line, bool storeSrc)
{
	// Guarda la linea de codigo fuente, si corresponde.
	if(storeSrc)
	{
		int lineLen = strlen(line);

		SourceLine *src = new SourceLine();
		src->Next = Source;
		src->Source = new char[lineLen + 1];
		STRCPY(src->Source, line);
		Source = src;
	}

	char *ptr = line, token[100];
	SkipSpaces(&ptr);
	if(*ptr == '\'' || *ptr == 0)
		return true;

	char sep = ParseToken(&ptr, token);
	if(sep == '=')
	{
		if(_stricmp(token, "POS") == 0)
		{
			PosLicense *pLic = new PosLicense();

			// Parsea y procesa el cuit
			sep = ParseToken(&ptr, token);
			if(sep != ',' || !ValidarCuit(token))
			{
				sprintf(errMessage, "Cuit Invalido (o falta una coma despues del cuit): '%s'", token);
				return false;
			}
			STRCPY(pLic->Cuit, token);

			// Parsea y procesa el punto de venta
			sep = ParseToken(&ptr, token);
			if(sep != ',' || !ValidarPtoVta(token))
			{
				sprintf(errMessage, "Punto de venta Invalido (o falta una coma): '%s'", token);
				return false;
			}
			STRCPY(pLic->PtoVta, token);

			// Parsea y procesa la fecha tope
			sep = ParseToken(&ptr, token);
			int dia = ValidaParteFecha(token, sep, '/'), mes = 0, aio = 0;
			if(dia > 0)
			{
				sep = ParseToken(&ptr, token);
				mes = ValidaParteFecha(token, sep, '/');
				if(mes > 0)
				{
					sep = ParseToken(&ptr, token);
					aio = ValidaParteFecha(token, sep, ' ');
				}
			}
			if((sep != '\n' && sep != '\'') || !ValidarFecha(dia, mes, aio, token))
			{
				sprintf(errMessage, "Fecha Invalida (o hay basura despues de la fecha): '%s'", token);
				return false;
			}
			STRCPY(pLic->MaxFecha, token);

			// Agrega la nueva licencia a la lista de licencias
			pLic->Next = PLicenses;
			PLicenses = pLic;
		}
		else if(_stricmp(token, "FIRMA") == 0)
		{
			sep = ParseToken(&ptr, token);
			firma[0] = ValidarFirma(token, sep, '*');

			sep = ParseToken(&ptr, token);
			firma[1] = ValidarFirma(token, sep, '*');

			sep = ParseToken(&ptr, token);
			firma[2] = ValidarFirma(token, sep, '*');

			sep = ParseToken(&ptr, token);
			firma[3] = ValidarFirma(token, sep, '*');

			sep = ParseToken(&ptr, token);
			firma[4] = ValidarFirma(token, sep, ' ');

		}
	}
	else
	{
		STRCPY(errMessage, "No encuentro separador '='");
		return false;
	}

	return true;
}

unsigned Lic::ValidarFirma(char *firma, char sep, char wsep)
{
	if((wsep != ' ') && (wsep != sep)) return 0;

	unsigned result = 0;
	while(*firma)
	{
		if(*firma >= '0' && *firma <='9')
			result = result * 16 + (*firma - '0');
		else if(*firma >= 'A' && *firma <= 'F')
			result = result * 16 + (*firma - 'A' + 10);
		else if(*firma >= 'a' && *firma <= 'f')
			result = result * 16 + (*firma - 'a' + 10);
		else return 0;

		firma++;
	}
	return result;
}

bool Lic::ValidarCuit(char *cuit)
{
	char *svcuit = cuit;
	while(*cuit)
	{
		if(*cuit == '-')
			STRCPY(cuit, cuit + 1);
		else if(*cuit >= '0' && *cuit <= '9')
			cuit++;
		else
			return false;
	}
	if(strlen(svcuit) == 11) return true;
	return false;
}

bool Lic::ValidarPtoVta(char *pv)
{
	int maxlenptovta = 4;
	if (strlen(pv) > maxlenptovta)
		maxlenptovta = strlen(pv);
	while(strlen(pv) < maxlenptovta)
	{
		memmove(pv+1, pv, strlen(pv) + 1);
		pv[0] = '0';
	}

	if(strlen(pv) != maxlenptovta) return false;
	while(*pv)
	{
		if(*pv >= '0' && *pv <= '9')
			pv++;
		else
			return false;
	}
	return true;
}

bool Lic::ValidarFecha(int dia, int mes, int aio, char *token)
{
	if(dia <= 0 || dia > 31) return false;
	if(mes <= 0 || mes > 12) return false;
	if(aio <= 2000 || aio > 2100) return false;

	sprintf(token, "%04d%02d%02d", aio, mes, dia);
	return true;
}

void Lic::SkipSpaces(char **ptr)
{
	while(**ptr == ' ' || **ptr == '\t')
		(*ptr)++;
}

char Lic::ParseToken(char **ptr, char *token)
{
	SkipSpaces(ptr);
	while(**ptr)
	{
		if(  (**ptr >= 'a' && **ptr <= 'z') ||
			 (**ptr >= 'A' && **ptr <= 'Z') ||
			 (**ptr >= '0' && **ptr <= '9') ||
			 (**ptr == '_' || **ptr == '-'))
		{
			*token++ = **ptr;
			(*ptr)++;
		}
		else
		{
			*token = 0;
			SkipSpaces(ptr);
			char result = **ptr;
			(*ptr)++;
			return result;
		}
	}

	*token = 0;
	return '\n';
}

bool Lic::ValidateSign(int initial)
{
	unsigned digest[5];
	CalculateSign(initial, digest);
	CorrectlySigned = false;
	if( (digest[0] == firma[0]) && 
		(digest[1] == firma[1]) && 
		(digest[2] == firma[2]) && 
		(digest[3] == firma[3]) && 
		(digest[4] == firma[4]))
	{
		CorrectlySigned = true;
		return true;
	}

	return false;
}

#ifdef RESIGN_ENABLED
bool Lic::ResignFile(char *filename, int initial)
{
	Reset();
	unsigned digest[5];
	if(this->LoadLicenseFile(filename, true))
	{
		CalculateSign(initial, digest);

		SourceLine *src = Source;
		SourceLine *prv = NULL;
		while(src != NULL)
		{
			src->Previous = prv;
			prv = src;
			
			SourceLine *nxt = src->Next;
			if(nxt == NULL) break;
			src = nxt;
		}

		_unlink(filename);
		close(creat(filename, S_IREAD | S_IWRITE));
		int handle = open(filename, O_RDWR | O_TEXT);
		while(src != NULL)
		{
			char *ptr = src->Source;
			SkipSpaces(&ptr);
			if(!_memicmp(ptr, "FIRMA", 5))
				sprintf(errMessage, "\tFIRMA = %08X * %08X * %08X * %08X * %08X\n",
					digest[0], digest[1], digest[2], digest[3], digest[4]);
			else
				sprintf(errMessage, "%s\n", src->Source);

			write(handle, errMessage, strlen(errMessage));
			src = src->Previous;
		}

		close(handle);
		errMessage[0] = 0;
		return true;
	}
	return false;
}
#endif

void Lic::CalculateSign(int initial, unsigned *digest)
{
	SHA1 *sha = new SHA1();

	sprintf(errMessage, "%d", initial);
	
	// Agrega nuestra clave interna.
	sha->Reset();
	sha->Input(errMessage, strlen(errMessage));

	PosLicense *pLic = PLicenses;
	while(pLic != NULL)
	{
		sha->Input(pLic->Cuit, 11);
		sha->Input(pLic->PtoVta, 4);
		sha->Input(pLic->MaxFecha, 8);

		pLic = pLic->Next;
	}
	sha->Result(digest);
	delete sha;
}

bool Lic::ValidatePos(char *cuit, char *pvta, char *dateNow)
{
#ifdef _DEBUG

	if((_stricmp(cuit, "30578411174") == 0) && (_stricmp(pvta, "0193") == 0)) return true;	// Alberdi
	if((_stricmp(cuit, "30708480246") == 0) && (_stricmp(pvta, "0036") == 0)) return true;	// El obrero hasar 715
	if((_stricmp(cuit, "00000000000") == 0) && (_stricmp(pvta, "0000") == 0)) return true;	// El obrero hasar 715

#endif

	if(!Loaded || !CorrectlySigned) return false;
	if(!ValidarCuit(cuit)) return false;
	if(!ValidarPtoVta(pvta)) return false;

	char *ptr = dateNow;
	char token[100];

	char sep = ParseToken(&ptr, token);
	int dia = ValidaParteFecha(token, sep, '/');

	sep = ParseToken(&ptr, token);
	int mes = ValidaParteFecha(token, sep, '/');

	sep = ParseToken(&ptr, token);
	int aio = ValidaParteFecha(token, sep, ' ');

	if(!ValidarFecha(dia, mes, aio, token)) return false;

	PosLicense *pLic = PLicenses;
	while(pLic)
	{
		if(!_stricmp(pLic->Cuit, cuit) && !_stricmp(pLic->PtoVta, pvta))
			return _stricmp(pLic->MaxFecha, token) >= 0;
		pLic = pLic->Next;
	}
	return false;
}

char *Decrypt(char *s)
{
	char key[20];
	char *result = new char[strlen(s) + 1];
	char *keyptr = key, *resptr = result;

	// Construye la key de forma tal que no quede como texto claro en el binario compilado.
	key[0] = 'C';
	key[9] = 'I';
	key[2] = key[0]; 
	key[10] = 'Z';
	key[1] = 'A';
	key[3] = key[1];
	key[4] = key[7] = ' ';
	key[6] = 'E';
	key[11] = 'M';
	key[5] = 'D'; 
	key[8] = 'G';
	key[12] = 'O';
	key[13] = 0;

	// Desencripta el bloque pasado dejandolo como texto limpio
	while(*s)
	{
		*resptr++ = (char)(*s++ ^ *keyptr++);
		if(!*keyptr) keyptr = key;
	}
	*resptr = 0;

	// Destruye la key para que un dump de la memoria no la muestre.
	for(int i=0; i<13; i++)
		key[i] = i * 3;

	return result;
}

// Destruye el texto limpio utilizado, eliminandolo de la memoria para que un posible
// dump no lo muestre.
void DecryptClear(char *s)
{
	char *ptr = s;
	while(*ptr) *ptr++ = 0;
	delete s;
}