
typedef struct spinlock{
    intptr_t locked;
    char name[10];
}spinlock, *spinlock_t;

spinlock *lk;

void spin_lock(spinlock *lk);
void spin_unlock(spinlock *lk);
