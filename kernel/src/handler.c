#include <common.h>
#include <klib.h>

#define TKNUM 20
extern spinlock_t pk;
extern int tottask;
extern task_t task[TKNUM];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

_Context *kmt_context_save(_Event ev, _Context *context){
    kmt->spin_lock(&pk);
    /*assert(0);*/
    kmt->spin_unlock(&pk);
    if(current) current->context = *context;
    return &current->context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
    kmt->spin_lock(&pk);
    assert(0);
    kmt->spin_unlock(&pk);
    assert(!current || current->state == RUNNING);
    int tmp = current->id;
    do{
        if(!current || current->id == tottask - 1){
            current = &task[0];
        }else{
            current = &task[++current->id];
        }
    }while(current->state != FREET);
    task[tmp].state = FREET;
    current->state = RUNNING;
    return &current->context;
}

