/* RSAREF.H - header file for RSAREF cryptographic toolkit 
 */ 
 
/* Copyright (C) RSA Laboratories, a division of RSA Data Security, 
     Inc., created 1991. All rights reserved. 
 */ 
 
#ifndef _RSAREF_H_ 
#define _RSAREF_H_ 1 
 
#include "md2.h" 
#include "md5.h" 
#include "des.h" 

#pragma pack(push)
#pragma pack(1)

/* Message-digest algorithms. 
 */ 
#define DA_MD2 3 
#define DA_MD5 5 
 
/* Encryption algorithms to be ored with digest algorithm in Seal and Open. 
 */ 
#define EA_DES_CBC 1 
#define EA_DES_EDE2_CBC 2 
#define EA_DES_EDE3_CBC 3 
#define EA_DESX_CBC 4 
 
/* RSA key lengths. 
 */ 
#define MIN_RSA_MODULUS_BITS 508 * 4
#define MAX_RSA_MODULUS_BITS 1024 * 4
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8) 
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2) 
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8) 
 
/* Maximum lengths of encoded and encrypted content, as a function of 
   content length len. Also, inverse functions. 
 */ 
#define ENCODED_CONTENT_LEN(len) (4*(len)/3 + 3) 
#define ENCRYPTED_CONTENT_LEN(len) ENCODED_CONTENT_LEN ((len)+8) 
#define DECODED_CONTENT_LEN(len) (3*(len)/4 + 1) 
#define DECRYPTED_CONTENT_LEN(len) (DECODED_CONTENT_LEN (len) - 1) 
 
/* Maximum lengths of signatures, encrypted keys, encrypted 
   signatures, and message digests. 
 */ 
#define MAX_SIGNATURE_LEN MAX_RSA_MODULUS_LEN 
#define MAX_PEM_SIGNATURE_LEN ENCODED_CONTENT_LEN (MAX_SIGNATURE_LEN) 
#define MAX_ENCRYPTED_KEY_LEN MAX_RSA_MODULUS_LEN 
#define MAX_PEM_ENCRYPTED_KEY_LEN ENCODED_CONTENT_LEN (MAX_ENCRYPTED_KEY_LEN) 
#define MAX_PEM_ENCRYPTED_SIGNATURE_LEN ENCRYPTED_CONTENT_LEN (MAX_SIGNATURE_LEN) 
#define MAX_DIGEST_LEN 16 
 
/* Maximum length of Diffie-Hellman parameters. 
 */ 
#define DH_PRIME_LEN(bits) (((bits) + 7) / 8) 
 
/* Error codes. 
 */ 
#define RE_CONTENT_ENCODING 0x0400 
#define RE_DATA 0x0401 
#define RE_DIGEST_ALGORITHM 0x0402 
#define RE_ENCODING 0x0403 
#define RE_KEY 0x0404 
#define RE_KEY_ENCODING 0x0405 
#define RE_LEN 0x0406 
#define RE_MODULUS_LEN 0x0407 
#define RE_NEED_RANDOM 0x0408 
#define RE_PRIVATE_KEY 0x0409 
#define RE_PUBLIC_KEY 0x040a 
#define RE_SIGNATURE 0x040b 
#define RE_SIGNATURE_ENCODING 0x040c 
#define RE_ENCRYPTION_ALGORITHM 0x040d 
 
/* Random structure. 
 */ 
typedef struct tagrRandom { 
  UINT bytesNeeded; 
  UCHAR state[16]; 
  UINT outputAvailable; 
  UCHAR output[16]; 
} R_RANDOM_STRUCT; 
 
/* RSA public and private key. 
 */ 
typedef struct tagRsaPublicKey { 
  UINT bits;                           /* length in bits of modulus */ 
  UCHAR modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */ 
  UCHAR exponent[MAX_RSA_MODULUS_LEN];           /* public exponent */ 
} R_RSA_PUBLIC_KEY; 
 
typedef struct tagRsaPrivateKey { 
  UINT bits;                           /* length in bits of modulus */ 
  UCHAR modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */ 
  UCHAR publicExponent[MAX_RSA_MODULUS_LEN];     /* public exponent */ 
  UCHAR exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */ 
  UCHAR prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */ 
  UCHAR primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */ 
  UCHAR coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */ 
} R_RSA_PRIVATE_KEY; 
 
/* RSA prototype key. 
 */ 
typedef struct tagRsaProtoKey { 
  UINT bits;                           /* length in bits of modulus */ 
  INT useFermat4;                        /* public exponent (1 = F4, 0 = 3) */ 
} R_RSA_PROTO_KEY; 
 
/* Diffie-Hellman parameters. 
 */ 
typedef struct tagRsaDHParam { 
  UCHAR *prime;                                            /* prime */ 
  UINT primeLen;                                 /* length of prime */ 
  UCHAR *generator;                                    /* generator */ 
  UINT generatorLen;                         /* length of generator */ 
} R_DH_PARAMS; 
 
typedef struct tagRsaContext { 
  INT digestAlgorithm; 
  union { 
    MD2_CTX md2; 
    MD5_CTX md5; 
  } context; 
} R_DIGEST_CTX; 
 
typedef struct tagRsaSignatureContext { 
  R_DIGEST_CTX digestContext; 
} R_SIGNATURE_CTX; 
 
typedef struct tagEnvolopeContext { 
  INT encryptionAlgorithm; 
  union { 
    DES_CBC_CTX des; 
    DES3_CBC_CTX des3; 
    DESX_CBC_CTX desx; 
  } cipherContext; 
   
  UCHAR buffer[8]; 
  UINT bufferLen; 
} R_ENVELOPE_CTX; 
 
/* Random structures. 
 */ 
INT R_RandomInit(R_RANDOM_STRUCT *); 
INT R_RandomUpdate(R_RANDOM_STRUCT *, UCHAR *, UINT); 
INT R_GetRandomBytesNeeded(UINT *, R_RANDOM_STRUCT *); 
void R_RandomFinal(R_RANDOM_STRUCT *); 
 
/* Cryptographic procedures "by parts" 
 */ 
INT R_DigestInit(R_DIGEST_CTX *, INT); 
INT R_DigestUpdate(R_DIGEST_CTX *, UCHAR *, UINT); 
INT R_DigestFinal(R_DIGEST_CTX *, UCHAR *, UINT *); 
 
INT R_SignInit(R_SIGNATURE_CTX *, INT);
INT R_SignUpdate(R_SIGNATURE_CTX *, UCHAR *, UINT); 
INT R_SignFinal(R_SIGNATURE_CTX *, UCHAR *, UINT *, R_RSA_PRIVATE_KEY *); 
 
INT R_VerifyInit(R_SIGNATURE_CTX *, INT); 
INT R_VerifyUpdate(R_SIGNATURE_CTX *, UCHAR *, UINT); 
INT R_VerifyFinal(R_SIGNATURE_CTX *, UCHAR *, UINT, R_RSA_PUBLIC_KEY *); 
 
INT R_SealInit(R_ENVELOPE_CTX *, UCHAR **, UINT *, UCHAR [8], 
    UINT, R_RSA_PUBLIC_KEY **, INT, R_RANDOM_STRUCT *); 
INT R_SealUpdate(R_ENVELOPE_CTX *, UCHAR *, UINT *, UCHAR *, 
    UINT); 
INT R_SealFinal(R_ENVELOPE_CTX *, UCHAR *, UINT *); 
 
INT R_OpenInit(R_ENVELOPE_CTX *, INT, UCHAR *, UINT, UCHAR [8], 
    R_RSA_PRIVATE_KEY *); 
INT R_OpenUpdate(R_ENVELOPE_CTX *, UCHAR *, UINT *, UCHAR *, 
    UINT); 
INT R_OpenFinal(R_ENVELOPE_CTX *, UCHAR *, UINT *); 
 
/* Cryptographic enhancements by block. 
 */ 
INT R_SignPEMBlock(UCHAR *, UINT *, UCHAR *, UINT *, 
    UCHAR *, UINT, INT, INT, R_RSA_PRIVATE_KEY *); 
INT R_SignBlock(UCHAR *, UINT *, UCHAR *, UINT, INT, 
    R_RSA_PRIVATE_KEY *); 
INT R_VerifyPEMSignature(UCHAR *, UINT *, UCHAR *, UINT, 
    UCHAR *, UINT, INT, INT, R_RSA_PUBLIC_KEY *); 
INT R_VerifyBlockSignature(UCHAR *, UINT, UCHAR *, UINT, INT, 
    R_RSA_PUBLIC_KEY *); 
INT R_SealPEMBlock(UCHAR *, UINT *, UCHAR *, UINT *, 
    UCHAR *, UINT *, UCHAR [8], UCHAR *, 
    UINT, INT, R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *, 
    R_RANDOM_STRUCT *); 
INT R_OpenPEMBlock(UCHAR *, UINT *, UCHAR *, UINT, 
    UCHAR *, UINT, UCHAR *, UINT, 
    UCHAR [8], INT, R_RSA_PRIVATE_KEY *, R_RSA_PUBLIC_KEY *); 
INT R_DigestBlock(UCHAR *, UINT *, UCHAR *, UINT, INT); 
 
/* PrINTable ASCII encoding and decoding. 
 */ 
INT R_EncodePEMBlock(UCHAR *, UINT *, UCHAR *, UINT); 
INT R_DecodePEMBlock(UCHAR *, UINT *, UCHAR *, UINT); 
   
/* Key-pair generation. 
 */ 
INT R_GeneratePEMKeys(R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *, R_RSA_PROTO_KEY *, 
    R_RANDOM_STRUCT *); 
 
/* Diffie-Hellman key agreement. 
 */ 
INT R_GenerateDHParams(R_DH_PARAMS *, UINT, UINT, R_RANDOM_STRUCT *); 
INT R_SetupDHAgreement(UCHAR *, UCHAR *, UINT, R_DH_PARAMS *, R_RANDOM_STRUCT *); 
INT R_ComputeDHAgreedKey(UCHAR *, UCHAR *, UCHAR *, UINT, R_DH_PARAMS *); 
 
/* Routines supplied by the implementor. - (Las dirijo a la libreria estandard)
void R_memset(POINTER, INT, UINT); 
void R_memcpy(POINTER, POINTER, UINT); 
INT R_memcmp(POINTER, POINTER, UINT);  */
#include "string.h"
#define R_memset memset
#define R_memcpy memcpy
#define R_memcmp memcmp

#pragma pack(pop)

#endif 
