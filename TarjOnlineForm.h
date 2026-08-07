#pragma once
#ifdef POSXP
using namespace System;
using namespace System::Reflection;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace Bitnet::TJOCommon;

#include "pos.h"
#include "Strings.h"
#include "tjonline.h"
#include "total.h"

namespace pos {

	/// <summary>
	/// Resumen de TarjOnlineForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class TarjOnlineForm : public Controles::BaseDialog
	{
	private:
		
		delegate void InvokeSupervCode();

		TarjInfo *actualTarj;
		double pend;
		InvokeSupervCode ^supervDelegate;
		bool globalTouchEvent;
	private: System::Windows::Forms::Panel^  pnlPlan;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TextBox^  tbPlan;
	private: System::Windows::Forms::Label^  label10;
























		ListViewItem ^itemActual;

		void GenValidacion()
		{
			bool res = actualTarj != NULL && !actualTarj->DatosValidos();
			timerFaltan->Enabled = res;
			if(actualTarj != NULL)
			{
				lbFaltan->Visible = true;
				lbFaltan->Text = (res? "Faltan Datos...": "Tarjeta Lista...");
				lbFaltan->ForeColor = (res? Color::Red: Color::Beige);
				lbFaltan->BackColor = (res? Color::White: Color::Blue);
				lbFaltan->BorderStyle = (res? System::Windows::Forms::BorderStyle::FixedSingle: System::Windows::Forms::BorderStyle::None);
			}
			else
				lbFaltan->Visible = false;

			btOk->Enabled = actualTarj != NULL && !res;
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
			ListViewItem ^lvi = lv->Items->Add("Nueva...");
			lvi->ImageIndex = -1;

			for(int i = 0; i < infoTarjetas.Count(); i++)
			{
				TarjInfo *tarj = (TarjInfo *)infoTarjetas.Item(i);
				lvi = lv->Items->Add(Strings::CharToString(tarj->Marca()));
				lvi->SubItems->Add(Strings::CharToString(tarj->TarjetaEnmascarada(true)));
				lvi->SubItems->Add(tarj->Cuotas.ToString());
				lvi->SubItems->Add(tarj->Monto.ToString("N2"));

				lvi->ImageIndex = i;				// Lo usa como tag.
				if(i == 0) 
				{
					itemActual = lvi;
					lvi->Selected = true;	// El primer item se selecciona siempre
				}
			}

			lv->ResumeLayout();
		}

		void DoSupervCode()
		{
			if(actualTarj != NULL)
			{
				actualTarj->UsoLector = false;
				lv_SelectedIndexChanged(nullptr, nullptr);
			}
		}

	public:

		void AcceptSupervisorCode()
		{
			if(this->InvokeRequired)
				this->Invoke(supervDelegate);
			else
				DoSupervCode();
		}

		TarjOnlineForm(double pendiente)
		{
			// Inicializacion para windows forms
			InitializeComponent();
			globalTouchEvent = false;
			this->pend = pendiente;
			this->CardReaderEnabled = true;
			this->tbCuotas->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &TarjOnlineForm::txtSoloNumeros_KeyPress);
			supervDelegate = gcnew InvokeSupervCode(this, &TarjOnlineForm::DoSupervCode);

			// Intercepta el teclado.
			Strings::InterceptKeyboard(this, 1);

			// Elimina el teclado si no estamos sobre una touchscreen.
			this->ExtendedKeyboardEnabled = usaTouch;

			// Prepara la tabulacion de los elementos.
			TabControls->Add(this->tbCard);
			TabControls->Add(this->tbVenc);
			TabControls->Add(this->tbNom);
			TabControls->Add(this->ult4Dig);
			TabControls->Add(this->tbDni);
			TabControls->Add(this->tbCvv);
			TabControls->Add(this->tbMonto);
			TabControls->Add(this->tbCuotas);
			TabControls->Add(this->tbPlan);

			// Sincroniza la lista de tarjetas cargadas
			SincroLista();
			SincroTotal();
			GenValidacion();
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén utilizando.
		/// </summary>
		~TarjOnlineForm()
		{
			if (components)
				delete components;
		}

private: System::Void lv_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			// Determina la tarjeta 'actual'
			if(lv->SelectedIndices->Count != 0)
			{
				itemActual = lv->SelectedItems[0];
				int idx = itemActual->ImageIndex;
				if(idx >= 0)
					actualTarj = (TarjInfo *)infoTarjetas.Item(itemActual->ImageIndex);
				else
					actualTarj = NULL;

				lv->Refresh();
			}
			else
			{
				actualTarj = NULL;
				itemActual = nullptr;
			}

			// Actualiza los elementos en la lista
			if(itemActual != nullptr && actualTarj != NULL)
			{
				itemActual->Text = Strings::CharToString(actualTarj->Marca());
				itemActual->SubItems[1]->Text = Strings::CharToString(actualTarj->TarjetaEnmascarada(true));
				itemActual->SubItems[2]->Text = actualTarj->Cuotas.ToString();
				itemActual->SubItems[3]->Text = actualTarj->Monto.ToString("N2");
			}

			// Carga los textboxes
			if(actualTarj == NULL)
			{
				this->tbCard->Text = "";
				this->tbVenc->Text = "";
				this->tbNom->Text = "";
				this->tbMonto->Text = "";
				this->tbCuotas->Text = "";
				this->tbDni->Text = "";
				this->tbCvv->Text = "";
				
				this->lbMarca->Text = "";
				this->lbCuoAvail->Text = "";
				this->lbCuotas->Text = "";
				this->lbReca->Text = "";

				this->tbCard->Enabled = false;
				this->tbVenc->Enabled = false;
				this->tbNom->Enabled = false;
				this->tbMonto->Enabled = false;
				this->tbCuotas->Enabled = false;
				this->tbDni->Enabled = false;
				this->tbCvv->Enabled = false;
			}
			else
			{
				this->tbCard->Text = Strings::CharToString(actualTarj->Tarjeta);
				this->tbVenc->Text = Strings::CharToString(actualTarj->VencimFormateado());
				this->tbNom->Text = Strings::CharToString(actualTarj->Nombre);
				this->tbMonto->Text = actualTarj->Monto.ToString("N2");
				this->tbCuotas->Text = actualTarj->Cuotas.ToString();
				this->tbDni->Text = Strings::CharToString(actualTarj->DniFormateado());
				this->tbCvv->Text = actualTarj->Cvv.ToString();
				
				this->lbMarca->Text = "(" + Strings::CharToString(actualTarj->Marca()) + ")";
				this->lbCuoAvail->Text = "Cuotas Posibles: " + Strings::CharToString(actualTarj->CuotasFormateadas());
				this->lbCuotas->Text = Strings::CharToString(actualTarj->NombreCuota());
				this->lbReca->Text = "(Recargo: $" + actualTarj->Recargo.ToString("N2") + ")";

				this->tbCard->Enabled = !actualTarj->UsoLector;
				this->tbVenc->Enabled = !actualTarj->UsoLector;
				this->tbNom->Enabled = false;
				this->tbMonto->Enabled = true;
				this->tbCuotas->Enabled = true;
				this->tbDni->Enabled = true;
				this->tbCvv->Enabled = (actualTarj->Definicion == NULL || actualTarj->Definicion->PideCvv);
			}

			this->tbCvv->Visible = this->tbCvv->Enabled;
			this->lbCvv->Visible = this->tbCvv->Enabled;
			VerPlan();
		}

	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Button^  btBorrar;
	private: System::Windows::Forms::TextBox^  tbCard;


	private: System::Windows::Forms::ListView^  lv;
	private: System::Windows::Forms::ColumnHeader^  colMarca;
	private: System::Windows::Forms::ColumnHeader^  colNumero;
	private: System::Windows::Forms::ColumnHeader^  colCuotas;
	private: System::Windows::Forms::ColumnHeader^  colMonto;
	private: System::Windows::Forms::Label^  lbReca;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  tbDni;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  tbCuotas;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  tbMonto;
	private: System::Windows::Forms::Label^  lbCvv;


	private: System::Windows::Forms::TextBox^  tbCvv;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  tbNom;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  tbVenc;
	private: System::Windows::Forms::Label^  lbCuotas;
	private: System::Windows::Forms::Label^  lbCardHelp;

	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  lbMarca;
	private: System::Windows::Forms::Label^  lbTotal;
	private: System::Windows::Forms::Label^  lbCuoAvail;
	private: System::Windows::Forms::Label^  lbFaltan;
	private: System::Windows::Forms::Timer^  timerFaltan;
	private: System::Windows::Forms::TextBox^  tbDummy;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  ult4Dig;




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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(TarjOnlineForm::typeid));
			System::Windows::Forms::ListViewItem^  listViewItem1 = (gcnew System::Windows::Forms::ListViewItem(gcnew cli::array< System::String^  >(4) {L"Mastercard", 
				L"**** - 1241", L"12", L"15.21"}, -1));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btBorrar = (gcnew System::Windows::Forms::Button());
			this->tbCard = (gcnew System::Windows::Forms::TextBox());
			this->lv = (gcnew System::Windows::Forms::ListView());
			this->colMarca = (gcnew System::Windows::Forms::ColumnHeader());
			this->colNumero = (gcnew System::Windows::Forms::ColumnHeader());
			this->colCuotas = (gcnew System::Windows::Forms::ColumnHeader());
			this->colMonto = (gcnew System::Windows::Forms::ColumnHeader());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbMonto = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->tbCuotas = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->tbDni = (gcnew System::Windows::Forms::TextBox());
			this->lbReca = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->tbVenc = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->tbNom = (gcnew System::Windows::Forms::TextBox());
			this->lbCvv = (gcnew System::Windows::Forms::Label());
			this->tbCvv = (gcnew System::Windows::Forms::TextBox());
			this->lbCuotas = (gcnew System::Windows::Forms::Label());
			this->lbCardHelp = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->lbMarca = (gcnew System::Windows::Forms::Label());
			this->lbTotal = (gcnew System::Windows::Forms::Label());
			this->lbCuoAvail = (gcnew System::Windows::Forms::Label());
			this->lbFaltan = (gcnew System::Windows::Forms::Label());
			this->timerFaltan = (gcnew System::Windows::Forms::Timer(this->components));
			this->tbDummy = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->ult4Dig = (gcnew System::Windows::Forms::TextBox());
			this->pnlPlan = (gcnew System::Windows::Forms::Panel());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->tbPlan = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->panel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->BeginInit();
			this->pnlPlan->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel
			// 
			this->panel->BackColor = System::Drawing::Color::Teal;
			this->panel->Controls->Add(this->pnlPlan);
			this->panel->Controls->Add(this->label5);
			this->panel->Controls->Add(this->ult4Dig);
			this->panel->Controls->Add(this->lbCardHelp);
			this->panel->Controls->Add(this->tbDummy);
			this->panel->Controls->Add(this->lbFaltan);
			this->panel->Controls->Add(this->lbTotal);
			this->panel->Controls->Add(this->lbMarca);
			this->panel->Controls->Add(this->label9);
			this->panel->Controls->Add(this->lbCuotas);
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
			this->panel->Controls->Add(this->tbCuotas);
			this->panel->Controls->Add(this->label2);
			this->panel->Controls->Add(this->tbMonto);
			this->panel->Controls->Add(this->label1);
			this->panel->Controls->Add(this->btBorrar);
			this->panel->Controls->Add(this->tbCard);
			this->panel->Controls->Add(this->lv);
			this->panel->Controls->Add(this->lbCuoAvail);
			this->panel->Size = System::Drawing::Size(659, 532);
			this->panel->Controls->SetChildIndex(this->lbCuoAvail, 0);
			this->panel->Controls->SetChildIndex(this->lv, 0);
			this->panel->Controls->SetChildIndex(this->tbCard, 0);
			this->panel->Controls->SetChildIndex(this->btBorrar, 0);
			this->panel->Controls->SetChildIndex(this->label1, 0);
			this->panel->Controls->SetChildIndex(this->tbMonto, 0);
			this->panel->Controls->SetChildIndex(this->label2, 0);
			this->panel->Controls->SetChildIndex(this->tbCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label3, 0);
			this->panel->Controls->SetChildIndex(this->tbDni, 0);
			this->panel->Controls->SetChildIndex(this->label4, 0);
			this->panel->Controls->SetChildIndex(this->lbReca, 0);
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
			this->panel->Controls->SetChildIndex(this->tbVenc, 0);
			this->panel->Controls->SetChildIndex(this->label6, 0);
			this->panel->Controls->SetChildIndex(this->tbNom, 0);
			this->panel->Controls->SetChildIndex(this->label7, 0);
			this->panel->Controls->SetChildIndex(this->tbCvv, 0);
			this->panel->Controls->SetChildIndex(this->lbCvv, 0);
			this->panel->Controls->SetChildIndex(this->lbCuotas, 0);
			this->panel->Controls->SetChildIndex(this->label9, 0);
			this->panel->Controls->SetChildIndex(this->lbMarca, 0);
			this->panel->Controls->SetChildIndex(this->lbTotal, 0);
			this->panel->Controls->SetChildIndex(this->lbFaltan, 0);
			this->panel->Controls->SetChildIndex(this->tbDummy, 0);
			this->panel->Controls->SetChildIndex(this->lbCardHelp, 0);
			this->panel->Controls->SetChildIndex(this->ult4Dig, 0);
			this->panel->Controls->SetChildIndex(this->label5, 0);
			this->panel->Controls->SetChildIndex(this->pnlPlan, 0);
			// 
			// cancel
			// 
			this->cancel->Location = System::Drawing::Point(466, 419);
			// 
			// btOk
			// 
			this->btOk->Location = System::Drawing::Point(466, 365);
			// 
			// Icono
			// 
			this->Icono->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Icono.Image")));
			this->Icono->Location = System::Drawing::Point(10, 3);
			this->Icono->Size = System::Drawing::Size(58, 46);
			// 
			// Titulo
			// 
			this->Titulo->ForeColor = System::Drawing::Color::Beige;
			this->Titulo->Location = System::Drawing::Point(74, 10);
			this->Titulo->Size = System::Drawing::Size(286, 23);
			this->Titulo->Text = L"Pago con Tarjetas de Crédito";
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
			this->label1->TabIndex = 49;
			this->label1->Text = L"Tarjeta:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// btBorrar
			// 
			this->btBorrar->BackColor = System::Drawing::Color::PaleGoldenrod;
			this->btBorrar->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btBorrar->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btBorrar.Image")));
			this->btBorrar->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->btBorrar->Location = System::Drawing::Point(466, 488);
			this->btBorrar->Name = L"btBorrar";
			this->btBorrar->Size = System::Drawing::Size(162, 32);
			this->btBorrar->TabIndex = 48;
			this->btBorrar->Text = L"[-] Borrar Renglón";
			this->btBorrar->UseVisualStyleBackColor = false;
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
			this->tbCard->TabIndex = 46;
			this->tbCard->Text = L"1234-1234-1234-1234";
			this->tbCard->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbCard->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbCard_Validating);
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
			this->lv->Items->AddRange(gcnew cli::array< System::Windows::Forms::ListViewItem^  >(1) {listViewItem1});
			this->lv->Location = System::Drawing::Point(22, 381);
			this->lv->MultiSelect = false;
			this->lv->Name = L"lv";
			this->lv->Size = System::Drawing::Size(406, 97);
			this->lv->TabIndex = 45;
			this->lv->UseCompatibleStateImageBehavior = false;
			this->lv->View = System::Windows::Forms::View::Details;
			this->lv->SelectedIndexChanged += gcnew System::EventHandler(this, &TarjOnlineForm::lv_SelectedIndexChanged);
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
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->ForeColor = System::Drawing::Color::Beige;
			this->label2->Location = System::Drawing::Point(64, 282);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(52, 14);
			this->label2->TabIndex = 51;
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
			this->tbMonto->TabIndex = 50;
			this->tbMonto->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->tbMonto->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbMonto->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbMonto_Validating);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->ForeColor = System::Drawing::Color::Beige;
			this->label3->Location = System::Drawing::Point(62, 305);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(54, 14);
			this->label3->TabIndex = 53;
			this->label3->Text = L"Cuotas:";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// tbCuotas
			// 
			this->tbCuotas->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbCuotas->Enabled = false;
			this->tbCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbCuotas->Location = System::Drawing::Point(140, 303);
			this->tbCuotas->Name = L"tbCuotas";
			this->tbCuotas->Size = System::Drawing::Size(29, 22);
			this->tbCuotas->TabIndex = 52;
			this->tbCuotas->Text = L"12";
			this->tbCuotas->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tbCuotas->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbCuotas->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbCuotas_Validating);
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
			this->label4->TabIndex = 55;
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
			this->tbDni->Name = L"tbDni";
			this->tbDni->Size = System::Drawing::Size(100, 22);
			this->tbDni->TabIndex = 54;
			this->tbDni->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbDni->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbDni_Validating);
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
			this->lbReca->TabIndex = 56;
			this->lbReca->Text = L"(Recargo $0.00)";
			this->lbReca->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->label6->TabIndex = 58;
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
			this->tbVenc->TabIndex = 57;
			this->tbVenc->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbVenc->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbVenc_Validating);
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
			this->label7->TabIndex = 60;
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
			this->tbNom->TabIndex = 59;
			this->tbNom->Text = L"Ricardo Federico Villafañes";
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
			this->lbCvv->TabIndex = 62;
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
			this->tbCvv->TabIndex = 61;
			this->tbCvv->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->tbCvv->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->tbCvv->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbCvv_Validating);
			// 
			// lbCuotas
			// 
			this->lbCuotas->AutoSize = true;
			this->lbCuotas->BackColor = System::Drawing::Color::Transparent;
			this->lbCuotas->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCuotas->ForeColor = System::Drawing::Color::Beige;
			this->lbCuotas->Location = System::Drawing::Point(186, 306);
			this->lbCuotas->Name = L"lbCuotas";
			this->lbCuotas->Size = System::Drawing::Size(111, 14);
			this->lbCuotas->TabIndex = 63;
			this->lbCuotas->Text = L"(1 a 12, 18 y 24)";
			this->lbCuotas->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->lbCardHelp->TabIndex = 64;
			this->lbCardHelp->Text = L"Pase la Tarjeta por el Lector de Banda para tomar sus Datos...";
			this->lbCardHelp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label9
			// 
			this->label9->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->label9->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label9->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(21, 477);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(407, 36);
			this->label9->TabIndex = 65;
			this->label9->Text = L"Use la Flecha Derecha para Moverse por la Lista. Pulse ENTER sobre \'Nueva...\' par" 
				L"a Ingresar una Nueva Tarjeta...";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
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
			this->lbMarca->TabIndex = 66;
			this->lbMarca->Text = L"Tarjeta:";
			this->lbMarca->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
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
			this->lbTotal->Location = System::Drawing::Point(466, 285);
			this->lbTotal->Name = L"lbTotal";
			this->lbTotal->Size = System::Drawing::Size(162, 64);
			this->lbTotal->TabIndex = 67;
			this->lbTotal->Text = L"Total: $150.25";
			this->lbTotal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbCuoAvail
			// 
			this->lbCuoAvail->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->lbCuoAvail->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lbCuoAvail->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCuoAvail->Location = System::Drawing::Point(21, 341);
			this->lbCuoAvail->Name = L"lbCuoAvail";
			this->lbCuoAvail->Size = System::Drawing::Size(406, 35);
			this->lbCuoAvail->TabIndex = 68;
			this->lbCuoAvail->Text = L"Use la Flecha Derecha para Moverse por la Lista. Pulse ENTER sobre \'Nueva...\' par" 
				L"a Ingresar una Nueva Tarjeta...";
			this->lbCuoAvail->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// lbFaltan
			// 
			this->lbFaltan->BackColor = System::Drawing::Color::Transparent;
			this->lbFaltan->Font = (gcnew System::Drawing::Font(L"Arial Black", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbFaltan->ForeColor = System::Drawing::Color::Crimson;
			this->lbFaltan->Location = System::Drawing::Point(441, 10);
			this->lbFaltan->Name = L"lbFaltan";
			this->lbFaltan->Size = System::Drawing::Size(187, 35);
			this->lbFaltan->TabIndex = 69;
			this->lbFaltan->Text = L"¡¡ Faltan Datos !!";
			this->lbFaltan->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// timerFaltan
			// 
			this->timerFaltan->Interval = 500;
			this->timerFaltan->Tick += gcnew System::EventHandler(this, &TarjOnlineForm::timerFaltan_Tick);
			// 
			// tbDummy
			// 
			this->tbDummy->Enabled = false;
			this->tbDummy->Location = System::Drawing::Point(323, 77);
			this->tbDummy->Name = L"tbDummy";
			this->tbDummy->Size = System::Drawing::Size(100, 20);
			this->tbDummy->TabIndex = 70;
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
			this->label5->TabIndex = 72;
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
			this->ult4Dig->TabIndex = 71;
			this->ult4Dig->Click += gcnew System::EventHandler(this, &TarjOnlineForm::AnyTbClick);
			this->ult4Dig->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::ult4Dig_Validating);
			// 
			// pnlPlan
			// 
			this->pnlPlan->Controls->Add(this->label10);
			this->pnlPlan->Controls->Add(this->tbPlan);
			this->pnlPlan->Controls->Add(this->label8);
			this->pnlPlan->Location = System::Drawing::Point(305, 301);
			this->pnlPlan->Name = L"pnlPlan";
			this->pnlPlan->Size = System::Drawing::Size(144, 35);
			this->pnlPlan->TabIndex = 73;
			this->pnlPlan->Visible = false;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::Color::Beige;
			this->label10->Location = System::Drawing::Point(62, 1);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(83, 28);
			this->label10->TabIndex = 2;
			this->label10->Text = L"1 - Comun\r\n2 - Nevaplan";
			// 
			// tbPlan
			// 
			this->tbPlan->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tbPlan->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->tbPlan->Location = System::Drawing::Point(37, 4);
			this->tbPlan->MaxLength = 1;
			this->tbPlan->Name = L"tbPlan";
			this->tbPlan->Size = System::Drawing::Size(22, 22);
			this->tbPlan->TabIndex = 1;
			this->tbPlan->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &TarjOnlineForm::tbPlan_Validating);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->BackColor = System::Drawing::Color::Transparent;
			this->label8->Font = (gcnew System::Drawing::Font(L"Tahoma", 9, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->ForeColor = System::Drawing::Color::Beige;
			this->label8->Location = System::Drawing::Point(0, 5);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(37, 14);
			this->label8->TabIndex = 0;
			this->label8->Text = L"Plan:";
			// 
			// TarjOnlineForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(659, 532);
			this->Name = L"TarjOnlineForm";
			this->Text = L"TarjOnlineForm";
			this->TopMost = false;
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &TarjOnlineForm::TarjOnlineForm_FormClosed);
			this->panel->ResumeLayout(false);
			this->panel->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Icono))->EndInit();
			this->pnlPlan->ResumeLayout(false);
			this->pnlPlan->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion







	private: System::Void TarjOnlineForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) 
			 {
				 Strings::releaseKeyboard(this);
			 }

			 // Procesa un numero de tarjeta.
			 void DecodeCard()
			 {
				 // Valida el numero de tarjeta.
				 if(strlen(actualTarj->Tarjeta) != 16)
					 STRCPY(actualTarj->Tarjeta, "");

				 // Busca el producto en los rangos disponibles.
				 if(strlen(actualTarj->Tarjeta) != 0)
				 {
					 TarjDef *def = BuscarDef(actualTarj->Tarjeta);
					 if(def == NULL)
					 {
						Alert("La tarjeta ingresada NO SE RECONOCE como una tarjeta",
							"que se acepte OnLine. Consulte con su Supervisor");
						STRCPY(actualTarj->Tarjeta, "");
					 }
					 else
					 {						
						actualTarj->Definicion = def;
						VerPlan();

						// Mira si hay lista restringida de medios de cobro posibles
						if(availPagos != NULL)
						{
							int i;
							for(i = 0; i < 20; i++)
							  if(availPagos[i] ==def->MPago)
									 break;
							if(i >= 20)
							{
								 Alert("La tarjeta ingresada NO SE PUEDE USAR para",
									 "Practipagos. Consulte con su Supervisor");
								 STRCPY(actualTarj->Tarjeta, "");
							}
						}
					 }
				 }

				 // Actualiza la form
				 if(strlen(actualTarj->Tarjeta) == 0)
				 {
					 actualTarj->Definicion = NULL;
					 actualTarj->Nombre[0] = 0;
				 }

				 Strings::StringToChar(cardAdicData, actualTarj->DatosBanda, 39);
				 
				 /*this->tbNom->Text = Strings::CharToString(actualTarj->Nombre);
				 this->tbCard->Text = Strings::CharToString(actualTarj->TarjetaFormateada());
				 this->lbMarca->Text = "(" + Strings::CharToString(actualTarj->Marca()) + ")";
				 this->lbCuoAvail->Text = Strings::CharToString(actualTarj->CuotasFormateadas());
				 this->tbCvv->Enabled = (actualTarj->Definicion != NULL && actualTarj->Definicion->PideCvv);*/
				 lv_SelectedIndexChanged(nullptr, nullptr);
				 GenValidacion();
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
				 BaseDialog::EndCardReader();
				 this->lbCardHelp->BackColor = Color::FromArgb(255, 255, 128);

				 if(cardAdicData->Length > 0 && cardNumber->Length > 0 && cardVenc > 0)
				 {
					 Strings::StringToChar(cardNumber, actualTarj->Tarjeta, 20);
					 Strings::StringToChar(cardHolder, actualTarj->Nombre, 40);
					 //Strings::StringToChar(serviceCode, actualTarj->CodigoServicio, 10);
					 DecodeCard();

					 this->tbVenc->Text = cardVenc.ToString();
					 globalTouchEvent = true;
					 tbVenc_Validating(nullptr, nullptr);
					 globalTouchEvent = false;
				 }
				 else
					 Alert("Tarjeta mal leida o Defectuosa.", "Pruebe de Nuevo...");
					 
				 this->tbDummy->Enabled = false;
				 SetFocusTo(tb);
			 }

			 public: virtual bool ProcessHotKey(Keys key) override
			 {
				 if(key == Keys::Right)
				 {
					 lv->SuspendLayout();
					 int selIndex = (lv->SelectedIndices->Count>0)? lv->SelectedIndices[0] + 1: 0;
					 lv->SelectedItems->Clear();
					 if(selIndex >= lv->Items->Count) selIndex = 0;
					 lv->Items[selIndex]->Selected = true;
					 lv->ResumeLayout();
					 return true;
				 }
				 return BaseDialog::ProcessHotKey(key);
			 }

			 // *********************************************************************************************
			 // ** Validacion y formateo de los textboxes

private: System::Void tbCard_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
			 char tbuff[30];

			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 String ^tbText = tbCard->Text->Trim();
				 if(tbText->Length == 0) return;
				 if(tbText->Length > 22) 
					 tbText = tbText->Substring(0, 22);
				 Strings::StringToChar(tbText, tbuff, 28);

				 // Copia solo los digitos numericos
				 char *ps, *pd;
				 for(ps = tbuff, pd = actualTarj->Tarjeta; *ps; ps++)
					 if(*ps >= '0' && *ps <= '9')
						 *pd++ = *ps;
				 *pd = 0;
			 }

			 // Valida el numero de tarjeta.
			 DecodeCard();
		 }

		 bool validatingVenc;

	private: System::Void tbVenc_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
			 char tbuff[30];

			 if(tbCard->Text->Trim()->Length == 0) return;
			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 if(validatingVenc) return;
				 validatingVenc = true;
			
				 String ^tbText = tbVenc->Text->Trim();
				 if(tbText->Length > 10)
					 tbText = tbText->Substring(0, 10);
				 Strings::StringToChar(tbText, tbuff, 20);

				 // Obtiene el año y mes minimos.
				 int minFecha = date() / 100;
				 int minMes = minFecha % 100;
				 int minAio = (minFecha / 100) - 2000;

				 // Convierte el mes en decimal.
				 actualTarj->Vencim = 0;
				 for(char *ps = tbuff; *ps; ps++)
					 if(*ps >= '0' && *ps <= '9')
						 actualTarj->Vencim = actualTarj->Vencim * 10 + (*ps - '0');

				 // Formatea como corresponde.
				 int mes = actualTarj->Vencim / 100, aio = actualTarj->Vencim % 100;
				 if(mes == 0 && aio == 0)
				 { 
					actualTarj->Vencim = 0;
				 }
				 else if((mes < 1 || mes > 12) || (aio < 8 && aio > 20))
				 {
					 Alert("Fecha de Vencimiento Invalida", "Trate de Nuevo...");
					 actualTarj->Vencim = 0;
				 }
				 else if(aio < minAio || (aio == minAio && mes < minMes))
				 {
					 Alert("La tarjeta esta Vencida", "Verifique los datos por favor...");
					 actualTarj->Vencim = 0;
				 }

				 sprintf(tbuff, "%02d-%02d", mes, aio);
				 tbVenc->Text = Strings::CharToString(tbuff);
				 validatingVenc = false;
				 GenValidacion();
			 }
		 }

private: System::Void tbMonto_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
 		    if(tbCard->Text->Trim()->Length == 0) return;
		    if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			{
				String ^tbText = this->tbMonto->Text->Trim();
				Decimal monto = 0;
				try { monto = Decimal::Parse(tbText); }
				catch(...) { monto = 0; }

				// El monto ingresado solo se acepta si es mayor que cero y, restada su porcion que
				// es recargo, no supera entonces el monto pendiente.
				if(monto < 0) monto = 0;
				actualTarj->Recargo = 0;
				Decimal tasa = Decimal(actualTarj->TasaRecargo / 100);
				Decimal recargo = 0;
				if(tasa != 0)
					recargo = Decimal::Round(monto - (monto / (1 + tasa)), 2);

				// Si el monto en tarjeta es mayor, entonces recalcula usando solamente el pendiente.
				if(monto - recargo > Decimal(pend))
				{
					recargo = Decimal::Round(Decimal(pend) * tasa, 2);
					monto = Decimal(pend) + recargo;
				}

				actualTarj->Monto = Decimal::ToDouble(monto);
				actualTarj->Recargo = Decimal::ToDouble(recargo);
				lv_SelectedIndexChanged(nullptr, nullptr);
				GenValidacion();
			}
		 }

private: System::Void tbCuotas_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
 			 if(tbCard->Text->Trim()->Length == 0) return;
			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 String ^tbText = this->tbCuotas->Text->Trim();
				 try { actualTarj->Cuotas = Int32::Parse(tbText); }
				 catch(...) { actualTarj->Cuotas = 1; }

				 double uMonto = actualTarj->Monto - actualTarj->Recargo;
				 actualTarj->Recargo = 0;
				 actualTarj->TasaRecargo = 0;
				 actualTarj->Monto = uMonto;
				 TarjCuota *cuo = actualTarj->DatosCuota();
				 if(cuo == NULL)
				 {
					 actualTarj->Cuotas = 0;
					 this->lbCuotas->Text = "";
					 Alert("El numero de cuotas indicado NO es Valido!!", "Ingreselo de nuevo...");
				 }
				 else
				 {
					 Decimal rec = Decimal(cuo->Porcen / 100 * uMonto);
					 actualTarj->Recargo = Decimal::ToDouble(Decimal::Round(rec, 2));
					 actualTarj->TasaRecargo = cuo->Porcen;
					 actualTarj->Monto = uMonto + actualTarj->Recargo;
				 }

				 lv_SelectedIndexChanged(nullptr, nullptr);
				 GenValidacion();
			 }
		 }

	private: System::Void tbDni_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
			 char temp[50];

			 if(tbCard->Text->Trim()->Length == 0) return;
			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 String ^tbText = this->tbDni->Text->Trim();
				 Strings::StringToChar(tbText, temp, 40);
				 actualTarj->Dni = 0;
				 for(char *ps = temp; *ps; ps++)
					 if(*ps >= '0' && *ps <= '9')
						 actualTarj->Dni = actualTarj->Dni * 10 + (*ps - '0');

				 if(actualTarj->Dni <= 99999 || actualTarj->Dni > 99999999)
				 {
					 actualTarj->Dni = 0;
					 Alert("Numero de Documento Invalido", "Pruebe de nuevo...");
				 }

				 this->tbDni->Text = Strings::CharToString(actualTarj->DniFormateado());
				 GenValidacion();
			 }
		 }

private: System::Void tbCvv_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
 			 if(tbCard->Text->Trim()->Length == 0) return;
			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 String ^tbText = this->tbCvv->Text->Trim();
				 actualTarj->Cvv = 0;
				 try { actualTarj->Cvv = Int32::Parse(tbText); }
				 catch(...) { actualTarj->Cvv = 0; }

				 if(!actualTarj->Definicion->CvvCero && (actualTarj->Cvv <= 0 || actualTarj->Cvv > 999))
				 {
					 actualTarj->Cvv = 0;
					 Alert("El CVV ingresado no es valido (Debe ser entre 1 y 999)", "Pruebe de Nuevo...");
				 }

				 this->tbCvv->Text = actualTarj->Cvv.ToString();
				 GenValidacion();
			 }
		 }

private: System::Void ult4Dig_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
		 {
			 if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
			 {
				 if(tbCard->Text->Trim()->Length == 0) return;
				 String ^tbText = this->ult4Dig->Text->Trim();
				 actualTarj->ult4Digitos = 0;
				 try { actualTarj->ult4Digitos = Int32::Parse(tbText); }
				 catch(...) { actualTarj->ult4Digitos = 0; }

				 if(actualTarj->LastDigits() != actualTarj->ult4Digitos)
					 Alert("Los ultimos 4 digitos NO COINCIDEN. Revise con cuidado", "la tarjeta y pruebe de nuevo...");
				 GenValidacion();
			 }
		 }

protected: 
		 virtual void btOk_Click(Object ^sender, EventArgs ^e) override
		 {
			 if(cardData == nullptr || actualTarj == NULL) return;
			 Strings::StringToChar(cardData->ToString(), actualTarj->CrudeCardData, 127);
			 Strings::StringToChar(track_1, actualTarj->Track_1, 80);
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

protected: virtual void OnLoad(EventArgs ^e) override
		 {
			 lv_SelectedIndexChanged(nullptr, nullptr);
			 BaseDialog::OnLoad(e);
		 }

private: System::Void AnyTbClick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(pantalla != 1 && pantalla != 3) SetFocusTo((Control ^)sender);
		 }

private: System::Void txtSoloNumeros_KeyPress(System::Object^ sender, KeyPressEventArgs^ e)
        {
				if (Char::IsLetter(e->KeyChar))
				{
					 e->Handled = true;
				}
				else if (Char::IsDigit(e->KeyChar))
				{
					 e->Handled = false;
				}
				else if (Char::IsControl(e->KeyChar))
            {
                e->Handled = false;
            }
            else
            {
                e->Handled = true;
            }
        }

private: System::Void VerPlan()
			{
				if (actualTarj != NULL && actualTarj->Definicion != NULL && strcmp(strupr(actualTarj->Definicion->Nombre), "NEVADA") == 0)
				{
					pnlPlan->Visible = true;
					//DummyProvider ^provider = gcnew DummyProvider();						
					int valor = Convert::ToInt32(actualTarj->Plan);
					valor = valor > 9 ? valor - 48 : valor;
					tbPlan->Text = valor.ToString();					
					tbPlan->Enabled = true;
				}
				else if (actualTarj != NULL && actualTarj->Definicion != NULL && strcmp(strupr(actualTarj->Definicion->Nombre), "TARJETA SHOPPING") == 0)
				{
					actualTarj->Plan = '1';
				}
				else
				{
					pnlPlan->Visible = false;
					tbPlan->Text = "0";
					tbPlan->Enabled = false;
				}
			}

	/*private: System::Void btnTarjeta_Click(System::Object^  sender, System::EventArgs^  e) 
				{
					this->cardData = gcnew StringBuilder("%B5046395423719710&RAMIREZ JOSE ALFREDO      &21091213280000000000_ñ5046395423719710¡21091213280000000000_");
					this->EndCardReader();
				}*/
private: System::Void tbPlan_Validating(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) 
			{
				if((pantalla == 1 || pantalla == 3 || pantalla == 5) || globalTouchEvent)
				 {
					 if(tbPlan->Text->Trim()->Length == 0) return;
					 String ^tbText = this->tbPlan->Text->Trim();
					 actualTarj->Plan = '0';
					 int valor = 0;
					 try { 
						valor = Int32::Parse(tbText); 
						actualTarj->Plan = valor > 2? '0' : (char) Convert::ToChar(tbText);					 
					 }
					 catch(...) { actualTarj->Plan = '0'; }

					 if(valor > 2)
						 Alert("El plan seleccionado no existe", "Ingrese otro numero...");
					 GenValidacion();
				 }
			}
private: System::Void btnTarjet_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				//String ^cadena = "%B4517650566121018^RAMIREZ/JOSE ALFREDO     ^150712110000        00496000000?;4517650566121018=15071214960000000000?";
				 //String ^cadena = "%B5043630003033923^RAMIREZ/JOSE ALFREDO     ^140210118911        00992000000?;5043630003033923=14021019920000000000?";				
				 //String ^cadena = "%B6034885423719710&RAMIREZ JOSE ALFREDO      &21091213280000000000_ñ6034885423719710¡21091213280000000000_";
				 //String ^cadena = "%B5254220053144024&HERRERA ARIEL HECTOR      &1306101071749651307803544_ñ5254220053144024¡13061010717496513078_";
				 //String ^cadena = "%B5465530779428997&ACUÁ VICTOR RUBEN        &1608101050718192152808098_ñ5465530779428997¡16081010507181921528_";
				 String ^cadena = "%B6042010593291529^RAMIREZ/JOSE ALFREDO     ^180412110000        00800000000?;6042010593291529=18041218000000000000?";
				 this->cardData = gcnew System::Text::StringBuilder();
				 this->cardData->Append(cadena);
				 this->EndCardReader();
			}

//private: System::Void btnTarj2_Click(System::Object^  sender, System::EventArgs^  e) 
//			{
//				//String ^cadena = "%B4517650566121018^RAMIREZ/JOSE ALFREDO     ^150712110000        00496000000?;4517650566121018=15071214960000000000?";
//				String ^cadena = "%B5043630566121018^RAMIREZ/JOSE ALFREDO     ^150712110000        00496000000?;5043630566121018=15071214960000000000?";
//				 //String ^cadena = "%B5043630003033923^RAMIREZ/JOSE ALFREDO     ^140210118911        00992000000?;5043630003033923=14021019920000000000?";				
//				 //String ^cadena = "%B5254220053144024&HERRERA ARIEL HECTOR      &1206101071749651307803544_ñ5254220053144024¡12061010717496513078_";
//				 this->cardData = gcnew System::Text::StringBuilder();
//				 this->cardData->Append(cadena);
//				 this->EndCardReader();
//			}


};
}
#endif
