#ifndef __COMMON_H__

#include <kernel.h>
#include <nanos.h>
#include <x86.h>
#include <spinlock.h>

enum BLOCKSTATE{FREE,USING,READY};

#define DEBUG
// #define CORRECTNESS_FIRST
#endif
