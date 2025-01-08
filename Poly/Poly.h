
#ifndef POLY_H
#define POLY_H

#include <iostream>
#include <math.h>

class Poly{
	double * data;
    unsigned int size;
	
public:
	
	Poly(double first);
    Poly();
	Poly(const Poly& p1);
	~Poly();
	Poly& operator =(const Poly& p1);
	double& operator[](unsigned int pos);
	double operator [](unsigned int pos) const;
	friend std::ostream& operator<<(std::ostream& o,const Poly & p);
	friend Poly operator +(const Poly& p1,const Poly& p2);
	friend Poly operator *(const Poly& p1,const Poly& p2);
	friend Poly operator -(const Poly& p1,const Poly& p2);
    friend Poly operator +(const Poly& p1, double value);
	double operator()(double value) const;
	
};

Poly operator +(const Poly& p1,const Poly& p2);
Poly operator *(const Poly& p1,const Poly& p2);
Poly operator -(const Poly& p1,const Poly& p2);
Poly operator +(const Poly& p1, double value);

#endif
