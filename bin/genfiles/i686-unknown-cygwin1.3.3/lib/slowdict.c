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
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static inline void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#ifdef _INLINE_FUNCTIONS
static inline char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
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
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_unknown_subscript(struct _tagged_arr arr,unsigned elt_sz,unsigned index) {
  struct _tagged_arr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
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
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tag_arr(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr *
_init_tag_arr(struct _tagged_arr *arr_ptr,
              void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _tagged_arr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_arr(struct _tagged_arr arr, unsigned elt_sz,unsigned num_elts) {
  struct _tagged_arr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_arr_size(struct _tagged_arr arr,unsigned elt_sz) {
  struct _tagged_arr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_plus(struct _tagged_arr arr,unsigned elt_sz,int change) {
  struct _tagged_arr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _tagged_arr
_tagged_arr_inplace_plus_post(struct _tagged_arr *arr_ptr,unsigned elt_sz,int change) {
  struct _tagged_arr * _arr_ptr = (arr_ptr);
  struct _tagged_arr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*
v;};struct Cyc_Splay_Node_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{
void*key;void*data;void*left;void*right;};int Cyc_Splay_splay(int(*f)(void*,void*),
void*,void*);struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[12];extern
char Cyc_SlowDict_Absent[11];struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*cmp)(
void*,void*));int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);int Cyc_SlowDict_member(
struct Cyc_SlowDict_Dict*d,void*k);struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict*d,void*k,void*v);struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict*d,void*k,void*v);struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(
struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*l);struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(
int(*cmp)(void*,void*),void*k,void*v);void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*
d,void*k);struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,
void*k);struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*
d,void*k);void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*
d,void*accum);void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*
env,struct Cyc_SlowDict_Dict*d,void*accum);void Cyc_SlowDict_app(void*(*f)(void*,
void*),struct Cyc_SlowDict_Dict*d);void Cyc_SlowDict_app_c(void*(*f)(void*,void*,
void*),void*env,struct Cyc_SlowDict_Dict*d);void Cyc_SlowDict_iter(void(*f)(void*,
void*),struct Cyc_SlowDict_Dict*d);void Cyc_SlowDict_iter_c(void(*f)(void*,void*,
void*),void*env,struct Cyc_SlowDict_Dict*d);struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(
void*(*f)(void*),struct Cyc_SlowDict_Dict*d);struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d);struct _tuple0{void*f1;
void*f2;};struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d);struct Cyc_List_List*
Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d);char Cyc_SlowDict_Absent[11]="\000\000\000\000Absent\000";
char Cyc_SlowDict_Present[12]="\000\000\000\000Present\000";struct Cyc_SlowDict_Dict{
int(*reln)(void*,void*);void*tree;};struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(
int(*comp)(void*,void*)){void*t=(void*)0;return({struct Cyc_SlowDict_Dict*_tmp0=
_cycalloc(sizeof(*_tmp0));_tmp0->reln=comp;_tmp0->tree=(void*)t;_tmp0;});}int Cyc_SlowDict_is_empty(
struct Cyc_SlowDict_Dict*d){void*_tmp1=(void*)d->tree;_LL1: if((int)_tmp1 != 0)goto
_LL3;_LL2: return 1;_LL3: if(_tmp1 <= (void*)1?1:*((int*)_tmp1)!= 0)goto _LL0;_LL4:
return 0;_LL0:;}int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){return
Cyc_Splay_splay(d->reln,key,(void*)d->tree);}struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(
struct Cyc_SlowDict_Dict*d,void*key,void*data){void*newleft=(void*)0;void*
newright=(void*)0;if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){void*_tmp2=(
void*)d->tree;struct Cyc_Splay_noderef*_tmp3;_LL6: if(_tmp2 <= (void*)1?1:*((int*)
_tmp2)!= 0)goto _LL8;_tmp3=((struct Cyc_Splay_Node_struct*)_tmp2)->f1;_LL7: newleft=(
void*)(_tmp3->v)->left;newright=(void*)(_tmp3->v)->right;goto _LL5;_LL8:;_LL9:
goto _LL5;_LL5:;}else{void*_tmp4=(void*)d->tree;struct Cyc_Splay_noderef*_tmp5;
_LLB: if(_tmp4 <= (void*)1?1:*((int*)_tmp4)!= 0)goto _LLD;_tmp5=((struct Cyc_Splay_Node_struct*)
_tmp4)->f1;_LLC: {struct Cyc_Splay_node*_tmp6=_tmp5->v;if((d->reln)(key,(void*)
_tmp6->key)< 0){newleft=(void*)_tmp6->left;newright=(void*)({struct Cyc_Splay_Node_struct*
_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Splay_Node_struct _tmp8;
_tmp8.tag=0;_tmp8.f1=({struct Cyc_Splay_noderef*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9->v=({struct Cyc_Splay_node*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->key=(void*)((
void*)_tmp6->key);_tmpA->data=(void*)((void*)_tmp6->data);_tmpA->left=(void*)((
void*)0);_tmpA->right=(void*)((void*)_tmp6->right);_tmpA;});_tmp9;});_tmp8;});
_tmp7;});}else{newleft=(void*)({struct Cyc_Splay_Node_struct*_tmpB=_cycalloc(
sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Splay_Node_struct _tmpC;_tmpC.tag=0;_tmpC.f1=({
struct Cyc_Splay_noderef*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->v=({struct Cyc_Splay_node*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->key=(void*)((void*)_tmp6->key);_tmpE->data=(
void*)((void*)_tmp6->data);_tmpE->left=(void*)((void*)_tmp6->left);_tmpE->right=(
void*)((void*)0);_tmpE;});_tmpD;});_tmpC;});_tmpB;});newright=(void*)_tmp6->right;}
goto _LLA;}_LLD: if((int)_tmp4 != 0)goto _LLA;_LLE: goto _LLA;_LLA:;}return({struct Cyc_SlowDict_Dict*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->reln=d->reln;_tmpF->tree=(void*)((void*)({
struct Cyc_Splay_Node_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct
Cyc_Splay_Node_struct _tmp11;_tmp11.tag=0;_tmp11.f1=({struct Cyc_Splay_noderef*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->v=({struct Cyc_Splay_node*_tmp13=
_cycalloc(sizeof(*_tmp13));_tmp13->key=(void*)key;_tmp13->data=(void*)data;
_tmp13->left=(void*)newleft;_tmp13->right=(void*)newright;_tmp13;});_tmp12;});
_tmp11;});_tmp10;}));_tmpF;});}struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(
struct Cyc_SlowDict_Dict*d,void*key,void*data){if(Cyc_Splay_splay(d->reln,key,(
void*)d->tree))(int)_throw((void*)Cyc_SlowDict_Present);return Cyc_SlowDict_insert(
d,key,data);}struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*
d,struct Cyc_List_List*kds){for(0;kds != 0;kds=kds->tl){d=Cyc_SlowDict_insert(d,(*((
struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}return d;}struct Cyc_SlowDict_Dict*
Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){return({
struct Cyc_SlowDict_Dict*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->reln=comp;
_tmp14->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*_tmp15=_cycalloc(
sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Splay_Node_struct _tmp16;_tmp16.tag=0;
_tmp16.f1=({struct Cyc_Splay_noderef*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->v=({
struct Cyc_Splay_node*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->key=(void*)key;
_tmp18->data=(void*)data;_tmp18->left=(void*)((void*)0);_tmp18->right=(void*)((
void*)0);_tmp18;});_tmp17;});_tmp16;});_tmp15;}));_tmp14;});}void*Cyc_SlowDict_lookup(
struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){
void*_tmp19=(void*)d->tree;struct Cyc_Splay_noderef*_tmp1A;_LL10: if(_tmp19 <= (
void*)1?1:*((int*)_tmp19)!= 0)goto _LL12;_tmp1A=((struct Cyc_Splay_Node_struct*)
_tmp19)->f1;_LL11: return(void*)(_tmp1A->v)->data;_LL12: if((int)_tmp19 != 0)goto
_LLF;_LL13:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1B=
_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_Core_Impossible_struct _tmp1C;
_tmp1C.tag=Cyc_Core_Impossible;_tmp1C.f1=({const char*_tmp1D="Dict::lookup";
_tag_arr(_tmp1D,sizeof(char),_get_zero_arr_size(_tmp1D,13));});_tmp1C;});_tmp1B;}));
_LLF:;}(int)_throw((void*)Cyc_SlowDict_Absent);}struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(
struct Cyc_SlowDict_Dict*d,void*key){if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){
void*_tmp1E=(void*)d->tree;struct Cyc_Splay_noderef*_tmp1F;_LL15: if(_tmp1E <= (
void*)1?1:*((int*)_tmp1E)!= 0)goto _LL17;_tmp1F=((struct Cyc_Splay_Node_struct*)
_tmp1E)->f1;_LL16: return({struct Cyc_Core_Opt*_tmp20=_cycalloc(sizeof(*_tmp20));
_tmp20->v=(void*)((void*)(_tmp1F->v)->data);_tmp20;});_LL17: if((int)_tmp1E != 0)
goto _LL14;_LL18:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp21=
_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Core_Impossible_struct _tmp22;
_tmp22.tag=Cyc_Core_Impossible;_tmp22.f1=({const char*_tmp23="Dict::lookup";
_tag_arr(_tmp23,sizeof(char),_get_zero_arr_size(_tmp23,13));});_tmp22;});_tmp21;}));
_LL14:;}return 0;}static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,(void*)d->tree)){void*_tmp24=(void*)d->tree;
struct Cyc_Splay_noderef*_tmp25;_LL1A: if((int)_tmp24 != 0)goto _LL1C;_LL1B:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Core_Impossible_struct _tmp27;_tmp27.tag=Cyc_Core_Impossible;
_tmp27.f1=({const char*_tmp28="Dict::lookup";_tag_arr(_tmp28,sizeof(char),
_get_zero_arr_size(_tmp28,13));});_tmp27;});_tmp26;}));_LL1C: if(_tmp24 <= (void*)
1?1:*((int*)_tmp24)!= 0)goto _LL19;_tmp25=((struct Cyc_Splay_Node_struct*)_tmp24)->f1;
_LL1D: {struct Cyc_Splay_node*n=_tmp25->v;void*_tmp29=(void*)n->left;struct Cyc_Splay_noderef*
_tmp2A;_LL1F: if((int)_tmp29 != 0)goto _LL21;_LL20: return({struct Cyc_SlowDict_Dict*
_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->reln=d->reln;_tmp2B->tree=(void*)((
void*)n->right);_tmp2B;});_LL21: if(_tmp29 <= (void*)1?1:*((int*)_tmp29)!= 0)goto
_LL1E;_tmp2A=((struct Cyc_Splay_Node_struct*)_tmp29)->f1;_LL22: {void*_tmp2C=(
void*)n->right;struct Cyc_Splay_noderef*_tmp2D;_LL24: if((int)_tmp2C != 0)goto _LL26;
_LL25: return({struct Cyc_SlowDict_Dict*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->reln=
d->reln;_tmp2E->tree=(void*)((void*)n->left);_tmp2E;});_LL26: if(_tmp2C <= (void*)
1?1:*((int*)_tmp2C)!= 0)goto _LL23;_tmp2D=((struct Cyc_Splay_Node_struct*)_tmp2C)->f1;
_LL27: Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,(void*)n->left);{struct Cyc_Splay_node*
newtop=_tmp2A->v;return({struct Cyc_SlowDict_Dict*_tmp2F=_cycalloc(sizeof(*_tmp2F));
_tmp2F->reln=d->reln;_tmp2F->tree=(void*)((void*)({struct Cyc_Splay_Node_struct*
_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30[0]=({struct Cyc_Splay_Node_struct _tmp31;
_tmp31.tag=0;_tmp31.f1=({struct Cyc_Splay_noderef*_tmp32=_cycalloc(sizeof(*_tmp32));
_tmp32->v=({struct Cyc_Splay_node*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->key=(
void*)((void*)newtop->key);_tmp33->data=(void*)((void*)newtop->data);_tmp33->left=(
void*)((void*)newtop->left);_tmp33->right=(void*)((void*)n->right);_tmp33;});
_tmp32;});_tmp31;});_tmp30;}));_tmp2F;});}_LL23:;}_LL1E:;}_LL19:;}else{return d;}}
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,
void*key){struct Cyc_SlowDict_Dict*_tmp34=Cyc_SlowDict_delete(d,key);if(d == 
_tmp34)(int)_throw((void*)Cyc_SlowDict_Absent);return _tmp34;}static void*Cyc_SlowDict_fold_tree(
void*(*f)(void*,void*,void*),void*t,void*accum){void*_tmp35=t;struct Cyc_Splay_noderef*
_tmp36;_LL29: if((int)_tmp35 != 0)goto _LL2B;_LL2A: return accum;_LL2B: if(_tmp35 <= (
void*)1?1:*((int*)_tmp35)!= 0)goto _LL28;_tmp36=((struct Cyc_Splay_Node_struct*)
_tmp35)->f1;_LL2C: {struct Cyc_Splay_node*n=_tmp36->v;return f((void*)n->key,(void*)
n->data,Cyc_SlowDict_fold_tree(f,(void*)n->left,Cyc_SlowDict_fold_tree(f,(void*)
n->right,accum)));}_LL28:;}void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),
struct Cyc_SlowDict_Dict*d,void*accum){return Cyc_SlowDict_fold_tree(f,(void*)d->tree,
accum);}static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),
void*env,void*t,void*accum){void*_tmp37=t;struct Cyc_Splay_noderef*_tmp38;_LL2E:
if((int)_tmp37 != 0)goto _LL30;_LL2F: return accum;_LL30: if(_tmp37 <= (void*)1?1:*((
int*)_tmp37)!= 0)goto _LL2D;_tmp38=((struct Cyc_Splay_Node_struct*)_tmp37)->f1;
_LL31: {struct Cyc_Splay_node*n=_tmp38->v;return f(env,(void*)n->key,(void*)n->data,
Cyc_SlowDict_fold_tree_c(f,env,(void*)n->left,Cyc_SlowDict_fold_tree_c(f,env,(
void*)n->right,accum)));}_LL2D:;}void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,
void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){return Cyc_SlowDict_fold_tree_c(
f,env,(void*)dict->tree,accum);}static void Cyc_SlowDict_app_tree(void*(*f)(void*,
void*),void*t){void*_tmp39=t;struct Cyc_Splay_noderef*_tmp3A;_LL33: if((int)_tmp39
!= 0)goto _LL35;_LL34: goto _LL32;_LL35: if(_tmp39 <= (void*)1?1:*((int*)_tmp39)!= 0)
goto _LL32;_tmp3A=((struct Cyc_Splay_Node_struct*)_tmp39)->f1;_LL36: {struct Cyc_Splay_node*
_tmp3B=_tmp3A->v;Cyc_SlowDict_app_tree(f,(void*)_tmp3B->left);f((void*)_tmp3B->key,(
void*)_tmp3B->data);Cyc_SlowDict_app_tree(f,(void*)_tmp3B->right);goto _LL32;}
_LL32:;}void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,(void*)d->tree);}static void Cyc_SlowDict_iter_tree(void(*
f)(void*,void*),void*t){void*_tmp3C=t;struct Cyc_Splay_noderef*_tmp3D;_LL38: if((
int)_tmp3C != 0)goto _LL3A;_LL39: goto _LL37;_LL3A: if(_tmp3C <= (void*)1?1:*((int*)
_tmp3C)!= 0)goto _LL37;_tmp3D=((struct Cyc_Splay_Node_struct*)_tmp3C)->f1;_LL3B: {
struct Cyc_Splay_node*n=_tmp3D->v;Cyc_SlowDict_iter_tree(f,(void*)n->left);f((
void*)n->key,(void*)n->data);Cyc_SlowDict_iter_tree(f,(void*)n->right);goto _LL37;}
_LL37:;}void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree(f,(void*)d->tree);}static void Cyc_SlowDict_app_tree_c(void*(*
f)(void*,void*,void*),void*env,void*t){void*_tmp3E=t;struct Cyc_Splay_noderef*
_tmp3F;_LL3D: if((int)_tmp3E != 0)goto _LL3F;_LL3E: goto _LL3C;_LL3F: if(_tmp3E <= (
void*)1?1:*((int*)_tmp3E)!= 0)goto _LL3C;_tmp3F=((struct Cyc_Splay_Node_struct*)
_tmp3E)->f1;_LL40: {struct Cyc_Splay_node*n=_tmp3F->v;Cyc_SlowDict_app_tree_c(f,
env,(void*)n->left);f(env,(void*)n->key,(void*)n->data);Cyc_SlowDict_app_tree_c(
f,env,(void*)n->right);goto _LL3C;}_LL3C:;}void Cyc_SlowDict_app_c(void*(*f)(void*,
void*,void*),void*env,struct Cyc_SlowDict_Dict*d){Cyc_SlowDict_app_tree_c(f,env,(
void*)d->tree);}static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),
void*env,void*t){void*_tmp40=t;struct Cyc_Splay_noderef*_tmp41;_LL42: if((int)
_tmp40 != 0)goto _LL44;_LL43: goto _LL41;_LL44: if(_tmp40 <= (void*)1?1:*((int*)_tmp40)
!= 0)goto _LL41;_tmp41=((struct Cyc_Splay_Node_struct*)_tmp40)->f1;_LL45: {struct
Cyc_Splay_node*n=_tmp41->v;Cyc_SlowDict_iter_tree_c(f,env,(void*)n->left);f(env,(
void*)n->key,(void*)n->data);Cyc_SlowDict_iter_tree_c(f,env,(void*)n->right);
goto _LL41;}_LL41:;}void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,
struct Cyc_SlowDict_Dict*d){Cyc_SlowDict_iter_tree_c(f,env,(void*)d->tree);}
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){void*_tmp42=t;struct
Cyc_Splay_noderef*_tmp43;_LL47: if((int)_tmp42 != 0)goto _LL49;_LL48: return(void*)0;
_LL49: if(_tmp42 <= (void*)1?1:*((int*)_tmp42)!= 0)goto _LL46;_tmp43=((struct Cyc_Splay_Node_struct*)
_tmp42)->f1;_LL4A: {struct Cyc_Splay_node*_tmp44=_tmp43->v;return(void*)({struct
Cyc_Splay_Node_struct*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Splay_Node_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=({struct Cyc_Splay_noderef*_tmp47=_cycalloc(sizeof(*
_tmp47));_tmp47->v=({struct Cyc_Splay_node*_tmp48=_cycalloc(sizeof(*_tmp48));
_tmp48->key=(void*)((void*)_tmp44->key);_tmp48->data=(void*)f((void*)_tmp44->data);
_tmp48->left=(void*)Cyc_SlowDict_map_tree(f,(void*)_tmp44->left);_tmp48->right=(
void*)Cyc_SlowDict_map_tree(f,(void*)_tmp44->right);_tmp48;});_tmp47;});_tmp46;});
_tmp45;});}_LL46:;}struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),
struct Cyc_SlowDict_Dict*d){return({struct Cyc_SlowDict_Dict*_tmp49=_cycalloc(
sizeof(*_tmp49));_tmp49->reln=d->reln;_tmp49->tree=(void*)Cyc_SlowDict_map_tree(
f,(void*)d->tree);_tmp49;});}static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,
void*),void*env,void*t){void*_tmp4A=t;struct Cyc_Splay_noderef*_tmp4B;_LL4C: if((
int)_tmp4A != 0)goto _LL4E;_LL4D: return(void*)0;_LL4E: if(_tmp4A <= (void*)1?1:*((
int*)_tmp4A)!= 0)goto _LL4B;_tmp4B=((struct Cyc_Splay_Node_struct*)_tmp4A)->f1;
_LL4F: {struct Cyc_Splay_node*n=_tmp4B->v;return(void*)({struct Cyc_Splay_Node_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Splay_Node_struct _tmp4D;
_tmp4D.tag=0;_tmp4D.f1=({struct Cyc_Splay_noderef*_tmp4E=_cycalloc(sizeof(*_tmp4E));
_tmp4E->v=({struct Cyc_Splay_node*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->key=(
void*)((void*)n->key);_tmp4F->data=(void*)f(env,(void*)n->data);_tmp4F->left=(
void*)Cyc_SlowDict_map_tree_c(f,env,(void*)n->left);_tmp4F->right=(void*)Cyc_SlowDict_map_tree_c(
f,env,(void*)n->right);_tmp4F;});_tmp4E;});_tmp4D;});_tmp4C;});}_LL4B:;}struct
Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*
d){return({struct Cyc_SlowDict_Dict*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->reln=
d->reln;_tmp50->tree=(void*)Cyc_SlowDict_map_tree_c(f,env,(void*)d->tree);_tmp50;});}
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){void*_tmp51=(void*)
d->tree;struct Cyc_Splay_noderef*_tmp52;_LL51: if((int)_tmp51 != 0)goto _LL53;_LL52:(
int)_throw((void*)Cyc_SlowDict_Absent);_LL53: if(_tmp51 <= (void*)1?1:*((int*)
_tmp51)!= 0)goto _LL50;_tmp52=((struct Cyc_Splay_Node_struct*)_tmp51)->f1;_LL54:
return({struct _tuple0*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->f1=(void*)(
_tmp52->v)->key;_tmp53->f2=(void*)(_tmp52->v)->data;_tmp53;});_LL50:;}struct Cyc_List_List*
Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){return({struct
Cyc_List_List*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->hd=({struct _tuple0*
_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->f1=k;_tmp55->f2=v;_tmp55;});_tmp54->tl=
accum;_tmp54;});}struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*
d){return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(void*,void*,struct
Cyc_List_List*),struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*accum))Cyc_SlowDict_fold)(
Cyc_SlowDict_to_list_f,d,0);}
