/* RSA.H - header file for RSA.C 
 */ 
 
/* Copyright (C) RSA Laboratories, a division of RSA Data Security, 
     Inc., created 1991. All rights reserved. 
 */ 
#ifndef _RSA_INCLUDED__
#define _RSA_INCLUDED__

#include "global.h"

INT RSAPublicEncrypt(UCHAR *, UINT *, UCHAR *, UINT, R_RSA_PUBLIC_KEY *, R_RANDOM_STRUCT *); 
INT RSAPrivateEncrypt(UCHAR *, UINT *, UCHAR *, UINT, R_RSA_PRIVATE_KEY *); 
INT RSAPublicDecrypt(UCHAR *, UINT *, UCHAR *, UINT, R_RSA_PUBLIC_KEY *); 
INT RSAPrivateDecrypt(UCHAR *, UINT *, UCHAR *, UINT, R_RSA_PRIVATE_KEY *); 

#endif
