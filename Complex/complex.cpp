#include "complex.h"
#include <cmath>

#define _USE_MATH_DEFINES
/*
Complex::Complex(){
	this->Real=0;
	this->Imag=0;
}
Complex::Complex(double co){
	this->Real=co;
	this->Imag=0;
}

 */

Complex::Complex(double Real, double Imag){
	this->Real = Real;
    this->Imag = Imag;
}

Complex Complex::operator /(const Complex c){
	Complex n(0,0);
	n.Real = (this->Real*c.Real + this->Imag*c.Imag)/(c.Real*c.Real+c.Imag*c.Imag);
	n.Imag = (c.Real*this->Imag - this->Real*c.Imag)/(c.Real*c.Real+c.Imag*c.Imag);
	return n;
}

Complex& Complex::operator /=(const Complex c){
	Complex n(0,0);
	n.Real = (this->Real*c.Real + this->Imag*c.Imag)/(c.Real*c.Real+c.Imag*c.Imag);
	n.Imag = (c.Real*this->Imag - this->Real*c.Imag)/(c.Real*c.Real+c.Imag*c.Imag);
	this->Real = n.Real;
	this->Imag = n.Imag;
	return *this;
}

Complex & Complex::operator =(const Complex& s){
	Real = s.Real;
	Imag = s.Imag;
	return *this;
}

Complex& Complex::operator =(double co){
	Real = co;
	Imag = 0;
	return *this;
}

Complex Complex::operator -()const{
	return Complex(-Real, -Imag);
}

Complex Complex::operator +(const Complex& co) const{
	Complex n(0,0);
	n.Real = this->Real + co.Real;
	n.Imag = this->Imag + co.Imag;
	return n;
}

Complex& Complex::operator -=(Complex co){
	Real -= co.Real;
	Imag -= co.Imag;
	return *this;
}

ostream& operator<<(ostream& s, const Complex& c){
	s << "(" << c.Real << "," << c.Imag << ")";
    return s;
}

inline Complex operator-(Complex s1, Complex s2){
	Complex n(s1.Real,s1.Imag);
    return n -= s2;
}

double Complex::abs(){
	return sqrt(this->Real*this->Real + this->Imag*this->Imag); 
}

Complex& Complex::conj(){
    this->Imag*=-1;
    return *this;

}

double Complex::phase(){
    if(this->Real>0){
        return atan(this->Real/ this->Imag);
    }
    else if (this->Real<0){
        return atan(this->Real/ this->Imag)+M_PI;
    }
    else{
        if(this->Imag>0){
            return M_PI/2;
        }
        else if(this->Imag<0){
            return M_PI/2*-1;
        }
    }
    return 0;
}







