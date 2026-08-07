#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
#ifdef FACTSQL
	using namespace TransSql;
#endif


namespace pos {

	/// <summary>
	/// Summary for frmFactSql
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class frmFactSql : public Controles::BaseDialog
	{
		delegate void InvokeSaltear(int);

		InvokeSaltear ^doSaltear;


















	public:
		long numeroseq;
	private: System::Windows::Forms::Label^  lblTotal;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  Column1;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column2;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column3;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column4;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Column5;





	public: 
		System::Decimal totalcompra;

		frmFactSql(void)
		{
			InitializeComponent();
			Strings::InterceptKeyboard(this, 1);
			doSaltear = gcnew InvokeSaltear(this, &frmFactSql::DoSaltear);
			totalcompra = 0;
			//
			//TODO: Add the constructor code here
			//
#ifdef FACTSQL
			try
			{			
				DatosSql::CadenaConexion = gcnew System::String(strconbd);
				DataTable ^dt = DatosSql::GetTransPendientes((int)date());
				System::Drawing::Font^ xFont = gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Regular);
				Grid->RowTemplate->Height = 36;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Bold);
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width);
					col->DefaultCellStyle->Font = xFont;
				}
				Grid->AutoGenerateColumns = false;
				Grid->ReadOnly = false;
				Grid->DataSource = dt;
				DataGridViewColumn ^col = Grid->Columns[0];				
				col->ReadOnly = false;
				col = Grid->Columns[1];
				col->DataPropertyName = "seq";
				col->ReadOnly = true;
				col = Grid->Columns[2];				
				col->DataPropertyName = "nombre";
				col->ReadOnly = true;
				col = Grid->Columns[3];
				col->DataPropertyName = "percepcion";
				col->ReadOnly = true;
				col = Grid->Columns[4];
				col->DataPropertyName = "monto";
				col->ReadOnly = true;
			}			
			catch (Exception ^e)
			{
				DatosSql::WriteLog(e, "frmFactSql");
			}
#endif
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~frmFactSql()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::DataGridView^  Grid;






	protected: 

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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->Grid = (gcnew System::Windows::Forms::DataGridView());
			this->lblTotal = (gcnew System::Windows::Forms::Label());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column4 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column5 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->Grid);
			this->panel->Controls->Add(this->lblTotal);
			this->panel->Size = System::Drawing::Size(730, 440);
			this->panel->Controls->SetChildIndex(this->lblTotal, 0);
			this->panel->Controls->SetChildIndex(this->Grid, 0);
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
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(556, 371);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(556, 317);
			this->btOk->Click += gcnew System::EventHandler(this, &frmFactSql::btOk_Click);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(612, 233);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(556, 233);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(612, 181);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(556, 181);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(612, 129);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(556, 129);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(612, 77);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(556, 77);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(428, 23);
			this->Titulo->Text = L"Elegir Ticket/Factura de cliente para Cobrar";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(668, 233);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(668, 181);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(668, 129);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(668, 77);
			// 
			// Grid
			// 
			this->Grid->AllowUserToAddRows = false;
			this->Grid->AllowUserToDeleteRows = false;
			this->Grid->AllowUserToResizeRows = false;
			this->Grid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->Grid->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(5) {this->Column1, this->Column2, 
				this->Column3, this->Column4, this->Column5});
			this->Grid->Location = System::Drawing::Point(10, 79);
			this->Grid->MultiSelect = false;
			this->Grid->Name = L"Grid";
			this->Grid->RowHeadersVisible = false;
			this->Grid->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->Grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->Grid->Size = System::Drawing::Size(540, 320);
			this->Grid->TabIndex = 39;
			this->Grid->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &frmFactSql::Grid_CellValueChanged);
			this->Grid->CurrentCellDirtyStateChanged += gcnew System::EventHandler(this, &frmFactSql::Grid_CurrentCellDirtyStateChanged);
			// 
			// lblTotal
			// 
			this->lblTotal->AutoSize = true;
			this->lblTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblTotal->Location = System::Drawing::Point(201, 409);
			this->lblTotal->Name = L"lblTotal";
			this->lblTotal->Size = System::Drawing::Size(224, 19);
			this->lblTotal->TabIndex = 40;
			this->lblTotal->Text = L"Total Seleccionado: $ 0.00";
			this->lblTotal->Visible = false;
			// 
			// Column1
			// 
			this->Column1->HeaderText = L"";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			this->Column1->Resizable = System::Windows::Forms::DataGridViewTriState::True;
			this->Column1->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::Automatic;
			this->Column1->Width = 40;
			// 
			// Column2
			// 
			this->Column2->HeaderText = L"Secuencia";
			this->Column2->Name = L"Column2";
			this->Column2->ReadOnly = true;
			this->Column2->Width = 80;
			// 
			// Column3
			// 
			dataGridViewCellStyle1->NullValue = nullptr;
			this->Column3->DefaultCellStyle = dataGridViewCellStyle1;
			this->Column3->HeaderText = L"Nombre";
			this->Column3->Name = L"Column3";
			this->Column3->Width = 200;
			// 
			// Column4
			// 
			dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleRight;
			dataGridViewCellStyle2->Format = L"C2";
			dataGridViewCellStyle2->NullValue = nullptr;
			this->Column4->DefaultCellStyle = dataGridViewCellStyle2;
			this->Column4->HeaderText = L"Percepciones";
			this->Column4->Name = L"Column4";
			// 
			// Column5
			// 
			dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleRight;
			dataGridViewCellStyle3->Format = L"C2";
			dataGridViewCellStyle3->NullValue = nullptr;
			this->Column5->DefaultCellStyle = dataGridViewCellStyle3;
			this->Column5->HeaderText = L"Monto";
			this->Column5->Name = L"Column5";
			this->Column5->Width = 120;
			// 
			// frmFactSql
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(730, 440);
			this->Name = L"frmFactSql";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Elegir Cliente";
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
			 {
				 if (Grid->SelectedRows->Count > 0)
				 {				 
					DataGridViewRow ^row = Grid->SelectedRows[0];
					DataGridViewCell ^celda = row->Cells[1];				 
					numeroseq = Convert::ToInt64(celda->Value);
				 }
				 else
					 numeroseq = -1;
				 BaseDialog::btOk_Click(sender, e);
			 }

	public: virtual bool ProcessHotKey(Keys key) override
			{
				if(key == Keys::Up)				
					Grid->Invoke(doSaltear, gcnew array<Object ^>{-1});								
				else if (key == Keys::Down)				
					Grid->Invoke(doSaltear, gcnew array<Object ^>{1});										 
				
				return BaseDialog::ProcessHotKey(key);
			}

	private: void DoSaltear(int c)
			{
				DataGridViewRow ^selectedRow = Grid->SelectedRows[0];
				
				if(selectedRow != nullptr)
				{
					int row = selectedRow->Index + c;
					if(row < 0 || row >= Grid->Rows->Count)
						row = (c < 0)? 0: Grid->Rows->Count - 1;
					selectedRow->Selected = false;
					selectedRow = Grid->Rows[row];
					
					selectedRow->Selected = true;
					Grid->CurrentCell = selectedRow->Cells[0];
					Grid->CurrentCell->Selected = true;
				}				
			}
private: System::Void Grid_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 if (e->RowIndex < 0)
				 return;
			 totalcompra = 0;
			 for each (DataGridViewRow^ row in Grid->Rows)
			 {
				 DataGridViewCell ^ccheck = row->Cells[0];
				 if (System::Convert::ToBoolean(ccheck->Value))
				 {				 
					DataGridViewCell ^celda = row->Cells[4];				 
					totalcompra += System::Decimal::Parse(celda->Value->ToString());				 
				 }
			 }
			 lblTotal->Text = String::Format("Total Seleccionado: $ {0:f2}", totalcompra);
			 lblTotal->Visible = totalcompra > 0;
		 }
private: System::Void Grid_CurrentCellDirtyStateChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (Grid->IsCurrentCellDirty)
				 Grid->CommitEdit(DataGridViewDataErrorContexts::Commit);
		 }
};
}
