#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;
using namespace Alberdi::TJOCommon::ComunicaMercadoPago;

#include "StaticDump.h"

namespace pos {

	/// <summary>
	/// Summary for frmMpCashout
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmMpCashout : public Controles::BaseDialog
	{
	public:
		frmMpCashout(Decimal ptotal)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			this->TabControls->Add(txtMonto);			
			this->TabControls->Add(txtExtracash);			

			Strings::InterceptKeyboard(this, 1);

			this->recibido = 0;
			this->faltante = ptotal;
			this->total = ptotal;

			this->ExtendedKeyboardEnabled = usaTouch;						
			this->txtMonto->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmMpCashout::txtMonto_KeyPress);			
			this->txtExtracash->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmMpCashout::txtMonto_KeyPress);	
			CantTotalTarjetas = System::Convert::ToUInt16(System::Configuration::ConfigurationManager::AppSettings["tiprecanttarj"]);
			actOrden = gcnew OrdenCashout();

			lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");
			NuevoCupon();
			ClearMsg();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmMpCashout()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtExtracash;
	protected:

		System::Decimal total;
		System::Decimal recibido;
		System::Decimal faltante;

		int CantTotalTarjetas;
		bool inGotFocus;
		OrdenCashout ^actOrden;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  lblMonto;
	private: System::Windows::Forms::TextBox^  txtMonto;
	private: System::Windows::Forms::ListView^  lvw;
	private: System::Windows::Forms::ColumnHeader^  clh1;
	private: System::Windows::Forms::ColumnHeader^  clh2;
	private: System::Windows::Forms::ColumnHeader^  clh3;
	private: System::Windows::Forms::ColumnHeader^  clh4;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btnAdd;
	private: System::Windows::Forms::Button^  btnDot;
	private: System::Windows::Forms::Label^  lblFaltante;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->txtExtracash = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->lblMonto = (gcnew System::Windows::Forms::Label());
			this->txtMonto = (gcnew System::Windows::Forms::TextBox());
			this->lvw = (gcnew System::Windows::Forms::ListView());
			this->clh1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh3 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh4 = (gcnew System::Windows::Forms::ColumnHeader());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnAdd = (gcnew System::Windows::Forms::Button());
			this->btnDot = (gcnew System::Windows::Forms::Button());
			this->lblFaltante = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(94)), 
				static_cast<System::Int32>(static_cast<System::Byte>(136)));
			this->panel->Controls->Add(this->btnDot);
			this->panel->Controls->Add(this->lblFaltante);
			this->panel->Controls->Add(this->txtExtracash);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->lblMonto);
			this->panel->Controls->Add(this->txtMonto);
			this->panel->Controls->Add(this->lvw);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->btnAdd);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->btnAdd, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->lvw, 0);
			this->panel->Controls->SetChildIndex(this->txtMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblMonto, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->txtExtracash, 0);
			this->panel->Controls->SetChildIndex(this->lblFaltante, 0);
			this->panel->Controls->SetChildIndex(this->btnDot, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 473);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 419);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(250, 23);
			this->Titulo->Text = L"Mercado Pago + Cashout";
			// 
			// txtExtracash
			// 
			this->txtExtracash->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtExtracash->Location = System::Drawing::Point(160, 219);
			this->txtExtracash->MaxLength = 10;
			this->txtExtracash->Name = L"txtExtracash";
			this->txtExtracash->Size = System::Drawing::Size(125, 26);
			this->txtExtracash->TabIndex = 86;
			this->txtExtracash->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label3
			// 
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(23, 221);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(125, 19);
			this->label3->TabIndex = 85;
			this->label3->Text = L"Extracash $$:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblMonto
			// 
			this->lblMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMonto->ForeColor = System::Drawing::Color::Black;
			this->lblMonto->Location = System::Drawing::Point(30, 185);
			this->lblMonto->Name = L"lblMonto";
			this->lblMonto->Size = System::Drawing::Size(125, 19);
			this->lblMonto->TabIndex = 74;
			this->lblMonto->Text = L"Monto $$:";
			this->lblMonto->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtMonto
			// 
			this->txtMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtMonto->Location = System::Drawing::Point(161, 183);
			this->txtMonto->MaxLength = 10;
			this->txtMonto->Name = L"txtMonto";
			this->txtMonto->Size = System::Drawing::Size(191, 26);
			this->txtMonto->TabIndex = 76;
			this->txtMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// lvw
			// 
			this->lvw->Activation = System::Windows::Forms::ItemActivation::OneClick;
			this->lvw->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(4) {this->clh1, this->clh2, this->clh3, 
				this->clh4});
			this->lvw->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lvw->FullRowSelect = true;
			this->lvw->HideSelection = false;
			this->lvw->HoverSelection = true;
			this->lvw->Location = System::Drawing::Point(21, 297);
			this->lvw->MultiSelect = false;
			this->lvw->Name = L"lvw";
			this->lvw->Size = System::Drawing::Size(411, 109);
			this->lvw->TabIndex = 79;
			this->lvw->UseCompatibleStateImageBehavior = false;
			this->lvw->View = System::Windows::Forms::View::Details;
			// 
			// clh1
			// 
			this->clh1->Text = L"Id Payment";
			this->clh1->Width = 180;
			// 
			// clh2
			// 
			this->clh2->Text = L"Importe";
			this->clh2->Width = 140;
			// 
			// clh3
			// 
			this->clh3->Text = L"Extracash";
			this->clh3->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->clh3->Width = 80;
			// 
			// clh4
			// 
			this->clh4->Text = L"Importe";
			this->clh4->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->clh4->Width = 0;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(17, 67);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(413, 67);
			this->label1->TabIndex = 80;
			this->label1->Text = L"Seleccionar la tarjeta y verificar las cuotas e importe de compra.\r\nPulse la tecl" 
				L"a \"+\" para Autorizar el cupon deTarjeta y seguir las instrucciones en pantalla.";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// btnAdd
			// 
			this->btnAdd->Font = (gcnew System::Drawing::Font(L"Tahoma", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAdd->Location = System::Drawing::Point(373, 180);
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size(50, 48);
			this->btnAdd->TabIndex = 78;
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Click += gcnew System::EventHandler(this, &frmMpCashout::btnAdd_Click);
			// 
			// btnDot
			// 
			this->btnDot->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btnDot->Font = (gcnew System::Drawing::Font(L"Tahoma", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnDot->Location = System::Drawing::Point(578, 288);
			this->btnDot->Name = L"btnDot";
			this->btnDot->Size = System::Drawing::Size(50, 48);
			this->btnDot->TabIndex = 88;
			this->btnDot->Text = L".";
			this->btnDot->UseVisualStyleBackColor = false;
			this->btnDot->Click += gcnew System::EventHandler(this, &frmMpCashout::btnDot_Click);
			// 
			// lblFaltante
			// 
			this->lblFaltante->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lblFaltante->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lblFaltante->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblFaltante->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->lblFaltante->Location = System::Drawing::Point(466, 346);
			this->lblFaltante->Name = L"lblFaltante";
			this->lblFaltante->Size = System::Drawing::Size(162, 64);
			this->lblFaltante->TabIndex = 87;
			this->lblFaltante->Text = L"Faltan $: 0.00";
			this->lblFaltante->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// frmMpCashout
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Name = L"frmMpCashout";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmMpCashout";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e) {
				 if (Dump::listCashout->Count <= CantTotalTarjetas){
					 actOrden->Caja = caja;
					 actOrden->Cajero = ckaNro;
					 actOrden->Tienda = gcnew String(saptienda);
					 actOrden->NroTicket = pf->GetTickNro(GlobalTF);//String::Format("{0:000000000000}", pf->GetTickNro(GlobalTF));;
					 actOrden->PtoVta = atoi(pf->ptovta);					 

					 Decimal lpMonto = 0;
					 try { 
						 lpMonto = Decimal::Parse(txtMonto->Text); }
					 catch(...) { 
						 lpMonto = 0; }	

					 actOrden->Monto = lpMonto;

					 lpMonto = 0;
					 try { 
						 lpMonto = Decimal::Parse(txtExtracash->Text); }
					 catch(...) { 
						 lpMonto = 0; }					 
					 actOrden->Cashout = lpMonto;

					 RtaOrdenCaja^ rta = nullptr;
					 MsgCashoutMp^ fcashout = gcnew MsgCashoutMp(actOrden);
					 fcashout->ShowMsgProcesaVenta("Autorizando QR MercadoPago", "Aguarde un instante");				 
					 
					 try
					 {
						 rta = fcashout->RespuestaVenta;
						 if (!fcashout->ConError){
							 //action_required
							 if (rta->Estado == 6){

							 }
							 if (rta->Estado == 3)
							 {
								 actOrden->ExternalReference = rta->ExternalReference;
								 actOrden->IdOrdenMercadoPago = rta->IdOrdenMercadoPago;
								 actOrden->IdPayment = rta->IdOperacion;
								 actOrden->Reference = rta->Reference;
								 
								 this->faltante = faltante - actOrden->Monto;
								 
								 Dump::listCashout->Add(actOrden);								  
								 actOrden = gcnew OrdenCashout();
								 lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");
								 LlenarLvw();
								 NuevoCupon();		
							 }
							 else
							 {
								 if (rta->Estado == 5)
									 MessageBox::Show("Orden CANCELADA con tecla Escape.");
								 else
									 MessageBox::Show("Orden NO Autorizada");
							 }
						 }
						 else
							 MessageBox::Show(fcashout->MensajeError);
					 }catch(System::Exception ^exc){
						 MessageBox::Show("Ocurrio el error: " + exc->Message);
					 }
					 ClearMsg();					 
				 }
				 else
				 {
					 Alert("Aviso - El sistema no permite ingresar mas", "Tarjetas para autorizar");		
				 }
			 }

			 ///Llena el listView con los datos de los cupones
	 private: System::Void LlenarLvw(System::Void)
			  {
				  lvw->BeginUpdate();
				  lvw->Items->Clear();				    				

				  for each (OrdenCashout ^tagposnet in Dump::listCashout)
				  {						
					  ListViewItem ^litem = gcnew ListViewItem();
					  litem->Text = tagposnet->Reference;
					  litem->SubItems->Add((tagposnet->Monto).ToString("F2"));				
					  litem->SubItems->Add((tagposnet->Cashout).ToString("F2"));
					  litem->SubItems->Add("");

					  litem->Tag = tagposnet;						
					  lvw->Items->Add(litem);					
					  this->recibido = (Decimal) this->recibido + (Decimal) tagposnet->Monto;						
				  }					

				  lvw->EndUpdate();
			  }

	public: virtual bool ProcessHotKey(Keys key) override
			{
				if (key == Keys::Add)
				{
					
					btnAdd_Click(this, gcnew System::EventArgs());
					return true;
				}				
				return BaseDialog::ProcessHotKey(key);
			}

			  ///Deja la pantalla lista para que se cargue un nuevo cupon
	private: System::Void NuevoCupon(System::Void)
			 {				 
				 this->txtMonto->Text = this->faltante.ToString("F2");				  
				 this->txtExtracash->Text = "";				 
			 }

	protected: virtual System::Void Boton_Click(System::Object^ sender, System::EventArgs^ e) override
			   {
				   String ^scadena = tb->GetType()->Name;
				   if (tb != nullptr && tb->GetType()->Name == "TextBox")
				   {
					   Button ^btn = (Button ^)sender;
					   String ^k = (String ^) btn->Tag;
					   TextBox ^ltext = (TextBox ^) tb;				
					   if (ltext->Text->Length < ltext->MaxLength)
					   {
						   if (borrar)
						   {
							   ltext->Text = (k == ".") ? "0." : k;
							   borrar = false;
						   }
						   else
						   {
							   if (k != "." || (k == "." && !ltext->Text->Contains(".")))
								   ltext->Text += k;							
						   }
					   }
				   }
			   }

		protected: void virtual bEnter_Click(Object ^sender, EventArgs ^e) override
				   {
					   if(tb != nullptr && tb->Name == "txtMonto" && tb->Text->IndexOf('.') < 0)
					   {				
						   if (tb->Text->Length == 0)
							   tb->Text = "0.";
						   else
							   tb->Text = tb->Text->Trim() + ".";
					   }
					   //procesarClicks = false;
				   }

				   void TbGotFocus(Object ^sender, EventArgs ^e)
				   {
					   if(inGotFocus) return;
					   inGotFocus = true;

					   Control ^ctl = dynamic_cast<Control ^>(sender);

					   BaseDialog::SetFocusTo(ctl);

					   inGotFocus = false;
				   }

	   protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
				  {
					  WriteBootLog("AVISO - Saliendo a pantalla de Cupones MercadoPago Cashout con CANCEL");			
				  }

	  private: System::Void btnDot_Click(System::Object^  sender, System::EventArgs^  e) {
				   if(tb != nullptr && (tb->Name == "txtMonto" || tb->Name == "txtExtraCashBack") && tb->Text->IndexOf('.') < 0)
				   {				
					   if (tb->Text->Length == 0)
						   tb->Text = "0.";
					   else
						   tb->Text = tb->Text->Trim() + ".";
				   }
			   }

};
}
