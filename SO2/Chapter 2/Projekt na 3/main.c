#include "file_reader.h"

/// http://www.c-jump.com/CIS24/Slides/FAT/lecture.html

int main(void){
    struct disk_t *disk = disk_open_from_file("race_fat12_volume.img");
    struct volume_t * volume = fat_open(disk,0);
    display_folder(volume->main_dir);
    display_folder(*volume->main_dir->folders);
    printf("%d",(*volume->main_dir->folders)->entry->first_cluster);

    return 0;
}
