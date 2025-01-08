#include "directory_reader.h"


struct dir_entry_t *read_directory_entry(const char *filename){
    if(filename==NULL && feof(f)!=0){
        fclose(f);
        return NULL;
    }
    if(filename!=NULL){
        f = fopen(filename,"rb");
    }
    else {
        fseek(f,-1, SEEK_CUR);
    }
    char input = (char)fgetc(f);
    if(input=='\0'){
        fclose(f);
        return NULL;
    }
    while (!(input>='A'&&input<='Z')){
        fseek(f,31,SEEK_CUR);
        input= (char)fgetc(f);
        if(feof(f)!=0 || input=='\0'){
            fclose(f);
            return NULL;
        }
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
    entry->first_cluster = cluster_parts[3]*16*16*16+cluster_parts[2]*16*16+cluster_parts[1]*16+cluster_parts[0];
    int size;
    fread(&size, sizeof(int),1,f);
    entry->size=(size_t)size;
    input=(char)fgetc(f);
    //fclose(f);
    return entry;
}


