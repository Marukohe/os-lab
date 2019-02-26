#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  switch('argv[1]'){
	  case 'V':
		  printf("pstree1.0 made by HEWEI");
		  break;
	  default: break;
  }
  assert(!argv[argc]); // always true
  return 0;
}
