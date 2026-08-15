#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Bitnet::TJOCommon;
using namespace Controles;
using namespace System::Reflection;

using namespace Alberdi::TJOCommon::Prisma;
using namespace Alberdi::TJOCommon::VerifoneCommon;

#include "pos.h"
#include "Strings.h"
#include "tjonline.h"
#include "total.h"

#include "StaticDump.h"
#include "FUNCS.H"
#include "frmCuponManualPrisma.h"

namespace pos {

	/// <summary>
	/// Summary for TarjOnlineTouch
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class TarjOnlineTouch : public Controles::BaseDialog
	{
	private:
		double pend;
		bool globalTouchEvent;

		List <PromoMedioPago ^> ^actPromoMp;
	private: System::Windows::Forms::Timer^  timerFaltan;
	private: System::Windows::Forms::Label^  lbTotal;

		int CantTotalTarjetas;
		bool inGotFocus;
		bool trabajaSeleccion;
	private: System::Windows::Forms::ComboBox^  cboTarjetas;

			 bool inLostFocus;

	public:
		TarjOnlineTouch(double pendiente)
		{
			struct mpag_ *p;

			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			actuTarjVerifonePrisma = gcnew TransVerifone();

			globalTouchEvent = false;
			this->pend = pendiente;

			if (!tjOnline && defTarjetas.Count() <= 0)
				CargaTjOnline();

			lstTarjPrisma = gcnew List<ShowTarjeta^>();
			ShowTarjeta^obj0 = gcnew ShowTarjeta();
			obj0->NombreTarjeta = "Seleccione una Tarjeta";
			obj0->CodMpagoAlbe = 0;
			obj0->CodPlanPrisma = "0";
			obj0->TipoTarje = 'C';
			lstTarjPrisma->Add(obj0);
			for(int k = 0; k < defTarjetas.Count(); k++)
			{
				TarjDef *def = (TarjDef *) defTarjetas.Item(k);
				p = BuscarMpag(def->MPago);
				ShowTarjeta^actprisma = gcnew ShowTarjeta();
				actprisma->CodMpagoAlbe = def->MPago;
				actprisma->NombreTarjeta = gcnew String(def->Nombre);
				actprisma->CodPlanPrisma = System::Convert::ToString(def->CodPlanPrisma);
				actprisma->CodTarjetaPrisma = gcnew String(def->CodTarjPrisma);
				actprisma->CodTarjetaAlbe = def->Codigo;
				actprisma->NroComercio = gcnew String(def->NumComercio);
				actprisma->TipoTarje = def->tipo == 'C' ? 'C' : 'D';
				if (actprisma->CodTarjetaPrisma->Trim() != "")
					lstTarjPrisma->Add(actprisma);

			}			

			cboTarjetas->DataSource = lstTarjPrisma;
			cboTarjetas->DisplayMember = "NombreTarjeta";
			cboTarjetas->ValueMember = "CodTarjetaPrisma";

			CantTotalTarjetas = System::Convert::ToUInt16(System::Configuration::ConfigurationManager::AppSettings["tiprecanttarj"]);

			indextar0 = cboTarjetas->Items->IndexOf(obj0);
			indexsel = indextar0;
			cboTarjetas->SelectedIndex = indextar0;			

			cuota0 = gcnew ShowCuota();
			cuota0->NombrePlan = "Sin Asignar";
			cuota0->NumCuota = 0;
			cuota0->Recargo = 0;
			lstCuotaPrisma = gcnew BindingList<ShowCuota^>();
			lstCuotaPrisma->Add(cuota0);
			cboPlan->DataSource = lstCuotaPrisma;
			cboPlan->DisplayMember = "NombrePlan";
			cboPlan->ValueMember = "NumCuota";
			cboPlan->SelectedIndex = 0;

			cboPlan->GotFocus += gcnew EventHandler(this, &TarjOnlineTouch::cbGotFocus);
			cboTarjetas->GotFocus += gcnew EventHandler(this, &TarjOnlineTouch::cbGotFocus);

			// Intercepta el teclado.
			Strings::InterceptKeyboard(this, 1);

			// Elimina el teclado si no estamos sobre una touchscreen.
			this->ExtendedKeyboardEnabled = usaTouch;			

			// Prepara la tabulacion de los elementos.
			TabControls->Add(this->cboTarjetas);			

			TabControls->Add(this->cboPlan);



			CantTotalTarjetas = System::Convert::ToUInt16(System::Configuration::ConfigurationManager::AppSettings["tiprecanttarj"]);
			this->Size.Width = 1024;
			this->Size.Height = 768;


			// Sincroniza la lista de tarjetas cargadas
			//SincroLista();
			SincroTotal();
			/*GenValidacion();
			LimpiarControles();*/
			trabajaSeleccion = false;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TarjOnlineTouch()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lbFaltan;







	private: System::Windows::Forms::ComboBox^  cboPlan;
	private: System::Windows::Forms::Label^  lbCardHelp;
	private: System::Windows::Forms::TextBox^  tbDummy;









	private: System::Windows::Forms::Label^  lbReca;


	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;

	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::Label^  lbMonto;
	private: System::Windows::Forms::Button^  btnCargaManual;
	private: System::Windows::Forms::Button^  btnConsultarUltima;
	private: System::ComponentModel::IContainer^  components;





	protected: 
		int indextar0;		//Indice de 'Otra Tarjeta'		
		int indexsel; 

		List<ShowTarjeta^> ^lstTarjPrisma;
		TransVerifone^actuTarjVerifonePrisma;
		BindingList<ShowCuota^> ^lstCuotaPrisma;
		ShowCuota ^cuota0;
		double frecargo;
		double ftasarecargo;


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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TarjOnlineTouch::typeid));
			this->lbFaltan = (gcnew System::Windows::Forms::Label());
			this->cboPlan = (gcnew System::Windows::Forms::ComboBox());
			this->lbCardHelp = (gcnew System::Windows::Forms::Label());
			this->tbDummy = (gcnew System::Windows::Forms::TextBox());
			this->lbReca = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->lbMonto = (gcnew System::Windows::Forms::Label());
			this->timerFaltan = (gcnew System::Windows::Forms::Timer(this->components));
			this->lbTotal = (gcnew System::Windows::Forms::Label());
			this->cboTarjetas = (gcnew System::Windows::Forms::ComboBox());
			this->btnCargaManual = (gcnew System::Windows::Forms::Button());
			this->btnConsultarUltima = (gcnew System::Windows::Forms::Button());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::CadetBlue;
			this->panel->Controls->Add(this->cboTarjetas);
			this->panel->Controls->Add(this->lbTotal);
			this->panel->Controls->Add(this->lbMonto);
			this->panel->Controls->Add(this->cboPlan);
			this->panel->Controls->Add(this->lbCardHelp);
			this->panel->Controls->Add(this->tbDummy);
			this->panel->Controls->Add(this->lbReca);
			this->panel->Controls->Add(this->label3);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->lbFaltan);
			this->panel->Controls->Add(this->btnCargaManual);
			this->panel->Controls->Add(this->btnConsultarUltima);
			this->panel->Size = System::Drawing::Size(1024, 768);
			this->panel->Controls->SetChildIndex(this->btnConsultarUltima, 0);
			this->panel->Controls->SetChildIndex(this->btnCargaManual, 0);
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
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->lbReca, 0);
			this->panel->Controls->SetChildIndex(this->tbDummy, 0);
			this->panel->Controls->SetChildIndex(this->lbCardHelp, 0);
			this->panel->Controls->SetChildIndex(this->cboPlan, 0);
			this->panel->Controls->SetChildIndex(this->lbMonto, 0);
			this->panel->Controls->SetChildIndex(this->lbTotal, 0);
			this->panel->Controls->SetChildIndex(this->cboTarjetas, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(677, 441);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(677, 387);
			// 
			// bBorra
			// 
			this->bBorra->Location = System::Drawing::Point(733, 250);
			// 
			// b0
			// 
			this->b0->Location = System::Drawing::Point(677, 250);
			// 
			// b2
			// 
			this->b2->Location = System::Drawing::Point(733, 198);
			// 
			// b1
			// 
			this->b1->Location = System::Drawing::Point(677, 198);
			// 
			// b5
			// 
			this->b5->Location = System::Drawing::Point(733, 146);
			// 
			// b4
			// 
			this->b4->Location = System::Drawing::Point(677, 146);
			// 
			// b8
			// 
			this->b8->Location = System::Drawing::Point(733, 94);
			// 
			// b7
			// 
			this->b7->Location = System::Drawing::Point(677, 94);
			// 
			// panel3
			// 
			this->panel3->Location = System::Drawing::Point(168, 72);
			this->panel3->Size = System::Drawing::Size(680, 4);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			this->Icono->Location = System::Drawing::Point(185, 27);
			// 
			// Titulo
			// 
			this->Titulo->Location = System::Drawing::Point(228, 36);
			this->Titulo->Size = System::Drawing::Size(360, 23);
			this->Titulo->Text = L"Pago con Tarjetas de Crédito/Debito";
			// 
			// bEnter
			// 
			this->bEnter->Location = System::Drawing::Point(787, 250);
			// 
			// b3
			// 
			this->b3->Location = System::Drawing::Point(787, 198);
			// 
			// b6
			// 
			this->b6->Location = System::Drawing::Point(787, 146);
			// 
			// b9
			// 
			this->b9->Location = System::Drawing::Point(787, 94);
			// 
			// lbFaltan
			// 
			this->lbFaltan->BackColor = System::Drawing::Color::Transparent;
			this->lbFaltan->Font = (gcnew System::Drawing::Font(L"Arial Black", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFaltan->ForeColor = System::Drawing::Color::Crimson;
			this->lbFaltan->Location = System::Drawing::Point(618, 27);
			this->lbFaltan->Name = L"lbFaltan";
			this->lbFaltan->Size = System::Drawing::Size(187, 35);
			this->lbFaltan->TabIndex = 71;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// cboPlan
			// 
			this->cboPlan->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboPlan->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboPlan->FormattingEnabled = true;
			this->cboPlan->Location = System::Drawing::Point(293, 346);
			this->cboPlan->Name = L"cboPlan";
			this->cboPlan->Size = System::Drawing::Size(191, 37);
			this->cboPlan->TabIndex = 131;
			this->cboPlan->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineTouch::cboPlan_Validating);
			this->cboPlan->SelectedIndexChanged += gcnew System::EventHandler(this, &TarjOnlineTouch::cboPlan_SelectedIndexChanged);
			this->cboPlan->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &TarjOnlineTouch::cboPlan_PreviewKeyDown);
			// 
			// lbCardHelp
			// 
			this->lbCardHelp->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lbCardHelp->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbCardHelp->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCardHelp->Location = System::Drawing::Point(133, 94);
			this->lbCardHelp->Name = L"lbCardHelp";
			this->lbCardHelp->Size = System::Drawing::Size(538, 176);
			this->lbCardHelp->TabIndex = 111;
			this->lbCardHelp->Text = resources->GetString(L"lbCardHelp.Text");
			this->lbCardHelp->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbDummy
			// 
			this->tbDummy->Enabled = false;
			this->tbDummy->Location = System::Drawing::Point(474, 102);
			this->tbDummy->Name = L"tbDummy";
			this->tbDummy->Size = System::Drawing::Size(100, 20);
			this->tbDummy->TabIndex = 130;
			// 
			// lbReca
			// 
			this->lbReca->AutoSize = true;
			this->lbReca->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbReca->ForeColor = System::Drawing::Color::Beige;
			this->lbReca->Location = System::Drawing::Point(189, 420);
			this->lbReca->Name = L"lbReca";
			this->lbReca->Size = System::Drawing::Size(208, 29);
			this->lbReca->TabIndex = 120;
			this->lbReca->Text = L"(Recargo $0.00)";
			this->lbReca->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::Beige;
			this->label3->Location = System::Drawing::Point(200, 350);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(90, 26);
			this->label3->TabIndex = 117;
			this->label3->Text = L"Cuotas:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label2
			// 
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Beige;
			this->label2->Location = System::Drawing::Point(186, 477);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(90, 26);
			this->label2->TabIndex = 116;
			this->label2->Text = L"Monto:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Beige;
			this->label1->Location = System::Drawing::Point(180, 293);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(110, 26);
			this->label1->TabIndex = 114;
			this->label1->Text = L"Tarjeta:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// lbMonto
			// 
			this->lbMonto->AutoSize = true;
			this->lbMonto->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbMonto->ForeColor = System::Drawing::Color::Beige;
			this->lbMonto->Location = System::Drawing::Point(281, 477);
			this->lbMonto->Name = L"lbMonto";
			this->lbMonto->Size = System::Drawing::Size(88, 29);
			this->lbMonto->TabIndex = 136;
			this->lbMonto->Text = L"$ 0.00";
			this->lbMonto->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &TarjOnlineTouch::timerFaltan_Tick);
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
			this->lbTotal->Location = System::Drawing::Point(677, 306);
			this->lbTotal->Name = L"lbTotal";
			this->lbTotal->Size = System::Drawing::Size(162, 64);
			this->lbTotal->TabIndex = 137;
			this->lbTotal->Text = L"Total: $150.25";
			this->lbTotal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// cboTarjetas
			// 
			this->cboTarjetas->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cboTarjetas->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cboTarjetas->FormattingEnabled = true;
			this->cboTarjetas->ItemHeight = 29;
			this->cboTarjetas->Location = System::Drawing::Point(293, 291);
			this->cboTarjetas->Name = L"cboTarjetas";
			this->cboTarjetas->Size = System::Drawing::Size(268, 37);
			this->cboTarjetas->Sorted = true;
			this->cboTarjetas->TabIndex = 139;
			this->cboTarjetas->SelectedIndexChanged += gcnew System::EventHandler(this, &TarjOnlineTouch::cboTarjetas_SelectedIndexChanged);
			this->cboTarjetas->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &TarjOnlineTouch::cboTarjetas_PreviewKeyDown);
			//
			// btnCargaManual
			//
			this->btnCargaManual->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnCargaManual->Location = System::Drawing::Point(130, 560);
			this->btnCargaManual->Name = L"btnCargaManual";
			this->btnCargaManual->Size = System::Drawing::Size(300, 60);
			this->btnCargaManual->TabIndex = 140;
			this->btnCargaManual->Text = L"Carga Manual de Cupon";
			this->btnCargaManual->UseVisualStyleBackColor = true;
			this->btnCargaManual->Visible = false;
			this->btnCargaManual->Click += gcnew System::EventHandler(this, &TarjOnlineTouch::btnCargaManual_Click);
			//
			// btnConsultarUltima
			//
			this->btnConsultarUltima->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnConsultarUltima->Location = System::Drawing::Point(130, 630);
			this->btnConsultarUltima->Name = L"btnConsultarUltima";
			this->btnConsultarUltima->Size = System::Drawing::Size(300, 60);
			this->btnConsultarUltima->TabIndex = 141;
			this->btnConsultarUltima->Text = L"Consultar Ultima Operacion";
			this->btnConsultarUltima->UseVisualStyleBackColor = true;
			this->btnConsultarUltima->Visible = false;
			this->btnConsultarUltima->Click += gcnew System::EventHandler(this, &TarjOnlineTouch::btnConsultarUltima_Click);
			//
			// TarjOnlineTouch
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1024, 768);
			this->Name = L"TarjOnlineTouch";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"TarjOnlineTouch";
			this->TopMost = false;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &TarjOnlineTouch::TarjOnlineTouch_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion

		System::Void AnyTbClick(System::Object^  sender, System::EventArgs^  e) 
		{
			if(pantalla != 1 && pantalla != 3 && pantalla != 7) 
				SetFocusTo((Control ^)sender);
		}

		void GenValidacion()
		{
			bool res = false;

			/*res = actualTipre != nullptr && actualTipre->DatosValidos() == "OK";

			timerFaltan->Enabled = !res && actualTipre->Nro_Tarjeta != "";
			if(actualTipre != nullptr && actualTipre->Nro_Tarjeta != "")
			{
				lbFaltan->Visible = true;
				lbFaltan->Text = (!res? "Faltan Datos...": "Tarjeta Lista...");
				lbFaltan->ForeColor = (res? Color::Red: Color::Beige);
				lbFaltan->BackColor = (res? Color::White: Color::Blue);
				lbFaltan->BorderStyle = (res? System::Windows::Forms::BorderStyle::FixedSingle: System::Windows::Forms::BorderStyle::None);
				btOk->Enabled = res;
			}
			else
				lbFaltan->Visible = false;*/
		
		}

		void SincroTotal()
		{
			char tmp[50];
			sprintf(tmp, "%.2lf", pend);
			this->lbTotal->Text = "Falta: $" + Strings::CharToString(tmp);
			this->lbMonto->Text = "$ " + Strings::CharToString(tmp);
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

		// Completa el cupon con datos cargados a mano o recuperados por consulta al Prisma
		// Integrado (en vez de la respuesta normal de HacerVenta), y sigue el mismo flujo de
		// cierre que una aprobacion automatica.
		void CompletarCuponManual(int lote, int cupon, System::String ^autor, System::String ^origen)
		{
			actuTarjVerifonePrisma->CodigoRespuesta = "0";
			actuTarjVerifonePrisma->MensajeRespuesta = origen;
			actuTarjVerifonePrisma->NroLote = lote;
			actuTarjVerifonePrisma->NroCupon = cupon;
			actuTarjVerifonePrisma->NroAutorizacion = autor;
			actuTarjVerifonePrisma->OperacionTipo = actuTarjVerifonePrisma->ImporteCashBack > 0 ? "COMPRA+CASHBACK" : "COMPRA";
			actuTarjVerifonePrisma->AlbEtapa = 3;

			if (actPromoMp != nullptr)
				Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);
			Dump::TarjPrisma->Add(actuTarjVerifonePrisma);
			actuTarjVerifonePrisma = gcnew TransVerifone();

			btnCargaManual->Visible = false;
			btnConsultarUltima->Visible = false;

			if (Dump::TarjPrisma->Count == 1)
			{
				bEnter_Click(this, System::EventArgs::Empty);
				BaseDialog::btOk_Click(this, System::EventArgs::Empty);
			}
			else
				Alert("No se autorizaron tarjetas", "...");
		}

		// Plan 1: carga 100% manual del cupon, autorizada por un supervisor. Pensado para
		// cuando el Prisma Integrado aprueba la operacion pero no devuelve la respuesta.
		System::Void btnCargaManual_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (indextar0 == cboTarjetas->SelectedIndex)
			{
				Alert("Debe elegir una tarjeta antes de cargar el cupon", "");
				return;
			}
			CalculosImporte();

			char nombreSuperv[50];
			if (!AutorizaSupervisorPuntual("Carga Manual de Cupon Prisma (Autocobro)", nombreSuperv, 50))
				return;

			frmCuponManualPrisma ^fcupon = gcnew frmCuponManualPrisma();
			if (fcupon->ShowDialog() != System::Windows::Forms::DialogResult::OK)
				return;

			CompletarCuponManual(fcupon->NroLoteIng, fcupon->NroCuponIng, fcupon->NroAutorIng,
				"CARGA MANUAL - Superv: " + Strings::CharToString(nombreSuperv));
		}

		// Plan 2: consulta la ultima operacion aprobada en el Prisma Integrado (comando ULT) y,
		// si el supervisor confirma que corresponde, la usa para completar el cupon.
		System::Void btnConsultarUltima_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (indextar0 == cboTarjetas->SelectedIndex)
			{
				Alert("Debe elegir una tarjeta antes de consultar", "");
				return;
			}
			CalculosImporte();

			char nombreSuperv[50];
			if (!AutorizaSupervisorPuntual("Consulta Ultima Operacion Prisma (Autocobro)", nombreSuperv, 50))
				return;

			OperarTrxPrisma ^hacer = gcnew OperarTrxPrisma(COM_NLD);
			RtaTrans ^aux = hacer->ObtenerUltimaTrans();
			if (hacer->ConError || aux == nullptr || aux->CodigoRespuesta != 0)
			{
				char msgErrorNewl[50];
				if (hacer->ConError)
					Strings::StringToChar(hacer->MensajeError, msgErrorNewl, 50);
				Alert("No se pudo recuperar una operacion aprobada", hacer->ConError ? msgErrorNewl : "");
				return;
			}

			frmCuponManualPrisma ^fcupon = gcnew frmCuponManualPrisma(aux->NumeroLote, aux->NumeroCupon, aux->CodigoAutorizacion, aux->Ult4Digitos, aux->Prim6Digitos);
			if (fcupon->ShowDialog() != System::Windows::Forms::DialogResult::OK)
				return;

			CompletarCuponManual(fcupon->NroLoteIng, fcupon->NroCuponIng, fcupon->NroAutorIng,
				"CONSULTA ULT.OPERACION - Superv: " + Strings::CharToString(nombreSuperv));
		}

		protected: virtual void OnLoad(EventArgs ^e) override
				   {
					   //lv_SelectedIndexChanged(nullptr, nullptr);
					   BaseDialog::OnLoad(e);
				   }



		 /*private: void LimpiarControles()
				  {
					  this->lbReca->Text = "";					  
				  }

		 private: void ActualizarDatosTarjeta()
				  {
					  this->lbMonto->Text = "$ " + actuTarjVerifonePrisma->Importe.ToString("F2");					  
					  this->lbReca->Text = "(Recargo: $" + actuTarjVerifonePrisma->ImporteRecargo.ToString("F2") + ")";					  
				  }*/

	
		System::Void TarjOnlineTouch_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
		{
			Strings::releaseKeyboard(this);
		}

			

		public: virtual bool ProcessHotKey(Keys key) override
		{
			if (key == Keys::Down)
			{
				if (this->tb->Name == "cboPlan")
				{
					if (pantalla != 2)//la pantalla es Touch Screen
						Strings::DisableKey(this);
					cboPlan->DroppedDown = true;
					return true;						 
				}
				if (this->tb->Name == "cboTarjetas")
				{
					if (pantalla != 2)//la pantalla es Touch Screen
						Strings::DisableKey(this);
					cboTarjetas->DroppedDown = true;
					return true;						 
				}
			}
			return BaseDialog::ProcessHotKey(key);
		}

		
		System::Void cboPlan_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		{
			if((pantalla == 1 || pantalla == 3 || pantalla == 5 || pantalla == 7) || globalTouchEvent)
			{				
				//CalculosImporte();

				GenValidacion();
			}
		 }
		
		

	protected: 
		virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		{
			btnCargaManual->Visible = false;
			btnConsultarUltima->Visible = false;

			ShowTarjeta^ mdatos = (ShowTarjeta^) cboTarjetas->SelectedItem;
			if (!String::IsNullOrEmpty(mdatos->CodTarjetaPrisma))
			{
				if (indextar0 == cboTarjetas->SelectedIndex){
					Alert("Debe elegir una tarjeta de la tarjeta a autorizar", "");
					return;
				}

				CalculosImporte();

				TrxPrisma ^trCompra = gcnew TrxPrisma();
				trCompra->CodigoTarjeta = mdatos->CodTarjetaPrisma;
				trCompra->CodigoPlan = mdatos->CodPlanPrisma;
				trCompra->Monto = actuTarjVerifonePrisma->Importe;
				trCompra->CantCuotas = actuTarjVerifonePrisma->AlbNumCuota;
				trCompra->NroFactura = String::Format("{0:000000000000}", pf->GetTickNro(GlobalTF));
				trCompra->MontoExtraCash = actuTarjVerifonePrisma->ImporteCashBack;
				trCompra->CodigoComercio = mdatos->NroComercio;

				//OperarTrxPrisma ^hacer = gcnew OperarTrxPrisma(COM_NLD);
				RtaVenta ^mrta = nullptr;
				//ClearMsg();
				//SendMsg("Realizando Operacion por Verifone Prisma", "Aguarde un instante");
				MsgAlertPrisma ^otroMsg = gcnew MsgAlertPrisma(trCompra, COM_NLD);
				otroMsg->ShowMsgProcesaVenta("Realizando Operacion por Verifone Prisma", "Aguarde un instante");
				mrta = otroMsg->RespuestaVenta;
				if (!otroMsg->ConError){
					//if (mrta != nullptr && trCompra->CodigoTarjeta == "QRQ" && mrta->CodigoRespuesta == 0){
					if (mrta != nullptr && mdatos->CodTarjetaPrisma == "QRQ" && mrta->CodigoRespuesta == 0){
						actuTarjVerifonePrisma->QrModo = true;
						bool seencontro = false;
						for each(System::Object ^ptitem in this->cboTarjetas->Items)
						{
							ShowTarjeta^unitem = (ShowTarjeta^) ptitem;
							RtaQrModo ^sprta = (RtaQrModo ^) mrta;
							if (unitem->CodTarjetaPrisma == sprta->CodigoTarjeta){
								actuTarjVerifonePrisma->AlbNroTarjeta = unitem->CodTarjetaAlbe;
								actuTarjVerifonePrisma->AlbNroMpago = unitem->CodMpagoAlbe;	
								actuTarjVerifonePrisma->longaut = unitem->LongAuto;
								seencontro = true;
								//break;
							}										 
						}
					}
				}
				else
				{
					MessageBox::Show(otroMsg->MensajeError);
					// No hubo respuesta clara del Prisma Integrado (pudo haberse aprobado igual):
					// habilita las alternativas de carga manual / consulta, con autorizacion de supervisor.
					btnCargaManual->Visible = true;
					btnConsultarUltima->Visible = true;
				}

				//ClearMsg();
				//otroMsg->CerrarVentana();
				if (mrta != nullptr)
				{
					actuTarjVerifonePrisma->CodigoRespuesta = mrta->CodigoRespuesta.ToString();
					actuTarjVerifonePrisma->MensajeRespuesta = mrta->MensajeRespuesta;
					//la operacion esta aprobada
					if (mrta->CodigoRespuesta == 0)
					{
						actuTarjVerifonePrisma->NroLote = mrta->NumeroLote;
						actuTarjVerifonePrisma->NroCupon = mrta->NumeroCupon;
						actuTarjVerifonePrisma->NroAutorizacion = mrta->CodigoAutorizacion;
						actuTarjVerifonePrisma->Ultimos4Digitos = mrta->Ult4Digitos;
						actuTarjVerifonePrisma->Prim6Digitos = mrta->Prim6Digitos;
						actuTarjVerifonePrisma->OperacionTipo = actuTarjVerifonePrisma->ImporteCashBack > 0 ? "COMPRA+CASHBACK" : "COMPRA";
						actuTarjVerifonePrisma->NombreCliente = mrta->NombreCliente;
						actuTarjVerifonePrisma->AlbEtapa = 3;
						actuTarjVerifonePrisma->NroTerminal = System::Convert::ToString(mrta->TerminalId);
						//this->faltante = faltante - actuTarjVerifonePrisma->ImporteIngresado;
						//para confirmar los descuentos por medio de pago
						if (actPromoMp != nullptr)
							Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);
						Dump::TarjPrisma->Add(actuTarjVerifonePrisma);								  
						actuTarjVerifonePrisma = gcnew TransVerifone();
						//lblFaltante->Text = "Faltan $: " + this->faltante.ToString("F2");
						//LlenarLvw();
						//NuevoCupon();								  
					}
					else
					{
						MessageBox::Show(mrta->MensajeRespuesta);
					}
				}

				//OperarTrxPrisma ^hacer = gcnew OperarTrxPrisma(COM_NLD);
				//RtaVenta ^mrta = nullptr;
				//ClearMsg();
				//SendMsg("Realizando Operacion por Verifone Prisma", "Aguarde un instante");

				//mrta = hacer->HacerVenta(trCompra);

				//ClearMsg();
				//if (hacer->ConError)
				//	MessageBox::Show(hacer->MensajeError);
				//else
				//{
				//	actuTarjVerifonePrisma->CodigoRespuesta = mrta->CodigoRespuesta.ToString();
				//	actuTarjVerifonePrisma->MensajeRespuesta = mrta->MensajeRespuesta;
				//	//la operacion esta aprobada
				//	if (mrta->CodigoRespuesta == 0)
				//	{
				//		actuTarjVerifonePrisma->NroLote = mrta->NumeroLote;
				//		actuTarjVerifonePrisma->NroCupon = mrta->NumeroCupon;
				//		actuTarjVerifonePrisma->NroAutorizacion = mrta->CodigoAutorizacion;
				//		actuTarjVerifonePrisma->Ultimos4Digitos = mrta->Ult4Digitos;
				//		actuTarjVerifonePrisma->Prim6Digitos = mrta->Prim6Digitos;
				//		actuTarjVerifonePrisma->OperacionTipo = actuTarjVerifonePrisma->ImporteCashBack > 0 ? "COMPRA+CASHBACK" : "COMPRA";
				//		actuTarjVerifonePrisma->NombreCliente = mrta->NombreCliente;
				//		actuTarjVerifonePrisma->AlbEtapa = 3;

				//		//para confirmar los descuentos por medio de pago
				//		if (actPromoMp != nullptr)
				//			Dump::promosmp->SetConfirmaDescto(actPromoMp[0]->NumMpago, actPromoMp[0]->NumTarjeta);
				//		Dump::TarjPrisma->Add(actuTarjVerifonePrisma);						
				//	}
				//	else
				//	{
				//		MessageBox::Show(mrta->MensajeRespuesta);
				//	}
				//}
			}		
			if (Dump::TarjPrisma->Count == 1)
			{			
				bEnter_Click(sender, e);
				BaseDialog::btOk_Click(sender, e);
			}
			else
				Alert("No se autorizaron tarjetas", "...");
		}

		virtual void bEnter_Click(Object ^sender, EventArgs ^e) override
		{
			if(pantalla != 1 || pantalla == 7)
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


		
private: System::Void cboPlan_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
			 if (e->KeyCode == Keys::Enter)
			 {
				 cboPlan->DroppedDown = false;
				 Strings::EnableKey(this);
				 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
				 cboPlan_Validating(this, gcnew CancelEventArgs());
			 }
		 }

		 System::Void TbGotFocus(Object ^sender, EventArgs ^e)
		 {
			 if(inGotFocus) return;
			 inGotFocus = true;

			 TextBox ^ctl = dynamic_cast<TextBox ^>(sender);
			 if(ctl != nullptr)
			 {
				SetFocusTo(ctl);
				ctl->SelectAll();
			 }
			 
			 inGotFocus = false;
		 }

		 System::Void cbGotFocus(Object ^sender, EventArgs ^e)
		 {
			 if(inGotFocus) return;
			 inGotFocus = true;

			 ComboBox ^ctl = dynamic_cast<ComboBox ^>(sender);
			 if(ctl != nullptr)
			 {
				 SetFocusTo(ctl);
				 //ctl->SelectAll();
			 }
			 inGotFocus = false;
		 }
		 

	protected: virtual void SetFocusTo(Control ^ctl) override
		  {
			  if (tb != nullptr)
			  {			  
				  if (tb->Name == "cboPlan")
					  cboPlan_Validating(this, gcnew System::ComponentModel::CancelEventArgs());
			  }

			  BaseDialog::SetFocusTo(ctl);
		  }

	private: System::Void cboTarjetas_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e) {
				 if (e->KeyCode == Keys::Enter)
				 {
					 cboTarjetas->DroppedDown = false;
					 Strings::EnableKey(this);
					 Strings::KDown(this, gcnew KeyEventArgs(e->KeyData));
					 
				 }

			 }
private: System::Void cboTarjetas_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (trabajaSeleccion)
				 return;
			 trabajaSeleccion = true;
			 if (lstCuotaPrisma != nullptr && lstTarjPrisma != nullptr){
				 lstCuotaPrisma->Clear();
				 if (indextar0 != cboTarjetas->SelectedIndex)
				 {
					 indexsel = cboTarjetas->SelectedIndex;
					 ShowTarjeta^ mdatos = (ShowTarjeta^) cboTarjetas->SelectedItem;
					 TarjDef *def;
					 bool encontrado = false;
					 for(int i = 0; i < defTarjetas.Count(); i++)
					 {
						 def = (TarjDef *) defTarjetas.Item(i);
						 if (def->Codigo == mdatos->CodTarjetaAlbe)
						 {
							 encontrado = true;
							 break;
						 }
					 }
					 if (encontrado)
					 {
						 for(int i=0; i < def->Cuotas->Count(); i++)
						 {
							 TarjCuota *tc = (TarjCuota *) def->Cuotas->Item(i);
							 ShowCuota ^ctcuota = gcnew ShowCuota();
							 ctcuota->NombrePlan = gcnew String(tc->Nombre);
							 ctcuota->NumCuota = tc->Cuota;
							 ctcuota->Recargo = System::Convert::ToDecimal(tc->Porcen);
							 lstCuotaPrisma->Add(ctcuota);
						 }							 
					 }
				 }
				 else
				 {
					 lstCuotaPrisma->Add(cuota0);
					 cboPlan->SelectedIndex = 0;
				 }
			 }
			 trabajaSeleccion = false;
		 }
private: System::Void cboPlan_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			 if (trabajaSeleccion)
				 return;

			 trabajaSeleccion = true;
			 CalculosImporte();

			 trabajaSeleccion = false;
		 }

		 System::Void CalculosImporte(){
			 if (lstCuotaPrisma != nullptr && lstTarjPrisma != nullptr){
				 ShowTarjeta^ mdatos = (ShowTarjeta^) cboTarjetas->SelectedItem;
				 TarjDef *def;
				 bool encontrado = false;
				 for(int i = 0; i < defTarjetas.Count(); i++)
				 {
					 def = (TarjDef *) defTarjetas.Item(i);
					 if (def->Codigo == mdatos->CodTarjetaAlbe)
					 {
						 encontrado = true;
						 break;
					 }
				 }
				 ShowCuota^ scuota = (ShowCuota^) cboPlan->SelectedItem;
				 int lpcuotas = scuota->NumCuota;

				 actuTarjVerifonePrisma->ImporteCashBack = 0;				

				 actPromoMp = nullptr;
				 if (encontrado && lpcuotas > 0)
				 {
					 TarjInfo *actualTarj = new TarjInfo();
					 actualTarj->Definicion = def;

					 double uMonto = pend;					

					 actualTarj->Recargo = 0;
					 actualTarj->TasaRecargo = 0;
					 actualTarj->Monto = uMonto;
					 actualTarj->Cuotas = lpcuotas;

					 ///////
					 actuTarjVerifonePrisma->AlbNroTarjeta = actualTarj->Definicion->Codigo;
					 actuTarjVerifonePrisma->AlbNroMpago = actualTarj->Definicion->MPago;
					 actuTarjVerifonePrisma->AlbNumCuota = lpcuotas;
					 actuTarjVerifonePrisma->ImporteIngresado = Convert::ToDecimal(uMonto);
					 actuTarjVerifonePrisma->NombreEmisor = gcnew String(actualTarj->Definicion->Nombre);
					 actuTarjVerifonePrisma->longaut = actualTarj->Definicion->longaut;
					 actuTarjVerifonePrisma->TarjCredito = actualTarj->Definicion->tipo == 'C';
					 actualTarj->montoIngresado = uMonto;
					 actualTarj->tasaDescto = def->porDesctoCom;				

					 Dump::promosmp->CalculoDescto(def->MPago, def->Codigo, uMonto);//para el recalculo de las promociones por medio de pago				
					 actPromoMp = Dump::promosmp->BuscarConTarjeta(def->MPago, def->Codigo);

					 TarjCuota *cuo = actualTarj->DatosCuota();
					 if(cuo == NULL)
					 {
						 Alert("El numero de cuotas indicado NO es Valido!!", "Ingreselo de nuevo...");
						 actualTarj->Cuotas = 1;							
					 }
					 else
					 {
						 Decimal rec = Decimal(cuo->Porcen / 100 * uMonto);					
						 Decimal cajaDescto = 0;					
						 if(actPromoMp != nullptr)
						 {
							 cajaDescto = 0;						
							 for(int ki = 0; ki < actPromoMp->Count; ki++)
							 {
								 PromoMedioPago ^miact = actPromoMp[ki];				
								 if (miact->Monto > 0 && !miact->Confirmado)// tope 2 veces el descuento								
								 {
									 cajaDescto += miact->Monto;							
									 //actPromoMp->ImporteIngresado = monto;
								 }								
							 }
						 }
						 else
							 cajaDescto = 0;
						 cajaDescto = System::Decimal::Round(cajaDescto,2);
						 actualTarj->montoDescto = System::Convert::ToDouble(cajaDescto);						
						 rec -= cajaDescto;
						 if (rec != 0){
							 lbReca->Visible = true;
							 lbReca->Text = String::Format("Recargo ${0}", rec.ToString("F2"));
						 }
						 else
							 lbReca->Visible = false;

						 frecargo = Decimal::ToDouble(Decimal::Round(rec, 2));
						 ftasarecargo = cuo->Porcen;
						 actuTarjVerifonePrisma->ImporteRecargo = Convert::ToDecimal(frecargo);					
						 actuTarjVerifonePrisma->Importe = actuTarjVerifonePrisma->ImporteIngresado + actuTarjVerifonePrisma->ImporteRecargo;
						 lbMonto->Text = actuTarjVerifonePrisma->Importe.ToString("F2");

					 }
					 delete actualTarj;
				 }
			 }
		 }
};

	
}
