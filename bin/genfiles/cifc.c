#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/* Need one of these per thread (see runtime_stack.c). The runtime maintains 
   a stack that contains either _handler_cons structs or _RegionHandle structs.
   The tag is 0 for a handler_cons and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; 
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

#ifndef offsetof
/* should be size_t but int is fine */
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions; //not used
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame { //not used
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

/* Reaps */
struct _ReapPage
#ifdef CYC_REGION_PROFILE
{ unsigned direct_flag;
  unsigned total_bytes;
  unsigned free_bytes;
  void *bget_page;
  struct _ReapPage *next;
}
#endif
; 
struct bget_region_key;
struct _ReapHandle {
  struct _RuntimeStack s;
  struct _ReapPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _pool *released_ptrs;
  struct bget_region_key *bkey;
#ifdef CYC_REGION_PROFILE
  unsigned int id;
  const char         *name;
#endif
};

// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct Cyc_Core_DynamicReap {
  struct _ReapHandle h;
};

/* Alias qualifier stuff */
typedef unsigned char _AliasQualHandle_t;

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

struct _ReapHandle _new_reap(const char*);
void* _reap_malloc(struct _ReapHandle*, _AliasQualHandle_t, unsigned);
void* _reap_calloc(struct _ReapHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void * _reap_aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _reap_aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_reap(struct _ReapHandle*);

struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);//not used
void _pop_dynregion(); //not used

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
void _npop_handler(int);
void _pop_handler();
void _pop_region();

void _push_reap(struct _ReapHandle*);

#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char*)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr);\
  int _index = (index);\
  if (!_cks_ptr) _throw_null(); \
  _cks_ptr + (elt_sz)*_index; })
#endif
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })

/* _zero_arr_plus_*_fn(x,sz,i,filename,lineno) adds i to zero-terminated ptr
   x that has at least sz elements */
char* _zero_arr_plus_char_fn(char*,unsigned,int,const char*,unsigned);
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
#endif

#define _tag_fat(tcurr,elt_sz,num_elts) ({ \
  struct _fat_ptr _ans; \
  unsigned _num_elts = (num_elts);\
  _ans.base = _ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _ans.last_plus_one = _ans.base ? (_ans.base + (elt_sz) * _num_elts) : 0; \
  _ans; })

#define _get_fat_size(arr,elt_sz) \
  ({struct _fat_ptr _arr = (arr); \
    unsigned char *_arr_curr=_arr.curr; \
    unsigned char *_arr_last=_arr.last_plus_one; \
    (_arr_curr < _arr.base || _arr_curr >= _arr_last) ? 0 : \
    ((_arr_last - _arr_curr) / (elt_sz));})

#define _fat_ptr_plus(arr,elt_sz,change) ({ \
  struct _fat_ptr _ans = (arr); \
  int _change = (change);\
  _ans.curr += (elt_sz) * _change;\
  _ans; })
#define _fat_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += (elt_sz) * (change);\
  *_arr_ptr; })
#define _fat_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  struct _fat_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += (elt_sz) * (change);\
  _ans; })

//Not a macro since initialization order matters. Defined in runtime_zeroterm.c.
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr,unsigned sz,unsigned numelts);

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void*);
#endif
// bound the allocation size to be < MAX_ALLOC_SIZE. See macros below for usage.
#define MAX_MALLOC_SIZE (1 << 28)
void* _bounded_GC_malloc(int,const char*,int);
void* _bounded_GC_malloc_atomic(int,const char*,int);
void* _bounded_GC_calloc(unsigned,unsigned,const char*,int);
void* _bounded_GC_calloc_atomic(unsigned,unsigned,const char*,int);
/* these macros are overridden below ifdef CYC_REGION_PROFILE */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long) x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast_reap_malloc
// static inline void*_fast_reap_malloc(struct _ReapHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
//   if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
// #ifdef CYC_NOALIGN
//     unsigned s =  orig_s;
// #else
//     unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
// #endif
//     char *result; 
//     result = r->offset; 
//     if (s <= (r->last_plus_one - result)) {
//       r->offset = result + s; 
// #ifdef CYC_REGION_PROFILE
//     r->curr->free_bytes = r->curr->free_bytes - s;
//     rgn_total_bytes += s;
// #endif
//       return result;
//     }
//   } 
//   return _region_malloc(r,aq,orig_s); 
// }

//migration to reaps -- Remove this block to revert to regions 
//... but the rufree etc. will not work
#ifndef RUNTIME_CYC
#define _new_region(n) _new_reap(n)
#define _free_region(r) _free_reap(r)
#define _region_malloc(rh,aq,n) _reap_malloc(rh,aq,n)
#define _region_calloc(rh,aq,n,t) _reap_calloc(rh,aq,n,t)
#define _push_region(rh) _push_reap(rh)
#define _aqual_malloc(aq,s) _reap_aqual_malloc(aq, s)
#define _aqual_calloc(aq,s,i) _reap_aqual_calloc(aq, s, i)
#define _RegionHandle _ReapHandle
#define Cyc_Core_NewDynamicRegion Cyc_Core_NewDynamicReap
#define Cyc_Core__new_ukey(f,fn,l) Cyc_Core__reap_new_ukey(f, fn, l)
#define Cyc_Core__new_rckey(f,fn,l) Cyc_Core__reap_new_rckey(f, fn, l)
#define Cyc_Core_free_ukey(k) Cyc_Core_reap_free_ukey(k)
#define Cyc_Core_free_rckey(k) Cyc_Core_reap_free_rckey(k)
#define Cyc_Core_open_region Cyc_Core_reap_open_region
#endif 

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_region_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_region_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 349
extern struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*,void*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 182
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 868 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 879
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 920
extern void*Cyc_Absyn_void_type;
# 967
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1095
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1142
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1148
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);
# 1150
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*);
# 75
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 91
void*Cyc_Tcutil_copy_type(void*);
# 82 "attributes.h"
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 93 "kinds.h"
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 42 "cifc.cyc"
static int Cyc_Cifc_glob_tvar_id=0;
int Cyc_Cifc_opt_inst_tvar=0;
# 45
static struct _fat_ptr Cyc_Cifc_get_type_kind(void*t){
switch(*((int*)t)){case 0:
 return _tag_fat("AppType",sizeof(char),8U);case 1:
 return _tag_fat("Evar",sizeof(char),5U);case 2:
 return _tag_fat("Vartype",sizeof(char),8U);case 3:
 return _tag_fat("Pointertype",sizeof(char),12U);case 4:
 return _tag_fat("ArrayType",sizeof(char),10U);case 5:
 return _tag_fat("FnType",sizeof(char),7U);case 6:
 return _tag_fat("AnonAggrType",sizeof(char),13U);case 7:
 return _tag_fat("Typedeftype",sizeof(char),12U);case 8:
 return _tag_fat("Valueoftype",sizeof(char),12U);case 9:
 return _tag_fat("Typedecltype",sizeof(char),13U);default:
 return _tag_fat("Typeoftype",sizeof(char),11U);};}
# 61
static int Cyc_Cifc_is_concrete(struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 8: _LL4:
 return 1;case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
return ad->impl!=0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
return ed->fields!=0;}default:
 return 0;};}
# 72
static struct Cyc_Absyn_Decl*Cyc_Cifc_lookup_concrete_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
# 74
struct Cyc_List_List*indecls=decls;
for(1;decls!=0;decls=decls->tl){
struct _fat_ptr*dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if(((unsigned)dname && !Cyc_strptrcmp(dname,name))&& Cyc_Cifc_is_concrete((struct Cyc_Absyn_Decl*)decls->hd))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 80
return Cyc_Absyn_lookup_decl(indecls,name);}
# 84
void Cyc_Cifc_set_inst_tvar (void){
Cyc_Cifc_opt_inst_tvar=1;}
# 88
struct _fat_ptr Cyc_Cifc_list2string(struct Cyc_List_List*l,struct _fat_ptr(*tostr)(void*)){
struct _fat_ptr ret=Cyc_aprintf(_tag_fat("(",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
for(1;(unsigned)l;l=l->tl){
ret=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)_check_null(tostr)(l->hd));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s %s,",sizeof(char),7U),_tag_fat(_Tmp2,sizeof(void*),2));});}
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)ret);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s)",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 97
static void Cyc_Cifc_fail_merge(int warn,unsigned loc,int is_buildlib,struct _tuple0*n,struct _fat_ptr s){
# 99
if(is_buildlib){
struct _fat_ptr preamble=warn?_tag_fat("Warning: user-defined",sizeof(char),22U): _tag_fat("User-defined",sizeof(char),13U);
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)preamble);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
# 103
struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(n));_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("%s type for %s incompatible with system version %s\n",sizeof(char),52U),_tag_fat(_Tmp3,sizeof(void*),3));});}else{
# 105
if(warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("User-defined type for ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=n;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" incompatible with system version ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_warn2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});else{
# 108
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("User-defined type for ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=n;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" incompatible with system version ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}}}
# 113
static void*Cyc_Cifc_expand_c_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp){
# 115
void*_Tmp0;int _Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;void*_Tmp3;switch(*((int*)ctyp)){case 7:
# 118
 Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
{void*_Tmp4;if(*((int*)ctyp)==7){_Tmp4=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)ctyp)->f4;{void*to=_Tmp4;
# 122
return _check_null(to);}}else{
# 124
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Impos",sizeof(char),6U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL17;}_LL17:;}
# 127
return ctyp;case 5: _Tmp3=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)ctyp)->f1;{struct Cyc_Absyn_FnInfo*finfo=_Tmp3;
# 129
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 131
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(finfo->ret_type));});finfo->ret_type=_Tmp4;});{
struct Cyc_List_List*args=finfo->args;
while((unsigned)args){
struct _tuple8*_Tmp4=(struct _tuple8*)args->hd;void*_Tmp5;_Tmp5=(void**)& _Tmp4->f2;{void**argType=(void**)_Tmp5;
({void*_Tmp6=({unsigned _Tmp7=loc;struct Cyc_Tcenv_Tenv*_Tmp8=te;Cyc_Cifc_expand_c_type(_Tmp7,_Tmp8,Cyc_Toc_typ_to_c(*argType));});*argType=_Tmp6;});
args=args->tl;}}
# 139
finfo->tvars=0;
finfo->effect=0;
return ctyp;}}case 3: _Tmp3=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)ctyp)->f1;{struct Cyc_Absyn_PtrInfo*pinfo=_Tmp3;
# 145
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(pinfo->elt_type));});pinfo->elt_type=_Tmp4;});
return ctyp;}case 4: _Tmp3=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)ctyp)->f1;{struct Cyc_Absyn_ArrayInfo*ainfo=_Tmp3;
# 148
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
# 150
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(ainfo->elt_type));});ainfo->elt_type=_Tmp4;});
return ctyp;}case 6: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)ctyp)->f1;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)ctyp)->f2;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)ctyp)->f3;{enum Cyc_Absyn_AggrKind knd=_Tmp2;int is_tuple=_Tmp1;struct Cyc_List_List*flst=_Tmp3;
# 154
if(is_tuple){
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ctyp);
for(1;flst!=0;flst=_check_null(flst)->tl){
void**elt_type=&((struct Cyc_Absyn_Aggrfield*)flst->hd)->type;
({void*_Tmp4=({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Cifc_expand_c_type(_Tmp5,_Tmp6,Cyc_Toc_typ_to_c(*elt_type));});*elt_type=_Tmp4;});
flst=flst->tl;}}
# 162
return ctyp;}case 9: _Tmp3=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)ctyp)->f1;_Tmp0=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)ctyp)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp3;void**tptr=_Tmp0;
# 165
return ctyp;}case 8: _Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)ctyp)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 168
return ctyp;}case 10: _Tmp3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)ctyp)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 171
return ctyp;}case 0:
# 174
 return ctyp;case 1:
# 177
 return ctyp;default:
# 180
 return ctyp;};}
# 184
static int Cyc_Cifc_is_boxed_kind(void*t){
# 186
void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 0: _Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*tc=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 188
{enum Cyc_Absyn_Size_of _Tmp2;switch(*((int*)tc)){case 1: _Tmp2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)tc)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp2;
# 190
return(int)sz==2 ||(int)sz==3;}case 4:
 goto _LLF;case 5: _LLF:
# 193
 return 1;default:
# 195
 return 0;};}
# 197
goto _LL0;}case 3: _Tmp1=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp1;
# 199
{void*_Tmp2=pi->ptr_atts.bounds;void*_Tmp3;if(*((int*)_Tmp2)==0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1;{void*tc=_Tmp3;
# 201
if(*((int*)tc)==15)
# 203
return 0;else{
# 205
return 1;};}}else{
# 208
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("ptrbound_t must have an AppType",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));};}
# 210
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 212
return 1;}default:
# 214
 return 0;}_LL0:;}
# 219
static int Cyc_Cifc_check_fntype_kinds(unsigned,struct Cyc_Tcenv_Tenv*,void*,void*);
# 225
static int Cyc_Cifc_c_types_ok(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*ctyp,void*cyctyp){
struct Cyc_Absyn_Tqual tq;
# 228
void*ctyp_cp=Cyc_Tcutil_copy_type(ctyp);
void*utyp_cp=Cyc_Tcutil_copy_type(cyctyp);
# 231
void*c_typ=Cyc_Toc_typ_to_c(ctyp_cp);
void*u_typ=Cyc_Toc_typ_to_c(utyp_cp);
# 236
if(!Cyc_Unify_unify(c_typ,u_typ)){
c_typ=Cyc_Cifc_expand_c_type(loc,te,c_typ);
u_typ=Cyc_Cifc_expand_c_type(loc,te,u_typ);
if(!Cyc_Unify_unify(c_typ,u_typ)){
# 242
if(Cyc_Tcutil_is_function_type(c_typ))
return Cyc_Cifc_check_fntype_kinds(loc,te,c_typ,u_typ);
return Cyc_Cifc_is_boxed_kind(c_typ)&& Cyc_Cifc_is_boxed_kind(u_typ);}}
# 247
return 1;}struct _tuple11{void*f0;void*f1;};
# 259 "cifc.cyc"
static int Cyc_Cifc_check_fntype_kinds(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*c_typ,void*u_typ){
struct _tuple11 _Tmp0=({struct _tuple11 _Tmp1;_Tmp1.f0=c_typ,_Tmp1.f1=u_typ;_Tmp1;});struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_FnInfo cfi=_Tmp2;struct Cyc_Absyn_FnInfo ufi=_Tmp1;
# 262
int typesOk=Cyc_Cifc_c_types_ok(loc,te,cfi.ret_type,ufi.ret_type);
if(typesOk){
struct Cyc_List_List*ca=cfi.args;
struct Cyc_List_List*ua=ufi.args;
while((unsigned)ca && typesOk){
if(!((unsigned)ua))
return 0;
typesOk=Cyc_Cifc_c_types_ok(loc,te,(*((struct _tuple8*)ca->hd)).f2,(*((struct _tuple8*)ua->hd)).f2);
# 271
ca=ca->tl;
ua=ua->tl;}}
# 275
return typesOk;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
# 278
 return 0;}_LL0:;}struct _tuple12{struct Cyc_Absyn_AggrdeclImpl*f0;struct Cyc_Absyn_AggrdeclImpl*f1;};
# 285
void Cyc_Cifc_merge_sys_user_decl(unsigned loc,struct Cyc_Tcenv_Tenv*te,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
# 287
struct _tuple11 _Tmp0=({struct _tuple11 _Tmp1;_Tmp1.f0=c_decl->r,_Tmp1.f1=user_decl->r;_Tmp1;});void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*cd=_Tmp2;struct Cyc_Absyn_Vardecl*ud=_Tmp1;
# 289
if(!Cyc_Cifc_c_types_ok(loc,te,cd->type,ud->type)){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=cd->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cd->type));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat(": type %s != %s",sizeof(char),16U),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 297
if(ud->attributes!=0)
({struct Cyc_List_List*_Tmp3=Cyc_Atts_merge_attributes(cd->attributes,ud->attributes);cd->attributes=_Tmp3;});
cd->type=ud->type;
cd->tq=ud->tq;}
# 302
goto _LL0;}}else{goto _LLB;}case 1: if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Fndecl*cd=_Tmp2;struct Cyc_Absyn_Vardecl*ud=_Tmp1;
# 305
if(!Cyc_Tcutil_is_function_type(ud->type)){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,ud->name,_tag_fat(": type must be a function type to match decl\n",sizeof(char),46U));if(!0)return;}{
# 308
void*cdtype;
if(cd->cached_type!=0)
cdtype=_check_null(cd->cached_type);else{
# 312
cdtype=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp3->tag=5,_Tmp3->f1=cd->i;_Tmp3;});}
if(!Cyc_Cifc_c_types_ok(loc,te,cdtype,ud->type)){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=ud->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cdtype));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ud->type));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat(": type %s != %s",sizeof(char),16U),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 321
void*_Tmp3=ud->type;struct Cyc_Absyn_FnInfo _Tmp4;if(*((int*)_Tmp3)==5){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp4;
# 323
struct Cyc_List_List*old_tvars=fi.tvars;
Cyc_Tctyp_check_valid_toplevel_type(loc,te,ud->type);
if(cd->i.attributes!=0)
({struct Cyc_List_List*_Tmp5=Cyc_Atts_merge_attributes(fi.attributes,cd->i.attributes);fi.attributes=_Tmp5;});
cd->i=fi;
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("oops!\n",sizeof(char),7U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(0U,_tag_fat(_Tmp6,sizeof(void*),1));});}_LLD:;}
# 332
goto _LL0;}}}else{goto _LLB;}case 5: if(*((int*)_Tmp0.f1)==5){_Tmp2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Aggrdecl*cd=_Tmp2;struct Cyc_Absyn_Aggrdecl*ud=_Tmp1;
# 340
if((int)ud->sc!=(int)cd->sc){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,_tag_fat(": scopes don't match (ignoring)",sizeof(char),32U));if(!1)return;}
# 343
if(ud->impl==0){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,_tag_fat(": no user definition",sizeof(char),21U));if(!0)return;}
if(cd->impl==0){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,_tag_fat(": no definition for system version",sizeof(char),35U));if(!1)return;
c_decl->r=user_decl->r;
return;}
# 350
{struct _tuple12 _Tmp3=({struct _tuple12 _Tmp4;_Tmp4.f0=cd->impl,_Tmp4.f1=ud->impl;_Tmp4;});int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;if(_Tmp3.f0!=0){if(_Tmp3.f1!=0){_TmpC=(struct Cyc_List_List**)& _Tmp3.f0->exist_vars;_TmpB=(struct Cyc_List_List**)& _Tmp3.f0->rgn_po;_TmpA=(struct Cyc_List_List**)& _Tmp3.f0->qual_bnd;_Tmp9=_Tmp3.f0->fields;_Tmp8=_Tmp3.f1->exist_vars;_Tmp7=_Tmp3.f1->rgn_po;_Tmp6=_Tmp3.f1->qual_bnd;_Tmp5=_Tmp3.f1->fields;_Tmp4=_Tmp3.f1->tagged;{struct Cyc_List_List**tvarsC=(struct Cyc_List_List**)_TmpC;struct Cyc_List_List**rgnpoC=(struct Cyc_List_List**)_TmpB;struct Cyc_List_List**qbC=(struct Cyc_List_List**)_TmpA;struct Cyc_List_List*cfields=_Tmp9;struct Cyc_List_List*tvars=_Tmp8;struct Cyc_List_List*rgnpo=_Tmp7;struct Cyc_List_List*qb=_Tmp6;struct Cyc_List_List*ufields=_Tmp5;int tagged=_Tmp4;
# 353
if(tagged){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,_tag_fat(": user @tagged annotation not allowed (ignoring)",sizeof(char),49U));if(!0)return;}{
# 356
struct Cyc_List_List*x=cfields;
# 358
while(x!=0){
struct Cyc_Absyn_Aggrfield*cfield=(struct Cyc_Absyn_Aggrfield*)x->hd;
struct Cyc_Absyn_Aggrfield*ufield=Cyc_Absyn_lookup_field(ufields,cfield->name);
# 362
if(ufield!=0){
# 365
if(!Cyc_Cifc_c_types_ok(loc,te,cfield->type,ufield->type)){
({unsigned _TmpD=loc;int _TmpE=is_buildlib;struct _tuple0*_TmpF=cd->name;Cyc_Cifc_fail_merge(0,_TmpD,_TmpE,_TmpF,({struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,({struct _fat_ptr _Tmp12=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_Tmp11.f1=_Tmp12;});_Tmp11;});struct Cyc_String_pa_PrintArg_struct _Tmp11=({struct Cyc_String_pa_PrintArg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=(struct _fat_ptr)((struct _fat_ptr)*cfield->name);_Tmp12;});struct Cyc_String_pa_PrintArg_struct _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13;_Tmp13.tag=0,({struct _fat_ptr _Tmp14=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(cfield->type));_Tmp13.f1=_Tmp14;});_Tmp13;});void*_Tmp13[3];_Tmp13[0]=& _Tmp10,_Tmp13[1]=& _Tmp11,_Tmp13[2]=& _Tmp12;Cyc_aprintf(_tag_fat(": type %s of user definition of field %s != %s",sizeof(char),47U),_tag_fat(_Tmp13,sizeof(void*),3));}));});if(!0)return;}else{
# 374
if(ufield->width!=0){
({unsigned _TmpD=loc;int _TmpE=is_buildlib;struct _tuple0*_TmpF=cd->name;Cyc_Cifc_fail_merge(1,_TmpD,_TmpE,_TmpF,({struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,({struct _fat_ptr _Tmp12=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(ufield->type));_Tmp11.f1=_Tmp12;});_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;Cyc_aprintf(_tag_fat(": ignoring width of user definition of field %s",sizeof(char),48U),_tag_fat(_Tmp11,sizeof(void*),1));}));});if(!1)return;}
# 379
if(ufield->attributes!=0)
({struct Cyc_List_List*_TmpD=Cyc_Atts_merge_attributes(cfield->attributes,ufield->attributes);cfield->attributes=_TmpD;});
# 383
cfield->type=ufield->type;
cfield->tq=ufield->tq;
cfield->requires_clause=ufield->requires_clause;}}
# 388
x=x->tl;}
# 392
if(ud->tvs!=0)cd->tvs=ud->tvs;
if((unsigned)tvars)*tvarsC=tvars;
if((unsigned)rgnpo)*rgnpoC=rgnpo;
if((unsigned)qb)*qbC=qb;
# 398
x=ufields;{
int missing_fields=0;
while(x!=0){
struct Cyc_Absyn_Aggrfield*cfield=Cyc_Absyn_lookup_field(cfields,((struct Cyc_Absyn_Aggrfield*)x->hd)->name);
if(cfield==0){
missing_fields=1;
({unsigned _TmpD=loc;int _TmpE=is_buildlib;struct _tuple0*_TmpF=cd->name;Cyc_Cifc_fail_merge(1,_TmpD,_TmpE,_TmpF,({struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)x->hd)->name);_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;Cyc_aprintf(_tag_fat(": no definition of field %s in system version",sizeof(char),46U),_tag_fat(_Tmp11,sizeof(void*),1));}));});if(!1)return;}
# 408
x=x->tl;}
# 410
goto _LL12;}}}}else{goto _LL15;}}else{_LL15:
# 412
 _throw((void*)({struct Cyc_Core_Impossible_exn_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_TmpD->tag=Cyc_Core_Impossible,_TmpD->f1=_tag_fat("Internal Error: NULL cases not possible",sizeof(char),40U);_TmpD;}));}_LL12:;}
# 414
goto _LL0;}}else{goto _LLB;}case 7: if(*((int*)_Tmp0.f1)==7){_Tmp2=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Enumdecl*cd=_Tmp2;struct Cyc_Absyn_Enumdecl*ud=_Tmp1;
# 417
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,_tag_fat(": enum merging not currently supported",sizeof(char),39U));if(!0)return;}}else{goto _LLB;}case 8: if(*((int*)_Tmp0.f1)==8){_Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Typedefdecl*cd=_Tmp2;struct Cyc_Absyn_Typedefdecl*ud=_Tmp1;
# 420
if(ud->defn==0){
Cyc_Cifc_fail_merge(0,loc,is_buildlib,cd->name,_tag_fat(": no user definition",sizeof(char),21U));if(!0)return;}
if(cd->defn==0){
Cyc_Cifc_fail_merge(1,loc,is_buildlib,cd->name,_tag_fat(": no definition for system version",sizeof(char),35U));if(!1)return;
c_decl->r=user_decl->r;
return;}
# 428
if(!({unsigned _Tmp3=loc;struct Cyc_Tcenv_Tenv*_Tmp4=te;void*_Tmp5=_check_null(cd->defn);Cyc_Cifc_c_types_ok(_Tmp3,_Tmp4,_Tmp5,_check_null(ud->defn));})){
({unsigned _Tmp3=loc;int _Tmp4=is_buildlib;struct _tuple0*_Tmp5=cd->name;Cyc_Cifc_fail_merge(0,_Tmp3,_Tmp4,_Tmp5,({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_check_null(ud->defn)));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(_check_null(cd->defn)));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat(": type definition %s of user definition != %s",sizeof(char),46U),_tag_fat(_Tmp8,sizeof(void*),2));}));});if(!0)return;}else{
# 435
cd->tvs=ud->tvs;
cd->defn=ud->defn;
cd->tq=ud->tq;
if(ud->atts!=0)
({struct Cyc_List_List*_Tmp3=Cyc_Atts_merge_attributes(cd->atts,ud->atts);cd->atts=_Tmp3;});}
# 442
goto _LL0;}}else{goto _LLB;}default: _LLB:
# 445
 if(is_buildlib)
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=user_decl;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));})));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("Error in .cys file: bad (or unsupported) user-defined type %s\n",sizeof(char),63U),_tag_fat(_Tmp4,sizeof(void*),1));});else{
# 449
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("bad (or unsupported) user-defined type %s",sizeof(char),42U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_Tmp7[1];_Tmp7[0]=user_decl;Cyc_List_list(_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Decl*),1));}));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),2));});}
return;}_LL0:;}
# 455
static int Cyc_Cifc_contains_type_vars(struct Cyc_Absyn_Decl*ud){
{void*_Tmp0=ud->r;void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 458
return ad->tvs!=0;}}else{
# 460
goto _LL0;}_LL0:;}
# 462
return 0;}
# 465
static struct Cyc_Absyn_Decl*Cyc_Cifc_make_abstract_decl(struct Cyc_Absyn_Decl*ud){
void*_Tmp0=ud->r;void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 468
struct Cyc_Absyn_Aggrdecl*absad;absad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),*absad=*ad;
absad->impl=0;{
struct Cyc_Absyn_Decl*nd=({void*_Tmp2=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp3->tag=5,_Tmp3->f1=absad;_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,ud->loc);});
return nd;}}}else{
# 473
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Only aggrdecls",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));};}
# 478
static int Cyc_Cifc_kindbound_subsumed(void*kb1,void*kb2){
void*_Tmp0;void*_Tmp1;switch(*((int*)kb1)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb1)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp1;
# 481
void*_Tmp2;if(*((int*)kb2)==0){_Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb2)->f1;{struct Cyc_Absyn_Kind*k2=_Tmp2;
# 483
return Cyc_Kinds_kind_eq(k1,k2);}}else{
# 485
return 0;};}case 1: if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb1)->f1!=0){_Tmp1=(void*)((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb1)->f1->v;{void*kbb1=_Tmp1;
# 488
void*_Tmp2;if(*((int*)kb2)==1){if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb2)->f1!=0){_Tmp2=(void*)((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb2)->f1->v;{void*kbb2=_Tmp2;
# 490
return Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL13;}}else{_LL13:
# 492
 return 1;};}}else{
# 495
return 1;}default: if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f1!=0){_Tmp1=(void*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f1->v;_Tmp0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f2;{void*kbb1=_Tmp1;struct Cyc_Absyn_Kind*k1=_Tmp0;
# 497
void*_Tmp2;void*_Tmp3;if(*((int*)kb2)==2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f1!=0){_Tmp3=(void*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f1->v;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f2;{void*kbb2=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 499
return Cyc_Kinds_kind_eq(k1,k2)&& Cyc_Cifc_kindbound_subsumed(kbb1,kbb2);}}else{goto _LL18;}}else{_LL18:
# 501
 return 0;};}}else{_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f2;{struct Cyc_Absyn_Kind*k1=_Tmp1;
# 504
void*_Tmp2;if(*((int*)kb2)==2){if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f1==0){_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f2;{struct Cyc_Absyn_Kind*k2=_Tmp2;
# 506
return Cyc_Kinds_kind_eq(k1,k2);}}else{goto _LL1D;}}else{_LL1D:
# 508
 return 0;};}}};}
# 513
static int Cyc_Cifc_find_and_remove(struct Cyc_List_List**lst,void*kind){
struct Cyc_List_List*cur=*lst;
struct Cyc_List_List*prev=0;
while((unsigned)cur){
void*t=(void*)cur->hd;
{void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp0;
# 520
if(Cyc_Cifc_kindbound_subsumed(tv->kind,kind)){
if((unsigned)prev)
prev->tl=cur->tl;else{
# 524
*lst=cur->tl;}
cur->tl=0;
return 1;}
# 528
goto _LL0;}}else{
# 530
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Cifc_get_type_kind(t));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("expects a VarType list only -- got %s(%s)",sizeof(char),42U),_tag_fat(_Tmp3,sizeof(void*),2));});}_LL0:;}
# 532
prev=cur;
cur=cur->tl;}
# 535
return 0;}
# 538
static struct Cyc_List_List*Cyc_Cifc_get_tvar_difference(struct Cyc_List_List*tvs,struct Cyc_List_List*remove){
struct Cyc_List_List*ret=0;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*can=(struct Cyc_Absyn_Tvar*)tvs->hd;
if(!Cyc_Cifc_find_and_remove(& remove,can->kind))
ret=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=can,_Tmp0->tl=ret;_Tmp0;});
tvs=tvs->tl;}
# 546
return ret;}char Cyc_Cifc_Contains_nontvar[17U]="Contains_nontvar";struct Cyc_Cifc_Contains_nontvar_exn_struct{char*tag;};
# 551
struct Cyc_Cifc_Contains_nontvar_exn_struct Cyc_Cifc_Contains_nontvar_val={Cyc_Cifc_Contains_nontvar};
# 553
static struct Cyc_List_List*Cyc_Cifc_extract_tvars(struct Cyc_List_List*ts){
struct Cyc_List_List*res=0;
while((unsigned)ts){
void*t=(void*)ts->hd;
{void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp0;
res=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=tv,_Tmp1->tl=res;_Tmp1;});goto _LL0;}}else{
_throw(& Cyc_Cifc_Contains_nontvar_val);}_LL0:;}
# 561
ts=ts->tl;}
# 563
return res;}
# 566
static void*Cyc_Cifc_instantiate_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
{void*_Tmp0=tv->kind;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
# 570
if((int)k->kind==2 ||(int)k->kind==0)
return({void*_Tmp2=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_Tmp2,Cyc_Absyn_empty_tqual(loc));});else{
if((int)k->kind==3)
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp2->tag=0,({void*_Tmp3=(void*)({struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct));_Tmp4->tag=6;_Tmp4;});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});}
goto _LL0;}default:
# 576
 return({void*_Tmp2=Cyc_Absyn_void_type;Cyc_Absyn_cstar_type(_Tmp2,Cyc_Absyn_empty_tqual(loc));});}_LL0:;}
# 578
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Unable to instantiate tvar ",sizeof(char),28U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_tvar2string(tv);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_void_type;}struct _tuple13{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 588
static struct _tuple13*Cyc_Cifc_update_tvars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,void*t,struct Cyc_Absyn_Decl*enclosing_decl,int instantiate){
# 592
{struct Cyc_Absyn_FnInfo _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*tc=_Tmp2;struct Cyc_List_List**ts=(struct Cyc_List_List**)_Tmp1;
# 594
{union Cyc_Absyn_AggrInfo _Tmp3;if(*((int*)tc)==25){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)tc)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp3;
# 596
struct _tuple2 _Tmp4;void*_Tmp5;if(ai.KnownAggr.tag==2){_Tmp5=*ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp5;
# 598
struct _tuple0*_Tmp6=ad->name;void*_Tmp7;union Cyc_Absyn_Nmspace _Tmp8;_Tmp8=_Tmp6->f0;_Tmp7=_Tmp6->f1;{union Cyc_Absyn_Nmspace ns=_Tmp8;struct _fat_ptr*name=_Tmp7;
# 601
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);
if((unsigned)ovd){
# 605
void*_Tmp9=ovd->r;void*_TmpA;if(*((int*)_Tmp9)==5){_TmpA=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp9)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_TmpA;
# 607
struct Cyc_List_List*removed_tvars=0;
{struct _handler_cons _TmpB;_push_handler(& _TmpB);{int _TmpC=0;if(setjmp(_TmpB.handler))_TmpC=1;if(!_TmpC){
removed_tvars=Cyc_Cifc_extract_tvars(*ts);;_pop_handler();}else{void*_TmpD=(void*)Cyc_Core_get_exn_thrown();void*_TmpE;if(((struct Cyc_Cifc_Contains_nontvar_exn_struct*)_TmpD)->tag==Cyc_Cifc_Contains_nontvar)
# 614
return 0;else{_TmpE=_TmpD;{void*exn=_TmpE;_rethrow(exn);}};}}}
# 616
*ts=0;{
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*tvs=ad->tvs;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
if(enclosing_decl==ovd)
({struct Cyc_List_List*_TmpB=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TmpE->tag=2,_TmpE->f1=tv;_TmpE;});_TmpC->hd=_TmpD;}),_TmpC->tl=*ts;_TmpC;});*ts=_TmpB;});else{
# 625
if(instantiate || Cyc_Cifc_opt_inst_tvar)
# 627
({struct Cyc_List_List*_TmpB=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=Cyc_Cifc_instantiate_tvar(enclosing_decl->loc,tv);_TmpC->hd=_TmpD;}),_TmpC->tl=*ts;_TmpC;});*ts=_TmpB;});else{
# 630
struct Cyc_Absyn_Tvar*tvcpy;tvcpy=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),*tvcpy=*tv;{
struct _fat_ptr*tvn;tvn=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _TmpB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;Cyc_aprintf(_tag_fat("`ovr_%d",sizeof(char),8U),_tag_fat(_TmpD,sizeof(void*),1));});*tvn=_TmpB;});
tvcpy->name=tvn;
added_tvars=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));_TmpB->hd=tvcpy,_TmpB->tl=added_tvars;_TmpB;});
({struct Cyc_List_List*_TmpB=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TmpE->tag=2,_TmpE->f1=tvcpy;_TmpE;});_TmpC->hd=_TmpD;}),_TmpC->tl=*ts;_TmpC;});*ts=_TmpB;});}}}
# 637
tvs=tvs->tl;}
# 639
({struct Cyc_List_List*_TmpB=Cyc_List_imp_rev(*ts);*ts=_TmpB;});
return((enclosing_decl==ovd || Cyc_Cifc_opt_inst_tvar)|| instantiate)?0:({struct _tuple13*_TmpB=_cycalloc(sizeof(struct _tuple13));_TmpB->f0=added_tvars,_TmpB->f1=removed_tvars;_TmpB;});}}}else{
# 642
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("ovd must be an aggr type",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));};}else{
# 646
return 0;}}}}else{_Tmp4=ai.UnknownAggr.val;{struct _tuple2 ua=_Tmp4;
# 648
return 0;}};}}else{
# 651
goto _LLB;}_LLB:;}
# 653
return 0;}case 3: _Tmp2=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp2;
# 655
return Cyc_Cifc_update_tvars(te,tv_ovrs,pi->elt_type,enclosing_decl,instantiate);}case 5: _Tmp0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp0;
# 657
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
if(Cyc_Cifc_opt_inst_tvar)
Cyc_Cifc_update_tvars(te,tv_ovrs,fi.ret_type,enclosing_decl,instantiate);{
# 662
struct Cyc_List_List*argit=fi.args;
while((unsigned)argit){
struct _tuple8*_Tmp3=(struct _tuple8*)argit->hd;void*_Tmp4;_Tmp4=_Tmp3->f2;{void*at=_Tmp4;
struct _tuple13*ar_tup=Cyc_Cifc_update_tvars(te,tv_ovrs,at,enclosing_decl,instantiate);
if((unsigned)ar_tup){
if((unsigned)(*ar_tup).f0)
added_tvars=Cyc_List_append((*ar_tup).f0,added_tvars);
if((unsigned)(*ar_tup).f1)
removed_tvars=Cyc_List_append((*ar_tup).f1,removed_tvars);}
# 672
argit=argit->tl;}}
# 674
if((unsigned)added_tvars ||(unsigned)removed_tvars)
return({struct _tuple13*_Tmp3=_cycalloc(sizeof(struct _tuple13));_Tmp3->f0=added_tvars,_Tmp3->f1=removed_tvars;_Tmp3;});
return 0;}}case 4: _Tmp2=(struct Cyc_Absyn_ArrayInfo*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_ArrayInfo*ai=(struct Cyc_Absyn_ArrayInfo*)_Tmp2;
# 678
return Cyc_Cifc_update_tvars(te,tv_ovrs,ai->elt_type,enclosing_decl,instantiate);}default:
# 680
 return 0;};}
# 682
return 0;}
# 685
static int Cyc_Cifc_update_fninfo_usage(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tv_ovrs,struct Cyc_Absyn_FnInfo*fi,struct Cyc_Absyn_Decl*enclosing_decl){
struct _tuple13*ad=Cyc_Cifc_update_tvars(te,tv_ovrs,fi->ret_type,enclosing_decl,0);
# 688
struct Cyc_List_List*argit=fi->args;
struct Cyc_List_List*added_tvars=(unsigned)ad?(*ad).f0: 0;
struct Cyc_List_List*removed_tvars=(unsigned)ad?(*ad).f1: 0;
int change=0;
while((unsigned)argit){
struct _tuple8*_Tmp0=(struct _tuple8*)argit->hd;void*_Tmp1;_Tmp1=(void**)& _Tmp0->f2;{void**at=(void**)_Tmp1;
struct _tuple13*ad=Cyc_Cifc_update_tvars(te,tv_ovrs,*at,enclosing_decl,0);
if((unsigned)ad){
added_tvars=Cyc_List_append((*ad).f0,added_tvars);
removed_tvars=Cyc_List_append((*ad).f1,removed_tvars);}
# 699
argit=argit->tl;}}
# 701
while((unsigned)removed_tvars){
change=1;{
struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(fi->tvars,rtv);fi->tvars=_Tmp2;});;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 709
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_warn(loc,_tag_fat("Removed tvar %s not found",sizeof(char),26U),_tag_fat(_Tmp5,sizeof(void*),1));});
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL3:;}}}
# 712
removed_tvars=removed_tvars->tl;}}
# 714
if((unsigned)added_tvars){
change=1;
({struct Cyc_List_List*_Tmp0=Cyc_List_append(added_tvars,fi->tvars);fi->tvars=_Tmp0;});}
# 719
fi->effect=0;
return change;}
# 723
static struct Cyc_List_List*Cyc_Cifc_remove_cur(struct Cyc_List_List*head,struct Cyc_List_List*cur,struct Cyc_List_List*prev){
if((unsigned)prev)
prev->tl=_check_null(cur)->tl;else{
# 727
head=_check_null(cur)->tl;}
# 729
return head;}
# 732
static int Cyc_Cifc_decl_name_cmp(struct Cyc_Absyn_Decl*a,struct Cyc_Absyn_Decl*b){
struct _fat_ptr*na=Cyc_Absyn_decl_name(a);
struct _fat_ptr*nb=Cyc_Absyn_decl_name(b);
if(na==0 ^ nb==0)
return 1;
if((unsigned)na)
return Cyc_strcmp(*na,*_check_null(nb));
return 1;}
# 743
static void Cyc_Cifc_deconstruct_type(void*t){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Type is %s\n",sizeof(char),12U),_tag_fat(_Tmp1,sizeof(void*),1));});{
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)t)){case 0: _Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*tc=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 747
Cyc_fprintf(Cyc_stderr,_tag_fat("Got AppType ... <\n",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));
# 749
while((unsigned)ts){
Cyc_Cifc_deconstruct_type((void*)ts->hd);
ts=ts->tl;}
# 753
Cyc_fprintf(Cyc_stderr,_tag_fat(">\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}case 1:
# 756
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got Evar\n",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2: _Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
# 758
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kindbound2string(tv->kind));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("Got VarType -- kindbnd is %s\n",sizeof(char),30U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp3=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp3;
# 760
Cyc_fprintf(Cyc_stderr,_tag_fat("Got PointerType\n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
Cyc_Cifc_deconstruct_type(pi->elt_type);
goto _LL0;}case 4:
# 764
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got ArrayType\n",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 5: _Tmp3=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*fi=_Tmp3;
# 766
Cyc_fprintf(Cyc_stderr,_tag_fat("Got FnType\n",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*tvit=fi->tvars;
while((unsigned)tvit){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)tvit->hd));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("tvar(%s), ",sizeof(char),11U),_tag_fat(_Tmp5,sizeof(void*),1));});
tvit=tvit->tl;}{
# 772
struct Cyc_List_List*argit=fi->args;
while((unsigned)argit){
struct _tuple8*_Tmp4=(struct _tuple8*)argit->hd;void*_Tmp5;_Tmp5=_Tmp4->f2;{void*t=_Tmp5;
Cyc_fprintf(Cyc_stderr,_tag_fat("Arg::",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));
Cyc_Cifc_deconstruct_type(t);
argit=argit->tl;}}
# 779
goto _LL0;}}}case 6:
# 781
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got AnonAggrType\n",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 7: _Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct _tuple0*n=_Tmp3;struct Cyc_List_List*ts=_Tmp2;struct Cyc_Absyn_Typedefdecl*d=_Tmp1;void*topt=_Tmp0;
# 783
Cyc_fprintf(Cyc_stderr,_tag_fat("Got TypedefType\n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
while((unsigned)ts){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)ts->hd));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("Types(i) = %s\n",sizeof(char),15U),_tag_fat(_Tmp5,sizeof(void*),1));});
ts=ts->tl;}
# 788
if((unsigned)topt)
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(topt));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("topt = %s\n",sizeof(char),11U),_tag_fat(_Tmp5,sizeof(void*),1));});
# 791
goto _LL0;}case 8:
# 793
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got ValueofType\n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 9:
# 795
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got TypeDeclType\n",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));goto _LL0;default:
# 797
 Cyc_fprintf(Cyc_stderr,_tag_fat("Got TypeofType\n",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}
# 801
static struct _tuple13*Cyc_Cifc_add_tvars_to_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Decl*encloser,void*t,struct Cyc_List_List*tv_ovrs){
# 805
{void*_Tmp0;void*_Tmp1;int _Tmp2;enum Cyc_Absyn_AggrKind _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)t)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*tc=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 807
return Cyc_Cifc_update_tvars(te,tv_ovrs,t,encloser,0);}case 3: _Tmp5=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=_Tmp5;
# 809
return Cyc_Cifc_add_tvars_to_type(te,encloser,pi->elt_type,tv_ovrs);}case 4:
# 811
 goto _LL0;case 5: _Tmp5=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*fi=_Tmp5;
# 813
struct Cyc_List_List*old_tvars=Cyc_List_copy(fi->tvars);
int changed=Cyc_Cifc_update_fninfo_usage(encloser->loc,te,tv_ovrs,fi,encloser);
if(changed)
return({struct _tuple13*_Tmp6=_cycalloc(sizeof(struct _tuple13));_Tmp6->f0=fi->tvars,_Tmp6->f1=old_tvars;_Tmp6;});
goto _LL0;}case 6: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;_Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{enum Cyc_Absyn_AggrKind knd=_Tmp3;int is_tuple=_Tmp2;struct Cyc_List_List*flst=_Tmp5;
# 819
goto _LL0;}case 7: _Tmp5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct _tuple0*n=_Tmp5;struct Cyc_List_List*ts=_Tmp4;struct Cyc_Absyn_Typedefdecl*d=_Tmp1;void*topt=_Tmp0;
# 830 "cifc.cyc"
goto _LL0;}case 9:
# 832
 goto _LL0;default:
# 834
 return 0;}_LL0:;}
# 836
return 0;}
# 839
static int Cyc_Cifc_update_typedef_decl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Decl*encloser,struct Cyc_List_List*tv_ovrs){
# 841
int opt_inst_tvar_sav=Cyc_Cifc_opt_inst_tvar;
Cyc_Cifc_opt_inst_tvar=0;{
int changed=0;
{void*_Tmp0=encloser->r;void*_Tmp1;if(*((int*)_Tmp0)==8){_Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td_decl=_Tmp1;
# 846
struct _tuple13*new_old_tvars=Cyc_Cifc_add_tvars_to_type(te,encloser,_check_null(td_decl->defn),tv_ovrs);
if((unsigned)new_old_tvars){
void*_Tmp2;void*_Tmp3;if(new_old_tvars!=0){_Tmp3=new_old_tvars->f0;_Tmp2=new_old_tvars->f1;{struct Cyc_List_List*nt=_Tmp3;struct Cyc_List_List*ot=_Tmp2;
while((unsigned)ot){
({struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete_cmp;})(Cyc_Absyn_tvar_cmp,td_decl->tvs,(struct Cyc_Absyn_Tvar*)ot->hd);td_decl->tvs=_Tmp4;});
ot=ot->tl;}
# 853
({struct Cyc_List_List*_Tmp4=Cyc_List_append(td_decl->tvs,nt);td_decl->tvs=_Tmp4;});
changed=1;}}else{_throw_match();}}
# 856
goto _LL0;}}else{
# 858
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("expect only a typedef_d",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 860
Cyc_Cifc_opt_inst_tvar=opt_inst_tvar_sav;
return changed;}}
# 869
static struct Cyc_List_List*Cyc_Cifc_update_usages(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*in_tv_ovrs,struct Cyc_List_List*dsmm){
unsigned niter=0U;
struct Cyc_List_List*abs_decls=0;
int some_change=0;
struct Cyc_List_List*tv_ovrs=in_tv_ovrs;
struct Cyc_List_List*dsm=Cyc_List_copy(dsmm);
struct Cyc_List_List*changed_decls=0;
do{
changed_decls=0;{
struct Cyc_List_List*ds=dsm;
struct Cyc_List_List*prev=0;
some_change=0;
while((unsigned)ds){
int changed=0;
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
struct _fat_ptr*dname=Cyc_Absyn_decl_name(d);
# 887
{void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 889
{void*_Tmp2=vd->type;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp2)){case 5: _Tmp4=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_FnInfo*fi=_Tmp4;
# 891
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,fi,d);
# 897
goto _LLB;}case 0: _Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{void*tc=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 899
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,vd->type,d,1)!=0;
goto _LLB;}case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;{void*et=_Tmp4;
_Tmp4=et;goto _LL13;}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;_LL13: {void*et=_Tmp4;
# 903
changed=Cyc_Cifc_update_tvars(te,tv_ovrs,et,d,1)!=0;
goto _LLB;}default:
# 906
 goto _LLB;}_LLB:;}
# 908
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 910
changed=Cyc_Cifc_update_fninfo_usage(loc,te,tv_ovrs,& fd->i,d);
# 914
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 916
if((unsigned)ad->impl){
struct Cyc_List_List*fit=_check_null(ad->impl)->fields;
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*removed_tvars=0;
while((unsigned)fit){
struct Cyc_Absyn_Aggrfield*fld=(struct Cyc_Absyn_Aggrfield*)fit->hd;
struct _tuple13*tvtup=Cyc_Cifc_update_tvars(te,tv_ovrs,fld->type,d,0);
if((unsigned)tvtup){
changed=1;{
struct Cyc_List_List*ad=(*tvtup).f0;
if((unsigned)ad)
added_tvars=Cyc_List_append(ad,added_tvars);{
struct Cyc_List_List*rm=(*tvtup).f1;
if((unsigned)rm)
removed_tvars=Cyc_List_append(rm,removed_tvars);}}}
# 932
fit=fit->tl;}
# 934
while((unsigned)removed_tvars){
struct Cyc_Absyn_Tvar*rtv=(struct Cyc_Absyn_Tvar*)removed_tvars->hd;
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
({struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_delete;})(ad->tvs,rtv);ad->tvs=_Tmp4;});;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp4)->tag==Cyc_Core_Not_found){
# 941
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string(rtv));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_warn(loc,_tag_fat("Removed tvar %s not found",sizeof(char),26U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL16;}else{_Tmp5=_Tmp4;{void*exn=_Tmp5;_rethrow(exn);}}_LL16:;}}}
# 944
removed_tvars=removed_tvars->tl;}
# 946
if((unsigned)added_tvars){
({struct Cyc_List_List*_Tmp2=Cyc_List_append(added_tvars,ad->tvs);ad->tvs=_Tmp2;});{
struct Cyc_Absyn_Decl*absdecl=Cyc_Cifc_make_abstract_decl(d);
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
abs_decls=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_Absyn_Decl*))Cyc_List_delete_cmp;})(Cyc_Cifc_decl_name_cmp,abs_decls,absdecl);;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp4)->tag==Cyc_Core_Not_found)
# 953
goto _LL1B;else{_Tmp5=_Tmp4;{void*exn=_Tmp5;_rethrow(exn);}}_LL1B:;}}}
# 955
abs_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=absdecl,_Tmp2->tl=abs_decls;_Tmp2;});}}
# 957
if(changed)
changed_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=changed_decls;_Tmp2;});}else{
# 961
struct _tuple0*_Tmp2=ad->name;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct _fat_ptr*name=_Tmp3;
struct Cyc_Absyn_Decl*ovd=Cyc_Absyn_lookup_decl(tv_ovrs,name);
if((unsigned)ovd){
changed=1;{
void*_Tmp4=ovd->r;void*_Tmp5;if(*((int*)_Tmp4)==5){_Tmp5=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Aggrdecl*ovd_ad=_Tmp5;
# 967
struct Cyc_List_List*added_tvars=0;
struct Cyc_List_List*tvs=ovd_ad->tvs;
while((unsigned)tvs){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
struct Cyc_Absyn_Tvar*tvcpy;tvcpy=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),*tvcpy=*tv;{
struct _fat_ptr*tvn;tvn=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp6=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)++ Cyc_Cifc_glob_tvar_id;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(_tag_fat("`ovr_%d",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),1));});*tvn=_Tmp6;});
tvcpy->name=tvn;
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=tvcpy,_Tmp7->tl=ad->tvs;_Tmp7;});ad->tvs=_Tmp6;});
tvs=tvs->tl;}}
# 977
({struct Cyc_List_List*_Tmp6=Cyc_List_imp_rev(ad->tvs);ad->tvs=_Tmp6;});
goto _LL23;}}else{
# 980
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("ovd must be an aggr type",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));}_LL23:;}}}}
# 986
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 996 "cifc.cyc"
changed=Cyc_Cifc_update_typedef_decl(te,d,tv_ovrs);
goto _LL0;}default:
# 999
 goto _LL0;}_LL0:;}
# 1001
prev=ds;
ds=ds->tl;
if(changed)
some_change=1;}
# 1006
tv_ovrs=changed_decls;}}while(
some_change && ++ niter < 10U);
if(niter >= 10U){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_decllist2string(changed_decls));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Suspected mutually recursive structs involving the following decls \n %s",sizeof(char),72U),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Suspected mutually recursive structs -- abandoning cifc",sizeof(char),56U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1013
return abs_decls;}
# 1016
static void Cyc_Cifc_i_clear_vartype_ids(void*t){
void*_Tmp0;struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_ArrayInfo _Tmp2;struct Cyc_Absyn_PtrInfo _Tmp3;void*_Tmp4;switch(*((int*)t)){case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 1019
tv->identity=-1;
goto _LL0;}case 0: _Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_Tmp4;
# 1022
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 1026
goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo pi=_Tmp3;
# 1028
Cyc_Cifc_i_clear_vartype_ids(pi.elt_type);
goto _LL0;}case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp2;
# 1031
Cyc_Cifc_i_clear_vartype_ids(ai.elt_type);
goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp1;
# 1034
Cyc_Cifc_i_clear_vartype_ids(fi.ret_type);{
struct Cyc_List_List*argit=fi.args;
while((unsigned)argit){
struct _tuple8*_Tmp5=(struct _tuple8*)argit->hd;void*_Tmp6;_Tmp6=_Tmp5->f2;{void*at=_Tmp6;
Cyc_Cifc_i_clear_vartype_ids(at);
argit=argit->tl;}}
# 1041
goto _LL0;}}case 7: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct Cyc_List_List*ts=_Tmp4;void*to=_Tmp0;
# 1046
while((unsigned)ts){
Cyc_Cifc_i_clear_vartype_ids((void*)ts->hd);
ts=ts->tl;}
# 1050
goto _LL0;}default:
# 1055
 goto _LL0;}_LL0:;}
# 1059
static void Cyc_Cifc_clear_vartype_ids(struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1062
Cyc_Cifc_i_clear_vartype_ids(vd->type);
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 1065
Cyc_Cifc_i_clear_vartype_ids(fd->i.ret_type);{
struct Cyc_List_List*ai=fd->i.args;
while((unsigned)ai){
Cyc_Cifc_i_clear_vartype_ids((*((struct _tuple8*)ai->hd)).f2);}
# 1070
fd->i.tvars=0;
goto _LL0;}}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 1073
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 1075
goto _LL0;}default:
# 1078
 goto _LL0;}_LL0:;}
# 1082
static int Cyc_Cifc_eq(void*a,void*b){
return a==b;}
# 1086
static int Cyc_Cifc_decl_not_present(struct Cyc_List_List*l,struct Cyc_Absyn_Decl*a){
return !({(int(*)(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*),struct Cyc_Absyn_Decl*,struct Cyc_List_List*))Cyc_List_exists_c;})(({(int(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Decl*))Cyc_Cifc_eq;}),a,l);}
# 1091
void Cyc_Cifc_user_overrides(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List**ds,struct Cyc_List_List*ovrs){
struct Cyc_List_List*type_overrides=0;
struct Cyc_List_List*overriden_decls=0;
struct Cyc_List_List*abs_decls=0;
{struct Cyc_List_List*x=ovrs;for(0;x!=0;x=x->tl){
struct Cyc_Absyn_Decl*ud=(struct Cyc_Absyn_Decl*)x->hd;
struct _fat_ptr*un=Cyc_Absyn_decl_name(ud);
if(!((unsigned)un))Cyc_Warn_warn(ud->loc,_tag_fat("Overriding decl without a name\n",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));else{
# 1100
struct Cyc_Absyn_Decl*d=Cyc_Cifc_lookup_concrete_decl(*ds,un);
if(!((unsigned)d))({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)*un);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn(ud->loc,_tag_fat("%s is overridden but not defined",sizeof(char),33U),_tag_fat(_Tmp1,sizeof(void*),1));});else{
# 1103
overriden_decls=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=overriden_decls;_Tmp0;});{
int pre_tvars_d=Cyc_Cifc_contains_type_vars(d);
Cyc_Cifc_merge_sys_user_decl(loc,te,0,ud,d);
Cyc_Cifc_clear_vartype_ids(ud);
if(Cyc_Cifc_contains_type_vars(ud)&& !pre_tvars_d){
abs_decls=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp1=Cyc_Cifc_make_abstract_decl(ud);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=abs_decls;_Tmp0;});
type_overrides=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=d,_Tmp0->tl=type_overrides;_Tmp0;});}}}}}}{
# 1115
struct Cyc_List_List*unoverriden_decls=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Decl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_filter_c;})(Cyc_Cifc_decl_not_present,overriden_decls,*ds);
abs_decls=({struct Cyc_List_List*_Tmp0=abs_decls;Cyc_List_append(_Tmp0,
Cyc_Cifc_update_usages(loc,te,type_overrides,unoverriden_decls));});
if((unsigned)abs_decls)
({struct Cyc_List_List*_Tmp0=Cyc_List_append(abs_decls,*ds);*ds=_Tmp0;});}}
