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
  unsigned int total_bytes;
  unsigned int free_bytes;
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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);

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
extern int _throw(void * e);
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
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern unsigned char Cyc_List_Nth[8];struct Cyc_Dict_Dict;extern unsigned char
Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_rempty(
struct _RegionHandle*,int(*cmp)(void*,void*));extern int Cyc_Dict_is_empty(struct
Cyc_Dict_Dict*d);extern int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);extern
struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern
struct Cyc_Dict_Dict*Cyc_Dict_insert_new(struct Cyc_Dict_Dict*d,void*k,void*v);
extern struct Cyc_Dict_Dict*Cyc_Dict_inserts(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
l);extern struct Cyc_Dict_Dict*Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,
void*v);extern struct Cyc_Dict_Dict*Cyc_Dict_rsingleton(struct _RegionHandle*,int(*
cmp)(void*,void*),void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*
d,void*k);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,
void*k);extern void**Cyc_Dict_rlookup_opt(struct _RegionHandle*,struct Cyc_Dict_Dict*
d,void*k);extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict*d,void*k,void**ans);
extern void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,void*
accum);extern void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d,void*accum);extern void Cyc_Dict_app(void*(*f)(void*,void*),
struct Cyc_Dict_Dict*d);extern void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter2(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern void Cyc_Dict_iter2_c(void(*
f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);
extern void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2,void*accum);extern struct Cyc_Dict_Dict*
Cyc_Dict_rcopy(struct _RegionHandle*,struct Cyc_Dict_Dict*);extern struct Cyc_Dict_Dict*
Cyc_Dict_copy(struct Cyc_Dict_Dict*);extern struct Cyc_Dict_Dict*Cyc_Dict_map(void*(*
f)(void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*
Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d);extern
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);extern struct Cyc_Dict_Dict*Cyc_Dict_intersect(void*(*f)(void*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);extern int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d);extern int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct _tuple0{void*f1;void*f2;};extern struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);extern struct Cyc_List_List*
Cyc_Dict_to_list(struct Cyc_Dict_Dict*d);extern struct Cyc_List_List*Cyc_Dict_rto_list(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_filter(
int(*f)(void*,void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_rfilter(
struct _RegionHandle*,int(*f)(void*,void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*
Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);
extern struct Cyc_Dict_Dict*Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_difference(
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern struct Cyc_Dict_Dict*Cyc_Dict_rdifference(
struct _RegionHandle*,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern struct
Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);extern struct Cyc_Dict_Dict*
Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict*,void*);extern struct
Cyc_Dict_Dict*Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict*,void*);unsigned char Cyc_Dict_Absent[
11]="\000\000\000\000Absent";unsigned char Cyc_Dict_Present[12]="\000\000\000\000Present";
struct Cyc_Dict_T{void*color;struct Cyc_Dict_T*left;struct Cyc_Dict_T*right;struct
_tuple0 key_val;};struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;struct Cyc_Dict_T*t;};struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*r,
int(*comp)(void*,void*)){return({struct Cyc_Dict_Dict*_tmp0=_region_malloc(r,
sizeof(struct Cyc_Dict_Dict));_tmp0->rel=comp;_tmp0->r=r;_tmp0->t=0;_tmp0;});}
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*comp)(void*,void*)){return Cyc_Dict_rempty(
Cyc_Core_heap_region,comp);}struct Cyc_Dict_Dict*Cyc_Dict_rsingleton(struct
_RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){return({struct Cyc_Dict_Dict*
_tmp1=_region_malloc(r,sizeof(struct Cyc_Dict_Dict));_tmp1->rel=comp;_tmp1->r=r;
_tmp1->t=({struct Cyc_Dict_T*_tmp2=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp2->color=(void*)((void*)1);_tmp2->left=0;_tmp2->right=0;_tmp2->key_val=({
struct _tuple0 _tmp3;_tmp3.f1=key;_tmp3.f2=data;_tmp3;});_tmp2;});_tmp1;});}struct
Cyc_Dict_Dict*Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict*d){return d->t == 0;}int Cyc_Dict_member(struct Cyc_Dict_Dict*d,
void*key){int(*_tmp4)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmp5=d->t;while(
_tmp5 != 0){int _tmp6=_tmp4(key,(_tmp5->key_val).f1);if(_tmp6 < 0){_tmp5=_tmp5->left;}
else{if(_tmp6 > 0){_tmp5=_tmp5->right;}else{return 1;}}}return 0;}void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*key){int(*_tmp7)(void*,void*)=d->rel;struct Cyc_Dict_T*
_tmp8=d->t;while(_tmp8 != 0){int _tmp9=_tmp7(key,(_tmp8->key_val).f1);if(_tmp9 < 0){
_tmp8=_tmp8->left;}else{if(_tmp9 > 0){_tmp8=_tmp8->right;}else{return(_tmp8->key_val).f2;}}}(
int)_throw((void*)Cyc_Dict_Absent);}struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*key){int(*_tmpA)(void*,void*)=d->rel;struct Cyc_Dict_T*
_tmpB=d->t;while(_tmpB != 0){int _tmpC=_tmpA(key,(_tmpB->key_val).f1);if(_tmpC < 0){
_tmpB=_tmpB->left;}else{if(_tmpC > 0){_tmpB=_tmpB->right;}else{return({struct Cyc_Core_Opt*
_tmpD=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD->v=(void*)(_tmpB->key_val).f2;
_tmpD;});}}}return 0;}void**Cyc_Dict_rlookup_opt(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d,void*key){int(*_tmpE)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmpF=d->t;while(
_tmpF != 0){int _tmp10=_tmpE(key,(_tmpF->key_val).f1);if(_tmp10 < 0){_tmpF=_tmpF->left;}
else{if(_tmp10 > 0){_tmpF=_tmpF->right;}else{return({void**_tmp11=_region_malloc(
r,sizeof(void*));_tmp11[0]=(_tmpF->key_val).f2;_tmp11;});}}}return 0;}int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict*d,void*key,void**ans_place){int(*_tmp12)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmp13=d->t;while(_tmp13 != 0){int _tmp14=_tmp12(key,(_tmp13->key_val).f1);
if(_tmp14 < 0){_tmp13=_tmp13->left;}else{if(_tmp14 > 0){_tmp13=_tmp13->right;}
else{*ans_place=(_tmp13->key_val).f2;return 1;}}}return 0;}struct _tuple1{void*f1;
struct Cyc_Dict_T*f2;struct Cyc_Dict_T*f3;struct _tuple0 f4;};static struct Cyc_Dict_T*
Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){struct _tuple1 _tmp15=
quad;struct _tuple0 _tmp16;struct Cyc_Dict_T*_tmp17;struct Cyc_Dict_T*_tmp18;struct
Cyc_Dict_T _tmp19;struct _tuple0 _tmp1A;struct Cyc_Dict_T*_tmp1B;struct Cyc_Dict_T*
_tmp1C;struct Cyc_Dict_T _tmp1D;struct _tuple0 _tmp1E;struct Cyc_Dict_T*_tmp1F;struct
Cyc_Dict_T*_tmp20;void*_tmp21;void*_tmp22;void*_tmp23;struct _tuple0 _tmp24;struct
Cyc_Dict_T*_tmp25;struct Cyc_Dict_T*_tmp26;struct Cyc_Dict_T _tmp27;struct _tuple0
_tmp28;struct Cyc_Dict_T*_tmp29;struct Cyc_Dict_T _tmp2A;struct _tuple0 _tmp2B;struct
Cyc_Dict_T*_tmp2C;struct Cyc_Dict_T*_tmp2D;void*_tmp2E;struct Cyc_Dict_T*_tmp2F;
void*_tmp30;void*_tmp31;struct _tuple0 _tmp32;struct Cyc_Dict_T*_tmp33;struct Cyc_Dict_T
_tmp34;struct _tuple0 _tmp35;struct Cyc_Dict_T*_tmp36;struct Cyc_Dict_T*_tmp37;
struct Cyc_Dict_T _tmp38;struct _tuple0 _tmp39;struct Cyc_Dict_T*_tmp3A;struct Cyc_Dict_T*
_tmp3B;void*_tmp3C;void*_tmp3D;struct Cyc_Dict_T*_tmp3E;void*_tmp3F;struct _tuple0
_tmp40;struct Cyc_Dict_T*_tmp41;struct Cyc_Dict_T _tmp42;struct _tuple0 _tmp43;struct
Cyc_Dict_T*_tmp44;struct Cyc_Dict_T _tmp45;struct _tuple0 _tmp46;struct Cyc_Dict_T*
_tmp47;struct Cyc_Dict_T*_tmp48;void*_tmp49;struct Cyc_Dict_T*_tmp4A;void*_tmp4B;
struct Cyc_Dict_T*_tmp4C;void*_tmp4D;struct _tuple0 _tmp4E;struct Cyc_Dict_T*_tmp4F;
struct Cyc_Dict_T*_tmp50;void*_tmp51;_LL1: _LL16: _tmp23=_tmp15.f1;if((int)_tmp23 == 
1){goto _LLD;}else{goto _LL3;}_LLD: _tmp18=_tmp15.f2;if(_tmp18 == 0){goto _LL3;}else{
_tmp19=*_tmp18;_LL15: _tmp22=(void*)_tmp19.color;if((int)_tmp22 == 0){goto _LL10;}
else{goto _LL3;}_LL10: _tmp1C=_tmp19.left;if(_tmp1C == 0){goto _LL3;}else{_tmp1D=*
_tmp1C;_LL14: _tmp21=(void*)_tmp1D.color;if((int)_tmp21 == 0){goto _LL13;}else{goto
_LL3;}_LL13: _tmp20=_tmp1D.left;goto _LL12;_LL12: _tmp1F=_tmp1D.right;goto _LL11;
_LL11: _tmp1E=_tmp1D.key_val;goto _LLF;}_LLF: _tmp1B=_tmp19.right;goto _LLE;_LLE:
_tmp1A=_tmp19.key_val;goto _LLC;}_LLC: _tmp17=_tmp15.f3;goto _LLB;_LLB: _tmp16=
_tmp15.f4;goto _LL2;_LL3: _LL22: _tmp31=_tmp15.f1;if((int)_tmp31 == 1){goto _LL19;}
else{goto _LL5;}_LL19: _tmp26=_tmp15.f2;if(_tmp26 == 0){goto _LL5;}else{_tmp27=*
_tmp26;_LL21: _tmp30=(void*)_tmp27.color;if((int)_tmp30 == 0){goto _LL20;}else{goto
_LL5;}_LL20: _tmp2F=_tmp27.left;goto _LL1B;_LL1B: _tmp29=_tmp27.right;if(_tmp29 == 0){
goto _LL5;}else{_tmp2A=*_tmp29;_LL1F: _tmp2E=(void*)_tmp2A.color;if((int)_tmp2E == 
0){goto _LL1E;}else{goto _LL5;}_LL1E: _tmp2D=_tmp2A.left;goto _LL1D;_LL1D: _tmp2C=
_tmp2A.right;goto _LL1C;_LL1C: _tmp2B=_tmp2A.key_val;goto _LL1A;}_LL1A: _tmp28=
_tmp27.key_val;goto _LL18;}_LL18: _tmp25=_tmp15.f3;goto _LL17;_LL17: _tmp24=_tmp15.f4;
goto _LL4;_LL5: _LL2E: _tmp3F=_tmp15.f1;if((int)_tmp3F == 1){goto _LL2D;}else{goto
_LL7;}_LL2D: _tmp3E=_tmp15.f2;goto _LL24;_LL24: _tmp33=_tmp15.f3;if(_tmp33 == 0){
goto _LL7;}else{_tmp34=*_tmp33;_LL2C: _tmp3D=(void*)_tmp34.color;if((int)_tmp3D == 
0){goto _LL27;}else{goto _LL7;}_LL27: _tmp37=_tmp34.left;if(_tmp37 == 0){goto _LL7;}
else{_tmp38=*_tmp37;_LL2B: _tmp3C=(void*)_tmp38.color;if((int)_tmp3C == 0){goto
_LL2A;}else{goto _LL7;}_LL2A: _tmp3B=_tmp38.left;goto _LL29;_LL29: _tmp3A=_tmp38.right;
goto _LL28;_LL28: _tmp39=_tmp38.key_val;goto _LL26;}_LL26: _tmp36=_tmp34.right;goto
_LL25;_LL25: _tmp35=_tmp34.key_val;goto _LL23;}_LL23: _tmp32=_tmp15.f4;goto _LL6;
_LL7: _LL3A: _tmp4D=_tmp15.f1;if((int)_tmp4D == 1){goto _LL39;}else{goto _LL9;}_LL39:
_tmp4C=_tmp15.f2;goto _LL30;_LL30: _tmp41=_tmp15.f3;if(_tmp41 == 0){goto _LL9;}else{
_tmp42=*_tmp41;_LL38: _tmp4B=(void*)_tmp42.color;if((int)_tmp4B == 0){goto _LL37;}
else{goto _LL9;}_LL37: _tmp4A=_tmp42.left;goto _LL32;_LL32: _tmp44=_tmp42.right;if(
_tmp44 == 0){goto _LL9;}else{_tmp45=*_tmp44;_LL36: _tmp49=(void*)_tmp45.color;if((
int)_tmp49 == 0){goto _LL35;}else{goto _LL9;}_LL35: _tmp48=_tmp45.left;goto _LL34;
_LL34: _tmp47=_tmp45.right;goto _LL33;_LL33: _tmp46=_tmp45.key_val;goto _LL31;}_LL31:
_tmp43=_tmp42.key_val;goto _LL2F;}_LL2F: _tmp40=_tmp15.f4;goto _LL8;_LL9: _LL3E:
_tmp51=_tmp15.f1;goto _LL3D;_LL3D: _tmp50=_tmp15.f2;goto _LL3C;_LL3C: _tmp4F=_tmp15.f3;
goto _LL3B;_LL3B: _tmp4E=_tmp15.f4;goto _LLA;_LL2: return({struct Cyc_Dict_T*_tmp52=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp52->color=(void*)((void*)0);
_tmp52->left=({struct Cyc_Dict_T*_tmp54=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp54->color=(void*)((void*)1);_tmp54->left=_tmp20;_tmp54->right=_tmp1F;_tmp54->key_val=
_tmp1E;_tmp54;});_tmp52->right=({struct Cyc_Dict_T*_tmp53=_region_malloc(r,
sizeof(struct Cyc_Dict_T));_tmp53->color=(void*)((void*)1);_tmp53->left=_tmp1B;
_tmp53->right=_tmp17;_tmp53->key_val=_tmp16;_tmp53;});_tmp52->key_val=_tmp1A;
_tmp52;});_LL4: return({struct Cyc_Dict_T*_tmp55=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp55->color=(void*)((void*)0);_tmp55->left=({struct Cyc_Dict_T*_tmp57=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp57->color=(void*)((void*)1);
_tmp57->left=_tmp2F;_tmp57->right=_tmp2D;_tmp57->key_val=_tmp28;_tmp57;});_tmp55->right=({
struct Cyc_Dict_T*_tmp56=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp56->color=(
void*)((void*)1);_tmp56->left=_tmp2C;_tmp56->right=_tmp25;_tmp56->key_val=_tmp24;
_tmp56;});_tmp55->key_val=_tmp2B;_tmp55;});_LL6: return({struct Cyc_Dict_T*_tmp58=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp58->color=(void*)((void*)0);
_tmp58->left=({struct Cyc_Dict_T*_tmp5A=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp5A->color=(void*)((void*)1);_tmp5A->left=_tmp3E;_tmp5A->right=_tmp3B;_tmp5A->key_val=
_tmp32;_tmp5A;});_tmp58->right=({struct Cyc_Dict_T*_tmp59=_region_malloc(r,
sizeof(struct Cyc_Dict_T));_tmp59->color=(void*)((void*)1);_tmp59->left=_tmp3A;
_tmp59->right=_tmp36;_tmp59->key_val=_tmp35;_tmp59;});_tmp58->key_val=_tmp39;
_tmp58;});_LL8: return({struct Cyc_Dict_T*_tmp5B=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp5B->color=(void*)((void*)0);_tmp5B->left=({struct Cyc_Dict_T*_tmp5D=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp5D->color=(void*)((void*)1);
_tmp5D->left=_tmp4C;_tmp5D->right=_tmp4A;_tmp5D->key_val=_tmp40;_tmp5D;});_tmp5B->right=({
struct Cyc_Dict_T*_tmp5C=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp5C->color=(
void*)((void*)1);_tmp5C->left=_tmp48;_tmp5C->right=_tmp47;_tmp5C->key_val=_tmp46;
_tmp5C;});_tmp5B->key_val=_tmp43;_tmp5B;});_LLA: return({struct Cyc_Dict_T*_tmp5E=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp5E->color=(void*)_tmp51;_tmp5E->left=
_tmp50;_tmp5E->right=_tmp4F;_tmp5E->key_val=_tmp4E;_tmp5E;});_LL0:;}static struct
Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct
_tuple0 key_val,struct Cyc_Dict_T*t){struct Cyc_Dict_T*_tmp5F=t;struct Cyc_Dict_T
_tmp60;struct _tuple0 _tmp61;struct Cyc_Dict_T*_tmp62;struct Cyc_Dict_T*_tmp63;void*
_tmp64;_LL40: if(_tmp5F == 0){goto _LL41;}else{goto _LL42;}_LL42: if(_tmp5F == 0){goto
_LL3F;}else{_tmp60=*_tmp5F;_LL47: _tmp64=(void*)_tmp60.color;goto _LL46;_LL46:
_tmp63=_tmp60.left;goto _LL45;_LL45: _tmp62=_tmp60.right;goto _LL44;_LL44: _tmp61=
_tmp60.key_val;goto _LL43;}_LL41: return({struct Cyc_Dict_T*_tmp65=_region_malloc(r,
sizeof(struct Cyc_Dict_T));_tmp65->color=(void*)((void*)0);_tmp65->left=0;_tmp65->right=
0;_tmp65->key_val=key_val;_tmp65;});_LL43: {int _tmp66=rel(key_val.f1,_tmp61.f1);
if(_tmp66 < 0){return Cyc_Dict_balance(r,({struct _tuple1 _tmp67;_tmp67.f1=_tmp64;
_tmp67.f2=Cyc_Dict_ins(r,rel,key_val,_tmp63);_tmp67.f3=_tmp62;_tmp67.f4=_tmp61;
_tmp67;}));}else{if(_tmp66 > 0){return Cyc_Dict_balance(r,({struct _tuple1 _tmp68;
_tmp68.f1=_tmp64;_tmp68.f2=_tmp63;_tmp68.f3=Cyc_Dict_ins(r,rel,key_val,_tmp62);
_tmp68.f4=_tmp61;_tmp68;}));}else{return({struct Cyc_Dict_T*_tmp69=_region_malloc(
r,sizeof(struct Cyc_Dict_T));_tmp69->color=(void*)_tmp64;_tmp69->left=_tmp63;
_tmp69->right=_tmp62;_tmp69->key_val=key_val;_tmp69;});}}}_LL3F:;}struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*key,void*data){struct Cyc_Dict_T*
_tmp6A=Cyc_Dict_ins(d->r,d->rel,({struct _tuple0 _tmp6C;_tmp6C.f1=key;_tmp6C.f2=
data;_tmp6C;}),d->t);(void*)(((struct Cyc_Dict_T*)_check_null(_tmp6A))->color=(
void*)((void*)1));return({struct Cyc_Dict_Dict*_tmp6B=_region_malloc(d->r,sizeof(
struct Cyc_Dict_Dict));_tmp6B->rel=d->rel;_tmp6B->r=d->r;_tmp6B->t=_tmp6A;_tmp6B;});}
struct Cyc_Dict_Dict*Cyc_Dict_insert_new(struct Cyc_Dict_Dict*d,void*key,void*data){
if(Cyc_Dict_member(d,key)){(int)_throw((void*)Cyc_Dict_Absent);}return Cyc_Dict_insert(
d,key,data);}struct Cyc_Dict_Dict*Cyc_Dict_inserts(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_Dict_insert(d,(((struct
_tuple0*)kds->hd)[_check_known_subscript_notnull(1,0)]).f1,(((struct _tuple0*)kds->hd)[
_check_known_subscript_notnull(1,0)]).f2);}return d;}static void*Cyc_Dict_fold_tree(
void*(*f)(void*,void*,void*),struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T
_tmp6E;struct _tuple0 _tmp6F;void*_tmp70;void*_tmp71;struct Cyc_Dict_T*_tmp72;
struct Cyc_Dict_T*_tmp73;struct Cyc_Dict_T*_tmp6D=t;_tmp6E=*_tmp6D;_LL4D: _tmp73=
_tmp6E.left;goto _LL4C;_LL4C: _tmp72=_tmp6E.right;goto _LL49;_LL49: _tmp6F=_tmp6E.key_val;
_LL4B: _tmp71=_tmp6F.f1;goto _LL4A;_LL4A: _tmp70=_tmp6F.f2;goto _LL48;_LL48: if(
_tmp73 != 0){accum=Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)_check_null(_tmp73),
accum);}accum=f(_tmp71,_tmp70,accum);if(_tmp72 != 0){accum=Cyc_Dict_fold_tree(f,(
struct Cyc_Dict_T*)_check_null(_tmp72),accum);}return accum;}void*Cyc_Dict_fold(
void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,void*accum){if(d->t == 0){
return accum;}return Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)_check_null(d->t),
accum);}static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T _tmp75;struct _tuple0 _tmp76;
void*_tmp77;void*_tmp78;struct Cyc_Dict_T*_tmp79;struct Cyc_Dict_T*_tmp7A;struct
Cyc_Dict_T*_tmp74=t;_tmp75=*_tmp74;_LL53: _tmp7A=_tmp75.left;goto _LL52;_LL52:
_tmp79=_tmp75.right;goto _LL4F;_LL4F: _tmp76=_tmp75.key_val;_LL51: _tmp78=_tmp76.f1;
goto _LL50;_LL50: _tmp77=_tmp76.f2;goto _LL4E;_LL4E: if(_tmp7A != 0){accum=Cyc_Dict_fold_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp7A),accum);}accum=f(env,_tmp78,_tmp77,
accum);if(_tmp79 != 0){accum=Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)
_check_null(_tmp79),accum);}return accum;}void*Cyc_Dict_fold_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum){if(d->t == 0){
return accum;}return Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(d->t),
accum);}static void Cyc_Dict_app_tree(void*(*f)(void*,void*),struct Cyc_Dict_T*t){
struct Cyc_Dict_T _tmp7C;struct _tuple0 _tmp7D;void*_tmp7E;void*_tmp7F;struct Cyc_Dict_T*
_tmp80;struct Cyc_Dict_T*_tmp81;struct Cyc_Dict_T*_tmp7B=t;_tmp7C=*_tmp7B;_LL59:
_tmp81=_tmp7C.left;goto _LL58;_LL58: _tmp80=_tmp7C.right;goto _LL55;_LL55: _tmp7D=
_tmp7C.key_val;_LL57: _tmp7F=_tmp7D.f1;goto _LL56;_LL56: _tmp7E=_tmp7D.f2;goto _LL54;
_LL54: if(_tmp81 != 0){Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)_check_null(_tmp81));}
f(_tmp7F,_tmp7E);if(_tmp80 != 0){Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmp80));}}void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict*
d){if(d->t != 0){Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)_check_null(d->t));}}
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_T*
t){struct Cyc_Dict_T _tmp83;struct _tuple0 _tmp84;void*_tmp85;void*_tmp86;struct Cyc_Dict_T*
_tmp87;struct Cyc_Dict_T*_tmp88;struct Cyc_Dict_T*_tmp82=t;_tmp83=*_tmp82;_LL5F:
_tmp88=_tmp83.left;goto _LL5E;_LL5E: _tmp87=_tmp83.right;goto _LL5B;_LL5B: _tmp84=
_tmp83.key_val;_LL5D: _tmp86=_tmp84.f1;goto _LL5C;_LL5C: _tmp85=_tmp84.f2;goto _LL5A;
_LL5A: if(_tmp88 != 0){Cyc_Dict_app_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(
_tmp88));}f(env,_tmp86,_tmp85);if(_tmp87 != 0){Cyc_Dict_app_tree_c(f,env,(struct
Cyc_Dict_T*)_check_null(_tmp87));}}void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_app_tree_c(f,env,(struct
Cyc_Dict_T*)_check_null(d->t));}}static void Cyc_Dict_iter_tree(void(*f)(void*,
void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp8A;struct _tuple0 _tmp8B;void*
_tmp8C;void*_tmp8D;struct Cyc_Dict_T*_tmp8E;struct Cyc_Dict_T*_tmp8F;struct Cyc_Dict_T*
_tmp89=t;_tmp8A=*_tmp89;_LL65: _tmp8F=_tmp8A.left;goto _LL64;_LL64: _tmp8E=_tmp8A.right;
goto _LL61;_LL61: _tmp8B=_tmp8A.key_val;_LL63: _tmp8D=_tmp8B.f1;goto _LL62;_LL62:
_tmp8C=_tmp8B.f2;goto _LL60;_LL60: if(_tmp8F != 0){Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmp8F));}f(_tmp8D,_tmp8C);if(_tmp8E != 0){Cyc_Dict_iter_tree(f,(
struct Cyc_Dict_T*)_check_null(_tmp8E));}}void Cyc_Dict_iter(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)
_check_null(d->t));}}static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp91;struct _tuple0 _tmp92;void*
_tmp93;void*_tmp94;struct Cyc_Dict_T*_tmp95;struct Cyc_Dict_T*_tmp96;struct Cyc_Dict_T*
_tmp90=t;_tmp91=*_tmp90;_LL6B: _tmp96=_tmp91.left;goto _LL6A;_LL6A: _tmp95=_tmp91.right;
goto _LL67;_LL67: _tmp92=_tmp91.key_val;_LL69: _tmp94=_tmp92.f1;goto _LL68;_LL68:
_tmp93=_tmp92.f2;goto _LL66;_LL66: if(_tmp96 != 0){Cyc_Dict_iter_tree_c(f,env,(
struct Cyc_Dict_T*)_check_null(_tmp96));}f(env,_tmp94,_tmp93);if(_tmp95 != 0){Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp95));}}void Cyc_Dict_iter_c(void(*f)(
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(d->t));}}struct _tuple2{void(*f1)(void*,void*);
struct Cyc_Dict_Dict*f2;};static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,
void*b1){struct _tuple2 _tmp98;struct Cyc_Dict_Dict*_tmp99;void(*_tmp9A)(void*,void*);
struct _tuple2*_tmp97=env;_tmp98=*_tmp97;_LL6E: _tmp9A=_tmp98.f1;goto _LL6D;_LL6D:
_tmp99=_tmp98.f2;goto _LL6C;_LL6C: _tmp9A(b1,Cyc_Dict_lookup(_tmp99,a));}void Cyc_Dict_iter2(
void(*f)(void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){struct
_tuple2 _tmp9B=({struct _tuple2 _tmp9C;_tmp9C.f1=f;_tmp9C.f2=d2;_tmp9C;});((void(*)(
void(*f)(struct _tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& _tmp9B,d1);}struct _tuple3{void(*f1)(void*,
void*,void*);struct Cyc_Dict_Dict*f2;void*f3;};static void Cyc_Dict_iter2_c_f(
struct _tuple3*env,void*a,void*b1){struct _tuple3 _tmp9E;void*_tmp9F;struct Cyc_Dict_Dict*
_tmpA0;void(*_tmpA1)(void*,void*,void*);struct _tuple3*_tmp9D=env;_tmp9E=*_tmp9D;
_LL72: _tmpA1=_tmp9E.f1;goto _LL71;_LL71: _tmpA0=_tmp9E.f2;goto _LL70;_LL70: _tmp9F=
_tmp9E.f3;goto _LL6F;_LL6F: _tmpA1(_tmp9F,b1,Cyc_Dict_lookup(_tmpA0,a));}void Cyc_Dict_iter2_c(
void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){struct _tuple3 _tmpA2=({struct _tuple3 _tmpA3;_tmpA3.f1=f;_tmpA3.f2=d2;_tmpA3.f3=
inner_env;_tmpA3;});((void(*)(void(*f)(struct _tuple3*,void*,void*),struct _tuple3*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& _tmpA2,d1);}
struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict*f2;
void*f3;};static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*
accum){struct _tuple4 _tmpA5;void*_tmpA6;struct Cyc_Dict_Dict*_tmpA7;void*(*_tmpA8)(
void*,void*,void*,void*,void*);struct _tuple4*_tmpA4=env;_tmpA5=*_tmpA4;_LL76:
_tmpA8=_tmpA5.f1;goto _LL75;_LL75: _tmpA7=_tmpA5.f2;goto _LL74;_LL74: _tmpA6=_tmpA5.f3;
goto _LL73;_LL73: return _tmpA8(_tmpA6,a,b1,Cyc_Dict_lookup(_tmpA7,a),accum);}void*
Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct
Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2,void*accum){struct _tuple4 _tmpA9=({struct
_tuple4 _tmpAA;_tmpAA.f1=f;_tmpAA.f2=d2;_tmpAA.f3=inner_env;_tmpAA;});return((
void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct _tuple4*env,struct Cyc_Dict_Dict*
d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,& _tmpA9,d1,accum);}static
struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,struct Cyc_Dict_T*t){
if(t == 0){return 0;}else{struct _tuple0 _tmpAC;struct Cyc_Dict_T*_tmpAD;struct Cyc_Dict_T*
_tmpAE;void*_tmpAF;struct Cyc_Dict_T _tmpAB=*t;_LL7B: _tmpAF=(void*)_tmpAB.color;
goto _LL7A;_LL7A: _tmpAE=_tmpAB.left;goto _LL79;_LL79: _tmpAD=_tmpAB.right;goto _LL78;
_LL78: _tmpAC=_tmpAB.key_val;goto _LL77;_LL77: {struct Cyc_Dict_T*_tmpB0=Cyc_Dict_copy_tree(
r2,_tmpAE);struct Cyc_Dict_T*_tmpB1=Cyc_Dict_copy_tree(r2,_tmpAD);return({struct
Cyc_Dict_T*_tmpB2=_region_malloc(r2,sizeof(struct Cyc_Dict_T));_tmpB2->color=(
void*)_tmpAF;_tmpB2->left=_tmpB0;_tmpB2->right=_tmpB1;_tmpB2->key_val=_tmpAC;
_tmpB2;});}}}struct Cyc_Dict_Dict*Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict*
d){return({struct Cyc_Dict_Dict*_tmpB3=_region_malloc(r2,sizeof(struct Cyc_Dict_Dict));
_tmpB3->rel=d->rel;_tmpB3->r=r2;_tmpB3->t=Cyc_Dict_copy_tree(r2,d->t);_tmpB3;});}
struct Cyc_Dict_Dict*Cyc_Dict_copy(struct Cyc_Dict_Dict*d){return Cyc_Dict_rcopy(
Cyc_Core_heap_region,d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree(struct
_RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpB5;
struct _tuple0 _tmpB6;void*_tmpB7;void*_tmpB8;struct Cyc_Dict_T*_tmpB9;struct Cyc_Dict_T*
_tmpBA;void*_tmpBB;struct Cyc_Dict_T*_tmpB4=t;_tmpB5=*_tmpB4;_LL82: _tmpBB=(void*)
_tmpB5.color;goto _LL81;_LL81: _tmpBA=_tmpB5.left;goto _LL80;_LL80: _tmpB9=_tmpB5.right;
goto _LL7D;_LL7D: _tmpB6=_tmpB5.key_val;_LL7F: _tmpB8=_tmpB6.f1;goto _LL7E;_LL7E:
_tmpB7=_tmpB6.f2;goto _LL7C;_LL7C: {struct Cyc_Dict_T*_tmpBC=_tmpBA == 0? 0: Cyc_Dict_map_tree(
r,f,(struct Cyc_Dict_T*)_check_null(_tmpBA));void*_tmpBD=f(_tmpB7);struct Cyc_Dict_T*
_tmpBE=_tmpB9 == 0? 0: Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)_check_null(_tmpB9));
return({struct Cyc_Dict_T*_tmpBF=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmpBF->color=(void*)_tmpBB;_tmpBF->left=_tmpBC;_tmpBF->right=_tmpBE;_tmpBF->key_val=({
struct _tuple0 _tmpC0;_tmpC0.f1=_tmpB8;_tmpC0.f2=_tmpBD;_tmpC0;});_tmpBF;});}}
struct Cyc_Dict_Dict*Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct
Cyc_Dict_Dict*d){if(d->t == 0){return({struct Cyc_Dict_Dict*_tmpC1=_region_malloc(
r,sizeof(struct Cyc_Dict_Dict));_tmpC1->rel=d->rel;_tmpC1->r=r;_tmpC1->t=0;_tmpC1;});}
return({struct Cyc_Dict_Dict*_tmpC2=_region_malloc(r,sizeof(struct Cyc_Dict_Dict));
_tmpC2->rel=d->rel;_tmpC2->r=r;_tmpC2->t=Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)
_check_null(d->t));_tmpC2;});}struct Cyc_Dict_Dict*Cyc_Dict_map(void*(*f)(void*),
struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}static
struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpC4;struct _tuple0 _tmpC5;void*
_tmpC6;void*_tmpC7;struct Cyc_Dict_T*_tmpC8;struct Cyc_Dict_T*_tmpC9;void*_tmpCA;
struct Cyc_Dict_T*_tmpC3=t;_tmpC4=*_tmpC3;_LL89: _tmpCA=(void*)_tmpC4.color;goto
_LL88;_LL88: _tmpC9=_tmpC4.left;goto _LL87;_LL87: _tmpC8=_tmpC4.right;goto _LL84;
_LL84: _tmpC5=_tmpC4.key_val;_LL86: _tmpC7=_tmpC5.f1;goto _LL85;_LL85: _tmpC6=_tmpC5.f2;
goto _LL83;_LL83: {struct Cyc_Dict_T*_tmpCB=_tmpC9 == 0? 0: Cyc_Dict_map_tree_c(r,f,
env,(struct Cyc_Dict_T*)_check_null(_tmpC9));void*_tmpCC=f(env,_tmpC6);struct Cyc_Dict_T*
_tmpCD=_tmpC8 == 0? 0: Cyc_Dict_map_tree_c(r,f,env,(struct Cyc_Dict_T*)_check_null(
_tmpC8));return({struct Cyc_Dict_T*_tmpCE=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmpCE->color=(void*)_tmpCA;_tmpCE->left=_tmpCB;_tmpCE->right=_tmpCD;_tmpCE->key_val=({
struct _tuple0 _tmpCF;_tmpCF.f1=_tmpC7;_tmpCF.f2=_tmpCC;_tmpCF;});_tmpCE;});}}
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d){if(d->t == 0){return({struct Cyc_Dict_Dict*_tmpD0=
_region_malloc(r,sizeof(struct Cyc_Dict_Dict));_tmpD0->rel=d->rel;_tmpD0->r=r;
_tmpD0->t=0;_tmpD0;});}return({struct Cyc_Dict_Dict*_tmpD1=_region_malloc(r,
sizeof(struct Cyc_Dict_Dict));_tmpD1->rel=d->rel;_tmpD1->r=r;_tmpD1->t=Cyc_Dict_map_tree_c(
r,f,env,(struct Cyc_Dict_T*)_check_null(d->t));_tmpD1;});}struct Cyc_Dict_Dict*Cyc_Dict_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap_c(Cyc_Core_heap_region,
f,env,d);}struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d){if(d->t == 0){(int)_throw((void*)Cyc_Dict_Absent);}return({struct _tuple0*
_tmpD2=_region_malloc(r,sizeof(struct _tuple0));_tmpD2->f1=(((struct Cyc_Dict_T*)
_check_null(d->t))->key_val).f1;_tmpD2->f2=(((struct Cyc_Dict_T*)_check_null(d->t))->key_val).f2;
_tmpD2;});}static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpD4;struct _tuple0 _tmpD5;void*_tmpD6;void*
_tmpD7;struct Cyc_Dict_T*_tmpD8;struct Cyc_Dict_T*_tmpD9;struct Cyc_Dict_T*_tmpD3=t;
_tmpD4=*_tmpD3;_LL8F: _tmpD9=_tmpD4.left;goto _LL8E;_LL8E: _tmpD8=_tmpD4.right;goto
_LL8B;_LL8B: _tmpD5=_tmpD4.key_val;_LL8D: _tmpD7=_tmpD5.f1;goto _LL8C;_LL8C: _tmpD6=
_tmpD5.f2;goto _LL8A;_LL8A: return((_tmpD9 == 0? 1: Cyc_Dict_forall_tree_c(f,env,(
struct Cyc_Dict_T*)_check_null(_tmpD9)))? f(env,_tmpD7,_tmpD6): 0)? _tmpD8 == 0? 1: Cyc_Dict_forall_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmpD8)): 0;}int Cyc_Dict_forall_c(int(*f)(
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t == 0){return 1;}return
Cyc_Dict_forall_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(d->t));}struct
_tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict*f2;};static int Cyc_Dict_forall_intersect_f(
struct _tuple5*env,void*a,void*b){struct _tuple5 _tmpDB;struct Cyc_Dict_Dict*_tmpDC;
int(*_tmpDD)(void*,void*,void*);struct _tuple5*_tmpDA=env;_tmpDB=*_tmpDA;_LL92:
_tmpDD=_tmpDB.f1;goto _LL91;_LL91: _tmpDC=_tmpDB.f2;goto _LL90;_LL90: if(Cyc_Dict_member(
_tmpDC,a)){return _tmpDD(a,b,Cyc_Dict_lookup(_tmpDC,a));}return 1;}int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){struct
_tuple5 _tmpDE=({struct _tuple5 _tmpDF;_tmpDF.f1=f;_tmpDF.f2=d2;_tmpDF;});return((
int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmpDE,d1);}struct _tuple6{
void*(*f1)(void*,void*,void*,void*);void*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_union_f(
struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){if(Cyc_Dict_member(d1,a)){
void*_tmpE0=Cyc_Dict_lookup(d1,a);void*_tmpE1=((*env).f1)((*env).f2,a,_tmpE0,b);
if(_tmpE1 != _tmpE0){return Cyc_Dict_insert(d1,a,_tmpE1);}return d1;}return Cyc_Dict_insert(
d1,a,b);}struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){if((int)d1 == (int)
d2){return d1;}return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct
_tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,({struct _tuple6*
_tmpE2=_cycalloc(sizeof(struct _tuple6));_tmpE2->f1=f;_tmpE2->f2=env;_tmpE2;}),d2,
d1);}struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){if(d1 == d2? 1: d2->t == 0){
return d2;}{struct _tagged_arr queue=_tag_arr(({unsigned int _tmpF1=(unsigned int)16;
struct Cyc_Dict_T**_tmpF2=(struct Cyc_Dict_T**)_cycalloc(_check_times(sizeof(
struct Cyc_Dict_T*),_tmpF1));{unsigned int _tmpF3=_tmpF1;unsigned int i;for(i=0;i < 
_tmpF3;i ++){_tmpF2[i]=(struct Cyc_Dict_T*)_check_null(d2->t);}};_tmpF2;}),sizeof(
struct Cyc_Dict_T*),(unsigned int)16);int ind=0;struct Cyc_Dict_T*_tmpE3=0;while(
ind != - 1){struct Cyc_Dict_T _tmpE5;struct _tuple0 _tmpE6;void*_tmpE7;void*_tmpE8;
struct Cyc_Dict_T*_tmpE9;struct Cyc_Dict_T*_tmpEA;struct Cyc_Dict_T*_tmpE4=*((
struct Cyc_Dict_T**)_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),ind
--));_tmpE5=*_tmpE4;_LL98: _tmpEA=_tmpE5.left;goto _LL97;_LL97: _tmpE9=_tmpE5.right;
goto _LL94;_LL94: _tmpE6=_tmpE5.key_val;_LL96: _tmpE8=_tmpE6.f1;goto _LL95;_LL95:
_tmpE7=_tmpE6.f2;goto _LL93;_LL93: if(ind + 2 >= _get_arr_size(queue,sizeof(struct
Cyc_Dict_T*))){queue=({unsigned int _tmpEB=_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))
* 2;struct Cyc_Dict_T**_tmpEC=(struct Cyc_Dict_T**)_cycalloc(_check_times(sizeof(
struct Cyc_Dict_T*),_tmpEB));struct _tagged_arr _tmpEE=_tag_arr(_tmpEC,sizeof(
struct Cyc_Dict_T*),_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))* 2);{
unsigned int _tmpED=_tmpEB;unsigned int i;for(i=0;i < _tmpED;i ++){_tmpEC[i]=i < 
_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))?((struct Cyc_Dict_T**)queue.curr)[(
int)i]:(struct Cyc_Dict_T*)_check_null(d2->t);}};_tmpEE;});}if(_tmpEA != 0){*((
struct Cyc_Dict_T**)_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),++
ind))=(struct Cyc_Dict_T*)_check_null(_tmpEA);}if(_tmpE9 != 0){*((struct Cyc_Dict_T**)
_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)
_check_null(_tmpE9);}if(Cyc_Dict_member(d1,_tmpE8)){_tmpE3=Cyc_Dict_ins(d2->r,d2->rel,({
struct _tuple0 _tmpEF;_tmpEF.f1=_tmpE8;_tmpEF.f2=f(env,_tmpE8,Cyc_Dict_lookup(d1,
_tmpE8),_tmpE7);_tmpEF;}),_tmpE3);}}return({struct Cyc_Dict_Dict*_tmpF0=
_region_malloc(d2->r,sizeof(struct Cyc_Dict_Dict));_tmpF0->rel=d2->rel;_tmpF0->r=
d2->r;_tmpF0->t=_tmpE3;_tmpF0;});}}static void*Cyc_Dict_intersect_f(void*(*f)(
void*,void*,void*),void*a,void*b1,void*b2){return f(a,b1,b2);}struct Cyc_Dict_Dict*
Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){return((struct Cyc_Dict_Dict*(*)(void*(*f)(void*(*)(void*,void*,void*),void*,
void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}static struct Cyc_List_List*
Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmpF4=_region_malloc(r,sizeof(struct Cyc_List_List));
_tmpF4->hd=({struct _tuple0*_tmpF5=_region_malloc(r,sizeof(struct _tuple0));_tmpF5->f1=
k;_tmpF5->f2=v;_tmpF5;});_tmpF4->tl=accum;_tmpF4;});}struct Cyc_List_List*Cyc_Dict_rto_list(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(
Cyc_Dict_to_list_f,r,d,0);}struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict*
d){return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(
struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7 _tmpF7;
struct _RegionHandle*_tmpF8;int(*_tmpF9)(void*,void*);struct _tuple7*_tmpF6=env;
_tmpF7=*_tmpF6;_LL9B: _tmpF9=_tmpF7.f1;goto _LL9A;_LL9A: _tmpF8=_tmpF7.f2;goto _LL99;
_LL99: return _tmpF9(x,y)? Cyc_Dict_insert(acc,x,y): acc;}struct Cyc_Dict_Dict*Cyc_Dict_rfilter(
struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict*d){struct _tuple7
_tmpFA=({struct _tuple7 _tmpFB;_tmpFB.f1=f;_tmpFB.f2=r2;_tmpFB;});struct Cyc_Dict_Dict*(*
ff)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*)=Cyc_Dict_filter_f;return((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple7*,void*,void*,
struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict*
accum))Cyc_Dict_fold_c)(ff,& _tmpFA,d,Cyc_Dict_rempty(r2,d->rel));}struct Cyc_Dict_Dict*
Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict*d){return Cyc_Dict_rfilter(
Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;
struct _RegionHandle*f3;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct
_tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8 _tmpFD;struct
_RegionHandle*_tmpFE;void*_tmpFF;int(*_tmp100)(void*,void*,void*);struct _tuple8*
_tmpFC=env;_tmpFD=*_tmpFC;_LL9F: _tmp100=_tmpFD.f1;goto _LL9E;_LL9E: _tmpFF=_tmpFD.f2;
goto _LL9D;_LL9D: _tmpFE=_tmpFD.f3;goto _LL9C;_LL9C: return _tmp100(_tmpFF,x,y)? Cyc_Dict_insert(
acc,x,y): acc;}struct Cyc_Dict_Dict*Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*
f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict*d){struct _tuple8 _tmp101=({
struct _tuple8 _tmp102;_tmp102.f1=f;_tmp102.f2=f_env;_tmp102.f3=r2;_tmp102;});
return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple8*,void*,
void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict*d,struct Cyc_Dict_Dict*
accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp101,d,Cyc_Dict_rempty(r2,d->rel));}
struct Cyc_Dict_Dict*Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,
struct Cyc_Dict_Dict*d){return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){return ! Cyc_Dict_member(
d,x);}struct Cyc_Dict_Dict*Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2){return((struct Cyc_Dict_Dict*(*)(struct _RegionHandle*
r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict*
d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,d2,d1);}struct Cyc_Dict_Dict*Cyc_Dict_difference(
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){return Cyc_Dict_rdifference(Cyc_Core_heap_region,
d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};static int Cyc_Dict_delete_f(
struct _tuple9*env,void*x,void*y){struct _tuple9 _tmp104;void*_tmp105;int(*_tmp106)(
void*,void*);struct _tuple9*_tmp103=env;_tmp104=*_tmp103;_LLA2: _tmp106=_tmp104.f1;
goto _LLA1;_LLA1: _tmp105=_tmp104.f2;goto _LLA0;_LLA0: return _tmp106(_tmp105,x)!= 0;}
struct Cyc_Dict_Dict*Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict*
d,void*x){if(! Cyc_Dict_member(d,x)){return Cyc_Dict_rcopy(r2,d);}{struct _tuple9
_tmp107=({struct _tuple9 _tmp108;_tmp108.f1=d->rel;_tmp108.f2=x;_tmp108;});return((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),
struct _tuple9*f_env,struct Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,&
_tmp107,d);}}struct Cyc_Dict_Dict*Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict*d,
void*x){if(! Cyc_Dict_member(d,x)){return d;}{struct _tuple9 _tmp109=({struct _tuple9
_tmp10A;_tmp10A.f1=d->rel;_tmp10A.f2=x;_tmp10A;});return((struct Cyc_Dict_Dict*(*)(
struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,
struct Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(d->r,Cyc_Dict_delete_f,& _tmp109,d);}}
struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*d,void*x){return Cyc_Dict_rdelete(
Cyc_Core_heap_region,d,x);}
