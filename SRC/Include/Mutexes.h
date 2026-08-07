#ifndef MUTEXES_INCLUDED__
#define MUTEXES_INCLUDED__

#ifdef POSXP

using namespace System::Threading;

ref class KB
{
	static System::Collections::Generic::List<String ^> ^locks = 
		gcnew System::Collections::Generic::List<String ^>();

	public: static Mutex ^Kb = gcnew System::Threading::Mutex();
	public: static Mutex ^Trans = gcnew System::Threading::Mutex();

	public: static void AddLockToTrans(char *func)
	{
		String ^s = "";
		if(locks->Count > 0) s = locks[0];
		locks->Add(Strings::CharToString(func));
		KB::Trans->WaitOne();
	}

	public: static void ReleaseLockFromTrans(char *func)
	{
		String ^n = Strings::CharToString(func);
		if(locks[locks->Count - 1] != n)
		{
			char tmsg[100], tbuff[52];
			Strings::StringToChar(locks[locks->Count - 1], tbuff, 50);
			sprintf(tmsg, "Error Interno - 'lock' desbalanceado en '%s'. Tope='%s'", func, tbuff);
			WLog(tmsg);
			POSExit();
		}
		
		locks->RemoveAt(locks->Count - 1);
		KB::Trans->ReleaseMutex();
	}
};

#define SetMutex() KB::Kb->WaitOne()
#define RelMutex() KB::Kb->ReleaseMutex()

#define LockTrans(x) KB::AddLockToTrans(x)
#define ReleaseTrans(x) KB::ReleaseLockFromTrans(x)

#else

#define SetMutex()
#define ReleaseMutex()

#define LockTrans()
#define UnlockTrans()

#endif POSXP
#endif MUTEXES_INCLUDED__
