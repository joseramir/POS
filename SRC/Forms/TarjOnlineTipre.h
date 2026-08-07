#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Reflection;

using namespace Bitnet::TJOCommon;
using namespace Controles;

#include "pos.h"
#include "Strings.h"
#include "tjonline.h"
#include "total.h"

#include "StaticDump.h"
#include "FUNCS.H"

namespace pos {

	/// <summary>
	/// Summary for TarjOnlineTipre
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class TarjOnlineTipre : public Controles::BaseDialog
	{

	private:
		delegate void InvokeSupervCode();

		//TarjInfo *actualTarj;
		double pend;
		InvokeSupervCode ^supervDelegate;
		bool globalTouchEvent;	
		bool validatingVenc;
		bool EsCompra;
		bool IngManual;
		bool tarjVencida;
		bool WebServiceTrabajando;
		bool ValidoExtraCash;
		TipreRequest ^actualTipre;

		List <PromoMedioPago ^> ^actPromoMp;

		int CantTotalTarjetas;
	private: System::Windows::Forms::ComboBox^  cboPlan;
	private: System::Windows::Forms::Button^  btnManual;

	private: System::Windows::Forms::Button^  btnTarjeta;
	private: System::Windows::Forms::ComboBox^  cboTarjeta;
	private: System::Windows::Forms::Button^  btnAdd;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Button^  btnDot;
	private: System::Windows::Forms::TextBox^  txtImpCashBack;
	private: System::Windows::Forms::Label^  lbExtraCash;


		ListViewItem ^itemActual;

	public:
		TarjOnlineTipre(double pendiente, bool pEsCompra)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			IngManual = false;
			globalTouchEvent = false;
			this->pend = pendiente;
			this->CardReaderEnabled = true;
			//this->tbCuotas->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloNumeros_KeyPress);
			this->ult4Dig->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloNumeros_KeyPress);
			this->tbCvv->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloNumeros_KeyPress);
			this->tbDni->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloDocumento_KeyPress);
			this->tbMonto->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloMonto_KeyPress);
			this->txtImpCashBack->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineTipre::txtSoloMonto_KeyPress);
			supervDelegate = gcnew InvokeSupervCode(this, &TarjOnlineTipre::DoSupervCode);

			// Intercepta el teclado.
			Strings::InterceptKeyboard(this, 1);

			// Elimina el teclado si no estamos sobre una touchscreen.
			this->ExtendedKeyboardEnabled = usaTouch;
			this->EsCompra = pEsCompra;

			if (!this->EsCompra)
				Titulo->Text = "Anulación de Compra con Tarjeta";

			// Prepara la tabulacion de los elementos.
			TabControls->Add(this->tbCard);
			TabControls->Add(this->tbVenc);
			TabControls->Add(this->tbNom);
			TabControls->Add(this->ult4Dig);
			TabControls->Add(this->tbDni);
			TabControls->Add(this->tbCvv);
			btnAdd->Visible = usaTouch;
			btnDot->Visible = usaTouch;
			if (this->EsCompra)
			{				
				TabControls->Add(this->tbMonto);
				TabControls->Add(this->cboPlan);
				TabControls->Add(this->txtImpCashBack);
				this->actualTipre = gcnew TipreRequest();
				CantTotalTarjetas = System::Convert::ToUInt16(System::Configuration::ConfigurationManager::AppSettings["tiprecanttarj"]);
			}	
			

			// Sincroniza la lista de tarjetas cargadas
			SincroLista();
			SincroTotal();
			GenValidacion();
			LimpiarControles();

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TarjOnlineTipre()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lbCardHelp;
	private: System::Windows::Forms::Label^  lbFaltan;
	private: System::Windows::Forms::Label^  lbTotal;

	private: System::Windows::Forms::TextBox^  tbDummy;



	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  ult4Dig;
	private: System::Windows::Forms::Label^  lbMarca;


	private: System::Windows::Forms::Label^  lbCvv;
	private: System::Windows::Forms::TextBox^  tbCvv;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  tbNom;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  tbVenc;
	private: System::Windows::Forms::Label^  lbReca;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  tbDni;
	private: System::Windows::Forms::Label^  label3;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  tbMonto;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  tbCard;
	private: System::Windows::Forms::ListView^  lv;
	private: System::Windows::Forms::ColumnHeader^  colMarca;
	private: System::Windows::Forms::ColumnHeader^  colNumero;
	private: System::Windows::Forms::ColumnHeader^  colCuotas;
	private: System::Windows::Forms::ColumnHeader^  colMonto;

	private: System::Windows::Forms::Timer^  timerFaltan;
	private: System::ComponentModel::IContainer^  components;
	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::ListViewItem^  listViewItem2 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {L"Mastercard", 
				L"**** - 1241", L"12", L"15.21"}, -1));
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TarjOnlineTipre::typeid));
			this->lbCardHelp = (gcnew System::Windows::Forms::Label());
			this->lbFaltan = (gcnew System::Windows::Forms::Label());
			this->lbTotal = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->ult4Dig = (gcnew System::Windows::Forms::TextBox());
			this->lbMarca = (gcnew System::Windows::Forms::Label());
			this->lbCvv = (gcnew System::Windows::Forms::Label());
			this->tbCvv = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->tbNom = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tbVenc = (gcnew System::Windows::Forms::TextBox());
			this->lbReca = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->tbDni = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbMonto = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tbCard = (gcnew System::Windows::Forms::TextBox());
			this->lv = (gcnew System::Windows::Forms::ListView());
			this->colMarca = (gcnew System::Windows::Forms::ColumnHeader());
			this->colNumero = (gcnew System::Windows::Forms::ColumnHeader());
			this->colCuotas = (gcnew System::Windows::Forms::ColumnHeader());
			this->colMonto = (gcnew System::Windows::Forms::ColumnHeader());
			this->timerFaltan = (gcnew System::Windows::Forms::Timer(this->components));
			this->tbDummy = (gcnew System::Windows::Forms::TextBox());
			this->cboPlan = (gcnew System::Windows::Forms::ComboBox());
			this->btnManual = (gcnew System::Windows::Forms::Button());
			this->cboTarjeta = (gcnew System::Windows::Forms::ComboBox());
			this->btnTarjeta = (gcnew System::Windows::Forms::Button());
			this->btnAdd = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->btnDot = (gcnew System::Windows::Forms::Button());
			this->lbExtraCash = (gcnew System::Windows::Forms::Label());
			this->txtImpCashBack = (gcnew System::Windows::Forms::TextBox());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->txtImpCashBack);
			this->panel->Controls->Add(this->lbExtraCash);
			this->panel->Controls->Add(this->btnDot);
			this->panel->Controls->Add(this->label9);
			this->panel->Controls->Add(this->label8);
			this->panel->Controls->Add(this->btnAdd);
			this->panel->Controls->Add(this->btnTarjeta);
			this->panel->Controls->Add(this->cboTarjeta);
			this->panel->Controls->Add(this->btnManual);
			this->panel->Controls->Add(this->cboPlan);
			this->panel->Controls->Add(this->lbCardHelp);
			this->panel->Controls->Add(this->tbDummy);
			this->panel->Controls->Add(this->label5);
			this->panel->Controls->Add(this->ult4Dig);
			this->panel->Controls->Add(this->lbMarca);
			this->panel->Controls->Add(this->lbCvv);
			this->panel->Controls->Add(this->tbCvv);
			this->panel->Controls->Add(this->label7);
			this->panel->Controls->Add(this->tbNom);
			this->panel->Controls->Add(this->label6);
			this->panel->Controls->Add(this->tbVenc);
			this->panel->Controls->Add(this->lbReca);
			this->panel->Controls->Add(this->label4);
			this->panel->Controls->Add(this->tbDni);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->tbMonto);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->tbCard);
			this->panel->Controls->Add(this->lv);
			this->panel->Controls->Add(this->lbTotal);
			this->panel->Controls->Add(this->lbFaltan);
			this->panel->Size = System::Drawing::Size(659, 550);
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
			this->panel->Controls->SetChildIndex(this->lbFaltan, 0);
			this->panel->Controls->SetChildIndex(this->lbTotal, 0);
			this->panel->Controls->SetChildIndex(this->lv, 0);
			this->panel->Controls->SetChildIndex(this->tbCard, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->tbMonto, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->tbDni, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
			this->panel->Controls->SetChildIndex(this->lbReca, 0);
			this->panel->Controls->SetChildIndex(this->tbVenc, 0);
			this->panel->Controls->SetChildIndex(this->label6, 0);
			this->panel->Controls->SetChildIndex(this->tbNom, 0);
			this->panel->Controls->SetChildIndex(this->label7, 0);
			this->panel->Controls->SetChildIndex(this->tbCvv, 0);
			this->panel->Controls->SetChildIndex(this->lbCvv, 0);
			this->panel->Controls->SetChildIndex(this->lbMarca, 0);
			this->panel->Controls->SetChildIndex(this->ult4Dig, 0);
			this->panel->Controls->SetChildIndex(this->label5, 0);
			this->panel->Controls->SetChildIndex(this->tbDummy, 0);
			this->panel->Controls->SetChildIndex(this->lbCardHelp, 0);
			this->panel->Controls->SetChildIndex(this->cboPlan, 0);
			this->panel->Controls->SetChildIndex(this->btnManual, 0);
			this->panel->Controls->SetChildIndex(this->cboTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->btnTarjeta, 0);
			this->panel->Controls->SetChildIndex(this->btnAdd, 0);
			this->panel->Controls->SetChildIndex(this->label8, 0);
			this->panel->Controls->SetChildIndex(this->label9, 0);
			this->panel->Controls->SetChildIndex(this->btnDot, 0);
			this->panel->Controls->SetChildIndex(this->lbExtraCash, 0);
			this->panel->Controls->SetChildIndex(this->txtImpCashBack, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(481, 450);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(481, 397);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(537, 219);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(481, 219);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(537, 167);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(481, 167);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(537, 115);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(481, 115);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(537, 63);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(481, 63);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			// 
			// Titulo
			// 
			this->Titulo->Location = System::Drawing::Point(62, 22);
			this->Titulo->Size = System::Drawing::Size(286, 23);
			this->Titulo->Text = L"Pago con Tarjetas de Crédito";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(593, 219);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(593, 167);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(593, 115);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(593, 63);
			// 
			// lbCardHelp
			// 
			this->lbCardHelp->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lbCardHelp->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbCardHelp->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCardHelp->Location = System::Drawing::Point(20, 69);
			this->lbCardHelp->Name = L"lbCardHelp";
			this->lbCardHelp->Size = System::Drawing::Size(407, 36);
			this->lbCardHelp->TabIndex = 65;
			this->lbCardHelp->Text = L"Pase la Tarjeta por el Lector de Banda para tomar sus Datos...";
			this->lbCardHelp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbFaltan
			// 
			this->lbFaltan->BackColor = System::Drawing::Color::Transparent;
			this->lbFaltan->Font = (gcnew System::Drawing::Font(L"Arial Black", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFaltan->ForeColor = System::Drawing::Color::Crimson;
			this->lbFaltan->Location = System::Drawing::Point(446, 10);
			this->lbFaltan->Name = L"lbFaltan";
			this->lbFaltan->Size = System::Drawing::Size(187, 35);
			this->lbFaltan->TabIndex = 70;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbTotal
			// 
			this->lbTotal->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lbTotal->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbTotal->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbTotal->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(0)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->lbTotal->Location = System::Drawing::Point(481, 328);
			this->lbTotal->Name = L"lbTotal";
			this->lbTotal->Size = System::Drawing::Size(162, 64);
			this->lbTotal->TabIndex = 71;
			this->lbTotal->Text = L"Total: $150.25";
			this->lbTotal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label5->ForeColor = System::Drawing::Color::Beige;
			this->label5->Location = System::Drawing::Point(34, 201);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(195, 14);
			this->label5->TabIndex = 96;
			this->label5->Text = L"Ultimos 4 Digitos de la Tarjeta:";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// ult4Dig
			// 
			this->ult4Dig->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ult4Dig->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ult4Dig->Location = System::Drawing::Point(256, 199);
			this->ult4Dig->MaxLength = 4;
			this->ult4Dig->Name = L"ult4Dig";
			this->ult4Dig->Size = System::Drawing::Size(53, 22);
			this->ult4Dig->TabIndex = 95;
			this->ult4Dig->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->ult4Dig->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::ult4Dig_Validating);
			// 
			// lbMarca
			// 
			this->lbMarca->AutoSize = true;
			this->lbMarca->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbMarca->ForeColor = System::Drawing::Color::Beige;
			this->lbMarca->Location = System::Drawing::Point(302, 121);
			this->lbMarca->Name = L"lbMarca";
			this->lbMarca->Size = System::Drawing::Size(54, 14);
			this->lbMarca->TabIndex = 93;
			this->lbMarca->Text = L"Tarjeta:";
			this->lbMarca->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lbCvv
			// 
			this->lbCvv->AutoSize = true;
			this->lbCvv->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCvv->ForeColor = System::Drawing::Color::Beige;
			this->lbCvv->Location = System::Drawing::Point(54, 247);
			this->lbCvv->Name = L"lbCvv";
			this->lbCvv->Size = System::Drawing::Size(175, 14);
			this->lbCvv->TabIndex = 90;
			this->lbCvv->Text = L"Cvv (al dorso de la tarjeta):";
			this->lbCvv->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbCvv
			// 
			this->tbCvv->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCvv->Enabled = false;
			this->tbCvv->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCvv->Location = System::Drawing::Point(256, 245);
			this->tbCvv->MaxLength = 4;
			this->tbCvv->Name = L"tbCvv";
			this->tbCvv->Size = System::Drawing::Size(53, 22);
			this->tbCvv->TabIndex = 89;
			this->tbCvv->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tbCvv->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->tbCvv->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbCvv_Validating);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->ForeColor = System::Drawing::Color::Beige;
			this->label7->Location = System::Drawing::Point(58, 165);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(58, 14);
			this->label7->TabIndex = 88;
			this->label7->Text = L"Nombre:";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbNom
			// 
			this->tbNom->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbNom->Enabled = false;
			this->tbNom->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbNom->Location = System::Drawing::Point(140, 163);
			this->tbNom->Name = L"tbNom";
			this->tbNom->Size = System::Drawing::Size(287, 22);
			this->tbNom->TabIndex = 87;
			this->tbNom->Text = L"Nombre del Titular";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->ForeColor = System::Drawing::Color::Beige;
			this->label6->Location = System::Drawing::Point(30, 142);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(86, 14);
			this->label6->TabIndex = 86;
			this->label6->Text = L"Vencimiento:";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbVenc
			// 
			this->tbVenc->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbVenc->Enabled = false;
			this->tbVenc->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbVenc->Location = System::Drawing::Point(140, 140);
			this->tbVenc->Name = L"tbVenc";
			this->tbVenc->Size = System::Drawing::Size(53, 22);
			this->tbVenc->TabIndex = 85;
			this->tbVenc->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->tbVenc->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbVenc_Validating);
			// 
			// lbReca
			// 
			this->lbReca->AutoSize = true;
			this->lbReca->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbReca->ForeColor = System::Drawing::Color::Beige;
			this->lbReca->Location = System::Drawing::Point(240, 283);
			this->lbReca->Name = L"lbReca";
			this->lbReca->Size = System::Drawing::Size(107, 14);
			this->lbReca->TabIndex = 84;
			this->lbReca->Text = L"(Recargo $0.00)";
			this->lbReca->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label4->ForeColor = System::Drawing::Color::Beige;
			this->label4->Location = System::Drawing::Point(111, 224);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(118, 14);
			this->label4->TabIndex = 83;
			this->label4->Text = L"Dni o Equivalente:";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbDni
			// 
			this->tbDni->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbDni->Enabled = false;
			this->tbDni->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbDni->Location = System::Drawing::Point(256, 222);
			this->tbDni->MaxLength = 10;
			this->tbDni->Name = L"tbDni";
			this->tbDni->Size = System::Drawing::Size(100, 22);
			this->tbDni->TabIndex = 82;
			this->tbDni->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->tbDni->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbDni_Validating);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::Beige;
			this->label3->Location = System::Drawing::Point(62, 307);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(54, 14);
			this->label3->TabIndex = 81;
			this->label3->Text = L"Cuotas:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Beige;
			this->label2->Location = System::Drawing::Point(64, 282);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 14);
			this->label2->TabIndex = 79;
			this->label2->Text = L"Monto:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbMonto
			// 
			this->tbMonto->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbMonto->Enabled = false;
			this->tbMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbMonto->Location = System::Drawing::Point(140, 280);
			this->tbMonto->Name = L"tbMonto";
			this->tbMonto->Size = System::Drawing::Size(87, 22);
			this->tbMonto->TabIndex = 78;
			this->tbMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->tbMonto->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->tbMonto->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbMonto_Validating);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Beige;
			this->label1->Location = System::Drawing::Point(62, 119);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(54, 14);
			this->label1->TabIndex = 77;
			this->label1->Text = L"Tarjeta:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbCard
			// 
			this->tbCard->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCard->Enabled = false;
			this->tbCard->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCard->Location = System::Drawing::Point(140, 117);
			this->tbCard->Name = L"tbCard";
			this->tbCard->Size = System::Drawing::Size(155, 22);
			this->tbCard->TabIndex = 76;
			this->tbCard->Text = L"1234-1234-1234-1234";
			this->tbCard->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::AnyTbClick);
			this->tbCard->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbCard_Validating);
			// 
			// lv
			// 
			this->lv->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lv->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(4) {this->colMarca, this->colNumero, 
				this->colCuotas, this->colMonto});
			this->lv->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lv->FullRowSelect = true;
			this->lv->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::Nonclickable;
			this->lv->HideSelection = false;
			this->lv->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(1) {listViewItem2});
			this->lv->Location = System::Drawing::Point(19, 374);
			this->lv->MultiSelect = false;
			this->lv->Name = L"lv";
			this->lv->Size = System::Drawing::Size(406, 97);
			this->lv->TabIndex = 75;
			this->lv->UseCompatibleStateImageBehavior = false;
			this->lv->View = System::Windows::Forms::View::Details;
			// 
			// colMarca
			// 
			this->colMarca->Text = L"Tarjeta";
			this->colMarca->Width = 120;
			// 
			// colNumero
			// 
			this->colNumero->Text = L"Numero";
			this->colNumero->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->colNumero->Width = 100;
			// 
			// colCuotas
			// 
			this->colCuotas->Text = L"Cuotas";
			this->colCuotas->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->colCuotas->Width = 80;
			// 
			// colMonto
			// 
			this->colMonto->Text = L"Importe";
			this->colMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->colMonto->Width = 100;
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &TarjOnlineTipre::timerFaltan_Tick);
			// 
			// tbDummy
			// 
			this->tbDummy->Enabled = false;
			this->tbDummy->Location = System::Drawing::Point(318, 77);
			this->tbDummy->Name = L"tbDummy";
			this->tbDummy->Size = System::Drawing::Size(100, 20);
			this->tbDummy->TabIndex = 99;
			// 
			// cboPlan
			// 
			this->cboPlan->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboPlan->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboPlan->FormattingEnabled = true;
			this->cboPlan->Location = System::Drawing::Point(140, 306);
			this->cboPlan->Name = L"cboPlan";
			this->cboPlan->Size = System::Drawing::Size(132, 22);
			this->cboPlan->TabIndex = 100;
			this->cboPlan->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::cboPlan_Validating);
			this->cboPlan->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &TarjOnlineTipre::cboPlan_PreviewKeyDown);
			// 
			// btnManual
			// 
			this->btnManual->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnManual->Location = System::Drawing::Point(481, 503);
			this->btnManual->Name = L"btnManual";
			this->btnManual->Size = System::Drawing::Size(162, 32);
			this->btnManual->TabIndex = 101;
			this->btnManual->Text = L"Ing. Manual";
			this->btnManual->UseVisualStyleBackColor = true;
			this->btnManual->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::btnManual_Click);
			// 
			// cboTarjeta
			// 
			this->cboTarjeta->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTarjeta->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboTarjeta->FormattingEnabled = true;
			this->cboTarjeta->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Visa Frances", L"Naranja", L"Visa Macro", L"Visa Credito", 
				L"Mastercard", L"Maestro"});
			this->cboTarjeta->Location = System::Drawing::Point(347, 195);
			this->cboTarjeta->Name = L"cboTarjeta";
			this->cboTarjeta->Size = System::Drawing::Size(105, 24);
			this->cboTarjeta->TabIndex = 103;
			this->cboTarjeta->Visible = false;
			// 
			// btnTarjeta
			// 
			this->btnTarjeta->Location = System::Drawing::Point(377, 224);
			this->btnTarjeta->Name = L"btnTarjeta";
			this->btnTarjeta->Size = System::Drawing::Size(75, 23);
			this->btnTarjeta->TabIndex = 104;
			this->btnTarjeta->Text = L"Tarjeta";
			this->btnTarjeta->UseVisualStyleBackColor = true;
			this->btnTarjeta->Visible = false;
			this->btnTarjeta->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::btnTarjeta_Click);
			// 
			// btnAdd
			// 
			this->btnAdd->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnAdd.Image")));
			this->btnAdd->Location = System::Drawing::Point(376, 474);
			this->btnAdd->Name = L"btnAdd";
			this->btnAdd->Size = System::Drawing::Size(50, 48);
			this->btnAdd->TabIndex = 105;
			this->btnAdd->UseVisualStyleBackColor = true;
			this->btnAdd->Visible = false;
			this->btnAdd->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::btnAdd_Click);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->ForeColor = System::Drawing::Color::Beige;
			this->label8->Location = System::Drawing::Point(214, 144);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(47, 14);
			this->label8->TabIndex = 106;
			this->label8->Text = L"MMAA";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->label9->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label9->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(19, 474);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(351, 48);
			this->label9->TabIndex = 107;
			this->label9->Text = L"Cuando termine de ingresar los datos de la tarjeta puse la tecla \'+\' para Agregar" 
				L".\'";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnDot
			// 
			this->btnDot->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnDot.Image")));
			this->btnDot->Location = System::Drawing::Point(593, 273);
			this->btnDot->Name = L"btnDot";
			this->btnDot->Size = System::Drawing::Size(50, 48);
			this->btnDot->TabIndex = 108;
			this->btnDot->UseVisualStyleBackColor = true;
			this->btnDot->Visible = false;
			this->btnDot->Click += gcnew System::EventHandler(this, &TarjOnlineTipre::btnDot_Click);
			// 
			// lbExtraCash
			// 
			this->lbExtraCash->AutoSize = true;
			this->lbExtraCash->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbExtraCash->ForeColor = System::Drawing::Color::Beige;
			this->lbExtraCash->Location = System::Drawing::Point(60, 336);
			this->lbExtraCash->Name = L"lbExtraCash";
			this->lbExtraCash->Size = System::Drawing::Size(74, 14);
			this->lbExtraCash->TabIndex = 109;
			this->lbExtraCash->Text = L"Extracción:";
			this->lbExtraCash->UseWaitCursor = true;
			// 
			// txtImpCashBack
			// 
			this->txtImpCashBack->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->txtImpCashBack->Location = System::Drawing::Point(140, 335);
			this->txtImpCashBack->Name = L"txtImpCashBack";
			this->txtImpCashBack->Size = System::Drawing::Size(87, 20);
			this->txtImpCashBack->TabIndex = 110;
			this->txtImpCashBack->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTipre::tbCashBack_Validating);
			// 
			// TarjOnlineTipre
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(659, 550);
			this->Name = L"TarjOnlineTipre";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"TarjOnlineTipre";
			this->TopMost = false;
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private:	
		
		System::Void AnyTbClick(System::Object^  sender, System::EventArgs^  e) 
		{
			if(pantalla != 1 && pantalla != 3) 
				SetFocusTo((Control ^)sender);
		}

		void GenValidacion()
		{
			bool res = false;
			if (this->EsCompra)
				res = actualTipre != nullptr && actualTipre->DatosValidos() == "OK";
			
			timerFaltan->Enabled = !res && actualTipre->Nro_Tarjeta != "";
			if(actualTipre != nullptr && actualTipre->Nro_Tarjeta != "")
			{
				lbFaltan->Visible = true;
				lbFaltan->Text = (!res? "Faltan Datos...": "Tarjeta Lista...");
				lbFaltan->ForeColor = (res? Color::Red: Color::Beige);
				lbFaltan->BackColor = (res? Color::White: Color::Blue);
				lbFaltan->BorderStyle = (res? System::Windows::Forms::BorderStyle::FixedSingle: System::Windows::Forms::BorderStyle::None);
			}
			else
				lbFaltan->Visible = false;

			btOk->Enabled = Dump::TarjTipre->Count > 0;
		}

		void SincroTotal()
		{
			char tmp[50];
			sprintf(tmp, "%.2lf", pend);
			this->lbTotal->Text = "Falta: $" + Strings::CharToString(tmp);
		}

		// Sincroniza la lista en pantalla con los datos en nuestra lista de tarjetas online.
		void SincroLista()
		{
			lv->SuspendLayout();
			lv->Items->Clear();
			Decimal asub = 0;
			for(int i = 0; i < Dump::TarjTipre->Count; i++)
			{
				TipreRequest ^unatar = Dump::TarjTipre[i];
				
				ListViewItem ^lvi = lv->Items->Add(unatar->Marca());
				lvi->SubItems->Add(unatar->TarjetaEnmascarada());
				lvi->SubItems->Add(unatar->PlanDePago);
				lvi->SubItems->Add(unatar->Importe.ToString("F2"));
				lvi->BackColor = unatar->AlbEtapa == 4 && unatar->CodigoRespuesta == 0 ? System::Drawing::Color::LightGreen : System::Drawing::Color::White;
				asub += unatar->Importe_Ingresado;			
			}

			lv->ResumeLayout();
			this->pend -= Convert::ToDouble(asub);
		}		

		System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			if (Char::IsLetter(e->KeyChar))			
				e->Handled = true;			
			else if (Char::IsDigit(e->KeyChar))			
				e->Handled = false;			
			else if (Char::IsControl(e->KeyChar))			
				e->Handled = false;			
			else			
				e->Handled = true;			
		}

		System::Void txtSoloDocumento_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			if (Char::IsLetter(e->KeyChar))			
				e->Handled = true;			
			else if (Char::IsDigit(e->KeyChar) || Char::IsPunctuation(e->KeyChar))			
				e->Handled = false;			
			else if (Char::IsControl(e->KeyChar))			
				e->Handled = false;			
			else			
				e->Handled = true;			
		}

		System::Void txtSoloMonto_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
		{
			if (Char::IsLetter(e->KeyChar))			
				e->Handled = true;			
			else if (Char::IsDigit(e->KeyChar) )			
				e->Handled = false;			
			else if (e->KeyChar == '.')
			{
				TextBox^ auxtext = dynamic_cast<TextBox ^>(sender);
				e->Handled = auxtext->Text->Contains(".");
			}
			else if (Char::IsControl(e->KeyChar))			
				e->Handled = false;			
			else			
				e->Handled = true;			
		}

		
	protected: virtual void OnLoad(EventArgs ^e) override
	   {
		   //lv_SelectedIndexChanged(nullptr, nullptr);
		   BaseDialog::OnLoad(e);
	   }


	
		 private: void LimpiarControles()
		 {
			 this->tbCard->Text = "";
			 this->tbVenc->Text = "";
			 this->tbNom->Text = "";
			 this->tbMonto->Text = "";
			 this->cboPlan->Items->Clear();
			 this->tbDni->Text = "";
			 this->tbCvv->Text = "";

			 this->lbMarca->Text = "";
			 this->lbReca->Text = "";
			 this->ult4Dig->Text = "";

			 this->tbCard->Enabled = IngManual; // false;
			 this->tbVenc->Enabled = IngManual; // false;
			 this->tbNom->Enabled = false;
			 this->tbMonto->Enabled = IngManual; // false;
			 this->cboPlan->Enabled = IngManual; // false;
			 this->tbDni->Enabled = IngManual; // false;

			 this->tbCvv->Enabled = IngManual; // false;
			 this->tbCvv->Visible = this->tbCvv->Enabled;
			 this->lbCvv->Visible = this->tbCvv->Enabled;

			 this->txtImpCashBack->Enabled = actualTipre != nullptr && System::Configuration::ConfigurationManager::AppSettings["tiprecashback"] == "si" && actualTipre->PermiteCashBack;
			 this->txtImpCashBack->Visible = this->txtImpCashBack->Enabled;
			 this->lbExtraCash->Visible = this->txtImpCashBack->Enabled;
		 }

		 private: void ActualizarDatosTarjeta()
		 {
			 this->tbCard->Text = actualTipre->Nro_Tarjeta;
			 this->tbVenc->Text = actualTipre->VencimFormateado();
			 this->tbNom->Text = actualTipre->NombreCliente;
			 this->tbMonto->Text = actualTipre->Importe.ToString("F2");
			 LlenaPlan();
			 this->tbDni->Text = actualTipre->DniFormateado();
			 this->tbCvv->Text = actualTipre->CodigoSeguridad;
			 this->ult4Dig->Text = actualTipre->Ult4DigitosTarjeta;

			 this->lbMarca->Text = "(" + actualTipre->Marca() + ")";
			 this->lbReca->Text = "(Recargo: $" + actualTipre->ImporteRecargo.ToString("F2") + ")";
			 this->tbCard->Enabled = IngManual && tbCard->TextLength == 0;
			 this->tbVenc->Enabled = IngManual && tbVenc->TextLength == 0;
			 this->tbNom->Enabled = false;
			 this->tbMonto->Enabled = true;
			 this->cboPlan->Enabled = true;
			 this->tbDni->Enabled = true;

			 this->tbCvv->Enabled = actualTipre->PideCvv();
			 this->tbCvv->Visible = this->tbCvv->Enabled;
			 this->lbCvv->Visible = this->tbCvv->Enabled;

			 this->txtImpCashBack->Enabled = System::Configuration::ConfigurationManager::AppSettings["tiprecashback"] == "si" && actualTipre->PermiteCashBack;
			 this->txtImpCashBack->Visible = this->txtImpCashBack->Enabled;
			 this->lbExtraCash->Visible = this->txtImpCashBack->Enabled;
		 }


	private: System::Void LlenaPlan()
			 {
				 cboPlan->Items->Clear();
				 for (int i = 0; i < actualTipre->Planes->Count; i++)
				 {
					 cboPlan->Items->Add(actualTipre->Planes[i]);
				 }
				 if (actualTipre->Planes->Count > 0)
					cboPlan->SelectedIndex = 0;
			 }

	private: 
		System::Void TarjOnlineForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
		{
			Strings::releaseKeyboard(this);
		}

		// Procesa un numero de tarjeta.
		void DecodeCard()
		{
			try
			{
				if (IngManual)
				{
					//actualTipre->Nro_Tarjeta = tbCard->Text;
					actualTipre->Modo_Ingreso = "MANUAL";
					actualTipre->Id = actualTipre->Nro_Tarjeta; //tbCard->Text;
				}
				// Valida el numero de tarjeta.
				if(actualTipre->Nro_Tarjeta->Length != 16)
					actualTipre->Nro_Tarjeta = "";

				// Busca el producto en los rangos disponibles.
				if(actualTipre->Nro_Tarjeta != "")
				{
					if (this->EsCompra)
					{	
						long fnroticket = pf->GetTickNro(GlobalTF);
						if (globalDelayPrint)
							fnroticket++;
						actualTipre->NroTicket = Convert::ToString(fnroticket);
						actualTipre->IdentificarCompra(caja);
						//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","Vuelve de DLL TipreRequest");
						if (actualTipre->CodigoRespuesta != 0)
						{
							//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","Algun error/respuesta de Identificar");
							if (actualTipre->AlbEtapa == 7)
								Alert("Error en la comunicación", "con el servidor de tarjetas");
							else
							{
								char *msgrta = new char[50];
								Strings::StringToChar(actualTipre->MensajeRespuesta, msgrta, 50);
								Alert("NO SE ACEPTA la tarjeta",
										msgrta);
								delete msgrta;
							}
							actualTipre->Nro_Tarjeta = "";
							actualTipre->Clear();
						}
						else
						{
							//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","comienza Encontrar Equiv Tarjeta");
							//se busca el mpago y Numero de Tarjeta de Alberdi
							char ntarjet[30];							
							Strings::StringToChar(actualTipre->NombreEmisor, ntarjet, 30);						
							TarjDef *nrot = BuscarTipre(ntarjet);
							//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","termina de encontrar equiv tarjeta");
							if (nrot != NULL)// se encontro tarjeta en las tablas del sistema
							{						
								//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","encontro una equiv tarjeta");
								actualTipre->AlbNroTarjeta = nrot->Codigo; // nrot;
								actualTipre->AlbNroMpago = nrot->MPago; // def->MPago;
								actualTipre->AlbLongAuto = nrot->longaut; // def->longaut;
								actualTipre->AlbNumVoucher = nrot->Voucher;
								actualTipre->AlbPorcDesctoCom = nrot->porDesctoCom;
								actualTipre->Importe = Convert::ToDecimal(this->pend);
								if (actualTipre ->PermiteCashBack)
								{								
									mpag_ *lcmpag = BuscarMpag(nrot->MPago);
									actualTipre->MinCashBack = System::Convert::ToDecimal(lcmpag->minextracc);
									actualTipre->MaxCashBack = System::Convert::ToDecimal(lcmpag->maxextracc);
								}

								Dump::promosmp->CalculoDescto(nrot->MPago, nrot->Codigo, this->pend);								
								actPromoMp = Dump::promosmp->BuscarConTarjeta(nrot->MPago, nrot->Codigo);
							
								if (IngManual)
									tbCard->Enabled = false;
							}				
							else
							{
								//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","No encontro equiv de tarjeta");
								Alert("La tarjeta ingresada NO SE RECONOCE como una tarjeta",
									"que se acepte OnLine. Consulte con su Supervisor");
								actualTipre->Nro_Tarjeta = "";
							}
							nrot = NULL;
							delete nrot;
						}
					}
				}

				//// Actualiza la form
				//if(actualTipre->Nro_Tarjeta == "")
				//{
				//	//actualTarj->Definicion = NULL;
				//	//actualTarj->Nombre[0] = 0;
				//}		
				
				//lv_SelectedIndexChanged(nullptr, nullptr);
				//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","Comienza GenValidacion");
				GenValidacion();
				//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","terminara DecodeCard");
			}catch(System::Exception ^excep)
			{
				ClrLog::escribeExcepcion("errormain.txt", "DecodeCard", excep);
			}
		}

		// NOA
		// %B6280480140177496^VILLAFA&ES RICARDO FEDERICO^091010301111111?;6280480140177496=09101017300000000008?
		// VISA INTERNACIONAL
		// %B4357609421893172^VILLAFANE/RICARDO FEDERICO^11101018514100323000000?;4357609421893172=111010185141323?
		// CABAL 24
		// %B6042015252251801^VILLAFA&E RICARDO FEDERI  ^17111212210000000000?;6042015252251801=17111212210000000000?
		// VISA DEBITO
		// %B4517650551447683^VILLAFA&ES/RICARDO FEDER ^120912110000        00118000000?;4517650551447683=12091211180000000000?
		// %B4517650551447683^VILLAFA&ES/RICARDO FEDER ^120912110000        00118000000?;4517650551447683=12091211180000000000?
		// %B4517650552419657^VILLAFA&ES/RICARDO FEDER^121112110000        00251000000?;4517650552419657=12111212510000000000?
		// TOMATO
		// %B5776370000131720^                          ^1011120?;5776370000131720=1011120010000?

		public: virtual void StartCardReader() override
		{
			//blanquea los datos de la tarjeta actual, no deberia estar autorizada
			actualTipre->Clear();
			this->tbDummy->Text = "";
			this->tbDummy->Enabled = true;
			this->tbDummy->Focus();
			BaseDialog::StartCardReader();
		}

		public: virtual bool CardReaderDigit(wchar_t ch) override
		{
			bool res = false;
			try
			{
				res = BaseDialog::CardReaderDigit(ch);
				this->lbCardHelp->BackColor = 
					(this->lbCardHelp->BackColor == System::Drawing::Color::Lime)?
						Color::FromArgb(255, 255, 128):
						System::Drawing::Color::Lime;
					 //System::Windows::Forms::Application::DoEvents();
			}
			catch (Exception ^excep)
			{
				WriteBootLog("ERROR - AL LEER LA BANDA MAGNETICA DE LA TARJETA");
			}
			return res;
		}

		public: virtual void EndCardReader() override
		{
			try
			{
				BaseDialog::EndCardReader();			
				//Strings::DisableKey(this);	
				if (WebServiceTrabajando)
					return;
				WebServiceTrabajando = true;
				if (Dump::TarjTipre->Count < CantTotalTarjetas)
				{			
					this->lbCardHelp->BackColor = Color::FromArgb(255, 255, 128);
	
					if(cardAdicData->Length > 0 && cardNumber->Length > 0 && cardVenc > 0)
					{
						actualTipre->Id = this->cardData->ToString();
						tbCard->Text = this->cardNumber;
						actualTipre->Nro_Tarjeta = cardNumber;
						actualTipre->NombreCliente = cardHolder->Replace('&', 'N');					
						this->tbVenc->Text = cardVenc.ToString();
						if (this->tbVenc->TextLength == 3)
							this->tbVenc->Text = "0" + cardVenc.ToString();
						globalTouchEvent = true;
						tbVenc_Validating(nullptr, nullptr);
						globalTouchEvent = false;
						if (!tarjVencida)
						{	
							SendMsg("Leyendo Datos de Tarjeta...", "Espere por favor...");
							DecodeCard();
							ClearMsg();
							ActualizarDatosTarjeta();
							//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","Termina de ActualizarDatosTarjeta");
						}				
					}
					else
						Alert("Tarjeta mal leida o Defectuosa.", "Pruebe de Nuevo...");
							 
					this->tbDummy->Enabled = false;
					SetFocusTo(this->ult4Dig);
					
				}
				else
				{
					Alert("Aviso - El sistema no permite ingresar mas", "Tarjetas para autorizar");
				}
				WebServiceTrabajando = false;
				//Strings::EnableKey(this);
				//ArTarjetas::escribeLog("TarjOnlineTipre.h","DecodeCard","sale de EndCardReader");
			}
			catch (System::Exception ^excep)
			{	
				if (WebServiceTrabajando)
				{
					//Strings::EnableKey(this);
					WebServiceTrabajando = false;
				}
				ClrLog::escribeExcepcion("errormain.txt", "EndCardReaderTipre", excep);
				Alert("Error al Leer la Banda", "Reintentar");
				actualTipre->Clear();
			}
		}

		public: virtual bool ProcessHotKey(Keys key) override
		{
			if(key == Keys::Right && Dump::TarjTipre->Count > 0)
			{
				lv->SuspendLayout();
				int selIndex = (lv->SelectedIndices->Count>0)? lv->SelectedIndices[0] + 1: 0;
				lv->SelectedItems->Clear();
				if(selIndex >= lv->Items->Count) selIndex = 0;
				lv->Items[selIndex]->Selected = true;
				lv->ResumeLayout();
				return true;
			}
			else if (key == Keys::Add && (actualTipre->DatosValidos() == "OK"))
			{
				btnAdd_Click(this, gcnew System::EventArgs());
			}
			else if (key == Keys::Down)
			{
				if (this->tb->Name == "cboPlan")
				{
					if (pantalla != 2)//la pantalla es Touch Screen
						Strings::DisableKey(this);
					cboPlan->DroppedDown = true;
					return true;						 
				}					 
			}
			return BaseDialog::ProcessHotKey(key);
		}

		// *********************************************************************************************
		// ** Validacion y formateo de los textboxes

private: 
		System::Void tbCard_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			//char tbuff[30];
			if (Dump::TarjTipre->Count > CantTotalTarjetas)	
			{
				Alert("Aviso - El sistema no permite ingresar mas", "Tarjetas para autorizar");
				e->Cancel = true;
				return;
			}

			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				String ^tbText = tbCard->Text->Trim();
				if(tbText->Length == 0) return;
				if(tbText->Length > 22) 
					tbText = tbText->Substring(0, 22);
				//Strings::StringToChar(tbText, tbuff, 28);

				// Copia solo los digitos numericos
				StringBuilder ^pd = gcnew StringBuilder();
				//for (int i = 0; i < actualTipre->Nro_Tarjeta->Length; i++)
				for (int i = 0; i < tbText->Length; i++)
				{
					if (Char::IsDigit(tbText[i])) // >= '0' && tbText[i] <= '9')
						pd->Append(tbText[i]);
				}
				actualTipre->Nro_Tarjeta = pd->ToString();
			}

			// Valida el numero de tarjeta.
			if (this->tbCard->Enabled)
			{
				DecodeCard();
				ActualizarDatosTarjeta();
			}
		}		 

		System::Void tbVenc_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			//char tbuff[30];

			if(tbCard->Text->Trim()->Length != 16 || tbVenc->TextLength != 4) 
				return;
			tarjVencida = true;
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				String ^tbText = tbVenc->Text->Trim();
				if(tbText->Length > 10)
					tbText = tbText->Substring(0, 10);
				actualTipre->FechaVto = "";
				if (IngManual && tbText->Length == 0)
				{
					Alert("Fecha de Vencimiento Invalida", "Trate de Nuevo...");					
					this->SetFocusTo(tbVenc);
					return;
				}
				if(validatingVenc) return;
				validatingVenc = true;

				// Obtiene el año y mes minimos.
				int minFecha = date() / 100;
				int minMes = minFecha % 100;
				int minAio = (minFecha / 100) - 2000;

				int actVenc = System::Convert::ToUInt32(tbText);
				// Convierte el mes en decimal.
				//actualTarj->Vencim = 0;
				
				// Formatea como corresponde.
				int mes = actVenc / 100, aio = actVenc % 100;
				if(mes == 0 && aio == 0)				 
					actualTipre->FechaVto = "";				
				else if((mes < 1 || mes > 12) || (aio < 8 && aio > 20))				
					Alert("Fecha de Vencimiento Invalida", "Trate de Nuevo...");									
				else if(aio < minAio || (aio == minAio && mes < minMes))				
					Alert("La tarjeta esta Vencida", "Verifique los datos por favor...");					
				
				else
				{
					actualTipre->FechaVto = String::Format("{0:D2}{1:D2}", mes, aio);
					tbVenc->Enabled = false;
					tarjVencida = false;
				}
				tbVenc->Text = String::Format("{0:D2}{1:D2}", mes, aio);
				validatingVenc = false;
				GenValidacion();
			}
		}

		System::Void tbMonto_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if(tbCard->Text->Trim()->Length == 0) 
				return;
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				/*String ^tbText = this->tbMonto->Text->Trim();
				Decimal monto = 0;
				Decimal::TryParse(tbText, monto);

				TiprePlan ^auxplan = dynamic_cast<TiprePlan ^>(cboPlan->SelectedItem);

				// El monto ingresado solo se acepta si es mayor que cero y, restada su porcion que
				// es recargo, no supera entonces el monto pendiente.
				if(monto < 0) 
					monto = 0;
				actualTipre->ImporteRecargo = 0;
				Decimal tasa = auxplan->Recargo / 100;
				Decimal tDescto = auxplan->Descuento / 100;				

				Decimal recargo = 0;
				Decimal descto = 0;
				if(tasa != 0)				
					recargo = Decimal::Round((monto * tasa), 2);
				if (tDescto != 0)
					descto = Decimal::Round((monto * tDescto), 2);

				Decimal tCajaDescto = 0;
				Decimal cajaDescto = 0;
				if(actPromoMp != nullptr)
				{
					tCajaDescto = ((Decimal)actualTipre->AlbPorcDesctoCom) / 100;
					actPromoMp->ImporteIngresado = 0;
					//if (tCajaDescto != 0)
					//	cajaDescto = Decimal::Round((monto * tCajaDescto), 2);
					if (actPromoMp->Monto > 0)
					{					
						if (monto == actPromoMp->ImporteSinDescto)
							cajaDescto = actPromoMp->Monto;
						else if (actPromoMp->ImporteSinDescto > monto)
							cajaDescto = Decimal::Round((monto * tCajaDescto), 2);
						else if (actPromoMp->ImporteSinDescto < monto)
							cajaDescto = actPromoMp->Monto;
						actPromoMp->ImporteIngresado = monto;
					}
					else
						cajaDescto = 0;
				}
				else
					cajaDescto = 0;		
				actualTipre->AlbMontoDescto = cajaDescto;

				// Si el monto en tarjeta es mayor, entonces recalcula usando solamente el pendiente.
				//if(monto + recargo > Decimal(pend))
				if(monto > Decimal(pend))
				{
					recargo = Decimal::Round(Decimal(pend) * tasa, 2);
					monto = Decimal(pend); // + recargo;
					descto = Decimal::Round(Decimal(pend) * tDescto, 2);
				}
				actualTipre->Importe_Ingresado = monto;
				actualTipre->Importe = monto + recargo - descto - cajaDescto;
				actualTipre->ImporteDescuento = descto;
				actualTipre->ImporteRecargo = recargo;
				if (actualTipre->ImporteRecargo > 0)
					this->lbReca->Text = "(Recargo: $" + actualTipre->ImporteRecargo.ToString("F2") + ")";				
				else
					this->lbReca->Text = "";
				if (actualTipre->ImporteDescuento > 0)
					this->lbReca->Text += "(Descuento: $" + actualTipre->ImporteDescuento.ToString("F2") + ")";

				if (cajaDescto > 0)
					this->lbReca->Text += "(Descuento: $" + cajaDescto.ToString("F2") + ")";*/
				CalculosImporte();
				GenValidacion();
			}
		}

		System::Void CalculosImporte()
		{
			String ^tbText = this->tbMonto->Text->Trim();
			Decimal monto = 0;
			Decimal::TryParse(tbText, monto);

			if(monto < 0) 
				monto = 0;		

			TiprePlan ^auxplan = dynamic_cast<TiprePlan ^>(cboPlan->SelectedItem);
			actualTipre->PlanDePago = auxplan->Nombre;
			actualTipre->AlbNumCuota = auxplan->Numcuota;					

			//Decimal uMonto = monto; // actualTipre->Importe_Ingresado;
			actualTipre->ImporteRecargo = 0;			

			Decimal rec = Decimal(auxplan->Recargo / 100 * monto);
			Decimal descto = Decimal(auxplan->Descuento / 100 * monto);

			if(monto > Decimal(pend))
			{
				rec = Decimal::Round(Decimal(pend) * auxplan->Recargo / 100, 2);
				monto = Decimal(pend); // + recargo;
				descto = Decimal::Round(Decimal(pend) * auxplan->Descuento / 100, 2);
			}

			actualTipre->ImporteRecargo = Decimal::Round(rec, 2);
			actualTipre->ImporteDescuento = Decimal::Round(descto, 2);			

			//Decimal tCajaDescto = 0;
			Decimal cajaDescto = 0;
			//if(actPromoMp != nullptr && actualTarj->tasaDescto > 0)
			if(actPromoMp != nullptr)
			{
				cajaDescto = 0;						
				for(int ki = 0; ki < actPromoMp->Count; ki++)
				{
					PromoMedioPago ^miact = actPromoMp[ki];				
					if (miact->Monto > 0 && !miact->Confirmado)// tope 2 veces el descuento
					//if (actPromoMp->Monto > 0 && !actPromoMp->Confirmado && monto >= (actPromoMp->Monto * 2))// tope 2 veces el descuento
					{
						cajaDescto += miact->Monto;							
						//actPromoMp->ImporteIngresado = monto;
					}
					/*else
						cajaDescto = 0;*/
				}
			}
			else
				cajaDescto = 0;	
			cajaDescto = System::Decimal::Round(cajaDescto,2);
			actualTipre->AlbMontoDescto = cajaDescto;


			actualTipre->Importe = monto + actualTipre->ImporteRecargo - actualTipre->ImporteDescuento - cajaDescto;
			actualTipre->Importe_Ingresado = monto;
			if (actualTipre->ImporteRecargo > 0)
				this->lbReca->Text = "(Recargo: $" + actualTipre->ImporteRecargo.ToString("F2") + ")";	
			else
				this->lbReca->Text = "";
			if (actualTipre->ImporteDescuento > 0)
				this->lbReca->Text += "(Descuento: $" + actualTipre->ImporteDescuento.ToString("F2") + ")";
			if (cajaDescto > 0)
				this->lbReca->Text += "(Descuento: $" + cajaDescto.ToString("F2") + ")";
			
		}

		System::Void tbCashBack_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if(tbCard->Text->Trim()->Length == 0) 
				return;
			/*if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{*/
				if (String::IsNullOrEmpty(txtImpCashBack->Text))
					return;
				String ^tbText = this->txtImpCashBack->Text->Trim();
				Decimal monto = 0;
				Decimal::TryParse(tbText, monto);
				
				if(monto < 0) 
					monto = 0;
				if (monto >= actualTipre->MinCashBack && monto <= actualTipre->MaxCashBack)
				{
					actualTipre->ImporteCashBack = monto;
					ValidoExtraCash = true;
				}
				else
				{
					Alert("Monto de Extraccion no permitido", "Volver a ingresar monto o dejar en blanco");
					actualTipre->ImporteCashBack = 0;
					txtImpCashBack->Text = "";
					return;
				}				
				GenValidacion();
			//}
		}

		System::Void cboPlan_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if(tbCard->Text->Trim()->Length == 0) 
				return;
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			{
				/*TiprePlan ^auxplan = dynamic_cast<TiprePlan ^>(cboPlan->SelectedItem);
				actualTipre->PlanDePago = auxplan->Nombre;
				actualTipre->AlbNumCuota = auxplan->Numcuota;					

				Decimal uMonto = actualTipre->Importe_Ingresado;// - actualTipre->ImporteRecargo;
				actualTipre->ImporteRecargo = 0;
								
				Decimal rec = Decimal(auxplan->Recargo / 100 * uMonto);
				Decimal descto = Decimal(auxplan->Descuento / 100 * uMonto);

				actualTipre->ImporteRecargo = Decimal::Round(rec, 2);
				actualTipre->ImporteDescuento = Decimal::Round(descto, 2);
				actualTipre->Importe = uMonto + actualTipre->ImporteRecargo - actualTipre->ImporteDescuento;
				if (actualTipre->ImporteRecargo > 0)
					this->lbReca->Text = "(Recargo: $" + actualTipre->ImporteRecargo.ToString("F2") + ")";	
				else
					this->lbReca->Text = "";
				if (actualTipre->ImporteDescuento > 0)
					this->lbReca->Text += "(Descuento: $" + actualTipre->ImporteDescuento.ToString("F2") + ")";*/
				CalculosImporte();

				GenValidacion();
			}
		 }

		System::Void tbDni_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			//char temp[50];

			if(tbCard->Text->Trim()->Length == 0) 
				return;
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				String ^tbText = this->tbDni->Text->Trim();
				actualTipre->Documento = 0;
				StringBuilder ^cadDni = gcnew StringBuilder();
				for(int k = 0; k < tbText->Length; k++)
					if (Char::IsDigit(tbText[k]))
						cadDni->Append(tbText[k]);
				if (cadDni->ToString()->Length > 0)
					actualTipre->Documento = System::Convert::ToUInt32(cadDni->ToString());

				if(actualTipre->Documento <= 99999 || actualTipre->Documento > 99999999)
				{
					actualTipre->Documento = 0;
					Alert("Numero de Documento Invalido", "Pruebe de nuevo...");
				}

				this->tbDni->Text = actualTipre->DniFormateado();
				GenValidacion();
			}
		}

		System::Void tbCvv_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if(tbCard->Text->Trim()->Length == 0) 
				return;
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				String ^tbText = this->tbCvv->Text->Trim();
				actualTipre->CodigoSeguridad = "0";
				int auxCvv = 0;
				Int32::TryParse(tbText, auxCvv);				

				if(!actualTipre->PideCvv() && (auxCvv <= 0 || auxCvv > 9999))
				{
					actualTipre->CodigoSeguridad = "0";
					actualTipre->CodigoSeguridadLen = 1;
					Alert("El CVV ingresado no es valido (Debe ser entre 1 y 9999)", "Pruebe de Nuevo...");
				}
				else 
				{
					actualTipre->CodigoSeguridad = auxCvv.ToString();
					actualTipre->CodigoSeguridadLen = tbText->Length; //actualTipre->CodigoSeguridad->Length;
				}

				this->tbCvv->Text = actualTipre->CodigoSeguridad;
				GenValidacion();
			}
		}

		System::Void ult4Dig_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent || IngManual)
			{
				if(tbCard->Text->Trim()->Length == 0) 
					return;
				String ^tbText = this->ult4Dig->Text->Trim();
				//actualTarj->ult4Digitos = 0;
				int auxUltDig = 0;
				Int32::TryParse(tbText, auxUltDig);
				
				if(actualTipre->LastDigits() != auxUltDig)
					Alert("Los ultimos 4 digitos NO COINCIDEN. Revise con cuidado", "la tarjeta y pruebe de nuevo...");
				GenValidacion();
			}
		}

	protected: 
		virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			/*if((!IngManual && cardData == nullptr) || actualTipre == nullptr) */
			if (Dump::TarjTipre->Count == 0)
				return;
			//Strings::StringToChar(cardData->ToString(), actualTarj->CrudeCardData, 127);
			//Strings::StringToChar(track_1, actualTarj->Track_1, 80);
			if (this->EsCompra)
				bEnter_Click(sender, e);
			BaseDialog::btOk_Click(sender, e);
		}

		virtual void bEnter_Click(Object ^sender, EventArgs ^e) override
		{
			if(pantalla != 1 || pantalla == 3)
			{
				globalTouchEvent = true;
				MethodInfo ^m = tb->GetType()->GetMethod("OnValidating",
					BindingFlags::NonPublic | BindingFlags::Public | BindingFlags::Instance);
				if(m != nullptr)
				{
					cli::array<Object ^> ^args = gcnew cli::array<Object ^> { gcnew CancelEventArgs() };
					m->Invoke(tb, args);
				}
				globalTouchEvent = false;
			}
			BaseDialog::bEnter_Click(sender, e);
		}

		private: System::Void timerFaltan_Tick(System::Object^  sender, System::EventArgs^  e) 
				 {
					 lbFaltan->Visible = !lbFaltan->Visible;
				 }

	public:

		void DoSupervCode()
		{
			if(actualTipre != nullptr)
			{
				//actualTarj->UsoLector = false;
				IngManual = true;
				if (EsCompra)
					actualTipre->Clear();
				LimpiarControles();
				//lv_SelectedIndexChanged(nullptr, nullptr);
				this->SetFocusTo(tbCard);
			}
		}

		private: System::Void btnTarj2_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 //String ^cadena = "%B4517650566121018^RAMIREZ/JOSE ALFREDO     ^150712110000        00496000000?;4517650566121018=15071214960000000000?";
			 //String ^cadena = "%B5043630566121018^RAMIREZ/JOSE ALFREDO     ^150912110000        00496000000?;5043630566121018=15091214960000000000?";
			 String ^cadena = "%B4338310593291529^RAMIREZ/JOSE ALFREDO     ^180412110000        00800000000?;4338310593291529=18041218000000000000?";
			 //String ^cadena = "%B5043630003033923^RAMIREZ/JOSE ALFREDO     ^140210118911        00992000000?;5043630003033923=14021019920000000000?";				
			 //String ^cadena = "%B5254220053144024&HERRERA ARIEL HECTOR      &1206101071749651307803544_ñ5254220053144024¡12061010717496513078_";
			 this->cardData = gcnew System::Text::StringBuilder();
			 this->cardData->Append(cadena);
			 this->EndCardReader();
		 }

	private: System::Void btnManual_Click(System::Object^  sender, System::EventArgs^  e) {
				 if (Dump::TarjTipre->Count < CantTotalTarjetas)				 	
					DoSupervCode();
				 else
					 Alert("Aviso - El sistema no permite ingresar mas", "Tarjetas para autorizar");
			 }

		 /*private: bool ValidaTarjetas()
		 {
			 bool aux = true;
			 for (int i = 0; i < Dump::TarjTipre->Count; i++)
			 {
				 TipreRequest ^unatar = Dump::TarjTipre[i];
				 aux = unatar->DatosValidos() == "OK";
				 if (!aux)
					 break;
			 }
			 return aux;
		 }*/

private: System::Void btnTarjeta_Click(System::Object^  sender, System::EventArgs^  e) {
			 //para selecciona una tarjeta
			 String ^bin = "";
			 switch (cboTarjeta->SelectedIndex)
			 {
				 case 0:
					 bin = "4338310593291527";
					 break;
				 case 1:
					 bin = "5895620593291529";//mal
					 break;
				 case 2:
					 bin = "4110100593291526";
					 break;
				 case 3:					 
					 //bin = "4029180593291528";
					 bin = "4029181308308011";
					 break;
				 case 4:
					 bin = "5155900593291522";
					 break;
				 case 5:
					 bin = "6184300593291529";//mal
					 break;
				 default:
					 bin = "433831";
					 cboTarjeta->SelectedIndex = 0;
					 break;
			 }
			 //String ^cadena = "%B" + bin + "0593291529^RAMIREZ/JOSE ALFREDO     ^180412110000        00800000000?;" + bin + "0593291529=18041218000000000000?";
			 //////////String ^cadena = "%B" + bin + "^RAMIREZ/JOSE ALFREDO     ^180412110000        00800000000?;" + bin + "=18041218000000000000?";
			 //String ^cadena = "%B4029181308308011^JUAN C. FIGUEROA     ^160310119744        00021000000?;4029181308308011=16031010210000000000?";
			 //String ^cadena = "%B4517650626056543&CUEVAS-VICTOR RAMON      &240222110000        00680000000_ñ4517650626056543¡24022216800000000000_";
			 //debito
			 String ^cadena = "%B4517650626056451^RAMIREZ/JOSE ALFREDO     ^240222110000        00384000000?;4517650626056451=24022213840000000000?";
			 //String ^cadena = "%B5226844229497472&CASTRO ERIKA DEL C        &2509101130421086102811437_ñ5226844229497472¡25091011304210861028_";
			 //credito
			 //String ^cadena = "%B4540750059180678&RAMIREZ-JOSE A       &251020199999        00512000000_ñ4540750059180678¿25102015120000000000_";
			 this->cardData = gcnew System::Text::StringBuilder();
			 this->cardData->Append(cadena);
			 this->EndCardReader();
		 }
private: System::Void cboPlan_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 if (e->KeyCode == Keys::Enter)
			 {
				 cboPlan->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
				 cboPlan_Validating(this, gcnew CancelEventArgs());
			 }
		 }
private: System::Void btnAdd_Click(System::Object^  sender, System::EventArgs^  e) {
			 //if ((this->EsCompra || Dump::TarjTipre->Count == 0) && (actualTipre->DatosValidos() == "OK"))
			 ValidoExtraCash = true;
			 if (!String::IsNullOrEmpty(txtImpCashBack->Text))
			 {
				 ValidoExtraCash = false;
				 tbCashBack_Validating(this, nullptr);
			 }
			 if (this->EsCompra  && actualTipre->DatosValidos() == "OK" && ValidoExtraCash)
				{
					Dump::TarjTipre->Add(actualTipre);					
					itemActual = gcnew ListViewItem();
					itemActual->Text = actualTipre->Marca();
					itemActual->SubItems->Add(actualTipre->TarjetaEnmascarada());
					itemActual->SubItems->Add(actualTipre->PlanDePago);
					itemActual->SubItems->Add(actualTipre->Importe.ToString("F2"));
					lv->Items->Add(itemActual);
					this->pend -= Convert::ToDouble(actualTipre->Importe_Ingresado);
					//actualTipre->Importe = actualTipre->Importe;
					if (actPromoMp != nullptr && actualTipre->AlbMontoDescto > 0)
					{
						//actPromoMp->Confirmado = true;
						Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);
						char *mpmsg = new char[100];
						sprintf(mpmsg, "Confirma importe descto $%.2lf", Convert::ToDouble(actualTipre->AlbMontoDescto));
						WriteBootLog(mpmsg);
						delete mpmsg;
						//Dump::promosmp->SetConfirmaDescto(actPromoMp->NumMpago, actPromoMp->NumTarjeta);
					}

					actualTipre = gcnew TipreRequest();
					LimpiarControles();					
					SincroTotal();
					GenValidacion();					
				}
			 else if (!this->EsCompra)
				 Alert("Solo se Permite una Anulación","");
		 }

private: System::Void btnDot_Click(System::Object^  sender, System::EventArgs^  e) {
			 if(tb != nullptr && (tb->Name == "tbMonto" || tb->Name == "txtImpCashBack") && tb->Text->IndexOf('.') < 0)
			 {				
				 if (tb->Text->Length == 0)
					 tb->Text = "0.";
				 else
					 tb->Text = tb->Text->Trim() + ".";
			 }
		 }
};	
	

}
