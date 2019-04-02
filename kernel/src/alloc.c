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
  assert(lmem==NULL);
  for(int i=0;i<CPUNUM;i++)
      assert(smem[i]==NULL);

  lmem = (kmem *)(pm_start);
  for(int i=0;i<CPUNUM;i++){
      smem[i] = (kmem *)(pm_start+(1+i)*STSIZE);
  }

  for(int i=0;i<CPUNUM;i++){
      smem[i]->start = 0;
      smem[i]->size = 0;
      smem[i]->state = FREE;
      smem[i]->next = NULL;
      smem[i]->prev = NULL;
  }

  lmem->start = pm_start + 5*STSIZE;
  lmem->size = pm_end-lmem->start;
  lmem->state = FREE;
  lmem->next = NULL;
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
  assert(lmem->next==NULL);
  assert(lmem->prev==NULL);
}

static void *my_bigalloc(size_t size){
    void *ret;
    return ret;
}

static void *my_smallalloc(size_t size){
    return NULL;
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
#ifdef DEBUG
    spin_lock(&pk);
    Logw("alloc space from %x",(uintptr_t)ret);
    spin_lock(&pk);
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
