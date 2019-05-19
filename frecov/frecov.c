#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

//Fat32文件系统结构索引
typedef struct tagFATstruct{
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSector;
    uint8_t NumberofFAT;
    uint16_t RootEntries;
    uint16_t SmallSector;
    uint8_t MediaDes;
    uint16_t SectorsPerFat16;
    uint16_t SectorsPerTrack;
    uint16_t NumberofHead;
    uint32_t HiddenSector;
    uint32_t LargeSector;
    uint32_t SectorsPerFat;
    uint16_t ExtendFlag;
    uint16_t FilesystemVersion;
    uint32_t RootClusterNumber;
}__attribute__((packed)) FATstruct;

FATstruct *fatstruct;

typedef struct tagBITMAPFILEHEADER
{
    uint16_t bfType;//位图文件的类型，必须为BM(1-2字节）
    uint32_t bfSize;//位图文件的大小，以字节为单位（3-6字节，低位在前）
    uint16_t bfReserved1;//位图文件保留字，必须为0(7-8字节）
    uint16_t bfReserved2;//位图文件保留字，必须为0(9-10字节）
    uint32_t bfOffBits;//位图数据的起始位置，以相对于位图（11-14字节，低位在前）
    //文件头的偏移量表示，以字节为单位
}__attribute__((packed)) BITMAPFILEHEADER;

unsigned long get_file_size(const char *path){
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0)
        return filesize;
    else
        filesize = statbuff.st_size;
    return filesize;
}

uint32_t SizeofCluster = 0;

int main(int argc, char *argv[]) {
    if(argc == 1)
        argv[1] = "fs.img";
    uint8_t *startaddr;
    printf("arvg[1] %s\n", argv[1]);
    int fd = open(argv[1], O_RDWR|O_CREAT, 0666);
    if(fd == -1)
        handle_error("open");
    unsigned long fsize = get_file_size(argv[1]);
    startaddr = mmap(NULL, fsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    fatstruct = (FATstruct *)(startaddr + 0xB);

    SizeofCluster = (uint32_t)fatstruct->SectorsPerCluster * (uint32_t)fatstruct->BytesPerSector;

    printf("0x%x\n", SizeofCluster * 2);

    // printf("0x%x\n", fatstruct->BytesPerSector);
    // printf("0x%x\n", fatstruct->SectorsPerCluster);
    // printf("0x%x\n", fatstruct->ReservedSector);
    // printf("0x%x\n", fatstruct->NumberofFAT);
    // printf("0x%x\n", fatstruct->RootEntries);
    // printf("0x%x\n", fatstruct->SmallSector);
    // printf("0x%x\n", fatstruct->MediaDes);
    // printf("0x%x\n", fatstruct->SectorsPerFat16);
    // printf("0x%x\n", fatstruct->SectorsPerTrack);
    // printf("0x%x\n", fatstruct->NumberofHead);
    // printf("0x%x\n", fatstruct->HiddenSector);
    // printf("0x%x\n", fatstruct->LargeSector);
    // printf("0x%x\n", fatstruct->SectorsPerTrack);
    munmap(startaddr, fsize);
    close(fd);
  return 0;
}
