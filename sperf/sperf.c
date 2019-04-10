#include "sperf.h"

int main(int argc, char *argv[]) {
    int pipefds[2];
    if(pipe(pipefds) == -1){
        exit(0);
    }
    int childpid;
    childpid = fork();
    if(childpid == 0){
        dup2(pipefds[1], STDOUT_FILENO);
        close(pipefds[0]);
        char * execv_str[] = {"echo", "executed by execv", NULL};
        if(execv("/bin/echo", execv_str) < 0){
            exit(0);
        }
    }else{
        Logy("here");
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[1]);
        execlp("less","less",NULL);
        wait(&childpid);
        printf("hello\n");
    }
  return 0;
}
