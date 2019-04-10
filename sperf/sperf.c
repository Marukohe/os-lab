#include "sperf.h"
#define MAXBUF 5000
int main(int argc, char *argv[]) {
    int pipefds[2];
    char r_buf[MAXBUF];
    if(pipe(pipefds) == -1){
        exit(0);
    }
    int childpid;
    childpid = fork();
    if(childpid == 0){
        close(STDOUT_FILENO);
        /*close(STDERR_FILENO);*/
        close(pipefds[0]);
        dup2(pipefds[1], STDERR_FILENO);
        close(pipefds[1]);
        char * execv_str[] = {"strace", "ls", NULL};
        if(execv("/usr/bin/strace", execv_str) < 0){
            exit(0);
        }
    }else{
        Logy("here");
        close(pipefds[1]);
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[0]);
        FILE *fp = NULL;
        fp = fdopen(STDIN_FILENO, "r");
        /*read(STDIN_FILENO, r_buf, MAXBUF);*/
        while(fgets(r_buf,MAXBUF,fp)!=NULL){
            printf("%s\n",r_buf);
        }

        /*wait(&childpid);*/
        printf("hello\n");
    }
  return 0;
}
