#include <stdafx.h>

#ifndef VARCMP_EQ

   // Codigos de resultado
   #define NOERROR				0
   #define VARCMP_EQ			0x4001
   #define VARCMP_LT			0x4002
   #define VARCMP_GT			0x4003

#endif

// Optimiza el codigo
#ifdef POSXP
#pragma managed(push, off)
#endif

// Numeros decimales por default.
int DefaultDecimals = 4;

// Helpers
BDecimal *BDecimal::TempDec()
{
	BDecimal *result = new BDecimal();
	Temp::SetTemporary(result);
	return result;
}

BDecimal *BDecimal::TempDec(int num)
{
	BDecimal *result = new BDecimal(num);
	Temp::SetTemporary(result);
	return result;
}

BDecimal *BDecimal::TempDec(double num)
{
	BDecimal *result = new BDecimal(num);
	Temp::SetTemporary(result);
	return result;
}

// Constructores
BDecimal::BDecimal()
{
	DECIMAL_SIGNSCALE(dec) = 0;
	DECIMAL_LO32(dec) = 0;
	DECIMAL_MID32(dec) = 0;
	DECIMAL_HI32(dec) = 0;
}

BDecimal::BDecimal(int num)
{
	if(num >= 0)
		DECIMAL_SIGNSCALE(dec) = 0;
	else
	{
		DECIMAL_SIGNSCALE(dec) = DECIMAL_NEG;
		num *= -1;
	}

	DECIMAL_LO32(dec) = num;
	DECIMAL_MID32(dec) = 0;
	DECIMAL_HI32(dec) = 0;
}

BDecimal::BDecimal(double d)
{
	DecFromR8(d, &this->dec);
	if(DefaultDecimals > 0) ApplyRound(DefaultDecimals);
}

BDecimal::BDecimal(double d, int decimals)
{
	DecFromR8(d, &this->dec);
	ApplyRound(decimals);
}

///////////////////////////////////////////////////////////////////////////////////////
// Asignaciones
BDecimal &BDecimal::operator=(BDecimal &d)
{
	this->dec = d.dec;
	return *this;
}

BDecimal &BDecimal::operator=(int num)
{
	if(num >= 0)
		DECIMAL_SIGNSCALE(dec) = 0;
	else
	{
		DECIMAL_SIGNSCALE(dec) = DECIMAL_NEG;
		num *= -1;
	}

	DECIMAL_LO32(dec) = num;
	DECIMAL_MID32(dec) = 0;
	DECIMAL_HI32(dec) = 0;
	return *this;
}

BDecimal &BDecimal::operator=(double num)
{
	DecFromR8(num, &this->dec);
	if(DefaultDecimals > 0) ApplyRound(DefaultDecimals);
	return *this;
}


///////////////////////////////////////////////////////////////////////////////////////
// Suma
BDecimal &BDecimal::operator+(BDecimal &d)
{
	BDecimal *res = TempDec();
	DecAddSub(&this->dec, &d.dec, &res->dec, 0);
	return *res;
}

BDecimal &BDecimal::operator+(int num)
{
	BDecimal *res = TempDec(num);
	DecAddSub(&this->dec, &res->dec, &res->dec, 0);
	return *res;
}

BDecimal &BDecimal::operator+(double num)
{
	BDecimal *res = TempDec(num);
	DecAddSub(&this->dec, &res->dec, &res->dec, 0);
	return *res;
}

BDecimal &BDecimal::operator+=(BDecimal &d)
{
	DecAddSub(&this->dec, &d.dec, &this->dec, 0);
	return *this;
}

BDecimal &BDecimal::operator+=(int num)
{
	BDecimal *temp = new BDecimal(num);
	DecAddSub(&this->dec, &temp->dec, &this->dec, 0);
	delete temp;
	return *this;
}

BDecimal &BDecimal::operator+=(double num)
{
	BDecimal *temp = new BDecimal(num);
	DecAddSub(&this->dec, &temp->dec, &this->dec, 0);
	delete temp;
	return *this;
}



///////////////////////////////////////////////////////////////////////////////////////
// Suma
BDecimal &BDecimal::operator-(BDecimal &d)
{
	BDecimal *res = TempDec();
	DecAddSub(&this->dec, &d.dec, &res->dec, DECIMAL_NEG);
	return *res;
}

BDecimal &BDecimal::operator-(int num)
{
	BDecimal *res = TempDec(num);
	DecAddSub(&this->dec, &res->dec, &res->dec, DECIMAL_NEG);
	return *res;
}

BDecimal &BDecimal::operator-(double num)
{
	BDecimal *res = TempDec(num);
	DecAddSub(&this->dec, &res->dec, &res->dec, DECIMAL_NEG);
	return *res;
}

BDecimal &BDecimal::operator-=(BDecimal &d)
{
	DecAddSub(&this->dec, &d.dec, &this->dec, DECIMAL_NEG);
	return *this;
}

BDecimal &BDecimal::operator-=(int num)
{
	BDecimal *temp = new BDecimal(num);
	DecAddSub(&this->dec, &temp->dec, &this->dec, DECIMAL_NEG);
	delete temp;
	return *this;
}

BDecimal &BDecimal::operator-=(double num)
{
	BDecimal *temp = new BDecimal(num);
	DecAddSub(&this->dec, &temp->dec, &this->dec, DECIMAL_NEG);
	delete temp;
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////
// Multiplicacion
BDecimal &BDecimal::operator*(BDecimal &d)
{
	BDecimal *res = TempDec();
	Multiply(&this->dec, &d.dec, &res->dec);
	return *res;
}

BDecimal &BDecimal::operator*(int num)
{
	BDecimal *res = TempDec(num);
	Multiply(&this->dec, &res->dec, &res->dec);
	return *res;
}

BDecimal &BDecimal::operator*(double num)
{
	BDecimal *res = TempDec(num);
	Multiply(&this->dec, &res->dec, &res->dec);
	return *res;
}

BDecimal &BDecimal::operator*=(BDecimal &d)
{
	Multiply(&this->dec, &d.dec, &this->dec);
	return *this;
}

BDecimal &BDecimal::operator*=(int num)
{
	BDecimal *temp = new BDecimal(num);
	Multiply(&this->dec, &temp->dec, &this->dec);
	delete temp;
	return *this;
}

BDecimal &BDecimal::operator*=(double num)
{
	BDecimal *temp = new BDecimal(num);
	Multiply(&this->dec, &temp->dec, &this->dec);
	delete temp;
	return *this;
}



///////////////////////////////////////////////////////////////////////////////////////
// Division
BDecimal &BDecimal::operator/(BDecimal &d)
{
	BDecimal *res = TempDec();
	Divide(&this->dec, &d.dec, &res->dec);
	if(DefaultDecimals > 0) res->ApplyRound(DefaultDecimals);
	return *res;
}

BDecimal &BDecimal::operator/(int num)
{
	BDecimal *res = TempDec(num);
	Divide(&this->dec, &res->dec, &res->dec);
	if(DefaultDecimals > 0) res->ApplyRound(DefaultDecimals);
	return *res;
}

BDecimal &BDecimal::operator/(double num)
{
	BDecimal *res = TempDec(num);
	Divide(&this->dec, &res->dec, &res->dec);
	if(DefaultDecimals > 0) res->ApplyRound(DefaultDecimals);
	return *res;
}

BDecimal &BDecimal::operator/=(BDecimal &d)
{
	Divide(&this->dec, &d.dec, &this->dec);
	if(DefaultDecimals > 0) ApplyRound(DefaultDecimals);
	return *this;
}

BDecimal &BDecimal::operator/=(int num)
{
	BDecimal *temp = new BDecimal(num);
	Divide(&this->dec, &temp->dec, &this->dec);
	delete temp;
	if(DefaultDecimals > 0) ApplyRound(DefaultDecimals);
	return *this;
}

BDecimal &BDecimal::operator/=(double num)
{
	BDecimal *temp = new BDecimal(num);
	Divide(&this->dec, &temp->dec, &this->dec);
	delete temp;
	if(DefaultDecimals > 0) ApplyRound(DefaultDecimals);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////
// Conversion a cadena
char *BDecimal::ToString()
{
	char *ptr;
	DecToString(&this->dec, 0, &ptr);
	return Temp::StrDup(ptr);
}

char *BDecimal::ToString(int decimals)
{
	char *ptr, tBuff[35];
	int ndec = 0;
	BDecimal temp = BDecimal(0);

	// Redondear antes!!!
	NativeRound(&this->dec, decimals, &temp.dec);
	DecToString(&temp.dec, 0, &ptr);
	STRCPY(tBuff, ptr);
	ptr = tBuff;
	while(*ptr && (*ptr != '.')) ptr++;
	if(*ptr == '.')
		ndec = strlen(++ptr);
	else
	{
		*ptr++ = '.';
		*ptr = 0;
	}
	if(decimals == 0)
		*(--ptr) = 0;
	else
	{
		while(ndec < decimals)
		{
			STRCAT(ptr, "0");
			ndec++;
		}
		if(ndec > decimals)
			*(ptr + decimals) = 0;
	}

	return Temp::StrDup(tBuff);	
}

///////////////////////////////////////////////////////////////////////////////////////
// Otras conversiones.
int BDecimal::ToInt32()
{
	int result;
	VarI4FromDec(&this->dec, &result);
	return result;
}

double BDecimal::ToDouble()
{
	double result;
	VarR8FromDec(&this->dec, &result);
	return result;
}

BDecimal::operator int()
{
	int result;
	VarI4FromDec(&this->dec, &result);
	return result;
}

BDecimal *BDecimal::Parse(char *num)
{
	DECIMAL diez, tempAdd;
	DECIMAL_SIGNSCALE(diez) = 0;
	DECIMAL_LO32(diez) = 10;
	DECIMAL_MID32(diez) = 0;
	DECIMAL_HI32(diez) = 0;
	DECIMAL_SIGNSCALE(tempAdd) = 0;
	DECIMAL_LO32(tempAdd) = 0;
	DECIMAL_MID32(tempAdd) = 0;
	DECIMAL_HI32(tempAdd) = 0;

	BDecimal *result = new BDecimal();
	int divisor = 1;
	bool negativo = false, enDecimal = false;

	// Saltea espacios en blanco al principio
	while(*num)
	{
		if(!isspace(*num))
			break;
		else
			num++;
	}
	// Procesa negativos
	if(*num == '-') { negativo = true; num++; }

	// Procesa cada uno de los digitos
	while(*num)
	{
		if(enDecimal) divisor *= 10;
		if(*num >= '0' && *num <= '9')
		{
			Multiply(&result->dec, &diez, &result->dec);
			DECIMAL_LO32(tempAdd) = (*num - '0');
			DecAddSub(&result->dec, &tempAdd, &result->dec, 0);
		}
		else if(*num == '.')
		{
			if(enDecimal) break;
			enDecimal = true;
		}
		else break;
		num++;
	}

	if(divisor != 1)
	{
		DECIMAL_LO32(tempAdd) = divisor;
		Divide(&result->dec, &tempAdd, &result->dec);
	}

	if(negativo)
		result->dec.u.u.sign = DECIMAL_NEG;

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////
// Comparaciones
bool BDecimal::ICompare(BDecimal &num, int expectedResult, bool negated)
{
	int result = DecCompare(&this->dec, &num.dec);
	bool bRes = (result == expectedResult);
	if(negated) return !bRes;
	return bRes;
}

bool BDecimal::ICompare(int num, int expectedResult, bool negated)
{
	BDecimal *temp = new BDecimal(num);
	int result = DecCompare(&this->dec, &temp->dec);
	delete temp;
	bool bRes = (result == expectedResult);
	if(negated) return !bRes;
	return bRes;
}

bool BDecimal::ICompare(double num, int expectedResult, bool negated)
{
	BDecimal *temp = new BDecimal(num);
	int result = DecCompare(&this->dec, &temp->dec);
	delete temp;
	bool bRes = (result == expectedResult);
	if(negated) return !bRes;
	return bRes;
}

bool BDecimal::operator==(BDecimal &num)	{ return ICompare(num, VARCMP_EQ, false); }
bool BDecimal::operator==(int num)			{ return ICompare(num, VARCMP_EQ, false); }
bool BDecimal::operator==(double num)		{ return ICompare(num, VARCMP_EQ, false); }
bool BDecimal::operator<=(BDecimal &num)	{ return ICompare(num, VARCMP_GT, true); }
bool BDecimal::operator<=(int num)			{ return ICompare(num, VARCMP_GT, true); }
bool BDecimal::operator<=(double num)		{ return ICompare(num, VARCMP_GT, true); }
bool BDecimal::operator<(BDecimal &num)		{ return ICompare(num, VARCMP_LT, false); }
bool BDecimal::operator<(int num)			{ return ICompare(num, VARCMP_LT, false); }
bool BDecimal::operator<(double num)		{ return ICompare(num, VARCMP_LT, false); }
bool BDecimal::operator>=(BDecimal &num)	{ return ICompare(num, VARCMP_LT, true); }
bool BDecimal::operator>=(int num)			{ return ICompare(num, VARCMP_LT, true); }
bool BDecimal::operator>=(double num)		{ return ICompare(num, VARCMP_LT, true); }
bool BDecimal::operator>(BDecimal &num)		{ return ICompare(num, VARCMP_GT, false); }
bool BDecimal::operator>(int num)			{ return ICompare(num, VARCMP_GT, false); }
bool BDecimal::operator>(double num)		{ return ICompare(num, VARCMP_GT, false); }
bool BDecimal::operator!=(BDecimal &num)	{ return ICompare(num, VARCMP_EQ, true); }
bool BDecimal::operator!=(int num)			{ return ICompare(num, VARCMP_EQ, true); }
bool BDecimal::operator!=(double num)		{ return ICompare(num, VARCMP_EQ, true); }



///////////////////////////////////////////////////////////////////////////////////////
// Funciones Utilitarias
BDecimal &BDecimal::ApplyRound(int decimals)
{
	NativeRound(&this->dec, decimals, &this->dec);
	return *this;
}

BDecimal &BDecimal::Round(int decimals)
{
	BDecimal *res = TempDec();
	NativeRound(&this->dec, decimals, &res->dec);
	return *res;
}

BDecimal &BDecimal::Abs()
{
	BDecimal *res = new BDecimal();
	Temp::SetTemporary(res);
	NativeAbs(&this->dec, &res->dec);
	return *res;
}

BDecimal &BDecimal::ApplyNegate()
{
	NativeNeg(&this->dec, &this->dec);
	return *this;
}

BDecimal &BDecimal::Negated()
{
	BDecimal *res = TempDec();
	NativeNeg(&this->dec, &res->dec);
	return *res;
}