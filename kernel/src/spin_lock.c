#include <common.h>
#include <klib.h>
#include <devices.h>

/*int spincnt[5]={};*/
/*int tmpcpu = -1;*/
/*int tmpcpu1 = -1;*/
int ncli[MAXCPU] = {};
int intena[MAXCPU] = {};

void spin_init(spinlock_t *lk, const char *name){
    lk->name = name;
    lk->locked = 0;
    lk->cpu = -1;
    return;
}

void spin_lock(spinlock_t *lk){
    /*cli();*/
    /*Logg("lk name %s, lk: %d", lk->name, lk->locked);*/
    pushcli();
    if(holding(lk)){
        printf("panic in lk locked %d, cpu %d, name, %s, %d\n", lk->locked, lk->cpu, lk->name, _cpu());
        panic("acquire");
    }
    while(_atomic_xchg(&lk->locked, 1));
    /*printf("spinlock %d\n",_cpu());*/
    __sync_synchronize();
    /*tmpcpu = _cpu();*/
    /*tmpcpu1 = _cpu();*/
    lk->cpu = _cpu();
}

void spin_unlock(spinlock_t *lk){
    if(!holding(lk)){
        printf("panic in lk locked %d, cpu %d name %s, %d\n", lk->locked, lk->cpu, lk->name, _cpu());
        panic("release");
    }
    lk->cpu = -1;
    /*_atomic_xchg(&lk->locked, 0);*/
    __sync_synchronize();
    _atomic_xchg(&lk->locked, 0);
    /*sti();*/
    popcli();
}

int holding(spinlock_t *lk){
    int r;
    pushcli();
    r = lk->locked && lk->cpu == _cpu();
    popcli();
    return r;
}

void pushcli(){
    uint32_t eflags;

    eflags = get_efl();
    cli();
    if(ncli[_cpu()] == 0)
        intena[_cpu()] = eflags & FL_IF;
    ncli[_cpu()]++;
    /*printf("push %d %d\n",_cpu(), spincnt[_cpu()]);*/
}

void popcli(){
    /*assert(spincnt[_cpu()]!=0);*/
    if(get_efl() & FL_IF)
        panic("popcli - interrupialbe");
    if(--ncli[_cpu()] < 0)
        panic("popcli");
    if(ncli[_cpu()] == 0 && intena[_cpu()]){
        /*printf("unlock %d\n"),_cpu();*/
        sti();
    }
}

