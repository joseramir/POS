#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;

#include "StaticDump.h"
#include "DatosMpagos.h"
#include "Funcs.h"
#include "total.h"

namespace pos {

	/// <summary>
	/// Summary for frmCargaEfectivo
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmCargaEfectivo : public Controles::BaseDialog
	{
	public:
		frmCargaEfectivo(Decimal ptotal, Decimal pMontoLimite, Decimal pAlicuota, class PagoExtendido *pex)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			montoLimite = System::Decimal::Round(pMontoLimite, 2);
			alicuotaRecargo = System::Decimal::Round(pAlicuota, 2);
			this->TabControls->Add(txtMonto);
			Strings::InterceptKeyboard(this, 1);
			if (pex->monto == 0){
				this->total = System::Decimal::Round(ptotal, 2);
				this->faltante = System::Decimal::Round(ptotal, 2);				
				MontoIngresado = 0;				
				this->recibido = 0;
			}
			else{			
				
				MontoIngresado = System::Decimal::Round(System::Convert::ToDecimal(pex->monto), 2);
				this->faltante = System::Decimal::Round((ptotal + MontoIngresado), 2);				
			}
			lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");

			MontoRecargo = System::Decimal::Round((this->total > montoLimite ? this->total * (alicuotaRecargo / 100) : 0), 2);

			lblRecargo->Text = System::Convert::ToString(MontoRecargo);
			lblTotal->Text = System::Convert::ToString(this->total + MontoRecargo);

			this->ExtendedKeyboardEnabled = usaTouch;	
			this->txtMonto->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCargaEfectivo::txtMonto_KeyPress);	
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmCargaEfectivo()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lblTotal;
	protected: 
		System::Decimal faltante;
		System::Decimal total;
		System::Decimal recibido;
		System::Decimal montoLimite;
		System::Decimal alicuotaRecargo;
	public:
		System::Decimal MontoIngresado;
		System::Decimal MontoRecargo;

	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  lblRecargo;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lblMonto;
	private: System::Windows::Forms::TextBox^  txtMonto;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmCargaEfectivo::typeid));
			this->lblRecargo = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->lblMonto = (gcnew System::Windows::Forms::Label());
			this->txtMonto = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->lblTotal = (gcnew System::Windows::Forms::Label());
			this->lblFaltante = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::Gray;
			this->panel->Controls->Add(this->lblFaltante);
			this->panel->Controls->Add(this->lblTotal);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->lblRecargo);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->lblMonto);
			this->panel->Controls->Add(this->txtMonto);
			this->panel->Size = System::Drawing::Size(669, 481);
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
			this->panel->Controls->SetChildIndex(this->txtMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblMonto, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->lblRecargo, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->lblTotal, 0);
			this->panel->Controls->SetChildIndex(this->lblFaltante, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(476, 414);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(476, 360);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(532, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(476, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(532, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(476, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(532, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(476, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(532, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(476, 77);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Location = System::Drawing::Point(63, 17);
			this->Titulo->Size = System::Drawing::Size(196, 23);
			this->Titulo->Text = L"Ingreso de Efectivo";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(588, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(588, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(588, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(588, 77);
			// 
			// lblRecargo
			// 
			this->lblRecargo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblRecargo->Location = System::Drawing::Point(148, 157);
			this->lblRecargo->Name = L"lblRecargo";
			this->lblRecargo->Size = System::Drawing::Size(87, 19);
			this->lblRecargo->TabIndex = 62;
			this->lblRecargo->Text = L"0.00";
			this->lblRecargo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(75, 157);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(67, 19);
			this->label2->TabIndex = 61;
			this->label2->Text = L"Recargo $:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblMonto
			// 
			this->lblMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMonto->ForeColor = System::Drawing::Color::Black;
			this->lblMonto->Location = System::Drawing::Point(20, 103);
			this->lblMonto->Name = L"lblMonto";
			this->lblMonto->Size = System::Drawing::Size(125, 19);
			this->lblMonto->TabIndex = 57;
			this->lblMonto->Text = L"Monto $$:";
			this->lblMonto->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtMonto
			// 
			this->txtMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtMonto->Location = System::Drawing::Point(151, 101);
			this->txtMonto->MaxLength = 12;
			this->txtMonto->Name = L"txtMonto";
			this->txtMonto->Size = System::Drawing::Size(191, 26);
			this->txtMonto->TabIndex = 59;
			this->txtMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMonto->TextChanged += gcnew System::EventHandler(this, &frmCargaEfectivo::txtMonto_TextChanged);
			// 
			// label1
			// 
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(45, 206);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(100, 23);
			this->label1->TabIndex = 63;
			this->label1->Text = L"Total $$:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblTotal
			// 
			this->lblTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblTotal->Location = System::Drawing::Point(151, 205);
			this->lblTotal->Name = L"lblTotal";
			this->lblTotal->Size = System::Drawing::Size(176, 23);
			this->lblTotal->TabIndex = 64;
			this->lblTotal->Text = L"0.00";
			this->lblTotal->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->lblFaltante->Location = System::Drawing::Point(477, 288);
			this->lblFaltante->Name = L"lblFaltante";
			this->lblFaltante->Size = System::Drawing::Size(162, 64);
			this->lblFaltante->TabIndex = 65;
			this->lblFaltante->Text = L"Faltan $: 0.00";
			this->lblFaltante->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// frmCargaEfectivo
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(669, 481);
			this->Name = L"frmCargaEfectivo";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmCargaEfectivo";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void txtMonto_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
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
	private: System::Void txtMonto_TextChanged(System::Object^  sender, System::EventArgs^  e) {
				 System::Decimal::TryParse(txtMonto->Text, MontoIngresado);				 
			 }

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
			   {
				   if (MontoIngresado < montoLimite)
					   MontoRecargo = 0;
				   //MontoRecargo = MontoIngresado > montoLimite ? MontoIngresado * (alicuotaRecargo / 100) : 0;
				   MontoRecargo = System::Decimal::Round(MontoRecargo, 2);
				   BaseDialog::btOk_Click(sender, e);
				   WriteBootLog("AVISO - Saliendo a pantalla de Carga Efectivo con OK");

			   }
   protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
			  {
				  this->MontoRecargo = 0;
				  this->MontoIngresado = 0;
				  WriteBootLog("AVISO - Saliendo a pantalla de Carga Efectivo con CANCEL");			
			  }
};
}
