#pragma once

#include "opc.h"
//#include "StaticDump.h"

#if !E_KEYB
#define E_KEYB 1
void addEvent(int clase, char *param, unsigned sizeparam);
#endif

using namespace System::Windows::Forms;

// Desdefinirla para no simular la lectura por lector de tarjetas magneticas
#define DEBUG_CARDS_READER

// En pos.cpp
void WriteBootLog(char *msg);
long time__(void);

// La siguiente variable se puede activar para obtener en el log
// un dump de los codigos de tecla procesados.
extern bool kbDump;

using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Media;

void putkey(unsigned int code);
extern int _stackModos[], _modoSP;

ref class KbFormData
{
public:
	int kbModo;
	bool handled;
	bool inCardReader;
};

ref class Strings
{
private:
	static StringBuilder ^sb = gcnew StringBuilder();
	static Mutex ^sHandLock = gcnew Mutex();

public:
	static String ^CharToString(char *data)
	{
		sHandLock->WaitOne();
		sb->Length = 0;
		if(data != NULL)
		{
			while(*data)
				sb->Append((wchar_t) *data++);
		}
		String ^res = sb->ToString();
		sHandLock->ReleaseMutex();
		return res;
	}

	static char *StringToChar(String ^str, char *data, int maxlen)
	{
		int strLen = str->Length;
		char *origPtr = data;
		if(strLen > maxlen - 1) strLen = maxlen - 1;
		for(int index = 0; index < strLen; index++)
			*data++ = (char)str[index];
		*data = 0;
		return origPtr;
	}

	public: static void InterceptKeyboard2(Control ^ctl)
	{
		ctl->PreviewKeyDown += gcnew PreviewKeyDownEventHandler(&KPreview);
		for each (Control ^subCtl in ctl->Controls)
			InterceptKeyboard2(subCtl);
	}

	public: static void InterceptKeyboard(Form ^ctl, int modo)
	{
		KbFormData ^fd = gcnew KbFormData();
		fd->handled = false;
		fd->kbModo = modo;
		fd->inCardReader = false;

		ctl->Tag = fd;
		ctl->KeyPreview = true;
		ctl->KeyPress += gcnew KeyPressEventHandler(&KPress);
		ctl->KeyDown += gcnew KeyEventHandler(&KDown);
		InterceptKeyboard2(ctl);
	}

	public: static void releaseKeyboard(Form ^ctl)
	{
	}
   
   public: static void DisableKey(Form ^ctl)
	{
		ctl->KeyPress -= gcnew KeyPressEventHandler(&KPress);
		ctl->KeyDown -= gcnew KeyEventHandler(&KDown);
	}

	public: static void EnableKey(Form ^ctl)
	{
		ctl->KeyPress += gcnew KeyPressEventHandler(&KPress);
		ctl->KeyDown += gcnew KeyEventHandler(&KDown);
	}


	public: static void KPreview(Object ^sender, PreviewKeyDownEventArgs ^e)
	{
		e->IsInputKey = true;
	}

	static bool acceptingCardReader, prvCtlEnabled;

	static void KPress(Object ^sender, KeyPressEventArgs ^e)
	{
		try{
			lastact = time__();
			/*if (selfCheckout)
				cntEsperaTouch = 0;*/
			KbFormData ^fd = nullptr;
			Form ^form = dynamic_cast<Form ^>(sender);
			if(form != nullptr) fd = dynamic_cast<KbFormData ^>(form->Tag);

			if(fd != nullptr && !e->Handled)
			{
				if(fd->handled)
				{
					fd->handled = false;
					//e->Handled = true;
					return;
				}

				switch(fd->kbModo)
				{
					case 1:
						Controles::BaseDialog ^bd = dynamic_cast<Controles::BaseDialog ^>(form);
						if(bd != nullptr)
						{
							if(fd->inCardReader)
							{
								e->Handled = true;
								if(e->KeyChar == '\r')
								{
									fd->inCardReader = false;
									bd->EndCardReader();
								}
								else
									fd->inCardReader = bd->CardReaderDigit(e->KeyChar);
							}
							else if(e->KeyChar == '%')
							{
								e->Handled = true;
								fd->inCardReader = true;
								bd->StartCardReader();
								bd->CardReaderDigit(e->KeyChar);
							}
						}
						break;
				}
			}
		}catch (System::Exception^ excep)
		 {	
			 //ClrLog::escribeExcepcion("errormain.txt", "KPress", excep);	
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");		 
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion KPress", System::DateTime::Now);
			 writer->WriteLine(fhdatos);
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);		 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
		 }
	}

	static void KDown(Object ^sender, KeyEventArgs ^e)
	{

		lastact = time__();
		KbFormData ^fd = nullptr;
		Form ^form = dynamic_cast<Form ^>(sender);
		if(form != nullptr) fd = dynamic_cast<KbFormData ^>(form->Tag);

		if(fd != nullptr && !e->Handled)
		{
			// Fuerza el modo que el control declaro.
			int _kbModo = fd->kbModo;

#ifdef POSDEBUG_KB
			char tBuff[100];
			WLog("Kb Modo %d: OK/Cancel: %d, HotKey: %s", 
				_kbModo, 
				(int)e->KeyData,
				Strings::StringToChar(e->KeyCode.ToString(), tBuff, 90));
#endif

			if(_kbModo == 1)
			{
				if(!fd->inCardReader)
				{
					if((int)e->KeyData == keycanc)
					{
						((Form ^)sender)->CancelButton->PerformClick();
						e->Handled = true;
					}
					else if((int)e->KeyData == keyenter || (int)e->KeyData == 21)
					{
						((Form ^)sender)->AcceptButton->PerformClick();
						e->Handled = true;
					}
					else
					{
						Controles::BaseDialog ^bd = dynamic_cast<Controles::BaseDialog ^>(sender);

						// Proceso de teclas
						if(bd != nullptr)
						{
							if(bd->ProcessHotKey(e->KeyCode))
								e->Handled = true;
							else if((int)e->KeyData == keyupper)
							{
								bd->ProcessTab(-1);
								e->Handled =  true;
							}
							else if((int)e->KeyData == keydown)
							{
								bd->ProcessTab(1);
								e->Handled = true;
							}
							else if(e->KeyCode == Keys::Enter)
								e->Handled = bd->ClickIfButton();
						}
					}
				}

				if(e->Handled) 
					fd->handled = true;
			}
			else if(_kbModo == 4)
			{
				Controles::IBuscador ^bus = dynamic_cast<Controles::IBuscador ^>(sender);
				if(bus != nullptr)
				{
					int key = (int)e->KeyCode;
					if(e->KeyCode == Keys::F8 || key == keyenter || key == 21 || key == 196693 || key == 85)
						bus->AcceptKey(Keys::F8);
					else
						bus->AcceptKey(e->KeyCode);
					e->Handled = true;
				}
			}
			else if(_kbModo == 2)
			{
				Controles::IBuscador ^bus = dynamic_cast<Controles::IBuscador ^>(sender);
				if(bus != nullptr)
				{
					int key = (int)e->KeyCode;
					if(e->KeyCode == Keys::F8 || key == keyenter || key == 21 || key == 196693 || key == 17)
						bus->AcceptKey(Keys::F8);
					else
						bus->AcceptKey(e->KeyCode);
					e->Handled = true;
				}
			}
			else
			{
				switch(e->KeyCode)
				{	
					case Keys::NumPad0:
					case Keys::NumPad1:
					case Keys::NumPad2:
					case Keys::NumPad3:
					case Keys::NumPad4:
					case Keys::NumPad5:
					case Keys::NumPad6:
					case Keys::NumPad7:
					case Keys::NumPad8:
					case Keys::NumPad9:
						e->Handled = true;
						putkey((unsigned int)e->KeyValue - 48);
						break;

					case Keys::Escape:
						e->Handled = true;
						putkey(27);
						break;

					case Keys::Return:
						e->Handled = true;
						putkey(13);
						break;

					default:
						int kc = (int)e->KeyCode;
						if(e->Control)
						{
							int kk = (int)e->KeyValue;
							e->Handled = true;
							if(kk != 16 && kk != 17)
								putkey(kk - 64);
						}
						else if(
							(kc >= (int)Keys::D0 && kc <= (int)Keys::D9) ||
							(kc >= (int)Keys::A && kc <= (int)Keys::Z)
						  ) 
						{
							e->Handled = true;
							putkey(kc);
						}
						else
						{
							e->Handled = true;
							putkey((int)e->KeyData);
						}
						
						break;
				}
			}
		}
	}
};
