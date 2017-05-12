#ifndef _a_CARDINALS_H
#define _a_CARDINALS_H

#include <string>
#include <vector>
#include <map>
#include <tmc.h>

#include "defs.h"

class Card_Item
{
  public:
    string name;
    uint value;
    uint bound;

    Card_Item (void) {value = 0; bound = 0; name = "";}
    Card_Item (string n, uint b) {value = 0; bound = b; name = n;}
};

class Cardinals
{
  typedef vector<Card_Item>::iterator VI;
  typedef vector<Card_Item>::const_iterator const_VI;
  typedef map<string,uint>::iterator MI;
  typedef map<string,uint>::const_iterator const_MI;

  public: 
    void add (const_tmstring name, int bound);
    void set_value (const_tmstring name, uint x);
    uint get_value (const_tmstring name) const;
    void remove (const_tmstring name);
    bool exists (const_tmstring name) const;

    void inc_value_i (uint idx);
    uint get_value_i (uint idx) const;
    void set_value_i (uint idx, uint val);
    int get_bound_i (uint idx) const;

    uint size (void) const;
    void print (void) const;

  private:
    vector <Card_Item> cardinals;
    map <string, uint> card_idx;
};

#endif
