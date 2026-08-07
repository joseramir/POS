#pragma once
#ifdef POSXP

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "pos.h"
#include "scrobj.h"

namespace pos {

	/// <summary>
	/// Resumen de GenericForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class GenericForm : public Controles::BaseDialog
	{
		GenForm *def;
		System::Drawing::Font ^titFont;
		System::Collections::Generic::List<Control ^> ^usanFont;

	public:
		void CambiarFont(String ^name, int size)
		{
			titFont = gcnew System::Drawing::Font(name, (float)size, System::Drawing::FontStyle::Bold);
			for(int i = usanFont->Count - 1; i >= 0; i--)
				usanFont[i]->Font = titFont;

			switch(size)
			{
				case FONT_CHICO:
					tbl->ColumnStyles[0]->Width = 80;
					tbl->ColumnStyles[1]->Width = 20;

				case FONT_MEDIANO:
					tbl->ColumnStyles[0]->Width = 64;
					tbl->ColumnStyles[1]->Width = 36;

				default:
					tbl->ColumnStyles[0]->Width = 54;
					tbl->ColumnStyles[1]->Width = 46;
					break;
			}
		}

		GenericForm(GenForm *_def)
		{
			InitializeComponent();
			usanFont = gcnew System::Collections::Generic::List<Control ^>();

			// cambia el titulo.
			def = _def;
			this->Titulo->Text = Strings::CharToString(def->VerTitulo());
			titFont = gcnew System::Drawing::Font("Tahoma", 11, System::Drawing::FontStyle::Bold);

			// Agrega los items.
			for(int j = 0; j < def->Items()->Count(); j++)
			{
				EditItem *item = (EditItem *)def->Items()->Item(j);

				// Prepara y asigna el titulo.
				Label ^titulo = gcnew Label();
				usanFont->Add(titulo);
				titulo->Font = titFont;
				titulo->Text = Strings::CharToString(item->VerLabel()) + ":";
				titulo->AutoSize = true;
				titulo->Anchor = AnchorStyles::Right;
				tbl->Controls->Add(titulo);
				tbl->SetColumn(titulo, 0);
				tbl->SetRow(titulo, j);

				TextBox ^textBox = gcnew TextBox();
				usanFont->Add(textBox);
				textBox->TextAlign = HorizontalAlignment::Right;
				textBox->Font = titFont;
				textBox->Height = 40;
				textBox->Width = 150;
				textBox->Text = Strings::CharToString(item->dato);
				textBox->Anchor = AnchorStyles::None;
				tbl->Controls->Add(textBox);
				tbl->SetColumn(textBox, 1);
				tbl->SetRow(textBox, j);
				textBox->Enabled = item->enabled;

				// Intercepta los eventos
				if(usaTouch)
					textBox->Click += gcnew EventHandler(this, &pos::GenericForm::TbClick);
				else
				{
					textBox->GotFocus += gcnew EventHandler(this, &pos::GenericForm::TbGotFocus);
					textBox->LostFocus += gcnew EventHandler(this, &pos::GenericForm::TbLostFocus);
				}

				tb = textBox;
				tb->Tag = j;
				if(tb->Enabled)	TabControls->Add(tb);
			}

			// Prepara el manejo del teclado, si corresponde.
			this->ExtendedKeyboardEnabled = usaTouch;
			Strings::InterceptKeyboard(this, 1);
			this->TopMost = false;
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~GenericForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^  tbl;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  lbHelp;
	protected: 

		protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			if(usaTouch && lastCtl != nullptr)
				TbLostFocus(lastCtl, nullptr);
			BaseDialog::btOk_Click(sender, e);
		}

		// Evento solu usado en forms con touch screen
		void TbClick(Object ^sender, EventArgs ^e)
		{
			if(lastCtl != sender)
			{
				TbLostFocus(lastCtl, nullptr);
				TbGotFocus(sender, nullptr);
			}
		}

		bool inGotFocus;

		void TbGotFocus(Object ^sender, EventArgs ^e)
		{
			if(inGotFocus) return;
			inGotFocus = true;

			// Si el destino es un textbox, cargamos su texto.
			TextBox ^tb = dynamic_cast<TextBox ^>(sender);
			if(tb != nullptr)
			{
				int itemIndex = (int)tb->Tag;
				EditItem *item = (EditItem *)def->Items()->Item(itemIndex);
				tb->Text = Strings::CharToString(item->dato);
				SetFocusTo(tb);
			}
			inGotFocus = false;
		}

		bool inLostFocus;

		void TbLostFocus(Object ^sender, EventArgs ^e)
		{
			char temp[60];

			if(inLostFocus) return;
			inLostFocus = true;

			// Si el origen es un textbox, actualizamos la info de backup
			TextBox ^tb = dynamic_cast<TextBox ^>(sender);
			if(tb != nullptr)
			{
				int itemIndex = (int)tb->Tag;
				EditItem *item = (EditItem *)def->Items()->Item(itemIndex);
				Strings::StringToChar(tb->Text, temp, 59);
				char *res = item->ValidarDatoIngresado(temp, true);
				if(res != NULL)
				{
					Alert(res, "");
					STRCPY(item->dato, "");
					tb->Text = "";
				}
				else
					tb->Text = Strings::CharToString(item->dato);
			}
			inLostFocus = false;
		}

	public:
		virtual void SetFocusTo(Control ^ctl) override
		{
			BaseDialog::SetFocusTo(ctl);
			TextBox ^textbox = dynamic_cast<TextBox ^>(ctl);
			if(textbox != nullptr)
			{
				int itemIndex = (int)textbox->Tag;
				EditItem *item = (EditItem *)def->Items()->Item(itemIndex);
				lbHelp->Text = Strings::CharToString(item->help);
			}
		}


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
			this->tbl = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->lbHelp = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->panel1);
			this->panel->Controls->Add(this->tbl);
			this->panel->Size = System::Drawing::Size(658, 491);
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
			this->panel->Controls->SetChildIndex(this->tbl, 0);
			this->panel->Controls->SetChildIndex(this->panel1, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 429);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 375);
			// 
			// tbl
			// 
			this->tbl->ColumnCount = 2;
			this->tbl->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 54.83871F)));
			this->tbl->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 45.16129F)));
			this->tbl->Location = System::Drawing::Point(19, 77);
			this->tbl->Name = L"tbl";
			this->tbl->RowCount = 8;
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Absolute, 40)));
			this->tbl->Size = System::Drawing::Size(373, 321);
			this->tbl->TabIndex = 39;
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::LightYellow;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->lbHelp);
			this->panel1->Location = System::Drawing::Point(19, 404);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(373, 73);
			this->panel1->TabIndex = 40;
			// 
			// lbHelp
			// 
			this->lbHelp->AutoEllipsis = true;
			this->lbHelp->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbHelp->Location = System::Drawing::Point(3, 5);
			this->lbHelp->Name = L"lbHelp";
			this->lbHelp->Size = System::Drawing::Size(365, 66);
			this->lbHelp->TabIndex = 0;
			this->lbHelp->Text = L"label1";
			// 
			// GenericForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(658, 491);
			this->Name = L"GenericForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GenericForm";
			this->Activated += gcnew System::EventHandler(this, &GenericForm::GenericForm_Activated);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &GenericForm::GenericForm_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	
private: System::Void GenericForm_Activated(System::Object^  sender, System::EventArgs^  e) {
			SetFocusTo(TabControls[0]);
		}
private: System::Void GenericForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			Strings::releaseKeyboard(this);
		}

protected: void virtual bEnter_Click(Object ^sender, EventArgs ^e) override
		 {
			 TbClick(sender, e);
			 ProcessTab(1);
		 }
};
}
#endif
