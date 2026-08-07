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
	/// Summary for BuscaCliSql
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class BuscaCliSql : public System::Windows::Forms::Form, Controles::IBuscador
	{

		String ^seekText, ^activeOrder;
		volatile bool seeking;
		int saltear;
		bool mostrandolvw;
	public:
		String ^actualCod;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column3;



			 bool ingtexto;
			 int actualRow;

	public:

		property bool OnlyNumeric;

		BuscaCliSql(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			InitKeyboard();
			Strings::InterceptKeyboard(this, 2);
			seeking = false;
			saltear = 0;
			Grid->AutoGenerateColumns = false;
			actualRow = -1;
			actualCod = "";
			//Grid->CellClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Buscador::ClickCelda);
			//mostrandolvw = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~BuscaCliSql()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	protected: 
	private: System::Windows::Forms::Label^  lbTit;
	private: System::Windows::Forms::Label^  l5;
	private: System::Windows::Forms::Label^  l4;
	private: System::Windows::Forms::Label^  l3;
	private: System::Windows::Forms::Label^  l2;
	private: System::Windows::Forms::Label^  l1;
	private: System::Windows::Forms::TextBox^  tbAyuda;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  cancelar;
	private: System::Windows::Forms::Button^  ok;
	private: System::Windows::Forms::Button^  pagArriba;
	private: System::Windows::Forms::Button^  pagAbajo;
	private: System::Windows::Forms::Button^  filaAbajo;
	private: System::Windows::Forms::Button^  filaArriba;
	private: System::Windows::Forms::Button^  b3;
	private: System::Windows::Forms::Button^  b6;
	private: System::Windows::Forms::Button^  b9;
	private: System::Windows::Forms::Button^  bbksp;
	private: System::Windows::Forms::Button^  b2;
	private: System::Windows::Forms::Button^  b5;
	private: System::Windows::Forms::Button^  b8;
	private: System::Windows::Forms::Button^  b1;
	private: System::Windows::Forms::Button^  b4;
	private: System::Windows::Forms::Button^  b7;
	private: System::Windows::Forms::Button^  b0;
	public: System::Windows::Forms::DataGridView^  Grid;
	private: 

	public: 
	private: System::Windows::Forms::Label^  box;
	private: System::Windows::Forms::ImageList^  img;
	private: System::Windows::Forms::Timer^  kbTimer;
	private: System::Windows::Forms::Timer^  timerSeek;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(BuscaCliSql::typeid));
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->lbTit = (gcnew System::Windows::Forms::Label());
			this->l5 = (gcnew System::Windows::Forms::Label());
			this->l4 = (gcnew System::Windows::Forms::Label());
			this->l3 = (gcnew System::Windows::Forms::Label());
			this->l2 = (gcnew System::Windows::Forms::Label());
			this->l1 = (gcnew System::Windows::Forms::Label());
			this->tbAyuda = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->cancelar = (gcnew System::Windows::Forms::Button());
			this->ok = (gcnew System::Windows::Forms::Button());
			this->pagArriba = (gcnew System::Windows::Forms::Button());
			this->pagAbajo = (gcnew System::Windows::Forms::Button());
			this->filaAbajo = (gcnew System::Windows::Forms::Button());
			this->filaArriba = (gcnew System::Windows::Forms::Button());
			this->b3 = (gcnew System::Windows::Forms::Button());
			this->img = (gcnew System::Windows::Forms::ImageList(this->components));
			this->b6 = (gcnew System::Windows::Forms::Button());
			this->b9 = (gcnew System::Windows::Forms::Button());
			this->bbksp = (gcnew System::Windows::Forms::Button());
			this->b2 = (gcnew System::Windows::Forms::Button());
			this->b5 = (gcnew System::Windows::Forms::Button());
			this->b8 = (gcnew System::Windows::Forms::Button());
			this->b1 = (gcnew System::Windows::Forms::Button());
			this->b4 = (gcnew System::Windows::Forms::Button());
			this->b7 = (gcnew System::Windows::Forms::Button());
			this->b0 = (gcnew System::Windows::Forms::Button());
			this->Grid = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->box = (gcnew System::Windows::Forms::Label());
			this->kbTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->timerSeek = (gcnew System::Windows::Forms::Timer(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(18, 2);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(32, 32);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 65;
			this->pictureBox1->TabStop = false;
			// 
			// lbTit
			// 
			this->lbTit->AutoSize = true;
			this->lbTit->Font = (gcnew System::Drawing::Font(L"Tahoma", 14, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbTit->ForeColor = System::Drawing::Color::Navy;
			this->lbTit->Location = System::Drawing::Point(57, 7);
			this->lbTit->Name = L"lbTit";
			this->lbTit->Size = System::Drawing::Size(313, 23);
			this->lbTit->TabIndex = 64;
			this->lbTit->Text = L"Buscar Artículos por su Nombre";
			// 
			// l5
			// 
			this->l5->AutoSize = true;
			this->l5->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->l5->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l5->Location = System::Drawing::Point(426, 400);
			this->l5->Name = L"l5";
			this->l5->Size = System::Drawing::Size(18, 16);
			this->l5->TabIndex = 62;
			this->l5->Text = L"A";
			this->l5->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->l5->Visible = false;
			// 
			// l4
			// 
			this->l4->AutoSize = true;
			this->l4->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->l4->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l4->Location = System::Drawing::Point(412, 400);
			this->l4->Name = L"l4";
			this->l4->Size = System::Drawing::Size(18, 16);
			this->l4->TabIndex = 61;
			this->l4->Text = L"A";
			this->l4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->l4->Visible = false;
			// 
			// l3
			// 
			this->l3->AutoSize = true;
			this->l3->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->l3->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l3->Location = System::Drawing::Point(398, 400);
			this->l3->Name = L"l3";
			this->l3->Size = System::Drawing::Size(18, 16);
			this->l3->TabIndex = 60;
			this->l3->Text = L"A";
			this->l3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->l3->Visible = false;
			// 
			// l2
			// 
			this->l2->AutoSize = true;
			this->l2->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->l2->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l2->Location = System::Drawing::Point(384, 400);
			this->l2->Name = L"l2";
			this->l2->Size = System::Drawing::Size(18, 16);
			this->l2->TabIndex = 59;
			this->l2->Text = L"A";
			this->l2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->l2->Visible = false;
			// 
			// l1
			// 
			this->l1->AutoSize = true;
			this->l1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->l1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l1->Location = System::Drawing::Point(370, 400);
			this->l1->Name = L"l1";
			this->l1->Size = System::Drawing::Size(18, 16);
			this->l1->TabIndex = 58;
			this->l1->Text = L"A";
			this->l1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->l1->Visible = false;
			// 
			// tbAyuda
			// 
			this->tbAyuda->BackColor = System::Drawing::Color::White;
			this->tbAyuda->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbAyuda->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbAyuda->Location = System::Drawing::Point(139, 44);
			this->tbAyuda->Name = L"tbAyuda";
			this->tbAyuda->ReadOnly = true;
			this->tbAyuda->Size = System::Drawing::Size(610, 23);
			this->tbAyuda->TabIndex = 57;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(14, 47);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(119, 16);
			this->label1->TabIndex = 56;
			this->label1->Text = L"Texto Ingresado:";
			// 
			// cancelar
			// 
			this->cancelar->BackColor = System::Drawing::Color::Silver;
			this->cancelar->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cancelar->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->cancelar->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"cancelar.Image")));
			this->cancelar->Location = System::Drawing::Point(566, 538);
			this->cancelar->Name = L"cancelar";
			this->cancelar->Size = System::Drawing::Size(224, 48);
			this->cancelar->TabIndex = 55;
			this->cancelar->UseVisualStyleBackColor = false;
			this->cancelar->Click += gcnew System::EventHandler(this, &BuscaCliSql::Cancelar_Click);
			// 
			// ok
			// 
			this->ok->BackColor = System::Drawing::Color::Silver;
			this->ok->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ok->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->ok->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"ok.Image")));
			this->ok->Location = System::Drawing::Point(333, 538);
			this->ok->Name = L"ok";
			this->ok->Size = System::Drawing::Size(224, 48);
			this->ok->TabIndex = 54;
			this->ok->UseVisualStyleBackColor = false;
			this->ok->Click += gcnew System::EventHandler(this, &BuscaCliSql::ok_Click);
			// 
			// pagArriba
			// 
			this->pagArriba->BackColor = System::Drawing::Color::Transparent;
			this->pagArriba->FlatAppearance->BorderSize = 0;
			this->pagArriba->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->pagArriba->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->pagArriba->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->pagArriba->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pagArriba.Image")));
			this->pagArriba->Location = System::Drawing::Point(566, 385);
			this->pagArriba->Name = L"pagArriba";
			this->pagArriba->Size = System::Drawing::Size(50, 48);
			this->pagArriba->TabIndex = 53;
			this->pagArriba->Tag = L"-8";
			this->pagArriba->UseVisualStyleBackColor = false;
			// 
			// pagAbajo
			// 
			this->pagAbajo->BackColor = System::Drawing::Color::Transparent;
			this->pagAbajo->FlatAppearance->BorderSize = 0;
			this->pagAbajo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->pagAbajo->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->pagAbajo->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->pagAbajo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pagAbajo.Image")));
			this->pagAbajo->Location = System::Drawing::Point(719, 385);
			this->pagAbajo->Name = L"pagAbajo";
			this->pagAbajo->Size = System::Drawing::Size(50, 48);
			this->pagAbajo->TabIndex = 52;
			this->pagAbajo->Tag = L"8";
			this->pagAbajo->UseVisualStyleBackColor = false;
			// 
			// filaAbajo
			// 
			this->filaAbajo->BackColor = System::Drawing::Color::Transparent;
			this->filaAbajo->FlatAppearance->BorderSize = 0;
			this->filaAbajo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->filaAbajo->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->filaAbajo->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->filaAbajo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"filaAbajo.Image")));
			this->filaAbajo->Location = System::Drawing::Point(668, 385);
			this->filaAbajo->Name = L"filaAbajo";
			this->filaAbajo->Size = System::Drawing::Size(50, 48);
			this->filaAbajo->TabIndex = 51;
			this->filaAbajo->Tag = L"1";
			this->filaAbajo->UseVisualStyleBackColor = false;
			// 
			// filaArriba
			// 
			this->filaArriba->BackColor = System::Drawing::Color::Transparent;
			this->filaArriba->FlatAppearance->BorderSize = 0;
			this->filaArriba->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->filaArriba->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->filaArriba->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->filaArriba->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"filaArriba.Image")));
			this->filaArriba->Location = System::Drawing::Point(617, 385);
			this->filaArriba->Name = L"filaArriba";
			this->filaArriba->Size = System::Drawing::Size(50, 48);
			this->filaArriba->TabIndex = 50;
			this->filaArriba->Tag = L"-1";
			this->filaArriba->UseVisualStyleBackColor = false;
			// 
			// b3
			// 
			this->b3->BackColor = System::Drawing::Color::Silver;
			this->b3->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b3->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b3->ImageIndex = 4;
			this->b3->ImageList = this->img;
			this->b3->Location = System::Drawing::Point(215, 391);
			this->b3->Name = L"b3";
			this->b3->Size = System::Drawing::Size(50, 48);
			this->b3->TabIndex = 49;
			this->b3->Tag = L"DEF3";
			this->b3->UseVisualStyleBackColor = false;
			this->b3->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// img
			// 
			this->img->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"img.ImageStream")));
			this->img->TransparentColor = System::Drawing::Color::Transparent;
			this->img->Images->SetKeyName(0, L"btnmult.png");
			this->img->Images->SetKeyName(1, L"btn0.png");
			this->img->Images->SetKeyName(2, L"btn1.png");
			this->img->Images->SetKeyName(3, L"btn2a.png");
			this->img->Images->SetKeyName(4, L"btn3a.png");
			this->img->Images->SetKeyName(5, L"btn4a.png");
			this->img->Images->SetKeyName(6, L"btn5a.png");
			this->img->Images->SetKeyName(7, L"btn6a.png");
			this->img->Images->SetKeyName(8, L"btn7a.png");
			this->img->Images->SetKeyName(9, L"btn8a.png");
			this->img->Images->SetKeyName(10, L"btn9a.png");
			this->img->Images->SetKeyName(11, L"btnback.png");
			this->img->Images->SetKeyName(12, L"btn2.png");
			this->img->Images->SetKeyName(13, L"btn3.png");
			this->img->Images->SetKeyName(14, L"btn4.png");
			this->img->Images->SetKeyName(15, L"btn5.png");
			this->img->Images->SetKeyName(16, L"btn6.png");
			this->img->Images->SetKeyName(17, L"btn7.png");
			this->img->Images->SetKeyName(18, L"btn8.png");
			this->img->Images->SetKeyName(19, L"btn9.png");
			// 
			// b6
			// 
			this->b6->BackColor = System::Drawing::Color::Silver;
			this->b6->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b6->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b6->ImageIndex = 7;
			this->b6->ImageList = this->img;
			this->b6->Location = System::Drawing::Point(215, 440);
			this->b6->Name = L"b6";
			this->b6->Size = System::Drawing::Size(50, 48);
			this->b6->TabIndex = 48;
			this->b6->Tag = L"MNO6";
			this->b6->UseVisualStyleBackColor = false;
			this->b6->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b9
			// 
			this->b9->BackColor = System::Drawing::Color::Silver;
			this->b9->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b9->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b9->ImageIndex = 10;
			this->b9->ImageList = this->img;
			this->b9->Location = System::Drawing::Point(215, 489);
			this->b9->Name = L"b9";
			this->b9->Size = System::Drawing::Size(50, 48);
			this->b9->TabIndex = 47;
			this->b9->Tag = L"WXYZ9";
			this->b9->UseVisualStyleBackColor = false;
			this->b9->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// bbksp
			// 
			this->bbksp->BackColor = System::Drawing::Color::Silver;
			this->bbksp->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->bbksp->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->bbksp->ImageIndex = 11;
			this->bbksp->ImageList = this->img;
			this->bbksp->Location = System::Drawing::Point(162, 538);
			this->bbksp->Name = L"bbksp";
			this->bbksp->Size = System::Drawing::Size(103, 48);
			this->bbksp->TabIndex = 46;
			this->bbksp->UseVisualStyleBackColor = false;
			this->bbksp->Click += gcnew System::EventHandler(this, &BuscaCliSql::bbksp_Click);
			// 
			// b2
			// 
			this->b2->BackColor = System::Drawing::Color::Silver;
			this->b2->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b2->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b2->ImageIndex = 3;
			this->b2->ImageList = this->img;
			this->b2->Location = System::Drawing::Point(162, 391);
			this->b2->Name = L"b2";
			this->b2->Size = System::Drawing::Size(50, 48);
			this->b2->TabIndex = 45;
			this->b2->Tag = L"ABC2";
			this->b2->UseVisualStyleBackColor = false;
			this->b2->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b5
			// 
			this->b5->BackColor = System::Drawing::Color::Silver;
			this->b5->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b5->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b5->ImageIndex = 6;
			this->b5->ImageList = this->img;
			this->b5->Location = System::Drawing::Point(162, 440);
			this->b5->Name = L"b5";
			this->b5->Size = System::Drawing::Size(50, 48);
			this->b5->TabIndex = 44;
			this->b5->Tag = L"JKL5";
			this->b5->UseVisualStyleBackColor = false;
			this->b5->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b8
			// 
			this->b8->BackColor = System::Drawing::Color::Silver;
			this->b8->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b8->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b8->ImageIndex = 9;
			this->b8->ImageList = this->img;
			this->b8->Location = System::Drawing::Point(162, 489);
			this->b8->Name = L"b8";
			this->b8->Size = System::Drawing::Size(50, 48);
			this->b8->TabIndex = 43;
			this->b8->Tag = L"TUV8";
			this->b8->UseVisualStyleBackColor = false;
			this->b8->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b1
			// 
			this->b1->BackColor = System::Drawing::Color::Silver;
			this->b1->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b1->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b1->ImageIndex = 2;
			this->b1->ImageList = this->img;
			this->b1->Location = System::Drawing::Point(110, 391);
			this->b1->Name = L"b1";
			this->b1->Size = System::Drawing::Size(50, 48);
			this->b1->TabIndex = 42;
			this->b1->Tag = L"1.-";
			this->b1->UseVisualStyleBackColor = false;
			this->b1->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b4
			// 
			this->b4->BackColor = System::Drawing::Color::Silver;
			this->b4->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b4->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b4->ImageIndex = 5;
			this->b4->ImageList = this->img;
			this->b4->Location = System::Drawing::Point(110, 440);
			this->b4->Name = L"b4";
			this->b4->Size = System::Drawing::Size(50, 48);
			this->b4->TabIndex = 41;
			this->b4->Tag = L"GHI4";
			this->b4->UseVisualStyleBackColor = false;
			this->b4->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b7
			// 
			this->b7->BackColor = System::Drawing::Color::Silver;
			this->b7->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b7->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b7->ImageIndex = 8;
			this->b7->ImageList = this->img;
			this->b7->Location = System::Drawing::Point(110, 489);
			this->b7->Name = L"b7";
			this->b7->Size = System::Drawing::Size(50, 48);
			this->b7->TabIndex = 40;
			this->b7->Tag = L"PQRS7";
			this->b7->UseVisualStyleBackColor = false;
			this->b7->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// b0
			// 
			this->b0->BackColor = System::Drawing::Color::Silver;
			this->b0->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->b0->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->b0->ImageIndex = 1;
			this->b0->ImageList = this->img;
			this->b0->Location = System::Drawing::Point(110, 538);
			this->b0->Name = L"b0";
			this->b0->Size = System::Drawing::Size(50, 48);
			this->b0->TabIndex = 39;
			this->b0->Tag = L" 0";
			this->b0->UseVisualStyleBackColor = false;
			this->b0->Click += gcnew System::EventHandler(this, &BuscaCliSql::CellButton_Click);
			// 
			// Grid
			// 
			this->Grid->AllowUserToAddRows = false;
			this->Grid->AllowUserToDeleteRows = false;
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->Grid->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->Grid->ColumnHeadersHeight = 35;
			this->Grid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::DisableResizing;
			this->Grid->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {this->Column1, this->Column2, 
				this->Column3});
			this->Grid->Location = System::Drawing::Point(4, 73);
			this->Grid->MultiSelect = false;
			this->Grid->Name = L"Grid";
			this->Grid->ReadOnly = true;
			this->Grid->RowHeadersWidth = 10;
			this->Grid->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->Grid->RowTemplate->Height = 35;
			this->Grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->Grid->Size = System::Drawing::Size(791, 312);
			this->Grid->TabIndex = 38;
			// 
			// Column1
			// 
			this->Column1->DataPropertyName = L"cod";
			this->Column1->HeaderText = L"Codigo";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			this->Column1->Width = 200;
			// 
			// Column2
			// 
			this->Column2->DataPropertyName = L"Nombre";
			this->Column2->HeaderText = L"Nombre";
			this->Column2->Name = L"Column2";
			this->Column2->ReadOnly = true;
			this->Column2->Width = 360;
			// 
			// Column3
			// 
			this->Column3->DataPropertyName = L"Cuit";
			this->Column3->HeaderText = L"CUIT";
			this->Column3->Name = L"Column3";
			this->Column3->ReadOnly = true;
			this->Column3->Width = 200;
			// 
			// box
			// 
			this->box->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->box->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->box->Location = System::Drawing::Point(326, 391);
			this->box->Name = L"box";
			this->box->Size = System::Drawing::Size(234, 35);
			this->box->TabIndex = 63;
			// 
			// kbTimer
			// 
			this->kbTimer->Interval = 1000;
			this->kbTimer->Tick += gcnew System::EventHandler(this, &BuscaCliSql::kbTimer_Tick);
			// 
			// timerSeek
			// 
			this->timerSeek->Interval = 200;
			this->timerSeek->Tick += gcnew System::EventHandler(this, &BuscaCliSql::timerSeek_Tick);
			// 
			// BuscaCliSql
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->ClientSize = System::Drawing::Size(800, 600);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->lbTit);
			this->Controls->Add(this->l5);
			this->Controls->Add(this->l4);
			this->Controls->Add(this->l3);
			this->Controls->Add(this->l2);
			this->Controls->Add(this->l1);
			this->Controls->Add(this->tbAyuda);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cancelar);
			this->Controls->Add(this->ok);
			this->Controls->Add(this->pagArriba);
			this->Controls->Add(this->pagAbajo);
			this->Controls->Add(this->filaAbajo);
			this->Controls->Add(this->filaArriba);
			this->Controls->Add(this->b3);
			this->Controls->Add(this->b6);
			this->Controls->Add(this->b9);
			this->Controls->Add(this->bbksp);
			this->Controls->Add(this->b2);
			this->Controls->Add(this->b5);
			this->Controls->Add(this->b8);
			this->Controls->Add(this->b1);
			this->Controls->Add(this->b4);
			this->Controls->Add(this->b7);
			this->Controls->Add(this->b0);
			this->Controls->Add(this->Grid);
			this->Controls->Add(this->box);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"BuscaCliSql";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"BuscaCliSql";
			this->Load += gcnew System::EventHandler(this, &BuscaCliSql::BuscaCliSql_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

public:

	virtual void AcceptKey(Keys key)
	{
		bool aceptar = true;
		switch(key)
		{
		case Keys::A: tbAyuda->Text += "A"; ingtexto = true; break;
		case Keys::B: tbAyuda->Text += "B"; ingtexto = true; break;
		case Keys::C: tbAyuda->Text += "C"; ingtexto = true; break;
		case Keys::D: tbAyuda->Text += "D"; ingtexto = true; break;
		case Keys::E: tbAyuda->Text += "E"; ingtexto = true; break;
		case Keys::F: tbAyuda->Text += "F"; ingtexto = true; break;
		case Keys::G: tbAyuda->Text += "G"; ingtexto = true; break;
		case Keys::H: tbAyuda->Text += "H"; ingtexto = true; break;
		case Keys::I: tbAyuda->Text += "I"; ingtexto = true; break;
		case Keys::J: tbAyuda->Text += "J"; ingtexto = true; break;
		case Keys::K: tbAyuda->Text += "K"; ingtexto = true; break;
		case Keys::L: tbAyuda->Text += "L"; ingtexto = true; break;
		case Keys::M: tbAyuda->Text += "M"; ingtexto = true; break;
		case Keys::N: tbAyuda->Text += "N"; ingtexto = true; break;
		case Keys::O: tbAyuda->Text += "O"; ingtexto = true; break;
		case Keys::P: tbAyuda->Text += "P"; ingtexto = true; break;
		case Keys::Q: tbAyuda->Text += "Q"; ingtexto = true; break;
		case Keys::R: tbAyuda->Text += "R"; ingtexto = true; break;
		case Keys::S: tbAyuda->Text += "S"; ingtexto = true; break;
		case Keys::T: tbAyuda->Text += "T"; ingtexto = true; break;
		case Keys::U: tbAyuda->Text += "U"; ingtexto = true; break;
		case Keys::V: tbAyuda->Text += "V"; ingtexto = true; break;
		case Keys::W: tbAyuda->Text += "W"; ingtexto = true; break;
		case Keys::X: tbAyuda->Text += "X"; ingtexto = true; break;
		case Keys::Y: tbAyuda->Text += "Y"; ingtexto = true; break;
		case Keys::Z: tbAyuda->Text += "Z"; ingtexto = true; break;

		case Keys::D0: case Keys::NumPad0: tbAyuda->Text += "0"; ingtexto = true; break;
		case Keys::D1: case Keys::NumPad1: tbAyuda->Text += "1"; ingtexto = true; break;
		case Keys::D2: case Keys::NumPad2: tbAyuda->Text += "2"; ingtexto = true; break;
		case Keys::D3: case Keys::NumPad3: tbAyuda->Text += "3"; ingtexto = true; break;
		case Keys::D4: case Keys::NumPad4: tbAyuda->Text += "4"; ingtexto = true; break;
		case Keys::D5: case Keys::NumPad5: tbAyuda->Text += "5"; ingtexto = true; break;
		case Keys::D6: case Keys::NumPad6: tbAyuda->Text += "6"; ingtexto = true; break;
		case Keys::D7: case Keys::NumPad7: tbAyuda->Text += "7"; ingtexto = true; break;
		case Keys::D8: case Keys::NumPad8: tbAyuda->Text += "8"; ingtexto = true; break;
		case Keys::D9: case Keys::NumPad9: tbAyuda->Text += "9"; ingtexto = true; break;

		case Keys::Space: tbAyuda->Text += " "; ingtexto = true; break;

		case Keys::Back:
		case Keys::Left:
			if(tbAyuda->Text->Length > 0)
				tbAyuda->Text = tbAyuda->Text->Substring(0, tbAyuda->Text->Length - 1);
			else
				aceptar = false;
			ingtexto = false;
			break;

		case Keys::Down:
			Saltear(1);
			aceptar = false;
			ingtexto = false;
			break;

		case Keys::Up:
			Saltear(-1);
			aceptar = false;
			ingtexto = false;
			break;

		case Keys::PageDown:
			Saltear(10);
			aceptar = false;
			ingtexto = false;
			break;

		case Keys::PageUp:
			Saltear(-10);
			aceptar = false;
			ingtexto = false;
			break;

		case Keys::Escape:
			this->Cancelar_Click(nullptr, nullptr);
			aceptar = false;
			ingtexto = false;
			break;

		default:
			aceptar = false;
			//if(key == Keys::Enter && tbAyuda->Text->Length < 13)
			if(key == Keys::Enter)
			{
				//tbAyuda->Text += "0";
				selIndex = -1;
				Buscar();
			}
			//ingtexto = (this->ActiveOrder == "mscansc") ? ingtexto : false;				

			bool pulsoEnter = (key == Keys::Enter && enterEnabled && !ingtexto);
			
			if((int)key == 21 || key == Keys::F8 || pulsoEnter)
				this->ok_Click(nullptr, nullptr);
			ingtexto = false;
			break;
		}

		if(aceptar)
		{
			selIndex = -1;
			Buscar();
		}
	}


	void Saltear(int cuanto)
	{
		GCStatic::LockTouch("timer_seek");
		saltear = cuanto;
		seekText = nullptr;
		timerSeek->Start();
		timerSeek->Enabled = true;		
		GCStatic::ReleaseTouch("timer_seek");
	}

	void Buscar()
	{
		GCStatic::LockTouch("timer_seek");
		//if(Completar < 0)
			seekText = tbAyuda->Text->Trim()->ToUpper();
		//else
		//	seekText = tbAyuda->Text->Trim()->ToUpper()->PadLeft(Completar, CompletarCon);		
		timerSeek->Stop();
		actualRow = -1;
		Grid->DataSource = Controles::FuncClientes::BuscaClientePorNom(seekText);
		if (Grid->RowCount > 0)
		{
			actualRow = 0;
			//Grid->CurrentCell = Grid->Rows[0, actualRow];
			Grid->CurrentCell =  Grid->Rows[actualRow]->Cells[0];
			actualCod = Grid->CurrentCell->Value->ToString();
			//actualCod = "";
		}
		timerSeek->Start();
		//timerSeek->Enabled = true;		
		saltear = 0;
		GCStatic::ReleaseTouch("timer_seek");
	}

	System::Void timerSeek_Tick(System::Object^  sender, System::EventArgs^  e) 
	{
		// Espera que termine cualquier proceso anterior
		if(seeking) return;

		// En estas variables se pone el contenido de los campos 'seekText' y 'saltear', a fin de manejar
		// la grilla de una forma previsible en multitarea
		String ^txt;
		int salt;

		GCStatic::LockTouch("timer_seek");
		txt = seekText;
		salt = saltear;
		timerSeek->Enabled = false;
		seeking = true;
		GCStatic::ReleaseTouch("timer_seek");

		if (Grid->RowCount > 0)
		{		
			if ((saltear + actualRow) >= Grid->RowCount)
				actualRow = Grid->RowCount - 1;
			else if ((saltear + actualRow) <= 0)		
				actualRow = 0;		
			else 
				actualRow += saltear;
			Grid->CurrentCell = Grid[0, actualRow];
			actualCod = Grid->CurrentCell->Value->ToString();
			//Grid->Rows[actualRow]->Selected = true;
		}
		else
		{
			actualRow = -1;
			actualCod = "";
		}

		GCStatic::LockTouch("timer_seek2");
		seeking = false;
		GCStatic::ReleaseTouch("timer_seek2");
	}

	private:
		array<Label ^> ^labels;
		String ^availKeys;
		int selIndex;

		void InitKeyboard()
		{
			selIndex = -1;
			labels = gcnew array<Label ^>(5);
			labels[0] = l1;
			labels[1] = l2;
			labels[2] = l3;
			labels[3] = l4;
			labels[4] = l5;
		}

		void DisplayKeys()
		{
			int x = ((box->Width - (22 * availKeys->Length)) / 2) + box->Left;
			for(int i=0; i < 5; i++)
			{
				Label ^l = labels[i];
				if(i < availKeys->Length)
				{
					l->Visible = true;
					l->Text = availKeys[i].ToString();
					l->ForeColor = (i == selIndex)? Color::Red : Color::Blue;
					l->BorderStyle = (i != selIndex)? 
						System::Windows::Forms::BorderStyle::None :
					System::Windows::Forms::BorderStyle::FixedSingle;
					l->Left = x;
					x += 22;
				}
				else
					l->Visible = false;
			}
		}

		void FlushKey()
		{
			if(selIndex < 0 || selIndex >= availKeys->Length)
			{
				selIndex = -1;
				return;
			}

			tbAyuda->Text += availKeys[selIndex].ToString();
			selIndex = -1;
			Buscar();
		}

	System::Void bbksp_Click(System::Object^  sender, System::EventArgs^  e) {
				 if(tbAyuda->Text->Length > 0)
				 {
					 tbAyuda->Text = tbAyuda->Text->Substring(0, tbAyuda->Text->Length - 1);
					 Buscar();
				 }
			 }
	System::Void kbTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
			 kbTimer->Enabled = false;
			 if(selIndex >= 0)
			 {
				 FlushKey();
				 availKeys = String::Empty;
				 DisplayKeys();
			 }
		 }

	System::Void CellButton_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		Button ^boton = (Button ^) sender;
		String ^listKeys = (String ^) boton->Tag;

		kbTimer->Enabled = false;
		if(listKeys != availKeys)
			FlushKey();

		if(selIndex < 0)
		{
			availKeys = listKeys;
			selIndex = 0;
		}
		else
		{
			selIndex++;
			if(selIndex >= availKeys->Length)
				selIndex = 0;
		}
		DisplayKeys();
		if(availKeys->Length == 1)
			FlushKey();
		else
			kbTimer->Enabled = true;
	}

	System::Void BuscaCliSql_Load(System::Object^  sender, System::EventArgs^  e) {

		// Elimina los botones en configuraciones sin touchscreen
		if(!usaTouch)
		{
			b1->Visible = false;
			b2->Visible = false;
			b3->Visible = false;
			b4->Visible = false;
			b5->Visible = false;
			b6->Visible = false;
			b7->Visible = false;
			b8->Visible = false;
			b9->Visible = false;
			b0->Visible = false;
			bbksp->Visible = false;
			l1->Visible = false;
			box->Visible = false;
		}
		else
		{
			if(OnlyNumeric)
			{
				b1->Tag = "1";
				b2->Tag = "2";
				b3->Tag = "3";
				b4->Tag = "4";
				b5->Tag = "5";
				b6->Tag = "6";
				b7->Tag = "7";
				b8->Tag = "8";
				b9->Tag = "9";
				b0->Tag = "0";

				b2->ImageIndex = 12;
				b3->ImageIndex = 13;
				b4->ImageIndex = 14;
				b5->ImageIndex = 15;
				b6->ImageIndex = 16;
				b7->ImageIndex = 17;
				b8->ImageIndex = 18;
				b9->ImageIndex = 19;
			}
		}

			 // Adapta los controles a distintos tipos de pantalla
			 System::Drawing::Font ^xFont;
			 switch(Controles::BaseDialog::TipoPantalla)
			 {
			 case 3:			// No touch - monitor B/W - 800 x 600
				 // Oculta controles que no se usan
				 pagArriba->Visible = false;
				 pagAbajo->Visible = false;
				 filaArriba->Visible = false;
				 filaAbajo->Visible = false;
				 ok->Visible = false;
				 cancelar->Visible = false;

				 // Agranda la grilla.
				 xFont = gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Regular);
				 Grid->Width = this->Width - 8;
				 Grid->DefaultCellStyle->SelectionBackColor = System::Drawing::Color::LightGray;
				 Grid->DefaultCellStyle->SelectionForeColor = System::Drawing::Color::Black;
				 Grid->RowTemplate->Height += 4;
				 Grid->ColumnHeadersDefaultCellStyle->Font =	gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Bold);

				 for(int i=0; i<Grid->ColumnCount; i++)
				 {
					 DataGridViewColumn ^col = Grid->Columns[i];
					 col->Width = (int)((double)col->Width * 1.3);
					 col->DefaultCellStyle->Font = xFont;
				 }
				 break;
			 }

			 // Acomoda un poco la pantalla, si no es version touchscreen
			 if(!b1->Visible)
			 {
				 this->pagArriba->Top = this->cancelar->Top - this->pagArriba->Height - 2;
				 this->filaArriba->Top = this->pagArriba->Top;
				 this->filaAbajo->Top = this->pagArriba->Top;
				 this->pagAbajo->Top = this->pagArriba->Top;			
				 
			 }
			 lbTit->Text = this->Text;
	 }


	System::Void Cancelar_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		actualCod = "";
		WriteBootLog("AVISO - Sale de la pantalla de busqueda con CANCEL");
		this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
		this->Close();
	}

	System::Void ok_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		WriteBootLog("AVISO - Sale de la pantalla de busqueda con OK");
		this->DialogResult = System::Windows::Forms::DialogResult::OK;
		this->Close();
	}
};
}
