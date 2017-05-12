/* $Id: mapint.h,v 1.1 2000/10/19 21:09:19 andi Exp $ */

#ifndef __AL_MAPINT_H
#define __AL_MAPINT_H

#include <vector>
#include <queue>



template <class T>
class mapint 
{
public:
  explicit mapint (unsigned int n) 
  {
    vec = new vector<val_type*> (n);
    ref = new val_type[n];
    for (unsigned int i = 0; i < n; i++)
    {
      ref[i].val = 0;
      ref[i].idx = i;
      (*vec)[i] = &ref[i];
    }
  }
  ~mapint () 
  {
    delete vec;
    delete[] ref;
  }

  T& operator[](unsigned int n) 
  {
    return ref[n].val;
  }
  const T& operator[](unsigned int n) const
  {
    return ref[n].val;
  }

  const T& top (void) const 
  { 
    return (*vec->begin())->val;
  }
  unsigned int top_ref (void) const 
  { 
    return (*vec->begin())->idx;
  }

  void make_heap (void)
  { 
    std::make_heap (vec->begin(), vec->end(), p_cmp);
  }

  void print (void)
  {
    for (unsigned int i = 0; i < vec->size(); i++)
      cout << ref[i].idx << ':' << ref[i].val << ' ';
    cout << endl;
    for (unsigned int i = 0; i < vec->size(); i++)
      cout << (*vec)[i]->idx << ':' << (*vec)[i]->val << ' ';
    cout << endl << "---" << endl;
  }


protected:
  struct val_type {T val; unsigned int idx;};
  vector<val_type*> *vec;
  val_type *ref;

  struct Compare
  {
    typedef val_type *VTP;
    bool operator()(const VTP &a, const VTP &b) const 
    {
      return a->val > b->val;
    }
  } p_cmp;
};


 /* __AL_MAPINT_H */  

#endif
