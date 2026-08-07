/*   Net.h   */

#include "bitmap.h"

///////////////////////////// netproto.c /////////////////////////////////
int      InitNet(void);
void     TerminateNet(void);
void     netProto(char *ptr);
void     net_SoftInit(void);
void     *ConstructList(char *);
void     SendFileToRemote(char *, int);
void     RecvFileFromRemote(char *, int);
void     SendMessage(char *, int);

//////////////////////////////////////////////////////////////////////////
//   Variables propias de la implementacion

extern int net_finalizar, net_finalizado;
extern int my_Nro, my_Lock, my_PCNro, my_Broad, my_Testigo, IamTest, my_Delay;
extern char  *my_Linea,  *my_Usa, TxInProgress, *comando, *cajas;
extern int ConnectSource;
extern unsigned uQueue, uDest;
