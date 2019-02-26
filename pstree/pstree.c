#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>

#define DEFAULT_DIR "/proc/"

void output_version(){
	printf("pstree1.0 made by HEWEI.\n");
}

void read_list(const char *dirPath){
	DIR * dir;
	dir = opendir(dirPath);
	struct dirent * file;
	while((file = readdir(dir))!=NULL){
		printf("filename: %s\n",file->d_name);
	 }
}

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  //int i;
  //for (i = 0; i < argc; i++) {
  //  assert(argv[i]); // always true
  //  printf("argv[%d] = %s\n", i, argv[i]);
  //}
  if(argc!=1 && strcmp(argv[1],"-V")==0)
	  output_version();
  read_list("~/NJU");
  assert(!argv[argc]); // always true
  return 0;
}
