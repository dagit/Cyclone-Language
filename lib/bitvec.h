#ifndef _BITVEC_H_
#define _BITVEC_H_
#include "core.h"
#include "list.h"
#include "set.h"
#include "dict.h"

namespace Bitvec {
typedef int BITVEC[?];
extern bool get(BITVEC, int);
extern void set(BITVEC, int);
extern void clear(BITVEC, int);
extern bool get_and_set(BITVEC, int);
extern void union_two(BITVEC dest, BITVEC src1, BITVEC src2);
extern void intersect_two(BITVEC dest, BITVEC src1, BITVEC src2);
extern void diff_two(BITVEC dest, BITVEC src1, BITVEC src2);
extern bool compare_two(BITVEC src1, BITVEC src2);
extern BITVEC new_empty(int);
extern BITVEC new_full(int);
extern BITVEC new_copy(BITVEC);
extern BITVEC from_list<`a,`b>(Dict::Dict<`a,`b> d, int f(`b), 
			       int sz, List::list<`a>);
extern List::list<int> to_sorted_list(BITVEC bvec, int sz);
extern void clear_all(BITVEC);
extern void set_all(BITVEC);
extern bool all_set(BITVEC bvec, int sz);
extern void print_bvec(BITVEC bvec);
}
#endif
