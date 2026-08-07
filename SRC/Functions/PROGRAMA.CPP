//
//   Programa.cpp
//   Cambia el contenido de algunos archivos.
//

#include "stdafx.h"

static Choose *hMenu = NULL;

//  esquema de colores para la programacion.
short prograEsq[16] = {         // Esquema de colores para choose.
	{ CELESTE * 16 + NEGRO },   // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ CELESTE * 16 + NEGRO },   // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno
	{ ROJO * 16 + BLANCO }      // Color cuando quiere resaltar el texto.
};

//   Funcion que es llamada por el despachador de teclas.
void programa(int dummy)
{
	int xSel;

	if(inOper)
	{
		Alert("No puede cambiar a PROGRAMA en el", "medio de un ticket!!!");
		return;
	}

	//   Mira si nos mandamos al modo Debug directamente
	if(dummy == 999)
	{
		pf->Debug();
		return;
	}

	//   Verifica la clave.
	if(!RequestPasswd("PROG")) return;
	
	//  Envia a pantalla el menu con las opciones.
	for( ;; )
	{
		if(hMenu == NULL)
		{
			hMenu = new Choose("Programa", "", 0, 0, CHOOSE_ALIGN_LEFT, NULL);
			hMenu->Add("Articulos");
			hMenu->Add("Departamentos");
			hMenu->Add("Mozos / Vendedores");
			hMenu->Add("Cajeros");
			hMenu->Add("Medios de Pago");
			hMenu->Add("Funciones Varias");
			hMenu->Add("Reindexa Articulos");
		}
		xSel = hMenu->Anima();
		if(xSel < 1 || xSel > 7) break;
		switch(xSel)
		{
			case 1:					// Articulos.
				PrograPlu();
				break;

			case 2:					// Departamentos.
				PrograDeptos();
				break;

			case 3:					// Mozos / Vendedores.
				PrograVended();
				break;

			case 4:					// Cajeros.
				PrograClk();
				break;

			case 5:					// Medios de Pago
				PrograMpag();
				break;

			case 6:					// Funciones Varias.
				break;

			case 7:					// Reindexado de Articulos.
				ForcePluReindex();
				break;
		}
	}
    if(UsaLlave) _outp(0x60, 0xe7);            // Pide posicion de la llave.
}

