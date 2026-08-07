/*
 *   SSaver.cpp
 *   Implementa un screen saver.
*/

#include <stdafx.h>

#define UDELAY 100

#ifndef POSXP
#include "wsup.h"

#ifdef __VSC__
//extern "C" { int _kbhit(void); }
#define kbhit _kbhit
#endif
#endif

//   Variables locales.
static time_t nextSSav;
int ssav_delay, terminateSSaver;

//   Inicializa el Screen Saver.
int InitSSaver()
{
    nextSSav = time(0) + ssav_delay;
    return 1;
}

//   Ejecuta el Screen Saver.
static char lChar[] = "תשרקקרשת";
void ScreenSaver()
{
    static int ssh, demora, idxchar, y, x, delta_y, delta_x, prev_y, prev_x;
#ifndef POSXP
	return;
    ssh = wDefine(0, 0, 25, 80, 0, 0, 0, "", "");
	wActivate(ssh);
    prev_y = y = 12;
    prev_x = x = 40;
    for(terminateSSaver = 0, demora = UDELAY, idxchar = 0; !terminateSSaver; )
    {
		if(KbHit()) terminateSSaver = 1;
        if(demora == 0)
        {
            FCursorPos(prev_y, prev_x);
            FWrtChar(0x0f20);
            FCursorPos(y, x);
            FWrtChar(0x0f00 + lChar[idxchar++]);
            if(idxchar >= 8) idxchar = 0;
            demora = UDELAY;

            //   Actualiza las coordenadas.
            if(rand() > 8000)
                delta_x = (rand() < 10000)? 0: ((rand() < 16500)? -1: 1);
            if(rand() > 8000)
                delta_y = (rand() < 10000)? 0: ((rand() < 16500)? -1: 1);
            prev_x = x;  prev_y = y;
            x += delta_x; y += delta_y;
            if(x < 1 || x >= 80)
            {
                delta_x = (-1) * delta_x;
                x += delta_x;
            }
            if(y < 1 || y >= 25)
            {
                delta_y = (-1) * delta_y;
                y += delta_y;
            }
        }
        demora--;
        tSwitch();
    }
	wRelease(ssh);
#endif
}

//   Resetea el screen Saver, para darle mas tiempo al programa.
void ResetSSaver()
{
    nextSSav = time(0) + ssav_delay;
}

//   Verifica si es tiempo de llamar al Screen Saver.
time_t actual;
void CheckSSaver()
{
    if(inOper)
    {
        nextSSav = time(0) + ssav_delay;
        return;
    }

    actual = time(0);
    if(actual >= nextSSav)
    {
        ScreenSaver();
        nextSSav = time(0) + ssav_delay;
    }
}

