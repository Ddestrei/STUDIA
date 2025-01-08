//
// Created by root on 12/3/24.
//

#ifndef DIRECTORY_HEADER_H
#define DIRECTORY_HEADER_H
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct __attribute__((packed)) dir_entry_t{
    char name[13];
    size_t size;
    int is_archived;
    int is_readonly;
    int is_system;
    int is_hidden;
    int is_directory;
    uint32_t creation_date_day;
    uint32_t creation_date_mount;
    uint32_t creation_date_year;
    uint32_t creation_time_hour;
    uint32_t creation_time_minute;
    uint32_t creation_time_second;
    int first_cluster;
};

struct dir_entry_t *read_directory_entry(const char *filename);

FILE * f;
#endif //DIRECTORY_HEADER_H
