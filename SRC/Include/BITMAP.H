/*   Bitmap.h  - Especificaciones para el modulo bitmap.c   */

#ifndef INCLUDED_BITMAP_H
#define INCLUDED_BITMAP_H

//  Estructura para guardar la informacion del bitmap.
class BitMap 
{
	unsigned cBits;
	unsigned FoundBit;
    char *p, *lf;

public:
	BitMap(unsigned cBits);
	~BitMap();

	void SetBit(unsigned bit);
	bool TestBit(unsigned bit);
	void ResetBit(unsigned bit);
	void SetALLBits(void);
	void ResetALLBits(void);

	unsigned FirstBit(void);
	unsigned NextBit(void);
};

#endif
