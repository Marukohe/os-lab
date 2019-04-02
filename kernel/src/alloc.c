#include <common.h>
#include <klib.h>
#include <spinlock.h>
#define SMALLSIZE 256
#define CPUNUM 4
#define BLOCK 4096
#define STSIZE 24
#define ALIGNED(size) (((size>>3)+1)<<3)

typedef struct Kmem{
    uintptr_t start;
    size_t size;
    //size_t maxsize;
    enum BLOCKSTATE state;
    struct Kmem *next;
    struct Kmem *prev;
}kmem;

static uintptr_t start;
static uintptr_t pm_start, pm_end;
spinlock lk;
spinlock pk;

kmem *smem[CPUNUM];
kmem *lmem;
kmem sizetest;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);
  Logb("struct size %ld",sizeof(sizetest));

  start = pm_start;

  lmem = (kmem *)((void *)pm_start);
  smem[0] = (kmem *)(pm_start+STSIZE);
  smem[1] = (kmem *)(pm_start+2*STSIZE);
  smem[2] = (kmem *)(pm_start+3*STSIZE);
  smem[3] = (kmem *)(pm_start+4*STSIZE);

  lk.locked = 0;
  pk.locked = 0;
  for(int i=0;i<CPUNUM;i++){
        smem[i]->start = 0;
        smem[i]->size = 0;
        smem[i]->state = FREE;
        smem[i]->next = NULL;
        smem[i]->prev = NULL;
  }
  lmem->start = pm_start+5*STSIZE;
  lmem->size = pm_end-lmem->start;
  lmem->state = FREE;
  lmem->next = NULL;
  lmem->prev = NULL;
  assert(lk.locked==0);
  assert(pk.locked==0);
}

static void *my_bigalloc(size_t size){
    /*spin_lock(&lk);*/
    void *ret;
    int k = (int)size/BLOCK;
#ifdef DEBUG
    spin_lock(&pk);
    Logb("k: %d cpu %d",k,_cpu());
    spin_unlock(&pk);
#endif

    size_t ssize = (k+1)*BLOCK;
#ifdef DEBUG
    spin_lock(&pk);
    Logb("ssize: %d cpu %d",ssize,_cpu());
    spin_unlock(&pk);
#endif
    uintptr_t sstart = lmem->start+lmem->size-ssize;
    kmem *newalloc = (kmem *)((void *)(sstart-STSIZE));
    assert(newalloc!=NULL);
    newalloc->start = sstart;
    newalloc->size = ssize;
    assert(newalloc->size==ssize);
#ifdef DEBUG
    spin_lock(&pk);
    Logb("newalloc->size: %d %d",newalloc->size,_cpu());
    spin_unlock(&pk);
#endif
    lmem->size = lmem->size - ssize - STSIZE;
    if(lmem->next==NULL)
        newalloc->next = NULL;
    else{
        lmem->next->prev = newalloc;
        newalloc->next = lmem->next;
    }
    newalloc->prev = lmem;
    assert(newalloc->prev==lmem);
    lmem->next = newalloc;
    assert(newalloc->prev==lmem);
    newalloc->state = USING;
    ret = (void *)sstart;

    spin_lock(&pk);
    assert(newalloc->prev==lmem);
    assert(newalloc->size==ssize);
    assert(newalloc->prev->next==newalloc);
    assert(newalloc->state==USING);
    assert(lmem->next->prev==lmem);
    assert(lmem->prev==NULL);
    assert(lmem->state==FREE);
    assert(lk.locked==1);
    spin_unlock(&pk);
#ifdef DEBUG
    spin_lock(&pk);
    Logp("newalloc finish %d",_cpu());
    spin_unlock(&pk);
#endif
    /*spin_unlock(&lk);*/
    return ret;
}

static void *my_smallalloc(size_t size){
    /*int cpu = _cpu();*/
    //小内存分配
    void *ret;
    size_t ssize = size + STSIZE;
    size_t minsize = BLOCK;
    kmem *tmp = NULL;
    kmem *head = smem[_cpu()];
#ifdef DEBUG
    spin_lock(&pk);
    Logw("smem->start %x, %d",smem[_cpu()]->start,_cpu());
    spin_unlock(&pk);
#endif
    while(head!=NULL){
        if(ssize<=head->size && head->state==FREE){
            if(minsize > head->size){
                tmp = head;
                spin_lock(&pk);
                Logy("%d %d",size,_cpu());
                assert(tmp->prev->next==tmp);
                spin_unlock(&pk);
                minsize = head->size;
            }
        }
        head = head->next;
    }
    //申请新的4k页
    if(tmp == NULL){
        spin_lock(&lk);
        void *new = my_bigalloc(size);
        kmem *newpage = (kmem *)(new-STSIZE);

        spin_lock(&pk);
        Logy("assert started %d %d",size,_cpu());

        assert(newpage->state==USING);
        assert(newpage->prev!=NULL);
        assert(newpage->prev->next==newpage);
        assert(newpage->size >= BLOCK);
        assert(newpage->start>0);
        spin_unlock(&pk);

        newpage->state = FREE;

        if(smem[_cpu()]->next==NULL){
            newpage->next = NULL;
        }else{
            assert(0);
            smem[_cpu()]->next->prev = newpage;
            newpage->next = smem[_cpu()]->next;
        }
        newpage->prev = smem[_cpu()];
        smem[_cpu()]->next = newpage;
        newpage->size = newpage->size-ssize;
        void *addr = (void *)(newpage->start+newpage->size);
    #ifdef DEBUG
        spin_lock(&pk);
        Logg("addr test !!!! %x %d %d",newpage->start,newpage->size,_cpu());
        spin_unlock(&pk);
    #endif
        kmem *myalloc =(kmem *)addr;
        myalloc->state = USING;
        myalloc->start = newpage->size+newpage->start+STSIZE;
        myalloc->size = size;
        if(newpage->next==NULL){
            myalloc->next = NULL;
        }else{
            newpage->next->prev = myalloc;
            myalloc->next = newpage->next;
        }
        newpage->next = myalloc;
        myalloc->prev = newpage;
    #ifdef DEBUG
        spin_lock(&pk);
        Logg("tmp==NULL myalloc->size %d %d",myalloc->size,_cpu());
        spin_unlock(&pk);
    #endif
    #ifdef DEBUG
        spin_lock(&pk);
        Logp("newalloc finish %d %d",size,_cpu());
        spin_unlock(&pk);
    #endif

        ret = (void *)myalloc->start;
        spin_unlock(&lk);
    }else{
    #ifdef DEBUG
        spin_lock(&pk);
        Logg("tmp!=NULL  %d %d",size,_cpu());
        Logg("tmp->start %d tmp->size %d",tmp->start,tmp->size);
        spin_unlock(&pk);
    #endif
        //在处理器的内存中分配
        spin_lock(&pk);
        assert(tmp->prev!=NULL);
        Logg("addr test !!!! %x %d %d",tmp->start,tmp->size,_cpu());
        Logg("addr test ??? prev %x, tmp %d",tmp->prev->start,tmp->prev->next->start);
        Logy("cpu %d",_cpu());
        assert(tmp->prev->next==tmp);
        spin_unlock(&pk);

        tmp->size = tmp->size-ssize;
        void *addr = (void *)(tmp->start+tmp->size);
        kmem *myalloc = (kmem *)addr;
        myalloc->state = USING;
        myalloc->start = tmp->size+tmp->start+STSIZE;
        myalloc->size = size;
        if(tmp->next==NULL){
            myalloc->next = NULL;
        }else{
            tmp->next->prev = myalloc;
            myalloc->next = tmp->next;
        }
        myalloc->prev = tmp;
        tmp->next = myalloc;

        spin_lock(&pk);
        assert(myalloc->size==size);
        assert(myalloc->prev->next==myalloc);
        assert(tmp->prev->next==tmp);
        spin_unlock(&pk);


        ret = (void *)myalloc->start;
    #ifdef DEBUG
        spin_lock(&pk);
        Logg("tmp!=NULL finish  %d %d",size,_cpu());
        spin_unlock(&pk);
    #endif
    }

    return ret;
}

static void *kalloc(size_t size) {
#ifdef CORRECTNESS_FIRST
    spin_lock(&lk);
    void *ret = (void *)start;
    start += size;
    spin_unlock(&lk);
    return ret;
#else
#ifdef DEBUG
    spin_lock(&pk);
    Logb("alloc size: %d cpu %d",size,_cpu());
    spin_unlock(&pk);
#endif
    /*spin_lock(lk);*/
    size = ALIGNED(size);
    void *ret;
    if(size > SMALLSIZE){
        spin_lock(&lk);
        assert(0);
        ret = my_bigalloc(size);
        spin_unlock(&lk);
    }else{
        ret = my_smallalloc(size);
    }
    /*spin_unlock(lk);*/
    return ret;
#endif
  /*return NULL;*/
}
/*
static void myfree(void *ptr){
    kmem *myfree = (kmem *)(ptr-STSIZE);
    assert(myfree->state==USING);
    assert(myfree->size！=0);
}
*/
static void kfree(void *ptr) {
#ifdef CORRECTNESS_FIRST
    return;
#else
    kmem *myfree = (kmem *)(ptr-STSIZE);
    assert(myfree->state==USING);
    assert(myfree->size!=0);
    assert(myfree->prev!=NULL);
    //myfree(ptr);
    return;
#endif
    //ret
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
