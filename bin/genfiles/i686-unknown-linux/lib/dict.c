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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));extern int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*
d);extern int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern struct Cyc_Dict_Dict*
Cyc_Dict_insert_new(struct Cyc_Dict_Dict*d,void*k,void*v);extern struct Cyc_Dict_Dict*
Cyc_Dict_inserts(struct Cyc_Dict_Dict*d,struct Cyc_List_List*l);extern struct Cyc_Dict_Dict*
Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);extern struct Cyc_Dict_Dict*
Cyc_Dict_rsingleton(struct _RegionHandle*,int(*cmp)(void*,void*),void*k,void*v);
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);extern void**Cyc_Dict_rlookup_opt(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict*d,void*k,void**ans);extern void*Cyc_Dict_fold(void*(*f)(void*,
void*,void*),struct Cyc_Dict_Dict*d,void*accum);extern void*Cyc_Dict_fold_c(void*(*
f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);extern
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict*d);extern void Cyc_Dict_app_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter(
void(*f)(void*,void*),struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter_c(void(*f)(
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);extern void Cyc_Dict_iter2(void(*
f)(void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern void Cyc_Dict_iter2_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);extern void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2,void*accum);extern struct Cyc_Dict_Dict*
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
static const int Cyc_Dict_R=0;static const int Cyc_Dict_B=1;struct Cyc_Dict_T{void*
color;struct Cyc_Dict_T*left;struct Cyc_Dict_T*right;struct _tuple0 key_val;};struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
return({struct Cyc_Dict_Dict*_tmp0=_region_malloc(r,sizeof(struct Cyc_Dict_Dict));
_tmp0->rel=comp;_tmp0->r=r;_tmp0->t=0;_tmp0;});}struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*comp)(void*,void*)){return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
struct Cyc_Dict_Dict*Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,
void*),void*key,void*data){return({struct Cyc_Dict_Dict*_tmp1=_region_malloc(r,
sizeof(struct Cyc_Dict_Dict));_tmp1->rel=comp;_tmp1->r=r;_tmp1->t=({struct Cyc_Dict_T*
_tmp2=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp2->color=(void*)((void*)Cyc_Dict_B);
_tmp2->left=0;_tmp2->right=0;_tmp2->key_val=({struct _tuple0 _tmp3;_tmp3.f1=key;
_tmp3.f2=data;_tmp3;});_tmp2;});_tmp1;});}struct Cyc_Dict_Dict*Cyc_Dict_singleton(
int(*comp)(void*,void*),void*key,void*data){return Cyc_Dict_rsingleton(Cyc_Core_heap_region,
comp,key,data);}int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d){return d->t == 0;}int
Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*key){int(*_tmp4)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmp5=d->t;while(_tmp5 != 0){int _tmp6=_tmp4(key,(_tmp5->key_val).f1);
if(_tmp6 < 0){_tmp5=_tmp5->left;}else{if(_tmp6 > 0){_tmp5=_tmp5->right;}else{
return 1;}}}return 0;}void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*key){int(*
_tmp7)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmp8=d->t;while(_tmp8 != 0){int _tmp9=
_tmp7(key,(_tmp8->key_val).f1);if(_tmp9 < 0){_tmp8=_tmp8->left;}else{if(_tmp9 > 0){
_tmp8=_tmp8->right;}else{return(_tmp8->key_val).f2;}}}(int)_throw((void*)Cyc_Dict_Absent);}
struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*key){int(*
_tmpA)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmpB=d->t;while(_tmpB != 0){int _tmpC=
_tmpA(key,(_tmpB->key_val).f1);if(_tmpC < 0){_tmpB=_tmpB->left;}else{if(_tmpC > 0){
_tmpB=_tmpB->right;}else{return({struct Cyc_Core_Opt*_tmpD=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpD->v=(void*)(_tmpB->key_val).f2;_tmpD;});}}}return 0;}
void**Cyc_Dict_rlookup_opt(struct _RegionHandle*r,struct Cyc_Dict_Dict*d,void*key){
int(*_tmpE)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmpF=d->t;while(_tmpF != 0){int
_tmp10=_tmpE(key,(_tmpF->key_val).f1);if(_tmp10 < 0){_tmpF=_tmpF->left;}else{if(
_tmp10 > 0){_tmpF=_tmpF->right;}else{return({void**_tmp11=_region_malloc(r,
sizeof(void*));_tmp11[0]=(_tmpF->key_val).f2;_tmp11;});}}}return 0;}int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict*d,void*key,void**ans_place){int(*_tmp12)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmp13=d->t;while(_tmp13 != 0){int _tmp14=_tmp12(key,(_tmp13->key_val).f1);
if(_tmp14 < 0){_tmp13=_tmp13->left;}else{if(_tmp14 > 0){_tmp13=_tmp13->right;}
else{*ans_place=(_tmp13->key_val).f2;return 1;}}}return 0;}struct _tuple1{void*f1;
struct Cyc_Dict_T*f2;struct Cyc_Dict_T*f3;struct _tuple0 f4;};static struct Cyc_Dict_T*
Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){struct _tuple1 _tmp15=
quad;struct _tuple0 _tmp21;struct Cyc_Dict_T*_tmp23;struct Cyc_Dict_T*_tmp25;struct
Cyc_Dict_T _tmp27;struct _tuple0 _tmp28;struct Cyc_Dict_T*_tmp2A;struct Cyc_Dict_T*
_tmp2C;struct Cyc_Dict_T _tmp2E;struct _tuple0 _tmp2F;struct Cyc_Dict_T*_tmp31;struct
Cyc_Dict_T*_tmp33;void*_tmp35;void*_tmp37;void*_tmp39;struct _tuple0 _tmp3B;struct
Cyc_Dict_T*_tmp3D;struct Cyc_Dict_T*_tmp3F;struct Cyc_Dict_T _tmp41;struct _tuple0
_tmp42;struct Cyc_Dict_T*_tmp44;struct Cyc_Dict_T _tmp46;struct _tuple0 _tmp47;struct
Cyc_Dict_T*_tmp49;struct Cyc_Dict_T*_tmp4B;void*_tmp4D;struct Cyc_Dict_T*_tmp4F;
void*_tmp51;void*_tmp53;struct _tuple0 _tmp55;struct Cyc_Dict_T*_tmp57;struct Cyc_Dict_T
_tmp59;struct _tuple0 _tmp5A;struct Cyc_Dict_T*_tmp5C;struct Cyc_Dict_T*_tmp5E;
struct Cyc_Dict_T _tmp60;struct _tuple0 _tmp61;struct Cyc_Dict_T*_tmp63;struct Cyc_Dict_T*
_tmp65;void*_tmp67;void*_tmp69;struct Cyc_Dict_T*_tmp6B;void*_tmp6D;struct _tuple0
_tmp6F;struct Cyc_Dict_T*_tmp71;struct Cyc_Dict_T _tmp73;struct _tuple0 _tmp74;struct
Cyc_Dict_T*_tmp76;struct Cyc_Dict_T _tmp78;struct _tuple0 _tmp79;struct Cyc_Dict_T*
_tmp7B;struct Cyc_Dict_T*_tmp7D;void*_tmp7F;struct Cyc_Dict_T*_tmp81;void*_tmp83;
struct Cyc_Dict_T*_tmp85;void*_tmp87;struct _tuple0 _tmp89;struct Cyc_Dict_T*_tmp8B;
struct Cyc_Dict_T*_tmp8D;void*_tmp8F;_LL23: _LL58: _tmp39=_tmp15.f1;if(_tmp39 == (
void*)Cyc_Dict_B){goto _LL38;}else{goto _LL25;}_LL38: _tmp25=_tmp15.f2;if(_tmp25 == 
0){goto _LL25;}else{_tmp27=*_tmp25;_LL56: _tmp37=(void*)_tmp27.color;if(_tmp37 == (
void*)Cyc_Dict_R){goto _LL45;}else{goto _LL25;}_LL45: _tmp2C=_tmp27.left;if(_tmp2C
== 0){goto _LL25;}else{_tmp2E=*_tmp2C;_LL54: _tmp35=(void*)_tmp2E.color;if(_tmp35
== (void*)Cyc_Dict_R){goto _LL52;}else{goto _LL25;}_LL52: _tmp33=_tmp2E.left;goto
_LL50;_LL50: _tmp31=_tmp2E.right;goto _LL48;_LL48: _tmp2F=_tmp2E.key_val;goto _LL43;}
_LL43: _tmp2A=_tmp27.right;goto _LL41;_LL41: _tmp28=_tmp27.key_val;goto _LL36;}_LL36:
_tmp23=_tmp15.f3;goto _LL34;_LL34: _tmp21=_tmp15.f4;goto _LL24;_LL25: _LL84: _tmp53=
_tmp15.f1;if(_tmp53 == (void*)Cyc_Dict_B){goto _LL64;}else{goto _LL27;}_LL64: _tmp3F=
_tmp15.f2;if(_tmp3F == 0){goto _LL27;}else{_tmp41=*_tmp3F;_LL82: _tmp51=(void*)
_tmp41.color;if(_tmp51 == (void*)Cyc_Dict_R){goto _LL80;}else{goto _LL27;}_LL80:
_tmp4F=_tmp41.left;goto _LL69;_LL69: _tmp44=_tmp41.right;if(_tmp44 == 0){goto _LL27;}
else{_tmp46=*_tmp44;_LL78: _tmp4D=(void*)_tmp46.color;if(_tmp4D == (void*)Cyc_Dict_R){
goto _LL76;}else{goto _LL27;}_LL76: _tmp4B=_tmp46.left;goto _LL74;_LL74: _tmp49=
_tmp46.right;goto _LL72;_LL72: _tmp47=_tmp46.key_val;goto _LL67;}_LL67: _tmp42=
_tmp41.key_val;goto _LL62;}_LL62: _tmp3D=_tmp15.f3;goto _LL60;_LL60: _tmp3B=_tmp15.f4;
goto _LL26;_LL27: _LL110: _tmp6D=_tmp15.f1;if(_tmp6D == (void*)Cyc_Dict_B){goto
_LL108;}else{goto _LL29;}_LL108: _tmp6B=_tmp15.f2;goto _LL88;_LL88: _tmp57=_tmp15.f3;
if(_tmp57 == 0){goto _LL29;}else{_tmp59=*_tmp57;_LL106: _tmp69=(void*)_tmp59.color;
if(_tmp69 == (void*)Cyc_Dict_R){goto _LL95;}else{goto _LL29;}_LL95: _tmp5E=_tmp59.left;
if(_tmp5E == 0){goto _LL29;}else{_tmp60=*_tmp5E;_LL104: _tmp67=(void*)_tmp60.color;
if(_tmp67 == (void*)Cyc_Dict_R){goto _LL102;}else{goto _LL29;}_LL102: _tmp65=_tmp60.left;
goto _LL100;_LL100: _tmp63=_tmp60.right;goto _LL98;_LL98: _tmp61=_tmp60.key_val;goto
_LL93;}_LL93: _tmp5C=_tmp59.right;goto _LL91;_LL91: _tmp5A=_tmp59.key_val;goto _LL86;}
_LL86: _tmp55=_tmp15.f4;goto _LL28;_LL29: _LL136: _tmp87=_tmp15.f1;if(_tmp87 == (void*)
Cyc_Dict_B){goto _LL134;}else{goto _LL31;}_LL134: _tmp85=_tmp15.f2;goto _LL114;
_LL114: _tmp71=_tmp15.f3;if(_tmp71 == 0){goto _LL31;}else{_tmp73=*_tmp71;_LL132:
_tmp83=(void*)_tmp73.color;if(_tmp83 == (void*)Cyc_Dict_R){goto _LL130;}else{goto
_LL31;}_LL130: _tmp81=_tmp73.left;goto _LL119;_LL119: _tmp76=_tmp73.right;if(_tmp76
== 0){goto _LL31;}else{_tmp78=*_tmp76;_LL128: _tmp7F=(void*)_tmp78.color;if(_tmp7F
== (void*)Cyc_Dict_R){goto _LL126;}else{goto _LL31;}_LL126: _tmp7D=_tmp78.left;goto
_LL124;_LL124: _tmp7B=_tmp78.right;goto _LL122;_LL122: _tmp79=_tmp78.key_val;goto
_LL117;}_LL117: _tmp74=_tmp73.key_val;goto _LL112;}_LL112: _tmp6F=_tmp15.f4;goto
_LL30;_LL31: _LL144: _tmp8F=_tmp15.f1;goto _LL142;_LL142: _tmp8D=_tmp15.f2;goto
_LL140;_LL140: _tmp8B=_tmp15.f3;goto _LL138;_LL138: _tmp89=_tmp15.f4;goto _LL32;
_LL24: return({struct Cyc_Dict_T*_tmp91=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp91->color=(void*)((void*)Cyc_Dict_R);_tmp91->left=({struct Cyc_Dict_T*_tmp93=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp93->color=(void*)((void*)Cyc_Dict_B);
_tmp93->left=_tmp33;_tmp93->right=_tmp31;_tmp93->key_val=_tmp2F;_tmp93;});_tmp91->right=({
struct Cyc_Dict_T*_tmp92=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp92->color=(
void*)((void*)Cyc_Dict_B);_tmp92->left=_tmp2A;_tmp92->right=_tmp23;_tmp92->key_val=
_tmp21;_tmp92;});_tmp91->key_val=_tmp28;_tmp91;});_LL26: return({struct Cyc_Dict_T*
_tmp94=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp94->color=(void*)((void*)
Cyc_Dict_R);_tmp94->left=({struct Cyc_Dict_T*_tmp96=_region_malloc(r,sizeof(
struct Cyc_Dict_T));_tmp96->color=(void*)((void*)Cyc_Dict_B);_tmp96->left=_tmp4F;
_tmp96->right=_tmp4B;_tmp96->key_val=_tmp42;_tmp96;});_tmp94->right=({struct Cyc_Dict_T*
_tmp95=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp95->color=(void*)((void*)
Cyc_Dict_B);_tmp95->left=_tmp49;_tmp95->right=_tmp3D;_tmp95->key_val=_tmp3B;
_tmp95;});_tmp94->key_val=_tmp47;_tmp94;});_LL28: return({struct Cyc_Dict_T*_tmp97=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp97->color=(void*)((void*)Cyc_Dict_R);
_tmp97->left=({struct Cyc_Dict_T*_tmp99=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmp99->color=(void*)((void*)Cyc_Dict_B);_tmp99->left=_tmp6B;_tmp99->right=
_tmp65;_tmp99->key_val=_tmp55;_tmp99;});_tmp97->right=({struct Cyc_Dict_T*_tmp98=
_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp98->color=(void*)((void*)Cyc_Dict_B);
_tmp98->left=_tmp63;_tmp98->right=_tmp5C;_tmp98->key_val=_tmp5A;_tmp98;});_tmp97->key_val=
_tmp61;_tmp97;});_LL30: return({struct Cyc_Dict_T*_tmp9A=_region_malloc(r,sizeof(
struct Cyc_Dict_T));_tmp9A->color=(void*)((void*)Cyc_Dict_R);_tmp9A->left=({
struct Cyc_Dict_T*_tmp9C=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp9C->color=(
void*)((void*)Cyc_Dict_B);_tmp9C->left=_tmp85;_tmp9C->right=_tmp81;_tmp9C->key_val=
_tmp6F;_tmp9C;});_tmp9A->right=({struct Cyc_Dict_T*_tmp9B=_region_malloc(r,
sizeof(struct Cyc_Dict_T));_tmp9B->color=(void*)((void*)Cyc_Dict_B);_tmp9B->left=
_tmp7D;_tmp9B->right=_tmp7B;_tmp9B->key_val=_tmp79;_tmp9B;});_tmp9A->key_val=
_tmp74;_tmp9A;});_LL32: return({struct Cyc_Dict_T*_tmp9D=_region_malloc(r,sizeof(
struct Cyc_Dict_T));_tmp9D->color=(void*)_tmp8F;_tmp9D->left=_tmp8D;_tmp9D->right=
_tmp8B;_tmp9D->key_val=_tmp89;_tmp9D;});_LL22:;}static struct Cyc_Dict_T*Cyc_Dict_ins(
struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,struct Cyc_Dict_T*
t){struct Cyc_Dict_T*_tmp9E=t;struct Cyc_Dict_T _tmpA4;struct _tuple0 _tmpA5;struct
Cyc_Dict_T*_tmpA7;struct Cyc_Dict_T*_tmpA9;void*_tmpAB;_LL160: if(_tmp9E == 0){goto
_LL161;}else{goto _LL162;}_LL162: if(_tmp9E == 0){goto _LL159;}else{_tmpA4=*_tmp9E;
_LL172: _tmpAB=(void*)_tmpA4.color;goto _LL170;_LL170: _tmpA9=_tmpA4.left;goto
_LL168;_LL168: _tmpA7=_tmpA4.right;goto _LL166;_LL166: _tmpA5=_tmpA4.key_val;goto
_LL163;}_LL161: return({struct Cyc_Dict_T*_tmpAD=_region_malloc(r,sizeof(struct Cyc_Dict_T));
_tmpAD->color=(void*)((void*)Cyc_Dict_R);_tmpAD->left=0;_tmpAD->right=0;_tmpAD->key_val=
key_val;_tmpAD;});_LL163: {int _tmpAE=rel(key_val.f1,_tmpA5.f1);if(_tmpAE < 0){
return Cyc_Dict_balance(r,({struct _tuple1 _tmpAF;_tmpAF.f1=_tmpAB;_tmpAF.f2=Cyc_Dict_ins(
r,rel,key_val,_tmpA9);_tmpAF.f3=_tmpA7;_tmpAF.f4=_tmpA5;_tmpAF;}));}else{if(
_tmpAE > 0){return Cyc_Dict_balance(r,({struct _tuple1 _tmpB0;_tmpB0.f1=_tmpAB;
_tmpB0.f2=_tmpA9;_tmpB0.f3=Cyc_Dict_ins(r,rel,key_val,_tmpA7);_tmpB0.f4=_tmpA5;
_tmpB0;}));}else{return({struct Cyc_Dict_T*_tmpB1=_region_malloc(r,sizeof(struct
Cyc_Dict_T));_tmpB1->color=(void*)_tmpAB;_tmpB1->left=_tmpA9;_tmpB1->right=
_tmpA7;_tmpB1->key_val=key_val;_tmpB1;});}}}_LL159:;}struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*key,void*data){struct Cyc_Dict_T*_tmpB2=Cyc_Dict_ins(d->r,
d->rel,({struct _tuple0 _tmpB4;_tmpB4.f1=key;_tmpB4.f2=data;_tmpB4;}),d->t);(void*)(((
struct Cyc_Dict_T*)_check_null(_tmpB2))->color=(void*)((void*)Cyc_Dict_B));return({
struct Cyc_Dict_Dict*_tmpB3=_region_malloc(d->r,sizeof(struct Cyc_Dict_Dict));
_tmpB3->rel=d->rel;_tmpB3->r=d->r;_tmpB3->t=_tmpB2;_tmpB3;});}struct Cyc_Dict_Dict*
Cyc_Dict_insert_new(struct Cyc_Dict_Dict*d,void*key,void*data){if(Cyc_Dict_member(
d,key)){(int)_throw((void*)Cyc_Dict_Absent);}return Cyc_Dict_insert(d,key,data);}
struct Cyc_Dict_Dict*Cyc_Dict_inserts(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
kds){for(0;kds != 0;kds=kds->tl){d=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[
_check_known_subscript_notnull(1,0)]).f1,(((struct _tuple0*)kds->hd)[
_check_known_subscript_notnull(1,0)]).f2);}return d;}static void*Cyc_Dict_fold_tree(
void*(*f)(void*,void*,void*),struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T
_tmpB7;struct _tuple0 _tmpB8;void*_tmpBA;void*_tmpBC;struct Cyc_Dict_T*_tmpBE;
struct Cyc_Dict_T*_tmpC0;struct Cyc_Dict_T*_tmpB5=t;_tmpB7=*_tmpB5;_LL193: _tmpC0=
_tmpB7.left;goto _LL191;_LL191: _tmpBE=_tmpB7.right;goto _LL185;_LL185: _tmpB8=
_tmpB7.key_val;_LL189: _tmpBC=_tmpB8.f1;goto _LL187;_LL187: _tmpBA=_tmpB8.f2;goto
_LL182;_LL182: if(_tmpC0 != 0){accum=Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmpC0),accum);}accum=f(_tmpBC,_tmpBA,accum);if(_tmpBE != 0){accum=
Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)_check_null(_tmpBE),accum);}return accum;}
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,void*accum){
if(d->t == 0){return accum;}return Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)
_check_null(d->t),accum);}static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T _tmpC4;
struct _tuple0 _tmpC5;void*_tmpC7;void*_tmpC9;struct Cyc_Dict_T*_tmpCB;struct Cyc_Dict_T*
_tmpCD;struct Cyc_Dict_T*_tmpC2=t;_tmpC4=*_tmpC2;_LL206: _tmpCD=_tmpC4.left;goto
_LL204;_LL204: _tmpCB=_tmpC4.right;goto _LL198;_LL198: _tmpC5=_tmpC4.key_val;_LL202:
_tmpC9=_tmpC5.f1;goto _LL200;_LL200: _tmpC7=_tmpC5.f2;goto _LL195;_LL195: if(_tmpCD
!= 0){accum=Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(_tmpCD),
accum);}accum=f(env,_tmpC9,_tmpC7,accum);if(_tmpCB != 0){accum=Cyc_Dict_fold_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmpCB),accum);}return accum;}void*Cyc_Dict_fold_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum){
if(d->t == 0){return accum;}return Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)
_check_null(d->t),accum);}static void Cyc_Dict_app_tree(void*(*f)(void*,void*),
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpD1;struct _tuple0 _tmpD2;void*_tmpD4;void*
_tmpD6;struct Cyc_Dict_T*_tmpD8;struct Cyc_Dict_T*_tmpDA;struct Cyc_Dict_T*_tmpCF=t;
_tmpD1=*_tmpCF;_LL219: _tmpDA=_tmpD1.left;goto _LL217;_LL217: _tmpD8=_tmpD1.right;
goto _LL211;_LL211: _tmpD2=_tmpD1.key_val;_LL215: _tmpD6=_tmpD2.f1;goto _LL213;
_LL213: _tmpD4=_tmpD2.f2;goto _LL208;_LL208: if(_tmpDA != 0){Cyc_Dict_app_tree(f,(
struct Cyc_Dict_T*)_check_null(_tmpDA));}f(_tmpD6,_tmpD4);if(_tmpD8 != 0){Cyc_Dict_app_tree(
f,(struct Cyc_Dict_T*)_check_null(_tmpD8));}}void Cyc_Dict_app(void*(*f)(void*,
void*),struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)
_check_null(d->t));}}static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpDE;struct _tuple0 _tmpDF;void*
_tmpE1;void*_tmpE3;struct Cyc_Dict_T*_tmpE5;struct Cyc_Dict_T*_tmpE7;struct Cyc_Dict_T*
_tmpDC=t;_tmpDE=*_tmpDC;_LL232: _tmpE7=_tmpDE.left;goto _LL230;_LL230: _tmpE5=
_tmpDE.right;goto _LL224;_LL224: _tmpDF=_tmpDE.key_val;_LL228: _tmpE3=_tmpDF.f1;
goto _LL226;_LL226: _tmpE1=_tmpDF.f2;goto _LL221;_LL221: if(_tmpE7 != 0){Cyc_Dict_app_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmpE7));}f(env,_tmpE3,_tmpE1);if(_tmpE5 != 
0){Cyc_Dict_app_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(_tmpE5));}}void Cyc_Dict_app_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_app_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(d->t));}}static void Cyc_Dict_iter_tree(void(*
f)(void*,void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpEB;struct _tuple0 _tmpEC;
void*_tmpEE;void*_tmpF0;struct Cyc_Dict_T*_tmpF2;struct Cyc_Dict_T*_tmpF4;struct
Cyc_Dict_T*_tmpE9=t;_tmpEB=*_tmpE9;_LL245: _tmpF4=_tmpEB.left;goto _LL243;_LL243:
_tmpF2=_tmpEB.right;goto _LL237;_LL237: _tmpEC=_tmpEB.key_val;_LL241: _tmpF0=_tmpEC.f1;
goto _LL239;_LL239: _tmpEE=_tmpEC.f2;goto _LL234;_LL234: if(_tmpF4 != 0){Cyc_Dict_iter_tree(
f,(struct Cyc_Dict_T*)_check_null(_tmpF4));}f(_tmpF0,_tmpEE);if(_tmpF2 != 0){Cyc_Dict_iter_tree(
f,(struct Cyc_Dict_T*)_check_null(_tmpF2));}}void Cyc_Dict_iter(void(*f)(void*,
void*),struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)
_check_null(d->t));}}static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpF8;struct _tuple0 _tmpF9;void*
_tmpFB;void*_tmpFD;struct Cyc_Dict_T*_tmpFF;struct Cyc_Dict_T*_tmp101;struct Cyc_Dict_T*
_tmpF6=t;_tmpF8=*_tmpF6;_LL258: _tmp101=_tmpF8.left;goto _LL256;_LL256: _tmpFF=
_tmpF8.right;goto _LL250;_LL250: _tmpF9=_tmpF8.key_val;_LL254: _tmpFD=_tmpF9.f1;
goto _LL252;_LL252: _tmpFB=_tmpF9.f2;goto _LL247;_LL247: if(_tmp101 != 0){Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp101));}f(env,_tmpFD,_tmpFB);if(_tmpFF != 
0){Cyc_Dict_iter_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(_tmpFF));}}void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t != 0){Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(d->t));}}struct _tuple2{void(*f1)(void*,void*);
struct Cyc_Dict_Dict*f2;};static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,
void*b1){struct _tuple2 _tmp105;struct Cyc_Dict_Dict*_tmp106;void(*_tmp108)(void*,
void*);struct _tuple2*_tmp103=env;_tmp105=*_tmp103;_LL265: _tmp108=_tmp105.f1;goto
_LL263;_LL263: _tmp106=_tmp105.f2;goto _LL260;_LL260: _tmp108(b1,Cyc_Dict_lookup(
_tmp106,a));}void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict*d1,
struct Cyc_Dict_Dict*d2){struct _tuple2 _tmp10A=({struct _tuple2 _tmp10B;_tmp10B.f1=f;
_tmp10B.f2=d2;_tmp10B;});((void(*)(void(*f)(struct _tuple2*,void*,void*),struct
_tuple2*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& _tmp10A,d1);}
struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict*f2;void*f3;};
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){struct _tuple3
_tmp10E;void*_tmp10F;struct Cyc_Dict_Dict*_tmp111;void(*_tmp113)(void*,void*,void*);
struct _tuple3*_tmp10C=env;_tmp10E=*_tmp10C;_LL276: _tmp113=_tmp10E.f1;goto _LL274;
_LL274: _tmp111=_tmp10E.f2;goto _LL272;_LL272: _tmp10F=_tmp10E.f3;goto _LL269;_LL269:
_tmp113(_tmp10F,b1,Cyc_Dict_lookup(_tmp111,a));}void Cyc_Dict_iter2_c(void(*f)(
void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){
struct _tuple3 _tmp115=({struct _tuple3 _tmp116;_tmp116.f1=f;_tmp116.f2=d2;_tmp116.f3=
inner_env;_tmp116;});((void(*)(void(*f)(struct _tuple3*,void*,void*),struct
_tuple3*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& _tmp115,
d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict*
f2;void*f3;};static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*
accum){struct _tuple4 _tmp119;void*_tmp11A;struct Cyc_Dict_Dict*_tmp11C;void*(*
_tmp11E)(void*,void*,void*,void*,void*);struct _tuple4*_tmp117=env;_tmp119=*
_tmp117;_LL287: _tmp11E=_tmp119.f1;goto _LL285;_LL285: _tmp11C=_tmp119.f2;goto
_LL283;_LL283: _tmp11A=_tmp119.f3;goto _LL280;_LL280: return _tmp11E(_tmp11A,a,b1,
Cyc_Dict_lookup(_tmp11C,a),accum);}void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,
void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2,
void*accum){struct _tuple4 _tmp120=({struct _tuple4 _tmp121;_tmp121.f1=f;_tmp121.f2=
d2;_tmp121.f3=inner_env;_tmp121;});return((void*(*)(void*(*f)(struct _tuple4*,
void*,void*,void*),struct _tuple4*env,struct Cyc_Dict_Dict*d,void*accum))Cyc_Dict_fold_c)(
Cyc_Dict_fold2_c_f,& _tmp120,d1,accum);}static struct Cyc_Dict_T*Cyc_Dict_copy_tree(
struct _RegionHandle*r2,struct Cyc_Dict_T*t){if(t == 0){return 0;}else{struct _tuple0
_tmp124;struct Cyc_Dict_T*_tmp126;struct Cyc_Dict_T*_tmp128;void*_tmp12A;struct Cyc_Dict_T
_tmp122=*t;_LL299: _tmp12A=(void*)_tmp122.color;goto _LL297;_LL297: _tmp128=_tmp122.left;
goto _LL295;_LL295: _tmp126=_tmp122.right;goto _LL293;_LL293: _tmp124=_tmp122.key_val;
goto _LL291;_LL291: {struct Cyc_Dict_T*_tmp12C=Cyc_Dict_copy_tree(r2,_tmp128);
struct Cyc_Dict_T*_tmp12D=Cyc_Dict_copy_tree(r2,_tmp126);return({struct Cyc_Dict_T*
_tmp12E=_region_malloc(r2,sizeof(struct Cyc_Dict_T));_tmp12E->color=(void*)
_tmp12A;_tmp12E->left=_tmp12C;_tmp12E->right=_tmp12D;_tmp12E->key_val=_tmp124;
_tmp12E;});}}}struct Cyc_Dict_Dict*Cyc_Dict_rcopy(struct _RegionHandle*r2,struct
Cyc_Dict_Dict*d){return({struct Cyc_Dict_Dict*_tmp12F=_region_malloc(r2,sizeof(
struct Cyc_Dict_Dict));_tmp12F->rel=d->rel;_tmp12F->r=r2;_tmp12F->t=Cyc_Dict_copy_tree(
r2,d->t);_tmp12F;});}struct Cyc_Dict_Dict*Cyc_Dict_copy(struct Cyc_Dict_Dict*d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree(
struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T
_tmp132;struct _tuple0 _tmp133;void*_tmp135;void*_tmp137;struct Cyc_Dict_T*_tmp139;
struct Cyc_Dict_T*_tmp13B;void*_tmp13D;struct Cyc_Dict_T*_tmp130=t;_tmp132=*
_tmp130;_LL318: _tmp13D=(void*)_tmp132.color;goto _LL316;_LL316: _tmp13B=_tmp132.left;
goto _LL314;_LL314: _tmp139=_tmp132.right;goto _LL308;_LL308: _tmp133=_tmp132.key_val;
_LL312: _tmp137=_tmp133.f1;goto _LL310;_LL310: _tmp135=_tmp133.f2;goto _LL305;_LL305: {
struct Cyc_Dict_T*_tmp13F=_tmp13B == 0? 0: Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)
_check_null(_tmp13B));void*_tmp140=f(_tmp135);struct Cyc_Dict_T*_tmp141=_tmp139 == 
0? 0: Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)_check_null(_tmp139));return({
struct Cyc_Dict_T*_tmp142=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp142->color=(
void*)_tmp13D;_tmp142->left=_tmp13F;_tmp142->right=_tmp141;_tmp142->key_val=({
struct _tuple0 _tmp143;_tmp143.f1=_tmp137;_tmp143.f2=_tmp140;_tmp143;});_tmp142;});}}
struct Cyc_Dict_Dict*Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct
Cyc_Dict_Dict*d){if(d->t == 0){return({struct Cyc_Dict_Dict*_tmp144=_region_malloc(
r,sizeof(struct Cyc_Dict_Dict));_tmp144->rel=d->rel;_tmp144->r=r;_tmp144->t=0;
_tmp144;});}return({struct Cyc_Dict_Dict*_tmp145=_region_malloc(r,sizeof(struct
Cyc_Dict_Dict));_tmp145->rel=d->rel;_tmp145->r=r;_tmp145->t=Cyc_Dict_map_tree(r,
f,(struct Cyc_Dict_T*)_check_null(d->t));_tmp145;});}struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap(Cyc_Core_heap_region,
f,d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(
void*,void*),void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp148;struct _tuple0
_tmp149;void*_tmp14B;void*_tmp14D;struct Cyc_Dict_T*_tmp14F;struct Cyc_Dict_T*
_tmp151;void*_tmp153;struct Cyc_Dict_T*_tmp146=t;_tmp148=*_tmp146;_LL340: _tmp153=(
void*)_tmp148.color;goto _LL338;_LL338: _tmp151=_tmp148.left;goto _LL336;_LL336:
_tmp14F=_tmp148.right;goto _LL330;_LL330: _tmp149=_tmp148.key_val;_LL334: _tmp14D=
_tmp149.f1;goto _LL332;_LL332: _tmp14B=_tmp149.f2;goto _LL327;_LL327: {struct Cyc_Dict_T*
_tmp155=_tmp151 == 0? 0: Cyc_Dict_map_tree_c(r,f,env,(struct Cyc_Dict_T*)_check_null(
_tmp151));void*_tmp156=f(env,_tmp14B);struct Cyc_Dict_T*_tmp157=_tmp14F == 0? 0: Cyc_Dict_map_tree_c(
r,f,env,(struct Cyc_Dict_T*)_check_null(_tmp14F));return({struct Cyc_Dict_T*
_tmp158=_region_malloc(r,sizeof(struct Cyc_Dict_T));_tmp158->color=(void*)_tmp153;
_tmp158->left=_tmp155;_tmp158->right=_tmp157;_tmp158->key_val=({struct _tuple0
_tmp159;_tmp159.f1=_tmp14D;_tmp159.f2=_tmp156;_tmp159;});_tmp158;});}}struct Cyc_Dict_Dict*
Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*
d){if(d->t == 0){return({struct Cyc_Dict_Dict*_tmp15A=_region_malloc(r,sizeof(
struct Cyc_Dict_Dict));_tmp15A->rel=d->rel;_tmp15A->r=r;_tmp15A->t=0;_tmp15A;});}
return({struct Cyc_Dict_Dict*_tmp15B=_region_malloc(r,sizeof(struct Cyc_Dict_Dict));
_tmp15B->rel=d->rel;_tmp15B->r=r;_tmp15B->t=Cyc_Dict_map_tree_c(r,f,env,(struct
Cyc_Dict_T*)_check_null(d->t));_tmp15B;});}struct Cyc_Dict_Dict*Cyc_Dict_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap_c(Cyc_Core_heap_region,
f,env,d);}struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d){if(d->t == 0){(int)_throw((void*)Cyc_Dict_Absent);}return({struct _tuple0*
_tmp15C=_region_malloc(r,sizeof(struct _tuple0));_tmp15C->f1=(((struct Cyc_Dict_T*)
_check_null(d->t))->key_val).f1;_tmp15C->f2=(((struct Cyc_Dict_T*)_check_null(d->t))->key_val).f2;
_tmp15C;});}static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp15F;struct _tuple0 _tmp160;void*_tmp162;
void*_tmp164;struct Cyc_Dict_T*_tmp166;struct Cyc_Dict_T*_tmp168;struct Cyc_Dict_T*
_tmp15D=t;_tmp15F=*_tmp15D;_LL361: _tmp168=_tmp15F.left;goto _LL359;_LL359: _tmp166=
_tmp15F.right;goto _LL353;_LL353: _tmp160=_tmp15F.key_val;_LL357: _tmp164=_tmp160.f1;
goto _LL355;_LL355: _tmp162=_tmp160.f2;goto _LL350;_LL350: return((_tmp168 == 0? 1: Cyc_Dict_forall_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp168)))? f(env,_tmp164,_tmp162): 0)?
_tmp166 == 0? 1: Cyc_Dict_forall_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(
_tmp166)): 0;}int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d){if(d->t == 0){return 1;}return Cyc_Dict_forall_tree_c(f,env,(struct Cyc_Dict_T*)
_check_null(d->t));}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict*
f2;};static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
struct _tuple5 _tmp16C;struct Cyc_Dict_Dict*_tmp16D;int(*_tmp16F)(void*,void*,void*);
struct _tuple5*_tmp16A=env;_tmp16C=*_tmp16A;_LL368: _tmp16F=_tmp16C.f1;goto _LL366;
_LL366: _tmp16D=_tmp16C.f2;goto _LL363;_LL363: if(Cyc_Dict_member(_tmp16D,a)){
return _tmp16F(a,b,Cyc_Dict_lookup(_tmp16D,a));}return 1;}int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){struct
_tuple5 _tmp171=({struct _tuple5 _tmp172;_tmp172.f1=f;_tmp172.f2=d2;_tmp172;});
return((int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmp171,d1);}struct _tuple6{
void*(*f1)(void*,void*,void*,void*);void*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_union_f(
struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){if(Cyc_Dict_member(d1,a)){
void*_tmp173=Cyc_Dict_lookup(d1,a);void*_tmp174=((*env).f1)((*env).f2,a,_tmp173,
b);if(_tmp174 != _tmp173){return Cyc_Dict_insert(d1,a,_tmp174);}return d1;}return
Cyc_Dict_insert(d1,a,b);}struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){if((
int)d1 == (int)d2){return d1;}return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*
f)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,({struct _tuple6*
_tmp175=_cycalloc(sizeof(struct _tuple6));_tmp175->f1=f;_tmp175->f2=env;_tmp175;}),
d2,d1);}struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){if(d1 == d2? 1: d2->t
== 0){return d2;}{struct _tagged_arr queue=_tag_arr(({unsigned int _tmp18A=(
unsigned int)16;struct Cyc_Dict_T**_tmp18B=(struct Cyc_Dict_T**)_cycalloc(
_check_times(sizeof(struct Cyc_Dict_T*),_tmp18A));{unsigned int _tmp18C=_tmp18A;
unsigned int i;for(i=0;i < _tmp18C;i ++){_tmp18B[i]=(struct Cyc_Dict_T*)_check_null(
d2->t);}};_tmp18B;}),sizeof(struct Cyc_Dict_T*),(unsigned int)16);int ind=0;struct
Cyc_Dict_T*_tmp176=0;while(ind != - 1){struct Cyc_Dict_T _tmp179;struct _tuple0
_tmp17A;void*_tmp17C;void*_tmp17E;struct Cyc_Dict_T*_tmp180;struct Cyc_Dict_T*
_tmp182;struct Cyc_Dict_T*_tmp177=*((struct Cyc_Dict_T**)_check_unknown_subscript(
queue,sizeof(struct Cyc_Dict_T*),ind --));_tmp179=*_tmp177;_LL387: _tmp182=_tmp179.left;
goto _LL385;_LL385: _tmp180=_tmp179.right;goto _LL379;_LL379: _tmp17A=_tmp179.key_val;
_LL383: _tmp17E=_tmp17A.f1;goto _LL381;_LL381: _tmp17C=_tmp17A.f2;goto _LL376;_LL376:
if(ind + 2 >= _get_arr_size(queue,sizeof(struct Cyc_Dict_T*))){queue=({unsigned int
_tmp184=_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))* 2;struct Cyc_Dict_T**
_tmp185=(struct Cyc_Dict_T**)_cycalloc(_check_times(sizeof(struct Cyc_Dict_T*),
_tmp184));struct _tagged_arr _tmp187=_tag_arr(_tmp185,sizeof(struct Cyc_Dict_T*),
_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))* 2);{unsigned int _tmp186=_tmp184;
unsigned int i;for(i=0;i < _tmp186;i ++){_tmp185[i]=i < _get_arr_size(queue,sizeof(
struct Cyc_Dict_T*))?((struct Cyc_Dict_T**)queue.curr)[(int)i]:(struct Cyc_Dict_T*)
_check_null(d2->t);}};_tmp187;});}if(_tmp182 != 0){*((struct Cyc_Dict_T**)
_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)
_check_null(_tmp182);}if(_tmp180 != 0){*((struct Cyc_Dict_T**)
_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)
_check_null(_tmp180);}if(Cyc_Dict_member(d1,_tmp17E)){_tmp176=Cyc_Dict_ins(d2->r,
d2->rel,({struct _tuple0 _tmp188;_tmp188.f1=_tmp17E;_tmp188.f2=f(env,_tmp17E,Cyc_Dict_lookup(
d1,_tmp17E),_tmp17C);_tmp188;}),_tmp176);}}return({struct Cyc_Dict_Dict*_tmp189=
_region_malloc(d2->r,sizeof(struct Cyc_Dict_Dict));_tmp189->rel=d2->rel;_tmp189->r=
d2->r;_tmp189->t=_tmp176;_tmp189;});}}static void*Cyc_Dict_intersect_f(void*(*f)(
void*,void*,void*),void*a,void*b1,void*b2){return f(a,b1,b2);}struct Cyc_Dict_Dict*
Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){return((struct Cyc_Dict_Dict*(*)(void*(*f)(void*(*)(void*,void*,void*),void*,
void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}static struct Cyc_List_List*
Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmp18D=_region_malloc(r,sizeof(struct Cyc_List_List));
_tmp18D->hd=({struct _tuple0*_tmp18E=_region_malloc(r,sizeof(struct _tuple0));
_tmp18E->f1=k;_tmp18E->f2=v;_tmp18E;});_tmp18D->tl=accum;_tmp18D;});}struct Cyc_List_List*
Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict*d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(
Cyc_Dict_to_list_f,r,d,0);}struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict*
d){return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(
struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7 _tmp191;
struct _RegionHandle*_tmp192;int(*_tmp194)(void*,void*);struct _tuple7*_tmp18F=env;
_tmp191=*_tmp18F;_LL405: _tmp194=_tmp191.f1;goto _LL403;_LL403: _tmp192=_tmp191.f2;
goto _LL400;_LL400: return _tmp194(x,y)? Cyc_Dict_insert(acc,x,y): acc;}struct Cyc_Dict_Dict*
Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict*
d){struct _tuple7 _tmp196=({struct _tuple7 _tmp197;_tmp197.f1=f;_tmp197.f2=r2;
_tmp197;});struct Cyc_Dict_Dict*(*ff)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*)=
Cyc_Dict_filter_f;return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(
struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(ff,& _tmp196,d,Cyc_Dict_rempty(r2,d->rel));}
struct Cyc_Dict_Dict*Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict*d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,
void*,void*);void*f2;struct _RegionHandle*f3;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(
struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8 _tmp19A;
struct _RegionHandle*_tmp19B;void*_tmp19D;int(*_tmp19F)(void*,void*,void*);struct
_tuple8*_tmp198=env;_tmp19A=*_tmp198;_LL416: _tmp19F=_tmp19A.f1;goto _LL414;_LL414:
_tmp19D=_tmp19A.f2;goto _LL412;_LL412: _tmp19B=_tmp19A.f3;goto _LL409;_LL409: return
_tmp19F(_tmp19D,x,y)? Cyc_Dict_insert(acc,x,y): acc;}struct Cyc_Dict_Dict*Cyc_Dict_rfilter_c(
struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict*
d){struct _tuple8 _tmp1A1=({struct _tuple8 _tmp1A2;_tmp1A2.f1=f;_tmp1A2.f2=f_env;
_tmp1A2.f3=r2;_tmp1A2;});return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(
struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp1A1,d,Cyc_Dict_rempty(
r2,d->rel));}struct Cyc_Dict_Dict*Cyc_Dict_filter_c(int(*f)(void*,void*,void*),
void*f_env,struct Cyc_Dict_Dict*d){return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,
f,f_env,d);}static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return ! Cyc_Dict_member(d,x);}struct Cyc_Dict_Dict*Cyc_Dict_rdifference(struct
_RegionHandle*r2,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){return((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),
struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,
d2,d1);}struct Cyc_Dict_Dict*Cyc_Dict_difference(struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*
f1)(void*,void*);void*f2;};static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,
void*y){struct _tuple9 _tmp1A5;void*_tmp1A6;int(*_tmp1A8)(void*,void*);struct
_tuple9*_tmp1A3=env;_tmp1A5=*_tmp1A3;_LL425: _tmp1A8=_tmp1A5.f1;goto _LL423;_LL423:
_tmp1A6=_tmp1A5.f2;goto _LL420;_LL420: return _tmp1A8(_tmp1A6,x)!= 0;}struct Cyc_Dict_Dict*
Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict*d,void*x){if(! Cyc_Dict_member(
d,x)){return Cyc_Dict_rcopy(r2,d);}{struct _tuple9 _tmp1AA=({struct _tuple9 _tmp1AB;
_tmp1AB.f1=d->rel;_tmp1AB.f2=x;_tmp1AB;});return((struct Cyc_Dict_Dict*(*)(struct
_RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct
Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,& _tmp1AA,d);}}struct
Cyc_Dict_Dict*Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict*d,void*x){if(! Cyc_Dict_member(
d,x)){return d;}{struct _tuple9 _tmp1AC=({struct _tuple9 _tmp1AD;_tmp1AD.f1=d->rel;
_tmp1AD.f2=x;_tmp1AD;});return((struct Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,
int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict*d))
Cyc_Dict_rfilter_c)(d->r,Cyc_Dict_delete_f,& _tmp1AC,d);}}struct Cyc_Dict_Dict*Cyc_Dict_delete(
struct Cyc_Dict_Dict*d,void*x){return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}
