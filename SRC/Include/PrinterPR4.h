//   Clase que encapsula el manejo de este impresor.
#ifndef __PRINTER_PR4_INCLUDED__
#define __PRINTER_PR4_INCLUDED__

class PrinterPR4 : public PrinterFiscal_ {
private:
	long proteClose;
	int  availHeaderLine;
	bool abierto;

	bool pieChanged;

	struct lstat_ lstatPR4;
	char *pc;                // Para explorar respuestas.
	//static int inTickPR4;           // Indica que el PR4 tiene ticket abierto.
	long _ultimoBcompleto, _ultimoAcompleto;

	char /*hdrPkt[400],*/ c_cuit[50], tDoc[10];
	int docFiscStat();	//para controlar si existe un documento fiscal abierto


public:
	int  ChkPrnXonXoff(void);
	int  OpenDrwr(void);
	int  PrinterUpLoad(void);
	int  PrintDescto(char *msg, double monto);
	int  PrintEncab();
	int  PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto, 
		           int variac, char *ades, double unitario, bool pesado, int cajaUni);
	int  PrintSubtot(char *msg, int pFlag);
	int  PrintMPago(char *msg, double monto);
	int  PrintClose();
	int  PrintCancel(void);
	int  PrintZeta(void);
    int  PrintText(char *msg);
    int  OpenVoucher(void);          // Abre un CNF
	int  OpenVoucher2(void);          // Abre un CNF
    int  PrintVoucher(char *des);    // Imprime texto en un CNF
    int  CloseVoucher(void);         // Cierra un CNF.
	int  CloseVoucher2(void);         // Cierra un CNF.
    int  chkPrnStat();
    int  chkFiscStat();
	int  SetPie(char *msg);
	int  SetPie(char *msg, int linea);
	int  SetHeader(char *msg);
	int  GetTickNro(int cual);
	void Configure(char *confParam);
	void CheckStatus();
	void Owner(char *cuit, char *pv);
	int PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp);

protected:
	virtual void CompleteInitialization();			
	virtual void PrepareCustomerData(char *buff);	// Nueva en el PR4, sirve para ser reemplazada en PR5
	virtual void PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto);
	virtual char *HasarBrand(void);
};

#endif