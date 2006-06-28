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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);
# 265
extern void*Cyc_List_find_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 165
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 326
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 333
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_SubsetType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_AssnDef_ExistAssnFn*f3;};
# 447 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 565
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};
# 733 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 920
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 923
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
int Cyc_Absyn_tvar_id(struct Cyc_Absyn_Tvar*);
# 928
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 934
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 941
void*Cyc_Absyn_compress(void*);
# 954
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 956
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 959
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 961
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 963
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned);
# 968
extern void*Cyc_Absyn_heap_rgn_type;
# 970
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 974
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 976
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);
# 995
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 999
void*Cyc_Absyn_exn_type (void);
# 1007
extern void*Cyc_Absyn_fat_bound_type;
# 1009
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 1011
void*Cyc_Absyn_bounds_one (void);
# 1014
void*Cyc_Absyn_fatconst (void);
void*Cyc_Absyn_thinconst (void);
# 1017
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1021
void*Cyc_Absyn_atb_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 1023
void*Cyc_Absyn_star_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1025
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1029
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1037
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1041
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1047
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 1049
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1082
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1104
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1109
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1111
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1118
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1121
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1126
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,int,unsigned);
# 1133
struct Cyc_Absyn_Exp*Cyc_Absyn_uniqueaqual_exp (void);
# 1141
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1152
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1161
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1165
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1170
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1210
void*Cyc_Absyn_pointer_expand(void*,int);
# 1212
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1217
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1219
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1244
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 1247
void Cyc_Absyn_visit_type(int(*)(void*,void*),void*,void*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 74
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 81
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 55 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 37
int Cyc_Tcutil_is_tag_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 51
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 54
int Cyc_Tcutil_is_bits_only_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 61
void*Cyc_Tcutil_pointer_region(void*);
# 64
int Cyc_Tcutil_rgn_of_pointer(void*,void**);
# 66
void*Cyc_Tcutil_pointer_aqual(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 75
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 77
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 82
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 85
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 92
void*Cyc_Tcutil_copy_type(void*);
# 95
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 99
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_coerce_arg(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_List_List*,void*,struct Cyc_List_List*);
# 120 "tcutil.h"
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f0;struct Cyc_Absyn_Exp*f1;int f2;};
# 123
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*,void*);
# 125
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 131
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 138
struct Cyc_List_List*Cyc_Tcutil_rsubst_effconstr(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 146
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 154
void*Cyc_Tcutil_evaluate_effect(struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;};struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 164
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 175
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 177
void Cyc_Tcutil_check_bound(unsigned,unsigned,void*,int);
# 179
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 195
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 200
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 205
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);struct _tuple15{int f0;void*f1;};
# 209
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 212
void*Cyc_Tcutil_normalize_effect(void*);
# 226
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 235
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 246
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 249
int Cyc_Tcutil_zeroable_type(void*);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 256
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 258
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);
# 271
void*Cyc_Tcutil_eval_aquals_of(struct Cyc_List_List*,void*);
# 277
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
void*Cyc_Tcutil_get_pointer_bounds(void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 53 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
# 55
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 57
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 60
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
struct Cyc_Core_Opt Cyc_Kinds_tvar_kind_opt(struct Cyc_Absyn_Tvar*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 75 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 92
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 114
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 123
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 139
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_List_List*Cyc_Tcenv_curr_effect_constraints(struct Cyc_Tcenv_Tenv*);
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 149
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 151
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 153
void Cyc_Tcenv_check_effect_constraints(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*);
# 33 "currgn.h"
void*Cyc_CurRgn_instantiate(void*,void*);struct _tuple16{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 53
extern int Cyc_Evexp_okay_szofarg(void*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
# 28
int Cyc_BansheeIf_add_constraint(unsigned,void*);
# 9 "subtype.h"
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible(unsigned,void*,void*);
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_warn(unsigned,void*,void*);
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned,void*,void*);
# 18
int Cyc_Subtype_check_aqual_bounds(struct Cyc_List_List*,void*,void*);struct _tuple17{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct Cyc_Tcpat_TcPatResult{struct _tuple17*tvars_and_effconstr_opt;struct Cyc_List_List*patvars;struct Cyc_List_List*aquals_bounds;};
# 58 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**,struct Cyc_Absyn_Exp*);
# 112
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 53 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){void*_T0;void**_T1;struct Cyc_Core_Opt*_T2;
# 55
Cyc_Warn_verr2(loc,ap);
if(topt==0)goto _TL0;_T1=topt;_T0=*_T1;goto _TL1;_TL0: _T2=Cyc_Tcenv_lookup_opt_type_vars(te);_T0=Cyc_Absyn_wildtyp(_T2);_TL1: return _T0;}
# 58
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 63
void Cyc_Tcexp_check_consume(unsigned loc,struct Cyc_List_List*aqb,void*t,struct Cyc_Absyn_Exp*e){int _T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;unsigned _T3;struct _fat_ptr _T4;_T0=
Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,t);if(!_T0)goto _TL2;_T1=Cyc_Tcutil_is_noalias_path(aqb,e);if(_T1)goto _TL2;else{goto _TL4;}
# 68
_TL4:{struct Cyc_Warn_String_Warn_Warg_struct _T5;_T5.tag=0;_T5.f1=_tag_fat("Cannot consume non-unique paths; do swap instead",sizeof(char),49U);_T2=_T5;}{struct Cyc_Warn_String_Warn_Warg_struct _T5=_T2;void*_T6[1];_T6[0]=& _T5;_T3=loc;_T4=_tag_fat(_T6,sizeof(void*),1);Cyc_Warn_err2(_T3,_T4);}goto _TL3;_TL2: _TL3:;}
# 73
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type_with_aqual(struct Cyc_Tcenv_Tenv*te,void*aq){struct Cyc_Core_Opt*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Core_Opt*_T3;struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Absyn_PtrInfo _T6;
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);_T0=& Cyc_Kinds_ako;_T1=(struct Cyc_Core_Opt*)_T0;{struct Cyc_Core_Opt*_T7=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T7->v=tenv_tvs;_T2=(struct Cyc_Core_Opt*)_T7;}{void*t2=Cyc_Absyn_new_evar(_T1,_T2);_T3=& Cyc_Kinds_eko;_T4=(struct Cyc_Core_Opt*)_T3;{struct Cyc_Core_Opt*_T7=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T7->v=tenv_tvs;_T5=(struct Cyc_Core_Opt*)_T7;}{void*rt=Cyc_Absyn_new_evar(_T4,_T5);
void*nbl=Cyc_Tcutil_any_bool(tenv_tvs);
void*b=Cyc_Tcutil_any_bounds(tenv_tvs);
void*zt=Cyc_Tcutil_any_bool(tenv_tvs);
void*rel=Cyc_Tcutil_any_bool(tenv_tvs);{struct Cyc_Absyn_PtrInfo _T7;
_T7.elt_type=t2;_T7.elt_tq=Cyc_Absyn_empty_tqual(0U);_T7.ptr_atts.eff=rt;_T7.ptr_atts.nullable=nbl;_T7.ptr_atts.bounds=b;_T7.ptr_atts.zero_term=zt;_T7.ptr_atts.ptrloc=0;_T7.ptr_atts.autoreleased=rel;_T7.ptr_atts.aqual=aq;_T6=_T7;}return _T6;}}}
# 84
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Core_Opt*_T3;void*_T4;void*_T5;void*_T6;struct Cyc_Absyn_PtrInfo _T7;
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);_T0=te;_T1=& Cyc_Kinds_aqko;_T2=(struct Cyc_Core_Opt*)_T1;{struct Cyc_Core_Opt*_T8=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T8->v=tenv_tvs;_T3=(struct Cyc_Core_Opt*)_T8;}_T4=Cyc_Absyn_new_evar(_T2,_T3);_T5=Cyc_Absyn_rtd_qual_type;_T6=Cyc_Absyn_aqual_var_type(_T4,_T5);_T7=Cyc_Tcexp_fresh_pointer_type_with_aqual(_T0,_T6);return _T7;}struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 90
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){struct Cyc_List_List*_T0;void*_T1;struct _tuple18*_T2;struct Cyc_List_List*_T3;int*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T9;void**_TA;int*_TB;unsigned _TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;void*_T11;union Cyc_Absyn_AggrInfo _T12;struct _union_AggrInfo_UnknownAggr _T13;unsigned _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;int(*_T16)(unsigned,struct _fat_ptr);void*(*_T17)(unsigned,struct _fat_ptr);unsigned _T18;struct _fat_ptr _T19;union Cyc_Absyn_AggrInfo _T1A;struct _union_AggrInfo_KnownAggr _T1B;struct Cyc_Absyn_Aggrdecl**_T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T1E;struct Cyc_Absyn_Aggrdecl*_T1F;struct Cyc_Absyn_ArrayInfo _T20;struct Cyc_Absyn_ArrayInfo _T21;struct Cyc_Absyn_Exp*_T22;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T25;struct Cyc_Absyn_Vardecl*_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T28;
# 94
if(topt!=0)goto _TL5;
# 98
if(des==0)goto _TL7;_T0=des;_T1=_T0->hd;_T2=(struct _tuple18*)_T1;{
struct _tuple18 _T29=*_T2;struct Cyc_List_List*_T2A;_T2A=_T29.f0;{struct Cyc_List_List*dls=_T2A;
if(dls==0)goto _TL9;_T3=dls;{
void*_T2B=_T3->hd;struct _fat_ptr*_T2C;_T4=(int*)_T2B;_T5=*_T4;if(_T5!=1)goto _TLB;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T2D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T2B;_T2C=_T2D->f1;}{struct _fat_ptr*n=_T2C;_T6=e;{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_T2D->tag=29;
# 103
_T2D->f1=0;_T2D->f2=0;_T2D->f3=des;_T7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T2D;}_T6->r=(void*)_T7;
return;}_TLB: goto _LL3;_LL3:;}goto _TLA;_TL9: _TLA:;}}goto _TL8;_TL7: _TL8: _T8=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T29=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T29->tag=25;
# 109
_T29->f1=des;_T9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T29;}_T8->r=(void*)_T9;
return;_TL5: _TA=topt;{
# 113
void*t=*_TA;
loop: {
void*_T29=Cyc_Absyn_compress(t);struct Cyc_Absyn_Vardecl*_T2A;int _T2B;struct Cyc_Absyn_Tqual _T2C;void*_T2D;union Cyc_Absyn_AggrInfo _T2E;_TB=(int*)_T29;_TC=*_TB;switch(_TC){case 0: _TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T29;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=24)goto _TLE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T29;_T11=_T2F->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T30=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T11;_T2E=_T30->f1;}}{union Cyc_Absyn_AggrInfo info=_T2E;{struct Cyc_Absyn_Aggrdecl*_T2F;_T12=info;_T13=_T12.UnknownAggr;_T14=_T13.tag;if(_T14!=1)goto _TL10;{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;
# 118
_T30.f1=_tag_fat("struct type improperly set",sizeof(char),27U);_T15=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_T15;void*_T31[1];_T31[0]=& _T30;_T17=Cyc_Warn_impos_loc2;{int(*_T32)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T17;_T16=_T32;}_T18=loc;_T19=_tag_fat(_T31,sizeof(void*),1);_T16(_T18,_T19);}goto _TL11;_TL10: _T1A=info;_T1B=_T1A.KnownAggr;_T1C=_T1B.val;{struct Cyc_Absyn_Aggrdecl*_T30=*_T1C;_T2F=_T30;}{struct Cyc_Absyn_Aggrdecl*ad=_T2F;_T1D=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T30=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T30->tag=28;_T1F=ad;
_T30->f1=_T1F->name;_T30->f2=0;_T30->f3=des;_T30->f4=ad;_T1E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T30;}_T1D->r=(void*)_T1E;}_TL11:;}goto _LL8;}_TLE: goto _LL11;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T29;_T20=_T2F->f1;_T2D=_T20.elt_type;_T21=_T2F->f1;_T2C=_T21.tq;}{void*at=_T2D;struct Cyc_Absyn_Tqual aq=_T2C;_T22=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T2F=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T2F->tag=25;
# 122
_T2F->f1=des;_T23=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2F;}_T22->r=(void*)_T23;goto _LL8;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T29;_T2B=_T2F->f2;}{int it=_T2B;_T24=e;{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T2F=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_T2F->tag=29;
_T2F->f1=t;_T2F->f2=it;_T2F->f3=des;_T25=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T2F;}_T24->r=(void*)_T25;goto _LL8;}case 12:{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T29;_T2A=_T2F->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2A;_T26=vd;
t=_T26->type;goto loop;}default: _LL11: _T27=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T2F=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T2F->tag=25;
_T2F->f1=des;_T28=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2F;}_T27->r=(void*)_T28;goto _LL8;}_LL8:;}}}
# 132
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 136
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T3;struct Cyc_Core_Opt*_T4;int _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Absyn_Exp*_T7;unsigned _T8;struct _fat_ptr _T9;_T0=e;{
void*_TA=_T0->r;_T1=(int*)_TA;_T2=*_T1;if(_T2!=4)goto _TL12;_T3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_TA;_T4=_T3->f2;if(_T4!=0)goto _TL14;_T5=Cyc_Flags_tc_aggressive_warn;
# 139
if(!_T5)goto _TL16;{struct Cyc_Warn_String_Warn_Warg_struct _TB;_TB.tag=0;
_TB.f1=_tag_fat("assignment in test",sizeof(char),19U);_T6=_TB;}{struct Cyc_Warn_String_Warn_Warg_struct _TB=_T6;void*_TC[1];_TC[0]=& _TB;_T7=e;_T8=_T7->loc;_T9=_tag_fat(_TC,sizeof(void*),1);Cyc_Warn_warn2(_T8,_T9);}goto _TL17;_TL16: _TL17: goto _LL0;_TL14: goto _LL3;_TL12: _LL3: goto _LL0;_LL0:;}}
# 147
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){struct Cyc_Tcenv_Tenv*_T0;void**_T1;struct Cyc_Absyn_Exp*_T2;int _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Absyn_Exp*_TA;unsigned _TB;struct _fat_ptr _TC;
Cyc_Tcexp_check_contains_assign(e);_T0=te;_T1=& Cyc_Absyn_sint_type;_T2=e;
Cyc_Tcexp_tcExp(_T0,_T1,_T2);_T3=
Cyc_Tcutil_coerce_to_bool(e);if(_T3)goto _TL18;else{goto _TL1A;}
_TL1A:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat("test of ",sizeof(char),9U);_T4=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T4;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;_TE.f1=msg_part;_T5=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T5;{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;_TF.f1=_tag_fat(" has type ",sizeof(char),11U);_T6=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_T6;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T10;_T10.tag=3;_T8=e;_T10.f1=_T8->topt;_T7=_T10;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T10=_T7;{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;
_T11.f1=_tag_fat(" instead of integral or pointer type",sizeof(char),37U);_T9=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T9;void*_T12[5];_T12[0]=& _TD;_T12[1]=& _TE;_T12[2]=& _TF;_T12[3]=& _T10;_T12[4]=& _T11;_TA=e;_TB=_TA->loc;_TC=_tag_fat(_T12,sizeof(void*),5);Cyc_Warn_err2(_TB,_TC);}}}}}goto _TL19;_TL18: _TL19:;}
# 169 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 174
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst*_T0;struct _union_Cnst_String_c _T1;unsigned _T2;struct _union_Cnst_Char_c _T3;struct _tuple4 _T4;enum Cyc_Absyn_Sign _T5;int _T6;void*_T7;void*_T8;void*_T9;void*_TA;struct _union_Cnst_Short_c _TB;struct _tuple5 _TC;enum Cyc_Absyn_Sign _TD;void*_TE;void*_TF;struct _union_Cnst_LongLong_c _T10;struct _tuple7 _T11;enum Cyc_Absyn_Sign _T12;void*_T13;void*_T14;struct _union_Cnst_Float_c _T15;struct _tuple8 _T16;int _T17;unsigned _T18;void*_T19;void**_T1A;void*_T1B;int*_T1C;int _T1D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1E;void*_T1F;int*_T20;int _T21;void*_T22;int _T23;unsigned _T24;void*_T25;int _T26;unsigned _T27;void*_T28;struct _union_Cnst_Int_c _T29;struct _tuple6 _T2A;struct _union_Cnst_Int_c _T2B;struct _tuple6 _T2C;void*_T2D;enum Cyc_Absyn_Sign _T2E;int _T2F;void**_T30;void*_T31;int*_T32;unsigned _T33;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T34;void*_T35;int*_T36;unsigned _T37;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T38;void*_T39;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T3A;enum Cyc_Absyn_Size_of _T3B;int _T3C;void*_T3D;union Cyc_Absyn_Cnst*_T3E;enum Cyc_Absyn_Sign _T3F;int _T40;char _T41;void*_T42;void*_T43;union Cyc_Absyn_Cnst*_T44;enum Cyc_Absyn_Sign _T45;int _T46;short _T47;void*_T48;void*_T49;void*_T4A;union Cyc_Absyn_Cnst*_T4B;void*_T4C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;void*_T50;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T51;int _T52;unsigned _T53;void*_T54;void*_T55;struct Cyc_Absyn_Exp*_T56;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T57;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T58;union Cyc_Absyn_Cnst*_T59;union Cyc_Absyn_Cnst _T5A;void*_T5B;void*_T5C;enum Cyc_Absyn_Sign _T5D;int _T5E;void**_T5F;void*_T60;int*_T61;int _T62;struct Cyc_Absyn_PtrInfo _T63;struct Cyc_Absyn_PtrAtts _T64;int _T65;struct Cyc_Warn_String_Warn_Warg_struct _T66;struct Cyc_Warn_Typ_Warn_Warg_struct _T67;void**_T68;struct Cyc_Absyn_Exp*_T69;unsigned _T6A;struct _fat_ptr _T6B;void**_T6C;void*_T6D;void*_T6E;struct _union_Cnst_String_c _T6F;struct _fat_ptr _T70;unsigned _T71;struct _union_Cnst_Wstring_c _T72;union Cyc_Absyn_Cnst _T73;unsigned _T74;struct Cyc_Absyn_Exp*_T75;void*_T76;void*_T77;void*_T78;struct Cyc_Absyn_Tqual _T79;void*_T7A;void*_T7B;void*_T7C;void*_T7D;void**_T7E;void*_T7F;int*_T80;unsigned _T81;struct Cyc_Absyn_ArrayInfo _T82;void*_T83;struct Cyc_Absyn_Tqual _T84;struct Cyc_Absyn_Exp*_T85;struct Cyc_List_List*_T86;void*_T87;void*_T88;struct Cyc_Absyn_Exp*_T89;unsigned _T8A;void*_T8B;void**_T8C;void*_T8D;enum Cyc_Absyn_Coercion _T8E;int _T8F;enum Cyc_Absyn_Coercion _T90;int _T91;struct Cyc_Absyn_Exp*_T92;struct Cyc_Absyn_Exp*_T93;void**_T94;void*_T95;void**_T96;void*_T97;void*_T98;void*_T99;
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;_T0=
_check_null(c);{union Cyc_Absyn_Cnst _T9A=*_T0;struct _fat_ptr _T9B;enum Cyc_Absyn_Sign _T9C;int _T9D;_T1=_T9A.String_c;_T2=_T1.tag;switch(_T2){case 2: _T3=_T9A.Char_c;_T4=_T3.val;_T5=_T4.f0;_T6=(int)_T5;switch(_T6){case Cyc_Absyn_Signed: _T7=Cyc_Absyn_schar_type;
return _T7;case Cyc_Absyn_Unsigned: _T8=Cyc_Absyn_uchar_type;
return _T8;default: _T9=Cyc_Absyn_char_type;
return _T9;};case 3: _TA=
Cyc_Absyn_wchar_type();return _TA;case 4: _TB=_T9A.Short_c;_TC=_TB.val;_TD=_TC.f0;if(_TD!=Cyc_Absyn_Unsigned)goto _TL1D;_TE=Cyc_Absyn_ushort_type;
return _TE;_TL1D: _TF=Cyc_Absyn_sshort_type;
return _TF;case 6: _T10=_T9A.LongLong_c;_T11=_T10.val;_T12=_T11.f0;if(_T12!=Cyc_Absyn_Unsigned)goto _TL1F;_T13=Cyc_Absyn_ulonglong_type;
return _T13;_TL1F: _T14=Cyc_Absyn_slonglong_type;
return _T14;case 7: _T15=_T9A.Float_c;_T16=_T15.val;_T9D=_T16.f1;{int i=_T9D;
# 189
if(topt!=0)goto _TL21;_T17=i;_T18=(unsigned)_T17;_T19=
Cyc_Absyn_gen_float_type(_T18);return _T19;_TL21: _T1A=topt;_T1B=*_T1A;{
void*_T9E=Cyc_Absyn_compress(_T1B);int _T9F;_T1C=(int*)_T9E;_T1D=*_T1C;if(_T1D!=0)goto _TL23;_T1E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T1F=_T1E->f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=2)goto _TL25;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T22=_TA0->f1;{struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*_TA1=(struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_T22;_T9F=_TA1->f1;}}{int i=_T9F;_T23=i;_T24=(unsigned)_T23;_T25=
Cyc_Absyn_gen_float_type(_T24);return _T25;}_TL25: goto _LL1E;_TL23: _LL1E: _T26=i;_T27=(unsigned)_T26;_T28=
Cyc_Absyn_gen_float_type(_T27);return _T28;;}}case 5: _T29=_T9A.Int_c;_T2A=_T29.val;_T9C=_T2A.f0;_T2B=_T9A.Int_c;_T2C=_T2B.val;_T9D=_T2C.f1;{enum Cyc_Absyn_Sign csn=_T9C;int i=_T9D;
# 199
if(topt!=0)goto _TL27;_T2E=csn;_T2F=(int)_T2E;
if(_T2F!=1)goto _TL29;_T2D=Cyc_Absyn_uint_type;goto _TL2A;_TL29: _T2D=Cyc_Absyn_sint_type;_TL2A: return _T2D;_TL27: _T30=topt;_T31=*_T30;{
void*_T9E=Cyc_Absyn_compress(_T31);void*_T9F;enum Cyc_Absyn_Sign _TA0;_T32=(int*)_T9E;_T33=*_T32;switch(_T33){case 0: _T34=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T35=_T34->f1;_T36=(int*)_T35;_T37=*_T36;switch(_T37){case 1: _T38=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T39=_T38->f1;_T3A=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T39;_T3B=_T3A->f2;_T3C=(int)_T3B;switch(_T3C){case Cyc_Absyn_Char_sz:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T3D=_TA1->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA2=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T3D;_TA0=_TA2->f1;}}{enum Cyc_Absyn_Sign sn=_TA0;_T3E=c;_T3F=sn;_T40=i;_T41=(char)_T40;
# 203
*_T3E=Cyc_Absyn_Char_c(_T3F,_T41);_T42=
Cyc_Tcexp_tcConst(te,loc,0,c,e);return _T42;}case Cyc_Absyn_Short_sz:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T43=_TA1->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA2=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T43;_TA0=_TA2->f1;}}{enum Cyc_Absyn_Sign sn=_TA0;_T44=c;_T45=sn;_T46=i;_T47=(short)_T46;
# 206
*_T44=Cyc_Absyn_Short_c(_T45,_T47);_T48=
Cyc_Tcexp_tcConst(te,loc,0,c,e);return _T48;}case Cyc_Absyn_Int_sz:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T49=_TA1->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA2=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T49;_TA0=_TA2->f1;}}{enum Cyc_Absyn_Sign sn=_TA0;_TA0=sn;goto _LL28;}case Cyc_Absyn_Long_sz:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T4A=_TA1->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA2=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T4A;_TA0=_TA2->f1;}}_LL28: {enum Cyc_Absyn_Sign sn=_TA0;_T4B=c;
# 212
*_T4B=Cyc_Absyn_Int_c(sn,i);_T4C=
Cyc_Tcexp_tcConst(te,loc,0,c,e);return _T4C;}default: goto _LL2D;};case 5: _T4D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T4E=_T4D->f2;if(_T4E==0)goto _TL2E;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9E;_T4F=_TA1->f2;{struct Cyc_List_List _TA2=*_T4F;_T50=_TA2.hd;_T9F=(void*)_T50;}}{void*t1=_T9F;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TA1=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TA1->tag=9;_T52=i;_T53=(unsigned)_T52;
# 221
_TA1->f1=Cyc_Absyn_uint_exp(_T53,0U);_T51=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TA1;}_T54=(void*)_T51;_T55=Cyc_Absyn_tag_type(_T54);return _T55;}_TL2E: goto _LL2D;default: goto _LL2D;};case 4: if(i!=0)goto _TL30;{
# 216
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};_T56=e;_T57=& nullc;_T58=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T57;
_T56->r=(void*)_T58;_T59=c;{union Cyc_Absyn_Cnst _TA1;_TA1.Null_c.tag=1U;
_TA1.Null_c.val=0;_T5A=_TA1;}*_T59=_T5A;_T5B=
Cyc_Tcexp_tcConst(te,loc,topt,c,e);return _T5B;}_TL30: goto _LL2D;default: _LL2D: _T5D=csn;_T5E=(int)_T5D;
# 223
if(_T5E!=1)goto _TL32;_T5C=Cyc_Absyn_uint_type;goto _TL33;_TL32: _T5C=Cyc_Absyn_sint_type;_TL33: return _T5C;};}}case 1:
# 226
 if(topt==0)goto _TL34;_T5F=topt;_T60=*_T5F;{
void*_T9E=Cyc_Absyn_compress(_T60);void*_T9F;_T61=(int*)_T9E;_T62=*_T61;if(_T62!=4)goto _TL36;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TA0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T9E;_T63=_TA0->f1;_T64=_T63.ptr_atts;_T9F=_T64.nullable;}{void*nbl=_T9F;_T65=
# 229
Cyc_Tcutil_force_type2bool(1,nbl);if(_T65)goto _TL38;else{goto _TL3A;}
_TL3A:{struct Cyc_Warn_String_Warn_Warg_struct _TA0;_TA0.tag=0;_TA0.f1=_tag_fat("Used NULL when expecting a value of type ",sizeof(char),42U);_T66=_TA0;}{struct Cyc_Warn_String_Warn_Warg_struct _TA0=_T66;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA1;_TA1.tag=2;_T68=topt;_TA1.f1=*_T68;_T67=_TA1;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA1=_T67;void*_TA2[2];_TA2[0]=& _TA0;_TA2[1]=& _TA1;_T69=e;_T6A=_T69->loc;_T6B=_tag_fat(_TA2,sizeof(void*),2);Cyc_Warn_err2(_T6A,_T6B);}}goto _TL39;_TL38: _TL39: _T6C=topt;_T6D=*_T6C;
return _T6D;}_TL36: goto _LL2F;_LL2F:;}goto _TL35;_TL34: _TL35: {
# 234
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;_T6E=
Cyc_Absyn_pointer_type(pi);return _T6E;}case 8: _T6F=_T9A.String_c;_T9B=_T6F.val;{struct _fat_ptr s=_T9B;_T70=s;_T71=
# 239
_get_fat_size(_T70,sizeof(char));string_numelts=(int)_T71;_T9B=s;goto _LL1A;}default: _T72=_T9A.Wstring_c;_T9B=_T72.val;_LL1A: {struct _fat_ptr s=_T9B;
# 242
if(string_numelts!=0)goto _TL3B;
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();goto _TL3C;_TL3B: _TL3C: _T73=
# 246
Cyc_Absyn_Int_c(1U,string_numelts);_T74=loc;{struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(_T73,_T74);_T75=elen;
_T75->topt=Cyc_Absyn_uint_type;_T76=string_elt_typ;_T77=Cyc_Absyn_heap_rgn_type;_T78=Cyc_Absyn_al_qual_type;_T79=
# 251
Cyc_Absyn_const_tqual(0U);_T7A=
Cyc_Absyn_thin_bounds_exp(elen);_T7B=Cyc_Absyn_true_type;_T7C=Cyc_Absyn_false_type;{
# 251
void*t=Cyc_Absyn_atb_type(_T76,_T77,_T78,_T79,_T7A,_T7B,_T7C);
# 253
if(topt!=0)goto _TL3D;_T7D=t;
return _T7D;_TL3D: _T7E=topt;_T7F=*_T7E;{
void*_T9E=Cyc_Absyn_compress(_T7F);struct Cyc_Absyn_Tqual _T9F;_T80=(int*)_T9E;_T81=*_T80;switch(_T81){case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TA0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T9E;_T82=_TA0->f1;_T9F=_T82.tq;}{struct Cyc_Absyn_Tqual tq=_T9F;_T83=string_elt_typ;_T84=tq;_T85=elen;_T86=
# 260
Cyc_Tcenv_lookup_type_vars(te);_T87=Cyc_Tcutil_any_bool(_T86);_T88=
# 259
Cyc_Absyn_array_type(_T83,_T84,_T85,_T87,0U);return _T88;}case 4: _T89=e;_T8A=_T89->loc;_T8B=t;_T8C=topt;_T8D=*_T8C;{
# 264
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(_T8A,_T8B,_T8D);_T8E=c;_T8F=(int)_T8E;
if(_T8F==0)goto _TL40;_T90=c;_T91=(int)_T90;if(_T91==1)goto _TL40;_T92=e;
_T92->topt=t;_T93=e;_T94=topt;_T95=*_T94;
Cyc_Tcutil_unchecked_cast(_T93,_T95,3U);_T96=topt;_T97=*_T96;
return _T97;_TL40: _T98=t;
# 270
return _T98;}default: _T99=t;
return _T99;};}}}}};}}
# 277
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){void**_T0;int*_T1;unsigned _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;int(*_T4)(unsigned,struct _fat_ptr);void*(*_T5)(unsigned,struct _fat_ptr);unsigned _T6;struct _fat_ptr _T7;struct Cyc_Tcenv_Tenv*_T8;unsigned _T9;struct Cyc_Absyn_Vardecl*_TA;struct _tuple1*_TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_Absyn_Fndecl*_TE;struct Cyc_Absyn_Vardecl*_TF;struct Cyc_Tcenv_Tenv*_T10;unsigned _T11;struct Cyc_Absyn_Fndecl*_T12;struct _tuple1*_T13;void*_T14;struct Cyc_Tcenv_Tenv*_T15;int _T16;struct Cyc_Absyn_Vardecl*_T17;void*_T18;int*_T19;int _T1A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1B;void*_T1C;int*_T1D;int _T1E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Absyn_Exp*_T23;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T24;struct Cyc_Absyn_Vardecl*_T25;void*_T26;_T0=
_check_null(b);{void*_T27=*_T0;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Absyn_Vardecl*_T29;struct _tuple1*_T2A;_T1=(int*)_T27;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T27;_T2A=_T2B->f1;}{struct _tuple1*q=_T2A;{struct Cyc_Warn_String_Warn_Warg_struct _T2B;_T2B.tag=0;
_T2B.f1=_tag_fat("unresolved binding in tcVar",sizeof(char),28U);_T3=_T2B;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B=_T3;void*_T2C[1];_T2C[0]=& _T2B;_T5=Cyc_Warn_impos_loc2;{int(*_T2D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T5;_T4=_T2D;}_T6=loc;_T7=_tag_fat(_T2C,sizeof(void*),1);_T4(_T6,_T7);}}case 1:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T27;_T29=_T2B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T29;_T8=te;_T9=loc;_TA=vd;_TB=_TA->name;
# 283
Cyc_Tcenv_lookup_ordinary_global(_T8,_T9,_TB,1);_TC=vd;_TD=_TC->type;
return _TD;}case 2:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T27;_T28=_T2B->f1;}{struct Cyc_Absyn_Fndecl*fd=_T28;_TE=fd;_TF=_TE->fn_vardecl;
# 290
if(_TF!=0)goto _TL43;_T10=te;_T11=loc;_T12=fd;_T13=_T12->name;
Cyc_Tcenv_lookup_ordinary_global(_T10,_T11,_T13,1);goto _TL44;_TL43: _TL44: _T14=
Cyc_Tcutil_fndecl2type(fd);return _T14;}case 5:{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T27;_T29=_T2B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T29;_T29=vd;goto _LLA;}case 4:{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T27;_T29=_T2B->f1;}_LLA: {struct Cyc_Absyn_Vardecl*vd=_T29;_T29=vd;goto _LLC;}default:{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T2B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T27;_T29=_T2B->f1;}_LLC: {struct Cyc_Absyn_Vardecl*vd=_T29;_T15=te;_T16=_T15->allow_valueof;
# 296
if(!_T16)goto _TL45;_T17=vd;_T18=_T17->type;{
void*_T2B=Cyc_Absyn_compress(_T18);void*_T2C;_T19=(int*)_T2B;_T1A=*_T19;if(_T1A!=0)goto _TL47;_T1B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2B;_T1C=_T1B->f1;_T1D=(int*)_T1C;_T1E=*_T1D;if(_T1E!=5)goto _TL49;_T1F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2B;_T20=_T1F->f2;if(_T20==0)goto _TL4B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2B;_T21=_T2D->f2;{struct Cyc_List_List _T2E=*_T21;_T22=_T2E.hd;_T2C=(void*)_T22;}}{void*i=_T2C;_T23=e;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_T2D->tag=38;
_T2D->f1=i;_T24=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T2D;}_T23->r=(void*)_T24;goto _LLD;}_TL4B: goto _LL10;_TL49: goto _LL10;_TL47: _LL10: goto _LLD;_LLD:;}goto _TL46;_TL45: _TL46: _T25=vd;_T26=_T25->type;
# 301
return _T26;}};}}
# 305
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_String_c _T5;unsigned _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_String_c _T8;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;void*_TC;int*_TD;int _TE;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;void*_T12;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T13;union Cyc_Absyn_Cnst _T14;struct _union_Cnst_String_c _T15;unsigned _T16;struct Cyc_Absyn_Exp*_T17;void*_T18;union Cyc_Absyn_Cnst _T19;struct _union_Cnst_String_c _T1A;struct Cyc_Tcenv_Tenv*_T1B;struct _fat_ptr _T1C;int _T1D;struct Cyc_Absyn_Exp*_T1E;unsigned _T1F;struct Cyc_Core_Opt*_T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Tcenv_Tenv*_T25;void**_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_List_List*_T28;struct Cyc_Absyn_Exp*_T29;void*_T2A;int*_T2B;int _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Typ_Warn_Warg_struct _T2E;struct Cyc_Warn_String_Warn_Warg_struct _T2F;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_Absyn_Exp*_T32;unsigned _T33;struct _fat_ptr _T34;int _T35;struct Cyc_List_List**_T36;struct Cyc_List_List*_T37;int _T38;struct Cyc_List_List**_T39;struct Cyc_Absyn_Exp*_T3A;unsigned _T3B;struct Cyc_List_List*_T3C;void*_T3D;struct Cyc_Absyn_Exp*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;struct Cyc_Warn_String_Warn_Warg_struct _T41;struct Cyc_Absyn_Exp*_T42;unsigned _T43;struct _fat_ptr _T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;struct Cyc_List_List*_T46;void*_T47;struct Cyc_Absyn_Exp*_T48;unsigned _T49;struct _fat_ptr _T4A;struct Cyc_Tcenv_Tenv*_T4B;struct Cyc_List_List*_T4C;void*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_List_List*_T4F;struct Cyc_Core_Opt*_T50;void*_T51;struct Cyc_Tcenv_Tenv*_T52;struct Cyc_List_List*_T53;void*_T54;struct Cyc_Absyn_Exp*_T55;struct Cyc_List_List*_T56;void*_T57;struct Cyc_Absyn_Exp*_T58;unsigned _T59;struct Cyc_List_List*_T5A;struct Cyc_List_List*_T5B;void*_T5C;struct Cyc_Absyn_Exp*_T5D;void*_T5E;void*_T5F;struct Cyc_List_List*_T60;void*_T61;struct Cyc_Absyn_Exp*_T62;struct Cyc_List_List*_T63;_T0=fmt;{
# 312
void*_T64=_T0->r;struct _fat_ptr _T65;_T1=(int*)_T64;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T64;_T4=_T3->f1;_T5=_T4.String_c;_T6=_T5.tag;if(_T6!=8)goto _TL4E;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T66=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T64;_T7=_T66->f1;_T8=_T7.String_c;_T65=_T8.val;}{struct _fat_ptr s=_T65;_T65=s;goto _LL4;}_TL4E: goto _LL5;case 14: _T9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T64;_TA=_T9->f2;_TB=(struct Cyc_Absyn_Exp*)_TA;_TC=_TB->r;_TD=(int*)_TC;_TE=*_TD;if(_TE!=0)goto _TL50;_TF=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T64;_T10=_TF->f2;_T11=(struct Cyc_Absyn_Exp*)_T10;_T12=_T11->r;_T13=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T12;_T14=_T13->f1;_T15=_T14.String_c;_T16=_T15.tag;if(_T16!=8)goto _TL52;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T66=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T64;_T17=_T66->f2;{struct Cyc_Absyn_Exp _T67=*_T17;_T18=_T67.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T68=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18;_T19=_T68->f1;_T1A=_T19.String_c;_T65=_T1A.val;}}}_LL4:{struct _fat_ptr s=_T65;_T1B=te;_T1C=s;_T1D=isCproto;_T1E=fmt;_T1F=_T1E->loc;{
# 315
struct Cyc_List_List*desc_types=type_getter(_T1B,_T1C,_T1D,_T1F);
if(opt_args!=0)goto _TL54;
return;_TL54: _T20=opt_args;_T21=_T20->v;{
struct Cyc_List_List*args=(struct Cyc_List_List*)_T21;
# 320
_TL59: if(desc_types!=0)goto _TL5A;else{goto _TL58;}_TL5A: if(args!=0)goto _TL57;else{goto _TL58;}
# 322
_TL57:{int alias_coercion=0;_T22=desc_types;{
void*t=_T22->hd;_T23=args;_T24=_T23->hd;{
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_T24;_T25=te;_T26=& t;_T27=e;
Cyc_Tcexp_tcExp(_T25,_T26,_T27);_T28=
Cyc_Tcenv_curr_aquals_bounds(te);_T29=e;_T2A=t;_T2B=& alias_coercion;_T2C=Cyc_Tcutil_coerce_arg(_T28,_T29,_T2A,_T2B);if(_T2C)goto _TL5B;else{goto _TL5D;}
_TL5D:{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;_T66.f1=_tag_fat("descriptor has type ",sizeof(char),21U);_T2D=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T2D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T67;_T67.tag=2;_T67.f1=t;_T2E=_T67;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T67=_T2E;{struct Cyc_Warn_String_Warn_Warg_struct _T68;_T68.tag=0;
_T68.f1=_tag_fat(" but argument has type ",sizeof(char),24U);_T2F=_T68;}{struct Cyc_Warn_String_Warn_Warg_struct _T68=_T2F;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T69;_T69.tag=3;_T31=e;_T69.f1=_T31->topt;_T30=_T69;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T69=_T30;void*_T6A[4];_T6A[0]=& _T66;_T6A[1]=& _T67;_T6A[2]=& _T68;_T6A[3]=& _T69;_T32=e;_T33=_T32->loc;_T34=_tag_fat(_T6A,sizeof(void*),4);Cyc_Tcexp_err_and_explain(_T33,_T34);}}}}goto _TL5C;_TL5B: _TL5C: _T35=alias_coercion;
if(!_T35)goto _TL5E;_T36=alias_arg_exps;{struct Cyc_List_List*_T66=_cycalloc(sizeof(struct Cyc_List_List));_T38=arg_cnt;
_T66->hd=(void*)_T38;_T39=alias_arg_exps;_T66->tl=*_T39;_T37=(struct Cyc_List_List*)_T66;}*_T36=_T37;goto _TL5F;_TL5E: _TL5F: _T3A=e;_T3B=_T3A->loc;_T3C=
Cyc_Tcenv_curr_aquals_bounds(te);_T3D=t;_T3E=e;Cyc_Tcexp_check_consume(_T3B,_T3C,_T3D,_T3E);}}}_T3F=desc_types;
# 321
desc_types=_T3F->tl;_T40=args;args=_T40->tl;arg_cnt=arg_cnt + 1;goto _TL59;_TL58:
# 334
 if(desc_types==0)goto _TL60;{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;
_T66.f1=_tag_fat("too few arguments",sizeof(char),18U);_T41=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T41;void*_T67[1];_T67[0]=& _T66;_T42=fmt;_T43=_T42->loc;_T44=_tag_fat(_T67,sizeof(void*),1);Cyc_Warn_err2(_T43,_T44);}goto _TL61;_TL60: _TL61:
 if(args==0)goto _TL62;{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;
_T66.f1=_tag_fat("too many arguments",sizeof(char),19U);_T45=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T45;void*_T67[1];_T67[0]=& _T66;_T46=args;_T47=_T46->hd;_T48=(struct Cyc_Absyn_Exp*)_T47;_T49=_T48->loc;_T4A=_tag_fat(_T67,sizeof(void*),1);Cyc_Warn_err2(_T49,_T4A);}
# 339
_TL67: if(args!=0)goto _TL65;else{goto _TL66;}
_TL65: _T4B=te;_T4C=args;_T4D=_T4C->hd;_T4E=(struct Cyc_Absyn_Exp*)_T4D;Cyc_Tcexp_tcExp(_T4B,0,_T4E);_T4F=args;
# 339
args=_T4F->tl;goto _TL67;_TL66: goto _TL63;_TL62: _TL63:
# 342
 return;}}}_TL52: goto _LL5;_TL50: goto _LL5;default: _LL5:
# 346
 if(opt_args!=0)goto _TL68;
return;_TL68: _T50=opt_args;_T51=_T50->v;{
struct Cyc_List_List*args=(struct Cyc_List_List*)_T51;_TL6D: if(args!=0)goto _TL6B;else{goto _TL6C;}
_TL6B: _T52=te;_T53=args;_T54=_T53->hd;_T55=(struct Cyc_Absyn_Exp*)_T54;Cyc_Tcexp_tcExp(_T52,0,_T55);_T56=args;_T57=_T56->hd;_T58=(struct Cyc_Absyn_Exp*)_T57;_T59=_T58->loc;_T5A=
Cyc_Tcenv_curr_aquals_bounds(te);_T5B=args;_T5C=_T5B->hd;_T5D=(struct Cyc_Absyn_Exp*)_T5C;_T5E=_T5D->topt;_T5F=_check_null(_T5E);_T60=args;_T61=_T60->hd;_T62=(struct Cyc_Absyn_Exp*)_T61;Cyc_Tcexp_check_consume(_T59,_T5A,_T5F,_T62);_T63=args;
# 348
args=_T63->tl;goto _TL6D;_TL6C:;}
# 352
return;};}}
# 356
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;enum Cyc_Absyn_Primop _T3;int _T4;int _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Warn_Typ_Warn_Warg_struct _T7;unsigned _T8;struct _fat_ptr _T9;struct Cyc_Absyn_Exp*_TA;void*_TB;void*_TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_Typ_Warn_Warg_struct _TF;unsigned _T10;struct _fat_ptr _T11;void*_T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Typ_Warn_Warg_struct _T15;unsigned _T16;struct _fat_ptr _T17;struct Cyc_Absyn_Exp*_T18;void*_T19;void*_T1A;void*_T1B;int*_T1C;int _T1D;void*_T1E;struct Cyc_Absyn_PtrInfo _T1F;struct Cyc_Absyn_PtrAtts _T20;int _T21;unsigned _T22;void*_T23;void*_T24;void*_T25;void*_T26;int _T27;struct Cyc_Tcenv_Tenv*_T28;int _T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;unsigned _T2B;struct _fat_ptr _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Typ_Warn_Warg_struct _T2E;unsigned _T2F;struct _fat_ptr _T30;void*_T31;int _T32;enum Cyc_Absyn_Coercion _T33;int _T34;struct Cyc_Warn_String_Warn_Warg_struct _T35;struct Cyc_Warn_Typ_Warn_Warg_struct _T36;unsigned _T37;struct _fat_ptr _T38;void*_T39;struct Cyc_Warn_String_Warn_Warg_struct _T3A;struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;unsigned _T3C;struct _fat_ptr _T3D;void*_T3E;void*_T3F;void*_T40;int*_T41;int _T42;void*_T43;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T44;void*_T45;int*_T46;unsigned _T47;struct Cyc_Warn_String_Warn_Warg_struct _T48;struct Cyc_Warn_Typ_Warn_Warg_struct _T49;unsigned _T4A;struct _fat_ptr _T4B;void*_T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;int(*_T4E)(unsigned,struct _fat_ptr);void*(*_T4F)(unsigned,struct _fat_ptr);unsigned _T50;struct _fat_ptr _T51;_T0=e;_T1=_T0->topt;_T2=
# 358
_check_null(_T1);{void*t=Cyc_Absyn_compress(_T2);_T3=p;_T4=(int)_T3;switch(_T4){case Cyc_Absyn_Plus: goto _LL4;case Cyc_Absyn_Minus: _LL4: _T5=
# 362
Cyc_Tcutil_is_numeric(e);if(_T5)goto _TL6F;else{goto _TL71;}
_TL71:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_T6=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T6;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=t;_T7=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_T7;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T8=loc;_T9=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T8,_T9);}}goto _TL70;_TL6F: _TL70: _TA=e;_TB=_TA->topt;_TC=
_check_null(_TB);return _TC;case Cyc_Absyn_Not:
# 366
 Cyc_Tcexp_check_contains_assign(e);_TD=
Cyc_Tcutil_coerce_to_bool(e);if(_TD)goto _TL72;else{goto _TL74;}
_TL74:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat("expecting integral or * type but found ",sizeof(char),40U);_TE=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_TE;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=t;_TF=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_TF;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T10=loc;_T11=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T10,_T11);}}goto _TL73;_TL72: _TL73: _T12=Cyc_Absyn_sint_type;
return _T12;case Cyc_Absyn_Bitnot: _T13=
# 371
Cyc_Tcutil_is_integral(e);if(_T13)goto _TL75;else{goto _TL77;}
_TL77:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat("expecting integral type but found ",sizeof(char),35U);_T14=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T14;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=t;_T15=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_T15;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T16=loc;_T17=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T16,_T17);}}goto _TL76;_TL75: _TL76: _T18=e;_T19=_T18->topt;_T1A=
_check_null(_T19);return _T1A;case Cyc_Absyn_Numelts:{void*_T52;_T1B=t;_T1C=(int*)_T1B;_T1D=*_T1C;if(_T1D!=4)goto _TL78;_T1E=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T53=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1E;_T1F=_T53->f1;_T20=_T1F.ptr_atts;_T52=_T20.bounds;}{void*b=_T52;_T21=
# 377
Cyc_Tcutil_is_cvar_type(b);if(!_T21)goto _TL7A;_T22=loc;_T23=
# 380
Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T24=_check_null(_T23);_T25=Cyc_Absyn_fatconst();_T26=Cyc_BansheeIf_equality_constraint(_T24,_T25);Cyc_BansheeIf_add_constraint(_T22,_T26);goto _TL7B;
# 383
_TL7A:{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt==0)goto _TL7C;_T27=Cyc_Evexp_c_can_eval(eopt);if(_T27)goto _TL7C;else{goto _TL7E;}_TL7E: _T28=te;_T29=_T28->allow_valueof;if(_T29)goto _TL7C;else{goto _TL7F;}
_TL7F:{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;_T53.f1=_tag_fat("cannot use numelts on a pointer with abstract bounds",sizeof(char),53U);_T2A=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T2A;void*_T54[1];_T54[0]=& _T53;_T2B=loc;_T2C=_tag_fat(_T54,sizeof(void*),1);Cyc_Warn_err2(_T2B,_T2C);}goto _TL7D;_TL7C: _TL7D:;}_TL7B: goto _LLF;}_TL78:{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;
# 388
_T53.f1=_tag_fat("numelts requires pointer type, not ",sizeof(char),36U);_T2D=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T2D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T54;_T54.tag=2;_T54.f1=t;_T2E=_T54;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T54=_T2E;void*_T55[2];_T55[0]=& _T53;_T55[1]=& _T54;_T2F=loc;_T30=_tag_fat(_T55,sizeof(void*),2);Cyc_Warn_err2(_T2F,_T30);}}_LLF:;}_T31=Cyc_Absyn_uint_type;
# 390
return _T31;case Cyc_Absyn_Tagof:  {
# 394
struct Cyc_Absyn_PtrInfo pinfo=Cyc_Tcexp_fresh_pointer_type(te);
pinfo.ptr_atts.nullable=Cyc_Absyn_false_type;
pinfo.ptr_atts.bounds=Cyc_Absyn_bounds_one();
pinfo.ptr_atts.zero_term=Cyc_Absyn_false_type;{
void*p=Cyc_Absyn_pointer_type(pinfo);_T32=
Cyc_Tcutil_is_pointer_type(t);if(!_T32)goto _TL80;{
# 408 "tcexp.cyc"
enum Cyc_Absyn_Coercion coercion=Cyc_Subtype_coercible_exact(loc,t,p);_T33=coercion;_T34=(int)_T33;
if(_T34!=0)goto _TL82;{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;
_T52.f1=_tag_fat("expecting @thin@numelts(1)@nozeroterm pointer type but found %s",sizeof(char),64U);_T35=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T35;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=t;_T36=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_T36;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T37=loc;_T38=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T37,_T38);}}_T39=Cyc_Absyn_uint_type;
return _T39;_TL82:;}goto _TL81;
# 415
_TL80:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat("expecting @thin@numelts(1)@nozeroterm pointer type but found %s",sizeof(char),64U);_T3A=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T3A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=t;_T3B=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_T3B;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T3C=loc;_T3D=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T3C,_T3D);}}_T3E=Cyc_Absyn_uint_type;
return _T3E;_TL81: _T3F=
# 419
Cyc_Tcutil_pointer_elt_type(p);{void*elt_type=Cyc_Absyn_compress(_T3F);
void*rgn_type=Cyc_Tcutil_pointer_region(p);_T40=elt_type;_T41=(int*)_T40;_T42=*_T41;if(_T42!=0)goto _TL84;_T43=elt_type;_T44=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T43;_T45=_T44->f1;_T46=(int*)_T45;_T47=*_T46;switch(_T47){case 22: goto _LL14;case 23: goto _LL14;default: goto _LL19;}goto _TL85;_TL84: _LL19:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;
# 424
_T52.f1=_tag_fat("expecting pointer to datatype but found %s",sizeof(char),43U);_T48=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T48;{struct Cyc_Warn_Typ_Warn_Warg_struct _T53;_T53.tag=2;_T53.f1=elt_type;_T49=_T53;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T53=_T49;void*_T54[2];_T54[0]=& _T52;_T54[1]=& _T53;_T4A=loc;_T4B=_tag_fat(_T54,sizeof(void*),2);Cyc_Warn_err2(_T4A,_T4B);}}_TL85: _LL14:
# 426
 Cyc_Tcenv_check_effect_accessible(te,loc,rgn_type);_T4C=Cyc_Absyn_uint_type;
return _T4C;}}}default:{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;
_T52.f1=_tag_fat("Non-unary primop",sizeof(char),17U);_T4D=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T4D;void*_T53[1];_T53[0]=& _T52;_T4F=Cyc_Warn_impos_loc2;{int(*_T54)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))_T4F;_T4E=_T54;}_T50=loc;_T51=_tag_fat(_T53,sizeof(void*),1);_T4E(_T50,_T51);}};}}
# 434
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;void*_T3;int _T4;int _T5;void*_T6;_T0=e1;_T1=_T0->topt;{
void*t1=_check_null(_T1);_T2=e2;_T3=_T2->topt;{
void*t2=_check_null(_T3);
void*new_typ=Cyc_Tcutil_max_arithmetic_type(t1,t2);_T4=
Cyc_Unify_unify(t1,new_typ);if(_T4)goto _TL87;else{goto _TL89;}_TL89: Cyc_Tcutil_unchecked_cast(e1,new_typ,1U);goto _TL88;_TL87: _TL88: _T5=
Cyc_Unify_unify(t2,new_typ);if(_T5)goto _TL8A;else{goto _TL8C;}_TL8C: Cyc_Tcutil_unchecked_cast(e2,new_typ,1U);goto _TL8B;_TL8A: _TL8B: _T6=new_typ;
return _T6;}}}
# 444
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){int _T0;void*_T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Absyn_Exp*_T7;unsigned _T8;struct _fat_ptr _T9;int _TA;void*_TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_TypOpt_Warn_Warg_struct _TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Tcenv_Tenv*_T10;struct Cyc_Absyn_Exp*_T11;unsigned _T12;struct _fat_ptr _T13;void*_T14;_T0=
# 447
checker(e1);if(_T0)goto _TL8D;else{goto _TL8F;}
_TL8F:{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;_T15.f1=_tag_fat("type ",sizeof(char),6U);_T2=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_T2;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T16;_T16.tag=3;_T4=e1;_T16.f1=_T4->topt;_T3=_T16;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T16=_T3;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;_T17.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_T5=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_T5;void*_T18[3];_T18[0]=& _T15;_T18[1]=& _T16;_T18[2]=& _T17;_T6=te;_T7=e1;_T8=_T7->loc;_T9=_tag_fat(_T18,sizeof(void*),3);_T1=Cyc_Tcexp_expr_err(_T6,_T8,0,_T9);}}}return _T1;_TL8D: _TA=
checker(e2);if(_TA)goto _TL90;else{goto _TL92;}
_TL92:{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;_T15.f1=_tag_fat("type ",sizeof(char),6U);_TC=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TC;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T16;_T16.tag=3;_TE=e2;_T16.f1=_TE->topt;_TD=_T16;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T16=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;_T17.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_TF=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TF;void*_T18[3];_T18[0]=& _T15;_T18[1]=& _T16;_T18[2]=& _T17;_T10=te;_T11=e2;_T12=_T11->loc;_T13=_tag_fat(_T18,sizeof(void*),3);_TB=Cyc_Tcexp_expr_err(_T10,_T12,0,_T13);}}}return _TB;_TL90: _T14=
Cyc_Tcexp_arith_convert(te,e1,e2);return _T14;}
# 454
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;void*_T6;int*_T7;int _T8;void*_T9;struct Cyc_Absyn_PtrInfo _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PtrAtts _TD;struct Cyc_Absyn_PtrInfo _TE;struct Cyc_Absyn_PtrAtts _TF;struct Cyc_Absyn_PtrInfo _T10;struct Cyc_Absyn_PtrAtts _T11;struct Cyc_Absyn_PtrInfo _T12;struct Cyc_Absyn_PtrAtts _T13;struct Cyc_Absyn_PtrInfo _T14;struct Cyc_Absyn_PtrAtts _T15;struct Cyc_Absyn_PtrInfo _T16;struct Cyc_Absyn_PtrAtts _T17;struct Cyc_Absyn_Kind*_T18;struct Cyc_Absyn_Kind*_T19;struct Cyc_Absyn_Kind*_T1A;int _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;struct Cyc_Absyn_Exp*_T1D;unsigned _T1E;struct _fat_ptr _T1F;struct Cyc_List_List*_T20;void*_T21;int _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Absyn_Exp*_T24;unsigned _T25;struct _fat_ptr _T26;int _T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;struct Cyc_Warn_Typ_Warn_Warg_struct _T29;struct Cyc_Absyn_Exp*_T2A;unsigned _T2B;struct _fat_ptr _T2C;int _T2D;void*_T2E;int _T2F;int _T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;struct Cyc_Absyn_Exp*_T32;unsigned _T33;struct _fat_ptr _T34;struct Cyc_Absyn_Exp*_T35;unsigned _T36;void*_T37;void*_T38;void*_T39;int _T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;struct Cyc_Warn_Typ_Warn_Warg_struct _T3C;struct Cyc_Absyn_Exp*_T3D;unsigned _T3E;struct _fat_ptr _T3F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T40;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T41;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T42;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T43;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T44;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T45;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T46;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T47;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T48;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4B;void*_T4C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4D;void*_T4E;struct Cyc_Tcenv_Tenv*_T4F;struct Cyc_Absyn_Exp*_T50;struct Cyc_Absyn_Exp*_T51;int(*_T52)(struct Cyc_Absyn_Exp*);void*_T53;_T0=e1;_T1=_T0->topt;_T2=
_check_null(_T1);{void*t1=Cyc_Absyn_compress(_T2);_T3=e2;_T4=_T3->topt;_T5=
_check_null(_T4);{void*t2=Cyc_Absyn_compress(_T5);void*_T54;void*_T55;void*_T56;void*_T57;void*_T58;void*_T59;struct Cyc_Absyn_Tqual _T5A;void*_T5B;_T6=t1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=4)goto _TL93;_T9=t1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T5C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T9;_TA=_T5C->f1;_T5B=_TA.elt_type;_TB=_T5C->f1;_T5A=_TB.elt_tq;_TC=_T5C->f1;_TD=_TC.ptr_atts;_T59=_TD.eff;_TE=_T5C->f1;_TF=_TE.ptr_atts;_T58=_TF.nullable;_T10=_T5C->f1;_T11=_T10.ptr_atts;_T57=_T11.bounds;_T12=_T5C->f1;_T13=_T12.ptr_atts;_T56=_T13.zero_term;_T14=_T5C->f1;_T15=_T14.ptr_atts;_T55=_T15.autoreleased;_T16=_T5C->f1;_T17=_T16.ptr_atts;_T54=_T17.aqual;}{void*et=_T5B;struct Cyc_Absyn_Tqual tq=_T5A;void*r=_T59;void*n=_T58;void*b=_T57;void*zt=_T56;void*rel=_T55;void*aq=_T54;_T18=
# 459
Cyc_Tcutil_type_kind(et);_T19=& Cyc_Kinds_mk;_T1A=(struct Cyc_Absyn_Kind*)_T19;_T1B=Cyc_Kinds_kind_leq(_T18,_T1A);if(_T1B)goto _TL95;else{goto _TL97;}
_TL97:{struct Cyc_Warn_String_Warn_Warg_struct _T5C;_T5C.tag=0;_T5C.f1=_tag_fat("can't do arithmetic on abstract pointer type",sizeof(char),45U);_T1C=_T5C;}{struct Cyc_Warn_String_Warn_Warg_struct _T5C=_T1C;void*_T5D[1];_T5D[0]=& _T5C;_T1D=e1;_T1E=_T1D->loc;_T1F=_tag_fat(_T5D,sizeof(void*),1);Cyc_Warn_err2(_T1E,_T1F);}goto _TL96;_TL95: _TL96: _T20=
Cyc_Tcenv_curr_aquals_bounds(te);_T21=t1;_T22=Cyc_Tcutil_is_noalias_pointer(_T20,_T21,1);if(!_T22)goto _TL98;{struct Cyc_Warn_String_Warn_Warg_struct _T5C;_T5C.tag=0;
_T5C.f1=_tag_fat("can't do arithmetic on unique pointer",sizeof(char),38U);_T23=_T5C;}{struct Cyc_Warn_String_Warn_Warg_struct _T5C=_T23;void*_T5D[1];_T5D[0]=& _T5C;_T24=e1;_T25=_T24->loc;_T26=_tag_fat(_T5D,sizeof(void*),1);Cyc_Warn_err2(_T25,_T26);}goto _TL99;_TL98: _TL99: _T27=
Cyc_Tcutil_coerce_sint_type(e2);if(_T27)goto _TL9A;else{goto _TL9C;}
_TL9C:{struct Cyc_Warn_String_Warn_Warg_struct _T5C;_T5C.tag=0;_T5C.f1=_tag_fat("expecting int but found ",sizeof(char),25U);_T28=_T5C;}{struct Cyc_Warn_String_Warn_Warg_struct _T5C=_T28;{struct Cyc_Warn_Typ_Warn_Warg_struct _T5D;_T5D.tag=2;_T5D.f1=t2;_T29=_T5D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T5D=_T29;void*_T5E[2];_T5E[0]=& _T5C;_T5E[1]=& _T5D;_T2A=e2;_T2B=_T2A->loc;_T2C=_tag_fat(_T5E,sizeof(void*),2);Cyc_Warn_err2(_T2B,_T2C);}}goto _TL9B;_TL9A: _TL9B: _T2D=
Cyc_Tcutil_is_cvar_type(b);if(_T2D)goto _TL9D;else{goto _TL9F;}
_TL9F:{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt!=0)goto _TLA0;_T2E=t1;
return _T2E;_TLA0: _T2F=
# 471
Cyc_Tcutil_force_type2bool(0,zt);if(!_T2F)goto _TLA2;{
# 473
struct _tuple16 _T5C=Cyc_Evexp_eval_const_uint_exp(eopt);int _T5D;unsigned _T5E;_T5E=_T5C.f0;_T5D=_T5C.f1;{unsigned i=_T5E;int known=_T5D;_T30=known;
if(!_T30)goto _TLA4;if(i!=1U)goto _TLA4;{struct Cyc_Warn_String_Warn_Warg_struct _T5F;_T5F.tag=0;
_T5F.f1=_tag_fat("pointer arithmetic on thin, zero-terminated pointer may be expensive.",sizeof(char),70U);_T31=_T5F;}{struct Cyc_Warn_String_Warn_Warg_struct _T5F=_T31;void*_T60[1];_T60[0]=& _T5F;_T32=e1;_T33=_T32->loc;_T34=_tag_fat(_T60,sizeof(void*),1);Cyc_Warn_warn2(_T33,_T34);}goto _TLA5;_TLA4: _TLA5:;}}goto _TLA3;_TLA2: _TLA3:;}goto _TL9E;
# 481
_TL9D: _T35=e1;_T36=_T35->loc;_T37=b;_T38=Cyc_Absyn_fatconst();_T39=Cyc_BansheeIf_equality_constraint(_T37,_T38);_T3A=Cyc_BansheeIf_add_constraint(_T36,_T39);if(_T3A)goto _TLA6;else{goto _TLA8;}
_TLA8:{struct Cyc_Warn_String_Warn_Warg_struct _T5C;_T5C.tag=0;_T5C.f1=_tag_fat("Unable to determine bound on pointer type ",sizeof(char),43U);_T3B=_T5C;}{struct Cyc_Warn_String_Warn_Warg_struct _T5C=_T3B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T5D;_T5D.tag=2;_T5D.f1=t1;_T3C=_T5D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T5D=_T3C;void*_T5E[2];_T5E[0]=& _T5C;_T5E[1]=& _T5D;_T3D=e1;_T3E=_T3D->loc;_T3F=_tag_fat(_T5E,sizeof(void*),2);Cyc_Warn_err2(_T3E,_T3F);}}goto _TLA7;_TLA6: _TLA7: _TL9E: {
# 490
struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1;new_t1=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_T40=new_t1;_T40->tag=4;_T41=new_t1;_T41->f1.elt_type=et;_T42=new_t1;_T42->f1.elt_tq=tq;_T43=new_t1;_T43->f1.ptr_atts.eff=r;_T44=new_t1;_T44->f1.ptr_atts.nullable=Cyc_Absyn_true_type;_T45=new_t1;
_T45->f1.ptr_atts.bounds=Cyc_Absyn_fat_bound_type;_T46=new_t1;_T46->f1.ptr_atts.zero_term=zt;_T47=new_t1;
_T47->f1.ptr_atts.ptrloc=0;_T48=new_t1;_T48->f1.ptr_atts.autoreleased=rel;_T49=new_t1;_T49->f1.ptr_atts.aqual=aq;_T4A=e1;_T4B=new_t1;_T4C=(void*)_T4B;
Cyc_Tcutil_unchecked_cast(_T4A,_T4C,3U);_T4D=new_t1;_T4E=(void*)_T4D;
return _T4E;}}_TL93: _T4F=te;_T50=e1;_T51=e2;_T52=Cyc_Tcutil_is_numeric;_T53=
Cyc_Tcexp_tcArithBinop(_T4F,_T50,_T51,_T52);return _T53;;}}}
# 498
static void*Cyc_Tcexp_relqual(void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_PtrInfo _T4;struct Cyc_Absyn_PtrAtts _T5;void*_T6;void*_T7;void*_T8;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=4)goto _TLA9;_T3=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3;_T4=_T9->f1;_T5=_T4.ptr_atts;_T8=_T5.autoreleased;}{void*rel=_T8;_T6=rel;
# 500
return _T6;}_TLA9: _T7=Cyc_Absyn_false_type;
return _T7;;}
# 509
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;void*_T3;int _T4;int _T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Exp*_T8;int(*_T9)(struct Cyc_Absyn_Exp*);void*_TA;int _TB;void**_TC;unsigned _TD;void**_TE;void*_TF;int _T10;void**_T11;unsigned _T12;int _T13;void*_T14;int _T15;struct Cyc_Absyn_Exp*_T16;unsigned _T17;void*_T18;void*_T19;void*_T1A;int _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;struct Cyc_Warn_Typ_Warn_Warg_struct _T1D;struct Cyc_Absyn_Exp*_T1E;unsigned _T1F;struct _fat_ptr _T20;void*_T21;int _T22;void*_T23;void*_T24;int _T25;struct Cyc_Absyn_Exp*_T26;unsigned _T27;void*_T28;void*_T29;void*_T2A;void*_T2B;int _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Typ_Warn_Warg_struct _T2E;struct Cyc_Absyn_Exp*_T2F;unsigned _T30;struct _fat_ptr _T31;void*_T32;struct Cyc_Absyn_Exp*_T33;unsigned _T34;void*_T35;void*_T36;enum Cyc_Absyn_Coercion _T37;int _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;struct Cyc_Warn_String_Warn_Warg_struct _T3A;struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_Typ_Warn_Warg_struct _T3D;struct Cyc_Warn_String_Warn_Warg_struct _T3E;struct Cyc_Absyn_Exp*_T3F;unsigned _T40;struct _fat_ptr _T41;struct Cyc_Absyn_Kind*_T42;struct Cyc_Absyn_Kind*_T43;struct Cyc_Absyn_Kind*_T44;int _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Absyn_Exp*_T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;void*_T4B;int _T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;struct Cyc_Absyn_Exp*_T4E;unsigned _T4F;struct _fat_ptr _T50;int _T51;int _T52;void*_T53;int _T54;struct Cyc_Absyn_Exp*_T55;void*_T56;void*_T57;void*_T58;struct Cyc_Absyn_Tqual _T59;void*_T5A;void*_T5B;void*_T5C;struct Cyc_Absyn_Exp*_T5D;void*_T5E;void*_T5F;void*_T60;struct Cyc_Absyn_Tqual _T61;void*_T62;void*_T63;void*_T64;void*_T65;_T0=e1;_T1=_T0->topt;{
void*t1=_check_null(_T1);_T2=e2;_T3=_T2->topt;{
void*t2=_check_null(_T3);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;_T4=
# 515
Cyc_Tcutil_is_pointer_type(t1);if(_T4)goto _TLAB;else{goto _TLAD;}
_TLAD: _T5=Cyc_Tcutil_is_pointer_type(t2);if(!_T5)goto _TLAE;
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);goto _TLAF;_TLAE: _TLAF: _T6=te;_T7=e1;_T8=e2;_T9=Cyc_Tcutil_is_numeric;_TA=
Cyc_Tcexp_tcArithBinop(_T6,_T7,_T8,_T9);return _TA;_TLAB: _TB=
# 522
Cyc_Tcutil_is_pointer_type(t2);if(_TB)goto _TLB0;else{goto _TLB2;}
# 524
_TLB2: _TC=topt;_TD=(unsigned)_TC;if(_TD)goto _TLB7;else{goto _TLB6;}_TLB7: _TE=topt;_TF=*_TE;_T10=Cyc_Tcutil_is_pointer_type(_TF);if(_T10)goto _TLB5;else{goto _TLB6;}_TLB6: _T11=topt;_T12=(unsigned)_T11;if(_T12)goto _TLB3;else{goto _TLB5;}
_TLB5: _T13=Cyc_Tcutil_is_fat_pointer_type(t1);if(_T13)goto _TLB8;else{goto _TLBA;}
_TLBA:{void*pb=Cyc_Tcutil_get_pointer_bounds(t1);_T14=
_check_null(pb);_T15=Cyc_Tcutil_is_cvar_type(_T14);if(_T15)goto _TLBE;else{goto _TLBD;}_TLBE: _T16=e1;_T17=_T16->loc;_T18=pb;_T19=
Cyc_Absyn_fatconst();_T1A=Cyc_BansheeIf_equality_constraint(_T18,_T19);_T1B=Cyc_BansheeIf_add_constraint(_T17,_T1A);
# 527
if(_T1B)goto _TLBB;else{goto _TLBD;}
# 529
_TLBD:{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;_T66.f1=_tag_fat("Expected fat pointer type but got ",sizeof(char),35U);_T1C=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T1C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T67;_T67.tag=2;_T67.f1=t1;_T1D=_T67;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T67=_T1D;void*_T68[2];_T68[0]=& _T66;_T68[1]=& _T67;_T1E=e1;_T1F=_T1E->loc;_T20=_tag_fat(_T68,sizeof(void*),2);Cyc_Warn_err2(_T1F,_T20);}}goto _TLBC;_TLBB: _TLBC:;}goto _TLB9;_TLB8: _TLB9: _T21=t1;
# 532
return _T21;
# 535
_TLB3: _T22=Cyc_Tcutil_is_fat_pointer_type(t1);if(_T22)goto _TLC1;else{goto _TLC2;}_TLC2: _T23=
Cyc_Tcutil_get_pointer_bounds(t2);_T24=_check_null(_T23);_T25=Cyc_Tcutil_is_cvar_type(_T24);
# 535
if(_T25)goto _TLC3;else{goto _TLBF;}_TLC3: _T26=e1;_T27=_T26->loc;_T28=
# 537
Cyc_Tcutil_get_pointer_bounds(t1);_T29=_check_null(_T28);_T2A=
Cyc_Absyn_thinconst();_T2B=
# 537
Cyc_BansheeIf_equality_constraint(_T29,_T2A);_T2C=Cyc_BansheeIf_add_constraint(_T27,_T2B);
# 535
if(_T2C)goto _TLBF;else{goto _TLC1;}
# 539
_TLC1:{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;_T66.f1=_tag_fat("Expected thin pointer type but got ",sizeof(char),36U);_T2D=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T2D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T67;_T67.tag=2;_T67.f1=t2;_T2E=_T67;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T67=_T2E;void*_T68[2];_T68[0]=& _T66;_T68[1]=& _T67;_T2F=e1;_T30=_T2F->loc;_T31=_tag_fat(_T68,sizeof(void*),2);Cyc_Warn_err2(_T30,_T31);}}goto _TLC0;_TLBF: _TLC0:
# 541
 Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,3U);
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);_T32=Cyc_Absyn_sint_type;
return _T32;_TLB0: {
# 548
void*t1_elt=Cyc_Tcutil_pointer_elt_type(t1);
void*t2_elt=Cyc_Tcutil_pointer_elt_type(t2);_T33=e1;_T34=_T33->loc;_T35=t1_elt;_T36=t2_elt;{
enum Cyc_Absyn_Coercion coercion=Cyc_Subtype_coercible_exact(_T34,_T35,_T36);_T37=coercion;_T38=(int)_T37;
if(_T38!=0)goto _TLC4;{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;
_T66.f1=_tag_fat("pointer arithmetic on values of different ",sizeof(char),43U);_T39=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T39;{struct Cyc_Warn_String_Warn_Warg_struct _T67;_T67.tag=0;
_T67.f1=_tag_fat("types (",sizeof(char),8U);_T3A=_T67;}{struct Cyc_Warn_String_Warn_Warg_struct _T67=_T3A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T68;_T68.tag=2;_T68.f1=t1;_T3B=_T68;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T68=_T3B;{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;_T69.f1=_tag_fat(" != ",sizeof(char),5U);_T3C=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T3C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T6A;_T6A.tag=2;_T6A.f1=t2;_T3D=_T6A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T6A=_T3D;{struct Cyc_Warn_String_Warn_Warg_struct _T6B;_T6B.tag=0;_T6B.f1=_tag_fat(")",sizeof(char),2U);_T3E=_T6B;}{struct Cyc_Warn_String_Warn_Warg_struct _T6B=_T3E;void*_T6C[6];_T6C[0]=& _T66;_T6C[1]=& _T67;_T6C[2]=& _T68;_T6C[3]=& _T69;_T6C[4]=& _T6A;_T6C[5]=& _T6B;_T3F=e1;_T40=_T3F->loc;_T41=_tag_fat(_T6C,sizeof(void*),6);Cyc_Tcexp_err_and_explain(_T40,_T41);}}}}}}goto _TLC5;_TLC4: _TLC5: _T42=
# 556
Cyc_Tcutil_type_kind(t1_elt);_T43=& Cyc_Kinds_mk;_T44=(struct Cyc_Absyn_Kind*)_T43;_T45=Cyc_Kinds_kind_leq(_T42,_T44);if(_T45)goto _TLC6;else{goto _TLC8;}
_TLC8:{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;_T66.f1=_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U);_T46=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T46;void*_T67[1];_T67[0]=& _T66;_T47=e1;_T48=_T47->loc;_T49=_tag_fat(_T67,sizeof(void*),1);Cyc_Warn_err2(_T48,_T49);}goto _TLC7;_TLC6: _TLC7: _T4A=
Cyc_Tcenv_curr_aquals_bounds(te);_T4B=t1;_T4C=Cyc_Tcutil_is_noalias_pointer(_T4A,_T4B,1);if(!_T4C)goto _TLC9;{struct Cyc_Warn_String_Warn_Warg_struct _T66;_T66.tag=0;
_T66.f1=_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U);_T4D=_T66;}{struct Cyc_Warn_String_Warn_Warg_struct _T66=_T4D;void*_T67[1];_T67[0]=& _T66;_T4E=e1;_T4F=_T4E->loc;_T50=_tag_fat(_T67,sizeof(void*),1);Cyc_Warn_err2(_T4F,_T50);}goto _TLCA;_TLC9: _TLCA: _T51=
# 562
Cyc_Tcutil_is_fat_pointer_type(t1);_T52=Cyc_Tcutil_is_fat_pointer_type(t2);if(_T51 ^ _T52)goto _TLCB;else{goto _TLCD;}
_TLCD: _T53=Cyc_Absyn_sint_type;return _T53;_TLCB: _T54=
# 565
Cyc_Tcutil_is_fat_pointer_type(t1);if(!_T54)goto _TLCE;_T55=e1;_T56=
Cyc_Tcutil_pointer_elt_type(t1);_T57=
Cyc_Tcutil_pointer_region(t1);_T58=Cyc_Tcutil_pointer_aqual(t1);_T59=
Cyc_Absyn_empty_tqual(0U);_T5A=Cyc_Absyn_false_type;_T5B=Cyc_Tcexp_relqual(t1);_T5C=
# 566
Cyc_Absyn_star_type(_T56,_T57,_T58,_T59,_T5A,_T5B);Cyc_Tcutil_unchecked_cast(_T55,_T5C,3U);goto _TLCF;
# 572
_TLCE: _T5D=e2;_T5E=Cyc_Tcutil_pointer_elt_type(t2);_T5F=
Cyc_Tcutil_pointer_region(t2);_T60=Cyc_Tcutil_pointer_aqual(t2);_T61=
Cyc_Absyn_empty_tqual(0U);_T62=Cyc_Absyn_false_type;_T63=Cyc_Tcexp_relqual(t2);_T64=
# 572
Cyc_Absyn_star_type(_T5E,_T5F,_T60,_T61,_T62,_T63);Cyc_Tcutil_unchecked_cast(_T5D,_T64,3U);_TLCF: _T65=Cyc_Absyn_sint_type;
# 578
return _T65;}}}}}
# 581
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;int _T6;int _T7;void*_T8;struct Cyc_Absyn_Kind*_T9;enum Cyc_Absyn_KindQual _TA;int _TB;struct Cyc_Absyn_Kind*_TC;enum Cyc_Absyn_KindQual _TD;int _TE;int _TF;int _T10;void*_T11;enum Cyc_Absyn_Coercion _T12;int _T13;enum Cyc_Absyn_Coercion _T14;int _T15;void*_T16;void*_T17;struct _tuple0 _T18;void*_T19;int*_T1A;unsigned _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;void*_T1E;int*_T1F;int _T20;void*_T21;int*_T22;int _T23;void*_T24;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25;void*_T26;int*_T27;int _T28;void*_T29;void*_T2A;int*_T2B;int _T2C;void*_T2D;struct Cyc_Absyn_PtrInfo _T2E;void*_T2F;struct Cyc_Absyn_PtrInfo _T30;enum Cyc_Absyn_Coercion _T31;int _T32;enum Cyc_Absyn_Coercion _T33;int _T34;void*_T35;enum Cyc_Absyn_Coercion _T36;int _T37;void*_T38;enum Cyc_Absyn_Coercion _T39;int _T3A;void*_T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_Typ_Warn_Warg_struct _T3D;struct Cyc_Warn_String_Warn_Warg_struct _T3E;struct Cyc_Warn_Typ_Warn_Warg_struct _T3F;unsigned _T40;struct _fat_ptr _T41;struct Cyc_Core_Opt*_T42;void*_T43;_T0=e1;_T1=_T0->topt;_T2=
_check_null(_T1);{void*t1=Cyc_Absyn_compress(_T2);_T3=e2;_T4=_T3->topt;_T5=
_check_null(_T4);{void*t2=Cyc_Absyn_compress(_T5);_T6=
Cyc_Tcutil_is_numeric(e1);if(!_T6)goto _TLD0;_T7=Cyc_Tcutil_is_numeric(e2);if(!_T7)goto _TLD0;
Cyc_Tcexp_arith_convert(te,e1,e2);_T8=Cyc_Absyn_sint_type;
return _T8;_TLD0: _T9=
# 588
Cyc_Tcutil_type_kind(t1);_TA=_T9->kind;_TB=(int)_TA;if(_TB!=2)goto _TLD2;_TC=Cyc_Tcutil_type_kind(t2);_TD=_TC->kind;_TE=(int)_TD;if(_TE!=2)goto _TLD2;_TF=
Cyc_Tcutil_zero_to_null(t2,e1);if(_TF)goto _TLD6;else{goto _TLD7;}_TLD7: _T10=Cyc_Tcutil_zero_to_null(t1,e2);if(_T10)goto _TLD6;else{goto _TLD4;}
_TLD6: _T11=Cyc_Absyn_sint_type;return _T11;_TLD4:{
# 593
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,t1,t2);_T12=c;_T13=(int)_T12;
if(_T13!=0)goto _TLD8;
c=Cyc_Subtype_coercible(loc,t2,t1);_T14=c;_T15=(int)_T14;
if(_T15==0)goto _TLDA;
Cyc_Tcutil_unchecked_cast(e2,t1,c);_T16=Cyc_Absyn_sint_type;
return _T16;_TLDA: goto _TLD9;
# 602
_TLD8: Cyc_Tcutil_unchecked_cast(e1,t2,c);_T17=Cyc_Absyn_sint_type;
return _T17;_TLD9:;}goto _TLD3;_TLD2: _TLD3:{struct _tuple0 _T44;
# 608
_T44.f0=Cyc_Absyn_compress(t1);_T44.f1=Cyc_Absyn_compress(t2);_T18=_T44;}{struct _tuple0 _T44=_T18;void*_T45;void*_T46;_T19=_T44.f0;_T1A=(int*)_T19;_T1B=*_T1A;switch(_T1B){case 0: _T1C=_T44.f0;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f1;_T1F=(int*)_T1E;_T20=*_T1F;if(_T20!=4)goto _TLDD;_T21=_T44.f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=0)goto _TLDF;_T24=_T44.f1;_T25=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24;_T26=_T25->f1;_T27=(int*)_T26;_T28=*_T27;if(_T28!=4)goto _TLE1;_T29=Cyc_Absyn_sint_type;
return _T29;_TLE1: goto _LL5;_TLDF: goto _LL5;_TLDD: goto _LL5;case 4: _T2A=_T44.f1;_T2B=(int*)_T2A;_T2C=*_T2B;if(_T2C!=4)goto _TLE3;_T2D=_T44.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2D;_T2E=_T47->f1;_T46=_T2E.elt_type;}_T2F=_T44.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2F;_T30=_T47->f1;_T45=_T30.elt_type;}{void*t1a=_T46;void*t2a=_T45;_T31=
# 611
Cyc_Subtype_coercible_exact(loc,t1a,t2a);_T32=(int)_T31;if(_T32!=0)goto _TLE7;else{goto _TLE8;}_TLE8: _T33=
Cyc_Subtype_coercible_exact(loc,t2a,t1a);_T34=(int)_T33;
# 611
if(_T34!=0)goto _TLE7;else{goto _TLE5;}
# 613
_TLE7: _T35=Cyc_Absyn_sint_type;return _T35;_TLE5: goto _LL6;}_TLE3: goto _LL5;default: _LL5: _LL6: {
# 617
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,t1,t2);_T36=c;_T37=(int)_T36;
if(_T37==0)goto _TLE9;
Cyc_Tcutil_unchecked_cast(e1,t2,c);_T38=Cyc_Absyn_sint_type;
return _T38;
# 623
_TLE9: c=Cyc_Subtype_coercible(loc,t2,t1);_T39=c;_T3A=(int)_T39;
if(_T3A==0)goto _TLEB;
Cyc_Tcutil_unchecked_cast(e2,t1,c);_T3B=Cyc_Absyn_sint_type;
return _T3B;_TLEB: goto _LL0;}}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;
# 632
_T44.f1=_tag_fat("comparison not allowed between ",sizeof(char),32U);_T3C=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T3C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T45;_T45.tag=2;_T45.f1=t1;_T3D=_T45;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T45=_T3D;{struct Cyc_Warn_String_Warn_Warg_struct _T46;_T46.tag=0;_T46.f1=_tag_fat(" and ",sizeof(char),6U);_T3E=_T46;}{struct Cyc_Warn_String_Warn_Warg_struct _T46=_T3E;{struct Cyc_Warn_Typ_Warn_Warg_struct _T47;_T47.tag=2;_T47.f1=t2;_T3F=_T47;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T47=_T3F;void*_T48[4];_T48[0]=& _T44;_T48[1]=& _T45;_T48[2]=& _T46;_T48[3]=& _T47;_T40=loc;_T41=_tag_fat(_T48,sizeof(void*),4);Cyc_Tcexp_err_and_explain(_T40,_T41);}}}}_T42=
Cyc_Tcenv_lookup_opt_type_vars(te);_T43=Cyc_Absyn_wildtyp(_T42);return _T43;}}}
# 638
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){enum Cyc_Absyn_Primop*_T0;int _T1;void*_T2;void*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Absyn_Exp*_T5;struct Cyc_Absyn_Exp*_T6;int(*_T7)(struct Cyc_Absyn_Exp*);void*_T8;struct Cyc_Tcenv_Tenv*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;int(*_TC)(struct Cyc_Absyn_Exp*);int*_TD;int _TE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TF;void*_T10;int*_T11;int _T12;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T13;void*_T14;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T15;enum Cyc_Absyn_Sign _T16;enum Cyc_Absyn_Primop*_T17;void*_T18;void*_T19;struct Cyc_Tcenv_Tenv*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_Exp*_T1C;int(*_T1D)(struct Cyc_Absyn_Exp*);int*_T1E;int _T1F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20;void*_T21;int*_T22;int _T23;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24;void*_T25;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T26;enum Cyc_Absyn_Sign _T27;enum Cyc_Absyn_Primop*_T28;void*_T29;void*_T2A;struct Cyc_Tcenv_Tenv*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Exp*_T2D;int(*_T2E)(struct Cyc_Absyn_Exp*);void*_T2F;int _T30;void*_T31;struct Cyc_Warn_String_Warn_Warg_struct _T32;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T33;struct Cyc_Absyn_Exp*_T34;struct Cyc_Warn_String_Warn_Warg_struct _T35;struct Cyc_Tcenv_Tenv*_T36;struct Cyc_Absyn_Exp*_T37;unsigned _T38;struct _fat_ptr _T39;int _T3A;void*_T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3D;struct Cyc_Absyn_Exp*_T3E;struct Cyc_Warn_String_Warn_Warg_struct _T3F;struct Cyc_Tcenv_Tenv*_T40;struct Cyc_Absyn_Exp*_T41;unsigned _T42;struct _fat_ptr _T43;struct Cyc_Absyn_Exp*_T44;void*_T45;void*_T46;void*_T47;struct _tuple0 _T48;struct Cyc_Absyn_Exp*_T49;void*_T4A;void*_T4B;struct Cyc_Absyn_Exp*_T4C;void*_T4D;void*_T4E;void*_T4F;int*_T50;int _T51;void*_T52;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T53;void*_T54;int*_T55;int _T56;void*_T57;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T58;void*_T59;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T5A;enum Cyc_Absyn_Sign _T5B;void*_T5C;int*_T5D;unsigned _T5E;void*_T5F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T60;void*_T61;int*_T62;int _T63;void*_T64;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T65;void*_T66;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T67;enum Cyc_Absyn_Sign _T68;void*_T69;int*_T6A;unsigned _T6B;void*_T6C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6D;void*_T6E;int*_T6F;int _T70;void*_T71;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T72;void*_T73;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T74;enum Cyc_Absyn_Sign _T75;void*_T76;int*_T77;int _T78;void*_T79;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7A;void*_T7B;int*_T7C;int _T7D;void*_T7E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7F;void*_T80;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T81;enum Cyc_Absyn_Sign _T82;void*_T83;int*_T84;int _T85;void*_T86;int*_T87;int _T88;void*_T89;int*_T8A;int _T8B;void*_T8C;int*_T8D;int _T8E;enum Cyc_Absyn_Primop*_T8F;enum Cyc_Absyn_Primop _T90;int _T91;enum Cyc_Absyn_Primop*_T92;enum Cyc_Absyn_Primop*_T93;enum Cyc_Absyn_Primop _T94;int _T95;enum Cyc_Absyn_Primop*_T96;enum Cyc_Absyn_Primop*_T97;enum Cyc_Absyn_Primop _T98;int _T99;enum Cyc_Absyn_Primop*_T9A;enum Cyc_Absyn_Primop*_T9B;enum Cyc_Absyn_Primop _T9C;int _T9D;enum Cyc_Absyn_Primop*_T9E;void*_T9F;struct Cyc_Warn_String_Warn_Warg_struct _TA0;int(*_TA1)(struct _fat_ptr);void*(*_TA2)(struct _fat_ptr);struct _fat_ptr _TA3;_T0=p;{
# 640
enum Cyc_Absyn_Primop _TA4=*_T0;_T1=(int)_TA4;switch(_T1){case Cyc_Absyn_Plus: _T2=
Cyc_Tcexp_tcPlus(te,e1,e2);return _T2;case Cyc_Absyn_Minus: _T3=
Cyc_Tcexp_tcMinus(te,topt,e1,e2);return _T3;case Cyc_Absyn_Times: _T4=te;_T5=e1;_T6=e2;_T7=Cyc_Tcutil_is_numeric;_T8=
# 644
Cyc_Tcexp_tcArithBinop(_T4,_T5,_T6,_T7);return _T8;case Cyc_Absyn_UDiv: goto _LLA;case Cyc_Absyn_Div: _LLA: _T9=te;_TA=e1;_TB=e2;_TC=Cyc_Tcutil_is_numeric;{
# 648
void*res=Cyc_Tcexp_tcArithBinop(_T9,_TA,_TB,_TC);
void*_TA5=Cyc_Absyn_compress(res);_TD=(int*)_TA5;_TE=*_TD;if(_TE!=0)goto _TLEE;_TF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA5;_T10=_TF->f1;_T11=(int*)_T10;_T12=*_T11;if(_T12!=1)goto _TLF0;_T13=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA5;_T14=_T13->f1;_T15=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T14;_T16=_T15->f1;if(_T16!=Cyc_Absyn_Unsigned)goto _TLF2;_T17=p;
# 651
*_T17=20U;_T18=res;return _T18;_TLF2: goto _LL32;_TLF0: goto _LL32;_TLEE: _LL32: _T19=res;
return _T19;;}case Cyc_Absyn_UMod: goto _LLE;case Cyc_Absyn_Mod: _LLE: _T1A=te;_T1B=e1;_T1C=e2;_T1D=Cyc_Tcutil_is_integral;{
# 658
void*res=Cyc_Tcexp_tcArithBinop(_T1A,_T1B,_T1C,_T1D);
void*_TA5=Cyc_Absyn_compress(res);_T1E=(int*)_TA5;_T1F=*_T1E;if(_T1F!=0)goto _TLF4;_T20=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA5;_T21=_T20->f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=1)goto _TLF6;_T24=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA5;_T25=_T24->f1;_T26=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T25;_T27=_T26->f1;if(_T27!=Cyc_Absyn_Unsigned)goto _TLF8;_T28=p;
# 661
*_T28=21U;_T29=res;return _T29;_TLF8: goto _LL37;_TLF6: goto _LL37;_TLF4: _LL37: _T2A=res;
return _T2A;;}case Cyc_Absyn_Bitand: goto _LL12;case Cyc_Absyn_Bitor: _LL12: goto _LL14;case Cyc_Absyn_Bitxor: _LL14: _T2B=te;_T2C=e1;_T2D=e2;_T2E=Cyc_Tcutil_is_integral;_T2F=
# 668
Cyc_Tcexp_tcArithBinop(_T2B,_T2C,_T2D,_T2E);return _T2F;case Cyc_Absyn_Bitlshift: goto _LL18;case Cyc_Absyn_Bitlrshift: _LL18: _T30=
# 671
Cyc_Tcutil_is_integral(e1);if(_T30)goto _TLFA;else{goto _TLFC;}
_TLFC:{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;_TA5.f1=_tag_fat("type ",sizeof(char),6U);_T32=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_T32;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _TA6;_TA6.tag=3;_T34=e1;_TA6.f1=_T34->topt;_T33=_TA6;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _TA6=_T33;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;_TA7.f1=_tag_fat(" cannot be used with << or >>",sizeof(char),30U);_T35=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T35;void*_TA8[3];_TA8[0]=& _TA5;_TA8[1]=& _TA6;_TA8[2]=& _TA7;_T36=te;_T37=e1;_T38=_T37->loc;_T39=_tag_fat(_TA8,sizeof(void*),3);_T31=Cyc_Tcexp_expr_err(_T36,_T38,0,_T39);}}}return _T31;_TLFA: _T3A=
Cyc_Tcutil_is_integral(e2);if(_T3A)goto _TLFD;else{goto _TLFF;}
_TLFF:{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;_TA5.f1=_tag_fat("type ",sizeof(char),6U);_T3C=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_T3C;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _TA6;_TA6.tag=3;_T3E=e2;_TA6.f1=_T3E->topt;_T3D=_TA6;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _TA6=_T3D;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;_TA7.f1=_tag_fat(" cannot be used with << or >>",sizeof(char),30U);_T3F=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T3F;void*_TA8[3];_TA8[0]=& _TA5;_TA8[1]=& _TA6;_TA8[2]=& _TA7;_T40=te;_T41=e2;_T42=_T41->loc;_T43=_tag_fat(_TA8,sizeof(void*),3);_T3B=Cyc_Tcexp_expr_err(_T40,_T42,0,_T43);}}}return _T3B;_TLFD: _T44=e1;_T45=_T44->topt;_T46=
_check_null(_T45);return _T46;case Cyc_Absyn_Eq: goto _LL1C;case Cyc_Absyn_Neq: _LL1C: _T47=
# 679
Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);return _T47;case Cyc_Absyn_Gt: goto _LL20;case Cyc_Absyn_Lt: _LL20: goto _LL22;case Cyc_Absyn_Gte: _LL22: goto _LL24;case Cyc_Absyn_Lte: _LL24: goto _LL26;case Cyc_Absyn_UGt: _LL26: goto _LL28;case Cyc_Absyn_ULt: _LL28: goto _LL2A;case Cyc_Absyn_UGte: _LL2A: goto _LL2C;case Cyc_Absyn_ULte: _LL2C: {
# 690
void*res=Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);{struct _tuple0 _TA5;_T49=e1;_T4A=_T49->topt;_T4B=
_check_null(_T4A);_TA5.f0=Cyc_Absyn_compress(_T4B);_T4C=e2;_T4D=_T4C->topt;_T4E=_check_null(_T4D);_TA5.f1=Cyc_Absyn_compress(_T4E);_T48=_TA5;}{struct _tuple0 _TA5=_T48;_T4F=_TA5.f0;_T50=(int*)_T4F;_T51=*_T50;if(_T51!=0)goto _TL100;_T52=_TA5.f0;_T53=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T52;_T54=_T53->f1;_T55=(int*)_T54;_T56=*_T55;if(_T56!=1)goto _TL102;_T57=_TA5.f0;_T58=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T57;_T59=_T58->f1;_T5A=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T59;_T5B=_T5A->f1;if(_T5B!=Cyc_Absyn_Unsigned)goto _TL104;goto _LL3D;_TL104: _T5C=_TA5.f1;_T5D=(int*)_T5C;_T5E=*_T5D;switch(_T5E){case 0: _T5F=_TA5.f1;_T60=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5F;_T61=_T60->f1;_T62=(int*)_T61;_T63=*_T62;if(_T63!=1)goto _TL107;_T64=_TA5.f1;_T65=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T64;_T66=_T65->f1;_T67=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T66;_T68=_T67->f1;if(_T68!=Cyc_Absyn_Unsigned)goto _TL109;goto _LL3C;_TL109: goto _LL42;_TL107: goto _LL42;case 4: goto _LL40;default: goto _LL42;}goto _TL103;_TL102: _T69=_TA5.f1;_T6A=(int*)_T69;_T6B=*_T6A;switch(_T6B){case 0: _T6C=_TA5.f1;_T6D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6C;_T6E=_T6D->f1;_T6F=(int*)_T6E;_T70=*_T6F;if(_T70!=1)goto _TL10C;_T71=_TA5.f1;_T72=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T71;_T73=_T72->f1;_T74=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T73;_T75=_T74->f1;if(_T75!=Cyc_Absyn_Unsigned)goto _TL10E;goto _LL3C;_TL10E: goto _LL42;_TL10C: goto _LL42;case 4: goto _LL40;default: goto _LL42;}_TL103: goto _TL101;_TL100: _T76=_TA5.f1;_T77=(int*)_T76;_T78=*_T77;if(_T78!=0)goto _TL110;_T79=_TA5.f1;_T7A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T79;_T7B=_T7A->f1;_T7C=(int*)_T7B;_T7D=*_T7C;if(_T7D!=1)goto _TL112;_T7E=_TA5.f1;_T7F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7E;_T80=_T7F->f1;_T81=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T80;_T82=_T81->f1;if(_T82!=Cyc_Absyn_Unsigned)goto _TL114;_LL3C: _LL3D: goto _LL3F;_TL114: _T83=_TA5.f0;_T84=(int*)_T83;_T85=*_T84;if(_T85!=4)goto _TL116;goto _LL3E;_TL116: goto _LL42;_TL112: _T86=_TA5.f0;_T87=(int*)_T86;_T88=*_T87;if(_T88!=4)goto _TL118;goto _LL3E;_TL118: goto _LL42;_TL110: _T89=_TA5.f0;_T8A=(int*)_T89;_T8B=*_T8A;if(_T8B!=4)goto _TL11A;_LL3E: _LL3F: goto _LL41;_TL11A: _T8C=_TA5.f1;_T8D=(int*)_T8C;_T8E=*_T8D;if(_T8E!=4)goto _TL11C;_LL40: _LL41: _T8F=p;_T90=*_T8F;_T91=(int)_T90;
# 696
if(_T91!=7)goto _TL11E;_T92=p;*_T92=22U;goto _TL11F;_TL11E: _TL11F: _T93=p;_T94=*_T93;_T95=(int)_T94;
if(_T95!=8)goto _TL120;_T96=p;*_T96=23U;goto _TL121;_TL120: _TL121: _T97=p;_T98=*_T97;_T99=(int)_T98;
if(_T99!=9)goto _TL122;_T9A=p;*_T9A=24U;goto _TL123;_TL122: _TL123: _T9B=p;_T9C=*_T9B;_T9D=(int)_T9C;
if(_T9D!=10)goto _TL124;_T9E=p;*_T9E=25U;goto _TL125;_TL124: _TL125: goto _LL39;_TL11C: _LL42: goto _LL39;_TL101: _LL39:;}_T9F=res;
# 703
return _T9F;}default:{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;
_TA5.f1=_tag_fat("bad binary primop",sizeof(char),18U);_TA0=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_TA0;void*_TA6[1];_TA6[0]=& _TA5;_TA2=Cyc_Warn_impos2;{int(*_TA7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TA2;_TA1=_TA7;}_TA3=_tag_fat(_TA6,sizeof(void*),1);_TA1(_TA3);}};}}
# 708
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop*p,struct Cyc_List_List*es){enum Cyc_Absyn_Primop*_T0;enum Cyc_Absyn_Primop _T1;int _T2;int _T3;struct Cyc_List_List*_T4;void*_T5;int _T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;struct Cyc_Warn_Typ_Warn_Warg_struct _T8;struct Cyc_Absyn_Exp*_T9;unsigned _TA;struct _fat_ptr _TB;void*_TC;struct Cyc_Tcenv_Tenv*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_List_List*_T11;int _T12;void*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Tcenv_Tenv*_T15;unsigned _T16;void**_T17;struct _fat_ptr _T18;struct Cyc_Tcenv_Tenv*_T19;unsigned _T1A;void**_T1B;enum Cyc_Absyn_Primop*_T1C;enum Cyc_Absyn_Primop _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Exp*_T20;void*_T21;struct Cyc_Tcenv_Tenv*_T22;unsigned _T23;void**_T24;enum Cyc_Absyn_Primop*_T25;struct Cyc_List_List*_T26;void*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_Absyn_Exp*_T2D;void*_T2E;void*_T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;struct Cyc_Tcenv_Tenv*_T31;unsigned _T32;void**_T33;struct _fat_ptr _T34;_T0=p;_T1=*_T0;_T2=(int)_T1;
# 716
if(_T2!=2)goto _TL126;_T3=Cyc_List_length(es);if(_T3!=1)goto _TL126;_T4=
_check_null(es);_T5=_T4->hd;{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_T5;
void*t=Cyc_Tcexp_tcExp(te,topt,e);_T6=
Cyc_Tcutil_is_numeric(e);if(_T6)goto _TL128;else{goto _TL12A;}
_TL12A:{struct Cyc_Warn_String_Warn_Warg_struct _T35;_T35.tag=0;_T35.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_T7=_T35;}{struct Cyc_Warn_String_Warn_Warg_struct _T35=_T7;{struct Cyc_Warn_Typ_Warn_Warg_struct _T36;_T36.tag=2;_T36.f1=t;_T8=_T36;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T36=_T8;void*_T37[2];_T37[0]=& _T35;_T37[1]=& _T36;_T9=e;_TA=_T9->loc;_TB=_tag_fat(_T37,sizeof(void*),2);Cyc_Warn_err2(_TA,_TB);}}goto _TL129;_TL128: _TL129: _TC=t;
return _TC;}_TL126:{
# 723
struct Cyc_List_List*es2=es;_TL12E: if(es2!=0)goto _TL12C;else{goto _TL12D;}
_TL12C: _TD=te;_TE=es2;_TF=_TE->hd;_T10=(struct Cyc_Absyn_Exp*)_TF;Cyc_Tcexp_tcExp(_TD,0,_T10);_T11=es2;
# 723
es2=_T11->tl;goto _TL12E;_TL12D:;}{
# 725
int _T35=Cyc_List_length(es);_T12=(int)_T35;switch(_T12){case 0:{struct Cyc_Warn_String_Warn_Warg_struct _T36;_T36.tag=0;
_T36.f1=_tag_fat("primitive operator has 0 arguments",sizeof(char),35U);_T14=_T36;}{struct Cyc_Warn_String_Warn_Warg_struct _T36=_T14;void*_T37[1];_T37[0]=& _T36;_T15=te;_T16=loc;_T17=topt;_T18=_tag_fat(_T37,sizeof(void*),1);_T13=Cyc_Tcexp_expr_err(_T15,_T16,_T17,_T18);}return _T13;case 1: _T19=te;_T1A=loc;_T1B=topt;_T1C=p;_T1D=*_T1C;_T1E=
_check_null(es);_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Exp*)_T1F;_T21=Cyc_Tcexp_tcUnPrimop(_T19,_T1A,_T1B,_T1D,_T20);return _T21;case 2: _T22=te;_T23=loc;_T24=topt;_T25=p;_T26=
_check_null(es);_T27=_T26->hd;_T28=(struct Cyc_Absyn_Exp*)_T27;_T29=es;_T2A=_T29->tl;_T2B=_check_null(_T2A);_T2C=_T2B->hd;_T2D=(struct Cyc_Absyn_Exp*)_T2C;_T2E=Cyc_Tcexp_tcBinPrimop(_T22,_T23,_T24,_T25,_T28,_T2D);return _T2E;default:{struct Cyc_Warn_String_Warn_Warg_struct _T36;_T36.tag=0;
_T36.f1=_tag_fat("primitive operator has > 2 arguments",sizeof(char),37U);_T30=_T36;}{struct Cyc_Warn_String_Warn_Warg_struct _T36=_T30;void*_T37[1];_T37[0]=& _T36;_T31=te;_T32=loc;_T33=topt;_T34=_tag_fat(_T37,sizeof(void*),1);_T2F=Cyc_Tcexp_expr_err(_T31,_T32,_T33,_T34);}return _T2F;};}}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};
# 733
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;void*_TA;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TB;union Cyc_Absyn_AggrInfo _TC;struct _union_AggrInfo_KnownAggr _TD;unsigned _TE;void*_TF;void*_T10;union Cyc_Absyn_AggrInfo _T11;struct _union_AggrInfo_KnownAggr _T12;struct Cyc_Absyn_Aggrdecl**_T13;struct Cyc_Absyn_Aggrdecl*_T14;struct Cyc_Absyn_AggrdeclImpl*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T17;unsigned _T18;struct _fat_ptr _T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_AggrdeclImpl*_T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;void*_T1E;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T1F;union Cyc_Absyn_DatatypeFieldInfo _T20;struct _union_DatatypeFieldInfo_KnownDatatypefield _T21;unsigned _T22;void*_T23;void*_T24;union Cyc_Absyn_DatatypeFieldInfo _T25;struct _union_DatatypeFieldInfo_KnownDatatypefield _T26;struct _tuple2 _T27;struct Cyc_Absyn_Datatypefield*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Tqual _T2B;int _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T2E;struct Cyc_Absyn_Datatypefield*_T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;unsigned _T31;struct _fat_ptr _T32;int _T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_List_List*_T36;void*_T37;struct Cyc_Absyn_Aggrfield*_T38;struct Cyc_Absyn_Tqual _T39;int _T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Absyn_Aggrfield*_T3D;struct _fat_ptr*_T3E;unsigned _T3F;struct _fat_ptr _T40;unsigned _T41;struct Cyc_Absyn_Aggrfield*_T42;void*_T43;int _T44;struct Cyc_List_List*_T45;void*_T46;struct Cyc_Absyn_ArrayInfo _T47;struct Cyc_Absyn_ArrayInfo _T48;struct Cyc_Absyn_Tqual _T49;int _T4A;struct Cyc_Warn_String_Warn_Warg_struct _T4B;unsigned _T4C;struct _fat_ptr _T4D;int _T4E;
t=Cyc_Absyn_compress(t);{struct Cyc_Absyn_Tqual _T4F;void*_T50;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 24: _T8=t;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_TA=_T9->f1;_TB=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TA;_TC=_TB->f1;_TD=_TC.KnownAggr;_TE=_TD.tag;if(_TE!=2)goto _TL132;_TF=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T10=_T51->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T52=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T10;_T11=_T52->f1;_T12=_T11.KnownAggr;_T13=_T12.val;{struct Cyc_Absyn_Aggrdecl*_T53=*_T13;_T50=_T53;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T50;_T14=ad;_T15=_T14->impl;
# 737
if(_T15!=0)goto _TL134;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;
_T51.f1=_tag_fat("attempt to write abstract ",sizeof(char),27U);_T16=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T16;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T52;_T52.tag=6;_T52.f1=ad;_T17=_T52;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T52=_T17;void*_T53[2];_T53[0]=& _T51;_T53[1]=& _T52;_T18=loc;_T19=_tag_fat(_T53,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}
return 0;_TL134: _T1A=ad;_T1B=_T1A->impl;_T50=_T1B->fields;goto _LL4;}_TL132: goto _LL9;case 23: _T1C=t;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f1;_T1F=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T1E;_T20=_T1F->f1;_T21=_T20.KnownDatatypefield;_T22=_T21.tag;if(_T22!=2)goto _TL136;_T23=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T23;_T24=_T51->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T52=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T24;_T25=_T52->f1;_T26=_T25.KnownDatatypefield;_T27=_T26.val;_T50=_T27.f1;}}{struct Cyc_Absyn_Datatypefield*df=_T50;_T28=df;{
# 753
struct Cyc_List_List*fs=_T28->typs;_TL13B: if(fs!=0)goto _TL139;else{goto _TL13A;}
_TL139: _T29=fs;_T2A=_T29->hd;{struct _tuple19*_T51=(struct _tuple19*)_T2A;void*_T52;struct Cyc_Absyn_Tqual _T53;{struct _tuple19 _T54=*_T51;_T53=_T54.f0;_T52=_T54.f1;}{struct Cyc_Absyn_Tqual tq=_T53;void*t=_T52;_T2B=tq;_T2C=_T2B.real_const;
if(!_T2C)goto _TL13C;{struct Cyc_Warn_String_Warn_Warg_struct _T54;_T54.tag=0;
_T54.f1=_tag_fat("attempt to overwrite a datatype field (",sizeof(char),40U);_T2D=_T54;}{struct Cyc_Warn_String_Warn_Warg_struct _T54=_T2D;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T55;_T55.tag=1;_T2F=df;_T55.f1=_T2F->name;_T2E=_T55;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T55=_T2E;{struct Cyc_Warn_String_Warn_Warg_struct _T56;_T56.tag=0;
_T56.f1=_tag_fat(") with a const member",sizeof(char),22U);_T30=_T56;}{struct Cyc_Warn_String_Warn_Warg_struct _T56=_T30;void*_T57[3];_T57[0]=& _T54;_T57[1]=& _T55;_T57[2]=& _T56;_T31=loc;_T32=_tag_fat(_T57,sizeof(void*),3);Cyc_Warn_err2(_T31,_T32);}}}
return 0;_TL13C: _T33=
# 760
Cyc_Tcexp_check_writable_aggr(loc,t);if(_T33)goto _TL13E;else{goto _TL140;}_TL140: return 0;_TL13E:;}}_T34=fs;
# 753
fs=_T34->tl;goto _TL13B;_TL13A:;}
# 762
return 1;}_TL136: goto _LL9;default: goto _LL9;};case 7: _T35=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T35;_T50=_T51->f3;}_LL4: {struct Cyc_List_List*fs=_T50;
# 743
_TL144: if(fs!=0)goto _TL142;else{goto _TL143;}
_TL142: _T36=fs;_T37=_T36->hd;{struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_T37;_T38=f;_T39=_T38->tq;_T3A=_T39.real_const;
if(!_T3A)goto _TL145;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;
_T51.f1=_tag_fat("attempt to overwrite an aggregate with const member ",sizeof(char),53U);_T3B=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T3B;{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T3D=f;_T3E=_T3D->name;_T52.f1=*_T3E;_T3C=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T3C;void*_T53[2];_T53[0]=& _T51;_T53[1]=& _T52;_T3F=loc;_T40=_tag_fat(_T53,sizeof(void*),2);Cyc_Warn_err2(_T3F,_T40);}}
return 0;_TL145: _T41=loc;_T42=f;_T43=_T42->type;_T44=
# 749
Cyc_Tcexp_check_writable_aggr(_T41,_T43);if(_T44)goto _TL147;else{goto _TL149;}_TL149: return 0;_TL147:;}_T45=fs;
# 743
fs=_T45->tl;goto _TL144;_TL143:
# 751
 return 1;}case 5: _T46=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T46;_T47=_T51->f1;_T50=_T47.elt_type;_T48=_T51->f1;_T4F=_T48.tq;}{void*elt_type=_T50;struct Cyc_Absyn_Tqual tq=_T4F;_T49=tq;_T4A=_T49.real_const;
# 764
if(!_T4A)goto _TL14A;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;
_T51.f1=_tag_fat("attempt to overwrite a const array",sizeof(char),35U);_T4B=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T4B;void*_T52[1];_T52[0]=& _T51;_T4C=loc;_T4D=_tag_fat(_T52,sizeof(void*),1);Cyc_Warn_err2(_T4C,_T4D);}
return 0;_TL14A: _T4E=
# 768
Cyc_Tcexp_check_writable_aggr(loc,elt_type);return _T4E;}default: _LL9:
 return 1;};}}
# 776
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;unsigned _T1;struct Cyc_Absyn_Exp*_T2;void*_T3;void*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;int*_T7;unsigned _T8;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T9;void*_TA;int*_TB;unsigned _TC;void*_TD;void*_TE;void*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_Absyn_Tqual _T12;int _T13;struct Cyc_Absyn_Exp*_T14;void*_T15;void*_T16;int*_T17;unsigned _T18;struct Cyc_Absyn_PtrInfo _T19;struct Cyc_Absyn_ArrayInfo _T1A;struct Cyc_Absyn_Tqual _T1B;int _T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;void*_T1F;int*_T20;unsigned _T21;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22;void*_T23;int*_T24;int _T25;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26;void*_T27;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T28;union Cyc_Absyn_AggrInfo _T29;struct _union_AggrInfo_KnownAggr _T2A;unsigned _T2B;void*_T2C;union Cyc_Absyn_AggrInfo _T2D;struct _union_AggrInfo_KnownAggr _T2E;struct Cyc_Absyn_Aggrfield*_T2F;struct Cyc_Absyn_Aggrdecl**_T30;struct Cyc_Absyn_Aggrdecl*_T31;struct _fat_ptr*_T32;struct Cyc_Absyn_Aggrfield*_T33;struct Cyc_Absyn_Tqual _T34;int _T35;struct Cyc_Absyn_Aggrfield*_T36;struct Cyc_Absyn_Tqual _T37;int _T38;struct Cyc_Absyn_Exp*_T39;void*_T3A;void*_T3B;int*_T3C;int _T3D;struct Cyc_Absyn_PtrInfo _T3E;struct Cyc_Absyn_PtrInfo _T3F;struct Cyc_Absyn_Tqual _T40;int _T41;int*_T42;unsigned _T43;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T44;void*_T45;int*_T46;int _T47;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T48;void*_T49;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T4A;union Cyc_Absyn_AggrInfo _T4B;struct _union_AggrInfo_KnownAggr _T4C;unsigned _T4D;void*_T4E;union Cyc_Absyn_AggrInfo _T4F;struct _union_AggrInfo_KnownAggr _T50;struct Cyc_Absyn_Aggrfield*_T51;struct Cyc_Absyn_Aggrdecl**_T52;struct Cyc_Absyn_Aggrdecl*_T53;struct _fat_ptr*_T54;struct Cyc_Absyn_Aggrfield*_T55;struct Cyc_Absyn_Tqual _T56;int _T57;struct Cyc_Absyn_Aggrfield*_T58;struct Cyc_Absyn_Tqual _T59;int _T5A;struct Cyc_Absyn_Exp*_T5B;void*_T5C;void*_T5D;int*_T5E;unsigned _T5F;struct Cyc_Absyn_PtrInfo _T60;struct Cyc_Absyn_ArrayInfo _T61;struct Cyc_Absyn_Tqual _T62;int _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Warn_Exp_Warn_Warg_struct _T65;struct Cyc_Absyn_Exp*_T66;unsigned _T67;struct _fat_ptr _T68;_T0=e;_T1=_T0->loc;_T2=e;_T3=_T2->topt;_T4=
# 779
_check_null(_T3);_T5=Cyc_Tcexp_check_writable_aggr(_T1,_T4);if(_T5)goto _TL14C;else{goto _TL14E;}_TL14E: return;_TL14C: _T6=e;{
void*_T69=_T6->r;struct _fat_ptr*_T6A;struct Cyc_Absyn_Exp*_T6B;struct Cyc_Absyn_Exp*_T6C;struct Cyc_Absyn_Vardecl*_T6D;_T7=(int*)_T69;_T8=*_T7;switch(_T8){case 1: _T9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T69;_TA=_T9->f1;_TB=(int*)_TA;_TC=*_TB;switch(_TC){case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T69;_TD=_T6E->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T6F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_TD;_T6D=_T6F->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T6D;_T6D=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T69;_TE=_T6E->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T6F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TE;_T6D=_T6F->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T6D;_T6D=vd;goto _LL6;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T69;_TF=_T6E->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T6F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_TF;_T6D=_T6F->f1;}}_LL6: {struct Cyc_Absyn_Vardecl*vd=_T6D;_T6D=vd;goto _LL8;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T69;_T10=_T6E->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T6F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T10;_T6D=_T6F->f1;}}_LL8: {struct Cyc_Absyn_Vardecl*vd=_T6D;_T11=vd;_T12=_T11->tq;_T13=_T12.real_const;
# 784
if(_T13)goto _TL151;else{goto _TL153;}_TL153: return;_TL151: goto _LL0;}default: goto _LL15;};case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;_T6B=_T6E->f2;}{struct Cyc_Absyn_Exp*e1=_T6C;struct Cyc_Absyn_Exp*e2=_T6B;_T14=e1;_T15=_T14->topt;_T16=
# 786
_check_null(_T15);{void*_T6E=Cyc_Absyn_compress(_T16);struct Cyc_Absyn_Tqual _T6F;_T17=(int*)_T6E;_T18=*_T17;switch(_T18){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6E;_T19=_T70->f1;_T6F=_T19.elt_tq;}{struct Cyc_Absyn_Tqual tq=_T6F;_T6F=tq;goto _LL1B;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T70=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6E;_T1A=_T70->f1;_T6F=_T1A.tq;}_LL1B: {struct Cyc_Absyn_Tqual tq=_T6F;_T1B=tq;_T1C=_T1B.real_const;
# 788
if(_T1C)goto _TL155;else{goto _TL157;}_TL157: return;_TL155: goto _LL17;}default: goto _LL17;}_LL17:;}goto _LL0;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;_T6A=_T6E->f2;}{struct Cyc_Absyn_Exp*e1=_T6C;struct _fat_ptr*f=_T6A;_T1D=e1;_T1E=_T1D->topt;_T1F=
# 793
_check_null(_T1E);{void*_T6E=Cyc_Absyn_compress(_T1F);struct Cyc_List_List*_T6F;struct Cyc_Absyn_Aggrdecl**_T70;_T20=(int*)_T6E;_T21=*_T20;switch(_T21){case 0: _T22=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T23=_T22->f1;_T24=(int*)_T23;_T25=*_T24;if(_T25!=24)goto _TL159;_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T27=_T26->f1;_T28=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T27;_T29=_T28->f1;_T2A=_T29.KnownAggr;_T2B=_T2A.tag;if(_T2B!=2)goto _TL15B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T2C=_T71->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T72=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T2C;_T2D=_T72->f1;_T2E=_T2D.KnownAggr;_T70=_T2E.val;}}{struct Cyc_Absyn_Aggrdecl**adp=_T70;
# 795
if(adp!=0)goto _TL15D;_T2F=0;goto _TL15E;_TL15D: _T30=adp;_T31=*_T30;_T32=f;_T2F=Cyc_Absyn_lookup_decl_field(_T31,_T32);_TL15E: {struct Cyc_Absyn_Aggrfield*sf=_T2F;
if(sf==0)goto _TL161;else{goto _TL162;}_TL162: _T33=sf;_T34=_T33->tq;_T35=_T34.real_const;if(_T35)goto _TL15F;else{goto _TL161;}_TL161: return;_TL15F: goto _LL1E;}}_TL15B: goto _LL23;_TL159: goto _LL23;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6E;_T6F=_T71->f3;}{struct Cyc_List_List*fs=_T6F;
# 799
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0)goto _TL165;else{goto _TL166;}_TL166: _T36=sf;_T37=_T36->tq;_T38=_T37.real_const;if(_T38)goto _TL163;else{goto _TL165;}_TL165: return;_TL163: goto _LL1E;}default: _LL23: goto _LL1E;}_LL1E:;}goto _LL0;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;_T6A=_T6E->f2;}{struct Cyc_Absyn_Exp*e1=_T6C;struct _fat_ptr*f=_T6A;_T39=e1;_T3A=_T39->topt;_T3B=
# 806
_check_null(_T3A);{void*_T6E=Cyc_Absyn_compress(_T3B);struct Cyc_Absyn_Tqual _T6F;void*_T70;_T3C=(int*)_T6E;_T3D=*_T3C;if(_T3D!=4)goto _TL167;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6E;_T3E=_T71->f1;_T70=_T3E.elt_type;_T3F=_T71->f1;_T6F=_T3F.elt_tq;}{void*elt_typ=_T70;struct Cyc_Absyn_Tqual tq=_T6F;_T40=tq;_T41=_T40.real_const;
# 808
if(_T41)goto _TL169;else{goto _TL16B;}
_TL16B:{void*_T71=Cyc_Absyn_compress(elt_typ);struct Cyc_List_List*_T72;struct Cyc_Absyn_Aggrdecl**_T73;_T42=(int*)_T71;_T43=*_T42;switch(_T43){case 0: _T44=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T71;_T45=_T44->f1;_T46=(int*)_T45;_T47=*_T46;if(_T47!=24)goto _TL16D;_T48=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T71;_T49=_T48->f1;_T4A=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T49;_T4B=_T4A->f1;_T4C=_T4B.KnownAggr;_T4D=_T4C.tag;if(_T4D!=2)goto _TL16F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T71;_T4E=_T74->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T75=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4E;_T4F=_T75->f1;_T50=_T4F.KnownAggr;_T73=_T50.val;}}{struct Cyc_Absyn_Aggrdecl**adp=_T73;
# 812
if(adp!=0)goto _TL171;_T51=0;goto _TL172;_TL171: _T52=adp;_T53=*_T52;_T54=f;_T51=Cyc_Absyn_lookup_decl_field(_T53,_T54);_TL172: {
# 811
struct Cyc_Absyn_Aggrfield*sf=_T51;
# 813
if(sf==0)goto _TL175;else{goto _TL176;}_TL176: _T55=sf;_T56=_T55->tq;_T57=_T56.real_const;if(_T57)goto _TL173;else{goto _TL175;}_TL175: return;_TL173: goto _LL2A;}}_TL16F: goto _LL2F;_TL16D: goto _LL2F;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T71;_T72=_T74->f3;}{struct Cyc_List_List*fs=_T72;
# 816
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0)goto _TL179;else{goto _TL17A;}_TL17A: _T58=sf;_T59=_T58->tq;_T5A=_T59.real_const;if(_T5A)goto _TL177;else{goto _TL179;}_TL179: return;_TL177: goto _LL2A;}default: _LL2F: goto _LL2A;}_LL2A:;}goto _TL16A;_TL169: _TL16A: goto _LL25;}_TL167: goto _LL25;_LL25:;}goto _LL0;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;}{struct Cyc_Absyn_Exp*e1=_T6C;_T5B=e1;_T5C=_T5B->topt;_T5D=
# 827
_check_null(_T5C);{void*_T6E=Cyc_Absyn_compress(_T5D);struct Cyc_Absyn_Tqual _T6F;_T5E=(int*)_T6E;_T5F=*_T5E;switch(_T5F){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6E;_T60=_T70->f1;_T6F=_T60.elt_tq;}{struct Cyc_Absyn_Tqual tq=_T6F;_T6F=tq;goto _LL35;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T70=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6E;_T61=_T70->f1;_T6F=_T61.tq;}_LL35: {struct Cyc_Absyn_Tqual tq=_T6F;_T62=tq;_T63=_T62.real_const;
# 829
if(_T63)goto _TL17C;else{goto _TL17E;}_TL17E: return;_TL17C: goto _LL31;}default: goto _LL31;}_LL31:;}goto _LL0;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;}{struct Cyc_Absyn_Exp*e1=_T6C;_T6C=e1;goto _LL14;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T6E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T69;_T6C=_T6E->f1;}_LL14: {struct Cyc_Absyn_Exp*e1=_T6C;
# 834
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15: goto _LL0;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;
# 837
_T69.f1=_tag_fat("attempt to write a const location: ",sizeof(char),36U);_T64=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T64;{struct Cyc_Warn_Exp_Warn_Warg_struct _T6A;_T6A.tag=4;_T6A.f1=e;_T65=_T6A;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T6A=_T65;void*_T6B[2];_T6B[0]=& _T69;_T6B[1]=& _T6A;_T66=e;_T67=_T66->loc;_T68=_tag_fat(_T6B,sizeof(void*),2);Cyc_Warn_err2(_T67,_T68);}}}
# 840
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;int _T2;void*_T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Tcenv_Tenv*_T5;unsigned _T6;void**_T7;struct _fat_ptr _T8;struct Cyc_Absyn_Exp*_T9;void*_TA;int _TB;void*_TC;unsigned _TD;int _TE;struct Cyc_Absyn_Exp*_TF;unsigned _T10;void*_T11;void*_T12;void*_T13;int _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Typ_Warn_Warg_struct _T16;struct Cyc_Absyn_Exp*_T17;unsigned _T18;struct _fat_ptr _T19;void*_T1A;void**_T1B;int _T1C;void*_T1D;void**_T1E;int _T1F;enum Cyc_Absyn_Incrementor _T20;int _T21;enum Cyc_Absyn_Incrementor _T22;int _T23;struct Cyc_Absyn_Kind*_T24;struct Cyc_Absyn_Kind*_T25;struct Cyc_Absyn_Kind*_T26;int _T27;struct Cyc_Absyn_Exp*_T28;unsigned _T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc_List_List*_T2C;void*_T2D;int _T2E;struct Cyc_Absyn_Exp*_T2F;unsigned _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;struct Cyc_Warn_String_Warn_Warg_struct _T33;struct Cyc_Warn_Typ_Warn_Warg_struct _T34;struct Cyc_Absyn_Exp*_T35;unsigned _T36;struct _fat_ptr _T37;void*_T38;_T0=
# 843
Cyc_Tcenv_enter_lhs(te);_T1=e;Cyc_Tcexp_tcExpNoPromote(_T0,0,_T1);_T2=
Cyc_Absyn_is_lvalue(e);if(_T2)goto _TL17F;else{goto _TL181;}
_TL181:{struct Cyc_Warn_String_Warn_Warg_struct _T39;_T39.tag=0;_T39.f1=_tag_fat("increment/decrement of non-lvalue",sizeof(char),34U);_T4=_T39;}{struct Cyc_Warn_String_Warn_Warg_struct _T39=_T4;void*_T3A[1];_T3A[0]=& _T39;_T5=te;_T6=loc;_T7=topt;_T8=_tag_fat(_T3A,sizeof(void*),1);_T3=Cyc_Tcexp_expr_err(_T5,_T6,_T7,_T8);}return _T3;_TL17F:
 Cyc_Tcexp_check_writable(te,e);_T9=e;_TA=_T9->topt;{
void*t=_check_null(_TA);_TB=
# 849
Cyc_Tcutil_is_numeric(e);if(_TB)goto _TL182;else{goto _TL184;}
_TL184:{void*pb=Cyc_Tcutil_get_pointer_bounds(t);
void*telt=Cyc_Absyn_void_type;_TC=pb;_TD=(unsigned)_TC;
if(!_TD)goto _TL185;_TE=Cyc_Tcutil_is_cvar_type(pb);if(!_TE)goto _TL185;_TF=e;_T10=_TF->loc;_T11=pb;_T12=
# 854
Cyc_Absyn_fatconst();_T13=Cyc_BansheeIf_equality_constraint(_T11,_T12);_T14=Cyc_BansheeIf_add_constraint(_T10,_T13);if(_T14)goto _TL187;else{goto _TL189;}
_TL189:{struct Cyc_Warn_String_Warn_Warg_struct _T39;_T39.tag=0;_T39.f1=_tag_fat("constraint failure: expecting arithmetic or ? type but found ",sizeof(char),62U);_T15=_T39;}{struct Cyc_Warn_String_Warn_Warg_struct _T39=_T15;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A;_T3A.tag=2;_T3A.f1=t;_T16=_T3A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A=_T16;void*_T3B[2];_T3B[0]=& _T39;_T3B[1]=& _T3A;_T17=e;_T18=_T17->loc;_T19=_tag_fat(_T3B,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}goto _TL188;_TL187: _TL188: goto _TL186;
# 859
_TL185: _T1A=t;_T1B=& telt;_T1C=Cyc_Tcutil_is_fat_pointer_type_elt(_T1A,_T1B);if(_T1C)goto _TL18C;else{goto _TL18D;}_TL18D: _T1D=t;_T1E=& telt;_T1F=
Cyc_Tcutil_is_zero_pointer_type_elt(_T1D,_T1E);
# 859
if(_T1F)goto _TL18E;else{goto _TL18A;}_TL18E: _T20=i;_T21=(int)_T20;if(_T21==0)goto _TL18C;else{goto _TL18F;}_TL18F: _T22=i;_T23=(int)_T22;if(_T23==1)goto _TL18C;else{goto _TL18A;}
# 861
_TL18C: _T24=Cyc_Tcutil_type_kind(telt);_T25=& Cyc_Kinds_mk;_T26=(struct Cyc_Absyn_Kind*)_T25;_T27=Cyc_Kinds_kind_leq(_T24,_T26);if(_T27)goto _TL190;else{goto _TL192;}
_TL192: _T28=e;_T29=_T28->loc;_T2A=_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U);_T2B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T29,_T2A,_T2B);goto _TL191;_TL190: _TL191: _T2C=
Cyc_Tcenv_curr_aquals_bounds(te);_T2D=t;_T2E=Cyc_Tcutil_is_noalias_pointer(_T2C,_T2D,1);if(!_T2E)goto _TL193;_T2F=e;_T30=_T2F->loc;_T31=
_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U);_T32=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T30,_T31,_T32);goto _TL194;_TL193: _TL194: goto _TL18B;
# 867
_TL18A:{struct Cyc_Warn_String_Warn_Warg_struct _T39;_T39.tag=0;_T39.f1=_tag_fat("expecting arithmetic or ? type but found ",sizeof(char),42U);_T33=_T39;}{struct Cyc_Warn_String_Warn_Warg_struct _T39=_T33;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A;_T3A.tag=2;_T3A.f1=t;_T34=_T3A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A=_T34;void*_T3B[2];_T3B[0]=& _T39;_T3B[1]=& _T3A;_T35=e;_T36=_T35->loc;_T37=_tag_fat(_T3B,sizeof(void*),2);Cyc_Warn_err2(_T36,_T37);}}_TL18B: _TL186:;}goto _TL183;_TL182: _TL183: _T38=t;
# 869
return _T38;}}
# 873
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;struct _fat_ptr _T2;struct Cyc_Core_Opt*_T3;int _T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;struct Cyc_Core_Opt*_T7;struct Cyc_Core_Opt*_T8;struct Cyc_List_List _T9;struct Cyc_List_List _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;int _T10;struct Cyc_Warn_String_Warn_Warg_struct _T11;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T12;struct Cyc_Absyn_Exp*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T15;struct Cyc_Absyn_Exp*_T16;unsigned _T17;struct _fat_ptr _T18;void*_T19;_T0=
# 875
Cyc_Tcenv_clear_abstract_val_ok(te);_T1=e1;_T2=_tag_fat("conditional expression",sizeof(char),23U);Cyc_Tcexp_tcTest(_T0,_T1,_T2);
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);_T4=
Cyc_Tcenv_abstract_val_ok(te);if(!_T4)goto _TL195;_T5=& Cyc_Kinds_ako;_T3=(struct Cyc_Core_Opt*)_T5;goto _TL196;_TL195: _T6=& Cyc_Kinds_mko;_T3=(struct Cyc_Core_Opt*)_T6;_TL196: {struct Cyc_Core_Opt*ko=_T3;_T7=ko;_T8=
Cyc_Tcenv_lookup_opt_type_vars(te);{void*t=Cyc_Absyn_new_evar(_T7,_T8);{struct Cyc_List_List _T1A;
_T1A.hd=e3;_T1A.tl=0;_T9=_T1A;}{struct Cyc_List_List l1=_T9;{struct Cyc_List_List _T1A;
_T1A.hd=e2;_TB=& l1;_T1A.tl=(struct Cyc_List_List*)_TB;_TA=_T1A;}{struct Cyc_List_List l2=_TA;_TC=
Cyc_Tcenv_curr_aquals_bounds(te);_TD=t;_TE=& l2;_TF=(struct Cyc_List_List*)_TE;_T10=Cyc_Tcutil_coerce_list(_TC,_TD,_TF);if(_T10)goto _TL197;else{goto _TL199;}
_TL199:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;_T1A.f1=_tag_fat("conditional clause types do not match: ",sizeof(char),40U);_T11=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T11;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1B;_T1B.tag=3;_T13=e2;
_T1B.f1=_T13->topt;_T12=_T1B;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1B=_T12;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=_tag_fat(" != ",sizeof(char),5U);_T14=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T14;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1D;_T1D.tag=3;_T16=e3;_T1D.f1=_T16->topt;_T15=_T1D;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1D=_T15;void*_T1E[4];_T1E[0]=& _T1A;_T1E[1]=& _T1B;_T1E[2]=& _T1C;_T1E[3]=& _T1D;_T17=loc;_T18=_tag_fat(_T1E,sizeof(void*),4);Cyc_Tcexp_err_and_explain(_T17,_T18);}}}}goto _TL198;_TL197: _TL198: _T19=t;
return _T19;}}}}}
# 889
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Tenv*_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;_T0=te;_T1=e1;_T2=
# 891
_tag_fat("logical-and expression",sizeof(char),23U);Cyc_Tcexp_tcTest(_T0,_T1,_T2);_T3=te;_T4=e2;_T5=
_tag_fat("logical-and expression",sizeof(char),23U);Cyc_Tcexp_tcTest(_T3,_T4,_T5);_T6=Cyc_Absyn_sint_type;
return _T6;}
# 895
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Tenv*_T3;struct Cyc_Absyn_Exp*_T4;struct _fat_ptr _T5;void*_T6;_T0=te;_T1=e1;_T2=
# 897
_tag_fat("logical-or expression",sizeof(char),22U);Cyc_Tcexp_tcTest(_T0,_T1,_T2);_T3=te;_T4=e2;_T5=
_tag_fat("logical-or expression",sizeof(char),22U);Cyc_Tcexp_tcTest(_T3,_T4,_T5);_T6=Cyc_Absyn_sint_type;
return _T6;}
# 903
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;struct Cyc_Tcenv_Tenv*_T5;void**_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Exp*_T8;void*_T9;int*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;unsigned _TD;struct _fat_ptr _TE;struct Cyc_Absyn_Kind*_TF;struct Cyc_Absyn_Kind*_T10;struct Cyc_Absyn_Kind*_T11;int _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;unsigned _T14;struct _fat_ptr _T15;int _T16;void*_T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;struct Cyc_Tcenv_Tenv*_T19;unsigned _T1A;void**_T1B;struct _fat_ptr _T1C;struct Cyc_Absyn_Exp*_T1D;unsigned _T1E;struct Cyc_List_List*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_List_List*_T22;struct Cyc_Absyn_Exp*_T23;void*_T24;int _T25;void*_T26;struct Cyc_Warn_String_Warn_Warg_struct _T27;struct Cyc_Warn_Typ_Warn_Warg_struct _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Typ_Warn_Warg_struct _T2A;struct Cyc_Tcenv_Tenv*_T2B;unsigned _T2C;void**_T2D;struct _fat_ptr _T2E;void*_T2F;void*_T30;struct Cyc_Tcenv_Tenv*_T31;unsigned _T32;struct Cyc_Core_Opt*_T33;void*_T34;enum Cyc_Absyn_Primop _T35;enum Cyc_Absyn_Primop*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Exp*_T38;int _T39;int _T3A;int _T3B;int _T3C;void*_T3D;int _T3E;int _T3F;int _T40;int _T41;void*_T42;struct Cyc_Warn_String_Warn_Warg_struct _T43;struct Cyc_Warn_String_Warn_Warg_struct _T44;struct Cyc_Warn_Typ_Warn_Warg_struct _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Warn_Typ_Warn_Warg_struct _T47;struct Cyc_Tcenv_Tenv*_T48;unsigned _T49;void**_T4A;struct _fat_ptr _T4B;void*_T4C;void*_T4D;_T0=
# 910
Cyc_Tcenv_enter_notreadctxt(te);_T1=Cyc_Tcenv_enter_lhs(_T0);_T2=e1;Cyc_Tcexp_tcExpNoPromote(_T1,0,_T2);_T3=e1;_T4=_T3->topt;{
void*t1=_check_null(_T4);_T5=te;_T6=& t1;_T7=e2;
Cyc_Tcexp_tcExp(_T5,_T6,_T7);_T8=e2;_T9=_T8->topt;{
void*t2=_check_null(_T9);{
# 915
void*_T4E=Cyc_Absyn_compress(t1);_TA=(int*)_T4E;_TB=*_TA;if(_TB!=5)goto _TL19A;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;
_T4F.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_TC=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_TC;void*_T50[1];_T50[0]=& _T4F;_TD=loc;_TE=_tag_fat(_T50,sizeof(void*),1);Cyc_Warn_err2(_TD,_TE);}goto _LL0;_TL19A: goto _LL0;_LL0:;}_TF=
# 920
Cyc_Tcutil_type_kind(t1);_T10=& Cyc_Kinds_mk;_T11=(struct Cyc_Absyn_Kind*)_T10;_T12=Cyc_Kinds_kind_leq(_TF,_T11);if(_T12)goto _TL19C;else{goto _TL19E;}
_TL19E:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("type is abstract (can't determine size)",sizeof(char),40U);_T13=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T13;void*_T4F[1];_T4F[0]=& _T4E;_T14=loc;_T15=_tag_fat(_T4F,sizeof(void*),1);Cyc_Warn_err2(_T14,_T15);}goto _TL19D;_TL19C: _TL19D: _T16=
# 924
Cyc_Absyn_is_lvalue(e1);if(_T16)goto _TL19F;else{goto _TL1A1;}
_TL1A1:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("assignment to non-lvalue",sizeof(char),25U);_T18=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T18;void*_T4F[1];_T4F[0]=& _T4E;_T19=te;_T1A=loc;_T1B=topt;_T1C=_tag_fat(_T4F,sizeof(void*),1);_T17=Cyc_Tcexp_expr_err(_T19,_T1A,_T1B,_T1C);}return _T17;_TL19F:
 Cyc_Tcexp_check_writable(te,e1);
if(po!=0)goto _TL1A2;_T1D=e2;_T1E=_T1D->loc;_T1F=
Cyc_Tcenv_curr_aquals_bounds(te);_T20=t2;_T21=e2;Cyc_Tcexp_check_consume(_T1E,_T1F,_T20,_T21);_T22=
Cyc_Tcenv_curr_aquals_bounds(te);_T23=e2;_T24=t1;_T25=Cyc_Tcutil_coerce_assign(_T22,_T23,_T24);if(_T25)goto _TL1A4;else{goto _TL1A6;}
_TL1A6:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_T27=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T27;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F;_T4F.tag=2;_T4F.f1=t1;_T28=_T4F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F=_T28;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;_T50.f1=_tag_fat(" != ",sizeof(char),5U);_T29=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_T29;{struct Cyc_Warn_Typ_Warn_Warg_struct _T51;_T51.tag=2;_T51.f1=t2;_T2A=_T51;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T51=_T2A;void*_T52[4];_T52[0]=& _T4E;_T52[1]=& _T4F;_T52[2]=& _T50;_T52[3]=& _T51;_T2B=te;_T2C=loc;_T2D=topt;_T2E=_tag_fat(_T52,sizeof(void*),4);_T26=Cyc_Tcexp_expr_err(_T2B,_T2C,_T2D,_T2E);}}}}{void*result=_T26;
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();_T2F=result;
return _T2F;}_TL1A4: _T30=t1;
# 935
return _T30;_TL1A2: {
# 937
struct Cyc_Absyn_Exp*e1copy=Cyc_Absyn_copy_exp(e1);_T31=te;_T32=loc;_T33=po;_T34=& _T33->v;_T35=(enum Cyc_Absyn_Primop)_T34;_T36=(enum Cyc_Absyn_Primop*)_T35;_T37=e1copy;_T38=e2;{
void*t_result=Cyc_Tcexp_tcBinPrimop(_T31,_T32,0,_T36,_T37,_T38);_T39=
Cyc_Unify_unify(t_result,t1);if(_T39)goto _TL1A7;else{goto _TL1A9;}_TL1A9: _T3A=Cyc_Tcutil_is_arithmetic_type(t_result);if(!_T3A)goto _TL1A7;_T3B=Cyc_Tcutil_is_arithmetic_type(t1);if(!_T3B)goto _TL1A7;_T3C=Cyc_Tcutil_is_tag_type(t1);if(_T3C)goto _TL1A7;else{goto _TL1AA;}
_TL1AA: _T3D=t1;return _T3D;_TL1A7: _T3E=
Cyc_Unify_unify(t_result,t1);if(_T3E)goto _TL1AB;else{goto _TL1AE;}_TL1AE: _T3F=Cyc_Tcutil_is_arithmetic_type(t_result);if(_T3F)goto _TL1B0;else{goto _TL1AD;}_TL1B0: _T40=Cyc_Tcutil_is_arithmetic_type(t1);if(_T40)goto _TL1AF;else{goto _TL1AD;}_TL1AF: _T41=Cyc_Tcutil_is_tag_type(t1);if(_T41)goto _TL1AD;else{goto _TL1AB;}
_TL1AD:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;
_T4E.f1=_tag_fat("Cannot use this operator in an assignment when ",sizeof(char),48U);_T43=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T43;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;
_T4F.f1=_tag_fat("the arguments have types ",sizeof(char),26U);_T44=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T44;{struct Cyc_Warn_Typ_Warn_Warg_struct _T50;_T50.tag=2;_T50.f1=t1;_T45=_T50;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T50=_T45;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;_T51.f1=_tag_fat(" and ",sizeof(char),6U);_T46=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T46;{struct Cyc_Warn_Typ_Warn_Warg_struct _T52;_T52.tag=2;_T52.f1=t2;_T47=_T52;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T52=_T47;void*_T53[5];_T53[0]=& _T4E;_T53[1]=& _T4F;_T53[2]=& _T50;_T53[3]=& _T51;_T53[4]=& _T52;_T48=te;_T49=loc;_T4A=topt;_T4B=_tag_fat(_T53,sizeof(void*),5);_T42=Cyc_Tcexp_expr_err(_T48,_T49,_T4A,_T4B);}}}}}{
# 942
void*result=_T42;
# 945
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();_T4C=result;
return _T4C;}_TL1AB: _T4D=t_result;
# 949
return _T4D;}}}}}
# 953
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;struct Cyc_Tcenv_Tenv*_T2;void**_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;void*_T7;_T0=
Cyc_Tcenv_clear_abstract_val_ok(te);_T1=e1;Cyc_Tcexp_tcExp(_T0,0,_T1);_T2=
Cyc_Tcenv_clear_abstract_val_ok(te);_T3=topt;_T4=e2;Cyc_Tcexp_tcExp(_T2,_T3,_T4);_T5=e2;_T6=_T5->topt;_T7=
_check_null(_T6);return _T7;}
# 962
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){struct Cyc_Absyn_Exp*_T0;void*_T1;int*_T2;int _T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*_TA;int _TB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC;void*_TD;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TE;enum Cyc_Absyn_Size_of _TF;int _T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_Absyn_Datatypefield*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple19*_T19;struct _tuple19 _T1A;void*_T1B;int _T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_Absyn_Datatypefield*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Absyn_Datatypefield*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;void*_T28;struct _tuple19*_T29;struct _tuple19 _T2A;void*_T2B;struct Cyc_Absyn_Exp*_T2C;unsigned _T2D;struct Cyc_Absyn_Exp*_T2E;void*_T2F;void*_T30;void*_T31;enum Cyc_Absyn_Coercion _T32;int _T33;enum Cyc_Absyn_Coercion _T34;int _T35;struct Cyc_List_List*_T36;void*_T37;struct Cyc_Absyn_Datatypefield*_T38;struct Cyc_List_List*_T39;struct Cyc_Warn_String_Warn_Warg_struct _T3A;struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_Typ_Warn_Warg_struct _T3D;struct Cyc_Absyn_Exp*_T3E;unsigned _T3F;struct _fat_ptr _T40;
# 965
struct Cyc_List_List*fields;_T0=e;_T1=_T0->topt;{
void*t1=_check_null(_T1);{
# 968
void*_T41=Cyc_Absyn_compress(t1);_T2=(int*)_T41;_T3=*_T2;if(_T3!=0)goto _TL1B1;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T41;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 2: _T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T41;_T9=_T8->f1;_TA=(struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_T9;_TB=_TA->f1;if(_TB!=0)goto _TL1B4;
# 970
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,1U);t1=Cyc_Absyn_double_type;goto _LL0;_TL1B4: goto _LL7;case 1: _TC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T41;_TD=_TC->f1;_TE=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TD;_TF=_TE->f2;_T10=(int)_TF;switch(_T10){case Cyc_Absyn_Char_sz: goto _LL6;case Cyc_Absyn_Short_sz: _LL6:
# 973
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,1U);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;};default: goto _LL7;}goto _TL1B2;_TL1B1: _LL7: goto _LL0;_TL1B2: _LL0:;}
# 977
fields=fs;_TL1BA: if(fields!=0)goto _TL1B8;else{goto _TL1B9;}
_TL1B8: _T11=fields;_T12=_T11->hd;_T13=(struct Cyc_Absyn_Datatypefield*)_T12;{struct Cyc_Absyn_Datatypefield _T41=*_T13;enum Cyc_Absyn_Scope _T42;unsigned _T43;struct Cyc_List_List*_T44;struct _tuple1*_T45;_T45=_T41.name;_T44=_T41.typs;_T43=_T41.loc;_T42=_T41.sc;{struct _tuple1*n=_T45;struct Cyc_List_List*typs=_T44;unsigned loc=_T43;enum Cyc_Absyn_Scope sc=_T42;
# 980
if(typs==0)goto _TL1BD;else{goto _TL1BE;}_TL1BE: _T14=typs;_T15=_T14->tl;if(_T15!=0)goto _TL1BD;else{goto _TL1BB;}_TL1BD: goto _TL1B7;_TL1BB: _T16=inst;_T17=typs;_T18=_T17->hd;_T19=(struct _tuple19*)_T18;_T1A=*_T19;_T1B=_T1A.f1;{
void*t2=Cyc_Tcutil_substitute(_T16,_T1B);_T1C=
# 983
Cyc_Unify_unify(t1,t2);if(!_T1C)goto _TL1BF;_T1D=fields;_T1E=_T1D->hd;_T1F=(struct Cyc_Absyn_Datatypefield*)_T1E;
return _T1F;_TL1BF:;}}}_TL1B7: _T20=fields;
# 977
fields=_T20->tl;goto _TL1BA;_TL1B9:
# 987
 fields=fs;_TL1C4: if(fields!=0)goto _TL1C2;else{goto _TL1C3;}
_TL1C2: _T21=fields;_T22=_T21->hd;_T23=(struct Cyc_Absyn_Datatypefield*)_T22;{struct Cyc_Absyn_Datatypefield _T41=*_T23;enum Cyc_Absyn_Scope _T42;unsigned _T43;struct Cyc_List_List*_T44;struct _tuple1*_T45;_T45=_T41.name;_T44=_T41.typs;_T43=_T41.loc;_T42=_T41.sc;{struct _tuple1*n=_T45;struct Cyc_List_List*typs=_T44;unsigned loc=_T43;enum Cyc_Absyn_Scope sc=_T42;
# 990
if(typs==0)goto _TL1C7;else{goto _TL1C8;}_TL1C8: _T24=typs;_T25=_T24->tl;if(_T25!=0)goto _TL1C7;else{goto _TL1C5;}_TL1C7: goto _TL1C1;_TL1C5: _T26=inst;_T27=typs;_T28=_T27->hd;_T29=(struct _tuple19*)_T28;_T2A=*_T29;_T2B=_T2A.f1;{
void*t2=Cyc_Tcutil_substitute(_T26,_T2B);
# 994
int bogus=0;_T2C=e;_T2D=_T2C->loc;_T2E=e;_T2F=_T2E->topt;_T30=
# 996
_check_null(_T2F);_T31=t2;{enum Cyc_Absyn_Coercion crc=Cyc_Subtype_coercible_warn(_T2D,_T30,_T31);_T32=crc;_T33=(int)_T32;
if(_T33==0)goto _TL1C9;_T34=crc;_T35=(int)_T34;
if(_T35==1)goto _TL1CB;
Cyc_Tcutil_unchecked_cast(e,t2,crc);goto _TL1CC;_TL1CB: _TL1CC: _T36=fields;_T37=_T36->hd;_T38=(struct Cyc_Absyn_Datatypefield*)_T37;
return _T38;_TL1C9:;}}}}_TL1C1: _T39=fields;
# 987
fields=_T39->tl;goto _TL1C4;_TL1C3:{struct Cyc_Warn_String_Warn_Warg_struct _T41;_T41.tag=0;
# 1003
_T41.f1=_tag_fat("can't find a field in ",sizeof(char),23U);_T3A=_T41;}{struct Cyc_Warn_String_Warn_Warg_struct _T41=_T3A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T42;_T42.tag=2;_T42.f1=tu;_T3B=_T42;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T42=_T3B;{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;_T43.f1=_tag_fat(" to inject value of type ",sizeof(char),26U);_T3C=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T3C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T44;_T44.tag=2;_T44.f1=t1;_T3D=_T44;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T44=_T3D;void*_T45[4];_T45[0]=& _T41;_T45[1]=& _T42;_T45[2]=& _T43;_T45[3]=& _T44;_T3E=e;_T3F=_T3E->loc;_T40=_tag_fat(_T45,sizeof(void*),4);Cyc_Warn_err2(_T3F,_T40);}}}}
return 0;}}
# 1007
static void Cyc_Tcexp_check_all_aqual_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*fn_or_aggr_qual_bnd){struct Cyc_List_List*_T0;void*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T7;void**_T8;void*_T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;int*_TC;unsigned _TD;void*_TE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;int*_T16;int _T17;void*_T18;struct Cyc_List_List*_T19;void*_T1A;void*_T1B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;void**_T20;struct Cyc_List_List*_T21;void*_T22;void*_T23;int _T24;void*_T25;struct Cyc_List_List*_T26;void*_T27;int _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Typ_Warn_Warg_struct _T2A;struct Cyc_List_List*_T2B;struct Cyc_Warn_String_Warn_Warg_struct _T2C;struct Cyc_Warn_Typ_Warn_Warg_struct _T2D;unsigned _T2E;struct _fat_ptr _T2F;void*_T30;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T31;struct Cyc_List_List*_T32;void*_T33;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T34;struct Cyc_List_List*_T35;struct Cyc_List_List*_T36;void*_T37;int*_T38;int _T39;struct Cyc_List_List*_T3A;void*_T3B;void*_T3C;int _T3D;struct Cyc_Warn_String_Warn_Warg_struct _T3E;struct Cyc_Warn_Typ_Warn_Warg_struct _T3F;struct Cyc_Warn_String_Warn_Warg_struct _T40;struct Cyc_Warn_Typ_Warn_Warg_struct _T41;unsigned _T42;struct _fat_ptr _T43;struct Cyc_List_List*_T44;void*_T45;void*_T46;int _T47;struct Cyc_Warn_String_Warn_Warg_struct _T48;struct Cyc_Warn_Typ_Warn_Warg_struct _T49;struct Cyc_Warn_String_Warn_Warg_struct _T4A;struct Cyc_Warn_Typ_Warn_Warg_struct _T4B;unsigned _T4C;struct _fat_ptr _T4D;struct Cyc_List_List*_T4E;
# 1011
struct Cyc_List_List*qbit=fn_or_aggr_qual_bnd;_TL1D0: if(qbit!=0)goto _TL1CE;else{goto _TL1CF;}
_TL1CE: _T0=qbit;_T1=_T0->hd;{struct _tuple0*_T4F=(struct _tuple0*)_T1;void*_T50;void*_T51;{struct _tuple0 _T52=*_T4F;_T51=_T52.f0;_T50=_T52.f1;}{void*qv=_T51;void*bnd=_T50;
void*qvc=Cyc_Absyn_compress(qv);struct Cyc_List_List*_T52;void*_T53;_T2=qvc;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 1: _T5=qvc;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T54=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T5;_T6=qvc;_T7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T6;_T8=& _T7->f2;_T53=(void**)_T8;}{void**ref=_T53;goto _LL3;}case 0: _T9=qvc;_TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9;_TB=_TA->f1;_TC=(int*)_TB;_TD=*_TC;switch(_TD){case 17: _TE=qvc;_TF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE;_T10=_TF->f2;if(_T10==0)goto _TL1D3;_T11=qvc;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T13=_T12->f2;_T14=(struct Cyc_List_List*)_T13;_T15=_T14->hd;_T16=(int*)_T15;_T17=*_T16;if(_T17!=1)goto _TL1D5;_T18=qvc;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T54=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_T19=_T54->f2;{struct Cyc_List_List _T55=*_T19;_T1A=_T55.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T56=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1A;_T1B=qvc;_T1C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B;_T1D=_T1C->f2;_T1E=_T1D->hd;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1E;_T20=& _T1F->f2;_T53=(void**)_T20;}_T52=_T55.tl;}}{void**ref=(void**)_T53;struct Cyc_List_List*ebnd=_T52;_T21=
# 1019
_check_null(ebnd);_T22=_T21->hd;_T23=bnd;_T24=Cyc_Subtype_check_aqual_bounds(0,_T22,_T23);if(_T24)goto _TL1D7;else{goto _TL1D9;}_TL1D9: _T25=bnd;_T26=ebnd;_T27=_T26->hd;_T28=
Cyc_Subtype_check_aqual_bounds(0,_T25,_T27);
# 1019
if(_T28)goto _TL1D7;else{goto _TL1DA;}
# 1021
_TL1DA:{struct Cyc_Warn_String_Warn_Warg_struct _T54;_T54.tag=0;_T54.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_T29=_T54;}{struct Cyc_Warn_String_Warn_Warg_struct _T54=_T29;{struct Cyc_Warn_Typ_Warn_Warg_struct _T55;_T55.tag=2;_T2B=ebnd;_T55.f1=_T2B->hd;_T2A=_T55;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T55=_T2A;{struct Cyc_Warn_String_Warn_Warg_struct _T56;_T56.tag=0;_T56.f1=_tag_fat(" <> ",sizeof(char),5U);_T2C=_T56;}{struct Cyc_Warn_String_Warn_Warg_struct _T56=_T2C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T57;_T57.tag=2;_T57.f1=bnd;_T2D=_T57;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T57=_T2D;void*_T58[4];_T58[0]=& _T54;_T58[1]=& _T55;_T58[2]=& _T56;_T58[3]=& _T57;_T2E=loc;_T2F=_tag_fat(_T58,sizeof(void*),4);Cyc_Warn_err2(_T2E,_T2F);}}}}goto _TL1D8;_TL1D7: _TL1D8: goto _LL3;}_TL1D5: goto _LLA;_TL1D3: goto _LLA;case 15: _T30=qvc;_T31=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T30;_T32=_T31->f2;if(_T32==0)goto _TL1DB;_T33=qvc;_T34=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T33;_T35=_T34->f2;_T36=(struct Cyc_List_List*)_T35;_T37=_T36->hd;_T38=(int*)_T37;_T39=*_T38;if(_T39!=1)goto _TL1DD;_T3A=
# 1025
Cyc_Tcenv_curr_aquals_bounds(te);_T3B=Cyc_Absyn_al_qual_type;_T3C=bnd;_T3D=Cyc_Subtype_check_aqual_bounds(_T3A,_T3B,_T3C);if(_T3D)goto _TL1DF;else{goto _TL1E1;}
_TL1E1:{struct Cyc_Warn_String_Warn_Warg_struct _T54;_T54.tag=0;_T54.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_T3E=_T54;}{struct Cyc_Warn_String_Warn_Warg_struct _T54=_T3E;{struct Cyc_Warn_Typ_Warn_Warg_struct _T55;_T55.tag=2;_T55.f1=qvc;_T3F=_T55;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T55=_T3F;{struct Cyc_Warn_String_Warn_Warg_struct _T56;_T56.tag=0;_T56.f1=_tag_fat(" <> ",sizeof(char),5U);_T40=_T56;}{struct Cyc_Warn_String_Warn_Warg_struct _T56=_T40;{struct Cyc_Warn_Typ_Warn_Warg_struct _T57;_T57.tag=2;_T57.f1=bnd;_T41=_T57;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T57=_T41;void*_T58[4];_T58[0]=& _T54;_T58[1]=& _T55;_T58[2]=& _T56;_T58[3]=& _T57;_T42=loc;_T43=_tag_fat(_T58,sizeof(void*),4);Cyc_Warn_err2(_T42,_T43);}}}}goto _TL1E0;_TL1DF: _TL1E0: goto _LL3;_TL1DD: goto _LLA;_TL1DB: goto _LLA;default: goto _LLA;};default: _LLA: _T44=
# 1029
Cyc_Tcenv_curr_aquals_bounds(te);_T45=qvc;_T46=bnd;_T47=Cyc_Subtype_check_aqual_bounds(_T44,_T45,_T46);if(_T47)goto _TL1E2;else{goto _TL1E4;}
_TL1E4:{struct Cyc_Warn_String_Warn_Warg_struct _T54;_T54.tag=0;_T54.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_T48=_T54;}{struct Cyc_Warn_String_Warn_Warg_struct _T54=_T48;{struct Cyc_Warn_Typ_Warn_Warg_struct _T55;_T55.tag=2;_T55.f1=qvc;_T49=_T55;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T55=_T49;{struct Cyc_Warn_String_Warn_Warg_struct _T56;_T56.tag=0;_T56.f1=_tag_fat(" <> ",sizeof(char),5U);_T4A=_T56;}{struct Cyc_Warn_String_Warn_Warg_struct _T56=_T4A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T57;_T57.tag=2;_T57.f1=bnd;_T4B=_T57;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T57=_T4B;void*_T58[4];_T58[0]=& _T54;_T58[1]=& _T55;_T58[2]=& _T56;_T58[3]=& _T57;_T4C=loc;_T4D=_tag_fat(_T58,sizeof(void*),4);Cyc_Warn_err2(_T4C,_T4D);}}}}goto _TL1E3;_TL1E2: _TL1E3: goto _LL3;}_LL3:;}}_T4E=qbit;
# 1011
qbit=_T4E->tl;goto _TL1D0;_TL1CF:;}char Cyc_Tcexp_Found_evar[11U]="Found_evar";struct Cyc_Tcexp_Found_evar_exn_struct{char*tag;};
# 1037
static struct Cyc_Tcexp_Found_evar_exn_struct Cyc_Tcexp_Found_evar_val={Cyc_Tcexp_Found_evar};
# 1039
static int Cyc_Tcexp_contains_free_evar(void*env,void*t){int*_T0;int _T1;struct Cyc_Tcexp_Found_evar_exn_struct*_T2;struct Cyc_Tcexp_Found_evar_exn_struct*_T3;{
void*_T4=Cyc_Absyn_compress(t);_T0=(int*)_T4;_T1=*_T0;if(_T1!=1)goto _TL1E5;_T2=& Cyc_Tcexp_Found_evar_val;_T3=(struct Cyc_Tcexp_Found_evar_exn_struct*)_T2;_throw(_T3);goto _TL1E6;_TL1E5: goto _LL0;_TL1E6: _LL0:;}
# 1046
return 1;}
# 1049
static struct Cyc_List_List*Cyc_Tcexp_fresh_effect_evars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*inst,struct Cyc_List_List*orig){int _T0;int _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr);void*(*_T3)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_List_List*_T6;unsigned _T7;struct Cyc_List_List*_T8;unsigned _T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Absyn_Kind*_TC;int(*_TD)(struct _fat_ptr,struct _fat_ptr);void*(*_TE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF;struct _fat_ptr _T10;void*_T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Absyn_Kind*_T13;struct Cyc_Absyn_Kind*_T14;int _T15;struct Cyc_List_List*_T16;void*_T17;void**_T18;void*_T19;int*_T1A;int _T1B;void**_T1C;struct Cyc_List_List*_T1D;struct Cyc_Core_Opt*_T1E;struct Cyc_Core_Opt*_T1F;struct Cyc_Core_Opt*_T20;struct _fat_ptr _T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;struct _handler_cons*_T25;int _T26;void(*_T27)(int(*)(int,void*),int,void*);void(*_T28)(int(*)(void*,void*),void*,void*);int(*_T29)(int,void*);int _T2A;void*_T2B;void*_T2C;struct Cyc_Tcexp_Found_evar_exn_struct*_T2D;char*_T2E;char*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;int*_T33;int _T34;void*_T35;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T36;void*_T37;int*_T38;int _T39;void*_T3A;struct Cyc_List_List*_T3B;unsigned _T3C;struct Cyc_List_List*_T3D;int*_T3E;int _T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;int(*_T43)(struct _fat_ptr,struct _fat_ptr);void*(*_T44)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T45;struct _fat_ptr _T46;struct Cyc_List_List*_T47;struct Cyc_Core_Opt*_T48;struct Cyc_Core_Opt*_T49;struct Cyc_Core_Opt*_T4A;void**_T4B;struct Cyc_List_List*_T4C;struct Cyc_List_List*_T4D;void*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;_T0=
# 1052
Cyc_List_length(inst);_T1=Cyc_List_length(orig);if(_T0==_T1)goto _TL1E7;_T3=Cyc_Warn_impos;{
int(*_T52)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3;_T2=_T52;}_T4=_tag_fat("injection orig and inst must have same length",sizeof(char),46U);_T5=_tag_fat(0U,sizeof(void*),0);_T2(_T4,_T5);goto _TL1E8;_TL1E7: _TL1E8: {
void*currrgn=Cyc_Tcenv_curr_rgn(te);
struct Cyc_List_List*fresh=0;
_TL1EC: _T6=inst;_T7=(unsigned)_T6;if(_T7)goto _TL1ED;else{goto _TL1EB;}_TL1ED: _T8=orig;_T9=(unsigned)_T8;if(_T9)goto _TL1EA;else{goto _TL1EB;}
_TL1EA: _TA=orig;_TB=_TA->hd;{struct _tuple14*_T52=(struct _tuple14*)_TB;void*_T53;struct Cyc_Absyn_Tvar*_T54;{struct _tuple14 _T55=*_T52;_T54=_T55.f0;_T53=_T55.f1;}{struct Cyc_Absyn_Tvar*otv=_T54;void*ot=_T53;
struct Cyc_Core_Opt _T55=Cyc_Kinds_tvar_kind_opt(otv);struct Cyc_Absyn_Kind*_T56;_TC=_T55.v;if(_TC!=0)goto _TL1EE;_TE=Cyc_Warn_impos;{
int(*_T57)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE;_TD=_T57;}_TF=_tag_fat("Tvar kind must be set by now",sizeof(char),29U);_T10=_tag_fat(0U,sizeof(void*),0);_TD(_TF,_T10);return 0;_TL1EE: _T11=_T55.v;_T56=(struct Cyc_Absyn_Kind*)_T11;{struct Cyc_Absyn_Kind*k=_T56;_T12=k;_T13=& Cyc_Kinds_ek;_T14=(struct Cyc_Absyn_Kind*)_T13;_T15=
# 1061
Cyc_Kinds_kind_eq(_T12,_T14);if(!_T15)goto _TL1F0;{
void*cot=Cyc_Tcutil_normalize_effect(ot);_T16=inst;_T17=_T16->hd;{
struct _tuple14*_T57=(struct _tuple14*)_T17;void*_T58;{struct _tuple14 _T59=*_T57;_T18=& _T57->f1;_T58=(void**)_T18;}{void**t=(void**)_T58;_T19=cot;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=1)goto _TL1F2;_T1C=t;{void*_T59[2];
# 1066
_T59[0]=currrgn;_T1E=& Cyc_Kinds_eko;_T1F=(struct Cyc_Core_Opt*)_T1E;_T20=Cyc_Tcenv_lookup_opt_type_vars(te);_T59[1]=Cyc_Absyn_new_evar(_T1F,_T20);_T21=_tag_fat(_T59,sizeof(void*),2);_T1D=Cyc_List_list(_T21);}*_T1C=Cyc_Absyn_join_eff(_T1D);{struct Cyc_List_List*_T59=_cycalloc(sizeof(struct Cyc_List_List));_T23=inst;_T24=_T23->hd;
_T59->hd=(struct _tuple14*)_T24;_T59->tl=fresh;_T22=(struct Cyc_List_List*)_T59;}fresh=_T22;goto _LLB;_TL1F2:{
# 1070
int dummy=0;{struct _handler_cons _T59;_T25=& _T59;_push_handler(_T25);{int _T5A=0;_T26=setjmp(_T59.handler);if(!_T26)goto _TL1F4;_T5A=1;goto _TL1F5;_TL1F4: _TL1F5: if(_T5A)goto _TL1F6;else{goto _TL1F8;}_TL1F8: _T28=Cyc_Absyn_visit_type;{
# 1072
void(*_T5B)(int(*)(int,void*),int,void*)=(void(*)(int(*)(int,void*),int,void*))_T28;_T27=_T5B;}{int(*_T5B)(int,void*)=(int(*)(int,void*))Cyc_Tcexp_contains_free_evar;_T29=_T5B;}_T2A=dummy;_T2B=cot;_T27(_T29,_T2A,_T2B);_pop_handler();goto _TL1F7;_TL1F6: _T2C=Cyc_Core_get_exn_thrown();{void*_T5B=(void*)_T2C;void*_T5C;_T2D=(struct Cyc_Tcexp_Found_evar_exn_struct*)_T5B;_T2E=_T2D->tag;_T2F=Cyc_Tcexp_Found_evar;if(_T2E!=_T2F)goto _TL1F9;{
# 1075
struct Cyc_List_List*inst_effect;inst_effect=_cycalloc(sizeof(struct Cyc_List_List));_T30=inst_effect;_T30->hd=currrgn;_T31=inst_effect;_T31->tl=0;{struct Cyc_List_List*_T5D;_T32=cot;_T33=(int*)_T32;_T34=*_T33;if(_T34!=0)goto _TL1FB;_T35=cot;_T36=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T35;_T37=_T36->f1;_T38=(int*)_T37;_T39=*_T38;if(_T39!=9)goto _TL1FD;_T3A=cot;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3A;_T5D=_T5E->f2;}{struct Cyc_List_List*ts=_T5D;
# 1078
_TL202: _T3B=ts;_T3C=(unsigned)_T3B;if(_T3C)goto _TL200;else{goto _TL201;}
_TL200: _T3D=ts;{void*_T5E=_T3D->hd;_T3E=(int*)_T5E;_T3F=*_T3E;if(_T3F!=1)goto _TL203;goto _LL1A;_TL203:{struct Cyc_List_List*_T5F=_cycalloc(sizeof(struct Cyc_List_List));_T41=ts;
# 1082
_T5F->hd=_T41->hd;_T5F->tl=inst_effect;_T40=(struct Cyc_List_List*)_T5F;}inst_effect=_T40;goto _LL1A;_LL1A:;}_T42=ts;
# 1078
ts=_T42->tl;goto _TL202;_TL201: goto _LL15;}_TL1FD: goto _LL18;_TL1FB: _LL18: _T44=Cyc_Warn_impos;{
# 1088
int(*_T5E)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T44;_T43=_T5E;}_T45=_tag_fat("!evar && Contains evar => JoinCon",sizeof(char),34U);_T46=_tag_fat(0U,sizeof(void*),0);_T43(_T45,_T46);_LL15:;}{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));_T48=& Cyc_Kinds_eko;_T49=(struct Cyc_Core_Opt*)_T48;_T4A=
# 1090
Cyc_Tcenv_lookup_opt_type_vars(te);_T5D->hd=Cyc_Absyn_new_evar(_T49,_T4A);
_T5D->tl=inst_effect;_T47=(struct Cyc_List_List*)_T5D;}
# 1090
inst_effect=_T47;_T4B=t;
# 1092
*_T4B=Cyc_Absyn_join_eff(inst_effect);{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));_T4D=inst;_T4E=_T4D->hd;
_T5D->hd=(struct _tuple14*)_T4E;_T5D->tl=fresh;_T4C=(struct Cyc_List_List*)_T5D;}fresh=_T4C;goto _LL10;}_TL1F9: _T5C=_T5B;{void*exn=_T5C;_rethrow(exn);}_LL10:;}_TL1F7:;}}goto _LLB;}_LLB: goto _LL3;}}}_TL1F0: goto _LL3;}_LL3:;}}_T4F=inst;
# 1056
inst=_T4F->tl;_T50=orig;orig=_T50->tl;goto _TL1EC;_TL1EB: _T51=fresh;
# 1103
return _T51;}}
# 1106
static int Cyc_Tcexp_inst_with_joineff(struct Cyc_List_List*orig,struct Cyc_Hashtable_Table*inst_effects){struct Cyc_List_List*_T0;void*_T1;struct _handler_cons*_T2;int _T3;struct Cyc_List_List*(*_T4)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*);void*(*_T5)(struct Cyc_Hashtable_Table*,void*);void*_T6;void*_T7;void*_T8;int _T9;void*_TA;struct Cyc_Core_Not_found_exn_struct*_TB;char*_TC;char*_TD;struct Cyc_List_List*_TE;int _TF;
# 1108
int found=0;
_TL208: if(orig!=0)goto _TL206;else{goto _TL207;}
_TL206: _T0=orig;_T1=_T0->hd;{struct _tuple14*_T10=(struct _tuple14*)_T1;void*_T11;struct Cyc_Absyn_Tvar*_T12;{struct _tuple14 _T13=*_T10;_T12=_T13.f0;_T11=_T13.f1;}{struct Cyc_Absyn_Tvar*tv=_T12;void*origrgn=_T11;struct _handler_cons _T13;_T2=& _T13;_push_handler(_T2);{int _T14=0;_T3=setjmp(_T13.handler);if(!_T3)goto _TL209;_T14=1;goto _TL20A;_TL209: _TL20A: if(_T14)goto _TL20B;else{goto _TL20D;}_TL20D: _T5=Cyc_Hashtable_lookup;{
# 1112
struct Cyc_List_List*(*_T15)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*))_T5;_T4=_T15;}{struct Cyc_List_List*l=_T4(inst_effects,tv);
found=1;_T6=origrgn;_T7=
Cyc_Absyn_join_eff(l);_T8=Cyc_Tcutil_normalize_effect(_T7);_T9=Cyc_Unify_unify(_T6,_T8);if(_T9)goto _TL20E;else{goto _TL210;}
_TL210:{int _T15=0;_npop_handler(0);return _T15;}_TL20E:;}_pop_handler();goto _TL20C;_TL20B: _TA=Cyc_Core_get_exn_thrown();{void*_T15=(void*)_TA;void*_T16;_TB=(struct Cyc_Core_Not_found_exn_struct*)_T15;_TC=_TB->tag;_TD=Cyc_Core_Not_found;if(_TC!=_TD)goto _TL211;goto _LL3;_TL211: _T16=_T15;{void*exn=_T16;_rethrow(exn);}_LL3:;}_TL20C:;}}}_TE=orig;
# 1109
orig=_TE->tl;goto _TL208;_TL207: _TF=found;
# 1121
return _TF;}struct _tuple20{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Exp*f1;};
# 1124
static int Cyc_Tcexp_substitute_vars_exp(struct Cyc_List_List*subst,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;void*_T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple20*_TC;struct Cyc_Absyn_Vardecl*_TD;struct _tuple1*_TE;struct Cyc_Absyn_Vardecl*_TF;struct _tuple1*_T10;int _T11;struct Cyc_Absyn_Exp*_T12;struct Cyc_Absyn_Exp*_T13;struct Cyc_List_List*_T14;_T0=e;{
void*_T15=_T0->r;struct Cyc_Absyn_Vardecl*_T16;_T1=(int*)_T15;_T2=*_T1;if(_T2!=1)goto _TL213;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T7=_T17->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T18=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T7;_T16=_T18->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T16;_T16=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T8=_T17->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T18=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T8;_T16=_T18->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T16;_T16=vd;goto _LL6;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T9=_T17->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T18=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T9;_T16=_T18->f1;}}_LL6: {struct Cyc_Absyn_Vardecl*vd=_T16;{
# 1129
struct Cyc_List_List*s=subst;_TL219: if(s!=0)goto _TL217;else{goto _TL218;}
_TL217: _TA=s;_TB=_TA->hd;_TC=(struct _tuple20*)_TB;{struct _tuple20 _T17=*_TC;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Vardecl*_T19;_T19=_T17.f0;_T18=_T17.f1;{struct Cyc_Absyn_Vardecl*vdopt=_T19;struct Cyc_Absyn_Exp*e2=_T18;
if(vdopt!=0)goto _TL21A;goto _TL216;_TL21A: _TD=vdopt;_TE=_TD->name;_TF=vd;_T10=_TF->name;_T11=
Cyc_Absyn_qvar_cmp(_TE,_T10);if(_T11!=0)goto _TL21C;_T12=e;_T13=e2;
_T12->r=_T13->r;
return 0;_TL21C:;}}_TL216: _T14=s;
# 1129
s=_T14->tl;goto _TL219;_TL218:;}goto _LL0;}default: goto _LL7;}goto _TL214;_TL213: _LL7: goto _LL0;_TL214: _LL0:;}
# 1140
return 1;}
# 1143
static int Cyc_Tcexp_substitute_vars_stmt(struct Cyc_List_List*subst,struct Cyc_Absyn_Stmt*e){
return 1;}struct _tuple21{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1149
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*call,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;struct Cyc_Absyn_PtrInfo _TD;struct Cyc_Absyn_PtrAtts _TE;struct Cyc_Absyn_PtrInfo _TF;struct Cyc_Absyn_PtrAtts _T10;void*_T11;void*_T12;void*_T13;int*_T14;int _T15;void*_T16;struct Cyc_Absyn_FnInfo _T17;struct Cyc_Absyn_FnInfo _T18;struct Cyc_Absyn_FnInfo _T19;struct Cyc_Absyn_FnInfo _T1A;struct Cyc_Absyn_FnInfo _T1B;struct Cyc_Absyn_FnInfo _T1C;struct Cyc_Absyn_FnInfo _T1D;struct Cyc_Absyn_FnInfo _T1E;struct Cyc_Absyn_FnInfo _T1F;struct Cyc_Absyn_FnInfo _T20;struct Cyc_Absyn_FnInfo _T21;struct Cyc_Absyn_FnInfo _T22;struct Cyc_Absyn_FnInfo _T23;struct Cyc_Absyn_FnInfo _T24;struct Cyc_Absyn_FnInfo _T25;struct Cyc_Absyn_FnInfo _T26;struct Cyc_Absyn_FnInfo _T27;struct Cyc_Absyn_FnInfo _T28;struct Cyc_Absyn_FnInfo _T29;struct Cyc_Absyn_FnInfo _T2A;struct Cyc_Absyn_Exp*_T2B;unsigned _T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;void*_T2F;void**_T30;void*_T31;struct Cyc_List_List*_T32;void*_T33;struct Cyc_List_List*_T34;void*_T35;struct _tuple9*_T36;struct _tuple9 _T37;struct Cyc_Tcenv_Tenv*_T38;void**_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Exp*_T3C;void*_T3D;int*_T3E;int _T3F;struct Cyc_Absyn_Exp*_T40;void*_T41;void*_T42;unsigned long _T43;unsigned long _T44;unsigned long _T45;unsigned long _T46;unsigned long _T47;unsigned long _T48;unsigned long _T49;struct Cyc_Warn_String_Warn_Warg_struct _T4A;struct Cyc_Warn_String_Warn_Warg_struct _T4B;struct Cyc_Warn_String_Warn_Warg_struct _T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;struct Cyc_Warn_String_Warn_Warg_struct _T4E;struct Cyc_Warn_String_Warn_Warg_struct _T4F;struct Cyc_Warn_String_Warn_Warg_struct _T50;struct Cyc_Absyn_Exp*_T51;unsigned _T52;struct _fat_ptr _T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;struct Cyc_Warn_String_Warn_Warg_struct _T55;struct Cyc_Warn_String_Warn_Warg_struct _T56;struct Cyc_Warn_String_Warn_Warg_struct _T57;struct Cyc_Absyn_Exp*_T58;unsigned _T59;struct _fat_ptr _T5A;struct Cyc_Absyn_Exp*_T5B;void*_T5C;void*_T5D;void*_T5E;int _T5F;struct Cyc_List_List**_T60;struct Cyc_List_List*_T61;int _T62;struct Cyc_List_List**_T63;struct Cyc_Absyn_Exp*_T64;unsigned _T65;struct Cyc_List_List*_T66;void*_T67;struct Cyc_Absyn_Exp*_T68;struct Cyc_List_List*_T69;struct Cyc_List_List*_T6A;struct Cyc_List_List*_T6B;int*_T6C;int _T6D;struct _handler_cons*_T6E;int _T6F;struct Cyc_Absyn_Exp*(*_T70)(struct Cyc_List_List*,int);void*(*_T71)(struct Cyc_List_List*,int);struct Cyc_List_List*_T72;int _T73;struct Cyc_Core_Opt*_T74;struct Cyc_List_List*_T75;int _T76;enum Cyc_Absyn_Format_Type _T77;int _T78;struct Cyc_Tcenv_Tenv*_T79;struct Cyc_Absyn_Exp*_T7A;struct Cyc_Core_Opt*_T7B;int _T7C;struct Cyc_List_List**_T7D;int _T7E;struct Cyc_List_List*(*_T7F)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);struct Cyc_Tcenv_Tenv*_T80;struct Cyc_Absyn_Exp*_T81;struct Cyc_Core_Opt*_T82;int _T83;struct Cyc_List_List**_T84;int _T85;struct Cyc_List_List*(*_T86)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);void*_T87;struct Cyc_List_Nth_exn_struct*_T88;char*_T89;char*_T8A;struct Cyc_Warn_String_Warn_Warg_struct _T8B;unsigned _T8C;struct _fat_ptr _T8D;struct Cyc_List_List*_T8E;struct Cyc_Warn_String_Warn_Warg_struct _T8F;struct Cyc_Warn_Exp_Warn_Warg_struct _T90;unsigned _T91;struct _fat_ptr _T92;int _T93;int _T94;struct Cyc_Tcenv_Tenv*_T95;struct Cyc_List_List*_T96;void*_T97;struct Cyc_Absyn_Exp*_T98;struct Cyc_List_List*_T99;struct Cyc_Warn_String_Warn_Warg_struct _T9A;struct Cyc_Warn_Exp_Warn_Warg_struct _T9B;unsigned _T9C;struct _fat_ptr _T9D;struct Cyc_Absyn_VarargInfo*_T9E;struct Cyc_Absyn_VarargCallInfo*_T9F;struct Cyc_Absyn_VarargCallInfo*_TA0;struct Cyc_Absyn_VarargCallInfo*_TA1;struct Cyc_Absyn_VarargCallInfo**_TA2;int _TA3;struct Cyc_List_List*_TA4;void*_TA5;struct Cyc_Absyn_VarargCallInfo*_TA6;struct Cyc_Tcenv_Tenv*_TA7;void**_TA8;struct Cyc_Absyn_Exp*_TA9;struct Cyc_List_List*_TAA;struct Cyc_Absyn_Exp*_TAB;void*_TAC;int*_TAD;int _TAE;struct Cyc_Warn_String_Warn_Warg_struct _TAF;struct Cyc_Warn_Typ_Warn_Warg_struct _TB0;struct Cyc_Warn_String_Warn_Warg_struct _TB1;struct Cyc_Warn_TypOpt_Warn_Warg_struct _TB2;struct Cyc_Absyn_Exp*_TB3;unsigned _TB4;struct _fat_ptr _TB5;int _TB6;struct Cyc_List_List**_TB7;struct Cyc_List_List*_TB8;int _TB9;struct Cyc_List_List**_TBA;struct Cyc_Absyn_Exp*_TBB;unsigned _TBC;struct Cyc_List_List*_TBD;void*_TBE;struct Cyc_Absyn_Exp*_TBF;struct Cyc_List_List*_TC0;void*_TC1;int*_TC2;int _TC3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC4;void*_TC5;int*_TC6;int _TC7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC8;void*_TC9;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TCA;union Cyc_Absyn_DatatypeInfo _TCB;struct _union_DatatypeInfo_KnownDatatype _TCC;unsigned _TCD;void*_TCE;union Cyc_Absyn_DatatypeInfo _TCF;struct _union_DatatypeInfo_KnownDatatype _TD0;struct Cyc_Absyn_Datatypedecl**_TD1;struct Cyc_Tcenv_Tenv*_TD2;unsigned _TD3;struct Cyc_Absyn_Datatypedecl*_TD4;struct _tuple1*_TD5;struct Cyc_Absyn_Datatypedecl**_TD6;struct Cyc_Absyn_Datatypedecl*_TD7;struct Cyc_Core_Opt*_TD8;struct Cyc_Warn_String_Warn_Warg_struct _TD9;struct Cyc_Warn_Typ_Warn_Warg_struct _TDA;unsigned _TDB;struct _fat_ptr _TDC;struct Cyc_Absyn_Datatypedecl*_TDD;struct Cyc_Core_Opt*_TDE;void*_TDF;struct Cyc_Absyn_Datatypedecl*_TE0;struct Cyc_List_List*_TE1;struct Cyc_List_List*_TE2;struct Cyc_Absyn_Datatypedecl*_TE3;struct Cyc_List_List*_TE4;struct Cyc_List_List*_TE5;struct Cyc_Hashtable_Table*(*_TE6)(int,int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),int(*)(struct Cyc_Absyn_Tvar*));struct Cyc_Hashtable_Table*(*_TE7)(int,int(*)(void*,void*),int(*)(void*));int(*_TE8)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);int(*_TE9)(struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_VarargCallInfo*_TEA;struct Cyc_List_List*_TEB;void*_TEC;int _TED;struct Cyc_Absyn_Exp*_TEE;unsigned _TEF;struct Cyc_List_List*_TF0;struct Cyc_Absyn_Exp*_TF1;void*_TF2;void*_TF3;struct Cyc_Absyn_Exp*_TF4;struct Cyc_Tcenv_Tenv*_TF5;struct Cyc_Absyn_Exp*_TF6;void*_TF7;struct Cyc_List_List*_TF8;struct Cyc_List_List*_TF9;struct Cyc_Absyn_VarargCallInfo*_TFA;struct Cyc_Absyn_VarargCallInfo*_TFB;struct Cyc_List_List*_TFC;struct Cyc_List_List*_TFD;struct Cyc_List_List*_TFE;void*_TFF;void*_T100;int*_T101;int _T102;struct _handler_cons*_T103;int _T104;struct Cyc_List_List*(*_T105)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*);void*(*_T106)(struct Cyc_Hashtable_Table*,void*);void*_T107;struct Cyc_Core_Not_found_exn_struct*_T108;char*_T109;char*_T10A;void(*_T10B)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*);void(*_T10C)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T10D;struct Cyc_Absyn_Tvar*_T10E;struct Cyc_List_List*_T10F;struct Cyc_List_List*_T110;struct Cyc_List_List*_T111;int _T112;int _T113;struct Cyc_Absyn_VarargCallInfo*_T114;int _T115;int _T116;struct Cyc_List_List*_T117;unsigned _T118;struct Cyc_List_List*_T119;unsigned _T11A;struct Cyc_List_List*_T11B;void*_T11C;struct Cyc_Tcenv_Tenv*_T11D;struct Cyc_Absyn_Exp*_T11E;void*_T11F;struct Cyc_List_List*_T120;struct Cyc_List_List*_T121;struct Cyc_Absyn_Datatypefield*_T122;struct Cyc_List_List*_T123;void*_T124;struct Cyc_Absyn_Datatypefield*_T125;struct Cyc_List_List*_T126;struct Cyc_List_List*_T127;struct Cyc_Warn_String_Warn_Warg_struct _T128;unsigned _T129;struct _fat_ptr _T12A;struct Cyc_List_List**_T12B;struct Cyc_List_List*_T12C;struct Cyc_Tcenv_Tenv*_T12D;unsigned _T12E;void*_T12F;struct Cyc_List_List*_T130;void*_T131;struct Cyc_List_List*_T132;struct _tuple19*_T133;struct _fat_ptr*_T134;struct _fat_ptr _T135;struct Cyc_Int_pa_PrintArg_struct _T136;int _T137;struct _fat_ptr _T138;struct _fat_ptr _T139;struct _tuple1*_T13A;struct _tuple1*_T13B;struct Cyc_List_List*_T13C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T13D;void*_T13E;unsigned _T13F;struct Cyc_Absyn_Exp*_T140;struct Cyc_List_List*_T141;struct Cyc_List_List*_T142;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T143;struct Cyc_List_List*_T144;void*_T145;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T146;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T147;void*_T148;unsigned _T149;void*_T14A;unsigned _T14B;struct Cyc_Absyn_Pat*_T14C;struct Cyc_List_List*_T14D;void*_T14E;struct Cyc_Absyn_Vardecl*_T14F;struct Cyc_List_List*_T150;struct _tuple21*_T151;struct Cyc_List_List*_T152;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T153;void*_T154;unsigned _T155;struct Cyc_Absyn_Pat*_T156;struct Cyc_Absyn_Exp*_T157;struct Cyc_List_List*_T158;struct _tuple20*_T159;struct Cyc_List_List*_T15A;void*_T15B;struct Cyc_List_List*_T15C;void*_T15D;struct Cyc_List_List*_T15E;struct Cyc_List_List*_T15F;void(*_T160)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Exp*);void(*_T161)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*_T162;struct Cyc_Absyn_Exp*_T163;struct Cyc_Absyn_Exp*_T164;struct Cyc_Absyn_Exp*_T165;unsigned _T166;struct _tuple1*_T167;struct _fat_ptr*_T168;void*_T169;struct Cyc_Absyn_Exp*_T16A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T16B;void*_T16C;unsigned _T16D;struct Cyc_Absyn_Exp*_T16E;struct Cyc_Tcenv_Tenv*_T16F;struct Cyc_Absyn_Pat*_T170;void**_T171;unsigned _T172;struct Cyc_Tcenv_Tenv*_T173;struct Cyc_Absyn_Pat*_T174;void**_T175;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T176;struct Cyc_Core_Opt*_T177;struct Cyc_Tcpat_TcPatResult _T178;struct Cyc_Absyn_Decl*_T179;struct Cyc_Absyn_Stmt*_T17A;struct Cyc_Absyn_Stmt*_T17B;unsigned _T17C;struct Cyc_Absyn_Stmt*_T17D;unsigned _T17E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T17F;void*_T180;unsigned _T181;struct Cyc_Absyn_Decl*_T182;struct Cyc_Absyn_Stmt*_T183;unsigned _T184;struct Cyc_Absyn_Exp*_T185;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T186;void*_T187;void*_T188;struct Cyc_Warn_String_Warn_Warg_struct _T189;struct Cyc_Warn_Typ_Warn_Warg_struct _T18A;struct Cyc_Tcenv_Tenv*_T18B;unsigned _T18C;void**_T18D;struct _fat_ptr _T18E;void*_T18F;struct Cyc_Warn_String_Warn_Warg_struct _T190;struct Cyc_Warn_Typ_Warn_Warg_struct _T191;struct Cyc_Tcenv_Tenv*_T192;unsigned _T193;void**_T194;struct _fat_ptr _T195;
# 1157
struct Cyc_List_List*es=args;
int arg_cnt=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_T196=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_T196;
Cyc_Tcexp_tcExp(te,0,e);_T0=e;_T1=_T0->topt;_T2=
_check_null(_T1);{void*t=Cyc_Absyn_compress(_T2);void*_T197;void*_T198;void*_T199;void*_T19A;struct Cyc_Absyn_Tqual _T19B;void*_T19C;_T3=t;_T4=(int*)_T3;_T5=*_T4;if(_T5!=4)goto _TL21E;_T6=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T19D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6;_T7=_T19D->f1;_T19C=_T7.elt_type;_T8=_T19D->f1;_T19B=_T8.elt_tq;_T9=_T19D->f1;_TA=_T9.ptr_atts;_T19A=_TA.eff;_TB=_T19D->f1;_TC=_TB.ptr_atts;_T199=_TC.nullable;_TD=_T19D->f1;_TE=_TD.ptr_atts;_T198=_TE.bounds;_TF=_T19D->f1;_T10=_TF.ptr_atts;_T197=_T10.zero_term;}{void*t1=_T19C;struct Cyc_Absyn_Tqual tq=_T19B;void*rgn=_T19A;void*x=_T199;void*b=_T198;void*zt=_T197;
# 1171
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);
# 1173
Cyc_Tcutil_check_nonzero_bound(loc,b);_T11=
# 1176
Cyc_Absyn_compress(t1);_T12=Cyc_Tcenv_curr_lifo_rgn(te);t1=Cyc_CurRgn_instantiate(_T11,_T12);{struct Cyc_List_List*_T19D;struct Cyc_List_List*_T19E;struct Cyc_Absyn_Vardecl*_T19F;struct Cyc_AssnDef_ExistAssnFn*_T1A0;struct Cyc_Absyn_Exp*_T1A1;struct Cyc_AssnDef_ExistAssnFn*_T1A2;struct Cyc_Absyn_Exp*_T1A3;struct Cyc_AssnDef_ExistAssnFn*_T1A4;struct Cyc_Absyn_Exp*_T1A5;struct Cyc_AssnDef_ExistAssnFn*_T1A6;struct Cyc_Absyn_Exp*_T1A7;struct Cyc_List_List*_T1A8;struct Cyc_List_List*_T1A9;struct Cyc_Absyn_VarargInfo*_T1AA;int _T1AB;struct Cyc_List_List*_T1AC;void*_T1AD;struct Cyc_Absyn_Tqual _T1AE;void*_T1AF;struct Cyc_List_List*_T1B0;_T13=t1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=6)goto _TL220;_T16=t1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1B1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T16;_T17=_T1B1->f1;_T1B0=_T17.tvars;_T18=_T1B1->f1;_T1AF=_T18.effect;_T19=_T1B1->f1;_T1AE=_T19.ret_tqual;_T1A=_T1B1->f1;_T1AD=_T1A.ret_type;_T1B=_T1B1->f1;_T1AC=_T1B.args;_T1C=_T1B1->f1;_T1AB=_T1C.c_varargs;_T1D=_T1B1->f1;_T1AA=_T1D.cyc_varargs;_T1E=_T1B1->f1;_T1A9=_T1E.qual_bnd;_T1F=_T1B1->f1;_T1A8=_T1F.attributes;_T20=_T1B1->f1;_T1A7=_T20.checks_clause;_T21=_T1B1->f1;_T1A6=_T21.checks_assn;_T22=_T1B1->f1;_T1A5=_T22.requires_clause;_T23=_T1B1->f1;_T1A4=_T23.requires_assn;_T24=_T1B1->f1;_T1A3=_T24.ensures_clause;_T25=_T1B1->f1;_T1A2=_T25.ensures_assn;_T26=_T1B1->f1;_T1A1=_T26.throws_clause;_T27=_T1B1->f1;_T1A0=_T27.throws_assn;_T28=_T1B1->f1;_T19F=_T28.return_value;_T29=_T1B1->f1;_T19E=_T29.arg_vardecls;_T2A=_T1B1->f1;_T19D=_T2A.effconstr;}{struct Cyc_List_List*tvars=_T1B0;void*eff=_T1AF;struct Cyc_Absyn_Tqual res_tq=_T1AE;void*res_typ=_T1AD;struct Cyc_List_List*orig_args_info=_T1AC;int c_vararg=_T1AB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T1AA;struct Cyc_List_List*qb=_T1A9;struct Cyc_List_List*atts=_T1A8;struct Cyc_Absyn_Exp*chk=_T1A7;struct Cyc_AssnDef_ExistAssnFn*chk_relns=_T1A6;struct Cyc_Absyn_Exp*req=_T1A5;struct Cyc_AssnDef_ExistAssnFn*req_relns=_T1A4;struct Cyc_Absyn_Exp*ens=_T1A3;struct Cyc_AssnDef_ExistAssnFn*ens_relns=_T1A2;struct Cyc_Absyn_Exp*thrws=_T1A1;struct Cyc_AssnDef_ExistAssnFn*thrws_relns=_T1A0;struct Cyc_Absyn_Vardecl*ret_var=_T19F;struct Cyc_List_List*arg_vardecls=_T19E;struct Cyc_List_List*effc=_T19D;
# 1180
if(tvars!=0)goto _TL224;else{goto _TL225;}_TL225: if(effc!=0)goto _TL224;else{goto _TL222;}
_TL224: _T2B=e;_T2C=_T2B->loc;_T2D=_tag_fat("function should have been instantiated prior to use -- probably a compiler bug",sizeof(char),79U);_T2E=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2C,_T2D,_T2E);goto _TL223;_TL222: _TL223: {
struct Cyc_List_List*args_info=orig_args_info;
# 1185
if(topt==0)goto _TL226;_T2F=res_typ;_T30=topt;_T31=*_T30;Cyc_Unify_unify(_T2F,_T31);goto _TL227;_TL226: _TL227:
# 1187
 _TL228: if(es!=0)goto _TL22B;else{goto _TL22A;}_TL22B: if(args_info!=0)goto _TL229;else{goto _TL22A;}
# 1189
_TL229:{int alias_coercion=0;_T32=es;_T33=_T32->hd;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_T33;_T34=args_info;_T35=_T34->hd;_T36=(struct _tuple9*)_T35;_T37=*_T36;{
void*t2=_T37.f2;_T38=te;_T39=& t2;_T3A=e1;
Cyc_Tcexp_tcExp(_T38,_T39,_T3A);_T3B=
Cyc_Tcenv_curr_aquals_bounds(te);_T3C=e1;_T3D=t2;_T3E=& alias_coercion;_T3F=Cyc_Tcutil_coerce_arg(_T3B,_T3C,_T3D,_T3E);if(_T3F)goto _TL22C;else{goto _TL22E;}
_TL22E:{struct _fat_ptr s0=_tag_fat("actual argument has type ",sizeof(char),26U);_T40=e1;_T41=_T40->topt;_T42=
_check_null(_T41);{struct _fat_ptr s1=Cyc_Absynpp_typ2string(_T42);
struct _fat_ptr s2=_tag_fat(" but formal has type ",sizeof(char),22U);
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);_T43=
Cyc_strlen(s0);_T44=Cyc_strlen(s1);_T45=_T43 + _T44;_T46=Cyc_strlen(s2);_T47=_T45 + _T46;_T48=Cyc_strlen(s3);_T49=_T47 + _T48;if(_T49 < 70U)goto _TL22F;{struct Cyc_Warn_String_Warn_Warg_struct _T1B1;_T1B1.tag=0;
_T1B1.f1=s0;_T4A=_T1B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B1=_T4A;{struct Cyc_Warn_String_Warn_Warg_struct _T1B2;_T1B2.tag=0;_T1B2.f1=_tag_fat("\n\t",sizeof(char),3U);_T4B=_T1B2;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B2=_T4B;{struct Cyc_Warn_String_Warn_Warg_struct _T1B3;_T1B3.tag=0;_T1B3.f1=s1;_T4C=_T1B3;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B3=_T4C;{struct Cyc_Warn_String_Warn_Warg_struct _T1B4;_T1B4.tag=0;_T1B4.f1=_tag_fat("\n",sizeof(char),2U);_T4D=_T1B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B4=_T4D;{struct Cyc_Warn_String_Warn_Warg_struct _T1B5;_T1B5.tag=0;_T1B5.f1=s2;_T4E=_T1B5;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B5=_T4E;{struct Cyc_Warn_String_Warn_Warg_struct _T1B6;_T1B6.tag=0;_T1B6.f1=_tag_fat("\n\t",sizeof(char),3U);_T4F=_T1B6;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B6=_T4F;{struct Cyc_Warn_String_Warn_Warg_struct _T1B7;_T1B7.tag=0;_T1B7.f1=s3;_T50=_T1B7;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B7=_T50;void*_T1B8[7];_T1B8[0]=& _T1B1;_T1B8[1]=& _T1B2;_T1B8[2]=& _T1B3;_T1B8[3]=& _T1B4;_T1B8[4]=& _T1B5;_T1B8[5]=& _T1B6;_T1B8[6]=& _T1B7;_T51=e1;_T52=_T51->loc;_T53=_tag_fat(_T1B8,sizeof(void*),7);Cyc_Warn_err2(_T52,_T53);}}}}}}}goto _TL230;
# 1201
_TL22F:{struct Cyc_Warn_String_Warn_Warg_struct _T1B1;_T1B1.tag=0;_T1B1.f1=s0;_T54=_T1B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B1=_T54;{struct Cyc_Warn_String_Warn_Warg_struct _T1B2;_T1B2.tag=0;_T1B2.f1=s1;_T55=_T1B2;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B2=_T55;{struct Cyc_Warn_String_Warn_Warg_struct _T1B3;_T1B3.tag=0;_T1B3.f1=s2;_T56=_T1B3;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B3=_T56;{struct Cyc_Warn_String_Warn_Warg_struct _T1B4;_T1B4.tag=0;_T1B4.f1=s3;_T57=_T1B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B4=_T57;void*_T1B5[4];_T1B5[0]=& _T1B1;_T1B5[1]=& _T1B2;_T1B5[2]=& _T1B3;_T1B5[3]=& _T1B4;_T58=e1;_T59=_T58->loc;_T5A=_tag_fat(_T1B5,sizeof(void*),4);Cyc_Warn_err2(_T59,_T5A);}}}}_TL230: _T5B=e1;_T5C=_T5B->topt;_T5D=
_check_null(_T5C);_T5E=t2;Cyc_Unify_unify(_T5D,_T5E);
Cyc_Unify_explain_failure();}}goto _TL22D;_TL22C: _TL22D: _T5F=alias_coercion;
# 1206
if(!_T5F)goto _TL231;_T60=alias_arg_exps;{struct Cyc_List_List*_T1B1=_cycalloc(sizeof(struct Cyc_List_List));_T62=arg_cnt;
_T1B1->hd=(void*)_T62;_T63=alias_arg_exps;_T1B1->tl=*_T63;_T61=(struct Cyc_List_List*)_T1B1;}*_T60=_T61;goto _TL232;_TL231: _TL232: _T64=e1;_T65=_T64->loc;_T66=
Cyc_Tcenv_curr_aquals_bounds(te);_T67=t2;_T68=e1;Cyc_Tcexp_check_consume(_T65,_T66,_T67,_T68);_T69=es;
es=_T69->tl;_T6A=args_info;
args_info=_T6A->tl;
arg_cnt=arg_cnt + 1;}}}goto _TL228;_TL22A: {
# 1216
int args_already_checked=0;{
struct Cyc_List_List*a=atts;_TL236: if(a!=0)goto _TL234;else{goto _TL235;}
_TL234: _T6B=a;{void*_T1B1=_T6B->hd;int _T1B2;int _T1B3;enum Cyc_Absyn_Format_Type _T1B4;_T6C=(int*)_T1B1;_T6D=*_T6C;if(_T6D!=19)goto _TL237;{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_T1B5=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_T1B1;_T1B4=_T1B5->f1;_T1B3=_T1B5->f2;_T1B2=_T1B5->f3;}{enum Cyc_Absyn_Format_Type ft=_T1B4;int fmt_arg_pos=_T1B3;int arg_start_pos=_T1B2;{struct _handler_cons _T1B5;_T6E=& _T1B5;_push_handler(_T6E);{int _T1B6=0;_T6F=setjmp(_T1B5.handler);if(!_T6F)goto _TL239;_T1B6=1;goto _TL23A;_TL239: _TL23A: if(_T1B6)goto _TL23B;else{goto _TL23D;}_TL23D: _T71=Cyc_List_nth;{
# 1222
struct Cyc_Absyn_Exp*(*_T1B7)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))_T71;_T70=_T1B7;}_T72=args;_T73=fmt_arg_pos - 1;{struct Cyc_Absyn_Exp*fmt_arg=_T70(_T72,_T73);
# 1224
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos!=0)goto _TL23E;
fmt_args=0;goto _TL23F;
# 1228
_TL23E:{struct Cyc_Core_Opt*_T1B7=_cycalloc(sizeof(struct Cyc_Core_Opt));_T75=args;_T76=arg_start_pos - 1;_T1B7->v=Cyc_List_nth_tail(_T75,_T76);_T74=(struct Cyc_Core_Opt*)_T1B7;}fmt_args=_T74;_TL23F:
 args_already_checked=1;_T77=ft;_T78=(int)_T77;switch(_T78){case Cyc_Absyn_Printf_ft: _T79=te;_T7A=fmt_arg;_T7B=fmt_args;_T7C=arg_start_pos - 1;_T7D=alias_arg_exps;_T7E=c_vararg;_T7F=Cyc_Formatstr_get_format_types;
# 1232
Cyc_Tcexp_check_format_args(_T79,_T7A,_T7B,_T7C,_T7D,_T7E,_T7F);goto _LLF;case Cyc_Absyn_Scanf_ft: goto _LL15;default: _LL15: _T80=te;_T81=fmt_arg;_T82=fmt_args;_T83=arg_start_pos - 1;_T84=alias_arg_exps;_T85=c_vararg;_T86=Cyc_Formatstr_get_scanf_types;
# 1238
Cyc_Tcexp_check_format_args(_T80,_T81,_T82,_T83,_T84,_T85,_T86);goto _LLF;}_LLF:;}_pop_handler();goto _TL23C;_TL23B: _T87=Cyc_Core_get_exn_thrown();{void*_T1B7=(void*)_T87;void*_T1B8;_T88=(struct Cyc_List_Nth_exn_struct*)_T1B7;_T89=_T88->tag;_T8A=Cyc_List_Nth;if(_T89!=_T8A)goto _TL241;{struct Cyc_Warn_String_Warn_Warg_struct _T1B9;_T1B9.tag=0;
# 1243
_T1B9.f1=_tag_fat("bad format arguments",sizeof(char),21U);_T8B=_T1B9;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B9=_T8B;void*_T1BA[1];_T1BA[0]=& _T1B9;_T8C=loc;_T8D=_tag_fat(_T1BA,sizeof(void*),1);Cyc_Warn_err2(_T8C,_T8D);}goto _LL16;_TL241: _T1B8=_T1B7;{void*exn=_T1B8;_rethrow(exn);}_LL16:;}_TL23C:;}}goto _LLA;}_TL237: goto _LLA;_LLA:;}_T8E=a;
# 1217
a=_T8E->tl;goto _TL236;_TL235:;}
# 1248
if(args_info==0)goto _TL243;{struct Cyc_Warn_String_Warn_Warg_struct _T1B1;_T1B1.tag=0;
_T1B1.f1=_tag_fat("too few arguments for function ",sizeof(char),32U);_T8F=_T1B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B1=_T8F;{struct Cyc_Warn_Exp_Warn_Warg_struct _T1B2;_T1B2.tag=4;_T1B2.f1=e;_T90=_T1B2;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T1B2=_T90;void*_T1B3[2];_T1B3[0]=& _T1B1;_T1B3[1]=& _T1B2;_T91=loc;_T92=_tag_fat(_T1B3,sizeof(void*),2);Cyc_Warn_err2(_T91,_T92);}}goto _TL244;
# 1251
_TL243: if(es!=0)goto _TL247;else{goto _TL249;}_TL249: _T93=c_vararg;if(_T93)goto _TL247;else{goto _TL248;}_TL248: if(cyc_vararg!=0)goto _TL247;else{goto _TL245;}
_TL247: _T94=c_vararg;if(!_T94)goto _TL24A;
_TL24F: if(es!=0)goto _TL24D;else{goto _TL24E;}
_TL24D: _T95=te;_T96=es;_T97=_T96->hd;_T98=(struct Cyc_Absyn_Exp*)_T97;Cyc_Tcexp_tcExp(_T95,0,_T98);_T99=es;
# 1253
es=_T99->tl;goto _TL24F;_TL24E: goto _TL24B;
# 1255
_TL24A: if(cyc_vararg!=0)goto _TL250;{struct Cyc_Warn_String_Warn_Warg_struct _T1B1;_T1B1.tag=0;
_T1B1.f1=_tag_fat("too many arguments for function ",sizeof(char),33U);_T9A=_T1B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B1=_T9A;{struct Cyc_Warn_Exp_Warn_Warg_struct _T1B2;_T1B2.tag=4;_T1B2.f1=e;_T9B=_T1B2;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T1B2=_T9B;void*_T1B3[2];_T1B3[0]=& _T1B1;_T1B3[1]=& _T1B2;_T9C=loc;_T9D=_tag_fat(_T1B3,sizeof(void*),2);Cyc_Warn_err2(_T9C,_T9D);}}goto _TL251;
# 1258
_TL250: _T9E=cyc_vararg;{struct Cyc_Absyn_VarargInfo _T1B1=*_T9E;int _T1B2;void*_T1B3;_T1B3=_T1B1.type;_T1B2=_T1B1.inject;{void*vt=_T1B3;int inject=_T1B2;
struct Cyc_Absyn_VarargCallInfo*vci;vci=_cycalloc(sizeof(struct Cyc_Absyn_VarargCallInfo));_T9F=vci;_T9F->num_varargs=0;_TA0=vci;
_TA0->injectors=0;_TA1=vci;
_TA1->vai=cyc_vararg;_TA2=vararg_call_info;
*_TA2=vci;_TA3=inject;
# 1264
if(_TA3)goto _TL252;else{goto _TL254;}
# 1266
_TL254: _TL258: if(es!=0)goto _TL256;else{goto _TL257;}
_TL256:{int alias_coercion=0;_TA4=es;_TA5=_TA4->hd;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TA5;_TA6=vci;
_TA6->num_varargs=_TA6->num_varargs + 1;_TA7=te;_TA8=& vt;_TA9=e1;
Cyc_Tcexp_tcExp(_TA7,_TA8,_TA9);_TAA=
Cyc_Tcenv_curr_aquals_bounds(te);_TAB=e1;_TAC=vt;_TAD=& alias_coercion;_TAE=Cyc_Tcutil_coerce_arg(_TAA,_TAB,_TAC,_TAD);if(_TAE)goto _TL259;else{goto _TL25B;}
_TL25B:{struct Cyc_Warn_String_Warn_Warg_struct _T1B4;_T1B4.tag=0;_T1B4.f1=_tag_fat("vararg requires type ",sizeof(char),22U);_TAF=_T1B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B4=_TAF;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B5;_T1B5.tag=2;_T1B5.f1=vt;_TB0=_T1B5;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B5=_TB0;{struct Cyc_Warn_String_Warn_Warg_struct _T1B6;_T1B6.tag=0;
_T1B6.f1=_tag_fat(" but argument has type ",sizeof(char),24U);_TB1=_T1B6;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B6=_TB1;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1B7;_T1B7.tag=3;_TB3=e1;_T1B7.f1=_TB3->topt;_TB2=_T1B7;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1B7=_TB2;void*_T1B8[4];_T1B8[0]=& _T1B4;_T1B8[1]=& _T1B5;_T1B8[2]=& _T1B6;_T1B8[3]=& _T1B7;_TB4=loc;_TB5=_tag_fat(_T1B8,sizeof(void*),4);Cyc_Tcexp_err_and_explain(_TB4,_TB5);}}}}goto _TL25A;_TL259: _TL25A: _TB6=alias_coercion;
if(!_TB6)goto _TL25C;_TB7=alias_arg_exps;{struct Cyc_List_List*_T1B4=_cycalloc(sizeof(struct Cyc_List_List));_TB9=arg_cnt;
_T1B4->hd=(void*)_TB9;_TBA=alias_arg_exps;_T1B4->tl=*_TBA;_TB8=(struct Cyc_List_List*)_T1B4;}*_TB7=_TB8;goto _TL25D;_TL25C: _TL25D: _TBB=e1;_TBC=_TBB->loc;_TBD=
Cyc_Tcenv_curr_aquals_bounds(te);_TBE=vt;_TBF=e1;Cyc_Tcexp_check_consume(_TBC,_TBD,_TBE,_TBF);}}_TC0=es;
# 1266
es=_TC0->tl;arg_cnt=arg_cnt + 1;goto _TL258;_TL257: goto _TL253;
# 1281
_TL252: _TC1=Cyc_Tcutil_pointer_elt_type(vt);{void*_T1B4=Cyc_Absyn_compress(_TC1);struct Cyc_List_List*_T1B5;struct Cyc_Absyn_Datatypedecl*_T1B6;_TC2=(int*)_T1B4;_TC3=*_TC2;if(_TC3!=0)goto _TL25E;_TC4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B4;_TC5=_TC4->f1;_TC6=(int*)_TC5;_TC7=*_TC6;if(_TC7!=22)goto _TL260;_TC8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B4;_TC9=_TC8->f1;_TCA=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC9;_TCB=_TCA->f1;_TCC=_TCB.KnownDatatype;_TCD=_TCC.tag;if(_TCD!=2)goto _TL262;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1B7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B4;_TCE=_T1B7->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T1B8=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TCE;_TCF=_T1B8->f1;_TD0=_TCF.KnownDatatype;_TD1=_TD0.val;{struct Cyc_Absyn_Datatypedecl*_T1B9=*_TD1;_T1B6=_T1B9;}}_T1B5=_T1B7->f2;}{struct Cyc_Absyn_Datatypedecl*td=_T1B6;struct Cyc_List_List*targs=_T1B5;_TD2=te;_TD3=loc;_TD4=td;_TD5=_TD4->name;_TD6=
# 1285
Cyc_Tcenv_lookup_datatypedecl(_TD2,_TD3,_TD5);{struct Cyc_Absyn_Datatypedecl*_T1B7=*_TD6;struct Cyc_Absyn_Datatypedecl*td=_T1B7;
struct Cyc_List_List*fields=0;_TD7=td;_TD8=_TD7->fields;
if(_TD8!=0)goto _TL264;{struct Cyc_Warn_String_Warn_Warg_struct _T1B8;_T1B8.tag=0;
_T1B8.f1=_tag_fat("can't inject into abstract ",sizeof(char),28U);_TD9=_T1B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B8=_TD9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B9;_T1B9.tag=2;_T1B9.f1=vt;_TDA=_T1B9;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B9=_TDA;void*_T1BA[2];_T1BA[0]=& _T1B8;_T1BA[1]=& _T1B9;_TDB=loc;_TDC=_tag_fat(_T1BA,sizeof(void*),2);Cyc_Warn_err2(_TDB,_TDC);}}goto _TL265;
_TL264: _TDD=td;_TDE=_TDD->fields;_TDF=_TDE->v;fields=(struct Cyc_List_List*)_TDF;_TL265: _TE0=td;_TE1=_TE0->tvs;_TE2=targs;{
# 1297
struct Cyc_List_List*inst=Cyc_List_zip(_TE1,_TE2);_TE3=td;_TE4=_TE3->tvs;_TE5=targs;{
struct Cyc_List_List*orig=Cyc_List_zip(_TE4,_TE5);_TE7=Cyc_Hashtable_create;{
# 1300
struct Cyc_Hashtable_Table*(*_T1B8)(int,int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),int(*)(struct Cyc_Absyn_Tvar*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),int(*)(struct Cyc_Absyn_Tvar*)))_TE7;_TE6=_T1B8;}_TE8=Cyc_Absyn_tvar_cmp;_TE9=Cyc_Absyn_tvar_id;{
# 1299
struct Cyc_Hashtable_Table*inst_effects=
_TE6(10,_TE8,_TE9);
int empty_inst_effects=1;
struct Cyc_List_List*espos=es;
_TL269: if(es!=0)goto _TL267;else{goto _TL268;}
_TL267: _TEA=vci;_TEA->num_varargs=_TEA->num_varargs + 1;_TEB=es;_TEC=_TEB->hd;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TEC;_TED=args_already_checked;
# 1307
if(_TED)goto _TL26A;else{goto _TL26C;}
_TL26C: Cyc_Tcexp_tcExp(te,0,e1);_TEE=e1;_TEF=_TEE->loc;_TF0=
Cyc_Tcenv_curr_aquals_bounds(te);_TF1=e1;_TF2=_TF1->topt;_TF3=_check_null(_TF2);_TF4=e1;Cyc_Tcexp_check_consume(_TEF,_TF0,_TF3,_TF4);goto _TL26B;_TL26A: _TL26B: {
# 1311
struct Cyc_List_List*fresh=Cyc_Tcexp_fresh_effect_evars(te,inst,orig);_TF5=te;_TF6=e1;_TF7=
Cyc_Tcutil_pointer_elt_type(vt);_TF8=inst;_TF9=fields;{struct Cyc_Absyn_Datatypefield*f=Cyc_Tcexp_tcInjection(_TF5,_TF6,_TF7,_TF8,_TF9);
if(f==0)goto _TL26D;_TFA=vci;_TFB=vci;_TFC=_TFB->injectors;{struct Cyc_List_List*_T1B8=_cycalloc(sizeof(struct Cyc_List_List));
# 1315
_T1B8->hd=f;_T1B8->tl=0;_TFD=(struct Cyc_List_List*)_T1B8;}
# 1314
_TFA->injectors=
Cyc_List_append(_TFC,_TFD);goto _TL26E;_TL26D: _TL26E:
 _TL272: if(fresh!=0)goto _TL270;else{goto _TL271;}
_TL270: _TFE=fresh;_TFF=_TFE->hd;{struct _tuple14*_T1B8=(struct _tuple14*)_TFF;void*_T1B9;struct Cyc_Absyn_Tvar*_T1BA;{struct _tuple14 _T1BB=*_T1B8;_T1BA=_T1BB.f0;_T1B9=_T1BB.f1;}{struct Cyc_Absyn_Tvar*tv=_T1BA;void*ev=_T1B9;
void*cf=Cyc_Absyn_compress(ev);_T100=cf;_T101=(int*)_T100;_T102=*_T101;if(_T102!=1)goto _TL273;goto _LL26;_TL273:{
# 1322
struct Cyc_List_List*l=0;{struct _handler_cons _T1BB;_T103=& _T1BB;_push_handler(_T103);{int _T1BC=0;_T104=setjmp(_T1BB.handler);if(!_T104)goto _TL275;_T1BC=1;goto _TL276;_TL275: _TL276: if(_T1BC)goto _TL277;else{goto _TL279;}_TL279: _T106=Cyc_Hashtable_lookup;{
# 1324
struct Cyc_List_List*(*_T1BD)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*))_T106;_T105=_T1BD;}l=_T105(inst_effects,tv);_pop_handler();goto _TL278;_TL277: _T107=Cyc_Core_get_exn_thrown();{void*_T1BD=(void*)_T107;void*_T1BE;_T108=(struct Cyc_Core_Not_found_exn_struct*)_T1BD;_T109=_T108->tag;_T10A=Cyc_Core_Not_found;if(_T109!=_T10A)goto _TL27A;
# 1326
l=0;goto _LL2B;_TL27A: _T1BE=_T1BD;{void*exn=_T1BE;_rethrow(exn);}_LL2B:;}_TL278:;}}_T10C=Cyc_Hashtable_insert;{
# 1328
void(*_T1BB)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*))_T10C;_T10B=_T1BB;}_T10D=inst_effects;_T10E=tv;{struct Cyc_List_List*_T1BB=_cycalloc(sizeof(struct Cyc_List_List));_T1BB->hd=cf;_T1BB->tl=l;_T10F=(struct Cyc_List_List*)_T1BB;}_T10B(_T10D,_T10E,_T10F);
empty_inst_effects=0;goto _LL26;}_LL26:;}}_T110=fresh;
# 1316
fresh=_T110->tl;goto _TL272;_TL271:;}}}_T111=es;
# 1303
es=_T111->tl;goto _TL269;_TL268: _T112=empty_inst_effects;
# 1334
if(!_T112)goto _TL27C;goto _LL1E;_TL27C: _T113=
# 1336
Cyc_Tcexp_inst_with_joineff(orig,inst_effects);if(!_T113)goto _TL27E;{
# 1339
struct Cyc_List_List*es=espos;_T114=vci;{struct Cyc_List_List*prevInjectors=_T114->injectors;_T115=
Cyc_List_length(prevInjectors);_T116=Cyc_List_length(es);if(_T115==_T116)goto _TL280;goto _LL22;_TL280:
# 1342
 _TL285: _T117=es;_T118=(unsigned)_T117;if(_T118)goto _TL286;else{goto _TL284;}_TL286: _T119=prevInjectors;_T11A=(unsigned)_T119;if(_T11A)goto _TL283;else{goto _TL284;}
_TL283: _T11B=es;_T11C=_T11B->hd;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_T11C;_T11D=te;_T11E=e1;_T11F=
Cyc_Tcutil_pointer_elt_type(vt);_T120=orig;_T121=fields;{struct Cyc_Absyn_Datatypefield*f=Cyc_Tcexp_tcInjection(_T11D,_T11E,_T11F,_T120,_T121);
if(f==0)goto _TL289;else{goto _TL28A;}_TL28A: _T122=f;_T123=prevInjectors;_T124=_T123->hd;_T125=(struct Cyc_Absyn_Datatypefield*)_T124;if(_T122!=_T125)goto _TL289;else{goto _TL287;}
_TL289: goto _LL22;_TL287:;}}_T126=es;
# 1342
es=_T126->tl;_T127=prevInjectors;prevInjectors=_T127->tl;goto _TL285;_TL284:;}}goto _TL27F;
# 1350
_TL27E: goto _LL22;_TL27F: goto _LL1E;}}}}}_TL262: goto _LL21;_TL260: goto _LL21;_TL25E: _LL21: _LL22:{struct Cyc_Warn_String_Warn_Warg_struct _T1B7;_T1B7.tag=0;
# 1352
_T1B7.f1=_tag_fat("bad inject vararg type",sizeof(char),23U);_T128=_T1B7;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B7=_T128;void*_T1B8[1];_T1B8[0]=& _T1B7;_T129=loc;_T12A=_tag_fat(_T1B8,sizeof(void*),1);Cyc_Warn_err2(_T129,_T12A);}goto _LL1E;_LL1E:;}_TL253:;}}_TL251: _TL24B: goto _TL246;_TL245: _TL246: _TL244:
# 1359
 Cyc_Tcexp_check_all_aqual_bounds(loc,te,qb);_T12B=alias_arg_exps;_T12C=*_T12B;
if(_T12C!=0)goto _TL28B;_T12D=te;_T12E=loc;_T12F=
# 1369 "tcexp.cyc"
_check_null(eff);Cyc_Tcenv_check_effect_accessible(_T12D,_T12E,_T12F);goto _TL28C;_TL28B: _TL28C:
# 1390 "tcexp.cyc"
 if(chk==0)goto _TL28D;{
struct Cyc_List_List*vds=0;
struct Cyc_List_List*ts=0;
struct Cyc_List_List*new_args=0;
# 1395
int i=0;{
struct Cyc_List_List*ainfo=orig_args_info;_TL292: if(ainfo!=0)goto _TL290;else{goto _TL291;}
_TL290: _T130=ainfo;_T131=_T130->hd;{struct _tuple9*_T1B1=(struct _tuple9*)_T131;void*_T1B2;struct Cyc_Absyn_Tqual _T1B3;{struct _tuple9 _T1B4=*_T1B1;_T1B3=_T1B4.f1;_T1B2=_T1B4.f2;}{struct Cyc_Absyn_Tqual tq=_T1B3;void*t=_T1B2;{struct Cyc_List_List*_T1B4=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple19*_T1B5=_cycalloc(sizeof(struct _tuple19));
_T1B5->f0=tq;_T1B5->f1=t;_T133=(struct _tuple19*)_T1B5;}_T1B4->hd=_T133;_T1B4->tl=ts;_T132=(struct Cyc_List_List*)_T1B4;}ts=_T132;{
struct _fat_ptr*ux;ux=_cycalloc(sizeof(struct _fat_ptr));_T134=ux;{struct Cyc_Int_pa_PrintArg_struct _T1B4;_T1B4.tag=1;_T137=i;_T1B4.f1=(unsigned long)_T137;_T136=_T1B4;}{struct Cyc_Int_pa_PrintArg_struct _T1B4=_T136;void*_T1B5[1];_T1B5[0]=& _T1B4;_T138=_tag_fat("arg%d",sizeof(char),6U);_T139=_tag_fat(_T1B5,sizeof(void*),1);_T135=Cyc_aprintf(_T138,_T139);}*_T134=_T135;{
struct _tuple1*x;x=_cycalloc(sizeof(struct _tuple1));_T13A=x;_T13A->f0=Cyc_Absyn_Loc_n();_T13B=x;_T13B->f1=ux;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(loc,x,t,0,0);{struct Cyc_List_List*_T1B4=_cycalloc(sizeof(struct Cyc_List_List));
_T1B4->hd=vd;_T1B4->tl=vds;_T13C=(struct Cyc_List_List*)_T1B4;}vds=_T13C;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T1B4=_cycalloc(sizeof(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct));_T1B4->tag=5;
_T1B4->f1=vd;_T13D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T1B4;}_T13E=(void*)_T13D;_T13F=loc;{struct Cyc_Absyn_Exp*new_arg=Cyc_Absyn_varb_exp(_T13E,_T13F);_T140=new_arg;
_T140->topt=t;{struct Cyc_List_List*_T1B4=_cycalloc(sizeof(struct Cyc_List_List));
_T1B4->hd=new_arg;_T1B4->tl=new_args;_T141=(struct Cyc_List_List*)_T1B4;}new_args=_T141;}}}}}}_T142=ainfo;
# 1396
ainfo=_T142->tl;i=i + 1;goto _TL292;_TL291:;}
# 1407
new_args=Cyc_List_imp_rev(new_args);
ts=Cyc_List_imp_rev(ts);
vds=Cyc_List_imp_rev(vds);{
# 1411
struct Cyc_List_List*ps=0;{
struct Cyc_List_List*vs=vds;_TL296: if(vs!=0)goto _TL294;else{goto _TL295;}
_TL294:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T1B1->tag=1;_T144=vs;_T145=_T144->hd;_T1B1->f1=(struct Cyc_Absyn_Vardecl*)_T145;_T146=& Cyc_Absyn_Wild_p_val;_T147=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T146;_T148=(void*)_T147;_T149=loc;_T1B1->f2=Cyc_Absyn_new_pat(_T148,_T149);_T143=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T1B1;}_T14A=(void*)_T143;_T14B=loc;{struct Cyc_Absyn_Pat*p=Cyc_Absyn_new_pat(_T14A,_T14B);_T14C=p;_T14D=vs;_T14E=_T14D->hd;_T14F=(struct Cyc_Absyn_Vardecl*)_T14E;
_T14C->topt=_T14F->type;{struct Cyc_List_List*_T1B1=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T1B2=_cycalloc(sizeof(struct _tuple21));
_T1B2->f0=0;_T1B2->f1=p;_T151=(struct _tuple21*)_T1B2;}_T1B1->hd=_T151;_T1B1->tl=ps;_T150=(struct Cyc_List_List*)_T1B1;}ps=_T150;}_T152=vs;
# 1412
vs=_T152->tl;goto _TL296;_TL295:;}
# 1417
ps=Cyc_List_imp_rev(ps);{
void*tup_type=Cyc_Absyn_tuple_type(ts);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1B1->tag=6;
_T1B1->f1=tup_type;_T1B1->f2=1;_T1B1->f3=0;_T1B1->f4=ps;_T1B1->f5=0;_T153=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1B1;}_T154=(void*)_T153;_T155=loc;{struct Cyc_Absyn_Pat*tup_pat=Cyc_Absyn_new_pat(_T154,_T155);_T156=tup_pat;
_T156->topt=tup_type;{
struct Cyc_Absyn_Exp*tup_exp=Cyc_Absyn_tuple_exp(args,loc);_T157=tup_exp;
_T157->topt=tup_type;{
struct Cyc_Absyn_Exp*subst_chk=Cyc_Tcutil_deep_copy_exp(1,chk);
# 1425
struct Cyc_List_List*subst=0;{
struct Cyc_List_List*es=new_args;_TL29A: if(arg_vardecls!=0)goto _TL298;else{goto _TL299;}
# 1428
_TL298:{struct Cyc_List_List*_T1B1=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple20*_T1B2=_cycalloc(sizeof(struct _tuple20));_T15A=arg_vardecls;_T15B=_T15A->hd;_T1B2->f0=(struct Cyc_Absyn_Vardecl*)_T15B;_T15C=_check_null(es);_T15D=_T15C->hd;_T1B2->f1=(struct Cyc_Absyn_Exp*)_T15D;_T159=(struct _tuple20*)_T1B2;}_T1B1->hd=_T159;_T1B1->tl=subst;_T158=(struct Cyc_List_List*)_T1B1;}subst=_T158;_T15E=arg_vardecls;
# 1427
arg_vardecls=_T15E->tl;_T15F=es;es=_T15F->tl;goto _TL29A;_TL299:;}_T161=Cyc_Absyn_visit_exp;{
# 1430
void(*_T1B1)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Exp*))_T161;_T160=_T1B1;}_T162=subst;_T163=subst_chk;_T160(Cyc_Tcexp_substitute_vars_exp,Cyc_Tcexp_substitute_vars_stmt,_T162,_T163);{
struct Cyc_Absyn_Exp*asrt=Cyc_Absyn_assert_exp(subst_chk,0,loc);_T164=asrt;
_T164->topt=Cyc_Absyn_sint_type;{
struct Cyc_Absyn_Exp*fncall=Cyc_Absyn_fncall_exp(e,new_args,loc);_T165=fncall;
_T165->topt=res_typ;_T166=loc;{struct _tuple1*_T1B1=_cycalloc(sizeof(struct _tuple1));
_T1B1->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1B2=_cycalloc(sizeof(struct _fat_ptr));*_T1B2=_tag_fat("temp",sizeof(char),5U);_T168=(struct _fat_ptr*)_T1B2;}_T1B1->f1=_T168;_T167=(struct _tuple1*)_T1B1;}_T169=tup_type;_T16A=tup_exp;{struct Cyc_Absyn_Vardecl*tempvd=Cyc_Absyn_new_vardecl(_T166,_T167,_T169,_T16A,0);{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T1B1->tag=4;
# 1437
_T1B1->f1=tempvd;_T16B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T1B1;}_T16C=(void*)_T16B;_T16D=loc;{struct Cyc_Absyn_Exp*tempexp=Cyc_Absyn_varb_exp(_T16C,_T16D);_T16E=tempexp;
_T16E->topt=tup_type;_T16F=te;_T170=tup_pat;_T171=& tup_type;{
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(_T16F,_T170,_T171,0);
void*dp=0;_T172=loc;_T173=te;_T174=tup_pat;_T175=& dp;
Cyc_Tcpat_check_let_pat_exhaustive(_T172,_T173,_T174,_T175);{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct));_T1B1->tag=2;
_T1B1->f1=tup_pat;{struct Cyc_Core_Opt*_T1B2=_cycalloc(sizeof(struct Cyc_Core_Opt));_T178=pat_res;_T1B2->v=_T178.patvars;_T177=(struct Cyc_Core_Opt*)_T1B2;}_T1B1->f2=_T177;_T1B1->f3=tempexp;_T1B1->f4=dp;_T176=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T1B1;}{void*rd=(void*)_T176;_T179=
Cyc_Absyn_new_decl(rd,loc);_T17A=
Cyc_Absyn_exp_stmt(asrt,loc);_T17B=
Cyc_Absyn_exp_stmt(fncall,loc);_T17C=loc;_T17D=
# 1444
Cyc_Absyn_seq_stmt(_T17A,_T17B,_T17C);_T17E=loc;{
# 1443
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_decl_stmt(_T179,_T17D,_T17E);{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T1B1->tag=0;
# 1446
_T1B1->f1=tempvd;_T17F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1B1;}_T180=(void*)_T17F;_T181=loc;_T182=Cyc_Absyn_new_decl(_T180,_T181);_T183=s;_T184=loc;s=Cyc_Absyn_decl_stmt(_T182,_T183,_T184);_T185=call;{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T1B1=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_T1B1->tag=36;
_T1B1->f1=s;_T186=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T1B1;}_T185->r=(void*)_T186;}}}}}}}}}}}}}goto _TL28E;_TL28D: _TL28E: _T187=res_typ;
# 1449
return _T187;}}}_TL220:{struct Cyc_Warn_String_Warn_Warg_struct _T1B1;_T1B1.tag=0;
# 1451
_T1B1.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_T189=_T1B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B1=_T189;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B2;_T1B2.tag=2;_T1B2.f1=t;_T18A=_T1B2;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B2=_T18A;void*_T1B3[2];_T1B3[0]=& _T1B1;_T1B3[1]=& _T1B2;_T18B=te;_T18C=loc;_T18D=topt;_T18E=_tag_fat(_T1B3,sizeof(void*),2);_T188=Cyc_Tcexp_expr_err(_T18B,_T18C,_T18D,_T18E);}}return _T188;;}}goto _TL21F;_TL21E:{struct Cyc_Warn_String_Warn_Warg_struct _T19D;_T19D.tag=0;
# 1454
_T19D.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_T190=_T19D;}{struct Cyc_Warn_String_Warn_Warg_struct _T19D=_T190;{struct Cyc_Warn_Typ_Warn_Warg_struct _T19E;_T19E.tag=2;_T19E.f1=t;_T191=_T19E;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T19E=_T191;void*_T19F[2];_T19F[0]=& _T19D;_T19F[1]=& _T19E;_T192=te;_T193=loc;_T194=topt;_T195=_tag_fat(_T19F,sizeof(void*),2);_T18F=Cyc_Tcexp_expr_err(_T192,_T193,_T194,_T195);}}return _T18F;_TL21F:;}}}
# 1458
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Tcenv_Tenv*_T0;void**_T1;struct Cyc_Absyn_Exp*_T2;unsigned _T3;struct Cyc_Absyn_Exp*_T4;void*_T5;void*_T6;void*_T7;enum Cyc_Absyn_Coercion _T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Typ_Warn_Warg_struct _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_TypOpt_Warn_Warg_struct _TD;struct Cyc_Absyn_Exp*_TE;unsigned _TF;struct _fat_ptr _T10;void**_T11;void*_T12;struct Cyc_Core_Opt*_T13;void*_T14;
int bogus=0;
void*exception_type=Cyc_Absyn_exn_type();_T0=te;_T1=& exception_type;_T2=e;
Cyc_Tcexp_tcExp(_T0,_T1,_T2);_T3=loc;_T4=e;_T5=_T4->topt;_T6=
# 1463
_check_null(_T5);_T7=exception_type;_T8=Cyc_Subtype_coercible(_T3,_T6,_T7);_T9=(int)_T8;if(_T9!=0)goto _TL29B;{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;
_T15.f1=_tag_fat("expected ",sizeof(char),10U);_TA=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TA;{struct Cyc_Warn_Typ_Warn_Warg_struct _T16;_T16.tag=2;_T16.f1=exception_type;_TB=_T16;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T16=_TB;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;_T17.f1=_tag_fat(" but found ",sizeof(char),12U);_TC=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TC;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T18;_T18.tag=3;_TE=e;_T18.f1=_TE->topt;_TD=_T18;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T18=_TD;void*_T19[4];_T19[0]=& _T15;_T19[1]=& _T16;_T19[2]=& _T17;_T19[3]=& _T18;_TF=loc;_T10=_tag_fat(_T19,sizeof(void*),4);Cyc_Warn_err2(_TF,_T10);}}}}goto _TL29C;_TL29B: _TL29C:
 if(topt==0)goto _TL29D;_T11=topt;_T12=*_T11;
return _T12;_TL29D: _T13=
Cyc_Tcenv_lookup_opt_type_vars(te);_T14=Cyc_Absyn_wildtyp(_T13);return _T14;}
# 1470
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrInfo _T9;int*_TA;int _TB;struct Cyc_Absyn_FnInfo _TC;struct Cyc_List_List*_TD;void*_TE;struct Cyc_Absyn_Tvar*_TF;struct Cyc_Absyn_Kind*_T10;struct Cyc_Absyn_Kind*_T11;unsigned _T12;struct Cyc_Tcenv_Tenv*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_Kind*_T15;struct Cyc_List_List*_T16;void*_T17;unsigned _T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_List_List*_T1B;struct _tuple14*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Tcenv_Tenv*_T24;unsigned _T25;void**_T26;struct _fat_ptr _T27;struct _RegionHandle*_T28;struct Cyc_List_List*_T29;struct Cyc_Absyn_FnInfo _T2A;struct Cyc_List_List*_T2B;struct Cyc_Tcenv_Tenv*_T2C;unsigned _T2D;struct Cyc_Absyn_FnInfo _T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T31;void*_T32;struct Cyc_Absyn_PtrInfo _T33;void*_T34;void*_T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Typ_Warn_Warg_struct _T37;struct Cyc_Tcenv_Tenv*_T38;unsigned _T39;void**_T3A;struct _fat_ptr _T3B;_T0=e;_T1=_T0->topt;_T2=
# 1472
_check_null(_T1);{void*t1=Cyc_Absyn_compress(_T2);{struct Cyc_Absyn_PtrAtts _T3C;struct Cyc_Absyn_Tqual _T3D;void*_T3E;_T3=t1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=4)goto _TL29F;_T6=t1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6;_T7=_T3F->f1;_T3E=_T7.elt_type;_T8=_T3F->f1;_T3D=_T8.elt_tq;_T9=_T3F->f1;_T3C=_T9.ptr_atts;}{void*t=_T3E;struct Cyc_Absyn_Tqual tq=_T3D;struct Cyc_Absyn_PtrAtts atts=_T3C;{
# 1475
void*_T3F=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _T40;_TA=(int*)_T3F;_TB=*_TA;if(_TB!=6)goto _TL2A1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T41=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3F;_T40=_T41->f1;}{struct Cyc_Absyn_FnInfo info=_T40;_TC=info;{
# 1477
struct Cyc_List_List*tvars=_TC.tvars;
struct Cyc_List_List*instantiation=0;
# 1480
_TL2A6: if(ts!=0)goto _TL2A7;else{goto _TL2A5;}_TL2A7: if(tvars!=0)goto _TL2A4;else{goto _TL2A5;}
_TL2A4: _TD=tvars;_TE=_TD->hd;_TF=(struct Cyc_Absyn_Tvar*)_TE;_T10=& Cyc_Kinds_bk;_T11=(struct Cyc_Absyn_Kind*)_T10;{struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(_TF,_T11);_T12=loc;_T13=te;_T14=
Cyc_Tcenv_lookup_type_vars(te);_T15=k;_T16=ts;_T17=_T16->hd;Cyc_Tctyp_check_type(_T12,_T13,_T14,_T15,1,1,_T17);_T18=loc;_T19=ts;_T1A=_T19->hd;
Cyc_Tcutil_check_no_qual(_T18,_T1A);{struct Cyc_List_List*_T41=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple14*_T42=_cycalloc(sizeof(struct _tuple14));_T1D=tvars;_T1E=_T1D->hd;
_T42->f0=(struct Cyc_Absyn_Tvar*)_T1E;_T1F=ts;_T42->f1=_T1F->hd;_T1C=(struct _tuple14*)_T42;}_T41->hd=_T1C;_T41->tl=instantiation;_T1B=(struct Cyc_List_List*)_T41;}instantiation=_T1B;}_T20=ts;
# 1480
ts=_T20->tl;_T21=tvars;tvars=_T21->tl;goto _TL2A6;_TL2A5:
# 1486
 info.tvars=tvars;
if(ts==0)goto _TL2A8;{struct Cyc_Warn_String_Warn_Warg_struct _T41;_T41.tag=0;
_T41.f1=_tag_fat("too many type variables in instantiation",sizeof(char),41U);_T23=_T41;}{struct Cyc_Warn_String_Warn_Warg_struct _T41=_T23;void*_T42[1];_T42[0]=& _T41;_T24=te;_T25=loc;_T26=topt;_T27=_tag_fat(_T42,sizeof(void*),1);_T22=Cyc_Tcexp_expr_err(_T24,_T25,_T26,_T27);}return _T22;_TL2A8:
# 1492
 if(tvars!=0)goto _TL2AA;_T28=Cyc_Core_heap_region;_T29=instantiation;_T2A=info;_T2B=_T2A.effconstr;
# 1494
info.effconstr=Cyc_Tcutil_rsubst_effconstr(_T28,_T29,_T2B);_T2C=te;_T2D=loc;_T2E=info;_T2F=_T2E.effconstr;
Cyc_Tcenv_check_effect_constraints(_T2C,_T2D,_T2F);
info.effconstr=0;goto _TL2AB;_TL2AA: _TL2AB: _T30=instantiation;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T41=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T41->tag=6;
# 1498
_T41->f1=info;_T31=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T41;}_T32=(void*)_T31;{void*new_fn_typ=Cyc_Tcutil_substitute(_T30,_T32);{struct Cyc_Absyn_PtrInfo _T41;
_T41.elt_type=new_fn_typ;_T41.elt_tq=tq;_T41.ptr_atts=atts;_T33=_T41;}_T34=Cyc_Absyn_pointer_type(_T33);return _T34;}}}_TL2A1: goto _LL5;_LL5:;}goto _LL0;}_TL29F: goto _LL0;_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C;_T3C.tag=0;
# 1505
_T3C.f1=_tag_fat("expecting polymorphic type but found ",sizeof(char),38U);_T36=_T3C;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C=_T36;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3D;_T3D.tag=2;_T3D.f1=t1;_T37=_T3D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3D=_T37;void*_T3E[2];_T3E[0]=& _T3C;_T3E[1]=& _T3D;_T38=te;_T39=loc;_T3A=topt;_T3B=_tag_fat(_T3E,sizeof(void*),2);_T35=Cyc_Tcexp_expr_err(_T38,_T39,_T3A,_T3B);}}return _T35;}}
# 1509
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;void*_T2;void*_T3;void*_T4;
# 1511
Cyc_Tcexp_tcExpNoInst(te,0,e);_T0=e;_T1=e;_T2=_T1->topt;_T3=
# 1513
_check_null(_T2);_T0->topt=Cyc_Absyn_pointer_expand(_T3,0);_T4=
Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);return _T4;}
# 1518
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){unsigned _T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Kind*_T3;int _T4;struct Cyc_Absyn_Kind*_T5;struct Cyc_Absyn_Kind*_T6;void*_T7;struct Cyc_Tcenv_Tenv*_T8;void**_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;void*_TC;enum Cyc_Absyn_Coercion _TD;int _TE;void*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;struct Cyc_Warn_Typ_Warn_Warg_struct _T13;struct Cyc_Tcenv_Tenv*_T14;unsigned _T15;void**_T16;struct _fat_ptr _T17;void*_T18;enum Cyc_Absyn_Coercion*_T19;struct _tuple0 _T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;int*_T1D;int _T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_Absyn_MallocInfo _T23;void*_T24;struct Cyc_Absyn_PtrInfo _T25;struct Cyc_Absyn_PtrAtts _T26;struct Cyc_Absyn_PtrInfo _T27;struct Cyc_Absyn_PtrAtts _T28;struct Cyc_Absyn_PtrInfo _T29;struct Cyc_Absyn_PtrAtts _T2A;struct Cyc_Absyn_PtrInfo _T2B;struct Cyc_Absyn_PtrAtts _T2C;int _T2D;int _T2E;int _T2F;int _T30;int _T31;void*_T32;void*_T33;struct _tuple16 _T34;unsigned _T35;unsigned _T36;struct _fat_ptr _T37;struct _fat_ptr _T38;unsigned _T39;void*_T3A;void*_T3B;void*_T3C;void*_T3D;int _T3E;struct Cyc_Warn_String_Warn_Warg_struct _T3F;struct Cyc_Warn_Typ_Warn_Warg_struct _T40;unsigned _T41;struct _fat_ptr _T42;void*_T43;_T0=loc;_T1=te;_T2=
# 1520
Cyc_Tcenv_lookup_type_vars(te);_T4=
Cyc_Tcenv_abstract_val_ok(te);if(!_T4)goto _TL2AC;_T5=& Cyc_Kinds_ak;_T3=(struct Cyc_Absyn_Kind*)_T5;goto _TL2AD;_TL2AC: _T6=& Cyc_Kinds_mk;_T3=(struct Cyc_Absyn_Kind*)_T6;_TL2AD: _T7=t;
# 1520
Cyc_Tctyp_check_type(_T0,_T1,_T2,_T3,1,0,_T7);
# 1522
Cyc_Tcutil_check_no_qual(loc,t);_T8=te;_T9=& t;_TA=e;
Cyc_Tcexp_tcExp(_T8,_T9,_TA);_TB=e;_TC=_TB->topt;{
void*t2=_check_null(_TC);
enum Cyc_Absyn_Coercion cc=Cyc_Subtype_coercible(loc,t2,t);_TD=cc;_TE=(int)_TD;
if(_TE!=0)goto _TL2AE;{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;
_T44.f1=_tag_fat("cannot cast ",sizeof(char),13U);_T10=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T45;_T45.tag=2;_T45.f1=t2;_T11=_T45;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T45=_T11;{struct Cyc_Warn_String_Warn_Warg_struct _T46;_T46.tag=0;_T46.f1=_tag_fat(" to ",sizeof(char),5U);_T12=_T46;}{struct Cyc_Warn_String_Warn_Warg_struct _T46=_T12;{struct Cyc_Warn_Typ_Warn_Warg_struct _T47;_T47.tag=2;_T47.f1=t;_T13=_T47;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T47=_T13;void*_T48[4];_T48[0]=& _T44;_T48[1]=& _T45;_T48[2]=& _T46;_T48[3]=& _T47;_T14=te;_T15=loc;_T16=& t;_T17=_tag_fat(_T48,sizeof(void*),4);_TF=Cyc_Tcexp_expr_err(_T14,_T15,_T16,_T17);}}}}{void*result=_TF;_T18=result;
return _T18;}_TL2AE: _T19=
# 1530
_check_null(c);*_T19=cc;{struct _tuple0 _T44;_T1B=e;
# 1535
_T44.f0=_T1B->r;_T44.f1=Cyc_Absyn_compress(t);_T1A=_T44;}{struct _tuple0 _T44=_T1A;void*_T45;void*_T46;void*_T47;void*_T48;int _T49;_T1C=_T44.f0;_T1D=(int*)_T1C;_T1E=*_T1D;if(_T1E!=33)goto _TL2B0;_T1F=_T44.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=4)goto _TL2B2;_T22=_T44.f0;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T22;_T23=_T4A->f1;_T49=_T23.fat_result;}_T24=_T44.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T24;_T25=_T4A->f1;_T26=_T25.ptr_atts;_T48=_T26.nullable;_T27=_T4A->f1;_T28=_T27.ptr_atts;_T47=_T28.bounds;_T29=_T4A->f1;_T2A=_T29.ptr_atts;_T46=_T2A.zero_term;_T2B=_T4A->f1;_T2C=_T2B.ptr_atts;_T45=_T2C.autoreleased;}{int fat_result=_T49;void*nbl=_T48;void*bds=_T47;void*zt=_T46;void*rel=_T45;_T2D=fat_result;
# 1538
if(!_T2D)goto _TL2B4;_T2E=Cyc_Tcutil_force_type2bool(0,zt);if(_T2E)goto _TL2B4;else{goto _TL2B6;}_TL2B6: _T2F=Cyc_Tcutil_force_type2bool(0,nbl);if(!_T2F)goto _TL2B4;_T30=Cyc_Tcutil_force_type2bool(0,rel);if(_T30)goto _TL2B4;else{goto _TL2B7;}
_TL2B7: _T31=Cyc_Tcutil_is_cvar_type(bds);if(_T31)goto _TL2B8;else{goto _TL2BA;}
_TL2BA: _T32=Cyc_Absyn_bounds_one();_T33=bds;{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(_T32,_T33);
if(eopt==0)goto _TL2BB;_T34=
Cyc_Evexp_eval_const_uint_exp(eopt);_T35=_T34.f0;if(_T35!=1U)goto _TL2BD;_T36=loc;_T37=
_tag_fat("cast from ? pointer to * pointer will lose size information",sizeof(char),60U);_T38=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T36,_T37,_T38);goto _TL2BE;_TL2BD: _TL2BE: goto _TL2BC;_TL2BB: _TL2BC:;}goto _TL2B9;
# 1548
_TL2B8: _T39=loc;_T3A=Cyc_Tcutil_ptrbnd_cvar_equivalent(bds);_T3B=_check_null(_T3A);_T3C=
Cyc_Absyn_fatconst();_T3D=
# 1548
Cyc_BansheeIf_equality_constraint(_T3B,_T3C);_T3E=Cyc_BansheeIf_add_constraint(_T39,_T3D);if(_T3E)goto _TL2BF;else{goto _TL2C1;}
# 1550
_TL2C1:{struct Cyc_Warn_String_Warn_Warg_struct _T4A;_T4A.tag=0;_T4A.f1=_tag_fat("Unable to constrain pointer bound",sizeof(char),34U);_T3F=_T4A;}{struct Cyc_Warn_String_Warn_Warg_struct _T4A=_T3F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4B;_T4B.tag=2;_T4B.f1=t;_T40=_T4B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4B=_T40;void*_T4C[2];_T4C[0]=& _T4A;_T4C[1]=& _T4B;_T41=loc;_T42=_tag_fat(_T4C,sizeof(void*),2);Cyc_Warn_err2(_T41,_T42);}}goto _TL2C0;_TL2BF: _TL2C0: _TL2B9: goto _TL2B5;_TL2B4: _TL2B5: goto _LL4;}_TL2B2: goto _LL3;_TL2B0: _LL3: _LL4: _T43=t;
# 1555
return _T43;;}}}
# 1560
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_T0;void*_T1;int*_T2;int _T3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4;void**_T5;struct Cyc_Absyn_PtrInfo _T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrAtts _T8;struct Cyc_Tcenv_Tenv*_T9;struct Cyc_Tcenv_Tenv*_TA;struct Cyc_Tcenv_Tenv*_TB;void**_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Exp*_TF;void*_T10;void*_T11;int _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;struct Cyc_Absyn_Exp*_T14;unsigned _T15;struct _fat_ptr _T16;struct Cyc_Absyn_Exp*_T17;int*_T18;int _T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;struct Cyc_Absyn_Exp*_T1D;int*_T1E;unsigned _T1F;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T20;int _T21;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T22;int _T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct Cyc_Absyn_Exp*_T25;unsigned _T26;struct _fat_ptr _T27;int _T28;void*(*_T29)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);int _T2A;struct Cyc_Absyn_Exp*_T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;struct Cyc_Absyn_Tqual _T30;void*_T31;void*_T32;void*_T33;
void**topt2=0;
struct Cyc_Absyn_Tqual tq2=Cyc_Absyn_empty_tqual(0U);
int nullable=0;
if(topt==0)goto _TL2C2;_T0=topt;_T1=*_T0;{
void*_T34=Cyc_Absyn_compress(_T1);void*_T35;struct Cyc_Absyn_Tqual _T36;void*_T37;_T2=(int*)_T34;_T3=*_T2;if(_T3!=4)goto _TL2C4;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T38=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T34;_T4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T34;_T5=& _T4->f1.elt_type;_T37=(void**)_T5;_T6=_T38->f1;_T36=_T6.elt_tq;_T7=_T38->f1;_T8=_T7.ptr_atts;_T35=_T8.nullable;}{void**elttype=(void**)_T37;struct Cyc_Absyn_Tqual tq=_T36;void*n=_T35;
# 1567
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);goto _LL0;}_TL2C4: goto _LL0;_LL0:;}goto _TL2C3;_TL2C2: _TL2C3: _T9=
# 1580
Cyc_Tcenv_clear_notreadctxt(te);_TA=Cyc_Tcenv_enter_lhs(_T9);_TB=Cyc_Tcenv_enter_abstract_val_ok(_TA);_TC=topt2;_TD=e;Cyc_Tcexp_tcExpNoInst(_TB,_TC,_TD);_TE=
# 1582
Cyc_Tcenv_curr_aquals_bounds(te);_TF=e;_T10=_TF->topt;_T11=_check_null(_T10);_T12=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TE,_T11);if(!_T12)goto _TL2C6;{struct Cyc_Warn_String_Warn_Warg_struct _T34;_T34.tag=0;
_T34.f1=_tag_fat("cannot take the address of an alias-free path",sizeof(char),46U);_T13=_T34;}{struct Cyc_Warn_String_Warn_Warg_struct _T34=_T13;void*_T35[1];_T35[0]=& _T34;_T14=e;_T15=_T14->loc;_T16=_tag_fat(_T35,sizeof(void*),1);Cyc_Warn_err2(_T15,_T16);}goto _TL2C7;_TL2C6: _TL2C7: _T17=e;{
# 1587
void*_T34=_T17->r;struct Cyc_Absyn_Exp*_T35;struct Cyc_Absyn_Exp*_T36;_T18=(int*)_T34;_T19=*_T18;if(_T19!=23)goto _TL2C8;{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T37=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T34;_T36=_T37->f1;_T35=_T37->f2;}{struct Cyc_Absyn_Exp*e1=_T36;struct Cyc_Absyn_Exp*e2=_T35;_T1A=e0;_T1B=
# 1591
Cyc_Absyn_add_exp(e1,e2,0U);_T1A->r=_T1B->r;_T1C=
Cyc_Tcexp_tcPlus(te,e1,e2);return _T1C;}_TL2C8: goto _LL5;_LL5:;}_T1D=e;{
# 1597
void*_T34=_T1D->r;_T1E=(int*)_T34;_T1F=*_T1E;switch(_T1F){case 21: _T20=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T34;_T21=_T20->f3;if(_T21!=1)goto _TL2CB;goto _LLE;_TL2CB: goto _LLF;case 22: _T22=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T34;_T23=_T22->f3;if(_T23!=1)goto _TL2CD;_LLE:{struct Cyc_Warn_String_Warn_Warg_struct _T35;_T35.tag=0;
# 1600
_T35.f1=_tag_fat("cannot take the address of a @tagged union member",sizeof(char),50U);_T24=_T35;}{struct Cyc_Warn_String_Warn_Warg_struct _T35=_T24;void*_T36[1];_T36[0]=& _T35;_T25=e;_T26=_T25->loc;_T27=_tag_fat(_T36,sizeof(void*),1);Cyc_Warn_err2(_T26,_T27);}goto _LLA;_TL2CD: goto _LLF;default: _LLF: goto _LLA;}_LLA:;}{
# 1606
struct _tuple15 _T34=Cyc_Tcutil_addressof_props(e);void*_T35;int _T36;_T36=_T34.f0;_T35=_T34.f1;{int is_const=_T36;void*rgn=_T35;
# 1611
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);_T28=is_const;
if(!_T28)goto _TL2CF;
tq.real_const=1;tq.print_const=tq.real_const;goto _TL2D0;_TL2CF: _TL2D0: _T2A=nullable;
# 1615
if(!_T2A)goto _TL2D1;_T29=Cyc_Absyn_star_type;goto _TL2D2;_TL2D1: _T29=Cyc_Absyn_at_type;_TL2D2: _T2B=e;_T2C=_T2B->topt;_T2D=_check_null(_T2C);_T2E=rgn;_T2F=Cyc_Absyn_al_qual_type;_T30=tq;_T31=Cyc_Absyn_false_type;_T32=Cyc_Absyn_false_type;_T33=_T29(_T2D,_T2E,_T2F,_T30,_T31,_T32);return _T33;}}}
# 1619
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){struct Cyc_Tcenv_Tenv*_T0;int _T1;void*_T2;unsigned _T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_List_List*_T5;struct Cyc_Absyn_Kind*_T6;struct Cyc_Absyn_Kind*_T7;void*_T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Typ_Warn_Warg_struct _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;unsigned _TD;struct _fat_ptr _TE;void*_TF;void**_T10;void*_T11;int*_T12;int _T13;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T14;void*_T15;int*_T16;int _T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;void*_T1B;void*_T1C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T1D;void*_T1E;int _T1F;void**_T20;void*_T21;void*_T22;void*_T23;void*_T24;_T0=te;_T1=_T0->allow_valueof;
# 1622
if(!_T1)goto _TL2D3;_T2=Cyc_Absyn_uint_type;
return _T2;_TL2D3: _T3=loc;_T4=te;_T5=
Cyc_Tcenv_lookup_type_vars(te);_T6=& Cyc_Kinds_mk;_T7=(struct Cyc_Absyn_Kind*)_T6;_T8=t;Cyc_Tctyp_check_type(_T3,_T4,_T5,_T7,1,0,_T8);
Cyc_Tcutil_check_no_qual(loc,t);_T9=
Cyc_Evexp_okay_szofarg(t);if(_T9)goto _TL2D5;else{goto _TL2D7;}
_TL2D7:{struct Cyc_Warn_String_Warn_Warg_struct _T25;_T25.tag=0;_T25.f1=_tag_fat("sizeof applied to type ",sizeof(char),24U);_TA=_T25;}{struct Cyc_Warn_String_Warn_Warg_struct _T25=_TA;{struct Cyc_Warn_Typ_Warn_Warg_struct _T26;_T26.tag=2;_T26.f1=t;_TB=_T26;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T26=_TB;{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;_T27.f1=_tag_fat(" which has unknown size here",sizeof(char),29U);_TC=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_TC;void*_T28[3];_T28[0]=& _T25;_T28[1]=& _T26;_T28[2]=& _T27;_TD=loc;_TE=_tag_fat(_T28,sizeof(void*),3);Cyc_Warn_err2(_TD,_TE);}}}goto _TL2D6;_TL2D5: _TL2D6:
 if(topt!=0)goto _TL2D8;_TF=Cyc_Absyn_uint_type;
return _TF;_TL2D8: _T10=topt;_T11=*_T10;{
void*_T25=Cyc_Absyn_compress(_T11);void*_T26;_T12=(int*)_T25;_T13=*_T12;if(_T13!=0)goto _TL2DA;_T14=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T15=_T14->f1;_T16=(int*)_T15;_T17=*_T16;if(_T17!=5)goto _TL2DC;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T19=_T18->f2;if(_T19==0)goto _TL2DE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T27=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T1A=_T27->f2;{struct Cyc_List_List _T28=*_T1A;_T1B=_T28.hd;_T26=(void*)_T1B;}}{void*tt=_T26;_T1C=tt;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T27=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_T27->tag=9;
# 1632
_T27->f1=Cyc_Absyn_sizeoftype_exp(t,0U);_T1D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T27;}_T1E=(void*)_T1D;_T1F=Cyc_Unify_unify(_T1C,_T1E);if(!_T1F)goto _TL2E0;_T20=topt;_T21=*_T20;_T22=
Cyc_Absyn_compress(_T21);return _T22;_TL2E0: _T23=Cyc_Absyn_uint_type;
return _T23;}_TL2DE: goto _LL3;_TL2DC: goto _LL3;_TL2DA: _LL3: _T24=Cyc_Absyn_uint_type;
return _T24;;}}
# 1639
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_Absyn_Aggrfield*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;int _T5;struct Cyc_Absyn_Aggrfield*_T6;void*_T7;_T0=n;_T1=*_T0;_T2=sf;_T3=_T2->name;_T4=*_T3;_T5=
Cyc_strcmp(_T1,_T4);if(_T5!=0)goto _TL2E2;_T6=sf;_T7=_T6->type;
return _T7;_TL2E2:
 return 0;}
# 1647
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){unsigned _T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Kind*_T3;struct Cyc_Absyn_Kind*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;int*_T8;unsigned _T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;int*_TC;int _TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;void*_TF;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T10;union Cyc_Absyn_AggrInfo _T11;struct _union_AggrInfo_KnownAggr _T12;unsigned _T13;void*_T14;union Cyc_Absyn_AggrInfo _T15;struct _union_AggrInfo_KnownAggr _T16;struct Cyc_Absyn_Aggrdecl**_T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_AggrdeclImpl*_T19;struct Cyc_Absyn_Aggrdecl**_T1A;struct Cyc_Absyn_Aggrdecl*_T1B;struct Cyc_Absyn_AggrdeclImpl*_T1C;void*(*_T1D)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*);void*(*_T1E)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _fat_ptr*_T1F;struct Cyc_List_List*_T20;void*_T21;unsigned _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct _fat_ptr*_T25;unsigned _T26;struct _fat_ptr _T27;int _T28;struct Cyc_Warn_Typ_Warn_Warg_struct _T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;unsigned _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;struct Cyc_String_pa_PrintArg_struct _T2E;struct _fat_ptr _T2F;struct _fat_ptr _T30;struct Cyc_List_List*_T31;struct _fat_ptr _T32;struct Cyc_String_pa_PrintArg_struct _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct _fat_ptr*_T35;struct _fat_ptr _T36;struct _fat_ptr _T37;struct Cyc_List_List*_T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;struct Cyc_Warn_String_Warn_Warg_struct _T3A;struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;unsigned _T3D;struct _fat_ptr _T3E;struct Cyc_List_List*_T3F;void*_T40;_T0=loc;_T1=te;_T2=
# 1649
Cyc_Tcenv_lookup_type_vars(te);_T3=& Cyc_Kinds_mk;_T4=(struct Cyc_Absyn_Kind*)_T3;_T5=t_orig;Cyc_Tctyp_check_type(_T0,_T1,_T2,_T4,1,0,_T5);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*l=fs;
void*t=t_orig;
_TL2E7: if(l!=0)goto _TL2E5;else{goto _TL2E6;}
_TL2E5: _T6=l;{void*n=_T6->hd;struct _fat_ptr*_T41;_T7=n;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T42=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T7;_T41=_T42->f1;}{struct _fat_ptr*n=_T41;
# 1657
int bad_type=1;{
void*_T42=Cyc_Absyn_compress(t);struct Cyc_List_List*_T43;struct Cyc_Absyn_Aggrdecl**_T44;_T8=(int*)_T42;_T9=*_T8;switch(_T9){case 0: _TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_TB=_TA->f1;_TC=(int*)_TB;_TD=*_TC;if(_TD!=24)goto _TL2E9;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_TF=_TE->f1;_T10=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TF;_T11=_T10->f1;_T12=_T11.KnownAggr;_T13=_T12.tag;if(_T13!=2)goto _TL2EB;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T45=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_T14=_T45->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T46=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T14;_T15=_T46->f1;_T16=_T15.KnownAggr;_T44=_T16.val;}}{struct Cyc_Absyn_Aggrdecl**adp=_T44;_T17=adp;_T18=*_T17;_T19=_T18->impl;
# 1660
if(_T19!=0)goto _TL2ED;goto _LL3;_TL2ED: _T1A=adp;_T1B=*_T1A;_T1C=_T1B->impl;_T43=_T1C->fields;goto _LL7;}_TL2EB: goto _LL8;_TL2E9: goto _LL8;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T45=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T42;_T43=_T45->f3;}_LL7: {struct Cyc_List_List*fields=_T43;_T1E=Cyc_List_find_c;{
# 1664
void*(*_T45)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*)=(void*(*)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*))_T1E;_T1D=_T45;}_T1F=n;_T20=fields;{void*t2=_T1D(Cyc_Tcexp_structfield_type,_T1F,_T20);_T21=t2;_T22=(unsigned)_T21;
if(_T22)goto _TL2EF;else{goto _TL2F1;}
_TL2F1:{struct Cyc_Warn_String_Warn_Warg_struct _T45;_T45.tag=0;_T45.f1=_tag_fat("no field of struct/union has name %s",sizeof(char),37U);_T23=_T45;}{struct Cyc_Warn_String_Warn_Warg_struct _T45=_T23;{struct Cyc_Warn_String_Warn_Warg_struct _T46;_T46.tag=0;_T25=n;_T46.f1=*_T25;_T24=_T46;}{struct Cyc_Warn_String_Warn_Warg_struct _T46=_T24;void*_T47[2];_T47[0]=& _T45;_T47[1]=& _T46;_T26=loc;_T27=_tag_fat(_T47,sizeof(void*),2);Cyc_Warn_err2(_T26,_T27);}}goto _TL2F0;
# 1668
_TL2EF: t=t2;_TL2F0:
 bad_type=0;goto _LL3;}}default: _LL8: goto _LL3;}_LL3:;}_T28=bad_type;
# 1673
if(!_T28)goto _TL2F2;
if(l!=fs)goto _TL2F4;{struct Cyc_Warn_Typ_Warn_Warg_struct _T42;_T42.tag=2;
_T42.f1=t;_T29=_T42;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T42=_T29;{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;_T43.f1=_tag_fat(" is not a known struct/union type",sizeof(char),34U);_T2A=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T2A;void*_T44[2];_T44[0]=& _T42;_T44[1]=& _T43;_T2B=loc;_T2C=_tag_fat(_T44,sizeof(void*),2);Cyc_Warn_err2(_T2B,_T2C);}}goto _TL2F5;
# 1677
_TL2F4:{struct Cyc_String_pa_PrintArg_struct _T42;_T42.tag=0;_T42.f1=Cyc_Absynpp_typ2string(t_orig);_T2E=_T42;}{struct Cyc_String_pa_PrintArg_struct _T42=_T2E;void*_T43[1];_T43[0]=& _T42;_T2F=_tag_fat("(%s)",sizeof(char),5U);_T30=_tag_fat(_T43,sizeof(void*),1);_T2D=Cyc_aprintf(_T2F,_T30);}{struct _fat_ptr s=_T2D;{
struct Cyc_List_List*x=fs;_TL2F9: if(x!=l)goto _TL2F7;else{goto _TL2F8;}
_TL2F7: _T31=_check_null(x);{void*_T42=_T31->hd;struct _fat_ptr*_T43;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T44=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T42;_T43=_T44->f1;}{struct _fat_ptr*n=_T43;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;
_T44.f1=s;_T33=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T33;{struct Cyc_String_pa_PrintArg_struct _T45;_T45.tag=0;_T35=n;_T45.f1=*_T35;_T34=_T45;}{struct Cyc_String_pa_PrintArg_struct _T45=_T34;void*_T46[2];_T46[0]=& _T44;_T46[1]=& _T45;_T36=_tag_fat("%s.%s",sizeof(char),6U);_T37=_tag_fat(_T46,sizeof(void*),2);_T32=Cyc_aprintf(_T36,_T37);}}s=_T32;goto _LLA;}_LLA:;}_T38=x;
# 1678
x=_T38->tl;goto _TL2F9;_TL2F8:;}{struct Cyc_Warn_String_Warn_Warg_struct _T42;_T42.tag=0;
# 1682
_T42.f1=s;_T39=_T42;}{struct Cyc_Warn_String_Warn_Warg_struct _T42=_T39;{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;_T43.f1=_tag_fat(" == ",sizeof(char),5U);_T3A=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T3A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T44;_T44.tag=2;_T44.f1=t;_T3B=_T44;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T44=_T3B;{struct Cyc_Warn_String_Warn_Warg_struct _T45;_T45.tag=0;_T45.f1=_tag_fat(" is not a struct/union type",sizeof(char),28U);_T3C=_T45;}{struct Cyc_Warn_String_Warn_Warg_struct _T45=_T3C;void*_T46[4];_T46[0]=& _T42;_T46[1]=& _T43;_T46[2]=& _T44;_T46[3]=& _T45;_T3D=loc;_T3E=_tag_fat(_T46,sizeof(void*),4);Cyc_Warn_err2(_T3D,_T3E);}}}}}_TL2F5: goto _TL2F3;_TL2F2: _TL2F3: goto _LL0;}_LL0:;}_T3F=l;
# 1653
l=_T3F->tl;goto _TL2E7;_TL2E6: _T40=Cyc_Absyn_uint_type;
# 1688
return _T40;}}
# 1692
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig_e,struct Cyc_Absyn_Exp*e){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;void*_T2;void*_T3;void*_T4;int*_T5;int _T6;struct Cyc_Core_Opt*_T7;struct Cyc_Core_Opt*_T8;struct Cyc_Core_Opt*_T9;void*_TA;int*_TB;int _TC;void*_TD;struct Cyc_Absyn_PtrInfo _TE;struct Cyc_Absyn_PtrInfo _TF;struct Cyc_Absyn_PtrAtts _T10;struct Cyc_Absyn_PtrInfo _T11;struct Cyc_Absyn_PtrAtts _T12;struct Cyc_Absyn_PtrInfo _T13;struct Cyc_Absyn_PtrAtts _T14;struct Cyc_Absyn_Kind*_T15;struct Cyc_Absyn_Kind*_T16;struct Cyc_Absyn_Kind*_T17;int _T18;int _T19;int*_T1A;int _T1B;int _T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;unsigned _T1E;struct _fat_ptr _T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_Absyn_Exp*_T21;void*_T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;unsigned _T24;struct _fat_ptr _T25;void*_T26;void*_T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;struct Cyc_Warn_Typ_Warn_Warg_struct _T29;struct Cyc_Tcenv_Tenv*_T2A;unsigned _T2B;void**_T2C;struct _fat_ptr _T2D;_T0=
# 1694
Cyc_Tcenv_clear_notreadctxt(te);te=Cyc_Tcenv_clear_lhs(_T0);
Cyc_Tcexp_tcExp(te,0,e);_T1=e;_T2=_T1->topt;_T3=
_check_null(_T2);{void*t=Cyc_Absyn_compress(_T3);_T4=t;_T5=(int*)_T4;_T6=*_T5;if(_T6!=1)goto _TL2FA;{
# 1699
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);_T7=& Cyc_Kinds_ako;_T8=(struct Cyc_Core_Opt*)_T7;{struct Cyc_Core_Opt*_T2E=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T2E->v=Cyc_Tcenv_lookup_type_vars(te);_T9=(struct Cyc_Core_Opt*)_T2E;}pi.elt_type=Cyc_Absyn_new_evar(_T8,_T9);{
void*new_typ=Cyc_Absyn_pointer_type(pi);
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);goto _LL0;}}_TL2FA: goto _LL0;_LL0: {void*_T2E;void*_T2F;void*_T30;void*_T31;_TA=t;_TB=(int*)_TA;_TC=*_TB;if(_TC!=4)goto _TL2FC;_TD=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T32=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD;_TE=_T32->f1;_T31=_TE.elt_type;_TF=_T32->f1;_T10=_TF.ptr_atts;_T30=_T10.eff;_T11=_T32->f1;_T12=_T11.ptr_atts;_T2F=_T12.bounds;_T13=_T32->f1;_T14=_T13.ptr_atts;_T2E=_T14.zero_term;}{void*t2=_T31;void*rt=_T30;void*b=_T2F;void*zt=_T2E;
# 1709
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);_T15=
Cyc_Tcutil_type_kind(t2);_T16=& Cyc_Kinds_mk;_T17=(struct Cyc_Absyn_Kind*)_T16;_T18=Cyc_Kinds_kind_leq(_T15,_T17);if(_T18)goto _TL2FE;else{goto _TL300;}_TL300: _T19=Cyc_Tcenv_abstract_val_ok(te);if(_T19)goto _TL2FE;else{goto _TL301;}
_TL301:{void*_T32=Cyc_Absyn_compress(t2);_T1A=(int*)_T32;_T1B=*_T1A;if(_T1B!=6)goto _TL302;_T1C=Cyc_Flags_tc_aggressive_warn;
# 1714
if(!_T1C)goto _TL304;{struct Cyc_Warn_String_Warn_Warg_struct _T33;_T33.tag=0;
_T33.f1=_tag_fat("unnecessary dereference for function type",sizeof(char),42U);_T1D=_T33;}{struct Cyc_Warn_String_Warn_Warg_struct _T33=_T1D;void*_T34[1];_T34[0]=& _T33;_T1E=loc;_T1F=_tag_fat(_T34,sizeof(void*),1);Cyc_Warn_warn2(_T1E,_T1F);}goto _TL305;_TL304: _TL305:
# 1717
 if(orig_e==0)goto _TL306;_T20=orig_e;_T21=e;
_T20->r=_T21->r;goto _TL307;_TL306: _TL307: _T22=t;
return _T22;_TL302:{struct Cyc_Warn_String_Warn_Warg_struct _T33;_T33.tag=0;
_T33.f1=_tag_fat("cannot dereference abstract pointer type",sizeof(char),41U);_T23=_T33;}{struct Cyc_Warn_String_Warn_Warg_struct _T33=_T23;void*_T34[1];_T34[0]=& _T33;_T24=loc;_T25=_tag_fat(_T34,sizeof(void*),1);Cyc_Warn_err2(_T24,_T25);};}goto _TL2FF;_TL2FE: _TL2FF: _T26=t2;
# 1722
return _T26;}_TL2FC:{struct Cyc_Warn_String_Warn_Warg_struct _T32;_T32.tag=0;
_T32.f1=_tag_fat("expecting pointer type but found ",sizeof(char),34U);_T28=_T32;}{struct Cyc_Warn_String_Warn_Warg_struct _T32=_T28;{struct Cyc_Warn_Typ_Warn_Warg_struct _T33;_T33.tag=2;_T33.f1=t;_T29=_T33;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T33=_T29;void*_T34[2];_T34[0]=& _T32;_T34[1]=& _T33;_T2A=te;_T2B=loc;_T2C=topt;_T2D=_tag_fat(_T34,sizeof(void*),2);_T27=Cyc_Tcexp_expr_err(_T2A,_T2B,_T2C,_T2D);}}return _T27;;}}}
# 1727
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){int*_T0;int _T1;int*_T2;unsigned _T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;int _T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TA;union Cyc_Absyn_AggrInfo _TB;struct _union_AggrInfo_KnownAggr _TC;unsigned _TD;void*_TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_KnownAggr _T10;struct Cyc_Absyn_Aggrdecl**_T11;void*_T12;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct _fat_ptr*_T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Tcenv_Tenv*_T18;unsigned _T19;void**_T1A;struct _fat_ptr _T1B;struct Cyc_Absyn_Aggrdecl*_T1C;struct Cyc_Absyn_AggrdeclImpl*_T1D;struct Cyc_Absyn_AggrdeclImpl*_T1E;int _T1F;int*_T20;struct Cyc_Absyn_Aggrfield*_T21;struct Cyc_Absyn_Aggrdecl*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Aggrfield*_T26;void*_T27;struct Cyc_Absyn_Kind*_T28;struct Cyc_Absyn_Kind*_T29;struct Cyc_Absyn_Kind*_T2A;int _T2B;int _T2C;int*_T2D;int _T2E;void*_T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;struct _fat_ptr*_T32;struct Cyc_Warn_String_Warn_Warg_struct _T33;struct Cyc_Tcenv_Tenv*_T34;unsigned _T35;void**_T36;struct _fat_ptr _T37;struct Cyc_Absyn_Aggrdecl*_T38;enum Cyc_Absyn_AggrKind _T39;int _T3A;struct Cyc_Absyn_Aggrdecl*_T3B;struct Cyc_Absyn_AggrdeclImpl*_T3C;struct Cyc_Absyn_AggrdeclImpl*_T3D;int _T3E;int _T3F;int _T40;struct Cyc_Absyn_Aggrfield*_T41;struct Cyc_Absyn_Exp*_T42;void*_T43;struct Cyc_Warn_String_Warn_Warg_struct _T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;struct _fat_ptr*_T46;struct Cyc_Warn_String_Warn_Warg_struct _T47;struct Cyc_Tcenv_Tenv*_T48;unsigned _T49;void**_T4A;struct _fat_ptr _T4B;struct Cyc_Absyn_Aggrdecl*_T4C;struct Cyc_Absyn_AggrdeclImpl*_T4D;struct Cyc_Absyn_AggrdeclImpl*_T4E;struct Cyc_List_List*_T4F;void*_T50;struct Cyc_Core_Opt*_T51;void*_T52;int _T53;void*_T54;struct Cyc_Warn_String_Warn_Warg_struct _T55;struct Cyc_Warn_String_Warn_Warg_struct _T56;struct _fat_ptr*_T57;struct Cyc_Warn_String_Warn_Warg_struct _T58;struct Cyc_Tcenv_Tenv*_T59;unsigned _T5A;void**_T5B;struct _fat_ptr _T5C;void*_T5D;void*_T5E;struct Cyc_Warn_String_Warn_Warg_struct _T5F;struct Cyc_Warn_Typ_Warn_Warg_struct _T60;struct Cyc_Warn_String_Warn_Warg_struct _T61;struct Cyc_Warn_String_Warn_Warg_struct _T62;struct _fat_ptr*_T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Tcenv_Tenv*_T65;unsigned _T66;void**_T67;struct _fat_ptr _T68;enum Cyc_Absyn_AggrKind _T69;int _T6A;struct Cyc_Absyn_Aggrfield*_T6B;void*_T6C;int _T6D;int _T6E;struct Cyc_Absyn_Aggrfield*_T6F;struct Cyc_Absyn_Exp*_T70;void*_T71;struct Cyc_Warn_String_Warn_Warg_struct _T72;struct Cyc_Warn_String_Warn_Warg_struct _T73;struct _fat_ptr*_T74;struct Cyc_Warn_String_Warn_Warg_struct _T75;struct Cyc_Tcenv_Tenv*_T76;unsigned _T77;void**_T78;struct _fat_ptr _T79;struct Cyc_Absyn_Aggrfield*_T7A;void*_T7B;void*_T7C;struct Cyc_Warn_String_Warn_Warg_struct _T7D;struct Cyc_Warn_Typ_Warn_Warg_struct _T7E;struct Cyc_Tcenv_Tenv*_T7F;unsigned _T80;void**_T81;struct _fat_ptr _T82;_T0=is_read;_T1=
# 1731
Cyc_Tcenv_in_notreadctxt(te);*_T0=!_T1;{
void*_T83=Cyc_Absyn_compress(aggr_type);enum Cyc_Absyn_AggrKind _T84;struct Cyc_List_List*_T85;struct Cyc_Absyn_Aggrdecl*_T86;_T2=(int*)_T83;_T3=*_T2;switch(_T3){case 0: _T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T83;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=24)goto _TL309;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T83;_T9=_T8->f1;_TA=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T9;_TB=_TA->f1;_TC=_TB.KnownAggr;_TD=_TC.tag;if(_TD!=2)goto _TL30B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T87=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T83;_TE=_T87->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T88=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TE;_TF=_T88->f1;_T10=_TF.KnownAggr;_T11=_T10.val;{struct Cyc_Absyn_Aggrdecl*_T89=*_T11;_T86=_T89;}}_T85=_T87->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T86;struct Cyc_List_List*ts=_T85;
# 1734
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_decl_field(ad,f);
if(finfo!=0)goto _TL30D;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T87;_T87.tag=6;
_T87.f1=ad;_T13=_T87;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T87=_T13;{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;_T88.f1=_tag_fat(" has no ",sizeof(char),9U);_T14=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T14;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T16=f;_T89.f1=*_T16;_T15=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T8A;_T8A.tag=0;_T8A.f1=_tag_fat(" member",sizeof(char),8U);_T17=_T8A;}{struct Cyc_Warn_String_Warn_Warg_struct _T8A=_T17;void*_T8B[4];_T8B[0]=& _T87;_T8B[1]=& _T88;_T8B[2]=& _T89;_T8B[3]=& _T8A;_T18=te;_T19=loc;_T1A=topt;_T1B=_tag_fat(_T8B,sizeof(void*),4);_T12=Cyc_Tcexp_expr_err(_T18,_T19,_T1A,_T1B);}}}}return _T12;_TL30D: _T1C=ad;_T1D=_T1C->impl;_T1E=
# 1738
_check_null(_T1D);_T1F=_T1E->tagged;if(!_T1F)goto _TL30F;_T20=is_tagged;*_T20=1;goto _TL310;_TL30F: _TL310: _T21=finfo;{
void*t2=_T21->type;
if(ts==0)goto _TL311;_T22=ad;_T23=_T22->tvs;_T24=ts;_T25=
Cyc_List_zip(_T23,_T24);_T26=finfo;_T27=_T26->type;t2=Cyc_Tcutil_substitute(_T25,_T27);goto _TL312;_TL311: _TL312: {
struct Cyc_Absyn_Kind*t2_kind=Cyc_Tcutil_type_kind(t2);_T28=& Cyc_Kinds_ak;_T29=(struct Cyc_Absyn_Kind*)_T28;_T2A=t2_kind;_T2B=
# 1746
Cyc_Kinds_kind_leq(_T29,_T2A);if(!_T2B)goto _TL313;_T2C=Cyc_Tcenv_abstract_val_ok(te);if(_T2C)goto _TL313;else{goto _TL315;}
_TL315:{void*_T87=Cyc_Absyn_compress(t2);_T2D=(int*)_T87;_T2E=*_T2D;if(_T2E!=5)goto _TL316;goto _LL7;_TL316:{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;
# 1749
_T88.f1=_tag_fat("cannot get member ",sizeof(char),19U);_T30=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T30;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T32=f;_T89.f1=*_T32;_T31=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T31;{struct Cyc_Warn_String_Warn_Warg_struct _T8A;_T8A.tag=0;
_T8A.f1=_tag_fat(" since its type is abstract",sizeof(char),28U);_T33=_T8A;}{struct Cyc_Warn_String_Warn_Warg_struct _T8A=_T33;void*_T8B[3];_T8B[0]=& _T88;_T8B[1]=& _T89;_T8B[2]=& _T8A;_T34=te;_T35=loc;_T36=topt;_T37=_tag_fat(_T8B,sizeof(void*),3);_T2F=Cyc_Tcexp_expr_err(_T34,_T35,_T36,_T37);}}}
# 1749
return _T2F;_LL7:;}goto _TL314;_TL313: _TL314: _T38=ad;_T39=_T38->kind;_T3A=(int)_T39;
# 1754
if(_T3A!=1)goto _TL318;_T3B=ad;_T3C=_T3B->impl;_T3D=_check_null(_T3C);_T3E=_T3D->tagged;if(_T3E)goto _TL318;else{goto _TL31A;}_TL31A: _T3F=Cyc_Tcutil_is_bits_only_type(t2);if(_T3F)goto _TL318;else{goto _TL31B;}_TL31B: _T40=Cyc_Tcenv_in_notreadctxt(te);if(_T40)goto _TL318;else{goto _TL31C;}_TL31C: _T41=finfo;_T42=_T41->requires_clause;if(_T42!=0)goto _TL318;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
_T87.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_T44=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T44;{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;_T46=f;_T88.f1=*_T46;_T45=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T45;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;
_T89.f1=_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_T47=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T47;void*_T8A[3];_T8A[0]=& _T87;_T8A[1]=& _T88;_T8A[2]=& _T89;_T48=te;_T49=loc;_T4A=topt;_T4B=_tag_fat(_T8A,sizeof(void*),3);_T43=Cyc_Tcexp_expr_err(_T48,_T49,_T4A,_T4B);}}}
# 1755
return _T43;_TL318: _T4C=ad;_T4D=_T4C->impl;_T4E=
# 1757
_check_null(_T4D);_T4F=_T4E->exist_vars;if(_T4F==0)goto _TL31D;_T50=t2;_T51=
# 1760
Cyc_Tcenv_lookup_opt_type_vars(te);_T52=Cyc_Absyn_wildtyp(_T51);_T53=Cyc_Unify_unify(_T50,_T52);if(_T53)goto _TL31F;else{goto _TL321;}
_TL321:{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;_T87.f1=_tag_fat("must use pattern-matching to access field ",sizeof(char),43U);_T55=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T55;{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;_T57=f;_T88.f1=*_T57;_T56=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T56;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat("\n\tdue to existential type variables.",sizeof(char),37U);_T58=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T58;void*_T8A[3];_T8A[0]=& _T87;_T8A[1]=& _T88;_T8A[2]=& _T89;_T59=te;_T5A=loc;_T5B=topt;_T5C=_tag_fat(_T8A,sizeof(void*),3);_T54=Cyc_Tcexp_expr_err(_T59,_T5A,_T5B,_T5C);}}}return _T54;_TL31F: goto _TL31E;_TL31D: _TL31E: _T5D=t2;
# 1763
return _T5D;}}}_TL30B: goto _LL5;_TL309: goto _LL5;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T87=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T83;_T84=_T87->f1;_T85=_T87->f3;}{enum Cyc_Absyn_AggrKind k=_T84;struct Cyc_List_List*fs=_T85;
# 1765
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fs,f);
if(finfo!=0)goto _TL322;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
_T87.f1=_tag_fat("type ",sizeof(char),6U);_T5F=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T5F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T88;_T88.tag=2;_T88.f1=aggr_type;_T60=_T88;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T88=_T60;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat(" has no ",sizeof(char),9U);_T61=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T61;{struct Cyc_Warn_String_Warn_Warg_struct _T8A;_T8A.tag=0;_T63=f;_T8A.f1=*_T63;_T62=_T8A;}{struct Cyc_Warn_String_Warn_Warg_struct _T8A=_T62;{struct Cyc_Warn_String_Warn_Warg_struct _T8B;_T8B.tag=0;_T8B.f1=_tag_fat(" member",sizeof(char),8U);_T64=_T8B;}{struct Cyc_Warn_String_Warn_Warg_struct _T8B=_T64;void*_T8C[5];_T8C[0]=& _T87;_T8C[1]=& _T88;_T8C[2]=& _T89;_T8C[3]=& _T8A;_T8C[4]=& _T8B;_T65=te;_T66=loc;_T67=topt;_T68=_tag_fat(_T8C,sizeof(void*),5);_T5E=Cyc_Tcexp_expr_err(_T65,_T66,_T67,_T68);}}}}}return _T5E;_TL322: _T69=k;_T6A=(int)_T69;
# 1770
if(_T6A!=1)goto _TL324;_T6B=finfo;_T6C=_T6B->type;_T6D=Cyc_Tcutil_is_bits_only_type(_T6C);if(_T6D)goto _TL324;else{goto _TL326;}_TL326: _T6E=Cyc_Tcenv_in_notreadctxt(te);if(_T6E)goto _TL324;else{goto _TL327;}_TL327: _T6F=finfo;_T70=_T6F->requires_clause;if(_T70!=0)goto _TL324;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
# 1772
_T87.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_T72=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T72;{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;_T74=f;_T88.f1=*_T74;_T73=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T73;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_T75=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T75;void*_T8A[3];_T8A[0]=& _T87;_T8A[1]=& _T88;_T8A[2]=& _T89;_T76=te;_T77=loc;_T78=topt;_T79=_tag_fat(_T8A,sizeof(void*),3);_T71=Cyc_Tcexp_expr_err(_T76,_T77,_T78,_T79);}}}return _T71;_TL324: _T7A=finfo;_T7B=_T7A->type;
return _T7B;}default: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
# 1775
_T87.f1=_tag_fat("expecting struct or union, found ",sizeof(char),34U);_T7D=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T7D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T88;_T88.tag=2;_T88.f1=aggr_type;_T7E=_T88;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T88=_T7E;void*_T89[2];_T89[0]=& _T87;_T89[1]=& _T88;_T7F=te;_T80=loc;_T81=topt;_T82=_tag_fat(_T89,sizeof(void*),2);_T7C=Cyc_Tcexp_expr_err(_T7F,_T80,_T81,_T82);}}return _T7C;};}}
# 1780
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;int*_T2;int _T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;_T0=
# 1784
Cyc_Tcenv_enter_abstract_val_ok(te);_T1=e;{void*t2=Cyc_Tcexp_tcExpNoPromote(_T0,0,_T1);
# 1786
_TL328: if(1)goto _TL329;else{goto _TL32A;}
_TL329:{void*_T6=Cyc_Absyn_compress(t2);struct Cyc_Absyn_Vardecl*_T7;_T2=(int*)_T6;_T3=*_T2;if(_T3!=12)goto _TL32B;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T8=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T6;_T7=_T8->f1;}{struct Cyc_Absyn_Vardecl*vd=_T7;_T4=vd;
# 1789
t2=_T4->type;goto _TL328;}_TL32B: goto _LL0;_LL0:;}goto _TL32A;goto _TL328;_TL32A: _T5=
# 1795
Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);return _T5;}}
# 1799
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){struct Cyc_Tcenv_Tenv*_T0;unsigned _T1;struct Cyc_Absyn_Exp*_T2;int*_T3;int _T4;struct Cyc_Absyn_Vardecl*_T5;void*_T6;_T0=
# 1803
Cyc_Tcenv_enter_abstract_val_ok(te);_T1=loc;_T2=e;{void*t2=Cyc_Tcexp_tcDeref(_T0,_T1,0,0,_T2);
_TL32D: if(1)goto _TL32E;else{goto _TL32F;}
_TL32E:{void*_T7=Cyc_Absyn_compress(t2);struct Cyc_Absyn_Vardecl*_T8;_T3=(int*)_T7;_T4=*_T3;if(_T4!=12)goto _TL330;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T9=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T7;_T8=_T9->f1;}{struct Cyc_Absyn_Vardecl*vd=_T8;_T5=vd;
t2=_T5->type;goto _TL32D;}_TL330: goto _LL0;_LL0:;}goto _TL32F;goto _TL32D;_TL32F: _T6=
# 1811
Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);return _T6;}}
# 1816
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Tcenv_Tenv*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;void*_T7;struct Cyc_Absyn_Exp*_T8;void*_T9;void*_TA;int _TB;void*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_Typ_Warn_Warg_struct _TE;struct Cyc_Tcenv_Tenv*_TF;struct Cyc_Absyn_Exp*_T10;unsigned _T11;void**_T12;struct _fat_ptr _T13;void*_T14;int*_T15;unsigned _T16;void*_T17;struct Cyc_Absyn_PtrInfo _T18;struct Cyc_Absyn_PtrInfo _T19;struct Cyc_Absyn_PtrInfo _T1A;struct Cyc_Absyn_PtrAtts _T1B;struct Cyc_Absyn_PtrInfo _T1C;struct Cyc_Absyn_PtrAtts _T1D;struct Cyc_Absyn_PtrInfo _T1E;struct Cyc_Absyn_PtrAtts _T1F;int _T20;unsigned _T21;void*_T22;void*_T23;void*_T24;int _T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Warn_Typ_Warn_Warg_struct _T27;unsigned _T28;struct _fat_ptr _T29;int _T2A;int _T2B;int _T2C;int _T2D;int _T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;int _T32;struct Cyc_Absyn_Exp*_T33;unsigned _T34;struct _fat_ptr _T35;struct _fat_ptr _T36;int _T37;int _T38;unsigned _T39;unsigned _T3A;void*_T3B;int _T3C;struct Cyc_Absyn_Kind*_T3D;struct Cyc_Absyn_Kind*_T3E;struct Cyc_Absyn_Kind*_T3F;int _T40;int _T41;struct Cyc_Warn_String_Warn_Warg_struct _T42;struct Cyc_Absyn_Exp*_T43;unsigned _T44;struct _fat_ptr _T45;void*_T46;void*_T47;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T48;enum Cyc_Absyn_AggrKind _T49;void*_T4A;int _T4B;int _T4C;void*_T4D;struct Cyc_Warn_String_Warn_Warg_struct _T4E;struct Cyc_Tcenv_Tenv*_T4F;unsigned _T50;struct _fat_ptr _T51;unsigned _T52;int _T53;struct Cyc_Absyn_Exp*_T54;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T55;int _T56;void*_T57;struct Cyc_Warn_String_Warn_Warg_struct _T58;struct Cyc_Warn_Int_Warn_Warg_struct _T59;unsigned _T5A;struct Cyc_Warn_String_Warn_Warg_struct _T5B;struct Cyc_Warn_Int_Warn_Warg_struct _T5C;struct Cyc_Warn_String_Warn_Warg_struct _T5D;struct Cyc_Tcenv_Tenv*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct Cyc_Absyn_Aggrfield*_T61;void*_T62;void*_T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Warn_Typ_Warn_Warg_struct _T65;struct Cyc_Tcenv_Tenv*_T66;unsigned _T67;void**_T68;struct _fat_ptr _T69;_T0=
# 1818
Cyc_Tcenv_clear_notreadctxt(te_orig);{struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_clear_lhs(_T0);_T1=
Cyc_Tcenv_clear_abstract_val_ok(te);_T2=e1;Cyc_Tcexp_tcExp(_T1,0,_T2);_T3=
Cyc_Tcenv_clear_abstract_val_ok(te);_T4=e2;Cyc_Tcexp_tcExp(_T3,0,_T4);_T5=e1;_T6=_T5->topt;_T7=
_check_null(_T6);{void*t1=Cyc_Absyn_compress(_T7);_T8=e2;_T9=_T8->topt;_TA=
_check_null(_T9);{void*t2=Cyc_Absyn_compress(_TA);_TB=
Cyc_Tcutil_coerce_sint_type(e2);if(_TB)goto _TL332;else{goto _TL334;}
_TL334:{struct Cyc_Warn_String_Warn_Warg_struct _T6A;_T6A.tag=0;_T6A.f1=_tag_fat("expecting int subscript, found ",sizeof(char),32U);_TD=_T6A;}{struct Cyc_Warn_String_Warn_Warg_struct _T6A=_TD;{struct Cyc_Warn_Typ_Warn_Warg_struct _T6B;_T6B.tag=2;_T6B.f1=t2;_TE=_T6B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T6B=_TE;void*_T6C[2];_T6C[0]=& _T6A;_T6C[1]=& _T6B;_TF=te;_T10=e2;_T11=_T10->loc;_T12=topt;_T13=_tag_fat(_T6C,sizeof(void*),2);_TC=Cyc_Tcexp_expr_err(_TF,_T11,_T12,_T13);}}return _TC;_TL332: {struct Cyc_List_List*_T6A;int _T6B;void*_T6C;void*_T6D;void*_T6E;struct Cyc_Absyn_Tqual _T6F;void*_T70;_T14=t1;_T15=(int*)_T14;_T16=*_T15;switch(_T16){case 4: _T17=t1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T17;_T18=_T71->f1;_T70=_T18.elt_type;_T19=_T71->f1;_T6F=_T19.elt_tq;_T1A=_T71->f1;_T1B=_T1A.ptr_atts;_T6E=_T1B.eff;_T1C=_T71->f1;_T1D=_T1C.ptr_atts;_T6D=_T1D.bounds;_T1E=_T71->f1;_T1F=_T1E.ptr_atts;_T6C=_T1F.zero_term;}{void*t=_T70;struct Cyc_Absyn_Tqual tq=_T6F;void*rt=_T6E;void*b=_T6D;void*zt=_T6C;_T20=
# 1829
Cyc_Tcutil_is_cvar_type(b);if(!_T20)goto _TL336;_T21=loc;_T22=b;_T23=
Cyc_Absyn_fatconst();_T24=Cyc_BansheeIf_equality_constraint(_T22,_T23);_T25=Cyc_BansheeIf_add_constraint(_T21,_T24);
# 1829
if(_T25)goto _TL336;else{goto _TL338;}
# 1832
_TL338:{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;_T71.f1=_tag_fat("Unable to determine bound on pointer type ",sizeof(char),43U);_T26=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T26;{struct Cyc_Warn_Typ_Warn_Warg_struct _T72;_T72.tag=2;_T72.f1=t1;_T27=_T72;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T72=_T27;void*_T73[2];_T73[0]=& _T71;_T73[1]=& _T72;_T28=loc;_T29=_tag_fat(_T73,sizeof(void*),2);Cyc_Warn_err2(_T28,_T29);}}goto _TL337;_TL336: _TL337: _T2A=
# 1836
Cyc_Tcutil_force_type2bool(0,zt);if(!_T2A)goto _TL339;{
int emit_warning=0;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt==0)goto _TL33B;{
struct Cyc_Absyn_Exp*e3=eopt;
struct _tuple16 _T71=Cyc_Evexp_eval_const_uint_exp(e3);int _T72;unsigned _T73;_T73=_T71.f0;_T72=_T71.f1;{unsigned i=_T73;int known_i=_T72;_T2B=known_i;
if(!_T2B)goto _TL33D;if(i!=1U)goto _TL33D;emit_warning=1;goto _TL33E;_TL33D: _TL33E: _T2C=
Cyc_Tcutil_is_const_exp(e2);if(!_T2C)goto _TL33F;{
struct _tuple16 _T74=Cyc_Evexp_eval_const_uint_exp(e2);int _T75;unsigned _T76;_T76=_T74.f0;_T75=_T74.f1;{unsigned j=_T76;int known_j=_T75;_T2D=known_j;
if(!_T2D)goto _TL341;{
struct _tuple16 _T77=Cyc_Evexp_eval_const_uint_exp(e3);int _T78;unsigned _T79;_T79=_T77.f0;_T78=_T77.f1;{unsigned j=_T79;int knownj=_T78;_T2E=known_i;
if(!_T2E)goto _TL343;if(j <= i)goto _TL343;if(i==1U)goto _TL343;_T2F=loc;_T30=
_tag_fat("subscript will fail at run-time",sizeof(char),32U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2F,_T30,_T31);goto _TL344;_TL343: _TL344:;}}goto _TL342;_TL341: _TL342:;}}goto _TL340;_TL33F: _TL340:;}}goto _TL33C;_TL33B: _TL33C: _T32=emit_warning;
# 1852
if(!_T32)goto _TL345;_T33=e2;_T34=_T33->loc;_T35=
_tag_fat("subscript on thin, zero-terminated pointer could be expensive.",sizeof(char),63U);_T36=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T34,_T35,_T36);goto _TL346;_TL345: _TL346:;}goto _TL33A;
# 1856
_TL339: _T37=Cyc_Tcutil_is_const_exp(e2);if(!_T37)goto _TL347;{
struct _tuple16 _T71=Cyc_Evexp_eval_const_uint_exp(e2);int _T72;unsigned _T73;_T73=_T71.f0;_T72=_T71.f1;{unsigned i=_T73;int known=_T72;_T38=known;
if(!_T38)goto _TL349;_T39=loc;_T3A=i;_T3B=b;_T3C=
# 1861
Cyc_Tcenv_abstract_val_ok(te);Cyc_Tcutil_check_bound(_T39,_T3A,_T3B,_T3C);goto _TL34A;_TL349: _TL34A:;}}goto _TL348;
# 1869
_TL347: Cyc_Tcutil_check_nonzero_bound(loc,b);_TL348: _TL33A:
# 1872
 Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rt);_T3D=
Cyc_Tcutil_type_kind(t);_T3E=& Cyc_Kinds_mk;_T3F=(struct Cyc_Absyn_Kind*)_T3E;_T40=Cyc_Kinds_kind_leq(_T3D,_T3F);if(_T40)goto _TL34B;else{goto _TL34D;}_TL34D: _T41=Cyc_Tcenv_abstract_val_ok(te);if(_T41)goto _TL34B;else{goto _TL34E;}
_TL34E:{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;_T71.f1=_tag_fat("can't subscript an abstract pointer",sizeof(char),36U);_T42=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T42;void*_T72[1];_T72[0]=& _T71;_T43=e1;_T44=_T43->loc;_T45=_tag_fat(_T72,sizeof(void*),1);Cyc_Warn_err2(_T44,_T45);}goto _TL34C;_TL34B: _TL34C: _T46=t;
return _T46;}case 7: _T47=t1;_T48=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T47;_T49=_T48->f1;if(_T49!=Cyc_Absyn_StructA)goto _TL34F;_T4A=t1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T71=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T4A;_T6B=_T71->f2;_T6A=_T71->f3;}_T4B=(int)_T6B;if(!_T4B)goto _TL351;{int is_tuple=_T6B;struct Cyc_List_List*fields=_T6A;
# 1877
struct _tuple16 _T71=Cyc_Evexp_eval_const_uint_exp(e2);int _T72;unsigned _T73;_T73=_T71.f0;_T72=_T71.f1;{unsigned i=_T73;int known=_T72;_T4C=known;
if(_T4C)goto _TL353;else{goto _TL355;}
_TL355:{struct Cyc_Warn_String_Warn_Warg_struct _T74;_T74.tag=0;
_T74.f1=_tag_fat("tuple projection cannot use sizeof or offsetof",sizeof(char),47U);_T4E=_T74;}{struct Cyc_Warn_String_Warn_Warg_struct _T74=_T4E;void*_T75[1];_T75[0]=& _T74;_T4F=te;_T50=loc;_T51=_tag_fat(_T75,sizeof(void*),1);_T4D=Cyc_Tcexp_expr_err(_T4F,_T50,0,_T51);}
# 1879
return _T4D;_TL353: _T52=i;_T53=(int)_T52;{
# 1881
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T74=Cyc_Absyn_tuple_field_designator(_T53);struct _fat_ptr*_T75;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T76=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T74;_T75=_T76->f1;}{struct _fat_ptr*n=_T75;_T54=orig;{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T76=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_T76->tag=21;
_T76->f1=e1;_T76->f2=n;_T76->f3=0;_T56=Cyc_Tcenv_in_notreadctxt(te);_T76->f4=!_T56;_T55=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T76;}_T54->r=(void*)_T55;{
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fields,n);
if(finfo!=0)goto _TL356;{struct Cyc_Warn_String_Warn_Warg_struct _T76;_T76.tag=0;
_T76.f1=_tag_fat("index is ",sizeof(char),10U);_T58=_T76;}{struct Cyc_Warn_String_Warn_Warg_struct _T76=_T58;{struct Cyc_Warn_Int_Warn_Warg_struct _T77;_T77.tag=12;_T5A=i;_T77.f1=(int)_T5A;_T59=_T77;}{struct Cyc_Warn_Int_Warn_Warg_struct _T77=_T59;{struct Cyc_Warn_String_Warn_Warg_struct _T78;_T78.tag=0;_T78.f1=_tag_fat(" but tuple has only ",sizeof(char),21U);_T5B=_T78;}{struct Cyc_Warn_String_Warn_Warg_struct _T78=_T5B;{struct Cyc_Warn_Int_Warn_Warg_struct _T79;_T79.tag=12;
_T79.f1=Cyc_List_length(fields);_T5C=_T79;}{struct Cyc_Warn_Int_Warn_Warg_struct _T79=_T5C;{struct Cyc_Warn_String_Warn_Warg_struct _T7A;_T7A.tag=0;_T7A.f1=_tag_fat(" fields",sizeof(char),8U);_T5D=_T7A;}{struct Cyc_Warn_String_Warn_Warg_struct _T7A=_T5D;void*_T7B[5];_T7B[0]=& _T76;_T7B[1]=& _T77;_T7B[2]=& _T78;_T7B[3]=& _T79;_T7B[4]=& _T7A;_T5E=te;_T5F=loc;_T60=_tag_fat(_T7B,sizeof(void*),5);_T57=Cyc_Tcexp_expr_err(_T5E,_T5F,0,_T60);}}}}}
# 1885
return _T57;_TL356: _T61=finfo;_T62=_T61->type;
# 1888
return _T62;}}}}}_TL351: goto _LL5;_TL34F: goto _LL5;default: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;
_T71.f1=_tag_fat("subscript applied to ",sizeof(char),22U);_T64=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T64;{struct Cyc_Warn_Typ_Warn_Warg_struct _T72;_T72.tag=2;_T72.f1=t1;_T65=_T72;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T72=_T65;void*_T73[2];_T73[0]=& _T71;_T73[1]=& _T72;_T66=te;_T67=loc;_T68=topt;_T69=_tag_fat(_T73,sizeof(void*),2);_T63=Cyc_Tcexp_expr_err(_T66,_T67,_T68,_T69);}}return _T63;};}}}}}
# 1894
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){struct _tuple9*_T0;unsigned _T1;struct Cyc_Tcenv_Tenv*_T2;struct Cyc_List_List*_T3;struct Cyc_Absyn_Kind*_T4;int _T5;struct Cyc_Absyn_Kind*_T6;struct Cyc_Absyn_Kind*_T7;void*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TA;unsigned _TB;void**_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Exp*_TF;void*_T10;void*_T11;void*_T12;_T0=targ;{struct _tuple9 _T13=*_T0;_T12=_T13.f2;}{void*t=_T12;_T1=loc;_T2=te;_T3=
# 1900
Cyc_Tcenv_lookup_type_vars(te);_T5=
Cyc_Tcenv_abstract_val_ok(te);if(!_T5)goto _TL358;_T6=& Cyc_Kinds_ak;_T4=(struct Cyc_Absyn_Kind*)_T6;goto _TL359;_TL358: _T7=& Cyc_Kinds_mk;_T4=(struct Cyc_Absyn_Kind*)_T7;_TL359: _T8=t;
# 1900
Cyc_Tctyp_check_type(_T1,_T2,_T3,_T4,1,0,_T8);_T9=orig_exp;{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T13=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T13->tag=35;
# 1902
_T13->f1=0;_T13->f2=des;_TA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T13;}_T9->r=(void*)_TA;_TB=loc;_TC=& t;_TD=orig_exp;_TE=des;
Cyc_Tcexp_resolve_unresolved_mem(_TB,_TC,_TD,_TE);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);_TF=orig_exp;_T10=_TF->topt;_T11=
_check_null(_T10);return _T11;}}
# 1915 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){struct Cyc_List_List*(*_T0)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T2)(struct _tuple18*);void*(*_T3)(struct _tuple0*);struct Cyc_List_List*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;struct Cyc_Core_Opt*_T7;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T8;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T9;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_TA;void*_TB;unsigned _TC;int _TD;struct Cyc_Absyn_Exp*(*_TE)(struct Cyc_List_List*,int);void*(*_TF)(struct Cyc_List_List*,int);struct Cyc_List_List*_T10;int _T11;int _T12;struct Cyc_Absyn_Exp*_T13;unsigned _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Absyn_Exp*_T17;void*_T18;struct Cyc_Absyn_Tqual _T19;struct Cyc_Absyn_Tqual*_T1A;unsigned _T1B;struct Cyc_Absyn_Tqual*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;int _T1F;struct Cyc_Tcenv_Tenv*_T20;void**_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;void*_T27;struct Cyc_List_List*_T28;int _T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;struct Cyc_Warn_Typ_Warn_Warg_struct _T2B;struct Cyc_Warn_String_Warn_Warg_struct _T2C;struct Cyc_List_List*_T2D;void*_T2E;struct Cyc_Absyn_Exp*_T2F;unsigned _T30;struct _fat_ptr _T31;struct Cyc_List_List*_T32;void*_T33;struct _tuple18*_T34;struct _tuple18 _T35;struct Cyc_List_List*_T36;int*_T37;int _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;unsigned _T3A;struct _fat_ptr _T3B;int _T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;struct Cyc_Absyn_Exp*_T3E;unsigned _T3F;struct _fat_ptr _T40;unsigned _T41;int _T42;unsigned _T43;struct Cyc_Warn_String_Warn_Warg_struct _T44;struct Cyc_Warn_Int_Warn_Warg_struct _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Warn_Int_Warn_Warg_struct _T47;unsigned _T48;struct Cyc_Absyn_Exp*_T49;unsigned _T4A;struct _fat_ptr _T4B;struct Cyc_List_List*_T4C;void*_T4D;
# 1918
void*res_t2;
int num_es=Cyc_List_length(des);_T1=Cyc_List_map;{
struct Cyc_List_List*(*_T4E)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*))_T1;_T0=_T4E;}_T3=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T4E)(struct _tuple18*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))_T3;_T2=_T4E;}_T4=des;{struct Cyc_List_List*es=_T0(_T2,_T4);_T5=& Cyc_Kinds_mko;_T6=(struct Cyc_Core_Opt*)_T5;_T7=
Cyc_Tcenv_lookup_opt_type_vars(te);{void*res=Cyc_Absyn_new_evar(_T6,_T7);
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp;sz_rexp=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct));_T8=sz_rexp;_T8->tag=0;_T9=sz_rexp;_T9->f1=Cyc_Absyn_Int_c(1U,num_es);_TA=sz_rexp;_TB=(void*)_TA;_TC=loc;{
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp(_TB,_TC);_TD=zero_term;
# 1926
if(!_TD)goto _TL35A;_TF=Cyc_List_nth;{
struct Cyc_Absyn_Exp*(*_T4E)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))_TF;_TE=_T4E;}_T10=es;_T11=num_es - 1;{struct Cyc_Absyn_Exp*e=_TE(_T10,_T11);_T12=
Cyc_Tcutil_is_zero(e);if(_T12)goto _TL35C;else{goto _TL35E;}
_TL35E: _T13=e;_T14=_T13->loc;_T15=_tag_fat("zero-terminated array doesn't end with zero.",sizeof(char),45U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T14,_T15,_T16);goto _TL35D;_TL35C: _TL35D:;}goto _TL35B;_TL35A: _TL35B: _T17=sz_exp;
# 1931
_T17->topt=Cyc_Absyn_uint_type;_T18=res;_T1A=elt_tqopt;_T1B=(unsigned)_T1A;
# 1933
if(!_T1B)goto _TL35F;_T1C=elt_tqopt;_T19=*_T1C;goto _TL360;_TL35F: _T19=Cyc_Absyn_empty_tqual(0U);_TL360: _T1D=sz_exp;_T1F=zero_term;
if(!_T1F)goto _TL361;_T1E=Cyc_Absyn_true_type;goto _TL362;_TL361: _T1E=Cyc_Absyn_false_type;_TL362:
# 1932
 res_t2=Cyc_Absyn_array_type(_T18,_T19,_T1D,_T1E,0U);{
# 1936
struct Cyc_List_List*es2=es;_TL366: if(es2!=0)goto _TL364;else{goto _TL365;}
_TL364: _T20=te;_T21=elt_topt;_T22=es2;_T23=_T22->hd;_T24=(struct Cyc_Absyn_Exp*)_T23;Cyc_Tcexp_tcExpInitializer(_T20,_T21,_T24);_T25=es2;
# 1936
es2=_T25->tl;goto _TL366;_TL365:;}_T26=
# 1939
Cyc_Tcenv_curr_aquals_bounds(te);_T27=res;_T28=es;_T29=Cyc_Tcutil_coerce_list(_T26,_T27,_T28);if(_T29)goto _TL367;else{goto _TL369;}
# 1941
_TL369:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("elements of array do not all have the same type (",sizeof(char),50U);_T2A=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T2A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F;_T4F.tag=2;
_T4F.f1=res;_T2B=_T4F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F=_T2B;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;_T50.f1=_tag_fat(")",sizeof(char),2U);_T2C=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_T2C;void*_T51[3];_T51[0]=& _T4E;_T51[1]=& _T4F;_T51[2]=& _T50;_T2D=
# 1941
_check_null(es);_T2E=_T2D->hd;_T2F=(struct Cyc_Absyn_Exp*)_T2E;_T30=_T2F->loc;_T31=_tag_fat(_T51,sizeof(void*),3);Cyc_Warn_err2(_T30,_T31);}}}goto _TL368;_TL367: _TL368:{
# 1944
int offset=0;_TL36D: if(des!=0)goto _TL36B;else{goto _TL36C;}
_TL36B: _T32=des;_T33=_T32->hd;_T34=(struct _tuple18*)_T33;_T35=*_T34;{struct Cyc_List_List*ds=_T35.f0;
if(ds==0)goto _TL36E;_T36=ds;{
void*_T4E=_T36->hd;struct Cyc_Absyn_Exp*_T4F;_T37=(int*)_T4E;_T38=*_T37;if(_T38!=1)goto _TL370;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;
# 1949
_T50.f1=_tag_fat("only array index designators are supported",sizeof(char),43U);_T39=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_T39;void*_T51[1];_T51[0]=& _T50;_T3A=loc;_T3B=_tag_fat(_T51,sizeof(void*),1);Cyc_Warn_err2(_T3A,_T3B);}goto _LL0;_TL370:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T50=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T4E;_T4F=_T50->f1;}{struct Cyc_Absyn_Exp*e=_T4F;
# 1951
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _T50=Cyc_Evexp_eval_const_uint_exp(e);int _T51;unsigned _T52;_T52=_T50.f0;_T51=_T50.f1;{unsigned i=_T52;int known=_T51;_T3C=known;
if(_T3C)goto _TL372;else{goto _TL374;}
_TL374:{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;_T53.f1=_tag_fat("index designator cannot use sizeof or offsetof",sizeof(char),47U);_T3D=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T3D;void*_T54[1];_T54[0]=& _T53;_T3E=e;_T3F=_T3E->loc;_T40=_tag_fat(_T54,sizeof(void*),1);Cyc_Warn_err2(_T3F,_T40);}goto _TL373;
_TL372: _T41=i;_T42=offset;_T43=(unsigned)_T42;if(_T41==_T43)goto _TL375;{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;
_T53.f1=_tag_fat("expecting index designator ",sizeof(char),28U);_T44=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T44;{struct Cyc_Warn_Int_Warn_Warg_struct _T54;_T54.tag=12;_T54.f1=offset;_T45=_T54;}{struct Cyc_Warn_Int_Warn_Warg_struct _T54=_T45;{struct Cyc_Warn_String_Warn_Warg_struct _T55;_T55.tag=0;_T55.f1=_tag_fat(" but found ",sizeof(char),12U);_T46=_T55;}{struct Cyc_Warn_String_Warn_Warg_struct _T55=_T46;{struct Cyc_Warn_Int_Warn_Warg_struct _T56;_T56.tag=12;_T48=i;_T56.f1=(int)_T48;_T47=_T56;}{struct Cyc_Warn_Int_Warn_Warg_struct _T56=_T47;void*_T57[4];_T57[0]=& _T53;_T57[1]=& _T54;_T57[2]=& _T55;_T57[3]=& _T56;_T49=e;_T4A=_T49->loc;_T4B=_tag_fat(_T57,sizeof(void*),4);Cyc_Warn_err2(_T4A,_T4B);}}}}goto _TL376;_TL375: _TL376: _TL373: goto _LL0;}}}_LL0:;}goto _TL36F;_TL36E: _TL36F:;}
# 1944
offset=offset + 1;_T4C=des;des=_T4C->tl;goto _TL36D;_TL36C:;}_T4D=res_t2;
# 1961
return _T4D;}}}}struct _tuple22{void**f0;struct Cyc_Absyn_Tqual*f1;void**f2;};
# 1966
static struct _tuple22 Cyc_Tcexp_ptrEltAtts(void**topt){void**_T0;unsigned _T1;struct _tuple22 _T2;void**_T3;void*_T4;int*_T5;unsigned _T6;struct _tuple22 _T7;void**_T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_Tqual*_TA;struct Cyc_Absyn_PtrInfo _TB;void**_TC;struct Cyc_Absyn_PtrInfo _TD;struct Cyc_Absyn_PtrAtts _TE;struct _tuple22 _TF;void**_T10;struct Cyc_Absyn_ArrayInfo _T11;struct Cyc_Absyn_Tqual*_T12;struct Cyc_Absyn_ArrayInfo _T13;void**_T14;struct Cyc_Absyn_ArrayInfo _T15;struct _tuple22 _T16;_T0=topt;_T1=(unsigned)_T0;
if(_T1)goto _TL377;else{goto _TL379;}
_TL379:{struct _tuple22 _T17;_T17.f0=0;_T17.f1=0;_T17.f2=0;_T2=_T17;}return _T2;_TL377: _T3=topt;_T4=*_T3;{
void*_T17=Cyc_Absyn_compress(_T4);struct Cyc_Absyn_ArrayInfo _T18;struct Cyc_Absyn_PtrInfo _T19;_T5=(int*)_T17;_T6=*_T5;switch(_T6){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T17;_T19=_T1A->f1;}{struct Cyc_Absyn_PtrInfo x=_T19;{struct _tuple22 _T1A;{void**_T1B=_cycalloc(sizeof(void*));_T9=x;
# 1971
*_T1B=_T9.elt_type;_T8=(void**)_T1B;}_T1A.f0=_T8;{struct Cyc_Absyn_Tqual*_T1B=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));_TB=x;*_T1B=_TB.elt_tq;_TA=(struct Cyc_Absyn_Tqual*)_T1B;}_T1A.f1=_TA;{void**_T1B=_cycalloc(sizeof(void*));_TD=x;_TE=_TD.ptr_atts;*_T1B=_TE.zero_term;_TC=(void**)_T1B;}_T1A.f2=_TC;_T7=_T1A;}return _T7;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T17;_T18=_T1A->f1;}{struct Cyc_Absyn_ArrayInfo x=_T18;{struct _tuple22 _T1A;{void**_T1B=_cycalloc(sizeof(void*));_T11=x;
*_T1B=_T11.elt_type;_T10=(void**)_T1B;}_T1A.f0=_T10;{struct Cyc_Absyn_Tqual*_T1B=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));_T13=x;*_T1B=_T13.tq;_T12=(struct Cyc_Absyn_Tqual*)_T1B;}_T1A.f1=_T12;{void**_T1B=_cycalloc(sizeof(void*));_T15=x;*_T1B=_T15.zero_term;_T14=(void**)_T1B;}_T1A.f2=_T14;_TF=_T1A;}return _TF;}default:{struct _tuple22 _T1A;
_T1A.f0=0;_T1A.f1=0;_T1A.f2=0;_T16=_T1A;}return _T16;};}}
# 1978
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;enum Cyc_Tcenv_NewStatus _TD;int _TE;void*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Exp*_T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp*_T17;unsigned _T18;struct _fat_ptr _T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_Absyn_Tqual _T1B;int _T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;int(*_T1E)(struct _fat_ptr);void*(*_T1F)(struct _fat_ptr);struct _fat_ptr _T20;struct Cyc_Tcenv_Tenv*_T21;struct Cyc_Tcenv_Fenv*_T22;struct Cyc_Absyn_Exp*_T23;unsigned _T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_Tcenv_Tenv*_T28;struct Cyc_Tcenv_Fenv*_T29;int _T2A;struct Cyc_Warn_String_Warn_Warg_struct _T2B;struct Cyc_Absyn_Exp*_T2C;unsigned _T2D;struct _fat_ptr _T2E;int _T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;struct Cyc_Absyn_Exp*_T31;unsigned _T32;struct _fat_ptr _T33;void**_T34;void*_T35;int _T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Exp*_T38;int*_T39;struct Cyc_Absyn_Exp*_T3A;unsigned _T3B;struct Cyc_List_List*_T3C;struct Cyc_Absyn_Exp*_T3D;void*_T3E;void*_T3F;struct Cyc_Absyn_Exp*_T40;void*_T41;struct Cyc_Absyn_Tqual _T42;struct Cyc_Absyn_Tqual*_T43;struct Cyc_Absyn_Exp*_T44;void*_T45;void**_T46;void*_T47;
# 1981
Cyc_Tcexp_tcExp(te,0,bound);_T0=bound;_T1=_T0->topt;_T2=
_check_null(_T1);{void*_T48=Cyc_Absyn_compress(_T2);void*_T49;_T3=(int*)_T48;_T4=*_T3;if(_T4!=0)goto _TL37B;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=5)goto _TL37D;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_TA=_T9->f2;if(_TA==0)goto _TL37F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_TB=_T4A->f2;{struct Cyc_List_List _T4B=*_TB;_TC=_T4B.hd;_T49=(void*)_TC;}}{void*t=_T49;_TD=
# 1986
Cyc_Tcenv_new_status(te);_TE=(int)_TD;if(_TE!=2)goto _TL381;_TF=Cyc_Absyn_uint_type;_T10=
Cyc_Absyn_valueof_exp(t,0U);{struct Cyc_Absyn_Exp*b=Cyc_Absyn_cast_exp(_TF,_T10,0,1U,0U);_T11=b;_T12=bound;
_T11->topt=_T12->topt;
bound=b;}goto _TL382;_TL381: _TL382: goto _LL0;}_TL37F: goto _LL3;_TL37D: goto _LL3;_TL37B: _LL3: _T13=
# 1993
Cyc_Tcutil_coerce_uint_type(bound);if(_T13)goto _TL383;else{goto _TL385;}
_TL385:{struct Cyc_Warn_String_Warn_Warg_struct _T4A;_T4A.tag=0;_T4A.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_T14=_T4A;}{struct Cyc_Warn_String_Warn_Warg_struct _T4A=_T14;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T4B;_T4B.tag=3;_T16=bound;_T4B.f1=_T16->topt;_T15=_T4B;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T4B=_T15;void*_T4C[2];_T4C[0]=& _T4A;_T4C[1]=& _T4B;_T17=bound;_T18=_T17->loc;_T19=_tag_fat(_T4C,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}goto _TL384;_TL383: _TL384: _LL0:;}_T1A=vd;_T1B=_T1A->tq;_T1C=_T1B.real_const;
# 1997
if(_T1C)goto _TL386;else{goto _TL388;}
_TL388:{struct Cyc_Warn_String_Warn_Warg_struct _T48;_T48.tag=0;_T48.f1=_tag_fat("comprehension index variable is not declared const!",sizeof(char),52U);_T1D=_T48;}{struct Cyc_Warn_String_Warn_Warg_struct _T48=_T1D;void*_T49[1];_T49[0]=& _T48;_T1F=Cyc_Warn_impos2;{int(*_T4A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1F;_T1E=_T4A;}_T20=_tag_fat(_T49,sizeof(void*),1);_T1E(_T20);}goto _TL387;_TL386: _TL387: _T21=te;_T22=_T21->le;
# 2000
if(_T22==0)goto _TL389;
te=Cyc_Tcenv_new_block(loc,te);goto _TL38A;_TL389: _TL38A: {
# 2003
struct _tuple22 _T48=Cyc_Tcexp_ptrEltAtts(topt);void**_T49;struct Cyc_Absyn_Tqual*_T4A;void**_T4B;_T4B=_T48.f0;_T4A=_T48.f1;_T49=_T48.f2;{void**topt2=_T4B;struct Cyc_Absyn_Tqual*tqopt=_T4A;void**ztopt=_T49;
# 2006
void*t=Cyc_Tcexp_tcExp(te,topt2,body);_T23=body;_T24=_T23->loc;_T25=
# 2008
Cyc_Tcenv_curr_aquals_bounds(te);_T26=t;_T27=body;Cyc_Tcexp_check_consume(_T24,_T25,_T26,_T27);_T28=te;_T29=_T28->le;
if(_T29!=0)goto _TL38B;_T2A=
# 2011
Cyc_Tcutil_is_const_exp(bound);if(_T2A)goto _TL38D;else{goto _TL38F;}
_TL38F:{struct Cyc_Warn_String_Warn_Warg_struct _T4C;_T4C.tag=0;_T4C.f1=_tag_fat("bound is not constant",sizeof(char),22U);_T2B=_T4C;}{struct Cyc_Warn_String_Warn_Warg_struct _T4C=_T2B;void*_T4D[1];_T4D[0]=& _T4C;_T2C=bound;_T2D=_T2C->loc;_T2E=_tag_fat(_T4D,sizeof(void*),1);Cyc_Warn_err2(_T2D,_T2E);}goto _TL38E;_TL38D: _TL38E: _T2F=
Cyc_Tcutil_is_const_exp(body);if(_T2F)goto _TL390;else{goto _TL392;}
_TL392:{struct Cyc_Warn_String_Warn_Warg_struct _T4C;_T4C.tag=0;_T4C.f1=_tag_fat("body is not constant",sizeof(char),21U);_T30=_T4C;}{struct Cyc_Warn_String_Warn_Warg_struct _T4C=_T30;void*_T4D[1];_T4D[0]=& _T4C;_T31=body;_T32=_T31->loc;_T33=_tag_fat(_T4D,sizeof(void*),1);Cyc_Warn_err2(_T32,_T33);}goto _TL391;_TL390: _TL391: goto _TL38C;_TL38B: _TL38C:
# 2016
 if(ztopt==0)goto _TL393;_T34=ztopt;_T35=*_T34;_T36=Cyc_Tcutil_force_type2bool(0,_T35);if(!_T36)goto _TL393;{
# 2018
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_uint_exp(1U,0U);_T37=e1;_T37->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);_T38=bound;_T38->topt=Cyc_Absyn_uint_type;_T39=is_zero_term;
*_T39=1;}goto _TL394;_TL393: _TL394: _T3A=body;_T3B=_T3A->loc;_T3C=
# 2022
Cyc_Tcenv_curr_aquals_bounds(te);_T3D=body;_T3E=_T3D->topt;_T3F=_check_null(_T3E);_T40=body;Cyc_Tcexp_check_consume(_T3B,_T3C,_T3F,_T40);_T41=t;
# 2024
if(tqopt!=0)goto _TL395;_T42=Cyc_Absyn_empty_tqual(0U);goto _TL396;_TL395: _T43=tqopt;_T42=*_T43;_TL396: _T44=bound;
if(ztopt!=0)goto _TL397;_T45=Cyc_Absyn_false_type;goto _TL398;_TL397: _T46=ztopt;_T45=*_T46;_TL398: _T47=
# 2024
Cyc_Absyn_array_type(_T41,_T42,_T44,_T45,0U);return _T47;}}}
# 2029
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;enum Cyc_Tcenv_NewStatus _TD;int _TE;void*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Exp*_T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp*_T17;unsigned _T18;struct _fat_ptr _T19;unsigned _T1A;struct Cyc_Tcenv_Tenv*_T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Kind*_T1D;struct Cyc_Absyn_Kind*_T1E;void*_T1F;void**_T20;void*_T21;void*_T22;struct Cyc_Tcenv_Tenv*_T23;struct Cyc_Tcenv_Fenv*_T24;int _T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Absyn_Exp*_T27;unsigned _T28;struct _fat_ptr _T29;void**_T2A;void*_T2B;int _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;unsigned _T2E;struct _fat_ptr _T2F;void*_T30;struct Cyc_Absyn_Tqual _T31;struct Cyc_Absyn_Tqual*_T32;struct Cyc_Absyn_Exp*_T33;void*_T34;void**_T35;void*_T36;
# 2032
Cyc_Tcexp_tcExp(te,0,bound);_T0=bound;_T1=_T0->topt;_T2=
_check_null(_T1);{void*_T37=Cyc_Absyn_compress(_T2);void*_T38;_T3=(int*)_T37;_T4=*_T3;if(_T4!=0)goto _TL399;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T37;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=5)goto _TL39B;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T37;_TA=_T9->f2;if(_TA==0)goto _TL39D;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T39=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T37;_TB=_T39->f2;{struct Cyc_List_List _T3A=*_TB;_TC=_T3A.hd;_T38=(void*)_TC;}}{void*t=_T38;_TD=
# 2037
Cyc_Tcenv_new_status(te);_TE=(int)_TD;if(_TE!=2)goto _TL39F;_TF=Cyc_Absyn_uint_type;_T10=
Cyc_Absyn_valueof_exp(t,0U);{struct Cyc_Absyn_Exp*b=Cyc_Absyn_cast_exp(_TF,_T10,0,1U,0U);_T11=b;_T12=bound;
_T11->topt=_T12->topt;
bound=b;}goto _TL3A0;_TL39F: _TL3A0: goto _LL0;}_TL39D: goto _LL3;_TL39B: goto _LL3;_TL399: _LL3: _T13=
# 2044
Cyc_Tcutil_coerce_uint_type(bound);if(_T13)goto _TL3A1;else{goto _TL3A3;}
_TL3A3:{struct Cyc_Warn_String_Warn_Warg_struct _T39;_T39.tag=0;_T39.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_T14=_T39;}{struct Cyc_Warn_String_Warn_Warg_struct _T39=_T14;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3A;_T3A.tag=3;_T16=bound;_T3A.f1=_T16->topt;_T15=_T3A;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3A=_T15;void*_T3B[2];_T3B[0]=& _T39;_T3B[1]=& _T3A;_T17=bound;_T18=_T17->loc;_T19=_tag_fat(_T3B,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}goto _TL3A2;_TL3A1: _TL3A2: _LL0:;}{
# 2047
struct _tuple22 _T37=Cyc_Tcexp_ptrEltAtts(topt);void**_T38;struct Cyc_Absyn_Tqual*_T39;void**_T3A;_T3A=_T37.f0;_T39=_T37.f1;_T38=_T37.f2;{void**topt2=_T3A;struct Cyc_Absyn_Tqual*tqopt=_T39;void**ztopt=_T38;_T1A=loc;_T1B=te;_T1C=
# 2050
Cyc_Tcenv_lookup_type_vars(te);_T1D=& Cyc_Kinds_mk;_T1E=(struct Cyc_Absyn_Kind*)_T1D;_T1F=t;Cyc_Tctyp_check_type(_T1A,_T1B,_T1C,_T1E,1,1,_T1F);
if(topt2==0)goto _TL3A4;_T20=topt2;_T21=*_T20;_T22=t;
Cyc_Unify_unify(_T21,_T22);goto _TL3A5;_TL3A4: _TL3A5: _T23=te;_T24=_T23->le;
# 2054
if(_T24!=0)goto _TL3A6;_T25=Cyc_Tcutil_is_const_exp(bound);if(_T25)goto _TL3A6;else{goto _TL3A8;}
_TL3A8:{struct Cyc_Warn_String_Warn_Warg_struct _T3B;_T3B.tag=0;_T3B.f1=_tag_fat("bound is not constant",sizeof(char),22U);_T26=_T3B;}{struct Cyc_Warn_String_Warn_Warg_struct _T3B=_T26;void*_T3C[1];_T3C[0]=& _T3B;_T27=bound;_T28=_T27->loc;_T29=_tag_fat(_T3C,sizeof(void*),1);Cyc_Warn_err2(_T28,_T29);}goto _TL3A7;_TL3A6: _TL3A7:
# 2058
 if(ztopt==0)goto _TL3A9;_T2A=ztopt;_T2B=*_T2A;_T2C=Cyc_Tcutil_force_type2bool(0,_T2B);if(!_T2C)goto _TL3A9;{struct Cyc_Warn_String_Warn_Warg_struct _T3B;_T3B.tag=0;
_T3B.f1=_tag_fat("non-initializing comprehensions do not currently support @zeroterm arrays",sizeof(char),74U);_T2D=_T3B;}{struct Cyc_Warn_String_Warn_Warg_struct _T3B=_T2D;void*_T3C[1];_T3C[0]=& _T3B;_T2E=loc;_T2F=_tag_fat(_T3C,sizeof(void*),1);Cyc_Warn_err2(_T2E,_T2F);}goto _TL3AA;_TL3A9: _TL3AA: _T30=t;
# 2062
if(tqopt!=0)goto _TL3AB;_T31=Cyc_Absyn_empty_tqual(0U);goto _TL3AC;_TL3AB: _T32=tqopt;_T31=*_T32;_TL3AC: _T33=bound;
if(ztopt!=0)goto _TL3AD;_T34=Cyc_Absyn_false_type;goto _TL3AE;_TL3AD: _T35=ztopt;_T34=*_T35;_TL3AE: _T36=
# 2062
Cyc_Absyn_array_type(_T30,_T31,_T33,_T34,0U);return _T36;}}}
# 2066
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5;struct Cyc_Absyn_FnInfo*_T6;int(*_T7)(void*,void*);struct Cyc_Absyn_FnInfo*_T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_TA;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_TB;void*_TC;int _TD;void*_TE;void*_TF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T10;struct Cyc_Absyn_PtrInfo*_T11;struct Cyc_Absyn_PtrInfo*_T12;void*_T13;int _T14;void*_T15;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 6: _T3=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T16=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3;_T4=t;_T5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T4;_T6=& _T5->f1;_T15=(struct Cyc_Absyn_FnInfo*)_T6;}{struct Cyc_Absyn_FnInfo*i=_T15;_T7=Cyc_Atts_attribute_cmp;_T8=i;_T9=_T8->attributes;_TA=& Cyc_Atts_No_throw_att_val;_TB=(struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*)_TA;_TC=(void*)_TB;_TD=
# 2069
Cyc_List_mem(_T7,_T9,_TC);return _TD;}case 4: _TE=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T16=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TE;_TF=t;_T10=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TF;_T11=& _T10->f1;_T15=(struct Cyc_Absyn_PtrInfo*)_T11;}{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_T15;_T12=pi;_T13=_T12->elt_type;_T14=
# 2073
Cyc_Tcexp_ensure_typqual_nothrow(_T13);return _T14;}default:
# 2075
 return 1;};}
# 2079
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;int _T3;struct Cyc_Absyn_Exp*_T4;void*_T5;void*_T6;int _T7;_T0=e;{
void*_T8=_T0->r;struct Cyc_Absyn_Exp*_T9;_T1=(int*)_T8;_T2=*_T1;if(_T2!=14)goto _TL3B0;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T8;_T9=_TA->f2;}{struct Cyc_Absyn_Exp*e1=_T9;_T3=
Cyc_Tcexp_ensure_nothrow_initializer(e1);return _T3;}_TL3B0: _T4=e;_T5=_T4->topt;_T6=
_check_null(_T5);_T7=Cyc_Tcexp_ensure_typqual_nothrow(_T6);return _T7;;}}struct _tuple23{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Exp*f1;};
# 2096 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl**_T0;struct Cyc_Absyn_Aggrdecl*_T1;struct Cyc_Absyn_Aggrdecl**_T2;struct Cyc_Absyn_Aggrdecl**_T3;struct _handler_cons*_T4;int _T5;struct Cyc_Tcenv_Tenv*_T6;unsigned _T7;struct _tuple1**_T8;struct _tuple1*_T9;struct Cyc_Absyn_Aggrdecl**_TA;void*_TB;struct Cyc_Dict_Absent_exn_struct*_TC;char*_TD;char*_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_Qvar_Warn_Warg_struct _T10;struct _tuple1**_T11;unsigned _T12;struct _fat_ptr _T13;void*_T14;void**_T15;struct Cyc_Absyn_Aggrdecl**_T16;struct _tuple1**_T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_Aggrdecl*_T19;struct Cyc_Absyn_AggrdeclImpl*_T1A;void*_T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T1D;struct Cyc_Tcenv_Tenv*_T1E;unsigned _T1F;struct _fat_ptr _T20;struct Cyc_Tcenv_Tenv*_T21;enum Cyc_Tcenv_NewStatus _T22;int _T23;struct _tuple13 _T24;struct Cyc_List_List*(*_T25)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T26)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T27)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T28;struct Cyc_Absyn_Aggrdecl*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*(*_T2B)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T2C)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T2D)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T2E;struct Cyc_Absyn_Aggrdecl*_T2F;struct Cyc_Absyn_AggrdeclImpl*_T30;struct Cyc_Absyn_AggrdeclImpl*_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*(*_T33)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T34)(void*(*)(void*),struct Cyc_List_List*);void*(*_T35)(struct _tuple14*);void*(*_T36)(struct _tuple0*);struct Cyc_List_List*_T37;struct Cyc_List_List*(*_T38)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T39)(void*(*)(void*),struct Cyc_List_List*);void*(*_T3A)(struct _tuple14*);void*(*_T3B)(struct _tuple0*);struct Cyc_List_List*_T3C;struct Cyc_Absyn_Aggrdecl*_T3D;struct Cyc_Absyn_AggrdeclImpl*_T3E;struct Cyc_Absyn_AggrdeclImpl*_T3F;struct Cyc_List_List*_T40;struct _tuple0*_T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;void*_T44;struct _tuple0*_T45;struct _tuple0 _T46;void*_T47;struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;void*_T4A;struct _tuple0*_T4B;struct _tuple0 _T4C;void*_T4D;struct Cyc_List_List*_T4E;void**_T4F;int*_T50;unsigned _T51;struct Cyc_Absyn_Vardecl*_T52;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T53;void*_T54;int*_T55;int _T56;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T57;void*_T58;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T59;union Cyc_Absyn_AggrInfo _T5A;struct _union_AggrInfo_KnownAggr _T5B;unsigned _T5C;void*_T5D;union Cyc_Absyn_AggrInfo _T5E;struct _union_AggrInfo_KnownAggr _T5F;struct Cyc_Absyn_Aggrdecl**_T60;struct Cyc_Absyn_Aggrdecl*_T61;struct Cyc_Absyn_Aggrdecl**_T62;struct Cyc_Absyn_Aggrdecl*_T63;struct Cyc_List_List*_T64;void*_T65;struct Cyc_List_List*_T66;void*_T67;struct Cyc_List_List*_T68;struct Cyc_List_List*_T69;union Cyc_Absyn_AggrInfo _T6A;struct Cyc_List_List*_T6B;union Cyc_Absyn_AggrInfo _T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List**_T6E;unsigned _T6F;struct Cyc_Tcenv_Tenv*_T70;struct Cyc_List_List*_T71;struct Cyc_Absyn_Kind*_T72;struct Cyc_Absyn_Kind*_T73;struct Cyc_List_List*_T74;void*_T75;unsigned _T76;struct Cyc_List_List*_T77;void*_T78;struct Cyc_List_List*_T79;void*_T7A;struct Cyc_List_List*_T7B;void*_T7C;int _T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;unsigned _T7F;struct _fat_ptr _T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;struct Cyc_Warn_String_Warn_Warg_struct _T83;unsigned _T84;struct _fat_ptr _T85;struct Cyc_List_List**_T86;struct _RegionHandle*_T87;unsigned _T88;struct Cyc_List_List*_T89;struct Cyc_Absyn_Aggrdecl*_T8A;enum Cyc_Absyn_AggrKind _T8B;struct Cyc_Absyn_Aggrdecl*_T8C;struct Cyc_Absyn_AggrdeclImpl*_T8D;struct Cyc_Absyn_AggrdeclImpl*_T8E;struct Cyc_List_List*_T8F;struct Cyc_Tcenv_Tenv*_T90;struct Cyc_Tcenv_Fenv*_T91;struct Cyc_Absyn_Aggrdecl*_T92;enum Cyc_Absyn_AggrKind _T93;int _T94;struct Cyc_List_List*_T95;void*_T96;struct Cyc_Absyn_Aggrdecl*_T97;struct Cyc_Absyn_AggrdeclImpl*_T98;struct Cyc_Absyn_AggrdeclImpl*_T99;struct Cyc_Absyn_Aggrfield*_T9A;struct Cyc_List_List*_T9B;void*_T9C;struct Cyc_Absyn_Aggrfield*_T9D;struct Cyc_Warn_String_Warn_Warg_struct _T9E;unsigned _T9F;struct _fat_ptr _TA0;struct Cyc_List_List*_TA1;void*_TA2;struct Cyc_List_List*_TA3;struct Cyc_Absyn_Aggrfield*_TA4;void*_TA5;struct Cyc_Tcenv_Tenv*_TA6;void**_TA7;struct Cyc_Absyn_Exp*_TA8;struct Cyc_List_List*_TA9;struct Cyc_Absyn_Exp*_TAA;void*_TAB;int*_TAC;int _TAD;struct Cyc_Warn_String_Warn_Warg_struct _TAE;struct Cyc_Warn_String_Warn_Warg_struct _TAF;struct Cyc_Absyn_Aggrfield*_TB0;struct _fat_ptr*_TB1;struct Cyc_Warn_String_Warn_Warg_struct _TB2;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TB3;struct Cyc_Warn_String_Warn_Warg_struct _TB4;struct Cyc_Warn_Typ_Warn_Warg_struct _TB5;struct Cyc_Warn_String_Warn_Warg_struct _TB6;struct Cyc_Warn_Typ_Warn_Warg_struct _TB7;struct Cyc_Absyn_Exp*_TB8;unsigned _TB9;struct _fat_ptr _TBA;int(*_TBB)(void*,void*);struct Cyc_Absyn_Aggrfield*_TBC;struct Cyc_List_List*_TBD;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_TBE;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_TBF;void*_TC0;int _TC1;int _TC2;struct Cyc_Warn_String_Warn_Warg_struct _TC3;struct Cyc_Warn_String_Warn_Warg_struct _TC4;struct Cyc_Absyn_Aggrfield*_TC5;struct _fat_ptr*_TC6;struct Cyc_Warn_String_Warn_Warg_struct _TC7;struct Cyc_Absyn_Exp*_TC8;unsigned _TC9;struct _fat_ptr _TCA;struct Cyc_List_List*_TCB;struct _RegionHandle*_TCC;struct Cyc_List_List*_TCD;struct Cyc_Absyn_Aggrdecl*_TCE;struct Cyc_Absyn_AggrdeclImpl*_TCF;struct Cyc_Absyn_AggrdeclImpl*_TD0;struct Cyc_List_List*_TD1;struct Cyc_List_List*_TD2;int*_TD3;unsigned _TD4;void*_TD5;void*_TD6;struct Cyc_List_List*_TD7;void*_TD8;int _TD9;int _TDA;struct Cyc_Warn_String_Warn_Warg_struct _TDB;unsigned _TDC;struct _fat_ptr _TDD;void*_TDE;struct Cyc_List_List*_TDF;struct Cyc_List_List*_TE0;int(*_TE1)(struct _fat_ptr,struct _fat_ptr);void*(*_TE2)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TE3;struct _fat_ptr _TE4;struct Cyc_List_List*_TE5;void*_TE6;
# 2100
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;_T0=ad_opt;_T1=*_T0;
if(_T1==0)goto _TL3B2;_T2=ad_opt;
ad=*_T2;{struct Cyc_Absyn_Aggrdecl**_TE7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_TE7=ad;_T3=(struct Cyc_Absyn_Aggrdecl**)_TE7;}adptr=_T3;goto _TL3B3;
# 2106
_TL3B2:{struct _handler_cons _TE7;_T4=& _TE7;_push_handler(_T4);{int _TE8=0;_T5=setjmp(_TE7.handler);if(!_T5)goto _TL3B4;_TE8=1;goto _TL3B5;_TL3B4: _TL3B5: if(_TE8)goto _TL3B6;else{goto _TL3B8;}_TL3B8: _T6=te;_T7=loc;_T8=tn;_T9=*_T8;adptr=Cyc_Tcenv_lookup_aggrdecl(_T6,_T7,_T9);_TA=adptr;
ad=*_TA;_pop_handler();goto _TL3B7;_TL3B6: _TB=Cyc_Core_get_exn_thrown();{void*_TE9=(void*)_TB;void*_TEA;_TC=(struct Cyc_Dict_Absent_exn_struct*)_TE9;_TD=_TC->tag;_TE=Cyc_Dict_Absent;if(_TD!=_TE)goto _TL3B9;{struct Cyc_Warn_String_Warn_Warg_struct _TEB;_TEB.tag=0;
# 2109
_TEB.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_TF=_TEB;}{struct Cyc_Warn_String_Warn_Warg_struct _TEB=_TF;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TEC;_TEC.tag=1;_T11=tn;_TEC.f1=*_T11;_T10=_TEC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TEC=_T10;void*_TED[2];_TED[0]=& _TEB;_TED[1]=& _TEC;_T12=loc;_T13=_tag_fat(_TED,sizeof(void*),2);Cyc_Warn_err2(_T12,_T13);}}
if(topt==0)goto _TL3BB;_T15=topt;_T14=*_T15;goto _TL3BC;_TL3BB: _T14=Cyc_Absyn_void_type;_TL3BC: return _T14;_TL3B9: _TEA=_TE9;{void*exn=_TEA;_rethrow(exn);};}_TL3B7:;}}_T16=ad_opt;
# 2112
*_T16=ad;_T17=tn;_T18=ad;
*_T17=_T18->name;_TL3B3: _T19=ad;_T1A=_T19->impl;
# 2115
if(_T1A!=0)goto _TL3BD;{struct Cyc_Warn_String_Warn_Warg_struct _TE7;_TE7.tag=0;
_TE7.f1=_tag_fat("can't construct abstract ",sizeof(char),26U);_T1C=_TE7;}{struct Cyc_Warn_String_Warn_Warg_struct _TE7=_T1C;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TE8;_TE8.tag=6;_TE8.f1=ad;_T1D=_TE8;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TE8=_T1D;void*_TE9[2];_TE9[0]=& _TE7;_TE9[1]=& _TE8;_T1E=te;_T1F=loc;_T20=_tag_fat(_TE9,sizeof(void*),2);_T1B=Cyc_Tcexp_expr_err(_T1E,_T1F,0,_T20);}}return _T1B;_TL3BD: _T22=
# 2122
Cyc_Tcenv_new_status(te);_T23=(int)_T22;if(_T23!=1)goto _TL3BF;_T21=Cyc_Tcenv_set_new_status(2U,te);goto _TL3C0;_TL3BF: _T21=te;_TL3C0: {struct Cyc_Tcenv_Tenv*te2=_T21;{struct _tuple13 _TE7;
# 2128
_TE7.f0=Cyc_Tcenv_lookup_type_vars(te2);_TE7.f1=Cyc_Core_heap_region;_T24=_TE7;}{struct _tuple13 env=_T24;_T26=Cyc_List_map_c;{
struct Cyc_List_List*(*_TE7)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T26;_T25=_TE7;}_T27=Cyc_Tcutil_r_make_inst_var;_T28=& env;_T29=ad;_T2A=_T29->tvs;{struct Cyc_List_List*all_inst=_T25(_T27,_T28,_T2A);_T2C=Cyc_List_map_c;{
struct Cyc_List_List*(*_TE7)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T2C;_T2B=_TE7;}_T2D=Cyc_Tcutil_r_make_inst_var;_T2E=& env;_T2F=ad;_T30=_T2F->impl;_T31=_check_null(_T30);_T32=_T31->exist_vars;{struct Cyc_List_List*exist_inst=_T2B(_T2D,_T2E,_T32);_T34=Cyc_List_map;{
struct Cyc_List_List*(*_TE7)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T34;_T33=_TE7;}_T36=Cyc_Core_snd;{void*(*_TE7)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T36;_T35=_TE7;}_T37=all_inst;{struct Cyc_List_List*all_typs=_T33(_T35,_T37);_T39=Cyc_List_map;{
struct Cyc_List_List*(*_TE7)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T39;_T38=_TE7;}_T3B=Cyc_Core_snd;{void*(*_TE7)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T3B;_T3A=_TE7;}_T3C=exist_inst;{struct Cyc_List_List*exist_typs=_T38(_T3A,_T3C);
struct Cyc_List_List*inst=Cyc_List_append(all_inst,exist_inst);
struct Cyc_List_List*qb_inst=0;_T3D=ad;_T3E=_T3D->impl;_T3F=
_check_null(_T3E);{struct Cyc_List_List*qbit=_T3F->qual_bnd;_TL3C4: if(qbit!=0)goto _TL3C2;else{goto _TL3C3;}
_TL3C2:{struct Cyc_List_List*_TE7=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TE8=_cycalloc(sizeof(struct _tuple0));_T42=inst;_T43=qbit;_T44=_T43->hd;_T45=(struct _tuple0*)_T44;_T46=*_T45;_T47=_T46.f0;_TE8->f0=Cyc_Tcutil_substitute(_T42,_T47);_T48=inst;_T49=qbit;_T4A=_T49->hd;_T4B=(struct _tuple0*)_T4A;_T4C=*_T4B;_T4D=_T4C.f1;
_TE8->f1=Cyc_Tcutil_substitute(_T48,_T4D);_T41=(struct _tuple0*)_TE8;}
# 2136
_TE7->hd=_T41;
# 2138
_TE7->tl=qb_inst;_T40=(struct Cyc_List_List*)_TE7;}
# 2136
qb_inst=_T40;_T4E=qbit;
# 2135
qbit=_T4E->tl;goto _TL3C4;_TL3C3:;}{
# 2139
void*res_typ;
# 2148 "tcexp.cyc"
if(topt==0)goto _TL3C5;_T4F=topt;{
void*tp=*_T4F;
loop: {
void*_TE7=Cyc_Absyn_compress(tp);struct Cyc_List_List*_TE8;struct Cyc_Absyn_Aggrdecl**_TE9;struct Cyc_Absyn_Vardecl*_TEA;_T50=(int*)_TE7;_T51=*_T50;switch(_T51){case 12:{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_TEB=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TE7;_TEA=_TEB->f1;}{struct Cyc_Absyn_Vardecl*vd=_TEA;_T52=vd;
tp=_T52->type;goto loop;}case 0: _T53=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE7;_T54=_T53->f1;_T55=(int*)_T54;_T56=*_T55;if(_T56!=24)goto _TL3C8;_T57=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE7;_T58=_T57->f1;_T59=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T58;_T5A=_T59->f1;_T5B=_T5A.KnownAggr;_T5C=_T5B.tag;if(_T5C!=2)goto _TL3CA;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TEB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE7;_T5D=_TEB->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TEC=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T5D;_T5E=_TEC->f1;_T5F=_T5E.KnownAggr;_TE9=_T5F.val;}_TE8=_TEB->f2;}{struct Cyc_Absyn_Aggrdecl**adptr2=_TE9;struct Cyc_List_List*all_typs2=_TE8;_T60=adptr2;_T61=*_T60;_T62=adptr;_T63=*_T62;
# 2154
if(_T61==_T63)goto _TL3CC;goto _LLB;_TL3CC:{
# 2156
struct Cyc_List_List*ats=all_typs;_TL3D1: if(ats!=0)goto _TL3D2;else{goto _TL3D0;}_TL3D2: if(all_typs2!=0)goto _TL3CF;else{goto _TL3D0;}
# 2158
_TL3CF: _T64=ats;_T65=_T64->hd;_T66=all_typs2;_T67=_T66->hd;Cyc_Unify_unify(_T65,_T67);_T68=ats;
# 2157
ats=_T68->tl;_T69=all_typs2;all_typs2=_T69->tl;goto _TL3D1;_TL3D0:;}
# 2160
res_typ=tp;goto _LL5;}_TL3CA: goto _LLA;_TL3C8: goto _LLA;default: _LLA: _LLB: _T6A=
# 2162
Cyc_Absyn_KnownAggr(adptr);_T6B=all_typs;res_typ=Cyc_Absyn_aggr_type(_T6A,_T6B);}_LL5:;}}goto _TL3C6;
# 2165
_TL3C5: _T6C=Cyc_Absyn_KnownAggr(adptr);_T6D=all_typs;res_typ=Cyc_Absyn_aggr_type(_T6C,_T6D);_TL3C6: _T6E=ts;{
# 2168
struct Cyc_List_List*user_ex_ts=*_T6E;
struct Cyc_List_List*ex_ts=exist_typs;
_TL3D3: if(user_ex_ts!=0)goto _TL3D6;else{goto _TL3D5;}_TL3D6: if(ex_ts!=0)goto _TL3D4;else{goto _TL3D5;}
# 2172
_TL3D4: _T6F=loc;_T70=te2;_T71=Cyc_Tcenv_lookup_type_vars(te2);_T72=& Cyc_Kinds_ak;_T73=(struct Cyc_Absyn_Kind*)_T72;_T74=user_ex_ts;_T75=_T74->hd;Cyc_Tctyp_check_type(_T6F,_T70,_T71,_T73,1,0,_T75);_T76=loc;_T77=user_ex_ts;_T78=_T77->hd;
Cyc_Tcutil_check_no_qual(_T76,_T78);_T79=user_ex_ts;_T7A=_T79->hd;_T7B=ex_ts;_T7C=_T7B->hd;_T7D=
Cyc_Unify_unify(_T7A,_T7C);if(_T7D)goto _TL3D7;else{goto _TL3D9;}
_TL3D9:{struct Cyc_Warn_String_Warn_Warg_struct _TE7;_TE7.tag=0;_TE7.f1=_tag_fat("bad instantiation of existential type",sizeof(char),38U);_T7E=_TE7;}{struct Cyc_Warn_String_Warn_Warg_struct _TE7=_T7E;void*_TE8[1];_TE8[0]=& _TE7;_T7F=loc;_T80=_tag_fat(_TE8,sizeof(void*),1);Cyc_Warn_err2(_T7F,_T80);}
Cyc_Unify_explain_failure();goto _TL3D8;_TL3D7: _TL3D8: _T81=user_ex_ts;
# 2178
user_ex_ts=_T81->tl;_T82=ex_ts;
ex_ts=_T82->tl;goto _TL3D3;_TL3D5:
# 2181
 if(user_ex_ts==0)goto _TL3DA;{struct Cyc_Warn_String_Warn_Warg_struct _TE7;_TE7.tag=0;
_TE7.f1=_tag_fat("too many explicit witness types",sizeof(char),32U);_T83=_TE7;}{struct Cyc_Warn_String_Warn_Warg_struct _TE7=_T83;void*_TE8[1];_TE8[0]=& _TE7;_T84=loc;_T85=_tag_fat(_TE8,sizeof(void*),1);Cyc_Warn_err2(_T84,_T85);}goto _TL3DB;_TL3DA: _TL3DB: _T86=ts;
# 2184
*_T86=exist_typs;_T87=Cyc_Core_heap_region;_T88=loc;_T89=args;_T8A=ad;_T8B=_T8A->kind;_T8C=ad;_T8D=_T8C->impl;_T8E=
# 2188
_check_null(_T8D);_T8F=_T8E->fields;{
# 2187
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(_T87,_T88,_T89,_T8B,_T8F);_T90=te;_T91=_T90->le;
# 2191
if(_T91!=0)goto _TL3DC;_T92=ad;_T93=_T92->kind;_T94=(int)_T93;if(_T94!=1)goto _TL3DC;_T95=
_check_null(fields);_T96=_T95->hd;{struct _tuple23*_TE7=(struct _tuple23*)_T96;struct Cyc_Absyn_Aggrfield*_TE8;{struct _tuple23 _TE9=*_TE7;_TE8=_TE9.f0;}{struct Cyc_Absyn_Aggrfield*f=_TE8;_T97=ad;_T98=_T97->impl;_T99=
_check_null(_T98);{struct Cyc_List_List*fs=_T99->fields;
if(fs==0)goto _TL3E0;else{goto _TL3E1;}_TL3E1: _T9A=f;_T9B=fs;_T9C=_T9B->hd;_T9D=(struct Cyc_Absyn_Aggrfield*)_T9C;if(_T9A!=_T9D)goto _TL3E0;else{goto _TL3DE;}
_TL3E0:{struct Cyc_Warn_String_Warn_Warg_struct _TE9;_TE9.tag=0;_TE9.f1=_tag_fat("you can only initialize a union at top-level with the first member",sizeof(char),67U);_T9E=_TE9;}{struct Cyc_Warn_String_Warn_Warg_struct _TE9=_T9E;void*_TEA[1];_TEA[0]=& _TE9;_T9F=loc;_TA0=_tag_fat(_TEA,sizeof(void*),1);Cyc_Warn_err2(_T9F,_TA0);}goto _TL3DF;_TL3DE: _TL3DF:;}}}goto _TL3DD;_TL3DC: _TL3DD:
# 2198
 _TL3E5: if(fields!=0)goto _TL3E3;else{goto _TL3E4;}
_TL3E3:{int bogus=0;_TA1=fields;_TA2=_TA1->hd;{
struct _tuple23*_TE7=(struct _tuple23*)_TA2;struct Cyc_Absyn_Exp*_TE8;struct Cyc_Absyn_Aggrfield*_TE9;{struct _tuple23 _TEA=*_TE7;_TE9=_TEA.f0;_TE8=_TEA.f1;}{struct Cyc_Absyn_Aggrfield*field=_TE9;struct Cyc_Absyn_Exp*field_exp=_TE8;_TA3=inst;_TA4=field;_TA5=_TA4->type;{
void*inst_fieldtyp=Cyc_Tcutil_substitute(_TA3,_TA5);_TA6=
Cyc_Tcenv_clear_abstract_val_ok(te2);_TA7=& inst_fieldtyp;_TA8=field_exp;{void*ftyp=Cyc_Tcexp_tcExpInitializer(_TA6,_TA7,_TA8);_TA9=
# 2204
Cyc_Tcenv_curr_aquals_bounds(te);_TAA=field_exp;_TAB=inst_fieldtyp;_TAC=& bogus;_TAD=Cyc_Tcutil_coerce_arg(_TA9,_TAA,_TAB,_TAC);if(_TAD)goto _TL3E6;else{goto _TL3E8;}
_TL3E8: Cyc_Unify_unify(ftyp,inst_fieldtyp);{struct Cyc_Warn_String_Warn_Warg_struct _TEA;_TEA.tag=0;
_TEA.f1=_tag_fat("field ",sizeof(char),7U);_TAE=_TEA;}{struct Cyc_Warn_String_Warn_Warg_struct _TEA=_TAE;{struct Cyc_Warn_String_Warn_Warg_struct _TEB;_TEB.tag=0;_TB0=field;_TB1=_TB0->name;_TEB.f1=*_TB1;_TAF=_TEB;}{struct Cyc_Warn_String_Warn_Warg_struct _TEB=_TAF;{struct Cyc_Warn_String_Warn_Warg_struct _TEC;_TEC.tag=0;_TEC.f1=_tag_fat(" of ",sizeof(char),5U);_TB2=_TEC;}{struct Cyc_Warn_String_Warn_Warg_struct _TEC=_TB2;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TED;_TED.tag=6;_TED.f1=ad;_TB3=_TED;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TED=_TB3;{struct Cyc_Warn_String_Warn_Warg_struct _TEE;_TEE.tag=0;
_TEE.f1=_tag_fat(" expects type ",sizeof(char),15U);_TB4=_TEE;}{struct Cyc_Warn_String_Warn_Warg_struct _TEE=_TB4;{struct Cyc_Warn_Typ_Warn_Warg_struct _TEF;_TEF.tag=2;_TEF.f1=inst_fieldtyp;_TB5=_TEF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TEF=_TB5;{struct Cyc_Warn_String_Warn_Warg_struct _TF0;_TF0.tag=0;_TF0.f1=_tag_fat(" != ",sizeof(char),5U);_TB6=_TF0;}{struct Cyc_Warn_String_Warn_Warg_struct _TF0=_TB6;{struct Cyc_Warn_Typ_Warn_Warg_struct _TF1;_TF1.tag=2;
_TF1.f1=ftyp;_TB7=_TF1;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TF1=_TB7;void*_TF2[8];_TF2[0]=& _TEA;_TF2[1]=& _TEB;_TF2[2]=& _TEC;_TF2[3]=& _TED;_TF2[4]=& _TEE;_TF2[5]=& _TEF;_TF2[6]=& _TF0;_TF2[7]=& _TF1;_TB8=field_exp;_TB9=_TB8->loc;_TBA=_tag_fat(_TF2,sizeof(void*),8);Cyc_Tcexp_err_and_explain(_TB9,_TBA);}}}}}}}}goto _TL3E7;_TL3E6: _TL3E7: _TBB=Cyc_Atts_attribute_cmp;_TBC=field;_TBD=_TBC->attributes;_TBE=& Cyc_Atts_No_throw_att_val;_TBF=(struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*)_TBE;_TC0=(void*)_TBF;{
# 2210
int nothrow_fld=Cyc_List_mem(_TBB,_TBD,_TC0);_TC1=nothrow_fld;
# 2213
if(!_TC1)goto _TL3E9;_TC2=Cyc_Tcexp_ensure_nothrow_initializer(field_exp);if(_TC2)goto _TL3E9;else{goto _TL3EB;}
_TL3EB:{struct Cyc_Warn_String_Warn_Warg_struct _TEA;_TEA.tag=0;_TEA.f1=_tag_fat("field ",sizeof(char),7U);_TC3=_TEA;}{struct Cyc_Warn_String_Warn_Warg_struct _TEA=_TC3;{struct Cyc_Warn_String_Warn_Warg_struct _TEB;_TEB.tag=0;_TC5=field;_TC6=_TC5->name;_TEB.f1=*_TC6;_TC4=_TEB;}{struct Cyc_Warn_String_Warn_Warg_struct _TEB=_TC4;{struct Cyc_Warn_String_Warn_Warg_struct _TEC;_TEC.tag=0;_TEC.f1=_tag_fat(" can only hold a no_throw function",sizeof(char),35U);_TC7=_TEC;}{struct Cyc_Warn_String_Warn_Warg_struct _TEC=_TC7;void*_TED[3];_TED[0]=& _TEA;_TED[1]=& _TEB;_TED[2]=& _TEC;_TC8=field_exp;_TC9=_TC8->loc;_TCA=_tag_fat(_TED,sizeof(void*),3);Cyc_Warn_err2(_TC9,_TCA);}}}goto _TL3EA;_TL3E9: _TL3EA:;}}}}}}_TCB=fields;
# 2198
fields=_TCB->tl;goto _TL3E5;_TL3E4: {
# 2217
struct Cyc_List_List*constraint_checks=0;_TCC=Cyc_Core_heap_region;_TCD=inst;_TCE=ad;_TCF=_TCE->impl;_TD0=
_check_null(_TCF);_TD1=_TD0->effconstr;{struct Cyc_List_List*effc=Cyc_Tcutil_rsubst_effconstr(_TCC,_TCD,_TD1);{
struct Cyc_List_List*ec=effc;_TL3EF: if(ec!=0)goto _TL3ED;else{goto _TL3EE;}
_TL3ED: _TD2=ec;{void*_TE7=_TD2->hd;void*_TE8;void*_TE9;_TD3=(int*)_TE7;_TD4=*_TD3;switch(_TD4){case 2:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TEA=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_TE7;_TD5=_TEA->f1;_TE9=(void*)_TD5;_TD6=_TEA->f2;_TE8=(void*)_TD6;}{void*e1s=_TE9;void*e2s=_TE8;_TD7=
# 2222
Cyc_Tcenv_curr_effect_constraints(te);_TD8=e1s;{void*ev1=Cyc_Tcutil_evaluate_effect(_TD7,_TD8);_TD9=
Cyc_Unify_unify(ev1,e2s);if(_TD9)goto _TL3F1;else{goto _TL3F3;}_TL3F3: _TDA=
Cyc_Tcutil_subset_effect(0,ev1,e2s);
# 2223
if(_TDA)goto _TL3F1;else{goto _TL3F4;}
# 2225
_TL3F4:{struct Cyc_Warn_String_Warn_Warg_struct _TEA;_TEA.tag=0;_TEA.f1=_tag_fat("Effect equality constraint not satisfied",sizeof(char),41U);_TDB=_TEA;}{struct Cyc_Warn_String_Warn_Warg_struct _TEA=_TDB;void*_TEB[1];_TEB[0]=& _TEA;_TDC=loc;_TDD=_tag_fat(_TEB,sizeof(void*),1);Cyc_Warn_err2(_TDC,_TDD);}goto _TL3F2;_TL3F1: _TL3F2: goto _LL12;}}case 0:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TEA=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_TE7;_TDE=_TEA->f1;_TE9=(void*)_TDE;}{void*e1=_TE9;{struct Cyc_List_List*_TEA=_cycalloc(sizeof(struct Cyc_List_List));_TE0=ec;
# 2229
_TEA->hd=_TE0->hd;_TEA->tl=constraint_checks;_TDF=(struct Cyc_List_List*)_TEA;}constraint_checks=_TDF;goto _LL12;}default: _TE2=Cyc_Warn_impos;{
# 2232
int(*_TEA)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE2;_TE1=_TEA;}_TE3=_tag_fat("Disjoint constraints in aggregates are not supported yet",sizeof(char),57U);_TE4=_tag_fat(0U,sizeof(void*),0);_TE1(_TE3,_TE4);}_LL12:;}_TE5=ec;
# 2219
ec=_TE5->tl;goto _TL3EF;_TL3EE:;}
# 2235
Cyc_Tcenv_check_effect_constraints(te,loc,constraint_checks);
# 2244 "tcexp.cyc"
Cyc_Tcexp_check_all_aqual_bounds(loc,te2,qb_inst);_TE6=res_typ;
return _TE6;}}}}}}}}}}}}
# 2248
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,void**ts,int is_tuple,struct Cyc_List_List*args){void**_T0;void*_T1;void**_T2;void**_T3;struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;int*_T7;unsigned _T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;int*_TC;int _TD;void*_TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_UnknownAggr _T10;unsigned _T11;union Cyc_Absyn_AggrInfo _T12;struct _union_AggrInfo_UnknownAggr _T13;struct _tuple3 _T14;union Cyc_Absyn_AggrInfo _T15;struct _union_AggrInfo_KnownAggr _T16;struct Cyc_Absyn_Aggrdecl**_T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T1A;struct Cyc_List_List*_T1B;void*_T1C;struct _tuple18*_T1D;int(*_T1E)(struct _fat_ptr,struct _fat_ptr);void*(*_T1F)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_List_List*_T22;int*_T23;int _T24;int(*_T25)(struct _fat_ptr,struct _fat_ptr);void*(*_T26)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc_Absyn_Aggrfield*_T29;struct Cyc_Absyn_Aggrfield*_T2A;struct Cyc_Absyn_Aggrfield*_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Core_Opt*_T2D;struct Cyc_Core_Opt*_T2E;struct Cyc_Absyn_Aggrfield*_T2F;struct Cyc_Absyn_Aggrfield*_T30;struct Cyc_Absyn_Aggrfield*_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;void*_T34;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T35;void*_T36;void**_T37;enum Cyc_Absyn_AggrKind _T38;int _T39;struct Cyc_Warn_String_Warn_Warg_struct _T3A;unsigned _T3B;struct _fat_ptr _T3C;struct Cyc_List_List*_T3D;void*_T3E;struct Cyc_Tcenv_Tenv*_T3F;struct Cyc_Absyn_Aggrfield*_T40;void**_T41;struct Cyc_Absyn_Exp*_T42;struct Cyc_List_List*_T43;struct Cyc_Absyn_Exp*_T44;struct Cyc_Absyn_Aggrfield*_T45;void*_T46;int*_T47;int _T48;struct Cyc_Warn_String_Warn_Warg_struct _T49;struct Cyc_Warn_String_Warn_Warg_struct _T4A;struct Cyc_Absyn_Aggrfield*_T4B;struct _fat_ptr*_T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;struct Cyc_Warn_Typ_Warn_Warg_struct _T4E;struct Cyc_Absyn_Aggrfield*_T4F;struct Cyc_Warn_String_Warn_Warg_struct _T50;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T51;struct Cyc_Absyn_Exp*_T52;struct Cyc_Absyn_Exp*_T53;unsigned _T54;struct _fat_ptr _T55;struct Cyc_List_List*_T56;void**_T57;void*_T58;void**_T59;void*_T5A;void*_T5B;struct Cyc_Warn_String_Warn_Warg_struct _T5C;struct Cyc_Warn_Typ_Warn_Warg_struct _T5D;struct Cyc_Warn_String_Warn_Warg_struct _T5E;struct Cyc_Warn_Exp_Warn_Warg_struct _T5F;int(*_T60)(struct _fat_ptr);void*(*_T61)(struct _fat_ptr);struct _fat_ptr _T62;void*_T63;
# 2252
void*struct_type;_T0=ts;_T1=*_T0;
if(_T1==0)goto _TL3F5;_T2=ts;struct_type=*_T2;goto _TL3F6;
_TL3F5: if(topt==0)goto _TL3F7;_T3=topt;struct_type=*_T3;goto _TL3F8;
_TL3F7: _T4=& Cyc_Kinds_mko;_T5=(struct Cyc_Core_Opt*)_T4;_T6=Cyc_Tcenv_lookup_opt_type_vars(te);struct_type=Cyc_Absyn_new_evar(_T5,_T6);_TL3F8: _TL3F6:
# 2257
 loop:{
void*_T64=Cyc_Absyn_compress(struct_type);enum Cyc_Absyn_AggrKind _T65;struct Cyc_List_List*_T66;union Cyc_Absyn_AggrInfo _T67;struct Cyc_Absyn_Vardecl*_T68;_T7=(int*)_T64;_T8=*_T7;switch(_T8){case 12:{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T64;_T68=_T69->f1;}{struct Cyc_Absyn_Vardecl*vd=_T68;_T9=vd;
struct_type=_T9->type;goto loop;}case 0: _TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T64;_TB=_TA->f1;_TC=(int*)_TB;_TD=*_TC;if(_TD!=24)goto _TL3FA;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T64;_TE=_T69->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6A=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TE;_T67=_T6A->f1;}_T66=_T69->f2;}{union Cyc_Absyn_AggrInfo ainfo=_T67;struct Cyc_List_List*ts=_T66;
# 2262
struct _tuple1*name;
struct Cyc_Absyn_Aggrdecl*adopt=0;{struct Cyc_Absyn_Aggrdecl*_T69;struct _tuple1*_T6A;_TF=ainfo;_T10=_TF.UnknownAggr;_T11=_T10.tag;if(_T11!=1)goto _TL3FC;_T12=ainfo;_T13=_T12.UnknownAggr;_T14=_T13.val;_T6A=_T14.f1;{struct _tuple1*n=_T6A;
# 2265
name=n;goto _LLB;}_TL3FC: _T15=ainfo;_T16=_T15.KnownAggr;_T17=_T16.val;{struct Cyc_Absyn_Aggrdecl*_T6B=*_T17;_T69=_T6B;}{struct Cyc_Absyn_Aggrdecl*ad=_T69;_T18=ad;
name=_T18->name;adopt=ad;goto _LLB;}_LLB:;}_T19=orig;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T69->tag=28;
# 2268
_T69->f1=name;_T69->f2=ts;_T69->f3=args;_T69->f4=adopt;_T1A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T69;}_T19->r=(void*)_T1A;
Cyc_Tcexp_tcExpNoInst(te,topt,orig);goto _LL0;}_TL3FA: goto _LL9;case 1:  {
# 2274
struct Cyc_List_List*fs=0;
struct Cyc_Core_Opt*tvs=Cyc_Tcenv_lookup_opt_type_vars(te);
int i=0;{
struct Cyc_List_List*a=args;_TL401: if(a!=0)goto _TL3FF;else{goto _TL400;}
_TL3FF: _T1B=a;_T1C=_T1B->hd;_T1D=(struct _tuple18*)_T1C;{struct _tuple18 _T69=*_T1D;struct Cyc_List_List*_T6A;_T6A=_T69.f0;{struct Cyc_List_List*ds=_T6A;
if(ds!=0)goto _TL402;_T1F=Cyc_Warn_impos;{int(*_T6B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1F;_T1E=_T6B;}_T20=_tag_fat("tcAnonStruct -- missing designators!",sizeof(char),37U);_T21=_tag_fat(0U,sizeof(void*),0);_T1E(_T20,_T21);goto _TL403;_TL402: _TL403: {
struct _fat_ptr*n;_T22=ds;{
void*_T6B=_T22->hd;struct _fat_ptr*_T6C;_T23=(int*)_T6B;_T24=*_T23;if(_T24!=1)goto _TL404;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T6D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T6B;_T6C=_T6D->f1;}{struct _fat_ptr*name=_T6C;
n=name;goto _LL13;}_TL404: _T26=Cyc_Warn_impos;{
int(*_T6D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T26;_T25=_T6D;}_T27=_tag_fat("tcAnonStruct -- designator is array!",sizeof(char),37U);_T28=_tag_fat(0U,sizeof(void*),0);_T25(_T27,_T28);_LL13:;}{
# 2285
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_T29=f;_T29->name=n;_T2A=f;_T2A->tq=Cyc_Absyn_empty_tqual(0U);_T2B=f;_T2C=& Cyc_Kinds_mko;_T2D=(struct Cyc_Core_Opt*)_T2C;_T2E=tvs;
_T2B->type=Cyc_Absyn_new_evar(_T2D,_T2E);_T2F=f;
_T2F->width=0;_T30=f;_T30->attributes=0;_T31=f;_T31->requires_clause=0;{struct Cyc_List_List*_T6B=_cycalloc(sizeof(struct Cyc_List_List));
_T6B->hd=f;_T6B->tl=fs;_T32=(struct Cyc_List_List*)_T6B;}fs=_T32;}}}}_T33=a;
# 2277
a=_T33->tl;i=i + 1;goto _TL401;_TL400:;}
# 2290
fs=Cyc_List_imp_rev(fs);_T34=struct_type;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T69=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T69->tag=7;
_T69->f1=0U;_T69->f2=is_tuple;_T69->f3=fs;_T35=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T69;}_T36=(void*)_T35;Cyc_Unify_unify(_T34,_T36);_T37=ts;
*_T37=struct_type;_T65=0U;_T66=fs;goto _LL8;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T64;_T65=_T69->f1;_T66=_T69->f3;}_LL8: {enum Cyc_Absyn_AggrKind k=_T65;struct Cyc_List_List*fs=_T66;_T38=k;_T39=(int)_T38;
# 2295
if(_T39!=1)goto _TL406;{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;
_T69.f1=_tag_fat("expecting struct but found union",sizeof(char),33U);_T3A=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T3A;void*_T6A[1];_T6A[0]=& _T69;_T3B=loc;_T3C=_tag_fat(_T6A,sizeof(void*),1);Cyc_Warn_err2(_T3B,_T3C);}goto _TL407;_TL406: _TL407: {
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,loc,args,0U,fs);
_TL40B: if(fields!=0)goto _TL409;else{goto _TL40A;}
_TL409:{int bogus=0;_T3D=fields;_T3E=_T3D->hd;{
struct _tuple23*_T69=(struct _tuple23*)_T3E;struct Cyc_Absyn_Exp*_T6A;struct Cyc_Absyn_Aggrfield*_T6B;{struct _tuple23 _T6C=*_T69;_T6B=_T6C.f0;_T6A=_T6C.f1;}{struct Cyc_Absyn_Aggrfield*field=_T6B;struct Cyc_Absyn_Exp*field_exp=_T6A;_T3F=
Cyc_Tcenv_clear_abstract_val_ok(te);_T40=field;_T41=& _T40->type;_T42=field_exp;Cyc_Tcexp_tcExpInitializer(_T3F,_T41,_T42);_T43=
Cyc_Tcenv_curr_aquals_bounds(te);_T44=field_exp;_T45=field;_T46=_T45->type;_T47=& bogus;_T48=Cyc_Tcutil_coerce_arg(_T43,_T44,_T46,_T47);if(_T48)goto _TL40C;else{goto _TL40E;}
_TL40E:{struct Cyc_Warn_String_Warn_Warg_struct _T6C;_T6C.tag=0;_T6C.f1=_tag_fat("field ",sizeof(char),7U);_T49=_T6C;}{struct Cyc_Warn_String_Warn_Warg_struct _T6C=_T49;{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;_T4B=field;_T4C=_T4B->name;_T6D.f1=*_T4C;_T4A=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T4A;{struct Cyc_Warn_String_Warn_Warg_struct _T6E;_T6E.tag=0;
_T6E.f1=_tag_fat(" of struct expects type ",sizeof(char),25U);_T4D=_T6E;}{struct Cyc_Warn_String_Warn_Warg_struct _T6E=_T4D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T6F;_T6F.tag=2;_T4F=field;_T6F.f1=_T4F->type;_T4E=_T6F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T6F=_T4E;{struct Cyc_Warn_String_Warn_Warg_struct _T70;_T70.tag=0;_T70.f1=_tag_fat(" != ",sizeof(char),5U);_T50=_T70;}{struct Cyc_Warn_String_Warn_Warg_struct _T70=_T50;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T71;_T71.tag=3;_T52=field_exp;
_T71.f1=_T52->topt;_T51=_T71;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T71=_T51;void*_T72[6];_T72[0]=& _T6C;_T72[1]=& _T6D;_T72[2]=& _T6E;_T72[3]=& _T6F;_T72[4]=& _T70;_T72[5]=& _T71;_T53=field_exp;_T54=_T53->loc;_T55=_tag_fat(_T72,sizeof(void*),6);Cyc_Tcexp_err_and_explain(_T54,_T55);}}}}}}goto _TL40D;_TL40C: _TL40D:;}}}_T56=fields;
# 2299
fields=_T56->tl;goto _TL40B;_TL40A: goto _LL0;}}default: _LL9: _T57=ts;_T58=*_T57;
# 2310
if(_T58!=0)goto _TL40F;if(topt==0)goto _TL40F;_T59=topt;_T5A=*_T59;if(_T5A==0)goto _TL40F;_T5B=
# 2312
Cyc_Tcexp_tcAnonStruct(te,loc,0,orig,ts,is_tuple,args);return _T5B;
# 2315
_TL40F:{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;_T69.f1=_tag_fat("tcAnonStruct: wrong type ",sizeof(char),26U);_T5C=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T5C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T6A;_T6A.tag=2;_T6A.f1=struct_type;_T5D=_T6A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T6A=_T5D;{struct Cyc_Warn_String_Warn_Warg_struct _T6B;_T6B.tag=0;_T6B.f1=_tag_fat(" for exp ",sizeof(char),10U);_T5E=_T6B;}{struct Cyc_Warn_String_Warn_Warg_struct _T6B=_T5E;{struct Cyc_Warn_Exp_Warn_Warg_struct _T6C;_T6C.tag=4;_T6C.f1=orig;_T5F=_T6C;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T6C=_T5F;void*_T6D[4];_T6D[0]=& _T69;_T6D[1]=& _T6A;_T6D[2]=& _T6B;_T6D[3]=& _T6C;_T61=Cyc_Warn_impos2;{int(*_T6E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T61;_T60=_T6E;}_T62=_tag_fat(_T6D,sizeof(void*),4);_T60(_T62);}}}};}_LL0:;}_T63=struct_type;
# 2317
return _T63;}
# 2321
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){struct _tuple13 _T0;struct Cyc_List_List*(*_T1)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T2)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T3)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T4;struct Cyc_Absyn_Datatypedecl*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*(*_T7)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T8)(void*(*)(void*),struct Cyc_List_List*);void*(*_T9)(struct _tuple14*);void*(*_TA)(struct _tuple0*);struct Cyc_List_List*_TB;union Cyc_Absyn_DatatypeFieldInfo _TC;struct Cyc_List_List*_TD;void**_TE;void*_TF;int*_T10;int _T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;void*_T13;int*_T14;int _T15;void**_T16;void*_T17;void*_T18;struct Cyc_Absyn_Datatypefield*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct _tuple19*_T1F;struct _tuple19 _T20;void*_T21;struct Cyc_Tcenv_Tenv*_T22;void**_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Exp*_T26;void*_T27;int*_T28;int _T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T2B;struct Cyc_Absyn_Datatypefield*_T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Typ_Warn_Warg_struct _T2E;struct Cyc_Warn_String_Warn_Warg_struct _T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;struct Cyc_Absyn_Exp*_T31;void*_T32;struct _fat_ptr _T33;struct Cyc_Absyn_Exp*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;unsigned _T37;struct _fat_ptr _T38;struct Cyc_List_List*_T39;struct Cyc_List_List*_T3A;void*_T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_Qvar_Warn_Warg_struct _T3D;struct Cyc_Absyn_Datatypefield*_T3E;struct Cyc_Tcenv_Tenv*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Absyn_Exp*_T42;unsigned _T43;void**_T44;struct _fat_ptr _T45;void*_T46;struct Cyc_Warn_String_Warn_Warg_struct _T47;struct Cyc_Warn_Qvar_Warn_Warg_struct _T48;struct Cyc_Absyn_Datatypefield*_T49;struct Cyc_Tcenv_Tenv*_T4A;unsigned _T4B;void**_T4C;struct _fat_ptr _T4D;void*_T4E;{struct _tuple13 _T4F;
# 2324
_T4F.f0=Cyc_Tcenv_lookup_type_vars(te);_T4F.f1=Cyc_Core_heap_region;_T0=_T4F;}{struct _tuple13 env=_T0;_T2=Cyc_List_map_c;{
struct Cyc_List_List*(*_T4F)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T2;_T1=_T4F;}_T3=Cyc_Tcutil_r_make_inst_var;_T4=& env;_T5=tud;_T6=_T5->tvs;{struct Cyc_List_List*inst=_T1(_T3,_T4,_T6);_T8=Cyc_List_map;{
struct Cyc_List_List*(*_T4F)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T8;_T7=_T4F;}_TA=Cyc_Core_snd;{void*(*_T4F)(struct _tuple14*)=(void*(*)(struct _tuple14*))_TA;_T9=_T4F;}_TB=inst;{struct Cyc_List_List*all_typs=_T7(_T9,_TB);_TC=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_TD=all_typs;{void*res=Cyc_Absyn_datatype_field_type(_TC,_TD);
# 2329
if(topt==0)goto _TL411;_TE=topt;_TF=*_TE;{
void*_T4F=Cyc_Absyn_compress(_TF);_T10=(int*)_T4F;_T11=*_T10;if(_T11!=0)goto _TL413;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4F;_T13=_T12->f1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=23)goto _TL415;_T16=topt;_T17=*_T16;_T18=res;
Cyc_Unify_unify(_T17,_T18);goto _LL0;_TL415: goto _LL3;_TL413: _LL3: goto _LL0;_LL0:;}goto _TL412;_TL411: _TL412: _T19=tuf;{
# 2335
struct Cyc_List_List*ts=_T19->typs;
_TL41A: if(es!=0)goto _TL41B;else{goto _TL419;}_TL41B: if(ts!=0)goto _TL418;else{goto _TL419;}
_TL418:{int bogus=0;_T1A=es;_T1B=_T1A->hd;{
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_T1B;_T1C=inst;_T1D=ts;_T1E=_T1D->hd;_T1F=(struct _tuple19*)_T1E;_T20=*_T1F;_T21=_T20.f1;{
void*t=Cyc_Tcutil_substitute(_T1C,_T21);_T22=te;_T23=& t;_T24=e;
Cyc_Tcexp_tcExpInitializer(_T22,_T23,_T24);_T25=
Cyc_Tcenv_curr_aquals_bounds(te);_T26=e;_T27=t;_T28=& bogus;_T29=Cyc_Tcutil_coerce_arg(_T25,_T26,_T27,_T28);if(_T29)goto _TL41C;else{goto _TL41E;}
_TL41E:{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;_T4F.f1=_tag_fat("datatype constructor ",sizeof(char),22U);_T2A=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T2A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50;_T50.tag=1;_T2C=tuf;_T50.f1=_T2C->name;_T2B=_T50;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50=_T2B;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;
_T51.f1=_tag_fat(" expects argument of type ",sizeof(char),27U);_T2D=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T2D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T52;_T52.tag=2;_T52.f1=t;_T2E=_T52;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T52=_T2E;{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;_T53.f1=_tag_fat(" but this argument has type ",sizeof(char),29U);_T2F=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T2F;{struct Cyc_Warn_String_Warn_Warg_struct _T54;_T54.tag=0;_T31=e;_T32=_T31->topt;
if(_T32!=0)goto _TL41F;_T33=_tag_fat("?",sizeof(char),2U);_T54.f1=_T33;goto _TL420;_TL41F: _T34=e;_T35=_T34->topt;_T54.f1=Cyc_Absynpp_typ2string(_T35);_TL420: _T30=_T54;}{struct Cyc_Warn_String_Warn_Warg_struct _T54=_T30;void*_T55[6];_T55[0]=& _T4F;_T55[1]=& _T50;_T55[2]=& _T51;_T55[3]=& _T52;_T55[4]=& _T53;_T55[5]=& _T54;_T36=e;_T37=_T36->loc;_T38=_tag_fat(_T55,sizeof(void*),6);Cyc_Warn_err2(_T37,_T38);}}}}}}
Cyc_Unify_explain_failure();goto _TL41D;_TL41C: _TL41D:;}}}_T39=es;
# 2336
es=_T39->tl;_T3A=ts;ts=_T3A->tl;goto _TL41A;_TL419:
# 2348
 if(es==0)goto _TL421;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;
# 2350
_T4F.f1=_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_T3C=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T3C;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50;_T50.tag=1;_T3E=tuf;_T50.f1=_T3E->name;_T3D=_T50;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50=_T3D;void*_T51[2];_T51[0]=& _T4F;_T51[1]=& _T50;_T3F=te;_T40=es;_T41=_T40->hd;_T42=(struct Cyc_Absyn_Exp*)_T41;_T43=_T42->loc;_T44=topt;_T45=_tag_fat(_T51,sizeof(void*),2);_T3B=Cyc_Tcexp_expr_err(_T3F,_T43,_T44,_T45);}}
# 2349
return _T3B;_TL421:
# 2351
 if(ts==0)goto _TL423;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;
# 2353
_T4F.f1=_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_T47=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T47;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50;_T50.tag=1;_T49=tuf;_T50.f1=_T49->name;_T48=_T50;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T50=_T48;void*_T51[2];_T51[0]=& _T4F;_T51[1]=& _T50;_T4A=te;_T4B=loc;_T4C=topt;_T4D=_tag_fat(_T51,sizeof(void*),2);_T46=Cyc_Tcexp_expr_err(_T4A,_T4B,_T4C,_T4D);}}
# 2352
return _T46;_TL423: _T4E=res;
# 2354
return _T4E;}}}}}}
# 2358
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){int _T0;int _T1;int _T2;void**_T3;void*_T4;int*_T5;int _T6;struct Cyc_Absyn_PtrInfo _T7;int _T8;_T0=
# 2360
Cyc_Tcutil_is_bits_only_type(t);if(_T0)goto _TL427;else{goto _TL428;}_TL428: _T1=allow_zero;if(_T1)goto _TL429;else{goto _TL425;}_TL429: _T2=Cyc_Tcutil_zeroable_type(t);if(_T2)goto _TL427;else{goto _TL425;}
_TL427: return 1;_TL425:
# 2363
 if(topt!=0)goto _TL42A;
return 0;_TL42A: _T3=topt;_T4=*_T3;{
void*_T9=Cyc_Absyn_compress(_T4);void*_TA;_T5=(int*)_T9;_T6=*_T5;if(_T6!=4)goto _TL42C;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T9;_T7=_TB->f1;_TA=_T7.elt_type;}{void*elt_typ=_TA;
# 2367
Cyc_Unify_unify(elt_typ,t);_T8=
Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);return _T8;}_TL42C:
 return 0;;}}
# 2373
static void*Cyc_Tcexp_mallocRgn(void*rgn){void*_T0;_T0=Cyc_Absyn_heap_rgn_type;
# 2375
return _T0;}
# 2381
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,struct Cyc_Absyn_Exp**aqopt,void***t,struct Cyc_Absyn_Exp**e,enum Cyc_Absyn_MallocKind*mknd,int*is_fat){struct Cyc_Absyn_Exp**_T0;struct Cyc_Absyn_Exp*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Core_Opt*_T3;struct Cyc_Core_Opt*_T4;void*_T5;struct Cyc_Tcenv_Tenv*_T6;void**_T7;struct Cyc_Absyn_Exp**_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Exp*_TA;int*_TB;int _TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Typ_Warn_Warg_struct _T16;struct Cyc_Absyn_Exp**_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Exp*_T19;unsigned _T1A;struct _fat_ptr _T1B;void**_T1C;void*_T1D;void**_T1E;int _T1F;struct Cyc_Absyn_Exp**_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_Core_Opt*_T22;struct Cyc_Core_Opt*_T23;struct Cyc_Core_Opt*_T24;void*_T25;struct Cyc_Tcenv_Tenv*_T26;void**_T27;struct Cyc_Absyn_Exp**_T28;struct Cyc_Absyn_Exp*_T29;struct Cyc_Absyn_Exp*_T2A;int*_T2B;int _T2C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2D;void*_T2E;int*_T2F;int _T30;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;void*_T34;struct Cyc_Warn_String_Warn_Warg_struct _T35;struct Cyc_Warn_Typ_Warn_Warg_struct _T36;struct Cyc_Absyn_Exp**_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_Absyn_Exp*_T39;unsigned _T3A;struct _fat_ptr _T3B;struct Cyc_List_List*_T3C;void**_T3D;void*_T3E;void*_T3F;int*_T40;int _T41;void*_T42;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T43;void*_T44;int*_T45;int _T46;void*_T47;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T48;void*_T49;struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4A;enum Cyc_Absyn_AliasQualVal _T4B;struct Cyc_Absyn_Exp**_T4C;struct Cyc_Tcenv_Tenv*_T4D;void**_T4E;struct Cyc_Absyn_Exp**_T4F;struct Cyc_Absyn_Exp*_T50;enum Cyc_Absyn_MallocKind*_T51;enum Cyc_Absyn_MallocKind _T52;int _T53;void***_T54;void**_T55;struct Cyc_Warn_String_Warn_Warg_struct _T56;int(*_T57)(struct _fat_ptr);void*(*_T58)(struct _fat_ptr);struct _fat_ptr _T59;void***_T5A;void**_T5B;unsigned _T5C;struct Cyc_Tcenv_Tenv*_T5D;struct Cyc_List_List*_T5E;struct Cyc_Absyn_Kind*_T5F;struct Cyc_Absyn_Kind*_T60;void*_T61;int _T62;struct Cyc_Warn_String_Warn_Warg_struct _T63;struct Cyc_Warn_Typ_Warn_Warg_struct _T64;struct Cyc_Warn_String_Warn_Warg_struct _T65;unsigned _T66;struct _fat_ptr _T67;struct Cyc_Absyn_Exp**_T68;struct Cyc_Absyn_Exp**_T69;struct Cyc_Absyn_Exp*_T6A;void*_T6B;int*_T6C;unsigned _T6D;void*_T6E;void*_T6F;void***_T70;void**_T71;struct Cyc_Tcenv_Tenv*_T72;void**_T73;struct Cyc_Absyn_Exp*_T74;void*_T75;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T76;enum Cyc_Absyn_Primop _T77;void*_T78;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T79;struct Cyc_List_List*_T7A;void*_T7B;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T7C;struct Cyc_List_List*_T7D;struct Cyc_List_List*_T7E;struct Cyc_List_List*_T7F;void*_T80;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;struct Cyc_List_List*_T84;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;void*_T87;struct Cyc_List_List*_T88;void*_T89;struct Cyc_List_List*_T8A;void*_T8B;struct _tuple0 _T8C;struct Cyc_Absyn_Exp*_T8D;struct Cyc_Absyn_Exp*_T8E;void*_T8F;int*_T90;int _T91;void*_T92;void*_T93;void*_T94;int*_T95;int _T96;void*_T97;void*_T98;int _T99;int _T9A;struct Cyc_Warn_String_Warn_Warg_struct _T9B;struct Cyc_Warn_Typ_Warn_Warg_struct _T9C;struct Cyc_Warn_String_Warn_Warg_struct _T9D;unsigned _T9E;struct _fat_ptr _T9F;enum Cyc_Absyn_MallocKind*_TA0;void***_TA1;void**_TA2;struct Cyc_Absyn_Exp**_TA3;struct Cyc_Absyn_Exp*_TA4;int*_TA5;int _TA6;struct Cyc_Absyn_Exp*_TA7;void*_TA8;void*_TA9;int*_TAA;int _TAB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAC;void*_TAD;int*_TAE;int _TAF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB0;struct Cyc_List_List*_TB1;struct Cyc_List_List*_TB2;void*_TB3;int*_TB4;int _TB5;struct Cyc_Absyn_Exp*_TB6;void***_TB7;void**_TB8;struct Cyc_Absyn_Exp**_TB9;int*_TBA;void*_TBB;int*_TBC;int _TBD;void*_TBE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TBF;void*_TC0;int*_TC1;int _TC2;void*_TC3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC4;void*_TC5;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TC6;union Cyc_Absyn_AggrInfo _TC7;struct _union_AggrInfo_KnownAggr _TC8;unsigned _TC9;void*_TCA;void*_TCB;union Cyc_Absyn_AggrInfo _TCC;struct _union_AggrInfo_KnownAggr _TCD;struct Cyc_Absyn_Aggrdecl**_TCE;struct Cyc_Absyn_Aggrdecl*_TCF;struct Cyc_Absyn_AggrdeclImpl*_TD0;struct Cyc_Absyn_Aggrdecl*_TD1;struct Cyc_Absyn_AggrdeclImpl*_TD2;struct Cyc_List_List*_TD3;struct Cyc_Warn_String_Warn_Warg_struct _TD4;unsigned _TD5;struct _fat_ptr _TD6;void**_TD7;void*_TD8;int*_TD9;int _TDA;struct Cyc_Absyn_PtrInfo _TDB;struct Cyc_Absyn_PtrAtts _TDC;struct Cyc_Absyn_PtrInfo _TDD;struct Cyc_Absyn_PtrAtts _TDE;struct Cyc_Absyn_PtrInfo _TDF;struct Cyc_Absyn_PtrAtts _TE0;struct Cyc_Absyn_PtrInfo _TE1;struct Cyc_Absyn_PtrAtts _TE2;int _TE3;struct Cyc_Warn_String_Warn_Warg_struct _TE4;unsigned _TE5;struct _fat_ptr _TE6;int _TE7;int _TE8;enum Cyc_Absyn_MallocKind*_TE9;enum Cyc_Absyn_MallocKind _TEA;int _TEB;struct Cyc_Warn_String_Warn_Warg_struct _TEC;unsigned _TED;struct _fat_ptr _TEE;enum Cyc_Absyn_MallocKind*_TEF;int _TF0;void*_TF1;void*_TF2;int _TF3;int _TF4;int _TF5;int*_TF6;void*_TF7;void*_TF8;void*_TF9;struct Cyc_Absyn_Tqual _TFA;void*_TFB;void*_TFC;void*_TFD;void*_TFE;struct Cyc_Absyn_Exp*_TFF;void*_T100;void*_T101;int*_T102;int _T103;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T104;void*_T105;int*_T106;int _T107;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T108;struct Cyc_List_List*_T109;struct Cyc_List_List*_T10A;void*_T10B;void*_T10C;struct Cyc_Absyn_Exp*_T10D;int _T10E;int*_T10F;void*_T110;void*_T111;void*_T112;struct Cyc_Absyn_Tqual _T113;void*_T114;void*_T115;void*_T116;void*_T117;enum Cyc_Absyn_MallocKind*_T118;enum Cyc_Absyn_MallocKind _T119;int _T11A;int _T11B;unsigned _T11C;void*_T11D;void*_T11E;void*_T11F;void*_T120;int _T121;struct Cyc_Warn_String_Warn_Warg_struct _T122;struct Cyc_Warn_Typ_Warn_Warg_struct _T123;void**_T124;unsigned _T125;struct _fat_ptr _T126;int _T127;void*_T128;void*_T129;void*_T12A;struct Cyc_Absyn_Tqual _T12B;void*_T12C;void*_T12D;void*_T12E;
# 2385
void*rgn=Cyc_Absyn_heap_rgn_type;_T0=ropt;_T1=*_T0;
if(_T1==0)goto _TL42E;_T2=& Cyc_Kinds_eko;_T3=(struct Cyc_Core_Opt*)_T2;_T4=
# 2390
Cyc_Tcenv_lookup_opt_type_vars(te);_T5=Cyc_Absyn_new_evar(_T3,_T4);{
# 2389
void*expected_type=
Cyc_Absyn_rgn_handle_type(_T5);_T6=te;_T7=& expected_type;_T8=ropt;_T9=*_T8;_TA=
_check_null(_T9);{void*handle_type=Cyc_Tcexp_tcExp(_T6,_T7,_TA);
void*_T12F=Cyc_Absyn_compress(handle_type);void*_T130;_TB=(int*)_T12F;_TC=*_TB;if(_TC!=0)goto _TL430;_TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=4)goto _TL432;_T11=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_T12=_T11->f2;if(_T12==0)goto _TL434;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T131=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_T13=_T131->f2;{struct Cyc_List_List _T132=*_T13;_T14=_T132.hd;_T130=(void*)_T14;}}{void*r=_T130;
# 2394
rgn=r;
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);goto _LL0;}_TL434: goto _LL3;_TL432: goto _LL3;_TL430: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T131;_T131.tag=0;
# 2398
_T131.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_T15=_T131;}{struct Cyc_Warn_String_Warn_Warg_struct _T131=_T15;{struct Cyc_Warn_Typ_Warn_Warg_struct _T132;_T132.tag=2;_T132.f1=handle_type;_T16=_T132;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T132=_T16;void*_T133[2];_T133[0]=& _T131;_T133[1]=& _T132;_T17=ropt;_T18=*_T17;_T19=_check_null(_T18);_T1A=_T19->loc;_T1B=_tag_fat(_T133,sizeof(void*),2);Cyc_Warn_err2(_T1A,_T1B);}}goto _LL0;_LL0:;}}goto _TL42F;
# 2404
_TL42E: if(topt==0)goto _TL436;{
void*optrgn=Cyc_Absyn_void_type;_T1C=topt;_T1D=*_T1C;_T1E=& optrgn;_T1F=
Cyc_Tcutil_rgn_of_pointer(_T1D,_T1E);if(!_T1F)goto _TL438;
rgn=Cyc_Tcexp_mallocRgn(optrgn);goto _TL439;_TL438: _TL439:;}goto _TL437;_TL436: _TL437: _TL42F: {
# 2411
void*aqual_type=Cyc_Absyn_al_qual_type;_T20=aqopt;_T21=*_T20;
if(_T21==0)goto _TL43A;_T22=& Cyc_Kinds_aqko;_T23=(struct Cyc_Core_Opt*)_T22;_T24=
# 2414
Cyc_Tcenv_lookup_opt_type_vars(te);_T25=Cyc_Absyn_new_evar(_T23,_T24);{
# 2413
void*expected_type=
Cyc_Absyn_aqual_handle_type(_T25);_T26=te;_T27=& expected_type;_T28=aqopt;_T29=*_T28;_T2A=
_check_null(_T29);{void*qhandle_type=Cyc_Tcexp_tcExp(_T26,_T27,_T2A);
void*_T12F=Cyc_Absyn_compress(qhandle_type);void*_T130;_T2B=(int*)_T12F;_T2C=*_T2B;if(_T2C!=0)goto _TL43C;_T2D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_T2E=_T2D->f1;_T2F=(int*)_T2E;_T30=*_T2F;if(_T30!=18)goto _TL43E;_T31=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_T32=_T31->f2;if(_T32==0)goto _TL440;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T131=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12F;_T33=_T131->f2;{struct Cyc_List_List _T132=*_T33;_T34=_T132.hd;_T130=(void*)_T34;}}{void*aq=_T130;
# 2418
aqual_type=aq;goto _LL5;}_TL440: goto _LL8;_TL43E: goto _LL8;_TL43C: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _T131;_T131.tag=0;
# 2421
_T131.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_T35=_T131;}{struct Cyc_Warn_String_Warn_Warg_struct _T131=_T35;{struct Cyc_Warn_Typ_Warn_Warg_struct _T132;_T132.tag=2;_T132.f1=qhandle_type;_T36=_T132;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T132=_T36;void*_T133[2];_T133[0]=& _T131;_T133[1]=& _T132;_T37=ropt;_T38=*_T37;_T39=_check_null(_T38);_T3A=_T39->loc;_T3B=_tag_fat(_T133,sizeof(void*),2);Cyc_Warn_err2(_T3A,_T3B);}}goto _LL5;_LL5:;}}goto _TL43B;
# 2425
_TL43A: if(topt==0)goto _TL442;_T3C=
Cyc_Tcenv_curr_aquals_bounds(te);_T3D=topt;_T3E=*_T3D;{void*aq=Cyc_Tcutil_eval_aquals_of(_T3C,_T3E);
if(aq==0)goto _TL444;if(aq==0)goto _TL446;_T3F=aq;_T40=(int*)_T3F;_T41=*_T40;if(_T41!=0)goto _TL448;_T42=aq;_T43=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_T44=_T43->f1;_T45=(int*)_T44;_T46=*_T45;if(_T46!=16)goto _TL44A;_T47=aq;_T48=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T47;_T49=_T48->f1;_T4A=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T49;_T4B=_T4A->f1;if(_T4B!=Cyc_Absyn_Unique_qual)goto _TL44C;
# 2430
aqual_type=Cyc_Absyn_un_qual_type;_T4C=aqopt;
*_T4C=Cyc_Absyn_uniqueaqual_exp();goto _LLA;_TL44C: goto _LLD;_TL44A: goto _LLD;_TL448: goto _LLD;_TL446: _LLD: goto _LLA;_LLA: goto _TL445;_TL444: _TL445:;}goto _TL443;_TL442: _TL443: _TL43B: _T4D=
# 2438
Cyc_Tcenv_clear_abstract_val_ok(te);_T4E=& Cyc_Absyn_uint_type;_T4F=e;_T50=*_T4F;Cyc_Tcexp_tcExp(_T4D,_T4E,_T50);{
# 2447 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;_T51=mknd;_T52=*_T51;_T53=(int)_T52;
if(_T53!=1)goto _TL44E;_T54=t;_T55=*_T54;
if(_T55!=0)goto _TL450;{struct Cyc_Warn_String_Warn_Warg_struct _T12F;_T12F.tag=0;
_T12F.f1=_tag_fat("calloc with empty type",sizeof(char),23U);_T56=_T12F;}{struct Cyc_Warn_String_Warn_Warg_struct _T12F=_T56;void*_T130[1];_T130[0]=& _T12F;_T58=Cyc_Warn_impos2;{int(*_T131)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T58;_T57=_T131;}_T59=_tag_fat(_T130,sizeof(void*),1);_T57(_T59);}goto _TL451;_TL450: _TL451: _T5A=t;_T5B=*_T5A;
elt_type=*_T5B;_T5C=loc;_T5D=te;_T5E=
Cyc_Tcenv_lookup_type_vars(te);_T5F=& Cyc_Kinds_mk;_T60=(struct Cyc_Absyn_Kind*)_T5F;_T61=elt_type;Cyc_Tctyp_check_type(_T5C,_T5D,_T5E,_T60,1,0,_T61);
Cyc_Tcutil_check_no_qual(loc,elt_type);_T62=
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);if(_T62)goto _TL452;else{goto _TL454;}
_TL454:{struct Cyc_Warn_String_Warn_Warg_struct _T12F;_T12F.tag=0;_T12F.f1=_tag_fat("calloc cannot be used with type ",sizeof(char),33U);_T63=_T12F;}{struct Cyc_Warn_String_Warn_Warg_struct _T12F=_T63;{struct Cyc_Warn_Typ_Warn_Warg_struct _T130;_T130.tag=2;_T130.f1=elt_type;_T64=_T130;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T130=_T64;{struct Cyc_Warn_String_Warn_Warg_struct _T131;_T131.tag=0;
_T131.f1=_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_T65=_T131;}{struct Cyc_Warn_String_Warn_Warg_struct _T131=_T65;void*_T132[3];_T132[0]=& _T12F;_T132[1]=& _T130;_T132[2]=& _T131;_T66=loc;_T67=_tag_fat(_T132,sizeof(void*),3);Cyc_Warn_err2(_T66,_T67);}}}goto _TL453;_TL452: _TL453: _T68=e;
num_elts=*_T68;
one_elt=0;goto _TL44F;
# 2462
_TL44E: _T69=e;_T6A=*_T69;{void*er=_T6A->r;
retry_sizeof: {struct Cyc_Absyn_Exp*_T12F;struct Cyc_Absyn_Exp*_T130;void*_T131;_T6B=er;_T6C=(int*)_T6B;_T6D=*_T6C;switch(_T6D){case 17: _T6E=er;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T132=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T6E;_T6F=_T132->f1;_T131=(void*)_T6F;}{void*t2=_T131;
# 2466
elt_type=t2;_T70=t;{void**_T132=_cycalloc(sizeof(void*));
*_T132=elt_type;_T71=(void**)_T132;}*_T70=_T71;
num_elts=Cyc_Absyn_uint_exp(1U,0U);_T72=te;_T73=& Cyc_Absyn_uint_type;_T74=num_elts;
Cyc_Tcexp_tcExp(_T72,_T73,_T74);
one_elt=1;goto _LLF;}case 3: _T75=er;_T76=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T75;_T77=_T76->f1;if(_T77!=Cyc_Absyn_Times)goto _TL456;_T78=er;_T79=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T78;_T7A=_T79->f2;if(_T7A==0)goto _TL458;_T7B=er;_T7C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T7B;_T7D=_T7C->f2;_T7E=(struct Cyc_List_List*)_T7D;_T7F=_T7E->tl;if(_T7F==0)goto _TL45A;_T80=er;_T81=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T80;_T82=_T81->f2;_T83=(struct Cyc_List_List*)_T82;_T84=_T83->tl;_T85=(struct Cyc_List_List*)_T84;_T86=_T85->tl;if(_T86!=0)goto _TL45C;_T87=er;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T132=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T87;_T88=_T132->f2;{struct Cyc_List_List _T133=*_T88;_T89=_T133.hd;_T130=(struct Cyc_Absyn_Exp*)_T89;_T8A=_T133.tl;{struct Cyc_List_List _T134=*_T8A;_T8B=_T134.hd;_T12F=(struct Cyc_Absyn_Exp*)_T8B;}}}{struct Cyc_Absyn_Exp*e1=_T130;struct Cyc_Absyn_Exp*e2=_T12F;{struct _tuple0 _T132;_T8D=e1;
# 2473
_T132.f0=_T8D->r;_T8E=e2;_T132.f1=_T8E->r;_T8C=_T132;}{struct _tuple0 _T132=_T8C;void*_T133;_T8F=_T132.f0;_T90=(int*)_T8F;_T91=*_T90;if(_T91!=17)goto _TL45E;_T92=_T132.f0;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T134=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T92;_T93=_T134->f1;_T133=(void*)_T93;}{void*t1=_T133;
# 2475
e1=e2;_T133=t1;goto _LL1A;}_TL45E: _T94=_T132.f1;_T95=(int*)_T94;_T96=*_T95;if(_T96!=17)goto _TL460;_T97=_T132.f1;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T134=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T97;_T98=_T134->f1;_T133=(void*)_T98;}_LL1A:{void*t2=_T133;_T99=
# 2479
Cyc_Tcexp_check_malloc_type(0,loc,topt,t2);if(_T99)goto _TL462;else{goto _TL464;}
# 2482
_TL464: _T9A=Cyc_Tcexp_check_malloc_type(1,loc,topt,t2);if(_T9A)goto _TL465;else{goto _TL467;}
_TL467:{struct Cyc_Warn_String_Warn_Warg_struct _T134;_T134.tag=0;_T134.f1=_tag_fat("malloc cannot be used with type ",sizeof(char),33U);_T9B=_T134;}{struct Cyc_Warn_String_Warn_Warg_struct _T134=_T9B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T135;_T135.tag=2;_T135.f1=t2;_T9C=_T135;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T135=_T9C;{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_T9D=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T9D;void*_T137[3];_T137[0]=& _T134;_T137[1]=& _T135;_T137[2]=& _T136;_T9E=loc;_T9F=_tag_fat(_T137,sizeof(void*),3);Cyc_Warn_err2(_T9E,_T9F);}}}goto _TL466;
# 2486
_TL465: _TA0=mknd;*_TA0=1U;_TL466: goto _TL463;_TL462: _TL463:
# 2488
 elt_type=t2;_TA1=t;{void**_T134=_cycalloc(sizeof(void*));
*_T134=elt_type;_TA2=(void**)_T134;}*_TA1=_TA2;
num_elts=e1;
one_elt=0;goto _LL16;}_TL460: goto No_sizeof;_LL16:;}goto _LLF;}_TL45C: goto _LL14;_TL45A: goto _LL14;_TL458: goto _LL14;_TL456: goto _LL14;default: _LL14:
# 2497
 No_sizeof: _TA3=e;{
# 2500
struct Cyc_Absyn_Exp*real_e=*_TA3;_TA4=real_e;{
void*_T132=_TA4->r;struct Cyc_Absyn_Exp*_T133;_TA5=(int*)_T132;_TA6=*_TA5;if(_TA6!=14)goto _TL468;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T134=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T132;_T133=_T134->f2;}{struct Cyc_Absyn_Exp*e=_T133;
real_e=e;goto _LL1D;}_TL468: goto _LL1D;_LL1D:;}_TA7=real_e;_TA8=_TA7->topt;_TA9=
# 2505
_check_null(_TA8);{void*_T132=Cyc_Absyn_compress(_TA9);void*_T133;_TAA=(int*)_T132;_TAB=*_TAA;if(_TAB!=0)goto _TL46A;_TAC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T132;_TAD=_TAC->f1;_TAE=(int*)_TAD;_TAF=*_TAE;if(_TAF!=5)goto _TL46C;_TB0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T132;_TB1=_TB0->f2;if(_TB1==0)goto _TL46E;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T132;_TB2=_T134->f2;{struct Cyc_List_List _T135=*_TB2;_TB3=_T135.hd;_T133=(void*)_TB3;}}{void*tagt=_T133;{
# 2507
void*_T134=Cyc_Absyn_compress(tagt);struct Cyc_Absyn_Exp*_T135;_TB4=(int*)_T134;_TB5=*_TB4;if(_TB5!=9)goto _TL470;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T136=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T134;_T135=_T136->f1;}{struct Cyc_Absyn_Exp*vexp=_T135;_TB6=vexp;
er=_TB6->r;goto retry_sizeof;}_TL470: goto _LL27;_LL27:;}goto _LL22;}_TL46E: goto _LL25;_TL46C: goto _LL25;_TL46A: _LL25: goto _LL22;_LL22:;}
# 2514
elt_type=Cyc_Absyn_char_type;_TB7=t;{void**_T132=_cycalloc(sizeof(void*));
*_T132=elt_type;_TB8=(void**)_T132;}*_TB7=_TB8;_TB9=e;
num_elts=*_TB9;
one_elt=0;}goto _LLF;}_LLF:;}}_TL44F: _TBA=is_fat;
# 2523
*_TBA=!one_elt;{struct Cyc_Absyn_Aggrdecl*_T12F;_TBB=elt_type;_TBC=(int*)_TBB;_TBD=*_TBC;if(_TBD!=0)goto _TL472;_TBE=elt_type;_TBF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TBE;_TC0=_TBF->f1;_TC1=(int*)_TC0;_TC2=*_TC1;if(_TC2!=24)goto _TL474;_TC3=elt_type;_TC4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC3;_TC5=_TC4->f1;_TC6=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC5;_TC7=_TC6->f1;_TC8=_TC7.KnownAggr;_TC9=_TC8.tag;if(_TC9!=2)goto _TL476;_TCA=elt_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T130=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TCA;_TCB=_T130->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T131=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TCB;_TCC=_T131->f1;_TCD=_TCC.KnownAggr;_TCE=_TCD.val;{struct Cyc_Absyn_Aggrdecl*_T132=*_TCE;_T12F=_T132;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T12F;_TCF=ad;_TD0=_TCF->impl;
# 2528
if(_TD0==0)goto _TL478;_TD1=ad;_TD2=_TD1->impl;_TD3=_TD2->exist_vars;if(_TD3==0)goto _TL478;{struct Cyc_Warn_String_Warn_Warg_struct _T130;_T130.tag=0;
_T130.f1=_tag_fat("malloc with existential types not yet implemented",sizeof(char),50U);_TD4=_T130;}{struct Cyc_Warn_String_Warn_Warg_struct _T130=_TD4;void*_T131[1];_T131[0]=& _T130;_TD5=loc;_TD6=_tag_fat(_T131,sizeof(void*),1);Cyc_Warn_err2(_TD5,_TD6);}goto _TL479;_TL478: _TL479: goto _LL2C;}_TL476: goto _LL2F;_TL474: goto _LL2F;_TL472: _LL2F: goto _LL2C;_LL2C:;}{
# 2535
void*(*ptr_maker)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=Cyc_Absyn_at_type;
void*zero_term=Cyc_Absyn_false_type;
if(topt==0)goto _TL47A;_TD7=topt;_TD8=*_TD7;{
void*_T12F=Cyc_Absyn_compress(_TD8);void*_T130;void*_T131;void*_T132;void*_T133;_TD9=(int*)_T12F;_TDA=*_TD9;if(_TDA!=4)goto _TL47C;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T12F;_TDB=_T134->f1;_TDC=_TDB.ptr_atts;_T133=_TDC.nullable;_TDD=_T134->f1;_TDE=_TDD.ptr_atts;_T132=_TDE.bounds;_TDF=_T134->f1;_TE0=_TDF.ptr_atts;_T131=_TE0.zero_term;_TE1=_T134->f1;_TE2=_TE1.ptr_atts;_T130=_TE2.autoreleased;}{void*n=_T133;void*b=_T132;void*zt=_T131;void*rel=_T130;
# 2540
zero_term=zt;_TE3=
Cyc_Tcutil_force_type2bool(0,rel);if(!_TE3)goto _TL47E;{struct Cyc_Warn_String_Warn_Warg_struct _T134;_T134.tag=0;
_T134.f1=_tag_fat("cannot malloc() an autoreleased pointer",sizeof(char),40U);_TE4=_T134;}{struct Cyc_Warn_String_Warn_Warg_struct _T134=_TE4;void*_T135[1];_T135[0]=& _T134;_TE5=loc;_TE6=_tag_fat(_T135,sizeof(void*),1);Cyc_Warn_err2(_TE5,_TE6);}goto _TL47F;_TL47E: _TL47F: _TE7=
Cyc_Tcutil_force_type2bool(0,n);if(!_TE7)goto _TL480;
ptr_maker=Cyc_Absyn_star_type;goto _TL481;_TL480: _TL481: _TE8=
# 2547
Cyc_Tcutil_force_type2bool(0,zt);if(!_TE8)goto _TL482;_TE9=mknd;_TEA=*_TE9;_TEB=(int)_TEA;if(_TEB==1)goto _TL482;else{goto _TL484;}
_TL484:{struct Cyc_Warn_String_Warn_Warg_struct _T134;_T134.tag=0;_T134.f1=_tag_fat("converting malloc to calloc to ensure zero-termination",sizeof(char),55U);_TEC=_T134;}{struct Cyc_Warn_String_Warn_Warg_struct _T134=_TEC;void*_T135[1];_T135[0]=& _T134;_TED=loc;_TEE=_tag_fat(_T135,sizeof(void*),1);Cyc_Warn_warn2(_TED,_TEE);}_TEF=mknd;
*_TEF=1U;goto _TL483;_TL482: _TL483: _TF0=
# 2553
Cyc_Tcutil_is_cvar_type(b);if(_TF0)goto _TL485;else{goto _TL487;}
_TL487: _TF1=Cyc_Absyn_bounds_one();_TF2=b;{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(_TF1,_TF2);
if(eopt==0)goto _TL488;_TF3=one_elt;if(_TF3)goto _TL488;else{goto _TL48A;}
_TL48A:{struct Cyc_Absyn_Exp*upper_exp=eopt;
int is_constant=Cyc_Evexp_c_can_eval(num_elts);_TF4=is_constant;
if(!_TF4)goto _TL48B;_TF5=Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts);if(!_TF5)goto _TL48B;_TF6=is_fat;
*_TF6=0;_TF7=elt_type;_TF8=rgn;_TF9=aqual_type;_TFA=
Cyc_Absyn_empty_tqual(0U);_TFB=b;_TFC=zero_term;_TFD=Cyc_Absyn_false_type;_TFE=Cyc_Absyn_atb_type(_TF7,_TF8,_TF9,_TFA,_TFB,_TFC,_TFD);return _TFE;_TL48B: _TFF=num_elts;_T100=_TFF->topt;_T101=
# 2562
_check_null(_T100);{void*_T134=Cyc_Absyn_compress(_T101);void*_T135;_T102=(int*)_T134;_T103=*_T102;if(_T103!=0)goto _TL48D;_T104=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T134;_T105=_T104->f1;_T106=(int*)_T105;_T107=*_T106;if(_T107!=5)goto _TL48F;_T108=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T134;_T109=_T108->f2;if(_T109==0)goto _TL491;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T136=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T134;_T10A=_T136->f2;{struct Cyc_List_List _T137=*_T10A;_T10B=_T137.hd;_T135=(void*)_T10B;}}{void*tagtyp=_T135;_T10C=Cyc_Absyn_uint_type;_T10D=
# 2564
Cyc_Absyn_valueof_exp(tagtyp,0U);{struct Cyc_Absyn_Exp*tagtyp_exp=Cyc_Absyn_cast_exp(_T10C,_T10D,0,1U,0U);_T10E=
# 2566
Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp);if(!_T10E)goto _TL493;_T10F=is_fat;
*_T10F=0;_T110=elt_type;_T111=rgn;_T112=aqual_type;_T113=
Cyc_Absyn_empty_tqual(0U);_T114=b;_T115=zero_term;_T116=Cyc_Absyn_false_type;_T117=Cyc_Absyn_atb_type(_T110,_T111,_T112,_T113,_T114,_T115,_T116);return _T117;_TL493: goto _LL36;}}_TL491: goto _LL39;_TL48F: goto _LL39;_TL48D: _LL39: goto _LL36;_LL36:;}}goto _TL489;_TL488: _TL489:;}goto _TL486;
# 2575
_TL485: _T118=mknd;_T119=*_T118;_T11A=(int)_T119;if(_T11A!=0)goto _TL495;_T11B=Cyc_Tcutil_is_bits_only_type(elt_type);if(_T11B)goto _TL495;else{goto _TL497;}
# 2578
_TL497: _T11C=loc;_T11D=Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T11E=_check_null(_T11D);_T11F=
Cyc_Absyn_thinconst();_T120=
# 2578
Cyc_BansheeIf_equality_constraint(_T11E,_T11F);_T121=Cyc_BansheeIf_add_constraint(_T11C,_T120);if(_T121)goto _TL498;else{goto _TL49A;}
# 2580
_TL49A:{struct Cyc_Warn_String_Warn_Warg_struct _T134;_T134.tag=0;_T134.f1=_tag_fat("Unable to constrain pointer bound",sizeof(char),34U);_T122=_T134;}{struct Cyc_Warn_String_Warn_Warg_struct _T134=_T122;{struct Cyc_Warn_Typ_Warn_Warg_struct _T135;_T135.tag=2;_T124=topt;_T135.f1=*_T124;_T123=_T135;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T135=_T123;void*_T136[2];_T136[0]=& _T134;_T136[1]=& _T135;_T125=loc;_T126=_tag_fat(_T136,sizeof(void*),2);Cyc_Warn_err2(_T125,_T126);}}goto _TL499;_TL498: _TL499: goto _TL496;_TL495: _TL496: _TL486: goto _LL31;}_TL47C: goto _LL31;_LL31:;}goto _TL47B;_TL47A: _TL47B: _T127=one_elt;
# 2586
if(_T127)goto _TL49B;else{goto _TL49D;}_TL49D: ptr_maker=Cyc_Absyn_fatptr_type;goto _TL49C;_TL49B: _TL49C: _T128=elt_type;_T129=rgn;_T12A=aqual_type;_T12B=
Cyc_Absyn_empty_tqual(0U);_T12C=zero_term;_T12D=Cyc_Absyn_false_type;_T12E=ptr_maker(_T128,_T129,_T12A,_T12B,_T12C,_T12D);return _T12E;}}}}
# 2591
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Tcenv_Tenv*_T2;void**_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;struct Cyc_Absyn_Exp*_T7;void*_T8;int*_T9;int _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;unsigned _TC;struct _fat_ptr _TD;int _TE;int _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;unsigned _T11;struct _fat_ptr _T12;int _T13;void*_T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Tcenv_Tenv*_T16;struct Cyc_Absyn_Exp*_T17;unsigned _T18;void**_T19;struct _fat_ptr _T1A;int _T1B;void*_T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Tcenv_Tenv*_T1E;struct Cyc_Absyn_Exp*_T1F;unsigned _T20;void**_T21;struct _fat_ptr _T22;enum Cyc_Absyn_Coercion _T23;int _T24;void*_T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Warn_Typ_Warn_Warg_struct _T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;struct Cyc_Warn_Typ_Warn_Warg_struct _T29;struct Cyc_Tcenv_Tenv*_T2A;unsigned _T2B;void**_T2C;struct _fat_ptr _T2D;void*_T2E;
# 2593
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);_T0=e1;_T1=_T0->topt;{
void*t1=_check_null(_T1);_T2=te2;_T3=& t1;_T4=e2;
Cyc_Tcexp_tcExpNoPromote(_T2,_T3,_T4);_T5=e1;_T6=_T5->topt;{
void*t1=_check_null(_T6);_T7=e2;_T8=_T7->topt;{
void*t2=_check_null(_T8);{
# 2600
void*_T2F=Cyc_Absyn_compress(t1);_T9=(int*)_T2F;_TA=*_T9;if(_TA!=5)goto _TL49E;{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;
_T30.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_TB=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_TB;void*_T31[1];_T31[0]=& _T30;_TC=loc;_TD=_tag_fat(_T31,sizeof(void*),1);Cyc_Warn_err2(_TC,_TD);}goto _LL0;_TL49E: goto _LL0;_LL0:;}_TE=
# 2605
Cyc_Tcutil_is_boxed(t1);if(_TE)goto _TL4A0;else{goto _TL4A2;}_TL4A2: _TF=Cyc_Tcutil_is_pointer_type(t1);if(_TF)goto _TL4A0;else{goto _TL4A3;}
_TL4A3:{struct Cyc_Warn_String_Warn_Warg_struct _T2F;_T2F.tag=0;_T2F.f1=_tag_fat("swap not allowed for non-pointer or non-word-sized types",sizeof(char),57U);_T10=_T2F;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F=_T10;void*_T30[1];_T30[0]=& _T2F;_T11=loc;_T12=_tag_fat(_T30,sizeof(void*),1);Cyc_Warn_err2(_T11,_T12);}goto _TL4A1;_TL4A0: _TL4A1: _T13=
# 2609
Cyc_Absyn_is_lvalue(e1);if(_T13)goto _TL4A4;else{goto _TL4A6;}
_TL4A6:{struct Cyc_Warn_String_Warn_Warg_struct _T2F;_T2F.tag=0;_T2F.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_T15=_T2F;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F=_T15;void*_T30[1];_T30[0]=& _T2F;_T16=te;_T17=e1;_T18=_T17->loc;_T19=topt;_T1A=_tag_fat(_T30,sizeof(void*),1);_T14=Cyc_Tcexp_expr_err(_T16,_T18,_T19,_T1A);}return _T14;_TL4A4: _T1B=
Cyc_Absyn_is_lvalue(e2);if(_T1B)goto _TL4A7;else{goto _TL4A9;}
_TL4A9:{struct Cyc_Warn_String_Warn_Warg_struct _T2F;_T2F.tag=0;_T2F.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_T1D=_T2F;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F=_T1D;void*_T30[1];_T30[0]=& _T2F;_T1E=te;_T1F=e2;_T20=_T1F->loc;_T21=topt;_T22=_tag_fat(_T30,sizeof(void*),1);_T1C=Cyc_Tcexp_expr_err(_T1E,_T20,_T21,_T22);}return _T1C;_TL4A7:
# 2614
 Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);_T23=
# 2617
Cyc_Subtype_coercible_exact(loc,t1,t2);_T24=(int)_T23;if(_T24!=0)goto _TL4AA;{struct Cyc_Warn_String_Warn_Warg_struct _T2F;_T2F.tag=0;
_T2F.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_T26=_T2F;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F=_T26;{struct Cyc_Warn_Typ_Warn_Warg_struct _T30;_T30.tag=2;_T30.f1=t1;_T27=_T30;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T30=_T27;{struct Cyc_Warn_String_Warn_Warg_struct _T31;_T31.tag=0;_T31.f1=_tag_fat(" != ",sizeof(char),5U);_T28=_T31;}{struct Cyc_Warn_String_Warn_Warg_struct _T31=_T28;{struct Cyc_Warn_Typ_Warn_Warg_struct _T32;_T32.tag=2;_T32.f1=t2;_T29=_T32;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T32=_T29;void*_T33[4];_T33[0]=& _T2F;_T33[1]=& _T30;_T33[2]=& _T31;_T33[3]=& _T32;_T2A=te;_T2B=loc;_T2C=topt;_T2D=_tag_fat(_T33,sizeof(void*),4);_T25=Cyc_Tcexp_expr_err(_T2A,_T2B,_T2C,_T2D);}}}}return _T25;_TL4AA: _T2E=Cyc_Absyn_void_type;
return _T2E;}}}}
# 2623
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Absyn_Stmt*_T2;struct Cyc_Absyn_Stmt*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Exp*_T6;void*_T7;void*_T8;struct Cyc_Core_Opt*_T9;void*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_Typ_Warn_Warg_struct _TD;unsigned _TE;struct _fat_ptr _TF;void*_T10;void*_T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;struct Cyc_Tcenv_Tenv*_T13;unsigned _T14;void**_T15;struct _fat_ptr _T16;_T0=
Cyc_Tcenv_clear_abstract_val_ok(te);_T1=Cyc_Tcenv_enter_stmt_exp(_T0);_T2=s;Cyc_Tcstmt_tcStmt(_T1,_T2,1);
# 2626
_TL4AC: if(1)goto _TL4AD;else{goto _TL4AE;}
_TL4AD: _T3=s;{void*_T17=_T3->r;struct Cyc_Absyn_Stmt*_T18;struct Cyc_Absyn_Exp*_T19;_T4=(int*)_T17;_T5=*_T4;switch(_T5){case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T1A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T17;_T19=_T1A->f1;}{struct Cyc_Absyn_Exp*e=_T19;_T6=e;_T7=_T6->topt;{
# 2630
void*t=_check_null(_T7);_T8=t;_T9=
Cyc_Tcenv_lookup_opt_type_vars(te);_TA=Cyc_Absyn_wildtyp(_T9);_TB=Cyc_Unify_unify(_T8,_TA);if(_TB)goto _TL4B0;else{goto _TL4B2;}
_TL4B2:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;_T1A.f1=_tag_fat("statement expression returns type ",sizeof(char),35U);_TC=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_TC;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B;_T1B.tag=2;_T1B.f1=t;_TD=_T1B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B=_TD;void*_T1C[2];_T1C[0]=& _T1A;_T1C[1]=& _T1B;_TE=loc;_TF=_tag_fat(_T1C,sizeof(void*),2);Cyc_Tcexp_err_and_explain(_TE,_TF);}}goto _TL4B1;_TL4B0: _TL4B1: _T10=t;
return _T10;}}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T1A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T17;_T18=_T1A->f2;}{struct Cyc_Absyn_Stmt*s2=_T18;
s=s2;goto _TL4AC;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T1A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T17;_T18=_T1A->f2;}{struct Cyc_Absyn_Stmt*s1=_T18;
s=s1;goto _TL4AC;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 2637
_T1A.f1=_tag_fat("statement expression must end with expression",sizeof(char),46U);_T12=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T12;void*_T1B[1];_T1B[0]=& _T1A;_T13=te;_T14=loc;_T15=topt;_T16=_tag_fat(_T1B,sizeof(void*),1);_T11=Cyc_Tcexp_expr_err(_T13,_T14,_T15,_T16);}
# 2636
return _T11;};}goto _TL4AC;_TL4AE:;}
# 2641
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp*_T1;void*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7;void*_T8;int*_T9;int _TA;void*_TB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC;void*_TD;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_KnownAggr _T10;unsigned _T11;void*_T12;void*_T13;union Cyc_Absyn_AggrInfo _T14;struct _union_AggrInfo_KnownAggr _T15;struct Cyc_Absyn_Aggrdecl**_T16;struct Cyc_Absyn_Aggrdecl*_T17;enum Cyc_Absyn_AggrKind _T18;int _T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_AggrdeclImpl*_T1B;struct Cyc_Absyn_Aggrdecl*_T1C;struct Cyc_Absyn_AggrdeclImpl*_T1D;int _T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;struct Cyc_Warn_Typ_Warn_Warg_struct _T20;unsigned _T21;struct _fat_ptr _T22;void*_T23;_T0=
Cyc_Tcenv_enter_abstract_val_ok(te);_T1=e;_T2=Cyc_Tcexp_tcExp(_T0,0,_T1);{void*t=Cyc_Absyn_compress(_T2);{struct Cyc_Absyn_Aggrdecl*_T24;_T3=t;_T4=(int*)_T3;_T5=*_T4;if(_T5!=0)goto _TL4B3;_T6=t;_T7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6;_T8=_T7->f1;_T9=(int*)_T8;_TA=*_T9;if(_TA!=24)goto _TL4B5;_TB=t;_TC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB;_TD=_TC->f1;_TE=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TD;_TF=_TE->f1;_T10=_TF.KnownAggr;_T11=_T10.tag;if(_T11!=2)goto _TL4B7;_T12=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12;_T13=_T25->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T26=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T13;_T14=_T26->f1;_T15=_T14.KnownAggr;_T16=_T15.val;{struct Cyc_Absyn_Aggrdecl*_T27=*_T16;_T24=_T27;}}}{struct Cyc_Absyn_Aggrdecl*ad=_T24;_T17=ad;_T18=_T17->kind;_T19=(int)_T18;
# 2645
if(_T19!=1)goto _TL4B9;_T1A=ad;_T1B=_T1A->impl;if(_T1B==0)goto _TL4B9;_T1C=ad;_T1D=_T1C->impl;_T1E=_T1D->tagged;if(!_T1E)goto _TL4B9;goto _LL0;_TL4B9: goto _LL4;}_TL4B7: goto _LL3;_TL4B5: goto _LL3;_TL4B3: _LL3: _LL4:{struct Cyc_Warn_String_Warn_Warg_struct _T25;_T25.tag=0;
# 2648
_T25.f1=_tag_fat("expecting @tagged union but found ",sizeof(char),35U);_T1F=_T25;}{struct Cyc_Warn_String_Warn_Warg_struct _T25=_T1F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T26;_T26.tag=2;_T26.f1=t;_T20=_T26;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T26=_T20;void*_T27[2];_T27[0]=& _T25;_T27[1]=& _T26;_T21=loc;_T22=_tag_fat(_T27,sizeof(void*),2);Cyc_Warn_err2(_T21,_T22);}}goto _LL0;_LL0:;}_T23=Cyc_Absyn_uint_type;
# 2650
return _T23;}}
# 2653
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e,int static_only){int _T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Absyn_Exp*_T2;struct _fat_ptr _T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Absyn_Exp*_T5;struct _fat_ptr _T6;void*_T7;_T0=static_only;
if(!_T0)goto _TL4BB;_T1=
# 2658
Cyc_Tcenv_allow_valueof(te);_T2=e;_T3=_tag_fat("@assert",sizeof(char),8U);Cyc_Tcexp_tcTest(_T1,_T2,_T3);goto _TL4BC;
# 2660
_TL4BB: _T4=te;_T5=e;_T6=_tag_fat("assert",sizeof(char),7U);Cyc_Tcexp_tcTest(_T4,_T5,_T6);_TL4BC: _T7=Cyc_Absyn_sint_type;
# 2662
return _T7;}
# 2666
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp**qual_hndl,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Absyn_Exp**_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Core_Opt*_T3;struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;void*_T6;struct Cyc_Tcenv_Tenv*_T7;void**_T8;struct Cyc_Absyn_Exp**_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;int*_TC;int _TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;void*_TF;int*_T10;int _T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_Typ_Warn_Warg_struct _T17;struct Cyc_Absyn_Exp**_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Exp*_T1A;unsigned _T1B;struct _fat_ptr _T1C;void**_T1D;void*_T1E;void**_T1F;int _T20;struct Cyc_Absyn_Exp**_T21;struct Cyc_Absyn_Exp*_T22;struct Cyc_Core_Opt*_T23;struct Cyc_Core_Opt*_T24;struct Cyc_Core_Opt*_T25;void*_T26;struct Cyc_Tcenv_Tenv*_T27;void**_T28;struct Cyc_Absyn_Exp**_T29;struct Cyc_Absyn_Exp*_T2A;struct Cyc_Absyn_Exp*_T2B;int*_T2C;int _T2D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2E;void*_T2F;int*_T30;int _T31;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T32;struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Typ_Warn_Warg_struct _T37;struct Cyc_Absyn_Exp**_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;unsigned _T3B;struct _fat_ptr _T3C;struct Cyc_List_List*_T3D;void**_T3E;void*_T3F;void*_T40;int*_T41;int _T42;void*_T43;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T44;void*_T45;int*_T46;int _T47;void*_T48;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T49;void*_T4A;struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4B;enum Cyc_Absyn_AliasQualVal _T4C;struct Cyc_Absyn_Exp**_T4D;struct Cyc_Absyn_Exp*_T4E;int*_T4F;unsigned _T50;int _T51;struct Cyc_Warn_String_Warn_Warg_struct _T52;int(*_T53)(struct _fat_ptr);void*(*_T54)(struct _fat_ptr);struct _fat_ptr _T55;unsigned _T56;void*_T57;void**_T58;void*_T59;enum Cyc_Absyn_Coercion _T5A;int _T5B;enum Cyc_Absyn_Coercion _T5C;int _T5D;struct Cyc_Absyn_Exp*_T5E;struct Cyc_Absyn_Exp*_T5F;void**_T60;void*_T61;void**_T62;void*_T63;struct Cyc_Absyn_Exp*_T64;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T65;void**_T66;void*_T67;int*_T68;int _T69;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6A;void**_T6B;struct Cyc_Absyn_PtrInfo _T6C;struct Cyc_Absyn_PtrAtts _T6D;struct Cyc_Tcenv_Tenv*_T6E;struct Cyc_Absyn_Exp*_T6F;unsigned _T70;void**_T71;int _T72;struct Cyc_List_List*_T73;struct Cyc_Absyn_Exp*_T74;int _T75;struct Cyc_Warn_String_Warn_Warg_struct _T76;int(*_T77)(struct _fat_ptr);void*(*_T78)(struct _fat_ptr);struct _fat_ptr _T79;unsigned _T7A;void*_T7B;void**_T7C;void*_T7D;enum Cyc_Absyn_Coercion _T7E;int _T7F;enum Cyc_Absyn_Coercion _T80;int _T81;struct Cyc_Absyn_Exp*_T82;struct Cyc_Absyn_Exp*_T83;void**_T84;void*_T85;void**_T86;void*_T87;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T88;union Cyc_Absyn_Cnst _T89;struct _union_Cnst_Wstring_c _T8A;unsigned _T8B;void*_T8C;void*_T8D;void*_T8E;struct Cyc_Absyn_Tqual _T8F;void*_T90;void*_T91;void*_T92;struct Cyc_Tcenv_Tenv*_T93;void**_T94;struct Cyc_Absyn_Exp*_T95;void*_T96;void*_T97;void*_T98;struct Cyc_Absyn_Tqual _T99;struct Cyc_Absyn_Exp*_T9A;void*_T9B;void*_T9C;void*_T9D;void*_T9E;void*_T9F;void*_TA0;void*_TA1;struct Cyc_Absyn_Tqual _TA2;void*_TA3;void*_TA4;void*_TA5;struct Cyc_Tcenv_Tenv*_TA6;void**_TA7;struct Cyc_Absyn_Exp*_TA8;void*_TA9;void*_TAA;void*_TAB;struct Cyc_Absyn_Tqual _TAC;struct Cyc_Absyn_Exp*_TAD;void*_TAE;void*_TAF;void*_TB0;void*_TB1;void**_TB2;void*_TB3;int*_TB4;unsigned _TB5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB6;void**_TB7;struct Cyc_Absyn_PtrInfo _TB8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB9;void*_TBA;int*_TBB;int _TBC;void**_TBD;struct Cyc_Absyn_Exp*_TBE;unsigned _TBF;struct Cyc_List_List*_TC0;void*_TC1;struct Cyc_Absyn_Exp*_TC2;unsigned _TC3;void*_TC4;void**_TC5;void*_TC6;enum Cyc_Absyn_Coercion _TC7;int _TC8;enum Cyc_Absyn_Coercion _TC9;int _TCA;struct Cyc_Absyn_Exp*_TCB;struct Cyc_Absyn_Exp*_TCC;void**_TCD;void*_TCE;enum Cyc_Absyn_Coercion _TCF;void**_TD0;void*_TD1;
# 2671
void*rgn=Cyc_Absyn_heap_rgn_type;_T0=
Cyc_Tcenv_set_new_status(1U,te);te=Cyc_Tcenv_clear_abstract_val_ok(_T0);_T1=rgn_handle;_T2=*_T1;
if(_T2==0)goto _TL4BD;_T3=& Cyc_Kinds_eko;_T4=(struct Cyc_Core_Opt*)_T3;_T5=
# 2677
Cyc_Tcenv_lookup_opt_type_vars(te);_T6=Cyc_Absyn_new_evar(_T4,_T5);{
# 2676
void*expected_type=
Cyc_Absyn_rgn_handle_type(_T6);_T7=te;_T8=& expected_type;_T9=rgn_handle;_TA=*_T9;_TB=
_check_null(_TA);{void*handle_type=Cyc_Tcexp_tcExp(_T7,_T8,_TB);
void*_TD2=Cyc_Absyn_compress(handle_type);void*_TD3;_TC=(int*)_TD2;_TD=*_TC;if(_TD!=0)goto _TL4BF;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_TF=_TE->f1;_T10=(int*)_TF;_T11=*_T10;if(_T11!=4)goto _TL4C1;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_T13=_T12->f2;if(_T13==0)goto _TL4C3;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_T14=_TD4->f2;{struct Cyc_List_List _TD5=*_T14;_T15=_TD5.hd;_TD3=(void*)_T15;}}{void*r=_TD3;
# 2681
rgn=r;
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);goto _LL0;}_TL4C3: goto _LL3;_TL4C1: goto _LL3;_TL4BF: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _TD4;_TD4.tag=0;
# 2685
_TD4.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_T16=_TD4;}{struct Cyc_Warn_String_Warn_Warg_struct _TD4=_T16;{struct Cyc_Warn_Typ_Warn_Warg_struct _TD5;_TD5.tag=2;
_TD5.f1=handle_type;_T17=_TD5;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TD5=_T17;void*_TD6[2];_TD6[0]=& _TD4;_TD6[1]=& _TD5;_T18=rgn_handle;_T19=*_T18;_T1A=
# 2685
_check_null(_T19);_T1B=_T1A->loc;_T1C=_tag_fat(_TD6,sizeof(void*),2);Cyc_Warn_err2(_T1B,_T1C);}}goto _LL0;_LL0:;}}goto _TL4BE;
# 2689
_TL4BD: if(topt==0)goto _TL4C5;{
# 2692
void*optrgn=Cyc_Absyn_void_type;_T1D=topt;_T1E=*_T1D;_T1F=& optrgn;_T20=
Cyc_Tcutil_rgn_of_pointer(_T1E,_T1F);if(!_T20)goto _TL4C7;
rgn=Cyc_Tcexp_mallocRgn(optrgn);goto _TL4C8;_TL4C7: _TL4C8:;}goto _TL4C6;_TL4C5: _TL4C6: _TL4BE: {
# 2698
void*aqual=Cyc_Absyn_al_qual_type;_T21=qual_hndl;_T22=*_T21;
if(_T22==0)goto _TL4C9;_T23=& Cyc_Kinds_aqko;_T24=(struct Cyc_Core_Opt*)_T23;_T25=
# 2703
Cyc_Tcenv_lookup_opt_type_vars(te);_T26=Cyc_Absyn_new_evar(_T24,_T25);{
# 2702
void*expected_type=
Cyc_Absyn_aqual_handle_type(_T26);_T27=te;_T28=& expected_type;_T29=qual_hndl;_T2A=*_T29;_T2B=
_check_null(_T2A);{void*qhndl_type=Cyc_Tcexp_tcExp(_T27,_T28,_T2B);
void*_TD2=Cyc_Absyn_compress(qhndl_type);void*_TD3;_T2C=(int*)_TD2;_T2D=*_T2C;if(_T2D!=0)goto _TL4CB;_T2E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_T2F=_T2E->f1;_T30=(int*)_T2F;_T31=*_T30;if(_T31!=18)goto _TL4CD;_T32=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_T33=_T32->f2;if(_T33==0)goto _TL4CF;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD2;_T34=_TD4->f2;{struct Cyc_List_List _TD5=*_T34;_T35=_TD5.hd;_TD3=(void*)_T35;}}{void*aq=_TD3;
# 2707
aqual=aq;goto _LL5;}_TL4CF: goto _LL8;_TL4CD: goto _LL8;_TL4CB: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _TD4;_TD4.tag=0;
# 2710
_TD4.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_T36=_TD4;}{struct Cyc_Warn_String_Warn_Warg_struct _TD4=_T36;{struct Cyc_Warn_Typ_Warn_Warg_struct _TD5;_TD5.tag=2;
_TD5.f1=qhndl_type;_T37=_TD5;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TD5=_T37;void*_TD6[2];_TD6[0]=& _TD4;_TD6[1]=& _TD5;_T38=qual_hndl;_T39=*_T38;_T3A=
# 2710
_check_null(_T39);_T3B=_T3A->loc;_T3C=_tag_fat(_TD6,sizeof(void*),2);Cyc_Warn_err2(_T3B,_T3C);}}goto _LL5;_LL5:;}}goto _TL4CA;
# 2715
_TL4C9: if(topt==0)goto _TL4D1;_T3D=
Cyc_Tcenv_curr_aquals_bounds(te);_T3E=topt;_T3F=*_T3E;{void*aq=Cyc_Tcutil_eval_aquals_of(_T3D,_T3F);
if(aq==0)goto _TL4D3;if(aq==0)goto _TL4D5;_T40=aq;_T41=(int*)_T40;_T42=*_T41;if(_T42!=0)goto _TL4D7;_T43=aq;_T44=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T43;_T45=_T44->f1;_T46=(int*)_T45;_T47=*_T46;if(_T47!=16)goto _TL4D9;_T48=aq;_T49=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_T4A=_T49->f1;_T4B=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T4A;_T4C=_T4B->f1;if(_T4C!=Cyc_Absyn_Unique_qual)goto _TL4DB;
# 2720
aqual=Cyc_Absyn_un_qual_type;_T4D=qual_hndl;
*_T4D=Cyc_Absyn_uniqueaqual_exp();goto _LLA;_TL4DB: goto _LLD;_TL4D9: goto _LLD;_TL4D7: goto _LLD;_TL4D5: _LLD: goto _LLA;_LLA: goto _TL4D4;_TL4D3: _TL4D4:;}goto _TL4D2;_TL4D1: _TL4D2: _TL4CA: _T4E=e1;{
# 2728
void*_TD2=_T4E->r;struct Cyc_List_List*_TD3;struct Cyc_Core_Opt*_TD4;_T4F=(int*)_TD2;_T50=*_T4F;switch(_T50){case 26: goto _LL13;case 27: _LL13: {
# 2733
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);_T51=
Cyc_Tcutil_is_array_type(res_typ);if(_T51)goto _TL4DE;else{goto _TL4E0;}
_TL4E0:{struct Cyc_Warn_String_Warn_Warg_struct _TD5;_TD5.tag=0;_TD5.f1=_tag_fat("tcNew: comprehension returned non-array type",sizeof(char),45U);_T52=_TD5;}{struct Cyc_Warn_String_Warn_Warg_struct _TD5=_T52;void*_TD6[1];_TD6[0]=& _TD5;_T54=Cyc_Warn_impos2;{int(*_TD7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T54;_T53=_TD7;}_T55=_tag_fat(_TD6,sizeof(void*),1);_T53(_T55);}goto _TL4DF;_TL4DE: _TL4DF:
 res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,1);
if(topt==0)goto _TL4E1;_T56=loc;_T57=res_typ;_T58=topt;_T59=*_T58;{
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(_T56,_T57,_T59);_T5A=c;_T5B=(int)_T5A;
if(_T5B==0)goto _TL4E3;_T5C=c;_T5D=(int)_T5C;if(_T5D==1)goto _TL4E3;_T5E=e;
_T5E->topt=res_typ;_T5F=e;_T60=topt;_T61=*_T60;
Cyc_Tcutil_unchecked_cast(_T5F,_T61,3U);_T62=topt;
res_typ=*_T62;goto _TL4E4;_TL4E3: _TL4E4:;}goto _TL4E2;_TL4E1: _TL4E2: _T63=res_typ;
# 2745
return _T63;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TD5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_TD2;_TD4=_TD5->f1;_TD3=_TD5->f2;}{struct Cyc_Core_Opt*nopt=_TD4;struct Cyc_List_List*des=_TD3;_T64=e1;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_TD5=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_TD5->tag=25;
# 2747
_TD5->f1=des;_T65=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TD5;}_T64->r=(void*)_T65;_TD3=des;goto _LL17;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_TD5=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TD2;_TD3=_TD5->f1;}_LL17: {struct Cyc_List_List*des=_TD3;
# 2750
void**elt_typ_opt=0;
int zero_term=0;
if(topt==0)goto _TL4E5;_T66=topt;_T67=*_T66;{
void*_TD5=Cyc_Absyn_compress(_T67);void*_TD6;void*_TD7;_T68=(int*)_TD5;_T69=*_T68;if(_T69!=4)goto _TL4E7;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD5;_T6A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD5;_T6B=& _T6A->f1.elt_type;_TD7=(void**)_T6B;_T6C=_TD8->f1;_T6D=_T6C.ptr_atts;_TD6=_T6D.zero_term;}{void**elt_typ=(void**)_TD7;void*zt=_TD6;
# 2756
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);goto _LL1E;}_TL4E7: goto _LL1E;_LL1E:;}goto _TL4E6;_TL4E5: _TL4E6: _T6E=te;_T6F=e1;_T70=_T6F->loc;_T71=elt_typ_opt;_T72=zero_term;_T73=des;{
# 2761
void*res_typ=Cyc_Tcexp_tcArray(_T6E,_T70,_T71,0,_T72,_T73);_T74=e1;
_T74->topt=res_typ;_T75=
Cyc_Tcutil_is_array_type(res_typ);if(_T75)goto _TL4E9;else{goto _TL4EB;}
_TL4EB:{struct Cyc_Warn_String_Warn_Warg_struct _TD5;_TD5.tag=0;_TD5.f1=_tag_fat("tcExpNoPromote on Array_e returned non-array type",sizeof(char),50U);_T76=_TD5;}{struct Cyc_Warn_String_Warn_Warg_struct _TD5=_T76;void*_TD6[1];_TD6[0]=& _TD5;_T78=Cyc_Warn_impos2;{int(*_TD7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T78;_T77=_TD7;}_T79=_tag_fat(_TD6,sizeof(void*),1);_T77(_T79);}goto _TL4EA;_TL4E9: _TL4EA:
 res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,0);
if(topt==0)goto _TL4EC;_T7A=loc;_T7B=res_typ;_T7C=topt;_T7D=*_T7C;{
# 2770
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(_T7A,_T7B,_T7D);_T7E=c;_T7F=(int)_T7E;
if(_T7F==0)goto _TL4EE;_T80=c;_T81=(int)_T80;if(_T81==1)goto _TL4EE;_T82=e;
_T82->topt=res_typ;_T83=e;_T84=topt;_T85=*_T84;
Cyc_Tcutil_unchecked_cast(_T83,_T85,3U);_T86=topt;
res_typ=*_T86;goto _TL4EF;_TL4EE: _TL4EF:;}goto _TL4ED;_TL4EC: _TL4ED: _T87=res_typ;
# 2777
return _T87;}}case 0: _T88=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TD2;_T89=_T88->f1;_T8A=_T89.Wstring_c;_T8B=_T8A.tag;switch(_T8B){case 8: _T8C=Cyc_Absyn_char_type;_T8D=rgn;_T8E=aqual;_T8F=
# 2782
Cyc_Absyn_const_tqual(0U);_T90=Cyc_Absyn_fat_bound_type;_T91=Cyc_Absyn_true_type;_T92=Cyc_Absyn_false_type;{void*topt2=Cyc_Absyn_atb_type(_T8C,_T8D,_T8E,_T8F,_T90,_T91,_T92);_T93=te;_T94=& topt2;_T95=e1;{
# 2784
void*t=Cyc_Tcexp_tcExp(_T93,_T94,_T95);_T96=t;_T97=rgn;_T98=aqual;_T99=
Cyc_Absyn_empty_tqual(0U);_T9A=
Cyc_Absyn_uint_exp(1U,0U);_T9B=Cyc_Absyn_thin_bounds_exp(_T9A);_T9C=Cyc_Absyn_false_type;_T9D=Cyc_Absyn_false_type;_T9E=
# 2785
Cyc_Absyn_atb_type(_T96,_T97,_T98,_T99,_T9B,_T9C,_T9D);return _T9E;}}case 9: _T9F=
# 2789
Cyc_Absyn_wchar_type();_TA0=rgn;_TA1=aqual;_TA2=Cyc_Absyn_const_tqual(0U);_TA3=Cyc_Absyn_fat_bound_type;_TA4=Cyc_Absyn_true_type;_TA5=Cyc_Absyn_false_type;{void*topt2=Cyc_Absyn_atb_type(_T9F,_TA0,_TA1,_TA2,_TA3,_TA4,_TA5);_TA6=te;_TA7=& topt2;_TA8=e1;{
# 2791
void*t=Cyc_Tcexp_tcExp(_TA6,_TA7,_TA8);_TA9=t;_TAA=rgn;_TAB=aqual;_TAC=
Cyc_Absyn_empty_tqual(0U);_TAD=
Cyc_Absyn_uint_exp(1U,0U);_TAE=Cyc_Absyn_thin_bounds_exp(_TAD);_TAF=Cyc_Absyn_false_type;_TB0=Cyc_Absyn_false_type;_TB1=
# 2792
Cyc_Absyn_atb_type(_TA9,_TAA,_TAB,_TAC,_TAE,_TAF,_TB0);return _TB1;}}default: goto _LL1C;};default: _LL1C: {
# 2799
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt==0)goto _TL4F1;_TB2=topt;_TB3=*_TB2;{
void*_TD5=Cyc_Absyn_compress(_TB3);struct Cyc_Absyn_Tqual _TD6;void*_TD7;_TB4=(int*)_TD5;_TB5=*_TB4;switch(_TB5){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD5;_TB6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD5;_TB7=& _TB6->f1.elt_type;_TD7=(void**)_TB7;_TB8=_TD8->f1;_TD6=_TB8.elt_tq;}{void**elttype=(void**)_TD7;struct Cyc_Absyn_Tqual tq=_TD6;
# 2804
topt2=elttype;goto _LL23;}case 0: _TB9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD5;_TBA=_TB9->f1;_TBB=(int*)_TBA;_TBC=*_TBB;if(_TBC!=22)goto _TL4F4;_TBD=topt;
# 2808
bogus=*_TBD;
topt2=& bogus;goto _LL23;_TL4F4: goto _LL28;default: _LL28: goto _LL23;}_LL23:;}goto _TL4F2;_TL4F1: _TL4F2: {
# 2814
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);_TBE=e1;_TBF=_TBE->loc;_TC0=
# 2816
Cyc_Tcenv_curr_aquals_bounds(te);_TC1=telt;_TC2=e1;Cyc_Tcexp_check_consume(_TBF,_TC0,_TC1,_TC2);{
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type_with_aqual(te,aqual);
pi.elt_type=telt;
pi.ptr_atts.eff=rgn;
pi.ptr_atts.bounds=Cyc_Absyn_bounds_one();
pi.ptr_atts.zero_term=Cyc_Absyn_false_type;
pi.ptr_atts.autoreleased=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if(topt==0)goto _TL4F6;_TC3=loc;_TC4=res_typ;_TC5=topt;_TC6=*_TC5;{
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(_TC3,_TC4,_TC6);_TC7=c;_TC8=(int)_TC7;
if(_TC8==0)goto _TL4F8;_TC9=c;_TCA=(int)_TC9;if(_TCA==1)goto _TL4F8;_TCB=e;
_TCB->topt=res_typ;_TCC=e;_TCD=topt;_TCE=*_TCD;_TCF=c;
Cyc_Tcutil_unchecked_cast(_TCC,_TCE,_TCF);_TD0=topt;
res_typ=*_TD0;goto _TL4F9;_TL4F8: _TL4F9:;}goto _TL4F7;_TL4F6: _TL4F7: _TD1=res_typ;
# 2832
return _TD1;}}}}};}}}
# 2839
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){int _T0;struct Cyc_Absyn_Exp*_T1;void*_T2;struct _tuple15 _T3;void*_T4;void*_T5;int*_T6;int _T7;struct Cyc_Absyn_Vardecl*_T8;void**_T9;void*_TA;int*_TB;int _TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);_T0=
Cyc_Tcutil_is_array_type(t);if(!_T0)goto _TL4FA;_T1=e;_T2=t;_T3=
Cyc_Tcutil_addressof_props(e);_T4=_T3.f1;_T5=Cyc_Absyn_al_qual_type;t=Cyc_Tcutil_promote_array(_T2,_T4,_T5,0);_T1->topt=t;goto _TL4FB;_TL4FA: _TL4FB:
 _TL4FC: if(1)goto _TL4FD;else{goto _TL4FE;}
_TL4FD:{void*_TF=Cyc_Absyn_compress(t);struct Cyc_Absyn_Vardecl*_T10;_T6=(int*)_TF;_T7=*_T6;if(_T7!=12)goto _TL4FF;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T11=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TF;_T10=_T11->f1;}{struct Cyc_Absyn_Vardecl*vd=_T10;
# 2846
if(topt!=0)goto _TL501;_T8=vd;
t=_T8->type;
Cyc_Tcutil_unchecked_cast(e,t,1U);goto _TL4FC;
# 2851
_TL501: _T9=topt;_TA=*_T9;{void*_T11=Cyc_Absyn_compress(_TA);_TB=(int*)_T11;_TC=*_TB;if(_TC!=12)goto _TL503;goto _LL5;_TL503: _TD=vd;
# 2854
t=_TD->type;
Cyc_Tcutil_unchecked_cast(e,t,1U);goto _TL4FC;_LL5:;}goto _LL0;}_TL4FF: goto _LL0;_LL0:;}goto _TL4FE;goto _TL4FC;_TL4FE: _TE=t;
# 2864
return _TE;}
# 2870
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;int*_T6;unsigned _T7;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T8;union Cyc_Absyn_Cnst _T9;struct _union_Cnst_String_c _TA;unsigned _TB;void*_TC;int _TD;struct Cyc_Absyn_Exp*_TE;void*_TF;struct _tuple15 _T10;void*_T11;void*_T12;void*_T13;
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);_T0=e;_T1=_T0->loc;_T2=
# 2874
Cyc_Tcenv_curr_aquals_bounds(te);_T3=t;_T4=e;Cyc_Tcexp_check_consume(_T1,_T2,_T3,_T4);_T5=e;{
void*_T14=_T5->r;_T6=(int*)_T14;_T7=*_T6;switch(_T7){case 25: goto _LL4;case 26: _LL4: goto _LL6;case 27: _LL6: goto _LL8;case 0: _T8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T14;_T9=_T8->f1;_TA=_T9.String_c;_TB=_TA.tag;switch(_TB){case 9: _LL8: goto _LLA;case 8: _LLA: _TC=t;
# 2880
return _TC;default: goto _LLB;};default: _LLB: _TD=
# 2882
Cyc_Tcutil_is_array_type(t);if(!_TD)goto _TL507;_TE=e;_TF=t;_T10=
Cyc_Tcutil_addressof_props(e);_T11=_T10.f1;_T12=Cyc_Absyn_al_qual_type;t=Cyc_Tcutil_promote_array(_TF,_T11,_T12,0);_TE->topt=t;goto _TL508;_TL507: _TL508: _T13=t;
return _T13;};}}
# 2895 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;struct Cyc_Absyn_Exp*_T6;int*_T7;int _T8;void*_T9;void*_TA;int*_TB;int _TC;void*_TD;struct Cyc_Absyn_PtrInfo _TE;int*_TF;int _T10;struct _tuple13 _T11;struct Cyc_List_List*(*_T12)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T13)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T14)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T15;struct Cyc_Absyn_FnInfo _T16;struct Cyc_List_List*_T17;struct Cyc_List_List*(*_T18)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T19)(void*(*)(void*),struct Cyc_List_List*);void*(*_T1A)(struct _tuple14*);void*(*_T1B)(struct _tuple0*);struct Cyc_List_List*_T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_Tcenv_Tenv*_T20;struct Cyc_Absyn_Exp*_T21;unsigned _T22;void**_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Exp*_T26;void*_T27;void*_T28;void*_T29;
Cyc_Tcexp_tcExpNoInst(te,topt,e);_T0=e;_T1=_T0->topt;{
void*cached_type=_check_null(_T1);_T2=e;_T3=e;_T4=_T3->topt;_T5=
Cyc_Absyn_pointer_expand(_T4,0);_T2->topt=Cyc_Absyn_compress(_T5);{void*t=_T2->topt;_T6=e;{
# 2900
void*_T2A=_T6->r;struct Cyc_Absyn_Exp*_T2B;_T7=(int*)_T2A;_T8=*_T7;if(_T8!=12)goto _TL509;{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T2C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T2A;_T2B=_T2C->f1;}{struct Cyc_Absyn_Exp*e2=_T2B;_T9=t;
return _T9;}_TL509: goto _LL0;_LL0:;}{void*_T2A;_TA=t;_TB=(int*)_TA;_TC=*_TB;if(_TC!=4)goto _TL50B;_TD=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD;_TE=_T2B->f1;_T2A=_TE.elt_type;}{void*t2=_T2A;
# 2907
void*_T2B=Cyc_Absyn_compress(t2);struct Cyc_Absyn_FnInfo _T2C;_TF=(int*)_T2B;_T10=*_TF;if(_T10!=6)goto _TL50D;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T2D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T2B;_T2C=_T2D->f1;}{struct Cyc_Absyn_FnInfo info=_T2C;{struct _tuple13 _T2D;
# 2909
_T2D.f0=Cyc_Tcenv_lookup_type_vars(te);_T2D.f1=Cyc_Core_heap_region;_T11=_T2D;}{struct _tuple13 env=_T11;_T13=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2D)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T13;_T12=_T2D;}_T14=Cyc_Tcutil_r_make_inst_var;_T15=& env;_T16=info;_T17=_T16.tvars;{struct Cyc_List_List*inst=_T12(_T14,_T15,_T17);_T19=Cyc_List_map;{
# 2913
struct Cyc_List_List*(*_T2D)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T19;_T18=_T2D;}_T1B=Cyc_Core_snd;{void*(*_T2D)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T1B;_T1A=_T2D;}_T1C=inst;{struct Cyc_List_List*ts=_T18(_T1A,_T1C);
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);_T1D=e;{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct));_T2D->tag=13;
_T2D->f1=inner;_T2D->f2=ts;_T1E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T2D;}_T1D->r=(void*)_T1E;_T1F=e;_T20=te;_T21=e;_T22=_T21->loc;_T23=topt;_T24=inner;_T25=ts;
_T1F->topt=Cyc_Tcexp_doInstantiate(_T20,_T22,_T23,_T24,_T25);_T26=e;_T27=_T26->topt;
return _T27;}}}}_TL50D: _T28=t;
return _T28;;}goto _TL50C;_TL50B: _T29=t;
# 2920
return _T29;_TL50C:;}}}}
# 2930
static int Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;int*_T4;int _T5;int _T6;struct Cyc_List_List*_T7;void*_T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Int_Warn_Warg_struct _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_Int_Warn_Warg_struct _TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;int(*_T11)(struct _fat_ptr);void*(*_T12)(struct _fat_ptr);struct _fat_ptr _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;void*_T1D;void*_T1E;int _T1F;struct Cyc_List_List*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;int(*_T25)(struct _fat_ptr);void*(*_T26)(struct _fat_ptr);struct _fat_ptr _T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;int(*_T29)(struct _fat_ptr);void*(*_T2A)(struct _fat_ptr);struct _fat_ptr _T2B;struct Cyc_List_List*_T2C;void*_T2D;struct Cyc_Absyn_Decl*_T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;unsigned _T31;struct Cyc_Absyn_Stmt*_T32;struct Cyc_Absyn_Exp*_T33;unsigned _T34;struct Cyc_Absyn_Stmt*_T35;struct Cyc_Absyn_Exp*_T36;unsigned _T37;struct Cyc_List_List*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;
# 2932
struct Cyc_List_List*decls=0;_T0=fn_exp;{
# 2934
void*_T3C=_T0->r;struct Cyc_List_List*_T3D;_T1=(int*)_T3C;_T2=*_T1;if(_T2!=10)goto _TL50F;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T3E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T3C;_T3D=_T3E->f2;}{struct Cyc_List_List*es=_T3D;_T3=e;{
# 2936
void*_T3E=_T3->r;struct Cyc_List_List*_T3F;_T4=(int*)_T3E;_T5=*_T4;if(_T5!=10)goto _TL511;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T40=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T3E;_T3F=_T40->f2;}{struct Cyc_List_List*es2=_T3F;
# 2938
struct Cyc_List_List*arg_exps=alias_arg_exps;
int arg_cnt=0;
_TL513: if(arg_exps!=0)goto _TL514;else{goto _TL515;}
_TL514: _TL516: _T6=arg_cnt;_T7=arg_exps;_T8=_T7->hd;_T9=(int)_T8;if(_T6!=_T9)goto _TL517;else{goto _TL518;}
_TL517: if(es!=0)goto _TL519;{struct Cyc_Warn_String_Warn_Warg_struct _T40;_T40.tag=0;
_T40.f1=_tag_fat("bad count ",sizeof(char),11U);_TA=_T40;}{struct Cyc_Warn_String_Warn_Warg_struct _T40=_TA;{struct Cyc_Warn_Int_Warn_Warg_struct _T41;_T41.tag=12;_T41.f1=arg_cnt;_TB=_T41;}{struct Cyc_Warn_Int_Warn_Warg_struct _T41=_TB;{struct Cyc_Warn_String_Warn_Warg_struct _T42;_T42.tag=0;_T42.f1=_tag_fat("/",sizeof(char),2U);_TC=_T42;}{struct Cyc_Warn_String_Warn_Warg_struct _T42=_TC;{struct Cyc_Warn_Int_Warn_Warg_struct _T43;_T43.tag=12;_TE=arg_exps;_TF=_TE->hd;_T43.f1=(int)_TF;_TD=_T43;}{struct Cyc_Warn_Int_Warn_Warg_struct _T43=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;
_T44.f1=_tag_fat(" for alias coercion!",sizeof(char),21U);_T10=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T10;void*_T45[5];_T45[0]=& _T40;_T45[1]=& _T41;_T45[2]=& _T42;_T45[3]=& _T43;_T45[4]=& _T44;_T12=Cyc_Warn_impos2;{
# 2943
int(*_T46)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T12;_T11=_T46;}_T13=_tag_fat(_T45,sizeof(void*),5);_T11(_T13);}}}}}goto _TL51A;_TL519: _TL51A:
# 2945
 arg_cnt=arg_cnt + 1;_T14=es;
es=_T14->tl;_T15=
_check_null(es2);es2=_T15->tl;goto _TL516;_TL518: _T16=
# 2950
_check_null(es);_T17=_T16->hd;_T18=(struct Cyc_Absyn_Exp*)_T17;_T19=_check_null(es2);_T1A=_T19->hd;_T1B=(struct Cyc_Absyn_Exp*)_T1A;_T1C=_T1B->topt;_T1D=_check_null(_T1C);_T1E=Cyc_Tcutil_copy_type(_T1D);{struct _tuple12 _T40=Cyc_Tcutil_insert_alias(_T18,_T1E);int _T41;struct Cyc_Absyn_Exp*_T42;struct Cyc_Absyn_Decl*_T43;_T43=_T40.f0;_T42=_T40.f1;_T41=_T40.f2;{struct Cyc_Absyn_Decl*d=_T43;struct Cyc_Absyn_Exp*ve=_T42;int succ=_T41;_T1F=succ;
if(_T1F)goto _TL51B;else{goto _TL51D;}
_TL51D: return 0;_TL51B: _T20=es;_T21=ve;
_T20->hd=(void*)_T21;{struct Cyc_List_List*_T44=_cycalloc(sizeof(struct Cyc_List_List));
_T44->hd=d;_T44->tl=decls;_T22=(struct Cyc_List_List*)_T44;}decls=_T22;_T23=arg_exps;
arg_exps=_T23->tl;}}goto _TL513;_TL515: goto _LL5;}_TL511:{struct Cyc_Warn_String_Warn_Warg_struct _T40;_T40.tag=0;
# 2958
_T40.f1=_tag_fat("not a function call!",sizeof(char),21U);_T24=_T40;}{struct Cyc_Warn_String_Warn_Warg_struct _T40=_T24;void*_T41[1];_T41[0]=& _T40;_T26=Cyc_Warn_impos2;{int(*_T42)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T26;_T25=_T42;}_T27=_tag_fat(_T41,sizeof(void*),1);_T25(_T27);}_LL5:;}goto _LL0;}_TL50F:{struct Cyc_Warn_String_Warn_Warg_struct _T3E;_T3E.tag=0;
# 2961
_T3E.f1=_tag_fat("not a function call!",sizeof(char),21U);_T28=_T3E;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E=_T28;void*_T3F[1];_T3F[0]=& _T3E;_T2A=Cyc_Warn_impos2;{int(*_T40)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2A;_T29=_T40;}_T2B=_tag_fat(_T3F,sizeof(void*),1);_T29(_T2B);}_LL0:;}
# 2964
_TL51E: if(decls!=0)goto _TL51F;else{goto _TL520;}
_TL51F: _T2C=decls;_T2D=_T2C->hd;{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_T2D;_T2E=d;_T2F=fn_exp;_T30=e;_T31=_T30->loc;_T32=
Cyc_Absyn_exp_stmt(_T2F,_T31);_T33=e;_T34=_T33->loc;_T35=Cyc_Absyn_decl_stmt(_T2E,_T32,_T34);_T36=e;_T37=_T36->loc;fn_exp=Cyc_Absyn_stmt_exp(_T35,_T37);_T38=decls;
decls=_T38->tl;}goto _TL51E;_TL520: _T39=e;
# 2970
_T39->topt=0;_T3A=e;_T3B=fn_exp;
_T3A->r=_T3B->r;
return 1;}
# 2976
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T7;int _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;int(*_TA)(struct _fat_ptr);void*(*_TB)(struct _fat_ptr);struct _fat_ptr _TC;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_TD;struct Cyc_Absyn_VarargCallInfo**_TE;struct _handler_cons*_TF;int _T10;void*_T11;struct Cyc_Core_Failure_exn_struct*_T12;char*_T13;char*_T14;struct Cyc_Tcenv_Tenv*_T15;unsigned _T16;void**_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_VarargCallInfo**_T1B;struct Cyc_List_List**_T1C;int _T1D;int _T1E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1F;union Cyc_Absyn_Cnst*_T20;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T21;void**_T22;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T23;enum Cyc_Absyn_Primop*_T24;void*_T25;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T26;enum Cyc_Absyn_Coercion*_T27;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T28;struct Cyc_Absyn_Exp**_T29;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T2A;struct Cyc_Absyn_Exp**_T2B;void*_T2C;void*_T2D;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T2E;int*_T2F;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T30;int*_T31;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T32;int*_T33;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T34;int*_T35;void**_T36;void*_T37;int*_T38;int _T39;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3A;void**_T3B;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3C;struct Cyc_Absyn_Tqual*_T3D;struct Cyc_Absyn_ArrayInfo _T3E;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T3F;int*_T40;void*_T41;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T42;int*_T43;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T44;struct _tuple1**_T45;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T46;struct Cyc_List_List**_T47;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T48;struct Cyc_Absyn_Aggrdecl**_T49;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T4A;void**_T4B;struct Cyc_Absyn_Enumdecl*_T4C;struct _tuple1*_T4D;struct Cyc_Absyn_Enumdecl*_T4E;void*_T4F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T50;enum Cyc_Absyn_MallocKind*_T51;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T52;struct Cyc_Absyn_Exp**_T53;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T54;struct Cyc_Absyn_Exp**_T55;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T56;void***_T57;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T58;struct Cyc_Absyn_Exp**_T59;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T5A;int*_T5B;void*_T5C;struct Cyc_Tcenv_Tenv*_T5D;int _T5E;struct Cyc_Warn_String_Warn_Warg_struct _T5F;struct Cyc_Absyn_Exp*_T60;unsigned _T61;struct _fat_ptr _T62;struct Cyc_Absyn_Exp*_T63;_T0=e;{
unsigned loc=_T0->loc;
void*t;_T1=e;{
# 2980
void*_T64=_T1->r;struct _fat_ptr*_T65;struct Cyc_Absyn_Exp*_T66;void*_T67;void*_T68;int _T69;void*_T6A;enum Cyc_Absyn_Incrementor _T6B;void*_T6C;void*_T6D;void*_T6E;_T2=(int*)_T64;_T3=*_T2;switch(_T3){case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e2=_T6E;
# 2985
Cyc_Tcexp_tcExpNoInst(te,0,e2);_T4=e;_T5=e2;_T6=_T5->topt;
_T4->topt=_check_null(_T6);
return;}case 10: _T7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T64;_T8=_T7->f4;if(_T8!=0)goto _TL522;{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;
# 2989
_T6F.f1=_tag_fat("unresolved function: tcExpNoInst",sizeof(char),33U);_T9=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T9;void*_T70[1];_T70[0]=& _T6F;_TB=Cyc_Warn_impos2;{int(*_T71)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TB;_TA=_T71;}_TC=_tag_fat(_T70,sizeof(void*),1);_TA(_TC);}goto _TL523;_TL522:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_TD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T64;_TE=& _TD->f3;_T6C=(struct Cyc_Absyn_VarargCallInfo**)_TE;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_List_List*es=_T6D;struct Cyc_Absyn_VarargCallInfo**vci=_T6C;
# 3012
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;{struct _handler_cons _T6F;_TF=& _T6F;_push_handler(_TF);{int _T70=0;_T10=setjmp(_T6F.handler);if(!_T10)goto _TL524;_T70=1;goto _TL525;_TL524: _TL525: if(_T70)goto _TL526;else{goto _TL528;}_TL528:
 fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);_pop_handler();goto _TL527;_TL526: _T11=Cyc_Core_get_exn_thrown();{void*_T71=(void*)_T11;void*_T72;_T12=(struct Cyc_Core_Failure_exn_struct*)_T71;_T13=_T12->tag;_T14=Cyc_Core_Failure;if(_T13!=_T14)goto _TL529;
# 3017
ok=0;
fn_exp=e;goto _LL59;_TL529: _T72=_T71;{void*exn=_T72;_rethrow(exn);}_LL59:;}_TL527:;}}_T15=te;_T16=loc;_T17=topt;_T18=e;_T19=e1;_T1A=es;_T1B=vci;_T1C=& alias_arg_exps;
# 3021
t=Cyc_Tcexp_tcFnCall(_T15,_T16,_T17,_T18,_T19,_T1A,_T1B,_T1C);
if(alias_arg_exps==0)goto _TL52B;_T1D=ok;if(!_T1D)goto _TL52B;
alias_arg_exps=Cyc_List_imp_rev(alias_arg_exps);_T1E=
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);if(!_T1E)goto _TL52D;
Cyc_Tcexp_tcExpNoInst(te,topt,e);goto _TL52E;_TL52D: _TL52E: goto _TL52C;_TL52B: _TL52C: goto _LL0;}_TL523:;case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Core_Opt*nopt=_T6E;struct Cyc_List_List*des=_T6D;
# 2993
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T64;_T1F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T64;_T20=& _T1F->f1;_T6E=(union Cyc_Absyn_Cnst*)_T20;}{union Cyc_Absyn_Cnst*c=_T6E;
# 2997
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T64;_T21=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T64;_T22=& _T21->f1;_T6E=(void**)_T22;}{void**b=_T6E;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T64;_T23=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T64;_T24=& _T23->f1;_T6E=(enum Cyc_Absyn_Primop*)_T24;_T6D=_T6F->f2;}{enum Cyc_Absyn_Primop*p=_T6E;struct Cyc_List_List*es=_T6D;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6B=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;enum Cyc_Absyn_Incrementor i=_T6B;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T6C=_T6F->f3;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Core_Opt*popt=_T6D;struct Cyc_Absyn_Exp*e2=_T6C;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T6C=_T6F->f3;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T6D;struct Cyc_Absyn_Exp*e3=_T6C;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T6D;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T6D;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T6D;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11:{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e1=_T6E;
# 3028
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Exp*e2=_T6E;struct Cyc_List_List*ts=_T6D;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T64;_T25=_T6F->f1;_T6E=(void*)_T25;_T6D=_T6F->f2;_T26=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T64;_T27=& _T26->f4;_T6C=(enum Cyc_Absyn_Coercion*)_T27;}{void*t1=_T6E;struct Cyc_Absyn_Exp*e1=_T6D;enum Cyc_Absyn_Coercion*c=_T6C;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e1=_T6E;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T64;_T28=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T64;_T29=& _T28->f1;_T6E=(struct Cyc_Absyn_Exp**)_T29;_T6D=_T6F->f2;_T2A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T64;_T2B=& _T2A->f3;_T6C=(struct Cyc_Absyn_Exp**)_T2B;}{struct Cyc_Absyn_Exp**rgn_handle=_T6E;struct Cyc_Absyn_Exp*e1=_T6D;struct Cyc_Absyn_Exp**qual_hndl=_T6C;
# 3033
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,qual_hndl,e,e1);goto _LL0;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e1=_T6E;_T6E=
# 3035
Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T64;_T2C=_T6F->f1;_T6E=(void*)_T2C;}_LL2A: {void*t1=_T6E;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T64;_T2D=_T6F->f1;_T6E=(void*)_T2D;_T6D=_T6F->f2;}{void*t1=_T6E;struct Cyc_List_List*l=_T6D;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e1=_T6E;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e,e1);goto _LL0;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T2E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T64;_T2F=& _T2E->f3;_T6C=(int*)_T2F;_T30=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T64;_T31=& _T30->f4;_T6A=(int*)_T31;}{struct Cyc_Absyn_Exp*e1=_T6E;struct _fat_ptr*f=_T6D;int*is_tagged=_T6C;int*is_read=_T6A;
# 3040
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T32=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T64;_T33=& _T32->f3;_T6C=(int*)_T33;_T34=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T64;_T35=& _T34->f4;_T6A=(int*)_T35;}{struct Cyc_Absyn_Exp*e1=_T6E;struct _fat_ptr*f=_T6D;int*is_tagged=_T6C;int*is_read=_T6A;
# 3042
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T6D;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e,e1,e2);goto _LL0;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct _tuple9*t1=_T6E;struct Cyc_List_List*des=_T6D;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_List_List*des=_T6E;
# 3048
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt==0)goto _TL52F;_T36=topt;_T37=*_T36;{
void*_T6F=Cyc_Absyn_compress(_T37);void*_T70;void*_T71;void*_T72;_T38=(int*)_T6F;_T39=*_T38;if(_T39!=5)goto _TL531;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T73=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6F;_T3A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6F;_T3B=& _T3A->f1.elt_type;_T72=(void**)_T3B;_T3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6F;_T3D=& _T3C->f1.tq;_T71=(struct Cyc_Absyn_Tqual*)_T3D;_T3E=_T73->f1;_T70=_T3E.zero_term;}{void**et=(void**)_T72;struct Cyc_Absyn_Tqual*etq=(struct Cyc_Absyn_Tqual*)_T71;void*zt=_T70;
# 3054
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);goto _LL5E;}_TL531: goto _LL5E;_LL5E:;}goto _TL530;_TL52F: _TL530:
# 3060
 t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Stmt*s=_T6E;
# 3062
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T6C=_T6F->f3;_T3F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T64;_T40=& _T3F->f4;_T6A=(int*)_T40;}{struct Cyc_Absyn_Vardecl*vd=_T6E;struct Cyc_Absyn_Exp*e1=_T6D;struct Cyc_Absyn_Exp*e2=_T6C;int*iszeroterm=_T6A;
# 3064
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T41=_T6F->f2;_T6D=(void*)_T41;_T42=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T64;_T43=& _T42->f3;_T6C=(int*)_T43;}{struct Cyc_Absyn_Exp*e1=_T6E;void*t1=_T6D;int*iszeroterm=_T6C;
# 3066
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T64;_T44=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T64;_T45=& _T44->f1;_T6E=(struct _tuple1**)_T45;_T46=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T64;_T47=& _T46->f2;_T6D=(struct Cyc_List_List**)_T47;_T6C=_T6F->f3;_T48=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T64;_T49=& _T48->f4;_T6A=(struct Cyc_Absyn_Aggrdecl**)_T49;}{struct _tuple1**tn=_T6E;struct Cyc_List_List**ts=_T6D;struct Cyc_List_List*args=_T6C;struct Cyc_Absyn_Aggrdecl**sd_opt=_T6A;
# 3068
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T64;_T4A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T64;_T4B=& _T4A->f1;_T6E=(void**)_T4B;_T69=_T6F->f2;_T6D=_T6F->f3;}{void**ts=_T6E;int is_tuple=_T69;struct Cyc_List_List*args=_T6D;
# 3070
t=Cyc_Tcexp_tcAnonStruct(te,loc,topt,e,ts,is_tuple,args);goto _LL0;}case 30:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;_T6C=_T6F->f3;}{struct Cyc_List_List*es=_T6E;struct Cyc_Absyn_Datatypedecl*tud=_T6D;struct Cyc_Absyn_Datatypefield*tuf=_T6C;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 31:{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T6D=_T6F->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T6E;struct Cyc_Absyn_Enumfield*ef=_T6D;_T4C=ed;_T4D=_T4C->name;_T4E=ed;
t=Cyc_Absyn_enum_type(_T4D,_T4E);goto _LL0;}case 32:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T64;_T4F=_T6F->f1;_T6E=(void*)_T4F;_T6D=_T6F->f2;}{void*t2=_T6E;struct Cyc_Absyn_Enumfield*ef=_T6D;
t=t2;goto _LL0;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T50=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T51=& _T50->f1.mknd;_T6E=(enum Cyc_Absyn_MallocKind*)_T51;_T52=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T53=& _T52->f1.rgn;_T6D=(struct Cyc_Absyn_Exp**)_T53;_T54=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T55=& _T54->f1.aqual;_T6C=(struct Cyc_Absyn_Exp**)_T55;_T56=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T57=& _T56->f1.elt_type;_T6A=(void***)_T57;_T58=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T59=& _T58->f1.num_elts;_T68=(struct Cyc_Absyn_Exp**)_T59;_T5A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T64;_T5B=& _T5A->f1.fat_result;_T67=(int*)_T5B;}{enum Cyc_Absyn_MallocKind*mknd=_T6E;struct Cyc_Absyn_Exp**ropt=(struct Cyc_Absyn_Exp**)_T6D;struct Cyc_Absyn_Exp**aqopt=(struct Cyc_Absyn_Exp**)_T6C;void***t2=(void***)_T6A;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_T68;int*isfat=(int*)_T67;
# 3075
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,aqopt,t2,e2,mknd,isfat);goto _LL0;}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T66=_T6F->f2;}{struct Cyc_Absyn_Exp*e1=_T6E;struct Cyc_Absyn_Exp*e2=_T66;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T65=_T6F->f2;}{struct Cyc_Absyn_Exp*e=_T6E;struct _fat_ptr*f=_T65;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e1=_T6E;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;_T69=_T6F->f2;}{struct Cyc_Absyn_Exp*e=_T6E;int static_only=_T69;
t=Cyc_Tcexp_tcAssert(te,loc,e,static_only);goto _LL0;}case 42:{struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_T64;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e=_T6E;
t=Cyc_Tcexp_tcAssert(te,loc,e,1);goto _LL0;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T64;_T5C=_T6F->f1;_T6E=(void*)_T5C;}{void*t2=_T6E;_T5D=te;_T5E=_T5D->allow_valueof;
# 3082
if(_T5E)goto _TL533;else{goto _TL535;}
_TL535:{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;_T6F.f1=_tag_fat("valueof(-) can only occur within types",sizeof(char),39U);_T5F=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T5F;void*_T70[1];_T70[0]=& _T6F;_T60=e;_T61=_T60->loc;_T62=_tag_fat(_T70,sizeof(void*),1);Cyc_Warn_err2(_T61,_T62);}goto _TL534;_TL533: _TL534:
# 3090
 t=Cyc_Absyn_sint_type;goto _LL0;}default:
# 3094
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}_T63=e;
# 3096
_T63->topt=t;}}
