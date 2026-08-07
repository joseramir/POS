/*    Ipx.h  - Estructuras del modulo IPX   */

#include "bitmap.h"

////////////////////////////////////////////////////////////////////////////
// La siguiente estructura codifica una direccion de ipx.

typedef struct ipxaddr_
{
    unsigned char netadd[4];            // Nro de la red.
    unsigned char nodeadd[6];           // Nro del nodo, en la red.
	unsigned short socket;              // Nro del socket que utiliza.
} IPXADDRESS;

///////////////////////////////////////////////////////////////////////////
//   La siguiente estructura codifica la informacion de header de un pa-
//   quete IPX. En estos paquetes, el header es un area al comienzo del
//   paquete.

typedef struct
{
    unsigned short checksum;            // Lo calcula IPX.
    unsigned short length;              // Lo calcula IPX.
    unsigned char tc;
    unsigned char type;
    IPXADDRESS dest;
    IPXADDRESS source;
} IPXHEADER;

/////////////////////////////////////////////////////////////////////////
//   La estructura ECB codifica toda la informacion necesaria para seguir
//   la pista de los paquetes enviados y los recibidos.

typedef struct
{
    void *linkaddress;
    void (*esraddress)();
    unsigned char inuse;
    unsigned char cc;
    unsigned short socket;
    unsigned char workspace[16];
    unsigned char immedaddr[6];
    unsigned short fragcount;
    void *fragaddr1;
    unsigned short fragsize1;
    void *fragaddr2;
    unsigned short fragsize2;
    void *fragaddr3;
	unsigned short fragsize3;
	int MyPacketSize;
} IPXECB;

struct rbuff_ {
	char used;
    char  *ptr;
} rbuff[];

////////////////////////////////////////////////////////////////////////////
//   Prototipos de las funciones en Assembler / C.

unsigned short ReverseWord(unsigned short);
unsigned int   ipx_Init(void);
void           ipx_GetAddress(void  *);
unsigned short ipx_GetMaxPacketSize(void);
unsigned       ipx_OpenSocket(unsigned);
void           ipx_CloseSocket(unsigned);
int            ipx_ListenForPacket(IPXECB  *);
int            ipx_SendPacket(IPXECB  *);
void           ipx_Iddle(void);

