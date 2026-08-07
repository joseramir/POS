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
	/// Summary for frmOcPromo
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmOcPromo : public Controles::BaseDialog
	{
	private:
		delegate void InvokeLeerCodBarra(String ^codigoBarra);
		InvokeLeerCodBarra ^SerCodBarraDelegate;
		System::Decimal ValorOc;
		System::Decimal pendiente;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  lPendiente;
	private: System::Windows::Forms::Label^  lTotalOc;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
			 System::String ^mensajerta;	
		

	public:
		frmOcPromo(Decimal ppendiente)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			SerCodBarraDelegate = gcnew InvokeLeerCodBarra(this, &frmOcPromo::DoLeerCodBarraSerial);
			mensajerta = "";
			TabControls->Add(txtNroOc);			
			pendiente = ppendiente;
			Decimal acum = 0;
			for each(Controles::OrdenComodin ^auxorden in Dump::OrdenesCompra)
			{
				ListViewItem ^litem = gcnew ListViewItem();
				litem->Text = auxorden->CodBarra;
				litem->SubItems->Add(auxorden->Valor.ToString());
				lvwOrdenes->Items->Add(litem);
				acum += auxorden->Valor;
			}
			Strings::InterceptKeyboard(this, 1);
			lPendiente->Text = pendiente.ToString("N2");
			lTotalOc->Text = acum.ToString("N2");
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmOcPromo()
		{
			if (components)
			{
				delete components;
			}
		}

		System::Void DoLeerCodBarraSerial(String ^codigoBarra)
		{
			txtNroOc->Text = codigoBarra;
			ResolverOc();
		}

	public:

		void AcceptSerialCodBarra(char *codigoBarra)
		{
			if(this->InvokeRequired)
			{
				cli::array<System::Object^>^ args = gcnew cli::array<System::Object^>(1);
				args[0] = gcnew System::String(codigoBarra);
				this->Invoke(SerCodBarraDelegate, args);
			}
			else
				DoLeerCodBarraSerial(gcnew System::String(codigoBarra));
		}

	private: System::Void ResolverOc(){
				 String ^rtaServ = "";
				 OrdenComodin ^auxorden = nullptr;
				 //if (txtNroOc->Text->Length != 18)
				 if (txtNroOc->Text->Length != 12)
				 {
					 Alert("Codigo erroneo", "Ingresar nuevamente");
					 return;
				 }
				 if (!Escaneada(txtNroOc->Text))
				 {	
					 //Controla comunicacion con el Servidor de la sucursal
					 if (Controles::ConexionRed::comprobarIP(gcnew String(ipServOC), 30))
					 {
						 //hay conexion, envia el codigo de Barra leido
						 rtaServ = Controles::OperarServidor::VerificarOrden(txtNroOc->Text, gcnew String(ipServOC), portOC);
						 array<String ^> ^arrrta = rtaServ->Split(',');
						 if (arrrta[0] == "OK")
						 {
							 ValorOc = Convert::ToDecimal(arrrta[1]);
							 lblValor->Text = arrrta[1];
							 auxorden = gcnew OrdenComodin();
							 auxorden->CodBarra = txtNroOc->Text;
							 auxorden->Valor = System::Convert::ToDecimal(lblValor->Text);
							 auxorden->Mpago = 1;
							 auxorden->OnLine = true;
						 }
						 //Hay comunicacion con el servidor, 
						 else if (rtaServ == "NOK,Error en conexion con servidor")
						 {
							 if (ControlaEnCaja(txtNroOc->Text) > 0) 
							 {
								 auxorden = gcnew OrdenComodin();
								 auxorden->CodBarra = txtNroOc->Text;
								 auxorden->Valor = ControlaEnCaja(txtNroOc->Text);
								 auxorden->Mpago = 1;
								 lblValor->Text = auxorden->Valor.ToString();
							 }								 
						 }
						 else if (arrrta->Length > 1)
							 mensajerta = arrrta[1];

					 }
					 else if (ControlaEnCaja(txtNroOc->Text) > 0) 
					 {
						 auxorden = gcnew OrdenComodin();
						 auxorden->CodBarra = txtNroOc->Text;
						 auxorden->Valor = ControlaEnCaja(txtNroOc->Text);
						 auxorden->Mpago = 1;
						 lblValor->Text = auxorden->Valor.ToString("N2");
					 }
					 if (auxorden != nullptr)
					 {
						 if (pendiente >= auxorden->Valor)
						 {
							 Dump::OrdenesCompra->Add(auxorden);
							 ListViewItem ^litem = gcnew ListViewItem();
							 litem->Text = auxorden->CodBarra;
							 litem->SubItems->Add(auxorden->Valor.ToString());
							 lvwOrdenes->Items->Add(litem);
							 pendiente -= auxorden->Valor;
							 txtNroOc->Text = "";
							 lPendiente->Text = pendiente.ToString("N2");
							 Decimal acum = Convert::ToDecimal(lTotalOc->Text);
							 acum += auxorden->Valor;
							 lTotalOc->Text = acum.ToString("N2");
						 }
						 else
							 Alert("Orden Valida, no se da Vuelto", "Faltante menor al valor de Orden");
					 }
					 else
					 {
						 char *otromens = new char[200];
						 Strings::StringToChar(mensajerta, otromens, 200);
						 Alert(otromens, "");
						 lblValor->Text = "0.00";
						 delete otromens;
					 }
				 }
			 }

			 //realiza el control de una control del numero de Codigo de Barra en la caja
	private: System::Decimal ControlaEnCaja(String ^codigobarra)
			 {
				 Decimal valor = 0;
				 String ^auxcb = "";
				 if (codigobarra->StartsWith("00"))
					 auxcb = codigobarra->Substring(0, 10) + codigobarra->Substring(14, 4) + codigobarra->Substring(10, 4);
				 else if (codigobarra->StartsWith("01"))				 
					 auxcb = "01" + codigobarra[15] + codigobarra[11] + codigobarra[7] + codigobarra[3]+ 
						 codigobarra[2] + codigobarra[17] + codigobarra[14] + codigobarra[10] + 
						 codigobarra[6] + codigobarra[4] + codigobarra[16] + codigobarra[13] + 
						 codigobarra[9] + codigobarra[5] + codigobarra[12] + codigobarra[8];
				 else if (codigobarra->StartsWith("02"))
					auxcb = "02" + codigobarra[15] + codigobarra[11] + codigobarra[7] + codigobarra[3]+ 
						codigobarra[17] + codigobarra[2] + codigobarra[14] + codigobarra[5] + 
						codigobarra[6] + codigobarra[4] + codigobarra[16] + codigobarra[13] + 
						codigobarra[9] + codigobarra[8] + codigobarra[12] + codigobarra[10];				 
				 else
				 {
					 mensajerta = "Orden no valida";
					 return valor;
				 }
				 try
				 {
					 System::DateTime ^fdesde, ^fhasta;
					 fdesde = gcnew DateTime(Int32::Parse(auxcb->Substring(2,4)), Int32::Parse(auxcb->Substring(6, 2)),
						 Int32::Parse(auxcb->Substring(8, 2)));
					 fhasta = gcnew DateTime(Int32::Parse(auxcb->Substring(2,4)), Int32::Parse(auxcb->Substring(10, 2)),
						 Int32::Parse(auxcb->Substring(12, 2)));
					 DateTime ^ahora = DateTime::Now;
					 DateTime ^malimp = gcnew DateTime(2015,5,30);
					 DateTime ^limite = gcnew DateTime(2015,6,14);
					 if (ahora->Date >= fdesde->Date && ahora->Date <= fhasta->Date)
						 valor = 25;
					 else if (fdesde->Date == malimp->Date && fhasta->Date == malimp->Date && ahora->Date <= limite->Date)
						 valor = 25;
					 else
						 mensajerta = "No valida para Hoy";

				 }
				 catch (System::Exception ^e)
				 {
				 	mensajerta = "Orden no valida, ERROR";
				 }
				 return valor;

			 }

	private: System::Boolean Escaneada(String ^codigobarra)
			 {
				 Boolean aux = false;
				 for each(OrdenComodin ^item in Dump::OrdenesCompra)
				 {
					 if (item->CodBarra == codigobarra)
					 {
						 mensajerta = "Orden ya figura en listado";
						 aux = true;
					 }
				 }
				 return aux;
			 }


	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::TextBox^  txtNroOc;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lblValor;
	private: System::Windows::Forms::ListView^  lvwOrdenes;
	private: System::Windows::Forms::ColumnHeader^  ch01;
	private: System::Windows::Forms::ColumnHeader^  ch02;
	private: System::Windows::Forms::Button^  btnAddOrden;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmOcPromo::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->txtNroOc = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->lblValor = (gcnew System::Windows::Forms::Label());
			this->lvwOrdenes = (gcnew System::Windows::Forms::ListView());
			this->ch01 = (gcnew System::Windows::Forms::ColumnHeader());
			this->ch02 = (gcnew System::Windows::Forms::ColumnHeader());
			this->btnAddOrden = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->lPendiente = (gcnew System::Windows::Forms::Label());
			this->lTotalOc = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->lvwOrdenes);
			this->panel->Controls->Add(this->panel1);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->txtNroOc);
			this->panel->Controls->Add(this->lblValor);
			this->panel->Controls->Add(this->btnAddOrden);
			this->panel->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->panel->Size = System::Drawing::Size(547, 438);
			this->panel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &frmOcPromo::panel_Paint);
			this->panel->Controls->SetChildIndex(this->btnAddOrden, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->lblValor, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->txtNroOc, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->panel1, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->lvwOrdenes, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(363, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(363, 317);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(419, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(363, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(419, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(363, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(419, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(363, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(419, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(363, 77);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			this->Icono->Location = System::Drawing::Point(17, 6);
			this->Icono->Size = System::Drawing::Size(46, 44);
			this->Icono->Visible = false;
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(306, 23);
			this->Titulo->Text = L"Voucher de Compra \"Comodin\"";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(475, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(475, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(475, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(475, 77);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(33, 108);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(64, 18);
			this->label1->TabIndex = 39;
			this->label1->Text = L"Codigo:";
			// 
			// txtNroOc
			// 
			this->txtNroOc->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroOc->Location = System::Drawing::Point(110, 105);
			this->txtNroOc->Name = L"txtNroOc";
			this->txtNroOc->Size = System::Drawing::Size(207, 26);
			this->txtNroOc->TabIndex = 0;
			this->txtNroOc->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmOcPromo::txtNroOc_KeyPress);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(33, 150);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(77, 18);
			this->label2->TabIndex = 41;
			this->label2->Text = L"Valor $$:";
			// 
			// lblValor
			// 
			this->lblValor->AutoSize = true;
			this->lblValor->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblValor->Location = System::Drawing::Point(134, 150);
			this->lblValor->Name = L"lblValor";
			this->lblValor->Size = System::Drawing::Size(47, 18);
			this->lblValor->TabIndex = 42;
			this->lblValor->Text = L"0.00 ";
			// 
			// lvwOrdenes
			// 
			this->lvwOrdenes->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) {this->ch01, this->ch02});
			this->lvwOrdenes->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lvwOrdenes->Location = System::Drawing::Point(38, 196);
			this->lvwOrdenes->Name = L"lvwOrdenes";
			this->lvwOrdenes->Size = System::Drawing::Size(279, 137);
			this->lvwOrdenes->TabIndex = 2;
			this->lvwOrdenes->UseCompatibleStateImageBehavior = false;
			this->lvwOrdenes->View = System::Windows::Forms::View::Details;
			// 
			// ch01
			// 
			this->ch01->Text = L"Codigo Barra";
			this->ch01->Width = 197;
			// 
			// ch02
			// 
			this->ch02->Text = L"Valor";
			this->ch02->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->ch02->Width = 76;
			// 
			// btnAddOrden
			// 
			this->btnAddOrden->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAddOrden->Location = System::Drawing::Point(244, 144);
			this->btnAddOrden->Name = L"btnAddOrden";
			this->btnAddOrden->Size = System::Drawing::Size(73, 28);
			this->btnAddOrden->TabIndex = 1;
			this->btnAddOrden->Text = L"Agregar";
			this->btnAddOrden->UseVisualStyleBackColor = true;
			this->btnAddOrden->Click += gcnew System::EventHandler(this, &frmOcPromo::btnAddOrden_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::LightSkyBlue;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->lPendiente);
			this->panel1->Controls->Add(this->lTotalOc);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Location = System::Drawing::Point(96, 339);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(221, 70);
			this->panel1->TabIndex = 43;
			// 
			// lPendiente
			// 
			this->lPendiente->AutoSize = true;
			this->lPendiente->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lPendiente->Location = System::Drawing::Point(121, 41);
			this->lPendiente->Name = L"lPendiente";
			this->lPendiente->Size = System::Drawing::Size(43, 18);
			this->lPendiente->TabIndex = 3;
			this->lPendiente->Text = L"0.00";
			// 
			// lTotalOc
			// 
			this->lTotalOc->AutoSize = true;
			this->lTotalOc->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lTotalOc->Location = System::Drawing::Point(121, 10);
			this->lTotalOc->Name = L"lTotalOc";
			this->lTotalOc->Size = System::Drawing::Size(43, 18);
			this->lTotalOc->TabIndex = 2;
			this->lTotalOc->Text = L"0.00";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(15, 41);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(88, 18);
			this->label4->TabIndex = 1;
			this->label4->Text = L"Pendiente:";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(15, 10);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(74, 18);
			this->label3->TabIndex = 0;
			this->label3->Text = L"Voucher:";
			// 
			// frmOcPromo
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(547, 438);
			this->Name = L"frmOcPromo";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmOcPromo";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnAddOrden_Click(System::Object^  sender, System::EventArgs^  e) {
				 ResolverOc();
			 }

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
			   {
				   
				   this->DialogResult = System::Windows::Forms::DialogResult::OK;
				   BaseDialog::btOk_Click(sender, e);
				   
			   }

	protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
			   {				   				   
				   Dump::OrdenesCompra->Clear();
				   this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				   BaseDialog::Cancel_Click(sender, e);
				   
			   }
   
    private: System::Void txtNroOc_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				 
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
					 if (e->KeyChar == ((Char) 13))
					 {
						 //if (txtNroOc->Text->Length == 18)
							 ResolverOc();
						 /*else
							 Alert("Codigo erroneo", "Ingresar nuevamente");
						 e->Handled = true;*/						 
					 }
				 }
				 else
				 {
					 e->Handled = true;
				 }
		 }
	public: virtual bool ProcessHotKey(Keys key) override
		 {			 
			 if (key == Keys::Enter)
				 return true;
			 return BaseDialog::ProcessHotKey(key);
		 }

private: System::Void panel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		 }
};
}
