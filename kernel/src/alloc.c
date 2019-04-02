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
kmem *bmem;
kmem sizetest;

static void pmm_init() {
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  Logp("pmm_init successfully, from pm_start 0x%x to pm_end 0x%x",pm_start,pm_end);
  Logp("struct size %ld",sizeof(sizetest));

  start = pm_start;
  assert(lmem==NULL);
  assert(bmem==NULL);
  for(int i=0;i<CPUNUM;i++)
      assert(smem[i]==NULL);

  lmem = (kmem *)(pm_start);
  for(int i=0;i<CPUNUM;i++){
      smem[i] = (kmem *)(pm_start+(1+i)*STSIZE);
  }
  bmem = (kmem *)(pm_start+5*STSIZE);

  for(int i=0;i<CPUNUM;i++){
      smem[i]->start = 0;
      smem[i]->size = 0;
      smem[i]->state = FREE;
      smem[i]->next = NULL;
      smem[i]->prev = NULL;
  }

  bmem->start = pm_start + 6*STSIZE;
  bmem->size = pm_end-bmem->start;
  bmem->state = FREE;
  bmem->next = NULL;
  bmem->prev = lmem;

  lmem->start = 0;
  lmem->size = 0;
  lmem->start = FREE;
  lmem->next = bmem;
  lmem->prev = NULL;

  lk.locked = 0;
  pk.locked = 0;

  assert(lmem!=NULL);
  for(int i=0;i<CPUNUM;i++){
      assert(smem[i]!=NULL);
      assert(smem[i]->next==NULL);
      assert(smem[i]->prev==NULL);
      assert(smem[i]->start==0);
      assert(smem[i]->size==0);
  }
  assert(lk.locked==0);
  assert(pk.locked==0);
  assert(lmem->next==bmem);
  assert(lmem->next->prev==lmem);
  assert(lmem->prev==NULL);
}

static void updatepoint(kmem *p1,kmem *p2){
    if(p1->next==NULL){
        p2->next = NULL;
    }else{
        p1->next->prev = p2;
        p2->next = p1->next;

        spin_lock(&pk);
        assert(p2->next->prev==p2);
        spin_unlock(&pk);
    }

    p1->next = p2;
    p2->prev = p1;
    spin_lock(&pk);
    assert(p1->next->prev==p1);
    assert(p2->prev->next==p2);
    spin_unlock(&pk);
}

static void *my_bigalloc(size_t size){
#ifdef DEBUG
    spin_lock(&pk);
    Logb("bigalloc start size: %d cpu: %d",size,_cpu());
    spin_unlock(&pk);
#endif
    void *ret = NULL;
    int k = size/BLOCK;

    size_t ssize = (k+1)*BLOCK;

    kmem *head = lmem;

    spin_lock(&pk);
    assert(ret==NULL);
    spin_unlock(&pk);

    while(head!=NULL){
        if(head->size>=ssize+STSIZE && head->state==FREE){
            break;
        }
        head = head->next;
    }

    uintptr_t sstart = head->start+head->size-ssize;

    kmem *newalloc = (kmem *)(sstart-STSIZE);
    newalloc->start = sstart;
    newalloc->size = ssize;
    newalloc->state = USING;
    head->size = head->size-ssize-STSIZE;

    spin_lock(&pk);
    assert(newalloc != NULL);
    spin_unlock(&pk);

    updatepoint(head,newalloc);
    ret = (void *)sstart;

    spin_lock(&pk);
    assert(ret!=NULL);
    assert(newalloc->size==ssize);
    assert(newalloc->prev==head);
    assert(head->next==newalloc);
    spin_unlock(&pk);

#ifdef DEBUG
    spin_lock(&pk);
    Logb("bigalloc finish size: %d cpu: %d",size,_cpu());
    spin_unlock(&pk);
#endif
    return ret;
}

static void *my_smallalloc(size_t size){
#ifdef DEBUG
    spin_lock(&pk);
    Logw("smallalloc start size: %d cpu: %d",size,_cpu());
    spin_unlock(&pk);
#endif
    void *ret = NULL;
    size_t ssize = size+STSIZE;
    size_t minsize = BLOCK;
    kmem *tmp = NULL;
    kmem *head = smem[_cpu()];

    while(head!=NULL){
        if(head->size>=ssize && head->state==FREE){
            if(minsize > head->size){
                tmp = head;
                minsize = head->size;
            }
        }
        head = head->next;
    }

    if(tmp==NULL){
    #ifdef DEBUG
        spin_lock(&pk);
        Logy("smallalloc start tmp==NULL size: %d cpu: %d",size,_cpu());
        spin_unlock(&pk);
    #endif
        void *new = my_bigalloc(size);
        kmem *newpage = (kmem *)(new-STSIZE);

        spin_lock(&pk);
        assert(newpage!=NULL);
        assert(newpage->state==USING);
        assert(newpage->prev->next==newpage);
        assert(newpage->size>=BLOCK);
        assert(newpage->start!=0);
        spin_unlock(&pk);

        newpage->state=FREE;
        updatepoint(smem[_cpu()],newpage);

        uintptr_t addr = newpage->start+newpage->size-size;
        kmem *myalloc = (kmem *)(addr-STSIZE);
        myalloc->start = addr;
        myalloc->size = size;
        myalloc->state = USING;
        newpage->size = newpage->size-ssize;
        updatepoint(newpage,myalloc);
        ret = (void *)myalloc->start;
    #ifdef DEBUG
        spin_lock(&pk);
        Logy("smallalloc finish tmp==NULL size: %d cpu: %d",size,_cpu());
        spin_unlock(&pk);
    #endif

    }else{
    #ifdef DEBUG
        spin_lock(&pk);
        Logy("smallalloc start tmp!=NULL size: %d cpu: %d",size,_cpu());
        spin_unlock(&pk);
    #endif
        uintptr_t addr= tmp->start+tmp->size-size;
        kmem *myalloc = (kmem *)(addr-STSIZE);
        myalloc->start = addr;
        myalloc->state = USING;
        myalloc->size = size;
        tmp->size = tmp->size-ssize;
        updatepoint(tmp,myalloc);
        ret = (void *)myalloc->start;
    #ifdef DEBUG
        spin_lock(&pk);
        Logy("smallalloc start tmp!=NULL size: %d cpu: %d",size,_cpu());
        spin_unlock(&pk);
    #endif
    }

#ifdef DEBUG
    spin_lock(&pk);
    Logw("smallalloc finish size: %d cpu: %d",size,_cpu());
    spin_unlock(&pk);
#endif
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
    spin_lock(&lk);
    size = ALIGNED(size);
    void *ret;
    if(size > SMALLSIZE){
        ret = my_bigalloc(size);
    }else{
        ret = my_smallalloc(size);
    }
    spin_lock(&pk);
    assert(ret!=NULL);
    spin_unlock(&pk);
#ifdef DEBUG
    spin_lock(&pk);
    Logw("alloc space from %x",(uintptr_t)ret);
    spin_unlock(&pk);
#endif
    spin_unlock(&lk);
    return ret;
#endif
}

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
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
