#pragma once
#ifdef POSXP

using namespace System;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "Strings.h"

void FlushScanner(void);		// declarados en pos.h
void BeepScanner(void);

namespace pos {

	/// <summary>
	/// Resumen de AlertForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class AlertForm : public System::Windows::Forms::Form, Controles::IBuscador
	{
	private:

		delegate void InvokeFormClose();

		InvokeFormClose ^invokeClose;
	private: System::Windows::Forms::Timer^  beepTimer;


		static StringBuilder ^sb = gcnew StringBuilder();

		static String ^PrepareBuffer(char *data)
		{
			sb->Length = 0;
			while(*data)
				sb->Append((wchar_t) *data++);
			return sb->ToString();
		}
	private: System::Windows::Forms::Button^  btNo;
	private: System::Windows::Forms::Button^  btSi;

	public:

		bool Cancelo;

		AlertForm(char *d1, char *d2, bool sino, bool conOk, bool enableBeep)
		{
			InitializeComponent();
			invokeClose = gcnew InvokeFormClose(this, &AlertForm::DoClose);

			// Pone el 'TopMost' de la form, pero solo si no estamos en modo debug.
			/*String ^sd = System::Environment::GetEnvironmentVariable("SYSDEBUG");
			this->TopMost = (sd != nullptr && sd->ToLower() == "on")? false: true;*/
			this->TopMost = true;

			/*this->label1->Text = PrepareBuffer(d1);
			this->label2->Text = PrepareBuffer(d2);		*/	
			this->label1->Text = gcnew String(d1);
			this->label2->Text = gcnew String(d2);

			if(sino)
			{
				this->btSi->Visible = true;
				this->btNo->Visible = true;
				this->btOk->Visible = false;
				this->AcceptButton = this->btSi;
				this->CancelButton = this->btNo;
			}
			else
			{
				this->btSi->Visible = false;
				this->btNo->Visible = false;
				this->btOk->Visible = conOk;
				this->AcceptButton = this->btOk;
				this->CancelButton = this->btOk;
			}
			Strings::InterceptKeyboard(this, 1);

			// Activa el alerta del scanner.
			if((sino || conOk) && enableBeep)
			{
				beepTimer->Enabled = true;
				BeepScanner();
			}			
		}


		//otra opcion para
		AlertForm(char *d1, char *d2)
		{
			InitializeComponent();
			invokeClose = gcnew InvokeFormClose(this, &AlertForm::DoClose);

			// Pone el 'TopMost' de la form, pero solo si no estamos en modo debug.
			/*String ^sd = System::Environment::GetEnvironmentVariable("SYSDEBUG");
			this->TopMost = (sd != nullptr && sd->ToLower() == "on")? false: true;*/
			this->TopMost = true;

			this->label1->Text = PrepareBuffer(d1);
			this->label2->Text = PrepareBuffer(d2);			

			
			this->btSi->Visible = false;
			this->btNo->Visible = false;
			this->btOk->Visible = false;
			this->AcceptButton = this->btOk;
			this->CancelButton = this->btOk;
			
		}

	public: virtual void AcceptKey(Keys key)
		{
			switch(key)
			{
				case Keys::Escape:
					if(btOk->Visible)
						button1_Click(btOk, nullptr);
					else
						btNo_Click(btNo, nullptr);
					break;

				default:
					if((key == Keys::Enter) || ((int)key == 21))
					{
						if(this->btOk->Visible)
							button1_Click(btOk, nullptr);
						else
							btSi_Click(btSi, nullptr);
					}
					break;
			}
		}

	private: void DoClose()
		{
			beepTimer->Enabled = false;
			FlushScanner();
			this->Close();
		}

	public: void AlertClose()
		{
			if(this->InvokeRequired)
				Invoke(invokeClose);
			else
				DoClose();
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~AlertForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Button^  btOk;
	protected: 

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Variable del diseñador requerida.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido del método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(AlertForm::typeid));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->btNo = (gcnew System::Windows::Forms::Button());
			this->btSi = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btOk = (gcnew System::Windows::Forms::Button());
			this->beepTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"panel1.BackgroundImage")));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->Controls->Add(this->btNo);
			this->panel1->Controls->Add(this->btSi);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->btOk);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(483, 144);
			this->panel1->TabIndex = 0;
			// 
			// btNo
			// 
			this->btNo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->btNo->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btNo->ForeColor = System::Drawing::Color::Navy;
			this->btNo->Location = System::Drawing::Point(247, 91);
			this->btNo->Name = L"btNo";
			this->btNo->Size = System::Drawing::Size(222, 41);
			this->btNo->TabIndex = 4;
			this->btNo->Text = L"No";
			this->btNo->UseVisualStyleBackColor = false;
			this->btNo->Click += gcnew System::EventHandler(this, &AlertForm::btNo_Click);
			// 
			// btSi
			// 
			this->btSi->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->btSi->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btSi->ForeColor = System::Drawing::Color::Navy;
			this->btSi->Location = System::Drawing::Point(11, 91);
			this->btSi->Name = L"btSi";
			this->btSi->Size = System::Drawing::Size(222, 41);
			this->btSi->TabIndex = 3;
			this->btSi->Text = L"Si";
			this->btSi->UseVisualStyleBackColor = false;
			this->btSi->Click += gcnew System::EventHandler(this, &AlertForm::btSi_Click);
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Maroon;
			this->label2->Location = System::Drawing::Point(11, 44);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(458, 23);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Intente en no menos de 10 segundos";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Maroon;
			this->label1->Location = System::Drawing::Point(11, 14);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(458, 23);
			this->label1->TabIndex = 1;
			this->label1->Text = L"La Función solicitada no está disponible";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btOk
			// 
			this->btOk->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->btOk->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btOk->ForeColor = System::Drawing::Color::Navy;
			this->btOk->Location = System::Drawing::Point(11, 91);
			this->btOk->Name = L"btOk";
			this->btOk->Size = System::Drawing::Size(458, 41);
			this->btOk->TabIndex = 0;
			this->btOk->Text = L"Borrar y Seguir";
			this->btOk->UseVisualStyleBackColor = false;
			this->btOk->Click += gcnew System::EventHandler(this, &AlertForm::button1_Click);
			// 
			// beepTimer
			// 
			this->beepTimer->Interval = 400;
			this->beepTimer->Tick += gcnew System::EventHandler(this, &AlertForm::beepTimer_Tick);
			// 
			// AlertForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(483, 144);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"AlertForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"AlertForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &AlertForm::AlertForm_FormClosing);
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 DoClose();
		 }

	private: System::Void btNo_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Cancelo = true;
			 DoClose();
		 }

	private: System::Void btSi_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Cancelo = false;
			 DoClose();
		 }
	private: System::Void AlertForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 Strings::releaseKeyboard(this);
		 }
private: System::Void beepTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
			 BeepScanner();
		 }
			
};
}
#endif
