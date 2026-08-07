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
	/// Summary for frmIngNumParking
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmIngNumParking : public Controles::BaseDialog
	{
	public:
		String ^NumIdParking;

		frmIngNumParking(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			NumIdParking = "";
			this->txtIdParking->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngNumParking::txtSoloNumeros_KeyPress);
			TabControls->Add(txtIdParking);
			Strings::InterceptKeyboard(this, 1);	
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmIngNumParking()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtIdParking;
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
			this->txtIdParking = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->txtIdParking);
			this->panel->Size = System::Drawing::Size(499, 441);
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
			this->panel->Controls->SetChildIndex(this->txtIdParking, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(326, 367);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(326, 313);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(382, 229);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(326, 229);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(382, 177);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(326, 177);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(382, 125);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(326, 125);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(382, 73);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(326, 73);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(480, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(283, 23);
			this->Titulo->Text = L"Ingresar Id Estacionamiento";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(438, 229);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(438, 177);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(438, 125);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(438, 73);
			// 
			// txtIdParking
			// 
			this->txtIdParking->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtIdParking->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtIdParking->Location = System::Drawing::Point(35, 107);
			this->txtIdParking->MaxLength = 6;
			this->txtIdParking->Name = L"txtIdParking";
			this->txtIdParking->Size = System::Drawing::Size(142, 27);
			this->txtIdParking->TabIndex = 40;
			// 
			// frmIngNumParking
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(499, 441);
			this->Name = L"frmIngNumParking";
			this->Text = L"frmIngNumParking";
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		   {

			   this->DialogResult = System::Windows::Forms::DialogResult::OK;
			   this->NumIdParking = this->txtIdParking->Text;
			   BaseDialog::btOk_Click(sender, e);

		   }

	protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
			   {				   				   
				   this->NumIdParking = "";
				   BaseDialog::Cancel_Click(sender, e);

			   }
	};
}
