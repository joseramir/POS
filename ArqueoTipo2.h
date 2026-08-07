#pragma once
#ifdef POSXP

#include "db.h"
#include "Strings.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace pos {

	/// <summary>
	/// Resumen de ArqueoTipo2
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class ArqueoTipo2 : public Controles::BaseDialog
	{
	private:
		dbf *data;

	public:
		ArqueoTipo2(System::String ^medName, dbf *tdata)
		{
			char tmsg[100];
			InitializeComponent();

			// Inicializa la form.
			data = tdata;
			Titulo->Text = "Arqueo de " + medName;
			TabControls->Add(tbCant);
			TabControls->Add(tbMonto);

			// Llena la lista con los items iniciales.
			lv->Items->Clear();
			data->Go(DB_GO_TOP);
			while(!data->Eof())
			{
				Double cant = Double(0), denom = Double(0), tot = Double(0);
				try
				{
					data->Get("N1", tmsg);
					cant = Double::Parse(Strings::CharToString(tmsg));

					data->Get("N2", tmsg);
					denom = Double::Parse(Strings::CharToString(tmsg));

					data->Get("N3", tmsg);
					tot = Double::Parse(Strings::CharToString(tmsg));

					ListViewItem ^item = lv->Items->Add(cant.ToString());
					item->SubItems->Add(denom.ToString("N2"));
					item->SubItems->Add(tot.ToString("N2"));
				}
				catch(Exception ^) {}

				data->Skip(1);
			}

			// Deja la form lista
			RefreshTotal();
			tbCant_Click(tbCant, nullptr);
			Strings::InterceptKeyboard(this, 1);

			TabControls->Add(tbCant);
			TabControls->Add(tbMonto);

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
				this->btDot->Visible = false;
				this->bBorra->Visible = false;
				this->bEnter->Visible = false;
			}
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~ArqueoTipo2()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::ListView^  lv;
	protected: 
	private: System::Windows::Forms::ColumnHeader^  colCantidad;
	private: System::Windows::Forms::ColumnHeader^  colDenom;
	private: System::Windows::Forms::ColumnHeader^  colTotal;
	private: System::Windows::Forms::TextBox^  tbMonto;

	private: System::Windows::Forms::TextBox^  tbCant;
	private: System::Windows::Forms::Button^  btBorrar;
	private: System::Windows::Forms::Label^  lbTotal;
	private: System::Windows::Forms::Button^  btDot;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;



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
			System::Windows::Forms::ListViewItem^  listViewItem1 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(3) {L"aaa", 
				L"xxx", L"yyy"}, -1));
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ArqueoTipo2::typeid));
			this->lv = (gcnew System::Windows::Forms::ListView());
			this->colCantidad = (gcnew System::Windows::Forms::ColumnHeader());
			this->colDenom = (gcnew System::Windows::Forms::ColumnHeader());
			this->colTotal = (gcnew System::Windows::Forms::ColumnHeader());
			this->tbCant = (gcnew System::Windows::Forms::TextBox());
			this->tbMonto = (gcnew System::Windows::Forms::TextBox());
			this->btBorrar = (gcnew System::Windows::Forms::Button());
			this->lbTotal = (gcnew System::Windows::Forms::Label());
			this->btDot = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->btDot);
			this->panel->Controls->Add(this->lbTotal);
			this->panel->Controls->Add(this->btBorrar);
			this->panel->Controls->Add(this->tbMonto);
			this->panel->Controls->Add(this->tbCant);
			this->panel->Controls->Add(this->lv);
			this->panel->Size = System::Drawing::Size(660, 518);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->lv, 0);
			this->panel->Controls->SetChildIndex(this->tbCant, 0);
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
			this->panel->Controls->SetChildIndex(this->tbMonto, 0);
			this->panel->Controls->SetChildIndex(this->btBorrar, 0);
			this->panel->Controls->SetChildIndex(this->lbTotal, 0);
			this->panel->Controls->SetChildIndex(this->btDot, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 453);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 399);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(578, 233);
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(466, 287);
			this->bEnter->Size = System::Drawing::Size(162, 48);
			// 
			// lv
			// 
			this->lv->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lv->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(3) {this->colCantidad, this->colDenom, 
				this->colTotal});
			this->lv->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lv->FullRowSelect = true;
			this->lv->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->lv->HideSelection = false;
			this->lv->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(1) {listViewItem1});
			this->lv->Location = System::Drawing::Point(19, 155);
			this->lv->MultiSelect = false;
			this->lv->Name = L"lv";
			this->lv->Size = System::Drawing::Size(406, 349);
			this->lv->TabIndex = 37;
			this->lv->UseCompatibleStateImageBehavior = false;
			this->lv->View = System::Windows::Forms::View::Details;
			// 
			// colCantidad
			// 
			this->colCantidad->Text = L"Cantidad";
			this->colCantidad->Width = 120;
			// 
			// colDenom
			// 
			this->colDenom->Text = L"Denominación";
			this->colDenom->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->colDenom->Width = 130;
			// 
			// colTotal
			// 
			this->colTotal->Text = L"Total";
			this->colTotal->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->colTotal->Width = 120;
			// 
			// tbCant
			// 
			this->tbCant->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCant->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCant->Location = System::Drawing::Point(121, 77);
			this->tbCant->Name = L"tbCant";
			this->tbCant->Size = System::Drawing::Size(121, 25);
			this->tbCant->TabIndex = 38;
			this->tbCant->Click += gcnew System::EventHandler(this, &ArqueoTipo2::tbCant_Click);
			// 
			// tbMonto
			// 
			this->tbMonto->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbMonto->Location = System::Drawing::Point(121, 108);
			this->tbMonto->Name = L"tbMonto";
			this->tbMonto->Size = System::Drawing::Size(121, 25);
			this->tbMonto->TabIndex = 39;
			this->tbMonto->Click += gcnew System::EventHandler(this, &ArqueoTipo2::tbCant_Click);
			// 
			// btBorrar
			// 
			this->btBorrar->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btBorrar->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btBorrar->Location = System::Drawing::Point(279, 77);
			this->btBorrar->Name = L"btBorrar";
			this->btBorrar->Size = System::Drawing::Size(115, 56);
			this->btBorrar->TabIndex = 40;
			this->btBorrar->Text = L"Borrar Renglón";
			this->btBorrar->UseVisualStyleBackColor = false;
			this->btBorrar->Click += gcnew System::EventHandler(this, &ArqueoTipo2::btBorrar_Click);
			// 
			// lbTotal
			// 
			this->lbTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbTotal->ForeColor = System::Drawing::Color::Navy;
			this->lbTotal->Location = System::Drawing::Point(463, 358);
			this->lbTotal->Name = L"lbTotal";
			this->lbTotal->Size = System::Drawing::Size(165, 18);
			this->lbTotal->TabIndex = 41;
			this->lbTotal->Text = L"Total: $ 0.00";
			this->lbTotal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btDot
			// 
			this->btDot->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btDot->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btDot->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btDot.Image")));
			this->btDot->Location = System::Drawing::Point(522, 233);
			this->btDot->Name = L"btDot";
			this->btDot->Size = System::Drawing::Size(50, 48);
			this->btDot->TabIndex = 42;
			this->btDot->Tag = L".";
			this->btDot->UseVisualStyleBackColor = false;
			this->btDot->Click += gcnew System::EventHandler(this, &ArqueoTipo2::btDot_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Navy;
			this->label1->Location = System::Drawing::Point(40, 80);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(79, 18);
			this->label1->TabIndex = 43;
			this->label1->Text = L"Cantidad:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Navy;
			this->label2->Location = System::Drawing::Point(45, 111);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(74, 18);
			this->label2->TabIndex = 44;
			this->label2->Text = L"Unitario:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// ArqueoTipo2
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(660, 518);
			this->Name = L"ArqueoTipo2";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"ArqueoTipo2";
			this->TopMost = true;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &ArqueoTipo2::ArqueoTipo2_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void tbCant_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				SetFocusTo((Control ^)sender);
			}

	protected: virtual System::Void btOk_Click(System::Object ^sender, System::EventArgs ^e) override
			{
				char tmsg[100];
				data->Zap();
				for each(ListViewItem ^item in lv->Items)
				{
					data->AppendBlank();
					data->Replace("N1", Strings::StringToChar(item->SubItems[0]->Text, tmsg, 90));
					data->Replace("N2", Strings::StringToChar(item->SubItems[1]->Text, tmsg, 90));
					data->Replace("N3", Strings::StringToChar(item->SubItems[2]->Text, tmsg, 90));
				}
				BaseDialog::btOk_Click(sender, e);
			}

	public: virtual bool ProcessHotKey(Keys key) override
		   {
			   bool res = true, found;
			   TextBox ^_tb = dynamic_cast<TextBox ^>(tb);
			   switch(key)
			   {
					case Keys::Enter:
						bEnter_Click(nullptr, nullptr);
						for(int index = 0; index < lv->Items->Count - 1; index++)
							lv->Items[0]->Selected = false;
						if(lv->Items->Count > 0)
						{
							lv->Items[lv->Items->Count - 1]->Selected = true;
							lv->EnsureVisible(lv->Items->Count - 1);
						}
						break;

					case Keys::Down:
						found = false;
						for(int index = 0; index < lv->Items->Count - 1; index++)
						{
							if(lv->Items[index]->Selected)
							{
								lv->Items[index]->Selected = false;
								lv->Items[index + 1]->Selected = true;
								lv->EnsureVisible(index + 1);
								found = true;
								break;
							}
						}
						if(!found && lv->Items->Count > 0)
						{
							lv->Items[0]->Selected = true;
							lv->EnsureVisible(0);
						}
						break;

					case Keys::Up:
						for(int index = 1; index < lv->Items->Count; index++)
						{
							if(lv->Items[index]->Selected)
							{
								lv->Items[index]->Selected = false;
								lv->Items[index - 1]->Selected = true;
								lv->EnsureVisible(index - 1);
								break;
							}
						}
						break;

					case Keys::Left:
						if(_tb != nullptr && _tb->Text->Trim()->Length != 0)
						{
							String ^text = _tb->Text->Trim();
							_tb->Text = text->Substring(0, text->Length - 1);
							_tb->SelectionStart = text->Length - 1;
							_tb->SelectionLength = 1;
						}
						break;

					case Keys::Right:		// La anula
						break;

					default:
						if((int)key == keyback || key == System::Windows::Forms::Keys::Subtract)
						{
							btBorrar_Click(nullptr, nullptr);
						}
						else
							res = false;
						break;
			   }
			   return res;
		   }

	protected: virtual System::Void SetFocusTo(System::Windows::Forms::Control ^ctl) override
			{
				TextBox ^text = (TextBox ^)ctl;
				this->tb = text;
				text->Focus();
				borrar = true;
			}

	protected: virtual System::Void bEnter_Click(System::Object ^sender, System::EventArgs ^e) override
			{
				Int32 cant = 0;
				Double monto = 0;
				try
				{
					cant = Int32::Parse(tbCant->Text);
					monto = Double::Parse(tbMonto->Text);
				}
				catch(Exception ^) {}
				if(cant != 0 && monto != 0)
				{
					// Agrega el item a la lista.
					ListViewItem ^root = lv->Items->Add(cant.ToString());
					root->SubItems->Add(monto.ToString("N2"));
					monto = monto * cant;
					root->SubItems->Add(monto.ToString("N2"));

					// Refresca el total
					RefreshTotal();

					// Resetea los textbox
					tbCant->Text = "0";
					tbMonto->Text = "0.00";
					tbCant_Click(tbCant, nullptr);
				}
				else
					BaseDialog::bEnter_Click(sender, e);
			}


	private: System::Void btBorrar_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				if(lv->SelectedIndices->Count != 0)
				{
					int index = lv->SelectedIndices[0];
					lv->Items->RemoveAt(index);
					if(index >= lv->Items->Count) index--;
					if(index >= 0)
					{
						lv->Items[index]->Selected = true;
						lv->EnsureVisible(index);
					}
				}

				RefreshTotal();
				SetFocusTo(tbCant);
			}

			void RefreshTotal()
			{
				System::Double tot = Double(0);
				for each(ListViewItem ^item in lv->Items)
				{
					Double monto = Double::Parse(item->SubItems[2]->Text);
					tot = tot + monto;
				}

				lbTotal->Text = "Total:  $ " + tot.ToString("N2");
			}

private: System::Void btDot_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Boton_Click(sender, e);
		 }
private: System::Void ArqueoTipo2_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 Strings::releaseKeyboard(this);
		 }
};
}
#endif
