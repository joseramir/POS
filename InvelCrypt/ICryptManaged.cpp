// Archivo DLL principal.

#include "stdafx.h"

#include "ICryptManaged.h"

namespace ICryptManaged {

	using namespace System::Runtime::InteropServices;

	ICrypt::ICrypt()
	{
		claves = NULL;
		int h = _open("key.tef", 0);
		if(h > 0)
		{
			int flen = _filelength(h);
			cantClaves = flen / 0x1006u;
			claves = (CHAR *)malloc(flen);
			if(claves != NULL)
			  _read(h, claves, flen);
			_close(h);
		}
		if(h <= 0 || claves == NULL)
			throw gcnew Exception("Error al crear ICrypt");
	}

	ICrypt::~ICrypt()
	{
		free(claves);
	}

	int ICrypt::ObtenerSubindiceClave(INT id_clave)
	{
		INT claveEncontrada = -1;

		for(int indexClave = 0; indexClave < cantClaves && claveEncontrada < 0; indexClave++)
		{
			if (id_clave == *((INT *)claves + 2051 * indexClave))
			claveEncontrada = indexClave;
		}
		return claveEncontrada;
	}

	int ICrypt::InternalRSAPrivateDecrypt(UCHAR *buffer_out, UINT *len_out, 
		UCHAR *buffer_in, UINT len_in,
		R_RSA_PRIVATE_KEY *privateKey)
	{
		int ok = 0;
		int decryptResult = 0;

		// Inicializa variables de blocking
		UINT bytes_por_bloque = privateKey->bits / 8;
		UINT cant_bloques = len_in / bytes_por_bloque;
		if(len_in % bytes_por_bloque) ++cant_bloques;

		// Manejo de la cuenta de bytes en el bloque saliente.
		UINT _len_out = *len_out, len_out_orig = *len_out;
		UINT bloques_procesados = 0, bytes_a_procesar;
		UINT indexSource = 0, indexResult = 0;
		*len_out = 0;

		while (bloques_procesados < cant_bloques && !decryptResult)
		{
			// Determina cuantos bytes del bloque entrante se deben procesar (maximo, 'bytes_por_bloque' por pasada).
			if (bloques_procesados == cant_bloques - 1 )
			  bytes_a_procesar = len_in - bytes_por_bloque * indexSource;
			else
			  bytes_a_procesar = bytes_por_bloque;

			// Efectua el desencriptado del bloque.
			decryptResult = RSAPrivateDecrypt(
				&buffer_out[indexResult], 
				&_len_out, 
				&buffer_in[indexSource], 
				bytes_a_procesar, 
				privateKey);

			// Actualiza las variables de blocking
			indexSource += bytes_a_procesar;
			*len_out += _len_out;
			indexResult += _len_out;
			_len_out = len_out_orig - indexResult;
			bloques_procesados++;
		}
		if (!decryptResult) ok = 1;
		return ok;
	}

	cli::array<Byte, 1> ^ICrypt::Decrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId)
	{
		R_RSA_PRIVATE_KEY pvk;
		UCHAR *input = new UCHAR[inLen + 10];
		for(UINT i = 0; i < inLen; i++)
			input[i] = inBuff[i];

		UCHAR *output = new UCHAR[inLen * 5];
		UINT lenOut = inLen * 5;

		int subClave = ObtenerSubindiceClave(keyId);
		CHAR *privateKey = claves + 4102 * subClave;
		UINT moveSize = sizeof(R_RSA_PRIVATE_KEY);
		if(moveSize > 0xb02) moveSize = 0xb02;
	    memcpy(&pvk, (privateKey + 6), moveSize);

		R_RSA_PRIVATE_KEY *pk = (R_RSA_PRIVATE_KEY *)(privateKey + 6);
		int result = InternalRSAPrivateDecrypt(output, &lenOut, input, inLen, pk);

		delete input;
		if(result)
		{
			cli::array<Byte, 1> ^salida = gcnew cli::array<Byte, 1>(lenOut);
			for(UINT i = 0; i < lenOut; i++)
				salida[i] = output[i];
			delete output;
			return salida;
		}

		delete output;
		return nullptr;
	}
}
