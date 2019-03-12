#ifndef __CO_H__
#define __CO_H__

#define DEFAULT_STACK_SIZE 4096
#define MAX_CO 100
enum COSTATE{FREE,READY,RUNNING,SUSPEND};
typedef void (*func_t)(void *arg);
struct co;

void co_init();
struct co* co_start(const char *name, func_t func, void *arg);
void co_yield();
void co_wait(struct co *thd);

#endif
