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
{ unsigned total_bytes;
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
  unsigned int id;
#ifdef CYC_REGION_PROFILE
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

//migration to reaps
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
 struct Cyc_Core_Opt{void*v;};
# 118 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 94
extern struct Cyc_List_List*Cyc_List_map2(void*(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 205
extern struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
extern struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 160
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 176 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 182
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 293
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 313
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 519
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 864 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 867
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 871
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 878
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 884
void*Cyc_Absyn_compress(void*);
# 888
int Cyc_Absyn_type2bool(int,void*);
# 897
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 902
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 904
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 911
extern void*Cyc_Absyn_heap_rgn_type;
# 913
extern void*Cyc_Absyn_al_qual_type;
# 915
extern void*Cyc_Absyn_empty_effect;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 947
extern void*Cyc_Absyn_fat_bound_type;
# 949
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 951
void*Cyc_Absyn_bounds_one (void);
# 953
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 957
void*Cyc_Absyn_atb_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 976
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
# 978
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 980
void*Cyc_Absyn_aqualsof_type(void*);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1095
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1138
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1140
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1142
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 75 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 57 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
extern struct Cyc_Core_Opt Cyc_Kinds_boolko;
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
# 76 "kinds.h"
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_copy_kindbound(void*);
# 83
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 92
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 47 "flags.h"
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 50
extern int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*,struct _RegionHandle*,struct Cyc_List_List*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RReturn (void);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 131
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 44 "rgnorder.h"
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*,void*);
# 69 "tcutil.h"
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 91
void*Cyc_Tcutil_copy_type(void*);
# 94
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 98
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 101
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 110
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 114
int Cyc_Tcutil_alias_qual_subtype(void*,void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 182
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 187
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);struct _tuple14{int f1;void*f2;};
# 199
void*Cyc_Tcutil_normalize_effect(void*);
# 202
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 212
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 230
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 237
int Cyc_Tcutil_force_type2bool(int,void*);
# 65 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 68
int Cyc_Atts_fntype_att(void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 39 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==0)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 45
int Cyc_Tcutil_is_array_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==4)
return 1;else{
return 0;};}
# 51
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==6)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 57
int Cyc_Tcutil_is_pointer_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==3)
return 1;else{
return 0;};}
# 64
int Cyc_Tcutil_is_char_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2==Cyc_Absyn_Char_sz)
return 1;else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 71
int Cyc_Tcutil_is_any_int_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==1)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 78
int Cyc_Tcutil_is_any_float_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==2)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 85
int Cyc_Tcutil_is_integral_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;{void*c=_Tmp1;
# 88
switch(*((int*)c)){case 1:
 goto _LL9;case 5: _LL9:
 goto _LLB;case 20: _LLB:
 goto _LLD;case 21: _LLD:
 return 1;default:
 return 0;};}}else{
# 95
return 0;};}
# 98
int Cyc_Tcutil_is_signed_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Signed)
return 1;else{goto _LL5;}case 2:
 return 1;default: goto _LL5;}else{_LL5:
 return 0;};}
# 105
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 108
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 111
int Cyc_Tcutil_is_tag_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 117
int Cyc_Tcutil_is_function_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==5)
return 1;else{
return 0;};}
# 123
int Cyc_Tcutil_is_typedef_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==8)
return 1;else{
return 0;};}
# 129
int Cyc_Tcutil_is_boxed(void*t){
return(int)Cyc_Tcutil_type_kind(t)->kind==2;}
# 137
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;if(*((int*)_Tmp0)==1)
return Cyc_Unify_unify(_check_null(e->topt),Cyc_Absyn_sint_type);else{_Tmp1=_Tmp0;{void*t=_Tmp1;
return Cyc_Tcutil_is_integral_type(t);}};}
# 145
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==2)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}}
# 155
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;{void*ztl=_Tmp1;
# 158
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{
return 0;};}
# 164
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*nbl=_Tmp1;
# 167
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{
return 0;};}
# 173
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*b=_Tmp1;
# 176
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{
return 0;};}
# 183
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*elt_type=_Tmp2;void*b=_Tmp1;
# 186
if(!Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type))
return 0;
*elt_type_dest=elt_type;
return 1;}}else{
return 0;};}
# 196
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;{void*elt_type=_Tmp2;void*zt=_Tmp1;
# 199
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{
return 0;};}
# 208
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 210
void*_Tmp0=Cyc_Absyn_compress(t);struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;{void*elt=_Tmp4;void*bnds=_Tmp3;void*zt=_Tmp2;
# 212
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_Tmp5=Cyc_Absyn_compress(bnds);if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==15){
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA:
*is_fat=0;goto _LL7;}_LL7:;}
# 220
return 1;}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*elt=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp1;struct Cyc_Absyn_Exp*n=_Tmp3;void*zt=_Tmp2;
# 222
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_Tmp5=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,0);*ptr_type=_Tmp5;});
return 1;}default:
 return 0;};}
# 235
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 241
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*eopt=({void*_Tmp0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp0,b);});
if(eopt==0)return 0;{
struct _tuple13 _Tmp0=Cyc_Evexp_eval_const_uint_exp(eopt);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned i=_Tmp2;int known=_Tmp1;
return known && i==1U;}}}
# 249
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 252
void*_Tmp3;switch(*((int*)c)){case 0:
 goto _LLF;case 1: _LLF:
 goto _LL11;case 2: _LL11:
 goto _LL13;case 3: _LL13:
 goto _LL15;case 21: _LL15:
 goto _LL17;case 20: _LL17:
 return 1;case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.UnknownAggr.tag==1)
# 260
return 0;else{_Tmp3=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;
# 262
if(ad->impl==0)
return 0;{
struct _RegionHandle _Tmp4=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp4;_push_region(rgn);
{struct Cyc_List_List*inst=Cyc_List_rzip(rgn,rgn,ad->tvs,ts);
{struct Cyc_List_List*fs=_check_null(ad->impl)->fields;for(0;fs!=0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _Tmp5=0;_npop_handler(0);return _Tmp5;}}}{
int _Tmp5=1;_npop_handler(0);return _Tmp5;}}
# 265
;_pop_region();}}}default:
# 270
 return 0;};}case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*t=_Tmp2;void*zero_term=_Tmp1;
# 275
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqs=_Tmp2;
# 277
for(1;tqs!=0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp2;
# 282
for(1;fs!=0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}default:
 return 0;};}
# 291
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 295
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp0->name=tv->name,_Tmp0->identity=-1,({void*_Tmp1=Cyc_Kinds_copy_kindbound(tv->kind);_Tmp0->kind=_Tmp1;});_Tmp0;});}
# 298
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;_Tmp2=arg->f1;_Tmp1=arg->f2;_Tmp0=arg->f3;{struct _fat_ptr*x=_Tmp2;struct Cyc_Absyn_Tqual y=_Tmp1;void*t=_Tmp0;
return({struct _tuple9*_Tmp3=_cycalloc(sizeof(struct _tuple9));_Tmp3->f1=x,_Tmp3->f2=y,({void*_Tmp4=Cyc_Tcutil_copy_type(t);_Tmp3->f3=_Tmp4;});_Tmp3;});}}
# 302
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_Tmp0=_cycalloc(sizeof(struct _tuple12));_Tmp0->f1=(*arg).f1,({void*_Tmp1=Cyc_Tcutil_copy_type((*arg).f2);_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 305
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e==0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 308
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp0->name=f->name,_Tmp0->tq=f->tq,({void*_Tmp1=Cyc_Tcutil_copy_type(f->type);_Tmp0->type=_Tmp1;}),_Tmp0->width=f->width,_Tmp0->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_Tmp1=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_Tmp0->requires_clause=_Tmp1;});_Tmp0;});}struct _tuple15{void*f1;void*f2;};
# 312
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
void*_Tmp0;void*_Tmp1;_Tmp1=x->f1;_Tmp0=x->f2;{void*r1=_Tmp1;void*r2=_Tmp0;
return({struct _tuple15*_Tmp2=_cycalloc(sizeof(struct _tuple15));({void*_Tmp3=Cyc_Tcutil_copy_type(r1);_Tmp2->f1=_Tmp3;}),({void*_Tmp3=Cyc_Tcutil_copy_type(r2);_Tmp2->f2=_Tmp3;});_Tmp2;});}}
# 316
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 320
void*Cyc_Tcutil_copy_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;int _Tmp8;unsigned _Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;void*_Tmp11;void*_Tmp12;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2==0){_Tmp12=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;{void*c=_Tmp12;
return t;}}else{_Tmp12=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp11=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp12;struct Cyc_List_List*ts=_Tmp11;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp13->tag=0,_Tmp13->f1=c,({struct Cyc_List_List*_Tmp14=Cyc_List_map(Cyc_Tcutil_copy_type,ts);_Tmp13->f2=_Tmp14;});_Tmp13;});}}case 1:
 return t;case 2: _Tmp12=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp12;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3: _Tmp12=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.ptrloc;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.autoreleased;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.aqual;{void*elt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp10;void*rgn=_Tmp11;void*nbl=_TmpF;void*bs=_TmpE;void*zt=_TmpD;struct Cyc_Absyn_PtrLoc*loc=_TmpC;void*rel=_TmpB;void*aq=_TmpA;
# 327
void*elt2=Cyc_Tcutil_copy_type(elt);
void*rgn2=Cyc_Tcutil_copy_type(rgn);
void*nbl2=Cyc_Tcutil_copy_type(nbl);
struct Cyc_Absyn_Tqual tq2=tq;
# 332
void*bs2=Cyc_Tcutil_copy_type(bs);
void*zt2=Cyc_Tcutil_copy_type(zt);
void*rel2=Cyc_Tcutil_copy_type(rel);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_Tmp13->tag=3,_Tmp13->f1.elt_type=elt2,_Tmp13->f1.elt_tq=tq2,_Tmp13->f1.ptr_atts.rgn=rgn2,_Tmp13->f1.ptr_atts.nullable=nbl2,_Tmp13->f1.ptr_atts.bounds=bs2,_Tmp13->f1.ptr_atts.zero_term=zt2,_Tmp13->f1.ptr_atts.ptrloc=loc,_Tmp13->f1.ptr_atts.autoreleased=rel2,_Tmp13->f1.ptr_atts.aqual=aq;_Tmp13;});}case 4: _Tmp12=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp10=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;_Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;_Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zt_loc;{void*et=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp10;struct Cyc_Absyn_Exp*eo=_Tmp11;void*zt=_TmpF;unsigned ztl=_Tmp9;
# 337
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp13->tag=4,({void*_Tmp14=Cyc_Tcutil_copy_type(et);_Tmp13->f1.elt_type=_Tmp14;}),_Tmp13->f1.tq=tq,({struct Cyc_Absyn_Exp*_Tmp14=Cyc_Tcutil_deep_copy_exp_opt(1,eo);_Tmp13->f1.num_elts=_Tmp14;}),({
void*_Tmp14=Cyc_Tcutil_copy_type(zt);_Tmp13->f1.zero_term=_Tmp14;}),_Tmp13->f1.zt_loc=ztl;_Tmp13;});}case 5: _Tmp12=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;_Tmp11=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.effect;_Tmp10=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_tqual;_TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_type;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.args;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.c_varargs;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.cyc_varargs;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.rgn_po;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.qual_bnd;_TmpA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.requires_clause;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.requires_relns;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ensures_clause;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ensures_relns;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.return_value;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.arg_vardecls;{struct Cyc_List_List*tvs=_Tmp12;void*effopt=_Tmp11;struct Cyc_Absyn_Tqual rt_tq=_Tmp10;void*rt=_TmpF;struct Cyc_List_List*args=_TmpE;int c_varargs=_Tmp8;struct Cyc_Absyn_VarargInfo*cyc_varargs=_TmpD;struct Cyc_List_List*rgn_po=_TmpC;struct Cyc_List_List*qb=_TmpB;struct Cyc_List_List*atts=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp7;struct Cyc_List_List*req_rlns=_Tmp6;struct Cyc_Absyn_Exp*ens=_Tmp5;struct Cyc_List_List*ens_rlns=_Tmp4;struct Cyc_Absyn_Vardecl*ret_val=_Tmp3;struct Cyc_List_List*argvds=_Tmp2;
# 340
struct Cyc_List_List*tvs2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_tvar,tvs);
void*effopt2=effopt==0?0: Cyc_Tcutil_copy_type(effopt);
void*rt2=Cyc_Tcutil_copy_type(rt);
struct Cyc_List_List*args2=({(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_arg,args);
int c_varargs2=c_varargs;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs==0?0:({struct Cyc_Absyn_VarargInfo*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));*_Tmp13=*cyc_varargs;_Tmp13;});
if(cyc_varargs!=0)
({void*_Tmp13=Cyc_Tcutil_copy_type(cyc_varargs->type);_check_null(cyc_varargs2)->type=_Tmp13;});{
struct Cyc_List_List*rgn_po2=({(struct Cyc_List_List*(*)(struct _tuple15*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_rgncmp,rgn_po);
struct Cyc_List_List*qb2=({(struct Cyc_List_List*(*)(struct _tuple15*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_rgncmp,qb);
struct Cyc_List_List*atts2=atts;
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_deep_copy_exp_opt(1,req);
struct Cyc_List_List*req_rlns2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_deep_copy_exp_opt(1,ens);
struct Cyc_List_List*ens_rlns2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);
# 359
struct Cyc_Absyn_Vardecl*ret_val2=ret_val;
struct Cyc_List_List*argvds2=argvds;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp13->tag=5,_Tmp13->f1.tvars=tvs2,_Tmp13->f1.effect=effopt2,_Tmp13->f1.ret_tqual=rt_tq,_Tmp13->f1.ret_type=rt2,_Tmp13->f1.args=args2,_Tmp13->f1.c_varargs=c_varargs2,_Tmp13->f1.cyc_varargs=cyc_varargs2,_Tmp13->f1.rgn_po=rgn_po2,_Tmp13->f1.qual_bnd=qb2,_Tmp13->f1.attributes=atts2,_Tmp13->f1.requires_clause=req2,_Tmp13->f1.requires_relns=req_rlns2,_Tmp13->f1.ensures_clause=ens2,_Tmp13->f1.ensures_relns=ens_rlns2,_Tmp13->f1.return_value=ret_val2,_Tmp13->f1.arg_vardecls=argvds2;_Tmp13;});}}case 6: _Tmp12=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqts=_Tmp12;
# 364
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp13->tag=6,({struct Cyc_List_List*_Tmp14=({(struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_tqt,tqts);_Tmp13->f1=_Tmp14;});_Tmp13;});}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp12=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp12;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp13->tag=7,_Tmp13->f1=k,({struct Cyc_List_List*_Tmp14=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_copy_field,fs);_Tmp13->f2=_Tmp14;});_Tmp13;});}case 9: _Tmp12=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp12;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp13->tag=9,_Tmp13->f1=e;_Tmp13;});}case 11: _Tmp12=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp12;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_TypeofType_Absyn_Type_struct));_Tmp13->tag=11,_Tmp13->f1=e;_Tmp13;});}case 8: _Tmp12=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp11=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_TmpF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;{struct _tuple1*tdn=_Tmp12;struct Cyc_List_List*ts=_Tmp11;struct Cyc_Absyn_Typedefdecl*td=_TmpF;
# 369
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_Tmp13->tag=8,_Tmp13->f1=tdn,({struct Cyc_List_List*_Tmp14=Cyc_List_map(Cyc_Tcutil_copy_type,ts);_Tmp13->f2=_Tmp14;}),_Tmp13->f3=td,_Tmp13->f4=0;_Tmp13;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 0: _Tmp12=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp12;
# 372
struct Cyc_List_List*targs=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_tvar2type,ad->tvs);
return({union Cyc_Absyn_AggrInfo _Tmp13=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_Tmp13,targs);});}case 1: _Tmp12=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp12;
# 375
return Cyc_Absyn_enum_type(ed->name,0);}default: _Tmp12=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp12;
# 377
struct Cyc_List_List*targs=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_tvar2type,dd->tvs);
return({union Cyc_Absyn_DatatypeInfo _Tmp13=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp14;_Tmp14.name=dd->name,_Tmp14.is_extensible=0;_Tmp14;}));Cyc_Absyn_datatype_type(_Tmp13,targs);});}}};}
# 383
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_Tmp0;if(*((int*)d)==0){_Tmp0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)d)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_Tmp1->f1=_Tmp2;});_Tmp1;});}}else{
return d;};}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 389
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 391
return({struct _tuple16*_Tmp0=_cycalloc(sizeof(struct _tuple16));({struct Cyc_List_List*_Tmp1=({(struct Cyc_List_List*(*)(void*(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_designator,preserve_types,e[0].f1);_Tmp0->f1=_Tmp1;}),({
struct Cyc_Absyn_Exp*_Tmp1=Cyc_Tcutil_deep_copy_exp(preserve_types,e[0].f2);_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 395
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*DC)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_deep_copy_exp;
struct Cyc_Absyn_Exp*new_e;
int pt=preserve_types;
{void*_Tmp0=e->r;enum Cyc_Absyn_MallocKind _Tmp1;enum Cyc_Absyn_Coercion _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;enum Cyc_Absyn_Incrementor _TmpB;enum Cyc_Absyn_Primop _TmpC;struct _fat_ptr _TmpD;void*_TmpE;union Cyc_Absyn_Cnst _TmpF;switch(*((int*)_Tmp0)){case 0: _TmpF=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_TmpF;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1: _TmpE=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_TmpE;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2: _TmpD=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_TmpD;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3: _TmpC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_TmpC;struct Cyc_List_List*es=_TmpE;
new_e=({enum Cyc_Absyn_Primop _Tmp10=p;struct Cyc_List_List*_Tmp11=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(DC,pt,es);Cyc_Absyn_primop_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 5: _TmpE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;enum Cyc_Absyn_Incrementor i=_TmpB;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);enum Cyc_Absyn_Incrementor _Tmp11=i;Cyc_Absyn_increment_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 7: _TmpE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);Cyc_Absyn_and_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 8: _TmpE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);Cyc_Absyn_or_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 9: _TmpE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);Cyc_Absyn_seq_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 6: _TmpE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;struct Cyc_Absyn_Exp*e3=_Tmp9;
# 409
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e3);Cyc_Absyn_conditional_exp(_Tmp10,_Tmp11,_Tmp12,e->loc);});goto _LL0;}case 4: _TmpE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Core_Opt*po=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;
# 411
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Core_Opt*_Tmp11=(unsigned)po?({struct Cyc_Core_Opt*_Tmp12=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp12->v=(void*)po->v;_Tmp12;}): 0;struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e2);Cyc_Absyn_assignop_exp(_Tmp10,_Tmp11,_Tmp12,e->loc);});
goto _LL0;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3!=0){_TmpE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->num_varargs;_Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->injectors;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->vai->name;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->vai->tq;_Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->vai->type;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->vai->inject;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_List_List*es=_TmpA;int n=_Tmp8;struct Cyc_List_List*is=_Tmp9;struct _fat_ptr*nm=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*t=_Tmp5;int i=_Tmp4;int resolved=_Tmp3;
# 414
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp11->tag=10,({
struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f1=_Tmp12;}),({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(DC,pt,es);_Tmp11->f2=_Tmp12;}),({
struct Cyc_Absyn_VarargCallInfo*_Tmp12=({struct Cyc_Absyn_VarargCallInfo*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_VarargCallInfo));_Tmp13->num_varargs=n,_Tmp13->injectors=is,({
struct Cyc_Absyn_VarargInfo*_Tmp14=({struct Cyc_Absyn_VarargInfo*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_Tmp15->name=nm,_Tmp15->tq=tq,({void*_Tmp16=Cyc_Tcutil_copy_type(t);_Tmp15->type=_Tmp16;}),_Tmp15->inject=i;_Tmp15;});_Tmp13->vai=_Tmp14;});_Tmp13;});
# 416
_Tmp11->f3=_Tmp12;}),_Tmp11->f4=resolved;_Tmp11;});
# 414
Cyc_Absyn_new_exp(_Tmp10,e->loc);});
# 419
goto _LL0;}}else{_TmpE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_VarargCallInfo*vci=_Tmp9;int resolved=_Tmp8;
# 421
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp11->tag=10,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f1=_Tmp12;}),({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(DC,pt,es);_Tmp11->f2=_Tmp12;}),_Tmp11->f3=vci,_Tmp11->f4=resolved;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});
goto _LL0;}}case 11: _TmpE=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;int b=_Tmp8;
# 424
new_e=b?({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_rethrow_exp(_Tmp10,e->loc);}):({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_throw_exp(_Tmp10,e->loc);});
goto _LL0;}case 12: _TmpE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
# 427
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_Tmp10,e->loc);});goto _LL0;}case 13: _TmpE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_List_List*ts=_TmpA;
# 429
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_List_List*_Tmp11=Cyc_List_map(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 14: _TmpE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t=_TmpE;struct Cyc_Absyn_Exp*e1=_TmpA;int b=_Tmp8;enum Cyc_Absyn_Coercion c=_Tmp2;
# 431
new_e=({void*_Tmp10=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e1);int _Tmp12=b;enum Cyc_Absyn_Coercion _Tmp13=c;Cyc_Absyn_cast_exp(_Tmp10,_Tmp11,_Tmp12,_Tmp13,e->loc);});goto _LL0;}case 16: _TmpE=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eo=_TmpE;struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*qo=_Tmp9;
# 433
new_e=({struct Cyc_Absyn_Exp*_Tmp10=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp12=(unsigned)qo?DC(pt,qo): 0;Cyc_Absyn_New_exp(_Tmp10,_Tmp11,_Tmp12,e->loc);});goto _LL0;}case 15: _TmpE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_address_exp(_Tmp10,e->loc);});goto _LL0;}case 17: _TmpE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpE;
new_e=({void*_Tmp10=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_Tmp10,e->loc);});goto _LL0;}case 18: _TmpE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_Tmp10,e->loc);});goto _LL0;}case 19: _TmpE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpE;struct Cyc_List_List*ofs=_TmpA;
new_e=({void*_Tmp10=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_Tmp11=ofs;Cyc_Absyn_offsetof_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 20: _TmpE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_deref_exp(_Tmp10,e->loc);});goto _LL0;}case 41: _TmpE=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);Cyc_Absyn_extension_exp(_Tmp10,e->loc);});goto _LL0;}case 24: _TmpE=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_TmpE;
new_e=({struct Cyc_List_List*_Tmp10=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(DC,pt,es);Cyc_Absyn_tuple_exp(_Tmp10,e->loc);});goto _LL0;}case 21: _TmpE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpE;struct _fat_ptr*n=_TmpA;int f1=_Tmp8;int f2=_Tmp4;
# 442
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_Tmp11->tag=21,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f1=_Tmp12;}),_Tmp11->f2=n,_Tmp11->f3=f1,_Tmp11->f4=f2;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 22: _TmpE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpE;struct _fat_ptr*n=_TmpA;int f1=_Tmp8;int f2=_Tmp4;
# 444
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct));_Tmp11->tag=22,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f1=_Tmp12;}),_Tmp11->f2=n,_Tmp11->f3=f1,_Tmp11->f4=f2;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 23: _TmpE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;
# 446
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);Cyc_Absyn_subscript_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 26: _TmpE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*eds=_TmpE;
# 448
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp11->tag=26,({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_eds,pt,eds);_Tmp11->f1=_Tmp12;});_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 31: _TmpE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_TmpE;struct Cyc_Absyn_Datatypedecl*dtd=_TmpA;struct Cyc_Absyn_Datatypefield*dtf=_Tmp9;
# 450
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_Tmp11->tag=31,({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(DC,pt,es);_Tmp11->f1=_Tmp12;}),_Tmp11->f2=dtd,_Tmp11->f3=dtf;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 27: _TmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_TmpE;struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;int b=_Tmp8;
# 452
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp11->tag=27,_Tmp11->f1=vd,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f2=_Tmp12;}),({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e2);_Tmp11->f3=_Tmp12;}),_Tmp11->f4=b;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 28: _TmpE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_TmpE;void*t=_TmpA;int b=_Tmp8;
# 454
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp11->tag=28,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e);_Tmp11->f1=_Tmp12;}),({void*_Tmp12=Cyc_Tcutil_copy_type(t);_Tmp11->f2=_Tmp12;}),_Tmp11->f3=b;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});
goto _LL0;}case 29: _TmpE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple1*n=_TmpE;struct Cyc_List_List*ts=_TmpA;struct Cyc_List_List*eds=_Tmp9;struct Cyc_Absyn_Aggrdecl*agr=_Tmp7;
# 457
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp11->tag=29,_Tmp11->f1=n,({struct Cyc_List_List*_Tmp12=Cyc_List_map(Cyc_Tcutil_copy_type,ts);_Tmp11->f2=_Tmp12;}),({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_eds,pt,eds);_Tmp11->f3=_Tmp12;}),_Tmp11->f4=agr;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});
# 459
goto _LL0;}case 30: _TmpE=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpE;struct Cyc_List_List*eds=_TmpA;
# 461
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp11->tag=30,({void*_Tmp12=Cyc_Tcutil_copy_type(t);_Tmp11->f1=_Tmp12;}),({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_eds,pt,eds);_Tmp11->f2=_Tmp12;});_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});
goto _LL0;}case 25: _TmpE=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1->f1;_Tmp6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1->f2;_TmpA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1->f3;_Tmp9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _fat_ptr*vopt=_TmpE;struct Cyc_Absyn_Tqual tq=_Tmp6;void*t=_TmpA;struct Cyc_List_List*eds=_Tmp9;
# 464
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp11->tag=25,({struct _tuple9*_Tmp12=({struct _tuple9*_Tmp13=_cycalloc(sizeof(struct _tuple9));_Tmp13->f1=vopt,_Tmp13->f2=tq,({void*_Tmp14=Cyc_Tcutil_copy_type(t);_Tmp13->f3=_Tmp14;});_Tmp13;});_Tmp11->f1=_Tmp12;}),({
struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_eds,pt,eds);_Tmp11->f2=_Tmp12;});_Tmp11;});
# 464
Cyc_Absyn_new_exp(_Tmp10,e->loc);});
# 466
goto _LL0;}case 32: _TmpE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_TmpE;struct Cyc_Absyn_Enumfield*ef=_TmpA;
new_e=e;goto _LL0;}case 33: _TmpE=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpE;struct Cyc_Absyn_Enumfield*ef=_TmpA;
# 469
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp11->tag=33,({void*_Tmp12=Cyc_Tcutil_copy_type(t);_Tmp11->f1=_Tmp12;}),_Tmp11->f2=ef;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 34: _Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_TmpE=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_TmpA=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp9=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp8=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.inline_call;{enum Cyc_Absyn_MallocKind mk=_Tmp1;struct Cyc_Absyn_Exp*r=_TmpE;struct Cyc_Absyn_Exp*q=_TmpA;void**t=_Tmp9;struct Cyc_Absyn_Exp*n=_Tmp7;int res=_Tmp8;int inlc=_Tmp4;
# 471
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r!=0)r1=DC(pt,r);{
struct Cyc_Absyn_Exp*q1=q;if(q!=0)q1=DC(pt,q);{
void**t1=t;if(t!=0)t1=({void**_Tmp10=_cycalloc(sizeof(void*));({void*_Tmp11=Cyc_Tcutil_copy_type(*t);*_Tmp10=_Tmp11;});_Tmp10;});
({void*_Tmp10=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp11->tag=34,_Tmp11->f1.mknd=mk,_Tmp11->f1.rgn=r1,_Tmp11->f1.aqual=q1,_Tmp11->f1.elt_type=t1,_Tmp11->f1.num_elts=n,_Tmp11->f1.fat_result=res,_Tmp11->f1.inline_call=inlc;_Tmp11;});e2->r=_Tmp10;});
new_e=e2;
goto _LL0;}}}case 35: _TmpE=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct Cyc_Absyn_Exp*e2=_TmpA;
new_e=({struct Cyc_Absyn_Exp*_Tmp10=DC(pt,e1);struct Cyc_Absyn_Exp*_Tmp11=DC(pt,e2);Cyc_Absyn_swap_exp(_Tmp10,_Tmp11,e->loc);});goto _LL0;}case 36: _TmpE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*nopt=_TmpE;struct Cyc_List_List*eds=_TmpA;
# 480
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt!=0)nopt1=({struct Cyc_Core_Opt*_Tmp10=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp10->v=(struct _tuple1*)nopt->v;_Tmp10;});
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp11->tag=36,_Tmp11->f1=nopt1,({struct Cyc_List_List*_Tmp12=({(struct Cyc_List_List*(*)(struct _tuple16*(*)(int,struct _tuple16*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_copy_eds,pt,eds);_Tmp11->f2=_Tmp12;});_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});
goto _LL0;}case 37:
# 485
 _throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp10->tag=Cyc_Core_Failure,_Tmp10->f1=_tag_fat("deep_copy: statement expressions unsupported",sizeof(char),45U);_Tmp10;}));case 38: _TmpE=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpE;struct _fat_ptr*fn=_TmpA;
# 487
new_e=({void*_Tmp10=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp11->tag=38,({struct Cyc_Absyn_Exp*_Tmp12=DC(pt,e1);_Tmp11->f1=_Tmp12;}),_Tmp11->f2=fn;_Tmp11;});Cyc_Absyn_new_exp(_Tmp10,e->loc);});goto _LL0;}case 39: _TmpE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpE;
new_e=({void*_Tmp10=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_Tmp10,e->loc);});goto _LL0;}case 40: _Tmp8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_TmpE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_TmpA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int v=_Tmp8;struct _fat_ptr t=_TmpD;struct Cyc_List_List*o=_TmpE;struct Cyc_List_List*i=_TmpA;struct Cyc_List_List*c=_Tmp9;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _TmpE=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpE;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 493
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 497
return new_e;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 502
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 504
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts!=0 && k==& Cyc_Kinds_ak){
# 508
struct _RegionHandle _Tmp0=_new_region("r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{struct Cyc_List_List*inst=0;
# 511
for(1;tvs!=0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*t=(void*)ts->hd;
enum Cyc_Absyn_KindQual _Tmp1=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk)->kind;switch((int)_Tmp1){case Cyc_Absyn_IntKind:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL4:
 inst=({struct Cyc_List_List*_Tmp2=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp3=({struct _tuple17*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple17));_Tmp4->f1=tv,_Tmp4->f2=t;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=inst;_Tmp2;});goto _LL0;default:
 goto _LL0;}_LL0:;}
# 520
if(inst!=0){
field_type=({struct _RegionHandle*_Tmp1=r;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(inst);Cyc_Tcutil_rsubstitute(_Tmp1,_Tmp2,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 509
;_pop_region();}
# 525
return k;}
# 532
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 534
void*_Tmp0=Cyc_Absyn_compress(t);struct Cyc_Absyn_PtrInfo _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt*k=_Tmp3;
return(struct Cyc_Absyn_Kind*)_check_null(k)->v;}case 2: _Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
return Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);}case 0: _Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 538
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;enum Cyc_Absyn_Size_of _Tmp8;switch(*((int*)c)){case 0:
 return& Cyc_Kinds_mk;case 1: _Tmp8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)c)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp8;
# 541
return((int)sz==2 ||(int)sz==3)?& Cyc_Kinds_bk:& Cyc_Kinds_mk;}case 3:
 goto _LL25;case 2: _LL25:
 return& Cyc_Kinds_mk;case 20:
 goto _LL29;case 21: _LL29:
 goto _LL2B;case 4: _LL2B:
 return& Cyc_Kinds_bk;case 19:
 return& Cyc_Kinds_bk;case 7:
 goto _LL31;case 8: _LL31:
 goto _LL33;case 6: _LL33:
 return& Cyc_Kinds_rk;case 22: _Tmp7=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)c)->f2;{struct Cyc_Absyn_Kind*k=_Tmp7;
return k;}case 5:
 return& Cyc_Kinds_bk;case 9:
 goto _LL3B;case 10: _LL3B:
 goto _LL3D;case 11: _LL3D:
 return& Cyc_Kinds_ek;case 13:
 goto _LL41;case 12: _LL41:
 return& Cyc_Kinds_boolk;case 14:
 goto _LL45;case 15: _LL45:
 return& Cyc_Kinds_ptrbk;case 18:
 goto _LL49;case 17: _LL49:
 goto _LL4B;case 16: _LL4B:
 return& Cyc_Kinds_aqk;case 23:
 return& Cyc_Kinds_ak;case 24: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1.KnownDatatypefield.tag==2)
return& Cyc_Kinds_mk;else{
# 566
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("type_kind: Unresolved DatatypeFieldType",sizeof(char),40U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});}default: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.UnknownAggr.tag==1)
# 569
return& Cyc_Kinds_ak;else{_Tmp6=(*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val)->kind;_Tmp7=(*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val)->tvs;_Tmp5=(*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val)->impl;_Tmp4=(*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1.KnownAggr.val)->expected_mem_kind;{enum Cyc_Absyn_AggrKind strOrU=_Tmp6;struct Cyc_List_List*tvs=_Tmp7;struct Cyc_Absyn_AggrdeclImpl*i=_Tmp5;int expected_mem_kind=_Tmp4;
# 571
if(i==0)
return expected_mem_kind?& Cyc_Kinds_mk:& Cyc_Kinds_ak;{
struct Cyc_List_List*fields=i->fields;
if(fields==0)return& Cyc_Kinds_mk;
# 576
if((int)strOrU==0){
for(1;_check_null(fields)->tl!=0;fields=fields->tl){
;}{
void*last_type=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_field_kind(last_type,ts,tvs);
if(k==& Cyc_Kinds_ak)return k;}}else{
# 585
for(1;fields!=0;fields=fields->tl){
void*type=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_field_kind(type,ts,tvs);
if(k==& Cyc_Kinds_ak)return k;}}
# 590
return& Cyc_Kinds_mk;}}}};}case 5:
# 592
 return& Cyc_Kinds_ak;case 7:
 return& Cyc_Kinds_mk;case 3: _Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo pinfo=_Tmp1;
# 595
void*_Tmp4=Cyc_Absyn_compress(pinfo.ptr_atts.bounds);if(*((int*)_Tmp4)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)){case 14:
# 597
 return& Cyc_Kinds_bk;case 15:
# 599
 goto _LL5C;default: goto _LL5B;}else{_LL5B: _LL5C:
# 601
 return& Cyc_Kinds_mk;};}case 9:
# 603
 return& Cyc_Kinds_ik;case 11:
# 607
 return& Cyc_Kinds_ak;case 4: _Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*num_elts=_Tmp3;
# 609
if(num_elts==0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Kinds_mk;
return& Cyc_Kinds_ak;}case 6:
 return& Cyc_Kinds_mk;case 8: _Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Typedefdecl*td=_Tmp3;
# 613
if(td==0 || td->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type_kind: typedef found: ",sizeof(char),27U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),2));});
return(struct Cyc_Absyn_Kind*)_check_null(td->kind)->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 1:
 return& Cyc_Kinds_bk;case 0:
 goto _LL1C;default: _LL1C:
 return& Cyc_Kinds_ak;}};}
# 622
static void*Cyc_Tcutil_rgns_of(void*);
# 624
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 628
static struct _tuple17*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_Tmp0=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);switch((int)((struct Cyc_Absyn_Kind*)_Tmp0)->kind){case Cyc_Absyn_RgnKind:
# 632
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;case Cyc_Absyn_EffKind:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp1->tag=9,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_uint_exp(0U,0U);_Tmp1->f1=_Tmp2;});_Tmp1;});goto _LL0;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 639
return({struct _tuple17*_Tmp0=_cycalloc(sizeof(struct _tuple17));_Tmp0->f1=tv,_Tmp0->f2=t;_Tmp0;});}
# 646
static void*Cyc_Tcutil_rgns_of(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;void*_Tmp7;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2==0)
return Cyc_Absyn_empty_effect;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==10)
return t;else{_Tmp7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp7;
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(Cyc_List_map(Cyc_Tcutil_rgns_of,ts)));}}}case 1:
 goto _LLA;case 2: _LLA: {
# 653
struct Cyc_Absyn_Kind*_Tmp8=Cyc_Tcutil_type_kind(t);switch((int)((struct Cyc_Absyn_Kind*)_Tmp8)->kind){case Cyc_Absyn_RgnKind:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind:
 return t;case Cyc_Absyn_IntKind:
 return Cyc_Absyn_empty_effect;default:
 return Cyc_Absyn_regionsof_eff(t);};}case 3: _Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;{void*et=_Tmp7;void*r=_Tmp6;
# 661
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_Tmp8[2];({void*_Tmp9=Cyc_Absyn_access_eff(r);_Tmp8[0]=_Tmp9;}),({void*_Tmp9=Cyc_Tcutil_rgns_of(et);_Tmp8[1]=_Tmp9;});Cyc_List_list(_tag_fat(_Tmp8,sizeof(void*),2));})));}case 4: _Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*et=_Tmp7;
# 663
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7: _Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp7;
# 665
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_rgns_of_field,fs)));}case 5: _Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.effect;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_tqual;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_type;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.args;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.cyc_varargs;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.rgn_po;{struct Cyc_List_List*tvs=_Tmp7;void*eff=_Tmp6;struct Cyc_Absyn_Tqual rt_tq=_Tmp5;void*rt=_Tmp4;struct Cyc_List_List*args=_Tmp3;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp2;struct Cyc_List_List*rpo=_Tmp1;
# 674
void*e=({struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(struct _tuple17*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_Tmp8,_check_null(eff));});
return Cyc_Tcutil_normalize_effect(e);}case 6: _Tmp7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqts=_Tmp7;
# 677
struct Cyc_List_List*ts=0;
for(1;tqts!=0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=(*((struct _tuple12*)tqts->hd)).f2,_Tmp8->tl=ts;_Tmp8;});}
_Tmp7=ts;goto _LL16;}case 8: _Tmp7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_LL16: {struct Cyc_List_List*ts=_Tmp7;
# 682
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(Cyc_List_map(Cyc_Tcutil_rgns_of,ts)));}case 10:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("typedecl in rgns_of",sizeof(char),20U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp9,sizeof(void*),1));});case 9:
 goto _LL1C;default: _LL1C:
 return Cyc_Absyn_empty_effect;};}
# 692
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_Tmp0;if(*((int*)e)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f1)){case 10: _Tmp0=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2;{struct Cyc_List_List**es=_Tmp0;
# 696
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs!=0;effs=effs->tl){
void*eff=(void*)effs->hd;
({void*_Tmp1=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_Tmp1;});{
void*_Tmp1=(void*)effs->hd;if(*((int*)_Tmp1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)){case 10:
 goto _LLB;case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->hd)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->hd)->f1)==6){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->tl==0){_LLB:
# 705
 redo_join=1;goto _LL7;}else{goto _LLC;}}else{goto _LLC;}}else{goto _LLC;}}else{goto _LLC;}default: goto _LLC;}else{_LLC:
 goto _LL7;}_LL7:;}}}
# 709
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs!=0;effs=effs->tl){
void*_Tmp1=Cyc_Absyn_compress((void*)effs->hd);void*_Tmp2;if(*((int*)_Tmp1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)){case 10: _Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2;{struct Cyc_List_List*nested_effs=_Tmp2;
# 714
effects=Cyc_List_revappend(nested_effs,effects);goto _LLE;}case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->hd)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->hd)->f1)==6){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2)->tl==0)
# 718
goto _LLE;else{goto _LL13;}}else{goto _LL13;}}else{goto _LL13;}}else{goto _LL13;}default: goto _LL13;}else{_LL13: _Tmp2=_Tmp1;{void*e=_Tmp2;
effects=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=effects;_Tmp3;});goto _LLE;}}_LLE:;}}
# 721
({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(effects);*es=_Tmp1;});
return e;}}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2->hd;{void*t=_Tmp0;
# 724
void*_Tmp1=Cyc_Absyn_compress(t);switch(*((int*)_Tmp1)){case 1:
 goto _LL19;case 2: _LL19:
 return e;default:
 return Cyc_Tcutil_rgns_of(t);};}}else{goto _LL5;}default: goto _LL5;}else{_LL5:
# 729
 return e;};}}
# 734
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype;fntype=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct)),fntype->tag=5,fntype->f1.tvars=0,fntype->f1.effect=eff,({
struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);fntype->f1.ret_tqual=_Tmp0;}),fntype->f1.ret_type=Cyc_Absyn_void_type,fntype->f1.args=0,fntype->f1.c_varargs=0,fntype->f1.cyc_varargs=0,fntype->f1.rgn_po=0,fntype->f1.qual_bnd=0,fntype->f1.attributes=0,fntype->f1.requires_clause=0,fntype->f1.requires_relns=0,fntype->f1.ensures_clause=0,fntype->f1.ensures_relns=0,fntype->f1.return_value=0,fntype->f1.arg_vardecls=0;
# 746
return({void*_Tmp0=(void*)fntype;void*_Tmp1=Cyc_Absyn_heap_rgn_type;void*_Tmp2=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);void*_Tmp4=Cyc_Absyn_bounds_one();void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);});}
# 753
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if(r==Cyc_Absyn_heap_rgn_type)
return 1;{
void*_Tmp0=Cyc_Absyn_compress(e);void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r2=_Tmp3;
# 760
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r==r2)return 1;{
struct _tuple15 _Tmp4=({struct _tuple15 _Tmp5;_Tmp5.f1=r,_Tmp5.f2=r2;_Tmp5;});void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp4.f1)==2){if(*((int*)_Tmp4.f2)==2){_Tmp6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp4.f1)->f1;_Tmp5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp4.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp6;struct Cyc_Absyn_Tvar*tv2=_Tmp5;
return Cyc_Absyn_tvar_cmp(tv1,tv2)==0;}}else{goto _LLE;}}else{_LLE:
 return 0;};}}}else{goto _LL9;}case 10: _Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
# 768
for(1;es!=0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp3;
# 772
void*_Tmp4=Cyc_Tcutil_rgns_of(t);void*_Tmp5;if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2!=0){_Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2->hd;{void*t=_Tmp5;
# 774
if(!constrain)return 0;{
void*_Tmp6=Cyc_Absyn_compress(t);void*_Tmp7;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp6)==1){_Tmp9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp6)->f1;_Tmp8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp6)->f2;_Tmp7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp6)->f4;{struct Cyc_Core_Opt*k=_Tmp9;void**p=(void**)_Tmp8;struct Cyc_Core_Opt*s=_Tmp7;
# 779
void*ev=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);
# 782
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)_check_null(s)->v,r);
({void*_TmpA=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_TmpB[2];_TmpB[0]=ev,({void*_TmpC=Cyc_Absyn_access_eff(r);_TmpB[1]=_TmpC;});Cyc_List_list(_tag_fat(_TmpB,sizeof(void*),2));})));*p=_TmpA;});
return 1;}}else{
return 0;};}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _Tmp5=_Tmp4;{void*e2=_Tmp5;
# 787
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f4;{struct Cyc_Core_Opt*k=_Tmp3;void**p=(void**)_Tmp2;struct Cyc_Core_Opt*s=_Tmp1;
# 790
if(k==0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind!=4)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("effect evar has wrong kind",sizeof(char),27U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(!constrain)return 0;{
# 795
void*ev=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);
# 798
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)_check_null(s)->v,r);
({void*_Tmp4=Cyc_Absyn_join_eff(({void*_Tmp5[2];_Tmp5[0]=ev,({void*_Tmp6=Cyc_Absyn_access_eff(r);_Tmp5[1]=_Tmp6;});Cyc_List_list(_tag_fat(_Tmp5,sizeof(void*),2));}));*p=_Tmp4;});
return 1;}}default: _LL9:
 return 0;};}}
# 808
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_Tmp0=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 9:
 return 0;case 10: _Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
# 813
for(1;es!=0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t2=_Tmp3;
# 818
t2=Cyc_Absyn_compress(t2);
if(t==t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_Tmp4=Cyc_Tcutil_rgns_of(t);void*_Tmp5;if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2!=0){_Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2->hd;{void*t3=_Tmp5;
# 823
struct _tuple15 _Tmp6=({struct _tuple15 _Tmp7;({void*_Tmp8=Cyc_Absyn_compress(t3);_Tmp7.f1=_Tmp8;}),_Tmp7.f2=t2;_Tmp7;});void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp6.f1)==2){if(*((int*)_Tmp6.f2)==2){_Tmp8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp6.f1)->f1;_Tmp7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp6.f2)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp8;struct Cyc_Absyn_Tvar*tv2=_Tmp7;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13:
 return t3==t2;};}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _Tmp5=_Tmp4;{void*e2=_Tmp5;
# 827
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}};}}}else{goto _LL9;}default: goto _LL9;}case 1: _Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f4;{struct Cyc_Core_Opt*k=_Tmp3;void**p=(void**)_Tmp2;struct Cyc_Core_Opt*s=_Tmp1;
# 830
if(k==0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind!=4)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("effect evar has wrong kind",sizeof(char),27U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(!may_constrain_evars)return 0;{
# 835
void*ev=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);
# 838
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)_check_null(s)->v,t);{
void*new_typ=Cyc_Absyn_join_eff(({void*_Tmp4[2];_Tmp4[0]=ev,({void*_Tmp5=Cyc_Absyn_regionsof_eff(t);_Tmp4[1]=_Tmp5;});Cyc_List_list(_tag_fat(_Tmp4,sizeof(void*),2));}));
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 848
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)e)){case 2: _Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)e)->f1;{struct Cyc_Absyn_Tvar*v2=_Tmp2;
return Cyc_Absyn_tvar_cmp(v,v2)==0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f1)){case 10: _Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2;{struct Cyc_List_List*es=_Tmp2;
# 853
for(1;es!=0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2->hd;{void*t=_Tmp2;
# 858
void*_Tmp3=Cyc_Tcutil_rgns_of(t);void*_Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f2!=0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f2->hd;{void*t2=_Tmp4;
# 860
if(!may_constrain_evars)return 0;{
void*_Tmp5=Cyc_Absyn_compress(t2);void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp5)==1){_Tmp8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f1;_Tmp7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f2;_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f4;{struct Cyc_Core_Opt*k=_Tmp8;void**p=(void**)_Tmp7;struct Cyc_Core_Opt*s=_Tmp6;
# 866
void*ev=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);
# 868
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)_check_null(s)->v,v))return 0;
({void*_Tmp9=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_TmpA[2];_TmpA[0]=ev,({void*_TmpB=Cyc_Absyn_var_type(v);_TmpA[1]=_TmpB;});Cyc_List_list(_tag_fat(_TmpA,sizeof(void*),2));})));*p=_Tmp9;});
return 1;}}else{
return 0;};}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _Tmp4=_Tmp3;{void*e2=_Tmp4;
# 873
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}};}}else{goto _LL9;}default: goto _LL9;}case 1: _Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)e)->f1;_Tmp1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)e)->f2;_Tmp0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)e)->f4;{struct Cyc_Core_Opt*k=_Tmp2;void**p=(void**)_Tmp1;struct Cyc_Core_Opt*s=_Tmp0;
# 876
if(k==0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind!=4)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("effect evar has wrong kind",sizeof(char),27U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),1));});{
# 880
void*ev=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);
# 882
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)_check_null(s)->v,v))
return 0;{
void*new_typ=Cyc_Absyn_join_eff(({void*_Tmp3[2];_Tmp3[0]=ev,({void*_Tmp4=Cyc_Absyn_var_type(v);_Tmp3[1]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(void*),2));}));
*p=new_typ;
return 1;}}}default: _LL9:
 return 0;};}}
# 892
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_Tmp0;switch(*((int*)e)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f1)){case 10: _Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2;{struct Cyc_List_List*es=_Tmp0;
# 896
for(1;es!=0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e)->f2->hd;{void*t=_Tmp0;
# 901
void*_Tmp1=Cyc_Tcutil_rgns_of(t);void*_Tmp2;if(*((int*)_Tmp1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2->hd;{void*t2=_Tmp2;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _Tmp2=_Tmp1;{void*e2=_Tmp2;
return Cyc_Tcutil_evar_in_effect(evar,e2);}};}}else{goto _LL7;}default: goto _LL7;}case 1:
# 905
 return evar==e;default: _LL7:
 return 0;};}}
# 919 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 922
void*_Tmp0=Cyc_Absyn_compress(e1);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 10: _Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp2;
# 924
for(1;es!=0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp2;
# 935
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp2;
# 939
void*_Tmp3=Cyc_Tcutil_rgns_of(t);void*_Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f2!=0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f2->hd;{void*t2=_Tmp4;
# 944
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _Tmp4=_Tmp3;{void*e=_Tmp4;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}};}}else{goto _LLB;}default: goto _LLB;}case 2: _Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*v=_Tmp2;
# 937
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1: _Tmp2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f4;{void**p=(void**)_Tmp2;struct Cyc_Core_Opt*s=_Tmp1;
# 949
if(Cyc_Tcutil_evar_in_effect(e1,e2))
return 0;
# 954
*p=Cyc_Absyn_empty_effect;
# 957
return 1;}default: _LLB:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("subset_effect: bad effect: ",sizeof(char),28U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)e1;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 968
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 970
{struct Cyc_List_List*r1=rpo1;for(0;r1!=0;r1=r1->tl){
struct _tuple15*_Tmp0=(struct _tuple15*)r1->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void*t1a=_Tmp2;void*t1b=_Tmp1;
int found=t1a==Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2!=0 && !found;r2=r2->tl){
struct _tuple15*_Tmp3=(struct _tuple15*)r2->hd;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3->f1;_Tmp4=_Tmp3->f2;{void*t2a=_Tmp5;void*t2b=_Tmp4;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 980
if(!found)return 0;}}}
# 982
return 1;}
# 986
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 988
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 991
static int Cyc_Tcutil_tycon2int(void*t){
switch(*((int*)t)){case 0:
 return 0;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)t)->f1){case Cyc_Absyn_Unsigned: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)t)->f2){case Cyc_Absyn_Char_sz:
 return 1;case Cyc_Absyn_Short_sz:
# 997
 return 4;case Cyc_Absyn_Int_sz:
# 1000
 return 7;case Cyc_Absyn_Long_sz:
# 1003
 return 7;case Cyc_Absyn_LongLong_sz:
# 1006
 return 13;default: goto _LL55;}case Cyc_Absyn_Signed: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)t)->f2){case Cyc_Absyn_Char_sz:
# 995
 return 2;case Cyc_Absyn_Short_sz:
# 998
 return 5;case Cyc_Absyn_Int_sz:
# 1001
 return 8;case Cyc_Absyn_Long_sz:
# 1004
 return 8;case Cyc_Absyn_LongLong_sz:
# 1007
 return 14;default: goto _LL55;}case Cyc_Absyn_None: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)t)->f2){case Cyc_Absyn_Char_sz:
# 996
 return 3;case Cyc_Absyn_Short_sz:
# 999
 return 6;case Cyc_Absyn_Int_sz:
# 1002
 return 9;case Cyc_Absyn_Long_sz:
# 1005
 return 9;case Cyc_Absyn_LongLong_sz:
# 1008
 return 15;default: goto _LL55;}default: _LL55:
# 1035
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad con",sizeof(char),8U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}case 2: switch((int)((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)t)->f1){case 0:
# 1009
 return 16;case 1:
 return 17;default:
 return 18;}case 4:
 return 19;case 5:
 return 20;case 6:
 return 21;case 7:
 return 22;case 8:
 return 23;case 9:
 return 24;case 10:
 return 25;case 11:
 return 26;case 12:
 return 27;case 13:
 return 28;case 15:
 return 29;case 14:
 return 30;case 20:
 return 31;case 21:
 return 32;case 22:
 return 33;case 23:
 return 34;case 24:
 return 35;case 25:
 return 36;case 3:
 return 37;case 18:
 return 38;case 17:
 return 39;case 16:
 return 40;default:
 return 41;};}
# 1038
static int Cyc_Tcutil_type_case_number(void*t){
void*_Tmp0;switch(*((int*)t)){case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;case 4:
 return 4;case 5:
 return 5;case 6:
 return 6;case 7:
 return 7;case 8:
 return 8;case 9:
 return 9;case 10:
 return 10;case 11:
 return 11;default: _Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;{void*c=_Tmp0;
return 12 + Cyc_Tcutil_tycon2int(c);}};}
# 1054
static int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1==a2)return 0;
if(a1==0 && a2!=0)return -1;
if(a1!=0 && a2==0)return 1;
return({int(*_Tmp0)(void*,void*)=cmp;void*_Tmp1=_check_null(a1);_Tmp0(_Tmp1,_check_null(a2));});}
# 1060
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int qc=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(qc!=0)return qc;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1065
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
void*_Tmp0;if(i.KnownDatatype.tag==2){_Tmp0=*i.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp0;
return dd->name;}}else{_Tmp0=i.UnknownDatatype.val.name;{struct _tuple1*n=_Tmp0;
return n;}};}struct _tuple18{struct _tuple1*f1;struct _tuple1*f2;};
# 1071
static struct _tuple18 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
void*_Tmp0;void*_Tmp1;if(i.KnownDatatypefield.tag==2){_Tmp1=i.KnownDatatypefield.val.f1;_Tmp0=i.KnownDatatypefield.val.f2;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp1;struct Cyc_Absyn_Datatypefield*df=_Tmp0;
# 1074
return({struct _tuple18 _Tmp2;_Tmp2.f1=dd->name,_Tmp2.f2=df->name;_Tmp2;});}}else{_Tmp1=i.UnknownDatatypefield.val.datatype_name;_Tmp0=i.UnknownDatatypefield.val.field_name;{struct _tuple1*d=_Tmp1;struct _tuple1*f=_Tmp0;
# 1076
return({struct _tuple18 _Tmp2;_Tmp2.f1=d,_Tmp2.f2=f;_Tmp2;});}};}struct _tuple19{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1079
static struct _tuple19 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
void*_Tmp0;enum Cyc_Absyn_AggrKind _Tmp1;if(i.UnknownAggr.tag==1){_Tmp1=i.UnknownAggr.val.f1;_Tmp0=i.UnknownAggr.val.f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct _tuple1*n=_Tmp0;
return({struct _tuple19 _Tmp2;_Tmp2.f1=k,_Tmp2.f2=n;_Tmp2;});}}else{_Tmp0=*i.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
return({struct _tuple19 _Tmp2;_Tmp2.f1=ad->kind,_Tmp2.f2=ad->name;_Tmp2;});}};}
# 1085
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1==t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1!=i2)return i1 - i2;{
# 1091
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});union Cyc_Absyn_AggrInfo _Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;union Cyc_Absyn_DatatypeFieldInfo _Tmp3;union Cyc_Absyn_DatatypeFieldInfo _Tmp4;union Cyc_Absyn_DatatypeInfo _Tmp5;union Cyc_Absyn_DatatypeInfo _Tmp6;struct _fat_ptr _Tmp7;struct _fat_ptr _Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp0.f1)){case 20: if(*((int*)_Tmp0.f2)==20){_TmpA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{struct _tuple1*n1=_TmpA;struct _tuple1*n2=_Tmp9;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 22: if(*((int*)_Tmp0.f2)==22){_Tmp8=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{struct _fat_ptr s1=_Tmp8;struct _fat_ptr s2=_Tmp7;
return Cyc_strcmp(s1,s2);}}else{goto _LLD;}case 21: if(*((int*)_Tmp0.f2)==21){_TmpA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{struct Cyc_List_List*fs1=_TmpA;struct Cyc_List_List*fs2=_Tmp9;
# 1095
return({(int(*)(int(*)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 23: if(*((int*)_Tmp0.f2)==23){_Tmp6=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp5=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{union Cyc_Absyn_DatatypeInfo info1=_Tmp6;union Cyc_Absyn_DatatypeInfo info2=_Tmp5;
# 1097
return({struct _tuple1*_TmpB=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_TmpB,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 24: if(*((int*)_Tmp0.f2)==24){_Tmp4=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{union Cyc_Absyn_DatatypeFieldInfo info1=_Tmp4;union Cyc_Absyn_DatatypeFieldInfo info2=_Tmp3;
# 1099
struct _tuple18 _TmpB=Cyc_Tcutil_get_datatype_field_qvars(info1);void*_TmpC;void*_TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{struct _tuple1*d1=_TmpD;struct _tuple1*f1=_TmpC;
struct _tuple18 _TmpE=Cyc_Tcutil_get_datatype_field_qvars(info2);void*_TmpF;void*_Tmp10;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{struct _tuple1*d2=_Tmp10;struct _tuple1*f2=_TmpF;
int c=Cyc_Absyn_qvar_cmp(d1,d2);
if(c!=0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 25: if(*((int*)_Tmp0.f2)==25){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp0.f2)->f1;{union Cyc_Absyn_AggrInfo info1=_Tmp2;union Cyc_Absyn_AggrInfo info2=_Tmp1;
# 1105
struct _tuple19 _TmpB=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);void*_TmpC;enum Cyc_Absyn_AggrKind _TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{enum Cyc_Absyn_AggrKind k1=_TmpD;struct _tuple1*q1=_TmpC;
struct _tuple19 _TmpE=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);void*_TmpF;enum Cyc_Absyn_AggrKind _Tmp10;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{enum Cyc_Absyn_AggrKind k2=_Tmp10;struct _tuple1*q2=_TmpF;
int c=Cyc_Absyn_qvar_cmp(q1,q2);
if(c!=0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD:
 return 0;};}}}
# 1114
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int i1=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int i2=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(i1,i2);}
# 1119
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt1->f1;_Tmp0=tqt1->f2;{struct Cyc_Absyn_Tqual tq1=_Tmp1;void*t1=_Tmp0;
void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=tqt2->f1;_Tmp2=tqt2->f2;{struct Cyc_Absyn_Tqual tq2=_Tmp3;void*t2=_Tmp2;
int tqc=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(tqc!=0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1127
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int zsc=Cyc_strptrcmp(f1->name,f2->name);
if(zsc!=0)return zsc;{
int tqc=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(tqc!=0)return tqc;{
int tc=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(tc!=0)return tc;{
int ac=Cyc_List_list_cmp(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);
if(ac!=0)return ac;
ac=({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac!=0)return ac;
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1144
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1==t2)return 0;{
int shallowcmp=({int _Tmp0=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_Tmp0,Cyc_Tcutil_type_case_number(t2));});
if(shallowcmp!=0)
return shallowcmp;{
# 1153
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f1=t2,_Tmp1.f2=t1;_Tmp1;});enum Cyc_Absyn_AggrKind _Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;struct Cyc_Absyn_FnInfo _Tmp3;struct Cyc_Absyn_FnInfo _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;struct Cyc_Absyn_Tqual _TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;void*_Tmp11;void*_Tmp12;void*_Tmp13;void*_Tmp14;switch(*((int*)_Tmp0.f1)){case 0: if(*((int*)_Tmp0.f2)==0){_Tmp14=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp12=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp11=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2;{void*c1=_Tmp14;struct Cyc_List_List*ts1=_Tmp13;void*c2=_Tmp12;struct Cyc_List_List*ts2=_Tmp11;
# 1155
int c=Cyc_Tcutil_tycon_cmp(c1,c2);
if(c!=0)return c;
return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL17;}case 1: if(*((int*)_Tmp0.f2)==1)
# 1159
return(int)t1 - (int)t2;else{goto _LL17;}case 2: if(*((int*)_Tmp0.f2)==2){_Tmp14=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp14;struct Cyc_Absyn_Tvar*tv1=_Tmp13;
# 1164
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL17;}case 3: if(*((int*)_Tmp0.f2)==3){_Tmp14=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp13=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.rgn;_Tmp12=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_type;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_tq;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.rgn;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.nullable;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.bounds;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.zero_term;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.autoreleased;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.aqual;{void*t2a=_Tmp14;struct Cyc_Absyn_Tqual tqual2a=_Tmp10;void*rgn2=_Tmp13;void*null2a=_Tmp12;void*b2=_Tmp11;void*zt2=_TmpF;void*rel2=_TmpE;void*aq2=_TmpD;void*t1a=_TmpC;struct Cyc_Absyn_Tqual tqual1a=_TmpB;void*rgn1=_TmpA;void*null1a=_Tmp9;void*b1=_Tmp8;void*zt1=_Tmp7;void*rel1=_Tmp6;void*aq1=_Tmp5;
# 1168
int etc=Cyc_Tcutil_typecmp(t1a,t2a);
if(etc!=0)return etc;{
int rc=Cyc_Tcutil_typecmp(rgn1,rgn2);
if(rc!=0)return rc;{
int tqc=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);
if(tqc!=0)return tqc;{
int cc=Cyc_Tcutil_typecmp(b1,b2);
if(cc!=0)return cc;{
int zc=Cyc_Tcutil_typecmp(zt1,zt2);
if(zc!=0)return zc;{
int relc=Cyc_Tcutil_typecmp(rel1,rel2);
if(relc!=0)return relc;{
int bc=Cyc_Tcutil_typecmp(b1,b2);
if(bc!=0)return bc;
if(aq1!=aq2)return 1;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}}else{goto _LL17;}case 4: if(*((int*)_Tmp0.f2)==4){_Tmp14=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp10=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.tq;_Tmp13=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.num_elts;_Tmp12=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.zero_term;_Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_type;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.tq;_TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.num_elts;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.zero_term;{void*t2a=_Tmp14;struct Cyc_Absyn_Tqual tq2a=_Tmp10;struct Cyc_Absyn_Exp*e1=_Tmp13;void*zt1=_Tmp12;void*t1a=_Tmp11;struct Cyc_Absyn_Tqual tq1a=_TmpB;struct Cyc_Absyn_Exp*e2=_TmpF;void*zt2=_TmpE;
# 1187
int tqc=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);
if(tqc!=0)return tqc;{
int tc=Cyc_Tcutil_typecmp(t1a,t2a);
if(tc!=0)return tc;{
int ztc=Cyc_Tcutil_typecmp(zt1,zt2);
if(ztc!=0)return ztc;
if(e1==e2)return 0;
if(e1==0 || e2==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("missing expression in array index",sizeof(char),34U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp16,sizeof(void*),1));});
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL17;}case 5: if(*((int*)_Tmp0.f2)==5){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_Tmp4;struct Cyc_Absyn_FnInfo f2=_Tmp3;
# 1199
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r!=0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r!=0)return r;{
struct Cyc_List_List*args1=f1.args;
struct Cyc_List_List*args2=f2.args;
for(1;args1!=0 && args2!=0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _Tmp15=*((struct _tuple9*)args1->hd);void*_Tmp16;struct Cyc_Absyn_Tqual _Tmp17;_Tmp17=_Tmp15.f2;_Tmp16=_Tmp15.f3;{struct Cyc_Absyn_Tqual tq1=_Tmp17;void*t1=_Tmp16;
struct _tuple9 _Tmp18=*((struct _tuple9*)args2->hd);void*_Tmp19;struct Cyc_Absyn_Tqual _Tmp1A;_Tmp1A=_Tmp18.f2;_Tmp19=_Tmp18.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp1A;void*t2=_Tmp19;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r!=0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r!=0)return r;}}}
# 1214
if(args1!=0)return 1;
if(args2!=0)return -1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return -1;
if(f1.cyc_varargs!=0 & f2.cyc_varargs==0)return 1;
if(f1.cyc_varargs==0 & f2.cyc_varargs!=0)return -1;
if(f1.cyc_varargs!=0 & f2.cyc_varargs!=0){
r=({struct Cyc_Absyn_Tqual _Tmp15=_check_null(f1.cyc_varargs)->tq;Cyc_Tcutil_tqual_cmp(_Tmp15,_check_null(f2.cyc_varargs)->tq);});
if(r!=0)return r;
r=Cyc_Tcutil_typecmp(f1.cyc_varargs->type,f2.cyc_varargs->type);
if(r!=0)return r;
if(f1.cyc_varargs->inject && !f2.cyc_varargs->inject)return 1;
if(!f1.cyc_varargs->inject && f2.cyc_varargs->inject)return -1;}
# 1228
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r!=0)return r;{
struct Cyc_List_List*rpo1=f1.rgn_po;
struct Cyc_List_List*rpo2=f2.rgn_po;
for(1;rpo1!=0 && rpo2!=0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _Tmp15=*((struct _tuple15*)rpo1->hd);void*_Tmp16;void*_Tmp17;_Tmp17=_Tmp15.f1;_Tmp16=_Tmp15.f2;{void*t1a=_Tmp17;void*t1b=_Tmp16;
struct _tuple15 _Tmp18=*((struct _tuple15*)rpo2->hd);void*_Tmp19;void*_Tmp1A;_Tmp1A=_Tmp18.f1;_Tmp19=_Tmp18.f2;{void*t2a=_Tmp1A;void*t2b=_Tmp19;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r!=0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r!=0)return r;}}}
# 1238
if(rpo1!=0)return 1;
if(rpo2!=0)return -1;
r=({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r!=0)return r;
r=({(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp;})(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r!=0)return r;
# 1246
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("typecmp: function type comparison should never get here!",sizeof(char),57U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp16,sizeof(void*),1));});}}}}}else{goto _LL17;}case 6: if(*((int*)_Tmp0.f2)==6){_Tmp14=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_List_List*ts2=_Tmp14;struct Cyc_List_List*ts1=_Tmp13;
# 1249
return({(int(*)(int(*)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL17;}case 7: if(*((int*)_Tmp0.f2)==7){_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp14=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp13=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f2)->f2;{enum Cyc_Absyn_AggrKind k2=_Tmp2;struct Cyc_List_List*fs2=_Tmp14;enum Cyc_Absyn_AggrKind k1=_Tmp1;struct Cyc_List_List*fs1=_Tmp13;
# 1252
if((int)k1!=(int)k2)
return(int)k1==0?-1: 1;
return({(int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL17;}case 9: if(*((int*)_Tmp0.f2)==9){_Tmp14=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp14;struct Cyc_Absyn_Exp*e2=_Tmp13;
# 1256
_Tmp14=e1;_Tmp13=e2;goto _LL14;}}else{goto _LL17;}case 11: if(*((int*)_Tmp0.f2)==11){_Tmp14=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp14;struct Cyc_Absyn_Exp*e2=_Tmp13;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL17;}case 8: if(*((int*)_Tmp0.f2)==8){_Tmp14=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp13=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp12=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f4;_Tmp11=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f2)->f1;_TmpF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f2)->f2;_TmpE=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f2)->f4;{struct _tuple1*n1=_Tmp14;struct Cyc_List_List*ts1=_Tmp13;void*def1=_Tmp12;struct _tuple1*n2=_Tmp11;struct Cyc_List_List*ts2=_TmpF;void*def2=_TmpE;
# 1260
if(def1!=0 && def2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("typecmp: compress did not remove typedefs",sizeof(char),42U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp16,sizeof(void*),1));});{
int i=Cyc_Absyn_qvar_cmp(n1,n2);if(i!=0)return i;
return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,ts1,ts2);}}}else{goto _LL17;}default: _LL17:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("Unmatched case in typecmp",sizeof(char),26U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp16,sizeof(void*),1));});};}}}
# 1270
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f2=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1;{void*c1=_Tmp2;void*c2=_Tmp1;
# 1273
struct _tuple15 _Tmp3=({struct _tuple15 _Tmp4;_Tmp4.f1=c1,_Tmp4.f2=c2;_Tmp4;});int _Tmp4;int _Tmp5;switch(*((int*)_Tmp3.f1)){case 2: switch(*((int*)_Tmp3.f2)){case 2: _Tmp5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f1)->f1;_Tmp4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1;{int i1=_Tmp5;int i2=_Tmp4;
return i2 < i1;}case 1:
 goto _LLB;case 5: _LLB:
 return 1;default: goto _LL26;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2){case Cyc_Absyn_LongLong_sz: if(*((int*)_Tmp3.f2)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz)
return 0;else{goto _LLE;}}else{_LLE:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_Tmp3.f2)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2){case Cyc_Absyn_Int_sz:
# 1281
 goto _LL13;case Cyc_Absyn_Short_sz: _LL19:
# 1286
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1F:
# 1289
 goto _LL21;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1==0)
# 1284
goto _LL17;else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_Tmp3.f2)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2){case Cyc_Absyn_Long_sz: _LL13:
# 1282
 return 0;case Cyc_Absyn_Short_sz: _LL1B:
# 1287
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL21:
# 1290
 goto _LL23;default: goto _LL26;}case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1==0){_LL17:
# 1285
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(*((int*)_Tmp3.f2)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Char_sz){_LL23:
# 1291
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 5: if(*((int*)_Tmp3.f2)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2){case Cyc_Absyn_Short_sz: _LL1D:
# 1288
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL25:
# 1292
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26:
# 1294
 return 0;};}}else{goto _LL3;}}else{_LL3:
# 1296
 return 0;};}
# 1300
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f2=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1;{void*c1=_Tmp2;void*c2=_Tmp1;
# 1303
{struct _tuple15 _Tmp3=({struct _tuple15 _Tmp4;_Tmp4.f1=c1,_Tmp4.f2=c2;_Tmp4;});int _Tmp4;int _Tmp5;if(*((int*)_Tmp3.f1)==2){if(*((int*)_Tmp3.f2)==2){_Tmp5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f1)->f1;_Tmp4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1;{int i1=_Tmp5;int i2=_Tmp4;
# 1305
if(i1!=0 && i1!=1)return t1;
if(i2!=0 && i2!=1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{
return t1;}}else{if(*((int*)_Tmp3.f2)==2)
return t2;else{if(*((int*)_Tmp3.f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f1==Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_LongLong_sz)
goto _LLF;else{if(*((int*)_Tmp3.f2)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1==Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz)goto _LL12;else{switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Long_sz){_LL14:
# 1315
 goto _LL17;}else{if(*((int*)_Tmp3.f2)==5)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1320
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(*((int*)_Tmp3.f2)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1==Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_LongLong_sz)goto _LL10;else{switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_LongLong_sz){_LL10:
# 1313
 goto _LL13;}else{if(*((int*)_Tmp3.f2)==5)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f1)->f2==Cyc_Absyn_Long_sz){_LL20:
# 1322
 goto _LL23;}else{goto _LL24;}}}}}}else{if(*((int*)_Tmp3.f2)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f1==Cyc_Absyn_Unsigned)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1312
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1316
 return Cyc_Absyn_ulong_type;default: if(*((int*)_Tmp3.f1)==5)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1321
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1314
 return Cyc_Absyn_slonglong_type;}else{if(*((int*)_Tmp3.f1)==5)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_Tmp3.f2)->f2==Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1323
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(*((int*)_Tmp3.f1)==5){_LL18:
# 1318
 goto _LL1B;}else{if(*((int*)_Tmp3.f2)==5){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24:
# 1324
 goto _LL5;}}}}}}_LL5:;}
# 1326
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 1329
return Cyc_Absyn_sint_type;}
# 1334
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*aqb,void*t,struct Cyc_List_List*es){
# 1337
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el!=0;el=el->tl){
void*t1=Cyc_Absyn_compress(_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type==0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=t1;_Tmp0;});}}}
# 1345
if(max_arith_type!=0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1349
{struct Cyc_List_List*el=es;for(0;el!=0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,aqb,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type mismatch: expecting ",sizeof(char),26U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(((struct Cyc_Absyn_Exp*)el->hd)->loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return 0;}}}
# 1355
return 1;}
# 1360
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));if(*((int*)_Tmp0)==3){
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,3U);goto _LL0;}else{
return 0;}_LL0:;}
# 1366
return 1;}
# 1369
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify(_check_null(e->topt),Xint_type))
return 1;
# 1373
if(Cyc_Tcutil_is_integral_type(_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision(_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("integral size mismatch; conversion supplied",sizeof(char),44U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Tcutil_unchecked_cast(e,Xint_type,1U);
return 1;}
# 1379
return 0;}
# 1382
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1386
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1392
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1398
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_Tmp0=t;Cyc_Unify_unify(_Tmp0,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1403
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*t=Cyc_Absyn_compress((void*)ts->hd);
void*_Tmp0;if(*((int*)t)==9){_Tmp0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return e;}}else{
# 1408
struct Cyc_Absyn_Exp*v=Cyc_Absyn_valueof_exp(t,0U);
v->topt=Cyc_Absyn_uint_type;
return v;};}
# 1417
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_Tmp0=Cyc_Absyn_compress(b);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 15:
 return 0;case 14: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("get_bounds_exp: ",sizeof(char),17U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)b;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}}
# 1426
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*b=_Tmp1;
# 1429
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("get_ptr_bounds_exp not pointer: ",sizeof(char),33U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}
# 1435
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_boolko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tvs;_Tmp0;}));}
# 1439
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_ptrbko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tvs;_Tmp0;}));}
# 1444
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple20{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1451
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;struct Cyc_Absyn_PtrInfo _Tmp9;struct Cyc_Absyn_PtrInfo _TmpA;switch(*((int*)_Tmp0.f1)){case 3: if(*((int*)_Tmp0.f2)==3){_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_PtrInfo pinfo_a=_TmpA;struct Cyc_Absyn_PtrInfo pinfo_b=_Tmp9;
# 1457
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.nullable,pinfo_b.ptr_atts.nullable)&&
 Cyc_Tcutil_force_type2bool(0,pinfo_a.ptr_atts.nullable))
return 0;
# 1461
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.bounds,pinfo_b.ptr_atts.bounds)){
struct _tuple20 _TmpB=({struct _tuple20 _TmpC;({struct Cyc_Absyn_Exp*_TmpD=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,pinfo_a.ptr_atts.bounds);});_TmpC.f1=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,pinfo_b.ptr_atts.bounds);});_TmpC.f2=_TmpD;});_TmpC;});
# 1462
void*_TmpC;void*_TmpD;if(_TmpB.f2==0)
# 1465
goto _LL7;else{if(_TmpB.f1==0){
# 1469
if(Cyc_Tcutil_force_type2bool(0,pinfo_a.ptr_atts.zero_term)&&({
void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_TmpE,pinfo_b.ptr_atts.bounds);}))
goto _LL7;
return 0;}else{_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{struct Cyc_Absyn_Exp*e1=_TmpD;struct Cyc_Absyn_Exp*e2=_TmpC;
# 1477
if(!Cyc_Tcutil_force_type2bool(0,pinfo_b.ptr_atts.zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("implicit cast to shorter array",sizeof(char),31U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_warn2(loc,_tag_fat(_TmpF,sizeof(void*),1));});
if(!({struct Cyc_Absyn_Exp*_TmpE=_check_null(e2);Cyc_Evexp_lte_const_exp(_TmpE,_check_null(e1));}))
return 0;
goto _LL7;}}}_LL7:;}
# 1485
if(pinfo_a.elt_tq.real_const && !pinfo_b.elt_tq.real_const)
return 0;
# 1488
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.rgn,pinfo_b.ptr_atts.rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,pinfo_a.ptr_atts.rgn,pinfo_b.ptr_atts.rgn))
# 1491
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("implicit cast from region ",sizeof(char),27U);_TmpC;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpD;_TmpD.tag=2,_TmpD.f1=(void*)pinfo_a.ptr_atts.rgn;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=
_tag_fat(" to region ",sizeof(char),12U);_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)pinfo_b.ptr_atts.rgn;_TmpF;});void*_TmpF[4];_TmpF[0]=& _TmpB,_TmpF[1]=& _TmpC,_TmpF[2]=& _TmpD,_TmpF[3]=& _TmpE;Cyc_Warn_warn2(loc,_tag_fat(_TmpF,sizeof(void*),4));});}
# 1497
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.aqual,pinfo_b.ptr_atts.aqual)&& !
Cyc_Tcutil_alias_qual_subtype(pinfo_a.ptr_atts.aqual,pinfo_b.ptr_atts.aqual))
return 0;
# 1501
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.zero_term,pinfo_b.ptr_atts.zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,pinfo_a.ptr_atts.zero_term)|| !pinfo_b.elt_tq.real_const))
# 1504
return 0;
# 1506
if(!Cyc_Unify_unify(pinfo_a.ptr_atts.autoreleased,pinfo_b.ptr_atts.autoreleased)&& !
Cyc_Tcutil_force_type2bool(1,pinfo_a.ptr_atts.autoreleased))
return 0;
# 1512
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1516
if(!({void*_TmpB=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_TmpB,pinfo_b.ptr_atts.bounds);})||
 Cyc_Tcutil_force_type2bool(0,pinfo_b.ptr_atts.zero_term))
return 0;
if(!pinfo_b.elt_tq.real_const && !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL5;}case 4: if(*((int*)_Tmp0.f2)==4){_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.tq;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.num_elts;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.zero_term;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.tq;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.num_elts;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1.zero_term;{void*t1a=_Tmp8;struct Cyc_Absyn_Tqual tq1a=_Tmp7;struct Cyc_Absyn_Exp*e1=_Tmp6;void*zt1=_Tmp5;void*t2a=_Tmp4;struct Cyc_Absyn_Tqual tq2a=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;void*zt2=_Tmp1;
# 1526
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1==0 || e2==0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1531
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL5;}default: _LL5:
 return Cyc_Unify_unify(t1,t2);};}}
# 1536
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*e=_Tmp1;
return e;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("pointer_elt_type",sizeof(char),17U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}
# 1542
void*Cyc_Tcutil_pointer_region(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(struct Cyc_Absyn_PtrAtts*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts;{struct Cyc_Absyn_PtrAtts*p=(struct Cyc_Absyn_PtrAtts*)_Tmp1;
return p->rgn;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("pointer_elt_type",sizeof(char),17U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}
# 1549
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;{void*r=_Tmp1;
*rgn=r;return 1;}}else{
return 0;};}
# 1556
void*Cyc_Tcutil_pointer_aqual(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(struct Cyc_Absyn_PtrAtts*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts;{struct Cyc_Absyn_PtrAtts*p=(struct Cyc_Absyn_PtrAtts*)_Tmp1;
return p->aqual;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("pointer_elt_type",sizeof(char),17U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}
# 1565
int Cyc_Tcutil_admits_zero(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1:
 goto _LL4;case 2: _LL4:
 return 1;default: goto _LL7;}case 3: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*nullable=_Tmp2;void*bounds=_Tmp1;
# 1573
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7:
 return 0;};}
# 1579
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;struct _fat_ptr _Tmp3;switch(*((int*)_Tmp0)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wchar_c.tag){case 5: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f2==0)
goto _LL4;else{goto _LLF;}case 2: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Char_c.val.f2==0){_LL4:
 goto _LL6;}else{goto _LLF;}case 4: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Short_c.val.f2==0){_LL6:
 goto _LL8;}else{goto _LLF;}case 6: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.LongLong_c.val.f2==0){_LL8:
 goto _LLA;}else{goto _LLF;}case 3: _Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wchar_c.val;{struct _fat_ptr s=_Tmp3;
# 1587
unsigned long l=Cyc_strlen(s);
int i=0;
if(l >= 2U &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))==92){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))==48)i=2;else{
if(((int)((const char*)s.curr)[1]==120 && l >= 3U)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))==48)i=3;else{
return 0;}}
for(1;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!=48)return 0;}
return 1;}
# 1597
return 0;}default: goto _LLF;}case 2: _LLA:
# 1585
 return 1;case 14: _Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1598
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF:
 return 0;};}
# 1605
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_Tmp0=Cyc_Absyn_compress(t2);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*nbl=_Tmp1;
# 1610
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_Tmp2=Cyc_Absyn_null_exp(0U)->r;e1->r=_Tmp2;});
e1->topt=t2;
return 1;}}else{
return 0;};}}
# 1619
int Cyc_Tcutil_is_aliasable_qual(void*aq){
void*_Tmp0=Cyc_Absyn_compress(aq);void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 17: _Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_Tmp2;
# 1622
return(int)aqv==0;}case 18: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*tv_bnd=_Tmp1;
# 1624
return Cyc_Tcutil_is_aliasable_qual((void*)_check_null(_check_null(tv_bnd)->tl)->hd);}default: goto _LL5;}else{_LL5:
# 1626
 return 0;};}struct _tuple21{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;int f3;};
# 1635
struct _tuple21 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Kinds_rk};
int success=1;
# 1639
static int counter=0;
struct _tuple1*v;v=_cycalloc(sizeof(struct _tuple1)),v->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)counter ++;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("__aliasvar%d",sizeof(char),13U),_tag_fat(_Tmp4,sizeof(void*),1));});*_Tmp1=_Tmp2;});_Tmp1;});v->f2=_Tmp0;});{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_Tmp0=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp1->tag=4,_Tmp1->f1=vd;_Tmp1;});Cyc_Absyn_varb_exp(_Tmp0,e->loc);});
# 1648
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_Tmp0=Cyc_Absyn_compress(e_type);void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;if(*((int*)_Tmp0)==3){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.aqual;{void*et=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*old_r=_Tmp2;void*aq=_Tmp1;
# 1651
{void*_Tmp5=Cyc_Absyn_compress(old_r);void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp5)==1){_Tmp7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f2;_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f4;{void**topt=(void**)_Tmp7;struct Cyc_Core_Opt*ts=_Tmp6;
# 1653
void*new_r=Cyc_Absyn_var_type(tv);
*topt=new_r;
goto _LL5;}}else{
# 1671 "tcutil.cyc"
goto _LL5;}_LL5:;}
# 1675
if(!Cyc_Tcutil_is_aliasable_qual(aq)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Automatic alias coercion of restricted pointers not supported",sizeof(char),62U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),1));});
success=0;}}}else{
# 1679
goto _LL0;}_LL0:;}
# 1681
e->topt=0;
vd->initializer=0;{
# 1685
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1687
return({struct _tuple21 _Tmp0;_Tmp0.f1=d,_Tmp0.f2=ve,_Tmp0.f3=success;_Tmp0;});}}}
# 1692
static int Cyc_Tcutil_can_insert_alias(struct Cyc_List_List*aqb,struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1695
if((Cyc_Tcutil_is_noalias_path(aqb,e)&&
 Cyc_Tcutil_is_noalias_pointer(aqb,e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1700
void*_Tmp0=Cyc_Absyn_compress(wants_type);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.aqual;{void*r2=_Tmp2;void*aq=_Tmp1;
# 1702
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(r2);
return(int)k->kind==3 &&
# 1706
 Cyc_Tcutil_is_aliasable_qual(aq);}}}else{
return 0;};}
# 1710
return 0;}
# 1714
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*aqb,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1718
void*t1=Cyc_Absyn_compress(_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
# 1721
int do_alias_coercion=Cyc_Tcutil_can_insert_alias(aqb,e,t1,t2,e->loc);
# 1723
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1726
if((Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1))&& !Cyc_Tcutil_is_tag_type(t1)){
# 1728
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("integral size mismatch; ",sizeof(char),25U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" -> ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" conversion supplied",sizeof(char),21U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp5,sizeof(void*),5));});
Cyc_Tcutil_unchecked_cast(e,t2,1U);
return 1;}
# 1736
if(do_alias_coercion){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("implicit alias coercion for ",sizeof(char),29U);_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(":",sizeof(char),2U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t1;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
if(alias_coercion!=0)
*alias_coercion=1;}
# 1743
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,3U);
return 1;}
# 1747
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!=0){
# 1751
if((int)c!=1)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c!=2)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("implicit cast from ",sizeof(char),20U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return 1;}
# 1757
return 0;}
# 1763
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*aqb,struct Cyc_Absyn_Exp*e,void*t){
# 1766
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,aqb,e,t,& bogus);}
# 1776
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple22{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1778
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple22*env,struct Cyc_Absyn_Aggrfield*x){
# 1781
struct _tuple22 _Tmp0=*env;int _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_List_List*inst=_Tmp3;struct _RegionHandle*r=_Tmp2;int flatten=_Tmp1;
void*t=inst==0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);
struct Cyc_List_List*ts=Cyc_Tcutil_flatten_type(r,flatten,t);
if(Cyc_List_length(ts)==1)
return({struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp5=({struct _tuple12*_Tmp6=_region_malloc(r,0U,sizeof(struct _tuple12));_Tmp6->f1=x->tq,_Tmp6->f2=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});
return ts;}}struct _tuple23{struct _RegionHandle*f1;int f2;};
# 1788
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple23*env,struct _tuple12*x){
# 1790
struct _tuple23 _Tmp0=*env;int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _RegionHandle*r=_Tmp2;int flatten=_Tmp1;
struct _tuple12 _Tmp3=*x;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;
struct Cyc_List_List*ts=Cyc_Tcutil_flatten_type(r,flatten,t);
if(Cyc_List_length(ts)==1)
return({struct Cyc_List_List*_Tmp6=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp7=({struct _tuple12*_Tmp8=_region_malloc(r,0U,sizeof(struct _tuple12));_Tmp8->f1=tq,_Tmp8->f2=t;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=0;_Tmp6;});
return ts;}}}
# 1797
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1799
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_Tmp0;void*_Tmp1;switch(*((int*)t1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)){case 0:
 return 0;case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.val;_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 1817
if((((int)ad->kind==1 || ad->impl==0)||
 _check_null(ad->impl)->exist_vars!=0)|| _check_null(ad->impl)->rgn_po!=0)
return({struct Cyc_List_List*_Tmp2=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp3=({struct _tuple12*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple12));({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);_Tmp4->f1=_Tmp5;}),_Tmp4->f2=t1;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});{
struct Cyc_List_List*inst=Cyc_List_rzip(r,r,ad->tvs,ts);
struct _tuple22 env=({struct _tuple22 _Tmp2;_Tmp2.f1=inst,_Tmp2.f2=r,_Tmp2.f3=flatten;_Tmp2;});
struct Cyc_List_List*_Tmp2=_check_null(ad->impl)->fields;void*_Tmp3;void*_Tmp4;if(_Tmp2==0)
return 0;else{_Tmp4=(struct Cyc_Absyn_Aggrfield*)_Tmp2->hd;_Tmp3=_Tmp2->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp4;struct Cyc_List_List*tl=_Tmp3;
# 1825
struct Cyc_List_List*hd2=Cyc_Tcutil_flatten_type_f(& env,hd);
env.f3=0;{
struct Cyc_List_List*tl2=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c;})(r,Cyc_Tcutil_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}}else{goto _LL9;}default: goto _LL9;}case 6: _Tmp1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)t1)->f1;{struct Cyc_List_List*tqs=_Tmp1;
# 1804
struct _tuple23 env=({struct _tuple23 _Tmp2;_Tmp2.f1=r,_Tmp2.f2=flatten;_Tmp2;});
# 1806
void*_Tmp2;void*_Tmp3;if(tqs==0)
return 0;else{_Tmp3=(struct _tuple12*)tqs->hd;_Tmp2=tqs->tl;{struct _tuple12*hd=_Tmp3;struct Cyc_List_List*tl=_Tmp2;
# 1809
struct Cyc_List_List*hd2=Cyc_Tcutil_rcopy_tqt(& env,hd);
env.f2=0;{
struct Cyc_List_List*tl2=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple23*,struct _tuple12*),struct _tuple23*,struct Cyc_List_List*))Cyc_List_rmap_c;})(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);
struct Cyc_List_List*temp;temp=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),temp->hd=hd2,temp->tl=tl2;
return({struct _RegionHandle*_Tmp4=r;Cyc_List_rflatten(_Tmp4,({struct Cyc_List_List*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=hd2,_Tmp5->tl=tl2;_Tmp5;}));});}}};}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f1==Cyc_Absyn_StructA){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f2;{struct Cyc_List_List*fs=_Tmp1;
# 1832
struct _tuple22 env=({struct _tuple22 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=r,_Tmp2.f3=flatten;_Tmp2;});
void*_Tmp2;void*_Tmp3;if(fs==0)
return 0;else{_Tmp3=(struct Cyc_Absyn_Aggrfield*)fs->hd;_Tmp2=fs->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp3;struct Cyc_List_List*tl=_Tmp2;
# 1836
struct Cyc_List_List*hd2=Cyc_Tcutil_flatten_type_f(& env,hd);
env.f3=0;{
struct Cyc_List_List*tl2=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*,struct Cyc_List_List*))Cyc_List_rmap_c;})(r,Cyc_Tcutil_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}else{goto _LL9;}default: _LL9:
# 1842
 goto _LL0;}_LL0:;}}
# 1845
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp1=({struct _tuple12*_Tmp2=_region_malloc(r,0U,sizeof(struct _tuple12));({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t1;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});}
# 1848
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f2=_Tmp2;});_Tmp1;});enum Cyc_Absyn_Size_of _Tmp1;enum Cyc_Absyn_Size_of _Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==1){_Tmp2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f2;_Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f2;{enum Cyc_Absyn_Size_of b1=_Tmp2;enum Cyc_Absyn_Size_of b2=_Tmp1;
# 1851
return((int)b1==(int)b2 ||(int)b1==2 &&(int)b2==3)||
(int)b1==3 &&(int)b2==2;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1859
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1861
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a!=0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;switch(*((int*)_Tmp0.f1)){case 3: if(*((int*)_Tmp0.f2)==3){_Tmp12=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.rgn;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_type;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.elt_tq;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.rgn;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.nullable;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.bounds;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.zero_term;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.autoreleased;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1.ptr_atts.aqual;{void*t_a=_Tmp12;struct Cyc_Absyn_Tqual q_a=_Tmp11;void*rt_a=_Tmp10;void*null_a=_TmpF;void*b_a=_TmpE;void*zt_a=_TmpD;void*rel_a=_TmpC;void*aq_a=_TmpB;void*t_b=_TmpA;struct Cyc_Absyn_Tqual q_b=_Tmp9;void*rt_b=_Tmp8;void*null_b=_Tmp7;void*b_b=_Tmp6;void*zt_b=_Tmp5;void*rel_b=_Tmp4;void*aq_b=_Tmp3;
# 1873
if(q_a.real_const && !q_b.real_const)
return 0;
# 1876
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1880
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1884
if((Cyc_Unify_unify(rel_a,rel_b)&& !
Cyc_Absyn_type2bool(0,rel_a))&& Cyc_Absyn_type2bool(0,rel_b))
return 0;
# 1888
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
if((!Cyc_Unify_unify(aq_a,aq_b)&& !Cyc_Tcutil_alias_qual_subtype(aq_a,aq_b))&& !
Cyc_Tcutil_subtype(po,assume,aq_a,aq_b))
return 0;
# 1895
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*e1=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_a);});
struct Cyc_Absyn_Exp*e2=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_b);});
if(e1!=e2){
if((e1==0 || e2==0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1904
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1909
int deep_subtype=({void*_Tmp13=b_b;Cyc_Unify_unify(_Tmp13,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 1912
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_Tmp13=po;struct Cyc_List_List*_Tmp14=({struct Cyc_List_List*_Tmp15=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp16=({struct _tuple15*_Tmp17=_cycalloc(sizeof(struct _tuple15));_Tmp17->f1=t1,_Tmp17->f2=t2;_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=assume;_Tmp15;});void*_Tmp15=t_a;Cyc_Tcutil_ptrsubtype(_Tmp13,_Tmp14,_Tmp15,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==24){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.tag==2){if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==23){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1.KnownDatatype.tag==2){_Tmp12=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.val.f1;_Tmp10=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.val.f2;_TmpF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_TmpE=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1.KnownDatatype.val;_TmpD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_Tmp12;struct Cyc_Absyn_Datatypefield*df=_Tmp10;struct Cyc_List_List*ts1=_TmpF;struct Cyc_Absyn_Datatypedecl*dd2=_TmpE;struct Cyc_List_List*ts2=_TmpD;
# 1919
if(dd1!=dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!=0)return 0;
# 1921
if(({int _Tmp13=Cyc_List_length(ts1);_Tmp13!=Cyc_List_length(ts2);}))return 0;
for(1;ts1!=0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)_check_null(ts2)->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5: if(*((int*)_Tmp0.f2)==5){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_FnInfo f1=_Tmp2;struct Cyc_Absyn_FnInfo f2=_Tmp1;
# 1927
if(f1.tvars!=0 || f2.tvars!=0){
struct Cyc_List_List*tvs1=f1.tvars;
struct Cyc_List_List*tvs2=f2.tvars;
if(({int _Tmp13=Cyc_List_length(tvs1);_Tmp13!=Cyc_List_length(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1!=0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)_check_null(tvs2)->hd)->kind))return 0;
inst=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp14=({struct _tuple17*_Tmp15=_cycalloc(sizeof(struct _tuple17));_Tmp15->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_Tmp16=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_Tmp15->f2=_Tmp16;});_Tmp15;});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=inst;_Tmp13;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 1938
if(inst!=0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_Tmp13=po;struct Cyc_List_List*_Tmp14=assume;void*_Tmp15=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp16=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp16->tag=5,_Tmp16->f1=f1;_Tmp16;});Cyc_Tcutil_subtype(_Tmp13,_Tmp14,_Tmp15,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp16=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp16->tag=5,_Tmp16->f1=f2;_Tmp16;}));});}}}
# 1945
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*args1=f1.args;
struct Cyc_List_List*args2=f2.args;
# 1950
if(({int _Tmp13=Cyc_List_length(args1);_Tmp13!=Cyc_List_length(args2);}))return 0;
# 1952
for(1;args1!=0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _Tmp13=*((struct _tuple9*)args1->hd);void*_Tmp14;struct Cyc_Absyn_Tqual _Tmp15;_Tmp15=_Tmp13.f2;_Tmp14=_Tmp13.f3;{struct Cyc_Absyn_Tqual tq1=_Tmp15;void*t1=_Tmp14;
struct _tuple9 _Tmp16=*((struct _tuple9*)args2->hd);void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;_Tmp18=_Tmp16.f2;_Tmp17=_Tmp16.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp18;void*t2=_Tmp17;
# 1956
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 1960
if(f1.c_varargs!=f2.c_varargs)return 0;
if(f1.cyc_varargs!=0 && f2.cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo v1=*f1.cyc_varargs;
struct Cyc_Absyn_VarargInfo v2=*f2.cyc_varargs;
# 1965
if(v2.tq.real_const && !v1.tq.real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs!=0 || f2.cyc_varargs!=0)return 0;}
# 1970
if(!({void*_Tmp13=_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_Tmp13,_check_null(f2.effect));}))return 0;
# 1972
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 1974
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 1976
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 1978
return 0;
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 1981
return 0;
# 1983
return 1;}}}else{goto _LL7;}default: _LL7:
 return 0;};}}
# 1994
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1997
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(1;tqs2!=0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1==0)return 0;{
struct _tuple12*_Tmp0=(struct _tuple12*)tqs1->hd;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_Absyn_Tqual tq1=_Tmp2;void*t1a=_Tmp1;
struct _tuple12*_Tmp3=(struct _tuple12*)tqs2->hd;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp3->f1;_Tmp4=_Tmp3->f2;{struct Cyc_Absyn_Tqual tq2=_Tmp5;void*t2a=_Tmp4;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2011
return 1;}
# 2016
int Cyc_Tcutil_check_aqual_bounds(struct Cyc_List_List*aquals_bnd,void*aq,void*bnd){
# 2018
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(bnd);_Tmp1.f2=_Tmp2;});_Tmp1;});void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;enum Cyc_Absyn_AliasQualVal _Tmp3;if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==17){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp2;
# 2020
return(int)v_sup==(int)v_sub ||(int)v_sup==3;}}else{goto _LL7;}}else{goto _LL7;}case 18: if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==17){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{struct Cyc_List_List*tv_sub=_Tmp1;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 2022
return Cyc_Tcutil_check_aqual_bounds(aquals_bnd,(void*)_check_null(_check_null(tv_sub)->tl)->hd,bnd);}}else{goto _LL7;}}else{goto _LL7;}case 16: if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==17){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 2024
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*aq_bound=Cyc_List_assoc_cmp(Cyc_Tcutil_typecmp,aquals_bnd,aq);
int _Tmp6=Cyc_Tcutil_check_aqual_bounds(aquals_bnd,aq_bound,bnd);_npop_handler(0);return _Tmp6;}
# 2025
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found)
# 2029
return 0;else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}};}}}}else{goto _LL7;}}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 2032
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("check_aqual_bounds expects a constant bound; got ",sizeof(char),50U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aq;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(",",sizeof(char),2U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)bnd;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp8,sizeof(void*),4));});};}
# 2037
void*Cyc_Tcutil_lookup_aquals(struct Cyc_List_List*aquals_map,void*t){
void*aqt=Cyc_Absyn_aqualsof_type(t);
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{void*_Tmp2=Cyc_List_assoc_cmp(Cyc_Tcutil_typecmp,aquals_map,aqt);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 2043
return Cyc_Absyn_al_qual_type;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 2050
int Cyc_Tcutil_alias_qual_subtype(void*aq_sub,void*aq_sup){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq_sub);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(aq_sup);_Tmp1.f2=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_AliasQualVal _Tmp5;enum Cyc_Absyn_AliasQualVal _Tmp6;if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(*((int*)_Tmp0.f2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)){case 17: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp5=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp5;
# 2053
return((int)v_sup==(int)v_sub ||(int)v_sup==3)||
(int)v_sup==0 &&((int)v_sub==1 ||(int)v_sub==2);}case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)){case 2: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 2075
return 0;}case 1: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->tl;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;struct Cyc_Core_Opt*k=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;
# 2077
return Cyc_Kinds_kind_eq((struct Cyc_Absyn_Kind*)_check_null(k)->v,& Cyc_Kinds_aqk)&& Cyc_Tcutil_alias_qual_subtype(aq_sub,(void*)_check_null(bnd)->hd);}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{goto _LL15;}case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 2: if(*((int*)_Tmp0.f2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)){case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)){case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_Absyn_Tvar*tv2=_Tmp3;
# 2056
return tv2->identity==tv1->identity;}case 1: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->tl;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 2061
return({struct Cyc_Absyn_Kind*_Tmp7=Cyc_Kinds_tvar_kind(tv1,& Cyc_Kinds_aqk);Cyc_Kinds_kind_eq(_Tmp7,(struct Cyc_Absyn_Kind*)_check_null(k)->v);})&&({void*_Tmp7=(void*)_check_null(bnd1)->hd;Cyc_Tcutil_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd);});}default: goto _LL15;}else{goto _LL15;}case 17: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 2070
return Cyc_Tcutil_alias_qual_subtype((void*)_check_null(bnd)->hd,aq_sup);}default: goto _LL15;}else{goto _LL15;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f1)==16){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2)->tl==0){if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==18){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)->f1)==16){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)->f2)->tl==0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f2->hd;_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f2->hd;{void*tv1=_Tmp4;void*tv2=_Tmp3;
# 2059
return Cyc_Tcutil_typecmp(tv1,tv2)==0;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}case 1: if(*((int*)_Tmp0.f2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)){case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2)->hd)){case 2: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;_Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Absyn_Tvar*tv2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 2063
return({struct Cyc_Absyn_Kind*_Tmp7=(struct Cyc_Absyn_Kind*)_check_null(k1)->v;Cyc_Kinds_kind_eq(_Tmp7,Cyc_Kinds_tvar_kind(tv2,& Cyc_Kinds_aqk));})&&({void*_Tmp7=(void*)_check_null(bnd2)->hd;Cyc_Tcutil_check_aqual_bounds(0,_Tmp7,(void*)_check_null(bnd1)->hd);});}case 1: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 2065
return({struct Cyc_Absyn_Kind*_Tmp7=(struct Cyc_Absyn_Kind*)_check_null(k1)->v;Cyc_Kinds_kind_eq(_Tmp7,(struct Cyc_Absyn_Kind*)_check_null(k2)->v);})&&({void*_Tmp7=(void*)_check_null(bnd1)->hd;Cyc_Tcutil_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd);});}default: goto _LL15;}else{goto _LL15;}case 17: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1;{struct Cyc_Core_Opt*k=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 2072
return Cyc_Kinds_kind_eq((struct Cyc_Absyn_Kind*)_check_null(k)->v,& Cyc_Kinds_aqk)&& Cyc_Tcutil_check_aqual_bounds(0,aq_sup,(void*)_check_null(bnd)->hd);}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{_LL15:
# 2079
 return 0;};}
# 2085
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return 1U;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(*((int*)t2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)){case 0:
 return 1U;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2){case Cyc_Absyn_Int_sz:
# 2093
 goto _LL6;case Cyc_Absyn_Long_sz: _LL6:
# 2095
 if((int)Cyc_Tcutil_type_kind(t1)->kind==2)return 3U;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;{
# 2099
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;switch(*((int*)t1)){case 3: _Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.rgn;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.aqual;{void*t_a=_Tmp7;struct Cyc_Absyn_Tqual q_a=_Tmp6;void*rt_a=_Tmp5;void*null_a=_Tmp4;void*b_a=_Tmp3;void*zt_a=_Tmp2;void*rel_a=_Tmp1;void*aq_a=_Tmp0;
# 2110 "tcutil.cyc"
{void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;struct Cyc_Absyn_Tqual _TmpE;void*_TmpF;if(*((int*)t2)==3){_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.elt_type;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.elt_tq;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.rgn;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.nullable;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.bounds;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.zero_term;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.autoreleased;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t2)->f1.ptr_atts.aqual;{void*t_b=_TmpF;struct Cyc_Absyn_Tqual q_b=_TmpE;void*rt_b=_TmpD;void*null_b=_TmpC;void*b_b=_TmpB;void*zt_b=_TmpA;void*rel_b=_Tmp9;void*aq_b=_Tmp8;
# 2112
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*assump;assump=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple15*_Tmp10=({struct _tuple15*_Tmp11=_cycalloc(sizeof(struct _tuple15));_Tmp11->f1=t1,_Tmp11->f2=t2;_Tmp11;});assump->hd=_Tmp10;}),assump->tl=0;{
int quals_okay=q_b.real_const || !q_a.real_const;
# 2125 "tcutil.cyc"
int deep_castable=
({void*_Tmp10=b_b;Cyc_Unify_unify(_Tmp10,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2128
int ptrsub=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
int autoreleased_ok=Cyc_Unify_unify(rel_a,rel_b)|| !Cyc_Absyn_type2bool(0,rel_b);
# 2134
int bitcase=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*e_a=({void*_Tmp10=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp10,b_a);});
struct Cyc_Absyn_Exp*e_b=({void*_Tmp10=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp10,b_b);});
if((e_a!=0 && e_b!=0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a==0 || e_b==0)
bounds_ok=1;}}{
# 2146
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2151
if(((((bounds_ok && zeroterm_ok)&& autoreleased_ok)&&(
ptrsub || bitcase))&& Cyc_Tcutil_alias_qual_subtype(aq_a,aq_b))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return 0U;}}}}}else{
goto _LL18;}_LL18:;}
# 2158
return 0U;}case 4: _Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t1)->f1.tq;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t1)->f1.num_elts;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t1)->f1.zero_term;{void*t1a=_Tmp7;struct Cyc_Absyn_Tqual tq1a=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp5;void*zt1=_Tmp4;
# 2160
{void*_Tmp8;void*_Tmp9;struct Cyc_Absyn_Tqual _TmpA;void*_TmpB;if(*((int*)t2)==4){_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t2)->f1.elt_type;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t2)->f1.tq;_Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t2)->f1.num_elts;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t2)->f1.zero_term;{void*t2a=_TmpB;struct Cyc_Absyn_Tqual tq2a=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;void*zt2=_Tmp8;
# 2162
int okay=
(((e1!=0 && e2!=0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2167
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?1U: 0U;}}else{
# 2169
return 0U;};}
# 2171
return 0U;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)){case 20: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed1=_Tmp7;
# 2175
{void*_Tmp8;if(*((int*)t2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)==20){_Tmp8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed2=_Tmp8;
# 2177
if((_check_null(ed1)->fields!=0 && _check_null(ed2)->fields!=0)&&({
int _Tmp9=Cyc_List_length((struct Cyc_List_List*)_check_null(ed1->fields)->v);_Tmp9 >= Cyc_List_length((struct Cyc_List_List*)_check_null(ed2->fields)->v);}))
return 3U;
goto _LL22;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}
# 2183
goto _LL11;}case 1: _LL11:
 goto _LL13;case 2: _LL13:
# 2186
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?3U: 0U;case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f2!=0){_Tmp7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f2->hd;{void*r1=_Tmp7;
# 2189
{void*_Tmp8;if(*((int*)t2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f2!=0){_Tmp8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f2->hd;{void*r2=_Tmp8;
# 2191
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return 1U;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A:
 goto _LL27;}_LL27:;}
# 2195
return 0U;}}else{goto _LL16;}default: goto _LL16;}default: _LL16:
 return 0U;};}}
# 2201
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify(_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);
({void*_Tmp0=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp1->tag=14,_Tmp1->f1=t,_Tmp1->f2=inner,_Tmp1->f3=0,_Tmp1->f4=c;_Tmp1;});e->r=_Tmp0;});
e->topt=t;}}
# 2210
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2215
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("#%d",sizeof(char),4U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k;_Tmp0;});}
# 2219
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript(t->name[0],sizeof(char),0U))==35;}
# 2224
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type==0){
# 2231
struct Cyc_List_List*fn_type_atts=0;
{struct Cyc_List_List*atts=fd->i.attributes;for(0;atts!=0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)atts->hd,_Tmp0->tl=fn_type_atts;_Tmp0;});}}{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp0->tag=5,_Tmp0->f1=type_info;_Tmp0;});}}
# 2239
return _check_null(fd->cached_type);}
# 2245
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2247
union Cyc_Relations_RelnOp _Tmp0=*rop;void*_Tmp1;switch(_Tmp0.RNumelts.tag){case 2: _Tmp1=_Tmp0.RVar.val;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 2249
struct _tuple1 _Tmp2=*vd->name;void*_Tmp3;union Cyc_Absyn_Nmspace _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{union Cyc_Absyn_Nmspace nmspace=_Tmp4;struct _fat_ptr*var=_Tmp3;
if(!((int)(nmspace.Loc_n.tag==4)))goto _LL0;
if(Cyc_strcmp(*var,_tag_fat("return_value",sizeof(char),13U))==0){
({union Cyc_Relations_RelnOp _Tmp5=Cyc_Relations_RReturn();*rop=_Tmp5;});
goto _LL0;}{
# 2255
unsigned c=0U;
{struct Cyc_List_List*a=args;for(0;a!=0;(a=a->tl,c ++)){
struct _tuple9*_Tmp5=(struct _tuple9*)a->hd;void*_Tmp6;_Tmp6=_Tmp5->f1;{struct _fat_ptr*vopt=_Tmp6;
if(vopt!=0 && Cyc_strcmp(*var,*vopt)==0){
({union Cyc_Relations_RelnOp _Tmp7=Cyc_Relations_RParam(c);*rop=_Tmp7;});
break;}}}}
# 2263
goto _LL0;}}}case 3: _Tmp1=_Tmp0.RNumelts.val;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 2265
struct _tuple1 _Tmp2=*vd->name;void*_Tmp3;union Cyc_Absyn_Nmspace _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{union Cyc_Absyn_Nmspace nmspace=_Tmp4;struct _fat_ptr*var=_Tmp3;
if(!((int)(nmspace.Loc_n.tag==4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*a=args;for(0;a!=0;(a=a->tl,c ++)){
struct _tuple9*_Tmp5=(struct _tuple9*)a->hd;void*_Tmp6;_Tmp6=_Tmp5->f1;{struct _fat_ptr*vopt=_Tmp6;
if(vopt!=0 && Cyc_strcmp(*var,*vopt)==0){
({union Cyc_Relations_RelnOp _Tmp7=Cyc_Relations_RParamNumelts(c);*rop=_Tmp7;});
break;}}}}
# 2275
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 2280
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2282
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2286
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2288
if(e==0)return 0;{
struct Cyc_List_List*relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2295
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=pr->f1;_Tmp0=pr->f2;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t2=_Tmp0;
return t2==t?pr:({struct _tuple12*_Tmp2=_cycalloc(sizeof(struct _tuple12));_Tmp2->f1=tq,_Tmp2->f2=t;_Tmp2;});}}struct _tuple24{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple25{struct _tuple24*f1;void*f2;};
# 2299
static struct _tuple25*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2301
return({struct _tuple25*_Tmp0=_region_malloc(rgn,0U,sizeof(struct _tuple25));({struct _tuple24*_Tmp1=({struct _tuple24*_Tmp2=_region_malloc(rgn,0U,sizeof(struct _tuple24));_Tmp2->f1=(*y).f1,_Tmp2->f2=(*y).f2;_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=(*y).f3;_Tmp0;});}
# 2303
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2305
struct _tuple9 _Tmp0=*orig_arg;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct _fat_ptr*vopt_orig=_Tmp3;struct Cyc_Absyn_Tqual tq_orig=_Tmp2;void*t_orig=_Tmp1;
if(t==t_orig)return orig_arg;
return({struct _tuple9*_Tmp4=_cycalloc(sizeof(struct _tuple9));_Tmp4->f1=vopt_orig,_Tmp4->f2=tq_orig,_Tmp4->f3=t;_Tmp4;});}}
# 2309
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2313
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2315
return({struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=old->topt,_Tmp0->r=r,_Tmp0->loc=old->loc,_Tmp0->annot=old->annot;_Tmp0;});}
# 2320
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Coercion _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;enum Cyc_Absyn_Primop _Tmp6;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 1: _LLA:
 return e;case 3: _Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp6;struct Cyc_List_List*es=_Tmp5;
# 2329
if(Cyc_List_length(es)==1){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
return new_e1==e1?e:({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct));_Tmp8->tag=3,_Tmp8->f1=p,({struct Cyc_List_List*_Tmp9=({struct Cyc_Absyn_Exp*_TmpA[1];_TmpA[0]=new_e1;Cyc_List_list(_tag_fat(_TmpA,sizeof(struct Cyc_Absyn_Exp*),1));});_Tmp8->f2=_Tmp9;});_Tmp8;}));});}else{
if(Cyc_List_length(es)==2){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd;
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
struct Cyc_Absyn_Exp*new_e2=Cyc_Tcutil_rsubsexp(r,inst,e2);
if(new_e1==e1 && new_e2==e2)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct));_Tmp8->tag=3,_Tmp8->f1=p,({struct Cyc_List_List*_Tmp9=({struct Cyc_Absyn_Exp*_TmpA[2];_TmpA[0]=new_e1,_TmpA[1]=new_e2;Cyc_List_list(_tag_fat(_TmpA,sizeof(struct Cyc_Absyn_Exp*),2));});_Tmp8->f2=_Tmp9;});_Tmp8;}));});}}
# 2341
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("primop does not have 1 or 2 args!",sizeof(char),34U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp8,sizeof(void*),1));});}case 6: _Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 2343
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
struct Cyc_Absyn_Exp*new_e2=Cyc_Tcutil_rsubsexp(r,inst,e2);
struct Cyc_Absyn_Exp*new_e3=Cyc_Tcutil_rsubsexp(r,inst,e3);
if((new_e1==e1 && new_e2==e2)&& new_e3==e3)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct));_Tmp8->tag=6,_Tmp8->f1=new_e1,_Tmp8->f2=new_e2,_Tmp8->f3=new_e3;_Tmp8;}));});}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 2349
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
struct Cyc_Absyn_Exp*new_e2=Cyc_Tcutil_rsubsexp(r,inst,e2);
if(new_e1==e1 && new_e2==e2)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct));_Tmp8->tag=7,_Tmp8->f1=new_e1,_Tmp8->f2=new_e2;_Tmp8;}));});}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 2354
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
struct Cyc_Absyn_Exp*new_e2=Cyc_Tcutil_rsubsexp(r,inst,e2);
if(new_e1==e1 && new_e2==e2)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct));_Tmp8->tag=8,_Tmp8->f1=new_e1,_Tmp8->f2=new_e2;_Tmp8;}));});}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 2359
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
struct Cyc_Absyn_Exp*new_e2=Cyc_Tcutil_rsubsexp(r,inst,e2);
if(new_e1==e1 && new_e2==e2)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct));_Tmp8->tag=9,_Tmp8->f1=new_e1,_Tmp8->f2=new_e2;_Tmp8;}));});}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;int b=_Tmp2;enum Cyc_Absyn_Coercion c=_Tmp1;
# 2364
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
void*new_typ=Cyc_Tcutil_rsubstitute(r,inst,t);
if(new_e1==e1 && new_typ==t)return e;
return({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp8->tag=14,_Tmp8->f1=new_typ,_Tmp8->f2=new_e1,_Tmp8->f3=b,_Tmp8->f4=c;_Tmp8;}));});}case 17: _Tmp5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp5;
# 2369
void*new_typ=Cyc_Tcutil_rsubstitute(r,inst,t);
return new_typ==t?e:({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_Tmp8->tag=17,_Tmp8->f1=new_typ;_Tmp8;}));});}case 18: _Tmp5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 2372
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
return new_e1==e1?e:({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct));_Tmp8->tag=18,_Tmp8->f1=new_e1;_Tmp8;}));});}case 41: _Tmp5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 2375
struct Cyc_Absyn_Exp*new_e1=Cyc_Tcutil_rsubsexp(r,inst,e1);
return new_e1==e1?e:({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct));_Tmp8->tag=41,_Tmp8->f1=new_e1;_Tmp8;}));});}case 19: _Tmp5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp5;struct Cyc_List_List*f=_Tmp4;
# 2378
void*new_typ=Cyc_Tcutil_rsubstitute(r,inst,t);
return new_typ==t?e:({struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_copye(_Tmp7,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct));_Tmp8->tag=19,_Tmp8->f1=new_typ,_Tmp8->f2=f;_Tmp8;}));});}case 39: _Tmp5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp5;
# 2381
void*new_typ=Cyc_Tcutil_rsubstitute(r,inst,t);
if(new_typ==t)return e;{
# 2384
void*_Tmp7=Cyc_Absyn_compress(new_typ);void*_Tmp8;if(*((int*)_Tmp7)==9){_Tmp8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
return e;}}else{
return({struct Cyc_Absyn_Exp*_Tmp9=e;Cyc_Tcutil_copye(_Tmp9,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_TmpA->tag=39,_TmpA->f1=new_typ;_TmpA;}));});};}}default:
# 2388
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("non-type-level-expression in Tcutil::rsubsexp",sizeof(char),46U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp8,sizeof(void*),1));});};}
# 2392
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2395
return e==0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2398
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2401
void*t=f->type;
struct Cyc_Absyn_Exp*req=f->requires_clause;
void*new_typ=Cyc_Tcutil_rsubstitute(r,inst,t);
struct Cyc_Absyn_Exp*new_req=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);
if(t==new_typ && req==new_req)
return f;{
struct Cyc_Absyn_Aggrfield*ans;ans=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),*ans=*f;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2413
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2416
if(fs==0)return 0;{
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*t=fs->tl;
struct Cyc_Absyn_Aggrfield*new_f=Cyc_Tcutil_subst_aggrfield(r,inst,f);
struct Cyc_List_List*new_typ=Cyc_Tcutil_subst_aggrfields(r,inst,t);
return new_f==f && new_typ==t?fs:({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=new_f,_Tmp0->tl=new_typ;_Tmp0;});}}
# 2424
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2427
struct _tuple0 _Tmp0=Cyc_List_rsplit(rgn,rgn,rgn_po);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*rpo1a=_Tmp2;struct Cyc_List_List*rpo1b=_Tmp1;
struct Cyc_List_List*rpo2a=Cyc_Tcutil_substs(rgn,inst,rpo1a);
struct Cyc_List_List*rpo2b=Cyc_Tcutil_substs(rgn,inst,rpo1b);
return rpo2a==rpo1a && rpo2b==rpo1b?rgn_po: Cyc_List_zip(rpo2a,rpo2b);}}
# 2435
static int Cyc_Tcutil_skip_fntypes=0;
# 2437
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2440
void*_Tmp0=Cyc_Absyn_compress(t);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;int _Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;unsigned _TmpB;struct Cyc_Absyn_Tqual _TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;switch(*((int*)_Tmp0)){case 2: _Tmp10=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*v=_Tmp10;
# 2443
struct _handler_cons _Tmp11;_push_handler(& _Tmp11);{int _Tmp12=0;if(setjmp(_Tmp11.handler))_Tmp12=1;if(!_Tmp12){{void*_Tmp13=({(void*(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_assoc_cmp;})(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0);return _Tmp13;};_pop_handler();}else{void*_Tmp13=(void*)Cyc_Core_get_exn_thrown();void*_Tmp14;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp13)->tag==Cyc_Core_Not_found)
return t;else{_Tmp14=_Tmp13;{void*exn=_Tmp14;_rethrow(exn);}};}}}case 8: _Tmp10=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_TmpF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_TmpE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;_TmpD=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;{struct _tuple1*n=_Tmp10;struct Cyc_List_List*ts=_TmpF;struct Cyc_Absyn_Typedefdecl*td=_TmpE;void*topt=_TmpD;
# 2446
struct Cyc_List_List*new_ts=Cyc_Tcutil_substs(rgn,inst,ts);
return new_ts==ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_Tmp11->tag=8,_Tmp11->f1=n,_Tmp11->f2=new_ts,_Tmp11->f3=td,_Tmp11->f4=topt;_Tmp11;});}case 4: _Tmp10=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;_TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zt_loc;{void*t1=_Tmp10;struct Cyc_Absyn_Tqual tq=_TmpC;struct Cyc_Absyn_Exp*e=_TmpF;void*zt=_TmpE;unsigned ztl=_TmpB;
# 2449
void*new_t1=Cyc_Tcutil_rsubstitute(rgn,inst,t1);
struct Cyc_Absyn_Exp*new_e=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);
void*new_zt=Cyc_Tcutil_rsubstitute(rgn,inst,zt);
return(new_t1==t1 && new_e==e)&& new_zt==zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));
_Tmp11->tag=4,_Tmp11->f1.elt_type=new_t1,_Tmp11->f1.tq=tq,_Tmp11->f1.num_elts=new_e,_Tmp11->f1.zero_term=new_zt,_Tmp11->f1.zt_loc=ztl;_Tmp11;});}case 3: _Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.autoreleased;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.aqual;{void*t1=_Tmp10;struct Cyc_Absyn_Tqual tq=_TmpC;void*r=_TmpF;void*n=_TmpE;void*b=_TmpD;void*zt=_TmpA;void*rel=_Tmp9;void*aq=_Tmp8;
# 2455
void*new_t1=Cyc_Tcutil_rsubstitute(rgn,inst,t1);
void*new_r=Cyc_Tcutil_rsubstitute(rgn,inst,r);
void*new_b=Cyc_Tcutil_rsubstitute(rgn,inst,b);
void*new_zt=Cyc_Tcutil_rsubstitute(rgn,inst,zt);
void*new_rel=Cyc_Tcutil_rsubstitute(rgn,inst,rel);
void*new_aq=Cyc_Tcutil_rsubstitute(rgn,inst,aq);
if(((((new_t1==t1 && new_r==r)&& new_b==b)&& new_zt==zt)&& new_rel==rel)&& new_aq==aq)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp11;_Tmp11.elt_type=new_t1,_Tmp11.elt_tq=tq,_Tmp11.ptr_atts.rgn=new_r,_Tmp11.ptr_atts.nullable=n,_Tmp11.ptr_atts.bounds=new_b,_Tmp11.ptr_atts.zero_term=new_zt,_Tmp11.ptr_atts.ptrloc=0,_Tmp11.ptr_atts.autoreleased=new_rel,_Tmp11.ptr_atts.aqual=new_aq;_Tmp11;}));}case 5: _Tmp10=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;_TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.effect;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_tqual;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ret_type;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.args;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.c_varargs;_TmpA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.cyc_varargs;_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.rgn_po;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.qual_bnd;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.requires_clause;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.ensures_clause;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.return_value;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.arg_vardecls;{struct Cyc_List_List*vs=_Tmp10;void*eff=_TmpF;struct Cyc_Absyn_Tqual rtq=_TmpC;void*rtyp=_TmpE;struct Cyc_List_List*args=_TmpD;int c_varargs=_Tmp7;struct Cyc_Absyn_VarargInfo*cyc_varargs=_TmpA;struct Cyc_List_List*rgn_po=_Tmp9;struct Cyc_List_List*qb=_Tmp8;struct Cyc_List_List*atts=_Tmp6;struct Cyc_Absyn_Exp*req=_Tmp5;struct Cyc_Absyn_Exp*ens=_Tmp4;struct Cyc_Absyn_Vardecl*ret_value=_Tmp3;struct Cyc_List_List*argvds=_Tmp2;
# 2466
if(Cyc_Tcutil_skip_fntypes)return t;
# 2468
{struct Cyc_List_List*p=vs;for(0;p!=0;p=p->tl){
inst=({struct Cyc_List_List*_Tmp11=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp12=({struct _tuple17*_Tmp13=_region_malloc(rgn,0U,sizeof(struct _tuple17));_Tmp13->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_Tmp14=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_Tmp13->f2=_Tmp14;});_Tmp13;});_Tmp11->hd=_Tmp12;}),_Tmp11->tl=inst;_Tmp11;});}}{
struct _tuple0 _Tmp11=({struct _RegionHandle*_Tmp12=rgn;struct _RegionHandle*_Tmp13=rgn;Cyc_List_rsplit(_Tmp12,_Tmp13,
({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;})(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2470
void*_Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f1;_Tmp12=_Tmp11.f2;{struct Cyc_List_List*qs=_Tmp13;struct Cyc_List_List*ts=_Tmp12;
# 2472
struct Cyc_List_List*args2=args;
struct Cyc_List_List*ts2=Cyc_Tcutil_substs(rgn,inst,ts);
if(ts2!=ts)
args2=({(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2;})(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff==0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2478
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs==0)
cyc_varargs2=0;else{
# 2482
struct Cyc_Absyn_VarargInfo _Tmp14=*cyc_varargs;int _Tmp15;void*_Tmp16;struct Cyc_Absyn_Tqual _Tmp17;void*_Tmp18;_Tmp18=_Tmp14.name;_Tmp17=_Tmp14.tq;_Tmp16=_Tmp14.type;_Tmp15=_Tmp14.inject;{struct _fat_ptr*n=_Tmp18;struct Cyc_Absyn_Tqual tq=_Tmp17;void*t=_Tmp16;int i=_Tmp15;
void*t2=Cyc_Tcutil_rsubstitute(rgn,inst,t);
cyc_varargs2=t2==t?cyc_varargs:({struct Cyc_Absyn_VarargInfo*_Tmp19=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_Tmp19->name=n,_Tmp19->tq=tq,_Tmp19->type=t2,_Tmp19->inject=i;_Tmp19;});}}{
# 2486
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_List_List*qb2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,qb);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*req_relns2=Cyc_Tcutil_extract_relns(args2,req2);
struct Cyc_List_List*ens_relns2=Cyc_Tcutil_extract_relns(args2,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp14=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp14->tag=5,_Tmp14->f1.tvars=vs,_Tmp14->f1.effect=eff2,_Tmp14->f1.ret_tqual=rtq,({void*_Tmp15=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);_Tmp14->f1.ret_type=_Tmp15;}),_Tmp14->f1.args=args2,_Tmp14->f1.c_varargs=c_varargs,_Tmp14->f1.cyc_varargs=cyc_varargs2,_Tmp14->f1.rgn_po=rgn_po2,_Tmp14->f1.qual_bnd=qb2,_Tmp14->f1.attributes=atts,_Tmp14->f1.requires_clause=req2,_Tmp14->f1.requires_relns=req_relns2,_Tmp14->f1.ensures_clause=ens2,_Tmp14->f1.ensures_relns=ens_relns2,_Tmp14->f1.return_value=ret_value,_Tmp14->f1.arg_vardecls=argvds;_Tmp14;});}}}}}case 6: _Tmp10=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqts=_Tmp10;
# 2496
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*ts1=tqts;for(0;ts1!=0;ts1=ts1->tl){
void*t1=(*((struct _tuple12*)ts1->hd)).f2;
void*t2=Cyc_Tcutil_rsubstitute(rgn,inst,t1);
if(t1!=t2)
change=1;
ts2=({struct Cyc_List_List*_Tmp11=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp11->hd=t2,_Tmp11->tl=ts2;_Tmp11;});}}
# 2505
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp11->tag=6,({struct Cyc_List_List*_Tmp12=({struct Cyc_List_List*_Tmp13=tqts;({(struct Cyc_List_List*(*)(struct _tuple12*(*)(struct _tuple12*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map2;})(Cyc_Tcutil_map2_tq,_Tmp13,Cyc_List_imp_rev(ts2));});_Tmp11->f1=_Tmp12;});_Tmp11;});}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp10=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp10;
# 2507
struct Cyc_List_List*new_fs=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);
return fs==new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp11->tag=7,_Tmp11->f1=k,_Tmp11->f2=new_fs;_Tmp11;});}case 1: _Tmp10=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;{void*r=_Tmp10;
# 2510
return r==0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0: _Tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_TmpF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp10;struct Cyc_List_List*ts=_TmpF;
# 2515
struct Cyc_List_List*new_ts=Cyc_Tcutil_substs(rgn,inst,ts);
return ts==new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp11->tag=0,_Tmp11->f1=c,_Tmp11->f2=new_ts;_Tmp11;});}case 9: _Tmp10=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp10;
# 2518
struct Cyc_Absyn_Exp*new_e=Cyc_Tcutil_rsubsexp(rgn,inst,e);
return new_e==e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp11->tag=9,_Tmp11->f1=new_e;_Tmp11;});}case 11: _Tmp10=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp10;
# 2521
struct Cyc_Absyn_Exp*new_e=Cyc_Tcutil_rsubsexp(rgn,inst,e);
return new_e==e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_TypeofType_Absyn_Type_struct));_Tmp11->tag=11,_Tmp11->f1=new_e;_Tmp11;});}default:
# 2524
 if(Cyc_Tcutil_skip_fntypes)return t;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=_tag_fat("found typedecltype in rsubs",sizeof(char),28U);_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp12,sizeof(void*),1));});};}
# 2529
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2532
if(ts==0)
return 0;{
void*old_hd=(void*)ts->hd;
struct Cyc_List_List*old_tl=ts->tl;
void*new_hd=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);
struct Cyc_List_List*new_tl=Cyc_Tcutil_substs(rgn,inst,old_tl);
if(old_hd==new_hd && old_tl==new_tl)
return ts;
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=new_hd,_Tmp0->tl=new_tl;_Tmp0;});}}
# 2543
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst==0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2547
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*new_typ=Cyc_Tcutil_substitute(inst,t);
Cyc_Tcutil_skip_fntypes=0;
return new_typ;}}
# 2555
struct _tuple17*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*k=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));
return({struct _tuple17*_Tmp0=_cycalloc(sizeof(struct _tuple17));_Tmp0->f1=tv,({void*_Tmp1=({struct Cyc_Core_Opt*_Tmp2=k;Cyc_Absyn_new_evar(_Tmp2,({struct Cyc_Core_Opt*_Tmp3=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp3->v=s;_Tmp3;}));});_Tmp0->f2=_Tmp1;});_Tmp0;});}struct _tuple26{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2560
struct _tuple17*Cyc_Tcutil_r_make_inst_var(struct _tuple26*env,struct Cyc_Absyn_Tvar*tv){
# 2562
void*_Tmp0;void*_Tmp1;_Tmp1=env->f1;_Tmp0=env->f2;{struct Cyc_List_List*s=_Tmp1;struct _RegionHandle*rgn=_Tmp0;
struct Cyc_Core_Opt*k=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));
return({struct _tuple17*_Tmp2=_region_malloc(rgn,0U,sizeof(struct _tuple17));_Tmp2->f1=tv,({void*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=k;Cyc_Absyn_new_evar(_Tmp4,({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=s;_Tmp5;}));});_Tmp2->f2=_Tmp3;});_Tmp2;});}}
# 2567
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2569
if(width!=0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bitfield ",sizeof(char),10U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*fn;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" does not have constant width",sizeof(char),30U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});else{
# 2574
struct _tuple13 _Tmp0=Cyc_Evexp_eval_const_uint_exp(width);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned i=_Tmp2;int known=_Tmp1;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("cannot evaluate bitfield width at compile time",sizeof(char),47U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_warn2(loc,_tag_fat(_Tmp4,sizeof(void*),1));});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("bitfield has negative width",sizeof(char),28U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),1));});
w=i;}}{
# 2581
void*_Tmp0=Cyc_Absyn_compress(field_type);enum Cyc_Absyn_Size_of _Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==1){_Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2;{enum Cyc_Absyn_Size_of b=_Tmp1;
# 2584
int bad;
switch((int)b){case Cyc_Absyn_Char_sz:
 bad=w > 8U;goto _LL8;case Cyc_Absyn_Short_sz:
 bad=w > 16U;goto _LL8;case Cyc_Absyn_Long_sz:
 goto _LL10;case Cyc_Absyn_Int_sz: _LL10:
 bad=w > 32U;goto _LL8;case Cyc_Absyn_LongLong_sz:
 goto _LL14;default: _LL14:
 bad=w > 64U;goto _LL8;}_LL8:;
# 2593
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("bitfield larger than type",sizeof(char),26U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});
goto _LL3;}}else{goto _LL6;}}else{_LL6:
# 2597
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("bitfield ",sizeof(char),10U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*fn;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" must have integral type but has type ",sizeof(char),39U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)field_type;_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),4));});
# 2599
goto _LL3;}_LL3:;}}}
# 2606
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_Tmp0;void*_Tmp1;if(*((int*)t)==8){_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;void*tdopt=_Tmp0;
# 2609
if(_check_null(td)->tq.real_const || td->tq.print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("extra const",sizeof(char),12U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});
return 1;}
# 2614
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{
return declared_const;};}
# 2621
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity==-1)
({int _Tmp0=Cyc_Tcutil_new_tvar_id();tv->identity=_Tmp0;});}
# 2625
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2631
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2634
for(1;vs!=0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2!=0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(": ",sizeof(char),3U);_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=a2string(vs->hd);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});}}}
# 2639
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2642
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
({(void(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct _fat_ptr*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted;})(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2645
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({(void(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,unsigned,struct _fat_ptr(*)(struct Cyc_Absyn_Tvar*),struct _fat_ptr))Cyc_Tcutil_check_unique_unsorted;})(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Absynpp_tvar2string,
_tag_fat("duplicate type variable",sizeof(char),24U));}struct _tuple27{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2660 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2665
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
# 2669
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields!=0;sd_fields=sd_fields->tl){
if(Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,_tag_fat("",sizeof(char),1U))!=0)
fields=({struct Cyc_List_List*_Tmp1=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp2=({struct _tuple27*_Tmp3=_region_malloc(temp,0U,sizeof(struct _tuple27));_Tmp3->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=fields;_Tmp1;});}}
fields=Cyc_List_imp_rev(fields);{
# 2675
struct _fat_ptr aggr_str=(int)aggr_kind==0?_tag_fat("struct",sizeof(char),7U): _tag_fat("union",sizeof(char),6U);
# 2678
struct Cyc_List_List*ans=0;
for(1;des!=0;des=des->tl){
struct _tuple28*_Tmp1=(struct _tuple28*)des->hd;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*dl=_Tmp3;void*a=_Tmp2;
if(dl==0){
# 2683
struct Cyc_List_List*fields2=fields;
for(1;fields2!=0;fields2=fields2->tl){
if(!(*((struct _tuple27*)fields2->hd)).f2){
(*((struct _tuple27*)fields2->hd)).f2=1;
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp6=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp7->tag=1,_Tmp7->f1=(*((struct _tuple27*)fields2->hd)).f1->name;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=0;_Tmp5;});(*((struct _tuple28*)des->hd)).f1=_Tmp4;});
ans=({struct Cyc_List_List*_Tmp4=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp5=({struct _tuple29*_Tmp6=_region_malloc(rgn,0U,sizeof(struct _tuple29));_Tmp6->f1=(*((struct _tuple27*)fields2->hd)).f1,_Tmp6->f2=a;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=ans;_Tmp4;});
break;}}
# 2691
if(fields2==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("too many arguments to ",sizeof(char),23U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=aggr_str;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),2));});}else{
if(dl->tl!=0)
# 2695
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("multiple designators are not yet supported",sizeof(char),43U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});else{
# 2698
void*_Tmp4=(void*)dl->hd;void*_Tmp5;if(*((int*)_Tmp4)==0){
# 2700
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("array designator used in argument to ",sizeof(char),38U);_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=aggr_str;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL3;}else{_Tmp5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp4)->f1;{struct _fat_ptr*v=_Tmp5;
# 2703
struct Cyc_List_List*fields2=fields;
for(1;fields2!=0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,(*((struct _tuple27*)fields2->hd)).f1->name)==0){
if((*((struct _tuple27*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("member ",sizeof(char),8U);_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*v;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" has already been used as an argument",sizeof(char),38U);_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),3));});
(*((struct _tuple27*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp7=({struct _tuple29*_Tmp8=_region_malloc(rgn,0U,sizeof(struct _tuple29));_Tmp8->f1=(*((struct _tuple27*)fields2->hd)).f1,_Tmp8->f2=a;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=ans;_Tmp6;});
break;}}
# 2712
if(fields2==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("bad field designator ",sizeof(char),22U);_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*v;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL3;}}_LL3:;}}}}
# 2719
if((int)aggr_kind==0){
if(({int _Tmp1=Cyc_List_length(ans);_Tmp1 < Cyc_List_length(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("too few arguments to struct",sizeof(char),28U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});}else{
if(Cyc_List_length(ans)!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("union expression should have one member",sizeof(char),40U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});}{
struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(ans);_npop_handler(0);return _Tmp1;}}}
# 2669
;_pop_region();}
# 2730
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2732
void*_Tmp0=e1->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 14:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("we have a cast in a lhs: ",sizeof(char),26U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=e1;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});case 20: _Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1a=_Tmp1;
_Tmp1=e1a;goto _LL6;}case 23: _Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_Tmp1;
# 2736
return Cyc_Tcutil_is_zero_ptr_type(_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22: _Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1a=_Tmp1;
_Tmp1=e1a;goto _LLA;}case 21: _Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_Tmp1;
# 2740
if(Cyc_Tcutil_is_zero_ptr_type(_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("found zero pointer aggregate member assignment: ",sizeof(char),49U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=e1;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1a=_Tmp1;
_Tmp1=e1a;goto _LLE;}case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_Tmp1;
# 2745
if(Cyc_Tcutil_is_zero_ptr_type(_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("found zero pointer instantiate/noinstantiate: ",sizeof(char),47U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=e1;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}case 1:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("found bad lhs in is_zero_ptr_deref: ",sizeof(char),37U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=e1;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),2));});};}
# 2783 "tcutil.cyc"
int Cyc_Tcutil_is_noalias_qual(void*aq,int must_be_unique){
void*_Tmp0=Cyc_Absyn_compress(aq);void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 17: _Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_Tmp2;
# 2786
switch((int)aqv){case Cyc_Absyn_Unique_qual:
 return 1;case Cyc_Absyn_Refcnt_qual:
 goto _LLF;case Cyc_Absyn_Restricted_qual: _LLF:
 return !must_be_unique;default:
 return 0;};}case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd)){case 2: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->tl;{struct Cyc_List_List*bnd=_Tmp1;
# 2794
_Tmp1=bnd;goto _LL6;}case 0: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->tl;_LL6: {struct Cyc_List_List*bnd=_Tmp1;
# 2796
void*_Tmp3=(void*)bnd->hd;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==17)switch((int)((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f1){case Cyc_Absyn_Unique_qual:
 return 1;case Cyc_Absyn_Refcnt_qual:
 goto _LL18;case Cyc_Absyn_Restricted_qual: _LL18:
 return !must_be_unique;default: goto _LL19;}else{goto _LL19;}}else{_LL19:
 return 0;};}default: goto _LL7;}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 2804
 return 0;};}
# 2809
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*aqb,void*t,int must_be_unique){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;switch(*((int*)_Tmp0)){case 3: _Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.aqual;{void*aq=_Tmp1;
# 2812
return Cyc_Tcutil_is_noalias_qual(aq,must_be_unique);}case 2: _Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 2814
return({void*_Tmp2=Cyc_Tcutil_lookup_aquals(aqb,t);Cyc_Tcutil_is_noalias_qual(_Tmp2,must_be_unique);});}default:
# 2830 "tcutil.cyc"
 return 0;};}
# 2833
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*aqb,void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(aqb,t,0))
return 1;{
union Cyc_Absyn_DatatypeFieldInfo _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)t)->f1;{struct Cyc_List_List*qts=_Tmp2;
# 2839
for(1;qts!=0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,(*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.tag==2){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 2844
if((*adp)->impl==0)
return 0;{
struct Cyc_List_List*inst=Cyc_List_zip((*adp)->tvs,ts);
{struct Cyc_List_List*x=_check_null((*adp)->impl)->fields;for(0;x!=0;x=x->tl){
if(({struct Cyc_List_List*_Tmp3=aqb;Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp3,Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type));}))
return 1;}}
return 0;}}}else{
# 2856
goto _LLA;}case 23: _LLA:
 return 0;case 24: _Tmp0=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeFieldInfo tinfo=_Tmp0;struct Cyc_List_List*ts=_Tmp2;
# 2860
void*_Tmp3;void*_Tmp4;if(tinfo.UnknownDatatypefield.tag==1)
return 0;else{_Tmp4=tinfo.KnownDatatypefield.val.f1;_Tmp3=tinfo.KnownDatatypefield.val.f2;{struct Cyc_Absyn_Datatypedecl*td=_Tmp4;struct Cyc_Absyn_Datatypefield*fld=_Tmp3;
# 2863
struct Cyc_List_List*inst=Cyc_List_zip(td->tvs,ts);
{struct Cyc_List_List*typs=fld->typs;for(0;typs!=0;typs=typs->tl){
t=inst==0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,t))
return 1;}}
# 2869
return 0;}};}default: goto _LLD;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*x=_Tmp2;
# 2852
for(1;x!=0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD:
# 2871
 return 0;};}}
# 2878
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*aqb,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==1)
return 0;else{goto _LL13;}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL6;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_Tmp2;
# 2883
return Cyc_Tcutil_is_noalias_pointer(aqb,_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(aqb,e1);}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
return Cyc_Tcutil_is_noalias_path(aqb,e1);}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 2886
void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));if(*((int*)_Tmp3)==6)
return Cyc_Tcutil_is_noalias_path(aqb,e1);else{
return 0;};}case 6: _Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 2891
return Cyc_Tcutil_is_noalias_path(aqb,e1)&& Cyc_Tcutil_is_noalias_path(aqb,e2);}case 9: _Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp2=e2;goto _LL10;}case 14: _Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_Tmp2;
return Cyc_Tcutil_is_noalias_path(aqb,e2);}case 37: _Tmp2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp2;
# 2895
while(1){
void*_Tmp3=s->r;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp3)){case 2: _Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
s=s2;goto _LL1A;}case 12: _Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;_Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp3)->f2;{struct Cyc_Absyn_Decl*d=_Tmp5;struct Cyc_Absyn_Stmt*s1=_Tmp4;
s=s1;goto _LL1A;}case 1: _Tmp5=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
return Cyc_Tcutil_is_noalias_path(aqb,e);}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("is_noalias_stmt_exp: ill-formed StmtExp",sizeof(char),40U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),1));});}_LL1A:;}}default: _LL13:
# 2902
 return 1;};}
# 2919 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2921
struct _tuple14 bogus_ans=({struct _tuple14 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=Cyc_Absyn_heap_rgn_type;_Tmp0;});
void*_Tmp0=e->r;int _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*x=_Tmp3;
# 2925
void*_Tmp4;switch(*((int*)x)){case 0:
 goto _LL11;case 2: _LL11:
 return bogus_ans;case 1: _Tmp4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)x)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 2929
void*_Tmp5=Cyc_Absyn_compress(_check_null(e->topt));if(*((int*)_Tmp5)==4)
# 2931
return({struct _tuple14 _Tmp6;_Tmp6.f1=1,_Tmp6.f2=Cyc_Absyn_heap_rgn_type;_Tmp6;});else{
return({struct _tuple14 _Tmp6;_Tmp6.f1=vd->tq.real_const,_Tmp6.f2=Cyc_Absyn_heap_rgn_type;_Tmp6;});};}case 4: _Tmp4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)x)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 2935
void*_Tmp5=Cyc_Absyn_compress(_check_null(e->topt));if(*((int*)_Tmp5)==4)
return({struct _tuple14 _Tmp6;_Tmp6.f1=1,_Tmp6.f2=_check_null(vd->rgn);_Tmp6;});else{
# 2938
vd->escapes=1;
return({struct _tuple14 _Tmp6;_Tmp6.f1=vd->tq.real_const,_Tmp6.f2=_check_null(vd->rgn);_Tmp6;});};}case 5: _Tmp4=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)x)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 2941
_Tmp4=vd;goto _LL19;}default: _Tmp4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)x)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 2943
vd->escapes=1;
return({struct _tuple14 _Tmp5;_Tmp5.f1=vd->tq.real_const,_Tmp5.f2=_check_null(vd->rgn);_Tmp5;});}};}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int is_tagged=_Tmp1;
# 2948
if(is_tagged)return bogus_ans;{
# 2951
void*_Tmp4=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp5;switch(*((int*)_Tmp4)){case 7: _Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp4)->f2;{struct Cyc_List_List*fs=_Tmp5;
# 2953
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fs,f);
if(finfo!=0 && finfo->width==0){
struct _tuple14 _Tmp6=Cyc_Tcutil_addressof_props(e1);void*_Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int c=_Tmp8;void*t=_Tmp7;
return({struct _tuple14 _Tmp9;_Tmp9.f1=finfo->tq.real_const || c,_Tmp9.f2=t;_Tmp9;});}}
# 2958
return bogus_ans;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1.KnownAggr.tag==2){_Tmp5=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp5;
# 2960
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_decl_field(ad,f);
if(finfo!=0 && finfo->width==0){
struct _tuple14 _Tmp6=Cyc_Tcutil_addressof_props(e1);void*_Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int c=_Tmp8;void*t=_Tmp7;
return({struct _tuple14 _Tmp9;_Tmp9.f1=finfo->tq.real_const || c,_Tmp9.f2=t;_Tmp9;});}}
# 2965
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29:
 return bogus_ans;};}}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int is_tagged=_Tmp1;
# 2970
if(is_tagged)return bogus_ans;{
# 2973
void*_Tmp4=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp4)==3){_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.elt_type;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.ptr_atts.rgn;{void*t1=_Tmp6;void*r=_Tmp5;
# 2975
struct Cyc_Absyn_Aggrfield*finfo;
{void*_Tmp7=Cyc_Absyn_compress(t1);void*_Tmp8;switch(*((int*)_Tmp7)){case 7: _Tmp8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp7)->f2;{struct Cyc_List_List*fs=_Tmp8;
# 2978
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f1)->f1.KnownAggr.tag==2){_Tmp8=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp7)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp8;
# 2980
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B:
 return bogus_ans;}_LL36:;}
# 2983
if(finfo!=0 && finfo->width==0)
return({struct _tuple14 _Tmp7;_Tmp7.f1=finfo->tq.real_const,_Tmp7.f2=r;_Tmp7;});
return bogus_ans;}}else{
return bogus_ans;};}}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp3;
# 2990
void*_Tmp4=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;if(*((int*)_Tmp4)==3){_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.ptr_atts.rgn;{struct Cyc_Absyn_Tqual tq=_Tmp6;void*r=_Tmp5;
# 2992
return({struct _tuple14 _Tmp7;_Tmp7.f1=tq.real_const,_Tmp7.f2=r;_Tmp7;});}}else{
return bogus_ans;};}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 2998
void*t=Cyc_Absyn_compress(_check_null(e1->topt));
struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;switch(*((int*)t)){case 6: _Tmp5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)t)->f1;{struct Cyc_List_List*ts=_Tmp5;
# 3002
struct _tuple13 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{unsigned i=_Tmp8;int known=_Tmp7;
if(!known)
return bogus_ans;{
struct _tuple12*finfo=Cyc_Absyn_lookup_tuple_field(ts,(int)i);
if(finfo!=0)
return({struct _tuple14 _Tmp9;_Tmp9.f1=(*finfo).f1.real_const,({void*_TmpA=Cyc_Tcutil_addressof_props(e1).f2;_Tmp9.f2=_TmpA;});_Tmp9;});
return bogus_ans;}}}case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*r=_Tmp5;
# 3010
return({struct _tuple14 _Tmp6;_Tmp6.f1=tq.real_const,_Tmp6.f2=r;_Tmp6;});}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;{struct Cyc_Absyn_Tqual tq=_Tmp4;
# 3016
return({struct _tuple14 _Tmp6;_Tmp6.f1=tq.real_const,({void*_Tmp7=Cyc_Tcutil_addressof_props(e1).f2;_Tmp6.f2=_Tmp7;});_Tmp6;});}default:
 return bogus_ans;};}default:
# 3020
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unary & applied to non-lvalue",sizeof(char),30U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp5,sizeof(void*),1));});
return bogus_ans;};}
# 3027
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*eopt=({void*_Tmp0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp0,b);});
if(eopt==0)return;{
struct _tuple13 _Tmp0=Cyc_Evexp_eval_const_uint_exp(eopt);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned j=_Tmp2;int known=_Tmp1;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("a dereference is out of bounds: ",sizeof(char),33U);_Tmp4;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp5;_Tmp5.tag=12,_Tmp5.f1=(int)j;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=
_tag_fat(" <= ",sizeof(char),5U);_Tmp6;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=(int)i;_Tmp7;});void*_Tmp7[4];_Tmp7[0]=& _Tmp3,_Tmp7[1]=& _Tmp4,_Tmp7[2]=& _Tmp5,_Tmp7[3]=& _Tmp6;((
# 3032
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2))(loc,_tag_fat(_Tmp7,sizeof(void*),4));});
# 3034
return;}}}
# 3037
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3045
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*rec)(int,struct Cyc_Absyn_Exp*)=Cyc_Tcutil_cnst_exp;
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 17: _LL6:
 goto _LL8;case 18: _LL8:
 goto _LLA;case 19: _LLA:
 goto _LLC;case 32: _LLC:
 goto _LLE;case 33: _LLE:
 return 1;case 1: _Tmp3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp3;
# 3058
void*_Tmp4;switch(*((int*)b)){case 2:
 return 1;case 1: _Tmp4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 3061
void*_Tmp5=Cyc_Absyn_compress(vd->type);switch(*((int*)_Tmp5)){case 4:
 goto _LL42;case 5: _LL42:
 return 1;default:
 return var_okay;};}case 4: _Tmp4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 3067
if((int)vd->sc!=0)
return var_okay;{
void*_Tmp5=Cyc_Absyn_compress(vd->type);switch(*((int*)_Tmp5)){case 4:
 goto _LL49;case 5: _LL49:
 return 1;default:
 return var_okay;};}}case 0:
# 3074
 return 0;default:
 return var_okay;};}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 3078
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
return rec(0,e1)&& rec(0,e2);}case 41: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp3=e2;goto _LL18;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp3=e2;goto _LL1A;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp3=e2;goto _LL1C;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_Tmp3;
# 3085
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp3;
return rec(1,e2);}case 27: _Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
return rec(0,e1)&& rec(0,e2);}case 28: _Tmp3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
return rec(0,e);}case 26: _Tmp3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp3;
_Tmp3=des;goto _LL26;}case 30: _Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL26: {struct Cyc_List_List*des=_Tmp3;
_Tmp3=des;goto _LL28;}case 29: _Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL28: {struct Cyc_List_List*des=_Tmp3;
# 3092
for(1;des!=0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
_Tmp3=es;goto _LL2C;}case 24: _Tmp3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_List_List*es=_Tmp3;
_Tmp3=es;goto _LL2E;}case 31: _Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_List_List*es=_Tmp3;
# 3099
for(1;es!=0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}case 37: _Tmp3=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
# 3104
void*_Tmp4=s->r;void*_Tmp5;if(*((int*)_Tmp4)==1){_Tmp5=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
return Cyc_Tcutil_cnst_exp(var_okay,e);}}else{
return 0;};}default:
# 3108
 return 0;};}
# 3111
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3115
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 3119
union Cyc_Absyn_AggrInfo _Tmp3;void*_Tmp4;switch(*((int*)c)){case 0:
 goto _LL11;case 1: _LL11:
 goto _LL13;case 3: _LL13:
 goto _LL15;case 2: _LL15:
 return 1;case 20: _Tmp4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f2;{struct Cyc_Absyn_Enumdecl*edo=_Tmp4;
# 3125
if(edo==0 || edo->fields==0)
return 0;
_Tmp4=(struct Cyc_List_List*)_check_null(edo->fields)->v;goto _LL19;}case 21: _Tmp4=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)c)->f1;_LL19: {struct Cyc_List_List*fs=_Tmp4;
# 3129
{struct Cyc_List_List*fs2=fs;for(0;fs2!=0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag==0)
return fs2==fs;{
struct _tuple13 _Tmp5=Cyc_Evexp_eval_const_uint_exp(_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));int _Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{unsigned i=_Tmp7;int known=_Tmp6;
if(known && i==0U)
return 1;}}}}
# 3136
return 0;}case 25: _Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp3;
# 3139
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)return 0;
if(_check_null(ad->impl)->exist_vars!=0)return 0;
if((int)ad->kind==1 && _check_null(ad->impl)->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,_check_null(ad->impl)->fields);}default:
 return 0;};}case 3: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*n=_Tmp2;
# 3147
return Cyc_Tcutil_force_type2bool(1,n);}case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*t=_Tmp2;
return Cyc_Tcutil_zeroable_type(t);}case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*tqs=_Tmp2;
# 3150
for(1;tqs!=0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp2;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default:
 return 0;};}
# 3158
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3160
struct _RegionHandle _Tmp0=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_List_List*inst=Cyc_List_rzip(rgn,rgn,tvs,ts);
for(1;fs!=0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _Tmp1=0;_npop_handler(0);return _Tmp1;}}{
# 3168
int _Tmp1=1;_npop_handler(0);return _Tmp1;}}
# 3161
;_pop_region();}
# 3172
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==8){_Tmp0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;{struct Cyc_Absyn_Typedefdecl*tdopt=_Tmp0;
# 3175
if(tdopt==0)
return;{
struct Cyc_Absyn_Tqual tq=tdopt->tq;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("qualifier within typedef type ",sizeof(char),31U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" is ignored",sizeof(char),12U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_warn2(loc,_tag_fat(_Tmp4,sizeof(void*),3));});
return;}}}else{
return;};}
# 3186
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;switch(*((int*)_Tmp0)){case 3: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo pi=_Tmp2;
return({void*_Tmp3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp3,pi.ptr_atts.bounds);});}case 4: _Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*e=_Tmp1;
return e;}default:
 return 0;};}
# 3196
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_Tmp0;switch(*((int*)b)){case 5: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp0;
_Tmp0=x;goto _LL4;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_Tmp0;
_Tmp0=x;goto _LL6;}case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_Tmp0;
_Tmp0=x;goto _LL8;}case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_Tmp0;
return x->escapes?0: x;}default:
 return 0;};}
# 3207
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*res=0;
for(1;l!=0;l=l->tl){
if((void**)l->hd!=0)res=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=*_check_null((void**)l->hd),_Tmp0->tl=res;_Tmp0;});}
return res;}
# 3214
void*Cyc_Tcutil_promote_array(void*t,void*rgn,void*aqual,int convert_tag){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;if(*((int*)_Tmp0)==4){_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*et=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Absyn_Exp*eopt=_Tmp2;void*zt=_Tmp1;
# 3217
void*b;
if(eopt==0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot convert tag without type!",sizeof(char),33U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),1));});{
void*_Tmp5=Cyc_Absyn_compress(_check_null(eopt->topt));void*_Tmp6;if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2!=0){_Tmp6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2->hd;{void*t=_Tmp6;
# 3226
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3231
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,aqual,tq,b,zt,Cyc_Absyn_false_type);}}else{
return t;};}
# 3237
static int Cyc_Tcutil_aquals_bnds_pred(struct _tuple15*bnd){
void*_Tmp0=(*bnd).f1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==16)
# 3240
return 1;else{goto _LL3;}}else{_LL3:
# 3242
 return 0;};}
# 3246
struct Cyc_List_List*Cyc_Tcutil_get_aquals_bounds(struct Cyc_Absyn_Fndecl*fd){
return({(struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_Tcutil_aquals_bnds_pred,fd->i.qual_bnd);}
