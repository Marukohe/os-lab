#include "sperf.h"
#define MAXBUF 50000
#define ITEM 500
#define INF 100000000
_trace_item _pitems[ITEM];
int tot = 0;

int cmp(const void *a, const void *b){
    return (int)((((_trace_item *)b)->t - ((_trace_item *)a)->t)*INF);
}

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
            char name[20];
            memset(name, 0, sizeof(name));
            memset(buf, 0, sizeof(buf));
            /*printf("%s",r_buf);*/
            sscanf(r_buf, "%[^(]", _pitems[tot].name);
            sscanf(r_buf, "%*[^<]<%[^>]", buf);
            /*sscanf(r_buf, "%<[0-9]*\\.?[0-9]+>", buf);*/
            /*printf("%s\n", buf);*/
            double tmp  = 0.0;
            tmp = atof(buf);
            if(tmp > 0){
                _pitems[tot].t = tmp;
                tot++;
            }
            /*printf("%lf\n", tmp);*/
        }

        qsort(_pitems, tot, sizeof(_pitems[0]),cmp);
        for(int i = 0; i < tot; i++){
            printf("%s %lf\n",_pitems[i].name, _pitems[i].t);
        }
        Logy("here");
        wait(&childpid);
        /*printf("hello\n");*/
    }
  return 0;
}
