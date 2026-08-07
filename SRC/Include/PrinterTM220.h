#ifndef __PRINTER_TM220_INCLUDED__
#define __PRINTER_TM220_INCLUDED__

class PrinterTM220 : public PrinterFiscal_ 
{
private:
	double redondeo[10];
	double rtasa[10];
	char   otradesc[50];
	char   otradesc2[50];
	char   pie[50];
	char   pie2[50];
	char   pie3[50];
	char   contpie2[50];
	char   contpie3[50];
	bool   hdrChanged, inVoucher;
	int    availHeaderLine;
	bool   pieChanged;
	char tdes[100];	

	void ImprimeAuxiliar(char *msg);
	int chkRta(int rta);

protected:
	char vendor[40];
	virtual void CompleteInitialization(void);
	virtual char *PrepararTexto(char *texto)  { return PrepararTexto(texto, false); }
	virtual char *PrepararTexto(char *texto, bool centrar);

public:
	PrinterTM220();
	char *ChkDes(char *s, char *dst);
	int  chkFiscStat(void);
	int  chkPrnStat(void);
	int  ChkPrnXonXoff(void);
	int  OpenDrwr(void);
	int  PrinterUpLoad(void);
    int  PrinterTM220::FiscalInit(char *cuit, char *rs, char *regnro, char *fec, char *ptovta,
							   char *initAct, char *ibrut, char *respon);
	int  PrintEncab();
	int  PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto,
		           int variac, char *ades, double unitario, bool forzarCant, int cajaUni);
	int  PrintSubtot(char *msg, int pFlag);
	int  PrintMPago(char *msg, double monto);
	int  PrintClose();
	int  PrintDescto(char *msg, double monto);
	int  PrintCancel(void);
	virtual int PrintZeta(void);
	int	 PrintText(char *msg);
	int  OpenVoucher(void);
	int  OpenVoucher2(void);
	int  CloseVoucher(void);
	int  CloseVoucher2(void);
	int  PrintVoucher(char *des);
	int  SetPie(char *msg);
	int  SetPie(char *msg, int linea);
	//int SetPie2(char *msg);
	int  SetHeader(char *msg);
	int  GetTickNro(int);
	void CheckStatus();
	void AjustarRedondeo(double);
	int PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp); ///Imprime la Percepcion
	int PrinterTM220::PrintRecargo(char *msg, double monto); ///Imprime un recargo sobre el total
	void ImprimirTextoDeVerdad(void);
	void Owner(char *cuit, char *pv);
	void InstallLogo(char *filename);
	void FastVoucher(bool pHeader, char *l1, char *l2, char *l3);
	void ObtenerAbierto(int &numticket, double &monto);
	/*void VoucherTJOnline(
		char *marca,
		char *tarjeta,
		char *nombre,
		char *vencimiento,
		char *lote,
		char *cupon,
		char *autorizacion, 
		double monto,
		int cuotas);*/
	int docFiscStat(); //controla si hay un documento fiscal abierto	
};

#endif __PRINTER_TM220_INCLUDED__
