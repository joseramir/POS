#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


using namespace Controles;

using namespace Alberdi::TJOCommon::VerifoneCommon;
using namespace Alberdi::TJOCommon::PrismaECR;

//#include "DatosMpagos.h"
#include "Funcs.h"
#include "tjonline.h"
//#include "frmCupones.h"
#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for frmPrismaECR
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmPrismaECR : public Controles::BaseDialog
	{
	public:
		frmPrismaECR(Decimal ptotal)
		{
			struct mpag_ *p;
			InitializeComponent();
			actuTarjVerifonePrisma = gcnew TransPrismaECR();

			this->TabControls->Add(this->cboTarjetas);
			this->TabControls->Add(txtMonto);			
			this->TabControls->Add(txtCuotas);							
			this->TabControls->Add(txtExtracash);
			//this->TabControls->Add(cboQrModo);

			Strings::InterceptKeyboard(this, 1);			
			this->nuevolapos = true;
			this->total = ptotal;
			this->recibido = 0;
			this->faltante = ptotal;

			this->ExtendedKeyboardEnabled = usaTouch;						
			this->txtMonto->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmPrismaECR::txtMonto_KeyPress);			
			this->txtExtracash->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmPrismaECR::txtMonto_KeyPress);			

			this->txtCuotas->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &frmPrismaECR::txtSoloNumeros_KeyPress);			
			this->txtMonto->GotFocus += gcnew System::EventHandler(this, &frmPrismaECR::TbGotFocus);
			this->txtCuotas->GotFocus += gcnew System::EventHandler(this, &frmPrismaECR::TbGotFocus);			
			this->txtExtracash->GotFocus += gcnew System::EventHandler(this, &frmPrismaECR::TbGotFocus);			

			LlenarLvw();				
			if ((Decimal)this->total <= Decimal(0)) 
				this->faltante = ((Decimal)this->total == Decimal(0)) ? Decimal(0) : (Decimal) this->total - (Decimal) this->recibido;

			if (!tjOnline && defTarjetas.Count() <= 0)
				CargaTjOnline();

			lstTarjPrisma = gcnew List<ShowTarjeta^>();
			ShowTarjeta ^obj0 = gcnew ShowTarjeta();
			obj0->NombreTarjeta = "Seleccione una Tarjeta";
			obj0->CodMpagoAlbe = 0;
			obj0->CodPlanPrisma = "0";
			obj0->TipoTarje = 'C';
			obj0->LongAuto = 0;
			lstTarjPrisma->Add(obj0);
			for(int k = 0; k < defTarjetas.Count(); k++)
			{
				TarjDef *def = (TarjDef *) defTarjetas.Item(k);
				p = BuscarMpag(def->MPago);
				ShowTarjeta^actprisma = gcnew ShowTarjeta();
				actprisma->CodMpagoAlbe = def->MPago;
				actprisma->NombreTarjeta = gcnew String(def->Nombre);
				actprisma->CodPlanPrisma = System::Convert::ToString(def->CodPlanPrisma);
				actprisma->CodTarjetaPrisma = gcnew String(def->CodTarjPrisma);
				actprisma->CodTarjetaAlbe = def->Codigo;
				actprisma->NroComercio = gcnew String(def->NumComercio);
				actprisma->TipoTarje = def->tipo == 'C' ? 'C' : 'D';
				actprisma->LongAuto = def->longaut;
				if (actprisma->CodTarjetaPrisma->Trim() != "")
					lstTarjPrisma->Add(actprisma);

			}		
			lblMensaje->Visible = false;

			cboTarjetas->DataSource = lstTarjPrisma;
			cboTarjetas->DisplayMember = "NombreTarjeta";
			cboTarjetas->ValueMember = "CodTarjetaPrisma";

			CantTotalTarjetas = System::Convert::ToUInt16(System::Configuration::ConfigurationManager::AppSettings["tiprecanttarj"]);

			indextar0 = cboTarjetas->Items->IndexOf(obj0);
			indexsel = indextar0;
			cboTarjetas->SelectedIndex = indextar0;
			lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");
			NuevoCupon();		

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmPrismaECR()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lblMensaje;
	protected: 
	private: System::Windows::Forms::TextBox^  txtExtracash;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  txtCuotas;
	private: System::Windows::Forms::Label^  lblRecargo;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lblCuotas;
	private: System::Windows::Forms::Label^  lblMonto;
	private: System::Windows::Forms::Label^  lblTarjeta;
	private: System::Windows::Forms::ComboBox^  cboTarjetas;
	private: System::Windows::Forms::TextBox^  txtMonto;
	private: System::Windows::Forms::ListView^  lvw;
	private: System::Windows::Forms::ColumnHeader^  clh1;
	private: System::Windows::Forms::ColumnHeader^  clh2;
	private: System::Windows::Forms::ColumnHeader^  clh3;
	private: System::Windows::Forms::ColumnHeader^  clh4;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btnAdd;
	private: System::Windows::Forms::Button^  btnDot;
	private: System::Windows::Forms::Label^  lblFaltante;

			 List<PromoMedioPago ^> ^actPromoMp;
			 int CantTotalTarjetas;

	protected:

		bool nuevolapos;
		System::Decimal total;
		System::Decimal recibido;
		bool escredito;
		bool inGotFocus;

		int indextar0;		//Indice de 'Otra Tarjeta'		
		int indexsel; 

		double frecargo;
		double ftasarecargo;
		bool calculandomonto;
		int uindex;

		List<ShowTarjeta^> ^lstTarjPrisma;
		TransPrismaECR^ actuTarjVerifonePrisma;

		System::Decimal faltante;
private: System::Windows::Forms::Timer^  tmrMensaje;
protected: 
private: System::Windows::Forms::Timer^  tmrFinalizar;
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
			this->lblMensaje = (gcnew System::Windows::Forms::Label());
			this->txtExtracash = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->txtCuotas = (gcnew System::Windows::Forms::TextBox());
			this->lblRecargo = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->lblCuotas = (gcnew System::Windows::Forms::Label());
			this->lblMonto = (gcnew System::Windows::Forms::Label());
			this->lblTarjeta = (gcnew System::Windows::Forms::Label());
			this->cboTarjetas = (gcnew System::Windows::Forms::ComboBox());
			this->txtMonto = (gcnew System::Windows::Forms::TextBox());
			this->lvw = (gcnew System::Windows::Forms::ListView());
			this->clh1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh3 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh4 = (gcnew System::Windows::Forms::ColumnHeader());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnAdd = (gcnew System::Windows::Forms::Button());
			this->btnDot = (gcnew System::Windows::Forms::Button());
			this->lblFaltante = (gcnew System::Windows::Forms::Label());
			this->tmrMensaje = (gcnew System::Windows::Forms::Timer(this->components));
			this->tmrFinalizar = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->btnDot);
			this->panel->Controls->Add(this->lblFaltante);
			this->panel->Controls->Add(this->lblMensaje);
			this->panel->Controls->Add(this->txtExtracash);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->txtCuotas);
			this->panel->Controls->Add(this->lblRecargo);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->lblCuotas);
			this->panel->Controls->Add(this->lblMonto);
			this->panel->Controls->Add(this->lblTarjeta);
			this->panel->Controls->Add(this->cboTarjetas);
			this->panel->Controls->Add(this->txtMonto);
			this->panel->Controls->Add(this->lvw);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->btnAdd);
			this->panel->Size = System::Drawing::Size(648, 567);
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
			this->panel->Controls->SetChildIndex(this->btnAdd, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->lvw, 0);
			this->panel->Controls->SetChildIndex(this->txtMonto, 0);
			this->panel->Controls->SetChildIndex(this->cboTarjetas, 0);
			this->panel->Controls->SetChildIndex(this->lblTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->lblMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->lblRecargo, 0);
			this->panel->Controls->SetChildIndex(this->txtCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->txtExtracash, 0);
			this->panel->Controls->SetChildIndex(this->lblMensaje, 0);
			this->panel->Controls->SetChildIndex(this->lblFaltante, 0);
			this->panel->Controls->SetChildIndex(this->btnDot, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 473);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 419);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(636, 4);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(290, 23);
			this->Titulo->Text = L"Autoriza Tarjetas Prisma ECR";
			// 
			// lblMensaje
			// 
			this->lblMensaje->AutoSize = true;
			this->lblMensaje->BackColor = System::Drawing::Color::White;
			this->lblMensaje->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMensaje->ForeColor = System::Drawing::Color::Black;
			this->lblMensaje->Location = System::Drawing::Point(16, 79);
			this->lblMensaje->Name = L"lblMensaje";
			this->lblMensaje->Size = System::Drawing::Size(122, 18);
			this->lblMensaje->TabIndex = 74;
			this->lblMensaje->Text = L"¡¡Faltan Datos!!";
			// 
			// txtExtracash
			// 
			this->txtExtracash->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtExtracash->Location = System::Drawing::Point(158, 285);
			this->txtExtracash->MaxLength = 10;
			this->txtExtracash->Name = L"txtExtracash";
			this->txtExtracash->Size = System::Drawing::Size(125, 26);
			this->txtExtracash->TabIndex = 73;
			this->txtExtracash->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtExtracash->TextChanged += gcnew System::EventHandler(this, &frmPrismaECR::txtCuotas_TextChanged);
			// 
			// label3
			// 
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(21, 287);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(125, 19);
			this->label3->TabIndex = 72;
			this->label3->Text = L"Extracash $$:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtCuotas
			// 
			this->txtCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtCuotas->Location = System::Drawing::Point(159, 253);
			this->txtCuotas->MaxLength = 2;
			this->txtCuotas->Name = L"txtCuotas";
			this->txtCuotas->Size = System::Drawing::Size(46, 26);
			this->txtCuotas->TabIndex = 64;
			this->txtCuotas->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtCuotas->TextChanged += gcnew System::EventHandler(this, &frmPrismaECR::txtCuotas_TextChanged);
			// 
			// lblRecargo
			// 
			this->lblRecargo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblRecargo->Location = System::Drawing::Point(289, 257);
			this->lblRecargo->Name = L"lblRecargo";
			this->lblRecargo->Size = System::Drawing::Size(87, 19);
			this->lblRecargo->TabIndex = 71;
			this->lblRecargo->Text = L"0.00";
			this->lblRecargo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(216, 257);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(67, 19);
			this->label2->TabIndex = 70;
			this->label2->Text = L"Recargo $:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblCuotas
			// 
			this->lblCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblCuotas->Location = System::Drawing::Point(22, 255);
			this->lblCuotas->Name = L"lblCuotas";
			this->lblCuotas->Size = System::Drawing::Size(125, 19);
			this->lblCuotas->TabIndex = 62;
			this->lblCuotas->Text = L"Cuotas:";
			this->lblCuotas->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblMonto
			// 
			this->lblMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMonto->ForeColor = System::Drawing::Color::Black;
			this->lblMonto->Location = System::Drawing::Point(28, 221);
			this->lblMonto->Name = L"lblMonto";
			this->lblMonto->Size = System::Drawing::Size(125, 19);
			this->lblMonto->TabIndex = 61;
			this->lblMonto->Text = L"Monto $$:";
			this->lblMonto->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblTarjeta
			// 
			this->lblTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblTarjeta->Location = System::Drawing::Point(28, 185);
			this->lblTarjeta->Name = L"lblTarjeta";
			this->lblTarjeta->Size = System::Drawing::Size(125, 19);
			this->lblTarjeta->TabIndex = 69;
			this->lblTarjeta->Text = L"Tarjeta:";
			this->lblTarjeta->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// cboTarjetas
			// 
			this->cboTarjetas->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTarjetas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboTarjetas->FormattingEnabled = true;
			this->cboTarjetas->ItemHeight = 18;
			this->cboTarjetas->Location = System::Drawing::Point(159, 184);
			this->cboTarjetas->Name = L"cboTarjetas";
			this->cboTarjetas->Size = System::Drawing::Size(191, 26);
			this->cboTarjetas->Sorted = true;
			this->cboTarjetas->TabIndex = 68;
			this->cboTarjetas->SelectedIndexChanged += gcnew System::EventHandler(this, &frmPrismaECR::cboTarjetas_SelectedIndexChanged);
			this->cboTarjetas->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &frmPrismaECR::cboTarjetas_PreviewKeyDown);
			// 
			// txtMonto
			// 
			this->txtMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtMonto->Location = System::Drawing::Point(159, 219);
			this->txtMonto->MaxLength = 10;
			this->txtMonto->Name = L"txtMonto";
			this->txtMonto->Size = System::Drawing::Size(191, 26);
			this->txtMonto->TabIndex = 63;
			this->txtMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMonto->TextChanged += gcnew System::EventHandler(this, &frmPrismaECR::txtCuotas_TextChanged);
			// 
			// lvw
			// 
			this->lvw->Activation = System::Windows::Forms::ItemActivation::OneClick;
			this->lvw->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(4) {this->clh1, this->clh2, this->clh3, 
				this->clh4});
			this->lvw->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lvw->FullRowSelect = true;
			this->lvw->HideSelection = false;
			this->lvw->HoverSelection = true;
			this->lvw->Location = System::Drawing::Point(19, 367);
			this->lvw->MultiSelect = false;
			this->lvw->Name = L"lvw";
			this->lvw->Size = System::Drawing::Size(411, 109);
			this->lvw->TabIndex = 66;
			this->lvw->UseCompatibleStateImageBehavior = false;
			this->lvw->View = System::Windows::Forms::View::Details;
			this->lvw->SelectedIndexChanged += gcnew System::EventHandler(this, &frmPrismaECR::lvw_SelectedIndexChanged);
			// 
			// clh1
			// 
			this->clh1->Text = L"Nro Tarjeta";
			this->clh1->Width = 100;
			// 
			// clh2
			// 
			this->clh2->Text = L"Nombre";
			this->clh2->Width = 160;
			// 
			// clh3
			// 
			this->clh3->Text = L"Cuotas";
			this->clh3->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// clh4
			// 
			this->clh4->Text = L"Importe";
			this->clh4->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->clh4->Width = 80;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(15, 103);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(413, 67);
			this->label1->TabIndex = 67;
			this->label1->Text = L"Seleccionar la tarjeta y verificar las cuotas e importe de compra.\r\nPulse la tecl" 
				L"a \"+\" para Autorizar el cupon deTarjeta y seguir las instrucciones en pantalla.";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// btnAdd
			// 
			this->btnAdd->Font = (gcnew System::Drawing::Font(L"Tahoma", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAdd->Location = System::Drawing::Point(371, 183);
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size(50, 48);
			this->btnAdd->TabIndex = 65;
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Click += gcnew System::EventHandler(this, &frmPrismaECR::btnAdd_Click);
			// 
			// btnDot
			// 
			this->btnDot->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btnDot->Font = (gcnew System::Drawing::Font(L"Tahoma", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnDot->Location = System::Drawing::Point(578, 286);
			this->btnDot->Name = L"btnDot";
			this->btnDot->Size = System::Drawing::Size(50, 48);
			this->btnDot->TabIndex = 76;
			this->btnDot->Text = L".";
			this->btnDot->UseVisualStyleBackColor = false;
			this->btnDot->Click += gcnew System::EventHandler(this, &frmPrismaECR::btnDot_Click);
			// 
			// lblFaltante
			// 
			this->lblFaltante->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lblFaltante->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lblFaltante->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblFaltante->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->lblFaltante->Location = System::Drawing::Point(466, 344);
			this->lblFaltante->Name = L"lblFaltante";
			this->lblFaltante->Size = System::Drawing::Size(162, 64);
			this->lblFaltante->TabIndex = 75;
			this->lblFaltante->Text = L"Faltan $: 0.00";
			this->lblFaltante->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// tmrMensaje
			// 
			this->tmrMensaje->Interval = 4000;
			this->tmrMensaje->Tick += gcnew System::EventHandler(this, &frmPrismaECR::tmrMensaje_Tick);
			// 
			// tmrFinalizar
			// 
			this->tmrFinalizar->Interval = 500;
			this->tmrFinalizar->Tick += gcnew System::EventHandler(this, &frmPrismaECR::tmrFinalizar_Tick);
			// 
			// frmPrismaECR
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Name = L"frmPrismaECR";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"frmPrismaECR";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e) {
				 String ^lmensaje = ValidaLapos();
				 if (lmensaje == "<none>")
				 {
					 if (Dump::TarjPrismaECR->Count <= CantTotalTarjetas){
						 ShowTarjeta^ mdatos = (ShowTarjeta^) cboTarjetas->SelectedItem;
						 if (!String::IsNullOrEmpty(mdatos->CodTarjetaPrisma))
						 {
							 PosPaymentRequest ^trCompra = gcnew PosPaymentRequest();
							 trCompra->EcrTransactionId = "";
							 //TrxPrisma ^trCompra = gcnew TrxPrisma();
							 trCompra->MarcaTarjeta = mdatos->CodTarjetaPrisma->Substring(1);
							 //trCompra->CodigoPlan = mdatos->CodPlanPrisma;
							 trCompra->MontoOperacion = actuTarjVerifonePrisma->Importe;
							 trCompra->NumeroCuotas = actuTarjVerifonePrisma->AlbNumCuota;
							 trCompra->NumeroComprobanteFiscal = String::Format("{0:000000000000}", pf->GetTickNro(GlobalTF));
							 trCompra->MontoExtracash = actuTarjVerifonePrisma->ImporteCashBack;
							 trCompra->NumeroComercio = "03659307";// mdatos->NroComercio;
							 trCompra->NumeroCajero = System::Convert::ToString(ckaNro);
							 trCompra->NumeroSucursal = suc;
							 trCompra->NumeroCaja = caja;
							 trCompra->PuntoVentaFiscal = gcnew String(pf->ptovta);
							 //trCompra->CodigoComercio = mdatos->NroComercio;


							 
							 PosPaymentResponse ^mrta = nullptr;
							 MsgAlertPrismaECR ^otroMsg = gcnew MsgAlertPrismaECR(trCompra);
							 otroMsg->ShowMsgProcesaVenta("Realizando Operacion por Verifone Prisma", "Aguarde un instante");
							 mrta = otroMsg->RespuestaVenta;
							 if (!otroMsg->ConError){
								 if (mrta != nullptr && mdatos->CodTarjetaPrisma == "QRQ" && mrta->Estado == "APROBADA"){
									 actuTarjVerifonePrisma->QrModo = true;
									 bool seencontro = false;
								 for each(System::Object ^ptitem in this->cboTarjetas->Items)
									 {
										 ShowTarjeta^unitem = (ShowTarjeta^) ptitem;
										 //RtaQrModo ^sprta = (RtaQrModo ^) mrta;
										 if (unitem->CodTarjetaPrisma != nullptr && unitem->CodTarjetaPrisma->Substring(1) == mrta->MarcaTarjeta){
											 actuTarjVerifonePrisma->AlbNroTarjeta = unitem->CodTarjetaAlbe;
											 actuTarjVerifonePrisma->AlbNroMpago = unitem->CodMpagoAlbe;	
											 actuTarjVerifonePrisma->longaut = unitem->LongAuto;
											 seencontro = true;
											 //break;
										 }										 
									 }
								 }
							 }
							 else
								 MessageBox::Show(otroMsg->MensajeError);

							 
							 if (mrta != nullptr)
							 {
								 actuTarjVerifonePrisma->CodigoRespuesta = mrta->Estado;
								 //actuTarjVerifonePrisma->MensajeRespuesta = mrta->MensajeRespuesta;
								 //la operacion esta aprobada
								 if (mrta->Estado == "APROBADA")
								 {
									 actuTarjVerifonePrisma->NroLote = System::Convert::ToInt32(mrta->NumeroLote);
									 actuTarjVerifonePrisma->NroCupon = System::Convert::ToInt32(mrta->NumeroCupon);
									 actuTarjVerifonePrisma->NroAutorizacion = mrta->CodigoAutorizacion;
									 actuTarjVerifonePrisma->Ultimos4Digitos = mrta->Ult4Digitos;
									 actuTarjVerifonePrisma->Prim6Digitos = mrta->Prim6Digitos;
									 actuTarjVerifonePrisma->OperacionTipo = actuTarjVerifonePrisma->ImporteCashBack > 0 ? "COMPRA+CASHBACK" : "COMPRA";
									 actuTarjVerifonePrisma->NombreCliente = ""; //mrta->NombreCliente;									 
									 actuTarjVerifonePrisma->AlbEtapa = 3;
									 //actuTarjVerifonePrisma->NroTerminal = System::Convert::ToString(mrta->TerminalId);
									 this->faltante = faltante - actuTarjVerifonePrisma->ImporteIngresado;
									 //para confirmar los descuentos por medio de pago
									 if (actPromoMp != nullptr)
										 Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);

									 // Diagnostico del recargo $0.01: el valor con el que el cupon entra a la
									 // lista, DESPUES de la espera de la autorizacion. Si difiere del que dejo
									 // 'RECAFIN.ECR - Calculado', se corrompio durante esa ventana.
									 WLog("RECAFIN.ECR - AlGuardar: ImporteRecargo=%.2lf ImporteIngresado=%.2lf Importe=%.2lf (lote=%d cupon=%d tarj=%d mpago=%d)",
										 System::Convert::ToDouble(actuTarjVerifonePrisma->ImporteRecargo),
										 System::Convert::ToDouble(actuTarjVerifonePrisma->ImporteIngresado),
										 System::Convert::ToDouble(actuTarjVerifonePrisma->Importe),
										 actuTarjVerifonePrisma->NroLote, actuTarjVerifonePrisma->NroCupon,
										 actuTarjVerifonePrisma->AlbNroTarjeta, actuTarjVerifonePrisma->AlbNroMpago);

									 Dump::TarjPrismaECR->Add(actuTarjVerifonePrisma);
									 actuTarjVerifonePrisma = gcnew TransPrismaECR();
									 lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");
									 LlenarLvw();
									 NuevoCupon();								  
								 }
								 else if (mrta->Estado == "RECHAZADA" || mrta->Estado == "ANULADA")
								 {
									 MessageBox::Show(mrta->Mensaje);
								 }
							 }
						 }
						 else{
							 lblMensaje->Visible = true;						 
							 tmrMensaje->Enabled = true;
						 }
					 }
					 else
					 {
						 Alert("Aviso - El sistema no permite ingresar mas", "Tarjetas para autorizar");		
					 }
				 }else
				 {
					 lblMensaje->Visible = true;
					 lblMensaje->Text = lmensaje;
					 tmrMensaje->Enabled = true;
				 }
			 }


			 private: System::Void LlenarLvw(System::Void)
			  {
				    lvw->BeginUpdate();
				    lvw->Items->Clear();				    				
					
					for each (TransPrismaECR ^tagposnet in Dump::TarjPrismaECR)
					{						
						ListViewItem ^litem = gcnew ListViewItem();
						litem->Text = tagposnet->Ultimos4Digitos;
						litem->SubItems->Add(tagposnet->NombreEmisor);						
						litem->SubItems->Add(Convert::ToString(tagposnet->AlbNumCuota));
						litem->SubItems->Add((tagposnet->Importe).ToString("F2"));						
						
						litem->Tag = tagposnet;						
						lvw->Items->Add(litem);					
						this->recibido = (Decimal) this->recibido + (Decimal) tagposnet->Importe;						
					}					
					
					lvw->EndUpdate();
			  }

	  
			  protected: String^ ValidaLapos()
				{
					String ^aux = String::Empty;
					String ^svalues = "";
					if (!String::IsNullOrEmpty(txtCuotas->Text))
					{
						svalues = txtCuotas->Text->Length > 2 ? txtCuotas->Text->Substring(0, 2) : txtCuotas->Text;						
						//param->m_cuotas = Convert::ToInt32(svalues);
						actuTarjVerifonePrisma->AlbNumCuota = Convert::ToInt32(svalues);
						if (actuTarjVerifonePrisma->AlbNumCuota <= 0)
							aux = "El número de cuotas debe ser mayor a Cero!!";

					}else
						aux = "Ingrese el número de cuotas!!";						

					if (!String::IsNullOrEmpty(txtMonto->Text)){
						//param->m_monto = Convert::ToDouble(this->txtMonto->Text);
						actuTarjVerifonePrisma->ImporteIngresado = Convert::ToDecimal(this->txtMonto->Text);
						if (actuTarjVerifonePrisma->ImporteIngresado <= 0)
							aux = "Verificar el importe de Compra ingresado!!";
					}
					else
						aux = "Ingrese el monto del Cupon!!";
					
					if (cboTarjetas->SelectedIndex == this->indextar0)
						aux = "Seleccione una Tarjeta";
					
					if (aux != String::Empty)
						return aux;

					
					if (actuTarjVerifonePrisma->ImporteIngresado <= 0)
						aux = "Ingrese el monto del pago";					
					else if (actuTarjVerifonePrisma->AlbNumCuota <= 0)
						aux = "Numero de cuotas Invalido!!";
					else if (actuTarjVerifonePrisma->ImporteIngresado > faltante)
						aux = "Importe ingresado es mayor al monto faltante";
					else
						aux = "<none>";
					return aux;
				}

	 public: virtual bool ProcessHotKey(Keys key) override
			 {
				 if(key == Keys::Right)
				 {
					 lvw->SuspendLayout();
					 if (lvw->Items->Count > 0)
					 {						 
						 int selIndex = lvw->SelectedIndices->Count > 0 ? lvw->SelectedIndices[0] + 1: 0;
						 lvw->SelectedItems->Clear();
						 if (selIndex >= lvw->Items->Count)
							 selIndex = 0;
						 lvw->Items[selIndex]->Selected = true;
						 lvw->Items[selIndex]->Focused = true;
						 this->nuevolapos = false;
					 }
					 lvw->ResumeLayout();
					 return true;
				 }
				 else if (key == Keys::Add)
				 {
					 if (this->nuevolapos)
						btnAdd_Click(this, gcnew System::EventArgs());
					 else
					 {
						 NuevoCupon();
						 this->nuevolapos = true;
					 }

					 return true;
				 }
				 else if (key == Keys::Down)
				 {
					 if (this->tb->Name == "cboTarjetas")
					 {
						 Strings::DisableKey(this);
						 cboTarjetas->DroppedDown = true;
						 return true;						 
					 }					 
				 }
				 return BaseDialog::ProcessHotKey(key);
			 }

    ///Deja la pantalla lista para que se cargue un nuevo cupon
	private: System::Void NuevoCupon(System::Void)
			  {
				  
				  this->cboTarjetas->SelectedIndex = indextar0;				 
				  this->txtMonto->Text = this->faltante.ToString("F2");				  
				  this->txtCuotas->Text = "1";
				  this->lblRecargo->Text = "0.00";
				  this->txtExtracash->Text = "";
				  this->frecargo = 0;
				  this->ftasarecargo = 0;
				  //this->cboQrModo->SelectedIndex = 0;				  
			  }

	private: System::Void lvw_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (this->lvw->SelectedItems->Count > 0)
			 {
				 
			}
		 }		

	
	///Muestra un mensaje de error cuando no puede agregar un cupon
	private: System::Void tmrMensaje_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 tmrMensaje->Enabled = false;
			 lblMensaje->Visible = false;
		 }

	

		protected: virtual System::Void Boton_Click(System::Object^ sender, System::EventArgs^ e) override
        {
			String ^scadena = tb->GetType()->Name;
			if (tb != nullptr && tb->GetType()->Name == "TextBox")
            {
               Button ^btn = (Button ^)sender;
					String ^k = (String ^) btn->Tag;
					TextBox ^ltext = (TextBox ^) tb;				
					if (ltext->Text->Length < ltext->MaxLength)
					{
						if (borrar)
						{
							ltext->Text = (k == ".") ? "0." : k;
							borrar = false;
						}
						else
						{
							if (k != "." || (k == "." && !ltext->Text->Contains(".")))
								ltext->Text += k;							
						}
					}
            }
        }

		protected: void virtual bEnter_Click(Object ^sender, EventArgs ^e) override
		{
			if(tb != nullptr && tb->Name == "txtMonto" && tb->Text->IndexOf('.') < 0)
			{				
				if (tb->Text->Length == 0)
					tb->Text = "0.";
				else
					tb->Text = tb->Text->Trim() + ".";
				}
				//procesarClicks = false;
		}

	   void TbGotFocus(Object ^sender, EventArgs ^e)
		{
			if(inGotFocus) return;
			inGotFocus = true;

			Control ^ctl = dynamic_cast<Control ^>(sender);
			
			BaseDialog::SetFocusTo(ctl);

			inGotFocus = false;
		}

	   protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
        {
            WriteBootLog("AVISO - Saliendo a pantalla de Cupones Prisma ECR con CANCEL");			
        }

private: System::Void cboTarjetas_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 if (e->KeyCode == Keys::Enter)
			 {
				 cboTarjetas->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
				 if (indexsel != cboTarjetas->SelectedIndex)
				 {
					 indexsel = cboTarjetas->SelectedIndex;
					 txtCuotas_TextChanged(sender, gcnew System::EventArgs());
				 }
			 }
		 }


		 private: System::Void txtCuotas_TextChanged(System::Object^  sender, System::EventArgs^  e) 
				  {
					  if (calculandomonto)
						  return;
					  //			cboQrModo->SelectedIndex = 0;

					  frecargo = 0;
					  ftasarecargo = 0;
					  calculandomonto = true;

					  //MDatosTarj^ mdatos = (MDatosTarj ^) cboTarjetas->Items[cboTarjetas->SelectedIndex];
					  ShowTarjeta^ mdatos = (ShowTarjeta^) cboTarjetas->SelectedItem;
					  if (mdatos->TipoTarje == 'C')
					  {
						  txtExtracash->Text = "0.00";
						  txtExtracash->Enabled = false;
					  }
					  else
						  txtExtracash->Enabled = true;
					  TarjDef *def;
					  bool encontrado = false;
					  for(int i = 0; i < defTarjetas.Count(); i++)
					  {
						  def = (TarjDef *) defTarjetas.Item(i);
						  if (def->Codigo == mdatos->CodTarjetaAlbe)
						  {
							  encontrado = true;
							  break;
						  }
					  }
					  String ^tbText = this->txtCuotas->Text->Trim();
					  int lpcuotas = 0;
					  try { 
						  lpcuotas = Int32::Parse(tbText); }
					  catch(...) { 
						  lpcuotas = 1; }

					  Decimal lpExtra = 0;
					  try { 
						  lpExtra = Decimal::Parse(txtExtracash->Text); }
					  catch(...) { 
						  lpExtra = 0; }
					  actuTarjVerifonePrisma->ImporteCashBack = lpExtra;

					  actPromoMp = nullptr;
					  if (encontrado && lpcuotas > 0)
					  {
						  TarjInfo *actualTarj = new TarjInfo();
						  actualTarj->Definicion = def;

						  double uMonto = 0;
						  try{
							  uMonto = Double::Parse(txtMonto->Text);
						  }catch(...){
							  uMonto = 0;
						  }

						  actualTarj->Recargo = 0;
						  actualTarj->TasaRecargo = 0;
						  actualTarj->Monto = uMonto;
						  actualTarj->Cuotas = lpcuotas;

						  ///////
						  actuTarjVerifonePrisma->AlbNroTarjeta = actualTarj->Definicion->Codigo;
						  actuTarjVerifonePrisma->AlbNroMpago = actualTarj->Definicion->MPago;
						  actuTarjVerifonePrisma->AlbNumCuota = lpcuotas;
						  actuTarjVerifonePrisma->ImporteIngresado = Convert::ToDecimal(uMonto);
						  actuTarjVerifonePrisma->NombreEmisor = gcnew String(actualTarj->Definicion->Nombre);
						  actuTarjVerifonePrisma->longaut = actualTarj->Definicion->longaut;
						  actuTarjVerifonePrisma->TarjCredito = actualTarj->Definicion->tipo == 'C';
						  actualTarj->montoIngresado = uMonto;
						  actualTarj->tasaDescto = def->porDesctoCom;				

						  Dump::promosmp->CalculoDescto(def->MPago, def->Codigo, uMonto);//para el recalculo de las promociones por medio de pago				
						  actPromoMp = Dump::promosmp->BuscarConTarjeta(def->MPago, def->Codigo);

						  TarjCuota *cuo = actualTarj->DatosCuota();
						  if(cuo == NULL)
						  {
							  Alert("El numero de cuotas indicado NO es Valido!!", "Ingreselo de nuevo...");
							  actualTarj->Cuotas = 1;
							  calculandomonto = false;
							  this->txtCuotas->Text = "1";					
							  calculandomonto = true;
						  }
						  else
						  {
							  Decimal rec = Decimal(cuo->Porcen / 100 * uMonto);
							  if ((def->Codigo == 2 || def->Codigo == 7) 
								  &&((cuo->Cuota == 13 && tresSinInteres > 0) || (cuo->Cuota == 16 && seisSinInteres > 0)))
							  {
								  double resta = System::Convert::ToDouble(this->faltante);
								  if (uMonto <= tresSinInteres || uMonto <= seisSinInteres)
									  rec = 0;
								  else 						
									  rec = Decimal(cuo->Porcen / 100 * resta);						
							  }

							  Decimal cajaDescto = 0;					
							  if(actPromoMp != nullptr)
							  {
								  cajaDescto = 0;						
								  for(int ki = 0; ki < actPromoMp->Count; ki++)
								  {
									  PromoMedioPago ^miact = actPromoMp[ki];				
									  if (miact->Monto > 0 && !miact->Confirmado)// tope 2 veces el descuento
									  {
										  cajaDescto += miact->Monto;															
									  }							
								  }
							  }
							  else
								  cajaDescto = 0;
							  cajaDescto = System::Decimal::Round(cajaDescto,2);
							  actualTarj->montoDescto = System::Convert::ToDouble(cajaDescto);
							  //actualTarj->Monto = uMonto + actualTarj->Recargo;					
							  rec -= cajaDescto;

							  frecargo = Decimal::ToDouble(Decimal::Round(rec, 2));
							  ftasarecargo = cuo->Porcen;
							  actuTarjVerifonePrisma->ImporteRecargo = Convert::ToDecimal(frecargo);
							  actuTarjVerifonePrisma->Importe = actuTarjVerifonePrisma->ImporteIngresado + actuTarjVerifonePrisma->ImporteRecargo;

							  // Diagnostico del recargo que queda en $0.01: deja registrado el recargo tal
							  // como se calcula aca (origen), para poder compararlo despues contra el que
							  // llega a mpago_() y contra el que termina facturado en el renglon del PLU.
							  WLog("RECAFIN.ECR - Calculado: tasa=%.4lf uMonto=%.2lf cajaDescto=%.2lf rec=%.2lf -> ImporteRecargo=%.2lf, Importe=%.2lf (tarj=%d mpago=%d cuotas=%d)",
								  cuo->Porcen, uMonto, System::Convert::ToDouble(cajaDescto), System::Convert::ToDouble(rec),
								  System::Convert::ToDouble(actuTarjVerifonePrisma->ImporteRecargo),
								  System::Convert::ToDouble(actuTarjVerifonePrisma->Importe),
								  actualTarj->Definicion->Codigo, actualTarj->Definicion->MPago, lpcuotas);

						  }
						  delete actualTarj;
					  }
					  lblRecargo->Text = frecargo.ToString("F2");
					  calculandomonto = false;
				  }


			protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
					   {

						   BaseDialog::btOk_Click(sender, e);
						   WriteBootLog("AVISO - Saliendo a pantalla de Cupones PRISMA ECR con OK");

					   }		  

private: System::Void cboTarjetas_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			{
				try
				{
					//if (pantalla == 2)
					txtCuotas_TextChanged(gcnew System::Object(), gcnew System::EventArgs());
				}
				catch (Exception ^excep)
				{						 
					WriteBootLog("AVISO - Error al cambiar de indice en combobox, cboTarjetas_SelectedIndexChanged");
				}

			}
private: System::Void tmrFinalizar_Tick(System::Object^  sender, System::EventArgs^  e) {
			 tmrFinalizar->Enabled = false;
			 Cancel_Click(sender, e);
			 this->Close();
		 }
private: System::Void btnDot_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(tb != nullptr && (tb->Name == "txtMonto" || tb->Name == "txtExtraCashBack") && tb->Text->IndexOf('.') < 0)
			 {				
				 if (tb->Text->Length == 0)
					 tb->Text = "0.";
				 else
					 tb->Text = tb->Text->Trim() + ".";
			 }
		 }

};
}
