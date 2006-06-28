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
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 205
extern struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*);
# 254
extern int Cyc_List_forall_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_SubsetType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_AssnDef_ExistAssnFn*f3;};
# 551 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
# 921 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 936
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 942
void*Cyc_Absyn_compress(void*);
# 946
int Cyc_Absyn_type2bool(int,void*);
# 977
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 1012
void*Cyc_Absyn_bounds_one (void);
# 1015
void*Cyc_Absyn_fatconst (void);
void*Cyc_Absyn_thinconst (void);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
# 20
void*Cyc_BansheeIf_implication_constraint(void*,void*);
# 24
void*Cyc_BansheeIf_cmpeq_constraint(void*,void*);
# 28
int Cyc_BansheeIf_add_constraint(unsigned,void*);
# 31 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 35
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_strict_arithmetic_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
# 54
int Cyc_Tcutil_is_bits_only_type(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 82
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 99
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 134
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 146
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 274
int Cyc_Tcutil_cmp_effect_constraints(struct Cyc_List_List*,struct Cyc_List_List*);
# 277
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
void*Cyc_Tcutil_get_pointer_bounds(void*);
# 285
int Cyc_Tcutil_will_lose_precision(void*,void*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
int Cyc_Unify_unify_c(void*,void*,int(*)(void*,void*,void*),void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
# 93 "kinds.h"
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 65 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Set_Set;
# 189 "assndef.h"
extern void*Cyc_AssnDef_fresh_var(struct Cyc_Absyn_Vardecl*,void*);struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};
# 233 "assndef.h"
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};
# 283
extern void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*,struct Cyc_List_List*);
# 315
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{enum Cyc_Absyn_Coercion f0;enum Cyc_Absyn_Coercion f1;};
# 41 "subtype.cyc"
static enum Cyc_Absyn_Coercion Cyc_Subtype_join_coercion(enum Cyc_Absyn_Coercion c1,enum Cyc_Absyn_Coercion c2){enum Cyc_Absyn_Coercion _T0;int _T1;enum Cyc_Absyn_Coercion _T2;int _T3;enum Cyc_Absyn_Coercion _T4;struct _tuple11 _T5;enum Cyc_Absyn_Coercion _T6;enum Cyc_Absyn_Coercion _T7;enum Cyc_Absyn_Coercion _T8;enum Cyc_Absyn_Coercion _T9;enum Cyc_Absyn_Coercion _TA;enum Cyc_Absyn_Coercion _TB;int _TC;enum Cyc_Absyn_Coercion _TD;enum Cyc_Absyn_Coercion _TE;_T0=c1;_T1=(int)_T0;_T2=c2;_T3=(int)_T2;
if(_T1!=_T3)goto _TL0;_T4=c1;return _T4;_TL0:{struct _tuple11 _TF;
_TF.f0=c1;_TF.f1=c2;_T5=_TF;}{struct _tuple11 _TF=_T5;enum Cyc_Absyn_Coercion _T10;_T6=_TF.f0;if(_T6!=Cyc_Absyn_Unknown_coercion)goto _TL2;goto _LL4;_TL2: _T7=_TF.f1;if(_T7!=Cyc_Absyn_Unknown_coercion)goto _TL4;_LL4:
# 45
 return 0U;_TL4: _T8=_TF.f0;if(_T8!=Cyc_Absyn_No_coercion)goto _TL6;_T10=_TF.f1;{enum Cyc_Absyn_Coercion o=_T10;_T10=o;goto _LL8;}_TL6: _T9=_TF.f1;if(_T9!=Cyc_Absyn_No_coercion)goto _TL8;_T10=_TF.f0;_LL8:{enum Cyc_Absyn_Coercion o=_T10;_TA=o;
# 47
return _TA;}_TL8: _TB=_TF.f0;_TC=(int)_TB;switch(_TC){case Cyc_Absyn_Null_to_NonNull: _TD=_TF.f1;if(_TD!=Cyc_Absyn_Other_coercion)goto _TLB;goto _LLC;_TLB: goto _LLD;case Cyc_Absyn_Other_coercion: _TE=_TF.f1;if(_TE!=Cyc_Absyn_Null_to_NonNull)goto _TLD;_LLC:
# 49
 return 2U;_TLD: goto _LLD;default: _LLD:
 return 0U;};}}
# 57
static int Cyc_Subtype_unify_cvar(struct Cyc_List_List**env,void*t1,void*t2){int _T0;int _T1;void*_T2;unsigned _T3;void*_T4;unsigned _T5;struct Cyc_List_List**_T6;struct Cyc_List_List*_T7;struct Cyc_List_List**_T8;_T0=
Cyc_Tcutil_is_cvar_type(t1);if(_T0)goto _TL11;else{goto _TL12;}_TL12: _T1=Cyc_Tcutil_is_cvar_type(t2);if(_T1)goto _TL11;else{goto _TLF;}
_TL11:{void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);_T2=cv1;_T3=(unsigned)_T2;
if(!_T3)goto _TL13;_T4=cv2;_T5=(unsigned)_T4;if(!_T5)goto _TL13;_T6=env;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));
_T9->hd=Cyc_BansheeIf_equality_constraint(cv1,cv2);_T8=_check_null(env);_T9->tl=*_T8;_T7=(struct Cyc_List_List*)_T9;}*_T6=_T7;
return 1;_TL13:;}goto _TL10;_TLF: _TL10:
# 66
 return 0;}struct _tuple12{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;};
# 70
static struct _tuple12 Cyc_Subtype_force_equivalence(void*t1,void*t2){int(*_T0)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**);int(*_T1)(void*,void*,int(*)(void*,void*,void*),void*);void*_T2;void*_T3;struct Cyc_List_List**_T4;int _T5;struct _tuple12 _T6;struct _tuple12 _T7;
struct Cyc_List_List*retc=0;_T1=Cyc_Unify_unify_c;{
int(*_T8)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**)=(int(*)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**))_T1;_T0=_T8;}_T2=t1;_T3=t2;_T4=& retc;_T5=_T0(_T2,_T3,Cyc_Subtype_unify_cvar,_T4);if(!_T5)goto _TL15;{struct _tuple12 _T8;
_T8.f0=1U;_T8.f1=retc;_T6=_T8;}return _T6;_TL15:{struct _tuple12 _T8;
# 75
_T8.f0=0U;_T8.f1=0;_T7=_T8;}return _T7;}
# 78
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*,void*,void*);
# 81
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*,int,void*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;int f2;};struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;};
# 83
static struct Cyc_List_List*Cyc_Subtype_flatten_type_f(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*x){struct _tuple13*_T0;void*_T1;struct Cyc_Absyn_Aggrfield*_T2;struct _RegionHandle*_T3;struct Cyc_List_List*_T4;struct Cyc_Absyn_Aggrfield*_T5;void*_T6;int _T7;struct Cyc_List_List*_T8;struct _RegionHandle*_T9;struct _tuple14*_TA;struct _RegionHandle*_TB;struct Cyc_Absyn_Aggrfield*_TC;struct Cyc_List_List*_TD;_T0=env;{
# 86
struct _tuple13 _TE=*_T0;int _TF;struct _RegionHandle*_T10;struct Cyc_List_List*_T11;_T11=_TE.f0;_T10=_TE.f1;_TF=_TE.f2;{struct Cyc_List_List*inst=_T11;struct _RegionHandle*r=_T10;int flatten=_TF;
if(inst!=0)goto _TL17;_T2=x;_T1=_T2->type;goto _TL18;_TL17: _T3=r;_T4=inst;_T5=x;_T6=_T5->type;_T1=Cyc_Tcutil_rsubstitute(_T3,_T4,_T6);_TL18: {void*t=_T1;
struct Cyc_List_List*ts=Cyc_Subtype_flatten_type(r,flatten,t);_T7=
Cyc_List_length(ts);if(_T7!=1)goto _TL19;_T9=r;{struct Cyc_List_List*_T12=_region_malloc(_T9,0U,sizeof(struct Cyc_List_List));_TB=r;{struct _tuple14*_T13=_region_malloc(_TB,0U,sizeof(struct _tuple14));_TC=x;
_T13->f0=_TC->tq;_T13->f1=t;_TA=(struct _tuple14*)_T13;}_T12->hd=_TA;_T12->tl=0;_T8=(struct Cyc_List_List*)_T12;}return _T8;_TL19: _TD=ts;
return _TD;}}}}
# 94
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*r,int flatten,void*t1){int _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TC;union Cyc_Absyn_AggrInfo _TD;struct _union_AggrInfo_KnownAggr _TE;unsigned _TF;void*_T10;void*_T11;union Cyc_Absyn_AggrInfo _T12;struct _union_AggrInfo_KnownAggr _T13;struct Cyc_Absyn_Aggrdecl**_T14;struct Cyc_Absyn_Aggrdecl*_T15;enum Cyc_Absyn_AggrKind _T16;int _T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_AggrdeclImpl*_T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_AggrdeclImpl*_T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Aggrdecl*_T1D;struct Cyc_Absyn_AggrdeclImpl*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct _RegionHandle*_T21;struct _tuple14*_T22;struct _RegionHandle*_T23;struct _RegionHandle*_T24;struct _RegionHandle*_T25;struct Cyc_Absyn_Aggrdecl*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct _tuple13 _T29;struct Cyc_Absyn_Aggrdecl*_T2A;struct Cyc_Absyn_AggrdeclImpl*_T2B;struct Cyc_Absyn_AggrdeclImpl*_T2C;void*_T2D;struct _tuple13*_T2E;struct Cyc_Absyn_Aggrfield*_T2F;struct Cyc_List_List*(*_T30)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T31)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T32;struct _tuple13*_T33;struct Cyc_List_List*_T34;struct _RegionHandle*_T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;void*_T39;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T3A;enum Cyc_Absyn_AggrKind _T3B;void*_T3C;struct _tuple13 _T3D;struct Cyc_List_List*_T3E;void*_T3F;struct _tuple13*_T40;struct Cyc_Absyn_Aggrfield*_T41;struct Cyc_List_List*(*_T42)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T43)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T44;struct _tuple13*_T45;struct Cyc_List_List*_T46;struct _RegionHandle*_T47;struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;struct _RegionHandle*_T4C;struct _tuple14*_T4D;struct _RegionHandle*_T4E;_T0=flatten;
# 96
if(!_T0)goto _TL1B;
t1=Cyc_Absyn_compress(t1);{struct Cyc_List_List*_T4F;struct Cyc_Absyn_Aggrdecl*_T50;_T1=t1;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=t1;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 0:
# 99
 return 0;case 24: _T9=t1;_TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9;_TB=_TA->f1;_TC=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TB;_TD=_TC->f1;_TE=_TD.KnownAggr;_TF=_TE.tag;if(_TF!=2)goto _TL1F;_T10=t1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10;_T11=_T51->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T52=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T11;_T12=_T52->f1;_T13=_T12.KnownAggr;_T14=_T13.val;{struct Cyc_Absyn_Aggrdecl*_T53=*_T14;_T50=_T53;}}_T4F=_T51->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T50;struct Cyc_List_List*ts=_T4F;_T15=ad;_T16=_T15->kind;_T17=(int)_T16;
# 102
if(_T17==1)goto _TL23;else{goto _TL26;}_TL26: _T18=ad;_T19=_T18->impl;if(_T19==0)goto _TL23;else{goto _TL25;}_TL25: _T1A=ad;_T1B=_T1A->impl;_T1C=_T1B->exist_vars;if(_T1C!=0)goto _TL23;else{goto _TL24;}_TL24: _T1D=ad;_T1E=_T1D->impl;_T1F=_T1E->effconstr;if(_T1F!=0)goto _TL23;else{goto _TL21;}
# 104
_TL23: _T21=r;{struct Cyc_List_List*_T51=_region_malloc(_T21,0U,sizeof(struct Cyc_List_List));_T23=r;{struct _tuple14*_T52=_region_malloc(_T23,0U,sizeof(struct _tuple14));_T52->f0=Cyc_Absyn_empty_tqual(0U);_T52->f1=t1;_T22=(struct _tuple14*)_T52;}_T51->hd=_T22;_T51->tl=0;_T20=(struct Cyc_List_List*)_T51;}return _T20;_TL21: _T24=r;_T25=r;_T26=ad;_T27=_T26->tvs;_T28=ts;{
struct Cyc_List_List*inst=Cyc_List_rzip(_T24,_T25,_T27,_T28);{struct _tuple13 _T51;
_T51.f0=inst;_T51.f1=r;_T51.f2=flatten;_T29=_T51;}{struct _tuple13 env=_T29;_T2A=ad;_T2B=_T2A->impl;_T2C=
_check_null(_T2B);{struct Cyc_List_List*_T51=_T2C->fields;struct Cyc_List_List*_T52;struct Cyc_Absyn_Aggrfield*_T53;if(_T51!=0)goto _TL27;
return 0;_TL27:{struct Cyc_List_List _T54=*_T51;_T2D=_T54.hd;_T53=(struct Cyc_Absyn_Aggrfield*)_T2D;_T52=_T54.tl;}{struct Cyc_Absyn_Aggrfield*hd=_T53;struct Cyc_List_List*tl=_T52;_T2E=& env;_T2F=hd;{
# 110
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(_T2E,_T2F);
env.f2=0;_T31=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T54)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))_T31;_T30=_T54;}_T32=r;_T33=& env;_T34=tl;{struct Cyc_List_List*tl2=_T30(_T32,Cyc_Subtype_flatten_type_f,_T33,_T34);
struct Cyc_List_List*tts;_T35=r;tts=_region_malloc(_T35,0U,sizeof(struct Cyc_List_List));_T36=tts;_T36->hd=hd2;_T37=tts;_T37->tl=tl2;_T38=
Cyc_List_rflatten(r,tts);return _T38;}}};}}}}goto _TL20;_TL1F: goto _LL7;_TL20:;default: goto _LL7;};case 7: _T39=t1;_T3A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T39;_T3B=_T3A->f1;if(_T3B!=Cyc_Absyn_StructA)goto _TL29;_T3C=t1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T3C;_T4F=_T51->f3;}{struct Cyc_List_List*fs=_T4F;{struct _tuple13 _T51;
# 117
_T51.f0=0;_T51.f1=r;_T51.f2=flatten;_T3D=_T51;}{struct _tuple13 env=_T3D;struct Cyc_List_List*_T51;struct Cyc_Absyn_Aggrfield*_T52;if(fs!=0)goto _TL2B;
# 119
return 0;_TL2B: _T3E=fs;{struct Cyc_List_List _T53=*_T3E;_T3F=_T53.hd;_T52=(struct Cyc_Absyn_Aggrfield*)_T3F;_T51=_T53.tl;}{struct Cyc_Absyn_Aggrfield*hd=_T52;struct Cyc_List_List*tl=_T51;_T40=& env;_T41=hd;{
# 121
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(_T40,_T41);
env.f2=0;_T43=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T53)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))_T43;_T42=_T53;}_T44=r;_T45=& env;_T46=tl;{struct Cyc_List_List*tl2=_T42(_T44,Cyc_Subtype_flatten_type_f,_T45,_T46);
struct Cyc_List_List*tts;_T47=r;tts=_region_malloc(_T47,0U,sizeof(struct Cyc_List_List));_T48=tts;_T48->hd=hd2;_T49=tts;_T49->tl=tl2;_T4A=
Cyc_List_rflatten(r,tts);return _T4A;}}};}}goto _TL2A;_TL29: goto _LL7;_TL2A:;default: _LL7: goto _LL0;}_LL0:;}goto _TL1C;_TL1B: _TL1C: _T4C=r;{struct Cyc_List_List*_T4F=_region_malloc(_T4C,0U,sizeof(struct Cyc_List_List));_T4E=r;{struct _tuple14*_T50=_region_malloc(_T4E,0U,sizeof(struct _tuple14));
# 130
_T50->f0=Cyc_Absyn_empty_tqual(0U);_T50->f1=t1;_T4D=(struct _tuple14*)_T50;}_T4F->hd=_T4D;_T4F->tl=0;_T4B=(struct Cyc_List_List*)_T4F;}return _T4B;}struct _tuple15{void*f0;void*f1;};
# 136
int Cyc_Subtype_check_aqual_bounds(struct Cyc_List_List*aquals_bnd,void*aq,void*bnd){struct _tuple15 _T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;int*_TA;int _TB;void*_TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;void*_T11;void*_T12;void*_T13;void*_T14;int _T15;enum Cyc_Absyn_AliasQualVal _T16;int _T17;enum Cyc_Absyn_AliasQualVal _T18;int _T19;enum Cyc_Absyn_AliasQualVal _T1A;int _T1B;void*_T1C;int*_T1D;int _T1E;void*_T1F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20;void*_T21;int*_T22;int _T23;void*_T24;void*_T25;void*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;void*_T2C;int _T2D;void*_T2E;int*_T2F;int _T30;void*_T31;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T32;void*_T33;int*_T34;int _T35;void*_T36;void*_T37;struct _handler_cons*_T38;int _T39;int(*_T3A)(void*,void*);struct Cyc_List_List*_T3B;void*_T3C;void*_T3D;struct Cyc_Core_Not_found_exn_struct*_T3E;char*_T3F;char*_T40;struct Cyc_Warn_String_Warn_Warg_struct _T41;struct Cyc_Warn_Typ_Warn_Warg_struct _T42;struct Cyc_Warn_String_Warn_Warg_struct _T43;struct Cyc_Warn_Typ_Warn_Warg_struct _T44;int(*_T45)(struct _fat_ptr);void*(*_T46)(struct _fat_ptr);struct _fat_ptr _T47;{struct _tuple15 _T48;
# 138
_T48.f0=Cyc_Absyn_compress(aq);_T48.f1=Cyc_Absyn_compress(bnd);_T0=_T48;}{struct _tuple15 _T48=_T0;struct Cyc_List_List*_T49;enum Cyc_Absyn_AliasQualVal _T4A;enum Cyc_Absyn_AliasQualVal _T4B;_T1=_T48.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL2D;_T4=_T48.f0;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 16: _T9=_T48.f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=0)goto _TL30;_TC=_T48.f1;_TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=16)goto _TL32;_T11=_T48.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T12=_T4C->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4D=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T12;_T4B=_T4D->f1;}}_T13=_T48.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13;_T14=_T4C->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4D=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T14;_T4A=_T4D->f1;}}{enum Cyc_Absyn_AliasQualVal v_sub=_T4B;enum Cyc_Absyn_AliasQualVal v_sup=_T4A;_T16=v_sup;_T17=(int)_T16;_T18=v_sub;_T19=(int)_T18;
# 140
if(_T17!=_T19)goto _TL34;_T15=1;goto _TL35;_TL34: _T1A=v_sup;_T1B=(int)_T1A;_T15=_T1B==3;_TL35: return _T15;}_TL32: goto _LL7;_TL30: goto _LL7;case 17: _T1C=_T48.f1;_T1D=(int*)_T1C;_T1E=*_T1D;if(_T1E!=0)goto _TL36;_T1F=_T48.f1;_T20=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1F;_T21=_T20->f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=16)goto _TL38;_T24=_T48.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24;_T49=_T4C->f2;}_T25=_T48.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T26=_T4C->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4D=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T26;_T4B=_T4D->f1;}}{struct Cyc_List_List*tv_sub=_T49;enum Cyc_Absyn_AliasQualVal v_sup=_T4B;_T27=aquals_bnd;_T28=
# 142
_check_null(tv_sub);_T29=_T28->tl;_T2A=_check_null(_T29);_T2B=_T2A->hd;_T2C=bnd;_T2D=Cyc_Subtype_check_aqual_bounds(_T27,_T2B,_T2C);return _T2D;}_TL38: goto _LL7;_TL36: goto _LL7;case 15: _T2E=_T48.f1;_T2F=(int*)_T2E;_T30=*_T2F;if(_T30!=0)goto _TL3A;_T31=_T48.f1;_T32=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T31;_T33=_T32->f1;_T34=(int*)_T33;_T35=*_T34;if(_T35!=16)goto _TL3C;_T36=_T48.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T37=_T4C->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_T4D=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T37;_T4B=_T4D->f1;}}{enum Cyc_Absyn_AliasQualVal v_sup=_T4B;struct _handler_cons _T4C;_T38=& _T4C;_push_handler(_T38);{int _T4D=0;_T39=setjmp(_T4C.handler);if(!_T39)goto _TL3E;_T4D=1;goto _TL3F;_TL3E: _TL3F: if(_T4D)goto _TL40;else{goto _TL42;}_TL42: _T3A=Cyc_Tcutil_typecmp;_T3B=aquals_bnd;_T3C=aq;{
# 145
void*aq_bound=Cyc_List_assoc_cmp(_T3A,_T3B,_T3C);int _T4E=
Cyc_Subtype_check_aqual_bounds(aquals_bnd,aq_bound,bnd);_npop_handler(0);return _T4E;}_pop_handler();goto _TL41;_TL40: _T3D=Cyc_Core_get_exn_thrown();{void*_T4E=(void*)_T3D;void*_T4F;_T3E=(struct Cyc_Core_Not_found_exn_struct*)_T4E;_T3F=_T3E->tag;_T40=Cyc_Core_Not_found;if(_T3F!=_T40)goto _TL43;
# 149
return 0;_TL43: _T4F=_T4E;{void*exn=_T4F;_rethrow(exn);};}_TL41:;}}goto _TL3D;_TL3C: goto _LL7;_TL3D: goto _TL3B;_TL3A: goto _LL7;_TL3B:;default: goto _LL7;}goto _TL2E;_TL2D: _LL7:{struct Cyc_Warn_String_Warn_Warg_struct _T4C;_T4C.tag=0;
# 152
_T4C.f1=_tag_fat("check_aqual_bounds expects a constant bound; got ",sizeof(char),50U);_T41=_T4C;}{struct Cyc_Warn_String_Warn_Warg_struct _T4C=_T41;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4D;_T4D.tag=2;_T4D.f1=aq;_T42=_T4D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4D=_T42;{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat(",",sizeof(char),2U);_T43=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T43;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F;_T4F.tag=2;_T4F.f1=bnd;_T44=_T4F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4F=_T44;void*_T50[4];_T50[0]=& _T4C;_T50[1]=& _T4D;_T50[2]=& _T4E;_T50[3]=& _T4F;_T46=Cyc_Warn_impos2;{int(*_T51)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T46;_T45=_T51;}_T47=_tag_fat(_T50,sizeof(void*),4);_T45(_T47);}}}}_TL2E:;}}
# 166 "subtype.cyc"
static int Cyc_Subtype_alias_qual_subtype(void*aq_sub,void*aq_sup,int mode){struct _tuple15 _T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;int*_TA;int _TB;void*_TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;void*_T11;void*_T12;void*_T13;void*_T14;enum Cyc_Absyn_AliasQualVal _T15;int _T16;enum Cyc_Absyn_AliasQualVal _T17;int _T18;int _T19;int _T1A;enum Cyc_Absyn_AliasQualVal _T1B;int _T1C;enum Cyc_Absyn_AliasQualVal _T1D;int _T1E;enum Cyc_Absyn_AliasQualVal _T1F;int _T20;int _T21;enum Cyc_Absyn_AliasQualVal _T22;int _T23;enum Cyc_Absyn_AliasQualVal _T24;int _T25;enum Cyc_Absyn_AliasQualVal _T26;int _T27;int _T28;enum Cyc_Absyn_AliasQualVal _T29;int _T2A;int _T2B;enum Cyc_Absyn_AliasQualVal _T2C;int _T2D;enum Cyc_Absyn_AliasQualVal _T2E;int _T2F;enum Cyc_Absyn_AliasQualVal _T30;int _T31;void*_T32;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;void*_T39;int*_T3A;unsigned _T3B;void*_T3C;int*_T3D;int _T3E;void*_T3F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T40;void*_T41;int*_T42;unsigned _T43;void*_T44;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T45;struct Cyc_List_List*_T46;void*_T47;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;void*_T4B;int*_T4C;unsigned _T4D;void*_T4E;struct Cyc_List_List*_T4F;void*_T50;void*_T51;struct Cyc_List_List*_T52;void*_T53;struct Cyc_Absyn_Tvar*_T54;int _T55;struct Cyc_Absyn_Tvar*_T56;int _T57;int _T58;void*_T59;struct Cyc_List_List*_T5A;void*_T5B;void*_T5C;struct Cyc_List_List*_T5D;void*_T5E;struct Cyc_List_List*_T5F;void*_T60;struct Cyc_List_List*_T61;void*_T62;int _T63;void*_T64;struct Cyc_List_List*_T65;void*_T66;void*_T67;void*_T68;struct Cyc_List_List*_T69;void*_T6A;void*_T6B;int _T6C;int _T6D;void*_T6E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;void*_T72;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T73;void*_T74;int*_T75;int _T76;void*_T77;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;void*_T7B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7C;struct Cyc_List_List*_T7D;void*_T7E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7F;struct Cyc_List_List*_T80;struct Cyc_List_List*_T81;void*_T82;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T83;struct Cyc_List_List*_T84;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;void*_T87;int*_T88;int _T89;void*_T8A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8B;void*_T8C;int*_T8D;int _T8E;void*_T8F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T90;struct Cyc_List_List*_T91;void*_T92;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T93;struct Cyc_List_List*_T94;struct Cyc_List_List*_T95;void*_T96;int*_T97;int _T98;void*_T99;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9A;struct Cyc_List_List*_T9B;struct Cyc_List_List*_T9C;void*_T9D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9E;void*_T9F;int*_TA0;int _TA1;void*_TA2;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA3;struct Cyc_List_List*_TA4;struct Cyc_List_List*_TA5;void*_TA6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7;struct Cyc_List_List*_TA8;void*_TA9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAA;struct Cyc_List_List*_TAB;struct Cyc_List_List*_TAC;void*_TAD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAE;struct Cyc_List_List*_TAF;struct Cyc_List_List*_TB0;struct Cyc_List_List*_TB1;void*_TB2;struct Cyc_List_List*_TB3;void*_TB4;struct Cyc_List_List*_TB5;void*_TB6;void*_TB7;struct Cyc_List_List*_TB8;void*_TB9;struct Cyc_List_List*_TBA;void*_TBB;int _TBC;int _TBD;void*_TBE;int*_TBF;int _TC0;void*_TC1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC2;void*_TC3;int*_TC4;unsigned _TC5;void*_TC6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC7;struct Cyc_List_List*_TC8;void*_TC9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TCA;struct Cyc_List_List*_TCB;struct Cyc_List_List*_TCC;void*_TCD;int*_TCE;unsigned _TCF;void*_TD0;struct Cyc_List_List*_TD1;void*_TD2;void*_TD3;struct Cyc_List_List*_TD4;void*_TD5;struct Cyc_List_List*_TD6;void*_TD7;struct Cyc_List_List*_TD8;void*_TD9;int _TDA;void*_TDB;struct Cyc_List_List*_TDC;void*_TDD;void*_TDE;struct Cyc_List_List*_TDF;void*_TE0;struct Cyc_List_List*_TE1;void*_TE2;struct Cyc_List_List*_TE3;void*_TE4;int _TE5;void*_TE6;struct Cyc_List_List*_TE7;void*_TE8;void*_TE9;void*_TEA;struct Cyc_List_List*_TEB;void*_TEC;void*_TED;int _TEE;int _TEF;{struct _tuple15 _TF0;
_TF0.f0=Cyc_Absyn_compress(aq_sub);_TF0.f1=Cyc_Absyn_compress(aq_sup);_T0=_TF0;}{struct _tuple15 _TF0=_T0;struct Cyc_Absyn_Tvar*_TF1;struct Cyc_List_List*_TF2;struct Cyc_Core_Opt*_TF3;struct Cyc_List_List*_TF4;struct Cyc_Core_Opt*_TF5;void*_TF6;void*_TF7;enum Cyc_Absyn_AliasQualVal _TF8;enum Cyc_Absyn_AliasQualVal _TF9;_T1=_TF0.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL45;_T4=_TF0.f0;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 16: _T9=_TF0.f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=0)goto _TL48;_TC=_TF0.f1;_TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=16)goto _TL4A;_T11=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T12=_TFA->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_TFB=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T12;_TF9=_TFB->f1;}}_T13=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13;_T14=_TFA->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_TFB=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T14;_TF8=_TFB->f1;}}{enum Cyc_Absyn_AliasQualVal v_sub=_TF9;enum Cyc_Absyn_AliasQualVal v_sup=_TF8;
# 169
if(mode!=1)goto _TL4C;_T15=v_sub;_T16=(int)_T15;_T17=v_sup;_T18=(int)_T17;_T19=_T16==_T18;
return _T19;_TL4C:
 if(mode!=2)goto _TL4E;_T1B=v_sub;_T1C=(int)_T1B;_T1D=v_sup;_T1E=(int)_T1D;
if(_T1C!=_T1E)goto _TL50;_T1F=v_sub;_T20=(int)_T1F;_T1A=_T20!=3;goto _TL51;_TL50: _T1A=0;_TL51: return _T1A;_TL4E: _T22=v_sub;_T23=(int)_T22;_T24=v_sup;_T25=(int)_T24;
if(_T23==_T25)goto _TL54;else{goto _TL55;}_TL55: _T26=v_sup;_T27=(int)_T26;if(_T27==3)goto _TL54;else{goto _TL52;}_TL54: _T21=1;goto _TL53;_TL52: _T29=v_sup;_T2A=(int)_T29;
if(_T2A!=0)goto _TL56;_T2C=v_sub;_T2D=(int)_T2C;if(_T2D==1)goto _TL5A;else{goto _TL5B;}_TL5B: _T2E=v_sub;_T2F=(int)_T2E;if(_T2F==2)goto _TL5A;else{goto _TL58;}_TL5A: _T2B=1;goto _TL59;_TL58: _T30=v_sub;_T31=(int)_T30;_T2B=_T31==3;_TL59: _T28=_T2B;goto _TL57;_TL56: _T28=0;_TL57: _T21=_T28;_TL53:
# 173
 return _T21;}_TL4A: goto _LL11;_TL48: goto _LL11;case 17: _T32=_TF0.f0;_T33=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T32;_T34=_T33->f2;if(_T34==0)goto _TL5C;_T35=_TF0.f0;_T36=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T35;_T37=_T36->f2;_T38=(struct Cyc_List_List*)_T37;_T39=_T38->hd;_T3A=(int*)_T39;_T3B=*_T3A;switch(_T3B){case 2: _T3C=_TF0.f1;_T3D=(int*)_T3C;_T3E=*_T3D;if(_T3E!=0)goto _TL5F;_T3F=_TF0.f1;_T40=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3F;_T41=_T40->f1;_T42=(int*)_T41;_T43=*_T42;switch(_T43){case 17: _T44=_TF0.f1;_T45=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T44;_T46=_T45->f2;if(_T46==0)goto _TL62;_T47=_TF0.f1;_T48=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T47;_T49=_T48->f2;_T4A=(struct Cyc_List_List*)_T49;_T4B=_T4A->hd;_T4C=(int*)_T4B;_T4D=*_T4C;switch(_T4D){case 2: _T4E=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4E;_T4F=_TFA->f2;{struct Cyc_List_List _TFB=*_T4F;_T50=_TFB.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T50;_TF7=_TFC->f1;}}}_T51=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T51;_T52=_TFA->f2;{struct Cyc_List_List _TFB=*_T52;_T53=_TFB.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T53;_TF6=_TFC->f1;}}}{struct Cyc_Absyn_Tvar*tv1=_TF7;struct Cyc_Absyn_Tvar*tv2=_TF6;_T54=tv2;_T55=_T54->identity;_T56=tv1;_T57=_T56->identity;_T58=_T55==_T57;
# 179
return _T58;}case 1: _T59=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T59;_T5A=_TFA->f2;{struct Cyc_List_List _TFB=*_T5A;_T5B=_TFB.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T5B;_TF7=_TFC->f1;}_TF6=_TFB.tl;}}_T5C=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5C;_T5D=_TFA->f2;{struct Cyc_List_List _TFB=*_T5D;_T5E=_TFB.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T5E;_TF5=_TFC->f1;}_TF4=_TFB.tl;}}{struct Cyc_Absyn_Tvar*tv1=_TF7;struct Cyc_List_List*bnd1=_TF6;struct Cyc_Core_Opt*k=_TF5;struct Cyc_List_List*bnd2=_TF4;_T5F=
# 196
_check_null(bnd1);_T60=_T5F->hd;_T61=_check_null(bnd2);_T62=_T61->hd;_T63=Cyc_Subtype_alias_qual_subtype(_T60,_T62,2);return _T63;}default: goto _LL11;}goto _TL63;_TL62: goto _LL11;_TL63:;case 16: _T64=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T64;_T65=_TFA->f2;{struct Cyc_List_List _TFB=*_T65;_T66=_TFB.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T66;_TF7=_TFC->f1;}_TF6=_TFB.tl;}}_T67=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T67;_T68=_TFA->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_TFB=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_T68;_TF9=_TFB->f1;}}{struct Cyc_Absyn_Tvar*tv=_TF7;struct Cyc_List_List*bnd=_TF6;enum Cyc_Absyn_AliasQualVal v_sup=_TF9;_T69=
# 189
_check_null(bnd);_T6A=_T69->hd;_T6B=aq_sup;_T6C=mode;_T6D=Cyc_Subtype_alias_qual_subtype(_T6A,_T6B,_T6C);return _T6D;}default: goto _LL11;}goto _TL60;_TL5F: goto _LL11;_TL60:;case 0: _T6E=_TF0.f0;_T6F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T70=_T6F->f2;_T71=(struct Cyc_List_List*)_T70;_T72=_T71->hd;_T73=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T72;_T74=_T73->f1;_T75=(int*)_T74;_T76=*_T75;if(_T76!=15)goto _TL65;_T77=_TF0.f0;_T78=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T77;_T79=_T78->f2;_T7A=(struct Cyc_List_List*)_T79;_T7B=_T7A->hd;_T7C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7B;_T7D=_T7C->f2;if(_T7D==0)goto _TL67;_T7E=_TF0.f0;_T7F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7E;_T80=_T7F->f2;_T81=(struct Cyc_List_List*)_T80;_T82=_T81->hd;_T83=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T84=_T83->f2;_T85=(struct Cyc_List_List*)_T84;_T86=_T85->tl;if(_T86!=0)goto _TL69;_T87=_TF0.f1;_T88=(int*)_T87;_T89=*_T88;if(_T89!=0)goto _TL6B;_T8A=_TF0.f1;_T8B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8A;_T8C=_T8B->f1;_T8D=(int*)_T8C;_T8E=*_T8D;if(_T8E!=17)goto _TL6D;_T8F=_TF0.f1;_T90=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8F;_T91=_T90->f2;if(_T91==0)goto _TL6F;_T92=_TF0.f1;_T93=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T92;_T94=_T93->f2;_T95=(struct Cyc_List_List*)_T94;_T96=_T95->hd;_T97=(int*)_T96;_T98=*_T97;if(_T98!=0)goto _TL71;_T99=_TF0.f1;_T9A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T99;_T9B=_T9A->f2;_T9C=(struct Cyc_List_List*)_T9B;_T9D=_T9C->hd;_T9E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9D;_T9F=_T9E->f1;_TA0=(int*)_T9F;_TA1=*_TA0;if(_TA1!=15)goto _TL73;_TA2=_TF0.f1;_TA3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA2;_TA4=_TA3->f2;_TA5=(struct Cyc_List_List*)_TA4;_TA6=_TA5->hd;_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA6;_TA8=_TA7->f2;if(_TA8==0)goto _TL75;_TA9=_TF0.f1;_TAA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA9;_TAB=_TAA->f2;_TAC=(struct Cyc_List_List*)_TAB;_TAD=_TAC->hd;_TAE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAD;_TAF=_TAE->f2;_TB0=(struct Cyc_List_List*)_TAF;_TB1=_TB0->tl;if(_TB1!=0)goto _TL77;_TB2=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB2;_TB3=_TFA->f2;{struct Cyc_List_List _TFB=*_TB3;_TB4=_TFB.hd;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB4;_TB5=_TFC->f2;{struct Cyc_List_List _TFD=*_TB5;_TB6=_TFD.hd;_TF7=(void*)_TB6;}}}}_TB7=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB7;_TB8=_TFA->f2;{struct Cyc_List_List _TFB=*_TB8;_TB9=_TFB.hd;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB9;_TBA=_TFC->f2;{struct Cyc_List_List _TFD=*_TBA;_TBB=_TFD.hd;_TF6=(void*)_TBB;}}}}{void*tv1=_TF7;void*tv2=_TF6;_TBC=
# 183
Cyc_Tcutil_typecmp(tv1,tv2);_TBD=_TBC==0;return _TBD;}_TL77: goto _LL11;_TL75: goto _LL11;_TL73: goto _LL11;_TL71: goto _LL11;_TL6F: goto _LL11;_TL6D: goto _LL11;_TL6B: goto _LL11;_TL69: goto _LL11;_TL67: goto _LL11;_TL65: goto _LL11;case 1: _TBE=_TF0.f1;_TBF=(int*)_TBE;_TC0=*_TBF;if(_TC0!=0)goto _TL79;_TC1=_TF0.f1;_TC2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC1;_TC3=_TC2->f1;_TC4=(int*)_TC3;_TC5=*_TC4;switch(_TC5){case 17: _TC6=_TF0.f1;_TC7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC6;_TC8=_TC7->f2;if(_TC8==0)goto _TL7C;_TC9=_TF0.f1;_TCA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC9;_TCB=_TCA->f2;_TCC=(struct Cyc_List_List*)_TCB;_TCD=_TCC->hd;_TCE=(int*)_TCD;_TCF=*_TCE;switch(_TCF){case 1: _TD0=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD0;_TD1=_TFA->f2;{struct Cyc_List_List _TFB=*_TD1;_TD2=_TFB.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TD2;_TF5=_TFC->f1;}_TF4=_TFB.tl;}}_TD3=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD3;_TD4=_TFA->f2;{struct Cyc_List_List _TFB=*_TD4;_TD5=_TFB.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TD5;_TF3=_TFC->f1;}_TF2=_TFB.tl;}}{struct Cyc_Core_Opt*k1=_TF5;struct Cyc_List_List*bnd1=_TF4;struct Cyc_Core_Opt*k2=_TF3;struct Cyc_List_List*bnd2=_TF2;_TD6=
# 186
_check_null(bnd1);_TD7=_TD6->hd;_TD8=_check_null(bnd2);_TD9=_TD8->hd;_TDA=Cyc_Subtype_alias_qual_subtype(_TD7,_TD9,1);return _TDA;}case 2: _TDB=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDB;_TDC=_TFA->f2;{struct Cyc_List_List _TFB=*_TDC;_TDD=_TFB.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TDD;_TF5=_TFC->f1;}_TF4=_TFB.tl;}}_TDE=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDE;_TDF=_TFA->f2;{struct Cyc_List_List _TFB=*_TDF;_TE0=_TFB.hd;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TE0;_TF1=_TFC->f1;}_TF2=_TFB.tl;}}{struct Cyc_Core_Opt*k1=_TF5;struct Cyc_List_List*bnd1=_TF4;struct Cyc_Absyn_Tvar*tv2=_TF1;struct Cyc_List_List*bnd2=_TF2;_TE1=
# 200
_check_null(bnd1);_TE2=_TE1->hd;_TE3=_check_null(bnd2);_TE4=_TE3->hd;_TE5=Cyc_Subtype_alias_qual_subtype(_TE2,_TE4,2);return _TE5;}default: goto _LL11;}goto _TL7D;_TL7C: goto _LL11;_TL7D:;case 16: _TE6=_TF0.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE6;_TE7=_TFA->f2;{struct Cyc_List_List _TFB=*_TE7;_TE8=_TFB.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TFC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TE8;_TF5=_TFC->f1;}_TF4=_TFB.tl;}}_TE9=_TF0.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE9;_TEA=_TFA->f1;{struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*_TFB=(struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)_TEA;_TF9=_TFB->f1;}}{struct Cyc_Core_Opt*k=_TF5;struct Cyc_List_List*bnd=_TF4;enum Cyc_Absyn_AliasQualVal v_sup=_TF9;_TEB=
# 192
_check_null(bnd);_TEC=_TEB->hd;_TED=aq_sup;_TEE=mode;_TEF=Cyc_Subtype_alias_qual_subtype(_TEC,_TED,_TEE);return _TEF;}default: goto _LL11;}goto _TL7A;_TL79: goto _LL11;_TL7A:;default: goto _LL11;}goto _TL5D;_TL5C: goto _LL11;_TL5D:;default: goto _LL11;}goto _TL46;_TL45: _LL11:
# 205
 return 0;_TL46:;}}
# 209
static int Cyc_Subtype_isomorphic(void*t1,void*t2){struct _tuple15 _T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;void*_T9;int*_TA;int _TB;void*_TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;void*_T11;void*_T12;void*_T13;void*_T14;int _T15;enum Cyc_Absyn_Size_of _T16;int _T17;enum Cyc_Absyn_Size_of _T18;int _T19;enum Cyc_Absyn_Size_of _T1A;int _T1B;enum Cyc_Absyn_Size_of _T1C;int _T1D;int _T1E;enum Cyc_Absyn_Size_of _T1F;int _T20;enum Cyc_Absyn_Size_of _T21;int _T22;{struct _tuple15 _T23;
_T23.f0=Cyc_Absyn_compress(t1);_T23.f1=Cyc_Absyn_compress(t2);_T0=_T23;}{struct _tuple15 _T23=_T0;enum Cyc_Absyn_Size_of _T24;enum Cyc_Absyn_Size_of _T25;_T1=_T23.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL7F;_T4=_T23.f0;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=1)goto _TL81;_T9=_T23.f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=0)goto _TL83;_TC=_T23.f1;_TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=1)goto _TL85;_T11=_T23.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T12=_T26->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T27=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T12;_T25=_T27->f2;}}_T13=_T23.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13;_T14=_T26->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T27=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T14;_T24=_T27->f2;}}{enum Cyc_Absyn_Size_of b1=_T25;enum Cyc_Absyn_Size_of b2=_T24;_T16=b1;_T17=(int)_T16;_T18=b2;_T19=(int)_T18;
# 212
if(_T17==_T19)goto _TL89;else{goto _TL8A;}_TL8A: _T1A=b1;_T1B=(int)_T1A;if(_T1B==2)goto _TL8B;else{goto _TL87;}_TL8B: _T1C=b2;_T1D=(int)_T1C;if(_T1D==3)goto _TL89;else{goto _TL87;}_TL89: _T15=1;goto _TL88;_TL87: _T1F=b1;_T20=(int)_T1F;
if(_T20!=3)goto _TL8C;_T21=b2;_T22=(int)_T21;_T1E=_T22==2;goto _TL8D;_TL8C: _T1E=0;_TL8D: _T15=_T1E;_TL88:
# 212
 return _T15;}_TL85: goto _LL3;_TL83: goto _LL3;_TL81: goto _LL3;_TL7F: _LL3:
# 214
 return 0;;}}struct _tuple16{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 222
static struct _tuple12 Cyc_Subtype_subtype_impl(struct Cyc_List_List*assume,void*t1,void*t2){int _T0;struct _tuple12 _T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple15*_T5;struct _tuple15 _T6;void*_T7;int _T8;void*_T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple15*_TC;struct _tuple15 _TD;void*_TE;int _TF;struct _tuple12 _T10;struct Cyc_List_List*_T11;struct _tuple15 _T12;void*_T13;int*_T14;unsigned _T15;void*_T16;int*_T17;int _T18;void*_T19;struct Cyc_Absyn_PtrInfo _T1A;struct Cyc_Absyn_PtrInfo _T1B;struct Cyc_Absyn_PtrInfo _T1C;struct Cyc_Absyn_PtrAtts _T1D;struct Cyc_Absyn_PtrInfo _T1E;struct Cyc_Absyn_PtrAtts _T1F;struct Cyc_Absyn_PtrInfo _T20;struct Cyc_Absyn_PtrAtts _T21;struct Cyc_Absyn_PtrInfo _T22;struct Cyc_Absyn_PtrAtts _T23;struct Cyc_Absyn_PtrInfo _T24;struct Cyc_Absyn_PtrAtts _T25;struct Cyc_Absyn_PtrInfo _T26;struct Cyc_Absyn_PtrAtts _T27;void*_T28;struct Cyc_Absyn_PtrInfo _T29;struct Cyc_Absyn_PtrInfo _T2A;struct Cyc_Absyn_PtrInfo _T2B;struct Cyc_Absyn_PtrAtts _T2C;struct Cyc_Absyn_PtrInfo _T2D;struct Cyc_Absyn_PtrAtts _T2E;struct Cyc_Absyn_PtrInfo _T2F;struct Cyc_Absyn_PtrAtts _T30;struct Cyc_Absyn_PtrInfo _T31;struct Cyc_Absyn_PtrAtts _T32;struct Cyc_Absyn_PtrInfo _T33;struct Cyc_Absyn_PtrAtts _T34;struct Cyc_Absyn_PtrInfo _T35;struct Cyc_Absyn_PtrAtts _T36;struct Cyc_Absyn_Tqual _T37;int _T38;struct Cyc_Absyn_Tqual _T39;int _T3A;struct _tuple12 _T3B;int _T3C;int _T3D;int _T3E;struct _tuple12 _T3F;int _T40;int _T41;int _T42;struct _tuple12 _T43;int _T44;int _T45;int _T46;struct _tuple12 _T47;int _T48;struct _tuple12 _T49;enum Cyc_Absyn_Coercion _T4A;int _T4B;int _T4C;struct _tuple12 _T4D;int _T4E;int _T4F;struct _tuple12 _T50;enum Cyc_Absyn_Coercion _T51;int _T52;struct _tuple12 _T53;int _T54;int _T55;int _T56;void*_T57;void*_T58;void*_T59;void*_T5A;int _T5B;struct _tuple12 _T5C;void*_T5D;unsigned _T5E;void*_T5F;unsigned _T60;struct Cyc_List_List*_T61;struct _tuple12 _T62;struct Cyc_Absyn_Tqual _T63;int _T64;struct Cyc_Absyn_Tqual _T65;int _T66;struct Cyc_Absyn_Kind*_T67;struct Cyc_Absyn_Kind*_T68;struct Cyc_Absyn_Kind*_T69;int _T6A;struct _tuple12 _T6B;int _T6C;struct _tuple12 _T6D;enum Cyc_Absyn_Coercion _T6E;int _T6F;struct _tuple12 _T70;int _T71;int _T72;void*_T73;void*_T74;int _T75;void*_T76;unsigned _T77;struct Cyc_List_List*_T78;void*_T79;void*_T7A;int _T7B;struct Cyc_List_List*_T7C;struct _tuple15*_T7D;void*_T7E;void*_T7F;enum Cyc_Absyn_Coercion _T80;int _T81;struct _tuple12 _T82;struct _tuple12 _T83;void*_T84;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T85;void*_T86;int*_T87;int _T88;void*_T89;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8A;void*_T8B;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T8C;union Cyc_Absyn_DatatypeFieldInfo _T8D;struct _union_DatatypeFieldInfo_KnownDatatypefield _T8E;unsigned _T8F;void*_T90;int*_T91;int _T92;void*_T93;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T94;void*_T95;int*_T96;int _T97;void*_T98;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T99;void*_T9A;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T9B;union Cyc_Absyn_DatatypeInfo _T9C;struct _union_DatatypeInfo_KnownDatatype _T9D;unsigned _T9E;void*_T9F;void*_TA0;union Cyc_Absyn_DatatypeFieldInfo _TA1;struct _union_DatatypeFieldInfo_KnownDatatypefield _TA2;struct _tuple1 _TA3;union Cyc_Absyn_DatatypeFieldInfo _TA4;struct _union_DatatypeFieldInfo_KnownDatatypefield _TA5;struct _tuple1 _TA6;void*_TA7;void*_TA8;union Cyc_Absyn_DatatypeInfo _TA9;struct _union_DatatypeInfo_KnownDatatype _TAA;struct Cyc_Absyn_Datatypedecl**_TAB;struct Cyc_Absyn_Datatypedecl*_TAC;struct _tuple0*_TAD;struct Cyc_Absyn_Datatypedecl*_TAE;struct _tuple0*_TAF;int _TB0;struct _tuple12 _TB1;int _TB2;int _TB3;struct _tuple12 _TB4;struct Cyc_List_List*_TB5;void*_TB6;struct Cyc_List_List*_TB7;void*_TB8;enum Cyc_Absyn_Coercion _TB9;int _TBA;struct Cyc_List_List*_TBB;struct Cyc_List_List*_TBC;struct _tuple12 _TBD;void*_TBE;int*_TBF;int _TC0;void*_TC1;void*_TC2;struct Cyc_Absyn_Vardecl*_TC3;void*_TC4;struct Cyc_Absyn_Vardecl*_TC5;void*_TC6;int _TC7;struct _tuple12 _TC8;struct Cyc_List_List*_TC9;struct Cyc_Absyn_Vardecl*_TCA;struct Cyc_Absyn_Vardecl*_TCB;void*_TCC;struct Cyc_List_List*_TCD;struct Cyc_AssnDef_ExistAssnFn*_TCE;struct Cyc_List_List*_TCF;struct Cyc_AssnDef_ExistAssnFn*_TD0;struct Cyc_List_List*_TD1;int _TD2;int _TD3;struct _tuple12 _TD4;struct _tuple12 _TD5;void*_TD6;struct Cyc_List_List*_TD7;struct Cyc_Absyn_Vardecl*_TD8;void*_TD9;void*_TDA;struct _tuple12 _TDB;void*_TDC;int*_TDD;int _TDE;void*_TDF;void*_TE0;struct Cyc_Absyn_FnInfo _TE1;struct Cyc_List_List*_TE2;struct Cyc_Absyn_FnInfo _TE3;struct Cyc_List_List*_TE4;struct Cyc_Absyn_FnInfo _TE5;struct Cyc_Absyn_FnInfo _TE6;int _TE7;int _TE8;struct _tuple12 _TE9;struct Cyc_List_List*_TEA;void*_TEB;struct Cyc_Absyn_Tvar*_TEC;void*_TED;struct Cyc_List_List*_TEE;void*_TEF;struct Cyc_Absyn_Tvar*_TF0;void*_TF1;int _TF2;struct _tuple12 _TF3;struct Cyc_List_List*_TF4;struct _tuple16*_TF5;struct Cyc_List_List*_TF6;void*_TF7;struct Cyc_List_List*_TF8;void*_TF9;struct Cyc_Absyn_Tvar*_TFA;struct Cyc_List_List*_TFB;struct Cyc_List_List*_TFC;struct _tuple12 _TFD;struct _RegionHandle*_TFE;struct Cyc_List_List*_TFF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T100;void*_T101;struct Cyc_List_List*_T102;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T103;void*_T104;void*_T105;struct _tuple12 _T106;struct Cyc_List_List*_T107;struct Cyc_Absyn_FnInfo _T108;void*_T109;struct Cyc_Absyn_FnInfo _T10A;void*_T10B;enum Cyc_Absyn_Coercion _T10C;int _T10D;struct _tuple12 _T10E;struct Cyc_Absyn_FnInfo _T10F;struct Cyc_Absyn_FnInfo _T110;int _T111;int _T112;struct _tuple12 _T113;struct Cyc_List_List*_T114;void*_T115;struct _tuple8*_T116;struct Cyc_List_List*_T117;void*_T118;struct _tuple8*_T119;struct Cyc_Absyn_Tqual _T11A;int _T11B;struct Cyc_Absyn_Tqual _T11C;int _T11D;struct _tuple12 _T11E;enum Cyc_Absyn_Coercion _T11F;int _T120;struct _tuple12 _T121;struct Cyc_List_List*_T122;struct Cyc_List_List*_T123;struct Cyc_Absyn_FnInfo _T124;int _T125;struct Cyc_Absyn_FnInfo _T126;int _T127;struct _tuple12 _T128;struct Cyc_Absyn_FnInfo _T129;struct Cyc_Absyn_VarargInfo*_T12A;struct Cyc_Absyn_FnInfo _T12B;struct Cyc_Absyn_VarargInfo*_T12C;struct Cyc_Absyn_FnInfo _T12D;struct Cyc_Absyn_VarargInfo*_T12E;struct Cyc_Absyn_FnInfo _T12F;struct Cyc_Absyn_VarargInfo*_T130;struct Cyc_Absyn_VarargInfo _T131;struct Cyc_Absyn_Tqual _T132;int _T133;struct Cyc_Absyn_VarargInfo _T134;struct Cyc_Absyn_Tqual _T135;int _T136;struct _tuple12 _T137;struct Cyc_List_List*_T138;struct Cyc_Absyn_VarargInfo _T139;void*_T13A;struct Cyc_Absyn_VarargInfo _T13B;void*_T13C;enum Cyc_Absyn_Coercion _T13D;int _T13E;struct _tuple12 _T13F;struct Cyc_Absyn_FnInfo _T140;struct Cyc_Absyn_VarargInfo*_T141;struct Cyc_Absyn_FnInfo _T142;struct Cyc_Absyn_VarargInfo*_T143;struct _tuple12 _T144;struct Cyc_Absyn_FnInfo _T145;void*_T146;void*_T147;struct Cyc_Absyn_FnInfo _T148;void*_T149;void*_T14A;int _T14B;struct _tuple12 _T14C;struct Cyc_Absyn_FnInfo _T14D;struct Cyc_List_List*_T14E;struct Cyc_Absyn_FnInfo _T14F;struct Cyc_List_List*_T150;int _T151;struct _tuple12 _T152;struct Cyc_Absyn_FnInfo _T153;struct Cyc_List_List*_T154;struct Cyc_Absyn_FnInfo _T155;struct Cyc_List_List*_T156;int _T157;struct _tuple12 _T158;struct Cyc_Absyn_FnInfo _T159;struct Cyc_List_List*_T15A;struct Cyc_List_List*_T15B;void*_T15C;struct Cyc_Absyn_Vardecl*_T15D;void*_T15E;struct Cyc_List_List*_T15F;void*_T160;struct Cyc_Absyn_Vardecl*_T161;struct Cyc_List_List*_T162;void*_T163;struct Cyc_Absyn_Vardecl*_T164;struct Cyc_List_List*_T165;struct Cyc_List_List*_T166;struct Cyc_Absyn_FnInfo _T167;struct Cyc_AssnDef_ExistAssnFn*_T168;struct Cyc_Absyn_FnInfo _T169;struct Cyc_AssnDef_ExistAssnFn*_T16A;struct Cyc_List_List*_T16B;void*_T16C;struct Cyc_Absyn_FnInfo _T16D;struct Cyc_AssnDef_ExistAssnFn*_T16E;unsigned _T16F;struct Cyc_Absyn_FnInfo _T170;struct Cyc_AssnDef_ExistAssnFn*_T171;struct Cyc_List_List*_T172;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T173;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T174;int _T175;int _T176;struct _tuple12 _T177;struct Cyc_Absyn_FnInfo _T178;struct Cyc_AssnDef_ExistAssnFn*_T179;struct Cyc_Absyn_FnInfo _T17A;struct Cyc_AssnDef_ExistAssnFn*_T17B;struct Cyc_List_List*_T17C;void*_T17D;struct Cyc_Absyn_FnInfo _T17E;struct Cyc_AssnDef_ExistAssnFn*_T17F;unsigned _T180;struct Cyc_Absyn_FnInfo _T181;struct Cyc_AssnDef_ExistAssnFn*_T182;struct Cyc_List_List*_T183;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T184;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T185;int _T186;int _T187;struct _tuple12 _T188;struct Cyc_Absyn_FnInfo _T189;struct Cyc_AssnDef_ExistAssnFn*_T18A;struct Cyc_Absyn_FnInfo _T18B;struct Cyc_Absyn_Vardecl*_T18C;struct Cyc_Absyn_FnInfo _T18D;struct Cyc_Absyn_Vardecl*_T18E;struct Cyc_Absyn_Vardecl*_T18F;void*_T190;struct Cyc_Absyn_FnInfo _T191;struct Cyc_AssnDef_ExistAssnFn*_T192;struct Cyc_List_List*_T193;void*_T194;struct Cyc_Absyn_FnInfo _T195;struct Cyc_AssnDef_ExistAssnFn*_T196;unsigned _T197;struct Cyc_Absyn_FnInfo _T198;struct Cyc_AssnDef_ExistAssnFn*_T199;struct Cyc_List_List*_T19A;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T19B;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T19C;int _T19D;int _T19E;struct _tuple12 _T19F;struct Cyc_Absyn_FnInfo _T1A0;struct Cyc_AssnDef_ExistAssnFn*_T1A1;struct Cyc_Absyn_FnInfo _T1A2;struct Cyc_AssnDef_ExistAssnFn*_T1A3;struct Cyc_List_List*_T1A4;void*_T1A5;struct Cyc_Absyn_FnInfo _T1A6;struct Cyc_AssnDef_ExistAssnFn*_T1A7;unsigned _T1A8;struct Cyc_Absyn_FnInfo _T1A9;struct Cyc_AssnDef_ExistAssnFn*_T1AA;struct Cyc_List_List*_T1AB;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1AC;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1AD;int _T1AE;int _T1AF;struct _tuple12 _T1B0;struct _tuple12 _T1B1;struct _tuple12 _T1B2;
# 224
struct Cyc_List_List*retc=0;_T0=
Cyc_Unify_unify(t1,t2);if(!_T0)goto _TL8E;{struct _tuple12 _T1B3;_T1B3.f0=1U;_T1B3.f1=0;_T1=_T1B3;}return _T1;_TL8E:{
struct Cyc_List_List*a=assume;_TL93: if(a!=0)goto _TL91;else{goto _TL92;}
_TL91: _T2=t1;_T3=a;_T4=_T3->hd;_T5=(struct _tuple15*)_T4;_T6=*_T5;_T7=_T6.f0;_T8=Cyc_Unify_unify(_T2,_T7);if(!_T8)goto _TL94;_T9=t2;_TA=a;_TB=_TA->hd;_TC=(struct _tuple15*)_TB;_TD=*_TC;_TE=_TD.f1;_TF=Cyc_Unify_unify(_T9,_TE);if(!_TF)goto _TL94;{struct _tuple12 _T1B3;
_T1B3.f0=1U;_T1B3.f1=0;_T10=_T1B3;}return _T10;_TL94: _T11=a;
# 226
a=_T11->tl;goto _TL93;_TL92:;}
# 229
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{struct _tuple15 _T1B3;
_T1B3.f0=t1;_T1B3.f1=t2;_T12=_T1B3;}{struct _tuple15 _T1B3=_T12;struct Cyc_Absyn_FnInfo _T1B4;struct Cyc_Absyn_FnInfo _T1B5;struct Cyc_AssnDef_ExistAssnFn*_T1B6;struct Cyc_AssnDef_ExistAssnFn*_T1B7;struct Cyc_Absyn_Vardecl*_T1B8;struct Cyc_List_List*_T1B9;struct Cyc_Absyn_Datatypedecl*_T1BA;struct Cyc_List_List*_T1BB;struct Cyc_Absyn_Datatypefield*_T1BC;void*_T1BD;void*_T1BE;void*_T1BF;void*_T1C0;void*_T1C1;void*_T1C2;struct Cyc_Absyn_Tqual _T1C3;void*_T1C4;void*_T1C5;void*_T1C6;void*_T1C7;void*_T1C8;void*_T1C9;void*_T1CA;struct Cyc_Absyn_Tqual _T1CB;void*_T1CC;_T13=_T1B3.f0;_T14=(int*)_T13;_T15=*_T14;switch(_T15){case 4: _T16=_T1B3.f1;_T17=(int*)_T16;_T18=*_T17;if(_T18!=4)goto _TL97;_T19=_T1B3.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T19;_T1A=_T1CD->f1;_T1CC=_T1A.elt_type;_T1B=_T1CD->f1;_T1CB=_T1B.elt_tq;_T1C=_T1CD->f1;_T1D=_T1C.ptr_atts;_T1CA=_T1D.eff;_T1E=_T1CD->f1;_T1F=_T1E.ptr_atts;_T1C9=_T1F.nullable;_T20=_T1CD->f1;_T21=_T20.ptr_atts;_T1C8=_T21.bounds;_T22=_T1CD->f1;_T23=_T22.ptr_atts;_T1C7=_T23.zero_term;_T24=_T1CD->f1;_T25=_T24.ptr_atts;_T1C6=_T25.autoreleased;_T26=_T1CD->f1;_T27=_T26.ptr_atts;_T1C5=_T27.aqual;}_T28=_T1B3.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T28;_T29=_T1CD->f1;_T1C4=_T29.elt_type;_T2A=_T1CD->f1;_T1C3=_T2A.elt_tq;_T2B=_T1CD->f1;_T2C=_T2B.ptr_atts;_T1C2=_T2C.eff;_T2D=_T1CD->f1;_T2E=_T2D.ptr_atts;_T1C1=_T2E.nullable;_T2F=_T1CD->f1;_T30=_T2F.ptr_atts;_T1C0=_T30.bounds;_T31=_T1CD->f1;_T32=_T31.ptr_atts;_T1BF=_T32.zero_term;_T33=_T1CD->f1;_T34=_T33.ptr_atts;_T1BE=_T34.autoreleased;_T35=_T1CD->f1;_T36=_T35.ptr_atts;_T1BD=_T36.aqual;}{void*t_a=_T1CC;struct Cyc_Absyn_Tqual q_a=_T1CB;void*eff_a=_T1CA;void*null_a=_T1C9;void*b_a=_T1C8;void*zt_a=_T1C7;void*rel_a=_T1C6;void*aq_a=_T1C5;void*t_b=_T1C4;struct Cyc_Absyn_Tqual q_b=_T1C3;void*eff_b=_T1C2;void*null_b=_T1C1;void*b_b=_T1C0;void*zt_b=_T1BF;void*rel_b=_T1BE;void*aq_b=_T1BD;
# 236
enum Cyc_Absyn_Coercion coerce=1U;_T37=q_a;_T38=_T37.real_const;
# 238
if(!_T38)goto _TL99;_T39=q_b;_T3A=_T39.real_const;if(_T3A)goto _TL99;else{goto _TL9B;}
_TL9B:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T3B=_T1CD;}return _T3B;_TL99: _T3C=
# 241
Cyc_Unify_unify(null_a,null_b);if(_T3C)goto _TL9C;else{goto _TL9E;}
_TL9E: _T3D=Cyc_Absyn_type2bool(0,null_a);if(!_T3D)goto _TL9F;_T3E=Cyc_Absyn_type2bool(0,null_b);if(_T3E)goto _TL9F;else{goto _TLA1;}
_TLA1:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T3F=_T1CD;}return _T3F;_TL9F: goto _TL9D;_TL9C: _TL9D: _T40=
# 247
Cyc_Unify_unify(zt_a,zt_b);if(_T40)goto _TLA2;else{goto _TLA4;}_TLA4: _T41=
Cyc_Absyn_type2bool(0,zt_a);
# 247
if(_T41)goto _TLA2;else{goto _TLA5;}_TLA5: _T42=
Cyc_Absyn_type2bool(0,zt_b);
# 247
if(!_T42)goto _TLA2;{struct _tuple12 _T1CD;
# 249
_T1CD.f0=0U;_T1CD.f1=0;_T43=_T1CD;}return _T43;_TLA2: _T44=
# 251
Cyc_Unify_unify(rel_a,rel_b);if(!_T44)goto _TLA6;_T45=
Cyc_Absyn_type2bool(0,rel_a);
# 251
if(_T45)goto _TLA6;else{goto _TLA8;}_TLA8: _T46=
Cyc_Absyn_type2bool(0,rel_b);
# 251
if(!_T46)goto _TLA6;{struct _tuple12 _T1CD;
# 253
_T1CD.f0=0U;_T1CD.f1=0;_T47=_T1CD;}return _T47;_TLA6: _T48=
# 255
Cyc_Unify_unify(eff_a,eff_b);if(_T48)goto _TLA9;else{goto _TLAB;}_TLAB: _T49=Cyc_Subtype_subtype_impl(assume,eff_a,eff_b);_T4A=_T49.f0;_T4B=(int)_T4A;if(_T4B)goto _TLA9;else{goto _TLAC;}_TLAC: _T4C=
Cyc_Tcutil_subset_effect(0,eff_a,eff_b);
# 255
if(_T4C)goto _TLA9;else{goto _TLAD;}
# 257
_TLAD:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T4D=_T1CD;}return _T4D;_TLA9: _T4E=
# 259
Cyc_Unify_unify(aq_a,aq_b);if(_T4E)goto _TLAE;else{goto _TLB0;}_TLB0: _T4F=Cyc_Subtype_alias_qual_subtype(aq_a,aq_b,3);if(_T4F)goto _TLAE;else{goto _TLB1;}_TLB1: _T50=
Cyc_Subtype_subtype_impl(assume,aq_a,aq_b);_T51=_T50.f0;_T52=(int)_T51;
# 259
if(_T52)goto _TLAE;else{goto _TLB2;}
# 261
_TLB2:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T53=_T1CD;}return _T53;_TLAE: _T54=
# 264
Cyc_Unify_unify(b_a,b_b);if(_T54)goto _TLB3;else{goto _TLB5;}
_TLB5: _T55=Cyc_Tcutil_is_cvar_type(b_a);if(_T55)goto _TLB6;else{goto _TLB8;}_TLB8: _T56=Cyc_Tcutil_is_cvar_type(b_b);if(_T56)goto _TLB6;else{goto _TLB9;}
_TLB9: _T57=Cyc_Absyn_bounds_one();_T58=b_a;{struct Cyc_Absyn_Exp*e1=Cyc_Tcutil_get_bounds_exp(_T57,_T58);_T59=
Cyc_Absyn_bounds_one();_T5A=b_b;{struct Cyc_Absyn_Exp*e2=Cyc_Tcutil_get_bounds_exp(_T59,_T5A);
if(e1==e2)goto _TLBA;
if(e1==0)goto _TLBE;else{goto _TLC0;}_TLC0: if(e2==0)goto _TLBE;else{goto _TLBF;}_TLBF: _T5B=Cyc_Evexp_lte_const_exp(e2,e1);if(_T5B)goto _TLBC;else{goto _TLBE;}
_TLBE:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T5C=_T1CD;}return _T5C;_TLBC: goto _TLBB;_TLBA: _TLBB:;}}goto _TLB7;
# 276
_TLB6:{void*cva=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_a);
void*cvb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);_T5D=cva;_T5E=(unsigned)_T5D;
if(!_T5E)goto _TLC1;_T5F=cvb;_T60=(unsigned)_T5F;if(!_T60)goto _TLC1;{struct Cyc_List_List*_T1CD=_cycalloc(sizeof(struct Cyc_List_List));
_T1CD->hd=Cyc_BansheeIf_equality_constraint(cva,cvb);_T1CD->tl=retc;_T61=(struct Cyc_List_List*)_T1CD;}retc=_T61;goto _TLC2;
# 282
_TLC1:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T62=_T1CD;}return _T62;_TLC2:;}_TLB7: goto _TLB4;_TLB3: _TLB4: _T63=q_b;_T64=_T63.real_const;
# 287
if(_T64)goto _TLC3;else{goto _TLC5;}_TLC5: _T65=q_a;_T66=_T65.real_const;if(!_T66)goto _TLC3;_T67=& Cyc_Kinds_ak;_T68=(struct Cyc_Absyn_Kind*)_T67;_T69=
Cyc_Tcutil_type_kind(t_b);_T6A=Cyc_Kinds_kind_leq(_T68,_T69);if(_T6A)goto _TLC6;else{goto _TLC8;}
_TLC8:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_T6B=_T1CD;}return _T6B;_TLC6: goto _TLC4;_TLC3: _TLC4: _T6C=
# 292
Cyc_Subtype_isomorphic(t_a,t_b);if(!_T6C)goto _TLC9;{struct _tuple12 _T1CD;
_T1CD.f0=coerce;_T1CD.f1=retc;_T6D=_T1CD;}return _T6D;_TLC9:{
# 296
struct _tuple12 _T1CD=Cyc_Subtype_force_equivalence(t_a,t_b);struct Cyc_List_List*_T1CE;enum Cyc_Absyn_Coercion _T1CF;_T1CF=_T1CD.f0;_T1CE=_T1CD.f1;{enum Cyc_Absyn_Coercion s=_T1CF;struct Cyc_List_List*c=_T1CE;_T6E=s;_T6F=(int)_T6E;
if(_T6F!=1)goto _TLCB;
retc=Cyc_List_imp_append(retc,c);{struct _tuple12 _T1D0;
_T1D0.f0=1U;_T1D0.f1=retc;_T70=_T1D0;}return _T70;_TLCB:;}}_T71=
# 303
Cyc_Tcutil_force_type2bool(0,zt_b);{int deep_subtype=!_T71;_T72=deep_subtype;
if(!_T72)goto _TLCD;_T73=b_b;_T74=Cyc_Absyn_bounds_one();_T75=Cyc_Unify_unify(_T73,_T74);if(_T75)goto _TLCD;else{goto _TLCF;}
# 306
_TLCF:{void*cbb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);_T76=cbb;_T77=(unsigned)_T76;
if(!_T77)goto _TLD0;{struct Cyc_List_List*_T1CD=_cycalloc(sizeof(struct Cyc_List_List));_T79=cbb;_T7A=
Cyc_Absyn_thinconst();_T1CD->hd=Cyc_BansheeIf_equality_constraint(_T79,_T7A);_T1CD->tl=retc;_T78=(struct Cyc_List_List*)_T1CD;}retc=_T78;goto _TLD1;
# 311
_TLD0: deep_subtype=0;_TLD1:;}goto _TLCE;_TLCD: _TLCE: _T7B=deep_subtype;
# 313
if(!_T7B)goto _TLD2;{struct Cyc_List_List*_T1CD=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple15*_T1CE=_cycalloc(sizeof(struct _tuple15));
_T1CE->f0=t1;_T1CE->f1=t2;_T7D=(struct _tuple15*)_T1CE;}_T1CD->hd=_T7D;_T1CD->tl=assume;_T7C=(struct Cyc_List_List*)_T1CD;}_T7E=t_a;_T7F=t_b;{struct _tuple12 _T1CD=Cyc_Subtype_ptrsubtype(_T7C,_T7E,_T7F);struct Cyc_List_List*_T1CE;enum Cyc_Absyn_Coercion _T1CF;_T1CF=_T1CD.f0;_T1CE=_T1CD.f1;{enum Cyc_Absyn_Coercion s=_T1CF;struct Cyc_List_List*c=_T1CE;_T80=s;_T81=(int)_T80;
if(_T81!=1)goto _TLD4;{struct _tuple12 _T1D0;
_T1D0.f0=1U;_T1D0.f1=Cyc_List_imp_append(retc,c);_T82=_T1D0;}return _T82;_TLD4:;}}goto _TLD3;_TLD2: _TLD3:{struct _tuple12 _T1CD;
# 319
_T1CD.f0=0U;_T1CD.f1=0;_T83=_T1CD;}return _T83;}}_TL97: goto _LLB;case 0: _T84=_T1B3.f0;_T85=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T84;_T86=_T85->f1;_T87=(int*)_T86;_T88=*_T87;if(_T88!=23)goto _TLD6;_T89=_T1B3.f0;_T8A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T89;_T8B=_T8A->f1;_T8C=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T8B;_T8D=_T8C->f1;_T8E=_T8D.KnownDatatypefield;_T8F=_T8E.tag;if(_T8F!=2)goto _TLD8;_T90=_T1B3.f1;_T91=(int*)_T90;_T92=*_T91;if(_T92!=0)goto _TLDA;_T93=_T1B3.f1;_T94=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T93;_T95=_T94->f1;_T96=(int*)_T95;_T97=*_T96;if(_T97!=22)goto _TLDC;_T98=_T1B3.f1;_T99=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T98;_T9A=_T99->f1;_T9B=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T9A;_T9C=_T9B->f1;_T9D=_T9C.KnownDatatype;_T9E=_T9D.tag;if(_T9E!=2)goto _TLDE;_T9F=_T1B3.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9F;_TA0=_T1CD->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T1CE=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TA0;_TA1=_T1CE->f1;_TA2=_TA1.KnownDatatypefield;_TA3=_TA2.val;_T1CC=_TA3.f0;_TA4=_T1CE->f1;_TA5=_TA4.KnownDatatypefield;_TA6=_TA5.val;_T1BC=_TA6.f1;}_T1BB=_T1CD->f2;}_TA7=_T1B3.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA7;_TA8=_T1CD->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T1CE=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TA8;_TA9=_T1CE->f1;_TAA=_TA9.KnownDatatype;_TAB=_TAA.val;{struct Cyc_Absyn_Datatypedecl*_T1CF=*_TAB;_T1BA=_T1CF;}}_T1B9=_T1CD->f2;}{struct Cyc_Absyn_Datatypedecl*dd1=_T1CC;struct Cyc_Absyn_Datatypefield*df=_T1BC;struct Cyc_List_List*ts1=_T1BB;struct Cyc_Absyn_Datatypedecl*dd2=_T1BA;struct Cyc_List_List*ts2=_T1B9;
# 325
if(dd1==dd2)goto _TLE0;_TAC=dd1;_TAD=_TAC->name;_TAE=dd2;_TAF=_TAE->name;_TB0=Cyc_Absyn_qvar_cmp(_TAD,_TAF);if(_TB0==0)goto _TLE0;{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_TB1=_T1CD;}return _TB1;_TLE0: _TB2=
# 327
Cyc_List_length(ts1);_TB3=Cyc_List_length(ts2);if(_TB2==_TB3)goto _TLE2;{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_TB4=_T1CD;}return _TB4;_TLE2:
 _TLE7: if(ts1!=0)goto _TLE5;else{goto _TLE6;}
_TLE5: _TB5=ts1;_TB6=_TB5->hd;_TB7=_check_null(ts2);_TB8=_TB7->hd;{struct _tuple12 _T1CD=Cyc_Subtype_force_equivalence(_TB6,_TB8);struct Cyc_List_List*_T1CE;enum Cyc_Absyn_Coercion _T1CF;_T1CF=_T1CD.f0;_T1CE=_T1CD.f1;{enum Cyc_Absyn_Coercion s=_T1CF;struct Cyc_List_List*c=_T1CE;_TB9=s;_TBA=(int)_TB9;
if(_TBA==0)goto _TLE8;
retc=Cyc_List_imp_append(retc,c);goto _TLE9;_TLE8: _TLE9:;}}_TBB=ts1;
# 328
ts1=_TBB->tl;_TBC=ts2;ts2=_TBC->tl;goto _TLE7;_TLE6:{struct _tuple12 _T1CD;
# 334
_T1CD.f0=4U;_T1CD.f1=retc;_TBD=_T1CD;}return _TBD;}_TLDE: goto _LLB;_TLDC: goto _LLB;_TLDA: goto _LLB;_TLD8: goto _LLB;_TLD6: goto _LLB;case 12: _TBE=_T1B3.f1;_TBF=(int*)_TBE;_TC0=*_TBF;if(_TC0!=12)goto _TLEA;_TC1=_T1B3.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TC1;_T1B8=_T1CD->f1;_T1B7=_T1CD->f3;}_TC2=_T1B3.f1;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TC2;_T1CC=_T1CD->f1;_T1B6=_T1CD->f3;}{struct Cyc_Absyn_Vardecl*vd1=_T1B8;struct Cyc_AssnDef_ExistAssnFn*a1=_T1B7;struct Cyc_Absyn_Vardecl*vd2=_T1CC;struct Cyc_AssnDef_ExistAssnFn*a2=_T1B6;_TC3=vd1;_TC4=_TC3->type;_TC5=vd2;_TC6=_TC5->type;_TC7=
# 336
Cyc_Unify_unify(_TC4,_TC6);if(_TC7)goto _TLEC;else{goto _TLEE;}_TLEE:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_TC8=_T1CD;}return _TC8;_TLEC: {
struct Cyc_List_List*terms;terms=_cycalloc(sizeof(struct Cyc_List_List));_TC9=terms;_TCA=vd1;_TCB=vd1;_TCC=_TCB->type;_TC9->hd=Cyc_AssnDef_fresh_var(_TCA,_TCC);_TCD=terms;_TCD->tl=0;_TCE=
_check_null(a1);_TCF=terms;{void*assn1=Cyc_AssnDef_existassnfn2assn(_TCE,_TCF);_TD0=
_check_null(a2);_TD1=terms;{void*assn2=Cyc_AssnDef_existassnfn2assn(_TD0,_TD1);_TD2=
Cyc_AssnDef_simple_prove(assn1,assn2);if(_TD2)goto _TLF1;else{goto _TLF2;}_TLF2: _TD3=
Cyc_PrattProver_constraint_prove(assn1,assn2);
# 340
if(_TD3)goto _TLF1;else{goto _TLEF;}
# 342
_TLF1:{struct _tuple12 _T1CD;_T1CD.f0=1U;_T1CD.f1=retc;_TD4=_T1CD;}return _TD4;_TLEF:{struct _tuple12 _T1CD;
_T1CD.f0=0U;_T1CD.f1=0;_TD5=_T1CD;}return _TD5;}}}}_TLEA: _TD6=_T1B3.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TD6;_T1B8=_T1CD->f1;_T1B7=_T1CD->f3;}_T1CC=_T1B3.f1;{struct Cyc_Absyn_Vardecl*vd1=_T1B8;struct Cyc_AssnDef_ExistAssnFn*a1=_T1B7;void*t2=_T1CC;_TD7=assume;_TD8=vd1;_TD9=_TD8->type;_TDA=t2;_TDB=
# 345
Cyc_Subtype_subtype_impl(_TD7,_TD9,_TDA);return _TDB;}case 6: _TDC=_T1B3.f1;_TDD=(int*)_TDC;_TDE=*_TDD;if(_TDE!=6)goto _TLF3;_TDF=_T1B3.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TDF;_T1B5=_T1CD->f1;}_TE0=_T1B3.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1CD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TE0;_T1B4=_T1CD->f1;}{struct Cyc_Absyn_FnInfo f1=_T1B5;struct Cyc_Absyn_FnInfo f2=_T1B4;_TE1=f1;_TE2=_TE1.tvars;
# 347
if(_TE2!=0)goto _TLF7;else{goto _TLF8;}_TLF8: _TE3=f2;_TE4=_TE3.tvars;if(_TE4!=0)goto _TLF7;else{goto _TLF5;}
_TLF7: _TE5=f1;{struct Cyc_List_List*tvs1=_TE5.tvars;_TE6=f2;{
struct Cyc_List_List*tvs2=_TE6.tvars;_TE7=
Cyc_List_length(tvs1);_TE8=Cyc_List_length(tvs2);if(_TE7==_TE8)goto _TLF9;{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_TE9=_T1CD;}return _TE9;_TLF9: {
struct Cyc_List_List*inst=0;
_TLFB: if(tvs1!=0)goto _TLFC;else{goto _TLFD;}
_TLFC: _TEA=tvs1;_TEB=_TEA->hd;_TEC=(struct Cyc_Absyn_Tvar*)_TEB;_TED=_TEC->kind;_TEE=_check_null(tvs2);_TEF=_TEE->hd;_TF0=(struct Cyc_Absyn_Tvar*)_TEF;_TF1=_TF0->kind;_TF2=Cyc_Unify_unify_kindbound(_TED,_TF1);if(_TF2)goto _TLFE;else{goto _TL100;}_TL100:{struct _tuple12 _T1CD;_T1CD.f0=0U;_T1CD.f1=0;_TF3=_T1CD;}return _TF3;_TLFE:{struct Cyc_List_List*_T1CD=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_T1CE=_cycalloc(sizeof(struct _tuple16));_TF6=tvs2;_TF7=_TF6->hd;
_T1CE->f0=(struct Cyc_Absyn_Tvar*)_TF7;_TF8=tvs1;_TF9=_TF8->hd;_TFA=(struct Cyc_Absyn_Tvar*)_TF9;_T1CE->f1=Cyc_Absyn_var_type(_TFA);_TF5=(struct _tuple16*)_T1CE;}_T1CD->hd=_TF5;_T1CD->tl=inst;_TF4=(struct Cyc_List_List*)_T1CD;}inst=_TF4;_TFB=tvs1;
tvs1=_TFB->tl;_TFC=tvs2;
tvs2=_TFC->tl;goto _TLFB;_TLFD:
# 358
 if(tvs2==0)goto _TL101;{struct _tuple12 _T1CD;
_T1CD.f0=0U;_T1CD.f1=0;_TFD=_T1CD;}return _TFD;_TL101:
 if(inst==0)goto _TL103;
f1.tvars=0;
f2.tvars=0;_TFE=Cyc_Core_heap_region;_TFF=inst;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1CD=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T1CD->tag=6;
_T1CD->f1=f2;_T100=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1CD;}_T101=(void*)_T100;{void*newftype2=Cyc_Tcutil_rsubstitute(_TFE,_TFF,_T101);_T102=assume;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1CD=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T1CD->tag=6;
_T1CD->f1=f1;_T103=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1CD;}_T104=(void*)_T103;_T105=newftype2;_T106=Cyc_Subtype_subtype_impl(_T102,_T104,_T105);return _T106;}_TL103:;}}}goto _TLF6;_TLF5: _TLF6: _T107=assume;_T108=f1;_T109=_T108.ret_type;_T10A=f2;_T10B=_T10A.ret_type;{
# 368
struct _tuple12 _T1CD=Cyc_Subtype_subtype_impl(_T107,_T109,_T10B);struct Cyc_List_List*_T1CE;enum Cyc_Absyn_Coercion _T1CF;_T1CF=_T1CD.f0;_T1CE=_T1CD.f1;{enum Cyc_Absyn_Coercion s=_T1CF;struct Cyc_List_List*c=_T1CE;_T10C=s;_T10D=(int)_T10C;
if(_T10D)goto _TL105;else{goto _TL107;}_TL107:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T10E=_T1D0;}return _T10E;_TL105:
 retc=Cyc_List_imp_append(retc,c);_T10F=f1;{
struct Cyc_List_List*args1=_T10F.args;_T110=f2;{
struct Cyc_List_List*args2=_T110.args;_T111=
# 375
Cyc_List_length(args1);_T112=Cyc_List_length(args2);if(_T111==_T112)goto _TL108;{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T113=_T1D0;}return _T113;_TL108:
# 377
 _TL10D: if(args1!=0)goto _TL10B;else{goto _TL10C;}
_TL10B: _T114=args1;_T115=_T114->hd;_T116=(struct _tuple8*)_T115;{struct _tuple8 _T1D0=*_T116;void*_T1D1;struct Cyc_Absyn_Tqual _T1D2;_T1D2=_T1D0.f1;_T1D1=_T1D0.f2;{struct Cyc_Absyn_Tqual tq1=_T1D2;void*t1=_T1D1;_T117=
_check_null(args2);_T118=_T117->hd;_T119=(struct _tuple8*)_T118;{struct _tuple8 _T1D3=*_T119;void*_T1D4;struct Cyc_Absyn_Tqual _T1D5;_T1D5=_T1D3.f1;_T1D4=_T1D3.f2;{struct Cyc_Absyn_Tqual tq2=_T1D5;void*t2=_T1D4;_T11A=tq2;_T11B=_T11A.real_const;
# 381
if(!_T11B)goto _TL10E;_T11C=tq1;_T11D=_T11C.real_const;if(_T11D)goto _TL10E;else{goto _TL110;}
_TL110:{struct _tuple12 _T1D6;_T1D6.f0=0U;_T1D6.f1=0;_T11E=_T1D6;}return _T11E;_TL10E: {
# 385
struct _tuple12 _T1D6=Cyc_Subtype_subtype_impl(0,t2,t1);struct Cyc_List_List*_T1D7;enum Cyc_Absyn_Coercion _T1D8;_T1D8=_T1D6.f0;_T1D7=_T1D6.f1;{enum Cyc_Absyn_Coercion s=_T1D8;struct Cyc_List_List*c=_T1D7;_T11F=s;_T120=(int)_T11F;
if(_T120!=0)goto _TL111;{struct _tuple12 _T1D9;
_T1D9.f0=0U;_T1D9.f1=0;_T121=_T1D9;}return _T121;_TL111:
 retc=Cyc_List_imp_append(retc,c);}}}}}}_T122=args1;
# 377
args1=_T122->tl;_T123=args2;args2=_T123->tl;goto _TL10D;_TL10C: _T124=f1;_T125=_T124.c_varargs;_T126=f2;_T127=_T126.c_varargs;
# 391
if(_T125==_T127)goto _TL113;{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T128=_T1D0;}return _T128;_TL113: _T129=f1;_T12A=_T129.cyc_varargs;
if(_T12A==0)goto _TL115;_T12B=f2;_T12C=_T12B.cyc_varargs;if(_T12C==0)goto _TL115;_T12D=f1;_T12E=_T12D.cyc_varargs;{
struct Cyc_Absyn_VarargInfo v1=*_T12E;_T12F=f2;_T130=_T12F.cyc_varargs;{
struct Cyc_Absyn_VarargInfo v2=*_T130;_T131=v2;_T132=_T131.tq;_T133=_T132.real_const;
# 396
if(!_T133)goto _TL117;_T134=v1;_T135=_T134.tq;_T136=_T135.real_const;if(_T136)goto _TL117;else{goto _TL119;}
_TL119:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T137=_T1D0;}return _T137;_TL117: _T138=assume;_T139=v2;_T13A=_T139.type;_T13B=v1;_T13C=_T13B.type;{
struct _tuple12 _T1D0=Cyc_Subtype_subtype_impl(_T138,_T13A,_T13C);struct Cyc_List_List*_T1D1;enum Cyc_Absyn_Coercion _T1D2;_T1D2=_T1D0.f0;_T1D1=_T1D0.f1;{enum Cyc_Absyn_Coercion s=_T1D2;struct Cyc_List_List*c=_T1D1;_T13D=s;_T13E=(int)_T13D;
if(_T13E!=0)goto _TL11A;{struct _tuple12 _T1D3;
_T1D3.f0=0U;_T1D3.f1=0;_T13F=_T1D3;}return _T13F;_TL11A:
 retc=Cyc_List_imp_append(retc,c);}}}}goto _TL116;
_TL115: _T140=f1;_T141=_T140.cyc_varargs;if(_T141!=0)goto _TL11E;else{goto _TL11F;}_TL11F: _T142=f2;_T143=_T142.cyc_varargs;if(_T143!=0)goto _TL11E;else{goto _TL11C;}_TL11E:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T144=_T1D0;}return _T144;_TL11C: _TL116: _T145=f1;_T146=_T145.effect;_T147=
# 404
_check_null(_T146);_T148=f2;_T149=_T148.effect;_T14A=_check_null(_T149);_T14B=Cyc_Tcutil_subset_effect(0,_T147,_T14A);if(_T14B)goto _TL120;else{goto _TL122;}_TL122:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T14C=_T1D0;}return _T14C;_TL120: _T14D=f1;_T14E=_T14D.effconstr;_T14F=f2;_T150=_T14F.effconstr;_T151=
# 406
Cyc_Tcutil_cmp_effect_constraints(_T14E,_T150);if(!_T151)goto _TL123;{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T152=_T1D0;}return _T152;_TL123: _T153=f1;_T154=_T153.attributes;_T155=f2;_T156=_T155.attributes;_T157=
# 408
Cyc_Atts_sub_attributes(_T154,_T156);if(_T157)goto _TL125;else{goto _TL127;}_TL127:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T158=_T1D0;}return _T158;_TL125: {
# 410
struct Cyc_List_List*terms=0;_T159=f1;{
# 412
struct Cyc_List_List*vds=_T159.arg_vardecls;_TL12B: if(vds!=0)goto _TL129;else{goto _TL12A;}
_TL129:{struct Cyc_List_List*_T1D0=_cycalloc(sizeof(struct Cyc_List_List));_T15B=vds;_T15C=_T15B->hd;_T15D=(struct Cyc_Absyn_Vardecl*)_T15C;_T15F=vds;_T160=_T15F->hd;_T161=(struct Cyc_Absyn_Vardecl*)_T160;if(_T161!=0)goto _TL12C;_T15E=0;goto _TL12D;_TL12C: _T162=vds;_T163=_T162->hd;_T164=(struct Cyc_Absyn_Vardecl*)_T163;_T15E=_T164->type;_TL12D: _T1D0->hd=Cyc_AssnDef_fresh_var(_T15D,_T15E);_T1D0->tl=terms;_T15A=(struct Cyc_List_List*)_T1D0;}terms=_T15A;_T165=vds;
# 412
vds=_T165->tl;goto _TL12B;_TL12A:;}
# 415
terms=Cyc_List_imp_rev(terms);{struct Cyc_List_List*_T1D0=_cycalloc(sizeof(struct Cyc_List_List));
# 417
_T1D0->hd=Cyc_AssnDef_fresh_var(0,0);_T1D0->tl=terms;_T166=(struct Cyc_List_List*)_T1D0;}terms=_T166;_T167=f1;_T168=_T167.checks_assn;
# 420
if(_T168==0)goto _TL12E;_T169=f1;_T16A=_T169.checks_assn;_T16B=terms;{
void*chk1=Cyc_AssnDef_existassnfn2assn(_T16A,_T16B);_T16D=f2;_T16E=_T16D.checks_assn;_T16F=(unsigned)_T16E;
if(!_T16F)goto _TL130;_T170=f2;_T171=_T170.checks_assn;_T172=terms;_T16C=Cyc_AssnDef_existassnfn2assn(_T171,_T172);goto _TL131;_TL130: _T173=& Cyc_AssnDef_true_assn;_T174=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T173;_T16C=(void*)_T174;_TL131: {void*chk2=_T16C;_T175=
Cyc_AssnDef_simple_prove(chk2,chk1);if(_T175)goto _TL132;else{goto _TL134;}_TL134: _T176=
Cyc_PrattProver_constraint_prove(chk2,chk1);
# 423
if(_T176)goto _TL132;else{goto _TL135;}
# 425
_TL135:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T177=_T1D0;}return _T177;_TL132:;}}goto _TL12F;_TL12E: _TL12F: _T178=f1;_T179=_T178.requires_assn;
# 430
if(_T179==0)goto _TL136;_T17A=f1;_T17B=_T17A.requires_assn;_T17C=terms;{
void*req1=Cyc_AssnDef_existassnfn2assn(_T17B,_T17C);_T17E=f2;_T17F=_T17E.requires_assn;_T180=(unsigned)_T17F;
if(!_T180)goto _TL138;_T181=f2;_T182=_T181.requires_assn;_T183=terms;_T17D=Cyc_AssnDef_existassnfn2assn(_T182,_T183);goto _TL139;_TL138: _T184=& Cyc_AssnDef_true_assn;_T185=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T184;_T17D=(void*)_T185;_TL139: {void*req2=_T17D;_T186=
Cyc_AssnDef_simple_prove(req2,req1);if(_T186)goto _TL13A;else{goto _TL13C;}_TL13C: _T187=
Cyc_PrattProver_constraint_prove(req2,req1);
# 433
if(_T187)goto _TL13A;else{goto _TL13D;}
# 435
_TL13D:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T188=_T1D0;}return _T188;_TL13A:;}}goto _TL137;_TL136: _TL137: _T189=f2;_T18A=_T189.ensures_assn;
# 440
if(_T18A==0)goto _TL13E;_T18B=f1;_T18C=_T18B.return_value;_T18D=f1;_T18E=_T18D.return_value;_T18F=
_check_null(_T18E);_T190=_T18F->type;{void*ret_value=Cyc_AssnDef_fresh_var(_T18C,_T190);
# 443
struct Cyc_List_List*_T1D0;_T1D0=_cycalloc(sizeof(struct Cyc_List_List));_T1D0->hd=Cyc_AssnDef_fresh_var(0,0);_T1D0->tl=terms;{struct Cyc_List_List*terms=_T1D0;
struct Cyc_List_List*_T1D1;_T1D1=_cycalloc(sizeof(struct Cyc_List_List));_T1D1->hd=ret_value;_T1D1->tl=terms;{struct Cyc_List_List*terms=_T1D1;_T191=f2;_T192=_T191.ensures_assn;_T193=terms;{
void*ens2=Cyc_AssnDef_existassnfn2assn(_T192,_T193);_T195=f1;_T196=_T195.ensures_assn;_T197=(unsigned)_T196;
if(!_T197)goto _TL140;_T198=f1;_T199=_T198.ensures_assn;_T19A=terms;_T194=Cyc_AssnDef_existassnfn2assn(_T199,_T19A);goto _TL141;_TL140: _T19B=& Cyc_AssnDef_true_assn;_T19C=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T19B;_T194=(void*)_T19C;_TL141: {void*ens1=_T194;_T19D=
Cyc_AssnDef_simple_prove(ens1,ens2);if(_T19D)goto _TL142;else{goto _TL144;}_TL144: _T19E=
Cyc_PrattProver_constraint_prove(ens1,ens2);
# 447
if(_T19E)goto _TL142;else{goto _TL145;}
# 449
_TL145:{struct _tuple12 _T1D2;_T1D2.f0=0U;_T1D2.f1=0;_T19F=_T1D2;}return _T19F;_TL142:;}}}}}goto _TL13F;_TL13E: _TL13F: _T1A0=f2;_T1A1=_T1A0.throws_assn;
# 452
if(_T1A1==0)goto _TL146;_T1A2=f2;_T1A3=_T1A2.throws_assn;_T1A4=terms;{
void*thrws2=Cyc_AssnDef_existassnfn2assn(_T1A3,_T1A4);_T1A6=f1;_T1A7=_T1A6.throws_assn;_T1A8=(unsigned)_T1A7;
if(!_T1A8)goto _TL148;_T1A9=f1;_T1AA=_T1A9.throws_assn;_T1AB=terms;_T1A5=Cyc_AssnDef_existassnfn2assn(_T1AA,_T1AB);goto _TL149;_TL148: _T1AC=& Cyc_AssnDef_true_assn;_T1AD=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T1AC;_T1A5=(void*)_T1AD;_TL149: {void*thrws1=_T1A5;_T1AE=
Cyc_AssnDef_simple_prove(thrws1,thrws2);if(_T1AE)goto _TL14A;else{goto _TL14C;}_TL14C: _T1AF=
Cyc_PrattProver_constraint_prove(thrws1,thrws2);
# 455
if(_T1AF)goto _TL14A;else{goto _TL14D;}
# 457
_TL14D:{struct _tuple12 _T1D0;_T1D0.f0=0U;_T1D0.f1=0;_T1B0=_T1D0;}return _T1B0;_TL14A:;}}goto _TL147;_TL146: _TL147:{struct _tuple12 _T1D0;
# 460
_T1D0.f0=1U;_T1D0.f1=retc;_T1B1=_T1D0;}return _T1B1;}}}}}}_TLF3: goto _LLB;default: _LLB: _T1B2=
# 462
Cyc_Subtype_force_equivalence(t1,t2);return _T1B2;};}}
# 466
int Cyc_Subtype_subtype(unsigned loc,struct Cyc_List_List*assume,void*t1,void*t2){enum Cyc_Absyn_Coercion _T0;int _T1;int(*_T2)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*);int(*_T3)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T4)(unsigned,void*);unsigned _T5;struct Cyc_List_List*_T6;int _T7;
# 468
struct _tuple12 _T8=Cyc_Subtype_subtype_impl(assume,t1,t2);struct Cyc_List_List*_T9;enum Cyc_Absyn_Coercion _TA;_TA=_T8.f0;_T9=_T8.f1;{enum Cyc_Absyn_Coercion r=_TA;struct Cyc_List_List*c=_T9;_T0=r;_T1=(int)_T0;
if(_T1!=1)goto _TL14E;_T3=Cyc_List_forall_c;{
int(*_TB)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T3;_T2=_TB;}_T4=Cyc_BansheeIf_add_constraint;_T5=loc;_T6=c;_T7=_T2(_T4,_T5,_T6);return _T7;_TL14E:
# 472
 return 0;}}
# 482 "subtype.cyc"
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*assume,void*t1,void*t2){struct _tuple12 _T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Tqual _T5;int _T6;struct Cyc_Absyn_Tqual _T7;int _T8;struct _tuple12 _T9;int _TA;int _TB;struct Cyc_Absyn_Tqual _TC;int _TD;struct Cyc_Absyn_Kind*_TE;struct Cyc_Absyn_Kind*_TF;struct Cyc_Absyn_Kind*_T10;int _T11;enum Cyc_Absyn_Coercion _T12;int _T13;enum Cyc_Absyn_Coercion _T14;int _T15;struct _tuple12 _T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct _tuple12 _T19;
# 484
struct Cyc_List_List*tqs1=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t2);
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;
_TL153: if(tqs2!=0)goto _TL151;else{goto _TL152;}
_TL151: if(tqs1!=0)goto _TL154;{struct _tuple12 _T1A;_T1A.f0=0U;_T1A.f1=0;_T0=_T1A;}return _T0;_TL154: _T1=tqs1;_T2=_T1->hd;{
struct _tuple14*_T1A=(struct _tuple14*)_T2;void*_T1B;struct Cyc_Absyn_Tqual _T1C;{struct _tuple14 _T1D=*_T1A;_T1C=_T1D.f0;_T1B=_T1D.f1;}{struct Cyc_Absyn_Tqual tq1=_T1C;void*t1a=_T1B;_T3=tqs2;_T4=_T3->hd;{
struct _tuple14*_T1D=(struct _tuple14*)_T4;void*_T1E;struct Cyc_Absyn_Tqual _T1F;{struct _tuple14 _T20=*_T1D;_T1F=_T20.f0;_T1E=_T20.f1;}{struct Cyc_Absyn_Tqual tq2=_T1F;void*t2a=_T1E;_T5=tq1;_T6=_T5.real_const;
if(!_T6)goto _TL156;_T7=tq2;_T8=_T7.real_const;if(_T8)goto _TL156;else{goto _TL158;}_TL158:{struct _tuple12 _T20;_T20.f0=0U;_T20.f1=0;_T9=_T20;}return _T9;_TL156: _TA=
Cyc_Unify_unify(t1a,t2a);if(_TA)goto _TL15B;else{goto _TL15C;}_TL15C: _TB=Cyc_Subtype_isomorphic(t1a,t2a);if(_TB)goto _TL15B;else{goto _TL159;}
_TL15B: goto _TL150;_TL159: _TC=tq2;_TD=_TC.real_const;
if(_TD)goto _TL15F;else{goto _TL160;}_TL160: _TE=& Cyc_Kinds_ak;_TF=(struct Cyc_Absyn_Kind*)_TE;_T10=Cyc_Tcutil_type_kind(t2a);_T11=Cyc_Kinds_kind_leq(_TF,_T10);if(_T11)goto _TL15F;else{goto _TL15D;}
_TL15F:{struct _tuple12 _T20=Cyc_Subtype_subtype_impl(assume,t1a,t2a);struct Cyc_List_List*_T21;enum Cyc_Absyn_Coercion _T22;_T22=_T20.f0;_T21=_T20.f1;{enum Cyc_Absyn_Coercion s=_T22;struct Cyc_List_List*c=_T21;
coerce=Cyc_Subtype_join_coercion(s,coerce);_T12=coerce;_T13=(int)_T12;
if(_T13==0)goto _TL161;
retc=Cyc_List_imp_append(retc,c);goto _TL150;_TL161:;}}goto _TL15E;
# 504
_TL15D:{struct _tuple12 _T20=Cyc_Subtype_force_equivalence(t1a,t2a);struct Cyc_List_List*_T21;enum Cyc_Absyn_Coercion _T22;_T22=_T20.f0;_T21=_T20.f1;{enum Cyc_Absyn_Coercion s=_T22;struct Cyc_List_List*c=_T21;
coerce=Cyc_Subtype_join_coercion(s,coerce);_T14=coerce;_T15=(int)_T14;
if(_T15==0)goto _TL163;
retc=Cyc_List_imp_append(retc,c);goto _TL150;_TL163:;}}_TL15E:{struct _tuple12 _T20;
# 511
_T20.f0=0U;_T20.f1=0;_T16=_T20;}return _T16;}}}}_TL150: _T17=tqs2;
# 488
tqs2=_T17->tl;_T18=tqs1;tqs1=_T18->tl;goto _TL153;_TL152:{struct _tuple12 _T1A;
# 513
_T1A.f0=coerce;_T1A.f1=retc;_T19=_T1A;}return _T19;}
# 520
static int Cyc_Subtype_gen_default_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){enum Cyc_Absyn_KindQual _T0;struct _tuple15 _T1;void*_T2;int*_T3;int _T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;void*_TD;struct Cyc_Absyn_PtrInfo _TE;struct Cyc_Absyn_PtrAtts _TF;struct Cyc_Absyn_PtrInfo _T10;struct Cyc_Absyn_PtrAtts _T11;int _T12;int _T13;void*_T14;unsigned _T15;void*_T16;unsigned _T17;void*_T18;void*_T19;void*_T1A;void*_T1B;int _T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;unsigned _T22;void*_T23;void*_T24;void*_T25;unsigned _T26;struct Cyc_List_List**_T27;struct Cyc_List_List*_T28;struct Cyc_List_List**_T29;int(*_T2A)(struct _fat_ptr,struct _fat_ptr);void*(*_T2B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2C;struct _fat_ptr _T2D;_T0=k;if(_T0!=Cyc_Absyn_PtrBndKind)goto _TL165;{struct _tuple15 _T2E;
# 523
_T2E.f0=Cyc_Absyn_compress(_t1);_T2E.f1=Cyc_Absyn_compress(_t2);_T1=_T2E;}{struct _tuple15 _T2E=_T1;void*_T2F;void*_T30;void*_T31;void*_T32;_T2=_T2E.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=4)goto _TL167;_T5=_T2E.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=4)goto _TL169;_T8=_T2E.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T33=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T8;_T9=_T33->f1;_TA=_T9.ptr_atts;_T32=_TA.bounds;_TB=_T33->f1;_TC=_TB.ptr_atts;_T31=_TC.zero_term;}_TD=_T2E.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T33=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD;_TE=_T33->f1;_TF=_TE.ptr_atts;_T30=_TF.bounds;_T10=_T33->f1;_T11=_T10.ptr_atts;_T2F=_T11.zero_term;}{void*t1=_T32;void*z1=_T31;void*t2=_T30;void*z2=_T2F;
# 525
t1=Cyc_Absyn_compress(t1);t2=Cyc_Absyn_compress(t2);_T12=
Cyc_Tcutil_is_cvar_type(t1);if(_T12)goto _TL16B;else{goto _TL16D;}_TL16D: _T13=Cyc_Tcutil_is_cvar_type(t2);if(_T13)goto _TL16B;else{goto _TL16E;}
_TL16E: return 0;_TL16B: {
void*ct1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*ct2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
# 536
void*c=0;_T14=ct1;_T15=(unsigned)_T14;
if(!_T15)goto _TL16F;_T16=ct2;_T17=(unsigned)_T16;if(!_T17)goto _TL16F;_T18=ct2;_T19=Cyc_Absyn_thinconst();if(_T18==_T19)goto _TL16F;_T1A=ct1;_T1B=
# 542
Cyc_Absyn_thinconst();if(_T1A!=_T1B)goto _TL173;else{goto _TL174;}_TL174: _T1C=Cyc_Absyn_type2bool(0,z1);if(_T1C)goto _TL171;else{goto _TL173;}
_TL173: _T1D=ct2;_T1E=Cyc_Absyn_fatconst();_T1F=Cyc_BansheeIf_cmpeq_constraint(_T1D,_T1E);_T20=
Cyc_BansheeIf_equality_constraint(ct1,ct2);
# 543
c=Cyc_BansheeIf_implication_constraint(_T1F,_T20);goto _TL172;_TL171: _TL172: goto _TL170;
# 557
_TL16F: _T21=ct2;_T22=(unsigned)_T21;if(_T22)goto _TL175;else{goto _TL177;}
_TL177: _T23=_check_null(ct1);_T24=Cyc_Absyn_fatconst();c=Cyc_BansheeIf_equality_constraint(_T23,_T24);goto _TL176;_TL175: _TL176: _TL170: _T25=c;_T26=(unsigned)_T25;
# 561
if(!_T26)goto _TL178;_T27=retc;{struct Cyc_List_List*_T33=_cycalloc(sizeof(struct Cyc_List_List));
_T33->hd=c;_T29=retc;_T33->tl=*_T29;_T28=(struct Cyc_List_List*)_T33;}*_T27=_T28;goto _TL179;_TL178: _TL179:
# 565
 return 1;}}_TL169: goto _LL8;_TL167: _LL8: _T2B=Cyc_Warn_impos;{
# 567
int(*_T33)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2B;_T2A=_T33;}_T2C=_tag_fat("Non pointer type in gen constraint",sizeof(char),35U);_T2D=_tag_fat(0U,sizeof(void*),0);_T2A(_T2C,_T2D);;}goto _TL166;_TL165:
# 570
 return 0;_TL166:;}
# 574
static int Cyc_Subtype_gen_equality_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){enum Cyc_Absyn_KindQual _T0;struct _tuple15 _T1;void*_T2;int*_T3;int _T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;void*_TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PtrAtts _TD;void*_TE;unsigned _TF;void*_T10;unsigned _T11;struct Cyc_List_List**_T12;struct Cyc_List_List*_T13;struct Cyc_List_List**_T14;int(*_T15)(struct _fat_ptr,struct _fat_ptr);void*(*_T16)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T17;struct _fat_ptr _T18;_T0=k;if(_T0!=Cyc_Absyn_PtrBndKind)goto _TL17A;{struct _tuple15 _T19;
# 579
_T19.f0=_t1;_T19.f1=_t2;_T1=_T19;}{struct _tuple15 _T19=_T1;void*_T1A;void*_T1B;_T2=_T19.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=4)goto _TL17C;_T5=_T19.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=4)goto _TL17E;_T8=_T19.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T8;_T9=_T1C->f1;_TA=_T9.ptr_atts;_T1B=_TA.bounds;}_TB=_T19.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TB;_TC=_T1C->f1;_TD=_TC.ptr_atts;_T1A=_TD.bounds;}{void*t1=_T1B;void*t2=_T1A;
# 581
void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);_TE=cv1;_TF=(unsigned)_TE;
if(!_TF)goto _TL180;_T10=cv2;_T11=(unsigned)_T10;if(!_T11)goto _TL180;_T12=retc;{struct Cyc_List_List*_T1C=_cycalloc(sizeof(struct Cyc_List_List));
_T1C->hd=Cyc_BansheeIf_equality_constraint(cv1,cv2);_T14=retc;_T1C->tl=*_T14;_T13=(struct Cyc_List_List*)_T1C;}*_T12=_T13;
return 1;_TL180:
# 587
 return 0;}_TL17E: goto _LL8;_TL17C: _LL8: _T16=Cyc_Warn_impos;{
# 589
int(*_T1C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T16;_T15=_T1C;}_T17=_tag_fat("Non pointer type in gen constraint",sizeof(char),35U);_T18=_tag_fat(0U,sizeof(void*),0);_T15(_T17,_T18);;}goto _TL17B;_TL17A:
# 592
 return 0;_TL17B:;}struct _tuple17{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;int f2;};
# 604 "subtype.cyc"
static struct _tuple17 Cyc_Subtype_constraint_subtype(void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_coercion){int _T0;struct _tuple17 _T1;struct _tuple15 _T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7;void*_T8;int*_T9;int _TA;struct _tuple17 _TB;void*_TC;int*_TD;int _TE;void*_TF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10;void*_T11;int*_T12;int _T13;void*_T14;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15;void*_T16;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T17;enum Cyc_Absyn_Size_of _T18;int _T19;struct Cyc_Absyn_Kind*_T1A;enum Cyc_Absyn_KindQual _T1B;int _T1C;struct _tuple17 _T1D;int _T1E;void*_T1F;int _T20;struct _tuple17 _T21;struct Cyc_List_List*_T22;void*_T23;void*_T24;void*_T25;void*_T26;int*_T27;int _T28;struct _tuple15 _T29;void*_T2A;int*_T2B;int _T2C;void*_T2D;int*_T2E;int _T2F;void*_T30;void*_T31;struct Cyc_Absyn_Vardecl*_T32;void*_T33;struct Cyc_Absyn_Vardecl*_T34;void*_T35;int _T36;struct _tuple17 _T37;struct Cyc_List_List*_T38;struct Cyc_Absyn_Vardecl*_T39;struct Cyc_Absyn_Vardecl*_T3A;void*_T3B;struct Cyc_List_List*_T3C;struct Cyc_AssnDef_ExistAssnFn*_T3D;struct Cyc_List_List*_T3E;struct Cyc_AssnDef_ExistAssnFn*_T3F;struct Cyc_List_List*_T40;int _T41;int _T42;struct _tuple17 _T43;struct _tuple17 _T44;void*_T45;struct Cyc_Absyn_Vardecl*_T46;void*_T47;void*_T48;int _T49;struct _tuple17 _T4A;struct _tuple17 _T4B;void*_T4C;int*_T4D;int _T4E;void*_T4F;void*_T50;struct Cyc_Absyn_Vardecl*_T51;void*_T52;int _T53;struct _tuple17 _T54;struct _tuple17 _T55;void*_T56;int*_T57;unsigned _T58;void*_T59;int*_T5A;int _T5B;void*_T5C;struct Cyc_Absyn_PtrInfo _T5D;struct Cyc_Absyn_PtrInfo _T5E;struct Cyc_Absyn_PtrInfo _T5F;struct Cyc_Absyn_PtrAtts _T60;struct Cyc_Absyn_PtrInfo _T61;struct Cyc_Absyn_PtrAtts _T62;struct Cyc_Absyn_PtrInfo _T63;struct Cyc_Absyn_PtrAtts _T64;struct Cyc_Absyn_PtrInfo _T65;struct Cyc_Absyn_PtrAtts _T66;struct Cyc_Absyn_PtrInfo _T67;struct Cyc_Absyn_PtrAtts _T68;struct Cyc_Absyn_PtrInfo _T69;struct Cyc_Absyn_PtrAtts _T6A;void*_T6B;struct Cyc_Absyn_PtrInfo _T6C;struct Cyc_Absyn_PtrInfo _T6D;struct Cyc_Absyn_PtrInfo _T6E;struct Cyc_Absyn_PtrAtts _T6F;struct Cyc_Absyn_PtrInfo _T70;struct Cyc_Absyn_PtrAtts _T71;struct Cyc_Absyn_PtrInfo _T72;struct Cyc_Absyn_PtrAtts _T73;struct Cyc_Absyn_PtrInfo _T74;struct Cyc_Absyn_PtrAtts _T75;struct Cyc_Absyn_PtrInfo _T76;struct Cyc_Absyn_PtrAtts _T77;struct Cyc_Absyn_PtrInfo _T78;struct Cyc_Absyn_PtrAtts _T79;int _T7A;int _T7B;int _T7C;int _T7D;struct _tuple17 _T7E;struct Cyc_Absyn_Tqual _T7F;int _T80;struct Cyc_Absyn_Tqual _T81;int _T82;struct _tuple17 _T83;int _T84;int _T85;struct _tuple17 _T86;int _T87;int _T88;struct Cyc_List_List**_T89;void*_T8A;void*_T8B;int _T8C;struct _tuple17 _T8D;int _T8E;int _T8F;struct _tuple17 _T90;int _T91;int _T92;struct Cyc_List_List**_T93;void*_T94;void*_T95;int _T96;struct _tuple17 _T97;int _T98;int _T99;int _T9A;void*_T9B;void*_T9C;void*_T9D;void*_T9E;struct Cyc_Absyn_Exp*_T9F;unsigned _TA0;struct Cyc_Absyn_Exp*_TA1;unsigned _TA2;int _TA3;struct _tuple17 _TA4;struct Cyc_Absyn_Exp*_TA5;unsigned _TA6;struct Cyc_List_List**_TA7;void*_TA8;void*_TA9;int _TAA;struct _tuple17 _TAB;int _TAC;int _TAD;struct _tuple17 _TAE;enum Cyc_Absyn_Coercion _TAF;int _TB0;struct _tuple17 _TB1;int _TB2;int _TB3;int _TB4;struct Cyc_Absyn_Tqual _TB5;int _TB6;struct Cyc_Absyn_Tqual _TB7;int _TB8;struct _tuple17 _TB9;int _TBA;int _TBB;void*_TBC;void*_TBD;int _TBE;void*_TBF;unsigned _TC0;struct Cyc_List_List*_TC1;void*_TC2;void*_TC3;int _TC4;enum Cyc_Absyn_Coercion _TC5;int _TC6;struct _tuple17 _TC7;struct _tuple17 _TC8;void*_TC9;int*_TCA;int _TCB;void*_TCC;struct Cyc_Absyn_ArrayInfo _TCD;struct Cyc_Absyn_ArrayInfo _TCE;struct Cyc_Absyn_ArrayInfo _TCF;struct Cyc_Absyn_ArrayInfo _TD0;void*_TD1;struct Cyc_Absyn_ArrayInfo _TD2;struct Cyc_Absyn_ArrayInfo _TD3;struct Cyc_Absyn_ArrayInfo _TD4;struct Cyc_Absyn_ArrayInfo _TD5;int _TD6;struct _tuple17 _TD7;struct _tuple17 _TD8;int _TD9;struct _tuple17 _TDA;enum Cyc_Absyn_Coercion _TDB;int _TDC;struct _tuple17 _TDD;struct _tuple17 _TDE;struct Cyc_Absyn_Tqual _TDF;int _TE0;struct Cyc_Absyn_Tqual _TE1;int _TE2;struct _tuple17 _TE3;struct _tuple17 _TE4;void*_TE5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE6;void*_TE7;int*_TE8;unsigned _TE9;void*_TEA;void*_TEB;void*_TEC;int*_TED;int _TEE;void*_TEF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TF0;void*_TF1;int*_TF2;int _TF3;void*_TF4;void*_TF5;struct Cyc_Absyn_Enumdecl*_TF6;struct Cyc_Core_Opt*_TF7;struct Cyc_Absyn_Enumdecl*_TF8;struct Cyc_Core_Opt*_TF9;struct Cyc_Absyn_Enumdecl*_TFA;struct Cyc_Core_Opt*_TFB;void*_TFC;struct Cyc_List_List*_TFD;int _TFE;struct Cyc_Absyn_Enumdecl*_TFF;struct Cyc_Core_Opt*_T100;struct Cyc_Core_Opt*_T101;void*_T102;struct Cyc_List_List*_T103;int _T104;struct _tuple17 _T105;struct _tuple17 _T106;int _T107;struct _tuple17 _T108;struct _tuple17 _T109;void*_T10A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10B;struct Cyc_List_List*_T10C;void*_T10D;int*_T10E;int _T10F;void*_T110;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T111;void*_T112;int*_T113;int _T114;void*_T115;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T116;struct Cyc_List_List*_T117;void*_T118;struct Cyc_List_List*_T119;void*_T11A;void*_T11B;struct Cyc_List_List*_T11C;void*_T11D;int _T11E;struct _tuple17 _T11F;struct _tuple17 _T120;_T0=
# 610
Cyc_Unify_unify(t1,t2);if(!_T0)goto _TL182;{struct _tuple17 _T121;
_T121.f0=1U;_T121.f1=0;_T121.f2=1;_T1=_T121;}return _T1;_TL182:
# 613
 t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{struct _tuple15 _T121;
_T121.f0=t1;_T121.f1=t2;_T2=_T121;}{struct _tuple15 _T121=_T2;_T3=_T121.f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=0)goto _TL184;_T6=_T121.f1;_T7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6;_T8=_T7->f1;_T9=(int*)_T8;_TA=*_T9;if(_TA!=0)goto _TL186;{struct _tuple17 _T122;
_T122.f0=1U;_T122.f1=0;_T122.f2=1;_TB=_T122;}return _TB;_TL186: _TC=_T121.f0;_TD=(int*)_TC;_TE=*_TD;if(_TE!=12)goto _TL188;goto _LL3;_TL188: _TF=_T121.f1;_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T11=_T10->f1;_T12=(int*)_T11;_T13=*_T12;if(_T13!=1)goto _TL18A;_T14=_T121.f1;_T15=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14;_T16=_T15->f1;_T17=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T16;_T18=_T17->f2;_T19=(int)_T18;switch(_T19){case Cyc_Absyn_Int_sz: goto _LL8;case Cyc_Absyn_Long_sz: _LL8: _T1A=
# 623
Cyc_Tcutil_type_kind(t1);_T1B=_T1A->kind;_T1C=(int)_T1B;if(_T1C!=2)goto _TL18D;{struct _tuple17 _T122;_T122.f0=4U;_T122.f1=0;_T122.f2=0;_T1D=_T122;}return _T1D;_TL18D: _T1E=
Cyc_Tcutil_is_pointer_type(t1);if(!_T1E)goto _TL18F;_T1F=
Cyc_Tcutil_get_pointer_bounds(t1);{void*cv1=_check_null(_T1F);_T20=
Cyc_Tcutil_is_cvar_type(cv1);if(!_T20)goto _TL191;{struct _tuple17 _T122;
_T122.f0=4U;{struct Cyc_List_List*_T123=_cycalloc(sizeof(struct Cyc_List_List));_T23=Cyc_Tcutil_ptrbnd_cvar_equivalent(cv1);_T24=_check_null(_T23);_T25=Cyc_Absyn_thinconst();_T123->hd=Cyc_BansheeIf_equality_constraint(_T24,_T25);_T123->tl=0;_T22=(struct Cyc_List_List*)_T123;}_T122.f1=_T22;_T122.f2=0;_T21=_T122;}return _T21;_TL191:;}goto _TL190;_TL18F: _TL190: goto _LL0;default: goto _LL9;}goto _TL18B;_TL18A: goto _LL9;_TL18B: goto _TL185;_TL184: _T26=_T121.f0;_T27=(int*)_T26;_T28=*_T27;if(_T28!=12)goto _TL193;_LL3: goto _LL0;_TL193: _LL9: goto _LL0;_TL185: _LL0:;}{
# 634
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;{struct _tuple15 _T121;
_T121.f0=t1;_T121.f1=t2;_T29=_T121;}{struct _tuple15 _T121=_T29;struct Cyc_Absyn_Exp*_T122;void*_T123;void*_T124;void*_T125;void*_T126;void*_T127;void*_T128;struct Cyc_Absyn_Tqual _T129;void*_T12A;void*_T12B;void*_T12C;void*_T12D;struct Cyc_Absyn_Tqual _T12E;void*_T12F;void*_T130;void*_T131;void*_T132;_T2A=_T121.f0;_T2B=(int*)_T2A;_T2C=*_T2B;if(_T2C!=12)goto _TL195;_T2D=_T121.f1;_T2E=(int*)_T2D;_T2F=*_T2E;if(_T2F!=12)goto _TL197;_T30=_T121.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T30;_T132=_T133->f1;_T131=_T133->f3;}_T31=_T121.f1;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T31;_T130=_T133->f1;_T12F=_T133->f3;}{struct Cyc_Absyn_Vardecl*vd1=_T132;struct Cyc_AssnDef_ExistAssnFn*a1=_T131;struct Cyc_Absyn_Vardecl*vd2=_T130;struct Cyc_AssnDef_ExistAssnFn*a2=_T12F;_T32=vd1;_T33=_T32->type;_T34=vd2;_T35=_T34->type;_T36=
# 640
Cyc_Unify_unify(_T33,_T35);if(_T36)goto _TL199;else{goto _TL19B;}_TL19B:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T37=_T133;}return _T37;_TL199: {
struct Cyc_List_List*terms;terms=_cycalloc(sizeof(struct Cyc_List_List));_T38=terms;_T39=vd1;_T3A=vd1;_T3B=_T3A->type;_T38->hd=Cyc_AssnDef_fresh_var(_T39,_T3B);_T3C=terms;_T3C->tl=0;_T3D=
_check_null(a1);_T3E=terms;{void*assn1=Cyc_AssnDef_existassnfn2assn(_T3D,_T3E);_T3F=
_check_null(a2);_T40=terms;{void*assn2=Cyc_AssnDef_existassnfn2assn(_T3F,_T40);_T41=
# 645
Cyc_AssnDef_simple_prove(assn1,assn2);if(_T41)goto _TL19E;else{goto _TL19F;}_TL19F: _T42=
Cyc_PrattProver_constraint_prove(assn1,assn2);
# 645
if(_T42)goto _TL19E;else{goto _TL19C;}
# 647
_TL19E:{struct _tuple17 _T133;_T133.f0=1U;_T133.f1=0;_T133.f2=1;_T43=_T133;}return _T43;
# 649
_TL19C:{struct _tuple17 _T133;_T133.f0=3U;_T133.f1=0;_T133.f2=1;_T44=_T133;}return _T44;}}}}_TL197: _T45=_T121.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T45;_T132=_T133->f1;_T131=_T133->f3;}_T130=_T121.f1;{struct Cyc_Absyn_Vardecl*vd1=_T132;struct Cyc_AssnDef_ExistAssnFn*a1=_T131;void*t2=_T130;_T46=vd1;_T47=_T46->type;_T48=t2;_T49=
# 652
Cyc_Unify_unify(_T47,_T48);if(_T49)goto _TL1A0;else{goto _TL1A2;}_TL1A2:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T4A=_T133;}return _T4A;_TL1A0:{struct _tuple17 _T133;
_T133.f0=1U;_T133.f1=0;_T133.f2=1;_T4B=_T133;}return _T4B;}_TL195: _T4C=_T121.f1;_T4D=(int*)_T4C;_T4E=*_T4D;if(_T4E!=12)goto _TL1A3;_T4F=_T121.f1;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T4F;_T132=_T133->f1;_T131=_T133->f3;}{struct Cyc_Absyn_Vardecl*vd2=_T132;struct Cyc_AssnDef_ExistAssnFn*a2=_T131;_T50=t1;_T51=vd2;_T52=_T51->type;_T53=
# 656
Cyc_Unify_unify(_T50,_T52);if(_T53)goto _TL1A5;else{goto _TL1A7;}_TL1A7:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T54=_T133;}return _T54;_TL1A5:{struct _tuple17 _T133;
_T133.f0=3U;_T133.f1=0;_T133.f2=1;_T55=_T133;}return _T55;}_TL1A3: _T56=_T121.f0;_T57=(int*)_T56;_T58=*_T57;switch(_T58){case 4: _T59=_T121.f1;_T5A=(int*)_T59;_T5B=*_T5A;if(_T5B!=4)goto _TL1A9;_T5C=_T121.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T5C;_T5D=_T133->f1;_T132=_T5D.elt_type;_T5E=_T133->f1;_T12E=_T5E.elt_tq;_T5F=_T133->f1;_T60=_T5F.ptr_atts;_T131=_T60.eff;_T61=_T133->f1;_T62=_T61.ptr_atts;_T130=_T62.nullable;_T63=_T133->f1;_T64=_T63.ptr_atts;_T12F=_T64.bounds;_T65=_T133->f1;_T66=_T65.ptr_atts;_T12D=_T66.zero_term;_T67=_T133->f1;_T68=_T67.ptr_atts;_T12C=_T68.autoreleased;_T69=_T133->f1;_T6A=_T69.ptr_atts;_T12B=_T6A.aqual;}_T6B=_T121.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6B;_T6C=_T133->f1;_T12A=_T6C.elt_type;_T6D=_T133->f1;_T129=_T6D.elt_tq;_T6E=_T133->f1;_T6F=_T6E.ptr_atts;_T128=_T6F.eff;_T70=_T133->f1;_T71=_T70.ptr_atts;_T127=_T71.nullable;_T72=_T133->f1;_T73=_T72.ptr_atts;_T126=_T73.bounds;_T74=_T133->f1;_T75=_T74.ptr_atts;_T125=_T75.zero_term;_T76=_T133->f1;_T77=_T76.ptr_atts;_T124=_T77.autoreleased;_T78=_T133->f1;_T79=_T78.ptr_atts;_T123=_T79.aqual;}{void*t1a=_T132;struct Cyc_Absyn_Tqual tqual1a=_T12E;void*eff1=_T131;void*null1a=_T130;void*b1=_T12F;void*zt1=_T12D;void*rel1=_T12C;void*aq1=_T12B;void*t2a=_T12A;struct Cyc_Absyn_Tqual tqual2a=_T129;void*eff2=_T128;void*null2a=_T127;void*b2=_T126;void*zt2=_T125;void*rel2=_T124;void*aq2=_T123;_T7A=
# 660
Cyc_Unify_unify(null1a,null2a);if(_T7A)goto _TL1AB;else{goto _TL1AD;}_TL1AD: _T7B=Cyc_Tcutil_force_type2bool(0,null1a);if(!_T7B)goto _TL1AB;_T7C=Cyc_Tcutil_force_type2bool(0,null2a);if(_T7C)goto _TL1AB;else{goto _TL1AE;}
_TL1AE: _T7D=allow_coercion;if(!_T7D)goto _TL1AF;
coerce=2U;goto _TL1B0;
# 664
_TL1AF:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T7E=_T133;}return _T7E;_TL1B0: goto _TL1AC;_TL1AB: _TL1AC: _T7F=tqual2a;_T80=_T7F.real_const;
# 666
if(_T80)goto _TL1B1;else{goto _TL1B3;}_TL1B3: _T81=tqual1a;_T82=_T81.real_const;if(!_T82)goto _TL1B1;{struct _tuple17 _T133;
_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T83=_T133;}return _T83;_TL1B1: _T84=
# 669
Cyc_Unify_unify(eff1,eff2);if(_T84)goto _TL1B4;else{goto _TL1B6;}_TL1B6: _T85=
Cyc_Tcutil_subset_effect(1,eff1,eff2);
# 669
if(_T85)goto _TL1B4;else{goto _TL1B7;}
# 671
_TL1B7:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T86=_T133;}return _T86;_TL1B4: _T87=
# 674
Cyc_Unify_unify(aq1,aq2);if(_T87)goto _TL1B8;else{goto _TL1BA;}_TL1BA: _T88=Cyc_Subtype_alias_qual_subtype(aq1,aq2,3);if(_T88)goto _TL1B8;else{goto _TL1BB;}_TL1BB: _T89=& retc;_T8A=t1;_T8B=t2;_T8C=
genconstr(_T89,7U,_T8A,_T8B);
# 674
if(_T8C)goto _TL1B8;else{goto _TL1BC;}
# 676
_TL1BC:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T8D=_T133;}return _T8D;_TL1B8: _T8E=
# 678
Cyc_Unify_unify(rel1,rel2);if(_T8E)goto _TL1BD;else{goto _TL1BF;}_TL1BF: _T8F=Cyc_Absyn_type2bool(0,rel2);if(_T8F)goto _TL1BD;else{goto _TL1C0;}
_TL1C0:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T90=_T133;}return _T90;_TL1BD: _T91=
# 686
Cyc_Unify_unify(zt1,zt2);if(_T91)goto _TL1C1;else{goto _TL1C3;}_TL1C3: _T92=Cyc_Absyn_type2bool(1,zt2);if(!_T92)goto _TL1C1;_T93=& retc;_T94=t1;_T95=t2;_T96=genconstr(_T93,5U,_T94,_T95);if(_T96)goto _TL1C1;else{goto _TL1C4;}
# 697 "subtype.cyc"
_TL1C4:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T97=_T133;}return _T97;_TL1C1: {
# 699
int silent=1;_T98=
Cyc_Unify_unify(b1,b2);if(_T98)goto _TL1C5;else{goto _TL1C7;}
_TL1C7: _T99=Cyc_Tcutil_is_cvar_type(b1);if(_T99)goto _TL1C8;else{goto _TL1CA;}_TL1CA: _T9A=Cyc_Tcutil_is_cvar_type(b2);if(_T9A)goto _TL1C8;else{goto _TL1CB;}
_TL1CB: _T9B=Cyc_Absyn_bounds_one();_T9C=b1;{struct Cyc_Absyn_Exp*e1=Cyc_Tcutil_get_bounds_exp(_T9B,_T9C);_T9D=
Cyc_Absyn_bounds_one();_T9E=b2;{struct Cyc_Absyn_Exp*e2=Cyc_Tcutil_get_bounds_exp(_T9D,_T9E);_T9F=e1;_TA0=(unsigned)_T9F;
if(!_TA0)goto _TL1CC;_TA1=e2;_TA2=(unsigned)_TA1;if(!_TA2)goto _TL1CC;_TA3=
Cyc_Evexp_lte_const_exp(e2,e1);if(_TA3)goto _TL1CE;else{goto _TL1D0;}
_TL1D0:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TA4=_T133;}return _TA4;_TL1CE:
 silent=1;goto _TL1CD;
# 709
_TL1CC: _TA5=e2;_TA6=(unsigned)_TA5;if(!_TA6)goto _TL1D1;
silent=0;goto _TL1D2;_TL1D1: _TL1D2: _TL1CD:;}}goto _TL1C9;
# 714
_TL1C8: _TA7=& retc;_TA8=t1;_TA9=t2;_TAA=genconstr(_TA7,6U,_TA8,_TA9);if(_TAA)goto _TL1D3;else{goto _TL1D5;}
_TL1D5:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TAB=_T133;}return _TAB;_TL1D3: _TL1C9:
# 718
 coerce=Cyc_Subtype_join_coercion(coerce,4U);goto _TL1C6;_TL1C5: _TL1C6: _TAC=
# 720
Cyc_Subtype_isomorphic(t1a,t2a);if(_TAC)goto _TL1D8;else{goto _TL1D9;}_TL1D9: _TAD=Cyc_Unify_unify(t1a,t2a);if(_TAD)goto _TL1D8;else{goto _TL1D6;}
_TL1D8:{struct _tuple17 _T133;_T133.f0=coerce;_T133.f1=retc;_T133.f2=silent;_TAE=_T133;}return _TAE;
# 723
_TL1D6:{struct _tuple12 _T133=Cyc_Subtype_force_equivalence(t1a,t2a);struct Cyc_List_List*_T134;enum Cyc_Absyn_Coercion _T135;_T135=_T133.f0;_T134=_T133.f1;{enum Cyc_Absyn_Coercion s=_T135;struct Cyc_List_List*c=_T134;_TAF=s;_TB0=(int)_TAF;
if(_TB0==0)goto _TL1DA;{struct _tuple17 _T136;
_T136.f0=Cyc_Subtype_join_coercion(coerce,s);_T136.f1=Cyc_List_imp_append(retc,c);_T136.f2=silent;_TB1=_T136;}return _TB1;_TL1DA:;}}_TB2=
# 728
Cyc_Tcutil_is_bits_only_type(t1a);if(!_TB2)goto _TL1DC;_TB3=Cyc_Tcutil_is_char_type(t2a);if(!_TB3)goto _TL1DC;_TB4=
Cyc_Tcutil_force_type2bool(0,zt2);
# 728
if(_TB4)goto _TL1DC;else{goto _TL1DE;}_TL1DE: _TB5=tqual2a;_TB6=_TB5.real_const;if(_TB6)goto _TL1DF;else{goto _TL1E0;}_TL1E0: _TB7=tqual1a;_TB8=_TB7.real_const;if(_TB8)goto _TL1DC;else{goto _TL1DF;}
# 731
_TL1DF:{struct _tuple17 _T133;_T133.f0=coerce;_T133.f1=retc;_T133.f2=silent;_TB9=_T133;}return _TB9;_TL1DC: _TBA=
# 733
Cyc_Tcutil_force_type2bool(0,zt2);{int deep_subtype=!_TBA;_TBB=deep_subtype;
# 735
if(!_TBB)goto _TL1E1;_TBC=b2;_TBD=Cyc_Absyn_bounds_one();_TBE=Cyc_Unify_unify(_TBC,_TBD);if(_TBE)goto _TL1E1;else{goto _TL1E3;}
_TL1E3:{void*cb2=Cyc_Tcutil_ptrbnd_cvar_equivalent(b2);_TBF=cb2;_TC0=(unsigned)_TBF;
if(!_TC0)goto _TL1E4;{struct Cyc_List_List*_T133=_cycalloc(sizeof(struct Cyc_List_List));_TC2=cb2;_TC3=
Cyc_Absyn_thinconst();_T133->hd=Cyc_BansheeIf_equality_constraint(_TC2,_TC3);_T133->tl=retc;_TC1=(struct Cyc_List_List*)_T133;}retc=_TC1;goto _TL1E5;
# 741
_TL1E4: deep_subtype=0;_TL1E5:;}goto _TL1E2;_TL1E1: _TL1E2: _TC4=deep_subtype;
# 744
if(!_TC4)goto _TL1E6;{
struct _tuple12 _T133=Cyc_Subtype_ptrsubtype(0,t1a,t2a);struct Cyc_List_List*_T134;enum Cyc_Absyn_Coercion _T135;_T135=_T133.f0;_T134=_T133.f1;{enum Cyc_Absyn_Coercion s=_T135;struct Cyc_List_List*c=_T134;_TC5=s;_TC6=(int)_TC5;
if(_TC6==0)goto _TL1E8;{struct _tuple17 _T136;
_T136.f0=Cyc_Subtype_join_coercion(coerce,4U);_T136.f1=Cyc_List_imp_append(retc,c);_T136.f2=silent;_TC7=_T136;}return _TC7;_TL1E8:;}}goto _TL1E7;_TL1E6: _TL1E7:{struct _tuple17 _T133;
# 750
_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TC8=_T133;}return _TC8;}}}_TL1A9: goto _LL1E;case 5: _TC9=_T121.f1;_TCA=(int*)_TC9;_TCB=*_TCA;if(_TCB!=5)goto _TL1EA;_TCC=_T121.f0;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TCC;_TCD=_T133->f1;_T132=_TCD.elt_type;_TCE=_T133->f1;_T12E=_TCE.tq;_TCF=_T133->f1;_T131=_TCF.num_elts;_TD0=_T133->f1;_T130=_TD0.zero_term;}_TD1=_T121.f1;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TD1;_TD2=_T133->f1;_T12F=_TD2.elt_type;_TD3=_T133->f1;_T129=_TD3.tq;_TD4=_T133->f1;_T122=_TD4.num_elts;_TD5=_T133->f1;_T12D=_TD5.zero_term;}{void*t1a=_T132;struct Cyc_Absyn_Tqual tq1a=_T12E;struct Cyc_Absyn_Exp*e1=_T131;void*zt1=_T130;void*t2a=_T12F;struct Cyc_Absyn_Tqual tq2a=_T129;struct Cyc_Absyn_Exp*e2=_T122;void*zt2=_T12D;_TD6=
# 752
Cyc_Unify_unify(zt1,zt2);if(_TD6)goto _TL1EC;else{goto _TL1EE;}_TL1EE:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TD7=_T133;}return _TD7;_TL1EC:
 if(e1==0)goto _TL1F1;else{goto _TL1F2;}_TL1F2: if(e2==0)goto _TL1F1;else{goto _TL1EF;}_TL1F1:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TD8=_T133;}return _TD8;_TL1EF: _TD9=
Cyc_Evexp_same_uint_const_exp(e1,e2);if(_TD9)goto _TL1F3;else{goto _TL1F5;}_TL1F5:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_TDA=_T133;}return _TDA;_TL1F3: {
struct _tuple12 _T133=Cyc_Subtype_force_equivalence(t1a,t2a);struct Cyc_List_List*_T134;enum Cyc_Absyn_Coercion _T135;_T135=_T133.f0;_T134=_T133.f1;{enum Cyc_Absyn_Coercion s=_T135;struct Cyc_List_List*c=_T134;_TDB=s;_TDC=(int)_TDB;
if(_TDC!=0)goto _TL1F6;{struct _tuple17 _T136;
_T136.f0=0U;_T136.f1=0;_T136.f2=0;_TDD=_T136;}return _TDD;_TL1F6: _TDF=tq1a;_TE0=_TDF.real_const;
if(_TE0)goto _TL1FB;else{goto _TL1FA;}_TL1FB: _TE1=tq2a;_TE2=_TE1.real_const;if(_TE2)goto _TL1FA;else{goto _TL1F8;}_TL1FA:{struct _tuple17 _T136;_T136.f0=1U;_T136.f1=c;_T136.f2=1;_TE3=_T136;}_TDE=_TE3;goto _TL1F9;_TL1F8:{struct _tuple17 _T136;_T136.f0=0U;_T136.f1=0;_T136.f2=0;_TE4=_T136;}_TDE=_TE4;_TL1F9: return _TDE;}}}_TL1EA: goto _LL1E;case 0: _TE5=_T121.f0;_TE6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE5;_TE7=_TE6->f1;_TE8=(int*)_TE7;_TE9=*_TE8;switch(_TE9){case 19: _TEA=_T121.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TEA;_TEB=_T133->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T134=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TEB;_T132=_T134->f2;}}{struct Cyc_Absyn_Enumdecl*ed1=_T132;{struct Cyc_Absyn_Enumdecl*_T133;_TEC=t2;_TED=(int*)_TEC;_TEE=*_TED;if(_TEE!=0)goto _TL1FD;_TEF=t2;_TF0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TEF;_TF1=_TF0->f1;_TF2=(int*)_TF1;_TF3=*_TF2;if(_TF3!=19)goto _TL1FF;_TF4=t2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF4;_TF5=_T134->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T135=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TF5;_T133=_T135->f2;}}{struct Cyc_Absyn_Enumdecl*ed2=_T133;_TF6=
# 764
_check_null(ed1);_TF7=_TF6->fields;if(_TF7==0)goto _TL201;_TF8=_check_null(ed2);_TF9=_TF8->fields;if(_TF9==0)goto _TL201;_TFA=ed1;_TFB=_TFA->fields;_TFC=_TFB->v;_TFD=(struct Cyc_List_List*)_TFC;_TFE=
Cyc_List_length(_TFD);_TFF=ed2;_T100=_TFF->fields;_T101=_check_null(_T100);_T102=_T101->v;_T103=(struct Cyc_List_List*)_T102;_T104=Cyc_List_length(_T103);
# 764
if(_TFE < _T104)goto _TL201;{struct _tuple17 _T134;
# 766
_T134.f0=1U;_T134.f1=0;_T134.f2=1;_T105=_T134;}return _T105;_TL201: goto _LL29;}_TL1FF: goto _LL2C;_TL1FD: _LL2C: goto _LL29;_LL29:;}goto _LL19;}case 1: _LL19: goto _LL1B;case 2: _LL1B: _T107=
# 773
Cyc_Tcutil_is_strict_arithmetic_type(t2);if(!_T107)goto _TL203;{struct _tuple17 _T133;_T133.f0=1U;_T133.f1=0;_T133.f2=1;_T108=_T133;}_T106=_T108;goto _TL204;_TL203:{struct _tuple17 _T133;_T133.f0=0U;_T133.f1=0;_T133.f2=0;_T109=_T133;}_T106=_T109;_TL204: return _T106;case 4: _T10A=_T121.f0;_T10B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10A;_T10C=_T10B->f2;if(_T10C==0)goto _TL205;_T10D=_T121.f1;_T10E=(int*)_T10D;_T10F=*_T10E;if(_T10F!=0)goto _TL207;_T110=_T121.f1;_T111=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T110;_T112=_T111->f1;_T113=(int*)_T112;_T114=*_T113;if(_T114!=4)goto _TL209;_T115=_T121.f1;_T116=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T115;_T117=_T116->f2;if(_T117==0)goto _TL20B;_T118=_T121.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T118;_T119=_T133->f2;{struct Cyc_List_List _T134=*_T119;_T11A=_T134.hd;_T132=(void*)_T11A;}}_T11B=_T121.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T133=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_T11C=_T133->f2;{struct Cyc_List_List _T134=*_T11C;_T11D=_T134.hd;_T131=(void*)_T11D;}}{void*r1=_T132;void*r2=_T131;_T11E=
# 776
Cyc_Tcutil_subset_effect(1,r1,r2);if(!_T11E)goto _TL20D;{struct _tuple17 _T133;
_T133.f0=1U;_T133.f1=0;_T133.f2=1;_T11F=_T133;}return _T11F;_TL20D: goto _LL1F;}_TL20B: goto _LL1E;_TL209: goto _LL1E;_TL207: goto _LL1E;_TL205: goto _LL1E;default: goto _LL1E;};default: _LL1E: _LL1F: {
# 783
struct _tuple12 _T133=Cyc_Subtype_force_equivalence(t1,t2);struct Cyc_List_List*_T134;enum Cyc_Absyn_Coercion _T135;_T135=_T133.f0;_T134=_T133.f1;{enum Cyc_Absyn_Coercion s=_T135;struct Cyc_List_List*c=_T134;{struct _tuple17 _T136;
_T136.f0=s;_T136.f1=c;_T136.f2=1;_T120=_T136;}return _T120;}}};}}}
# 789
static enum Cyc_Absyn_Coercion Cyc_Subtype_internal_coercible_c(unsigned loc,int warn,void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_coercion){enum Cyc_Absyn_Coercion _T0;int _T1;enum Cyc_Absyn_Coercion _T2;int(*_T3)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*);int(*_T4)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T5)(unsigned,void*);unsigned _T6;struct Cyc_List_List*_T7;int _T8;int _T9;int _TA;int _TB;int _TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_Typ_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;unsigned _T13;struct _fat_ptr _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Typ_Warn_Warg_struct _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_Typ_Warn_Warg_struct _T18;unsigned _T19;struct _fat_ptr _T1A;enum Cyc_Absyn_Coercion _T1B;
# 796
struct _tuple17 _T1C=Cyc_Subtype_constraint_subtype(t1,t2,genconstr,allow_coercion);int _T1D;struct Cyc_List_List*_T1E;enum Cyc_Absyn_Coercion _T1F;_T1F=_T1C.f0;_T1E=_T1C.f1;_T1D=_T1C.f2;{enum Cyc_Absyn_Coercion s=_T1F;struct Cyc_List_List*c=_T1E;int b=_T1D;_T0=s;_T1=(int)_T0;
if(_T1!=0)goto _TL20F;_T2=s;
return _T2;_TL20F: _T4=Cyc_List_forall_c;{
int(*_T20)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T4;_T3=_T20;}_T5=Cyc_BansheeIf_add_constraint;_T6=loc;_T7=c;_T8=_T3(_T5,_T6,_T7);if(!_T8)goto _TL211;_T9=b;
if(_T9)goto _TL213;else{goto _TL215;}_TL215: _TA=warn;if(!_TA)goto _TL213;_TB=
Cyc_Tcutil_is_arithmetic_type(t1);if(!_TB)goto _TL216;_TC=Cyc_Tcutil_is_arithmetic_type(t2);if(!_TC)goto _TL216;_TD=
Cyc_Tcutil_will_lose_precision(t1,t2);if(!_TD)goto _TL218;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
_T20.f1=_tag_fat("integral size mismatch; ",sizeof(char),25U);_TE=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_TE;{struct Cyc_Warn_Typ_Warn_Warg_struct _T21;_T21.tag=2;_T21.f1=t1;_TF=_T21;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T21=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;_T22.f1=_tag_fat(" -> ",sizeof(char),5U);_T10=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T23;_T23.tag=2;_T23.f1=t2;_T11=_T23;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T23=_T11;{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=_tag_fat(" conversion supplied",sizeof(char),21U);_T12=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T12;void*_T25[5];_T25[0]=& _T20;_T25[1]=& _T21;_T25[2]=& _T22;_T25[3]=& _T23;_T25[4]=& _T24;_T13=loc;_T14=_tag_fat(_T25,sizeof(void*),5);Cyc_Warn_warn2(_T13,_T14);}}}}}goto _TL219;_TL218: _TL219: goto _TL217;
# 807
_TL216:{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;_T20.f1=_tag_fat("implicit cast from ",sizeof(char),20U);_T15=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T15;{struct Cyc_Warn_Typ_Warn_Warg_struct _T21;_T21.tag=2;_T21.f1=t1;_T16=_T21;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T21=_T16;{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;_T22.f1=_tag_fat(" to ",sizeof(char),5U);_T17=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T17;{struct Cyc_Warn_Typ_Warn_Warg_struct _T23;_T23.tag=2;_T23.f1=t2;_T18=_T23;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T23=_T18;void*_T24[4];_T24[0]=& _T20;_T24[1]=& _T21;_T24[2]=& _T22;_T24[3]=& _T23;_T19=loc;_T1A=_tag_fat(_T24,sizeof(void*),4);Cyc_Warn_warn2(_T19,_T1A);}}}}_TL217: goto _TL214;_TL213: _TL214: _T1B=s;
# 809
return _T1B;_TL211:
# 811
 return 0U;}}
# 814
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible(unsigned loc,void*t1,void*t2){enum Cyc_Absyn_Coercion _T0;_T0=
Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_default_constraint,1);return _T0;}
# 818
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_warn(unsigned loc,void*t1,void*t2){enum Cyc_Absyn_Coercion _T0;_T0=
Cyc_Subtype_internal_coercible_c(loc,1,t1,t2,Cyc_Subtype_gen_default_constraint,1);return _T0;}
# 822
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned loc,void*t1,void*t2){enum Cyc_Absyn_Coercion _T0;_T0=
Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_equality_constraint,0);return _T0;}
