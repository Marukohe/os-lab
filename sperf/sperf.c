#include "sperf.h"
#define MAXBUF 50000
#define ITEM 500
_trace_item _pitems[ITEM];
int tot = 0;

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
        char * execv_str[] = {"strace", "-T", "-xx","ls", NULL};
        if(execv("/usr/bin/strace", execv_str) < 0){
            exit(0);
        }
    }else{
        /*Logy("here");*/
        close(pipefds[1]);
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[0]);
        FILE *fp = NULL;
        fp = fdopen(STDIN_FILENO, "r");
        while(fgets(r_buf,MAXBUF,fp)!=NULL){
            char buf[30];
            memset(buf, 0, sizeof(buf));
            printf("%s",r_buf);
            sscanf(r_buf, "%*[^<]<%[^>]", buf);
            /*sscanf(r_buf, "%<[0-9]*\\.?[0-9]+>", buf);*/
            printf("%s\n", buf);
            double tmp  = 0.0;
            tmp = atof(buf);
            printf("%lf\n", tmp);
        }

        wait(&childpid);
        /*printf("hello\n");*/
    }
  return 0;
}
