#pragma once
#ifdef POSXP

#include "gcstatic.h"
#include "PosConsole.h"


using namespace System;
using namespace System::Reflection;
using namespace System::Text;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace pos;
using namespace Pos::Desktop;

#include "Strings.h"
#include "MuestraForm.h"
#include "frmTouchInicial.h"

#undef GetEnvironmentVariable

extern int keyupper, keydown;
extern char *logo_Main;

// Convierte la consola a una form comun
void PrepareConsoleForm();

namespace pos {

	/// <summary>
	/// Resumen de MainForm
	///
	/// ADVERTENCIA: si cambia el nombre de esta clase, deberá cambiar la
	///          propiedad 'Nombre de archivos de recursos' de la herramienta de compilación de recursos administrados
	///          asociada con todos los archivos .resx de los que depende esta clase. De lo contrario,
	///          los diseñadores no podrán interactuar correctamente con los
	///          recursos adaptados asociados con este formulario.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	private:
		int logoPointer;				// El numero del siguiente logo a mostrar.
		List<Image ^> ^imgLogos;			// La lista de logos cargados
		

		delegate void InvokeGridRefresh(void);
		delegate void InvokeClose();
		delegate void InvokeActualizeTermo(String ^msg, int percent);
		delegate void InvokeFormSetter(String ^msg);
		delegate void InvokeFormSetter4(String ^msg1, String ^msg2, String ^msg3, String ^msg4, int cnt);
		delegate void InvokeSetCaptions(int cons, int tf, int rm, String ^lista, int ckenab,
			String ^ckNom, int vendEnab, String ^vendNom, int rep, int net);

		delegate void InvokeSetPeso(void);
		delegate void InvokeClearPeso(void);


		InvokeGridRefresh ^doRefreshGrid;
		InvokeClose ^doCloseDelegate;
		InvokeActualizeTermo ^actualizeTermo;
		InvokeFormSetter ^numDisplaySetter;
		InvokeFormSetter ^multiDisplaySetter;
		InvokeFormSetter4 ^totDisplaySetter;
		InvokeSetCaptions ^captionsSetter;

		InvokeSetPeso ^doSetPeso;
		InvokeClearPeso ^doClearPeso;

		StringBuilder ^leoPesoSystel;

	static StringBuilder ^sb = gcnew StringBuilder();

	public:
		DataGridView^  Grid;

	private:
		String ^txtNum, ^txtMulti, ^txtTot, ^txtCobrar, ^txtSal, ^txtCC;
		Label^  lbVersion;
		ToolStripStatusLabel^  btEspera;
		ProgressBar^  termo;
		Label^  termoMsg;
		Panel^  termoPanel;
		Panel^  publicidad;
		PictureBox^  publiFoto;
		System::Windows::Forms::Timer ^timerHora, ^timerLogo, ^completeTimer, ^perfTimer;
		Label^  lbCaja;
		Label^  visorTot;
		Label^  m1;
		Label^  m2;
		Label^  m3;
		ToolStripStatusLabel^  btCajero;
		Button^  arriba;
		Button^  abajo;
		NumKbd^  numKbd;
		VentaKbd^  ventaKbd;
		PictureBox^  logoBitnet;
		Label^  lbBitnetwww;
		Label^  visorMulti;
		Label^  visorCnt;
		DataGridViewTextBoxColumn^  Column1;
		DataGridViewTextBoxColumn^  Column2;
		DataGridViewTextBoxColumn^  Column3;
		StatusStrip^  statusStrip1;
		ToolStripStatusLabel^  btModo;
		ToolStripStatusLabel^  btRed;
		ToolStripStatusLabel^  btConsulta;
		ToolStripStatusLabel^  btTF;
		ToolStripStatusLabel^  btLista;
		ToolStripStatusLabel^  btVendedor;
	private: System::Windows::Forms::Button^  btnMuestraPromos;

	private: System::IO::Ports::SerialPort^  spBalan;
	private: System::Windows::Forms::Panel^  pnlBalanza;

	private: System::Windows::Forms::Label^  lPeso;
	private: System::Windows::Forms::Label^  lbLimFiscal;

	private: System::Windows::Forms::Timer^  tmrTouch;
	private: System::IO::Ports::SerialPort^  spControlBal;
	private: Pos::Desktop::VentaTouchKbd^  vtaTouchKbd;

	










		int txtCnt;
		

		void RedoRefresh()
		{
			perfTimer->Interval = 100;
			if(!perfTimer->Enabled)
			{
				//perfTimer->Start();
				perfTimer->Enabled = true;
				//WriteBootLog("Inicia el refresh");
			}
			GCStatic::ReleaseMainForm("avoid-refresh");
		}
		
		void perfTimer_Tick(System::Object^  sender, System::EventArgs^  e) 
		{			
			GCStatic::LockMainForm("perfomance-timer");
			this->SuspendLayout();

			// Visor de numeros tecleados
			if(txtNum != nullptr && txtNum != visorDer->Text)
			{
				int rPos = visorDer->Right;
				visorDer->Text = txtNum;
				visorDer->Left += (rPos - visorDer->Right);
			}

			// Visor de multiplicacion
			if(txtMulti != nullptr && txtMulti != visorMulti->Text)
				visorMulti->Text = txtMulti;

			// Visores de total
			if(txtTot != nullptr && txtTot != visorTot->Text)
			{
				int rPos = visorTot->Right;
				visorTot->Text = txtTot;
			}

			if(txtCC != nullptr && txtCC != m1->Text) 
			{
				//WriteBootLog("Pasa por Cliente aqui2");
				m1->Text = txtCC;
			}
			if(txtCobrar != nullptr && txtCobrar != m2->Text) 
				m2->Text = txtCobrar;
			if(txtSal != nullptr && txtSal != m3->Text) 
				m3->Text = txtSal;
			if(txtCnt != 0)
			{
				String ^txt = String::Format("Artículos: {0}", txtCnt);
				if(!visorCnt->Visible || visorCnt->Text != txt)
				{
					this->visorCnt->Visible = true;
					this->visorCnt->Text = txt;
				}
			}
			else
				this->visorCnt->Visible = false;

			this->ResumeLayout();
			GCStatic::ReleaseMainForm("perfomance-timer");
		}

		/*static String ^PrepareBuffer(char *data)
		{
			sb->Length = 0;
			while(*data)
				sb->Append((wchar_t) *data++);
			return sb->ToString();
		}*/

		void DoRefreshGrid()
		{
			GCStatic::LockMainForm("avoid-refresh");
			Grid->Refresh();
			GCStatic::ReleaseMainForm("avoid-refresh");
		}

		void DoSetMultiDisplay(String ^msg)
		{
			GCStatic::LockMainForm("avoid-refresh");
			perfTimer->Enabled = false;
			txtMulti = msg;
			RedoRefresh();
		}

		void DoActualizeTermo(String ^msg, int percent)
		{
			if(percent < 0)
				termoPanel->Visible = false;
			else
			{
				if(termoMsg->Text != msg) termoMsg->Text = msg;
				if(termo->Value != percent) termo->Value = percent;
				if(!termoPanel->Visible) termoPanel->Visible = true;
			}
		}

		void DoSetNumDisplay(String ^msg)
		{
			GCStatic::LockMainForm("avoid-refresh");
			perfTimer->Enabled = false;
			txtNum = msg;
			if (txtNum->Trim()->Length == 13 && (txtNum->Trim()->StartsWith("9931") || txtNum->Trim()->StartsWith("9932") || txtNum->Trim()->StartsWith("9933")))
				txtNum = "*************";
			RedoRefresh();
		}

		void DoSetTotDisplay(String ^msgTot, String ^msgCobrar, String ^msgSal, String ^msgCC, int cnt)
		{			
			GCStatic::LockMainForm("avoid-refresh");
			perfTimer->Enabled = false;
			txtTot = msgTot;
			txtCobrar = msgCobrar;
			txtSal = msgSal;
			txtCC = msgCC;
			txtCnt = cnt;
			RedoRefresh();
		}
		
		System::String^ leyendaTicket(){
			String^ tleyen = "";
			if (inOper > 0){
				if (pf->TipoComprobante == 83)
					tleyen = "TI: " + System::Convert::ToString(globalDelayPrint ? pf->NumeroComprobante + 1 : pf->NumeroComprobante);
				else if (pf->TipoComprobante == 82)
					tleyen = "FB: " + System::Convert::ToString(globalDelayPrint ? pf->NumeroComprobante + 1 : pf->NumeroComprobante);
				else if (pf->TipoComprobante == 81)
					tleyen = "FA: " + System::Convert::ToString(globalDelayPrint ? pf->NumeroComprobante + 1 : pf->NumeroComprobante);
			}
			return tleyen;
		}

		void DoSetCaptions(int cons, int tf, int rm, String ^lista, int ckenab,
			String ^ckNom, int vendEnab, String ^vendNom, int rep, int net)
		{
			GCStatic::LockMainForm("avoid-refresh");
			perfTimer->Enabled = false;
			this->btModo->Text = 
				(rm == 0)? "Modo REG" :
				(rm == 1)? "Modo X1" :
				(rm == 2)? "Modo X2" :
				"Modo ZETA";
			
			this->btConsulta->Text = cons? "Consulta": String::Empty;
			//this->btTF->Text = tf? "T/Factura": String::Empty;
			this->btTF->Text = this->leyendaTicket();
			this->btModo->BackColor = (rm == 0)? System::Drawing::SystemColors::Control: System::Drawing::Color::Yellow;
			this->btModo->ForeColor = (rm == 0)? System::Drawing::Color::Black: System::Drawing::Color::Blue;
			this->btLista->Text = lista;
			this->btCajero->Text = ckenab? ckNom: String::Empty;
			this->btEspera->Text = enEspera? "Ticket Espera": String::Empty;
			//this->btRepa->Text = (rep != 0)? String::Format("Reparto: {0}", rep): String::Empty;

			if(net)
			{
				this->btRed->Text = "OnLine";
				this->btRed->BackColor = System::Drawing::SystemColors::Control;
				this->btRed->ForeColor = System::Drawing::Color::Black;
			}
			else
			{
				this->btRed->Text = "OffLine";
				this->btRed->BackColor == System::Drawing::Color::Red;
				this->btRed->ForeColor == System::Drawing::Color::White;
			}

			RedoRefresh();
		}

		void DoBorraPeso()
		{
			pesoLeido = 0;
			pnlBalanza->Visible = false;
			lPeso->Visible = false;
		}

		void DoSetPeso()
		{			
			if (pesoLeido > 0)
			{
				pnlBalanza->Visible = true;				
				//pbBalanza->Visible = true;
				pnlBalanza->BackColor = System::Drawing::Color::Transparent;
				lPeso->Visible = true;
			}
			else			
			{
				pnlBalanza->Visible = false;
				//pbBalanza->Visible = false;
				lPeso->Visible = false;
			}
			lPeso->Text = String::Format("{0:0.000} Kg", pesoLeido);				
		}

	public: void ActualizarPeso()
		{			
			if(this->InvokeRequired)
				Invoke(doSetPeso);
			else
				DoSetPeso();
		}
		void BorrarPeso()
		{
			if (this->InvokeRequired)
				Invoke(doClearPeso);
			else
				DoBorraPeso();
		}

	public:
		static MainForm ^F;

		void SetCaptions(
			int consmode,			/*  Si estamos en modo de consulta   */
			int GlobalTF,			/*  Si estamos emitiendo ticket factura   */
			int rMode,				/*  El modo actual de registracion   */
			char *NameLista,		/*  El nombre de la lista de precios    */
			int ckenab,				/*  Si está habilitado el cajero   */
			char *clkName,			/*  El nombre del cajero   */
			int ven_enab,			/*  Si está habilitado el vendedor   */
			char *venName,			/*  El nombre del vendedor   */
			int reparto_nro,		/*  Si está habilitado, el número de reparto actual  */
			int networkAvailable)	/*  1 si esta la red activa   */
		{
			String ^nLista = gcnew String(NameLista);	//PrepareBuffer(NameLista);
			String ^ckName = gcnew String(clkName);		//PrepareBuffer(clkName);
			String ^vName = gcnew String(venName);		//PrepareBuffer(venName);

			if(this->InvokeRequired)
				Invoke(captionsSetter, consmode, GlobalTF, rMode, nLista, ckenab, ckName, ven_enab, vName, reparto_nro, networkAvailable);
			else
				DoSetCaptions(consmode, GlobalTF, rMode, nLista, ckenab, ckName, ven_enab, vName, reparto_nro, networkAvailable);

			// Algunos mensajes...
			timerHora_Tick(nullptr, nullptr);
			lbCaja->Text = "Caja Nro: " + caja.ToString() + 
				" (" + Strings::CharToString(pf->PuntoDeVenta()) + ")";
			lbLimFiscal->Text = System::String::Format("Lim.Fact.A/B: ${0:0.00}",limTicket);
		}

		// Refresca la grilla
		void RefreshGrid()
		{
			if(this->InvokeRequired)
				Invoke(doRefreshGrid);
			else
				DoRefreshGrid();
		}

		// Necesitamos cerrar la ventana desde otra thread...
		bool closed;
		void DoClose()
		{
			if(closed) return;
			if(this->InvokeRequired)
				Invoke(doCloseDelegate);
			else
				this->Close();
		}

		void SetMultiDisplay(char *rng)
		{
			//String ^msg = PrepareBuffer(rng);
			String ^msg = gcnew String(rng);
			if(this->InvokeRequired)
				Invoke(multiDisplaySetter, msg);
			else
				DoSetMultiDisplay(msg);
		}

		void SetNumDisplay(char *rng)
		{
			//String ^msg = PrepareBuffer(rng);
			String ^msg = gcnew String(rng);
			if(this->InvokeRequired)
				Invoke(numDisplaySetter, msg);
			else
				DoSetNumDisplay(msg);
		}

		void SetTotDisplay(char *tot, char *cobrar, char *sal, char *cc, int cnt)
		{
			//char mensaje[100];
			String ^msgTot = (tot == NULL)? nullptr: gcnew String(tot);// PrepareBuffer(tot);
			String ^msgCobrar =(cobrar == NULL)? nullptr: gcnew String(cobrar); // PrepareBuffer(cobrar);
			String ^msgSal = (sal == NULL)? nullptr: gcnew  String(sal); // PrepareBuffer(sal);
			String ^msgCC = (cc == NULL)? nullptr: gcnew String(cc); // PrepareBuffer(cc);
			//sprintf(mensaje, "cliente: %s, saldo: %s", cc, sal);
			//WriteBootLog(mensaje);
			if(this->InvokeRequired)
				Invoke(totDisplaySetter, msgTot, msgCobrar, msgSal, msgCC, cnt);
			else
				DoSetTotDisplay(msgTot, msgCobrar, msgSal, msgCC, cnt);
		}

		void ActualizeTermo(char *msg, int percent)
		{
			//String ^netMsg = PrepareBuffer(msg);
			String ^netMsg = gcnew String(msg);
			if(this->InvokeRequired)
				Invoke(actualizeTermo, netMsg, percent);
			else
				DoActualizeTermo(netMsg, percent);
		}

		int tipoForm;

		MainForm(int tipo)
		{
			// Inicializacion para el WindowForm's designer
			F = this;
			InitializeComponent();			
			imgLogos = gcnew List<Image ^>();
			closed = false;

			leoPesoSystel = gcnew StringBuilder();

			// La 'MainForm' NUNCA debe tener el 'TopMost' en true, porque sino automaticamente dejan de
			// valer los 'TopMost' de las otras ventanas...
			this->TopMost = false;

			// Construye los delegados necesarios para llamar a metodos de esta form desde
			// cualquier thread, porque el POS hace uso intensivo del multithreading.
			doRefreshGrid = gcnew InvokeGridRefresh(this, &MainForm::DoRefreshGrid);
			doCloseDelegate = gcnew InvokeClose(this, &MainForm::Close);
			numDisplaySetter = gcnew InvokeFormSetter(this, &MainForm::DoSetNumDisplay);
			totDisplaySetter = gcnew InvokeFormSetter4(this, &MainForm::DoSetTotDisplay);
			captionsSetter = gcnew InvokeSetCaptions(this, &MainForm::DoSetCaptions);
			multiDisplaySetter = gcnew InvokeFormSetter(this, &MainForm::DoSetMultiDisplay);
			actualizeTermo = gcnew InvokeActualizeTermo(this, &MainForm::DoActualizeTermo);

			doSetPeso = gcnew InvokeSetPeso(this, &MainForm::DoSetPeso);
			doClearPeso = gcnew InvokeClearPeso(this, &MainForm::DoBorraPeso);
			Strings::InterceptKeyboard(this, 0);

			// Personaliza la form segun el equipamiento host en la que se ejecuta.
			int corriM1 = this->m1->Height + 3;
			tipoForm = tipo;
			System::Drawing::Font ^font1 = gcnew System::Drawing::Font("Tahoma", 20, FontStyle::Bold);
			System::Drawing::Font ^font2 = gcnew System::Drawing::Font("Tahoma", 30, FontStyle::Bold);
			System::Drawing::Font ^font3 = gcnew System::Drawing::Font("Tahoma", 18, FontStyle::Bold);
			this->vtaTouchKbd->Visible = false;
			switch(tipo)
			{
				// Touch 800x600 - Default
				case 0:
					break;

				// No Touch - 1024 x 768
				case 1:
					this->Width = 1022;
					this->Height = 766;
					this->numKbd->Visible = false;
					this->ventaKbd->Visible = false;
					
					this->visorDer->Font = font1;
					this->visorDer->Left += 1020 - this->visorDer->Right;
					this->visorDer->Top += 10;
					
					this->visorMulti->Left += 65;
					this->visorMulti->Font = font1;
					this->visorMulti->Top += 10;
					this->visorMulti->Height += 8;
					this->visorMulti->Width += 20;

					this->pnlBalanza->Left += 50;
					this->pnlBalanza->Top += 10;
					this->pnlBalanza->Width += 30;
					this->pnlBalanza->Height += 15;

					this->visorTot->Height += 12;
					this->visorTot->Font = font2;
					this->visorTot->Top += 15;
					this->visorTot->Left += 65;
					
					this->visorCnt->Height += 8;
					this->visorCnt->Font = font3;
					this->visorCnt->Top += 20;
					this->visorCnt->Left += 1020 - this->visorCnt->Right;
					
					this->Grid->Top += 40;
					this->Grid->Width += (1020 - Grid->Right);
					this->Grid->Height += (740 - Grid->Bottom);
					this->publicidad->Top = statusStrip1->Top - this->publicidad->Height - 4;
					this->publicidad->Left = 2;
					this->publicidad->Visible = true;

					this->m1->Top += corriM1 * 2;
					this->m2->Top += corriM1 * 2;
					this->m3->Top += corriM1 * 2;

					break;

				case 2:			// Touch 1024 * 768
					this->Width = 1022;
					this->Height = 766;
					
					this->visorDer->Font = font1;
					this->visorDer->Left += 1020 - this->visorDer->Right;
					this->visorDer->Top += 10;
					
					this->visorMulti->Left += 65;
					this->visorMulti->Font = font1;
					this->visorMulti->Top += 10;

					this->pnlBalanza->Left += 50;
					this->pnlBalanza->Top += 10;
					this->pnlBalanza->Width += 30;
					this->pnlBalanza->Height += 15;

					this->visorTot->Height += 12;
					this->visorTot->Font = font2;
					this->visorTot->Top += 15;
					this->visorTot->Left += 65;
					
					this->visorCnt->Height += 8;
					this->visorCnt->Font = font3;
					this->visorCnt->Top += 20;
					this->visorCnt->Left += 1020 - this->visorCnt->Right;
					
					this->Grid->Top += 50;
					this->Grid->Height += 130;
					this->publicidad->Top = this->Grid->Top;
					this->publicidad->Left += 1020 - this->publicidad->Right;
					this->publicidad->Height = this->Grid->Height;
					this->publicidad->Visible = true;
					this->Grid->Left -= 64;
					this->arriba->Left -= 64;
					this->abajo->Left -= 64;


					this->numKbd->Left = 2;
					this->ventaKbd->Left = 2;
					this->ventaKbd->Top += 745 - this->ventaKbd->Bottom;
					this->numKbd->Top += this->ventaKbd->Top - this->numKbd->Bottom - 2;
					this->termoPanel->Left = 2;
					this->termoPanel->Top = this->numKbd->Top - this->termoPanel->Height - 2;

					this->m1->Left = this->ventaKbd->Right + 20;
					this->m2->Left = this->ventaKbd->Right + 20;
					this->m3->Left = this->ventaKbd->Right + 20;
					this->m1->Top += 460;
					this->m2->Top += 460;
					this->m3->Top += 460;

					break;

				case 3:			// No touch - monitor B/W - 800 x 600
					this->numKbd->Visible = false;
					this->ventaKbd->Visible = false;
					this->publicidad->Visible = false;
					this->arriba->Visible = false;
					this->abajo->Visible = false;

					this->m1->Left = this->Grid->Left;
					this->m1->Top = this->Grid->Top;
					this->m2->Left = this->Grid->Left;
					this->m2->Top = this->m1->Bottom + 2;
					this->m3->Left = this->m1->Left + 200;
					this->m3->Top = this->m2->Top;

					//this->pnlBalanza->Left += 50;
					//this->pnlBalanza->Top += 10;
					//this->pnlBalanza->Width += 30;
					//this->pnlBalanza->Height += 15;

					this->Grid->Left = 2;
					this->Grid->Width = 796;
					this->Grid->Height += 140;
					this->Grid->Top += this->statusStrip1->Top - this->Grid->Bottom  - 4;
					for(int i = this->Grid->Columns->Count - 1; i >= 0; i--)
					{
						DataGridViewColumn ^col = this->Grid->Columns[i];
						col->Width = (int)((double)col->Width * 1.6);
					}
					break;

				case 4:			// Touch 1280 x 720
					this->Width = 1280;
					this->Height = 720;
					this->numKbd->Visible = false;
					this->ventaKbd->Visible = false;

					this->logoBitnet->Left += 45;
					this->lbBitnetwww->Left += 45;
					this->lbCaja->Left += 45;
					this->lbVersion->Left += 45;

					this->visorDer->Font = font1;
					this->visorDer->Left += 1280 - this->visorDer->Right;
					this->visorDer->Top += 10;
					
					this->visorMulti->Left += 160;
					this->visorMulti->Font = font1;
					this->visorMulti->Top += 5;
					this->visorMulti->Height += 8;
					this->visorMulti->Width += 20;
					
					this->pnlBalanza->Left += 110;
					this->pnlBalanza->Top += 10;
					this->pnlBalanza->Width += 60;
					this->pnlBalanza->Height += 10;

					this->visorTot->Height += 12;
					this->visorTot->Font = font2;
					this->visorTot->Top += 15;
					this->visorTot->Left = this->visorMulti->Left;
					
					this->visorCnt->Height += 8;
					this->visorCnt->Font = font3;
					this->visorCnt->Top += 10;
					this->visorCnt->Left += 1280 - this->visorCnt->Left;
					
					this->Grid->Top += 40;
					this->Grid->Width += (1280 - Grid->Right);
					this->Grid->Height += (690 - Grid->Bottom);
					this->Grid->RowTemplate->Height = 36;
					this->Grid->Left += 60;
					this->Grid->Width -= 66;
					this->Grid->BackgroundColor = System::Drawing::Color::DarkSeaGreen;
			
					this->publicidad->Top = statusStrip1->Top - this->publicidad->Height - 4;
					this->publicidad->Left = 2;
					this->publicidad->Visible = true;
					break;

				case 5:
					this->Width = 1022;
					this->Height = 766;
					this->numKbd->Visible = false;
					this->ventaKbd->Visible = false;

					this->visorDer->Font = font1;
					this->visorDer->Left += 1020 - this->visorDer->Right;
					this->visorDer->Top += 10;

					this->visorMulti->Left += 65;
					this->visorMulti->Font = font1;
					this->visorMulti->Top += 10;
					this->visorMulti->Height += 8;
					this->visorMulti->Width += 20;

					this->pnlBalanza->Left += 50;
					this->pnlBalanza->Top += 10;
					this->pnlBalanza->Width += 30;
					this->pnlBalanza->Height += 15;

					this->visorTot->Height += 12;
					this->visorTot->Font = font2;
					this->visorTot->Top += 15;
					this->visorTot->Left += 65;

					this->visorCnt->Height += 8;
					this->visorCnt->Font = font3;
					this->visorCnt->Top += 20;
					this->visorCnt->Left += 1020 - this->visorCnt->Right;

					this->Grid->Top += 40;
					this->Grid->Width += (1020 - Grid->Right);
					this->Grid->Height += (740 - Grid->Bottom);
					this->publicidad->Top = statusStrip1->Top - this->publicidad->Height - 4;
					this->publicidad->Left = 2;
					this->publicidad->Visible = true;

					this->m1->Top += corriM1 * 2;
					this->m2->Top += corriM1 * 2;
					this->m3->Top += corriM1 * 2;
					break;

				case 6:			// Touch 800 * 600
					//this->Width = 1022;
					//this->Height = 766;

					//this->visorDer->Font = font1;
					//this->visorDer->Left += 1020 - this->visorDer->Right;
					//this->visorDer->Top += 10;

					//this->visorMulti->Left += 65;
					//this->visorMulti->Font = font1;
					//this->visorMulti->Top += 10;

					//this->pnlBalanza->Left += 50;
					//this->pnlBalanza->Top += 10;
					//this->pnlBalanza->Width += 30;
					//this->pnlBalanza->Height += 15;

					//this->visorTot->Height += 12;
					//this->visorTot->Font = font2;
					//this->visorTot->Top += 15;
					//this->visorTot->Left += 65;

					//this->visorCnt->Height += 8;
					//this->visorCnt->Font = font3;
					//this->visorCnt->Top += 20;
					//this->visorCnt->Left += 1020 - this->visorCnt->Right;

					//this->Grid->Top += 50;
					//this->Grid->Height += 130;
					//this->publicidad->Top = this->Grid->Top;
					//this->publicidad->Left += 1020 - this->publicidad->Right;
					//this->publicidad->Height = this->Grid->Height;
					this->publicidad->Visible = false;
					//this->Grid->Left -= 64;
					//this->arriba->Left -= 64;
					//this->abajo->Left -= 64;


					//this->numKbd->Left = 2;
					//this->ventaKbd->Left = 2;
					//this->ventaKbd->Top += 745 - this->ventaKbd->Bottom;
					//this->numKbd->Top += this->ventaKbd->Top - this->numKbd->Bottom - 2;
					//this->termoPanel->Left = 2;
					//this->termoPanel->Top = this->numKbd->Top - this->termoPanel->Height - 2;

					//this->m1->Left = this->ventaKbd->Right + 20;
					//this->m2->Left = this->ventaKbd->Right + 20;
					//this->m3->Left = this->ventaKbd->Right + 20;
					//this->m1->Top += 460;
					//this->m2->Top += 460;
					//this->m3->Top += 460;

					break;
				case 7://Touch 1024 x 768
					logoBitnet->Visible = false;
					lbBitnetwww->Visible = false;
					lbVersion->Visible = false;
					lbCaja->Visible = false;
					lbLimFiscal->Visible = false;
					
					System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
					this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Comodin.Self")));
					this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;

					this->Width = 1022;
					this->Height = 766;

					this->visorDer->Font = font1;
					this->visorDer->Left += 1020 - this->visorDer->Right;
					this->visorDer->Top += 10;

					this->visorMulti->Left += 65;
					this->visorMulti->Font = font1;
					this->visorMulti->Top += 10;

					this->pnlBalanza->Left += 50;
					this->pnlBalanza->Top += 10;
					this->pnlBalanza->Width += 30;
					this->pnlBalanza->Height += 15;

					this->visorTot->Height += 12;
					this->visorTot->Font = font2;
					this->visorTot->Top += 15;
					this->visorTot->Left += 65;

					this->visorCnt->Height += 8;
					this->visorCnt->Font = font3;
					this->visorCnt->Top += 20;
					this->visorCnt->Left += 1020 - this->visorCnt->Right;

					this->Grid->Top += 50;
					this->Grid->Left = 320; //Agregado
					this->Grid->Height += (740 - Grid->Bottom);
					this->Grid->Width += (1020 - Grid->Right + 70); //Agregado
					/*this->publicidad->Top = this->Grid->Top;
					this->publicidad->Left += 1020 - this->publicidad->Right;
					this->publicidad->Height = this->Grid->Height;*/
					this->publicidad->Visible = false;
					this->arriba->Left += 40;
					this->abajo->Left += 40;


					this->numKbd->Left = 2;
					this->numKbd->Visible = false;
					
					this->numKbd->Width = 220;
					this->ventaKbd->Visible = false;
					//this->vtaTouchKbd->Location.X = 184;
					//this->vtaTouchKbd->Location.Y = 400;
					this->vtaTouchKbd->BringToFront();
					this->vtaTouchKbd->Visible = true;
					this->vtaTouchKbd->Left = 30;
					this->vtaTouchKbd->Top += 650 - this->vtaTouchKbd->Bottom;
					this->numKbd->Top += this->vtaTouchKbd->Top - this->numKbd->Bottom - 2;
					
					this->termoPanel->Left = 2;
					this->termoPanel->Top = this->numKbd->Top - this->termoPanel->Height - 2;

					this->m1->Left = this->vtaTouchKbd->Right + 20;
					this->m2->Left = this->vtaTouchKbd->Right + 20;
					this->m3->Left = this->vtaTouchKbd->Right + 20;
					this->m1->Top += 460;
					this->m2->Top += 460;
					this->m3->Top += 460;

					break;

			}			
		}

	protected:
		/// <summary>
		/// Limpiar los recursos que se están utilizando.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}
private: System::ComponentModel::IContainer^  components;
protected: 

	private:
		/// <summary>
		/// Variable del diseñador requerida.
		/// </summary>

	private: System::Windows::Forms::Label^  visorDer;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido del método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->visorDer = (gcnew System::Windows::Forms::Label());
			this->visorTot = (gcnew System::Windows::Forms::Label());
			this->m1 = (gcnew System::Windows::Forms::Label());
			this->m2 = (gcnew System::Windows::Forms::Label());
			this->m3 = (gcnew System::Windows::Forms::Label());
			this->visorMulti = (gcnew System::Windows::Forms::Label());
			this->visorCnt = (gcnew System::Windows::Forms::Label());
			this->Grid = (gcnew System::Windows::Forms::DataGridView());
			this->Column1 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Column3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->btModo = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btRed = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btConsulta = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btTF = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btLista = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btVendedor = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btEspera = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->btCajero = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->arriba = (gcnew System::Windows::Forms::Button());
			this->abajo = (gcnew System::Windows::Forms::Button());
			this->logoBitnet = (gcnew System::Windows::Forms::PictureBox());
			this->lbBitnetwww = (gcnew System::Windows::Forms::Label());
			this->perfTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->lbVersion = (gcnew System::Windows::Forms::Label());
			this->termo = (gcnew System::Windows::Forms::ProgressBar());
			this->termoMsg = (gcnew System::Windows::Forms::Label());
			this->termoPanel = (gcnew System::Windows::Forms::Panel());
			this->publicidad = (gcnew System::Windows::Forms::Panel());
			this->publiFoto = (gcnew System::Windows::Forms::PictureBox());
			this->timerHora = (gcnew System::Windows::Forms::Timer(this->components));
			this->lbCaja = (gcnew System::Windows::Forms::Label());
			this->timerLogo = (gcnew System::Windows::Forms::Timer(this->components));
			this->completeTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->btnMuestraPromos = (gcnew System::Windows::Forms::Button());
			this->spBalan = (gcnew System::IO::Ports::SerialPort(this->components));
			this->pnlBalanza = (gcnew System::Windows::Forms::Panel());
			this->lPeso = (gcnew System::Windows::Forms::Label());
			this->lbLimFiscal = (gcnew System::Windows::Forms::Label());
			this->tmrTouch = (gcnew System::Windows::Forms::Timer(this->components));
			this->spControlBal = (gcnew System::IO::Ports::SerialPort(this->components));
			this->ventaKbd = (gcnew Pos::Desktop::VentaKbd());
			this->numKbd = (gcnew Pos::Desktop::NumKbd());
			this->vtaTouchKbd = (gcnew Pos::Desktop::VentaTouchKbd());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->BeginInit();
			this->statusStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->logoBitnet))->BeginInit();
			this->termoPanel->SuspendLayout();
			this->publicidad->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->publiFoto))->BeginInit();
			this->pnlBalanza->SuspendLayout();
			this->SuspendLayout();
			// 
			// visorDer
			// 
			this->visorDer->AutoSize = true;
			this->visorDer->BackColor = System::Drawing::Color::Transparent;
			this->visorDer->Font = (gcnew System::Drawing::Font(L"Tahoma", 20, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorDer->ForeColor = System::Drawing::Color::Navy;
			this->visorDer->Location = System::Drawing::Point(697, 32);
			this->visorDer->Name = L"visorDer";
			this->visorDer->Size = System::Drawing::Size(91, 33);
			this->visorDer->TabIndex = 0;
			this->visorDer->Text = L"1.234";
			this->visorDer->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// visorTot
			// 
			this->visorTot->BackColor = System::Drawing::Color::Transparent;
			this->visorTot->Font = (gcnew System::Drawing::Font(L"Tahoma", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorTot->ForeColor = System::Drawing::Color::Green;
			this->visorTot->Location = System::Drawing::Point(298, 87);
			this->visorTot->Name = L"visorTot";
			this->visorTot->Size = System::Drawing::Size(449, 35);
			this->visorTot->TabIndex = 1;
			this->visorTot->Text = L"1.234";
			this->visorTot->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// m1
			// 
			this->m1->AutoSize = true;
			this->m1->BackColor = System::Drawing::Color::Transparent;
			this->m1->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m1->ForeColor = System::Drawing::Color::Navy;
			this->m1->Location = System::Drawing::Point(10, 185);
			this->m1->Name = L"m1";
			this->m1->Size = System::Drawing::Size(90, 18);
			this->m1->TabIndex = 2;
			this->m1->Text = L"C/C: 123 - ";
			this->m1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// m2
			// 
			this->m2->AutoSize = true;
			this->m2->BackColor = System::Drawing::Color::Transparent;
			this->m2->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m2->ForeColor = System::Drawing::Color::Navy;
			this->m2->Location = System::Drawing::Point(10, 205);
			this->m2->Name = L"m2";
			this->m2->Size = System::Drawing::Size(104, 18);
			this->m2->TabIndex = 3;
			this->m2->Text = L"Saldo: 12.50";
			this->m2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// m3
			// 
			this->m3->AutoSize = true;
			this->m3->BackColor = System::Drawing::Color::Transparent;
			this->m3->Font = (gcnew System::Drawing::Font(L"Tahoma", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->m3->ForeColor = System::Drawing::Color::Navy;
			this->m3->Location = System::Drawing::Point(10, 225);
			this->m3->Name = L"m3";
			this->m3->Size = System::Drawing::Size(120, 18);
			this->m3->TabIndex = 4;
			this->m3->Text = L"Máximo: 12.50";
			this->m3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// visorMulti
			// 
			this->visorMulti->BackColor = System::Drawing::Color::Transparent;
			this->visorMulti->Font = (gcnew System::Drawing::Font(L"Tahoma", 20, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorMulti->ForeColor = System::Drawing::Color::Navy;
			this->visorMulti->Location = System::Drawing::Point(298, 31);
			this->visorMulti->Name = L"visorMulti";
			this->visorMulti->Size = System::Drawing::Size(341, 33);
			this->visorMulti->TabIndex = 13;
			this->visorMulti->Text = L"1.234";
			this->visorMulti->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// visorCnt
			// 
			this->visorCnt->BackColor = System::Drawing::Color::Transparent;
			this->visorCnt->Font = (gcnew System::Drawing::Font(L"Tahoma", 14.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->visorCnt->ForeColor = System::Drawing::Color::Green;
			this->visorCnt->Location = System::Drawing::Point(547, 94);
			this->visorCnt->Name = L"visorCnt";
			this->visorCnt->Size = System::Drawing::Size(238, 23);
			this->visorCnt->TabIndex = 14;
			this->visorCnt->Text = L"1.234";
			this->visorCnt->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// Grid
			// 
			this->Grid->AllowUserToAddRows = false;
			this->Grid->AllowUserToDeleteRows = false;
			this->Grid->AllowUserToResizeColumns = false;
			this->Grid->AllowUserToResizeRows = false;
			this->Grid->BackgroundColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->Grid->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {this->Column1, this->Column2, 
				this->Column3});
			this->Grid->Location = System::Drawing::Point(285, 141);
			this->Grid->Name = L"Grid";
			this->Grid->ReadOnly = true;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->Grid->RowHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->Grid->RowHeadersWidth = 10;
			this->Grid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
			this->Grid->Size = System::Drawing::Size(512, 250);
			this->Grid->TabIndex = 15;
			// 
			// Column1
			// 
			this->Column1->HeaderText = L"Column1";
			this->Column1->Name = L"Column1";
			this->Column1->ReadOnly = true;
			// 
			// Column2
			// 
			this->Column2->HeaderText = L"Column2";
			this->Column2->Name = L"Column2";
			this->Column2->ReadOnly = true;
			// 
			// Column3
			// 
			this->Column3->HeaderText = L"Column3";
			this->Column3->Name = L"Column3";
			this->Column3->ReadOnly = true;
			// 
			// statusStrip1
			// 
			this->statusStrip1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->btModo, this->btRed, 
				this->btConsulta, this->btTF, this->btLista, this->btVendedor, this->btEspera, this->btCajero});
			this->statusStrip1->Location = System::Drawing::Point(0, 578);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(800, 22);
			this->statusStrip1->TabIndex = 16;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// btModo
			// 
			this->btModo->AutoSize = false;
			this->btModo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btModo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btModo->Name = L"btModo";
			this->btModo->Size = System::Drawing::Size(74, 17);
			this->btModo->Text = L"Modo REG";
			// 
			// btRed
			// 
			this->btRed->AutoSize = false;
			this->btRed->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btRed->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btRed->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btRed->Name = L"btRed";
			this->btRed->Size = System::Drawing::Size(74, 17);
			this->btRed->Text = L"Online";
			// 
			// btConsulta
			// 
			this->btConsulta->AutoSize = false;
			this->btConsulta->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btConsulta->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btConsulta->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btConsulta->Name = L"btConsulta";
			this->btConsulta->Size = System::Drawing::Size(74, 17);
			this->btConsulta->Text = L"Consulta";
			// 
			// btTF
			// 
			this->btTF->AutoSize = false;
			this->btTF->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btTF->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btTF->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btTF->Name = L"btTF";
			this->btTF->Size = System::Drawing::Size(74, 17);
			this->btTF->Text = L"T/ Factura";
			// 
			// btLista
			// 
			this->btLista->AutoSize = false;
			this->btLista->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btLista->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btLista->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btLista->Name = L"btLista";
			this->btLista->Size = System::Drawing::Size(74, 17);
			this->btLista->Text = L"Precio";
			// 
			// btVendedor
			// 
			this->btVendedor->AutoSize = false;
			this->btVendedor->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btVendedor->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btVendedor->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btVendedor->Name = L"btVendedor";
			this->btVendedor->Size = System::Drawing::Size(150, 17);
			this->btVendedor->Text = L"12/10/2009 15:25";
			// 
			// btEspera
			// 
			this->btEspera->AutoSize = false;
			this->btEspera->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btEspera->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold));
			this->btEspera->ForeColor = System::Drawing::Color::Red;
			this->btEspera->Name = L"btEspera";
			this->btEspera->Size = System::Drawing::Size(100, 17);
			this->btEspera->Text = L"Ticket Espera";
			// 
			// btCajero
			// 
			this->btCajero->AutoSize = false;
			this->btCajero->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->btCajero->BorderSides = System::Windows::Forms::ToolStripStatusLabelBorderSides::Left;
			this->btCajero->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btCajero->Name = L"btCajero";
			this->btCajero->Size = System::Drawing::Size(150, 17);
			this->btCajero->Text = L"Caj: Juan Puca";
			// 
			// arriba
			// 
			this->arriba->BackColor = System::Drawing::Color::Transparent;
			this->arriba->FlatAppearance->BorderSize = 0;
			this->arriba->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->arriba->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->arriba->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"arriba.Image")));
			this->arriba->Location = System::Drawing::Point(229, 187);
			this->arriba->Name = L"arriba";
			this->arriba->Size = System::Drawing::Size(50, 48);
			this->arriba->TabIndex = 20;
			this->arriba->Tag = L"65";
			this->arriba->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->arriba->UseVisualStyleBackColor = false;
			this->arriba->Click += gcnew System::EventHandler(this, &MainForm::arriba_Click);
			// 
			// abajo
			// 
			this->abajo->BackColor = System::Drawing::Color::Transparent;
			this->abajo->FlatAppearance->BorderSize = 0;
			this->abajo->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->abajo->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->abajo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"abajo.Image")));
			this->abajo->Location = System::Drawing::Point(229, 241);
			this->abajo->Name = L"abajo";
			this->abajo->Size = System::Drawing::Size(50, 48);
			this->abajo->TabIndex = 21;
			this->abajo->Tag = L"65";
			this->abajo->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->abajo->UseVisualStyleBackColor = false;
			this->abajo->Click += gcnew System::EventHandler(this, &MainForm::abajo_Click);
			// 
			// logoBitnet
			// 
			this->logoBitnet->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(124)), static_cast<System::Int32>(static_cast<System::Byte>(156)), 
				static_cast<System::Int32>(static_cast<System::Byte>(207)));
			this->logoBitnet->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"logoBitnet.Image")));
			this->logoBitnet->Location = System::Drawing::Point(53, 17);
			this->logoBitnet->Name = L"logoBitnet";
			this->logoBitnet->Size = System::Drawing::Size(80, 80);
			this->logoBitnet->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->logoBitnet->TabIndex = 24;
			this->logoBitnet->TabStop = false;
			// 
			// lbBitnetwww
			// 
			this->lbBitnetwww->AutoSize = true;
			this->lbBitnetwww->BackColor = System::Drawing::Color::Transparent;
			this->lbBitnetwww->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbBitnetwww->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbBitnetwww->Location = System::Drawing::Point(10, 86);
			this->lbBitnetwww->Name = L"lbBitnetwww";
			this->lbBitnetwww->Size = System::Drawing::Size(200, 13);
			this->lbBitnetwww->TabIndex = 25;
			this->lbBitnetwww->Text = L"www.supermercadoscomodin.com";
			// 
			// perfTimer
			// 
			this->perfTimer->Tick += gcnew System::EventHandler(this, &MainForm::perfTimer_Tick);
			// 
			// lbVersion
			// 
			this->lbVersion->BackColor = System::Drawing::Color::Transparent;
			this->lbVersion->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbVersion->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbVersion->Location = System::Drawing::Point(23, 102);
			this->lbVersion->Name = L"lbVersion";
			this->lbVersion->Size = System::Drawing::Size(154, 16);
			this->lbVersion->TabIndex = 26;
			this->lbVersion->Text = L"(Version 1.0.2345)";
			this->lbVersion->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// termo
			// 
			this->termo->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(192)), 
				static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->termo->Location = System::Drawing::Point(7, 18);
			this->termo->Name = L"termo";
			this->termo->Size = System::Drawing::Size(151, 16);
			this->termo->TabIndex = 27;
			this->termo->Value = 50;
			// 
			// termoMsg
			// 
			this->termoMsg->AutoSize = true;
			this->termoMsg->BackColor = System::Drawing::Color::Transparent;
			this->termoMsg->Font = (gcnew System::Drawing::Font(L"Tahoma", 8, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->termoMsg->ForeColor = System::Drawing::Color::Navy;
			this->termoMsg->Location = System::Drawing::Point(4, 2);
			this->termoMsg->Name = L"termoMsg";
			this->termoMsg->Size = System::Drawing::Size(89, 13);
			this->termoMsg->TabIndex = 28;
			this->termoMsg->Text = L"Máximo: 12.50";
			this->termoMsg->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// termoPanel
			// 
			this->termoPanel->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->termoPanel->Controls->Add(this->termoMsg);
			this->termoPanel->Controls->Add(this->termo);
			this->termoPanel->Location = System::Drawing::Point(8, 251);
			this->termoPanel->Name = L"termoPanel";
			this->termoPanel->Size = System::Drawing::Size(171, 38);
			this->termoPanel->TabIndex = 29;
			this->termoPanel->Visible = false;
			// 
			// publicidad
			// 
			this->publicidad->BackColor = System::Drawing::Color::White;
			this->publicidad->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->publicidad->Controls->Add(this->publiFoto);
			this->publicidad->Location = System::Drawing::Point(349, 67);
			this->publicidad->Name = L"publicidad";
			this->publicidad->Size = System::Drawing::Size(280, 402);
			this->publicidad->TabIndex = 30;
			this->publicidad->Visible = false;
			// 
			// publiFoto
			// 
			this->publiFoto->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->publiFoto->Dock = System::Windows::Forms::DockStyle::Fill;
			this->publiFoto->Location = System::Drawing::Point(0, 0);
			this->publiFoto->Name = L"publiFoto";
			this->publiFoto->Size = System::Drawing::Size(278, 400);
			this->publiFoto->TabIndex = 0;
			this->publiFoto->TabStop = false;
			// 
			// timerHora
			// 
			this->timerHora->Enabled = true;
			this->timerHora->Interval = 60000;
			this->timerHora->Tick += gcnew System::EventHandler(this, &MainForm::timerHora_Tick);
			// 
			// lbCaja
			// 
			this->lbCaja->BackColor = System::Drawing::Color::Transparent;
			this->lbCaja->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbCaja->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbCaja->Location = System::Drawing::Point(21, 120);
			this->lbCaja->Name = L"lbCaja";
			this->lbCaja->Size = System::Drawing::Size(154, 16);
			this->lbCaja->TabIndex = 31;
			this->lbCaja->Text = L"Caja Nro: 12";
			this->lbCaja->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// timerLogo
			// 
			this->timerLogo->Tick += gcnew System::EventHandler(this, &MainForm::timerLogo_Tick);
			// 
			// completeTimer
			// 
			this->completeTimer->Interval = 2000;
			this->completeTimer->Tick += gcnew System::EventHandler(this, &MainForm::completeTimer_Tick);
			// 
			// btnMuestraPromos
			// 
			this->btnMuestraPromos->Location = System::Drawing::Point(135, 31);
			this->btnMuestraPromos->Name = L"btnMuestraPromos";
			this->btnMuestraPromos->Size = System::Drawing::Size(75, 23);
			this->btnMuestraPromos->TabIndex = 32;
			this->btnMuestraPromos->Text = L"Promos";
			this->btnMuestraPromos->UseVisualStyleBackColor = true;
			this->btnMuestraPromos->Visible = false;
			this->btnMuestraPromos->Click += gcnew System::EventHandler(this, &MainForm::btnMuestraPromos_Click);
			// 
			// spBalan
			// 
			this->spBalan->DataReceived += gcnew System::IO::Ports::SerialDataReceivedEventHandler(this, &MainForm::spBalan_DataReceived);
			// 
			// pnlBalanza
			// 
			this->pnlBalanza->BackColor = System::Drawing::Color::Transparent;
			this->pnlBalanza->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pnlBalanza.BackgroundImage")));
			this->pnlBalanza->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->pnlBalanza->Controls->Add(this->lPeso);
			this->pnlBalanza->Location = System::Drawing::Point(172, 16);
			this->pnlBalanza->Name = L"pnlBalanza";
			this->pnlBalanza->Size = System::Drawing::Size(110, 131);
			this->pnlBalanza->TabIndex = 34;
			this->pnlBalanza->Visible = false;
			// 
			// lPeso
			// 
			this->lPeso->BackColor = System::Drawing::Color::Transparent;
			this->lPeso->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->lPeso->Font = (gcnew System::Drawing::Font(L"Tahoma", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lPeso->ForeColor = System::Drawing::Color::Red;
			this->lPeso->Location = System::Drawing::Point(0, 59);
			this->lPeso->Name = L"lPeso";
			this->lPeso->Size = System::Drawing::Size(110, 72);
			this->lPeso->TabIndex = 34;
			this->lPeso->Text = L"label1";
			this->lPeso->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lPeso->Visible = false;
			// 
			// lbLimFiscal
			// 
			this->lbLimFiscal->BackColor = System::Drawing::Color::Transparent;
			this->lbLimFiscal->Font = (gcnew System::Drawing::Font(L"Tahoma", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lbLimFiscal->ForeColor = System::Drawing::Color::PapayaWhip;
			this->lbLimFiscal->Location = System::Drawing::Point(15, 136);
			this->lbLimFiscal->Name = L"lbLimFiscal";
			this->lbLimFiscal->Size = System::Drawing::Size(200, 16);
			this->lbLimFiscal->TabIndex = 35;
			this->lbLimFiscal->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// tmrTouch
			// 
			this->tmrTouch->Enabled = true;
			this->tmrTouch->Interval = 1000;
			this->tmrTouch->Tick += gcnew System::EventHandler(this, &MainForm::tmrTouch_Tick);
			// 
			// spControlBal
			// 
			this->spControlBal->DataReceived += gcnew System::IO::Ports::SerialDataReceivedEventHandler(this, &MainForm::spControlBal_DataReceived);
			// 
			// ventaKbd
			// 
			this->ventaKbd->BackColor = System::Drawing::Color::LightSteelBlue;
			this->ventaKbd->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->ventaKbd->Colapsable = false;
			this->ventaKbd->Location = System::Drawing::Point(184, 400);
			this->ventaKbd->Name = L"ventaKbd";
			this->ventaKbd->Size = System::Drawing::Size(613, 162);
			this->ventaKbd->TabIndex = 23;
			this->ventaKbd->ClickEnBoton += gcnew Pos::Desktop::BotonClick(this, &MainForm::ClickEnBoton);
			// 
			// numKbd
			// 
			this->numKbd->BackColor = System::Drawing::Color::LightSteelBlue;
			this->numKbd->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->numKbd->Location = System::Drawing::Point(8, 291);
			this->numKbd->Name = L"numKbd";
			this->numKbd->Size = System::Drawing::Size(170, 271);
			this->numKbd->TabIndex = 22;
			this->numKbd->ClickEnBoton += gcnew Pos::Desktop::BotonClick(this, &MainForm::ClickEnBoton);
			// 
			// vtaTouchKbd
			// 
			this->vtaTouchKbd->BackColor = System::Drawing::Color::Transparent;
			this->vtaTouchKbd->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->vtaTouchKbd->Colapsable = false;
			this->vtaTouchKbd->Location = System::Drawing::Point(30, 60);
			this->vtaTouchKbd->Name = L"vtaTouchKbd";
			this->vtaTouchKbd->Size = System::Drawing::Size(263, 300);
			this->vtaTouchKbd->TabIndex = 36;
			this->vtaTouchKbd->ClickEnBoton += gcnew Pos::Desktop::BotonClick(this, &MainForm::ClickEnBoton);
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(192)), static_cast<System::Int32>(static_cast<System::Byte>(255)), 
				static_cast<System::Int32>(static_cast<System::Byte>(192)));
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"$this.BackgroundImage")));
			this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->ClientSize = System::Drawing::Size(800, 600);
			this->Controls->Add(this->vtaTouchKbd);
			this->Controls->Add(this->lbLimFiscal);
			this->Controls->Add(this->pnlBalanza);
			this->Controls->Add(this->btnMuestraPromos);
			this->Controls->Add(this->lbCaja);
			this->Controls->Add(this->visorCnt);
			this->Controls->Add(this->m1);
			this->Controls->Add(this->publicidad);
			this->Controls->Add(this->termoPanel);
			this->Controls->Add(this->lbVersion);
			this->Controls->Add(this->visorDer);
			this->Controls->Add(this->visorMulti);
			this->Controls->Add(this->visorTot);
			this->Controls->Add(this->lbBitnetwww);
			this->Controls->Add(this->logoBitnet);
			this->Controls->Add(this->ventaKbd);
			this->Controls->Add(this->numKbd);
			this->Controls->Add(this->abajo);
			this->Controls->Add(this->arriba);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->Grid);
			this->Controls->Add(this->m3);
			this->Controls->Add(this->m2);
			this->Font = (gcnew System::Drawing::Font(L"Tahoma", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"MainForm";
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &MainForm::MainForm_FormClosed);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MainForm::MainForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Grid))->EndInit();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->logoBitnet))->EndInit();
			this->termoPanel->ResumeLayout(false);
			this->termoPanel->PerformLayout();
			this->publicidad->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->publiFoto))->EndInit();
			this->pnlBalanza->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	System::Void MainForm_Load(System::Object^  sender, System::EventArgs^  e) 
	{
		// Pone el numero de version
		lbVersion->Text = Strings::CharToString(posVersion);

		// Personaliza el logo
		if(pantalla != 3)
			timerLogo_Tick(nullptr, nullptr);

		// Acomoda las forms.
		SetPosConsole(0, this->Height);

		// Refresca los visores
		System::Drawing::Font ^xFont;
		switch(tipoForm)
		{
			// Touch 800x600 - Default
			case 0:
				break;

			// No Touch - 1024 x 768
			case 1:
				xFont = gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Regular);
				Grid->RowTemplate->Height = 36;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 10.0, FontStyle::Bold);
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width * 1.45);
					col->DefaultCellStyle->Font = xFont;
				}
				break;

			case 3:
				visorMulti->ForeColor = System::Drawing::Color::Black;
				visorDer->ForeColor = System::Drawing::Color::Black;
				visorTot->ForeColor = System::Drawing::Color::Black;
				visorCnt->ForeColor = System::Drawing::Color::Black;
				xFont = gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Regular);
				Grid->DefaultCellStyle->SelectionBackColor = System::Drawing::Color::LightGray;
				Grid->DefaultCellStyle->SelectionForeColor = System::Drawing::Color::Black;
				Grid->RowTemplate->Height += 6;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Bold);
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width * 1.6);
					col->DefaultCellStyle->Font = xFont;
				}
				break;

			// No Touch - 1280 x 720
			case 4:
				xFont = gcnew System::Drawing::Font("Tahoma", 12.0, FontStyle::Regular);
				Grid->RowTemplate->Height = 36;
				Grid->ColumnHeadersHeight = 36;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 12.0, FontStyle::Bold);
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width * 1.8);
					col->DefaultCellStyle->Font = xFont;
				}
				break;

			case 5:
				xFont = gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Regular);
				Grid->RowTemplate->Height = 36;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Bold);
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width * 1.45);
					col->DefaultCellStyle->Font = xFont;
				}
				break;
			case 7:
				xFont = gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Regular);
				Grid->RowTemplate->Height = 36;
				Grid->ColumnHeadersDefaultCellStyle->Font = 
					gcnew System::Drawing::Font("Tahoma", 11.0, FontStyle::Bold);
				this->Grid->BackgroundColor = System::Drawing::Color::White;
				for each (DataGridViewColumn ^col in Grid->Columns)
				{
					col->Width = (int)((double)col->Width * 1.45);
					col->DefaultCellStyle->Font = xFont;
				}				
				break;
		}
		if (TieneBalanza)
		{	
			spBalan->PortName = "COM" + (COM_BALAN + 1);			
			try
			{
				spBalan->Open();
				//lEstado->Text = "Conectado a la balanza";
			}
			catch (System::Exception ^exce)
			{
				lPeso->Text = "No Balan";
				lPeso->Visible = true;
				WriteBootLog("Error en puerto de balanza");
			}
		}

		if (tieneBalanzaControl)
		{	
			spControlBal->PortName = "COM" + (COM_BCL + 1);			
			try
			{
				spControlBal->Open();
				//lEstado->Text = "Conectado a la balanza";
			}
			catch (System::Exception ^exce)
			{
				lPeso->Text = "No Balan";
				lPeso->Visible = true;
				WriteBootLog("Error en puerto de balanza");
			}
		}

		// Habilita el completado de la pantalla.
		this->completeTimer->Enabled = true;
		clear(1);
	}

	System::Void ClickEnBoton(System::UInt32 tecla) 
	{
		putkey(tecla);
	}

	System::Void arriba_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		putkey(keyupper);	 
	}

	System::Void abajo_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		putkey(keydown);
	}
private: System::Void MainForm_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
			 Strings::releaseKeyboard(this);
		 }
private: System::Void timerHora_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->btVendedor->Text = DateTime::Now.ToShortDateString() + " - " +
				 DateTime::Now.ToShortTimeString();
			 this->btTF->Text = this->leyendaTicket();
		 }
private: System::Void MainForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			 
			 // Mata todas las tareas que se esten ejecutando
			 closed = true;
			 killall();
		 }

		 int prevLogoPointer;


private: System::Void timerLogo_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(reloadLogos) 
			 {
				 // Carga los datos desde el ini y, desde ahi, carga las imagenes en imgLogos
				 LoadLogos();
				 imgLogos->Clear();
				 for(int k=0; k<logos->Count(); k++)
				 {
					String  ^logoPath = Strings::CharToString(logos->ItemString(k));
					if(logoPath->StartsWith("\""))
						logoPath = logoPath->Substring(1);
					if(logoPath->EndsWith("\""))
						logoPath = logoPath->Substring(0, logoPath->Length - 1);
					logoPath = System::IO::Path::GetFullPath(logoPath);
					if(System::IO::File::Exists(logoPath))
						imgLogos->Add(Image::FromFile(logoPath));
				 }

				 // Inicializa las variables de control del proceso.
		 		 logoPointer = 0;
				 timerLogo->Interval = 1000 * ((logoDelay == 0)? 120: logoDelay);
				 timerLogo->Enabled = true;
				 prevLogoPointer = -1;
			 }  

			 if(imgLogos->Count <= logoPointer) { logoPointer = 0; return; }
			 if(prevLogoPointer != logoPointer)
			 {
				prevLogoPointer = logoPointer;
				publiFoto->BackgroundImage = imgLogos[logoPointer++];
			 }
		 }

private: System::Void completeTimer_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->BringToFront();
			 this->Focus();
			 completeTimer->Enabled = false;
		 }
private: System::Void btnMuestraPromos_Click(System::Object^  sender, System::EventArgs^  e) {
			 MuestraForm ^msform = gcnew MuestraForm();
			 msform->muestraPromos();
			 msform->ShowDialog();
		 }
private: System::Void spBalan_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e) {

			 // Este handler corre en un hilo de fondo de .NET (evento SerialDataReceived), en paralelo
			 // al hilo principal del POS. Para no pisar el estado compartido (pesoLeido y los buffers
			 // globales) mientras una operacion -plu / pago / registracion- esta en curso, se serializa
			 // la actualizacion con el mismo lock del lazo principal (LockMainLoop / PosProc).
			 // El lock NO se sostiene durante ActualizarPeso(): ese metodo hace Invoke al hilo de UI y
			 // mantener el lock durante el marshalling podria provocar un interbloqueo.
			 String ^leido = nullptr;
			 try
			 {
				 leido = spBalan->ReadExisting();
			 }
			 catch (...)
			 {
				 WriteBootLog("Error en Recepcion datos balanza...");
				 pesoLeido = 0;
				 return;
			 }

			 GCStatic::LockMainLoop("balanza");
			 try
			 {
				 if (TieneBalanza == 1)
					 LeePesoBalanza(leido);
				 else if (TieneBalanza == 2)
					 LeePesoBalanzaSystel(leido);
			 }
			 catch (...)
			 {
				 WriteBootLog("Error en Recepcion datos balanza...");
				 pesoLeido = 0;
			 }
			 finally
			 {
				 GCStatic::ReleaseLockMainLoop("balanza");
			 }

			 // Refresco visual FUERA del lock (hace Invoke al hilo de UI).
			 try { this->ActualizarPeso(); }
			 catch (...) {}
		 }

		 System::Void LeePesoBalanza(System::String ^leido) {
			 StringBuilder^ cad = gcnew StringBuilder();
			 bool conpunto = false;
			 bool confin = false;
			 for (int i = 0; i < leido->Length; i++)
			 {
				 if (Char::IsDigit(leido[i]) || leido[i] == '.' || leido[i] == ',')
				 {
					 if (leido[i] == '.')
						 cad->Append((Char)44);
					 else
						 cad->Append(leido[i]);
					 if (leido[i] == '.')
						 conpunto = true;
				 }
				 else
				 {
					 confin = true;
					 break;
				 }
			 }
			 pesoLeido = 0;
			 if (cad->Length >= 4 && cad->Length <= 6)
			 {
				 pesoLeido = Convert::ToDouble(cad->ToString());
				 if (!conpunto)
					 pesoLeido = pesoLeido / 1000;
			 }
			 if (confin)
				 spBalan->DiscardInBuffer();
		 }

		 System::Void LeePesoBalanzaSystel(System::String^ leido) {

			 for each (Char ^itemleido in leido)
			 {
				 if (System::Convert::ToInt16(itemleido) == 2)
					 leoPesoSystel->Length = 0;
				 else if (System::Convert::ToInt16(itemleido) == 13){//'\x0D') {
					 pesoLeido = Convert::ToDouble(leoPesoSystel->ToString()->Replace(".", ","));
				 }
				 else
					 leoPesoSystel->Append(itemleido);
			 }

		 }

private: System::Void tmrTouch_Tick(System::Object^  sender, System::EventArgs^  e) {
			 tmrTouch->Enabled = false;
			 if (selfCheckout)
			 {
				 tmrTouch->Interval = 1000;				 
				 if (inOper == 0 && !enSeleccionCliente)
				 {				 
					 cntEsperaTouch++;
					 if (cntEsperaTouch > timeoutSelf)
					 {					 
						 frmTouchInicial ^ftouch = gcnew frmTouchInicial();
						 ftouch->ShowDialog();
						 putkey(keycanc);
						 cntEsperaTouch = 0;
					 }					 
				 }
				 tmrTouch->Enabled = true;
			 }
		 }
private: System::Void spControlBal_DataReceived(System::Object^  sender, System::IO::Ports::SerialDataReceivedEventArgs^  e) {
			 try
			 {
				 String ^leido = spControlBal->ReadExisting();
				 StringBuilder ^cad =  gcnew StringBuilder();			 
				 bool conpunto = false;
				 bool confin = false;
				 bool comienzaNum = false;
				 for (int i = 0; i < leido->Length; i++)
				 {
					 if (!comienzaNum && !Char::IsDigit(leido[i]))
						 continue;
					 if (Char::IsDigit(leido[i]) || leido[i] == '.' || leido[i] == ',')
					 {
						 comienzaNum = true;
						 if (leido[i] == '.')
							 cad->Append((Char)44);
						 else
							 cad->Append(leido[i]);
						 if (leido[i] == '.')
							 conpunto = true;
					 }
					 else
					 {
						 confin = true;
						 break;
					 }
				 }			 
				 pesoBalanControl = 0;
				 if (cad->Length >= 4 && cad->Length <= 6)
				 {
					 pesoBalanControl = Convert::ToDouble(cad->ToString());
					 if (!conpunto)
						 pesoBalanControl = pesoBalanControl /1000;
				 }
				 if (confin)
					 spControlBal->DiscardInBuffer();
				 //lblPesoControl->Text = System::Convert::ToString(acumPeso);
				 
			 }
			 catch (...)
			 {
				 WriteBootLog("Error en Recepcion datos balanza...");
			 }
		 }
};
}
#endif	POSXP
