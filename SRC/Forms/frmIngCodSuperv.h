#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Summary for frmIngCodSuperv
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmIngCodSuperv : public Controles::BaseDialog
	{
	public:
		System::String ^CodigoIng;

		frmIngCodSuperv(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->txtCodigo->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngCodSuperv::txtSoloNumeros_KeyPress);
			TabControls->Add(txtCodigo);
			Strings::InterceptKeyboard(this, 1);
			this->CodigoIng = "";
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmIngCodSuperv()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  txtCodigo;
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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtCodigo = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			//
			// panel
			//
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->txtCodigo);
			this->panel->Size = System::Drawing::Size(576, 449);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->txtCodigo, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			//
			// cancel
			//
			this->cancel->Location = System::Drawing::Point(396, 368);
			//
			// btOk
			//
			this->btOk->Location = System::Drawing::Point(396, 314);
			//
			// bBorra
			//
			this->bBorra->Location = System::Drawing::Point(452, 230);
			//
			// b0
			//
			this->b0->Location = System::Drawing::Point(396, 230);
			//
			// b2
			//
			this->b2->Location = System::Drawing::Point(452, 178);
			//
			// b1
			//
			this->b1->Location = System::Drawing::Point(396, 178);
			//
			// b5
			//
			this->b5->Location = System::Drawing::Point(452, 126);
			//
			// b4
			//
			this->b4->Location = System::Drawing::Point(396, 126);
			//
			// b8
			//
			this->b8->Location = System::Drawing::Point(452, 74);
			//
			// b7
			//
			this->b7->Location = System::Drawing::Point(396, 74);
			//
			// panel3
			//
			this->panel3->Size = System::Drawing::Size(550, 4);
			//
			// Titulo
			//
			this->Titulo->Size = System::Drawing::Size(317, 23);
			this->Titulo->Text = L"Autorizacion de Usuario";
			//
			// bEnter
			//
			this->bEnter->Location = System::Drawing::Point(508, 230);
			//
			// b3
			//
			this->b3->Location = System::Drawing::Point(508, 178);
			//
			// b6
			//
			this->b6->Location = System::Drawing::Point(508, 126);
			//
			// b9
			//
			this->b9->Location = System::Drawing::Point(508, 74);
			//
			// label1
			//
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(40, 129);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(220, 19);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Codigo de Cajero:";
			//
			// txtCodigo
			//
			this->txtCodigo->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtCodigo->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->txtCodigo->Location = System::Drawing::Point(40, 160);
			this->txtCodigo->MaxLength = 4;
			this->txtCodigo->Name = L"txtCodigo";
			this->txtCodigo->Size = System::Drawing::Size(153, 27);
			this->txtCodigo->TabIndex = 40;
			//
			// frmIngCodSuperv
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(576, 449);
			this->Name = L"frmIngCodSuperv";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"frmIngCodSuperv";
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


		protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		   {
			   if (this->txtCodigo->Text->Trim()->Length == 0)
			   {
				   Alert("Debe ingresar el codigo de cajero", "");
				   return;
			   }
			   this->DialogResult = System::Windows::Forms::DialogResult::OK;
			   this->CodigoIng = this->txtCodigo->Text->Trim();
			   BaseDialog::btOk_Click(sender, e);

		   }

		protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
		   {
			   this->CodigoIng = "";
			   BaseDialog::Cancel_Click(sender, e);

		   }
	};
}
