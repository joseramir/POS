// Archivo DLL principal.

#include "stdafx.h"

#include "CryptManaged.h"

namespace CryptManaged {

	using namespace System::Runtime::InteropServices;

	Crypt::Crypt()
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

	Crypt::~Crypt()
	{
		free(claves);
	}

	int Crypt::ObtenerSubindiceClave(INT id_clave)
	{
		INT claveEncontrada = -1;

		for(int indexClave = 0; indexClave < cantClaves && claveEncontrada < 0; indexClave++)
		{
			if (id_clave == *((INT *)claves + 2051 * indexClave))
			claveEncontrada = indexClave;
		}
		return claveEncontrada;
	}

	int Crypt::InternalRSAPrivateDecrypt(UCHAR *buffer_out, UINT *len_out, 
		UCHAR *buffer_in, UINT len_in,
		R_RSA_PRIVATE_KEY *privateKey)
	{
		int ok = 0;
		int decryptResult = 0;

		// Inicializa variables de blocking
		int bytes_por_bloque = privateKey->bits / 8;
		int cant_bloques = len_in / bytes_por_bloque;
		if(len_in % bytes_por_bloque) ++cant_bloques;

		// Manejo de la cuenta de bytes en el bloque saliente.
		UINT _len_out = *len_out, len_out_orig = *len_out;
		int bloques_procesados = 0, bytes_a_procesar;
		int indexSource = 0, indexResult = 0;
		*len_out = 0;

		while (bloques_procesados < cant_bloques && !decryptResult)
		{
			// Determina cuantos bytes del bloque entrante se deben procesar (maximo, 'bytes_por_bloque' por pasada).
			if (bloques_procesados == cant_bloques - 1 )
			  bytes_a_procesar = len_in - bytes_por_bloque * bloques_procesados;
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

	int Crypt::InternalRSAPublicEncrypt(UCHAR *buffer_out, UINT *len_out, 
		UCHAR *buffer_in, UINT len_in,
		R_RSA_PUBLIC_KEY *publicKey)
	{
		int ok = 0;
		int decryptResult = 0;

		// Inicializa variables de blocking
		int bytes_por_bloque = publicKey->bits / 8 - 11;
		int cant_bloques = len_in / bytes_por_bloque;
		if(len_in % bytes_por_bloque) ++cant_bloques;

		// Manejo de la cuenta de bytes en el bloque saliente.
		UINT _len_out = *len_out, len_out_orig = *len_out;
		int bloques_procesados = 0, bytes_a_procesar;
		int indexSource = 0, indexResult = 0;
		*len_out = 0;

		while (bloques_procesados < cant_bloques && !decryptResult)
		{
			// Determina cuantos bytes del bloque entrante se deben procesar (maximo, 'bytes_por_bloque' por pasada).
			if (bloques_procesados == cant_bloques - 1 )
			  bytes_a_procesar = len_in - bytes_por_bloque * bloques_procesados;
			else
			  bytes_a_procesar = bytes_por_bloque;

			// Construye el generandor random a usar
			R_RANDOM_STRUCT random;
			R_RandomInit(&random);

			// Efectua el desencriptado del bloque.
			decryptResult = RSAPublicEncrypt(
				&buffer_out[indexResult], 
				&_len_out, 
				&buffer_in[indexSource], 
				bytes_a_procesar, 
				publicKey,
				&random);

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

	cli::array<Byte, 1> ^Crypt::Decrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId)
	{
		UCHAR *input = new UCHAR[inLen + 10];
		for(UINT i = 0; i < inLen; i++)
			input[i] = inBuff[i];

		UCHAR *output = new UCHAR[inLen * 5];
		UINT lenOut = inLen * 5;

		int subClave = ObtenerSubindiceClave(keyId);
		CHAR *privateKey = claves + 4102 * subClave;

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

	cli::array<Byte, 1> ^Crypt::Encrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId)
	{
		UCHAR *input = new UCHAR[inLen + 10];
		for(UINT i = 0; i < inLen; i++)
			input[i] = inBuff[i];

		UCHAR *output = new UCHAR[inLen * 5];
		UINT lenOut = inLen * 5;

		int subClave = ObtenerSubindiceClave(keyId);
		CHAR *publicKey = claves + 4102 * subClave;

		R_RSA_PUBLIC_KEY *pk = (R_RSA_PUBLIC_KEY *)(publicKey + 6);
		int result = InternalRSAPublicEncrypt(output, &lenOut, input, inLen, pk);

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
