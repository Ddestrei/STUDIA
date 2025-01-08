#ifndef __Complex_H__
#define __Complex_H__
#include <iostream>
#include <math.h>

using namespace std;

class Complex {
  private:
    double Real, Imag;

  public:
    //Complex();

    //Complex(double co);

    Complex(double Real, double Imag);
    
	Complex operator /(const Complex c);
	
	Complex& operator /=(const Complex c);
	
    Complex& operator=(const Complex& s);

    Complex operator-() const;

    Complex& operator=(double co);

    Complex operator+(const Complex& co) const;

    Complex& operator-=(Complex co);

    double phase();

    double abs();

    friend Complex operator-(Complex, Complex);
    friend ostream& operator<<(ostream& s, const Complex& c);

    Complex& conj();

};

inline Complex operator-(Complex s1, Complex s2);

#endif /* __Complex_H__ */
