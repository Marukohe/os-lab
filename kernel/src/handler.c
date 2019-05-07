#include <common.h>
#define TKNUM 20
extern int tottask;
extern task_t task[TKNUM];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

_Context *kmt_context_save(_Event ev, _Context *context){
    if(current) current->context = *context;
    return &current->context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
    assert(!current || current->state == RUNNING);
    int tmp = current->id;
    do{
        if(!current || current->id == tottask - 1){
            current = &task[0];
        }else{
            current = &task[++current->id];
        }
    }while(current->state != FREET);
    task[id].state = FREET;
    current->state = RUNNING;
    return &current->context;
}
