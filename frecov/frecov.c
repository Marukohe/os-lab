#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#define namesize 1000
#define bmpsize 10000000
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

// #define OUTPUTFILE

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
    // uint64_t filename;
    uint8_t filename[8];
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

    // printf("arvg[1] %s\n", argv[1]);
    int fd = open(argv[1], O_RDWR|O_CREAT, 0666);
    if(fd == -1)
        handle_error("open");
    unsigned long fsize = get_file_size(argv[1]);
    startaddr = mmap(NULL, fsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    fatstruct = (FATstruct *)(startaddr + 0xB);

    SizeofCluster = (uint32_t)fatstruct->SectorsPerCluster * (uint32_t)fatstruct->BytesPerSector;
    uintptr_t RootCluster = 0;
    RootCluster += (uintptr_t)fatstruct->ReservedSector * (uintptr_t)fatstruct->BytesPerSector;
    RootCluster += (uintptr_t)fatstruct->SectorsPerFat * (uintptr_t)fatstruct->BytesPerSector;
    RootCluster += (uintptr_t)fatstruct->SectorsPerFat * (uintptr_t)fatstruct->BytesPerSector;
    // printf("%lx\n", (unsigned long)RootCluster);


    uint32_t startsearchcluster = SizeofCluster * 3;
    // printf("0x%x\n", SizeofCluster * 2);


    uintptr_t searchaddr = (uintptr_t)startaddr;

    startsearchcluster = RootCluster;
    while(startsearchcluster < fsize){
        shortdic * sdic;
        char filename[namesize];    //文件名
        memset(filename, 0, sizeof(filename));
        int fileoffset = 0;
        char extendname[4];         //扩展名
        uint8_t flag = 1;            //文件是否连续
        sdic = (shortdic *)(startsearchcluster + searchaddr);
        for(int i = 0; i < 3; i++)
            extendname[i] = sdic->extendname[i];
        if(strncmp(extendname, "BMP", 3) == 0){
            // for(int i = 0; i < 8; i++){
            //     filename[fileoffset++] = sdic->filename[i];
            // }
            // for(int i = 0; i < 3; i++){
            //     filename[fileoffset++] = sdic->extendname[i];
            // }
            // printf("%s\n", filename);
            int cnt = 1;
            uintptr_t tmpaddr = startsearchcluster + searchaddr - 0x20 * (cnt);
            longdic *ldic = (longdic *)(tmpaddr);
            // if(ldic->flag != 0xF)
            //     continue;

            while((ldic->attribute & 0x40) == 0){
                if((ldic->attribute & 0x1F) != cnt){
                    flag = 0;
                    break;
                }

                for(int i = 0; i < 5; i++){
                    if(ldic->unicode1[2 * i] != 0xFF)
                        filename[fileoffset++] = ldic->unicode1[2 * i];
                }
                for(int i = 0; i < 6; i++){
                    if(ldic->unicode2[2 * i] != 0xFF)
                        filename[fileoffset++] = ldic->unicode2[2 * i];
                }
                for(int i = 0; i < 2; i++){
                    if(ldic->unicode3[2 * i] != 0xFF)
                        filename[fileoffset++] = ldic->unicode3[2 * i];
                }
                cnt ++;
                tmpaddr = startsearchcluster + searchaddr - 0x20 * (cnt);
                ldic = (longdic *)(tmpaddr);
            }
            for(int i = 0; i < 5; i++){
                if(ldic->unicode1[2 * i] != 0xFF)
                    filename[fileoffset++] = ldic->unicode1[2 * i];
            }
            for(int i = 0; i < 6; i++){
                if(ldic->unicode2[2 * i] != 0xFF)
                    filename[fileoffset++] = ldic->unicode2[2 * i];
            }
            for(int i = 0; i < 2; i++){
                if(ldic->unicode3[2 * i] != 0xFF)
                    filename[fileoffset++] = ldic->unicode3[2 * i];
            }
            if(flag && ldic->flag == 0xF && ldic->attribute != 0xE5){
                 printf("%s\n", filename);

                // printf("%x %x\n", sdic->highCluster, sdic->lowCluster);
                uintptr_t bmpstart = (((uintptr_t)sdic->highCluster) << 16) + (uintptr_t)sdic->lowCluster - fatstruct->RootClusterNumber;
                // printf("%lx\n", (unsigned long)(bmpstart * SizeofCluster + RootCluster));
                BITMAPFILEHEADER * bmp = (BITMAPFILEHEADER *)(bmpstart * SizeofCluster + RootCluster + searchaddr);

                // printf("bmpsize %lx\n", (unsigned long)(bmp->bfSize));
                void * tmpfile = (void *)(bmpstart * SizeofCluster + RootCluster + searchaddr);
                printf("%lx\n", (unsigned long)bmpstart * SizeofCluster + RootCluster);
                // printf("%x\n", *(uint8_t*)(tmpfile));
                #ifdef OUTPUTFILE
                FILE *fp = fopen(filename, "w+");
                fwrite(tmpfile, sizeof(uint8_t), bmp->bfSize, fp);
                fclose(fp);
                #endif

                char cmd[1000] = "sha1sum ";
                FILE *fp = fopen(filename, "w+");
                fwrite(tmpfile, sizeof(uint8_t), bmp->bfSize, fp);
                strcat(cmd, filename);
                system(cmd);
                /*unlink(filename);*/
                fclose(fp);

                // int pipefds[2];
                // int pipefds1[2];
                // if(pipe(pipefds) == -1){
                //     handle_error("pipe");
                // }
                // if(pipe(pipefds1) == -1){
                //     handle_error("pipe1");
                // }
                // int childpid = fork();
                // if(childpid == 0){
                //     char * execv_str[] = {"sha1sum", NULL};
                //     close(STDIN_FILENO);
                //     dup2(pipefds1[0], STDIN_FILENO);
                //     close(pipefds1[1]);

                //     close(STDOUT_FILENO);
                //     // char r_buf[10000];
                //     // FILE *fpout = fdopen(STDIN_FILENO, "r");
                //     // while(fgets(r_buf, 1000, fpout) != NULL){
                //     //     printf("%s\n", r_buf);
                //     // }
                //     dup2(pipefds[1], STDOUT_FILENO);
                //     close(pipefds[0]);

                //     if(execv("/usr/bin/sha1sum", execv_str) < 0){
                //         handle_error("execve");
                //     }
                // }else{
                //     dup2(pipefds1[1], STDOUT_FILENO);
                //     close(pipefds1[0]);
                //     // open(STDOUT_FILENO);
                //     fwrite(tmpfile, sizeof(uint8_t), bmp->bfSize, stdout);
                //     // write(pipefds1[1], tmpfile, bmp->bfSize);
                //     // wait(&childpid);
                //     dup2(pipefds[0], STDIN_FILENO);
                //     close(pipefds[1]);

                //     char buf[1000];
                //     fread(buf, sizeof(char), 100, stdin);
                //     printf("%s\n", buf);
                // }
            }
        }
        startsearchcluster += 0x10;
    }

    munmap(startaddr, fsize);
    close(fd);
  return 0;
}
