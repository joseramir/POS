#include <stdafx.h>

#ifdef POSXP
#include "DbfBrowse.h"
#include "Buscador.h"
#include "GCStatic.h"
using namespace pos;

ref class BuscadorStatic { public: static DbfBrowse ^browser; };

Choose *opcBusq = NULL;

#endif

// Referencias externas.
extern int globalProgramPlu;					// plu.cpp
extern void (*ScannerRedirectedFunc)(char *);	// scanner.cpp
extern void pesar_plu(int);						// Balanza.cpp

void ing_codpromo(int);

//   Esquema de colores para el browse del buscador.
#ifdef PANTACOLOR
short bpluesq[16] = {           // Esquema default de colores
    { VERDE * 16 + BLANCO },    // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
    { VERDE * 16 + BLANCO },    // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color del fondo cuando usa relleno.
	{ ROJO * 16 + BLANCO },		// Color para titulos resaltados.
};
#else
short bpluesq[16] = {            // Esquema default de colores
	{ NEGRO * 16 + BLANCO },    // Color del borde
	{ BLANCO * 16 + NEGRO },    // Titulo a ventana activa.
	{ NEGRO * 16 + BLANCO },    // Titulo a ventana inactiva.
	{ NEGRO * 16 + BLANCO },    // Texto contenido.
	{ NEGRO * 16 + BLANCO },    // Color de Fondo cuando usa relleno.
	{ BLANCO * 16 + NEGRO },
};
#endif

void ScaneoDetectado(char *sc)
{
#ifdef POSXP
	BuscadorStatic::browser->Seek(Strings::CharToString(sc), "mscansc");
#endif
}

void buscaplu_(int);
void buscaplu(int cual)
{
	GCStatic::LockMainLoop("buscaplu");
	CheckNewPlu();
	buscaplu_(cual);
	GCStatic::ReleaseLockMainLoop("buscaplu");
}

extern int maxMpluPrec;

//   BuscaPlu. Presenta un browse para buscar el artículo que nos interesa
//   en forma mas amena.
//   El parametro como, nos indicara que indice se utilizara:
//			1: Scanner.
//			2: Codigo.
//          3: Descripcion.
//          4: Marca + Descripcion.
//          0: Presentar un menu para que se elija la forma de busqueda.
void buscaplu_(int como)
{
    dbf *usaDb;

	if (como == 10)
	{
		if(opcBusq == NULL)
			opcBusq	= new Choose("Opciones de Busqueda", "ESC=Cancela", 20, 0, CHOOSE_ALIGN_LEFT, NULL);
		else
			opcBusq->Clear();
		opcBusq->Add("Por SCANNER");
		opcBusq->Add("Por CODIGO");
		opcBusq->Add("Por NOMBRE");
		opcBusq->Add("Por MARCA");
		if (habilitaCargaVoucher)
			opcBusq->Add("Ingreso Codigo promo");

		como = opcBusq->Anima();
		if (como == 0)//Presiono Cancelar
			return;
		else if (como == 6)//Ingreso Cupones
		{
			ing_codpromo(0);
			return;
		}
		como--;
	}

	// Corrige como si corresponde
	if(como < 1 || como > 4) como = 3;

#ifdef POSXP
	DbfBrowse ^browser = gcnew DbfBrowse;
	BuscadorStatic::browser = browser;
	Buscador ^buscador = gcnew Buscador("");
	String ^key;
	String ^precioUsar = "precio";
	if(listap >= 2 && listap <= maxMpluPrec)
		precioUsar += listap.ToString();

	switch(como)
	{
		case 1:
			key = "pluscan";
			dbplu.Setorder("mplucod");
			dbscan.Setorder("mscansc");
			dbscan.SetRelation(&dbplu, "cod");
			usaDb = &dbscan;
			buscador->OnlyNumeric = true;
			buscador->Text = "Buscar Artículos por su Scanner";
			buscador->ActiveOrder = "mscansc";
			buscador->Completar = 13;
			buscador->CompletarCon = '0';
			browser->KeyCellIndex = 1;
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbscan), "scan", "  Scanner", 100));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cod", "Código", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "marca", "  Marca", 80, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "des", "Nombre", 220, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "stk", "Stock", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), precioUsar, "Precio", 70));
			if (modoMayor)
			{
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf1", "Frac1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio4", "P.F.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf2", "Frac2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio2", "P.F.2", 60));
				/*browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf3", "Frac3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "f3", "P.F.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb1", "Bul.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b1", "P.B.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb2", "Bul.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b2", "P.B.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb3", "Bul.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b3", "P.B.3", 60));*/
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "uni", "Un X Bulto", 70));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "pvm", "PVM", 60));
			}
			/*browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio2", "P.Mayor", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cant2", "Minimo", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio5", "P.Mejor", 60));*/

			ScannerRedirectedFunc = ScaneoDetectado;

			break;

		case 2:		// Por codigo
			key = "plucod";
			dbscan.Setorder("mscancod");
			dbplu.Setorder("mplucod");
			dbplu.SetRelation(&dbscan, "cod");
			usaDb = &dbplu;
			buscador->Text = "Buscar Artículos por su Código";
			buscador->ActiveOrder = "mplucod";
			buscador->Completar = 6;
			buscador->CompletarCon = '0';
			browser->KeyCellIndex = 0;
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cod", "Código", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbscan), "scan", "  Scanner", 100));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "marca", "  Marca", 80, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "des", "Nombre", 210, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "stk", "Stock", 70));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), precioUsar, "Precio", 70));
			if (modoMayor)
			{
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf1", "Frac1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio4", "P.F.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf2", "Frac2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio2", "P.F.2", 60));
				/*browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf3", "Frac3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "f3", "P.F.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb1", "Bul.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b1", "P.B.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb2", "Bul.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b2", "P.B.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb3", "Bul.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b3", "P.B.3", 60));*/
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "uni", "Un X Bulto", 70));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "pvm", "PVM", 60));
			}
			break;

		case 3:		// Por descripcion
			key = "pludes";
			dbplu.Setorder("mpludes");
			dbscan.Setorder("mscancod");
			dbplu.SetRelation(&dbscan, "cod");
			usaDb = &dbplu;
			buscador->Text = "Buscar Artículos por su Nombre";
			buscador->ActiveOrder = "mpludes";
			buscador->Completar = -1;
			browser->KeyCellIndex = 0;
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cod", "Código", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbscan), "scan", "  Scanner", 100));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "marca", "  Marca", 80, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "des", "Nombre", 220, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "stk", "Stock", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), precioUsar, "Precio", 70));
			if (modoMayor)
			{
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf1", "Frac1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio4", "P.F.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf2", "Frac2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio2", "P.F.2", 60));
				/*browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf3", "Frac3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "f3", "P.F.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb1", "Bul.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b1", "P.B.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb2", "Bul.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b2", "P.B.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb3", "Bul.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b3", "P.B.3", 60));*/
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "uni", "Un X Bulto", 70));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "pvm", "PVM", 60));
			}
			
			break;

		case 4:		// Por marca
			key = "plumarc";
			dbscan.Setorder("mscancod");
			dbplu.Setorder("mplumarc");
			dbplu.SetRelation(&dbscan, "cod");
			usaDb = &dbplu;
			buscador->Text = "Buscar Artículos por su Marca";
			buscador->ActiveOrder = "mplumarc";
			buscador->Completar = -1;
			browser->KeyCellIndex = 0;
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cod", "Código", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbscan), "scan", "  Scanner", 100));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "marca", "  Marca", 80, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "des", "Nombre", 220, System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "stk", "Stock", 60));
			browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), precioUsar, "Precio", 70));
			if (modoMayor)
			{
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf1", "Frac1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio4", "P.F.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf2", "Frac2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "precio2", "P.F.2", 60));
				/*browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cf3", "Frac3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "f3", "P.F.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb1", "Bul.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b1", "P.B.1", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb2", "Bul.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b2", "P.B.2", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "cb3", "Bul.3", 60));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "b3", "P.B.3", 60));*/
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "uni", "Un X Bulto", 70));
				browser->Columnas->Add(gcnew DbfBrowseColumn(IntPtr(&dbplu), "pvm", "PVM", 60));
			}
			break;
	}

	browser->Link(key, buscador->Grid, usaDb);

	if (modoMayor)
	{
		buscador->Grid->Columns[6]->Visible = false;
		buscador->Grid->Columns[7]->Visible = false;
		buscador->Grid->Columns[8]->Visible = false;
		buscador->Grid->Columns[9]->Visible = false;
		buscador->Grid->Columns[10]->Visible = true;//Visible la columna de unidades
		buscador->Grid->Columns[11]->Visible = false;
		//buscador->Grid->Columns[12]->Visible = false;
		//buscador->Grid->Columns[13]->Visible = false;
		//buscador->Grid->Columns[14]->Visible = false;
		//buscador->Grid->Columns[15]->Visible = false;
		//buscador->Grid->Columns[16]->Visible = false;
		//buscador->Grid->Columns[17]->Visible = false;
		//buscador->Grid->Columns[18]->Visible = true;
		//buscador->Grid->Columns[19]->Visible = false;
	}	
	buscador->Browser = browser;

	usaDb->Go(DB_GO_TOP);

	buscador->ShowDialog();

	if(usaDb == &dbplu)
		dbplu.ClearRelation(&dbscan);
	else
		dbscan.ClearRelation(&dbplu);
	dbplu.Setorder("mplucod");
	dbscan.Setorder("mscansc");
	ScannerRedirectedFunc = NULL;

	bool saltar = false;
	if(buscador->DialogResult == System::Windows::Forms::DialogResult::OK)
	{
		xReg = 0.0;
		double::TryParse(browser->Key, xReg);// Parse(browser->Key);
		saltar = (xReg != 0.0);
	}

	BuscadorStatic::browser = nullptr;
	delete buscador;
	delete browser;
	if(saltar && !globalProgramPlu)
	{
		char esconpeso[10];
		dbplu.Get("balanza", esconpeso);
		if (TieneBalanza && esconpeso[0] == 'T')
			pesar_plu(1);
		else
			plu(1);
	}
	
#else
	xBrowse *bPlu;
	int tecla;

	bPlu = new(xBrowse);
	switch(como)
	{
		case 1:		// Por scanner.
			dbplu.Setorder("mplucod");
			dbscan.Setorder("mscansc");
			dbscan.SetRelation(&dbplu, "cod");
			bPlu->xBrowseF(&dbscan, "scan", "   Scaner");
			bPlu->xBrowseF(&dbplu,  "cod", "Codigo");
			bPlu->xBrowseF(&dbplu,  "des", "Nombre");
			bPlu->xBrowseF(&dbplu,  "precio", "  Precio");
			usaDb = &dbscan;
			break;

		case 2:		// Por codigo
			dbscan.Setorder("mscancod");
			dbplu.Setorder("mplucod");
			dbplu.SetRelation(&dbscan, "cod");
			bPlu->xBrowseF(&dbplu,  "cod", "Codigo");
			bPlu->xBrowseF(&dbscan, "scan", "   Scaner");
			bPlu->xBrowseF(&dbplu,  "des", "Nombre");
			bPlu->xBrowseF(&dbplu,  "precio", "  Precio");
			usaDb = &dbplu;
			break;

		case 3:		// Por descripcion
			dbscan.Setorder("mscancod");
			dbplu.Setorder("mpludes");
			dbplu.SetRelation(&dbscan, "cod");
			bPlu->xBrowseF(&dbplu,  "cod", "Codigo");
			bPlu->xBrowseF(&dbscan, "scan", "   Scaner");
			bPlu->xBrowseF(&dbplu,  "des", "Nombre");
			bPlu->xBrowseF(&dbplu,  "precio", "  Precio");
			usaDb = &dbplu;
			break;

		case 4:		// Por marca
			dbscan.Setorder("mscancod");
			dbplu.Setorder("mplumarc");
			dbplu.SetRelation(&dbscan, "cod");
			//bPlu->xBrowseF(&dbplu,  "cod", "Codigo");
			bPlu->xBrowseF(&dbscan, "scan", "   Scaner");
			bPlu->xBrowseF(&dbplu,  "marca", "  Marca")->cLen = 12;
			bPlu->xBrowseF(&dbplu,  "des", "Nombre");
			bPlu->xBrowseF(&dbplu,  "precio", "  Precio");
			usaDb = &dbplu;
			break;
	}

	usaDb->Go(DB_GO_TOP);
	tecla = bPlu->xBrowseAnimate(0, usaDb, 1, 1, 0, bpluesq);
	if(usaDb == &dbplu)
		dbplu.ClearRelation(&dbscan);
	else
		dbscan.ClearRelation(&dbplu);
	dbplu.Setorder("mplucod");
	dbscan.Setorder("mscansc");
	delete bPlu;
	if(tecla == keyenter && !globalProgramPlu)
	{
		dbplu.Get("cod", msg);
		xReg = atof_(msg);
		plu(1);
	}
#endif
}
