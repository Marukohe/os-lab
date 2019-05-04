
struct spinlock{
    volatile intptr_t locked;
    char name[10];
};


void spin_lock(struct spinlock *lk);
void spin_unlock(struct spinlock *lk);
void pushcli();
void popcli();
