#pragma once
#ifdef POSXP

void PrepareConsoleForm();
void SetPosConsole(int x, int y);

namespace pos {

	public ref class PosConsoles
	{
	public:
		static int Height;	
	};
}
#endif