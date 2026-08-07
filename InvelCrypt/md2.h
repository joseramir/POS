/* MD2.H - header file for MD2C.C 
 */ 
 
/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All 
   rights reserved. 
 
   License to copy and use this software is granted for 
   non-commercial Internet Privacy-Enhanced Mail provided that it is 
   identified as the "RSA Data Security, Inc. MD2 Message Digest 
   Algorithm" in all material mentioning or referencing this software 
   or this function. 
 
   RSA Data Security, Inc. makes no representations concerning either 
   the merchantability of this software or the suitability of this 
   software for any particular purpose. It is provided "as is" 
   without express or implied warranty of any kind. 
 
   These notices must be retained in any copies of any part of this 
   documentation and/or software.   
 */ 
 
#ifndef _MD2_H_ 
#define _MD2_H_ 1 

#include "global.h"

#pragma pack(push)
#pragma pack(1)

typedef struct tagMd2Context { 
  UCHAR state[16];                                 /* state */ 
  UCHAR checksum[16];                           /* checksum */ 
  UINT  count;                /* number of bytes, modulo 16 */ 
  UCHAR buffer[16];                         /* input buffer */ 
} MD2_CTX; 
 
void MD2Init(MD2_CTX *); 
void MD2Update(MD2_CTX *, UCHAR *, UINT); 
void MD2Final(UCHAR [16], MD2_CTX *); 

#pragma pack(pop)
#endif 


