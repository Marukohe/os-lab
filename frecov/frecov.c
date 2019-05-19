#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#define namesize 1000
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


typedef struct Shortdic{
    uint64_t filename;
    uint8_t extendname[3];
    uint8_t attribute;
    uint8_t dummy;
    uint8_t dummy1;
    uint16_t filecreattime;
    uint16_t filecreatdate;
    uint16_t fileaccessdate;
    uint16_t highCluster;
    uint16_t latestmodifytime;
    uint16_t latestmodiftdate;
    uint16_t lowCluster;
    uint32_t SizeofFile;
}__attribute__((packed)) shortdic;

shortdic * sdic;

typedef struct Longdic{
    uint8_t attribute;
    uint8_t unicode1[10];
    uint8_t flag;
    uint8_t dummy;
    uint8_t dummy1;
    uint8_t unicode2[12];
    uint16_t filestartcluster;
    uint8_t unicode3[4];
}__attribute__((packed)) longdic;

longdic * ldic;

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
    uint32_t startsearchcluster = SizeofCluster * 3;

    char filename[namesize];

    uint32_t searchoffset = startsearchcluster;
    while(searchoffset < fsize){
        ldic = (longdic *)((uintptr_t)startaddr + searchoffset);
        if(ldic->flag == 0xF){
            int filenameoffset = 0;
            // printf("Find one longdic\n");
            uint32_t tmpcntlongdic = ldic->attribute & 0xF;
            for(int i = tmpcntlongdic ; i >= 0; i--){
                uintptr_t tmp = searchoffset + i * 0x20;
                longdic * tmpdic = (longdic *)(tmp);
                if(i == tmpcntlongdic)
                    assert((tmpdic->attribute & 0xF) == 0);

                for(int k = 0; k < 10; k++){
                    if(tmpdic->unicode1[k] != 0xFF)
                        filename[filenameoffset++] = tmpdic->unicode1[k];
                }
                for(int k = 0; k < 12; k++){
                    if(tmpdic->unicode2[k] != 0xFF)
                        filename[filenameoffset++] = tmpdic->unicode2[k];
                }
                for(int k = 0; k <4; k++){
                    if(tmpdic->unicode3[k] != 0xFF)
                        filename[filenameoffset++] = tmpdic->unicode3[k];
                }
            }
            printf("%s\n", filename);
            searchoffset += 0x20 * (tmpcntlongdic + 1);
        }else{
            searchoffset += 0x10;
        }
    }

    // printf("0x%x\n", SizeofCluster * 2);
    munmap(startaddr, fsize);
    close(fd);
  return 0;
}
