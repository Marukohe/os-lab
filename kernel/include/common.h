#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>
#include <kmt.h>
#include <os.h>

#define MAXCPU 4
#define MAXHANDLER 10
#define STSIZE 4096
#define INT_MAX 0x7ffffff
#define INT_MIN (-INT_MAX - 1)
enum BLOCKSTATE{FREE,USING,READY};
enum TSKSTATE{FREET = 0, WAITING, RUNNING}; //can schedule, wait sem, running

//#define DEBUG
// #define CORRECTNESS_FIRST
struct task {
    int id;
    const char *name;
    _Context context;
    uint8_t fence1;
    uint8_t stack[STSIZE];
    uint8_t fence2;
    void (*entry)(void *arg);
    void *arg;
    enum TSKSTATE state;
};

struct spinlock {
    volatile intptr_t locked;
    const char *name;
    int cpu;
};


struct semaphore {
    const char *name;
    spinlock_t locked;
    int count;
    int id[25];     //task id
    int cntid;      //total id
};

//=======================handler====================
_Context *kmt_context_save(_Event ev, _Context *context);
_Context *kmt_context_switch(_Event ev, _Context *context);

#endif
