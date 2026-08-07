#pragma once
#ifdef POSXP
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

extern ::ArrayList *promos;


namespace pos {

	/// <summary>
	/// Summary for MuestraForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MuestraForm : public System::Windows::Forms::Form
	{
	public:
		MuestraForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		void limpiaText()
		{
			this->txtDatos->Text = System::String::Empty;
		}

		void cargaInfo(char *data)
		{
			StringBuilder ^sb = gcnew StringBuilder();
			sb->Length = 0;
			while(*data)
				sb->Append((wchar_t) *data++);
			String ^texto = sb->ToString();
			this->txtDatos->Text += texto;
			this->txtDatos->Text += "\r\n------------------------------\r\n";
		}

		void muestraPromos()
		{
			limpiaText();
			char *mimsg = new char[100];
			for(int i = 0; i < promos->Count(); i++)
			{	
				*mimsg = 0;
				PromoDef *miPromo = (PromoDef *)promos->Item(i);
				sprintf(mimsg, "promo%d", miPromo->num);
				cargaInfo(mimsg);
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MuestraForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  txtDatos;
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
			this->txtDatos = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// txtDatos
			// 
			this->txtDatos->AcceptsReturn = true;
			this->txtDatos->Location = System::Drawing::Point(3, 2);
			this->txtDatos->Multiline = true;
			this->txtDatos->Name = L"txtDatos";
			this->txtDatos->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->txtDatos->Size = System::Drawing::Size(430, 348);
			this->txtDatos->TabIndex = 0;
			// 
			// MuestraForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(445, 375);
			this->Controls->Add(this->txtDatos);
			this->Name = L"MuestraForm";
			this->Text = L"MuestraForm";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
#endif