/*    Event.h   */

int  initEvent(void);
void eventDispatch(void);
void addEvent(int, char *, unsigned);
bool PeekEvent(int clase, char *param);		// param debe tener por lo menos 32 caracteres
char *removeEvent(int);
void defineEvent(int, void (*)());

/*   Predefinicion de eventos   */

#define E_KEYB   1           /*  Interrupciones por teclado  */

