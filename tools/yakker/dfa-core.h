#ifndef DFACORE_H_
#define DFACORE_H_

#include "semiring.h"


#define iterator(x) (Set::make_iter(Core::heap_region,x))
#define dict_iter(x) (Dict::make_iter(Core::heap_region,x))

int att_cmp(att_t a,att_t b);


struct span {
  st_t  target;
  act_t ub;
  Semiring::weight_t w;
};
typedef struct span @span_t;

/* Transitions.  lb is the implicit lower bound for the first span,
   the implicit lb for subsequent spans is given by the ub of the
   previous span. */
struct trans {
  act_t                lb;
  List::list_t<span_t> spans;
};
typedef struct trans @trans_t;
typedef struct trans *trans_opt_t;

struct DFA {
  unsigned int number_of_states;
  Hashtable::table_t<st_t,aset_t> final_attrs; // for final states only
  Hashtable::table_t<st_t,Semiring::weight_t@`H> final_weights; // for final states only
  Axarray::xarray_t<trans_opt_t> states;

  Set::set_t<st_t> final_states; // this could be calculated from final_attrs or final_weights.

  //  Hashtable::table_t<act_t,str_t> act_map;

  // For compression
  // if (ranges) then actions in the transitions are translated
  // as follows:
  // suppose there are n actions numbered 0 to n-1.
  // then ranges has n+1 elements and
  // action 0 is translated to [ range[0],range[1] )
  // ...
  // action n-1 is translated to [ range[n-1],range[n] )
  unsigned int ?ranges;
  unsigned int number_of_actions; // numelts(ranges)-1
};

extern span_t new_span(st_t target, act_t ub);

extern aset_t aset_empty() ;
extern aset_t aset_insert(aset_t attrs,att_t x);
extern aset_t aset_union(aset_t attrs1,aset_t attrs2) ;
extern aset_t aset_diff(aset_t attrs1,aset_t attrs2) ;
extern aset_t aset_singleton(att_t x) ;

#endif /*DFACORE_H_*/
