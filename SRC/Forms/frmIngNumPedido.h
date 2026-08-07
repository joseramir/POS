#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Summary for frmIngNumPedido
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmIngNumPedido : public Controles::BaseDialog
	{
	public:
		String ^NumPedido;

		frmIngNumPedido(bool paramIngNroVdor)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			NumPedido = "";
			if(paramIngNroVdor){
				this->Titulo->Text = "Ingresar numero de Vendedor";
				this->txtNroPedido->MaxLength = 3;
			}
			this->txtNroPedido->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmIngNumPedido::txtSoloNumeros_KeyPress);
			TabControls->Add(txtNroPedido);
			Strings::InterceptKeyboard(this, 1);			
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmIngNumPedido()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtNroPedido;
	protected: 

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
			this->txtNroPedido = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->txtNroPedido);
			this->panel->Size = System::Drawing::Size(499, 441);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->txtNroPedido, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(315, 366);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(315, 312);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(371, 228);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(315, 228);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(371, 176);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(315, 176);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(371, 124);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(315, 124);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(371, 72);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(315, 72);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(480, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(360, 23);
			this->Titulo->Text = L"Ingresar Numero Pedido Ecommerce";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(427, 228);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(427, 176);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(427, 124);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(427, 72);
			// 
			// txtNroPedido
			// 
			this->txtNroPedido->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtNroPedido->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroPedido->Location = System::Drawing::Point(66, 129);
			this->txtNroPedido->MaxLength = 10;
			this->txtNroPedido->Name = L"txtNroPedido";
			this->txtNroPedido->Size = System::Drawing::Size(134, 27);
			this->txtNroPedido->TabIndex = 39;
			// 
			// frmIngNumPedido
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(499, 441);
			this->Name = L"frmIngNumPedido";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"frmIngNumPedido";
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

					   this->DialogResult = System::Windows::Forms::DialogResult::OK;
					   this->NumPedido = this->txtNroPedido->Text;
					   BaseDialog::btOk_Click(sender, e);

				   }

		protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
				   {				   				   
					   this->NumPedido = "";
					   BaseDialog::Cancel_Click(sender, e);

				   }
	
};
}
