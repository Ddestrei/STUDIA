//
// Created by root on 11/14/24.
//

#ifndef EMPTY_PROJECT_HEAP_H
#define EMPTY_PROJECT_HEAP_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#define FENCES_SIZE 2
#define PADDING 64

struct __attribute__((packed)) memblock_t{
    struct memblock_t * next;
    struct memblock_t * prev;
    size_t block_size;
    int32_t size;
    int32_t control_sum;
};

struct __attribute__((packed)) heap_t{
    struct memblock_t * first;
    void * start_heap;
    uint32_t size;
};

enum pointer_type_t
{
    pointer_null,
    pointer_heap_corrupted,
    pointer_control_block,
    pointer_inside_fences,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};
int heap_setup(void);
void heap_clean(void);
void* heap_malloc(size_t size);
void* heap_calloc(size_t number, size_t size);
void* heap_realloc(void* memblock, size_t count);
void  heap_free(void* memblock);
uint32_t count_heap_size();
int resize_heap(uint32_t size);
size_t   heap_get_largest_used_block_size(void);
enum pointer_type_t get_pointer_type(const void* const pointer);
int heap_validate(void);
void print_heap_in_file();
void count_control_sum(struct memblock_t * block);
int check_control_sum(struct memblock_t * block);
struct memblock_t * find_block(void * pointer);

struct heap_t heap;

#endif //EMPTY_PROJECT_HEAP_H
