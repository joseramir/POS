//
//  Lic.h
//  Manejo de licencias de productos.
//

#ifndef LIC_INCLUDED
#define LIC_INCLUDED

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
extern "C" { 
#include <io.h> 
}

#define BITNET_SALT 0xA1EFEDE

class SourceLine
{
public:
	class SourceLine *Next;
	class SourceLine *Previous;
	char *Source;
};

class PosLicense
{
public:
	class PosLicense *Next;
	char *Cuit;
	char *PtoVta;
	char *MaxFecha;

	PosLicense()
	{
		Next = NULL;
		Cuit = new char[12];
		PtoVta = new char[6];
		MaxFecha = new char[10];
	}

	~PosLicense() { delete Cuit; delete PtoVta; delete MaxFecha; }
};

class Lic
{
public:

	// Constructor y destructor
	Lic();
	~Lic();

	// Carga un archivo de licencias.
	bool LoadLic(char *filename, int initial);

	// Validacion de productos.
	bool ValidatePos(char *cuit, char *pvta, char *dateNow);

	// Retorna una descripcion del ultimo error detectado.
	char *Error(void) { return (errMessage[0] == 0)? NULL: errMessage; }

	// Valida que el cuit/pto de venta/fecha pasados sean validos.
	bool ValidarCuit(char *cuit);
	bool ValidarPtoVta(char *pv);
	bool ValidarFecha(int dia, int mes, int aio, char *fec);

	// Recalcula la firma del archivo y la reeplaza.
	bool ResignFile(char *filename, int initial);

	// Reinicializa el manejador
	void Reset();

private:
	// Carga un archivo de licencias
	bool LoadLicenseFile(char *filename, bool storeSrc);

	// Valida que la firma del archivo sea la correcta
	bool ValidateSign(int initial);

	// Firma nuevamente el archivo y lo graba
	void CalculateSign(int initial, unsigned *digest);
	unsigned ValidarFirma(char *firma, char sep, char wsep);

	// Libera la memoria ocupada por el source
	void FreeSource();
	void FreePosLicenses();
	bool ProcessLine(char *line, bool storeSource);
	void SkipSpaces(char **ptr);
	char ParseToken(char **ptr, char *token);

	// Indica si hay un archivo correctamente cargado.
	bool Loaded;

	// Indica si hay un archivo correctamente firmado.
	bool CorrectlySigned; 

	// Ultimo mensaje de error
	char *errMessage;

	// La firma tal cual se codifico en el archivo.
	unsigned *firma;

	// Codigo fuente
	SourceLine *Source;

	// Licencias de POS
	PosLicense *PLicenses;
};

/******************************************************************************************
 *	sha1.h
 *
 *	Copyright (C) 1998
 *	Paul E. Jones <paulej@arid.us>
 *	All Rights Reserved.
 *
 *****************************************************************************
 *	$Id: sha1.h,v 1.6 2004/03/27 18:02:26 paulej Exp $
 *****************************************************************************
 *
 *	Description:
 * 		This class implements the Secure Hashing Standard as defined
 * 		in FIPS PUB 180-1 published April 17, 1995.
 *
 *		Many of the variable names in this class, especially the single
 *		character names, were used because those were the names used
 *		in the publication.
 *
 * 		Please read the file sha1.cpp for more information.
 *
 */

class SHA1
{
	public:
		SHA1();
		virtual ~SHA1();

		
		//	Re-initialize the class
		void Reset();

		//	Returns the message digest
		bool Result(unsigned *message_digest_array);

		//	Provide input to SHA1
		void Input(	const unsigned char	*message_array,
					unsigned			length);
		void Input(	const char	*message_array,
					unsigned	length);
		void Input(unsigned char message_element);
		void Input(char message_element);
		SHA1& operator<<(const char *message_array);
		SHA1& operator<<(const unsigned char *message_array);
		SHA1& operator<<(const char message_element);
		SHA1& operator<<(const unsigned char message_element);

	private:

		//	Process the next 512 bits of the message
		void ProcessMessageBlock();

		//	Pads the current message block to 512 bits
		void PadMessage();

		//	Performs a circular left shift operation
		inline unsigned CircularShift(int bits, unsigned word);

		unsigned H[5];						// Message digest buffers

		unsigned Length_Low;				// Message length in bits
		unsigned Length_High;				// Message length in bits

		unsigned char Message_Block[64];	// 512-bit message blocks
		int Message_Block_Index;			// Index into message block array

		bool Computed;						// Is the digest computed?
		bool Corrupted;						// Is the message digest corruped?
	
};

// Funciones globales
char *Decrypt(char *s);
void DecryptClear(char *s);

#endif