/*
 *   PPR4.c
 *   Driver del impresor HASAR PR4F Fiscal.
*/

#include <stdafx.h>

//#include <fstream>
//#include <iostream> // Para entrada/salida de consola (cout, cerr)
//#include <fstream>  // Para operaciones con archivos (ifstream)
//#include <string>   // Para std::string
//#include <cstdio>   // Para la función std::remove (borrar archivos)
#include <ctime>

using namespace std;

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif


PrinterTermica::PrinterTermica():myPrinter(NULL)
{
	//myPrinter = new ThermalPrinter():myPrinter(NULL);
}

// Helper: mostrar error y retornar false
bool PrinterTermica::CheckResult(int result)
{
	if (result != 0)
	{
		//printf("ERROR de impresion: %s\n", PrintBridge_LastError());
		return false;
	}
	return true;
}


//char *PrinterTermica::splitChar(char **text)
//{
//		char *r;
//	if(**text == SEP) 
//		(*text)++; 
//	if(**text == 0 ) 
//		return "";
//	for(r = mfld; **text != SEP || **text == 0; )
//	{
//		if (**text == 0)
//			break;
//		else
//		{
//			*r++ = **text;
//			(*text)++;			
//		}
//	}
//	*r = 0;
//	return mfld;
//}

////   Extrae un campo y lo procesa como una constante hexadecimal.
//unsigned long PrinterTermica::sgethexa(char **pc)    
//{
//	unsigned long res;
//	char *p;
//
//	for(p = splitChar(pc), res = 0l; isxdigit(*p); p++)
//	{
//		res *= 16;
//		res += (*p <= '9')? (*p - '0') : (
//			(*p <= 'F')? (*p - 'A' + 10): (*p - 'a' + 10));
//	}
//	return res;
//}

////   Extrae un campo y lo procesa como una constante decimal.
//unsigned long PrinterTermica::sgetdec(char **pc)     // Lee un nro expresado decimal
//{
//	unsigned long res;
//	char *p;
//
//	for(p = splitChar(pc), res = 0l; isdigit(*p); p++)
//	{
//		res *= 10;
//		res += *p - '0';
//	}
//	return res;
//}

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
char *PrinterTermica::ChkDesP(char *s, char *dst)
{
	if(!_stricmp(s, "..."))
		return " ";
	if(dst == NULL && strlen(s) == 0) 
		return s;
	if(dst == NULL) 
		dst = s;
	char *ret = dst;
	int maxLen = MAXLENCAR;
	while(*s && (maxLen-- != 0))
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			if( *(s + 1) == '4')
			{
				s++;
			}
		}
		else
		{
			if((*s != 'ñ' && *s != 'Ñ' && *s != 'ü' && *s != 'á' && *s != 'é' && *s != 'í' && *s != 'ó' && *s != 'ú' && *s != 'Á' && *s != 'É' && *s != 'Í' && *s != 'Ó' && *s != 'Ú') && *s < ' ' || *s > 'z' && (*s != '[' && *s != ']'))			
				*dst++ = ' ';
			else
				*dst++ = *s;
		}
		s++;
	}

	if(strlen(ret) > MAXLENCAR) 
		*(ret + MAXLENCAR) = 0;
	*dst = 0;
	return ret;
}

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
char *PrinterTermica::ChkDesP2(char *s, char *dst)
{
	if(dst == NULL && strlen(s) == 0) return s;
	if(dst == NULL) dst = s;
	char *ret = dst;
	int maxLen = 41;
	while(*s && (maxLen-- != 0))
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			if( *(s + 1) == '4')
			{				
				s++;
			}
		}
		else
		{
			if(*s < ' ' || *s > 'z' && (*s != '[' && *s != ']')){
				if (*s == 'Ñ')
					*dst++ = 'N';
				else if (*s == 'ñ')
					*dst++ = 'n';
				else if (*s == 'Á')
					*dst++ = 'A';
				else if (*s == 'É')
					*dst++ = 'E';
				else if (*s == 'Í')
					*dst++ = 'I';
				else if (*s == 'Ó')
					*dst++ = 'o';
				else if (*s == 'ú')
					*dst++ = 'u';
				else if (*s == 'á')
					*dst++ = 'a';
				else if (*s == 'é')
					*dst++ = 'e';
				else if (*s == 'í')
					*dst++ = 'i';
				else if (*s == 'ó')
					*dst++ = 'o';
				else if (*s == 'ú')
					*dst++ = 'u';
				else
					*dst++ = ' ';
			}
			else
				*dst++ = *s;
		}
		s++;
	}

	if(strlen(ret) > 40) *(ret + 40) = 0;
	*dst = 0;
	return ret;
}

// Chequea las descripciones. (Que sean ascii, que no tenga 'ñ', etc).
bool PrinterTermica::ChkTextGran(char *s)
{
	bool baux = false;
	if(strlen(s) == 0) return baux;
	//if(dst == NULL) dst = s;
	//char *ret = dst;
	int maxLen = MAXLENCAR;
	while(*s && (maxLen-- != 0))
	{
		// Caracteres de control?
		if((*s == '@' || *s == '&') && (*(s + 1) != 0))
		{
			if( *(s + 1) == '4')
			{
				//*dst++ = '\xf4';
				s++;
				baux = true;
			}
		}		
		s++;
	}	
	return baux;
}



// Retorna datos del propietario del impresor
void PrinterTermica::Owner(char *cuit, char *pv)
{
	SetHeader("");
	SetPie("");
	
	cuit[0] = '2'; cuit[1] = '0';
	cuit[2] = '1'; cuit[3] = '2'; cuit[4] = '3'; cuit[5] = '4' ;cuit[6] = '5'; cuit[7] = '6'; cuit[8] = '7'; cuit[9] = '8';
	cuit[10] = '6'; cuit[11] = 0;
	//pv[0] = pv[1] = pv[2] = '0'; pv[3] = '1'; pv[4] = 0;
	pv[0] = pv[1] = pv[2] = '0'; pv[3] = '0'; pv[4] = '0';pv[5] = 0;
	STRCPY(ptovta, pv);
	
	return;
}

//  Manda al impresor algun comando de configuracion.
void PrinterTermica::Configure(char *confParam)
{
	char tmpPkt[50];
	if (strlen(confParam) > 40)
		confParam[40] = 0;
	/*sprintf(tmpPkt, "d%c7%c%s", SEP, SEP, confParam);	
	EnviaRecibePaquete(tmpPkt);*/
}

//  Verifica la disponibilidad del impresor y de la linea serial.
int PrinterTermica::ChkPrnXonXoff()
{
	int rtachk = 0;
	{
		//cfg = {};
		cfg.connection_type = CONN_NETWORK;
		
		cfg.network_host = "192.168.0.40";

		cfg.network_port = 9100;
		cfg.print_width     = 48;
		cfg.auto_cut        = 1;
		cfg.supports_qr     = 1;
		cfg.qr_module_size  = 8;
		cfg.logo_scale		= 0.75f;

		myPrinter = new ThermalPrinter(cfg);

		rtachk = 0;
	}while (rtachk == 1)
	
	return 0;
}

void PrinterTermica::CheckStatus()
{
	
	//EnviaRecibePaquete("\x2A");
	//char *pc = bufImpre;// rxBuff + 1;
	//lstat.prnstat = sgethexa(&pc);// gethexa(&pc);
	//lstat.fiscstat = sgethexa(&pc);

	//this->EstadoAuxiliar = sgethexa(&pc);

	//sgetdec(&pc);					// Estado Interno
	//TipoComprobante = sgetdec(&pc);	// Codigo comprobante en curso
	//sgetdec(&pc);					// Codigo comprobante informado
	//NumeroComprobante = sgetdec(&pc);	//Numero de Comprobante

	//chkFiscStat();
}

// Devuelve el ultimo numero de comprobante emitido.
//    tipo: 1 = ticket b/c factura b/c
//    tipo: 2 = ticket 'a' o factura 'a'
int PrinterTermica::GetTickNro(int cual)
{
	//codigos de comprobante 
	//81 Tique FActura A
	//82 Tique Factura B
	//83 Tique

	//// Envia el comando.
	//ignoraError4 = 1;
	////Si tipoComprobante no tiene asignado, se lo setea a 83=Ticket comun
	//if (TipoComprobante == 0)
	//	TipoComprobante = 83;
	//sprintf(txbuff, "*%c%d", SEP, TipoComprobante); //(cual == 2 ? 81 : 82));
	////int result = xchgPkt(txbuff);
	//EnviaRecibePaquete(txbuff);
	//char *pc = bufImpre;// rxBuff + 1;
	//ignoraError4 = 0;
	//

	////   Desensambla los distintos campos del estado.
	//sgethexa(&pc);					// Estado de la impresora
	//sgethexa(&pc);					// Estado Fiscal

	//sgetdec(&pc);					// Estado auxiliar
	//sgetdec(&pc);					// Estado Interno
	//sgetdec(&pc);					// Codigo comprobante en curso
	//sgetdec(&pc);					// Codigo comprobante informado
	//NumeroComprobante = sgetdec(&pc);	//Numero de Comprobante
	NumeroComprobante = 1;
		
	return NumeroComprobante;
}

//   Abre la gaveta (Cajon de dinero). Utilizamos para esta tarea el dispositivo presente 
//   en el impresor PR4 para activarla.
int PrinterTermica::OpenDrwr()
{
	//EnviaRecibePaquete("\x7b");
	return 0;
}

// Completa la inicializacion que es especifica del modelo.
void PrinterTermica::CompleteInitialization()
{
	double llimfiscal = 0;
	//// Envia el comando.	
	//sprintf(txbuff, "\xae%c%c", SEP, 'B');			
	//EnviaRecibePaquete(txbuff);

	////   Desensambla los distintos campos del estado.	
	//pc = bufImpre;// rxBuff + 1;
	//sgethexa(&pc);					// prnstat
	//sgethexa(&pc);					// fiscstat
	//llimfiscal = sgetdec(&pc);	// Limite Tique-Factura
	//sprintf(txbuff, "Limite Fiscal: %.2lf", llimfiscal);
	//WriteBootLog(txbuff);
	//if (llimfiscal == 0) //No tiene limite fiscal se coloca el que esta configurado
	//	llimfiscal = limTicket;
	//if (checkLimFisc)	
	//	limTicket = llimfiscal;
	//else
	//{
	//	if (limTicket > llimfiscal)
	//		limTicket = llimfiscal;
	//}
	//txbuff[0] = 0;
	sprintf(txbuff, "Limite Fiscal: %.2lf NO TIENE LIMITE TERMICA", llimfiscal);
	WriteBootLog(txbuff);
	TipoComprobante = 82; //Setea el tipo de Comprobante por defecto a Tique Factura
}

char *PrinterTermica::HasarBrand()
{
	return "Espere. Cargando el impresor TERMICA...";
}

//   Descarga en el printer todos los datos instalables.
int PrinterTermica::PrinterUpLoad()
{
	int i;
	abierto = false;
	MAXLENCAR = 48;//maximo de caracteres por linea

    //  Activa las capacidades del impresor.
    //PrinterCap = PRN_FISCAL | PRN_TICKFAC | PRN_SYNC | PRN_IDENTIFY;	
	DeInitSerial(COM_PRN);
	handImpre = -1000;
	SendMsg(HasarBrand(), "");
    ChkPrnXonXoff();
	recHeader.store_name = LogoLines[0];
	//  Primero, instala los encabezamientos.
	for(i=1; i< cLinesLogo; i++)
	{
		recHeader.lines_header.push_back(LogoLines[i]);
        /*sprintf(txbuff, "\x9E%c%d%c00%c%s%cD%cO", SEP, i, SEP, SEP, ChkDesP(Trim(LogoLines[i]),0), SEP, SEP);
		EnviaRecibePaquete(txbuff);*/
	}
	availHeaderLine = i;	

	//  Ahora, instala las definiciones de PIE de ticket.
	for(i=0; i<cLinesPie; i++)
	{
		/*sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, i+1, SEP, (ChkTextGran(LogoPie[i]) ? "0A": "00"), SEP, ChkDesP(LogoPie[i],0), SEP, SEP);		
		EnviaRecibePaquete(txbuff);*/
		recFooter.lines_footer.push_back(LogoPie[i]);
	}
	recFooter.qr_url = "http://qr.afip.gob.ar/?qr=f5XufOZOX-nC9V_vJdkMfQ,,";

	//NUEVO agregado
	//CargaImagenLogo();
	recHeader.LoadLogoFromBMP("pos\\logo.bmp");

	CompleteInitialization();
    ClearMsg();
	return 0;
}

int PrinterTermica::CargaImagenLogo(){
	std::string filename = "pos\\logo.txt"; 
	// Asegúrate de que este archivo exista para la prueba.
	// ¡Ten en cuenta que el programa lo borrará al finalizar!

	// --- 1. Controlar la existencia del archivo y su accesibilidad ---
	// La forma estándar y portable en C++ de verificar si un archivo "existe"
	// y es "accesible" para lectura es intentar abrirlo y luego verificar si
	// la operación de apertura fue exitosa.
	//std::ifstream inputFile("pos\\logo.txt", std::ios::binary);

	//if (!inputFile.is_open()) {
	//	/*std::cerr << "Error: No se pudo abrir el archivo '" << filename << "'." << std::endl;
	//	std::cerr << "Asegúrate de que el archivo existe en la ruta correcta "
	//		<< "y que tienes permisos de lectura." << std::endl;*/
	//	WriteBootLog("El archivo de LOGO no existe o no se puede abrir");
	//	return 1; // Retorna un código de error
	//}

	//// --- Si llegamos aquí, el archivo se abrió correctamente y existe/es accesible ---

	//char buffer[CHUNK_SIZE + 1]; // Buffer para almacenar cada fragmento. +1 para el terminador nulo '\0'.
	//int fragmentCount = 0;
	//bool isFirstChunk = true; // Bandera para identificar el primer fragmento

	///*std::cout << "Archivo '" << filename << "' abierto exitosamente." << std::endl;
	//std::cout << "Leyendo en fragmentos de " << CHUNK_SIZE << " caracteres (bytes):\n";*/

	//// --- 2. Leer el archivo en bucle, fragmento a fragmento ---
	//while (true) {
	//	// Leer CHUNK_SIZE bytes en el buffer
	//	inputFile.read(buffer, CHUNK_SIZE);

	//	// gcount() devuelve el número de caracteres (bytes) realmente leídos por la última operación.
	//	std::streamsize bytesRead = inputFile.gcount();

	//	// Si no se leyó ningún byte, hemos llegado al final del archivo o hubo un error.
	//	if (bytesRead == 0) {
	//		break; // Salir del bucle
	//	}

	//	// Colocar un terminador nulo al final de los bytes leídos.
	//	buffer[bytesRead] = '\0'; 

	//	// --- 3. Procesar y mostrar el fragmento (diferenciando el primero) ---
	//	if (isFirstChunk) {
	//		//std::cout << "\n--- PRIMER FRAGMENTO (Tamaño: " << bytesRead << " bytes) ---\n";
	//		sprintf(txbuff, "\x90%c%c%c%s", SEP, 'I', SEP, buffer);
	//		EnviaRecibePaquete(txbuff);
	//		isFirstChunk = false; // Desactivar la bandera para los siguientes fragmentos
	//	} else if (bytesRead < CHUNK_SIZE){
	//		//std::cout << "\n--- FRAGMENTO " << ++fragmentCount << " (Tamaño: " << bytesRead << " bytes) ---\n";
	//		sprintf(txbuff, "\x90%c%c%c%s", SEP, 'F', SEP, buffer);
	//		EnviaRecibePaquete(txbuff);
	//	}else{
	//		sprintf(txbuff, "\x90%c%c%c%s", SEP, 'C', SEP, buffer);
	//		EnviaRecibePaquete(txbuff);
	//	}


	//	//std::cout << buffer; // Imprimir el contenido del fragmento

	//	// El último fragmento (que puede ser menor de 128) se mostrará correctamente
	//	// porque 'bytesRead' solo contendrá la cantidad de bytes realmente leídos.
	//	// La condición 'if (bytesRead == 0)' garantiza que no se procese un fragmento vacío.

	//	// Si se alcanzó el final del archivo durante esta lectura, salir después de procesar.
	//	if (inputFile.eof()) {
	//		// Este es el último fragmento si la lectura actual alcanzó el EOF.
	//		// Se imprimirá su contenido y luego se saldrá del bucle.
	//		break; 
	//	}
	//}

	////std::cout << "\n\n--- Fin de la lectura del archivo ---\n";

	//// --- 4. Cerrar el archivo ---
	//// Es buena práctica cerrar explícitamente, aunque el destructor de ifstream lo haría automáticamente.
	//inputFile.close(); 

	//// --- 5. Borrar el archivo ---
	////std::cout << "Intentando borrar el archivo '" << filename << "'..." << std::endl;
	//if (std::remove(filename.c_str()) == 0) {
	//	//std::cout << "El archivo se ha borrado exitosamente." << std::endl;
	//	WriteBootLog("Archivo de LOGO borrado");
	//} else {
	//	// std::perror("remove"); // Descomentar para obtener un mensaje de error del sistema más detallado
	//	/*std::cerr << "Error: No se pudo borrar el archivo '" << filename << "'." << std::endl;
	//	std::cerr << "Asegúrate de que no está siendo usado por otro programa y que tienes permisos de escritura/borrado." << std::endl;*/
	//	WriteBootLog("No se pudo borrar archivo de LOGO");
	//	return 1; // Retorna un código de error
	//}

	return 0; // Retorna 0 para indicar éxito

}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterTermica::SetPie(char *msg)
{
	char tmsg[80];

	/*ChkDesP(msg, tmsg);	
	sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, 2, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);		
	EnviaRecibePaquete(txbuff);*/
	recFooter.lines_footer[1] = msg;
	return 0;
}

//   Cambia el mensaje de la ultima linea del PIE del ticket.
int PrinterTermica::SetPie(char *msg, int linea)
{
	char tmsg[80];

	//ChkDesP(msg, tmsg);
	switch (linea)
	{
		case 1:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, 1, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[1] = msg;
			break;
		case 2:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, 2, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[2] = msg;
			break;
		case 3:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, 3, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[3] = msg;
			break;
		case 11:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 1, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[1] = msg;
			break;
		case 12:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 2, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[2] = msg;
			break;
		case 13:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 3, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[3] = msg;
			break;
		default:			
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, 1, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " " : ChkDesP(tmsg, 0), SEP, SEP);	
			recFooter.lines_footer[1] = msg;
			break;
	}
	
	//EnviaRecibePaquete(txbuff);
	pieChanged = true;
	return 0;
}


//  Cambia el mensaje de la ultima linea del HEADER del ticket.
int PrinterTermica::SetHeader(char *msg)
{
	char tmsg[80];

	ChkDesP(msg, tmsg);
	if(availHeaderLine > 10) availHeaderLine = 10;	
	//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cH", SEP, 1, SEP, (ChkTextGran(msg) ? "0A": "00"), SEP, (strlen(tmsg) == 0)? " ": ChkDesP(tmsg, 0),SEP, SEP);	
	//EnviaRecibePaquete(txbuff);
	recibo.header.cashier = msg;
	return 0;
}

//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
//   Imprime el encabezamiento de un ticket / ticket factura.
//char hdrPkt[400], c_cuit[50], tDoc[10];
int PrinterTermica::PrintEncab()
{
	char hdrPkt[400], tmpChar[30], *p;
	int validarCuit = 1;
	TipoComprobante = EsNotaCredito ? 113 : 82;//No hay ticket comun solo facturas
	//char ncliente[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
    
	std::time_t now = std::time(NULL);
	std::tm *ltm = std::localtime(&now);

	recibo.header = recHeader;
	recibo.footer = recFooter;
	
	sprintf(tmpChar, "%d", caja);
	recibo.header.cash_register = tmpChar;
	std::strftime(tmpChar, sizeof(tmpChar), "%d/%m/%Y %H:%M:%S", ltm);
	recibo.header.receipt_date = tmpChar;
	//recibo.header.cashier = clkName();
	PrintClerk();               // Imprime los datos del cajero.
	SetPie("");
    if(GlobalTF)
    {
        //STRCPY(tDoc, "CUIT");
		  STRCPY(c_cuit, c_numero);
        if(!_stricmp(c_condiva, "RI"))
        {
            f_letra =  'A';
            f_respon = 'I';
			TipoComprobante = EsNotaCredito ? 112 : 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "NI"))
        {
            f_letra =  'A';
            f_respon = 'T';
			TipoComprobante = EsNotaCredito ? 112 : 81; //tipocomp = 81;
        }
        else if(!_stricmp(c_condiva, "EX"))
        {
            f_letra =  'B';
            f_respon = 'E';
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
        }
        else if(!_stricmp(c_condiva, "NR"))
        {
            STRCPY(c_numero, "20-11111111-2");
            f_letra =  'B';
            f_respon = 'A';
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
			validarCuit = 0;
        }
        else if(!_stricmp(c_condiva, "MO"))
        {
			//f_letra =  'B';
			//f_respon = 'M';
			//TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
			//Cambia como si fuese un Responsable Inscripto para hacer Factura A
			f_letra =  'A';
			f_respon = 'I';
			TipoComprobante = EsNotaCredito ? 112 : 81; //tipocomp = 81;
			SetPie("El credito fiscal discriminado en el presente comprobante, solo", 11);
			SetPie("podra ser computado a efectos del Regimen de Sostenimiento e", 12);
			SetPie("Inclusion Fiscal para Pequeños Contribuyentes de la Ley 27.618", 13);
        }
        else        // Consumidor final.
        {
            STRCPY(c_numero, "20-11111111-2");			
			/*if(strlen(c_cuit) == 0)/// || strlen(c_cuit) > 8)
				STRCPY(c_cuit, "11111111");*/
            f_letra =  'B';
            f_respon = 'C';
			TipoComprobante = EsNotaCredito ? 113 : 82; //tipocomp = 82;
			validarCuit = 0;
        }

		//   Corrige la variable GlobalTF de acuerdo con el comprobante abierto
		GlobalTF = (f_letra == 'A')?1 : 2;

        //   Prepara el cuit.		
        Trim(c_cuit);
        for(p = c_cuit; *p; )
            if(*p == '-')
                STRCPY(p, p+1);
            else
                p++;
		
		if(validarCuit && !vCuit(c_numero))
		{
			Alert("El CUIT del cliente no es valido.", "Se emitira un ticket comun...");
			GlobalTF = 0;
			TipoComprobante = 82;
			/*sprintf(hdrPkt, "\x40%c%d", SEP, TipoComprobante);
			EnviaRecibePaquete(hdrPkt);*/
			STRCPY(GlobalComprob, "TI");

			/*char *pc = bufImpre;
			splitChar(&pc);
			splitChar(&pc);
			NumeroComprobante = sgetdec(&pc);
			pc = NULL;*/
		}
		else
		{
			//   Prepara el resto de los campos.
			//STRCPY(ncliente, c_nombre);
			ChkDesP2(c_nombre, NULL);
			ChkDesP2(c_direcc, NULL);
			Trim(c_nombre);			
			Trim(c_direcc);
			recibo.header.nom_cliente = c_nombre;
			recibo.header.dir_cliente = c_direcc;
			recibo.header.cod_cliente = c_cod;


			//   Manda los datos del cliente.
			//PrepareCustomerData(hdrPkt);
			//if((lstat.fiscstat & 0x8000) != 0)
			//{
			//	ConsultaUltimoError();
			//	Alert("Error - Verificar datos Obligatorios CUIT, Nombre y Direccion", "Se imprimira un ticket comun");
			//	TipoComprobante = EsNotaCredito ? 110 : 83;
			//	sprintf(hdrPkt, "\x40%c%d", SEP, TipoComprobante);
			//}
			//else
			//	sprintf(hdrPkt, "\x40%c%d", SEP, TipoComprobante);// tipocomp);				

			//EnviaRecibePaquete(hdrPkt);
			
			/*char *pc = bufImpre;
			splitChar(&pc);
			splitChar(&pc);
			NumeroComprobante = sgetdec(&pc);
			pc = NULL;

			if (chkFiscStat() == 0)	
			{			
				GlobalComprob[0] = 'F';
				GlobalComprob[1] = f_letra;
				GlobalComprob[2] = 0;
			}*/
		}
    }
    else
    {
  //      sprintf(hdrPkt, "\x40%c%d", SEP , TipoComprobante); // tipocomp);
  //      EnviaRecibePaquete(hdrPkt);
		//
		//char *pc = bufImpre;
		//splitChar(&pc);
		//splitChar(&pc);
		//NumeroComprobante = sgetdec(&pc);
		//pc = NULL;

		//if (chkFiscStat() == 0)		
		//	STRCPY(GlobalComprob, "TI");		
    }
	
	inOper = 1;
	abierto = true;
    return 0;
}

//void PrinterTermica::PrepareCustomerData(char *hdrPkt)
//{
//	if (strlen(c_direcc) == 0)
//		sprintf(c_direcc, "%s", "...");
//	if(strlen(c_nombre) > 30) 
//		c_nombre[30] = 0;
//	if(!_stricmp(c_condiva, "MO"))
//		sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s%c%s%c%s%c%s" ,SEP, c_nombre, SEP, c_cuit, SEP, f_respon, SEP, f_respon=='C'? ' ': 'C',
//		SEP, "Receptor del comprobante - Responsable Monotributo", SEP, c_direcc, SEP, "", SEP, "");		
//	else
//		sprintf(hdrPkt, "\x62%c%s%c%s%c%c%c%c%c%s%c%s%c%s%c%s" ,SEP, c_nombre, SEP, c_cuit, SEP, f_respon, SEP, f_respon=='C'? ' ': 'C',
//			SEP, c_direcc, SEP, "", SEP, "", SEP, "");		
//	
//	EnviaRecibePaquete(hdrPkt);
//}

void PrinterTermica::PrepareCommand(char *txbuff, char *tdes, double mult, double monto, double xtasa, char qual, int cajaBulto)
{
	if(cajaBulto < 1) 
		cajaBulto = 1;
	
	if(qual == 'D')			
		sprintf(txbuff, "m%c%s%c%.2lf%c%d%c%.2lf%c%c%c%d%c%d%c%c%c%s%c%c" ,
			SEP, tdes, SEP, monto, SEP, 7, SEP, xtasa, SEP, '%', SEP, 0, SEP, 0, SEP, 'T', SEP, "7790001001030", SEP, 'B');	
	else
	{		
		
		sprintf(txbuff, "\x42%c%s%c%.6lf%c%.2lf%c%d%c%02.2lf%c%c%c%c%c%.2lf%c%d%c%c%c%d%c%s%c%d",//%c%d" ,
			SEP, tdes, SEP, mult * cajaBulto, SEP, monto, SEP, 7, SEP, xtasa, SEP, qual,
			SEP, '0', SEP, 0.0, SEP, 0, SEP, 'T', SEP, 1, SEP, dscan, SEP, imprecodigo);
		
	}
}

//   Imprime un item de linea.
int PrinterTermica::PrintItem(char *des, double mult, double monto, int tasa, char qual, int bulto, int variac, char *ades, 
						  double unitario, bool pesado, int cajaBulto)
{
    //char tdes[100];	
    //unsigned maxLen;
	char multLine[60];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	if (!abierto)
		TipoComprobante = 83;

    //maxLen = GlobalTF? 40: 20;
    //ChkDesP(des, tdes);	
    //ChkDesP(ades, NULL);
	

	// Ajusta parametros
	double xtasa = ((double)tasa)/100;	

	
	
        //   Abre el comprobante, si corresponde.
		if(!abierto)//(!inOper) 
			PrintEncab();
		//this->CheckStatus();
		double lounitario = monto;		
		if ((fabs(mult - 1.000) > 0.001))
		{
			lounitario = unitario;
			if (pesado)			
				mult = monto / unitario;
		}			
		else if (qual == 'D')
			lounitario = unitario;
		else if (cajaBulto > 1)
			lounitario = unitario;
		if (pesado && ((mult-floor(mult)) > 0.001))
		{
			char otrades[30];
			sprintf(otrades, "%.6lf Kg x $ %.2lf", mult, lounitario);
			PrintText(otrades);
		}
		//   Imprime la descripcion adicional del articulo.
		if(strlen(ades) != 0)
		{
			/*if(qual != 'D')
			{
				if (strlen(tdes) > 50)
					tdes[49] = 0;
				PrintText(ades);			
			}
			else
			{				
				strcat(tdes, ades);
				if (strlen(tdes) > 34)
					tdes[33] = 0;								
			}*/
			//strcat(tdes, ades);
			if (strlen(ades) > 49)
				ades[48] = 0;								

		}
		
		//   Adapta el calificador de la operacion.
		//PrepareCommand(txbuff, tdes, mult, (pesado ? monto : unitario), xtasa, qual, cajaBulto);
		//if (pesado && ((mult-floor(mult)) > 0.001))			
		//	PrepareCommand(txbuff, tdes, 1, monto, xtasa, qual, cajaBulto);		
		//else
		///	PrepareCommand(txbuff, tdes, mult, lounitario, xtasa, qual, cajaBulto);

		//	 Prepara el paquete a enviar.
		//   Envia los datos concretos del articulo. Verifica errores tipicos.		
		//EnviaRecibePaquete(txbuff);
		double xprecbase = 0;
		xprecbase = lounitario / (1 + (xtasa/100));

		{
			ReceiptItem it;
			it.description = ades;
			it.barcode     = des; // "K 7790895000454";
			it.tax_rate    = xtasa;
			//it.base_price  = xprecbase;
			it.quantity    = mult;
			it.unit_price  = lounitario;
			it.amount      = lounitario * mult;
			//it.discounts.push_back(ReceiptDiscount("PROMO ANSES Y J", -270.00));
			recibo.items.push_back(it);
			recibo.total += monto;
		}
		//int rtaStatus = chkPrnStat();
		////Controla el estado de la impresora si hubo algun error.
		//if (rtaStatus){//if(chkPrnStat()){
		//	if (rtaStatus == 2){				
		//		//Espera 1 segundo a que se recupere del error de impresion
		//		int cntWait = 0;
		//		while (cntWait < 1000)
		//			cntWait++;
		//	}else
		//	{
		//		ConsultaUltimoError();
		//		if (strcmp(ultCodError, "NO_CURRENT_ERROR") == 0){
		//			//Sale de la rutina de reimpresion
		//			break;
		//		}
		//		continue;
		//	}
		//}
		////Controla el estado Fiscal de la impreso si hubo algun error
		//if(chkFiscStat()) 
		//{
		//	ConsultaUltimoError();
		//	//El error es de Promocion por un iva que no corresponde
		//	if (strcmp(ultCodError, "ARITHMETIC_NEGATIVE_VAT") == 0){
		//		WriteBootLog("Hay diferencia en el IVA de Descarga");
		//		if (xtasa == 0)
		//			xtasa = 21;
		//		else
		//			xtasa = xtasa == 21 ? 10.5 : 21; 
		//	}
		//	else if (strcmp(ultCodError, "NO_CURRENT_ERROR") == 0){
		//		//Sale de la rutina de reimpresion
		//		break;
		//	}
		//	continue;
		//}

		//   Si el ticket no estaba abierto, lo abre.
        /*if(lstat.fiscstat & 0x30)
		{
			if(GlobalReprint || inOper == 0)
                PrintEncab();
			continue;
		}
		break;*/
	
    
	proteClose = time__();
	return 0;
}


//   Imprime y chequea el subtotal de la operacion.
int PrinterTermica::PrintSubtot(char *msg, int pFlag)
{
	char tdes[30];

	if(FactuFlag > 1)    return 1;
	if(globalDelayPrint) return 0;

	
	//TotalActual = printerTot;
	return 0;
}

//   Envia un texto fiscal al ticket.
int PrinterTermica::PrintText(char *msg)
{
	char tdes[60];

	ChkDesP(msg, tdes);
	if (strlen(tdes) > 50)
		tdes[50] = 0;
    if(!inOper) 
		PrintEncab();

    /*sprintf(txbuff2, "\x41%c%s%c%s%c0", SEP, "", SEP, tdes, SEP);	
	EnviaRecibePaquete(txbuff2);		*/
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterTermica::PrintMPago(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDes(msg, tdes);
    /*sprintf(txbuff, "\x44%c%s%c%.2lf%c%c%c%c", SEP, tdes, SEP, monto, SEP, 'T', SEP, '0');
	EnviaRecibePaquete(txbuff);*/
	recibo.payments.push_back(PaymentMethod(msg, monto));
	proteClose = time__();
	return 0;
}

//   Envia un monto de pago y, si asi se lo indica, el cierre del ticket.
int PrinterTermica::PrintDescto(char *msg, double monto)
{
	char tdes[100];

	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;

	if(strcmp(msg, "Ajuste por redondeo") == 0)
		return 0;

	if(proteClose == time_()) proteClose++;
	while(proteClose == time__())
		tSwitch();

	ChkDes(msg, tdes);
	//sprintf(txbuff, "\x54%c%s%c%.2lf%c%c%c%c%c%s%c%c", SEP, tdes, SEP, monto, SEP, '0', SEP, 'T', SEP, "7790001001030", SEP, 'B'); //(monto >= 0 ? 'R' : 'D'));	
	//EnviaRecibePaquete(txbuff);
	proteClose = time__();
	return 0;
}

int PrinterTermica::PrintClose()
{
	if(proteClose == time__()) proteClose++;
	while(proteClose > time__())
		tSwitch();
	
	if (TipoComprobante == 82 || TipoComprobante == 83){
		
		char auxley[50];
		//SetPie("TRANSPARENCIA FISCAL LEY 27.743", 11);
		recibo.fiscal.law_reference = "TRANSPARENCIA FISCAL LEY 27.743";
		recibo.fiscal.vat_amount = AcumIVA;
		//sprintf(auxley, "IVA CONTENIDO: $ %.2lf", AcumIVA);		
		//recFooter.lines_footer.push_back(auxley);
		//sprintf(auxley, "OTROS IMPUESTOS NACIONALES INDIRECTOS: $ %.2lf", AcumImpuestosInternos);
		recibo.fiscal.other_taxes = AcumImpuestosInternos;
		//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 3, SEP, "08", SEP, auxley, SEP, SEP);	
		//EnviaRecibePaquete(txbuff);
		recFooter.lines_footer.push_back(auxley);
		if (leyendaFinde != 0){
			SetPie(".", 14);
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 5, SEP, "08", SEP, leyendaFinde, SEP, SEP);	
			//EnviaRecibePaquete(txbuff);
			recFooter.lines_footer.push_back(leyendaFinde);
		}
		
	}else if ((TipoComprobante == 110 || TipoComprobante == 112 || TipoComprobante == 113) && (motivoNotaCredito[0] != 0)){
		SetPie(motivoNotaCredito, 11);
		SetPie(supervNotaCredito, 12);
		SetPie("", 13);
	}	
	else if(_stricmp(c_condiva, "MO")){
		SetPie("", 11);
		SetPie("", 12);
		SetPie("", 13);
		if (leyendaFinde != 0){
			SetPie(".", 14);
			//sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%ct", SEP, 5, SEP, "08", SEP, leyendaFinde, SEP, SEP);	
			//EnviaRecibePaquete(txbuff);
			recFooter.lines_footer.push_back(leyendaFinde);
		}
	}	

	myPrinter->print(recibo);
	
	abierto = false;
	if (pieChanged)
	{
		recFooter.lines_footer.push_back(LogoPie[0]);
		recFooter.lines_footer.push_back(LogoPie[1]);
		recFooter.lines_footer.push_back(LogoPie[2]);
		
	}
	pieChanged = false;
	recibo.Clear();
	return 0;
}

//   Envia al impresor fiscal un comando para que cancele todo el
//   ticket actualmente en curso.
int PrinterTermica::PrintCancel()
{
	if(FactuFlag > 1) return 1;
	if(globalDelayPrint) return 0;
	
	//EnviaRecibePaquete("\x98");
	abierto = false;
	return 0;
}

//   Ordena al impresor que haga una zeta.
int PrinterTermica::PrintZeta()
{
    //sprintf(txbuff, "\x39%cZ", SEP);
	//EnviaRecibePaquete(txbuff);
	/*char *pc = bufImpre;
	splitChar(&pc);
	splitChar(&pc);
	splitChar(&pc);
	return atoi(splitChar(&pc));*/
	return 1;
}


//   Ordena al impresor que abra un ticket no fiscal.
int PrinterTermica::OpenVoucher()
{
	//sprintf(txbuff, "\x40%c%d", SEP , 910);	
	//EnviaRecibePaquete(txbuff);	

	GlobalVoucherFlag = 1;
	return 0;
}

//   Ordena al impresor que abra un ticket no fiscal.
int PrinterTermica::OpenVoucher2()
{	
	////borrado de lineas. Va un comando por cada Zona
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%cF", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%cO", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%cH", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%ch", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%cT", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	//sprintf(txbuff, "\x9E%c0%c01%c%s%cD%ct", SEP, SEP, SEP, "\x7f",SEP, SEP);
	//EnviaRecibePaquete(txbuff);
	////abre el voucher	
	//sprintf(txbuff, "\x40%c%d", SEP , 910);
	//EnviaRecibePaquete(txbuff);
	GlobalVoucherFlag = 1;
	return 0;
}

//   Imprime un texto en un comprobante NO fiscal.
int PrinterTermica::PrintVoucher(char *des)
{	
	char *itera = des;
	int i = 0; int j = 0;
	char aux[100];
	strcpy(aux, des);
	bool cambio = false;
	aux[0] = 0;
	while (itera[i] != '\0')
	{
		if ((itera[i] == '&' || itera[i] == '@') && itera[i+1] == '4')
		{
			i++;
			i++;
			cambio = true;
			continue;
		}
		else		
		{
			aux[j] = itera[i];			
		}		
		i++; j++;
	}
	int aformato = 0;
	if (cambio)
	{
		aux[j] = 0;
		aformato = 10;		
	}
	ChkDesP(aux, NULL);		
	/*sprintf(txbuff, "\x49%c%02X%c%s%c0", SEP, aformato, SEP, aux, SEP);    
	EnviaRecibePaquete(txbuff);*/

	myPrinter->print(recibo);
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterTermica::CloseVoucher()
{
	//sprintf(txbuff, "\x45%c%d%c%s", SEP, 0, SEP, "");   
	//EnviaRecibePaquete(txbuff);
    GlobalVoucherFlag = 0;
	return 0;
}

//   Cierra un ticket NO fiscal.
int PrinterTermica::CloseVoucher2()
{
	int i;

	GlobalVoucherFlag = 0;

	//vuelve poner los encabezados y colas
	/*sprintf(txbuff, "\x9E%c1%c0E%c%s%cD%cF", SEP, SEP, SEP, ChkDesP(Trim(LogoLines[0]),0),SEP, SEP);	
	EnviaRecibePaquete(txbuff);	*/
	
	//  Primero, instala los encabezamientos.
	//for(i=1; i<5; i++)
	//{
	//	sprintf(txbuff, "\x9E%c%d%c00%c%s%cD%cO", SEP, i, SEP, SEP, ChkDesP(Trim(LogoLines[i]),0), SEP, SEP);
	//	//xchgPkt(txbuff);
	//	EnviaRecibePaquete(txbuff);
	//}
	//availHeaderLine = i;	

	////  Ahora, instala las definiciones de PIE de ticket.
	//for(i=0; i<cLinesPie; i++)
	//{
	//	sprintf(txbuff, "\x9E%c%d%c%s%c%s%cD%cT", SEP, i+1, SEP, (ChkTextGran(LogoPie[i]) ? "0A": "00"), SEP, ChkDesP(LogoPie[i],0), SEP, SEP);				
	//	EnviaRecibePaquete(txbuff);
	//}
	return 0;
}

//   Chequea los bits de estado del controlador fiscal y muestra los mensajes
//   adecuados, de corresponder.
int PrinterTermica::chkFiscStat()
{
	char *msg;

	if((lstat.fiscstat & 0x81) || (lstat.fiscstat & 0x80))
		msg = "Memoria Fiscal Llena";
	else if(lstat.fiscstat & 0x1)
		msg = "Error al Verificar la Memoria Fiscal";
	else if(lstat.fiscstat & 0x2)
		msg = "Error al Verificar la Memoria de Trabajo (CMOS)";
    else if(lstat.fiscstat & 0x8)	
        msg = "Comando Desconocido";	
	
    //else if(lstat.fiscstat & 0x10)
    //    msg = "No coinciden los totales del ticket y pantalla.";
    else if(lstat.fiscstat & 0x20)	
        msg = "Campo no valido para el estado fiscal actual";
	else if(lstat.fiscstat & 0x40)
		msg = "Monto Total excede lo permitido por el impresor";
    else if(lstat.fiscstat & 0x80)
        msg = "Memoria fiscal llena, bloqueada o dada de baja";
	else if(lstat.fiscstat & 0x100)
		msg = "Memoria fiscal CASI llena. Avise al Tecnico";	
	else
	{
		if(lstat.fiscstat & 0x10) return 1;
		return 0;
	}
    #ifdef INPOS
	Alert("Error Fiscal en la Impresora", msg);
	ConsultaUltimoError();
    #else
    printf("Error Fiscal en la Impresora!!!\n");
    #endif
	return 1;
}

//   Chequea los bits de estado del impresor fiscal y muestra los mensajes
//   orientativos que correspondieren.
int PrinterTermica::chkPrnStat()
{
	char *msg;
	int adevolver = -1;
		
	if(lstat.prnstat & 0x4)
	{
		msg = "Error / Falla de Impresora"; //se ha interrumpido la conexion entre el controlador fiscal y la impresora
		adevolver = 2;//adevolver = 1; se cambio para volver a consultar el error cuando se envia imprimir Item
	}
    else if(lstat.prnstat & 0x08)
	{
		msg = "Impresora Fuera de Linea (OFFLINE)";//la impresora no ha logrado comunicarse dentro le periodo de tiempo establecido
		adevolver = 1;
	}
    else if(lstat.prnstat & 0x10)
		msg = "Falta Papel en la Cinta Testigo";//El sensor de papel diario ha detectado falta de papel
    else if(lstat.prnstat & 0x20)
		msg = "Falta Papel en el Ticket";//El sensor de papel de tiques ha detectado falta de papel
	else if(lstat.prnstat & 0x40)
	{
		msg = "Buffer de impresora lleno";
		adevolver = 1;
	}
    else if(lstat.prnstat & 0x100)
        msg = "Tapa del impresor abierta o mal cerrada";
	else adevolver = 0;
    if (adevolver != 0)
		Alert("Error en la Impresora", msg);
	if(adevolver < 0)
		adevolver = 0;
    
	return adevolver;
}


// Imprime la percepcion en la impresor fiscal
int PrinterTermica::PrintPercep(char *perdescrip, double permonto, int pertasa, double baseImp)
{
//	int intMonto = (int)(permonto * 100);
	int i = 0;	
	int tverror = 0;
	//char auxmonto[10];
	char tvaux[70];
	char hdrPkt[500];	
	//char sep[2]; 
	int result = 0;

	if (permonto < 0.01)
		return result;
	if(strcmp(perdescrip, "Ajuste por redondeo") == 0)
		return result;
	if (!docFiscStat()) //si no existe un documento fiscal abierto no se imprimira la percepcion
		return result;
	int atipoper = 9;
	if (AplicarPercep > 0)
		atipoper = 7;
	else if (AplicarTissh > 0)
		atipoper = 8;
	while(true)
	{
		/*sprintf(hdrPkt, "\x61%c%d%c%s%c%.2lf%c%.2lf", SEP, atipoper, SEP, perdescrip, SEP, baseImp, SEP, permonto);							
		EnviaRecibePaquete(hdrPkt);*/
		bool fallo = (chkPrnStat() != 0);
		if (!fallo)
		{
			result = 1;
			break;
		}
		else if (i < 3)
		{
			sprintf(tvaux, "ErrorPercep en impresion: %d", tverror);
			WriteBootLog(tvaux);
			i++;
		}else
			break;		
	}
	return result;
}

int PrinterTermica::docFiscStat()
{
	if (lstat.fiscstat & 0x1000) 
	{		
		return 1;
	}
	return 0;
}

int PrinterTermica::GeneraReporteAfip(int fdesde, int fhasta)
{	
	return 1;
}


//para esta nueva modalidad se deben enviar dos fechas que pueden ser mas de una semana
//el programa lo tiene que separar en las semanas correspondientes.
int PrinterTermica::GeneraReporteAfipNew(int fdesde, int fhasta)
{	
	return 1;
}


void PrinterTermica::ConsultaUltimoError()
{
	/*sprintf(txbuff, "\x2C");
	EnviaRecibePaquete(txbuff);
	char *pvc = bufImpre;
	splitChar(&pvc);
	splitChar(&pvc);
	char *ultMsjError = new char[10000];
	
	STRCPY(ultCodError, splitChar(&pvc));
	splitChar(&pvc);
	if (strcmp(ultCodError, "POS_DOCUMENT_BEYOND_FISCAL_DAY") == 0)
	{
		Alert("ATENCION!! LA IMPRESORA EMITIRA UNA ZETA", "la ZETA que requiere el impresor !!!");		
		PrintZeta();
		POSExit();
	}
	delete ultMsjError;	*/
}

int PrinterTermica::CopiaDocumento(int tipoDocumento, int nroDocumento)
{
	SendMsg("Reimprimiendo Comprobante", "");

	/*sprintf(txbuff, "\xAF%c%d%c%d", SEP, tipoDocumento, SEP, nroDocumento, SEP, 'P');	
	EnviaRecibePaquete(txbuff);

	int devolver = chkFiscStat() ? 0 : 1;
	
	ClearMsg();
	*/
	return 1;
}

//void PrinterTermica::EnviaRecibePaquete(char *pkt)
//{
//	int rtaSend, rtaRec;
//	//   Emite el paquete al archivo de depuracion.
//#ifdef POSDEBUG
//	sprintf(dbgStr, "PC==>PRN: '%s'", FormatPkt(pkt, strlen(pkt)));
//	DBG(DBGPrintProc, 10, dbgStr);
//#endif
//	rtaSend = rtaRec = -10000;
//	while (rtaSend != 0)
//	{
//		rtaSend = MandaPaqueteFiscal(handImpre, pkt);
//		if (rtaSend != 0)
//			Alert("Error Comunicacion Impresora", "Verificar Operatividad");
//	}
//
//	while (rtaRec != 0)
//	{
//		rtaRec = UltimaRespuesta(handImpre, bufImpre);	
//		if (rtaRec != 0)
//			Alert("Error Comunicacion Impresora", "Verificar Operatividad");
//	}
//	
//	char *cpc = bufImpre;
//	lstat.prnstat = sgethexa(&cpc);
//	lstat.fiscstat = sgethexa(&cpc);
//#ifdef POSDEBUG
//		sprintf(dbgStr, "PRN==>PC: '%s'", FormatPkt(bufImpre, strlen(bufImpre)));
//		DBG(DBGPrintProc, 10, dbgStr);
//#endif
//	
//}


void PrinterTermica::FastVoucher(bool pHeader, char *l1, char *l2, char *l3)
{
	this->OpenVoucher2();
	this->PrintVoucher(l1);
	this->PrintVoucher(l2);
	this->PrintVoucher(l3);
	this->CloseVoucher2();
}