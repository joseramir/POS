#pragma once

#ifdef POSXP

#include "DatosMpagos.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "StaticDump.h"
#endif

#include "Funcs.h"
#include "MainForm.h"
#include "total.h"
#include "tjonline.h"
//#include "frmExtracciones.h"
#include "PagoDatosTarj.h"
//#include "frmVuelto.h"
//#include "frmIngresaCod.h"
#include "frmTarjVerifone.h"

//#include "frmOcPromo.h"
#include "frmCargaEfectivo.h"
#include "frmTarjSmartPointMp.h"
#include "frmPrismaECR.h"
#include "frmMpCashout.h"


#ifdef POSXP
namespace pos {

	/// <summary>
	/// Resumen de PagoForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class PagoForm : public Controles::BaseDialog
	{
	private:

		int faltaCount;

		bool saltearActivacion;

		Decimal saldoMutComodin;
		Decimal saldoOriMutComodin;
		bool    oriUsatoMut;

		bool inActuDynStatus;
	private: System::Windows::Forms::Label^  lblDesctoTipre;


		Decimal _tot;
		Decimal miTotal;
		System::Decimal TotPagado() { return _tot; }		

		List<MP ^> ^pagos;

		Panel^  summary;

		Label^  lbSaldo;

		Label^  lbIngre;

		Label^  lbTotal;

		Button^ tarjOnline;
		Button^ tarjPrisma;
		Button^ tarjSmartPointMp;
		Button^ btMercadoPago;

		Button^ bVuelto;

		//Decimal vueltoIng;
		Decimal montoEfectivo; //Para Saber si hay efectivo ingresado como Medio de Pago asi se puede cargar Vuelto

		bool root;

		System::Drawing::Font ^titFont;

		List<TextBox ^> ^dynTotal;

		//MDescMedioPago ^pfdescuentos;

	private: System::Windows::Forms::Label^  lbFalta;
	private: System::Windows::Forms::Timer^  timerFaltante;
	private: System::Windows::Forms::Label^  lblPiibb;


	bool procesarClicks;
	

	public:

		Decimal DynamicTotal()
		{
			Decimal res = _tot;
			for(int i=0; i<pagos->Count; i++)
			{
				MP ^mp = pagos[i];
				/*if (mp->PagoExt->mp->percentRecargo > 0)
					mp->PagoExt->variacion = (mp->PagoExt->monto * mp->PagoExt->mp->percentRecargo / 100);	*/			
				res += Decimal(mp->PagoExt->variacion);
			}
			//res -= Decimal(desctoTipre);
			return res;
		}

		void PrepareLabel(String ^nom, int row)
		{
			Label ^titulo = gcnew Label();
			titulo->Font = titFont;
			titulo->Text = nom;
			titulo->AutoSize = true;
			titulo->BackColor = Color::White;
			titulo->Anchor = AnchorStyles::Right;
			tbl->Controls->Add(titulo);
			tbl->SetColumn(titulo, 0);
			tbl->SetRow(titulo, row);
		}

		

		// Agrega el boton de tarjeta online, cuando corresponde.
		void AddTjPrisma(int row)
		{
			PrepareLabel("Tarjetas Prisma: ", row);

			Button ^bt = gcnew Button();
			bt->BackColor = Color::BurlyWood;
			bt->TextAlign = ContentAlignment::MiddleRight;
			bt->Text = TotalTarjetasPrisma(false).ToString("F2");
			bt->Font = titFont;
			bt->Height = 40;
			bt->Width = 150;
			bt->Anchor = AnchorStyles::None;

			//			bt->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
			bt->Click += gcnew EventHandler(this, &pos::PagoForm::TjPrismaClick);

			tb = bt;
			tarjPrisma = bt;
			tbl->Controls->Add(tb);
			tbl->SetColumn(tb, 2);
			tbl->SetRow(tb, row);
		}

		void AddTjECRPrisma(int row)
		{
			PrepareLabel("Tarjetas ECR Prisma: ", row);

			Button ^bt = gcnew Button();
			bt->BackColor = Color::BurlyWood;
			bt->TextAlign = ContentAlignment::MiddleRight;
			bt->Text = TotalTarjetasPrismaECR(false).ToString("F2");
			//bt->Text = TotalTarjetasTipre(false).ToString("N2");
			bt->Font = titFont;
			bt->Height = 40;
			bt->Width = 150;
			bt->Anchor = AnchorStyles::None;

			//			bt->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
			bt->Click += gcnew EventHandler(this, &pos::PagoForm::TjECRPrismaClick);

			tb = bt;
			tarjPrisma = bt;
			tbl->Controls->Add(tb);
			tbl->SetColumn(tb, 2);
			tbl->SetRow(tb, row);
		}

		//void AddTjSmartPointMp(int row)
		//{
		//	PrepareLabel("Smart Point: ", row);

		//	Button ^bt = gcnew Button();
		//	bt->BackColor = Color::BurlyWood;
		//	bt->TextAlign = ContentAlignment::MiddleRight;
		//	bt->Text = TotalTarjetasSmartPointMp(false).ToString("F2");
		//	//bt->Text = TotalTarjetasTipre(false).ToString("N2");
		//	bt->Font = titFont;
		//	bt->Height = 40;
		//	bt->Width = 150;
		//	bt->Anchor = AnchorStyles::None;

		//	//			bt->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
		//	bt->Click += gcnew EventHandler(this, &pos::PagoForm::TjSmartPointClick);

		//	tb = bt;
		//	tarjSmartPointMp = bt;
		//	tbl->Controls->Add(tb);
		//	tbl->SetColumn(tb, 2);
		//	tbl->SetRow(tb, row);
		//}

		
		// Construye un medio de pago normal. El control de edicion creado create en la variable 'tb'
		void AddMPagoToList(class PagoExtendido *pex, int index, int row)
		{
			// Determina el 'PagoExtendido' a utilizar
			PagoExtendido *pe = pex->items[index];
			if (pe == NULL)
				return;
			if(pe->mp->tipo == 'Z') return;
			if (pe->mp->desctomp && Dump::promosmp->GetCantidad(pe->mp->nro) <= 0)
				return;

			if (strcmp(pe->mp->grptot, "13-MUTUALCOM    ") == 0){
				if (!janisEcom)
					return;
				if (Dump::actCliente == nullptr)
					return;
				if (Dump::actCliente->Repa != 9)
					return;
				saldoOriMutComodin = Dump::actCliente->SaldoMutual;
				oriUsatoMut = Dump::actCliente->UsadoMutual;
				if (Dump::actCliente->UsadoMutual)
					return;
				
			}

			// Prepara el nombre con el indicativo del descuento/recargo a efectuar
			String ^textPorcen = String::Empty;
			if(pe->subItems == 0 && pe->mp != NULL && pe->mp->descto != 0)
			{
				textPorcen = 
					" (" + 
					((pe->mp->descto > 0)? "+": "") +
					Decimal(pe->mp->descto).ToString("F2") + 
					"%)";
			}

			// Construye el MP que utilizara para rastrear el problema
			MP ^mp = gcnew MP();
			pagos->Add(mp);
			mp->Nombre = (Strings::CharToString((pe->subItems != 0)?
					pe->mp->grptot: 
					pe->mp->des)->Trim() + textPorcen + ": ")->ToUpper();
			mp->PagoExt = pe;
			mp->DesctoMp = Dump::promosmp->GetMontoDescSinConfirmar(pe->mp->nro);
			mp->ImpSinDesctoMp = System::Convert::ToDecimal(Dump::promosmp->GetImporteSinDescto(pe->mp->nro));

			// Crea y agrega la etiqueta
			PrepareLabel(mp->Nombre, row);

			// Prepara y asigna el textbox o boton a usar.
			tb = nullptr;
			
			if (mp->Nombre->ToUpper()->StartsWith("SMARTPOINT MP"))
			{
				//PrepareLabel("Smart Point: ", row);

				Button ^bt = gcnew Button();
				bt->BackColor = Color::BurlyWood;
				bt->TextAlign = ContentAlignment::MiddleRight;
				bt->Text = TotalTarjetasSmartPointMp(false).ToString("F2");
				bt->Font = titFont;
				bt->Height = 40;
				bt->Width = 150;
				bt->Anchor = AnchorStyles::None;

				//			bt->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
				bt->Click += gcnew EventHandler(this, &pos::PagoForm::TjSmartPointClick);

				tb = bt;
				tarjSmartPointMp = bt;
				/*tbl->Controls->Add(tb);
				tbl->SetColumn(tb, 2);
				tbl->SetRow(tb, row);*/
			}else if (mpCashout && mp->Nombre->ToUpper()->StartsWith("MERCADOPAGO-QR"))
			{
				//PrepareLabel("Smart Point: ", row);

				Button ^bt = gcnew Button();
				bt->BackColor = Color::BurlyWood;
				bt->TextAlign = ContentAlignment::MiddleRight;
				bt->Text = TotalMpQrCashout(false).ToString("F2");
				bt->Font = titFont;
				bt->Height = 40;
				bt->Width = 150;
				bt->Anchor = AnchorStyles::None;

				//			bt->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
				bt->Click += gcnew EventHandler(this, &pos::PagoForm::MercadoPagoCashoutClick);

				tb = bt;
				btMercadoPago = bt;
				/*tbl->Controls->Add(tb);
				tbl->SetColumn(tb, 2);
				tbl->SetRow(tb, row);*/
			}
#ifdef FACTSQL
			else if (mp->Nombre->ToUpper()->StartsWith("EFECTIVO $: ") && mp->PagoExt->mp->mlimreca > 0 &&
				this->_tot > System::Convert::ToDecimal(mp->PagoExt->mp->mlimreca))
			{								
				Button ^bt = gcnew Button();
				bt->BackColor = Color::BurlyWood;
				bt->TextAlign = ContentAlignment::MiddleRight;
				bt->Click += gcnew EventHandler(this, &pos::PagoForm::CobraEfectivoRecaClick);
				tb = bt;				
			}
#endif			
			else if(pe->subItems == 0 && !pe->mp->extraible)
			{				
				TextBox ^textBox = gcnew TextBox();
				textBox->TextAlign = HorizontalAlignment::Right;
				textBox->Click += gcnew EventHandler(this, &pos::PagoForm::TbClick);

				// Mira si corresponde agregar el textbox que maneja recargos/descuentos
				if(pe->mp->descto != 0)
				{
					TextBox ^totConVariac = gcnew TextBox();
					totConVariac->ReadOnly = false;
					totConVariac->TextAlign = HorizontalAlignment::Left;
					totConVariac->Font = titFont;
					totConVariac->Height = 56;
					totConVariac->Anchor = AnchorStyles::Left;
					totConVariac->BackColor = tbl->BackColor;
					totConVariac->BorderStyle = BorderStyle::None;
					totConVariac->Visible = true;
					totConVariac->AutoSize = true;
					totConVariac->Text = ".";

					dynTotal->Add(totConVariac);
					pe->dynTotalIndex = dynTotal->Count - 1;

					tbl->Controls->Add(totConVariac);
					tbl->SetColumn(totConVariac, 1);
					tbl->SetRow(totConVariac, row);
				}

				// Los eventos solo se usan en la version con teclado.   FEDE Ver
				textBox->GotFocus += gcnew EventHandler(this, &pos::PagoForm::TbGotFocus);
				textBox->LostFocus += gcnew EventHandler(this, &pos::PagoForm::TbLostFocus);
				// Fin FEDE Ver


				tb = textBox;
			}			
			else
			{
				Button ^bt = gcnew Button();
				bt->BackColor = Color::BurlyWood;
				bt->TextAlign = ContentAlignment::MiddleRight;
				bt->Click += gcnew EventHandler(this, &pos::PagoForm::BtClick);
				tb = bt;
			}

			tb->Tag = mp;
			mp->Handler = tb;
			tb->Text = "0";// pe->monto == 0 ? "0" : pe->monto.ToString("N2");
			MP ^tempMP = ((MP ^) tb->Tag);
			tempMP->Monto = 0;
			tempMP->PagoExt->monto = 0;
			tempMP->PagoExt->variacion = 0;

			//tb->Text = ((MP ^) tb->Tag)->Monto.ToString("N2");
			tb->Font = titFont;
			tb->Height = 40;
			tb->Width = 150;
			tb->Anchor = AnchorStyles::None;
			tb->TextChanged += gcnew EventHandler(this, &pos::PagoForm::TbTextChanged);
			tbl->Controls->Add(tb);
			tbl->SetColumn(tb, 2);
			tbl->SetRow(tb, row);
		}

		// *******************************************************************************************************
		// **   CONSTRUCTOR 
		// *******************************************************************************************************
		//PagoForm(class PagoExtendido *pex, Decimal total, bool esRoot, Decimal piibb)
		PagoForm(class PagoExtendido *pex, Decimal total, bool esRoot)
		{
			InitializeComponent();

			//this->Shown	+= gcnew System::EventHandler(this, &pos::PagoForm::SetParent);

			dynTotal = gcnew List<TextBox ^>();
			saltearActivacion = false;
			total = Decimal::Round(total, 2);
			mpTotal = System::Convert::ToDouble(total);
			root = esRoot;
			procesarClicks = true;
			if(!root)
			{
				summary->Visible = false;
				panel->BackColor = Color::SteelBlue;
				Titulo->Text = "Ingrese " + Strings::CharToString(pex->mp->grptot);
			}

			pagos = gcnew List<MP ^>();
			_tot = total;
			miTotal = total;
			//pfdescuentos = gcnew MDescMedioPago();
			titFont = gcnew System::Drawing::Font("Tahoma", 9, System::Drawing::FontStyle::Bold);
			int row = 0;
			if (esRoot)
			{					
				this->lblPiibb->Text = "";
				Decimal piibb = Decimal(xTotal.VerRecargo(10002).ToDouble());
				if (piibb > 0)
				{					
					this->lblPiibb->Text = "IIBB RG 974: " + piibb.ToString("F2");
					this->lblPiibb->Visible = true;				
				}
				Decimal ptissh = 0;
				if (total >= Convert::ToDecimal(MinimoTissh))
					ptissh = Decimal(xTotal.VerRecargo(10003).ToDouble());
				if (piibb > 0 && ptissh > 0)
				{
					this->lblPiibb->Text += "\n";
					this->lblPiibb->Height = 46;
				}
				if (ptissh > 0)
				{
					this->lblPiibb->Text += "TISSH Salta: " + ptissh.ToString("F2");
					this->lblPiibb->Visible = true;
				}

				Decimal pPerIva = 0;
				pPerIva = Decimal(xTotal.VerRecargo(10004).ToDouble());
				if ((piibb > 0 || ptissh > 0) && pPerIva > 0)
				{
					this->lblPiibb->Text += "\n";
					this->lblPiibb->Height = 46;
				}
				if (pPerIva > 0)
				{
					this->lblPiibb->Text += "Percep. IVA: " + pPerIva.ToString("F2");
					this->lblPiibb->Visible = true;
				}

				saldoMutComodin = 0;
			}
			int limite = (tjOnline && root) ? (pex->subItems + 1) : pex->subItems;
			//limite = (tjPrisma && root) ? limite + (tjPrisma + tjSmartPoint) : limite;
			limite = (tjPrisma && root) ? limite + (tjPrisma) : limite;
			limite = (EcrPrisma && root) ? limite + 1 : limite;
			//limite = (tjSmartPoint && root) ? limite + (tjSmartPoint) : limite;
			int j = 0;
			for(j=0; j< limite; j++) // j<pex->subItems; j++)
			{
				if(limite > 1 && j == 1 && tjPrisma && root)
					AddTjPrisma(j);		
				else if(limite > 1 && j == 1 && EcrPrisma && root)
					AddTjECRPrisma(j);		
				/*else if(limite > 1 && j == 2 && tjSmartPoint && root)
					AddTjSmartPointMp(j);*/
				else
					AddMPagoToList(pex, row++, j);				

				
				// Manda el cursor al primer textbox
				this->TabControls->Add(tb);
				if(j == 0)
					((MP ^)tb->Tag)->Borrar = true;
				

			}
			
			this->ExtendedKeyboardEnabled = usaTouch;
			Strings::InterceptKeyboard(this, 1);
			ActuStatus();
			ActuDynStatus();
		}

	protected:

		void SetParent(System::Object ^sender, System::EventArgs ^e)
		{
			if (MainForm::F != nullptr)
			{
				Control::CheckForIllegalCrossThreadCalls = false;
				this->Owner = MainForm::F; /* Owning form here */;
				//		form.CenterToParent();      // Not necessary
				Control::CheckForIllegalCrossThreadCalls = true;
				this->TopMost = true;        // Works now!			
			}

		}

		void TbTextChanged(Object ^sender, EventArgs ^e)
		{
			ActuStatus();
		}

		void TbClick(Object ^sender, EventArgs ^e)
		{
			if(procesarClicks)
			{
				ActuStatus();
				TextBox ^utb = (TextBox ^)sender;
				if(lbSaldo->Text->StartsWith("Faltante:"))
				{
					Decimal dec = Decimal(0);
					try { dec = Decimal::Parse(lbSaldo->Text->Substring(9)); }
					catch(Exception ^) {}

					if(dec != 0)
					{
						MP ^mp = (MP ^)utb->Tag;
						//mp->Monto = dec;
						//mp->Automatico = dec;
						//utb->Text = dec.ToString("N2");
						mp->Borrar = true;
						ActuStatus();
					}					
				}
			}
		}

		//para Acumular los totales de los verifones
		System::Decimal TotalTarjetasEnVerifones(bool SoloRecargo){
			return System::Convert::ToDecimal(TotalTarjetasPrisma(SoloRecargo)) + 
				System::Convert::ToDecimal(TotalTarjetasSmartPointMp(SoloRecargo)) +
				System::Convert::ToDecimal(TotalTarjetasPrismaECR(SoloRecargo));
		}

		
		void TjPrismaClick(Object ^sender, EventArgs ^e)
		{
			Decimal ingre = DynamicTotal() - (CalcIngresos() - TotalTarjetasEnVerifones(true));
			if (ingre > 0)
			{			
				frmTarjVerifone ^fTarjPrisma = gcnew frmTarjVerifone(ingre);
				fTarjPrisma->ShowDialog();

				tarjPrisma->Text = TotalTarjetasPrisma(false).ToString("F2");
				ActuStatus();
			}
			else
				Alert("No hay monto pendiente", "Para Autorizar Tarjeta");
		}

		void TjSmartPointClick(Object ^sender, EventArgs ^e)
		{
			Decimal ingre = DynamicTotal() - (CalcIngresos() - TotalTarjetasEnVerifones(true));
			if (ingre > 0)
			{			
				frmTarjSmartPointMp ^fTarjSmartPoint = gcnew frmTarjSmartPointMp(ingre);
				fTarjSmartPoint->ShowDialog();

				Button ^bt = (Button ^) sender;
				MP ^mp = (MP ^) bt->Tag;
				PagoExtendido *pex = mp->PagoExt;	
				mp->Monto = System::Convert::ToDecimal(TotalTarjetasSmartPointMp(false));
				bt->Text = mp->Monto.ToString("F2");
				procesarClicks = false;

				//tarjSmartPointMp->Text = TotalTarjetasSmartPointMp(false).ToString("F2");
				ActuStatus();
				SetFocusTo(bt);
			}
			else
				Alert("No hay monto pendiente", "Para Autorizar Tarjeta");
		}

		void MercadoPagoCashoutClick(Object ^sender, EventArgs ^e)
		{
			Decimal ingre = DynamicTotal() - (CalcIngresos() - TotalTarjetasEnVerifones(true));
			if (ingre > 0)
			{			
				frmMpCashout ^fCashout = gcnew frmMpCashout(ingre);
				fCashout->ShowDialog();

				Button ^bt = (Button ^) sender;
				MP ^mp = (MP ^) bt->Tag;
				PagoExtendido *pex = mp->PagoExt;	
				mp->Monto = System::Convert::ToDecimal(TotalMpQrCashout(false));
				bt->Text = mp->Monto.ToString("F2");
				procesarClicks = false;

				//tarjSmartPointMp->Text = TotalTarjetasSmartPointMp(false).ToString("F2");
				ActuStatus();
				SetFocusTo(bt);
			}
			else
				Alert("No hay monto pendiente", "Para Autorizar Tarjeta");
		}

		void TjECRPrismaClick(Object ^sender, EventArgs ^e)
		{
			Decimal ingre = DynamicTotal() - (CalcIngresos() - TotalTarjetasEnVerifones(true));
			if (ingre > 0)
			{			
				frmPrismaECR ^fTarjECRPrisma = gcnew frmPrismaECR(ingre);
				fTarjECRPrisma->ShowDialog();

				tarjPrisma->Text = TotalTarjetasPrismaECR(false).ToString("F2");
				ActuStatus();
			}
			else
				Alert("No hay monto pendiente", "Para Autorizar Tarjeta");
		}

						

		void CobraEfectivoRecaClick(Object ^sender, EventArgs ^e)
		{
			bool bcupones = false;
			ActuStatus();
			Decimal dec = Decimal(0);
			try { dec = Decimal::Parse(lbSaldo->Text->Substring(9)); }
			catch(Exception ^) {}

			Button ^bt = (Button ^) sender;
			MP ^mp = (MP ^) bt->Tag;
			PagoExtendido *pex = mp->PagoExt;

			Decimal ingre = DynamicTotal() - CalcIngresos();

			xTotal.AddRecargo(10005, BDecimal::BDecimal(0));
			frmCargaEfectivo ^loadEfectivo = gcnew frmCargaEfectivo(ingre, System::Convert::ToDecimal(mp->PagoExt->mp->mlimreca), System::Convert::ToDecimal(mp->PagoExt->mp->alirecargo), pex);
			loadEfectivo->ShowDialog();

			Decimal auxingre = Decimal(0);
			auxingre = loadEfectivo->MontoIngresado;
			mp->Monto = auxingre;
			mp->PagoExt->variacion = System::Convert::ToDouble(loadEfectivo->MontoRecargo);
			xTotal.AddRecargo(10005, BDecimal::BDecimal(System::Convert::ToDouble(loadEfectivo->MontoRecargo)));
			bt->Text = mp->Monto.ToString("F2");

			procesarClicks = false;
			ActuStatus();

			SetFocusTo(bt);
		}
		

		void BtClick(Object ^sender, EventArgs ^e)
		{
			bool bcupones = false;
			ActuStatus();
			Decimal dec = Decimal(0);
			try { dec = Decimal::Parse(lbSaldo->Text->Substring(9)); }
			catch(Exception ^) {}

			Button ^bt = (Button ^) sender;
			MP ^mp = (MP ^) bt->Tag;
			PagoExtendido *pex = mp->PagoExt;
			
			if (mp->Nombre == "2-TARJETAS: " || mp->Nombre == "4-TARJ.DEBITO: ")
			{
				//PagoTarjetas ^tform = gcnew PagoTarjetas(pex, dec, true);
				bcupones = true;
				PagoDatosTarj ^tform = gcnew PagoDatosTarj(pex, dec, mp->Nombre == "2-TARJETAS: ");
				tform->ShowDialog();
				delete tform;
				if (GCStatic::tjPosnet->Count == 0)
				{
					for each(MP ^mp2 in pagos)
					{
						if (mp2->Nombre == "2-TARJETAS: " || mp2->Nombre == "4-TARJ.DEBITO: ") 
						{
							mp2->PagoExt->monto = 0;
							mp2->PagoExt->variacion = 0;
							for(int k=0; k<mp2->PagoExt->subItems; k++)
							{
								PagoExtendido *pe2 = mp2->PagoExt->items[k];
								pe2->monto = 0;
								pe2->variacion = 0;
							}
							Button ^bt2 = (Button ^) mp2->Handler;
							bt2->Text = "0";
						}
					}
				}				
			}
			else{
				//PagoForm ^subForm = gcnew PagoForm(pex, dec, false, Decimal(0));
				PagoForm ^subForm = gcnew PagoForm(pex, dec, false);
				subForm->ShowDialog();
				delete subForm;
			}

			double ingre = 0;
			mp->PagoExt->variacion = 0;			
			for(int j=0; j<pex->subItems; j++)
			{
				PagoExtendido *pe = pex->items[j];
				
				if (bcupones)
				{				
					for each(CPosnet ^posnet in GCStatic::tjPosnet)
					{
						if (pe->mp->nro == posnet->m_mpago)// && posnet->m_recargo > 0)
						{
							ingre += (posnet->m_monto + posnet->m_recargo);
							mp->PagoExt->variacion += posnet->m_recargo;
						}
					}
				}
				else
				{
					ingre += pe->monto;				
					mp->PagoExt->variacion += pe->variacion;
				}
			}

			mp->Monto = Decimal(ingre);
			bt->Text = mp->Monto.ToString("F2");
			procesarClicks = false;
			ActuStatus();

			SetFocusTo(bt);
		}

		bool inGotFocus, inLostFocus;

		void TbLostFocus(Object ^sender, EventArgs ^e)
		{
			if(inLostFocus) return;
			inLostFocus = true;

			Control ^lossFocus = dynamic_cast<Control ^>(sender);
			if(lossFocus != nullptr)
			{
				Decimal dec;
				if(lossFocus->Text->Length == 0)
					dec = 0;
				else
				{
					try { dec = Decimal::Parse(lossFocus->Text); } 
					catch(Exception ^) { dec = Decimal(0); }
				}

				MP ^mp = (MP ^) lossFocus->Tag;
				if(mp->Automatico == dec) dec = 0;
				//if (dec == 0)

				Decimal xpendiente = GetPendiente();
				if (mp->DesctoMp > 0)
				{
					if (dec > 0)
					{					
						//if (dec < (miTotal - mp->DesctoMp))
						if (dec < (xpendiente - mp->DesctoMp))
						{
							//_tot = _tot + mp->DesctoMp;
							Dump::promosmp->CalculoDescto(mp->PagoExt->mp->nro, System::Convert::ToDouble(dec));
							mp->DesctoMp = Dump::promosmp->GetMontoDescSinConfirmar(mp->PagoExt->mp->nro);
							_tot = miTotal - mp->DesctoMp;						
						}
					}
					else
						_tot = miTotal;
					ActuStatus();
				}
				
				Decimal dTotal = DynamicTotal();
				Decimal ingre = CalcIngresos();
				double saldo = Decimal::ToDouble(ingre - dTotal); // Decimal::ToDouble(dec - tot) - TotalTarjetasOnline(true);
				double maxVuelto = mp->PagoExt->mp->maxvlto;
				mp->PagoExt->variacion = 0;
				if(mp->PagoExt->mp->descto != 0)
				{
					double v = 0;
					double baseCalc = 0;
					if(maxVuelto < 0)	
						baseCalc = Decimal::ToDouble(dec);
					else
					{
						double vDec = Decimal::ToDouble(dec);
						if(vDec > saldo)
						    baseCalc = vDec - saldo;
						else
							baseCalc = 0;
					}

					//v = baseCalc * ((double)mp->PagoExt->mp->descto / 100);
					v = (double) System::Decimal::Round((dec * (1 - (100 / (100 + Decimal(mp->PagoExt->mp->descto))))), 2);
					/*v = mp->PagoExt->mp->descto > 0 ? (double) (dec * (1 - (100 / (100 + Decimal(mp->PagoExt->mp->descto)))))
						: (double) System::Decimal::Round(dec * (1 - Decimal(mp->PagoExt->mp->descto / 100)), 2);*/
					mp->PagoExt->variacion = dec == 0 ? 0 : v;
				}

				dTotal = DynamicTotal();
				ingre = CalcIngresos();
				saldo = Decimal::ToDouble(ingre - dTotal); // Decimal::ToDouble(dec - tot) - TotalTarjetasOnline(true);
				if(fabs(maxVuelto) < 0.001) maxVuelto = 0.01;
				if(maxVuelto >= 1 && saldo > maxVuelto && dec > 0)
				{
					Alert("Vuelto excede maximo permitido", mp->PagoExt->mp->des);
					dec = 0;
					_tot += mp->DesctoMp;
					mp->PagoExt->variacion = 0;
					MainForm::F->SetMultiDisplay("Total");
					MainForm::F->SetNumDisplay(xTotal.VerPendiente().ToString(2));
				}
				else if(maxVuelto >= 0 && saldo > maxVuelto && dec > 0)
				{
					Alert("No puede dar vuelto de", mp->PagoExt->mp->des);
					dec = 0;					
					_tot += mp->DesctoMp;
					mp->PagoExt->variacion = 0;
					MainForm::F->SetMultiDisplay("Total");
					MainForm::F->SetNumDisplay(xTotal.VerPendiente().ToString(2));
				}
				else if (strcmp(mp->PagoExt->mp->grptot, "13-MUTUALCOM    ") == 0 && Dump::actCliente != nullptr)
				{
					if (dec > (Dump::actCliente->SaldoMutual + saldoMutComodin))
					{
						Alert("Mutual Comodin ya usado verificar", "O excede saldo");
						dec = 0;					
						_tot += mp->DesctoMp;
						mp->PagoExt->variacion = 0;
						Dump::actCliente->SaldoMutual += saldoMutComodin;
						Dump::actCliente->UsadoMutual = false;
						saldoMutComodin = 0;
						MainForm::F->SetMultiDisplay("Total");
						MainForm::F->SetNumDisplay(xTotal.VerPendiente().ToString(2));
					}
					if (dec > 0 && saldoMutComodin == 0){
						saldoMutComodin = dec;
						Dump::actCliente->SaldoMutual -= dec;
						Dump::actCliente->UsadoMutual = true;
					}
				}
				mp->Monto = dec;				
				lossFocus->Text = dec.ToString();
				lossFocus = nullptr;
				ActuStatus();
			}

			inLostFocus = false;
		}

		Decimal GetPendiente(){
			Decimal ingre = CalcIngresos();

			Decimal xtot = DynamicTotal() + 
				(tjPrisma ? Decimal(TotalTarjetasPrisma(true)) : 0) + (tjSmartPoint ? Decimal(TotalTarjetasSmartPointMp(true)) : 0) +
				(EcrPrisma ? Decimal(TotalTarjetasPrismaECR(true)) : 0);
			Decimal devolver = xtot - ingre;
			if (devolver <= 0)
				devolver = 0;
			return devolver;
		}

		void TbGotFocus(Object ^sender, EventArgs ^e)
		{
			if(inGotFocus) return;
			inGotFocus = true;

			Control ^ctl = dynamic_cast<Control ^>(sender);
			if(ctl != nullptr)
			{
				MP ^mp = (MP ^)ctl->Tag;
				ctl->Text = mp->Monto.ToString("F2");
				mp->PagoExt->variacion = 0;				
				//if (mp->DesctoMp > 0 && mp->Monto == 0)
				if (mp->DesctoMp > 0)
				{
					if (mp->Monto == 0)
					{
						//_tot = _tot - mp->DesctoMp;
						Decimal xpendiente = GetPendiente();
						if (xpendiente > 0){
							Dump::promosmp->CalculoDescto(mp->PagoExt->mp->nro, System::Convert::ToDouble(xpendiente));
							mp->DesctoMp = Dump::promosmp->GetMontoDescSinConfirmar(mp->PagoExt->mp->nro);
						}
						_tot = _tot - mp->DesctoMp;
					}
					else if (mp->Monto < (miTotal - mp->DesctoMp))
					{
						Dump::promosmp->CalculoDescto(mp->PagoExt->mp->nro, System::Convert::ToDouble(mp->Monto));
						mp->DesctoMp = Dump::promosmp->GetMontoDescSinConfirmar(mp->PagoExt->mp->nro);
						_tot = miTotal - Dump::promosmp->GetMontoDescSinConfirmar(mp->PagoExt->mp->nro);
					}
					ActuStatus();
				}
			}

			

			// Actualiza el estado dinamico
			ActuDynStatus();

			// Codigo que depende si hay o no touch
			if(!usaTouch)
			{
				// Simula el click que hacen las touchscreen
				TextBox ^tbNew = dynamic_cast<TextBox ^>(ctl);
				if(tbNew != nullptr)
					TbClick(tbNew, nullptr);
			}
			else
				BaseDialog::SetFocusTo(ctl);

			tb = dynamic_cast<Control ^>(ctl);
			if(tb != nullptr)
				((MP ^)tb->Tag)->Borrar = true;

			inGotFocus = false;
		}

		void virtual btOk_Click(Object ^sender, EventArgs ^e) override
		{
			// El teclado puede estar llamando a este metodo sin que necesariamente el control que
			// se esta editando pierda el foco. Esto hace que no se realicen validaciones por lo
			// que forzamos el cambio de foco hacia el boton de OK.
			try
			{			
				btOk->Focus();

				// Valida que la pantalla este lista para ser aprobada.
				if(summary->Visible && lbSaldo->ForeColor != Color::Green) 
				{
					faltaCount = 9;
					lbFalta->Visible = true;
					timerFaltante->Enabled = true;
					return;
				}

				bool todos = true;
				//Seccion de codigo que calcula el monto total de la compra tiene en cuenta
				//los recargos financieros
				if (root)
				{				
					double totalconrecargo = System::Convert::ToDouble(_tot);
					for each(MP ^mp in pagos)
					{
						PagoExtendido *lmpex;
						if (mp->PagoExt->subItems > 0)
						{
							for(int i = 0; i < mp->PagoExt->subItems; i++)
							{
								lmpex = mp->PagoExt->items[i];
								totalconrecargo += lmpex->variacion;
							}						
						}
						else
							totalconrecargo += mp->PagoExt->variacion;
					}

					for each(CPosnet ^llapos in GCStatic::tjPosnet)
					{
						totalconrecargo += llapos->m_recargo;
					}

					if (totalconrecargo > limTicket)
					{
						Alert("Total de la compra con Recargos Financieros", "Excede Limite Fiscal");
						return;
					}

					if (GCStatic::tjPosnet->Count > 0)
					{	
						for each(CPosnet ^llapos in GCStatic::tjPosnet)
						{
							PagoExtendido *lmpex;
							bool cencontro = false;
							for each(MP ^mp in pagos)
							{
								if (mp->PagoExt->subItems > 0)
								{
									for(int i = 0; i < mp->PagoExt->subItems; i++)
									{
										lmpex = mp->PagoExt->items[i];
										if (llapos->m_mpago == lmpex->mp->nro)
										{
											cencontro = true;
											break;
										}
									}
									if (cencontro)
										break;
								}
							}
							if (!cencontro)
							{
								todos = false;
								continue;					
							}
							frmCupones^ subform = gcnew frmCupones(llapos, llapos->m_nomtarjeta);
							subform->ShowDialog();
							if (subform->DialogResult == System::Windows::Forms::DialogResult::Cancel)
								todos = false;
							else
							{
								lmpex->variacion = llapos->m_recargo;
								lmpex->monto += llapos->m_monto + llapos->m_recargo;
							}
							delete subform;
						}
					}
					if (!todos)
						return;
				}				
				BaseDialog::btOk_Click(sender, e);			
				procesarClicks = false;

			}						
			catch (System::Exception^ excep)
			{
				System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");		 
				System::String^ fhdatos = System::String::Format("{0} - Error capturado en btok_click pagoform.h", System::DateTime::Now);
				writer->WriteLine(fhdatos);
				writer->WriteLine("Mensaje: " + excep->Message);
				writer->WriteLine("StackTrace: " + excep->StackTrace);		 
				writer->WriteLine("----------------------------------");
				writer->Close();
				this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			}
			
		}

		void virtual bEnter_Click(Object ^sender, EventArgs ^e) override
		{
			if(tb != nullptr && tb->Text->IndexOf('.') < 0)
			{
				MP ^mp = (MP ^) tb->Tag;
				if(mp->Borrar)
				{
					mp->Borrar = false;
					tb->Text = "0.";
				}
				else
					tb->Text = tb->Text->Trim() + ".";
				}
				procesarClicks = false;
		}

		void virtual bBorra_Click(Object ^sender, EventArgs ^e) override
		{
			if(tb != nullptr)
			{
				MP ^mp = (MP ^) tb->Tag;
				if(mp->Borrar)
				{
					mp->Borrar = false;
					tb->Text = String::Empty;
				}

				BaseDialog::bBorra_Click(sender, e);
				procesarClicks = false;
			}
		}

		void virtual Boton_Click(Object ^sender, EventArgs ^e) override
		{
			if(tb != nullptr)
			{
				MP ^mp = (MP ^) tb->Tag;
				if(mp->Borrar)
				{
					mp->Borrar = false;
					tb->Text = String::Empty;
				}

				BaseDialog::Boton_Click(sender, e);
				procesarClicks = false;

				// En las touch, NUNCA se produce el lostfocus
				if(usaTouch)
				{
					if (mp->DesctoMp > 0 && mp->Monto == 0)
					{
						_tot = _tot - mp->DesctoMp;
						ActuStatus();
					}
					TbLostFocus(tb, nullptr);
				}
			}
		}

		Decimal CalcIngresos()
		{
			Decimal ingre = 0;
			montoEfectivo = 0;
			for each (MP ^mp in pagos)
			{								
				Decimal itemValue;
				if(mp->Handler->Text->Length == 0)
					itemValue = 0;
				else
				{
					try { itemValue = Decimal::Parse(mp->Handler->Text); }
					catch(Exception ^) { itemValue = Decimal(0); }
					if (mp->PagoExt->mp->nro == 1)//si es efectivo
						montoEfectivo = itemValue;
				}
				ingre = ingre + itemValue;				
			}
			//Resta el Ingreso de SOS Movil
			Decimal ingsm = 0;
			/*if (Dump::ListaVueltos->Count > 0 && Dump::ListaVueltos[0]->TipoOperacion == 'C')
				ingsm = System::Convert::ToDecimal(Dump::ListaVueltos[0]->ImpOperacion);
			ingre -= ingsm;*/
			////antes de agregar SMARTPOINT como medio de pago se consideraba comoun form para cargar tarjetas
			//ingre += System::Convert::ToDecimal(TotalTarjetasPrisma(false)) + System::Convert::ToDecimal(TotalTarjetasSmartPointMp(false));
			//ingre += System::Convert::ToDecimal(TotalTarjetasPrisma(false));
			ingre += TotalTarjetasEnVerifones(false);
			return ingre;
		}	

		void ActuStatus()
		{
			Decimal ingre = CalcIngresos();

			/*if(tjOnline && tarjOnline != nullptr)
				ingre += Decimal::Parse(tarjOnline->Text);*/

			Decimal xtot = DynamicTotal() + (tjPrisma ? Decimal(TotalTarjetasPrisma(true)) : 0) + 
				(tjSmartPoint ? Decimal(TotalTarjetasSmartPointMp(true)) : 0) + (EcrPrisma ? Decimal(TotalTarjetasPrismaECR(true)) : 0);
			if (desctoTipre > 0)
			{
				lblDesctoTipre->Visible = true;
				lblDesctoTipre->Text = "Descto: $ " + Convert::ToDecimal(desctoTipre).ToString("F2");
			}
			else
				lblDesctoTipre->Visible = false;
			//Decimal xtot = DynamicTotal() + Decimal(TotalTarjetasTipre(true));
			Decimal recaTot = xtot - ingre;

			// Actualiza las otras representaciones del total
			lbTotal->Text = "Total $$: " + xtot.ToString("F2")->Trim();
			lbIngre->Text = "Recibido: " + ingre.ToString("F2")->Trim();
			if(ingre >= (xtot))
			{				
				lbSaldo->Text = "Vuelto...: " + (ingre - (xtot)).ToString("F2")->Trim();
				lbSaldo->ForeColor = Color::Green;
			}
			else
			{
				lbSaldo->Text = "Faltante: " + (xtot - ingre).ToString("F2")->Trim();
				lbSaldo->ForeColor = Color::Red;			
				
			}
		}

		void ActuDynStatus()
		{
			// Calcula los totales			
			inActuDynStatus = true;
			Decimal ingre = CalcIngresos();
			if(tjOnline && tarjOnline != nullptr)
				ingre += Decimal::Parse(tarjOnline->Text);
			
			if((tjPrisma || EcrPrisma) && tarjPrisma != nullptr)
				ingre += Decimal::Parse(tarjPrisma->Text);
			
			//Decimal xtot = DynamicTotal() + (AutorPinPad ? (Decimal(TotalTarjetasPinPad(true)) - Convert::ToDecimal(desctoTipre)) : (Decimal(TotalTarjetasTipre(true)) - Convert::ToDecimal(desctoTipre)));
			Decimal xtot = DynamicTotal();
			if (desctoTipre > 0)
			{
				lblDesctoTipre->Visible = true;
				lblDesctoTipre->Text = "Descto: " + Convert::ToDecimal(desctoTipre).ToString("F2");
			}
			else
				lblDesctoTipre->Visible = false;
			//Decimal xtot = DynamicTotal() + Decimal(TotalTarjetasTipre(true));
			Decimal recaTot = xtot - ingre;

			// Actualiza los totales dinamicos
			for(int idyn = 0; idyn < pagos->Count; idyn++)
			{
				MP ^mp = pagos[idyn];
				double tasa = mp->PagoExt->mp->descto;
				if(tasa != 0 && recaTot >= 0)
				{
					int index = mp->PagoExt->dynTotalIndex;
					if(index < dynTotal->Count)
					{
						Decimal dyn = recaTot + recaTot * Decimal((double)tasa / 100);
						dynTotal[index]->Text = "(" + dyn.ToString("F2") + ")";
					}
				}
			}			
		}

		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~PagoForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private:
		System::Windows::Forms::TableLayoutPanel^  tbl;

	System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Variable del diseñador requerida.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido del método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PagoForm::typeid));
			this->tbl = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->lblDesctoTipre = (gcnew System::Windows::Forms::Label());
			this->lbTotal = (gcnew System::Windows::Forms::Label());
			this->lbIngre = (gcnew System::Windows::Forms::Label());
			this->lbSaldo = (gcnew System::Windows::Forms::Label());
			this->summary = (gcnew System::Windows::Forms::Panel());
			this->lbFalta = (gcnew System::Windows::Forms::Label());
			this->timerFaltante = (gcnew System::Windows::Forms::Timer(this->components));
			this->lblPiibb = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->tbl->SuspendLayout();
			this->summary->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->lblPiibb);
			this->panel->Controls->Add(this->lbFalta);
			this->panel->Controls->Add(this->summary);
			this->panel->Controls->Add(this->tbl);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &PagoForm::panel_Paint);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->tbl, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->summary, 0);
			this->panel->Controls->SetChildIndex(this->lbFalta, 0);
			this->panel->Controls->SetChildIndex(this->lblPiibb, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(471, 350);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(471, 296);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(527, 234);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(471, 234);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(527, 182);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(471, 182);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(527, 130);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(471, 130);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(527, 78);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(471, 78);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(750, 4);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(310, 23);
			this->Titulo->Text = L"Ingreso del Pago de la Compra.";
			// 
			// bEnter
			// 
			this->bEnter->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bEnter.Image")));
			this->bEnter->Location = System::Drawing::Point(583, 234);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(583, 182);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(583, 130);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(583, 78);
			// 
			// tbl
			// 
			this->tbl->ColumnCount = 3;
			this->tbl->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tbl->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 25)));
			this->tbl->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 25)));
			this->tbl->Controls->Add(this->lblDesctoTipre, 1, 1);
			this->tbl->Location = System::Drawing::Point(19, 67);
			this->tbl->Name = L"tbl";
			this->tbl->RowCount = 12;
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->Size = System::Drawing::Size(386, 486);
			this->tbl->TabIndex = 37;
			// 
			// lblDesctoTipre
			// 
			this->lblDesctoTipre->AutoSize = true;
			this->lblDesctoTipre->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblDesctoTipre->ForeColor = System::Drawing::Color::Black;
			this->lblDesctoTipre->Location = System::Drawing::Point(196, 40);
			this->lblDesctoTipre->Name = L"lblDesctoTipre";
			this->lblDesctoTipre->Size = System::Drawing::Size(46, 16);
			this->lblDesctoTipre->TabIndex = 25;
			this->lblDesctoTipre->Text = L"label1";
			this->lblDesctoTipre->Visible = false;
			// 
			// lbTotal
			// 
			this->lbTotal->AutoSize = true;
			this->lbTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbTotal->ForeColor = System::Drawing::Color::Navy;
			this->lbTotal->Location = System::Drawing::Point(8, 9);
			this->lbTotal->Name = L"lbTotal";
			this->lbTotal->Size = System::Drawing::Size(59, 19);
			this->lbTotal->TabIndex = 38;
			this->lbTotal->Text = L"label1";
			// 
			// lbIngre
			// 
			this->lbIngre->AutoSize = true;
			this->lbIngre->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbIngre->ForeColor = System::Drawing::Color::Navy;
			this->lbIngre->Location = System::Drawing::Point(8, 39);
			this->lbIngre->Name = L"lbIngre";
			this->lbIngre->Size = System::Drawing::Size(59, 19);
			this->lbIngre->TabIndex = 39;
			this->lbIngre->Text = L"label2";
			// 
			// lbSaldo
			// 
			this->lbSaldo->AutoSize = true;
			this->lbSaldo->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbSaldo->ForeColor = System::Drawing::Color::Red;
			this->lbSaldo->Location = System::Drawing::Point(8, 69);
			this->lbSaldo->Name = L"lbSaldo";
			this->lbSaldo->Size = System::Drawing::Size(59, 19);
			this->lbSaldo->TabIndex = 40;
			this->lbSaldo->Text = L"label3";
			// 
			// summary
			// 
			this->summary->BackColor = System::Drawing::Color::PaleTurquoise;
			this->summary->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->summary->Controls->Add(this->lbTotal);
			this->summary->Controls->Add(this->lbSaldo);
			this->summary->Controls->Add(this->lbIngre);
			this->summary->Location = System::Drawing::Point(405, 404);
			this->summary->Name = L"summary";
			this->summary->Size = System::Drawing::Size(228, 103);
			this->summary->TabIndex = 41;
			// 
			// lbFalta
			// 
			this->lbFalta->AutoSize = true;
			this->lbFalta->Font = (gcnew System::Drawing::Font(L"Tahoma", 14, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFalta->ForeColor = System::Drawing::Color::Red;
			this->lbFalta->Location = System::Drawing::Point(462, 16);
			this->lbFalta->Name = L"lbFalta";
			this->lbFalta->Size = System::Drawing::Size(171, 23);
			this->lbFalta->TabIndex = 42;
			this->lbFalta->Text = L"¡¡ Hay Faltante !!";
			this->lbFalta->Visible = false;
			// 
			// timerFaltante
			// 
			this->timerFaltante->Interval = 500;
			this->timerFaltante->Tick += gcnew System::EventHandler(this, &PagoForm::timerFaltante_Tick);
			// 
			// lblPiibb
			// 
			this->lblPiibb->BackColor = System::Drawing::Color::PaleTurquoise;
			this->lblPiibb->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lblPiibb->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblPiibb->ForeColor = System::Drawing::Color::Navy;
			this->lblPiibb->Location = System::Drawing::Point(405, 517);
			this->lblPiibb->Name = L"lblPiibb";
			this->lblPiibb->Size = System::Drawing::Size(228, 21);
			this->lblPiibb->TabIndex = 43;
			this->lblPiibb->Text = L"label1";
			this->lblPiibb->Visible = false;
			// 
			// PagoForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Location = System::Drawing::Point(60, 5);
			this->Name = L"PagoForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PagoForm";
			this->TopMost = false;
			this->Activated += gcnew System::EventHandler(this, &PagoForm::PagoForm_Activated);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &PagoForm::PagoForm_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->tbl->ResumeLayout(false);
			this->tbl->PerformLayout();
			this->summary->ResumeLayout(false);
			this->summary->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

private: System::Void PagoForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
		 }

private: System::Void PagoForm_Activated(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(!saltearActivacion)
				this->SetFocusTo(TabControls[0]);
			 saltearActivacion = true;
		 }

private: System::Void timerFaltante_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(faltaCount == 0)
			 {
				timerFaltante->Enabled = false;
				lbFalta->Visible = false;
			 }
			 else
			 {
				 faltaCount--;
				 lbFalta->Visible = ((faltaCount % 2) == 0);
			 }
		 }
private: System::Void PagoForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			 Strings::releaseKeyboard(this);
		 }

protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
        {
            
			for(int k = 0; k < tbl->Controls->Count; k++)
			{
				Control ^locontrol = tbl->Controls[k];					
				if (locontrol->GetType() != TextBox::typeid)
					continue;					
				locontrol->Text = "0";					
			}
			codcolsel = 0;
			if (janisEcom && Dump::actCliente != nullptr && Dump.actCliente->Repa == 9){
				Dump::actCliente->UsadoMutual = oriUsatoMut;
				Dump::actCliente->SaldoMutual = saldoOriMutComodin;
			}
			BaseDialog::Cancel_Click(sender, e);
        }
private: System::Void panel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		 }
};
}
#endif
