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
#else
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
static inline unsigned char *
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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];struct Cyc_Splay_node;
struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Node_struct{int
tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*
left;void*right;};int Cyc_Splay_rsplay(struct _RegionHandle*,int(*f)(void*,void*),
void*,void*);int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);static void Cyc_Splay_rotate_left(
struct _RegionHandle*r,struct Cyc_Splay_noderef*nr){struct Cyc_Splay_node*_tmp0=nr->v;
void*_tmp1=(void*)_tmp0->left;struct Cyc_Splay_noderef*_tmp2;_LL1: if(_tmp1 <= (
void*)1?1:*((int*)_tmp1)!= 0)goto _LL3;_tmp2=((struct Cyc_Splay_Node_struct*)_tmp1)->f1;
_LL2: {struct Cyc_Splay_node*_tmp3=_tmp2->v;struct Cyc_Splay_Node_struct*_tmp4=({
struct Cyc_Splay_Node_struct*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6[0]=({
struct Cyc_Splay_Node_struct _tmp7;_tmp7.tag=0;_tmp7.f1=({struct Cyc_Splay_noderef*
_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->v=({struct Cyc_Splay_node*_tmp9=
_region_malloc(r,sizeof(*_tmp9));_tmp9->key=(void*)((void*)_tmp0->key);_tmp9->data=(
void*)((void*)_tmp0->data);_tmp9->left=(void*)((void*)_tmp3->right);_tmp9->right=(
void*)((void*)_tmp0->right);_tmp9;});_tmp8;});_tmp7;});_tmp6;});nr->v=({struct
Cyc_Splay_node*_tmp5=_region_malloc(r,sizeof(*_tmp5));_tmp5->key=(void*)((void*)
_tmp3->key);_tmp5->data=(void*)((void*)_tmp3->data);_tmp5->left=(void*)((void*)
_tmp3->left);_tmp5->right=(void*)((void*)_tmp4);_tmp5;});goto _LL0;}_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpA=_cycalloc(
sizeof(*_tmpA));_tmpA[0]=({struct Cyc_Core_Invalid_argument_struct _tmpB;_tmpB.tag=
Cyc_Core_Invalid_argument;_tmpB.f1=({const char*_tmpC="Splay::rotate_left";
_tag_arr(_tmpC,sizeof(char),_get_zero_arr_size(_tmpC,19));});_tmpB;});_tmpA;}));
_LL0:;}static void Cyc_Splay_rotate_right(struct _RegionHandle*r,struct Cyc_Splay_noderef*
nr){struct Cyc_Splay_node*_tmpD=nr->v;void*_tmpE=(void*)_tmpD->right;struct Cyc_Splay_noderef*
_tmpF;_LL6: if(_tmpE <= (void*)1?1:*((int*)_tmpE)!= 0)goto _LL8;_tmpF=((struct Cyc_Splay_Node_struct*)
_tmpE)->f1;_LL7: {struct Cyc_Splay_node*_tmp10=_tmpF->v;struct Cyc_Splay_Node_struct*
_tmp11=({struct Cyc_Splay_Node_struct*_tmp13=_region_malloc(r,sizeof(*_tmp13));
_tmp13[0]=({struct Cyc_Splay_Node_struct _tmp14;_tmp14.tag=0;_tmp14.f1=({struct Cyc_Splay_noderef*
_tmp15=_region_malloc(r,sizeof(*_tmp15));_tmp15->v=({struct Cyc_Splay_node*_tmp16=
_region_malloc(r,sizeof(*_tmp16));_tmp16->key=(void*)((void*)_tmpD->key);_tmp16->data=(
void*)((void*)_tmpD->data);_tmp16->left=(void*)((void*)_tmpD->left);_tmp16->right=(
void*)((void*)_tmp10->left);_tmp16;});_tmp15;});_tmp14;});_tmp13;});nr->v=({
struct Cyc_Splay_node*_tmp12=_region_malloc(r,sizeof(*_tmp12));_tmp12->key=(void*)((
void*)_tmp10->key);_tmp12->data=(void*)((void*)_tmp10->data);_tmp12->left=(void*)((
void*)_tmp11);_tmp12->right=(void*)((void*)_tmp10->right);_tmp12;});goto _LL5;}
_LL8:;_LL9:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp17=
_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp18;_tmp18.tag=Cyc_Core_Invalid_argument;_tmp18.f1=({const char*_tmp19="Splay::rotate_right";
_tag_arr(_tmp19,sizeof(char),_get_zero_arr_size(_tmp19,20));});_tmp18;});_tmp17;}));
_LL5:;}struct _tuple0{void*f1;struct Cyc_Splay_noderef*f2;};static void Cyc_Splay_lift(
struct _RegionHandle*r,struct Cyc_List_List*dnl){while(dnl != 0){if(dnl->tl == 0){{
struct _tuple0*_tmp1A=(struct _tuple0*)dnl->hd;struct _tuple0 _tmp1B;void*_tmp1C;
struct Cyc_Splay_noderef*_tmp1D;struct _tuple0 _tmp1E;void*_tmp1F;struct Cyc_Splay_noderef*
_tmp20;_LLB: _tmp1B=*_tmp1A;_tmp1C=_tmp1B.f1;if((int)_tmp1C != 0)goto _LLD;_tmp1D=
_tmp1B.f2;_LLC: Cyc_Splay_rotate_left(r,_tmp1D);goto _LLA;_LLD: _tmp1E=*_tmp1A;
_tmp1F=_tmp1E.f1;if((int)_tmp1F != 1)goto _LLA;_tmp20=_tmp1E.f2;_LLE: Cyc_Splay_rotate_right(
r,_tmp20);goto _LLA;_LLA:;}return;}{struct _tuple0 _tmp22;void*_tmp23;struct Cyc_Splay_noderef*
_tmp24;struct _tuple0*_tmp21=(struct _tuple0*)dnl->hd;_tmp22=*_tmp21;_tmp23=_tmp22.f1;
_tmp24=_tmp22.f2;{struct _tuple0 _tmp26;void*_tmp27;struct Cyc_Splay_noderef*_tmp28;
struct _tuple0*_tmp25=(struct _tuple0*)((struct Cyc_List_List*)_check_null(dnl->tl))->hd;
_tmp26=*_tmp25;_tmp27=_tmp26.f1;_tmp28=_tmp26.f2;dnl=((struct Cyc_List_List*)
_check_null(dnl->tl))->tl;{void*_tmp29=_tmp23;_LL10: if((int)_tmp29 != 0)goto _LL12;
_LL11:{void*_tmp2A=_tmp27;_LL15: if((int)_tmp2A != 0)goto _LL17;_LL16: Cyc_Splay_rotate_left(
r,_tmp28);Cyc_Splay_rotate_left(r,_tmp28);goto _LL14;_LL17: if((int)_tmp2A != 1)
goto _LL14;_LL18: Cyc_Splay_rotate_left(r,_tmp24);Cyc_Splay_rotate_right(r,_tmp28);
goto _LL14;_LL14:;}goto _LLF;_LL12: if((int)_tmp29 != 1)goto _LLF;_LL13:{void*_tmp2B=
_tmp27;_LL1A: if((int)_tmp2B != 0)goto _LL1C;_LL1B: Cyc_Splay_rotate_right(r,_tmp24);
Cyc_Splay_rotate_left(r,_tmp28);goto _LL19;_LL1C: if((int)_tmp2B != 1)goto _LL19;
_LL1D: Cyc_Splay_rotate_right(r,_tmp28);Cyc_Splay_rotate_right(r,_tmp28);goto
_LL19;_LL19:;}goto _LLF;_LLF:;}}}}}int Cyc_Splay_rsplay(struct _RegionHandle*r,int(*
reln)(void*,void*),void*reln_first_arg,void*tree){struct _RegionHandle _tmp2C=
_new_region("temp");struct _RegionHandle*temp=& _tmp2C;_push_region(temp);{struct
Cyc_List_List*path=0;while(1){void*_tmp2D=tree;struct Cyc_Splay_noderef*_tmp2E;
_LL1F: if(_tmp2D <= (void*)1?1:*((int*)_tmp2D)!= 0)goto _LL21;_tmp2E=((struct Cyc_Splay_Node_struct*)
_tmp2D)->f1;_LL20: {struct Cyc_Splay_node*_tmp2F=_tmp2E->v;int _tmp30=reln(
reln_first_arg,(void*)_tmp2F->key);if(_tmp30 == 0){Cyc_Splay_lift(r,path);{int
_tmp31=1;_npop_handler(0);return _tmp31;}}else{if(_tmp30 < 0){path=({struct Cyc_List_List*
_tmp32=_region_malloc(temp,sizeof(*_tmp32));_tmp32->hd=({struct _tuple0*_tmp33=
_region_malloc(temp,sizeof(*_tmp33));_tmp33->f1=(void*)0;_tmp33->f2=_tmp2E;
_tmp33;});_tmp32->tl=path;_tmp32;});tree=(void*)_tmp2F->left;}else{path=({struct
Cyc_List_List*_tmp34=_region_malloc(temp,sizeof(*_tmp34));_tmp34->hd=({struct
_tuple0*_tmp35=_region_malloc(temp,sizeof(*_tmp35));_tmp35->f1=(void*)1;_tmp35->f2=
_tmp2E;_tmp35;});_tmp34->tl=path;_tmp34;});tree=(void*)_tmp2F->right;}}goto _LL1E;}
_LL21: if((int)_tmp2D != 0)goto _LL1E;_LL22: if(path != 0)Cyc_Splay_lift(r,path->tl);{
int _tmp36=0;_npop_handler(0);return _tmp36;}_LL1E:;}};_pop_region(temp);}int Cyc_Splay_splay(
int(*reln)(void*,void*),void*reln_first_arg,void*tree){return Cyc_Splay_rsplay(
Cyc_Core_heap_region,reln,reln_first_arg,tree);}
