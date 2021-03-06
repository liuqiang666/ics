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
  NUM = 258,
  HEXNUM,
  REG,
  TK_NEQ,
  TK_AND,
  TK_OR,
  TK_NOT,
  DEREF
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
  {"!=", TK_NEQ},     // not equal
  {"&&", TK_AND},     // and
  {"\\|\\|", TK_OR},  // or
  {"!", TK_NOT},      // not
  {"\\-", '-'},       // sub
  {"\\*", '*'},       // multi deref
  {"\\/", '/'},       // divide
  {"^0[xX][0-9a-fA-F]+", HEXNUM},    // hex number
  {"[0-9]+", NUM},    // number
  {"^\\$[eE][a-zA-Z]{2}", REG},    // reg name
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
		  case HEXNUM:
		  case REG:
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
			tokens[nr_token].str[substr_len] = '\0';
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
  if(tokens[p].type == '+' || tokens[p].type == '-' || tokens[p].type == '*' || tokens[p].type == '/' || tokens[p].type == TK_EQ || tokens[p].type == TK_NEQ || tokens[p].type == TK_AND || tokens[p].type == TK_OR || tokens[p].type == TK_NOT  || tokens[p].type == DEREF)
	return true;
  return false;
}

static uint32_t op_priority(Token t) {
  int p;
  switch(t.type) {
	case TK_OR:
	case TK_AND: p = 0;break;
	case TK_EQ:
	case TK_NEQ: p = 1;break;
	case '+':
	case '-': p = 2;break;
	case '*':
	case '/': p = 3;break;
	case DEREF:
	case TK_NOT: p = 4;break;
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
    *success = false;
	return 0;
  }
  else if (p == q) {
	*success = true;
	char *input_name = &tokens[p].str[1];
	int i = 1;
	switch(tokens[p].type) {
	  case NUM:
	  case HEXNUM:
	    return strtoul(tokens[p].str, NULL, 0);
	  case REG:
	    if(strcmp("eip", input_name) == 0){
		  return cpu.eip;
	    }
	    for(i = 0;i < 8; i++) {
	      if(strcmp(input_name, reg_name(i, 4)) == 0) {
		    return reg_l(i);
		  }  
	    }
	    printf("Invalid reg name: %s\n", tokens[p].str);
	  default:
	    *success = false;
        return 0;	
	}
  }
  else if (check_parentheses(p,q)) {
	return eval(p + 1, q - 1, success);
  }
  else {
	int op = main_op_position(p, q);
	uint32_t val1 = 0;
	if(tokens[op].type != DEREF && tokens[op].type != TK_NOT) {
	  val1 = eval(p, op-1, success);
      if(!(*success))
	    return 0;
	}
	uint32_t val2 = eval(op+1, q, success);
    if(!(*success))
	  return 0;
	switch (tokens[op].type) {
	  case '+': return val1 + val2;
	  case '-': 
		if(val1 < val2) {
		  printf("Result is a signed number, less than zero.\n");
		  *success = false;
 		  return 0;
		}
		return val1 - val2;
	  case '*': return val1 * val2;
	  case '/': 
		if(val2 == 0){
		  *success = false;
		  printf("Division by zero.\n");
		  return 0;
		}
		return val1 / val2;
	  case TK_EQ: return (val1 == val2);
	  case TK_NEQ: return (val1 != val2);
	  case TK_AND: return (val1 && val2);
      case TK_OR: return (val1 || val2);
	  case TK_NOT: return (!val2);
	  case DEREF: return vaddr_read(val2, 4);
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
  for(int i = 0; i < nr_token; i++) {
	if(tokens[i].type == '*' && (i == 0 || is_op(i - 1))) 
	  tokens[i].type = DEREF;
  }
  return eval(0, nr_token-1, success);
}
