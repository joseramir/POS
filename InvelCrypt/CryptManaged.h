// ICryptManaged.h

#pragma once

using namespace System;

// Implementacion RSA
#include "global.h"    
#include "rsaref.h"    
#include "r_random.h"    
#include "rsa.h"    
#include "nn.h"  
#include "md5.h"
#include "digit.h"

namespace CryptManaged {

	// Clase que maneja la encriptacion que usan los sistemas de invel.
	public ref class Crypt
	{
	private:
		INT cantClaves;
		CHAR *claves;

		int ObtenerSubindiceClave(INT id_clave);
		int InternalRSAPrivateDecrypt(UCHAR *buffer_out, UINT *len_out, 
			UCHAR *buffer_in, UINT len_in,
			R_RSA_PRIVATE_KEY *privateKey);
		int InternalRSAPublicEncrypt(UCHAR *buffer_out, UINT *len_out, 
			UCHAR *buffer_in, UINT len_in,
			R_RSA_PUBLIC_KEY *publicKey);

	public:
		Crypt();
		~Crypt();

		cli::array<Byte, 1> ^Decrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId);
		cli::array<Byte, 1> ^Encrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId);
	};
}
