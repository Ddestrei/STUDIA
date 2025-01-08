#include "Poly.h"

int main(){
	Poly p1(0);
	p1[0]=1;
	p1[1]=1;
	p1[2]=1;
	p1[3]=1;
	std::cout<<p1;
	Poly p2(0);
	p2[0]=2;
	p2[1]=2;
	p2[2]=2;
	p2[3]=2;
	std::cout<<p2;
	Poly p3=p1+p2;
	std::cout<<p3;
    std::cout<<p3(1)<<std::endl;
    Poly p4(0);
    p4[0]=1;
    p4[1]=2;
    p4[2]=1;
    Poly p5(0);
    p5[0]=1;
    p5[1]=2;
    p5[2]=1;
    Poly p6 = p4*p5;
    std::cout<<p4;
    std::cout<<p5;
    std::cout<<p6;

    Poly p7(0);
    p7[0]=1;
    p7[1]=1;

    Poly p8(0);
    p8[0]=1;
    p8[1]=1;
    p8[2]=2;

    std::cout<<p7-p8;

    return 0;
}
