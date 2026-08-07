
//   Declaración de la clase y objeto que maneja el impresor virtual.
class PrinterDummy : public PrinterFiscal_
{
	int PrinterUpLoad()
	{
		PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC;
		return 0;
	}

	int GetTickNro()
	{
		return 66666;
	}
	void Owner(char *cuit, char *pv)
	{
		cuit = pv = NULL;
	}
};



//   Clase que encapsula el manejo de este impresor.
class PrinterPR5 : public PrinterPR4 {

protected:
	void PrepareCustomerData(char *buff);	// Agrega el campo de direccion
	void PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaUni);
	char *HasarBrand(void);
};

