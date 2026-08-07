#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;
using namespace Alberdi::TJOCommon::VerifoneCommon;

using namespace Alberdi::TJOCommon::ComunicaMercadoPago;

#include "Funcs.h"
#include "tjonline.h"
#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for frmAnulaSmartPoint
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmAnulaSmartPoint : public Controles::BaseDialog
	{
	public:
		frmAnulaSmartPoint(int cual)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			this->TabControls->Add(txtExternalReference);			
			
			this->ExtendedKeyboardEnabled = usaTouch;

			//this->txtExternalReference->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmAnulaSmartPoint::txtSoloNumeros_KeyPress);			
			this->txtExternalReference->GotFocus += gcnew System::EventHandler(this, &frmAnulaSmartPoint::TbGotFocus);
			ExternalReference = "";
		}

	String ^ExternalReference;

	protected:
		bool inGotFocus;
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmAnulaSmartPoint()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::TextBox^  txtExternalReference;

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
			this->txtExternalReference = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(94)), 
				static_cast<System::Int32>(static_cast<System::Byte>(136)));
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->txtExternalReference);
			this->panel->Size = System::Drawing::Size(656, 458);
			this->panel->Controls->SetChildIndex(this->txtExternalReference, 0);
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
			this->Titulo->Size = System::Drawing::Size(301, 23);
			this->Titulo->Text = L"Anula Operaciones SmartPoint";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(21, 160);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(31, 18);
			this->label1->TabIndex = 39;
			this->label1->Text = L"ID:";
			// 
			// txtExternalReference
			// 
			this->txtExternalReference->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtExternalReference->Location = System::Drawing::Point(87, 157);
			this->txtExternalReference->Name = L"txtExternalReference";
			this->txtExternalReference->Size = System::Drawing::Size(174, 26);
			this->txtExternalReference->TabIndex = 40;
			// 
			// frmAnulaSmartPoint
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(656, 458);
			this->Name = L"frmAnulaSmartPoint";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmAnulaSmartPoint";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	protected: void TbGotFocus(Object ^sender, EventArgs ^e)
		   {
			   if(inGotFocus) return;
			   inGotFocus = true;

			   Control ^ctl = dynamic_cast<Control ^>(sender);
			   BaseDialog::SetFocusTo(ctl);

			   inGotFocus = false;
		   }
   protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		  {
			  if (String::IsNullOrEmpty(txtExternalReference->Text)){
				  Alert("Falta ingresar el Numero de Orden", "");
				  return;
			  }
			  ExternalReference = txtExternalReference->Text;
			  BaseDialog::btOk_Click(sender, e);
			  WriteBootLog("AVISO - Saliendo a pantalla de Anula Cupones SMARTPOINT con OK");

		  }
  protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
		 {
			 WriteBootLog("AVISO - Saliendo a pantalla de Anula Cupones SMARTPOINT con CANCEL");			
		 }
	};
}
