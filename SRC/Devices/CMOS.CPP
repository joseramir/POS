/*
 *   Cmos.c
 *   Permite la funcionalidad de una RAM Cmos.
 *   Por cuestiones de perfomance, cuando arranca el sistema se leen los
 *   datos de la CMOS-RAM en variables comunes. Cuando estas se actualizan,
 *   solamente se escriben los nuevos datos en la CMOS-RAM.
*/

#include <stdafx.h>

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

#define STARTCMOS ((char _far *) 0xE8800000l)
char CmosFile[] = "cmos.dat";
char *msg1 = new char[200];
char *msg2 = new char[200];

struct cmos_ cmos[] =
{
	{ &ticket,          1l,  sizeof(long),		sizeof(ticket)			},	// 0
	{ &zeta,            6l,  sizeof(long),		sizeof(zeta)			},
	{ &netos[0],       11l,  sizeof(double),	sizeof(netos[0])		},
	{ &netos[1],       21l,  sizeof(double),	sizeof(netos[1])		},
	{ &netos[2],       31l,  sizeof(double),	sizeof(netos[2])		},	// 4
	{ &netos[3],       41l,  sizeof(double),	sizeof(netos[3])		},
	{ &iva[0],         51l,  sizeof(double),	sizeof(iva[0])			},
	{ &iva[1],         61l,  sizeof(double),	sizeof(iva[1])			},
	{ &iva[2],         71l,  sizeof(double),	sizeof(iva[2])			},	// 8
	{ &iva[3],         81l,  sizeof(double),	sizeof(iva[3])			},
	{ &tVta,           91l,  sizeof(double),	sizeof(tVta)			},
	{ &cAnula,        101l,  sizeof(long),		sizeof(cAnula)			},
	{ &tAnula,        106l,  sizeof(double),	sizeof(tAnula)			},	// 12
	{ &sobret,        116l,  sizeof(double),	sizeof(sobret)			},
	{ &ckaNro,        126l,  sizeof(int),		sizeof(ckaNro)			},
	{ &GlobalGetPrec, 130l,  sizeof(int),		sizeof(GlobalGetPrec)   },
	{ &tCobro,		  134l,  sizeof(double),	sizeof(tCobro)			},	// 16
	{ &monitorMode,   144l,  sizeof(int),		sizeof(monitorMode)		},
	{ &tVtaReal,	  150l,  sizeof(double),	sizeof(tVtaReal)		},
	{ &promosCalc,	  160l,  sizeof(bool),		sizeof(promosCalc)		},
	{ &ultCajero,     164l,  sizeof(int),		sizeof(ultCajero)		},	// 20
	{ &supervNro,     168l,  sizeof(int),		sizeof(supervNro)		},
	{ &promosHechas,  172l,  sizeof(bool),      sizeof(promosHechas)    },
	{ &forceReloadPromos,  176l,  sizeof(bool),      sizeof(forceReloadPromos)    },
	{ &reloadPremioInst,  180l,  sizeof(bool),      sizeof(reloadPremioInst)    },	
	{ &numsecuencia,  184l,  sizeof(long),      sizeof(numsecuencia)    },
	{ &variacionpp,	  189l,  sizeof(bool),		sizeof(variacionpp)		},
	{ &applyvarpp,	  193l,  sizeof(bool),		sizeof(applyvarpp)		},
	{ &dummyHabil,	197l,  sizeof(bool),	sizeof(dummyHabil)		},
	{ &NroRetiro,     201l,  sizeof(int),		sizeof(NroRetiro)		},

	{ &bSaleRepElec,	205l,  sizeof(bool),	sizeof(bSaleRepElec)	},
	{ &DesdeRepElec,    209l,  sizeof(int),		sizeof(DesdeRepElec)	},
	{ &HastaRepElec,    213l,  sizeof(int),		sizeof(HastaRepElec)	},
};

//Para recuperar los reportes electronicos luego de ZETA Fiscal
extern bool bSaleRepElec;
extern int DesdeRepElec;
extern int HastaRepElec;

#define BYTES_IN_CMOS 250		/* REVISAR AL AGREGAR VARIABLES !!! */
int cVarCmos = sizeof(cmos) / sizeof(struct cmos_);

int InitCmos()
{
	int h, i;
	struct cmos_ *c;
	char *tcmos;

	// Valida la estructura del CMOS, para dar un error fatal si algo no coincide
	int nxtOffset = 1;
	for(int i=0; i<cVarCmos; i++)
	{
		c = &cmos[i];
		if(c->alloc < nxtOffset) 
		{
			sprintf(tmsg, "Error interno en CMOS - Var %d", i);
			AbnormalEnd(tmsg);
		}
		if(c->len != c->real_len)
		{
			sprintf(tmsg, "Error interno en CMOS - No coinciden los largos en Var %d", i);
			AbnormalEnd(tmsg);
		}

		nxtOffset = c->alloc + c->len;
	}

    //h = open(CmosFile, O_RDWR | O_BINARY);
	std::fstream ifs(CmosFile, std::ios::in | std::ios::binary);
    //if(h == -1)
	if (!ifs)
	{
		//close(creat(CmosFile, S_IREAD | S_IWRITE));
		std::ofstream ofs(CmosFile, std::ios::out |std::ios::binary);		
		//h = open(CmosFile, O_RDWR);
        tcmos = (char *) MALLOC(BYTES_IN_CMOS, "InitCmos1");
        if(tcmos)
		{
			memset(tcmos, 0, BYTES_IN_CMOS);						
			//write(h, tcmos, BYTES_IN_CMOS);
			
			ofs.write(tcmos, BYTES_IN_CMOS);
			ofs.flush();
            FREE(tcmos);
			ofs.close();
		}
		//lseek(h, 0l, SEEK_SET);
    }	
	//close(h);
	if (!ifs)
		ifs.open(CmosFile, std::ios::in | std::ios::binary);
	for(c = cmos, i = 0; i < cVarCmos; i++, c++)
	{
		ifs.seekg(c->alloc);
		//lseek(h, c->alloc, SEEK_SET);
		ifs.read( reinterpret_cast<char*>( c->vari ), c->len);
			//memset(c->vari, 0, c->len);
		/*if(c->len != read(h, c->vari, c->len))
			memset(c->vari, 0, c->len);*/
	}
	ifs.close();
	//close(h);

	// Inicializa otras variables de control
	if(inOper <= 1) listap = defListaPrec;
	if(listap < 1 || listap > 5) listap = 1;

    return 1;
}

//   Escribe una variable de cmos en la misma.

void WriteCmos(int cual)
{
	int h;
	register struct cmos_ *c;
	std::ofstream ofs(CmosFile, std::ios::out | std::ios::in | std::ios::binary);

	//if(-1 == (h = open(CmosFile, O_RDWR)))

	//	AbnormalEnd("ReAbriendo CMOS file");
	c = &cmos[cual];
	ofs.seekp(c->alloc);
	//lseek(h, c->alloc, SEEK_SET);
	ofs.write( reinterpret_cast<const char*>( c->vari ), c->len );
	//write(h, c->vari, c->len);
	ofs.close();
	//close(h);
}

