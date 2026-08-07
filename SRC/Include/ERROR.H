/*
**   Error.h
*/

#include <errno.h>

/*
**   Definicion de codigos de error.
*/

#define ALIASNOTFOUND   1
#define NODBOPEN        2
#define RECSHARED       3       /* Registro en uso por otro usuario  */
#define FILESHARED      4       /* Archivo en uso por otro */
#define FILENOTFOUND    5
#define NOTDB           6       /* No parece ser una dbf  */
#define NOMEM           7       /* No alcanza la memoria para algo  */
#define FLDNOTFOUND     8
#define BADIDXKEY       9       /* La clave de un indice no evalua bien */
