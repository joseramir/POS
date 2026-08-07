#pragma once
#ifdef POSXP
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#ifdef POSXP
#include "DbfBrowse.h"
#include "db.h"
#include "Strings.h"

namespace pos {

	/// <summary>
	/// Resumen de ConfCorri
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class ConfCorri : public System::Windows::Forms::Form,
		Controles::IBuscador
	{
		DbfBrowse ^browser;
	
	public:
		ConfCorri(DbfBrowse ^_browser, dbf *data)
		{
			InitializeComponent();

			// Pone el 'TopMost' de la form, pero solo si no estamos en modo debug.
			/*String ^sd = System::Environment::GetEnvironmentVariable("SYSDEBUG");
			this->TopMost = (sd != nullptr && sd->ToLower() == "on")? false: true;*/
			this->TopMost = true;

			browser = _browser;
			browser->Link(nullptr, grid, data);
			Strings::InterceptKeyboard(this, 2);
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~ConfCorri()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btOk;
	protected: 
	private: System::Windows::Forms::Button^  btEsc;
	private: System::Windows::Forms::DataGridView^  grid;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btAbajo;
	private: System::Windows::Forms::Button^  btArriba;



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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ConfCorri::typeid));
			this->btOk = (gcnew System::Windows::Forms::Button());
			this->btEsc = (gcnew System::Windows::Forms::Button());
			this->grid = (gcnew System::Windows::Forms::DataGridView());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btAbajo = (gcnew System::Windows::Forms::Button());
			this->btArriba = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid))->BeginInit();
			this->SuspendLayout();
			// 
			// btOk
			// 
			this->btOk->BackColor = System::Drawing::SystemColors::Control;
			this->btOk->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btOk->Location = System::Drawing::Point(326, 417);
			this->btOk->Name = L"btOk";
			this->btOk->Size = System::Drawing::Size(241, 41);
			this->btOk->TabIndex = 0;
			this->btOk->Text = L"OK=Aprueba las Correcciones";
			this->btOk->UseVisualStyleBackColor = false;
			this->btOk->Click += gcnew System::EventHandler(this, &ConfCorri::btOk_Click);
			// 
			// btEsc
			// 
			this->btEsc->BackColor = System::Drawing::SystemColors::Control;
			this->btEsc->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btEsc->Location = System::Drawing::Point(32, 417);
			this->btEsc->Name = L"btEsc";
			this->btEsc->Size = System::Drawing::Size(241, 41);
			this->btEsc->TabIndex = 1;
			this->btEsc->Text = L"ESC=Salir sin Aprobar";
			this->btEsc->UseVisualStyleBackColor = false;
			this->btEsc->Click += gcnew System::EventHandler(this, &ConfCorri::btEsc_Click);
			// 
			// grid
			// 
			this->grid->AllowUserToAddRows = false;
			this->grid->AllowUserToDeleteRows = false;
			this->grid->BackgroundColor = System::Drawing::Color::White;
			this->grid->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->grid->Location = System::Drawing::Point(11, 46);
			this->grid->MultiSelect = false;
			this->grid->Name = L"grid";
			this->grid->ReadOnly = true;
			this->grid->RowHeadersVisible = false;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->grid->RowsDefaultCellStyle = dataGridViewCellStyle1;
			this->grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->grid->Size = System::Drawing::Size(589, 356);
			this->grid->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::SystemColors::Info;
			this->label1->Location = System::Drawing::Point(13, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(377, 19);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Renglones Anulados Pendientes de Autorizar";
			// 
			// btAbajo
			// 
			this->btAbajo->BackColor = System::Drawing::Color::Transparent;
			this->btAbajo->FlatAppearance->BorderSize = 0;
			this->btAbajo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btAbajo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btAbajo.Image")));
			this->btAbajo->Location = System::Drawing::Point(603, 126);
			this->btAbajo->Name = L"btAbajo";
			this->btAbajo->Size = System::Drawing::Size(54, 53);
			this->btAbajo->TabIndex = 4;
			this->btAbajo->UseVisualStyleBackColor = false;
			this->btAbajo->Click += gcnew System::EventHandler(this, &ConfCorri::btAbajo_Click);
			// 
			// btArriba
			// 
			this->btArriba->BackColor = System::Drawing::Color::Transparent;
			this->btArriba->FlatAppearance->BorderSize = 0;
			this->btArriba->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->btArriba->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btArriba.Image")));
			this->btArriba->Location = System::Drawing::Point(603, 67);
			this->btArriba->Name = L"btArriba";
			this->btArriba->Size = System::Drawing::Size(54, 53);
			this->btArriba->TabIndex = 5;
			this->btArriba->UseVisualStyleBackColor = false;
			this->btArriba->Click += gcnew System::EventHandler(this, &ConfCorri::btArriba_Click);
			// 
			// ConfCorri
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Desktop;
			this->ClientSize = System::Drawing::Size(670, 474);
			this->ControlBox = false;
			this->Controls->Add(this->btArriba);
			this->Controls->Add(this->btAbajo);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->grid);
			this->Controls->Add(this->btEsc);
			this->Controls->Add(this->btOk);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Name = L"ConfCorri";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L" Confirma Anulaciones de Renglones";
			this->TopMost = true;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ConfCorri::ConfCorri_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->grid))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	public: virtual void AcceptKey(Keys key)
		{
			bool proceder = true;
			switch(key)
			{
				case Keys::Down:
					browser->Saltear(1);
					break;

				case Keys::Up:
					browser->Saltear(-1);
					break;

				case Keys::F8:	
				case Keys::Enter:
					btOk_Click(nullptr, nullptr);
					break;

				case Keys::Escape:
					btEsc_Click(nullptr, nullptr);
					break;

				default:
					if((int)key == 21 || key == Keys::ControlKey)
						btOk_Click(nullptr, nullptr);
			}
		}

	private: System::Void btOk_Click(System::Object^  sender, System::EventArgs^  e) 
	    {
			 this->DialogResult = System::Windows::Forms::DialogResult::OK;
			 this->Close();
		}

	private: System::Void btEsc_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			 this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			 this->Close();
		}
	private: System::Void ConfCorri_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
		{
			Strings::releaseKeyboard(this);
		}
	private: System::Void btArriba_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			browser->Saltear(-1);
		}
	private: System::Void btAbajo_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			browser->Saltear(1);
		}
};
}
#endif
#endif