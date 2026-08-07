#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


using namespace Alberdi::TJOCommon::VerifoneCommon;

#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for frmAnulaPrismaECR
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmAnulaPrismaECR : public Controles::BaseDialog
	{
	public:
		frmAnulaPrismaECR(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->txtLote->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrismaECR::txtSoloNumeros_KeyPress);
			this->txtCupon->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrismaECR::txtSoloNumeros_KeyPress);
			//this->txtAutorizacion->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaPrisma::txtSoloNumeros_KeyPress);

			Strings::InterceptKeyboard(this, 1);

			TabControls->Add(this->txtLote);
			TabControls->Add(this->txtCupon);
			TabControls->Add(this->txtAutorizacion);					

			this->ExtendedKeyboardEnabled = usaTouch;						
			lbFaltan->Visible = false;
			actVenta = gcnew TransPrismaECR();
			actVenta->TipoVerifone = 3;

			btOk->Enabled = false;

						
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmAnulaPrismaECR()
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
	private: System::Windows::Forms::Timer^  tmrFinalizar;


	private: System::ComponentModel::IContainer^  components;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		TransPrismaECR^ actVenta;


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
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
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->txtLote);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->txtCupon);
			this->panel->Controls->Add(this->txtAutorizacion);
			this->panel->Controls->Add(this->lbFaltan);
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
			this->cancel->Location = System::Drawing::Point(381, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(381, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(437, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(381, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(437, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(381, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(437, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(381, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(437, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(381, 77);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(550, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(304, 23);
			this->Titulo->Text = L"Anulación Tarjetas Prisma ECR";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(493, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(493, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(493, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(493, 77);
			// 
			// lbFaltan
			// 
			this->lbFaltan->BackColor = System::Drawing::Color::Transparent;
			this->lbFaltan->Font = (gcnew System::Drawing::Font(L"Arial Black", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFaltan->ForeColor = System::Drawing::Color::Crimson;
			this->lbFaltan->Location = System::Drawing::Point(371, 28);
			this->lbFaltan->Name = L"lbFaltan";
			this->lbFaltan->Size = System::Drawing::Size(176, 24);
			this->lbFaltan->TabIndex = 106;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(24, 109);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(38, 13);
			this->label4->TabIndex = 116;
			this->label4->Text = L"LOTE:";
			// 
			// txtLote
			// 
			this->txtLote->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtLote->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtLote->Location = System::Drawing::Point(138, 106);
			this->txtLote->MaxLength = 3;
			this->txtLote->Name = L"txtLote";
			this->txtLote->Size = System::Drawing::Size(140, 23);
			this->txtLote->TabIndex = 117;
			this->txtLote->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrismaECR::txtLote_Validating);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(24, 213);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(91, 13);
			this->label3->TabIndex = 113;
			this->label3->Text = L"AUTORIZACION:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(24, 161);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 13);
			this->label2->TabIndex = 112;
			this->label2->Text = L"CUPON:";
			// 
			// txtCupon
			// 
			this->txtCupon->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtCupon->Location = System::Drawing::Point(138, 158);
			this->txtCupon->MaxLength = 4;
			this->txtCupon->Name = L"txtCupon";
			this->txtCupon->Size = System::Drawing::Size(140, 23);
			this->txtCupon->TabIndex = 114;
			this->txtCupon->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrismaECR::txtCupon_Validating);
			// 
			// txtAutorizacion
			// 
			this->txtAutorizacion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtAutorizacion->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtAutorizacion->Location = System::Drawing::Point(138, 206);
			this->txtAutorizacion->MaxLength = 6;
			this->txtAutorizacion->Name = L"txtAutorizacion";
			this->txtAutorizacion->Size = System::Drawing::Size(140, 23);
			this->txtAutorizacion->TabIndex = 115;
			this->txtAutorizacion->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmAnulaPrismaECR::txtAutorizacion_Validating);
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &frmAnulaPrismaECR::timerFaltan_Tick);
			// 
			// tmrFinalizar
			// 
			this->tmrFinalizar->Interval = 500;
			this->tmrFinalizar->Tick += gcnew System::EventHandler(this, &frmAnulaPrismaECR::tmrFinalizar_Tick);
			// 
			// frmAnulaPrismaECR
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(567, 448);
			this->Name = L"frmAnulaPrismaECR";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmAnulaPrismaECR";
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

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
			Dump::TarjPrismaECR->Add(actVenta);
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
