#pragma once
#ifdef POSXP

#define ELIGE_FOR_INCLUDED
#include "arraylist.h"
#include "strings.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;
using namespace System::Drawing;

namespace pos {

	/// <summary>
	/// Resumen de EligeForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class EligeForm : public System::Windows::Forms::Form,
		Controles::IBuscador
	{
	private:
		int (*Valid)(int);
	private: System::Windows::Forms::PictureBox^  flagDer;
	private: System::Windows::Forms::PictureBox^  flagIzq;
			 int focused;

	public:
		EligeForm(char *tit, char *pie, int (*exec)(int))
		{
			InitializeComponent();

			// Pone el 'TopMost' de la form, pero solo si no estamos en modo debug.
			/*String ^sd = System::Environment::GetEnvironmentVariable("SYSDEBUG");
			this->TopMost = (sd != nullptr && sd->ToLower() == "on")? false: true;*/

			Valid = exec;
			String ^_tit = Strings::CharToString(tit),
				^_pie = Strings::CharToString(pie);
			if(_pie->Length > 0)
				_tit += " (" + _pie + ")";
			this->Text = _tit;
			Strings::InterceptKeyboard(this, 2);
		}

		property int Actual;

		void ChangeButton(int index, char *text, bool enabled)
		{
			int xindex = 0;
			for(int i=0; i<Controls->Count; i++)
			{
				Button ^b = dynamic_cast<Button ^>(this->Controls[i]);
				if(b != nullptr)
				{
					if(xindex == index)
					{
						b->Enabled = enabled;
						b->Text = Strings::CharToString(text);
						this->Refresh();
						return;
					}
					xindex++;
				}
			}
		}

		String ^GetButtonText(int index)
		{
			int xindex = 0;
			for(int i=0; i<Controls->Count; i++)
			{
				Button ^b = dynamic_cast<Button ^>(this->Controls[i]);
				if(b != nullptr)
				{
					if(xindex == index)	return b->Text;
					xindex++;
				}
			}
			return nullptr;
		}

		void AgregarRenglon(char *rng)
		{
			//String ^dato = Strings::CharToString(rng);
			String ^dato = gcnew String(rng);
			for each(Control ^ctl in this->Controls)
			{
				Button ^b = dynamic_cast<Button ^>(ctl);
				if(b != nullptr && b->Text == dato)
					return;
			}

			Button ^boton = AgregaBoton(dato);
			boton->Click += gcnew EventHandler(this, &EligeForm::PulsoOpcion);
			boton->Tag = this->Controls->Count - 2;
			Strings::InterceptKeyboard2(boton);
		}


	public:
		void SetTitulo(int item, String ^tit)
		{
			int xindex = 0;
			for(int i=0; i<Controls->Count; i++)
			{
				Button ^b = dynamic_cast<Button ^>(this->Controls[i]);
				if(b != nullptr)
				{
					if(xindex == item)
					{
						b->Text = tit;
						return;
					}
					xindex++;
				}
			}
		}

	protected:
		Button ^AgregaBoton(String ^dato)
		{
			Button ^nue = gcnew Button();
			nue->Text = dato;
			nue->Width = 475;
			nue->Height = 43;
			nue->BackColor = Color::FromArgb(255, 255, 192);
			nue->Font = gcnew System::Drawing::Font("Tahoma", 9);
			this->Controls->Add(nue);
			return nue;
		}

		System::Void EligeForm_Load(System::Object^  sender, System::EventArgs^  e) 
		{
			Button ^salir = AgregaBoton("** Cancelar **");
			salir->Click += gcnew EventHandler(this, &EligeForm::Salir);
			int y = 11;
			for each(Control ^ctl in this->Controls)
			{
				Button ^b = dynamic_cast<Button ^>(ctl);
				if(b != nullptr)
				{
					ctl->Left = 18;
					ctl->Top = y;
					y += ctl->Height + 4;
					b->GotFocus += gcnew EventHandler(this, &EligeForm::BotonGotFocus);
				}
			}

			if(y > 500) y = 500;
			this->Height = y + 50;
		}

		System::Void Salir(Object ^sender, EventArgs ^e)
		{
			Button ^b = (Button ^) this->Controls[this->Controls->Count - 1];
			if(b->Text != "** Cancelar **")
				this->DialogResult = System::Windows::Forms::DialogResult::OK;
			else
				this->DialogResult = System::Windows::Forms::DialogResult::Cancel;

			this->Close();
		}

		System::Void PulsoOpcion(Object ^sender, EventArgs ^e)
		{
			Actual = 0;
			Control ^cSender = (Control ^)sender;
			try { Actual = (Int32)cSender->Tag; } catch(...) {}
			if(Valid != NULL)
			{
				int res = (*Valid)(Actual);
				if(res != 0)
				{
					this->DialogResult = System::Windows::Forms::DialogResult::OK;
					this->Close();
				}
			}
			else
			{
				this->DialogResult = (cSender->Text == "** Cancelar **")?
					System::Windows::Forms::DialogResult::Cancel:
					System::Windows::Forms::DialogResult::OK;	
				this->Close();
			}
		}

		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~EligeForm()
		{
			if (components)
			{
				delete components;
			}
		}

		private: void MoveFocused(int offset)
		{
			while(true)
			{
				focused += offset;
				if(focused >= Controls->Count) focused = 0;
				if(focused < 0) focused = Controls->Count - 1;
				Button ^b = dynamic_cast<Button ^>(Controls[focused]);
				if(b != nullptr) break;
			}
		}

		public: virtual void AcceptKey(Keys key)
		{
			bool proceder = true;
			switch(key)
			{
				case Keys::Down:
					MoveFocused(1);
					break;

				case Keys::Up:
					MoveFocused(-1);
					break;

				case Keys::Enter:
					PulsoOpcion(Controls[focused], nullptr);
					proceder = false;
					break;

				case Keys::Escape:
					Salir(nullptr, nullptr);
					proceder = false;
					break;
			}

			if(proceder)
				ShowFocused();
		}

	private:

		void BotonGotFocus(Object ^sender, EventArgs ^e)
		{
			Button ^b = (Button ^)sender;
			for(int i=0; i<Controls->Count; i++)
			{
				if(Controls[i] == b)
				{
					focused = i;
					break;
				}
			}

			// Reposiciona las banderas indicadoras.
            Point loc = b->Parent->PointToScreen(b->Location);
            loc.Y += ((b->Height > flagIzq->Height) ?
                      (b->Height - flagIzq->Height) / 2 :
                       0);
            
			loc.X -= flagIzq->Width + 2;
            flagIzq->Location = flagIzq->Parent->PointToClient(loc);

            loc.X += b->Width + flagDer->Width + 2;
            flagDer->Location = flagDer->Parent->PointToClient(loc);
            flagIzq->BringToFront();
            flagDer->BringToFront();
            flagIzq->Visible = true;
			flagDer->Visible = true;
		}

		void ShowFocused()
		{
			Control ^ctl = Controls[focused];
			ctl->Focus();

		}

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(EligeForm::typeid));
			this->flagDer = (gcnew System::Windows::Forms::PictureBox());
			this->flagIzq = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->flagDer))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->flagIzq))->BeginInit();
			this->SuspendLayout();
			// 
			// flagDer
			// 
			this->flagDer->Enabled = false;
			this->flagDer->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"flagDer.Image")));
			this->flagDer->Location = System::Drawing::Point(198, 202);
			this->flagDer->Name = L"flagDer";
			this->flagDer->Size = System::Drawing::Size(16, 16);
			this->flagDer->TabIndex = 40;
			this->flagDer->TabStop = false;
			this->flagDer->Visible = false;
			// 
			// flagIzq
			// 
			this->flagIzq->Enabled = false;
			this->flagIzq->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"flagIzq.Image")));
			this->flagIzq->Location = System::Drawing::Point(165, 196);
			this->flagIzq->Name = L"flagIzq";
			this->flagIzq->Size = System::Drawing::Size(16, 16);
			this->flagIzq->TabIndex = 39;
			this->flagIzq->TabStop = false;
			this->flagIzq->Visible = false;
			// 
			// EligeForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->ClientSize = System::Drawing::Size(546, 431);
			this->ControlBox = false;
			this->Controls->Add(this->flagDer);
			this->Controls->Add(this->flagIzq);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Location = System::Drawing::Point(205, 5);
			this->Name = L"EligeForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Elija una Opción";
			this->TopMost = true;
			this->Load += gcnew System::EventHandler(this, &EligeForm::EligeForm_Load);
			this->Activated += gcnew System::EventHandler(this, &EligeForm::EligeForm_Activated);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &EligeForm::EligeForm_FormClosed);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->flagDer))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->flagIzq))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void EligeForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 Strings::releaseKeyboard(this);
			 }

			 bool yaActivada;

private: System::Void EligeForm_Activated(System::Object^  sender, System::EventArgs^  e) {
			 if(!yaActivada)
			 {
				yaActivada = true;
				focused = Controls->Count;
				MoveFocused(1);
				ShowFocused();
			}
		 }
};
}
#endif
