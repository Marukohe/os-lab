#include "sperf.h"

int main(int argc, char *argv[]) {

    /*printf("%d\n",argc);*/
    /*printf("%s\n",argv[0]);*/
    /*Logy("hello");*/
    int childpid;
    /*int i;*/
    childpid = fork();
    if(childpid == 0){
        char * execv_str[] = {"echo", "executed by execv", NULL};
        if(execv("/bin/echo", execv_str) < 0){
            exit(0);
        }
    }else{
        wait(&childpid);
        printf("hello\n");
    }
  return 0;
}
