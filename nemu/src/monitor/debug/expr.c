#include "nemu.h"

#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_PLUS = '+', 
  TK_SUB = '-',
  TK_MULTI = '*',
  TK_DIVIDE = '/',
  TK_LP = '(',
  TK_RP = ')', 
  NUM
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},  // spaces
  {"\\+", '+'},       // plus
  {"==", TK_EQ},      // equal
  {"\\-", '-'},       // sub
  {"\\*", '*'},       // multi
  {"\\/", '/'},       // divide
  {"[0-9]+", NUM},    // number
  {"\\(", '('},       // left parentheses
  {"\\)", ')'},       // right parentheses
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
		  case NUM:
			/*
			if(substr_len >= 32)
			  substr_len = 31;*/
			if(substr_len >=32) {
			  printf("sub expr max len 31,your len: %d", substr_len);
			  return false;
			}
			for(int i = 0;i < substr_len;i++) {
			  tokens[nr_token].str[i] = substr_start[i];
			}
          default: 
			tokens[nr_token].type = rules[i].token_type;
			nr_token ++;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int p, int q) {
  int i, j = 0;
  if(tokens[p].type != '(' || tokens[q].type != ')')
	return false;
  else {
	for(i = p + 1;i < q;i ++){
	  if(tokens[i].type == '(')
		j ++;//push statck
	  else if(tokens[i].type == ')') {
		j --;//pop statck
		if(j < 0)//statck low overflow
		  return false;	//not matched
	  }
	}
	if(j != 0)//statck not empty, not matched
	  return false;
	return true;
  }
}

static inline bool is_op(int p) {
  if(tokens[p].type == '+' || tokens[p].type == '-' || tokens[p].type == '*' || tokens[p].type == '/')
	return true;
  return false;
}

static uint32_t op_priority(Token t) {
  int p;
  switch(t.type) {
	case '+':
	case '-':p=0;break;
	case '*':
	case '/':p=1;break;
	default: p = -1;
  }
  return p;
}

static int main_op_position(int p, int q) {
  int i;
  int poistion = p;
  Token m_op=tokens[p];
  for(i = p;i < q; i++) {
	if(is_op(i)) {
 	  if(op_priority(tokens[i]) <= op_priority(m_op)) {
	    m_op = tokens[i];
		poistion = i;
	  }
	} 
    else if(tokens[i].type == '(') {
	  while(tokens[i].type != ')' && i < q) {
	    i ++;
      }
	}
  }
  return poistion;
}

static uint32_t eval(int p, int q, bool *success) {
  if(p > q) {
	printf("Bad expression\n");
    *success = false;
	return 0;
  }
  else if (p == q) {
	return strtoul(tokens[p].str, NULL, 10);
  }
  else if (check_parentheses(p,q)) {
	return eval(p+1, q-1, success);
  }
  else {
	int op = main_op_position(p, q);
    printf("main op posi: %d\n", op);
	uint32_t val1 = eval(p, op-1, success);
	if(!(*success)) 
	  return 0;
	uint32_t val2 = eval(op+1, q, success);
	if(!(*success)) 
	  return 0;
	printf("val1:%d val2:%d\n", val1, val2);
	*success = true;
	switch (tokens[op].type) {
	  case '+': return val1 + val2;break;
	  case '-': return val1 - val2;break;
	  case '*': return val1 * val2;break;
	  case '/': return val1 / val2;break;
	  default: assert(0);
	}
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1, success);
}
