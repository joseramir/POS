#pragma once
#ifdef POSXP

#include "ArrayList.h"
#include "Strings.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Resumen de ArqueoTipo1
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class ArqueoTipo1 : public Controles::BaseDialog
	{
	private:
		String ^PrepareData(::ArrayList *data, int item)
		{
			String ^res = Strings::CharToString(data->ItemString(item));
			if(res == nullptr || res->Length == 0) return "0";
			return res;
		}

	public:
		ArqueoTipo1(String ^titulo, ::ArrayList *datos, ::ArrayList *aData)
		{
			InitializeComponent();

			// Completa la form
			Titulo->Text = "Arqueo de " + titulo;
			bEnter->Enabled = false;

			l1->Text = Strings::CharToString(datos->ItemString(1));
			l2->Text = Strings::CharToString(datos->ItemString(2));
			l3->Text = Strings::CharToString(datos->ItemString(3));

			t1->Text = "$ " + Strings::CharToString(datos->ItemString(4));
			t2->Text = "$ " + Strings::CharToString(datos->ItemString(5));
			t3->Text = "$ " + Strings::CharToString(datos->ItemString(6));
			t4->Text = "$ " + Strings::CharToString(datos->ItemString(7));
			t5->Text = "$ " + Strings::CharToString(datos->ItemString(8));
			t6->Text = "$ " + Strings::CharToString(datos->ItemString(9));
			t7->Text = "$ " + Strings::CharToString(datos->ItemString(10));
			t8->Text = "$ " + Strings::CharToString(datos->ItemString(11));
			t9->Text = "$ " + Strings::CharToString(datos->ItemString(12));
			t10->Text = "$ " + Strings::CharToString(datos->ItemString(13));
			t11->Text = "$ " + Strings::CharToString(datos->ItemString(14));
			t12->Text = "$ " + Strings::CharToString(datos->ItemString(15));

			tb1->Text = PrepareData(aData, 0);
			tb2->Text = PrepareData(aData, 1);
			tb3->Text = PrepareData(aData, 2);
			tb4->Text = PrepareData(aData, 3);
			tb5->Text = PrepareData(aData, 4);
			tb6->Text = PrepareData(aData, 5);
			tb7->Text = PrepareData(aData, 6);
			tb8->Text = PrepareData(aData, 7);
			tb9->Text = PrepareData(aData, 8);
			tb10->Text = PrepareData(aData, 9);
			tb11->Text = PrepareData(aData, 10);
			tb12->Text = PrepareData(aData, 11);

			TabControls->Add(tb1);
			TabControls->Add(tb2);
			TabControls->Add(tb3);
			TabControls->Add(tb4);
			TabControls->Add(tb5);
			TabControls->Add(tb6);
			TabControls->Add(tb7);
			TabControls->Add(tb8);
			TabControls->Add(tb9);
			TabControls->Add(tb10);
			TabControls->Add(tb11);
			TabControls->Add(tb12);

			tb1_Click(tb1, nullptr);
			Strings::InterceptKeyboard(this, 1);

			// Elimina los botones en configuraciones sin touchscreen
			if(!usaTouch)
			{
				this->b0->Visible = false;
				this->b1->Visible = false;
				this->b2->Visible = false;
				this->b3->Visible = false;
				this->b4->Visible = false;
				this->b5->Visible = false;
				this->b6->Visible = false;
				this->b7->Visible = false;
				this->b8->Visible = false;
				this->b9->Visible = false;
				this->bBorra->Visible = false;
				this->bEnter->Visible = false;
			}
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~ArqueoTipo1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
	private: System::Windows::Forms::Label^  l3;
	private: System::Windows::Forms::Label^  l2;
	private: System::Windows::Forms::Label^  l1;
	private: System::Windows::Forms::Label^  t8;

	private: System::Windows::Forms::Label^  t2;
	private: System::Windows::Forms::Label^  t3;
	private: System::Windows::Forms::Label^  t4;
	private: System::Windows::Forms::Label^  t5;
	private: System::Windows::Forms::Label^  t6;
	private: System::Windows::Forms::Label^  t7;



	private: System::Windows::Forms::Label^  t1;
	private: System::Windows::Forms::Label^  t12;

	private: System::Windows::Forms::Label^  t9;
	private: System::Windows::Forms::Label^  t10;
	private: System::Windows::Forms::Label^  t11;
	private: System::Windows::Forms::TextBox^  tb1;
	private: System::Windows::Forms::TextBox^  tb2;
	private: System::Windows::Forms::TextBox^  tb3;
	private: System::Windows::Forms::TextBox^  tb4;
	private: System::Windows::Forms::TextBox^  tb5;
	private: System::Windows::Forms::TextBox^  tb6;
	private: System::Windows::Forms::TextBox^  tb7;
	private: System::Windows::Forms::TextBox^  tb8;
	private: System::Windows::Forms::TextBox^  tb9;
	private: System::Windows::Forms::TextBox^  tb10;
	private: System::Windows::Forms::TextBox^  tb11;
	private: System::Windows::Forms::TextBox^  tb12;
	private: System::Windows::Forms::Label^  s1;
	private: System::Windows::Forms::Label^  s2;
	private: System::Windows::Forms::Label^  s3;
	private: System::Windows::Forms::Label^  s4;
	private: System::Windows::Forms::Label^  s5;
	private: System::Windows::Forms::Label^  s6;
	private: System::Windows::Forms::Label^  s7;
	private: System::Windows::Forms::Label^  s8;
	private: System::Windows::Forms::Label^  s9;
	private: System::Windows::Forms::Label^  s10;
	private: System::Windows::Forms::Label^  s11;
	private: System::Windows::Forms::Label^  s12;
	private: System::Windows::Forms::Label^  lTotal;










	protected: 

	private:
		/// <summary>
		/// Variable del diseñador requerida.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido del método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->t8 = (gcnew System::Windows::Forms::Label());
			this->l3 = (gcnew System::Windows::Forms::Label());
			this->l2 = (gcnew System::Windows::Forms::Label());
			this->l1 = (gcnew System::Windows::Forms::Label());
			this->t2 = (gcnew System::Windows::Forms::Label());
			this->t3 = (gcnew System::Windows::Forms::Label());
			this->t4 = (gcnew System::Windows::Forms::Label());
			this->t5 = (gcnew System::Windows::Forms::Label());
			this->t6 = (gcnew System::Windows::Forms::Label());
			this->t7 = (gcnew System::Windows::Forms::Label());
			this->t1 = (gcnew System::Windows::Forms::Label());
			this->t9 = (gcnew System::Windows::Forms::Label());
			this->t10 = (gcnew System::Windows::Forms::Label());
			this->t11 = (gcnew System::Windows::Forms::Label());
			this->tb1 = (gcnew System::Windows::Forms::TextBox());
			this->tb2 = (gcnew System::Windows::Forms::TextBox());
			this->tb3 = (gcnew System::Windows::Forms::TextBox());
			this->tb4 = (gcnew System::Windows::Forms::TextBox());
			this->tb5 = (gcnew System::Windows::Forms::TextBox());
			this->tb6 = (gcnew System::Windows::Forms::TextBox());
			this->tb7 = (gcnew System::Windows::Forms::TextBox());
			this->tb8 = (gcnew System::Windows::Forms::TextBox());
			this->tb9 = (gcnew System::Windows::Forms::TextBox());
			this->tb10 = (gcnew System::Windows::Forms::TextBox());
			this->tb11 = (gcnew System::Windows::Forms::TextBox());
			this->tb12 = (gcnew System::Windows::Forms::TextBox());
			this->s1 = (gcnew System::Windows::Forms::Label());
			this->t12 = (gcnew System::Windows::Forms::Label());
			this->s2 = (gcnew System::Windows::Forms::Label());
			this->s3 = (gcnew System::Windows::Forms::Label());
			this->s4 = (gcnew System::Windows::Forms::Label());
			this->s5 = (gcnew System::Windows::Forms::Label());
			this->s6 = (gcnew System::Windows::Forms::Label());
			this->s7 = (gcnew System::Windows::Forms::Label());
			this->s8 = (gcnew System::Windows::Forms::Label());
			this->s9 = (gcnew System::Windows::Forms::Label());
			this->s10 = (gcnew System::Windows::Forms::Label());
			this->s11 = (gcnew System::Windows::Forms::Label());
			this->s12 = (gcnew System::Windows::Forms::Label());
			this->lTotal = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->tableLayoutPanel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel->Controls->Add(this->lTotal);
			this->panel->Controls->Add(this->tableLayoutPanel1);
			this->panel->Size = System::Drawing::Size(658, 513);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->tableLayoutPanel1, 0);
			this->panel->Controls->SetChildIndex(this->Titulo, 0);
			this->panel->Controls->SetChildIndex(this->Icono, 0);
			this->panel->Controls->SetChildIndex(this->panel3, 0);
			this->panel->Controls->SetChildIndex(this->b7, 0);
			this->panel->Controls->SetChildIndex(this->b8, 0);
			this->panel->Controls->SetChildIndex(this->b4, 0);
			this->panel->Controls->SetChildIndex(this->b5, 0);
			this->panel->Controls->SetChildIndex(this->b1, 0);
			this->panel->Controls->SetChildIndex(this->b2, 0);
			this->panel->Controls->SetChildIndex(this->b0, 0);
			this->panel->Controls->SetChildIndex(this->bBorra, 0);
			this->panel->Controls->SetChildIndex(this->btOk, 0);
			this->panel->Controls->SetChildIndex(this->cancel, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
			this->panel->Controls->SetChildIndex(this->lTotal, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 443);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 389);
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->tableLayoutPanel1->CellBorderStyle = System::Windows::Forms::TableLayoutPanelCellBorderStyle::Single;
			this->tableLayoutPanel1->ColumnCount = 3;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				33.33332F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				33.33334F)));
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				33.33334F)));
			this->tableLayoutPanel1->Controls->Add(this->t8, 0, 8);
			this->tableLayoutPanel1->Controls->Add(this->l3, 2, 0);
			this->tableLayoutPanel1->Controls->Add(this->l2, 1, 0);
			this->tableLayoutPanel1->Controls->Add(this->l1, 0, 0);
			this->tableLayoutPanel1->Controls->Add(this->t2, 0, 2);
			this->tableLayoutPanel1->Controls->Add(this->t3, 0, 3);
			this->tableLayoutPanel1->Controls->Add(this->t4, 0, 4);
			this->tableLayoutPanel1->Controls->Add(this->t5, 0, 5);
			this->tableLayoutPanel1->Controls->Add(this->t6, 0, 6);
			this->tableLayoutPanel1->Controls->Add(this->t7, 0, 7);
			this->tableLayoutPanel1->Controls->Add(this->t1, 0, 1);
			this->tableLayoutPanel1->Controls->Add(this->t9, 0, 9);
			this->tableLayoutPanel1->Controls->Add(this->t10, 0, 10);
			this->tableLayoutPanel1->Controls->Add(this->t11, 0, 11);
			this->tableLayoutPanel1->Controls->Add(this->tb1, 1, 1);
			this->tableLayoutPanel1->Controls->Add(this->tb2, 1, 2);
			this->tableLayoutPanel1->Controls->Add(this->tb3, 1, 3);
			this->tableLayoutPanel1->Controls->Add(this->tb4, 1, 4);
			this->tableLayoutPanel1->Controls->Add(this->tb5, 1, 5);
			this->tableLayoutPanel1->Controls->Add(this->tb6, 1, 6);
			this->tableLayoutPanel1->Controls->Add(this->tb7, 1, 7);
			this->tableLayoutPanel1->Controls->Add(this->tb8, 1, 8);
			this->tableLayoutPanel1->Controls->Add(this->tb9, 1, 9);
			this->tableLayoutPanel1->Controls->Add(this->tb10, 1, 10);
			this->tableLayoutPanel1->Controls->Add(this->tb11, 1, 11);
			this->tableLayoutPanel1->Controls->Add(this->tb12, 1, 12);
			this->tableLayoutPanel1->Controls->Add(this->s1, 2, 1);
			this->tableLayoutPanel1->Controls->Add(this->t12, 0, 12);
			this->tableLayoutPanel1->Controls->Add(this->s2, 2, 2);
			this->tableLayoutPanel1->Controls->Add(this->s3, 2, 3);
			this->tableLayoutPanel1->Controls->Add(this->s4, 2, 4);
			this->tableLayoutPanel1->Controls->Add(this->s5, 2, 5);
			this->tableLayoutPanel1->Controls->Add(this->s6, 2, 6);
			this->tableLayoutPanel1->Controls->Add(this->s7, 2, 7);
			this->tableLayoutPanel1->Controls->Add(this->s8, 2, 8);
			this->tableLayoutPanel1->Controls->Add(this->s9, 2, 9);
			this->tableLayoutPanel1->Controls->Add(this->s10, 2, 10);
			this->tableLayoutPanel1->Controls->Add(this->s11, 2, 11);
			this->tableLayoutPanel1->Controls->Add(this->s12, 2, 12);
			this->tableLayoutPanel1->Location = System::Drawing::Point(10, 77);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 13;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.680763F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.82516F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.682183F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.682183F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.682183F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 7.682183F)));
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 20)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(430, 422);
			this->tableLayoutPanel1->TabIndex = 37;
			// 
			// t8
			// 
			this->t8->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t8->AutoSize = true;
			this->t8->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t8->ForeColor = System::Drawing::Color::Navy;
			this->t8->Location = System::Drawing::Point(44, 263);
			this->t8->Name = L"t8";
			this->t8->Size = System::Drawing::Size(55, 18);
			this->t8->TabIndex = 10;
			this->t8->Text = L"label1";
			// 
			// l3
			// 
			this->l3->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->l3->AutoSize = true;
			this->l3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l3->ForeColor = System::Drawing::Color::Navy;
			this->l3->Location = System::Drawing::Point(369, 7);
			this->l3->Name = L"l3";
			this->l3->Size = System::Drawing::Size(57, 18);
			this->l3->TabIndex = 2;
			this->l3->Text = L"$ 0.00";
			// 
			// l2
			// 
			this->l2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->l2->AutoSize = true;
			this->l2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l2->ForeColor = System::Drawing::Color::Navy;
			this->l2->Location = System::Drawing::Point(186, 7);
			this->l2->Name = L"l2";
			this->l2->Size = System::Drawing::Size(55, 18);
			this->l2->TabIndex = 1;
			this->l2->Text = L"label1";
			// 
			// l1
			// 
			this->l1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->l1->AutoSize = true;
			this->l1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->l1->ForeColor = System::Drawing::Color::Navy;
			this->l1->Location = System::Drawing::Point(44, 7);
			this->l1->Name = L"l1";
			this->l1->Size = System::Drawing::Size(55, 18);
			this->l1->TabIndex = 0;
			this->l1->Text = L"label1";
			// 
			// t2
			// 
			this->t2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t2->AutoSize = true;
			this->t2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t2->ForeColor = System::Drawing::Color::Navy;
			this->t2->Location = System::Drawing::Point(44, 71);
			this->t2->Name = L"t2";
			this->t2->Size = System::Drawing::Size(55, 18);
			this->t2->TabIndex = 4;
			this->t2->Text = L"label1";
			// 
			// t3
			// 
			this->t3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t3->AutoSize = true;
			this->t3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t3->ForeColor = System::Drawing::Color::Navy;
			this->t3->Location = System::Drawing::Point(44, 103);
			this->t3->Name = L"t3";
			this->t3->Size = System::Drawing::Size(55, 18);
			this->t3->TabIndex = 5;
			this->t3->Text = L"label1";
			// 
			// t4
			// 
			this->t4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t4->AutoSize = true;
			this->t4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t4->ForeColor = System::Drawing::Color::Navy;
			this->t4->Location = System::Drawing::Point(44, 135);
			this->t4->Name = L"t4";
			this->t4->Size = System::Drawing::Size(55, 18);
			this->t4->TabIndex = 6;
			this->t4->Text = L"label1";
			// 
			// t5
			// 
			this->t5->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t5->AutoSize = true;
			this->t5->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t5->ForeColor = System::Drawing::Color::Navy;
			this->t5->Location = System::Drawing::Point(44, 167);
			this->t5->Name = L"t5";
			this->t5->Size = System::Drawing::Size(55, 18);
			this->t5->TabIndex = 7;
			this->t5->Text = L"label1";
			// 
			// t6
			// 
			this->t6->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t6->AutoSize = true;
			this->t6->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t6->ForeColor = System::Drawing::Color::Navy;
			this->t6->Location = System::Drawing::Point(44, 199);
			this->t6->Name = L"t6";
			this->t6->Size = System::Drawing::Size(55, 18);
			this->t6->TabIndex = 8;
			this->t6->Text = L"label1";
			// 
			// t7
			// 
			this->t7->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t7->AutoSize = true;
			this->t7->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t7->ForeColor = System::Drawing::Color::Navy;
			this->t7->Location = System::Drawing::Point(44, 231);
			this->t7->Name = L"t7";
			this->t7->Size = System::Drawing::Size(55, 18);
			this->t7->TabIndex = 9;
			this->t7->Text = L"label1";
			// 
			// t1
			// 
			this->t1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t1->AutoSize = true;
			this->t1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t1->ForeColor = System::Drawing::Color::Navy;
			this->t1->Location = System::Drawing::Point(44, 39);
			this->t1->Name = L"t1";
			this->t1->Size = System::Drawing::Size(55, 18);
			this->t1->TabIndex = 3;
			this->t1->Text = L"label1";
			// 
			// t9
			// 
			this->t9->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t9->AutoSize = true;
			this->t9->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t9->ForeColor = System::Drawing::Color::Navy;
			this->t9->Location = System::Drawing::Point(44, 295);
			this->t9->Name = L"t9";
			this->t9->Size = System::Drawing::Size(55, 18);
			this->t9->TabIndex = 11;
			this->t9->Text = L"label1";
			// 
			// t10
			// 
			this->t10->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t10->AutoSize = true;
			this->t10->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t10->ForeColor = System::Drawing::Color::Navy;
			this->t10->Location = System::Drawing::Point(44, 327);
			this->t10->Name = L"t10";
			this->t10->Size = System::Drawing::Size(55, 18);
			this->t10->TabIndex = 12;
			this->t10->Text = L"label2";
			// 
			// t11
			// 
			this->t11->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t11->AutoSize = true;
			this->t11->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t11->ForeColor = System::Drawing::Color::Navy;
			this->t11->Location = System::Drawing::Point(44, 359);
			this->t11->Name = L"t11";
			this->t11->Size = System::Drawing::Size(55, 18);
			this->t11->TabIndex = 13;
			this->t11->Text = L"label3";
			// 
			// tb1
			// 
			this->tb1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb1->Location = System::Drawing::Point(146, 36);
			this->tb1->Name = L"tb1";
			this->tb1->Size = System::Drawing::Size(136, 25);
			this->tb1->TabIndex = 15;
			this->tb1->Text = L"0";
			this->tb1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb1->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb1->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb2
			// 
			this->tb2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb2->Location = System::Drawing::Point(146, 68);
			this->tb2->Name = L"tb2";
			this->tb2->Size = System::Drawing::Size(136, 25);
			this->tb2->TabIndex = 16;
			this->tb2->Text = L"0";
			this->tb2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb2->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb2->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb3
			// 
			this->tb3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb3->Location = System::Drawing::Point(146, 100);
			this->tb3->Name = L"tb3";
			this->tb3->Size = System::Drawing::Size(136, 25);
			this->tb3->TabIndex = 17;
			this->tb3->Text = L"0";
			this->tb3->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb3->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb3->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb4
			// 
			this->tb4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb4->Location = System::Drawing::Point(146, 132);
			this->tb4->Name = L"tb4";
			this->tb4->Size = System::Drawing::Size(136, 25);
			this->tb4->TabIndex = 18;
			this->tb4->Text = L"0";
			this->tb4->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb4->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb4->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb5
			// 
			this->tb5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb5->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb5->Location = System::Drawing::Point(146, 164);
			this->tb5->Name = L"tb5";
			this->tb5->Size = System::Drawing::Size(136, 25);
			this->tb5->TabIndex = 19;
			this->tb5->Text = L"0";
			this->tb5->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb5->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb5->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb6
			// 
			this->tb6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb6->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb6->Location = System::Drawing::Point(146, 196);
			this->tb6->Name = L"tb6";
			this->tb6->Size = System::Drawing::Size(136, 25);
			this->tb6->TabIndex = 20;
			this->tb6->Text = L"0";
			this->tb6->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb6->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb6->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb7
			// 
			this->tb7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb7->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb7->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb7->Location = System::Drawing::Point(146, 228);
			this->tb7->Name = L"tb7";
			this->tb7->Size = System::Drawing::Size(136, 25);
			this->tb7->TabIndex = 21;
			this->tb7->Text = L"0";
			this->tb7->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb7->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb7->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb8
			// 
			this->tb8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb8->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb8->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb8->Location = System::Drawing::Point(146, 260);
			this->tb8->Name = L"tb8";
			this->tb8->Size = System::Drawing::Size(136, 25);
			this->tb8->TabIndex = 22;
			this->tb8->Text = L"0";
			this->tb8->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb8->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb8->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb9
			// 
			this->tb9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb9->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb9->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb9->Location = System::Drawing::Point(146, 292);
			this->tb9->Name = L"tb9";
			this->tb9->Size = System::Drawing::Size(136, 25);
			this->tb9->TabIndex = 23;
			this->tb9->Text = L"0";
			this->tb9->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb9->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb9->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb10
			// 
			this->tb10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb10->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb10->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb10->Location = System::Drawing::Point(146, 324);
			this->tb10->Name = L"tb10";
			this->tb10->Size = System::Drawing::Size(136, 25);
			this->tb10->TabIndex = 24;
			this->tb10->Text = L"0";
			this->tb10->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb10->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb10->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb11
			// 
			this->tb11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb11->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb11->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb11->Location = System::Drawing::Point(146, 356);
			this->tb11->Name = L"tb11";
			this->tb11->Size = System::Drawing::Size(136, 25);
			this->tb11->TabIndex = 25;
			this->tb11->Text = L"0";
			this->tb11->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb11->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb11->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// tb12
			// 
			this->tb12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right));
			this->tb12->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tb12->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tb12->Location = System::Drawing::Point(146, 390);
			this->tb12->Name = L"tb12";
			this->tb12->Size = System::Drawing::Size(136, 25);
			this->tb12->TabIndex = 26;
			this->tb12->Text = L"0";
			this->tb12->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tb12->TextChanged += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_TextChanged);
			this->tb12->Click += gcnew System::EventHandler(this, &ArqueoTipo1::tb1_Click);
			// 
			// s1
			// 
			this->s1->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s1->AutoSize = true;
			this->s1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s1->ForeColor = System::Drawing::Color::Navy;
			this->s1->Location = System::Drawing::Point(369, 39);
			this->s1->Name = L"s1";
			this->s1->Size = System::Drawing::Size(57, 18);
			this->s1->TabIndex = 27;
			this->s1->Text = L"$ 0.00";
			// 
			// t12
			// 
			this->t12->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->t12->AutoSize = true;
			this->t12->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->t12->ForeColor = System::Drawing::Color::Navy;
			this->t12->Location = System::Drawing::Point(44, 394);
			this->t12->Name = L"t12";
			this->t12->Size = System::Drawing::Size(55, 18);
			this->t12->TabIndex = 14;
			this->t12->Text = L"label4";
			// 
			// s2
			// 
			this->s2->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s2->AutoSize = true;
			this->s2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s2->ForeColor = System::Drawing::Color::Navy;
			this->s2->Location = System::Drawing::Point(369, 71);
			this->s2->Name = L"s2";
			this->s2->Size = System::Drawing::Size(57, 18);
			this->s2->TabIndex = 28;
			this->s2->Text = L"$ 0.00";
			// 
			// s3
			// 
			this->s3->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s3->AutoSize = true;
			this->s3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s3->ForeColor = System::Drawing::Color::Navy;
			this->s3->Location = System::Drawing::Point(369, 103);
			this->s3->Name = L"s3";
			this->s3->Size = System::Drawing::Size(57, 18);
			this->s3->TabIndex = 29;
			this->s3->Text = L"$ 0.00";
			// 
			// s4
			// 
			this->s4->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s4->AutoSize = true;
			this->s4->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s4->ForeColor = System::Drawing::Color::Navy;
			this->s4->Location = System::Drawing::Point(369, 135);
			this->s4->Name = L"s4";
			this->s4->Size = System::Drawing::Size(57, 18);
			this->s4->TabIndex = 30;
			this->s4->Text = L"$ 0.00";
			// 
			// s5
			// 
			this->s5->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s5->AutoSize = true;
			this->s5->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s5->ForeColor = System::Drawing::Color::Navy;
			this->s5->Location = System::Drawing::Point(369, 167);
			this->s5->Name = L"s5";
			this->s5->Size = System::Drawing::Size(57, 18);
			this->s5->TabIndex = 31;
			this->s5->Text = L"$ 0.00";
			// 
			// s6
			// 
			this->s6->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s6->AutoSize = true;
			this->s6->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s6->ForeColor = System::Drawing::Color::Navy;
			this->s6->Location = System::Drawing::Point(369, 199);
			this->s6->Name = L"s6";
			this->s6->Size = System::Drawing::Size(57, 18);
			this->s6->TabIndex = 32;
			this->s6->Text = L"$ 0.00";
			// 
			// s7
			// 
			this->s7->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s7->AutoSize = true;
			this->s7->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s7->ForeColor = System::Drawing::Color::Navy;
			this->s7->Location = System::Drawing::Point(369, 231);
			this->s7->Name = L"s7";
			this->s7->Size = System::Drawing::Size(57, 18);
			this->s7->TabIndex = 33;
			this->s7->Text = L"$ 0.00";
			// 
			// s8
			// 
			this->s8->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s8->AutoSize = true;
			this->s8->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s8->ForeColor = System::Drawing::Color::Navy;
			this->s8->Location = System::Drawing::Point(369, 263);
			this->s8->Name = L"s8";
			this->s8->Size = System::Drawing::Size(57, 18);
			this->s8->TabIndex = 34;
			this->s8->Text = L"$ 0.00";
			// 
			// s9
			// 
			this->s9->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s9->AutoSize = true;
			this->s9->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s9->ForeColor = System::Drawing::Color::Navy;
			this->s9->Location = System::Drawing::Point(369, 295);
			this->s9->Name = L"s9";
			this->s9->Size = System::Drawing::Size(57, 18);
			this->s9->TabIndex = 35;
			this->s9->Text = L"$ 0.00";
			// 
			// s10
			// 
			this->s10->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s10->AutoSize = true;
			this->s10->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s10->ForeColor = System::Drawing::Color::Navy;
			this->s10->Location = System::Drawing::Point(369, 327);
			this->s10->Name = L"s10";
			this->s10->Size = System::Drawing::Size(57, 18);
			this->s10->TabIndex = 36;
			this->s10->Text = L"$ 0.00";
			// 
			// s11
			// 
			this->s11->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s11->AutoSize = true;
			this->s11->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s11->ForeColor = System::Drawing::Color::Navy;
			this->s11->Location = System::Drawing::Point(369, 359);
			this->s11->Name = L"s11";
			this->s11->Size = System::Drawing::Size(57, 18);
			this->s11->TabIndex = 37;
			this->s11->Text = L"$ 0.00";
			// 
			// s12
			// 
			this->s12->Anchor = System::Windows::Forms::AnchorStyles::Right;
			this->s12->AutoSize = true;
			this->s12->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->s12->ForeColor = System::Drawing::Color::Navy;
			this->s12->Location = System::Drawing::Point(369, 394);
			this->s12->Name = L"s12";
			this->s12->Size = System::Drawing::Size(57, 18);
			this->s12->TabIndex = 38;
			this->s12->Text = L"$ 0.00";
			// 
			// lTotal
			// 
			this->lTotal->AutoSize = true;
			this->lTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lTotal->ForeColor = System::Drawing::Color::Navy;
			this->lTotal->Location = System::Drawing::Point(472, 358);
			this->lTotal->Name = L"lTotal";
			this->lTotal->Size = System::Drawing::Size(100, 18);
			this->lTotal->TabIndex = 38;
			this->lTotal->Text = L"Total: $0.00";
			// 
			// ArqueoTipo1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(658, 513);
			this->Name = L"ArqueoTipo1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"ArqueoTipo1";
			this->TopMost = true;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ArqueoTipo1::ArqueoTipo1_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->tableLayoutPanel1->ResumeLayout(false);
			this->tableLayoutPanel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
private:
		Control ^FindControl(Control ^actual, String ^name)
		{
			for each (Control ^subControl in actual->Controls)
			{
				if(subControl->Name == name)
					return subControl;
				if(subControl->Controls->Count != 0)
				{
					Control ^found = FindControl(subControl, name);
					if(found != nullptr)
						return found;
				}
			}
			return nullptr;
		}

		System::Void tb1_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		{
			TextBox ^editTb = (TextBox ^)sender;
			String ^suffix = editTb->Name->Substring(2);
			Label ^tit = (Label ^)FindControl(this, "t" + suffix);
			Label ^sum = (Label ^)FindControl(this, "s" + suffix);

			try
			{
				Decimal dec = Decimal::Parse(tit->Text->Substring(1));
				Decimal cant = Decimal::Parse(editTb->Text);
				Decimal stot = dec * cant;
				sum->Text = "$ " + stot.ToString("N2");

				stot = 0;
				stot += Decimal::Parse(s1->Text->Substring(1));
				stot += Decimal::Parse(s2->Text->Substring(1));
				stot += Decimal::Parse(s3->Text->Substring(1));
				stot += Decimal::Parse(s4->Text->Substring(1));
				stot += Decimal::Parse(s5->Text->Substring(1));
				stot += Decimal::Parse(s6->Text->Substring(1));
				stot += Decimal::Parse(s7->Text->Substring(1));
				stot += Decimal::Parse(s8->Text->Substring(1));
				stot += Decimal::Parse(s9->Text->Substring(1));
				stot += Decimal::Parse(s10->Text->Substring(1));
				stot += Decimal::Parse(s11->Text->Substring(1));
				stot += Decimal::Parse(s12->Text->Substring(1));

				lTotal->Text = "Total: $ " + stot.ToString("N2");
			}
			catch(Exception ^)
			{
				editTb->Text = "0";
			}
		}

		public: int GetResult(System::Int32 item)
		{
			TextBox ^cant = (TextBox ^) FindControl(this, "tb" + item.ToString());
			if(cant != nullptr) 
			{
				try { return Int32::Parse(cant->Text); } catch(Exception ^) {}
			}
			return 0;
		}

private: System::Void tb1_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->tb = (TextBox ^)sender;
			 borrar = true;
		 }
private: System::Void ArqueoTipo1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 Strings::releaseKeyboard(this);

		 }
};
}
#endif
