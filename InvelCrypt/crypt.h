// Manejo del contenedor de claves RSA

#ifndef _CRYPT_INCLUDED__
#define _CRYPT_INCLUDED__

extern int cant_claves;
extern char *claves;

// Una clave RC5 para efectuar una encriptacion sencilla de datos.
extern char RC5key[];

// Prototipos
unsigned int CRYPT(int key2, char *plainText, char *cryptedText, unsigned int cryptedTextLength);
int InitDesencriptacion(void);
int DesencriptarDatos(unsigned char *posBuffer);

#endif
