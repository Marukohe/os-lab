/*
typedef struct spinlock{
    volatile intptr_t locked;
    char name[10];
}spinlock;
*/

void spin_lock(spinlock *lk);
void spin_unlock(spinlock *lk);
void pushcli();
void popcli();
