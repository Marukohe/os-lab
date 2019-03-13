#include <stdio.h>
#include <ucontext.h>
#include <stdint.h>
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
struct co *current;
ucontext_t  umain;
int max_co = 0;
int running_co = 0;

void co_init() {
    for(int i=1;i<MAX_CO;i++){
        coroutines[i].state = FREE;
    }
    running_co = -1;
}

struct co* co_start(const char *name, func_t func, void *arg) {
    int id = max_co++;
    running_co = id;
    coroutines[id].id = id;
    coroutines[id].state = READY;
    coroutines[id].co_fun = func;
    coroutines[id].arg = arg;
    getcontext(&(coroutines[id].ctx));

    coroutines[id].ctx.uc_stack.ss_sp = coroutines[id].stack;
    coroutines[id].ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
    coroutines[id].ctx.uc_stack.ss_flags = 0;
    coroutines[id].ctx.uc_link = &umain;

    makecontext(&(coroutines[id].ctx),(void(*)(void))func,1,arg);
    swapcontext(&umain,&(coroutines[id].ctx));
    current = &(coroutines[id]);
    //func(arg); // Test #2 hangs
    return current;
}

void co_yield() {
    if(running_co != -1){
        struct co *t = &coroutines[running_co];
        coroutines[running_co].state = SUSPEND;
        running_co = -1;
        swapcontext(&(t->ctx),&umain);
    }
}

void co_wait(struct co *thd) {
    if(running_co != -1){
        running_co = thd->id;
        swapcontext(&umain,&(thd->ctx));
    }else{
        struct co *t = &coroutines[running_co];
        running_co = thd->id;
        t->state = SUSPEND;
        thd->state = READY;
        swapcontext(&(t->ctx),&(thd->ctx));
    }
}
