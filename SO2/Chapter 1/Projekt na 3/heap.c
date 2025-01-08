#include "heap.h"

#include "stdio.h"

int heap_setup(void){
    heap.first=NULL;
    heap.start_heap= custom_sbrk(0);
    heap.size=0;
    if(heap.start_heap==NULL){
        return 1;
    }
    return 0;
}
void heap_clean(void){
    if(heap.start_heap==NULL){
        return;
    }
    long size = (char*)custom_sbrk(0)-(char*)heap.start_heap;
    custom_sbrk(size*-1);
    heap.size=0;
    heap.first=NULL;
    heap.start_heap=NULL;
    return;
}
void* heap_malloc(size_t size){
    if(size<=0){
        return NULL;
    }
    uint32_t needed_size_for_block = size+ sizeof(struct memblock_t)+4;
    uint32_t needed_size = count_heap_size()+needed_size_for_block;
    needed_size-=needed_size%64;
    needed_size+=64;
    if(needed_size>heap.size){
        if(resize_heap(needed_size-heap.size)==-1){
            return NULL;
        }
    }
    struct memblock_t * tmp = heap.first;
    if(tmp==NULL){
        char * ptr = (char *)heap.start_heap;
        tmp = (struct memblock_t *)ptr;
        tmp->next=NULL;
        tmp->prev=NULL;
        tmp->size=(int32_t)size;
        tmp->block_size = tmp->size+2*FENCES_SIZE+ sizeof(struct memblock_t);
        heap.first=tmp;
        ptr += sizeof(struct memblock_t);
        for(int i=0;i<FENCES_SIZE;i++){
            *ptr='#';
            ptr+=1;
        }
        ptr+=(int)size;
        for(int i=0;i<FENCES_SIZE;i++){
            *ptr='#';
            ptr+=1;
        }
        count_control_sum(tmp);
        return ptr- size-FENCES_SIZE;
    }
    else {
        // 0 - at the end
        // 1 - use free block
        // 2 - between blocks
        int malloc_option=0;
        while (tmp->next!=NULL){
            if(tmp->size<0){
                if(tmp->block_size>=(int)size+2*FENCES_SIZE+ sizeof(struct memblock_t)){
                    malloc_option=1;
                    break;
                }
            }
            if(tmp->size>0) {
                if (tmp->block_size - tmp->size - sizeof(struct memblock_t) - FENCES_SIZE * 2 >
                    sizeof(struct memblock_t) + FENCES_SIZE * 2 + size) {
                    malloc_option = 2;
                    break;
                }
            }
            tmp=tmp->next;
        }
        if(malloc_option==0) {
            char *ptr = (char *) tmp;
            ptr += tmp->block_size;
            struct memblock_t *new = (struct memblock_t *) ptr;
            new->prev = tmp;
            new->next = NULL;
            new->size = (int32_t) size;
            new->block_size=new->size+ sizeof(struct memblock_t)+2*FENCES_SIZE;
            tmp->next = new;
            ptr += sizeof(struct memblock_t);
            for(int i=0;i<FENCES_SIZE;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr += (int) size;
            for(int i=0;i<FENCES_SIZE;i++){
                *ptr='#';
                ptr+=1;
            }
            count_control_sum(new);
            count_control_sum(tmp);
            return ptr - size - FENCES_SIZE;
        }
        else if(malloc_option==1){
            char * ptr = (char*)tmp;
            tmp->size=(int32_t)size;
            //tmp->block_size=(int32_t)size+ sizeof(struct memblock_t)+2*FENCES_SIZE;
            ptr+= sizeof(struct memblock_t)+tmp->size+FENCES_SIZE;
            for(int i=0;i<FENCES_SIZE;i++){
                *ptr='#';
                ptr+=1;
            }
            count_control_sum(tmp);
            return ptr-tmp->size-FENCES_SIZE;
        }
        else if(malloc_option==2){
            char * ptr = (char *)tmp;
            ptr+=tmp->size+FENCES_SIZE*2+ sizeof(struct memblock_t);
            struct memblock_t * new = (struct memblock_t *)ptr;
            new->size=(int32_t)size;
            new->prev=tmp;
            new->next=tmp->next;
            tmp->next=new;
            new->next->prev=new;
            new->block_size=(char*)new->next-(char*)new;
            ptr = (char *)new;
            ptr+=sizeof(struct memblock_t);
            for(int i=0;i<FENCES_SIZE;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr+=new->size;
            for(int i=0;i<FENCES_SIZE;i++){
                *ptr='#';
                ptr+=1;
            }
            tmp->block_size=tmp->size+ sizeof(struct memblock_t)+FENCES_SIZE*2;
            count_control_sum(new);
            count_control_sum(new->next);
            count_control_sum(new->prev);
            return ptr-new->size-FENCES_SIZE;
        }
    }
    return NULL;
}

uint32_t count_heap_size(){
    uint32_t size =0;
    struct memblock_t * tmp = heap.first;
    while (tmp!=NULL){
        size+=tmp->block_size;
        tmp = tmp->next;
    }
    return size;
}

int resize_heap(uint32_t size){
    if(heap.size+size>=67108864){
        return -1;
    }
    custom_sbrk(size);
    void * ptr = custom_sbrk(0);
    if(ptr==NULL){
        return -1;
    }
    heap.size+=size;
    return 1;
}

void* heap_calloc(size_t number, size_t size){
    if(size<=0){
        return NULL;
    }
    char * ptr = (char*)heap_malloc(number*size);
    if(ptr==NULL){
        return NULL;
    }
    for(size_t i=0;i<number*size;i++){
        *(ptr+i)='\0';
    }
    return ptr;
}
void* heap_realloc(void* memblock, size_t count){
    if(heap_validate()!=0){
        return NULL;
    }
    if(memblock==NULL){
        return heap_malloc(count);
    }
    if(count==0){
        heap_free(memblock);
        return NULL;
    }
    if(count_heap_size()+count+ sizeof(struct memblock_t)+2*FENCES_SIZE>67108864){
        return NULL;
    }
    struct memblock_t * tmp = find_block(memblock);
    if(tmp==NULL){
        return NULL;
    }
    int tmp_size = 0;
    if(count>(size_t)tmp->size){
        tmp_size=tmp->size;
    }
    else {
        tmp_size=(int)count;
    }
    heap_free(memblock);
    void * ptr = heap_malloc(count);
    memcpy(ptr,memblock,tmp_size);
    return ptr;
}
void  heap_free(void* memblock){
    if(memblock==NULL){
        return;
    }
    if(heap_validate()!=0){
        return;
    }
    if(heap.first==NULL){
        return;
    }
    if(get_pointer_type(memblock)!=pointer_valid){
        return;
    }
    struct memblock_t * tmp = find_block(memblock);
    tmp->size*=-1;
    count_control_sum(tmp);
    if(tmp->next==NULL && tmp->prev==NULL){
        heap.first=NULL;
    }
    else if(tmp->next!=NULL && tmp->prev==NULL){
        if(tmp->next->size<0 && tmp->next->next!=NULL){
            tmp->block_size+=tmp->next->block_size;
            tmp->next->next->prev=tmp;
            tmp->next=tmp->next->next;
            count_control_sum(tmp);
            count_control_sum(tmp->next);
        }
    }
    else if(tmp->next==NULL && tmp->prev!=NULL){
        if(tmp->prev->size>0){
            tmp->prev->next=NULL;
            tmp->prev->block_size+=tmp->block_size;
            count_control_sum(tmp->prev);
        }
        else if(tmp->prev->size<0 && tmp->prev->prev==NULL){
            heap.first=NULL;
        }
        else if(tmp->prev->size<0 && tmp->prev->prev!=NULL){
            tmp->prev->prev->next=NULL;
            tmp->prev->prev->block_size+=tmp->block_size+tmp->prev->block_size;
            count_control_sum(tmp->prev->prev);
        }
    }
    else if(tmp->next!=NULL && tmp->prev!=NULL){
        if(tmp->prev->size>0 && tmp->next->size<0){
            tmp->block_size+=tmp->next->block_size;
            tmp->next->next->prev=tmp;
            tmp->next=tmp->next->next;
            count_control_sum(tmp);
            count_control_sum(tmp->next);
        }
        else if(tmp->prev->size<0 && tmp->next->size>0){
            tmp->prev->block_size+=tmp->block_size;
            tmp->next->prev=tmp->prev;
            tmp->prev->next=tmp->next;
            count_control_sum(tmp->prev);
            count_control_sum(tmp->next);
        }
        else if(tmp->prev->size<0 && tmp->next->size<0 && tmp->next->next!=NULL){
            tmp->prev->block_size+=tmp->block_size+tmp->next->block_size;
            tmp->prev->next=tmp->next->next;
            tmp->next->next->prev=tmp->prev;
            count_control_sum(tmp->prev);
            count_control_sum(tmp->next->next);
        }
    }
}

size_t   heap_get_largest_used_block_size(void){
    if(heap.first==NULL || heap.start_heap==NULL || heap_validate()!=0){
        return 0;
    }
    size_t max=0;
    struct memblock_t * tmp = heap.first;
    while (tmp!=NULL){
        if((int32_t)max<tmp->size){
            max=tmp->size;
        }
        tmp = tmp->next;
    }
    return (size_t)max;
}
enum pointer_type_t get_pointer_type(const void* const pointer){
    if(pointer==NULL){
        return pointer_null;
    }
    if(heap_validate()!=0){
        return pointer_heap_corrupted;
    }
    struct memblock_t * tmp = heap.first;
    while (tmp!=NULL){
        char * ptr = (char *)tmp;
        for(size_t i=0;i< sizeof(struct memblock_t );i++){
            if(ptr==(char*)pointer){
                if(tmp->size<0){
                    return pointer_unallocated;
                }
                return pointer_control_block;
            }
            ptr+=1;
        }
        for(uint8_t i=0; i<FENCES_SIZE;i++){
            if(ptr==(char*)pointer){
                if(tmp->size<0){
                    return pointer_unallocated;
                }
                return pointer_inside_fences;
            }
            ptr+=1;
        }
        if(ptr==(char*)pointer && tmp->size>0){
            if(tmp->size<0){
                return pointer_unallocated;
            }
            return pointer_valid;
        }
        ptr+=1;
        if(tmp->size<0){
            for(int32_t i=-1;i>tmp->size;i--){
                if(ptr==(char*)pointer){
                    if(tmp->size<0){
                        return pointer_unallocated;
                    }
                    return pointer_inside_data_block;
                }
                ptr+=1;
            }
        }
        else {
            for(int32_t i=1;i<tmp->size;i++){
                if(ptr==(char*)pointer){
                    if(tmp->size<0){
                        return pointer_unallocated;
                    }
                    return pointer_inside_data_block;
                }
                ptr+=1;
            }
        }
        for(uint8_t i=0; i<FENCES_SIZE;i++){
            if(ptr==(char*)pointer){
                if(tmp->size<0){
                    return pointer_unallocated;
                }
                return pointer_inside_fences;
            }
            ptr+=1;
        }
        tmp=tmp->next;
    }
    return pointer_unallocated;
}
int heap_validate(void){
    if(heap.start_heap==NULL){
        return 2;
    }
    struct memblock_t * tmp = heap.first;
    while (tmp!=NULL){
        char * ptr = (char*)tmp;
        if(check_control_sum(tmp)==1){
            return 3;
        }
        ptr+= sizeof(struct memblock_t);
        for(int i=0;i<FENCES_SIZE;i++){
            if(*ptr!='#'){
                return 1;
            }
            ptr+=1;
        }
        if(tmp->size>0){
            ptr+=tmp->size;
        }
        else {
            ptr+=tmp->size*-1;
        }
        for(int i=0;i<FENCES_SIZE;i++){
            if(*ptr!='#'){
                return 1;
            }
            ptr+=1;
        }
        //prev=tmp;
        tmp = tmp->next;
    }
    return 0;
}


void print_heap_in_file(char * file){
    FILE * f = fopen(file,"wb");
    for(uint32_t i=0;i<heap.size;i++){
        fprintf(f,"%c",*((uint8_t*)heap.start_heap+i));
    }
    fclose(f);
}

void count_control_sum(struct memblock_t * block){
    int32_t control_sum=0;
    char * ptr = (char *)block;
    for(int i=0;i< (int)sizeof(struct memblock_t)- (int)sizeof(block->control_sum);i++){
        control_sum+=*(ptr+i);
    }
    block->control_sum=control_sum;
}

int check_control_sum(struct memblock_t * block){
    int32_t control_sum=0;
    char * ptr = (char *)block;
    for(int i=0;i< (int)sizeof(struct memblock_t)- (int)sizeof(block->control_sum);i++){
        control_sum+=*(ptr+i);
    }
    if(control_sum!=block->control_sum){
        return 1;
    }
    return 0;
}

struct memblock_t * find_block(void * pointer){
    struct memblock_t* tmp = heap.first;
    char * ptr = (char *)tmp;
    ptr+= sizeof(struct memblock_t)+FENCES_SIZE;
    while (ptr!=pointer){
        tmp = tmp->next;
        if(tmp==NULL){
            return NULL;
        }
        ptr = (char *)tmp;
        ptr+= sizeof(struct memblock_t)+FENCES_SIZE;
    }
    return tmp;
}

/*
 if (tmp->next->size * -1 > (int32_t) needed_size) {
                    struct memblock_t *next = tmp->next;
                    tmp->size = count;
                    next->size += needed_size;
                    char *ptr = (char *) next;
                    memmove(ptr + needed_size, ptr, sizeof(struct memblock_t) + 2);
                    next+=needed_size;
                    tmp->next=next;
                    ptr = (char *) next;
                    ptr += sizeof(struct memblock_t) - next->size + FENCES_SIZE;
                    for (int i = 0; i < 2; i++) {
                        *ptr = '#';
                        ptr += 1;
                    }
                    ptr = (char *) tmp;
                    ptr += sizeof(struct memblock_t) + FENCES_SIZE + tmp->size;
                    for (int i = 0; i < FENCES_SIZE; i++) {
                        *ptr = '#';
                        ptr += 1;
                    }
                    tmp->block_size = sizeof(struct memblock_t) + FENCES_SIZE * 2 + tmp->size;
                    next->block_size = sizeof(struct memblock_t) + FENCES_SIZE * 2 - next->size;
                    count_control_sum(tmp);
                    count_control_sum(tmp->next);
                    return ptr - FENCES_SIZE - tmp->size;
                }
                else
 */

/*
 if((tmp->block_size- sizeof(struct memblock_t)-FENCES_SIZE*2)+
                    (tmp->next->block_size- sizeof(struct memblock_t)-FENCES_SIZE*2)<count){
                used_used_type=used_type;
                used_type=3;
                tmp->next->next->prev=tmp;
                tmp->next=tmp->next->next;
                tmp->size=count;
                char * ptr =
                return ptr;
            }
            else if(tmp->block_size- sizeof(struct memblock_t)-FENCES_SIZE*2<count) {
                unsigned long needed_size = count - (tmp->block_size- sizeof(struct memblock_t)-FENCES_SIZE*2);
                if(tmp->block_size>needed_size){
                    used_used_type=used_type;
                    used_type=4;
                    tmp->size=count;
                    if(tmp->next->next!=NULL){
                        tmp->next=tmp->next->next;
                        tmp->next->prev=tmp;
                        count_control_sum(tmp->next);
                    }
                    else {
                        tmp->next=NULL;
                    }
                    char * ptr = (char *)tmp;
                    ptr+= sizeof(struct memblock_t) + FENCES_SIZE + tmp->size;
                    for (int i = 0; i < FENCES_SIZE; i++) {
                        *ptr = '#';
                        ptr += 1;
                    }
                    tmp->block_size = sizeof(struct memblock_t) + FENCES_SIZE * 2 + tmp->size;
                    count_control_sum(tmp);
                    return ptr - FENCES_SIZE - tmp->size;
                }
            }
            else{
                used_used_type=used_type;
                used_type=5;
                tmp->size=count;
                char * ptr = (char *) tmp;
                ptr += sizeof(struct memblock_t) + FENCES_SIZE + tmp->size;
                for (int i = 0; i < FENCES_SIZE; i++) {
                    *ptr = '#';
                    ptr += 1;
                }
                tmp->block_size = sizeof(struct memblock_t) + FENCES_SIZE * 2 + tmp->size;
                count_control_sum(tmp);
                return ptr - FENCES_SIZE - tmp->size;
            }
 */