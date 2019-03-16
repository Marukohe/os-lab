#include <stdio.h>
#include <ucontext.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "co.h"

struct co {
    int fin;  //0即将结束，1未结束
    int id;
    ucontext_t ctx;
    char names[200];
    func_t co_fun;
    void *arg;
    enum COSTATE state;
    uint8_t stack[DEFAULT_STACK_SIZE];
}__attribute__((aligned (16)));

struct co coroutines[MAX_CO];
struct co *current=NULL;
ucontext_t umain;
int max_co = 0;
int running_co = 0;
long long cnt_yield = 0;

void co_init() {
    for(int i=1;i<MAX_CO;i++){
        coroutines[i].state = FREE;
        coroutines[i].fin = 0;
    }
    //current = (struct co*)malloc(sizeof(struct co));
    //umain = (struct co *)malloc(sizeof(struct co));
    //umain->state = FREE;
    running_co = -1;
}

struct co* co_start(const char *name, func_t func, void *arg) {
    //printf("Y2\n");

    int tmp = max_co;
    int cnt=-1;
    for(int i=0;i<tmp;i++){
        if(coroutines[i].fin == 0){
            cnt = i;
            coroutines[i].state = FREE;
            coroutines[i]=coroutines[i+1];
            max_co--;
        }else{
            if(cnt!=-1)
                coroutines[cnt] = coroutines[i];
        }
    }
    printf("%d\n",max_co);
    int id = max_co++;
    running_co = id;
    coroutines[id].fin = 1;
    coroutines[id].id = id;
    coroutines[id].state = RUNNING;
    coroutines[id].co_fun = func;
    coroutines[id].arg = arg;
    getcontext(&(coroutines[id].ctx));

    coroutines[id].ctx.uc_stack.ss_sp = coroutines[id].stack;
    coroutines[id].ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    coroutines[id].ctx.uc_stack.ss_flags = 0;
    coroutines[id].ctx.uc_link = &umain;

    makecontext(&(coroutines[id].ctx),(void(*)(void))func,1,arg);
    //printf("makecontext\n");
    /*printf("Y2 is here\n");*/
    /*printf("%d\n",id);*/
    swapcontext(&umain,&(coroutines[id].ctx));
    /*printf("???????????????\n");*/
    running_co = -1;
    current = &(coroutines[id]);
    /*printf("xxxxxxxxxxxx\n");*/
    //func(arg); // Test #2 hangs
    return current;
}

/*
 * main to co running_co=-1
 * co1 to co2 running_co!=-1
 */
void co_yield() {
    /*
    if(cnt_yield==0){
        printf("here\n");
        //return;
        coroutines[running_co].state = READY;
        int id = running_co;
        running_co = -1;
        cnt_yield++;
        swapcontext(&(coroutines[id].ctx),&(umain->ctx));
    }*/
    if(running_co != -1 && max_co!=1){  //不在main 有多个协程
        cnt_yield++;
        struct co *t = &coroutines[running_co];
        t->state = READY;
        int id = rand()%max_co;
        while(coroutines[id].state == SUSPEND){
            id = rand()%max_co;
        }
        running_co = id;
        coroutines[id].state = RUNNING;
        swapcontext(&(t->ctx),&(coroutines[id].ctx));
    }else if(running_co!=-1 && max_co==1){
        cnt_yield++;
        //assert(0);
        struct co *t = &coroutines[running_co];
        t->state = READY;
        running_co = -1;
        /*printf("assert at here\n\n\n");*/
        swapcontext(&(t->ctx),&umain);
    } else{
        cnt_yield++;
        int id = rand()%max_co;
        while(coroutines[id].state==SUSPEND){
            id = rand()%max_co;
        }
        /*struct co *t = &corourines[running];*/
        running_co = id;
        coroutines[id].state = RUNNING;
        swapcontext(&umain,&(coroutines[id].ctx));
    }
}

void co_wait(struct co *thd) {
    if(running_co == -1){
        thd->fin = 0;
        running_co = thd->id;
        thd->state = RUNNING;
        /*umain->state = SUSPEND;*/
        swapcontext(&umain,&(thd->ctx));
    }else{
        thd->fin = 0;
        struct co *t = &coroutines[running_co];
        running_co = thd->id;
        t->state = SUSPEND;
        thd->state = RUNNING;
        /*printf("I'm here\n\n\n");*/
        swapcontext(&(t->ctx),&(thd->ctx));
    }
}

