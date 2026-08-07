#pragma once

#include "arraylist.h"
#include "DbfBrowse.h"

using namespace System::Globalization;
using namespace System::Drawing;
using namespace System::Collections::Generic;
using namespace System::Threading;
//using namespace LibEntidades::Alberdi;

ref class Dump 
{ 
public: 
	//static DbfBrowse ^Browse = nullptr; 
	static CultureInfo ^cInfo = nullptr;
	static List<Thread ^> ^Forked = gcnew List<Thread ^>();
	static ListBrowse ^cTicket = nullptr;
	static Controles::DetPromoMedioPago ^promosmp = gcnew Controles::DetPromoMedioPago();		
		

	static List<Alberdi::TJOCommon::VerifoneCommon::TransVerifone ^> ^TarjPrisma = gcnew List<Alberdi::TJOCommon::VerifoneCommon::TransVerifone ^>();

	static List<Alberdi::TJOCommon::VerifoneCommon::TransSmartPointMp ^> ^TarjSmartPointMp = gcnew List<Alberdi::TJOCommon::VerifoneCommon::TransSmartPointMp ^>();

	static List<Alberdi::TJOCommon::ComunicaMercadoPago::OrdenCashout ^> ^listCashout = gcnew List<Alberdi::TJOCommon::ComunicaMercadoPago::OrdenCashout ^>();

	static List<Alberdi::TJOCommon::VerifoneCommon::TransPrismaECR ^> ^TarjPrismaECR = gcnew List<Alberdi::TJOCommon::VerifoneCommon::TransPrismaECR ^>();

	static Controles::MCanasta ^lascanastas = nullptr;	
	static List<Controles::PromoExtra ^> ^promosfiel = nullptr;
	static List<Controles::LeyenPromo ^> ^lstLeyenPromo = nullptr;

	static Controles::PedidoPos^ PedidoJanis = nullptr;
	//static LibEntidades::Janis::PedidoPos^ PedidoJanis = nullptr;
	
	static Controles::ClienteSql^ actCliente = nullptr;
	static List<Controles::VendedorMayor ^> ^ListadoVendedoresMay = gcnew List<Controles::VendedorMayor ^>();

	static LibEntidades::Alberdi::HeaderDoc ^docActual = nullptr;
	static LibEntidades::Alberdi::ClienteComprobante ^cliComprobante = gcnew LibEntidades::Alberdi::ClienteComprobante();

#ifdef FACTSQL
	static List<TransSql::DRegTrans ^> ^sqltrans = gcnew List<TransSql::DRegTrans ^>(); 
#endif
};

public ref class ClrLog{
	public:
		static void escribeExcepcion(System::String ^filename, System::String ^metodo, System::Exception ^excep){
			System::IO::StreamWriter^ writer = System::IO::File::AppendText(filename);		 
			System::String^ fhdatos = System::String::Format("{0} - Error capturado en metodo " + metodo, System::DateTime::Now);
			writer->WriteLine(fhdatos);
			writer->WriteLine("Mensaje: " + excep->Message);
			writer->WriteLine("StackTrace: " + excep->StackTrace);
			writer->WriteLine("----------------------------------");
			writer->Close();
		}

		static void escribeAnulado(System::String ^filename, System::Int32 cajero, System::Double montoAnul){
			System::IO::StreamWriter^ writer = System::IO::File::AppendText(filename);		 
			System::String^ fhdatos = System::String::Format("{0}\t{1}\t{2}\t{3}", System::DateTime::Now.ToString("yyyyMMdd"), 
				System::DateTime::Now.ToString("HHmmss"), cajero, montoAnul);
			writer->WriteLine(fhdatos);
			writer->Close();
		}
};
