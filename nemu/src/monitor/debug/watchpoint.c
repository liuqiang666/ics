#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
	wp_pool[i].status = true;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
  WP *wp = free_;
  if(wp == NULL) {
	printf("No free watchpoint.\n");
	assert(0);
  }  
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

void free_wp(int n) {
  if(head == NULL)
	return;
  WP *wp = NULL;
  if(head->NO == n){
	wp = head;
    head = head->next;
  }
  else {
    WP *pre = head, *p = head->next;
    while(p != NULL){
      if(p->NO == n) {
	    wp = p;
	    pre->next = p-> next;
	    break;
	  }
	  pre = p;
	  p = p->next;
    }
  }
  if(wp == NULL){
    printf("watchpoint %d does not exist.\n", n);
	return;
  } 
  wp->next = free_;
  free_ = wp;
}

bool check_watchpoints() {
  WP *p = head;
  bool success;
  bool hit = false;
  while(p != NULL) {
    uint32_t res = expr(p->expr, &success);
    if(!success) {
      hit = true;
	  p->status = false;
      printf("Expression evalution failed at watchpoint %d: %s\n", p->NO, p->expr);
	  p = p->next;
      continue;
    }
    if(res != p->result) {
      hit = true;
      printf("watchpoint %d: %s\n", p->NO, p->expr);
      printf("old value: %d\n", p->result);
      printf("new value: %d\n", res);
      p->result = res;
    }
    p = p->next;
  }
  return hit;
}

void watchpoints_info() {
  if(head != NULL)
    printf("NO\tEXPR\t\tSTATUS\tVALUE\n");
  else {
	printf("No watchpoint.\n"); 
	return;
  } 
  WP *p = head;
  while(p != NULL) {
    printf("%d\t%s\t\t%d\t%d\n", p->NO, p->expr, p->status, p->result);
	p = p->next;	
  }
}
