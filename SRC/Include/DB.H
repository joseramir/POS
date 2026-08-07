//
//   Db.hpp
//   Prototipos del motor de acceso a bases de Foxpro.
//

#pragma once
//#define TESTIDX
#ifndef _DBH_INCLUDED_
#define _DBH_INCLUDED_

// Los "include" deben figurar ANTES de los pragma
#include "ArrayList.h"

#pragma pack(push)
#pragma pack(1)

//   Definición de la clase que maneja el i/o de los archivos.
class xFile
{
public:
	char lfn[200];							// Copia del nombre del archivo usado.
	xFile *next;						// Lista linkeada de archivos abiertos.

private:
	int h;
	char *chFile;						// Buffer donde cacheo el archivo.
	long chPos;							// Posicion actualmente seekeada.
	long szMem;							// Largo del archivo.
	int OpenFlags1, OpenFlags2;			// Para poder reemitir el sopen(...)
	struct flags_ {
		int opened:1;					// Archivo abierto.
		int loaded:1;					// Si pudo cachearlo entero.
		int writePend: 1;				// Si hay escrituras pendientes.
		int iscursor: 1;				// Borrar al cerrar.
		int reopen:1;					// El archivo debe ser REABIERTO.
	} flags;

public:
	char *Error;						// Codigo de Error de OPEN
	xFile();							//  Constructor y Destructor.
	~xFile();
	int  xOpen(char *, int, int);		// sopen(...)
	int  xCreate(char *, int);			// creat(...)
	int  xClose(void);					// close(...)
	int  xRead(void *, int);			// read(...)
	int  xWrite(void *, int);			// write(...)
	long xLseek(long, int);				// lseek(...)
	int  xLocking(int, long);			// locking(...)
	long xTell(void);					// tell(...)
	int  xChsize(long);					// _chsize(...)
	long xFilelen(void);				// filelength(...)
	int  xEof(void);					// eof(...)
	int  xCache(int);					// Esquema a usar.
	void xCursor(void);					// Flaguea como cursor.
	void xFlush(void);					// Vacia buffers.
	int  xOpened(void);					// Indica si hay archivo ab.
	void xReopen(void);					// Reabre un archivo.
	void xTmpClose(void);				// Cierra temporariamente un archivo.
};

//  Distintos tipos de caches a activar. (Si no se indica ninguno,
//  se pasan los pedidos directamente a las funciones de bajo nivel).
#define CACHE_LOAD_FILE	1

// Reemplaza la extension.
void ReplaceExtension(char *fn, char *ext, bool allways);

// Retorna todos el nombre de los archivos que cumplen con una mascara
::ArrayList *FindFiles(char *mask);

// Retorna la fecha y hora de la ultima vez que se modifico el archivo
struct tm *LastAccessFile(char *fn);

//  Las funciones de transacción están aparte, dado que se aplican a todos
//  los archivos que estén abiertos al momento.
int  xTTStart(void);					// Comienza una transacción.
int  xTTCommit(void);					// Confirma una TT y descarga a disco.
int  xTTRollback(void);					// Deshace una transacción.
void xTTBusy(void);						// Mientras se espera.

//  Encabezamiento de las dbfs, como se encuentra en el archivo.
struct dbHeader {
	char dbID;							// Identificador del tipo de archivo
	char LastUpdate[3];					// Fecha de la ultima modificacion
	unsigned long RecCount;				// Cantidad de registros en el archivo
	unsigned short FirstDataByte;		// Posicion del primer Byte de datos.
	unsigned short LenRecord;			// Cantidad de Bytes que ocupa un registro.
	char PADD[20];						// Completa los 32 bytes.
};

struct dbField {
	char name[11];						// Nombre del campo (max. 10 caracteres)
	char DataType;				        // Definicion del tipo de dato.
	unsigned long OffsetInRec;			// Posicion del campo, en un registro
	unsigned char FieldLen;				// Largo en bytes del campo
	unsigned char FieldDec;			    // Si corresponde, cantidad de decimales.
	char PADD[32-18];					// Para completar los 32 bytes que ocupa en el file.
};

//   Parámetros posibles para rLock()
#define		DB_UNLOCK	0
#define		DB_LOCK		1

//   Parámetros posibles para dbGo()
#define		DB_GO_TOP	    -1
#define		DB_GO_BOTTOM  100000000

//   Definiciones que hacen al campo 'dbID' del 'dbHeader'   
#define     NOMEMO      '\x03'  // FoxBASE+, dBASE III Plus, Foxpro S/MEMO 
#define     MEM_DB3     '\x83'  // FoxBASE+, dBASE III Plus CON MEMO  
#define     MEM_FXP     '\xf5'  // FoxPRO CON MEMO 
#define     MEM_DB4     '\x8b'  // dBASE IV CON MEMO  

//   Definiciones que hacen al campo 'DataType' de un 'dbField'  
#define     CHAR        'C'     //  Caracteres en general  
#define     NUM         'N'     //  Numericos en general (ascii)  
#define     FLOAT       'F'     //  Numericos en general (double)  
#define     LOGIC       'L'     //  Datos logicos (.t. / .f.)  
#define     MEMO        'M'     //  Memos  
#define     GENER       'G'     //  Windows / OLE-DDE  
#define     DATE        'D'     //  Fechas: yyyymmdd  
#define     PICTURE     'P'     //  Windows / BMP-PIC-...  

//   Listado de errores permitidos.

extern char *dberrMSG[];     

#define DB_NO_ERROR			0
#define DB_FILE_NOT_FOUND	1
#define DB_NOT_DB           2
#define DB_FILE_EMPTY		3
#define DB_NO_MEMORY		4
#define DB_NO_DBOPEN		5
#define DB_BAD_RECNUM		6
#define DB_REC_LOCKED		7
#define DB_FLD_NOTFOUND		8
#define DB_REC_NO_LOCKED	9
#define DB_FILE_EXCLU	   10
#define DB_EOF_ENCOUNT	   11
#define DB_BOF_ENCOUNT     12
#define DB_BAD_IDXKEY	   13
#define DB_IDX_EXCLU       14
#define DB_IDX_NOTFOUND    15
#define DB_CREATE_SINTAX   16

//   Estructura que contiene todas las variables de ambiente, de las
//   que depende el view actual.

class view_
{
public:
	int _exclusive;				// Flag de acceso exclusivo.
	int dberr;					// Nro del último error producido.
	int _deleted;				// Saltea o no los deleteados.
	unsigned long recno_0;		// Resultado del último seek.

	//   Constructor. Inicializa las variables de control.
	view_()					
	{
		dberr = 0;
		_deleted = 1;
		_exclusive = 1;
	}
};

//   Forma en que se graban las estructuras en los archivos .idx
struct idxHeader {
	   unsigned long  RootNode; //  Puntero al nodo raiz   
	   unsigned long  FreeNode; //  Puntero a la lista de nodos libres 
	   unsigned long  EofNode;  //  Puntero al final del archivo   
	   unsigned short LenKey;   //  Largo de la clave  
	   char idxFlag;            //  Banderas de modo   
	   char signature;          //  Identificacion  
	   char key[220];           //  Expresion clave  
	   char forc[220];          //  Expresion filtro  
};

struct idxNodeHd				//  La usa reindex.
{
	   short attrib;			//  Atributo del nodo.
	   short nkey;				//  Cantidad de claves.
	   long  nleft;				//  Nodo directamente a la izq.
	   long  nright;			//  Nodo directamente a la der.
};

struct idxNode
{
	   short attrib;            //  Atributo del nodo   
	   short nkey;              //  Cantidad de claves en el nodo  
	   long nleft;              //  Nodo directamente a la izquierda 
	   long nright;             //  Nodo directamente a la derecha 
	   char data[1000];         //  Contenido de hasta 2 nodos
};

#define     IDH_UNIQUE      1   //  No permite claves repetidas 
#define     IDH_FILTER      8   //  Utiliza la clausula FOR 

#define     ATT_ROOT        1   //  El nodo actual es el raiz   
#define     ATT_LEAF        2   //  El nodo actual es extremo   

//   Definición de la clase que maneja un archivo de indice.
struct idx
{
	struct   idxHeader hd;       //  Encabezamiento   
	struct   idxNode   node;     //  Nodo actual  
	char     *idName;		    //  Nombre del archivo.
	char	    *ixNic;				//  NIC a utilizar en SET ORDER TO...
	class Expression *keyexpr;	// Expresion compilada con la clave.
	xFile    fh;					//  Manejador del archivo físico.
	char     keyType;            //  Tipo de la clave (No esta en idx) 
	char     *tempKey;           //  Lugar para la clave de trabajo  
	char     *reKey;				//  Lugar para la clave de reseek.
	long	    NodeTree[50];       //  Arbol de nodos utilizados.   
	unsigned NodeTreeCount;      //  Contador de Nodos Utilizados. 
	unsigned long lFile;         //  Para detectar corrupciones  
	char     *KeyFound;			//  Donde reseek vuelve el ptr.
	struct   idx *next;          //  Siguiente idx en la lista   
};

//   Estructura para almacenar relaciones.
struct dbRelation {
	class dbf *target;				// Base relacionada.
	char *field;				// Campo relacionado.
	dbRelation *next;			// Siguiente relación.
};

//   Definición de la clase que maneja un archivo de base de datos.
class dbf
{
private:
	struct dbHeader dbhd;		// Header leido.
	char *dbName;				// Nombre del archivo.
	xFile fh;					// Para manejo del archivo físico.
	struct dbField *Flds;		// Lista de campos.
	unsigned char *uRec;		// Record actual.
	unsigned short cFlds;		// Cantidad de campos.
	unsigned long recno;		// Record actual posicionado.
	dbRelation *rel;			// Relaciones activas.
	struct flags_ {				// Flags del estado de la base.
		int in_use: 1;			// La base está en uso.
		int eof: 1;				// Está posicionado más allá del final.
		int wri: 1;				// Hay escrituras pendientes.
		int bof: 1;				// Está antes del principio.
		int excl: 1;			// Se abrió en forma exclusiva.
		int rdy: 1;				// El registro leído NO ESTA en buffer.
		int lock: 1;			// El registro actual está trabado.
		int wOnce: 1;			// El registro está en TT automática.
		int iscursor: 1;		// Borrar al cerrar.
		int wCdx: 1;			// Tiene CDX estructural atachado.
	} flags;

	struct idx *idxlist;		// Lista de indices abiertos.
	struct idx *idxmaster;		// Indice controlador (NULL=ninguno)
	char *activeKey;			// Clave del indice controlador.
	char tempKey[256];			// Usado en los procesos de indexado
	
	//  Funciones privadas.
	void flushWrite(void);	    // Vacía escrituras pendientes.
	void closeIndex(void);		// Cierra indices activos.

	unsigned long idxSearch(struct idx *, char *);
	void DeleteKey(unsigned long, struct idx *);
	void InsertKey(void *, char *, struct idx *);
	void idxSkip(int, struct idx *);
	int  idxGo(int, struct idx *);
	int  dbGo(unsigned long nrec);
	int  idxReseek(struct idx *);
	void idxZap(struct idx *);
	void getActualRecord(void);
	void CheckIndexUpdate(char *, char *, char *, int);
	void InsertBlankKey(void);
	void GoRelation(void);
	void DefaultTally(int, char *);
	void lReindex(struct idx *,				// Reconstruye un indice.
                  void (*)(unsigned long));

public:
	char *Error;
	dbf(void);								// Constructor.
	~dbf(void);								// Destruye la estructura.
	#ifdef TESTIDX
	int CheckIdx(void);						// Valida la estructura.
	#endif

	int  Use(char *fn);						// Abre el archivo.
	void Close(void);						// Cierra el archivo.
	int  Go(unsigned long rec);				// Va a determinado registro.
	int  Lock(int acc);						// Destraba/Traba el registro actual.
	int  Get(char *fld, char *buf);			// Extrae datos de un campo.
	int  xGet(char *fld);					// Extrae datos y coloca en xString.
	int  FieldLen(char *fld);				// Retorna el largo en bytes de 1 fld.
	int  FieldDec(char *fld);				// Retorna el valor de la parte decimal.
	char FieldType(char *fld);				// Retorna el tipo de un campo.
	struct dbField *getField(char *name);	// Retorna todos los datos de una columna
	int  Replace(char *fld, char *exp);		// Reemplaza datos de un campo.
	int  AppendBlank(void);					// Agrega un registro en blanco.
	void Zap(void);							// Elimina TODOS registros.
	int  Pack(void);						// Elimina los registros borrados.
	void SetRelation(dbf *, char *);		// Establece una relacion.
	void ClearRelation(dbf *target);		// Elimina una relación.

	int  Bof(void);							// Retorna el flag BOF.
	int  Eof(void);							// Retorna el flag EOF.
	unsigned long RecCount(void);			// Retorna el nro de registros.
	unsigned long Recno(void);				// Retorna el nro de registro.
	int  Deleted(void);						// TRUE si el record está borrado.
	int  Skip(long offset);					// Salta registros.
	int  Seek(char *key);					// Busca en indices.
	void Setorder(char *key);				// Cambia índice activo.
	const char *GetOrder();					// Obtiene la clave del indice actual
    int  SetIndex(char *fn);				// Abre y asocia un indice.
	int  Locate(char *fld, char *exo);		// Busca secuencial

	int  IndexOn(char *key, char *fn);		// Crea un indice y lo activa.
	void Reindex(void);						// Reconstruye todos los indices.
	int  creaTable(char *fn, char *fld);	// Crea una base de datos.
	int  creaCursor(char *fn, char *fld);	// Crea una base cursor.
	void InsertFrom(dbf &src);				// Copia de una base a otra.
	void Flush(void);						// Descarga datos a disco.
	void Delete(void);						// Borra el registro actual.
	void Trunc(unsigned long rec);			// Trunca en det. registro.
	int  Opened(void);						// Indica si esta abierto.
	int  *AllIndexEntries(void);			// Devuelve la lista completa de recnos indexados.
};

extern char pbuff[];	// Definido en db.cpp
extern view_ view;		// La declara db.cpp
void testDb(void);
void chkE(int);			// Verifica el resultado.
#pragma pack()

//   Otras funciones globales.
void Syncronize(int);			// Sincroniza archivos.
int  xCopyFile(char *src, char *dst, char *help, bool usaTemp);	// Copia un archivo a otro. - En sync.cpp
void TemporaryClose(char *fn);	// Se asegura que 'fn' essté cerrado.
void FlushXPBuffers(void);		// Limpia todos los buffers de escritura.

//   Constantes varias.
#define SZ_NODE		512			/* Tamaño de un nodo, en bytes */
#define MAXFIELDS   100			/* Cant Max. de campos por base */

#pragma pack(pop)

// Control de la tabla seleccionada para el resolvedor de nombres
void SetTemporarySelected(dbf *_dbf);
void RestoreTemporarySelected();

#endif //				_DBH_INCLUDED_
