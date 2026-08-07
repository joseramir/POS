#include "stdafx.h"

//   Retorna la fecha de modificacion de un archivo.
unsigned long LastUpda(char *fn)
{
    struct stat st;

    if(stat(fn, &st)) return 0;
    return (long)st.st_mtime;
}

