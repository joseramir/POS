/*
 *   BitMap.c
 *   Maneja arrays de bits para marcar distintos datos.
*/

#include <stdafx.h>

// Mascaras para operar bit a bit
static unsigned directMask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
static unsigned invertedMask[] = { 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f };

//   Crea un bitmap. 
//   Los bits comienzan todos inicializados a cero.
BitMap::BitMap(unsigned cBits)
{
	int cBytes = (cBits / 8) + 1;
	this->cBits = cBits;
	this->p = (char *)MALLOC(cBytes, "MakeBitMap");
	this->ResetALLBits();
}

//   Elimina un bitmap que ya no se usara.
BitMap::~BitMap()
{
	FREE(this->p);
}

//   Setea a 1 todos los bits del bitmap que se indica.
void BitMap::SetALLBits()
{
    int cBytes = (this->cBits / 8) + 1;
	memset(this->p, 0xff, cBytes);
}

//   Setea a 0 todos los bits del bitmap que se indica.
void BitMap::ResetALLBits()
{
    int cBytes = (this->cBits / 8) + 1;
	memset(this->p, 0x00, cBytes);
}

// Pone a 1 el bit que se indica, del bitmap que se indica.
void BitMap::SetBit(unsigned bit)
{
    if(bit < 1 || bit > this->cBits) return;
	bit--;
	this->p[bit / 8] |= directMask[bit % 8];
}

// Pone a 0 el bit que se indica, del bitmap que se indica.
void BitMap::ResetBit(unsigned bit)
{
    if(bit < 1 || bit > this->cBits) return;
	bit--;
	this->p[bit / 8] &= invertedMask[bit % 8];
}

//  Devuelve el valor de un bit que se indica, en el bitmap que se indica
bool BitMap::TestBit(unsigned bit)
{
    if(bit < 1 || bit > this->cBits) return 0;
	bit--;
	return ((this->p[bit / 8] & directMask[bit % 8]) != 0);

}

//  Busca el primer bit a 1 del bitmap que se indica. Si no encuentra
//  ninguno, vuelve con 0. (los bits van desde 1 a ...)
unsigned BitMap::FirstBit()
{
    this->lf = this->p;
	this->FoundBit = 0;
	return NextBit();
}

//  Busca el siguiente bit a 1, a partir desde el ultimo encontrado.
//  Si no encuentra ninguno, vuelve con cero.
unsigned BitMap::NextBit()
{
	char *myp;
	unsigned FndBit;

	// A pesar que externamente nos manejemos con el numero de bits comenzando en uno, para esta funcion es mas comodo
	// comenzar en zero. Por eso, iniciamos directamente con this->FoundBit
	int actualByte = this->p[this->FoundBit / 8];
	for(FndBit = this->FoundBit % 8; FndBit < 8; FndBit++)
	{
		if((actualByte & directMask[FndBit]) != 0)
		{
			this->FoundBit = ((this->FoundBit / 8) * 8) + FndBit + 1;
			return this->FoundBit;
		}
	}

	// Se acabo el 'byte' actual. Busca el primer siguiente byte donde las cosas no esten en cero.
	int pendBytes = (this->cBits / 8) - (this->p - this->lf);
	for(myp = this->lf ; pendBytes > 0; pendBytes--)
		if(*myp != 0)
			break;
	if(pendBytes <= 0) return 0;

	// Encontro un byte que contiene algun bit en uno. Identifica el bit y lo retorna.
	actualByte = *myp;
	for(FndBit = 0; FndBit < 8; FndBit++)
	{
		if((actualByte & directMask[FndBit]) != 0)
		{
			this->FoundBit = ((myp - p) * 8) + FndBit;
			return this->FoundBit;
		}
	}

	return 0;
}
