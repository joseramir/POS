//
//   Enum.c
//   Al cargar un archivo de codigo en la memoria, este programa lo
//   preprocesa para:
//
//      - Remover lineas en blanco o comentarios.
//      - Cargar definicion de constantes.
//      - Transformar constantes en sus recipientes.
//      - Convierte en minusculas TODO lo que no este entre comillas.

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

#define MAXCONST 100
struct constan_ {
	char name[17];          // Nombre
	int  lend;              // Largo de la definicion.
	char *def;              // definicion impuesta.
	char id;                // Id del programa que establecio la const.
} constan[MAXCONST];

#define SIZELINE 200
char r[SIZELINE];
char l[SIZELINE];
char *t;
char token[50];
char useId;
int  cConst;
char datadir[] = "pos\\";
FILE *EnumH;

//   Prototipos  
char *SkipSpaces(char *);
char *GetToken(void);
int  StartFile(char *, char);
void CloseFile(char);
char *GetPLine(void);
char *GetPToken(void);
void AddConst(void);

//   Abre un archivo y lo prepara para entrada desde el.
int StartFile(char *file, char id)
{
	useId = id;
	STRCAT(STRCPY(r, datadir), file);
    EnumH = fopen(r, "r");
    if(!EnumH)
	{
		printf("No encuentro el archivo '%s'.\n", r);
		_getch();
		return 1;
	}
	return 0;
}

//   Cierra el archivo usado para entrada. Si el flag esta a algun valor,
//   entonces ELIMINA las constantes que esten definidas con ese id.
void CloseFile(char id)
{
	fclose(EnumH);
}

//   Agrega una constante a la tabla de constantes.
int ConstComp1(const void *c1, const void *c2)
{
    return strcmp(((struct constan_ *) c1)->name,
                  ((struct constan_ *) c2)->name);
}

void AddConst()
{
	static int lend;
	static struct constan_ *c;

	t = SkipSpaces(t + 5);          // Saltea espacios y el 'const'.
	GetToken();                     // Nombre de la definicion.
	token[16] = '\0';               // Si el nombre es muy largo.
	t = SkipSpaces(t);              // Busca el comienzo de la definicion.
	lend = strlen(t);               // Largo de la definicion
	if(cConst + 1 > MAXCONST)
	{
		printf("Sin lugar para mas constantes.\n");
		_getch();
		return;
	}
	c = &constan[cConst++];
	STRCPY(c->name, _strlwr(token));
	c->lend = lend;
	c->id = useId;
    c->def = (char *) MALLOC(lend + 1, "AddConst");
    if(c->def)
		STRCPY(c->def, t);
	qsort(constan, cConst, sizeof(struct constan_), ConstComp1);
}

//   Procesa una linea, extrayendo y limpiando a la misma.
char *GetPLine()
{
	static char *s;

	do {
			if(!fgets(l, SIZELINE, EnumH))
				return 0;

			l[strlen(l) - 1] = '\0';                 // Remueve el \n
			while(isspace(l[0])) STRCPY(l, l+1);     // Quita espacios.
			for(s = l + strlen(l) - 1; s >= l; s--)	 // Quita comentarios.
				if(*s == ';' || *s == '\"' || *s == '\'')
				{
					if(*s == ';' || *s == '\'') *s = 0;
					break;
				}
			for(s = l + strlen(l) - 1; s >= l; s--)	 // Quita espacios
				if(!isspace(*s))
					break;
			if(++s >= l) *s = '\0';
			if(!_memicmp(l, "const", 4))
			{
				STRCPY(t = r, l);
				AddConst();
				l[0] = '\0';
			}
	} while(!l[0] || l[0] == '\'');
	return STRCPY(t = r, l);
}

//   Saltea espacios en blanco en la string que se indica.
char *SkipSpaces(register char *s)
{
	while(isspace(*s)) s++;
	return s;
}

//   Extrae un token sin pre-procesar.
char *GetToken()
{
	register char *m, *s;
	static int ccoma, inString;

	s = SkipSpaces(t);
	inString = 0;
	token[0] = 0;
	if(!*s) return token;
	for(m = token; *s; *m++ = *s++)
	{
		if(*s == '\"')
			if(inString)
				break;
			else
			{
				inString = 1;
				continue;
			}
		if(!inString && ((*s == ',') || isspace(*s)))
			break;
	}
	if(inString && (*s == '\"')) 
	{
		t = ++s;
		*m = 0;
		STRCPY(token, token + 1);
		return token;
	}
	ccoma = (*s == ',');
	for(; *s; s++)
		if(!isspace(*s))
			if((*s == ',' && !ccoma) || (*s != ','))
				break;
			 else
				ccoma = 1;
	t = s;
	*m = '\0';
	return token;
}

//   Igual que GetToken, pero preprocesandolo.
int ConstComp2(const void *s, const void *co)
{
    return strcmp((char *) s, ((struct constan_ *) co)->name);
}

char *GetPToken()
{
	static constan_ *co;

	_strlwr(GetToken());
	if(!strlen(token)) return token;
	co = (constan_ *) bsearch(token, constan, cConst, sizeof(struct constan_),
				              ConstComp2);
	if(co) return co->def;
	return token;
}

