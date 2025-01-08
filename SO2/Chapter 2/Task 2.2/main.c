#include "directory_reader.h"

int main(void){
    char name[100];
    printf("Enter file name:");
    scanf("%s",name);
    struct dir_entry_t * entry = read_directory_entry(name);
    while (entry!=NULL){
        if(entry->is_directory==0) {
            printf("%02d/%02d/%04d %02d:%02d   %11zu  %s\n", entry->creation_date_day, entry->creation_date_mount,
                   entry->creation_date_year, entry->creation_time_hour, entry->creation_time_minute,entry->size,entry->name);
        }
        else{
            printf("%02d/%02d/%04d %02d:%02d   *DIRECTORY*  %s\n", entry->creation_date_day, entry->creation_date_mount,
                   entry->creation_date_year, entry->creation_time_hour, entry->creation_time_minute,entry->name);
        }
        free(entry);
        entry= read_directory_entry(NULL);

    }
    return 0;
}
