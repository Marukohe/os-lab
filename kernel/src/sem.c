#include <common.h>
#include <klib.h>
#define TKNUM 25

/*#define SEMDEBUG*/
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
    /*kmt->spin_lock(&pk);*/
    /*Logq("in sem_wait, the name of sem: %s, the value: %d", sem->name, sem->count);*/
    /*Logq("in sem_wait %d %s", current->id, current->name);*/
    /*kmt->spin_unlock(&pk);*/
    kmt->spin_lock(&sem->locked);
    /*int flag = 0;*/
    /*sem->count--;*/
    while(sem->count <= 0){
        current->state = WAITING;
#ifdef SEMDEBUG
        kmt->spin_lock(&pk);
        Logq("in sem_wait %d %s", current->id, current->name);
        kmt->spin_unlock(&pk);
#endif
        int flag = 0;
        for(int i = 0; i < sem->cntid; i++){
            if(sem->id[i] == current->id)
                flag = 1;
        }
        if(!flag)
            sem->id[sem->cntid++] = current->id;

#ifdef SEMDEBUG
        kmt->spin_lock(&pk);
        assert(sem->cntid < tottask);
        kmt->spin_unlock(&pk);
#endif

        kmt->spin_unlock(&sem->locked);
        /*kmt->spin_lock(&pk);*/
        assert(get_efl() & FL_IF);
        /*kmt->spin_unlock(&pk);*/
        _yield();
        kmt->spin_lock(&sem->locked);
        /*flag = 1;*/
    }
    sem->count--;
    kmt->spin_unlock(&sem->locked);
    /*if(flag)*/
        /*_yield();*/
    return;
}

void sem_signal(sem_t *sem){
    kmt->spin_lock(&pk);
    Logy("in sem_signal, the name of sem: %s, the value: %d", sem->name, sem->count);
    kmt->spin_unlock(&pk);
    kmt->spin_lock(&sem->locked);
    sem->count++;
    /*TODO(); wakeup*/
    /*task[sem->id].state = FREET;*/
    for(int i = 0; i < sem->cntid; i++){
        cputask[sem->id[i]]->state = FREET;
/*#ifdef SEMDEBUG*/
        kmt->spin_lock(&pk);
        printf("sem_signal, %s, %d", cputask[sem->id[i]]->name, cputask[sem->id[i]]->state);
        Logy("sem_signal, %s, %d", cputask[sem->id[i]]->name, cputask[sem->id[i]]->state);
        kmt->spin_unlock(&pk);
/*#endif*/
        sem->id[i] = -1;
    }
    sem->cntid = 0;
    kmt->spin_unlock(&sem->locked);
    return;
}
