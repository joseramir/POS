#pragma once
#ifdef POSXP
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "strings.h"

namespace pos {

	/// <summary>
	/// Resumen de PrecManual
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class PrecManual : public Controles::BaseDialog
	{
	public:
		double Precio;

		PrecManual(String ^codNom, double precio)
		{
			InitializeComponent();
			Precio = precio;
			lbNom->Text = codNom;

			Strings::InterceptKeyboard(this, 1);
			TabControls->Add(tbPrecio);
			tbPrecio->Text = precio.ToString("N2");

			// Elimina los botones en configuraciones sin touchscreen
			this->ExtendedKeyboardEnabled = usaTouch;
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~PrecManual()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Panel^  panelKey;
	protected: 
	private: System::Windows::Forms::Label^  lbNom;
	private: System::Windows::Forms::TextBox^  tbPrecio;
	private: System::Windows::Forms::Label^  label2;

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
			this->panelKey = (gcnew System::Windows::Forms::Panel());
			this->lbNom = (gcnew System::Windows::Forms::Label());
			this->tbPrecio = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->panelKey->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(128)), 
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->panel->Controls->Add(this->panelKey);
			this->panel->Size = System::Drawing::Size(656, 441);
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
			this->panel->Controls->SetChildIndex(this->panelKey, 0);
			// 
			// Titulo
			// 
			this->Titulo->Size = System::Drawing::Size(278, 23);
			this->Titulo->Text = L"Facturación a Precio Manual";
			// 
			// panelKey
			// 
			this->panelKey->BackColor = System::Drawing::Color::DarkKhaki;
			this->panelKey->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panelKey->Controls->Add(this->lbNom);
			this->panelKey->Controls->Add(this->tbPrecio);
			this->panelKey->Controls->Add(this->label2);
			this->panelKey->Location = System::Drawing::Point(19, 77);
			this->panelKey->Name = L"panelKey";
			this->panelKey->Size = System::Drawing::Size(417, 203);
			this->panelKey->TabIndex = 41;
			// 
			// lbNom
			// 
			this->lbNom->AutoSize = true;
			this->lbNom->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbNom->ForeColor = System::Drawing::Color::Navy;
			this->lbNom->Location = System::Drawing::Point(15, 19);
			this->lbNom->Name = L"lbNom";
			this->lbNom->Size = System::Drawing::Size(68, 23);
			this->lbNom->TabIndex = 37;
			this->lbNom->Text = L"label1";
			// 
			// tbPrecio
			// 
			this->tbPrecio->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbPrecio->Location = System::Drawing::Point(104, 59);
			this->tbPrecio->Name = L"tbPrecio";
			this->tbPrecio->Size = System::Drawing::Size(123, 30);
			this->tbPrecio->TabIndex = 1;
			this->tbPrecio->Text = L"121212";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Navy;
			this->label2->Location = System::Drawing::Point(15, 62);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(76, 23);
			this->label2->TabIndex = 38;
			this->label2->Text = L"Precio:";
			// 
			// PrecManual
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(656, 441);
			this->Name = L"PrecManual";
			this->Text = L"PrecManual";
			this->TopMost = true;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &PrecManual::PrecManual_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->panelKey->ResumeLayout(false);
			this->panelKey->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	protected: virtual void btOk_Click(Object ^sender, EventArgs ^e) override
        {
			this->DialogResult = System::Windows::Forms::DialogResult::OK;
			try { Precio = double::Parse(tbPrecio->Text); }
			catch(...) 
			{ 
				Precio = 0; 
				this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			}
			this->Close();
        }
	private: System::Void PrecManual_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 Strings::releaseKeyboard(this);
			 }
};
}
#endif
