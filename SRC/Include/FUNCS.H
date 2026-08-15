//
//   Funcs.h
//   Descripcion para estructuras de almacenamiento.
//

#pragma once
#include "opc.h"
#include "arraylist.h"
#include "posdb.h"
#include "pos.h"

#pragma pack(1)

struct cobro_ {
	int nro;				// Codigo
	char *nombre;			// Descripcion
	int ecr;				// Codigo en el sistema de recaudacion
	int voucher;			// Codigo del voucher a emitir
	bool habilitado;		// Si esta habilitado/bloqueado
	int *medios;			// Medios de cobro habilitados.
	char **datos;			// Datos adicionales a pedirle al cajero.
	char **helpdato;		// Ayuda para los datos adicionales.
	int *tdato;				// Tipo de los datos adicionales.
	
	cobro_()
	{
		voucher = ecr = nro = 0;
		nombre = new char[25];
		medios = (int *)new int[20];
		datos = (char **)new char*[10];
		helpdato = (char **)new char*[10];
		tdato = (int *)new int[10];
	}

	~cobro_() { delete tdato; delete nombre; delete medios; delete datos; delete helpdato; }
};

struct dep_ {
	int nro;                // Nro asignado.
	char *des;				// Descripcion.
	int iva;                // Iva Asignado.
	struct flags_           // Banderas Operativas.
	{
		int habilitado:1;
	} flags;

	dep_()
	{
		nro = iva = 0;
		flags.habilitado = 0;
		des = (char *) new char[25];
	}
	~dep_() { delete des; }
};

struct env_ {               // Estructura de envases en 'envases.dat'
	unsigned long cod;		// Codigo del envase.
	double cant;			// Cantidad registrada.
};

struct mpag_ {
	int nro;                // Nro asignado.
	char *des;			    // Descripcion.
	char tipo;				// Modo de operar.
	char *grptot;			// Grupo, para la tecla total
	char *linkcc;			// Cta Cte.
	int linkecr;			// Como acumula en el sistema de tesoreria
	int voucher;			// Nro de Voucher a imprimir.
	double maxsal;			// Saldo maximo.
	double descto;			// Descuento (Promocion)
	double maxvlto;			// Maximo vuelto permitido en el medio
	double limgav;			// Limite en gaveta para el medio. 0 = Desactivado
	double bolsin;			// Monto fijo en el bolsin para cambio
	int vltocod;			// Codigo en el que se dara el vuelto
	::ArrayList *arqueo;	// Datos para el arqueo.
	::ArrayList *aData;		// Datos del arqueo efectuado.
	bool extraible;
	double minextracc;
	double maxextracc;
	bool desctomp;
	
	//Agregado para calcular recargo en efectivo
	double mlimreca;
	double alirecargo;

	mpag_()
	{
		nro = linkecr = voucher = vltocod = 0;
		tipo = 0;
		maxsal = descto = maxvlto = limgav = bolsin = 0;
		arqueo = aData = NULL;
		des = (char *) new char[25];
		grptot = (char *) new char[20];
		linkcc = (char *) new char[7];
		extraible = false;
		minextracc = maxextracc = 0;
		desctomp = false;
		
		mlimreca = 0;
		alirecargo = 0;
	}
	~mpag_() { delete des; delete grptot; delete linkcc; }
};

struct ven_ {
	int nro;                // Nro asignado.
	char *Nom;				// Nombre del cajero.

	ven_()
	{
		nro = 0;
		Nom = (char *)new char[25];
	}
	~ven_() { delete Nom; }
};

struct misc_ {
	int  nro;
	char *des;
	int  signo;
	int  voucher;

	misc_()
	{
		nro = signo = voucher = 0;
		des = (char *)new char[25];
	}
	~misc_() { delete des; }
};

//		Listado de funciones, tal como se codifican en el 'trans.dbf'.
#define DDepto			 1
#define DMpag			 2
#define DEop			 3
#define DPlu			 4
#define DMode			 5
#define DCorri			 6
#define DZeta			 7
#define DFactu1			 8
#define DFactu2			 9
#define DMisc			10
#define DClk			11
#define DEnvas			12
#define DVen			13
#define DDecla			14
#define DDOtrop			15
#define DPTarea			16
#define DReparto		17
#define DZVended		18
#define DTickFac		19
#define DCCOnln			20
#define DCobraCC		21
#define DDeclaDT		22
#define DSuspen			23
#define DCobranza		24
#define DSuperv			25
#define DPromo			26
#define DCellLoad		27
#define DFactu3			28
#define DExtraCash		29
#define DVuelto			30
#define DFactu4			31
#define DAnul			32
#define DItrackq		33

//#define DNcHead			34
#define DNcItem			35
#define DNcPer			36
#define DNcPago			37

#define DFactu5			38

#define DCompAnul		39

#define DVendMay		40

#define DSeniaMP		41


#define MAX_FUNC		50				/* Ajustarlo si se agregan funciones!!!  */

//  Tamaño de los posibles componentes de estructura.
#define SZ_CHAR  sizeof(char)
#define SZ_ULONG sizeof(unsigned long)
#define SZ_INT   sizeof(int)
#define SZ_BOOL  sizeof(char)
#define SZ_LLONG sizeof(long long)

//  Estructura para soporte del dumping estio .dbf
struct dFld_ {
	int ancho;							// Ancho del campo.
	char type;							// Tipo 'C' que utiliza.
	char *fName;						// Campo correspondiente, en trans.dbf
};
struct dDbf_ {
	int func;							// Id de la funcion.
	struct dFld_ dFld[30];				// Campos de la estructura.
};

//  Estructuras de dumping sobre el archivo 'trans.dat'.

struct DCellLoad_ {
	int func;							// Id de la funcion
	char cell[30];						// Numero de celular al que se le cargo monto.
	char imp[14];						// Monto que se le cargo
};
#ifdef IN_DUMP
dDbf_ xxDDCellLoad = { DCellLoad, {
	{ SZ_INT,		'I', "nro" },
	{ SZ_CHAR*30,	'C', "local" },
	{ SZ_CHAR*14,	'C', "imp" },
	{0}
}};

#endif

struct DTickFac_ {
	int func;							// Id de la funcion
	int nro;							// Nro de factura.
	int cajero;							// Cajero que hizo la operacion
	char flag;							// 1=ACTIVA ticket factura, 0=Desactiva
	unsigned long fecha;				// Fecha y hora de la operacion
	unsigned long hora;
};
#ifdef IN_DUMP
dDbf_ xxDDTickFac = { DTickFac, {
	{ SZ_INT,		'I', "nro" }, 
	{ SZ_INT, 		'I', "cajero" }, 
	{ SZ_BOOL,		'T', "flag" },
	{ SZ_ULONG,		'L', "fecha" }, 
	{ SZ_ULONG,		'L', "hora" }, 
	{0}
}};
#endif

struct DPromo_ {
	int func;
	int nro;							// Numero de promocion que se activo.
	long cod;
	long long codpromo;					// Codigo (PLU) en el que se debe reportar la actividad de la promo
	int cajero;							// Cajero que hizo la operacion
	int tpremio;						// Tipo de premio otorgado
	char imprime;						// Indica si la promocion se imprimira al pie del ticket
	int  linea;							// Indica en que linea se imprime 
	char p1[30];						// Parametro 1 de la activacion
	char p2[30];						// Parametro 2
	char p3[30];						// Parametro 3
	char cantpromo[13];
	char preciouni[13];
	unsigned long fecha;				// Fecha y hora de la operacion
	unsigned long hora;
	int accionSeq;					// Indice 1-based de la accion que grabo el renglon (0 = grabado por version anterior)
};
#ifdef IN_DUMP
dDbf_ xxDDPromo = { DPromo, {
	{ SZ_INT,		'I', "nro" },
	{ SZ_ULONG,		'L', "cod" },
	{ SZ_LLONG,		'X', "envase"},
	{ SZ_INT,		'I', "cajero" },
	{ SZ_INT,		'I', "mpag" },		// Tipo de premio entregado
	{ SZ_BOOL,		'T', "flag" },
	{ SZ_INT,		'I', "posi" },
	{ SZ_CHAR*30,	'C', "direcc" },	// Parametro 1
	{ SZ_CHAR*30,	'C', "barrio" },	// Parametro 2
	{ SZ_CHAR*30,	'C', "local" },		// Parametro 3
	{ SZ_CHAR*13,	'C', "cant" },		// cantidad llevada
	{ SZ_CHAR*13,	'C', "vuelto" },	// cantidad llevada
	{ SZ_ULONG,		'L', "fecha" },
	{ SZ_ULONG,		'L', "hora" },
	{ SZ_INT,		'I', "que_anula" },	// Indice de accion. Reusa una columna libre del DBF: NO cambia el esquema.
	{0}
}};
#endif

struct DCobranza_ {
	int func;				// Id de la funcion.
	char monto[14];			// monto de la cobranza.
	int mpago;				// Medio de cobro usado.
	int servicio;			// Servicio cobrado.
	char dato1[31];			// Dato adicional 1
	char dato2[31];			// Dato adicional 2
	char dato3[31];			// Dato adicional 3
	unsigned long fecha;	// Fecha y hora de la transaccion.
	unsigned long hora;
	int cajero;				// Codigo del cajero
};
#ifdef IN_DUMP
dDbf_ xxDDCobranza = { DCobranza, {
	{ SZ_CHAR*14,	'C', "imp" },
	{ SZ_INT,		'I', "mpag" },
	{ SZ_INT,		'I', "nro" },
	{ SZ_CHAR*31,	'C', "nombre" },
	{ SZ_CHAR*31,	'C', "direcc" },
	{ SZ_CHAR*31,	'C', "barrio" },
	{ SZ_ULONG,		'L', "fecha"}, 
	{ SZ_ULONG,		'L', "hora"}, 
	{ SZ_INT,       'I', "cajero"}, 
	{0}
}};
#endif

struct DSuspende_ {
	int func;				// Id de la funcion
	int comprob;			// Tipo de comprobante
	int ticknum;			// Numero de ticket suspendido
	char monto[14];			// Monto del ticket anulado
	char flag;				// 0=Suspende, 1=Recupera
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
};
#ifdef IN_DUMP
dDbf_ xxDSuspende = { DSuspen, {
	{SZ_INT,		'I', "tipo" },
	{SZ_INT,		'I', "ticket" },
	{SZ_CHAR*14,	'C', "imp" },
	{SZ_BOOL,		'T', "flag" },
	{SZ_ULONG,		'L', "fecha" }, 
	{SZ_ULONG,		'L', "hora" }, 
	{SZ_INT,		'I', "cajero" },
	{0}
}};
#endif


struct DSenia_ {
	int func;				// Id de la funcion
	char monto[14];			// Monto del ticket anulado
	int mediopago;				// 0=Suspende, 1=Recupera
	int supervisor;			// Codigo del supervisor que autorizo la seña
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
};
#ifdef IN_DUMP
dDbf_ xxDSeniaMP = { DSeniaMP, {
	{SZ_CHAR*14,	'C', "imp" },
	{SZ_INT,		'I', "nro" },
	{SZ_INT,		'I', "cod" },
	{SZ_ULONG,		'L', "fecha" }, 
	{SZ_ULONG,		'L', "hora" }, 
	{SZ_INT,		'I', "cajero" },
	{0}
}};
#endif


struct DRepar_ {
	int func;				// Id de la funcion
	int nro;				// Nro de ID del reparto elegido.
	char permFlag;          // Permanece permanentemente en reparto.
};
#ifdef IN_DUMP
dDbf_ xxDRepar = { DReparto, {
	{ SZ_INT, 'I', "nro" }, 
	{ SZ_BOOL, 'T', "flag"},
	{0}
}};
#endif

struct DOtrop_ {
	int func;				// Id de la funcion.
	int nprec;				// Nuevo precio seleccionado.
	char permFlag;			// Permanece permanentemente en el prec.
};
#ifdef IN_DUMP
dDbf_ xxDOtrop = { DDOtrop, {
	{ SZ_INT, 'I', "nprec" }, 
	{ SZ_BOOL, 'T', "flag" },
	{0}
}};
#endif

struct DPTarea_ {
	int func;				// Id de la funcion.
	int nRec;				// Nro de registro ejecutado.
	int tFunc;				// Funcion que se ejecuto.
};
#ifdef IN_DUMP
dDbf_ xxDPTarea = { DPTarea, {
	{ SZ_INT, 'I', "nro" }, 
	{ SZ_INT, 'I', "ticket" },
	{0}
}};

#endif

struct DDecla_ {
	int func;				// Id de la funcion.
	int mpag;				// Nro de medio de cobro.
	int cajero;				// Cajero arqueado.
	char imp[14];			// Monto recontado.
	char teorico[14];		// Monto teorico.
	char control;			// 1=Si es arqueo de control, sino, cero.
	char nombregrupo;		//Agrupacion del medio de pago, toma lo que se imprime
};
#ifdef IN_DUMP
dDbf_ xxDDecla = { DDecla, {
	{ SZ_INT,     'I', "mpag" }, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_CHAR*14, 'C', "imp" }, 
	{ SZ_CHAR*14, 'C', "teorico" }, 
	{ SZ_BOOL,    'T', "flag" },
	{ SZ_CHAR*30, 'C', "nombre" },
	{0}
}}; 
#endif

struct DDeclaDT_ {
	int func;				// Id de la funcion.
	int mpag;				// Nro de medio de cobro.
	int cajero;				// Cajero arqueado.
	char item[30];			// Codigo y cantidad item
	char imp[14];			// Monto recontado.
};
#ifdef IN_DUMP
dDbf_ xxDDeclaDT = { DDeclaDT, {
	{ SZ_INT,     'I', "mpag" }, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_CHAR*30, 'C', "nombre" },
	{ SZ_CHAR*14, 'C', "imp" }, 
	{0}
}}; 
#endif

struct DDMisc_ {
	int func;               // Id de la funcion.
	int nro;                // Nro de la funcion varia utilizada.
	char imp[14];           // Importe de la operacion (con signo).
	int mpag;               // Medio de pago envuelto en la transaccion.
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que la efectuo
	int numRetiro;			// Numero de retiro
};
#ifdef IN_DUMP
dDbf_ xxDDMisc = { DMisc, {
	{ SZ_INT,     'I', "nro"}, 
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_INT,     'I', "mpag" }, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" }, 
	{ SZ_INT,     'I', "otrop" }, 
	{0}
}};
#endif

struct DDFactu1_ {
	int func;               // Id de la funcion.
	int modo;               // Bandera de activacion/Desactivación.
	char nombre[30];		// Nombre del cliente CF utilizado.
};
#ifdef IN_DUMP
dDbf_ xxDDFactu1 = { DFactu1, {
	{ SZ_INT, 'I', "modo" }, 
	{ SZ_CHAR*30, 'C', "nombre" },
	{0}
}};
#endif
 
struct DDFactu2_
{
	int func;               // Id de la funcion.
	char ni[14];            // SobreTasa cobrada.
};

#ifdef IN_DUMP
dDbf_ xxDDFactu2 = { DFactu2, {
	{ SZ_CHAR*14, 'C', "imp" }, 
	{0}
}};
#endif

struct DDFactu3_
{
	int func;               // Id de la funcion.
	char percep[14];            // Importe cobrado.
	char montoneto[14];
	char alicuota[14];			 // Alicuota de la percepcion.
	int cajero;
};

#ifdef IN_DUMP
dDbf_ xxDDFactu3 = { DFactu3, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_INT,		'I', "cajero" },
	{0}
}};
#endif

struct DDFactu4_
{
	int func;               // Id de la funcion.
	char percep[14];            // Importe cobrado.
	char montoneto[14];
	char alicuota[14];			 // Alicuota de la percepcion.
	int cajero;
};

#ifdef IN_DUMP
dDbf_ xxDDFactu4 = { DFactu4, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_INT,		'I', "cajero" },
	{0}
}};
#endif


struct DDFactu5_
{
	int func;               // Id de la funcion.
	char percep[14];            // Importe cobrado.
	char montoneto[14];
	char alicuota[14];			 // Alicuota de la percepcion.
	int cajero;
};

#ifdef IN_DUMP
dDbf_ xxDDFactu5 = { DFactu5, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_INT,		'I', "cajero" },
	{0}
}};
#endif


struct DDNcPercep_
{
	int func;               // Id de la funcion.
	char percep[14];            // Importe cobrado.
	char montoneto[14];
	char alicuota[14];			 // Alicuota de la percepcion.
	int cajero;
	int tipo;
};

#ifdef IN_DUMP
dDbf_ xxDDNcPercep = { DNcPer, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_INT,		'I', "cajero" },
	{ SZ_INT,		'I', "nro" },
	{0}
}};
#endif


struct DDVuelto_
{
	int func;               // Id de la funcion.
	char Importe[14];            // Importe del vuelto.
	int TipoOperacion;				// Tipo de Operacion, 1 = Acredita Vuelto, 2 = Debita vuelto, 3 = cierre Sos Movil
	char Dni[14];				//Numero de documento del cliente
	char NumTransaccion[15];			 // Alicuota de la percepcion.
	char Telefono[13];			// Numero de Telefono para $O$ Movil
	int cajero;
	unsigned long ticket;					//Numero de Ticket
};

#ifdef IN_DUMP
dDbf_ xxDDVuelto = { DVuelto, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_INT, 'I', "nro" },
	{ SZ_CHAR*14, 'C', "dni" },
	{ SZ_CHAR*15, 'C', "cod" },
	{ SZ_CHAR*13, 'C', "telefono" },
	{ SZ_INT,		'I', "cajero" },
	{ SZ_ULONG,   'L', "ticket" },
	{0}
}};
#endif


struct DDdep_ {
	int func;               // Id de la funcion.
	int nro;                // Nro del departamento.
	char uCant;             // Se especifico cantidad.
	int iva;                // Tasa a la que vende
	char yReg[14];
	char xReg[14];
	char anulado;			// A TRUE si se anulo el renglon.
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que hizo la venta
};
#ifdef IN_DUMP
dDbf_ xxDDdep = { DDepto, {
	{ SZ_INT,     'I', "nro"}, 
	{ SZ_BOOL,    'T', "uso_cant"},
	{ SZ_INT,     'I', "iva"}, 
	{ SZ_CHAR*14, 'C', "cant" }, 
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_BOOL,    'T', "anulado"}, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero"}, 
	{0}
}};
#endif

struct DDcorri_ {
	int func;				// ID.
	int anula;				// Funcion (Depto o Plu) que anula
	unsigned long posi;     // Posicion del LOG que cancela.
	int cajero;				// Cajero logineado cuando se corrigio
	int supervisor;			// Codigo del supervisor que autorizo la anulacion
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	char monto[14];			// Monto anulado

};
#ifdef IN_DUMP
dDbf_ xxDDcorri = { DCorri, {
	{ SZ_INT,     'I', "que_anula"}, 
	{ SZ_ULONG,   'L', "posi" }, 
	{ SZ_INT,     'I', "cajero" }, 
	{ SZ_INT,     'I', "cod" },
	{ SZ_ULONG,   'L', "fecha" }, 
	{ SZ_ULONG,   'L', "hora" }, 
	{ SZ_CHAR*14, 'C', "imp" },
	{0}
}};
#endif

struct DDchmode_ {
	int func;               // Id de la funcion.
	int nmode;              // Nuevo modo de operacion.
	int cajero;				// Cajero habilitado cuando se cambio el modo
	int supervisor;			// Codigo del supervisor habilitado cuando se cambio el modo
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
};
#ifdef IN_DUMP
dDbf_ xxDDchmode = { DMode, {
	{ SZ_INT,     'I', "nro" }, 
	{ SZ_INT,     'I', "cajero" }, 
	{ SZ_INT,     'I', "cod" },
	{ SZ_ULONG,   'L', "fecha" }, 
	{ SZ_ULONG,   'L', "hora" }, 
	{0}
}};
#endif

struct DDplu_ {
	int func;               // Id de la funcion.
	unsigned long cod;      // Codigo (real) que se utilizo.
	char s_c_flag;          // 1 si vendio x scanner, 0 si por codigo.
	char uCant;             // Utilizo multiplicacion.
	int iva;                // Tasa de iva.
	char pesado;            // A uno si es de venta al peso
	int dnro;               // Departamento al que debe acumular la venta.
	unsigned long envas;    // Codigo de Envase - 0 si no tiene.
	char yReg[14];
	char xReg[14];
	char miles[14];
	char origCode[14];		// Codigo original tipeado o escaneado.
	int  otroP;				// Si facturo a otro precio.
	char anulado;			// Si el renglon se anulo.
	long precuni;			// Precio unitario utilizado (fundamental para articulos pesados)
	int uni;				// Unidad por bulto (para decodificar DUN14)
	char esBulto;			// Si no se scaneo una unidad sino un bulto completo
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	char dun14;				// Si se escaneo scanner de bulto cerrado
	int sdnro;				// Subdepartamento
	char impiva[12];		//IVA calculado para SAP
	char precmay[12];		//Precio utilizado en las ofertas mayoristas
	long long ppuntual;		//Precio puntual, indica el numero de transaccion
	int umillas;		//millas del articulo
	char pesoenbalanza[12]; //El peso registrado en la balanza 
	char apliPerIva;			//para discriminar los materriales sobre los que se aplica la percepcion de iva
	long precpuntual;		//Precio puntual, indica el numero de transaccion
	char montoImpInterno[14];	//Monto de Impuesto Interno
};
#ifdef IN_DUMP
dDbf_ xxDDplu = { DPlu, {
	{ SZ_ULONG,   'L', "cod"}, 
	{ SZ_BOOL,    'T', "uso_scan"},
	{ SZ_BOOL,    'T', "uso_cant" }, 
	{ SZ_INT,     'I', "iva"}, 
	{ SZ_BOOL,    'T', "pesado" },
	{ SZ_INT,     'I', "nro" }, 
	{ SZ_ULONG,   'L', "envase" }, 
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_CHAR*14, 'C', "imp" }, 
	{ SZ_CHAR*14, 'C', "millas" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_INT,     'I', "otrop" },
	{ SZ_BOOL,    'T', "anulado"}, 
	{ SZ_ULONG,   'L', "vuelto"},
	{ SZ_INT,     'I', "numero" },
	{ SZ_BOOL,    'T', "u_vuelto" },
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_BOOL,    'T', "flag" },
	{ SZ_INT,     'I', "posi" },
	{ SZ_CHAR*12,	'C', "impiva" },
	{ SZ_CHAR*12,	'C', "fecha_c" },
	{ SZ_LLONG,   'X', "fecha_z"},
	{ SZ_INT,     'I', "modo" }, 
	{ SZ_CHAR*12,	'C', "telefono" },
	{ SZ_BOOL,    'T', "u_pend" },
	{ SZ_ULONG,   'L', "ticket"}, 
	{ SZ_CHAR*14, 'C', "dni" },
	{0}
}};
#endif


struct DDNcItem_ {
	int func;               // Id de la funcion.
	unsigned long cod;      // Codigo (real) que se utilizo.
	char s_c_flag;          // 1 si vendio x scanner, 0 si por codigo.
	char uCant;             // Utilizo multiplicacion.
	int iva;                // Tasa de iva.
	char pesado;            // A uno si es de venta al peso
	int dnro;               // Departamento al que debe acumular la venta.
	unsigned long envas;    // Codigo de Envase - 0 si no tiene.
	char yReg[14];
	char xReg[14];
	char miles[14];
	char origCode[14];		// Codigo original tipeado o escaneado.
	int  otroP;				// Si facturo a otro precio.
	char anulado;			// Si el renglon se anulo.
	long precuni;			// Precio unitario utilizado (fundamental para articulos pesados)
	int uni;				// Unidad por bulto (para decodificar DUN14)
	char esBulto;			// Si no se scaneo una unidad sino un bulto completo
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	char dun14;				// Si se escaneo scanner de bulto cerrado
	int sdnro;				// Subdepartamento
	char impiva[12];		//IVA calculado para SAP
	char precmay[12];		//Precio utilizado en las ofertas mayoristas
	long long ppuntual;		//indica el numero de transaccion/pedido en Ecommerce o pedidos Ya
	int umillas;		//millas del articulo
	char pesoenbalanza[12]; //El peso registrado en la balanza 
	char apliPerIva;			//para discriminar los materriales sobre los que se aplica la percepcion de iva
	long precpuntual;		//Precio puntual, indica el numero de transaccion
	char montoImpInterno[14];	//Monto de Impuesto Interno
};
#ifdef IN_DUMP
dDbf_ xxDDNcItem = { DNcItem, {
	{ SZ_ULONG,   'L', "cod"}, 
	{ SZ_BOOL,    'T', "uso_scan"},
	{ SZ_BOOL,    'T', "uso_cant" }, 
	{ SZ_INT,     'I', "iva"}, 
	{ SZ_BOOL,    'T', "pesado" },
	{ SZ_INT,     'I', "nro" }, 
	{ SZ_ULONG,   'L', "envase" }, 
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_CHAR*14, 'C', "imp" }, 
	{ SZ_CHAR*14, 'C', "millas" },
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_INT,     'I', "otrop" },
	{ SZ_BOOL,    'T', "anulado"}, 
	{ SZ_ULONG,   'L', "vuelto"},
	{ SZ_INT,     'I', "numero" },
	{ SZ_BOOL,    'T', "u_vuelto" },
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_BOOL,    'T', "flag" },
	{ SZ_INT,     'I', "posi" },
	{ SZ_CHAR*12,	'C', "impiva" },
	{ SZ_CHAR*12,	'C', "fecha_c" },
	{ SZ_LLONG,   'X', "fecha_z"},
	{ SZ_INT,     'I', "modo" }, 
	{ SZ_CHAR*12,	'C', "telefono" },
	{ SZ_BOOL,    'T', "u_pend" },
	{ SZ_ULONG,   'L', "ticket"}, 
	{ SZ_CHAR*14, 'C', "dni" },
	{0}
}};
#endif

struct DDclk_ {
	int func;               // Id de la funcion.
	char logflg;            // Id de login/logout.
	int nro;                // Nro de cajero
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
};
#ifdef IN_DUMP
dDbf_ xxDDclk = { DClk, {
	{ SZ_BOOL,    'T', "flag" }, 
	{ SZ_INT,     'I', "nro" }, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{0}}};
#endif

struct DDSuperv_ {
	int func;
	char logflg;
	int nro;
	int cajero;				// Cajero habilitado cuando se cambio el modo
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
};
#ifdef IN_DUMP
dDbf_ xxDDsuperv = { DSuperv, {
	{ SZ_BOOL,    'T', "flag" },
	{ SZ_INT,     'I', "nro" },
	{ SZ_INT,     'I', "cajero"}, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{0}}};
#endif

struct DDven_ {
	int func;               // Id de la funcion.
	char logflg;            // Id de login/logout.
	int nro;                // Nro de mozo.
};
#ifdef IN_DUMP
dDbf_ xxDDven = { DVen, {
	{ SZ_BOOL, 'T', "flag" }, 
	{ SZ_INT, 'I', "nro" }, 
	{0}}};
#endif


struct DVendMay_ {
		int func;               // Id de la funcion.		
		int cajero;				// Cajero habilitado
		int nro;                // Nro de Vendedor Mayorista.
	};
#ifdef IN_DUMP
	dDbf_ xxDVendMay = { DVendMay, {	
		{ SZ_INT,     'I', "cajero"}, 
		{ SZ_INT, 'I', "nro" }, 
		{0}}};
#endif


struct DDmpag_ {
	int  func;              // Id de la funcion.
	int  nro;               // Nro del medio utilizado.
	char uVuelto, uPend;    // Flags de procesamiento.
	char importe[14];
	char vuelto[14];
	char cc[12];				// Cta Cte usada.
	char numero[14];		// Nro ingresado.
	char fecha[12];			// Fecha ingresada.
	char nombre[31];        // Nombre ingresado.
	char dni[16];			// DNI
	char telef[31];			// Telefono
	char direc[31];         // Direccion.
	char local[31];			// Localidad
	char civa[4];			// Condicion ante el iva.
	char barrio[31];		// Barrio.
	char mtodesc[14];		// Monto descontado.
	char uOnline;			// Si existio autorizacion ONLINE
	//int  lista;				// Lista de precio a usar
	unsigned long _fecha;   // Fecha de la transaccion.
	unsigned long _hora;    // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	char dtarjeta[14];
	long long nticket;	
	int alipercep;
	char escredito;			// Indica si el cupon POSNET es de credito (true) o debito (false)
	char esposnet;			// Indica si el campo es de un mpago POSNET
	char esextracash;
	char montoextrac[14];
	int codpostal;			//Codigo postal del cliente
	int reparticion;		//reparticion del cliente

};
#ifdef IN_DUMP
dDbf_ xxDDmpag = { DMpag, {
	{ SZ_INT,     'I', "nro" },
	{ SZ_BOOL,    'T', "u_vuelto" },
	{ SZ_BOOL,    'T', "u_pend" },    
	{ SZ_CHAR*14, 'C', "imp" },    
	{ SZ_CHAR*14, 'C', "vuelto" },
	{ SZ_CHAR*12,  'C', "codcli" },  
	{ SZ_CHAR*14, 'C', "numero" }, 
	{ SZ_CHAR*12, 'C', "fecha_c" },
	{ SZ_CHAR*31, 'C', "nombre" }, 
	{ SZ_CHAR*16, 'C', "dni" },    
	{ SZ_CHAR*31, 'C', "telefono" },
	{ SZ_CHAR*31, 'C', "direcc" }, 
	{ SZ_CHAR*31, 'C', "local" },  
	{ SZ_CHAR*4,  'C', "condiva"}, 
	{ SZ_CHAR*31, 'C', "barrio" }, 
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_BOOL,    'T', "flag" },
	//{ SZ_INT,     'I', "otrop" },
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_CHAR*14, 'C', "teorico" }, 
	{ SZ_LLONG,   'X', "fecha_z" },
	{ SZ_INT,     'I', "modo" },
	{ SZ_BOOL,    'T', "uso_cant" },
	{ SZ_BOOL,    'T', "uso_scan"},
	{ SZ_BOOL,    'T', "pesado"},
	{ SZ_CHAR*14, 'C', "millas" },    
	{ SZ_INT,     'I', "ticket" },
	{ SZ_INT,     'I', "tipo" },
	{0}
}};
#endif

struct DDNcPago_ {
	int  func;              // Id de la funcion.
	int  nro;               // Nro del medio utilizado.
	char uVuelto, uPend;    // Flags de procesamiento.
	char importe[14];
	char vuelto[14];
	char cc[12];				// Cta Cte usada.
	char numero[14];		// Nro ingresado.
	char fecha[12];			// Fecha ingresada.
	char nombre[31];        // Nombre ingresado.
	char dni[16];			// DNI
	char telef[31];			// Telefono
	char direc[31];         // Direccion.
	char local[31];			// Localidad
	char civa[4];			// Condicion ante el iva.
	char barrio[31];		// Barrio.
	char mtodesc[14];		// Monto descontado.
	char uOnline;			// Si existio autorizacion ONLINE
	//int  lista;				// Lista de precio a usar
	unsigned long _fecha;   // Fecha de la transaccion.
	unsigned long _hora;    // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	char dtarjeta[14];
	unsigned long nticket;	
	int alipercep;
	char escredito;			// Indica si el cupon POSNET es de credito (true) o debito (false)
	char esposnet;			// Indica si el campo es de un mpago POSNET
	char esextracash;
	char montoextrac[14];
	int codpostal;			//Codigo postal del cliente
	int reparticion;		//reparticion del cliente

};
#ifdef IN_DUMP
dDbf_ xxDDNcPago = { DNcPago, {
	{ SZ_INT,     'I', "nro" },
	{ SZ_BOOL,    'T', "u_vuelto" },
	{ SZ_BOOL,    'T', "u_pend" },    
	{ SZ_CHAR*14, 'C', "imp" },    
	{ SZ_CHAR*14, 'C', "vuelto" },
	{ SZ_CHAR*12,  'C', "codcli" },  
	{ SZ_CHAR*14, 'C', "numero" }, 
	{ SZ_CHAR*12, 'C', "fecha_c" },
	{ SZ_CHAR*31, 'C', "nombre" }, 
	{ SZ_CHAR*16, 'C', "dni" },    
	{ SZ_CHAR*31, 'C', "telefono" },
	{ SZ_CHAR*31, 'C', "direcc" }, 
	{ SZ_CHAR*31, 'C', "local" },  
	{ SZ_CHAR*4,  'C', "condiva"}, 
	{ SZ_CHAR*31, 'C', "barrio" }, 
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_BOOL,    'T', "flag" },
	//{ SZ_INT,     'I', "otrop" },
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_CHAR*14, 'C', "teorico" }, 
	{ SZ_ULONG,   'L', "fecha_z" },
	{ SZ_INT,     'I', "modo" },
	{ SZ_BOOL,    'T', "uso_cant" },
	{ SZ_BOOL,    'T', "uso_scan"},
	{ SZ_BOOL,    'T', "pesado"},
	{ SZ_CHAR*14, 'C', "millas" },    
	{ SZ_INT,     'I', "ticket" },
	{ SZ_INT,     'I', "tipo" },
	{0}
}};
#endif

struct DDeop_ {
	int func;
	int tipo;               // Fiscal, no Fiscal.
	unsigned long ntick;    // Nro de Ticket.
	char imp[14];			// Si era anulacion de ticket, cuanto.
	char anuFlag;			// TRUE si anula TICKET, FALSE si cierra.
	unsigned long fecha;    // Fecha de la transaccion.
	unsigned long hora;     // Hora de la transaccion.
	int cajero;				// Cajero que cerro la operacion
	char montoAnul[14];		// Monto, si era ticket anulado
	int ptovta;				// Punto de venta donde se realizo la opeacion.
	char enviadoCobradora;	// TRUE si se envia el ticket a la cobradora
	int tipoDoc;			// tipo de documento fiscal ticket/FA/FB TI=83,FA=81,FB=82,NT=110,NA=112,NB=113
	char ticketOriginal[14];// El numero de Ticket formateado, usado para Nota Credito
	char montoImpre[14];	//para almacenar el importe de subtotal de la impresora
	char codMotivoNc[4];	//Motivo de la nota de Credito
	int superv;				//supervisor que intervino en el ticket anulado
	char writeOnline;		//Para identificar los que fueron grabados en base centralizada
};
#ifdef IN_DUMP
dDbf_ xxDDeop = { DEop, {
	{ SZ_INT,     'I', "tipo" }, 
	{ SZ_ULONG,   'L', "ticket" },
	{ SZ_CHAR*14, 'C', "imp"}, 
	{ SZ_BOOL,    'T', "flag"}, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" }, 
	{ SZ_CHAR*14, 'C', "cant" },
	{ SZ_ULONG,   'I', "cod"}, 
	{ SZ_BOOL,    'T', "uso_scan"}, 
	{ SZ_INT,     'I', "posi" }, 
	{ SZ_CHAR*14, 'C', "teorico" },
	{ SZ_CHAR*14, 'C', "vuelto" },
	{ SZ_CHAR*4, 'C', "fecha_c" },
	{ SZ_INT,     'I', "mpag" }, 
	{ SZ_BOOL,    'T', "u_pend"}, 
	{0}
}};
#endif


struct DDanul_ {
	int  func;              // Id de la funcion.
	int  nro;               // Nro del medio utilizado.
	//char uVuelto, uPend;    // Flags de procesamiento.
	char importe[14];
	//char vuelto[14];
	//char cc[12];				// Cta Cte usada.
	//char numero[14];		// Nro ingresado.
	char fecha[12];			// Fecha ingresada.
	char nombre[31];        // Nombre ingresado.
	char dni[16];			// DNI
	char telef[31];			// Telefono
	char direc[31];         // Direccion.
	//char local[31];			// Localidad
	//char civa[4];			// Condicion ante el iva.
	char barrio[31];		// Barrio.
	//char mtodesc[14];		// Monto descontado.
	//char uOnline;			// Si existio autorizacion ONLINE	
	unsigned long _fecha;   // Fecha de la transaccion.
	unsigned long _hora;    // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	char dtarjeta[14];
	unsigned long nticket;	
	//int alipercep;
	char escredito;			// Indica si el cupon POSNET es de credito (true) o debito (false)
	char esposnet;			// Indica si el campo es de un mpago POSNET
	//char esextracash;

};
#ifdef IN_DUMP
dDbf_ xxDDanul = { DAnul, {
	{ SZ_INT,     'I', "nro" },
	//{ SZ_BOOL,    'T', "u_vuelto" },
	//{ SZ_BOOL,    'T', "u_pend" },    
	{ SZ_CHAR*14, 'C', "imp" },    
	//{ SZ_CHAR*14, 'C', "vuelto" },
	//{ SZ_CHAR*12,  'C', "codcli" },  
	//{ SZ_CHAR*14, 'C', "numero" }, 
	{ SZ_CHAR*12, 'C', "fecha_c" },
	{ SZ_CHAR*31, 'C', "nombre" }, 
	{ SZ_CHAR*16, 'C', "dni" },    
	{ SZ_CHAR*31, 'C', "telefono" },
	//{ SZ_CHAR*31, 'C', "direcc" }, 
	//{ SZ_CHAR*31, 'C', "local" },  
	//{ SZ_CHAR*4,  'C', "condiva"}, 
	//{ SZ_CHAR*31, 'C', "barrio" }, 
	//{ SZ_CHAR*14, 'C', "cant" },
	//{ SZ_BOOL,    'T', "flag" },	
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_CHAR*14, 'C', "teorico" }, 
	{ SZ_ULONG,   'L', "ticket" },
	//{ SZ_INT,     'I', "modo" },
	{ SZ_BOOL,    'T', "uso_cant" },
	{ SZ_BOOL,    'T', "uso_scan"},
	//{ SZ_BOOL,    'T', "pesado"},
	{0}
}};
#endif

struct DCompDanul_ {
	int  func;              // Id de la funcion.
	char importe[14];
	unsigned long _fecha;   // Fecha de la transaccion.
	unsigned long _hora;    // Hora de la transaccion.
	int cajero;				// Cajero que hizo la operacion.
	unsigned long nticket;	
	bool pescribir;			// bandera para escribir
	int ptovta;
	int tipoDoc;
};
#ifdef IN_DUMP
dDbf_ xxDDCompAnul = { DCompAnul, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{ SZ_ULONG,   'L', "ticket" },
	{ SZ_BOOL,    'T', "uso_cant" },

	{ SZ_ULONG,   'I', "cod"}, 	
	{ SZ_INT,     'I', "posi" }, 
	{0}
}};
#endif



struct DDitrackq_ {
	int  func;              // Id de la funcion.
	int  nro;               // 1 = inicio del ticket ITrackQ
	char codcoaster[14];	
	int cajero;				// Cajero que hizo la operacion.	
};
#ifdef IN_DUMP
dDbf_ xxDDitrackq = { DItrackq, {
	{ SZ_INT,     'I', "nro" },	
	{ SZ_CHAR*14, 'C', "teorico" }, 	
	{ SZ_INT,     'I', "cajero" },	
	{0}
}};
#endif


struct DDEnvas_ {
	int func;				
	unsigned long cod;		// Codigo del envase.
	int cant;				// Cantidad vendida/devuelta.
	char flag;				// 0=Devuelto, 1=Vendido.
	unsigned long fecha;	// Fecha de la operacion
	unsigned long hora;		// Hora de la operacion
	int cajero;				// Cajero que hizo la operacion.
};
#ifdef IN_DUMP
dDbf_ xxDDEnvas = { DEnvas, {
	{ SZ_ULONG,	  'L', "cod" }, 
	{ SZ_INT,     'I', "cant" },
	{ SZ_BOOL,    'T', "flag" }, 
	{ SZ_ULONG,   'L', "fecha"}, 
	{ SZ_ULONG,   'L', "hora"}, 
	{ SZ_INT,     'I', "cajero" },
	{0}
}};
#endif

struct DDzeta_ {
	int  func;              // Id de la funcion.
	unsigned long znro;     // Nro de Zeta
	unsigned long tnro;     // Nro de Ticket
	unsigned long fecha;    // Fecha de la operacion.
	int  nro;				// Numero de Mozo.
};
#ifdef IN_DUMP
dDbf_ xxDDZeta = { DZeta, {
	{ SZ_ULONG, 'L', "zeta" }, 
	{ SZ_ULONG, 'L', "ticket" }, 
	{ SZ_ULONG, 'L', "fecha_z" }, 
	{ SZ_INT, 'I', "nro" }, 
	{0}
}};

dDbf_ xxDDZVen = { DZVended, {
	{ SZ_ULONG, 'L', "zeta" }, 
	{ SZ_ULONG, 'L', "ticket" }, 
	{ SZ_ULONG, 'L', "fecha_z" }, 
	{ SZ_INT, 'I', "nro" }, 
	{0}
}};
#endif

struct DCCOnln_ {
	int func;				// Id de la funcion.
	long long ctacte;				// Codigo de la cuenta corriente (0=Desactiva).
	int flagPrec;			// Indica que solo se activo para verificar PRECIO.
};
#ifdef IN_DUMP
dDbf_ xxDDCCOnln = { DCCOnln, {
	{SZ_LLONG, 'X', "nro"}, 
	{SZ_INT, 'I', "ticket"}, 
	{0}
}};
#endif

struct DCobraCC_ {
	int func;				// Id de la funcion.
	int ctacte;				// Codigo de la cuenta corriente.
	char imp[14];			// Monto cobrado.
};
#ifdef IN_DUMP
dDbf_ xxDCobraCC = { DCobraCC, {
	{SZ_INT, 'I', "nro"}, 
	{SZ_CHAR*14, 'C', "imp"}, 
	{0}
}};
#endif
struct DDExtraCash_
{
	int func;               // Id de la funcion.
	char monto[14];            // Monto de la extraccion.	
	int nmpago;			 // medio de pago desde donde se hace la operacion Electron, Visa Debito, etc.
	int cajero;
	int destmpago;			//Medio de pago donde se extra el dinero, Efectivo = 1;
	int fecha;
	int hora;
};

#ifdef IN_DUMP
dDbf_ xxDDExtraCash = { DExtraCash, {
	{ SZ_CHAR*14, 'C', "imp" },
	{ SZ_INT, 'I', "mpag" },	
	{ SZ_INT,		'I', "cajero" },
	{ SZ_INT,		'I', "nro" },
	{ SZ_INT,		'I', "fecha" },
	{ SZ_INT,		'I', "hora" },
	{0}
}};
#endif

//  Estructuras de acumulacion para generar zetas expandidas.
struct acum {
	double cant, monto, cobros;
	char des[80];
	int nro;
};

// Estructura para acumular otros datos.
class Acum
{
	int _cod;
	char *_des;
	int _cant;
	double _monto;
	double _precio;

public:

	int VerCodigo() { return _cod; }
	char *VerNombre() { return _des; }
	int VerCantidad() { return _cant; }
	double VerMonto() { return _monto; }

	void Agrega(int cant, double monto)
	{
		_cant += cant;
		if(monto == 0)
			_monto += (_precio * cant);
		else
			_monto += monto;
	}

	void CambiarPrecio(double precio)
	{
		_precio = precio;
	}

	Acum(int cod)
	{
		_des = (char *)new char[80];
		_cod = cod;
		dbplu.Setorder("mplucod");
		if(dbplu.Seek(padl((double)cod, 6)))
		{
			dbplu.Get("precio", _des);
			_precio = atof(_des);
			dbplu.Get("des", _des);
		}
		else
		{
			sprintf(_des, "Codigo: %d", cod);
		}
		_cant = 0;
		_monto = 0;

	}

	Acum(int cod, char *des)
	{
		_des = (char *)new char[80];
		if(strlen(des) > 78) des[78] = 0;
		STRCPY(_des, des);
		_cod = cod;
		_cant = 0;
		_monto = 0;
	}

	Acum()
	{
		_des = (char *)new char[80];
	}

	~Acum() { delete _des; }
};

// Datos que se acumulan para cada medio de cobro.
class AcumMedio
{
private:
	int cVentas, cCobros, cRetiros, cDotaciones, cOnline, cCobOnl, cExtracc;
	double mVentas, mCobros, mRetiros, mDotaciones, mOnline, mCobOnl, mExtracc;

	int medioNum;
	//char *medioNom;

public:

	void AgregaVenta(double monto, int cant)
	{
		mVentas += monto;
		if(monto > 0) cVentas += cant;
	}

	void AgregaVenta(double monto)
	{
		mVentas += monto;
		if(monto > 0) cVentas++;
	}

	void AgregaVenta2(double monto)
	{
		mVentas += monto;
		//if(monto > 0) cVentas++;
	}

	void AgregaVentaOnline(double monto, int cant)
	{
		mOnline += monto;
		if(monto > 0) cOnline += cant;
	}

	void AgregaVentaOnline(double monto)
	{
		mOnline += monto;
		if(monto > 0) cOnline++;
	}

	void AgregaVentaOnline2(double monto)
	{
		mOnline += monto;
		//if(monto > 0) cOnline++;
	}

	void AgregaCobro(double monto, int cant)
	{
		mCobros += monto;
		if(monto > 0) cCobros += cant;
	}

	void AgregaCobro(double monto)
	{
		mCobros += monto;
		if(monto > 0) cCobros++;
	}

	void AgregaCobro2(double monto)
	{
		mCobros += monto;
		//if(monto > 0) cCobros++;
	}

	void AgregaCobroOnline(double monto, int cant)
	{
		mCobOnl += monto;
		if(monto > 0) cCobOnl += cant;
	}	

	void AgregaCobroOnline(double monto)
	{
		mCobOnl += monto;
		if(monto > 0) cCobOnl++;
	}

	void AgregaCobroOnline2(double monto)
	{
		mCobOnl += monto;
		///if(monto > 0) cCobOnl++;
	}


	void AgregaRetiro(double monto, int cant)
	{
		mRetiros += monto;
		cRetiros += cant;
	}

	void AgregaRetiro(double monto)
	{
		mRetiros += monto;
		cRetiros++;
	}

	void AgregaExtraccion(double monto, int cant)
	{
		mExtracc += monto;
		cExtracc += cant;
	}

	void AgregaExtraccion(double monto)
	{
		mExtracc += monto;
		cExtracc++;
	}
	
	void AgregaDotacion(double monto, int cant)
	{
		mDotaciones += monto;
		cDotaciones += cant;
	}

	void AgregaDotacion(double monto)
	{
		mDotaciones += monto;
		cDotaciones++;
	}

	int VerCantidadVentas()		{ return cVentas + cOnline; }
	int verCantidadVtasOnline()	{ return cOnline; }
	int VerCantidadCobros()		{ return cCobros + cCobOnl; }
	int VerCantidadCobOnline()	{ return cCobOnl; }
	int VerCantidadRetiros()	{ return cRetiros; }
	int VerCantidadDotaciones() { return cDotaciones; }
	int VerCantidadExtracciones() { return cExtracc; }
	double VerMontoRetiros()	{ return mRetiros; }
	double VerMontoDotaciones() { return mDotaciones; }
	double VerMontoVentas()		{ return mVentas + mOnline; }
	double VerMontoOnline()		{ return mOnline; }
	double VerMontoCobros()		{ return mCobros + mCobOnl; }
	double VerMontoCobOnl()		{ return mCobOnl; }
	double VerMontoExtracciones()		{ return mExtracc; }
	//char *VerNombreMedio()		{ return medioNom; }
	int VerNumeroMedio()		{ return medioNum; }

	AcumMedio(int nro)
	{
		//medioNom = (char *)new char[80];
		cVentas = cCobros = cRetiros = cDotaciones = cOnline = cCobOnl = cExtracc = 0;
		mVentas = mCobros = mRetiros = mDotaciones = mOnline = mCobOnl = mExtracc = 0;

		medioNum = nro;
		//sprintf(medioNom, "Medio de Cobro #%d", medioNum);
		for(int i=0; i<cMpag; i++)
		{
			struct mpag_ *mp = &mpagos[i];
			if(mp->nro == medioNum)
			{
				//STRCPY(medioNom, mp->des);
				break;
			}
		}
	}

	AcumMedio() 
	{ 
		//medioNom = (char *)new char[80]; 
	}

	~AcumMedio() 
	{ 
		//delete medioNom; 
	}
};


// Datos que se acumulan para cada Tarjeta.
class AcumTarjeta
{
private:
	int cVentas, cCobros, cOnline, cExtracc, cCobOnl;
	double mVentas, mCobros, mOnline, mExtracc, mCobOnl;

	//int medioNum;
	int numTarj;
	//char *medioNom;

public:
	
	void AgregaVenta(double monto)
	{
		mVentas += monto;
		if(monto > 0) cVentas++;
	}
	
	void AgregaVentaOnline(double monto)
	{
		mOnline += monto;
		if(monto > 0) cOnline++;
	}

	void AgregaCobro(double monto)
	{
		mCobros += monto;
		if(monto > 0) cCobros++;
	}

	void AgregaCobroOnline(double monto)
	{
		mCobOnl += monto;
		if(monto > 0) cCobOnl++;
	}
	
	void AgregaExtraccion(double monto)
	{
		mExtracc += monto;
		cExtracc++;
	}	

	int VerCantidadVentas()		{ return cVentas + cOnline; }
	int verCantidadVtasOnline()	{ return cOnline; }	
	int VerCantidadExtracciones() { return cExtracc; }
	double VerMontoVentas()		{ return mVentas + mOnline; }
	double VerMontoOnline()		{ return mOnline; }
	double VerMontoExtracciones()		{ return mExtracc; }
	//char *VerNombreMedio()		{ return medioNom; }
	int VerNumeroTarjeta()		{ return numTarj; }

	AcumTarjeta(int nroTarj)
	{
		//medioNom = (char *)new char[80];
		cVentas = cOnline = cExtracc = 0;
		mVentas = mOnline = mExtracc = 0;

		numTarj = nroTarj;		
	}

	AcumTarjeta() 
	{ 
		//medioNom = (char *)new char[80]; 
	}

	~AcumTarjeta() 
	{ 
		//delete medioNom; 
	}
};

class QuickCajero
{
private:
	double ventas, cash, cashInDrawer, otros, retiros;
	int hora, cajNum, cantOpe, cantReti;
	::ArrayList *ventaMedios;

public:
	int CajeroNum()                       { return cajNum; }
	int VerCantOperac()                   { return cantOpe; }
	double VerVentas()                    { return ventas; }
	void SumarVentas(double vta)          { ventas += vta; cantOpe++; }
	double VerCash()                      { return cash; }
	void SumarCash(double newCash)        { cash += newCash; }
	double VerCashInDrawer()              { return cashInDrawer; }
	void SumarCashInDrawer(double newCid) { cashInDrawer += newCid; }
	double VerOtros()                     { return otros; }
	void SumarOtros(double newOtros)      { otros += newOtros; }
	double VerRetiros()					  { return retiros; }
	int VerCantidadRetiros()			  { return cantReti; }
	void SumarRetiros(double newReti)	  { if(newReti < 0) { retiros -= newReti; cantReti++; }}
	int VerHora()                         { return hora; }
	void SetHora(int _hora)               { hora = _hora; }
	::ArrayList *VerVentaMedios()         { return ventaMedios; }

	void SumarMedio(int medio, double m)
	{
		acum *ac = (acum *)ventaMedios->Find(medio);
		if(ac == NULL)
		{
			ac = new acum();
			ac->nro = medio;
			ac->cant = 0;
			ac->monto = 0;
			ventaMedios->Add(medio, ac);
		}

		ac->cant += 1;
		ac->monto += m;
	}

	QuickCajero(int _cajNum)
	{
		cajNum = _cajNum;
		retiros = ventas = cash = cashInDrawer = otros = 0.0;
		cantReti = cantOpe = hora = 0;
		ventaMedios = new ::ArrayList(true, "QuickCajeroCtor");
	}

	~QuickCajero()
	{
		delete ventaMedios;
	}
};

class AcumCajero
{
private:
	int cajNum;
	char *cajNom;
	::ArrayList *ventas;
	::ArrayList *tarjetas;
	::ArrayList *envases;
	::ArrayList *envVend;
	::ArrayList *cobros;

	int cAnul1, cAnul2, cAnul3;
	double mAnul1, mAnul2, mAnul3;

	int cEnvCob;		//Cantidad de enviados a cobradora
	double mEnvCob;		//Monto de enviados a cobradora

	int cRetiros;
	double mRetiros;

	int cCobroCC;
	double mCobroCC;

	int cPercep;
	double mPercep;

	int cTissh;
	double mTissh;

	int cPercepIva;
	double mPercepIva;

	int cExtracc;
	double mExtracc;

	int cAnulTarj;
	double mAnulTarj;

	int cAnulSosMovil;
	double mAnulSosMovil;

	int cSenia;
	double mSenia;
	
	int cCantPromo;
	//double mMontoPromo;

public:

	int CajeroNum()	{ return cajNum; }

	char *CajeroNombre() { return cajNom; }

	AcumMedio *VerAcumuladorMedio(int medio)
	{
		AcumMedio *m = (AcumMedio *)ventas->Find(medio);
		if(m == NULL)
		{
			m = new AcumMedio(medio);
			ventas->Add(medio, m);
		}
		return m;
	}

	AcumTarjeta *VerAcumuladorTarjeta(int nroTarj)
	{
		AcumTarjeta *m = (AcumTarjeta *)ventas->Find(nroTarj);
		if(m == NULL)
		{
			m = new AcumTarjeta(nroTarj);
			tarjetas->Add(nroTarj, m);
		}
		return m;
	}

	Acum *VerAcumuladorEnvases(int cod)
	{
		Acum *env = (Acum *)envases->Find(cod);
		if(env == NULL)
		{
			env = new Acum(cod);
			envases->Add(cod, env);
		}
		return env;
	}

	Acum *VerAcumuladorEnvasesVendidos(int cod)
	{
		Acum *env = (Acum *) envVend->Find(cod);
		if(env == NULL)
		{
			env = new Acum(cod);
			envVend->Add(cod, env);
		}
		return env;
	}

	Acum *VerAcumuladorCobros(int cod)
	{
		Acum *cob = (Acum *)cobros->Find(cod);
		if(cob == NULL)
		{
			cobro_ *def = BuscarCobro(cod);
			cob = new Acum(cod, def->nombre);
			cobros->Add(cod, cob);
		}
		return cob;
	}

	::ArrayList *VentaMedios() { return ventas; }

	::ArrayList *TarjetasHechas() { return tarjetas; }

	::ArrayList *Envases() { return envases; }

	::ArrayList *EnvasesVendidos() { return envVend; }

	::ArrayList *Cobranzas() { return cobros; }

	void AnulaRenglon(int cant, double monto)
	{
		cAnul1 += cant;
		mAnul1 += monto;
	}

	void AnulaTicket(double monto)
	{
		cAnul2++;
		mAnul2 += monto;
	}

	void AnulaTicket(int cant, double monto)
	{
		cAnul2 += cant;
		mAnul2 += monto;
	}

	void Suspension(double monto)
	{
		cAnul3++;
		mAnul3 += monto;
	}

	void Suspension(int cant, double monto)
	{
		cAnul3 += cant;
		mAnul3 += monto;
	}

	void EnviadoCobradora(double monto)
	{
		cEnvCob++;
		mEnvCob += monto;
	}

	void EnviadoCobradora(int cant, double monto)
	{
		cEnvCob += cant;
		mEnvCob += monto;
	}


	void Retiro(double monto)
	{
		if(monto < 0)
		{
			cRetiros++;
			mRetiros -= monto;
		}
	}

	void Percepcion(double monto)
	{
		cPercep++;
		mPercep += monto;
	}

	void Percepcion(int cant, double monto)
	{
		cPercep += cant;
		mPercep += monto;
	}

	void Tissh(double monto)
	{
		cTissh++;
		mTissh += monto;
	}

	void Tissh(int cant, double monto)
	{
		cTissh += cant;
		mTissh += monto;
	}

	void PercepIva(double monto)
	{
		cPercepIva++;
		mPercepIva += monto;
	}

	void PercepIva(int cant, double monto)
	{
		cPercepIva += cant;
		mPercepIva += monto;
	}

	void Extraccion(double monto)
	{
		cExtracc++;
		mExtracc += monto;
	}

	void Extraccion(int cant, double monto)
	{
		cExtracc += cant;
		mExtracc += monto;
	}

	void AnulTarj(double monto)
	{
		cAnulTarj++;
		mAnulTarj += monto;
	}

	void AnulTarj(int cant, double monto)
	{
		cAnulTarj += cant;
		mAnulTarj += monto;
	}

	void AnulSosMovil(double monto)
	{
		cAnulSosMovil++;
		mAnulSosMovil += monto;
	}

	void AnulSosMovil(int cant, double monto)
	{
		cAnulSosMovil += cant;
		mAnulSosMovil += monto;
	}

	void CobranzaCC(double monto)
	{
		cCobroCC++;
		mCobroCC += monto;
	}

	void Retiro(int cant, double monto)
	{
		cRetiros += cant;
		mRetiros += monto;
	}

	void AcumSenia(double monto)
	{
		cSenia++;
		mSenia += monto;
	}

	void AcumSenia(int cant, double monto)
	{
		cSenia += cant;
		mSenia += monto;
	}

	void IncrCantPromo()
	{
		cCantPromo++;
	}

	int VerCantidadCobroCC()
	{
		return cCobroCC;
	}

	double VerMontoCobroCC()
	{
		return mCobroCC;
	}

	int VerCantidadRetiros()
	{
		return cRetiros;
	}

	int VerCantidadPercepciones()
	{
		return cPercep;
	}

	int VerCantidadTissh()
	{
		return cTissh;
	}

	int VerCantidadPercepIva()
	{
		return cPercepIva;
	}

	int VerCantidadExtracciones()
	{
		return cExtracc;
	}

	int VerCantidadAnulTarj()
	{
		return cAnulTarj;
	}

	int VerCantidadAnulSosMovil()
	{
		return cAnulSosMovil;
	}

	int VerCantidadSenia()
	{
		return cSenia;
	}



	double VerMontoRetiros()
	{
		return mRetiros;
	}

	double VerMontoPercepciones()
	{
		return mPercep;
	}

	double VerMontoTissh()
	{
		return mTissh;
	}

	double VerMontoPercepIva()
	{
		return mPercepIva;
	}

	double VerMontoExtracciones()
	{
		return mExtracc;
	}
	
	double VerMontoAnulTarj()
	{
		return mAnulTarj;
	}

	double VerMontoAnulSosMovil()
	{
		return mAnulSosMovil;
	}

	double VerMontoSenia()
	{
		return mSenia;
	}

	void VerAnulacionRenglones(int *cant, double *monto)
	{
		*cant = cAnul1;
		*monto = mAnul1;
	}

	void VerAnulacionTotal(int *cant, double *monto)
	{
		*cant = cAnul2;
		*monto = mAnul2;
	}

	void VerSuspensiones(int *cant, double *monto)
	{
		*cant = cAnul3;
		*monto = mAnul3;
	}

	void VerEnviadoCobradora(int *cant, double *monto)
	{
		*cant = cEnvCob;
		*monto = mEnvCob;
	}

	void VerPercepciones(int *cant, double *monto)
	{
		*cant = cPercep;
		*monto = mPercep;
	}

	void VerTissh(int *cant, double *monto)
	{
		*cant = cTissh;
		*monto = mTissh;
	}

	void VerPercepIva(int *cant, double *monto)
	{
		*cant = cPercepIva;
		*monto = mPercepIva;
	}

	void VerExtracciones(int *cant, double *monto)
	{
		*cant = cExtracc;
		*monto = mExtracc;
	}

	void VerAnulTarj(int *cant, double *monto)
	{
		*cant = cAnulTarj;
		*monto = mAnulTarj;
	}

	void VerAnulSosMovil(int *cant, double *monto)
	{
		*cant = cAnulSosMovil;
		*monto = mAnulSosMovil;
	}

	void VerSenia(int *cant, double *monto)
	{
		*cant = cSenia;
		*monto = mSenia;
	}

	int VerCantPromo(){
		return cCantPromo;
	}

	AcumCajero(int cajeroNum)
	{
		char ckaCod[10];

		cajNum = cajeroNum;
		cajNom = (char *)new char[80];
	    sprintf(ckaCod, "%4d", cajNum);
		if(dbclk.Locate("codigo", ckaCod))
			dbclk.Get("nombre", cajNom);
		else
			sprintf(cajNom, "Cajero #%d", cajNum);

		Trim(cajNom);
		ventas  = new ::ArrayList(true, "AcumCajeroCtor-Ventas");
		tarjetas  = new ::ArrayList(true, "AcumCajeroCtor-Tarjetas");
		envases = new ::ArrayList(true, "AcumCajeroCtor-Envases");
		envVend = new ::ArrayList(true, "AcumCajeroCtor-EnvVend");
		cobros  = new ::ArrayList(true, "AcumCajeroCtor-Cobros");

		cAnul1 = cAnul2 = cAnul3 = 0;
		mAnul1 = mAnul2 = mAnul3 = 0;

		cRetiros = 0;
		mRetiros = 0;

		cCobroCC = 0;
		mCobroCC = 0;

		cPercep = 0;
		mPercep = 0;

		cTissh = 0;
		mTissh = 0;

		cExtracc = 0;
		mExtracc = 0;

		cAnulTarj = 0;
		mAnulTarj = 0;

		cAnulSosMovil = 0;
		mAnulSosMovil = 0;

		cEnvCob = 0;
		mEnvCob = 0;

		cSenia = 0;
		mSenia = 0;
	}

	~AcumCajero()
	{
		delete ventas;
		delete tarjetas;
		delete envases;
		delete envVend;
		delete cobros;
		delete cajNom;
	}
};


/**************************************************************************************************
 *****                                                                                        *****
 *****  Las siguientes uniones y arrays permiten linkear todas las funciones declaradas para  *****
 *****  su utilizacion desde los distintos modulos, de una forma extensible.                  *****
 *****                                                                                        *****
 **************************************************************************************************/
struct fijo_ {
	int id;
	char fijo[176];
};

union Dump_ {
	struct DDdep_ depto;				// Departamento.
	struct DDmpag_ mpag;				// Medios de Pago.
	struct DDeop_ eop;					// Final de Operacion.
	struct DDplu_ plu;					// Venta x Articulo.
	struct DDchmode_ mode;				// Cambio de modo.
	struct DDcorri_ corri;				// Anulacion de un renglon previo.
	struct DDzeta_ zeta;				// Cierre de Cajas
	struct DDFactu1_ fac1;				// Paso 1 de refacturacion.
	struct DDFactu2_ fac2;				// Paso 2 de refacturacion.
	struct DDMisc_ misc;				// Retiros/Dotaciones.
	struct DDclk_ clk;					// Conexion/Desconexion de Cajero.
	struct DDEnvas_ env;				// Tickets de Envases.
	struct DDven_ ven;					// Mozo Activo.
	struct DPTarea_ pt;					// Activacion tarea pre-programada.
	struct DCCOnln_ onln;				// Cuenta Corriente con seguimiento.
	struct DCobraCC_ cobra;				// Cobranza de cuenta corriente.
	struct DDecla_  decla;				// Declaracion de cajero
	struct DDeclaDT_ decladt;			// Detalle de declaracion de cajero
	struct DSuspende_ susp;				// Detalle de suspension de ticket.
	struct DCobranza_ ppago;			// Cobranza de servicios.
	struct DDSuperv_ superv;			// Login/Logout de supervisor
	struct DPromo_ promo;				// Realizacion de premios de promociones
	struct DCellLoad_ cell;				// Carga de celulares
	struct DDFactu3_ fac3;				// Percepción
	struct DDExtraCash_ dextra;		//Extraccion de efectivo	
	struct DDFactu4_ fac4;				// Percepción Salta TISSH
	struct DDFactu5_ fac5;				// Percepción IVA RG 5329
	struct DDVuelto_ dvuelto;			//Para Vuelto Seguro
	struct DDanul_ fanul;				//Para anulaciones de Tarjetas y SOS Movil
	struct DDitrackq_ fitrackq;			//Para operaciones de ITrackq

	struct DDNcItem_ itemNc;			//Para Items de Nota Credito	
	struct DDNcPercep_ percepNc;		//Percepcion en Nota de Credito
	struct DDNcPago_ pagoNc;			//Para los pagos de Nota Credito
	struct DCompDanul_ compAnul;		//Para acumular los tickets anulados luego de reinicar la compu
	struct DVendMay_ vendedorMayor;		//Para tener un registro de un vendedor
	struct DSenia_ seniaMP;				//Para Tener un registro de la seña entregada por un cliente
	struct fijo_ f;						// Para forzarle a la UNION un largo fijo
};


#ifdef IN_DUMP
dDbf_ *xxDump[] = 
{ 
	&xxDOtrop,		&xxDDecla,		&xxDDeclaDT,	&xxDDMisc,			&xxDDFactu1,
    &xxDDFactu2,	&xxDDdep,		&xxDDcorri,		&xxDDchmode,		&xxDDplu, 
	&xxDDclk,		&xxDDven,		&xxDDmpag,		&xxDDeop,			&xxDDEnvas, 
	&xxDDZeta,		&xxDPTarea,		&xxDRepar,		&xxDDZVen,			&xxDDTickFac, 
	&xxDDCCOnln,	&xxDCobraCC,	&xxDSuspende,	&xxDDCobranza,		&xxDDsuperv,
	&xxDDPromo,		&xxDDCellLoad,	&xxDDFactu3,	&xxDDExtraCash,		&xxDDVuelto,
	&xxDDFactu4,	&xxDDanul,		&xxDDitrackq,	&xxDDNcItem,		
	&xxDDNcPercep,	&xxDDNcPago,	&xxDDFactu5,	&xxDDCompAnul,		&xxDVendMay,
	&xxDSeniaMP,
};
#endif

#pragma pack()
