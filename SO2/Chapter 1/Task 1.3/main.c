#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

void *concat_zip(const void * const float_buffer, const void * const int16_t_buffer, int size);

void display_float_short(const void * const ptr, int size);

int main(void){

    int16_t int_array[] = {76};
    float float_array[] = {-82.025299};
    void *ptr = concat_zip(float_array, int_array, 1);
    display_float_short(ptr,6);
    free(ptr);
    return 0;
}

void *concat_zip(const void * const float_buffer, const void * const int16_t_buffer, int size){
    if(float_buffer==NULL || int16_t_buffer==NULL || size<=0){
        return NULL;
    }

    void* concat_array = malloc(2*size+4*size);
    if(concat_array==NULL){
        return NULL;
    }

    for(int i=0, j=0;i<size;i++){
        *(float *)((uint8_t*)concat_array+j) = *(float *)((uint8_t*)float_buffer+(i*4));
        j+=4;
        *(int16_t *)((uint8_t*)concat_array+j) = *(int16_t *)((uint8_t*)int16_t_buffer+(i*2));
        j+=2;
    }
    return concat_array;
}

void display_float_short(const void * const ptr, int size){
    for(int i=0;i<size;i++){
        printf("%02x ", *(uint8_t*)((uint8_t*)ptr+i));
    }
}
