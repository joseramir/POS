#pragma once
#ifdef POSXP
#include "Buscador.h"
#include "DatosMpagos.h"
#include "BuscaCliSql.h"
#include "StaticDump.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

//extern dbf dbcta;

namespace pos {

	/// <summary>
	/// Resumen de MpagCliente
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class MpagCliente : public Controles::BaseDialog, 
		Controles::IBuscador
	{
	public:

		delegate int ValidateCodDelegate(String ^msg);

		ValidateCodDelegate ^ValidateCod;
	private: System::Windows::Forms::Panel^  panel4;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::TextBox^  tbFechaPib;

	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::TextBox^  tbPercepcion;

	private: System::Windows::Forms::Label^  label11;



	private: System::Windows::Forms::TextBox^  tbAlicuota;

	private: System::Windows::Forms::Label^  lblMessagePercep;
	private: System::Windows::Forms::Label^  lblReparticion;
	private: System::Windows::Forms::TextBox^  tbSaldo;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label16;
	private: System::Windows::Forms::Label^  lblSaldoMut;



	public: 

		String ^validatedCod;

		MpagCliente(void)
		{
			InitializeComponent();
			tb = tbBusca;
			Strings::InterceptKeyboard(this, 4);
			validatedCod = String::Empty;
			lblMessagePercep->Visible = false;
			lblReparticion->Text = "";

			// Elimina los botones en configuraciones sin touchscreen
			if(!usaTouch)
			{
				b0->Visible = false;
				b1->Visible = false;
				b2->Visible = false;
				b3->Visible = false;
				b4->Visible = false;
				b5->Visible = false;
				b6->Visible = false;
				b7->Visible = false;
				b8->Visible = false;
				b9->Visible = false;
				bBorra->Visible = false;
				bEnter->Visible = false;
			}
			if (selfCheckout){
				this->TopMost = true;
				btBusca->Visible = false;
				this->WindowState = FormWindowState::Maximized;
			}
		}

	public: virtual void AcceptKey(Keys key)
	{
		switch(key)
		{
			case Keys::D0: case Keys::NumPad0: tbBusca->Text += "0"; break;
			case Keys::D1: case Keys::NumPad1: tbBusca->Text += "1"; break;
			case Keys::D2: case Keys::NumPad2: tbBusca->Text += "2"; break;
			case Keys::D3: case Keys::NumPad3: tbBusca->Text += "3"; break;
			case Keys::D4: case Keys::NumPad4: tbBusca->Text += "4"; break;
			case Keys::D5: case Keys::NumPad5: tbBusca->Text += "5"; break;
			case Keys::D6: case Keys::NumPad6: tbBusca->Text += "6"; break;
			case Keys::D7: case Keys::NumPad7: tbBusca->Text += "7"; break;
			case Keys::D8: case Keys::NumPad8: tbBusca->Text += "8"; break;
			case Keys::D9: case Keys::NumPad9: tbBusca->Text += "9"; break;

			case Keys::Back:
			case Keys::Left:
				if(tbBusca->Text->Trim()->Length > 0)
				{
					tbBusca->Text = tbBusca->Text->Trim()->Substring(0, tbBusca->Text->Trim()->Length - 1);
					tbBusca->SelectionStart = tbBusca->Text->Trim()->Length;
					tbBusca->SelectionLength = 1;
				}
				break;

			/*case Keys::B:
				this->chkBienUso->Checked = !this->chkBienUso->Checked;
				break;*/

			case Keys::Enter:
				this->bEnter_Click(nullptr, nullptr);
				break;

			case Keys::Escape:
				this->Cancel_Click(nullptr, nullptr);
				break;

			case Keys::Up:
			case Keys::Down:				
				this->btBusca_Click(nullptr, nullptr);				
				break;

			case Keys::F8:
				this->btOk_Click(nullptr, nullptr);
				break;

			default:
				WLog("Tecla desconocida en modo dialog 2: %d", (int)key);
				break;
		}

		if(tbBusca->Text->Trim()->Length > 11)
			tbBusca->Text = tbBusca->Text->Trim()->Substring(0, tbBusca->Text->Trim()->Length - 1);
	}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~MpagCliente()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Panel^  panel1;
	protected: 
	private: System::Windows::Forms::TextBox^  tbCodigo;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Panel^  panel5;
	private: System::Windows::Forms::TextBox^  tbComprador;
	private: System::Windows::Forms::TextBox^  tbMaximo;
	private: System::Windows::Forms::TextBox^  tbConsumo;
	private: System::Windows::Forms::TextBox^  tbTelefono;
	private: System::Windows::Forms::TextBox^  tbLocalidad;
	private: System::Windows::Forms::TextBox^  tbDireccion;
	private: System::Windows::Forms::TextBox^  tbCuit;
	private: System::Windows::Forms::TextBox^  tbCondIva;
	private: System::Windows::Forms::TextBox^  tbNombre;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Button^  btBusca;
	private: System::Windows::Forms::TextBox^  tbBusca;

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MpagCliente::typeid));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->tbSaldo = (gcnew System::Windows::Forms::TextBox());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->lblReparticion = (gcnew System::Windows::Forms::Label());
			this->lblMessagePercep = (gcnew System::Windows::Forms::Label());
			this->tbAlicuota = (gcnew System::Windows::Forms::TextBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->tbFechaPib = (gcnew System::Windows::Forms::TextBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->tbPercepcion = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->tbCodigo = (gcnew System::Windows::Forms::TextBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->tbComprador = (gcnew System::Windows::Forms::TextBox());
			this->tbMaximo = (gcnew System::Windows::Forms::TextBox());
			this->tbConsumo = (gcnew System::Windows::Forms::TextBox());
			this->tbTelefono = (gcnew System::Windows::Forms::TextBox());
			this->tbLocalidad = (gcnew System::Windows::Forms::TextBox());
			this->tbDireccion = (gcnew System::Windows::Forms::TextBox());
			this->tbCuit = (gcnew System::Windows::Forms::TextBox());
			this->tbCondIva = (gcnew System::Windows::Forms::TextBox());
			this->tbNombre = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->btBusca = (gcnew System::Windows::Forms::Button());
			this->tbBusca = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->lblSaldoMut = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->Controls->Add(this->panel1);
			this->panel->Controls->Add(this->panel2);
			this->panel->Controls->Add(this->label16);
			this->panel->Size = System::Drawing::Size(651, 506);
			this->panel->Controls->SetChildIndex(this->label16, 0);
			this->panel->Controls->SetChildIndex(this->panel2, 0);
			this->panel->Controls->SetChildIndex(this->panel1, 0);
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
			// 
			// btOk
			// 
			this->btOk->Click += gcnew System::EventHandler(this, &MpagCliente::btOk_Click);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::DarkKhaki;
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel1->Controls->Add(this->lblSaldoMut);
			this->panel1->Controls->Add(this->tbSaldo);
			this->panel1->Controls->Add(this->label15);
			this->panel1->Controls->Add(this->lblReparticion);
			this->panel1->Controls->Add(this->lblMessagePercep);
			this->panel1->Controls->Add(this->tbAlicuota);
			this->panel1->Controls->Add(this->label14);
			this->panel1->Controls->Add(this->tbFechaPib);
			this->panel1->Controls->Add(this->label13);
			this->panel1->Controls->Add(this->tbPercepcion);
			this->panel1->Controls->Add(this->label11);
			this->panel1->Controls->Add(this->panel4);
			this->panel1->Controls->Add(this->tbCodigo);
			this->panel1->Controls->Add(this->label12);
			this->panel1->Controls->Add(this->panel5);
			this->panel1->Controls->Add(this->tbComprador);
			this->panel1->Controls->Add(this->tbMaximo);
			this->panel1->Controls->Add(this->tbConsumo);
			this->panel1->Controls->Add(this->tbTelefono);
			this->panel1->Controls->Add(this->tbLocalidad);
			this->panel1->Controls->Add(this->tbDireccion);
			this->panel1->Controls->Add(this->tbCuit);
			this->panel1->Controls->Add(this->tbCondIva);
			this->panel1->Controls->Add(this->tbNombre);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->label10);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->label9);
			this->panel1->Controls->Add(this->label6);
			this->panel1->Controls->Add(this->label8);
			this->panel1->Controls->Add(this->label7);
			this->panel1->Location = System::Drawing::Point(10, 144);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(433, 350);
			this->panel1->TabIndex = 37;
			// 
			// tbSaldo
			// 
			this->tbSaldo->BackColor = System::Drawing::Color::White;
			this->tbSaldo->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbSaldo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbSaldo->Location = System::Drawing::Point(334, 183);
			this->tbSaldo->Name = L"tbSaldo";
			this->tbSaldo->ReadOnly = true;
			this->tbSaldo->Size = System::Drawing::Size(92, 21);
			this->tbSaldo->TabIndex = 40;
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label15->Location = System::Drawing::Point(250, 187);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(81, 13);
			this->label15->TabIndex = 39;
			this->label15->Text = L"Saldo Promo:";
			// 
			// lblReparticion
			// 
			this->lblReparticion->AutoSize = true;
			this->lblReparticion->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblReparticion->Location = System::Drawing::Point(228, 6);
			this->lblReparticion->Name = L"lblReparticion";
			this->lblReparticion->Size = System::Drawing::Size(54, 16);
			this->lblReparticion->TabIndex = 38;
			this->lblReparticion->Text = L"label15";
			// 
			// lblMessagePercep
			// 
			this->lblMessagePercep->AutoSize = true;
			this->lblMessagePercep->Font = (gcnew System::Drawing::Font(L"Tahoma", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblMessagePercep->ForeColor = System::Drawing::Color::Red;
			this->lblMessagePercep->Location = System::Drawing::Point(132, 268);
			this->lblMessagePercep->Name = L"lblMessagePercep";
			this->lblMessagePercep->Size = System::Drawing::Size(153, 19);
			this->lblMessagePercep->TabIndex = 37;
			this->lblMessagePercep->Text = L"Aplica Percepción";
			// 
			// tbAlicuota
			// 
			this->tbAlicuota->BackColor = System::Drawing::Color::White;
			this->tbAlicuota->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbAlicuota->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbAlicuota->Location = System::Drawing::Point(135, 318);
			this->tbAlicuota->Name = L"tbAlicuota";
			this->tbAlicuota->ReadOnly = true;
			this->tbAlicuota->Size = System::Drawing::Size(45, 21);
			this->tbAlicuota->TabIndex = 33;
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label14->Location = System::Drawing::Point(70, 321);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(56, 13);
			this->label14->TabIndex = 32;
			this->label14->Text = L"Alicuota:";
			// 
			// tbFechaPib
			// 
			this->tbFechaPib->BackColor = System::Drawing::Color::White;
			this->tbFechaPib->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbFechaPib->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbFechaPib->Location = System::Drawing::Point(261, 293);
			this->tbFechaPib->Name = L"tbFechaPib";
			this->tbFechaPib->ReadOnly = true;
			this->tbFechaPib->Size = System::Drawing::Size(70, 21);
			this->tbFechaPib->TabIndex = 31;
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label13->Location = System::Drawing::Point(214, 295);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(43, 13);
			this->label13->TabIndex = 30;
			this->label13->Text = L"Fecha:";
			// 
			// tbPercepcion
			// 
			this->tbPercepcion->BackColor = System::Drawing::Color::White;
			this->tbPercepcion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbPercepcion->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbPercepcion->Location = System::Drawing::Point(135, 293);
			this->tbPercepcion->Name = L"tbPercepcion";
			this->tbPercepcion->ReadOnly = true;
			this->tbPercepcion->Size = System::Drawing::Size(35, 21);
			this->tbPercepcion->TabIndex = 29;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label11->Location = System::Drawing::Point(58, 295);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(72, 13);
			this->label11->TabIndex = 28;
			this->label11->Text = L"Percepción:";
			// 
			// panel4
			// 
			this->panel4->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel4->Location = System::Drawing::Point(0, 260);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(433, 4);
			this->panel4->TabIndex = 27;
			// 
			// tbCodigo
			// 
			this->tbCodigo->BackColor = System::Drawing::Color::White;
			this->tbCodigo->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCodigo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCodigo->Location = System::Drawing::Point(135, 4);
			this->tbCodigo->Name = L"tbCodigo";
			this->tbCodigo->ReadOnly = true;
			this->tbCodigo->Size = System::Drawing::Size(87, 21);
			this->tbCodigo->TabIndex = 26;
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label12->Location = System::Drawing::Point(81, 9);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(48, 13);
			this->label12->TabIndex = 25;
			this->label12->Text = L"Código:";
			// 
			// panel5
			// 
			this->panel5->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->panel5->Location = System::Drawing::Point(0, 174);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(433, 4);
			this->panel5->TabIndex = 21;
			// 
			// tbComprador
			// 
			this->tbComprador->BackColor = System::Drawing::Color::White;
			this->tbComprador->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbComprador->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbComprador->Location = System::Drawing::Point(135, 235);
			this->tbComprador->Name = L"tbComprador";
			this->tbComprador->ReadOnly = true;
			this->tbComprador->Size = System::Drawing::Size(251, 21);
			this->tbComprador->TabIndex = 24;
			// 
			// tbMaximo
			// 
			this->tbMaximo->BackColor = System::Drawing::Color::White;
			this->tbMaximo->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbMaximo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbMaximo->Location = System::Drawing::Point(135, 210);
			this->tbMaximo->Name = L"tbMaximo";
			this->tbMaximo->ReadOnly = true;
			this->tbMaximo->Size = System::Drawing::Size(113, 21);
			this->tbMaximo->TabIndex = 23;
			// 
			// tbConsumo
			// 
			this->tbConsumo->BackColor = System::Drawing::Color::White;
			this->tbConsumo->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbConsumo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbConsumo->Location = System::Drawing::Point(135, 183);
			this->tbConsumo->Name = L"tbConsumo";
			this->tbConsumo->ReadOnly = true;
			this->tbConsumo->Size = System::Drawing::Size(113, 21);
			this->tbConsumo->TabIndex = 22;
			// 
			// tbTelefono
			// 
			this->tbTelefono->BackColor = System::Drawing::Color::White;
			this->tbTelefono->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbTelefono->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbTelefono->Location = System::Drawing::Point(135, 148);
			this->tbTelefono->Name = L"tbTelefono";
			this->tbTelefono->ReadOnly = true;
			this->tbTelefono->Size = System::Drawing::Size(113, 21);
			this->tbTelefono->TabIndex = 21;
			// 
			// tbLocalidad
			// 
			this->tbLocalidad->BackColor = System::Drawing::Color::White;
			this->tbLocalidad->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbLocalidad->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbLocalidad->Location = System::Drawing::Point(135, 124);
			this->tbLocalidad->Name = L"tbLocalidad";
			this->tbLocalidad->ReadOnly = true;
			this->tbLocalidad->Size = System::Drawing::Size(262, 21);
			this->tbLocalidad->TabIndex = 20;
			// 
			// tbDireccion
			// 
			this->tbDireccion->BackColor = System::Drawing::Color::White;
			this->tbDireccion->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbDireccion->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbDireccion->Location = System::Drawing::Point(135, 100);
			this->tbDireccion->Name = L"tbDireccion";
			this->tbDireccion->ReadOnly = true;
			this->tbDireccion->Size = System::Drawing::Size(262, 21);
			this->tbDireccion->TabIndex = 19;
			// 
			// tbCuit
			// 
			this->tbCuit->BackColor = System::Drawing::Color::White;
			this->tbCuit->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCuit->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCuit->Location = System::Drawing::Point(135, 76);
			this->tbCuit->Name = L"tbCuit";
			this->tbCuit->ReadOnly = true;
			this->tbCuit->Size = System::Drawing::Size(127, 21);
			this->tbCuit->TabIndex = 18;
			// 
			// tbCondIva
			// 
			this->tbCondIva->BackColor = System::Drawing::Color::White;
			this->tbCondIva->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCondIva->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCondIva->Location = System::Drawing::Point(135, 52);
			this->tbCondIva->Name = L"tbCondIva";
			this->tbCondIva->ReadOnly = true;
			this->tbCondIva->Size = System::Drawing::Size(207, 21);
			this->tbCondIva->TabIndex = 17;
			// 
			// tbNombre
			// 
			this->tbNombre->BackColor = System::Drawing::Color::White;
			this->tbNombre->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbNombre->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbNombre->Location = System::Drawing::Point(135, 28);
			this->tbNombre->Name = L"tbNombre";
			this->tbNombre->ReadOnly = true;
			this->tbNombre->Size = System::Drawing::Size(262, 21);
			this->tbNombre->TabIndex = 16;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(75, 31);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(54, 13);
			this->label2->TabIndex = 1;
			this->label2->Text = L"Nombre:";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(42, 55);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(87, 13);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Condición IVA:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(92, 79);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(37, 13);
			this->label4->TabIndex = 3;
			this->label4->Text = L"CUIT:";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->Location = System::Drawing::Point(8, 238);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(121, 13);
			this->label10->TabIndex = 9;
			this->label10->Text = L"Persona Autorizada:";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(67, 103);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(62, 13);
			this->label5->TabIndex = 4;
			this->label5->Text = L"Dirección:";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(19, 212);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(110, 13);
			this->label9->TabIndex = 8;
			this->label9->Text = L"Consumo Máximo:";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(66, 127);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(63, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"Localidad:";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(28, 186);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(101, 13);
			this->label8->TabIndex = 7;
			this->label8->Text = L"Consumo Actual:";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(70, 151);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(59, 13);
			this->label7->TabIndex = 6;
			this->label7->Text = L"Teléfono:";
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::DarkKhaki;
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel2->Controls->Add(this->btBusca);
			this->panel2->Controls->Add(this->tbBusca);
			this->panel2->Controls->Add(this->label1);
			this->panel2->Location = System::Drawing::Point(10, 65);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(282, 74);
			this->panel2->TabIndex = 38;
			// 
			// btBusca
			// 
			this->btBusca->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btBusca->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btBusca->ForeColor = System::Drawing::Color::Navy;
			this->btBusca->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btBusca.Image")));
			this->btBusca->ImageAlign = System::Drawing::ContentAlignment::TopCenter;
			this->btBusca->Location = System::Drawing::Point(148, 10);
			this->btBusca->Name = L"btBusca";
			this->btBusca->Size = System::Drawing::Size(114, 48);
			this->btBusca->TabIndex = 14;
			this->btBusca->Tag = L"66";
			this->btBusca->Text = L"Busca por Nombre";
			this->btBusca->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->btBusca->UseVisualStyleBackColor = false;
			this->btBusca->Click += gcnew System::EventHandler(this, &MpagCliente::btBusca_Click);
			// 
			// tbBusca
			// 
			this->tbBusca->Location = System::Drawing::Point(9, 37);
			this->tbBusca->Name = L"tbBusca";
			this->tbBusca->Size = System::Drawing::Size(108, 22);
			this->tbBusca->TabIndex = 10;
			this->tbBusca->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MpagCliente::tbBusca_KeyPress);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(9, 14);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(54, 14);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Código:";
			// 
			// label16
			// 
			this->label16->BackColor = System::Drawing::Color::Khaki;
			this->label16->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label16->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->Location = System::Drawing::Point(11, 518);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(434, 83);
			this->label16->TabIndex = 39;
			this->label16->Text = resources->GetString(L"label16.Text");
			// 
			// lblSaldoMut
			// 
			this->lblSaldoMut->AutoSize = true;
			this->lblSaldoMut->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblSaldoMut->Location = System::Drawing::Point(254, 214);
			this->lblSaldoMut->Name = L"lblSaldoMut";
			this->lblSaldoMut->Size = System::Drawing::Size(48, 13);
			this->lblSaldoMut->TabIndex = 41;
			this->lblSaldoMut->Text = L"label17";
			this->lblSaldoMut->Visible = false;
			// 
			// MpagCliente
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(7, 14);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->ClientSize = System::Drawing::Size(651, 506);
			this->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Name = L"MpagCliente";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"MpagCliente";
			this->TopMost = false;
			this->Activated += gcnew System::EventHandler(this, &MpagCliente::MpagCliente_Activated);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MpagCliente::MpagCliente_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->ResumeLayout(false);

		}
	#pragma endregion
	
	protected:

		virtual System::Void bEnter_Click(System::Object^ sender, System::EventArgs^  e) override
		{			
			String ^cod = tb->Text->Trim();
			lblSaldoMut->Visible = false;
			if(cod->Length != 0)
			{
				if (cod->Length <= 12 && Convert::ToUInt64(cod) <= 0)
				{
					ClearMsg();
					Alert("Codigo cliente = 0, intente de nuevo o", "Realice indices desde Sistema de Registradoras");
					return;
				}
				int res = 0;

				if (FactBconCuit)
				{
					if (!vCuit(cod->Trim()))
					{
						Alert("Solo se Realizan Facturas a clientes con CUIT", "");
						return;
					}
				}
				res = ValidateCod(cod);
				String ^civa = CuentaCorriente::CondIva;
				if (CuentaCorriente::CondIva == "RI" || CuentaCorriente::CondIva == "NI" || 
					CuentaCorriente::CondIva == "EX" || CuentaCorriente::CondIva == "MO")
				{
					if (!vCuit(CuentaCorriente::Cuit))
					{
						ClearMsg();
						Alert("El CUIT del cliente es invalido!!", "Registrelo correctamente");
						return;
					}
				}
				
				if(res)
				{
					this->tbCodigo->Text = CuentaCorriente::Codigo;
					this->tbComprador->Text = CuentaCorriente::Comprador;
					this->tbCondIva->Text = CuentaCorriente::CondIva;
					this->tbConsumo->Text = CuentaCorriente::Saldo;
					this->tbCuit->Text = CuentaCorriente::Cuit;
					this->tbDireccion->Text = CuentaCorriente::Direccion;
					this->tbLocalidad->Text = CuentaCorriente::Localidad;
					this->tbMaximo->Text = CuentaCorriente::SaldoMaximo;
					this->tbNombre->Text = CuentaCorriente::Nombre;
					this->tbTelefono->Text = CuentaCorriente::Telefono;
					this->tbPercepcion->Text = CuentaCorriente::Percepcion;
					
					this->lblReparticion->Text = CuentaCorriente::RepaDes;

					if (Dump::actCliente != nullptr && !Dump::actCliente->UsadoMutual && Dump::actCliente->SaldoMutual > 0)
					{
						lblSaldoMut->Visible = true;
						lblSaldoMut->Text = "Saldo Mutual: " + Dump::actCliente->SaldoMutual.ToString("F2");
					}


					this->tbSaldo->Text = Strings::CharToString(c_saldopromo);
					if (CuentaCorriente::FechaPib->Trim() != String::Empty && 
							CuentaCorriente::FechaPib->Trim()->Length == 8){
						System::Int32 fanio = System::Convert::ToInt32(CuentaCorriente::FechaPib->Substring(0,4));
						System::Int32 fmes = System::Convert::ToInt32(CuentaCorriente::FechaPib->Substring(4, 2));
						System::Int32 fdia = System::Convert::ToInt32(CuentaCorriente::FechaPib->Substring(6, 2));
						System::DateTime ^tmpfpib = gcnew System::DateTime(fanio, fmes, fdia);						
						this->tbFechaPib->Text = tmpfpib->Date.ToString("dd/MM/yyyy");
						long tmpfpercep = atol(c_fechapib);
						long tmpfhoy = (long) date();						
						if (CuentaCorriente::Percepcion == "Si" && (tmpfpercep < tmpfhoy)){
							if (atof(c_alicuota) > 0.0 && AplicarPercep)
							{
								this->lblMessagePercep->Text = "Aplica Percepción";								
							} else
								this->lblMessagePercep->Text = "No Aplica Percepción";

						}
						else
							this->lblMessagePercep->Text = "No Aplica Percepción";
					}
					else
					{
						this->tbFechaPib->Text = CuentaCorriente::FechaPib;
						if (CuentaCorriente::Percepcion == "Si"){
							if (atof(c_alicuota) > 0.0 && AplicarPercep)
							{
								this->lblMessagePercep->Text = "Aplica Percepción";								
							}else
								this->lblMessagePercep->Text = "No Aplica Percepción";
						}
						else
							this->lblMessagePercep->Text = "No Aplica Percepción";
					}
					this->lblMessagePercep->Visible = true;					
					this->tbAlicuota->Text = CuentaCorriente::Alicuota;					
					this->btOk->Enabled = true;

					validatedCod = cod;
				}
				else
				{
					this->btOk->Enabled = false;
					this->lblMessagePercep->Visible = false;					
				}
			}
			else			
				this->btOk->Enabled = false;			
			
		}

		System::Void btBusca_Click(System::Object^ sender, System::EventArgs^  e)
		{
			
			BuscaCliSql ^buscador = gcnew BuscaCliSql();
			buscador->Text = "Buscar Clientes por su Nombre";						
			
			buscador->ShowDialog();
			if(buscador->DialogResult == System::Windows::Forms::DialogResult::OK)
			{
				String^ auxcod = buscador->actualCod;
				System::Int64 codcliente = 0;
				System::Int64::TryParse(auxcod, codcliente);
				if (auxcod->Trim()->Length <=12 && (codcliente <= 0))
				{
					ClearMsg();
					Alert("Codigo cliente = 0, intente de nuevo o", "Realice indices desde Sistema Stock");
				}
				else
				{
					tb->Text = auxcod;				
					tbBusca->Text = auxcod;
					bEnter_Click(nullptr, nullptr);
				}
			}			
			delete buscador;
		}

		System::Void virtual btOk_Click(Object ^sender, EventArgs ^e) override
		{
			if(tb->Text->Trim() != validatedCod) 
				return;
			
			BaseDialog::btOk_Click(sender, e);

			if (validatedCod->Trim() == String::Empty)
				this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
		}

		System::Void virtual Cancel_Click(Object ^sender, EventArgs ^e) override
		{
			STRCPY(c_condiva, "");
			ClienteBenef = 0;
			BaseDialog::Cancel_Click(sender, e);			
		}

		System::Void MpagCliente_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
		{
			 Strings::releaseKeyboard(this);
		}

		System::Void MpagCliente_Activated(System::Object^  sender, System::EventArgs^  e) 
		{
			 tbBusca->Focus();
		}

		
private: System::Void tbBusca_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				e->Handled = true;
			}
		 bool vCuit(String ^cuit)
		 {
			 bool aux = false;

			 int i, j, suma;
			 array<int >^ digicuit = gcnew array<int>{ 5, 4, 3, 2, 7, 6, 5, 4, 3, 2 };
			 System::Text::StringBuilder ^tcuit = gcnew System::Text::StringBuilder();

			 //   Valida el largo del cuit.			  
			 if(cuit->Length >= 11 || cuit->Length <= 13) 
			 {				 
				 //   Remueve posibles guiones.
				 for each(System::Char car in cuit)
				 {
					 if (System::Char::IsDigit(car))
						 tcuit->Append(car);
				 }
				 if(tcuit->Length == 11)
				 {
					 //   Calcula cada digito, controlandolo!!!
					 for(i = suma = 0; i < 10; i++)
					 {						 
						 j = System::Convert::ToUInt16(tcuit[i]) - 48;
						 suma += j * digicuit[i];
					 }

					 j = (11-(suma % 11)) % 11;
					 if(j == (Convert::ToUInt16(tcuit[10]) - 48)) 
						 aux = true;
				 }
			 }
			 return aux;
		 }

};
}
#endif
