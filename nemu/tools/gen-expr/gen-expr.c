#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#define NUM 1000
// this should be enough
static char buf[NUM];
static int pos = 0;

static uint32_t choose(uint32_t n) {
  return rand()%n;
}

static void gen_num(){
  char num[255];
  sprintf( num, "%d", choose(10)+1);
  int i=0;
  while(num[i] != '\0' && pos < NUM-1) {
	buf[pos++] = num[i++];
  }
}

static void gen(char a) {
  buf[pos++] = a;
}

static void gen_rand_op() {
  switch(choose(4)) {
	case 0: buf[pos++] = '+';break;
	case 1: buf[pos++] = '-';break;
	case 2: buf[pos++] = '*';break;
	default: buf[pos++] = '/';break;
  }
}

static inline void gen_rand_expr() {
  if(pos >= NUM)
	return;
  switch (choose(3)) {
	case 0: gen_num();break;
	case 1: gen('(');gen_rand_expr();gen(')');break;
	default: gen_rand_expr();gen_rand_op();gen_rand_expr();break;
  }
  int i, j = choose(2);
  for(i = 0;i < j &&  pos < NUM-1;i++){
	buf[pos++] = ' ';
  }
  buf[pos] = '\0';
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i, j;
  for (i = 0; i < loop; i ++) {
	for(j=0;j<pos;j++)
	  buf[j] = '\0';
	pos = 0;
    gen_rand_expr();
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
