#include <stdio.h>
#include <ucontext.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "co.h"

struct co {
    int prev, next;
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
struct co *umain=NULL;
int max_co = 0;
int running_co = 0;
long long cnt_yield = 0;

void co_init() {
    for(int i=1;i<MAX_CO;i++){
        coroutines[i].state = FREE;
    }
    current = (struct co*)malloc(sizeof(struct co));
    umain = (struct co *)malloc(sizeof(struct co));
    umain->state = FREE;
    running_co = -1;
}

struct co* co_start(const char *name, func_t func, void *arg) {
    int id = max_co++;
    running_co = id;
    coroutines[id].id = id;
    coroutines[id].state = RUNNING;
    coroutines[id].co_fun = func;
    coroutines[id].arg = arg;
    getcontext(&(coroutines[id].ctx));

    coroutines[id].ctx.uc_stack.ss_sp = coroutines[id].stack;
    coroutines[id].ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    coroutines[id].ctx.uc_stack.ss_flags = 0;
    coroutines[id].ctx.uc_link = &(umain->ctx);

    makecontext(&(coroutines[id].ctx),(void(*)(void))func,1,arg);
    //printf("makecontext\n");
    swapcontext(&(umain->ctx),&(coroutines[id].ctx));
    printf("???????????????\n");
    current = &(coroutines[id]);
    //func(arg); // Test #2 hangs
    return current;
}

/*
 * main to co running_co=-1
 * co1 to co2 running_co!=-1
 */
void co_yield() {
    if(cnt_yield==0){
        //printf("here\n");
        //return;
        coroutines[running_co].state = READY;
        int id = running_co;
        running_co = -1;
        cnt_yield++;
        swapcontext(&(coroutines[id].ctx),&(umain->ctx));
    }
    if(running_co != -1 && max_co!=1){  //不在main 有多个协程
        cnt_yield++;
        struct co *t = &coroutines[running_co];
        t->state = READY;
        int id = rand()%max_co + 1;
        while(coroutines[id].state == SUSPEND){
            id = rand()%max_co+1;
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
        //printf("assert at here\n\n\n");
        swapcontext(&(t->ctx),&(umain->ctx));
    } else{
        cnt_yield++;
        int id = rand()%max_co+1;
        while(coroutines[id].state==SUSPEND){
            id = rand()%max_co+1;
        }
        /*struct co *t = &corourines[running];*/
        running_co = id;
        coroutines[id].state = RUNNING;
        swapcontext(&(umain->ctx),&(coroutines[id].ctx));
    }
}

void co_wait(struct co *thd) {
    if(running_co == -1){
        running_co = thd->id;
        thd->state = RUNNING;
        umain->state = SUSPEND;
        swapcontext(&(umain->ctx),&(thd->ctx));
    }else{
        struct co *t = &coroutines[running_co];
        running_co = thd->id;
        t->state = SUSPEND;
        thd->state = RUNNING;
        swapcontext(&(t->ctx),&(thd->ctx));
    }
}

