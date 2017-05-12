#include <stdio.h>
#include <assert.h>

#include "data/arguments.h"
#include "task.h"
#include "defs.h"
#include "tm2dag.h"
#include "m_string.h"
#include "error.h"
#include "cardinals.h"
#include "io.h"



typedef struct str_task_entry_item
{
  uint size;
  cardinal *card;
} *task_entry_item_p;

union u_cardinal
{
  struct str_task_entry_item te;
  int label;
};


task_list_item_p tasks = NULL;


static task_list_item_p get_task_item (const_tmstring name);
static task_list_item_p add_task_item (tmstring name);
static void set_task_label_ (cardinal c, uint idx, uint nr_card, int label);
static int get_task_label_ (cardinal c, uint idx, uint nr_card, uint *card);
static void print_task_items (cardinal c, uint idx, uint nr_card, m_string str);


static int label = 0;

uint
add_task (tmstring name)
{
  task_list_item_p t = get_task_item (name);
  if (t == NULL)
    t = add_task_item (name);


  if (t->nr_cardinals == 0)
    t->card->label = label;
  else
    set_task_label_ (t->card, 0, t->nr_cardinals, label);

  label++;
  
  return (label - 1);
}

void
set_task_label (tmstring name, int task_id)
{
  task_list_item_p t = get_task_item (name);
  if (t == NULL)
    t = add_task_item (name);

  if (t->nr_cardinals == 0)
    t->card->label = task_id;
  else
    set_task_label_ (t->card, 0, t->nr_cardinals, task_id);
}

int
get_task_label (const_tmstring name, uint *card)
{
  task_list_item_p t = get_task_item (name);
  if (t == NULL)
    return -1;

  if (t->nr_cardinals == 0)
    return t->card->label;
  else
    return get_task_label_ (t->card, 0, t->nr_cardinals, card);
}

void
print_tasks (void)
{
  m_string m_str = get_new_m_string();
  task_list_item_p t;

  for (t = tasks; t != NULL; t = t->next)
  {
    if (t->nr_cardinals == 0)
    {
      fprintf (outfile, "%s[] - T%d\n", t->name, t->card->label);
      continue;
    }

    fprintf (outfile, "task: %s: %d\n", t->name, t->nr_cardinals);
    clear_m_string (m_str);
    append_string (m_str, t->name);
    append_char (m_str, '[');    
    print_task_items (t->card, 0, t->nr_cardinals, m_str);
  }

  free_m_string (m_str);
}


uint
get_nr_tasks (void)
{
  task_list_item_p t;
  uint nr = 0;

  for (t = tasks; t != NULL; t = t->next)
    nr++;

  return nr;
}

task_list_item_p
get_first_task (void)
{
  return tasks;
}

task_list_item_p
get_next_task (task_list_item_p t)
{
  if (t == NULL)
    return NULL;

  return t->next;
}

char*
get_task_name (task_list_item_p t)
{
   if (t == NULL)
    return NULL;

  return t->name;
}

uint
get_task_nr_card (task_list_item_p t)
{
  if (t == NULL)
    return 0;

  return t->nr_cardinals;
}

uint
get_task_nr_card (const_tmstring name)
{
  task_list_item_p t;

  for (t = tasks; t != NULL; t = t->next)
    if (strcmp (t->name, name) == 0)
      return t->nr_cardinals;

  return 0;
}

uint
get_task_max_nr_cards (void)
{
  task_list_item_p t;
  uint max = 0;

  for (t = tasks; t != NULL; t = t->next)
    if (get_task_nr_card(t) > max)
      max = get_task_nr_card(t);

  return max;
}


static task_list_item_p
get_task_item (const_tmstring name)
{
  task_list_item_p t;

  for (t = tasks; t != NULL; t = t->next)
  {
    if (cmp_tmstring (name, t->name) == 0)
      return t;
  }

  return NULL;
}

static task_list_item_p
add_task_item (tmstring name)
{
  task_list_item_p t = (task_list_item_p)
                       malloc (sizeof(struct str_task_list_item));

  t->name = name;
  t->nr_cardinals = cardinals.size();
  t->card = (cardinal) malloc (sizeof(union u_cardinal));

  if (t->nr_cardinals == 0)
  {
    t->card->label = -1;
  }
  else
  {
    uint i;

    t->card->te.size = cardinals.get_bound_i (0);
    t->card->te.card = (cardinal*)
                       malloc (t->card->te.size * sizeof(cardinal));
    for (i = 0; i < t->card->te.size; i++)
    {
      t->card->te.card[i] = NULL;
    }
  }

  t->next = tasks;

  tasks = t;
  return t;
}

static void
set_task_label_ (cardinal c, uint idx, uint nr_card, int label)
{
  assert (idx <= nr_card);

  if (idx == nr_card)
  {
    c->label = label;
    return;
  }
  else
  {
    uint card_value = cardinals.get_value_i (idx);

    assert (card_value < c->te.size);

    if (c->te.card[card_value] == NULL)
    {
      uint i;

      c->te.card[card_value] = (cardinal) malloc (sizeof(union u_cardinal));

      if ((idx + 1) == nr_card)
      {
        c->te.card[card_value]->label = -1;
      }
      else
      {
        uint size = cardinals.get_bound_i (idx + 1);
        c->te.card[card_value]->te.size = size;
        c->te.card[card_value]->te.card = (cardinal*)
                                             malloc (size*sizeof(cardinal));

        for (i = 0; i < size; i++)
        {
          c->te.card[card_value]->te.card[i] = NULL;
        }
      }
    }

    set_task_label_ (c->te.card[card_value], idx + 1, nr_card, label);
  }
}

static int
get_task_label_ (cardinal c, uint idx, uint nr_card, uint *card)
{
  assert (idx <= nr_card);

  if (idx == nr_card)
  {
    return c->label;
  }
  else
  {
    uint card_value = card[idx];
    assert (card_value < c->te.size);
    assert (c->te.card[card_value] != NULL);

    return get_task_label_ (c->te.card[card_value], idx + 1, nr_card, card);
  }
}

static void
print_task_items (cardinal c, uint idx, uint nr_card, m_string m_str)
{
  m_string aux_m_str;
  uint i;

  if (c == NULL)
    return;

  assert (idx <= nr_card);

  if (idx == nr_card)
  {
    print_m_string (m_str);
    fprintf (outfile, "] - T%d\n", c->label);
    return;
  }


  aux_m_str = get_new_m_string();

  for (i = 0; i < c->te.size; i++)
  {
    copy_m_string (aux_m_str, m_str);
    append_int (aux_m_str, i);
    if ((idx + 1) < nr_card)
      append_char (aux_m_str, ',');

    print_task_items (c->te.card[i], idx + 1, nr_card, aux_m_str);
  }

  free_m_string (aux_m_str);  
}

