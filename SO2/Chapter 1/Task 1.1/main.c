#include "stdio.h"
#include "inttypes.h"

int sum_short_int_int(const void * const in1, const void * const in2, void *out);

int main(void){

    int in1_int = 1721231348;
    short in2_short = 12916;
    int out_int;

    int * in1 = &in1_int;
    short * in2 = &in2_short;
    int * out = &out_int;
    sum_short_int_int((void*)in2,(void*)in1, (void*)out);
    printf("%d", out_int);
    return 0;
}
int sum_short_int_int(const void * const in1, const void * const in2, void *out){
    if(in1==NULL || in2==NULL || out==NULL){
        return 1;
    }
    int input1=0;
    int input2=0;
    input1+=*(int16_t*)in1;
    input2+=*(int32_t*)in2;
    //printf("%d %d\n", input1,input2);
    *(int*)out = input1 + input2;
    return 0;
}