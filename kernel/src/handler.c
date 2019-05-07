#include <common.h>
extern tottask;
extern task_t task[TKNUM];
extern current_task[4];
#define current (current_task[_cpu()]);

_Context *kmt_context_save(_Event ev, _Context *context){
    current.context = *context;
    return &current.context;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
    if(current.id == tottask - 1){
        current.id = 0;
        current.context = task[0].context;;
    }else{
        current.context = task[++current.id].context;
    }
    return &current.context;
}
