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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

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
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
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

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })
#endif

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{int __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _tagged_arr f1;};struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));
struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d);int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*
v);struct Cyc_Dict_Dict*Cyc_Dict_insert_new(struct Cyc_Dict_Dict*d,void*k,void*v);
struct Cyc_Dict_Dict*Cyc_Dict_inserts(struct Cyc_Dict_Dict*d,struct Cyc_List_List*l);
struct Cyc_Dict_Dict*Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
struct Cyc_Dict_Dict*Cyc_Dict_rsingleton(struct _RegionHandle*,int(*cmp)(void*,
void*),void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);struct
Cyc_Core_Opt*Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);void**Cyc_Dict_rlookup_opt(
struct _RegionHandle*,struct Cyc_Dict_Dict*d,void*k);int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict*d,void*k,void**ans);void*Cyc_Dict_fold(void*(*f)(void*,void*,
void*),struct Cyc_Dict_Dict*d,void*accum);void*Cyc_Dict_fold_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_app(
void*(*f)(void*,void*),struct Cyc_Dict_Dict*d);void Cyc_Dict_app_c(void*(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d);void Cyc_Dict_iter(void(*f)(void*,
void*),struct Cyc_Dict_Dict*d);void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),
void*env,struct Cyc_Dict_Dict*d);void Cyc_Dict_iter2(void(*f)(void*,void*),struct
Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);void Cyc_Dict_iter2_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);void*Cyc_Dict_fold2_c(
void*(*f)(void*,void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct
Cyc_Dict_Dict*d2,void*accum);struct Cyc_Dict_Dict*Cyc_Dict_rcopy(struct
_RegionHandle*,struct Cyc_Dict_Dict*);struct Cyc_Dict_Dict*Cyc_Dict_copy(struct Cyc_Dict_Dict*);
struct Cyc_Dict_Dict*Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict*d);struct
Cyc_Dict_Dict*Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict*
d);struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*
d);struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(void*(*
f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct
Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d);int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_List_List*Cyc_Dict_to_list(
struct Cyc_Dict_Dict*d);struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter(int(*f)(void*,void*),
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_rfilter(struct _RegionHandle*,
int(*f)(void*,void*),struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*Cyc_Dict_difference(struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*Cyc_Dict_rdifference(struct
_RegionHandle*,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);struct Cyc_Dict_Dict*
Cyc_Dict_delete(struct Cyc_Dict_Dict*,void*);struct Cyc_Dict_Dict*Cyc_Dict_rdelete(
struct _RegionHandle*,struct Cyc_Dict_Dict*,void*);struct Cyc_Dict_Dict*Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict*,void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict*d);void*Cyc_Dict_marshal(struct
_RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),
void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,
struct Cyc_Dict_Dict*dict);struct Cyc_Dict_Dict*Cyc_Dict_unmarshal(struct
_RegionHandle*rgn,void**env,int(*cmp)(void*,void*),void*(*read_key)(void**,
struct Cyc___cycFILE*),void*(*read_val)(void**,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp);char Cyc_Dict_Absent[11]="\000\000\000\000Absent\000";char Cyc_Dict_Present[12]="\000\000\000\000Present\000";
struct Cyc_Dict_T{void*color;struct Cyc_Dict_T*left;struct Cyc_Dict_T*right;struct
_tuple0 key_val;};struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;struct Cyc_Dict_T*t;};struct Cyc_Dict_Dict*Cyc_Dict_rempty(struct _RegionHandle*r,
int(*comp)(void*,void*)){return({struct Cyc_Dict_Dict*_tmp0=_region_malloc(r,
sizeof(*_tmp0));_tmp0->rel=comp;_tmp0->r=r;_tmp0->t=0;_tmp0;});}struct Cyc_Dict_Dict*
Cyc_Dict_empty(int(*comp)(void*,void*)){return Cyc_Dict_rempty(Cyc_Core_heap_region,
comp);}struct Cyc_Dict_Dict*Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(
void*,void*),void*key,void*data){return({struct Cyc_Dict_Dict*_tmp1=
_region_malloc(r,sizeof(*_tmp1));_tmp1->rel=comp;_tmp1->r=r;_tmp1->t=({struct Cyc_Dict_T*
_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->color=(void*)((void*)1);_tmp2->left=
0;_tmp2->right=0;_tmp2->key_val=({struct _tuple0 _tmp3;_tmp3.f1=key;_tmp3.f2=data;
_tmp3;});_tmp2;});_tmp1;});}struct Cyc_Dict_Dict*Cyc_Dict_singleton(int(*comp)(
void*,void*),void*key,void*data){return Cyc_Dict_rsingleton(Cyc_Core_heap_region,
comp,key,data);}int Cyc_Dict_is_empty(struct Cyc_Dict_Dict*d){return d->t == 0;}int
Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*key){int(*_tmp4)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmp5=d->t;while(_tmp5 != 0){int _tmp6=_tmp4(key,(_tmp5->key_val).f1);
if(_tmp6 < 0)_tmp5=_tmp5->left;else{if(_tmp6 > 0)_tmp5=_tmp5->right;else{return 1;}}}
return 0;}void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*key){int(*_tmp7)(void*,
void*)=d->rel;struct Cyc_Dict_T*_tmp8=d->t;while(_tmp8 != 0){int _tmp9=_tmp7(key,(
_tmp8->key_val).f1);if(_tmp9 < 0)_tmp8=_tmp8->left;else{if(_tmp9 > 0)_tmp8=_tmp8->right;
else{return(_tmp8->key_val).f2;}}}(int)_throw((void*)Cyc_Dict_Absent);}struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*key){int(*_tmpA)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmpB=d->t;while(_tmpB != 0){int _tmpC=_tmpA(key,(_tmpB->key_val).f1);
if(_tmpC < 0)_tmpB=_tmpB->left;else{if(_tmpC > 0)_tmpB=_tmpB->right;else{return({
struct Cyc_Core_Opt*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->v=(void*)(_tmpB->key_val).f2;
_tmpD;});}}}return 0;}void**Cyc_Dict_rlookup_opt(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d,void*key){int(*_tmpE)(void*,void*)=d->rel;struct Cyc_Dict_T*_tmpF=d->t;while(
_tmpF != 0){int _tmp10=_tmpE(key,(_tmpF->key_val).f1);if(_tmp10 < 0)_tmpF=_tmpF->left;
else{if(_tmp10 > 0)_tmpF=_tmpF->right;else{return({void**_tmp11=_region_malloc(r,
sizeof(*_tmp11));_tmp11[0]=(_tmpF->key_val).f2;_tmp11;});}}}return 0;}int Cyc_Dict_lookup_bool(
struct Cyc_Dict_Dict*d,void*key,void**ans_place){int(*_tmp12)(void*,void*)=d->rel;
struct Cyc_Dict_T*_tmp13=d->t;while(_tmp13 != 0){int _tmp14=_tmp12(key,(_tmp13->key_val).f1);
if(_tmp14 < 0)_tmp13=_tmp13->left;else{if(_tmp14 > 0)_tmp13=_tmp13->right;else{*((
void**)ans_place)=(_tmp13->key_val).f2;return 1;}}}return 0;}struct _tuple1{void*f1;
struct Cyc_Dict_T*f2;struct Cyc_Dict_T*f3;struct _tuple0 f4;};static struct Cyc_Dict_T*
Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){struct _tuple1 _tmp15=
quad;void*_tmp16;struct Cyc_Dict_T*_tmp17;struct Cyc_Dict_T _tmp18;void*_tmp19;
struct Cyc_Dict_T*_tmp1A;struct Cyc_Dict_T _tmp1B;void*_tmp1C;struct Cyc_Dict_T*
_tmp1D;struct Cyc_Dict_T*_tmp1E;struct _tuple0 _tmp1F;struct Cyc_Dict_T*_tmp20;
struct _tuple0 _tmp21;struct Cyc_Dict_T*_tmp22;struct _tuple0 _tmp23;void*_tmp24;
struct Cyc_Dict_T*_tmp25;struct Cyc_Dict_T _tmp26;void*_tmp27;struct Cyc_Dict_T*
_tmp28;struct Cyc_Dict_T*_tmp29;struct Cyc_Dict_T _tmp2A;void*_tmp2B;struct Cyc_Dict_T*
_tmp2C;struct Cyc_Dict_T*_tmp2D;struct _tuple0 _tmp2E;struct _tuple0 _tmp2F;struct Cyc_Dict_T*
_tmp30;struct _tuple0 _tmp31;void*_tmp32;struct Cyc_Dict_T*_tmp33;struct Cyc_Dict_T*
_tmp34;struct Cyc_Dict_T _tmp35;void*_tmp36;struct Cyc_Dict_T*_tmp37;struct Cyc_Dict_T
_tmp38;void*_tmp39;struct Cyc_Dict_T*_tmp3A;struct Cyc_Dict_T*_tmp3B;struct _tuple0
_tmp3C;struct Cyc_Dict_T*_tmp3D;struct _tuple0 _tmp3E;struct _tuple0 _tmp3F;void*
_tmp40;struct Cyc_Dict_T*_tmp41;struct Cyc_Dict_T*_tmp42;struct Cyc_Dict_T _tmp43;
void*_tmp44;struct Cyc_Dict_T*_tmp45;struct Cyc_Dict_T*_tmp46;struct Cyc_Dict_T
_tmp47;void*_tmp48;struct Cyc_Dict_T*_tmp49;struct Cyc_Dict_T*_tmp4A;struct _tuple0
_tmp4B;struct _tuple0 _tmp4C;struct _tuple0 _tmp4D;void*_tmp4E;struct Cyc_Dict_T*
_tmp4F;struct Cyc_Dict_T*_tmp50;struct _tuple0 _tmp51;_LL1: _tmp16=_tmp15.f1;if((int)
_tmp16 != 1)goto _LL3;_tmp17=_tmp15.f2;if(_tmp17 == 0)goto _LL3;_tmp18=*_tmp17;
_tmp19=(void*)_tmp18.color;if((int)_tmp19 != 0)goto _LL3;_tmp1A=_tmp18.left;if(
_tmp1A == 0)goto _LL3;_tmp1B=*_tmp1A;_tmp1C=(void*)_tmp1B.color;if((int)_tmp1C != 0)
goto _LL3;_tmp1D=_tmp1B.left;_tmp1E=_tmp1B.right;_tmp1F=_tmp1B.key_val;_tmp20=
_tmp18.right;_tmp21=_tmp18.key_val;_tmp22=_tmp15.f3;_tmp23=_tmp15.f4;_LL2: return({
struct Cyc_Dict_T*_tmp52=_region_malloc(r,sizeof(*_tmp52));_tmp52->color=(void*)((
void*)0);_tmp52->left=({struct Cyc_Dict_T*_tmp54=_region_malloc(r,sizeof(*_tmp54));
_tmp54->color=(void*)((void*)1);_tmp54->left=_tmp1D;_tmp54->right=_tmp1E;_tmp54->key_val=
_tmp1F;_tmp54;});_tmp52->right=({struct Cyc_Dict_T*_tmp53=_region_malloc(r,
sizeof(*_tmp53));_tmp53->color=(void*)((void*)1);_tmp53->left=_tmp20;_tmp53->right=
_tmp22;_tmp53->key_val=_tmp23;_tmp53;});_tmp52->key_val=_tmp21;_tmp52;});_LL3:
_tmp24=_tmp15.f1;if((int)_tmp24 != 1)goto _LL5;_tmp25=_tmp15.f2;if(_tmp25 == 0)goto
_LL5;_tmp26=*_tmp25;_tmp27=(void*)_tmp26.color;if((int)_tmp27 != 0)goto _LL5;
_tmp28=_tmp26.left;_tmp29=_tmp26.right;if(_tmp29 == 0)goto _LL5;_tmp2A=*_tmp29;
_tmp2B=(void*)_tmp2A.color;if((int)_tmp2B != 0)goto _LL5;_tmp2C=_tmp2A.left;_tmp2D=
_tmp2A.right;_tmp2E=_tmp2A.key_val;_tmp2F=_tmp26.key_val;_tmp30=_tmp15.f3;_tmp31=
_tmp15.f4;_LL4: return({struct Cyc_Dict_T*_tmp55=_region_malloc(r,sizeof(*_tmp55));
_tmp55->color=(void*)((void*)0);_tmp55->left=({struct Cyc_Dict_T*_tmp57=
_region_malloc(r,sizeof(*_tmp57));_tmp57->color=(void*)((void*)1);_tmp57->left=
_tmp28;_tmp57->right=_tmp2C;_tmp57->key_val=_tmp2F;_tmp57;});_tmp55->right=({
struct Cyc_Dict_T*_tmp56=_region_malloc(r,sizeof(*_tmp56));_tmp56->color=(void*)((
void*)1);_tmp56->left=_tmp2D;_tmp56->right=_tmp30;_tmp56->key_val=_tmp31;_tmp56;});
_tmp55->key_val=_tmp2E;_tmp55;});_LL5: _tmp32=_tmp15.f1;if((int)_tmp32 != 1)goto
_LL7;_tmp33=_tmp15.f2;_tmp34=_tmp15.f3;if(_tmp34 == 0)goto _LL7;_tmp35=*_tmp34;
_tmp36=(void*)_tmp35.color;if((int)_tmp36 != 0)goto _LL7;_tmp37=_tmp35.left;if(
_tmp37 == 0)goto _LL7;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.color;if((int)_tmp39 != 0)
goto _LL7;_tmp3A=_tmp38.left;_tmp3B=_tmp38.right;_tmp3C=_tmp38.key_val;_tmp3D=
_tmp35.right;_tmp3E=_tmp35.key_val;_tmp3F=_tmp15.f4;_LL6: return({struct Cyc_Dict_T*
_tmp58=_region_malloc(r,sizeof(*_tmp58));_tmp58->color=(void*)((void*)0);_tmp58->left=({
struct Cyc_Dict_T*_tmp5A=_region_malloc(r,sizeof(*_tmp5A));_tmp5A->color=(void*)((
void*)1);_tmp5A->left=_tmp33;_tmp5A->right=_tmp3A;_tmp5A->key_val=_tmp3F;_tmp5A;});
_tmp58->right=({struct Cyc_Dict_T*_tmp59=_region_malloc(r,sizeof(*_tmp59));_tmp59->color=(
void*)((void*)1);_tmp59->left=_tmp3B;_tmp59->right=_tmp3D;_tmp59->key_val=_tmp3E;
_tmp59;});_tmp58->key_val=_tmp3C;_tmp58;});_LL7: _tmp40=_tmp15.f1;if((int)_tmp40
!= 1)goto _LL9;_tmp41=_tmp15.f2;_tmp42=_tmp15.f3;if(_tmp42 == 0)goto _LL9;_tmp43=*
_tmp42;_tmp44=(void*)_tmp43.color;if((int)_tmp44 != 0)goto _LL9;_tmp45=_tmp43.left;
_tmp46=_tmp43.right;if(_tmp46 == 0)goto _LL9;_tmp47=*_tmp46;_tmp48=(void*)_tmp47.color;
if((int)_tmp48 != 0)goto _LL9;_tmp49=_tmp47.left;_tmp4A=_tmp47.right;_tmp4B=_tmp47.key_val;
_tmp4C=_tmp43.key_val;_tmp4D=_tmp15.f4;_LL8: return({struct Cyc_Dict_T*_tmp5B=
_region_malloc(r,sizeof(*_tmp5B));_tmp5B->color=(void*)((void*)0);_tmp5B->left=({
struct Cyc_Dict_T*_tmp5D=_region_malloc(r,sizeof(*_tmp5D));_tmp5D->color=(void*)((
void*)1);_tmp5D->left=_tmp41;_tmp5D->right=_tmp45;_tmp5D->key_val=_tmp4D;_tmp5D;});
_tmp5B->right=({struct Cyc_Dict_T*_tmp5C=_region_malloc(r,sizeof(*_tmp5C));_tmp5C->color=(
void*)((void*)1);_tmp5C->left=_tmp49;_tmp5C->right=_tmp4A;_tmp5C->key_val=_tmp4B;
_tmp5C;});_tmp5B->key_val=_tmp4C;_tmp5B;});_LL9: _tmp4E=_tmp15.f1;_tmp4F=_tmp15.f2;
_tmp50=_tmp15.f3;_tmp51=_tmp15.f4;_LLA: return({struct Cyc_Dict_T*_tmp5E=
_region_malloc(r,sizeof(*_tmp5E));_tmp5E->color=(void*)_tmp4E;_tmp5E->left=
_tmp4F;_tmp5E->right=_tmp50;_tmp5E->key_val=_tmp51;_tmp5E;});_LL0:;}static struct
Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct
_tuple0 key_val,struct Cyc_Dict_T*t){struct Cyc_Dict_T*_tmp5F=t;struct Cyc_Dict_T
_tmp60;void*_tmp61;struct Cyc_Dict_T*_tmp62;struct Cyc_Dict_T*_tmp63;struct _tuple0
_tmp64;_LLC: if(_tmp5F != 0)goto _LLE;_LLD: return({struct Cyc_Dict_T*_tmp65=
_region_malloc(r,sizeof(*_tmp65));_tmp65->color=(void*)((void*)0);_tmp65->left=0;
_tmp65->right=0;_tmp65->key_val=key_val;_tmp65;});_LLE: if(_tmp5F == 0)goto _LLB;
_tmp60=*_tmp5F;_tmp61=(void*)_tmp60.color;_tmp62=_tmp60.left;_tmp63=_tmp60.right;
_tmp64=_tmp60.key_val;_LLF: {int _tmp66=rel(key_val.f1,_tmp64.f1);if(_tmp66 < 0)
return Cyc_Dict_balance(r,({struct _tuple1 _tmp67;_tmp67.f1=_tmp61;_tmp67.f2=Cyc_Dict_ins(
r,rel,key_val,_tmp62);_tmp67.f3=_tmp63;_tmp67.f4=_tmp64;_tmp67;}));else{if(
_tmp66 > 0)return Cyc_Dict_balance(r,({struct _tuple1 _tmp68;_tmp68.f1=_tmp61;_tmp68.f2=
_tmp62;_tmp68.f3=Cyc_Dict_ins(r,rel,key_val,_tmp63);_tmp68.f4=_tmp64;_tmp68;}));
else{return({struct Cyc_Dict_T*_tmp69=_region_malloc(r,sizeof(*_tmp69));_tmp69->color=(
void*)_tmp61;_tmp69->left=_tmp62;_tmp69->right=_tmp63;_tmp69->key_val=key_val;
_tmp69;});}}}_LLB:;}struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,
void*key,void*data){struct Cyc_Dict_T*_tmp6A=Cyc_Dict_ins(d->r,d->rel,({struct
_tuple0 _tmp6C;_tmp6C.f1=key;_tmp6C.f2=data;_tmp6C;}),d->t);(void*)(((struct Cyc_Dict_T*)
_check_null(_tmp6A))->color=(void*)((void*)1));return({struct Cyc_Dict_Dict*
_tmp6B=_region_malloc(d->r,sizeof(*_tmp6B));_tmp6B->rel=d->rel;_tmp6B->r=d->r;
_tmp6B->t=_tmp6A;_tmp6B;});}struct Cyc_Dict_Dict*Cyc_Dict_insert_new(struct Cyc_Dict_Dict*
d,void*key,void*data){if(Cyc_Dict_member(d,key))(int)_throw((void*)Cyc_Dict_Absent);
return Cyc_Dict_insert(d,key,data);}struct Cyc_Dict_Dict*Cyc_Dict_inserts(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_Dict_insert(
d,(((struct _tuple0*)kds->hd)[_check_known_subscript_notnull(1,0)]).f1,(((struct
_tuple0*)kds->hd)[_check_known_subscript_notnull(1,0)]).f2);}return d;}static void*
Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),struct Cyc_Dict_T*t,void*accum){
struct Cyc_Dict_T _tmp6E;struct Cyc_Dict_T*_tmp6F;struct Cyc_Dict_T*_tmp70;struct
_tuple0 _tmp71;void*_tmp72;void*_tmp73;struct Cyc_Dict_T*_tmp6D=t;_tmp6E=*_tmp6D;
_tmp6F=_tmp6E.left;_tmp70=_tmp6E.right;_tmp71=_tmp6E.key_val;_tmp72=_tmp71.f1;
_tmp73=_tmp71.f2;if(_tmp6F != 0)accum=Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmp6F),accum);accum=f(_tmp72,_tmp73,accum);if(_tmp70 != 0)accum=Cyc_Dict_fold_tree(
f,(struct Cyc_Dict_T*)_check_null(_tmp70),accum);return accum;}void*Cyc_Dict_fold(
void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,void*accum){if(d->t == 0)
return accum;return Cyc_Dict_fold_tree(f,(struct Cyc_Dict_T*)_check_null(d->t),
accum);}static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_T*t,void*accum){struct Cyc_Dict_T _tmp75;struct Cyc_Dict_T*
_tmp76;struct Cyc_Dict_T*_tmp77;struct _tuple0 _tmp78;void*_tmp79;void*_tmp7A;
struct Cyc_Dict_T*_tmp74=t;_tmp75=*_tmp74;_tmp76=_tmp75.left;_tmp77=_tmp75.right;
_tmp78=_tmp75.key_val;_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;if(_tmp76 != 0)accum=Cyc_Dict_fold_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp76),accum);accum=f(env,_tmp79,_tmp7A,
accum);if(_tmp77 != 0)accum=Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)
_check_null(_tmp77),accum);return accum;}void*Cyc_Dict_fold_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d,void*accum){if(d->t == 0)return
accum;return Cyc_Dict_fold_tree_c(f,env,(struct Cyc_Dict_T*)_check_null(d->t),
accum);}static void Cyc_Dict_app_tree(void*(*f)(void*,void*),struct Cyc_Dict_T*t){
struct Cyc_Dict_T _tmp7C;struct Cyc_Dict_T*_tmp7D;struct Cyc_Dict_T*_tmp7E;struct
_tuple0 _tmp7F;void*_tmp80;void*_tmp81;struct Cyc_Dict_T*_tmp7B=t;_tmp7C=*_tmp7B;
_tmp7D=_tmp7C.left;_tmp7E=_tmp7C.right;_tmp7F=_tmp7C.key_val;_tmp80=_tmp7F.f1;
_tmp81=_tmp7F.f2;if(_tmp7D != 0)Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmp7D));f(_tmp80,_tmp81);if(_tmp7E != 0)Cyc_Dict_app_tree(f,(struct
Cyc_Dict_T*)_check_null(_tmp7E));}void Cyc_Dict_app(void*(*f)(void*,void*),struct
Cyc_Dict_Dict*d){if(d->t != 0)Cyc_Dict_app_tree(f,(struct Cyc_Dict_T*)_check_null(
d->t));}static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp83;struct Cyc_Dict_T*_tmp84;struct Cyc_Dict_T*
_tmp85;struct _tuple0 _tmp86;void*_tmp87;void*_tmp88;struct Cyc_Dict_T*_tmp82=t;
_tmp83=*_tmp82;_tmp84=_tmp83.left;_tmp85=_tmp83.right;_tmp86=_tmp83.key_val;
_tmp87=_tmp86.f1;_tmp88=_tmp86.f2;if(_tmp84 != 0)Cyc_Dict_app_tree_c(f,env,(
struct Cyc_Dict_T*)_check_null(_tmp84));f(env,_tmp87,_tmp88);if(_tmp85 != 0)Cyc_Dict_app_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp85));}void Cyc_Dict_app_c(void*(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t != 0)Cyc_Dict_app_tree_c(f,
env,(struct Cyc_Dict_T*)_check_null(d->t));}static void Cyc_Dict_iter_tree(void(*f)(
void*,void*),struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp8A;struct Cyc_Dict_T*_tmp8B;
struct Cyc_Dict_T*_tmp8C;struct _tuple0 _tmp8D;void*_tmp8E;void*_tmp8F;struct Cyc_Dict_T*
_tmp89=t;_tmp8A=*_tmp89;_tmp8B=_tmp8A.left;_tmp8C=_tmp8A.right;_tmp8D=_tmp8A.key_val;
_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;if(_tmp8B != 0)Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)
_check_null(_tmp8B));f(_tmp8E,_tmp8F);if(_tmp8C != 0)Cyc_Dict_iter_tree(f,(struct
Cyc_Dict_T*)_check_null(_tmp8C));}void Cyc_Dict_iter(void(*f)(void*,void*),struct
Cyc_Dict_Dict*d){if(d->t != 0)Cyc_Dict_iter_tree(f,(struct Cyc_Dict_T*)_check_null(
d->t));}static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmp91;struct Cyc_Dict_T*_tmp92;struct Cyc_Dict_T*
_tmp93;struct _tuple0 _tmp94;void*_tmp95;void*_tmp96;struct Cyc_Dict_T*_tmp90=t;
_tmp91=*_tmp90;_tmp92=_tmp91.left;_tmp93=_tmp91.right;_tmp94=_tmp91.key_val;
_tmp95=_tmp94.f1;_tmp96=_tmp94.f2;if(_tmp92 != 0)Cyc_Dict_iter_tree_c(f,env,(
struct Cyc_Dict_T*)_check_null(_tmp92));f(env,_tmp95,_tmp96);if(_tmp93 != 0)Cyc_Dict_iter_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmp93));}void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t != 0)Cyc_Dict_iter_tree_c(f,
env,(struct Cyc_Dict_T*)_check_null(d->t));}struct _tuple2{void(*f1)(void*,void*);
struct Cyc_Dict_Dict*f2;};static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,
void*b1){struct _tuple2 _tmp98;void(*_tmp99)(void*,void*);struct Cyc_Dict_Dict*
_tmp9A;struct _tuple2*_tmp97=env;_tmp98=*_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
_tmp99(b1,Cyc_Dict_lookup(_tmp9A,a));}void Cyc_Dict_iter2(void(*f)(void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){struct _tuple2 _tmp9B=({struct
_tuple2 _tmp9C;_tmp9C.f1=f;_tmp9C.f2=d2;_tmp9C;});((void(*)(void(*f)(struct
_tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Dict_iter2_f,& _tmp9B,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct
Cyc_Dict_Dict*f2;void*f3;};static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,
void*b1){struct _tuple3 _tmp9E;void(*_tmp9F)(void*,void*,void*);struct Cyc_Dict_Dict*
_tmpA0;void*_tmpA1;struct _tuple3*_tmp9D=env;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;
_tmpA0=_tmp9E.f2;_tmpA1=_tmp9E.f3;_tmp9F(_tmpA1,b1,Cyc_Dict_lookup(_tmpA0,a));}
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2){struct _tuple3 _tmpA2=({struct _tuple3 _tmpA3;_tmpA3.f1=f;
_tmpA3.f2=d2;_tmpA3.f3=inner_env;_tmpA3;});((void(*)(void(*f)(struct _tuple3*,
void*,void*),struct _tuple3*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,&
_tmpA2,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict*
f2;void*f3;};static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*
accum){struct _tuple4 _tmpA5;void*(*_tmpA6)(void*,void*,void*,void*,void*);struct
Cyc_Dict_Dict*_tmpA7;void*_tmpA8;struct _tuple4*_tmpA4=env;_tmpA5=*_tmpA4;_tmpA6=
_tmpA5.f1;_tmpA7=_tmpA5.f2;_tmpA8=_tmpA5.f3;return _tmpA6(_tmpA8,a,b1,Cyc_Dict_lookup(
_tmpA7,a),accum);}void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),
void*inner_env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2,void*accum){struct
_tuple4 _tmpA9=({struct _tuple4 _tmpAA;_tmpAA.f1=f;_tmpAA.f2=d2;_tmpAA.f3=inner_env;
_tmpAA;});return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct
_tuple4*env,struct Cyc_Dict_Dict*d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,&
_tmpA9,d1,accum);}static struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*
r2,struct Cyc_Dict_T*t){if(t == 0)return 0;else{void*_tmpAC;struct Cyc_Dict_T*_tmpAD;
struct Cyc_Dict_T*_tmpAE;struct _tuple0 _tmpAF;struct Cyc_Dict_T _tmpAB=*((struct Cyc_Dict_T*)
t);_tmpAC=(void*)_tmpAB.color;_tmpAD=_tmpAB.left;_tmpAE=_tmpAB.right;_tmpAF=
_tmpAB.key_val;{struct Cyc_Dict_T*_tmpB0=Cyc_Dict_copy_tree(r2,_tmpAD);struct Cyc_Dict_T*
_tmpB1=Cyc_Dict_copy_tree(r2,_tmpAE);return({struct Cyc_Dict_T*_tmpB2=
_region_malloc(r2,sizeof(*_tmpB2));_tmpB2->color=(void*)_tmpAC;_tmpB2->left=
_tmpB0;_tmpB2->right=_tmpB1;_tmpB2->key_val=_tmpAF;_tmpB2;});}}}struct Cyc_Dict_Dict*
Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict*d){return({struct Cyc_Dict_Dict*
_tmpB3=_region_malloc(r2,sizeof(*_tmpB3));_tmpB3->rel=d->rel;_tmpB3->r=r2;_tmpB3->t=
Cyc_Dict_copy_tree(r2,d->t);_tmpB3;});}struct Cyc_Dict_Dict*Cyc_Dict_copy(struct
Cyc_Dict_Dict*d){return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}static struct Cyc_Dict_T*
Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_T*t){
struct Cyc_Dict_T _tmpB5;void*_tmpB6;struct Cyc_Dict_T*_tmpB7;struct Cyc_Dict_T*
_tmpB8;struct _tuple0 _tmpB9;void*_tmpBA;void*_tmpBB;struct Cyc_Dict_T*_tmpB4=t;
_tmpB5=*_tmpB4;_tmpB6=(void*)_tmpB5.color;_tmpB7=_tmpB5.left;_tmpB8=_tmpB5.right;
_tmpB9=_tmpB5.key_val;_tmpBA=_tmpB9.f1;_tmpBB=_tmpB9.f2;{struct Cyc_Dict_T*_tmpBC=
_tmpB7 == 0?0: Cyc_Dict_map_tree(r,f,(struct Cyc_Dict_T*)_check_null(_tmpB7));void*
_tmpBD=f(_tmpBB);struct Cyc_Dict_T*_tmpBE=_tmpB8 == 0?0: Cyc_Dict_map_tree(r,f,(
struct Cyc_Dict_T*)_check_null(_tmpB8));return({struct Cyc_Dict_T*_tmpBF=
_region_malloc(r,sizeof(*_tmpBF));_tmpBF->color=(void*)_tmpB6;_tmpBF->left=
_tmpBC;_tmpBF->right=_tmpBE;_tmpBF->key_val=({struct _tuple0 _tmpC0;_tmpC0.f1=
_tmpBA;_tmpC0.f2=_tmpBD;_tmpC0;});_tmpBF;});}}struct Cyc_Dict_Dict*Cyc_Dict_rmap(
struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict*d){if(d->t == 0)return({
struct Cyc_Dict_Dict*_tmpC1=_region_malloc(r,sizeof(*_tmpC1));_tmpC1->rel=d->rel;
_tmpC1->r=r;_tmpC1->t=0;_tmpC1;});return({struct Cyc_Dict_Dict*_tmpC2=
_region_malloc(r,sizeof(*_tmpC2));_tmpC2->rel=d->rel;_tmpC2->r=r;_tmpC2->t=Cyc_Dict_map_tree(
r,f,(struct Cyc_Dict_T*)_check_null(d->t));_tmpC2;});}struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap(Cyc_Core_heap_region,
f,d);}static struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(
void*,void*),void*env,struct Cyc_Dict_T*t){struct Cyc_Dict_T _tmpC4;void*_tmpC5;
struct Cyc_Dict_T*_tmpC6;struct Cyc_Dict_T*_tmpC7;struct _tuple0 _tmpC8;void*_tmpC9;
void*_tmpCA;struct Cyc_Dict_T*_tmpC3=t;_tmpC4=*_tmpC3;_tmpC5=(void*)_tmpC4.color;
_tmpC6=_tmpC4.left;_tmpC7=_tmpC4.right;_tmpC8=_tmpC4.key_val;_tmpC9=_tmpC8.f1;
_tmpCA=_tmpC8.f2;{struct Cyc_Dict_T*_tmpCB=_tmpC6 == 0?0: Cyc_Dict_map_tree_c(r,f,
env,(struct Cyc_Dict_T*)_check_null(_tmpC6));void*_tmpCC=f(env,_tmpCA);struct Cyc_Dict_T*
_tmpCD=_tmpC7 == 0?0: Cyc_Dict_map_tree_c(r,f,env,(struct Cyc_Dict_T*)_check_null(
_tmpC7));return({struct Cyc_Dict_T*_tmpCE=_region_malloc(r,sizeof(*_tmpCE));
_tmpCE->color=(void*)_tmpC5;_tmpCE->left=_tmpCB;_tmpCE->right=_tmpCD;_tmpCE->key_val=({
struct _tuple0 _tmpCF;_tmpCF.f1=_tmpC9;_tmpCF.f2=_tmpCC;_tmpCF;});_tmpCE;});}}
struct Cyc_Dict_Dict*Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),
void*env,struct Cyc_Dict_Dict*d){if(d->t == 0)return({struct Cyc_Dict_Dict*_tmpD0=
_region_malloc(r,sizeof(*_tmpD0));_tmpD0->rel=d->rel;_tmpD0->r=r;_tmpD0->t=0;
_tmpD0;});return({struct Cyc_Dict_Dict*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));
_tmpD1->rel=d->rel;_tmpD1->r=r;_tmpD1->t=Cyc_Dict_map_tree_c(r,f,env,(struct Cyc_Dict_T*)
_check_null(d->t));_tmpD1;});}struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_Dict_Dict*d){return Cyc_Dict_rmap_c(Cyc_Core_heap_region,
f,env,d);}struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d){if(d->t == 0)(int)_throw((void*)Cyc_Dict_Absent);return({struct _tuple0*_tmpD2=
_region_malloc(r,sizeof(*_tmpD2));_tmpD2->f1=(((struct Cyc_Dict_T*)_check_null(d->t))->key_val).f1;
_tmpD2->f2=(((struct Cyc_Dict_T*)_check_null(d->t))->key_val).f2;_tmpD2;});}
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_T*
t){struct Cyc_Dict_T _tmpD4;struct Cyc_Dict_T*_tmpD5;struct Cyc_Dict_T*_tmpD6;struct
_tuple0 _tmpD7;void*_tmpD8;void*_tmpD9;struct Cyc_Dict_T*_tmpD3=t;_tmpD4=*_tmpD3;
_tmpD5=_tmpD4.left;_tmpD6=_tmpD4.right;_tmpD7=_tmpD4.key_val;_tmpD8=_tmpD7.f1;
_tmpD9=_tmpD7.f2;return((_tmpD5 == 0?1: Cyc_Dict_forall_tree_c(f,env,(struct Cyc_Dict_T*)
_check_null(_tmpD5)))?f(env,_tmpD8,_tmpD9): 0)?_tmpD6 == 0?1: Cyc_Dict_forall_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(_tmpD6)): 0;}int Cyc_Dict_forall_c(int(*f)(
void*,void*,void*),void*env,struct Cyc_Dict_Dict*d){if(d->t == 0)return 1;return Cyc_Dict_forall_tree_c(
f,env,(struct Cyc_Dict_T*)_check_null(d->t));}struct _tuple5{int(*f1)(void*,void*,
void*);struct Cyc_Dict_Dict*f2;};static int Cyc_Dict_forall_intersect_f(struct
_tuple5*env,void*a,void*b){struct _tuple5 _tmpDB;int(*_tmpDC)(void*,void*,void*);
struct Cyc_Dict_Dict*_tmpDD;struct _tuple5*_tmpDA=env;_tmpDB=*_tmpDA;_tmpDC=_tmpDB.f1;
_tmpDD=_tmpDB.f2;if(Cyc_Dict_member(_tmpDD,a))return _tmpDC(a,b,Cyc_Dict_lookup(
_tmpDD,a));return 1;}int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),
struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){struct _tuple5 _tmpDE=({struct
_tuple5 _tmpDF;_tmpDF.f1=f;_tmpDF.f2=d2;_tmpDF;});return((int(*)(int(*f)(struct
_tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)(
Cyc_Dict_forall_intersect_f,& _tmpDE,d1);}struct _tuple6{void*(*f1)(void*,void*,
void*,void*);void*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*
env,void*a,void*b,struct Cyc_Dict_Dict*d1){if(Cyc_Dict_member(d1,a)){void*_tmpE0=
Cyc_Dict_lookup(d1,a);void*_tmpE1=((*((struct _tuple6*)env)).f1)((*((struct
_tuple6*)env)).f2,a,_tmpE0,b);if(_tmpE1 != _tmpE0)return Cyc_Dict_insert(d1,a,
_tmpE1);return d1;}return Cyc_Dict_insert(d1,a,b);}struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){if((int)d1 == (int)d2)return d1;return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*
f)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,({struct _tuple6*
_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->f1=f;_tmpE2->f2=env;_tmpE2;}),d2,d1);}
struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){if(d1 == d2?1: d2->t == 0)return
d2;{struct _tagged_arr queue=_tag_arr(({unsigned int _tmpF1=(unsigned int)16;struct
Cyc_Dict_T**_tmpF2=(struct Cyc_Dict_T**)_cycalloc(_check_times(sizeof(struct Cyc_Dict_T*),
_tmpF1));{unsigned int _tmpF3=_tmpF1;unsigned int i;for(i=0;i < _tmpF3;i ++){_tmpF2[
i]=(struct Cyc_Dict_T*)_check_null(d2->t);}}_tmpF2;}),sizeof(struct Cyc_Dict_T*),(
unsigned int)16);int ind=0;struct Cyc_Dict_T*_tmpE3=0;while(ind != - 1){struct Cyc_Dict_T
_tmpE5;struct Cyc_Dict_T*_tmpE6;struct Cyc_Dict_T*_tmpE7;struct _tuple0 _tmpE8;void*
_tmpE9;void*_tmpEA;struct Cyc_Dict_T*_tmpE4=*((struct Cyc_Dict_T**)
_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),ind --));_tmpE5=*_tmpE4;
_tmpE6=_tmpE5.left;_tmpE7=_tmpE5.right;_tmpE8=_tmpE5.key_val;_tmpE9=_tmpE8.f1;
_tmpEA=_tmpE8.f2;if(ind + 2 >= _get_arr_size(queue,sizeof(struct Cyc_Dict_T*)))
queue=({unsigned int _tmpEB=_get_arr_size(queue,sizeof(struct Cyc_Dict_T*))* 2;
struct Cyc_Dict_T**_tmpEC=(struct Cyc_Dict_T**)_cycalloc(_check_times(sizeof(
struct Cyc_Dict_T*),_tmpEB));struct _tagged_arr _tmpEE=_tag_arr(_tmpEC,sizeof(
struct Cyc_Dict_T*),_tmpEB);{unsigned int _tmpED=_tmpEB;unsigned int i;for(i=0;i < 
_tmpED;i ++){_tmpEC[i]=i < _get_arr_size(queue,sizeof(struct Cyc_Dict_T*))?((struct
Cyc_Dict_T**)queue.curr)[(int)i]:(struct Cyc_Dict_T*)_check_null(d2->t);}}_tmpEE;});
if(_tmpE6 != 0)*((struct Cyc_Dict_T**)_check_unknown_subscript(queue,sizeof(struct
Cyc_Dict_T*),++ ind))=(struct Cyc_Dict_T*)_check_null(_tmpE6);if(_tmpE7 != 0)*((
struct Cyc_Dict_T**)_check_unknown_subscript(queue,sizeof(struct Cyc_Dict_T*),++
ind))=(struct Cyc_Dict_T*)_check_null(_tmpE7);if(Cyc_Dict_member(d1,_tmpE9))
_tmpE3=Cyc_Dict_ins(d2->r,d2->rel,({struct _tuple0 _tmpEF;_tmpEF.f1=_tmpE9;_tmpEF.f2=
f(env,_tmpE9,Cyc_Dict_lookup(d1,_tmpE9),_tmpEA);_tmpEF;}),_tmpE3);}return({
struct Cyc_Dict_Dict*_tmpF0=_region_malloc(d2->r,sizeof(*_tmpF0));_tmpF0->rel=d2->rel;
_tmpF0->r=d2->r;_tmpF0->t=_tmpE3;_tmpF0;});}}static void*Cyc_Dict_intersect_f(
void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){return f(a,b1,b2);}struct Cyc_Dict_Dict*
Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){return((struct Cyc_Dict_Dict*(*)(void*(*f)(void*(*)(void*,void*,void*),void*,
void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}static struct Cyc_List_List*
Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmpF4=_region_malloc(r,sizeof(*_tmpF4));_tmpF4->hd=({
struct _tuple0*_tmpF5=_region_malloc(r,sizeof(*_tmpF5));_tmpF5->f1=k;_tmpF5->f2=v;
_tmpF5;});_tmpF4->tl=accum;_tmpF4;});}struct Cyc_List_List*Cyc_Dict_rto_list(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(
Cyc_Dict_to_list_f,r,d,0);}struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict*
d){return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,
void*);struct _RegionHandle*f2;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(
struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple7 _tmpF7;int(*
_tmpF8)(void*,void*);struct _RegionHandle*_tmpF9;struct _tuple7*_tmpF6=env;_tmpF7=*
_tmpF6;_tmpF8=_tmpF7.f1;_tmpF9=_tmpF7.f2;return _tmpF8(x,y)?Cyc_Dict_insert(acc,x,
y): acc;}struct Cyc_Dict_Dict*Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,
void*),struct Cyc_Dict_Dict*d){struct _tuple7 _tmpFA=({struct _tuple7 _tmpFB;_tmpFB.f1=
f;_tmpFB.f2=r2;_tmpFB;});return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(
struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmpFA,d,Cyc_Dict_rempty(
r2,d->rel));}struct Cyc_Dict_Dict*Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict*
d){return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,
void*,void*);void*f2;struct _RegionHandle*f3;};static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(
struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){struct _tuple8 _tmpFD;int(*
_tmpFE)(void*,void*,void*);void*_tmpFF;struct _RegionHandle*_tmp100;struct _tuple8*
_tmpFC=env;_tmpFD=*_tmpFC;_tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;_tmp100=_tmpFD.f3;
return _tmpFE(_tmpFF,x,y)?Cyc_Dict_insert(acc,x,y): acc;}struct Cyc_Dict_Dict*Cyc_Dict_rfilter_c(
struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict*
d){struct _tuple8 _tmp101=({struct _tuple8 _tmp102;_tmp102.f1=f;_tmp102.f2=f_env;
_tmp102.f3=r2;_tmp102;});return((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(
struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict*
d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmp101,d,Cyc_Dict_rempty(
r2,d->rel));}struct Cyc_Dict_Dict*Cyc_Dict_filter_c(int(*f)(void*,void*,void*),
void*f_env,struct Cyc_Dict_Dict*d){return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,
f,f_env,d);}static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(d,x);}struct Cyc_Dict_Dict*Cyc_Dict_rdifference(struct
_RegionHandle*r2,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2){return((struct
Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),
struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,
d2,d1);}struct Cyc_Dict_Dict*Cyc_Dict_difference(struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2){return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*
f1)(void*,void*);void*f2;};static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,
void*y){struct _tuple9 _tmp104;int(*_tmp105)(void*,void*);void*_tmp106;struct
_tuple9*_tmp103=env;_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;
return _tmp105(_tmp106,x)!= 0;}struct Cyc_Dict_Dict*Cyc_Dict_rdelete(struct
_RegionHandle*r2,struct Cyc_Dict_Dict*d,void*x){if(!Cyc_Dict_member(d,x))return
Cyc_Dict_rcopy(r2,d);{struct _tuple9 _tmp107=({struct _tuple9 _tmp108;_tmp108.f1=d->rel;
_tmp108.f2=x;_tmp108;});return((struct Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,
int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict*d))
Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,& _tmp107,d);}}struct Cyc_Dict_Dict*Cyc_Dict_rdelete_same(
struct Cyc_Dict_Dict*d,void*x){if(!Cyc_Dict_member(d,x))return d;{struct _tuple9
_tmp109=({struct _tuple9 _tmp10A;_tmp10A.f1=d->rel;_tmp10A.f2=x;_tmp10A;});return((
struct Cyc_Dict_Dict*(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),
struct _tuple9*f_env,struct Cyc_Dict_Dict*d))Cyc_Dict_rfilter_c)(d->r,Cyc_Dict_delete_f,&
_tmp109,d);}}struct Cyc_Dict_Dict*Cyc_Dict_delete(struct Cyc_Dict_Dict*d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct
_tagged_arr f1;int f2;};int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
struct _tuple10 _tmp10C;struct _tagged_arr _tmp10D;int _tmp10E;int*_tmp10F;struct
_tuple10*_tmp10B=stk;_tmp10C=*_tmp10B;_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;
_tmp10F=(int*)&(*_tmp10B).f2;{int _tmp110=*((int*)_tmp10F);if(_tmp110 == - 1)return
0;{struct Cyc_Dict_T*_tmp111=*((struct Cyc_Dict_T**)_check_unknown_subscript(
_tmp10D,sizeof(struct Cyc_Dict_T*),_tmp110));*((struct _tuple0*)dest)=((struct Cyc_Dict_T*)
_check_null(_tmp111))->key_val;-- _tmp110;if((unsigned int)_tmp111->left)*((
struct Cyc_Dict_T**)_check_unknown_subscript(_tmp10D,sizeof(struct Cyc_Dict_T*),++
_tmp110))=_tmp111->left;if((unsigned int)_tmp111->right)*((struct Cyc_Dict_T**)
_check_unknown_subscript(_tmp10D,sizeof(struct Cyc_Dict_T*),++ _tmp110))=_tmp111->right;*((
int*)_tmp10F)=_tmp110;return 1;}}}struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict*d){int half_max_size=1;struct Cyc_Dict_T*
_tmp112=d->t;while(_tmp112 != 0){_tmp112=_tmp112->left;++ half_max_size;}_tmp112=d->t;{
struct _tagged_arr _tmp113=({unsigned int _tmp116=(unsigned int)(2 * half_max_size);
struct Cyc_Dict_T**_tmp117=(struct Cyc_Dict_T**)_region_malloc(rgn,_check_times(
sizeof(struct Cyc_Dict_T*),_tmp116));struct _tagged_arr _tmp119=_tag_arr(_tmp117,
sizeof(struct Cyc_Dict_T*),_tmp116);{unsigned int _tmp118=_tmp116;unsigned int i;
for(i=0;i < _tmp118;i ++){_tmp117[i]=_tmp112;}}_tmp119;});return({struct Cyc_Iter_Iter
_tmp114;_tmp114.env=(void*)({struct _tuple10*_tmp115=_region_malloc(rgn,sizeof(*
_tmp115));_tmp115->f1=_tmp113;_tmp115->f2=(unsigned int)_tmp112?0: - 1;_tmp115;});
_tmp114.next=(int(*)(void*env,void*dest))Cyc_Dict_iter_f;_tmp114;});}}void*Cyc_Dict_marshal(
struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,
void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*
fp,struct Cyc_Dict_Dict*dict){struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,
dict);int len=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dict_list);(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));
_tmp11A[0]=({struct Cyc_Core_Failure_struct _tmp11B;_tmp11B.tag=Cyc_Core_Failure;
_tmp11B.f1=({const char*_tmp11C="Dict::marshal: Write failure";_tag_arr(_tmp11C,
sizeof(char),_get_zero_arr_size(_tmp11C,29));});_tmp11B;});_tmp11A;}));while(
dict_list != 0){env=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))
write_key)(env,fp,(struct _tuple0*)dict_list->hd);env=((void*(*)(void*,struct Cyc___cycFILE*,
struct _tuple0*))write_val)(env,fp,(struct _tuple0*)dict_list->hd);dict_list=
dict_list->tl;}return env;}struct Cyc_Dict_Dict*Cyc_Dict_unmarshal(struct
_RegionHandle*rgn,void**env,int(*cmp)(void*,void*),void*(*read_key)(void**,
struct Cyc___cycFILE*),void*(*read_val)(void**,struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp){struct Cyc_Dict_Dict*dict=Cyc_Dict_empty(cmp);int len=Cyc_getw(fp);if(len == - 1)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp11D=_cycalloc(sizeof(*
_tmp11D));_tmp11D[0]=({struct Cyc_Core_Failure_struct _tmp11E;_tmp11E.tag=Cyc_Core_Failure;
_tmp11E.f1=({const char*_tmp11F="Dict::unmarshal: list length is -1";_tag_arr(
_tmp11F,sizeof(char),_get_zero_arr_size(_tmp11F,35));});_tmp11E;});_tmp11D;}));{
int i=0;for(0;i < len;i ++){void*key=read_key(env,fp);void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}return dict;}
