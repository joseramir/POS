#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Controles;


#include "DatosMpagos.h"
#include "Funcs.h"
#include "tjonline.h"
#include "frmCupones.h"
#include "StaticDump.h"


namespace pos {

	/// <summary>
	/// Summary for PagoDatosTarj
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class PagoDatosTarj : public Controles::BaseDialog
	{
	public:
		PagoDatosTarj(class PagoExtendido *pex, Decimal ptotal, bool credito)
		{
			struct mpag_ *p;
			InitializeComponent();
			//Titulo->Text = Titulo->Text + (credito? "Crédito": "Débito");
			
			this->TabControls->Add(this->txtNroTarjeta);
			this->TabControls->Add(this->cboTarjetas);
			this->TabControls->Add(txtMonto);			
			if (credito)
			{
				this->TabControls->Add(txtCuotas);				
			}
			else 
			{
				this->txtCuotas->Enabled = false;							
			}
			this->mpex = pex;
			Strings::InterceptKeyboard(this, 1);			
			this->nuevolapos = true;
			this->total = ptotal;
			this->recibido = gcnew Decimal(0);
			this->faltante = ptotal;
			this->lblMensaje->Visible = false;
			this->escredito = credito;
			this->ExtendedKeyboardEnabled = usaTouch;						
			this->txtNroTarjeta->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PagoDatosTarj::txtSoloNumeros_KeyPress);
			this->txtMonto->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PagoDatosTarj::txtMonto_KeyPress);			
			this->txtCuotas->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &PagoDatosTarj::txtSoloNumeros_KeyPress);			
			this->txtNroTarjeta->GotFocus += gcnew System::EventHandler(this, &PagoDatosTarj::TbGotFocus);			
			this->txtMonto->GotFocus += gcnew System::EventHandler(this, &PagoDatosTarj::TbGotFocus);
			this->txtCuotas->GotFocus += gcnew System::EventHandler(this, &PagoDatosTarj::TbGotFocus);			
			
			LlenarLvw();				
			if ((Decimal)this->total <= Decimal(0)) 
				this->faltante = ((Decimal)this->total == Decimal(0)) ? Decimal(0) : (Decimal) this->total - (Decimal) this->recibido;
			
			if (!tjOnline && defTarjetas.Count() <= 0)
				CargaTjOnline();
			cboTarjetas->Items->Clear();			
			MDatosTarj^ tar0 = gcnew MDatosTarj(0, "Otra Tarjeta", credito ? 9 : 3, false, 6);
			cboTarjetas->Items->Add(tar0);
			for(int k = 0; k < defTarjetas.Count(); k++)
			{
				TarjDef *def = (TarjDef *) defTarjetas.Item(k);
				p = BuscarMpag(def->MPago);
				MDatosTarj^ mdatos = gcnew MDatosTarj(def->Codigo, gcnew String(def->Nombre), def->MPago, p->extraible, def->longaut);				
				if (credito && def->tipo == 'C')
					cboTarjetas->Items->Add(mdatos);
				else if (!credito && def->tipo == 'D')
					cboTarjetas->Items->Add(mdatos);
			}
			indextar0 = cboTarjetas->Items->IndexOf(tar0);
			indexsel = indextar0;
			cboTarjetas->SelectedIndex = indextar0;
			NuevoCupon();			
			lblFaltante->Text = "Faltan $: " + this->faltante->ToString("F2");
			WriteBootLog("AVISO - Entrando a pantalla de Cupones posnet");
		}
	private: System::Windows::Forms::Button^  btnAdd;
	private: System::Windows::Forms::ListView^  lvw;
	private: System::Windows::Forms::ColumnHeader^  clh1;
	private: System::Windows::Forms::ColumnHeader^  clh2;
	private: System::Windows::Forms::ColumnHeader^  clh3;
				

	private: System::Windows::Forms::Button^  btnQuit;

			 List<PromoMedioPago ^> ^actPromoMp;
	public: 

	protected:
		class PagoExtendido *mpex;
		bool nuevolapos;
		System::Decimal ^total;
		System::Decimal ^recibido;
		bool escredito;
		bool inGotFocus;
		int indextar0;		//Indice de 'Otra Tarjeta'		
		int indexsel; 
		double frecargo;
		double ftasarecargo;
		bool calculandomonto;
		int uindex;

	private: System::Windows::Forms::Label^  lblMensaje;
	private: System::Windows::Forms::Timer^  tmrMensaje;
	private: System::Windows::Forms::Label^  lblNroTarjeta;
	private: System::Windows::Forms::TextBox^  txtNroTarjeta;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  tbDummy;

	private: System::Windows::Forms::ComboBox^  cboTarjetas;
	private: System::Windows::Forms::Label^  lblTarjeta;
	private: System::Windows::Forms::ColumnHeader^  clh4;

	private: System::Windows::Forms::Label^  lblRecargo;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lblFaltante;













	protected: 
		System::Decimal ^faltante;


		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~PagoDatosTarj()
		{
			tmrMensaje->Enabled = false;
			if (components)
			{
				delete components;
			}
		}
	
	private: System::Windows::Forms::Label^  lblMonto;


	private: System::Windows::Forms::Label^  lblCuotas;
	private: System::Windows::Forms::TextBox^  txtMonto;

	private: System::Windows::Forms::TextBox^  txtCuotas;
				

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PagoDatosTarj::typeid));
			this->lblMonto = (gcnew System::Windows::Forms::Label());
			this->lblCuotas = (gcnew System::Windows::Forms::Label());
			this->txtMonto = (gcnew System::Windows::Forms::TextBox());
			this->txtCuotas = (gcnew System::Windows::Forms::TextBox());
			this->cboTarjetas = (gcnew System::Windows::Forms::ComboBox());
			this->lblNroTarjeta = (gcnew System::Windows::Forms::Label());
			this->txtNroTarjeta = (gcnew System::Windows::Forms::TextBox());
			this->lblTarjeta = (gcnew System::Windows::Forms::Label());
			this->btnAdd = (gcnew System::Windows::Forms::Button());
			this->lvw = (gcnew System::Windows::Forms::ListView());
			this->clh1 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh2 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh3 = (gcnew System::Windows::Forms::ColumnHeader());
			this->clh4 = (gcnew System::Windows::Forms::ColumnHeader());
			this->btnQuit = (gcnew System::Windows::Forms::Button());
			this->lblMensaje = (gcnew System::Windows::Forms::Label());
			this->tmrMensaje = (gcnew System::Windows::Forms::Timer(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tbDummy = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->lblRecargo = (gcnew System::Windows::Forms::Label());
			this->lblFaltante = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->btnQuit);
			this->panel->Controls->Add(this->lblFaltante);
			this->panel->Controls->Add(this->txtCuotas);
			this->panel->Controls->Add(this->lblRecargo);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->lblCuotas);
			this->panel->Controls->Add(this->lblMonto);
			this->panel->Controls->Add(this->lblTarjeta);
			this->panel->Controls->Add(this->lblNroTarjeta);
			this->panel->Controls->Add(this->cboTarjetas);
			this->panel->Controls->Add(this->lblMensaje);
			this->panel->Controls->Add(this->txtMonto);
			this->panel->Controls->Add(this->txtNroTarjeta);
			this->panel->Controls->Add(this->lvw);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->btnAdd);
			this->panel->Controls->Add(this->tbDummy);
			this->panel->Size = System::Drawing::Size(648, 567);
			this->panel->Controls->SetChildIndex(this->tbDummy, 0);
			this->panel->Controls->SetChildIndex(this->btnAdd, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->lvw, 0);
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
			this->panel->Controls->SetChildIndex(this->txtNroTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->txtMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblMensaje, 0);
			this->panel->Controls->SetChildIndex(this->cboTarjetas, 0);
			this->panel->Controls->SetChildIndex(this->lblNroTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->lblTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->lblMonto, 0);
			this->panel->Controls->SetChildIndex(this->lblCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->lblRecargo, 0);
			this->panel->Controls->SetChildIndex(this->txtCuotas, 0);
			this->panel->Controls->SetChildIndex(this->lblFaltante, 0);
			this->panel->Controls->SetChildIndex(this->btnQuit, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 419);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 365);
			// 
			// panel3
			// 
			this->panel3->Size = System::Drawing::Size(636, 4);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(161, 23);
			this->Titulo->Text = L"Cupon Tarjetas ";
			// 
			// bEnter
			// 
			this->bEnter->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"bEnter.Image")));
			// 
			// lblMonto
			// 
			this->lblMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMonto->ForeColor = System::Drawing::Color::Black;
			this->lblMonto->Location = System::Drawing::Point(42, 183);
			this->lblMonto->Name = L"lblMonto";
			this->lblMonto->Size = System::Drawing::Size(125, 19);
			this->lblMonto->TabIndex = 25;
			this->lblMonto->Text = L"Monto $$:";
			this->lblMonto->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lblCuotas
			// 
			this->lblCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblCuotas->Location = System::Drawing::Point(36, 217);
			this->lblCuotas->Name = L"lblCuotas";
			this->lblCuotas->Size = System::Drawing::Size(125, 19);
			this->lblCuotas->TabIndex = 41;
			this->lblCuotas->Text = L"Cuotas:";
			this->lblCuotas->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtMonto
			// 
			this->txtMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtMonto->Location = System::Drawing::Point(173, 181);
			this->txtMonto->MaxLength = 10;
			this->txtMonto->Name = L"txtMonto";
			this->txtMonto->Size = System::Drawing::Size(191, 26);
			this->txtMonto->TabIndex = 42;
			this->txtMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtMonto->TextChanged += gcnew System::EventHandler(this, &PagoDatosTarj::txtCuotas_TextChanged);
			// 
			// txtCuotas
			// 
			this->txtCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtCuotas->Location = System::Drawing::Point(173, 215);
			this->txtCuotas->MaxLength = 2;
			this->txtCuotas->Name = L"txtCuotas";
			this->txtCuotas->Size = System::Drawing::Size(46, 26);
			this->txtCuotas->TabIndex = 45;
			this->txtCuotas->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtCuotas->TextChanged += gcnew System::EventHandler(this, &PagoDatosTarj::txtCuotas_TextChanged);
			// 
			// cboTarjetas
			// 
			this->cboTarjetas->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTarjetas->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboTarjetas->FormattingEnabled = true;
			this->cboTarjetas->ItemHeight = 18;
			this->cboTarjetas->Location = System::Drawing::Point(173, 146);
			this->cboTarjetas->Name = L"cboTarjetas";
			this->cboTarjetas->Size = System::Drawing::Size(191, 26);
			this->cboTarjetas->Sorted = true;
			this->cboTarjetas->TabIndex = 53;
			this->cboTarjetas->SelectedIndexChanged += gcnew System::EventHandler(this, &PagoDatosTarj::cboTarjetas_SelectedIndexChanged);
			this->cboTarjetas->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &PagoDatosTarj::cboTarjetas_PreviewKeyDown);
			// 
			// lblNroTarjeta
			// 
			this->lblNroTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblNroTarjeta->Location = System::Drawing::Point(42, 118);
			this->lblNroTarjeta->Name = L"lblNroTarjeta";
			this->lblNroTarjeta->Size = System::Drawing::Size(125, 19);
			this->lblNroTarjeta->TabIndex = 46;
			this->lblNroTarjeta->Text = L"Nro Tarjeta:";
			this->lblNroTarjeta->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtNroTarjeta
			// 
			this->txtNroTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->txtNroTarjeta->Location = System::Drawing::Point(173, 115);
			this->txtNroTarjeta->MaxLength = 16;
			this->txtNroTarjeta->Name = L"txtNroTarjeta";
			this->txtNroTarjeta->Size = System::Drawing::Size(191, 26);
			this->txtNroTarjeta->TabIndex = 47;
			this->txtNroTarjeta->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->txtNroTarjeta->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &PagoDatosTarj::txtNroTarjeta_Validating);
			// 
			// lblTarjeta
			// 
			this->lblTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblTarjeta->Location = System::Drawing::Point(42, 147);
			this->lblTarjeta->Name = L"lblTarjeta";
			this->lblTarjeta->Size = System::Drawing::Size(125, 19);
			this->lblTarjeta->TabIndex = 54;
			this->lblTarjeta->Text = L"Tarjeta:";
			this->lblTarjeta->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// btnAdd
			// 
			this->btnAdd->Font = (gcnew System::Drawing::Font(L"Tahoma", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnAdd->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnAdd.Image")));
			this->btnAdd->Location = System::Drawing::Point(378, 125);
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size(50, 48);
			this->btnAdd->TabIndex = 47;
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Click += gcnew System::EventHandler(this, &PagoDatosTarj::btnAdd_Click);
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
			this->lvw->Location = System::Drawing::Point(39, 380);
			this->lvw->MultiSelect = false;
			this->lvw->Name = L"lvw";
			this->lvw->Size = System::Drawing::Size(411, 94);
			this->lvw->TabIndex = 48;
			this->lvw->UseCompatibleStateImageBehavior = false;
			this->lvw->View = System::Windows::Forms::View::Details;
			this->lvw->SelectedIndexChanged += gcnew System::EventHandler(this, &PagoDatosTarj::lvw_SelectedIndexChanged);
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
			// btnQuit
			// 
			this->btnQuit->Font = (gcnew System::Drawing::Font(L"Tahoma", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnQuit->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnQuit.Image")));
			this->btnQuit->Location = System::Drawing::Point(378, 177);
			this->btnQuit->Name = L"btnQuit";
			this->btnQuit->Size = System::Drawing::Size(50, 48);
			this->btnQuit->TabIndex = 49;
			this->btnQuit->UseVisualStyleBackColor = true;
			this->btnQuit->Click += gcnew System::EventHandler(this, &PagoDatosTarj::btnQuit_Click);
			// 
			// lblMensaje
			// 
			this->lblMensaje->AutoSize = true;
			this->lblMensaje->BackColor = System::Drawing::Color::White;
			this->lblMensaje->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMensaje->ForeColor = System::Drawing::Color::Red;
			this->lblMensaje->Location = System::Drawing::Point(35, 77);
			this->lblMensaje->Name = L"lblMensaje";
			this->lblMensaje->Size = System::Drawing::Size(55, 18);
			this->lblMensaje->TabIndex = 50;
			this->lblMensaje->Text = L"label1";
			// 
			// tmrMensaje
			// 
			this->tmrMensaje->Interval = 4000;
			this->tmrMensaje->Tick += gcnew System::EventHandler(this, &PagoDatosTarj::tmrMensaje_Tick);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->label1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(38, 328);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(413, 48);
			this->label1->TabIndex = 51;
			this->label1->Text = L"Use la Flecha Derecha para Moverse por la Lista. \r\nPulse la tecla \"+\" para Ingres" 
				L"ar un nuevo cupon deTarjeta. \r\nPulse la tecla \"-\" para eliminar un cupon...";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbDummy
			// 
			this->tbDummy->Location = System::Drawing::Point(67, 79);
			this->tbDummy->Name = L"tbDummy";
			this->tbDummy->Size = System::Drawing::Size(100, 20);
			this->tbDummy->TabIndex = 52;
			this->tbDummy->Visible = false;
			// 
			// label2
			// 
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(230, 219);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(67, 19);
			this->label2->TabIndex = 55;
			this->label2->Text = L"Recargo $:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblRecargo
			// 
			this->lblRecargo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblRecargo->Location = System::Drawing::Point(300, 219);
			this->lblRecargo->Name = L"lblRecargo";
			this->lblRecargo->Size = System::Drawing::Size(112, 19);
			this->lblRecargo->TabIndex = 56;
			this->lblRecargo->Text = L"0.00";
			this->lblRecargo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->lblFaltante->Location = System::Drawing::Point(466, 290);
			this->lblFaltante->Name = L"lblFaltante";
			this->lblFaltante->Size = System::Drawing::Size(162, 64);
			this->lblFaltante->TabIndex = 57;
			this->lblFaltante->Text = L"Faltan $: 0.00";
			this->lblFaltante->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// PagoDatosTarj
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(648, 567);
			this->Name = L"PagoDatosTarj";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PagoDatosTarj";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	///
	private: System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e) 
			 {				 
				 try
				 {
					 CPosnet ^vlapos = gcnew CPosnet();
					 String ^lmensaje = ValidaLapos(vlapos);
					 if (lmensaje == "<none>")
					 {
						 PagoExtendido *lpex;					 
						 MDatosTarj ^mdatos = (MDatosTarj ^) cboTarjetas->SelectedItem;
						 for(int i = 0; i < mpex->subItems; i++)
						 {
							 PagoExtendido *pe = (PagoExtendido *) mpex->items[i];
							 if (pe->mp->nro == mdatos->mpago)
							 {
								 lpex = pe;
								 break;
							 }
						 }
						 System::Decimal^ obtenido = gcnew System::Decimal(vlapos->m_monto);
						 vlapos->m_mpago = mdatos->mpago;
						 vlapos->m_nomtarjeta = mdatos->nombre;
						 vlapos->extraible = mdatos->extraible;
						 vlapos->indice = ++uindex;
						 vlapos->longaut = mdatos->longaut;
						 if ((Decimal) obtenido <= (Decimal) faltante)
						 {
							 char *vnrot = new char[17];
							 Strings::StringToChar(vlapos->m_nrotarjeta, vnrot, 17);
							 vlapos->m_codtarjeta = BuscarTarjeta(vnrot);
							 if (vlapos->m_codtarjeta == 0)
								 vlapos->m_codtarjeta = mdatos->cod;
							 this->faltante = (Decimal) faltante - (Decimal) obtenido;
							 this->recibido = (Decimal) this->recibido + (Decimal) obtenido;
							 lpex->monto += Convert::ToDouble(obtenido);	
							 if (actPromoMp != nullptr)
								 Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);
							 GCStatic::tjPosnet->Add(vlapos);						 
							 LlenarLvw();						 						 
							 NuevoCupon();						 
							 delete vnrot;
						 }
						 else 
						 {					
							 lblMensaje->Visible = true;
							 lblMensaje->Text = "Monto ingresado es mayor al importe a cobrar!!";
							 tmrMensaje->Enabled = true;
						 }
					 }else
					 {
						 lblMensaje->Visible = true;
						 lblMensaje->Text = lmensaje;
						 tmrMensaje->Enabled = true;
					 }
					 lblFaltante->Text = "Faltan $: " + this->faltante->ToString("F2");
				}				
				catch (Exception ^e)
				{
					WriteBootLog("error");
				}
			 }
	 
	 ///Llena el listView con los datos de los cupones
	 private: System::Void LlenarLvw(System::Void)
			  {
				    lvw->BeginUpdate();
				    lvw->Items->Clear();				    				
					 int auxindex = 0;
					
					for each (CPosnet ^tagposnet in GCStatic::tjPosnet)
					{						
						ListViewItem ^litem = gcnew ListViewItem();
						String^ ultdig = gcnew String(tagposnet->m_nrotarjeta);
						litem->Text = "****-" + (ultdig->Length > 15 ? ultdig->Substring(12): ultdig->Substring(0, 4));
						litem->SubItems->Add(tagposnet->m_nomtarjeta);						
						litem->SubItems->Add(tagposnet->m_cuotas.ToString());
						litem->SubItems->Add((tagposnet->m_monto + tagposnet->m_recargo).ToString("F2"));						
						
						litem->Tag = tagposnet;						
						lvw->Items->Add(litem);					
						this->recibido = (Decimal) this->recibido + (Decimal) tagposnet->m_monto;
						auxindex = tagposnet->indice;
					}					
					
					if (auxindex > 0)
						this->uindex = auxindex;
					lvw->EndUpdate();
			  }

	  ///Valida que los datos ingresados sean los correctos
	 protected: String^ ValidaLapos(CPosnet ^param)
				{
					String ^aux = String::Empty;
					String ^svalues = txtNroTarjeta->Text;
					if (svalues->Length == 0)
						svalues = "0";					
					if (txtCuotas->Text->Trim()->Length > 0)
					{
						svalues = txtCuotas->Text->Length > 2 ? txtCuotas->Text->Substring(0, 2) : txtCuotas->Text;						
						param->m_cuotas = Convert::ToInt32(svalues);
						if (param->m_cuotas <= 0)
							aux = "El número de cuotas debe ser mayor a Cero!!";

					}else
						aux = "Ingrese el número de cuotas!!";						

					if (txtMonto->Text->Trim()->Length > 0)
						param->m_monto = Convert::ToDouble(this->txtMonto->Text);
					else
						aux = "Ingrese el monto del Cupon!!";
					
					if (cboTarjetas->SelectedIndex == this->indextar0)
						aux = "Seleccione una Tarjeta";
					
					if (txtNroTarjeta->Text->Length > 15)
					{						
						param->m_nrotarjeta = txtNroTarjeta->Text;
						param->m_vencim = Convert::ToInt32(this->cardVenc);
					}
					else
					{						
						param->m_nrotarjeta = gcnew String("000000");
						param->m_vencim = 0;						
					}					
					
					if (aux != String::Empty)
						return aux;

					param->m_recargo = frecargo;
					param->m_credito = this->escredito;
					param->m_tasarecargo = ftasarecargo;
					if (param->m_monto <= 0)
						aux = "Ingrese el monto del pago";					
					else if (param->m_cuotas <= 0)
						aux = "Numero de cuotas Invalido!!";
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
				 else if (key == Keys::Subtract)
				 {
					 btnQuit_Click(this, gcnew System::EventArgs());
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
				  this->txtNroTarjeta->Text = "";				  
				  this->cboTarjetas->SelectedIndex = indextar0;				 
				  this->txtMonto->Text = this->faltante->ToString("F2");				  
				  this->txtCuotas->Text = "1";
				  this->lblRecargo->Text = "0.00";
				  this->frecargo = 0;
				  this->ftasarecargo = 0;
				  this->txtNroTarjeta->Focus();				  
			  }

	private: System::Void lvw_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (this->lvw->SelectedItems->Count > 0)
			 {
				 PagoExtendido *lpex;
				 int selIndex = lvw->SelectedIndices[0];
				 CPosnet ^tagposnet = (CPosnet ^) lvw->Items[selIndex]->Tag;				 				 
				 int i = 0;
				 for(i = 0; i < this->mpex->subItems; i++)
				 {
					 lpex = (PagoExtendido *) this->mpex->items[i];
					 if (lpex->mp->nro == tagposnet->m_mpago)
						 break;
				 }
				 bool encontrado = false;
				 CPosnet ^lpnet;
				 for each(lpnet in GCStatic::tjPosnet)
				 {					 
					 if (tagposnet->indice == lpnet->indice) //tagposnet->m_codtarjeta == lpnet->m_codtarjeta && lpnet->m_monto == tagposnet->m_monto)
					 {
						 encontrado = true;
						 break;
					 }
				 }
				if (encontrado)
				{
					this->txtNroTarjeta->Text = lpnet->m_nrotarjeta == "000000" ? String::Empty : lpnet->m_nrotarjeta;									
					this->txtMonto->Text = Convert::ToString(lpnet->m_monto.ToString("F2"));				
					this->txtCuotas->Text = Convert::ToString(lpnet->m_cuotas);
					SeleccionaTarjeta(lpnet->m_codtarjeta);
				}
			}
		 }		

	
	///Muestra un mensaje de error cuando no puede agregar un cupon
	private: System::Void tmrMensaje_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 tmrMensaje->Enabled = false;
			 lblMensaje->Visible = false;
		 }

	///Valida que la entrada sean solo numeros
	private: System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
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

	
	private: System::Void txtMonto_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
        {
				if (Char::IsLetter(e->KeyChar))
            {
                e->Handled = true;
            }
				else if (Char::IsDigit(e->KeyChar) )
            {
                e->Handled = false;
            }
				else if (e->KeyChar == '.')
				{
					TextBox^ auxtext = dynamic_cast<TextBox ^>(sender);
					e->Handled = auxtext->Text->Contains(".");
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

private: System::Void btnQuit_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			 try
			 {
				 if (lvw->Items->Count > 0)
				 {				 
					 if (lvw->SelectedIndices != nullptr && lvw->SelectedIndices->Count > 0)
					 {		
						 int selIndex = lvw->SelectedIndices[0];						 						 
						 CPosnet ^tagposnet = (CPosnet ^) lvw->Items[selIndex]->Tag;
						 PagoExtendido *lpex;
						 int i;
						 for(i = 0; i < this->mpex->subItems; i++)
						 {
							 lpex = (PagoExtendido *) this->mpex->items[i];
							 if (lpex->mp->nro == tagposnet->m_mpago)
								 break;
						 }
						 CPosnet ^llapos;
						 bool encontrado = false;
						 i = 0;
						 for each(llapos in GCStatic::tjPosnet)
						 {
							 //llapos = (CPosnet ^) lpex->lapos->Item(i);
							 if (llapos->indice == tagposnet->indice)//llapos->m_codtarjeta == tagposnet->m_codtarjeta && llapos->m_monto == tagposnet->m_monto)							 
							 {
								 encontrado = true;
								 break;
							 }
							 i++;
						 }
						 if (encontrado)
						 {
							 double lmonto = llapos->m_monto;
							 //lpex->lapos->RemoveAt(i);
							 GCStatic::tjPosnet->RemoveAt(i);
							 lpex->monto -= lmonto;
							 this->faltante = (Decimal) this->faltante + (Decimal) lmonto;
							 this->recibido = (Decimal) this->recibido - (Decimal) lmonto;							 
							 LlenarLvw();
						    NuevoCupon();
							 this->nuevolapos = true;							 
						 }
					 }
				 }
				 lblFaltante->Text = "Faltan $: " + this->faltante->ToString("F2");
			 }
			 catch (Exception^ ex)
			 {
				 MessageBox::Show("Ocurrio el error: " + ex->Message);
			 }
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
							//tb->Text = (k == ".") ? "0." : k;
							ltext->Text = (k == ".") ? "0." : k;
							borrar = false;
						}
						else
						{
							//if (k != "." || (k == "." && !tb->Text->Contains(".")))
							if (k != "." || (k == "." && !ltext->Text->Contains(".")))
								ltext->Text += k;
								//tb->Text += k;
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
			/*if(ctl != nullptr)
			{
				MP ^mp = (MP ^)ctl->Tag;
				ctl->Text = mp->Monto.ToString("N2");
				mp->PagoExt->variacion = 0;
			}*/

			// Actualiza el estado dinamico
			//ActuDynStatus();

			// Codigo que depende si hay o no touch
			//if(!usaTouch)
			//{
			//	// Simula el click que hacen las touchscreen
			//	TextBox ^tbNew = dynamic_cast<TextBox ^>(ctl);
			//	if(tbNew != nullptr)
			//		TbClick(tbNew, nullptr);
			//}
			//else
				BaseDialog::SetFocusTo(ctl);

			/*tb = dynamic_cast<Control ^>(ctl);
			if(tb != nullptr)
				((MP ^)tb->Tag)->Borrar = true;*/

			inGotFocus = false;
		}

	   protected: virtual void Cancel_Click(Object ^sender, EventArgs ^e) override
        {
            if (this->cancel->Enabled)
				{
						
					for(int i = 0; i < mpex->subItems; i++)
					{
						PagoExtendido *lpex = (PagoExtendido *) this->mpex->items[i];
						
						//lpex->lapos->Clear();
						lpex->monto = 0;
						lpex->variacion = 0;
					}
					//Agregado para borrar los medios de Pago
					for each(CPosnet ^llapos in GCStatic::tjPosnet){
						Dump::promosmp->QuitaConfirmaDescto(llapos->m_mpago, llapos->m_codtarjeta);
					}
					GCStatic::tjPosnet->Clear();
					
					/*if (GCStatic::tjPosnet->Count > 0)
					{	
						for each(CPosnet ^llapos in GCStatic::tjPosnet)
						{
							PagoExtendido *lmpex;
							bool cencontro = false;
							for each(MP ^mp in pagos)
							{
								if (mp->PagoExt->subItems > 0)
								{
									for(int i = 0; i < mp->PagoExt->subItems; i++)
									{
										lmpex = mp->PagoExt->items[i];
										if (llapos->m_mpago == lmpex->mp->nro)
										{
											cencontro = true;
											break;
										}
									}
									if (cencontro)
										break;
								}
							}
							if (!cencontro)
							{
								lmpex->variacion = 0;
								lmpex->monto = 0;					
							}							
						}
					}*/
				}
				BaseDialog::Cancel_Click(sender, e);
				WriteBootLog("AVISO - Saliendo a pantalla de Cupones posnet con CANCEL");			
        }

	  public: virtual void StartCardReader() override
			 {
				 this->tbDummy->Enabled = true;
				 this->tbDummy->Text = String::Empty;
				 this->tbDummy->Focus();
				 BaseDialog::StartCardReader();
			 }

	  public: virtual bool CardReaderDigit(wchar_t ch) override
				 {
					 bool res = false;
					 try
					 {
						 res = BaseDialog::CardReaderDigit(ch);
						 /*this->lbCardHelp->BackColor = 
							 (this->lbCardHelp->BackColor == System::Drawing::Color::Lime)?
							 Color::FromArgb(255, 255, 128):
						 System::Drawing::Color::Lime;*/
						 //System::Windows::Forms::Application::DoEvents();
					 }
					 catch (Exception ^excep)
					 {						 
						 WriteBootLog("ERROR - AL LEER LA BANDA MAGNETICA DE LA TARJETA, CardReaderDigit");
					 }
					 return res;
				 }

	  public: virtual void EndCardReader() override
			 {
				 try
				 {
					BaseDialog::EndCardReader();				 

					if(cardAdicData->Length > 0 && cardNumber->Length > 0 && cardVenc > 0)
					{	
						 char ntarjet[23];					 
						 txtNroTarjeta->Text = cardNumber;					 
						 Strings::StringToChar(cardNumber, ntarjet, 22);
						 int nrot = BuscarTarjeta(ntarjet);
						 if (nrot > 0)
						 {
							 int posic = -1;						 
							 for(int i = 0; i < cboTarjetas->Items->Count; i++)
							 {
								 MDatosTarj^ tarj = (MDatosTarj ^) cboTarjetas->Items[i];
								 if (tarj->cod == nrot)
								 {
									 posic = i;
									 indexsel = i;
									 break;
								 }
							 }
							 if  (posic >= 0 )
							 {
								 cboTarjetas->SelectedIndex = posic;
								 SetFocusTo(txtMonto);
							 }
						 }
						 else
						 {
							 cboTarjetas->SelectedIndex = indextar0;
							 SetFocusTo(cboTarjetas);
						 }
					 
					 }
					 else
					 {
						 SetFocusTo(txtNroTarjeta);
						 Alert("Tarjeta mal leida o Defectuosa.", "Pruebe de Nuevo...");
					 }					 
					 this->tbDummy->Enabled = false;	
				 }
				 catch (Exception^ excep)
				 {
					 WriteBootLog("ERROR - AL LEER LA BANDA MAGNETICA DE LA TARJETA, EndCardReader");
				 }
			 }	  

private: System::Void SeleccionaTarjeta(int codtarj)
		 {
			 for(int i = 0; i < cboTarjetas->Items->Count; i++)
			 {
				 MDatosTarj^ mdatos = (MDatosTarj ^) cboTarjetas->Items[i];
				 if (mdatos->cod == codtarj)
				 {
					 cboTarjetas->SelectedIndex = i;
					 indexsel = i;
					 break;
				 }
			 }
		 }

private: System::Void cboTarjetas_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {

			 if (e->KeyCode == Keys::Enter)
			 {
				 cboTarjetas->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
				 if (indexsel != cboTarjetas->SelectedIndex)
				{
					/*txtMonto->Text = String::Empty;
					txtCuotas->Text = String::Empty;*/
					indexsel = cboTarjetas->SelectedIndex;
					txtCuotas_TextChanged(sender, gcnew System::EventArgs());
				}
			 }
			 
		 }
private: System::Void txtNroTarjeta_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
			 if (txtNroTarjeta->Text->Length >= 14)
			 {
				 char ntarjet[18];
				 Strings::StringToChar(txtNroTarjeta->Text, ntarjet, 17);
				 int nrot = BuscarTarjeta(ntarjet);
				 if (nrot > 0)
				 {
					 int posic = -1;
					 
					 for(int i = 0; i < cboTarjetas->Items->Count; i++)
					 {
						 MDatosTarj^ tarj = (MDatosTarj ^) cboTarjetas->Items[i];
						 if (tarj->cod == nrot)
						 {
							 posic = i;
							 break;
						 }
					 }
					 if  (posic >= 0 )
					 {
						 cboTarjetas->SelectedIndex = posic;
						 indexsel = posic;
						 txtCuotas_TextChanged(sender, gcnew System::EventArgs());
						 SetFocusTo(txtMonto);
					 }
				 }
				 else
				 {
					 cboTarjetas->SelectedIndex = indextar0;
					 SetFocusTo(cboTarjetas);
					 panel->Refresh();
				 }				 
			 }
			 else
			 {
				 lblMensaje->Visible = true;
				 lblMensaje->Text = "Falta ingresar el numero de Tarjeta";
				 tmrMensaje->Enabled = true;				 
			 }
			 e->Cancel = false;
		 }

	private: System::Void txtCuotas_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			if (calculandomonto)
				return;
			frecargo = 0;
			ftasarecargo = 0;
			calculandomonto = true;
			MDatosTarj^ mdatos = (MDatosTarj ^) cboTarjetas->Items[cboTarjetas->SelectedIndex];
			TarjDef *def;
			bool encontrado = false;
			for(int i = 0; i < defTarjetas.Count(); i++)
			{
				def = (TarjDef *) defTarjetas.Item(i);
				if (def->Codigo == mdatos->cod)
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
								miact->ImporteIngresado = Convert::ToDecimal(uMonto);							
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

				}
				delete actualTarj;
			}
			lblRecargo->Text = frecargo.ToString("F2");
			calculandomonto = false;
		}

		/*private: System::Void cboTarjetas_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			{
				if (indexsel != cboTarjetas->SelectedIndex)
				{
					indexsel = cboTarjetas->SelectedIndex;
					txtCuotas_TextChanged(this, gcnew System::EventArgs());
				}
			}*/

			
			protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
			{
				//if (lvw->Items->Count > 0)
				//{
				//	bool todos = true;
				//	for (int j = 0; j < mpex->subItems; j++)
				//	{					
				//		PagoExtendido *lmpex = (PagoExtendido *) mpex->items[j];
				//		lmpex->variacion = 0;
				//		lmpex->monto = 0;
				//		//if (lmpex->lapos->Count() > 0)
				//		if (GCStatic::tjPosnet->Count > 0)
				//		{
				//			//for (int k = 0; k < lmpex->lapos->Count(); k++)
				//			for each(CPosnet ^llapos in GCStatic::tjPosnet)
				//			{
				//				//CPosnet ^llapos = (CPosnet ^) lmpex->lapos->Item(k);								
				//				String^ nametarj = NombreTarjeta(llapos->m_codtarjeta);
				//				frmCupones^ subform = gcnew frmCupones(llapos, nametarj);
				//				subform->ShowDialog();
				//				if (subform->DialogResult == System::Windows::Forms::DialogResult::Cancel)
				//					todos = false;
				//				else
				//				{
				//					lmpex->variacion = llapos->m_recargo;
				//					lmpex->monto += llapos->m_monto + llapos->m_recargo;
				//				}
				//			}
				//		}
				//	}
				//	if (todos)
						BaseDialog::btOk_Click(sender, e);
						WriteBootLog("AVISO - Saliendo a pantalla de Cupones posnet con OK");
					/*else
						Alert("Faltan completar datos de los cupones", "Indique numero de lote y cupon");*/

				//}				
			}

		  /*private: System::String^ NombreTarjeta(int codigo)
			{
				String^ aux = String::Empty;
				for(int i = 0; i < cboTarjetas->Items->Count; i++)
				{
					MDatosTarj^ mdato = (MDatosTarj ^) cboTarjetas->Items[i];
					if (mdato->cod == codigo)
					{
						aux = mdato->nombre;
						break;
					}
				}
				return aux;
			}*/
//private: System::Void btnTarjeta_Click(System::Object^  sender, System::EventArgs^  e) {
//				this->cardData = gcnew StringBuilder("%B4508430003033923^RIOS/LUCIANA M       ^140210118911        00992000000?;4508430003033923=14021019920000000000?");
//				//this->cardData = gcnew StringBuilder("%B4517650566121018^RAMIREZ/JOSE ALFREDO     ^150712110000        00496000000?;4517650566121018=15071214960000000000?");
//				this->EndCardReader();
//			}

private: System::Void cboTarjetas_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
			{
				try
				{
					if (pantalla == 2)
						txtCuotas_TextChanged(gcnew System::Object(), gcnew System::EventArgs());
				 }
				 catch (Exception ^excep)
				 {						 
					 WriteBootLog("AVISO - Error al cambiar de indice en combobox, cboTarjetas_SelectedIndexChanged");
				 }
				
			}
};

}
