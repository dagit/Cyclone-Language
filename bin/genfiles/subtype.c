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
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 166
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 170
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};
# 554 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};
# 923 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 938
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 944
void*Cyc_Absyn_compress(void*);
# 948
int Cyc_Absyn_type2bool(int,void*);
# 979
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 1014
void*Cyc_Absyn_bounds_one (void);
# 1017
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
# 32 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 36
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_strict_arithmetic_type(void*);
# 42
int Cyc_Tcutil_is_pointer_type(void*);
# 55
int Cyc_Tcutil_is_bits_only_type(void*);
# 70
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 129 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 135
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 147
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 254
int Cyc_Tcutil_force_type2bool(int,void*);
# 277
int Cyc_Tcutil_cmp_effect_constraints(struct Cyc_List_List*,struct Cyc_List_List*);
# 280
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
void*Cyc_Tcutil_get_pointer_bounds(void*);
# 288
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
# 313
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{enum Cyc_Absyn_Coercion f0;enum Cyc_Absyn_Coercion f1;};
# 40 "subtype.cyc"
static enum Cyc_Absyn_Coercion Cyc_Subtype_join_coercion(enum Cyc_Absyn_Coercion c1,enum Cyc_Absyn_Coercion c2){
if((int)c1==(int)c2)return c1;{
struct _tuple11 _Tmp0=({struct _tuple11 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});enum Cyc_Absyn_Coercion _Tmp1;if(_Tmp0.f0==Cyc_Absyn_Unknown_coercion)
goto _LL4;else{if(_Tmp0.f1==Cyc_Absyn_Unknown_coercion){_LL4:
 return 0U;}else{if(_Tmp0.f0==Cyc_Absyn_No_coercion){_Tmp1=_Tmp0.f1;{enum Cyc_Absyn_Coercion o=_Tmp1;
_Tmp1=o;goto _LL8;}}else{if(_Tmp0.f1==Cyc_Absyn_No_coercion){_Tmp1=_Tmp0.f0;_LL8: {enum Cyc_Absyn_Coercion o=_Tmp1;
return o;}}else{switch((int)_Tmp0.f0){case Cyc_Absyn_Null_to_NonNull: if(_Tmp0.f1==Cyc_Absyn_Other_coercion)
goto _LLC;else{goto _LLD;}case Cyc_Absyn_Other_coercion: if(_Tmp0.f1==Cyc_Absyn_Null_to_NonNull){_LLC:
 return 2U;}else{goto _LLD;}default: _LLD:
 return 0U;}}}}};}}
# 56
static int Cyc_Subtype_unify_cvar(struct Cyc_List_List**env,void*t1,void*t2){
if(Cyc_Tcutil_is_cvar_type(t1)|| Cyc_Tcutil_is_cvar_type(t2)){
void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
if((unsigned)cv1 &&(unsigned)cv2){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_BansheeIf_equality_constraint(cv1,cv2);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*_check_null(env);_Tmp1;});*env=_Tmp0;});
return 1;}}
# 65
return 0;}struct _tuple12{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;};
# 69
static struct _tuple12 Cyc_Subtype_force_equivalence(void*t1,void*t2){
struct Cyc_List_List*retc=0;
if(({int(*_Tmp0)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**)=(int(*)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**))Cyc_Unify_unify_c;_Tmp0;})(t1,t2,Cyc_Subtype_unify_cvar,& retc)){
struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=retc;return _Tmp0;}{
# 74
struct _tuple12 _Tmp0;_Tmp0.f0=0U,_Tmp0.f1=0;return _Tmp0;}}
# 77
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*,void*,void*);
# 80
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*,int,void*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;int f2;};struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;};
# 82
static struct Cyc_List_List*Cyc_Subtype_flatten_type_f(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*x){
# 85
struct _tuple13 _Tmp0=*env;int _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*inst=_Tmp3;struct _RegionHandle*r=_Tmp2;int flatten=_Tmp1;
void*t=inst==0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);
struct Cyc_List_List*ts=Cyc_Subtype_flatten_type(r,flatten,t);
if(Cyc_List_length(ts)==1){
struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp5=({struct _tuple14*_Tmp6=_region_malloc(r,0U,sizeof(struct _tuple14));_Tmp6->f0=x->tq,_Tmp6->f1=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;return _Tmp4;}
return ts;}}
# 93
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 95
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_Tmp0;void*_Tmp1;switch(*((int*)t1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)){case 0:
 return 0;case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.val;_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 101
if((((int)ad->kind==1 || ad->impl==0)|| ad->impl->exist_vars!=0)|| ad->impl->effconstr!=0){
# 103
struct Cyc_List_List*_Tmp2=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp3=({struct _tuple14*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple14));({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);_Tmp4->f0=_Tmp5;}),_Tmp4->f1=t1;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;return _Tmp2;}{
struct Cyc_List_List*inst=Cyc_List_rzip(r,r,ad->tvs,ts);
struct _tuple13 env=({struct _tuple13 _Tmp2;_Tmp2.f0=inst,_Tmp2.f1=r,_Tmp2.f2=flatten;_Tmp2;});
struct Cyc_List_List*_Tmp2=_check_null(ad->impl)->fields;void*_Tmp3;void*_Tmp4;if(_Tmp2==0)
return 0;else{_Tmp4=(struct Cyc_Absyn_Aggrfield*)_Tmp2->hd;_Tmp3=_Tmp2->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp4;struct Cyc_List_List*tl=_Tmp3;
# 109
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(& env,hd);
env.f2=0;{
struct Cyc_List_List*tl2=({struct Cyc_List_List*(*_Tmp5)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp5;})(r,Cyc_Subtype_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}}else{goto _LL7;}default: goto _LL7;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f1==Cyc_Absyn_StructA){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f3;{struct Cyc_List_List*fs=_Tmp1;
# 116
struct _tuple13 env=({struct _tuple13 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=r,_Tmp2.f2=flatten;_Tmp2;});
void*_Tmp2;void*_Tmp3;if(fs==0)
return 0;else{_Tmp3=(struct Cyc_Absyn_Aggrfield*)fs->hd;_Tmp2=fs->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp3;struct Cyc_List_List*tl=_Tmp2;
# 120
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(& env,hd);
env.f2=0;{
struct Cyc_List_List*tl2=({struct Cyc_List_List*(*_Tmp4)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp4;})(r,Cyc_Subtype_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}else{goto _LL7;}default: _LL7:
# 126
 goto _LL0;}_LL0:;}}{
# 129
struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp1=({struct _tuple14*_Tmp2=_region_malloc(r,0U,sizeof(struct _tuple14));({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);_Tmp2->f0=_Tmp3;}),_Tmp2->f1=t1;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;return _Tmp0;}}struct _tuple15{void*f0;void*f1;};
# 135
int Cyc_Subtype_check_aqual_bounds(struct Cyc_List_List*aquals_bnd,void*aq,void*bnd){
# 137
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(bnd);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;enum Cyc_Absyn_AliasQualVal _Tmp3;if(*((int*)_Tmp0.f0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 16: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp2;
# 139
return(int)v_sup==(int)v_sub ||(int)v_sup==3;}}else{goto _LL7;}}else{goto _LL7;}case 17: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_List_List*tv_sub=_Tmp1;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 141
return Cyc_Subtype_check_aqual_bounds(aquals_bnd,(void*)_check_null(_check_null(tv_sub)->tl)->hd,bnd);}}else{goto _LL7;}}else{goto _LL7;}case 15: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 143
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*aq_bound=Cyc_List_assoc_cmp(Cyc_Tcutil_typecmp,aquals_bnd,aq);
int _Tmp6=Cyc_Subtype_check_aqual_bounds(aquals_bnd,aq_bound,bnd);_npop_handler(0);return _Tmp6;}
# 144
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found)
# 148
return 0;else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}};}}}}else{goto _LL7;}}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 151
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("check_aqual_bounds expects a constant bound; got ",sizeof(char),50U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aq;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(",",sizeof(char),2U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)bnd;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),4));});};}
# 165 "subtype.cyc"
static int Cyc_Subtype_alias_qual_subtype(void*aq_sub,void*aq_sup,int mode){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq_sub);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(aq_sup);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_AliasQualVal _Tmp5;enum Cyc_Absyn_AliasQualVal _Tmp6;if(*((int*)_Tmp0.f0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 16: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp5=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp5;
# 168
if(mode==1)
return(int)v_sub==(int)v_sup;
if(mode==2)
return(int)v_sub==(int)v_sup &&(int)v_sub!=3;
return((int)v_sub==(int)v_sup ||(int)v_sup==3)||
(int)v_sup==0 &&(((int)v_sub==1 ||(int)v_sub==2)||(int)v_sub==3);}}else{goto _LL11;}}else{goto _LL11;}case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)){case 2: if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_Absyn_Tvar*tv2=_Tmp3;
# 178
return tv2->identity==tv1->identity;}case 1: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 195
void*_Tmp7=(void*)_check_null(bnd1)->hd;return Cyc_Subtype_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd,2);}default: goto _LL11;}else{goto _LL11;}case 16: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 188
return Cyc_Subtype_alias_qual_subtype((void*)_check_null(bnd)->hd,aq_sup,mode);}default: goto _LL11;}else{goto _LL11;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f2)->tl==0){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==17){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2)->tl==0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f2->hd;_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f2->hd;{void*tv1=_Tmp4;void*tv2=_Tmp3;
# 182
return Cyc_Tcutil_typecmp(tv1,tv2)==0;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}}else{goto _LL11;}case 1: if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 1: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 185
void*_Tmp7=(void*)_check_null(bnd1)->hd;return Cyc_Subtype_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd,1);}case 2: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Absyn_Tvar*tv2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 199
void*_Tmp7=(void*)_check_null(bnd1)->hd;return Cyc_Subtype_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd,2);}default: goto _LL11;}else{goto _LL11;}case 16: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_Core_Opt*k=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 191
return Cyc_Subtype_alias_qual_subtype((void*)_check_null(bnd)->hd,aq_sup,mode);}default: goto _LL11;}else{goto _LL11;}default: goto _LL11;}else{goto _LL11;}default: goto _LL11;}else{_LL11:
# 204
 return 0;};}
# 208
static int Cyc_Subtype_isomorphic(void*t1,void*t2){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});enum Cyc_Absyn_Size_of _Tmp1;enum Cyc_Absyn_Size_of _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==1){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){_Tmp2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f2;_Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f2;{enum Cyc_Absyn_Size_of b1=_Tmp2;enum Cyc_Absyn_Size_of b2=_Tmp1;
# 211
return((int)b1==(int)b2 ||(int)b1==2 &&(int)b2==3)||
(int)b1==3 &&(int)b2==2;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}struct _tuple16{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 221
static struct _tuple12 Cyc_Subtype_subtype_impl(struct Cyc_List_List*assume,void*t1,void*t2){
# 223
struct Cyc_List_List*retc=0;
if(Cyc_Unify_unify(t1,t2)){struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0;return _Tmp0;}
{struct Cyc_List_List*a=assume;for(0;a!=0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f0)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f1)){
struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0;return _Tmp0;}}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp12=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.autoreleased;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t_a=_Tmp12;struct Cyc_Absyn_Tqual q_a=_Tmp11;void*eff_a=_Tmp10;void*null_a=_TmpF;void*b_a=_TmpE;void*zt_a=_TmpD;void*rel_a=_TmpC;void*aq_a=_TmpB;void*t_b=_TmpA;struct Cyc_Absyn_Tqual q_b=_Tmp9;void*eff_b=_Tmp8;void*null_b=_Tmp7;void*b_b=_Tmp6;void*zt_b=_Tmp5;void*rel_b=_Tmp4;void*aq_b=_Tmp3;
# 235
enum Cyc_Absyn_Coercion coerce=1U;
# 237
if(q_a.real_const && !q_b.real_const){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 240
if(!Cyc_Unify_unify(null_a,null_b)){
if(Cyc_Absyn_type2bool(0,null_a)&& !Cyc_Absyn_type2bool(0,null_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}
# 246
if((!Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 250
if((Cyc_Unify_unify(rel_a,rel_b)&& !
Cyc_Absyn_type2bool(0,rel_a))&& Cyc_Absyn_type2bool(0,rel_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 254
if((!Cyc_Unify_unify(eff_a,eff_b)&& !((int)Cyc_Subtype_subtype_impl(assume,eff_a,eff_b).f0))&& !
Cyc_Tcutil_subset_effect(0,eff_a,eff_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 258
if((!Cyc_Unify_unify(aq_a,aq_b)&& !Cyc_Subtype_alias_qual_subtype(aq_a,aq_b,3))&& !((int)
Cyc_Subtype_subtype_impl(assume,aq_a,aq_b).f0)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 263
if(!Cyc_Unify_unify(b_a,b_b)){
if(!Cyc_Tcutil_is_cvar_type(b_a)&& !Cyc_Tcutil_is_cvar_type(b_b)){
struct Cyc_Absyn_Exp*e1=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_a);});
struct Cyc_Absyn_Exp*e2=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_b);});
if(e1!=e2){
if((e1==0 || e2==0)|| !Cyc_Evexp_lte_const_exp(e2,e1)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}else{
# 275
void*cva=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_a);
void*cvb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);
if((unsigned)cva &&(unsigned)cvb)
retc=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp14=Cyc_BansheeIf_equality_constraint(cva,cvb);_Tmp13->hd=_Tmp14;}),_Tmp13->tl=retc;_Tmp13;});else{
# 281
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}
# 286
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b))){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}
# 291
if(Cyc_Subtype_isomorphic(t_a,t_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=coerce,_Tmp13.f1=retc;return _Tmp13;}
# 294
{
struct _tuple12 _Tmp13=Cyc_Subtype_force_equivalence(t_a,t_b);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s==1){
retc=Cyc_List_imp_append(retc,c);{
struct _tuple12 _Tmp16;_Tmp16.f0=1U,_Tmp16.f1=retc;return _Tmp16;}}}}{
# 302
int deep_subtype=!Cyc_Tcutil_force_type2bool(0,zt_b);
if(deep_subtype && !({void*_Tmp13=b_b;Cyc_Unify_unify(_Tmp13,Cyc_Absyn_bounds_one());})){
# 305
void*cbb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);
if((unsigned)cbb)
retc=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp14=({void*_Tmp15=cbb;Cyc_BansheeIf_equality_constraint(_Tmp15,Cyc_Absyn_thinconst());});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=retc;_Tmp13;});else{
# 310
deep_subtype=0;}}
# 312
if(deep_subtype){
struct _tuple12 _Tmp13=({struct Cyc_List_List*_Tmp14=({struct Cyc_List_List*_Tmp15=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp16=({struct _tuple15*_Tmp17=_cycalloc(sizeof(struct _tuple15));_Tmp17->f0=t1,_Tmp17->f1=t2;_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=assume;_Tmp15;});void*_Tmp15=t_a;Cyc_Subtype_ptrsubtype(_Tmp14,_Tmp15,t_b);});void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s==1){
struct _tuple12 _Tmp16;_Tmp16.f0=1U,({struct Cyc_List_List*_Tmp17=Cyc_List_imp_append(retc,c);_Tmp16.f1=_Tmp17;});return _Tmp16;}}}{
# 318
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==23){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.tag==2){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.tag==2){_Tmp12=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f0;_Tmp10=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f1;_TmpF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpE=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.val;_TmpD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_Tmp12;struct Cyc_Absyn_Datatypefield*df=_Tmp10;struct Cyc_List_List*ts1=_TmpF;struct Cyc_Absyn_Datatypedecl*dd2=_TmpE;struct Cyc_List_List*ts2=_TmpD;
# 324
if(dd1!=dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!=0){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 326
if(({int _Tmp13=Cyc_List_length(ts1);_Tmp13!=Cyc_List_length(ts2);})){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
for(1;ts1!=0;(ts1=ts1->tl,ts2=ts2->tl)){
struct _tuple12 _Tmp13=Cyc_Subtype_force_equivalence((void*)ts1->hd,(void*)_check_null(ts2)->hd);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s!=0)
retc=Cyc_List_imp_append(retc,c);}}{
# 333
struct _tuple12 _Tmp13;_Tmp13.f0=3U,_Tmp13.f1=retc;return _Tmp13;}}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_FnInfo f1=_Tmp2;struct Cyc_Absyn_FnInfo f2=_Tmp1;
# 335
if(f1.tvars!=0 || f2.tvars!=0){
struct Cyc_List_List*tvs1=f1.tvars;
struct Cyc_List_List*tvs2=f2.tvars;
if(({int _Tmp13=Cyc_List_length(tvs1);_Tmp13!=Cyc_List_length(tvs2);})){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}{
struct Cyc_List_List*inst=0;
while(tvs1!=0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)_check_null(tvs2)->hd)->kind)){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
inst=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp14=({struct _tuple16*_Tmp15=_cycalloc(sizeof(struct _tuple16));_Tmp15->f0=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_Tmp16=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_Tmp15->f1=_Tmp16;});_Tmp15;});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=inst;_Tmp13;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;
# 341
1U;}
# 346
if(tvs2!=0){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
if(inst!=0){
f1.tvars=0;
f2.tvars=0;{
void*newftype2=({struct _RegionHandle*_Tmp13=Cyc_Core_heap_region;struct Cyc_List_List*_Tmp14=inst;Cyc_Tcutil_rsubstitute(_Tmp13,_Tmp14,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp15->tag=6,_Tmp15->f1=f2;_Tmp15;}));});
struct Cyc_List_List*_Tmp13=assume;void*_Tmp14=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp15->tag=6,_Tmp15->f1=f1;_Tmp15;});return Cyc_Subtype_subtype_impl(_Tmp13,_Tmp14,newftype2);}}}}{
# 356
struct _tuple12 _Tmp13=Cyc_Subtype_subtype_impl(assume,f1.ret_type,f2.ret_type);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if(!((int)s)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
retc=Cyc_List_imp_append(retc,c);{
struct Cyc_List_List*args1=f1.args;
struct Cyc_List_List*args2=f2.args;
# 363
if(({int _Tmp16=Cyc_List_length(args1);_Tmp16!=Cyc_List_length(args2);})){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 365
for(1;args1!=0;(args1=args1->tl,args2=args2->tl)){
struct _tuple8 _Tmp16=*((struct _tuple8*)args1->hd);void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;_Tmp18=_Tmp16.f1;_Tmp17=_Tmp16.f2;{struct Cyc_Absyn_Tqual tq1=_Tmp18;void*t1=_Tmp17;
struct _tuple8 _Tmp19=*((struct _tuple8*)_check_null(args2)->hd);void*_Tmp1A;struct Cyc_Absyn_Tqual _Tmp1B;_Tmp1B=_Tmp19.f1;_Tmp1A=_Tmp19.f2;{struct Cyc_Absyn_Tqual tq2=_Tmp1B;void*t2=_Tmp1A;
# 369
if(tq2.real_const && !tq1.real_const){
struct _tuple12 _Tmp1C;_Tmp1C.f0=0U,_Tmp1C.f1=0;return _Tmp1C;}{
# 373
struct _tuple12 _Tmp1C=Cyc_Subtype_subtype_impl(0,t2,t1);void*_Tmp1D;enum Cyc_Absyn_Coercion _Tmp1E;_Tmp1E=_Tmp1C.f0;_Tmp1D=_Tmp1C.f1;{enum Cyc_Absyn_Coercion s=_Tmp1E;struct Cyc_List_List*c=_Tmp1D;
if((int)s==0){
struct _tuple12 _Tmp1F;_Tmp1F.f0=0U,_Tmp1F.f1=0;return _Tmp1F;}
retc=Cyc_List_imp_append(retc,c);}}}}}
# 379
if(f1.c_varargs!=f2.c_varargs){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
if(f1.cyc_varargs!=0 && f2.cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo v1=*f1.cyc_varargs;
struct Cyc_Absyn_VarargInfo v2=*f2.cyc_varargs;
# 384
if(v2.tq.real_const && !v1.tq.real_const){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}{
struct _tuple12 _Tmp16=Cyc_Subtype_subtype_impl(assume,v2.type,v1.type);void*_Tmp17;enum Cyc_Absyn_Coercion _Tmp18;_Tmp18=_Tmp16.f0;_Tmp17=_Tmp16.f1;{enum Cyc_Absyn_Coercion s=_Tmp18;struct Cyc_List_List*c=_Tmp17;
if((int)s==0){
struct _tuple12 _Tmp19;_Tmp19.f0=0U,_Tmp19.f1=0;return _Tmp19;}
retc=Cyc_List_imp_append(retc,c);}}}else{
if(f1.cyc_varargs!=0 || f2.cyc_varargs!=0){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}
# 392
if(!({void*_Tmp16=_check_null(f1.effect);Cyc_Tcutil_subset_effect(0,_Tmp16,_check_null(f2.effect));})){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 394
if(Cyc_Tcutil_cmp_effect_constraints(f1.effconstr,f2.effconstr)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 396
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}{
# 398
struct Cyc_List_List*terms=0;
# 400
{struct Cyc_List_List*vds=f1.arg_vardecls;for(0;vds!=0;vds=vds->tl){
terms=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp17=Cyc_AssnDef_fresh_var((struct Cyc_Absyn_Vardecl*)vds->hd,(struct Cyc_Absyn_Vardecl*)vds->hd==0?0:((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_Tmp16->hd=_Tmp17;}),_Tmp16->tl=terms;_Tmp16;});}}
# 403
terms=Cyc_List_imp_rev(terms);
# 405
terms=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp17=Cyc_AssnDef_fresh_var(0,0);_Tmp16->hd=_Tmp17;}),_Tmp16->tl=terms;_Tmp16;});
# 408
if(f1.checks_assn!=0){
void*chk1=Cyc_AssnDef_existassnfn2assn(f1.checks_assn,terms);
void*chk2=(unsigned)f2.checks_assn?Cyc_AssnDef_existassnfn2assn(f2.checks_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(chk2,chk1)&& !
Cyc_PrattProver_constraint_prove(chk2,chk1)){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}
# 418
if(f1.requires_assn!=0){
void*req1=Cyc_AssnDef_existassnfn2assn(f1.requires_assn,terms);
void*req2=(unsigned)f2.requires_assn?Cyc_AssnDef_existassnfn2assn(f2.requires_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(req2,req1)&& !
Cyc_PrattProver_constraint_prove(req2,req1)){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}
# 428
if(f2.ensures_assn!=0){
void*ret_value=Cyc_AssnDef_fresh_var(f1.return_value,_check_null(f1.return_value)->type);
# 431
struct Cyc_List_List*_Tmp16;_Tmp16=_cycalloc(sizeof(struct Cyc_List_List)),({void*_Tmp17=Cyc_AssnDef_fresh_var(0,0);_Tmp16->hd=_Tmp17;}),_Tmp16->tl=terms;{struct Cyc_List_List*terms=_Tmp16;
struct Cyc_List_List*_Tmp17;_Tmp17=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp17->hd=ret_value,_Tmp17->tl=terms;{struct Cyc_List_List*terms=_Tmp17;
void*ens2=Cyc_AssnDef_existassnfn2assn(f2.ensures_assn,terms);
void*ens1=(unsigned)f1.ensures_assn?Cyc_AssnDef_existassnfn2assn(f1.ensures_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(ens1,ens2)&& !
Cyc_PrattProver_constraint_prove(ens1,ens2)){
struct _tuple12 _Tmp18;_Tmp18.f0=0U,_Tmp18.f1=0;return _Tmp18;}}}}
# 440
if(f2.throws_assn!=0){
void*thrws2=Cyc_AssnDef_existassnfn2assn(f2.throws_assn,terms);
void*thrws1=(unsigned)f1.throws_assn?Cyc_AssnDef_existassnfn2assn(f1.throws_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(thrws1,thrws2)&& !
Cyc_PrattProver_constraint_prove(thrws1,thrws2)){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}{
# 448
struct _tuple12 _Tmp16;_Tmp16.f0=1U,_Tmp16.f1=retc;return _Tmp16;}}}}}}}else{goto _LL7;}default: _LL7:
# 450
 return Cyc_Subtype_force_equivalence(t1,t2);};}}
# 454
int Cyc_Subtype_subtype(unsigned loc,struct Cyc_List_List*assume,void*t1,void*t2){
# 456
struct _tuple12 _Tmp0=Cyc_Subtype_subtype_impl(assume,t1,t2);void*_Tmp1;enum Cyc_Absyn_Coercion _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{enum Cyc_Absyn_Coercion r=_Tmp2;struct Cyc_List_List*c=_Tmp1;
if((int)r==1)
return({int(*_Tmp3)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_forall_c;_Tmp3;})(Cyc_BansheeIf_add_constraint,loc,c);
# 460
return 0;}}
# 470 "subtype.cyc"
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*assume,void*t1,void*t2){
# 472
struct Cyc_List_List*tqs1=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t2);
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;
for(1;tqs2!=0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1==0){struct _tuple12 _Tmp0;_Tmp0.f0=0U,_Tmp0.f1=0;return _Tmp0;}{
struct _tuple14*_Tmp0=(struct _tuple14*)tqs1->hd;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Tqual tq1=_Tmp2;void*t1a=_Tmp1;
struct _tuple14*_Tmp3=(struct _tuple14*)tqs2->hd;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp3->f0;_Tmp4=_Tmp3->f1;{struct Cyc_Absyn_Tqual tq2=_Tmp5;void*t2a=_Tmp4;
if(tq1.real_const && !tq2.real_const){struct _tuple12 _Tmp6;_Tmp6.f0=0U,_Tmp6.f1=0;return _Tmp6;}
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Subtype_isomorphic(t1a,t2a))
continue;
if(tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a))){
struct _tuple12 _Tmp6=Cyc_Subtype_subtype_impl(assume,t1a,t2a);void*_Tmp7;enum Cyc_Absyn_Coercion _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{enum Cyc_Absyn_Coercion s=_Tmp8;struct Cyc_List_List*c=_Tmp7;
coerce=Cyc_Subtype_join_coercion(s,coerce);
if((int)coerce!=0){
retc=Cyc_List_imp_append(retc,c);
continue;}}}else{
# 492
struct _tuple12 _Tmp6=Cyc_Subtype_force_equivalence(t1a,t2a);void*_Tmp7;enum Cyc_Absyn_Coercion _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{enum Cyc_Absyn_Coercion s=_Tmp8;struct Cyc_List_List*c=_Tmp7;
coerce=Cyc_Subtype_join_coercion(s,coerce);
if((int)coerce!=0){
retc=Cyc_List_imp_append(retc,c);
continue;}}}{
# 499
struct _tuple12 _Tmp6;_Tmp6.f0=0U,_Tmp6.f1=0;return _Tmp6;}}}}}{
# 501
struct _tuple12 _Tmp0;_Tmp0.f0=coerce,_Tmp0.f1=retc;return _Tmp0;}}
# 508
static int Cyc_Subtype_gen_default_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){
if(k==Cyc_Absyn_PtrBndKind){
# 511
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(_t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(_t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;{void*t1=_Tmp4;void*z1=_Tmp3;void*t2=_Tmp2;void*z2=_Tmp1;
# 513
t1=Cyc_Absyn_compress(t1);t2=Cyc_Absyn_compress(t2);
if(!Cyc_Tcutil_is_cvar_type(t1)&& !Cyc_Tcutil_is_cvar_type(t2))
return 0;{
void*ct1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*ct2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
# 524
void*c=0;
if(((unsigned)ct1 &&(unsigned)ct2)&&({void*_Tmp5=ct2;_Tmp5!=Cyc_Absyn_thinconst();})){
# 530
if(({void*_Tmp5=ct1;_Tmp5!=Cyc_Absyn_thinconst();})|| !Cyc_Absyn_type2bool(0,z1))
c=({void*_Tmp5=({void*_Tmp6=ct2;Cyc_BansheeIf_cmpeq_constraint(_Tmp6,Cyc_Absyn_fatconst());});Cyc_BansheeIf_implication_constraint(_Tmp5,
Cyc_BansheeIf_equality_constraint(ct1,ct2));});}else{
# 545
if(!((unsigned)ct2))
c=({void*_Tmp5=_check_null(ct1);Cyc_BansheeIf_equality_constraint(_Tmp5,Cyc_Absyn_fatconst());});}
# 549
if((unsigned)c)
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=c,_Tmp6->tl=*retc;_Tmp6;});*retc=_Tmp5;});
# 553
return 1;}}}else{goto _LL8;}}else{_LL8:
# 555
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Non pointer type in gen constraint",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}else{
# 558
return 0;};}
# 562
static int Cyc_Subtype_gen_equality_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){
# 565
if(k==Cyc_Absyn_PtrBndKind){
# 567
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=_t1,_Tmp1.f1=_t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;{void*t1=_Tmp2;void*t2=_Tmp1;
# 569
void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
if((unsigned)cv1 &&(unsigned)cv2){
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp5=Cyc_BansheeIf_equality_constraint(cv1,cv2);_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*retc;_Tmp4;});*retc=_Tmp3;});
return 1;}
# 575
return 0;}}else{goto _LL8;}}else{_LL8:
# 577
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Non pointer type in gen constraint",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}else{
# 580
return 0;};}struct _tuple17{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;int f2;};
# 592 "subtype.cyc"
static struct _tuple17 Cyc_Subtype_constraint_subtype(void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_coercion){
# 598
if(Cyc_Unify_unify(t1,t2)){
struct _tuple17 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0,_Tmp0.f2=1;return _Tmp0;}
# 601
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(*((int*)t2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)){case 0:  {
struct _tuple17 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0,_Tmp0.f2=1;return _Tmp0;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2){case Cyc_Absyn_Int_sz:
# 606
 goto _LL6;case Cyc_Absyn_Long_sz: _LL6:
# 608
 if((int)Cyc_Tcutil_type_kind(t1)->kind==2){struct _tuple17 _Tmp0;_Tmp0.f0=3U,_Tmp0.f1=0,_Tmp0.f2=0;return _Tmp0;}
if(Cyc_Tcutil_is_pointer_type(t1)){
void*cv1=_check_null(Cyc_Tcutil_get_pointer_bounds(t1));
if(Cyc_Tcutil_is_cvar_type(cv1)){
struct _tuple17 _Tmp0;_Tmp0.f0=3U,({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=({void*_Tmp4=_check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(cv1));Cyc_BansheeIf_equality_constraint(_Tmp4,Cyc_Absyn_thinconst());});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});_Tmp0.f1=_Tmp1;}),_Tmp0.f2=0;return _Tmp0;}}
# 615
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;{
# 619
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;struct Cyc_Absyn_Tqual _TmpF;void*_Tmp10;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.autoreleased;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t1a=_Tmp10;struct Cyc_Absyn_Tqual tqual1a=_TmpF;void*eff1=_TmpE;void*null1a=_TmpD;void*b1=_TmpC;void*zt1=_TmpB;void*rel1=_TmpA;void*aq1=_Tmp9;void*t2a=_Tmp8;struct Cyc_Absyn_Tqual tqual2a=_Tmp7;void*eff2=_Tmp6;void*null2a=_Tmp5;void*b2=_Tmp4;void*zt2=_Tmp3;void*rel2=_Tmp2;void*aq2=_Tmp1;
# 624
if((!Cyc_Unify_unify(null1a,null2a)&& Cyc_Tcutil_force_type2bool(0,null1a))&& !Cyc_Tcutil_force_type2bool(0,null2a)){
if(allow_coercion)
coerce=2U;else{
# 628
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}}
# 630
if(!tqual2a.real_const && tqual1a.real_const){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 633
if(!Cyc_Unify_unify(eff1,eff2)&& !
Cyc_Tcutil_subset_effect(1,eff1,eff2)){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 638
if((!Cyc_Unify_unify(aq1,aq2)&& !Cyc_Subtype_alias_qual_subtype(aq1,aq2,3))&& !
genconstr(& retc,7U,t1,t2)){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 642
if(!Cyc_Unify_unify(rel1,rel2)&& !Cyc_Absyn_type2bool(0,rel2)){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 650
if((!Cyc_Unify_unify(zt1,zt2)&& Cyc_Absyn_type2bool(1,zt2))&& !genconstr(& retc,5U,t1,t2)){
# 661 "subtype.cyc"
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}{
# 663
int silent=1;
if(!Cyc_Unify_unify(b1,b2)){
if(!Cyc_Tcutil_is_cvar_type(b1)&& !Cyc_Tcutil_is_cvar_type(b2)){
struct Cyc_Absyn_Exp*e1=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp11,b1);});
struct Cyc_Absyn_Exp*e2=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp11,b2);});
if((unsigned)e1 &&(unsigned)e2){
if(!Cyc_Evexp_lte_const_exp(e2,e1)){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
silent=1;}else{
# 673
if((unsigned)e2)
silent=0;}}else{
# 678
if(!genconstr(& retc,6U,t1,t2)){
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}}
# 682
coerce=Cyc_Subtype_join_coercion(coerce,3U);}
# 684
if(Cyc_Subtype_isomorphic(t1a,t2a)|| Cyc_Unify_unify(t1a,t2a)){
struct _tuple17 _Tmp11;_Tmp11.f0=coerce,_Tmp11.f1=retc,_Tmp11.f2=silent;return _Tmp11;}else{
# 687
struct _tuple12 _Tmp11=Cyc_Subtype_force_equivalence(t1a,t2a);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
if((int)s!=0){
struct _tuple17 _Tmp14;({enum Cyc_Absyn_Coercion _Tmp15=Cyc_Subtype_join_coercion(coerce,s);_Tmp14.f0=_Tmp15;}),({struct Cyc_List_List*_Tmp15=Cyc_List_imp_append(retc,c);_Tmp14.f1=_Tmp15;}),_Tmp14.f2=silent;return _Tmp14;}}}
# 692
if(((Cyc_Tcutil_is_bits_only_type(t1a)&& Cyc_Tcutil_is_char_type(t2a))&& !
Cyc_Tcutil_force_type2bool(0,zt2))&&(
tqual2a.real_const || !tqual1a.real_const)){
struct _tuple17 _Tmp11;_Tmp11.f0=coerce,_Tmp11.f1=retc,_Tmp11.f2=silent;return _Tmp11;}{
# 697
int deep_subtype=!Cyc_Tcutil_force_type2bool(0,zt2);
# 699
if(deep_subtype && !({void*_Tmp11=b2;Cyc_Unify_unify(_Tmp11,Cyc_Absyn_bounds_one());})){
void*cb2=Cyc_Tcutil_ptrbnd_cvar_equivalent(b2);
if((unsigned)cb2)
retc=({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp12=({void*_Tmp13=cb2;Cyc_BansheeIf_equality_constraint(_Tmp13,Cyc_Absyn_thinconst());});_Tmp11->hd=_Tmp12;}),_Tmp11->tl=retc;_Tmp11;});else{
# 705
deep_subtype=0;}}
# 708
if(deep_subtype){
struct _tuple12 _Tmp11=Cyc_Subtype_ptrsubtype(0,t1a,t2a);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
if((int)s!=0){
struct _tuple17 _Tmp14;({enum Cyc_Absyn_Coercion _Tmp15=Cyc_Subtype_join_coercion(coerce,3U);_Tmp14.f0=_Tmp15;}),({struct Cyc_List_List*_Tmp15=Cyc_List_imp_append(retc,c);_Tmp14.f1=_Tmp15;}),_Tmp14.f2=silent;return _Tmp14;}}}{
# 714
struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}}}}}else{goto _LL16;}case 5: if(*((int*)_Tmp0.f1)==5){_Tmp10=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.tq;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.num_elts;_TmpD=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.zero_term;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.tq;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.num_elts;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.zero_term;{void*t1a=_Tmp10;struct Cyc_Absyn_Tqual tq1a=_TmpF;struct Cyc_Absyn_Exp*e1=_TmpE;void*zt1=_TmpD;void*t2a=_TmpC;struct Cyc_Absyn_Tqual tq2a=_Tmp7;struct Cyc_Absyn_Exp*e2=_TmpB;void*zt2=_TmpA;
# 716
if(!Cyc_Unify_unify(zt1,zt2)){struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
if(e1==0 || e2==0){struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
if(!Cyc_Evexp_same_uint_const_exp(e1,e2)){struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}{
struct _tuple12 _Tmp11=Cyc_Subtype_force_equivalence(t1a,t2a);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
if((int)s==0){
struct _tuple17 _Tmp14;_Tmp14.f0=0U,_Tmp14.f1=0,_Tmp14.f2=0;return _Tmp14;}
if(!tq1a.real_const || tq2a.real_const){struct _tuple17 _Tmp14;_Tmp14.f0=1U,_Tmp14.f1=c,_Tmp14.f2=1;return _Tmp14;}else{struct _tuple17 _Tmp14;_Tmp14.f0=0U,_Tmp14.f1=0,_Tmp14.f2=0;return _Tmp14;}}}}}else{goto _LL16;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 19: _Tmp10=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed1=_Tmp10;
# 726
{void*_Tmp11;if(*((int*)t2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)==19){_Tmp11=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed2=_Tmp11;
# 728
if((_check_null(ed1)->fields!=0 && _check_null(ed2)->fields!=0)&&({
int _Tmp12=Cyc_List_length((struct Cyc_List_List*)ed1->fields->v);_Tmp12 >= Cyc_List_length((struct Cyc_List_List*)_check_null(ed2->fields)->v);})){
struct _tuple17 _Tmp12;_Tmp12.f0=1U,_Tmp12.f1=0,_Tmp12.f2=1;return _Tmp12;}
goto _LL21;}}else{goto _LL24;}}else{_LL24:
 goto _LL21;}_LL21:;}
# 734
goto _LL11;}case 1: _LL11:
 goto _LL13;case 2: _LL13:
# 737
 if(Cyc_Tcutil_is_strict_arithmetic_type(t2)){struct _tuple17 _Tmp11;_Tmp11.f0=1U,_Tmp11.f1=0,_Tmp11.f2=1;return _Tmp11;}else{struct _tuple17 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2!=0){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0){_Tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd;_TmpE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd;{void*r1=_Tmp10;void*r2=_TmpE;
# 740
if(Cyc_Tcutil_subset_effect(1,r1,r2)){
struct _tuple17 _Tmp11;_Tmp11.f0=1U,_Tmp11.f1=0,_Tmp11.f2=1;return _Tmp11;}
# 743
goto _LL17;}}else{goto _LL16;}}else{goto _LL16;}}else{goto _LL16;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17: {
# 747
struct _tuple12 _Tmp11=Cyc_Subtype_force_equivalence(t1,t2);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
struct _tuple17 _Tmp14;_Tmp14.f0=s,_Tmp14.f1=c,_Tmp14.f2=1;return _Tmp14;}}};}}
# 753
static enum Cyc_Absyn_Coercion Cyc_Subtype_internal_coercible_c(unsigned loc,int warn,void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_coercion){
# 760
struct _tuple17 _Tmp0=Cyc_Subtype_constraint_subtype(t1,t2,genconstr,allow_coercion);int _Tmp1;void*_Tmp2;enum Cyc_Absyn_Coercion _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Coercion s=_Tmp3;struct Cyc_List_List*c=_Tmp2;int b=_Tmp1;
if((int)s==0)
return s;
if(({int(*_Tmp4)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_forall_c;_Tmp4;})(Cyc_BansheeIf_add_constraint,loc,c)){
if(!b && warn){
if(Cyc_Tcutil_is_arithmetic_type(t1)&& Cyc_Tcutil_is_arithmetic_type(t2)){
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("integral size mismatch; ",sizeof(char),25U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" -> ",sizeof(char),5U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t2;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" conversion supplied",sizeof(char),21U);_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;Cyc_Warn_warn2(loc,_tag_fat(_Tmp9,sizeof(void*),5));});}else{
# 771
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("implicit cast from ",sizeof(char),20U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t2;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;Cyc_Warn_warn2(loc,_tag_fat(_Tmp8,sizeof(void*),4));});}}
# 773
return s;}
# 775
return 0U;}}
# 778
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_default_constraint,1);}
# 782
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_warn(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,1,t1,t2,Cyc_Subtype_gen_default_constraint,1);}
# 786
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_equality_constraint,0);}
