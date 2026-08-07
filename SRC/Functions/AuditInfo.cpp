#include "stdafx.h"

#define SIZE_BLOCK 200

char *binFile = "cdata.bin";
unsigned long date(void);
long time__(void);

struct AuditData
{
	char CrudeCardData[128];		// Datos de la tarjeta.
	double monto;					// Monto de la autorizacion
	int cuotas;						// Cuotas de la operacion
	int dni;						// DNI del cliente
	int cvv;						// CVV ingresado
	int lote;						// Numero de lote para la transaccion
	int cupon;						// Numero de autorizacion
	int fecha, hora;				// Fecha y hora de la transaccion
};

void SaveAuditInfo(TarjInfo *info)
{
	struct AuditData data;

	data.monto = info->Monto;
	data.cuotas = info->Cuotas;
	data.dni = info->Dni;
	data.cvv = info->Cvv;
	data.lote = info->Lote;
	data.cupon = info->Cupon;
	data.fecha = date();
	data.hora = time__();
	memmove(data.CrudeCardData, info->CrudeCardData, 128);

	// Encripta el buffer (medio simple todavia) - Contiene datos sensibles!!!
	char buffer[SIZE_BLOCK];
	memmove(buffer, &data, sizeof(AuditData));
	//for(int i=0; i<SIZE_BLOCK; i++)
	//	buffer[i] ^= (char)i;

	int h = open(binFile, O_RDWR | O_BINARY);
	if(h < 0)
	{
		close(creat(binFile, S_IREAD | S_IWRITE));
		h = open(binFile, O_RDWR | O_BINARY);
	}
	if(h > 0)
	{
		lseek(h, 0, SEEK_END);
		write(h, buffer, SIZE_BLOCK);
		close(h);
	}
}
