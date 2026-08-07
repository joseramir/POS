////   
////   MapTbl.h
////
//
//#pragma once
//
////   Variables globales
//extern int wMapH, WndX, WndY, WndXX, WndYY, szObj;
//extern struct tbl_ {
//	int y, x;			// Coordenada donde está ubicada.
//	int nro;			// 0=Sin asignar. 1..n = Nro asignado.
//	struct tbl_ *next;	// Siguiente en la lista.
//	char flag;			// Estado actual de la mesa.
//} *tbl;
//
//extern struct text_ {
//	int y, x;			// Coordenada donde está ubicado.
//	char *texto;		// Texto contenido.
//	int delFlag;		// 1=Si está deleteado.
//	short color;		// Color a utilizar.
//	struct text_ *next;	// Siguiente en la lista.
//} *textos;
//
//extern struct line_ {
//	int y, x;			// Coordenada donde comienza.
//	int delFlag;		// 1=Si está deleteado.
//	int cantidad;       // Cantidad de caracteres.
//	char fg_hori;		// TRUE si el movimiento es horizontal.
//	short color;		// Color a utilizar.
//	struct line_ *next;	// Siguiente en la lista.
//} *lineas;
//
////   Esquema de colores utilizados.
//extern short tblesq[16];
//
////   Estados de una mesa.
//#define TABLE_EMPTY		0
//#define TABLE_INCURSE	1
//#define TABLE_BUSY		2
//
////   Prototipos
//void LoadMapDef(void);
//void DrawMapWindow(void);
//void WriteMapDef(void);
//void DrawTable(tbl_ *t, int bold);
//int  AnimateGraf(int StartTbl);
//void SetTableStatus(int table, int status);
//char GetTableStatus(int nro);
//void DrawTexto(text_ *x);
//void DrawLine(line_ *l);