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
    Logg("current task: name-> %s id->%d state->%d CPU[%d]", current->name, current->id, current->state, _cpu());
    assert(!current || current->state != FREET);
    kmt->spin_unlock(&pk);
    /*assert(!current || current->state != FREET);*/
    int tmp = current->id;
    cputask[tmp]->state = FREET;
    do{
        if(!current || current->id == tottask - 1){
            current = cputask[0];
        }else{
            current = cputask[current->id + 1];
        }
        kmt->spin_lock(&pk);
        /*assert(0);*/
        Logq("current task: name-> %s id->%d state->%d CPU[%d]", current->name, current->id, current->state, _cpu());
        kmt->spin_unlock(&pk);
    }while(current->state != FREET);

    /*cputask[tmp]->state = FREET;*/
    current->state = RUNNING;

    kmt->spin_lock(&pk);
    /*assert(0);*/
    Logw("current task: name-> %s id->%d state->%d cpu: %d", current->name, current->id, current->state, _cpu());
    Logb("task0 %d, task1 %d, task2 %d, task3 %d", cputask[0]->state, cputask[1]->state, cputask[2]->state, cputask[3]->state);
    assert(current->fence1 == 0xcc && current->fence2 == 0xcc);
    kmt->spin_unlock(&pk);

    /*assert(get_efl() & FL_IF);*/
    return &current->context;
}

