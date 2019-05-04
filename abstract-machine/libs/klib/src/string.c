#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t i = 0;
	while(s[i]!='\0')
		i++;
  return i;
}

char *strcpy(char* dst,const char* src) {
	size_t i;
	//size_t sizesrc=strlen(src);
	for( i=0;src[i]!='\0';i++)
	   dst[i]=src[i];
	dst[i]='\0';
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t i;
	for(i = 0; i < n && src[i] != '\0'; i++)
		dst[i]=src[i];
	for( ; i<n; i++)
		dst[i]='\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
	size_t dest_len = strlen(dst);
	size_t i;

	for(i=0;src[i]!='\0';i++)
		dst[dest_len+i]=src[i];
	dst[dest_len+i]='\0';
	return dst;
}

int strcmp(const char* s1,const char*s2){
	for(;*s1==*s2;++s1,++s2)
		if(*s1=='\0')
			return 0;
	return ((*(unsigned char*)s1<*(unsigned char*)s2)?-1:1);
}

int strncmp(const char* s1, const char* s2, size_t n) {
	size_t i;
	int value=0;
	for(i=0;i<n;i++)
	{
		if(s1[i]-'\0'==s2[i]-'\0')
			continue;
		else
		{
			if(s1[i]-'\0'<s2[i]-'\0')
			{
				value=-1;
				return value;
			}
			else return 1;
		}
	}
	return value;
}

void* memset(void* v,int c,size_t n) {
	const unsigned char ch = c;
	unsigned char *ret;
	for(ret=v;0<n;++ret,--n)
		*ret=ch;
	return v;
}

void* memcpy(void* out, const void* in, size_t n){
	char *s1;
	const char *s2;
	for(s1=out,s2=in;0<n;++s1,++s2,--n)
		*s1=*s2;
	//*s1 = '\0';
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
	if(s1==NULL||s2==NULL||n<0)
	   return 0;
	const unsigned char* dst = (const unsigned char*)s1;
	const unsigned char* src = (const unsigned char*)s2;
	while(n--){
		while(*dst==*src)
		{
			if(*dst=='\0')
				return 0;
			dst++;
			src++;
		}
		//if(n<=0)
		//	return 0;
	}
	if(n==-1)
		return 0;
	if(*dst>*src)
		return 1;
	else if(*dst<*src)
		return -1;
	return 0;
}

void *memmove(void *dest, const void *src, size_t n){
    char *sc1;
    const char *sc2;
    sc1 = dest;
    sc2 = src;
    if(sc2 < sc1 && sc1 < sc2 + n){
        for(sc1 += n, sc2 += n; 0 < n; n--){
            *--sc1 = *--sc2;
        }
    }
    else{
        for(; 0 < n; n--){
            *sc1++ = *sc2++;
        }
    }
    return dest;
}

#endif
