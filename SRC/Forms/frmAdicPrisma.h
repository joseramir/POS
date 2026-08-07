#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


using namespace Controles;

using namespace Alberdi::TJOCommon::Prisma;


#include "DatosMpagos.h"
#include "Funcs.h"
#include "tjonline.h"
#include "frmCupones.h"
#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for frmAdicPrisma
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmAdicPrisma : public Controles::BaseDialog
	{
	public:
		frmAdicPrisma(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->TabControls->Add(btnTest);
			this->TabControls->Add(btnCierre);
			this->TabControls->Add(btnReimpCierre);
			this->TabControls->Add(btnReimUltCupon);			
			this->TabControls->Add(btnConsUltTrans);
			this->TabControls->Add(btnConsUltCierre);

			//this->TabControls->Add(btnTablaTarj);
			//this->TabControls->Add(btnTablaPlan);
			Strings::InterceptKeyboard(this, 1);	

			hacer = gcnew OperarTrxPrisma(COM_NLD);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmAdicPrisma()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnCierre;
	private: System::Windows::Forms::Button^  btnReimUltCupon;
	protected: 

	private: System::Windows::Forms::Button^  btnReimpCierre;
	private: System::Windows::Forms::Button^  btnTablaTarj;
	private: System::Windows::Forms::Button^  btnTablaPlan;

			 OperarTrxPrisma ^hacer;
	private: System::Windows::Forms::Button^  btnTest;
	private: System::Windows::Forms::Button^  btnConsUltTrans;
	private: System::Windows::Forms::Button^  btnConsUltCierre;





	protected: 

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmAdicPrisma::typeid));
			this->btnCierre = (gcnew System::Windows::Forms::Button());
			this->btnReimpCierre = (gcnew System::Windows::Forms::Button());
			this->btnReimUltCupon = (gcnew System::Windows::Forms::Button());
			this->btnTablaTarj = (gcnew System::Windows::Forms::Button());
			this->btnTablaPlan = (gcnew System::Windows::Forms::Button());
			this->btnTest = (gcnew System::Windows::Forms::Button());
			this->btnConsUltTrans = (gcnew System::Windows::Forms::Button());
			this->btnConsUltCierre = (gcnew System::Windows::Forms::Button());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::LightSalmon;
			this->panel->Controls->Add(this->btnConsUltCierre);
			this->panel->Controls->Add(this->btnConsUltTrans);
			this->panel->Controls->Add(this->btnTest);
			this->panel->Controls->Add(this->btnTablaPlan);
			this->panel->Controls->Add(this->btnTablaTarj);
			this->panel->Controls->Add(this->btnReimUltCupon);
			this->panel->Controls->Add(this->btnReimpCierre);
			this->panel->Controls->Add(this->btnCierre);
			this->panel->Size = System::Drawing::Size(650, 460);
			this->panel->Controls->SetChildIndex(this->btnCierre, 0);
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
			this->panel->Controls->SetChildIndex(this->btnReimpCierre, 0);
			this->panel->Controls->SetChildIndex(this->btnReimUltCupon, 0);
			this->panel->Controls->SetChildIndex(this->btnTablaTarj, 0);
			this->panel->Controls->SetChildIndex(this->btnTablaPlan, 0);
			this->panel->Controls->SetChildIndex(this->btnTest, 0);
			this->panel->Controls->SetChildIndex(this->btnConsUltTrans, 0);
			this->panel->Controls->SetChildIndex(this->btnConsUltCierre, 0);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(400, 23);
			this->Titulo->Text = L"Operaciones Adicionales Verifone Prisma";
			// 
			// btnCierre
			// 
			this->btnCierre->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnCierre->Location = System::Drawing::Point(77, 121);
			this->btnCierre->Name = L"btnCierre";
			this->btnCierre->Size = System::Drawing::Size(162, 38);
			this->btnCierre->TabIndex = 39;
			this->btnCierre->Text = L"Cierre";
			this->btnCierre->UseVisualStyleBackColor = true;
			this->btnCierre->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnCierre_Click);
			// 
			// btnReimpCierre
			// 
			this->btnReimpCierre->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnReimpCierre->Location = System::Drawing::Point(77, 165);
			this->btnReimpCierre->Name = L"btnReimpCierre";
			this->btnReimpCierre->Size = System::Drawing::Size(162, 38);
			this->btnReimpCierre->TabIndex = 40;
			this->btnReimpCierre->Text = L"Reimp Cierre";
			this->btnReimpCierre->UseVisualStyleBackColor = true;
			this->btnReimpCierre->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnReimpCierre_Click);
			// 
			// btnReimUltCupon
			// 
			this->btnReimUltCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnReimUltCupon->Location = System::Drawing::Point(77, 209);
			this->btnReimUltCupon->Name = L"btnReimUltCupon";
			this->btnReimUltCupon->Size = System::Drawing::Size(162, 38);
			this->btnReimUltCupon->TabIndex = 41;
			this->btnReimUltCupon->Text = L"Reimp Ult cupon";
			this->btnReimUltCupon->UseVisualStyleBackColor = true;
			this->btnReimUltCupon->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnReimUltCupon_Click);
			// 
			// btnTablaTarj
			// 
			this->btnTablaTarj->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnTablaTarj->Location = System::Drawing::Point(74, 358);
			this->btnTablaTarj->Name = L"btnTablaTarj";
			this->btnTablaTarj->Size = System::Drawing::Size(162, 38);
			this->btnTablaTarj->TabIndex = 42;
			this->btnTablaTarj->Text = L"Tabla Tarjetas";
			this->btnTablaTarj->UseVisualStyleBackColor = true;
			this->btnTablaTarj->Visible = false;
			this->btnTablaTarj->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnTablaTarj_Click);
			// 
			// btnTablaPlan
			// 
			this->btnTablaPlan->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnTablaPlan->Location = System::Drawing::Point(74, 402);
			this->btnTablaPlan->Name = L"btnTablaPlan";
			this->btnTablaPlan->Size = System::Drawing::Size(162, 38);
			this->btnTablaPlan->TabIndex = 43;
			this->btnTablaPlan->Text = L"Tabla Planes";
			this->btnTablaPlan->UseVisualStyleBackColor = true;
			this->btnTablaPlan->Visible = false;
			this->btnTablaPlan->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnTablaPlan_Click);
			// 
			// btnTest
			// 
			this->btnTest->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnTest->Location = System::Drawing::Point(77, 77);
			this->btnTest->Name = L"btnTest";
			this->btnTest->Size = System::Drawing::Size(162, 38);
			this->btnTest->TabIndex = 44;
			this->btnTest->Text = L"TEST";
			this->btnTest->UseVisualStyleBackColor = true;
			this->btnTest->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnTest_Click);
			// 
			// btnConsUltTrans
			// 
			this->btnConsUltTrans->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnConsUltTrans->Location = System::Drawing::Point(77, 260);
			this->btnConsUltTrans->Name = L"btnConsUltTrans";
			this->btnConsUltTrans->Size = System::Drawing::Size(162, 38);
			this->btnConsUltTrans->TabIndex = 45;
			this->btnConsUltTrans->Text = L"Cons Ult Trans";
			this->btnConsUltTrans->UseVisualStyleBackColor = true;
			this->btnConsUltTrans->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnConsUltTrans_Click);
			// 
			// btnConsUltCierre
			// 
			this->btnConsUltCierre->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnConsUltCierre->Location = System::Drawing::Point(77, 304);
			this->btnConsUltCierre->Name = L"btnConsUltCierre";
			this->btnConsUltCierre->Size = System::Drawing::Size(162, 38);
			this->btnConsUltCierre->TabIndex = 46;
			this->btnConsUltCierre->Text = L"Cons Ult Cierre";
			this->btnConsUltCierre->UseVisualStyleBackColor = true;
			this->btnConsUltCierre->Click += gcnew System::EventHandler(this, &frmAdicPrisma::btnConsUltCierre_Click);
			// 
			// frmAdicPrisma
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(650, 460);
			this->Name = L"frmAdicPrisma";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmAdicPrisma";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnCierre_Click(System::Object^  sender, System::EventArgs^  e) {
				 MsgAlertPrisma ^otroMsg = gcnew MsgAlertPrisma(COM_NLD);
				 otroMsg->ShowMsgHacerCierreLotes("Realizando Operacion por Verifone Prisma", "Aguarde un instante");
				 if (otroMsg->ConError)
					 MessageBox::Show(otroMsg, otroMsg->MensajeError);
				 delete otroMsg;

				 /*hacer->HacerCierre();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }*/
			 }
	private: System::Void btnReimpCierre_Click(System::Object^  sender, System::EventArgs^  e) {
				 hacer->ReimpreUltCierre();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }
			 }
	private: System::Void btnReimUltCupon_Click(System::Object^  sender, System::EventArgs^  e) {
				 hacer->ReimpreUltTrans();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }
			 }
	private: System::Void btnTablaTarj_Click(System::Object^  sender, System::EventArgs^  e) {
				 hacer->ObtenerTarjetas();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }
			 }
	private: System::Void btnTablaPlan_Click(System::Object^  sender, System::EventArgs^  e) {
				 hacer->ObtenerPlanes();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }
			 }
	private: System::Void btnTest_Click(System::Object^  sender, System::EventArgs^  e) {
				 String ^rtaTest = hacer->HacerTest();
				 if (hacer->ConError)
				 {
					 MessageBox::Show(hacer->MensajeError);
				 }
				 else
					 MessageBox::Show(rtaTest);
			 }
	private: System::Void btnConsUltTrans_Click(System::Object^  sender, System::EventArgs^  e) {
				 RtaTrans ^aux = hacer->ObtenerUltimaTrans();
				 if (hacer->ConError)
					 MessageBox::Show(hacer->MensajeError, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				 else
					 MessageBox::Show("Datos de ultima operacion\nTipo: " + aux->TipoTransaccion + "\nLote: " + aux->NumeroLote + "\nCupon: " + aux->NumeroCupon);
			 }

	private: System::Void btnConsUltCierre_Click(System::Object^  sender, System::EventArgs^  e) {
				 
				 hacer->ObtenerUltCierre();
				 if (hacer->ConError)
					 MessageBox::Show(hacer->MensajeError, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				 else
					 MessageBox::Show("Datos de ultima operacion\nTipo: ");/// + aux->TipoTransaccion + "\nLote: " + aux->NumeroLote + "\nCupon: " + aux->NumeroCupon);
			 }
};
}
