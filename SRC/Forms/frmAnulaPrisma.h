#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


using namespace System::Reflection;

using namespace Alberdi::TJOCommon::Prisma;
using namespace Alberdi::TJOCommon::VerifoneCommon;

#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for frmAnulaPrisma
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmAnulaPrisma : public Controles::BaseDialog
	{
	public:
		frmAnulaPrisma(int pTipoVerifone)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->txtLote->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrisma::txtSoloNumeros_KeyPress);
			this->txtCupon->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrisma::txtSoloNumeros_KeyPress);
			//this->txtAutorizacion->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrisma::txtSoloNumeros_KeyPress);

			Strings::InterceptKeyboard(this, 1);

			TabControls->Add(this->txtLote);
			TabControls->Add(this->txtCupon);
			TabControls->Add(this->txtAutorizacion);					

			this->ExtendedKeyboardEnabled = usaTouch;						
			lbFaltan->Visible = false;
			actVenta = gcnew TransVerifone();
			actVenta->TipoVerifone = pTipoVerifone;

			btOk->Enabled = false;

			//REaliza el test
			OperarTrxPrisma ^hacer = gcnew OperarTrxPrisma(COM_NLD);						 
			if (actVenta->TipoVerifone == 1) {
				ClearMsg();
				SendMsg("Prueba conexion Newland Prisma", "Aguarde un instante");
				String^ rtaTest = hacer->HacerTest();
				ClearMsg();
				if (rtaTest != "002") {
					Alert("Sin conexion con Dispositivo Newland", "Verificar conexion y configuracion.");
					tmrFinalizar->Enabled = true;
				}
			}
			else {
				this->Titulo->Text = "Anulación Tarjetas Verifone POSNET";
				this->panel->BackColor = System::Drawing::Color::DarkSeaGreen;
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmAnulaPrisma()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lbFaltan;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  txtLote;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  txtCupon;
	private: System::Windows::Forms::TextBox^  txtAutorizacion;
	private: System::Windows::Forms::Timer^  timerFaltan;



	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::Timer^  tmrFinalizar;


			 
	protected: 
	

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		TransVerifone^ actVenta;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmAnulaPrisma::typeid));
			this->lbFaltan = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->txtLote = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txtCupon = (gcnew System::Windows::Forms::TextBox());
			this->txtAutorizacion = (gcnew System::Windows::Forms::TextBox());
			this->timerFaltan = (gcnew System::Windows::Forms::Timer(this->components));
			this->tmrFinalizar = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::LightSalmon;
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->txtLote);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->txtCupon);
			this->panel->Controls->Add(this->txtAutorizacion);
			this->panel->Controls->Add(this->lbFaltan);
			this->panel->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->panel->Size = System::Drawing::Size(567, 448);
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
			this->panel->Controls->SetChildIndex(this->lbFaltan, 0);
			this->panel->Controls->SetChildIndex(this->txtAutorizacion, 0);
			this->panel->Controls->SetChildIndex(this->txtCupon, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->txtLote, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(382, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(382, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(438, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(382, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(438, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(382, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(438, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(382, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(438, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(382, 77);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(550, 4);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(364, 23);
			this->Titulo->Text = L"Anulación Tarjetas Verifone Newland";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(494, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(494, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(494, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(494, 77);
			// 
			// lbFaltan
			// 
			this->lbFaltan->BackColor = System::Drawing::Color::Transparent;
			this->lbFaltan->Font = (gcnew System::Drawing::Font(L"Arial Black", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFaltan->ForeColor = System::Drawing::Color::Crimson;
			this->lbFaltan->Location = System::Drawing::Point(381, 28);
			this->lbFaltan->Name = L"lbFaltan";
			this->lbFaltan->Size = System::Drawing::Size(176, 24);
			this->lbFaltan->TabIndex = 105;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(43, 105);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(43, 16);
			this->label4->TabIndex = 110;
			this->label4->Text = L"LOTE:";
			// 
			// txtLote
			// 
			this->txtLote->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtLote->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtLote->Location = System::Drawing::Point(157, 102);
			this->txtLote->MaxLength = 3;
			this->txtLote->Name = L"txtLote";
			this->txtLote->Size = System::Drawing::Size(140, 23);
			this->txtLote->TabIndex = 111;
			this->txtLote->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrisma::txtLote_Validating);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(43, 209);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(108, 16);
			this->label3->TabIndex = 107;
			this->label3->Text = L"AUTORIZACION:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(43, 157);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(54, 16);
			this->label2->TabIndex = 106;
			this->label2->Text = L"CUPON:";
			// 
			// txtCupon
			// 
			this->txtCupon->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtCupon->Location = System::Drawing::Point(157, 154);
			this->txtCupon->MaxLength = 4;
			this->txtCupon->Name = L"txtCupon";
			this->txtCupon->Size = System::Drawing::Size(140, 23);
			this->txtCupon->TabIndex = 108;
			this->txtCupon->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrisma::txtCupon_Validating);
			// 
			// txtAutorizacion
			// 
			this->txtAutorizacion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtAutorizacion->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtAutorizacion->Location = System::Drawing::Point(157, 202);
			this->txtAutorizacion->MaxLength = 6;
			this->txtAutorizacion->Name = L"txtAutorizacion";
			this->txtAutorizacion->Size = System::Drawing::Size(140, 23);
			this->txtAutorizacion->TabIndex = 109;
			this->txtAutorizacion->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrisma::txtAutorizacion_Validating);
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &frmAnulaPrisma::timerFaltan_Tick);
			// 
			// tmrFinalizar
			// 
			this->tmrFinalizar->Interval = 500;
			this->tmrFinalizar->Tick += gcnew System::EventHandler(this, &frmAnulaPrisma::tmrFinalizar_Tick);
			// 
			// frmAnulaPrisma
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(567, 448);
			this->Name = L"frmAnulaPrisma";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmAnulaPrisma";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			if (Char::IsLetter(e->KeyChar))
			{
				e->Handled = true;
			}
			else if (Char::IsDigit(e->KeyChar))
			{
				e->Handled = false;
			}
			else if (Char::IsControl(e->KeyChar))
			{
				e->Handled = false;
			}
			else
			{
				e->Handled = true;
			}
		}

		System::Void txtSoloMonto_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			if (Char::IsLetter(e->KeyChar))
			{
				e->Handled = true;
			}
			else if (Char::IsDigit(e->KeyChar) )
			{
				e->Handled = false;
			}
			else if (e->KeyChar == '.')
			{
				TextBox^ auxtext = dynamic_cast<TextBox ^>(sender);
				e->Handled = auxtext->Text->Contains(".");
			}
			else if (Char::IsControl(e->KeyChar))
			{
				e->Handled = false;
			}
			else
			{
				e->Handled = true;
			}
		}

	protected:
		System::Void virtual Cancel_Click(Object ^sender, EventArgs ^e) override
		{			
			BaseDialog::Cancel_Click(sender, e);			
		}

		System::Void timerFaltan_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			lbFaltan->Visible = !lbFaltan->Visible;
		}

		bool Valida(){
			bool res = false;
			if(!String::IsNullOrEmpty(txtLote->Text)){
				if (!String::IsNullOrEmpty(txtCupon->Text)){
					if (!String::IsNullOrEmpty(txtAutorizacion->Text))
					{
						res = true;
						actVenta->NroLote = System::Convert::ToInt32(txtLote->Text);
						actVenta->NroCupon = System::Convert::ToInt32(txtCupon->Text);
						actVenta->NroAutorizacion = txtAutorizacion->Text;
					}
				}
			}

			timerFaltan->Enabled = !res;

			lbFaltan->Visible = true;
			lbFaltan->Text = (!res? "Faltan Datos...": "Tarjeta Lista...");
			lbFaltan->ForeColor = (res? Color::Red: Color::Beige);
			lbFaltan->BackColor = (res? Color::White: Color::Blue);
			lbFaltan->BorderStyle = (res? System::Windows::Forms::BorderStyle::FixedSingle: System::Windows::Forms::BorderStyle::None);

			btOk->Enabled = res;

			return res;
		}

		virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{			
			Dump::TarjPrisma->Add(actVenta);
			bEnter_Click(sender, e);
			BaseDialog::btOk_Click(sender, e);
		}


	private: System::Void txtLote_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
				 Valida();
			 }
	private: System::Void txtCupon_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
				 Valida();
			 }
	private: System::Void txtAutorizacion_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
				 Valida();
			 }
private: System::Void tmrFinalizar_Tick(System::Object^  sender, System::EventArgs^  e) {
			 tmrFinalizar->Enabled = false;
			 Cancel_Click(sender, e);
			 this->Close();
		 }
};
}
