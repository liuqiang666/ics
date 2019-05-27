#include "klib.h"
//#include "stdio.h"
//#include "assert.h"
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//void* memcpy(void* out, const void* in, size_t n);

size_t strlen(const char *s) {
  //assert(s != NULL);
  if(s == NULL)
	return 0;
  size_t size = 0;
  while(s[size] != '\0') size ++;
  return size;
}

char *strcpy(char* dst,const char* src) {
  assert((dst != NULL) && (src != NULL));
  size_t size = strlen(src);
  int i = 0;
  //无重叠，从低地址开始复制
  if(dst <= src || dst >= src + size ) {
    i = 0;
    while(src[i] != '\0') {
      dst[i] = src[i];
      i ++;
    }
    dst[i] = '\0';
  } else {
    //重叠，从高地址开始复制
	for(i = size; i >= 0;i --)
	  dst[i] = src[i];
  }
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  assert((dst != NULL) && (src != NULL));
  size_t size = strlen(src);
  if(size >= n) {
    dst = memcpy(dst, src, n);
   //for(int i = 0; i < n; i++)
	 //dst[i] = src[i];
  }
  else {
	strcpy(dst, src);
    size_t size_d = strlen(dst);
    for(int i = size; i < size_d && i < n; i++)
 	  dst[i] = '\0';
  }

  return dst;
}

char* strcat(char* dst, const char* src) {
  assert((dst != NULL) && (src != NULL));
  char *p = dst;
  while(*p != '\0') p ++;
  while(*src != '\0') *p ++ = *src ++;
  *p = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  if(s1 == NULL && s2 == NULL)
    return 0;
  if(s1 == NULL)
    return -1;
  if(s2 == NULL)
    return 1;
  while(*s1 != '\0' && *s1 == *s2){
	s1 ++;
    s2 ++;
  }
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  if(s1 == NULL && s2 == NULL)
    return 0;
  if(s1 == NULL)
    return -1;
  if(s2 == NULL)
    return 1;
  while((n--) && *s1 != '\0' && *s2 != '\0' && *s1 == *s2){
	s1 ++;
    s2 ++;
  }
  return *s1 - *s2;
}

void* memset(void* v,int c,size_t n) {
  for(int i = 0; i < n; i++)
    *(unsigned char *)(v + i) = c;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  assert(out != NULL && in != NULL);
  void* ret = out;
  //没有重叠，从低地址开始复制
  if(out <= in || (char *)out >=(char *)in + n){
    while(n --) {
      *(char *)out = *(char *)in;
	  in = (char *)in + 1;
      out = (char *)out + 1;
    }
  } else {
    //有内存重叠，从高地址开始复制
    in = (char *)in + n - 1;
    out = (char *)out + n - 1;
    while(n --) {
      *(char *)out = *(char *)in;
	  in = (char *)in - 1;
      out = (char *)out - 1;
    }
  }
  return ret;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if(s1 == NULL && s2 == NULL)
    return 0;
  if(s1 == NULL)
    return -1;
  if(s2 == NULL)
    return 1;
  while((--n) && *(char *)s1 == *(char *)s2){
	s1 = (char *)s1 + 1;
    s2 = (char *)s2 + 1;
  }
  return (*((unsigned char *)s1)) - (*((unsigned char *)s2));
}

#endif
/*
char *s[] = {
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
	"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
	", World!\n",
	"Hello, World!\n",
	"#####"
};

char str1[] = "Hello";
char str[20];

int main() {
	assert(strcmp(s[0], s[2]) == 0);
	assert(strcmp(s[0], s[1]) == -1);
	assert(strcmp(s[0] + 1, s[1] + 1) == -1);
	assert(strcmp(s[0] + 2, s[1] + 2) == -1);
	assert(strcmp(s[0] + 3, s[1] + 3) == -1);
    
	assert(strcmp( strcat(strcpy(str, str1), s[3]), s[4]) == 0);

	assert(memcmp(memset(str, '#', 5), s[5], 5) == 0);

	return 0;
}*/
