#include <stdafx.h>
#include "funcs.h"               // Carga la estructura ven_

#include "frmIngVendMayor.h"

void IngVendedorMayor(int cual){
	//static struct DVendMay_ vendMay;

	if (ckaNro > 0){
		if(inOper == 0){
			IngNroVdor = true;
			frmIngVendMayor ^fvendmayor = gcnew frmIngVendMayor();
			if (fvendmayor->ShowDialog() == System::Windows::Forms::DialogResult::OK){		
				//vendMay.func = DVendMay;
				//vendMay.cajero = ckaNro;
				nroVendMay = fvendmayor->NroVendMayor == "" ? 0 : System::Convert::ToInt32(fvendmayor->NroVendMayor);				
				/*if (vendMay.nro	 > 0)
					ProcVendedorMayor(&vendMay, 1);*/
			}
			delete fvendmayor;
		}
		else
			Alert("No se puede pulsar la tecla en medio", "en medio de una venta");
	}
	else{
		Alert("No hay cajero habilitado.", "");
	}

}

void ProcVendedorMayor(void *fac5_, int fwrite)
{
	struct DVendMay_ *fac5;

	fac5 = (struct DVendMay_ *) fac5_;
	if (fwrite)	 
		WriteDump(fac5);
}