#ifndef DEC_NATIVE_INCLUDED__
#define DEC_NATIVE_INCLUDED__

#include "string.h"

// Algunos tipos usados por las librerias de Microsoft.
#ifndef AVOID_DUPLICATED_SYMS
typedef unsigned int DWORD;
#define STDAPI int

// Codigos de resultado
#define NOERROR				0
#define DISP_E_OVERFLOW		0x8001
#define DISP_E_DIVBYZERO	0x8002
#define E_INVALIDARG		0x8003
#define VARCMP_EQ			0x4001
#define VARCMP_LT			0x4002
#define VARCMP_GT			0x4003

#define min(p1, p2) (((p1)<=(p2))?(p1):p2)

#endif

typedef unsigned short int USHORT;
typedef unsigned char BYTE;
typedef unsigned long ULONG;
typedef unsigned long long DWORDLONG;
typedef unsigned long long ULONGLONG;
typedef unsigned __int64 UI64;
typedef long long LONGLONG;

// Forma en la que estan construidos los 'double' del runtime.
typedef union{
    struct {
      ULONG mantLo;
      ULONG mantHi:20;
      ULONG exp:11;
      ULONG sign:1;
    } u;
    double dbl;
} DBLSTRUCT;

#define DBLBIAS 1022
#define DECMAX 28

// Incializadors para DBLSTRUCT
#define DEFDS(Lo, Hi, exp, sign) { {Lo, Hi, exp, sign} }

// Forma en la que estan construidos los longlong del runtme.
typedef union {
    DWORDLONG int64;
    struct {
        ULONG Lo;
        ULONG Hi;
    } u;
} SPLIT64;

// Definicion del STORAGE necesario para un decimal.
#ifndef TAG_DEC_DEFINED_
#define TAG_DEC_DEFINED_

typedef struct tagDEC {
    USHORT wReserved;
    union {
        struct {
            BYTE scale;
            BYTE sign;
        } u;
        USHORT signscale;
    } u;
    ULONG Hi32;
    union {
        struct {
            ULONG Lo32;
            ULONG Mid32;
        } v;
    } v;
} DECIMAL, *LPDECIMAL;
#endif

// Alungas macros para acceso a las partes del decimal
#define DECIMAL_NEG				 ((BYTE)0x80)
#define DECIMAL_SCALE(dec)       ((dec).u.u.scale)
#define DECIMAL_SIGN(dec)        ((dec).u.u.sign)
#define DECIMAL_SIGNSCALE(dec)   ((dec).u.signscale)
#define DECIMAL_LO32(dec)        ((dec).v.v.Lo32)
#define DECIMAL_MID32(dec)       ((dec).v.v.Mid32)
#define DECIMAL_HI32(dec)        ((dec).Hi32)

// Pone un decimal a cero.
#ifdef DECIMAL_SETZERO
#undef DECIMAL_SETZERO
#endif
#define DECIMAL_SETZERO(dec) {DECIMAL_LO32(dec) = 0; DECIMAL_MID32(dec) = 0; DECIMAL_HI32(dec) = 0; DECIMAL_SIGNSCALE(dec) = 0;}

// Copia un decimal sobre otro.
#define COPYDEC(dest, src) {DECIMAL_SIGNSCALE(dest) = DECIMAL_SIGNSCALE(src); DECIMAL_HI32(dest) = DECIMAL_HI32(src); \
    DECIMAL_MID32(dest) = DECIMAL_MID32(src); DECIMAL_LO32(dest) = DECIMAL_LO32(src); }

// Devuelve la parte baja de 64 bits de un decimal.
#define DECIMAL_LO64_GET(dec)    (((ULONGLONG)DECIMAL_MID32(dec) << 32) | DECIMAL_LO32(dec))

// Cambia la parte baja de 64 bits de un decimal.
#define DECIMAL_LO64_SET(dec, value)    {ULONGLONG _Value = value; DECIMAL_MID32(dec) = (ULONG)(_Value >> 32); \
    DECIMAL_LO32(dec) = (ULONG)_Value; }

// Algunas constantes matematicas para decimales.
#define DEC_SCALE_MAX   28
#define POWER10_MAX     9

// Definicion de los overflows entre las partes.
#define OVFL_MAX_9_HI   4
#define OVFL_MAX_9_MID  1266874889
#define OVFL_MAX_5_HI   42949
#define OVFL_MAX_5_MID  2890341191
#define OVFL_MAX_1_HI   429496729

// Macros paa simplificar conversiones
#ifndef UInt32x32To64
#define UInt32x32To64(a, b) ((DWORDLONG)((DWORD)(a)) * (DWORDLONG)((DWORD)(b)))
#endif

#define Div64by32(num, den) ((ULONG)((DWORDLONG)(num) / (ULONG)(den)))
#define Mod64by32(num, den) ((ULONG)((DWORDLONG)(num) % (ULONG)(den)))

// Prototipos de funciones nativas
STDAPI DecFromR8(double d, LPDECIMAL dec);
STDAPI DecAddSub(LPDECIMAL pdecL, LPDECIMAL pdecR, LPDECIMAL pRes, char bSign);
STDAPI Multiply(LPDECIMAL pdecL, LPDECIMAL pdecR, LPDECIMAL pRes);
STDAPI Divide(LPDECIMAL pdecL, LPDECIMAL pdecR, LPDECIMAL pRes);
STDAPI DecToString(LPDECIMAL pdecIn, ULONG dwFlags, char **pbstrOut);
STDAPI NativeRound(LPDECIMAL pdecIn, int decim, LPDECIMAL pRes);
STDAPI NativeAbs(LPDECIMAL pdecOprd, LPDECIMAL pdecRes);
STDAPI DecCompare(LPDECIMAL pdecL, LPDECIMAL pdecR);
STDAPI NativeNeg(LPDECIMAL pdecOprd, LPDECIMAL pdecRes);

#ifndef VAR_C_DEFINED_
#define VAR_C_DEFINED_

STDAPI VarI4FromDec(LPDECIMAL pdecIn, int *plValOut);
STDAPI VarR8FromDec(LPDECIMAL pdecIn, double *pdblOut);

#endif

#endif	DEC_NATIVE_INCLUDED__

