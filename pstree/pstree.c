#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  if(strcmp(argv[1],"V")==0)
  	printf("pstree1.0 made by HEWEI");
  assert(!argv[argc]); // always true
  return 0;
}
