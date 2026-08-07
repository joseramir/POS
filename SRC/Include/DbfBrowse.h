#pragma once

#ifdef POSXP
#include "posdb.h"
#include "string.h"
#include "pos.h"

using namespace System;
using namespace System::Threading;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;

extern int keyupper, keydown;

public ref class DbfBrowseColumn
{
private:
	StringBuilder ^sb;
	int dataSize, tempBuffSize;
	char *dataBuff, *tempBuff, *fldptr;
	dbf *data;
	
	void AllocTempBuff(int sz)
	{
		if(tempBuffSize < sz)
		{
			if(tempBuffSize >= 0) delete tempBuff;
			tempBuff = new char[sz];
			tempBuffSize = sz;
		}
	}

	String ^CharToString(char *data)
	{
		sb->Length = 0;
		while(*data)
		{
			sb->Append((wchar_t)*data);
			data++;
		}
		return sb->ToString();
	}

	char *StringToChar(String ^str)
	{
		AllocTempBuff(str->Length + 1);
		int i = 0;
		for(; i<str->Length; i++)
			tempBuff[i] = (char)str[i];
		tempBuff[i] = 0;
		return tempBuff;
	}

	void Initialize(IntPtr dbfHandler, String ^campo, String ^titulo, int columnWidth,
		DataGridViewContentAlignment align)
	{
		this->Titulo = titulo;
		this->Campo = campo;
		this->DbfHandler = dbfHandler;
		this->ColumnWidth = columnWidth;

		sb = gcnew StringBuilder;
		tempBuffSize = -1;
		data = (dbf *) dbfHandler.ToPointer();
		dataSize = data->FieldLen(StringToChar(campo));
		dataBuff = new char[dataSize + 10];
		fldptr = STRDUP(StringToChar(campo), "DbfBrowseColumn:Ctor");
		TextAlignment = align;
	}

public:
	
	void Release()
	{
		if(fldptr != NULL) { delete fldptr; fldptr = NULL; }
		if(tempBuff != NULL) { delete tempBuff; tempBuff = NULL; }
		if(dataBuff != NULL) { delete dataBuff; dataBuff = NULL; }
		if(sb != nullptr) { delete sb; sb = nullptr; }
	}

public:
	property String ^Titulo;

	property String ^Campo;

	property IntPtr DbfHandler;

	property DataGridViewColumn ^GridColumn;

	property int ColumnWidth;

	property DataGridViewContentAlignment TextAlignment;

	property String ^Value;

	void RefreshValue()
	{
		data->Get(fldptr, dataBuff);
		Value = CharToString(dataBuff);
	}

	DbfBrowseColumn(IntPtr dbfHandler, String ^campo, String ^titulo, int columnWidth,
		DataGridViewContentAlignment align)
	{
		Initialize(dbfHandler, campo, titulo, columnWidth, align);
	}

	DbfBrowseColumn(IntPtr dbfHandler, String ^campo, String ^titulo, int columnWidth)
	{
		Initialize(dbfHandler, campo, titulo, columnWidth, DataGridViewContentAlignment::MiddleCenter);
	}

	~DbfBrowseColumn()
	{
		Release();
	}
};

public ref class DbfBrowse
{
private:
	static Dictionary<String^, int> ^Images = gcnew Dictionary<String ^, int>;

	delegate void InvokeDoPosicionar();

	InvokeDoPosicionar ^doPosicionar;

	delegate void InvokeDoRefresh();

	InvokeDoRefresh ^doRefresh;

	bool plainRecnos;

	dbf *usaDb;

	int *recnos;

	char *tempBuff;

	DataGridView ^usaGrid;

	DataGridViewRow ^selectedRow;

	int pointedRecno;

	bool ignoreChangeInSelection;

	char *StringToChar(String ^str)
	{
		int i = 0;
		for(; i<str->Length; i++)
			tempBuff[i] = (char)str[i];
		tempBuff[i] = 0;
		return tempBuff;
	}

	void CellValueNeeded(Object ^sender, DataGridViewCellValueEventArgs ^e)
	{
		int ivreccount, ivgrows;		
		try
		{
			ivreccount = (int) usaDb->RecCount();
			ivgrows = usaGrid->Rows->Count;
			if(e->RowIndex >= (int)usaDb->RecCount() || e->ColumnIndex >= (int)Columnas->Count)
				e->Value = String::Empty;
			else
			{
				int usaRecno = plainRecnos? e->RowIndex + 1: recnos[e->RowIndex];
				DataGridViewRow ^row = usaGrid->Rows[e->RowIndex];
				if(row->Selected) selectedRow = row;
				pointedRecno = usaRecno;
				int prevRecno = usaDb->Recno();
				usaDb->Go(usaRecno);

				for(int colIndex = Columnas->Count - 1; colIndex >= 0; colIndex--)
					Columnas[colIndex]->RefreshValue();

				usaDb->Go(prevRecno);
				DbfBrowseColumn ^col = Columnas[e->ColumnIndex];
				e->Value = col->Value;
			}
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion CellValueNeeded", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);
			 fhdatos = System::String::Format("rowIndex = {0}, columnIndex= {1}, usaDb->RecCount() = {2}, usaGrid->Rows->Count = {3}", e->RowIndex, e->ColumnIndex, ivreccount, ivgrows);
			 writer->WriteLine(fhdatos);
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		}
		catch(...)
		{
			 WriteBootLog("ERROR - No manejado en SelectRowInGrid");		 		
		}

	}

	void SelectRowInGrid(int rowIndex)
	{
		usaGrid->SuspendLayout();
		try
		{
			//int scrollRow = rowIndex - 5;
			//if(scrollRow < 0) scrollRow = 0;
			//usaGrid->FirstDisplayedScrollingRowIndex = scrollRow;
			usaGrid->Refresh();			
			if(usaGrid->Rows->Count > 0)
			{
				ignoreChangeInSelection = true;
				usaGrid->Rows[rowIndex]->Selected = true;
				usaGrid->CurrentCell = usaGrid->Rows[rowIndex]->Cells[0];
				usaGrid->CurrentCell->Selected = true;
				ignoreChangeInSelection = false;
			}
		}		
		 catch (System::Exception^ excep)
		 {		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion SelectRowInGrid", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);
			 fhdatos = System::String::Format("rowIndex = {0}, usaGrid->Rows->Count= {1}, ignoreChangeInSelection = {2}", rowIndex, usaGrid->Rows->Count, ignoreChangeInSelection);
			 writer->WriteLine(fhdatos);
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
		 catch(...)
		 {
			 WriteBootLog("ERROR - No manejado en SelectRowInGrid");		 		
		 }
		usaGrid->ResumeLayout();
	}

	void DoPosicionar()
	{
		int ivgrows, ivdbrows, ivrecno;
		bool ivplrecno = false;
		try
		{
			// Se asegura que existan las filas necesarioas
			ivgrows = usaGrid->Rows->Count;
			ivdbrows = (int) usaDb->RecCount();
			ivrecno = (int) usaDb->Recno();
			if(plainRecnos)
			{
				ivplrecno = true;				
				while(usaGrid->Rows->Count > 0 && usaGrid->Rows->Count > (int)usaDb->RecCount())
					usaGrid->Rows->RemoveAt(usaGrid->Rows->Count - 1);
				if(usaGrid->Rows->Count <= 0)
					usaGrid->Rows->Add();
				while(usaGrid->Rows->Count < (int)usaDb->RecCount())
					usaGrid->Rows->AddCopy(0);

				int urec = (int) (usaDb->Recno()) - 1;
				if(urec >= 0 && urec <= usaGrid->Rows->Count)
					SelectRowInGrid(usaDb->Recno() - 1);
			}
			else
			{
				if(recnos != NULL)
				{
					int recFound = (int)usaDb->Recno();
					for(int *ptr = recnos, i = usaDb->RecCount() - 1; i >= 0; i--, ptr++)
					{
						if(*ptr == recFound)
						{
							i = usaDb->RecCount() - i - 1;
							if(i >= usaGrid->Rows->Count)
								i = 0;
							SelectRowInGrid(i);
							break;
						}
					}
				}
			}
			System::Windows::Forms::Application::DoEvents();
		}
		 catch (System::Exception^ excep)
		 {		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoPosicionar", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);
			 fhdatos = System::String::Format("plainrecnos = {0}, usaGrid->Rows->Count= {1}, usaDb->RecCount() = {2}, usaDb->Recno() = {3}", ivplrecno, ivgrows, ivdbrows, ivrecno);
			 writer->WriteLine(fhdatos);
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
		 catch(...)
		 {
			 WriteBootLog("ERROR - No manejado en DoPosicionar");		 		
		 }
	}

	void Posicionar()
	{
		if(usaGrid->InvokeRequired)
			usaGrid->Invoke(doPosicionar);
		else
			DoPosicionar();
	}

	void DoRefresh()
	{
		usaGrid->SuspendLayout();
		usaDb->Go(DB_GO_BOTTOM);
		Posicionar();
		usaGrid->Refresh();
		usaGrid->ResumeLayout();
	}

	void ChangeSelection(Object ^sender, EventArgs ^e)
	{
		try
		{
			if(!ignoreChangeInSelection)
			{
				if(plainRecnos)
				{
					if(usaGrid->SelectedRows->Count > 0)
						usaDb->Go(usaGrid->SelectedRows[0]->Index + 1);
					else
						usaDb->Go(DB_GO_TOP);
				}
				else
				{
					if(recnos != NULL)
					{
						int recFound = usaGrid->SelectedRows[0]->Index + 1;
						usaDb->Go(recnos[recFound - 1]);
					}
				}
			}
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ChangeSelection", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
		 catch(...)
		 {
			 WriteBootLog("ERROR - No manejado en DoPosicionar");		 		
		 }
	}

public:
	property List<DbfBrowseColumn ^> ^Columnas;

	property int KeyCellIndex;

	property String ^Key
	{
		String ^get()
		{
			if(selectedRow == nullptr)
				return String::Empty;
			return (String ^)selectedRow->Cells[KeyCellIndex]->Value;
		}
	}

	static void ClearImages()
	{
		Dictionary<String ^, int>::Enumerator enumerator = Images->GetEnumerator();
		while(enumerator.MoveNext())
			delete (void *)(enumerator.Current.Value);
		Images->Clear();
	}

	void Seek(String ^key, String ^order)
	{
		char actualKey[100];
		STRCPY(actualKey, usaDb->GetOrder());
		usaDb->Setorder(StringToChar(order));
		if(!usaDb->Seek(StringToChar(key)))
			usaDb->Go(view.recno_0);
		Posicionar();
		usaDb->Setorder(actualKey);
	}

	void Refresh()
	{
		if(usaGrid == nullptr) return;
		if(usaGrid->InvokeRequired)
			usaGrid->Invoke(doPosicionar);
		else
			DoRefresh();
	}

	void Saltear(int c)
	{
		if(selectedRow != nullptr)
		{
			int row = selectedRow->Index + c;
			if(row < 0 || row > usaGrid->Rows->Count)
				row = (c < 0)? 0: usaGrid->Rows->Count - 1;
			if(plainRecnos)
				usaDb->Go(row + 1);
			else
				usaDb->Go(recnos[row]);
			Posicionar();
		}
	}

	void Link(String ^key, DataGridView ^grid, dbf *data)
	{
		// Inicializa la base de datos.
		usaDb = data;
		usaGrid = grid;

		// Define las columnas en la grilla.
		grid->Columns->Clear();
		for(int i=0; i<Columnas->Count; i++)
		{
			DbfBrowseColumn ^dbCol = Columnas[i];
			DataGridViewTextBoxColumn ^gridCol = gcnew DataGridViewTextBoxColumn();
			grid->Columns->Add(gridCol);
			dbCol->GridColumn = gridCol;

			gridCol->Name = dbCol->Campo;
			gridCol->HeaderText = dbCol->Titulo;
			gridCol->Width = dbCol->ColumnWidth;
			gridCol->DefaultCellStyle->Alignment = dbCol->TextAlignment;
			gridCol->HeaderCell->Style->Alignment = dbCol->TextAlignment;
		}

		// Prepara para que suministremos las celdas a mano.
		grid->VirtualMode = true;
		grid->CellValueNeeded += gcnew DataGridViewCellValueEventHandler(this, &DbfBrowse::CellValueNeeded);
		grid->SelectionChanged += gcnew EventHandler(this, &DbfBrowse::ChangeSelection);

		// Crea las filas que usaremos para editar.
		grid->Rows->Add();
		int rowsNeeded = usaDb->RecCount() - 1;
		if(rowsNeeded > 0)
			grid->Rows->AddCopies(0, rowsNeeded);

		// Crea el array virtual con los nros de recno que necesitaremos
		if(key == nullptr)
			plainRecnos = true;
		else 
		{
			plainRecnos = false;
			if(Images->ContainsKey(key))
				recnos = (int *) Images[key];
			else
			{
				SendMsg("Espere unos segundos por favor.", "Inicializando el motor de Busquedas...");
				recnos = usaDb->AllIndexEntries();
				if(key != nullptr)
					Images->Add(key, (int)recnos);
				ClearMsg();
			}
		}

		// Buffer para seek
		tempBuff = new char[500];
	}

	void ClearImage(String ^key)
	{
		if(Images->ContainsKey(key))
			Images->Remove(key);
	}

	void RemoteKey(int key)
	{
		if(key == keyupper)
			Saltear(-1);
		else if(key == keydown)
			Saltear(1);
	}

	void Release()
	{
		if(Columnas != nullptr)
		{
			for(int i = 0; i < Columnas->Count; i++)
			{
				DbfBrowseColumn ^dbCol = Columnas[i];
				dbCol->Release();
			}
			Columnas->Clear();
			delete Columnas;
			Columnas = nullptr;
		}
		if(tempBuff != NULL)
		{
			delete tempBuff;
			tempBuff = NULL; 
		}
	}

	~DbfBrowse()
	{
		Release();
	}

	DbfBrowse()
	{
		pointedRecno = -1;
		Columnas = gcnew List<DbfBrowseColumn ^>;
		recnos = NULL;
		doPosicionar = gcnew InvokeDoPosicionar(this, &DbfBrowse::DoPosicionar);
		doRefresh = gcnew InvokeDoRefresh(this, &DbfBrowse::DoRefresh);
	}
};


public ref class TicketItem
{
private:
	String ^_codigo;
	String ^_nombre;
	String ^_cantidad;
	String ^_unitario;
	String ^_total;
	int _dump;
	bool _anulable;

public:
	
	property String^ Codigo
	{
		String^ get()
		{
			return _codigo;
		}
		void set(String^ x)
		{
			_codigo = x;
		}
	}

	property String^ Nombre
	{
		String^ get()
		{
			return _nombre;
		}
		void set(String^ x)
		{
			_nombre = x;
		}
	}

	property String^ Cantidad
	{
		String^ get()
		{
			return _cantidad;
		}
		void set(String^ x)
		{
			_cantidad = x;
		}
	}

	property String^ Unitario
	{
		String^ get()
		{
			return _unitario;
		}
		void set(String^ x)
		{
			_unitario = x;
		}
	}

	property String^ Total
	{
		String^ get()
		{
			return _total;
		}
		void set(String^ x)
		{
			_total = x;
		}
	}

	property int Dump
	{
		int get()
		{
			return _dump;
		}
		void set(int x)
		{
			_dump = x;
		}
	}

	property bool Anulable
	{
		bool get()
		{
			return _anulable;
		}
		void set(bool x)
		{
			_anulable = x;
		}
	}

	TicketItem()
	{
		_codigo = String::Empty;
		_nombre = String::Empty;
		_cantidad = String::Empty;
		_unitario = String::Empty;
		_total = String::Empty;
		_dump = 0;
		_anulable = false;
	}

	TicketItem(String^ cod, String^ nom, String^ cant, String^ uni, String^ tot, int dmp, bool anul)
	{
		_codigo = cod;
		_nombre = nom;
		_cantidad = cant;
		_unitario = uni;
		_total = tot;
		_dump = dmp;
		_anulable = anul;
	}

	~TicketItem()
	{
		delete _codigo;
		delete _nombre;
		delete _cantidad;
		delete _unitario;
		delete _total;		
	}
};

public ref class ListBrowse
{
private:

	DataGridView^ usaGrid;

	DataGridViewRow^ selectedRow;

	TicketItem^ actualItem;

	int indice;

	bool vacio;

	bool ignoreChangeInSelection;

	BindingList<TicketItem ^> ^ticket;

	delegate void InvokeDoAddItem(String^ cod, String^ nom, String^ cant, String^ unit, String^ tot, int ndump, bool anul);

	InvokeDoAddItem ^doAddItem;

	delegate void InvokeClearItems();

	InvokeClearItems ^doClearItems;

	delegate void InvokeDelItem (int);
	
	InvokeDelItem ^doDelItem;

	delegate void InvokeReprintItem(int, String^, String^);
	
	InvokeReprintItem ^doReprintItem;

	delegate void InvokeSaltear(int);

	InvokeSaltear ^doSaltear;

	delegate void InvokeDoRefresh();

	InvokeDoRefresh ^doRefresh;

	delegate void InvokeDelNoAnulables();

	InvokeDelNoAnulables ^doEliminaNoAnulables;

	void DoAddItem(String^ cod, String^ nom, String^ cant, String^ unit, String^ tot, int ndump, bool anul)
	{
		try
		{
			if (vacio)
			{
				vacio = false;
				ticket->Clear();
			}
			ignoreChangeInSelection = true;
			actualItem = gcnew TicketItem(cod, nom, cant, unit, tot, ndump, anul);
			ticket->Add(actualItem);
			int lastRow = usaGrid->Rows->Count;
			if(selectedRow != nullptr)
				selectedRow->Selected = false;
			selectedRow = usaGrid->Rows[lastRow - 1];
			selectedRow->Selected = true;
			usaGrid->CurrentCell = selectedRow->Cells[0];
			usaGrid->CurrentCell->Selected = true;
			ignoreChangeInSelection = false;
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoAddItem", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 WriteBootLog("Error - Ha ocurrido un error interno del POS");
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
	}

	void DoClearItems()
	{
		try
		{
			actualItem = nullptr;
			ticket->Clear();
			selectedRow = nullptr;
			vacio = true;			
			ticket->Add(gcnew TicketItem());
			indice = -1;
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoClearItems", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 WriteBootLog("Error - Ha ocurrido un error interno del POS");
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
	}

	void DoDelItem(int ndump)
	{
		int i = 0;
		bool encontrado = false;
		try
		{
			for( i = 0; i < ticket->Count; i++)
			{
				TicketItem^ titem = ticket[i];
				if (titem->Dump == ndump)
				{
					encontrado = true;
					break;
				}
			}
			if (encontrado)
			{
				selectedRow = nullptr;
				ticket->RemoveAt(i);				
				if (ticket->Count > 0)
				{
					i = (i - 1) < 0 ? 0 : (i - 1);
					actualItem = ticket[i];
					selectedRow = usaGrid->Rows[i];
					selectedRow->Selected = true;
					usaGrid->CurrentCell = selectedRow->Cells[0];
					usaGrid->CurrentCell->Selected = true;
				}
			}
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoDelItem", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 WriteBootLog("Error - Ha ocurrido un error interno del POS");
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
	}

	void DoDelNoAnulables()
	{
		int i = 0;
		bool encontrado = false;
		double vitem = 0;
		try
		{
			for( i = ticket->Count - 1; i > 0; i--)
			{
				TicketItem^ titem = ticket[i];				
				if (titem->Anulable)
				{
					System::Double::TryParse(titem->Total, vitem);
					if (vitem < 0)
						ticket->RemoveAt(i);
				}
			}
			
			selectedRow = nullptr;							
			if (ticket->Count > 0)
			{
				i = ticket->Count - 1;
				actualItem = ticket[i];
				selectedRow = usaGrid->Rows[i];
				selectedRow->Selected = true;
				usaGrid->CurrentCell = selectedRow->Cells[0];
				usaGrid->CurrentCell->Selected = true;
			}
			
		}
		catch (System::Exception^ excep)
		{		 
			System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoDelItem", System::DateTime::Now);
			writer->WriteLine(fhdatos);		 
			writer->WriteLine("Mensaje: " + excep->Message);
			writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			writer->WriteLine("----------------------------------");
			writer->Close();
			WriteBootLog("Error - Ha ocurrido un error interno del POS");
			ClearMsg();
			Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		}
	}

	///Reimprime en la grilla el articulo indicado mediante el dump, modifica el Prec unit y total	
	void DoReprintItem(int ndump, String^ unit, String^ tot)
	{
		TicketItem^ titem;
		bool encontrado = false;
		try
		{
			for(int i = 0; i < ticket->Count; i++)
			{
				titem = ticket[i];
				if (titem->Dump == ndump)
				{
					encontrado = true;
					break;
				}
			}
			if (encontrado)
			{
				titem->Unitario = unit;
				titem->Total = tot;
			}
			titem = nullptr;
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion DoReprintItem", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 WriteBootLog("Error - Ha ocurrido un error interno del POS");
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
	}

	void DoSaltear(int c)
	{
		ignoreChangeInSelection = true;
		if(selectedRow != nullptr)
		{
			int row = selectedRow->Index + c;
			if(row < 0 || row >= usaGrid->Rows->Count)
				row = (c < 0)? 0: usaGrid->Rows->Count - 1;
			selectedRow->Selected = false;
			selectedRow = usaGrid->Rows[row];
			actualItem = ticket[row];
			selectedRow->Selected = true;
			usaGrid->CurrentCell = selectedRow->Cells[0];
			usaGrid->CurrentCell->Selected = true;
		}
		ignoreChangeInSelection = false;
	}

	void ChangeSelection(Object ^sender, EventArgs ^e)
	{
		try
		{
			if(!ignoreChangeInSelection)
			{
				if (usaGrid->SelectedRows->Count > 0 && ticket->Count > 0)
				{
					selectedRow = usaGrid->SelectedRows[0];
					int row = selectedRow->Index;					
					selectedRow->Selected = true;				
					actualItem = ticket[row];
				}				
			}
		}
		catch (System::Exception^ excep)
		{		 
			 System::IO::StreamWriter^ writer = System::IO::File::AppendText("errormain.txt");
			 System::String^ fhdatos = System::String::Format("{0} - Error capturado en funcion ChangeSelection", System::DateTime::Now);
			 writer->WriteLine(fhdatos);		 
			 writer->WriteLine("Mensaje: " + excep->Message);
			 writer->WriteLine("StackTrace: " + excep->StackTrace);			 
			 writer->WriteLine("----------------------------------");
			 writer->Close();
			 ClearMsg();
			 Alert("Ha ocurrido un error Interno del POS", "Si el problema continua reinicie el equipo.");
		 }
		 catch(...)
		 {
			 WriteBootLog("ERROR - No manejado en DoPosicionar");		 		
		 }
	}

public:
	property List<DataGridViewTextBoxColumn ^> ^Columnas;

	ListBrowse()
	{
		selectedRow = nullptr;
		indice = -1;
		Columnas = gcnew List<DataGridViewTextBoxColumn ^>();
		ticket = gcnew BindingList<TicketItem ^>();
		vacio = true;
		ticket->Add(gcnew TicketItem());
		doAddItem = gcnew InvokeDoAddItem(this, &ListBrowse::DoAddItem);
		doClearItems = gcnew InvokeClearItems(this, &ListBrowse::DoClearItems);
		doDelItem = gcnew InvokeDelItem(this, &ListBrowse::DoDelItem);
		doReprintItem = gcnew InvokeReprintItem(this, &ListBrowse::DoReprintItem);
		doSaltear = gcnew InvokeSaltear(this, &ListBrowse::DoSaltear);
		doRefresh = gcnew InvokeDoRefresh(this, &ListBrowse::DoRefresh);
		doEliminaNoAnulables = gcnew InvokeDelNoAnulables(this, &ListBrowse::DoDelNoAnulables);
	}

	void DoRefresh()
	{
		usaGrid->Refresh();
		ticket->ResetBindings();
		usaGrid->ResetBindings();
	}

	void Refresh()
	{
		//usaGrid->Refresh();
		if(usaGrid == nullptr) return;
		if(usaGrid->InvokeRequired)
			usaGrid->Invoke(doRefresh);
		else
		{
			usaGrid->Refresh();
			ticket->ResetBindings();
			usaGrid->ResetBindings();
		}
	}	

	void Link(DataGridView^ grilla)
	{
		usaGrid = grilla;
		usaGrid->Rows->Clear();
		usaGrid->Columns->Clear();
		for each(DataGridViewTextBoxColumn^ col in Columnas)
		{
			usaGrid->Columns->Add(col);
		}
		ticket->Clear();
		usaGrid->AutoGenerateColumns = false;
		usaGrid->MultiSelect = false;
		usaGrid->DataSource = ticket;		
		usaGrid->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
		usaGrid->SelectionChanged += gcnew EventHandler(this, &ListBrowse::ChangeSelection);
	}

	void RemoteKey(int key)
	{
		if(key == keyupper)
			usaGrid->Invoke(doSaltear, gcnew array<Object ^>{-1});
		else if(key == keydown)
			usaGrid->Invoke(doSaltear, gcnew array<Object ^>{1});
	}

	void AddColumn(String^ htitulo, String^ hmapping, DataGridViewContentAlignment alinea, int width)
	{
		DataGridViewTextBoxColumn^ col = gcnew DataGridViewTextBoxColumn();
		col->DataPropertyName = hmapping;
		col->HeaderText = htitulo;
		col->DefaultCellStyle->Alignment = alinea;
		col->Width = width;
		Columnas->Add(col);
	}

	void Agrega(TicketItem^ unTicket)
	{
		ticket->Add(unTicket);
	}

	void Agrega(String^ cod, String^ nom, String^ cant, String^ unit, String^ tot, int ndump, bool anul)
	{
		usaGrid->Invoke(doAddItem, gcnew array<Object ^>{cod, nom, cant, unit, tot, ndump, anul});		
	}

	void LimpiarTicket()
	{
		if (this != nullptr)		
			usaGrid->Invoke(doClearItems);
	}	

	void EliminaItem(int ndump)
	{
		usaGrid->Invoke(doDelItem, gcnew array<Object ^>{ndump});		
	}

	void LimpiarNoAnulables()
	{
		usaGrid->Invoke(doEliminaNoAnulables);
	}

	void ReimprimeItem(int ndump, String^ unit, String^ tot)
	{
		usaGrid->Invoke(doReprintItem, gcnew array<Object ^> {ndump, unit, tot});
	}
	
	void Clear()
	{
		ticket->Clear();
	}

	TicketItem^ Item()
	{
		TicketItem^ item = nullptr;
		if (selectedRow != nullptr)
		{
			item = gcnew TicketItem();
			item->Codigo = actualItem->Codigo;
			item->Nombre = actualItem->Nombre;
			item->Cantidad = actualItem->Cantidad;
			item->Unitario = actualItem->Unitario;
			item->Total = actualItem->Total;
			item->Dump = actualItem->Dump;
			item->Anulable = actualItem->Anulable;
			if (!item->Anulable)
			{
				item->Anulable = ticket->Count > 1;
			}
		}
		
		return item;
	}

	int RecCount()
	{
		return ticket->Count;
	}

};
#endif
