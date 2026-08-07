#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "DatosMpagos.h"
#include "FUNCS.H"


namespace pos {

	/// <summary>
	/// Summary for frmCupones
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmCupones : public Controles::BaseDialog
	{
	public:
		frmCupones(CPosnet ^paramposnet, String^ nametarj)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->posnet = paramposnet;
			Strings::InterceptKeyboard(this, 1);			
			txtNroLote->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCupones::txtSoloNumeros_KeyPress);
			txtNroCupon->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCupones::txtSoloNumeros_KeyPress);
			//txtNroAutor->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCupones::txtSoloNumeros_KeyPress);
			txtExtraccion->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmCupones::txtMonto_KeyPress);
			txtNroLote->GotFocus += gcnew System::EventHandler(this, &frmCupones::TbGotFocus);
			txtNroCupon->GotFocus += gcnew System::EventHandler(this, &frmCupones::TbGotFocus);
			txtNroAutor->GotFocus += gcnew System::EventHandler(this, &frmCupones::TbGotFocus);
			txtExtraccion->GotFocus += gcnew System::EventHandler(this, &frmCupones::TbGotFocus);
			this->TabControls->Add(txtNroLote);
			this->TabControls->Add(txtNroCupon);
			this->TabControls->Add(txtNroAutor);
			if (paramposnet->m_credito)
			{
				txtExtraccion->Visible = false;
				lblExtraccion->Visible = false;
			} 
			else
			{ 
				txtExtraccion->Visible = false;
				lblExtraccion->Visible = false;
				if (paramposnet->extraible)
				{				
					this->TabControls->Add(txtExtraccion);
					txtExtraccion->Visible = true;
					lblExtraccion->Visible = true;
				}
			}
			this->ExtendedKeyboardEnabled = usaTouch;
			String^ nrot = gcnew String(posnet->m_nrotarjeta);
			this->lblNroTarjeta->Text = "****-" + (nrot->Length > 15 ? nrot->Substring(12) : nrot->Substring(2));
			this->lblTarjeta->Text = nametarj;
			this->lblMonto->Text = (posnet->m_monto + posnet->m_recargo).ToString("F2");
			this->lblCuotas->Text = posnet->m_cuotas.ToString();
			this->txtNroLote->Text = posnet->m_lote == 0 ? String::Empty : posnet->m_lote.ToString();
			this->txtNroCupon->Text = posnet->m_cupon == 0 ? String::Empty : posnet->m_cupon.ToString();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmCupones()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: CPosnet ^posnet;
				  bool inGotFocus;
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  lblCuotas;
	private: System::Windows::Forms::Label^  lblMonto;
	private: System::Windows::Forms::Label^  lblNroTarjeta;
	private: System::Windows::Forms::Label^  lblTarjeta;
	private: System::Windows::Forms::Label^  lblNroLote;
	private: System::Windows::Forms::Label^  lblNroCupon;
	private: System::Windows::Forms::TextBox^  txtNroLote;
	private: System::Windows::Forms::TextBox^  txtNroCupon;
	private: System::Windows::Forms::Label^  lblMensaje;
	private: System::Windows::Forms::Timer^  tmrMensaje;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  txtNroAutor;
	private: System::Windows::Forms::TextBox^  txtExtraccion;
	private: System::Windows::Forms::Label^  lblExtraccion;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmCupones::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->lblNroTarjeta = (gcnew System::Windows::Forms::Label());
			this->lblTarjeta = (gcnew System::Windows::Forms::Label());
			this->lblCuotas = (gcnew System::Windows::Forms::Label());
			this->lblMonto = (gcnew System::Windows::Forms::Label());
			this->lblNroLote = (gcnew System::Windows::Forms::Label());
			this->lblNroCupon = (gcnew System::Windows::Forms::Label());
			this->txtNroLote = (gcnew System::Windows::Forms::TextBox());
			this->txtNroCupon = (gcnew System::Windows::Forms::TextBox());
			this->lblMensaje = (gcnew System::Windows::Forms::Label());
			this->tmrMensaje = (gcnew System::Windows::Forms::Timer(this->components));
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->txtNroAutor = (gcnew System::Windows::Forms::TextBox());
			this->lblExtraccion = (gcnew System::Windows::Forms::Label());
			this->txtExtraccion = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->lblCuotas);
			this->panel->Controls->Add(this->lblTarjeta);
			this->panel->Controls->Add(this->lblNroLote);
			this->panel->Controls->Add(this->lblNroCupon);
			this->panel->Controls->Add(this->txtNroLote);
			this->panel->Controls->Add(this->txtNroCupon);
			this->panel->Controls->Add(this->lblMensaje);
			this->panel->Controls->Add(this->lblMonto);
			this->panel->Controls->Add(this->lblNroTarjeta);
			this->panel->Controls->Add(this->txtNroAutor);
			this->panel->Controls->Add(this->label5);
			this->panel->Controls->Add(this->txtExtraccion);
			this->panel->Controls->Add(this->lblExtraccion);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Controls->SetChildIndex(this->lblExtraccion, 0);
			this->panel->Controls->SetChildIndex(this->txtExtraccion, 0);
			this->panel->Controls->SetChildIndex(this->label5, 0);
			this->panel->Controls->SetChildIndex(this->txtNroAutor, 0);
			this->panel->Controls->SetChildIndex(this->lblNroTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->lblMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblMensaje, 0);
			this->panel->Controls->SetChildIndex(this->txtNroCupon, 0);
			this->panel->Controls->SetChildIndex(this->txtNroLote, 0);
			this->panel->Controls->SetChildIndex(this->lblNroCupon, 0);
			this->panel->Controls->SetChildIndex(this->lblNroLote, 0);
			this->panel->Controls->SetChildIndex(this->lblTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->lblCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
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
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(269, 23);
			this->Titulo->Text = L"Completar Datos del cupon";
			// 
			// bEnter
			// 
			this->bEnter->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bEnter.Image")));
			// 
			// label1
			// 
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(84, 115);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(100, 19);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Nro Tarjeta:";
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
			this->label2->Text = L"Tarjeta:";
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
			this->label3->Text = L"Cuotas:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label4
			// 
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(84, 205);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(100, 19);
			this->label4->TabIndex = 42;
			this->label4->Text = L"Monto $$:";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblNroTarjeta
			// 
			this->lblNroTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblNroTarjeta->Location = System::Drawing::Point(190, 115);
			this->lblNroTarjeta->Name = L"lblNroTarjeta";
			this->lblNroTarjeta->Size = System::Drawing::Size(177, 19);
			this->lblNroTarjeta->TabIndex = 43;
			this->lblNroTarjeta->Text = L"1234567890123456";
			// 
			// lblTarjeta
			// 
			this->lblTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblTarjeta->Location = System::Drawing::Point(190, 145);
			this->lblTarjeta->Name = L"lblTarjeta";
			this->lblTarjeta->Size = System::Drawing::Size(201, 19);
			this->lblTarjeta->TabIndex = 44;
			this->lblTarjeta->Text = L"label6";
			// 
			// lblCuotas
			// 
			this->lblCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblCuotas->Location = System::Drawing::Point(190, 175);
			this->lblCuotas->Name = L"lblCuotas";
			this->lblCuotas->Size = System::Drawing::Size(100, 19);
			this->lblCuotas->TabIndex = 45;
			this->lblCuotas->Text = L"0";
			// 
			// lblMonto
			// 
			this->lblMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMonto->Location = System::Drawing::Point(190, 205);
			this->lblMonto->Name = L"lblMonto";
			this->lblMonto->Size = System::Drawing::Size(100, 19);
			this->lblMonto->TabIndex = 46;
			this->lblMonto->Text = L"0.00";
			// 
			// lblNroLote
			// 
			this->lblNroLote->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblNroLote->Location = System::Drawing::Point(84, 245);
			this->lblNroLote->Name = L"lblNroLote";
			this->lblNroLote->Size = System::Drawing::Size(100, 19);
			this->lblNroLote->TabIndex = 47;
			this->lblNroLote->Text = L"Nro Lote:";
			this->lblNroLote->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblNroCupon
			// 
			this->lblNroCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblNroCupon->Location = System::Drawing::Point(84, 275);
			this->lblNroCupon->Name = L"lblNroCupon";
			this->lblNroCupon->Size = System::Drawing::Size(100, 19);
			this->lblNroCupon->TabIndex = 48;
			this->lblNroCupon->Text = L"Nro Cupon:";
			this->lblNroCupon->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtNroLote
			// 
			this->txtNroLote->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroLote->Location = System::Drawing::Point(190, 245);
			this->txtNroLote->MaxLength = 3;
			this->txtNroLote->Name = L"txtNroLote";
			this->txtNroLote->Size = System::Drawing::Size(100, 26);
			this->txtNroLote->TabIndex = 49;
			// 
			// txtNroCupon
			// 
			this->txtNroCupon->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroCupon->Location = System::Drawing::Point(190, 275);
			this->txtNroCupon->MaxLength = 4;
			this->txtNroCupon->Name = L"txtNroCupon";
			this->txtNroCupon->Size = System::Drawing::Size(100, 26);
			this->txtNroCupon->TabIndex = 50;
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
			this->tmrMensaje->Tick += gcnew System::EventHandler(this, &frmCupones::tmrMensaje_Tick);
			// 
			// label5
			// 
			this->label5->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(74, 306);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(110, 23);
			this->label5->TabIndex = 52;
			this->label5->Text = L"Nro Autoriz:";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtNroAutor
			// 
			this->txtNroAutor->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroAutor->Location = System::Drawing::Point(190, 305);
			this->txtNroAutor->MaxLength = 6;
			this->txtNroAutor->Name = L"txtNroAutor";
			this->txtNroAutor->Size = System::Drawing::Size(100, 26);
			this->txtNroAutor->TabIndex = 53;
			// 
			// lblExtraccion
			// 
			this->lblExtraccion->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblExtraccion->Location = System::Drawing::Point(58, 359);
			this->lblExtraccion->Name = L"lblExtraccion";
			this->lblExtraccion->Size = System::Drawing::Size(126, 23);
			this->lblExtraccion->TabIndex = 54;
			this->lblExtraccion->Text = L"Extracción $$:";
			this->lblExtraccion->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtExtraccion
			// 
			this->txtExtraccion->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtExtraccion->Location = System::Drawing::Point(190, 359);
			this->txtExtraccion->MaxLength = 8;
			this->txtExtraccion->Name = L"txtExtraccion";
			this->txtExtraccion->Size = System::Drawing::Size(100, 26);
			this->txtExtraccion->TabIndex = 55;
			// 
			// frmCupones
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Name = L"frmCupones";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmCupones";
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
			double auxextrac = 0;
			bool errorextrac = false;
			if (Int32::TryParse(txtNroLote->Text, posnet->m_lote) && Int32::TryParse(txtNroCupon->Text, posnet->m_cupon) &&
				(txtNroAutor->Text->Trim() != ""))
			{
				posnet->m_autorizacion = txtNroAutor->Text;
				if (txtExtraccion->Text->Length > 0)
				{
					errorextrac = true;
					struct mpag_ *lmpag = BuscarMpag(posnet->m_mpago);
					if (Double::TryParse(txtExtraccion->Text, auxextrac))
					{						
						//if (auxextrac >= minlimextrac && auxextrac <= maxlimextrac)
						if (auxextrac >= lmpag->minextracc && auxextrac <= lmpag->maxextracc)
						{
							posnet->m_extraccion = auxextrac;
							errorextrac = false;
						}
					}
					if (errorextrac)
					{
						lblMensaje->Text = String::Format("Monto de extracción INVALIDO. MIN $ = {0}, MAX $ = {1}", lmpag->minextracc, lmpag->maxextracc);
						lblMensaje->Visible = true;
						tmrMensaje->Enabled = true;
						return;
					}
					lmpag = NULL;

				}
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
				BaseDialog::btOk_Click(sender, e);
			}
			else
			{				
				posnet->m_lote = 0;
				posnet->m_cupon = 0;
				posnet->m_autorizacion = "";
				lblMensaje->Text = "Complete con el número de lote, Cupon y autorización";
				lblMensaje->Visible = true;
				tmrMensaje->Enabled = true;
			}
		}
	
	protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
		{
			posnet->m_lote = 0;
			posnet->m_cupon = 0;
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			BaseDialog::Cancel_Click(sender, e);
		}

	private: System::Void tmrMensaje_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			lblMensaje->Visible = false;
			tmrMensaje->Enabled = false;
		}

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

	void TbGotFocus(Object ^sender, EventArgs ^e)
		{
			if(inGotFocus) return;
			inGotFocus = true;

			Control ^ctl = dynamic_cast<Control ^>(sender);			
			BaseDialog::SetFocusTo(ctl);			
			inGotFocus = false;
		}

	protected: void virtual bEnter_Click(Object ^sender, EventArgs ^e) override
	  {
		  if(tb != nullptr && tb->Name == "txtExtraccion" && tb->Text->IndexOf('.') < 0)
		  {				
			  if (tb->Text->Length == 0)
				  tb->Text = "0.";
			  else
				  tb->Text = tb->Text->Trim() + ".";
		  }
		  //procesarClicks = false;
	  }
   
	protected: virtual void Boton_Click(Object ^sender, EventArgs ^e) override
	 {
		 TextBox^ textbx = dynamic_cast<TextBox ^>(tb);
		 if (textbx->Text->Length < textbx->MaxLength)
			BaseDialog::Boton_Click(sender, e);
	 }
};
}
