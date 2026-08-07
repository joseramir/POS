#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;



namespace pos {

	/// <summary>
	/// Summary for frmSeniaMP
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmSeniaMP : Controles::BaseDialog
	{
	public:
		System::String ^SMontoIngresado;
		int NroMedioPago;

		frmSeniaMP(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			//txtNroComp->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmNroNotaCredito::txtSoloNumeros_KeyPress);
			txtMontoMP->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmSeniaMP::txtMonto_KeyPress);
			Strings::InterceptKeyboard(this, 1);
			TabControls->Add(cboMedioPago);
			TabControls->Add(txtMontoMP);
			
			this->ExtendedKeyboardEnabled = usaTouch;
			cboMedioPago->Items->Add("SIN SELECCIONAR");
			cboMedioPago->Items->Add("EFECTIVO");
			cboMedioPago->Items->Add("TRANSFERENCIA");
			cboMedioPago->SelectedIndex = 0;
			indexsel = cboMedioPago->SelectedIndex;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmSeniaMP()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  cboMedioPago;
	private: System::Windows::Forms::TextBox^  txtMontoMP;

			 int indexsel; 



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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->cboMedioPago = (gcnew System::Windows::Forms::ComboBox());
			this->txtMontoMP = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->cboMedioPago);
			this->panel->Controls->Add(this->txtMontoMP);
			this->panel->Size = System::Drawing::Size(610, 438);
			this->panel->Controls->SetChildIndex(this->txtMontoMP, 0);
			this->panel->Controls->SetChildIndex(this->cboMedioPago, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
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
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(433, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(433, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(489, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(433, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(489, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(433, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(489, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(433, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(489, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(433, 77);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(600, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(270, 23);
			this->Titulo->Text = L"Señas de Pagos de Clientes";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(545, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(545, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(545, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(545, 77);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(21, 112);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(121, 18);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Medio de Pago:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(21, 176);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(83, 18);
			this->label2->TabIndex = 40;
			this->label2->Text = L"Monto $$:";
			// 
			// cboMedioPago
			// 
			this->cboMedioPago->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboMedioPago->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboMedioPago->FormattingEnabled = true;
			this->cboMedioPago->Location = System::Drawing::Point(150, 109);
			this->cboMedioPago->Name = L"cboMedioPago";
			this->cboMedioPago->Size = System::Drawing::Size(182, 26);
			this->cboMedioPago->TabIndex = 41;
			this->cboMedioPago->SelectedIndexChanged += gcnew System::EventHandler(this, &frmSeniaMP::cboMedioPago_SelectedIndexChanged);
			this->cboMedioPago->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &frmSeniaMP::cboMedioPago_PreviewKeyDown);
			// 
			// txtMontoMP
			// 
			this->txtMontoMP->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtMontoMP->Location = System::Drawing::Point(150, 169);
			this->txtMontoMP->MaxLength = 15;
			this->txtMontoMP->Name = L"txtMontoMP";
			this->txtMontoMP->Size = System::Drawing::Size(182, 26);
			this->txtMontoMP->TabIndex = 42;
			// 
			// frmSeniaMP
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(610, 438);
			this->Name = L"frmSeniaMP";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmSeniaMP";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
				   {
					   System::Decimal MontoIngresado = 0;
					   if (cboMedioPago->SelectedIndex == 0)
					   {
						   Alert("Seleccionar el medio de pago ", "De la SEÑA");
						   return;
					   }
					   if (cboMedioPago->SelectedIndex == 1)
						   NroMedioPago = 1;
					   else if (cboMedioPago->SelectedIndex == 2)
						   NroMedioPago = 48;
					   System::Decimal::TryParse(txtMontoMP->Text, MontoIngresado);
					   if (MontoIngresado <= 0){
						   Alert("Ingrese un monto de Seña", "");
						   return ;
					   }
					   SMontoIngresado = MontoIngresado.ToString("F2");
					   BaseDialog::btOk_Click(sender, e);
					   WriteBootLog("AVISO - Saliendo a pantalla de Carga Seña con OK");

				   }
	   protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
				  {
					  WriteBootLog("AVISO - Saliendo a pantalla de Carga Seña con CANCEL");			
				  }
	private: System::Void cboMedioPago_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
				 if (e->KeyCode == Keys::Enter)
				 {
					 cboMedioPago->DroppedDown = false;
					 Strings::EnableKey(this);
					 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
					 if (indexsel != cboMedioPago->SelectedIndex)
					 {
						 indexsel = cboMedioPago->SelectedIndex;
						 //txtCuotas_TextChanged(sender, gcnew System::EventArgs());
					 }
				 }

			 }
private: System::Void cboMedioPago_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

public: virtual bool ProcessHotKey(Keys key) override
			 {
				 if (key == Keys::Down)
				 {
					 if (this->tb->Name == "cboMedioPago")
					 {
						 Strings::DisableKey(this);
						 cboMedioPago->DroppedDown = true;
						 return true;						 
					 }	
					 
				 }
				 return BaseDialog::ProcessHotKey(key);
			 }
};
}
