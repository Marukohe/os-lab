#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define DEFAULT_DIR "/proc/"
#define Assert(_con...) \
do{\
	if(!(_con)){\
		fprintf(stderr,__VA_ARGS__)\
		assert(0);\
	}\
}while(0)

struct pst{
	char pidname[200];
	int fa;
	int son[1000];
	int cntson;
	int flag;
}P[200000];

void output_version(){
	printf("pstree1.0 made by HEWEI.\n");
}

bool is_digit(char n){
	if(n >='0' && n <='9')
		return true;
	else
		return false;
}

void read_list(const char *dirPath){
	DIR * dir;
	dir = opendir(dirPath);
	//assert(!dir);
	struct dirent * file;
	while((file = readdir(dir))!=NULL){
		//printf("filename: %s\n",file->d_name);
		if(is_digit(file->d_name[0])){
			int pidnum = atoi(file->d_name);
			//printf("num: %d\n",pidnum);
			char path[100];
			strcpy(path,DEFAULT_DIR);
			strcat(path,file->d_name);
			strcat(path,"/status");
			//printf("%s\n",path);
			FILE * fp = fopen(path,"r");
			assert(fp!=NULL);
			char str[100];
			//if(strcmp(file->d_name,"1")==0){
			while(!feof(fp)){
				fgets(str,100,fp);
				//printf("%s",str);
				int size = strlen(str);
				if(strncmp(str,"Name",4)==0){
					int k=0;
					for(int i = 4;i<size;i++){
						if(str[i]>'a' && str[i]<'z')
							P[pidnum].pidname[k++] = str[i];
			 		}
					P[pidnum].pidname[k] = '\0';
			 	}
				//printf("%s\n",P[pidnum].pidname);
				if(strncmp(str,"PPid",4)==0){
					char fanum[10];
					int j=0;
					for(int i = 4;i<size;i++){
						if(str[i]>='0' && str[i]<='9')
							fanum[j++] = str[i];
					}
					//printf("fanum : %d %d\n",pidnum,atoi(fanum));
					P[pidnum].fa = atoi(fanum);
				}
			}
			//}
			fclose(fp);
		}
	}
	closedir(dir);
}

void print_tree(){
	printf("\n");
}

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  //int i;
  //for (i = 0; i < argc; i++) {
  //  assert(argv[i]); // always true
  //  printf("argv[%d] = %s\n", i, argv[i]);
  //}
  //if(argc!=1 && strcmp(argv[1],"-V")==0)
  //	  output_version();
  read_list(DEFAULT_DIR);
  int o;
  while((o = getopt(argc,argv,"-pnV"))!= -1){
	  switch(o){
		  case 'V':
			  output_version();
			  break;
		  case 'n':
			  printf("-n\n");
			  break;
		  case 'p':
			  printf("-p\n");
			  break;
		  default:
			  Assert("invalid usage\n"); 
			  break;
	  }
  }
  assert(!argv[argc]); // always true
  return 0;
}
