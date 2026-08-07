#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;

///#include "AlertForm.h"

namespace pos {

	/// <summary>
	/// Summary for frmNroNotaCredito
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmNroNotaCredito : Controles::BaseDialog
	{
		System::Collections::Generic::List<Controles::MotivoNotaCredito ^> ^lstMotivosNc;
	public:
		frmNroNotaCredito(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			txtNroComp->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmNroNotaCredito::txtSoloNumeros_KeyPress);
			txtPtoVenta->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmNroNotaCredito::txtSoloNumeros_KeyPress);
			Strings::InterceptKeyboard(this, 1);
			TabControls->Add(cboTipoComp);
			TabControls->Add(txtPtoVenta);
			TabControls->Add(this->txtNroComp);
			TabControls->Add(this->mtbFecha);

			this->ExtendedKeyboardEnabled = usaTouch;
			cboTipoComp->SelectedIndex = 0;

			lstMotivosNc = gcnew System::Collections::Generic::List<Controles::MotivoNotaCredito ^>();
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("000", "Sin Elegir"));

			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("001", "Devolución de mercaderia"));
			//lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("002", "Doble facturación"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("003", "Cambio de Producto"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("004", "Falta de stock"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("005", "No corresponde al cliente"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("006", "Mal fichado"));

			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("009", "Por solicitud de Factura"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("010", "Cambio de medio de pago"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("011", "Diferencia de precio"));
			//lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("012", "Reintegro  p-Extra cash –vuelto"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("012", "Por  combos-ofertas  y/o promoción"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("013", "Por  error  en  medio de  pago"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("014", "Por confusión en el precio"));

			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("015", "Por Prueba"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("016", "Por Error de Impresora"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("017", "Por Devoluciones Extracash"));

			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("101", "Mala calidad"));
			//lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("103", "Diferencia de cantidad"));
			lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("104", "Mercadería deteriorada"));
			cboMotivo->DataSource = lstMotivosNc;
			cboMotivo->DisplayMember = "DescripcionMotivo";
			cboMotivo->ValueMember = "CodigoMotivo";
			cboMotivo->SelectedIndex = 0;

			TabControls->Add(cboMotivo);
			motivoActual= gcnew Controles::MotivoNotaCredito("", "");

			//lstMotivosNc->Add(gcnew Controles::MotivoNotaCredito("", "");
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmNroNotaCredito()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label2;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::ComboBox^  cboTipoComp;
	private: System::Windows::Forms::TextBox^  txtNroComp;

	public:
		int pTipoComp;
		int pNroComp;
		int pPtoVta;
		DateTime pFechaComp;
		Controles::MotivoNotaCredito ^motivoActual;

	private: System::Windows::Forms::MaskedTextBox^  mtbFecha;
	public: 
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ComboBox^  cboMotivo;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  txtPtoVenta;
	private: System::Windows::Forms::Label^  label5;




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
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->cboTipoComp = (gcnew System::Windows::Forms::ComboBox());
			this->txtNroComp = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->mtbFecha = (gcnew System::Windows::Forms::MaskedTextBox());
			this->cboMotivo = (gcnew System::Windows::Forms::ComboBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->txtPtoVenta = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->txtPtoVenta);
			this->panel->Controls->Add(this->label5);
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->cboMotivo);
			this->panel->Controls->Add(this->cboTipoComp);
			this->panel->Controls->Add(this->txtNroComp);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->mtbFecha);
			this->panel->Controls->Add(this->label3);
			this->panel->Size = System::Drawing::Size(519, 444);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->mtbFecha, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->txtNroComp, 0);
			this->panel->Controls->SetChildIndex(this->cboTipoComp, 0);
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
			this->panel->Controls->SetChildIndex(this->cboMotivo, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
			this->panel->Controls->SetChildIndex(this->label5, 0);
			this->panel->Controls->SetChildIndex(this->txtPtoVenta, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(338, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(338, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(394, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(338, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(394, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(338, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(394, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(338, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(394, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(338, 77);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(500, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(402, 23);
			this->Titulo->Text = L"Ingresar Comprobante para Nota Credito";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(450, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(450, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(450, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(450, 77);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(37, 99);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(45, 18);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Tipo:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(37, 185);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(72, 18);
			this->label2->TabIndex = 40;
			this->label2->Text = L"Número:";
			// 
			// cboTipoComp
			// 
			this->cboTipoComp->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTipoComp->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboTipoComp->FormattingEnabled = true;
			this->cboTipoComp->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Ticket", L"Factura A", L"Factura B"});
			this->cboTipoComp->Location = System::Drawing::Point(129, 96);
			this->cboTipoComp->Name = L"cboTipoComp";
			this->cboTipoComp->Size = System::Drawing::Size(153, 26);
			this->cboTipoComp->TabIndex = 41;
			this->cboTipoComp->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &frmNroNotaCredito::cboTipoComp_PreviewKeyDown);
			// 
			// txtNroComp
			// 
			this->txtNroComp->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtNroComp->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroComp->Location = System::Drawing::Point(129, 182);
			this->txtNroComp->MaxLength = 8;
			this->txtNroComp->Name = L"txtNroComp";
			this->txtNroComp->Size = System::Drawing::Size(100, 26);
			this->txtNroComp->TabIndex = 42;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(36, 230);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(62, 19);
			this->label3->TabIndex = 43;
			this->label3->Text = L"Fecha:";
			// 
			// mtbFecha
			// 
			this->mtbFecha->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->mtbFecha->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->mtbFecha->Location = System::Drawing::Point(129, 229);
			this->mtbFecha->Mask = L"00/00/0000";
			this->mtbFecha->Name = L"mtbFecha";
			this->mtbFecha->Size = System::Drawing::Size(100, 26);
			this->mtbFecha->TabIndex = 44;
			this->mtbFecha->ValidatingType = System::DateTime::typeid;
			// 
			// cboMotivo
			// 
			this->cboMotivo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboMotivo->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboMotivo->FormattingEnabled = true;
			this->cboMotivo->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Ticket", L"Factura A", L"Factura B"});
			this->cboMotivo->Location = System::Drawing::Point(128, 270);
			this->cboMotivo->Name = L"cboMotivo";
			this->cboMotivo->Size = System::Drawing::Size(204, 26);
			this->cboMotivo->TabIndex = 45;
			this->cboMotivo->SelectedIndexChanged += gcnew System::EventHandler(this, &frmNroNotaCredito::cboMotivo_SelectedIndexChanged);
			this->cboMotivo->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &frmNroNotaCredito::cboMotivo_PreviewKeyDown);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(37, 278);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(64, 18);
			this->label4->TabIndex = 46;
			this->label4->Text = L"Motivo:";
			// 
			// txtPtoVenta
			// 
			this->txtPtoVenta->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtPtoVenta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtPtoVenta->Location = System::Drawing::Point(129, 138);
			this->txtPtoVenta->MaxLength = 5;
			this->txtPtoVenta->Name = L"txtPtoVenta";
			this->txtPtoVenta->Size = System::Drawing::Size(100, 26);
			this->txtPtoVenta->TabIndex = 48;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(37, 141);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(90, 18);
			this->label5->TabIndex = 47;
			this->label5->Text = L"Pto. Venta:";
			// 
			// frmNroNotaCredito
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(519, 444);
			this->Name = L"frmNroNotaCredito";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmNroNotaCredito";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		private: System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
				 {
					 if (Char::IsLetter(e->KeyChar))				 
						 e->Handled = true;				 
					 else if (Char::IsDigit(e->KeyChar))				 
						 e->Handled = false;				 
					 else if (Char::IsControl(e->KeyChar))				 
						 e->Handled = false;				 
					 else				 
						 e->Handled = true;				 
				 }

	   protected: void virtual btOk_Click(System::Object^  sender, System::EventArgs^  e) override{
					  
					  System::DateTime fechaIng = (System::DateTime::Now).AddYears(-1);
					  bool daerror = false;
					  try{
						  fechaIng = DateTime::ParseExact(mtbFecha->Text, "dd/MM/yyyy", nullptr);
					  }catch(Exception ^exc)
					  {
						  daerror = true;
					  }
					  if (daerror)
					  {
						  Alert("corregir la fecha ingresada", "");
						  return;
					  }
					  if (cboTipoComp->SelectedIndex == 1){
						  Alert("FACTURAS A ", "OPCION NO HABILITADA");
						  return ;
					  }
					  if (fechaIng < DateTime::Now)
					  {
						  TimeSpan tdif =  (DateTime::Now - fechaIng);
						  if (tdif.TotalDays <= 7)
						  {
							  pTipoComp = cboTipoComp->SelectedIndex;
							  //pNroComp = System::Convert::ToInt32(txtNroComp->Text);
							  System::Int32::TryParse(txtNroComp->Text, pNroComp);
							  System::Int32::TryParse(txtPtoVenta->Text, pPtoVta);

							  pFechaComp = DateTime::ParseExact(mtbFecha->Text, "dd/MM/yyyy", nullptr);
							  if (motivoActual != nullptr && motivoActual->CodigoMotivo != "" && motivoActual->CodigoMotivo != "000"){
								  Strings::StringToChar(("Motivo NC: " + motivoActual->DescripcionMotivo), motivoNotaCredito, 51);								  
								  sprintf(supervNotaCredito, "SUPERVISOR: %s", supervName());
								  //int respuesta = GetSiNo("Los datos ingresados son correctos", "Desea continuar?");
								  //if (respuesta == 1){
									  BaseDialog::btOk_Click(sender, e);
									  this->DialogResult = System::Windows::Forms::DialogResult::OK;	
								  /*}
								  else
									  return;*/
							  }
							  else
								  Alert("Tiene que seleccionar un MOTIVO", "para la Nota de Credito");
						  }
						  else
							  Alert("Fecha superior a 7 dias", "Ingresar otra fecha");
					  }
					  else
						  Alert("Verificar la fecha ingresada", "tiene que ser de hoy o anterior");

					  
				  }	
	   
	  public: virtual bool ProcessHotKey(Keys key) override
			  {
				  if (key == Keys::Down)
				  {
					  if (this->tb->Name == "cboTipoComp")
					  {
						  Strings::DisableKey(this);
						  cboTipoComp->DroppedDown = true;
						  return true;						 
					  }else
					  if (this->tb->Name == "cboMotivo")
					  {
						Strings::DisableKey(this);
						cboMotivo->DroppedDown = true;
						return true;						 
					  }
				  }
				  
				  return BaseDialog::ProcessHotKey(key);
			  }

	private: System::Void cboTipoComp_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
				 if (e->KeyCode == Keys::Enter)
				 {
					 cboTipoComp->DroppedDown = false;
					 Strings::EnableKey(this);
					 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));					
				 }
			 }
private: System::Void cboMotivo_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 if (e->KeyCode == Keys::Enter)
			 {
				 cboMotivo->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));					
			 }
		 }
private: System::Void cboMotivo_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 motivoActual = (Controles::MotivoNotaCredito ^) cboMotivo->SelectedItem;
		 }
};
}
