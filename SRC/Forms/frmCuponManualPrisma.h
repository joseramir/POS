#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Summary for frmCuponManualPrisma
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmCuponManualPrisma : public Controles::BaseDialog
	{
	public:
		int NroLoteIng;
		int NroCuponIng;
		System::String ^NroAutorIng;

		// Carga 100% manual: los 3 campos arrancan vacios y se tipean leyendo el cupon fisico.
		frmCuponManualPrisma(void)
		{
			InitializeComponent();
			Init();
			this->Titulo->Text = "Carga Manual de Cupon";
		}

	private:
		void Init()
		{
			this->txtNroLote->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCuponManualPrisma::txtSoloNumeros_KeyPress);
			this->txtNroCupon->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCuponManualPrisma::txtSoloNumeros_KeyPress);
			this->txtNroLote->GotFocus += gcnew System::EventHandler(this, &frmCuponManualPrisma::TbGotFocus);
			this->txtNroCupon->GotFocus += gcnew System::EventHandler(this, &frmCuponManualPrisma::TbGotFocus);
			this->txtNroAutor->GotFocus += gcnew System::EventHandler(this, &frmCuponManualPrisma::TbGotFocus);
			this->TabControls->Add(txtNroLote);
			this->TabControls->Add(txtNroCupon);
			this->TabControls->Add(txtNroAutor);
			Strings::InterceptKeyboard(this, 1);
			this->ExtendedKeyboardEnabled = usaTouch;
			this->NroLoteIng = 0;
			this->NroCuponIng = 0;
			this->NroAutorIng = "";
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmCuponManualPrisma()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:
					  bool inGotFocus;
	private: System::Windows::Forms::Label^  label1;
	protected:
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  txtNroLote;
	private: System::Windows::Forms::TextBox^  txtNroCupon;
	private: System::Windows::Forms::TextBox^  txtNroAutor;
	private: System::Windows::Forms::Label^  lblMensaje;
	private: System::Windows::Forms::Timer^  tmrMensaje;

	private: System::ComponentModel::IContainer^  components;



	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->txtNroLote = (gcnew System::Windows::Forms::TextBox());
			this->txtNroCupon = (gcnew System::Windows::Forms::TextBox());
			this->txtNroAutor = (gcnew System::Windows::Forms::TextBox());
			this->lblMensaje = (gcnew System::Windows::Forms::Label());
			this->tmrMensaje = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			//
			// panel
			//
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->txtNroLote);
			this->panel->Controls->Add(this->txtNroCupon);
			this->panel->Controls->Add(this->txtNroAutor);
			this->panel->Controls->Add(this->lblMensaje);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Controls->SetChildIndex(this->lblMensaje, 0);
			this->panel->Controls->SetChildIndex(this->txtNroAutor, 0);
			this->panel->Controls->SetChildIndex(this->txtNroCupon, 0);
			this->panel->Controls->SetChildIndex(this->txtNroLote, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
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
			// Titulo
			//
			this->Titulo->Size = System::Drawing::Size(280, 23);
			this->Titulo->Text = L"Carga Manual de Cupon";
			//
			// label1
			//
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(84, 115);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(100, 19);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Nro Lote:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			//
			// label2
			//
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(84, 145);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(100, 19);
			this->label2->TabIndex = 40;
			this->label2->Text = L"Nro Cupon:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			//
			// label3
			//
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(84, 175);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(100, 19);
			this->label3->TabIndex = 41;
			this->label3->Text = L"Nro Autoriz:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			//
			// txtNroLote
			//
			this->txtNroLote->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->txtNroLote->Location = System::Drawing::Point(190, 112);
			this->txtNroLote->MaxLength = 3;
			this->txtNroLote->Name = L"txtNroLote";
			this->txtNroLote->Size = System::Drawing::Size(100, 26);
			this->txtNroLote->TabIndex = 49;
			//
			// txtNroCupon
			//
			this->txtNroCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->txtNroCupon->Location = System::Drawing::Point(190, 142);
			this->txtNroCupon->MaxLength = 7;
			this->txtNroCupon->Name = L"txtNroCupon";
			this->txtNroCupon->Size = System::Drawing::Size(100, 26);
			this->txtNroCupon->TabIndex = 50;
			//
			// txtNroAutor
			//
			this->txtNroAutor->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->txtNroAutor->Location = System::Drawing::Point(190, 172);
			this->txtNroAutor->MaxLength = 6;
			this->txtNroAutor->Name = L"txtNroAutor";
			this->txtNroAutor->Size = System::Drawing::Size(100, 26);
			this->txtNroAutor->TabIndex = 53;
			//
			// lblMensaje
			//
			this->lblMensaje->AutoSize = true;
			this->lblMensaje->BackColor = System::Drawing::Color::White;
			this->lblMensaje->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lblMensaje->ForeColor = System::Drawing::Color::Red;
			this->lblMensaje->Location = System::Drawing::Point(19, 77);
			this->lblMensaje->Name = L"lblMensaje";
			this->lblMensaje->Size = System::Drawing::Size(55, 18);
			this->lblMensaje->TabIndex = 51;
			this->lblMensaje->Text = L"label5";
			this->lblMensaje->Visible = false;
			//
			// tmrMensaje
			//
			this->tmrMensaje->Interval = 4000;
			this->tmrMensaje->Tick += gcnew System::EventHandler(this, &frmCuponManualPrisma::tmrMensaje_Tick);
			//
			// frmCuponManualPrisma
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Name = L"frmCuponManualPrisma";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmCuponManualPrisma";
			this->TopMost = true;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion


		///Valida que la entrada sean solo numeros
	private: System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
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

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			int auxLote = 0, auxCupon = 0;
			if (Int32::TryParse(txtNroLote->Text, auxLote) && Int32::TryParse(txtNroCupon->Text, auxCupon) &&
				(txtNroAutor->Text->Trim() != ""))
			{
				this->NroLoteIng = auxLote;
				this->NroCuponIng = auxCupon;
				this->NroAutorIng = txtNroAutor->Text->Trim();
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
				BaseDialog::btOk_Click(sender, e);
			}
			else
			{
				this->NroLoteIng = 0;
				this->NroCuponIng = 0;
				this->NroAutorIng = "";
				lblMensaje->Text = "Complete con el numero de lote, cupon y autorizacion";
				lblMensaje->Visible = true;
				tmrMensaje->Enabled = true;
			}
		}

	protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
		{
			this->NroLoteIng = 0;
			this->NroCuponIng = 0;
			this->NroAutorIng = "";
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			BaseDialog::Cancel_Click(sender, e);
		}

	private: System::Void tmrMensaje_Tick(System::Object^  sender, System::EventArgs^  e)
		{
			lblMensaje->Visible = false;
			tmrMensaje->Enabled = false;
		}

		System::Void TbGotFocus(Object ^sender, EventArgs ^e)
		{
			if(inGotFocus) return;
			inGotFocus = true;

			Control ^ctl = dynamic_cast<Control ^>(sender);
			BaseDialog::SetFocusTo(ctl);
			inGotFocus = false;
		}

	protected: virtual void Boton_Click(Object ^sender, EventArgs ^e) override
	 {
		 TextBox^ textbx = dynamic_cast<TextBox ^>(tb);
		 if (textbx->Text->Length < textbx->MaxLength)
			BaseDialog::Boton_Click(sender, e);
	 }
	};
}
