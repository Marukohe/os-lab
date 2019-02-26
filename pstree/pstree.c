#include <stdio.h>
#include <string.h>
#include <assert.h>

void output_version(){
	printf("pstree1.0 made by HEWEI\n");
}

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  if(strcmp(argv[1],"-V")==0)
	  output_version();
  assert(!argv[argc]); // always true
  return 0;
}
