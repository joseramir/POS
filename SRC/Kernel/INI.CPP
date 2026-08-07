/*
**   Ini.c
**   Permite el manejo de archivos de inicializacion, donde se graban en texto las distinas secuencias default.
**
**   En estos archivos, la sintaxis es:
**
**      [NombreDelGrupo]
**          Parametro=valor
**          Parametro=valor
**          ...
**
**   El nombre de los grupos va encerrado entre corchetes y NO es sensitivo a las minusculas.
**
**   El signo igual DEBE existir o se generara un error de sintaxis y la linea se ignorara.
**   Al nombre del parametro, se le removeran los espacios a la izquierda y derecha y se ignoraran minusculas.
**
**   Al valor, tambien se le quitaran espacios a la izquierda y derecha.
**
**   Lineas en blanco o cuyo primer caracter no blanco sea ';' seran tratadas como comentarios y se ignoraran.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//  directorio default donde ser buscaran los 'ini'.
char iniDir[] = "pos\\";

// -------------- Parametros ---------------- 
static int IniP;
xFile IniH;
static char *seccName[200]; //antes tenia 64
static char *line = new char[256];
static long *seccPos = new long[200]; //antes tenia 64
static char *secc = new char[40];
static char *vari = new char[40];

// -------------- Prototipos ---------------- 
int GetIniLine(void);

//	CloseIni()
//	Cierra el archivo ini y libera la memoria utilizada.
void CloseIni()
{
	static int i;

	if(IniP)
	{
		IniH.xClose();
		IniP = 0;
		for(i = 0; i < 32; i++)
		{
			if(seccPos[i])
			 {
                FREE(seccName[i]);
				seccPos[i] = 0l;
			 }
		}
	}
}


int CheckExisteIni(char *fn)
{
	char lifn[100], *p;
	//long svpos;

	STRCAT(STRCPY(lifn, iniDir), fn);
	if(strchr(lifn, '.') == NULL) STRCAT(lifn, ".ini");

	if(IniP) CloseIni();
	if(IniH.xOpen(lifn, O_RDWR, SH_DENYRW) < 0)
		return 1;
	else
		IniH.xClose();
	return 0;
}

//	OpenIni(char *s)
//	Abre un archivo INI y lo prepara para ser leido.
//	Retorna 1 si ocurrio algun error, sino, 0.
int OpenIni(char *fn)
{
	static char ifn[100], *p;
	static long svpos;

	STRCAT(STRCPY(ifn, iniDir), fn);
	if(strchr(ifn, '.') == NULL) STRCAT(ifn, ".ini");

	if(IniP) CloseIni();
	if(IniH.xOpen(ifn, O_RDWR, SH_DENYRW) < 0)
		return 1;

	IniH.xCache(CACHE_LOAD_FILE);
	IniP = 1;
    while(!IniH.xEof())
	{
		svpos = IniH.xTell();
		if(GetIniLine()) break;          // Si no pudo leer la linea.
		Trim(line);
		if(line[0] != '[') continue;
		STRCPY(line, line + 1);         //  Remueve '['
        for(p = line; *p && *p != ']'; p++)
            ;
		if(!*p)
		{
			printf("El nombre de seccion '%s' NO termina con ']'.\n", line);
			_getch();
			continue;
		}
		*p = '\0';
        seccName[IniP - 1] = (char *) MALLOC(strlen(Trim(line)) + 1, "OpenIni");
        if(!seccName[IniP - 1])
		{
			printf("Sin memoria para mas secciones, desde '%s'.\n", line);
			_getch();
			break;
		}
		STRCPY(seccName[IniP - 1], _strupr(line));
		seccPos[IniP - 1] = svpos;
		IniP++;
	}

	if(!(--IniP))
	{
		printf("No hay secciones validas en '%s'.\n", ifn);
		_getch();
		IniH.xClose();
	}
	return 0;
}

//	 GetIniLine()
//	 Toma una linea del archivo de IniH, de hasta 254 caracteres, y la
//	 coloca en el buffer 'line'.
//	 Si no hay archivo ini abierto, en line coloca la linea vacia.
//	 Si ocurre algun error al leer del archivo, vuelve con 1 sino con 0.
int GetIniLine()
{
	static char *p, ch;
	static int c;

	if(!IniP)
	{
		line[0] = '\0';
		return 0;
	}
    for(p = line, c = 0; !IniH.xEof();)
	{
		if(IniH.xRead(&ch, 1) <= 0)
			return 1;
		if(ch == '\n')
			break;
		if(ch != '\r' && c < 255)
		{
			*p++ = ch;
			c++;
		}
	}
	*p = '\0';
	return 0;
}

//	Las siguientes funciones utilizan a 'ExtractIni' para extraer diver-
//	sos valores ya formateados de forma predeterminada.
int ExtractIniSiNo(char *secc, char *vari, int def)
{
	char c;

	ExtractIni(secc, vari);
	c = *Trim(line);
	if(strlen(line) == 0) return def;
	if(c == 's' || c == 'S' || c == '1' || c == 'V' || c == 'v')
		return 1;
	return 0;
}
int ExtractIniSiNo(char *secc, char *vari)
{
	return ExtractIniSiNo(secc, vari, 0);
}

long ExtractIniLong(char *secc, char *vari, long def)
{
	char c;

	ExtractIni(secc, vari);
	c = *Trim(line);
	if(c)
		return atol(line);
	return def;
}

long ExtractIniLong(char *secc, char *vari)
{
	ExtractIni(secc, vari);
	return atol(line);
}

char *ExtractIniChar(char *secc, char *vari)
{
	ExtractIni(secc, vari);
	return line;
}

char *ExtractIniStr(char *secc, char *vari)
{
	ExtractIni(secc, vari);
	char *res = STRDUP(line, "ExtractIniStr");
	Trim(res);
	return res;
}

unsigned ExtractIniHexa(char *secc, char *vari)
{
	register unsigned result;
	register char *p;

	ExtractIni(secc, vari);
	for(p = line, result = 0; *p; p++, result *= 16)
		if(*p <= '9')
			result += (*p - '0');
		 else
		  {
			if((*p <= 'F') && (*p >= 'A'))
			   result += (*p - 'A' + 10);
			 else
			  {
			   if((*p <= 'f') && (*p >= 'a'))
				  result += (*p - 'a' + 10);
			  }
		  }
	return result;
}

//   Verifica si la seccion cuyo nombre se pasa, esta definida en el
//	 archivo INI que se esta verificando.                     
int CheckSecc(char *secc)
{
	int i;

	_strupr(secc);
	for(i = 0; i < IniP; i++)
		if(!strcmp(secc, seccName[i]))
			return 0;
	return 1;
}

//	 ExtractIni(Seccion, Variable)
//	 Rutinas que extraen valores del archivo ini. Todas se basan en la
//	 siguiente funcion generica.
//	 El Resultado queda en 'line'.
char *ExtractIni(char *Usecc, char *Uvari)
{
	int i, found;
	register char *p;
	char *ver = line;	

	line[0] = '\0';
	_strupr(STRCPY(secc, Usecc));
	_strupr(STRCPY(vari, Uvari));
	for(i = 0; i < IniP; i++)
		if(!strcmp(secc, seccName[i]))
			break;
	if(i >= IniP) return line;
	IniH.xLseek(seccPos[i], SEEK_SET);
	GetIniLine();	
	found = 0;
    while(!IniH.xEof())
	{		
		if(GetIniLine()) break;
		Trim(line);
		if(line[0] == ';' || !line[0]) continue;
		if(line[0] == '[') break;
        for(p = line; *p && *p != '='; p++)
            ;		
		if(*p)
		{
			*p = '\0';
			_strupr(Trim(line));
            found = !strcmp(line, vari);
            if(found) break;
		}
	}
	if(found)
		STRCPY(line, p+1);
	 else
		line[0] = '\0';	
	/*if (!strcmp(Usecc, "promo65")){
		WriteBootLog(Usecc);
		WriteBootLog(line);
	}*/
	return line;
}
