// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void* e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

//// Allocation
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);extern
unsigned char Cyc_List_List_mismatch[18];void Cyc_List_app(void*(*f)(void*),struct
Cyc_List_List*x);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void
Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_singleton(
int(*cmp)(void*,void*),void*x);struct Cyc_Set_Set*Cyc_Set_from_list(int(*cmp)(
void*,void*),struct Cyc_List_List*l);struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2);struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2);struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2);int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);int Cyc_Set_equals(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);void*Cyc_Set_fold(void*(*f)(void*,
void*),struct Cyc_Set_Set*s,void*accum);void*Cyc_Set_fold_c(void*(*f)(void*,void*,
void*),void*env,struct Cyc_Set_Set*s,void*accum);void Cyc_Set_app(void*(*f)(void*),
struct Cyc_Set_Set*s);void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);void
Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s);extern
unsigned char Cyc_Set_Absent[11];void*Cyc_Set_choose(struct Cyc_Set_Set*s);struct
Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
struct Cyc_Set_Set{int(*cmp)(void*,void*);int cardinality;struct Cyc_List_List*
nodes;};struct Cyc_Set_Set*Cyc_Set_empty(int(*comp)(void*,void*)){return({struct
Cyc_Set_Set*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->cmp=comp;_tmp0->cardinality=0;
_tmp0->nodes=0;_tmp0;});}struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*
rgn,int(*comp)(void*,void*)){return({struct Cyc_Set_Set*_tmp1=_region_malloc(rgn,
sizeof(*_tmp1));_tmp1->cmp=comp;_tmp1->cardinality=0;_tmp1->nodes=0;_tmp1;});}
struct Cyc_Set_Set*Cyc_Set_singleton(int(*comp)(void*,void*),void*x){return({
struct Cyc_Set_Set*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->cmp=comp;_tmp2->cardinality=
1;_tmp2->nodes=({struct Cyc_List_List*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->hd=(
void*)x;_tmp3->tl=0;_tmp3;});_tmp2;});}int Cyc_Set_cardinality(struct Cyc_Set_Set*
s){return s->cardinality;}int Cyc_Set_is_empty(struct Cyc_Set_Set*s){return s->cardinality
== 0;}static int Cyc_Set_member_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,
void*elt){while(n != 0){int i=cmp(elt,(void*)n->hd);if(i == 0)return 1;else{if(i < 0)
return 0;else{n=n->tl;}}}return 0;}int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt){
return Cyc_Set_member_b(s->cmp,s->nodes,elt);}static struct Cyc_List_List*Cyc_Set_insert_b(
struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
if(n == 0)return({struct Cyc_List_List*_tmp4=_region_malloc(rgn,sizeof(*_tmp4));
_tmp4->hd=(void*)elt;_tmp4->tl=0;_tmp4;});else{int i=cmp(elt,(void*)n->hd);if(i < 
0)return({struct Cyc_List_List*_tmp5=_region_malloc(rgn,sizeof(*_tmp5));_tmp5->hd=(
void*)elt;_tmp5->tl=n;_tmp5;});else{struct Cyc_List_List*result=({struct Cyc_List_List*
_tmp8=_region_malloc(rgn,sizeof(*_tmp8));_tmp8->hd=(void*)((void*)n->hd);_tmp8->tl=
0;_tmp8;});struct Cyc_List_List*prev=result;n=n->tl;while(n != 0?(i=cmp((void*)n->hd,
elt))< 0: 0){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp6=_region_malloc(rgn,sizeof(*_tmp6));_tmp6->hd=(void*)((void*)n->hd);_tmp6->tl=
0;_tmp6;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;n=n->tl;}((struct
Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp7=_region_malloc(
rgn,sizeof(*_tmp7));_tmp7->hd=(void*)elt;_tmp7->tl=n;_tmp7;});return result;}}}
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt){if(Cyc_Set_member(
s,elt))return s;else{return({struct Cyc_Set_Set*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9->cmp=s->cmp;_tmp9->cardinality=s->cardinality + 1;_tmp9->nodes=Cyc_Set_insert_b(
Cyc_Core_heap_region,s->cmp,s->nodes,elt);_tmp9;});}}struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){if(Cyc_Set_member(s,elt))
return s;else{return({struct Cyc_Set_Set*_tmpA=_region_malloc(rgn,sizeof(*_tmpA));
_tmpA->cmp=s->cmp;_tmpA->cardinality=s->cardinality + 1;_tmpA->nodes=Cyc_Set_insert_b(
rgn,s->cmp,s->nodes,elt);_tmpA;});}}struct Cyc_Set_Set*Cyc_Set_union_two(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){if(s1 == s2)return s1;if(s1->cardinality == 0)
return s2;if(s2->cardinality == 0)return s1;{struct Cyc_List_List*nodes=0;int
cardinality=0;int(*comp)(void*,void*)=s1->cmp;struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;struct Cyc_List_List*curr=0;while(x1 != 0?x2 != 0:
0){int i=comp((void*)x1->hd,(void*)x2->hd);if(i == 0)x2=x2->tl;else{if(i < 0){if(
curr == 0){nodes=({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=(
void*)((void*)x1->hd);_tmpB->tl=0;_tmpB;});curr=nodes;}else{((struct Cyc_List_List*)
_check_null(curr))->tl=({struct Cyc_List_List*_tmpC=_cycalloc(sizeof(*_tmpC));
_tmpC->hd=(void*)((void*)x1->hd);_tmpC->tl=0;_tmpC;});curr=curr->tl;}x1=x1->tl;
++ cardinality;}else{if(curr == 0){nodes=({struct Cyc_List_List*_tmpD=_cycalloc(
sizeof(*_tmpD));_tmpD->hd=(void*)((void*)x2->hd);_tmpD->tl=0;_tmpD;});curr=nodes;}
else{((struct Cyc_List_List*)_check_null(curr))->tl=({struct Cyc_List_List*_tmpE=
_cycalloc(sizeof(*_tmpE));_tmpE->hd=(void*)((void*)x2->hd);_tmpE->tl=0;_tmpE;});
curr=curr->tl;}x2=x2->tl;++ cardinality;}}}if(x1 != 0){if(curr == 0)nodes=x1;else{((
struct Cyc_List_List*)_check_null(curr))->tl=x1;}cardinality +=Cyc_List_length(x1);}
else{if(x2 != 0){if(curr == 0)nodes=x2;else{((struct Cyc_List_List*)_check_null(
curr))->tl=x2;}cardinality +=Cyc_List_length(x2);}}return({struct Cyc_Set_Set*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->cmp=comp;_tmpF->cardinality=cardinality;
_tmpF->nodes=nodes;_tmpF;});}}static struct Cyc_List_List*Cyc_Set_delete_b(int(*
cmp)(void*,void*),struct Cyc_List_List*n,void*elt){if(cmp((void*)((struct Cyc_List_List*)
_check_null(n))->hd,elt)== 0)return n->tl;{struct Cyc_List_List*result=({struct Cyc_List_List*
_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=(void*)((void*)n->hd);_tmp11->tl=0;
_tmp11;});struct Cyc_List_List*prev=result;n=n->tl;while(n != 0?cmp((void*)n->hd,
elt)!= 0: 0){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=(void*)((void*)n->hd);_tmp10->tl=0;
_tmp10;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;n=n->tl;}((struct
Cyc_List_List*)_check_null(prev))->tl=((struct Cyc_List_List*)_check_null(n))->tl;
return result;}}struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return({struct Cyc_Set_Set*_tmp12=_cycalloc(sizeof(*
_tmp12));_tmp12->cmp=s->cmp;_tmp12->cardinality=s->cardinality - 1;_tmp12->nodes=
Cyc_Set_delete_b(s->cmp,s->nodes,elt);_tmp12;});else{return s;}}void*Cyc_Set_fold(
void*(*f)(void*,void*),struct Cyc_Set_Set*s,void*accum){struct Cyc_List_List*n=s->nodes;
while(n != 0){accum=f((void*)n->hd,accum);n=n->tl;}return accum;}void*Cyc_Set_fold_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_Set_Set*s,void*accum){struct Cyc_List_List*
n=s->nodes;while(n != 0){accum=f(env,(void*)n->hd,accum);n=n->tl;}return accum;}
void Cyc_Set_app(void*(*f)(void*),struct Cyc_Set_Set*s){Cyc_List_app(f,s->nodes);}
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s){Cyc_List_iter(f,s->nodes);}
void Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s){Cyc_List_iter_c(
f,env,s->nodes);}struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2){if(s1 == s2)return s1;{int(*comp)(void*,void*)=s1->cmp;struct Cyc_List_List*
x1=s1->nodes;struct Cyc_List_List*x2=s2->nodes;struct Cyc_List_List*result=0;
struct Cyc_List_List*prev=0;int card=0;if(x1 == 0)return s1;if(x2 == 0)return s2;
while(x1 != 0?x2 != 0: 0){int i=comp((void*)x1->hd,(void*)x2->hd);if(i == 0){if(
result == 0){result=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));
_tmp13->hd=(void*)((void*)x1->hd);_tmp13->tl=0;_tmp13;});prev=result;}else{((
struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*_tmp14=
_cycalloc(sizeof(*_tmp14));_tmp14->hd=(void*)((void*)x1->hd);_tmp14->tl=0;_tmp14;});
prev=prev->tl;}++ card;x1=x1->tl;x2=x2->tl;}else{if(i < 0)x1=x1->tl;else{x2=x2->tl;}}}
return({struct Cyc_Set_Set*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->cmp=comp;
_tmp15->cardinality=card;_tmp15->nodes=result;_tmp15;});}}struct Cyc_Set_Set*Cyc_Set_from_list(
int(*comp)(void*,void*),struct Cyc_List_List*x){struct Cyc_List_List*z=Cyc_List_merge_sort(
comp,x);{struct Cyc_List_List*y=z;for(0;y != 0;y=y->tl){if(y->tl != 0?comp((void*)y->hd,(
void*)((struct Cyc_List_List*)_check_null(y->tl))->hd)== 0: 0)y->tl=((struct Cyc_List_List*)
_check_null(y->tl))->tl;}}return({struct Cyc_Set_Set*_tmp16=_cycalloc(sizeof(*
_tmp16));_tmp16->cmp=comp;_tmp16->cardinality=Cyc_List_length(z);_tmp16->nodes=z;
_tmp16;});}int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){int(*
comp)(void*,void*)=s1->cmp;struct Cyc_List_List*x1=s1->nodes;struct Cyc_List_List*
x2=s2->nodes;while(1){if(x1 == 0)return 1;if(x2 == 0)return 0;{int i=comp((void*)x1->hd,(
void*)x2->hd);if(i == 0){x1=x1->tl;x2=x2->tl;}else{if(i > 0)x2=x2->tl;else{return 0;}}}}
return 1;}struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2){int(*comp)(void*,void*)=s1->cmp;struct Cyc_List_List*x1=s1->nodes;struct Cyc_List_List*
x2=s2->nodes;int card=s1->cardinality;if(x2 == 0)return s1;while(x2 != 0){void*elt=(
void*)x2->hd;if(Cyc_Set_member_b(comp,x1,elt)){-- card;x1=Cyc_Set_delete_b(comp,
x1,elt);}x2=x2->tl;}return({struct Cyc_Set_Set*_tmp17=_cycalloc(sizeof(*_tmp17));
_tmp17->cmp=comp;_tmp17->cardinality=card;_tmp17->nodes=x1;_tmp17;});}int Cyc_Set_setcmp(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){if(s1->cardinality != s2->cardinality)
return s1->cardinality - s2->cardinality;{struct Cyc_List_List*x1=s1->nodes;struct
Cyc_List_List*x2=s2->nodes;int(*cmp)(void*,void*)=s1->cmp;while(x1 != 0){int diff=
cmp((void*)x1->hd,(void*)((struct Cyc_List_List*)_check_null(x2))->hd);if(diff != 
0)return diff;x1=x1->tl;x2=x2->tl;}return 0;}}int Cyc_Set_equals(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2){return Cyc_Set_setcmp(s1,s2)== 0;}unsigned char Cyc_Set_Absent[
11]="\000\000\000\000Absent";void*Cyc_Set_choose(struct Cyc_Set_Set*s){if(s->nodes
== 0)(int)_throw((void*)Cyc_Set_Absent);return(void*)((struct Cyc_List_List*)
_check_null(s->nodes))->hd;}int Cyc_Set_iter_f(struct Cyc_List_List**elts_left,
void**dest){if(!((unsigned int)*elts_left))return 0;*dest=(void*)((struct Cyc_List_List*)
_check_null(*elts_left))->hd;*elts_left=((struct Cyc_List_List*)_check_null(*
elts_left))->tl;return 1;}struct Cyc_Iter_Iter Cyc_Set_make_iter(struct
_RegionHandle*rgn,struct Cyc_Set_Set*s){return({struct Cyc_Iter_Iter _tmp18;_tmp18.env=(
void*)({struct Cyc_List_List**_tmp19=_region_malloc(rgn,sizeof(*_tmp19));_tmp19[0]=
s->nodes;_tmp19;});_tmp18.next=(int(*)(void*env,void*dest))Cyc_Set_iter_f;_tmp18;});}
