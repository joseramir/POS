//   Clase que encapsula el manejo de este impresor.
//#ifndef __PRINTER_TERMPR250_INCLUDED__
//#define __PRINTER_TERMPR250_INCLUDED__
//

class PrinterTermica : public PrinterFiscal_ {
private:
	// Definimos el tamaño del fragmento
	//static const int CHUNK_SIZE = 128;
	//char mfld[10000];

	PrinterConfig cfg;
	ReceiptHeader recHeader;
	Receipt recibo;
	ThermalPrinter* myPrinter;
	ReceiptFooter recFooter;
	


	long proteClose;
	int  availHeaderLine;
	bool abierto;
	int handImpre;
	//char bufImpre[15000];
	//char ultCodError[100];


	bool pieChanged;	
	int MAXLENCAR; //para almacenar el maximo de caracteres por linea

	//struct lstat_ lstatPR4;
	char *pc;                // Para explorar respuestas.
	


	char c_cuit[50], tDoc[10];

	long EstadoAuxiliar, EstadoInterno;	
	int docFiscStat();	//para controlar si existe un documento fiscal abierto
	void ConsultaUltimoError();
	//char *splitChar(char **text);
	//unsigned long sgethexa(char **text);  
	//unsigned long sgetdec(char **text);
	//void EnviaRecibePaquete(char *msg);	
	int CargaImagenLogo();

	bool CheckResult(int result);

public:
	PrinterTermica();
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
	int GeneraReporteAfip(int fdesde, int fhasta);
	int GeneraReporteAfipNew(int fdesde, int fhasta);
	int CopiaDocumento(int tipoDocumento, int nroDocumento);
	void FastVoucher(bool pHeader, char *l1, char *l2, char *l3);
	

protected:
	virtual void CompleteInitialization();			
	//virtual void PrepareCustomerData(char *buff);	// Nueva en el PR4, sirve para ser reemplazada en PR5
	virtual void PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto);
	virtual char *HasarBrand(void);

	virtual char *ChkDesP(char *src, char *dst);	// Chequea la validez de descripciones.
	virtual char *ChkDesP2(char *src, char *dst);	// Chequea la validez de descripciones.
	virtual bool ChkTextGran(char *s);
};

//#endif