#ifndef _a_TASK_H
#define _a_TASK_H

#include <tmc.h>

typedef union u_cardinal *cardinal;

typedef struct str_task_list_item
{
  tmstring name;
  uint nr_cardinals;
  cardinal card;

  struct str_task_list_item *next;
} *task_list_item_p;

uint add_task (tmstring name);
void set_task_label (tmstring name, int task_id);

int get_task_label (const_tmstring name, uint *card);
void print_tasks (void);

uint get_nr_tasks (void);
task_list_item_p get_first_task (void);
task_list_item_p get_next_task (task_list_item_p t);
char* get_task_name (task_list_item_p t);
uint get_task_nr_card (task_list_item_p t);
uint get_task_nr_card (const_tmstring name);
uint get_task_max_nr_cards (void);

#endif

