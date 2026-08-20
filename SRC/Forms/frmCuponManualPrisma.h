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
			this->txtNroAutor->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCuponManualPrisma::txtAlfaNum_KeyPress);
			this->TabControls->Add(txtNroLote);
			this->TabControls->Add(txtNroCupon);
			this->TabControls->Add(txtNroAutor);
			Strings::InterceptKeyboard(this, 1);
			this->ExtendedKeyboardEnabled = usaTouch;
			this->NroLoteIng = 0;
			this->NroCuponIng = 0;
			this->NroAutorIng = "";
			this->mtTeclas = nullptr;
			this->mtIndice = -1;
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
	private: System::Windows::Forms::Label^  lblTeclas;
	private: System::Windows::Forms::Label^  lblAyudaLetras;
	private: System::Windows::Forms::Timer^  tmrMultiTap;

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
			this->lblTeclas = (gcnew System::Windows::Forms::Label());
			this->lblAyudaLetras = (gcnew System::Windows::Forms::Label());
			this->tmrMultiTap = (gcnew System::Windows::Forms::Timer(this->components));
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
			this->panel->Controls->Add(this->lblTeclas);
			this->panel->Controls->Add(this->lblAyudaLetras);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Controls->SetChildIndex(this->lblAyudaLetras, 0);
			this->panel->Controls->SetChildIndex(this->lblTeclas, 0);
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
			this->txtNroAutor->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->txtNroAutor->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->txtNroAutor->Location = System::Drawing::Point(190, 172);
			this->txtNroAutor->MaxLength = 6;
			this->txtNroAutor->Name = L"txtNroAutor";
			this->txtNroAutor->Size = System::Drawing::Size(100, 26);
			this->txtNroAutor->TabIndex = 53;
			//
			// lblTeclas
			//
			this->lblTeclas->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->lblTeclas->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lblTeclas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lblTeclas->ForeColor = System::Drawing::Color::Blue;
			this->lblTeclas->Location = System::Drawing::Point(300, 172);
			this->lblTeclas->Name = L"lblTeclas";
			this->lblTeclas->Size = System::Drawing::Size(150, 26);
			this->lblTeclas->TabIndex = 54;
			this->lblTeclas->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lblTeclas->Visible = false;
			//
			// lblAyudaLetras
			//
			this->lblAyudaLetras->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lblAyudaLetras->Location = System::Drawing::Point(84, 202);
			this->lblAyudaLetras->Name = L"lblAyudaLetras";
			this->lblAyudaLetras->Size = System::Drawing::Size(380, 32);
			this->lblAyudaLetras->TabIndex = 55;
			this->lblAyudaLetras->Text = L"En Nro Autoriz se cargan letras tocando la misma tecla varias veces (2=ABC, 3=DEF, ...).";
			this->lblAyudaLetras->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			//
			// tmrMultiTap
			//
			this->tmrMultiTap->Interval = 1000;
			this->tmrMultiTap->Tick += gcnew System::EventHandler(this, &frmCuponManualPrisma::tmrMultiTap_Tick);
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

		///Valida que la entrada sean letras o numeros (para el nro de autorizacion)
	private: System::Void txtAlfaNum_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			// Escribir con el teclado fisico corta cualquier ciclo del teclado touch.
			CerrarMultiTap();

			wchar_t ch = Char::ToUpper(e->KeyChar);
			e->Handled = !((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || Char::IsControl(e->KeyChar));
		}

		// --- Teclado multi-tap para el Nro de Autorizacion --------------------------
		// El cupon de Prisma puede traer un codigo de autorizacion alfanumerico, pero el
		// teclado touch de la caja solo tiene los digitos 0 a 9. Se reusa la idea del
		// teclado corto del Buscador: cada tecla cicla entre su digito y sus letras si se
		// la toca de nuevo antes de un segundo. El primer toque siempre deja el digito, asi
		// que quien carga solo numeros no nota ningun cambio.
	private:
		System::String ^mtTeclas;	// caracteres del boton en curso, o nullptr si no hay ciclo activo
		int mtIndice;				// posicion, dentro de mtTeclas, del caracter que ya se escribio

		System::String ^TeclasDelBoton(System::String ^nombre)
		{
			if (nombre == "b0") return "0";
			if (nombre == "b1") return "1";
			if (nombre == "b2") return "2ABC";
			if (nombre == "b3") return "3DEF";
			if (nombre == "b4") return "4GHI";
			if (nombre == "b5") return "5JKL";
			if (nombre == "b6") return "6MNO";
			if (nombre == "b7") return "7PQRS";
			if (nombre == "b8") return "8TUV";
			if (nombre == "b9") return "9WXYZ";
			return nullptr;
		}

		// Cierra el ciclo en curso: el caracter que se ve en el campo queda confirmado.
		void CerrarMultiTap()
		{
			tmrMultiTap->Enabled = false;
			mtTeclas = nullptr;
			mtIndice = -1;
			lblTeclas->Visible = false;
		}

		// Muestra las opciones de la tecla en curso, con la elegida entre corchetes.
		void MostrarTeclas()
		{
			System::Text::StringBuilder ^sb = gcnew System::Text::StringBuilder();
			for (int i = 0; i < mtTeclas->Length; i++)
			{
				if (i > 0)
					sb->Append(L" ");
				if (i == mtIndice)
					sb->Append(L"[")->Append(mtTeclas->Substring(i, 1))->Append(L"]");
				else
					sb->Append(mtTeclas->Substring(i, 1));
			}
			lblTeclas->Text = sb->ToString();
			lblTeclas->Visible = true;
		}

	private: System::Void tmrMultiTap_Tick(System::Object^  sender, System::EventArgs^  e)
		{
			CerrarMultiTap();
		}

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			CerrarMultiTap();

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
			CerrarMultiTap();
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
			if (ctl != tb)
				CerrarMultiTap();
			BaseDialog::SetFocusTo(ctl);
			inGotFocus = false;
		}

	protected: virtual void Boton_Click(Object ^sender, EventArgs ^e) override
	 {
		 TextBox^ textbx = dynamic_cast<TextBox ^>(tb);
		 if (textbx == nullptr)
			 return;

		 // Lote y cupon son numericos puros: el comportamiento de siempre.
		 if (textbx != txtNroAutor)
		 {
			 CerrarMultiTap();
			 if (textbx->Text->Length < textbx->MaxLength)
				 BaseDialog::Boton_Click(sender, e);
			 return;
		 }

		 Button ^boton = dynamic_cast<Button ^>(sender);
		 System::String ^teclas = (boton != nullptr)? TeclasDelBoton(boton->Name): nullptr;
		 if (teclas == nullptr)
		 {
			 CerrarMultiTap();
			 return;
		 }

		 if (mtTeclas == teclas && mtIndice >= 0 && textbx->Text->Length > 0)
		 {
			 // Toque repetido de la misma tecla: cambia el caracter que ya se escribio.
			 mtIndice = (mtIndice + 1) % teclas->Length;
			 textbx->Text = textbx->Text->Substring(0, textbx->Text->Length - 1) + teclas->Substring(mtIndice, 1);
		 }
		 else
		 {
			 // Otra tecla, o se vencio el segundo de espera: confirma la anterior y agrega una nueva.
			 CerrarMultiTap();
			 if (borrar)
			 {
				 textbx->Text = "";
				 borrar = false;
			 }
			 if (textbx->Text->Length >= textbx->MaxLength)
				 return;
			 mtTeclas = teclas;
			 mtIndice = 0;
			 textbx->Text += teclas->Substring(0, 1);
		 }

		 textbx->SelectionStart = textbx->Text->Length;
		 if (teclas->Length > 1)
		 {
			 MostrarTeclas();
			 tmrMultiTap->Enabled = false;
			 tmrMultiTap->Enabled = true;	// reinicia el segundo de espera
		 }
		 else
			 CerrarMultiTap();
	 }

	public: virtual bool ProcessHotKey(Keys key) override
	 {
		 if (key == Keys::Left)
			 CerrarMultiTap();
		 return BaseDialog::ProcessHotKey(key);
	 }

	protected: virtual void bBorra_Click(Object ^sender, EventArgs ^e) override
	 {
		 CerrarMultiTap();
		 BaseDialog::bBorra_Click(sender, e);
	 }

	protected: virtual void bEnter_Click(Object ^sender, EventArgs ^e) override
	 {
		 CerrarMultiTap();
		 BaseDialog::bEnter_Click(sender, e);
	 }

	protected: virtual void SetFocusTo(Control ^ctl) override
	 {
		 if (ctl != tb)
			 CerrarMultiTap();
		 BaseDialog::SetFocusTo(ctl);
	 }
	};
}
