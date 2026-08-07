/*    Cmos.h     */

#ifndef CMOS_INCLUDED__
#define CMOS_INCLUDED__

//   Declara variables que se corresponden en el cmos-ram
#define   TICKET		 0
#define   ZETA			 1
#define   NETO_1		 2
#define   NETO_2		 3
#define   NETO_3		 4
#define   NETO_4		 5
#define   IVA_1			 6
#define   IVA_2			 7
#define   IVA_3			 8
#define   IVA_4			 9
#define   TOTVTA		10
#define   CANULA		11
#define   TANULA		12
#define   SOBRET		13
#define   CAJERO		14
#define   FLAGPREC		15
#define   TCOBRO		16
#define   MONITOR       17
#define   TOTVTAREAL	18
#define   PROMOCALC		19
#define	  ULTCAJERO	    20
#define   SUPERVISOR	21
#define   PROMOSHECHAS  22
#define   FRELPROMOS	23
#define   FRELPREMIN	24
#define   NSECUEN		25
#define   VARCONPP      26
#define   APPLYVARPP    27
#define   HABITRACKQ    28
#define   SEQRETIRO		29

#define   SACAREPEL     30
#define   DESDEREPEL    31
#define   HASTAREPEL	32

//   Estructura para seguir la pista a las variables.

struct cmos_ {
    void *vari;             // Posicion de la variable real.
    long alloc;             // Posicion asignada en la cmos.
    int len;                // Bytes que ocupa.
	int real_len;			// Largo real de la variable, para validar.
};
#endif
