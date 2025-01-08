#include "Poly.h"

using namespace std;

Poly::Poly(double first){
	this->data = new double[1];
	this->size=1;
	this->data[0]=first;
	
}


Poly::Poly(){
    this->data = new double[1];
    this->size=1;
    this->data[0]=0;

}

Poly::~Poly(){
	delete [] this->data;
}

Poly::Poly(const Poly&  p1){
	this->data=new double[p1.size];
	this->size=p1.size;
	for (int i=0;i<this->size;i++){
		this->data[i] = p1.data[i];
	}
}

Poly& Poly::operator=(const Poly& p1){
	if(this==&p1){
		return *this;
	}
	delete [] this->data;
	this->data = new double[p1.size];
	this->size= p1.size;
	for (int i=0;i<this->size;i++){
		this->data[i] = p1.data[i];
	}
	return *this;
}

double& Poly::operator[](unsigned int pos){
	if(pos>=this->size){
		auto * buffor = new double[pos+1];
        for(int i=0;i<pos+1;i++){
            buffor[i]=0;
        }
		for(int i=0;i<this->size;i++){
			buffor[i]=this->data[i];
		}
		delete [] this->data;
		this->data=buffor;
		this->size=pos+1;
        this->data[pos]=0;
	}
	return this->data[pos];
}

double Poly::operator[](unsigned int pos) const{
	if(pos>=this->size){
		return 0;
	}
	return this->data[pos];
}

ostream& operator << (ostream& o,const Poly&p){
    if(p(1)==0){
        cout<<"0"<<endl;
        return o;
    }
	if(p.data[p.size-1]<0){
        cout<<"-";
    }
    cout<<abs(p.data[p.size-1])<<"x^"<<p.size-1;
	for (int i=(int)p.size-2;i>=0;i--){
        if(p.data[i]==0){
            continue;
        }
        if(p.data[i]<0){
            cout<<" - ";
        }
        else {
            cout<<" + ";
        }
        cout<<abs(p.data[i])<<"x^"<<i<<"";
	}
    cout<<endl;
	return o;
}

Poly operator +(const Poly& p1,const Poly& p2){
	Poly p3(1);
	if(p1.size>=p2.size){
		int i=0;
		for(;i<p2.size;i++){
			p3[i]=p1[i]+p2[i];
		}
		for(;i<p1.size;i++){
			p3[i]=p1[i];
		}
	}
	else {
		int i=0;
		for(;i<p1.size;i++){
			p3[i]=p1[i]+p2[i];
		}
		for(;i<p2.size;i++){
			p3[i]=p2[i];
		}
	}
	return p3;
}

double Poly::operator()(double value) const{
    double sum=0;
    double power=1;
    for(int i=0;i<this->size;i++){
        sum += this->data[i]*power;
        power*=value;
    }
    return sum;
}

Poly operator*(const Poly& p1,const Poly& p2){
    Poly p3(0);
    for(int i=0;i<p1.size;i++){
        for(int j=0;j<p2.size;j++){
            p3[i+j]+=p1[i]*p2[j];
        }
    }
    return p3;
}

Poly operator-(const Poly& p1,const Poly& p2){
    if(&p1==&p2){
        Poly p3;
        for(int i=0;i<p1.size;i++){
            p3[i]=0;
        }
        return p3;
    }
    Poly p3;
    for (int i=0;i<p2.size;i++){
        p3[i]=p2.data[i]*-1;
    }
    return p1+p2;
}

Poly operator+(const Poly& p1, double value){
    Poly p3(p1);
    p3.data[0]+=value;
    return p3;
}