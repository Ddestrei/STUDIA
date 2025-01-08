#include "inttypes.h"
#include "stdio.h"

double sum(const void * const buffer, int n);

int main(void){

    unsigned char output_array[] = {0x48, 0xcd, 0x57, 0xb8, 0xfa, 0xdc, 0x4, 0xb7, 0x56, 0x55, 0x2b, 0xcb, 0x97, 0x4a, 0x40};

    double res = sum(output_array, 1);
    printf("%lf\n", res);
    return 0;
}

double sum(const void * const buffer, int n){
    if(buffer==NULL || n<=0){
        return -1;
    }

    double sum=0;
    int offset=0;
    for (int i=0;i<n;i++){
        sum += (double)*((uint8_t*)buffer+offset);
        offset+=1;
        sum += (double)*(uint16_t*)((uint8_t*)buffer+offset);
        offset+=2;
        sum += (double)*(uint32_t*)((uint8_t*)buffer+offset);
        offset+=4;
        sum += *(double *)((uint8_t*)buffer+offset);
        offset+=8;
    }
    return sum;
}