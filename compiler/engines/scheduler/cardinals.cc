#include <assert.h>
#include <iostream>

#include "cardinals.h"


void
Cardinals::add (const_tmstring name, int bound)
{
  cardinals.push_back (Card_Item (name, bound));
  card_idx[name] = cardinals.size()-1;
}

void
Cardinals::set_value (const_tmstring name, uint x)
{
  MI i = card_idx.find (name);
  assert (i != card_idx.end());
  cardinals[i->second].value = x;
}

uint
Cardinals::get_value (const_tmstring name) const
{
  const_MI i = card_idx.find (name);
  assert (i != card_idx.end());
  return cardinals[i->second].value;
}

void
Cardinals::remove (const_tmstring name)
{
  MI i = card_idx.find (name);
  assert (i != card_idx.end());
  assert (i->second == cardinals.size()-1);
  cardinals.pop_back();
  card_idx.erase (name);
}

bool
Cardinals::exists (const_tmstring name) const
{
  return (card_idx.find(name) != card_idx.end());
}

uint
Cardinals::size (void) const
{
  return cardinals.size();
}


void
Cardinals::inc_value_i (uint idx)
{
  assert (idx < cardinals.size());
  cardinals[idx].value++;
}

uint
Cardinals::get_value_i (uint idx) const
{
  assert (idx < cardinals.size());
  return cardinals[idx].value;
}

void
Cardinals::set_value_i (uint idx, uint val)
{
  assert (idx < cardinals.size());
  cardinals[idx].value = val;
}

int
Cardinals::get_bound_i (uint idx) const
{
  assert (idx < cardinals.size());
  return cardinals[idx].bound;
}


void
Cardinals::print (void) const
{
  cout << "Cardinals: \n";

  for (const_VI i = cardinals.begin(); i != cardinals.end(); i++)
  {
      cout << i->name << '=' << i->value << ',' << i->bound << endl;
  }
}
