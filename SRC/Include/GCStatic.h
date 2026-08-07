#ifndef MUTEXES_INCLUDED__
#define MUTEXES_INCLUDED__

#ifdef POSXP

#ifndef __STA_INCLUDED
	#define __STA_INCLUDED
	#include "AlertForm.h"

	ref class StaticAlerts 
	{ 
	public: 
		static AlertForm ^af; 
		static SoundPlayer ^mp;
	};

#endif
#include "Strings.h"
#include "DatosMpagos.h"


using namespace System;
using namespace System::Threading;
using namespace Bitnet::TJOCommon;
//using namespace Fiscal2Gen;

#ifdef WFACT
using namespace fact;
#endif

// Referencias a 'Dump.cpp'
extern dbf tDbf;
void OpenTransFiles();

ref class TransLockBag
{
	public:
		String ^LockName;
		bool WasOpened;
		int Recno;
		bool atEOF;
};

// Clase que contiene definiciones estaticas GC.
ref class GCStatic 
{
#ifdef WFACT
	public: static FactPluginClass ^factPlugin;

	static bool loadFactPlugin = true;

	public: static FactPluginClass ^GetFactPlugin()
	{
		if(loadFactPlugin)
		{
			loadFactPlugin = false;
			try
			{
				factPlugin = gcnew FactPluginClass();
				if((int)factPlugin->NUMCAJA > 0)
					factPlugin->NUMCAJA = ckaNro;
				else
					factPlugin = nullptr;
				WriteBootLog("PLUGIN de facturacion bien cargado");
			}
			catch(...)
			{
				WriteBootLog("Error al cargar el PLUGIN de facturacion");
				factPlugin = nullptr;
			}
		}

		if(factPlugin == nullptr)			
			return nullptr;		
		factPlugin->NUMCAJERO = ckaNro;
		return factPlugin;
	}
#endif

	static System::Collections::Generic::List<TransLockBag ^> ^locks = 
		gcnew System::Collections::Generic::List<TransLockBag ^>();

	private: 
		static bool transWasOpened;
		static int transRecno;
	
	public: 
		static Mutex ^Kb = gcnew System::Threading::Mutex();
		static Mutex ^Trans = gcnew System::Threading::Mutex();
		static Mutex ^PosProc = gcnew System::Threading::Mutex();
		static Mutex ^EventLock = gcnew System::Threading::Mutex();
		static Mutex ^TouchSync = gcnew System::Threading::Mutex();
		static Mutex ^MainFormLock = gcnew System::Threading::Mutex();
		static TJOClient ^X25Client;
		static List<CPosnet ^> ^tjPosnet = gcnew List<CPosnet ^>();

		//static Fiscal2Gen::Fiscal2Generacion ^impre2Gen = gcnew Fiscal2Gen::Fiscal2Generacion();

	public: static void LockMainForm(char *func)
	{
		GCStatic::MainFormLock->WaitOne();
	}

	public: static void ReleaseMainForm(char *func)
	{
		GCStatic::MainFormLock->ReleaseMutex();
	}

	public: static void LockTouch(char *func)
	{
		GCStatic::TouchSync->WaitOne();
	}

	public: static void ReleaseTouch(char *func)
	{
		GCStatic::TouchSync->ReleaseMutex();
	}

	public:static void LockKeyboard(char *func) 
	{
	   GCStatic::Kb->WaitOne();
	}

	public:static void ReleaseKeyboard(char *func) 
	{ 
		GCStatic::Kb->ReleaseMutex();
	}

	public: static void LockEvent(char *func)
	{
		GCStatic::EventLock->WaitOne();
	}

	public:static void ReleaseLockEvent(char *func)
	{
		GCStatic::EventLock->ReleaseMutex();
	}

	public: static void LockMainLoop(char *func)
	{
		GCStatic::PosProc->WaitOne();
	}

	public:static void ReleaseLockMainLoop(char *func)
	{
		GCStatic::PosProc->ReleaseMutex();
	}

	public: static void AddLockToTrans(char *func)
	{
		// Pone el lock
		GCStatic::Trans->WaitOne();

		// Construye el bag en el que almacenaremos el estado del TRANS
		TransLockBag ^bag = gcnew TransLockBag();
		locks->Add(bag);

		// Salva el estado del lock y el del TRANS.
		bag->LockName = Strings::CharToString(func);
		bag->WasOpened = tDbf.Opened();
		if(bag->WasOpened)
		{
			bag->Recno = tDbf.Recno();
			bag->atEOF = (tDbf.Recno() == tDbf.RecCount());
		}
	}

	public: static void ReleaseLockFromTrans(char *func)
	{
		// Ayuda al desarrollo, porque valida que las llamadas a 'Release' esten correctamente balanceadas
		// con la correspondiente llamada a 'Lock'
		String ^n = Strings::CharToString(func);
		int index = locks->Count - 1;
		if(locks->Count < 1 || locks[index]->LockName != n)
		{
			char tmsg[100], tbuff[52];
			Strings::StringToChar(locks[index]->LockName, tbuff, 50);
			sprintf(tmsg, "Error Interno - 'lock' desbalanceado en '%s'. Tope='%s'", func, tbuff);
			WLog(tmsg);
			POSExit();
		}
		
		// Recupera el estado del TRANS a partir de la informacion atesorada en el bag.
		TransLockBag ^bag = locks[index];
		locks->RemoveAt(index);
		if(bag->WasOpened)
		{
			if(!tDbf.Opened()) OpenTransFiles();
			/*
			if(bag->atEOF)
				tDbf.Go(DB_GO_BOTTOM);
			else
				tDbf.Go(bag->Recno);*/
		}

		// Libera el lock
		GCStatic::Trans->ReleaseMutex();
	}
};

#else

class GCLockBag
{
public:
	int lockCount;
	int ownerThread;

	GCLockBag() { lockCount = ownerThread = 0; }
};
extern GCLockBag kbLock, eventLock, transLock, posLock;

// Clase que contiene definiciones estaticas GC.
class GCStatic
{
	static void DoLock(GCLockBag *bag);
	static void DoUnlock(GCLockBag *bag);

public: 
	static inline void LockKeyboard(char *func) { DoLock(&kbLock); }
	static void ReleaseKeyboard(char *func) { DoUnlock(&kbLock); }

	static void LockEvent(char *func) { DoLock(&eventLock); }
	static void ReleaseLockEvent(char *func) { DoUnlock(&eventLock); }

	static void LockMainLoop(char *func) { DoLock(&posLock); }
	static void ReleaseLockMainLoop(char *func) { DoUnlock(&posLock); }

	static void AddLockToTrans(char *func) { DoLock(&transLock); }
	static void ReleaseLockFromTrans(char *func) { DoUnlock(&transLock); }
};

#ifndef __VSC__
#define _CLI_ cli
#define _STI_ sti
#else
#define _CLI_ nop
#define _STI_ nop
#endif
#endif
#endif

