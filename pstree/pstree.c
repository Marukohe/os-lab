#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#define MAX_LEN 200000
#define DEFAULT_DIR "/proc/"
#define Assert(_con,...) \
do{\
	if(!(_con)){\
		fprintf(stderr,__VA_ARGS__);\
		assert(0);\
	}\
}while(0)

struct pst{
	int pid;
	char pidname[200];
	int fa;
	int son[1000];
	int cntson;
	int flag;       //是否已经输出
}P[MAX_LEN]={};

//int cmp1(struct pst a, struct pst b){
//	return a.pid<b.pid;
//}
int cmp1( const void *a ,const void *b)
{
     return ((struct pst *)a)->pid - ((struct pst *)b)->pid;
}

//int cmp2(struct pst a, struct pst b){
//	return a.pidname[0]<b.pidname[0];
//}
int cmp2( const void *a , const void *b )
{
     return strcmp( ((struct pst *)a)->pidname , ((struct pst *)b)->pidname );
}

void output_version(){
	printf("pstree1.0 made by HEWEI.\n");
}

bool is_digit(char n){
	if(n >='0' && n <='9')
		return true;
	else
		return false;
}

int search_tree(int pidd){
	for(int i=0;i<MAX_LEN;i++)
		if(P[i].pid == pidd)
			return i;
	return -1;
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
			P[pidnum].pid = pidnum;
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
					for(int i = 5;i<size;i++){
						if(str[i] != 32 && str[i]!=13 && str[i]!=10)
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
					P[pidnum].fa = atoi(fanum);  //记录父亲节点
					P[atoi(fanum)].son[P[atoi(fanum)].cntson++] = pidnum; //记录儿子节点
				}
			}
			//}
			fclose(fp);
			char path2[100];
			strcpy(path2,DEFAULT_DIR);
			strcat(path2,file->d_name);
			strcat(path2,"/task/");

			DIR * dir1;
			dir1 = opendir(path2);
			//assert(!dir);
			struct dirent * file2;
			while((file2 = readdir(dir1))!=NULL){
				int pidnum2 = atoi(file2->d_name);
				if(pidnum != pidnum2){
					P[pidnum2].pid = pidnum2;
					P[pidnum2].fa = pidnum;
					P[pidnum].son[P[pidnum].cntson++] = pidnum2;
					strcpy(P[pidnum2].pidname,"{");
					strcat(P[pidnum2].pidname,P[pidnum].pidname);
					strcat(P[pidnum2].pidname,"}");
				}
			}
			closedir(dir1);
		}
	}
	closedir(dir);
}

void print_tree(bool is_n, bool is_p){
	//for(int i=0;i<MAX_LEN;i++){
	//	if(P[i].pid!=0)
	//		printf("pid: %d, fa: %d, name: %s\n",P[i].pid, P[i].fa,P[i].pidname);
	//}
	if(is_n){
		qsort(P,MAX_LEN,sizeof(P[0]),cmp1);
	}else{
		qsort(P,MAX_LEN,sizeof(P[0]),cmp2);
	}

	//for(int i=0;i<MAX_LEN;i++){
	//	if(P[i].pid!=0)
	//		printf("pid: %d, fa: %d, name: %s\n",P[i].pid, P[i].fa,P[i].pidname);
	//}
	printf("%s\n",P[search_tree(1)].pidname);
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
  bool is_n = false;
  bool is_p = false;
  while((o = getopt(argc,argv,"-pnV"))!= -1){
	  switch(o){
		  case 'V':
			  output_version();
			  break;
		  case 'n':
			  is_n = true;
			  printf("-n\n");
			  break;
		  case 'p':
			  is_p = true;
			  printf("-p\n");
			  break;
		  default:
			  Assert(0,"invalid usage\n"); 
			  break;
	  }
  }
  print_tree(is_n, is_p);
  assert(!argv[argc]); // always true
  return 0;
}
