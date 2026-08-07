//
//   Voucher.cpp
//   Manejo de impresion de vouchers.
//

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

//   Lista de vouchers cargados.
voucher *voucherList[1000];
voucher *vouchersPend[10];
int voucherLoadFlags[1000];
int voucherPendCount = 0;

//   Lista de variables que puede utilizar el voucher. Las variables son inicializadas y
//   exportadas por las funciones que imprimen vouchers.
struct _voucherVarList
{
	char vName[50];			// Nombre de la variable.
	char *value;			// Ultimo valor correcto de la misma.
} voucherVarList[300];
int voucherVarCount = 0;

//   Agrega o actualiza el valor de una variable
void VoucherVar(char *name, char *value)
{
	int i;
	_voucherVarList *v = voucherVarList;
	if(strlen(name) > 49) throw "Nombre muy largo para Voucher-Var";
	for(i = 0; i < voucherVarCount; i++, v++)
	{
		if(!_stricmp(v->vName, name))
		{
			if(v->value != NULL) FREE(v->value);
			v->value = STRDUP((value == NULL)? (char *)"": value, "VoucherVar-SET");
			return;
		}
	}
	if(voucherVarCount >= 299)
	{
		WriteBootLog("Demasiadas Voucher VAr sale con excepcion");
		throw "Demasiadas voucher-var";
	}
	v = &voucherVarList[voucherVarCount++];
	STRCPY(v->vName, name);
	v->value = STRDUP((value == NULL)? (char *)"": value, "VoucherVar-NEW");
}

void VoucherVar(char *name, int value)
{
	char tValue[20];
	_itoa(value, tValue, 10);
	VoucherVar(name, tValue);
}

void VoucherVar(char *name, double value)
{
	char tValue[20];
	if(fabs(value - (double((int)value))) < 0.01)
		VoucherVar(name, (int)value);
	else
	{
		sprintf(tValue, "%.2lf", value);
		VoucherVar(name, tValue);
	}
}

void VoucherVar(char *name, double value, bool esMoneda)
{
	char tValue[20];
	if(!esMoneda)
		VoucherVar(name, value);
	else
	{
		sprintf(tValue, "$%.2lf", value);
		VoucherVar(name, tValue);
	}
}

//   Expande el voucher que se le indica.
voucher *ExpandVoucher(int nro)
{
	voucher *v, *s;
	char tChar[300], *l, *p, vName[50], *vn;
	int i, j;

	v = new voucher(); // (voucher *) MALLOC(sizeof(voucher), "xList:ExpandVoucher");
	s = voucherList[nro-1];
	v->cLines = s->cLines;	
	v->sinencycola = s->sinencycola;
	for(i = 0; i < s->cLines; i++)
	{
		//   Prepara las cadenas origen y destino.
		tChar[0] = 0;
		p = tChar;
		l = s->msg[i];

		//   Scanea la linea de entrada buscando especificaciones de variables.
		while(*l)
		{
			//  Si encuentra una arroba, comienza el nombre de una variable.
			if(*l == '@')
			{
				//   Busca la arroba que marca el final del nombre.
				for(vn = ++l; *l; l++)
				{
					if(*l == '@')
					{
						//   Obtiene el nombre de la variable.
						memmove(vName + 1, vn, l-vn);
						vName[l-vn + 2] = 0;
						vName[0] = vName[l-vn+1] = '@';
						l++;

						//   La busca en la tabla de variables.
						for(j = 0; j < voucherVarCount; j++)
						{
							if(!_stricmp(voucherVarList[j].vName, vName))
							{
								STRCPY(p, voucherVarList[j].value);
								p += strlen(voucherVarList[j].value);
								break;
							}
						}
						break;
					}
				}
			}
			else
				*p++ = *l++;
		}

		//   Almacena el resultado procesado.
		*p = 0;
		v->msg[i] = STRDUP(tChar, "ExpandVoucher2");				
	}
		
	return v;
}

//   Apila la impresion de un voucher.
void StackVoucher(int nro)
{
	if(voucherPendCount == 9 || nro < 0 || nro > 999) return;
	if(voucherLoadFlags[nro] == 0)
		LoadVoucher(nro);
	if(voucherLoadFlags[nro] != 1)
		return;

	//   Recupera algunos datos para variables.
	VoucherVar("@TICK_NRO@", pf->GetTickNro(GlobalTF));

	//   Expande el voucher
	vouchersPend[voucherPendCount++] = ExpandVoucher(nro);
}

//   Imprime todos los vouchers que esten pendientes.
void PrintVouchers(void)
{
	voucher *v;
	int i, j;

	for(i = 0; i < voucherPendCount; i++)
	{
		//   Abre voucher en impresora.
		v = vouchersPend[i];
		if (v->sinencycola)
			pf->OpenVoucher2();
		else
			pf->OpenVoucher();
		pf->PrintVoucher("");

		for(j = 0; j < v->cLines; j++)
		{
			//   Imprime linea en la impresora.
			pf->PrintVoucher(v->msg[j]);

			//   Libera la memoria utilizada por la linea. (Se cargo con STRDUP).
			FREE(v->msg[j]);
		}
		if(v->sinencycola)
			pf->CloseVoucher2();
		else
		{
			pf->PrintVoucher("");
			pf->CloseVoucher();
		}
		//  Cierra el voucher y recupera la memoria utilizada.
		FREE(vouchersPend[i]);
		
	}
	
	voucherPendCount = 0;	// Indica que esta todo impreso.
}


//   Elimina los vouchers que esten generados.
void ResetVouchers()
{
	voucher *v;
	int i, j;

	for(i = 0; i < voucherPendCount; i++)
	{
		v = vouchersPend[i];
		for(j = 0; j < v->cLines; j++)
			FREE(v->msg[j]);

		//  Cierra el voucher y recupera la memoria utilizada.
		FREE(vouchersPend[i]);
	}
	voucherPendCount = 0;	// Indica que esta todo impreso.
}


//   A pedido de otras funciones (por ejemplo, del medio de pago), carga en memoria la
//   definicion de un voucher dado. Retorna TRUE si pudo cargar algo, sino, false.
char *GetPropLine(char *secc, char *name, int maxLen);		// Messg.cpp
char *GetPropLine(char *secc, char *name);					// Messg.cpp

int LoadVoucher(int nro)
{
	int res = 0, cLines, i;
	char voucherName[50], lineName[60];
	voucher *v;

	if(nro < 0 || nro > 999)  return res;
	if(voucherLoadFlags[nro] != 0) return 1;				// Ya esta cargado

	if(!OpenIni(F_Mensajes))
	{
		sprintf(voucherName, "voucher%d", nro);
		cLines = ExtractIniLong(voucherName, "cant");
		if(cLines > 0)
		{
			res = 1;
			v = new voucher();
			v->cLines = cLines;
			v->sinencycola =	ExtractIniSiNo(voucherName, "sin-encab-cola", 0);
			voucherList[nro - 1] = v;
			for(i = 1; i <= cLines; i++)
			{
				sprintf(lineName, "linea%d", i);
				v->msg[i-1] = GetPropLine(voucherName, lineName, 59);
			}

			voucherLoadFlags[nro] = 1;
		}
		else
			voucherLoadFlags[nro] = -1;

		CloseIni();
	}
	return res;
}


