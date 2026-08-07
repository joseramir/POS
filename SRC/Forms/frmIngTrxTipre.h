#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Bitnet::TJOCommon;
using namespace System::Reflection;

#include "StaticDump.h"

namespace pos {

	/// <summary>
	/// Summary for frmIngTrxTipre
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmIngTrxTipre : public Controles::BaseDialog
	{
	public:
		frmIngTrxTipre(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->txtIdTrxTipre->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngTrxTipre::txtSoloNumeros_KeyPress);
			this->txtImporte->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngTrxTipre::txtSoloMonto_KeyPress);
			this->tbDni->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngTrxTipre::txtSoloNumeros_KeyPress);
			this->tbCard->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngTrxTipre::txtSoloNumeros_KeyPress);
			

			Strings::InterceptKeyboard(this, 1);
			TabControls->Add(this->tbCard);
			TabControls->Add(this->txtIdTrxTipre);
			TabControls->Add(this->txtImporte);
			TabControls->Add(this->tbDni);
			
			this->ExtendedKeyboardEnabled = usaTouch;
			btnDot->Visible = usaTouch;
			btnManual->Visible = usaTouch;
			
			actualTipre = gcnew TipreRequest();
			lbFaltan->Visible = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmIngTrxTipre()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtIdTrxTipre;
	protected: 

	protected: 
	private: System::Windows::Forms::Label^  label1;

	
	private: TipreRequest ^actualTipre;
			 bool globalTouchEvent;

	private: System::Windows::Forms::TextBox^  txtImporte;
	public: 

	public: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  btnDot;
	private: System::Windows::Forms::TextBox^  tbDummy;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  tbCard;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  tbDni;
	private: System::Windows::Forms::Button^  btnManual;
	private: System::Windows::Forms::Label^  lbFaltan;
	private: System::Windows::Forms::Timer^  timerFaltan;
	private: System::Windows::Forms::Label^  lbCardHelp;
	private: System::Windows::Forms::TextBox^  tbNom;

	private: System::Windows::Forms::Label^  label5;



	private: System::ComponentModel::IContainer^  components;





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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmIngTrxTipre::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtIdTrxTipre = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txtImporte = (gcnew System::Windows::Forms::TextBox());
			this->btnDot = (gcnew System::Windows::Forms::Button());
			this->tbDummy = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tbCard = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->tbDni = (gcnew System::Windows::Forms::TextBox());
			this->btnManual = (gcnew System::Windows::Forms::Button());
			this->lbFaltan = (gcnew System::Windows::Forms::Label());
			this->timerFaltan = (gcnew System::Windows::Forms::Timer(this->components));
			this->lbCardHelp = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->tbNom = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->tbNom);
			this->panel->Controls->Add(this->label5);
			this->panel->Controls->Add(this->lbCardHelp);
			this->panel->Controls->Add(this->lbFaltan);
			this->panel->Controls->Add(this->btnManual);
			this->panel->Controls->Add(this->btnDot);
			this->panel->Controls->Add(this->tbDummy);
			this->panel->Controls->Add(this->txtIdTrxTipre);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->txtImporte);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->tbCard);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->tbDni);
			this->panel->Size = System::Drawing::Size(555, 477);
			this->panel->Controls->SetChildIndex(this->tbDni, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->tbCard, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->txtImporte, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->txtIdTrxTipre, 0);
			this->panel->Controls->SetChildIndex(this->tbDummy, 0);
			this->panel->Controls->SetChildIndex(this->btnDot, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
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
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->btnManual, 0);
			this->panel->Controls->SetChildIndex(this->lbFaltan, 0);
			this->panel->Controls->SetChildIndex(this->lbCardHelp, 0);
			this->panel->Controls->SetChildIndex(this->label5, 0);
			this->panel->Controls->SetChildIndex(this->tbNom, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(375, 389);
			// 
			// btOk
			// 
			this->btOk->Enabled = false;
			this->btOk->Location = System::Drawing::Point(375, 337);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(431, 221);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(375, 221);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(431, 169);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(375, 169);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(431, 117);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(375, 117);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(431, 65);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(375, 65);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(540, 4);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(328, 23);
			this->Titulo->Text = L"Anulación Tarjeta Sistema Propio";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(487, 221);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(487, 169);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(487, 117);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(487, 65);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(37, 202);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(88, 16);
			this->label1->TabIndex = 39;
			this->label1->Text = L"IDTRXTIPRE:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// txtIdTrxTipre
			// 
			this->txtIdTrxTipre->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtIdTrxTipre->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtIdTrxTipre->Location = System::Drawing::Point(143, 200);
			this->txtIdTrxTipre->Name = L"txtIdTrxTipre";
			this->txtIdTrxTipre->Size = System::Drawing::Size(110, 23);
			this->txtIdTrxTipre->TabIndex = 40;
			this->txtIdTrxTipre->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::AnyTbClick);
			this->txtIdTrxTipre->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmIngTrxTipre::txtIdTrxTipre_Validating);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(6, 239);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(122, 16);
			this->label2->TabIndex = 41;
			this->label2->Text = L"IMPORTE Compra:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// txtImporte
			// 
			this->txtImporte->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtImporte->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtImporte->Location = System::Drawing::Point(143, 237);
			this->txtImporte->Name = L"txtImporte";
			this->txtImporte->Size = System::Drawing::Size(110, 23);
			this->txtImporte->TabIndex = 42;
			this->txtImporte->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::AnyTbClick);
			this->txtImporte->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmIngTrxTipre::txtImporte_Validating);
			// 
			// btnDot
			// 
			this->btnDot->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnDot.Image")));
			this->btnDot->Location = System::Drawing::Point(487, 275);
			this->btnDot->Name = L"btnDot";
			this->btnDot->Size = System::Drawing::Size(50, 48);
			this->btnDot->TabIndex = 43;
			this->btnDot->UseVisualStyleBackColor = true;
			this->btnDot->Visible = false;
			this->btnDot->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::btnDot_Click);
			// 
			// tbDummy
			// 
			this->tbDummy->Location = System::Drawing::Point(260, 65);
			this->tbDummy->Name = L"tbDummy";
			this->tbDummy->Size = System::Drawing::Size(100, 20);
			this->tbDummy->TabIndex = 44;
			this->tbDummy->Visible = false;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(65, 133);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(60, 16);
			this->label3->TabIndex = 45;
			this->label3->Text = L"Tarjeta:";
			// 
			// tbCard
			// 
			this->tbCard->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCard->Enabled = false;
			this->tbCard->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCard->Location = System::Drawing::Point(143, 130);
			this->tbCard->MaxLength = 16;
			this->tbCard->Name = L"tbCard";
			this->tbCard->Size = System::Drawing::Size(194, 23);
			this->tbCard->TabIndex = 46;
			this->tbCard->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::AnyTbClick);
			this->tbCard->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmIngTrxTipre::tbCard_Validating);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(90, 276);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(35, 16);
			this->label4->TabIndex = 47;
			this->label4->Text = L"DNI:";
			// 
			// tbDni
			// 
			this->tbDni->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbDni->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbDni->Location = System::Drawing::Point(143, 271);
			this->tbDni->MaxLength = 10;
			this->tbDni->Name = L"tbDni";
			this->tbDni->Size = System::Drawing::Size(110, 23);
			this->tbDni->TabIndex = 48;
			this->tbDni->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::AnyTbClick);
			this->tbDni->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmIngTrxTipre::tbDni_Validating);
			// 
			// btnManual
			// 
			this->btnManual->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnManual->Location = System::Drawing::Point(376, 441);
			this->btnManual->Name = L"btnManual";
			this->btnManual->Size = System::Drawing::Size(162, 32);
			this->btnManual->TabIndex = 102;
			this->btnManual->Text = L"Ing. Manual";
			this->btnManual->UseVisualStyleBackColor = true;
			this->btnManual->Click += gcnew System::EventHandler(this, &frmIngTrxTipre::btnManual_Click);
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
			this->lbFaltan->TabIndex = 103;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &frmIngTrxTipre::timerFaltan_Tick);
			// 
			// lbCardHelp
			// 
			this->lbCardHelp->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lbCardHelp->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbCardHelp->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCardHelp->Location = System::Drawing::Point(19, 73);
			this->lbCardHelp->Name = L"lbCardHelp";
			this->lbCardHelp->Size = System::Drawing::Size(335, 36);
			this->lbCardHelp->TabIndex = 104;
			this->lbCardHelp->Text = L"Pase la Tarjeta por el Lector de Banda...";
			this->lbCardHelp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(63, 167);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(62, 16);
			this->label5->TabIndex = 105;
			this->label5->Text = L"Nombre:";
			// 
			// tbNom
			// 
			this->tbNom->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbNom->Enabled = false;
			this->tbNom->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbNom->Location = System::Drawing::Point(143, 165);
			this->tbNom->Name = L"tbNom";
			this->tbNom->Size = System::Drawing::Size(194, 23);
			this->tbNom->TabIndex = 106;
			// 
			// frmIngTrxTipre
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(555, 477);
			this->Name = L"frmIngTrxTipre";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmIngTrxTipre";
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

		protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			if(txtIdTrxTipre->Text == "") 
			{
				Alert("No se ingreso el Numero de Transaccion", "Ingreselo o Cancele la operación");
				return;
			}
			System::Decimal auximp = 0;
			System::Decimal::TryParse(txtImporte->Text, auximp);
			if (auximp <= 0)
			{
				Alert("Corregir el importe ingresado", "");
				return;
			}
			actualTipre->IdTrxTipre = System::Convert::ToUInt64(txtIdTrxTipre->Text);
			actualTipre->Importe = System::Convert::ToDecimal(txtImporte->Text);
			Dump::TarjTipre->Add(actualTipre);
			bEnter_Click(sender, e);
			BaseDialog::btOk_Click(sender, e);
		}

		System::Void virtual Cancel_Click(Object ^sender, EventArgs ^e) override
		{			
			BaseDialog::Cancel_Click(sender, e);			
		}

	private: System::Void btnDot_Click(System::Object^  sender, System::EventArgs^  e) {
				 if(tb != nullptr && tb->Name == "txtImporte" && tb->Text->IndexOf('.') < 0)
				 {				
					 if (tb->Text->Length == 0)
						 tb->Text = "0.";
					 else
						 tb->Text = tb->Text->Trim() + ".";
				 }
			 }

		public: virtual void StartCardReader() override
			{
				
				actualTipre->Clear();

				this->tbDummy->Enabled = true;
				this->tbDummy->Focus();
				BaseDialog::StartCardReader();
			}

		public: virtual bool CardReaderDigit(wchar_t ch) override
			{
				bool res = false;
				try
				{
					res = BaseDialog::CardReaderDigit(ch);
					this->lbCardHelp->BackColor = 
						(this->lbCardHelp->BackColor == System::Drawing::Color::Lime)?
						Color::FromArgb(255, 255, 128):
					System::Drawing::Color::Lime;
					//System::Windows::Forms::Application::DoEvents();
				}
				catch (Exception ^excep)
				{
					WriteBootLog("ERROR - AL LEER LA BANDA MAGNETICA DE LA TARJETA");
				}
				return res;
			}
		
		public: virtual void EndCardReader() override
			{
				BaseDialog::EndCardReader();
						
				this->lbCardHelp->BackColor = Color::FromArgb(255, 255, 128);

				if(cardAdicData->Length > 0 && cardNumber->Length > 0 && cardVenc > 0)
				{
					actualTipre->Id = this->cardData->ToString();
					tbCard->Text = this->cardNumber;
					actualTipre->Nro_Tarjeta = cardNumber;
					actualTipre->NombreCliente = cardHolder;					
					///*this->tbVenc->Text = cardVenc.ToString();
					//if (this->tbVenc->TextLength == 3)
					//	this->tbVenc->Text = "0" + cardVenc.ToString();*/
					//globalTouchEvent = true;
					//tbVenc_Validating(nullptr, nullptr);
					//globalTouchEvent = false;
					//if (!tarjVencida)
					//{				
						//***DecodeCard();
					GenValidacion();
						ActualizarDatosTarjeta();
					/*}	*/			
				}
				else
					Alert("Tarjeta mal leida o Defectuosa.", "Pruebe de Nuevo...");

				this->tbDummy->Enabled = false;
				SetFocusTo(tb);
						
			}

		private: void ActualizarDatosTarjeta()
			 {
				 this->tbCard->Text = actualTipre->Nro_Tarjeta;
				 //this->tbVenc->Text = actualTipre->VencimFormateado();
				 this->tbNom->Text = actualTipre->NombreCliente;
				 //this->tbMonto->Text = actualTipre->Importe.ToString("N2");
				 //LlenaPlan();
				 //this->tbDni->Text = actualTipre->DniFormateado();
				 //this->tbCvv->Text = actualTipre->CodigoSeguridad;
				 //this->ult4Dig->Text = actualTipre->Ult4DigitosTarjeta;

				 //this->lbMarca->Text = "(" + actualTipre->Marca() + ")";
				 ////this->lbCuoAvail->Text = "Cuotas Posibles: "; //+ Strings::CharToString(actualTarj->CuotasFormateadas());
				 ////this->lbCuotas->Text = actualTipre->PlanDePago;
				 //this->lbReca->Text = "(Recargo: $" + actualTipre->ImporteRecargo.ToString("N2") + ")";

				 //this->tbCard->Enabled = IngManual && tbCard->TextLength == 0;
				 //this->tbVenc->Enabled = IngManual && tbVenc->TextLength == 0;
				 //this->tbNom->Enabled = false;
				 //this->tbMonto->Enabled = true;
				 //this->cboPlan->Enabled = true;
				 //this->tbDni->Enabled = true;
				 //this->tbCvv->Enabled = (actualTipre == nullptr || actualTipre->PideCvv());

				 //this->tbCvv->Visible = this->tbCvv->Enabled;
				 //this->lbCvv->Visible = this->tbCvv->Enabled;
			 }

		private: System::Void GenValidacion()
			 {
				 bool res = false;				 
				 
				 res = actualTipre != nullptr && actualTipre->DatosValidosAnulacion() == "OK";

				 timerFaltan->Enabled = !res;// && actualTipre->Nro_Tarjeta != "";
				/* if(!res)
				 {*/
					 lbFaltan->Visible = true;
					 lbFaltan->Text = (!res? "Faltan Datos...": "Tarjeta Lista...");
					 lbFaltan->ForeColor = (res? Color::Red: Color::Beige);
					 lbFaltan->BackColor = (res? Color::White: Color::Blue);
					 lbFaltan->BorderStyle = (res? System::Windows::Forms::BorderStyle::FixedSingle: System::Windows::Forms::BorderStyle::None);
				 /*}
				 else
					 lbFaltan->Visible = false;*/

				 btOk->Enabled = res;
			 }

		private: System::Void AnyTbClick(System::Object^  sender, System::EventArgs^  e) 
			{
				if(pantalla != 1 && pantalla != 3) 
					SetFocusTo((Control ^)sender);
			}

		private: System::Void tbDni_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
			 {				 
				 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
				 {
					 String ^tbText = this->tbDni->Text->Trim();
					 actualTipre->Documento = 0;
					 StringBuilder ^cadDni = gcnew StringBuilder();
					 for(int k = 0; k < tbText->Length; k++)
						 if (Char::IsDigit(tbText[k]))
							 cadDni->Append(tbText[k]);
					 if (cadDni->ToString()->Length > 0)
						 actualTipre->Documento = System::Convert::ToUInt32(cadDni->ToString());

					 if(actualTipre->Documento <= 99999 || actualTipre->Documento > 99999999)
					 {
						 actualTipre->Documento = 0;
						 Alert("Numero de Documento Invalido", "Pruebe de nuevo...");
					 }

					 this->tbDni->Text = actualTipre->DniFormateado();
					 GenValidacion();
				 }
			 }
		private: System::Void txtIdTrxTipre_Validating(System::Object^ sender, System::ComponentModel::CancelEventArgs^e){
				 if ((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
				 {
					 System::Int64 auxnum = 0;
					 System::Int64::TryParse(txtIdTrxTipre->Text, auxnum);
					 
					 if(auxnum <= 0) 
					 {
						 Alert("No se ingreso el Numero de Transaccion", "Ingreselo o Cancele la operación");							 
					 }
					 actualTipre->IdTrxTipre = auxnum;
					 GenValidacion();
				 }
			 }

		private: System::Void txtImporte_Validating(System::Object^ sender, System::ComponentModel::CancelEventArgs^e){
			 if ((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 System::Decimal auximp = 0;
				 System::Decimal::TryParse(txtImporte->Text, auximp);
				 if (auximp <= 0)
				 {
					 Alert("Corregir el importe ingresado", "");							 
				 }
				 actualTipre->Importe = auximp;
				 GenValidacion();
			 }
		 }

	 private: System::Void timerFaltan_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 lbFaltan->Visible = !lbFaltan->Visible;
		 }

	 private: System::Void tbCard_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent )
			{
				String ^tbText = tbCard->Text->Trim();
				if(tbText->Length == 0) return;
				if(tbText->Length > 22) 
					tbText = tbText->Substring(0, 22);					

				// Copia solo los digitos numericos
				StringBuilder ^pd = gcnew StringBuilder();					
				for (int i = 0; i < tbText->Length; i++)
				{
					if (Char::IsDigit(tbText[i])) // >= '0' && tbText[i] <= '9')
						pd->Append(tbText[i]);
				}
				actualTipre->Nro_Tarjeta = pd->ToString();
				actualTipre->Id = pd->ToString();
				GenValidacion();
			}				
		}

	 private: System::Void btnManual_Click(System::Object^  sender, System::EventArgs^  e) {
			 actualTipre->Modo_Ingreso = "MANUAL"; 	
			 this->tbCard->Enabled = true;
			 this->SetFocusTo(tbCard);
		 }

	 protected: virtual void bEnter_Click(Object ^sender, EventArgs ^e) override
		{
			if(pantalla != 1 || pantalla == 3)
			{
				globalTouchEvent = true;
				MethodInfo ^m = tb->GetType()->GetMethod("OnValidating",
					BindingFlags::NonPublic | BindingFlags::Public | BindingFlags::Instance);
				if(m != nullptr)
				{
					cli::array<Object ^> ^args = gcnew cli::array<Object ^> { gcnew CancelEventArgs() };
					m->Invoke(tb, args);
				}
				globalTouchEvent = false;
			}
			BaseDialog::bEnter_Click(sender, e);
		}


};
}
