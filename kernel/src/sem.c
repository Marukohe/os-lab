#include <common.h>
#include <klib.h>
#define TKNUM 25
extern spinlock_t pk;
extern int tottask;
extern task_t *cputask[TKNUM];
extern task_t *current_task[4];
#define current (current_task[_cpu()])

void sem_init(sem_t *sem, const char *name, int value){
    sem->name = name;
    sem->count = value;
    for(int i = 0; i < TKNUM; i++)
        sem->id[i] = -1;
    kmt->spin_init(&sem->locked, name);
    return;
}

void sem_wait(sem_t *sem){
    kmt->spin_lock(&pk);
    Logq("in sem_wait %d %s", current->id, current->name);
    kmt->spin_unlock(&pk);
    kmt->spin_lock(&sem->locked);
    int flag = 0;
    /*sem->count--;*/
    if(sem->count <= 0){
        current->state = WAITING;

        kmt->spin_lock(&pk);
        Logq("in sem_wait %d %s", current->id, current->name);
        kmt->spin_unlock(&pk);

        sem->id[sem->cntid++] = current->id;

        kmt->spin_lock(&pk);
        assert(sem->cntid < tottask);
        kmt->spin_unlock(&pk);

        /*_yield();*/
        flag = 1;
    }
    sem->count--;
    kmt->spin_unlock(&sem->locked);
    if(flag)
        _yield();
    return;
}

void sem_signal(sem_t *sem){
    kmt->spin_lock(&sem->locked);
    sem->count++;
    /*TODO(); wakeup*/
    /*task[sem->id].state = FREET;*/
    for(int i = 0; i < sem->cntid; i++){
        cputask[sem->id[i]]->state = FREET;
        kmt->spin_lock(&pk);
        Logq("sem_signal, %s, %d", cputask[sem->id[i]]->name, cputask[sem->id[i]]->state);
        kmt->spin_unlock(&pk);
        sem->id[i] = -1;
    }
    sem->cntid = 0;
    kmt->spin_unlock(&sem->locked);
    return;
}
