#ifndef _BITVEC_H_
#define _BITVEC_H_
#include <list.h>
#include <dict.h>

namespace Bitvec {
typedef int ?`r1 gbitvec_t<`r1>;
typedef gbitvec_t<`H> bitvec_t;

extern bool get(gbitvec_t<`r>, int);
extern void set(gbitvec_t<`r>, int);
extern void clear(gbitvec_t<`r>, int);
extern bool get_and_set(gbitvec_t<`r>, int);
extern void union_two(gbitvec_t<`r1> dest, 
                      gbitvec_t<`r2> src1, gbitvec_t<`r3> src2);
extern void intersect_two(gbitvec_t<`r1> dest, gbitvec_t<`r2> src1, 
                          gbitvec_t<`r3> src2);
extern void diff_two(gbitvec_t<`r1> dest, 
                     gbitvec_t<`r2> src1, gbitvec_t<`r3> src2);
extern bool compare_two(gbitvec_t<`r1> src1, gbitvec_t<`r2> src2);
extern bitvec_t new_empty(int);
extern bitvec_t new_full(int);
extern bitvec_t new_copy(gbitvec_t<`r>);
extern bitvec_t from_list(Dict::hdict_t<`a,`b,`e> d,
                          int f(`b),int sz,List::glist_t<`a,`r>);
extern List::list_t<int> to_sorted_list(gbitvec_t<`r> bvec, int sz);
extern void clear_all(gbitvec_t<`r>);
extern void set_all(gbitvec_t<`r>);
extern bool all_set(gbitvec_t<`r> bvec, int sz);
extern void print_bvec(gbitvec_t<`r> bvec);
}
#endif
