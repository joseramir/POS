#include "stdafx.h"
#ifdef POSXP

#define SKIP_BDECIM
#include "pos.h"
#include "MainForm.h"
#include "PosConsole.h"

#include "windows.h"

using namespace System::Windows::Forms;

void SetPosConsole(int x, int y)
{
	HWND handle = GetConsoleWindow();
	//SetWindowPos(handle, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
	ShowScrollBar(handle, SB_BOTH, false);
}
#endif
