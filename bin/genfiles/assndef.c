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

# 74 "flags.h"
 extern int Cyc_Flags_debug_assn;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 234
extern struct Cyc_List_List*Cyc_List_imp_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 480 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 901 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 72
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 73 "assndef.h"
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 85
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Not_AssnDef_Assn_struct{int tag;void*f1;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 103
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 43 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;int _Tmp2;void*_Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp4;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 1: _Tmp3=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
res=Cyc_Absynpp_exp2string(e);goto _LL0;}case 2: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 49
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned)vd;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("%s_%x",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),2));});else{
# 52
res=Cyc_Absynpp_qvar2string(vd->name);}
goto _LL0;}case 3: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp2;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}}else{_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;int i=_Tmp2;
# 56
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_check_null(vd)->name));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}}case 5: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;void*t2=_Tmp0;
# 58
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp8,sizeof(void*),3));});
goto _LL0;}case 4: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;
# 62
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}default: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp3;void*tm=_Tmp0;
# 64
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(tm));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}}_LL0:;}
# 66
return res;}
# 69
static int Cyc_AssnDef_hash_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 0:
 return 0;case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;case 6:
 return 4;case 7:
 return 5;case 8:
 return 6;case 9:
 return 7;case 12:
 return 8;case 13:
 return 9;case 14:
 return 10;case 15:
 return 11;case 17:
 return 12;case 18:
 return 13;case 19:
 return 14;case 31:
 return 15;case 32:
 return 16;case 38:
 return 17;case 41:
 return 18;default:
 return 19;};}
# 94
static int Cyc_AssnDef_termhash(void*t){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;int _Tmp2;void*_Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp4;
return(int)i;}case 1: _Tmp3=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
return Cyc_AssnDef_hash_exp(e)+ 61;}case 2: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
return(int)((unsigned)vd);}case 3: _Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;int i=_Tmp2;
return(int)((unsigned)vd ^ (unsigned)(i + 1));}case 5: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;void*t2=_Tmp0;
# 101
return(int)(((unsigned)p ^ (unsigned)t1)^ (unsigned)t2);}case 4: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;
return(int)((unsigned)p ^ (unsigned)t1 + 1U);}default: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp3;
return Cyc_AssnDef_termhash(t);}};}struct _tuple23{void*f0;void*f1;};
# 107
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;enum Cyc_Absyn_Primop _Tmp4;void*_Tmp5;void*_Tmp6;int _Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;unsigned _TmpB;unsigned _TmpC;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_TmpC=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i1=_TmpC;unsigned i2=_TmpB;
return(int)(i1 - i2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1){_TmpA=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_TmpA=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_TmpA;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==2)
return 1;else{if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_TmpA=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp8=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*vd1=_TmpA;int i=_Tmp8;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;int j=_Tmp7;
# 119
if(vd1==vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==3)
return 1;else{if(*((int*)_Tmp0.f0)==6){if(*((int*)_Tmp0.f1)==6){_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_TmpA;void*tm1=_Tmp9;void*tp2=_Tmp6;void*tm2=_Tmp5;
# 124
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==6)
return 1;else{if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp4=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t1=_TmpA;enum Cyc_Absyn_Primop p2=_Tmp3;void*t2=_Tmp9;
# 130
if((int)p1!=(int)p2)return(int)p1 - (int)p2;
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==4)
return 1;else{_Tmp4=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t11=_TmpA;void*t12=_Tmp9;void*tp1=_Tmp6;enum Cyc_Absyn_Primop p2=_Tmp3;void*t21=_Tmp5;void*t22=_Tmp2;void*tp2=_Tmp1;
# 135
if((int)p1!=(int)p2)return(int)p1 - (int)p2;{
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}}}}}}}}}}}}};}
# 142
static void*Cyc_AssnDef_copy_term(void*t){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp6->tag=0,_Tmp6->f1=i;_Tmp6;});}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_Tmp6->tag=1,_Tmp6->f1=e;_Tmp6;});}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp6->tag=2,_Tmp6->f1=vd;_Tmp6;});}case 3: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp6->tag=3,_Tmp6->f1=vd,_Tmp6->f2=i,_Tmp6->f3=tp;_Tmp6;});}case 5: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp4;void*t2=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Binop_AssnDef_Term_struct));_Tmp6->tag=5,_Tmp6->f1=p,_Tmp6->f2=t1,_Tmp6->f3=t2,_Tmp6->f4=tp;_Tmp6;});}case 4: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t=_Tmp4;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Unop_AssnDef_Term_struct));_Tmp6->tag=4,_Tmp6->f1=p,_Tmp6->f2=t,_Tmp6->f3=tp;_Tmp6;});}default: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*t=_Tmp2;
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_Tmp6->tag=6,_Tmp6->f1=tp,_Tmp6->f2=t;_Tmp6;});}};}
# 155
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 157
static void*Cyc_AssnDef_hash_cons_term(void*t){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;}else{
# 163
h=_check_null(Cyc_AssnDef_term_hash_cons_table);}{
# 165
void**resopt=Cyc_Hashtable_lookup_other_opt(h,t,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
if(resopt==0){
void*t2=Cyc_AssnDef_copy_term(t);
Cyc_Hashtable_insert(h,t2,t2);
return t2;}else{
return*resopt;}}}
# 175
void*Cyc_AssnDef_uint(unsigned i){
struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=({struct Cyc_AssnDef_Uint_AssnDef_Term_struct _Tmp0;_Tmp0.tag=0,_Tmp0.f1=i;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 179
void*Cyc_AssnDef_zero (void){
return Cyc_AssnDef_uint(0U);}
# 182
void*Cyc_AssnDef_one (void){
return Cyc_AssnDef_uint(1U);}
# 185
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned c=_Tmp2;int known=_Tmp1;
if(known)return Cyc_AssnDef_uint(c);{
struct Cyc_AssnDef_Const_AssnDef_Term_struct t=({struct Cyc_AssnDef_Const_AssnDef_Term_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=e;_Tmp3;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}}
# 191
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Var_AssnDef_Term_struct t=({struct Cyc_AssnDef_Var_AssnDef_Term_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 195
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){
struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=vd,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 200
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL4;case Cyc_Absyn_Neq: _LL4:
 return 1;default:
 return 0;};}
# 208
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Bitand: _LL6:
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}
# 219
void*Cyc_AssnDef_get_term_type(void*t){
void*_Tmp0;switch(*((int*)t)){case 0:
 return Cyc_Absyn_int_type(2U,2U);case 1: _Tmp0=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return e->topt;}case 2: _Tmp0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd->type;}case 3: _Tmp0=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp0;
return topt;}case 5: _Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp0;
return tp;}case 4: _Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp0;
return tp;}default: _Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*tp=_Tmp0;
return tp;}};}
# 231
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 234
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 238
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 240
if((int)p==0)return t;{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 247
static void*Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Times:
 return Cyc_AssnDef_uint(i * j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}struct _tuple24{struct Cyc_List_List*f0;unsigned*f1;};
# 260
static struct _tuple24 Cyc_AssnDef_flatten(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
unsigned*res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=({unsigned*_Tmp4=_cycalloc_atomic(sizeof(unsigned));*_Tmp4=i;_Tmp4;});goto _LL0;}case 5: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 267
res=({unsigned*_Tmp4=_cycalloc_atomic(sizeof(unsigned));*_Tmp4=i;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 271
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 275
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}
# 278
return({struct _tuple24 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;_Tmp0;});}
# 281
static int Cyc_AssnDef_inv_cmp_term(void*t1,void*t2){
return - Cyc_AssnDef_cmp_term(t1,t2);}
# 284
static void*Cyc_AssnDef_flatten_merge(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*topt){
struct _tuple24 _Tmp0=Cyc_AssnDef_flatten(p,t1);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*tlist1=_Tmp2;unsigned*ip1=_Tmp1;
struct _tuple24 _Tmp3=Cyc_AssnDef_flatten(p,t2);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*tlist2=_Tmp5;unsigned*ip2=_Tmp4;
# 288
struct Cyc_List_List*termlist=Cyc_List_imp_merge(Cyc_AssnDef_inv_cmp_term,tlist1,tlist2);
if(termlist==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat(" commutative associative operator has no non constant operators!",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));{
# 292
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=5,_Tmp6.f1=p,_Tmp6.f2=t,_Tmp6.f3=(void*)termlist->hd,_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);
termlist=termlist->tl;}
# 299
if(ip1==0){
if(ip2!=0){
if((int)p==0 &&*ip2==0U ||
(int)p==1 &&*ip2==1U)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=5,_Tmp6.f1=p,_Tmp6.f2=t,({void*_Tmp7=Cyc_AssnDef_uint(*ip2);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}}else{
# 308
if(ip2!=0){
void*binres=Cyc_AssnDef_eval_binop(p,*ip1,*ip2);
if((int)p==0 &&({void*_Tmp6=binres;Cyc_AssnDef_cmp_term(_Tmp6,Cyc_AssnDef_zero());})==0 ||
(int)p==1 &&({void*_Tmp6=binres;Cyc_AssnDef_cmp_term(_Tmp6,Cyc_AssnDef_one());})==0)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=5,_Tmp6.f1=p,_Tmp6.f2=t,_Tmp6.f3=binres,_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}else{
# 316
if((int)p==0 &&*ip1==0U ||
(int)p==1 &&*ip1==1U)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=5,_Tmp6.f1=p,_Tmp6.f2=t,({void*_Tmp7=Cyc_AssnDef_uint(*ip1);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}}
# 323
return t;}}}}struct _tuple25{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 326
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
if(Cyc_AssnDef_is_nonassociative_commutative(p)&& Cyc_AssnDef_cmp_term(t1,t2)< 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 330
{struct _tuple25 _Tmp0=({struct _tuple25 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 333
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL9;}_LL9:;
# 343
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL5;else{goto _LL7;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){if(*((int*)_Tmp0.f2)==0){_Tmp2=_Tmp0.f1;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{void*t1=_Tmp2;unsigned i=_Tmp4;
# 345
return({void*_Tmp5=t1;void*_Tmp6=Cyc_AssnDef_uint(0U - i);Cyc_AssnDef_plus(_Tmp5,_Tmp6,tp);});}}else{_LL5: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 348
if(Cyc_AssnDef_cmp_term(t1,t2)==0)return Cyc_AssnDef_uint(0U);
goto _LL0;}}}else{_LL7:
# 358
 if(Cyc_AssnDef_is_associative_commutative(p))return Cyc_AssnDef_flatten_merge(p,t1,t2,tp);
goto _LL0;}}_LL0:;}{
# 361
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=5,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 365
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 369
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return Cyc_AssnDef_logicvar(0,counter ++,t);}
# 374
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)==0;}
# 390
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 392
LOOP:
 if(Cyc_List_memq(*prev,a))return c;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
_Tmp2=a1;_Tmp1=a2;goto _LLA;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LLA: {void*a1=_Tmp2;void*a2=_Tmp1;
# 401
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp2;
# 405
++ c;
a=a1;
goto LOOP;}case 6: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a1=_Tmp0;
# 409
++ c;
a=a1;
goto LOOP;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp2;
# 413
++ c;
a=a1;
goto LOOP;}};}}
# 420
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 423
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned c=Cyc_AssnDef_assn_size_loop(a,0U,& prev);
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 429
return c;}
# 434
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return _tag_fat("true",sizeof(char),5U);case 1:
 return _tag_fat("false",sizeof(char),6U);case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 439
*size +=1;{
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LL11;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LL11;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LL11;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LL11;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LL11;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LL11;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LL11:;
# 450
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp7,sizeof(void*),3));});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 452
*size +=1;
if(prec <= 5)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 456
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s || %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 458
*size +=1;
if(prec <= 10)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 462
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s && %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 464
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,20,size));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("!%s",sizeof(char),4U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
# 467
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("Subst[%s:=%s](%s)",sizeof(char),18U),_tag_fat(_Tmp7,sizeof(void*),3));});}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 471
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("Kill(%s)",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),1));});}};}
# 476
int Cyc_AssnDef_tree_size=0;
# 479
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 485
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 496 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 501
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
return(int)((unsigned)a1 + 61U);}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 507
return(int)((unsigned)a + 37U);}};}
# 513
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1==a2)return 0;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_AssnDef_Primreln _Tmp3;enum Cyc_AssnDef_Primreln _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp8=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
_Tmp8=a11;_Tmp7=a12;_Tmp6=a21;_Tmp5=a22;goto _LL10;}}else{
# 530
return -1;}case 3: switch(*((int*)_Tmp0.f1)){case 3: _Tmp8=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL10: {void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
# 525
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 532
 return -1;}default: switch(*((int*)_Tmp0.f1)){case 4: _LL13:
# 531
 return 1;case 3:
# 533
 return 1;default: if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_Tmp8=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp8;enum Cyc_AssnDef_Primreln p1=_Tmp4;void*t12=_Tmp7;void*t21=_Tmp6;enum Cyc_AssnDef_Primreln p2=_Tmp3;void*t22=_Tmp5;
# 535
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==2)
return 1;else{if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_Tmp8=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)_Tmp0.f1)->f1;{void*na1=_Tmp8;void*na2=_Tmp7;
a1=na1;a2=na2;goto LOOP;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==5)
return 1;else{if(*((int*)_Tmp0.f0)==7){if(*((int*)_Tmp0.f1)==7){_Tmp8=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_Tmp0.f1)->f1;{void*na1=_Tmp8;void*na2=_Tmp7;
a1=na1;a2=na2;goto LOOP;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==7)
return 1;else{_Tmp8=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp5=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp8;void*t1=_Tmp7;void*a11=_Tmp6;struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp2;void*a22=_Tmp1;
# 549
int c=(int)vd1 - (int)vd2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c!=0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}}}};}}
# 562
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 565
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_hash_cons_table=0;}struct _tuple26{void*f0;int f1;};
# 571
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table==0)return 0;{
struct _tuple26**resopt=({(struct _tuple26**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;})(_check_null(Cyc_AssnDef_hash_cons_table),a);
if(resopt==0)return 0;{
struct _tuple26*_Tmp0=*resopt;int _Tmp1;_Tmp1=_Tmp0->f1;{int b=_Tmp1;
return b;}}}}
# 582
static struct _tuple26 Cyc_AssnDef_copy_assn(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return({struct _tuple26 _Tmp4;_Tmp4.f0=(void*)& Cyc_AssnDef_true_assn,_Tmp4.f1=1;_Tmp4;});case 1:
 return({struct _tuple26 _Tmp4;_Tmp4.f0=(void*)& Cyc_AssnDef_false_assn,_Tmp4.f1=1;_Tmp4;});case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 587
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp6->tag=3,_Tmp6->f1=a1,_Tmp6->f2=a2;_Tmp6;});_Tmp4.f0=_Tmp5;}),({int _Tmp5=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_Tmp4.f1=_Tmp5;});_Tmp4;});}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 589
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp6->tag=4,_Tmp6->f1=a1,_Tmp6->f2=a2;_Tmp6;});_Tmp4.f0=_Tmp5;}),({int _Tmp5=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_Tmp4.f1=_Tmp5;});_Tmp4;});}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Not_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Not_AssnDef_Assn_struct));_Tmp6->tag=5,_Tmp6->f1=a;_Tmp6;});_Tmp4.f0=_Tmp5;}),_Tmp4.f1=0;_Tmp4;});}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp2;
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp6->tag=2,_Tmp6->f1=t1,_Tmp6->f2=p,_Tmp6->f3=t2;_Tmp6;});_Tmp4.f0=_Tmp5;}),_Tmp4.f1=1;_Tmp4;});}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp3;void*t1=_Tmp2;void*a1=_Tmp0;
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Subst_AssnDef_Assn_struct));_Tmp6->tag=6,_Tmp6->f1=vd1,_Tmp6->f2=t1,_Tmp6->f3=a1;_Tmp6;});_Tmp4.f0=_Tmp5;}),_Tmp4.f1=0;_Tmp4;});}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
return({struct _tuple26 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Kill_AssnDef_Assn_struct));_Tmp6->tag=7,_Tmp6->f1=a;_Tmp6;});_Tmp4.f0=_Tmp5;}),_Tmp4.f1=0;_Tmp4;});}};}
# 601
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 608
({struct Cyc_Hashtable_Table*_Tmp0=h;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple26*))Cyc_Hashtable_insert;})(_Tmp0,(void*)& Cyc_AssnDef_true_assn,({struct _tuple26*_Tmp1=_cycalloc(sizeof(struct _tuple26));_Tmp1->f0=(void*)& Cyc_AssnDef_true_assn,_Tmp1->f1=1;_Tmp1;}));});
({struct Cyc_Hashtable_Table*_Tmp0=h;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple26*))Cyc_Hashtable_insert;})(_Tmp0,(void*)& Cyc_AssnDef_false_assn,({struct _tuple26*_Tmp1=_cycalloc(sizeof(struct _tuple26));_Tmp1->f0=(void*)& Cyc_AssnDef_false_assn,_Tmp1->f1=1;_Tmp1;}));});}else{
# 611
h=_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple26**resopt=({(struct _tuple26**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
struct _tuple26 _Tmp0=Cyc_AssnDef_copy_assn(a);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*res=_Tmp2;int reduced=_Tmp1;
({struct Cyc_Hashtable_Table*_Tmp3=h;void*_Tmp4=res;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple26*))Cyc_Hashtable_insert;})(_Tmp3,_Tmp4,({struct _tuple26*_Tmp5=_cycalloc(sizeof(struct _tuple26));_Tmp5->f0=res,_Tmp5->f1=reduced;_Tmp5;}));});
return res;}}else{
# 618
struct _tuple26*_Tmp0=*resopt;void*_Tmp1;_Tmp1=_Tmp0->f0;{void*res=_Tmp1;
return res;}}}}
# 624
void*Cyc_AssnDef_kill(void*a){
switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 7: _LL6:
 return a;default:  {
# 630
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=a;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 636
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 646
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 652
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 662
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 668
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;default:  {
# 673
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=x,_Tmp0.f2=newx,_Tmp0.f3=a;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 679
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 689
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,0U,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,1U,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 700
static void*Cyc_AssnDef_not_prim(void*t1,enum Cyc_Absyn_Primop p,void*t2){
switch((int)p){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen::not_prim: non-primop",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));};}
# 712
void*Cyc_AssnDef_not(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
return Cyc_AssnDef_not_prim(t1,(enum Cyc_Absyn_Primop)p,t2);}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp2;
return a;}default:  {
# 719
struct Cyc_AssnDef_Not_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Not_AssnDef_Assn_struct _Tmp3;_Tmp3.tag=5,_Tmp3.f1=a;_Tmp3;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}};}struct _tuple27{struct Cyc_Absyn_Vardecl*f0;void*f1;};
# 742 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 745
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 3: _LL6:
 return t;case 6: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp3;void*tm=_Tmp2;
# 750
void*tm2=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 4: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*tm=_Tmp3;void*tp=_Tmp2;
# 754
void*tm2=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);
if(tm!=tm2)return Cyc_AssnDef_unop(p,tm2,tp);else{
return t;}}case 5: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;void*s1=_Tmp2;void*tp=_Tmp0;
# 758
void*t2=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);
void*s2=Cyc_AssnDef_reduce_term(s1,non_esc_subst,esc_subst);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_binop(p,t2,s2,tp);else{
return t;}}default: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 763
if(vd->escapes && esc_subst!=0){
# 766
struct Cyc_List_List*s=*esc_subst;
for(1;s!=0;s=s->tl){
struct _tuple27 _Tmp4=*((struct _tuple27*)s->hd);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*vd2=_Tmp6;void*t2=_Tmp5;
# 770
if(vd2==vd)return t2;}}{
# 774
void*t=Cyc_AssnDef_fresh_var(vd->type);
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp6=({struct _tuple27*_Tmp7=_cycalloc(sizeof(struct _tuple27));_Tmp7->f0=vd,_Tmp7->f1=t;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*esc_subst;_Tmp5;});*esc_subst=_Tmp4;});
return t;}}else{
if(!vd->escapes)
# 779
for(1;non_esc_subst!=0;non_esc_subst=non_esc_subst->tl){
struct _tuple27 _Tmp4=*((struct _tuple27*)non_esc_subst->hd);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*vd2=_Tmp6;void*t2=_Tmp5;
if(vd==vd2)return t2;}}}
# 784
return t;}};}
# 788
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst,int negate){
# 792
void*res;
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 if(negate)return(void*)& Cyc_AssnDef_false_assn;else{return(void*)& Cyc_AssnDef_true_assn;}case 1:
 if(negate)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 798
void*newt1=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);
void*newt2=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);
if(negate)return Cyc_AssnDef_not_prim(t1,(enum Cyc_Absyn_Primop)p,t2);
if(t1!=newt1 || t2!=newt2)
a=Cyc_AssnDef_prim(newt1,p,newt2);
return a;}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 805
void*newa1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst,negate);
void*newa2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst,negate);
if(negate)return Cyc_AssnDef_or(newa1,newa2);
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_and(newa1,newa2);
return a;}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 812
void*newa1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst,negate);
void*newa2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst,negate);
if(negate)return Cyc_AssnDef_and(newa1,newa2);
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_or(newa1,newa2);
return a;}case 7: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 821
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst,negate);}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a1=_Tmp0;
# 825
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp5=({struct _tuple27*_Tmp6=_cycalloc(sizeof(struct _tuple27));_Tmp6->f0=vd,_Tmp6->f1=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=non_esc_subst;_Tmp4;});
a=a1;
goto LOOP;}default: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 830
a=a1;
negate=!negate;
goto LOOP;}};}}
# 837
void*Cyc_AssnDef_reduce(void*a){
# 839
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0,0);}
# 844
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;void*_Tmp3;switch(*((int*)t)){case 2: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 847
if(vd==x)return newx;
return t;}case 4: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp3;void*tp=_Tmp1;
# 850
void*t2=Cyc_AssnDef_subst_term(t1,x,newx);
if(t1!=t2)return Cyc_AssnDef_unop(p,t2,tp);
return t;}case 5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp3;void*s1=_Tmp1;void*tp=_Tmp0;
# 854
void*t2=Cyc_AssnDef_subst_term(t1,x,newx);
void*s2=Cyc_AssnDef_subst_term(s1,x,newx);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_binop(p,t2,s2,tp);else{
return t;}}case 6: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp3;void*tm=_Tmp1;
# 859
void*tm2=Cyc_AssnDef_subst_term(tm,x,newx);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0:
 goto _LLC;case 1: _LLC:
 goto _LLE;default: _LLE:
 return t;};}
# 869
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*esc_subst=0;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 884 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 888
while(1){
struct _tuple28 _Tmp0=({struct _tuple28 _Tmp1;_Tmp1.f0=a1s,_Tmp1.f1=a2s;_Tmp1;});if(_Tmp0.f0==0){if(_Tmp0.f1==0)
return;else{
# 892
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a2s,*res2);*res2=_Tmp1;});return;}}else{if(_Tmp0.f1==0){
# 891
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a1s,*res1);*res1=_Tmp1;});return;}else{
# 894
int c=({void*_Tmp1=(void*)_check_null(a1s)->hd;Cyc_AssnDef_assncmp(_Tmp1,(void*)_check_null(a2s)->hd);});
if(c==0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*common;_Tmp2;});*common=_Tmp1;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*res1;_Tmp2;});*res1=_Tmp1;});
a1s=a1s->tl;}else{
# 903
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a2s->hd,_Tmp2->tl=*res2;_Tmp2;});*res2=_Tmp1;});
a2s=a2s->tl;}}}};}}struct _tuple29{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 910
struct _tuple29 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 912
struct Cyc_List_List*common=0;
struct Cyc_List_List*res1=0;
struct Cyc_List_List*res2=0;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple29 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(common);_Tmp0.f0=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res1);_Tmp0.f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res2);_Tmp0.f2=_Tmp1;});_Tmp0;});}struct _tuple30{struct Cyc_List_List*f0;void*f1;};
# 921
struct _tuple30 Cyc_AssnDef_factor(void*a){
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 1:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp2;});case 2:
 return({struct _tuple30 _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=a,_Tmp4->tl=0;_Tmp4;});_Tmp2.f0=_Tmp3;}),_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 927
struct _tuple30 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple30 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
return({struct _tuple30 _Tmp8;({struct Cyc_List_List*_Tmp9=Cyc_List_merge(Cyc_AssnDef_assncmp,c1s,c2s);_Tmp8.f0=_Tmp9;}),({void*_Tmp9=Cyc_AssnDef_and(a1,a2);_Tmp8.f1=_Tmp9;});_Tmp8;});}}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 931
struct _tuple30 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple30 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
struct _tuple29 _Tmp8=Cyc_AssnDef_intersect_assns(c1s,c2s);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_List_List*cs=_TmpB;struct Cyc_List_List*c1s=_TmpA;struct Cyc_List_List*c2s=_Tmp9;
return({struct _tuple30 _TmpC;_TmpC.f0=cs,({void*_TmpD=({void*_TmpE=Cyc_List_fold_left(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_TmpE,Cyc_List_fold_left(Cyc_AssnDef_and,a2,c2s));});_TmpC.f1=_TmpD;});_TmpC;});}}}}default:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=a;_Tmp2;});};}
# 939
void*Cyc_AssnDef_factor_assn(void*a){
# 941
struct _tuple30 _Tmp0=Cyc_AssnDef_factor(a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*cs=_Tmp2;void*a=_Tmp1;
a=Cyc_List_fold_left(Cyc_AssnDef_and,a,cs);
# 946
return a;}}struct _tuple31{enum Cyc_AssnDef_Primreln f0;enum Cyc_AssnDef_Primreln f1;};
# 953
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)ctxt)){case 1:
 return 1;case 0:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)ctxt)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)ctxt)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 959
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)ctxt)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)ctxt)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 963
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f3;{void*t2=_Tmp2;enum Cyc_AssnDef_Primreln p2=_Tmp0;void*u2=_Tmp1;
# 967
struct _tuple31 _Tmp3=({struct _tuple31 _Tmp4;_Tmp4.f0=p2,_Tmp4.f1=p1;_Tmp4;});switch((int)_Tmp3.f0){case Cyc_AssnDef_Neq: if(_Tmp3.f1==Cyc_AssnDef_Neq)
goto _LL11;else{goto _LL26;}case Cyc_AssnDef_Eq: switch((int)_Tmp3.f1){case Cyc_AssnDef_Eq: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 972
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch((int)_Tmp3.f1){case Cyc_AssnDef_Neq:
 goto _LL19;case Cyc_AssnDef_SLt:
# 978
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1F:
# 980
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch((int)_Tmp3.f1){case Cyc_AssnDef_Neq: _LL19:
# 976
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1D:
# 979
 goto _LL1F;case Cyc_AssnDef_ULte: _LL21:
# 981
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_Tmp3.f1==Cyc_AssnDef_SLte){_LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_Tmp3.f1==Cyc_AssnDef_ULte){_LL25:
# 984
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26:
 return 0;};}default:
# 988
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;};}}
# 995
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt==a)return 1;{
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1002
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1006
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default:
# 1011
 a=Cyc_AssnDef_reduce(a);
goto LOOP;};}}
