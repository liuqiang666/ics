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

void free_wp(WP *wp) {
  wp->next = free_;
  free_ = wp;
  if(head == NULL)
	return;
  if(head->NO == wp->NO){
    head = head->next;
  }
  WP *pre = head, *p = head->next;
  while(p != NULL){
    if(wp->NO == p->NO) {
	  pre->next = p-> next;
	  break;
	}
	pre = p;
	p = p->next;
  }
}
