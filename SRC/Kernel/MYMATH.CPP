/*
 *    MyMath.c
 *    algunas rutinas matematicas.
*/

#include <stdafx.h>

using namespace std;

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//  Resultado donde queda la transformacion de un numero a un monto "en letras"
char enlet_Reng1[50];
char enlet_Reng2[50];
char enlet_Reng3[50];
char enlet[500];

static int SubAtoi(char *str, int start, int len)
{
	int res = 0;
	int end = start + len;
	while(start < end && str[start] == ' ')
		start++;

	if(str[start] == ' ') return 0;
	while(start < end)
	{
		char ch = str[start++] - '0';
		if(ch < 0 || ch >= 10) break;
		res = res * 10 + ch;
	}

	return res;
}

static char *wn_c[10];
static char *wn_d[10];
static char *wn_u[10];
static char *wn_du[10];
static bool wn_iniciado;

static void PrepareLetras()
{
	if(wn_iniciado) return;
	wn_iniciado = true;

	wn_c[0] = "";
	wn_c[1] = "Ciento ";
	wn_c[2] = "Doscientos ";
	wn_c[3] = "Trescientos ";
	wn_c[4] = "Cuatrocientos ";
	wn_c[5] = "Quinientos ";
	wn_c[6] = "Seiscientos ";
	wn_c[7] = "Setecientos ";
	wn_c[8] = "Ochocientos ";
	wn_c[9] = "Novecientos ";

	wn_d[0] = "";
	wn_d[1] = "Dieci";
	wn_d[2] = "Veinti";
	wn_d[3] = "Treinta ";
	wn_d[4] = "Cuarenta ";
	wn_d[5] = "Cincuenta ";
	wn_d[6] = "Sesenta ";
	wn_d[7] = "Setenta ";
	wn_d[8] = "Ochenta ";
	wn_d[9] = "Noventa ";

	wn_u[0] = "";
	wn_u[1] = "Un ";
	wn_u[2] = "Dos ";
	wn_u[3] = "Tres ";
	wn_u[4] = "Cuatro ";
	wn_u[5] = "Cinco ";
	wn_u[6] = "Seis ";
	wn_u[7] = "Siete ";
	wn_u[8] = "Ocho ";
	wn_u[9] = "Nueve ";

	wn_du[0] = "Diez ";
	wn_du[1] = "Once ";
	wn_du[2] = "Doce ";
	wn_du[3] = "Trece ";
	wn_du[4] = "Catorce ";
	wn_du[5] = "Quince ";
	wn_du[6] = "Dieciseis ";
	wn_du[7] = "Diecisiete ";
	wn_du[8] = "Dieciocho ";
	wn_du[9] = "Diecinueve ";
}

static char *AuxEnLetras(int nro)
{
	static char res[50];

	if(nro == 100)
		STRCPY(res, "Cien ");
	else
	{
		int c = (nro / 100);
		int d = (nro - (c * 100)) / 10;
		int u = (nro % 10);
		int du = u + d * 10;

		STRCPY(res, wn_c[c]);
		if(du >= 10 && du <= 20)
			STRCAT(res, wn_du[du - 10]);
		else
		{
			STRCAT(res, wn_d[d]);
			if(u != 0 && du >= 30 && du <= 99)
			{
				while(res[strlen(res) - 1] == ' ')
					res[strlen(res) - 1] = 0;
				STRCAT(res, " y ");
			}
			STRCAT(res, wn_u[u]);
		}
	}

	return res;
}

void SubStringCopy(char *src, char *dst, int start, int len)
{
	while(len--)
		*dst++ = src[start++];
	*dst = 0;
}

//  Genera a partir de un numero una "frase" que describe al numero en palabras.
void EnLetras(double num, char *postfix)
{
	char varName[50];

	PrepareLetras();

    char nro[50];
	sprintf(nro, "%012.2lf", num);
    int t3 = SubAtoi(nro, 0, 3);
	int t2 = SubAtoi(nro, 3, 3);
	int t1 = SubAtoi(nro, 6, 3);
	int cvos = SubAtoi(nro, 10, 2);

    STRCPY(enlet, "");
	if(t3 == 1)
		STRCAT(enlet, "Un Millon ");
	else
		if(t3 != 0)
			STRCAT(STRCAT(enlet, AuxEnLetras(t3)), "Millones ");

    if(t2 != 0)
		STRCAT(STRCAT(enlet, AuxEnLetras(t2)), "Mil ");

	if(t1 != 0)
	{
		char *t = AuxEnLetras(t1);
		if((t1 % 10) == 1 && (t1 % 100) != 11)
		{
			t[strlen(t) - 1] = 0;
			STRCAT(t, "o ");
			STRCAT(enlet, t);
		}
		else
			STRCAT(enlet, t);
	}
	else if(t2 == 0 && t3 == 0)
		STRCAT(enlet, "Cero ");

	if(cvos != 0)
	{
		char cvosStr[10];
		STRCAT(STRCAT(STRCAT(enlet, "Con "), _itoa(cvos, cvosStr, 10)), " Cvos");
	}

	// Elimina espacios innecesarios a la derecha.
	while(enlet[strlen(enlet) - 1] == ' ')
		enlet[strlen(enlet) - 1] = 0;
	STRCAT(enlet, ".-");

	char *rngs[3];
	rngs[0] = enlet_Reng1; rngs[1] = enlet_Reng2; rngs[2] = enlet_Reng3;
	STRCPY(rngs[0], "");
	STRCPY(rngs[1], "");
	STRCPY(rngs[2], "");

	// Separa el mensaje en 3 renglones de no mas de 40 caracteres cada uno
	int posi = 40, index = 0, ant = 0;
	while(true)
	{
		if(index > 2) break;
		if(strlen(enlet) - ant < 40)
		{
			SubStringCopy(enlet, rngs[index], ant, posi - ant);
			break;
		}

		while(posi > ant)
		{
			if(enlet[posi] == ' ') break;
			posi--;
		}

		SubStringCopy(enlet, rngs[index++], ant, posi - ant);
		ant = posi + 1;
		posi += 40;
	}

	STRCAT(STRCAT(STRCPY(varName, "@"), postfix), "@");
	VoucherVar(varName, num, true);
	STRCAT(STRCAT(STRCPY(varName, "@LET1_"), postfix), "@");
	VoucherVar(varName, enlet_Reng1);
	STRCAT(STRCAT(STRCPY(varName, "@LET2_"), postfix), "@");
	VoucherVar(varName, enlet_Reng2);
	STRCAT(STRCAT(STRCPY(varName, "@LET3_"), postfix), "@");
	VoucherVar(varName, enlet_Reng3);
	STRCAT(STRCAT(STRCPY(varName, "@LETRAS_"), postfix), "@");
	VoucherVar(varName, enlet);
}

/*
 *   La siguiente funcion "corre" una cadena dos lugares a la izquierda.
 *   Ojo. Los dos caracteres de la izquierda se pierden.
*/

char *ml(char *s)
{
	if(strlen(s)<3) return s;
	STRCPY(s, s+2);
	return STRCAT(s, "**");
}

/*
 *   El 'atof' de la libreria no hace lo que de ella esperaba. Por esto
 *   reescribi la siguiente 'atof_' para que se comporte de forma mas
 *   natural. NO tiene todos los refinamientos del 'atof' de la libreria.
*/

double atof_(char *c)
{
	return atof(c);
	double res;
	double divi, multi, signo;
	int fg;

	while(isspace(*c)) c++;
	if(*c == '-')
	 {
		signo = -1.0;
		c++;
	 }
	 else
		signo = 1.0;
	divi = 1.0;
	multi = 10.0;
	fg = 0;
	for(res = 0.0; *c == '.' || isdigit(*c);)
	{
		if(*c == '.')
		 {
			multi = 1.0;
			divi = 10.0;
			fg = 1;
			c++;
		 }
		else
		 {
			res *= multi;
			res += ((double)(*c++ - '0') / divi);
			if(fg)
				divi *= 10.0;
		 }
	}
	return res * signo;
}

/*
 *   A una expresion double, la almacena en el buffer de 13 caracteres que
 *   se le pasa.
*/

char *store(double que, char *donde)
{
	sprintf(donde, "%12.3lf", que);
	return donde;
}




