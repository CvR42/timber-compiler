#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "vnusctl.h"
#include "global.h"
#include "error.h"
#include "issubset.h"
#include "service.h"


/* issubset of two distrs
      \de2  re  dc  bl2 cy2 bc2 lo2
    de1\
    re      T   F   F   F   F   F  
    dc      T   T   T   T   T   T  
    bl1     T   F   x   F   F   F  
    cy1     T   F   F   x   F   F  
    bc1     T   F   F   F   x   F  
    lo1     T   F   F   F   F   x  

    x = **1 == **2
*/
static int issubset_distrs(DistrExpr de1, DistrExpr de2) {
  int res = 0;

  if (de1 == DistrExprNIL || de2 == DistrExprNIL) return res;

  if (de2->tag == TAGNewDistReplicated || de1->tag == TAGNewDistDontCare) return 1;

  return is_equivalent_DistrExpr(de1, de2);
}

static int issubset_NewOwnLocations(NewOwnLocation ol1, NewOwnLocation ol2) {
  int res = 0; 

  if (ol1 != NewOwnLocationNIL && ol2 != NewOwnLocationNIL &&
      ol1->distr != DistrExpr_listNIL && ol2->distr != DistrExpr_listNIL && 
      ol1->distr->sz == ol2->distr->sz &&
      is_equivalent_expression(ol1->proc, ol2->proc)) {

    for (unsigned int i = 0; i < ol1->distr->sz; i++) {
      if (!issubset_distrs(ol1->distr->arr[i], ol2->distr->arr[i]))
        return 0;
    }
    res = 1;
  }

  return res;
}

static int is_replicated_NewOwnLocation(NewOwnLocation ol) {
    int res = true;
    for (unsigned int i = 0; i < ol->distr->sz; i++) {
	if (ol->distr->arr[i] == DistrExprNIL ||
	    ol->distr->arr[i]->tag != TAGNewDistReplicated)
	{
	    res = false;
	}
    }
    return res;
}

/* issubset of two owners
      \oe2   re  dc lo2 ma2
    oe1\
    re        T   T   F   F
    dc        T   T   T   F
    lo1       T   T   x   F
    ma1       F   F   F   F

    x =  issubset_NewOwnLocations(lo1, lo2)
*/
int issubset_owner(OwnerExpr oe1, OwnerExpr oe2) {
  int res = 0;

  if (oe1 != OwnerExprNIL && oe2 != OwnerExprNIL &&
      oe1->tag != TAGNewOwnMap && oe2->tag != TAGNewOwnMap) {
    switch (oe1->tag) {
      case TAGNewOwnDontCare:
        res = 1;
      break;
      case TAGNewOwnReplicated:
        res = (oe2->tag != TAGNewOwnLocation ||
	       is_replicated_NewOwnLocation(to_NewOwnLocation(oe2)));
      break;
      case TAGNewOwnLocation:
        res = (oe2->tag != TAGNewOwnLocation)
            ? 1
            : issubset_NewOwnLocations(to_NewOwnLocation(oe1), to_NewOwnLocation(oe2));
      break;
      case TAGNewOwnMap:
      break;
      default:
	tm_badtag(__FILE__,__LINE__,oe1->tag);
    }
  }

  return res;
}

int issubset_owner_list(OwnerExpr_list o_l1, OwnerExpr_list o_l2) {
  int res = 0;
 
  if (o_l1 != OwnerExpr_listNIL && o_l2 != OwnerExpr_listNIL &&
      o_l1->sz !=0 && o_l2->sz !=0) {
    res = 1;
    for (unsigned int i = 0; i < o_l1->sz; i++) {
      int res1 = 0;
      for (unsigned int j = 0; j < o_l2->sz; j++) {
        if (issubset_owner(o_l1->arr[i], o_l2->arr[j])) {
          res1 = 1;
          break;
        }
      }
      if (!res1) {
        res = 0;
        break;
      }
    }
  }

  return res;
}

