#pragma once
#ifdef POSXP

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

#include "Strings.h"

namespace pos {

	/// <summary>
	/// Resumen de PideClave
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class PideClave : public Controles::BaseDialog
	{
		String ^rPassword;
		bool initialFocus;

	public:
		PideClave(String ^msg, String ^pass)
		{
			InitializeComponent();
			this->lbMessage->Text = msg;
			rPassword = pass->Trim();
			this->tb = this->tbClave;
			tbClave->Text = String::Empty;
			Strings::InterceptKeyboard(this, 1);
			TabControls->Add(tbClave);

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

				btOk->Left = panelKey->Right - btOk->Width;
				cancel->Left = panelKey->Right - cancel->Width;
				this->Width = panelKey->Left + panelKey->Right;
				initialFocus = true;
			}
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~PideClave()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  tbClave;
	protected: 

	protected: 
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  lbMessage;
	private: System::Windows::Forms::Panel^  panelKey;



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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(PideClave::typeid));
			this->lbMessage = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbClave = (gcnew System::Windows::Forms::TextBox());
			this->panelKey = (gcnew System::Windows::Forms::Panel());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->panelKey->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel->Controls->Add(this->panelKey);
			this->panel->Size = System::Drawing::Size(653, 449);
			this->panel->Controls->SetChildIndex(this->b9, 0);
			this->panel->Controls->SetChildIndex(this->b6, 0);
			this->panel->Controls->SetChildIndex(this->b3, 0);
			this->panel->Controls->SetChildIndex(this->bEnter, 0);
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
			this->panel->Controls->SetChildIndex(this->panelKey, 0);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->ForeColor = System::Drawing::Color::Navy;
			this->Titulo->Size = System::Drawing::Size(250, 23);
			this->Titulo->Text = L"Ingresar Clave de Acceso";
			// 
			// lbMessage
			// 
			this->lbMessage->AutoSize = true;
			this->lbMessage->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbMessage->ForeColor = System::Drawing::Color::Navy;
			this->lbMessage->Location = System::Drawing::Point(15, 19);
			this->lbMessage->Name = L"lbMessage";
			this->lbMessage->Size = System::Drawing::Size(68, 23);
			this->lbMessage->TabIndex = 37;
			this->lbMessage->Text = L"label1";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Navy;
			this->label2->Location = System::Drawing::Point(15, 62);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(69, 23);
			this->label2->TabIndex = 38;
			this->label2->Text = L"Clave:";
			// 
			// tbClave
			// 
			this->tbClave->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbClave->Location = System::Drawing::Point(91, 59);
			this->tbClave->Name = L"tbClave";
			this->tbClave->PasswordChar = '*';
			this->tbClave->Size = System::Drawing::Size(219, 30);
			this->tbClave->TabIndex = 1;
			this->tbClave->Text = L"121212";
			// 
			// panelKey
			// 
			this->panelKey->BackColor = System::Drawing::Color::DarkKhaki;
			this->panelKey->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panelKey->Controls->Add(this->lbMessage);
			this->panelKey->Controls->Add(this->tbClave);
			this->panelKey->Controls->Add(this->label2);
			this->panelKey->Location = System::Drawing::Point(16, 78);
			this->panelKey->Name = L"panelKey";
			this->panelKey->Size = System::Drawing::Size(417, 203);
			this->panelKey->TabIndex = 40;
			// 
			// PideClave
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(653, 449);
			this->Name = L"PideClave";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"PideClave";
			this->TopMost = true;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &PideClave::PideClave_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->panelKey->ResumeLayout(false);
			this->panelKey->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	protected:
        virtual void btOk_Click(Object ^sender, EventArgs ^e) override
        {
			this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			if(this->tbClave->Text->Trim() == this->rPassword)
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
        }
private: System::Void PideClave_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 Strings::releaseKeyboard(this);
		 }
};
}
#endif
