#include <am.h>
#include <amdev.h>

//#define SIDE 16
#define SIDE 16
//0 for x 1 for y
struct snake{
	int head[3];
	int tail[3];
	int pivot[20]; //turning point
};

static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}
