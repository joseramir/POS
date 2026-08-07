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

namespace ICryptManaged {

	// Clase que maneja la encriptacion que usan los sistemas de invel.
	public ref class ICrypt
	{
	private:
		INT cantClaves;
		CHAR *claves;

		int ObtenerSubindiceClave(INT id_clave);
		int InternalRSAPrivateDecrypt(UCHAR *buffer_out, UINT *len_out, 
			UCHAR *buffer_in, UINT len_in,
			R_RSA_PRIVATE_KEY *privateKey);

	public:
		ICrypt();
		~ICrypt();

		cli::array<Byte, 1> ^Decrypt(cli::array<Byte, 1> ^inBuff, unsigned int inLen, short keyId);
	};
}
