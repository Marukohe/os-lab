#include <am.h>
#include <amdev.h>
#include <klib.h>

//#define SIDE 16
#define SIDE 16
//0 for x 1 for y
struct snake{
	int head[3];
	int tail[3];
	int pivot[20][2]; //turning point
	int pnum;
	int dire;   // 0 up, 1 right, 2 down, 3 left 
};

static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}

