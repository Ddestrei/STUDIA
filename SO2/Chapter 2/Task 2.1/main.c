#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

struct clusters_chain_t {
    uint16_t *clusters;
    size_t size;
};

struct uint12_t{
    uint8_t tab[3];
};

struct clusters_chain_t *get_chain_fat16(const void * const buffer, size_t size, uint16_t first_cluster){
    struct clusters_chain_t *chain = (struct clusters_chain_t*) malloc(sizeof(struct clusters_chain_t));
    if(chain==NULL){
        return NULL;
    }
    chain->size=1;
    chain->clusters = (uint16_t *) malloc(chain->size* sizeof(uint16_t));
    *chain->clusters=first_cluster;
    uint16_t * ptr = (uint16_t*)buffer;
    uint16_t current_cluster = first_cluster;
    while (*(ptr+current_cluster)!=65528){
        current_cluster = *(ptr+current_cluster);
        uint16_t * buff = (uint16_t *) realloc(chain->clusters, (chain->size+1)*sizeof(uint16_t));
        chain->clusters=buff;
        *(chain->clusters+chain->size)=current_cluster;
        chain->size+=1;
    }
    return chain;
}


struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster){
    struct clusters_chain_t *chain = (struct clusters_chain_t*) malloc(sizeof(struct clusters_chain_t));
    if(chain==NULL){
        return NULL;
    }
    chain->size=0;
    chain->clusters = (uint16_t *) malloc(chain->size* sizeof(uint16_t));
    uint8_t * ptr = (uint8_t*)buffer;
    /*for(int i=0;i<(int)size;i++){
        printf("%d - %d\n",*(ptr+i),i);
    }*/
    uint16_t old_cluster = first_cluster;
    uint16_t current_cluster = first_cluster;
    uint16_t current_pos = (first_cluster*3)/2;
    while (current_cluster!=4088){
        uint16_t * buffor = (uint16_t*) realloc(chain->clusters,(chain->size+1)*sizeof(uint16_t));
        chain->clusters=buffor;
        if(current_cluster%2==0){
            printf("%d %d\n",*(ptr+current_pos),*(ptr+current_pos+1)%16);
            uint8_t first = *(ptr+current_pos)%16;
            uint8_t second = *(ptr+current_pos)/16;
            uint8_t third = *(ptr+current_pos+1)%16;

            current_cluster = third*16*16+second*16+first;
        }
        else{
            current_pos+=1;
            printf("%d %d\n",*(ptr+current_pos-1), *(ptr+current_pos));
            uint8_t first = *(ptr+current_pos-1)/16;
            uint8_t second = *(ptr+current_pos)%16;
            uint8_t third = *(ptr+current_pos)/16;
            current_cluster = third*16*16+second*16+first;
        }
        *(chain->clusters+chain->size)=old_cluster;
        old_cluster=current_cluster;
        chain->size+=1;
        current_pos = (current_cluster*3)/2;
    }
    return chain;
}

int main(void){
    return 0;
}


/*

struct uint12_t * convert(const void * const buffer,size_t size){
    struct uint12_t * converted = NULL;
    int size_con=0;
    uint8_t * ptr_buffer = (uint8_t*)buffer;
    for(size_t i=0;i<size;i+=3){
        struct uint12_t * buff = (struct uint12_t*) realloc(converted,(size_con+2)*sizeof(struct uint12_t));
        converted=buff;
        uint8_t vals[3];
        vals[0]=*(ptr_buffer+i);
        vals[1]=*(ptr_buffer+i+1);
        vals[2]=*(ptr_buffer+i+2);
        (converted+size_con)->tab[0]=vals[0]%16;
        vals[0]/=16;
        (converted+size_con)->tab[1]=vals[0];
        (converted+size_con)->tab[2]=vals[1]%16;
        vals[1]/=16;
        (converted+size_con+1)->tab[1]=vals[1];
        (converted+size_con+1)->tab[1]=vals[2]%16;
        vals[2]/=16;
        (converted+size_con+1)->tab[2]=vals[2];
        size_con+=2;
    }
    struct uint12_t * buff = (struct uint12_t*) realloc(converted,(size_con+1)*sizeof(struct uint12_t));
    converted=buff;
    *(converted+size_con)=NULL;
    return converted;
}

 */
