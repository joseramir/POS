/*   Serial.h   */

#define COM1    0
#define COM2    1
#define COM3    2
#define COM4    3

#define MOD_CTS  0x10
#define MOD_DSR  0x20
#define MOD_OUT1 0x40
#define MOD_OUT2 0x80

void txChar(int, char);
int  rxChar(int);
int  rxPoll(int);
void txStr(int, char *);
int  InitAllSerial(void);
void InstallSerial(int handl, char *secc);
void DeInitSerial(int handl);
void DeInitAllSerial(void);
void reviseSerial(void);
unsigned GetModemStat(int);
void FlushComm(int);

// Funcion de redireccion del scanner
extern void (*ScannerRedirectedFunc)(char *);		// scanner.cpp
int FitPattern(char *patt);							// Plu.cpp
