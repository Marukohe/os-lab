#include "sperf.h"
#define MAXBUF 5000000
#define ITEM 5000
#define INF 100000000
_trace_item _pitems[ITEM];
int tot = 0;
int flag = 0;

//qsort the struct
int cmp(const void *a, const void *b){
    return (int)((((_trace_item *)b)->t - ((_trace_item *)a)->t)*INF);
}

int main(int argc, char *argv[]) {
    char * execv_str[] = {"strace", "-T", "-xx"};
    /*printf("argc : %d\n", argc);*/
    for(int i = 1; i < argc; i++){
        execv_str[i + 2] = argv[i];
        /*printf("%s\n", execv_str[i + 2]);*/
    }
    if(argc > 1)
        execv_str[2 + argc] = NULL;
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
        /*char * execv_str[] = {"strace", "-T", "-xx","ls", NULL};*/
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
        int tmpnum = -1;                      //record the position for some syscall;
        while(fgets(r_buf,MAXBUF,fp)!=NULL){
            char buf[300];
            char name[1000];
            memset(name, 0, sizeof(name));
            memset(buf, 0, sizeof(buf));
            /*printf("\n%s",r_buf);*/
            /*sscanf(r_buf, "%[^(]", _pitems[tot].name);*/
            sscanf(r_buf, "%*[^<]<%[^>]", buf);
            /*sscanf(r_buf, "%<[0-9]*\\.?[0-9]+>", buf);*/
            /*printf("%s\n", buf);*/
            double tmp  = 0.0;
            tmp = atof(buf);
            /*printf("tmp: %lf  flag: %d\n", tmp, flag);*/
            if(tmp > 0 && flag == 0){                     //this line have a time with no name before
                assert(tmpnum == -1);
                /*printf("hello tmp: %d\n",tmpnum);*/
                sscanf(r_buf, "%[^(]", name);
                for(int i = 0; i < tot; i++){
                    if(strcmp(_pitems[i].name, name) == 0){
                        /*printf("%s   %s\n", _pitems[i].name, name);*/
                        tmpnum = i;
                        break;
                    }
                }
                /*printf("itemname: %s name: %s tot: %d  tmpnum:  %d\n",_pitems[tmpnum].name, name, tot, tmpnum);*/
                if(tmpnum != -1){
                    _pitems[tmpnum].t += tmp;
                }
                else{
                    strcpy(_pitems[tot].name, name);
                    /*printf("%s %d\n",name, tot);*/
                    _pitems[tot].t = tmp;
                    tot++;
                }
                tmpnum = -1;
            }else if(tmp > 0 && flag ==1){                  //this line have a time with a name before
                _pitems[tmpnum].t += tmp;
                /*tot++;*/
                tmpnum = -1;
                flag = 0;
            }else if(tmp == 0 && flag == 0){                  //this line have no time but with a name
                sscanf(r_buf, "%[^(]", name);
                for(int i = 0; i < tot; i++){
                    if(strcmp(_pitems[i].name, name) == 0){
                        /*printf("%s   %s\n", _pitems[i].name, name);*/
                        tmpnum = i;
                        break;
                    }
                }
                if(tmpnum == -1){
                    strcpy(_pitems[tot].name, name);
                    tmpnum = tot;
                }
                flag = 1;
            }else if(tmp == 0 && flag == 1){

            }
            /*printf("%lf\n", tmp);*/
        }

        qsort(_pitems, tot, sizeof(_pitems[0]),cmp);
        double cntall = 0.0;
        for(int i = 0; i < tot; i++){
            cntall += _pitems[i].t;
        }
        for(int i = 0; i < tot; i++){
            printf("%s: %.2lf%%\n",_pitems[i].name, ((_pitems[i].t)/cntall)*100);
        }
        /*Logy("here");*/
        wait(&childpid);
        /*printf("hello\n");*/
    }
  return 0;
}
