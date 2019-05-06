
//==========================spinlock==============================
void spin_init(spinlock_t *lk, const char *name);
void spin_lock(spinlock_t *lk);
void spin_unlock(spinlock_t *lk);
int holding(spinlock_t *lk);
void pushcli();
void popcli();

//===========================sem==================================
void sem_init(sem_t *sem, const char *name, int value);
void sem_wait(sem_t *sem);
void sem_signal(sem_t *sem);
