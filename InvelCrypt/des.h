/* DES.H - header file for DESC.C 
 */ 
 
#ifndef _DES_H_ 
#define _DES_H_ 1 

#pragma pack(push)
#pragma pack(1)

typedef struct tagDesCBC { 
  UINT4 subkeys[32];                                             /* subkeys */ 
  UINT4 iv[2];                                       /* initializing vector */ 
  UINT4 originalIV[2];                        /* for restarting the context */ 
  INT encrypt;                                               /* encrypt flag */ 
} DES_CBC_CTX; 
 
typedef struct tagDesxCBC { 
  UINT4 subkeys[32];                                             /* subkeys */ 
  UINT4 iv[2];                                       /* initializing vector */ 
  UINT4 inputWhitener[2];                                 /* input whitener */ 
  UINT4 outputWhitener[2];                               /* output whitener */ 
  UINT4 originalIV[2];                        /* for restarting the context */ 
  INT encrypt;                                              /* encrypt flag */ 
} DESX_CBC_CTX; 
 
typedef struct tagDes3CBC { 
  UINT4 subkeys[3][32];                     /* subkeys for three operations */ 
  UINT4 iv[2];                                       /* initializing vector */ 
  UINT4 originalIV[2];                        /* for restarting the context */ 
  INT encrypt;                                              /* encrypt flag */ 
} DES3_CBC_CTX; 
 
void DES_CBCInit(DES_CBC_CTX *, CHAR *, CHAR *, INT); 
INT DES_CBCUpdate(DES_CBC_CTX *, CHAR *, CHAR *, UINT); 
void DES_CBCRestart(DES_CBC_CTX *);
 
void DESX_CBCInit(DESX_CBC_CTX *, CHAR *, CHAR *, INT); 
INT DESX_CBCUpdate(DESX_CBC_CTX *, CHAR *, CHAR *, UINT); 
void DESX_CBCRestart(DESX_CBC_CTX *); 
 
void DES3_CBCInit(DES3_CBC_CTX *, CHAR *, CHAR *, INT); 
INT DES3_CBCUpdate(DES3_CBC_CTX *, CHAR *, CHAR *, UINT); 
void DES3_CBCRestart(DES3_CBC_CTX *); 

#pragma pack(pop)
#endif 
