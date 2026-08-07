//
//  Security.cpp
//  Manejo de claves de acceso y de seguridad del sistema.
//

#include <stdafx.h>

#ifdef POSXP
#include "PideClave.h"
#include "Strings.h"
using namespace pos;
#else

#ifdef PANTACOLOR
short passesq[16] = {           // Esquema default de colores
	{ ROJO * 16 + BLANCO },     // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ ROJO * 16 + VERDE }       // Texto contenido.
};
#else
short passesq[16] = {           // Esquema default de colores
	{ BLANCO * 16 + NEGRO },    // Color del borde
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ BLANCO * 16 + NEGRO }     // Texto contenido.
};
#endif
#endif

//   Requiere un 'password' para la funcion 'k'. Todas aquellas funciones que se desee proteger, podran habilitarse
//   con password. Estos password se guardaran en el archivo pos\claves.dbf.
//   Por compatibilidad con el funcionamiento anterior, la clave '1' esta reservada para solicitar 'X1'.
int RequestPasswd(char *k)
{
	dbf *kdb = new dbf;
	int haveK = kdb->Use(F_Claves);
	char kValue[10], kDesc[40];

	//   Levanta del archivo la clave a utilizar.
	if(haveK)
	{
		haveK = kdb->Locate("clave", k);
		if(haveK)
		{
			haveK = kdb->Get("des", kDesc);
			if(haveK)
			{
				haveK = kdb->Get("valor", kValue);
				if(haveK)
				{
					Trim(kValue);
					Trim(kDesc);
				}
			}
		}
		kdb->Close();
		delete kdb;
	}

	//   Si en el archivo no habia clave valida, entonces retorna VALIDO (permiso irrestricto)
	if(!haveK) return 1;
	if(CheckPassword(kDesc, kValue)) return 1;
	Alert("Clave invalida", "");
	return 0;
}

//   Pide en la pantalla una clave (cuyo mensaje orientativo se pasa) a fin
//   de poder continuar. Vuelve con 1 si la clave ingresada COINCIDE con la pasada
//   como referencia. Vuelve 0 en caso contrario. En cualquier caso, 'LastPassword'
//   contiene la ultima clave tipeada.
int CheckPassword(char *msg, char *rPassw)
{
#ifdef POSXP
	PideClave ^pide = gcnew PideClave(Strings::CharToString(msg), Strings::CharToString(rPassw));
	return
		(pide->ShowDialog() == DialogResult::OK);
#else
	int hWin, res;
	char lastPassword[12];

	hWin = wDefine(8, 18, 14, 62, BORDERSINGLE, SNGCURS, passesq, " Ingrese Clave ", "ESC=Cancela, F8=Confirma");

	EditWindow *ew = new EditWindow(hWin);
	wActivate(hWin);
	wLocate(1, 2);  wPstr(msg);
	wLocate(3, 2);  wPstr("Clave:");

	lastPassword[0] = 0;
	ew->AddGet(3, 11, PASSWORD_MASK, 10, 0, lastPassword, NULL, NULL, NULL);
	res = ew->AnimaGets(0);
	
	delete ew;
	wRelease(hWin);
	HClear();

	if(!res) return 0;
	Trim(lastPassword);
	return !_stricmp(lastPassword, rPassw);
#endif
}