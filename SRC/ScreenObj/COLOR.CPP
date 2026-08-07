//
//   Color.cpp
//   Soporte del color picker.
//

#include <stdafx.h>

//   Prototipos de funciones que se usan aqui.
int GetKey(void);

//   Construye y anima el picker.
short MakeColorPicker(short aColor)
{
	int h, uy, ux, k;
	short tColor;

	h = wDefine(1, 1, 19, 50, BORDERSINGLE, SNGCURS, 0, " Color ",
				"Pulse <ESC>");

	wActivate(h);
	for(tColor = 0; tColor < 255; tColor++)
	{
		wLocate(tColor / 16, (tColor % 16) * 3);
		wSetColor(tColor);
		wPstr("XX");
		wSetColor(AZUL * 16 + NEGRO);
		wPch(' ');
	}
	uy = ux = 0;
	while(1)
	{
		wLocate(uy, ux * 3);
		k = GetKey();
		if(k == keycanc) { tColor = aColor; break;}
		else if(k == keyenter) { tColor = uy * 16 + ux; break;}
		else if(k == keyleft)
		{
			ux--;
			if(ux < 0) ux = 15;
		}
		else if(k == keyright)
		{
			ux++;
			if(ux > 15) ux = 0;
		}
        else if(k == keydown)
		{
			uy--;
			if(uy < 0) uy = 15;
		}
        else if(k == keyupper)
		{
			uy++;
			if(uy > 15) uy = 0;
		}
	}
	wRelease(h);
	return tColor;
}
