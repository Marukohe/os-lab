#include <common.h>
#include <klib.h>
/*#define HANDDEBUG*/

#define TKNUM 25
extern spinlock_t pk;
extern int tottask;
extern task_t *cputask[TKNUM];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

_Context *kmt_context_save(_Event ev, _Context *context){
    if(current) current->context = *context;
    return NULL;
}

int flag_switch(){
    for(int i = 0; i < 4; i++){
        if(i!=_cpu() && current_task[i]->id == current->id)
            return 1;
    }
    return 0;
}

_Context *kmt_context_switch(_Event ev, _Context *context){
    if(current && current->state != WAITING){
        current->state = FREET;
    }
    do{
        if(!current || current->id == tottask - 1){
            current = cputask[0];
        }else{
            current = cputask[current->id + 1];
        }
#ifdef HANDDEBUG
        kmt->spin_lock(&pk);
        printf("context switch current task: name-> %s id->%d state->%d CPU[%d]\n", current->name, current->id, current->state, _cpu());
        /*Logq("current task: name-> %s id->%d state->%d CPU[%d]", current->name, current->id, current->state, _cpu());*/
        kmt->spin_unlock(&pk);
#endif
        /*kmt->spin_lock(&pk);*/
        /*if(strncmp(current->name, "tty-task", 8) == 0){*/
            /*printf("context switch current task: name-> %s id->%d state->%d CPU[%d]\n", current->name, current->id, current->state, _cpu());*/
        /*}*/
        /*kmt->spin_unlock(&pk);*/

    }while(current->state != FREET || flag_switch());

    /*cputask[tmp]->state = FREET;*/
    /*kmt->spin_lock(&pk);*/
    /*if(strncmp(current->name, "tty-task", 8) == 0){*/
        /*printf("context switch current task: name-> %s id->%d state->%d CPU[%d]\n", current->name, current->id, current->state, _cpu());*/
    /*}*/
    /*kmt->spin_unlock(&pk);*/
    current->state = RUNNING;

    /*kmt->spin_lock(&pk);*/
    /*Logw("current task: name-> %s id->%d state->%d cpu: %d", current->name, current->id, current->state, _cpu());*/
    /*Logb("task0 %d, task1 %d, task2 %d, task3 %d", cputask[0]->state, cputask[1]->state, cputask[2]->state, cputask[3]->state);*/
    assert(current->fence1 == 0xcc && current->fence2 == 0xcc);
    assert(current != NULL);
    int tmp = 0;
    for(int i = 0 ; i < 4; i++){
        if(current->id == current_task[i]->id && current->state == current_task[i]->state){
            /*printf("current_task name %s, taskid %d\n", current_task[i]->name, current_task[i]->id);*/
            tmp++;
        }
    }
    assert(tmp == 1);
    /*kmt->spin_unlock(&pk);*/

    /*assert(get_efl() & FL_IF);*/
    return &current->context;
}

