#include <common.h>
#include <klib.h>

#define TKNUM 25
extern spinlock_t pk;
extern int tottask;
extern task_t *cputask[TKNUM];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

_Context *kmt_context_save(_Event ev, _Context *context){
    kmt->spin_lock(&pk);
    /*assert(0);*/
    Logp("kmtsave");
    kmt->spin_unlock(&pk);
    if(current) current->context = *context;
    if(!current) return context;
    return &current->context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
    kmt->spin_lock(&pk);
    /*assert(0);*/
    /*Logg("current task: name-> %s id->%d state->%d", current->name, current->id, current->state);*/
    kmt->spin_unlock(&pk);
    assert(!current || current->state == RUNNING);
    int tmp = current->id;
    do{
        if(!current || current->id == tottask - 1){
            current = cputask[0];
        }else{
            current = cputask[++current->id];
        }
    }while(current->state != FREET);
    cputask[tmp]->state = FREET;
    current->state = RUNNING;
    kmt->spin_lock(&pk);
    /*assert(0);*/
    Logw("current task: name-> %s id->%d state->%d", current->name, current->id, current->state);
    kmt->spin_unlock(&pk);
    return &current->context;
}

