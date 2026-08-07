#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace System::IO;
using namespace System::Xml;

#include "STDAFX.H"

namespace pos {

	

	/// <summary>
	/// Summary for frmSelColegio
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmSelColegio : public Controles::BaseDialog
	{
	public:
		String ^codigosel;
	private: System::Windows::Forms::Timer^  tmrSalir;
	public: 
		List<Colegio ^> ^lstColegios;
		frmSelColegio(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			TabControls->Add(txtCodColegio);
			Strings::InterceptKeyboard(this, 1);
			codigosel = "0";
			lstColegios = gcnew List<Colegio ^>();
			String ^nombrearchivo = Application::StartupPath + "\\pos\\colegios.xml";
			bool continuar = true;
			continuar = System::IO::File::Exists(nombrearchivo);
			try
			{					
				XmlReader ^xmlReader = XmlReader::Create(nombrearchivo);
				while (xmlReader->Read())
				{
					if ((xmlReader->NodeType == XmlNodeType::Element) && (xmlReader->Name == "colegio"))
					{
						Colegio ^newcol = gcnew Colegio();

						newcol->Codigo = xmlReader->GetAttribute("codigo");
						newcol->Nombre = xmlReader->GetAttribute("nombre");
						lstColegios->Add(newcol);
					}				
				}
				xmlReader->Close();				
			}
			catch(System::Exception ^exc)
			{
				continuar = false;
			}
			finally
			{
				if (!continuar)
					tmrSalir->Enabled = true;
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmSelColegio()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::Label^  lblMensaje;
	private: System::Windows::Forms::TextBox^  txtCodColegio;
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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtCodColegio = (gcnew System::Windows::Forms::TextBox());
			this->lblMensaje = (gcnew System::Windows::Forms::Label());
			this->tmrSalir = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->lblMensaje);
			this->panel->Controls->Add(this->txtCodColegio);
			this->panel->Controls->Add(this->label1);
			this->panel->Size = System::Drawing::Size(560, 435);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->txtCodColegio, 0);
			this->panel->Controls->SetChildIndex(this->lblMensaje, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(371, 366);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(371, 312);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(427, 228);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(371, 228);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(427, 176);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(371, 176);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(427, 124);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(371, 124);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(427, 72);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(371, 72);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(354, 23);
			this->Titulo->Text = L"Ingresar Codigo de Escuela/Colegio";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(483, 228);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(483, 176);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(483, 124);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(483, 72);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(63, 108);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(64, 18);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Codigo:";
			// 
			// txtCodColegio
			// 
			this->txtCodColegio->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtCodColegio->Location = System::Drawing::Point(133, 104);
			this->txtCodColegio->MaxLength = 6;
			this->txtCodColegio->Name = L"txtCodColegio";
			this->txtCodColegio->Size = System::Drawing::Size(118, 26);
			this->txtCodColegio->TabIndex = 40;
			this->txtCodColegio->TextChanged += gcnew System::EventHandler(this, &frmSelColegio::txtCodColegio_TextChanged);
			this->txtCodColegio->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmSelColegio::txtCodColegio_KeyPress);
			// 
			// lblMensaje
			// 
			this->lblMensaje->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMensaje->Location = System::Drawing::Point(30, 149);
			this->lblMensaje->Name = L"lblMensaje";
			this->lblMensaje->Size = System::Drawing::Size(323, 80);
			this->lblMensaje->TabIndex = 41;
			// 
			// tmrSalir
			// 
			this->tmrSalir->Interval = 500;
			this->tmrSalir->Tick += gcnew System::EventHandler(this, &frmSelColegio::tmrSalir_Tick);
			// 
			// frmSelColegio
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(560, 435);
			this->Name = L"frmSelColegio";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmSelColegio";
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void txtCodColegio_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
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

			 Colegio^ BuscarColegio(String ^codigo)
			 {
				 Colegio^ uncol = nullptr;
				 for (int i = 0; i < lstColegios->Count; i++)
				 {
					 if (codigo == lstColegios[i]->Codigo)
					 {
						 uncol = lstColegios[i];
						 break;
					 }
				 }
				 return uncol;
			 }

    protected: void virtual btOk_Click(System::Object^  sender, System::EventArgs^  e) override{
				   Decimal auxVuelto = 0;
				 //  Decimal::TryParse(txtMonto->Text, auxVuelto);
				 //  if (auxVuelto > 0 && auxVuelto <= maxVuelto)
				 //  {
					//   vueltoIngre = auxVuelto;
					//   unaDonac->importe = System::Convert::ToDouble(vueltoIngre);
					//   /*Controles::TVuelto^ auxtvuelto = (Controles::TVuelto^) cbVuelto->SelectedItem;
					//   unaDonac->codvuelto = System::Convert::ToInt32(auxtvuelto->Codigo);

					//   Strings::StringToChar(txtApellido->Text, unaDonac->apellido, 30);
					//   Strings::StringToChar(txtNombre->Text, unaDonac->nombre, 30);*/
					//   
					//   Strings::StringToChar(txtDni->Text, unaDonac->dni, 20);
					//   Strings::StringToChar(txtTelArea->Text, unaDonac->codarea, 10);
					//   Strings::StringToChar(txtTelNum->Text, unaDonac->telefono, 20);					   
					//   BaseDialog::btOk_Click(sender, e);
					//   this->DialogResult = System::Windows::Forms::DialogResult::OK;					   
				 //  }
				 //  else
					//Alert("Importe de vuelto incorrecto", "");
				   //codigosel = "0";
				   BaseDialog::btOk_Click(sender, e);
				   this->DialogResult = System::Windows::Forms::DialogResult::OK;	
			 }	

	 protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
		   {
			   /*unaDonac->Reset();
			   this->DialogResult = System::Windows::Forms::DialogResult::Cancel;*/
			   codigosel = "0";
			   BaseDialog::Cancel_Click(sender, e);
		   }
private: System::Void txtCodColegio_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 this->codigosel = "0";
			 lblMensaje->Text = "";
			 if (txtCodColegio->Text->Length >= 5)
			 {
				 Colegio ^uncol = this->BuscarColegio(txtCodColegio->Text);
				 if (uncol != nullptr)
				 {
					 lblMensaje->Text = uncol->Nombre;
					 this->codigosel = uncol->Codigo;

				 }				 
			 }
		 }
private: System::Void tmrSalir_Tick(System::Object^  sender, System::EventArgs^  e) {
			 this->Close();
		 }
};

}
