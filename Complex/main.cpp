#include "complex.h"

int main(){
	std::cout<<"Working"<<std::endl;
	Complex c1(1,8);
	Complex c2(2,3);
	c2 = c2/c1;
	std::cout<<c2;
	std::cout<<std::endl;
	
	Complex c3(1,8);
	Complex c4(2,3);
	c3/=c4;
	std::cout<<c2;
	std::cout<<std::endl;

    std::cout<<"Modul liczby 2 + 3i = "<<c4.abs()<<endl;
    std::cout<<"Sprzezenie liczby 2 + 3i = "<<c4.conj()<<endl;
    std::cout<<"Faza liczby 2 + 3i = "<<c4.phase()<<endl;

    Complex c5(1,0);
    c5 = c5 - Complex(1,0);
    std::cout<<c5<<endl;
    return 0;
}
