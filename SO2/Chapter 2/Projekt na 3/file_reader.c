#include "file_reader.h"


struct disk_t* disk_open_from_file(const char* volume_file_name){
    if(volume_file_name==NULL){
        printf("ERROR CODE: %d\n",EFAULT);
        return NULL;
    }
    struct disk_t * disk = (struct disk_t *) malloc(sizeof(struct disk_t));
    if(disk==NULL){
        printf("ERROR CODE: %d\n",ENOMEM);
        return NULL;
    }
    disk->f= fopen(volume_file_name,"rb");
    if(disk->f==NULL){
        free(disk);
        printf("ERROR CODE: %d\n",ENOENT);
        return NULL;
    }
    disk->bootSector = (struct Boot_sector*) malloc(sizeof(struct Boot_sector));
    if(disk->bootSector==NULL){
        free(disk);
        fclose(disk->f);
        printf("ERROR CODE: %d\n",ENOMEM);
        return NULL;
    }
    fread(disk->bootSector, sizeof(struct Boot_sector),1,disk->f);
    disk->volume=NULL;
    disk->disc_name=(char *)volume_file_name;
    return disk;
}

int disk_close(struct disk_t* pdisk){
    if(pdisk==NULL){
        printf("ERROR CODE: %d\n",EFAULT);
        return -1;
    }
    //fat_close(pdisk->volume);
    fclose(pdisk->f);
    free(pdisk->bootSector);
    free(pdisk->volume);
    free(pdisk);
    return 0;
}

struct volume_t* fat_open(struct disk_t* pdisk, uint32_t first_sector){
    if(pdisk==NULL ){
        printf("ERROR CODE: %d\n",EFAULT);
        return NULL;
    }
    if(pdisk->bootSector->signature_value!=43605){
        return NULL;
    }
    pdisk->volume = (struct volume_t *) malloc(sizeof(struct volume_t));
    if(pdisk->volume==NULL){
        disk_close(pdisk);
        printf("ERROR CODE: %d\n",ENOMEM);
        return NULL;
    }
    pdisk->volume->main_dir = (struct dir_t *) malloc(sizeof(struct dir_t));
    if(pdisk->volume->main_dir==NULL){
        disk_close(pdisk);
        printf("ERROR CODE: %d\n",ENOMEM);
        return NULL;
    }
    pdisk->volume->main_dir->clusters=NULL;
    pdisk->volume->main_dir->entry=NULL;
    pdisk->volume->main_dir->files=NULL;
    pdisk->volume->main_dir->folders=NULL;
    pdisk->volume->main_dir->deleted=0;
    pdisk->volume->main_dir->amount_files=(int)first_sector;
    pdisk->volume->main_dir->amount_files-=(int)first_sector;
    pdisk->volume->main_dir->amount_folders=0;
    pdisk->volume->disc_name=pdisk->disc_name;
    pdisk->volume->fat_size=pdisk->bootSector->size_of_each_fat*pdisk->bootSector->bytes_per_sector;
    fseek(pdisk->f,pdisk->bootSector->reserved_area_in_sector*pdisk->bootSector->bytes_per_sector,SEEK_SET);
    uint8_t * first_fat_bytes = (uint8_t *) malloc(pdisk->volume->fat_size);
    uint8_t * second_fat_bytes = (uint8_t *) malloc(pdisk->volume->fat_size);
    if(first_fat_bytes==NULL || second_fat_bytes == NULL){
        disk_close(pdisk);
        printf("ERROR CODE: %d\n",ENOMEM);
        return NULL;
    }
    fread(first_fat_bytes,1,pdisk->volume->fat_size,pdisk->f);
    fread(second_fat_bytes,1,pdisk->volume->fat_size,pdisk->f);
    //printf("%d %d %d %d",*first_fat_bytes,*(first_fat_bytes+pdisk->volume->fat_size-1),
    //       *second_fat_bytes,*(second_fat_bytes+pdisk->volume->fat_size-1));
    for(int i=0;i<pdisk->volume->fat_size;i++){
        if(*(first_fat_bytes+i)!=*(second_fat_bytes+i)){
            disk_close(pdisk);
            printf("ERROR CODE: %d\n",EINVAL);
            return NULL;
        }
    }
    free(second_fat_bytes);
    pdisk->volume->fat_table_in_bytes=first_fat_bytes;
    // pdisc->f wskazuje na pierwszy bajt po tablicach FAT12
    long start_of_data_arena = ftell(pdisk->f)+pdisk->bootSector->max_files_in_root_dir*SIZE_OF_ENTRY;
    struct dir_entry_t * entry = NULL;
    for(int i=0;i<pdisk->bootSector->max_files_in_root_dir;i++){
        fseek(pdisk->f,start_of_data_arena-(pdisk->bootSector->max_files_in_root_dir-i)*SIZE_OF_ENTRY,SEEK_SET);
        entry = read_directory_entry(pdisk->f);
        if(entry!=NULL) {
            entry->off_set =  start_of_data_arena-pdisk->bootSector->sector_per_cluster*pdisk->bootSector->bytes_per_sector*2;
            if (entry->is_directory == 1) {
                if (pdisk->volume->main_dir->amount_folders == 0) {
                    pdisk->volume->main_dir->folders = (struct dir_t **) malloc(sizeof(struct dir_t *));
                    if (pdisk->volume->main_dir->folders == NULL) {
                        disk_close(pdisk);
                        printf("ERROR CODE: %d\n", ENOMEM);
                        return NULL;
                    }
                    pdisk->volume->main_dir->amount_folders = 1;
                } else {
                    struct dir_t **buffor = (struct dir_t **) realloc( pdisk->volume->main_dir->folders,
                                                                       sizeof(struct dir_t *) * (pdisk->volume->main_dir->amount_folders + 1));
                    if (buffor == NULL) {
                        disk_close(pdisk);
                        printf("ERROR CODE: %d\n", ENOMEM);
                        return NULL;
                    }
                    pdisk->volume->main_dir->folders = buffor;
                    pdisk->volume->main_dir->amount_folders += 1;
                }
                struct dir_t *dir = (struct dir_t *) malloc(sizeof(struct dir_t));
                if (dir == NULL) {
                    disk_close(pdisk);
                    printf("ERROR CODE: %d\n", ENOMEM);
                    return NULL;
                }
                dir->entry = entry;
                dir->folders = NULL;
                dir->files = NULL;
                dir->amount_files=0;
                dir->amount_folders=0;
                dir->clusters = get_chain_fat12(pdisk->volume->fat_table_in_bytes, pdisk->volume->fat_size, entry->first_cluster);
                *(pdisk->volume->main_dir->folders+pdisk->volume->main_dir->amount_folders-1)=dir;

                int return_0 = read_directory(dir,pdisk->f,pdisk->bootSector->sector_per_cluster*pdisk->bootSector->bytes_per_sector,pdisk->volume->fat_table_in_bytes,pdisk->volume->fat_size);
                if(return_0!=0){
                    disk_close(pdisk);
                    printf("ERROR CODE: %d\n", ENOMEM);
                    return NULL;
                }
                //display_folder(dir);
            } else if (entry->is_directory == 0) {
                if(pdisk->volume->main_dir->amount_files==0){
                    pdisk->volume->main_dir->files = (struct file_t **) malloc(sizeof(struct file_t*));
                    if(pdisk->volume->main_dir->files==NULL){
                        disk_close(pdisk);
                        printf("ERROR CODE: %d\n", ENOMEM);
                        return NULL;
                    }
                    pdisk->volume->main_dir->amount_files+=1;
                }
                else{
                    struct file_t ** buffor = (struct file_t **) realloc(pdisk->volume->main_dir->files, sizeof(struct file_t*)*
                                                                                                         (pdisk->volume->main_dir->amount_files+1));
                    if(buffor==NULL){
                        disk_close(pdisk);
                        printf("ERROR CODE: %d\n", ENOMEM);
                        return NULL;
                    }
                    pdisk->volume->main_dir->files=buffor;
                    pdisk->volume->main_dir->amount_files+=1;
                }
                struct file_t * file = (struct file_t*) malloc(sizeof(struct file_t));
                if(file==NULL){
                    disk_close(pdisk);
                    printf("ERROR CODE: %d\n", ENOMEM);
                    return NULL;
                }
                file->entry=entry;
                file->f=NULL;
                file->cluser_size=pdisk->bootSector->sector_per_cluster*pdisk->bootSector->bytes_per_sector;
                file->clusters = get_chain_fat12(pdisk->volume->fat_table_in_bytes,pdisk->volume->fat_size,entry->first_cluster);
                *(pdisk->volume->main_dir->files+pdisk->volume->main_dir->amount_files-1)=file;
            }
        }
    }
    return pdisk->volume;
}

struct clusters_chain_t *get_chain_fat12(const void * const buffer, size_t size, uint16_t first_cluster){
    struct clusters_chain_t *chain = (struct clusters_chain_t*) malloc(sizeof(struct clusters_chain_t));
    if(chain==NULL){
        return NULL;
    }
    chain->size=size;
    chain->size-=size;
    chain->clusters = (uint16_t *) malloc(chain->size* sizeof(uint16_t));
    uint8_t * ptr = (uint8_t*)buffer;
    /*for(int i=0;i<(int)size;i++){
        printf("%d - %d\n",*(ptr+i),i);
    }*/
    uint16_t old_cluster = first_cluster;
    uint16_t current_cluster = first_cluster;
    uint16_t current_pos = (first_cluster*3)/2;
    while (current_cluster<=4088){
        uint16_t * buffor = (uint16_t*) realloc(chain->clusters,(chain->size+1)*sizeof(uint16_t));
        chain->clusters=buffor;
        if(current_cluster%2==0){
            uint8_t first = *(ptr+current_pos)%16;
            uint8_t second = *(ptr+current_pos)/16;
            uint8_t third = *(ptr+current_pos+1)%16;

            current_cluster = third*16*16+second*16+first;
        }
        else{
            current_pos+=1;
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

struct dir_entry_t *read_directory_entry(FILE * f){
    char input = (char)fgetc(f);
    if(!(input>='A'&&input<='Z')){
        fseek(f,31,SEEK_CUR);
        return NULL;
    }
    fseek(f,-1,SEEK_CUR);
    struct dir_entry_t * entry = (struct dir_entry_t *)malloc(sizeof(struct dir_entry_t));
    for(int i=0;i<12;i++){
        entry->name[i]='\0';
    }
    int name_size=0;
    for(int i=0;i<8;i++){
        input = (char)fgetc(f);
        if(input!=' '){
            entry->name[i]=input;
        }
        else {
            entry->name[i]='.';
            for(int j=i;j<7;j++){
                fgetc(f);
            }
            name_size+=1;
            break;
        }
        name_size+=1;
    }
    if(entry->name[name_size-1]!='.'){
        entry->name[name_size]='.';
        name_size+=1;
    }
    for(int i=0;i<3;i++){
        input=(char) fgetc(f);
        if(input>='A' &&input<='Z'){
            entry->name[name_size+i]=input;
        }
        else {
            if(i==0){
                entry->name[name_size-1]='\0';
                fseek(f,2,SEEK_CUR);
            }
            break;
        }
    }
    entry->name[12]='\0';
    int bytes_attributes = fgetc(f);
    int bits[16]={0};
    for(int i=0;i<16;i++){
        bits[i]=bytes_attributes%2;
        bytes_attributes/=2;
        //printf("%d\n",bits[i]);
    }
    entry->is_readonly=bits[0];
    entry->is_hidden=bits[1];
    entry->is_system=bits[2];
    entry->is_directory=bits[4];
    entry->is_archived=bits[5];
    input= (char)fgetc(f);
    input= (char)fgetc(f);
    uint16_t creations_time;
    fread(&creations_time, sizeof(uint16_t),1,f);
    int bits_time[16]={0};
    for(int i=0;i<16;i++){
        bits_time[i]=creations_time%2;
        creations_time/=2;
    }
    entry->creation_time_second=bits_time[4]*16+bits_time[3]*8+bits_time[2]*4+bits_time[1]*2+bits_time[0];
    entry->creation_time_minute=bits_time[10]*32+bits_time[9]*16+bits_time[8]*8+bits_time[7]*4+bits_time[6]*2+bits_time[5];
    entry->creation_time_hour=bits_time[15]*16+bits_time[14]*8+bits_time[13]*4+bits_time[12]*2+bits_time[11];
    uint16_t creations_date;
    fread(&creations_date, sizeof(uint16_t),1,f);
    int bits_date[16]={0};
    for(int i=0;i<16;i++){
        bits_date[i]=creations_date%2;
        creations_date/=2;
    }
    entry->creation_date_day= bits_date[4]*16+bits_date[3]*8+bits_date[2]*4+bits_date[1]*2+bits_date[0];
    entry->creation_date_mount = bits_date[8]*8+bits_date[7]*4+bits_date[6]*2+bits_date[5];
    entry->creation_date_year =bits_date[15]*64+bits_date[14]*32+bits_date[13]*16+bits_date[12]*8+bits_date[11]*4+bits_date[10]*2+bits_date[9]+1980;
    fgetc(f);//18
    fgetc(f);//19
    uint8_t cluster_parts[4]={0};
    cluster_parts[2]= fgetc(f);
    cluster_parts[3]= fgetc(f);
    for(int i=0;i<4;i++){
        fgetc(f);
    }
    cluster_parts[0]= fgetc(f);
    cluster_parts[1]= fgetc(f);
    entry->first_cluster = cluster_parts[3]*256*256*256+cluster_parts[2]*256*256+cluster_parts[1]*256+cluster_parts[0];
    int size;
    fread(&size, sizeof(int),1,f);
    entry->size=(size_t)size;
    //display_entry(entry);
    return entry;
}

void display_entry(struct dir_entry_t * entry){
    if(entry->is_directory==0) {
        printf("%02d/%02d/%04d %02d:%02d   %5d   %11zu  %s\n", entry->creation_date_day, entry->creation_date_mount,
               entry->creation_date_year, entry->creation_time_hour, entry->creation_time_minute,entry->first_cluster,entry->size,entry->name);
    }
    else{
        printf("%02d/%02d/%04d %02d:%02d   %5d   *DIRECTORY*  %s\n", entry->creation_date_day, entry->creation_date_mount,
               entry->creation_date_year, entry->creation_time_hour, entry->creation_time_minute,entry->first_cluster,entry->name);
    }
}

int read_directory(struct dir_t * dir, FILE * f, int cluster_size,uint8_t * fat_table,size_t fat_size){
    for(size_t i=0;i< dir->clusters->size;i++){
        for(int j=0;j<(cluster_size/SIZE_OF_ENTRY);j++){
            fseek(f,*(dir->clusters->clusters+i)*cluster_size+j*SIZE_OF_ENTRY+dir->entry->off_set,SEEK_SET);
            struct dir_entry_t * entry = read_directory_entry(f);
            if(entry!=NULL) {
                entry->off_set = dir->entry->off_set;
                if (entry->is_directory == 1) {
                    if (dir->amount_folders == 0) {
                        dir->folders = (struct dir_t **) malloc(sizeof(struct dir_t *));
                        if (dir->folders == NULL) {
                            return -1;
                        }
                        dir->amount_folders = 1;
                    } else {
                        struct dir_t **buffor = (struct dir_t **) realloc( dir->folders,
                                                                           sizeof(struct dir_t *) * (dir->amount_folders + 1));
                        if (buffor == NULL) {
                            return -1;
                        }
                        dir->folders = buffor;
                        dir->amount_folders += 1;
                    }
                    struct dir_t *dir_l = (struct dir_t *) malloc(sizeof(struct dir_t));
                    if (dir_l == NULL) {
                        return -1;
                    }
                    dir_l->entry = entry;
                    dir_l->folders = NULL;
                    dir_l->files = NULL;
                    dir_l->amount_folders=0;
                    dir_l->amount_files=0;
                    dir_l->clusters = get_chain_fat12(fat_table, fat_size, entry->first_cluster);
                    *(dir->folders+dir->amount_folders-1)=dir_l;
                    int return_0 = read_directory(dir_l,f,cluster_size,fat_table,fat_size);
                    if(return_0!=0){
                        return -1;
                    }
                    //display_folder(dir_l);
                } else if (entry->is_directory == 0) {
                    if(dir->amount_files==0){
                        dir->files = (struct file_t **) malloc(sizeof(struct file_t*));
                        if(dir->files==NULL){
                            return -1;
                        }
                        dir->amount_files+=1;
                    }
                    else{
                        struct file_t ** buffor = (struct file_t **) realloc(dir->files, sizeof(struct file_t*)*
                                                                                         (dir->amount_files+1));
                        if(buffor==NULL){
                            return -1;
                        }
                        dir->files=buffor;
                        dir->amount_files+=1;
                    }
                    struct file_t * file = (struct file_t*) malloc(sizeof(struct file_t));
                    if(file==NULL){
                        return -1;
                    }
                    file->entry=entry;
                    file->f=NULL;
                    file->cluser_size=cluster_size;
                    file->clusters = get_chain_fat12(fat_table, fat_size, entry->first_cluster);
                    *(dir->files+dir->amount_files-1)=file;
                }
            }
        }
    }
    return 0;
}

void display_folder(struct dir_t * dir){
    printf("#########################################################################################\n");
    if(dir->entry!=NULL) {
        printf("%s\n", dir->entry->name);
    }
    printf("FOLDERS:\n");
    if(dir->amount_folders!=0) {
        for (int i = 0; i < dir->amount_folders; i++) {
            printf("%s\n", (*(dir->folders + i))->entry->name);
        }
    }
    printf("FILES:\n");
    if(dir->amount_files!=0) {
        for (int i = 0; i < dir->amount_files; i++) {
            printf("%s\n", (*(dir->files + i))->entry->name);
        }
    }
    printf("#########################################################################################\n");
}

int fat_close(struct volume_t* pvolume){
    if(pvolume==NULL){
        printf("ERROR CODE: %d\n", ENOMEM);
        return -1;
    }
    if(pvolume->main_dir->deleted==0) {
        dir_close(pvolume->main_dir);
    }
    free(pvolume->main_dir);
    free(pvolume->fat_table_in_bytes);
    return 0;
}

struct file_t* file_open(struct volume_t* pvolume, const char* file_name){
    if(pvolume==NULL || file_name==NULL){
        printf("ERROR CODE: %d\n", EFAULT);
        return NULL;
    }
    int error_code;
    struct file_t * file = find_file(pvolume->main_dir,file_name,&error_code);
    if(error_code==-1){
        printf("BRAK TAKIEGO PLIKU!!!\n");
        printf("ERROR CODE: %d\n", ENOENT);
        return NULL;
    }
    else if(error_code==-2){
        printf("TO JEST FOLDER!!!\n");
        printf("ERROR CODE: %d\n", EISDIR);
        return NULL;
    }
    file->f = fopen(pvolume->disc_name,"rb");
    fseek(file->f,file->entry->off_set,SEEK_SET);
    file->pos_in_file=0;
    return file;
}

// 0 wszystko git
// -1 brak takiego pliku
// -2 to jest katalog

struct file_t* find_file(struct dir_t * dir, const char * file_name, int* error_code){
    *error_code=0;
    for(int i=0;i<dir->amount_files;i++){
        if(strcmp((*(dir->files+i))->entry->name,file_name)==0){
            return *(dir->files+i);
        }
    }
    for(int i=0;i<dir->amount_folders;i++){
        if(strcmp((*(dir->folders+i))->entry->name,file_name)==0){
            *error_code = -2;
            return NULL;
        }
    }
    struct file_t * to_return = NULL;
    int error;
    for(int i=0;i<dir->amount_folders;i++){
        to_return = find_file(*(dir->folders+i),file_name,&error);
        if(error==0){
            *error_code=error;
            return to_return;
        }
        if(error==-2){
            *error_code=error;
            return NULL;
        }
    }
    *error_code=-1;
    return NULL;
}


void file_delete(struct file_t * file){
    free(file->entry);
    free(file->clusters->clusters);
    free(file->clusters);
    free(file);
}

int file_close(struct file_t* stream){
    if(stream==NULL){
        printf("ERROR CODE: %d\n", EFAULT);
        return -1;
    }
    fclose(stream->f);
    return 0;
}

size_t file_read(void *ptr, size_t size, size_t nmemb, struct file_t *stream){
    if(ptr==NULL || size<=0 || nmemb<=0 || stream==NULL){
        return -1;
    }
    char * ptr_char = (char *)ptr;
    size_t readed = 0;
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<nmemb;j++){
            if(stream->pos_in_file>=stream->entry->size){
                if(size==1) {
                    return readed;
                }
                return j;
            }
            readed+=1;
            char c = (char )fgetc(stream->f);
            printf("%c",c);
            *(ptr_char+j+i*nmemb)= c;
            stream->pos_in_file+=1;
            if(stream->pos_in_file%stream->cluser_size==0){
                fseek(stream->f,*(stream->clusters->clusters+stream->pos_in_file/stream->cluser_size)*stream->cluser_size+ stream->entry->off_set,SEEK_SET);
            }

        }
    }
    if(size==1){
        return readed;
    }
    return nmemb;
}


int32_t file_seek(struct file_t* stream, int32_t offset, int whence){
    if(stream==NULL || !(whence==SEEK_SET || whence==SEEK_CUR|| whence==SEEK_END)){
        printf("ERROR CODE: %d\n", EFAULT);
        return -1;
    }
    if(whence==SEEK_SET){
        if(offset>(int)stream->entry->size){
            return -1;
        }
        int cluster_after = offset/stream->cluser_size;
        fseek(stream->f,*(stream->clusters->clusters+cluster_after)*stream->cluser_size+stream->entry->off_set+offset%stream->cluser_size,SEEK_SET);
        stream->pos_in_file = offset;
    }
    else if(whence==SEEK_END){
        if(-offset>(int)stream->entry->size){
            return -1;
        }
        return file_seek(stream,(int32_t)stream->entry->size+offset,SEEK_SET);
    }
    else if(whence==SEEK_CUR){
        if(!((int)stream->pos_in_file+offset>=0 && (int)stream->pos_in_file+offset<=(int)stream->entry->size)){
            return -1;
        }
        return file_seek(stream,(int)stream->pos_in_file+offset,SEEK_SET);
    }
    return 0;
}

struct dir_t* find_dir(struct dir_t * dir, const char * file_name){
    for(int i=0;i<dir->amount_folders;i++){
        if(strcmp((*(dir->folders+i))->entry->name,file_name)==0){
            return *(dir->folders+i);
        }
    }
    for(int i=0;i<dir->amount_folders;i++){
        return find_dir(*(dir->folders+i),file_name);
    }
    return NULL;
}

struct dir_t* dir_open(struct volume_t* pvolume, const char* dir_path){
    if(strcmp(dir_path,"\\")!=0){
        return NULL;
    }
    return pvolume->main_dir;
}

int reader_entries = 0;

int dir_read(struct dir_t* pdir, struct dir_entry_t* pentry){
    if(reader_entries<pdir->amount_folders){
        *pentry = *(*(pdir->folders+reader_entries))->entry;
    }
    else {
        int pos = reader_entries-pdir->amount_folders;
        if(pos>=pdir->amount_files){
            return 1;
        }
        *pentry = *(*(pdir->files+pos))->entry;
    }
    reader_entries+=1;
    return 0;
}

int dir_close(struct dir_t* pdir){
    for(int i=0;i<pdir->amount_files;i++){
        file_delete(*(pdir->files+i));
    }
    free(pdir->files);
    if(pdir->clusters!=NULL) {
        free(pdir->clusters->clusters);
        free(pdir->clusters);
    }
    if(pdir->entry!=NULL) {
        free(pdir->entry);
    }
    for(int i=0;i<pdir->amount_folders;i++){
        dir_close(*(pdir->folders+i));
        free(*(pdir->folders+i));
    }
    free(pdir->folders);
    pdir->deleted=1;
    return 0;
}
