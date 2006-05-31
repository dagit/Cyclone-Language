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
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

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


#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
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

#define _CYC_MAX_REGION_CONST 0
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

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
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
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 180
extern unsigned Cyc_Core_unique_qual;
# 193 "core.h"
void Cyc_Core_ufree(void*);
# 209 "core.h"
struct _fat_ptr Cyc_Core_alias_refptr(struct _fat_ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 276
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_rckey(unsigned,const char*,const char*,int);
# 303 "core.h"
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 57
extern struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);
# 328
extern void*Cyc_List_assoc(struct Cyc_List_List*,void*);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 67
extern struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 71
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 113
extern void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 332
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 563
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 935
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 939
void*Cyc_Absyn_compress(void*);
# 957
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 959
extern void*Cyc_Absyn_sint_type;
# 962
void*Cyc_Absyn_gen_float_type(unsigned);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 997
void*Cyc_Absyn_exn_type (void);
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1021
void*Cyc_Absyn_star_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1025
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1027
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1029
void*Cyc_Absyn_strct(struct _fat_ptr*);
void*Cyc_Absyn_strctq(struct _tuple1*);
void*Cyc_Absyn_unionq_type(struct _tuple1*);
# 1035
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1040
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1094
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1102
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1107
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1112
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1117
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1126
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1134
struct Cyc_Absyn_Exp*Cyc_Absyn_set_type(struct Cyc_Absyn_Exp*,void*);
# 1137
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1142
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1150
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1154
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1159
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1163
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1194
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1210
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1215
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1225
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1227
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1238
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 1250
int Cyc_Absyn_var_may_appear_exp(struct _tuple1*,struct Cyc_Absyn_Exp*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
# 75 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
void*Cyc_Warn_vimpos2(struct _fat_ptr);
void*Cyc_Warn_impos2(struct _fat_ptr);
# 81
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 34 "flags.h"
extern int Cyc_Flags_no_regions;
# 36
extern int Cyc_Flags_no_reaps;
# 81
extern int Cyc_Flags_noexpand_r;
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 30 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
# 35
int Cyc_Tcutil_is_arithmetic_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 46
int Cyc_Tcutil_is_fat_ptr(void*);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 51
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 70
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp_constrain(void*,void*,int);
# 75
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 77
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 87
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 95
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 99
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 131
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 134
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 186
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 192
int Cyc_Tcutil_is_noalias_qual(void*,int);
# 242
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 279
void*Cyc_Tcutil_get_pointer_bounds(void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 84 "attributes.h"
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
# 37
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 42
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;void*f1;struct _fat_ptr*f2;int f3;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;void*f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;void*f4;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;void*f1;int f2;struct _fat_ptr*f3;void*f4;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};char Cyc_Toc_NewInfo[8U]="NewInfo";
# 69 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){struct Cyc_Warn_String_Warn_Warg_struct _T0;void**_T1;struct _fat_ptr _T2;{struct Cyc_Warn_String_Warn_Warg_struct _T3;_T3.tag=0;
_T3.f1=str;_T0=_T3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3=_T0;void*_T4[1];_T1=_T4 + 0;*_T1=& _T3;_T2=_tag_fat(_T4,sizeof(void*),1);Cyc_Warn_impos2(_T2);}}
# 72
static void*Cyc_Toc_toc_impos(struct _fat_ptr ap){struct Cyc_Warn_String_Warn_Warg_struct*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;unsigned _T3;void**_T4;unsigned _T5;struct _fat_ptr _T6;unsigned _T7;void**_T8;unsigned _T9;void**_TA;unsigned _TB;struct _fat_ptr _TC;unsigned char*_TD;void**_TE;unsigned _TF;int _T10;{struct Cyc_Warn_String_Warn_Warg_struct*_T11=_cycalloc(sizeof(struct Cyc_Warn_String_Warn_Warg_struct));_T11->tag=0;
_T11->f1=_tag_fat("Toc: ",sizeof(char),6U);_T0=(struct Cyc_Warn_String_Warn_Warg_struct*)_T11;}{void*toc=(void*)_T0;_T2=ap;_T3=
_get_fat_size(_T2,sizeof(void*));{unsigned _T11=_T3 + 1U;_T5=_check_times(_T11,sizeof(void*));{void**_T12=_cycalloc(_T5);_T6=ap;_T7=_get_fat_size(_T6,sizeof(void*));{unsigned _T13=_T7 + 1U;unsigned i;i=0;_TL3: if(i < _T13)goto _TL1;else{goto _TL2;}_TL1: if(i!=0U)goto _TL4;_T9=i;_T8=_T12 + _T9;*_T8=toc;goto _TL5;_TL4: _TB=i;_TA=_T12 + _TB;_TC=ap;_TD=_TC.curr;_TE=(void**)_TD;_TF=i - 1U;_T10=(int)_TF;*_TA=_TE[_T10];_TL5: i=i + 1;goto _TL3;_TL2:;}_T4=(void**)_T12;}_T1=_tag_fat(_T4,sizeof(void*),_T11);}{struct _fat_ptr args=_T1;
Cyc_Warn_vimpos2(args);}}}
# 78
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 81
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 91
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 93
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){int(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table**_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;int _T5;_T1=Cyc_Hashtable_lookup;{
int(*_T6)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T6;}_T2=_check_null(Cyc_Toc_fn_pop_table);_T3=*_T2;_T4=s;_T5=_T0(_T3,_T4);return _T5;}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f0;struct _tuple1*f1;};
# 118
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){struct _tuple14*_T0;struct _tuple14 _T1;struct _tuple1*_T2;struct _tuple14*_T3;struct _tuple14 _T4;struct _tuple1*_T5;int _T6;struct _tuple14*_T7;struct _tuple14 _T8;struct _tuple1*_T9;struct _tuple14*_TA;struct _tuple14 _TB;struct _tuple1*_TC;int _TD;_T0=x;_T1=*_T0;_T2=_T1.f0;_T3=y;_T4=*_T3;_T5=_T4.f0;{
int i=Cyc_Absyn_qvar_cmp(_T2,_T5);
if(i==0)goto _TL6;_T6=i;return _T6;_TL6: _T7=x;_T8=*_T7;_T9=_T8.f1;_TA=y;_TB=*_TA;_TC=_TB.f1;_TD=
Cyc_Absyn_qvar_cmp(_T9,_TC);return _TD;}}
# 125
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){struct Cyc_Toc_TocState*_T0;struct _RegionHandle*_T1;struct Cyc_List_List**_T2;struct _RegionHandle*_T3;struct Cyc_List_List**_T4;struct _RegionHandle*_T5;struct Cyc_Dict_Dict*_T6;struct _RegionHandle*_T7;struct Cyc_Dict_Dict(*_T8)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*));struct Cyc_Dict_Dict(*_T9)(struct _RegionHandle*,int(*)(void*,void*));struct _RegionHandle*_TA;int(*_TB)(struct _tuple1*,struct _tuple1*);struct Cyc_List_List**_TC;struct _RegionHandle*_TD;struct Cyc_Set_Set**_TE;struct _RegionHandle*_TF;struct Cyc_Set_Set*(*_T10)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*));struct Cyc_Set_Set*(*_T11)(struct _RegionHandle*,int(*)(void*,void*));struct _RegionHandle*_T12;int(*_T13)(struct _tuple1*,struct _tuple1*);struct Cyc_Dict_Dict*_T14;struct _RegionHandle*_T15;struct Cyc_Dict_Dict(*_T16)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*));struct Cyc_Dict_Dict(*_T17)(struct _RegionHandle*,int(*)(void*,void*));struct _RegionHandle*_T18;int(*_T19)(struct _tuple1*,struct _tuple1*);struct Cyc_Dict_Dict*_T1A;struct _RegionHandle*_T1B;struct Cyc_Dict_Dict(*_T1C)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*));struct Cyc_Dict_Dict(*_T1D)(struct _RegionHandle*,int(*)(void*,void*));struct _RegionHandle*_T1E;_T1=d;{struct Cyc_Toc_TocState*_T1F=_region_malloc(_T1,0U,sizeof(struct Cyc_Toc_TocState));_T3=d;{struct Cyc_List_List**_T20=_region_malloc(_T3,0U,sizeof(struct Cyc_List_List*));
# 127
*_T20=0;_T2=(struct Cyc_List_List**)_T20;}_T1F->tuple_types=_T2;_T5=d;{struct Cyc_List_List**_T20=_region_malloc(_T5,0U,sizeof(struct Cyc_List_List*));
*_T20=0;_T4=(struct Cyc_List_List**)_T20;}_T1F->anon_aggr_types=_T4;_T7=d;{struct Cyc_Dict_Dict*_T20=_region_malloc(_T7,0U,sizeof(struct Cyc_Dict_Dict));_T9=Cyc_Dict_rempty;{
struct Cyc_Dict_Dict(*_T21)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))_T9;_T8=_T21;}_TA=d;_TB=Cyc_Absyn_qvar_cmp;*_T20=_T8(_TA,_TB);_T6=(struct Cyc_Dict_Dict*)_T20;}_T1F->aggrs_so_far=_T6;_TD=d;{struct Cyc_List_List**_T20=_region_malloc(_TD,0U,sizeof(struct Cyc_List_List*));
*_T20=0;_TC=(struct Cyc_List_List**)_T20;}_T1F->abs_struct_types=_TC;_TF=d;{struct Cyc_Set_Set**_T20=_region_malloc(_TF,0U,sizeof(struct Cyc_Set_Set*));_T11=Cyc_Set_rempty;{
struct Cyc_Set_Set*(*_T21)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))_T11;_T10=_T21;}_T12=d;_T13=Cyc_Absyn_qvar_cmp;*_T20=_T10(_T12,_T13);_TE=(struct Cyc_Set_Set**)_T20;}_T1F->datatypes_so_far=_TE;_T15=d;{struct Cyc_Dict_Dict*_T20=_region_malloc(_T15,0U,sizeof(struct Cyc_Dict_Dict));_T17=Cyc_Dict_rempty;{
struct Cyc_Dict_Dict(*_T21)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))_T17;_T16=_T21;}_T18=d;_T19=Cyc_Absyn_qvar_cmp;*_T20=_T16(_T18,_T19);_T14=(struct Cyc_Dict_Dict*)_T20;}_T1F->xdatatypes_so_far=_T14;_T1B=d;{struct Cyc_Dict_Dict*_T20=_region_malloc(_T1B,0U,sizeof(struct Cyc_Dict_Dict));_T1D=Cyc_Dict_rempty;{
struct Cyc_Dict_Dict(*_T21)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*)))_T1D;_T1C=_T21;}_T1E=d;*_T20=_T1C(_T1E,Cyc_Toc_qvar_tag_cmp);_T1A=(struct Cyc_Dict_Dict*)_T20;}_T1F->qvar_tags=_T1A;
_T1F->temp_labels=Cyc_Xarray_rcreate_empty(d);_T0=(struct Cyc_Toc_TocState*)_T1F;}
# 126
return _T0;}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 145
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f0;void*f1;};
# 151
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){struct Cyc_Toc_TocStateWrap*_T0;struct Cyc_Core_DynamicRegion*_T1;struct _fat_ptr _T2;struct Cyc_Toc_TocStateWrap*_T3;struct Cyc_Toc_TocStateWrap _T4;struct _fat_ptr _T5;char*_T6;char*_T7;struct Cyc_Core_DynamicRegion*_T8;struct _tuple15 _T9;struct _RegionHandle*_TA;struct _tuple15*_TB;struct _fat_ptr _TC;char*_TD;struct Cyc_Core_DynamicRegion*_TE;void*_TF;
# 154
struct Cyc_Toc_TocStateWrap*ts=0;{
struct Cyc_Toc_TocStateWrap*_T10=ts;struct Cyc_Toc_TocStateWrap*_T11=Cyc_Toc_toc_state;ts=_T11;Cyc_Toc_toc_state=_T10;}_T0=
_check_null(ts);{struct Cyc_Toc_TocStateWrap _T10=*_T0;struct Cyc_Toc_TocState*_T11;struct Cyc_Core_DynamicRegion*_T12;_T12=_T10.dyn;_T11=_T10.state;{struct Cyc_Core_DynamicRegion*dyn=_T12;struct Cyc_Toc_TocState*s=_T11;_T1=dyn;_T2=
_tag_fat(_T1,sizeof(struct Cyc_Core_DynamicRegion),1U);{struct _fat_ptr dyn2=Cyc_Core_alias_refptr(_T2);_T3=ts;{struct Cyc_Toc_TocStateWrap _T13;
_T13.dyn=dyn;_T13.state=s;_T4=_T13;}*_T3=_T4;{
struct Cyc_Toc_TocStateWrap*_T13=ts;struct Cyc_Toc_TocStateWrap*_T14=Cyc_Toc_toc_state;ts=_T14;Cyc_Toc_toc_state=_T13;}{
void*res;_T5=_check_fat_at_base(dyn2);_T6=_untag_fat_ptr_check_bound(_T5,sizeof(struct Cyc_Core_DynamicRegion),1U);_T7=_check_null(_T6);_T8=(struct Cyc_Core_DynamicRegion*)_T7;{struct _RegionHandle*h=& _T8->h;{struct _tuple15 _T13;
# 162
_T13.f0=s;_T13.f1=arg;_T9=_T13;}{struct _tuple15 env=_T9;_TA=h;_TB=& env;
res=f(_TA,_TB);}}_TC=_check_fat_at_base(dyn2);_TD=_untag_fat_ptr(_TC,sizeof(struct Cyc_Core_DynamicRegion),1U);_TE=(struct Cyc_Core_DynamicRegion*)_TD;
# 165
Cyc_Core_free_rckey(_TE);_TF=res;
return _TF;}}}}}struct _tuple16{struct _tuple1*f0;void*(*f1)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f0;struct _tuple16*f1;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f0;void*f1;};
# 169
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){struct _tuple17*_T0;struct _tuple16*_T1;struct _tuple18**(*_T2)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Toc_TocState*_T4;struct Cyc_Dict_Dict*_T5;struct Cyc_Dict_Dict _T6;struct _tuple1*_T7;void*_T8;struct _tuple18**_T9;struct _tuple18*_TA;struct _tuple18 _TB;_T0=env;{
# 172
struct _tuple17 _TC=*_T0;void*(*_TD)(struct _tuple1*);struct _tuple1*_TE;struct Cyc_Toc_TocState*_TF;_TF=_TC.f0;_T1=_TC.f1;{struct _tuple16 _T10=*_T1;_TE=_T10.f0;_TD=_T10.f1;}{struct Cyc_Toc_TocState*s=_TF;struct _tuple1*q=_TE;void*(*type_maker)(struct _tuple1*)=_TD;_T3=Cyc_Dict_lookup_opt;{
struct _tuple18**(*_T10)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T3;_T2=_T10;}_T4=s;_T5=_T4->aggrs_so_far;_T6=*_T5;_T7=q;{struct _tuple18**v=_T2(_T6,_T7);
if(v!=0)goto _TL8;_T8=type_maker(q);goto _TL9;_TL8: _T9=v;_TA=*_T9;_TB=*_TA;_T8=_TB.f1;_TL9: return _T8;}}}}
# 177
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){struct _tuple16 _T0;void*(*_T1)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*));struct _tuple16*_T2;void*_T3;{struct _tuple16 _T4;
_T4.f0=q;_T4.f1=type_maker;_T0=_T4;}{struct _tuple16 env=_T0;{
void*(*_T4)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*))=(void*(*)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state;_T1=_T4;}_T2=& env;_T3=_T1(_T2,Cyc_Toc_aggrdecl_type_body);return _T3;}}struct _tuple19{struct _fat_ptr*f0;struct _fat_ptr*f1;void*f2;};
# 184
static void*Cyc_Toc_tagged_union_field_struct(void*t,struct _fat_ptr*f){int*_T0;int _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6;void*_T7;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T8;union Cyc_Absyn_AggrInfo _T9;struct _union_AggrInfo_KnownAggr _TA;unsigned _TB;void*_TC;union Cyc_Absyn_AggrInfo _TD;struct _union_AggrInfo_KnownAggr _TE;struct Cyc_Absyn_Aggrdecl**_TF;struct Cyc_Absyn_Aggrdecl*_T10;void*_T11;union Cyc_Absyn_AggrInfo _T12;struct _union_AggrInfo_UnknownAggr _T13;struct _tuple3 _T14;struct _tuple1*_T15;struct Cyc_List_List*_T16;void*_T17;int _T18;int _T19;void*_T1A;struct Cyc_List_List*_T1B;struct _fat_ptr _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct _fat_ptr*_T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct _fat_ptr*_T20;void**_T21;void**_T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct _fat_ptr*_T25;struct Cyc_List_List*_T26;struct _tuple19*_T27;void*_T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;void**_T2A;int(*_T2B)(struct _fat_ptr ap);struct _fat_ptr _T2C;
# 186
static struct Cyc_List_List*cache=0;
void*_T2D=Cyc_Absyn_compress(t);struct _tuple1*_T2E;struct Cyc_Absyn_Aggrdecl*_T2F;_T0=(int*)_T2D;_T1=*_T0;if(_T1!=0)goto _TLA;_T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TLC;_T6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T7=_T6->f1;_T8=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T7;_T9=_T8->f1;_TA=_T9.KnownAggr;_TB=_TA.tag;if(_TB!=2)goto _TLE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T30=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_TC=_T30->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T31=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC;_TD=_T31->f1;_TE=_TD.KnownAggr;_TF=_TE.val;{struct Cyc_Absyn_Aggrdecl*_T32=*_TF;_T2F=_T32;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T2F;_T10=ad;_T2E=_T10->name;goto _LL4;}_TLE:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T30=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T11=_T30->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T31=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T11;_T12=_T31->f1;_T13=_T12.UnknownAggr;_T14=_T13.val;_T2E=_T14.f1;}}_LL4:{struct _tuple1*name=_T2E;struct _fat_ptr*_T30;_T15=name;{struct _tuple1 _T31=*_T15;_T30=_T31.f1;}{struct _fat_ptr*n=_T30;{
# 191
struct Cyc_List_List*c=cache;_TL13: if(c!=0)goto _TL11;else{goto _TL12;}
_TL11: _T16=c;_T17=_T16->hd;{struct _tuple19*_T31=(struct _tuple19*)_T17;void*_T32;struct _fat_ptr*_T33;struct _fat_ptr*_T34;{struct _tuple19 _T35=*_T31;_T34=_T35.f0;_T33=_T35.f1;_T32=_T35.f2;}{struct _fat_ptr*n2=_T34;struct _fat_ptr*f2=_T33;void*t=_T32;_T18=
Cyc_strptrcmp(n2,n);if(_T18!=0)goto _TL14;_T19=Cyc_strptrcmp(f,f2);if(_T19!=0)goto _TL14;_T1A=t;return _T1A;_TL14:;}}_T1B=c;
# 191
c=_T1B->tl;goto _TL13;_TL12:;}{struct Cyc_String_pa_PrintArg_struct _T31;_T31.tag=0;_T1E=n;
# 195
_T31.f1=*_T1E;_T1D=_T31;}{struct Cyc_String_pa_PrintArg_struct _T31=_T1D;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;_T20=f;_T32.f1=*_T20;_T1F=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T1F;void*_T33[2];_T21=_T33 + 0;*_T21=& _T31;_T22=_T33 + 1;*_T22=& _T32;_T23=_tag_fat("_union_%s_%s",sizeof(char),13U);_T24=_tag_fat(_T33,sizeof(void*),2);_T1C=Cyc_aprintf(_T23,_T24);}}{struct _fat_ptr str=_T1C;{struct _fat_ptr*_T31=_cycalloc(sizeof(struct _fat_ptr));
*_T31=str;_T25=(struct _fat_ptr*)_T31;}t=Cyc_Absyn_strct(_T25);{struct Cyc_List_List*_T31=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple19*_T32=_cycalloc(sizeof(struct _tuple19));
_T32->f0=n;_T32->f1=f;_T32->f2=t;_T27=(struct _tuple19*)_T32;}_T31->hd=_T27;_T31->tl=cache;_T26=(struct Cyc_List_List*)_T31;}cache=_T26;_T28=t;
return _T28;}}}_TLC: goto _LL5;_TLA: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;
_T30.f1=_tag_fat("tagged_union_field_struct",sizeof(char),26U);_T29=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_T29;void*_T31[1];_T2A=_T31 + 0;*_T2A=& _T30;{int(*_T32)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T2B=_T32;}_T2C=_tag_fat(_T31,sizeof(void*),1);_T2B(_T2C);};}static char _TmpG0[5U]="curr";
# 211 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_TmpG0,_TmpG0,_TmpG0 + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=(struct _fat_ptr*)& Cyc_Toc_curr_string;static char _TmpG1[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_TmpG1,_TmpG1,_TmpG1 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=(struct _fat_ptr*)& Cyc_Toc_tag_string;static char _TmpG2[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_TmpG2,_TmpG2,_TmpG2 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=(struct _fat_ptr*)& Cyc_Toc_val_string;static char _TmpG3[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_TmpG3,_TmpG3,_TmpG3 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=(struct _fat_ptr*)& Cyc_Toc__handler_cons_string;static char _TmpG4[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_TmpG4,_TmpG4,_TmpG4 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=(struct _fat_ptr*)& Cyc_Toc_handler_string;static char _TmpG5[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_TmpG5,_TmpG5,_TmpG5 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=(struct _fat_ptr*)& Cyc_Toc__RegionHandle_string;static char _TmpG6[7U]="_throw";
# 230 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_TmpG6,_TmpG6,_TmpG6 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__throw_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__throw_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__throw_ev;static char _TmpG7[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_TmpG7,_TmpG7,_TmpG7 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc_setjmp_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc_setjmp_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc_setjmp_ev;static char _TmpG8[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_TmpG8,_TmpG8,_TmpG8 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__push_handler_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__push_handler_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__push_handler_ev;static char _TmpG9[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_TmpG9,_TmpG9,_TmpG9 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__pop_handler_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__pop_handler_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__pop_handler_ev;static char _TmpGA[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_TmpGA,_TmpGA,_TmpGA + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__exn_thrown_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__exn_thrown_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__exn_thrown_ev;static char _TmpGB[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_TmpGB,_TmpGB,_TmpGB + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__npop_handler_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__npop_handler_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__npop_handler_ev;static char _TmpGC[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_TmpGC,_TmpGC,_TmpGC + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_null_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_null_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_null_ev;static char _TmpGD[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_TmpGD,_TmpGD,_TmpGD + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_known_subscript_null_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_known_subscript_null_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_known_subscript_null_ev;static char _TmpGE[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_TmpGE,_TmpGE,_TmpGE + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_known_subscript_notnull_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_known_subscript_notnull_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_known_subscript_notnull_ev;static char _TmpGF[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_TmpGF,_TmpGF,_TmpGF + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_fat_subscript_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_fat_subscript_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_fat_subscript_ev;static char _TmpG10[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_TmpG10,_TmpG10,_TmpG10 + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fat_ptr_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fat_ptr_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fat_ptr_ev;static char _TmpG11[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_TmpG11,_TmpG11,_TmpG11 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__tag_fat_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__tag_fat_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__tag_fat_ev;static char _TmpG12[27U]="_untag_fat_ptr_check_bound";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_check_bound_str={_TmpG12,_TmpG12,_TmpG12 + 27U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_check_bound_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__untag_fat_ptr_check_bound_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_check_bound_bnd={0,& Cyc_Toc__untag_fat_ptr_check_bound_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_check_bound_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__untag_fat_ptr_check_bound_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_check_bound_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__untag_fat_ptr_check_bound_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_check_bound_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__untag_fat_ptr_check_bound_ev;static char _TmpG13[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_TmpG13,_TmpG13,_TmpG13 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__untag_fat_ptr_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__untag_fat_ptr_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__untag_fat_ptr_ev;static char _TmpG14[19U]="_check_fat_at_base";
static struct _fat_ptr Cyc_Toc__check_fat_at_base_str={_TmpG14,_TmpG14,_TmpG14 + 19U};static struct _tuple1 Cyc_Toc__check_fat_at_base_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_fat_at_base_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_at_base_bnd={0,& Cyc_Toc__check_fat_at_base_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_at_base_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_fat_at_base_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_at_base_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_fat_at_base_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_at_base_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_fat_at_base_ev;static char _TmpG15[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_TmpG15,_TmpG15,_TmpG15 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__get_fat_size_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__get_fat_size_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__get_fat_size_ev;static char _TmpG16[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_TmpG16,_TmpG16,_TmpG16 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fat_ptr_decrease_size_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fat_ptr_decrease_size_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fat_ptr_decrease_size_ev;static char _TmpG17[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_TmpG17,_TmpG17,_TmpG17 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__get_zero_arr_size_char_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__get_zero_arr_size_char_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__get_zero_arr_size_char_ev;static char _TmpG18[25U]="_get_zero_arr_size_other";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_other_str={_TmpG18,_TmpG18,_TmpG18 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_other_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__get_zero_arr_size_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_other_bnd={0,& Cyc_Toc__get_zero_arr_size_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_other_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__get_zero_arr_size_other_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_other_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__get_zero_arr_size_other_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_other_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__get_zero_arr_size_other_ev;static char _TmpG19[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_TmpG19,_TmpG19,_TmpG19 + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fat_ptr_plus_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fat_ptr_plus_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fat_ptr_plus_ev;static char _TmpG1A[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_TmpG1A,_TmpG1A,_TmpG1A + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_plus_char_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_plus_char_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_plus_char_ev;static char _TmpG1B[21U]="_zero_arr_plus_other";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_other_str={_TmpG1B,_TmpG1B,_TmpG1B + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_other_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_plus_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_other_bnd={0,& Cyc_Toc__zero_arr_plus_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_other_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_plus_other_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_other_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_plus_other_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_other_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_plus_other_ev;static char _TmpG1C[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_TmpG1C,_TmpG1C,_TmpG1C + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fat_ptr_inplace_plus_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fat_ptr_inplace_plus_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _TmpG1D[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_TmpG1D,_TmpG1D,_TmpG1D + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _TmpG1E[29U]="_zero_arr_inplace_plus_other";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_other_str={_TmpG1E,_TmpG1E,_TmpG1E + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_other_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_inplace_plus_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_other_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_other_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_inplace_plus_other_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_other_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_inplace_plus_other_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_other_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_inplace_plus_other_ev;static char _TmpG1F[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_TmpG1F,_TmpG1F,_TmpG1F + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fat_ptr_inplace_plus_post_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _TmpG20[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_TmpG20,_TmpG20,_TmpG20 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _TmpG21[34U]="_zero_arr_inplace_plus_post_other";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_other_str={_TmpG21,_TmpG21,_TmpG21 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_other_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_other_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_other_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__zero_arr_inplace_plus_post_other_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_other_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__zero_arr_inplace_plus_post_other_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_other_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__zero_arr_inplace_plus_post_other_ev;static char _TmpG22[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_TmpG22,_TmpG22,_TmpG22 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__cycalloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__cycalloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__cycalloc_ev;static char _TmpG23[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_TmpG23,_TmpG23,_TmpG23 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__cyccalloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__cyccalloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__cyccalloc_ev;static char _TmpG24[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_TmpG24,_TmpG24,_TmpG24 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__cycalloc_atomic_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__cycalloc_atomic_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__cycalloc_atomic_ev;static char _TmpG25[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_TmpG25,_TmpG25,_TmpG25 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__cyccalloc_atomic_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__cyccalloc_atomic_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__cyccalloc_atomic_ev;static char _TmpG26[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_TmpG26,_TmpG26,_TmpG26 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__region_malloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__region_malloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__region_malloc_ev;static char _TmpG27[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_TmpG27,_TmpG27,_TmpG27 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__region_calloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__region_calloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__region_calloc_ev;static char _TmpG28[16U]="_region_vmalloc";
static struct _fat_ptr Cyc_Toc__region_vmalloc_str={_TmpG28,_TmpG28,_TmpG28 + 16U};static struct _tuple1 Cyc_Toc__region_vmalloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__region_vmalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_vmalloc_bnd={0,& Cyc_Toc__region_vmalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_vmalloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__region_vmalloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__region_vmalloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__region_vmalloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_vmalloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__region_vmalloc_ev;static char _TmpG29[14U]="_aqual_malloc";
static struct _fat_ptr Cyc_Toc__aqual_malloc_str={_TmpG29,_TmpG29,_TmpG29 + 14U};static struct _tuple1 Cyc_Toc__aqual_malloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__aqual_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__aqual_malloc_bnd={0,& Cyc_Toc__aqual_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__aqual_malloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__aqual_malloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__aqual_malloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__aqual_malloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__aqual_malloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__aqual_malloc_ev;static char _TmpG2A[14U]="_aqual_calloc";
static struct _fat_ptr Cyc_Toc__aqual_calloc_str={_TmpG2A,_TmpG2A,_TmpG2A + 14U};static struct _tuple1 Cyc_Toc__aqual_calloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__aqual_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__aqual_calloc_bnd={0,& Cyc_Toc__aqual_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__aqual_calloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__aqual_calloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__aqual_calloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__aqual_calloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__aqual_calloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__aqual_calloc_ev;static char _TmpG2B[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_TmpG2B,_TmpG2B,_TmpG2B + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__fast_region_malloc_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__fast_region_malloc_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__fast_region_malloc_ev;static char _TmpG2C[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_TmpG2C,_TmpG2C,_TmpG2C + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__check_times_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__check_times_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__check_times_ev;static char _TmpG2D[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_TmpG2D,_TmpG2D,_TmpG2D + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__new_region_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__new_region_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__new_region_ev;static char _TmpG2E[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_TmpG2E,_TmpG2E,_TmpG2E + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__push_region_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__push_region_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__push_region_ev;static char _TmpG2F[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_TmpG2F,_TmpG2F,_TmpG2F + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__pop_region_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__pop_region_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__pop_region_ev;static char _TmpG30[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_TmpG30,_TmpG30,_TmpG30 + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__throw_arraybounds_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__throw_arraybounds_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__throw_arraybounds_ev;static char _TmpG31[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_TmpG31,_TmpG31,_TmpG31 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__throw_match_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__throw_match_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__throw_match_ev;static char _TmpG32[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_TmpG32,_TmpG32,_TmpG32 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__rethrow_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__rethrow_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__rethrow_ev;static char _TmpG33[14U]="_throw_assert";
static struct _fat_ptr Cyc_Toc__throw_assert_str={_TmpG33,_TmpG33,_TmpG33 + 14U};static struct _tuple1 Cyc_Toc__throw_assert_pr={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_Toc__throw_assert_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_assert_bnd={0,& Cyc_Toc__throw_assert_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_assert_re={1,(void*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)& Cyc_Toc__throw_assert_bnd)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_assert_ev={0,(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)& Cyc_Toc__throw_assert_re),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_assert_e=(struct Cyc_Absyn_Exp*)& Cyc_Toc__throw_assert_ev;
# 280
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 282
void*Cyc_Toc_void_star_type (void){void*_T0;void*_T1;void*_T2;struct Cyc_Absyn_Tqual _T3;void*_T4;void*_T5;void*_T6;
static void*t=0;
if(t!=0)goto _TL16;_T0=Cyc_Absyn_void_type;_T1=Cyc_Absyn_heap_rgn_type;_T2=Cyc_Absyn_al_qual_type;_T3=
Cyc_Absyn_empty_tqual(0U);_T4=Cyc_Absyn_false_type;_T5=Cyc_Absyn_false_type;t=Cyc_Absyn_star_type(_T0,_T1,_T2,_T3,_T4,_T5);goto _TL17;_TL16: _TL17: _T6=t;
return _T6;}
# 288
static void*Cyc_Toc_fat_ptr_type (void){struct _tuple1*_T0;union Cyc_Absyn_AggrInfo _T1;void*_T2;
static void*t=0;
if(t!=0)goto _TL18;_T0=& Cyc_Toc__fat_ptr_pr;_T1=
Cyc_Absyn_UnknownAggr(0U,_T0,0);t=Cyc_Absyn_aggr_type(_T1,0);goto _TL19;_TL18: _TL19: _T2=t;
return _T2;}
# 294
static void*Cyc_Toc_rgn_type (void){void*_T0;struct Cyc_Absyn_Tqual _T1;void*_T2;
static void*r=0;
if(r!=0)goto _TL1A;_T0=
Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);_T1=Cyc_Toc_mt_tq;r=Cyc_Absyn_cstar_type(_T0,_T1);goto _TL1B;_TL1A: _TL1B: _T2=r;
return _T2;}
# 300
static void*Cyc_Toc_aqualt_type (void){void*_T0;_T0=Cyc_Absyn_uint_type;
return _T0;}
# 303
static struct Cyc_Absyn_Exp*Cyc_Toc_aliasable_qual_exp (void){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;
static struct Cyc_Absyn_Exp*aq=0;
if(aq!=0)goto _TL1C;_T0=
Cyc_Absyn_uint_exp(0U,0U);_T1=Cyc_Absyn_uint_type;aq=Cyc_Absyn_set_type(_T0,_T1);goto _TL1D;_TL1C: _TL1D: _T2=aq;
return _T2;}
# 309
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){struct Cyc_Absyn_Stmt*_T0;_T0=
Cyc_Absyn_skip_stmt(0U);return _T0;}struct _tuple20{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 313
static struct _tuple20*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){struct _tuple20*_T0;struct Cyc_List_List*_T1;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T2;{struct _tuple20*_T3=_cycalloc(sizeof(struct _tuple20));{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T5=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T5->tag=1;
_T5->f1=name;_T2=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T5;}_T4->hd=(void*)_T2;_T4->tl=0;_T1=(struct Cyc_List_List*)_T4;}_T3->f0=_T1;_T3->f1=e;_T0=(struct _tuple20*)_T3;}return _T0;}
# 317
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){struct Cyc_Absyn_Exp*_T0;struct Cyc_List_List*_T1;struct Cyc_Absyn_Exp*_T2;_T0=f;_T1=
Cyc_List_from_array(args);_T2=Cyc_Absyn_fncall_exp(_T0,_T1,0U);return _T2;}
# 320
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T3;enum Cyc_Absyn_Coercion _T4;struct Cyc_Absyn_Exp*_T5;struct Cyc_Absyn_Exp*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;_T0=e;{
void*_T9=_T0->r;struct Cyc_Absyn_Exp*_TA;_T1=(int*)_T9;_T2=*_T1;if(_T2!=14)goto _TL1E;_T3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T9;_T4=_T3->f4;if(_T4!=Cyc_Absyn_No_coercion)goto _TL20;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T9;_TA=_TB->f2;}{struct Cyc_Absyn_Exp*e=_TA;_T5=
Cyc_Toc_cast_it(t,e);return _T5;}_TL20: goto _LL3;_TL1E: _LL3: _T6=
Cyc_Absyn_cast_exp(t,e,0,1U,0U);_T7=t;_T8=Cyc_Absyn_set_type(_T6,_T7);return _T8;;}}
# 327
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){struct Cyc_Absyn_Exp*_T0;unsigned _T1;struct Cyc_Absyn_Exp*_T2;struct _tuple1*_T3;struct _tuple1*_T4;struct Cyc_List_List*_T5;struct _fat_ptr**_T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _tuple1*_T9;struct _fat_ptr*_TA;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_T11;struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_T12;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Absyn_Tqual _T15;void*_T16;void*_T17;struct Cyc_Absyn_Tqual _T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_T1A;struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Absyn_Exp*_T1D;
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;_T0=_get_exn_thrown_e;_T1=(unsigned)_T0;
# 330
if(!_T1)goto _TL22;_T2=
Cyc_Absyn_copy_exp(_get_exn_thrown_e);return _T2;_TL22: {
struct _tuple1*qv;qv=_cycalloc(sizeof(struct _tuple1));_T3=qv;_T3->f0.Abs_n.tag=1U;_T4=qv;{struct _fat_ptr*_T1E[1];_T6=_T1E + 0;{struct _fat_ptr*_T1F=_cycalloc(sizeof(struct _fat_ptr));*_T1F=_tag_fat("Core",sizeof(char),5U);_T7=(struct _fat_ptr*)_T1F;}*_T6=_T7;_T8=_tag_fat(_T1E,sizeof(struct _fat_ptr*),1);_T5=Cyc_List_list(_T8);}_T4->f0.Abs_n.val=_T5;_T9=qv;{struct _fat_ptr*_T1E=_cycalloc(sizeof(struct _fat_ptr));*_T1E=_tag_fat("get_exn_thrown",sizeof(char),15U);_TA=(struct _fat_ptr*)_T1E;}_T9->f1=_TA;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T1E=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_T1E->tag=0;
_T1E->f1=qv;_TB=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T1E;}{void*bnd=(void*)_TB;
struct Cyc_Absyn_Exp*fnname;fnname=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_TC=fnname;_TC->topt=0;_TD=fnname;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1E=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_T1E->tag=1;_T1E->f1=bnd;_TE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T1E;}_TD->r=(void*)_TE;_TF=fnname;_TF->loc=0U;_T10=fnname;_T11=& Cyc_Absyn_EmptyAnnot_val;_T12=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_T11;_T10->annot=(void*)_T12;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T1E=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_T1E->tag=10;
_T1E->f1=fnname;_T1E->f2=0;_T1E->f3=0;_T1E->f4=0;_T13=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T1E;}{void*fncall_re=(void*)_T13;_T14=fnname;_T15=Cyc_Toc_mt_tq;_T16=
# 337
Cyc_Toc_void_star_type();_T17=Cyc_Absyn_function_type(0,0,_T15,_T16,0,0,0,0,0,0,0,0,0,0);_T18=Cyc_Toc_mt_tq;
# 336
_T14->topt=
Cyc_Absyn_cstar_type(_T17,_T18);{struct Cyc_Absyn_Exp*_T1E=_cycalloc(sizeof(struct Cyc_Absyn_Exp));
# 340
_T1E->topt=0;_T1E->r=fncall_re;_T1E->loc=0U;_T1A=& Cyc_Absyn_EmptyAnnot_val;_T1B=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_T1A;_T1E->annot=(void*)_T1B;_T19=(struct Cyc_Absyn_Exp*)_T1E;}_get_exn_thrown_e=_T19;_T1C=
_check_null(_get_exn_thrown_e);_T1C->topt=Cyc_Toc_void_star_type();_T1D=
_check_null(_get_exn_thrown_e);return _T1D;}}}}
# 347
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_T2->tag=14;
_T2->f1=t;_T2->f2=e;_T2->f3=0;_T2->f4=1U;_T0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 350
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct));_T2->tag=20;
_T2->f1=e;_T0=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 353
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct));_T2->tag=23;
_T2->f1=e1;_T2->f2=e2;_T0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 356
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_T2->tag=36;
_T2->f1=s;_T0=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 359
static void*Cyc_Toc_sizeoftype_exp_r(void*t){struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_T2->tag=17;
_T2->f1=t;_T0=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 362
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_T2->tag=10;
_T2->f1=e;_T2->f2=Cyc_List_from_array(es);_T2->f3=0;_T2->f4=1;_T0=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 365
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T0;void*_T1;{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_T2->tag=2;
_T2->f1=s1;_T2->f2=s2;_T0=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 368
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct));_T2->tag=6;
_T2->f1=e1;_T2->f2=e2;_T2->f3=e3;_T0=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 371
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_T2->tag=21;
_T2->f1=e;_T2->f2=n;_T2->f3=0;_T2->f4=0;_T0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 374
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T2->tag=35;
# 377
_T2->f1=tdopt;_T2->f2=ds;_T0=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 379
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_T0;void*_T1;{struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct));_T2->tag=8;
_T2->f1=v;_T0=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 383
static void*Cyc_Toc_zero_exp (void){struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T0;void*_T1;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T2=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct));_T2->tag=0;_T2->f1.Int_c.tag=5U;
_T2->f1.Int_c.val.f0=0U;_T2->f1.Int_c.val.f1=0;_T0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 389
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Absyn_Exp*_T4;_T0=e;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;_T1=(int*)_T5;_T2=*_T1;if(_T2!=20)goto _TL24;{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T5;_T6=_T7->f1;}{struct Cyc_Absyn_Exp*e1=_T6;_T3=
Cyc_Absyn_aggrarrow_exp(e1,f,loc);return _T3;}_TL24: _T4=
Cyc_Absyn_aggrmember_exp(e,f,loc);return _T4;;}}
# 397
static struct Cyc_Absyn_Exp*Cyc_Toc_get_curr(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;_T0=
Cyc_Toc_member_exp(e,Cyc_Toc_curr_sp,0U);_T1=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);_T2=Cyc_Absyn_set_type(_T0,_T1);return _T2;}
# 401
static int Cyc_Toc_isCharStar(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_PtrInfo _T5;void*_T6;int _T7;_T0=e;_T1=_T0->topt;_T2=
_check_null(_T1);{void*_T8=Cyc_Absyn_compress(_T2);struct Cyc_Absyn_PtrInfo _T9;_T3=(int*)_T8;_T4=*_T3;if(_T4!=4)goto _TL26;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T8;_T9=_TA->f1;}{struct Cyc_Absyn_PtrInfo pi=_T9;_T5=pi;_T6=_T5.elt_type;_T7=
Cyc_Tcutil_is_char_type(_T6);return _T7;}_TL26:
 return 0;;}}
# 413
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_Null_c _T5;unsigned _T6;int _T7;struct Cyc_List_List*_T8;void*_T9;struct _tuple20*_TA;struct _tuple20 _TB;struct Cyc_Absyn_Exp*_TC;int _TD;struct Cyc_List_List*_TE;int _TF;_T0=e;{
void*_T10=_T0->r;struct Cyc_List_List*_T11;struct Cyc_Absyn_Exp*_T12;_T1=(int*)_T10;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T10;_T4=_T3->f1;_T5=_T4.Null_c;_T6=_T5.tag;if(_T6!=1)goto _TL29;
return 1;_TL29: goto _LLF;case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T10;_T12=_T13->f2;}{struct Cyc_Absyn_Exp*e1=_T12;_T7=
Cyc_Toc_is_zero(e1);return _T7;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T10;_T11=_T13->f1;}{struct Cyc_List_List*dles=_T11;_T11=dles;goto _LL8;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T10;_T11=_T13->f3;}_LL8: {struct Cyc_List_List*dles=_T11;_T11=dles;goto _LLA;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T10;_T11=_T13->f3;}_LLA: {struct Cyc_List_List*dles=_T11;_T11=dles;goto _LLC;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T10;_T11=_T13->f2;}_LLC: {struct Cyc_List_List*dles=_T11;_T11=dles;goto _LLE;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T10;_T11=_T13->f2;}_LLE: {struct Cyc_List_List*dles=_T11;
# 422
_TL2E: if(dles!=0)goto _TL2C;else{goto _TL2D;}
_TL2C: _T8=dles;_T9=_T8->hd;_TA=(struct _tuple20*)_T9;_TB=*_TA;_TC=_TB.f1;_TD=Cyc_Toc_is_zero(_TC);if(_TD)goto _TL2F;else{goto _TL31;}_TL31: return 0;_TL2F: _TE=dles;
# 422
dles=_TE->tl;goto _TL2E;_TL2D:
# 424
 return 1;}default: _LLF: _TF=
Cyc_Tcutil_is_zero(e);return _TF;};}}
# 430
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){struct _tuple1*_T0;union Cyc_Absyn_Nmspace _T1;struct _union_Nmspace_Abs_n _T2;unsigned _T3;union Cyc_Absyn_Nmspace _T4;struct _union_Nmspace_Rel_n _T5;union Cyc_Absyn_Nmspace _T6;struct _union_Nmspace_Abs_n _T7;union Cyc_Absyn_Nmspace _T8;struct _union_Nmspace_C_n _T9;struct _fat_ptr _TA;struct Cyc_String_pa_PrintArg_struct _TB;struct _fat_ptr*_TC;struct Cyc_String_pa_PrintArg_struct _TD;struct _fat_ptr*_TE;void**_TF;void**_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct _fat_ptr*_T15;struct Cyc_String_pa_PrintArg_struct _T16;struct _RegionHandle*_T17;struct Cyc_List_List*_T18;struct _fat_ptr _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct _fat_ptr*_T1B;void**_T1C;void**_T1D;void**_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr*_T21;union Cyc_Absyn_Nmspace _T22;_T0=x;{struct _tuple1 _T23=*_T0;_T22=_T23.f0;_T21=_T23.f1;}{union Cyc_Absyn_Nmspace ns=_T22;struct _fat_ptr*v=_T21;struct Cyc_List_List*_T23;_T1=ns;_T2=_T1.Abs_n;_T3=_T2.tag;switch(_T3){case 4: _T23=0;goto _LL7;case 2: _T4=ns;_T5=_T4.Rel_n;_T23=_T5.val;_LL7: {struct Cyc_List_List*vs=_T23;_T23=vs;goto _LL9;}case 1: _T6=ns;_T7=_T6.Abs_n;_T23=_T7.val;_LL9: {struct Cyc_List_List*vs=_T23;_T23=vs;goto _LLB;}default: _T8=ns;_T9=_T8.C_n;_T23=_T9.val;_LLB: {struct Cyc_List_List*vs=_T23;
# 439
if(vs!=0)goto _TL33;{struct Cyc_String_pa_PrintArg_struct _T24;_T24.tag=0;_TC=s;
_T24.f1=*_TC;_TB=_T24;}{struct Cyc_String_pa_PrintArg_struct _T24=_TB;{struct Cyc_String_pa_PrintArg_struct _T25;_T25.tag=0;_TE=v;_T25.f1=*_TE;_TD=_T25;}{struct Cyc_String_pa_PrintArg_struct _T25=_TD;void*_T26[2];_TF=_T26 + 0;*_TF=& _T24;_T10=_T26 + 1;*_T10=& _T25;_T11=_tag_fat("%s_%s_struct",sizeof(char),13U);_T12=_tag_fat(_T26,sizeof(void*),2);_TA=Cyc_aprintf(_T11,_T12);}}return _TA;_TL33: {struct _RegionHandle _T24=_new_region(0U,"r");struct _RegionHandle*r=& _T24;_push_region(r);{struct Cyc_String_pa_PrintArg_struct _T25;_T25.tag=0;_T15=s;
# 442
_T25.f1=*_T15;_T14=_T25;}{struct Cyc_String_pa_PrintArg_struct _T25=_T14;{struct Cyc_String_pa_PrintArg_struct _T26;_T26.tag=0;_T17=r;_T18=vs;_T19=_tag_fat("_",sizeof(char),2U);_T26.f1=Cyc_rstr_sepstr(_T17,_T18,_T19);_T16=_T26;}{struct Cyc_String_pa_PrintArg_struct _T26=_T16;{struct Cyc_String_pa_PrintArg_struct _T27;_T27.tag=0;_T1B=v;_T27.f1=*_T1B;_T1A=_T27;}{struct Cyc_String_pa_PrintArg_struct _T27=_T1A;void*_T28[3];_T1C=_T28 + 0;*_T1C=& _T25;_T1D=_T28 + 1;*_T1D=& _T26;_T1E=_T28 + 2;*_T1E=& _T27;_T1F=_tag_fat("%s_%s_%s_struct",sizeof(char),16U);_T20=_tag_fat(_T28,sizeof(void*),3);_T13=Cyc_aprintf(_T1F,_T20);}}}{struct _fat_ptr _T25=_T13;_npop_handler(0);return _T25;}_pop_region();}}};}}struct _tuple21{struct Cyc_Toc_TocState*f0;struct _tuple14*f1;};
# 452
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){struct _tuple21*_T0;struct Cyc_Toc_TocState*_T1;struct _tuple14*_T2;struct _handler_cons*_T3;int*_T4;int _T5;struct _tuple1*(*_T6)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*);void*(*_T7)(struct Cyc_Dict_Dict,int(*)(void*,void*),void*);struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict _T9;struct _tuple14*_TA;void*_TB;struct Cyc_Dict_Absent_exn_struct*_TC;char*_TD;char*_TE;struct _RegionHandle*_TF;struct _tuple14*_T10;struct _tuple14*_T11;struct _tuple1*_T12;struct _tuple1*_T13;struct _tuple1*_T14;struct _fat_ptr*_T15;struct Cyc_Dict_Dict*_T16;struct Cyc_Dict_Dict(*_T17)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*);struct Cyc_Dict_Dict(*_T18)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T19;struct Cyc_Dict_Dict _T1A;struct _tuple14*_T1B;struct _tuple1*_T1C;struct _tuple1*_T1D;_T0=env;{
# 455
struct _tuple21 _T1E=*_T0;struct _tuple14*_T1F;struct Cyc_Dict_Dict*_T20;_T1=_T1E.f0;{struct Cyc_Toc_TocState _T21=*_T1;_T20=_T21.qvar_tags;}_T1F=_T1E.f1;{struct Cyc_Dict_Dict*qvs=_T20;struct _tuple14*pair=_T1F;_T2=pair;{
struct _tuple14 _T21=*_T2;struct _tuple1*_T22;struct _tuple1*_T23;_T23=_T21.f0;_T22=_T21.f1;{struct _tuple1*fieldname=_T23;struct _tuple1*dtname=_T22;struct _handler_cons _T24;_T3=& _T24;_push_handler(_T3);{int _T25=0;_T4=_T24.handler;_T5=setjmp(_T4);if(!_T5)goto _TL35;_T25=1;goto _TL36;_TL35: _TL36: if(_T25)goto _TL37;else{goto _TL39;}_TL39: _T7=Cyc_Dict_lookup_other;{
struct _tuple1*(*_T26)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*)=(struct _tuple1*(*)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*))_T7;_T6=_T26;}_T8=qvs;_T9=*_T8;_TA=pair;{struct _tuple1*_T26=_T6(_T9,Cyc_Toc_qvar_tag_cmp,_TA);_npop_handler(0);return _T26;}_pop_handler();goto _TL38;_TL37: _TB=Cyc_Core_get_exn_thrown();{void*_T26=(void*)_TB;void*_T27;_TC=(struct Cyc_Dict_Absent_exn_struct*)_T26;_TD=_TC->tag;_TE=Cyc_Dict_Absent;if(_TD!=_TE)goto _TL3A;{
# 459
struct _tuple14*new_pair;_TF=d;new_pair=_region_malloc(_TF,0U,sizeof(struct _tuple14));_T10=new_pair;_T10->f0=fieldname;_T11=new_pair;_T11->f1=dtname;{struct _fat_ptr*_T28;union Cyc_Absyn_Nmspace _T29;_T12=fieldname;{struct _tuple1 _T2A=*_T12;_T29=_T2A.f0;_T28=_T2A.f1;}{union Cyc_Absyn_Nmspace nmspace=_T29;struct _fat_ptr*fieldvar=_T28;
# 461
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res;res=_cycalloc(sizeof(struct _tuple1));_T13=res;_T13->f0=nmspace;_T14=res;{struct _fat_ptr*_T2A=_cycalloc(sizeof(struct _fat_ptr));*_T2A=newvar;_T15=(struct _fat_ptr*)_T2A;}_T14->f1=_T15;_T16=qvs;_T18=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T2A)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*))_T18;_T17=_T2A;}_T19=qvs;_T1A=*_T19;_T1B=new_pair;_T1C=res;*_T16=_T17(_T1A,_T1B,_T1C);_T1D=res;
return _T1D;}}}_TL3A: _T27=_T26;{void*exn=_T27;_rethrow(exn);};}_TL38:;}}}}}}
# 467
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){struct _tuple14 _T0;struct _tuple1*(*_T1)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple21*));struct _tuple14*_T2;struct _tuple1*_T3;{struct _tuple14 _T4;
_T4.f0=fieldname;_T4.f1=dtname;_T0=_T4;}{struct _tuple14 env=_T0;{
struct _tuple1*(*_T4)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple21*))=(struct _tuple1*(*)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state;_T1=_T4;}_T2=& env;_T3=_T1(_T2,Cyc_Toc_collapse_qvars_body);return _T3;}}
# 474
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){struct Cyc_Absyn_Aggrdecl*_T0;struct _tuple1*_T1;struct Cyc_Absyn_AggrdeclImpl*_T2;{struct Cyc_Absyn_Aggrdecl*_T3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));
_T3->kind=0U;_T3->sc=2U;_T3->tvs=0;
_T3->attributes=0;_T3->expected_mem_kind=0;{struct _tuple1*_T4=_cycalloc(sizeof(struct _tuple1));
_T4->f0=Cyc_Absyn_Rel_n(0);_T4->f1=name;_T1=(struct _tuple1*)_T4;}_T3->name=_T1;{struct Cyc_Absyn_AggrdeclImpl*_T4=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));
_T4->exist_vars=0;_T4->qual_bnd=0;_T4->fields=fs;_T4->tagged=0;_T4->effconstr=0;_T2=(struct Cyc_Absyn_AggrdeclImpl*)_T4;}_T3->impl=_T2;_T0=(struct Cyc_Absyn_Aggrdecl*)_T3;}
# 475
return _T0;}struct _tuple22{struct Cyc_Toc_TocState*f0;struct Cyc_List_List*f1;};struct _tuple23{void*f0;struct Cyc_List_List*f1;};struct _tuple24{struct Cyc_Absyn_Tqual f0;void*f1;};
# 483
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){struct _tuple22*_T0;struct Cyc_Toc_TocState*_T1;struct Cyc_List_List**_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;void*_T6;struct _tuple24*_T7;struct _tuple24 _T8;void*_T9;struct Cyc_List_List*_TA;void*_TB;int _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_List_List*_T10;struct _fat_ptr*_T11;struct _fat_ptr _T12;struct Cyc_Int_pa_PrintArg_struct _T13;int _T14;int _T15;void**_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_List_List*_T19;void*_T1A;struct _tuple24*_T1B;struct _tuple24 _T1C;struct Cyc_List_List*_T1D;struct Cyc_Absyn_Aggrfield*_T1E;struct Cyc_List_List*_T1F;struct _RegionHandle*_T20;struct Cyc_List_List*_T21;struct Cyc_Absyn_Aggrdecl**_T22;union Cyc_Absyn_AggrInfo _T23;struct Cyc_List_List*_T24;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T25;void*_T26;struct Cyc_List_List**_T27;struct Cyc_List_List*_T28;struct _RegionHandle*_T29;struct _tuple23*_T2A;struct _RegionHandle*_T2B;struct Cyc_List_List**_T2C;void*_T2D;_T0=env;{
# 486
struct _tuple22 _T2E=*_T0;struct Cyc_List_List*_T2F;struct Cyc_List_List**_T30;_T1=_T2E.f0;{struct Cyc_Toc_TocState _T31=*_T1;_T30=_T31.tuple_types;}_T2F=_T2E.f1;{struct Cyc_List_List**tuple_types=_T30;struct Cyc_List_List*tqs0=_T2F;_T2=tuple_types;{
# 488
struct Cyc_List_List*tts=*_T2;_TL3F: if(tts!=0)goto _TL3D;else{goto _TL3E;}
_TL3D: _T3=tts;_T4=_T3->hd;{struct _tuple23*_T31=(struct _tuple23*)_T4;struct Cyc_List_List*_T32;void*_T33;{struct _tuple23 _T34=*_T31;_T33=_T34.f0;_T32=_T34.f1;}{void*x=_T33;struct Cyc_List_List*ts=_T32;
struct Cyc_List_List*tqs=tqs0;
_TL43: if(tqs!=0)goto _TL44;else{goto _TL42;}_TL44: if(ts!=0)goto _TL41;else{goto _TL42;}
_TL41: _T5=tqs;_T6=_T5->hd;_T7=(struct _tuple24*)_T6;_T8=*_T7;_T9=_T8.f1;_TA=ts;_TB=_TA->hd;_TC=Cyc_Unify_unify(_T9,_TB);if(_TC)goto _TL45;else{goto _TL47;}
_TL47: goto _TL42;_TL45: _TD=tqs;
# 491
tqs=_TD->tl;_TE=ts;ts=_TE->tl;goto _TL43;_TL42:
# 494
 if(tqs!=0)goto _TL48;if(ts!=0)goto _TL48;_TF=x;
return _TF;_TL48:;}}_T10=tts;
# 488
tts=_T10->tl;goto _TL3F;_TL3E:;}{
# 499
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr));_T11=xname;{struct Cyc_Int_pa_PrintArg_struct _T31;_T31.tag=1;_T14=Cyc_Toc_tuple_type_counter;Cyc_Toc_tuple_type_counter=_T14 + 1;_T15=_T14;_T31.f1=(unsigned long)_T15;_T13=_T31;}{struct Cyc_Int_pa_PrintArg_struct _T31=_T13;void*_T32[1];_T16=_T32 + 0;*_T16=& _T31;_T17=_tag_fat("_tuple%d",sizeof(char),9U);_T18=_tag_fat(_T32,sizeof(void*),1);_T12=Cyc_aprintf(_T17,_T18);}*_T11=_T12;{
struct Cyc_List_List*fs=0;
struct Cyc_List_List*ts=0;{
int i=1;_TL4D: if(tqs0!=0)goto _TL4B;else{goto _TL4C;}
_TL4B: _T19=tqs0;_T1A=_T19->hd;_T1B=(struct _tuple24*)_T1A;_T1C=*_T1B;{void*t=_T1C.f1;{struct Cyc_List_List*_T31=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggrfield*_T32=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));
_T32->name=Cyc_Absyn_fieldname(i);_T32->tq=Cyc_Toc_mt_tq;_T32->type=t;_T32->width=0;_T32->attributes=0;_T32->requires_clause=0;_T1E=(struct Cyc_Absyn_Aggrfield*)_T32;}_T31->hd=_T1E;_T31->tl=fs;_T1D=(struct Cyc_List_List*)_T31;}fs=_T1D;_T20=d;{struct Cyc_List_List*_T31=_region_malloc(_T20,0U,sizeof(struct Cyc_List_List));
_T31->hd=t;_T31->tl=ts;_T1F=(struct Cyc_List_List*)_T31;}ts=_T1F;}_T21=tqs0;
# 502
tqs0=_T21->tl;i=i + 1;goto _TL4D;_TL4C:;}
# 507
fs=Cyc_List_imp_rev(fs);
ts=Cyc_List_imp_rev(ts);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);{struct Cyc_Absyn_Aggrdecl**_T31=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_T31=sd;_T22=(struct Cyc_Absyn_Aggrdecl**)_T31;}_T23=Cyc_Absyn_KnownAggr(_T22);{void*ans=Cyc_Absyn_aggr_type(_T23,0);{struct Cyc_List_List*_T31=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T32=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T32->tag=5;
_T32->f1=sd;_T25=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T32;}_T26=(void*)_T25;_T31->hd=Cyc_Absyn_new_decl(_T26,0U);_T31->tl=Cyc_Toc_result_decls;_T24=(struct Cyc_List_List*)_T31;}Cyc_Toc_result_decls=_T24;_T27=tuple_types;_T29=d;{struct Cyc_List_List*_T31=_region_malloc(_T29,0U,sizeof(struct Cyc_List_List));_T2B=d;{struct _tuple23*_T32=_region_malloc(_T2B,0U,sizeof(struct _tuple23));
_T32->f0=ans;_T32->f1=ts;_T2A=(struct _tuple23*)_T32;}_T31->hd=_T2A;_T2C=tuple_types;_T31->tl=*_T2C;_T28=(struct Cyc_List_List*)_T31;}*_T27=_T28;_T2D=ans;
return _T2D;}}}}}}}
# 515
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){void*(*_T0)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple22*));void*_T1;{
void*(*_T2)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple22*))=(void*(*)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state;_T0=_T2;}_T1=_T0(tqs0,Cyc_Toc_add_tuple_type_body);return _T1;}struct _tuple25{enum Cyc_Absyn_AggrKind f0;struct Cyc_List_List*f1;};struct _tuple26{struct Cyc_Toc_TocState*f0;struct _tuple25*f1;};struct _tuple27{void*f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 520
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple26*env){struct _tuple26*_T0;struct Cyc_Toc_TocState*_T1;struct _tuple25*_T2;struct Cyc_List_List**_T3;struct Cyc_List_List*_T4;void*_T5;enum Cyc_Absyn_AggrKind _T6;int _T7;enum Cyc_Absyn_AggrKind _T8;int _T9;int(*_TA)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);int(*_TB)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);int(*_TC)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;int _TF;void*_T10;struct Cyc_List_List*_T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct Cyc_Int_pa_PrintArg_struct _T14;int _T15;int _T16;void**_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_Aggrdecl**_T1B;union Cyc_Absyn_AggrInfo _T1C;struct Cyc_List_List*_T1D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T1E;void*_T1F;struct Cyc_List_List**_T20;struct Cyc_List_List*_T21;struct _RegionHandle*_T22;struct _tuple27*_T23;struct _RegionHandle*_T24;struct Cyc_List_List**_T25;void*_T26;struct Cyc_List_List*_T27;enum Cyc_Absyn_AggrKind _T28;struct Cyc_List_List**_T29;_T0=env;{struct _tuple26 _T2A=*_T0;_T1=_T2A.f0;{struct Cyc_Toc_TocState _T2B=*_T1;_T29=_T2B.anon_aggr_types;}_T2=_T2A.f1;{struct _tuple25 _T2B=*_T2;_T28=_T2B.f0;_T27=_T2B.f1;}}{struct Cyc_List_List**anon_aggr_types=_T29;enum Cyc_Absyn_AggrKind ak=_T28;struct Cyc_List_List*fs=_T27;_T3=anon_aggr_types;{
# 525
struct Cyc_List_List*ts=*_T3;_TL51: if(ts!=0)goto _TL4F;else{goto _TL50;}
_TL4F: _T4=ts;_T5=_T4->hd;{struct _tuple27*_T2A=(struct _tuple27*)_T5;struct Cyc_List_List*_T2B;enum Cyc_Absyn_AggrKind _T2C;void*_T2D;{struct _tuple27 _T2E=*_T2A;_T2D=_T2E.f0;_T2C=_T2E.f1;_T2B=_T2E.f2;}{void*x=_T2D;enum Cyc_Absyn_AggrKind ak2=_T2C;struct Cyc_List_List*fs2=_T2B;_T6=ak;_T7=(int)_T6;_T8=ak2;_T9=(int)_T8;
if(_T7!=_T9)goto _TL52;_TB=Cyc_List_list_cmp;{int(*_T2E)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_TB;_TA=_T2E;}_TC=Cyc_Tcutil_aggrfield_cmp;_TD=fs2;_TE=fs;_TF=_TA(_TC,_TD,_TE);if(_TF)goto _TL52;else{goto _TL54;}
_TL54: _T10=x;return _T10;_TL52:;}}_T11=ts;
# 525
ts=_T11->tl;goto _TL51;_TL50:;}{
# 532
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr));_T12=xname;{struct Cyc_Int_pa_PrintArg_struct _T2A;_T2A.tag=1;_T15=Cyc_Toc_tuple_type_counter;Cyc_Toc_tuple_type_counter=_T15 + 1;_T16=_T15;_T2A.f1=(unsigned long)_T16;_T14=_T2A;}{struct Cyc_Int_pa_PrintArg_struct _T2A=_T14;void*_T2B[1];_T17=_T2B + 0;*_T17=& _T2A;_T18=_tag_fat("_tuple%d",sizeof(char),9U);_T19=_tag_fat(_T2B,sizeof(void*),1);_T13=Cyc_aprintf(_T18,_T19);}*_T12=_T13;{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);_T1A=sd;
_T1A->kind=ak;{struct Cyc_Absyn_Aggrdecl**_T2A=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_T2A=sd;_T1B=(struct Cyc_Absyn_Aggrdecl**)_T2A;}_T1C=Cyc_Absyn_KnownAggr(_T1B);{void*ans=Cyc_Absyn_aggr_type(_T1C,0);{struct Cyc_List_List*_T2A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T2B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T2B->tag=5;
_T2B->f1=sd;_T1E=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T2B;}_T1F=(void*)_T1E;_T2A->hd=Cyc_Absyn_new_decl(_T1F,0U);_T2A->tl=Cyc_Toc_result_decls;_T1D=(struct Cyc_List_List*)_T2A;}Cyc_Toc_result_decls=_T1D;_T20=anon_aggr_types;_T22=d;{struct Cyc_List_List*_T2A=_region_malloc(_T22,0U,sizeof(struct Cyc_List_List));_T24=d;{struct _tuple27*_T2B=_region_malloc(_T24,0U,sizeof(struct _tuple27));
_T2B->f0=ans;_T2B->f1=ak;_T2B->f2=fs;_T23=(struct _tuple27*)_T2B;}_T2A->hd=_T23;_T25=anon_aggr_types;_T2A->tl=*_T25;_T21=(struct Cyc_List_List*)_T2A;}*_T20=_T21;_T26=ans;
return _T26;}}}}}
# 540
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){void*(*_T0)(struct _tuple25*,void*(*)(struct _RegionHandle*,struct _tuple26*));struct _tuple25*_T1;void*_T2;{
void*(*_T3)(struct _tuple25*,void*(*)(struct _RegionHandle*,struct _tuple26*))=(void*(*)(struct _tuple25*,void*(*)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state;_T0=_T3;}{struct _tuple25*_T3=_cycalloc(sizeof(struct _tuple25));_T3->f0=ak;_T3->f1=fs;_T1=(struct _tuple25*)_T3;}_T2=_T0(_T1,Cyc_Toc_add_anon_aggr_type_body);return _T2;}struct _tuple28{struct _tuple1*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _tuple29{struct Cyc_Toc_TocState*f0;struct _tuple28*f1;};struct _tuple30{struct _tuple1*f0;struct Cyc_List_List*f1;void*f2;};
# 549
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple29*env){struct _tuple29*_T0;struct Cyc_Toc_TocState*_T1;struct _tuple28*_T2;struct Cyc_List_List**_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;int _T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Kind*_TB;enum Cyc_Absyn_KindQual _TC;int _TD;void*_TE;void*_TF;int _T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;int _T13;void*_T14;struct Cyc_List_List*_T15;struct _fat_ptr*_T16;struct _fat_ptr _T17;struct Cyc_Int_pa_PrintArg_struct _T18;int _T19;int _T1A;void**_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;struct Cyc_List_List**_T1E;struct Cyc_List_List*_T1F;struct _RegionHandle*_T20;struct _tuple30*_T21;struct _RegionHandle*_T22;struct Cyc_List_List**_T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Absyn_Aggrfield*_T26;struct Cyc_Absyn_Aggrfield*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_Absyn_Kind*_T2A;struct Cyc_Absyn_Kind*_T2B;struct Cyc_Absyn_Kind*_T2C;int _T2D;int _T2E;struct Cyc_List_List*_T2F;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T30;void*_T31;int _T32;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T33;struct Cyc_Absyn_Exp*_T34;void*_T35;struct Cyc_List_List*_T36;struct Cyc_Absyn_Aggrfield*_T37;struct Cyc_Absyn_Aggrfield*_T38;struct Cyc_Absyn_Aggrfield*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T3C;void*_T3D;_T0=env;{
# 558
struct _tuple29 _T3E=*_T0;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct _tuple1*_T42;struct Cyc_List_List**_T43;_T1=_T3E.f0;{struct Cyc_Toc_TocState _T44=*_T1;_T43=_T44.abs_struct_types;}_T2=_T3E.f1;{struct _tuple28 _T44=*_T2;_T42=_T44.f0;_T41=_T44.f1;_T40=_T44.f2;_T3F=_T44.f3;}{struct Cyc_List_List**abs_struct_types=_T43;struct _tuple1*struct_name=_T42;struct Cyc_List_List*type_vars=_T41;struct Cyc_List_List*type_args=_T40;struct Cyc_List_List*fields=_T3F;_T3=abs_struct_types;{
# 562
struct Cyc_List_List*tts=*_T3;_TL58: if(tts!=0)goto _TL56;else{goto _TL57;}
_TL56: _T4=tts;_T5=_T4->hd;{struct _tuple30*_T44=(struct _tuple30*)_T5;void*_T45;struct Cyc_List_List*_T46;struct _tuple1*_T47;{struct _tuple30 _T48=*_T44;_T47=_T48.f0;_T46=_T48.f1;_T45=_T48.f2;}{struct _tuple1*x=_T47;struct Cyc_List_List*ts2=_T46;void*t=_T45;_T6=
Cyc_Absyn_qvar_cmp(x,struct_name);if(_T6!=0)goto _TL59;_T7=
Cyc_List_length(type_args);_T8=Cyc_List_length(ts2);
# 564
if(_T7!=_T8)goto _TL59;{
# 566
int okay=1;{
struct Cyc_List_List*ts=type_args;_TL5E: if(ts!=0)goto _TL5C;else{goto _TL5D;}
_TL5C: _T9=ts;{void*t=_T9->hd;_TA=
_check_null(ts2);{void*t2=_TA->hd;{
struct Cyc_Absyn_Kind*_T48=Cyc_Tcutil_type_kind(t);_TB=(struct Cyc_Absyn_Kind*)_T48;_TC=_TB->kind;if(_TC!=Cyc_Absyn_EffKind)goto _TL5F;goto _TL5B;_TL5F: _TD=
# 575
Cyc_Unify_unify(t,t2);if(_TD)goto _TL63;else{goto _TL64;}_TL64: _TE=Cyc_Toc_typ_to_c(t);_TF=Cyc_Toc_typ_to_c(t2);_T10=Cyc_Unify_unify(_TE,_TF);if(_T10)goto _TL63;else{goto _TL61;}
_TL63: goto _TL5B;_TL61:
 okay=0;goto _LL6;_LL6:;}goto _TL5D;}}_TL5B: _T11=ts;
# 567
ts=_T11->tl;_T12=ts2;ts2=_T12->tl;goto _TL5E;_TL5D:;}_T13=okay;
# 582
if(!_T13)goto _TL65;_T14=t;
return _T14;_TL65:;}goto _TL5A;_TL59: _TL5A:;}}_T15=tts;
# 562
tts=_T15->tl;goto _TL58;_TL57:;}{
# 588
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr));_T16=xname;{struct Cyc_Int_pa_PrintArg_struct _T44;_T44.tag=1;_T19=Cyc_Toc_tuple_type_counter;Cyc_Toc_tuple_type_counter=_T19 + 1;_T1A=_T19;_T44.f1=(unsigned long)_T1A;_T18=_T44;}{struct Cyc_Int_pa_PrintArg_struct _T44=_T18;void*_T45[1];_T1B=_T45 + 0;*_T1B=& _T44;_T1C=_tag_fat("_tuple%d",sizeof(char),9U);_T1D=_tag_fat(_T45,sizeof(void*),1);_T17=Cyc_aprintf(_T1C,_T1D);}*_T16=_T17;{
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*fs=0;_T1E=abs_struct_types;_T20=d;{struct Cyc_List_List*_T44=_region_malloc(_T20,0U,sizeof(struct Cyc_List_List));_T22=d;{struct _tuple30*_T45=_region_malloc(_T22,0U,sizeof(struct _tuple30));
# 592
_T45->f0=struct_name;_T45->f1=type_args;_T45->f2=x;_T21=(struct _tuple30*)_T45;}_T44->hd=_T21;_T23=abs_struct_types;
_T44->tl=*_T23;_T1F=(struct Cyc_List_List*)_T44;}
# 592
*_T1E=_T1F;{struct _RegionHandle _T44=_new_region(0U,"r");struct _RegionHandle*r=& _T44;_push_region(r);{
# 596
struct Cyc_List_List*inst=Cyc_List_rzip(r,r,type_vars,type_args);
_TL6A: if(fields!=0)goto _TL68;else{goto _TL69;}
_TL68: _T24=fields;_T25=_T24->hd;{struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_T25;_T26=f;{
void*t=_T26->type;_T27=f;{
struct Cyc_List_List*atts=_T27->attributes;_T28=fields;_T29=_T28->tl;
# 605
if(_T29!=0)goto _TL6B;_T2A=& Cyc_Kinds_ak;_T2B=(struct Cyc_Absyn_Kind*)_T2A;_T2C=
Cyc_Tcutil_type_kind(t);_T2D=Cyc_Kinds_kind_leq(_T2B,_T2C);
# 605
if(!_T2D)goto _TL6B;_T2E=
# 607
Cyc_Tcutil_is_array_type(t);
# 605
if(_T2E)goto _TL6B;else{goto _TL6D;}
# 608
_TL6D:{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T46=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_T46->tag=6;_T46->f1=0;_T30=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_T46;}_T45->hd=(void*)_T30;_T45->tl=atts;_T2F=(struct Cyc_List_List*)_T45;}atts=_T2F;goto _TL6C;_TL6B: _TL6C: _T31=
Cyc_Tcutil_rsubstitute(r,inst,t);t=Cyc_Toc_typ_to_c(_T31);_T32=
# 612
Cyc_Unify_unify(t,Cyc_Absyn_void_type);if(!_T32)goto _TL6E;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T45=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T45->tag=5;
_T45->f1.elt_type=Cyc_Toc_void_star_type();_T45->f1.tq=Cyc_Toc_mt_tq;_T34=
Cyc_Absyn_uint_exp(0U,0U);_T35=Cyc_Absyn_uint_type;_T45->f1.num_elts=Cyc_Absyn_set_type(_T34,_T35);
_T45->f1.zero_term=Cyc_Absyn_false_type;_T45->f1.zt_loc=0U;_T33=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T45;}
# 613
t=(void*)_T33;goto _TL6F;_TL6E: _TL6F:{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggrfield*_T46=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T38=f;
# 616
_T46->name=_T38->name;_T46->tq=Cyc_Toc_mt_tq;_T46->type=t;_T39=f;_T46->width=_T39->width;_T46->attributes=atts;_T46->requires_clause=0;_T37=(struct Cyc_Absyn_Aggrfield*)_T46;}_T45->hd=_T37;_T45->tl=fs;_T36=(struct Cyc_List_List*)_T45;}fs=_T36;}}}_T3A=fields;
# 597
fields=_T3A->tl;goto _TL6A;_TL69:
# 618
 fs=Cyc_List_imp_rev(fs);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T46=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T46->tag=5;
_T46->f1=sd;_T3C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T46;}_T3D=(void*)_T3C;_T45->hd=Cyc_Absyn_new_decl(_T3D,0U);_T45->tl=Cyc_Toc_result_decls;_T3B=(struct Cyc_List_List*)_T45;}Cyc_Toc_result_decls=_T3B;{void*_T45=x;_npop_handler(0);return _T45;}}}_pop_region();}}}}}}
# 624
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){struct _tuple28 _T0;void*(*_T1)(struct _tuple28*,void*(*)(struct _RegionHandle*,struct _tuple29*));struct _tuple28*_T2;void*_T3;{struct _tuple28 _T4;
# 628
_T4.f0=struct_name;_T4.f1=type_vars;_T4.f2=type_args;_T4.f3=fields;_T0=_T4;}{struct _tuple28 env=_T0;{
void*(*_T4)(struct _tuple28*,void*(*)(struct _RegionHandle*,struct _tuple29*))=(void*(*)(struct _tuple28*,void*(*)(struct _RegionHandle*,struct _tuple29*)))Cyc_Toc_use_toc_state;_T1=_T4;}_T2=& env;_T3=_T1(_T2,Cyc_Toc_add_struct_type_body);return _T3;}}
# 636
struct _tuple1*Cyc_Toc_temp_var (void){struct _tuple1*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;void**_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;{struct _tuple1*_T9=_cycalloc(sizeof(struct _tuple1));
_T9->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_TA=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TB;_TB.tag=1;_T4=Cyc_Toc_temp_var_counter;Cyc_Toc_temp_var_counter=_T4 + 1;_T5=_T4;_TB.f1=(unsigned)_T5;_T3=_TB;}{struct Cyc_Int_pa_PrintArg_struct _TB=_T3;void*_TC[1];_T6=_TC + 0;*_T6=& _TB;_T7=_tag_fat("_tmp%X",sizeof(char),7U);_T8=_tag_fat(_TC,sizeof(void*),1);_T2=Cyc_aprintf(_T7,_T8);}*_TA=_T2;_T1=(struct _fat_ptr*)_TA;}_T9->f1=_T1;_T0=(struct _tuple1*)_T9;}return _T0;}struct _tuple31{struct _tuple1*f0;struct Cyc_Absyn_Exp*f1;};
# 639
struct _tuple31 Cyc_Toc_temp_var_and_exp (void){struct _tuple31 _T0;
struct _tuple1*v=Cyc_Toc_temp_var();{struct _tuple31 _T1;
_T1.f0=v;_T1.f1=Cyc_Absyn_var_exp(v,0U);_T0=_T1;}return _T0;}struct _tuple32{struct Cyc_Toc_TocState*f0;int f1;};
# 646
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple32*env){struct _tuple32*_T0;struct Cyc_Toc_TocState*_T1;int _T2;int _T3;int _T4;struct _fat_ptr*(*_T5)(struct Cyc_Xarray_Xarray*,int);void*(*_T6)(struct Cyc_Xarray_Xarray*,int);struct _fat_ptr*_T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;struct Cyc_Int_pa_PrintArg_struct _TA;int _TB;void**_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;int(*_TF)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*);int(*_T10)(struct Cyc_Xarray_Xarray*,void*);int _T11;int _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;int(*_T15)(struct _fat_ptr ap);struct _fat_ptr _T16;struct _fat_ptr*_T17;_T0=env;{
struct _tuple32 _T18=*_T0;struct Cyc_Xarray_Xarray*_T19;_T1=_T18.f0;{struct Cyc_Toc_TocState _T1A=*_T1;_T19=_T1A.temp_labels;}{struct Cyc_Xarray_Xarray*temp_labels=_T19;_T2=Cyc_Toc_fresh_label_counter;
Cyc_Toc_fresh_label_counter=_T2 + 1;{int i=_T2;_T3=i;_T4=
Cyc_Xarray_length(temp_labels);if(_T3 >= _T4)goto _TL70;_T6=Cyc_Xarray_get;{
struct _fat_ptr*(*_T1A)(struct Cyc_Xarray_Xarray*,int)=(struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))_T6;_T5=_T1A;}_T7=_T5(temp_labels,i);return _T7;_TL70: {
struct _fat_ptr*res;res=_cycalloc(sizeof(struct _fat_ptr));_T8=res;{struct Cyc_Int_pa_PrintArg_struct _T1A;_T1A.tag=1;_TB=i;_T1A.f1=(unsigned)_TB;_TA=_T1A;}{struct Cyc_Int_pa_PrintArg_struct _T1A=_TA;void*_T1B[1];_TC=_T1B + 0;*_TC=& _T1A;_TD=_tag_fat("_LL%X",sizeof(char),6U);_TE=_tag_fat(_T1B,sizeof(void*),1);_T9=Cyc_aprintf(_TD,_TE);}*_T8=_T9;_T10=Cyc_Xarray_add_ind;{
int(*_T1A)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*)=(int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))_T10;_TF=_T1A;}_T11=_TF(temp_labels,res);_T12=i;if(_T11==_T12)goto _TL72;{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
_T1A.f1=_tag_fat("fresh_label: add_ind returned bad index...",sizeof(char),43U);_T13=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T13;void*_T1B[1];_T14=_T1B + 0;*_T14=& _T1A;{int(*_T1C)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T15=_T1C;}_T16=_tag_fat(_T1B,sizeof(void*),1);_T15(_T16);}goto _TL73;_TL72: _TL73: _T17=res;
return _T17;}}}}}
# 656
static struct _fat_ptr*Cyc_Toc_fresh_label (void){struct _fat_ptr*(*_T0)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple32*));struct _fat_ptr*_T1;{
struct _fat_ptr*(*_T2)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple32*))=(struct _fat_ptr*(*)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple32*)))Cyc_Toc_use_toc_state;_T0=_T2;}_T1=_T0(0,Cyc_Toc_fresh_label_body);return _T1;}
# 662
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;void*_T3;struct _tuple1*_T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_Absyn_Datatypefield*_T7;struct _tuple1*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Exp*_TB;void*_TC;struct Cyc_Absyn_Exp*_TD;
int ans=0;_T0=td;_T1=_T0->fields;_T2=
_check_null(_T1);_T3=_T2->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T3;_TL77: _T4=name;_T5=_check_null(fs);_T6=_T5->hd;_T7=(struct Cyc_Absyn_Datatypefield*)_T6;_T8=_T7->name;_T9=Cyc_Absyn_qvar_cmp(_T4,_T8);if(_T9!=0)goto _TL75;else{goto _TL76;}
_TL75: ans=ans + 1;_TA=fs;
# 664
fs=_TA->tl;goto _TL77;_TL76:;}_TB=
# 666
Cyc_Absyn_signed_int_exp(ans,0U);_TC=Cyc_Absyn_sint_type;_TD=Cyc_Absyn_set_type(_TB,_TC);return _TD;}
# 672
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*);
# 679
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){struct _tuple9*_T0;struct _tuple9*_T1;struct _tuple9 _T2;struct _tuple9*_T3;struct _tuple9 _T4;void*_T5;{struct _tuple9*_T6=_cycalloc(sizeof(struct _tuple9));
_T6->f0=0;_T1=a;_T2=*_T1;_T6->f1=_T2.f1;_T3=a;_T4=*_T3;_T5=_T4.f2;_T6->f2=Cyc_Toc_typ_to_c(_T5);_T0=(struct _tuple9*)_T6;}return _T0;}
# 698 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){int*_T0;int _T1;struct Cyc_Absyn_ArrayInfo _T2;void*_T3;void*_T4;struct Cyc_Absyn_ArrayInfo _T5;struct Cyc_Absyn_Tqual _T6;void*_T7;void*_T8;
void*_T9=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _TA;_T0=(int*)_T9;_T1=*_T0;if(_T1!=5)goto _TL78;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T9;_TA=_TB->f1;}{struct Cyc_Absyn_ArrayInfo ai=_TA;_T2=ai;_T3=_T2.elt_type;_T4=
Cyc_Toc_typ_to_c_array(_T3);_T5=ai;_T6=_T5.tq;_T7=Cyc_Absyn_cstar_type(_T4,_T6);return _T7;}_TL78: _T8=
Cyc_Toc_typ_to_c(t);return _T8;;}
# 705
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){struct Cyc_Absyn_Aggrfield*_T0;struct Cyc_Absyn_Aggrfield*_T1;struct Cyc_Absyn_Aggrfield*_T2;struct Cyc_Absyn_Aggrfield*_T3;struct Cyc_Absyn_Aggrfield*_T4;struct Cyc_Absyn_Aggrfield*_T5;
# 707
struct Cyc_Absyn_Aggrfield*ans;ans=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T0=ans;_T1=f;*_T0=*_T1;_T2=ans;
_T2->type=new_type;_T3=ans;
_T3->requires_clause=0;_T4=ans;
_T4->tq=Cyc_Toc_mt_tq;_T5=ans;
return _T5;}
# 714
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}
# 718
static int Cyc_Toc_is_boxed_tvar(void*t){int*_T0;int _T1;struct Cyc_Absyn_Kind*_T2;struct Cyc_Absyn_Kind*_T3;struct Cyc_Absyn_Kind*_T4;int _T5;
void*_T6=Cyc_Absyn_compress(t);struct Cyc_Absyn_Tvar*_T7;_T0=(int*)_T6;_T1=*_T0;if(_T1!=2)goto _TL7A;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6;_T7=_T8->f1;}{struct Cyc_Absyn_Tvar*tv=_T7;_T2=
Cyc_Tcutil_type_kind(t);_T3=& Cyc_Kinds_bk;_T4=(struct Cyc_Absyn_Kind*)_T3;_T5=Cyc_Kinds_kind_leq(_T2,_T4);return _T5;}_TL7A:
 return 0;;}
# 724
static int Cyc_Toc_is_abstract_type(void*t){struct Cyc_Absyn_Kind*_T0;enum Cyc_Absyn_KindQual _T1;
struct Cyc_Absyn_Kind*_T2=Cyc_Tcutil_type_kind(t);_T0=(struct Cyc_Absyn_Kind*)_T2;_T1=_T0->kind;if(_T1!=Cyc_Absyn_AnyKind)goto _TL7C;
return 1;_TL7C:
 return 0;;}
# 730
static int Cyc_Toc_is_void_star(void*t){int*_T0;int _T1;struct Cyc_Absyn_PtrInfo _T2;int _T3;
void*_T4=Cyc_Absyn_compress(t);void*_T5;_T0=(int*)_T4;_T1=*_T0;if(_T1!=4)goto _TL7E;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4;_T2=_T6->f1;_T5=_T2.elt_type;}{void*t2=_T5;_T3=
Cyc_Tcutil_is_void_type(t2);return _T3;}_TL7E:
 return 0;;}
# 736
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){int _T0;int _T1;_T1=
Cyc_Toc_is_void_star(t);if(!_T1)goto _TL80;_T0=1;goto _TL81;_TL80: _T0=Cyc_Toc_is_boxed_tvar(t);_TL81: return _T0;}
# 739
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){int _T0;int _T1;_T1=
Cyc_Tcutil_is_pointer_type(t);if(!_T1)goto _TL82;_T0=1;goto _TL83;_TL82: _T0=Cyc_Toc_is_boxed_tvar(t);_TL83: return _T0;}
# 743
void*Cyc_Toc_typ_to_c(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TD;union Cyc_Absyn_DatatypeFieldInfo _TE;struct _union_DatatypeFieldInfo_KnownDatatypefield _TF;unsigned _T10;void*_T11;void*_T12;union Cyc_Absyn_DatatypeFieldInfo _T13;struct _union_DatatypeFieldInfo_KnownDatatypefield _T14;struct _tuple2 _T15;union Cyc_Absyn_DatatypeFieldInfo _T16;struct _union_DatatypeFieldInfo_KnownDatatypefield _T17;struct _tuple2 _T18;struct Cyc_Absyn_Datatypefield*_T19;struct _tuple1*_T1A;struct Cyc_Absyn_Datatypedecl*_T1B;struct _tuple1*_T1C;struct _tuple1*_T1D;void*_T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;void**_T20;int(*_T21)(struct _fat_ptr ap);struct _fat_ptr _T22;void*_T23;void*_T24;void*_T25;union Cyc_Absyn_AggrInfo _T26;struct _union_AggrInfo_UnknownAggr _T27;unsigned _T28;void*_T29;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2A;struct Cyc_Absyn_Aggrdecl*_T2B;int _T2C;struct Cyc_Absyn_Aggrdecl*_T2D;struct Cyc_Absyn_AggrdeclImpl*_T2E;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;void**_T31;void**_T32;struct _fat_ptr _T33;struct Cyc_Absyn_Aggrdecl*_T34;enum Cyc_Absyn_AggrKind _T35;int _T36;struct Cyc_Absyn_Aggrdecl*_T37;struct _tuple1*_T38;void*(*_T39)(struct _tuple1*);void*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Aggrdecl*_T3C;struct Cyc_Absyn_AggrdeclImpl*_T3D;struct Cyc_Absyn_Aggrdecl*_T3E;struct Cyc_Absyn_AggrdeclImpl*_T3F;struct Cyc_Absyn_Aggrdecl*_T40;struct _tuple1*_T41;void*(*_T42)(struct _tuple1*);void*_T43;struct Cyc_List_List*_T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_Absyn_Aggrfield*_T49;struct Cyc_Absyn_Kind*_T4A;struct Cyc_Absyn_Kind*_T4B;struct Cyc_Absyn_Kind*_T4C;int _T4D;struct Cyc_Absyn_Aggrdecl*_T4E;int _T4F;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T50;struct Cyc_Warn_String_Warn_Warg_struct _T51;void**_T52;void**_T53;struct _fat_ptr _T54;struct _RegionHandle*_T55;struct _RegionHandle*_T56;struct Cyc_Absyn_Aggrdecl*_T57;struct Cyc_List_List*_T58;struct Cyc_List_List*_T59;int _T5A;struct Cyc_Absyn_Aggrdecl*_T5B;struct _tuple1*_T5C;void*(*_T5D)(struct _tuple1*);struct Cyc_Absyn_Aggrdecl*_T5E;struct _tuple1*_T5F;struct Cyc_Absyn_Aggrdecl*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List*_T62;struct Cyc_Absyn_Aggrdecl*_T63;struct Cyc_Absyn_AggrdeclImpl*_T64;struct Cyc_List_List*_T65;struct Cyc_Absyn_Aggrdecl*_T66;struct _tuple1*_T67;void*(*_T68)(struct _tuple1*);void*_T69;void*_T6A;void*_T6B;void*_T6C;void*_T6D;void*_T6E;void*_T6F;void*_T70;void*_T71;void*_T72;void*_T73;void*_T74;void*_T75;void*_T76;void*_T77;void*_T78;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T79;void**_T7A;void**_T7B;void*_T7C;void**_T7D;void*_T7E;void*_T7F;void**_T80;void*_T81;void*_T82;struct Cyc_Absyn_Tvar*_T83;struct Cyc_Absyn_Kind*_T84;struct Cyc_Absyn_Kind*_T85;struct Cyc_Absyn_Kind*_T86;enum Cyc_Absyn_KindQual _T87;int _T88;void*_T89;void*_T8A;void*_T8B;struct Cyc_Absyn_PtrInfo _T8C;struct Cyc_Absyn_PtrInfo _T8D;struct Cyc_Absyn_PtrInfo _T8E;struct Cyc_Absyn_PtrAtts _T8F;void*_T90;void*_T91;struct Cyc_Absyn_Exp*_T92;void*_T93;void*_T94;void*_T95;struct Cyc_Absyn_ArrayInfo _T96;struct Cyc_Absyn_ArrayInfo _T97;struct Cyc_Absyn_ArrayInfo _T98;struct Cyc_Absyn_ArrayInfo _T99;void*_T9A;struct Cyc_Absyn_Tqual _T9B;struct Cyc_Absyn_Exp*_T9C;void*_T9D;unsigned _T9E;void*_T9F;void*_TA0;struct Cyc_Absyn_FnInfo _TA1;struct Cyc_Absyn_FnInfo _TA2;struct Cyc_Absyn_FnInfo _TA3;struct Cyc_Absyn_FnInfo _TA4;struct Cyc_Absyn_FnInfo _TA5;struct Cyc_Absyn_FnInfo _TA6;struct Cyc_List_List*(*_TA7)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*);struct Cyc_List_List*(*_TA8)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_TA9;struct Cyc_Absyn_VarargInfo*_TAA;void*_TAB;void*_TAC;void*_TAD;struct Cyc_Absyn_Tqual _TAE;void*_TAF;void*_TB0;void*_TB1;struct _tuple9*_TB2;struct Cyc_Absyn_VarargInfo*_TB3;struct _tuple9*_TB4;struct Cyc_Absyn_VarargInfo*_TB5;struct _tuple9*_TB6;struct Cyc_List_List*_TB7;struct Cyc_List_List*_TB8;struct Cyc_Absyn_FnType_Absyn_Type_struct*_TB9;void*_TBA;void*_TBB;struct Cyc_List_List*_TBC;struct Cyc_List_List*_TBD;void*_TBE;struct Cyc_Absyn_Aggrfield*_TBF;struct Cyc_List_List*_TC0;void*_TC1;struct Cyc_Absyn_Aggrfield*_TC2;void*_TC3;void*_TC4;struct Cyc_List_List*_TC5;enum Cyc_Absyn_AggrKind _TC6;struct Cyc_List_List*_TC7;void*_TC8;void*_TC9;void*_TCA;void*_TCB;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TCC;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TCD;void*_TCE;void*_TCF;void*_TD0;void*_TD1;void*_TD2;struct Cyc_Absyn_Exp*_TD3;void*_TD4;struct Cyc_Absyn_Exp*_TD5;void*_TD6;void*_TD7;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD8;struct Cyc_Absyn_TypeDecl*_TD9;struct Cyc_Absyn_TypeDecl*_TDA;void*_TDB;int*_TDC;unsigned _TDD;void*_TDE;struct Cyc_Absyn_TypeDecl*_TDF;void*_TE0;struct Cyc_Absyn_Aggrdecl*_TE1;struct _tuple1*_TE2;void*(*_TE3)(struct _tuple1*);struct Cyc_Absyn_Aggrdecl*_TE4;enum Cyc_Absyn_AggrKind _TE5;int _TE6;void*_TE7;void*_TE8;struct Cyc_Absyn_TypeDecl*_TE9;void*_TEA;void*_TEB;void*_TEC;struct Cyc_Absyn_TypeDecl*_TED;void*_TEE;void**_TEF;void*_TF0;void*_TF1;void**_TF2;struct Cyc_Absyn_Datatypedecl*_TF3;struct Cyc_Absyn_Enumdecl*_TF4;struct Cyc_Absyn_Aggrdecl*_TF5;struct Cyc_Absyn_Typedefdecl*_TF6;struct _tuple1*_TF7;enum Cyc_Absyn_AggrKind _TF8;struct Cyc_List_List*_TF9;struct Cyc_Absyn_VarargInfo*_TFA;int _TFB;struct Cyc_List_List*_TFC;unsigned _TFD;struct Cyc_Absyn_Exp*_TFE;struct Cyc_Absyn_Tqual _TFF;union Cyc_Absyn_AggrInfo _T100;void*_T101;void*_T102;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 0: _T8=t;
# 745
return _T8;case 22: _T9=Cyc_Absyn_void_type;
# 755
return _T9;case 23: _TA=t;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TC;_TE=_TD->f1;_TF=_TE.KnownDatatypefield;_T10=_TF.tag;if(_T10!=2)goto _TL86;_T11=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T12=_T103->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T104=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T12;_T13=_T104->f1;_T14=_T13.KnownDatatypefield;_T15=_T14.val;_T102=_T15.f0;_T16=_T104->f1;_T17=_T16.KnownDatatypefield;_T18=_T17.val;_T101=_T18.f1;}}{struct Cyc_Absyn_Datatypedecl*tud=_T102;struct Cyc_Absyn_Datatypefield*tuf=_T101;_T19=tuf;_T1A=_T19->name;_T1B=tud;_T1C=_T1B->name;_T1D=
# 757
Cyc_Toc_collapse_qvars(_T1A,_T1C);_T1E=Cyc_Absyn_strctq(_T1D);return _T1E;}_TL86:{struct Cyc_Warn_String_Warn_Warg_struct _T103;_T103.tag=0;
# 759
_T103.f1=_tag_fat("unresolved DatatypeFieldType",sizeof(char),29U);_T1F=_T103;}{struct Cyc_Warn_String_Warn_Warg_struct _T103=_T1F;void*_T104[1];_T20=_T104 + 0;*_T20=& _T103;{int(*_T105)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T21=_T105;}_T22=_tag_fat(_T104,sizeof(void*),1);_T21(_T22);};case 1: goto _LL14;case 2: _LL14: _T23=t;
# 768
return _T23;case 24: _T24=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24;_T25=_T103->f1;_T102=(void*)_T25;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T104=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T102;_T100=_T104->f1;}_T101=_T103->f2;}{void*c=_T102;union Cyc_Absyn_AggrInfo info=_T100;struct Cyc_List_List*ts=_T101;_T26=info;_T27=_T26.UnknownAggr;_T28=_T27.tag;if(_T28!=1)goto _TL88;
# 795
if(ts!=0)goto _TL8A;_T29=t;goto _TL8B;_TL8A:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T103=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T103->tag=0;_T103->f1=c;_T103->f2=0;_T2A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T103;}_T29=(void*)_T2A;_TL8B: return _T29;_TL88: goto _LL39;_LL39: {
# 798
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T2B=ad;_T2C=_T2B->expected_mem_kind;
# 800
if(!_T2C)goto _TL8C;_T2D=ad;_T2E=_T2D->impl;if(_T2E!=0)goto _TL8C;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T103;_T103.tag=6;
_T103.f1=ad;_T2F=_T103;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T103=_T2F;{struct Cyc_Warn_String_Warn_Warg_struct _T104;_T104.tag=0;_T104.f1=_tag_fat(" was never defined.",sizeof(char),20U);_T30=_T104;}{struct Cyc_Warn_String_Warn_Warg_struct _T104=_T30;void*_T105[2];_T31=_T105 + 0;*_T31=& _T103;_T32=_T105 + 1;*_T32=& _T104;_T33=_tag_fat(_T105,sizeof(void*),2);Cyc_Warn_warn2(0U,_T33);}}goto _TL8D;_TL8C: _TL8D: _T34=ad;_T35=_T34->kind;_T36=(int)_T35;
# 803
if(_T36!=1)goto _TL8E;_T37=ad;_T38=_T37->name;_T39=Cyc_Absyn_unionq_type;_T3A=
Cyc_Toc_aggrdecl_type(_T38,_T39);return _T3A;_TL8E: _T3C=ad;_T3D=_T3C->impl;
if(_T3D!=0)goto _TL90;_T3B=0;goto _TL91;_TL90: _T3E=ad;_T3F=_T3E->impl;_T3B=_T3F->fields;_TL91: {struct Cyc_List_List*fs=_T3B;
if(fs!=0)goto _TL92;_T40=ad;_T41=_T40->name;_T42=Cyc_Absyn_strctq;_T43=Cyc_Toc_aggrdecl_type(_T41,_T42);return _T43;_TL92:
 _TL97: _T44=fs;_T45=_T44->tl;if(_T45!=0)goto _TL95;else{goto _TL96;}_TL95: _T46=fs;fs=_T46->tl;goto _TL97;_TL96: _T47=fs;_T48=_T47->hd;_T49=(struct Cyc_Absyn_Aggrfield*)_T48;{
void*last_type=_T49->type;_T4A=& Cyc_Kinds_ak;_T4B=(struct Cyc_Absyn_Kind*)_T4A;_T4C=
Cyc_Tcutil_type_kind(last_type);_T4D=Cyc_Kinds_kind_leq(_T4B,_T4C);if(!_T4D)goto _TL98;_T4E=ad;_T4F=_T4E->expected_mem_kind;
if(!_T4F)goto _TL9A;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T103;_T103.tag=6;
_T103.f1=ad;_T50=_T103;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T103=_T50;{struct Cyc_Warn_String_Warn_Warg_struct _T104;_T104.tag=0;_T104.f1=_tag_fat(" ended up being abstract.",sizeof(char),26U);_T51=_T104;}{struct Cyc_Warn_String_Warn_Warg_struct _T104=_T51;void*_T105[2];_T52=_T105 + 0;*_T52=& _T103;_T53=_T105 + 1;*_T53=& _T104;_T54=_tag_fat(_T105,sizeof(void*),2);Cyc_Warn_warn2(0U,_T54);}}goto _TL9B;_TL9A: _TL9B:{struct _RegionHandle _T103=_new_region(0U,"r");struct _RegionHandle*r=& _T103;_push_region(r);_T55=r;_T56=r;_T57=ad;_T58=_T57->tvs;_T59=ts;{
# 815
struct Cyc_List_List*inst=Cyc_List_rzip(_T55,_T56,_T58,_T59);
void*t=Cyc_Tcutil_rsubstitute(r,inst,last_type);_T5A=
Cyc_Toc_is_abstract_type(t);if(!_T5A)goto _TL9C;_T5B=ad;_T5C=_T5B->name;_T5D=Cyc_Absyn_strctq;{void*_T104=Cyc_Toc_aggrdecl_type(_T5C,_T5D);_npop_handler(0);return _T104;}_TL9C: _T5E=ad;_T5F=_T5E->name;_T60=ad;_T61=_T60->tvs;_T62=ts;_T63=ad;_T64=_T63->impl;_T65=_T64->fields;{void*_T104=
Cyc_Toc_add_struct_type(_T5F,_T61,_T62,_T65);_npop_handler(0);return _T104;}}_pop_region();}goto _TL99;_TL98: _TL99: _T66=ad;_T67=_T66->name;_T68=Cyc_Absyn_strctq;_T69=
# 820
Cyc_Toc_aggrdecl_type(_T67,_T68);return _T69;}}}}case 19: _T6A=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6A;_T6B=_T103->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T104=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T6B;_T102=_T104->f1;}}{struct _tuple1*tdn=_T102;_T6C=t;
return _T6C;}case 20: _T6D=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6D;_T6E=_T103->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_T104=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T6E;_T102=_T104->f1;}}{struct Cyc_List_List*fs=_T102;
Cyc_Toc_enumfields_to_c(fs);_T6F=t;return _T6F;}case 5: _T70=Cyc_Absyn_uint_type;
# 829
return _T70;case 4: _T71=
Cyc_Toc_rgn_type();return _T71;case 18: _T72=
Cyc_Toc_aqualt_type();return _T72;case 21: _T73=t;
return _T73;case 3: _T74=t;
return _T74;default: _T75=
# 836
Cyc_Toc_void_star_type();return _T75;};case 3: _T76=
# 746
Cyc_Toc_void_star_type();return _T76;case 1: _T77=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T103=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T77;_T78=t;_T79=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T78;_T7A=& _T79->f2;_T102=(void**)_T7A;}{void**t2=_T102;_T7B=t2;_T7C=*_T7B;
# 748
if(_T7C==0)goto _TL9E;_T7D=t2;_T7E=*_T7D;_T7F=
Cyc_Toc_typ_to_c(_T7E);return _T7F;_TL9E: _T80=t2;
*_T80=Cyc_Toc_void_star_type();_T81=*_T80;return _T81;}case 2: _T82=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T82;_T102=_T103->f1;}{struct Cyc_Absyn_Tvar*tv=_T102;_T83=tv;_T84=& Cyc_Kinds_bk;_T85=(struct Cyc_Absyn_Kind*)_T84;_T86=
# 752
Cyc_Kinds_tvar_kind(_T83,_T85);_T87=_T86->kind;_T88=(int)_T87;if(_T88!=0)goto _TLA0;_T89=Cyc_Absyn_void_type;
return _T89;_TLA0: _T8A=
Cyc_Toc_void_star_type();return _T8A;}case 4: _T8B=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T8B;_T8C=_T103->f1;_T102=_T8C.elt_type;_T8D=_T103->f1;_TFF=_T8D.elt_tq;_T8E=_T103->f1;_T8F=_T8E.ptr_atts;_T101=_T8F.bounds;}{void*t2=_T102;struct Cyc_Absyn_Tqual tq=_TFF;void*bnds=_T101;
# 763
t2=Cyc_Toc_typ_to_c(t2);_T90=
Cyc_Absyn_bounds_one();_T91=bnds;_T92=Cyc_Tcutil_get_bounds_exp_constrain(_T90,_T91,1);if(_T92!=0)goto _TLA2;_T93=
Cyc_Toc_fat_ptr_type();return _T93;_TLA2: _T94=
Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);return _T94;}case 5: _T95=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T95;_T96=_T103->f1;_T102=_T96.elt_type;_T97=_T103->f1;_TFF=_T97.tq;_T98=_T103->f1;_TFE=_T98.num_elts;_T99=_T103->f1;_TFD=_T99.zt_loc;}{void*t2=_T102;struct Cyc_Absyn_Tqual tq=_TFF;struct Cyc_Absyn_Exp*e=_TFE;unsigned ztl=_TFD;_T9A=
# 770
Cyc_Toc_typ_to_c(t2);_T9B=tq;_T9C=e;_T9D=Cyc_Absyn_false_type;_T9E=ztl;_T9F=Cyc_Absyn_array_type(_T9A,_T9B,_T9C,_T9D,_T9E);return _T9F;}case 6: _TA0=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TA0;_TA1=_T103->f1;_TFF=_TA1.ret_tqual;_TA2=_T103->f1;_T102=_TA2.ret_type;_TA3=_T103->f1;_TFC=_TA3.args;_TA4=_T103->f1;_TFB=_TA4.c_varargs;_TA5=_T103->f1;_TFA=_TA5.cyc_varargs;_TA6=_T103->f1;_TF9=_TA6.attributes;}{struct Cyc_Absyn_Tqual tq2=_TFF;void*t2=_T102;struct Cyc_List_List*args=_TFC;int c_vararg=_TFB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TFA;struct Cyc_List_List*atts=_TF9;
# 774
struct Cyc_List_List*new_atts=Cyc_Atts_atts2c(atts);_TA8=Cyc_List_map;{
struct Cyc_List_List*(*_T103)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))_TA8;_TA7=_T103;}_TA9=args;{struct Cyc_List_List*new_args=_TA7(Cyc_Toc_arg_to_c,_TA9);
if(cyc_vararg==0)goto _TLA4;_TAA=cyc_vararg;_TAB=_TAA->type;_TAC=Cyc_Absyn_heap_rgn_type;_TAD=Cyc_Absyn_al_qual_type;_TAE=Cyc_Toc_mt_tq;_TAF=Cyc_Absyn_false_type;_TB0=Cyc_Absyn_false_type;_TB1=
# 778
Cyc_Absyn_fatptr_type(_TAB,_TAC,_TAD,_TAE,_TAF,_TB0);{void*t=Cyc_Toc_typ_to_c(_TB1);
struct _tuple9*vararg;vararg=_cycalloc(sizeof(struct _tuple9));_TB2=vararg;_TB3=cyc_vararg;_TB2->f0=_TB3->name;_TB4=vararg;_TB5=cyc_vararg;_TB4->f1=_TB5->tq;_TB6=vararg;_TB6->f2=t;_TB7=new_args;{struct Cyc_List_List*_T103=_cycalloc(sizeof(struct Cyc_List_List));
_T103->hd=vararg;_T103->tl=0;_TB8=(struct Cyc_List_List*)_T103;}new_args=Cyc_List_imp_append(_TB7,_TB8);}goto _TLA5;_TLA4: _TLA5:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T103=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T103->tag=6;
# 782
_T103->f1.tvars=0;_T103->f1.effect=0;_T103->f1.ret_tqual=tq2;_T103->f1.ret_type=Cyc_Toc_typ_to_c(t2);_T103->f1.args=new_args;
_T103->f1.c_varargs=c_vararg;_T103->f1.cyc_varargs=0;_T103->f1.qual_bnd=0;_T103->f1.attributes=new_atts;_T103->f1.checks_clause=0;_T103->f1.checks_assn=0;_T103->f1.requires_clause=0;_T103->f1.requires_assn=0;_T103->f1.ensures_clause=0;_T103->f1.ensures_assn=0;_T103->f1.throws_clause=0;_T103->f1.throws_assn=0;_T103->f1.return_value=0;_T103->f1.arg_vardecls=0;_T103->f1.effconstr=0;_TB9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T103;}_TBA=(void*)_TB9;
# 782
return _TBA;}}case 7: _TBB=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_TBB;_TF8=_T103->f1;_TFC=_T103->f3;}{enum Cyc_Absyn_AggrKind k=_TF8;struct Cyc_List_List*fs=_TFC;
# 787
struct Cyc_List_List*fs2=0;
_TLA9: if(fs!=0)goto _TLA7;else{goto _TLA8;}
_TLA7:{struct Cyc_List_List*_T103=_cycalloc(sizeof(struct Cyc_List_List));_TBD=fs;_TBE=_TBD->hd;_TBF=(struct Cyc_Absyn_Aggrfield*)_TBE;_TC0=fs;_TC1=_TC0->hd;_TC2=(struct Cyc_Absyn_Aggrfield*)_TC1;_TC3=_TC2->type;_TC4=Cyc_Toc_typ_to_c(_TC3);_T103->hd=Cyc_Toc_aggrfield_to_c(_TBF,_TC4);_T103->tl=fs2;_TBC=(struct Cyc_List_List*)_T103;}fs2=_TBC;_TC5=fs;
# 788
fs=_TC5->tl;goto _TLA9;_TLA8: _TC6=k;_TC7=
# 790
Cyc_List_imp_rev(fs2);_TC8=Cyc_Toc_add_anon_aggr_type(_TC6,_TC7);return _TC8;}case 8: _TC9=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TC9;_TF7=_T103->f1;_TFC=_T103->f2;_TF6=_T103->f3;_TCA=_T103->f4;_T102=(void*)_TCA;}{struct _tuple1*tdn=_TF7;struct Cyc_List_List*ts=_TFC;struct Cyc_Absyn_Typedefdecl*td=_TF6;void*topt=_T102;
# 826
if(topt!=0)goto _TLAA;
if(ts!=0)goto _TLAC;_TCB=t;goto _TLAD;_TLAC:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T103=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_T103->tag=8;_T103->f1=tdn;_T103->f2=0;_T103->f3=td;_T103->f4=0;_TCC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T103;}_TCB=(void*)_TCC;_TLAD: return _TCB;_TLAA:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T103=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_T103->tag=8;
_T103->f1=tdn;_T103->f2=0;_T103->f3=td;_T103->f4=Cyc_Toc_typ_to_c(topt);_TCD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T103;}_TCE=(void*)_TCD;return _TCE;}case 9: _TCF=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TCF;_TFE=_T103->f1;}{struct Cyc_Absyn_Exp*e=_TFE;_TD0=t;
# 841
return _TD0;}case 11: _TD1=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_TD1;_TFE=_T103->f1;}{struct Cyc_Absyn_Exp*e=_TFE;
# 846
Cyc_Toc_exptypes_to_c(e);_TD3=e;_TD4=_TD3->topt;
if(_TD4!=0)goto _TLAE;_TD2=t;goto _TLAF;_TLAE: _TD5=e;_TD6=_TD5->topt;_TD2=Cyc_Toc_typ_to_c(_TD6);_TLAF: return _TD2;}default: _TD7=t;_TD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_TD7;_TD9=_TD8->f1;_TDA=(struct Cyc_Absyn_TypeDecl*)_TD9;_TDB=_TDA->r;_TDC=(int*)_TDB;_TDD=*_TDC;switch(_TDD){case 0: _TDE=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_TDE;_TDF=_T103->f1;{struct Cyc_Absyn_TypeDecl _T104=*_TDF;_TE0=_T104.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T105=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_TE0;_TF5=_T105->f1;}}}{struct Cyc_Absyn_Aggrdecl*ad=_TF5;
# 849
Cyc_Toc_aggrdecl_to_c(ad);_TE1=ad;_TE2=_TE1->name;_TE4=ad;_TE5=_TE4->kind;_TE6=(int)_TE5;
if(_TE6!=1)goto _TLB1;_TE3=Cyc_Absyn_unionq_type;goto _TLB2;_TLB1: _TE3=Cyc_Absyn_strctq;_TLB2: _TE7=Cyc_Toc_aggrdecl_type(_TE2,_TE3);return _TE7;}case 1: _TE8=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_TE8;_TE9=_T103->f1;{struct Cyc_Absyn_TypeDecl _T104=*_TE9;_TEA=_T104.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T105=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_TEA;_TF4=_T105->f1;}}}{struct Cyc_Absyn_Enumdecl*ed=_TF4;
# 852
Cyc_Toc_enumdecl_to_c(ed);_TEB=t;
return _TEB;}default: _TEC=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T103=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_TEC;_TED=_T103->f1;{struct Cyc_Absyn_TypeDecl _T104=*_TED;_TEE=_T104.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T105=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_TEE;_TF3=_T105->f1;}}_TF2=_T103->f2;}{struct Cyc_Absyn_Datatypedecl*dd=_TF3;void**t=_TF2;
# 855
Cyc_Toc_datatypedecl_to_c(dd);_TEF=
_check_null(t);_TF0=*_TEF;_TF1=Cyc_Toc_typ_to_c(_TF0);return _TF1;}};};}
# 860
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_ArrayInfo _T4;struct Cyc_Absyn_ArrayInfo _T5;void*_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Tqual _TA;void*_TB;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=5)goto _TLB3;_T3=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3;_T4=_TC->f1;_TB=_T4.elt_type;_T5=_TC->f1;_TA=_T5.tq;}{void*t2=_TB;struct Cyc_Absyn_Tqual tq=_TA;_T6=
# 863
Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);_T7=e;_T8=Cyc_Toc_cast_it(_T6,_T7);return _T8;}_TLB3: _T9=
Cyc_Toc_cast_it(t,e);return _T9;;}
# 870
static int Cyc_Toc_atomic_type(void*t){int*_T0;unsigned _T1;void*_T2;void*_T3;int*_T4;unsigned _T5;void*_T6;union Cyc_Absyn_AggrInfo _T7;struct _union_AggrInfo_UnknownAggr _T8;unsigned _T9;struct Cyc_Absyn_Aggrdecl*_TA;struct Cyc_Absyn_AggrdeclImpl*_TB;struct Cyc_Absyn_Aggrdecl*_TC;struct Cyc_Absyn_AggrdeclImpl*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Absyn_Aggrfield*_T10;void*_T11;int _T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T15;union Cyc_Absyn_DatatypeFieldInfo _T16;struct _union_DatatypeFieldInfo_KnownDatatypefield _T17;unsigned _T18;void*_T19;union Cyc_Absyn_DatatypeFieldInfo _T1A;struct _union_DatatypeFieldInfo_KnownDatatypefield _T1B;struct _tuple2 _T1C;union Cyc_Absyn_DatatypeFieldInfo _T1D;struct _union_DatatypeFieldInfo_KnownDatatypefield _T1E;struct _tuple2 _T1F;struct Cyc_Absyn_Datatypefield*_T20;struct Cyc_List_List*_T21;void*_T22;struct _tuple24*_T23;struct _tuple24 _T24;void*_T25;int _T26;struct Cyc_List_List*_T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;struct Cyc_Warn_Typ_Warn_Warg_struct _T29;void**_T2A;void**_T2B;int(*_T2C)(struct _fat_ptr ap);struct _fat_ptr _T2D;struct Cyc_Absyn_ArrayInfo _T2E;int _T2F;struct Cyc_List_List*_T30;void*_T31;struct Cyc_Absyn_Aggrfield*_T32;void*_T33;int _T34;struct Cyc_List_List*_T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Typ_Warn_Warg_struct _T37;void**_T38;void**_T39;int(*_T3A)(struct _fat_ptr ap);struct _fat_ptr _T3B;
void*_T3C=Cyc_Absyn_compress(t);struct Cyc_List_List*_T3D;void*_T3E;_T0=(int*)_T3C;_T1=*_T0;switch(_T1){case 2:
 return 0;case 0:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C;_T2=_T3F->f1;_T3E=(void*)_T2;_T3D=_T3F->f2;}{void*c=_T3E;struct Cyc_List_List*ts=_T3D;struct Cyc_Absyn_Datatypefield*_T3F;struct Cyc_Absyn_Datatypedecl*_T40;union Cyc_Absyn_AggrInfo _T41;_T3=c;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 0: goto _LL13;case 1: _LL13: goto _LL15;case 2: _LL15: goto _LL17;case 5: _LL17: goto _LL19;case 19: _LL19: goto _LL1B;case 20: _LL1B:
# 876
 return 1;case 22: goto _LL1F;case 4: _LL1F: goto _LL21;case 21: _LL21:
# 879
 return 0;case 24: _T6=c;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T42=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T41=_T42->f1;}{union Cyc_Absyn_AggrInfo info=_T41;_T7=info;_T8=_T7.UnknownAggr;_T9=_T8.tag;if(_T9!=1)goto _TLB7;
# 887
return 0;_TLB7: goto _LL28;_LL28: {
# 890
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_TA=ad;_TB=_TA->impl;
if(_TB!=0)goto _TLB9;
return 0;_TLB9: _TC=ad;_TD=_TC->impl;{
struct Cyc_List_List*fs=_TD->fields;_TLBE: if(fs!=0)goto _TLBC;else{goto _TLBD;}
_TLBC: _TE=fs;_TF=_TE->hd;_T10=(struct Cyc_Absyn_Aggrfield*)_TF;_T11=_T10->type;_T12=Cyc_Toc_atomic_type(_T11);if(_T12)goto _TLBF;else{goto _TLC1;}_TLC1: return 0;_TLBF: _T13=fs;
# 893
fs=_T13->tl;goto _TLBE;_TLBD:;}
# 895
return 1;}}case 23: _T14=c;_T15=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T14;_T16=_T15->f1;_T17=_T16.KnownDatatypefield;_T18=_T17.tag;if(_T18!=2)goto _TLC2;_T19=c;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T42=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T19;_T1A=_T42->f1;_T1B=_T1A.KnownDatatypefield;_T1C=_T1B.val;_T40=_T1C.f0;_T1D=_T42->f1;_T1E=_T1D.KnownDatatypefield;_T1F=_T1E.val;_T3F=_T1F.f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T40;struct Cyc_Absyn_Datatypefield*tuf=_T3F;_T20=tuf;{
# 897
struct Cyc_List_List*tqs=_T20->typs;_TLC7: if(tqs!=0)goto _TLC5;else{goto _TLC6;}
_TLC5: _T21=tqs;_T22=_T21->hd;_T23=(struct _tuple24*)_T22;_T24=*_T23;_T25=_T24.f1;_T26=Cyc_Toc_atomic_type(_T25);if(_T26)goto _TLC8;else{goto _TLCA;}_TLCA: return 0;_TLC8: _T27=tqs;
# 897
tqs=_T27->tl;goto _TLC7;_TLC6:;}
# 899
return 1;}_TLC2: goto _LL26;default: _LL26:{struct Cyc_Warn_String_Warn_Warg_struct _T42;_T42.tag=0;
_T42.f1=_tag_fat("atomic_typ: bad type ",sizeof(char),22U);_T28=_T42;}{struct Cyc_Warn_String_Warn_Warg_struct _T42=_T28;{struct Cyc_Warn_Typ_Warn_Warg_struct _T43;_T43.tag=2;_T43.f1=t;_T29=_T43;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T43=_T29;void*_T44[2];_T2A=_T44 + 0;*_T2A=& _T42;_T2B=_T44 + 1;*_T2B=& _T43;{int(*_T45)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T2C=_T45;}_T2D=_tag_fat(_T44,sizeof(void*),2);_T2C(_T2D);}}};}case 6:
# 902
 return 1;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3C;_T2E=_T3F->f1;_T3E=_T2E.elt_type;}{void*t=_T3E;_T2F=
Cyc_Toc_atomic_type(t);return _T2F;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T3F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T3C;_T3D=_T3F->f3;}{struct Cyc_List_List*fs=_T3D;
# 905
_TLCE: if(fs!=0)goto _TLCC;else{goto _TLCD;}
_TLCC: _T30=fs;_T31=_T30->hd;_T32=(struct Cyc_Absyn_Aggrfield*)_T31;_T33=_T32->type;_T34=Cyc_Toc_atomic_type(_T33);if(_T34)goto _TLCF;else{goto _TLD1;}_TLD1: return 0;_TLCF: _T35=fs;
# 905
fs=_T35->tl;goto _TLCE;_TLCD:
# 907
 return 1;}case 4:
 return 0;default:{struct Cyc_Warn_String_Warn_Warg_struct _T3F;_T3F.tag=0;
_T3F.f1=_tag_fat("atomic_typ:  bad type ",sizeof(char),23U);_T36=_T3F;}{struct Cyc_Warn_String_Warn_Warg_struct _T3F=_T36;{struct Cyc_Warn_Typ_Warn_Warg_struct _T40;_T40.tag=2;_T40.f1=t;_T37=_T40;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T40=_T37;void*_T41[2];_T38=_T41 + 0;*_T38=& _T3F;_T39=_T41 + 1;*_T39=& _T40;{int(*_T42)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T3A=_T42;}_T3B=_tag_fat(_T41,sizeof(void*),2);_T3A(_T3B);}}};}
# 914
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){int*_T0;unsigned _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_Aggrdecl*_T7;struct Cyc_Absyn_AggrdeclImpl*_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;void**_TA;int(*_TB)(struct _fat_ptr ap);struct _fat_ptr _TC;struct Cyc_Absyn_Aggrdecl*_TD;struct Cyc_Absyn_AggrdeclImpl*_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct _fat_ptr*_T11;void**_T12;void**_T13;int(*_T14)(struct _fat_ptr ap);struct _fat_ptr _T15;struct Cyc_Absyn_Aggrfield*_T16;void*_T17;int _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_Typ_Warn_Warg_struct _T1A;void**_T1B;void**_T1C;int(*_T1D)(struct _fat_ptr ap);struct _fat_ptr _T1E;
void*_T1F=Cyc_Absyn_compress(t);struct Cyc_List_List*_T20;union Cyc_Absyn_AggrInfo _T21;_T0=(int*)_T1F;_T1=*_T0;switch(_T1){case 0: _T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1F;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TLD3;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1F;_T6=_T22->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T23=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T21=_T23->f1;}}{union Cyc_Absyn_AggrInfo info=_T21;
# 917
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T7=ad;_T8=_T7->impl;
if(_T8!=0)goto _TLD5;{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
_T22.f1=_tag_fat("is_poly_field: type missing fields",sizeof(char),35U);_T9=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T9;void*_T23[1];_TA=_T23 + 0;*_TA=& _T22;{int(*_T24)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TB=_T24;}_TC=_tag_fat(_T23,sizeof(void*),1);_TB(_TC);}goto _TLD6;_TLD5: _TLD6: _TD=ad;_TE=_TD->impl;_T20=_TE->fields;goto _LL4;}_TLD3: goto _LL5;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T22=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T1F;_T20=_T22->f3;}_LL4: {struct Cyc_List_List*fs=_T20;
# 922
struct Cyc_Absyn_Aggrfield*field=Cyc_Absyn_lookup_field(fs,f);
if(field!=0)goto _TLD7;{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
_T22.f1=_tag_fat("is_poly_field: bad field ",sizeof(char),26U);_TF=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T11=f;_T23.f1=*_T11;_T10=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_T10;void*_T24[2];_T12=_T24 + 0;*_T12=& _T22;_T13=_T24 + 1;*_T13=& _T23;{int(*_T25)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T14=_T25;}_T15=_tag_fat(_T24,sizeof(void*),2);_T14(_T15);}}goto _TLD8;_TLD7: _TLD8: _T16=field;_T17=_T16->type;_T18=
Cyc_Toc_is_void_star_or_boxed_tvar(_T17);return _T18;}default: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
_T22.f1=_tag_fat("is_poly_field: bad type ",sizeof(char),25U);_T19=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T19;{struct Cyc_Warn_Typ_Warn_Warg_struct _T23;_T23.tag=2;_T23.f1=t;_T1A=_T23;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T23=_T1A;void*_T24[2];_T1B=_T24 + 0;*_T1B=& _T22;_T1C=_T24 + 1;*_T1C=& _T23;{int(*_T25)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T1D=_T25;}_T1E=_tag_fat(_T24,sizeof(void*),2);_T1D(_T1E);}}};}
# 933
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int _T3;struct Cyc_Absyn_Exp*_T4;void*_T5;void*_T6;struct _fat_ptr*_T7;int _T8;struct Cyc_Absyn_Exp*_T9;void*_TA;void*_TB;int _TC;struct Cyc_Absyn_Exp*_TD;void*_TE;void*_TF;int*_T10;int _T11;struct Cyc_Absyn_PtrInfo _T12;int _T13;int _T14;struct Cyc_Absyn_Exp*_T15;void*_T16;void*_T17;int _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_Typ_Warn_Warg_struct _T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;void**_T1D;void**_T1E;int(*_T1F)(struct _fat_ptr ap);struct _fat_ptr _T20;_T0=e;{
void*_T21=_T0->r;struct _fat_ptr*_T22;struct Cyc_Absyn_Exp*_T23;_T1=(int*)_T21;_T2=*_T1;switch(_T2){case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T24=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T21;_T23=_T24->f1;_T22=_T24->f2;}{struct Cyc_Absyn_Exp*e1=_T23;struct _fat_ptr*f=_T22;_T4=e1;_T5=_T4->topt;_T6=
# 936
_check_null(_T5);_T7=f;_T8=Cyc_Toc_is_poly_field(_T6,_T7);if(!_T8)goto _TLDA;_T9=e;_TA=_T9->topt;_TB=
_check_null(_TA);_TC=Cyc_Toc_is_void_star_or_boxed_tvar(_TB);_T3=!_TC;goto _TLDB;_TLDA: _T3=0;_TLDB:
# 936
 return _T3;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T24=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T21;_T23=_T24->f1;_T22=_T24->f2;}{struct Cyc_Absyn_Exp*e1=_T23;struct _fat_ptr*f=_T22;_TD=e1;_TE=_TD->topt;_TF=
# 939
_check_null(_TE);{void*_T24=Cyc_Absyn_compress(_TF);void*_T25;_T10=(int*)_T24;_T11=*_T10;if(_T11!=4)goto _TLDC;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T24;_T12=_T26->f1;_T25=_T12.elt_type;}{void*t=_T25;_T14=
# 941
Cyc_Toc_is_poly_field(t,f);if(!_T14)goto _TLDE;_T15=e;_T16=_T15->topt;_T17=_check_null(_T16);_T18=Cyc_Toc_is_void_star_or_boxed_tvar(_T17);_T13=!_T18;goto _TLDF;_TLDE: _T13=0;_TLDF: return _T13;}_TLDC:{struct Cyc_Warn_String_Warn_Warg_struct _T26;_T26.tag=0;
_T26.f1=_tag_fat("is_poly_project: bad type ",sizeof(char),27U);_T19=_T26;}{struct Cyc_Warn_String_Warn_Warg_struct _T26=_T19;{struct Cyc_Warn_Typ_Warn_Warg_struct _T27;_T27.tag=2;_T1B=e1;_T1C=_T1B->topt;_T27.f1=_check_null(_T1C);_T1A=_T27;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T27=_T1A;void*_T28[2];_T1D=_T28 + 0;*_T1D=& _T26;_T1E=_T28 + 1;*_T1E=& _T27;{int(*_T29)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T1F=_T29;}_T20=_tag_fat(_T28,sizeof(void*),2);_T1F(_T20);}};}}default:
# 944
 return 0;};}}
# 949
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_T0;int _T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;struct Cyc_Absyn_Exp*_T7;_T1=
Cyc_Toc_atomic_type(t);if(!_T1)goto _TLE0;_T0=Cyc_Toc__cycalloc_atomic_e;goto _TLE1;_TLE0: _T0=Cyc_Toc__cycalloc_e;_TLE1: {struct Cyc_Absyn_Exp*fn_e=_T0;{struct Cyc_Absyn_Exp*_T8[1];_T3=_T8 + 0;*_T3=s;_T4=fn_e;_T5=_tag_fat(_T8,sizeof(struct Cyc_Absyn_Exp*),1);_T2=Cyc_Toc_fncall_exp_dl(_T4,_T5);}_T6=
Cyc_Toc_void_star_type();_T7=Cyc_Absyn_set_type(_T2,_T6);return _T7;}}
# 954
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_T0;int _T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp**_T4;struct Cyc_Absyn_Exp*_T5;struct _fat_ptr _T6;void*_T7;struct Cyc_Absyn_Exp*_T8;_T1=
Cyc_Toc_atomic_type(elt_type);if(!_T1)goto _TLE2;_T0=Cyc_Toc__cyccalloc_atomic_e;goto _TLE3;_TLE2: _T0=Cyc_Toc__cyccalloc_e;_TLE3: {struct Cyc_Absyn_Exp*fn_e=_T0;{struct Cyc_Absyn_Exp*_T9[2];_T3=_T9 + 0;
*_T3=s;_T4=_T9 + 1;*_T4=n;_T5=fn_e;_T6=_tag_fat(_T9,sizeof(struct Cyc_Absyn_Exp*),2);_T2=Cyc_Toc_fncall_exp_dl(_T5,_T6);}_T7=Cyc_Toc_void_star_type();_T8=Cyc_Absyn_set_type(_T2,_T7);return _T8;}}
# 959
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;struct Cyc_Absyn_Exp*_T7;{struct Cyc_Absyn_Exp*_T8[3];_T1=_T8 + 0;
*_T1=rgn;_T2=_T8 + 1;*_T2=aq;_T3=_T8 + 2;*_T3=s;_T4=Cyc_Toc__region_malloc_e;_T5=_tag_fat(_T8,sizeof(struct Cyc_Absyn_Exp*),3);_T0=Cyc_Toc_fncall_exp_dl(_T4,_T5);}_T6=Cyc_Toc_void_star_type();_T7=Cyc_Absyn_set_type(_T0,_T6);return _T7;}
# 962
static struct Cyc_Absyn_Exp*Cyc_Toc_aqmalloc_exp(struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp*_T3;struct _fat_ptr _T4;void*_T5;struct Cyc_Absyn_Exp*_T6;{struct Cyc_Absyn_Exp*_T7[2];_T1=_T7 + 0;
*_T1=aq;_T2=_T7 + 1;*_T2=s;_T3=Cyc_Toc__aqual_malloc_e;_T4=_tag_fat(_T7,sizeof(struct Cyc_Absyn_Exp*),2);_T0=Cyc_Toc_fncall_exp_dl(_T3,_T4);}_T5=Cyc_Toc_void_star_type();_T6=Cyc_Absyn_set_type(_T0,_T5);return _T6;}
# 966
static struct Cyc_Absyn_Exp*Cyc_Toc_rvmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp*_T3;struct _fat_ptr _T4;void*_T5;struct Cyc_Absyn_Exp*_T6;{struct Cyc_Absyn_Exp*_T7[2];_T1=_T7 + 0;
*_T1=rgn;_T2=_T7 + 1;*_T2=s;_T3=Cyc_Toc__region_vmalloc_e;_T4=_tag_fat(_T7,sizeof(struct Cyc_Absyn_Exp*),2);_T0=Cyc_Toc_fncall_exp_dl(_T3,_T4);}_T5=Cyc_Toc_void_star_type();_T6=Cyc_Absyn_set_type(_T0,_T5);return _T6;}
# 970
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;struct Cyc_Absyn_Exp*_T7;{struct Cyc_Absyn_Exp*_T8[3];_T1=_T8 + 0;
*_T1=rgn;_T2=_T8 + 1;*_T2=aq;_T3=_T8 + 2;*_T3=s;_T4=Cyc_Toc__fast_region_malloc_e;_T5=_tag_fat(_T8,sizeof(struct Cyc_Absyn_Exp*),3);_T0=Cyc_Toc_fncall_exp_dl(_T4,_T5);}_T6=Cyc_Toc_void_star_type();_T7=Cyc_Absyn_set_type(_T0,_T6);return _T7;}
# 974
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp**_T4;struct Cyc_Absyn_Exp*_T5;struct _fat_ptr _T6;void*_T7;struct Cyc_Absyn_Exp*_T8;{struct Cyc_Absyn_Exp*_T9[4];_T1=_T9 + 0;
*_T1=rgn;_T2=_T9 + 1;*_T2=aq;_T3=_T9 + 2;*_T3=s;_T4=_T9 + 3;*_T4=n;_T5=Cyc_Toc__region_calloc_e;_T6=_tag_fat(_T9,sizeof(struct Cyc_Absyn_Exp*),4);_T0=Cyc_Toc_fncall_exp_dl(_T5,_T6);}_T7=Cyc_Toc_void_star_type();_T8=Cyc_Absyn_set_type(_T0,_T7);return _T8;}
# 977
static struct Cyc_Absyn_Exp*Cyc_Toc_aqcalloc_exp(struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp**_T2;struct Cyc_Absyn_Exp**_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;struct Cyc_Absyn_Exp*_T7;{struct Cyc_Absyn_Exp*_T8[3];_T1=_T8 + 0;
*_T1=aq;_T2=_T8 + 1;*_T2=s;_T3=_T8 + 2;*_T3=n;_T4=Cyc_Toc__aqual_calloc_e;_T5=_tag_fat(_T8,sizeof(struct Cyc_Absyn_Exp*),3);_T0=Cyc_Toc_fncall_exp_dl(_T4,_T5);}_T6=Cyc_Toc_void_star_type();_T7=Cyc_Absyn_set_type(_T0,_T6);return _T7;}
# 981
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){struct Cyc_Absyn_Exp*_T0;struct _fat_ptr _T1;struct Cyc_Absyn_Exp*_T2;void*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Stmt*_T5;_T0=Cyc_Toc__throw_match_e;_T1=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_T2=
Cyc_Toc_fncall_exp_dl(_T0,_T1);_T3=
Cyc_Toc_void_star_type();_T4=
# 982
Cyc_Absyn_set_type(_T2,_T3);_T5=Cyc_Absyn_exp_stmt(_T4,0U);return _T5;}
# 987
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_Wstring_c _T5;unsigned _T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T8;void*_T9;struct Cyc_Absyn_Exp*_TA;void*_TB;struct Cyc_Absyn_Exp*_TC;void*_TD;void*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Exp*_T10;void*_T11;void*_T12;struct Cyc_Absyn_Exp*_T13;struct Cyc_List_List*_T14;struct _tuple20**_T15;struct _tuple20*_T16;struct _tuple20**_T17;struct _tuple20*_T18;struct _tuple20**_T19;struct _tuple20*_T1A;struct _fat_ptr _T1B;struct Cyc_Absyn_Exp*_T1C;
# 995
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;_T0=e;{
void*_T1D=_T0->r;_T1=(int*)_T1D;_T2=*_T1;if(_T2!=0)goto _TLE4;_T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1D;_T4=_T3->f1;_T5=_T4.Wstring_c;_T6=_T5.tag;switch(_T6){case 8: goto _LL4;case 9: _LL4: {
# 1000
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);{struct Cyc_List_List*_T1E=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1F=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T1F->tag=0;
_T1F->f1=vd;_T8=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1F;}_T9=(void*)_T8;_T1E->hd=Cyc_Absyn_new_decl(_T9,0U);_T1E->tl=Cyc_Toc_result_decls;_T7=(struct Cyc_List_List*)_T1E;}Cyc_Toc_result_decls=_T7;_TA=
Cyc_Absyn_var_exp(x,0U);_TB=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);xexp=Cyc_Absyn_set_type(_TA,_TB);_TC=
Cyc_Absyn_add_exp(xexp,sz,0U);_TD=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);xplussz=Cyc_Absyn_set_type(_TC,_TD);goto _LL0;}default: goto _LL5;}goto _TLE5;_TLE4: _LL5: _TE=
# 1008
Cyc_Toc_void_star_type();_TF=e;_T10=Cyc_Toc_cast_it(_TE,_TF);_T11=Cyc_Toc_void_star_type();xexp=Cyc_Absyn_set_type(_T10,_T11);_T12=
# 1010
Cyc_Toc_void_star_type();_T13=Cyc_Absyn_add_exp(e,sz,0U);xplussz=Cyc_Toc_cast_it(_T12,_T13);goto _LL0;_TLE5: _LL0:;}{struct _tuple20*_T1D[3];_T15=_T1D + 0;{struct _tuple20*_T1E=_cycalloc(sizeof(struct _tuple20));
# 1013
_T1E->f0=0;_T1E->f1=xexp;_T16=(struct _tuple20*)_T1E;}*_T15=_T16;_T17=_T1D + 1;{struct _tuple20*_T1E=_cycalloc(sizeof(struct _tuple20));
_T1E->f0=0;_T1E->f1=Cyc_Absyn_copy_exp(xexp);_T18=(struct _tuple20*)_T1E;}*_T17=_T18;_T19=_T1D + 2;{struct _tuple20*_T1E=_cycalloc(sizeof(struct _tuple20));
_T1E->f0=0;_T1E->f1=xplussz;_T1A=(struct _tuple20*)_T1E;}*_T19=_T1A;_T1B=_tag_fat(_T1D,sizeof(struct _tuple20*),3);_T14=Cyc_List_list(_T1B);}_T1C=
# 1013
Cyc_Absyn_unresolvedmem_exp(0,_T14,0U);return _T1C;}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1047 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){struct Cyc_Toc_Env*_T0;struct _RegionHandle*_T1;int*_T2;struct _RegionHandle*_T3;_T1=r;{struct Cyc_Toc_Env*_T4=_region_malloc(_T1,0U,sizeof(struct Cyc_Toc_Env));
_T4->break_lab=0;_T4->continue_lab=0;_T4->fallthru_info=0;_T4->toplevel=1;_T3=r;{int*_T5=_region_malloc(_T3,0U,sizeof(int));*_T5=0;_T2=(int*)_T5;}_T4->in_lhs=_T2;_T4->rgn=r;_T0=(struct Cyc_Toc_Env*)_T4;}return _T0;}
# 1051
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env*_T0;int _T1;int _T2;_T0=nv;{struct Cyc_Toc_Env _T3=*_T0;_T2=_T3.toplevel;}{int t=_T2;_T1=t;
# 1053
return _T1;}}
# 1055
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*_T0;struct Cyc_Toc_Env*_T1;struct _RegionHandle*_T2;int*_T3;int _T4;struct Cyc_Toc_FallthruInfo*_T5;struct _fat_ptr**_T6;struct _fat_ptr**_T7;_T0=e;{struct Cyc_Toc_Env _T8=*_T0;_T7=_T8.break_lab;_T6=_T8.continue_lab;_T5=_T8.fallthru_info;_T4=_T8.toplevel;_T3=_T8.in_lhs;}{struct _fat_ptr**b=_T7;struct _fat_ptr**c=_T6;struct Cyc_Toc_FallthruInfo*f=_T5;int t=_T4;int*lhs=_T3;_T2=r;{struct Cyc_Toc_Env*_T8=_region_malloc(_T2,0U,sizeof(struct Cyc_Toc_Env));
# 1057
_T8->break_lab=b;_T8->continue_lab=c;_T8->fallthru_info=f;_T8->toplevel=0;_T8->in_lhs=lhs;_T8->rgn=r;_T1=(struct Cyc_Toc_Env*)_T8;}return _T1;}}
# 1059
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*_T0;struct Cyc_Toc_Env*_T1;struct _RegionHandle*_T2;int*_T3;int _T4;struct Cyc_Toc_FallthruInfo*_T5;struct _fat_ptr**_T6;struct _fat_ptr**_T7;_T0=e;{struct Cyc_Toc_Env _T8=*_T0;_T7=_T8.break_lab;_T6=_T8.continue_lab;_T5=_T8.fallthru_info;_T4=_T8.toplevel;_T3=_T8.in_lhs;}{struct _fat_ptr**b=_T7;struct _fat_ptr**c=_T6;struct Cyc_Toc_FallthruInfo*f=_T5;int t=_T4;int*lhs=_T3;_T2=r;{struct Cyc_Toc_Env*_T8=_region_malloc(_T2,0U,sizeof(struct Cyc_Toc_Env));
# 1061
_T8->break_lab=b;_T8->continue_lab=c;_T8->fallthru_info=f;_T8->toplevel=1;_T8->in_lhs=lhs;_T8->rgn=r;_T1=(struct Cyc_Toc_Env*)_T8;}return _T1;}}
# 1063
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env*_T0;int*_T1;int _T2;int*_T3;_T0=nv;{struct Cyc_Toc_Env _T4=*_T0;_T3=_T4.in_lhs;}{int*b=_T3;_T1=b;_T2=*_T1;
# 1065
return _T2;}}
# 1067
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){struct Cyc_Toc_Env*_T0;int*_T1;int*_T2;_T0=e;{struct Cyc_Toc_Env _T3=*_T0;_T2=_T3.in_lhs;}{int*lhs=_T2;_T1=lhs;
# 1069
*_T1=b;}}
# 1072
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*_T0;struct Cyc_Toc_Env*_T1;struct _RegionHandle*_T2;int*_T3;int _T4;struct Cyc_Toc_FallthruInfo*_T5;struct _fat_ptr**_T6;struct _fat_ptr**_T7;_T0=e;{struct Cyc_Toc_Env _T8=*_T0;_T7=_T8.break_lab;_T6=_T8.continue_lab;_T5=_T8.fallthru_info;_T4=_T8.toplevel;_T3=_T8.in_lhs;}{struct _fat_ptr**b=_T7;struct _fat_ptr**c=_T6;struct Cyc_Toc_FallthruInfo*f=_T5;int t=_T4;int*lhs=_T3;_T2=r;{struct Cyc_Toc_Env*_T8=_region_malloc(_T2,0U,sizeof(struct Cyc_Toc_Env));
# 1074
_T8->break_lab=b;_T8->continue_lab=c;_T8->fallthru_info=f;_T8->toplevel=t;_T8->in_lhs=lhs;_T8->rgn=r;_T1=(struct Cyc_Toc_Env*)_T8;}return _T1;}}
# 1079
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*_T0;struct Cyc_Toc_Env*_T1;struct _RegionHandle*_T2;int*_T3;int _T4;struct Cyc_Toc_FallthruInfo*_T5;struct _fat_ptr**_T6;struct _fat_ptr**_T7;_T0=e;{struct Cyc_Toc_Env _T8=*_T0;_T7=_T8.break_lab;_T6=_T8.continue_lab;_T5=_T8.fallthru_info;_T4=_T8.toplevel;_T3=_T8.in_lhs;}{struct _fat_ptr**b=_T7;struct _fat_ptr**c=_T6;struct Cyc_Toc_FallthruInfo*f=_T5;int t=_T4;int*lhs=_T3;_T2=r;{struct Cyc_Toc_Env*_T8=_region_malloc(_T2,0U,sizeof(struct Cyc_Toc_Env));
# 1081
_T8->break_lab=0;_T8->continue_lab=0;_T8->fallthru_info=f;_T8->toplevel=t;_T8->in_lhs=lhs;_T8->rgn=r;_T1=(struct Cyc_Toc_Env*)_T8;}return _T1;}}
# 1085
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){struct Cyc_Toc_Env*_T0;struct _RegionHandle*_T1;struct Cyc_Toc_FallthruInfo*_T2;struct Cyc_Toc_FallthruInfo*_T3;struct Cyc_Toc_Env*_T4;struct _RegionHandle*_T5;struct _fat_ptr**_T6;struct _RegionHandle*_T7;int*_T8;int _T9;struct Cyc_Toc_FallthruInfo*_TA;struct _fat_ptr**_TB;struct _fat_ptr**_TC;_T0=e;{struct Cyc_Toc_Env _TD=*_T0;_TC=_TD.break_lab;_TB=_TD.continue_lab;_TA=_TD.fallthru_info;_T9=_TD.toplevel;_T8=_TD.in_lhs;}{struct _fat_ptr**b=_TC;struct _fat_ptr**c=_TB;struct Cyc_Toc_FallthruInfo*f=_TA;int t=_T9;int*lhs=_T8;
# 1090
struct Cyc_Toc_FallthruInfo*fi;_T1=r;fi=_region_malloc(_T1,0U,sizeof(struct Cyc_Toc_FallthruInfo));_T2=fi;
_T2->label=fallthru_l;_T3=fi;_T3->binders=fallthru_binders;_T5=r;{struct Cyc_Toc_Env*_TD=_region_malloc(_T5,0U,sizeof(struct Cyc_Toc_Env));_T7=r;{struct _fat_ptr**_TE=_region_malloc(_T7,0U,sizeof(struct _fat_ptr*));
*_TE=break_l;_T6=(struct _fat_ptr**)_TE;}_TD->break_lab=_T6;_TD->continue_lab=c;_TD->fallthru_info=fi;_TD->toplevel=t;_TD->in_lhs=lhs;_TD->rgn=r;_T4=(struct Cyc_Toc_Env*)_TD;}return _T4;}}
# 1094
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){struct Cyc_Toc_Env*_T0;struct Cyc_Toc_Env*_T1;struct _RegionHandle*_T2;struct _fat_ptr**_T3;struct _RegionHandle*_T4;int*_T5;int _T6;struct Cyc_Toc_FallthruInfo*_T7;struct _fat_ptr**_T8;struct _fat_ptr**_T9;_T0=e;{struct Cyc_Toc_Env _TA=*_T0;_T9=_TA.break_lab;_T8=_TA.continue_lab;_T7=_TA.fallthru_info;_T6=_TA.toplevel;_T5=_TA.in_lhs;}{struct _fat_ptr**b=_T9;struct _fat_ptr**c=_T8;struct Cyc_Toc_FallthruInfo*f=_T7;int t=_T6;int*lhs=_T5;_T2=r;{struct Cyc_Toc_Env*_TA=_region_malloc(_T2,0U,sizeof(struct Cyc_Toc_Env));_T4=r;{struct _fat_ptr**_TB=_region_malloc(_T4,0U,sizeof(struct _fat_ptr*));
# 1097
*_TB=break_l;_T3=(struct _fat_ptr**)_TB;}_TA->break_lab=_T3;_TA->continue_lab=c;_TA->fallthru_info=0;_TA->toplevel=t;_TA->in_lhs=lhs;_TA->rgn=r;_T1=(struct Cyc_Toc_Env*)_TA;}return _T1;}}
# 1109 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*);
# 1113
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Fndecl*,int);struct _tuple33{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
# 1116
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){struct Cyc_List_List*_T0;void*_T1;struct _tuple33*_T2;struct _tuple33 _T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_List_List*_T5;
_TLEA: if(l!=0)goto _TLE8;else{goto _TLE9;}
_TLE8: _T0=l;_T1=_T0->hd;_T2=(struct _tuple33*)_T1;_T3=*_T2;_T4=_T3.f1;Cyc_Toc_exptypes_to_c(_T4);_T5=l;
# 1117
l=_T5->tl;goto _TLEA;_TLE9:;}
# 1121
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T1;char*_T2;char*_T3;struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*_T4;char*_T5;char*_T6;struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*_T7;char*_T8;char*_T9;struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_TA;char*_TB;char*_TC;struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*_TD;char*_TE;char*_TF;struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_T10;char*_T11;char*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;int(*_T15)(unsigned,struct _fat_ptr);void*(*_T16)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Exp*_T17;unsigned _T18;struct _fat_ptr _T19;_T0=e;{
void*_T1A=_T0->annot;_T1=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_T1A;_T2=_T1->tag;_T3=Cyc_InsertChecks_NoCheck;if(_T2!=_T3)goto _TLEB;
return 0;_TLEB: _T4=(struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_T1A;_T5=_T4->tag;_T6=Cyc_InsertChecks_NullOnly;if(_T5!=_T6)goto _TLED;goto _LL6;_TLED: _T7=(struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_T1A;_T8=_T7->tag;_T9=Cyc_InsertChecks_NullAndFatBound;if(_T8!=_T9)goto _TLEF;_LL6: goto _LL8;_TLEF: _TA=(struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_T1A;_TB=_TA->tag;_TC=Cyc_InsertChecks_NullAndThinBound;if(_TB!=_TC)goto _TLF1;_LL8:
# 1126
 return 1;_TLF1: _TD=(struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_T1A;_TE=_TD->tag;_TF=Cyc_InsertChecks_FatBound;if(_TE!=_TF)goto _TLF3;goto _LLC;_TLF3: _T10=(struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_T1A;_T11=_T10->tag;_T12=Cyc_InsertChecks_ThinBound;if(_T11!=_T12)goto _TLF5;_LLC:
# 1128
 return 0;_TLF5:{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
_T1B.f1=_tag_fat("Toc: do_null_check",sizeof(char),19U);_T13=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T13;void*_T1C[1];_T14=_T1C + 0;*_T14=& _T1B;_T16=Cyc_Warn_impos_loc2;{int(*_T1D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T16;_T15=_T1D;}_T17=e;_T18=_T17->loc;_T19=_tag_fat(_T1C,sizeof(void*),1);_T15(_T18,_T19);};}}
# 1140 "toc.cyc"
enum Cyc_Toc_PTR_ANS{Cyc_Toc_DIRECT_DEREF =0U,Cyc_Toc_DROP_SUBSCRIPT =1U,Cyc_Toc_LEAVE_SUBSCRIPT =2U};
# 1142
static enum Cyc_Toc_PTR_ANS Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;void*_TD;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_TE;char*_TF;char*_T10;void*_T11;void*_T12;struct Cyc_Absyn_Exp*_T13;unsigned _T14;void*_T15;struct Cyc_Absyn_Tqual _T16;struct Cyc_Absyn_Exp*_T17;void*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*_T1D;char*_T1E;char*_T1F;void*_T20;void*_T21;struct Cyc_Absyn_Exp*_T22;unsigned _T23;void*_T24;struct Cyc_Absyn_Tqual _T25;struct Cyc_Absyn_Exp*_T26;void*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_Absyn_Exp*_T29;struct Cyc_Absyn_Exp*_T2A;struct Cyc_Warn_String_Warn_Warg_struct _T2B;void**_T2C;int(*_T2D)(struct _fat_ptr ap);struct _fat_ptr _T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp**_T31;struct Cyc_Absyn_Exp*_T32;struct _fat_ptr _T33;void*_T34;struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*_T35;char*_T36;char*_T37;void*_T38;struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*_T39;char*_T3A;char*_T3B;struct Cyc_Absyn_Exp*_T3C;struct Cyc_Absyn_Exp*_T3D;unsigned _T3E;struct Cyc_Absyn_Exp*_T3F;void*_T40;struct Cyc_Absyn_Exp*_T41;void*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Exp**_T44;struct Cyc_Absyn_Exp**_T45;struct Cyc_Absyn_Exp*_T46;void*_T47;struct Cyc_Absyn_Exp**_T48;struct Cyc_Absyn_Exp*_T49;struct _fat_ptr _T4A;void*_T4B;struct Cyc_Absyn_Exp*_T4C;struct Cyc_Absyn_Exp*_T4D;void*_T4E;struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_T4F;char*_T50;char*_T51;void*_T52;void*_T53;struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_T54;char*_T55;char*_T56;void*_T57;struct Cyc_Absyn_Exp*_T58;struct Cyc_Absyn_Exp*_T59;unsigned _T5A;struct Cyc_Absyn_Exp*_T5B;void*_T5C;int _T5D;struct Cyc_Absyn_Exp*_T5E;void*_T5F;void*_T60;int _T61;int*_T62;int _T63;struct Cyc_Absyn_Exp*_T64;struct Cyc_Absyn_ArrayInfo _T65;void*_T66;struct Cyc_Absyn_Tqual _T67;struct Cyc_Absyn_Exp*_T68;struct Cyc_Absyn_ArrayInfo _T69;void*_T6A;struct Cyc_Absyn_Tqual _T6B;void*_T6C;struct Cyc_Absyn_Exp*_T6D;struct Cyc_Absyn_Exp**_T6E;struct Cyc_Absyn_Exp**_T6F;struct Cyc_Absyn_Exp**_T70;struct Cyc_Absyn_ArrayInfo _T71;struct Cyc_Absyn_Exp*_T72;struct Cyc_Absyn_Exp*_T73;struct Cyc_Absyn_ArrayInfo _T74;void*_T75;struct Cyc_Absyn_Exp*_T76;void*_T77;struct Cyc_Absyn_Exp*_T78;struct Cyc_Absyn_Exp*_T79;void*_T7A;struct Cyc_Absyn_Exp**_T7B;struct Cyc_Absyn_Exp*_T7C;struct _fat_ptr _T7D;void*_T7E;struct Cyc_Absyn_Exp*_T7F;struct Cyc_Absyn_Exp*_T80;struct Cyc_Absyn_Exp*_T81;void*_T82;struct Cyc_Absyn_Exp*_T83;struct Cyc_Absyn_Exp**_T84;struct Cyc_Absyn_Exp**_T85;struct Cyc_Absyn_Exp**_T86;struct Cyc_Absyn_Exp*_T87;void*_T88;struct Cyc_Absyn_Exp**_T89;struct Cyc_Absyn_Exp*_T8A;struct _fat_ptr _T8B;void*_T8C;struct Cyc_Absyn_Exp*_T8D;int _T8E;struct Cyc_Absyn_Exp*_T8F;struct Cyc_Absyn_Exp*_T90;struct Cyc_Absyn_Exp**_T91;struct Cyc_Absyn_Exp*_T92;struct _fat_ptr _T93;int _T94;struct Cyc_Absyn_Exp*_T95;void*_T96;struct Cyc_Absyn_Exp**_T97;struct Cyc_Absyn_Exp**_T98;struct Cyc_Absyn_Exp**_T99;struct Cyc_Absyn_Exp*_T9A;struct _fat_ptr _T9B;struct Cyc_Absyn_Exp*_T9C;void*_T9D;struct Cyc_Absyn_Exp*_T9E;struct Cyc_Absyn_Exp**_T9F;void*_TA0;void*_TA1;struct Cyc_Absyn_Exp*_TA2;void*_TA3;struct Cyc_Absyn_Exp**_TA4;struct Cyc_Absyn_Exp**_TA5;struct Cyc_Absyn_Exp**_TA6;struct Cyc_Absyn_Exp*_TA7;struct _fat_ptr _TA8;struct Cyc_Warn_String_Warn_Warg_struct _TA9;void**_TAA;int(*_TAB)(struct _fat_ptr ap);struct _fat_ptr _TAC;enum Cyc_Toc_PTR_ANS _TAD;struct Cyc_Warn_String_Warn_Warg_struct _TAE;void**_TAF;int(*_TB0)(struct _fat_ptr ap);struct _fat_ptr _TB1;
# 1144
enum Cyc_Toc_PTR_ANS ans;
int old_lhs=Cyc_Toc_in_lhs(nv);_T0=ptr;_T1=_T0->topt;_T2=
_check_null(_T1);{void*old_typ=Cyc_Absyn_compress(_T2);
void*new_typ=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index==0)goto _TLF7;
Cyc_Toc_exp_to_c(nv,index);goto _TLF8;_TLF7: _TLF8: {void*_TB2;void*_TB3;struct Cyc_Absyn_Tqual _TB4;void*_TB5;_T3=old_typ;_T4=(int*)_T3;_T5=*_T4;if(_T5!=4)goto _TLF9;_T6=old_typ;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6;_T7=_TB6->f1;_TB5=_T7.elt_type;_T8=_TB6->f1;_TB4=_T8.elt_tq;_T9=_TB6->f1;_TA=_T9.ptr_atts;_TB3=_TA.bounds;_TB=_TB6->f1;_TC=_TB.ptr_atts;_TB2=_TC.zero_term;}{void*ta=_TB5;struct Cyc_Absyn_Tqual tq=_TB4;void*b=_TB3;void*zt=_TB2;{struct Cyc_Absyn_Exp*_TB6;_TD=annot;_TE=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_TD;_TF=_TE->tag;_T10=Cyc_InsertChecks_NoCheck;if(_TF!=_T10)goto _TLFB;_T11=
# 1157
Cyc_Absyn_bounds_one();_T12=b;_T13=Cyc_Tcutil_get_bounds_exp_constrain(_T11,_T12,1);_T14=(unsigned)_T13;if(_T14)goto _TLFD;else{goto _TLFF;}
# 1160
_TLFF: _T15=Cyc_Toc_typ_to_c(ta);_T16=tq;{void*newt=Cyc_Absyn_cstar_type(_T15,_T16);_T17=ptr;_T18=newt;_T19=
Cyc_Absyn_copy_exp(ptr);_T1A=Cyc_Toc_get_curr(_T19);_T17->r=Cyc_Toc_cast_it_r(_T18,_T1A);_T1B=ptr;
_T1B->topt=newt;}goto _TLFE;_TLFD: _TLFE:
# 1164
 ans=2U;goto _LL5;_TLFB: _T1C=annot;_T1D=(struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_T1C;_T1E=_T1D->tag;_T1F=Cyc_InsertChecks_NullOnly;if(_T1E!=_T1F)goto _TL100;_T20=
# 1167
Cyc_Absyn_bounds_one();_T21=b;_T22=Cyc_Tcutil_get_bounds_exp_constrain(_T20,_T21,1);_T23=(unsigned)_T22;if(_T23)goto _TL102;else{goto _TL104;}
# 1169
_TL104: _T24=Cyc_Toc_typ_to_c(ta);_T25=tq;{void*newt=Cyc_Absyn_cstar_type(_T24,_T25);_T26=ptr;_T27=newt;_T28=
Cyc_Absyn_copy_exp(ptr);_T29=Cyc_Toc_get_curr(_T28);_T26->r=Cyc_Toc_cast_it_r(_T27,_T29);_T2A=ptr;
_T2A->topt=newt;
# 1176
if(index==0)goto _TL105;{struct Cyc_Warn_String_Warn_Warg_struct _TB7;_TB7.tag=0;
_TB7.f1=_tag_fat("subscript of ? with no bounds check but need null check",sizeof(char),56U);_T2B=_TB7;}{struct Cyc_Warn_String_Warn_Warg_struct _TB7=_T2B;void*_TB8[1];_T2C=_TB8 + 0;*_T2C=& _TB7;{int(*_TB9)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T2D=_TB9;}_T2E=_tag_fat(_TB8,sizeof(void*),1);_T2D(_T2E);}goto _TL106;_TL105: _TL106:;}goto _TL103;_TL102: _TL103: _T2F=ptr;{struct Cyc_Absyn_Exp*_TB7[1];_T31=_TB7 + 0;*_T31=
# 1180
Cyc_Absyn_copy_exp(ptr);_T32=Cyc_Toc__check_null_e;_T33=_tag_fat(_TB7,sizeof(struct Cyc_Absyn_Exp*),1);_T30=Cyc_Toc_fncall_exp_dl(_T32,_T33);}_T2F->r=_T30->r;
ans=2U;goto _LL5;_TL100: _T34=annot;_T35=(struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_T34;_T36=_T35->tag;_T37=Cyc_InsertChecks_NullAndFatBound;if(_T36!=_T37)goto _TL107;goto _LLD;_TL107: _T38=annot;_T39=(struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_T38;_T3A=_T39->tag;_T3B=Cyc_InsertChecks_FatBound;if(_T3A!=_T3B)goto _TL109;_LLD:{
# 1186
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);_T3D=index;_T3E=(unsigned)_T3D;
if(!_T3E)goto _TL10B;_T3C=index;goto _TL10C;_TL10B: _T3F=Cyc_Absyn_uint_exp(0U,0U);_T40=Cyc_Absyn_uint_type;_T3C=Cyc_Absyn_set_type(_T3F,_T40);_TL10C: {struct Cyc_Absyn_Exp*ind=_T3C;_T41=ptr;_T42=newt;{struct Cyc_Absyn_Exp*_TB7[3];_T44=_TB7 + 0;
# 1191
*_T44=Cyc_Absyn_copy_exp(ptr);_T45=_TB7 + 1;_T46=
Cyc_Absyn_sizeoftype_exp(ta1,0U);_T47=Cyc_Absyn_uint_type;*_T45=Cyc_Absyn_set_type(_T46,_T47);_T48=_TB7 + 2;
*_T48=ind;_T49=Cyc_Toc__check_fat_subscript_e;_T4A=_tag_fat(_TB7,sizeof(struct Cyc_Absyn_Exp*),3);_T43=Cyc_Toc_fncall_exp_dl(_T49,_T4A);}_T4B=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_T4C=
# 1190
Cyc_Absyn_set_type(_T43,_T4B);
# 1189
_T41->r=Cyc_Toc_cast_it_r(_T42,_T4C);_T4D=ptr;
# 1194
_T4D->topt=newt;
ans=0U;goto _LL5;}}_TL109: _T4E=annot;_T4F=(struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_T4E;_T50=_T4F->tag;_T51=Cyc_InsertChecks_NullAndThinBound;if(_T50!=_T51)goto _TL10D;_T52=annot;{struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_TB7=(struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_T52;_TB6=_TB7->f1;}{struct Cyc_Absyn_Exp*bd=_TB6;
# 1198
fn_e=Cyc_Toc__check_known_subscript_null_e;_TB6=bd;goto _LL11;}_TL10D: _T53=annot;_T54=(struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_T53;_T55=_T54->tag;_T56=Cyc_InsertChecks_ThinBound;if(_T55!=_T56)goto _TL10F;_T57=annot;{struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_TB7=(struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_T57;_TB6=_TB7->f1;}_LL11:{struct Cyc_Absyn_Exp*bd=_TB6;
# 1201
void*ta1=Cyc_Toc_typ_to_c(ta);_T59=index;_T5A=(unsigned)_T59;
if(!_T5A)goto _TL111;_T58=index;goto _TL112;_TL111: _T5B=Cyc_Absyn_uint_exp(0U,0U);_T5C=Cyc_Absyn_uint_type;_T58=Cyc_Absyn_set_type(_T5B,_T5C);_TL112: {struct Cyc_Absyn_Exp*ind=_T58;
# 1206
struct _tuple13 _TB7=Cyc_Evexp_eval_const_uint_exp(bd);int _TB8;unsigned _TB9;_TB9=_TB7.f0;_TB8=_TB7.f1;{unsigned i=_TB9;int valid=_TB8;_T5D=valid;
if(_T5D)goto _TL117;else{goto _TL115;}_TL117: if(i!=1U)goto _TL115;else{goto _TL116;}_TL116: _T5E=ptr;_T5F=_T5E->topt;_T60=_check_null(_T5F);_T61=Cyc_Tcutil_is_zeroterm_pointer_type(_T60);if(_T61)goto _TL113;else{goto _TL115;}
# 1210
_TL115:{struct Cyc_Absyn_Exp*newp=Cyc_Absyn_copy_exp(ptr);
void*_TBA=Cyc_Absyn_compress(ta1);struct Cyc_Absyn_ArrayInfo _TBB;_T62=(int*)_TBA;_T63=*_T62;if(_T63!=5)goto _TL118;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TBC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TBA;_TBB=_TBC->f1;}{struct Cyc_Absyn_ArrayInfo ainfo=_TBB;_T64=newp;_T65=ainfo;_T66=_T65.elt_type;_T67=tq;
# 1213
_T64->topt=Cyc_Absyn_cstar_type(_T66,_T67);_T68=ptr;_T69=ainfo;_T6A=_T69.elt_type;_T6B=tq;_T6C=
Cyc_Absyn_cstar_type(_T6A,_T6B);{struct Cyc_Absyn_Exp*_TBC[4];_T6E=_TBC + 0;
*_T6E=newp;_T6F=_TBC + 1;
*_T6F=bd;_T70=_TBC + 2;_T71=ainfo;_T72=_T71.num_elts;_T73=
_check_null(_T72);_T74=ainfo;_T75=_T74.elt_type;_T76=
Cyc_Absyn_sizeoftype_exp(_T75,0U);_T77=Cyc_Absyn_uint_type;_T78=Cyc_Absyn_set_type(_T76,_T77);_T79=
# 1217
Cyc_Absyn_times_exp(_T73,_T78,0U);_T7A=Cyc_Absyn_uint_type;*_T70=Cyc_Absyn_set_type(_T79,_T7A);_T7B=_TBC + 3;
# 1220
*_T7B=ind;_T7C=fn_e;_T7D=_tag_fat(_TBC,sizeof(struct Cyc_Absyn_Exp*),4);_T6D=Cyc_Toc_fncall_exp_dl(_T7C,_T7D);}_T7E=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,tq);_T7F=
# 1215
Cyc_Absyn_set_type(_T6D,_T7E);
# 1214
_T68->r=Cyc_Toc_cast_it_r(_T6C,_T7F);
# 1221
ans=1U;goto _LL17;}_TL118: _T80=newp;
# 1224
_T80->topt=Cyc_Absyn_cstar_type(ta1,tq);_T81=ptr;_T82=
Cyc_Absyn_cstar_type(ta1,tq);{struct Cyc_Absyn_Exp*_TBC[4];_T84=_TBC + 0;
*_T84=newp;_T85=_TBC + 1;
*_T85=bd;_T86=_TBC + 2;_T87=Cyc_Absyn_sizeoftype_exp(ta1,0U);_T88=Cyc_Absyn_uint_type;*_T86=Cyc_Absyn_set_type(_T87,_T88);_T89=_TBC + 3;*_T89=ind;_T8A=fn_e;_T8B=_tag_fat(_TBC,sizeof(struct Cyc_Absyn_Exp*),4);_T83=Cyc_Toc_fncall_exp_dl(_T8A,_T8B);}_T8C=
Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,tq);_T8D=
# 1226
Cyc_Absyn_set_type(_T83,_T8C);
# 1225
_T81->r=Cyc_Toc_cast_it_r(_T82,_T8D);
# 1229
ans=0U;goto _LL17;_LL17:;}goto _TL114;
# 1234
_TL113: _T8E=Cyc_Toc_is_zero(bd);if(!_T8E)goto _TL11A;
if(fn_e!=Cyc_Toc__check_known_subscript_null_e)goto _TL11C;_T8F=ptr;{struct Cyc_Absyn_Exp*_TBA[1];_T91=_TBA + 0;*_T91=
# 1237
Cyc_Absyn_copy_exp(ptr);_T92=Cyc_Toc__check_null_e;_T93=_tag_fat(_TBA,sizeof(struct Cyc_Absyn_Exp*),1);_T90=Cyc_Toc_fncall_exp_dl(_T92,_T93);}_T8F->r=_T90->r;goto _TL11D;_TL11C: _TL11D:
 ans=2U;goto _TL11B;
# 1242
_TL11A: _T94=Cyc_Toc_isCharStar(ptr);if(!_T94)goto _TL11E;_T95=ptr;{struct Cyc_Absyn_Exp*_TBA[3];_T97=_TBA + 0;
# 1244
*_T97=Cyc_Absyn_copy_exp(ptr);_T98=_TBA + 1;*_T98=bd;_T99=_TBA + 2;*_T99=ind;_T9A=Cyc_Toc__zero_arr_plus_char_e;_T9B=_tag_fat(_TBA,sizeof(struct Cyc_Absyn_Exp*),3);_T96=Cyc_Toc_fncall_exp_r(_T9A,_T9B);}
# 1243
_T95->r=_T96;goto _TL11F;
# 1246
_TL11E: _T9C=ptr;_T9D=Cyc_Toc_typ_to_c(old_typ);{struct Cyc_Absyn_Exp*_TBA[4];_T9F=_TBA + 0;_TA0=
# 1248
Cyc_Toc_typ_to_c(old_typ);_TA1=Cyc_Tcutil_pointer_elt_type(_TA0);_TA2=Cyc_Absyn_sizeoftype_exp(_TA1,0U);_TA3=Cyc_Absyn_uint_type;*_T9F=Cyc_Absyn_set_type(_TA2,_TA3);_TA4=_TBA + 1;
*_TA4=Cyc_Absyn_copy_exp(ptr);_TA5=_TBA + 2;*_TA5=bd;_TA6=_TBA + 3;*_TA6=ind;_TA7=Cyc_Toc__zero_arr_plus_other_e;_TA8=_tag_fat(_TBA,sizeof(struct Cyc_Absyn_Exp*),4);_T9E=Cyc_Toc_fncall_exp_dl(_TA7,_TA8);}
# 1246
_T9C->r=Cyc_Toc_cast_it_r(_T9D,_T9E);_TL11F:
# 1250
 ans=0U;_TL11B: _TL114: goto _LL5;}}}_TL10F:{struct Cyc_Warn_String_Warn_Warg_struct _TB7;_TB7.tag=0;
# 1254
_TB7.f1=_tag_fat("FIX: ptr_use_to_c, bad annotation",sizeof(char),34U);_TA9=_TB7;}{struct Cyc_Warn_String_Warn_Warg_struct _TB7=_TA9;void*_TB8[1];_TAA=_TB8 + 0;*_TAA=& _TB7;{int(*_TB9)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TAB=_TB9;}_TAC=_tag_fat(_TB8,sizeof(void*),1);_TAB(_TAC);}_LL5:;}
# 1256
Cyc_Toc_set_lhs(nv,old_lhs);_TAD=ans;
return _TAD;}_TLF9:{struct Cyc_Warn_String_Warn_Warg_struct _TB6;_TB6.tag=0;
_TB6.f1=_tag_fat("ptr_use_to_c: non-pointer-type",sizeof(char),31U);_TAE=_TB6;}{struct Cyc_Warn_String_Warn_Warg_struct _TB6=_TAE;void*_TB7[1];_TAF=_TB7 + 0;*_TAF=& _TB6;{int(*_TB8)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TB0=_TB8;}_TB1=_tag_fat(_TB7,sizeof(void*),1);_TB0(_TB1);};}}}
# 1262
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;void**_T3;int(*_T4)(struct _fat_ptr ap);struct _fat_ptr _T5;struct Cyc_Absyn_Exp*_T6;void*_T7;_T0=e;_T1=_T0->topt;
if(_T1!=0)goto _TL120;{struct Cyc_Warn_String_Warn_Warg_struct _T8;_T8.tag=0;_T8.f1=_tag_fat("Missing type in primop ",sizeof(char),24U);_T2=_T8;}{struct Cyc_Warn_String_Warn_Warg_struct _T8=_T2;void*_T9[1];_T3=_T9 + 0;*_T3=& _T8;{int(*_TA)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T4=_TA;}_T5=_tag_fat(_T9,sizeof(void*),1);_T4(_T5);}goto _TL121;_TL120: _TL121: _T6=e;_T7=_T6->topt;
return _T7;}
# 1266
static struct _tuple24*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple24*_T0;struct Cyc_Absyn_Exp*_T1;void*_T2;void*_T3;{struct _tuple24*_T4=_cycalloc(sizeof(struct _tuple24));
_T4->f0=Cyc_Toc_mt_tq;_T1=e;_T2=_T1->topt;_T3=_check_null(_T2);_T4->f1=Cyc_Toc_typ_to_c(_T3);_T0=(struct _tuple24*)_T4;}return _T0;}
# 1271
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple20*_T5;struct _tuple20 _T6;struct Cyc_List_List*_T7;int(*_T8)(struct _fat_ptr);struct _fat_ptr _T9;struct Cyc_List_List*_TA;int _TB;struct Cyc_Absyn_Exp*_TC;void*_TD;struct Cyc_Absyn_Exp*_TE;int _TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;void*_T12;struct Cyc_Absyn_Exp*_T13;struct Cyc_Absyn_Exp*_T14;void*_T15;_T0=e;{
void*_T16=_T0->r;int _T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_List_List*_T19;_T1=(int*)_T16;_T2=*_T1;switch(_T2){case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T1A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T16;_T19=_T1A->f1;}{struct Cyc_List_List*dles=_T19;{
# 1274
struct Cyc_List_List*dles2=dles;_TL126: if(dles2!=0)goto _TL124;else{goto _TL125;}
_TL124: _T3=dles2;_T4=_T3->hd;_T5=(struct _tuple20*)_T4;_T6=*_T5;_T7=_T6.f0;if(_T7==0)goto _TL127;{
int(*_T1A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_T8=_T1A;}_T9=_tag_fat("array designators for abstract-field initialization",sizeof(char),52U);_T8(_T9);goto _TL128;_TL127: _TL128: _TA=dles2;
# 1274
dles2=_TA->tl;goto _TL126;_TL125:;}_TB=
# 1277
Cyc_List_length(dles);_TC=Cyc_Absyn_signed_int_exp(_TB,0U);_TD=Cyc_Absyn_sint_type;_T18=Cyc_Absyn_set_type(_TC,_TD);_T17=0;goto _LL4;}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T1A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T16;_T18=_T1A->f2;_T17=_T1A->f4;}_LL4: {struct Cyc_Absyn_Exp*bd=_T18;int zt=_T17;_T18=bd;_T17=zt;goto _LL6;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T1A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T16;_T18=_T1A->f1;_T17=_T1A->f3;}_LL6: {struct Cyc_Absyn_Exp*bd=_T18;int zt=_T17;
# 1280
bd=Cyc_Absyn_copy_exp(bd);_TF=zt;
if(!_TF)goto _TL129;_T10=bd;_T11=Cyc_Absyn_uint_exp(1U,0U);_T12=Cyc_Absyn_uint_type;_T13=Cyc_Absyn_set_type(_T11,_T12);_T14=Cyc_Absyn_add_exp(_T10,_T13,0U);_T15=Cyc_Absyn_uint_type;_TE=Cyc_Absyn_set_type(_T14,_T15);goto _TL12A;_TL129: _TE=bd;_TL12A: return _TE;}default:
 return 0;};}}
# 1289
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;int*_T6;int _T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;int*_TA;int _TB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC;void*_TD;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_KnownAggr _T10;unsigned _T11;void*_T12;union Cyc_Absyn_AggrInfo _T13;struct _union_AggrInfo_KnownAggr _T14;struct Cyc_Absyn_Aggrdecl**_T15;struct Cyc_Absyn_Aggrdecl*_T16;struct Cyc_Absyn_AggrdeclImpl*_T17;struct Cyc_Absyn_Aggrdecl*_T18;enum Cyc_Absyn_AggrKind _T19;int _T1A;struct Cyc_Absyn_Aggrdecl*_T1B;struct Cyc_Absyn_AggrdeclImpl*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_List_List*_T22;void*_T23;struct _fat_ptr*_T24;struct Cyc_Absyn_Aggrfield*_T25;struct _fat_ptr*_T26;int _T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_Absyn_Aggrfield*_T29;void*_T2A;int*_T2B;int _T2C;struct Cyc_Absyn_ArrayInfo _T2D;struct Cyc_Absyn_ArrayInfo _T2E;int _T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp**_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_Absyn_Exp**_T33;struct Cyc_Absyn_Exp*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;struct _fat_ptr _T37;void*_T38;struct Cyc_Absyn_Exp*_T39;int _T3A;struct Cyc_Absyn_Exp*_T3B;void*_T3C;struct Cyc_Absyn_Exp*_T3D;struct Cyc_Absyn_Exp*_T3E;void*_T3F;struct Cyc_Absyn_Exp*_T40;struct Cyc_List_List*_T41;struct Cyc_Warn_String_Warn_Warg_struct _T42;void**_T43;int(*_T44)(struct _fat_ptr ap);struct _fat_ptr _T45;
# 1297
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;_T0=e;{
void*_T46=_T0->r;struct Cyc_List_List*_T47;_T1=(int*)_T46;_T2=*_T1;if(_T2!=28)goto _TL12B;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T48=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T46;_T47=_T48->f3;}{struct Cyc_List_List*dles2=_T47;
dles=dles2;goto _LL0;}_TL12B:
 return 0;_LL0:;}_T3=e;_T4=_T3->topt;_T5=
# 1303
_check_null(_T4);{void*_T46=Cyc_Absyn_compress(_T5);struct Cyc_Absyn_Aggrdecl*_T47;_T6=(int*)_T46;_T7=*_T6;if(_T7!=0)goto _TL12D;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T46;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=24)goto _TL12F;_TC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T46;_TD=_TC->f1;_TE=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TD;_TF=_TE->f1;_T10=_TF.KnownAggr;_T11=_T10.tag;if(_T11!=2)goto _TL131;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T48=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T46;_T12=_T48->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T49=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T12;_T13=_T49->f1;_T14=_T13.KnownAggr;_T15=_T14.val;{struct Cyc_Absyn_Aggrdecl*_T4A=*_T15;_T47=_T4A;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T47;_T16=ad;_T17=_T16->impl;
# 1305
if(_T17==0)goto _TL135;else{goto _TL136;}_TL136: _T18=ad;_T19=_T18->kind;_T1A=(int)_T19;if(_T1A==1)goto _TL135;else{goto _TL133;}
_TL135: return 0;_TL133: _T1B=ad;_T1C=_T1B->impl;
field_types=_T1C->fields;goto _LL5;}_TL131: goto _LL8;_TL12F: goto _LL8;_TL12D: _LL8:
# 1311
 return 0;_LL5:;}
# 1313
if(field_types!=0)goto _TL137;
return 0;_TL137:
 _TL13C: _T1D=_check_null(field_types);_T1E=_T1D->tl;if(_T1E!=0)goto _TL13A;else{goto _TL13B;}
_TL13A: _T1F=field_types;
# 1315
field_types=_T1F->tl;goto _TL13C;_TL13B: _T20=field_types;_T21=_T20->hd;{
# 1317
struct Cyc_Absyn_Aggrfield*last_type_field=(struct Cyc_Absyn_Aggrfield*)_T21;
_TL140: if(dles!=0)goto _TL13E;else{goto _TL13F;}
_TL13E: _T22=dles;_T23=_T22->hd;{struct _tuple20*_T46=(struct _tuple20*)_T23;struct Cyc_Absyn_Exp*_T47;struct Cyc_List_List*_T48;{struct _tuple20 _T49=*_T46;_T48=_T49.f0;_T47=_T49.f1;}{struct Cyc_List_List*ds=_T48;struct Cyc_Absyn_Exp*e2=_T47;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);_T24=f;_T25=last_type_field;_T26=_T25->name;_T27=
Cyc_strptrcmp(_T24,_T26);if(_T27)goto _TL141;else{goto _TL143;}
_TL143:{struct Cyc_Absyn_Exp*nested_ans=Cyc_Toc_get_varsizeexp(e2);
if(nested_ans==0)goto _TL144;_T28=nested_ans;
return _T28;_TL144: _T29=last_type_field;_T2A=_T29->type;{
void*_T49=Cyc_Absyn_compress(_T2A);struct Cyc_Absyn_Exp*_T4A;void*_T4B;_T2B=(int*)_T49;_T2C=*_T2B;if(_T2C!=5)goto _TL146;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T49;_T2D=_T4C->f1;_T4B=_T2D.elt_type;_T2E=_T4C->f1;_T4A=_T2E.num_elts;}{void*elt_type=_T4B;struct Cyc_Absyn_Exp*type_bd=_T4A;
# 1328
if(type_bd==0)goto _TL14A;else{goto _TL14B;}_TL14B: _T2F=Cyc_Toc_is_zero(type_bd);if(_T2F)goto _TL148;else{goto _TL14A;}
_TL14A: return 0;_TL148:{struct Cyc_Absyn_Exp*_T4C[2];_T31=_T4C + 0;_T32=
# 1336
Cyc_Toc_array_length_exp(e2);*_T31=_check_null(_T32);_T33=_T4C + 1;_T34=
Cyc_Absyn_sizeoftype_exp(elt_type,0U);_T35=Cyc_Absyn_uint_type;*_T33=Cyc_Absyn_set_type(_T34,_T35);_T36=Cyc_Toc__check_times_e;_T37=_tag_fat(_T4C,sizeof(struct Cyc_Absyn_Exp*),2);_T30=Cyc_Toc_fncall_exp_dl(_T36,_T37);}_T38=Cyc_Absyn_uint_type;_T39=
# 1335
Cyc_Absyn_set_type(_T30,_T38);_T3A=(int)sizeof(double);_T3B=
# 1339
Cyc_Absyn_signed_int_exp(_T3A,0U);_T3C=Cyc_Absyn_sint_type;_T3D=Cyc_Absyn_set_type(_T3B,_T3C);_T3E=
# 1335
Cyc_Absyn_add_exp(_T39,_T3D,0U);_T3F=Cyc_Absyn_uint_type;_T40=Cyc_Absyn_set_type(_T3E,_T3F);
# 1334
return _T40;}_TL146:
# 1341
 return 0;;}}goto _TL142;_TL141: _TL142:;}}_T41=dles;
# 1318
dles=_T41->tl;goto _TL140;_TL13F:{struct Cyc_Warn_String_Warn_Warg_struct _T46;_T46.tag=0;
# 1345
_T46.f1=_tag_fat("get_varsizeexp: did not find last struct field",sizeof(char),47U);_T42=_T46;}{struct Cyc_Warn_String_Warn_Warg_struct _T46=_T42;void*_T47[1];_T43=_T47 + 0;*_T43=& _T46;{int(*_T48)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T44=_T48;}_T45=_tag_fat(_T47,sizeof(void*),1);_T44(_T45);}}}
# 1348
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){struct Cyc_Absyn_Aggrdecl*_T0;struct Cyc_Absyn_AggrdeclImpl*_T1;struct Cyc_Absyn_AggrdeclImpl*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Aggrfield*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;int _TA;int _TB;struct Cyc_List_List*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct _fat_ptr*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;void**_T12;void**_T13;int(*_T14)(struct _fat_ptr ap);struct _fat_ptr _T15;_T0=ad;_T1=_T0->impl;_T2=
_check_null(_T1);{struct Cyc_List_List*fs=_T2->fields;{
int i=1;_TL14F: if(fs!=0)goto _TL14D;else{goto _TL14E;}
_TL14D: _T3=fs;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Aggrfield*)_T4;_T6=_T5->name;_T7=*_T6;_T8=f;_T9=*_T8;_TA=Cyc_strcmp(_T7,_T9);if(_TA!=0)goto _TL150;_TB=i;
return _TB;_TL150: _TC=fs;
# 1350
fs=_TC->tl;i=i + 1;goto _TL14F;_TL14E:;}{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;
# 1353
_T16.f1=_tag_fat("get_member_offset ",sizeof(char),19U);_TD=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;_TF=f;_T17.f1=*_TF;_TE=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;_T18.f1=_tag_fat(" failed",sizeof(char),8U);_T10=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_T10;void*_T19[3];_T11=_T19 + 0;*_T11=& _T16;_T12=_T19 + 1;*_T12=& _T17;_T13=_T19 + 2;*_T13=& _T18;{int(*_T1A)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T14=_T1A;}_T15=_tag_fat(_T19,sizeof(void*),3);_T14(_T15);}}}}}struct _tuple34{struct Cyc_Core_Opt*f0;struct Cyc_Absyn_Exp*f1;};
# 1357
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple34*pr){struct Cyc_Absyn_Exp*_T0;struct _tuple34*_T1;struct _tuple34 _T2;struct Cyc_Core_Opt*_T3;struct _tuple34*_T4;struct _tuple34 _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Exp*_T9;_T0=el;_T1=pr;_T2=*_T1;_T3=_T2.f0;_T4=pr;_T5=*_T4;_T6=_T5.f1;{
struct Cyc_Absyn_Exp*res=Cyc_Absyn_assignop_exp(_T0,_T3,_T6,0U);_T7=res;_T8=el;
_T7->topt=_T8->topt;_T9=res;
return _T9;}}
# 1362
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;struct Cyc_Absyn_Tqual _T5;struct Cyc_Absyn_Exp*_T6;
struct Cyc_Absyn_Exp*res=Cyc_Absyn_address_exp(e1,0U);_T0=e1;_T1=_T0->topt;
if(_T1==0)goto _TL152;_T2=res;_T3=e1;_T4=_T3->topt;_T5=Cyc_Toc_mt_tq;_T2->topt=Cyc_Absyn_cstar_type(_T4,_T5);goto _TL153;_TL152: _TL153: _T6=res;
return _T6;}
# 1367
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Absyn_Exp*_T4;{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T5=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_T5->tag=5;
_T5->f1=e1;_T5->f2=incr;_T0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T5;}_T1=(void*)_T0;{struct Cyc_Absyn_Exp*res=Cyc_Absyn_new_exp(_T1,0U);_T2=res;_T3=e1;
_T2->topt=_T3->topt;_T4=res;
return _T4;}}
# 1373
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,void*),void*);
# 1382
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Stmt*_T3;struct Cyc_List_List*_T4;struct Cyc_Absyn_Exp*(*_T5)(struct Cyc_Absyn_Exp*,void*);void*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_Exp*(*_TA)(struct Cyc_Absyn_Exp*,void*);void*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_List_List*_TF;struct Cyc_Absyn_Exp*(*_T10)(struct Cyc_Absyn_Exp*,void*);void*_T11;struct Cyc_Absyn_Exp*_T12;struct Cyc_List_List*_T13;void*_T14;struct _fat_ptr*_T15;struct Cyc_Absyn_Exp*_T16;unsigned _T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Exp*_T1A;_T0=e1;{
# 1384
void*_T1B=_T0->r;int _T1C;int _T1D;struct _fat_ptr*_T1E;struct Cyc_Absyn_Exp*_T1F;void*_T20;_T1=(int*)_T1B;_T2=*_T1;switch(_T2){case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T21=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T1B;_T20=_T21->f1;}{struct Cyc_Absyn_Stmt*s=_T20;_T3=s;_T4=fs;_T5=f;_T6=f_env;
Cyc_Toc_lvalue_assign_stmt(_T3,_T4,_T5,_T6);goto _LL0;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T21=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T7=_T21->f1;_T20=(void*)_T7;_T1F=_T21->f2;}{void*t=_T20;struct Cyc_Absyn_Exp*e=_T1F;_T8=e;_T9=fs;_TA=f;_TB=f_env;
Cyc_Toc_lvalue_assign(_T8,_T9,_TA,_TB);goto _LL0;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T21=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T1B;_T1F=_T21->f1;_T1E=_T21->f2;_T1D=_T21->f3;_T1C=_T21->f4;}{struct Cyc_Absyn_Exp*e=_T1F;struct _fat_ptr*fld=_T1E;int is_tagged=_T1D;int is_read=_T1C;_TC=e1;_TD=e;
# 1389
_TC->r=_TD->r;_TE=e1;{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
_T21->hd=fld;_T21->tl=fs;_TF=(struct Cyc_List_List*)_T21;}_T10=f;_T11=f_env;Cyc_Toc_lvalue_assign(_TE,_TF,_T10,_T11);goto _LL0;}default:  {
# 1397
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1399
_TL158: if(fs!=0)goto _TL156;else{goto _TL157;}
_TL156: _T12=e1_copy;_T13=fs;_T14=_T13->hd;_T15=(struct _fat_ptr*)_T14;_T16=e1_copy;_T17=_T16->loc;e1_copy=Cyc_Toc_member_exp(_T12,_T15,_T17);_T18=fs;
# 1399
fs=_T18->tl;goto _TL158;_TL157: _T19=e1;_T1A=
# 1401
f(e1_copy,f_env);_T19->r=_T1A->r;goto _LL0;}}_LL0:;}}
# 1405
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_List_List*_T4;struct Cyc_Absyn_Exp*(*_T5)(struct Cyc_Absyn_Exp*,void*);void*_T6;struct Cyc_Absyn_Stmt*_T7;struct Cyc_List_List*_T8;struct Cyc_Absyn_Exp*(*_T9)(struct Cyc_Absyn_Exp*,void*);void*_TA;struct Cyc_Absyn_Stmt*_TB;struct Cyc_List_List*_TC;struct Cyc_Absyn_Exp*(*_TD)(struct Cyc_Absyn_Exp*,void*);void*_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_Stmt_Warn_Warg_struct _T10;void**_T11;void**_T12;struct _fat_ptr _T13;_T0=s;{
# 1407
void*_T14=_T0->r;struct Cyc_Absyn_Stmt*_T15;struct Cyc_Absyn_Decl*_T16;struct Cyc_Absyn_Exp*_T17;_T1=(int*)_T14;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T18=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T14;_T17=_T18->f1;}{struct Cyc_Absyn_Exp*e1=_T17;_T3=e1;_T4=fs;_T5=f;_T6=f_env;
Cyc_Toc_lvalue_assign(_T3,_T4,_T5,_T6);goto _LL0;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T18=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T14;_T16=_T18->f1;_T15=_T18->f2;}{struct Cyc_Absyn_Decl*d=_T16;struct Cyc_Absyn_Stmt*s2=_T15;_T7=s2;_T8=fs;_T9=f;_TA=f_env;
# 1410
Cyc_Toc_lvalue_assign_stmt(_T7,_T8,_T9,_TA);goto _LL0;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T18=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T14;_T15=_T18->f2;}{struct Cyc_Absyn_Stmt*s2=_T15;_TB=s2;_TC=fs;_TD=f;_TE=f_env;
Cyc_Toc_lvalue_assign_stmt(_TB,_TC,_TD,_TE);goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;
_T18.f1=_tag_fat("lvalue_assign_stmt: ",sizeof(char),21U);_TF=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_TF;{struct Cyc_Warn_Stmt_Warn_Warg_struct _T19;_T19.tag=5;_T19.f1=s;_T10=_T19;}{struct Cyc_Warn_Stmt_Warn_Warg_struct _T19=_T10;void*_T1A[2];_T11=_T1A + 0;*_T11=& _T18;_T12=_T1A + 1;*_T12=& _T19;_T13=_tag_fat(_T1A,sizeof(void*),2);Cyc_Toc_toc_impos(_T13);}}}_LL0:;}}
# 1418
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T3;void**_T4;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T5;struct Cyc_Absyn_Exp**_T6;struct Cyc_Absyn_Exp**_T7;struct Cyc_Absyn_Exp**_T8;struct Cyc_Absyn_Exp*_T9;void**_TA;void**_TB;void*_TC;struct Cyc_Absyn_Tqual _TD;struct Cyc_Absyn_Exp*_TE;void**_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Exp*_T12;int _T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Exp_Warn_Warg_struct _T16;void**_T17;void**_T18;int(*_T19)(struct _fat_ptr ap);struct _fat_ptr _T1A;_T0=e;{
void*_T1B=_T0->r;struct Cyc_Absyn_Stmt*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;void*_T1F;_T1=(int*)_T1B;_T2=*_T1;switch(_T2){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T20=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T4=& _T3->f1;_T1F=(void**)_T4;_T5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T6=& _T5->f2;_T1E=(struct Cyc_Absyn_Exp**)_T6;}{void**t=(void**)_T1F;struct Cyc_Absyn_Exp**e1=(struct Cyc_Absyn_Exp**)_T1E;_T7=e1;_T8=e1;_T9=*_T8;
# 1422
*_T7=Cyc_Toc_push_address_exp(_T9);_TA=t;_TB=t;_TC=*_TB;_TD=Cyc_Toc_mt_tq;
*_TA=Cyc_Absyn_cstar_type(_TC,_TD);_TE=e;_TF=t;
_TE->topt=*_TF;_T10=e;
return _T10;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T20=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T1B;_T1D=_T20->f1;}{struct Cyc_Absyn_Exp*e1=_T1D;_T11=e1;
return _T11;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T20=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T1B;_T1C=_T20->f1;}{struct Cyc_Absyn_Stmt*s=_T1C;
# 1429
Cyc_Toc_push_address_stmt(s);_T12=e;return _T12;}default: _T13=
# 1431
Cyc_Absyn_is_lvalue(e);if(!_T13)goto _TL15B;_T14=Cyc_Toc_address_lvalue(e,0);return _T14;_TL15B:{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
_T20.f1=_tag_fat("can't take & of exp ",sizeof(char),21U);_T15=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T15;{struct Cyc_Warn_Exp_Warn_Warg_struct _T21;_T21.tag=4;_T21.f1=e;_T16=_T21;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T21=_T16;void*_T22[2];_T17=_T22 + 0;*_T17=& _T20;_T18=_T22 + 1;*_T18=& _T21;{int(*_T23)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T19=_T23;}_T1A=_tag_fat(_T22,sizeof(void*),2);_T19(_T1A);}}};}}
# 1435
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T3;struct Cyc_Absyn_Exp**_T4;struct Cyc_Absyn_Exp**_T5;struct Cyc_Absyn_Exp**_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;struct Cyc_Warn_Stmt_Warn_Warg_struct _T9;void**_TA;void**_TB;int(*_TC)(struct _fat_ptr ap);struct _fat_ptr _TD;_T0=s;{
void*_TE=_T0->r;void*_TF;_T1=(int*)_TE;_T2=*_T1;switch(_T2){case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T10=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_TE;_TF=_T10->f2;}{struct Cyc_Absyn_Stmt*s2=_TF;_TF=s2;goto _LL4;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T10=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_TE;_TF=_T10->f2;}_LL4: {struct Cyc_Absyn_Stmt*s2=_TF;
# 1438
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T10=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TE;_T3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TE;_T4=& _T3->f1;_TF=(struct Cyc_Absyn_Exp**)_T4;}{struct Cyc_Absyn_Exp**e=(struct Cyc_Absyn_Exp**)_TF;_T5=e;_T6=e;_T7=*_T6;
*_T5=Cyc_Toc_push_address_exp(_T7);goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T10;_T10.tag=0;
_T10.f1=_tag_fat("can't take & of stmt ",sizeof(char),22U);_T8=_T10;}{struct Cyc_Warn_String_Warn_Warg_struct _T10=_T8;{struct Cyc_Warn_Stmt_Warn_Warg_struct _T11;_T11.tag=5;_T11.f1=s;_T9=_T11;}{struct Cyc_Warn_Stmt_Warn_Warg_struct _T11=_T9;void*_T12[2];_TA=_T12 + 0;*_TA=& _T10;_TB=_T12 + 1;*_TB=& _T11;{int(*_T13)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TC=_T13;}_TD=_tag_fat(_T12,sizeof(void*),2);_TC(_TD);}}}_LL0:;}}
# 1447
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){struct Cyc_Core_Opt*_T0;struct Cyc_Absyn_Exp*_T1;int*_T2;unsigned _T3;int _T4;struct Cyc_Absyn_Exp*_T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T7;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T8;int _T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_TC;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Absyn_Exp**_TF;struct Cyc_Absyn_Exp**_T10;void*_T11;struct Cyc_Absyn_Exp*_T12;void*_T13;struct Cyc_Absyn_Exp**_T14;struct Cyc_Absyn_Exp*_T15;struct _fat_ptr _T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;void**_T19;int(*_T1A)(struct _fat_ptr ap);struct _fat_ptr _T1B;struct Cyc_Absyn_Vardecl*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Absyn_Vardecl*_T1E;struct Cyc_Absyn_Vardecl*_T1F;struct Cyc_Absyn_Vardecl*_T20;struct Cyc_Absyn_Vardecl*_T21;struct Cyc_Absyn_Vardecl*_T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_Absyn_Vardecl*_T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Vardecl*_T26;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T27;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T28;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T29;void*_T2A;struct Cyc_Absyn_Exp*_T2B;struct Cyc_Absyn_Exp*_T2C;int _T2D;struct Cyc_Absyn_Exp*_T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*_T31;struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*_T32;struct Cyc_Absyn_Vardecl*_T33;struct Cyc_Absyn_Vardecl*_T34;struct Cyc_Absyn_Vardecl*_T35;struct Cyc_Absyn_Vardecl*_T36;struct Cyc_Absyn_Vardecl*_T37;struct Cyc_Absyn_Vardecl*_T38;struct Cyc_Absyn_Vardecl*_T39;struct Cyc_Absyn_Vardecl*_T3A;struct Cyc_Absyn_Vardecl*_T3B;struct Cyc_Absyn_Vardecl*_T3C;struct Cyc_Absyn_Vardecl*_T3D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T3E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T3F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T40;void*_T41;struct Cyc_Absyn_Exp*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Core_Opt*_T44;void*_T45;enum Cyc_Absyn_Primop _T46;struct Cyc_Absyn_Exp*_T47;struct Cyc_Absyn_Exp*_T48;struct Cyc_Absyn_Exp*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_Absyn_Exp*_T4B;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T4C;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T4D;struct Cyc_Absyn_Vardecl*_T4E;struct Cyc_Absyn_Vardecl*_T4F;struct Cyc_Absyn_Vardecl*_T50;struct Cyc_Absyn_Vardecl*_T51;struct Cyc_Absyn_Vardecl*_T52;struct Cyc_Absyn_Vardecl*_T53;struct Cyc_Absyn_Vardecl*_T54;struct Cyc_Absyn_Vardecl*_T55;struct Cyc_Absyn_Vardecl*_T56;struct Cyc_Absyn_Vardecl*_T57;struct Cyc_Absyn_Vardecl*_T58;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T59;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T5A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T5B;void*_T5C;struct Cyc_Absyn_Exp*_T5D;struct Cyc_Absyn_Exp*_T5E;struct Cyc_Absyn_Exp*_T5F;struct Cyc_Absyn_Exp*_T60;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T61;void*_T62;struct Cyc_Absyn_Exp*_T63;struct Cyc_Absyn_Exp*_T64;struct Cyc_Absyn_Exp*_T65;struct Cyc_Absyn_Exp*_T66;struct Cyc_List_List*_T67;struct Cyc_Absyn_Exp**_T68;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T69;void*_T6A;struct Cyc_Absyn_Exp*_T6B;void*_T6C;struct Cyc_Absyn_Exp**_T6D;void*_T6E;struct Cyc_Absyn_Exp*_T6F;void*_T70;struct _fat_ptr _T71;struct Cyc_Absyn_Exp*_T72;void*_T73;struct Cyc_Absyn_Exp*_T74;void*_T75;struct Cyc_Absyn_Exp*_T76;struct Cyc_Absyn_Exp*_T77;struct Cyc_Absyn_Exp*_T78;struct Cyc_Absyn_Exp*_T79;struct Cyc_Absyn_Exp*_T7A;struct _fat_ptr _T7B;struct Cyc_Absyn_Exp*_T7C;void*_T7D;struct Cyc_Absyn_Exp*_T7E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T7F;void*_T80;struct Cyc_Absyn_Exp*_T81;void*_T82;struct Cyc_Absyn_Exp*_T83;struct Cyc_Absyn_Exp*_T84;void*_T85;struct Cyc_Absyn_Exp*_T86;struct Cyc_Absyn_Exp*_T87;void*_T88;struct Cyc_Absyn_Exp*_T89;struct Cyc_Absyn_Exp*_T8A;void*_T8B;struct Cyc_Absyn_Exp*_T8C;struct Cyc_Absyn_Stmt*_T8D;struct Cyc_Absyn_Stmt*_T8E;struct Cyc_Absyn_Stmt*_T8F;struct Cyc_Absyn_Stmt*_T90;struct Cyc_Absyn_Decl*_T91;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T92;struct Cyc_Absyn_Stmt*_T93;struct Cyc_Absyn_Decl*_T94;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T95;struct Cyc_Absyn_Stmt*_T96;struct Cyc_Absyn_Decl*_T97;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T98;struct Cyc_Absyn_Stmt*_T99;struct Cyc_Absyn_Exp*_T9A;
# 1459
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*c_elt_type=Cyc_Toc_typ_to_c(elt_type);
void*c_fat_ptr_type=Cyc_Toc_typ_to_c(fat_ptr_type);
void*c_ptr_type=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*c_ptr_type_opt;c_ptr_type_opt=_cycalloc(sizeof(struct Cyc_Core_Opt));_T0=c_ptr_type_opt;_T0->v=c_ptr_type;{
struct Cyc_Absyn_Exp*xinit;_T1=e1;{
void*_T9B=_T1->r;struct Cyc_Absyn_Exp*_T9C;struct Cyc_Absyn_Exp*_T9D;_T2=(int*)_T9B;_T3=*_T2;switch(_T3){case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T9E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T9B;_T9D=_T9E->f1;}{struct Cyc_Absyn_Exp*ea=_T9D;_T4=is_fat;
# 1467
if(_T4)goto _TL15F;else{goto _TL161;}
_TL161: ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);_T5=ea;
_T5->topt=fat_ptr_type;_T6=ea;_T7=& Cyc_InsertChecks_NoCheck_val;_T8=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_T7;
_T6->annot=(void*)_T8;goto _TL160;_TL15F: _TL160:
# 1472
 Cyc_Toc_exp_to_c(nv,ea);
xinit=ea;goto _LL0;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T9E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T9B;_T9D=_T9E->f1;_T9C=_T9E->f2;}{struct Cyc_Absyn_Exp*ea=_T9D;struct Cyc_Absyn_Exp*eb=_T9C;_T9=is_fat;
# 1476
if(_T9)goto _TL162;else{goto _TL164;}
_TL164: ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);_TA=ea;
_TA->topt=fat_ptr_type;_TB=ea;_TC=& Cyc_InsertChecks_NoCheck_val;_TD=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_TC;
_TB->annot=(void*)_TD;goto _TL163;_TL162: _TL163:
# 1481
 Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);{struct Cyc_Absyn_Exp*_T9E[3];_TF=_T9E + 0;
# 1483
*_TF=ea;_T10=_T9E + 1;_T11=Cyc_Toc_typ_to_c(elt_type);_T12=Cyc_Absyn_sizeoftype_exp(_T11,0U);_T13=Cyc_Absyn_uint_type;*_T10=Cyc_Absyn_set_type(_T12,_T13);_T14=_T9E + 2;*_T14=eb;_T15=Cyc_Toc__fat_ptr_plus_e;_T16=_tag_fat(_T9E,sizeof(struct Cyc_Absyn_Exp*),3);_TE=Cyc_Toc_fncall_exp_dl(_T15,_T16);}
# 1482
xinit=_TE;_T17=xinit;
# 1484
_T17->topt=c_fat_ptr_type;goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T9E;_T9E.tag=0;
# 1486
_T9E.f1=_tag_fat("found bad lhs for zero-terminated pointer assignment",sizeof(char),53U);_T18=_T9E;}{struct Cyc_Warn_String_Warn_Warg_struct _T9E=_T18;void*_T9F[1];_T19=_T9F + 0;*_T19=& _T9E;{int(*_TA0)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T1A=_TA0;}_T1B=_tag_fat(_T9F,sizeof(void*),1);_T1A(_T1B);}}_LL0:;}{
# 1488
struct _tuple1*x=Cyc_Toc_temp_var();struct _RegionHandle _T9B=_new_region(0U,"rgn2");struct _RegionHandle*rgn2=& _T9B;_push_region(rgn2);{
# 1490
struct Cyc_Absyn_Vardecl*x_vd;x_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T1C=x_vd;_T1C->sc=2U;_T1D=x_vd;_T1D->name=x;_T1E=x_vd;_T1E->varloc=0U;_T1F=x_vd;_T1F->tq=Cyc_Toc_mt_tq;_T20=x_vd;_T20->type=c_fat_ptr_type;_T21=x_vd;_T21->initializer=xinit;_T22=x_vd;_T22->rgn=0;_T23=x_vd;_T23->attributes=0;_T24=x_vd;_T24->escapes=0;_T25=x_vd;_T25->is_proto=0;_T26=x_vd;_T26->rename=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd;x_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T27=x_bnd;_T27->tag=4;_T28=x_bnd;_T28->f1=x_vd;_T29=x_bnd;_T2A=(void*)_T29;{
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_varb_exp(_T2A,0U);_T2B=x_exp;
_T2B->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*deref_x=Cyc_Absyn_deref_exp(x_exp,0U);_T2C=deref_x;
_T2C->topt=elt_type;_T2D=is_fat;
# 1498
if(!_T2D)goto _TL165;_T2E=deref_x;_T2F=e1;_T2E->annot=_T2F->annot;goto _TL166;
_TL165: _T30=deref_x;_T31=& Cyc_InsertChecks_NullAndFatBound_val;_T32=(struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_T31;_T30->annot=(void*)_T32;_TL166:
 Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*y=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*y_vd;y_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T33=y_vd;_T33->sc=2U;_T34=y_vd;_T34->name=y;_T35=y_vd;_T35->varloc=0U;_T36=y_vd;_T36->tq=Cyc_Toc_mt_tq;_T37=y_vd;_T37->type=c_elt_type;_T38=y_vd;_T38->initializer=deref_x;_T39=y_vd;_T39->rgn=0;_T3A=y_vd;_T3A->attributes=0;_T3B=y_vd;_T3B->escapes=0;_T3C=y_vd;_T3C->is_proto=0;_T3D=y_vd;_T3D->rename=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd;y_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T3E=y_bnd;_T3E->tag=4;_T3F=y_bnd;_T3F->f1=y_vd;{
struct Cyc_Absyn_Exp*z_init=e2;
if(popt==0)goto _TL167;_T40=y_bnd;_T41=(void*)_T40;{
struct Cyc_Absyn_Exp*y_exp=Cyc_Absyn_varb_exp(_T41,0U);_T42=y_exp;_T43=deref_x;
_T42->topt=_T43->topt;_T44=popt;_T45=_T44->v;_T46=(enum Cyc_Absyn_Primop)_T45;_T47=y_exp;_T48=
Cyc_Absyn_copy_exp(e2);z_init=Cyc_Absyn_prim2_exp(_T46,_T47,_T48,0U);_T49=z_init;_T4A=y_exp;
_T49->topt=_T4A->topt;_T4B=z_init;_T4C=& Cyc_InsertChecks_NoCheck_val;_T4D=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_T4C;
_T4B->annot=(void*)_T4D;}goto _TL168;_TL167: _TL168:
# 1512
 Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*z=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*z_vd;z_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T4E=z_vd;_T4E->sc=2U;_T4F=z_vd;_T4F->name=z;_T50=z_vd;_T50->varloc=0U;_T51=z_vd;_T51->tq=Cyc_Toc_mt_tq;_T52=z_vd;_T52->type=c_elt_type;_T53=z_vd;_T53->initializer=z_init;_T54=z_vd;_T54->rgn=0;_T55=z_vd;_T55->attributes=0;_T56=z_vd;_T56->escapes=0;_T57=z_vd;_T57->is_proto=0;_T58=z_vd;_T58->rename=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd;z_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T59=z_bnd;_T59->tag=4;_T5A=z_bnd;_T5A->f1=z_vd;_T5B=y_bnd;_T5C=(void*)_T5B;{
# 1518
struct Cyc_Absyn_Exp*y2_exp=Cyc_Absyn_varb_exp(_T5C,0U);_T5D=y2_exp;_T5E=deref_x;_T5D->topt=_T5E->topt;{
struct Cyc_Absyn_Exp*zero1_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp1_exp=Cyc_Absyn_prim2_exp(5U,y2_exp,zero1_exp,0U);_T5F=zero1_exp;
_T5F->topt=Cyc_Absyn_sint_type;_T60=comp1_exp;
_T60->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);_T61=z_bnd;_T62=(void*)_T61;{
# 1525
struct Cyc_Absyn_Exp*z_exp=Cyc_Absyn_varb_exp(_T62,0U);_T63=z_exp;_T64=deref_x;_T63->topt=_T64->topt;{
struct Cyc_Absyn_Exp*zero2_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp2_exp=Cyc_Absyn_prim2_exp(6U,z_exp,zero2_exp,0U);_T65=zero2_exp;
_T65->topt=Cyc_Absyn_sint_type;_T66=comp2_exp;
_T66->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{struct Cyc_Absyn_Exp*_T9C[2];_T68=_T9C + 0;_T69=x_bnd;_T6A=(void*)_T69;_T6B=
# 1532
Cyc_Absyn_varb_exp(_T6A,0U);_T6C=c_fat_ptr_type;*_T68=Cyc_Absyn_set_type(_T6B,_T6C);_T6D=_T9C + 1;_T6E=
Cyc_Toc_typ_to_c(elt_type);_T6F=Cyc_Absyn_sizeoftype_exp(_T6E,0U);_T70=Cyc_Absyn_uint_type;*_T6D=Cyc_Absyn_set_type(_T6F,_T70);_T71=_tag_fat(_T9C,sizeof(struct Cyc_Absyn_Exp*),2);_T67=Cyc_List_list(_T71);}{
# 1532
struct Cyc_List_List*xsizeargs=_T67;_T72=
# 1534
Cyc_Absyn_uint_exp(1U,0U);_T73=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*oneexp=Cyc_Absyn_set_type(_T72,_T73);
struct Cyc_Absyn_Exp*xsize;_T74=
Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);_T75=Cyc_Absyn_uint_type;_T76=Cyc_Absyn_set_type(_T74,_T75);_T77=oneexp;xsize=Cyc_Absyn_prim2_exp(5U,_T76,_T77,0U);_T78=xsize;_T79=
# 1539
Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U);{struct Cyc_Absyn_Exp*comp_exp=Cyc_Absyn_and_exp(_T78,_T79,0U);_T7A=Cyc_Toc__throw_arraybounds_e;_T7B=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_T7C=
Cyc_Toc_fncall_exp_dl(_T7A,_T7B);_T7D=Cyc_Toc_void_star_type();_T7E=Cyc_Absyn_set_type(_T7C,_T7D);{struct Cyc_Absyn_Stmt*thr_stmt=Cyc_Absyn_exp_stmt(_T7E,0U);_T7F=x_bnd;_T80=(void*)_T7F;_T81=
Cyc_Absyn_varb_exp(_T80,0U);_T82=c_fat_ptr_type;_T83=Cyc_Absyn_set_type(_T81,_T82);{struct Cyc_Absyn_Exp*xcurr=Cyc_Toc_get_curr(_T83);
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);_T84=
Cyc_Absyn_deref_exp(xcurr,0U);_T85=c_elt_type;{struct Cyc_Absyn_Exp*deref_xcurr=Cyc_Absyn_set_type(_T84,_T85);_T86=deref_xcurr;_T87=
Cyc_Absyn_var_exp(z,0U);_T88=c_elt_type;_T89=Cyc_Absyn_set_type(_T87,_T88);_T8A=Cyc_Absyn_assign_exp(_T86,_T89,0U);_T8B=c_elt_type;{struct Cyc_Absyn_Exp*asn_exp=Cyc_Absyn_set_type(_T8A,_T8B);
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(asn_exp,0U);_T8C=comp_exp;_T8D=thr_stmt;_T8E=
Cyc_Absyn_skip_stmt(0U);_T8F=Cyc_Absyn_ifthenelse_stmt(_T8C,_T8D,_T8E,0U);_T90=s;s=Cyc_Absyn_seq_stmt(_T8F,_T90,0U);{struct Cyc_Absyn_Decl*_T9C=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T9D=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T9D->tag=0;
_T9D->f1=z_vd;_T92=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T9D;}_T9C->r=(void*)_T92;_T9C->loc=0U;_T91=(struct Cyc_Absyn_Decl*)_T9C;}_T93=s;s=Cyc_Absyn_decl_stmt(_T91,_T93,0U);{struct Cyc_Absyn_Decl*_T9C=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T9D=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T9D->tag=0;
_T9D->f1=y_vd;_T95=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T9D;}_T9C->r=(void*)_T95;_T9C->loc=0U;_T94=(struct Cyc_Absyn_Decl*)_T9C;}_T96=s;s=Cyc_Absyn_decl_stmt(_T94,_T96,0U);{struct Cyc_Absyn_Decl*_T9C=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T9D=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T9D->tag=0;
_T9D->f1=x_vd;_T98=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T9D;}_T9C->r=(void*)_T98;_T9C->loc=0U;_T97=(struct Cyc_Absyn_Decl*)_T9C;}_T99=s;s=Cyc_Absyn_decl_stmt(_T97,_T99,0U);_T9A=e;
_T9A->r=Cyc_Toc_stmt_exp_r(s);}}}}}}}}}}}}}}}}}}}}_pop_region();}}}
# 1565 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(void*res_type,struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){int*_T0;int _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;struct Cyc_Warn_Typ_Warn_Warg_struct _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;void**_TA;void**_TB;void**_TC;int(*_TD)(struct _fat_ptr ap);struct _fat_ptr _TE;int _TF;struct Cyc_Absyn_Exp*_T10;void*_T11;int _T12;struct Cyc_Absyn_Exp*_T13;void*_T14;struct Cyc_Absyn_Exp*_T15;void*_T16;struct Cyc_Absyn_Exp*_T17;struct _fat_ptr*_T18;struct Cyc_Absyn_Exp*_T19;void*_T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_Absyn_Stmt*_T1F;struct Cyc_Absyn_Stmt*_T20;struct Cyc_Absyn_Exp*_T21;void*_T22;struct Cyc_Absyn_Exp*_T23;struct Cyc_Absyn_Exp*_T24;void*_T25;struct _tuple1*_T26;void*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_Absyn_Stmt*_T29;void*_T2A;struct Cyc_Absyn_Exp*_T2B;void*_T2C;struct Cyc_Absyn_Exp*_T2D;struct _fat_ptr*_T2E;struct Cyc_Absyn_Exp*_T2F;void*_T30;struct Cyc_Absyn_Exp*_T31;struct _fat_ptr*_T32;struct Cyc_Absyn_Exp*_T33;void*_T34;struct Cyc_Absyn_Exp*_T35;struct _fat_ptr*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Stmt*_T38;struct Cyc_Absyn_Stmt*_T39;struct _tuple1*_T3A;void*_T3B;struct Cyc_Absyn_Exp*_T3C;struct Cyc_Absyn_Stmt*_T3D;void*_T3E;
# 1570
struct Cyc_Absyn_Aggrdecl*ad;{
void*_T3F=Cyc_Absyn_compress(aggrtype);union Cyc_Absyn_AggrInfo _T40;_T0=(int*)_T3F;_T1=*_T0;if(_T1!=0)goto _TL169;_T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3F;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TL16B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3F;_T6=_T41->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T42=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T40=_T42->f1;}}{union Cyc_Absyn_AggrInfo info=_T40;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}_TL16B: goto _LL3;_TL169: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T41;_T41.tag=0;
_T41.f1=_tag_fat("expecting union but found ",sizeof(char),27U);_T7=_T41;}{struct Cyc_Warn_String_Warn_Warg_struct _T41=_T7;{struct Cyc_Warn_Typ_Warn_Warg_struct _T42;_T42.tag=2;_T42.f1=aggrtype;_T8=_T42;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T42=_T8;{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;
_T43.f1=_tag_fat(" in check_tagged_union",sizeof(char),23U);_T9=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T9;void*_T44[3];_TA=_T44 + 0;*_TA=& _T41;_TB=_T44 + 1;*_TB=& _T42;_TC=_T44 + 2;*_TC=& _T43;{
# 1573
int(*_T45)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TD=_T45;}_TE=_tag_fat(_T44,sizeof(void*),3);_TD(_TE);}}}_LL0:;}{
# 1576
void*strct_name=Cyc_Toc_tagged_union_field_struct(aggrtype,f);
struct _tuple31 _T3F=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T40;struct _tuple1*_T41;_T41=_T3F.f0;_T40=_T3F.f1;{struct _tuple1*temp=_T41;struct Cyc_Absyn_Exp*temp_exp=_T40;_TF=
Cyc_Toc_get_member_offset(ad,f);_T10=Cyc_Absyn_signed_int_exp(_TF,0U);_T11=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_set_type(_T10,_T11);_T12=in_lhs;
if(!_T12)goto _TL16D;{
void*e1_p_type=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
Cyc_Absyn_set_type(temp_exp,e1_p_type);_T13=
Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);_T14=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*temp_f_tag=Cyc_Absyn_set_type(_T13,_T14);_T15=
Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);_T16=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_set_type(_T15,_T16);_T17=
Cyc_Absyn_copy_exp(temp_exp);_T18=Cyc_Toc_val_sp;_T19=Cyc_Absyn_aggrarrow_exp(_T17,_T18,0U);_T1A=res_type;{struct Cyc_Absyn_Exp*temp_f_val=Cyc_Absyn_set_type(_T19,_T1A);_T1B=
# 1586
Cyc_Absyn_address_exp(temp_f_val,0U);_T1C=Cyc_Absyn_cstar_type(res_type,Cyc_Toc_mt_tq);_T1D=Cyc_Absyn_set_type(_T1B,_T1C);{struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(_T1D,0U);_T1E=test_exp;_T1F=
Cyc_Toc_throw_match_stmt();_T20=Cyc_Toc_skip_stmt_dl();{struct Cyc_Absyn_Stmt*ifs=Cyc_Absyn_ifthenelse_stmt(_T1E,_T1F,_T20,0U);_T21=
aggrproj(e1,f,0U);_T22=res_type;_T23=Cyc_Absyn_set_type(_T21,_T22);_T24=Cyc_Absyn_address_exp(_T23,0U);_T25=e1_p_type;{struct Cyc_Absyn_Exp*e1_f=Cyc_Absyn_set_type(_T24,_T25);_T26=temp;_T27=e1_p_type;_T28=e1_f;_T29=
Cyc_Absyn_seq_stmt(ifs,sres,0U);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T26,_T27,_T28,_T29,0U);_T2A=
Cyc_Toc_stmt_exp_r(s);return _T2A;}}}}}}}}
# 1592
_TL16D: Cyc_Absyn_set_type(temp_exp,e1_c_type);_T2B=
aggrproj(temp_exp,f,0U);_T2C=strct_name;_T2D=Cyc_Absyn_set_type(_T2B,_T2C);_T2E=Cyc_Toc_tag_sp;_T2F=Cyc_Toc_member_exp(_T2D,_T2E,0U);_T30=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*temp_f_tag=Cyc_Absyn_set_type(_T2F,_T30);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);_T31=
Cyc_Absyn_copy_exp(temp_exp);_T32=f;_T33=aggrproj(_T31,_T32,0U);_T34=strct_name;_T35=Cyc_Absyn_set_type(_T33,_T34);_T36=Cyc_Toc_val_sp;{struct Cyc_Absyn_Exp*temp_f_val=Cyc_Toc_member_exp(_T35,_T36,0U);
Cyc_Absyn_set_type(temp_f_val,res_type);{
struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(temp_f_val,0U);_T37=test_exp;_T38=
Cyc_Toc_throw_match_stmt();_T39=Cyc_Toc_skip_stmt_dl();{struct Cyc_Absyn_Stmt*ifs=Cyc_Absyn_ifthenelse_stmt(_T37,_T38,_T39,0U);_T3A=temp;_T3B=e1_c_type;_T3C=e1;_T3D=
Cyc_Absyn_seq_stmt(ifs,sres,0U);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T3A,_T3B,_T3C,_T3D,0U);_T3E=
Cyc_Toc_stmt_exp_r(s);return _T3E;}}}}}}}}}
# 1604
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){int*_T0;int _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;void*_T6;int*_T7;void**_T8;int _T9;int*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_Absyn_AggrdeclImpl*_TC;struct Cyc_Absyn_AggrdeclImpl*_TD;int _TE;
# 1607
void*_TF=Cyc_Absyn_compress(t);union Cyc_Absyn_AggrInfo _T10;_T0=(int*)_TF;_T1=*_T0;if(_T1!=0)goto _TL16F;_T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TL171;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T6=_T11->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T12=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T10=_T12->f1;}}{union Cyc_Absyn_AggrInfo info=_T10;
# 1609
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T7=f_tag;
*_T7=Cyc_Toc_get_member_offset(ad,f);_T8=tagged_member_type;
*_T8=Cyc_Toc_tagged_union_field_struct(t,f);_T9=clear_read;
if(!_T9)goto _TL173;_TA=_check_null(is_read);*_TA=0;goto _TL174;_TL173: _TL174: _TB=ad;_TC=_TB->impl;_TD=
_check_null(_TC);_TE=_TD->tagged;return _TE;}_TL171: goto _LL3;_TL16F: _LL3:
 return 0;;}
# 1621
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;void**_T4;int(*_T5)(struct _fat_ptr ap);struct _fat_ptr _T6;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T7;int*_T8;struct Cyc_Absyn_Exp*_T9;void*_TA;void*_TB;struct _fat_ptr*_TC;int*_TD;void**_TE;int _TF;int*_T10;int _T11;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T12;int*_T13;struct Cyc_Absyn_Exp*_T14;void*_T15;void*_T16;int*_T17;int _T18;struct Cyc_Absyn_PtrInfo _T19;int _T1A;_T0=e;{
# 1624
void*_T1B=_T0->r;void*_T1C;struct _fat_ptr*_T1D;struct Cyc_Absyn_Exp*_T1E;_T1=(int*)_T1B;_T2=*_T1;switch(_T2){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T1E=_T1F->f2;}{struct Cyc_Absyn_Exp*e1=_T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T1F;_T1F.tag=0;
_T1F.f1=_tag_fat("cast on lhs!",sizeof(char),13U);_T3=_T1F;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F=_T3;void*_T20[1];_T4=_T20 + 0;*_T4=& _T1F;{int(*_T21)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T5=_T21;}_T6=_tag_fat(_T20,sizeof(void*),1);_T5(_T6);}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T1B;_T1E=_T1F->f1;_T1D=_T1F->f2;_T7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T1B;_T8=& _T7->f4;_T1C=(int*)_T8;}{struct Cyc_Absyn_Exp*e1=_T1E;struct _fat_ptr*f=_T1D;int*is_read=_T1C;_T9=e1;_TA=_T9->topt;_TB=
# 1627
_check_null(_TA);_TC=f;_TD=f_tag;_TE=tagged_member_type;_TF=clear_read;_T10=is_read;_T11=Cyc_Toc_is_tagged_union_project_impl(_TB,_TC,_TD,_TE,_TF,_T10);return _T11;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T1B;_T1E=_T1F->f1;_T1D=_T1F->f2;_T12=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T1B;_T13=& _T12->f4;_T1C=(int*)_T13;}{struct Cyc_Absyn_Exp*e1=_T1E;struct _fat_ptr*f=_T1D;int*is_read=(int*)_T1C;_T14=e1;_T15=_T14->topt;_T16=
# 1630
_check_null(_T15);{void*_T1F=Cyc_Absyn_compress(_T16);void*_T20;_T17=(int*)_T1F;_T18=*_T17;if(_T18!=4)goto _TL176;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T21=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1F;_T19=_T21->f1;_T20=_T19.elt_type;}{void*et=_T20;_T1A=
# 1632
Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);return _T1A;}_TL176:
# 1634
 return 0;;}}default:
# 1636
 return 0;};}}
# 1649 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;void*_T3;struct Cyc_Absyn_Exp*_T4;void*_T5;struct Cyc_Absyn_Exp*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Stmt*_TB;struct Cyc_Absyn_Stmt*_TC;struct _tuple1*_TD;void*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Stmt*_T10;void*_T11;
# 1653
struct _tuple31 _T12=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T13;struct _tuple1*_T14;_T14=_T12.f0;_T13=_T12.f1;{struct _tuple1*temp=_T14;struct Cyc_Absyn_Exp*temp_exp=_T13;_T0=temp_exp;_T1=
Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);Cyc_Absyn_set_type(_T0,_T1);_T2=
Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);_T3=member_type;{struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_set_type(_T2,_T3);_T4=
Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);_T5=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_set_type(_T4,_T5);_T6=
Cyc_Absyn_signed_int_exp(tag_num,0U);_T7=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_set_type(_T6,_T7);_T8=
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U);{struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(_T8,0U);
struct Cyc_Absyn_Stmt*s2;
if(popt!=0)goto _TL178;_T9=
Cyc_Absyn_assign_exp(temp_tag,f_tag,0U);s2=Cyc_Absyn_exp_stmt(_T9,0U);goto _TL179;
# 1663
_TL178:{struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);_TA=test_exp;_TB=
Cyc_Toc_throw_match_stmt();_TC=Cyc_Toc_skip_stmt_dl();s2=Cyc_Absyn_ifthenelse_stmt(_TA,_TB,_TC,0U);}_TL179: _TD=temp;_TE=
# 1666
Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);_TF=
Cyc_Toc_push_address_exp(e1);_T10=
Cyc_Absyn_seq_stmt(s2,s3,0U);{
# 1666
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_TD,_TE,_TF,_T10,0U);_T11=
# 1669
Cyc_Toc_stmt_exp_r(s1);return _T11;}}}}}}}struct _tuple35{struct _tuple1*f0;void*f1;};struct _tuple36{void*f0;void*f1;};struct _tuple37{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple38{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Exp*f1;};
# 1673
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;struct Cyc_Warn_Exp_Warn_Warg_struct _T3;void**_T4;void**_T5;int(*_T6)(unsigned,struct _fat_ptr);void*(*_T7)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Exp*_T8;unsigned _T9;struct _fat_ptr _TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_TE;char*_TF;char*_T10;struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*_T11;char*_T12;char*_T13;struct Cyc_Absyn_Exp*_T14;int*_T15;unsigned _T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T18;union Cyc_Absyn_Cnst _T19;struct _union_Cnst_Null_c _T1A;unsigned _T1B;struct Cyc_Absyn_Exp*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;int _T22;struct Cyc_Absyn_Exp*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_Exp*_T25;void*_T26;struct Cyc_Absyn_Exp**_T27;struct Cyc_Absyn_Exp**_T28;struct Cyc_Absyn_Exp**_T29;struct Cyc_Absyn_Exp*_T2A;struct _fat_ptr _T2B;struct Cyc_Absyn_Exp*_T2C;int _T2D;int _T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_Absyn_Exp*_T33;void*_T34;struct Cyc_Absyn_Exp*_T35;struct _fat_ptr _T36;struct Cyc_Absyn_Exp*_T37;void*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;struct Cyc_Absyn_Exp*_T3C;struct Cyc_Absyn_Exp*_T3D;struct Cyc_List_List*(*_T3E)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);struct Cyc_List_List*(*_T3F)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T40;void(*_T41)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*);void(*_T42)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Toc_Env*_T43;struct Cyc_List_List*_T44;enum Cyc_Absyn_Primop _T45;int _T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_Absyn_Exp*_T49;void*_T4A;struct Cyc_Absyn_Tqual _T4B;void*_T4C;struct Cyc_Absyn_Exp*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_Absyn_Exp*_T4F;struct Cyc_List_List*_T50;void*_T51;struct Cyc_List_List*_T52;void*_T53;int*_T54;int _T55;struct Cyc_Absyn_PtrInfo _T56;struct Cyc_Absyn_PtrInfo _T57;struct Cyc_Absyn_PtrAtts _T58;struct Cyc_Absyn_PtrInfo _T59;struct Cyc_Absyn_PtrAtts _T5A;struct Cyc_Absyn_PtrInfo _T5B;struct Cyc_Absyn_PtrAtts _T5C;void*_T5D;void*_T5E;struct Cyc_Absyn_Exp*_T5F;void*_T60;struct Cyc_Absyn_Exp**_T61;struct Cyc_List_List*_T62;void*_T63;struct Cyc_Absyn_Exp**_T64;void*_T65;struct Cyc_Absyn_Exp*_T66;void*_T67;struct Cyc_Absyn_Exp*_T68;struct _fat_ptr _T69;int _T6A;struct Cyc_List_List*_T6B;void*_T6C;struct Cyc_Absyn_Exp*_T6D;int _T6E;struct Cyc_Absyn_Exp*_T6F;void*_T70;struct Cyc_Absyn_Exp**_T71;struct Cyc_List_List*_T72;void*_T73;struct Cyc_Absyn_Exp**_T74;struct Cyc_Absyn_Exp*_T75;struct _fat_ptr _T76;struct Cyc_Absyn_Exp*_T77;void*_T78;struct Cyc_Absyn_Exp**_T79;struct Cyc_List_List*_T7A;void*_T7B;void*_T7C;void*_T7D;struct Cyc_Absyn_Exp*_T7E;void*_T7F;struct Cyc_Absyn_Exp**_T80;struct Cyc_List_List*_T81;void*_T82;struct Cyc_Absyn_Exp**_T83;struct Cyc_Absyn_Exp*_T84;struct _fat_ptr _T85;int _T86;int _T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;void**_T89;struct Cyc_Absyn_Exp*_T8A;unsigned _T8B;struct _fat_ptr _T8C;struct Cyc_Absyn_Exp*_T8D;struct Cyc_Absyn_Exp*_T8E;struct Cyc_Absyn_Exp*_T8F;struct Cyc_Absyn_Exp*_T90;void*_T91;struct Cyc_Absyn_Exp*_T92;struct Cyc_Absyn_Exp*_T93;struct Cyc_Absyn_Exp*_T94;struct Cyc_Warn_String_Warn_Warg_struct _T95;struct Cyc_Warn_Typ_Warn_Warg_struct _T96;struct Cyc_Absyn_Exp*_T97;void*_T98;void**_T99;void**_T9A;int(*_T9B)(struct _fat_ptr ap);struct _fat_ptr _T9C;int _T9D;int(*_T9E)(struct _fat_ptr);struct _fat_ptr _T9F;struct Cyc_List_List*_TA0;void*_TA1;int*_TA2;int _TA3;struct Cyc_Absyn_PtrInfo _TA4;struct Cyc_Absyn_PtrInfo _TA5;struct Cyc_Absyn_PtrAtts _TA6;struct Cyc_Absyn_PtrInfo _TA7;struct Cyc_Absyn_PtrAtts _TA8;void*_TA9;void*_TAA;struct Cyc_List_List*_TAB;void*_TAC;struct Cyc_List_List*_TAD;struct Cyc_List_List*_TAE;struct Cyc_List_List*_TAF;void*_TB0;struct Cyc_Absyn_Exp*_TB1;void*_TB2;struct Cyc_Absyn_Exp**_TB3;struct Cyc_Absyn_Exp**_TB4;void*_TB5;struct Cyc_Absyn_Exp*_TB6;void*_TB7;struct Cyc_Absyn_Exp**_TB8;struct Cyc_Absyn_Exp*_TB9;struct _fat_ptr _TBA;int _TBB;int _TBC;struct Cyc_Absyn_Exp*_TBD;void*_TBE;struct Cyc_Absyn_Exp**_TBF;struct Cyc_Absyn_Exp**_TC0;struct Cyc_Absyn_Exp**_TC1;struct Cyc_Absyn_Exp*_TC2;struct _fat_ptr _TC3;struct Cyc_Absyn_Exp*_TC4;void*_TC5;struct Cyc_Absyn_Exp**_TC6;struct Cyc_List_List*_TC7;void*_TC8;void*_TC9;void*_TCA;struct Cyc_Absyn_Exp*_TCB;void*_TCC;struct Cyc_Absyn_Exp**_TCD;struct Cyc_Absyn_Exp**_TCE;struct Cyc_Absyn_Exp*_TCF;struct _fat_ptr _TD0;struct Cyc_List_List*_TD1;void*_TD2;void**_TD3;int _TD4;struct Cyc_List_List*_TD5;void*_TD6;struct Cyc_List_List*_TD7;struct Cyc_List_List*_TD8;struct Cyc_List_List*_TD9;void*_TDA;struct Cyc_List_List*_TDB;struct Cyc_List_List*_TDC;struct Cyc_List_List*_TDD;void*_TDE;int _TDF;struct Cyc_Absyn_Exp*_TE0;struct Cyc_Absyn_Exp*_TE1;struct _fat_ptr*_TE2;struct Cyc_Absyn_Exp*_TE3;struct Cyc_Absyn_Exp*_TE4;struct _fat_ptr*_TE5;struct Cyc_Absyn_Exp*_TE6;struct Cyc_Absyn_Exp*_TE7;struct Cyc_Absyn_Exp*_TE8;struct Cyc_Absyn_Exp*_TE9;void*_TEA;struct Cyc_Absyn_Exp*_TEB;void*_TEC;struct Cyc_Absyn_Exp*_TED;struct Cyc_Absyn_Exp*_TEE;struct Cyc_Absyn_Exp*_TEF;void*_TF0;struct Cyc_Absyn_Exp**_TF1;struct Cyc_Absyn_Exp**_TF2;void*_TF3;struct Cyc_Absyn_Exp*_TF4;void*_TF5;struct Cyc_Absyn_Exp**_TF6;struct Cyc_Absyn_Exp*_TF7;void*_TF8;struct Cyc_Absyn_Exp*_TF9;struct _fat_ptr _TFA;struct Cyc_List_List*_TFB;void*_TFC;struct Cyc_List_List*_TFD;struct Cyc_List_List*_TFE;struct Cyc_List_List*_TFF;void*_T100;struct Cyc_List_List*_T101;struct Cyc_List_List*_T102;struct Cyc_List_List*_T103;struct Cyc_List_List*_T104;void*_T105;void**_T106;int _T107;void*_T108;struct Cyc_Absyn_Tqual _T109;struct Cyc_Absyn_Exp*_T10A;void*_T10B;struct Cyc_Absyn_Exp*_T10C;struct Cyc_Absyn_Exp*_T10D;struct Cyc_Absyn_Exp*_T10E;int _T10F;void*_T110;struct Cyc_Absyn_Tqual _T111;struct Cyc_Absyn_Exp*_T112;void*_T113;struct Cyc_Absyn_Exp*_T114;struct Cyc_Absyn_Exp*_T115;struct Cyc_Absyn_Exp*_T116;struct Cyc_Absyn_Exp*_T117;void*_T118;enum Cyc_Absyn_Incrementor _T119;int _T11A;enum Cyc_Absyn_Incrementor _T11B;int _T11C;struct Cyc_Absyn_Exp*_T11D;void**_T11E;int*_T11F;void**_T120;int _T121;struct Cyc_Warn_String_Warn_Warg_struct _T122;struct Cyc_Warn_String_Warn_Warg_struct _T123;struct Cyc_Warn_String_Warn_Warg_struct _T124;struct Cyc_Warn_String_Warn_Warg_struct _T125;void**_T126;void**_T127;void**_T128;void**_T129;int(*_T12A)(unsigned,struct _fat_ptr);void*(*_T12B)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Exp*_T12C;unsigned _T12D;struct _fat_ptr _T12E;struct Cyc_Absyn_Exp*_T12F;int*_T130;void**_T131;int _T132;struct Cyc_Absyn_Exp*_T133;enum Cyc_Absyn_Incrementor _T134;int _T135;struct Cyc_Warn_String_Warn_Warg_struct _T136;struct Cyc_Warn_String_Warn_Warg_struct _T137;struct Cyc_Warn_String_Warn_Warg_struct _T138;void**_T139;void**_T13A;void**_T13B;int(*_T13C)(unsigned,struct _fat_ptr);void*(*_T13D)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Exp*_T13E;unsigned _T13F;struct _fat_ptr _T140;struct Cyc_Absyn_Exp*_T141;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T142;struct Cyc_Core_Opt*_T143;enum Cyc_Absyn_Primop _T144;void*_T145;void**_T146;int _T147;struct Cyc_Absyn_Exp*_T148;enum Cyc_Absyn_Incrementor _T149;int _T14A;enum Cyc_Absyn_Incrementor _T14B;int _T14C;enum Cyc_Absyn_Incrementor _T14D;int _T14E;enum Cyc_Absyn_Incrementor _T14F;int _T150;struct Cyc_Absyn_Exp*_T151;void*_T152;struct Cyc_Absyn_Exp**_T153;struct Cyc_Absyn_Exp**_T154;void*_T155;struct Cyc_Absyn_Exp*_T156;void*_T157;struct Cyc_Absyn_Exp**_T158;struct Cyc_Absyn_Exp*_T159;void*_T15A;struct Cyc_Absyn_Exp*_T15B;struct _fat_ptr _T15C;void*_T15D;void**_T15E;int _T15F;enum Cyc_Absyn_Incrementor _T160;int _T161;struct Cyc_Absyn_Exp*_T162;void*_T163;struct Cyc_Absyn_Exp*_T164;void*_T165;struct Cyc_Absyn_Exp*_T166;void*_T167;struct Cyc_Absyn_Exp*_T168;struct Cyc_Absyn_Exp*_T169;void*_T16A;struct Cyc_Absyn_Exp*_T16B;int _T16C;struct Cyc_Absyn_Exp*_T16D;struct Cyc_Absyn_Exp*_T16E;void*_T16F;struct Cyc_Absyn_Exp*_T170;struct Cyc_Absyn_Exp*_T171;struct Cyc_Absyn_Exp*_T172;struct Cyc_Absyn_Exp*_T173;struct Cyc_Absyn_Exp*_T174;struct Cyc_Absyn_Exp*_T175;struct Cyc_Absyn_Stmt*_T176;struct Cyc_Absyn_Exp*_T177;struct _fat_ptr _T178;struct Cyc_Absyn_Exp*_T179;void*_T17A;struct Cyc_Absyn_Exp*_T17B;struct Cyc_Absyn_Stmt*_T17C;struct Cyc_Absyn_Stmt*_T17D;struct Cyc_Absyn_Exp*_T17E;struct Cyc_Absyn_Stmt*_T17F;struct Cyc_Absyn_Exp*_T180;struct Cyc_Absyn_Stmt*_T181;struct Cyc_Absyn_Exp*_T182;int _T183;struct Cyc_Absyn_Exp*_T184;void*_T185;struct Cyc_Absyn_Exp**_T186;struct Cyc_Absyn_Exp**_T187;struct Cyc_Absyn_Exp*_T188;void*_T189;struct Cyc_Absyn_Exp*_T18A;struct _fat_ptr _T18B;struct Cyc_Absyn_Exp*_T18C;void*_T18D;struct Cyc_Absyn_Exp*_T18E;struct Cyc_Absyn_Exp**_T18F;void*_T190;void*_T191;struct Cyc_Absyn_Exp*_T192;void*_T193;struct Cyc_Absyn_Exp**_T194;struct Cyc_Absyn_Exp**_T195;struct Cyc_Absyn_Exp*_T196;void*_T197;struct Cyc_Absyn_Exp*_T198;struct _fat_ptr _T199;int _T19A;void(*_T19B)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor);struct Cyc_Absyn_Exp*_T19C;enum Cyc_Absyn_Incrementor _T19D;struct Cyc_Absyn_Exp*_T19E;struct Cyc_Absyn_Exp*_T19F;struct Cyc_Absyn_Exp*_T1A0;void*_T1A1;struct Cyc_Absyn_Exp*_T1A2;void*_T1A3;struct Cyc_Absyn_Exp*_T1A4;int*_T1A5;void**_T1A6;int _T1A7;struct Cyc_Absyn_Exp*_T1A8;struct Cyc_Absyn_Exp*_T1A9;void**_T1AA;int*_T1AB;void**_T1AC;int _T1AD;void*_T1AE;void**_T1AF;int _T1B0;struct Cyc_Core_Opt*_T1B1;void*_T1B2;int _T1B3;struct Cyc_Absyn_Exp*_T1B4;void*_T1B5;struct Cyc_Warn_String_Warn_Warg_struct _T1B6;void**_T1B7;int(*_T1B8)(struct _fat_ptr ap);struct _fat_ptr _T1B9;struct Cyc_Absyn_Exp*_T1BA;void*_T1BB;struct Cyc_Absyn_Exp**_T1BC;struct Cyc_Absyn_Exp**_T1BD;void*_T1BE;struct Cyc_Absyn_Exp*_T1BF;void*_T1C0;struct Cyc_Absyn_Exp**_T1C1;struct Cyc_Absyn_Exp*_T1C2;struct _fat_ptr _T1C3;void*_T1C4;void**_T1C5;int _T1C6;struct Cyc_Core_Opt*_T1C7;void*_T1C8;int _T1C9;struct Cyc_Absyn_Exp*_T1CA;void*_T1CB;struct Cyc_Absyn_Exp**_T1CC;struct Cyc_Absyn_Exp**_T1CD;struct Cyc_Absyn_Exp*_T1CE;struct _fat_ptr _T1CF;struct Cyc_Absyn_Exp*_T1D0;void*_T1D1;struct Cyc_Absyn_Exp*_T1D2;struct Cyc_Absyn_Exp**_T1D3;void*_T1D4;void*_T1D5;struct Cyc_Absyn_Exp*_T1D6;void*_T1D7;struct Cyc_Absyn_Exp**_T1D8;struct Cyc_Absyn_Exp**_T1D9;struct Cyc_Absyn_Exp*_T1DA;struct _fat_ptr _T1DB;struct Cyc_Warn_String_Warn_Warg_struct _T1DC;void**_T1DD;int(*_T1DE)(struct _fat_ptr ap);struct _fat_ptr _T1DF;int _T1E0;int _T1E1;struct Cyc_Absyn_Exp*_T1E2;void*_T1E3;struct Cyc_Absyn_Exp*_T1E4;struct Cyc_Absyn_Exp*_T1E5;int _T1E6;void(*_T1E7)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple34*),struct _tuple34*);struct Cyc_Absyn_Exp*_T1E8;struct _tuple34*_T1E9;struct Cyc_Absyn_Exp*_T1EA;struct Cyc_Absyn_Exp*_T1EB;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T1EC;struct Cyc_Absyn_VarargCallInfo*_T1ED;struct Cyc_Absyn_Exp*_T1EE;void*_T1EF;int _T1F0;struct Cyc_Absyn_Exp*_T1F1;struct Cyc_Absyn_Exp*_T1F2;struct Cyc_Absyn_Exp**_T1F3;struct Cyc_Absyn_Exp*_T1F4;struct _fat_ptr _T1F5;void(*_T1F6)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*);void(*_T1F7)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Toc_Env*_T1F8;struct Cyc_List_List*_T1F9;struct Cyc_Absyn_VarargCallInfo*_T1FA;struct Cyc_Absyn_Exp*_T1FB;void*_T1FC;struct Cyc_Absyn_VarargInfo*_T1FD;void*_T1FE;struct Cyc_Toc_Env*_T1FF;struct Cyc_List_List*_T200;void*_T201;struct Cyc_Absyn_Exp*_T202;struct Cyc_List_List*_T203;struct Cyc_List_List*_T204;void*_T205;struct Cyc_List_List*_T206;struct Cyc_List_List*_T207;struct Cyc_Absyn_Exp*_T208;struct Cyc_Absyn_Exp**_T209;struct Cyc_Absyn_Exp*_T20A;void*_T20B;struct Cyc_Absyn_Exp**_T20C;struct Cyc_Absyn_Exp*_T20D;void*_T20E;struct Cyc_Absyn_Exp*_T20F;void*_T210;struct Cyc_Absyn_Exp**_T211;struct Cyc_Absyn_Exp*_T212;struct _fat_ptr _T213;void*_T214;struct Cyc_Absyn_Exp*_T215;void*_T216;int _T217;struct Cyc_Absyn_Exp*_T218;struct Cyc_Absyn_Exp*_T219;struct Cyc_Absyn_Exp**_T21A;struct Cyc_Absyn_Exp*_T21B;struct _fat_ptr _T21C;struct Cyc_Absyn_Exp*_T21D;struct Cyc_Absyn_Exp*_T21E;struct Cyc_Absyn_Exp*_T21F;struct Cyc_Absyn_Exp*_T220;void*_T221;void*_T222;void*_T223;struct Cyc_Absyn_Exp*_T224;struct Cyc_Absyn_VarargInfo*_T225;int _T226;struct Cyc_Absyn_VarargInfo*_T227;void*_T228;void*_T229;int*_T22A;int _T22B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22C;void*_T22D;int*_T22E;int _T22F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T230;void*_T231;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T232;union Cyc_Absyn_DatatypeInfo _T233;struct _union_DatatypeInfo_KnownDatatype _T234;unsigned _T235;void*_T236;union Cyc_Absyn_DatatypeInfo _T237;struct _union_DatatypeInfo_KnownDatatype _T238;struct Cyc_Absyn_Datatypedecl**_T239;struct Cyc_Warn_String_Warn_Warg_struct _T23A;void**_T23B;int(*_T23C)(struct _fat_ptr ap);struct _fat_ptr _T23D;struct _fat_ptr _T23E;int _T23F;struct _tuple35*_T240;struct _RegionHandle*_T241;unsigned _T242;int _T243;struct _tuple35*_T244;unsigned _T245;struct _tuple35*_T246;unsigned _T247;int _T248;struct Cyc_List_List*_T249;void*_T24A;struct Cyc_Absyn_Exp*_T24B;void*_T24C;struct _fat_ptr _T24D;int _T24E;char*_T24F;struct _tuple35*_T250;struct _tuple35 _T251;struct Cyc_List_List*_T252;void*_T253;struct _tuple1*_T254;struct Cyc_Absyn_Datatypedecl*_T255;struct _tuple1*_T256;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T257;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T258;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T259;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25A;struct _tuple35*_T25B;unsigned char*_T25C;struct _tuple35*_T25D;int _T25E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25F;struct Cyc_List_List*_T260;struct Cyc_List_List*_T261;struct _fat_ptr _T262;int _T263;char*_T264;struct _tuple35*_T265;struct Cyc_Absyn_Exp*_T266;void*_T267;struct Cyc_Absyn_Exp*_T268;void*_T269;struct Cyc_List_List*_T26A;struct _tuple1*_T26B;void*_T26C;struct Cyc_Absyn_Exp*_T26D;void*_T26E;struct Cyc_Absyn_Exp*_T26F;struct Cyc_Absyn_Stmt*_T270;int _T271;struct Cyc_List_List*_T272;void*_T273;struct Cyc_Absyn_Exp*_T274;void*_T275;struct _fat_ptr _T276;int _T277;char*_T278;struct _tuple35*_T279;struct _tuple35 _T27A;struct _fat_ptr _T27B;unsigned char*_T27C;struct _tuple35*_T27D;int _T27E;struct _tuple35 _T27F;void*_T280;struct Cyc_List_List*_T281;void*_T282;int _T283;struct _tuple1*_T284;struct Cyc_Absyn_Datatypedecl*_T285;struct _tuple1*_T286;struct Cyc_List_List*_T287;struct _tuple20**_T288;struct _fat_ptr*_T289;struct Cyc_Absyn_Exp*_T28A;struct _tuple20**_T28B;struct _fat_ptr*_T28C;struct Cyc_Absyn_Exp*_T28D;struct _fat_ptr _T28E;struct _tuple1*_T28F;void*_T290;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T291;void*_T292;struct Cyc_Absyn_Exp*_T293;void*_T294;struct Cyc_Absyn_Exp*_T295;struct Cyc_Absyn_Stmt*_T296;struct Cyc_List_List*_T297;struct Cyc_List_List*_T298;struct Cyc_Toc_Env*_T299;struct Cyc_List_List*_T29A;void*_T29B;struct Cyc_Absyn_Exp*_T29C;struct Cyc_List_List*_T29D;struct Cyc_List_List*_T29E;void*_T29F;struct Cyc_List_List*_T2A0;struct Cyc_List_List*_T2A1;struct Cyc_Absyn_Exp*_T2A2;void*_T2A3;struct Cyc_Absyn_Exp*_T2A4;struct Cyc_Absyn_Exp*_T2A5;int _T2A6;struct Cyc_Absyn_Exp*_T2A7;void*_T2A8;struct Cyc_Absyn_Exp*_T2A9;struct Cyc_Absyn_Exp**_T2AA;struct Cyc_Absyn_Exp*_T2AB;struct _fat_ptr _T2AC;void*_T2AD;struct Cyc_Absyn_Exp*_T2AE;struct Cyc_Absyn_Exp*_T2AF;struct Cyc_Absyn_Exp*_T2B0;struct Cyc_Absyn_Exp*_T2B1;struct Cyc_Absyn_Exp*_T2B2;void*_T2B3;void*_T2B4;int _T2B5;int _T2B6;struct Cyc_Absyn_Exp*_T2B7;struct _tuple1*_T2B8;void*_T2B9;struct Cyc_Absyn_Exp*_T2BA;struct Cyc_Absyn_Exp*_T2BB;struct Cyc_Absyn_Exp*_T2BC;unsigned _T2BD;struct Cyc_Absyn_Stmt*_T2BE;struct Cyc_Absyn_Exp*_T2BF;unsigned _T2C0;struct Cyc_Absyn_Stmt*_T2C1;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T2C2;void**_T2C3;struct Cyc_Absyn_Exp*_T2C4;void*_T2C5;void**_T2C6;void**_T2C7;struct _tuple36 _T2C8;void*_T2C9;int*_T2CA;int _T2CB;void*_T2CC;int*_T2CD;unsigned _T2CE;void*_T2CF;void*_T2D0;void*_T2D1;struct Cyc_Absyn_PtrInfo _T2D2;struct Cyc_Absyn_PtrAtts _T2D3;void*_T2D4;void*_T2D5;struct Cyc_Absyn_PtrInfo _T2D6;struct Cyc_Absyn_PtrAtts _T2D7;void*_T2D8;struct Cyc_Absyn_PtrInfo _T2D9;struct Cyc_Absyn_PtrAtts _T2DA;void*_T2DB;struct Cyc_Absyn_PtrInfo _T2DC;struct Cyc_Absyn_PtrAtts _T2DD;void*_T2DE;struct _tuple37 _T2DF;struct Cyc_Absyn_Exp*_T2E0;struct Cyc_Absyn_Exp*_T2E1;int _T2E2;struct Cyc_Absyn_Exp*_T2E3;struct Cyc_Absyn_Exp*_T2E4;struct Cyc_Absyn_Exp**_T2E5;struct Cyc_Absyn_Exp*_T2E6;struct _fat_ptr _T2E7;int _T2E8;struct Cyc_Absyn_Exp*_T2E9;struct Cyc_Absyn_Exp*_T2EA;int _T2EB;int _T2EC;struct Cyc_Absyn_PtrInfo _T2ED;struct Cyc_Absyn_Tqual _T2EE;int _T2EF;int _T2F0;struct Cyc_Absyn_Exp*_T2F1;struct Cyc_Absyn_Exp*_T2F2;void*_T2F3;struct Cyc_Absyn_Exp*_T2F4;struct Cyc_Absyn_Exp*_T2F5;struct Cyc_Absyn_Exp*_T2F6;void*_T2F7;void*_T2F8;struct Cyc_Absyn_Exp*_T2F9;struct Cyc_Absyn_Exp*_T2FA;int _T2FB;struct Cyc_Absyn_Exp*_T2FC;void*_T2FD;struct Cyc_Absyn_Exp*_T2FE;int*_T2FF;unsigned _T300;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T301;union Cyc_Absyn_Cnst _T302;struct _union_Cnst_Wstring_c _T303;unsigned _T304;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T305;void*_T306;int*_T307;unsigned _T308;void*_T309;void*_T30A;struct Cyc_Absyn_Vardecl*_T30B;void*_T30C;int _T30D;void*_T30E;struct Cyc_Absyn_Exp*_T30F;int _T310;int _T311;struct Cyc_Absyn_Exp*_T312;struct Cyc_Absyn_Exp**_T313;struct Cyc_Absyn_Exp**_T314;struct Cyc_Absyn_Exp*_T315;struct _fat_ptr _T316;struct Cyc_Absyn_Exp*_T317;struct Cyc_Absyn_Exp**_T318;void*_T319;void*_T31A;struct Cyc_Absyn_Exp*_T31B;void*_T31C;struct Cyc_Absyn_Exp*_T31D;void*_T31E;struct Cyc_Absyn_Exp**_T31F;struct Cyc_Absyn_Exp**_T320;struct Cyc_Absyn_Exp*_T321;struct _fat_ptr _T322;struct Cyc_Absyn_Exp*_T323;int _T324;struct Cyc_Absyn_PtrInfo _T325;struct Cyc_Absyn_Tqual _T326;int _T327;struct Cyc_Absyn_Exp*_T328;struct Cyc_Absyn_Exp*_T329;void*_T32A;struct Cyc_Absyn_Exp*_T32B;struct Cyc_Absyn_Exp*_T32C;struct Cyc_Absyn_Exp*_T32D;void*_T32E;void*_T32F;struct Cyc_Absyn_PtrInfo _T330;void*_T331;void*_T332;struct Cyc_Absyn_Exp*_T333;void*_T334;struct Cyc_Absyn_Exp*_T335;void*_T336;int _T337;struct Cyc_Absyn_Exp*_T338;struct Cyc_Absyn_Exp**_T339;struct Cyc_Absyn_Exp**_T33A;struct Cyc_Absyn_Exp**_T33B;struct Cyc_Absyn_Exp*_T33C;struct _fat_ptr _T33D;struct Cyc_Absyn_Exp*_T33E;struct _tuple1*_T33F;void*_T340;struct Cyc_Absyn_Exp*_T341;struct Cyc_Absyn_Stmt*_T342;struct Cyc_Absyn_Exp*_T343;struct Cyc_Absyn_Exp*_T344;void*_T345;struct Cyc_Absyn_Exp**_T346;struct Cyc_Absyn_Exp**_T347;struct Cyc_Absyn_Exp**_T348;struct Cyc_Absyn_Exp*_T349;struct _fat_ptr _T34A;struct Cyc_Absyn_Exp*_T34B;void*_T34C;struct Cyc_Absyn_Exp**_T34D;struct Cyc_Absyn_Exp**_T34E;struct Cyc_Absyn_PtrInfo _T34F;void*_T350;void*_T351;struct Cyc_Absyn_Exp*_T352;void*_T353;struct Cyc_Absyn_Exp*_T354;void*_T355;struct Cyc_Absyn_Exp**_T356;struct Cyc_Absyn_Exp*_T357;struct _fat_ptr _T358;struct Cyc_Absyn_Exp*_T359;int _T35A;int _T35B;struct Cyc_Absyn_Exp*_T35C;struct Cyc_Absyn_Exp*_T35D;void*_T35E;struct Cyc_Absyn_Exp*_T35F;struct Cyc_Absyn_Exp*_T360;void*_T361;struct Cyc_Absyn_PtrInfo _T362;struct Cyc_Absyn_PtrAtts _T363;void*_T364;int _T365;struct Cyc_Absyn_PtrInfo _T366;struct Cyc_Absyn_PtrAtts _T367;void*_T368;int _T369;struct Cyc_Absyn_Exp*_T36A;struct Cyc_Absyn_Exp**_T36B;struct Cyc_Absyn_Exp*_T36C;struct _fat_ptr _T36D;void*_T36E;struct Cyc_Absyn_Exp*_T36F;struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*_T370;char*_T371;char*_T372;struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*_T373;char*_T374;char*_T375;struct Cyc_Absyn_Exp*_T376;struct Cyc_Absyn_Exp**_T377;struct Cyc_Absyn_Exp**_T378;struct Cyc_Absyn_PtrInfo _T379;void*_T37A;void*_T37B;struct Cyc_Absyn_Exp*_T37C;void*_T37D;struct Cyc_Absyn_Exp*_T37E;void*_T37F;struct Cyc_Absyn_Exp**_T380;struct Cyc_Absyn_Exp*_T381;struct _fat_ptr _T382;void*_T383;struct Cyc_Absyn_Exp*_T384;struct Cyc_Absyn_Exp**_T385;struct Cyc_Absyn_Exp**_T386;struct Cyc_Absyn_PtrInfo _T387;void*_T388;void*_T389;struct Cyc_Absyn_Exp*_T38A;void*_T38B;struct Cyc_Absyn_Exp*_T38C;void*_T38D;struct Cyc_Absyn_Exp**_T38E;struct Cyc_Absyn_Exp*_T38F;struct _fat_ptr _T390;void*_T391;int _T392;struct Cyc_Absyn_Exp*_T393;void*_T394;struct Cyc_Absyn_Exp**_T395;struct Cyc_Absyn_Exp*_T396;struct _fat_ptr _T397;struct Cyc_Absyn_Exp*_T398;void**_T399;void*_T39A;struct Cyc_Absyn_Exp*_T39B;int _T39C;int _T39D;struct Cyc_Absyn_PtrInfo _T39E;struct Cyc_Absyn_Tqual _T39F;int _T3A0;int _T3A1;int(*_T3A2)(struct _fat_ptr);struct _fat_ptr _T3A3;struct Cyc_Absyn_Exp*_T3A4;void*_T3A5;struct Cyc_Absyn_Exp**_T3A6;struct Cyc_Absyn_Exp**_T3A7;struct Cyc_Absyn_PtrInfo _T3A8;void*_T3A9;void*_T3AA;struct Cyc_Absyn_Exp*_T3AB;void*_T3AC;struct Cyc_Absyn_Exp*_T3AD;void*_T3AE;struct Cyc_Absyn_Exp**_T3AF;struct Cyc_Absyn_Exp*_T3B0;void*_T3B1;struct Cyc_Absyn_Exp*_T3B2;struct _fat_ptr _T3B3;void*_T3B4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3B5;void*_T3B6;int*_T3B7;int _T3B8;void*_T3B9;void*_T3BA;struct Cyc_Absyn_PtrInfo _T3BB;struct Cyc_Absyn_PtrAtts _T3BC;void*_T3BD;struct Cyc_Absyn_Exp*_T3BE;struct Cyc_Absyn_Exp*_T3BF;struct _fat_ptr*_T3C0;struct Cyc_Absyn_Exp*_T3C1;int _T3C2;struct Cyc_Absyn_Exp*_T3C3;struct Cyc_Absyn_Exp*_T3C4;int _T3C5;void(*_T3C6)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int);struct Cyc_Absyn_Exp*_T3C7;struct Cyc_Absyn_Exp*_T3C8;void*_T3C9;struct Cyc_Absyn_Exp*_T3CA;struct Cyc_Absyn_Kind*_T3CB;struct Cyc_Absyn_Kind*_T3CC;struct Cyc_Absyn_Exp*_T3CD;void*_T3CE;void*_T3CF;struct Cyc_Absyn_Kind*_T3D0;int _T3D1;struct Cyc_Absyn_Exp*_T3D2;void*_T3D3;struct Cyc_Absyn_Exp*_T3D4;struct Cyc_Absyn_Exp*_T3D5;void*_T3D6;void*_T3D7;struct Cyc_Absyn_Tqual _T3D8;void*_T3D9;struct Cyc_Absyn_Exp*_T3DA;struct Cyc_Absyn_Exp*_T3DB;void*_T3DC;void*_T3DD;int*_T3DE;int _T3DF;struct Cyc_Absyn_ArrayInfo _T3E0;struct _tuple1*_T3E1;struct Cyc_Absyn_Exp*_T3E2;unsigned _T3E3;struct Cyc_Absyn_Exp*_T3E4;struct Cyc_Absyn_Exp*_T3E5;unsigned _T3E6;struct Cyc_Absyn_Exp*_T3E7;void*_T3E8;struct Cyc_Absyn_Exp*_T3E9;unsigned _T3EA;struct Cyc_Absyn_Exp*_T3EB;struct Cyc_Absyn_Exp**_T3EC;struct Cyc_Absyn_Exp**_T3ED;struct Cyc_Absyn_Exp*_T3EE;void*_T3EF;struct Cyc_Absyn_Exp*_T3F0;struct _fat_ptr _T3F1;struct Cyc_Absyn_Exp*_T3F2;void*_T3F3;struct Cyc_Absyn_Exp*_T3F4;struct Cyc_Absyn_Exp*_T3F5;void*_T3F6;int _T3F7;struct Cyc_Absyn_Exp*_T3F8;struct Cyc_Absyn_Exp*_T3F9;struct Cyc_Absyn_Exp*_T3FA;struct Cyc_Absyn_Exp*_T3FB;void*_T3FC;struct Cyc_Absyn_Exp*_T3FD;struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T3FE;int _T3FF;struct Cyc_Absyn_Exp*_T400;void*_T401;struct Cyc_Absyn_Exp*_T402;struct Cyc_Absyn_Exp*_T403;struct Cyc_Absyn_Exp**_T404;struct Cyc_Absyn_Exp**_T405;struct Cyc_Absyn_Exp*_T406;void*_T407;struct Cyc_Absyn_Exp*_T408;unsigned _T409;struct Cyc_Absyn_Exp**_T40A;struct Cyc_Absyn_Exp**_T40B;struct Cyc_Absyn_Exp*_T40C;void*_T40D;struct Cyc_Absyn_Exp*_T40E;struct _fat_ptr _T40F;struct Cyc_Absyn_Exp*_T410;void*_T411;struct Cyc_Absyn_Exp*_T412;struct Cyc_Absyn_Exp*_T413;unsigned _T414;struct _tuple1*_T415;void*_T416;struct Cyc_Absyn_Exp*_T417;struct Cyc_Absyn_Exp*_T418;struct Cyc_Absyn_Stmt*_T419;struct Cyc_Absyn_Exp*_T41A;void*_T41B;struct Cyc_Absyn_Exp*_T41C;struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T41D;void*_T41E;struct Cyc_Absyn_Exp*_T41F;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T420;struct Cyc_Absyn_Exp*_T421;void*_T422;void*_T423;int _T424;int _T425;int _T426;struct Cyc_Absyn_Exp*_T427;void*_T428;struct Cyc_Absyn_Exp*_T429;void*_T42A;void*_T42B;struct _fat_ptr*_T42C;int _T42D;int _T42E;struct Cyc_Absyn_Exp*_T42F;int _T430;struct Cyc_Absyn_Exp*_T431;struct Cyc_Absyn_Exp*_T432;struct Cyc_Absyn_Exp*_T433;struct Cyc_Absyn_Exp*_T434;void*_T435;void*_T436;int _T437;int*_T438;int _T439;struct Cyc_Absyn_PtrInfo _T43A;struct Cyc_Warn_String_Warn_Warg_struct _T43B;void**_T43C;int(*_T43D)(struct _fat_ptr ap);struct _fat_ptr _T43E;struct Cyc_Toc_Env*_T43F;struct Cyc_Absyn_Exp*_T440;struct Cyc_Absyn_Exp*_T441;void*_T442;int _T443;int _T444;struct Cyc_Absyn_Exp*_T445;void*_T446;struct Cyc_Absyn_Exp*_T447;void*_T448;void*_T449;struct _fat_ptr*_T44A;int _T44B;struct Cyc_Absyn_Exp*(*_T44C)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);int _T44D;struct Cyc_Absyn_Exp*_T44E;int _T44F;int _T450;struct Cyc_Absyn_Exp*_T451;struct Cyc_Absyn_Exp*_T452;struct Cyc_Absyn_Exp*_T453;int _T454;struct Cyc_Absyn_Exp*_T455;struct Cyc_Toc_Env*_T456;struct Cyc_Absyn_Exp*_T457;struct Cyc_Absyn_Exp*_T458;void*_T459;struct Cyc_Toc_Env*_T45A;struct Cyc_Absyn_Exp*_T45B;struct Cyc_Absyn_Exp*_T45C;void*_T45D;struct Cyc_Absyn_Exp*_T45E;enum Cyc_Toc_PTR_ANS _T45F;int _T460;struct Cyc_Absyn_Exp*_T461;enum Cyc_Toc_PTR_ANS _T462;int _T463;struct Cyc_Absyn_Exp*_T464;struct Cyc_Absyn_Exp*_T465;struct Cyc_Toc_Env*_T466;struct Cyc_List_List*_T467;void*_T468;struct _tuple20*_T469;struct _tuple20 _T46A;struct Cyc_Absyn_Exp*_T46B;struct Cyc_List_List*_T46C;int _T46D;struct Cyc_Absyn_Exp*_T46E;int _T46F;struct Cyc_Absyn_Exp*_T470;void*_T471;void*_T472;int _T473;int _T474;struct Cyc_Warn_String_Warn_Warg_struct _T475;void**_T476;struct Cyc_Absyn_Exp*_T477;unsigned _T478;struct _fat_ptr _T479;struct Cyc_List_List*_T47A;struct _tuple20*_T47B;int _T47C;void*_T47D;struct Cyc_Absyn_Exp*_T47E;void*_T47F;struct Cyc_Absyn_Exp*_T480;struct Cyc_List_List*_T481;struct Cyc_List_List*_T482;struct _tuple20*_T483;struct Cyc_Absyn_Exp*_T484;void*_T485;int _T486;struct Cyc_Absyn_Exp*_T487;void*_T488;struct Cyc_Toc_Env*_T489;struct Cyc_List_List*_T48A;void*_T48B;struct _tuple20*_T48C;struct _tuple20 _T48D;struct Cyc_Absyn_Exp*_T48E;struct Cyc_List_List*_T48F;struct Cyc_Absyn_Exp*_T490;int _T491;int*_T492;int _T493;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T494;void*_T495;int*_T496;int _T497;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T498;struct Cyc_List_List*_T499;void*_T49A;struct Cyc_Absyn_Exp*_T49B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T49C;struct Cyc_Absyn_Aggrdecl*_T49D;struct Cyc_Absyn_Exp*_T49E;struct Cyc_Warn_String_Warn_Warg_struct _T49F;void**_T4A0;int(*_T4A1)(struct _fat_ptr ap);struct _fat_ptr _T4A2;struct Cyc_Absyn_Exp*_T4A3;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T4A4;struct _tuple1**_T4A5;struct Cyc_Absyn_Aggrdecl*_T4A6;struct Cyc_Absyn_AggrdeclImpl*_T4A7;struct Cyc_Warn_String_Warn_Warg_struct _T4A8;void**_T4A9;int(*_T4AA)(struct _fat_ptr ap);struct _fat_ptr _T4AB;int*_T4AC;int _T4AD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4AE;void*_T4AF;int*_T4B0;int _T4B1;void*_T4B2;struct _tuple1**_T4B3;struct _tuple12 _T4B4;struct Cyc_Warn_String_Warn_Warg_struct _T4B5;void**_T4B6;int(*_T4B7)(struct _fat_ptr ap);struct _fat_ptr _T4B8;struct Cyc_Absyn_Aggrdecl*_T4B9;struct Cyc_Absyn_AggrdeclImpl*_T4BA;struct Cyc_Absyn_AggrdeclImpl*_T4BB;struct Cyc_List_List*_T4BC;struct Cyc_List_List*_T4BD;struct Cyc_List_List*_T4BE;struct Cyc_List_List*_T4BF;void*_T4C0;struct _RegionHandle*_T4C1;struct Cyc_Absyn_Exp*_T4C2;unsigned _T4C3;struct Cyc_List_List*_T4C4;struct Cyc_Absyn_Aggrdecl*_T4C5;enum Cyc_Absyn_AggrKind _T4C6;struct Cyc_Absyn_Aggrdecl*_T4C7;struct Cyc_Absyn_AggrdeclImpl*_T4C8;struct Cyc_List_List*_T4C9;int _T4CA;int*_T4CB;int _T4CC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4CD;void*_T4CE;int*_T4CF;int _T4D0;struct Cyc_List_List*_T4D1;void*_T4D2;struct Cyc_Absyn_Exp*_T4D3;struct Cyc_Absyn_Aggrfield*_T4D4;void*_T4D5;int _T4D6;struct Cyc_Absyn_Exp*_T4D7;void*_T4D8;void*_T4D9;int _T4DA;struct Cyc_Absyn_Aggrfield*_T4DB;void*_T4DC;struct Cyc_Absyn_Exp*_T4DD;void*_T4DE;struct Cyc_Absyn_Exp*_T4DF;struct Cyc_Absyn_Exp*_T4E0;struct Cyc_Absyn_Exp*_T4E1;struct Cyc_Absyn_Aggrdecl*_T4E2;struct Cyc_Absyn_AggrdeclImpl*_T4E3;struct Cyc_Absyn_AggrdeclImpl*_T4E4;int _T4E5;struct Cyc_Absyn_Aggrfield*_T4E6;int _T4E7;unsigned _T4E8;struct Cyc_Absyn_Exp*_T4E9;void*_T4EA;struct _tuple1*_T4EB;struct _tuple1*_T4EC;struct _fat_ptr*_T4ED;struct _fat_ptr _T4EE;struct Cyc_String_pa_PrintArg_struct _T4EF;struct Cyc_Absyn_Aggrdecl*_T4F0;struct _tuple1*_T4F1;struct _tuple1 _T4F2;struct _fat_ptr*_T4F3;struct Cyc_String_pa_PrintArg_struct _T4F4;struct _fat_ptr*_T4F5;void**_T4F6;void**_T4F7;struct _fat_ptr _T4F8;struct _fat_ptr _T4F9;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T4FA;struct Cyc_List_List*_T4FB;struct _tuple20**_T4FC;struct _tuple20**_T4FD;struct _fat_ptr _T4FE;void*_T4FF;struct Cyc_Absyn_Exp*_T500;void*_T501;struct Cyc_Absyn_Exp*_T502;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T503;struct _tuple1**_T504;struct Cyc_List_List*_T505;struct _tuple20**_T506;struct _fat_ptr _T507;int _T508;struct Cyc_Absyn_Aggrdecl*_T509;struct Cyc_Absyn_AggrdeclImpl*_T50A;struct Cyc_Absyn_AggrdeclImpl*_T50B;struct Cyc_List_List*_T50C;struct Cyc_List_List*_T50D;struct Cyc_List_List*_T50E;struct Cyc_Absyn_Aggrdecl*_T50F;struct Cyc_List_List*_T510;struct Cyc_List_List*_T511;struct Cyc_List_List*_T512;struct Cyc_Absyn_Aggrdecl*_T513;struct Cyc_Absyn_AggrdeclImpl*_T514;struct Cyc_Absyn_AggrdeclImpl*_T515;struct Cyc_List_List*_T516;void*_T517;struct Cyc_List_List*_T518;struct Cyc_Absyn_Aggrfield*_T519;void*_T51A;struct Cyc_Absyn_Aggrfield*_T51B;void*_T51C;void*_T51D;struct Cyc_List_List*_T51E;struct Cyc_List_List*_T51F;struct Cyc_List_List*_T520;struct Cyc_Absyn_Aggrfield*_T521;void*_T522;int*_T523;int _T524;struct Cyc_Absyn_ArrayInfo _T525;struct Cyc_Absyn_Exp*_T526;struct Cyc_Absyn_Exp*_T527;int _T528;struct Cyc_Absyn_Exp*_T529;void*_T52A;struct Cyc_Absyn_Aggrfield*_T52B;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T52C;struct Cyc_Absyn_Exp*_T52D;void*_T52E;struct Cyc_Absyn_Exp*_T52F;void*_T530;int*_T531;int _T532;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T533;void*_T534;int*_T535;int _T536;struct Cyc_Absyn_Aggrfield*_T537;struct Cyc_Absyn_Exp*_T538;void*_T539;struct Cyc_Absyn_Aggrfield*_T53A;void*_T53B;int _T53C;struct Cyc_Absyn_Kind*_T53D;struct Cyc_Absyn_Kind*_T53E;struct Cyc_Absyn_Aggrfield*_T53F;void*_T540;struct Cyc_Absyn_Kind*_T541;int _T542;struct Cyc_Absyn_Aggrfield*_T543;struct Cyc_List_List*_T544;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T545;struct Cyc_Absyn_Aggrfield*_T546;struct Cyc_List_List*_T547;struct _fat_ptr*_T548;struct _fat_ptr _T549;struct Cyc_Int_pa_PrintArg_struct _T54A;int _T54B;int _T54C;void**_T54D;struct _fat_ptr _T54E;struct _fat_ptr _T54F;struct Cyc_List_List*_T550;struct _tuple1**_T551;struct Cyc_Absyn_Aggrdecl*_T552;struct Cyc_Absyn_Exp*_T553;struct Cyc_Absyn_Aggrdecl**_T554;union Cyc_Absyn_AggrInfo _T555;struct Cyc_List_List*_T556;struct Cyc_Warn_String_Warn_Warg_struct _T557;void**_T558;int(*_T559)(struct _fat_ptr ap);struct _fat_ptr _T55A;struct Cyc_Absyn_Aggrdecl*_T55B;struct Cyc_Absyn_AggrdeclImpl*_T55C;struct Cyc_Absyn_AggrdeclImpl*_T55D;int _T55E;struct Cyc_List_List*_T55F;void*_T560;struct Cyc_Absyn_Exp*_T561;void*_T562;struct Cyc_Absyn_Aggrfield*_T563;int _T564;int _T565;struct Cyc_Absyn_Exp*_T566;void*_T567;struct Cyc_Absyn_Exp*_T568;struct Cyc_Absyn_Aggrdecl*_T569;struct Cyc_Absyn_Aggrfield*_T56A;struct _fat_ptr*_T56B;struct Cyc_Absyn_Exp*_T56C;void*_T56D;struct Cyc_List_List*_T56E;struct _tuple20**_T56F;struct _tuple20*_T570;struct _tuple20**_T571;struct _tuple20*_T572;struct _fat_ptr _T573;struct Cyc_Absyn_Exp*_T574;struct Cyc_List_List*_T575;struct _tuple20**_T576;struct Cyc_Absyn_Aggrfield*_T577;struct _fat_ptr*_T578;struct Cyc_Absyn_Exp*_T579;struct _fat_ptr _T57A;struct Cyc_Absyn_Aggrdecl*_T57B;struct Cyc_Absyn_AggrdeclImpl*_T57C;struct Cyc_List_List*_T57D;void*_T57E;struct _tuple38*_T57F;struct _tuple38 _T580;struct Cyc_Absyn_Aggrfield*_T581;struct Cyc_List_List*_T582;void*_T583;struct Cyc_Absyn_Aggrfield*_T584;struct Cyc_List_List*_T585;void*_T586;struct Cyc_Absyn_Aggrfield*_T587;void*_T588;struct Cyc_Absyn_Exp*_T589;void*_T58A;void*_T58B;int _T58C;int _T58D;int _T58E;int _T58F;void*_T590;struct Cyc_Absyn_Exp*_T591;struct Cyc_List_List*_T592;struct _tuple20*_T593;struct Cyc_List_List*_T594;struct Cyc_List_List*_T595;struct Cyc_Absyn_Exp*_T596;struct Cyc_List_List*_T597;struct Cyc_Absyn_Datatypefield*_T598;struct Cyc_List_List*_T599;void*_T59A;struct Cyc_Absyn_Exp*_T59B;void*_T59C;struct Cyc_List_List*_T59D;void*_T59E;struct _tuple24*_T59F;struct _tuple24 _T5A0;void*_T5A1;int _T5A2;int _T5A3;struct Cyc_Absyn_Exp*_T5A4;void*_T5A5;struct Cyc_Absyn_Exp*_T5A6;struct Cyc_Absyn_Exp*_T5A7;struct Cyc_List_List*_T5A8;struct Cyc_List_List*_T5A9;struct Cyc_Absyn_Exp*_T5AA;struct Cyc_Absyn_Datatypedecl*_T5AB;int _T5AC;struct Cyc_Absyn_Datatypefield*_T5AD;struct _tuple1*_T5AE;struct Cyc_Absyn_Exp*_T5AF;void*_T5B0;struct Cyc_Absyn_Datatypedecl*_T5B1;struct Cyc_Absyn_Datatypefield*_T5B2;struct _tuple1*_T5B3;int _T5B4;struct Cyc_List_List*_T5B5;struct _fat_ptr*_T5B6;struct Cyc_List_List*_T5B7;void*_T5B8;struct Cyc_Absyn_Exp*_T5B9;struct Cyc_List_List*_T5BA;struct Cyc_Absyn_Exp*_T5BB;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T5BC;struct Cyc_Absyn_Datatypefield*_T5BD;struct _tuple1*_T5BE;struct Cyc_Absyn_Datatypedecl*_T5BF;struct _tuple1*_T5C0;struct Cyc_List_List*_T5C1;struct Cyc_List_List*_T5C2;struct _tuple20*_T5C3;struct Cyc_List_List*_T5C4;void*_T5C5;struct Cyc_List_List*_T5C6;struct Cyc_Absyn_Exp*_T5C7;struct Cyc_List_List*_T5C8;struct _tuple20*_T5C9;struct Cyc_Absyn_MallocInfo _T5CA;struct Cyc_Absyn_MallocInfo _T5CB;struct Cyc_Absyn_MallocInfo _T5CC;struct Cyc_Absyn_MallocInfo _T5CD;struct Cyc_Absyn_MallocInfo _T5CE;struct Cyc_Absyn_MallocInfo _T5CF;struct Cyc_Absyn_MallocInfo _T5D0;void**_T5D1;void*_T5D2;int _T5D3;enum Cyc_Absyn_MallocKind _T5D4;int _T5D5;int _T5D6;void**_T5D7;void*_T5D8;struct Cyc_Absyn_Exp*_T5D9;void*_T5DA;struct Cyc_Absyn_Exp*_T5DB;struct Cyc_Absyn_Exp*_T5DC;struct Cyc_Absyn_Exp*_T5DD;struct Cyc_Absyn_Exp*_T5DE;struct Cyc_Absyn_Exp*_T5DF;void*_T5E0;struct Cyc_Absyn_Exp*_T5E1;struct Cyc_Absyn_Exp*_T5E2;struct Cyc_Absyn_Exp*_T5E3;struct Cyc_Absyn_Exp*_T5E4;void*_T5E5;struct Cyc_Absyn_Exp*_T5E6;struct Cyc_Absyn_Exp*_T5E7;int _T5E8;void**_T5E9;void*_T5EA;struct Cyc_Absyn_Exp*_T5EB;enum Cyc_Absyn_MallocKind _T5EC;int _T5ED;struct Cyc_Absyn_Exp*(*_T5EE)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);int _T5EF;struct Cyc_Absyn_Exp*_T5F0;struct Cyc_Absyn_Exp*_T5F1;struct Cyc_Absyn_Exp*_T5F2;int _T5F3;struct Cyc_Absyn_Exp*_T5F4;enum Cyc_Absyn_MallocKind _T5F5;int _T5F6;struct Cyc_Absyn_Exp*_T5F7;void*_T5F8;struct Cyc_Absyn_Exp*_T5F9;void*_T5FA;struct Cyc_Absyn_Exp*_T5FB;struct Cyc_Absyn_Exp**_T5FC;struct Cyc_Absyn_Exp**_T5FD;struct Cyc_Absyn_Exp**_T5FE;struct Cyc_Absyn_Exp*_T5FF;struct _fat_ptr _T600;struct Cyc_Absyn_Exp*_T601;struct _tuple1*_T602;void*_T603;struct Cyc_Absyn_Exp*_T604;struct Cyc_Absyn_Stmt*_T605;struct Cyc_Absyn_Exp*_T606;struct Cyc_Absyn_Exp*_T607;void*_T608;struct Cyc_Absyn_Exp*_T609;struct Cyc_Absyn_Exp*_T60A;void*_T60B;struct Cyc_Absyn_Exp*_T60C;void*_T60D;int _T60E;int _T60F;struct Cyc_Warn_String_Warn_Warg_struct _T610;void**_T611;int(*_T612)(struct _fat_ptr ap);struct _fat_ptr _T613;struct Cyc_Absyn_Exp*_T614;struct Cyc_Absyn_Exp*_T615;struct Cyc_Absyn_Exp*_T616;struct Cyc_Absyn_Exp*_T617;struct Cyc_Absyn_Exp*_T618;unsigned _T619;struct Cyc_Absyn_Exp*_T61A;struct Cyc_Absyn_Exp*_T61B;struct Cyc_Absyn_Exp*_T61C;unsigned _T61D;struct Cyc_Absyn_Exp*_T61E;struct _tuple1*_T61F;void*_T620;struct Cyc_Absyn_Exp*_T621;struct _tuple1*_T622;void*_T623;struct Cyc_Absyn_Exp*_T624;struct Cyc_Absyn_Stmt*_T625;unsigned _T626;struct Cyc_Absyn_Stmt*_T627;unsigned _T628;struct Cyc_Absyn_Exp*_T629;struct Cyc_Absyn_Exp*_T62A;void*_T62B;void*_T62C;void*_T62D;int*_T62E;int _T62F;void*_T630;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T631;void*_T632;int*_T633;int _T634;void*_T635;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T636;void*_T637;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T638;union Cyc_Absyn_AggrInfo _T639;struct _union_AggrInfo_KnownAggr _T63A;unsigned _T63B;void*_T63C;void*_T63D;union Cyc_Absyn_AggrInfo _T63E;struct _union_AggrInfo_KnownAggr _T63F;struct Cyc_Absyn_Aggrdecl**_T640;int _T641;struct Cyc_Absyn_Exp*_T642;void*_T643;struct Cyc_Absyn_Exp*_T644;void*_T645;struct Cyc_Absyn_Exp*_T646;void*_T647;struct Cyc_Absyn_Exp*_T648;struct Cyc_Absyn_Exp*_T649;struct Cyc_Warn_String_Warn_Warg_struct _T64A;void**_T64B;int(*_T64C)(struct _fat_ptr ap);struct _fat_ptr _T64D;struct Cyc_Warn_String_Warn_Warg_struct _T64E;void**_T64F;int(*_T650)(struct _fat_ptr ap);struct _fat_ptr _T651;struct Cyc_Warn_String_Warn_Warg_struct _T652;void**_T653;int(*_T654)(struct _fat_ptr ap);struct _fat_ptr _T655;struct Cyc_Warn_String_Warn_Warg_struct _T656;void**_T657;int(*_T658)(struct _fat_ptr ap);struct _fat_ptr _T659;int _T65A;struct Cyc_Warn_String_Warn_Warg_struct _T65B;struct Cyc_Warn_Exp_Warn_Warg_struct _T65C;void**_T65D;void**_T65E;int(*_T65F)(unsigned,struct _fat_ptr);void*(*_T660)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Exp*_T661;unsigned _T662;struct _fat_ptr _T663;_T0=e;_T1=_T0->topt;
if(_T1!=0)goto _TL17A;{struct Cyc_Warn_String_Warn_Warg_struct _T664;_T664.tag=0;
_T664.f1=_tag_fat("exp_to_c: no type for ",sizeof(char),23U);_T2=_T664;}{struct Cyc_Warn_String_Warn_Warg_struct _T664=_T2;{struct Cyc_Warn_Exp_Warn_Warg_struct _T665;_T665.tag=4;_T665.f1=e;_T3=_T665;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T665=_T3;void*_T666[2];_T4=_T666 + 0;*_T4=& _T664;_T5=_T666 + 1;*_T5=& _T665;_T7=Cyc_Warn_impos_loc2;{int(*_T667)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T7;_T6=_T667;}_T8=e;_T9=_T8->loc;_TA=_tag_fat(_T666,sizeof(void*),2);_T6(_T9,_TA);}}goto _TL17B;_TL17A: _TL17B: _TB=e;{
void*old_typ=_TB->topt;
void*new_typ=Cyc_Toc_typ_to_c(old_typ);_TC=e;
_TC->topt=new_typ;{
# 1681
int did_inserted_checks=0;_TD=e;{
void*_T664=_TD->annot;_TE=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_T664;_TF=_TE->tag;_T10=Cyc_Absyn_EmptyAnnot;if(_TF!=_T10)goto _TL17C;goto _LL4;_TL17C: _T11=(struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_T664;_T12=_T11->tag;_T13=Cyc_InsertChecks_NoCheck;if(_T12!=_T13)goto _TL17E;_LL4:
# 1684
 did_inserted_checks=1;goto _LL0;_TL17E: goto _LL0;_LL0:;}_T14=e;{
# 1687
void*_T664=_T14->r;struct Cyc_Absyn_Stmt*_T665;struct _fat_ptr*_T666;struct Cyc_Absyn_Exp*_T667;void**_T668;struct Cyc_Absyn_Exp*_T669;enum Cyc_Absyn_MallocKind _T66A;struct Cyc_Absyn_Datatypefield*_T66B;struct Cyc_Absyn_Datatypedecl*_T66C;struct Cyc_Absyn_Aggrdecl*_T66D;struct Cyc_List_List*_T66E;enum Cyc_Absyn_Coercion _T66F;struct Cyc_Absyn_VarargInfo*_T670;struct Cyc_List_List*_T671;struct Cyc_Absyn_Exp*_T672;void*_T673;enum Cyc_Absyn_Incrementor _T674;enum Cyc_Absyn_Primop _T675;int _T676;int _T677;void*_T678;_T15=(int*)_T664;_T16=*_T15;switch(_T16){case 2: _T17=e;
# 1689
_T17->r=Cyc_Toc_zero_exp();goto _LL7;case 0: _T18=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T664;_T19=_T18->f1;_T1A=_T19.Null_c;_T1B=_T1A.tag;if(_T1B!=1)goto _TL181;_T1C=
# 1695
Cyc_Absyn_signed_int_exp(0,0U);_T1D=Cyc_Toc_void_star_type();{struct Cyc_Absyn_Exp*zero=Cyc_Absyn_set_type(_T1C,_T1D);_T1E=
# 1697
Cyc_Absyn_bounds_one();_T1F=Cyc_Tcutil_get_pointer_bounds(old_typ);_T20=_check_null(_T1F);_T21=Cyc_Tcutil_get_bounds_exp_constrain(_T1E,_T20,1);if(_T21!=0)goto _TL183;_T22=
Cyc_Toc_is_toplevel(nv);if(!_T22)goto _TL185;_T23=e;_T24=
Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero);_T23->r=_T24->r;goto _TL186;
# 1701
_TL185: _T25=e;{struct Cyc_Absyn_Exp*_T679[3];_T27=_T679 + 0;*_T27=zero;_T28=_T679 + 1;*_T28=zero;_T29=_T679 + 2;*_T29=zero;_T2A=Cyc_Toc__tag_fat_e;_T2B=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T26=Cyc_Toc_fncall_exp_r(_T2A,_T2B);}_T25->r=_T26;_TL186: goto _TL184;
# 1703
_TL183: _T2C=e;_T2C->r=Cyc_Toc_zero_exp();_TL184: goto _LL7;}_TL181: goto _LL7;case 1: goto _LL7;case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T677=_T679->f2;_T676=_T679->f3;}{struct Cyc_Absyn_Exp*e1=_T678;int static_only=_T677;int do_check=_T676;_T2D=static_only;
# 1708
if(_T2D)goto _TL189;else{goto _TL18A;}_TL18A: _T2E=do_check;if(_T2E)goto _TL187;else{goto _TL189;}
_TL189: _T2F=e;_T30=Cyc_Absyn_int_exp(0U,1,0U);_T2F->r=_T30->r;goto _TL188;
# 1712
_TL187: Cyc_Toc_exp_to_c(nv,e1);_T31=e;_T32=e1;_T33=
Cyc_Absyn_int_exp(0U,1,0U);_T34=
Cyc_Toc_typ_to_c(old_typ);_T35=Cyc_Toc__throw_assert_e;_T36=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_T37=
Cyc_Toc_fncall_exp_dl(_T35,_T36);_T38=Cyc_Toc_void_star_type();_T39=Cyc_Absyn_set_type(_T37,_T38);_T3A=
# 1714
Cyc_Toc_array_to_ptr_cast(_T34,_T39,0U);_T3B=
# 1713
Cyc_Absyn_conditional_exp(_T32,_T33,_T3A,0U);_T31->r=_T3B->r;_TL188: goto _LL7;}case 42: _T3C=e;_T3D=
# 1718
Cyc_Absyn_int_exp(0U,0,0U);_T3C->r=_T3D->r;goto _LL7;case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_Absyn_Exp*e1=_T678;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T664;_T675=_T679->f1;_T678=_T679->f2;}{enum Cyc_Absyn_Primop p=_T675;struct Cyc_List_List*es=_T678;_T3F=Cyc_List_map;{
# 1722
struct Cyc_List_List*(*_T679)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T3F;_T3E=_T679;}_T40=es;{struct Cyc_List_List*old_types=_T3E(Cyc_Toc_get_cyc_type,_T40);_T42=Cyc_List_iter_c;{
# 1724
void(*_T679)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))_T42;_T41=_T679;}_T43=nv;_T44=es;_T41(Cyc_Toc_exp_to_c,_T43,_T44);_T45=p;_T46=(int)_T45;switch(_T46){case Cyc_Absyn_Tagof: _T47=
# 1729
_check_null(es);_T48=_T47->hd;{struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_T48;_T49=e;_T4A=Cyc_Absyn_uint_type;_T4B=
Cyc_Absyn_const_tqual(0U);_T4C=Cyc_Absyn_cstar_type(_T4A,_T4B);_T4D=arg;_T4E=Cyc_Toc_cast_it(_T4C,_T4D);_T4F=Cyc_Absyn_deref_exp(_T4E,0U);_T49->r=_T4F->r;goto _LL62;}case Cyc_Absyn_Numelts: _T50=
# 1733
_check_null(es);_T51=_T50->hd;{struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_T51;_T52=
_check_null(old_types);_T53=_T52->hd;{void*_T679=Cyc_Absyn_compress(_T53);void*_T67A;void*_T67B;void*_T67C;void*_T67D;_T54=(int*)_T679;_T55=*_T54;if(_T55!=4)goto _TL18C;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T679;_T56=_T67E->f1;_T67D=_T56.elt_type;_T57=_T67E->f1;_T58=_T57.ptr_atts;_T67C=_T58.nullable;_T59=_T67E->f1;_T5A=_T59.ptr_atts;_T67B=_T5A.bounds;_T5B=_T67E->f1;_T5C=_T5B.ptr_atts;_T67A=_T5C.zero_term;}{void*elt_type=_T67D;void*nbl=_T67C;void*bound=_T67B;void*zt=_T67A;_T5D=
# 1736
Cyc_Absyn_bounds_one();_T5E=bound;{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp_constrain(_T5D,_T5E,1);
if(eopt!=0)goto _TL18E;_T5F=e;{struct Cyc_Absyn_Exp*_T67E[2];_T61=_T67E + 0;_T62=es;_T63=_T62->hd;
# 1740
*_T61=(struct Cyc_Absyn_Exp*)_T63;_T64=_T67E + 1;_T65=Cyc_Toc_typ_to_c(elt_type);_T66=Cyc_Absyn_sizeoftype_exp(_T65,0U);_T67=Cyc_Absyn_uint_type;*_T64=Cyc_Absyn_set_type(_T66,_T67);_T68=Cyc_Toc__get_fat_size_e;_T69=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),2);_T60=Cyc_Toc_fncall_exp_r(_T68,_T69);}
# 1739
_T5F->r=_T60;goto _TL18F;
# 1741
_TL18E: _T6A=Cyc_Tcutil_force_type2bool(0,zt);if(!_T6A)goto _TL190;_T6B=es;_T6C=_T6B->hd;_T6D=(struct Cyc_Absyn_Exp*)_T6C;_T6E=
# 1743
Cyc_Toc_isCharStar(_T6D);if(!_T6E)goto _TL192;_T6F=e;{struct Cyc_Absyn_Exp*_T67E[2];_T71=_T67E + 0;_T72=es;_T73=_T72->hd;
*_T71=(struct Cyc_Absyn_Exp*)_T73;_T74=_T67E + 1;*_T74=eopt;_T75=Cyc_Toc__get_zero_arr_size_char_e;_T76=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),2);_T70=Cyc_Toc_fncall_exp_r(_T75,_T76);}_T6F->r=_T70;goto _TL193;
# 1746
_TL192: _T77=e;{struct Cyc_Absyn_Exp*_T67E[3];_T79=_T67E + 0;_T7A=old_types;_T7B=_T7A->hd;_T7C=
Cyc_Tcutil_pointer_elt_type(_T7B);_T7D=Cyc_Toc_typ_to_c(_T7C);_T7E=Cyc_Absyn_sizeoftype_exp(_T7D,0U);_T7F=Cyc_Absyn_uint_type;*_T79=Cyc_Absyn_set_type(_T7E,_T7F);_T80=_T67E + 1;_T81=es;_T82=_T81->hd;
*_T80=(struct Cyc_Absyn_Exp*)_T82;_T83=_T67E + 2;*_T83=eopt;_T84=Cyc_Toc__get_zero_arr_size_other_e;_T85=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),3);_T78=Cyc_Toc_fncall_exp_r(_T84,_T85);}
# 1746
_T77->r=_T78;_TL193: goto _TL191;
# 1749
_TL190: _T86=Cyc_Tcutil_force_type2bool(0,nbl);if(!_T86)goto _TL194;_T87=
Cyc_Evexp_c_can_eval(eopt);if(_T87)goto _TL196;else{goto _TL198;}
_TL198:{struct Cyc_Warn_String_Warn_Warg_struct _T67E;_T67E.tag=0;_T67E.f1=_tag_fat("can't calculate numelts",sizeof(char),24U);_T88=_T67E;}{struct Cyc_Warn_String_Warn_Warg_struct _T67E=_T88;void*_T67F[1];_T89=_T67F + 0;*_T89=& _T67E;_T8A=e;_T8B=_T8A->loc;_T8C=_tag_fat(_T67F,sizeof(void*),1);Cyc_Warn_err2(_T8B,_T8C);}goto _TL197;_TL196: _TL197: _T8D=e;_T8E=arg;_T8F=eopt;_T90=
# 1753
Cyc_Absyn_uint_exp(0U,0U);_T91=Cyc_Absyn_uint_type;_T92=Cyc_Absyn_set_type(_T90,_T91);_T8D->r=Cyc_Toc_conditional_exp_r(_T8E,_T8F,_T92);goto _TL195;
# 1755
_TL194: _T93=e;_T94=eopt;_T93->r=_T94->r;_TL195: _TL191: _TL18F: goto _LL81;}}_TL18C:{struct Cyc_Warn_String_Warn_Warg_struct _T67E;_T67E.tag=0;
# 1757
_T67E.f1=_tag_fat("numelts primop applied to non-pointer ",sizeof(char),39U);_T95=_T67E;}{struct Cyc_Warn_String_Warn_Warg_struct _T67E=_T95;{struct Cyc_Warn_Typ_Warn_Warg_struct _T67F;_T67F.tag=2;_T97=arg;_T98=_T97->topt;
_T67F.f1=_check_null(_T98);_T96=_T67F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T67F=_T96;void*_T680[2];_T99=_T680 + 0;*_T99=& _T67E;_T9A=_T680 + 1;*_T9A=& _T67F;{
# 1757
int(*_T681)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T9B=_T681;}_T9C=_tag_fat(_T680,sizeof(void*),2);_T9B(_T9C);}}_LL81:;}goto _LL62;}case Cyc_Absyn_Plus: _T9D=
# 1765
Cyc_Toc_is_toplevel(nv);if(!_T9D)goto _TL199;{
int(*_T679)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_T9E=_T679;}_T9F=_tag_fat("can't do pointer arithmetic at top-level",sizeof(char),41U);_T9E(_T9F);goto _TL19A;_TL199: _TL19A: _TA0=
_check_null(old_types);_TA1=_TA0->hd;{void*_T679=Cyc_Absyn_compress(_TA1);void*_T67A;void*_T67B;void*_T67C;_TA2=(int*)_T679;_TA3=*_TA2;if(_TA3!=4)goto _TL19B;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T679;_TA4=_T67D->f1;_T67C=_TA4.elt_type;_TA5=_T67D->f1;_TA6=_TA5.ptr_atts;_T67B=_TA6.bounds;_TA7=_T67D->f1;_TA8=_TA7.ptr_atts;_T67A=_TA8.zero_term;}{void*elt_type=_T67C;void*b=_T67B;void*zt=_T67A;_TA9=
# 1769
Cyc_Absyn_bounds_one();_TAA=b;{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp_constrain(_TA9,_TAA,1);_TAB=
_check_null(es);_TAC=_TAB->hd;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TAC;_TAD=es;_TAE=_TAD->tl;_TAF=
_check_null(_TAE);_TB0=_TAF->hd;{struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_TB0;
if(eopt!=0)goto _TL19D;_TB1=e;{struct Cyc_Absyn_Exp*_T67D[3];_TB3=_T67D + 0;
# 1774
*_TB3=e1;_TB4=_T67D + 1;_TB5=Cyc_Toc_typ_to_c(elt_type);_TB6=Cyc_Absyn_sizeoftype_exp(_TB5,0U);_TB7=Cyc_Absyn_uint_type;*_TB4=Cyc_Absyn_set_type(_TB6,_TB7);_TB8=_T67D + 2;*_TB8=e2;_TB9=Cyc_Toc__fat_ptr_plus_e;_TBA=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),3);_TB2=Cyc_Toc_fncall_exp_r(_TB9,_TBA);}
# 1773
_TB1->r=_TB2;goto _TL19E;
# 1775
_TL19D: _TBB=Cyc_Tcutil_force_type2bool(0,zt);if(!_TBB)goto _TL19F;_TBC=
Cyc_Toc_isCharStar(e1);if(!_TBC)goto _TL1A1;_TBD=e;{struct Cyc_Absyn_Exp*_T67D[3];_TBF=_T67D + 0;
*_TBF=e1;_TC0=_T67D + 1;*_TC0=eopt;_TC1=_T67D + 2;*_TC1=e2;_TC2=Cyc_Toc__zero_arr_plus_char_e;_TC3=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),3);_TBE=Cyc_Toc_fncall_exp_r(_TC2,_TC3);}_TBD->r=_TBE;goto _TL1A2;
# 1779
_TL1A1: _TC4=e;{struct Cyc_Absyn_Exp*_T67D[3];_TC6=_T67D + 0;_TC7=old_types;_TC8=_TC7->hd;_TC9=
Cyc_Tcutil_pointer_elt_type(_TC8);_TCA=Cyc_Toc_typ_to_c(_TC9);_TCB=Cyc_Absyn_sizeoftype_exp(_TCA,0U);_TCC=Cyc_Absyn_uint_type;*_TC6=Cyc_Absyn_set_type(_TCB,_TCC);_TCD=_T67D + 1;
*_TCD=eopt;_TCE=_T67D + 2;*_TCE=e2;_TCF=Cyc_Toc__zero_arr_plus_other_e;_TD0=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),3);_TC5=Cyc_Toc_fncall_exp_r(_TCF,_TD0);}
# 1779
_TC4->r=_TC5;_TL1A2: goto _TL1A0;_TL19F: _TL1A0: _TL19E: goto _LL86;}}}}_TL19B: goto _LL86;_LL86:;}goto _LL62;case Cyc_Absyn_Minus:  {
# 1791
void*elt_type=Cyc_Absyn_void_type;_TD1=
_check_null(old_types);_TD2=_TD1->hd;_TD3=& elt_type;_TD4=Cyc_Tcutil_is_fat_pointer_type_elt(_TD2,_TD3);if(!_TD4)goto _TL1A3;_TD5=
_check_null(es);_TD6=_TD5->hd;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TD6;_TD7=es;_TD8=_TD7->tl;_TD9=
_check_null(_TD8);_TDA=_TD9->hd;{struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_TDA;_TDB=old_types;_TDC=_TDB->tl;_TDD=
_check_null(_TDC);_TDE=_TDD->hd;_TDF=Cyc_Tcutil_is_fat_pointer_type(_TDE);if(!_TDF)goto _TL1A5;_TE0=e1;_TE1=
Cyc_Absyn_copy_exp(e1);_TE2=Cyc_Toc_curr_sp;_TE0->r=Cyc_Toc_aggrmember_exp_r(_TE1,_TE2);_TE3=e2;_TE4=
Cyc_Absyn_copy_exp(e2);_TE5=Cyc_Toc_curr_sp;_TE3->r=Cyc_Toc_aggrmember_exp_r(_TE4,_TE5);_TE6=e1;_TE7=e2;
_TE7->topt=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);_TE6->topt=_TE7->topt;_TE8=e;_TE9=
Cyc_Absyn_copy_exp(e);_TEA=
Cyc_Toc_typ_to_c(elt_type);_TEB=Cyc_Absyn_sizeoftype_exp(_TEA,0U);_TEC=Cyc_Absyn_uint_type;_TED=Cyc_Absyn_set_type(_TEB,_TEC);_TEE=
# 1799
Cyc_Absyn_divide_exp(_TE9,_TED,0U);_TE8->r=_TEE->r;goto _TL1A6;
# 1802
_TL1A5: _TEF=e;{struct Cyc_Absyn_Exp*_T679[3];_TF1=_T679 + 0;
*_TF1=e1;_TF2=_T679 + 1;_TF3=Cyc_Toc_typ_to_c(elt_type);_TF4=Cyc_Absyn_sizeoftype_exp(_TF3,0U);_TF5=Cyc_Absyn_uint_type;*_TF2=Cyc_Absyn_set_type(_TF4,_TF5);_TF6=_T679 + 2;_TF7=
Cyc_Absyn_prim1_exp(2U,e2,0U);_TF8=Cyc_Absyn_sint_type;*_TF6=Cyc_Absyn_set_type(_TF7,_TF8);_TF9=Cyc_Toc__fat_ptr_plus_e;_TFA=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_TF0=Cyc_Toc_fncall_exp_r(_TF9,_TFA);}
# 1802
_TEF->r=_TF0;_TL1A6:;}}goto _TL1A4;_TL1A3: _TL1A4: goto _LL62;}case Cyc_Absyn_Eq: goto _LL6E;case Cyc_Absyn_Neq: _LL6E: goto _LL70;case Cyc_Absyn_Gt: _LL70: goto _LL72;case Cyc_Absyn_Gte: _LL72: goto _LL74;case Cyc_Absyn_Lt: _LL74: goto _LL76;case Cyc_Absyn_Lte: _LL76: goto _LL78;case Cyc_Absyn_UGt: _LL78: goto _LL7A;case Cyc_Absyn_UGte: _LL7A: goto _LL7C;case Cyc_Absyn_ULt: _LL7C: goto _LL7E;case Cyc_Absyn_ULte: _LL7E: _TFB=
# 1811
_check_null(es);_TFC=_TFB->hd;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TFC;_TFD=es;_TFE=_TFD->tl;_TFF=
_check_null(_TFE);_T100=_TFF->hd;{struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_T100;_T101=
_check_null(old_types);{void*t1=_T101->hd;_T102=old_types;_T103=_T102->tl;_T104=
_check_null(_T103);{void*t2=_T104->hd;
void*elt_type=Cyc_Absyn_void_type;_T105=t1;_T106=& elt_type;_T107=
Cyc_Tcutil_is_fat_pointer_type_elt(_T105,_T106);if(!_T107)goto _TL1A7;_T108=
Cyc_Toc_typ_to_c(elt_type);_T109=Cyc_Toc_mt_tq;{void*t=Cyc_Absyn_cstar_type(_T108,_T109);_T10A=e1;_T10B=t;_T10C=
Cyc_Absyn_copy_exp(e1);_T10D=Cyc_Toc_get_curr(_T10C);_T10A->r=Cyc_Toc_cast_it_r(_T10B,_T10D);_T10E=e1;
_T10E->topt=t;}goto _TL1A8;_TL1A7: _TL1A8: _T10F=
# 1821
Cyc_Tcutil_is_fat_pointer_type(t2);if(!_T10F)goto _TL1A9;_T110=
Cyc_Toc_typ_to_c(elt_type);_T111=Cyc_Toc_mt_tq;{void*t=Cyc_Absyn_cstar_type(_T110,_T111);_T112=e2;_T113=t;_T114=
Cyc_Absyn_copy_exp(e2);_T115=Cyc_Toc_get_curr(_T114);_T112->r=Cyc_Toc_cast_it_r(_T113,_T115);_T116=e2;
_T116->topt=t;}goto _TL1AA;_TL1A9: _TL1AA: goto _LL62;}}}}default: goto _LL62;}_LL62: goto _LL7;}}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T674=_T679->f2;}{struct Cyc_Absyn_Exp*e2=_T678;enum Cyc_Absyn_Incrementor incr=_T674;_T117=e2;_T118=_T117->topt;{
# 1831
void*e2_cyc_typ=_check_null(_T118);
# 1840 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=_tag_fat("increment",sizeof(char),10U);_T119=incr;_T11A=(int)_T119;
if(_T11A==2)goto _TL1AD;else{goto _TL1AE;}_TL1AE: _T11B=incr;_T11C=(int)_T11B;if(_T11C==3)goto _TL1AD;else{goto _TL1AB;}_TL1AD: incr_str=_tag_fat("decrement",sizeof(char),10U);goto _TL1AC;_TL1AB: _TL1AC: _T11D=e2;_T11E=& ignore_typ;_T11F=& ignore_bool;_T120=& ignore_typ;_T121=
Cyc_Tcutil_is_zero_ptr_deref(_T11D,_T11E,_T11F,_T120);if(!_T121)goto _TL1AF;{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;
_T679.f1=_tag_fat("in-place ",sizeof(char),10U);_T122=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T122;{struct Cyc_Warn_String_Warn_Warg_struct _T67A;_T67A.tag=0;_T67A.f1=incr_str;_T123=_T67A;}{struct Cyc_Warn_String_Warn_Warg_struct _T67A=_T123;{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;_T67B.f1=_tag_fat(" is not supported when ",sizeof(char),24U);_T124=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T124;{struct Cyc_Warn_String_Warn_Warg_struct _T67C;_T67C.tag=0;
_T67C.f1=_tag_fat("dereferencing a zero-terminated pointer",sizeof(char),40U);_T125=_T67C;}{struct Cyc_Warn_String_Warn_Warg_struct _T67C=_T125;void*_T67D[4];_T126=_T67D + 0;*_T126=& _T679;_T127=_T67D + 1;*_T127=& _T67A;_T128=_T67D + 2;*_T128=& _T67B;_T129=_T67D + 3;*_T129=& _T67C;_T12B=Cyc_Warn_impos_loc2;{
# 1846
int(*_T67E)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T12B;_T12A=_T67E;}_T12C=e;_T12D=_T12C->loc;_T12E=_tag_fat(_T67D,sizeof(void*),4);_T12A(_T12D,_T12E);}}}}goto _TL1B0;_TL1AF: _TL1B0: _T12F=e2;_T130=& ignore_int;_T131=& ignore_typ;_T132=
# 1848
Cyc_Toc_is_tagged_union_project(_T12F,_T130,_T131,1);if(!_T132)goto _TL1B1;{
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;_T133=one;
_T133->topt=Cyc_Absyn_sint_type;_T134=incr;_T135=(int)_T134;switch(_T135){case Cyc_Absyn_PreInc:
# 1853
 op=0U;goto _LL8B;case Cyc_Absyn_PreDec:
 op=2U;goto _LL8B;default:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;
_T679.f1=_tag_fat("in-place post",sizeof(char),14U);_T136=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T136;{struct Cyc_Warn_String_Warn_Warg_struct _T67A;_T67A.tag=0;_T67A.f1=incr_str;_T137=_T67A;}{struct Cyc_Warn_String_Warn_Warg_struct _T67A=_T137;{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;
_T67B.f1=_tag_fat(" is not supported on @tagged union members",sizeof(char),43U);_T138=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T138;void*_T67C[3];_T139=_T67C + 0;*_T139=& _T679;_T13A=_T67C + 1;*_T13A=& _T67A;_T13B=_T67C + 2;*_T13B=& _T67B;_T13D=Cyc_Warn_impos_loc2;{
# 1855
int(*_T67D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T13D;_T13C=_T67D;}_T13E=e;_T13F=_T13E->loc;_T140=_tag_fat(_T67C,sizeof(void*),3);_T13C(_T13F,_T140);}}}}_LL8B: _T141=e;{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T679=_cycalloc(sizeof(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct));_T679->tag=4;
# 1858
_T679->f1=e2;{struct Cyc_Core_Opt*_T67A=_cycalloc(sizeof(struct Cyc_Core_Opt));_T144=op;_T67A->v=(void*)_T144;_T143=(struct Cyc_Core_Opt*)_T67A;}_T679->f2=_T143;_T679->f3=one;_T142=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T679;}_T141->r=(void*)_T142;
Cyc_Toc_exp_to_c(nv,e);
return;}_TL1B1:
# 1862
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1867
void*elt_typ=Cyc_Absyn_void_type;_T145=old_typ;_T146=& elt_typ;_T147=
Cyc_Tcutil_is_fat_pointer_type_elt(_T145,_T146);if(!_T147)goto _TL1B4;{
int change=1;_T149=incr;_T14A=(int)_T149;
if(_T14A==1)goto _TL1B8;else{goto _TL1B9;}_TL1B9: _T14B=incr;_T14C=(int)_T14B;if(_T14C==3)goto _TL1B8;else{goto _TL1B6;}_TL1B8: _T148=Cyc_Toc__fat_ptr_inplace_plus_post_e;goto _TL1B7;_TL1B6: _T148=Cyc_Toc__fat_ptr_inplace_plus_e;_TL1B7: {struct Cyc_Absyn_Exp*fn_e=_T148;_T14D=incr;_T14E=(int)_T14D;
# 1872
if(_T14E==2)goto _TL1BC;else{goto _TL1BD;}_TL1BD: _T14F=incr;_T150=(int)_T14F;if(_T150==3)goto _TL1BC;else{goto _TL1BA;}
_TL1BC: change=-1;goto _TL1BB;_TL1BA: _TL1BB: _T151=e;{struct Cyc_Absyn_Exp*_T679[3];_T153=_T679 + 0;
*_T153=Cyc_Toc_push_address_exp(e2);_T154=_T679 + 1;_T155=
Cyc_Toc_typ_to_c(elt_typ);_T156=Cyc_Absyn_sizeoftype_exp(_T155,0U);_T157=Cyc_Absyn_uint_type;*_T154=Cyc_Absyn_set_type(_T156,_T157);_T158=_T679 + 2;_T159=
Cyc_Absyn_signed_int_exp(change,0U);_T15A=Cyc_Absyn_sint_type;*_T158=Cyc_Absyn_set_type(_T159,_T15A);_T15B=fn_e;_T15C=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T152=Cyc_Toc_fncall_exp_r(_T15B,_T15C);}
# 1874
_T151->r=_T152;}}goto _TL1B5;
# 1877
_TL1B4: _T15D=old_typ;_T15E=& elt_typ;_T15F=Cyc_Tcutil_is_zero_pointer_type_elt(_T15D,_T15E);if(!_T15F)goto _TL1BE;
# 1883
did_inserted_checks=1;_T160=incr;_T161=(int)_T160;
if(_T161==1)goto _TL1C0;{
struct _tuple1*x=Cyc_Toc_temp_var();
void*et=Cyc_Toc_typ_to_c(old_typ);
void*t=Cyc_Absyn_cstar_type(et,Cyc_Toc_mt_tq);_T162=
Cyc_Absyn_var_exp(x,0U);_T163=t;{struct Cyc_Absyn_Exp*xe=Cyc_Absyn_set_type(_T162,_T163);_T164=
Cyc_Absyn_deref_exp(xe,0U);_T165=et;{struct Cyc_Absyn_Exp*deref_x=Cyc_Absyn_set_type(_T164,_T165);
struct Cyc_Absyn_Exp*xexp=Cyc_Toc_push_address_exp(e2);_T166=
Cyc_Absyn_deref_exp(deref_x,0U);_T167=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*test_left=Cyc_Absyn_set_type(_T166,_T167);_T168=test_left;_T169=
Cyc_Absyn_int_exp(2U,0,0U);_T16A=Cyc_Absyn_sint_type;_T16B=Cyc_Absyn_set_type(_T169,_T16A);{struct Cyc_Absyn_Exp*testexp=Cyc_Absyn_neq_exp(_T168,_T16B,0U);_T16C=
Cyc_Toc_do_null_check(e);if(!_T16C)goto _TL1C2;_T16D=
Cyc_Absyn_copy_exp(deref_x);_T16E=
Cyc_Absyn_int_exp(2U,0,0U);_T16F=Cyc_Absyn_sint_type;_T170=Cyc_Absyn_set_type(_T16E,_T16F);_T171=
# 1894
Cyc_Absyn_neq_exp(_T16D,_T170,0U);_T172=testexp;testexp=Cyc_Absyn_and_exp(_T171,_T172,0U);goto _TL1C3;_TL1C2: _TL1C3: _T173=testexp;_T174=
# 1898
Cyc_Absyn_copy_exp(deref_x);_T175=Cyc_Absyn_increment_exp(_T174,0U,0U);_T176=Cyc_Absyn_exp_stmt(_T175,0U);_T177=Cyc_Toc__throw_arraybounds_e;_T178=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_T179=
Cyc_Toc_fncall_exp_dl(_T177,_T178);_T17A=
Cyc_Toc_void_star_type();_T17B=
# 1899
Cyc_Absyn_set_type(_T179,_T17A);_T17C=Cyc_Absyn_exp_stmt(_T17B,0U);{
# 1897
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_ifthenelse_stmt(_T173,_T176,_T17C,0U);_T17D=s;_T17E=
# 1901
Cyc_Absyn_copy_exp(deref_x);_T17F=Cyc_Absyn_exp_stmt(_T17E,0U);s=Cyc_Absyn_seq_stmt(_T17D,_T17F,0U);_T180=e;_T181=
Cyc_Absyn_declare_stmt(x,t,xexp,s,0U);_T182=Cyc_Absyn_stmt_exp(_T181,0U);_T180->r=_T182->r;}}}}}}goto _TL1C1;
# 1905
_TL1C0: _T183=Cyc_Toc_isCharStar(e2);if(!_T183)goto _TL1C4;_T184=e;{struct Cyc_Absyn_Exp*_T679[2];_T186=_T679 + 0;
# 1907
*_T186=Cyc_Toc_push_address_exp(e2);_T187=_T679 + 1;_T188=
Cyc_Absyn_signed_int_exp(1,0U);_T189=Cyc_Absyn_sint_type;*_T187=Cyc_Absyn_set_type(_T188,_T189);_T18A=Cyc_Toc__zero_arr_inplace_plus_post_char_e;_T18B=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),2);_T185=Cyc_Toc_fncall_exp_r(_T18A,_T18B);}
# 1906
_T184->r=_T185;goto _TL1C5;
# 1910
_TL1C4: _T18C=e;_T18D=Cyc_Toc_typ_to_c(e2_cyc_typ);{struct Cyc_Absyn_Exp*_T679[3];_T18F=_T679 + 0;_T190=
# 1912
Cyc_Toc_typ_to_c(e2_cyc_typ);_T191=Cyc_Tcutil_pointer_elt_type(_T190);_T192=Cyc_Absyn_sizeoftype_exp(_T191,0U);_T193=Cyc_Absyn_uint_type;*_T18F=Cyc_Absyn_set_type(_T192,_T193);_T194=_T679 + 1;
*_T194=Cyc_Toc_push_address_exp(e2);_T195=_T679 + 2;_T196=
Cyc_Absyn_signed_int_exp(1,0U);_T197=Cyc_Absyn_sint_type;*_T195=Cyc_Absyn_set_type(_T196,_T197);_T198=Cyc_Toc__zero_arr_inplace_plus_post_other_e;_T199=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T18E=Cyc_Toc_fncall_exp_dl(_T198,_T199);}
# 1910
_T18C->r=Cyc_Toc_cast_it_r(_T18D,_T18E);_TL1C5: _TL1C1: goto _TL1BF;
# 1916
_TL1BE: if(elt_typ!=Cyc_Absyn_void_type)goto _TL1C6;_T19A=Cyc_Absyn_is_lvalue(e2);if(_T19A)goto _TL1C6;else{goto _TL1C8;}
_TL1C8:{void(*_T679)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor))Cyc_Toc_lvalue_assign;_T19B=_T679;}_T19C=e2;_T19D=incr;_T19B(_T19C,0,Cyc_Toc_incr_lvalue,_T19D);_T19E=e;_T19F=e2;
*_T19E=*_T19F;goto _TL1C7;_TL1C6: _TL1C7: _TL1BF: _TL1B5: goto _LL7;}}}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T672=_T679->f3;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Core_Opt*popt=_T673;struct Cyc_Absyn_Exp*e2=_T672;_T1A0=e1;_T1A1=_T1A0->topt;{
# 1939 "toc.cyc"
void*e1_old_typ=_check_null(_T1A1);_T1A2=e2;_T1A3=_T1A2->topt;{
void*e2_old_typ=_check_null(_T1A3);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;_T1A4=e1;_T1A5=& f_tag;_T1A6=& tagged_member_struct_type;_T1A7=
Cyc_Toc_is_tagged_union_project(_T1A4,_T1A5,_T1A6,1);if(!_T1A7)goto _TL1C9;
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);_T1A8=e;
_T1A8->r=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);goto _LL7;_TL1C9: {
# 1952
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;_T1A9=e1;_T1AA=& ptr_type;_T1AB=& is_fat;_T1AC=& elt_type;_T1AD=
Cyc_Tcutil_is_zero_ptr_deref(_T1A9,_T1AA,_T1AB,_T1AC);if(!_T1AD)goto _TL1CB;
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
return;_TL1CB: {
# 1961
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1967
int done=0;
if(popt==0)goto _TL1CD;{
void*elt_typ=Cyc_Absyn_void_type;_T1AE=old_typ;_T1AF=& elt_typ;_T1B0=
Cyc_Tcutil_is_fat_pointer_type_elt(_T1AE,_T1AF);if(!_T1B0)goto _TL1CF;{
struct Cyc_Absyn_Exp*change;_T1B1=popt;_T1B2=_T1B1->v;{
enum Cyc_Absyn_Primop _T679=(enum Cyc_Absyn_Primop)_T1B2;_T1B3=(int)_T679;switch(_T1B3){case Cyc_Absyn_Plus:
 change=e2;goto _LL92;case Cyc_Absyn_Minus: _T1B4=
Cyc_Absyn_prim1_exp(2U,e2,0U);_T1B5=Cyc_Absyn_sint_type;change=Cyc_Absyn_set_type(_T1B4,_T1B5);goto _LL92;default:{struct Cyc_Warn_String_Warn_Warg_struct _T67A;_T67A.tag=0;
_T67A.f1=_tag_fat("bad t ? pointer arithmetic",sizeof(char),27U);_T1B6=_T67A;}{struct Cyc_Warn_String_Warn_Warg_struct _T67A=_T1B6;void*_T67B[1];_T1B7=_T67B + 0;*_T1B7=& _T67A;{int(*_T67C)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T1B8=_T67C;}_T1B9=_tag_fat(_T67B,sizeof(void*),1);_T1B8(_T1B9);}}_LL92:;}
# 1977
done=1;{
# 1979
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__fat_ptr_inplace_plus_e;_T1BA=e;{struct Cyc_Absyn_Exp*_T679[3];_T1BC=_T679 + 0;
*_T1BC=Cyc_Toc_push_address_exp(e1);_T1BD=_T679 + 1;_T1BE=
Cyc_Toc_typ_to_c(elt_typ);_T1BF=Cyc_Absyn_sizeoftype_exp(_T1BE,0U);_T1C0=Cyc_Absyn_uint_type;*_T1BD=Cyc_Absyn_set_type(_T1BF,_T1C0);_T1C1=_T679 + 2;
*_T1C1=change;_T1C2=fn_e;_T1C3=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T1BB=Cyc_Toc_fncall_exp_r(_T1C2,_T1C3);}
# 1980
_T1BA->r=_T1BB;}}goto _TL1D0;
# 1983
_TL1CF: _T1C4=old_typ;_T1C5=& elt_typ;_T1C6=Cyc_Tcutil_is_zero_pointer_type_elt(_T1C4,_T1C5);if(!_T1C6)goto _TL1D2;_T1C7=popt;_T1C8=_T1C7->v;{
# 1986
enum Cyc_Absyn_Primop _T679=(enum Cyc_Absyn_Primop)_T1C8;if(_T679!=Cyc_Absyn_Plus)goto _TL1D4;
# 1988
done=1;_T1C9=
Cyc_Toc_isCharStar(e1);if(!_T1C9)goto _TL1D6;_T1CA=e;{struct Cyc_Absyn_Exp*_T67A[2];_T1CC=_T67A + 0;
# 1991
*_T1CC=e1;_T1CD=_T67A + 1;*_T1CD=e2;_T1CE=Cyc_Toc__zero_arr_inplace_plus_char_e;_T1CF=_tag_fat(_T67A,sizeof(struct Cyc_Absyn_Exp*),2);_T1CB=Cyc_Toc_fncall_exp_r(_T1CE,_T1CF);}
# 1990
_T1CA->r=_T1CB;goto _TL1D7;
# 1993
_TL1D6: _T1D0=e;_T1D1=Cyc_Toc_typ_to_c(e1_old_typ);{struct Cyc_Absyn_Exp*_T67A[3];_T1D3=_T67A + 0;_T1D4=
# 1995
Cyc_Toc_typ_to_c(e1_old_typ);_T1D5=Cyc_Tcutil_pointer_elt_type(_T1D4);_T1D6=Cyc_Absyn_sizeoftype_exp(_T1D5,0U);_T1D7=Cyc_Absyn_uint_type;*_T1D3=Cyc_Absyn_set_type(_T1D6,_T1D7);_T1D8=_T67A + 1;
*_T1D8=e1;_T1D9=_T67A + 2;*_T1D9=e2;_T1DA=Cyc_Toc__zero_arr_inplace_plus_other_e;_T1DB=_tag_fat(_T67A,sizeof(struct Cyc_Absyn_Exp*),3);_T1D2=Cyc_Toc_fncall_exp_dl(_T1DA,_T1DB);}
# 1993
_T1D0->r=Cyc_Toc_cast_it_r(_T1D1,_T1D2);_TL1D7: goto _LL99;_TL1D4:{struct Cyc_Warn_String_Warn_Warg_struct _T67A;_T67A.tag=0;
# 1998
_T67A.f1=_tag_fat("bad zero-terminated pointer arithmetic",sizeof(char),39U);_T1DC=_T67A;}{struct Cyc_Warn_String_Warn_Warg_struct _T67A=_T1DC;void*_T67B[1];_T1DD=_T67B + 0;*_T1DD=& _T67A;{int(*_T67C)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T1DE=_T67C;}_T1DF=_tag_fat(_T67B,sizeof(void*),1);_T1DE(_T1DF);}_LL99:;}goto _TL1D3;_TL1D2: _TL1D3: _TL1D0:;}goto _TL1CE;_TL1CD: _TL1CE: _T1E0=done;
# 2001
if(_T1E0)goto _TL1D8;else{goto _TL1DA;}
# 2003
_TL1DA: _T1E1=e1_poly;if(!_T1E1)goto _TL1DB;_T1E2=e2;_T1E3=
Cyc_Toc_void_star_type();_T1E4=Cyc_Absyn_copy_exp(e2);_T1E2->r=Cyc_Toc_cast_it_r(_T1E3,_T1E4);_T1E5=e2;
_T1E5->topt=Cyc_Toc_void_star_type();goto _TL1DC;_TL1DB: _TL1DC: _T1E6=
# 2011
Cyc_Absyn_is_lvalue(e1);if(_T1E6)goto _TL1DD;else{goto _TL1DF;}
_TL1DF:{void(*_T679)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple34*),struct _tuple34*)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple34*),struct _tuple34*))Cyc_Toc_lvalue_assign;_T1E7=_T679;}_T1E8=e1;{struct _tuple34*_T679=_cycalloc(sizeof(struct _tuple34));_T679->f0=popt;_T679->f1=e2;_T1E9=(struct _tuple34*)_T679;}_T1E7(_T1E8,0,Cyc_Toc_assignop_lvalue,_T1E9);_T1EA=e;_T1EB=e1;
*_T1EA=*_T1EB;goto _TL1DE;_TL1DD: _TL1DE: goto _TL1D9;_TL1D8: _TL1D9: goto _LL7;}}}}}}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T672=_T679->f3;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Absyn_Exp*e2=_T673;struct Cyc_Absyn_Exp*e3=_T672;
# 2018
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);goto _LL7;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Absyn_Exp*e2=_T673;_T678=e1;_T673=e2;goto _LL21;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}_LL21: {struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Absyn_Exp*e2=_T673;_T678=e1;_T673=e2;goto _LL23;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}_LL23: {struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Absyn_Exp*e2=_T673;
# 2025
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);goto _LL7;}case 10: _T1EC=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T664;_T1ED=_T1EC->f3;if(_T1ED!=0)goto _TL1E0;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_List_List*es=_T673;_T1EE=e1;_T1EF=_T1EE->topt;{
# 2029
void*e1_typ=_check_null(_T1EF);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;_T1F0=
Cyc_Toc_do_null_check(e);if(!_T1F0)goto _TL1E2;_T1F1=e1;{struct Cyc_Absyn_Exp*_T679[1];_T1F3=_T679 + 0;*_T1F3=
Cyc_Absyn_copy_exp(e1);_T1F4=Cyc_Toc__check_null_e;_T1F5=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),1);_T1F2=Cyc_Toc_fncall_exp_dl(_T1F4,_T1F5);}_T1F1->r=_T1F2->r;goto _TL1E3;_TL1E2: _TL1E3: _T1F7=Cyc_List_iter_c;{
void(*_T679)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))_T1F7;_T1F6=_T679;}_T1F8=nv;_T1F9=es;_T1F6(Cyc_Toc_exp_to_c,_T1F8,_T1F9);goto _LL7;}}_TL1E0:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T1FA=_T679->f3;{struct Cyc_Absyn_VarargCallInfo _T67A=*_T1FA;_T677=_T67A.num_varargs;_T671=_T67A.injectors;_T670=_T67A.vai;}}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_List_List*es=_T673;int num_varargs=_T677;struct Cyc_List_List*injectors=_T671;struct Cyc_Absyn_VarargInfo*vai=_T670;struct _RegionHandle _T679=_new_region(0U,"r");struct _RegionHandle*r=& _T679;_push_region(r);{
# 2046 "toc.cyc"
struct _tuple31 _T67A=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T67B;struct _tuple1*_T67C;_T67C=_T67A.f0;_T67B=_T67A.f1;{struct _tuple1*argv=_T67C;struct Cyc_Absyn_Exp*argvexp=_T67B;_T1FB=
Cyc_Absyn_signed_int_exp(num_varargs,0U);_T1FC=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_set_type(_T1FB,_T1FC);_T1FD=vai;_T1FE=_T1FD->type;{
void*cva_type=Cyc_Toc_typ_to_c(_T1FE);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 2052
int num_args=Cyc_List_length(es);
int num_normargs=num_args - num_varargs;
# 2056
struct Cyc_List_List*new_args=0;{
int i=0;_TL1E7: if(i < num_normargs)goto _TL1E5;else{goto _TL1E6;}
_TL1E5: _T1FF=nv;_T200=_check_null(es);_T201=_T200->hd;_T202=(struct Cyc_Absyn_Exp*)_T201;Cyc_Toc_exp_to_c(_T1FF,_T202);{struct Cyc_List_List*_T67D=_cycalloc(sizeof(struct Cyc_List_List));_T204=es;_T205=_T204->hd;
_T67D->hd=(struct Cyc_Absyn_Exp*)_T205;_T67D->tl=new_args;_T203=(struct Cyc_List_List*)_T67D;}new_args=_T203;
# 2057
i=i + 1;_T206=es;es=_T206->tl;goto _TL1E7;_TL1E6:;}{struct Cyc_List_List*_T67D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Exp*_T67E[3];_T209=_T67E + 0;
# 2063
if(num_varargs!=0)goto _TL1E8;_T20A=
Cyc_Absyn_uint_exp(0U,0U);goto _TL1E9;_TL1E8: _T20A=argvexp;_TL1E9: _T20B=Cyc_Absyn_uint_type;
# 2062
*_T209=Cyc_Absyn_set_type(_T20A,_T20B);_T20C=_T67E + 1;_T20D=
# 2066
Cyc_Absyn_sizeoftype_exp(cva_type,0U);_T20E=Cyc_Absyn_uint_type;_T20F=Cyc_Absyn_set_type(_T20D,_T20E);_T210=Cyc_Absyn_uint_type;*_T20C=Cyc_Absyn_set_type(_T20F,_T210);_T211=_T67E + 2;
*_T211=num_varargs_exp;_T212=Cyc_Toc__tag_fat_e;_T213=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),3);_T208=Cyc_Toc_fncall_exp_dl(_T212,_T213);}_T214=Cyc_Toc_fat_ptr_type();
# 2061
_T67D->hd=Cyc_Absyn_set_type(_T208,_T214);
# 2068
_T67D->tl=new_args;_T207=(struct Cyc_List_List*)_T67D;}
# 2061
new_args=_T207;
# 2069
new_args=Cyc_List_imp_rev(new_args);_T215=e1;_T216=_T215->topt;{
# 2071
void*e1_typ=_check_null(_T216);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;_T217=
Cyc_Toc_do_null_check(e);if(!_T217)goto _TL1EA;_T218=e1;{struct Cyc_Absyn_Exp*_T67D[1];_T21A=_T67D + 0;*_T21A=
Cyc_Absyn_copy_exp(e1);_T21B=Cyc_Toc__check_null_e;_T21C=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),1);_T219=Cyc_Toc_fncall_exp_dl(_T21B,_T21C);}_T218->r=_T219->r;goto _TL1EB;_TL1EA: _TL1EB:
 if(num_varargs!=0)goto _TL1EC;_T21D=e;_T21E=
Cyc_Absyn_fncall_exp(e1,new_args,0U);_T21D->r=_T21E->r;_npop_handler(0);goto _LL7;_TL1EC: _T21F=
# 2080
Cyc_Absyn_fncall_exp(e1,new_args,0U);_T220=e;_T221=_T220->topt;_T222=_check_null(_T221);_T223=Cyc_Toc_typ_to_c(_T222);_T224=Cyc_Absyn_set_type(_T21F,_T223);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(_T224,0U);_T225=vai;_T226=_T225->inject;
# 2083
if(!_T226)goto _TL1EE;{
struct Cyc_Absyn_Datatypedecl*tud;_T227=vai;_T228=_T227->type;_T229=
Cyc_Tcutil_pointer_elt_type(_T228);{void*_T67D=Cyc_Absyn_compress(_T229);struct Cyc_Absyn_Datatypedecl*_T67E;_T22A=(int*)_T67D;_T22B=*_T22A;if(_T22B!=0)goto _TL1F0;_T22C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T67D;_T22D=_T22C->f1;_T22E=(int*)_T22D;_T22F=*_T22E;if(_T22F!=22)goto _TL1F2;_T230=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T67D;_T231=_T230->f1;_T232=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T231;_T233=_T232->f1;_T234=_T233.KnownDatatype;_T235=_T234.tag;if(_T235!=2)goto _TL1F4;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T67F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T67D;_T236=_T67F->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T680=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T236;_T237=_T680->f1;_T238=_T237.KnownDatatype;_T239=_T238.val;{struct Cyc_Absyn_Datatypedecl*_T681=*_T239;_T67E=_T681;}}}{struct Cyc_Absyn_Datatypedecl*x=_T67E;
tud=x;goto _LLA1;}_TL1F4: goto _LLA4;_TL1F2: goto _LLA4;_TL1F0: _LLA4:{struct Cyc_Warn_String_Warn_Warg_struct _T67F;_T67F.tag=0;
_T67F.f1=_tag_fat("unknown datatype in vararg with inject",sizeof(char),39U);_T23A=_T67F;}{struct Cyc_Warn_String_Warn_Warg_struct _T67F=_T23A;void*_T680[1];_T23B=_T680 + 0;*_T23B=& _T67F;{int(*_T681)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T23C=_T681;}_T23D=_tag_fat(_T680,sizeof(void*),1);_T23C(_T23D);}_LLA1:;}_T23F=num_varargs;{unsigned _T67D=(unsigned)_T23F;_T241=r;_T242=_check_times(_T67D,sizeof(struct _tuple35));{struct _tuple35*_T67E=_region_malloc(_T241,0U,_T242);_T243=num_varargs;{unsigned _T67F=(unsigned)_T243;unsigned i;i=0;_TL1F9: if(i < _T67F)goto _TL1F7;else{goto _TL1F8;}_TL1F7: _T245=i;_T244=_T67E + _T245;
# 2089
(*_T244).f0=Cyc_Toc_temp_var();_T247=i;_T246=_T67E + _T247;(*_T246).f1=0;i=i + 1;goto _TL1F9;_TL1F8:;}_T240=(struct _tuple35*)_T67E;}_T23E=_tag_fat(_T240,sizeof(struct _tuple35),_T67D);}{struct _fat_ptr vs=_T23E;
# 2091
struct Cyc_List_List*is2=Cyc_List_rev(injectors);_T248=
Cyc_List_length(es);{int i=_T248 - 1;{
struct Cyc_List_List*es2=es;_TL1FD: if(es2!=0)goto _TL1FB;else{goto _TL1FC;}
_TL1FB: _T249=es2;_T24A=_T249->hd;{struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_T24A;_T24B=arg;_T24C=_T24B->topt;{
void*arg_type=_check_null(_T24C);_T24D=vs;_T24E=i;_T24F=_check_fat_subscript(_T24D,sizeof(struct _tuple35),_T24E);_T250=(struct _tuple35*)_T24F;_T251=*_T250;{
struct _tuple1*var=_T251.f0;_T252=
_check_null(is2);_T253=_T252->hd;{struct Cyc_Absyn_Datatypefield*_T67D=(struct Cyc_Absyn_Datatypefield*)_T253;struct Cyc_List_List*_T67E;struct _tuple1*_T67F;{struct Cyc_Absyn_Datatypefield _T680=*_T67D;_T67F=_T680.name;_T67E=_T680.typs;}{struct _tuple1*qv=_T67F;struct Cyc_List_List*tqts=_T67E;_T254=qv;_T255=tud;_T256=_T255->name;{
struct _tuple1*tdn=Cyc_Toc_collapse_qvars(_T254,_T256);
struct Cyc_Absyn_AppType_Absyn_Type_struct*t;t=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T257=t;_T257->tag=0;_T258=t;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T680=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T680->tag=24;_T680->f1=Cyc_Absyn_UnknownAggr(0U,tdn,0);_T259=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T680;}_T258->f1=(void*)_T259;_T25A=t;_T25A->f2=0;
# 2101
_T25C=vs.curr;_T25D=(struct _tuple35*)_T25C;_T25E=i;_T25B=_T25D + _T25E;_T25F=t;(*_T25B).f1=(void*)_T25F;}}}}}}_T260=es2;
# 2093
es2=_T260->tl;_T261=is2;is2=_T261->tl;i=i + -1;goto _TL1FD;_TL1FC:;}{
# 2106
struct Cyc_List_List*array_args=0;{
int i=num_varargs - 1;_TL201: if(i >= 0)goto _TL1FF;else{goto _TL200;}
_TL1FF: _T262=vs;_T263=i;_T264=_check_fat_subscript(_T262,sizeof(struct _tuple35),_T263);_T265=(struct _tuple35*)_T264;{struct _tuple35 _T67D=*_T265;void*_T67E;struct _tuple1*_T67F;_T67F=_T67D.f0;_T67E=_T67D.f1;{struct _tuple1*v=_T67F;void*t=_T67E;_T266=
Cyc_Absyn_var_exp(v,0U);_T267=_check_null(t);{struct Cyc_Absyn_Exp*e1=Cyc_Absyn_set_type(_T266,_T267);_T268=
Cyc_Absyn_address_exp(e1,0U);_T269=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*e2=Cyc_Absyn_set_type(_T268,_T269);{struct Cyc_List_List*_T680=_cycalloc(sizeof(struct Cyc_List_List));
_T680->hd=e2;_T680->tl=array_args;_T26A=(struct Cyc_List_List*)_T680;}array_args=_T26A;}}}}
# 2107
i=i + -1;goto _TL201;_TL200:;}_T26B=argv;_T26C=arr_type;_T26D=
# 2114
Cyc_Absyn_array_exp(array_args,0U);_T26E=arr_type;_T26F=Cyc_Absyn_set_type(_T26D,_T26E);_T270=s;
# 2113
s=Cyc_Absyn_declare_stmt(_T26B,_T26C,_T26F,_T270,0U);
# 2116
es=Cyc_List_imp_rev(es);
injectors=Cyc_List_imp_rev(injectors);_T271=
Cyc_List_length(es);{int i=_T271 - 1;
_TL205: if(es!=0)goto _TL203;else{goto _TL204;}
_TL203: _T272=es;_T273=_T272->hd;{struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_T273;_T274=arg;_T275=_T274->topt;{
void*arg_type=_check_null(_T275);_T276=vs;_T277=i;_T278=_check_fat_subscript(_T276,sizeof(struct _tuple35),_T277);_T279=(struct _tuple35*)_T278;_T27A=*_T279;{
struct _tuple1*var=_T27A.f0;_T27B=vs;_T27C=_T27B.curr;_T27D=(struct _tuple35*)_T27C;_T27E=i;_T27F=_T27D[_T27E];_T280=_T27F.f1;{
void*field_typ=_check_null(_T280);_T281=
_check_null(injectors);_T282=_T281->hd;{struct Cyc_Absyn_Datatypefield*_T67D=(struct Cyc_Absyn_Datatypefield*)_T282;struct Cyc_List_List*_T67E;struct _tuple1*_T67F;{struct Cyc_Absyn_Datatypefield _T680=*_T67D;_T67F=_T680.name;_T67E=_T680.typs;}{struct _tuple1*qv=_T67F;struct Cyc_List_List*tqts=_T67E;
Cyc_Toc_exp_to_c(nv,arg);_T283=
Cyc_Toc_is_void_star_or_boxed_tvar(field_typ);if(!_T283)goto _TL206;
arg=Cyc_Toc_cast_it(field_typ,arg);goto _TL207;_TL206: _TL207: _T284=qv;_T285=tud;_T286=_T285->name;{
# 2129
struct _tuple1*tdn=Cyc_Toc_collapse_qvars(_T284,_T286);{struct _tuple20*_T680[2];_T288=_T680 + 0;_T289=Cyc_Toc_tag_sp;_T28A=
Cyc_Toc_datatype_tag(tud,qv);*_T288=Cyc_Toc_make_field(_T289,_T28A);_T28B=_T680 + 1;_T28C=
Cyc_Absyn_fieldname(1);_T28D=arg;*_T28B=Cyc_Toc_make_field(_T28C,_T28D);_T28E=_tag_fat(_T680,sizeof(struct _tuple20*),2);_T287=Cyc_List_list(_T28E);}{
# 2130
struct Cyc_List_List*dles=_T287;_T28F=var;_T290=
# 2132
Cyc_Absyn_strctq(tdn);{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T680=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T680->tag=28;
_T680->f1=tdn;_T680->f2=0;_T680->f3=dles;_T680->f4=0;_T291=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T680;}_T292=(void*)_T291;_T293=Cyc_Absyn_new_exp(_T292,0U);_T294=field_typ;_T295=Cyc_Absyn_set_type(_T293,_T294);_T296=s;
# 2132
s=Cyc_Absyn_declare_stmt(_T28F,_T290,_T295,_T296,0U);}}}}}}}}_T297=es;
# 2119
es=_T297->tl;_T298=injectors;injectors=_T298->tl;i=i + -1;goto _TL205;_TL204:;}}}}}goto _TL1EF;
# 2140
_TL1EE:{struct Cyc_List_List*array_args=0;
_TL20B: if(es!=0)goto _TL209;else{goto _TL20A;}
_TL209: _T299=nv;_T29A=es;_T29B=_T29A->hd;_T29C=(struct Cyc_Absyn_Exp*)_T29B;Cyc_Toc_exp_to_c(_T299,_T29C);{struct Cyc_List_List*_T67D=_cycalloc(sizeof(struct Cyc_List_List));_T29E=es;_T29F=_T29E->hd;
_T67D->hd=(struct Cyc_Absyn_Exp*)_T29F;_T67D->tl=array_args;_T29D=(struct Cyc_List_List*)_T67D;}array_args=_T29D;_T2A0=es;
# 2141
es=_T2A0->tl;goto _TL20B;_TL20A: _T2A1=
# 2145
Cyc_List_imp_rev(array_args);_T2A2=Cyc_Absyn_array_exp(_T2A1,0U);_T2A3=arr_type;{struct Cyc_Absyn_Exp*init=Cyc_Absyn_set_type(_T2A2,_T2A3);
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}_TL1EF: _T2A4=e;
# 2148
_T2A4->r=Cyc_Toc_stmt_exp_r(s);}}}}}}_npop_handler(0);goto _LL7;_pop_region();};case 11:{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T677=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T678;int b=_T677;
# 2153
Cyc_Toc_exp_to_c(nv,e1);_T2A6=b;
if(!_T2A6)goto _TL20C;_T2A5=Cyc_Toc__rethrow_e;goto _TL20D;_TL20C: _T2A5=Cyc_Toc__throw_e;_TL20D: {struct Cyc_Absyn_Exp*fn_e=_T2A5;
void*ctype=Cyc_Toc_typ_to_c(old_typ);_T2A7=e;_T2A8=ctype;{struct Cyc_Absyn_Exp*_T679[1];_T2AA=_T679 + 0;*_T2AA=e1;_T2AB=fn_e;_T2AC=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),1);_T2A9=Cyc_Toc_fncall_exp_dl(_T2AB,_T2AC);}_T2AD=ctype;_T2AE=
# 2157
Cyc_Absyn_set_type(_T2A9,_T2AD);_T2AF=
# 2156
Cyc_Toc_array_to_ptr_cast(_T2A8,_T2AE,0U);_T2A7->r=_T2AF->r;goto _LL7;}}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_Absyn_Exp*e1=_T678;
# 2160
Cyc_Toc_exp_to_c(nv,e1);_T2B0=e;_T2B1=e1;
*_T2B0=*_T2B1;goto _LL7;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_List_List*ts=_T673;
# 2164
Cyc_Toc_exp_to_c(nv,e1);{
# 2173 "toc.cyc"
void*tc=Cyc_Toc_typ_to_c(old_typ);_T2B2=e1;_T2B3=_T2B2->topt;_T2B4=
_check_null(_T2B3);{void*tc1=Cyc_Toc_typ_to_c(_T2B4);_T2B5=
Cyc_Tcutil_typecmp(tc,tc1);_T2B6=!_T2B5;if(_T2B6!=0)goto _TL20E;{
# 2185 "toc.cyc"
struct _tuple31 _T679=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T67A;struct _tuple1*_T67B;_T67B=_T679.f0;_T67A=_T679.f1;{struct _tuple1*temp=_T67B;struct Cyc_Absyn_Exp*temp_exp=_T67A;_T2B7=e;_T2B8=temp;_T2B9=tc;_T2BA=
Cyc_Toc_array_to_ptr_cast(tc,e1,0U);_T2BB=temp_exp;_T2BC=e;_T2BD=_T2BC->loc;_T2BE=
Cyc_Absyn_exp_stmt(_T2BB,_T2BD);_T2BF=e;_T2C0=_T2BF->loc;_T2C1=
# 2186
Cyc_Absyn_declare_stmt(_T2B8,_T2B9,_T2BA,_T2BE,_T2C0);_T2B7->r=Cyc_Toc_stmt_exp_r(_T2C1);}}goto _TL20F;_TL20E: _TL20F: goto _LL7;}}}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T664;_T2C2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T664;_T2C3=& _T2C2->f1;_T678=(void**)_T2C3;_T673=_T679->f2;_T677=_T679->f3;_T66F=_T679->f4;}{void**t=_T678;struct Cyc_Absyn_Exp*e1=_T673;int user_inserted=_T677;enum Cyc_Absyn_Coercion coercion=_T66F;_T2C4=e1;_T2C5=_T2C4->topt;{
# 2191
void*old_t2=_check_null(_T2C5);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);_T2C6=t;{
void*new_typ=*_T2C6;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);_T2C7=t;
*_T2C7=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);{struct _tuple36 _T679;
# 2198
_T679.f0=Cyc_Absyn_compress(old_t2);_T679.f1=Cyc_Absyn_compress(new_typ);_T2C8=_T679;}{struct _tuple36 _T679=_T2C8;struct Cyc_Absyn_PtrInfo _T67A;struct Cyc_Absyn_PtrInfo _T67B;_T2C9=_T679.f0;_T2CA=(int*)_T2C9;_T2CB=*_T2CA;if(_T2CB!=4)goto _TL210;_T2CC=_T679.f1;_T2CD=(int*)_T2CC;_T2CE=*_T2CD;switch(_T2CE){case 4: _T2CF=_T679.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2CF;_T67B=_T67C->f1;}_T2D0=_T679.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2D0;_T67A=_T67C->f1;}{struct Cyc_Absyn_PtrInfo p1=_T67B;struct Cyc_Absyn_PtrInfo p2=_T67A;_T2D1=
# 2200
Cyc_Absyn_bounds_one();_T2D2=p1;_T2D3=_T2D2.ptr_atts;_T2D4=_T2D3.bounds;{struct Cyc_Absyn_Exp*b1=Cyc_Tcutil_get_bounds_exp_constrain(_T2D1,_T2D4,1);_T2D5=
Cyc_Absyn_bounds_one();_T2D6=p2;_T2D7=_T2D6.ptr_atts;_T2D8=_T2D7.bounds;{struct Cyc_Absyn_Exp*b2=Cyc_Tcutil_get_bounds_exp_constrain(_T2D5,_T2D8,1);_T2D9=p1;_T2DA=_T2D9.ptr_atts;_T2DB=_T2DA.zero_term;{
int zt1=Cyc_Tcutil_force_type2bool(0,_T2DB);_T2DC=p2;_T2DD=_T2DC.ptr_atts;_T2DE=_T2DD.zero_term;{
int zt2=Cyc_Tcutil_force_type2bool(0,_T2DE);{struct _tuple37 _T67C;
_T67C.f0=b1;_T67C.f1=b2;_T2DF=_T67C;}{struct _tuple37 _T67C=_T2DF;_T2E0=_T67C.f0;if(_T2E0==0)goto _TL213;_T2E1=_T67C.f1;if(_T2E1==0)goto _TL215;
# 2207
did_inserted_checks=1;_T2E2=
Cyc_Toc_do_null_check(e);if(!_T2E2)goto _TL217;_T2E3=e;{struct Cyc_Absyn_Exp*_T67D[1];_T2E5=_T67D + 0;*_T2E5=e1;_T2E6=Cyc_Toc__check_null_e;_T2E7=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),1);_T2E4=Cyc_Toc_fncall_exp_dl(_T2E6,_T2E7);}
_T2E3->r=_T2E4->r;goto _TL218;
_TL217: _T2E8=Cyc_Unify_unify(old_t2_c,new_typ_c);if(!_T2E8)goto _TL219;_T2E9=e;_T2EA=e1;
*_T2E9=*_T2EA;goto _TL21A;_TL219: _TL21A: _TL218: goto _LLB9;_TL215:{
# 2218
struct Cyc_Absyn_Exp*e2=_check_null(b1);
struct _tuple13 _T67D=Cyc_Evexp_eval_const_uint_exp(e2);int _T67E;unsigned _T67F;_T67F=_T67D.f0;_T67E=_T67D.f1;{unsigned i=_T67F;int valid=_T67E;_T2EB=
Cyc_Toc_is_toplevel(nv);if(!_T2EB)goto _TL21B;_T2EC=zt1;
# 2224
if(!_T2EC)goto _TL21D;_T2ED=p2;_T2EE=_T2ED.elt_tq;_T2EF=_T2EE.real_const;if(_T2EF)goto _TL21D;else{goto _TL21F;}_TL21F: _T2F0=zt2;if(_T2F0)goto _TL21D;else{goto _TL220;}
_TL220: _T2F1=e2;_T2F2=Cyc_Absyn_uint_exp(1U,0U);_T2F3=Cyc_Absyn_uint_type;_T2F4=Cyc_Absyn_set_type(_T2F2,_T2F3);_T2F5=Cyc_Absyn_prim2_exp(2U,_T2F1,_T2F4,0U);_T2F6=e2;_T2F7=_T2F6->topt;_T2F8=_check_null(_T2F7);e2=Cyc_Absyn_set_type(_T2F5,_T2F8);goto _TL21E;_TL21D: _TL21E: _T2F9=e;_T2FA=
Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1);_T2F9->r=_T2FA->r;goto _TL21C;
_TL21B: _T2FB=zt1;if(!_T2FB)goto _TL221;{
# 2233
struct _tuple31 _T680=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T681;struct _tuple1*_T682;_T682=_T680.f0;_T681=_T680.f1;{struct _tuple1*x=_T682;struct Cyc_Absyn_Exp*x_exp=_T681;_T2FC=x_exp;_T2FD=
Cyc_Toc_typ_to_c(old_t2);Cyc_Absyn_set_type(_T2FC,_T2FD);{
struct Cyc_Absyn_Exp*arg3;_T2FE=e1;{
# 2238
void*_T683=_T2FE->r;struct Cyc_Absyn_Vardecl*_T684;_T2FF=(int*)_T683;_T300=*_T2FF;switch(_T300){case 0: _T301=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T683;_T302=_T301->f1;_T303=_T302.Wstring_c;_T304=_T303.tag;switch(_T304){case 8:
 arg3=e2;goto _LLC8;case 9:
 arg3=e2;goto _LLC8;default: goto _LLD1;};case 1: _T305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T683;_T306=_T305->f1;_T307=(int*)_T306;_T308=*_T307;switch(_T308){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T685=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T683;_T309=_T685->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T686=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T309;_T684=_T686->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T684;_T684=vd;goto _LLD0;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T685=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T683;_T30A=_T685->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T686=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T30A;_T684=_T686->f1;}}_LLD0: {struct Cyc_Absyn_Vardecl*vd=_T684;_T30B=vd;_T30C=_T30B->type;_T30D=
# 2243
Cyc_Tcutil_is_array_type(_T30C);if(_T30D)goto _TL226;else{goto _TL228;}
_TL228: goto _LLD2;_TL226:
 arg3=e2;goto _LLC8;}default: goto _LLD1;};default: _LLD1: _LLD2: _T30E=
# 2250
Cyc_Toc_void_star_type();_T30F=x_exp;x_exp=Cyc_Toc_cast_it(_T30E,_T30F);_T310=valid;
if(!_T310)goto _TL229;if(i==1U)goto _TL229;
arg3=e2;goto _TL22A;
_TL229: _T311=Cyc_Toc_isCharStar(e1);if(!_T311)goto _TL22B;{struct Cyc_Absyn_Exp*_T685[2];_T313=_T685 + 0;
*_T313=x_exp;_T314=_T685 + 1;*_T314=e2;_T315=Cyc_Toc__get_zero_arr_size_char_e;_T316=_tag_fat(_T685,sizeof(struct Cyc_Absyn_Exp*),2);_T312=Cyc_Toc_fncall_exp_dl(_T315,_T316);}arg3=_T312;goto _TL22C;
# 2256
_TL22B:{struct Cyc_Absyn_Exp*_T685[3];_T318=_T685 + 0;_T319=
Cyc_Toc_typ_to_c(old_t2);_T31A=Cyc_Tcutil_pointer_elt_type(_T319);_T31B=Cyc_Absyn_sizeoftype_exp(_T31A,0U);_T31C=Cyc_Absyn_uint_type;_T31D=Cyc_Absyn_set_type(_T31B,_T31C);_T31E=Cyc_Absyn_uint_type;*_T318=Cyc_Absyn_set_type(_T31D,_T31E);_T31F=_T685 + 1;
*_T31F=x_exp;_T320=_T685 + 2;*_T320=e2;_T321=Cyc_Toc__get_zero_arr_size_other_e;_T322=_tag_fat(_T685,sizeof(struct Cyc_Absyn_Exp*),3);_T317=Cyc_Toc_fncall_exp_dl(_T321,_T322);}
# 2256
arg3=_T317;_TL22C: _TL22A: _T323=arg3;
# 2259
_T323->topt=Cyc_Absyn_uint_type;goto _LLC8;}_LLC8:;}{
# 2264
int use_temp=arg3!=e2;_T324=zt2;
if(_T324)goto _TL22D;else{goto _TL22F;}_TL22F: _T325=p2;_T326=_T325.elt_tq;_T327=_T326.real_const;if(_T327)goto _TL22D;else{goto _TL230;}
_TL230: _T328=arg3;_T329=Cyc_Absyn_uint_exp(1U,0U);_T32A=Cyc_Absyn_uint_type;_T32B=Cyc_Absyn_set_type(_T329,_T32A);_T32C=Cyc_Absyn_prim2_exp(2U,_T328,_T32B,0U);_T32D=arg3;_T32E=_T32D->topt;_T32F=_check_null(_T32E);arg3=Cyc_Absyn_set_type(_T32C,_T32F);goto _TL22E;_TL22D: _TL22E: _T330=p2;_T331=_T330.elt_type;_T332=
Cyc_Toc_typ_to_c(_T331);_T333=Cyc_Absyn_sizeoftype_exp(_T332,0U);_T334=Cyc_Absyn_uint_type;_T335=Cyc_Absyn_set_type(_T333,_T334);_T336=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*arg2=Cyc_Absyn_set_type(_T335,_T336);_T337=use_temp;
if(!_T337)goto _TL231;{struct Cyc_Absyn_Exp*_T683[3];_T339=_T683 + 0;
*_T339=x_exp;_T33A=_T683 + 1;*_T33A=arg2;_T33B=_T683 + 2;*_T33B=arg3;_T33C=Cyc_Toc__tag_fat_e;_T33D=_tag_fat(_T683,sizeof(struct Cyc_Absyn_Exp*),3);_T338=Cyc_Toc_fncall_exp_dl(_T33C,_T33D);}{struct Cyc_Absyn_Exp*tg_exp=_T338;_T33E=tg_exp;
_T33E->topt=Cyc_Toc_fat_ptr_type();{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(tg_exp,0U);_T33F=x;_T340=
Cyc_Toc_typ_to_c(old_t2);_T341=e1;_T342=s;s=Cyc_Absyn_declare_stmt(_T33F,_T340,_T341,_T342,0U);_T343=e;
_T343->r=Cyc_Toc_stmt_exp_r(s);}}goto _TL232;
# 2275
_TL231: _T344=e;{struct Cyc_Absyn_Exp*_T683[3];_T346=_T683 + 0;*_T346=e1;_T347=_T683 + 1;*_T347=arg2;_T348=_T683 + 2;*_T348=arg3;_T349=Cyc_Toc__tag_fat_e;_T34A=_tag_fat(_T683,sizeof(struct Cyc_Absyn_Exp*),3);_T345=Cyc_Toc_fncall_exp_r(_T349,_T34A);}_T344->r=_T345;_TL232:;}}}}}goto _TL222;
# 2278
_TL221: _T34B=e;{struct Cyc_Absyn_Exp*_T680[3];_T34D=_T680 + 0;*_T34D=e1;_T34E=_T680 + 1;_T34F=p2;_T350=_T34F.elt_type;_T351=
Cyc_Toc_typ_to_c(_T350);_T352=Cyc_Absyn_sizeoftype_exp(_T351,0U);_T353=Cyc_Absyn_uint_type;_T354=Cyc_Absyn_set_type(_T352,_T353);_T355=Cyc_Absyn_uint_type;*_T34E=Cyc_Absyn_set_type(_T354,_T355);_T356=_T680 + 2;*_T356=e2;_T357=Cyc_Toc__tag_fat_e;_T358=_tag_fat(_T680,sizeof(struct Cyc_Absyn_Exp*),3);_T34C=Cyc_Toc_fncall_exp_r(_T357,_T358);}
# 2278
_T34B->r=_T34C;_TL222: _TL21C: goto _LLB9;}}_TL213: _T359=_T67C.f1;if(_T359==0)goto _TL233;{
# 2290 "toc.cyc"
struct Cyc_Absyn_Exp*new_e2=_check_null(b2);_T35A=zt1;
if(!_T35A)goto _TL235;_T35B=zt2;if(_T35B)goto _TL235;else{goto _TL237;}
_TL237: _T35C=b2;_T35D=Cyc_Absyn_uint_exp(1U,0U);_T35E=Cyc_Absyn_uint_type;_T35F=Cyc_Absyn_set_type(_T35D,_T35E);_T360=Cyc_Absyn_add_exp(_T35C,_T35F,0U);_T361=Cyc_Absyn_uint_type;new_e2=Cyc_Absyn_set_type(_T360,_T361);goto _TL236;_TL235: _TL236: {
struct Cyc_Absyn_Exp*ptr_exp=e1;_T362=p1;_T363=_T362.ptr_atts;_T364=_T363.aqual;_T365=
# 2298
Cyc_Tcutil_is_noalias_qual(_T364,0);if(!_T365)goto _TL238;_T366=p1;_T367=_T366.ptr_atts;_T368=_T367.aqual;_T369=
Cyc_Tcutil_is_noalias_qual(_T368,1);
# 2298
if(_T369)goto _TL238;else{goto _TL23A;}
# 2300
_TL23A:{struct Cyc_Absyn_Exp*_T67D[1];_T36B=_T67D + 0;*_T36B=ptr_exp;_T36C=Cyc_Toc__check_fat_at_base_e;_T36D=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),1);_T36A=Cyc_Toc_fncall_exp_dl(_T36C,_T36D);}_T36E=Cyc_Toc_fat_ptr_type();ptr_exp=Cyc_Absyn_set_type(_T36A,_T36E);goto _TL239;_TL238: _TL239: _T36F=e;{
# 2302
void*_T67D=_T36F->annot;_T370=(struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_T67D;_T371=_T370->tag;_T372=Cyc_InsertChecks_FatBound;if(_T371!=_T372)goto _TL23B;goto _LLD7;_TL23B: _T373=(struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_T67D;_T374=_T373->tag;_T375=Cyc_InsertChecks_NullAndFatBound;if(_T374!=_T375)goto _TL23D;_LLD7:{struct Cyc_Absyn_Exp*_T67E[3];_T377=_T67E + 0;
# 2311
*_T377=ptr_exp;_T378=_T67E + 1;_T379=p1;_T37A=_T379.elt_type;_T37B=
Cyc_Toc_typ_to_c(_T37A);_T37C=Cyc_Absyn_sizeoftype_exp(_T37B,0U);_T37D=Cyc_Absyn_uint_type;_T37E=Cyc_Absyn_set_type(_T37C,_T37D);_T37F=Cyc_Absyn_uint_type;*_T378=Cyc_Absyn_set_type(_T37E,_T37F);_T380=_T67E + 2;
# 2314
*_T380=new_e2;_T381=Cyc_Toc__untag_fat_ptr_check_bound_e;_T382=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),3);_T376=Cyc_Toc_fncall_exp_dl(_T381,_T382);}_T383=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);
# 2309
ptr_exp=
Cyc_Absyn_set_type(_T376,_T383);goto _LLD3;_TL23D:{struct Cyc_Absyn_Exp*_T67E[3];_T385=_T67E + 0;
# 2319
*_T385=ptr_exp;_T386=_T67E + 1;_T387=p1;_T388=_T387.elt_type;_T389=
Cyc_Toc_typ_to_c(_T388);_T38A=Cyc_Absyn_sizeoftype_exp(_T389,0U);_T38B=Cyc_Absyn_uint_type;_T38C=Cyc_Absyn_set_type(_T38A,_T38B);_T38D=Cyc_Absyn_uint_type;*_T386=Cyc_Absyn_set_type(_T38C,_T38D);_T38E=_T67E + 2;
# 2322
*_T38E=new_e2;_T38F=Cyc_Toc__untag_fat_ptr_e;_T390=_tag_fat(_T67E,sizeof(struct Cyc_Absyn_Exp*),3);_T384=Cyc_Toc_fncall_exp_dl(_T38F,_T390);}_T391=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);
# 2317
ptr_exp=
Cyc_Absyn_set_type(_T384,_T391);goto _LLD3;_LLD3:;}
# 2325
did_inserted_checks=1;_T392=
Cyc_Toc_do_null_check(e);if(!_T392)goto _TL23F;_T393=ptr_exp;{struct Cyc_Absyn_Exp*_T67D[1];_T395=_T67D + 0;*_T395=
Cyc_Absyn_copy_exp(ptr_exp);_T396=Cyc_Toc__check_null_e;_T397=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),1);_T394=Cyc_Toc_fncall_exp_r(_T396,_T397);}_T393->r=_T394;goto _TL240;_TL23F: _TL240: _T398=e;_T399=t;_T39A=*_T399;_T39B=ptr_exp;
_T398->r=Cyc_Toc_cast_it_r(_T39A,_T39B);goto _LLB9;}}_TL233: _T39C=zt1;
# 2333
if(!_T39C)goto _TL241;_T39D=zt2;if(_T39D)goto _TL241;else{goto _TL243;}_TL243: _T39E=p2;_T39F=_T39E.elt_tq;_T3A0=_T39F.real_const;if(_T3A0)goto _TL241;else{goto _TL244;}
_TL244: _T3A1=Cyc_Toc_is_toplevel(nv);if(!_T3A1)goto _TL245;{
int(*_T67D)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_T3A2=_T67D;}_T3A3=_tag_fat("can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",sizeof(char),70U);_T3A2(_T3A3);goto _TL246;_TL245: _TL246: _T3A4=e;{struct Cyc_Absyn_Exp*_T67D[3];_T3A6=_T67D + 0;
# 2337
*_T3A6=e1;_T3A7=_T67D + 1;_T3A8=p1;_T3A9=_T3A8.elt_type;_T3AA=
Cyc_Toc_typ_to_c(_T3A9);_T3AB=Cyc_Absyn_sizeoftype_exp(_T3AA,0U);_T3AC=Cyc_Absyn_uint_type;_T3AD=Cyc_Absyn_set_type(_T3AB,_T3AC);_T3AE=Cyc_Absyn_uint_type;*_T3A7=Cyc_Absyn_set_type(_T3AD,_T3AE);_T3AF=_T67D + 2;_T3B0=
# 2340
Cyc_Absyn_uint_exp(1U,0U);_T3B1=Cyc_Absyn_uint_type;*_T3AF=Cyc_Absyn_set_type(_T3B0,_T3B1);_T3B2=Cyc_Toc__fat_ptr_decrease_size_e;_T3B3=_tag_fat(_T67D,sizeof(struct Cyc_Absyn_Exp*),3);_T3A5=Cyc_Toc_fncall_exp_r(_T3B2,_T3B3);}
# 2336
_T3A4->r=_T3A5;goto _TL242;_TL241: _TL242: goto _LLB9;_LLB9:;}goto _LLB2;}}}}}case 0: _T3B4=_T679.f1;_T3B5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3B4;_T3B6=_T3B5->f1;_T3B7=(int*)_T3B6;_T3B8=*_T3B7;if(_T3B8!=1)goto _TL247;_T3B9=_T679.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3B9;_T67B=_T67C->f1;}{struct Cyc_Absyn_PtrInfo p1=_T67B;_T3BA=
# 2346
Cyc_Absyn_bounds_one();_T3BB=p1;_T3BC=_T3BB.ptr_atts;_T3BD=_T3BC.bounds;{struct Cyc_Absyn_Exp*_T67C=Cyc_Tcutil_get_bounds_exp_constrain(_T3BA,_T3BD,1);if(_T67C!=0)goto _TL249;_T3BE=e1;_T3BF=
# 2348
Cyc_Absyn_copy_exp(e1);_T3C0=Cyc_Toc_curr_sp;_T3BE->r=Cyc_Toc_aggrmember_exp_r(_T3BF,_T3C0);_T3C1=e1;
_T3C1->topt=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);goto _LLDA;_TL249: goto _LLDA;_LLDA:;}goto _LLB2;}_TL247: goto _LLB7;default: goto _LLB7;}goto _TL211;_TL210: _LLB7: _T3C2=
# 2355
Cyc_Unify_unify(old_t2_c,new_typ_c);if(!_T3C2)goto _TL24B;_T3C3=e;_T3C4=e1;
*_T3C3=*_T3C4;goto _TL24C;_TL24B: _TL24C: goto _LLB2;_TL211: _LLB2:;}goto _LL7;}}}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_Absyn_Exp*e1=_T678;
# 2362
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);_T3C5=
Cyc_Absyn_is_lvalue(e1);if(_T3C5)goto _TL24D;else{goto _TL24F;}
_TL24F:{void(*_T679)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int))Cyc_Toc_lvalue_assign;_T3C6=_T679;}_T3C7=e1;_T3C6(_T3C7,0,Cyc_Toc_address_lvalue,1);_T3C8=e;_T3C9=
# 2368
Cyc_Toc_typ_to_c(old_typ);_T3CA=e1;_T3C8->r=Cyc_Toc_cast_it_r(_T3C9,_T3CA);goto _TL24E;
_TL24D: _T3CB=& Cyc_Kinds_ak;_T3CC=(struct Cyc_Absyn_Kind*)_T3CB;_T3CD=e1;_T3CE=_T3CD->topt;_T3CF=_check_null(_T3CE);_T3D0=Cyc_Tcutil_type_kind(_T3CF);_T3D1=Cyc_Kinds_kind_leq(_T3CC,_T3D0);if(!_T3D1)goto _TL250;_T3D2=e;_T3D3=
# 2371
Cyc_Toc_typ_to_c(old_typ);_T3D4=
Cyc_Absyn_address_exp(e1,0U);_T3D5=e1;_T3D6=_T3D5->topt;_T3D7=
_check_null(_T3D6);_T3D8=Cyc_Toc_mt_tq;_T3D9=Cyc_Absyn_cstar_type(_T3D7,_T3D8);_T3DA=
# 2372
Cyc_Absyn_set_type(_T3D4,_T3D9);
# 2371
_T3D2->r=Cyc_Toc_cast_it_r(_T3D3,_T3DA);goto _TL251;_TL250: _TL251: _TL24E: goto _LL7;}case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T672=_T679->f3;}{struct Cyc_Absyn_Exp*rgnopt=_T678;struct Cyc_Absyn_Exp*e1=_T673;struct Cyc_Absyn_Exp*qual=_T672;
# 2381
Cyc_Toc_exp_to_c(nv,e1);_T3DB=e1;_T3DC=_T3DB->topt;_T3DD=
# 2383
_check_null(_T3DC);{void*elt_typ=Cyc_Toc_typ_to_c(_T3DD);{
void*_T679=Cyc_Absyn_compress(elt_typ);void*_T67A;_T3DE=(int*)_T679;_T3DF=*_T3DE;if(_T3DF!=5)goto _TL252;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T67B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T679;_T3E0=_T67B->f1;_T67A=_T3E0.elt_type;}{void*t2=_T67A;
elt_typ=Cyc_Toc_typ_to_c(t2);goto _LLDF;}_TL252: goto _LLDF;_LLDF:;}{
# 2390
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);_T3E2=array_len;_T3E3=(unsigned)_T3E2;
if(!_T3E3)goto _TL254;_T3E1=Cyc_Toc_temp_var();goto _TL255;_TL254: _T3E1=0;_TL255: {struct _tuple1*lenvar=_T3E1;_T3E5=array_len;_T3E6=(unsigned)_T3E5;
if(!_T3E6)goto _TL256;_T3E7=Cyc_Absyn_var_exp(lenvar,0U);_T3E8=Cyc_Absyn_uint_type;_T3E4=Cyc_Absyn_set_type(_T3E7,_T3E8);goto _TL257;_TL256: _T3E4=0;_TL257: {struct Cyc_Absyn_Exp*lenexp=_T3E4;
struct Cyc_Absyn_Exp*mexp;_T3E9=lenexp;_T3EA=(unsigned)_T3E9;
if(!_T3EA)goto _TL258;{struct Cyc_Absyn_Exp*_T679[2];_T3EC=_T679 + 0;
*_T3EC=lenexp;_T3ED=_T679 + 1;_T3EE=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_T3EF=Cyc_Absyn_uint_type;*_T3ED=Cyc_Absyn_set_type(_T3EE,_T3EF);_T3F0=Cyc_Toc__check_times_e;_T3F1=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),2);_T3EB=Cyc_Toc_fncall_exp_dl(_T3F0,_T3F1);}mexp=_T3EB;goto _TL259;
# 2397
_TL258: _T3F2=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_T3F3=Cyc_Absyn_uint_type;mexp=Cyc_Absyn_set_type(_T3F2,_T3F3);_TL259: _T3F4=mexp;
_T3F4->topt=Cyc_Absyn_uint_type;{
# 2401
struct Cyc_Absyn_Exp*vse=Cyc_Toc_get_varsizeexp(e1);
if(vse==0)goto _TL25A;_T3F5=
Cyc_Absyn_add_exp(mexp,vse,0U);_T3F6=Cyc_Absyn_uint_type;mexp=Cyc_Absyn_set_type(_T3F5,_T3F6);goto _TL25B;_TL25A: _TL25B:
# 2405
 if(rgnopt==0)goto _TL260;else{goto _TL25F;}_TL260: if(qual==0)goto _TL25E;else{goto _TL25F;}_TL25F: _T3F7=Cyc_Flags_no_regions;if(_T3F7)goto _TL25E;else{goto _TL25C;}
_TL25E: mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);goto _TL25D;
# 2408
_TL25C: if(qual==0)goto _TL261;Cyc_Toc_exp_to_c(nv,qual);goto _TL262;_TL261: _TL262:
 if(rgnopt==0)goto _TL263;
Cyc_Toc_exp_to_c(nv,rgnopt);_T3F8=rgnopt;
# 2412
if(qual!=0)goto _TL265;_T3F9=Cyc_Toc_aliasable_qual_exp();goto _TL266;_TL265: _T3F9=qual;_TL266: _T3FA=mexp;
# 2411
mexp=Cyc_Toc_rmalloc_exp(_T3F8,_T3F9,_T3FA);goto _TL264;
# 2416
_TL263: mexp=Cyc_Toc_aqmalloc_exp(qual,mexp);_TL264: _TL25D: _T3FB=mexp;_T3FC=
# 2419
Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);Cyc_Absyn_set_type(_T3FB,_T3FC);_T3FD=e;{struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T679=_cycalloc(sizeof(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct));_T679->tag=Cyc_Toc_NewInfo;
_T679->f1=mexp;_T679->f2=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);_T3FE=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T679;}_T3FD->annot=(void*)_T3FE;_T3FF=
Cyc_Tcutil_is_fat_ptr(old_typ);if(!_T3FF)goto _TL267;_T400=
Cyc_Absyn_copy_exp(e);_T401=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*e2=Cyc_Absyn_set_type(_T400,_T401);_T402=e;{struct Cyc_Absyn_Exp*_T679[3];_T404=_T679 + 0;
*_T404=e2;_T405=_T679 + 1;_T406=
Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_T407=Cyc_Absyn_uint_type;*_T405=Cyc_Absyn_set_type(_T406,_T407);_T408=lenexp;_T409=(unsigned)_T408;
if(!_T409)goto _TL269;_T40A=_T679 + 2;*_T40A=lenexp;goto _TL26A;_TL269: _T40B=_T679 + 2;_T40C=
Cyc_Absyn_uint_exp(1U,0U);_T40D=Cyc_Absyn_uint_type;*_T40B=Cyc_Absyn_set_type(_T40C,_T40D);_TL26A: _T40E=Cyc_Toc__tag_fat_e;_T40F=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T403=Cyc_Toc_fncall_exp_dl(_T40E,_T40F);}
# 2423
_T402->r=_T403->r;}goto _TL268;_TL267: _TL268:
# 2428
 if(vse==0)goto _TL26B;_T410=e;_T411=
Cyc_Toc_typ_to_c(old_typ);_T412=Cyc_Absyn_copy_exp(e);_T410->r=Cyc_Toc_cast_it_r(_T411,_T412);goto _TL26C;_TL26B: _TL26C: _T413=array_len;_T414=(unsigned)_T413;
if(!_T414)goto _TL26D;_T415=
_check_null(lenvar);_T416=Cyc_Absyn_uint_type;_T417=array_len;_T418=
Cyc_Absyn_copy_exp(e);_T419=Cyc_Absyn_exp_stmt(_T418,0U);{
# 2431
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T415,_T416,_T417,_T419,0U);_T41A=e;
# 2433
_T41A->r=Cyc_Toc_stmt_exp_r(s);}goto _TL26E;_TL26D: _TL26E: goto _LL7;}}}}}}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_Absyn_Exp*e1=_T678;
# 2438
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T664;_T41B=_T679->f1;_T678=(void*)_T41B;}{void*t=_T678;_T41C=e;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T679=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_T679->tag=17;
_T679->f1=Cyc_Toc_typ_to_c(t);_T41D=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T679;}_T41C->r=(void*)_T41D;goto _LL7;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T664;_T41E=_T679->f1;_T678=(void*)_T41E;_T673=_T679->f2;}{void*t=_T678;struct Cyc_List_List*fs=_T673;_T41F=e;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T679=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct));_T679->tag=19;
# 2441
_T679->f1=Cyc_Toc_typ_to_c(t);_T679->f2=fs;_T420=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T679;}_T41F->r=(void*)_T420;goto _LL7;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T677=_T679->f3;_T676=_T679->f4;}{struct Cyc_Absyn_Exp*e1=_T678;struct _fat_ptr*f=_T673;int is_tagged=_T677;int is_read=_T676;
# 2444
int is_poly=Cyc_Toc_is_poly_project(e);_T421=e1;_T422=_T421->topt;{
void*e1_cyc_type=_check_null(_T422);
void*t=Cyc_Toc_typ_to_c_array(old_typ);_T424=is_poly;
if(!_T424)goto _TL26F;_T423=Cyc_Toc_void_star_type();goto _TL270;_TL26F: _T423=t;_TL270: {void*res_type=_T423;
Cyc_Toc_exp_to_c(nv,e1);_T425=is_tagged;
if(!_T425)goto _TL271;_T426=is_read;if(!_T426)goto _TL271;_T427=e;_T428=res_type;_T429=e1;_T42A=
# 2451
Cyc_Toc_typ_to_c(e1_cyc_type);_T42B=e1_cyc_type;_T42C=f;_T42D=
Cyc_Toc_in_lhs(nv);
# 2450
_T427->r=Cyc_Toc_check_tagged_union(_T428,_T429,_T42A,_T42B,_T42C,_T42D,Cyc_Toc_member_exp);goto _TL272;
# 2453
_TL271: _T42E=is_tagged;if(!_T42E)goto _TL273;_T42F=e;
_T42F->topt=Cyc_Toc_tagged_union_field_struct(e1_cyc_type,f);goto _TL274;_TL273: _TL274: _TL272: _T430=is_poly;
if(!_T430)goto _TL275;{
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);_T431=e2;
_T431->topt=Cyc_Toc_void_star_type();_T432=e;
_T432->r=Cyc_Toc_cast_it_r(t,e2);_T433=e;
_T433->topt=t;}goto _TL276;_TL275: _TL276: goto _LL7;}}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T677=_T679->f3;_T676=_T679->f4;}{struct Cyc_Absyn_Exp*e1=_T678;struct _fat_ptr*f=_T673;int is_tagged=_T677;int is_read=_T676;
# 2463
int is_poly=Cyc_Toc_is_poly_project(e);_T434=e1;_T435=_T434->topt;{
void*e1typ=_check_null(_T435);
void*ta;
void*t=Cyc_Toc_typ_to_c_array(old_typ);_T437=is_poly;
if(!_T437)goto _TL277;_T436=Cyc_Toc_void_star_type();goto _TL278;_TL277: _T436=t;_TL278: {void*res_type=_T436;{
void*_T679=Cyc_Absyn_compress(e1typ);struct Cyc_Absyn_PtrInfo _T67A;_T438=(int*)_T679;_T439=*_T438;if(_T439!=4)goto _TL279;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T67B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T679;_T67A=_T67B->f1;}{struct Cyc_Absyn_PtrInfo p=_T67A;_T43A=p;
ta=_T43A.elt_type;goto _LLE4;}_TL279:{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;
_T67B.f1=_tag_fat("get_ptr_typ: not a pointer!",sizeof(char),28U);_T43B=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T43B;void*_T67C[1];_T43C=_T67C + 0;*_T43C=& _T67B;{int(*_T67D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T43D=_T67D;}_T43E=_tag_fat(_T67C,sizeof(void*),1);_T43D(_T43E);}_LLE4:;}
# 2472
did_inserted_checks=1;_T43F=nv;_T440=e1;_T441=e;_T442=_T441->annot;
Cyc_Toc_ptr_use_to_c(_T43F,_T440,_T442,0);_T443=is_tagged;
if(!_T443)goto _TL27B;_T444=is_read;if(!_T444)goto _TL27B;_T445=e;_T446=res_type;_T447=e1;_T448=
Cyc_Toc_typ_to_c(e1typ);_T449=ta;_T44A=f;_T44B=Cyc_Toc_in_lhs(nv);_T44C=Cyc_Absyn_aggrarrow_exp;_T445->r=Cyc_Toc_check_tagged_union(_T446,_T447,_T448,_T449,_T44A,_T44B,_T44C);goto _TL27C;
_TL27B: _T44D=is_tagged;if(!_T44D)goto _TL27D;_T44E=e;
_T44E->topt=Cyc_Toc_tagged_union_field_struct(ta,f);goto _TL27E;_TL27D: _TL27E: _TL27C: _T44F=is_poly;
# 2479
if(!_T44F)goto _TL27F;_T450=is_read;if(!_T450)goto _TL27F;{
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);_T451=e2;
_T451->topt=Cyc_Toc_void_star_type();_T452=e;
_T452->r=Cyc_Toc_cast_it_r(t,e2);_T453=e;
_T453->topt=t;}goto _TL280;
_TL27F: _T454=is_poly;if(!_T454)goto _TL281;_T455=e;
_T455->topt=Cyc_Toc_void_star_type();goto _TL282;_TL281: _TL282: _TL280: goto _LL7;}}}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_Absyn_Exp*e1=_T678;
# 2489
did_inserted_checks=1;_T456=nv;_T457=e1;_T458=e;_T459=_T458->annot;
Cyc_Toc_ptr_use_to_c(_T456,_T457,_T459,0);goto _LL7;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T678;struct Cyc_Absyn_Exp*e2=_T673;
# 2493
did_inserted_checks=1;_T45A=nv;_T45B=e1;_T45C=e;_T45D=_T45C->annot;_T45E=e2;{
enum Cyc_Toc_PTR_ANS res=Cyc_Toc_ptr_use_to_c(_T45A,_T45B,_T45D,_T45E);_T45F=res;_T460=(int)_T45F;
if(_T460!=0)goto _TL283;_T461=e;
_T461->r=Cyc_Toc_deref_exp_r(e1);goto _TL284;
_TL283: _T462=res;_T463=(int)_T462;if(_T463!=1)goto _TL285;_T464=e;_T465=e1;
_T464->r=_T465->r;goto _TL286;_TL285: _TL286: _TL284: goto _LL7;}}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;}{struct Cyc_List_List*dles0=_T678;{
# 2505
struct Cyc_List_List*dles=dles0;_TL28A: if(dles!=0)goto _TL288;else{goto _TL289;}
_TL288: _T466=nv;_T467=dles;_T468=_T467->hd;_T469=(struct _tuple20*)_T468;_T46A=*_T469;_T46B=_T46A.f1;Cyc_Toc_exp_to_c(_T466,_T46B);_T46C=dles;
# 2505
dles=_T46C->tl;goto _TL28A;_TL289:;}_T46D=
# 2507
Cyc_Toc_is_toplevel(nv);if(!_T46D)goto _TL28B;_T46E=e;
_T46E->r=Cyc_Toc_unresolvedmem_exp_r(0,dles0);goto _TL28C;_TL28B: _TL28C: goto _LL7;}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T673=_T679->f2;_T672=_T679->f3;_T677=_T679->f4;}{struct Cyc_Absyn_Vardecl*vd=_T678;struct Cyc_Absyn_Exp*e1=_T673;struct Cyc_Absyn_Exp*e2=_T672;int iszeroterm=_T677;
# 2513
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);_T46F=
Cyc_Toc_is_toplevel(nv);if(!_T46F)goto _TL28D;{
struct _tuple13 _T679=Cyc_Evexp_eval_const_uint_exp(e1);int _T67A;unsigned _T67B;_T67B=_T679.f0;_T67A=_T679.f1;{unsigned sz=_T67B;int known=_T67A;_T470=e2;_T471=_T470->topt;_T472=
_check_null(_T471);{void*elt_typ=Cyc_Toc_typ_to_c(_T472);
struct Cyc_List_List*es=0;_T473=
# 2520
Cyc_Toc_is_zero(e2);if(_T473)goto _TL28F;else{goto _TL291;}
_TL291: _T474=known;if(_T474)goto _TL292;else{goto _TL294;}
_TL294:{struct Cyc_Warn_String_Warn_Warg_struct _T67C;_T67C.tag=0;_T67C.f1=_tag_fat("cannot determine value of constant",sizeof(char),35U);_T475=_T67C;}{struct Cyc_Warn_String_Warn_Warg_struct _T67C=_T475;void*_T67D[1];_T476=_T67D + 0;*_T476=& _T67C;_T477=e1;_T478=_T477->loc;_T479=_tag_fat(_T67D,sizeof(void*),1);Cyc_Warn_err2(_T478,_T479);}goto _TL293;_TL292: _TL293:{
unsigned i=0U;_TL298: if(i < sz)goto _TL296;else{goto _TL297;}
_TL296:{struct Cyc_List_List*_T67C=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T67D=_cycalloc(sizeof(struct _tuple20));_T67D->f0=0;_T67D->f1=e2;_T47B=(struct _tuple20*)_T67D;}_T67C->hd=_T47B;_T67C->tl=es;_T47A=(struct Cyc_List_List*)_T67C;}es=_T47A;
# 2523
i=i + 1;goto _TL298;_TL297:;}_T47C=iszeroterm;
# 2526
if(!_T47C)goto _TL299;_T47D=elt_typ;_T47E=
Cyc_Absyn_uint_exp(0U,0U);_T47F=Cyc_Absyn_uint_type;_T480=Cyc_Absyn_set_type(_T47E,_T47F);{struct Cyc_Absyn_Exp*ezero=Cyc_Toc_cast_it(_T47D,_T480);_T481=es;{struct Cyc_List_List*_T67C=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T67D=_cycalloc(sizeof(struct _tuple20));
_T67D->f0=0;_T67D->f1=ezero;_T483=(struct _tuple20*)_T67D;}_T67C->hd=_T483;_T67C->tl=0;_T482=(struct Cyc_List_List*)_T67C;}es=Cyc_List_imp_append(_T481,_T482);}goto _TL29A;_TL299: _TL29A: goto _TL290;_TL28F: _TL290: _T484=e;
# 2531
_T484->r=Cyc_Toc_unresolvedmem_exp_r(0,es);}}}goto _TL28E;_TL28D: _TL28E: goto _LL7;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T664;_T678=_T679->f1;_T485=_T679->f2;_T673=(void*)_T485;_T677=_T679->f3;}{struct Cyc_Absyn_Exp*e1=_T678;void*t1=_T673;int iszeroterm=_T677;_T486=
# 2540
Cyc_Toc_is_toplevel(nv);if(!_T486)goto _TL29B;_T487=e;
_T487->r=Cyc_Toc_unresolvedmem_exp_r(0,0);goto _TL29C;
# 2543
_TL29B: Cyc_Toc_exp_to_c(nv,e1);_TL29C: goto _LL7;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T664;_T488=_T679->f1;_T678=(void*)_T488;_T671=_T679->f3;}{void*st=_T678;struct Cyc_List_List*dles=_T671;{
# 2547
struct Cyc_List_List*dles2=dles;_TL2A0: if(dles2!=0)goto _TL29E;else{goto _TL29F;}
_TL29E: _T489=nv;_T48A=dles2;_T48B=_T48A->hd;_T48C=(struct _tuple20*)_T48B;_T48D=*_T48C;_T48E=_T48D.f1;Cyc_Toc_exp_to_c(_T489,_T48E);_T48F=dles2;
# 2547
dles2=_T48F->tl;goto _TL2A0;_TL29F:;}{
# 2549
void*strct_typ=Cyc_Toc_typ_to_c(old_typ);_T490=e;
_T490->topt=strct_typ;_T491=
Cyc_Toc_is_toplevel(nv);if(_T491)goto _TL2A1;else{goto _TL2A3;}
_TL2A3:{void*_T679=Cyc_Absyn_compress(strct_typ);union Cyc_Absyn_AggrInfo _T67A;_T492=(int*)_T679;_T493=*_T492;if(_T493!=0)goto _TL2A4;_T494=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T495=_T494->f1;_T496=(int*)_T495;_T497=*_T496;if(_T497!=24)goto _TL2A6;_T498=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T499=_T498->f2;if(_T499!=0)goto _TL2A8;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T67B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T49A=_T67B->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T67C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T49A;_T67A=_T67C->f1;}}{union Cyc_Absyn_AggrInfo aggrinfo=_T67A;
# 2554
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);_T49B=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T67B=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T67B->tag=28;_T49D=sd;
_T67B->f1=_T49D->name;_T67B->f2=0;_T67B->f3=dles;_T67B->f4=sd;_T49C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T67B;}_T49B->r=(void*)_T49C;_T49E=e;
_T49E->topt=strct_typ;goto _LLEC;}_TL2A8: goto _LLEF;_TL2A6: goto _LLEF;_TL2A4: _LLEF:{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;
# 2558
_T67B.f1=_tag_fat("anonStruct type not an aggregate",sizeof(char),33U);_T49F=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T49F;void*_T67C[1];_T4A0=_T67C + 0;*_T4A0=& _T67B;{int(*_T67D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T4A1=_T67D;}_T4A2=_tag_fat(_T67C,sizeof(void*),1);_T4A1(_T4A2);}_LLEC:;}goto _TL2A2;
# 2561
_TL2A1: _T4A3=e;_T4A3->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);_TL2A2: goto _LL7;}}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T664;_T4A4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T664;_T4A5=& _T4A4->f1;_T678=(struct _tuple1**)_T4A5;_T671=_T679->f2;_T66E=_T679->f3;_T66D=_T679->f4;}{struct _tuple1**tdn=(struct _tuple1**)_T678;struct Cyc_List_List*exist_ts=_T671;struct Cyc_List_List*dles=_T66E;struct Cyc_Absyn_Aggrdecl*sd=_T66D;
# 2567
if(sd==0)goto _TL2AC;else{goto _TL2AD;}_TL2AD: _T4A6=sd;_T4A7=_T4A6->impl;if(_T4A7==0)goto _TL2AC;else{goto _TL2AA;}
_TL2AC:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;_T679.f1=_tag_fat("exp_to_c, Aggregate_e: missing aggrdecl pointer or fields",sizeof(char),58U);_T4A8=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T4A8;void*_T67A[1];_T4A9=_T67A + 0;*_T4A9=& _T679;{int(*_T67B)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T4AA=_T67B;}_T4AB=_tag_fat(_T67A,sizeof(void*),1);_T4AA(_T4AB);}goto _TL2AB;_TL2AA: _TL2AB: {
void*new_typ=Cyc_Toc_typ_to_c(old_typ);{
void*_T679=Cyc_Absyn_compress(new_typ);union Cyc_Absyn_AggrInfo _T67A;_T4AC=(int*)_T679;_T4AD=*_T4AC;if(_T4AD!=0)goto _TL2AE;_T4AE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T4AF=_T4AE->f1;_T4B0=(int*)_T4AF;_T4B1=*_T4B0;if(_T4B1!=24)goto _TL2B0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T67B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T4B2=_T67B->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T67C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4B2;_T67A=_T67C->f1;}}{union Cyc_Absyn_AggrInfo info=_T67A;_T4B3=tdn;_T4B4=
Cyc_Absyn_aggr_kinded_name(info);*_T4B3=_T4B4.f1;goto _LLF1;}_TL2B0: goto _LLF4;_TL2AE: _LLF4:{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;
_T67B.f1=_tag_fat("exp_to_c, Aggregate_e: bad type translation",sizeof(char),44U);_T4B5=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T4B5;void*_T67C[1];_T4B6=_T67C + 0;*_T4B6=& _T67B;{int(*_T67D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T4B7=_T67D;}_T4B8=_tag_fat(_T67C,sizeof(void*),1);_T4B7(_T4B8);}_LLF1:;}_T4B9=sd;_T4BA=_T4B9->impl;_T4BB=
# 2578
_check_null(_T4BA);{struct Cyc_List_List*typ_fields=_T4BB->fields;
if(typ_fields!=0)goto _TL2B2;goto _LL7;_TL2B2:
 _TL2B7: _T4BC=_check_null(typ_fields);_T4BD=_T4BC->tl;if(_T4BD!=0)goto _TL2B5;else{goto _TL2B6;}
_TL2B5: _T4BE=typ_fields;
# 2580
typ_fields=_T4BE->tl;goto _TL2B7;_TL2B6: _T4BF=typ_fields;_T4C0=_T4BF->hd;{
# 2582
struct Cyc_Absyn_Aggrfield*last_typ_field=(struct Cyc_Absyn_Aggrfield*)_T4C0;_T4C1=Cyc_Core_heap_region;_T4C2=e;_T4C3=_T4C2->loc;_T4C4=dles;_T4C5=sd;_T4C6=_T4C5->kind;_T4C7=sd;_T4C8=_T4C7->impl;_T4C9=_T4C8->fields;{
struct Cyc_List_List*fields=Cyc_Tcutil_resolve_aggregate_designators(_T4C1,_T4C3,_T4C4,_T4C6,_T4C9);_T4CA=
# 2585
Cyc_Toc_is_toplevel(nv);if(_T4CA)goto _TL2B8;else{goto _TL2BA;}
_TL2BA:{void*_T679=Cyc_Absyn_compress(old_typ);struct Cyc_List_List*_T67A;_T4CB=(int*)_T679;_T4CC=*_T4CB;if(_T4CC!=0)goto _TL2BB;_T4CD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T4CE=_T4CD->f1;_T4CF=(int*)_T4CE;_T4D0=*_T4CF;if(_T4D0!=24)goto _TL2BD;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T67B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T679;_T67A=_T67B->f2;}{struct Cyc_List_List*param_ts=_T67A;{
# 2589
struct Cyc_List_List*fields2=fields;_TL2C2: if(fields2!=0)goto _TL2C0;else{goto _TL2C1;}
_TL2C0: _T4D1=fields2;_T4D2=_T4D1->hd;{struct _tuple38*_T67B=(struct _tuple38*)_T4D2;struct Cyc_Absyn_Exp*_T67C;struct Cyc_Absyn_Aggrfield*_T67D;{struct _tuple38 _T67E=*_T67B;_T67D=_T67E.f0;_T67C=_T67E.f1;}{struct Cyc_Absyn_Aggrfield*aggrfield=_T67D;struct Cyc_Absyn_Exp*fieldexp=_T67C;_T4D3=fieldexp;{
void*old_field_typ=_T4D3->topt;
Cyc_Toc_exp_to_c(nv,fieldexp);_T4D4=aggrfield;_T4D5=_T4D4->type;_T4D6=
# 2594
Cyc_Toc_is_void_star_or_boxed_tvar(_T4D5);if(!_T4D6)goto _TL2C3;_T4D7=fieldexp;_T4D8=_T4D7->topt;_T4D9=
_check_null(_T4D8);_T4DA=Cyc_Toc_is_pointer_or_boxed_tvar(_T4D9);
# 2594
if(_T4DA)goto _TL2C3;else{goto _TL2C5;}
# 2596
_TL2C5: _T4DB=aggrfield;_T4DC=_T4DB->type;{void*t=Cyc_Toc_typ_to_c(_T4DC);_T4DD=fieldexp;_T4DE=t;_T4DF=
Cyc_Absyn_copy_exp(fieldexp);_T4E0=Cyc_Toc_cast_it(_T4DE,_T4DF);_T4DD->r=_T4E0->r;_T4E1=fieldexp;
_T4E1->topt=t;}goto _TL2C4;_TL2C3: _TL2C4: _T4E2=sd;_T4E3=_T4E2->impl;_T4E4=
# 2601
_check_null(_T4E3);_T4E5=_T4E4->tagged;if(!_T4E5)goto _TL2C6;_T4E6=aggrfield;{
struct _fat_ptr*n=_T4E6->name;_T4E7=
Cyc_Toc_get_member_offset(sd,n);_T4E8=(unsigned)_T4E7;_T4E9=Cyc_Absyn_uint_exp(_T4E8,0U);_T4EA=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*tag_exp=Cyc_Absyn_set_type(_T4E9,_T4EA);
struct _tuple20*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);
struct _tuple20*val_dle=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);
struct _tuple1*s;s=_cycalloc(sizeof(struct _tuple1));_T4EB=s;_T4EB->f0=Cyc_Absyn_Abs_n(0,1);_T4EC=s;{struct _fat_ptr*_T67E=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T67F;_T67F.tag=0;_T4F0=sd;_T4F1=_T4F0->name;_T4F2=*_T4F1;_T4F3=_T4F2.f1;
# 2608
_T67F.f1=*_T4F3;_T4EF=_T67F;}{struct Cyc_String_pa_PrintArg_struct _T67F=_T4EF;{struct Cyc_String_pa_PrintArg_struct _T680;_T680.tag=0;_T4F5=n;_T680.f1=*_T4F5;_T4F4=_T680;}{struct Cyc_String_pa_PrintArg_struct _T680=_T4F4;void*_T681[2];_T4F6=_T681 + 0;*_T4F6=& _T67F;_T4F7=_T681 + 1;*_T4F7=& _T680;_T4F8=
# 2607
_tag_fat("_union_%s_%s",sizeof(char),13U);_T4F9=_tag_fat(_T681,sizeof(void*),2);_T4EE=Cyc_aprintf(_T4F8,_T4F9);}}*_T67E=_T4EE;_T4ED=(struct _fat_ptr*)_T67E;}_T4EC->f1=_T4ED;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T67E=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T67E->tag=28;
# 2609
_T67E->f1=s;_T67E->f2=0;{struct _tuple20*_T67F[2];_T4FC=_T67F + 0;*_T4FC=tag_dle;_T4FD=_T67F + 1;*_T4FD=val_dle;_T4FE=_tag_fat(_T67F,sizeof(struct _tuple20*),2);_T4FB=Cyc_List_list(_T4FE);}_T67E->f3=_T4FB;_T67E->f4=0;_T4FA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T67E;}_T4FF=(void*)_T4FA;{struct Cyc_Absyn_Exp*aggr_exp=Cyc_Absyn_new_exp(_T4FF,0U);_T500=aggr_exp;_T501=
Cyc_Toc_tagged_union_field_struct(old_typ,n);Cyc_Absyn_set_type(_T500,_T501);{
struct _tuple20*u_dle=Cyc_Toc_make_field(n,aggr_exp);_T502=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T67E=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T67E->tag=28;_T504=tdn;
_T67E->f1=*_T504;_T67E->f2=0;{struct _tuple20*_T67F[1];_T506=_T67F + 0;*_T506=u_dle;_T507=_tag_fat(_T67F,sizeof(struct _tuple20*),1);_T505=Cyc_List_list(_T507);}_T67E->f3=_T505;_T67E->f4=sd;_T503=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T67E;}_T502->r=(void*)_T503;}}}}goto _TL2C7;_TL2C6: _TL2C7: _T508=
# 2618
Cyc_Toc_is_abstract_type(old_typ);if(!_T508)goto _TL2C8;if(last_typ_field!=aggrfield)goto _TL2C8;_T509=sd;_T50A=_T509->impl;_T50B=
_check_null(_T50A);_T50C=_T50B->exist_vars;_T50D=exist_ts;_T50E=Cyc_List_zip(_T50C,_T50D);_T50F=sd;_T510=_T50F->tvs;_T511=param_ts;_T512=
Cyc_List_zip(_T510,_T511);{
# 2619
struct Cyc_List_List*inst=Cyc_List_append(_T50E,_T512);
# 2621
struct Cyc_List_List*new_fields=0;_T513=sd;_T514=_T513->impl;_T515=
_check_null(_T514);{struct Cyc_List_List*fs=_T515->fields;_TL2CD: if(fs!=0)goto _TL2CB;else{goto _TL2CC;}
_TL2CB: _T516=fs;_T517=_T516->hd;{struct Cyc_Absyn_Aggrfield*old_f=(struct Cyc_Absyn_Aggrfield*)_T517;_T518=inst;_T519=old_f;_T51A=_T519->type;{
void*old_ftyp=Cyc_Tcutil_substitute(_T518,_T51A);_T51B=old_f;_T51C=
# 2626
Cyc_Tcutil_substitute(inst,old_ftyp);_T51D=Cyc_Toc_typ_to_c(_T51C);{
# 2625
struct Cyc_Absyn_Aggrfield*new_f=Cyc_Toc_aggrfield_to_c(_T51B,_T51D);{struct Cyc_List_List*_T67E=_cycalloc(sizeof(struct Cyc_List_List));
# 2627
_T67E->hd=new_f;_T67E->tl=new_fields;_T51E=(struct Cyc_List_List*)_T67E;}new_fields=_T51E;_T51F=fs;_T520=_T51F->tl;
# 2633
if(_T520!=0)goto _TL2CE;_T521=new_f;_T522=_T521->type;{
void*_T67E=Cyc_Absyn_compress(_T522);struct Cyc_Absyn_ArrayInfo _T67F;_T523=(int*)_T67E;_T524=*_T523;if(_T524!=5)goto _TL2D0;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T680=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T67E;_T67F=_T680->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T67F;_T525=ai;_T526=_T525.num_elts;_T527=
# 2636
_check_null(_T526);_T528=Cyc_Evexp_c_can_eval(_T527);if(_T528)goto _TL2D2;else{goto _TL2D4;}
_TL2D4:{struct Cyc_Absyn_ArrayInfo ai2=ai;_T529=
Cyc_Absyn_uint_exp(0U,0U);_T52A=Cyc_Absyn_uint_type;ai2.num_elts=Cyc_Absyn_set_type(_T529,_T52A);_T52B=new_f;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T680=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T680->tag=5;
_T680->f1=ai2;_T52C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T680;}_T52B->type=(void*)_T52C;}goto _TL2D3;_TL2D2: _TL2D3: goto _LLFE;}_TL2D0: _T52D=fieldexp;_T52E=_T52D->topt;
# 2645
if(_T52E!=0)goto _TL2D5;goto _LLFE;_TL2D5: _T52F=fieldexp;_T530=_T52F->topt;{
# 2647
void*_T680=Cyc_Absyn_compress(_T530);_T531=(int*)_T680;_T532=*_T531;if(_T532!=0)goto _TL2D7;_T533=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T680;_T534=_T533->f1;_T535=(int*)_T534;_T536=*_T535;if(_T536!=24)goto _TL2D9;_T537=new_f;_T538=fieldexp;_T539=_T538->topt;
# 2649
_T537->type=_check_null(_T539);goto _LL103;_TL2D9: goto _LL106;_TL2D7: _LL106: goto _LL103;_LL103:;}goto _LLFE;_LLFE:;}_T53A=old_f;_T53B=_T53A->type;_T53C=
# 2656
Cyc_Tcutil_is_array_type(_T53B);if(_T53C)goto _TL2DB;else{goto _TL2DD;}_TL2DD: _T53D=& Cyc_Kinds_ak;_T53E=(struct Cyc_Absyn_Kind*)_T53D;_T53F=old_f;_T540=_T53F->type;_T541=
Cyc_Tcutil_type_kind(_T540);_T542=Cyc_Kinds_kind_leq(_T53E,_T541);
# 2656
if(!_T542)goto _TL2DB;_T543=new_f;{struct Cyc_List_List*_T67E=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T67F=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_T67F->tag=6;
# 2658
_T67F->f1=0;_T545=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_T67F;}_T67E->hd=(void*)_T545;_T546=new_f;
_T67E->tl=_T546->attributes;_T544=(struct Cyc_List_List*)_T67E;}
# 2658
_T543->attributes=_T544;goto _TL2DC;_TL2DB: _TL2DC: goto _TL2CF;_TL2CE: _TL2CF:;}}}_T547=fs;
# 2622
fs=_T547->tl;goto _TL2CD;_TL2CC:;}{struct _fat_ptr*_T67E=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _T67F;_T67F.tag=1;_T54B=Cyc_Toc_tuple_type_counter;
# 2663
Cyc_Toc_tuple_type_counter=_T54B + 1;_T54C=_T54B;_T67F.f1=(unsigned long)_T54C;_T54A=_T67F;}{struct Cyc_Int_pa_PrintArg_struct _T67F=_T54A;void*_T680[1];_T54D=_T680 + 0;*_T54D=& _T67F;_T54E=
# 2662
_tag_fat("_genStruct%d",sizeof(char),13U);_T54F=_tag_fat(_T680,sizeof(void*),1);_T549=Cyc_aprintf(_T54E,_T54F);}*_T67E=_T549;_T548=(struct _fat_ptr*)_T67E;}_T550=
# 2664
Cyc_List_imp_rev(new_fields);
# 2662
sd=Cyc_Toc_make_c_struct_defn(_T548,_T550);_T551=tdn;_T552=sd;
# 2665
*_T551=_T552->name;
Cyc_Toc_aggrdecl_to_c(sd);_T553=e;{struct Cyc_Absyn_Aggrdecl**_T67E=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 2668
*_T67E=sd;_T554=(struct Cyc_Absyn_Aggrdecl**)_T67E;}_T555=Cyc_Absyn_KnownAggr(_T554);_T553->topt=Cyc_Absyn_aggr_type(_T555,0);}goto _TL2C9;_TL2C8: _TL2C9:;}}}_T556=fields2;
# 2589
fields2=_T556->tl;goto _TL2C2;_TL2C1:;}goto _LLF6;}_TL2BD: goto _LLF9;_TL2BB: _LLF9:{struct Cyc_Warn_String_Warn_Warg_struct _T67B;_T67B.tag=0;
# 2672
_T67B.f1=_tag_fat("exp_to_c, Aggregate_e: bad struct type",sizeof(char),39U);_T557=_T67B;}{struct Cyc_Warn_String_Warn_Warg_struct _T67B=_T557;void*_T67C[1];_T558=_T67C + 0;*_T558=& _T67B;{int(*_T67D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T559=_T67D;}_T55A=_tag_fat(_T67C,sizeof(void*),1);_T559(_T55A);}_LLF6:;}goto _TL2B9;
# 2680
_TL2B8: _T55B=sd;_T55C=_T55B->impl;_T55D=_check_null(_T55C);_T55E=_T55D->tagged;if(!_T55E)goto _TL2DE;_T55F=
# 2682
_check_null(fields);_T560=_T55F->hd;{struct _tuple38*_T679=(struct _tuple38*)_T560;struct Cyc_Absyn_Exp*_T67A;struct Cyc_Absyn_Aggrfield*_T67B;{struct _tuple38 _T67C=*_T679;_T67B=_T67C.f0;_T67A=_T67C.f1;}{struct Cyc_Absyn_Aggrfield*field=_T67B;struct Cyc_Absyn_Exp*fieldexp=_T67A;_T561=fieldexp;_T562=_T561->topt;{
void*fieldexp_type=_check_null(_T562);_T563=field;{
void*fieldtyp=_T563->type;
Cyc_Toc_exp_to_c(nv,fieldexp);_T564=
Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp);if(!_T564)goto _TL2E0;_T565=
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type);
# 2686
if(_T565)goto _TL2E0;else{goto _TL2E2;}
# 2688
_TL2E2: _T566=fieldexp;_T567=
Cyc_Toc_void_star_type();_T568=Cyc_Absyn_copy_exp(fieldexp);
# 2688
_T566->r=
Cyc_Toc_cast_it_r(_T567,_T568);goto _TL2E1;_TL2E0: _TL2E1: _T569=sd;_T56A=field;_T56B=_T56A->name;{
# 2691
int i=Cyc_Toc_get_member_offset(_T569,_T56B);_T56C=
Cyc_Absyn_signed_int_exp(i,0U);_T56D=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_set_type(_T56C,_T56D);{struct _tuple20*_T67C[2];_T56F=_T67C + 0;{struct _tuple20*_T67D=_cycalloc(sizeof(struct _tuple20));
_T67D->f0=0;_T67D->f1=field_tag_exp;_T570=(struct _tuple20*)_T67D;}*_T56F=_T570;_T571=_T67C + 1;{struct _tuple20*_T67D=_cycalloc(sizeof(struct _tuple20));_T67D->f0=0;_T67D->f1=fieldexp;_T572=(struct _tuple20*)_T67D;}*_T571=_T572;_T573=_tag_fat(_T67C,sizeof(struct _tuple20*),2);_T56E=Cyc_List_list(_T573);}{struct Cyc_List_List*newdles=_T56E;
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);_T574=e;{struct _tuple20*_T67C[1];_T576=_T67C + 0;_T577=field;_T578=_T577->name;_T579=umem;*_T576=
Cyc_Toc_make_field(_T578,_T579);_T57A=_tag_fat(_T67C,sizeof(struct _tuple20*),1);_T575=Cyc_List_list(_T57A);}_T574->r=Cyc_Toc_unresolvedmem_exp_r(0,_T575);}}}}}}}goto _TL2DF;
# 2698
_TL2DE:{struct Cyc_List_List*newdles=0;_T57B=sd;_T57C=_T57B->impl;{
struct Cyc_List_List*sdfields=_T57C->fields;
_TL2E6: if(sdfields!=0)goto _TL2E4;else{goto _TL2E5;}
_TL2E4:{struct Cyc_List_List*fields2=fields;_TL2EA: if(fields2!=0)goto _TL2E8;else{goto _TL2E9;}
_TL2E8: _T57D=fields2;_T57E=_T57D->hd;_T57F=(struct _tuple38*)_T57E;_T580=*_T57F;_T581=_T580.f0;_T582=sdfields;_T583=_T582->hd;_T584=(struct Cyc_Absyn_Aggrfield*)_T583;if(_T581!=_T584)goto _TL2EB;_T585=fields2;_T586=_T585->hd;{
struct _tuple38*_T679=(struct _tuple38*)_T586;struct Cyc_Absyn_Exp*_T67A;struct Cyc_Absyn_Aggrfield*_T67B;{struct _tuple38 _T67C=*_T679;_T67B=_T67C.f0;_T67A=_T67C.f1;}{struct Cyc_Absyn_Aggrfield*field=_T67B;struct Cyc_Absyn_Exp*fieldexp=_T67A;_T587=field;_T588=_T587->type;{
void*fieldtyp=Cyc_Toc_typ_to_c(_T588);_T589=fieldexp;_T58A=_T589->topt;_T58B=
_check_null(_T58A);{void*fieldexp_typ=Cyc_Toc_typ_to_c(_T58B);
Cyc_Toc_exp_to_c(nv,fieldexp);_T58C=
# 2710
Cyc_Unify_unify(fieldtyp,Cyc_Absyn_void_type);if(_T58C)goto _TL2ED;else{goto _TL2EF;}_TL2EF: _T58D=
Cyc_Unify_unify(fieldtyp,fieldexp_typ);
# 2710
if(_T58D)goto _TL2ED;else{goto _TL2F0;}
# 2713
_TL2F0: _T58E=Cyc_Tcutil_is_arithmetic_type(fieldtyp);if(_T58E)goto _TL2F4;else{goto _TL2F3;}_TL2F4: _T58F=
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ);
# 2713
if(_T58F)goto _TL2F1;else{goto _TL2F3;}
# 2715
_TL2F3: _T590=fieldtyp;_T591=Cyc_Absyn_copy_exp(fieldexp);fieldexp=Cyc_Toc_cast_it(_T590,_T591);goto _TL2F2;_TL2F1: _TL2F2: goto _TL2EE;_TL2ED: _TL2EE:{struct Cyc_List_List*_T67C=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T67D=_cycalloc(sizeof(struct _tuple20));
_T67D->f0=0;_T67D->f1=fieldexp;_T593=(struct _tuple20*)_T67D;}_T67C->hd=_T593;_T67C->tl=newdles;_T592=(struct Cyc_List_List*)_T67C;}newdles=_T592;goto _TL2E9;}}}}_TL2EB: _T594=fields2;
# 2701
fields2=_T594->tl;goto _TL2EA;_TL2E9:;}_T595=sdfields;
# 2700
sdfields=_T595->tl;goto _TL2E6;_TL2E5: _T596=e;_T597=
# 2720
Cyc_List_imp_rev(newdles);_T596->r=Cyc_Toc_unresolvedmem_exp_r(0,_T597);}}_TL2DF: _TL2B9: goto _LL7;}}}}}case 30:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T664;_T671=_T679->f1;_T66C=_T679->f2;_T66B=_T679->f3;}{struct Cyc_List_List*es=_T671;struct Cyc_Absyn_Datatypedecl*tud=_T66C;struct Cyc_Absyn_Datatypefield*tuf=_T66B;_T598=tuf;{
# 2727
struct Cyc_List_List*tqts=_T598->typs;{
struct Cyc_List_List*es2=es;_TL2F8: if(es2!=0)goto _TL2F6;else{goto _TL2F7;}
_TL2F6: _T599=es2;_T59A=_T599->hd;{struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_T59A;_T59B=cur_e;_T59C=_T59B->topt;{
void*cur_e_typ=_check_null(_T59C);_T59D=
_check_null(tqts);_T59E=_T59D->hd;_T59F=(struct _tuple24*)_T59E;_T5A0=*_T59F;_T5A1=_T5A0.f1;{void*field_typ=Cyc_Toc_typ_to_c(_T5A1);
Cyc_Toc_exp_to_c(nv,cur_e);_T5A2=
Cyc_Toc_is_void_star_or_boxed_tvar(field_typ);if(!_T5A2)goto _TL2F9;_T5A3=
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ);
# 2733
if(_T5A3)goto _TL2F9;else{goto _TL2FB;}
# 2735
_TL2FB: _T5A4=cur_e;_T5A5=field_typ;_T5A6=Cyc_Absyn_copy_exp(cur_e);_T5A7=Cyc_Toc_cast_it(_T5A5,_T5A6);_T5A4->r=_T5A7->r;goto _TL2FA;_TL2F9: _TL2FA:;}}}_T5A8=es2;
# 2728
es2=_T5A8->tl;_T5A9=tqts;tqts=_T5A9->tl;goto _TL2F8;_TL2F7:;}_T5AB=tud;_T5AC=_T5AB->is_extensible;
# 2739
if(!_T5AC)goto _TL2FC;_T5AD=tuf;_T5AE=_T5AD->name;_T5AF=Cyc_Absyn_var_exp(_T5AE,0U);_T5B0=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_T5AA=Cyc_Absyn_set_type(_T5AF,_T5B0);goto _TL2FD;_TL2FC: _T5B1=tud;_T5B2=tuf;_T5B3=_T5B2->name;_T5AA=Cyc_Toc_datatype_tag(_T5B1,_T5B3);_TL2FD: {
# 2738
struct Cyc_Absyn_Exp*tag_exp=_T5AA;_T5B4=
# 2742
Cyc_Toc_is_toplevel(nv);if(_T5B4)goto _TL2FE;else{goto _TL300;}
# 2744
_TL300:{struct Cyc_List_List*dles=0;{
int i=1;_TL304: if(es!=0)goto _TL302;else{goto _TL303;}
_TL302:{struct Cyc_List_List*_T679=_cycalloc(sizeof(struct Cyc_List_List));_T5B6=Cyc_Absyn_fieldname(i);_T5B7=es;_T5B8=_T5B7->hd;_T5B9=(struct Cyc_Absyn_Exp*)_T5B8;_T679->hd=Cyc_Toc_make_field(_T5B6,_T5B9);_T679->tl=dles;_T5B5=(struct Cyc_List_List*)_T679;}dles=_T5B5;_T5BA=es;
# 2745
es=_T5BA->tl;i=i + 1;goto _TL304;_TL303:;}{
# 2750
struct _tuple20*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);_T5BB=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T679=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T679->tag=28;_T5BD=tuf;_T5BE=_T5BD->name;_T5BF=tud;_T5C0=_T5BF->name;
_T679->f1=Cyc_Toc_collapse_qvars(_T5BE,_T5C0);_T679->f2=0;{struct Cyc_List_List*_T67A=_cycalloc(sizeof(struct Cyc_List_List));
_T67A->hd=tag_dle;_T67A->tl=Cyc_List_imp_rev(dles);_T5C1=(struct Cyc_List_List*)_T67A;}_T679->f3=_T5C1;_T679->f4=0;_T5BC=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T679;}
# 2751
_T5BB->r=(void*)_T5BC;}}goto _TL2FF;
# 2756
_TL2FE:{struct Cyc_List_List*dles=0;
_TL308: if(es!=0)goto _TL306;else{goto _TL307;}
_TL306:{struct Cyc_List_List*_T679=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T67A=_cycalloc(sizeof(struct _tuple20));_T67A->f0=0;_T5C4=es;_T5C5=_T5C4->hd;_T67A->f1=(struct Cyc_Absyn_Exp*)_T5C5;_T5C3=(struct _tuple20*)_T67A;}_T679->hd=_T5C3;_T679->tl=dles;_T5C2=(struct Cyc_List_List*)_T679;}dles=_T5C2;_T5C6=es;
# 2757
es=_T5C6->tl;goto _TL308;_TL307: _T5C7=e;{struct Cyc_List_List*_T679=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T67A=_cycalloc(sizeof(struct _tuple20));
# 2759
_T67A->f0=0;_T67A->f1=tag_exp;_T5C9=(struct _tuple20*)_T67A;}_T679->hd=_T5C9;
_T679->tl=Cyc_List_imp_rev(dles);_T5C8=(struct Cyc_List_List*)_T679;}
# 2759
_T5C7->r=Cyc_Toc_unresolvedmem_exp_r(0,_T5C8);}_TL2FF: goto _LL7;}}}case 31: goto _LL51;case 32: _LL51: goto _LL7;case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T664;_T5CA=_T679->f1;_T66A=_T5CA.mknd;_T5CB=_T679->f1;_T672=_T5CB.rgn;_T5CC=_T679->f1;_T669=_T5CC.aqual;_T5CD=_T679->f1;_T668=_T5CD.elt_type;_T5CE=_T679->f1;_T667=_T5CE.num_elts;_T5CF=_T679->f1;_T677=_T5CF.fat_result;_T5D0=_T679->f1;_T676=_T5D0.inline_call;}{enum Cyc_Absyn_MallocKind mknd=_T66A;struct Cyc_Absyn_Exp*rgnopt=_T672;struct Cyc_Absyn_Exp*aqopt=_T669;void**topt=_T668;struct Cyc_Absyn_Exp*num_elts=_T667;int is_fat=_T677;int inline_call=_T676;_T5D1=
# 2768
_check_null(topt);_T5D2=*_T5D1;{void*t_c=Cyc_Toc_typ_to_c(_T5D2);
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;_T5D3=is_fat;
if(!_T5D3)goto _TL309;
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);goto _TL30A;_TL309: _TL30A: _T5D4=mknd;_T5D5=(int)_T5D4;
# 2777
if(_T5D5!=1)goto _TL30B;
if(rgnopt==0)goto _TL311;else{goto _TL310;}_TL311: if(aqopt==0)goto _TL30F;else{goto _TL310;}_TL310: _T5D6=Cyc_Flags_no_regions;if(_T5D6)goto _TL30F;else{goto _TL30D;}
_TL30F: _T5D7=topt;_T5D8=*_T5D7;_T5D9=Cyc_Absyn_sizeoftype_exp(t_c,0U);_T5DA=Cyc_Absyn_uint_type;_T5DB=Cyc_Absyn_set_type(_T5D9,_T5DA);_T5DC=num_elts2;pexp=Cyc_Toc_calloc_exp(_T5D8,_T5DB,_T5DC);goto _TL30E;
# 2781
_TL30D: if(aqopt==0)goto _TL312;Cyc_Toc_exp_to_c(nv,aqopt);goto _TL313;_TL312: _TL313:
 if(rgnopt==0)goto _TL314;
Cyc_Toc_exp_to_c(nv,rgnopt);_T5DD=rgnopt;
if(aqopt!=0)goto _TL316;_T5DE=Cyc_Toc_aliasable_qual_exp();goto _TL317;_TL316: _T5DE=aqopt;_TL317: _T5DF=
Cyc_Absyn_sizeoftype_exp(t_c,0U);_T5E0=Cyc_Absyn_uint_type;_T5E1=Cyc_Absyn_set_type(_T5DF,_T5E0);_T5E2=num_elts2;
# 2784
pexp=Cyc_Toc_rcalloc_exp(_T5DD,_T5DE,_T5E1,_T5E2);goto _TL315;
# 2788
_TL314: _T5E3=aqopt;_T5E4=Cyc_Absyn_sizeoftype_exp(t_c,0U);_T5E5=Cyc_Absyn_uint_type;_T5E6=Cyc_Absyn_set_type(_T5E4,_T5E5);_T5E7=num_elts2;pexp=Cyc_Toc_aqcalloc_exp(_T5E3,_T5E6,_T5E7);_TL315: _TL30E: goto _TL30C;
# 2792
_TL30B: if(rgnopt==0)goto _TL31C;else{goto _TL31B;}_TL31C: if(aqopt==0)goto _TL31A;else{goto _TL31B;}_TL31B: _T5E8=Cyc_Flags_no_regions;if(_T5E8)goto _TL31A;else{goto _TL318;}
_TL31A: _T5E9=topt;_T5EA=*_T5E9;_T5EB=num_elts2;pexp=Cyc_Toc_malloc_exp(_T5EA,_T5EB);goto _TL319;
# 2795
_TL318: if(aqopt==0)goto _TL31D;Cyc_Toc_exp_to_c(nv,aqopt);goto _TL31E;_TL31D: _TL31E:
 if(rgnopt==0)goto _TL31F;
Cyc_Toc_exp_to_c(nv,rgnopt);_T5EC=mknd;_T5ED=(int)_T5EC;
if(_T5ED!=2)goto _TL321;
pexp=Cyc_Toc_rvmalloc_exp(rgnopt,num_elts2);goto _TL322;
# 2801
_TL321: _T5EF=inline_call;if(!_T5EF)goto _TL323;_T5EE=Cyc_Toc_rmalloc_inline_exp;goto _TL324;_TL323: _T5EE=Cyc_Toc_rmalloc_exp;_TL324:{struct Cyc_Absyn_Exp*(*fn_e)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=_T5EE;_T5F0=rgnopt;
if(aqopt!=0)goto _TL325;_T5F1=Cyc_Toc_aliasable_qual_exp();goto _TL326;_TL325: _T5F1=aqopt;_TL326: _T5F2=num_elts2;pexp=fn_e(_T5F0,_T5F1,_T5F2);}_TL322: goto _TL320;
# 2806
_TL31F: pexp=Cyc_Toc_aqmalloc_exp(aqopt,num_elts2);_TL320: _TL319: _TL30C: _T5F3=is_fat;
# 2809
if(!_T5F3)goto _TL327;_T5F5=mknd;_T5F6=(int)_T5F5;
if(_T5F6!=1)goto _TL329;_T5F7=Cyc_Absyn_sizeoftype_exp(t_c,0U);_T5F8=Cyc_Absyn_uint_type;_T5F4=Cyc_Absyn_set_type(_T5F7,_T5F8);goto _TL32A;_TL329: _T5F9=Cyc_Absyn_uint_exp(1U,0U);_T5FA=Cyc_Absyn_uint_type;_T5F4=Cyc_Absyn_set_type(_T5F9,_T5FA);_TL32A:{struct Cyc_Absyn_Exp*elt_sz=_T5F4;{struct Cyc_Absyn_Exp*_T679[3];_T5FC=_T679 + 0;
*_T5FC=pexp;_T5FD=_T679 + 1;*_T5FD=elt_sz;_T5FE=_T679 + 2;*_T5FE=num_elts2;_T5FF=Cyc_Toc__tag_fat_e;_T600=_tag_fat(_T679,sizeof(struct Cyc_Absyn_Exp*),3);_T5FB=Cyc_Toc_fncall_exp_dl(_T5FF,_T600);}{struct Cyc_Absyn_Exp*rexp=_T5FB;_T601=rexp;
_T601->topt=Cyc_Toc_fat_ptr_type();_T602=x;_T603=Cyc_Absyn_uint_type;_T604=num_elts;_T605=
# 2814
Cyc_Absyn_exp_stmt(rexp,0U);{
# 2813
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T602,_T603,_T604,_T605,0U);_T606=e;
# 2815
_T606->r=Cyc_Toc_stmt_exp_r(s);}}}goto _TL328;
# 2817
_TL327: _T607=e;_T608=Cyc_Toc_typ_to_c(old_typ);_T609=pexp;_T607->r=Cyc_Toc_cast_it_r(_T608,_T609);_TL328: goto _LL7;}}}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T664;_T672=_T679->f1;_T669=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T672;struct Cyc_Absyn_Exp*e2=_T669;_T60A=e1;_T60B=_T60A->topt;{
# 2832
void*e1_old_typ=_check_null(_T60B);_T60C=e2;_T60D=_T60C->topt;{
void*e2_old_typ=_check_null(_T60D);_T60E=
Cyc_Tcutil_is_boxed(e1_old_typ);if(_T60E)goto _TL32B;else{goto _TL32D;}_TL32D: _T60F=Cyc_Tcutil_is_pointer_type(e1_old_typ);if(_T60F)goto _TL32B;else{goto _TL32E;}
_TL32E:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;_T679.f1=_tag_fat("Swap_e: is_pointer_or_boxed: not a pointer or boxed type",sizeof(char),57U);_T610=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T610;void*_T67A[1];_T611=_T67A + 0;*_T611=& _T679;{int(*_T67B)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T612=_T67B;}_T613=_tag_fat(_T67A,sizeof(void*),1);_T612(_T613);}goto _TL32C;_TL32B: _TL32C: _T614=e;{
# 2840
unsigned loc=_T614->loc;
struct _tuple1*v1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v1e=Cyc_Absyn_var_exp(v1,loc);_T615=v1e;_T615->topt=e1_old_typ;{
struct _tuple1*v2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v2e=Cyc_Absyn_var_exp(v2,loc);_T616=v2e;_T616->topt=e2_old_typ;_T617=
# 2846
Cyc_Tcutil_deep_copy_exp(1,e1);_T618=v2e;_T619=loc;{struct Cyc_Absyn_Exp*s1e=Cyc_Absyn_assign_exp(_T617,_T618,_T619);_T61A=s1e;_T61A->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_exp_stmt(s1e,loc);_T61B=
Cyc_Tcutil_deep_copy_exp(1,e2);_T61C=v1e;_T61D=loc;{struct Cyc_Absyn_Exp*s2e=Cyc_Absyn_assign_exp(_T61B,_T61C,_T61D);_T61E=s2e;_T61E->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_exp_stmt(s2e,loc);_T61F=v1;_T620=e1_old_typ;_T621=e1;_T622=v2;_T623=e2_old_typ;_T624=e2;_T625=
# 2853
Cyc_Absyn_seq_stmt(s1,s2,loc);_T626=loc;_T627=
# 2852
Cyc_Absyn_declare_stmt(_T622,_T623,_T624,_T625,_T626);_T628=loc;{
# 2851
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T61F,_T620,_T621,_T627,_T628);
# 2854
Cyc_Toc_stmt_to_c(nv,s);_T629=e;
_T629->r=Cyc_Toc_stmt_exp_r(s);goto _LL7;}}}}}}}}}}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T664;_T672=_T679->f1;_T666=_T679->f2;}{struct Cyc_Absyn_Exp*e1=_T672;struct _fat_ptr*f=_T666;_T62A=e1;_T62B=_T62A->topt;_T62C=
# 2859
_check_null(_T62B);{void*e1_typ=Cyc_Absyn_compress(_T62C);
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Absyn_Aggrdecl*_T679;_T62D=e1_typ;_T62E=(int*)_T62D;_T62F=*_T62E;if(_T62F!=0)goto _TL32F;_T630=e1_typ;_T631=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T630;_T632=_T631->f1;_T633=(int*)_T632;_T634=*_T633;if(_T634!=24)goto _TL331;_T635=e1_typ;_T636=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T635;_T637=_T636->f1;_T638=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T637;_T639=_T638->f1;_T63A=_T639.KnownAggr;_T63B=_T63A.tag;if(_T63B!=2)goto _TL333;_T63C=e1_typ;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T67A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T63C;_T63D=_T67A->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T67B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T63D;_T63E=_T67B->f1;_T63F=_T63E.KnownAggr;_T640=_T63F.val;{struct Cyc_Absyn_Aggrdecl*_T67C=*_T640;_T679=_T67C;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T679;_T641=
# 2863
Cyc_Toc_get_member_offset(ad,f);_T642=Cyc_Absyn_signed_int_exp(_T641,0U);_T643=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_set_type(_T642,_T643);_T644=
Cyc_Toc_member_exp(e1,f,0U);_T645=Cyc_Toc_tagged_union_field_struct(e1_typ,f);{struct Cyc_Absyn_Exp*e1_f=Cyc_Absyn_set_type(_T644,_T645);_T646=
Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);_T647=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*e1_f_tag=Cyc_Absyn_set_type(_T646,_T647);_T648=e;_T649=
Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U);_T648->r=_T649->r;goto _LL10E;}}}}_TL333: goto _LL111;_TL331: goto _LL111;_TL32F: _LL111:{struct Cyc_Warn_String_Warn_Warg_struct _T67A;_T67A.tag=0;
# 2868
_T67A.f1=_tag_fat("non-aggregate type in tagcheck",sizeof(char),31U);_T64A=_T67A;}{struct Cyc_Warn_String_Warn_Warg_struct _T67A=_T64A;void*_T67B[1];_T64B=_T67B + 0;*_T64B=& _T67A;{int(*_T67C)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T64C=_T67C;}_T64D=_tag_fat(_T67B,sizeof(void*),1);_T64C(_T64D);}_LL10E:;}goto _LL7;}}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T664;_T665=_T679->f1;}{struct Cyc_Absyn_Stmt*s=_T665;
# 2871
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 35:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;
_T679.f1=_tag_fat("UnresolvedMem",sizeof(char),14U);_T64E=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T64E;void*_T67A[1];_T64F=_T67A + 0;*_T64F=& _T679;{int(*_T67B)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T650=_T67B;}_T651=_tag_fat(_T67A,sizeof(void*),1);_T650(_T651);}case 24:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;
_T679.f1=_tag_fat("compoundlit",sizeof(char),12U);_T652=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T652;void*_T67A[1];_T653=_T67A + 0;*_T653=& _T679;{int(*_T67B)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T654=_T67B;}_T655=_tag_fat(_T67A,sizeof(void*),1);_T654(_T655);}case 38:{struct Cyc_Warn_String_Warn_Warg_struct _T679;_T679.tag=0;
_T679.f1=_tag_fat("valueof(-)",sizeof(char),11U);_T656=_T679;}{struct Cyc_Warn_String_Warn_Warg_struct _T679=_T656;void*_T67A[1];_T657=_T67A + 0;*_T657=& _T679;{int(*_T67B)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T658=_T67B;}_T659=_tag_fat(_T67A,sizeof(void*),1);_T658(_T659);}default:{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T679=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T664;_T671=_T679->f3;_T66E=_T679->f4;}{struct Cyc_List_List*o=_T671;struct Cyc_List_List*i=_T66E;
# 2876
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);goto _LL7;}}_LL7:;}_T65A=did_inserted_checks;
# 2880
if(_T65A)goto _TL335;else{goto _TL337;}
_TL337:{struct Cyc_Warn_String_Warn_Warg_struct _T664;_T664.tag=0;_T664.f1=_tag_fat("Toc did not examine an inserted check: ",sizeof(char),40U);_T65B=_T664;}{struct Cyc_Warn_String_Warn_Warg_struct _T664=_T65B;{struct Cyc_Warn_Exp_Warn_Warg_struct _T665;_T665.tag=4;_T665.f1=e;_T65C=_T665;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T665=_T65C;void*_T666[2];_T65D=_T666 + 0;*_T65D=& _T664;_T65E=_T666 + 1;*_T65E=& _T665;_T660=Cyc_Warn_impos_loc2;{int(*_T667)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T660;_T65F=_T667;}_T661=e;_T662=_T661->loc;_T663=_tag_fat(_T666,sizeof(void*),2);_T65F(_T662,_T663);}}goto _TL336;_TL335: _TL336:;}}}struct _tuple39{int f0;struct _fat_ptr*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 2909 "toc.cyc"
static struct _tuple39*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple39*_T0;struct _RegionHandle*_T1;_T1=r;{struct _tuple39*_T2=_region_malloc(_T1,0U,sizeof(struct _tuple39));
# 2911
_T2->f0=0;_T2->f1=Cyc_Toc_fresh_label();_T2->f2=Cyc_Toc_fresh_label();_T2->f3=sc;_T0=(struct _tuple39*)_T2;}return _T0;}
# 2917
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Tcpat_PathNode*_T2;union Cyc_Tcpat_PatOrWhere _T3;struct _union_PatOrWhere_pattern _T4;unsigned _T5;unsigned _T6;int _T7;struct Cyc_Absyn_Pat*_T8;struct Cyc_Tcpat_PathNode*_T9;struct _union_PatOrWhere_pattern _TA;unsigned _TB;struct _union_PatOrWhere_pattern _TC;void*_TD;void*_TE;void*_TF;int*_T10;unsigned _T11;void*_T12;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T13;void*_T14;int*_T15;unsigned _T16;struct Cyc_Tcpat_PathNode*_T17;int*_T18;unsigned _T19;void*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Tcpat_PathNode*_T20;int*_T21;int _T22;void*_T23;struct Cyc_List_List*_T24;struct Cyc_Absyn_Datatypefield*_T25;struct _tuple1*_T26;struct Cyc_Absyn_Datatypedecl*_T27;struct _tuple1*_T28;struct _tuple1*_T29;void*_T2A;struct Cyc_Absyn_Tqual _T2B;void*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Absyn_Exp*_T2E;unsigned _T2F;int _T30;struct _fat_ptr*_T31;struct Cyc_Absyn_Exp*_T32;void*_T33;struct Cyc_Absyn_Exp*_T34;void*_T35;void*_T36;struct Cyc_Absyn_Exp*_T37;unsigned _T38;int _T39;struct _fat_ptr*_T3A;struct Cyc_Absyn_Exp*_T3B;void*_T3C;void*_T3D;void*_T3E;int _T3F;struct Cyc_Absyn_Exp*_T40;void*_T41;struct Cyc_Absyn_Exp*_T42;void*_T43;struct Cyc_List_List*_T44;struct Cyc_Absyn_Exp*_T45;
_TL33B: if(ps!=0)goto _TL339;else{goto _TL33A;}
_TL339: _T0=ps;_T1=_T0->hd;{struct Cyc_Tcpat_PathNode*p=(struct Cyc_Tcpat_PathNode*)_T1;_T2=p;_T3=_T2->orig_pat;_T4=_T3.pattern;_T5=_T4.tag;_T6=_T5==1;_T7=(int)_T6;
# 2923
if(!_T7)goto _TL33C;_T9=p;{union Cyc_Tcpat_PatOrWhere _T46=_T9->orig_pat;_TA=_T46.pattern;_TB=_TA.tag;if(_TB==1)goto _TL33E;_throw_match();goto _TL33F;_TL33E: _TL33F: _TC=_T46.pattern;_T8=_TC.val;}_TD=_T8->topt;{
void*t=_check_null(_TD);_TE=
Cyc_Toc_typ_to_c_array(t);{void*t2=Cyc_Absyn_compress(_TE);_TF=t2;_T10=(int*)_TF;_T11=*_T10;switch(_T11){case 0: _T12=t2;_T13=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12;_T14=_T13->f1;_T15=(int*)_T14;_T16=*_T15;switch(_T16){case 0: goto _LL4;case 24: _LL4: goto _LL6;default: goto _LL7;};case 7: _LL6: goto _LL0;default: _LL7:
# 2931
 v=Cyc_Toc_cast_it(t2,v);goto _LL0;}_LL0:;}}goto _TL33D;_TL33C: _TL33D: _T17=p;{
# 2934
void*_T46=_T17->access;void*_T47;struct _fat_ptr*_T48;int _T49;unsigned _T4A;void*_T4B;_T18=(int*)_T46;_T19=*_T18;switch(_T19){case 0: goto _LL9;case 1:{struct Cyc_Tcpat_Deref_Tcpat_Access_struct*_T4C=(struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_T46;_T1A=_T4C->f1;_T4B=(void*)_T1A;}{void*res_type=_T4B;_T1B=ps;_T1C=_T1B->tl;
# 2943
if(_T1C==0)goto _TL343;_T1D=ps;_T1E=_T1D->tl;_T1F=_T1E->hd;_T20=(struct Cyc_Tcpat_PathNode*)_T1F;{
void*_T4C=_T20->access;void*_T4D;unsigned _T4E;struct Cyc_Absyn_Datatypefield*_T4F;struct Cyc_Absyn_Datatypedecl*_T50;_T21=(int*)_T4C;_T22=*_T21;if(_T22!=2)goto _TL345;{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T51=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T4C;_T50=_T51->f1;_T4F=_T51->f2;_T4E=_T51->f3;_T23=_T51->f4;_T4D=(void*)_T23;}{struct Cyc_Absyn_Datatypedecl*tud=_T50;struct Cyc_Absyn_Datatypefield*tuf=_T4F;unsigned i=_T4E;void*res_type2=_T4D;_T24=ps;
# 2946
ps=_T24->tl;_T25=tuf;_T26=_T25->name;_T27=tud;_T28=_T27->name;_T29=
Cyc_Toc_collapse_qvars(_T26,_T28);_T2A=Cyc_Absyn_strctq(_T29);_T2B=Cyc_Toc_mt_tq;_T2C=Cyc_Absyn_cstar_type(_T2A,_T2B);_T2D=v;v=Cyc_Toc_cast_it(_T2C,_T2D);_T2E=v;_T2F=i + 1U;_T30=(int)_T2F;_T31=
Cyc_Absyn_fieldname(_T30);_T32=Cyc_Absyn_aggrarrow_exp(_T2E,_T31,0U);_T33=Cyc_Toc_typ_to_c_array(res_type2);v=Cyc_Absyn_set_type(_T32,_T33);goto _TL338;}_TL345: goto _LL12;_LL12:;}goto _TL344;_TL343: _TL344: _T34=
# 2952
Cyc_Absyn_deref_exp(v,0U);_T35=Cyc_Toc_typ_to_c_array(res_type);v=Cyc_Absyn_set_type(_T34,_T35);goto _LL9;}case 2:{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T4C=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T46;_T4A=_T4C->f3;_T36=_T4C->f4;_T4B=(void*)_T36;}{unsigned i=_T4A;void*res_type=_T4B;_T37=v;_T38=i + 1U;_T39=(int)_T38;_T3A=
# 2955
Cyc_Absyn_fieldname(_T39);_T3B=Cyc_Toc_member_exp(_T37,_T3A,0U);_T3C=Cyc_Toc_typ_to_c_array(res_type);v=Cyc_Absyn_set_type(_T3B,_T3C);goto _LL9;}default:{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T4C=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T46;_T3D=_T4C->f1;_T4B=(void*)_T3D;_T49=_T4C->f2;_T48=_T4C->f3;_T3E=_T4C->f4;_T47=(void*)_T3E;}{void*at=_T4B;int tagged=_T49;struct _fat_ptr*f=_T48;void*res_type=_T47;
# 2958
v=Cyc_Toc_member_exp(v,f,0U);_T3F=tagged;
if(!_T3F)goto _TL347;_T40=v;_T41=
# 2961
Cyc_Toc_tagged_union_field_struct(at,f);Cyc_Absyn_set_type(_T40,_T41);
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);goto _TL348;_TL347: _TL348: _T42=v;_T43=
# 2964
Cyc_Toc_typ_to_c_array(res_type);v=Cyc_Absyn_set_type(_T42,_T43);goto _LL9;}}_LL9:;}}_TL338: _T44=ps;
# 2918
ps=_T44->tl;goto _TL33B;_TL33A: _T45=v;
# 2968
return _T45;}
# 2973
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;struct Cyc_Absyn_Exp*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Exp*_T9;void*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Absyn_Exp*_TD;void*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Exp*_T10;void*_T11;struct Cyc_Absyn_Exp*_T12;void*_T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T15;void*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Enumdecl*_T18;struct _tuple1*_T19;struct Cyc_Absyn_Enumdecl*_T1A;void*_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;struct Cyc_Absyn_Exp*_T1F;void*_T20;void*_T21;struct Cyc_Absyn_Exp*_T22;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T23;void*_T24;struct Cyc_Absyn_Exp*_T25;void*_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_Absyn_Exp*_T28;void*_T29;struct Cyc_Absyn_Exp*_T2A;void*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Exp*_T2D;int _T2E;unsigned _T2F;void*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_Absyn_Exp*_T32;void*_T33;struct Cyc_Absyn_Exp*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;unsigned _T37;int _T38;struct Cyc_Absyn_Exp*_T39;void*_T3A;struct Cyc_Absyn_Exp*_T3B;struct Cyc_Absyn_Exp*_T3C;void*_T3D;struct Cyc_Absyn_Exp*_T3E;void*_T3F;struct Cyc_Absyn_Exp*_T40;int*_T41;unsigned _T42;void*_T43;struct Cyc_Absyn_Exp*_T44;struct Cyc_Absyn_Exp*_T45;void*_T46;struct Cyc_Absyn_Exp*_T47;struct Cyc_Absyn_Exp*_T48;void*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_Absyn_Exp*_T4B;void*_T4C;struct Cyc_Absyn_Exp*_T4D;void*_T4E;void*_T4F;struct Cyc_Absyn_Exp*_T50;void*_T51;struct Cyc_Absyn_Exp*_T52;struct Cyc_Absyn_Exp*_T53;void*_T54;struct Cyc_Absyn_Exp*_T55;struct Cyc_Absyn_Exp*_T56;void*_T57;struct Cyc_Absyn_Exp*_T58;void*_T59;struct Cyc_Absyn_Exp*_T5A;int*_T5B;unsigned _T5C;struct Cyc_Absyn_Datatypefield*_T5D;struct _tuple1*_T5E;struct Cyc_Absyn_Datatypedecl*_T5F;struct _tuple1*_T60;struct _tuple1*_T61;void*_T62;struct Cyc_Absyn_Tqual _T63;void*_T64;struct Cyc_Absyn_Exp*_T65;struct Cyc_Absyn_Exp*_T66;void*_T67;struct Cyc_Absyn_Exp*_T68;struct Cyc_Absyn_Datatypefield*_T69;struct _tuple1*_T6A;struct Cyc_Absyn_Exp*_T6B;void*_T6C;struct Cyc_Absyn_Exp*_T6D;struct Cyc_Absyn_Exp*_T6E;struct Cyc_Absyn_Datatypedecl*_T6F;struct _fat_ptr*_T70;struct Cyc_Absyn_Datatypefield*_T71;unsigned _T72;int _T73;struct _fat_ptr _T74;struct Cyc_Absyn_Enumfield*_T75;void*_T76;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T3;_T76=_T77->f1;}{struct Cyc_Absyn_Exp*e=_T76;
# 2975
if(e!=0)goto _TL34A;_T4=v;goto _TL34B;_TL34A: _T4=e;_TL34B: return _T4;}case 1: _T5=v;_T6=
# 2977
Cyc_Absyn_signed_int_exp(0,0U);_T7=Cyc_Toc_void_star_type();_T8=Cyc_Absyn_set_type(_T6,_T7);_T9=Cyc_Absyn_eq_exp(_T5,_T8,0U);_TA=Cyc_Absyn_sint_type;_TB=Cyc_Absyn_set_type(_T9,_TA);return _TB;case 2: _TC=v;_TD=
# 2979
Cyc_Absyn_signed_int_exp(0,0U);_TE=Cyc_Toc_void_star_type();_TF=Cyc_Absyn_set_type(_TD,_TE);_T10=Cyc_Absyn_neq_exp(_TC,_TF,0U);_T11=Cyc_Absyn_sint_type;_T12=Cyc_Absyn_set_type(_T10,_T11);return _T12;case 3: _T13=t;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T13;_T76=_T77->f1;_T75=_T77->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T76;struct Cyc_Absyn_Enumfield*ef=_T75;_T14=v;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T77=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_T77->tag=31;
# 2981
_T77->f1=ed;_T77->f2=ef;_T15=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T77;}_T16=(void*)_T15;_T17=Cyc_Absyn_new_exp(_T16,0U);_T18=ed;_T19=_T18->name;_T1A=ed;_T1B=
Cyc_Absyn_enum_type(_T19,_T1A);_T1C=
# 2981
Cyc_Absyn_set_type(_T17,_T1B);_T1D=Cyc_Absyn_eq_exp(_T14,_T1C,0U);_T1E=Cyc_Absyn_sint_type;_T1F=Cyc_Absyn_set_type(_T1D,_T1E);return _T1F;}case 4: _T20=t;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T20;_T21=_T77->f1;_T76=(void*)_T21;_T75=_T77->f2;}{void*t=_T76;struct Cyc_Absyn_Enumfield*ef=_T75;_T22=v;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T77=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_T77->tag=32;
# 2985
_T77->f1=t;_T77->f2=ef;_T23=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T77;}_T24=(void*)_T23;_T25=Cyc_Absyn_new_exp(_T24,0U);_T26=Cyc_Absyn_sint_type;_T27=Cyc_Absyn_set_type(_T25,_T26);_T28=Cyc_Absyn_eq_exp(_T22,_T27,0U);_T29=Cyc_Absyn_sint_type;_T2A=Cyc_Absyn_set_type(_T28,_T29);return _T2A;}case 5: _T2B=t;{struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_T2B;_T74=_T77->f1;_T73=_T77->f2;}{struct _fat_ptr s=_T74;int i=_T73;_T2C=v;_T2D=
# 2988
Cyc_Absyn_float_exp(s,i,0U);_T2E=i;_T2F=(unsigned)_T2E;_T30=Cyc_Absyn_gen_float_type(_T2F);_T31=Cyc_Absyn_set_type(_T2D,_T30);_T32=Cyc_Absyn_eq_exp(_T2C,_T31,0U);_T33=Cyc_Absyn_sint_type;_T34=Cyc_Absyn_set_type(_T32,_T33);return _T34;}case 6: _T35=t;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T35;_T72=_T77->f1;}{unsigned i=_T72;_T36=v;_T37=i;_T38=(int)_T37;_T39=
# 2991
Cyc_Absyn_signed_int_exp(_T38,0U);_T3A=Cyc_Absyn_sint_type;_T3B=Cyc_Absyn_set_type(_T39,_T3A);_T3C=Cyc_Absyn_eq_exp(_T36,_T3B,0U);_T3D=Cyc_Absyn_sint_type;_T3E=Cyc_Absyn_set_type(_T3C,_T3D);return _T3E;}case 7: _T3F=t;{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_T3F;_T73=_T77->f1;_T76=_T77->f2;_T71=_T77->f3;}{int i=_T73;struct Cyc_Absyn_Datatypedecl*tud=_T76;struct Cyc_Absyn_Datatypefield*tuf=_T71;
# 2995
LOOP1: _T40=v;{
void*_T77=_T40->r;struct Cyc_Absyn_Exp*_T78;_T41=(int*)_T77;_T42=*_T41;switch(_T42){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T79=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T77;_T78=_T79->f2;}{struct Cyc_Absyn_Exp*e=_T78;
v=e;goto LOOP1;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T79=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T77;_T78=_T79->f1;}{struct Cyc_Absyn_Exp*e=_T78;
v=e;goto _LL15;}default: goto _LL15;}_LL15:;}_T43=
# 3005
Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);_T44=v;v=Cyc_Toc_cast_it(_T43,_T44);_T45=
Cyc_Absyn_deref_exp(v,0U);_T46=Cyc_Absyn_sint_type;_T47=Cyc_Absyn_set_type(_T45,_T46);_T48=
Cyc_Absyn_signed_int_exp(i,0U);_T49=Cyc_Absyn_sint_type;_T4A=Cyc_Absyn_set_type(_T48,_T49);_T4B=
# 3006
Cyc_Absyn_eq_exp(_T47,_T4A,0U);_T4C=Cyc_Absyn_sint_type;_T4D=Cyc_Absyn_set_type(_T4B,_T4C);return _T4D;}case 8: _T4E=t;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T4E;_T4F=_T77->f1;_T76=(void*)_T4F;_T70=_T77->f2;_T73=_T77->f3;}{void*atype=_T76;struct _fat_ptr*f=_T70;int i=_T73;
# 3009
struct Cyc_Absyn_Exp*e0=Cyc_Toc_member_exp(v,f,0U);
void*t=Cyc_Toc_tagged_union_field_struct(atype,f);
Cyc_Absyn_set_type(e0,t);_T50=
Cyc_Toc_member_exp(e0,Cyc_Toc_tag_sp,0U);_T51=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*e=Cyc_Absyn_set_type(_T50,_T51);_T52=e;_T53=
Cyc_Absyn_signed_int_exp(i,0U);_T54=Cyc_Absyn_sint_type;_T55=Cyc_Absyn_set_type(_T53,_T54);_T56=Cyc_Absyn_eq_exp(_T52,_T55,0U);_T57=Cyc_Absyn_sint_type;_T58=Cyc_Absyn_set_type(_T56,_T57);return _T58;}}default: _T59=t;{struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T77=(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_T59;_T6F=_T77->f1;_T71=_T77->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T6F;struct Cyc_Absyn_Datatypefield*tuf=_T71;
# 3016
LOOP2: _T5A=v;{
void*_T77=_T5A->r;struct Cyc_Absyn_Exp*_T78;_T5B=(int*)_T77;_T5C=*_T5B;switch(_T5C){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T79=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T77;_T78=_T79->f2;}{struct Cyc_Absyn_Exp*e=_T78;
v=e;goto LOOP2;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T79=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T77;_T78=_T79->f1;}{struct Cyc_Absyn_Exp*e=_T78;
v=e;goto _LL1C;}default: goto _LL1C;}_LL1C:;}_T5D=tuf;_T5E=_T5D->name;_T5F=tud;_T60=_T5F->name;_T61=
# 3023
Cyc_Toc_collapse_qvars(_T5E,_T60);_T62=Cyc_Absyn_strctq(_T61);_T63=Cyc_Toc_mt_tq;_T64=Cyc_Absyn_cstar_type(_T62,_T63);_T65=v;v=Cyc_Toc_cast_it(_T64,_T65);_T66=
Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);_T67=
Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_T68=
# 3024
Cyc_Absyn_set_type(_T66,_T67);_T69=tuf;_T6A=_T69->name;_T6B=
# 3026
Cyc_Absyn_var_exp(_T6A,0U);_T6C=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_T6D=Cyc_Absyn_set_type(_T6B,_T6C);_T6E=
# 3024
Cyc_Absyn_eq_exp(_T68,_T6D,0U);return _T6E;}};}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;void*f1;struct _fat_ptr*f2;};
# 3038
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple40{int f0;void*f1;};
# 3045
static struct _tuple40 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){struct Cyc_Toc_OtherTest_Toc_TestKind_struct*_T0;struct Cyc_Toc_OtherTest_Toc_TestKind_struct*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple36*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;struct Cyc_Toc_OtherTest_Toc_TestKind_struct*_TB;struct Cyc_Toc_OtherTest_Toc_TestKind_struct*_TC;void*_TD;struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_TE;struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct*_TF;struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct*_T10;void*_T11;struct Cyc_Absyn_Exp*_T12;struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_T13;struct _tuple40 _T14;struct _tuple40 _T15;struct Cyc_List_List*_T16;struct _tuple40 _T17;
# 3047
int c=0;_T0=& Cyc_Toc_OtherTest_val;_T1=(struct Cyc_Toc_OtherTest_Toc_TestKind_struct*)_T0;{
void*k=(void*)_T1;
_TL351: if(ss!=0)goto _TL34F;else{goto _TL350;}
_TL34F: _T2=ss;_T3=_T2->hd;_T4=(struct _tuple36*)_T3;{struct _tuple36 _T18=*_T4;void*_T19;_T19=_T18.f0;{void*ptest=_T19;struct Cyc_Absyn_Exp*_T1A;struct _fat_ptr*_T1B;void*_T1C;_T5=ptest;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 3: goto _LL7;case 4: _LL7: goto _LL9;case 6: _LL9: goto _TL34E;case 8: _T8=ptest;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T1D=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T8;_T9=_T1D->f1;_T1C=(void*)_T9;_T1B=_T1D->f2;}{void*t=_T1C;struct _fat_ptr*f=_T1B;_TA=k;_TB=& Cyc_Toc_OtherTest_val;_TC=(struct Cyc_Toc_OtherTest_Toc_TestKind_struct*)_TB;_TD=(void*)_TC;
# 3057
if(_TA!=_TD)goto _TL353;{struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_T1D=_cycalloc(sizeof(struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct));_T1D->tag=3;
_T1D->f1=t;_T1D->f2=f;_TE=(struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_T1D;}k=(void*)_TE;goto _TL354;_TL353: _TL354: goto _TL34E;}case 7: _TF=& Cyc_Toc_DatatypeTagTest_val;_T10=(struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct*)_TF;
# 3060
k=(void*)_T10;goto _TL34E;case 0: _T11=ptest;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T1D=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T11;_T1A=_T1D->f1;}_T12=(struct Cyc_Absyn_Exp*)_T1A;if(_T12==0)goto _TL355;{struct Cyc_Absyn_Exp*e=_T1A;{struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_T1D=_cycalloc(sizeof(struct Cyc_Toc_WhereTest_Toc_TestKind_struct));_T1D->tag=2;
# 3062
_T1D->f1=e;_T13=(struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_T1D;}k=(void*)_T13;{struct _tuple40 _T1D;_T1D.f0=0;_T1D.f1=k;_T14=_T1D;}return _T14;}_TL355: goto _LL13;case 1: _LL13: goto _LL15;case 2: _LL15: goto _LL17;case 5: _LL17: goto _LL19;default: _LL19:{struct _tuple40 _T1D;
# 3067
_T1D.f0=0;_T1D.f1=k;_T15=_T1D;}return _T15;};}}_TL34E: _T16=ss;
# 3049
ss=_T16->tl;c=c + 1;goto _TL351;_TL350:{struct _tuple40 _T18;
# 3070
_T18.f0=c;_T18.f1=k;_T17=_T18;}return _T17;}}
# 3075
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T4;void*_T5;void*_T6;void*_T7;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T8;void*_T9;void*_TA;unsigned _TB;void*_TC;void*_TD;struct Cyc_Absyn_Exp*_TE;void*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;int(*_T12)(struct _fat_ptr ap);struct _fat_ptr _T13;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T14;void*_T15;struct Cyc_Absyn_Pat*_T16;
struct Cyc_Absyn_Exp*e;{int _T17;unsigned _T18;struct Cyc_Absyn_Enumfield*_T19;void*_T1A;_T0=p;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 3: _T3=p;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T1B=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T3;_T1A=_T1B->f1;_T19=_T1B->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T1A;struct Cyc_Absyn_Enumfield*ef=_T19;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T1B=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_T1B->tag=31;
# 3078
_T1B->f1=ed;_T1B->f2=ef;_T4=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T1B;}_T5=(void*)_T4;e=Cyc_Absyn_new_exp(_T5,0U);goto _LL0;}case 4: _T6=p;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T1B=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T6;_T7=_T1B->f1;_T1A=(void*)_T7;_T19=_T1B->f2;}{void*t=_T1A;struct Cyc_Absyn_Enumfield*ef=_T19;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T1B=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_T1B->tag=32;
_T1B->f1=t;_T1B->f2=ef;_T8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T1B;}_T9=(void*)_T8;e=Cyc_Absyn_new_exp(_T9,0U);goto _LL0;}case 6: _TA=p;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T1B=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_TA;_T18=_T1B->f1;}{unsigned i=_T18;_TB=i;_T17=(int)_TB;goto _LL8;}case 7: _TC=p;{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T1B=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_TC;_T17=_T1B->f1;}_LL8: {int i=_T17;_T17=i;goto _LLA;}case 8: _TD=p;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T1B=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_TD;_T17=_T1B->f3;}_LLA: {int i=_T17;_TE=
# 3082
Cyc_Absyn_signed_int_exp(i,0U);_TF=Cyc_Absyn_sint_type;e=Cyc_Absyn_set_type(_TE,_TF);goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
_T1B.f1=_tag_fat("compile_pat_test_as_case!",sizeof(char),26U);_T10=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T10;void*_T1C[1];_T11=_T1C + 0;*_T11=& _T1B;{int(*_T1D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T12=_T1D;}_T13=_tag_fat(_T1C,sizeof(void*),1);_T12(_T13);}}_LL0:;}{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T17=_cycalloc(sizeof(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct));_T17->tag=16;
# 3085
_T17->f1=e;_T14=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T17;}_T15=(void*)_T14;_T16=Cyc_Absyn_new_pat(_T15,0U);return _T16;}
# 3089
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Stmt*_T0;struct Cyc_Absyn_Stmt*_T1;struct Cyc_Absyn_Stmt*_T2;
if(s1!=0)goto _TL358;_T0=s2;return _T0;_TL358:
 if(s2!=0)goto _TL35A;_T1=s1;return _T1;_TL35A: _T2=
Cyc_Absyn_seq_stmt(s1,s2,0U);return _T2;}struct _tuple41{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Vardecl*f1;void*f2;int f3;};
# 3101
int Cyc_Toc_vd_in_use(struct Cyc_Absyn_Vardecl*other_vd,struct Cyc_List_List**newdecls){struct Cyc_List_List**_T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple41*_T4;struct _tuple41 _T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_List_List*_T7;_T0=newdecls;{
struct Cyc_List_List*ds=*_T0;_TL35F: if(ds!=0)goto _TL35D;else{goto _TL35E;}
# 3105
_TL35D: _T1=other_vd;_T2=ds;_T3=_T2->hd;_T4=(struct _tuple41*)_T3;_T5=*_T4;_T6=_T5.f1;if(_T1!=_T6)goto _TL360;return 1;_TL360: _T7=ds;
# 3102
ds=_T7->tl;goto _TL35F;_TL35E:;}
# 3106
return 0;}
# 3109
static int Cyc_Toc_is_usuable_type(void*ctype,void*other){int*_T0;int _T1;struct Cyc_Absyn_PtrInfo _T2;int _T3;int _T4;int _T5;int _T6;
void*_T7=Cyc_Absyn_compress(ctype);struct Cyc_Absyn_Tqual _T8;_T0=(int*)_T7;_T1=*_T0;if(_T1!=4)goto _TL362;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T2=_T9->f1;_T8=_T2.elt_tq;}{struct Cyc_Absyn_Tqual tq=_T8;_T3=
# 3112
Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq);if(!_T3)goto _TL364;_T4=Cyc_Toc_is_void_star(other);if(!_T4)goto _TL364;
return 1;_TL364: goto _LL4;}_TL362: _LL4: _T5=
# 3115
Cyc_Tcutil_typecmp(ctype,other);_T6=_T5==0;return _T6;;}
# 3118
static int Cyc_Toc_can_use_voidstar(void*ctype){int*_T0;int _T1;struct Cyc_Absyn_PtrInfo _T2;int _T3;
void*_T4=Cyc_Absyn_compress(ctype);struct Cyc_Absyn_Tqual _T5;_T0=(int*)_T4;_T1=*_T0;if(_T1!=4)goto _TL366;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4;_T2=_T6->f1;_T5=_T2.elt_tq;}{struct Cyc_Absyn_Tqual tq=_T5;_T3=
Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq);return _T3;}_TL366:
 return 0;;}struct _tuple42{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Vardecl*f1;};struct _tuple43{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 3130
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**newdecls,struct Cyc_List_List**prevdecls,struct Cyc_Absyn_Exp*lpath,struct Cyc_Absyn_Exp*rpath,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;void*_T1;struct Cyc_Absyn_Pat*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T5;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T6;void*_T7;struct Cyc_Absyn_Pat*_T8;struct Cyc_Absyn_Pat*_T9;struct Cyc_Absyn_Vardecl*_TA;void*_TB;struct Cyc_List_List**_TC;struct Cyc_List_List*_TD;void*_TE;struct _tuple42*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;void*_T12;int _T13;int _T14;struct Cyc_List_List**_T15;struct Cyc_List_List*_T16;struct _RegionHandle*_T17;struct _tuple41*_T18;struct _RegionHandle*_T19;struct Cyc_List_List**_T1A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T1B;void*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_Absyn_Stmt*_T21;struct _RegionHandle*_T22;struct Cyc_Toc_Env**_T23;struct Cyc_List_List**_T24;struct Cyc_List_List**_T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_Absyn_Pat*_T28;struct Cyc_Absyn_Stmt*_T29;struct Cyc_Absyn_Stmt*_T2A;struct Cyc_List_List*_T2B;struct _tuple1*_T2C;void*_T2D;struct Cyc_List_List**_T2E;struct Cyc_List_List*_T2F;struct _RegionHandle*_T30;struct _tuple41*_T31;struct _RegionHandle*_T32;struct Cyc_List_List**_T33;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Vardecl*_T37;void*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Stmt*_T3B;struct _RegionHandle*_T3C;struct Cyc_Toc_Env**_T3D;struct Cyc_List_List**_T3E;struct Cyc_List_List**_T3F;struct Cyc_Absyn_Exp*_T40;struct Cyc_Absyn_Exp*_T41;struct Cyc_Absyn_Pat*_T42;struct Cyc_Absyn_Stmt*_T43;struct Cyc_Absyn_Stmt*_T44;struct Cyc_Absyn_Vardecl*_T45;void*_T46;struct Cyc_List_List**_T47;struct Cyc_List_List*_T48;void*_T49;struct _tuple42*_T4A;void*_T4B;struct Cyc_Absyn_Vardecl*_T4C;void*_T4D;int _T4E;int _T4F;struct Cyc_List_List**_T50;struct Cyc_List_List*_T51;struct _RegionHandle*_T52;struct _tuple41*_T53;struct _RegionHandle*_T54;struct Cyc_List_List**_T55;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T56;void*_T57;struct Cyc_Absyn_Exp*_T58;struct Cyc_Absyn_Vardecl*_T59;void*_T5A;struct Cyc_Absyn_Exp*_T5B;struct Cyc_Absyn_Exp*_T5C;struct Cyc_Absyn_Stmt*_T5D;struct Cyc_List_List*_T5E;int _T5F;struct _tuple1*_T60;void*_T61;struct Cyc_List_List**_T62;struct Cyc_List_List*_T63;struct _RegionHandle*_T64;struct _tuple41*_T65;struct _RegionHandle*_T66;struct Cyc_List_List**_T67;struct _tuple1*_T68;void*_T69;struct Cyc_List_List**_T6A;struct Cyc_List_List*_T6B;struct _RegionHandle*_T6C;struct _tuple41*_T6D;struct _RegionHandle*_T6E;struct Cyc_List_List**_T6F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T70;void*_T71;struct Cyc_Absyn_Exp*_T72;struct Cyc_Absyn_Vardecl*_T73;void*_T74;struct Cyc_Absyn_Exp*_T75;struct Cyc_Absyn_Exp*_T76;struct Cyc_Absyn_Stmt*_T77;struct Cyc_Absyn_Vardecl*_T78;struct Cyc_Absyn_Vardecl*_T79;void*_T7A;struct Cyc_List_List**_T7B;struct Cyc_List_List*_T7C;void*_T7D;struct _tuple42*_T7E;void*_T7F;struct Cyc_Absyn_Vardecl*_T80;void*_T81;int _T82;int _T83;struct Cyc_List_List**_T84;struct Cyc_List_List*_T85;struct _RegionHandle*_T86;struct _tuple41*_T87;struct _RegionHandle*_T88;struct Cyc_List_List**_T89;void*_T8A;struct Cyc_Absyn_Tqual _T8B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T8C;void*_T8D;struct Cyc_Absyn_Exp*_T8E;struct Cyc_Absyn_Vardecl*_T8F;void*_T90;struct Cyc_Absyn_Exp*_T91;struct Cyc_Absyn_Exp*_T92;void*_T93;struct Cyc_Absyn_Exp*_T94;struct Cyc_Absyn_Exp*_T95;struct Cyc_Absyn_Stmt*_T96;struct Cyc_Absyn_Stmt*_T97;struct Cyc_Absyn_Stmt*_T98;struct Cyc_List_List*_T99;int _T9A;struct _tuple1*_T9B;void*_T9C;struct Cyc_List_List**_T9D;struct Cyc_List_List*_T9E;struct _RegionHandle*_T9F;struct _tuple41*_TA0;struct _RegionHandle*_TA1;struct Cyc_List_List**_TA2;struct _tuple1*_TA3;void*_TA4;struct Cyc_List_List**_TA5;struct Cyc_List_List*_TA6;struct _RegionHandle*_TA7;struct _tuple41*_TA8;struct _RegionHandle*_TA9;struct Cyc_List_List**_TAA;void*_TAB;struct Cyc_Absyn_Tqual _TAC;struct Cyc_Absyn_Exp*_TAD;struct Cyc_Absyn_Exp*_TAE;void*_TAF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TB0;void*_TB1;struct Cyc_Absyn_Exp*_TB2;struct Cyc_Absyn_Vardecl*_TB3;void*_TB4;struct Cyc_Absyn_Exp*_TB5;struct Cyc_Absyn_Exp*_TB6;struct Cyc_Absyn_Stmt*_TB7;struct Cyc_Absyn_Stmt*_TB8;struct Cyc_Absyn_Stmt*_TB9;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_TBA;struct Cyc_Absyn_Pat*_TBB;struct Cyc_Absyn_Pat*_TBC;void*_TBD;int*_TBE;int _TBF;struct Cyc_Absyn_Pat*_TC0;void*_TC1;struct Cyc_Absyn_Datatypefield*_TC2;struct _tuple1*_TC3;struct Cyc_Absyn_Datatypedecl*_TC4;struct _tuple1*_TC5;void*_TC6;struct Cyc_Absyn_Tqual _TC7;struct Cyc_Absyn_Datatypefield*_TC8;struct _tuple1*_TC9;void*_TCA;struct Cyc_Absyn_Exp*_TCB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TCC;void*_TCD;struct Cyc_Absyn_Exp*_TCE;void*_TCF;struct Cyc_List_List*_TD0;void*_TD1;struct Cyc_Absyn_Pat*_TD2;void*_TD3;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TD4;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TD5;void*_TD6;struct Cyc_List_List*_TD7;void*_TD8;struct _tuple24*_TD9;struct _tuple24 _TDA;struct Cyc_Absyn_Pat*_TDB;void*_TDC;struct Cyc_Absyn_Exp*_TDD;struct _fat_ptr*_TDE;struct Cyc_Absyn_Exp*_TDF;struct _fat_ptr*_TE0;void*_TE1;int _TE2;struct Cyc_Absyn_Stmt*_TE3;struct Cyc_Absyn_Stmt*_TE4;struct Cyc_List_List*_TE5;struct Cyc_List_List*_TE6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TE7;void*_TE8;struct Cyc_Absyn_Decl*_TE9;struct Cyc_Absyn_Stmt*_TEA;struct Cyc_Absyn_Stmt*_TEB;struct Cyc_Absyn_Pat*_TEC;void*_TED;struct _tuple1*_TEE;void*_TEF;struct Cyc_Absyn_Exp*_TF0;void*_TF1;struct Cyc_Absyn_Exp*_TF2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TF3;void*_TF4;struct Cyc_Absyn_Exp*_TF5;void*_TF6;struct _RegionHandle*_TF7;struct Cyc_Toc_Env**_TF8;struct Cyc_List_List**_TF9;struct Cyc_List_List**_TFA;struct Cyc_Absyn_Exp*_TFB;void*_TFC;struct Cyc_Absyn_Exp*_TFD;struct Cyc_Absyn_Exp*_TFE;struct Cyc_Absyn_Pat*_TFF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T100;void*_T101;struct Cyc_Absyn_Decl*_T102;struct Cyc_Absyn_Stmt*_T103;struct Cyc_Absyn_Stmt*_T104;struct Cyc_List_List*_T105;void*_T106;struct Cyc_Absyn_Pat*_T107;void*_T108;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T109;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T10A;void*_T10B;struct Cyc_Absyn_Pat*_T10C;void*_T10D;struct Cyc_Absyn_Exp*_T10E;void*_T10F;struct Cyc_Absyn_Exp*_T110;void*_T111;struct Cyc_Absyn_Stmt*_T112;struct Cyc_Absyn_Stmt*_T113;struct Cyc_List_List*_T114;struct Cyc_Absyn_Stmt*_T115;void*_T116;struct Cyc_Warn_String_Warn_Warg_struct _T117;void**_T118;int(*_T119)(struct _fat_ptr ap);struct _fat_ptr _T11A;int*_T11B;unsigned _T11C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11D;void*_T11E;int*_T11F;int _T120;void*_T121;struct Cyc_Absyn_Aggrdecl*_T122;struct Cyc_Absyn_AggrdeclImpl*_T123;struct Cyc_Absyn_AggrdeclImpl*_T124;struct Cyc_Absyn_Aggrdecl*_T125;struct Cyc_Absyn_AggrdeclImpl*_T126;struct Cyc_Warn_String_Warn_Warg_struct _T127;void**_T128;int(*_T129)(struct _fat_ptr ap);struct _fat_ptr _T12A;struct Cyc_List_List*_T12B;void*_T12C;struct _tuple43*_T12D;struct _tuple43 _T12E;struct Cyc_Absyn_Pat*_T12F;void*_T130;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T131;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T132;void*_T133;struct _tuple43*_T134;struct _tuple43 _T135;struct Cyc_List_List*_T136;struct Cyc_Absyn_Pat*_T137;void*_T138;struct Cyc_Absyn_Aggrfield*_T139;struct Cyc_Absyn_Aggrfield*_T13A;int _T13B;struct Cyc_Absyn_Exp*_T13C;struct Cyc_Absyn_Exp*_T13D;int _T13E;int _T13F;int _T140;struct Cyc_Absyn_Kind*_T141;struct Cyc_Absyn_Kind*_T142;struct Cyc_Absyn_Kind*_T143;int _T144;void*_T145;struct Cyc_Absyn_Exp*_T146;void*_T147;struct Cyc_Absyn_Exp*_T148;struct Cyc_Absyn_Exp*_T149;struct Cyc_Absyn_Exp*_T14A;void*_T14B;struct Cyc_Absyn_Exp*_T14C;void*_T14D;struct Cyc_Absyn_Exp*_T14E;struct Cyc_Absyn_Exp*_T14F;struct Cyc_Absyn_Exp*_T150;struct Cyc_Absyn_Stmt*_T151;struct Cyc_Absyn_Stmt*_T152;struct Cyc_List_List*_T153;struct Cyc_Absyn_Stmt*_T154;struct Cyc_Warn_String_Warn_Warg_struct _T155;void**_T156;int(*_T157)(struct _fat_ptr ap);struct _fat_ptr _T158;struct Cyc_Warn_String_Warn_Warg_struct _T159;void**_T15A;int(*_T15B)(struct _fat_ptr ap);struct _fat_ptr _T15C;struct Cyc_Warn_String_Warn_Warg_struct _T15D;void**_T15E;int(*_T15F)(struct _fat_ptr ap);struct _fat_ptr _T160;_T0=p;_T1=_T0->topt;{
# 3135
void*t=_check_null(_T1);_T2=p;{
void*_T161=_T2->r;struct Cyc_List_List*_T162;struct Cyc_Absyn_Datatypefield*_T163;struct Cyc_Absyn_Pat*_T164;void*_T165;_T3=(int*)_T161;_T4=*_T3;switch(_T4){case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f2;}{struct Cyc_Absyn_Vardecl*vd=_T165;_T5=& Cyc_Absyn_Wild_p_val;_T6=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T5;_T7=(void*)_T6;{
# 3138
struct Cyc_Absyn_Pat*p2=Cyc_Absyn_new_pat(_T7,0U);_T8=p2;_T9=p;
_T8->topt=_T9->topt;_T165=vd;_T164=p2;goto _LL4;}}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f1;_T164=_T166->f2;}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T165;struct Cyc_Absyn_Pat*p2=_T164;_TA=vd;_TB=_TA->type;{
# 3143
void*ctype=Cyc_Toc_typ_to_c_array(_TB);_TC=prevdecls;{
# 3145
struct Cyc_List_List*pds=*_TC;_TL36C: if(pds!=0)goto _TL36A;else{goto _TL36B;}
_TL36A: _TD=pds;_TE=_TD->hd;_TF=(struct _tuple42*)_TE;{struct _tuple42 _T166=*_TF;struct Cyc_Absyn_Vardecl*_T167;_T167=_T166.f1;{struct Cyc_Absyn_Vardecl*other_vd=_T167;_T10=ctype;_T11=other_vd;_T12=_T11->type;_T13=
# 3149
Cyc_Toc_is_usuable_type(_T10,_T12);if(!_T13)goto _TL36D;_T14=
Cyc_Toc_vd_in_use(other_vd,newdecls);
# 3149
if(_T14)goto _TL36D;else{goto _TL36F;}
# 3151
_TL36F: _T15=newdecls;_T17=rgn;{struct Cyc_List_List*_T168=_region_malloc(_T17,0U,sizeof(struct Cyc_List_List));_T19=rgn;{struct _tuple41*_T169=_region_malloc(_T19,0U,sizeof(struct _tuple41));_T169->f0=vd;_T169->f1=other_vd;_T169->f2=ctype;_T169->f3=0;_T18=(struct _tuple41*)_T169;}_T168->hd=_T18;_T1A=newdecls;
_T168->tl=*_T1A;_T16=(struct Cyc_List_List*)_T168;}
# 3151
*_T15=_T16;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T168=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T168->tag=4;
# 3153
_T168->f1=other_vd;_T1B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T168;}_T1C=(void*)_T1B;_T1D=Cyc_Absyn_varb_exp(_T1C,0U);_T1E=ctype;{struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_set_type(_T1D,_T1E);_T1F=lhs;_T20=
Cyc_Absyn_copy_exp(rpath);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_assign_stmt(_T1F,_T20,0U);_T21=s;_T22=rgn;_T23=nv;_T24=newdecls;_T25=prevdecls;_T26=lpath;_T27=
# 3157
Cyc_Absyn_copy_exp(lhs);_T28=p2;_T29=
# 3155
Cyc_Toc_extract_pattern_vars(_T22,_T23,_T24,_T25,_T26,_T27,_T28);_T2A=Cyc_Toc_seq_stmt_opt(_T21,_T29);return _T2A;}}_TL36D:;}}_T2B=pds;
# 3145
pds=_T2B->tl;goto _TL36C;_TL36B:;}{
# 3162
struct Cyc_Absyn_Vardecl*new_vd;
struct Cyc_Absyn_Exp*lhs;_T2C=
Cyc_Toc_temp_var();_T2D=ctype;new_vd=Cyc_Absyn_new_vardecl(0U,_T2C,_T2D,0,0);_T2E=newdecls;_T30=rgn;{struct Cyc_List_List*_T166=_region_malloc(_T30,0U,sizeof(struct Cyc_List_List));_T32=rgn;{struct _tuple41*_T167=_region_malloc(_T32,0U,sizeof(struct _tuple41));
_T167->f0=vd;_T167->f1=new_vd;_T167->f2=ctype;_T167->f3=0;_T31=(struct _tuple41*)_T167;}_T166->hd=_T31;_T33=newdecls;
_T166->tl=*_T33;_T2F=(struct Cyc_List_List*)_T166;}
# 3165
*_T2E=_T2F;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T166->tag=4;
# 3167
_T166->f1=new_vd;_T34=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T166;}_T35=(void*)_T34;_T36=Cyc_Absyn_varb_exp(_T35,0U);_T37=new_vd;_T38=_T37->type;{struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_set_type(_T36,_T38);_T39=lhs;_T3A=
Cyc_Absyn_copy_exp(rpath);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_assign_stmt(_T39,_T3A,0U);_T3B=s;_T3C=rgn;_T3D=nv;_T3E=newdecls;_T3F=prevdecls;_T40=lpath;_T41=
# 3171
Cyc_Absyn_copy_exp(lhs);_T42=p2;_T43=
# 3169
Cyc_Toc_extract_pattern_vars(_T3C,_T3D,_T3E,_T3F,_T40,_T41,_T42);_T44=Cyc_Toc_seq_stmt_opt(_T3B,_T43);return _T44;}}}}}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f2;}{struct Cyc_Absyn_Vardecl*vd=_T165;_T45=vd;_T46=_T45->type;{
# 3174
void*ctype=Cyc_Toc_typ_to_c_array(_T46);_T47=prevdecls;{
# 3176
struct Cyc_List_List*pds=*_T47;_TL373: if(pds!=0)goto _TL371;else{goto _TL372;}
_TL371: _T48=pds;_T49=_T48->hd;_T4A=(struct _tuple42*)_T49;{struct _tuple42 _T166=*_T4A;struct Cyc_Absyn_Vardecl*_T167;_T167=_T166.f1;{struct Cyc_Absyn_Vardecl*other_vd=_T167;_T4B=ctype;_T4C=other_vd;_T4D=_T4C->type;_T4E=
# 3180
Cyc_Toc_is_usuable_type(_T4B,_T4D);if(!_T4E)goto _TL374;_T4F=
Cyc_Toc_vd_in_use(other_vd,newdecls);
# 3180
if(_T4F)goto _TL374;else{goto _TL376;}
# 3182
_TL376: _T50=newdecls;_T52=rgn;{struct Cyc_List_List*_T168=_region_malloc(_T52,0U,sizeof(struct Cyc_List_List));_T54=rgn;{struct _tuple41*_T169=_region_malloc(_T54,0U,sizeof(struct _tuple41));_T169->f0=vd;_T169->f1=other_vd;_T169->f2=ctype;_T169->f3=0;_T53=(struct _tuple41*)_T169;}_T168->hd=_T53;_T55=newdecls;
_T168->tl=*_T55;_T51=(struct Cyc_List_List*)_T168;}
# 3182
*_T50=_T51;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T168=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T168->tag=4;
# 3184
_T168->f1=other_vd;_T56=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T168;}_T57=(void*)_T56;_T58=Cyc_Absyn_varb_exp(_T57,0U);_T59=other_vd;_T5A=_T59->type;{struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_set_type(_T58,_T5A);_T5B=lhs;_T5C=
# 3186
Cyc_Absyn_copy_exp(rpath);_T5D=Cyc_Absyn_assign_stmt(_T5B,_T5C,0U);return _T5D;}_TL374:;}}_T5E=pds;
# 3176
pds=_T5E->tl;goto _TL373;_TL372:;}{
# 3190
struct Cyc_Absyn_Vardecl*new_vd;_T5F=
Cyc_Toc_can_use_voidstar(ctype);if(!_T5F)goto _TL377;_T60=
Cyc_Toc_temp_var();_T61=Cyc_Toc_void_star_type();new_vd=Cyc_Absyn_new_vardecl(0U,_T60,_T61,0,0);_T62=newdecls;_T64=rgn;{struct Cyc_List_List*_T166=_region_malloc(_T64,0U,sizeof(struct Cyc_List_List));_T66=rgn;{struct _tuple41*_T167=_region_malloc(_T66,0U,sizeof(struct _tuple41));
_T167->f0=vd;_T167->f1=new_vd;_T167->f2=ctype;_T167->f3=1;_T65=(struct _tuple41*)_T167;}_T166->hd=_T65;_T67=newdecls;
_T166->tl=*_T67;_T63=(struct Cyc_List_List*)_T166;}
# 3193
*_T62=_T63;goto _TL378;
# 3196
_TL377: _T68=Cyc_Toc_temp_var();_T69=ctype;new_vd=Cyc_Absyn_new_vardecl(0U,_T68,_T69,0,0);_T6A=newdecls;_T6C=rgn;{struct Cyc_List_List*_T166=_region_malloc(_T6C,0U,sizeof(struct Cyc_List_List));_T6E=rgn;{struct _tuple41*_T167=_region_malloc(_T6E,0U,sizeof(struct _tuple41));
_T167->f0=vd;_T167->f1=new_vd;_T167->f2=ctype;_T167->f3=0;_T6D=(struct _tuple41*)_T167;}_T166->hd=_T6D;_T6F=newdecls;
_T166->tl=*_T6F;_T6B=(struct Cyc_List_List*)_T166;}
# 3197
*_T6A=_T6B;_TL378:{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T166->tag=4;
# 3200
_T166->f1=new_vd;_T70=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T166;}_T71=(void*)_T70;_T72=Cyc_Absyn_varb_exp(_T71,0U);_T73=new_vd;_T74=_T73->type;_T75=Cyc_Absyn_set_type(_T72,_T74);_T76=
Cyc_Absyn_copy_exp(rpath);_T77=
# 3200
Cyc_Absyn_assign_stmt(_T75,_T76,0U);return _T77;}}}case 0:
# 3202
 return 0;case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f1;_T164=_T166->f2;}{struct Cyc_Absyn_Vardecl*vd=_T165;struct Cyc_Absyn_Pat*p2=_T164;_T78=vd;
# 3205
_T78->type=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);_T79=vd;_T7A=_T79->type;{
# 3207
void*ctype=Cyc_Toc_typ_to_c_array(_T7A);_T7B=prevdecls;{
# 3209
struct Cyc_List_List*pds=*_T7B;_TL37C: if(pds!=0)goto _TL37A;else{goto _TL37B;}
_TL37A: _T7C=pds;_T7D=_T7C->hd;_T7E=(struct _tuple42*)_T7D;{struct _tuple42 _T166=*_T7E;struct Cyc_Absyn_Vardecl*_T167;_T167=_T166.f1;{struct Cyc_Absyn_Vardecl*other_vd=_T167;_T7F=ctype;_T80=other_vd;_T81=_T80->type;_T82=
# 3213
Cyc_Toc_is_usuable_type(_T7F,_T81);if(!_T82)goto _TL37D;_T83=
Cyc_Toc_vd_in_use(other_vd,newdecls);
# 3213
if(_T83)goto _TL37D;else{goto _TL37F;}
# 3215
_TL37F: _T84=newdecls;_T86=rgn;{struct Cyc_List_List*_T168=_region_malloc(_T86,0U,sizeof(struct Cyc_List_List));_T88=rgn;{struct _tuple41*_T169=_region_malloc(_T88,0U,sizeof(struct _tuple41));_T169->f0=vd;_T169->f1=other_vd;_T169->f2=ctype;_T169->f3=0;_T87=(struct _tuple41*)_T169;}_T168->hd=_T87;_T89=newdecls;
_T168->tl=*_T89;_T85=(struct Cyc_List_List*)_T168;}
# 3215
*_T84=_T85;_T8A=
# 3217
Cyc_Toc_typ_to_c_array(t);_T8B=Cyc_Toc_mt_tq;{void*ptype=Cyc_Absyn_cstar_type(_T8A,_T8B);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T168=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T168->tag=4;
# 3219
_T168->f1=other_vd;_T8C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T168;}_T8D=(void*)_T8C;_T8E=Cyc_Absyn_varb_exp(_T8D,0U);_T8F=other_vd;_T90=_T8F->type;{struct Cyc_Absyn_Exp*vb=Cyc_Absyn_set_type(_T8E,_T90);_T91=
# 3221
Cyc_Absyn_copy_exp(lpath);_T92=Cyc_Toc_push_address_exp(_T91);_T93=ptype;{struct Cyc_Absyn_Exp*ptr=Cyc_Absyn_set_type(_T92,_T93);_T94=vb;_T95=
Cyc_Toc_cast_it(ptype,ptr);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_assign_stmt(_T94,_T95,0U);_T96=s;_T97=
Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,lpath,rpath,p2);_T98=Cyc_Toc_seq_stmt_opt(_T96,_T97);return _T98;}}}}_TL37D:;}}_T99=pds;
# 3209
pds=_T99->tl;goto _TL37C;_TL37B:;}{
# 3229
struct Cyc_Absyn_Vardecl*new_vd;_T9A=
Cyc_Toc_can_use_voidstar(ctype);if(!_T9A)goto _TL380;_T9B=
Cyc_Toc_temp_var();_T9C=Cyc_Toc_void_star_type();new_vd=Cyc_Absyn_new_vardecl(0U,_T9B,_T9C,0,0);_T9D=newdecls;_T9F=rgn;{struct Cyc_List_List*_T166=_region_malloc(_T9F,0U,sizeof(struct Cyc_List_List));_TA1=rgn;{struct _tuple41*_T167=_region_malloc(_TA1,0U,sizeof(struct _tuple41));
_T167->f0=vd;_T167->f1=new_vd;_T167->f2=ctype;_T167->f3=1;_TA0=(struct _tuple41*)_T167;}_T166->hd=_TA0;_TA2=newdecls;
_T166->tl=*_TA2;_T9E=(struct Cyc_List_List*)_T166;}
# 3232
*_T9D=_T9E;goto _TL381;
# 3235
_TL380: _TA3=Cyc_Toc_temp_var();_TA4=ctype;new_vd=Cyc_Absyn_new_vardecl(0U,_TA3,_TA4,0,0);_TA5=newdecls;_TA7=rgn;{struct Cyc_List_List*_T166=_region_malloc(_TA7,0U,sizeof(struct Cyc_List_List));_TA9=rgn;{struct _tuple41*_T167=_region_malloc(_TA9,0U,sizeof(struct _tuple41));
_T167->f0=vd;_T167->f1=new_vd;_T167->f2=ctype;_T167->f3=0;_TA8=(struct _tuple41*)_T167;}_T166->hd=_TA8;_TAA=newdecls;
_T166->tl=*_TAA;_TA6=(struct Cyc_List_List*)_T166;}
# 3236
*_TA5=_TA6;_TL381: _TAB=
# 3241
Cyc_Toc_typ_to_c_array(t);_TAC=Cyc_Toc_mt_tq;{void*ptype=Cyc_Absyn_cstar_type(_TAB,_TAC);_TAD=
Cyc_Absyn_copy_exp(lpath);_TAE=Cyc_Toc_push_address_exp(_TAD);_TAF=ptype;{struct Cyc_Absyn_Exp*ptr=Cyc_Absyn_set_type(_TAE,_TAF);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T166->tag=4;
_T166->f1=new_vd;_TB0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T166;}_TB1=(void*)_TB0;_TB2=Cyc_Absyn_varb_exp(_TB1,0U);_TB3=new_vd;_TB4=_TB3->type;_TB5=Cyc_Absyn_set_type(_TB2,_TB4);_TB6=
Cyc_Toc_cast_it(ptype,ptr);{
# 3243
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_assign_stmt(_TB5,_TB6,0U);_TB7=s;_TB8=
# 3245
Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,lpath,rpath,p2);_TB9=Cyc_Toc_seq_stmt_opt(_TB7,_TB8);return _TB9;}}}}}}case 8: goto _LLE;case 9: _LLE: goto _LL10;case 10: _LL10: goto _LL12;case 11: _LL12: goto _LL14;case 12: _LL14: goto _LL16;case 13: _LL16:
# 3252
 return 0;case 5: _TBA=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T161;_TBB=_TBA->f1;_TBC=(struct Cyc_Absyn_Pat*)_TBB;_TBD=_TBC->r;_TBE=(int*)_TBD;_TBF=*_TBE;if(_TBF!=7)goto _TL382;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T161;_TC0=_T166->f1;{struct Cyc_Absyn_Pat _T167=*_TC0;_TC1=_T167.r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T168=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_TC1;_T165=_T168->f1;_T163=_T168->f2;_T162=_T168->f3;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T165;struct Cyc_Absyn_Datatypefield*tuf=_T163;struct Cyc_List_List*ps=_T162;
# 3256
if(ps!=0)goto _TL384;return 0;_TL384: _TC2=tuf;_TC3=_TC2->name;_TC4=tud;_TC5=_TC4->name;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_TC3,_TC5);_TC6=
Cyc_Absyn_strctq(tufstrct);_TC7=Cyc_Toc_mt_tq;{void*field_ptr_typ=Cyc_Absyn_cstar_type(_TC6,_TC7);
lpath=Cyc_Toc_cast_it(field_ptr_typ,lpath);{
int cnt=1;_TC8=tuf;{
struct Cyc_List_List*tuf_tqts=_TC8->typs;
struct Cyc_Absyn_Stmt*s=0;_TC9=
Cyc_Toc_temp_var();_TCA=field_ptr_typ;_TCB=
Cyc_Toc_cast_it(field_ptr_typ,rpath);{
# 3263
struct Cyc_Absyn_Vardecl*new_vd=Cyc_Absyn_new_vardecl(0U,_TC9,_TCA,_TCB,0);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T166->tag=4;
# 3265
_T166->f1=new_vd;_TCC=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T166;}_TCD=(void*)_TCC;_TCE=Cyc_Absyn_varb_exp(_TCD,0U);_TCF=field_ptr_typ;{struct Cyc_Absyn_Exp*new_rpath=Cyc_Absyn_set_type(_TCE,_TCF);
_TL389: if(ps!=0)goto _TL387;else{goto _TL388;}
_TL387: _TD0=ps;_TD1=_TD0->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_TD1;_TD2=p2;_TD3=_TD2->r;_TD4=& Cyc_Absyn_Wild_p_val;_TD5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TD4;_TD6=(void*)_TD5;
if(_TD3!=_TD6)goto _TL38A;goto _TL386;_TL38A: _TD7=
_check_null(tuf_tqts);_TD8=_TD7->hd;_TD9=(struct _tuple24*)_TD8;_TDA=*_TD9;{void*tuf_typ=_TDA.f1;_TDB=p2;_TDC=_TDB->topt;{
void*t2=_check_null(_TDC);
void*t2c=Cyc_Toc_typ_to_c_array(t2);_TDD=
Cyc_Absyn_copy_exp(new_rpath);_TDE=Cyc_Absyn_fieldname(cnt);{struct Cyc_Absyn_Exp*rarrow_exp=Cyc_Absyn_aggrarrow_exp(_TDD,_TDE,0U);_TDF=
Cyc_Absyn_copy_exp(lpath);_TE0=Cyc_Absyn_fieldname(cnt);{struct Cyc_Absyn_Exp*larrow_exp=Cyc_Absyn_aggrarrow_exp(_TDF,_TE0,0U);
Cyc_Absyn_set_type(rarrow_exp,t2c);
Cyc_Absyn_set_type(larrow_exp,t2c);_TE1=
Cyc_Toc_typ_to_c(tuf_typ);_TE2=Cyc_Toc_is_void_star_or_boxed_tvar(_TE1);if(!_TE2)goto _TL38C;
rarrow_exp=Cyc_Toc_cast_it(t2c,rarrow_exp);
larrow_exp=Cyc_Toc_cast_it(t2c,larrow_exp);goto _TL38D;_TL38C: _TL38D: _TE3=s;_TE4=
# 3280
Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,larrow_exp,rarrow_exp,p2);s=Cyc_Toc_seq_stmt_opt(_TE3,_TE4);}}}}}_TL386: _TE5=ps;
# 3266
ps=_TE5->tl;_TE6=_check_null(tuf_tqts);tuf_tqts=_TE6->tl;cnt=cnt + 1;goto _TL389;_TL388:
# 3282
 if(s==0)goto _TL38E;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T166->tag=0;
_T166->f1=new_vd;_TE7=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T166;}_TE8=(void*)_TE7;_TE9=Cyc_Absyn_new_decl(_TE8,0U);_TEA=s;s=Cyc_Absyn_decl_stmt(_TE9,_TEA,0U);goto _TL38F;_TL38E: _TL38F: _TEB=s;
return _TEB;}}}}}}}_TL382:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f1;}{struct Cyc_Absyn_Pat*p2=_T165;_TEC=p2;_TED=_TEC->topt;{
# 3361
void*t2=_check_null(_TED);
void*t2c=Cyc_Toc_typ_to_c_array(t2);_TEE=
Cyc_Toc_temp_var();_TEF=t2c;_TF0=
Cyc_Absyn_deref_exp(rpath,0U);_TF1=t2c;_TF2=Cyc_Absyn_set_type(_TF0,_TF1);{
# 3363
struct Cyc_Absyn_Vardecl*new_vd=Cyc_Absyn_new_vardecl(0U,_TEE,_TEF,_TF2,0);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T166->tag=4;
# 3365
_T166->f1=new_vd;_TF3=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T166;}_TF4=(void*)_TF3;_TF5=Cyc_Absyn_varb_exp(_TF4,0U);_TF6=t2c;{struct Cyc_Absyn_Exp*rp=Cyc_Absyn_set_type(_TF5,_TF6);_TF7=rgn;_TF8=nv;_TF9=newdecls;_TFA=prevdecls;_TFB=
# 3367
Cyc_Absyn_deref_exp(lpath,0U);_TFC=t2c;_TFD=Cyc_Absyn_set_type(_TFB,_TFC);_TFE=rp;_TFF=p2;{
# 3366
struct Cyc_Absyn_Stmt*s=Cyc_Toc_extract_pattern_vars(_TF7,_TF8,_TF9,_TFA,_TFD,_TFE,_TFF);
# 3368
if(s==0)goto _TL390;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T166=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T166->tag=0;
_T166->f1=new_vd;_T100=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T166;}_T101=(void*)_T100;_T102=Cyc_Absyn_new_decl(_T101,0U);_T103=s;s=Cyc_Absyn_decl_stmt(_T102,_T103,0U);goto _TL391;_TL390: _TL391: _T104=s;
return _T104;}}}}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T161;_T165=_T166->f3;}{struct Cyc_List_List*ps=_T165;
# 3287
struct Cyc_Absyn_Stmt*s=0;{
int cnt=1;_TL395: if(ps!=0)goto _TL393;else{goto _TL394;}
_TL393: _T105=ps;_T106=_T105->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_T106;_T107=p2;_T108=_T107->r;_T109=& Cyc_Absyn_Wild_p_val;_T10A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T109;_T10B=(void*)_T10A;
if(_T108!=_T10B)goto _TL396;goto _TL392;_TL396: _T10C=p2;_T10D=_T10C->topt;{
# 3292
void*t2=_check_null(_T10D);
void*t2c=Cyc_Toc_typ_to_c_array(t2);
struct _fat_ptr*f=Cyc_Absyn_fieldname(cnt);_T10E=
Cyc_Toc_member_exp(lpath,f,0U);_T10F=t2c;{struct Cyc_Absyn_Exp*lp=Cyc_Absyn_set_type(_T10E,_T10F);_T110=
Cyc_Toc_member_exp(rpath,f,0U);_T111=t2c;{struct Cyc_Absyn_Exp*rp=Cyc_Absyn_set_type(_T110,_T111);_T112=s;_T113=
Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,lp,rp,p2);s=Cyc_Toc_seq_stmt_opt(_T112,_T113);}}}}_TL392: _T114=ps;
# 3288
ps=_T114->tl;cnt=cnt + 1;goto _TL395;_TL394:;}_T115=s;
# 3299
return _T115;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T166=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T161;_T116=_T166->f1;_T165=(void*)_T116;_T162=_T166->f4;}{void*topt=_T165;struct Cyc_List_List*dlps=_T162;
# 3302
if(topt!=0)goto _TL398;{struct Cyc_Warn_String_Warn_Warg_struct _T166;_T166.tag=0;_T166.f1=_tag_fat("unresolved aggregate pattern!",sizeof(char),30U);_T117=_T166;}{struct Cyc_Warn_String_Warn_Warg_struct _T166=_T117;void*_T167[1];_T118=_T167 + 0;*_T118=& _T166;{int(*_T168)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T119=_T168;}_T11A=_tag_fat(_T167,sizeof(void*),1);_T119(_T11A);}goto _TL399;_TL398: _TL399: {
void*atype=topt;
struct Cyc_List_List*fields;
int is_tagged=0;{
void*_T166=Cyc_Absyn_compress(atype);struct Cyc_List_List*_T167;int _T168;enum Cyc_Absyn_AggrKind _T169;union Cyc_Absyn_AggrInfo _T16A;_T11B=(int*)_T166;_T11C=*_T11B;switch(_T11C){case 0: _T11D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T166;_T11E=_T11D->f1;_T11F=(int*)_T11E;_T120=*_T11F;if(_T120!=24)goto _TL39B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T16B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T166;_T121=_T16B->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T16C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T121;_T16A=_T16C->f1;}}{union Cyc_Absyn_AggrInfo info=_T16A;
# 3308
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T122=ad;_T123=_T122->impl;_T124=
_check_null(_T123);fields=_T124->fields;_T125=ad;_T126=_T125->impl;
is_tagged=_T126->tagged;goto _LL2E;}_TL39B: goto _LL33;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T16B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T166;_T169=_T16B->f1;_T168=_T16B->f2;_T167=_T16B->f3;}{enum Cyc_Absyn_AggrKind ak=_T169;int is_tuple=_T168;struct Cyc_List_List*fs=_T167;
# 3313
fields=fs;goto _LL2E;}default: _LL33:{struct Cyc_Warn_String_Warn_Warg_struct _T16B;_T16B.tag=0;
# 3315
_T16B.f1=_tag_fat("bad type in aggr pattern",sizeof(char),25U);_T127=_T16B;}{struct Cyc_Warn_String_Warn_Warg_struct _T16B=_T127;void*_T16C[1];_T128=_T16C + 0;*_T128=& _T16B;{int(*_T16D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T129=_T16D;}_T12A=_tag_fat(_T16C,sizeof(void*),1);_T129(_T12A);}}_LL2E:;}{
# 3317
struct Cyc_Absyn_Stmt*s=0;
_TL3A0: if(dlps!=0)goto _TL39E;else{goto _TL39F;}
_TL39E: _T12B=dlps;_T12C=_T12B->hd;{struct _tuple43*tup=(struct _tuple43*)_T12C;_T12D=tup;_T12E=*_T12D;{
struct Cyc_Absyn_Pat*p2=_T12E.f1;_T12F=p2;_T130=_T12F->r;_T131=& Cyc_Absyn_Wild_p_val;_T132=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T131;_T133=(void*)_T132;
if(_T130!=_T133)goto _TL3A1;goto _TL39D;_TL3A1: _T134=tup;_T135=*_T134;_T136=_T135.f0;{
# 3323
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(_T136);_T137=p2;_T138=_T137->topt;{
void*t2=_check_null(_T138);
void*t2c=Cyc_Toc_typ_to_c_array(t2);_T139=
Cyc_Absyn_lookup_field(fields,f);_T13A=_check_null(_T139);{void*ftype=_T13A->type;
struct Cyc_Absyn_Exp*lmemexp=Cyc_Toc_member_exp(lpath,f,0U);
struct Cyc_Absyn_Exp*rmemexp=Cyc_Toc_member_exp(rpath,f,0U);_T13B=is_tagged;
# 3330
if(!_T13B)goto _TL3A3;{
void*u=Cyc_Toc_tagged_union_field_struct(atype,f);
Cyc_Absyn_set_type(lmemexp,u);
Cyc_Absyn_set_type(rmemexp,u);
lmemexp=Cyc_Toc_member_exp(lmemexp,Cyc_Toc_val_sp,0U);
rmemexp=Cyc_Toc_member_exp(rmemexp,Cyc_Toc_val_sp,0U);}goto _TL3A4;_TL3A3: _TL3A4: _T13C=lmemexp;
# 3337
_T13C->topt=Cyc_Toc_typ_to_c_array(ftype);_T13D=rmemexp;
_T13D->topt=Cyc_Toc_typ_to_c_array(ftype);_T13E=
Cyc_Toc_is_void_star_or_boxed_tvar(ftype);if(!_T13E)goto _TL3A5;
lmemexp=Cyc_Toc_cast_it(t2c,lmemexp);
rmemexp=Cyc_Toc_cast_it(t2c,rmemexp);goto _TL3A6;
# 3343
_TL3A5: _T13F=Cyc_Tcutil_is_array_type(ftype);if(!_T13F)goto _TL3A7;
rmemexp=Cyc_Absyn_copy_exp(lmemexp);goto _TL3A8;
# 3346
_TL3A7: _T140=Cyc_Tcutil_is_array_type(ftype);if(_T140)goto _TL3A9;else{goto _TL3AB;}_TL3AB: _T141=& Cyc_Kinds_ak;_T142=(struct Cyc_Absyn_Kind*)_T141;_T143=
Cyc_Tcutil_type_kind(ftype);_T144=Cyc_Kinds_kind_leq(_T142,_T143);
# 3346
if(!_T144)goto _TL3A9;{
# 3350
void*ct=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);_T145=ct;_T146=
Cyc_Absyn_address_exp(lmemexp,0U);_T147=ct;_T148=Cyc_Absyn_set_type(_T146,_T147);_T149=Cyc_Toc_cast_it(_T145,_T148);lmemexp=Cyc_Absyn_deref_exp(_T149,0U);_T14A=lmemexp;
_T14A->topt=t2c;_T14B=ct;_T14C=
Cyc_Absyn_address_exp(rmemexp,0U);_T14D=ct;_T14E=Cyc_Absyn_set_type(_T14C,_T14D);_T14F=Cyc_Toc_cast_it(_T14B,_T14E);rmemexp=Cyc_Absyn_deref_exp(_T14F,0U);_T150=lmemexp;
_T150->topt=t2c;}goto _TL3AA;_TL3A9: _TL3AA: _TL3A8: _TL3A6: _T151=s;_T152=
# 3356
Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,lmemexp,rmemexp,p2);s=Cyc_Toc_seq_stmt_opt(_T151,_T152);}}}}}_TL39D: _T153=dlps;
# 3318
dlps=_T153->tl;goto _TL3A0;_TL39F: _T154=s;
# 3358
return _T154;}}}case 14:{struct Cyc_Warn_String_Warn_Warg_struct _T166;_T166.tag=0;
# 3372
_T166.f1=_tag_fat("unknownid pat",sizeof(char),14U);_T155=_T166;}{struct Cyc_Warn_String_Warn_Warg_struct _T166=_T155;void*_T167[1];_T156=_T167 + 0;*_T156=& _T166;{int(*_T168)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T157=_T168;}_T158=_tag_fat(_T167,sizeof(void*),1);_T157(_T158);}case 15:{struct Cyc_Warn_String_Warn_Warg_struct _T166;_T166.tag=0;
_T166.f1=_tag_fat("unknowncall pat",sizeof(char),16U);_T159=_T166;}{struct Cyc_Warn_String_Warn_Warg_struct _T166=_T159;void*_T167[1];_T15A=_T167 + 0;*_T15A=& _T166;{int(*_T168)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T15B=_T168;}_T15C=_tag_fat(_T167,sizeof(void*),1);_T15B(_T15C);}default:{struct Cyc_Warn_String_Warn_Warg_struct _T166;_T166.tag=0;
_T166.f1=_tag_fat("exp pat",sizeof(char),8U);_T15D=_T166;}{struct Cyc_Warn_String_Warn_Warg_struct _T166=_T15D;void*_T167[1];_T15E=_T167 + 0;*_T15E=& _T166;{int(*_T168)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T15F=_T168;}_T160=_tag_fat(_T167,sizeof(void*),1);_T15F(_T160);}};}}}
# 3381
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;struct _handler_cons*_T9;int*_TA;int _TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_Absyn_Vardecl*(*_TE)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);void*(*_TF)(struct Cyc_List_List*,void*);struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T10;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T11;struct Cyc_Absyn_Vardecl*(*_T12)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);void*(*_T13)(struct Cyc_List_List*,void*);void*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T18;void*_T19;struct Cyc_Core_Not_found_exn_struct*_T1A;char*_T1B;char*_T1C;void(*_T1D)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T1E)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;void*_T23;struct _tuple20*_T24;struct _tuple20 _T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_List_List*_T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;void**_T29;int(*_T2A)(struct _fat_ptr);void*(*_T2B)(struct _fat_ptr);struct _fat_ptr _T2C;_T0=e;{
void*_T2D=_T0->r;struct Cyc_List_List*_T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_Absyn_Vardecl*_T32;_T1=(int*)_T2D;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T2D;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T2D;_T7=_T33->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T34=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T7;_T32=_T34->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T32;_T32=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T2D;_T8=_T33->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T34=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T8;_T32=_T34->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T32;{struct _handler_cons _T33;_T9=& _T33;_push_handler(_T9);{int _T34=0;_TA=_T33.handler;_TB=setjmp(_TA);if(!_TB)goto _TL3AE;_T34=1;goto _TL3AF;_TL3AE: _TL3AF: if(_T34)goto _TL3B0;else{goto _TL3B2;}_TL3B2: _TC=vd;_TD=_TC->type;{
# 3388
void*ctype=Cyc_Toc_typ_to_c_array(_TD);_TF=Cyc_List_assoc;{
struct Cyc_Absyn_Vardecl*(*_T35)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))_TF;_TE=_T35;}{struct Cyc_Absyn_Vardecl*tmpvd=_TE(env,vd);{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T35=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_T35->tag=1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T36=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T36->tag=4;_T13=Cyc_List_assoc;{
struct Cyc_Absyn_Vardecl*(*_T37)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))_T13;_T12=_T37;}_T36->f1=_T12(env,vd);_T11=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T36;}_T35->f1=(void*)_T11;_T10=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T35;}_T14=(void*)_T10;{struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp(_T14,0U);_T15=e1;_T16=tmpvd;
_T15->topt=_T16->type;_T17=e;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T35=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_T35->tag=14;
_T35->f1=ctype;_T35->f2=e1;_T35->f3=0;_T35->f4=1U;_T18=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T35;}_T17->r=(void*)_T18;}}}_pop_handler();goto _TL3B1;_TL3B0: _T19=Cyc_Core_get_exn_thrown();{void*_T35=(void*)_T19;void*_T36;_T1A=(struct Cyc_Core_Not_found_exn_struct*)_T35;_T1B=_T1A->tag;_T1C=Cyc_Core_Not_found;if(_T1B!=_T1C)goto _TL3B3;goto _LL41;_TL3B3: _T36=_T35;{void*exn=_T36;_rethrow(exn);}_LL41:;}_TL3B1:;}}goto _LL0;}default: goto _LL3F;};case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f2;_T2F=_T33->f3;}{struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;struct Cyc_Absyn_Exp*e3=_T2F;
# 3396
Cyc_Toc_subst_pattern_vars(env,e1);_T31=e2;_T30=e3;goto _LL8;}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f2;_T30=_T33->f3;}_LL8: {struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;_T31=e1;_T30=e2;goto _LLA;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f2;}_LLA: {struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;_T31=e1;_T30=e2;goto _LLC;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f2;}_LLC: {struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;_T31=e1;_T30=e2;goto _LLE;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f2;}_LLE: {struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;_T31=e1;_T30=e2;goto _LL10;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f2;}_LL10: {struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;
# 3402
Cyc_Toc_subst_pattern_vars(env,e1);_T31=e2;goto _LL12;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL12: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL14;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL14: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL16;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL16: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL18;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f2;}_LL18: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL1A;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL1A: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL1C;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL1C: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL1E;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL1E: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL20;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL20: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL22;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL22: {struct Cyc_Absyn_Exp*e=_T31;_T31=e;goto _LL24;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;}_LL24: {struct Cyc_Absyn_Exp*e=_T31;
# 3412
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T30=_T33->f3;}{struct Cyc_Absyn_Exp*e1=_T31;struct Cyc_Absyn_Exp*e2=_T30;
# 3414
Cyc_Toc_subst_pattern_vars(env,e1);
Cyc_Toc_subst_pattern_vars(env,e2);goto _LL0;}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T2D;_T31=_T33->f1;_T2E=_T33->f2;}{struct Cyc_Absyn_Exp*e=_T31;struct Cyc_List_List*es=_T2E;
# 3417
Cyc_Toc_subst_pattern_vars(env,e);_T2E=es;goto _LL2A;}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f2;}_LL2A: {struct Cyc_List_List*es=_T2E;_T1E=Cyc_List_iter_c;{
void(*_T33)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))_T1E;_T1D=_T33;}_T1F=env;_T20=es;_T1D(Cyc_Toc_subst_pattern_vars,_T1F,_T20);goto _LL0;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f2;}{struct Cyc_List_List*dles=_T2E;_T2E=dles;goto _LL2E;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f3;}_LL2E: {struct Cyc_List_List*dles=_T2E;_T2E=dles;goto _LL30;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f3;}_LL30: {struct Cyc_List_List*dles=_T2E;_T2E=dles;goto _LL32;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f1;}_LL32: {struct Cyc_List_List*dles=_T2E;_T2E=dles;goto _LL34;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T33=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T2D;_T2E=_T33->f2;}_LL34: {struct Cyc_List_List*dles=_T2E;
# 3424
_TL3B8: if(dles!=0)goto _TL3B6;else{goto _TL3B7;}
_TL3B6: _T21=env;_T22=dles;_T23=_T22->hd;_T24=(struct _tuple20*)_T23;_T25=*_T24;_T26=_T25.f1;Cyc_Toc_subst_pattern_vars(_T21,_T26);_T27=dles;
# 3424
dles=_T27->tl;goto _TL3B8;_TL3B7: goto _LL0;}case 37: goto _LL38;case 11: _LL38: goto _LL3A;case 33: _LL3A: goto _LL3C;case 16: _LL3C: goto _LL3E;case 30: _LL3E:{struct Cyc_Warn_String_Warn_Warg_struct _T33;_T33.tag=0;
# 3431
_T33.f1=_tag_fat("subst_pattern_vars, unexpected exp",sizeof(char),35U);_T28=_T33;}{struct Cyc_Warn_String_Warn_Warg_struct _T33=_T28;void*_T34[1];_T29=_T34 + 0;*_T29=& _T33;_T2B=Cyc_Warn_impos2;{int(*_T35)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2B;_T2A=_T35;}_T2C=_tag_fat(_T34,sizeof(void*),1);_T2A(_T2C);}default: _LL3F: goto _LL0;}_LL0:;}}struct _tuple44{struct Cyc_Toc_Env*f0;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};
# 3441
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct Cyc_Absyn_Exp*v){struct Cyc_Absyn_Stmt*_T0;void*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Stmt*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;int*_T8;struct Cyc_Absyn_Switch_clause*_T9;struct Cyc_Absyn_Stmt*_TA;struct Cyc_Tcpat_Rhs*_TB;struct Cyc_Absyn_Stmt*_TC;int*_TD;int _TE;struct Cyc_Absyn_Stmt*_TF;int*_T10;struct _RegionHandle*_T11;struct Cyc_Toc_Env**_T12;struct Cyc_List_List**_T13;struct Cyc_List_List**_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Switch_clause*_T17;struct Cyc_Absyn_Pat*_T18;struct Cyc_Absyn_Switch_clause*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct _tuple41*_T1C;struct Cyc_List_List**_T1D;struct Cyc_List_List*_T1E;struct _RegionHandle*_T1F;struct _tuple42*_T20;struct _RegionHandle*_T21;struct Cyc_List_List**_T22;int _T23;struct Cyc_Absyn_Vardecl*_T24;void*_T25;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T26;void*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_Absyn_Vardecl*_T29;void*_T2A;struct Cyc_Absyn_Exp*_T2B;struct Cyc_Absyn_Vardecl*_T2C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2D;void*_T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Vardecl*_T30;void*_T31;struct Cyc_Absyn_Vardecl*_T32;struct Cyc_Absyn_Exp*_T33;struct Cyc_Absyn_Vardecl*_T34;struct Cyc_Absyn_Vardecl*_T35;struct Cyc_Absyn_Decl*_T36;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T37;struct Cyc_Absyn_Stmt*_T38;struct Cyc_List_List*_T39;struct Cyc_List_List**_T3A;struct Cyc_List_List*_T3B;struct _RegionHandle*_T3C;struct _tuple44*_T3D;struct _RegionHandle*_T3E;struct Cyc_List_List**_T3F;struct Cyc_Absyn_Stmt*_T40;struct Cyc_List_List*_T41;struct Cyc_Warn_String_Warn_Warg_struct _T42;void**_T43;int(*_T44)(struct _fat_ptr ap);struct _fat_ptr _T45;void*_T46;void*_T47;struct Cyc_List_List*_T48;struct Cyc_Absyn_Exp*_T49;struct Cyc_List_List*_T4A;struct Cyc_Absyn_Switch_clause*_T4B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T4C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T4D;void*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;void*_T51;struct _tuple36*_T52;struct Cyc_List_List*_T53;struct Cyc_Absyn_Switch_clause*_T54;struct Cyc_List_List*_T55;void*_T56;int*_T57;unsigned _T58;struct Cyc_Absyn_Exp*_T59;int*_T5A;unsigned _T5B;void*_T5C;struct Cyc_Absyn_Exp*_T5D;struct Cyc_Absyn_Exp*_T5E;struct Cyc_Absyn_Exp*_T5F;void*_T60;void*_T61;void*_T62;struct Cyc_Absyn_Exp*_T63;void*_T64;struct Cyc_Absyn_Exp*_T65;void*_T66;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T67;void*_T68;void*_T69;int*_T6A;int _T6B;void*_T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List*_T6E;struct _tuple36*_T6F;struct _tuple36*_T70;void*_T71;int*_T72;int _T73;struct Cyc_List_List*_T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;void*_T78;struct _tuple36*_T79;void*_T7A;struct Cyc_List_List*_T7B;void*_T7C;int*_T7D;struct Cyc_Absyn_Switch_clause*_T7E;struct Cyc_Absyn_Stmt*_T7F;struct Cyc_Tcpat_Rhs*_T80;struct Cyc_Absyn_Stmt*_T81;int*_T82;int _T83;struct Cyc_Absyn_Stmt*_T84;int*_T85;struct _RegionHandle*_T86;struct Cyc_Toc_Env**_T87;struct Cyc_List_List**_T88;struct Cyc_List_List**_T89;struct Cyc_Absyn_Exp*_T8A;struct Cyc_Absyn_Exp*_T8B;struct Cyc_Absyn_Switch_clause*_T8C;struct Cyc_Absyn_Pat*_T8D;struct Cyc_Absyn_Switch_clause*_T8E;struct Cyc_List_List*_T8F;void*_T90;struct _tuple41*_T91;struct Cyc_List_List**_T92;struct Cyc_List_List*_T93;struct _RegionHandle*_T94;struct _tuple42*_T95;struct _RegionHandle*_T96;struct Cyc_List_List**_T97;int _T98;struct Cyc_Absyn_Vardecl*_T99;void*_T9A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T9B;void*_T9C;struct Cyc_Absyn_Exp*_T9D;struct Cyc_Absyn_Vardecl*_T9E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T9F;void*_TA0;struct Cyc_Absyn_Vardecl*_TA1;struct Cyc_Absyn_Exp*_TA2;struct Cyc_Absyn_Vardecl*_TA3;struct Cyc_Absyn_Vardecl*_TA4;struct Cyc_Absyn_Decl*_TA5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TA6;struct Cyc_Absyn_Stmt*_TA7;struct Cyc_List_List*_TA8;struct Cyc_List_List**_TA9;struct Cyc_List_List*_TAA;struct Cyc_Absyn_Exp*_TAB;struct Cyc_List_List**_TAC;struct Cyc_List_List*_TAD;struct _RegionHandle*_TAE;struct _tuple44*_TAF;struct _RegionHandle*_TB0;struct Cyc_List_List**_TB1;struct Cyc_Absyn_Stmt*_TB2;struct Cyc_List_List*_TB3;struct Cyc_Warn_String_Warn_Warg_struct _TB4;void**_TB5;int(*_TB6)(struct _fat_ptr ap);struct _fat_ptr _TB7;struct Cyc_Warn_String_Warn_Warg_struct _TB8;void**_TB9;int(*_TBA)(struct _fat_ptr ap);struct _fat_ptr _TBB;struct Cyc_List_List*_TBC;void*_TBD;struct _tuple36*_TBE;struct Cyc_List_List*_TBF;struct Cyc_Absyn_Stmt*_TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;void*_TC3;if(dopt!=0)goto _TL3B9;_T0=
# 3451
Cyc_Absyn_skip_stmt(0U);return _T0;_TL3B9: _T1=dopt;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=
Cyc_Toc_throw_match_stmt();return _T4;case 1: _T5=dopt;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_TC4=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T5;_TC3=_TC4->f1;}{struct Cyc_Tcpat_Rhs*rhs=_TC3;
# 3455
_TL3BF: if(lscs!=0)goto _TL3BD;else{goto _TL3BE;}
_TL3BD: _T6=lscs;_T7=_T6->hd;{struct _tuple39*_TC4=(struct _tuple39*)_T7;struct Cyc_Absyn_Switch_clause*_TC5;struct _fat_ptr*_TC6;struct _fat_ptr*_TC7;void*_TC8;{struct _tuple39 _TC9=*_TC4;_T8=& _TC4->f0;_TC8=(int*)_T8;_TC7=_TC9.f1;_TC6=_TC9.f2;_TC5=_TC9.f3;}{int*already_emitted=(int*)_TC8;struct _fat_ptr*init_lab=_TC7;struct _fat_ptr*code_lab=_TC6;struct Cyc_Absyn_Switch_clause*sc=_TC5;_T9=sc;{
struct Cyc_Absyn_Stmt*body=_T9->body;_TA=body;_TB=rhs;_TC=_TB->rhs;
if(_TA!=_TC)goto _TL3C0;_TD=already_emitted;_TE=*_TD;
# 3460
if(!_TE)goto _TL3C2;_TF=
Cyc_Absyn_goto_stmt(init_lab,0U);return _TF;_TL3C2: _T10=already_emitted;
*_T10=1;{
struct Cyc_List_List*newdecls=0;_T11=rgn;_T12=& nv;_T13=& newdecls;_T14=decls;_T15=
# 3468
Cyc_Absyn_copy_exp(v);_T16=
Cyc_Absyn_copy_exp(v);_T17=sc;_T18=_T17->pattern;{
# 3467
struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(_T11,_T12,_T13,_T14,_T15,_T16,_T18);_T19=sc;{
# 3472
struct Cyc_Absyn_Stmt*res=_T19->body;{
struct Cyc_List_List*ds=newdecls;_TL3C7: if(ds!=0)goto _TL3C5;else{goto _TL3C6;}
_TL3C5: _T1A=ds;_T1B=_T1A->hd;_T1C=(struct _tuple41*)_T1B;{struct _tuple41 _TC9=*_T1C;int _TCA;void*_TCB;struct Cyc_Absyn_Vardecl*_TCC;struct Cyc_Absyn_Vardecl*_TCD;_TCD=_TC9.f0;_TCC=_TC9.f1;_TCB=_TC9.f2;_TCA=_TC9.f3;{struct Cyc_Absyn_Vardecl*oldv=_TCD;struct Cyc_Absyn_Vardecl*newv=_TCC;void*oldvt=_TCB;int cast=_TCA;_T1D=decls;_T1F=rgn;{struct Cyc_List_List*_TCE=_region_malloc(_T1F,0U,sizeof(struct Cyc_List_List));_T21=rgn;{struct _tuple42*_TCF=_region_malloc(_T21,0U,sizeof(struct _tuple42));
_TCF->f0=oldv;_TCF->f1=newv;_T20=(struct _tuple42*)_TCF;}_TCE->hd=_T20;_T22=decls;_TCE->tl=*_T22;_T1E=(struct Cyc_List_List*)_TCE;}*_T1D=_T1E;_T23=cast;
if(!_T23)goto _TL3C8;_T24=oldv;_T25=oldvt;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TCE=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TCE->tag=4;
_TCE->f1=newv;_T26=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TCE;}_T27=(void*)_T26;_T28=Cyc_Absyn_varb_exp(_T27,0U);_T29=newv;_T2A=_T29->type;_T2B=Cyc_Absyn_set_type(_T28,_T2A);_T24->initializer=Cyc_Toc_cast_it(_T25,_T2B);goto _TL3C9;
_TL3C8: _T2C=oldv;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TCE=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TCE->tag=4;_TCE->f1=newv;_T2D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TCE;}_T2E=(void*)_T2D;_T2F=Cyc_Absyn_varb_exp(_T2E,0U);_T30=newv;_T31=_T30->type;_T2C->initializer=Cyc_Absyn_set_type(_T2F,_T31);_TL3C9: _T32=oldv;_T33=_T32->initializer;_T34=newv;
_T33->topt=_T34->type;_T35=oldv;
# 3481
_T35->type=oldvt;{struct Cyc_Absyn_Decl*_TCE=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TCF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TCF->tag=0;
_TCF->f1=oldv;_T37=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TCF;}_TCE->r=(void*)_T37;_TCE->loc=0U;_T36=(struct Cyc_Absyn_Decl*)_TCE;}_T38=res;res=Cyc_Absyn_decl_stmt(_T36,_T38,0U);}}_T39=ds;
# 3473
ds=_T39->tl;goto _TL3C7;_TL3C6:;}
# 3484
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt==0)goto _TL3CA;
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);goto _TL3CB;_TL3CA: _TL3CB:
 res=Cyc_Absyn_label_stmt(init_lab,res,0U);_T3A=bodies;_T3C=rgn;{struct Cyc_List_List*_TC9=_region_malloc(_T3C,0U,sizeof(struct Cyc_List_List));_T3E=rgn;{struct _tuple44*_TCA=_region_malloc(_T3E,0U,sizeof(struct _tuple44));
_TCA->f0=nv;_TCA->f1=code_lab;_TCA->f2=body;_T3D=(struct _tuple44*)_TCA;}_TC9->hd=_T3D;_T3F=bodies;_TC9->tl=*_T3F;_T3B=(struct Cyc_List_List*)_TC9;}*_T3A=_T3B;_T40=res;
return _T40;}}}_TL3C0:;}}}_T41=lscs;
# 3455
lscs=_T41->tl;goto _TL3BF;_TL3BE:{struct Cyc_Warn_String_Warn_Warg_struct _TC4;_TC4.tag=0;
# 3492
_TC4.f1=_tag_fat("couldn't find rhs!",sizeof(char),19U);_T42=_TC4;}{struct Cyc_Warn_String_Warn_Warg_struct _TC4=_T42;void*_TC5[1];_T43=_TC5 + 0;*_T43=& _TC4;{int(*_TC6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T44=_TC6;}_T45=_tag_fat(_TC5,sizeof(void*),1);_T44(_T45);}}default: _T46=dopt;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_TC4=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T46;_TC2=_TC4->f1;_TC1=_TC4->f2;_T47=_TC4->f3;_TC3=(void*)_T47;}{struct Cyc_List_List*symbolic_path=_TC2;struct Cyc_List_List*switches=_TC1;void*other_decision=_TC3;
# 3495
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);_T48=
# 3497
Cyc_List_rev(symbolic_path);_T49=Cyc_Absyn_copy_exp(v);{struct Cyc_Absyn_Exp*p=Cyc_Toc_compile_path(_T48,_T49);
struct Cyc_List_List*ss=Cyc_List_rev(switches);
# 3500
struct _tuple40 _TC4=Cyc_Toc_admits_switch(ss);void*_TC5;int _TC6;_TC6=_TC4.f0;_TC5=_TC4.f1;{int allows_switch=_TC6;void*test_kind=_TC5;
if(allows_switch <= 1)goto _TL3CC;{
# 3504
struct Cyc_List_List*new_lscs;new_lscs=_cycalloc(sizeof(struct Cyc_List_List));_T4A=new_lscs;{struct Cyc_Absyn_Switch_clause*_TC7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_T4C=& Cyc_Absyn_Wild_p_val;_T4D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T4C;_T4E=(void*)_T4D;
_TC7->pattern=Cyc_Absyn_new_pat(_T4E,0U);_TC7->pat_vars=0;_TC7->where_clause=0;_TC7->body=res;_TC7->loc=0U;_T4B=(struct Cyc_Absyn_Switch_clause*)_TC7;}_T4A->hd=_T4B;_T4F=new_lscs;_T4F->tl=0;
# 3507
_TL3D1: if(ss!=0)goto _TL3CF;else{goto _TL3D0;}
_TL3CF: _T50=ss;_T51=_T50->hd;_T52=(struct _tuple36*)_T51;{struct _tuple36 _TC7=*_T52;void*_TC8;void*_TC9;_TC9=_TC7.f0;_TC8=_TC7.f1;{void*pat_test=_TC9;void*dec_tree=_TC8;
# 3510
struct Cyc_Absyn_Pat*case_exp=Cyc_Toc_compile_pat_test_as_case(pat_test);
# 3512
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);{struct Cyc_List_List*_TCA=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_TCB=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
# 3514
_TCB->pattern=case_exp;_TCB->pat_vars=0;_TCB->where_clause=0;_TCB->body=s;_TCB->loc=0U;_T54=(struct Cyc_Absyn_Switch_clause*)_TCB;}_TCA->hd=_T54;_TCA->tl=new_lscs;_T53=(struct Cyc_List_List*)_TCA;}new_lscs=_T53;}}_T55=ss;
# 3507
ss=_T55->tl;goto _TL3D1;_TL3D0:{struct _fat_ptr*_TC7;void*_TC8;_T56=test_kind;_T57=(int*)_T56;_T58=*_T57;switch(_T58){case 1:
# 3518
 LOOP1: _T59=p;{
void*_TC9=_T59->r;struct Cyc_Absyn_Exp*_TCA;_T5A=(int*)_TC9;_T5B=*_T5A;switch(_T5B){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TCB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TC9;_TCA=_TCB->f2;}{struct Cyc_Absyn_Exp*e=_TCA;
p=e;goto LOOP1;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_TCB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_TC9;_TCA=_TCB->f1;}{struct Cyc_Absyn_Exp*e=_TCA;
p=e;goto _LL1E;}default: goto _LL1E;}_LL1E:;}_T5C=
# 3524
Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);_T5D=p;_T5E=Cyc_Toc_cast_it(_T5C,_T5D);_T5F=Cyc_Absyn_deref_exp(_T5E,0U);_T60=Cyc_Absyn_uint_type;p=Cyc_Absyn_set_type(_T5F,_T60);goto _LL15;case 3: _T61=test_kind;{struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_TC9=(struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_T61;_T62=_TC9->f1;_TC8=(void*)_T62;_TC7=_TC9->f2;}{void*t=_TC8;struct _fat_ptr*f=_TC7;
# 3527
void*t2=Cyc_Toc_tagged_union_field_struct(t,f);_T63=
Cyc_Toc_member_exp(p,f,0U);_T64=t2;p=Cyc_Absyn_set_type(_T63,_T64);_T65=
Cyc_Toc_member_exp(p,Cyc_Toc_tag_sp,0U);_T66=Cyc_Absyn_uint_type;p=Cyc_Absyn_set_type(_T65,_T66);goto _LL15;}case 2: goto _LL1D;default: _LL1D:
# 3536
 p=Cyc_Toc_cast_it(Cyc_Absyn_sint_type,p);goto _LL15;}_LL15:;}{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_TC7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_TC7->tag=10;
# 3539
_TC7->f1=p;_TC7->f2=new_lscs;_TC7->f3=0;_T67=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_TC7;}_T68=(void*)_T67;res=Cyc_Absyn_new_stmt(_T68,0U);}goto _TL3CD;
# 3543
_TL3CC:{struct Cyc_Absyn_Exp*_TC7;_T69=test_kind;_T6A=(int*)_T69;_T6B=*_T6A;if(_T6B!=2)goto _TL3D4;_T6C=test_kind;{struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_TC8=(struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_T6C;_TC7=_TC8->f1;}{struct Cyc_Absyn_Exp*e=_TC7;struct Cyc_Tcpat_Rhs*_TC8;void*_TC9;if(ss==0)goto _TL3D6;_T6D=ss;_T6E=(struct Cyc_List_List*)_T6D;_T6F=_T6E->hd;_T70=(struct _tuple36*)_T6F;_T71=_T70->f1;_T72=(int*)_T71;_T73=*_T72;if(_T73!=1)goto _TL3D8;_T74=ss;_T75=(struct Cyc_List_List*)_T74;_T76=_T75->tl;if(_T76!=0)goto _TL3DA;_T77=ss;{struct Cyc_List_List _TCA=*_T77;_T78=_TCA.hd;_T79=(struct _tuple36*)_T78;{struct _tuple36 _TCB=*_T79;_TC9=_TCB.f0;_T7A=_TCB.f1;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_TCC=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T7A;_TC8=_TCC->f1;}}}{void*pat_test=_TC9;struct Cyc_Tcpat_Rhs*rhs=_TC8;
# 3551
_TL3DF: if(lscs!=0)goto _TL3DD;else{goto _TL3DE;}
_TL3DD: _T7B=lscs;_T7C=_T7B->hd;{struct _tuple39*_TCA=(struct _tuple39*)_T7C;struct Cyc_Absyn_Switch_clause*_TCB;struct _fat_ptr*_TCC;struct _fat_ptr*_TCD;void*_TCE;{struct _tuple39 _TCF=*_TCA;_T7D=& _TCA->f0;_TCE=(int*)_T7D;_TCD=_TCF.f1;_TCC=_TCF.f2;_TCB=_TCF.f3;}{int*already_emitted=(int*)_TCE;struct _fat_ptr*init_lab=_TCD;struct _fat_ptr*code_lab=_TCC;struct Cyc_Absyn_Switch_clause*sc=_TCB;_T7E=sc;{
struct Cyc_Absyn_Stmt*body=_T7E->body;_T7F=body;_T80=rhs;_T81=_T80->rhs;
if(_T7F!=_T81)goto _TL3E0;_T82=already_emitted;_T83=*_T82;
# 3556
if(!_T83)goto _TL3E2;_T84=
Cyc_Absyn_goto_stmt(init_lab,0U);return _T84;_TL3E2: _T85=already_emitted;
*_T85=1;{
struct Cyc_List_List*newdecls=0;_T86=rgn;_T87=& nv;_T88=& newdecls;_T89=decls;_T8A=
# 3561
Cyc_Absyn_copy_exp(v);_T8B=Cyc_Absyn_copy_exp(v);_T8C=sc;_T8D=_T8C->pattern;{struct Cyc_Absyn_Stmt*init_opt=Cyc_Toc_extract_pattern_vars(_T86,_T87,_T88,_T89,_T8A,_T8B,_T8D);_T8E=sc;{
# 3564
struct Cyc_Absyn_Stmt*r=_T8E->body;{
struct Cyc_List_List*ds=newdecls;_TL3E7: if(ds!=0)goto _TL3E5;else{goto _TL3E6;}
_TL3E5: _T8F=ds;_T90=_T8F->hd;_T91=(struct _tuple41*)_T90;{struct _tuple41 _TCF=*_T91;int _TD0;void*_TD1;struct Cyc_Absyn_Vardecl*_TD2;struct Cyc_Absyn_Vardecl*_TD3;_TD3=_TCF.f0;_TD2=_TCF.f1;_TD1=_TCF.f2;_TD0=_TCF.f3;{struct Cyc_Absyn_Vardecl*oldv=_TD3;struct Cyc_Absyn_Vardecl*newv=_TD2;void*oldvt=_TD1;int cast=_TD0;_T92=decls;_T94=rgn;{struct Cyc_List_List*_TD4=_region_malloc(_T94,0U,sizeof(struct Cyc_List_List));_T96=rgn;{struct _tuple42*_TD5=_region_malloc(_T96,0U,sizeof(struct _tuple42));
_TD5->f0=oldv;_TD5->f1=newv;_T95=(struct _tuple42*)_TD5;}_TD4->hd=_T95;_T97=decls;_TD4->tl=*_T97;_T93=(struct Cyc_List_List*)_TD4;}*_T92=_T93;_T98=cast;
if(!_T98)goto _TL3E8;_T99=oldv;_T9A=oldvt;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TD4=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TD4->tag=4;_TD4->f1=newv;_T9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TD4;}_T9C=(void*)_T9B;_T9D=Cyc_Absyn_varb_exp(_T9C,0U);_T99->initializer=Cyc_Toc_cast_it(_T9A,_T9D);goto _TL3E9;
_TL3E8: _T9E=oldv;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TD4=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TD4->tag=4;_TD4->f1=newv;_T9F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TD4;}_TA0=(void*)_T9F;_T9E->initializer=Cyc_Absyn_varb_exp(_TA0,0U);_TL3E9: _TA1=oldv;_TA2=_TA1->initializer;_TA3=newv;
_TA2->topt=_TA3->type;_TA4=oldv;
# 3572
_TA4->type=oldvt;{struct Cyc_Absyn_Decl*_TD4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TD5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TD5->tag=0;
_TD5->f1=oldv;_TA6=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TD5;}_TD4->r=(void*)_TA6;_TD4->loc=0U;_TA5=(struct Cyc_Absyn_Decl*)_TD4;}_TA7=r;r=Cyc_Absyn_decl_stmt(_TA5,_TA7,0U);}}_TA8=ds;
# 3565
ds=_TA8->tl;goto _TL3E7;_TL3E6:;}
# 3575
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3579
Cyc_Toc_exp_to_c(nv,e);_TA9=decls;_TAA=*_TA9;_TAB=e;
Cyc_Toc_subst_pattern_vars(_TAA,_TAB);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt==0)goto _TL3EA;
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);goto _TL3EB;_TL3EA: _TL3EB:
 r=Cyc_Absyn_label_stmt(init_lab,r,0U);_TAC=bodies;_TAE=rgn;{struct Cyc_List_List*_TCF=_region_malloc(_TAE,0U,sizeof(struct Cyc_List_List));_TB0=rgn;{struct _tuple44*_TD0=_region_malloc(_TB0,0U,sizeof(struct _tuple44));
_TD0->f0=nv;_TD0->f1=code_lab;_TD0->f2=body;_TAF=(struct _tuple44*)_TD0;}_TCF->hd=_TAF;_TB1=bodies;_TCF->tl=*_TB1;_TAD=(struct Cyc_List_List*)_TCF;}*_TAC=_TAD;_TB2=r;
return _TB2;}}}_TL3E0:;}}}_TB3=lscs;
# 3551
lscs=_TB3->tl;goto _TL3DF;_TL3DE:{struct Cyc_Warn_String_Warn_Warg_struct _TCA;_TCA.tag=0;
# 3589
_TCA.f1=_tag_fat("couldn't find rhs!",sizeof(char),19U);_TB4=_TCA;}{struct Cyc_Warn_String_Warn_Warg_struct _TCA=_TB4;void*_TCB[1];_TB5=_TCB + 0;*_TB5=& _TCA;{int(*_TCC)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TB6=_TCC;}_TB7=_tag_fat(_TCB,sizeof(void*),1);_TB6(_TB7);}}goto _TL3DB;_TL3DA: goto _LL2D;_TL3DB: goto _TL3D9;_TL3D8: goto _LL2D;_TL3D9: goto _TL3D7;_TL3D6: _LL2D:{struct Cyc_Warn_String_Warn_Warg_struct _TCA;_TCA.tag=0;
_TCA.f1=_tag_fat("bad where clause in match compiler",sizeof(char),35U);_TB8=_TCA;}{struct Cyc_Warn_String_Warn_Warg_struct _TCA=_TB8;void*_TCB[1];_TB9=_TCB + 0;*_TB9=& _TCA;{int(*_TCC)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TBA=_TCC;}_TBB=_tag_fat(_TCB,sizeof(void*),1);_TBA(_TBB);}_TL3D7:;}goto _TL3D5;_TL3D4:
# 3594
 _TL3EF: if(ss!=0)goto _TL3ED;else{goto _TL3EE;}
_TL3ED: _TBC=ss;_TBD=_TBC->hd;_TBE=(struct _tuple36*)_TBD;{struct _tuple36 _TC8=*_TBE;void*_TC9;void*_TCA;_TCA=_TC8.f0;_TC9=_TC8.f1;{void*pat_test=_TCA;void*dec_tree=_TC9;
struct Cyc_Absyn_Exp*test_exp=Cyc_Toc_compile_pat_test(p,pat_test);
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}_TBF=ss;
# 3594
ss=_TBF->tl;goto _TL3EF;_TL3EE: _TL3D5:;}_TL3CD: _TC0=res;
# 3602
return _TC0;}}}};}
# 3612
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Toc_Env**_T2;struct Cyc_Toc_Env**_T3;struct Cyc_List_List*_T4;
# 3614
_TL3F3: if(bodies!=0)goto _TL3F1;else{goto _TL3F2;}
_TL3F1: _T0=bodies;_T1=_T0->hd;{struct _tuple44*_T5=(struct _tuple44*)_T1;struct Cyc_Absyn_Stmt*_T6;void*_T7;{struct _tuple44 _T8=*_T5;_T2=& _T5->f0;_T7=(struct Cyc_Toc_Env**)_T2;_T6=_T8.f2;}{struct Cyc_Toc_Env**nv=(struct Cyc_Toc_Env**)_T7;struct Cyc_Absyn_Stmt*s2=_T6;
if(s2!=s)goto _TL3F4;_T3=nv;return _T3;_TL3F4:;}}_T4=bodies;
# 3614
bodies=_T4->tl;goto _TL3F3;_TL3F2:
# 3620
 return 0;}
# 3624
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;int*_T3;int _T4;void*_T5;struct _tuple1*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T9;void*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_List_List*(*_TE)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_List_List*(*_TF)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T10;struct _RegionHandle*_T11;struct Cyc_List_List*_T12;struct _RegionHandle*_T13;struct Cyc_Toc_Env*_T14;struct Cyc_List_List**_T15;struct Cyc_List_List**_T16;void*_T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_Absyn_Switch_clause*_T1C;struct Cyc_Toc_Env**_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_List_List*_T23;struct Cyc_Absyn_Switch_clause*_T24;struct Cyc_Absyn_Stmt*_T25;struct Cyc_Toc_Env*_T26;struct Cyc_Absyn_Stmt*_T27;struct Cyc_Absyn_Switch_clause*_T28;struct Cyc_Core_Opt*_T29;struct Cyc_Absyn_Switch_clause*_T2A;struct Cyc_Core_Opt*_T2B;void*_T2C;struct Cyc_List_List*_T2D;struct _tuple0 _T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*(*_T30)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T31)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Absyn_Vardecl*(*_T32)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);void*(*_T33)(struct Cyc_List_List*,void*);struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;struct Cyc_Toc_Env*_T36;struct Cyc_Absyn_Stmt*_T37;struct Cyc_Toc_Env*_T38;struct Cyc_Absyn_Stmt*_T39;struct Cyc_List_List*_T3A;struct Cyc_Absyn_Stmt*_T3B;struct _fat_ptr*_T3C;struct Cyc_Absyn_Stmt*_T3D;struct Cyc_Absyn_Stmt*_T3E;struct Cyc_List_List*_T3F;void*_T40;struct _tuple42*_T41;struct Cyc_List_List*_T42;void*_T43;struct Cyc_Absyn_Vardecl*_T44;struct Cyc_Absyn_Vardecl*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct _RegionHandle*_T48;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T49;void*_T4A;struct Cyc_Absyn_Decl*_T4B;struct Cyc_Absyn_Stmt*_T4C;struct Cyc_List_List*_T4D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4E;void*_T4F;struct Cyc_Absyn_Decl*_T50;struct Cyc_Absyn_Stmt*_T51;struct Cyc_Absyn_Stmt*_T52;struct Cyc_Absyn_Stmt*_T53;_T0=e;_T1=_T0->topt;{
# 3627
void*t=_check_null(_T1);
Cyc_Toc_exp_to_c(nv,e);{
# 3630
struct Cyc_Absyn_Vardecl*v=0;
struct Cyc_Absyn_Exp*vexp=e;_T2=e;{
void*_T54=_T2->r;void*_T55;_T3=(int*)_T54;_T4=*_T3;if(_T4!=1)goto _TL3F6;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T56=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T5=_T56->f1;_T55=(void*)_T5;}{void*b=_T55;goto _LL0;}_TL3F6:{
# 3635
struct _tuple1*x=Cyc_Toc_temp_var();_T6=x;_T7=
Cyc_Toc_typ_to_c(t);_T8=e;v=Cyc_Absyn_new_vardecl(0U,_T6,_T7,_T8,0);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T56=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T56->tag=4;
_T56->f1=v;_T9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T56;}_TA=(void*)_T9;_TB=Cyc_Absyn_varb_exp(_TA,0U);_TC=v;_TD=_TC->type;vexp=Cyc_Absyn_set_type(_TB,_TD);goto _LL0;}_LL0:;}{
# 3640
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _T54=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T54;_push_region(rgn);{
# 3642
struct Cyc_Toc_Env*_T55=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_T55;_TF=Cyc_List_rmap_c;{
# 3646
struct Cyc_List_List*(*_T56)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))_TF;_TE=_T56;}_T10=rgn;_T11=rgn;_T12=scs;{
# 3645
struct Cyc_List_List*lscs=
_TE(_T10,Cyc_Toc_gen_labels,_T11,_T12);
# 3650
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;_T13=rgn;_T14=nv;_T15=& mydecls;_T16=& mybodies;_T17=dopt;_T18=lscs;_T19=vexp;{
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(_T13,_T14,_T15,_T16,_T17,_T18,_T19);{
# 3658
struct Cyc_List_List*lscs2=lscs;_TL3FB: if(lscs2!=0)goto _TL3F9;else{goto _TL3FA;}
_TL3F9: _T1A=lscs2;_T1B=_T1A->hd;{struct _tuple39*_T56=(struct _tuple39*)_T1B;struct Cyc_Absyn_Switch_clause*_T57;struct _fat_ptr*_T58;{struct _tuple39 _T59=*_T56;_T58=_T59.f2;_T57=_T59.f3;}{struct _fat_ptr*body_lab=_T58;struct Cyc_Absyn_Switch_clause*body_sc=_T57;_T1C=body_sc;{
struct Cyc_Absyn_Stmt*s=_T1C->body;
# 3662
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3665
if(envp!=0)goto _TL3FC;goto _TL3F8;_TL3FC: _T1D=envp;{
struct Cyc_Toc_Env*env=*_T1D;_T1E=lscs2;_T1F=_T1E->tl;
# 3668
if(_T1F==0)goto _TL3FE;_T20=lscs2;_T21=_T20->tl;_T22=_T21->hd;{
struct _tuple39*_T59=(struct _tuple39*)_T22;struct Cyc_Absyn_Switch_clause*_T5A;struct _fat_ptr*_T5B;{struct _tuple39 _T5C=*_T59;_T5B=_T5C.f2;_T5A=_T5C.f3;}{struct _fat_ptr*fallthru_lab=_T5B;struct Cyc_Absyn_Switch_clause*next_sc=_T5A;_T23=mybodies;_T24=next_sc;_T25=_T24->body;{
# 3672
struct Cyc_Toc_Env**next_case_env=Cyc_Toc_find_case_env(_T23,_T25);
# 3677
if(next_case_env!=0)goto _TL400;_T26=
Cyc_Toc_last_switchclause_env(rgn,env,end_l);_T27=s;Cyc_Toc_stmt_to_c(_T26,_T27);goto _TL401;
# 3681
_TL400:{struct Cyc_List_List*vs=0;_T28=next_sc;_T29=_T28->pat_vars;
if(_T29==0)goto _TL402;_T2A=next_sc;_T2B=_T2A->pat_vars;_T2C=_T2B->v;_T2D=(struct Cyc_List_List*)_T2C;_T2E=
Cyc_List_split(_T2D);_T2F=_T2E.f0;vs=Cyc_Tcutil_filter_nulls(_T2F);_T31=Cyc_List_map_c;{
struct Cyc_List_List*(*_T5C)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*))_T31;_T30=_T5C;}_T33=Cyc_List_assoc;{struct Cyc_Absyn_Vardecl*(*_T5C)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))_T33;_T32=_T5C;}_T34=mydecls;_T35=Cyc_List_imp_rev(vs);vs=_T30(_T32,_T34,_T35);goto _TL403;_TL402: _TL403: _T36=
# 3686
Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);_T37=s;Cyc_Toc_stmt_to_c(_T36,_T37);}_TL401:;}}}goto _TL3FF;
# 3690
_TL3FE: _T38=Cyc_Toc_last_switchclause_env(rgn,env,end_l);_T39=s;Cyc_Toc_stmt_to_c(_T38,_T39);_TL3FF:;}}}}_TL3F8: _T3A=lscs2;
# 3658
lscs2=_T3A->tl;goto _TL3FB;_TL3FA:;}_T3B=test_tree;_T3C=end_l;_T3D=
# 3694
Cyc_Toc_skip_stmt_dl();_T3E=Cyc_Absyn_label_stmt(_T3C,_T3D,0U);{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(_T3B,_T3E,0U);
# 3696
struct Cyc_List_List*added=0;
_TL407: if(mydecls!=0)goto _TL405;else{goto _TL406;}
_TL405: _T3F=mydecls;_T40=_T3F->hd;_T41=(struct _tuple42*)_T40;{struct _tuple42 _T56=*_T41;struct Cyc_Absyn_Vardecl*_T57;_T57=_T56.f1;{struct Cyc_Absyn_Vardecl*vd=_T57;{
struct Cyc_List_List*itr=added;_TL40B: if(itr!=0)goto _TL409;else{goto _TL40A;}
_TL409: _T42=itr;_T43=_T42->hd;_T44=(struct Cyc_Absyn_Vardecl*)_T43;_T45=vd;if(_T44!=_T45)goto _TL40C;goto bottom;_TL40C: _T46=itr;
# 3699
itr=_T46->tl;goto _TL40B;_TL40A:;}_T48=rgn;{struct Cyc_List_List*_T58=_region_malloc(_T48,0U,sizeof(struct Cyc_List_List));
# 3701
_T58->hd=vd;_T58->tl=added;_T47=(struct Cyc_List_List*)_T58;}added=_T47;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T58=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T58->tag=0;
_T58->f1=vd;_T49=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T58;}_T4A=(void*)_T49;_T4B=Cyc_Absyn_new_decl(_T4A,0U);_T4C=res;res=Cyc_Absyn_decl_stmt(_T4B,_T4C,0U);
bottom: goto _TL404;}}_TL404: _T4D=
# 3697
_check_null(mydecls);mydecls=_T4D->tl;goto _TL407;_TL406:
# 3707
 if(v==0)goto _TL40E;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T56=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T56->tag=0;
_T56->f1=v;_T4E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T56;}_T4F=(void*)_T4E;_T50=Cyc_Absyn_new_decl(_T4F,0U);_T51=res;res=Cyc_Absyn_decl_stmt(_T50,_T51,0U);goto _TL40F;_TL40E: _TL40F: _T52=whole_s;_T53=res;
# 3710
_T52->r=_T53->r;_npop_handler(0);return;}}}}_pop_region();}}}}
# 3718
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp**_T1;int _T2;struct Cyc_Absyn_Exp*_T3;void*_T4;struct Cyc_Absyn_Exp*_T5;struct _fat_ptr _T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Stmt*_T9;{struct Cyc_Absyn_Exp*_TA[1];_T1=_TA + 0;_T2=n - 1;_T3=
# 3720
Cyc_Absyn_signed_int_exp(_T2,0U);_T4=Cyc_Absyn_sint_type;*_T1=Cyc_Absyn_set_type(_T3,_T4);_T5=Cyc_Toc__npop_handler_e;_T6=_tag_fat(_TA,sizeof(struct Cyc_Absyn_Exp*),1);_T0=Cyc_Toc_fncall_exp_dl(_T5,_T6);}_T7=Cyc_Absyn_void_type;_T8=
# 3719
Cyc_Absyn_set_type(_T0,_T7);_T9=Cyc_Absyn_exp_stmt(_T8,0U);return _T9;}
# 3722
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;struct Cyc_Absyn_Stmt*_T1;struct Cyc_Absyn_Stmt*_T2;void*_T3;struct Cyc_Absyn_Stmt*_T4;
if(n <= 0)goto _TL410;_T0=s;_T1=
Cyc_Toc_make_npop_handler(n);_T2=s;_T3=_T2->r;_T4=Cyc_Absyn_new_stmt(_T3,0U);_T0->r=Cyc_Toc_seq_stmt_r(_T1,_T4);goto _TL411;_TL410: _TL411:;}
# 3727
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T3;struct Cyc_Absyn_Exp*_T4;int _T5;struct Cyc_Absyn_Stmt*_T6;struct Cyc_Absyn_Exp*_T7;void*_T8;void*_T9;struct Cyc_Absyn_Exp*_TA;void*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Absyn_Stmt*_TD;struct Cyc_Absyn_Stmt*_TE;struct _tuple10 _TF;struct _tuple10 _T10;struct _tuple10 _T11;struct _tuple10 _T12;struct Cyc_Toc_Env*_T13;struct Cyc_Absyn_Stmt*_T14;struct Cyc_Toc_Env*_T15;struct Cyc_Absyn_Stmt*_T16;struct _fat_ptr**_T17;struct _fat_ptr*_T18;int _T19;struct Cyc_Absyn_Stmt*_T1A;struct Cyc_Toc_Env*_T1B;struct Cyc_Absyn_Stmt*_T1C;struct _fat_ptr**_T1D;struct _fat_ptr*_T1E;int _T1F;struct Cyc_Absyn_Stmt*_T20;void*_T21;struct Cyc_Toc_Env*_T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;void**_T24;int(*_T25)(struct _fat_ptr ap);struct _fat_ptr _T26;struct Cyc_Toc_FallthruInfo*_T27;int _T28;struct Cyc_Absyn_Stmt*_T29;struct Cyc_Toc_Env*_T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T2E;struct Cyc_List_List*_T2F;void*_T30;void*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_List_List*_T33;void*_T34;struct Cyc_Absyn_Exp*_T35;struct Cyc_Absyn_Stmt*_T36;struct Cyc_Absyn_Stmt*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;struct Cyc_Absyn_Stmt*_T3A;struct Cyc_Absyn_Stmt*_T3B;struct Cyc_Absyn_Decl*_T3C;int*_T3D;unsigned _T3E;void*_T3F;struct Cyc_Absyn_Pat*_T40;int*_T41;int _T42;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T43;struct Cyc_Absyn_Pat*_T44;struct Cyc_Absyn_Pat*_T45;void*_T46;int*_T47;int _T48;struct Cyc_Absyn_Vardecl*_T49;struct _tuple1*_T4A;struct Cyc_Absyn_Exp*_T4B;int _T4C;struct _tuple1*_T4D;struct Cyc_Absyn_Vardecl*_T4E;void*_T4F;struct Cyc_Absyn_Exp*_T50;struct Cyc_Absyn_Vardecl*_T51;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T52;void*_T53;struct Cyc_Absyn_Vardecl*_T54;struct Cyc_Absyn_Exp*_T55;struct Cyc_Absyn_Vardecl*_T56;struct Cyc_Absyn_Stmt*_T57;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T58;void*_T59;struct Cyc_Absyn_Stmt*_T5A;unsigned _T5B;struct Cyc_Absyn_Decl*_T5C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T5D;void*_T5E;struct Cyc_Absyn_Stmt*_T5F;unsigned _T60;struct Cyc_Absyn_Decl*_T61;struct Cyc_Absyn_Stmt*_T62;struct Cyc_Absyn_Stmt*_T63;unsigned _T64;struct Cyc_Absyn_Stmt*_T65;struct Cyc_Absyn_Stmt*_T66;unsigned _T67;struct Cyc_Absyn_Stmt*_T68;struct Cyc_Absyn_Vardecl*_T69;struct Cyc_Absyn_Stmt*_T6A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T6B;void*_T6C;struct Cyc_Absyn_Stmt*_T6D;unsigned _T6E;struct Cyc_Absyn_Decl*_T6F;struct Cyc_Absyn_Stmt*_T70;struct Cyc_Absyn_Stmt*_T71;unsigned _T72;struct Cyc_Absyn_Stmt*_T73;struct Cyc_Absyn_Stmt*_T74;struct Cyc_Toc_Env*_T75;struct Cyc_Absyn_Pat*_T76;void*_T77;struct Cyc_Absyn_Exp*_T78;void*_T79;void*_T7A;struct Cyc_Absyn_Exp*_T7B;struct Cyc_Absyn_Stmt*_T7C;struct Cyc_Absyn_Stmt*_T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;void**_T7F;int(*_T80)(struct _fat_ptr ap);struct _fat_ptr _T81;struct Cyc_Absyn_Decl*_T82;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T83;struct Cyc_List_List*_T84;void*_T85;struct Cyc_List_List*_T86;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T87;struct Cyc_List_List*_T88;void*_T89;void*_T8A;struct Cyc_Absyn_Stmt*_T8B;struct Cyc_Absyn_Decl*_T8C;struct Cyc_Absyn_Stmt*_T8D;void*_T8E;struct Cyc_Absyn_Stmt*_T8F;struct Cyc_Absyn_Stmt*_T90;struct Cyc_List_List*_T91;struct Cyc_Absyn_Vardecl*_T92;struct Cyc_Absyn_Exp*_T93;void*_T94;int _T95;struct Cyc_Absyn_Stmt*_T96;struct _tuple1*_T97;void*_T98;struct Cyc_Absyn_Exp*_T99;void*_T9A;struct Cyc_Absyn_Exp*_T9B;struct Cyc_Absyn_Stmt*_T9C;struct Cyc_Absyn_Stmt*_T9D;struct Cyc_Absyn_Exp*_T9E;unsigned _T9F;struct Cyc_Absyn_Exp*_TA0;struct _fat_ptr*_TA1;struct Cyc_Absyn_Stmt*_TA2;struct Cyc_Absyn_Stmt*_TA3;struct Cyc_Absyn_Exp*_TA4;void*_TA5;struct Cyc_Absyn_Exp*_TA6;void*_TA7;struct Cyc_Absyn_Exp*_TA8;int _TA9;struct Cyc_Absyn_Stmt*_TAA;struct _tuple1*_TAB;void*_TAC;struct Cyc_Absyn_Exp*_TAD;struct Cyc_Absyn_Exp**_TAE;struct Cyc_Absyn_Exp**_TAF;struct _tuple1*_TB0;struct _tuple1 _TB1;struct _fat_ptr*_TB2;struct _fat_ptr _TB3;struct Cyc_Absyn_Exp*_TB4;struct _fat_ptr _TB5;void*_TB6;struct Cyc_Absyn_Exp*_TB7;struct _tuple1*_TB8;void*_TB9;struct Cyc_Absyn_Exp*_TBA;void*_TBB;struct Cyc_Absyn_Exp*_TBC;struct Cyc_Absyn_Exp*_TBD;struct Cyc_Absyn_Exp**_TBE;struct Cyc_Absyn_Exp*_TBF;struct _fat_ptr _TC0;void*_TC1;struct Cyc_Absyn_Exp*_TC2;struct Cyc_Absyn_Stmt*_TC3;struct Cyc_Absyn_Stmt*_TC4;struct Cyc_Absyn_Exp*_TC5;struct _fat_ptr _TC6;struct Cyc_Absyn_Exp*_TC7;void*_TC8;struct Cyc_Absyn_Exp*_TC9;struct Cyc_Absyn_Stmt*_TCA;struct Cyc_Absyn_Stmt*_TCB;struct Cyc_Absyn_Stmt*_TCC;struct Cyc_Absyn_Stmt*_TCD;struct Cyc_Absyn_Stmt*_TCE;struct Cyc_Warn_String_Warn_Warg_struct _TCF;void**_TD0;int(*_TD1)(struct _fat_ptr);void*(*_TD2)(struct _fat_ptr);struct _fat_ptr _TD3;void*_TD4;void*_TD5;struct Cyc_Absyn_Exp*_TD6;struct Cyc_Absyn_Exp*_TD7;struct Cyc_Absyn_Stmt*_TD8;struct Cyc_Absyn_Exp*_TD9;struct _fat_ptr _TDA;struct Cyc_Absyn_Exp*_TDB;void*_TDC;struct Cyc_Absyn_Exp*_TDD;struct Cyc_Absyn_Stmt*_TDE;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_TDF;void*_TE0;struct Cyc_Absyn_Exp*_TE1;struct Cyc_Absyn_Exp**_TE2;struct Cyc_Absyn_Exp*_TE3;void*_TE4;struct Cyc_Absyn_Exp*_TE5;struct _fat_ptr _TE6;void*_TE7;struct Cyc_Absyn_Exp*_TE8;struct Cyc_Absyn_Exp**_TE9;struct Cyc_Absyn_Exp*_TEA;void*_TEB;struct Cyc_Absyn_Exp*_TEC;struct _fat_ptr _TED;void*_TEE;struct Cyc_Absyn_Exp*_TEF;struct Cyc_Absyn_Exp*_TF0;void*_TF1;struct Cyc_Absyn_Exp*_TF2;void*_TF3;struct Cyc_Absyn_Stmt*_TF4;struct _tuple1*_TF5;void*_TF6;struct Cyc_Absyn_Stmt*_TF7;struct _tuple1*_TF8;void*_TF9;struct Cyc_Absyn_Exp*_TFA;struct Cyc_Absyn_Exp*_TFB;struct Cyc_Absyn_Exp*_TFC;struct Cyc_Absyn_Exp*_TFD;struct Cyc_Absyn_Stmt*_TFE;struct Cyc_Absyn_Stmt*_TFF;struct Cyc_Absyn_Stmt*_T100;struct Cyc_Absyn_Exp*_T101;void*_T102;struct Cyc_Absyn_Exp*_T103;struct Cyc_Absyn_Stmt*_T104;struct _tuple1*_T105;void*_T106;void*_T107;struct Cyc_Absyn_Exp*_T108;void*_T109;struct Cyc_Absyn_Exp*_T10A;struct Cyc_Absyn_Exp*_T10B;struct Cyc_Absyn_Stmt*_T10C;struct Cyc_Absyn_Stmt*_T10D;struct Cyc_Absyn_Stmt*_T10E;struct Cyc_Absyn_Stmt*_T10F;struct Cyc_Absyn_Stmt*_T110;struct Cyc_Absyn_Stmt*_T111;struct Cyc_Absyn_Stmt*_T112;
# 3729
_TL412: if(1)goto _TL413;else{goto _TL414;}
_TL413: _T0=s;{void*_T113=_T0->r;struct Cyc_Absyn_Switch_clause**_T114;struct Cyc_List_List*_T115;struct Cyc_Absyn_Exp*_T116;struct Cyc_Absyn_Stmt*_T117;void*_T118;void*_T119;_T1=(int*)_T113;_T2=*_T1;switch(_T2){case 0:
 return;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;}{struct Cyc_Absyn_Exp*e=_T119;
Cyc_Toc_exp_to_c(nv,e);return;}case 3: _T3=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T113;_T4=_T3->f1;if(_T4!=0)goto _TL416;_T5=
Cyc_Toc_get_npop(s);_T6=s;Cyc_Toc_do_npop_before(_T5,_T6);return;_TL416:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;}{struct Cyc_Absyn_Exp*e=_T119;_T7=e;_T8=_T7->topt;_T9=
# 3736
_check_null(_T8);{void*t=Cyc_Toc_typ_to_c(_T9);
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop <= 0)goto _TL418;{
struct _tuple1*x=Cyc_Toc_temp_var();_TA=
Cyc_Absyn_var_exp(x,0U);_TB=t;_TC=Cyc_Absyn_set_type(_TA,_TB);{struct Cyc_Absyn_Stmt*retn_stmt=Cyc_Absyn_return_stmt(_TC,0U);
Cyc_Toc_do_npop_before(npop,retn_stmt);_TD=s;_TE=
Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U);_TD->r=_TE->r;}}goto _TL419;_TL418: _TL419:
# 3745
 return;}}}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T118=_T11A->f2;}{struct Cyc_Absyn_Stmt*s1=_T119;struct Cyc_Absyn_Stmt*s2=_T118;
# 3747
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;goto _TL412;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T118=_T11A->f2;_T117=_T11A->f3;}{struct Cyc_Absyn_Exp*e=_T119;struct Cyc_Absyn_Stmt*s1=_T118;struct Cyc_Absyn_Stmt*s2=_T117;
# 3751
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;goto _TL412;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_TF=_T11A->f2;_T118=_TF.f0;_T10=_T11A->f3;_T116=_T10.f0;_T117=_T11A->f4;}{struct Cyc_Absyn_Exp*e1=_T119;struct Cyc_Absyn_Exp*e2=_T118;struct Cyc_Absyn_Exp*e3=_T116;struct Cyc_Absyn_Stmt*s2=_T117;
# 3756
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);_T119=s2;_T118=e3;goto _LL10;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T11=_T11A->f2;_T118=_T11.f0;}_LL10: {struct Cyc_Absyn_Stmt*s2=_T119;struct Cyc_Absyn_Exp*e=_T118;_T119=e;_T118=s2;goto _LL12;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T113;_T12=_T11A->f1;_T119=_T12.f0;_T118=_T11A->f2;}_LL12: {struct Cyc_Absyn_Exp*e=_T119;struct Cyc_Absyn_Stmt*s2=_T118;
# 3761
Cyc_Toc_exp_to_c(nv,e);{struct _RegionHandle _T11A=_new_region(0U,"temp");struct _RegionHandle*temp=& _T11A;_push_region(temp);_T13=
Cyc_Toc_loop_env(temp,nv);_T14=s2;Cyc_Toc_stmt_to_c(_T13,_T14);_npop_handler(0);return;_pop_region();}}case 6:  {struct _fat_ptr**_T11A;_T15=nv;{struct Cyc_Toc_Env _T11B=*_T15;_T11A=_T11B.break_lab;}{struct _fat_ptr**b=_T11A;
# 3766
if(b==0)goto _TL41A;_T16=s;_T17=b;_T18=*_T17;
_T16->r=Cyc_Toc_goto_stmt_r(_T18);goto _TL41B;_TL41A: _TL41B: _T19=
# 3769
Cyc_Toc_get_npop(s);_T1A=s;Cyc_Toc_do_npop_before(_T19,_T1A);
return;}}case 7:  {struct _fat_ptr**_T11A;_T1B=nv;{struct Cyc_Toc_Env _T11B=*_T1B;_T11A=_T11B.continue_lab;}{struct _fat_ptr**c=_T11A;
# 3773
if(c==0)goto _TL41C;_T1C=s;_T1D=c;_T1E=*_T1D;
_T1C->r=Cyc_Toc_goto_stmt_r(_T1E);goto _TL41D;_TL41C: _TL41D: goto _LL18;}}case 8: _LL18: _T1F=
# 3777
Cyc_Toc_get_npop(s);_T20=s;Cyc_Toc_do_npop_before(_T1F,_T20);
return;case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T118=_T11A->f2;}{struct _fat_ptr*lab=_T119;struct Cyc_Absyn_Stmt*s1=_T118;
s=s1;goto _TL412;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T115=_T11A->f2;_T21=_T11A->f3;_T118=(void*)_T21;}{struct Cyc_Absyn_Exp*e=_T119;struct Cyc_List_List*scs=_T115;void*dec_tree_opt=_T118;
# 3781
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T114=_T11A->f2;}{struct Cyc_List_List*es=_T119;struct Cyc_Absyn_Switch_clause**dest_clause=_T114;struct Cyc_Toc_FallthruInfo*_T11A;_T22=nv;{struct Cyc_Toc_Env _T11B=*_T22;_T11A=_T11B.fallthru_info;}{struct Cyc_Toc_FallthruInfo*fi=_T11A;
# 3785
if(fi!=0)goto _TL41E;{struct Cyc_Warn_String_Warn_Warg_struct _T11B;_T11B.tag=0;
_T11B.f1=_tag_fat("fallthru in unexpected place",sizeof(char),29U);_T23=_T11B;}{struct Cyc_Warn_String_Warn_Warg_struct _T11B=_T23;void*_T11C[1];_T24=_T11C + 0;*_T24=& _T11B;{int(*_T11D)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T25=_T11D;}_T26=_tag_fat(_T11C,sizeof(void*),1);_T25(_T26);}goto _TL41F;_TL41E: _TL41F: _T27=fi;{
struct Cyc_Toc_FallthruInfo _T11B=*_T27;struct Cyc_List_List*_T11C;struct _fat_ptr*_T11D;_T11D=_T11B.label;_T11C=_T11B.binders;{struct _fat_ptr*l=_T11D;struct Cyc_List_List*vs=_T11C;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);_T28=
# 3790
Cyc_Toc_get_npop(s);_T29=s2;Cyc_Toc_do_npop_before(_T28,_T29);{
struct Cyc_List_List*vs2=Cyc_List_rev(vs);
struct Cyc_List_List*es2=Cyc_List_rev(es);
_TL423: if(vs2!=0)goto _TL421;else{goto _TL422;}
_TL421: _T2A=nv;_T2B=_check_null(es2);_T2C=_T2B->hd;_T2D=(struct Cyc_Absyn_Exp*)_T2C;Cyc_Toc_exp_to_c(_T2A,_T2D);{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T11E=_cycalloc(sizeof(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct));_T11E->tag=5;_T2F=vs2;_T30=_T2F->hd;
_T11E->f1=(struct Cyc_Absyn_Vardecl*)_T30;_T2E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T11E;}_T31=(void*)_T2E;_T32=Cyc_Absyn_varb_exp(_T31,0U);_T33=es2;_T34=_T33->hd;_T35=(struct Cyc_Absyn_Exp*)_T34;_T36=Cyc_Absyn_assign_stmt(_T32,_T35,0U);_T37=s2;s2=Cyc_Absyn_seq_stmt(_T36,_T37,0U);_T38=vs2;
# 3793
vs2=_T38->tl;_T39=es2;es2=_T39->tl;goto _TL423;_TL422: _T3A=s;_T3B=s2;
# 3798
_T3A->r=_T3B->r;
return;}}}}}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T113;_T119=_T11A->f1;_T117=_T11A->f2;}{struct Cyc_Absyn_Decl*d=_T119;struct Cyc_Absyn_Stmt*body=_T117;_T3C=d;{
# 3804
void*_T11A=_T3C->r;struct Cyc_Absyn_Vardecl*_T11B;struct Cyc_Absyn_Tvar*_T11C;struct Cyc_Absyn_Fndecl*_T11D;struct Cyc_List_List*_T11E;struct Cyc_Absyn_Exp*_T11F;struct Cyc_Absyn_Pat*_T120;void*_T121;_T3D=(int*)_T11A;_T3E=*_T3D;switch(_T3E){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T122=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T11A;_T121=_T122->f1;}{struct Cyc_Absyn_Vardecl*vd=_T121;
Cyc_Toc_local_decl_to_c(nv,vd,body);goto _LL2F;}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T122=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T11A;_T120=_T122->f1;_T11F=_T122->f3;_T3F=_T122->f4;_T121=(void*)_T3F;}{struct Cyc_Absyn_Pat*p=_T120;struct Cyc_Absyn_Exp*e=_T11F;void*dec_tree=_T121;_T40=p;{
# 3811
void*_T122=_T40->r;struct Cyc_Absyn_Vardecl*_T123;_T41=(int*)_T122;_T42=*_T41;if(_T42!=1)goto _TL425;_T43=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T122;_T44=_T43->f2;_T45=(struct Cyc_Absyn_Pat*)_T44;_T46=_T45->r;_T47=(int*)_T46;_T48=*_T47;if(_T48!=0)goto _TL427;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T124=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T122;_T123=_T124->f1;}{struct Cyc_Absyn_Vardecl*vd=_T123;_T49=vd;_T4A=_T49->name;_T4B=e;_T4C=
# 3813
Cyc_Absyn_var_may_appear_exp(_T4A,_T4B);if(!_T4C)goto _TL429;_T4D=
Cyc_Toc_temp_var();_T4E=vd;_T4F=_T4E->type;_T50=e;{struct Cyc_Absyn_Vardecl*new_vd=Cyc_Absyn_new_vardecl(0U,_T4D,_T4F,_T50,0);_T51=vd;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T124=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T124->tag=4;
_T124->f1=new_vd;_T52=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T124;}_T53=(void*)_T52;_T51->initializer=Cyc_Absyn_varb_exp(_T53,0U);_T54=vd;_T55=_T54->initializer;_T56=new_vd;
_T55->topt=_T56->type;_T57=s;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T124=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T124->tag=0;
_T124->f1=new_vd;_T58=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T124;}_T59=(void*)_T58;_T5A=s;_T5B=_T5A->loc;_T5C=Cyc_Absyn_new_decl(_T59,_T5B);{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T124=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T124->tag=0;
_T124->f1=vd;_T5D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T124;}_T5E=(void*)_T5D;_T5F=s;_T60=_T5F->loc;_T61=Cyc_Absyn_new_decl(_T5E,_T60);_T62=body;_T63=s;_T64=_T63->loc;_T65=Cyc_Absyn_decl_stmt(_T61,_T62,_T64);_T66=s;_T67=_T66->loc;_T68=
# 3817
Cyc_Absyn_decl_stmt(_T5C,_T65,_T67);_T57->r=_T68->r;}goto _TL42A;
# 3821
_TL429: _T69=vd;_T69->initializer=e;_T6A=s;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T124=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T124->tag=0;
_T124->f1=vd;_T6B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T124;}_T6C=(void*)_T6B;_T6D=s;_T6E=_T6D->loc;_T6F=Cyc_Absyn_new_decl(_T6C,_T6E);_T70=body;_T71=s;_T72=_T71->loc;_T73=Cyc_Absyn_decl_stmt(_T6F,_T70,_T72);_T6A->r=_T73->r;_TL42A:
# 3824
 Cyc_Toc_stmt_to_c(nv,s);goto _LL3C;}_TL427: goto _LL3F;_TL425: _LL3F: _T74=s;_T75=nv;_T76=p;_T77=dec_tree;_T78=e;_T79=_T78->topt;_T7A=
# 3830
_check_null(_T79);_T7B=e;_T7C=body;_T7D=Cyc_Toc_letdecl_to_c(_T75,_T76,_T77,_T7A,_T7B,_T7C);_T74->r=_T7D->r;goto _LL3C;_LL3C:;}goto _LL2F;}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T122=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T11A;_T11E=_T122->f1;}{struct Cyc_List_List*vds=_T11E;
# 3837
struct Cyc_List_List*rvds=Cyc_List_rev(vds);
if(rvds!=0)goto _TL42B;{struct Cyc_Warn_String_Warn_Warg_struct _T122;_T122.tag=0;
_T122.f1=_tag_fat("empty Letv_d",sizeof(char),13U);_T7E=_T122;}{struct Cyc_Warn_String_Warn_Warg_struct _T122=_T7E;void*_T123[1];_T7F=_T123 + 0;*_T7F=& _T122;{int(*_T124)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T80=_T124;}_T81=_tag_fat(_T123,sizeof(void*),1);_T80(_T81);}goto _TL42C;_TL42B: _TL42C: _T82=d;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T122=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T122->tag=0;_T84=rvds;_T85=_T84->hd;
_T122->f1=(struct Cyc_Absyn_Vardecl*)_T85;_T83=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T122;}_T82->r=(void*)_T83;_T86=rvds;
rvds=_T86->tl;
_TL430: if(rvds!=0)goto _TL42E;else{goto _TL42F;}
_TL42E:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T122=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T122->tag=0;_T88=rvds;_T89=_T88->hd;_T122->f1=(struct Cyc_Absyn_Vardecl*)_T89;_T87=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T122;}_T8A=(void*)_T87;{struct Cyc_Absyn_Decl*d2=Cyc_Absyn_new_decl(_T8A,0U);_T8B=s;_T8C=d2;_T8D=s;_T8E=_T8D->r;_T8F=
Cyc_Absyn_new_stmt(_T8E,0U);_T90=Cyc_Absyn_decl_stmt(_T8C,_T8F,0U);_T8B->r=_T90->r;}_T91=rvds;
# 3842
rvds=_T91->tl;goto _TL430;_TL42F:
# 3846
 Cyc_Toc_stmt_to_c(nv,s);goto _LL2F;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T122=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T11A;_T11D=_T122->f1;}{struct Cyc_Absyn_Fndecl*fd=_T11D;
# 3849
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,body);goto _LL2F;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T122=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T11A;_T11C=_T122->f1;_T11B=_T122->f2;_T11F=_T122->f3;}{struct Cyc_Absyn_Tvar*tv=_T11C;struct Cyc_Absyn_Vardecl*vd=_T11B;struct Cyc_Absyn_Exp*open_exp_opt=_T11F;
# 3854
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple31 _T122=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T123;struct _tuple1*_T124;_T124=_T122.f0;_T123=_T122.f1;{struct _tuple1*rh_var=_T124;struct Cyc_Absyn_Exp*rh_exp=_T123;
Cyc_Absyn_set_type(rh_exp,rh_struct_ptr_typ);_T92=vd;{
struct _tuple1*x_var=_T92->name;_T93=
Cyc_Absyn_var_exp(x_var,0U);_T94=rh_struct_ptr_typ;{struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_set_type(_T93,_T94);
# 3861
Cyc_Toc_stmt_to_c(nv,body);_T95=Cyc_Flags_no_regions;
if(!_T95)goto _TL431;_T96=s;_T97=x_var;_T98=rh_struct_ptr_typ;_T99=
# 3864
Cyc_Absyn_signed_int_exp(0,0U);_T9A=Cyc_Absyn_sint_type;_T9B=Cyc_Absyn_set_type(_T99,_T9A);_T9C=body;_T9D=
# 3863
Cyc_Absyn_declare_stmt(_T97,_T98,_T9B,_T9C,0U);_T96->r=_T9D->r;goto _TL432;
# 3865
_TL431: _T9E=open_exp_opt;_T9F=(unsigned)_T9E;if(!_T9F)goto _TL433;
# 3867
Cyc_Toc_exp_to_c(nv,open_exp_opt);_TA0=open_exp_opt;{struct _fat_ptr*_T125=_cycalloc(sizeof(struct _fat_ptr));
*_T125=_tag_fat("h",sizeof(char),2U);_TA1=(struct _fat_ptr*)_T125;}{struct Cyc_Absyn_Exp*oparrow=Cyc_Absyn_aggrarrow_exp(_TA0,_TA1,0U);
Cyc_Absyn_set_type(oparrow,rh_struct_typ);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(oparrow,0U);
Cyc_Absyn_set_type(arg,rh_struct_ptr_typ);_TA2=s;_TA3=
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U);_TA2->r=_TA3->r;}}goto _TL434;
# 3879
_TL433: _TA4=Cyc_Absyn_uint_exp(0U,0U);_TA5=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*_zero_exp=Cyc_Absyn_set_type(_TA4,_TA5);_TA6=
Cyc_Absyn_uint_exp(1U,0U);_TA7=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*_one_exp=Cyc_Absyn_set_type(_TA6,_TA7);_TA9=Cyc_Flags_no_reaps;
if(!_TA9)goto _TL435;_TA8=_one_exp;goto _TL436;_TL435: _TA8=_zero_exp;_TL436: {struct Cyc_Absyn_Exp*rgn_flag_exp=_TA8;_TAA=s;_TAB=rh_var;_TAC=rh_struct_typ;{struct Cyc_Absyn_Exp*_T125[2];_TAE=_T125 + 0;
# 3883
*_TAE=rgn_flag_exp;_TAF=_T125 + 1;_TB0=x_var;_TB1=*_TB0;_TB2=_TB1.f1;_TB3=*_TB2;*_TAF=Cyc_Absyn_string_exp(_TB3,0U);_TB4=Cyc_Toc__new_region_e;_TB5=_tag_fat(_T125,sizeof(struct Cyc_Absyn_Exp*),2);_TAD=Cyc_Toc_fncall_exp_dl(_TB4,_TB5);}_TB6=rh_struct_typ;_TB7=Cyc_Absyn_set_type(_TAD,_TB6);_TB8=x_var;_TB9=rh_struct_ptr_typ;_TBA=
Cyc_Absyn_address_exp(rh_exp,0U);_TBB=rh_struct_ptr_typ;_TBC=Cyc_Absyn_set_type(_TBA,_TBB);{struct Cyc_Absyn_Exp*_T125[1];_TBE=_T125 + 0;*_TBE=x_exp;_TBF=Cyc_Toc__push_region_e;_TC0=_tag_fat(_T125,sizeof(struct Cyc_Absyn_Exp*),1);_TBD=Cyc_Toc_fncall_exp_dl(_TBF,_TC0);}_TC1=Cyc_Absyn_void_type;_TC2=
Cyc_Absyn_set_type(_TBD,_TC1);_TC3=Cyc_Absyn_exp_stmt(_TC2,0U);_TC4=body;_TC5=Cyc_Toc__pop_region_e;_TC6=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_TC7=
# 3887
Cyc_Toc_fncall_exp_dl(_TC5,_TC6);_TC8=Cyc_Absyn_void_type;_TC9=Cyc_Absyn_set_type(_TC7,_TC8);_TCA=Cyc_Absyn_exp_stmt(_TC9,0U);_TCB=
# 3886
Cyc_Absyn_seq_stmt(_TC4,_TCA,0U);_TCC=
# 3885
Cyc_Absyn_seq_stmt(_TC3,_TCB,0U);_TCD=
# 3884
Cyc_Absyn_declare_stmt(_TB8,_TB9,_TBC,_TCC,0U);_TCE=
# 3882
Cyc_Absyn_declare_stmt(_TAB,_TAC,_TB7,_TCD,0U);_TAA->r=_TCE->r;}}}_TL434: _TL432:
# 3890
 return;}}}}default:{struct Cyc_Warn_String_Warn_Warg_struct _T122;_T122.tag=0;
_T122.f1=_tag_fat("bad nested declaration within function",sizeof(char),39U);_TCF=_T122;}{struct Cyc_Warn_String_Warn_Warg_struct _T122=_TCF;void*_T123[1];_TD0=_T123 + 0;*_TD0=& _T122;_TD2=Cyc_Warn_impos2;{int(*_T124)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TD2;_TD1=_T124;}_TD3=_tag_fat(_T123,sizeof(void*),1);_TD1(_TD3);}}_LL2F:;}
# 3893
return;}default:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T11A=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T113;_T117=_T11A->f1;_T115=_T11A->f2;_TD4=_T11A->f3;_T119=(void*)_TD4;}{struct Cyc_Absyn_Stmt*body=_T117;struct Cyc_List_List*scs=_T115;void*dec_tree=_T119;
# 3907 "toc.cyc"
struct _tuple31 _T11A=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T11B;struct _tuple1*_T11C;_T11C=_T11A.f0;_T11B=_T11A.f1;{struct _tuple1*h_var=_T11C;struct Cyc_Absyn_Exp*h_exp=_T11B;
struct _tuple31 _T11D=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T11E;struct _tuple1*_T11F;_T11F=_T11D.f0;_T11E=_T11D.f1;{struct _tuple1*e_var=_T11F;struct Cyc_Absyn_Exp*e_exp=_T11E;
struct _tuple31 _T120=Cyc_Toc_temp_var_and_exp();struct Cyc_Absyn_Exp*_T121;struct _tuple1*_T122;_T122=_T120.f0;_T121=_T120.f1;{struct _tuple1*was_thrown_var=_T122;struct Cyc_Absyn_Exp*was_thrown_exp=_T121;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);_TD5=
Cyc_Absyn_exn_type();{void*e_typ=Cyc_Toc_typ_to_c(_TD5);
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);_TD6=was_thrown_exp;
_TD6->topt=was_thrown_typ;_TD7=e_exp;
# 3915
_TD7->topt=e_typ;{struct _RegionHandle _T123=_new_region(0U,"temp");struct _RegionHandle*temp=& _T123;_push_region(temp);
# 3918
Cyc_Toc_stmt_to_c(nv,body);_TD8=body;_TD9=Cyc_Toc__pop_handler_e;_TDA=_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0);_TDB=
# 3920
Cyc_Toc_fncall_exp_dl(_TD9,_TDA);_TDC=Cyc_Absyn_void_type;_TDD=Cyc_Absyn_set_type(_TDB,_TDC);_TDE=Cyc_Absyn_exp_stmt(_TDD,0U);{
# 3919
struct Cyc_Absyn_Stmt*tryandpop_stmt=Cyc_Absyn_seq_stmt(_TD8,_TDE,0U);{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T124=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_T124->tag=10;
# 3921
_T124->f1=e_exp;_T124->f2=scs;_T124->f3=dec_tree;_TDF=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T124;}_TE0=(void*)_TDF;{struct Cyc_Absyn_Stmt*handler_stmt=Cyc_Absyn_new_stmt(_TE0,0U);
# 3923
Cyc_Toc_stmt_to_c(nv,handler_stmt);{struct Cyc_Absyn_Exp*_T124[1];_TE2=_T124 + 0;_TE3=
# 3928
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_TE4=
Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);*_TE2=
# 3928
Cyc_Absyn_set_type(_TE3,_TE4);_TE5=Cyc_Toc_setjmp_e;_TE6=_tag_fat(_T124,sizeof(struct Cyc_Absyn_Exp*),1);_TE1=Cyc_Toc_fncall_exp_dl(_TE5,_TE6);}_TE7=Cyc_Absyn_sint_type;{
# 3926
struct Cyc_Absyn_Exp*setjmp_call=
Cyc_Absyn_set_type(_TE1,_TE7);{struct Cyc_Absyn_Exp*_T124[1];_TE9=_T124 + 0;_TEA=
# 3933
Cyc_Absyn_address_exp(h_exp,0U);_TEB=
Cyc_Absyn_cstar_type(h_typ,Cyc_Toc_mt_tq);*_TE9=
# 3933
Cyc_Absyn_set_type(_TEA,_TEB);_TEC=Cyc_Toc__push_handler_e;_TED=_tag_fat(_T124,sizeof(struct Cyc_Absyn_Exp*),1);_TE8=Cyc_Toc_fncall_exp_dl(_TEC,_TED);}_TEE=Cyc_Absyn_void_type;_TEF=
# 3932
Cyc_Absyn_set_type(_TE8,_TEE);{
# 3931
struct Cyc_Absyn_Stmt*pushhandler_call=
Cyc_Absyn_exp_stmt(_TEF,0U);_TF0=
# 3936
Cyc_Absyn_int_exp(0U,0,0U);_TF1=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*zero_exp=Cyc_Absyn_set_type(_TF0,_TF1);_TF2=
Cyc_Absyn_int_exp(0U,1,0U);_TF3=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*one_exp=Cyc_Absyn_set_type(_TF2,_TF3);_TF4=s;_TF5=h_var;_TF6=h_typ;_TF7=pushhandler_call;_TF8=was_thrown_var;_TF9=was_thrown_typ;_TFA=zero_exp;_TFB=setjmp_call;_TFC=
# 3943
Cyc_Absyn_copy_exp(was_thrown_exp);_TFD=one_exp;_TFE=Cyc_Absyn_assign_stmt(_TFC,_TFD,0U);_TFF=
Cyc_Toc_skip_stmt_dl();_T100=
# 3942
Cyc_Absyn_ifthenelse_stmt(_TFB,_TFE,_TFF,0U);_T101=
# 3945
Cyc_Absyn_prim1_exp(11U,was_thrown_exp,0U);_T102=Cyc_Absyn_sint_type;_T103=Cyc_Absyn_set_type(_T101,_T102);_T104=tryandpop_stmt;_T105=e_var;_T106=e_typ;_T107=e_typ;_T108=
# 3948
Cyc_Toc_get_exn_thrown_expression();_T109=Cyc_Absyn_cstar_type(Cyc_Absyn_void_type,Cyc_Toc_mt_tq);_T10A=Cyc_Absyn_set_type(_T108,_T109);_T10B=Cyc_Toc_cast_it(_T107,_T10A);_T10C=handler_stmt;_T10D=
# 3947
Cyc_Absyn_declare_stmt(_T105,_T106,_T10B,_T10C,0U);_T10E=
# 3945
Cyc_Absyn_ifthenelse_stmt(_T103,_T104,_T10D,0U);_T10F=
# 3942
Cyc_Absyn_seq_stmt(_T100,_T10E,0U);_T110=
# 3941
Cyc_Absyn_declare_stmt(_TF8,_TF9,_TFA,_T10F,0U);_T111=
# 3940
Cyc_Absyn_seq_stmt(_TF7,_T110,0U);_T112=
# 3939
Cyc_Absyn_declare_stmt(_TF5,_TF6,0,_T111,0U);
# 3938
_TF4->r=_T112->r;}}}}}}_npop_handler(0);return;_pop_region();}}}}}}};}goto _TL412;_TL414:;}
# 3958
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){struct Cyc_Absyn_Fndecl*_T0;struct Cyc_Absyn_Fndecl*_T1;struct Cyc_Absyn_Fndecl*_T2;struct Cyc_Absyn_Fndecl*_T3;struct Cyc_Absyn_Fndecl*_T4;struct Cyc_Absyn_Fndecl*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Fndecl*_T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_Fndecl*_T9;struct Cyc_Absyn_FnInfo _TA;void*_TB;struct Cyc_Absyn_Fndecl*_TC;struct Cyc_Absyn_Fndecl*_TD;void*_TE;void*_TF;struct Cyc_Absyn_Fndecl*_T10;struct Cyc_Absyn_FnInfo _T11;struct Cyc_List_List*_T12;void*_T13;struct _tuple9*_T14;struct Cyc_List_List*_T15;void*_T16;struct _tuple9*_T17;struct _tuple9 _T18;void*_T19;struct Cyc_List_List*_T1A;int _T1B;struct Cyc_Absyn_Fndecl*_T1C;struct Cyc_Absyn_Stmt*_T1D;struct Cyc_Hashtable_Table**_T1E;struct Cyc_Hashtable_Table*(*_T1F)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*);void*(*_T20)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table**_T21;struct Cyc_Hashtable_Table*_T22;struct Cyc_Absyn_Fndecl*_T23;struct Cyc_Absyn_Fndecl*_T24;struct Cyc_Absyn_FnInfo _T25;struct Cyc_Absyn_VarargInfo*_T26;unsigned _T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Absyn_FnInfo _T29;struct Cyc_Absyn_VarargInfo*_T2A;struct _fat_ptr*_T2B;struct Cyc_Absyn_Fndecl*_T2C;struct Cyc_Absyn_FnInfo _T2D;struct Cyc_Absyn_VarargInfo*_T2E;void*_T2F;struct _tuple1*_T30;struct _tuple1*_T31;struct Cyc_Absyn_Fndecl*_T32;struct Cyc_Absyn_Fndecl*_T33;struct Cyc_Absyn_FnInfo _T34;struct Cyc_List_List*_T35;struct Cyc_List_List*_T36;struct _tuple9*_T37;struct Cyc_Absyn_Fndecl*_T38;struct Cyc_Absyn_Fndecl*_T39;struct Cyc_Core_Opt*_T3A;struct Cyc_Core_Opt*_T3B;void*_T3C;struct Cyc_List_List*_T3D;void*_T3E;struct Cyc_Absyn_Vardecl*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Absyn_Vardecl*_T42;void*_T43;struct Cyc_List_List*_T44;struct Cyc_Toc_Env*_T45;struct Cyc_Absyn_Fndecl*_T46;struct Cyc_Absyn_Stmt*_T47;_T0=f;
_T0->i.tvars=0;_T1=f;
_T1->i.effect=0;_T2=f;
# 3962
_T2->i.qual_bnd=0;_T3=f;
_T3->i.checks_clause=0;_T4=f;
_T4->i.requires_clause=0;_T5=f;
_T5->i.ensures_clause=0;_T6=f;
_T6->i.throws_clause=0;_T7=f;
_T7->i.effconstr=0;_T8=f;_T9=f;_TA=_T9->i;_TB=_TA.ret_type;
_T8->i.ret_type=Cyc_Toc_typ_to_c(_TB);_TC=f;_TD=f;_TE=_TD->cached_type;_TF=
_check_null(_TE);_TC->cached_type=Cyc_Toc_typ_to_c(_TF);{struct _RegionHandle _T48=_new_region(0U,"frgn");struct _RegionHandle*frgn=& _T48;_push_region(frgn);{
# 3971
struct Cyc_Toc_Env*_T49=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_T49;_T10=f;_T11=_T10->i;{
struct Cyc_List_List*args=_T11.args;_TL43A: if(args!=0)goto _TL438;else{goto _TL439;}
_TL438: _T12=args;_T13=_T12->hd;_T14=(struct _tuple9*)_T13;_T15=args;_T16=_T15->hd;_T17=(struct _tuple9*)_T16;_T18=*_T17;_T19=_T18.f2;(*_T14).f2=Cyc_Toc_typ_to_c(_T19);_T1A=args;
# 3972
args=_T1A->tl;goto _TL43A;_TL439:;}_T1B=cinclude;
# 3976
if(!_T1B)goto _TL43B;_T1C=f;_T1D=_T1C->body;
Cyc_Toc_stmttypes_to_c(_T1D);_npop_handler(0);return;_TL43B:{struct Cyc_Hashtable_Table**_T4A=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));_T20=Cyc_Hashtable_lookup;{
# 3980
struct Cyc_Hashtable_Table*(*_T4B)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))_T20;_T1F=_T4B;}_T21=_check_null(Cyc_Toc_gpop_tables);_T22=*_T21;_T23=f;*_T4A=_T1F(_T22,_T23);_T1E=(struct Cyc_Hashtable_Table**)_T4A;}Cyc_Toc_fn_pop_table=_T1E;_T24=f;_T25=_T24->i;_T26=_T25.cyc_varargs;_T27=(unsigned)_T26;
if(!_T27)goto _TL43D;_T28=f;_T29=_T28->i;_T2A=_T29.cyc_varargs;_T2B=_T2A->name;if(_T2B==0)goto _TL43D;_T2C=f;_T2D=_T2C->i;_T2E=_T2D.cyc_varargs;{
struct Cyc_Absyn_VarargInfo _T4A=*_T2E;int _T4B;void*_T4C;struct Cyc_Absyn_Tqual _T4D;struct _fat_ptr*_T4E;_T4E=_T4A.name;_T4D=_T4A.tq;_T4C=_T4A.type;_T4B=_T4A.inject;{struct _fat_ptr*n=_T4E;struct Cyc_Absyn_Tqual tq=_T4D;void*t=_T4C;int i=_T4B;_T2F=
Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);{void*t2=Cyc_Toc_typ_to_c(_T2F);
struct _tuple1*x2;x2=_cycalloc(sizeof(struct _tuple1));_T30=x2;_T30->f0=Cyc_Absyn_Loc_n();_T31=x2;_T31->f1=n;_T32=f;_T33=f;_T34=_T33->i;_T35=_T34.args;{struct Cyc_List_List*_T4F=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple9*_T50=_cycalloc(sizeof(struct _tuple9));
_T50->f0=n;_T50->f1=tq;_T50->f2=t2;_T37=(struct _tuple9*)_T50;}_T4F->hd=_T37;_T4F->tl=0;_T36=(struct Cyc_List_List*)_T4F;}_T32->i.args=Cyc_List_append(_T35,_T36);_T38=f;
_T38->i.cyc_varargs=0;}}}goto _TL43E;_TL43D: _TL43E: _T39=f;_T3A=_T39->param_vardecls;_T3B=
# 3989
_check_null(_T3A);_T3C=_T3B->v;{struct Cyc_List_List*arg_vds=(struct Cyc_List_List*)_T3C;_TL442: if(arg_vds!=0)goto _TL440;else{goto _TL441;}
_TL440: _T3D=arg_vds;_T3E=_T3D->hd;_T3F=(struct Cyc_Absyn_Vardecl*)_T3E;_T40=arg_vds;_T41=_T40->hd;_T42=(struct Cyc_Absyn_Vardecl*)_T41;_T43=_T42->type;_T3F->type=Cyc_Toc_typ_to_c(_T43);_T44=arg_vds;
# 3989
arg_vds=_T44->tl;goto _TL442;_TL441:;}_T45=
# 3991
Cyc_Toc_clear_toplevel(frgn,nv);_T46=f;_T47=_T46->body;Cyc_Toc_stmt_to_c(_T45,_T47);}_pop_region();}}
# 3994
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){enum Cyc_Absyn_Scope _T0;int _T1;enum Cyc_Absyn_Scope _T2;_T0=s;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Abstract:
# 3996
 return 2U;case Cyc_Absyn_ExternC:
 return 3U;default: _T2=s;
return _T2;};}struct _tuple45{struct Cyc_Toc_TocState*f0;struct Cyc_Absyn_Aggrdecl**f1;};
# 4010 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple45*env){struct _tuple45*_T0;struct Cyc_Absyn_Aggrdecl**_T1;struct Cyc_Absyn_Aggrdecl*_T2;struct Cyc_Toc_TocState*_T3;struct _tuple18**(*_T4)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T5)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T6;struct Cyc_Dict_Dict _T7;struct _tuple1*_T8;struct _RegionHandle*_T9;struct _tuple18*_TA;struct Cyc_Absyn_Aggrdecl*_TB;enum Cyc_Absyn_AggrKind _TC;int _TD;struct _tuple18*_TE;struct _tuple18*_TF;struct Cyc_Dict_Dict*_T10;struct Cyc_Dict_Dict(*_T11)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*);struct Cyc_Dict_Dict(*_T12)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T13;struct Cyc_Dict_Dict _T14;struct _tuple1*_T15;struct _tuple18*_T16;struct _tuple18**_T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_AggrdeclImpl*_T19;struct Cyc_Dict_Dict*_T1A;struct Cyc_Dict_Dict(*_T1B)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*);struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T1D;struct Cyc_Dict_Dict _T1E;struct _tuple1*_T1F;struct _tuple18*_T20;struct _RegionHandle*_T21;struct Cyc_Absyn_Aggrdecl*_T22;struct Cyc_Absyn_Aggrdecl*_T23;struct Cyc_Absyn_Aggrdecl*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct Cyc_Absyn_Aggrdecl*_T26;struct Cyc_Absyn_Aggrdecl*_T27;struct Cyc_Absyn_Aggrdecl*_T28;struct Cyc_Absyn_Aggrdecl*_T29;struct Cyc_Absyn_Aggrdecl*_T2A;struct Cyc_Absyn_Aggrdecl*_T2B;struct Cyc_Absyn_Aggrdecl*_T2C;struct Cyc_Absyn_AggrdeclImpl*_T2D;int _T2E;struct Cyc_Absyn_Aggrdecl*_T2F;struct Cyc_Absyn_AggrdeclImpl*_T30;struct Cyc_Absyn_Aggrdecl*_T31;struct Cyc_Absyn_AggrdeclImpl*_T32;struct Cyc_Absyn_AggrdeclImpl*_T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_Absyn_Aggrfield*_T36;struct Cyc_Absyn_Aggrfield*_T37;struct Cyc_Absyn_Kind*_T38;struct Cyc_Absyn_Kind*_T39;struct Cyc_Absyn_Kind*_T3A;int _T3B;struct Cyc_Absyn_Aggrdecl*_T3C;enum Cyc_Absyn_AggrKind _T3D;int _T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;struct Cyc_Absyn_Aggrdecl*_T41;enum Cyc_Absyn_AggrKind _T42;int _T43;int*_T44;int _T45;struct Cyc_Absyn_ArrayInfo _T46;struct Cyc_Absyn_ArrayInfo _T47;struct Cyc_Absyn_ArrayInfo _T48;struct Cyc_Absyn_ArrayInfo _T49;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T4A;struct Cyc_Absyn_Exp*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T4E;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T4F;struct Cyc_Absyn_Exp*_T50;void*_T51;struct Cyc_Absyn_Aggrfield*_T52;struct Cyc_Absyn_Aggrfield*_T53;struct Cyc_Absyn_Aggrfield*_T54;struct Cyc_Absyn_Aggrfield*_T55;struct Cyc_Absyn_Aggrfield*_T56;struct Cyc_Absyn_Aggrfield*_T57;struct Cyc_Absyn_Aggrfield*_T58;struct Cyc_Absyn_Aggrfield*_T59;struct Cyc_Absyn_Aggrdecl*_T5A;struct Cyc_Absyn_AggrdeclImpl*_T5B;struct Cyc_Absyn_AggrdeclImpl*_T5C;int _T5D;struct Cyc_Absyn_Aggrfield*_T5E;struct Cyc_Absyn_Aggrfield*_T5F;struct _fat_ptr _T60;struct Cyc_String_pa_PrintArg_struct _T61;struct Cyc_Absyn_Aggrdecl*_T62;struct _tuple1*_T63;struct _tuple1 _T64;struct _fat_ptr*_T65;struct Cyc_String_pa_PrintArg_struct _T66;struct _fat_ptr*_T67;void**_T68;void**_T69;struct _fat_ptr _T6A;struct _fat_ptr _T6B;struct _fat_ptr*_T6C;struct Cyc_Absyn_Aggrfield*_T6D;struct Cyc_Absyn_Aggrfield*_T6E;struct Cyc_Absyn_Aggrfield*_T6F;struct Cyc_Absyn_Aggrfield*_T70;struct Cyc_Absyn_Aggrfield*_T71;struct Cyc_Absyn_Aggrfield*_T72;struct Cyc_Absyn_Aggrfield*_T73;struct Cyc_Absyn_Aggrfield*_T74;struct Cyc_Absyn_Aggrfield*_T75;struct Cyc_Absyn_Aggrfield*_T76;struct Cyc_Absyn_Aggrfield*_T77;struct Cyc_Absyn_Aggrfield*_T78;struct _fat_ptr*_T79;struct Cyc_List_List*_T7A;struct Cyc_Absyn_Aggrfield**_T7B;struct Cyc_Absyn_Aggrfield**_T7C;struct _fat_ptr _T7D;struct Cyc_List_List*_T7E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T7F;void*_T80;struct Cyc_Absyn_Aggrfield*_T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;struct Cyc_Absyn_Aggrdecl*_T84;struct Cyc_Absyn_AggrdeclImpl*_T85;int _T86;struct Cyc_List_List*_T87;struct Cyc_Absyn_Decl*_T88;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T89;_T0=env;{
# 4012
struct _tuple45 _T8A=*_T0;struct Cyc_Absyn_Aggrdecl*_T8B;struct Cyc_Toc_TocState*_T8C;_T8C=_T8A.f0;_T1=_T8A.f1;{struct Cyc_Absyn_Aggrdecl*_T8D=*_T1;_T8B=_T8D;}{struct Cyc_Toc_TocState*s=_T8C;struct Cyc_Absyn_Aggrdecl*ad=_T8B;_T2=ad;{
struct _tuple1*n=_T2->name;_T3=s;{
struct Cyc_Toc_TocState _T8D=*_T3;struct Cyc_Dict_Dict*_T8E;_T8E=_T8D.aggrs_so_far;{struct Cyc_Dict_Dict*aggrs_so_far=_T8E;
int seen_defn_before;_T5=Cyc_Dict_lookup_opt;{
struct _tuple18**(*_T8F)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T5;_T4=_T8F;}_T6=aggrs_so_far;_T7=*_T6;_T8=n;{struct _tuple18**dopt=_T4(_T7,_T8);
if(dopt!=0)goto _TL444;
seen_defn_before=0;{
struct _tuple18*v;_T9=d;v=_region_malloc(_T9,0U,sizeof(struct _tuple18));_TA=v;
_TA->f0=ad;_TB=ad;_TC=_TB->kind;_TD=(int)_TC;if(_TD!=0)goto _TL446;_TE=v;_TE->f1=Cyc_Absyn_strctq(n);goto _TL447;_TL446: _TF=v;_TF->f1=Cyc_Absyn_unionq_type(n);_TL447: _T10=aggrs_so_far;_T12=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T8F)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))_T12;_T11=_T8F;}_T13=aggrs_so_far;_T14=*_T13;_T15=n;_T16=v;*_T10=_T11(_T14,_T15,_T16);}goto _TL445;
# 4023
_TL444: _T17=dopt;{struct _tuple18*_T8F=*_T17;void*_T90;struct Cyc_Absyn_Aggrdecl*_T91;{struct _tuple18 _T92=*_T8F;_T91=_T92.f0;_T90=_T92.f1;}{struct Cyc_Absyn_Aggrdecl*ad2=_T91;void*t=_T90;_T18=ad2;_T19=_T18->impl;
if(_T19!=0)goto _TL448;_T1A=aggrs_so_far;_T1C=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T92)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))_T1C;_T1B=_T92;}_T1D=aggrs_so_far;_T1E=*_T1D;_T1F=n;_T21=d;{struct _tuple18*_T92=_region_malloc(_T21,0U,sizeof(struct _tuple18));_T92->f0=ad;_T92->f1=t;_T20=(struct _tuple18*)_T92;}*_T1A=_T1B(_T1E,_T1F,_T20);
seen_defn_before=0;goto _TL449;
# 4028
_TL448: seen_defn_before=1;_TL449:;}}_TL445: {
# 4030
struct Cyc_Absyn_Aggrdecl*new_ad;new_ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T22=new_ad;_T23=ad;_T22->kind=_T23->kind;_T24=new_ad;
_T24->sc=2U;_T25=new_ad;_T26=ad;
_T25->name=_T26->name;_T27=new_ad;
_T27->tvs=0;_T28=new_ad;
_T28->impl=0;_T29=new_ad;
_T29->expected_mem_kind=0;_T2A=new_ad;_T2B=ad;
_T2A->attributes=_T2B->attributes;_T2C=ad;_T2D=_T2C->impl;
if(_T2D==0)goto _TL44A;_T2E=seen_defn_before;if(_T2E)goto _TL44A;else{goto _TL44C;}
_TL44C: _T2F=new_ad;{struct Cyc_Absyn_AggrdeclImpl*_T8F=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_T8F->exist_vars=0;
# 4040
_T8F->qual_bnd=0;
_T8F->fields=0;
_T8F->tagged=0;
_T8F->effconstr=0;_T30=(struct Cyc_Absyn_AggrdeclImpl*)_T8F;}
# 4038
_T2F->impl=_T30;{
# 4044
struct Cyc_List_List*new_fields=0;_T31=ad;_T32=_T31->impl;_T33=
_check_null(_T32);{struct Cyc_List_List*fields=_T33->fields;_TL450: if(fields!=0)goto _TL44E;else{goto _TL44F;}
# 4048
_TL44E: _T34=fields;_T35=_T34->hd;{struct Cyc_Absyn_Aggrfield*old_field=(struct Cyc_Absyn_Aggrfield*)_T35;_T36=old_field;{
void*old_type=_T36->type;_T37=old_field;{
struct Cyc_List_List*old_atts=_T37->attributes;_T38=& Cyc_Kinds_ak;_T39=(struct Cyc_Absyn_Kind*)_T38;_T3A=
Cyc_Tcutil_type_kind(old_type);_T3B=Cyc_Kinds_kind_leq(_T39,_T3A);if(!_T3B)goto _TL451;_T3C=ad;_T3D=_T3C->kind;_T3E=(int)_T3D;if(_T3E==0)goto _TL455;else{goto _TL454;}_TL455: _T3F=fields;_T40=_T3F->tl;if(_T40==0)goto _TL453;else{goto _TL454;}_TL454: _T41=ad;_T42=_T41->kind;_T43=(int)_T42;if(_T43==1)goto _TL453;else{goto _TL451;}
# 4062 "toc.cyc"
_TL453:{void*_T8F=Cyc_Absyn_compress(old_type);unsigned _T90;void*_T91;struct Cyc_Absyn_Tqual _T92;void*_T93;_T44=(int*)_T8F;_T45=*_T44;if(_T45!=5)goto _TL456;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T94=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T8F;_T46=_T94->f1;_T93=_T46.elt_type;_T47=_T94->f1;_T92=_T47.tq;_T48=_T94->f1;_T91=_T48.zero_term;_T49=_T94->f1;_T90=_T49.zt_loc;}{void*et=_T93;struct Cyc_Absyn_Tqual tq=_T92;void*zt=_T91;unsigned ztl=_T90;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T94=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T94->tag=5;
# 4065
_T94->f1.elt_type=et;_T94->f1.tq=tq;_T4B=Cyc_Absyn_uint_exp(0U,0U);_T4C=Cyc_Absyn_uint_type;_T94->f1.num_elts=Cyc_Absyn_set_type(_T4B,_T4C);_T94->f1.zero_term=zt;_T94->f1.zt_loc=ztl;_T4A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T94;}old_type=(void*)_T4A;goto _LL9;}_TL456:{struct Cyc_List_List*_T94=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_T95=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_T95->tag=6;
# 4068
_T95->f1=0;_T4E=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_T95;}_T94->hd=(void*)_T4E;_T94->tl=old_atts;_T4D=(struct Cyc_List_List*)_T94;}old_atts=_T4D;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T94=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T94->tag=5;
_T94->f1.elt_type=Cyc_Toc_void_star_type();
_T94->f1.tq=Cyc_Toc_mt_tq;_T50=
Cyc_Absyn_uint_exp(0U,0U);_T51=Cyc_Absyn_uint_type;_T94->f1.num_elts=Cyc_Absyn_set_type(_T50,_T51);
_T94->f1.zero_term=Cyc_Absyn_false_type;_T94->f1.zt_loc=0U;_T4F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T94;}
# 4069
old_type=(void*)_T4F;_LL9:;}goto _TL452;_TL451: _TL452: {
# 4075
struct Cyc_Absyn_Aggrfield*new_field;new_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T52=new_field;_T53=old_field;_T52->name=_T53->name;_T54=new_field;
_T54->tq=Cyc_Toc_mt_tq;_T55=new_field;
_T55->type=Cyc_Toc_typ_to_c(old_type);_T56=new_field;_T57=old_field;
_T56->width=_T57->width;_T58=new_field;
_T58->attributes=Cyc_Atts_atts2c(old_atts);_T59=new_field;
_T59->requires_clause=0;_T5A=ad;_T5B=_T5A->impl;_T5C=
# 4085
_check_null(_T5B);_T5D=_T5C->tagged;if(!_T5D)goto _TL458;_T5E=new_field;{
void*T=_T5E->type;_T5F=new_field;{
struct _fat_ptr*f=_T5F->name;{struct Cyc_String_pa_PrintArg_struct _T8F;_T8F.tag=0;_T62=ad;_T63=_T62->name;_T64=*_T63;_T65=_T64.f1;
_T8F.f1=*_T65;_T61=_T8F;}{struct Cyc_String_pa_PrintArg_struct _T8F=_T61;{struct Cyc_String_pa_PrintArg_struct _T90;_T90.tag=0;_T67=f;_T90.f1=*_T67;_T66=_T90;}{struct Cyc_String_pa_PrintArg_struct _T90=_T66;void*_T91[2];_T68=_T91 + 0;*_T68=& _T8F;_T69=_T91 + 1;*_T69=& _T90;_T6A=_tag_fat("_union_%s_%s",sizeof(char),13U);_T6B=_tag_fat(_T91,sizeof(void*),2);_T60=Cyc_aprintf(_T6A,_T6B);}}{struct _fat_ptr s=_T60;
struct _fat_ptr*str;str=_cycalloc(sizeof(struct _fat_ptr));_T6C=str;*_T6C=s;{
struct Cyc_Absyn_Aggrfield*value_field;value_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T6D=value_field;_T6D->name=Cyc_Toc_val_sp;_T6E=value_field;_T6E->tq=Cyc_Toc_mt_tq;_T6F=value_field;_T6F->type=T;_T70=value_field;_T70->width=0;_T71=value_field;_T71->attributes=0;_T72=value_field;_T72->requires_clause=0;{
struct Cyc_Absyn_Aggrfield*tag_field;tag_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T73=tag_field;_T73->name=Cyc_Toc_tag_sp;_T74=tag_field;_T74->tq=Cyc_Toc_mt_tq;_T75=tag_field;_T75->type=Cyc_Absyn_sint_type;_T76=tag_field;_T76->width=0;_T77=tag_field;_T77->attributes=0;_T78=tag_field;_T78->requires_clause=0;_T79=str;{struct Cyc_Absyn_Aggrfield*_T8F[2];_T7B=_T8F + 0;
*_T7B=tag_field;_T7C=_T8F + 1;*_T7C=value_field;_T7D=_tag_fat(_T8F,sizeof(struct Cyc_Absyn_Aggrfield*),2);_T7A=Cyc_List_list(_T7D);}{struct Cyc_Absyn_Aggrdecl*ad2=Cyc_Toc_make_c_struct_defn(_T79,_T7A);{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T90=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T90->tag=5;
_T90->f1=ad2;_T7F=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T90;}_T80=(void*)_T7F;_T8F->hd=Cyc_Absyn_new_decl(_T80,0U);_T8F->tl=Cyc_Toc_result_decls;_T7E=(struct Cyc_List_List*)_T8F;}Cyc_Toc_result_decls=_T7E;_T81=new_field;
_T81->type=Cyc_Absyn_strct(str);}}}}}}goto _TL459;_TL458: _TL459:{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));
# 4096
_T8F->hd=new_field;_T8F->tl=new_fields;_T82=(struct Cyc_List_List*)_T8F;}new_fields=_T82;}}}}_T83=fields;
# 4045 "toc.cyc"
fields=_T83->tl;goto _TL450;_TL44F:;}_T84=new_ad;_T85=_T84->impl;
# 4098 "toc.cyc"
_T85->fields=Cyc_List_imp_rev(new_fields);}goto _TL44B;_TL44A: _TL44B: _T86=seen_defn_before;
# 4101
if(_T86)goto _TL45A;else{goto _TL45C;}
_TL45C:{struct Cyc_List_List*_T8F=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T90=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T91=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T91->tag=5;_T91->f1=new_ad;_T89=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T91;}_T90->r=(void*)_T89;_T90->loc=0U;_T88=(struct Cyc_Absyn_Decl*)_T90;}_T8F->hd=_T88;_T8F->tl=Cyc_Toc_result_decls;_T87=(struct Cyc_List_List*)_T8F;}Cyc_Toc_result_decls=_T87;goto _TL45B;_TL45A: _TL45B:
 return 0;}}}}}}}}
# 4105
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){int(*_T0)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple45*));struct Cyc_Absyn_Aggrdecl**_T1;{
int(*_T2)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple45*))=(int(*)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple45*)))Cyc_Toc_use_toc_state;_T0=_T2;}_T1=& ad;_T0(_T1,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple46{struct Cyc_Toc_TocState*f0;struct Cyc_Absyn_Datatypedecl*f1;};
# 4133 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple46*env){struct _tuple46*_T0;struct Cyc_Toc_TocState*_T1;struct Cyc_Absyn_Datatypedecl*_T2;struct Cyc_Absyn_Datatypedecl*_T3;struct Cyc_Core_Opt*_T4;int(*_T5)(struct Cyc_Set_Set*,struct _tuple1*);int(*_T6)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T7;struct Cyc_Set_Set*_T8;struct _tuple1*_T9;int _TA;struct Cyc_Set_Set**_TB;struct Cyc_Set_Set*(*_TC)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*);struct Cyc_Set_Set*(*_TD)(struct _RegionHandle*,struct Cyc_Set_Set*,void*);struct _RegionHandle*_TE;struct Cyc_Set_Set**_TF;struct Cyc_Set_Set*_T10;struct _tuple1*_T11;struct Cyc_Absyn_Datatypedecl*_T12;struct Cyc_Core_Opt*_T13;struct Cyc_Core_Opt*_T14;void*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Datatypefield*_T18;struct Cyc_Absyn_Aggrfield*_T19;struct Cyc_Absyn_Aggrfield*_T1A;struct Cyc_List_List*_T1B;void*_T1C;struct _tuple24*_T1D;struct _tuple24 _T1E;struct Cyc_Absyn_Aggrfield*_T1F;struct Cyc_List_List*_T20;void*_T21;struct _tuple24*_T22;struct _tuple24 _T23;void*_T24;struct Cyc_Absyn_Aggrfield*_T25;struct Cyc_Absyn_Aggrfield*_T26;struct Cyc_Absyn_Aggrfield*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_Aggrfield*_T2B;struct _fat_ptr*_T2C;struct Cyc_List_List*_T2D;struct Cyc_Absyn_Aggrdecl*_T2E;struct Cyc_Absyn_Datatypefield*_T2F;struct _tuple1*_T30;struct Cyc_Absyn_Datatypedecl*_T31;struct _tuple1*_T32;struct Cyc_List_List*_T33;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T34;void*_T35;struct Cyc_List_List*_T36;_T0=env;{
# 4136
struct _tuple46 _T37=*_T0;struct Cyc_Absyn_Datatypedecl*_T38;struct Cyc_Set_Set**_T39;_T1=_T37.f0;{struct Cyc_Toc_TocState _T3A=*_T1;_T39=_T3A.datatypes_so_far;}_T38=_T37.f1;{struct Cyc_Set_Set**datatypes_so_far=_T39;struct Cyc_Absyn_Datatypedecl*tud=_T38;_T2=tud;{
struct _tuple1*n=_T2->name;_T3=tud;_T4=_T3->fields;
if(_T4==0)goto _TL45F;else{goto _TL460;}_TL460: _T6=Cyc_Set_member;{int(*_T3A)(struct Cyc_Set_Set*,struct _tuple1*)=(int(*)(struct Cyc_Set_Set*,struct _tuple1*))_T6;_T5=_T3A;}_T7=datatypes_so_far;_T8=*_T7;_T9=n;_TA=_T5(_T8,_T9);if(_TA)goto _TL45F;else{goto _TL45D;}
_TL45F: return 0;_TL45D: _TB=datatypes_so_far;_TD=Cyc_Set_rinsert;{
struct Cyc_Set_Set*(*_T3A)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*)=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*))_TD;_TC=_T3A;}_TE=d;_TF=datatypes_so_far;_T10=*_TF;_T11=n;*_TB=_TC(_TE,_T10,_T11);_T12=tud;_T13=_T12->fields;_T14=
_check_null(_T13);_T15=_T14->v;{struct Cyc_List_List*fields=(struct Cyc_List_List*)_T15;_TL464: if(fields!=0)goto _TL462;else{goto _TL463;}
_TL462: _T16=fields;_T17=_T16->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T17;
# 4144
struct Cyc_List_List*fs=0;
int i=1;_T18=f;{
struct Cyc_List_List*ts=_T18->typs;_TL468: if(ts!=0)goto _TL466;else{goto _TL467;}
_TL466:{struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T19=f;_T19->name=Cyc_Absyn_fieldname(i);_T1A=f;_T1B=ts;_T1C=_T1B->hd;_T1D=(struct _tuple24*)_T1C;_T1E=*_T1D;_T1A->tq=_T1E.f0;_T1F=f;_T20=ts;_T21=_T20->hd;_T22=(struct _tuple24*)_T21;_T23=*_T22;_T24=_T23.f1;
_T1F->type=Cyc_Toc_typ_to_c(_T24);_T25=f;_T25->width=0;_T26=f;_T26->attributes=0;_T27=f;_T27->requires_clause=0;{struct Cyc_List_List*_T3A=_cycalloc(sizeof(struct Cyc_List_List));
_T3A->hd=f;_T3A->tl=fs;_T28=(struct Cyc_List_List*)_T3A;}fs=_T28;}_T29=ts;
# 4146
ts=_T29->tl;i=i + 1;goto _TL468;_TL467:;}{struct Cyc_List_List*_T3A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggrfield*_T3B=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));
# 4151
_T3B->name=Cyc_Toc_tag_sp;_T3B->tq=Cyc_Toc_mt_tq;_T3B->type=Cyc_Absyn_sint_type;_T3B->width=0;_T3B->attributes=0;_T3B->requires_clause=0;_T2B=(struct Cyc_Absyn_Aggrfield*)_T3B;}_T3A->hd=_T2B;
_T3A->tl=Cyc_List_imp_rev(fs);_T2A=(struct Cyc_List_List*)_T3A;}
# 4151
fs=_T2A;{struct _fat_ptr*_T3A=_cycalloc(sizeof(struct _fat_ptr));
# 4153
*_T3A=_tag_fat("",sizeof(char),1U);_T2C=(struct _fat_ptr*)_T3A;}_T2D=fs;{struct Cyc_Absyn_Aggrdecl*ad=Cyc_Toc_make_c_struct_defn(_T2C,_T2D);_T2E=ad;_T2F=f;_T30=_T2F->name;_T31=tud;_T32=_T31->name;
_T2E->name=Cyc_Toc_collapse_qvars(_T30,_T32);{struct Cyc_List_List*_T3A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T3B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T3B->tag=5;
_T3B->f1=ad;_T34=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T3B;}_T35=(void*)_T34;_T3A->hd=Cyc_Absyn_new_decl(_T35,0U);_T3A->tl=Cyc_Toc_result_decls;_T33=(struct Cyc_List_List*)_T3A;}Cyc_Toc_result_decls=_T33;}}_T36=fields;
# 4141
fields=_T36->tl;goto _TL464;_TL463:;}
# 4157
return 0;}}}}
# 4160
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){int(*_T0)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*));{
int(*_T1)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*))=(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*)))Cyc_Toc_use_toc_state;_T0=_T1;}_T0(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4179 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple46*env){struct _tuple46*_T0;struct Cyc_Absyn_Datatypedecl*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Toc_TocState*_T3;struct Cyc_Absyn_Datatypedecl*_T4;struct Cyc_Absyn_Datatypedecl*_T5;struct Cyc_Core_Opt*_T6;void*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_Absyn_Datatypefield*_TA;struct _tuple1*_TB;struct _tuple1 _TC;struct _fat_ptr*_TD;struct _fat_ptr _TE;unsigned _TF;struct Cyc_Absyn_Exp*_T10;void*_T11;int*(*_T12)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T13)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T14;struct Cyc_Dict_Dict _T15;struct Cyc_Absyn_Datatypefield*_T16;struct _tuple1*_T17;struct Cyc_Absyn_Datatypefield*_T18;enum Cyc_Absyn_Scope _T19;int _T1A;struct _fat_ptr*_T1B;struct _fat_ptr _T1C;struct Cyc_Absyn_Datatypefield*_T1D;struct _tuple1*_T1E;void*_T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_Absyn_Vardecl*_T21;struct Cyc_Absyn_Datatypefield*_T22;struct Cyc_List_List*_T23;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T24;void*_T25;struct Cyc_Dict_Dict*_T26;struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,struct _tuple1*,int);struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T29;struct Cyc_Dict_Dict _T2A;struct Cyc_Absyn_Datatypefield*_T2B;struct _tuple1*_T2C;struct Cyc_Absyn_Datatypefield*_T2D;enum Cyc_Absyn_Scope _T2E;int _T2F;int _T30;struct Cyc_Absyn_Datatypefield*_T31;struct Cyc_Absyn_Aggrfield*_T32;struct Cyc_Absyn_Aggrfield*_T33;struct Cyc_List_List*_T34;void*_T35;struct _tuple24*_T36;struct _tuple24 _T37;struct Cyc_Absyn_Aggrfield*_T38;struct Cyc_List_List*_T39;void*_T3A;struct _tuple24*_T3B;struct _tuple24 _T3C;void*_T3D;struct Cyc_Absyn_Aggrfield*_T3E;struct Cyc_Absyn_Aggrfield*_T3F;struct Cyc_Absyn_Aggrfield*_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;struct Cyc_Absyn_Aggrfield*_T44;struct _fat_ptr*_T45;struct Cyc_List_List*_T46;struct Cyc_Absyn_Aggrdecl*_T47;struct Cyc_Absyn_Datatypefield*_T48;struct _tuple1*_T49;struct Cyc_Absyn_Datatypedecl*_T4A;struct _tuple1*_T4B;struct Cyc_List_List*_T4C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T4D;void*_T4E;int*_T4F;int _T50;struct Cyc_Absyn_Datatypefield*_T51;enum Cyc_Absyn_Scope _T52;int _T53;struct _fat_ptr*_T54;struct _fat_ptr _T55;struct Cyc_Absyn_Datatypefield*_T56;struct _tuple1*_T57;void*_T58;struct Cyc_Absyn_Exp*_T59;struct Cyc_Absyn_Vardecl*_T5A;struct Cyc_Absyn_Datatypefield*_T5B;struct Cyc_List_List*_T5C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T5D;void*_T5E;struct Cyc_Dict_Dict*_T5F;struct Cyc_Dict_Dict(*_T60)(struct Cyc_Dict_Dict,struct _tuple1*,int);struct Cyc_Dict_Dict(*_T61)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T62;struct Cyc_Dict_Dict _T63;struct Cyc_Absyn_Datatypefield*_T64;struct _tuple1*_T65;struct Cyc_List_List*_T66;_T0=env;{
# 4182
struct _tuple46 _T67=*_T0;struct Cyc_Absyn_Datatypedecl*_T68;struct Cyc_Toc_TocState*_T69;_T69=_T67.f0;_T68=_T67.f1;{struct Cyc_Toc_TocState*s=_T69;struct Cyc_Absyn_Datatypedecl*xd=_T68;_T1=xd;_T2=_T1->fields;
if(_T2!=0)goto _TL469;
return 0;_TL469: _T3=s;{
struct Cyc_Toc_TocState _T6A=*_T3;struct Cyc_Dict_Dict*_T6B;_T6B=_T6A.xdatatypes_so_far;{struct Cyc_Dict_Dict*xdatatypes_so_far=_T6B;_T4=xd;{
struct _tuple1*n=_T4->name;_T5=xd;_T6=_T5->fields;_T7=_T6->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T7;_TL46E: if(fs!=0)goto _TL46C;else{goto _TL46D;}
_TL46C: _T8=fs;_T9=_T8->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T9;_TA=f;_TB=_TA->name;_TC=*_TB;{
struct _fat_ptr*fn=_TC.f1;_TD=fn;_TE=*_TD;_TF=
_get_fat_size(_TE,sizeof(char));_T10=Cyc_Absyn_uint_exp(_TF,0U);_T11=Cyc_Absyn_uint_type;{struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_set_type(_T10,_T11);
void*tag_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);_T13=Cyc_Dict_lookup_opt;{
# 4193
int*(*_T6C)(struct Cyc_Dict_Dict,struct _tuple1*)=(int*(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T13;_T12=_T6C;}_T14=xdatatypes_so_far;_T15=*_T14;_T16=f;_T17=_T16->name;{int*_T6C=_T12(_T15,_T17);if(_T6C!=0)goto _TL46F;{
# 4195
struct Cyc_Absyn_Exp*initopt=0;_T18=f;_T19=_T18->sc;_T1A=(int)_T19;
if(_T1A==3)goto _TL471;_T1B=fn;_T1C=*_T1B;
initopt=Cyc_Absyn_string_exp(_T1C,0U);goto _TL472;_TL471: _TL472: _T1D=f;_T1E=_T1D->name;_T1F=tag_typ;_T20=initopt;{
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,_T1E,_T1F,_T20,0);_T21=tag_decl;_T22=f;
_T21->sc=_T22->sc;{struct Cyc_List_List*_T6D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T6E=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T6E->tag=0;
_T6E->f1=tag_decl;_T24=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T6E;}_T25=(void*)_T24;_T6D->hd=Cyc_Absyn_new_decl(_T25,0U);_T6D->tl=Cyc_Toc_result_decls;_T23=(struct Cyc_List_List*)_T6D;}Cyc_Toc_result_decls=_T23;_T26=xdatatypes_so_far;_T28=Cyc_Dict_insert;{
# 4202
struct Cyc_Dict_Dict(*_T6D)(struct Cyc_Dict_Dict,struct _tuple1*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))_T28;_T27=_T6D;}_T29=xdatatypes_so_far;_T2A=*_T29;_T2B=f;_T2C=_T2B->name;_T2D=f;_T2E=_T2D->sc;_T2F=(int)_T2E;_T30=_T2F!=3;
# 4201
*_T26=
_T27(_T2A,_T2C,_T30);{
struct Cyc_List_List*fields=0;
int i=1;_T31=f;{
struct Cyc_List_List*tqts=_T31->typs;_TL476: if(tqts!=0)goto _TL474;else{goto _TL475;}
_TL474:{struct _fat_ptr*field_n=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*newf;newf=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T32=newf;_T32->name=field_n;_T33=newf;_T34=tqts;_T35=_T34->hd;_T36=(struct _tuple24*)_T35;_T37=*_T36;_T33->tq=_T37.f0;_T38=newf;_T39=tqts;_T3A=_T39->hd;_T3B=(struct _tuple24*)_T3A;_T3C=*_T3B;_T3D=_T3C.f1;
_T38->type=Cyc_Toc_typ_to_c(_T3D);_T3E=newf;_T3E->width=0;_T3F=newf;_T3F->attributes=0;_T40=newf;_T40->requires_clause=0;{struct Cyc_List_List*_T6D=_cycalloc(sizeof(struct Cyc_List_List));
_T6D->hd=newf;_T6D->tl=fields;_T41=(struct Cyc_List_List*)_T6D;}fields=_T41;}_T42=tqts;
# 4205
tqts=_T42->tl;i=i + 1;goto _TL476;_TL475:;}{struct Cyc_List_List*_T6D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggrfield*_T6E=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));
# 4211
_T6E->name=Cyc_Toc_tag_sp;_T6E->tq=Cyc_Toc_mt_tq;
_T6E->type=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_T6E->width=0;_T6E->attributes=0;_T6E->requires_clause=0;_T44=(struct Cyc_Absyn_Aggrfield*)_T6E;}
# 4211
_T6D->hd=_T44;
# 4213
_T6D->tl=Cyc_List_imp_rev(fields);_T43=(struct Cyc_List_List*)_T6D;}
# 4211
fields=_T43;{struct _fat_ptr*_T6D=_cycalloc(sizeof(struct _fat_ptr));
# 4214
*_T6D=_tag_fat("",sizeof(char),1U);_T45=(struct _fat_ptr*)_T6D;}_T46=fields;{struct Cyc_Absyn_Aggrdecl*strct_decl=Cyc_Toc_make_c_struct_defn(_T45,_T46);_T47=strct_decl;_T48=f;_T49=_T48->name;_T4A=xd;_T4B=_T4A->name;
_T47->name=Cyc_Toc_collapse_qvars(_T49,_T4B);{struct Cyc_List_List*_T6D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T6E=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T6E->tag=5;
_T6E->f1=strct_decl;_T4D=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T6E;}_T4E=(void*)_T4D;_T6D->hd=Cyc_Absyn_new_decl(_T4E,0U);_T6D->tl=Cyc_Toc_result_decls;_T4C=(struct Cyc_List_List*)_T6D;}Cyc_Toc_result_decls=_T4C;goto _LL6;}}}}_TL46F: _T4F=(int*)_T6C;_T50=*_T4F;if(_T50!=0)goto _TL477;_T51=f;_T52=_T51->sc;_T53=(int)_T52;
# 4219
if(_T53==3)goto _TL479;_T54=fn;_T55=*_T54;{
struct Cyc_Absyn_Exp*initopt=Cyc_Absyn_string_exp(_T55,0U);_T56=f;_T57=_T56->name;_T58=tag_typ;_T59=initopt;{
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,_T57,_T58,_T59,0);_T5A=tag_decl;_T5B=f;
_T5A->sc=_T5B->sc;{struct Cyc_List_List*_T6D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T6E=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T6E->tag=0;
_T6E->f1=tag_decl;_T5D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T6E;}_T5E=(void*)_T5D;_T6D->hd=Cyc_Absyn_new_decl(_T5E,0U);_T6D->tl=Cyc_Toc_result_decls;_T5C=(struct Cyc_List_List*)_T6D;}Cyc_Toc_result_decls=_T5C;_T5F=xdatatypes_so_far;_T61=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T6D)(struct Cyc_Dict_Dict,struct _tuple1*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))_T61;_T60=_T6D;}_T62=xdatatypes_so_far;_T63=*_T62;_T64=f;_T65=_T64->name;*_T5F=_T60(_T63,_T65,1);}}goto _TL47A;_TL479: _TL47A: goto _LL6;_TL477: goto _LL6;_LL6:;}}}}_T66=fs;
# 4187
fs=_T66->tl;goto _TL46E;_TL46D:;}
# 4230
return 0;}}}}}}
# 4233
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){int(*_T0)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*));{
int(*_T1)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*))=(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple46*)))Cyc_Toc_use_toc_state;_T0=_T1;}_T0(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4237
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl*_T0;struct Cyc_Absyn_Enumdecl*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Absyn_Enumdecl*_T3;struct Cyc_Core_Opt*_T4;void*_T5;struct Cyc_List_List*_T6;_T0=ed;
_T0->sc=2U;_T1=ed;_T2=_T1->fields;
if(_T2==0)goto _TL47B;_T3=ed;_T4=_T3->fields;_T5=_T4->v;_T6=(struct Cyc_List_List*)_T5;
Cyc_Toc_enumfields_to_c(_T6);goto _TL47C;_TL47B: _TL47C:;}
# 4243
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Vardecl*_T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_Absyn_Vardecl*_T2;enum Cyc_Absyn_Scope _T3;int _T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Absyn_Vardecl*_TA;enum Cyc_Absyn_Scope _TB;int _TC;int*_TD;int _TE;struct Cyc_Absyn_ArrayInfo _TF;struct Cyc_Absyn_ArrayInfo _T10;struct Cyc_Absyn_ArrayInfo _T11;int _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;int(*_T15)(struct _fat_ptr ap);struct _fat_ptr _T16;struct Cyc_Absyn_Vardecl*_T17;struct _tuple1*_T18;struct Cyc_Absyn_Exp*_T19;void*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_Absyn_Exp*_T21;void*_T22;struct Cyc_Absyn_Stmt*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_Stmt*_T25;struct Cyc_Absyn_Stmt*_T26;void*_T27;struct Cyc_Absyn_Stmt*_T28;_T0=vd;{
void*old_typ=_T0->type;_T1=vd;
_T1->type=Cyc_Toc_typ_to_c(old_typ);_T2=vd;_T3=_T2->sc;_T4=(int)_T3;
# 4247
if(_T4!=5)goto _TL47D;_T5=Cyc_Tcutil_is_fat_pointer_type(old_typ);if(!_T5)goto _TL47D;_T6=vd;
_T6->sc=2U;goto _TL47E;_TL47D: _TL47E:
 Cyc_Toc_stmt_to_c(nv,s);_T7=vd;_T8=_T7->initializer;
if(_T8==0)goto _TL47F;_T9=vd;{
struct Cyc_Absyn_Exp*init=_T9->initializer;_TA=vd;_TB=_TA->sc;_TC=(int)_TB;
if(_TC!=0)goto _TL481;{struct _RegionHandle _T29=_new_region(0U,"temp");struct _RegionHandle*temp=& _T29;_push_region(temp);{
# 4257
struct Cyc_Toc_Env*nv2=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(nv2,init);}_pop_region();}goto _TL482;
# 4261
_TL481: Cyc_Toc_exp_to_c(nv,init);_TL482:;}goto _TL480;
# 4264
_TL47F:{void*_T29=Cyc_Absyn_compress(old_typ);void*_T2A;struct Cyc_Absyn_Exp*_T2B;void*_T2C;_TD=(int*)_T29;_TE=*_TD;if(_TE!=5)goto _TL483;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T29;_TF=_T2D->f1;_T2C=_TF.elt_type;_T10=_T2D->f1;_T2B=_T10.num_elts;_T11=_T2D->f1;_T2A=_T11.zero_term;}{void*et=_T2C;struct Cyc_Absyn_Exp*num_elts_opt=_T2B;void*zt=_T2A;_T12=
# 4266
Cyc_Tcutil_force_type2bool(0,zt);if(!_T12)goto _TL485;
if(num_elts_opt!=0)goto _TL487;{struct Cyc_Warn_String_Warn_Warg_struct _T2D;_T2D.tag=0;
_T2D.f1=_tag_fat("can't initialize zero-terminated array -- size unknown",sizeof(char),55U);_T13=_T2D;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D=_T13;void*_T2E[1];_T14=_T2E + 0;*_T14=& _T2D;{int(*_T2F)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T15=_T2F;}_T16=_tag_fat(_T2E,sizeof(void*),1);_T15(_T16);}goto _TL488;_TL487: _TL488: _T17=vd;_T18=_T17->name;_T19=
Cyc_Absyn_var_exp(_T18,0U);_T1A=
Cyc_Toc_typ_to_c_array(old_typ);_T1B=
# 4269
Cyc_Absyn_set_type(_T19,_T1A);_T1C=num_elts_opt;_T1D=
# 4272
Cyc_Absyn_signed_int_exp(-1,0U);_T1E=Cyc_Absyn_sint_type;_T1F=Cyc_Absyn_set_type(_T1D,_T1E);_T20=
# 4271
Cyc_Absyn_add_exp(_T1C,_T1F,0U);{
# 4269
struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_subscript_exp(_T1B,_T20,0U);_T21=
# 4274
Cyc_Absyn_signed_int_exp(0,0U);_T22=Cyc_Absyn_sint_type;{struct Cyc_Absyn_Exp*rhs=Cyc_Absyn_set_type(_T21,_T22);_T23=s;_T24=
Cyc_Absyn_assign_exp(lhs,rhs,0U);_T25=Cyc_Absyn_exp_stmt(_T24,0U);_T26=s;_T27=_T26->r;_T28=
Cyc_Absyn_new_stmt(_T27,0U);
# 4275
_T23->r=Cyc_Toc_seq_stmt_r(_T25,_T28);}}goto _TL486;_TL485: _TL486: goto _LL0;}_TL483: goto _LL0;_LL0:;}_TL480:;}}
# 4285
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct Cyc_Tcpat_Rhs*_T5;void*_T6;void*_T7;void*_T8;struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T9;void**_TA;void**_TB;void**_TC;void*_TD;struct Cyc_Absyn_Stmt*_TE;struct Cyc_List_List*_TF;void*_T10;void**_T11;void**_T12;void**_T13;void*_T14;struct Cyc_Absyn_Stmt*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;void*_T1A;_T0=d;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=d;
# 4287
return _T3;case 1: _T4=d;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T1B=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T4;_T1A=_T1B->f1;}{struct Cyc_Tcpat_Rhs*rhs=_T1A;_T5=rhs;
_T5->rhs=success;_T6=d;return _T6;}default: _T7=d;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T1B=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T7;_T19=_T1B->f1;_T18=_T1B->f2;_T8=d;_T9=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T8;_TA=& _T9->f3;_T1A=(void**)_TA;}{struct Cyc_List_List*path=_T19;struct Cyc_List_List*sws=_T18;void**d2=(void**)_T1A;_TB=d2;_TC=d2;_TD=*_TC;_TE=success;
# 4290
*_TB=Cyc_Toc_rewrite_decision(_TD,_TE);
_TL48D: if(sws!=0)goto _TL48B;else{goto _TL48C;}
_TL48B: _TF=sws;_T10=_TF->hd;{struct _tuple36*_T1B=(struct _tuple36*)_T10;void*_T1C;{struct _tuple36 _T1D=*_T1B;_T11=& _T1B->f1;_T1C=(void**)_T11;}{void**d2=(void**)_T1C;_T12=d2;_T13=d2;_T14=*_T13;_T15=success;
*_T12=Cyc_Toc_rewrite_decision(_T14,_T15);}}_T16=sws;
# 4291
sws=_T16->tl;goto _TL48D;_TL48C: _T17=d;
# 4295
return _T17;}};}
# 4306 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;int*_T3;int _T4;void*_T5;struct _tuple1*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T9;void*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_Absyn_Switch_clause*_TE;struct Cyc_Absyn_Switch_clause*_TF;struct Cyc_Absyn_Switch_clause*_T10;struct Cyc_Absyn_Switch_clause*_T11;struct Cyc_Absyn_Switch_clause*_T12;struct Cyc_List_List*(*_T13)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*);struct Cyc_List_List*(*_T14)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T15;struct _RegionHandle*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Switch_clause**_T18;struct _RegionHandle*_T19;struct _fat_ptr _T1A;struct _RegionHandle*_T1B;struct Cyc_Toc_Env*_T1C;struct Cyc_List_List**_T1D;struct Cyc_List_List**_T1E;void*_T1F;struct Cyc_List_List*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_Warn_String_Warn_Warg_struct _T22;void**_T23;int(*_T24)(struct _fat_ptr ap);struct _fat_ptr _T25;struct Cyc_List_List*_T26;void*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2B;void*_T2C;struct Cyc_Absyn_Decl*_T2D;struct Cyc_Absyn_Stmt*_T2E;struct Cyc_List_List*_T2F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T30;void*_T31;struct Cyc_Absyn_Decl*_T32;struct Cyc_Absyn_Stmt*_T33;struct _RegionHandle _T34=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T34;_push_region(rgn);{
# 4309
struct Cyc_Toc_Env*_T35=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_T35;_T0=e;_T1=_T0->topt;{
void*t=_check_null(_T1);
Cyc_Toc_exp_to_c(nv,e);{
# 4313
struct Cyc_Absyn_Vardecl*v=0;
struct Cyc_Absyn_Exp*vexp=e;_T2=e;{
void*_T36=_T2->r;void*_T37;_T3=(int*)_T36;_T4=*_T3;if(_T4!=1)goto _TL48E;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T36;_T5=_T38->f1;_T37=(void*)_T5;}{void*b=_T37;goto _LL0;}_TL48E:{
# 4318
struct _tuple1*x=Cyc_Toc_temp_var();_T6=x;_T7=
Cyc_Toc_typ_to_c(t);_T8=e;v=Cyc_Absyn_new_vardecl(0U,_T6,_T7,_T8,0);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T38=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T38->tag=4;
_T38->f1=v;_T9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T38;}_TA=(void*)_T9;_TB=Cyc_Absyn_varb_exp(_TA,0U);_TC=v;_TD=_TC->type;vexp=Cyc_Absyn_set_type(_TB,_TD);goto _LL0;}_LL0:;}{
# 4323
struct Cyc_Absyn_Exp*path=Cyc_Absyn_copy_exp(vexp);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4328
struct Cyc_Absyn_Stmt*succ_stmt=s;
if(dopt==0)goto _TL490;
# 4332
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);goto _TL491;_TL490: _TL491: {
# 4334
struct Cyc_Absyn_Switch_clause*c1;c1=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE=c1;_TE->pattern=p;_TF=c1;_TF->pat_vars=0;_T10=c1;_T10->where_clause=0;_T11=c1;_T11->body=succ_stmt;_T12=c1;_T12->loc=0U;_T14=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T36)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple39*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))_T14;_T13=_T36;}_T15=rgn;_T16=rgn;{struct Cyc_Absyn_Switch_clause*_T36[1];_T18=_T36 + 0;*_T18=c1;_T19=rgn;_T1A=_tag_fat(_T36,sizeof(struct Cyc_Absyn_Switch_clause*),1);_T17=Cyc_List_rlist(_T19,_T1A);}{struct Cyc_List_List*lscs=_T13(_T15,Cyc_Toc_gen_labels,_T16,_T17);
# 4339
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;_T1B=rgn;_T1C=nv;_T1D=& mydecls;_T1E=& mybodies;_T1F=dopt;_T20=lscs;_T21=vexp;{
# 4345
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(_T1B,_T1C,_T1D,_T1E,_T1F,_T20,_T21);_TL495:
# 4349
 if(1)goto _TL493;else{goto _TL494;}
_TL493: if(mybodies!=0)goto _TL496;{struct Cyc_Warn_String_Warn_Warg_struct _T36;_T36.tag=0;
_T36.f1=_tag_fat("letdecl_to_c: couldn't find env!",sizeof(char),33U);_T22=_T36;}{struct Cyc_Warn_String_Warn_Warg_struct _T36=_T22;void*_T37[1];_T23=_T37 + 0;*_T23=& _T36;{int(*_T38)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T24=_T38;}_T25=_tag_fat(_T37,sizeof(void*),1);_T24(_T25);}goto _TL497;_TL496: _TL497: _T26=mybodies;_T27=_T26->hd;{
struct _tuple44*_T36=(struct _tuple44*)_T27;struct Cyc_Absyn_Stmt*_T37;struct Cyc_Toc_Env*_T38;{struct _tuple44 _T39=*_T36;_T38=_T39.f0;_T37=_T39.f2;}{struct Cyc_Toc_Env*env=_T38;struct Cyc_Absyn_Stmt*st=_T37;
if(st!=succ_stmt)goto _TL498;Cyc_Toc_stmt_to_c(env,s);goto _TL494;_TL498:;}}_T28=mybodies;
# 4349
mybodies=_T28->tl;goto _TL495;_TL494: {
# 4356
struct Cyc_Absyn_Stmt*res=test_tree;
# 4358
_TL49D: if(mydecls!=0)goto _TL49B;else{goto _TL49C;}
_TL49B: _T29=mydecls;_T2A=_T29->hd;{struct _tuple42*_T36=(struct _tuple42*)_T2A;struct Cyc_Absyn_Vardecl*_T37;{struct _tuple42 _T38=*_T36;_T37=_T38.f1;}{struct Cyc_Absyn_Vardecl*vd=_T37;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T38=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T38->tag=0;
_T38->f1=vd;_T2B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T38;}_T2C=(void*)_T2B;_T2D=Cyc_Absyn_new_decl(_T2C,0U);_T2E=res;res=Cyc_Absyn_decl_stmt(_T2D,_T2E,0U);}}_T2F=
# 4358
_check_null(mydecls);mydecls=_T2F->tl;goto _TL49D;_TL49C:
# 4362
 if(v==0)goto _TL49E;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T36=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T36->tag=0;
_T36->f1=v;_T30=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T36;}_T31=(void*)_T30;_T32=Cyc_Absyn_new_decl(_T31,0U);_T33=res;res=Cyc_Absyn_decl_stmt(_T32,_T33,0U);goto _TL49F;_TL49E: _TL49F: {struct Cyc_Absyn_Stmt*_T36=res;_npop_handler(0);return _T36;}}}}}}}}}_pop_region();}
# 4369
static int Cyc_Toc_types_to_c_f1(int ignore,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct _tuple9*_T3;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T4;struct _tuple9*_T5;void**_T6;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T7;void**_T8;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T9;void**_TA;struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TB;void**_TC;void**_TD;void**_TE;void*_TF;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T10;struct Cyc_Absyn_MallocInfo*_T11;struct Cyc_Absyn_MallocInfo*_T12;void**_T13;struct Cyc_Absyn_MallocInfo*_T14;void**_T15;struct Cyc_Absyn_MallocInfo*_T16;void**_T17;void*_T18;_T0=e;{
void*_T19=_T0->r;void*_T1A;_T1=(int*)_T19;_T2=*_T1;switch(_T2){case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T19;_T3=_T1B->f1;{struct _tuple9 _T1C=*_T3;_T4=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T19;_T5=_T4->f1;_T6=& _T5->f2;_T1A=(void**)_T6;}}{void**t=_T1A;_T1A=t;goto _LL4;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T19;_T7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T19;_T8=& _T7->f1;_T1A=(void**)_T8;}_LL4: {void**t=_T1A;_T1A=t;goto _LL6;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T19;_T9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T19;_TA=& _T9->f1;_T1A=(void**)_TA;}_LL6: {void**t=_T1A;_T1A=t;goto _LL8;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T19;_TB=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T19;_TC=& _TB->f1;_T1A=(void**)_TC;}_LL8: {void**t=_T1A;_TD=t;_TE=t;_TF=*_TE;
# 4374
*_TD=Cyc_Toc_typ_to_c(_TF);goto _LL0;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T19;_T10=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T19;_T11=& _T10->f1;_T1A=(struct Cyc_Absyn_MallocInfo*)_T11;}{struct Cyc_Absyn_MallocInfo*m=(struct Cyc_Absyn_MallocInfo*)_T1A;_T12=m;_T13=_T12->elt_type;
# 4376
if(_T13==0)goto _TL4A1;_T14=m;{void**_T1B=_cycalloc(sizeof(void*));_T16=m;_T17=_T16->elt_type;_T18=*_T17;
*_T1B=Cyc_Toc_typ_to_c(_T18);_T15=(void**)_T1B;}_T14->elt_type=_T15;goto _TL4A2;_TL4A1: _TL4A2: goto _LL0;}default: goto _LL0;}_LL0:;}
# 4381
return 1;}
# 4383
static int Cyc_Toc_types_to_c_f2(int ignore,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;struct Cyc_Absyn_Fndecl*_T9;struct Cyc_Absyn_Fndecl*_TA;struct Cyc_Absyn_FnInfo _TB;void*_TC;struct Cyc_Absyn_Fndecl*_TD;struct Cyc_Absyn_FnInfo _TE;struct Cyc_List_List*_TF;void*_T10;struct _tuple9*_T11;struct Cyc_List_List*_T12;void*_T13;struct _tuple9*_T14;struct _tuple9 _T15;void*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Enumdecl*_T18;struct Cyc_Core_Opt*_T19;struct Cyc_Absyn_Enumdecl*_T1A;struct Cyc_Core_Opt*_T1B;void*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_Absyn_Enumfield*_T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Absyn_Enumfield*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Typedefdecl*_T26;struct Cyc_Absyn_Typedefdecl*_T27;void*_T28;void*_T29;struct Cyc_Absyn_Stmt*_T2A;struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_T2B;_T0=s;{
void*_T2C=_T0->r;struct Cyc_Absyn_Decl*_T2D;_T1=(int*)_T2C;_T2=*_T1;switch(_T2){case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T2E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T2C;_T2D=_T2E->f1;}{struct Cyc_Absyn_Decl*d=_T2D;_T3=d;{
# 4386
void*_T2E=_T3->r;struct Cyc_Absyn_Typedefdecl*_T2F;struct Cyc_Absyn_Enumdecl*_T30;struct Cyc_Absyn_Aggrdecl*_T31;struct Cyc_Absyn_Fndecl*_T32;struct Cyc_Absyn_Vardecl*_T33;_T4=(int*)_T2E;_T5=*_T4;switch(_T5){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T34=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2E;_T33=_T34->f1;}{struct Cyc_Absyn_Vardecl*vd=_T33;_T6=vd;_T7=vd;_T8=_T7->type;
_T6->type=Cyc_Toc_typ_to_c(_T8);goto _LL7;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T34=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2E;_T32=_T34->f1;}{struct Cyc_Absyn_Fndecl*fd=_T32;_T9=fd;_TA=fd;_TB=_TA->i;_TC=_TB.ret_type;
# 4389
_T9->i.ret_type=Cyc_Toc_typ_to_c(_TC);_TD=fd;_TE=_TD->i;{
struct Cyc_List_List*args=_TE.args;_TL4A8: if(args!=0)goto _TL4A6;else{goto _TL4A7;}
_TL4A6: _TF=args;_T10=_TF->hd;_T11=(struct _tuple9*)_T10;_T12=args;_T13=_T12->hd;_T14=(struct _tuple9*)_T13;_T15=*_T14;_T16=_T15.f2;(*_T11).f2=Cyc_Toc_typ_to_c(_T16);_T17=args;
# 4390
args=_T17->tl;goto _TL4A8;_TL4A7:;}goto _LL7;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T34=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T2E;_T31=_T34->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T31;
# 4393
Cyc_Toc_aggrdecl_to_c(ad);goto _LL7;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T34=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T2E;_T30=_T34->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T30;_T18=ed;_T19=_T18->fields;
# 4395
if(_T19==0)goto _TL4A9;_T1A=ed;_T1B=_T1A->fields;_T1C=_T1B->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T1C;_TL4AE: if(fs!=0)goto _TL4AC;else{goto _TL4AD;}
_TL4AC: _T1D=fs;_T1E=_T1D->hd;_T1F=(struct Cyc_Absyn_Enumfield*)_T1E;_T20=_T1F->tag;if(_T20==0)goto _TL4AF;_T21=fs;_T22=_T21->hd;_T23=(struct Cyc_Absyn_Enumfield*)_T22;_T24=_T23->tag;Cyc_Toc_exptypes_to_c(_T24);goto _TL4B0;_TL4AF: _TL4B0: _T25=fs;
# 4396
fs=_T25->tl;goto _TL4AE;_TL4AD:;}goto _TL4AA;_TL4A9: _TL4AA: goto _LL7;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T34=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T2E;_T2F=_T34->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T2F;_T26=td;_T27=td;_T28=_T27->defn;_T29=
# 4399
_check_null(_T28);_T26->defn=Cyc_Toc_typ_to_c(_T29);goto _LL7;}default: goto _LL7;}_LL7:;}goto _LL0;}case 11: _T2A=s;{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_T2E=_cycalloc(sizeof(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct));
# 4405
_T2E->tag=0;_T2B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_T2E;}_T2A->r=(void*)_T2B;
return 0;default: goto _LL0;}_LL0:;}
# 4409
return 1;}
# 4411
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*_T2;_T1=Cyc_Absyn_visit_exp;{
void(*_T3)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*))_T1;_T0=_T3;}_T2=e;_T0(Cyc_Toc_types_to_c_f1,Cyc_Toc_types_to_c_f2,1,_T2);}
# 4414
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Absyn_Stmt*_T2;_T1=Cyc_Absyn_visit_stmt;{
void(*_T3)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T1;_T0=_T3;}_T2=s;_T0(Cyc_Toc_types_to_c_f1,Cyc_Toc_types_to_c_f2,1,_T2);}
# 4421
static void Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_Absyn_Vardecl*_T6;enum Cyc_Absyn_Scope _T7;int _T8;struct _tuple1*_T9;struct _tuple1*_TA;struct _tuple1 _TB;struct Cyc_Absyn_Vardecl*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Vardecl*_TE;enum Cyc_Absyn_Scope _TF;int _T10;struct Cyc_Absyn_Vardecl*_T11;int _T12;struct Cyc_Absyn_Vardecl*_T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Toc_Env*_T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Vardecl*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Vardecl*_T1A;enum Cyc_Absyn_Scope _T1B;struct Cyc_Absyn_Vardecl*_T1C;struct Cyc_Absyn_Vardecl*_T1D;void*_T1E;struct Cyc_List_List*_T1F;struct Cyc_Absyn_Fndecl*_T20;enum Cyc_Absyn_Scope _T21;int _T22;struct Cyc_Absyn_Fndecl*_T23;struct _tuple1*_T24;struct Cyc_Absyn_Fndecl*_T25;struct _tuple1*_T26;struct _tuple1 _T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_List_List*_T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;void**_T2B;int(*_T2C)(struct _fat_ptr ap);struct _fat_ptr _T2D;struct Cyc_Warn_String_Warn_Warg_struct _T2E;void**_T2F;int(*_T30)(struct _fat_ptr ap);struct _fat_ptr _T31;struct Cyc_Absyn_Datatypedecl*_T32;int _T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Typedefdecl*_T35;struct Cyc_Absyn_Typedefdecl*_T36;void*_T37;struct Cyc_Absyn_Typedefdecl*_T38;struct Cyc_Absyn_Typedefdecl*_T39;void*_T3A;struct Cyc_Absyn_Typedefdecl*_T3B;int*_T3C;int _T3D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T3E;struct Cyc_Absyn_TypeDecl*_T3F;struct Cyc_Absyn_TypeDecl*_T40;void*_T41;int*_T42;int _T43;struct Cyc_Absyn_TypeDecl*_T44;void*_T45;struct Cyc_Absyn_Decl*_T46;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T47;struct Cyc_Absyn_Decl*_T48;struct Cyc_List_List*_T49;struct Cyc_Absyn_Typedefdecl*_T4A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4B;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T4C;struct Cyc_Absyn_Enumdecl*_T4D;struct Cyc_Absyn_Typedefdecl*_T4E;void*_T4F;struct Cyc_Absyn_Typedefdecl*_T50;struct Cyc_Core_Opt*_T51;struct Cyc_Core_Opt*_T52;void*_T53;struct Cyc_Absyn_Kind*_T54;enum Cyc_Absyn_KindQual _T55;int _T56;int _T57;struct Cyc_List_List*_T58;struct Cyc_List_List*_T59;
_TL4B4: if(ds!=0)goto _TL4B2;else{goto _TL4B3;}
_TL4B2: Cyc_Toc_fresh_label_counter=0;_T0=ds;_T1=_T0->hd;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_T1;_T2=d;{
void*_T5A=_T2->r;struct Cyc_List_List*_T5B;struct Cyc_Absyn_Typedefdecl*_T5C;struct Cyc_Absyn_Enumdecl*_T5D;struct Cyc_Absyn_Datatypedecl*_T5E;struct Cyc_Absyn_Aggrdecl*_T5F;struct Cyc_Absyn_Fndecl*_T60;struct Cyc_Absyn_Vardecl*_T61;_T3=(int*)_T5A;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T5A;_T61=_T62->f1;}{struct Cyc_Absyn_Vardecl*vd=_T61;_T5=vd;{
# 4427
struct _tuple1*c_name=_T5->name;_T6=vd;_T7=_T6->sc;_T8=(int)_T7;
# 4429
if(_T8!=4)goto _TL4B6;{struct _tuple1*_T62=_cycalloc(sizeof(struct _tuple1));
_T62->f0=Cyc_Absyn_Abs_n(0,1);_TA=c_name;_TB=*_TA;_T62->f1=_TB.f1;_T9=(struct _tuple1*)_T62;}c_name=_T9;goto _TL4B7;_TL4B6: _TL4B7: _TC=vd;_TD=_TC->initializer;
if(_TD==0)goto _TL4B8;_TE=vd;_TF=_TE->sc;_T10=(int)_TF;
if(_T10!=4)goto _TL4BA;_T11=vd;_T11->sc=2U;goto _TL4BB;_TL4BA: _TL4BB: _T12=cinclude;
if(!_T12)goto _TL4BC;_T13=vd;_T14=_T13->initializer;
Cyc_Toc_exptypes_to_c(_T14);goto _TL4BD;
# 4436
_TL4BC: _T15=nv;_T16=vd;_T17=_T16->initializer;Cyc_Toc_exp_to_c(_T15,_T17);_TL4BD: goto _TL4B9;_TL4B8: _TL4B9: _T18=vd;
# 4438
_T18->name=c_name;_T19=vd;_T1A=vd;_T1B=_T1A->sc;
_T19->sc=Cyc_Toc_scope_to_c(_T1B);_T1C=vd;_T1D=vd;_T1E=_T1D->type;
_T1C->type=Cyc_Toc_typ_to_c(_T1E);{struct Cyc_List_List*_T62=_cycalloc(sizeof(struct Cyc_List_List));
_T62->hd=d;_T62->tl=Cyc_Toc_result_decls;_T1F=(struct Cyc_List_List*)_T62;}Cyc_Toc_result_decls=_T1F;goto _LL0;}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T5A;_T60=_T62->f1;}{struct Cyc_Absyn_Fndecl*fd=_T60;_T20=fd;_T21=_T20->sc;_T22=(int)_T21;
# 4445
if(_T22!=4)goto _TL4BE;_T23=fd;{struct _tuple1*_T62=_cycalloc(sizeof(struct _tuple1));
_T62->f0=Cyc_Absyn_Abs_n(0,1);_T25=fd;_T26=_T25->name;_T27=*_T26;_T62->f1=_T27.f1;_T24=(struct _tuple1*)_T62;}_T23->name=_T24;_T28=fd;
_T28->sc=2U;goto _TL4BF;_TL4BE: _TL4BF:
# 4449
 Cyc_Toc_fndecl_to_c(nv,fd,cinclude);{struct Cyc_List_List*_T62=_cycalloc(sizeof(struct Cyc_List_List));
_T62->hd=d;_T62->tl=Cyc_Toc_result_decls;_T29=(struct Cyc_List_List*)_T62;}Cyc_Toc_result_decls=_T29;goto _LL0;}case 2: goto _LL8;case 3: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _T62;_T62.tag=0;
# 4453
_T62.f1=_tag_fat("letdecl at toplevel",sizeof(char),20U);_T2A=_T62;}{struct Cyc_Warn_String_Warn_Warg_struct _T62=_T2A;void*_T63[1];_T2B=_T63 + 0;*_T2B=& _T62;{int(*_T64)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T2C=_T64;}_T2D=_tag_fat(_T63,sizeof(void*),1);_T2C(_T2D);}case 4:{struct Cyc_Warn_String_Warn_Warg_struct _T62;_T62.tag=0;
_T62.f1=_tag_fat("region decl at toplevel",sizeof(char),24U);_T2E=_T62;}{struct Cyc_Warn_String_Warn_Warg_struct _T62=_T2E;void*_T63[1];_T2F=_T63 + 0;*_T2F=& _T62;{int(*_T64)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_T30=_T64;}_T31=_tag_fat(_T63,sizeof(void*),1);_T30(_T31);}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T5A;_T5F=_T62->f1;}{struct Cyc_Absyn_Aggrdecl*sd=_T5F;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T5A;_T5E=_T62->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T5E;_T32=tud;_T33=_T32->is_extensible;
# 4457
if(!_T33)goto _TL4C0;Cyc_Toc_xdatatypedecl_to_c(tud);goto _TL4C1;_TL4C0: Cyc_Toc_datatypedecl_to_c(tud);_TL4C1: goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T5A;_T5D=_T62->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T5D;
# 4460
Cyc_Toc_enumdecl_to_c(ed);{struct Cyc_List_List*_T62=_cycalloc(sizeof(struct Cyc_List_List));
_T62->hd=d;_T62->tl=Cyc_Toc_result_decls;_T34=(struct Cyc_List_List*)_T62;}Cyc_Toc_result_decls=_T34;goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T5A;_T5C=_T62->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T5C;_T35=td;
# 4464
_T35->tvs=0;_T36=td;_T37=_T36->defn;
if(_T37==0)goto _TL4C2;_T38=td;_T39=td;_T3A=_T39->defn;
_T38->defn=Cyc_Toc_typ_to_c(_T3A);_T3B=td;{
# 4469
void*_T62=_T3B->defn;unsigned _T63;struct Cyc_Absyn_Enumdecl*_T64;if(_T62==0)goto _TL4C4;_T3C=(int*)_T62;_T3D=*_T3C;if(_T3D!=10)goto _TL4C6;_T3E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T62;_T3F=_T3E->f1;_T40=(struct Cyc_Absyn_TypeDecl*)_T3F;_T41=_T40->r;_T42=(int*)_T41;_T43=*_T42;if(_T43!=1)goto _TL4C8;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T65=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T62;_T44=_T65->f1;{struct Cyc_Absyn_TypeDecl _T66=*_T44;_T45=_T66.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T67=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T45;_T64=_T67->f1;}_T63=_T66.loc;}}{struct Cyc_Absyn_Enumdecl*ed2=_T64;unsigned loc=_T63;
# 4471
struct Cyc_Absyn_Decl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T46=ed;{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T65=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T65->tag=7;_T65->f1=ed2;_T47=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T65;}_T46->r=(void*)_T47;_T48=ed;_T48->loc=loc;{struct Cyc_List_List*_T65=_cycalloc(sizeof(struct Cyc_List_List));
_T65->hd=ed;_T65->tl=Cyc_Toc_result_decls;_T49=(struct Cyc_List_List*)_T65;}Cyc_Toc_result_decls=_T49;_T4A=td;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T65=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T65->tag=0;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T66=_cycalloc(sizeof(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct));_T66->tag=19;_T4D=ed2;
_T66->f1=_T4D->name;_T66->f2=ed2;_T4C=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T66;}_T65->f1=(void*)_T4C;_T65->f2=0;_T4B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T65;}_T4A->defn=(void*)_T4B;goto _LL23;}_TL4C8: goto _LL26;_TL4C6: goto _LL26;_TL4C4: _LL26: goto _LL23;_LL23:;}goto _TL4C3;
# 4478
_TL4C2: _T4E=td;_T50=td;_T51=_T50->kind;_T52=_check_null(_T51);_T53=_T52->v;_T54=(struct Cyc_Absyn_Kind*)_T53;_T55=_T54->kind;_T56=(int)_T55;if(_T56!=2)goto _TL4CA;_T4F=Cyc_Toc_void_star_type();goto _TL4CB;_TL4CA: _T4F=Cyc_Absyn_void_type;_TL4CB: _T4E->defn=_T4F;_TL4C3: _T57=Cyc_Flags_noexpand_r;
# 4482
if(!_T57)goto _TL4CC;{struct Cyc_List_List*_T62=_cycalloc(sizeof(struct Cyc_List_List));
_T62->hd=d;_T62->tl=Cyc_Toc_result_decls;_T58=(struct Cyc_List_List*)_T62;}Cyc_Toc_result_decls=_T58;goto _TL4CD;_TL4CC: _TL4CD: goto _LL0;}case 13: goto _LL16;case 14: _LL16: goto _LL18;case 15: _LL18: goto _LL1A;case 16: _LL1A: goto _LL0;case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T5A;_T5B=_T62->f2;}{struct Cyc_List_List*ds2=_T5B;_T5B=ds2;goto _LL1E;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T5A;_T5B=_T62->f2;}_LL1E: {struct Cyc_List_List*ds2=_T5B;_T5B=ds2;goto _LL20;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T5A;_T5B=_T62->f1;}_LL20: {struct Cyc_List_List*ds2=_T5B;
# 4491
Cyc_Toc_decls_to_c(nv,ds2,cinclude);goto _LL0;}default:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T62=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T5A;_T5B=_T62->f1;}{struct Cyc_List_List*ds2=_T5B;
Cyc_Toc_decls_to_c(nv,ds2,1);goto _LL0;}}_LL0:;}}_T59=ds;
# 4422
ds=_T59->tl;goto _TL4B4;_TL4B3:;}
# 4499
void Cyc_Toc_init (void){struct Cyc_Core_DynamicRegion*_T0;struct Cyc_Toc_TocStateWrap*_T1;unsigned _T2;struct _fat_ptr**_T3;struct _fat_ptr**_T4;unsigned _T5;struct _fat_ptr**_T6;struct _fat_ptr*_T7;struct _fat_ptr**_T8;struct _fat_ptr*_T9;struct _fat_ptr**_TA;struct _fat_ptr*_TB;struct _fat_ptr**_TC;struct _fat_ptr*_TD;struct _fat_ptr**_TE;struct _fat_ptr*_TF;struct _fat_ptr**_T10;struct _fat_ptr*_T11;struct _fat_ptr**_T12;struct _fat_ptr*_T13;struct _fat_ptr**_T14;struct _fat_ptr*_T15;struct _fat_ptr**_T16;struct _fat_ptr*_T17;struct _fat_ptr**_T18;struct _fat_ptr*_T19;struct _fat_ptr**_T1A;struct _fat_ptr*_T1B;struct _fat_ptr**_T1C;struct _fat_ptr*_T1D;struct _fat_ptr**_T1E;struct _fat_ptr*_T1F;struct _fat_ptr**_T20;struct _fat_ptr*_T21;struct _fat_ptr**_T22;struct _fat_ptr*_T23;struct _fat_ptr**_T24;struct _fat_ptr*_T25;struct _fat_ptr**_T26;struct _fat_ptr*_T27;struct _fat_ptr**_T28;struct _fat_ptr*_T29;struct _fat_ptr**_T2A;struct _fat_ptr*_T2B;struct _fat_ptr**_T2C;struct _fat_ptr*_T2D;struct _fat_ptr**_T2E;struct _fat_ptr*_T2F;struct _fat_ptr**_T30;struct _fat_ptr*_T31;struct _fat_ptr**_T32;struct _fat_ptr*_T33;struct _fat_ptr**_T34;struct _fat_ptr*_T35;struct _fat_ptr**_T36;struct _fat_ptr*_T37;struct _fat_ptr**_T38;struct _fat_ptr*_T39;struct _fat_ptr**_T3A;struct _fat_ptr*_T3B;struct _fat_ptr**_T3C;struct _fat_ptr*_T3D;struct _fat_ptr**_T3E;struct _fat_ptr*_T3F;struct _fat_ptr**_T40;struct _fat_ptr*_T41;struct _fat_ptr**_T42;struct _fat_ptr*_T43;struct _fat_ptr**_T44;struct _fat_ptr*_T45;struct _fat_ptr**_T46;struct _fat_ptr*_T47;struct _fat_ptr**_T48;struct _fat_ptr*_T49;struct _fat_ptr**_T4A;struct _fat_ptr*_T4B;struct _fat_ptr**_T4C;struct _fat_ptr*_T4D;struct _fat_ptr**_T4E;struct _fat_ptr*_T4F;struct _fat_ptr**_T50;struct _fat_ptr*_T51;struct _fat_ptr**_T52;struct _fat_ptr*_T53;struct _fat_ptr**_T54;struct _fat_ptr*_T55;struct _fat_ptr**_T56;struct _fat_ptr*_T57;struct _fat_ptr**_T58;struct _fat_ptr*_T59;struct _fat_ptr**_T5A;struct _fat_ptr*_T5B;struct _fat_ptr**_T5C;struct _fat_ptr*_T5D;struct _fat_ptr**_T5E;struct _fat_ptr*_T5F;
# 4501
struct Cyc_Core_NewDynamicRegion _T60=Cyc_Core__new_rckey(0U,"internal-error","internal-error",0);struct Cyc_Core_DynamicRegion*_T61;_T61=_T60.key;{struct Cyc_Core_DynamicRegion*dyn=_T61;
struct Cyc_Toc_TocState*ts;_T0=dyn;{struct _RegionHandle*h=& _T0->h;
# 4504
ts=Cyc_Toc_empty_toc_state(h);}_T2=Cyc_Core_unique_qual;{struct Cyc_Toc_TocStateWrap*_T62=_aqual_malloc(_T2,sizeof(struct Cyc_Toc_TocStateWrap));
_T62->dyn=dyn;_T62->state=ts;_T1=(struct Cyc_Toc_TocStateWrap*)_T62;}Cyc_Toc_toc_state=_T1;
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__throw_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc_setjmp_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__push_handler_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__pop_handler_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__exn_thrown_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__npop_handler_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_null_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_known_subscript_null_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_known_subscript_notnull_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_fat_subscript_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fat_ptr_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__tag_fat_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__untag_fat_ptr_check_bound_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__untag_fat_ptr_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_fat_at_base_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__get_fat_size_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fat_ptr_decrease_size_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__get_zero_arr_size_char_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__get_zero_arr_size_other_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fat_ptr_plus_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_plus_char_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_plus_other_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fat_ptr_inplace_plus_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_inplace_plus_char_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_inplace_plus_other_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fat_ptr_inplace_plus_post_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_inplace_plus_post_char_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__zero_arr_inplace_plus_post_other_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__cycalloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__cyccalloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__cycalloc_atomic_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__cyccalloc_atomic_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__region_malloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__region_calloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__region_vmalloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__aqual_malloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__aqual_calloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__fast_region_malloc_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__check_times_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__new_region_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__push_region_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__pop_region_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__throw_arraybounds_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__throw_match_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__rethrow_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{struct _union_Nmspace_C_n*_T62=& Cyc_Toc__throw_assert_pr.f0.C_n;_T62->tag=3;_T62->val=0;}{unsigned _T62=45;_T5=_check_times(_T62,sizeof(struct _fat_ptr*));{struct _fat_ptr**_T63=_cycalloc(_T5);_T6=_T63 + 0;_T7=& Cyc_Toc__throw_str;
# 4559
*_T6=(struct _fat_ptr*)_T7;_T8=_T63 + 1;_T9=& Cyc_Toc_setjmp_str;
*_T8=(struct _fat_ptr*)_T9;_TA=_T63 + 2;_TB=& Cyc_Toc__push_handler_str;
*_TA=(struct _fat_ptr*)_TB;_TC=_T63 + 3;_TD=& Cyc_Toc__pop_handler_str;
*_TC=(struct _fat_ptr*)_TD;_TE=_T63 + 4;_TF=& Cyc_Toc__exn_thrown_str;
*_TE=(struct _fat_ptr*)_TF;_T10=_T63 + 5;_T11=& Cyc_Toc__npop_handler_str;
*_T10=(struct _fat_ptr*)_T11;_T12=_T63 + 6;_T13=& Cyc_Toc__check_null_str;
*_T12=(struct _fat_ptr*)_T13;_T14=_T63 + 7;_T15=& Cyc_Toc__check_known_subscript_null_str;
*_T14=(struct _fat_ptr*)_T15;_T16=_T63 + 8;_T17=& Cyc_Toc__check_known_subscript_notnull_str;
*_T16=(struct _fat_ptr*)_T17;_T18=_T63 + 9;_T19=& Cyc_Toc__check_fat_subscript_str;
*_T18=(struct _fat_ptr*)_T19;_T1A=_T63 + 10;_T1B=& Cyc_Toc__tag_fat_str;
*_T1A=(struct _fat_ptr*)_T1B;_T1C=_T63 + 11;_T1D=& Cyc_Toc__untag_fat_ptr_check_bound_str;
*_T1C=(struct _fat_ptr*)_T1D;_T1E=_T63 + 12;_T1F=& Cyc_Toc__untag_fat_ptr_str;
*_T1E=(struct _fat_ptr*)_T1F;_T20=_T63 + 13;_T21=& Cyc_Toc__check_fat_at_base_str;
*_T20=(struct _fat_ptr*)_T21;_T22=_T63 + 14;_T23=& Cyc_Toc__get_fat_size_str;
*_T22=(struct _fat_ptr*)_T23;_T24=_T63 + 15;_T25=& Cyc_Toc__get_zero_arr_size_char_str;
*_T24=(struct _fat_ptr*)_T25;_T26=_T63 + 16;_T27=& Cyc_Toc__get_zero_arr_size_other_str;
*_T26=(struct _fat_ptr*)_T27;_T28=_T63 + 17;_T29=& Cyc_Toc__fat_ptr_plus_str;
*_T28=(struct _fat_ptr*)_T29;_T2A=_T63 + 18;_T2B=& Cyc_Toc__zero_arr_plus_char_str;
*_T2A=(struct _fat_ptr*)_T2B;_T2C=_T63 + 19;_T2D=& Cyc_Toc__zero_arr_plus_other_str;
*_T2C=(struct _fat_ptr*)_T2D;_T2E=_T63 + 20;_T2F=& Cyc_Toc__fat_ptr_inplace_plus_str;
*_T2E=(struct _fat_ptr*)_T2F;_T30=_T63 + 21;_T31=& Cyc_Toc__zero_arr_inplace_plus_char_str;
*_T30=(struct _fat_ptr*)_T31;_T32=_T63 + 22;_T33=& Cyc_Toc__zero_arr_inplace_plus_other_str;
*_T32=(struct _fat_ptr*)_T33;_T34=_T63 + 23;_T35=& Cyc_Toc__fat_ptr_inplace_plus_post_str;
*_T34=(struct _fat_ptr*)_T35;_T36=_T63 + 24;_T37=& Cyc_Toc__zero_arr_inplace_plus_post_char_str;
*_T36=(struct _fat_ptr*)_T37;_T38=_T63 + 25;_T39=& Cyc_Toc__zero_arr_inplace_plus_post_other_str;
*_T38=(struct _fat_ptr*)_T39;_T3A=_T63 + 26;_T3B=& Cyc_Toc__cycalloc_str;
*_T3A=(struct _fat_ptr*)_T3B;_T3C=_T63 + 27;_T3D=& Cyc_Toc__cyccalloc_str;
*_T3C=(struct _fat_ptr*)_T3D;_T3E=_T63 + 28;_T3F=& Cyc_Toc__cycalloc_atomic_str;
*_T3E=(struct _fat_ptr*)_T3F;_T40=_T63 + 29;_T41=& Cyc_Toc__cyccalloc_atomic_str;
*_T40=(struct _fat_ptr*)_T41;_T42=_T63 + 30;_T43=& Cyc_Toc__region_malloc_str;
*_T42=(struct _fat_ptr*)_T43;_T44=_T63 + 31;_T45=& Cyc_Toc__region_calloc_str;
*_T44=(struct _fat_ptr*)_T45;_T46=_T63 + 32;_T47=& Cyc_Toc__check_times_str;
*_T46=(struct _fat_ptr*)_T47;_T48=_T63 + 33;_T49=& Cyc_Toc__new_region_str;
*_T48=(struct _fat_ptr*)_T49;_T4A=_T63 + 34;_T4B=& Cyc_Toc__push_region_str;
*_T4A=(struct _fat_ptr*)_T4B;_T4C=_T63 + 35;_T4D=& Cyc_Toc__pop_region_str;
*_T4C=(struct _fat_ptr*)_T4D;_T4E=_T63 + 36;_T4F=& Cyc_Toc__throw_arraybounds_str;
*_T4E=(struct _fat_ptr*)_T4F;_T50=_T63 + 37;_T51=& Cyc_Toc__fat_ptr_decrease_size_str;
*_T50=(struct _fat_ptr*)_T51;_T52=_T63 + 38;_T53=& Cyc_Toc__throw_match_str;
*_T52=(struct _fat_ptr*)_T53;_T54=_T63 + 39;_T55=& Cyc_Toc__fast_region_malloc_str;
*_T54=(struct _fat_ptr*)_T55;_T56=_T63 + 40;_T57=& Cyc_Toc__region_vmalloc_str;
*_T56=(struct _fat_ptr*)_T57;_T58=_T63 + 41;_T59=& Cyc_Toc__aqual_malloc_str;
*_T58=(struct _fat_ptr*)_T59;_T5A=_T63 + 42;_T5B=& Cyc_Toc__aqual_calloc_str;
*_T5A=(struct _fat_ptr*)_T5B;_T5C=_T63 + 43;_T5D=& Cyc_Toc__throw_assert_str;
*_T5C=(struct _fat_ptr*)_T5D;_T5E=_T63 + 44;_T5F=& Cyc_Toc__rethrow_str;
*_T5E=(struct _fat_ptr*)_T5F;_T4=(struct _fat_ptr**)_T63;}_T3=_T4;}
# 4558
Cyc_Toc_globals=_tag_fat(_T3,sizeof(struct _fat_ptr*),45U);}}
# 4607
void Cyc_Toc_finish (void){struct Cyc_Toc_TocStateWrap*_T0;struct Cyc_Core_DynamicRegion*_T1;struct Cyc_Toc_TocState*_T2;void(*_T3)(struct Cyc_Toc_TocStateWrap*);void(*_T4)(void*);
struct Cyc_Toc_TocStateWrap*ts=0;{
struct Cyc_Toc_TocStateWrap*_T5=ts;struct Cyc_Toc_TocStateWrap*_T6=Cyc_Toc_toc_state;ts=_T6;Cyc_Toc_toc_state=_T5;}_T0=
_check_null(ts);{struct Cyc_Toc_TocStateWrap _T5=*_T0;struct Cyc_Toc_TocState*_T6;struct Cyc_Core_DynamicRegion*_T7;_T7=_T5.dyn;_T6=_T5.state;{struct Cyc_Core_DynamicRegion*dyn=_T7;struct Cyc_Toc_TocState*s=_T6;_T1=dyn;{struct _RegionHandle*h=& _T1->h;_T2=s;{
# 4613
struct Cyc_Toc_TocState _T8=*_T2;struct Cyc_Xarray_Xarray*_T9;_T9=_T8.temp_labels;{struct Cyc_Xarray_Xarray*tls=_T9;
Cyc_Xarray_reuse(tls);}}}
# 4617
Cyc_Core_free_rckey(dyn);_T4=Cyc_Core_ufree;{
void(*_T8)(struct Cyc_Toc_TocStateWrap*)=(void(*)(struct Cyc_Toc_TocStateWrap*))_T4;_T3=_T8;}_T3(ts);
# 4620
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4626
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){struct Cyc_Hashtable_Table**_T0;struct Cyc_Toc_Env*_T1;struct Cyc_List_List*_T2;{struct Cyc_Hashtable_Table**_T3=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));
# 4628
*_T3=pop_tables;_T0=(struct Cyc_Hashtable_Table**)_T3;}Cyc_Toc_gpop_tables=_T0;
Cyc_Toc_init();{struct _RegionHandle _T3=_new_region(0U,"start");struct _RegionHandle*start=& _T3;_push_region(start);_T1=
# 4631
Cyc_Toc_empty_env(start);_T2=ds;Cyc_Toc_decls_to_c(_T1,_T2,0);{struct Cyc_List_List*_T4=
Cyc_List_imp_rev(Cyc_Toc_result_decls);_npop_handler(0);return _T4;}_pop_region();}}
