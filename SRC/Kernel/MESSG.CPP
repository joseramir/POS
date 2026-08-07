//
//   Messg.c
//   Carga la tabla de mensajes del usuario.
//

#include <stdafx.h>
char logo[] = "logo", salute[] = "saludos";     // Secciones en 'mensajes.ini'
char premio[] = "millas", enva[] = "envases";
char varias[] = "varias", susp_[] = "suspende";
char dmp[] = "dctomp";

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Variables publicas a todos los modulos (declaradas en pos.h).
char **LogoLines = (char **)new char*[13];
char **LogoPie = (char **)new char*[13];
char **LogoEpson = (char **)new char*[17];
char **saludos = (char **)new char*[3];
char **closed = (char **)new char*[2];

extern char **PremLogoLine, **PremPieLine, *PremLine, *PremLineMed;
extern char **EnvasLogo, **EnvasPie;
extern char *vFuncName;
extern char *suspMsg1, *suspMsg2;

//extern int  dmp_Depto, dmp_CntHeader, dmp_CntTrailer;
//extern char *dmp_Header[5], *dmp_Trailer[5];

//   Toma una propiedad del archivo ini, reserv ndole lugar en memoria y retornando un puntero.
char *ExtractIniChar(char *, char *);
char *GetPropLine(char *secc, char *name, int maxLen)
{
	 char *r, comi, *t, *ro;
	 int l;

	 t = "";

     r = ExtractIniChar(secc, name);
     if(r)
	 {
		while(isspace(*r)) r++;
		comi = 0;
		if(*r == '"' || *r == '\'') comi = *r++;
        for(l = 0, ro = r; *r && *r != comi; r++) l++;
		*r = 0;
		if(l > maxLen) l = maxLen;
		if(l)
		{
            t = (char *) MALLOC(l + 1, "GetPropLine");
			memmove(t, ro, l + 1);
			t[l] = 0;
		}
	 }
	 return t;
}

char *GetPropLine(char *secc, char *name)
{
	return GetPropLine(secc, name, 40);
}

//
//   La siguiente funcion carga la tabla de mensajes en la pantalla.
//
int LoadMessages()
{
	 char name[40], *r;
	 int i;

	 if(!OpenIni(F_Cabecera))
	 {
		// Carga lineas del logo.
		cLinesLogo = (int) ExtractIniLong(logo, "CantLineasArriba");
		if(cLinesLogo > 11) cLinesLogo = 11;
		for(i = 1; i <= cLinesLogo; i++)
		{
			sprintf(name, "Linea%d", i);
			r = GetPropLine(logo, name);
			//Supone que nombre de la sucursal esta en la primer linea
			if (i == 1)
				VoucherVar("@NombreSucursal@", Trim(r));
            if(r && (*r))
				LogoLines[i - 1] = r;
			else
				LogoLines[i - 1] = STRDUP("", "LoadMessagesDUP");
		}

		// Carga lineas del pie.
		cLinesPie = (int) ExtractIniLong(logo, "CantLineasPie");
		if(cLinesPie > 8) cLinesPie = 8;
		for(i = 1; i <= cLinesPie; i++)
		{
			sprintf(name, "Pie%d", i);
			r = GetPropLine(logo, name);
            if(r)
				LogoPie[i - 1] = r;
		}

		cLinesEpson = (int)ExtractIniLong(logo, "CantHeaderTicketFactura");
		if(cLinesEpson > 15) cLinesEpson = 15;
		for(i = 1; i <= cLinesEpson; i++)
		{
			sprintf(name, "TicketFacturaLinea%d", 49 + i);
			r = GetPropLine(logo, name);
			if(r)
				LogoEpson[i - 1] = r;
		}
		CloseIni();
	 }

	 if(!OpenIni(F_Mensajes))
	 {
		// Carga las lineas que se imprimiran al suspender ticket.
           r = GetPropLine(susp_, "suspen1");
		if(r) suspMsg1 = r;
           r = GetPropLine(susp_, "suspen2");
		if(r) suspMsg2 = r;

		// Carga las lineas de saludo.
        r = GetPropLine(salute, "visor1");
        if(r) saludos[0] = r;
        r = GetPropLine(salute, "visor2");
        if(r) saludos[1] = r;
        r = GetPropLine(salute, "AlCajero");
        if(r) saludos[2] = r;
		r = GetPropLine(salute, "cerrado1");
		if(r) closed[0] = r;
		r = GetPropLine(salute, "cerrado2");
		if(r) closed[1] = r;

		// Carga descripciones de funciones.
        r = GetPropLine(varias, "VenFunc");
        if(r) vFuncName = r;

		//  Carga las lineas de descripcion de mensajes p/premios.
		for(PremiosLogo = 0; PremiosLogo<5; PremiosLogo++)
		{
			sprintf(name, "Linea%d", PremiosLogo + 1);
            PremLogoLine[PremiosLogo] = GetPropLine(premio, name);
            if(!strlen(PremLogoLine[PremiosLogo]))
				break;
		}
		for(PremiosPie = 0; PremiosPie<5; PremiosPie++)
		{
			sprintf(name, "Pie%d", PremiosPie + 1);
            PremPieLine[PremiosPie] = GetPropLine(premio, name);
            if(!strlen(PremPieLine[PremiosPie]))
				break;
		}

		PremLine = GetPropLine(premio, "renglon");
		PremLineMed = GetPropLine(premio, "lineam");

		//  Carga las lineas de descripcion para la vta de envases
		for(EnvasLogoCnt = 0; EnvasLogoCnt < 5; EnvasLogoCnt++)
		{
			sprintf(name, "Encab%d", EnvasLogoCnt + 1);
            EnvasLogo[EnvasLogoCnt] = GetPropLine(enva, name);
            if(!strlen(EnvasLogo[EnvasLogoCnt]))
				break;
		}
		for(EnvasPieCnt = 0; EnvasPieCnt < 5; EnvasPieCnt++)
		{
			sprintf(name, "Pie%d", EnvasPieCnt + 1);
            EnvasPie[EnvasPieCnt] = GetPropLine(enva, name);
            if(!strlen(EnvasPie[EnvasPieCnt]))
				break;
		}

		//  Carga las descripciones para promociones por medio de pago.
		dmp_Depto = (int) ExtractIniLong(dmp, "plu");
		dmp_Voucher = (int) ExtractIniLong(dmp, "voucher");
		if(dmp_Voucher != 0)
			if(!LoadVoucher(dmp_Voucher))
				dmp_Voucher = 0;

		//  Finaliza la carga de mensajes
		CloseIni();
	 }
	 return 0;
}
