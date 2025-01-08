#include "my_malloc.h"
#include "string.h"

void memory_init(void *address, size_t size){
    if(address==NULL|| size==0){
        return;
    }
    memory_manager.memory_start=address;
    memory_manager.memory_size=size;
    memory_manager.first_memory_chunk=NULL;
}
void *memory_malloc(size_t size){
    if(size<=0 ||heap_validate()==1){
        return NULL;
    }

    size_t needed_size = count_memory_size(size);
    if(needed_size>memory_manager.memory_size){
        return NULL;
    }
    char * ptr = memory_manager.memory_start;
    if(memory_manager.first_memory_chunk==NULL){
        struct memory_chunk_t * chunk = (struct memory_chunk_t*)ptr;
        chunk->size=size;
        chunk->free=0;
        chunk->prev=NULL;
        chunk->next=NULL;
        memory_manager.first_memory_chunk=chunk;
        ptr+= sizeof(struct memory_chunk_t);
        for (int i=0;i<16;i++){
            *ptr='#';
            ptr+=1;
        }
        ptr+=chunk->size;
        for (int i=0;i<16;i++){
            *ptr='#';
            ptr+=1;
        }
        ptr -= 16+chunk->size;
        return ptr;
    }
    else {
        struct memory_chunk_t * chunk = memory_manager.first_memory_chunk;
        int magic=0;
        size_t size_free=0;
        while (chunk->next!=NULL){
            if(chunk->free==1 && chunk->size>=(size+32)){
                break;
            }
            else {
                ptr = (char*)chunk;
                ptr+= sizeof(struct memory_chunk_t)+chunk->size;
                if(chunk->free==0){
                    ptr+=32;
                }
                char * next_ptr = (char*)chunk->next;
                size_free=next_ptr-ptr;
                if(size_free>= (sizeof(struct memory_chunk_t)+size+32)){
                    magic=1;
                    break;
                }
            }
            chunk=chunk->next;
        }
        if(chunk->next==NULL && magic==0){
            ptr = (char*)chunk;
            ptr+= sizeof(struct memory_chunk_t)+chunk->size;
            if(chunk->free==0){
                ptr+=32;
            }
            struct memory_chunk_t* new_chunk = (struct memory_chunk_t*)ptr;
            if(new_chunk==NULL){
                return NULL;
            }
            new_chunk->size=size;
            new_chunk->free=0;
            new_chunk->next=NULL;
            new_chunk->prev=chunk;
            chunk->next=new_chunk;
            ptr += sizeof(struct memory_chunk_t);
            for (int i=0;i<16;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr+=new_chunk->size;
            for (int i=0;i<16;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr -= 16+new_chunk->size;
            return ptr;
        }
        else if(chunk->next!=NULL && magic==0){
            chunk->size=size;
            chunk->free=0;
            ptr = (char*)chunk;
            ptr+= sizeof(struct memory_chunk_t);
            ptr+=chunk->size+16;
            for (int i=0;i<16;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr -= 16+chunk->size;
            return ptr;
        }
        else if(chunk->next!=NULL && magic==1){
            ptr = (char*)chunk;
            ptr+= sizeof(struct memory_chunk_t)+chunk->size;
            if(chunk->free==0){
                ptr+=32;
            }
            struct memory_chunk_t * new_chunk = (struct memory_chunk_t *)ptr;
            new_chunk->size=size;
            new_chunk->free=0;
            new_chunk->next=chunk->next;
            new_chunk->prev=chunk;
            chunk->next->prev=new_chunk;
            chunk->next=new_chunk;
            ptr += sizeof(struct memory_chunk_t);
            for (int i=0;i<16;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr+=new_chunk->size;
            for (int i=0;i<16;i++){
                *ptr='#';
                ptr+=1;
            }
            ptr -= 16+new_chunk->size;
            return ptr;
        }
    }

    return NULL;
}

size_t count_memory_size(size_t size){
    size_t memory_size=0;
    struct memory_chunk_t * chunk = memory_manager.first_memory_chunk;
    while (chunk!=NULL){
        if(chunk->free==0) {
            memory_size += sizeof(struct memory_chunk_t);
            memory_size += chunk->size;
            memory_size += 32;
            if(memory_size>memory_manager.memory_size){
                break;
            }
        }
        chunk = chunk->next;
    }
    memory_size+= size;
    memory_size += sizeof(struct memory_chunk_t);
    memory_size += 32;
    return memory_size;
}

void memory_free(void *address){
    //printf("\n\n\n\n\n\n\n\n%lu\n\n\n\n", sizeof(struct memory_chunk_t));
    if(address==NULL ||heap_validate()==1){
        return;
    }
    char * ptr = memory_manager.memory_start;
    struct memory_chunk_t * chunk = memory_manager.first_memory_chunk;
    ptr += sizeof(struct memory_chunk_t)+16;
    while (ptr !=address&& chunk->next!=NULL){
        ptr = (char *)chunk->next;
        ptr+= sizeof(struct memory_chunk_t)+16;
        chunk = chunk->next;
    }
    if(ptr!=address){
        return;
    }
    chunk->free=1;
    int free_size=0;
    if(chunk->next!=NULL) {
        while ((struct memory_chunk_t *) ptr != chunk->next) {
            ptr += 1;
            free_size += 1;
        }
        free_size+=16;
        chunk->size=free_size;
    }
    if(chunk->next==NULL && chunk->prev==NULL){
        memory_manager.first_memory_chunk=NULL;
    }
    else if(chunk->next!=NULL && chunk->prev==NULL){
        if(chunk->next->free==1 && chunk->next->next!=NULL){
            chunk->size+=chunk->next->size+ sizeof(struct memory_chunk_t);
            chunk->next->next->prev=chunk;
            chunk->next=chunk->next->next;
        }
    }
    else if(chunk->next==NULL && chunk->prev!=NULL){
        if(chunk->prev->free==0){
            chunk->prev->next=NULL;
        }
        else if(chunk->prev->free==1 && chunk->prev->prev==NULL){
            memory_manager.first_memory_chunk=NULL;
        }
        else if(chunk->prev->free==1 && chunk->prev->prev!=NULL){
            chunk->prev->prev->next=NULL;
        }
    }
    else if(chunk->next!=NULL && chunk->prev!=NULL){
        if(chunk->prev->free==0 && chunk->next->free==1){
            chunk->size+=chunk->next->size+ sizeof(struct memory_chunk_t);
            chunk->next->next->prev=chunk;
            chunk->next=chunk->next->next;
        }
        if(chunk->prev->free==1 && chunk->next->free==0){
            chunk->prev->size+=chunk->size+ sizeof(struct memory_chunk_t);
            chunk->next->prev=chunk->prev;
            chunk->prev->next=chunk->next;
        }
        if(chunk->prev->free==1 && chunk->next->free==1 &&  chunk->next->next!=NULL){
            chunk->prev->size+=chunk->size+ sizeof(struct memory_chunk_t)*2+chunk->next->size;
            chunk->prev->next=chunk->next->next;
            chunk->next->next->prev=chunk->prev;
        }
    }
}

int heap_validate(void){
    char * ptr = memory_manager.memory_start;
    struct memory_chunk_t * chunk = memory_manager.first_memory_chunk;
    while (chunk!=NULL){
        ptr+= sizeof(struct memory_chunk_t);
        if(chunk->free==1){
            ptr+=chunk->size;
            chunk=chunk->next;
        }
        else {
            for (int i = 0; i < 16; i++) {
                if (*ptr != '#') {
                    return 1;
                }
                ptr += 1;
            }
            ptr += chunk->size;
            for (int i = 0; i < 16; i++) {
                if (*ptr != '#') {
                    return 1;
                }
                ptr += 1;
            }
            chunk = chunk->next;
            ptr = (char *)chunk;
        }
    }
    return 0;
}

