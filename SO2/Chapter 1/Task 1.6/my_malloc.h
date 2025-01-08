#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>

struct memory_manager_t
{
    void *memory_start;
    size_t memory_size;
    struct memory_chunk_t *first_memory_chunk;
};

struct memory_chunk_t
{
    struct memory_chunk_t* prev;
    struct memory_chunk_t* next;
    size_t size;
    int free;
};

struct memory_manager_t memory_manager;

void memory_init(void *address, size_t size);
void *memory_malloc(size_t size);
void memory_free(void *address);
size_t count_memory_size(size_t size);

int heap_validate(void);

#endif