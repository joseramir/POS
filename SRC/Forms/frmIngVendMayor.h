#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "StaticDump.h"

namespace pos {

	/// <summary>
	/// Summary for frmIngVendMayor
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmIngVendMayor : public Controles::BaseDialog
	{
	public:
		String ^NroVendMayor;
		frmIngVendMayor(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			NroVendMayor = "";			
			
			if (Dump::ListadoVendedoresMay->Count == 0){
				Dump::ListadoVendedoresMay = Controles::FuncVendedorMay::BuscaVendedores();
			}
			if (Dump::ListadoVendedoresMay->Count == 0){
				Alert("No se pudo conseguir listado de vendedores", "");
				tmrFin->Enabled = true;
			}
			cboNroVendMay->DisplayMember = "Nombre";
			cboNroVendMay->ValueMember = "IdVendedor";
			cboNroVendMay->DataSource = Dump::ListadoVendedoresMay;
			cboNroVendMay->SelectedIndex = 0;
			TabControls->Add(cboNroVendMay);
			Strings::InterceptKeyboard(this, 1);	
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmIngVendMayor()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ComboBox^  cboNroVendMay;
	private: System::Windows::Forms::Timer^  tmrFin;
	private: System::ComponentModel::IContainer^  components;
	protected: 

	protected: 

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
			this->cboNroVendMay = (gcnew System::Windows::Forms::ComboBox());
			this->tmrFin = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->cboNroVendMay);
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
			this->panel->Controls->SetChildIndex(this->cboNroVendMay, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(323, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(323, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(379, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(323, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(379, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(323, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(379, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(323, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(379, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(323, 77);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(480, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(239, 23);
			this->Titulo->Text = L"Seleccionar el Vendedor";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(435, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(435, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(435, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(435, 77);
			// 
			// cboNroVendMay
			// 
			this->cboNroVendMay->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboNroVendMay->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboNroVendMay->FormattingEnabled = true;
			this->cboNroVendMay->Location = System::Drawing::Point(41, 197);
			this->cboNroVendMay->Name = L"cboNroVendMay";
			this->cboNroVendMay->Size = System::Drawing::Size(260, 27);
			this->cboNroVendMay->TabIndex = 41;
			this->cboNroVendMay->SelectedIndexChanged += gcnew System::EventHandler(this, &frmIngVendMayor::cboNroVendMay_SelectedIndexChanged);
			this->cboNroVendMay->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &frmIngVendMayor::cboNroVendMay_PreviewKeyDown);
			// 
			// tmrFin
			// 
			this->tmrFin->Tick += gcnew System::EventHandler(this, &frmIngVendMayor::tmrFin_Tick);
			// 
			// frmIngVendMayor
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->ClientSize = System::Drawing::Size(499, 441);
			this->Name = L"frmIngVendMayor";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		
		protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
				   {
					   Controles::VendedorMayor ^vndedor = (Controles::VendedorMayor^) cboNroVendMay->SelectedItem;
					   if (vndedor->IdVendedor > 0){
						   this->NroVendMayor = vndedor->IdVendedor == 0 ? "" : System::Convert::ToString(vndedor->IdVendedor);

						   this->DialogResult = System::Windows::Forms::DialogResult::OK;
						   BaseDialog::btOk_Click(sender, e);
					   }else
						   Alert("Seleccione un Vendedor del listado", "");

				   }

		protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
				   {				   				   
					   this->NroVendMayor = "";
					   BaseDialog::Cancel_Click(sender, e);

				   }
	private: System::Void tmrFin_Tick(System::Object^  sender, System::EventArgs^  e) {
				 this->NroVendMayor = "";
				 this->DialogResult	= System::Windows::Forms::DialogResult::Cancel;
				 this->Close();
			 }
private: System::Void cboNroVendMay_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 if (e->KeyCode == Keys::Enter)
			 {
				 cboNroVendMay->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
				 /*if (indexsel != cboNroVendMay->SelectedIndex)
				 {
					 indexsel = cboNroVendMay->SelectedIndex;
					 txtCuotas_TextChanged(sender, gcnew System::EventArgs());
				 }*/
			 }
		 }
private: System::Void cboNroVendMay_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

public: virtual bool ProcessHotKey(Keys key) override
			 {
				 if (key == Keys::Down)
				 {
					 if (this->tb->Name == "cboNroVendMay")
					 {
						 Strings::DisableKey(this);
						 cboNroVendMay->DroppedDown = true;
						 return true;						 
					 }	

					 /*if (this->tb->Name == "cboQrModo")
					 {
						 Strings::DisableKey(this);
						 cboQrModo->DroppedDown = true;
						 return true;						 
					 }				*/	 
				 }
				 return BaseDialog::ProcessHotKey(key);
			 }
};
}
