////
////   MapHand.cpp
////
//
#include <stdafx.h>
//
////   Variables globales.
//int wMapH, WndX, WndY, WndXX, WndYY, cTbl, defLoaded, szObj;
//text_ *textos;
//line_ *lineas;
//
////   Esquema de colores a utilizar.
//short tblesq[16] = {
//	{ 16 * AZUL     + MARRON },		// Color del borde
//	{ 16 * BLANCO   + NEGRO },		// Titulo a ventana activa.
//	{ 16 * NEGRO    + BLANCO },		// Titulo a ventana inactiva.
//	{ 16 * MARRON   + BLANCO },		// Texto contenido.
//    { 16 * NEGRO    + MARRON },		// Color del fondo cuando usa relleno.
//	{ 16 * ROJO     + BLANCO },		// Color para mesas normales
//	{ 16 * VERDE    + NEGRO },		// Color para la mesa actual.
//	{ 16 * AMARILLO + NEGRO },		// Color para mesas usadas.
//};
//
//char G_TABLES[] = "tables";			// Grupo [tables] en el ini.
//char G_WINDOW[] = "window";			// Grupo [window] en el ini
//char G_TEXTOS[] = "textos";			// Grupo [textos] en el ini.
//char G_LINEAS[] = "lineas";			// Grupo [lineas] en el ini.
//
////   Cambia el flag de estado de una mesa.
//void SetTableStatus(int nro, int nStat)
//{
//	tbl_ *t;
//
//	if(!defLoaded) { LoadMapDef(); defLoaded = 1; }
//	if(!nro) return;
//	for(t = tbl; t; t = t->next)
//		if(t->nro == nro)
//		{
//			t->flag = (char) nStat;
//			return;
//		}
//}
//
////   Retorna el estado de una mesa cualquiera.
//char GetTableStatus(int nro)
//{
//	tbl_ *t;
//
//	if(!defLoaded) { LoadMapDef(); defLoaded = 1; }
//	if(!nro) return TABLE_EMPTY;
//	for(t = tbl; t; t = t->next)
//		if(t->nro == nro)
//			return t->flag;
//	return TABLE_EMPTY;
//}
//
////   Busca el objeto mas cercano, arriba o abajo del que se le in-
////   dica. Si encuentra uno, retorna el puntero al mismo.
////   El parametro 'donde' es 1 si busca abajo o -1 si busca arriba.
//tbl_ *FindY(tbl_ *act, int donde)
//{
//	tbl_ *t, *u;
//	int difeX, difeY, dX, dY;
//
//	u = 0;
//	difeX = difeY = 9999;
//	for(t = tbl; t; t = t->next)
//		if(t != act && (donde < 0)? (t->y < act->y): (t->y > act->y))
//		{
//			dX = abs(t->x - act->x) / 5;
//			dY = abs(t->y - act->y);
//			if(dX < difeX)
//			{
//				difeX = dX; difeY = dY;
//				u = t;
//			}
//			else
//				if((dX == difeX) && (dY < difeY))
//				{
//					difeX = dX; difeY = dY;
//					u = t;
//				}
//		}
//	return u;
//}
//
////   Busca el objeto mas cercano, a derecha o izquierda del que se le
////   indica. Si encuentra uno, retorna el puntero al mismo.
////   El parametro 'donde' es 1 si busca derecha o -1 si busca izq.
//tbl_ *FindX(tbl_ *act, int donde)
//{
//	tbl_ *t, *u;
//	int difeX, difeY, dX, dY;
//
//	u = 0;
//	difeX = difeY = 9999;
//	for(t = tbl; t; t = t->next)
//		if(t != act && (donde < 0)? (t->x < act->x): (t->x > act->x))
//		{
//			dX = abs(t->x - act->x);
//			dY = abs(t->y - act->y) / 5;
//			if(dY < difeY)
//			{
//				difeX = dX; difeY = dY;
//				u = t;
//			}
//			else
//				if((dY == difeY) && (dX < difeX))
//				{
//					difeX = dX; difeY = dY;
//					u = t;
//				}
//		}
//	return u;
//}
//
////   Anima el grafico. Devuelve 0 si salio con ESC o no existe
////   mesas definidas en el mapa. Sino, retorna el nro de mesa
////   seleccionado por el usuario.
//int  AnimateGraf(int startTbl)
//{
//	int seguir, ch;
//	tbl_ *t, *u;
//
//	//   Inicializa el subsistema, si corresponde.
//	if(!defLoaded) { LoadMapDef(); defLoaded = 1; }
//	if(!tbl) return 0;
//	DrawMapWindow();
//
//	//   Busca la 'table' indicada. Si es cero, busca la primera
//	//   que encuentre mas arriba y a la izquierda.
//	if(startTbl)
//	{
//		for(u = 0, t = tbl; t; t = t->next)
//			if(t->nro == startTbl)
//			{
//				u = t;
//				break;
//			}
//		if(!u)
//			for(t = u = tbl; t; t = t->next)
//				if(t->x < u->x || t->y < u->y)
//					u = t;
//	}
//	else
//	{
//		for(t = u = tbl; t; t = t->next)
//			if(t->x < u->x || t->y < u->y)
//				u = t;
//	}
//
//	//   Anima la ventana,
//	wActivate(wMapH);
//	for(seguir = 1; seguir; )
//	{
//		DrawTable(u, 1);
//		ch = GetKey();
//		DrawTable(u, 0);
//		if(ch == keycanc) { seguir = 0; }
//		else if(ch == keydown)  {t = FindY(u, 1); if(t) u=t;}
//		else if(ch == keyupper) {t = FindY(u,-1); if(t) u=t;}
//		else if(ch == keyleft)  {t = FindX(u,-1); if(t) u=t;}
//		else if(ch == keyright) {t = FindX(u, 1); if(t) u=t;}
//		else if(ch == keyenter || ch == keyout) { seguir = 0;}
//	}
//	wDeactivate(wMapH, 0);
//	if(ch == keycanc) return 0;
//	return u->nro;
//}
//
////   Carga del archivo pertinente, la definicion del mapa.
//void LoadMapDef()
//{
//	tbl_ *t;
//	text_ *x;
//	line_ *l;
//	static int t_y, t_x, t_nro, t_sent, i, t_color;
//	char *dato;
//
//	if(OpenIni("mesas"))
//	{
//		WndX = 40;
//		WndY = 20;
//		tbl = 0;
//		return;
//	}
//
//	//   Datos generales de la ventana.
//	WndX =  (int) ExtractIniLong(G_WINDOW, "Ancho");
//	WndY =  (int) ExtractIniLong(G_WINDOW, "Alto");
//	WndXX = (int) ExtractIniLong(G_WINDOW, "Pos_x");
//	WndYY = (int) ExtractIniLong(G_WINDOW, "Pos_y");
//	szObj = (ExtractIniSiNo(G_WINDOW, "mesa_grande"))? 5: 3;
//
//	//   Datos de las mesas.
//	for(i = 1; ; i++)
//	{
//		sprintf(tmsg_, "Table%d_pos_y", i);
//		t_y = ExtractIniLong(G_TABLES, tmsg_);
//		sprintf(tmsg_, "Table%d_pos_x", i);
//		t_x = ExtractIniLong(G_TABLES, tmsg_);
//		sprintf(tmsg_, "Table%d_nro", i);
//		t_nro = ExtractIniLong(G_TABLES, tmsg_);
//		if(!t_nro) break;
//        if((t_y<0) || (t_y>(WndY-2)) || (t_x<0) || (t_x>(WndX-4)))
//			continue;
//
//		//   Incorpora la nueva mesa.
//        t = new tbl_(); // (tbl_ *) MALLOC(sizeof(tbl_), "LoadMapDef");
//		memset(t, 0, sizeof(tbl_));
//		t->y = t_y; 
//		t->x = t_x;
//		t->nro = t_nro;
//		t->next = tbl;
//		tbl = t;
//	}
//
//	//  Datos de los textos.
//	for( i = 1; ; i++)
//	{
//		sprintf(tmsg_, "Texto%d_pos_y", i);
//		t_y = ExtractIniLong(G_TEXTOS, tmsg_);
//		sprintf(tmsg_, "Texto%d_pos_x", i);
//		t_x = ExtractIniLong(G_TEXTOS, tmsg_);
//		sprintf(tmsg_, "Texto%d_valor", i);
//		dato = Trim(ExtractIniStr(G_TEXTOS, tmsg_));
//		sprintf(tmsg_, "Texto%d_color", i);
//		t_color = ExtractIniLong(G_TEXTOS, tmsg_);
//
//        if(!dato || *dato == 0)
//			break;
//		if((t_y<0) || (t_y>(WndY-2)) || (t_x<0) || (t_x>(WndX-4)))
//			continue;
//		
//        x = new text_(); // (text_ *) MALLOC(sizeof(text_), "LoadMapDef2");
//		memset(x, 0, sizeof(text_));
//		x->delFlag = 0;
//		x->x = t_x;
//		x->y = t_y;
//		x->texto = dato;
//		x->next = textos;
//		x->color = t_color? t_color: tblesq[3];
//		textos = x;
//	}
//
//	//  Datos de las lineas.
//	for( i = 1; ; i++)
//	{
//		sprintf(tmsg_, "Linea%d_pos_y", i);
//		t_y = ExtractIniLong(G_LINEAS, tmsg_);
//		sprintf(tmsg_, "Linea%d_pos_x", i);
//		t_x = ExtractIniLong(G_LINEAS, tmsg_);
//		sprintf(tmsg_, "Linea%d_mueve_h", i);
//		t_sent = ExtractIniSiNo(G_LINEAS, tmsg_);
//		sprintf(tmsg_, "Linea%d_dimen", i);
//		t_nro = ExtractIniLong(G_LINEAS, tmsg_);
//		sprintf(tmsg_, "Linea%d_color", i);
//		t_color = ExtractIniLong(G_LINEAS, tmsg_);
//
//        if(!t_nro) break;
//		if((t_y < 0) || (t_y > WndY) || (t_x < 0) || (t_x > WndX))
//			continue;
//		
//        l = new line_(); // (line_ *) MALLOC(sizeof(line_), "LoadMapDef3");
//		memset(l, 0, sizeof(line_));
//		l->delFlag = 0;
//		l->x = t_x;
//		l->y = t_y;
//		l->cantidad = t_nro;
//		l->fg_hori = (char) t_sent;
//		l->next = lineas;
//		l->color = t_color? t_color: tblesq[3];
//		lineas = l;
//	}
//
//	//  Cierra el .ini y valida los datos cargados.CloseIni();
//	CloseIni();
//	if(!tbl || WndX == 0 || WndY == 0)
//		AbnormalEnd("Error en pos\\mesas.ini!!!");
//}
//
////   Escribe la nueva definición de archivo.
//void WriteMapDef()
//{
//	FILE *oh;
//	tbl_ *t;
//	text_ *x;
//	line_ *l;
//	int i;
//
//	oh = fopen("pos\\mesas.ini", "w");
//	if(!oh)
//		AbnormalEnd("No puedo crear a 'pos\\mesas.ini' para grabar los cambios !!!");
//
//	fprintf(oh, ";\n; Mesas.Ini\n; ATENCION: Este archivo es manejado directamente por DEFTBL.\n");
//	fprintf(oh, ";           NO TOQUE NADA MANUALMENTE !!!\n;\n\n");
//
//	//   Escribe los parametros generales de la ventana.
//	fprintf(oh, "[%s]\n", G_WINDOW);
//	fprintf(oh, "\tAncho = %d\n\tAlto = %d\n", WndX, WndY);
//	fprintf(oh, "\tPos_x = %d\n\tPos_y = %d\n", WndXX, WndYY);
//	fprintf(oh, "\tMesa_grande = %s\n\n", (szObj==3)? "no": "si");
//
//	//   Escribe la tabla de mesas definidas.
//	fprintf(oh, "[%s]\n", G_TABLES);
//	for(i = 1, t = tbl; t; t = t->next)
//		if(t->nro)
//		{
//			fprintf(oh, "\tTable%d_pos_y = %d\n", i, t->y);
//			fprintf(oh, "\tTable%d_pos_x = %d\n", i, t->x);
//			fprintf(oh, "\tTable%d_nro = %d\n\n", i, t->nro);
//			i++;
//		}
//
//	//   Escribe la tabla de textos definidos.
//	fprintf(oh, "[%s]\n", G_TEXTOS);
//	for(i = 1, x = textos; x; x = x->next)
//		if(!x->delFlag)
//		{
//			fprintf(oh, "\tTexto%d_pos_y = %d\n", i, x->y);
//			fprintf(oh, "\tTexto%d_pos_x = %d\n", i, x->x);
//			fprintf(oh, "\tTexto%d_color = %d\n", i, (int) x->color);
//			fprintf(oh, "\tTexto%d_valor = %s\n\n", i, x->texto);
//			i++;
//		}
//
//	//   Escribe la tabla de textos definidos.
//	fprintf(oh, "[%s]\n", G_LINEAS);
//	for(i = 1, l = lineas; l; l = l->next)
//		if(!l->delFlag)
//		{
//			fprintf(oh, "\tLinea%d_pos_y = %d\n", i, l->y);
//			fprintf(oh, "\tLinea%d_pos_x = %d\n", i, l->x);
//			fprintf(oh, "\tLinea%d_mueve_h = %s\n", i, 
//				        l->fg_hori? "si": "no");
//			fprintf(oh, "\tLinea%d_color = %d\n", i, (int) l->color);
//			fprintf(oh, "\tLinea%d_dimen = %d\n\n", i, l->cantidad);
//			i++;
//		}
//
//	fclose(oh);
//}
//
////   Dibuja una linea sencilla en la posicion que le indica.
//void DrawLine(line_ *l)
//{
//	int i, aY, aX, addV;
//
//	wActivate(wMapH);
//	wSetColor(l->color);
//	aY = l->y; aX = l->x;
//	wLocate(aY, aX);
//	i = l->cantidad;
//	addV = (l->cantidad < 0)? -1: 1;
//	do {
//			wPch(l->fg_hori? '\xc4': '\xb3');
//			if(l->fg_hori) aX += addV; else aY += addV;
//			wLocate(aY, aX);
//			i -= addV;
//	} while(i);
//}
//
////   Dibuja un texto en la posicion que el indica.
//void DrawTexto(text_ *x)
//{
//	wActivate(wMapH);
//	wSetColor(x->color);
//	wLocate(x->y, x->x);
//	wPstr(x->texto);
//}
//
////   Dibuja una table en la posición que se indica.
//void DrawTable(tbl_ *t, int bold)
//{
//	char msg[5];
//	rect r;
//
//	sprintf(msg, "%3d", t->nro);
//	if(szObj == 5)
//	{
//		if(bold)
//			wEsqSetColor(6);
//		else
//			wEsqSetColor(5 + (t->flag? 2: 0));
//		wLocate(t->y, t->x);
//		wPstr("     ");
//	
//		wLocate(t->y + 1, t->x);
//		wPch(' '); wPstr(msg); wPch(' ');
//	
//		wLocate(t->y + 2, t->x);
//		wPstr("     ");
//		if(bold)
//		{
//			r.sx = t->x;  r.sy = t->y;
//			r.ex = t->x + 4;  r.ey = t->y + 2;
//			wDrawBox(&r, BORDERSINGLE);
//		}
//	}
//	else
//	{
//		wLocate(t->y, t->x);
//		if(bold)
//			wEsqSetColor(6);
//		else
//			wEsqSetColor(5 + (t->flag? 2: 0));
//		wPstr(msg);
//	}
//}
//
////   Imprime (o refresca) la visualizacion del mapa.
//void DrawMapWindow()
//{
//	tbl_ *t;
//	text_ *x;
//	line_ *l;
//
//	//   Define la ventana, si todavia no se definio.
//	if(!wMapH)
//		wMapH = wDefine(WndYY, WndXX, 2+WndY+WndYY, 2+WndX+WndXX,
//		                BORDERSINGLE, SNGCURS, tblesq, "Mapa",
//						"Pulse <ESC>");
//
//	//   Dibuja la ventana principal.
//	wActivate(wMapH);
//    wCls('±', MARRON * 16);
//
//	//   Dibuja las lineas sencillas
//	for(l = lineas; l; l = l->next)
//		if(!l->delFlag)
//			DrawLine(l);
//
//	//   Dibuja los textos pre-definidos.
//	for(x = textos; x; x = x->next)
//		if(!x->delFlag)
//			DrawTexto(x);
//
//	//   Dibuja las mesas pre-ubicadas.
//	for(t = tbl; t; t = t->next)
//		if(t->nro)
//			DrawTable(t, 0);
//}
//
