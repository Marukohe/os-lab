#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dlfcn.h>
#define maxlen 1000
/*#define MODE 775*/
char s[maxlen];

int main(int argc, char *argv[]) {
    char *path = "./tmpc";
    int cnt = 0;
    /*assert(mkdir(path, 0777) == 0);*/
    int a = mkdir(path, 0777);
    /*printf("%d\n", a);*/
    assert(a == 0);
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    printf(">> ");
    while(fgets(s, maxlen,stdin)){
        if(strncmp(s, "quit()", 6) == 0){
            /*rmdir(path);*/
            break;
        }
        if(strncmp(s, "int", 3) == 0){
            printf(">> add function successfully!\n");
            printf(">> ");
            cnt++;
            char fpath[1000];
            char num[1000];
            sprintf(num, "%d", cnt);
            strcpy(fpath, "./tmpc/");
            strcat(fpath, num);
            strcat(fpath, ".c");
            FILE *fp = fopen(fpath, "wb");
            fwrite(s, sizeof(char), strlen(s), fp);
            fclose(fp);
            system("gcc -fPIC -shared ./tmpc/1.c -o ./tmpc/1.so");
            /*printf("%s\n", fpath);*/
            /*printf(">> ");*/
        }else{
            printf(">> hello, world\n");
            printf(">> ");
        }
    }
  return 0;
}
