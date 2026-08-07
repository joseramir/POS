// Extraido de DecNative.h
#if !defined(BDECIM_DEFINED__) && !defined(SKIP_BDECIM)
#define BDECIM_DEFINED__

#include "TempHandler.h"
#include "DecNative.h"
#include "string.h"

// Numeros decimales por default para divisiones y para cuando intervienen numeros double.
extern int DefaultDecimals;

//  Implementacion del decimal nativo.
class BDecimal
{
private:
	DECIMAL dec;

	static BDecimal *TempDec();
	static BDecimal *TempDec(int num);
	static BDecimal *TempDec(double num);

	bool ICompare(BDecimal &num, int expectedResult, bool negated);
	bool ICompare(int num, int expectedResult, bool negated);
	bool ICompare(double num, int expectedResult, bool negated);

public:
	// Constructores
	BDecimal(void);
	BDecimal(int num);
	BDecimal(double d);
	BDecimal(double d, int decimals);

	// Asignacion
	BDecimal &operator=(BDecimal &d);
	BDecimal &operator=(int num);
	BDecimal &operator=(double num);

	// Suma
	BDecimal &operator+(BDecimal &d);
	BDecimal &operator+(int num);
	BDecimal &operator+(double num);
	BDecimal &operator+=(BDecimal &d);
	BDecimal &operator+=(int num);
	BDecimal &operator+=(double num);

	// Resta
	BDecimal &operator-(BDecimal &d);
	BDecimal &operator-(int num);
	BDecimal &operator-(double num);
	BDecimal &operator-=(BDecimal &d);
	BDecimal &operator-=(int num);
	BDecimal &operator-=(double num);

	// Multiplicacion
	BDecimal &operator*(BDecimal &d);
	BDecimal &operator*(int num);
	BDecimal &operator*(double num);
	BDecimal &operator*=(BDecimal &d);
	BDecimal &operator*=(int num);
	BDecimal &operator*=(double num);

	// Division
	BDecimal &operator/(BDecimal &d);
	BDecimal &operator/(int num);
	BDecimal &operator/(double num);
	BDecimal &operator/=(BDecimal &d);
	BDecimal &operator/=(int num);
	BDecimal &operator/=(double num);

	// Comparaciones
	bool operator==(BDecimal &d);
	bool operator==(int num);
	bool operator==(double num);
	bool operator<=(BDecimal &d);
	bool operator<=(int num);
	bool operator<=(double num);
	bool operator<(BDecimal &d);
	bool operator<(int num);
	bool operator<(double num);
	bool operator>=(BDecimal &d);
	bool operator>=(int num);
	bool operator>=(double num);
	bool operator>(BDecimal &d);
	bool operator>(int num);
	bool operator>(double num);
	bool operator!=(BDecimal &d);
	bool operator!=(int num);
	bool operator!=(double num);

	// Funciones Utilitarias
	BDecimal &ApplyRound(int decimals);
	BDecimal &ApplyNegate(void);
	BDecimal &Round(int decimals);
	BDecimal &Abs(void);
	BDecimal &Negated(void);

	// Conversiones.
	char  *ToString();
	char  *ToString(int decimals);
	int    ToInt32();
	double ToDouble();
	static BDecimal *Parse(char *num);
	operator int();
};

#endif