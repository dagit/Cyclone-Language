#ifndef _BITVEC_H_
#define _BITVEC_H_
#include "list.h"
#include "dict.h"

namespace Bitvec {
typedef int ?bitvec_t;
extern bool get(bitvec_t, int);
extern void set(bitvec_t, int);
extern void clear(bitvec_t, int);
extern bool get_and_set(bitvec_t, int);
extern void union_two(bitvec_t dest, bitvec_t src1, bitvec_t src2);
extern void intersect_two(bitvec_t dest, bitvec_t src1, bitvec_t src2);
extern void diff_two(bitvec_t dest, bitvec_t src1, bitvec_t src2);
extern bool compare_two(bitvec_t src1, bitvec_t src2);
extern bitvec_t new_empty(int);
extern bitvec_t new_full(int);
extern bitvec_t new_copy(bitvec_t);
extern bitvec_t from_list<`a,`b>(Dict::Dict<`a,`b> d, int f(`b), 
			       int sz, List::list<`a>);
extern List::list<int> to_sorted_list(bitvec_t bvec, int sz);
extern void clear_all(bitvec_t);
extern void set_all(bitvec_t);
extern bool all_set(bitvec_t bvec, int sz);
extern void print_bvec(bitvec_t bvec);
}
#endif
