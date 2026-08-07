//
//   Miscs.c
//   Permite ingresar / egresar plata desde la caja por un concepto que
//   no sea venta.
//
#include <stdafx.h>
#include "funcs.h"

#ifdef POSXP
#include "MainForm.h"
#else
#include "wsup.h"
#endif

int cMisc;
char desc[26];
struct misc_  *miscs;
struct DDMisc_ dmisc;
extern ::ArrayList *mpreti;


//   La funcion que sigue es la que se utiliza para cuando se deprime la
//   tecla de varios relacionada.
int msccmp2(void const *k, void const *p)
{
    return *((int *) k) - ((struct mpag_ *) p)->nro;
}

struct misc_ *BuscaMisc(int cual)
{
	 return (misc_ *) bsearch(&cual, miscs, cMisc, sizeof(struct misc_), msccmp2);
}

void misc(int cual)
{
	struct misc_ *p;

	//   El siguiente bloque hace algunas validaciones, antes de dar por
	//   buena a la pulsada de la tecla.
	if(consmode) return;

	Choose *elanul = new Choose("Retiro/Dotacion", "ESC=Cancela", 20, 0, CHOOSE_ALIGN_LEFT, NULL);
	elanul->Add("RETIRO CAJA");
	//elanul->Add("Vuelto SOS Movil");
	elanul->Add("PRESTAMO EXTRACASH");
	cual = elanul->Anima();
	cual -=1;
	if (cual != 1 && cual != 2)
		return;
	p = BuscaMisc(cual);
	if(!p || inOper >= 1  || yReg != 0)
	{
		Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
		return;
	}
	if(chkClk()) return;

	// Asegura que la gaveta este abierta
	pf->OpenDrwr();

	//  Confirma el monto del retiro y el medio de cobro en el que se efectuara.
	GenForm *gf = new GenForm(p->nro == 1 ? " Retiros " : " Dotaciones " );
	EditItem *monto = new EditItem(p->nro == 1 ? "MONTO A RETIRAR" : "MONTO A INGRESAR");
	monto->CambiarTipo(TIPO_MONEDA);
	sprintf(monto->dato, "%.2lf", xReg);
	gf->Items()->Add(monto);

	// Limpia el visor principal de 'basura'
#ifdef POSXP
	MainForm::F->SetMultiDisplay("");
	MainForm::F->SetNumDisplay("");
	MainForm::F->SetTotDisplay("", NULL, NULL, NULL, NULL);
#else
	wSetColor(NEGRO * 16 + BLANCO);
	DispAdic(0, 1, "");
#endif

	// Anima la form de retiro
	int res = gf->Animar();

	// Procesa el resultado
	if(res > 0)
	{
		Trim(monto->dato);
		double mReti = atof_(monto->dato);
		if(mReti > 0)
		{
			DispCash(xReg, 2);
			int cualMP = (mpreti->Count() > 0)? atoi(mpreti->ItemString(0)): 1;
			if(mpreti->Count() > 1)
			{
				Choose *mpch = new Choose(" Retiros ", "ESC=Cancela", 0, 5, CHOOSE_ALIGN_LEFT, NULL);
				for(int i=0; i<mpreti->Count(); i++)
					mpch->Add(mpreti->ItemString(i));
				int opt = mpch->Anima();
				if(opt == 0)
					cualMP = -1;
				else
					cualMP = atoi(mpch->GetText(opt - 2));
				delete mpch;
			}

			//  Se puede cancelar en el paso anterior...
			if(cualMP > 0)
			{
				// Vouchers
				if(p->voucher > 0)
				{
					char tMonto[50], tFormatMonto[50];
					//sprintf(tMonto, "%.2lf", mReti);		// Monto del retiro formateado
					System::String ^montoFormat = System::String::Format("{0:0,0.00}", mReti);
					Strings::StringToChar(montoFormat, tMonto, 50);
					//pretty_number(tMonto, tFormatMonto);
					mpag_ *mp = BuscarMpag(cualMP);			// Medio de pago que retira
					VoucherVar("@CAJA@", caja);				// Variables de voucher...
					VoucherVar("@CAJERO@", clkName());
					VoucherVar("@SUPERV@", supervName());
					VoucherVar("@IMPORTE@", tMonto);
					VoucherVar("@MPAGO@", mp->des);
					NroRetiro++;
					WriteCmos(SEQRETIRO);
					VoucherVar("@SEQRETIRO@", NroRetiro);

					StackVoucher(p->voucher);				// Original
					StackVoucher(p->voucher + 1);			// Duplicado
				}

				//   Graba y borra la pantalla.
				sprintf(msg, "%s $%s", p->des, monto->dato);
#ifdef POSXP
				MainForm::F->SetMultiDisplay(msg);
				MainForm::F->SetNumDisplay("");
#else
				wSetColor(NEGRO * 16 + BLANCO);
				DispAdic(0, 1, msg);
#endif

				// Asienta
				dmisc.func = DMisc;
				dmisc.nro = cual;
				dmisc.cajero = ckaNro;
				dmisc.fecha = date();
				dmisc.hora = time__();
				store(mReti, dmisc.imp);
				dmisc.mpag = cualMP;
				dmisc.numRetiro = NroRetiro;
				ProcMisc((void *)&dmisc, 1);
			}
		}
	}

	delete gf;
	inOper = 0;
	xReg = yReg = 0;
	superv(0);
	HClear();
}


void ProcMisc(void *msc_, int fwrite)
{
    struct DDMisc_ *msc;

    msc = (struct DDMisc_ *) msc_;
	if(fwrite)
	{
		// Modifica el acumulador de dinero en gaveta.
		struct misc_ *m = BuscaMisc(msc->nro);
		double import = atof(msc->imp) * (double)m->signo;
		struct mpag_ *mp = BuscarMpag(msc->mpag);
		StoreGaveta(mp, import);

		// Graba datos en el trans.
		WriteDump(msc);
		WriteEOPNoFiscal();
	}
}


//   Carga la Definicion de teclas varias.
int msccmp1(void const *d1, void const *d2)
{
    return ((struct mpag_ *) d1)->nro - ((struct mpag_ *) d2)->nro;
}

int LoadMisc()
{
	 struct misc_ *msc;
	 char cod[4], des[40], signo[3], voucher[5];
	 dbf misc;

	 cMisc = 0;
	 if(misc.Use("pos\\varias"))
	 {
		 miscs = new misc_[misc.RecCount()];
		 misc.Go(DB_GO_TOP);
		 while(!misc.Eof())
		 {
			 misc.Get("codigo", cod);
			 misc.Get("des", des);
			 misc.Get("signo", signo);
			 misc.Get("voucher", voucher);

			 msc = &miscs[cMisc++];
			 msc->nro = atoi(cod);
			 msc->signo = atoi(signo);
			 msc->voucher = atoi(voucher);
			 STRCPY(msc->des, Trim(des));
			 misc.Skip(1);
		 }
		 misc.Close();
	 }
	 else
		 AbnormalEnd("No puedo cargar funciones varias");
	 if(cMisc)
		qsort(miscs, cMisc, sizeof(struct misc_), msccmp1);
     return 1;
}


/********************************************************************************************/

void extracash(int cualMP, double pmonto)
{
	struct DDExtraCash_ dextra;	
	
	// Asienta
	dextra.func = DExtraCash;				
	dextra.nmpago = cualMP;
	dextra.cajero = ckaNro;		
	dextra.destmpago = 1;
	dextra.fecha = date();
	dextra.hora = time__();
	store(pmonto, dextra.monto);				
	ProcExtraCash((void *)&dextra, 1);			
	
}

void ProcExtraCash(void *msc_, int fwrite)
{
    struct DDExtraCash_ *lextra;

    lextra = (struct DDExtraCash_ *) msc_;
	if(fwrite)
	{
		// Modifica el acumulador de dinero en gaveta.		
		double import = atof(lextra->monto) * (-1);
		struct mpag_ *mp = BuscarMpag(lextra->nmpago);
		StoreGaveta(mp, import);

		// Graba datos en el trans.
		WriteDump(lextra);
		///WriteEOPNoFiscal();
	}
}

//void extracash(int cual)
//{
//	struct DDExtraCash_ dextra;
//
//	//   El siguiente bloque hace algunas validaciones, antes de dar por
//	//   buena a la pulsada de la tecla.
//	if(consmode) return;
//	///p = BuscaMisc(cual);
//	if(inOper >= 1  || yReg != 0)
//	{
//		Alert("No puede utilizar la tecla pulsada", "En este momento!!!");
//		return;
//	}
//	if(chkClk()) return;
//
//	// Asegura que la gaveta este abierta
//	pf->OpenDrwr();
//
//	//  Confirma el monto del retiro y el medio de cobro en el que se efectuara.
//	GenForm *gf = new GenForm(" Extra Cash ");
//	EditItem *monto = new EditItem("MONTO A RETIRAR");
//	monto->CambiarTipo(TIPO_MONEDA);
//	sprintf(monto->dato, "%.2lf", xReg);
//	gf->Items()->Add(monto);
//
//	// Limpia el visor principal de 'basura'
//	MainForm::F->SetMultiDisplay("");
//	MainForm::F->SetNumDisplay("");
//	MainForm::F->SetTotDisplay("", NULL, NULL, NULL, NULL);
//
//	// Anima la form de retiro
//	int res = gf->Animar();
//
//	// Procesa el resultado
//	if(res > 0)
//	{
//		Trim(monto->dato);
//		double mReti = atof_(monto->dato);
//		if(mReti > 0)
//		{
//			DispCash(xReg, 2);
//			/*int cualMP = (mpreti->Count() > 0)? atoi(mpreti->ItemString(0)): 1;
//			if(mpreti->Count() > 1)
//			{
//				Choose *mpch = new Choose(" Retiros ", "ESC=Cancela", 0, 5, CHOOSE_ALIGN_LEFT, NULL);
//				for(int i=0; i<mpreti->Count(); i++)
//					mpch->Add(mpreti->ItemString(i));
//				int opt = mpch->Anima();
//				if(opt == 0)
//					cualMP = -1;
//				else
//					cualMP = atoi(mpch->GetText(opt - 2));
//				delete mpch;
//			}*/
//			int cualMP = 1; // Entregará EFECTIVO.
//
//			//  Se puede cancelar en el paso anterior...
//			if(cualMP > 0)
//			{
//				// Vouchers
//				/*if(p->voucher > 0)
//				{*/
//					char tMonto[50];
//					sprintf(tMonto, "%.2lf", mReti);		// Monto del retiro formateado
//					mpag_ *mp = BuscarMpag(cualMP);			// Medio de pago que retira
//					VoucherVar("@CAJA@", caja);				// Variables de voucher...
//					VoucherVar("@CAJERO@", clkName());
//					VoucherVar("@SUPERV@", supervName());
//					VoucherVar("@IMPORTE@", tMonto);
//					VoucherVar("@MPAGO@", mp->des);
//
//					StackVoucher(ecvoucher);///p->voucher);				// Original
//					StackVoucher(ecvoucher + 1);//p->voucher + 1);			// Duplicado
//				//}
//
//				//   Graba y borra la pantalla.
//				sprintf(msg, "Extra Cash $%s",  monto->dato);
//				MainForm::F->SetMultiDisplay(msg);
//				MainForm::F->SetNumDisplay("");
//
//				// Asienta
//				dextra.func = DExtraCash;				
//				dextra.nmpago = cualMP;
//				dextra.cajero = ckaNro;				
//				dextra.fecha = date();
//				dextra.hora = time__();
//				store(mReti, dextra.monto);				
//				ProcExtraCash((void *)&dextra, 1);
//			}
//		}
//	}
//
//	delete gf;
//	inOper = 0;
//	xReg = yReg = 0;
//	superv(0);
//	HClear();
//}
