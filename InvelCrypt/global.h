/* GLOBAL.H - RSAREF types and constants 
 
/* Copyright (C) RSA Laboratories, a division of RSA Data Security, 
     Inc., created 1991. All rights reserved. 
 */ 
 
#ifndef _GLOBAL_H_ 
#define _GLOBAL_H_ 1 
 
#pragma pack(push)
#pragma pack(1)

/* POINTER defines a generic pointer type */ 
typedef unsigned char *POINTER; 
 
/* UINT2 defines a two byte word */ 
typedef unsigned short int UINT2; 
 
/* UINT4 defines a four byte word */ 
typedef unsigned long int UINT4; 

/*  INT/UINT defines a signed/unsigned integer as used by this old library   */
typedef signed short INT;
typedef unsigned short UINT;

/*  CHAR/UCHAR defines a signed/unsiged character  */
typedef signed char CHAR;
typedef unsigned char UCHAR;

#ifndef NULL_PTR 
#define NULL_PTR ((POINTER)0) 
#endif 
 
#ifndef UNUSED_ARG 
#define UNUSED_ARG(x) x = *(&x); 
#endif 
 

#pragma pack(pop)
#endif /* end _GLOBAL_H_ */ 

