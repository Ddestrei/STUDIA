
#ifndef EMPTY_PROJECT_FILE_READER_H
#define EMPTY_PROJECT_FILE_READER_H

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// int bits_per_fat_entry = (student_id % 2 == 1) ? 12 : 16;
// 251147 FAT12

/// MOJE FUNKCJE I STRUKTURY

// zawiera informacje na temat folderu oraz pliki które znajduję się w tym folderze

#define SIZE_OF_ENTRY 32

struct clusters_chain_t {
    uint16_t *clusters;
    size_t size;
};

struct dir_t{
    struct dir_entry_t * entry;
    struct file_t ** files;
    struct dir_t ** folders;
    struct clusters_chain_t * clusters;
    int amount_files;
    int amount_folders;
    int deleted;
};

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
    uint16_t first_cluster;
    long off_set;
};

struct __attribute__((packed)) Boot_sector{
    char OEM[3];
    char name[8];
    uint16_t bytes_per_sector;
    uint8_t sector_per_cluster;
    uint16_t reserved_area_in_sector;
    uint8_t number_of_fats;
    uint16_t max_files_in_root_dir;
    uint16_t sectors_in_file_system;
    uint8_t media_type;
    uint16_t size_of_each_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t number_of_sectors_before_start_partition;
    uint32_t number_of_sectors_in_file_system;
    uint8_t BIOS_init;
    uint8_t not_used;
    uint8_t extended_bios_signature;
    uint32_t volume_serial_number;
    char volume_label[11];
    char file_system_type[8];
    char not_used_second[448];
    uint16_t signature_value;

};

struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster);
struct dir_entry_t ** read_entries(FILE * f,int max_number_of_entries);
struct dir_entry_t *read_directory_entry(FILE * f);
void display_entry(struct dir_entry_t * entry);
int read_directory(struct dir_t * dir, FILE * f, int cluster_size,uint8_t * fat_table,size_t fat_size);
void display_folder(struct dir_t * dir);
struct file_t* find_file(struct dir_t * dir, const char * file_name, int *error_code);
void file_delete(struct file_t * file);
struct dir_t* find_dir(struct dir_t * dir, const char * file_name);

extern int reader_entries;

/// FUNKCJE I STRUKTURY Z DANTE(DANTE HUI)

struct disk_t{
    FILE *f;
    struct Boot_sector * bootSector;
    struct volume_t * volume;
    char * disc_name;
};
struct disk_t* disk_open_from_file(const char* volume_file_name);
int disk_read(struct disk_t* pdisk, int32_t first_sector, void* buffer, int32_t sectors_to_read);
int disk_close(struct disk_t* pdisk);

struct volume_t{
    uint8_t * fat_table_in_bytes;
    int fat_size;
    struct dir_t * main_dir;
    char * disc_name;
};
struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector);
int fat_close(struct volume_t* pvolume);

//zawiera dir_entry_t na swój temat oraz pozycje od krórej zaczynją się dane na temat tego pliku.
struct file_t{
    struct dir_entry_t * entry;
    FILE * f;
    struct clusters_chain_t * clusters;
    int cluser_size;
    size_t pos_in_file;
};

struct file_t* file_open(struct volume_t* pvolume, const char* file_name);
int file_close(struct file_t* stream);
size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream);
int32_t file_seek(struct file_t* stream, int32_t offset, int whence);


struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path);
int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry);
int dir_close(struct dir_t* pdir);

#endif //EMPTY_PROJECT_FILE_READER_H
