#pragma once

#include "StaticDump.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;

namespace pos {

	/// <summary>
	/// Summary for GetPromoLine
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class GetPromoLine : public Controles::BaseDialog
	{
	public:
		GetPromoLine(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			this->txtIngCod->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &GetPromoLine::txtSoloNumeros_KeyPress);
			TabControls->Add(txtIngCod);	
			this->ExtendedKeyboardEnabled = usaTouch;
			Strings::InterceptKeyboard(this, 1);
			if (Dump::promosfiel != nullptr)
			{
				for each(LeyenPromo ^item in Dump::lstLeyenPromo)
				{
					ListViewItem ^litem = gcnew ListViewItem(item->CodPromo);
					litem->SubItems->Add(item->Leyenda);
					lvwListado->Items->Add(litem);
				}
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GetPromoLine()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtIngCod;
	protected: 
	private: System::Windows::Forms::ListView^  lvwListado;
	private: System::Windows::Forms::ColumnHeader^  columnHeader1;
	private: System::Windows::Forms::ColumnHeader^  columnHeader2;
	private: System::Windows::Forms::Button^  btnAgregar;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GetPromoLine::typeid));
			this->txtIngCod = (gcnew System::Windows::Forms::TextBox());
			this->lvwListado = (gcnew System::Windows::Forms::ListView());
			this->columnHeader1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->columnHeader2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->btnAgregar = (gcnew System::Windows::Forms::Button());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->lvwListado);
			this->panel->Controls->Add(this->txtIngCod);
			this->panel->Controls->Add(this->btnAgregar);
			this->panel->Size = System::Drawing::Size(645, 440);
			this->panel->Controls->SetChildIndex(this->btnAgregar, 0);
			this->panel->Controls->SetChildIndex(this->txtIngCod, 0);
			this->panel->Controls->SetChildIndex(this->lvwListado, 0);
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
			this->Titulo->Size = System::Drawing::Size(298, 23);
			this->Titulo->Text = L"Ingresa el Código del Voucher";
			// 
			// txtIngCod
			// 
			this->txtIngCod->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtIngCod->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtIngCod->Location = System::Drawing::Point(25, 88);
			this->txtIngCod->MaxLength = 12;
			this->txtIngCod->Name = L"txtIngCod";
			this->txtIngCod->Size = System::Drawing::Size(157, 26);
			this->txtIngCod->TabIndex = 39;
			// 
			// lvwListado
			// 
			this->lvwListado->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->columnHeader1, this->columnHeader2});
			this->lvwListado->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lvwListado->GridLines = true;
			this->lvwListado->Location = System::Drawing::Point(25, 137);
			this->lvwListado->Name = L"lvwListado";
			this->lvwListado->Size = System::Drawing::Size(426, 233);
			this->lvwListado->TabIndex = 40;
			this->lvwListado->UseCompatibleStateImageBehavior = false;
			this->lvwListado->View = System::Windows::Forms::View::Details;
			// 
			// columnHeader1
			// 
			this->columnHeader1->Text = L"Codigo";
			this->columnHeader1->Width = 140;
			// 
			// columnHeader2
			// 
			this->columnHeader2->Text = L"Descripción";
			this->columnHeader2->Width = 280;
			// 
			// btnAgregar
			// 
			this->btnAgregar->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAgregar->Location = System::Drawing::Point(204, 87);
			this->btnAgregar->Name = L"btnAgregar";
			this->btnAgregar->Size = System::Drawing::Size(92, 30);
			this->btnAgregar->TabIndex = 41;
			this->btnAgregar->Text = L"Agregar";
			this->btnAgregar->UseVisualStyleBackColor = true;
			this->btnAgregar->Click += gcnew System::EventHandler(this, &GetPromoLine::btnAgregar_Click);
			// 
			// GetPromoLine
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(645, 440);
			this->Name = L"GetPromoLine";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GetPromoLine";
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
				if (Convert::ToUInt32(e->KeyChar) == 13)
					ConsultarCodigo();
				else
					e->Handled = false;				
			}
			else
			{
				e->Handled = true;
			}
		}

		System::Void ConsultarCodigo()
		{
			String ^leyenda = "";
			PromoExtra ^auxpromo = nullptr;
			if (txtIngCod->Text->Length != 12)
			{
				Alert("Codigo erroneo", "Ingresar nuevamente");
				txtIngCod->Focus();
				txtIngCod->SelectAll();
				return;
			}
			
			//Controla comunicacion con el Servidor de la sucursal
			if (Controles::ConexionRed::comprobarIP(gcnew String(ipServOC), 30))
			{
				//hay conexion, envia el codigo de Barra leido
				//faltaprinter
				leyenda = "";
				System::Int64 vaCod = 0;
				System::Int64::TryParse(gcnew String(c_cod), vaCod);
				if (vaCod == 0)
				{
					Alert("No se ha iniciado un ticket Factura", "");
					return;
				}
				//Controles::OperarServidor::ObtenerPromoGen33(gcnew String(ipServOC), portOC, txtIngCod->Text, caja, ckaNro, pf->GetTickNro(GlobalTF), Convert::ToInt32(gcnew String(c_cod)), auxpromo, leyenda);
				Controles::OperarServidor::ObtenerPromoGen33(gcnew String(ipServOC), portOC, txtIngCod->Text, caja, ckaNro, pf->GetTickNro(GlobalTF), vaCod, auxpromo, leyenda);
				if (auxpromo != nullptr)
				{
					ListViewItem ^litem = gcnew ListViewItem(txtIngCod->Text);
					litem->SubItems->Add(leyenda);
					lvwListado->Items->Add(litem);
					if (Dump::promosfiel == nullptr)
						Dump::promosfiel = gcnew List<Controles::PromoExtra ^>();
					Dump::promosfiel->Add(auxpromo);

					if (Dump::lstLeyenPromo == nullptr)
						Dump::lstLeyenPromo = gcnew List<Controles::LeyenPromo ^>();
					Dump::lstLeyenPromo->Add(gcnew LeyenPromo(txtIngCod->Text, leyenda));
					txtIngCod->Text = "";
					txtIngCod->Focus();
				}
				else
					if (leyenda != "")
					{
						char mimsj[200];
						Strings::StringToChar(leyenda, mimsj, 40);
						Alert("Mensaje de Error", mimsj);
					}
					else					
						Alert("Codigo no existe o ya fue usado","sin comunicacion con el Servicio");			
					//Alert("Verificar el codigo ingresado", "Codigo no existe o ya fue usado");			
			}
			else
				Alert("Sin comunicacion con el servidor", "");
			
		}


	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
			   {
				   this->DialogResult = System::Windows::Forms::DialogResult::OK;				   
				   BaseDialog::btOk_Click(sender, e);
			   }

	protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
			   {				   				   
				   //Dump::Billetes->Clear();
				   this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				   BaseDialog::Cancel_Click(sender, e);
			   }
	
	protected: virtual System::Void bEnter_Click(System::Object^  sender, System::EventArgs^  e) override{
				 ConsultarCodigo();
			 }
	private: System::Void btnAgregar_Click(System::Object^  sender, System::EventArgs^  e) {
				 ConsultarCodigo();
			 }
};
}
