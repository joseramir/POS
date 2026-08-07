// stdafx.h : Precompila los headers mas comunmente usados.

#if !defined STDAFX_INCLUDED__
#define STDAFX_INCLUDED__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Pragmas para optimizar la ejecucion del codigo.
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <sys/locking.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <share.h>
#include <ctype.h>
#include <conio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <time.h>
#include <math.h>
#include <process.h>
#include <dos.h>

#include <fstream>
#include <iostream>
extern "C" {
#include <io.h>
}

#if defined(POSXP) || defined(WIN32)
#define AVOID_DUPLICATED_SYMS 
#include <winsock2.h>          // 1° siempre
#include <ws2tcpip.h>          // 2°
#include <windows.h>           // 3°
#include <winspool.h>          // 4°

//#include <windows.h>
#undef GetEnvironmentVariable
#endif


#include "debug.h"
#include "DecNative.h"
#include "BDecimal.h"
#include "pos.h"
#include "Evaluator.h"
#include "arraylist.h"
#include "TempHandler.h"
#include "bitmap.h"
#include "cmos.h"
#include "opc.h"
#include "db.h"
#include "posdb.h"
#include "comprob.h"
#include "scrobj.h"
#include "total.h"
#include "GCStatic.h"
#include "MapFile.h"
#include "compat.h"
#include "wsup.h"
#include "tjonline.h"
#include "asigport.h"
#include "lic.h"
#include "evaluator.h"
#include "xstring.h"
#include "serial.h"
#include "promo.h"

#include "Thermalprinter.h"
#include "PrinterTM220.h"
#include "PrinterPR4.h"
#include "PrinterPr250.h"
#include "PrinterPR5.h"
#include "PrinterTermica.h"

#include "monitor.h"
#include "maptbl.h"
#include "DatosMpagos.h"
#include "total.h"
#include "MP_Param.h"
#include "bitmap.h"
#include "event.h"





#include "Colegios.h"

#include "Winfis.h"

#endif // STDAFX_INCLUDED
