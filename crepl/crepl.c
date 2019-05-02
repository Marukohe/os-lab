#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#define maxlen 1000
char s[maxlen];

int main(int argc, char *argv[]) {
    printf(">> ");
    printf("Welcome to the C-shell, Please enter quit() to quit!\n");
    printf(">> ");
    while(fgets(s, maxlen,stdin)){
        if(strncmp(s, "quit()", 6) == 0)
            break;
        printf(">> hello, world\n");
        printf(">> ");
    }
  return 0;
}
