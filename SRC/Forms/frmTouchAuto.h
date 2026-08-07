#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


#include "Strings.h"

namespace pos {

	/// <summary>
	/// Summary for frmTouchAuto
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmTouchAuto : public System::Windows::Forms::Form
	{
	public:
		frmTouchAuto(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmTouchAuto()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lbLimFiscal;
	protected: 
	private: System::Windows::Forms::Label^  lbCaja;
	private: System::Windows::Forms::Label^  lbVersion;
	private: System::Windows::Forms::Label^  lbBitnetwww;
	private: System::Windows::Forms::PictureBox^  logoBitnet;
	private: System::Windows::Forms::Label^  m1;
	private: System::Windows::Forms::Panel^  termoPanel;
	private: System::Windows::Forms::Label^  termoMsg;
	private: System::Windows::Forms::ProgressBar^  termo;
	private: Pos::Desktop::NumKbd^  numKbd;
	private: System::Windows::Forms::Label^  m3;
	private: System::Windows::Forms::Label^  m2;
	private: System::Windows::Forms::Label^  visorCnt;
	private: System::Windows::Forms::Label^  visorDer;
	private: System::Windows::Forms::Label^  visorMulti;
	private: System::Windows::Forms::Label^  visorTot;
	private: System::Windows::Forms::Button^  abajo;
	private: System::Windows::Forms::Button^  arriba;
	public: System::Windows::Forms::DataGridView^  Grid;
	private: 
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column1;
	public: 
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column3;
	private: System::Windows::Forms::StatusStrip^  statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^  btModo;
	private: System::Windows::Forms::ToolStripStatusLabel^  btRed;
	private: System::Windows::Forms::ToolStripStatusLabel^  btConsulta;
	private: System::Windows::Forms::ToolStripStatusLabel^  btTF;
	private: System::Windows::Forms::ToolStripStatusLabel^  btLista;
	private: System::Windows::Forms::ToolStripStatusLabel^  btVendedor;
	private: System::Windows::Forms::ToolStripStatusLabel^  btEspera;
	private: System::Windows::Forms::ToolStripStatusLabel^  btCajero;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(frmTouchAuto::typeid));
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->lbLimFiscal = (gcnew System::Windows::Forms::Label());
			this->lbCaja = (gcnew System::Windows::Forms::Label());
			this->lbVersion = (gcnew System::Windows::Forms::Label());
			this->lbBitnetwww = (gcnew System::Windows::Forms::Label());
			this->logoBitnet = (gcnew System::Windows::Forms::PictureBox());
			this->m1 = (gcnew System::Windows::Forms::Label());
			this->termoPanel = (gcnew System::Windows::Forms::Panel());
			this->termoMsg = (gcnew System::Windows::Forms::Label());
			this->termo = (gcnew System::Windows::Forms::ProgressBar());
			this->numKbd = (gcnew Pos::Desktop::NumKbd());
			this->m3 = (gcnew System::Windows::Forms::Label());
			this->m2 = (gcnew System::Windows::Forms::Label());
			this->visorCnt = (gcnew System::Windows::Forms::Label());
			this->visorDer = (gcnew System::Windows::Forms::Label());
			this->visorMulti = (gcnew System::Windows::Forms::Label());
			this->visorTot = (gcnew System::Windows::Forms::Label());
			this->abajo = (gcnew System::Windows::Forms::Button());
			this->arriba = (gcnew System::Windows::Forms::Button());
			this->Grid = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->btModo = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btRed = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btConsulta = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btTF = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btLista = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btVendedor = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btEspera = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btCajero = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->logoBitnet))->BeginInit();
			this->termoPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->BeginInit();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// lbLimFiscal
			// 
			this->lbLimFiscal->BackColor = System::Drawing::Color::Transparent;
			this->lbLimFiscal->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbLimFiscal->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbLimFiscal->Location = System::Drawing::Point(10, 124);
			this->lbLimFiscal->Name = L"lbLimFiscal";
			this->lbLimFiscal->Size = System::Drawing::Size(200, 16);
			this->lbLimFiscal->TabIndex = 40;
			this->lbLimFiscal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbCaja
			// 
			this->lbCaja->BackColor = System::Drawing::Color::Transparent;
			this->lbCaja->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCaja->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbCaja->Location = System::Drawing::Point(16, 108);
			this->lbCaja->Name = L"lbCaja";
			this->lbCaja->Size = System::Drawing::Size(154, 16);
			this->lbCaja->TabIndex = 39;
			this->lbCaja->Text = L"Caja Nro: 12";
			this->lbCaja->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbVersion
			// 
			this->lbVersion->BackColor = System::Drawing::Color::Transparent;
			this->lbVersion->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbVersion->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbVersion->Location = System::Drawing::Point(18, 90);
			this->lbVersion->Name = L"lbVersion";
			this->lbVersion->Size = System::Drawing::Size(154, 16);
			this->lbVersion->TabIndex = 38;
			this->lbVersion->Text = L"(Version 1.0.2345)";
			this->lbVersion->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbBitnetwww
			// 
			this->lbBitnetwww->AutoSize = true;
			this->lbBitnetwww->BackColor = System::Drawing::Color::Transparent;
			this->lbBitnetwww->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbBitnetwww->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbBitnetwww->Location = System::Drawing::Point(5, 74);
			this->lbBitnetwww->Name = L"lbBitnetwww";
			this->lbBitnetwww->Size = System::Drawing::Size(200, 13);
			this->lbBitnetwww->TabIndex = 37;
			this->lbBitnetwww->Text = L"www.supermercadoscomodin.com";
			// 
			// logoBitnet
			// 
			this->logoBitnet->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(124)), static_cast<System::Int32>(static_cast<System::Byte>(156)), 
				static_cast<System::Int32>(static_cast<System::Byte>(207)));
			this->logoBitnet->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"logoBitnet.Image")));
			this->logoBitnet->Location = System::Drawing::Point(48, 5);
			this->logoBitnet->Name = L"logoBitnet";
			this->logoBitnet->Size = System::Drawing::Size(80, 80);
			this->logoBitnet->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->logoBitnet->TabIndex = 36;
			this->logoBitnet->TabStop = false;
			// 
			// m1
			// 
			this->m1->AutoSize = true;
			this->m1->BackColor = System::Drawing::Color::Transparent;
			this->m1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m1->ForeColor = System::Drawing::Color::Navy;
			this->m1->Location = System::Drawing::Point(5, 194);
			this->m1->Name = L"m1";
			this->m1->Size = System::Drawing::Size(90, 18);
			this->m1->TabIndex = 41;
			this->m1->Text = L"C/C: 123 - ";
			this->m1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// termoPanel
			// 
			this->termoPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->termoPanel->Controls->Add(this->termoMsg);
			this->termoPanel->Controls->Add(this->termo);
			this->termoPanel->Location = System::Drawing::Point(3, 260);
			this->termoPanel->Name = L"termoPanel";
			this->termoPanel->Size = System::Drawing::Size(171, 38);
			this->termoPanel->TabIndex = 45;
			this->termoPanel->Visible = false;
			// 
			// termoMsg
			// 
			this->termoMsg->AutoSize = true;
			this->termoMsg->BackColor = System::Drawing::Color::Transparent;
			this->termoMsg->Font = (gcnew System::Drawing::Font(L"Tahoma", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->termoMsg->ForeColor = System::Drawing::Color::Navy;
			this->termoMsg->Location = System::Drawing::Point(4, 2);
			this->termoMsg->Name = L"termoMsg";
			this->termoMsg->Size = System::Drawing::Size(89, 13);
			this->termoMsg->TabIndex = 28;
			this->termoMsg->Text = L"Máximo: 12.50";
			this->termoMsg->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// termo
			// 
			this->termo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->termo->Location = System::Drawing::Point(7, 18);
			this->termo->Name = L"termo";
			this->termo->Size = System::Drawing::Size(151, 16);
			this->termo->TabIndex = 27;
			this->termo->Value = 50;
			// 
			// numKbd
			// 
			this->numKbd->BackColor = System::Drawing::Color::LightSteelBlue;
			this->numKbd->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->numKbd->Location = System::Drawing::Point(3, 300);
			this->numKbd->Name = L"numKbd";
			this->numKbd->Size = System::Drawing::Size(170, 271);
			this->numKbd->TabIndex = 44;
			// 
			// m3
			// 
			this->m3->AutoSize = true;
			this->m3->BackColor = System::Drawing::Color::Transparent;
			this->m3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m3->ForeColor = System::Drawing::Color::Navy;
			this->m3->Location = System::Drawing::Point(5, 234);
			this->m3->Name = L"m3";
			this->m3->Size = System::Drawing::Size(120, 18);
			this->m3->TabIndex = 43;
			this->m3->Text = L"Máximo: 12.50";
			this->m3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// m2
			// 
			this->m2->AutoSize = true;
			this->m2->BackColor = System::Drawing::Color::Transparent;
			this->m2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m2->ForeColor = System::Drawing::Color::Navy;
			this->m2->Location = System::Drawing::Point(5, 214);
			this->m2->Name = L"m2";
			this->m2->Size = System::Drawing::Size(104, 18);
			this->m2->TabIndex = 42;
			this->m2->Text = L"Saldo: 12.50";
			this->m2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// visorCnt
			// 
			this->visorCnt->BackColor = System::Drawing::Color::Transparent;
			this->visorCnt->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorCnt->ForeColor = System::Drawing::Color::Green;
			this->visorCnt->Location = System::Drawing::Point(545, 91);
			this->visorCnt->Name = L"visorCnt";
			this->visorCnt->Size = System::Drawing::Size(238, 23);
			this->visorCnt->TabIndex = 49;
			this->visorCnt->Text = L"1.234";
			this->visorCnt->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// visorDer
			// 
			this->visorDer->AutoSize = true;
			this->visorDer->BackColor = System::Drawing::Color::Transparent;
			this->visorDer->Font = (gcnew System::Drawing::Font(L"Tahoma", 20, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorDer->ForeColor = System::Drawing::Color::Navy;
			this->visorDer->Location = System::Drawing::Point(695, 29);
			this->visorDer->Name = L"visorDer";
			this->visorDer->Size = System::Drawing::Size(91, 33);
			this->visorDer->TabIndex = 46;
			this->visorDer->Text = L"1.234";
			this->visorDer->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// visorMulti
			// 
			this->visorMulti->BackColor = System::Drawing::Color::Transparent;
			this->visorMulti->Font = (gcnew System::Drawing::Font(L"Tahoma", 20, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorMulti->ForeColor = System::Drawing::Color::Navy;
			this->visorMulti->Location = System::Drawing::Point(296, 28);
			this->visorMulti->Name = L"visorMulti";
			this->visorMulti->Size = System::Drawing::Size(341, 33);
			this->visorMulti->TabIndex = 48;
			this->visorMulti->Text = L"1.234";
			this->visorMulti->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// visorTot
			// 
			this->visorTot->BackColor = System::Drawing::Color::Transparent;
			this->visorTot->Font = (gcnew System::Drawing::Font(L"Tahoma", 21.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorTot->ForeColor = System::Drawing::Color::Green;
			this->visorTot->Location = System::Drawing::Point(296, 84);
			this->visorTot->Name = L"visorTot";
			this->visorTot->Size = System::Drawing::Size(449, 35);
			this->visorTot->TabIndex = 47;
			this->visorTot->Text = L"1.234";
			this->visorTot->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// abajo
			// 
			this->abajo->BackColor = System::Drawing::Color::Transparent;
			this->abajo->FlatAppearance->BorderSize = 0;
			this->abajo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->abajo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->abajo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"abajo.Image")));
			this->abajo->Location = System::Drawing::Point(229, 253);
			this->abajo->Name = L"abajo";
			this->abajo->Size = System::Drawing::Size(50, 48);
			this->abajo->TabIndex = 52;
			this->abajo->Tag = L"65";
			this->abajo->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->abajo->UseVisualStyleBackColor = false;
			// 
			// arriba
			// 
			this->arriba->BackColor = System::Drawing::Color::Transparent;
			this->arriba->FlatAppearance->BorderSize = 0;
			this->arriba->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->arriba->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->arriba->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"arriba.Image")));
			this->arriba->Location = System::Drawing::Point(229, 199);
			this->arriba->Name = L"arriba";
			this->arriba->Size = System::Drawing::Size(50, 48);
			this->arriba->TabIndex = 51;
			this->arriba->Tag = L"65";
			this->arriba->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->arriba->UseVisualStyleBackColor = false;
			// 
			// Grid
			// 
			this->Grid->AllowUserToAddRows = false;
			this->Grid->AllowUserToDeleteRows = false;
			this->Grid->AllowUserToResizeColumns = false;
			this->Grid->AllowUserToResizeRows = false;
			this->Grid->BackgroundColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->Grid->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {this->Column1, this->Column2, 
				this->Column3});
			this->Grid->Location = System::Drawing::Point(285, 153);
			this->Grid->Name = L"Grid";
			this->Grid->ReadOnly = true;
			dataGridViewCellStyle2->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			dataGridViewCellStyle2->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle2->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle2->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->Grid->RowHeadersDefaultCellStyle = dataGridViewCellStyle2;
			this->Grid->RowHeadersWidth = 10;
			this->Grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->Grid->Size = System::Drawing::Size(512, 250);
			this->Grid->TabIndex = 50;
			// 
			// Column1
			// 
			this->Column1->HeaderText = L"Column1";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			// 
			// Column2
			// 
			this->Column2->HeaderText = L"Column2";
			this->Column2->Name = L"Column2";
			this->Column2->ReadOnly = true;
			// 
			// Column3
			// 
			this->Column3->HeaderText = L"Column3";
			this->Column3->Name = L"Column3";
			this->Column3->ReadOnly = true;
			// 
			// statusStrip1
			// 
			this->statusStrip1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->btModo, this->btRed, 
				this->btConsulta, this->btTF, this->btLista, this->btVendedor, this->btEspera, this->btCajero});
			this->statusStrip1->Location = System::Drawing::Point(0, 578);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(800, 22);
			this->statusStrip1->TabIndex = 53;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// btModo
			// 
			this->btModo->AutoSize = false;
			this->btModo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btModo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btModo->Name = L"btModo";
			this->btModo->Size = System::Drawing::Size(74, 17);
			this->btModo->Text = L"Modo REG";
			// 
			// btRed
			// 
			this->btRed->AutoSize = false;
			this->btRed->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btRed->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btRed->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btRed->Name = L"btRed";
			this->btRed->Size = System::Drawing::Size(74, 17);
			this->btRed->Text = L"Online";
			// 
			// btConsulta
			// 
			this->btConsulta->AutoSize = false;
			this->btConsulta->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btConsulta->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btConsulta->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btConsulta->Name = L"btConsulta";
			this->btConsulta->Size = System::Drawing::Size(74, 17);
			this->btConsulta->Text = L"Consulta";
			// 
			// btTF
			// 
			this->btTF->AutoSize = false;
			this->btTF->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btTF->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btTF->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btTF->Name = L"btTF";
			this->btTF->Size = System::Drawing::Size(74, 17);
			this->btTF->Text = L"T/ Factura";
			// 
			// btLista
			// 
			this->btLista->AutoSize = false;
			this->btLista->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btLista->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btLista->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btLista->Name = L"btLista";
			this->btLista->Size = System::Drawing::Size(74, 17);
			this->btLista->Text = L"Precio";
			// 
			// btVendedor
			// 
			this->btVendedor->AutoSize = false;
			this->btVendedor->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btVendedor->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btVendedor->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btVendedor->Name = L"btVendedor";
			this->btVendedor->Size = System::Drawing::Size(150, 17);
			this->btVendedor->Text = L"12/10/2009 15:25";
			// 
			// btEspera
			// 
			this->btEspera->AutoSize = false;
			this->btEspera->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btEspera->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold));
			this->btEspera->ForeColor = System::Drawing::Color::Red;
			this->btEspera->Name = L"btEspera";
			this->btEspera->Size = System::Drawing::Size(100, 17);
			this->btEspera->Text = L"Ticket Espera";
			// 
			// btCajero
			// 
			this->btCajero->AutoSize = false;
			this->btCajero->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btCajero->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btCajero->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btCajero->Name = L"btCajero";
			this->btCajero->Size = System::Drawing::Size(150, 17);
			this->btCajero->Text = L"Caj: Juan Puca";
			// 
			// frmTouchAuto
			// 
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(124)), static_cast<System::Int32>(static_cast<System::Byte>(156)), 
				static_cast<System::Int32>(static_cast<System::Byte>(207)));
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(800, 600);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->abajo);
			this->Controls->Add(this->arriba);
			this->Controls->Add(this->Grid);
			this->Controls->Add(this->visorCnt);
			this->Controls->Add(this->visorDer);
			this->Controls->Add(this->visorMulti);
			this->Controls->Add(this->visorTot);
			this->Controls->Add(this->m1);
			this->Controls->Add(this->termoPanel);
			this->Controls->Add(this->numKbd);
			this->Controls->Add(this->m3);
			this->Controls->Add(this->m2);
			this->Controls->Add(this->lbLimFiscal);
			this->Controls->Add(this->lbCaja);
			this->Controls->Add(this->lbVersion);
			this->Controls->Add(this->lbBitnetwww);
			this->Controls->Add(this->logoBitnet);
			this->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"frmTouchAuto";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->logoBitnet))->EndInit();
			this->termoPanel->ResumeLayout(false);
			this->termoPanel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->EndInit();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
