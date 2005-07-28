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
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 328
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 706 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 883
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 890
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 898
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 901
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 904
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 907
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 911
void*Cyc_Absyn_compress(void*);
# 929
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_uint_type;
# 931
extern void*Cyc_Absyn_sint_type;
# 938
extern void*Cyc_Absyn_heap_rgn_type;
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 946
extern void*Cyc_Absyn_void_type;
# 969
void*Cyc_Absyn_exn_type (void);
# 977
extern void*Cyc_Absyn_fat_bound_type;
# 981
void*Cyc_Absyn_bounds_one (void);
# 989
void*Cyc_Absyn_star_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 993
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 995
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 997
void*Cyc_Absyn_strct(struct _fat_ptr*);
void*Cyc_Absyn_strctq(struct _tuple1*);
void*Cyc_Absyn_unionq_type(struct _tuple1*);
# 1003
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1008
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1102
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1110
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*,struct Cyc_Absyn_Stmt*,unsigned);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1119
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1123
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1128
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*,void*,struct Cyc_Absyn_Exp*);
# 1168
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1173
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1183
struct _fat_ptr*Cyc_Absyn_fieldname(int);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1185
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1187
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1193
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1196
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1199
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 1206
int Cyc_Absyn_var_may_appear_exp(struct _tuple1*,struct Cyc_Absyn_Exp*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
void*Cyc_Warn_vimpos2(struct _fat_ptr);
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 34 "flags.h"
extern int Cyc_Flags_no_regions;
# 36
extern int Cyc_Flags_no_reaps;
# 72
extern int Cyc_Flags_noexpand_r;
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 31 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
# 36
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
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 74
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 76
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 86
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 94
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 98
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 176
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 183
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*,void**,int*,void**);
# 189
int Cyc_Tcutil_is_noalias_qual(void*,int);
# 236
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 247
int Cyc_Tcutil_force_type2bool(int,void*);
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
extern struct _fat_ptr Cyc_Toc_globals;extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;void*f1;int f2;struct _fat_ptr*f3;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};char Cyc_Toc_NewInfo[8U]="NewInfo";
# 68 "toc.cyc"
static void*Cyc_Toc_unimp(struct _fat_ptr str){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=str;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_impos2(_tag_fat(_Tmp1,sizeof(void*),1));});}
# 71
static void*Cyc_Toc_toc_impos(struct _fat_ptr ap){
void*toc=(void*)({struct Cyc_Warn_String_Warn_Warg_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Warn_String_Warn_Warg_struct));_Tmp0->tag=0,_Tmp0->f1=_tag_fat("Toc: ",sizeof(char),6U);_Tmp0;});
struct _fat_ptr args=({unsigned _Tmp0=_get_fat_size(ap,sizeof(void*))+ 1U;_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=_get_fat_size(ap,sizeof(void*))+ 1U;unsigned i;for(i=0;i < _Tmp2;++ i){i==0U?_Tmp1[i]=toc:(_Tmp1[i]=*((void**)_check_fat_subscript(ap,sizeof(void*),(int)(i - 1U))));}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
Cyc_Warn_vimpos2(args);}
# 77
struct _fat_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 80
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 90
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 92
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return({int(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_Tmp0;})(*_check_null(Cyc_Toc_fn_pop_table),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f0;struct _tuple1*f1;};
# 117
static int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
int i=Cyc_Absyn_qvar_cmp((*x).f0,(*y).f0);
if(i!=0)return i;
return Cyc_Absyn_qvar_cmp((*x).f1,(*y).f1);}
# 124
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
struct Cyc_Toc_TocState*_Tmp0=_region_malloc(d,0U,sizeof(struct Cyc_Toc_TocState));
({struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->tuple_types=_Tmp1;}),({
struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->anon_aggr_types=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;_Tmp4;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->aggrs_so_far=_Tmp1;}),({
struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_List_List*));*_Tmp2=0;_Tmp2;});_Tmp0->abs_struct_types=_Tmp1;}),({
struct Cyc_Set_Set**_Tmp1=({struct Cyc_Set_Set**_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp3=({struct Cyc_Set_Set*(*_Tmp4)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty;_Tmp4;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->datatypes_so_far=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty;_Tmp4;})(d,Cyc_Absyn_qvar_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->xdatatypes_so_far=_Tmp1;}),({
struct Cyc_Dict_Dict*_Tmp1=({struct Cyc_Dict_Dict*_Tmp2=_region_malloc(d,0U,sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty;_Tmp4;})(d,Cyc_Toc_qvar_tag_cmp);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->qvar_tags=_Tmp1;}),({
struct Cyc_Xarray_Xarray*_Tmp1=Cyc_Xarray_rcreate_empty(d);_Tmp0->temp_labels=_Tmp1;});return _Tmp0;}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 144
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f0;void*f1;};
# 150
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 153
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_Tmp0=ts;struct Cyc_Toc_TocStateWrap*_Tmp1=Cyc_Toc_toc_state;ts=_Tmp1;Cyc_Toc_toc_state=_Tmp0;});{
struct Cyc_Toc_TocStateWrap _Tmp0=*_check_null(ts);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.state;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Toc_TocState*s=_Tmp1;
struct _fat_ptr dyn2=Cyc_Core_alias_refptr(_tag_fat(dyn,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _Tmp3=({struct Cyc_Toc_TocStateWrap _Tmp4;_Tmp4.dyn=dyn,_Tmp4.state=s;_Tmp4;});*ts=_Tmp3;});
({struct Cyc_Toc_TocStateWrap*_Tmp3=ts;struct Cyc_Toc_TocStateWrap*_Tmp4=Cyc_Toc_toc_state;ts=_Tmp4;Cyc_Toc_toc_state=_Tmp3;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_fat_ptr(_check_fat_at_base(dyn2),sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple15 env=({struct _tuple15 _Tmp3;_Tmp3.f0=s,_Tmp3.f1=arg;_Tmp3;});
res=f(h,& env);}
# 161
;}
# 164
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_fat_ptr(_check_fat_at_base(dyn2),sizeof(struct Cyc_Core_DynamicRegion),1U));
return res;}}}}struct _tuple16{struct _tuple1*f0;void*(*f1)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f0;struct _tuple16*f1;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f0;void*f1;};
# 168
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 171
struct _tuple17 _Tmp0=*env;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1->f0;_Tmp1=_Tmp0.f1->f1;{struct Cyc_Toc_TocState*s=_Tmp3;struct _tuple1*q=_Tmp2;void*(*type_maker)(struct _tuple1*)=_Tmp1;
struct _tuple18**v=({struct _tuple18**(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp4;})(*s->aggrs_so_far,q);
if(v==0)return type_maker(q);else{return(*(*v)).f1;}}}
# 176
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 env=({struct _tuple16 _Tmp0;_Tmp0.f0=q,_Tmp0.f1=type_maker;_Tmp0;});
return({void*(*_Tmp0)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*))=(void*(*)(struct _tuple16*,void*(*)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state;_Tmp0;})(& env,Cyc_Toc_aggrdecl_type_body);}static char _TmpG0[5U]="curr";
# 189 "toc.cyc"
static struct _fat_ptr Cyc_Toc_curr_string={_TmpG0,_TmpG0,_TmpG0 + 5U};static struct _fat_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _TmpG1[4U]="tag";
static struct _fat_ptr Cyc_Toc_tag_string={_TmpG1,_TmpG1,_TmpG1 + 4U};static struct _fat_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _TmpG2[4U]="val";
static struct _fat_ptr Cyc_Toc_val_string={_TmpG2,_TmpG2,_TmpG2 + 4U};static struct _fat_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _TmpG3[14U]="_handler_cons";
static struct _fat_ptr Cyc_Toc__handler_cons_string={_TmpG3,_TmpG3,_TmpG3 + 14U};static struct _fat_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _TmpG4[8U]="handler";
static struct _fat_ptr Cyc_Toc_handler_string={_TmpG4,_TmpG4,_TmpG4 + 8U};static struct _fat_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _TmpG5[14U]="_RegionHandle";
static struct _fat_ptr Cyc_Toc__RegionHandle_string={_TmpG5,_TmpG5,_TmpG5 + 14U};static struct _fat_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _TmpG6[7U]="_throw";
# 208 "toc.cyc"
static struct _fat_ptr Cyc_Toc__throw_str={_TmpG6,_TmpG6,_TmpG6 + 7U};static struct _tuple1 Cyc_Toc__throw_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _TmpG7[7U]="setjmp";
static struct _fat_ptr Cyc_Toc_setjmp_str={_TmpG7,_TmpG7,_TmpG7 + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _TmpG8[14U]="_push_handler";
static struct _fat_ptr Cyc_Toc__push_handler_str={_TmpG8,_TmpG8,_TmpG8 + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _TmpG9[13U]="_pop_handler";
static struct _fat_ptr Cyc_Toc__pop_handler_str={_TmpG9,_TmpG9,_TmpG9 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _TmpGA[12U]="_exn_thrown";
static struct _fat_ptr Cyc_Toc__exn_thrown_str={_TmpGA,_TmpGA,_TmpGA + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _TmpGB[14U]="_npop_handler";
static struct _fat_ptr Cyc_Toc__npop_handler_str={_TmpGB,_TmpGB,_TmpGB + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _TmpGC[12U]="_check_null";
static struct _fat_ptr Cyc_Toc__check_null_str={_TmpGC,_TmpGC,_TmpGC + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _TmpGD[28U]="_check_known_subscript_null";
static struct _fat_ptr Cyc_Toc__check_known_subscript_null_str={_TmpGD,_TmpGD,_TmpGD + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _TmpGE[31U]="_check_known_subscript_notnull";
static struct _fat_ptr Cyc_Toc__check_known_subscript_notnull_str={_TmpGE,_TmpGE,_TmpGE + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _TmpGF[21U]="_check_fat_subscript";
static struct _fat_ptr Cyc_Toc__check_fat_subscript_str={_TmpGF,_TmpGF,_TmpGF + 21U};static struct _tuple1 Cyc_Toc__check_fat_subscript_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_fat_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_subscript_bnd={0,& Cyc_Toc__check_fat_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_subscript_re={1,(void*)& Cyc_Toc__check_fat_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_subscript_ev={0,(void*)& Cyc_Toc__check_fat_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_subscript_e=& Cyc_Toc__check_fat_subscript_ev;static char _TmpG10[9U]="_fat_ptr";
static struct _fat_ptr Cyc_Toc__fat_ptr_str={_TmpG10,_TmpG10,_TmpG10 + 9U};static struct _tuple1 Cyc_Toc__fat_ptr_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_bnd={0,& Cyc_Toc__fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_re={1,(void*)& Cyc_Toc__fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_ev={0,(void*)& Cyc_Toc__fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_e=& Cyc_Toc__fat_ptr_ev;static char _TmpG11[9U]="_tag_fat";
static struct _fat_ptr Cyc_Toc__tag_fat_str={_TmpG11,_TmpG11,_TmpG11 + 9U};static struct _tuple1 Cyc_Toc__tag_fat_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__tag_fat_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_fat_bnd={0,& Cyc_Toc__tag_fat_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_fat_re={1,(void*)& Cyc_Toc__tag_fat_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_fat_ev={0,(void*)& Cyc_Toc__tag_fat_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_fat_e=& Cyc_Toc__tag_fat_ev;static char _TmpG12[15U]="_untag_fat_ptr";
static struct _fat_ptr Cyc_Toc__untag_fat_ptr_str={_TmpG12,_TmpG12,_TmpG12 + 15U};static struct _tuple1 Cyc_Toc__untag_fat_ptr_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__untag_fat_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_fat_ptr_bnd={0,& Cyc_Toc__untag_fat_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_fat_ptr_re={1,(void*)& Cyc_Toc__untag_fat_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_fat_ptr_ev={0,(void*)& Cyc_Toc__untag_fat_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_fat_ptr_e=& Cyc_Toc__untag_fat_ptr_ev;static char _TmpG13[19U]="_check_fat_at_base";
static struct _fat_ptr Cyc_Toc__check_fat_at_base_str={_TmpG13,_TmpG13,_TmpG13 + 19U};static struct _tuple1 Cyc_Toc__check_fat_at_base_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_fat_at_base_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_fat_at_base_bnd={0,& Cyc_Toc__check_fat_at_base_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_fat_at_base_re={1,(void*)& Cyc_Toc__check_fat_at_base_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_fat_at_base_ev={0,(void*)& Cyc_Toc__check_fat_at_base_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_fat_at_base_e=& Cyc_Toc__check_fat_at_base_ev;static char _TmpG14[14U]="_get_fat_size";
static struct _fat_ptr Cyc_Toc__get_fat_size_str={_TmpG14,_TmpG14,_TmpG14 + 14U};static struct _tuple1 Cyc_Toc__get_fat_size_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__get_fat_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_fat_size_bnd={0,& Cyc_Toc__get_fat_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_fat_size_re={1,(void*)& Cyc_Toc__get_fat_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_fat_size_ev={0,(void*)& Cyc_Toc__get_fat_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_fat_size_e=& Cyc_Toc__get_fat_size_ev;static char _TmpG15[23U]="_fat_ptr_decrease_size";
static struct _fat_ptr Cyc_Toc__fat_ptr_decrease_size_str={_TmpG15,_TmpG15,_TmpG15 + 23U};static struct _tuple1 Cyc_Toc__fat_ptr_decrease_size_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fat_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_decrease_size_bnd={0,& Cyc_Toc__fat_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_decrease_size_re={1,(void*)& Cyc_Toc__fat_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__fat_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_decrease_size_e=& Cyc_Toc__fat_ptr_decrease_size_ev;static char _TmpG16[24U]="_get_zero_arr_size_char";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_char_str={_TmpG16,_TmpG16,_TmpG16 + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _TmpG17[25U]="_get_zero_arr_size_other";
static struct _fat_ptr Cyc_Toc__get_zero_arr_size_other_str={_TmpG17,_TmpG17,_TmpG17 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_other_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__get_zero_arr_size_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_other_bnd={0,& Cyc_Toc__get_zero_arr_size_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_other_re={1,(void*)& Cyc_Toc__get_zero_arr_size_other_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_other_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_other_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_other_e=& Cyc_Toc__get_zero_arr_size_other_ev;static char _TmpG18[14U]="_fat_ptr_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_plus_str={_TmpG18,_TmpG18,_TmpG18 + 14U};static struct _tuple1 Cyc_Toc__fat_ptr_plus_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fat_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_plus_bnd={0,& Cyc_Toc__fat_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_plus_re={1,(void*)& Cyc_Toc__fat_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_plus_e=& Cyc_Toc__fat_ptr_plus_ev;static char _TmpG19[20U]="_zero_arr_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_char_str={_TmpG19,_TmpG19,_TmpG19 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _TmpG1A[21U]="_zero_arr_plus_other";
static struct _fat_ptr Cyc_Toc__zero_arr_plus_other_str={_TmpG1A,_TmpG1A,_TmpG1A + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_other_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_plus_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_other_bnd={0,& Cyc_Toc__zero_arr_plus_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_other_re={1,(void*)& Cyc_Toc__zero_arr_plus_other_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_other_ev={0,(void*)& Cyc_Toc__zero_arr_plus_other_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_other_e=& Cyc_Toc__zero_arr_plus_other_ev;static char _TmpG1B[22U]="_fat_ptr_inplace_plus";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_str={_TmpG1B,_TmpG1B,_TmpG1B + 22U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fat_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_e=& Cyc_Toc__fat_ptr_inplace_plus_ev;static char _TmpG1C[28U]="_zero_arr_inplace_plus_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_TmpG1C,_TmpG1C,_TmpG1C + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _TmpG1D[29U]="_zero_arr_inplace_plus_other";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_other_str={_TmpG1D,_TmpG1D,_TmpG1D + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_other_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_inplace_plus_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_other_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_other_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_other_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_other_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_other_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_other_e=& Cyc_Toc__zero_arr_inplace_plus_other_ev;static char _TmpG1E[27U]="_fat_ptr_inplace_plus_post";
static struct _fat_ptr Cyc_Toc__fat_ptr_inplace_plus_post_str={_TmpG1E,_TmpG1E,_TmpG1E + 27U};static struct _tuple1 Cyc_Toc__fat_ptr_inplace_plus_post_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fat_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fat_ptr_inplace_plus_post_bnd={0,& Cyc_Toc__fat_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fat_ptr_inplace_plus_post_re={1,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fat_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__fat_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fat_ptr_inplace_plus_post_e=& Cyc_Toc__fat_ptr_inplace_plus_post_ev;static char _TmpG1F[33U]="_zero_arr_inplace_plus_post_char";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_TmpG1F,_TmpG1F,_TmpG1F + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _TmpG20[34U]="_zero_arr_inplace_plus_post_other";
static struct _fat_ptr Cyc_Toc__zero_arr_inplace_plus_post_other_str={_TmpG20,_TmpG20,_TmpG20 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_other_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__zero_arr_inplace_plus_post_other_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_other_bnd={0,& Cyc_Toc__zero_arr_inplace_plus_post_other_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_other_re={1,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_other_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_other_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_other_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_other_e=& Cyc_Toc__zero_arr_inplace_plus_post_other_ev;static char _TmpG21[10U]="_cycalloc";
static struct _fat_ptr Cyc_Toc__cycalloc_str={_TmpG21,_TmpG21,_TmpG21 + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _TmpG22[11U]="_cyccalloc";
static struct _fat_ptr Cyc_Toc__cyccalloc_str={_TmpG22,_TmpG22,_TmpG22 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _TmpG23[17U]="_cycalloc_atomic";
static struct _fat_ptr Cyc_Toc__cycalloc_atomic_str={_TmpG23,_TmpG23,_TmpG23 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _TmpG24[18U]="_cyccalloc_atomic";
static struct _fat_ptr Cyc_Toc__cyccalloc_atomic_str={_TmpG24,_TmpG24,_TmpG24 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _TmpG25[15U]="_region_malloc";
static struct _fat_ptr Cyc_Toc__region_malloc_str={_TmpG25,_TmpG25,_TmpG25 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _TmpG26[15U]="_region_calloc";
static struct _fat_ptr Cyc_Toc__region_calloc_str={_TmpG26,_TmpG26,_TmpG26 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _TmpG27[16U]="_region_vmalloc";
static struct _fat_ptr Cyc_Toc__region_vmalloc_str={_TmpG27,_TmpG27,_TmpG27 + 16U};static struct _tuple1 Cyc_Toc__region_vmalloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__region_vmalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_vmalloc_bnd={0,& Cyc_Toc__region_vmalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_vmalloc_re={1,(void*)& Cyc_Toc__region_vmalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_vmalloc_ev={0,(void*)& Cyc_Toc__region_vmalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_vmalloc_e=& Cyc_Toc__region_vmalloc_ev;static char _TmpG28[14U]="_aqual_malloc";
static struct _fat_ptr Cyc_Toc__aqual_malloc_str={_TmpG28,_TmpG28,_TmpG28 + 14U};static struct _tuple1 Cyc_Toc__aqual_malloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__aqual_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__aqual_malloc_bnd={0,& Cyc_Toc__aqual_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__aqual_malloc_re={1,(void*)& Cyc_Toc__aqual_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__aqual_malloc_ev={0,(void*)& Cyc_Toc__aqual_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__aqual_malloc_e=& Cyc_Toc__aqual_malloc_ev;static char _TmpG29[14U]="_aqual_calloc";
static struct _fat_ptr Cyc_Toc__aqual_calloc_str={_TmpG29,_TmpG29,_TmpG29 + 14U};static struct _tuple1 Cyc_Toc__aqual_calloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__aqual_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__aqual_calloc_bnd={0,& Cyc_Toc__aqual_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__aqual_calloc_re={1,(void*)& Cyc_Toc__aqual_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__aqual_calloc_ev={0,(void*)& Cyc_Toc__aqual_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__aqual_calloc_e=& Cyc_Toc__aqual_calloc_ev;static char _TmpG2A[20U]="_fast_region_malloc";
static struct _fat_ptr Cyc_Toc__fast_region_malloc_str={_TmpG2A,_TmpG2A,_TmpG2A + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;static char _TmpG2B[13U]="_check_times";
static struct _fat_ptr Cyc_Toc__check_times_str={_TmpG2B,_TmpG2B,_TmpG2B + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _TmpG2C[12U]="_new_region";
static struct _fat_ptr Cyc_Toc__new_region_str={_TmpG2C,_TmpG2C,_TmpG2C + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _TmpG2D[13U]="_push_region";
static struct _fat_ptr Cyc_Toc__push_region_str={_TmpG2D,_TmpG2D,_TmpG2D + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _TmpG2E[12U]="_pop_region";
static struct _fat_ptr Cyc_Toc__pop_region_str={_TmpG2E,_TmpG2E,_TmpG2E + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _TmpG2F[19U]="_throw_arraybounds";
static struct _fat_ptr Cyc_Toc__throw_arraybounds_str={_TmpG2F,_TmpG2F,_TmpG2F + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _TmpG30[13U]="_throw_match";
static struct _fat_ptr Cyc_Toc__throw_match_str={_TmpG30,_TmpG30,_TmpG30 + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _TmpG31[9U]="_rethrow";
static struct _fat_ptr Cyc_Toc__rethrow_str={_TmpG31,_TmpG31,_TmpG31 + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={.f0={.Loc_n={4,0}},.f1=& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;
# 256
struct Cyc_Absyn_Exp*Cyc_Toc_get_exn_thrown_expression (void){
static struct Cyc_Absyn_Exp*_get_exn_thrown_e=0;
# 259
if((unsigned)_get_exn_thrown_e)
return _get_exn_thrown_e;{
struct _tuple1*qv;qv=_cycalloc(sizeof(struct _tuple1)),qv->f0.Abs_n.tag=2U,({struct Cyc_List_List*_Tmp0=({struct _fat_ptr*_Tmp1[1];({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=_tag_fat("Core",sizeof(char),5U);_Tmp3;});_Tmp1[0]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct _fat_ptr*),1));});qv->f0.Abs_n.val=_Tmp0;}),({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=_tag_fat("get_exn_thrown",sizeof(char),15U);_Tmp1;});qv->f1=_Tmp0;});{
void*bnd=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_Tmp0->tag=0,_Tmp0->f1=qv;_Tmp0;});
struct Cyc_Absyn_Exp*fnname;fnname=_cycalloc(sizeof(struct Cyc_Absyn_Exp)),fnname->topt=0,({void*_Tmp0=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp1->tag=1,_Tmp1->f1=bnd;_Tmp1;});fnname->r=_Tmp0;}),fnname->loc=0U,fnname->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;{
void*fncall_re=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp0->tag=10,_Tmp0->f1=fnname,_Tmp0->f2=0,_Tmp0->f3=0,_Tmp0->f4=0;_Tmp0;});
_get_exn_thrown_e=({struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=0,_Tmp0->r=fncall_re,_Tmp0->loc=0U,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;_Tmp0;});
return _get_exn_thrown_e;}}}}
# 269
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 271
void*Cyc_Toc_void_star_type (void){
static void*t=0;
if(t==0)
t=({void*_Tmp0=Cyc_Absyn_void_type;void*_Tmp1=Cyc_Absyn_heap_rgn_type;void*_Tmp2=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);void*_Tmp4=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Absyn_false_type);});
return t;}
# 277
static void*Cyc_Toc_fat_ptr_type (void){
static void*t=0;
if(t==0)
t=Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(0U,& Cyc_Toc__fat_ptr_pr,0),0);
return t;}
# 283
static void*Cyc_Toc_rgn_type (void){
static void*r=0;
if(r==0)
r=({void*_Tmp0=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_type(_Tmp0,Cyc_Toc_mt_tq);});
return r;}
# 289
static void*Cyc_Toc_aqualt_type (void){
return Cyc_Absyn_uint_type;}
# 292
static struct Cyc_Absyn_Exp*Cyc_Toc_aliasable_qual_exp (void){
static struct Cyc_Absyn_Exp*aq=0;
if(aq==0)
aq=Cyc_Absyn_uint_exp(0U,0U);
return aq;}
# 298
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl (void){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 302
static struct _tuple19*Cyc_Toc_make_field(struct _fat_ptr*name,struct Cyc_Absyn_Exp*e){
struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp4->tag=1,_Tmp4->f1=name;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});_Tmp0->f0=_Tmp1;}),_Tmp0->f1=e;return _Tmp0;}
# 306
static struct Cyc_Absyn_Exp*Cyc_Toc_fncall_exp_dl(struct Cyc_Absyn_Exp*f,struct _fat_ptr args){
struct Cyc_Absyn_Exp*_Tmp0=f;return Cyc_Absyn_fncall_exp(_Tmp0,Cyc_List_from_array(args),0U);}
# 309
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==14){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4==Cyc_Absyn_No_coercion){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp1;
return Cyc_Toc_cast_it(t,e);}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_cast_exp(t,e,0,1U,0U);};}
# 318
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp0->tag=14,_Tmp0->f1=t,_Tmp0->f2=e,_Tmp0->f3=0,_Tmp0->f4=1U;_Tmp0;});}
# 321
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct));_Tmp0->tag=20,_Tmp0->f1=e;_Tmp0;});}
# 324
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct));_Tmp0->tag=23,_Tmp0->f1=e1,_Tmp0->f2=e2;_Tmp0;});}
# 327
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_Tmp0->tag=36,_Tmp0->f1=s;_Tmp0;});}
# 330
static void*Cyc_Toc_sizeoftype_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_Tmp0->tag=17,_Tmp0->f1=t;_Tmp0;});}
# 333
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp0->tag=10,_Tmp0->f1=e,({struct Cyc_List_List*_Tmp1=Cyc_List_from_array(es);_Tmp0->f2=_Tmp1;}),_Tmp0->f3=0,_Tmp0->f4=1;_Tmp0;});}
# 336
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_Tmp0->tag=2,_Tmp0->f1=s1,_Tmp0->f2=s2;_Tmp0;});}
# 339
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct));_Tmp0->tag=6,_Tmp0->f1=e1,_Tmp0->f2=e2,_Tmp0->f3=e3;_Tmp0;});}
# 342
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_Tmp0->tag=21,_Tmp0->f1=e,_Tmp0->f2=n,_Tmp0->f3=0,_Tmp0->f4=0;_Tmp0;});}
# 345
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 348
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp0->tag=35,_Tmp0->f1=tdopt,_Tmp0->f2=ds;_Tmp0;});}
# 350
static void*Cyc_Toc_goto_stmt_r(struct _fat_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct));_Tmp0->tag=8,_Tmp0->f1=v;_Tmp0;});}
# 354
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{.f0=Cyc_Absyn_Signed,.f1=0}}}};
# 358
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==20){_Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}
# 365
static int Cyc_Toc_isCharStar(struct Cyc_Absyn_Exp*e){
void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));struct Cyc_Absyn_PtrInfo _Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo pi=_Tmp1;
return Cyc_Tcutil_is_char_type(pi.elt_type);}}else{
return 0;};}
# 375
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Null_c.tag==1)
return 1;else{goto _LLF;}case 14: _Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Toc_is_zero(e1);}case 25: _Tmp1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LL8;}case 29: _Tmp1=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL8: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLA;}case 28: _Tmp1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LLA: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLC;}case 24: _Tmp1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LLE;}case 35: _Tmp1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_List_List*dles=_Tmp1;
# 384
for(1;dles!=0;dles=dles->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)dles->hd)).f1))return 0;}
return 1;}default: _LLF:
 return Cyc_Tcutil_is_zero(e);};}
# 392
static struct _fat_ptr Cyc_Toc_collapse_qvar(struct _fat_ptr*s,struct _tuple1*x){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=x->f0;_Tmp0=x->f1;{union Cyc_Absyn_Nmspace ns=_Tmp1;struct _fat_ptr*v=_Tmp0;
void*_Tmp2;switch(ns.Abs_n.tag){case 4:
 _Tmp2=0;goto _LL7;case 1: _Tmp2=ns.Rel_n.val;_LL7: {struct Cyc_List_List*vs=_Tmp2;
_Tmp2=vs;goto _LL9;}case 2: _Tmp2=ns.Abs_n.val;_LL9: {struct Cyc_List_List*vs=_Tmp2;
_Tmp2=vs;goto _LLB;}default: _Tmp2=ns.C_n.val;_LLB: {struct Cyc_List_List*vs=_Tmp2;
# 401
if(vs==0)
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)*s;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)*v;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s_%s_struct",sizeof(char),13U),_tag_fat(_Tmp5,sizeof(void*),2));});{
struct _RegionHandle _Tmp3=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp3;_push_region(r);
{struct _fat_ptr _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)*s;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_rstr_sepstr(r,vs,_tag_fat("_",sizeof(char),2U));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)*v;_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(_tag_fat("%s_%s_%s_struct",sizeof(char),16U),_tag_fat(_Tmp8,sizeof(void*),3));});_npop_handler(0);return _Tmp4;};_pop_region();}}};}}struct _tuple20{struct Cyc_Toc_TocState*f0;struct _tuple14*f1;};
# 414
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 417
struct _tuple20 _Tmp0=*env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0->qvar_tags;_Tmp1=_Tmp0.f1;{struct Cyc_Dict_Dict*qvs=_Tmp2;struct _tuple14*pair=_Tmp1;
struct _tuple14 _Tmp3=*pair;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _tuple1*fieldname=_Tmp5;struct _tuple1*dtname=_Tmp4;
struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){{struct _tuple1*_Tmp8=({struct _tuple1*(*_Tmp9)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*)=(struct _tuple1*(*)(struct Cyc_Dict_Dict,int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*))Cyc_Dict_lookup_other;_Tmp9;})(*qvs,Cyc_Toc_qvar_tag_cmp,pair);_npop_handler(0);return _Tmp8;};_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp8)->tag==Cyc_Dict_Absent){
# 421
struct _tuple14*new_pair;new_pair=_region_malloc(d,0U,sizeof(struct _tuple14)),new_pair->f0=fieldname,new_pair->f1=dtname;{
void*_TmpA;union Cyc_Absyn_Nmspace _TmpB;_TmpB=fieldname->f0;_TmpA=fieldname->f1;{union Cyc_Absyn_Nmspace nmspace=_TmpB;struct _fat_ptr*fieldvar=_TmpA;
struct _fat_ptr newvar=Cyc_Toc_collapse_qvar(fieldvar,dtname);
struct _tuple1*res;res=_cycalloc(sizeof(struct _tuple1)),res->f0=nmspace,({struct _fat_ptr*_TmpC=({struct _fat_ptr*_TmpD=_cycalloc(sizeof(struct _fat_ptr));*_TmpD=newvar;_TmpD;});res->f1=_TmpC;});
({struct Cyc_Dict_Dict _TmpC=({struct Cyc_Dict_Dict(*_TmpD)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple14*,struct _tuple1*))Cyc_Dict_insert;_TmpD;})(*qvs,new_pair,res);*qvs=_TmpC;});
return res;}}}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}};}}}}}
# 429
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 env=({struct _tuple14 _Tmp0;_Tmp0.f0=fieldname,_Tmp0.f1=dtname;_Tmp0;});
return({struct _tuple1*(*_Tmp0)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple20*))=(struct _tuple1*(*)(struct _tuple14*,struct _tuple1*(*)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state;_Tmp0;})(& env,Cyc_Toc_collapse_qvars_body);}
# 436
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _fat_ptr*name,struct Cyc_List_List*fs){
struct Cyc_Absyn_Aggrdecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp0->kind=0U,_Tmp0->sc=2U,_Tmp0->tvs=0,_Tmp0->attributes=0,_Tmp0->expected_mem_kind=0,({
# 439
struct _tuple1*_Tmp1=({struct _tuple1*_Tmp2=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Rel_n(0);_Tmp2->f0=_Tmp3;}),_Tmp2->f1=name;_Tmp2;});_Tmp0->name=_Tmp1;}),({
struct Cyc_Absyn_AggrdeclImpl*_Tmp1=({struct Cyc_Absyn_AggrdeclImpl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_Tmp2->exist_vars=0,_Tmp2->qual_bnd=0,_Tmp2->fields=fs,_Tmp2->tagged=0,_Tmp2->effconstr=0;_Tmp2;});_Tmp0->impl=_Tmp1;});return _Tmp0;}struct _tuple21{struct Cyc_Toc_TocState*f0;struct Cyc_List_List*f1;};struct _tuple22{void*f0;struct Cyc_List_List*f1;};struct _tuple23{struct Cyc_Absyn_Tqual f0;void*f1;};
# 445
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 448
struct _tuple21 _Tmp0=*env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0->tuple_types;_Tmp1=_Tmp0.f1;{struct Cyc_List_List**tuple_types=_Tmp2;struct Cyc_List_List*tqs0=_Tmp1;
# 450
{struct Cyc_List_List*tts=*tuple_types;for(0;tts!=0;tts=tts->tl){
struct _tuple22*_Tmp3=(struct _tuple22*)tts->hd;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3->f0;_Tmp4=_Tmp3->f1;{void*x=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
struct Cyc_List_List*tqs=tqs0;
for(1;tqs!=0 && ts!=0;(tqs=tqs->tl,ts=ts->tl)){
if(!Cyc_Unify_unify((*((struct _tuple23*)tqs->hd)).f1,(void*)ts->hd))
break;}
if(tqs==0 && ts==0)
return x;}}}{
# 461
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("_tuple%d",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),1));});*xname=_Tmp3;});{
struct Cyc_List_List*fs=0;
struct Cyc_List_List*ts=0;
{int i=1;for(0;tqs0!=0;(tqs0=tqs0->tl,i ++)){
void*t=(*((struct _tuple23*)tqs0->hd)).f1;
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp4=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));({struct _fat_ptr*_Tmp6=Cyc_Absyn_fieldname(i);_Tmp5->name=_Tmp6;}),_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=t,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=fs;_Tmp3;});
ts=({struct Cyc_List_List*_Tmp3=_region_malloc(d,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=ts;_Tmp3;});}}
# 469
fs=Cyc_List_imp_rev(fs);
ts=Cyc_List_imp_rev(ts);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
void*ans=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp3=sd;_Tmp3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp5->tag=5,_Tmp5->f1=sd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_Toc_result_decls;_Tmp3;});
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(d,0U,sizeof(struct Cyc_List_List));({struct _tuple22*_Tmp5=({struct _tuple22*_Tmp6=_region_malloc(d,0U,sizeof(struct _tuple22));_Tmp6->f0=ans,_Tmp6->f1=ts;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*tuple_types;_Tmp4;});*tuple_types=_Tmp3;});
return ans;}}}}}
# 477
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return({void*(*_Tmp0)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple21*))=(void*(*)(struct Cyc_List_List*,void*(*)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state;_Tmp0;})(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f0;struct Cyc_List_List*f1;};struct _tuple25{struct Cyc_Toc_TocState*f0;struct _tuple24*f1;};struct _tuple26{void*f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 482
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 485
void*_Tmp0;enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;_Tmp2=env->f0->anon_aggr_types;_Tmp1=env->f1->f0;_Tmp0=env->f1->f1;{struct Cyc_List_List**anon_aggr_types=_Tmp2;enum Cyc_Absyn_AggrKind ak=_Tmp1;struct Cyc_List_List*fs=_Tmp0;
# 487
{struct Cyc_List_List*ts=*anon_aggr_types;for(0;ts!=0;ts=ts->tl){
struct _tuple26*_Tmp3=(struct _tuple26*)ts->hd;void*_Tmp4;enum Cyc_Absyn_AggrKind _Tmp5;void*_Tmp6;_Tmp6=_Tmp3->f0;_Tmp5=_Tmp3->f1;_Tmp4=_Tmp3->f2;{void*x=_Tmp6;enum Cyc_Absyn_AggrKind ak2=_Tmp5;struct Cyc_List_List*fs2=_Tmp4;
if((int)ak==(int)ak2 && !({int(*_Tmp7)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;_Tmp7;})(Cyc_Tcutil_aggrfield_cmp,fs2,fs))
return x;}}}{
# 494
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("_tuple%d",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),1));});*xname=_Tmp3;});{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
sd->kind=ak;{
void*ans=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp3=sd;_Tmp3;})),0);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp5->tag=5,_Tmp5->f1=sd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_Toc_result_decls;_Tmp3;});
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(d,0U,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp5=({struct _tuple26*_Tmp6=_region_malloc(d,0U,sizeof(struct _tuple26));_Tmp6->f0=ans,_Tmp6->f1=ak,_Tmp6->f2=fs;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*anon_aggr_types;_Tmp4;});*anon_aggr_types=_Tmp3;});
return ans;}}}}}
# 502
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
void*(*_Tmp0)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*))=({void*(*_Tmp1)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*))=(void*(*)(struct _tuple24*,void*(*)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state;_Tmp1;});return _Tmp0(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f0=ak,_Tmp1->f1=fs;_Tmp1;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _tuple28{struct Cyc_Toc_TocState*f0;struct _tuple27*f1;};struct _tuple29{struct _tuple1*f0;struct Cyc_List_List*f1;void*f2;};
# 511
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 520
struct _tuple28 _Tmp0=*env;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0.f0->abs_struct_types;_Tmp4=_Tmp0.f1->f0;_Tmp3=_Tmp0.f1->f1;_Tmp2=_Tmp0.f1->f2;_Tmp1=_Tmp0.f1->f3;{struct Cyc_List_List**abs_struct_types=_Tmp5;struct _tuple1*struct_name=_Tmp4;struct Cyc_List_List*type_vars=_Tmp3;struct Cyc_List_List*type_args=_Tmp2;struct Cyc_List_List*fields=_Tmp1;
# 524
{struct Cyc_List_List*tts=*abs_struct_types;for(0;tts!=0;tts=tts->tl){
struct _tuple29*_Tmp6=(struct _tuple29*)tts->hd;void*_Tmp7;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp6->f0;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct _tuple1*x=_Tmp9;struct Cyc_List_List*ts2=_Tmp8;void*t=_Tmp7;
if(Cyc_Absyn_qvar_cmp(x,struct_name)==0 &&({
int _TmpA=Cyc_List_length(type_args);_TmpA==Cyc_List_length(ts2);})){
int okay=1;
{struct Cyc_List_List*ts=type_args;for(0;ts!=0;(ts=ts->tl,ts2=ts2->tl)){
void*t=(void*)ts->hd;
void*t2=(void*)_check_null(ts2)->hd;
{struct Cyc_Absyn_Kind*_TmpA=Cyc_Tcutil_type_kind(t);if(((struct Cyc_Absyn_Kind*)_TmpA)->kind==Cyc_Absyn_EffKind)
continue;else{
# 537
if(Cyc_Unify_unify(t,t2)||({void*_TmpB=Cyc_Toc_typ_to_c(t);Cyc_Unify_unify(_TmpB,Cyc_Toc_typ_to_c(t2));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 542
break;}}
# 544
if(okay)
return t;}}}}{
# 550
struct _fat_ptr*xname;xname=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp6=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(_tag_fat("_tuple%d",sizeof(char),9U),_tag_fat(_Tmp8,sizeof(void*),1));});*xname=_Tmp6;});{
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*fs=0;
# 554
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(d,0U,sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp8=({struct _tuple29*_Tmp9=_region_malloc(d,0U,sizeof(struct _tuple29));_Tmp9->f0=struct_name,_Tmp9->f1=type_args,_Tmp9->f2=x;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=*abs_struct_types;_Tmp7;});*abs_struct_types=_Tmp6;});{
# 557
struct _RegionHandle _Tmp6=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp6;_push_region(r);
{struct Cyc_List_List*inst=Cyc_List_rzip(r,r,type_vars,type_args);
for(1;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=f->type;
struct Cyc_List_List*atts=f->attributes;
# 567
if((fields->tl==0 &&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t)))&& !
Cyc_Tcutil_is_array_type(t))
atts=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp8=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_Tmp9->tag=6,_Tmp9->f1=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=atts;_Tmp7;});
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,inst,t));
# 574
if(Cyc_Unify_unify(t,Cyc_Absyn_void_type))
t=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp7->tag=4,({void*_Tmp8=Cyc_Toc_void_star_type();_Tmp7->f1.elt_type=_Tmp8;}),_Tmp7->f1.tq=Cyc_Toc_mt_tq,({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_uint_exp(0U,0U);_Tmp7->f1.num_elts=_Tmp8;}),_Tmp7->f1.zero_term=Cyc_Absyn_false_type,_Tmp7->f1.zt_loc=0U;_Tmp7;});
fs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp8=({struct Cyc_Absyn_Aggrfield*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp9->name=f->name,_Tmp9->tq=Cyc_Toc_mt_tq,_Tmp9->type=t,_Tmp9->width=f->width,_Tmp9->attributes=atts,_Tmp9->requires_clause=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=fs;_Tmp7;});}
# 579
fs=Cyc_List_imp_rev(fs);{
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Toc_make_c_struct_defn(xname,fs);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp9->tag=5,_Tmp9->f1=sd;_Tmp9;}),0U);_Tmp7->hd=_Tmp8;}),_Tmp7->tl=Cyc_Toc_result_decls;_Tmp7;});{
void*_Tmp7=x;_npop_handler(0);return _Tmp7;}}}
# 558
;_pop_region();}}}}}
# 585
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 589
struct _tuple27 env=({struct _tuple27 _Tmp0;_Tmp0.f0=struct_name,_Tmp0.f1=type_vars,_Tmp0.f2=type_args,_Tmp0.f3=fields;_Tmp0;});
return({void*(*_Tmp0)(struct _tuple27*,void*(*)(struct _RegionHandle*,struct _tuple28*))=(void*(*)(struct _tuple27*,void*(*)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state;_Tmp0;})(& env,Cyc_Toc_add_struct_type_body);}
# 597
struct _tuple1*Cyc_Toc_temp_var (void){
struct _tuple1*_Tmp0=_cycalloc(sizeof(struct _tuple1));_Tmp0->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)Cyc_Toc_temp_var_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("_tmp%X",sizeof(char),7U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}struct _tuple30{struct _tuple1*f0;struct Cyc_Absyn_Exp*f1;};
# 600
struct _tuple30 Cyc_Toc_temp_var_and_exp (void){
struct _tuple1*v=Cyc_Toc_temp_var();
struct _tuple30 _Tmp0;_Tmp0.f0=v,({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_var_exp(v,0U);_Tmp0.f1=_Tmp1;});return _Tmp0;}struct _tuple31{struct Cyc_Toc_TocState*f0;int f1;};
# 607
static struct _fat_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple31*env){
struct _tuple31 _Tmp0=*env;void*_Tmp1;_Tmp1=_Tmp0.f0->temp_labels;{struct Cyc_Xarray_Xarray*temp_labels=_Tmp1;
int i=Cyc_Toc_fresh_label_counter ++;
if(({int _Tmp2=i;_Tmp2 < Cyc_Xarray_length(temp_labels);}))
return({struct _fat_ptr*(*_Tmp2)(struct Cyc_Xarray_Xarray*,int)=(struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;_Tmp2;})(temp_labels,i);{
struct _fat_ptr*res;res=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp2=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned)i;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("_LL%X",sizeof(char),6U),_tag_fat(_Tmp4,sizeof(void*),1));});*res=_Tmp2;});
if(({int _Tmp2=({int(*_Tmp3)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*)=(int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind;_Tmp3;})(temp_labels,res);_Tmp2!=i;}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("fresh_label: add_ind returned bad index...",sizeof(char),43U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});
return res;}}}
# 617
static struct _fat_ptr*Cyc_Toc_fresh_label (void){
return({struct _fat_ptr*(*_Tmp0)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple31*))=(struct _fat_ptr*(*)(int,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple31*)))Cyc_Toc_use_toc_state;_Tmp0;})(0,Cyc_Toc_fresh_label_body);}
# 623
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
{struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(td->fields)->v;for(0;Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)_check_null(fs)->hd)->name)!=0;fs=fs->tl){
++ ans;}}
return Cyc_Absyn_signed_int_exp(ans,0U);}
# 633
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*);
# 640
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9*_Tmp0=_cycalloc(sizeof(struct _tuple9));_Tmp0->f0=0,_Tmp0->f1=(*a).f1,({void*_Tmp1=Cyc_Toc_typ_to_c((*a).f2);_Tmp0->f2=_Tmp1;});return _Tmp0;}
# 659 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _Tmp1;if(*((int*)_Tmp0)==4){_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp1;
void*_Tmp2=Cyc_Toc_typ_to_c_array(ai.elt_type);return Cyc_Absyn_cstar_type(_Tmp2,ai.tq);}}else{
return Cyc_Toc_typ_to_c(t);};}
# 666
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 668
struct Cyc_Absyn_Aggrfield*ans;ans=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),*ans=*f;
ans->type=new_type;
ans->requires_clause=0;
ans->tq=Cyc_Toc_mt_tq;
return ans;}
# 675
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}
# 679
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
return Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_bk);}}else{
return 0;};}
# 685
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_Tmp0=Cyc_Tcutil_type_kind(t);if(((struct Cyc_Absyn_Kind*)_Tmp0)->kind==Cyc_Absyn_AnyKind)
return 1;else{
return 0;};}
# 691
static int Cyc_Toc_is_void_star(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*t2=_Tmp1;
return Cyc_Tcutil_is_void_type(t2);}}else{
return 0;};}
# 697
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 700
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t)|| Cyc_Toc_is_boxed_tvar(t);}
# 704
void*Cyc_Toc_typ_to_c(void*t){
enum Cyc_Absyn_AggrKind _Tmp0;void*_Tmp1;void*_Tmp2;int _Tmp3;unsigned _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;union Cyc_Absyn_AggrInfo _Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)t)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 0:
 return t;case 22:
# 715
 return Cyc_Absyn_void_type;case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.tag==2){_Tmp8=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.val.f0;_Tmp7=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.val.f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp8;struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;
# 717
return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));}}else{
# 719
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("unresolved DatatypeFieldType",sizeof(char),29U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});}case 1:
# 727
 goto _LL12;case 2: _LL12:
 return t;case 24: _Tmp8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*c=_Tmp8;union Cyc_Absyn_AggrInfo info=_Tmp6;struct Cyc_List_List*ts=_Tmp7;
# 753
if(info.UnknownAggr.tag==1){
# 755
if(ts==0)return t;else{return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp9->tag=0,_Tmp9->f1=c,_Tmp9->f2=0;_Tmp9;});}}else{
goto _LL37;}_LL37:;{
# 758
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
# 760
if(ad->expected_mem_kind && ad->impl==0)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpA;_TmpA.tag=6,_TmpA.f1=ad;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat(" was never defined.",sizeof(char),20U);_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_warn2(0U,_tag_fat(_TmpB,sizeof(void*),2));});
# 763
if((int)ad->kind==1)
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_unionq_type);{
struct Cyc_List_List*fs=ad->impl==0?0: ad->impl->fields;
if(fs==0)return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);
for(1;fs->tl!=0;fs=fs->tl){;}{
void*last_type=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(last_type))){
if(ad->expected_mem_kind)
({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpA;_TmpA.tag=6,_TmpA.f1=ad;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat(" ended up being abstract.",sizeof(char),26U);_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_warn2(0U,_tag_fat(_TmpB,sizeof(void*),2));});{
# 774
struct _RegionHandle _Tmp9=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp9;_push_region(r);
{struct Cyc_List_List*inst=Cyc_List_rzip(r,r,ad->tvs,ts);
void*t=Cyc_Tcutil_rsubstitute(r,inst,last_type);
if(Cyc_Toc_is_abstract_type(t)){void*_TmpA=Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);_npop_handler(0);return _TmpA;}{
void*_TmpA=Cyc_Toc_add_struct_type(ad->name,ad->tvs,ts,ad->impl->fields);_npop_handler(0);return _TmpA;}}
# 775
;_pop_region();}}
# 780
return Cyc_Toc_aggrdecl_type(ad->name,Cyc_Absyn_strctq);}}}}case 19: _Tmp8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _tuple1*tdn=_Tmp8;
return t;}case 20: _Tmp8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct Cyc_List_List*fs=_Tmp8;
Cyc_Toc_enumfields_to_c(fs);return t;}case 5:
# 789
 return Cyc_Absyn_uint_type;case 4:
 return Cyc_Toc_rgn_type();case 18:
 return Cyc_Toc_aqualt_type();case 21:
 return t;case 3:
 return t;default:
# 796
 return Cyc_Toc_void_star_type();}case 1: _Tmp8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;{void**t2=_Tmp8;
# 708
if(*t2!=0)
return Cyc_Toc_typ_to_c(*t2);{
void*_Tmp9=Cyc_Toc_void_star_type();return*t2=_Tmp9;}}case 2: _Tmp8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp8;
# 712
if((int)Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk)->kind==0)
return Cyc_Absyn_void_type;
return Cyc_Toc_void_star_type();}case 3: _Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;{void*t2=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp5;void*bnds=_Tmp7;
# 723
t2=Cyc_Toc_typ_to_c(t2);
if(Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,bnds)==0)
return Cyc_Toc_fat_ptr_type();
return Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}case 4: _Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*t2=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Absyn_Exp*e=_Tmp7;unsigned ztl=_Tmp4;
# 730
void*_Tmp9=Cyc_Toc_typ_to_c(t2);struct Cyc_Absyn_Tqual _TmpA=tq;struct Cyc_Absyn_Exp*_TmpB=e;void*_TmpC=Cyc_Absyn_false_type;return Cyc_Absyn_array_type(_Tmp9,_TmpA,_TmpB,_TmpC,ztl);}case 5: _Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_tqual;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_type;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.args;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.c_varargs;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.cyc_varargs;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.attributes;{struct Cyc_Absyn_Tqual tq2=_Tmp5;void*t2=_Tmp8;struct Cyc_List_List*args=_Tmp7;int c_vararg=_Tmp3;struct Cyc_Absyn_VarargInfo*cyc_vararg=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 734
struct Cyc_List_List*new_atts=Cyc_Atts_atts2c(atts);
struct Cyc_List_List*new_args=({struct Cyc_List_List*(*_Tmp9)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple9*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;_Tmp9;})(Cyc_Toc_arg_to_c,args);
if(cyc_vararg!=0){
# 738
void*t=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(cyc_vararg->type,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,Cyc_Toc_mt_tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));
struct _tuple9*vararg;vararg=_cycalloc(sizeof(struct _tuple9)),vararg->f0=cyc_vararg->name,vararg->f1=cyc_vararg->tq,vararg->f2=t;
new_args=({struct Cyc_List_List*_Tmp9=new_args;Cyc_List_imp_append(_Tmp9,({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=vararg,_TmpA->tl=0;_TmpA;}));});}
# 742
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp9->tag=5,_Tmp9->f1.tvars=0,_Tmp9->f1.effect=0,_Tmp9->f1.ret_tqual=tq2,({void*_TmpA=Cyc_Toc_typ_to_c(t2);_Tmp9->f1.ret_type=_TmpA;}),_Tmp9->f1.args=new_args,_Tmp9->f1.c_varargs=c_vararg,_Tmp9->f1.cyc_varargs=0,_Tmp9->f1.qual_bnd=0,_Tmp9->f1.attributes=new_atts,_Tmp9->f1.requires_clause=0,_Tmp9->f1.requires_assn=0,_Tmp9->f1.ensures_clause=0,_Tmp9->f1.ensures_assn=0,_Tmp9->f1.throws_clause=0,_Tmp9->f1.throws_assn=0,_Tmp9->f1.return_value=0,_Tmp9->f1.arg_vardecls=0,_Tmp9->f1.effconstr=0;_Tmp9;});}case 6: _Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f1;_Tmp8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp0;struct Cyc_List_List*fs=_Tmp8;
# 747
struct Cyc_List_List*fs2=0;
for(1;fs!=0;fs=fs->tl){
fs2=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_TmpA=({struct Cyc_Absyn_Aggrfield*_TmpB=(struct Cyc_Absyn_Aggrfield*)fs->hd;Cyc_Toc_aggrfield_to_c(_TmpB,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type));});_Tmp9->hd=_TmpA;}),_Tmp9->tl=fs2;_Tmp9;});}{
enum Cyc_Absyn_AggrKind _Tmp9=k;return Cyc_Toc_add_anon_aggr_type(_Tmp9,Cyc_List_imp_rev(fs2));}}case 7: _Tmp8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct _tuple1*tdn=_Tmp8;struct Cyc_List_List*ts=_Tmp7;struct Cyc_Absyn_Typedefdecl*td=_Tmp2;void*topt=_Tmp1;
# 786
if(topt==0){
if(ts==0)return t;else{return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_Tmp9->tag=7,_Tmp9->f1=tdn,_Tmp9->f2=0,_Tmp9->f3=td,_Tmp9->f4=0;_Tmp9;});}}
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_Tmp9->tag=7,_Tmp9->f1=tdn,_Tmp9->f2=0,_Tmp9->f3=td,({void*_TmpA=Cyc_Toc_typ_to_c(topt);_Tmp9->f4=_TmpA;});_Tmp9;});}case 8: _Tmp8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 801
return t;}case 10: _Tmp8=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 806
Cyc_Toc_exptypes_to_c(e);
if(e->topt==0)return t;else{return Cyc_Toc_typ_to_c(e->topt);}}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1)->r)){case 0: _Tmp8=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp8;
# 809
Cyc_Toc_aggrdecl_to_c(ad);
return Cyc_Toc_aggrdecl_type(ad->name,(int)ad->kind==1?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq);}case 1: _Tmp8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp8;
# 812
Cyc_Toc_enumdecl_to_c(ed);
return t;}default: _Tmp8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;_Tmp7=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp8;void**t=_Tmp7;
# 815
Cyc_Toc_datatypedecl_to_c(dd);
return Cyc_Toc_typ_to_c(*_check_null(t));}}};}
# 820
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned l){
struct Cyc_Absyn_Tqual _Tmp0;void*_Tmp1;if(*((int*)t)==4){_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp0=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;{void*t2=_Tmp1;struct Cyc_Absyn_Tqual tq=_Tmp0;
# 823
void*_Tmp2=Cyc_Absyn_star_type(t2,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);return Cyc_Toc_cast_it(_Tmp2,e);}}else{
return Cyc_Toc_cast_it(t,e);};}
# 830
static int Cyc_Toc_atomic_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 2:
 return 0;case 0: _Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 834
void*_Tmp3;void*_Tmp4;union Cyc_Absyn_AggrInfo _Tmp5;switch(*((int*)c)){case 0:
 goto _LL13;case 1: _LL13: goto _LL15;case 2: _LL15: goto _LL17;case 5: _LL17:
 goto _LL19;case 19: _LL19: goto _LL1B;case 20: _LL1B: return 1;case 22:
 goto _LL1F;case 4: _LL1F:
 goto _LL21;case 21: _LL21:
 return 0;case 24: _Tmp5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp5;
# 846
if(info.UnknownAggr.tag==1)
return 0;else{
goto _LL28;}_LL28:;{
# 850
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)
return 0;
{struct Cyc_List_List*fs=ad->impl->fields;for(0;fs!=0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}}
return 1;}}case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1.KnownDatatypefield.tag==2){_Tmp4=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1.KnownDatatypefield.val.f0;_Tmp3=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1.KnownDatatypefield.val.f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
# 857
{struct Cyc_List_List*tqs=tuf->typs;for(0;tqs!=0;tqs=tqs->tl){
if(!Cyc_Toc_atomic_type((*((struct _tuple23*)tqs->hd)).f1))return 0;}}
return 1;}}else{goto _LL26;}default: _LL26:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("atomic_typ: bad type ",sizeof(char),22U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),2));});};}case 5:
# 862
 return 1;case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*t=_Tmp2;
return Cyc_Toc_atomic_type(t);}case 6: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_List_List*fs=_Tmp2;
# 865
for(1;fs!=0;fs=fs->tl){
if(!Cyc_Toc_atomic_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}case 3:
 return 0;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("atomic_typ:  bad type ",sizeof(char),23U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 874
static int Cyc_Toc_is_poly_field(void*t,struct _fat_ptr*f){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp2;
# 877
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("is_poly_field: type missing fields",sizeof(char),35U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});
_Tmp1=ad->impl->fields;goto _LL4;}}else{goto _LL5;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 882
struct Cyc_Absyn_Aggrfield*field=Cyc_Absyn_lookup_field(fs,f);
if(field==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("is_poly_field: bad field ",sizeof(char),26U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*f;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});
return Cyc_Toc_is_void_star_or_boxed_tvar(field->type);}default: _LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("is_poly_field: bad type ",sizeof(char),25U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 893
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 896
return Cyc_Toc_is_poly_field(_check_null(e1->topt),f)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 899
void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp4;if(*((int*)_Tmp3)==3){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;{void*t=_Tmp4;
# 901
return Cyc_Toc_is_poly_field(t,f)&& !Cyc_Toc_is_void_star_or_boxed_tvar(_check_null(e->topt));}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("is_poly_project: bad type ",sizeof(char),27U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)_check_null(e1->topt);_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),2));});};}default:
# 904
 return 0;};}
# 909
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(t)?Cyc_Toc__cycalloc_atomic_e: Cyc_Toc__cycalloc_e;
struct Cyc_Absyn_Exp*_Tmp0[1];_Tmp0[0]=s;return Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),1));}
# 914
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_atomic_type(elt_type)?Cyc_Toc__cyccalloc_atomic_e: Cyc_Toc__cyccalloc_e;
struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=s,_Tmp0[1]=n;return Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));}
# 919
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_Tmp0[3];_Tmp0[0]=rgn,_Tmp0[1]=aq,_Tmp0[2]=s;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_malloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),3));}
# 922
static struct Cyc_Absyn_Exp*Cyc_Toc_aqmalloc_exp(struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=aq,_Tmp0[1]=s;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__aqual_malloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));}
# 926
static struct Cyc_Absyn_Exp*Cyc_Toc_rvmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_Tmp0[2];_Tmp0[0]=rgn,_Tmp0[1]=s;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_vmalloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),2));}
# 930
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_Tmp0[3];_Tmp0[0]=rgn,_Tmp0[1]=aq,_Tmp0[2]=s;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__fast_region_malloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),3));}
# 934
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_Tmp0[4];_Tmp0[0]=rgn,_Tmp0[1]=aq,_Tmp0[2]=s,_Tmp0[3]=n;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__region_calloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),4));}
# 937
static struct Cyc_Absyn_Exp*Cyc_Toc_aqcalloc_exp(struct Cyc_Absyn_Exp*aq,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_Tmp0[3];_Tmp0[0]=aq,_Tmp0[1]=s,_Tmp0[2]=n;return Cyc_Toc_fncall_exp_dl(Cyc_Toc__aqual_calloc_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),3));}
# 941
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt (void){
return Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_match_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U);}
# 946
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 954
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
{void*_Tmp0=e->r;if(*((int*)_Tmp0)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 8:
 goto _LL4;case 9: _LL4: {
# 959
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_type,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp3->tag=0,_Tmp3->f1=vd;_Tmp3;}),0U);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=Cyc_Toc_result_decls;_Tmp1;});
xexp=Cyc_Absyn_var_exp(x,0U);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0U);
goto _LL0;}default: goto _LL5;}else{_LL5:
# 967
 xexp=({void*_Tmp1=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp1,e);});
# 969
xplussz=({void*_Tmp1=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp1,Cyc_Absyn_add_exp(e,sz,0U));});
goto _LL0;}_LL0:;}
# 972
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple19*_Tmp0[3];({struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=0,_Tmp2->f1=xexp;_Tmp2;});_Tmp0[0]=_Tmp1;}),({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=0,_Tmp2->f1=xexp;_Tmp2;});_Tmp0[1]=_Tmp1;}),({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f0=0,_Tmp2->f1=xplussz;_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _tuple19*),3));}),0U);}struct Cyc_Toc_FallthruInfo{struct _fat_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _fat_ptr**break_lab;struct _fat_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1006 "toc.cyc"
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
struct Cyc_Toc_Env*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));_Tmp0->break_lab=0,_Tmp0->continue_lab=0,_Tmp0->fallthru_info=0,_Tmp0->toplevel=1,({int*_Tmp1=({int*_Tmp2=_region_malloc(r,0U,sizeof(int));*_Tmp2=0;_Tmp2;});_Tmp0->in_lhs=_Tmp1;}),_Tmp0->rgn=r;return _Tmp0;}
# 1010
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _Tmp0;_Tmp0=nv->toplevel;{int t=_Tmp0;
return t;}}
# 1014
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));_Tmp5->break_lab=b,_Tmp5->continue_lab=c,_Tmp5->fallthru_info=f,_Tmp5->toplevel=0,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}
# 1018
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));_Tmp5->break_lab=b,_Tmp5->continue_lab=c,_Tmp5->fallthru_info=f,_Tmp5->toplevel=1,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}
# 1022
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
void*_Tmp0;_Tmp0=nv->in_lhs;{int*b=_Tmp0;
return*b;}}
# 1026
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
void*_Tmp0;_Tmp0=e->in_lhs;{int*lhs=_Tmp0;
*lhs=b;}}
# 1031
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));_Tmp5->break_lab=b,_Tmp5->continue_lab=c,_Tmp5->fallthru_info=f,_Tmp5->toplevel=t,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}
# 1038
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));_Tmp5->break_lab=0,_Tmp5->continue_lab=0,_Tmp5->fallthru_info=f,_Tmp5->toplevel=t,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}
# 1044
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l,struct _fat_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1048
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_FallthruInfo*fi;
fi=_region_malloc(r,0U,sizeof(struct Cyc_Toc_FallthruInfo)),fi->label=fallthru_l,fi->binders=fallthru_binders;{
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));({struct _fat_ptr**_Tmp6=({struct _fat_ptr**_Tmp7=_region_malloc(r,0U,sizeof(struct _fat_ptr*));*_Tmp7=break_l;_Tmp7;});_Tmp5->break_lab=_Tmp6;}),_Tmp5->continue_lab=c,_Tmp5->fallthru_info=fi,_Tmp5->toplevel=t,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}}
# 1053
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _fat_ptr*break_l){
# 1055
void*_Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=e->break_lab;_Tmp3=e->continue_lab;_Tmp2=e->fallthru_info;_Tmp1=e->toplevel;_Tmp0=e->in_lhs;{struct _fat_ptr**b=_Tmp4;struct _fat_ptr**c=_Tmp3;struct Cyc_Toc_FallthruInfo*f=_Tmp2;int t=_Tmp1;int*lhs=_Tmp0;
struct Cyc_Toc_Env*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Toc_Env));({struct _fat_ptr**_Tmp6=({struct _fat_ptr**_Tmp7=_region_malloc(r,0U,sizeof(struct _fat_ptr*));*_Tmp7=break_l;_Tmp7;});_Tmp5->break_lab=_Tmp6;}),_Tmp5->continue_lab=c,_Tmp5->fallthru_info=0,_Tmp5->toplevel=t,_Tmp5->in_lhs=lhs,_Tmp5->rgn=r;return _Tmp5;}}
# 1068 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*);
# 1072
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Stmt*);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*,struct Cyc_Absyn_Fndecl*,int);struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
# 1075
static void Cyc_Toc_asm_iolist_types_toc(struct Cyc_List_List*l){
for(1;l!=0;l=l->tl){
Cyc_Toc_exptypes_to_c((*((struct _tuple32*)l->hd)).f1);}}
# 1080
static int Cyc_Toc_do_null_check(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->annot;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NoCheck)
return 0;else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NullOnly)
goto _LL6;else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NullAndFatBound){_LL6:
 goto _LL8;}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NullAndThinBound){_LL8:
 return 1;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_FatBound)
goto _LLC;else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_ThinBound){_LLC:
 return 0;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Toc: do_null_check",sizeof(char),19U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp3;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});}}}}}};}
# 1096
static int Cyc_Toc_ptr_use_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*ptr,void*annot,struct Cyc_Absyn_Exp*index){
# 1098
int ans;
int old_lhs=Cyc_Toc_in_lhs(nv);
void*old_typ=Cyc_Absyn_compress(_check_null(ptr->topt));
void*new_typ=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__check_known_subscript_notnull_e;
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,ptr);
if(index!=0)
Cyc_Toc_exp_to_c(nv,index);{
void*_Tmp0;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;if(*((int*)old_typ)==3){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)old_typ)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)old_typ)->f1.elt_tq;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)old_typ)->f1.ptr_atts.bounds;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)old_typ)->f1.ptr_atts.zero_term;{void*ta=_Tmp3;struct Cyc_Absyn_Tqual tq=_Tmp2;void*b=_Tmp1;void*zt=_Tmp0;
# 1109
{void*_Tmp4;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_NoCheck){
# 1111
if(!((unsigned)({void*_Tmp5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp5,b);}))){
# 1114
void*newt=({void*_Tmp5=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_Tmp5,tq);});
({void*_Tmp5=({void*_Tmp6=newt;Cyc_Toc_cast_it_r(_Tmp6,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_Tmp7,Cyc_Toc_curr_sp,0U);}));});ptr->r=_Tmp5;});
ptr->topt=newt;}
# 1118
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_NullOnly){
# 1121
if(!((unsigned)({void*_Tmp5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp5,b);}))){
# 1123
void*newt=({void*_Tmp5=Cyc_Toc_typ_to_c(ta);Cyc_Absyn_cstar_type(_Tmp5,tq);});
({void*_Tmp5=({void*_Tmp6=newt;Cyc_Toc_cast_it_r(_Tmp6,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_new_exp(ptr->r,0U);Cyc_Toc_member_exp(_Tmp7,Cyc_Toc_curr_sp,0U);}));});ptr->r=_Tmp5;});
ptr->topt=newt;
# 1130
if(index!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("subscript of ? with no bounds check but need null check",sizeof(char),56U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),1));});}
# 1134
({void*_Tmp5=({struct Cyc_Absyn_Exp*_Tmp6[1];({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp6[0]=_Tmp7;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_Tmp5;});
ans=0;
goto _LL5;}else{if(((struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_NullAndFatBound)
goto _LLD;else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_FatBound){_LLD: {
# 1140
void*ta1=Cyc_Toc_typ_to_c(ta);
void*newt=Cyc_Absyn_cstar_type(ta1,tq);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
({void*_Tmp5=({void*_Tmp6=newt;Cyc_Toc_cast_it_r(_Tmp6,({struct Cyc_Absyn_Exp*_Tmp7[3];({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp7[0]=_Tmp8;}),({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_sizeoftype_exp(ta1,0U);_Tmp7[1]=_Tmp8;}),_Tmp7[2]=ind;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_fat_subscript_e,_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1143
ptr->r=_Tmp5;});
# 1147
ptr->topt=newt;
ans=1;
goto _LL5;}}else{if(((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_NullAndThinBound){_Tmp4=((struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*)annot)->f1;{struct Cyc_Absyn_Exp*bd=_Tmp4;
# 1151
fn_e=Cyc_Toc__check_known_subscript_null_e;
_Tmp4=bd;goto _LL11;}}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)annot)->tag==Cyc_InsertChecks_ThinBound){_Tmp4=((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)annot)->f1;_LL11: {struct Cyc_Absyn_Exp*bd=_Tmp4;
# 1154
void*ta1=Cyc_Toc_typ_to_c(ta);
struct Cyc_Absyn_Exp*ind=(unsigned)index?index: Cyc_Absyn_uint_exp(0U,0U);
# 1159
struct _tuple13 _Tmp5=Cyc_Evexp_eval_const_uint_exp(bd);int _Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{unsigned i=_Tmp7;int valid=_Tmp6;
if((!valid || i!=1U)|| !Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(ptr->topt))){
# 1162
({void*_Tmp8=({void*_Tmp9=Cyc_Absyn_cstar_type(ta1,tq);Cyc_Toc_cast_it_r(_Tmp9,({struct Cyc_Absyn_Exp*_TmpA[4];({
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_new_exp(ptr->r,0U);_TmpA[0]=_TmpB;}),_TmpA[1]=bd,({
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_sizeoftype_exp(ta1,0U);_TmpA[2]=_TmpB;}),_TmpA[3]=ind;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_TmpA,sizeof(struct Cyc_Absyn_Exp*),4));}));});
# 1162
ptr->r=_Tmp8;});
# 1165
ans=1;}else{
# 1168
if(Cyc_Toc_is_zero(bd)){
if(fn_e==Cyc_Toc__check_known_subscript_null_e)
# 1171
({void*_Tmp8=({struct Cyc_Absyn_Exp*_Tmp9[1];({struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp9[0]=_TmpA;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Exp*),1));})->r;ptr->r=_Tmp8;});
ans=0;}else{
# 1176
if(Cyc_Toc_isCharStar(ptr))
({void*_Tmp8=({struct Cyc_Absyn_Exp*_Tmp9[3];({
struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_new_exp(ptr->r,0U);_Tmp9[0]=_TmpA;}),_Tmp9[1]=bd,_Tmp9[2]=ind;Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_plus_char_e,_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1177
ptr->r=_Tmp8;});else{
# 1180
({void*_Tmp8=({void*_Tmp9=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_Tmp9,({struct Cyc_Absyn_Exp*_TmpA[4];({
# 1182
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_sizeoftype_exp(Cyc_Tcutil_pointer_elt_type(Cyc_Toc_typ_to_c(old_typ)),0U);_TmpA[0]=_TmpB;}),({
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_new_exp(ptr->r,0U);_TmpA[1]=_TmpB;}),_TmpA[2]=bd,_TmpA[3]=ind;Cyc_Toc_fncall_exp_dl(Cyc_Toc__zero_arr_plus_other_e,_tag_fat(_TmpA,sizeof(struct Cyc_Absyn_Exp*),4));}));});
# 1180
ptr->r=_Tmp8;});}
# 1184
ans=1;}}
# 1187
goto _LL5;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("FIX: ptr_use_to_c, bad annotation",sizeof(char),34U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),1));});}}}}}}_LL5:;}
# 1190
Cyc_Toc_set_lhs(nv,old_lhs);
return ans;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("ptr_use_to_c: non-pointer-type",sizeof(char),31U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});};}}
# 1196
static void*Cyc_Toc_get_cyc_type(struct Cyc_Absyn_Exp*e){
if(e->topt==0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Missing type in primop ",sizeof(char),24U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
return e->topt;}
# 1200
static struct _tuple23*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f0=Cyc_Toc_mt_tq,({void*_Tmp1=Cyc_Toc_typ_to_c(_check_null(e->topt));_Tmp0->f1=_Tmp1;});return _Tmp0;}
# 1205
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;int _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 25: _Tmp2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp2;
# 1208
{struct Cyc_List_List*dles2=dles;for(0;dles2!=0;dles2=dles2->tl){
if((*((struct _tuple19*)dles2->hd)).f0!=0)
({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_Tmp3;})(_tag_fat("array designators for abstract-field initialization",sizeof(char),52U));}}
_Tmp2=Cyc_Absyn_signed_int_exp(Cyc_List_length(dles),0U);_Tmp1=0;goto _LL4;}case 26: _Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_LL4: {struct Cyc_Absyn_Exp*bd=_Tmp2;int zt=_Tmp1;
_Tmp2=bd;_Tmp1=zt;goto _LL6;}case 27: _Tmp2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL6: {struct Cyc_Absyn_Exp*bd=_Tmp2;int zt=_Tmp1;
# 1214
bd=Cyc_Absyn_copy_exp(bd);
if(zt){struct Cyc_Absyn_Exp*_Tmp3=bd;return Cyc_Absyn_add_exp(_Tmp3,Cyc_Absyn_uint_exp(1U,0U),0U);}else{return bd;}}default:
 return 0;};}
# 1223
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1231
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==28){_Tmp1=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*dles2=_Tmp1;
dles=dles2;goto _LL0;}}else{
return 0;}_LL0:;}
# 1237
{void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 1239
if(ad->impl==0 ||(int)ad->kind==1)
return 0;
field_types=ad->impl->fields;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 1245
 return 0;}_LL5:;}
# 1247
if(field_types==0)
return 0;
for(1;_check_null(field_types)->tl!=0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*last_type_field=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(1;dles!=0;dles=dles->tl){
struct _tuple19*_Tmp0=(struct _tuple19*)dles->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_List_List*ds=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);
if(!Cyc_strptrcmp(f,last_type_field->name)){
struct Cyc_Absyn_Exp*nested_ans=Cyc_Toc_get_varsizeexp(e2);
if(nested_ans!=0)
return nested_ans;{
void*_Tmp3=Cyc_Absyn_compress(last_type_field->type);void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==4){_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.num_elts;{void*elt_type=_Tmp5;struct Cyc_Absyn_Exp*type_bd=_Tmp4;
# 1262
if(type_bd==0 || !Cyc_Toc_is_zero(type_bd))
return 0;{
# 1268
struct Cyc_Absyn_Exp*_Tmp6=({struct Cyc_Absyn_Exp*_Tmp7[2];_Tmp7[0]=
_check_null(Cyc_Toc_array_length_exp(e2)),({
struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_sizeoftype_exp(elt_type,0U);_Tmp7[1]=_Tmp8;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1268
return Cyc_Absyn_add_exp(_Tmp6,
# 1271
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);}}}else{
return 0;};}}}}
# 1276
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("get_varsizeexp: did not find last struct field",sizeof(char),47U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}}
# 1279
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
struct Cyc_List_List*fs=_check_null(ad->impl)->fields;
{int i=1;for(0;fs!=0;(fs=fs->tl,++ i)){
if(Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,*f)==0)
return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("get_member_offset ",sizeof(char),19U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*f;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" failed",sizeof(char),8U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),3));});}struct _tuple33{struct Cyc_Core_Opt*f0;struct Cyc_Absyn_Exp*f1;};
# 1288
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple33*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f0,(*pr).f1,0U);}
# 1291
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1294
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp0->tag=5,_Tmp0->f1=e1,_Tmp0->f2=incr;_Tmp0;}),0U);}
# 1298
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,void*),void*);
# 1307
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1309
void*_Tmp0=e1->r;int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 36: _Tmp4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp4;
Cyc_Toc_lvalue_assign_stmt(s,fs,f,f_env);goto _LL0;}case 14: _Tmp4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Toc_lvalue_assign(e,fs,f,f_env);goto _LL0;}case 21: _Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;struct _fat_ptr*fld=_Tmp3;int is_tagged=_Tmp2;int is_read=_Tmp1;
# 1314
e1->r=e->r;
({struct Cyc_Absyn_Exp*_Tmp5=e1;struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=fld,_Tmp7->tl=fs;_Tmp7;});struct Cyc_Absyn_Exp*(*_Tmp7)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_Tmp5,_Tmp6,_Tmp7,f_env);});
goto _LL0;}default:  {
# 1322
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1324
for(1;fs!=0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _fat_ptr*)fs->hd,e1_copy->loc);}
({void*_Tmp5=f(e1_copy,f_env)->r;e1->r=_Tmp5;});
goto _LL0;}}_LL0:;}
# 1330
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1332
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
Cyc_Toc_lvalue_assign(e1,fs,f,f_env);goto _LL0;}case 12: _Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 1335
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}case 2: _Tmp2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp2;
Cyc_Toc_lvalue_assign_stmt(s2,fs,f,f_env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("lvalue_assign_stmt: ",sizeof(char),21U);_Tmp4;});struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp5;_Tmp5.tag=5,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Toc_toc_impos(_tag_fat(_Tmp5,sizeof(void*),2));});}_LL0:;}
# 1343
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*);
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp2;struct Cyc_Absyn_Exp**e1=(struct Cyc_Absyn_Exp**)_Tmp1;
# 1347
({struct Cyc_Absyn_Exp*_Tmp3=Cyc_Toc_push_address_exp(*e1);*e1=_Tmp3;});
({void*_Tmp3=Cyc_Absyn_cstar_type(*t,Cyc_Toc_mt_tq);*t=_Tmp3;});
return e;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
return e1;}case 36: _Tmp2=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp2;
# 1353
Cyc_Toc_push_address_stmt(s);return e;}default:
# 1355
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("can't take & of exp ",sizeof(char),21U);_Tmp4;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp5;_Tmp5.tag=4,_Tmp5.f1=e;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}
# 1359
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp1;
_Tmp1=s2;goto _LL4;}case 12: _Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Stmt*s2=_Tmp1;
Cyc_Toc_push_address_stmt(s2);goto _LL0;}case 1: _Tmp1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp**e=(struct Cyc_Absyn_Exp**)_Tmp1;
({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Toc_push_address_exp(*e);*e=_Tmp2;});goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("can't take & of stmt ",sizeof(char),22U);_Tmp3;});struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Stmt_Warn_Warg_struct _Tmp4;_Tmp4.tag=5,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 1371
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_fat,void*elt_type){
# 1383
void*fat_ptr_type=Cyc_Absyn_fatptr_type(elt_type,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,Cyc_Toc_mt_tq,Cyc_Absyn_true_type,Cyc_Absyn_false_type);
void*c_elt_type=Cyc_Toc_typ_to_c(elt_type);
void*c_fat_ptr_type=Cyc_Toc_typ_to_c(fat_ptr_type);
void*c_ptr_type=Cyc_Absyn_cstar_type(c_elt_type,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*c_ptr_type_opt;c_ptr_type_opt=_cycalloc(sizeof(struct Cyc_Core_Opt)),c_ptr_type_opt->v=c_ptr_type;{
struct Cyc_Absyn_Exp*xinit;
{void*_Tmp0=e1->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*ea=_Tmp2;
# 1391
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1396
Cyc_Toc_exp_to_c(nv,ea);
xinit=ea;
goto _LL0;}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*ea=_Tmp2;struct Cyc_Absyn_Exp*eb=_Tmp1;
# 1400
if(!is_fat){
ea=Cyc_Absyn_cast_exp(fat_ptr_type,ea,0,3U,0U);
ea->topt=fat_ptr_type;
ea->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1405
Cyc_Toc_exp_to_c(nv,ea);Cyc_Toc_exp_to_c(nv,eb);
xinit=({struct Cyc_Absyn_Exp*_Tmp3[3];_Tmp3[0]=ea,({
struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp3[1]=_Tmp4;}),_Tmp3[2]=eb;Cyc_Toc_fncall_exp_dl(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_Tmp3,sizeof(struct Cyc_Absyn_Exp*),3));});
goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("found bad lhs for zero-terminated pointer assignment",sizeof(char),53U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});}_LL0:;}{
# 1411
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _Tmp0=_new_region(0U,"rgn2");struct _RegionHandle*rgn2=& _Tmp0;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*x_vd;x_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),x_vd->sc=2U,x_vd->name=x,x_vd->varloc=0U,x_vd->tq=Cyc_Toc_mt_tq,x_vd->type=c_fat_ptr_type,x_vd->initializer=xinit,x_vd->rgn=0,x_vd->attributes=0,x_vd->escapes=0,x_vd->is_proto=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*x_bnd;x_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct)),x_bnd->tag=4,x_bnd->f1=x_vd;{
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_varb_exp((void*)x_bnd,0U);
x_exp->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*deref_x=Cyc_Absyn_deref_exp(x_exp,0U);
deref_x->topt=elt_type;
deref_x->annot=(void*)& Cyc_InsertChecks_NullAndFatBound_val;
Cyc_Toc_exp_to_c(nv,deref_x);{
struct _tuple1*y=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*y_vd;y_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),y_vd->sc=2U,y_vd->name=y,y_vd->varloc=0U,y_vd->tq=Cyc_Toc_mt_tq,y_vd->type=c_elt_type,y_vd->initializer=deref_x,y_vd->rgn=0,y_vd->attributes=0,y_vd->escapes=0,y_vd->is_proto=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*y_bnd;y_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct)),y_bnd->tag=4,y_bnd->f1=y_vd;{
struct Cyc_Absyn_Exp*z_init=e2;
if(popt!=0){
struct Cyc_Absyn_Exp*y_exp=Cyc_Absyn_varb_exp((void*)y_bnd,0U);
y_exp->topt=deref_x->topt;
z_init=({enum Cyc_Absyn_Primop _Tmp1=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_Absyn_Exp*_Tmp2=y_exp;Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,Cyc_Absyn_copy_exp(e2),0U);});
z_init->topt=y_exp->topt;
z_init->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
# 1432
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*z=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*z_vd;z_vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),z_vd->sc=2U,z_vd->name=z,z_vd->varloc=0U,z_vd->tq=Cyc_Toc_mt_tq,z_vd->type=c_elt_type,z_vd->initializer=z_init,z_vd->rgn=0,z_vd->attributes=0,z_vd->escapes=0,z_vd->is_proto=0;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*z_bnd;z_bnd=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct)),z_bnd->tag=4,z_bnd->f1=z_vd;{
# 1438
struct Cyc_Absyn_Exp*y2_exp=Cyc_Absyn_varb_exp((void*)y_bnd,0U);y2_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*zero1_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp1_exp=Cyc_Absyn_prim2_exp(5U,y2_exp,zero1_exp,0U);
zero1_exp->topt=Cyc_Absyn_sint_type;
comp1_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp1_exp);{
# 1445
struct Cyc_Absyn_Exp*z_exp=Cyc_Absyn_varb_exp((void*)z_bnd,0U);z_exp->topt=deref_x->topt;{
struct Cyc_Absyn_Exp*zero2_exp=Cyc_Absyn_signed_int_exp(0,0U);
struct Cyc_Absyn_Exp*comp2_exp=Cyc_Absyn_prim2_exp(6U,z_exp,zero2_exp,0U);
zero2_exp->topt=Cyc_Absyn_sint_type;
comp2_exp->topt=Cyc_Absyn_sint_type;
Cyc_Toc_exp_to_c(nv,comp2_exp);{
# 1452
struct Cyc_List_List*xsizeargs=({struct Cyc_Absyn_Exp*_Tmp1[2];({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_varb_exp((void*)x_bnd,0U);_Tmp1[0]=_Tmp2;}),({
struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp1[1]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct Cyc_Absyn_Exp*),2));});
struct Cyc_Absyn_Exp*oneexp=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
xsize=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_fncall_exp(Cyc_Toc__get_fat_size_e,xsizeargs,0U);Cyc_Absyn_prim2_exp(5U,_Tmp1,oneexp,0U);});{
# 1459
struct Cyc_Absyn_Exp*comp_exp=({struct Cyc_Absyn_Exp*_Tmp1=xsize;Cyc_Absyn_and_exp(_Tmp1,Cyc_Absyn_and_exp(comp1_exp,comp2_exp,0U),0U);});
struct Cyc_Absyn_Stmt*thr_stmt=Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U);
struct Cyc_Absyn_Exp*xcurr=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_varb_exp((void*)x_bnd,0U);Cyc_Toc_member_exp(_Tmp1,Cyc_Toc_curr_sp,0U);});
xcurr=Cyc_Toc_cast_it(c_ptr_type,xcurr);{
struct Cyc_Absyn_Exp*deref_xcurr=Cyc_Absyn_deref_exp(xcurr,0U);
struct Cyc_Absyn_Exp*asn_exp=({struct Cyc_Absyn_Exp*_Tmp1=deref_xcurr;Cyc_Absyn_assign_exp(_Tmp1,Cyc_Absyn_var_exp(z,0U),0U);});
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(asn_exp,0U);
s=({struct Cyc_Absyn_Stmt*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2=comp_exp;struct Cyc_Absyn_Stmt*_Tmp3=thr_stmt;Cyc_Absyn_ifthenelse_stmt(_Tmp2,_Tmp3,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=z_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=y_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
s=({struct Cyc_Absyn_Decl*_Tmp1=({struct Cyc_Absyn_Decl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp4->tag=0,_Tmp4->f1=x_vd;_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=0U;_Tmp2;});Cyc_Absyn_decl_stmt(_Tmp1,s,0U);});
({void*_Tmp1=Cyc_Toc_stmt_exp_r(s);e->r=_Tmp1;});}}}}}}}}}}}}}}}}
# 1413
;_pop_region();}}}
# 1485 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _fat_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned)){
# 1489
struct Cyc_Absyn_Aggrdecl*ad;
{void*_Tmp0=Cyc_Absyn_compress(aggrtype);union Cyc_Absyn_AggrInfo _Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
ad=Cyc_Absyn_get_known_aggrdecl(info);goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting union but found ",sizeof(char),27U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)aggrtype;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" in check_tagged_union",sizeof(char),23U);_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;({
# 1492
int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),3));});}_LL0:;}{
# 1495
struct _tuple30 _Tmp0=Cyc_Toc_temp_var_and_exp();void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct _tuple1*temp=_Tmp2;struct Cyc_Absyn_Exp*temp_exp=_Tmp1;
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*temp_f_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);
struct Cyc_Absyn_Exp*temp_f_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_f_val,0U),0U);
struct Cyc_Absyn_Stmt*ifs=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});
void*e1_p_type=Cyc_Absyn_cstar_type(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*e1_f=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_Tmp3=temp;void*_Tmp4=e1_p_type;struct Cyc_Absyn_Exp*_Tmp5=e1_f;Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});
return Cyc_Toc_stmt_exp_r(s);}else{
# 1508
struct Cyc_Absyn_Exp*temp_f_tag=({struct Cyc_Absyn_Exp*_Tmp3=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_Tmp3,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_f_tag,f_tag,0U);
struct Cyc_Absyn_Exp*temp_f_val=({struct Cyc_Absyn_Exp*_Tmp3=aggrproj(temp_exp,f,0U);Cyc_Toc_member_exp(_Tmp3,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*sres=Cyc_Absyn_exp_stmt(temp_f_val,0U);
struct Cyc_Absyn_Stmt*ifs=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_Tmp3=temp;void*_Tmp4=e1_c_type;struct Cyc_Absyn_Exp*_Tmp5=e1;Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_seq_stmt(ifs,sres,0U),0U);});
return Cyc_Toc_stmt_exp_r(s);}}}}
# 1518
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _fat_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1521
void*_Tmp0=Cyc_Absyn_compress(t);union Cyc_Absyn_AggrInfo _Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
# 1523
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
({int _Tmp2=Cyc_Toc_get_member_offset(ad,f);*f_tag=_Tmp2;});{
# 1526
struct _fat_ptr str=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)*(*ad->name).f1;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)*f;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("_union_%s_%s",sizeof(char),13U),_tag_fat(_Tmp4,sizeof(void*),2));});
({void*_Tmp2=Cyc_Absyn_strct(({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=str;_Tmp3;}));*tagged_member_type=_Tmp2;});
if(clear_read)*_check_null(is_read)=0;
return _check_null(ad->impl)->tagged;}}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 1537
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1540
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("cast on lhs!",sizeof(char),13U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int*is_read=_Tmp1;
# 1543
return Cyc_Toc_is_tagged_union_project_impl(_check_null(e1->topt),f,f_tag,tagged_member_type,clear_read,is_read);}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct _fat_ptr*f=_Tmp2;int*is_read=(int*)_Tmp1;
# 1546
void*_Tmp4=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp5;if(*((int*)_Tmp4)==3){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.elt_type;{void*et=_Tmp5;
# 1548
return Cyc_Toc_is_tagged_union_project_impl(et,f,f_tag,tagged_member_type,clear_read,is_read);}}else{
# 1550
return 0;};}default:
# 1552
 return 0;};}
# 1565 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1569
struct _tuple30 _Tmp0=Cyc_Toc_temp_var_and_exp();void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct _tuple1*temp=_Tmp2;struct Cyc_Absyn_Exp*temp_exp=_Tmp1;
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt==0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);else{
# 1578
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
s2=({struct Cyc_Absyn_Exp*_Tmp3=test_exp;struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_Tmp3,_Tmp4,Cyc_Toc_skip_stmt_dl(),0U);});}{
# 1581
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_Tmp3=temp;void*_Tmp4=Cyc_Absyn_cstar_type(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_Tmp5=
Cyc_Toc_push_address_exp(e1);
# 1581
Cyc_Absyn_declare_stmt(_Tmp3,_Tmp4,_Tmp5,
# 1583
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);}}}struct _tuple34{void*f0;void*f1;};struct _tuple35{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Exp*f1;};
# 1588
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
if(e->topt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("exp_to_c: no type for ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp3;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});{
void*old_typ=e->topt;
# 1593
int did_inserted_checks=0;
{void*_Tmp0=e->annot;if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_Absyn_EmptyAnnot)
goto _LL4;else{if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NoCheck){_LL4:
 did_inserted_checks=1;goto _LL0;}else{
goto _LL0;}}_LL0:;}
# 1599
{void*_Tmp0=e->r;enum Cyc_Absyn_MallocKind _Tmp1;int _Tmp2;enum Cyc_Absyn_Coercion _Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Incrementor _Tmp8;enum Cyc_Absyn_Primop _Tmp9;void*_TmpA;switch(*((int*)_Tmp0)){case 2:
# 1601
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL7;case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Null_c.tag==1){
# 1607
struct Cyc_Absyn_Exp*zero=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_fat_pointer_type(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=Cyc_Toc_make_toplevel_dyn_arr(old_typ,zero,zero)->r;e->r=_TmpB;});else{
# 1612
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];_TmpC[0]=zero,_TmpC[1]=zero,_TmpC[2]=zero;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});e->r=_TmpB;});}}else{
# 1614
e->r=(void*)& Cyc_Toc_zero_exp;}
goto _LL7;}else{
goto _LL7;}case 1:
 goto _LL7;case 41:
({void*_TmpB=Cyc_Absyn_int_exp(0U,1,0U)->r;e->r=_TmpB;});goto _LL7;case 42:
({void*_TmpB=Cyc_Absyn_int_exp(0U,0,0U)->r;e->r=_TmpB;});goto _LL7;case 40: _TmpA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 3: _Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp9;struct Cyc_List_List*es=_TmpA;
# 1623
struct Cyc_List_List*old_types=({struct Cyc_List_List*(*_TmpB)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;_TmpB;})(Cyc_Toc_get_cyc_type,es);
# 1625
({void(*_TmpB)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;_TmpB;})(Cyc_Toc_exp_to_c,nv,es);
switch((int)p){case Cyc_Absyn_Tagof:  {
# 1630
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
({void*_TmpB=Cyc_Absyn_deref_exp(({void*_TmpC=({void*_TmpD=Cyc_Absyn_uint_type;Cyc_Absyn_cstar_type(_TmpD,Cyc_Absyn_const_tqual(0U));});Cyc_Toc_cast_it(_TmpC,arg);}),0U)->r;e->r=_TmpB;});
goto _LL62;}case Cyc_Absyn_Numelts:  {
# 1634
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
{void*_TmpB=Cyc_Absyn_compress(_check_null(arg->topt));void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;if(*((int*)_TmpB)==3){_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.elt_type;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.ptr_atts.nullable;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.ptr_atts.bounds;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.ptr_atts.zero_term;{void*elt_type=_TmpF;void*nbl=_TmpE;void*bound=_TmpD;void*zt=_TmpC;
# 1637
struct Cyc_Absyn_Exp*eopt=({void*_Tmp10=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp10,bound);});
if(eopt==0)
# 1640
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[2];_Tmp11[0]=(struct Cyc_Absyn_Exp*)es->hd,({
struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp11[1]=_Tmp12;});Cyc_Toc_fncall_exp_r(Cyc_Toc__get_fat_size_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1640
e->r=_Tmp10;});else{
# 1642
if(Cyc_Tcutil_force_type2bool(0,zt)){
# 1644
if(Cyc_Toc_isCharStar((struct Cyc_Absyn_Exp*)es->hd))
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[2];_Tmp11[0]=(struct Cyc_Absyn_Exp*)es->hd,_Tmp11[1]=eopt;Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_char_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_Tmp10;});else{
# 1647
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[3];({
struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(Cyc_Tcutil_pointer_elt_type((void*)_check_null(old_types)->hd)),0U);_Tmp11[0]=_Tmp12;}),_Tmp11[1]=(struct Cyc_Absyn_Exp*)es->hd,_Tmp11[2]=eopt;Cyc_Toc_fncall_exp_r(Cyc_Toc__get_zero_arr_size_other_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1647
e->r=_Tmp10;});}}else{
# 1650
if(Cyc_Tcutil_force_type2bool(0,nbl)){
if(!Cyc_Evexp_c_can_eval(eopt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=_tag_fat("can't calculate numelts",sizeof(char),24U);_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp11,sizeof(void*),1));});
# 1654
({void*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11=arg;struct Cyc_Absyn_Exp*_Tmp12=eopt;Cyc_Toc_conditional_exp_r(_Tmp11,_Tmp12,Cyc_Absyn_uint_exp(0U,0U));});e->r=_Tmp10;});}else{
# 1656
e->r=eopt->r;}}}
goto _LL79;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=_tag_fat("numelts primop applied to non-pointer ",sizeof(char),39U);_Tmp11;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp12;_Tmp12.tag=2,_Tmp12.f1=(void*)
_check_null(arg->topt);_Tmp12;});void*_Tmp12[2];_Tmp12[0]=& _Tmp10,_Tmp12[1]=& _Tmp11;({
# 1658
int(*_Tmp13)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp13;})(_tag_fat(_Tmp12,sizeof(void*),2));});}_LL79:;}
# 1661
goto _LL62;}case Cyc_Absyn_Plus:
# 1666
 if(Cyc_Toc_is_toplevel(nv))
({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_TmpB;})(_tag_fat("can't do pointer arithmetic at top-level",sizeof(char),41U));
{void*_TmpB=Cyc_Absyn_compress((void*)_check_null(old_types)->hd);void*_TmpC;void*_TmpD;void*_TmpE;if(*((int*)_TmpB)==3){_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.elt_type;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.ptr_atts.bounds;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1.ptr_atts.zero_term;{void*elt_type=_TmpE;void*b=_TmpD;void*zt=_TmpC;
# 1670
struct Cyc_Absyn_Exp*eopt=({void*_TmpF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpF,b);});
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd;
if(eopt==0)
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[3];_Tmp10[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_Tmp10[1]=_Tmp11;}),_Tmp10[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1674
e->r=_TmpF;});else{
# 1676
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(Cyc_Toc_isCharStar(e1))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[3];_Tmp10[0]=e1,_Tmp10[1]=eopt,_Tmp10[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_plus_char_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),3));});e->r=_TmpF;});else{
# 1680
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[3];({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(Cyc_Tcutil_pointer_elt_type((void*)old_types->hd)),0U);_Tmp10[0]=_Tmp11;}),_Tmp10[1]=eopt,_Tmp10[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_plus_other_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1680
e->r=_TmpF;});}}}
# 1684
goto _LL7E;}}else{
goto _LL7E;}_LL7E:;}
# 1687
goto _LL62;case Cyc_Absyn_Minus:  {
# 1692
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt((void*)_check_null(old_types)->hd,& elt_type)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd;
if(Cyc_Tcutil_is_fat_pointer_type((void*)_check_null(old_types->tl)->hd)){
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_TmpC,Cyc_Toc_curr_sp);});e1->r=_TmpB;});
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_TmpC,Cyc_Toc_curr_sp);});e2->r=_TmpB;});
({void*_TmpB=({void*_TmpC=Cyc_Absyn_cstar_type(Cyc_Absyn_uchar_type,Cyc_Toc_mt_tq);e2->topt=_TmpC;});e1->topt=_TmpB;});
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_TmpC,
Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U),0U);})->r;
# 1700
e->r=_TmpB;});}else{
# 1703
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];_TmpC[0]=e1,({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_type),0U);_TmpC[1]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_prim1_exp(2U,e2,0U);_TmpC[2]=_TmpD;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_plus_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1703
e->r=_TmpB;});}}
# 1707
goto _LL62;}case Cyc_Absyn_Eq:
 goto _LL6E;case Cyc_Absyn_Neq: _LL6E: goto _LL70;case Cyc_Absyn_Gt: _LL70: goto _LL72;case Cyc_Absyn_Gte: _LL72: goto _LL74;case Cyc_Absyn_Lt: _LL74: goto _LL76;case Cyc_Absyn_Lte: _LL76: {
# 1710
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd;
void*t1=(void*)_check_null(old_types)->hd;
void*t2=(void*)_check_null(old_types->tl)->hd;
void*elt_type=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& elt_type)){
void*t=({void*_TmpB=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_TmpB,Cyc_Toc_mt_tq);});
({void*_TmpB=({void*_TmpC=t;Cyc_Toc_cast_it_r(_TmpC,({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_TmpD,Cyc_Toc_curr_sp,0U);}));});e1->r=_TmpB;});
e1->topt=t;}
# 1720
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
void*t=({void*_TmpB=Cyc_Toc_typ_to_c(elt_type);Cyc_Absyn_cstar_type(_TmpB,Cyc_Toc_mt_tq);});
({void*_TmpB=({void*_TmpC=t;Cyc_Toc_cast_it_r(_TmpC,({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_TmpD,Cyc_Toc_curr_sp,0U);}));});e2->r=_TmpB;});
e2->topt=t;}
# 1725
goto _LL62;}default:
 goto _LL62;}_LL62:;
# 1728
goto _LL7;}case 5: _TmpA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_TmpA;enum Cyc_Absyn_Incrementor incr=_Tmp8;
# 1730
void*e2_cyc_typ=_check_null(e2->topt);
# 1739 "toc.cyc"
void*ignore_typ=Cyc_Absyn_void_type;
int ignore_bool=0;
int ignore_int=0;
struct _fat_ptr incr_str=_tag_fat("increment",sizeof(char),10U);
if((int)incr==2 ||(int)incr==3)incr_str=_tag_fat("decrement",sizeof(char),10U);
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& ignore_typ,& ignore_bool,& ignore_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("in-place ",sizeof(char),10U);_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=incr_str;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat(" is not supported when ",sizeof(char),24U);_TmpE;});struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=
_tag_fat("dereferencing a zero-terminated pointer",sizeof(char),40U);_TmpF;});void*_TmpF[4];_TmpF[0]=& _TmpB,_TmpF[1]=& _TmpC,_TmpF[2]=& _TmpD,_TmpF[3]=& _TmpE;({
# 1745
int(*_Tmp10)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp10;})(e->loc,_tag_fat(_TmpF,sizeof(void*),4));});
# 1747
if(Cyc_Toc_is_tagged_union_project(e2,& ignore_int,& ignore_typ,1)){
struct Cyc_Absyn_Exp*one=Cyc_Absyn_signed_int_exp(1,0U);
enum Cyc_Absyn_Primop op;
one->topt=Cyc_Absyn_sint_type;
switch((int)incr){case Cyc_Absyn_PreInc:
 op=0U;goto _LL83;case Cyc_Absyn_PreDec:
 op=2U;goto _LL83;default:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("in-place post",sizeof(char),14U);_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=incr_str;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=
_tag_fat(" is not supported on @tagged union members",sizeof(char),43U);_TmpE;});void*_TmpE[3];_TmpE[0]=& _TmpB,_TmpE[1]=& _TmpC,_TmpE[2]=& _TmpD;({
# 1754
int(*_TmpF)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_TmpF;})(e->loc,_tag_fat(_TmpE,sizeof(void*),3));});}_LL83:;
# 1757
({void*_TmpB=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct));_TmpC->tag=4,_TmpC->f1=e2,({struct Cyc_Core_Opt*_TmpD=({struct Cyc_Core_Opt*_TmpE=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpE->v=(void*)op;_TmpE;});_TmpC->f2=_TmpD;}),_TmpC->f3=one;_TmpC;});e->r=_TmpB;});
Cyc_Toc_exp_to_c(nv,e);
return;}
# 1761
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_set_lhs(nv,0);{
# 1766
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
int change=1;
struct Cyc_Absyn_Exp*fn_e=((int)incr==1 ||(int)incr==3)?Cyc_Toc__fat_ptr_inplace_plus_post_e: Cyc_Toc__fat_ptr_inplace_plus_e;
# 1771
if((int)incr==2 ||(int)incr==3)
change=-1;
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e2);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_TmpC[1]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_signed_int_exp(change,0U);_TmpC[2]=_TmpD;});Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1773
e->r=_TmpB;});}else{
# 1776
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1782
did_inserted_checks=1;
if((int)incr!=1){
struct _tuple1*x=Cyc_Toc_temp_var();
void*t=({void*_TmpB=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_type(_TmpB,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*xexp=Cyc_Toc_push_address_exp(e2);
struct Cyc_Absyn_Exp*testexp=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U);Cyc_Absyn_neq_exp(_TmpB,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
if(Cyc_Toc_do_null_check(e))
testexp=({struct Cyc_Absyn_Exp*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U);Cyc_Absyn_neq_exp(_TmpC,
Cyc_Absyn_int_exp(2U,0,0U),0U);});
# 1790
Cyc_Absyn_and_exp(_TmpB,testexp,0U);});{
# 1792
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_TmpB=testexp;struct Cyc_Absyn_Stmt*_TmpC=
Cyc_Absyn_exp_stmt(Cyc_Absyn_increment_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U,0U),0U);
# 1792
Cyc_Absyn_ifthenelse_stmt(_TmpB,_TmpC,
# 1794
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__throw_arraybounds_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);});
s=({struct Cyc_Absyn_Stmt*_TmpB=s;Cyc_Absyn_seq_stmt(_TmpB,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0U),0U),0U),0U);});
({void*_TmpB=Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,xexp,s,0U),0U)->r;e->r=_TmpB;});}}else{
# 1799
if(Cyc_Toc_isCharStar(e2))
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[2];({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e2);_TmpC[0]=_TmpD;}),({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_signed_int_exp(1,0U);_TmpC[1]=_TmpD;});Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_post_char_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),2));});
# 1800
e->r=_TmpB;});else{
# 1803
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(e2_cyc_typ);Cyc_Toc_cast_it_r(_TmpC,({struct Cyc_Absyn_Exp*_TmpD[3];({
# 1805
struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_sizeoftype_exp(Cyc_Tcutil_pointer_elt_type(Cyc_Toc_typ_to_c(e2_cyc_typ)),0U);_TmpD[0]=_TmpE;}),({
struct Cyc_Absyn_Exp*_TmpE=Cyc_Toc_push_address_exp(e2);_TmpD[1]=_TmpE;}),({
struct Cyc_Absyn_Exp*_TmpE=Cyc_Absyn_signed_int_exp(1,0U);_TmpD[2]=_TmpE;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__zero_arr_inplace_plus_post_other_e,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1803
e->r=_TmpB;});}}}else{
# 1809
if(elt_typ==Cyc_Absyn_void_type && !Cyc_Absyn_is_lvalue(e2)){
({void(*_TmpB)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor))Cyc_Toc_lvalue_assign;_TmpB;})(e2,0,Cyc_Toc_incr_lvalue,incr);
*e=*e2;}}}
# 1813
goto _LL7;}}case 4: _TmpA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Core_Opt*popt=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1832 "toc.cyc"
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
int f_tag=0;
void*tagged_member_struct_type=Cyc_Absyn_void_type;
if(Cyc_Toc_is_tagged_union_project(e1,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);
({void*_TmpB=Cyc_Toc_tagged_union_assignop(e1,e1_old_typ,popt,e2,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_TmpB;});
# 1843
goto _LL7;}{
# 1845
void*ptr_type=Cyc_Absyn_void_type;
void*elt_type=Cyc_Absyn_void_type;
int is_fat=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& ptr_type,& is_fat,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,e1,popt,e2,ptr_type,is_fat,elt_type);
return;}{
# 1854
int e1_poly=Cyc_Toc_is_poly_project(e1);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,e2);{
# 1860
int done=0;
if(popt!=0){
void*elt_typ=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _TmpB=(enum Cyc_Absyn_Primop)popt->v;switch((int)_TmpB){case Cyc_Absyn_Plus:
 change=e2;goto _LL8A;case Cyc_Absyn_Minus:
 change=Cyc_Absyn_prim1_exp(2U,e2,0U);goto _LL8A;default:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("bad t ? pointer arithmetic",sizeof(char),27U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL8A:;}
# 1870
done=1;{
# 1872
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc__fat_ptr_inplace_plus_e;
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Toc_push_address_exp(e1);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_TmpC[1]=_TmpD;}),_TmpC[2]=change;Cyc_Toc_fncall_exp_r(fn_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1873
e->r=_TmpB;});}}else{
# 1876
if(Cyc_Tcutil_is_zero_pointer_type_elt(old_typ,& elt_typ)){
# 1879
enum Cyc_Absyn_Primop _TmpB=(enum Cyc_Absyn_Primop)popt->v;if(_TmpB==Cyc_Absyn_Plus){
# 1881
done=1;
if(Cyc_Toc_isCharStar(e1))
({void*_TmpC=({struct Cyc_Absyn_Exp*_TmpD[2];_TmpD[0]=e1,_TmpD[1]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__zero_arr_inplace_plus_char_e,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),2));});e->r=_TmpC;});else{
# 1886
({void*_TmpC=({void*_TmpD=Cyc_Toc_typ_to_c(e1_old_typ);Cyc_Toc_cast_it_r(_TmpD,({struct Cyc_Absyn_Exp*_TmpE[3];({
# 1888
struct Cyc_Absyn_Exp*_TmpF=Cyc_Absyn_sizeoftype_exp(Cyc_Tcutil_pointer_elt_type(Cyc_Toc_typ_to_c(e1_old_typ)),0U);_TmpE[0]=_TmpF;}),_TmpE[1]=e1,_TmpE[2]=e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__zero_arr_inplace_plus_other_e,_tag_fat(_TmpE,sizeof(struct Cyc_Absyn_Exp*),3));}));});
# 1886
e->r=_TmpC;});}
# 1890
goto _LL91;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("bad zero-terminated pointer arithmetic",sizeof(char),39U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL91:;}}}
# 1894
if(!done){
# 1896
if(e1_poly)
({void*_TmpB=({void*_TmpC=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_TmpC,Cyc_Absyn_new_exp(e2->r,0U));});e2->r=_TmpB;});
# 1902
if(!Cyc_Absyn_is_lvalue(e1)){
({void(*_TmpB)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*)=({void(*_TmpC)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,struct _tuple33*),struct _tuple33*))Cyc_Toc_lvalue_assign;_TmpC;});struct Cyc_Absyn_Exp*_TmpC=e1;_TmpB(_TmpC,0,Cyc_Toc_assignop_lvalue,({struct _tuple33*_TmpD=_cycalloc(sizeof(struct _tuple33));_TmpD->f0=popt,_TmpD->f1=e2;_TmpD;}));});
*e=*e1;}}
# 1907
goto _LL7;}}}}case 6: _TmpA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;struct Cyc_Absyn_Exp*e3=_Tmp6;
# 1909
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
Cyc_Toc_exp_to_c(nv,e3);
goto _LL7;}case 7: _TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
_TmpA=e1;_Tmp7=e2;goto _LL21;}case 8: _TmpA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL21: {struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
_TmpA=e1;_Tmp7=e2;goto _LL23;}case 9: _TmpA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL23: {struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1916
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
goto _LL7;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==0){_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp7;
# 1920
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[1];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_copy_exp(e1);_TmpC[0]=_TmpD;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_TmpB;});
({void(*_TmpB)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*,struct Cyc_List_List*))Cyc_List_iter_c;_TmpB;})(Cyc_Toc_exp_to_c,nv,es);
goto _LL7;}}else{_TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->num_varargs;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->injectors;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3->vai;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp7;int num_varargs=_Tmp5;struct Cyc_List_List*injectors=_Tmp6;struct Cyc_Absyn_VarargInfo*vai=_Tmp4;
# 1936 "toc.cyc"
struct _RegionHandle _TmpB=_new_region(0U,"r");struct _RegionHandle*r=& _TmpB;_push_region(r);{
struct _tuple30 _TmpC=Cyc_Toc_temp_var_and_exp();void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{struct _tuple1*argv=_TmpE;struct Cyc_Absyn_Exp*argvexp=_TmpD;
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_signed_int_exp(num_varargs,0U);
void*cva_type=Cyc_Toc_typ_to_c(vai->type);
void*arr_type=Cyc_Absyn_array_type(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_type,0U);
# 1943
int num_args=Cyc_List_length(es);
int num_normargs=num_args - num_varargs;
# 1947
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,es=es->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(es)->hd);
new_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(struct Cyc_Absyn_Exp*)es->hd,_TmpF->tl=new_args;_TmpF;});}}
# 1952
new_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp10=({struct Cyc_Absyn_Exp*_Tmp11[3];
num_varargs==0?({struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_uint_exp(0U,0U);_Tmp11[0]=_Tmp12;}):(_Tmp11[0]=argvexp),({
struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_sizeoftype_exp(cva_type,0U);_Tmp11[1]=_Tmp12;}),_Tmp11[2]=num_varargs_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp11,sizeof(struct Cyc_Absyn_Exp*),3));});
# 1952
_TmpF->hd=_Tmp10;}),_TmpF->tl=new_args;_TmpF;});
# 1957
new_args=Cyc_List_imp_rev(new_args);{
# 1959
void*e1_typ=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];({struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_copy_exp(e1);_Tmp10[0]=_Tmp11;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e1->r=_TmpF;});
if(num_varargs==0){
({void*_TmpF=Cyc_Absyn_fncall_exp(e1,new_args,0U)->r;e->r=_TmpF;});
_npop_handler(0);goto _LL7;}{
# 1968
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(e1,new_args,0U),0U);
# 1971
if(vai->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_TmpF=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vai->type));void*_Tmp10;if(*((int*)_TmpF)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)->f1.KnownDatatype.tag==2){_Tmp10=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*x=_Tmp10;
tud=x;goto _LL99;}}else{goto _LL9C;}}else{goto _LL9C;}}else{_LL9C:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=_tag_fat("unknown datatype in vararg with inject",sizeof(char),39U);_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;({int(*_Tmp13)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp13;})(_tag_fat(_Tmp12,sizeof(void*),1));});}_LL99:;}{
# 1977
struct _fat_ptr vs=({unsigned _TmpF=(unsigned)num_varargs;_tag_fat(({struct _tuple1**_Tmp10=({struct _RegionHandle*_Tmp11=r;_region_malloc(_Tmp11,0U,_check_times(_TmpF,sizeof(struct _tuple1*)));});({{unsigned _Tmp11=(unsigned)num_varargs;unsigned i;for(i=0;i < _Tmp11;++ i){({struct _tuple1*_Tmp12=Cyc_Toc_temp_var();_Tmp10[i]=_Tmp12;});}}0;});_Tmp10;}),sizeof(struct _tuple1*),_TmpF);});
# 1981
struct Cyc_List_List*array_args=0;
{int i=num_varargs - 1;for(0;i >= 0;-- i){
array_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_fat_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_TmpF->hd=_Tmp10;}),_TmpF->tl=array_args;_TmpF;});}}
s=({struct _tuple1*_TmpF=argv;void*_Tmp10=arr_type;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_array_exp(array_args,0U);Cyc_Absyn_declare_stmt(_TmpF,_Tmp10,_Tmp11,s,0U);});
# 1986
es=Cyc_List_imp_rev(es);
injectors=Cyc_List_imp_rev(injectors);{
int i=Cyc_List_length(es)- 1;
for(1;es!=0;(es=es->tl,injectors=injectors->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)es->hd;
void*arg_type=arg->topt;
struct _tuple1*var=((struct _tuple1**)vs.curr)[i];
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_TmpF=(struct Cyc_Absyn_Datatypefield*)injectors->hd;void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF->name;_Tmp10=_TmpF->typs;{struct _tuple1*qv=_Tmp11;struct Cyc_List_List*tqts=_Tmp10;
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f1);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);{
# 2000
struct _tuple1*tdn=Cyc_Toc_collapse_qvars(qv,tud->name);
struct Cyc_List_List*dles=({struct _tuple19*_Tmp12[2];({struct _tuple19*_Tmp13=({struct _fat_ptr*_Tmp14=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_Tmp14,Cyc_Toc_datatype_tag(tud,qv));});_Tmp12[0]=_Tmp13;}),({
struct _tuple19*_Tmp13=({struct _fat_ptr*_Tmp14=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_Tmp14,arg);});_Tmp12[1]=_Tmp13;});Cyc_List_list(_tag_fat(_Tmp12,sizeof(struct _tuple19*),2));});
s=({struct _tuple1*_Tmp12=var;void*_Tmp13=Cyc_Absyn_strctq(tdn);struct Cyc_Absyn_Exp*_Tmp14=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp15->tag=28,_Tmp15->f1=tdn,_Tmp15->f2=0,_Tmp15->f3=dles,_Tmp15->f4=0;_Tmp15;}),0U);
# 2003
Cyc_Absyn_declare_stmt(_Tmp12,_Tmp13,_Tmp14,s,0U);});}}}}}}else{
# 2010
struct Cyc_List_List*array_args=0;
for(1;es!=0;es=es->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)es->hd);
array_args=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(struct Cyc_Absyn_Exp*)es->hd,_TmpF->tl=array_args;_TmpF;});}{
# 2015
struct Cyc_Absyn_Exp*init=Cyc_Absyn_array_exp(Cyc_List_imp_rev(array_args),0U);
s=Cyc_Absyn_declare_stmt(argv,arr_type,init,s,0U);}}
# 2018
({void*_TmpF=Cyc_Toc_stmt_exp_r(s);e->r=_TmpF;});}}}}
# 2020
_npop_handler(0);goto _LL7;
# 1936
;_pop_region();}}case 11: _TmpA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;int b=_Tmp5;
# 2023
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*fn_e=b?Cyc_Toc__rethrow_e: Cyc_Toc__throw_e;
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_TmpC,({struct Cyc_Absyn_Exp*_TmpD[1];_TmpD[0]=e1;Cyc_Toc_fncall_exp_dl(fn_e,_tag_fat(_TmpD,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);})->r;e->r=_TmpB;});
goto _LL7;}}case 12: _TmpA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 13: _TmpA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*ts=_Tmp7;
# 2029
Cyc_Toc_exp_to_c(nv,e1);{
# 2038 "toc.cyc"
void*tc=Cyc_Toc_typ_to_c(_check_null(e->topt));
void*tc1=Cyc_Toc_typ_to_c(_check_null(e1->topt));
if(!Cyc_Tcutil_typecmp(tc,tc1)==0){
# 2050 "toc.cyc"
struct _tuple30 _TmpB=Cyc_Toc_temp_var_and_exp();void*_TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct _tuple1*temp=_TmpD;struct Cyc_Absyn_Exp*temp_exp=_TmpC;
({void*_TmpE=Cyc_Toc_stmt_exp_r(({struct _tuple1*_TmpF=temp;void*_Tmp10=tc;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Toc_array_to_ptr_cast(tc,e1,0U);struct Cyc_Absyn_Stmt*_Tmp12=
Cyc_Absyn_exp_stmt(temp_exp,e->loc);
# 2051
Cyc_Absyn_declare_stmt(_TmpF,_Tmp10,_Tmp11,_Tmp12,e->loc);}));e->r=_TmpE;});}}
# 2054
goto _LL7;}}case 14: _TmpA=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void**t=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;int user_inserted=_Tmp5;enum Cyc_Absyn_Coercion coercion=_Tmp3;
# 2056
void*old_t2=_check_null(e1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*t;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*t=new_typ_c;
Cyc_Toc_exp_to_c(nv,e1);
# 2063
{struct _tuple34 _TmpB=({struct _tuple34 _TmpC;({void*_TmpD=Cyc_Absyn_compress(old_t2);_TmpC.f0=_TmpD;}),({void*_TmpD=Cyc_Absyn_compress(new_typ);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_Absyn_PtrInfo _TmpC;struct Cyc_Absyn_PtrInfo _TmpD;if(*((int*)_TmpB.f0)==3)switch(*((int*)_TmpB.f1)){case 3: _TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f0)->f1;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_TmpD;struct Cyc_Absyn_PtrInfo p2=_TmpC;
# 2065
struct Cyc_Absyn_Exp*b1=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,p1.ptr_atts.bounds);});
struct Cyc_Absyn_Exp*b2=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,p2.ptr_atts.bounds);});
int zt1=Cyc_Tcutil_force_type2bool(0,p1.ptr_atts.zero_term);
int zt2=Cyc_Tcutil_force_type2bool(0,p2.ptr_atts.zero_term);
{struct _tuple35 _TmpE=({struct _tuple35 _TmpF;_TmpF.f0=b1,_TmpF.f1=b2;_TmpF;});if(_TmpE.f0!=0){if(_TmpE.f1!=0){
# 2072
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];_Tmp10[0]=e1;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));})->r;e->r=_TmpF;});else{
if(Cyc_Unify_unify(old_t2_c,new_typ_c))
*e=*e1;}
# 2081
goto _LLAB;}else{
# 2083
struct Cyc_Absyn_Exp*e2=_check_null(b1);
struct _tuple13 _TmpF=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp10;unsigned _Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{unsigned i=_Tmp11;int valid=_Tmp10;
if(Cyc_Toc_is_toplevel(nv)){
# 2089
if((zt1 && !p2.elt_tq.real_const)&& !zt2)
e2=({struct Cyc_Absyn_Exp*_Tmp12=e2;Cyc_Absyn_prim2_exp(2U,_Tmp12,Cyc_Absyn_uint_exp(1U,0U),0U);});
({void*_Tmp12=Cyc_Toc_make_toplevel_dyn_arr(old_t2,e2,e1)->r;e->r=_Tmp12;});}else{
if(zt1){
# 2098
struct _tuple30 _Tmp12=Cyc_Toc_temp_var_and_exp();void*_Tmp13;void*_Tmp14;_Tmp14=_Tmp12.f0;_Tmp13=_Tmp12.f1;{struct _tuple1*x=_Tmp14;struct Cyc_Absyn_Exp*x_exp=_Tmp13;
struct Cyc_Absyn_Exp*arg3;
# 2102
{void*_Tmp15=e1->r;void*_Tmp16;switch(*((int*)_Tmp15)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp15)->f1.Wstring_c.tag){case 8:
 arg3=e2;goto _LLBA;case 9:
 arg3=e2;goto _LLBA;default: goto _LLC3;}case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)){case 1: _Tmp16=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp16;
_Tmp16=vd;goto _LLC2;}case 4: _Tmp16=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp15)->f1)->f1;_LLC2: {struct Cyc_Absyn_Vardecl*vd=_Tmp16;
# 2107
if(!Cyc_Tcutil_is_array_type(vd->type))
goto _LLC4;
arg3=e2;
goto _LLBA;}default: goto _LLC3;}default: _LLC3: _LLC4:
# 2114
 x_exp=({void*_Tmp17=Cyc_Toc_void_star_type();Cyc_Toc_cast_it(_Tmp17,x_exp);});
if(valid && i!=1U)
arg3=e2;else{
if(Cyc_Toc_isCharStar(e1))
arg3=({struct Cyc_Absyn_Exp*_Tmp17[2];_Tmp17[0]=x_exp,_Tmp17[1]=e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__get_zero_arr_size_char_e,_tag_fat(_Tmp17,sizeof(struct Cyc_Absyn_Exp*),2));});else{
# 2120
arg3=({struct Cyc_Absyn_Exp*_Tmp17[3];({
struct Cyc_Absyn_Exp*_Tmp18=Cyc_Absyn_sizeoftype_exp(Cyc_Tcutil_pointer_elt_type(Cyc_Toc_typ_to_c(old_t2)),0U);_Tmp17[0]=_Tmp18;}),_Tmp17[1]=x_exp,_Tmp17[2]=e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__get_zero_arr_size_other_e,_tag_fat(_Tmp17,sizeof(struct Cyc_Absyn_Exp*),3));});}}
# 2123
goto _LLBA;}_LLBA:;}{
# 2127
int use_temp=arg3!=e2;
if(!zt2 && !p2.elt_tq.real_const)
arg3=({struct Cyc_Absyn_Exp*_Tmp15=arg3;Cyc_Absyn_prim2_exp(2U,_Tmp15,Cyc_Absyn_uint_exp(1U,0U),0U);});{
struct Cyc_Absyn_Exp*arg2=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);
if(use_temp){
struct Cyc_Absyn_Exp*tg_exp=({struct Cyc_Absyn_Exp*_Tmp15[3];_Tmp15[0]=x_exp,_Tmp15[1]=arg2,_Tmp15[2]=arg3;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp15,sizeof(struct Cyc_Absyn_Exp*),3));});
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(tg_exp,0U);
s=({struct _tuple1*_Tmp15=x;void*_Tmp16=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_Tmp17=e1;Cyc_Absyn_declare_stmt(_Tmp15,_Tmp16,_Tmp17,s,0U);});
({void*_Tmp15=Cyc_Toc_stmt_exp_r(s);e->r=_Tmp15;});}else{
# 2137
({void*_Tmp15=({struct Cyc_Absyn_Exp*_Tmp16[3];_Tmp16[0]=e1,_Tmp16[1]=arg2,_Tmp16[2]=arg3;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp16,sizeof(struct Cyc_Absyn_Exp*),3));});e->r=_Tmp15;});}}}}}else{
# 2140
({void*_Tmp12=({struct Cyc_Absyn_Exp*_Tmp13[3];_Tmp13[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp14=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p2.elt_type),0U);_Tmp13[1]=_Tmp14;}),_Tmp13[2]=e2;Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_fat_e,_tag_fat(_Tmp13,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2140
e->r=_Tmp12;});}}
# 2142
goto _LLAB;}}}else{if(_TmpE.f1!=0){
# 2152 "toc.cyc"
struct Cyc_Absyn_Exp*new_e2=_check_null(b2);
if(zt1 && !zt2)
new_e2=({struct Cyc_Absyn_Exp*_TmpF=b2;Cyc_Absyn_add_exp(_TmpF,Cyc_Absyn_uint_exp(1U,0U),0U);});{
struct Cyc_Absyn_Exp*ptr_exp=e1;
# 2160
if(Cyc_Tcutil_is_noalias_qual(p1.ptr_atts.aqual,0)&& !
Cyc_Tcutil_is_noalias_qual(p1.ptr_atts.aqual,1))
ptr_exp=({struct Cyc_Absyn_Exp*_TmpF[1];_TmpF[0]=ptr_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_fat_at_base_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),1));});
# 2168
ptr_exp=({struct Cyc_Absyn_Exp*_TmpF[3];_TmpF[0]=ptr_exp,({
# 2170
struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_TmpF[1]=_Tmp10;}),_TmpF[2]=new_e2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__untag_fat_ptr_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2172
did_inserted_checks=1;
if(Cyc_Toc_do_null_check(e))
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[1];({struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_copy_exp(ptr_exp);_Tmp10[0]=_Tmp11;});Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),1));});ptr_exp->r=_TmpF;});
({void*_TmpF=Cyc_Toc_cast_it_r(*t,ptr_exp);e->r=_TmpF;});
goto _LLAB;}}else{
# 2180
if((zt1 && !zt2)&& !p2.elt_tq.real_const){
if(Cyc_Toc_is_toplevel(nv))
({int(*_TmpF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Toc_unimp;_TmpF;})(_tag_fat("can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",sizeof(char),70U));
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[3];_Tmp10[0]=e1,({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_sizeoftype_exp(Cyc_Toc_typ_to_c(p1.elt_type),0U);_Tmp10[1]=_Tmp11;}),({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(1U,0U);_Tmp10[2]=_Tmp11;});Cyc_Toc_fncall_exp_r(Cyc_Toc__fat_ptr_decrease_size_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),3));});
# 2183
e->r=_TmpF;});}
# 2187
goto _LLAB;}}_LLAB:;}
# 2189
goto _LLA4;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB.f1)->f1)==1){_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB.f0)->f1;{struct Cyc_Absyn_PtrInfo p1=_TmpD;
# 2191
{struct Cyc_Absyn_Exp*_TmpE=({void*_TmpF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpF,p1.ptr_atts.bounds);});if(_TmpE==0){
# 2193
({void*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_new_exp(e1->r,e1->loc);Cyc_Toc_aggrmember_exp_r(_Tmp10,Cyc_Toc_curr_sp);});e1->r=_TmpF;});
e1->topt=new_typ_c;
goto _LLC5;}else{
goto _LLC5;}_LLC5:;}
# 2198
goto _LLA4;}}else{goto _LLA9;}default: goto _LLA9;}else{_LLA9:
# 2200
 if(Cyc_Unify_unify(old_t2_c,new_typ_c))
*e=*e1;
goto _LLA4;}_LLA4:;}
# 2204
goto _LL7;}case 15: _TmpA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2207
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(e1)){
({void(*_TmpB)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int)=(void(*)(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Exp*,int),int))Cyc_Toc_lvalue_assign;_TmpB;})(e1,0,Cyc_Toc_address_lvalue,1);
# 2213
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_TmpC,e1);});e->r=_TmpB;});}else{
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(_check_null(e1->topt))))
# 2216
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_TmpC,e1);});e->r=_TmpB;});}
goto _LL7;}case 16: _TmpA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*rgnopt=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*qual=_Tmp6;
# 2224
Cyc_Toc_exp_to_c(nv,e1);{
# 2226
void*elt_typ=Cyc_Toc_typ_to_c(_check_null(e1->topt));
{void*_TmpB=Cyc_Absyn_compress(elt_typ);void*_TmpC;if(*((int*)_TmpB)==4){_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TmpB)->f1.elt_type;{void*t2=_TmpC;
elt_typ=Cyc_Toc_typ_to_c(t2);goto _LLCA;}}else{
goto _LLCA;}_LLCA:;}{
# 2231
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(e1);
struct _tuple1*lenvar=(unsigned)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*lenexp=(unsigned)array_len?Cyc_Absyn_var_exp(lenvar,0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned)lenexp)
mexp=({struct Cyc_Absyn_Exp*_TmpB[2];_TmpB[0]=lenexp,({struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_TmpB[1]=_TmpC;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__check_times_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),2));});else{
# 2238
mexp=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);}{
# 2241
struct Cyc_Absyn_Exp*vse=Cyc_Toc_get_varsizeexp(e1);
if(vse!=0)
mexp=Cyc_Absyn_add_exp(mexp,vse,0U);
# 2245
if(rgnopt==0 && qual==0 || Cyc_Flags_no_regions)
mexp=Cyc_Toc_malloc_exp(elt_typ,mexp);else{
# 2248
if(qual!=0)Cyc_Toc_exp_to_c(nv,qual);
if(rgnopt!=0){
Cyc_Toc_exp_to_c(nv,rgnopt);
mexp=({struct Cyc_Absyn_Exp*_TmpB=rgnopt;struct Cyc_Absyn_Exp*_TmpC=
qual==0?Cyc_Toc_aliasable_qual_exp(): qual;
# 2251
Cyc_Toc_rmalloc_exp(_TmpB,_TmpC,mexp);});}else{
# 2256
mexp=Cyc_Toc_aqmalloc_exp(qual,mexp);}}
# 2259
({void*_TmpB=(void*)({struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct));_TmpC->tag=Cyc_Toc_NewInfo,_TmpC->f1=mexp,({void*_TmpD=Cyc_Absyn_cstar_type(elt_typ,Cyc_Toc_mt_tq);_TmpC->f2=_TmpD;});_TmpC;});e->annot=_TmpB;});
if(Cyc_Tcutil_is_fat_ptr(old_typ))
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC[3];({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_copy_exp(e);_TmpC[0]=_TmpD;}),({
struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(elt_typ,0U);_TmpC[1]=_TmpD;}),
(unsigned)lenexp?_TmpC[2]=lenexp:({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_uint_exp(1U,0U);_TmpC[2]=_TmpD;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_TmpC,sizeof(struct Cyc_Absyn_Exp*),3));})->r;
# 2261
e->r=_TmpB;});
# 2264
if(vse!=0)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_TmpC,Cyc_Absyn_copy_exp(e));});e->r=_TmpB;});
if((unsigned)array_len){
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=_check_null(lenvar);void*_TmpC=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_TmpD=array_len;Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,
Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U),0U);});
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});}
# 2271
goto _LL7;}}}}case 18: _TmpA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2274
Cyc_Toc_exp_to_c(nv,e1);goto _LL7;}case 17: _TmpA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpA;
({void*_TmpB=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_TmpC->tag=17,({void*_TmpD=Cyc_Toc_typ_to_c(t);_TmpC->f1=_TmpD;});_TmpC;});e->r=_TmpB;});goto _LL7;}case 19: _TmpA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpA;struct Cyc_List_List*fs=_Tmp7;
# 2277
({void*_TmpB=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct));_TmpC->tag=19,({void*_TmpD=Cyc_Toc_typ_to_c(t);_TmpC->f1=_TmpD;}),_TmpC->f2=fs;_TmpC;});e->r=_TmpB;});
goto _LL7;}case 21: _TmpA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp5;int is_read=_Tmp2;
# 2280
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=_check_null(e1->topt);
Cyc_Toc_exp_to_c(nv,e1);
if(is_tagged && is_read)
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=e1;void*_TmpD=Cyc_Toc_typ_to_c(e1_cyc_type);void*_TmpE=e1_cyc_type;struct _fat_ptr*_TmpF=f;Cyc_Toc_check_tagged_union(_TmpC,_TmpD,_TmpE,_TmpF,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2284
e->r=_TmpB;});
# 2286
if(is_poly)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_TmpC,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_TmpB;});
goto _LL7;}case 22: _TmpA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp5;int is_read=_Tmp2;
# 2290
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1typ=_check_null(e1->topt);
void*ta;
{void*_TmpB=Cyc_Absyn_compress(e1typ);struct Cyc_Absyn_PtrInfo _TmpC;if(*((int*)_TmpB)==3){_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpB)->f1;{struct Cyc_Absyn_PtrInfo p=_TmpC;
ta=p.elt_type;goto _LLCF;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("get_ptr_typ: not a pointer!",sizeof(char),28U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({int(*_TmpF)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpF;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLCF:;}
# 2297
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
if(is_tagged && is_read)
({void*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=e1;void*_TmpD=Cyc_Toc_typ_to_c(e1typ);void*_TmpE=ta;struct _fat_ptr*_TmpF=f;Cyc_Toc_check_tagged_union(_TmpC,_TmpD,_TmpE,_TmpF,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_TmpB;});
if(is_poly && is_read)
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_TmpC,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_TmpB;});
goto _LL7;}case 20: _TmpA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 2305
did_inserted_checks=1;
Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,0);
goto _LL7;}case 23: _TmpA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 2309
did_inserted_checks=1;{
int index_used=Cyc_Toc_ptr_use_to_c(nv,e1,e->annot,e2);
if(index_used)
({void*_TmpB=Cyc_Toc_deref_exp_r(e1);e->r=_TmpB;});
goto _LL7;}}case 25: _TmpA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles0=_TmpA;
# 2319
{struct Cyc_List_List*dles=dles0;for(0;dles!=0;dles=dles->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles->hd)).f1);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,dles0);e->r=_TmpB;});
goto _LL7;}case 26: _TmpA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;int iszeroterm=_Tmp5;
# 2327
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _TmpB=Cyc_Evexp_eval_const_uint_exp(e1);int _TmpC;unsigned _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{unsigned sz=_TmpD;int known=_TmpC;
void*elt_typ=Cyc_Toc_typ_to_c(_check_null(e2->topt));
struct Cyc_List_List*es=0;
# 2334
if(!Cyc_Toc_is_zero(e2)){
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("cannot determine value of constant",sizeof(char),35U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(e1->loc,_tag_fat(_TmpF,sizeof(void*),1));});
{unsigned i=0U;for(0;i < sz;++ i){
es=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f0=0,_Tmp10->f1=e2;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=es;_TmpE;});}}
# 2340
if(iszeroterm){
struct Cyc_Absyn_Exp*ezero=({void*_TmpE=elt_typ;Cyc_Toc_cast_it(_TmpE,Cyc_Absyn_uint_exp(0U,0U));});
es=({struct Cyc_List_List*_TmpE=es;Cyc_List_imp_append(_TmpE,({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp10=({struct _tuple19*_Tmp11=_cycalloc(sizeof(struct _tuple19));_Tmp11->f0=0,_Tmp11->f1=ezero;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=0;_TmpF;}));});}}
# 2345
({void*_TmpE=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_TmpE;});}}
# 2347
goto _LL7;}case 27: _TmpA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;void*t1=_Tmp7;int iszeroterm=_Tmp5;
# 2354
if(Cyc_Toc_is_toplevel(nv))
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_TmpB;});else{
# 2357
Cyc_Toc_exp_to_c(nv,e1);}
goto _LL7;}case 29: _TmpA=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{void*st=_TmpA;struct Cyc_List_List*dles=_Tmp7;
# 2361
{struct Cyc_List_List*dles2=dles;for(0;dles2!=0;dles2=dles2->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple19*)dles2->hd)).f1);}}{
void*strct_typ=Cyc_Toc_typ_to_c(_check_null(e->topt));
e->topt=strct_typ;
if(!Cyc_Toc_is_toplevel(nv)){
void*_TmpB=Cyc_Absyn_compress(strct_typ);union Cyc_Absyn_AggrInfo _TmpC;if(*((int*)_TmpB)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)==24){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f2==0){_TmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1;{union Cyc_Absyn_AggrInfo aggrinfo=_TmpC;
# 2368
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(aggrinfo);
({void*_TmpD=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpE->tag=28,_TmpE->f1=sd->name,_TmpE->f2=0,_TmpE->f3=dles,_TmpE->f4=sd;_TmpE;});e->r=_TmpD;});
e->topt=strct_typ;
goto _LLD7;}}else{goto _LLDA;}}else{goto _LLDA;}}else{_LLDA:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("anonStruct type not an aggregate",sizeof(char),33U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({int(*_TmpF)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpF;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLD7:;}else{
# 2375
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,dles);e->r=_TmpB;});}
goto _LL7;}}case 28: _TmpA=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple1**tdn=_TmpA;struct Cyc_List_List*exist_ts=_Tmp7;struct Cyc_List_List*dles=_Tmp6;struct Cyc_Absyn_Aggrdecl*sd=_Tmp4;
# 2381
if(sd==0 || sd->impl==0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("exp_to_c, Aggregate_e: missing aggrdecl pointer or fields",sizeof(char),58U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});{
void*new_typ=Cyc_Toc_typ_to_c(old_typ);
{void*_TmpB=Cyc_Absyn_compress(new_typ);union Cyc_Absyn_AggrInfo _TmpC;if(*((int*)_TmpB)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)==24){_TmpC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_TmpC;
({struct _tuple1*_TmpD=Cyc_Absyn_aggr_kinded_name(info).f1;*tdn=_TmpD;});goto _LLDC;}}else{goto _LLDF;}}else{_LLDF:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("exp_to_c, Aggregate_e: bad type translation",sizeof(char),44U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({int(*_TmpF)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpF;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLDC:;}{
# 2392
struct Cyc_List_List*typ_fields=_check_null(sd->impl)->fields;
if(typ_fields==0)goto _LL7;
for(1;_check_null(typ_fields)->tl!=0;typ_fields=typ_fields->tl){
;}{
struct Cyc_Absyn_Aggrfield*last_typ_field=(struct Cyc_Absyn_Aggrfield*)typ_fields->hd;
struct Cyc_List_List*fields=Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,e->loc,dles,sd->kind,
_check_null(sd->impl)->fields);
if(!Cyc_Toc_is_toplevel(nv)){
void*_TmpB=Cyc_Absyn_compress(old_typ);void*_TmpC;if(*((int*)_TmpB)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f1)==24){_TmpC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpB)->f2;{struct Cyc_List_List*param_ts=_TmpC;
# 2403
{struct Cyc_List_List*fields2=fields;for(0;fields2!=0;fields2=fields2->tl){
struct _tuple36*_TmpD=(struct _tuple36*)fields2->hd;void*_TmpE;void*_TmpF;_TmpF=_TmpD->f0;_TmpE=_TmpD->f1;{struct Cyc_Absyn_Aggrfield*aggrfield=_TmpF;struct Cyc_Absyn_Exp*fieldexp=_TmpE;
void*old_field_typ=fieldexp->topt;
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2408
if(Cyc_Toc_is_void_star_or_boxed_tvar(aggrfield->type)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(_check_null(fieldexp->topt)))
({void*_Tmp10=({void*_Tmp11=Cyc_Toc_typ_to_c(aggrfield->type);Cyc_Toc_cast_it(_Tmp11,
Cyc_Absyn_copy_exp(fieldexp));})->r;
# 2410
fieldexp->r=_Tmp10;});
# 2413
if(_check_null(sd->impl)->tagged){
struct _fat_ptr*n=aggrfield->name;
struct Cyc_Absyn_Exp*tag_exp=Cyc_Absyn_uint_exp((unsigned)Cyc_Toc_get_member_offset(sd,n),0U);
struct _tuple19*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);
struct _tuple19*val_dle=Cyc_Toc_make_field(Cyc_Toc_val_sp,fieldexp);
struct _tuple1*s;s=_cycalloc(sizeof(struct _tuple1)),({union Cyc_Absyn_Nmspace _Tmp10=Cyc_Absyn_Abs_n(0,1);s->f0=_Tmp10;}),({
struct _fat_ptr*_Tmp10=({struct _fat_ptr*_Tmp11=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp12=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp13=({struct Cyc_String_pa_PrintArg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=(struct _fat_ptr)*(*sd->name).f1;_Tmp14;});struct Cyc_String_pa_PrintArg_struct _Tmp14=({struct Cyc_String_pa_PrintArg_struct _Tmp15;_Tmp15.tag=0,_Tmp15.f1=(struct _fat_ptr)*n;_Tmp15;});void*_Tmp15[2];_Tmp15[0]=& _Tmp13,_Tmp15[1]=& _Tmp14;Cyc_aprintf(_tag_fat("_union_%s_%s",sizeof(char),13U),_tag_fat(_Tmp15,sizeof(void*),2));});*_Tmp11=_Tmp12;});_Tmp11;});s->f1=_Tmp10;});{
# 2421
struct _tuple19*u_dle=({
struct _fat_ptr*_Tmp10=n;Cyc_Toc_make_field(_Tmp10,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp11->tag=28,_Tmp11->f1=s,_Tmp11->f2=0,({struct Cyc_List_List*_Tmp12=({struct _tuple19*_Tmp13[2];_Tmp13[0]=tag_dle,_Tmp13[1]=val_dle;Cyc_List_list(_tag_fat(_Tmp13,sizeof(struct _tuple19*),2));});_Tmp11->f3=_Tmp12;}),_Tmp11->f4=0;_Tmp11;}),0U));});
# 2424
({void*_Tmp10=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp11->tag=28,_Tmp11->f1=*tdn,_Tmp11->f2=0,({struct Cyc_List_List*_Tmp12=({struct _tuple19*_Tmp13[1];_Tmp13[0]=u_dle;Cyc_List_list(_tag_fat(_Tmp13,sizeof(struct _tuple19*),1));});_Tmp11->f3=_Tmp12;}),_Tmp11->f4=sd;_Tmp11;});e->r=_Tmp10;});}}
# 2430
if(Cyc_Toc_is_abstract_type(old_typ)&& last_typ_field==aggrfield){
struct Cyc_List_List*inst=({struct Cyc_List_List*_Tmp10=Cyc_List_zip(_check_null(sd->impl)->exist_vars,exist_ts);Cyc_List_append(_Tmp10,
Cyc_List_zip(sd->tvs,param_ts));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*fs=_check_null(sd->impl)->fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*old_f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
void*old_ftyp=Cyc_Tcutil_substitute(inst,old_f->type);
struct Cyc_Absyn_Aggrfield*new_f=({struct Cyc_Absyn_Aggrfield*_Tmp10=old_f;Cyc_Toc_aggrfield_to_c(_Tmp10,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(inst,old_ftyp)));});
new_fields=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=new_f,_Tmp10->tl=new_fields;_Tmp10;});
# 2445
if(fs->tl==0){
{void*_Tmp10=Cyc_Absyn_compress(new_f->type);struct Cyc_Absyn_ArrayInfo _Tmp11;if(*((int*)_Tmp10)==4){_Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp10)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp11;
# 2448
if(!Cyc_Evexp_c_can_eval(_check_null(ai.num_elts))){
struct Cyc_Absyn_ArrayInfo ai2=ai;
({struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_uint_exp(0U,0U);ai2.num_elts=_Tmp12;});
({void*_Tmp12=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp13->tag=4,_Tmp13->f1=ai2;_Tmp13;});new_f->type=_Tmp12;});}
# 2453
goto _LLE9;}}else{
# 2457
if(fieldexp->topt==0)
goto _LLE9;
{void*_Tmp12=Cyc_Absyn_compress(fieldexp->topt);if(*((int*)_Tmp12)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp12)->f1)==24){
# 2461
new_f->type=_check_null(fieldexp->topt);goto _LLEE;}else{goto _LLF1;}}else{_LLF1:
 goto _LLEE;}_LLEE:;}
# 2464
goto _LLE9;}_LLE9:;}
# 2468
if(!Cyc_Tcutil_is_array_type(old_f->type)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_f->type)))
({struct Cyc_List_List*_Tmp10=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp12=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_Tmp13=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_Tmp13->tag=6,_Tmp13->f1=0;_Tmp13;});_Tmp11->hd=_Tmp12;}),_Tmp11->tl=new_f->attributes;_Tmp11;});new_f->attributes=_Tmp10;});}}}
# 2474
sd=({struct _fat_ptr*_Tmp10=({struct _fat_ptr*_Tmp11=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp12=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp13=({struct Cyc_Int_pa_PrintArg_struct _Tmp14;_Tmp14.tag=1,_Tmp14.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_Tmp14;});void*_Tmp14[1];_Tmp14[0]=& _Tmp13;Cyc_aprintf(_tag_fat("_genStruct%d",sizeof(char),13U),_tag_fat(_Tmp14,sizeof(void*),1));});*_Tmp11=_Tmp12;});_Tmp11;});Cyc_Toc_make_c_struct_defn(_Tmp10,
# 2476
Cyc_List_imp_rev(new_fields));});
*tdn=sd->name;
Cyc_Toc_aggrdecl_to_c(sd);
# 2480
({void*_Tmp10=Cyc_Absyn_aggr_type(Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp11=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp11=sd;_Tmp11;})),0);e->topt=_Tmp10;});}}}}
# 2483
goto _LLE1;}}else{goto _LLE4;}}else{_LLE4:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("exp_to_c, Aggregate_e: bad struct type",sizeof(char),39U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({int(*_TmpF)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpF;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LLE1:;}else{
# 2492
if(_check_null(sd->impl)->tagged){
# 2494
struct _tuple36*_TmpB=(struct _tuple36*)_check_null(fields)->hd;void*_TmpC;void*_TmpD;_TmpD=_TmpB->f0;_TmpC=_TmpB->f1;{struct Cyc_Absyn_Aggrfield*field=_TmpD;struct Cyc_Absyn_Exp*fieldexp=_TmpC;
void*fieldexp_type=_check_null(fieldexp->topt);
void*fieldtyp=field->type;
Cyc_Toc_exp_to_c(nv,fieldexp);
if(Cyc_Toc_is_void_star_or_boxed_tvar(fieldtyp)&& !
Cyc_Toc_is_void_star_or_boxed_tvar(fieldexp_type))
({void*_TmpE=({
void*_TmpF=Cyc_Toc_void_star_type();Cyc_Toc_cast_it_r(_TmpF,Cyc_Absyn_new_exp(fieldexp->r,0U));});
# 2500
fieldexp->r=_TmpE;});{
# 2503
int i=Cyc_Toc_get_member_offset(sd,field->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*newdles=({struct _tuple19*_TmpE[2];({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f0=0,_Tmp10->f1=field_tag_exp;_Tmp10;});_TmpE[0]=_TmpF;}),({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f0=0,_Tmp10->f1=fieldexp;_Tmp10;});_TmpE[1]=_TmpF;});Cyc_List_list(_tag_fat(_TmpE,sizeof(struct _tuple19*),2));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,newdles,0U);
({void*_TmpE=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple19*_TmpF[1];({struct _tuple19*_Tmp10=Cyc_Toc_make_field(field->name,umem);_TmpF[0]=_Tmp10;});Cyc_List_list(_tag_fat(_TmpF,sizeof(struct _tuple19*),1));}));e->r=_TmpE;});}}}else{
# 2510
struct Cyc_List_List*newdles=0;
struct Cyc_List_List*sdfields=sd->impl->fields;
for(1;sdfields!=0;sdfields=sdfields->tl){
struct Cyc_List_List*fields2=fields;for(0;fields2!=0;fields2=fields2->tl){
if((*((struct _tuple36*)fields2->hd)).f0==(struct Cyc_Absyn_Aggrfield*)sdfields->hd){
struct _tuple36*_TmpB=(struct _tuple36*)fields2->hd;void*_TmpC;void*_TmpD;_TmpD=_TmpB->f0;_TmpC=_TmpB->f1;{struct Cyc_Absyn_Aggrfield*field=_TmpD;struct Cyc_Absyn_Exp*fieldexp=_TmpC;
void*fieldtyp=Cyc_Toc_typ_to_c(field->type);
void*fieldexp_typ=Cyc_Toc_typ_to_c(_check_null(fieldexp->topt));
Cyc_Toc_exp_to_c(nv,fieldexp);
# 2522
if(!Cyc_Unify_unify(fieldtyp,Cyc_Absyn_void_type)&& !
Cyc_Unify_unify(fieldtyp,fieldexp_typ)){
# 2525
if(!Cyc_Tcutil_is_arithmetic_type(fieldtyp)|| !
Cyc_Tcutil_is_arithmetic_type(fieldexp_typ))
fieldexp=({void*_TmpE=fieldtyp;Cyc_Toc_cast_it(_TmpE,Cyc_Absyn_copy_exp(fieldexp));});}
newdles=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpF=({struct _tuple19*_Tmp10=_cycalloc(sizeof(struct _tuple19));_Tmp10->f0=0,_Tmp10->f1=fieldexp;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=newdles;_TmpE;});
break;}}}}
# 2532
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,Cyc_List_imp_rev(newdles));e->r=_TmpB;});}}
# 2535
goto _LL7;}}}}case 30: _TmpA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Datatypedecl*tud=_Tmp7;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;
# 2539
struct Cyc_List_List*tqts=tuf->typs;
{struct Cyc_List_List*es2=es;for(0;es2!=0;(es2=es2->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)es2->hd;
void*cur_e_typ=_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple23*)_check_null(tqts)->hd)).f1);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ)&& !
Cyc_Toc_is_pointer_or_boxed_tvar(cur_e_typ))
({void*_TmpB=({void*_TmpC=field_typ;Cyc_Toc_cast_it(_TmpC,Cyc_Absyn_copy_exp(cur_e));})->r;cur_e->r=_TmpB;});}}{
# 2550
struct Cyc_Absyn_Exp*tag_exp=
tud->is_extensible?Cyc_Absyn_var_exp(tuf->name,0U): Cyc_Toc_datatype_tag(tud,tuf->name);
# 2553
if(!Cyc_Toc_is_toplevel(nv)){
# 2555
struct Cyc_List_List*dles=0;
{int i=1;for(0;es!=0;(es=es->tl,++ i)){
dles=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpC=({struct _fat_ptr*_TmpD=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_TmpD,(struct Cyc_Absyn_Exp*)es->hd);});_TmpB->hd=_TmpC;}),_TmpB->tl=dles;_TmpB;});}}{
# 2561
struct _tuple19*tag_dle=Cyc_Toc_make_field(Cyc_Toc_tag_sp,tag_exp);
({void*_TmpB=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpC->tag=28,({struct _tuple1*_TmpD=Cyc_Toc_collapse_qvars(tuf->name,tud->name);_TmpC->f1=_TmpD;}),_TmpC->f2=0,({
struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));_TmpE->hd=tag_dle,({struct Cyc_List_List*_TmpF=Cyc_List_imp_rev(dles);_TmpE->tl=_TmpF;});_TmpE;});_TmpC->f3=_TmpD;}),_TmpC->f4=0;_TmpC;});
# 2562
e->r=_TmpB;});}}else{
# 2567
struct Cyc_List_List*dles=0;
for(1;es!=0;es=es->tl){
dles=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpC=({struct _tuple19*_TmpD=_cycalloc(sizeof(struct _tuple19));_TmpD->f0=0,_TmpD->f1=(struct Cyc_Absyn_Exp*)es->hd;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=dles;_TmpB;});}
({void*_TmpB=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_TmpD=({struct _tuple19*_TmpE=_cycalloc(sizeof(struct _tuple19));_TmpE->f0=0,_TmpE->f1=tag_exp;_TmpE;});_TmpC->hd=_TmpD;}),({
struct Cyc_List_List*_TmpD=Cyc_List_imp_rev(dles);_TmpC->tl=_TmpD;});_TmpC;}));
# 2570
e->r=_TmpB;});}
# 2573
goto _LL7;}}case 31:
# 2575
 goto _LL51;case 32: _LL51:
 goto _LL7;case 33: _Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_TmpA=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp7=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result;_Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp1;struct Cyc_Absyn_Exp*rgnopt=_TmpA;struct Cyc_Absyn_Exp*aqopt=_Tmp7;void**topt=_Tmp6;struct Cyc_Absyn_Exp*num_elts=_Tmp4;int is_fat=_Tmp5;int inline_call=_Tmp2;
# 2579
void*t_c=Cyc_Toc_typ_to_c(*_check_null(topt));
Cyc_Toc_exp_to_c(nv,num_elts);{
struct _tuple1*x=0;
struct Cyc_Absyn_Exp*pexp;
struct Cyc_Absyn_Exp*num_elts2=num_elts;
if(is_fat){
x=Cyc_Toc_temp_var();
num_elts2=Cyc_Absyn_var_exp(x,0U);}
# 2588
if((int)mknd==1){
if(rgnopt==0 && aqopt==0 || Cyc_Flags_no_regions)
pexp=({void*_TmpB=*topt;struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_calloc_exp(_TmpB,_TmpC,num_elts2);});else{
# 2592
if(aqopt!=0)Cyc_Toc_exp_to_c(nv,aqopt);
if(rgnopt!=0){
Cyc_Toc_exp_to_c(nv,rgnopt);
pexp=({struct Cyc_Absyn_Exp*_TmpB=rgnopt;struct Cyc_Absyn_Exp*_TmpC=aqopt==0?Cyc_Toc_aliasable_qual_exp(): aqopt;struct Cyc_Absyn_Exp*_TmpD=
Cyc_Absyn_sizeoftype_exp(t_c,0U);
# 2595
Cyc_Toc_rcalloc_exp(_TmpB,_TmpC,_TmpD,num_elts2);});}else{
# 2599
pexp=({struct Cyc_Absyn_Exp*_TmpB=aqopt;struct Cyc_Absyn_Exp*_TmpC=Cyc_Absyn_sizeoftype_exp(t_c,0U);Cyc_Toc_aqcalloc_exp(_TmpB,_TmpC,num_elts2);});}}}else{
# 2603
if(rgnopt==0 && aqopt==0 || Cyc_Flags_no_regions)
pexp=Cyc_Toc_malloc_exp(*topt,num_elts2);else{
# 2606
if(aqopt!=0)Cyc_Toc_exp_to_c(nv,aqopt);
if(rgnopt!=0){
Cyc_Toc_exp_to_c(nv,rgnopt);
if((int)mknd==2)
pexp=Cyc_Toc_rvmalloc_exp(rgnopt,num_elts2);else{
# 2612
struct Cyc_Absyn_Exp*(*fn_e)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=inline_call?Cyc_Toc_rmalloc_inline_exp: Cyc_Toc_rmalloc_exp;
pexp=({struct Cyc_Absyn_Exp*(*_TmpB)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=fn_e;struct Cyc_Absyn_Exp*_TmpC=rgnopt;struct Cyc_Absyn_Exp*_TmpD=aqopt==0?Cyc_Toc_aliasable_qual_exp(): aqopt;_TmpB(_TmpC,_TmpD,num_elts2);});}}else{
# 2617
pexp=Cyc_Toc_aqmalloc_exp(aqopt,num_elts2);}}}
# 2620
if(is_fat){
struct Cyc_Absyn_Exp*elt_sz=(int)mknd==1?Cyc_Absyn_sizeoftype_exp(t_c,0U): Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rexp=({struct Cyc_Absyn_Exp*_TmpB[3];_TmpB[0]=pexp,_TmpB[1]=elt_sz,_TmpB[2]=num_elts2;Cyc_Toc_fncall_exp_dl(Cyc_Toc__tag_fat_e,_tag_fat(_TmpB,sizeof(struct Cyc_Absyn_Exp*),3));});
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=x;void*_TmpC=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_TmpD=num_elts;Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,
Cyc_Absyn_exp_stmt(rexp,0U),0U);});
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});}else{
# 2627
({void*_TmpB=({void*_TmpC=Cyc_Toc_typ_to_c(_check_null(e->topt));Cyc_Toc_cast_it_r(_TmpC,pexp);});e->r=_TmpB;});}
# 2630
({void*_TmpB=Cyc_Toc_void_star_type();e->topt=_TmpB;});
goto _LL7;}}case 34: _TmpA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 2640
void*e1_old_typ=_check_null(e1->topt);
void*e2_old_typ=_check_null(e2->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ)&& !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("Swap_e: is_pointer_or_boxed: not a pointer or boxed type",sizeof(char),57U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});{
# 2648
unsigned loc=e->loc;
struct _tuple1*v1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v1e=Cyc_Absyn_var_exp(v1,loc);v1e->topt=e1_old_typ;{
struct _tuple1*v2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*v2e=Cyc_Absyn_var_exp(v2,loc);v2e->topt=e2_old_typ;{
# 2654
struct Cyc_Absyn_Exp*s1e=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Tcutil_deep_copy_exp(1,e1);struct Cyc_Absyn_Exp*_TmpC=v2e;Cyc_Absyn_assign_exp(_TmpB,_TmpC,loc);});s1e->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_exp_stmt(s1e,loc);
struct Cyc_Absyn_Exp*s2e=({struct Cyc_Absyn_Exp*_TmpB=Cyc_Tcutil_deep_copy_exp(1,e2);struct Cyc_Absyn_Exp*_TmpC=v1e;Cyc_Absyn_assign_exp(_TmpB,_TmpC,loc);});s2e->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_exp_stmt(s2e,loc);
# 2659
struct Cyc_Absyn_Stmt*s=({struct _tuple1*_TmpB=v1;void*_TmpC=e1_old_typ;struct Cyc_Absyn_Exp*_TmpD=e1;struct Cyc_Absyn_Stmt*_TmpE=({
struct _tuple1*_TmpF=v2;void*_Tmp10=e2_old_typ;struct Cyc_Absyn_Exp*_Tmp11=e2;struct Cyc_Absyn_Stmt*_Tmp12=
Cyc_Absyn_seq_stmt(s1,s2,loc);
# 2660
Cyc_Absyn_declare_stmt(_TmpF,_Tmp10,_Tmp11,_Tmp12,loc);});
# 2659
Cyc_Absyn_declare_stmt(_TmpB,_TmpC,_TmpD,_TmpE,loc);});
# 2662
Cyc_Toc_stmt_to_c(nv,s);
({void*_TmpB=Cyc_Toc_stmt_exp_r(s);e->r=_TmpB;});
goto _LL7;}}}}}}case 37: _TmpA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp7;
# 2667
void*e1_typ=Cyc_Absyn_compress(_check_null(e1->topt));
Cyc_Toc_exp_to_c(nv,e1);
{void*_TmpB;if(*((int*)e1_typ)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e1_typ)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e1_typ)->f1)->f1.KnownAggr.tag==2){_TmpB=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)e1_typ)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_TmpB;
# 2671
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
struct Cyc_Absyn_Exp*e1_f=Cyc_Toc_member_exp(e1,f,0U);
struct Cyc_Absyn_Exp*e1_f_tag=Cyc_Toc_member_exp(e1_f,Cyc_Toc_tag_sp,0U);
({void*_TmpC=Cyc_Absyn_eq_exp(e1_f_tag,f_tag,0U)->r;e->r=_TmpC;});
goto _LLF9;}}else{goto _LLFC;}}else{goto _LLFC;}}else{_LLFC:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("non-aggregate type in tagcheck",sizeof(char),31U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LLF9:;}
# 2678
goto _LL7;}case 36: _TmpA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_TmpA;
Cyc_Toc_stmt_to_c(nv,s);goto _LL7;}case 35:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("UnresolvedMem",sizeof(char),14U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});case 24:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("compoundlit",sizeof(char),12U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});case 38:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("valueof(-)",sizeof(char),11U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});default: _TmpA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_List_List*o=_TmpA;struct Cyc_List_List*i=_Tmp7;
# 2684
Cyc_Toc_asm_iolist_types_toc(o);
Cyc_Toc_asm_iolist_types_toc(i);
goto _LL7;}}_LL7:;}
# 2688
if(!did_inserted_checks)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Toc did not examine an inserted check: ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp3;})(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}}struct _tuple37{int f0;struct _fat_ptr*f1;struct _fat_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 2717 "toc.cyc"
static struct _tuple37*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 2719
struct _tuple37*_Tmp0=_region_malloc(r,0U,sizeof(struct _tuple37));_Tmp0->f0=0,({struct _fat_ptr*_Tmp1=Cyc_Toc_fresh_label();_Tmp0->f1=_Tmp1;}),({struct _fat_ptr*_Tmp1=Cyc_Toc_fresh_label();_Tmp0->f2=_Tmp1;}),_Tmp0->f3=sc;return _Tmp0;}
# 2725
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(1;ps!=0;ps=ps->tl){
struct Cyc_Tcpat_PathNode*p=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 2731
if((int)(p->orig_pat.pattern.tag==1)){
void*t=_check_null(({union Cyc_Tcpat_PatOrWhere _Tmp0=p->orig_pat;if(_Tmp0.pattern.tag!=1)_throw_match();_Tmp0.pattern.val;})->topt);
void*t2=Cyc_Absyn_compress(Cyc_Toc_typ_to_c_array(t));
switch(*((int*)t2)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)){case 0:
# 2736
 goto _LL4;case 24: _LL4:
 goto _LL6;default: goto _LL7;}case 6: _LL6:
 goto _LL0;default: _LL7:
 v=({void*_Tmp0=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_Tmp0,v);});goto _LL0;}_LL0:;}{
# 2742
void*_Tmp0=p->access;void*_Tmp1;int _Tmp2;unsigned _Tmp3;switch(*((int*)_Tmp0)){case 0:
# 2746
 goto _LL9;case 1:
# 2751
 if(ps->tl!=0){
void*_Tmp4=((struct Cyc_Tcpat_PathNode*)ps->tl->hd)->access;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp4)==2){_Tmp7=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f1;_Tmp6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f2;_Tmp5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp4)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp7;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;unsigned i=_Tmp5;
# 2754
ps=ps->tl;
v=({void*_Tmp8=({void*_Tmp9=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_Tmp9,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp8,v);});
v=({struct Cyc_Absyn_Exp*_Tmp8=v;Cyc_Absyn_aggrarrow_exp(_Tmp8,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});
continue;}}else{
goto _LL12;}_LL12:;}
# 2760
v=Cyc_Absyn_deref_exp(v,0U);
goto _LL9;case 2: _Tmp3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0)->f3;{unsigned i=_Tmp3;
v=({struct Cyc_Absyn_Exp*_Tmp4=v;Cyc_Toc_member_exp(_Tmp4,Cyc_Absyn_fieldname((int)(i + 1U)),0U);});goto _LL9;}default: _Tmp2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0)->f3;{int tagged=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 2764
v=Cyc_Toc_member_exp(v,f,0U);
if(tagged)
v=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);
goto _LL9;}}_LL9:;}}
# 2770
return v;}
# 2775
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
unsigned _Tmp0;int _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
if(e==0)return v;else{return e;}}case 1:  {
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_signed_int_exp(0,0U),0U);}case 2:  {
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_neq_exp(_Tmp5,Cyc_Absyn_signed_int_exp(0,0U),0U);}case 3: _Tmp4=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)t)->f1;_Tmp3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)t)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp6->tag=31,_Tmp6->f1=ed,_Tmp6->f2=ef;_Tmp6;}),0U),0U);}case 4: _Tmp4=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)t)->f1;_Tmp3=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)t)->f2;{void*t=_Tmp4;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp6->tag=32,_Tmp6->f1=t,_Tmp6->f2=ef;_Tmp6;}),0U),0U);}case 5: _Tmp2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)t)->f1;_Tmp1=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)t)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp1;
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_float_exp(s,i,0U),0U);}case 6: _Tmp0=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)t)->f1;{unsigned i=_Tmp0;
struct Cyc_Absyn_Exp*_Tmp5=v;return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_signed_int_exp((int)i,0U),0U);}case 7: _Tmp1=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)t)->f1;_Tmp4=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)t)->f2;_Tmp3=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)t)->f3;{int i=_Tmp1;struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
# 2787
LOOP1: {
void*_Tmp5=v->r;void*_Tmp6;switch(*((int*)_Tmp5)){case 14: _Tmp6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp5)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;
v=e;goto LOOP1;}case 20: _Tmp6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
v=e;goto _LL15;}default:
 goto _LL15;}_LL15:;}
# 2797
v=({void*_Tmp5=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp5,v);});{
struct Cyc_Absyn_Exp*_Tmp5=Cyc_Absyn_deref_exp(v,0U);return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_signed_int_exp(i,0U),0U);}}case 8: _Tmp4=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)t)->f1;_Tmp1=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)t)->f2;{struct _fat_ptr*f=_Tmp4;int i=_Tmp1;
# 2800
struct Cyc_Absyn_Exp*_Tmp5=({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Toc_member_exp(v,f,0U);Cyc_Toc_member_exp(_Tmp6,Cyc_Toc_tag_sp,0U);});return Cyc_Absyn_eq_exp(_Tmp5,
Cyc_Absyn_signed_int_exp(i,0U),0U);}default: _Tmp4=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)t)->f1;_Tmp3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)t)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
# 2804
LOOP2: {
void*_Tmp5=v->r;void*_Tmp6;switch(*((int*)_Tmp5)){case 14: _Tmp6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp5)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;
v=e;goto LOOP2;}case 20: _Tmp6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
v=e;goto _LL1C;}default:
 goto _LL1C;}_LL1C:;}
# 2811
v=({void*_Tmp5=({void*_Tmp6=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(tuf->name,tud->name));Cyc_Absyn_cstar_type(_Tmp6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp5,v);});{
struct Cyc_Absyn_Exp*_Tmp5=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);return Cyc_Absyn_eq_exp(_Tmp5,Cyc_Absyn_var_exp(tuf->name,0U),0U);}}};}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _fat_ptr*f1;};
# 2823
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1};struct _tuple38{int f0;void*f1;};
# 2830
static struct _tuple38 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 2832
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(1;ss!=0;(ss=ss->tl,c=c + 1)){
struct _tuple34 _Tmp0=*((struct _tuple34*)ss->hd);void*_Tmp1;_Tmp1=_Tmp0.f0;{void*ptest=_Tmp1;
void*_Tmp2;switch(*((int*)ptest)){case 3:
# 2838
 goto _LL7;case 4: _LL7:
 goto _LL9;case 6: _LL9:
 continue;case 8: _Tmp2=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)ptest)->f1;{struct _fat_ptr*f=_Tmp2;
# 2842
if(k==(void*)& Cyc_Toc_OtherTest_val)
k=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct));_Tmp3->tag=3,_Tmp3->f1=f;_Tmp3;});
continue;}case 7:
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;continue;case 0: _Tmp2=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)ptest)->f1;if((struct Cyc_Absyn_Exp*)_Tmp2!=0){struct Cyc_Absyn_Exp*e=_Tmp2;
# 2847
k=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Toc_WhereTest_Toc_TestKind_struct));_Tmp3->tag=2,_Tmp3->f1=e;_Tmp3;});{struct _tuple38 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=k;return _Tmp3;}}else{
goto _LL13;}case 1: _LL13:
 goto _LL15;case 2: _LL15:
 goto _LL17;case 5: _LL17:
 goto _LL19;default: _LL19: {
struct _tuple38 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=k;return _Tmp3;}};}}{
# 2855
struct _tuple38 _Tmp0;_Tmp0.f0=c,_Tmp0.f1=k;return _Tmp0;}}
# 2860
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{int _Tmp0;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)p)){case 3: _Tmp3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)p)->f1;_Tmp2=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)p)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp3;struct Cyc_Absyn_Enumfield*ef=_Tmp2;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp4->tag=31,_Tmp4->f1=ed,_Tmp4->f2=ef;_Tmp4;}),0U);goto _LL0;}case 4: _Tmp3=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)p)->f1;_Tmp2=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)p)->f2;{void*t=_Tmp3;struct Cyc_Absyn_Enumfield*ef=_Tmp2;
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp4->tag=32,_Tmp4->f1=t,_Tmp4->f2=ef;_Tmp4;}),0U);goto _LL0;}case 6: _Tmp1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)p)->f1;{unsigned i=_Tmp1;
_Tmp0=(int)i;goto _LL8;}case 7: _Tmp0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)p)->f1;_LL8: {int i=_Tmp0;
_Tmp0=i;goto _LLA;}case 8: _Tmp0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)p)->f2;_LLA: {int i=_Tmp0;
e=Cyc_Absyn_signed_int_exp(i,0U);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("compile_pat_test_as_case!",sizeof(char),26U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL0:;}
# 2870
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct));_Tmp0->tag=16,_Tmp0->f1=e;_Tmp0;}),0U);}
# 2874
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1==0)return s2;
if(s2==0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple39{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Vardecl*f1;void*f2;int f3;};
# 2886
int Cyc_Toc_vd_in_use(struct Cyc_Absyn_Vardecl*other_vd,struct Cyc_List_List**newdecls){
{struct Cyc_List_List*ds=*newdecls;for(0;ds!=0;ds=ds->tl){
# 2890
if(other_vd==(*((struct _tuple39*)ds->hd)).f1)return 1;}}
return 0;}
# 2894
static int Cyc_Toc_is_usuable_type(void*ctype,void*other){
void*_Tmp0=Cyc_Absyn_compress(ctype);struct Cyc_Absyn_Tqual _Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp1;
# 2897
if(Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq)&& Cyc_Toc_is_void_star(other))
return 1;
goto _LL4;}}else{_LL4:
 return Cyc_Tcutil_typecmp(ctype,other)==0;};}
# 2903
static int Cyc_Toc_can_use_voidstar(void*ctype){
void*_Tmp0=Cyc_Absyn_compress(ctype);struct Cyc_Absyn_Tqual _Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp1;
return Cyc_Absyn_equal_tqual(tq,Cyc_Toc_mt_tq);}}else{
return 0;};}struct _tuple40{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Vardecl*f1;};struct _tuple41{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 2913
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**newdecls,struct Cyc_List_List**prevdecls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 2917
void*t=_check_null(p->topt);
void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 2: _Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 2920
struct Cyc_Absyn_Pat*p2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
p2->topt=p->topt;
_Tmp3=vd;_Tmp2=p2;goto _LL4;}case 1: _Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 2925
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 2927
{struct Cyc_List_List*pds=*prevdecls;for(0;pds!=0;pds=pds->tl){
struct _tuple40 _Tmp4=*((struct _tuple40*)pds->hd);void*_Tmp5;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp5;
# 2931
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp8=({struct _tuple39*_Tmp9=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp9->f0=vd,_Tmp9->f1=other_vd,_Tmp9->f2=ctype,_Tmp9->f3=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=*newdecls;_Tmp7;});*newdecls=_Tmp6;});{
# 2935
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp7->tag=4,_Tmp7->f1=other_vd;_Tmp7;}),0U);Cyc_Absyn_assign_stmt(_Tmp6,
Cyc_Absyn_copy_exp(path),0U);});
struct Cyc_Absyn_Stmt*_Tmp6=s;return Cyc_Toc_seq_stmt_opt(_Tmp6,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));}}}}}{
# 2943
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=0;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}else{
# 2949
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,ctype,0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=0;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}{
# 2953
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp5->tag=4,_Tmp5->f1=new_vd;_Tmp5;}),0U);Cyc_Absyn_assign_stmt(_Tmp4,
Cyc_Absyn_copy_exp(path),0U);});
struct Cyc_Absyn_Stmt*_Tmp4=s;return Cyc_Toc_seq_stmt_opt(_Tmp4,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));}}}case 4: _Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 2959
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 2961
{struct Cyc_List_List*pds=*prevdecls;for(0;pds!=0;pds=pds->tl){
struct _tuple40 _Tmp4=*((struct _tuple40*)pds->hd);void*_Tmp5;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp5;
# 2965
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp8=({struct _tuple39*_Tmp9=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp9->f0=vd,_Tmp9->f1=other_vd,_Tmp9->f2=ctype,_Tmp9->f3=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=*newdecls;_Tmp7;});*newdecls=_Tmp6;});{
# 2969
struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp7->tag=4,_Tmp7->f1=other_vd;_Tmp7;}),0U);return Cyc_Absyn_assign_stmt(_Tmp6,
Cyc_Absyn_copy_exp(path),0U);}}}}}{
# 2974
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}else{
# 2980
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,ctype,0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=0;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}{
# 2984
struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp5->tag=4,_Tmp5->f1=new_vd;_Tmp5;}),0U);return Cyc_Absyn_assign_stmt(_Tmp4,Cyc_Absyn_copy_exp(path),0U);}}}case 0:
 return 0;case 3: _Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 2988
({void*_Tmp4=Cyc_Absyn_cstar_type(t,Cyc_Toc_mt_tq);vd->type=_Tmp4;});{
# 2990
void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
# 2992
{struct Cyc_List_List*pds=*prevdecls;for(0;pds!=0;pds=pds->tl){
struct _tuple40 _Tmp4=*((struct _tuple40*)pds->hd);void*_Tmp5;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*other_vd=_Tmp5;
# 2996
if(Cyc_Toc_is_usuable_type(ctype,other_vd->type)&& !
Cyc_Toc_vd_in_use(other_vd,newdecls)){
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp8=({struct _tuple39*_Tmp9=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp9->f0=vd,_Tmp9->f1=other_vd,_Tmp9->f2=ctype,_Tmp9->f3=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=*newdecls;_Tmp7;});*newdecls=_Tmp6;});{
# 3001
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp7->tag=4,_Tmp7->f1=other_vd;_Tmp7;}),0U);Cyc_Absyn_assign_stmt(_Tmp6,({
void*_Tmp7=({void*_Tmp8=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_Tmp8,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp7,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
struct Cyc_Absyn_Stmt*_Tmp6=s;return Cyc_Toc_seq_stmt_opt(_Tmp6,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));}}}}}{
# 3009
struct Cyc_Absyn_Vardecl*new_vd;
if(Cyc_Toc_can_use_voidstar(ctype)){
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,Cyc_Toc_void_star_type(),0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}else{
# 3015
new_vd=({struct _tuple1*_Tmp4=Cyc_Toc_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp4,ctype,0);});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple39*_Tmp6=({struct _tuple39*_Tmp7=_region_malloc(rgn,0U,sizeof(struct _tuple39));_Tmp7->f0=vd,_Tmp7->f1=new_vd,_Tmp7->f2=ctype,_Tmp7->f3=0;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=*newdecls;_Tmp5;});*newdecls=_Tmp4;});}{
# 3021
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp5->tag=4,_Tmp5->f1=new_vd;_Tmp5;}),0U);Cyc_Absyn_assign_stmt(_Tmp4,({
void*_Tmp5=({void*_Tmp6=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_type(_Tmp6,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_Tmp5,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
struct Cyc_Absyn_Stmt*_Tmp4=s;return Cyc_Toc_seq_stmt_opt(_Tmp4,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,path,p2));}}}}case 8:
# 3026
 goto _LLE;case 9: _LLE:
 goto _LL10;case 10: _LL10:
 goto _LL12;case 11: _LL12:
 goto _LL14;case 12: _LL14:
 goto _LL16;case 13: _LL16:
 return 0;case 5: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->r)==7){_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->r)->f1;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->r)->f2;_Tmp1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->r)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;struct Cyc_List_List*ps=_Tmp1;
# 3035
if(ps==0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(tuf->name,tud->name);
void*field_ptr_typ=({void*_Tmp4=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_type(_Tmp4,Cyc_Toc_mt_tq);});
path=Cyc_Toc_cast_it(field_ptr_typ,path);{
int cnt=1;
struct Cyc_List_List*tuf_tqts=tuf->typs;
struct Cyc_Absyn_Stmt*s=0;
for(1;ps!=0;(ps=ps->tl,tuf_tqts=tuf_tqts->tl,++ cnt)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
if(p2->r==(void*)& Cyc_Absyn_Wild_p_val)continue;{
void*tuf_typ=(*((struct _tuple23*)_check_null(tuf_tqts)->hd)).f1;
void*t2=_check_null(p2->topt);
void*t2c=Cyc_Toc_typ_to_c_array(t2);
struct Cyc_Absyn_Exp*arrow_exp=({struct Cyc_Absyn_Exp*_Tmp4=path;Cyc_Absyn_aggrarrow_exp(_Tmp4,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(tuf_typ)))
arrow_exp=Cyc_Toc_cast_it(t2c,arrow_exp);
s=({struct Cyc_Absyn_Stmt*_Tmp4=s;Cyc_Toc_seq_stmt_opt(_Tmp4,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,arrow_exp,p2));});}}
# 3053
return s;}}}}else{_Tmp3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp3;
# 3110
struct _RegionHandle*_Tmp4=rgn;struct Cyc_Toc_Env**_Tmp5=nv;struct Cyc_List_List**_Tmp6=newdecls;struct Cyc_List_List**_Tmp7=prevdecls;struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_deref_exp(path,0U);return Cyc_Toc_extract_pattern_vars(_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,p2);}}case 7: _Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp3;
# 3056
struct Cyc_Absyn_Stmt*s=0;
{int cnt=1;for(0;ps!=0;(ps=ps->tl,++ cnt)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
if(p2->r==(void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*t2=_check_null(p2->topt);
struct _fat_ptr*f=Cyc_Absyn_fieldname(cnt);
s=({struct Cyc_Absyn_Stmt*_Tmp4=s;Cyc_Toc_seq_stmt_opt(_Tmp4,({struct _RegionHandle*_Tmp5=rgn;struct Cyc_Toc_Env**_Tmp6=nv;struct Cyc_List_List**_Tmp7=newdecls;struct Cyc_List_List**_Tmp8=prevdecls;struct Cyc_Absyn_Exp*_Tmp9=Cyc_Toc_member_exp(path,f,0U);Cyc_Toc_extract_pattern_vars(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,p2);}));});}}}
# 3065
return s;}case 6: _Tmp3=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{void*topt=_Tmp3;struct Cyc_List_List*dlps=_Tmp2;
# 3068
if(topt==0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unresolved aggregate pattern!",sizeof(char),30U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
void*atype=topt;
struct Cyc_List_List*fields;
int is_tagged=0;
{void*_Tmp4=Cyc_Absyn_compress(atype);void*_Tmp5;int _Tmp6;enum Cyc_Absyn_AggrKind _Tmp7;union Cyc_Absyn_AggrInfo _Tmp8;switch(*((int*)_Tmp4)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==24){_Tmp8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp8;
# 3074
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
fields=_check_null(ad->impl)->fields;
is_tagged=ad->impl->tagged;
goto _LL2E;}}else{goto _LL33;}case 6: _Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp4)->f1;_Tmp6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp4)->f2;_Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp4)->f3;{enum Cyc_Absyn_AggrKind ak=_Tmp7;int is_tuple=_Tmp6;struct Cyc_List_List*fs=_Tmp5;
# 3079
fields=fs;
goto _LL2E;}default: _LL33:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("bad type in aggr pattern",sizeof(char),25U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL2E:;}{
# 3083
struct Cyc_Absyn_Stmt*s=0;
for(1;dlps!=0;dlps=dlps->tl){
struct _tuple41*tup=(struct _tuple41*)dlps->hd;
struct Cyc_Absyn_Pat*p2=(*tup).f1;
if(p2->r==(void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname((*tup).f0);
void*t2=_check_null(p2->topt);
void*t2c=Cyc_Toc_typ_to_c_array(t2);
struct Cyc_Absyn_Exp*memexp=Cyc_Toc_member_exp(path,f,0U);
# 3094
if(is_tagged)memexp=Cyc_Toc_member_exp(memexp,Cyc_Toc_val_sp,0U);{
void*ftype=_check_null(Cyc_Absyn_lookup_field(fields,f))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(ftype))
memexp=Cyc_Toc_cast_it(t2c,memexp);else{
if(!Cyc_Tcutil_is_array_type(ftype)&&
 Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(ftype)))
# 3102
memexp=Cyc_Absyn_deref_exp(({void*_Tmp4=Cyc_Absyn_cstar_type(t2c,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp4,
Cyc_Absyn_address_exp(memexp,0U));}),0U);}
# 3105
s=({struct Cyc_Absyn_Stmt*_Tmp4=s;Cyc_Toc_seq_stmt_opt(_Tmp4,Cyc_Toc_extract_pattern_vars(rgn,nv,newdecls,prevdecls,memexp,p2));});}}}
# 3107
return s;}}}case 14:
# 3112
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unknownid pat",sizeof(char),14U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});case 15:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unknowncall pat",sizeof(char),16U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("exp pat",sizeof(char),8U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});};}
# 3121
static void Cyc_Toc_subst_pattern_vars(struct Cyc_List_List*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 5: _Tmp3=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
_Tmp3=vd;goto _LL4;}case 4: _Tmp3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 3127
{struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*ctype=Cyc_Toc_typ_to_c_array(vd->type);
struct Cyc_Absyn_Vardecl*tmpvd=({struct Cyc_Absyn_Vardecl*(*_Tmp6)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;_Tmp6;})(env,vd);
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp6->tag=1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,({struct Cyc_Absyn_Vardecl*_Tmp9=({struct Cyc_Absyn_Vardecl*(*_TmpA)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;_TmpA;})(env,vd);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;}),0U);
e1->topt=tmpvd->type;
({void*_Tmp6=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp7->tag=14,_Tmp7->f1=ctype,_Tmp7->f2=e1,_Tmp7->f3=0,_Tmp7->f4=1U;_Tmp7;});e->r=_Tmp6;});}
# 3128
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found)
# 3133
goto _LL41;else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LL41:;}}}
goto _LL0;}default: goto _LL3F;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 3136
Cyc_Toc_subst_pattern_vars(env,e1);_Tmp3=e2;_Tmp2=e3;goto _LL8;}case 26: _Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLA;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLC;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLE;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL10;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 3142
Cyc_Toc_subst_pattern_vars(env,e1);_Tmp3=e2;goto _LL12;}case 40: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL14;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL16;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL18;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL18: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1A;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1C;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1C: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL1E;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL20;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL22;}case 27: _Tmp3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL24;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Toc_subst_pattern_vars(env,e);goto _LL0;}case 4: _Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 3154
Cyc_Toc_subst_pattern_vars(env,e1);
Cyc_Toc_subst_pattern_vars(env,e2);
goto _LL0;}case 10: _Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp3;struct Cyc_List_List*es=_Tmp2;
Cyc_Toc_subst_pattern_vars(env,e);_Tmp3=es;goto _LL2A;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {struct Cyc_List_List*es=_Tmp3;
({void(*_Tmp4)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp4;})(Cyc_Toc_subst_pattern_vars,env,es);goto _LL0;}case 35: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL2E;}case 28: _Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL2E: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL30;}case 29: _Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL30: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL32;}case 25: _Tmp3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_List_List*dles=_Tmp3;
_Tmp3=dles;goto _LL34;}case 24: _Tmp3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL34: {struct Cyc_List_List*dles=_Tmp3;
# 3164
for(1;dles!=0;dles=dles->tl){
Cyc_Toc_subst_pattern_vars(env,(*((struct _tuple19*)dles->hd)).f1);}
goto _LL0;}case 37:
 goto _LL38;case 11: _LL38:
 goto _LL3A;case 33: _LL3A:
 goto _LL3C;case 16: _LL3C:
 goto _LL3E;case 30: _LL3E:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("subst_pattern_vars, unexpected exp",sizeof(char),35U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});default: _LL3F:
 goto _LL0;}_LL0:;}struct _tuple42{struct Cyc_Toc_Env*f0;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};
# 3181
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3189
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(dopt==0)
# 3191
return Cyc_Absyn_skip_stmt(0U);else{switch(*((int*)dopt)){case 0:
 return Cyc_Toc_throw_match_stmt();case 1: _Tmp2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)dopt)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp2;
# 3195
for(1;lscs!=0;lscs=lscs->tl){
struct _tuple37*_Tmp3=(struct _tuple37*)lscs->hd;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;_Tmp7=(int*)& _Tmp3->f0;_Tmp6=_Tmp3->f1;_Tmp5=_Tmp3->f2;_Tmp4=_Tmp3->f3;{int*already_emitted=(int*)_Tmp7;struct _fat_ptr*init_lab=_Tmp6;struct _fat_ptr*code_lab=_Tmp5;struct Cyc_Absyn_Switch_clause*sc=_Tmp4;
struct Cyc_Absyn_Stmt*body=sc->body;
if(body==rhs->rhs){
# 3200
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3207
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_Tmp8=rgn;struct Cyc_List_List**_Tmp9=decls;struct Cyc_Absyn_Exp*_TmpA=
Cyc_Absyn_var_exp(v,0U);
# 3207
Cyc_Toc_extract_pattern_vars(_Tmp8,& nv,& newdecls,_Tmp9,_TmpA,sc->pattern);});
# 3211
struct Cyc_Absyn_Stmt*res=sc->body;
{struct Cyc_List_List*ds=newdecls;for(0;ds!=0;ds=ds->tl){
struct _tuple39 _Tmp8=*((struct _tuple39*)ds->hd);int _Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;_TmpC=_Tmp8.f0;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_Absyn_Vardecl*oldv=_TmpC;struct Cyc_Absyn_Vardecl*newv=_TmpB;void*oldvt=_TmpA;int cast=_Tmp9;
({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple40*_TmpF=({struct _tuple40*_Tmp10=_region_malloc(rgn,0U,sizeof(struct _tuple40));_Tmp10->f0=oldv,_Tmp10->f1=newv;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=*decls;_TmpE;});*decls=_TmpD;});
if(cast)({struct Cyc_Absyn_Exp*_TmpD=({void*_TmpE=oldvt;Cyc_Toc_cast_it(_TmpE,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TmpF->tag=4,_TmpF->f1=newv;_TmpF;}),0U));});oldv->initializer=_TmpD;});else{
({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TmpE->tag=4,_TmpE->f1=newv;_TmpE;}),0U);oldv->initializer=_TmpD;});}
_check_null(oldv->initializer)->topt=newv->type;
# 3219
oldv->type=oldvt;
res=({struct Cyc_Absyn_Decl*_TmpD=({struct Cyc_Absyn_Decl*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpF=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp10->tag=0,_Tmp10->f1=oldv;_Tmp10;});_TmpE->r=_TmpF;}),_TmpE->loc=0U;_TmpE;});Cyc_Absyn_decl_stmt(_TmpD,res,0U);});}}}
# 3222
res=Cyc_Absyn_label_stmt(code_lab,res,0U);
if(init_opt!=0)
res=Cyc_Absyn_seq_stmt(init_opt,res,0U);
res=Cyc_Absyn_label_stmt(init_lab,res,0U);
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple42*_TmpA=({struct _tuple42*_TmpB=_region_malloc(rgn,0U,sizeof(struct _tuple42));_TmpB->f0=nv,_TmpB->f1=code_lab,_TmpB->f2=body;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=*bodies;_Tmp9;});*bodies=_Tmp8;});
return res;}}}}
# 3230
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("couldn't find rhs!",sizeof(char),19U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});}default: _Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)dopt)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)dopt)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)dopt)->f3;{struct Cyc_List_List*symbolic_path=_Tmp2;struct Cyc_List_List*switches=_Tmp1;void*other_decision=_Tmp0;
# 3233
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,other_decision,lscs,v);
# 3235
struct Cyc_Absyn_Exp*p=({struct Cyc_List_List*_Tmp3=Cyc_List_rev(symbolic_path);Cyc_Toc_compile_path(_Tmp3,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*ss=Cyc_List_rev(switches);
# 3238
struct _tuple38 _Tmp3=Cyc_Toc_admits_switch(ss);void*_Tmp4;int _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{int allows_switch=_Tmp5;void*test_kind=_Tmp4;
if(allows_switch > 1){
# 3242
struct Cyc_List_List*new_lscs;
new_lscs=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Switch_clause*_Tmp6=({struct Cyc_Absyn_Switch_clause*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_Tmp7->pattern=_Tmp8;}),_Tmp7->pat_vars=0,_Tmp7->where_clause=0,_Tmp7->body=res,_Tmp7->loc=0U;_Tmp7;});new_lscs->hd=_Tmp6;}),new_lscs->tl=0;
# 3245
for(1;ss!=0;ss=ss->tl){
struct _tuple34 _Tmp6=*((struct _tuple34*)ss->hd);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*pat_test=_Tmp8;void*dec_tree=_Tmp7;
# 3248
struct Cyc_Absyn_Pat*case_exp=Cyc_Toc_compile_pat_test_as_case(pat_test);
# 3250
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);
# 3252
new_lscs=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_TmpA=({struct Cyc_Absyn_Switch_clause*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TmpB->pattern=case_exp,_TmpB->pat_vars=0,_TmpB->where_clause=0,_TmpB->body=s,_TmpB->loc=0U;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=new_lscs;_Tmp9;});}}
# 3254
{void*_Tmp6;switch(*((int*)test_kind)){case 1:
# 3256
 LOOP1: {
void*_Tmp7=p->r;void*_Tmp8;switch(*((int*)_Tmp7)){case 14: _Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;
p=e;goto LOOP1;}case 20: _Tmp8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
p=e;goto _LL1E;}default:
 goto _LL1E;}_LL1E:;}
# 3262
p=Cyc_Absyn_deref_exp(({void*_Tmp7=Cyc_Absyn_cstar_type(Cyc_Absyn_sint_type,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_Tmp7,p);}),0U);goto _LL15;case 3: _Tmp6=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)test_kind)->f1;{struct _fat_ptr*f=_Tmp6;
# 3264
p=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Toc_member_exp(p,f,0U);Cyc_Toc_member_exp(_Tmp7,Cyc_Toc_tag_sp,0U);});goto _LL15;}case 2:
 goto _LL1D;default: _LL1D:
# 3271
 p=Cyc_Toc_cast_it(Cyc_Absyn_sint_type,p);
goto _LL15;}_LL15:;}
# 3274
res=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_Tmp6->tag=10,_Tmp6->f1=p,_Tmp6->f2=new_lscs,_Tmp6->f3=0;_Tmp6;}),0U);}else{
# 3278
void*_Tmp6;if(*((int*)test_kind)==2){_Tmp6=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)test_kind)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 3280
void*_Tmp7;void*_Tmp8;if(ss!=0){if(*((int*)((struct _tuple34*)((struct Cyc_List_List*)ss)->hd)->f1)==1){if(((struct Cyc_List_List*)ss)->tl==0){_Tmp8=((struct _tuple34*)ss->hd)->f0;_Tmp7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple34*)ss->hd)->f1)->f1;{void*pat_test=_Tmp8;struct Cyc_Tcpat_Rhs*rhs=_Tmp7;
# 3286
for(1;lscs!=0;lscs=lscs->tl){
struct _tuple37*_Tmp9=(struct _tuple37*)lscs->hd;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;_TmpD=(int*)& _Tmp9->f0;_TmpC=_Tmp9->f1;_TmpB=_Tmp9->f2;_TmpA=_Tmp9->f3;{int*already_emitted=(int*)_TmpD;struct _fat_ptr*init_lab=_TmpC;struct _fat_ptr*code_lab=_TmpB;struct Cyc_Absyn_Switch_clause*sc=_TmpA;
struct Cyc_Absyn_Stmt*body=sc->body;
if(body==rhs->rhs){
# 3291
if(*already_emitted)
return Cyc_Absyn_goto_stmt(init_lab,0U);
*already_emitted=1;{
struct Cyc_List_List*newdecls=0;
# 3296
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_TmpE=rgn;struct Cyc_List_List**_TmpF=decls;struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_TmpE,& nv,& newdecls,_TmpF,_Tmp10,sc->pattern);});
# 3300
struct Cyc_Absyn_Stmt*r=sc->body;
{struct Cyc_List_List*ds=newdecls;for(0;ds!=0;ds=ds->tl){
struct _tuple39 _TmpE=*((struct _tuple39*)ds->hd);int _TmpF;void*_Tmp10;void*_Tmp11;void*_Tmp12;_Tmp12=_TmpE.f0;_Tmp11=_TmpE.f1;_Tmp10=_TmpE.f2;_TmpF=_TmpE.f3;{struct Cyc_Absyn_Vardecl*oldv=_Tmp12;struct Cyc_Absyn_Vardecl*newv=_Tmp11;void*oldvt=_Tmp10;int cast=_TmpF;
({struct Cyc_List_List*_Tmp13=({struct Cyc_List_List*_Tmp14=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple40*_Tmp15=({struct _tuple40*_Tmp16=_region_malloc(rgn,0U,sizeof(struct _tuple40));_Tmp16->f0=oldv,_Tmp16->f1=newv;_Tmp16;});_Tmp14->hd=_Tmp15;}),_Tmp14->tl=*decls;_Tmp14;});*decls=_Tmp13;});
if(cast)({struct Cyc_Absyn_Exp*_Tmp13=({void*_Tmp14=oldvt;Cyc_Toc_cast_it(_Tmp14,Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp15->tag=4,_Tmp15->f1=newv;_Tmp15;}),0U));});oldv->initializer=_Tmp13;});else{
({struct Cyc_Absyn_Exp*_Tmp13=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp14=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp14->tag=4,_Tmp14->f1=newv;_Tmp14;}),0U);oldv->initializer=_Tmp13;});}
_check_null(oldv->initializer)->topt=newv->type;
# 3308
oldv->type=oldvt;
r=({struct Cyc_Absyn_Decl*_Tmp13=({struct Cyc_Absyn_Decl*_Tmp14=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp15=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp16=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp16->tag=0,_Tmp16->f1=oldv;_Tmp16;});_Tmp14->r=_Tmp15;}),_Tmp14->loc=0U;_Tmp14;});Cyc_Absyn_decl_stmt(_Tmp13,r,0U);});}}}
# 3311
r=Cyc_Absyn_label_stmt(code_lab,r,0U);
# 3315
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_subst_pattern_vars(*decls,e);
r=Cyc_Absyn_ifthenelse_stmt(e,r,res,0U);
if(init_opt!=0)
r=Cyc_Absyn_seq_stmt(init_opt,r,0U);
r=Cyc_Absyn_label_stmt(init_lab,r,0U);
({struct Cyc_List_List*_TmpE=({struct Cyc_List_List*_TmpF=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));({struct _tuple42*_Tmp10=({struct _tuple42*_Tmp11=_region_malloc(rgn,0U,sizeof(struct _tuple42));_Tmp11->f0=nv,_Tmp11->f1=code_lab,_Tmp11->f2=body;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=*bodies;_TmpF;});*bodies=_TmpE;});
return r;}}}}
# 3325
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("couldn't find rhs!",sizeof(char),19U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});}}else{goto _LL2D;}}else{goto _LL2D;}}else{_LL2D:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("bad where clause in match compiler",sizeof(char),35U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});};}}else{
# 3330
for(1;ss!=0;ss=ss->tl){
struct _tuple34 _Tmp7=*((struct _tuple34*)ss->hd);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*pat_test=_Tmp9;void*dec_tree=_Tmp8;
struct Cyc_Absyn_Exp*test_exp=Cyc_Toc_compile_pat_test(p,pat_test);
struct Cyc_Absyn_Stmt*s=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,dec_tree,lscs,v);
res=Cyc_Absyn_ifthenelse_stmt(test_exp,s,res,0U);}}};}
# 3338
return res;}}}};}
# 3348
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3350
for(1;bodies!=0;bodies=bodies->tl){
struct _tuple42*_Tmp0=(struct _tuple42*)bodies->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(struct Cyc_Toc_Env**)& _Tmp0->f0;_Tmp1=_Tmp0->f2;{struct Cyc_Toc_Env**nv=(struct Cyc_Toc_Env**)_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
if(s2==s)return nv;}}
# 3356
return 0;}
# 3360
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3363
void*t=_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3366
struct _tuple1*v;
int already_var;
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
already_var=1;v=Cyc_Absyn_binding2qvar(b);goto _LL0;}}else{
already_var=0;v=Cyc_Toc_temp_var();goto _LL0;}_LL0:;}{
# 3372
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _Tmp0=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_Tmp1;
# 3377
struct Cyc_List_List*lscs=
({struct Cyc_List_List*(*_Tmp2)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp2;})(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3382
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 3390
{struct Cyc_List_List*lscs2=lscs;for(0;lscs2!=0;lscs2=lscs2->tl){
struct _tuple37*_Tmp2=(struct _tuple37*)lscs2->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f2;_Tmp3=_Tmp2->f3;{struct _fat_ptr*body_lab=_Tmp4;struct Cyc_Absyn_Switch_clause*body_sc=_Tmp3;
struct Cyc_Absyn_Stmt*s=body_sc->body;
# 3394
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3397
if(envp==0)continue;{
struct Cyc_Toc_Env*env=*envp;
# 3400
if(lscs2->tl!=0){
struct _tuple37*_Tmp5=(struct _tuple37*)lscs2->tl->hd;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f2;_Tmp6=_Tmp5->f3;{struct _fat_ptr*fallthru_lab=_Tmp7;struct Cyc_Absyn_Switch_clause*next_sc=_Tmp6;
# 3404
struct Cyc_Toc_Env**next_case_env=Cyc_Toc_find_case_env(mybodies,next_sc->body);
# 3409
if(next_case_env==0)
({struct Cyc_Toc_Env*_Tmp8=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_Tmp8,s);});else{
# 3413
struct Cyc_List_List*vs=0;
if(next_sc->pat_vars!=0){
vs=Cyc_Tcutil_filter_nulls(Cyc_List_split((struct Cyc_List_List*)next_sc->pat_vars->v).f0);
vs=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp9)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp9;});struct Cyc_Absyn_Vardecl*(*_Tmp9)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=({struct Cyc_Absyn_Vardecl*(*_TmpA)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*))Cyc_List_assoc;_TmpA;});struct Cyc_List_List*_TmpA=mydecls;_Tmp8(_Tmp9,_TmpA,Cyc_List_imp_rev(vs));});}
# 3418
({struct Cyc_Toc_Env*_Tmp8=Cyc_Toc_non_last_switchclause_env(rgn,env,end_l,fallthru_lab,vs);Cyc_Toc_stmt_to_c(_Tmp8,s);});}}}else{
# 3422
({struct Cyc_Toc_Env*_Tmp5=Cyc_Toc_last_switchclause_env(rgn,env,end_l);Cyc_Toc_stmt_to_c(_Tmp5,s);});}}}}}{
# 3426
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_Tmp2=test_tree;Cyc_Absyn_seq_stmt(_Tmp2,({struct _fat_ptr*_Tmp3=end_l;Cyc_Absyn_label_stmt(_Tmp3,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3428
struct Cyc_List_List*added=0;
for(1;mydecls!=0;mydecls=mydecls->tl){
struct _tuple40 _Tmp2=*((struct _tuple40*)mydecls->hd);void*_Tmp3;_Tmp3=_Tmp2.f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
{struct Cyc_List_List*itr=added;for(0;itr!=0;itr=itr->tl){
if((struct Cyc_Absyn_Vardecl*)itr->hd==vd)goto bottom;}}
added=({struct Cyc_List_List*_Tmp4=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp4->hd=vd,_Tmp4->tl=added;_Tmp4;});
res=({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=vd;_Tmp5;}),0U);Cyc_Absyn_decl_stmt(_Tmp4,res,0U);});
bottom:
 continue;}}
# 3439
if(!already_var)
res=({struct _tuple1*_Tmp2=v;void*_Tmp3=Cyc_Toc_typ_to_c(_check_null(e->topt));struct Cyc_Absyn_Exp*_Tmp4=e;Cyc_Absyn_declare_stmt(_Tmp2,_Tmp3,_Tmp4,res,0U);});
whole_s->r=res->r;
_npop_handler(0);return;}}
# 3374
;_pop_region();}}}
# 3449
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_Tmp0[1];({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_signed_int_exp(n - 1,0U);_Tmp0[0]=_Tmp1;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__npop_handler_e,_tag_fat(_Tmp0,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);}
# 3452
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_Tmp0=({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_Tmp1,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_Tmp0;});}
# 3457
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3459
while(1){
{void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Toc_exp_to_c(nv,e);return;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){
({int _Tmp5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp5,s);});return;}else{_Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 3466
void*t=Cyc_Toc_typ_to_c(_check_null(e->topt));
Cyc_Toc_exp_to_c(nv,e);{
int npop=Cyc_Toc_get_npop(s);
if(npop > 0){
struct _tuple1*x=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*retn_stmt=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(x,0U),0U);
Cyc_Toc_do_npop_before(npop,retn_stmt);
({void*_Tmp5=Cyc_Absyn_declare_stmt(x,t,e,retn_stmt,0U)->r;s->r=_Tmp5;});}
# 3475
return;}}}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 3477
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 3481
Cyc_Toc_exp_to_c(nv,e);
Cyc_Toc_stmt_to_c(nv,s1);
s=s2;
continue;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 3486
Cyc_Toc_exp_to_c(nv,e1);
Cyc_Toc_exp_to_c(nv,e2);
_Tmp4=s2;_Tmp3=e3;goto _LL10;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_LL10: {struct Cyc_Absyn_Stmt*s2=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp4=e;_Tmp3=s2;goto _LL12;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 3491
Cyc_Toc_exp_to_c(nv,e);{
struct _RegionHandle _Tmp5=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp5;_push_region(temp);({struct Cyc_Toc_Env*_Tmp6=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_Tmp6,s2);});
_npop_handler(0);return;
# 3492
;_pop_region();}}case 6:  {
# 3495
void*_Tmp5;_Tmp5=nv->break_lab;{struct _fat_ptr**b=_Tmp5;
if(b!=0)
({void*_Tmp6=Cyc_Toc_goto_stmt_r(*b);s->r=_Tmp6;});
# 3499
({int _Tmp6=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp6,s);});
return;}}case 7:  {
# 3502
void*_Tmp5;_Tmp5=nv->continue_lab;{struct _fat_ptr**c=_Tmp5;
if(c!=0)
({void*_Tmp6=Cyc_Toc_goto_stmt_r(*c);s->r=_Tmp6;});
goto _LL18;}}case 8: _LL18:
# 3507
({int _Tmp5=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp5,s);});
return;case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*lab=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
s=s1;continue;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;void*dec_tree_opt=_Tmp2;
# 3511
Cyc_Toc_xlate_switch(nv,s,e,scs,dec_tree_opt);
return;}case 11: _Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp4;struct Cyc_Absyn_Switch_clause**dest_clause=_Tmp3;
# 3514
void*_Tmp5;_Tmp5=nv->fallthru_info;{struct Cyc_Toc_FallthruInfo*fi=_Tmp5;
if(fi==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("fallthru in unexpected place",sizeof(char),29U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});{
struct Cyc_Toc_FallthruInfo _Tmp6=*fi;void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.label;_Tmp7=_Tmp6.binders;{struct _fat_ptr*l=_Tmp8;struct Cyc_List_List*vs=_Tmp7;
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(l,0U);
# 3520
({int _Tmp9=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_Tmp9,s2);});{
struct Cyc_List_List*vs2=Cyc_List_rev(vs);
struct Cyc_List_List*es2=Cyc_List_rev(es);
for(1;vs2!=0;(vs2=vs2->tl,es2=es2->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(es2)->hd);
s2=({struct Cyc_Absyn_Stmt*_Tmp9=({struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct));_TmpB->tag=5,_TmpB->f1=(struct Cyc_Absyn_Vardecl*)vs2->hd;_TmpB;}),0U);Cyc_Absyn_assign_stmt(_TmpA,(struct Cyc_Absyn_Exp*)es2->hd,0U);});Cyc_Absyn_seq_stmt(_Tmp9,s2,0U);});}
# 3528
s->r=s2->r;
return;}}}}}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*body=_Tmp3;
# 3534
{void*_Tmp5=d->r;void*_Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp5)){case 0: _Tmp8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;
Cyc_Toc_local_decl_to_c(nv,vd,body);goto _LL2F;}case 2: _Tmp8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;_Tmp7=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;_Tmp6=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f4;{struct Cyc_Absyn_Pat*p=_Tmp8;struct Cyc_Absyn_Exp*e=_Tmp7;void*dec_tree=_Tmp6;
# 3541
{void*_Tmp9=p->r;void*_TmpA;if(*((int*)_Tmp9)==1){if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp9)->f2)->r)==0){_TmpA=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp9)->f1;{struct Cyc_Absyn_Vardecl*vd=_TmpA;
# 3543
if(Cyc_Absyn_var_may_appear_exp(vd->name,e)){
struct Cyc_Absyn_Vardecl*new_vd=({struct _tuple1*_TmpB=Cyc_Toc_temp_var();void*_TmpC=vd->type;Cyc_Absyn_new_vardecl(0U,_TmpB,_TmpC,e);});
({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_TmpC->tag=4,_TmpC->f1=new_vd;_TmpC;}),0U);vd->initializer=_TmpB;});
vd->initializer->topt=new_vd->type;
({void*_TmpB=({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpE->tag=0,_TmpE->f1=new_vd;_TmpE;});Cyc_Absyn_new_decl(_TmpD,s->loc);});struct Cyc_Absyn_Stmt*_TmpD=({
struct Cyc_Absyn_Decl*_TmpE=({void*_TmpF=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp10->tag=0,_Tmp10->f1=vd;_Tmp10;});Cyc_Absyn_new_decl(_TmpF,s->loc);});struct Cyc_Absyn_Stmt*_TmpF=body;Cyc_Absyn_decl_stmt(_TmpE,_TmpF,s->loc);});
# 3547
Cyc_Absyn_decl_stmt(_TmpC,_TmpD,s->loc);})->r;s->r=_TmpB;});}else{
# 3551
vd->initializer=e;
({void*_TmpB=({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpE->tag=0,_TmpE->f1=vd;_TmpE;});Cyc_Absyn_new_decl(_TmpD,s->loc);});struct Cyc_Absyn_Stmt*_TmpD=body;Cyc_Absyn_decl_stmt(_TmpC,_TmpD,s->loc);})->r;s->r=_TmpB;});}
# 3554
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3C;}}else{goto _LL3F;}}else{_LL3F:
# 3560
({void*_TmpB=Cyc_Toc_letdecl_to_c(nv,p,dec_tree,_check_null(e->topt),e,body)->r;s->r=_TmpB;});
goto _LL3C;}_LL3C:;}
# 3563
goto _LL2F;}case 3: _Tmp8=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_List_List*vds=_Tmp8;
# 3567
struct Cyc_List_List*rvds=Cyc_List_rev(vds);
if(rvds==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("empty Letv_d",sizeof(char),13U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});
({void*_Tmp9=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpA->tag=0,_TmpA->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_TmpA;});d->r=_Tmp9;});
rvds=rvds->tl;
for(1;rvds!=0;rvds=rvds->tl){
struct Cyc_Absyn_Decl*d2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp9->tag=0,_Tmp9->f1=(struct Cyc_Absyn_Vardecl*)rvds->hd;_Tmp9;}),0U);
({void*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=d2;Cyc_Absyn_decl_stmt(_TmpA,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_Tmp9;});}
# 3576
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2F;}case 1: _Tmp8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp8;
# 3579
Cyc_Toc_fndecl_to_c(nv,fd,0);
Cyc_Toc_stmt_to_c(nv,body);
goto _LL2F;}case 4: _Tmp8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;_Tmp7=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f2;_Tmp6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp8;struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp6;
# 3584
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_type(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple30 _Tmp9=Cyc_Toc_temp_var_and_exp();void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{struct _tuple1*rh_var=_TmpB;struct Cyc_Absyn_Exp*rh_exp=_TmpA;
struct _tuple1*x_var=vd->name;
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3590
Cyc_Toc_stmt_to_c(nv,body);
if(Cyc_Flags_no_regions)
({void*_TmpC=({struct _tuple1*_TmpD=x_var;void*_TmpE=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_TmpF=
Cyc_Absyn_signed_int_exp(0,0U);
# 3592
Cyc_Absyn_declare_stmt(_TmpD,_TmpE,_TmpF,body,0U);})->r;s->r=_TmpC;});else{
# 3594
if((unsigned)open_exp_opt){
# 3596
Cyc_Toc_exp_to_c(nv,open_exp_opt);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_TmpC=open_exp_opt;Cyc_Absyn_aggrarrow_exp(_TmpC,({struct _fat_ptr*_TmpD=_cycalloc(sizeof(struct _fat_ptr));*_TmpD=_tag_fat("h",sizeof(char),2U);_TmpD;}),0U);}),0U);
({void*_TmpC=Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,body,0U)->r;s->r=_TmpC;});}}else{
# 3605
struct Cyc_Absyn_Exp*_zero_exp=Cyc_Absyn_uint_exp(0U,0U);
struct Cyc_Absyn_Exp*_one_exp=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*rgn_flag_exp=Cyc_Flags_no_reaps?_one_exp: _zero_exp;
({void*_TmpC=({struct _tuple1*_TmpD=rh_var;void*_TmpE=rh_struct_typ;struct Cyc_Absyn_Exp*_TmpF=({struct Cyc_Absyn_Exp*_Tmp10[2];_Tmp10[0]=rgn_flag_exp,({
struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_string_exp(*(*x_var).f1,0U);_Tmp10[1]=_Tmp11;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__new_region_e,_tag_fat(_Tmp10,sizeof(struct Cyc_Absyn_Exp*),2));});
# 3608
Cyc_Absyn_declare_stmt(_TmpD,_TmpE,_TmpF,({
# 3610
struct _tuple1*_Tmp10=x_var;void*_Tmp11=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_Tmp12=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_Tmp10,_Tmp11,_Tmp12,({
struct Cyc_Absyn_Stmt*_Tmp13=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_Tmp14[1];_Tmp14[0]=x_exp;Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_region_e,_tag_fat(_Tmp14,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);Cyc_Absyn_seq_stmt(_Tmp13,({
struct Cyc_Absyn_Stmt*_Tmp14=body;Cyc_Absyn_seq_stmt(_Tmp14,
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_region_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3608
s->r=_TmpC;});}}
# 3616
return;}}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("bad nested declaration within function",sizeof(char),39U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL2F:;}
# 3619
return;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*body=_Tmp4;struct Cyc_List_List*scs=_Tmp3;void*dec_tree=_Tmp2;
# 3633 "toc.cyc"
struct _tuple30 _Tmp5=Cyc_Toc_temp_var_and_exp();void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct _tuple1*h_var=_Tmp7;struct Cyc_Absyn_Exp*h_exp=_Tmp6;
struct _tuple30 _Tmp8=Cyc_Toc_temp_var_and_exp();void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct _tuple1*e_var=_TmpA;struct Cyc_Absyn_Exp*e_exp=_Tmp9;
struct _tuple30 _TmpB=Cyc_Toc_temp_var_and_exp();void*_TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct _tuple1*was_thrown_var=_TmpD;struct Cyc_Absyn_Exp*was_thrown_exp=_TmpC;
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_type());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_type);
# 3640
e_exp->topt=e_typ;{
struct _RegionHandle _TmpE=_new_region(0U,"temp");struct _RegionHandle*temp=& _TmpE;_push_region(temp);
# 3643
Cyc_Toc_stmt_to_c(nv,body);{
struct Cyc_Absyn_Stmt*tryandpop_stmt=({struct Cyc_Absyn_Stmt*_TmpF=body;Cyc_Absyn_seq_stmt(_TmpF,
Cyc_Absyn_exp_stmt(Cyc_Toc_fncall_exp_dl(Cyc_Toc__pop_handler_e,_tag_fat(0U,sizeof(struct Cyc_Absyn_Exp*),0)),0U),0U);});
# 3647
struct Cyc_Absyn_Stmt*handler_stmt=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_TmpF->tag=10,_TmpF->f1=e_exp,_TmpF->f2=scs,_TmpF->f3=dec_tree;_TmpF;}),0U);
# 3649
Cyc_Toc_stmt_to_c(nv,handler_stmt);{
# 3652
struct Cyc_Absyn_Exp*setjmp_call=({struct Cyc_Absyn_Exp*_TmpF[1];({struct Cyc_Absyn_Exp*_Tmp10=
Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_TmpF[0]=_Tmp10;});Cyc_Toc_fncall_exp_dl(Cyc_Toc_setjmp_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),1));});
struct Cyc_Absyn_Stmt*pushhandler_call=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_TmpF[1];({struct Cyc_Absyn_Exp*_Tmp10=Cyc_Absyn_address_exp(h_exp,0U);_TmpF[0]=_Tmp10;});Cyc_Toc_fncall_exp_dl(Cyc_Toc__push_handler_e,_tag_fat(_TmpF,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*zero_exp=Cyc_Absyn_int_exp(0U,0,0U);
struct Cyc_Absyn_Exp*one_exp=Cyc_Absyn_int_exp(0U,1,0U);
({void*_TmpF=({
struct _tuple1*_Tmp10=h_var;void*_Tmp11=h_typ;Cyc_Absyn_declare_stmt(_Tmp10,_Tmp11,0,({
struct Cyc_Absyn_Stmt*_Tmp12=pushhandler_call;Cyc_Absyn_seq_stmt(_Tmp12,({
struct _tuple1*_Tmp13=was_thrown_var;void*_Tmp14=was_thrown_typ;struct Cyc_Absyn_Exp*_Tmp15=zero_exp;Cyc_Absyn_declare_stmt(_Tmp13,_Tmp14,_Tmp15,({
struct Cyc_Absyn_Stmt*_Tmp16=({struct Cyc_Absyn_Exp*_Tmp17=setjmp_call;struct Cyc_Absyn_Stmt*_Tmp18=
Cyc_Absyn_assign_stmt(was_thrown_exp,one_exp,0U);
# 3662
Cyc_Absyn_ifthenelse_stmt(_Tmp17,_Tmp18,
# 3664
Cyc_Toc_skip_stmt_dl(),0U);});
# 3662
Cyc_Absyn_seq_stmt(_Tmp16,({
# 3665
struct Cyc_Absyn_Exp*_Tmp17=Cyc_Absyn_prim1_exp(11U,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_Tmp18=tryandpop_stmt;Cyc_Absyn_ifthenelse_stmt(_Tmp17,_Tmp18,({
# 3667
struct _tuple1*_Tmp19=e_var;void*_Tmp1A=e_typ;struct Cyc_Absyn_Exp*_Tmp1B=({
void*_Tmp1C=e_typ;Cyc_Toc_cast_it(_Tmp1C,Cyc_Toc_get_exn_thrown_expression());});
# 3667
Cyc_Absyn_declare_stmt(_Tmp19,_Tmp1A,_Tmp1B,handler_stmt,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 3658
s->r=_TmpF;});}}
# 3671
_npop_handler(0);return;
# 3641
;_pop_region();}}}}}};}
# 3460 "toc.cyc"
1U;}}
# 3678 "toc.cyc"
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->i.tvars=0;
f->i.effect=0;
# 3682
f->i.qual_bnd=0;
f->i.requires_clause=0;
f->i.ensures_clause=0;
f->i.throws_clause=0;
f->i.effconstr=0;
({void*_Tmp0=Cyc_Toc_typ_to_c(f->i.ret_type);f->i.ret_type=_Tmp0;});
({void*_Tmp0=Cyc_Toc_typ_to_c(_check_null(f->cached_type));f->cached_type=_Tmp0;});{
struct _RegionHandle _Tmp0=_new_region(0U,"frgn");struct _RegionHandle*frgn=& _Tmp0;_push_region(frgn);
{struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_share_env(frgn,nv);struct Cyc_Toc_Env*nv=_Tmp1;
{struct Cyc_List_List*args=f->i.args;for(0;args!=0;args=args->tl){
({void*_Tmp2=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f2);(*((struct _tuple9*)args->hd)).f2=_Tmp2;});}}
# 3695
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 3699
Cyc_Toc_fn_pop_table=({struct Cyc_Hashtable_Table**_Tmp2=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));({struct Cyc_Hashtable_Table*_Tmp3=({struct Cyc_Hashtable_Table*(*_Tmp4)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;_Tmp4;})(*_check_null(Cyc_Toc_gpop_tables),f);*_Tmp2=_Tmp3;});_Tmp2;});
if((unsigned)f->i.cyc_varargs && f->i.cyc_varargs->name!=0){
struct Cyc_Absyn_VarargInfo _Tmp2=*f->i.cyc_varargs;int _Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;_Tmp6=_Tmp2.name;_Tmp5=_Tmp2.tq;_Tmp4=_Tmp2.type;_Tmp3=_Tmp2.inject;{struct _fat_ptr*n=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;int i=_Tmp3;
void*t2=Cyc_Toc_typ_to_c(Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type));
struct _tuple1*x2;x2=_cycalloc(sizeof(struct _tuple1)),x2->f0=Cyc_Absyn_Loc_n,x2->f1=n;
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=f->i.args;Cyc_List_append(_Tmp8,({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple9*_TmpA=({struct _tuple9*_TmpB=_cycalloc(sizeof(struct _tuple9));_TmpB->f0=n,_TmpB->f1=tq,_TmpB->f2=t2;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=0;_Tmp9;}));});f->i.args=_Tmp7;});
f->i.cyc_varargs=0;}}
# 3708
{struct Cyc_List_List*arg_vds=(struct Cyc_List_List*)_check_null(f->param_vardecls)->v;for(0;arg_vds!=0;arg_vds=arg_vds->tl){
({void*_Tmp2=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type);((struct Cyc_Absyn_Vardecl*)arg_vds->hd)->type=_Tmp2;});}}
({struct Cyc_Toc_Env*_Tmp2=Cyc_Toc_clear_toplevel(frgn,nv);Cyc_Toc_stmt_to_c(_Tmp2,f->body);});}
# 3690
;_pop_region();}}
# 3713
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch((int)s){case Cyc_Absyn_Abstract:
 return 2U;case Cyc_Absyn_ExternC:
 return 3U;default:
 return s;};}struct _tuple43{struct Cyc_Toc_TocState*f0;struct Cyc_Absyn_Aggrdecl**f1;};
# 3729 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple43*env){
# 3731
struct _tuple43 _Tmp0=*env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=*_Tmp0.f1;{struct Cyc_Toc_TocState*s=_Tmp2;struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
struct _tuple1*n=ad->name;
struct Cyc_Toc_TocState _Tmp3=*s;void*_Tmp4;_Tmp4=_Tmp3.aggrs_so_far;{struct Cyc_Dict_Dict*aggrs_so_far=_Tmp4;
int seen_defn_before;
struct _tuple18**dopt=({struct _tuple18**(*_Tmp5)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple18**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp5;})(*aggrs_so_far,n);
if(dopt==0){
seen_defn_before=0;{
struct _tuple18*v;
v=_region_malloc(d,0U,sizeof(struct _tuple18)),v->f0=ad,(int)ad->kind==0?({void*_Tmp5=Cyc_Absyn_strctq(n);v->f1=_Tmp5;}):({void*_Tmp5=Cyc_Absyn_unionq_type(n);v->f1=_Tmp5;});
({struct Cyc_Dict_Dict _Tmp5=({struct Cyc_Dict_Dict(*_Tmp6)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;_Tmp6;})(*aggrs_so_far,n,v);*aggrs_so_far=_Tmp5;});}}else{
# 3742
struct _tuple18*_Tmp5=*dopt;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f0;_Tmp6=_Tmp5->f1;{struct Cyc_Absyn_Aggrdecl*ad2=_Tmp7;void*t=_Tmp6;
if(ad2->impl==0){
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=({struct Cyc_Dict_Dict(*_TmpA)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple18*))Cyc_Dict_insert;_TmpA;});struct Cyc_Dict_Dict _TmpA=*aggrs_so_far;struct _tuple1*_TmpB=n;_Tmp9(_TmpA,_TmpB,({struct _tuple18*_TmpC=_region_malloc(d,0U,sizeof(struct _tuple18));_TmpC->f0=ad,_TmpC->f1=t;_TmpC;}));});*aggrs_so_far=_Tmp8;});
seen_defn_before=0;}else{
# 3747
seen_defn_before=1;}}}{
# 3749
struct Cyc_Absyn_Aggrdecl*new_ad;new_ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),new_ad->kind=ad->kind,new_ad->sc=2U,new_ad->name=ad->name,new_ad->tvs=0,new_ad->impl=0,new_ad->expected_mem_kind=0,new_ad->attributes=ad->attributes;
# 3756
if(ad->impl!=0 && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_Tmp5=({struct Cyc_Absyn_AggrdeclImpl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_Tmp6->exist_vars=0,_Tmp6->qual_bnd=0,_Tmp6->fields=0,_Tmp6->tagged=0,_Tmp6->effconstr=0;_Tmp6;});new_ad->impl=_Tmp5;});{
# 3763
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*fields=_check_null(ad->impl)->fields;for(0;fields!=0;fields=fields->tl){
# 3767
struct Cyc_Absyn_Aggrfield*old_field=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*old_type=old_field->type;
struct Cyc_List_List*old_atts=old_field->attributes;
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(old_type))&&(
(int)ad->kind==0 && fields->tl==0 ||(int)ad->kind==1)){
# 3781 "toc.cyc"
void*_Tmp5=Cyc_Absyn_compress(old_type);unsigned _Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;if(*((int*)_Tmp5)==4){_Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1.tq;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1.zero_term;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp5)->f1.zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*zt=_Tmp7;unsigned ztl=_Tmp6;
# 3784
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_TmpA->tag=4,_TmpA->f1.elt_type=et,_TmpA->f1.tq=tq,({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_uint_exp(0U,0U);_TmpA->f1.num_elts=_TmpB;}),_TmpA->f1.zero_term=zt,_TmpA->f1.zt_loc=ztl;_TmpA;});
goto _LL9;}}else{
# 3787
old_atts=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_TmpC->tag=6,_TmpC->f1=0;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=old_atts;_TmpA;});
old_type=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_TmpA->tag=4,({void*_TmpB=Cyc_Toc_void_star_type();_TmpA->f1.elt_type=_TmpB;}),_TmpA->f1.tq=Cyc_Toc_mt_tq,({
# 3790
struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_uint_exp(0U,0U);_TmpA->f1.num_elts=_TmpB;}),_TmpA->f1.zero_term=Cyc_Absyn_false_type,_TmpA->f1.zt_loc=0U;_TmpA;});}_LL9:;}{
# 3794
struct Cyc_Absyn_Aggrfield*new_field;new_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),new_field->name=old_field->name,new_field->tq=Cyc_Toc_mt_tq,({
# 3796
void*_Tmp5=Cyc_Toc_typ_to_c(old_type);new_field->type=_Tmp5;}),new_field->width=old_field->width,({
# 3798
struct Cyc_List_List*_Tmp5=Cyc_Atts_atts2c(old_atts);new_field->attributes=_Tmp5;}),new_field->requires_clause=0;
# 3804
if(_check_null(ad->impl)->tagged){
void*T=new_field->type;
struct _fat_ptr*f=new_field->name;
struct _fat_ptr s=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)*(*ad->name).f1;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)*f;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("_union_%s_%s",sizeof(char),13U),_tag_fat(_Tmp7,sizeof(void*),2));});
struct _fat_ptr*str;str=_cycalloc(sizeof(struct _fat_ptr)),*str=s;{
struct Cyc_Absyn_Aggrfield*value_field;value_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),value_field->name=Cyc_Toc_val_sp,value_field->tq=Cyc_Toc_mt_tq,value_field->type=T,value_field->width=0,value_field->attributes=0,value_field->requires_clause=0;{
struct Cyc_Absyn_Aggrfield*tag_field;tag_field=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),tag_field->name=Cyc_Toc_tag_sp,tag_field->tq=Cyc_Toc_mt_tq,tag_field->type=Cyc_Absyn_sint_type,tag_field->width=0,tag_field->attributes=0,tag_field->requires_clause=0;{
struct Cyc_Absyn_Aggrdecl*ad2=({struct _fat_ptr*_Tmp5=str;Cyc_Toc_make_c_struct_defn(_Tmp5,({struct Cyc_Absyn_Aggrfield*_Tmp6[2];_Tmp6[0]=tag_field,_Tmp6[1]=value_field;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Aggrfield*),2));}));});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp7->tag=5,_Tmp7->f1=ad2;_Tmp7;}),0U);_Tmp5->hd=_Tmp6;}),_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
({void*_Tmp5=Cyc_Absyn_strct(str);new_field->type=_Tmp5;});}}}}
# 3815
new_fields=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=new_field,_Tmp5->tl=new_fields;_Tmp5;});}}}
# 3817
({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(new_fields);new_ad->impl->fields=_Tmp5;});}}
# 3820
if(!seen_defn_before)
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp8=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp9->tag=5,_Tmp9->f1=new_ad;_Tmp9;});_Tmp7->r=_Tmp8;}),_Tmp7->loc=0U;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
return 0;}}}}
# 3824
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){
({int(*_Tmp0)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple43*))=(int(*)(struct Cyc_Absyn_Aggrdecl**,int(*)(struct _RegionHandle*,struct _tuple43*)))Cyc_Toc_use_toc_state;_Tmp0;})(& ad,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple44{struct Cyc_Toc_TocState*f0;struct Cyc_Absyn_Datatypedecl*f1;};
# 3852 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3855
struct _tuple44 _Tmp0=*env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0->datatypes_so_far;_Tmp1=_Tmp0.f1;{struct Cyc_Set_Set**datatypes_so_far=_Tmp2;struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
struct _tuple1*n=tud->name;
if(tud->fields==0 ||({int(*_Tmp3)(struct Cyc_Set_Set*,struct _tuple1*)=(int(*)(struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_member;_Tmp3;})(*datatypes_so_far,n))
return 0;
({struct Cyc_Set_Set*_Tmp3=({struct Cyc_Set_Set*(*_Tmp4)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*)=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct _tuple1*))Cyc_Set_rinsert;_Tmp4;})(d,*datatypes_so_far,n);*datatypes_so_far=_Tmp3;});
{struct Cyc_List_List*fields=(struct Cyc_List_List*)_check_null(tud->fields)->v;for(0;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fields->hd;
# 3863
struct Cyc_List_List*fs=0;
int i=1;
{struct Cyc_List_List*ts=f->typs;for(0;ts!=0;(ts=ts->tl,i ++)){
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),({struct _fat_ptr*_Tmp3=Cyc_Absyn_fieldname(i);f->name=_Tmp3;}),f->tq=(*((struct _tuple23*)ts->hd)).f0,({
void*_Tmp3=Cyc_Toc_typ_to_c((*((struct _tuple23*)ts->hd)).f1);f->type=_Tmp3;}),f->width=0,f->attributes=0,f->requires_clause=0;
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=f,_Tmp3->tl=fs;_Tmp3;});}}
# 3870
fs=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp4=({struct Cyc_Absyn_Aggrfield*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp5->name=Cyc_Toc_tag_sp,_Tmp5->tq=Cyc_Toc_mt_tq,_Tmp5->type=Cyc_Absyn_sint_type,_Tmp5->width=0,_Tmp5->attributes=0,_Tmp5->requires_clause=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(fs);_Tmp3->tl=_Tmp4;});_Tmp3;});{
struct Cyc_Absyn_Aggrdecl*ad=({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=_tag_fat("",sizeof(char),1U);_Tmp4;});Cyc_Toc_make_c_struct_defn(_Tmp3,fs);});
({struct _tuple1*_Tmp3=Cyc_Toc_collapse_qvars(f->name,tud->name);ad->name=_Tmp3;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp5->tag=5,_Tmp5->f1=ad;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_Toc_result_decls;_Tmp3;});}}}
# 3876
return 0;}}
# 3879
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
({int(*_Tmp0)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*))=(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;_Tmp0;})(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 3898 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple44*env){
# 3901
struct _tuple44 _Tmp0=*env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Toc_TocState*s=_Tmp2;struct Cyc_Absyn_Datatypedecl*xd=_Tmp1;
if(xd->fields==0)
return 0;{
struct Cyc_Toc_TocState _Tmp3=*s;void*_Tmp4;_Tmp4=_Tmp3.xdatatypes_so_far;{struct Cyc_Dict_Dict*xdatatypes_so_far=_Tmp4;
struct _tuple1*n=xd->name;
{struct Cyc_List_List*fs=(struct Cyc_List_List*)xd->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct _fat_ptr*fn=(*f->name).f1;
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_uint_exp(_get_fat_size(*fn,sizeof(char)),0U);
void*tag_typ=Cyc_Absyn_array_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq,sz_exp,Cyc_Absyn_false_type,0U);
# 3912
int*_Tmp5=({int*(*_Tmp6)(struct Cyc_Dict_Dict,struct _tuple1*)=(int*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp6;})(*xdatatypes_so_far,f->name);if(_Tmp5==0){
# 3914
struct Cyc_Absyn_Exp*initopt=0;
if((int)f->sc!=3)
initopt=Cyc_Absyn_string_exp(*fn,0U);{
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp8->tag=0,_Tmp8->f1=tag_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
({struct Cyc_Dict_Dict _Tmp6=
({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple1*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;_Tmp7;})(*xdatatypes_so_far,f->name,(int)f->sc!=3);
# 3920
*xdatatypes_so_far=_Tmp6;});{
# 3922
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*tqts=f->typs;for(0;tqts!=0;(tqts=tqts->tl,i ++)){
struct _fat_ptr*field_n=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*newf;newf=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),newf->name=field_n,newf->tq=(*((struct _tuple23*)tqts->hd)).f0,({
void*_Tmp6=Cyc_Toc_typ_to_c((*((struct _tuple23*)tqts->hd)).f1);newf->type=_Tmp6;}),newf->width=0,newf->attributes=0,newf->requires_clause=0;
fields=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=newf,_Tmp6->tl=fields;_Tmp6;});}}
# 3930
fields=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Aggrfield*_Tmp7=({struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=Cyc_Toc_tag_sp,_Tmp8->tq=Cyc_Toc_mt_tq,({
void*_Tmp9=Cyc_Absyn_cstar_type(Cyc_Absyn_char_type,Cyc_Toc_mt_tq);_Tmp8->type=_Tmp9;}),_Tmp8->width=0,_Tmp8->attributes=0,_Tmp8->requires_clause=0;_Tmp8;});
# 3930
_Tmp6->hd=_Tmp7;}),({
# 3932
struct Cyc_List_List*_Tmp7=Cyc_List_imp_rev(fields);_Tmp6->tl=_Tmp7;});_Tmp6;});{
struct Cyc_Absyn_Aggrdecl*strct_decl=({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("",sizeof(char),1U);_Tmp7;});Cyc_Toc_make_c_struct_defn(_Tmp6,fields);});
({struct _tuple1*_Tmp6=Cyc_Toc_collapse_qvars(f->name,xd->name);strct_decl->name=_Tmp6;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp8->tag=5,_Tmp8->f1=strct_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
goto _LL6;}}}}else{if(*((int*)_Tmp5)==0){
# 3938
if((int)f->sc!=3){
struct Cyc_Absyn_Exp*initopt=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*tag_decl=Cyc_Absyn_new_vardecl(0U,f->name,tag_typ,initopt);
tag_decl->sc=f->sc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp8->tag=0,_Tmp8->f1=tag_decl;_Tmp8;}),0U);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=Cyc_Toc_result_decls;_Tmp6;});
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple1*,int)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,int))Cyc_Dict_insert;_Tmp7;})(*xdatatypes_so_far,f->name,1);*xdatatypes_so_far=_Tmp6;});}
# 3945
goto _LL6;}else{
goto _LL6;}}_LL6:;}}
# 3949
return 0;}}}}
# 3952
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
({int(*_Tmp0)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*))=(int(*)(struct Cyc_Absyn_Datatypedecl*,int(*)(struct _RegionHandle*,struct _tuple44*)))Cyc_Toc_use_toc_state;_Tmp0;})(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 3956
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=2U;
if(ed->fields!=0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)ed->fields->v);}
# 3962
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_Tmp0=Cyc_Toc_typ_to_c(old_typ);vd->type=_Tmp0;});
# 3966
if((int)vd->sc==5 && Cyc_Tcutil_is_fat_pointer_type(old_typ))
vd->sc=2U;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer!=0){
struct Cyc_Absyn_Exp*init=vd->initializer;
if((int)vd->sc==0){
# 3975
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct Cyc_Toc_Env*nv2=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(nv2,init);}
# 3976
;_pop_region();}else{
# 3980
Cyc_Toc_exp_to_c(nv,init);}}else{
# 3983
void*_Tmp0=Cyc_Absyn_compress(old_typ);void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0)==4){_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*et=_Tmp3;struct Cyc_Absyn_Exp*num_elts_opt=_Tmp2;void*zt=_Tmp1;
# 3985
if(Cyc_Tcutil_force_type2bool(0,zt)){
if(num_elts_opt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("can't initialize zero-terminated array -- size unknown",sizeof(char),55U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_Absyn_Exp*lhs=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_Tmp4,({
struct Cyc_Absyn_Exp*_Tmp5=num_elts_opt;Cyc_Absyn_add_exp(_Tmp5,Cyc_Absyn_signed_int_exp(-1,0U),0U);}),0U);});
# 3991
struct Cyc_Absyn_Exp*rhs=Cyc_Absyn_signed_int_exp(0,0U);
({void*_Tmp4=({struct Cyc_Absyn_Stmt*_Tmp5=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lhs,rhs,0U),0U);Cyc_Toc_seq_stmt_r(_Tmp5,
Cyc_Absyn_new_stmt(s->r,0U));});
# 3992
s->r=_Tmp4;});}}
# 3995
goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 4002
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)d)){case 0:
 return d;case 1: _Tmp2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)d)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp2;
rhs->rhs=success;return d;}default: _Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f2;_Tmp0=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f3;{struct Cyc_List_List*path=_Tmp2;struct Cyc_List_List*sws=_Tmp1;void**d2=(void**)_Tmp0;
# 4007
({void*_Tmp3=Cyc_Toc_rewrite_decision(*d2,success);*d2=_Tmp3;});
for(1;sws!=0;sws=sws->tl){
struct _tuple34*_Tmp3=(struct _tuple34*)sws->hd;void*_Tmp4;_Tmp4=(void**)& _Tmp3->f1;{void**d2=(void**)_Tmp4;
({void*_Tmp5=Cyc_Toc_rewrite_decision(*d2,success);*d2=_Tmp5;});}}
# 4012
return d;}};}
# 4023 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4025
struct _RegionHandle _Tmp0=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_share_env(rgn,nv);struct Cyc_Toc_Env*nv=_Tmp1;
void*t=_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 4030
struct _tuple1*v;
int already_var;
{void*_Tmp2=e->r;void*_Tmp3;if(*((int*)_Tmp2)==1){_Tmp3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp2)->f1;{void*b=_Tmp3;
already_var=1;v=Cyc_Absyn_binding2qvar(b);goto _LL0;}}else{
already_var=0;v=Cyc_Toc_temp_var();goto _LL0;}_LL0:;}{
# 4036
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _fat_ptr*end_l=Cyc_Toc_fresh_label();
# 4041
struct Cyc_Absyn_Stmt*succ_stmt=s;
if(dopt!=0)
# 4045
dopt=Cyc_Toc_rewrite_decision(dopt,succ_stmt);{
# 4047
struct Cyc_Absyn_Switch_clause*c1;c1=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause)),c1->pattern=p,c1->pat_vars=0,c1->where_clause=0,c1->body=succ_stmt,c1->loc=0U;{
struct Cyc_List_List*lscs=({struct Cyc_List_List*(*_Tmp2)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp3)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple37*(*)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp3;});struct _RegionHandle*_Tmp3=rgn;struct _RegionHandle*_Tmp4=rgn;_Tmp2(_Tmp3,Cyc_Toc_gen_labels,_Tmp4,({struct Cyc_Absyn_Switch_clause*_Tmp5[1];_Tmp5[0]=c1;Cyc_List_rlist(rgn,_tag_fat(_Tmp5,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});
# 4052
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4058
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,nv,& mydecls,& mybodies,dopt,lscs,v);
# 4062
for(1;1;mybodies=mybodies->tl){
if(mybodies==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("letdecl_to_c: couldn't find env!",sizeof(char),33U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});{
struct _tuple42*_Tmp2=(struct _tuple42*)mybodies->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f2;{struct Cyc_Toc_Env*env=_Tmp4;struct Cyc_Absyn_Stmt*st=_Tmp3;
if(st==succ_stmt){Cyc_Toc_stmt_to_c(env,s);break;}}}}{
# 4069
struct Cyc_Absyn_Stmt*res=test_tree;
# 4071
for(1;mydecls!=0;mydecls=_check_null(mydecls)->tl){
struct _tuple40*_Tmp2=(struct _tuple40*)mydecls->hd;void*_Tmp3;_Tmp3=_Tmp2->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
res=({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=vd;_Tmp5;}),0U);Cyc_Absyn_decl_stmt(_Tmp4,res,0U);});}}
# 4075
if(!already_var)
res=({struct _tuple1*_Tmp2=v;void*_Tmp3=Cyc_Toc_typ_to_c(t);struct Cyc_Absyn_Exp*_Tmp4=e;Cyc_Absyn_declare_stmt(_Tmp2,_Tmp3,_Tmp4,res,0U);});{
struct Cyc_Absyn_Stmt*_Tmp2=res;_npop_handler(0);return _Tmp2;}}}}}}}
# 4026
;_pop_region();}
# 4082
static int Cyc_Toc_types_to_c_f1(int ignore,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 24: _Tmp1=(void**)&((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1->f2;{void**t=_Tmp1;
_Tmp1=t;goto _LL4;}case 19: _Tmp1=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL4: {void**t=_Tmp1;
_Tmp1=t;goto _LL6;}case 14: _Tmp1=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {void**t=_Tmp1;
_Tmp1=t;goto _LL8;}case 17: _Tmp1=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL8: {void**t=_Tmp1;
({void*_Tmp2=Cyc_Toc_typ_to_c(*t);*t=_Tmp2;});goto _LL0;}case 33: _Tmp1=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_MallocInfo*m=(struct Cyc_Absyn_MallocInfo*)_Tmp1;
# 4089
if(m->elt_type!=0)
({void**_Tmp2=({void**_Tmp3=_cycalloc(sizeof(void*));({void*_Tmp4=Cyc_Toc_typ_to_c(*m->elt_type);*_Tmp3=_Tmp4;});_Tmp3;});m->elt_type=_Tmp2;});
goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 4094
return 1;}
# 4096
static int Cyc_Toc_types_to_c_f2(int ignore,struct Cyc_Absyn_Stmt*s){
{void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 12: _Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Decl*d=_Tmp1;
# 4099
{void*_Tmp2=d->r;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
({void*_Tmp4=Cyc_Toc_typ_to_c(vd->type);vd->type=_Tmp4;});goto _LL7;}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp3;
# 4102
({void*_Tmp4=Cyc_Toc_typ_to_c(fd->i.ret_type);fd->i.ret_type=_Tmp4;});
{struct Cyc_List_List*args=fd->i.args;for(0;args!=0;args=args->tl){
({void*_Tmp4=Cyc_Toc_typ_to_c((*((struct _tuple9*)args->hd)).f2);(*((struct _tuple9*)args->hd)).f2=_Tmp4;});}}
goto _LL7;}case 5: _Tmp3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;
Cyc_Toc_aggrdecl_to_c(ad);goto _LL7;}case 7: _Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp3;
# 4108
if(ed->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)ed->fields->v;for(0;fs!=0;fs=fs->tl){
if(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag!=0)Cyc_Toc_exptypes_to_c(((struct Cyc_Absyn_Enumfield*)fs->hd)->tag);}}
goto _LL7;}case 8: _Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp3;
({void*_Tmp4=Cyc_Toc_typ_to_c(_check_null(td->defn));td->defn=_Tmp4;});goto _LL7;}default:
 goto _LL7;}_LL7:;}
# 4115
goto _LL0;}case 11:
# 4118
({void*_Tmp2=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct));_Tmp3->tag=0;_Tmp3;});s->r=_Tmp2;});
return 0;default:
 goto _LL0;}_LL0:;}
# 4122
return 1;}
# 4124
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
({void(*_Tmp0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_Toc_types_to_c_f1,Cyc_Toc_types_to_c_f2,1,e);}
# 4127
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
({void(*_Tmp0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp0;})(Cyc_Toc_types_to_c_f1,Cyc_Toc_types_to_c_f2,1,s);}
# 4134
static void Cyc_Toc_decls_to_c(struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int cinclude){
for(1;ds!=0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("decls_to_c: not at toplevel!",sizeof(char),29U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 4142
struct _tuple1*c_name=vd->name;
# 4144
if((int)vd->sc==4)
c_name=({struct _tuple1*_Tmp2=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Abs_n(0,1);_Tmp2->f0=_Tmp3;}),_Tmp2->f1=(*c_name).f1;_Tmp2;});
if(vd->initializer!=0){
if((int)vd->sc==4)vd->sc=2U;
if(cinclude)
Cyc_Toc_exptypes_to_c(_check_null(vd->initializer));else{
# 4151
Cyc_Toc_exp_to_c(nv,_check_null(vd->initializer));}}
# 4153
vd->name=c_name;
({enum Cyc_Absyn_Scope _Tmp2=Cyc_Toc_scope_to_c(vd->sc);vd->sc=_Tmp2;});
({void*_Tmp2=Cyc_Toc_typ_to_c(vd->type);vd->type=_Tmp2;});
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 4160
if((int)fd->sc==4){
({struct _tuple1*_Tmp2=({struct _tuple1*_Tmp3=_cycalloc(sizeof(struct _tuple1));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Abs_n(0,1);_Tmp3->f0=_Tmp4;}),_Tmp3->f1=(*fd->name).f1;_Tmp3;});fd->name=_Tmp2;});
fd->sc=2U;}
# 4164
Cyc_Toc_fndecl_to_c(nv,fd,cinclude);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 2:
 goto _LL8;case 3: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("letdecl at toplevel",sizeof(char),20U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});case 4:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("region decl at toplevel",sizeof(char),24U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr ap)=(int(*)(struct _fat_ptr ap))Cyc_Toc_toc_impos;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*sd=_Tmp1;
Cyc_Toc_aggrdecl_to_c(sd);goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
# 4172
tud->is_extensible?Cyc_Toc_xdatatypedecl_to_c(tud): Cyc_Toc_datatypedecl_to_c(tud);
goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 4175
Cyc_Toc_enumdecl_to_c(ed);
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 4179
td->tvs=0;
if(td->defn!=0){
({void*_Tmp2=Cyc_Toc_typ_to_c(td->defn);td->defn=_Tmp2;});{
# 4184
void*_Tmp2=td->defn;unsigned _Tmp3;void*_Tmp4;if(_Tmp2!=0){if(*((int*)_Tmp2)==9){if(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1)->r)==1){_Tmp4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1->r)->f1;_Tmp3=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1->loc;{struct Cyc_Absyn_Enumdecl*ed2=_Tmp4;unsigned loc=_Tmp3;
# 4186
struct Cyc_Absyn_Decl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Decl)),({void*_Tmp5=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp6->tag=7,_Tmp6->f1=ed2;_Tmp6;});ed->r=_Tmp5;}),ed->loc=loc;
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=ed,_Tmp5->tl=Cyc_Toc_result_decls;_Tmp5;});
({void*_Tmp5=(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp6->tag=0,({void*_Tmp7=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct));_Tmp8->tag=19,_Tmp8->f1=ed2->name,_Tmp8->f2=ed2;_Tmp8;});_Tmp6->f1=_Tmp7;}),_Tmp6->f2=0;_Tmp6;});td->defn=_Tmp5;});
goto _LL23;}}else{goto _LL26;}}else{goto _LL26;}}else{_LL26:
 goto _LL23;}_LL23:;}}else{
# 4193
({void*_Tmp2=(int)((struct Cyc_Absyn_Kind*)_check_null(td->kind)->v)->kind==2?Cyc_Toc_void_star_type(): Cyc_Absyn_void_type;td->defn=_Tmp2;});}
# 4197
if(Cyc_Flags_noexpand_r)
Cyc_Toc_result_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=d,_Tmp2->tl=Cyc_Toc_result_decls;_Tmp2;});
goto _LL0;}case 13:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL0;case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL1E;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_List_List*ds2=_Tmp1;
_Tmp1=ds2;goto _LL20;}case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_LL20: {struct Cyc_List_List*ds2=_Tmp1;
Cyc_Toc_decls_to_c(nv,ds2,cinclude);goto _LL0;}default: _Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp1;
Cyc_Toc_decls_to_c(nv,ds2,1);goto _LL0;}}_LL0:;}}}
# 4214
void Cyc_Toc_init (void){
# 4216
struct Cyc_Core_NewDynamicRegion _Tmp0=Cyc_Core__new_rckey(0U,"internal-error","internal-error",0);void*_Tmp1;_Tmp1=_Tmp0.key;{struct Cyc_Core_DynamicRegion*dyn=_Tmp1;
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& dyn->h;
ts=Cyc_Toc_empty_toc_state(h);;}
Cyc_Toc_toc_state=({struct Cyc_Toc_TocStateWrap*_Tmp2=_aqual_malloc(Cyc_Core_unique_qual,sizeof(struct Cyc_Toc_TocStateWrap));_Tmp2->dyn=dyn,_Tmp2->state=ts;_Tmp2;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_globals=_tag_fat(({unsigned _Tmp2=42;({struct _fat_ptr**_Tmp3=_cycalloc(_check_times(_Tmp2,sizeof(struct _fat_ptr*)));_Tmp3[0]=& Cyc_Toc__throw_str,_Tmp3[1]=& Cyc_Toc_setjmp_str,_Tmp3[2]=& Cyc_Toc__push_handler_str,_Tmp3[3]=& Cyc_Toc__pop_handler_str,_Tmp3[4]=& Cyc_Toc__exn_thrown_str,_Tmp3[5]=& Cyc_Toc__npop_handler_str,_Tmp3[6]=& Cyc_Toc__check_null_str,_Tmp3[7]=& Cyc_Toc__check_known_subscript_null_str,_Tmp3[8]=& Cyc_Toc__check_known_subscript_notnull_str,_Tmp3[9]=& Cyc_Toc__check_fat_subscript_str,_Tmp3[10]=& Cyc_Toc__tag_fat_str,_Tmp3[11]=& Cyc_Toc__untag_fat_ptr_str,_Tmp3[12]=& Cyc_Toc__check_fat_at_base_str,_Tmp3[13]=& Cyc_Toc__get_fat_size_str,_Tmp3[14]=& Cyc_Toc__get_zero_arr_size_char_str,_Tmp3[15]=& Cyc_Toc__get_zero_arr_size_other_str,_Tmp3[16]=& Cyc_Toc__fat_ptr_plus_str,_Tmp3[17]=& Cyc_Toc__zero_arr_plus_char_str,_Tmp3[18]=& Cyc_Toc__zero_arr_plus_other_str,_Tmp3[19]=& Cyc_Toc__fat_ptr_inplace_plus_str,_Tmp3[20]=& Cyc_Toc__zero_arr_inplace_plus_char_str,_Tmp3[21]=& Cyc_Toc__zero_arr_inplace_plus_other_str,_Tmp3[22]=& Cyc_Toc__fat_ptr_inplace_plus_post_str,_Tmp3[23]=& Cyc_Toc__zero_arr_inplace_plus_post_char_str,_Tmp3[24]=& Cyc_Toc__zero_arr_inplace_plus_post_other_str,_Tmp3[25]=& Cyc_Toc__cycalloc_str,_Tmp3[26]=& Cyc_Toc__cyccalloc_str,_Tmp3[27]=& Cyc_Toc__cycalloc_atomic_str,_Tmp3[28]=& Cyc_Toc__cyccalloc_atomic_str,_Tmp3[29]=& Cyc_Toc__region_malloc_str,_Tmp3[30]=& Cyc_Toc__region_calloc_str,_Tmp3[31]=& Cyc_Toc__check_times_str,_Tmp3[32]=& Cyc_Toc__new_region_str,_Tmp3[33]=& Cyc_Toc__push_region_str,_Tmp3[34]=& Cyc_Toc__pop_region_str,_Tmp3[35]=& Cyc_Toc__throw_arraybounds_str,_Tmp3[36]=& Cyc_Toc__fat_ptr_decrease_size_str,_Tmp3[37]=& Cyc_Toc__throw_match_str,_Tmp3[38]=& Cyc_Toc__fast_region_malloc_str,_Tmp3[39]=& Cyc_Toc__region_vmalloc_str,_Tmp3[40]=& Cyc_Toc__aqual_malloc_str,_Tmp3[41]=& Cyc_Toc__aqual_calloc_str;_Tmp3;});}),sizeof(struct _fat_ptr*),42U);}}
# 4271
void Cyc_Toc_finish (void){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_Tmp0=ts;struct Cyc_Toc_TocStateWrap*_Tmp1=Cyc_Toc_toc_state;ts=_Tmp1;Cyc_Toc_toc_state=_Tmp0;});{
struct Cyc_Toc_TocStateWrap _Tmp0=*_check_null(ts);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.state;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Toc_TocState*s=_Tmp1;
# 4276
{struct _RegionHandle*h=& dyn->h;
{struct Cyc_Toc_TocState _Tmp3=*s;void*_Tmp4;_Tmp4=_Tmp3.temp_labels;{struct Cyc_Xarray_Xarray*tls=_Tmp4;
Cyc_Xarray_reuse(tls);}}
# 4277
;}
# 4281
Cyc_Core_free_rckey(dyn);
({void(*_Tmp3)(struct Cyc_Toc_TocStateWrap*)=(void(*)(struct Cyc_Toc_TocStateWrap*))Cyc_Core_ufree;_Tmp3;})(ts);
# 4284
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;}}}
# 4290
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4292
Cyc_Toc_gpop_tables=({struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));*_Tmp0=pop_tables;_Tmp0;});
Cyc_Toc_init();{
struct _RegionHandle _Tmp0=_new_region(0U,"start");struct _RegionHandle*start=& _Tmp0;_push_region(start);
({struct Cyc_Toc_Env*_Tmp1=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_Tmp1,ds,0);});{
struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(Cyc_Toc_result_decls);_npop_handler(0);return _Tmp1;}
# 4295
;_pop_region();}}
