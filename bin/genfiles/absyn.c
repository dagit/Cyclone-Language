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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 371
extern struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 33 "position.h"
extern unsigned Cyc_Position_segment_join(unsigned,unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_ComplexCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueHeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntHeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AqualsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_AqualVarCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AqualHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};char Cyc_Absyn_EmptyAnnot[11U]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 886
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 914
void*Cyc_Absyn_compress(void*);
# 932
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 934
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 936
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 941
extern void*Cyc_Absyn_heap_rgn_type;
# 943
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 947
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 970
extern struct _tuple0*Cyc_Absyn_exn_name;
# 980
extern void*Cyc_Absyn_fat_bound_type;
# 994
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1126
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1131
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1169
void*Cyc_Absyn_pointer_expand(void*,int);
# 1202
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 32 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 55 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 82 "hashtable.h"
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 54
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty (void);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 36 "absyn.cyc"
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(1;ss1!=0 && ss2!=0;(ss1=ss1->tl,ss2=ss2->tl)){
int i=Cyc_strptrcmp((struct _fat_ptr*)ss1->hd,(struct _fat_ptr*)ss2->hd);
if(i!=0)return i;}
# 41
if(ss1!=0)return 1;
if(ss2!=0)return -1;
return 0;}
# 45
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1==(int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f0;union Cyc_Absyn_Nmspace f1;};
# 49
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1==q2)return 0;{
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=q1->f0;_Tmp0=q1->f1;{union Cyc_Absyn_Nmspace n1=_Tmp1;struct _fat_ptr*v1=_Tmp0;
void*_Tmp2;union Cyc_Absyn_Nmspace _Tmp3;_Tmp3=q2->f0;_Tmp2=q2->f1;{union Cyc_Absyn_Nmspace n2=_Tmp3;struct _fat_ptr*v2=_Tmp2;
int i=Cyc_strptrcmp(v1,v2);
if(i!=0)return i;{
struct _tuple12 _Tmp4=({struct _tuple12 _Tmp5;_Tmp5.f0=n1,_Tmp5.f1=n2;_Tmp5;});void*_Tmp5;void*_Tmp6;switch(_Tmp4.f0.Abs_n.tag){case 4: if(_Tmp4.f1.Loc_n.tag==4)
return 0;else{
# 61
return -1;}case 1: switch(_Tmp4.f1.Loc_n.tag){case 1: _Tmp6=_Tmp4.f0.Rel_n.val;_Tmp5=_Tmp4.f1.Rel_n.val;{struct Cyc_List_List*x1=_Tmp6;struct Cyc_List_List*x2=_Tmp5;
# 57
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;default:
# 63
 return -1;}case 2: switch(_Tmp4.f1.Rel_n.tag){case 2: _Tmp6=_Tmp4.f0.Abs_n.val;_Tmp5=_Tmp4.f1.Abs_n.val;{struct Cyc_List_List*x1=_Tmp6;struct Cyc_List_List*x2=_Tmp5;
# 58
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: goto _LL11;case 1: goto _LL15;default:
# 65
 return -1;}default: switch(_Tmp4.f1.Rel_n.tag){case 3: _Tmp6=_Tmp4.f0.C_n.val;_Tmp5=_Tmp4.f1.C_n.val;{struct Cyc_List_List*x1=_Tmp6;struct Cyc_List_List*x2=_Tmp5;
# 59
return Cyc_Absyn_strlist_cmp(x1,x2);}case 4: _LL11:
# 62
 return 1;case 1: _LL15:
# 64
 return 1;default:
# 66
 return 1;}};}}}}}
# 69
int Cyc_Absyn_hash_qvar(struct _tuple0*q){return Cyc_Hashtable_hash_string(*(*q).f1);}
# 71
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i!=0)return i;else{return tv1->identity - tv2->identity;}}
# 75
int Cyc_Absyn_tvar_id(struct Cyc_Absyn_Tvar*tv){
return tv->identity;}
# 78
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope){union Cyc_Absyn_Nmspace _Tmp0;_Tmp0.C_n.tag=3U,_Tmp0.C_n.val=x;return _Tmp0;}else{union Cyc_Absyn_Nmspace _Tmp0;_Tmp0.Abs_n.tag=2U,_Tmp0.Abs_n.val=x;return _Tmp0;}}
# 82
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _Tmp0;_Tmp0.Rel_n.tag=1U,_Tmp0.Rel_n.val=x;return _Tmp0;}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 85
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _Tmp0=(*qv).f0;switch(_Tmp0.Loc_n.tag){case 1: if(_Tmp0.Rel_n.val==0)
goto _LL4;else{goto _LL7;}case 2: if(_Tmp0.Abs_n.val==0){_LL4:
 goto _LL6;}else{goto _LL7;}case 4: _LL6:
 return 0;default: _LL7:
 return 1;};}
# 94
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
static int new_type_counter=0;
return(void*)({struct Cyc_Absyn_Evar_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Evar_Absyn_Type_struct));_Tmp0->tag=1,_Tmp0->f1=k,_Tmp0->f2=0,_Tmp0->f3=new_type_counter ++,_Tmp0->f4=env;_Tmp0;});}
# 98
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Kinds_mko,tenv);}
# 102
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned loc){struct Cyc_Absyn_Tqual _Tmp0;_Tmp0.print_const=0,_Tmp0.q_volatile=0,_Tmp0.q_restrict=0,_Tmp0.real_const=0,_Tmp0.loc=loc;return _Tmp0;}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned loc){struct Cyc_Absyn_Tqual _Tmp0;_Tmp0.print_const=1,_Tmp0.q_volatile=0,_Tmp0.q_restrict=0,_Tmp0.real_const=1,_Tmp0.loc=loc;return _Tmp0;}
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
struct Cyc_Absyn_Tqual _Tmp0;_Tmp0.print_const=x.print_const || y.print_const,_Tmp0.q_volatile=
x.q_volatile || y.q_volatile,_Tmp0.q_restrict=
x.q_restrict || y.q_restrict,_Tmp0.real_const=
x.real_const || y.real_const,({
unsigned _Tmp1=Cyc_Position_segment_join(x.loc,y.loc);_Tmp0.loc=_Tmp1;});return _Tmp0;}
# 111
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const==tq2.real_const && tq1.q_volatile==tq2.q_volatile)&& tq1.q_restrict==tq2.q_restrict;}
# 117
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 119
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
union Cyc_Absyn_DatatypeInfo _Tmp0;_Tmp0.UnknownDatatype.tag=1U,_Tmp0.UnknownDatatype.val=udi;return _Tmp0;}
# 122
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
union Cyc_Absyn_DatatypeInfo _Tmp0;_Tmp0.KnownDatatype.tag=2U,_Tmp0.KnownDatatype.val=d;return _Tmp0;}
# 125
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
union Cyc_Absyn_DatatypeFieldInfo _Tmp0;_Tmp0.UnknownDatatypefield.tag=1U,_Tmp0.UnknownDatatypefield.val=s;return _Tmp0;}
# 128
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
union Cyc_Absyn_DatatypeFieldInfo _Tmp0;_Tmp0.KnownDatatypefield.tag=2U,_Tmp0.KnownDatatypefield.val.f0=dd,_Tmp0.KnownDatatypefield.val.f1=df;return _Tmp0;}
# 131
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
union Cyc_Absyn_AggrInfo _Tmp0;_Tmp0.UnknownAggr.tag=1U,_Tmp0.UnknownAggr.val.f0=ak,_Tmp0.UnknownAggr.val.f1=n,_Tmp0.UnknownAggr.val.f2=tagged;return _Tmp0;}
# 134
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
union Cyc_Absyn_AggrInfo _Tmp0;_Tmp0.KnownAggr.tag=2U,_Tmp0.KnownAggr.val=ad;return _Tmp0;}
# 138
void*Cyc_Absyn_app_type(void*c,struct _fat_ptr args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,_Tmp0->f1=c,({struct Cyc_List_List*_Tmp1=Cyc_List_from_array(args);_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 141
void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp0->tag=8,_Tmp0->f1=e;_Tmp0;});}
# 150
static struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct Cyc_Absyn_al_qual_type_tyc={16,Cyc_Absyn_Aliasable_qual};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_al_qual_type_gval={0,(void*)& Cyc_Absyn_al_qual_type_tyc,0};void*Cyc_Absyn_al_qual_type=(void*)& Cyc_Absyn_al_qual_type_gval;
static struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct Cyc_Absyn_un_qual_type_tyc={16,Cyc_Absyn_Unique_qual};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_un_qual_type_gval={0,(void*)& Cyc_Absyn_un_qual_type_tyc,0};void*Cyc_Absyn_un_qual_type=(void*)& Cyc_Absyn_un_qual_type_gval;
static struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct Cyc_Absyn_rc_qual_type_tyc={16,Cyc_Absyn_Refcnt_qual};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_rc_qual_type_gval={0,(void*)& Cyc_Absyn_rc_qual_type_tyc,0};void*Cyc_Absyn_rc_qual_type=(void*)& Cyc_Absyn_rc_qual_type_gval;
static struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct Cyc_Absyn_rtd_qual_type_tyc={16,Cyc_Absyn_Restricted_qual};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_rtd_qual_type_gval={0,(void*)& Cyc_Absyn_rtd_qual_type_tyc,0};void*Cyc_Absyn_rtd_qual_type=(void*)& Cyc_Absyn_rtd_qual_type_gval;
# 177 "absyn.cyc"
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_Absyn_void_type_cval={0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_void_type_tval={0,(void*)& Cyc_Absyn_void_type_cval,0};void*Cyc_Absyn_void_type=(void*)& Cyc_Absyn_void_type_tval;
static struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct Cyc_Absyn_heap_rgn_type_cval={6};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_heap_rgn_type_tval={0,(void*)& Cyc_Absyn_heap_rgn_type_cval,0};void*Cyc_Absyn_heap_rgn_type=(void*)& Cyc_Absyn_heap_rgn_type_tval;
static struct Cyc_Absyn_UniqueHeapCon_Absyn_TyCon_struct Cyc_Absyn_unique_rgn_shorthand_type_cval={7};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_unique_rgn_shorthand_type_tval={0,(void*)& Cyc_Absyn_unique_rgn_shorthand_type_cval,0};void*Cyc_Absyn_unique_rgn_shorthand_type=(void*)& Cyc_Absyn_unique_rgn_shorthand_type_tval;
static struct Cyc_Absyn_RefCntHeapCon_Absyn_TyCon_struct Cyc_Absyn_refcnt_rgn_shorthand_type_cval={8};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_refcnt_rgn_shorthand_type_tval={0,(void*)& Cyc_Absyn_refcnt_rgn_shorthand_type_cval,0};void*Cyc_Absyn_refcnt_rgn_shorthand_type=(void*)& Cyc_Absyn_refcnt_rgn_shorthand_type_tval;
static struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct Cyc_Absyn_true_type_cval={11};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_true_type_tval={0,(void*)& Cyc_Absyn_true_type_cval,0};void*Cyc_Absyn_true_type=(void*)& Cyc_Absyn_true_type_tval;
static struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct Cyc_Absyn_false_type_cval={12};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_false_type_tval={0,(void*)& Cyc_Absyn_false_type_cval,0};void*Cyc_Absyn_false_type=(void*)& Cyc_Absyn_false_type_tval;
static struct Cyc_Absyn_FatCon_Absyn_TyCon_struct Cyc_Absyn_fat_bound_type_cval={14};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_fat_bound_type_tval={0,(void*)& Cyc_Absyn_fat_bound_type_cval,0};void*Cyc_Absyn_fat_bound_type=(void*)& Cyc_Absyn_fat_bound_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_schar_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_schar_type_tval={0,(void*)& Cyc_Absyn_schar_type_cval,0};void*Cyc_Absyn_schar_type=(void*)& Cyc_Absyn_schar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uchar_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uchar_type_tval={0,(void*)& Cyc_Absyn_uchar_type_cval,0};void*Cyc_Absyn_uchar_type=(void*)& Cyc_Absyn_uchar_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_char_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Char_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_char_type_tval={0,(void*)& Cyc_Absyn_char_type_cval,0};void*Cyc_Absyn_char_type=(void*)& Cyc_Absyn_char_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sshort_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sshort_type_tval={0,(void*)& Cyc_Absyn_sshort_type_cval,0};void*Cyc_Absyn_sshort_type=(void*)& Cyc_Absyn_sshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ushort_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ushort_type_tval={0,(void*)& Cyc_Absyn_ushort_type_cval,0};void*Cyc_Absyn_ushort_type=(void*)& Cyc_Absyn_ushort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nshort_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Short_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nshort_type_tval={0,(void*)& Cyc_Absyn_nshort_type_cval,0};void*Cyc_Absyn_nshort_type=(void*)& Cyc_Absyn_nshort_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_sint_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_sint_type_tval={0,(void*)& Cyc_Absyn_sint_type_cval,0};void*Cyc_Absyn_sint_type=(void*)& Cyc_Absyn_sint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_uint_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_uint_type_tval={0,(void*)& Cyc_Absyn_uint_type_cval,0};void*Cyc_Absyn_uint_type=(void*)& Cyc_Absyn_uint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nint_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Int_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nint_type_tval={0,(void*)& Cyc_Absyn_nint_type_cval,0};void*Cyc_Absyn_nint_type=(void*)& Cyc_Absyn_nint_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slong_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slong_type_tval={0,(void*)& Cyc_Absyn_slong_type_cval,0};void*Cyc_Absyn_slong_type=(void*)& Cyc_Absyn_slong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulong_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulong_type_tval={0,(void*)& Cyc_Absyn_ulong_type_cval,0};void*Cyc_Absyn_ulong_type=(void*)& Cyc_Absyn_ulong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlong_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_Long_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlong_type_tval={0,(void*)& Cyc_Absyn_nlong_type_cval,0};void*Cyc_Absyn_nlong_type=(void*)& Cyc_Absyn_nlong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_slonglong_type_cval={1,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_slonglong_type_tval={0,(void*)& Cyc_Absyn_slonglong_type_cval,0};void*Cyc_Absyn_slonglong_type=(void*)& Cyc_Absyn_slonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_ulonglong_type_cval={1,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_ulonglong_type_tval={0,(void*)& Cyc_Absyn_ulonglong_type_cval,0};void*Cyc_Absyn_ulonglong_type=(void*)& Cyc_Absyn_ulonglong_type_tval;
static struct Cyc_Absyn_IntCon_Absyn_TyCon_struct Cyc_Absyn_nlonglong_type_cval={1,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_nlonglong_type_tval={0,(void*)& Cyc_Absyn_nlonglong_type_cval,0};void*Cyc_Absyn_nlonglong_type=(void*)& Cyc_Absyn_nlonglong_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_float_type_cval={2,0};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_float_type_tval={0,(void*)& Cyc_Absyn_float_type_cval,0};void*Cyc_Absyn_float_type=(void*)& Cyc_Absyn_float_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_double_type_cval={2,1};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_double_type_tval={0,(void*)& Cyc_Absyn_double_type_cval,0};void*Cyc_Absyn_double_type=(void*)& Cyc_Absyn_double_type_tval;
static struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct Cyc_Absyn_long_double_type_cval={2,2};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_long_double_type_tval={0,(void*)& Cyc_Absyn_long_double_type_cval,0};void*Cyc_Absyn_long_double_type=(void*)& Cyc_Absyn_long_double_type_tval;
# 203
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_empty_effect_cval={9};static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_Absyn_empty_effect_tval={0,(void*)& Cyc_Absyn_empty_effect_cval,0};void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_effect_tval;
# 205
static struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct Cyc_Absyn_RgnHandleCon_val={4};
static struct Cyc_Absyn_AqualHandleCon_Absyn_TyCon_struct Cyc_Absyn_AqualHandleCon_val={18};
static struct Cyc_Absyn_AqualVarCon_Absyn_TyCon_struct Cyc_Absyn_AqualVarCon_val={17};
static struct Cyc_Absyn_TagCon_Absyn_TyCon_struct Cyc_Absyn_TagCon_val={5};
# 210
static struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct Cyc_Absyn_RgnsCon_val={10};
static struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct Cyc_Absyn_ThinCon_val={13};
static struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct Cyc_Absyn_JoinCon_val={9};
static struct Cyc_Absyn_AqualsCon_Absyn_TyCon_struct Cyc_Absyn_AqualsCon_val={15};
# 215
void*Cyc_Absyn_aqual_constant(enum Cyc_Absyn_AliasQualVal v){
switch((int)v){case Cyc_Absyn_Aliasable_qual:
 return Cyc_Absyn_al_qual_type;case Cyc_Absyn_Unique_qual:
 return Cyc_Absyn_un_qual_type;case Cyc_Absyn_Refcnt_qual:
 return Cyc_Absyn_rc_qual_type;case Cyc_Absyn_Restricted_qual:
 return Cyc_Absyn_rtd_qual_type;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Impossible alias qualifier constant",sizeof(char),36U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 225
void*Cyc_Absyn_rgn_handle_type(void*r){void*_Tmp0[1];_Tmp0[0]=r;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnHandleCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
void*Cyc_Absyn_aqual_handle_type(void*aq){void*_Tmp0[1];_Tmp0[0]=aq;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_AqualHandleCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
void*Cyc_Absyn_aqual_var_type(void*tv,void*bnd){void*_Tmp0[2];_Tmp0[0]=tv,_Tmp0[1]=bnd;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_AqualVarCon_val,_tag_fat(_Tmp0,sizeof(void*),2));}
# 229
void*Cyc_Absyn_tag_type(void*t){void*_Tmp0[1];_Tmp0[0]=t;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_TagCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
# 231
void*Cyc_Absyn_regionsof_eff(void*t){void*_Tmp0[1];_Tmp0[0]=t;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_RgnsCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
void*Cyc_Absyn_thin_bounds_type(void*t){void*_Tmp0[1];_Tmp0[0]=t;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_ThinCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
void*Cyc_Absyn_join_eff(struct Cyc_List_List*ts){return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,_Tmp0->f1=(void*)& Cyc_Absyn_empty_effect_cval,_Tmp0->f2=ts;_Tmp0;});}
# 235
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d){
void*_Tmp0=(void*)({struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct));_Tmp1->tag=19,_Tmp1->f1=n,_Tmp1->f2=d;_Tmp1;});return Cyc_Absyn_app_type(_Tmp0,_tag_fat(0U,sizeof(void*),0));}
# 238
void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*fs){
void*_Tmp0=(void*)({struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct));_Tmp1->tag=20,_Tmp1->f1=fs;_Tmp1;});return Cyc_Absyn_app_type(_Tmp0,_tag_fat(0U,sizeof(void*),0));}
# 241
void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k){
void*_Tmp0=(void*)({struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct));_Tmp1->tag=21,_Tmp1->f1=s,_Tmp1->f2=k;_Tmp1;});return Cyc_Absyn_app_type(_Tmp0,_tag_fat(0U,sizeof(void*),0));}
# 244
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,({void*_Tmp1=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_Tmp2->tag=22,_Tmp2->f1=di;_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=args;_Tmp0;});}
# 247
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo di,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,({void*_Tmp1=(void*)({struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct));_Tmp2->tag=23,_Tmp2->f1=di;_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=args;_Tmp0;});}
# 250
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo ai,struct Cyc_List_List*args){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,({void*_Tmp1=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp2->tag=24,_Tmp2->f1=ai;_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=args;_Tmp0;});}
# 253
void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*x){return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp0->tag=2,_Tmp0->f1=x;_Tmp0;});}
# 256
void*Cyc_Absyn_aqualsof_type(void*tv){
void*_Tmp0[1];_Tmp0[0]=tv;return Cyc_Absyn_app_type((void*)& Cyc_Absyn_AqualsCon_val,_tag_fat(_Tmp0,sizeof(void*),1));}
# 260
void*Cyc_Absyn_gen_float_type(unsigned i){
switch((int)i){case 0:
 return Cyc_Absyn_float_type;case 1:
 return Cyc_Absyn_double_type;case 2:
 return Cyc_Absyn_long_double_type;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("gen_float_type(",sizeof(char),16U);_Tmp1;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp2;_Tmp2.tag=12,_Tmp2.f1=(int)i;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(")",sizeof(char),2U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),3));});};}
# 268
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
switch((int)sn){case Cyc_Absyn_Signed:
# 271
 switch((int)sz){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_sshort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_sint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_slong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL15;default: _LL15:
 return Cyc_Absyn_slonglong_type;};case Cyc_Absyn_Unsigned:
# 280
 switch((int)sz){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_uchar_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_ushort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_uint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL22;default: _LL22:
 return Cyc_Absyn_ulonglong_type;};case Cyc_Absyn_None:
# 288
 goto _LL8;default: _LL8:
# 290
 switch((int)sz){case Cyc_Absyn_Char_sz:
 return Cyc_Absyn_char_type;case Cyc_Absyn_Short_sz:
 return Cyc_Absyn_nshort_type;case Cyc_Absyn_Int_sz:
 return Cyc_Absyn_nint_type;case Cyc_Absyn_Long_sz:
 return Cyc_Absyn_nlong_type;case Cyc_Absyn_LongLong_sz:
 goto _LL2F;default: _LL2F:
 return Cyc_Absyn_nlonglong_type;};};}
# 301
void*Cyc_Absyn_complex_type(void*t){
# 304
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1:
 goto _LL4;case 2: _LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp1->tag=0,({void*_Tmp2=(void*)({struct Cyc_Absyn_ComplexCon_Absyn_TyCon_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_ComplexCon_Absyn_TyCon_struct));_Tmp3->tag=3;_Tmp3;});_Tmp1->f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({void*_Tmp3[1];_Tmp3[0]=t;Cyc_List_list(_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp1->f2=_Tmp2;});_Tmp1;});default: goto _LL5;}else{_LL5:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("bad complex type ",sizeof(char),18U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),2));});};}
# 311
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_thin_bounds_type(Cyc_Absyn_valueof_type(e));}
# 314
void*Cyc_Absyn_thin_bounds_int(unsigned i){
struct Cyc_Absyn_Exp*e=Cyc_Absyn_uint_exp(i,0U);
e->topt=Cyc_Absyn_uint_type;
return Cyc_Absyn_thin_bounds_exp(e);}
# 319
void*Cyc_Absyn_bounds_one (void){
static void*bone=0;
if(bone==0)
bone=Cyc_Absyn_thin_bounds_int(1U);
return bone;}
# 329
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 332
void*Cyc_Absyn_wchar_type (void){
switch((int)Sizeof_wchar_t){case 1:
# 343 "absyn.cyc"
 if(Wchar_t_unsigned)return Cyc_Absyn_uchar_type;else{return Cyc_Absyn_schar_type;}case 2:
 if(Wchar_t_unsigned)return Cyc_Absyn_ushort_type;else{return Cyc_Absyn_sshort_type;}default:
# 347
 if(Wchar_t_unsigned)return Cyc_Absyn_uint_type;else{return Cyc_Absyn_sint_type;}};}static char _TmpG0[4U]="exn";
# 352
static struct _fat_ptr Cyc_Absyn_exn_str={_TmpG0,_TmpG0,_TmpG0 + 4U};
static struct _tuple0 Cyc_Absyn_exn_name_v={.f0={.Abs_n={2,0}},.f1=& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _TmpG1[15U]="Null_Exception";static char _TmpG2[13U]="Array_bounds";static char _TmpG3[16U]="Match_Exception";static char _TmpG4[10U]="Bad_alloc";
# 356
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void){
static struct _fat_ptr builtin_exns[4U]={{_TmpG1,_TmpG1,_TmpG1 + 15U},{_TmpG2,_TmpG2,_TmpG2 + 13U},{_TmpG3,_TmpG3,_TmpG3 + 16U},{_TmpG4,_TmpG4,_TmpG4 + 10U}};
# 359
static struct Cyc_Absyn_Datatypedecl*tud_opt=0;
if(tud_opt==0){
struct Cyc_List_List*tufs=0;
{int i=0;for(0;(unsigned)i < 4U;++ i){
tufs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp1=({struct Cyc_Absyn_Datatypefield*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f0.Abs_n.tag=2U,_Tmp4->f0.Abs_n.val=0,({
struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=*((struct _fat_ptr*)_check_known_subscript_notnull(builtin_exns,4U,sizeof(struct _fat_ptr),i));_Tmp6;});_Tmp4->f1=_Tmp5;});_Tmp4;});
# 363
_Tmp2->name=_Tmp3;}),_Tmp2->typs=0,_Tmp2->loc=0U,_Tmp2->sc=3U;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tufs;_Tmp0;});}}
# 367
tud_opt=({struct Cyc_Absyn_Datatypedecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp0->sc=3U,_Tmp0->name=Cyc_Absyn_exn_name,_Tmp0->tvs=0,({struct Cyc_Core_Opt*_Tmp1=({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=tufs;_Tmp2;});_Tmp0->fields=_Tmp1;}),_Tmp0->is_extensible=1;_Tmp0;});}
# 369
return tud_opt;}
# 372
void*Cyc_Absyn_exn_type (void){
static void*exn_typ=0;
static void*eopt=0;
if(exn_typ==0){
eopt=Cyc_Absyn_datatype_type(({union Cyc_Absyn_DatatypeInfo _Tmp0;_Tmp0.KnownDatatype.tag=2U,({struct Cyc_Absyn_Datatypedecl**_Tmp1=({struct Cyc_Absyn_Datatypedecl**_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));({struct Cyc_Absyn_Datatypedecl*_Tmp3=Cyc_Absyn_exn_tud();*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0.KnownDatatype.val=_Tmp1;});_Tmp0;}),0);
exn_typ=({void*_Tmp0=eopt;void*_Tmp1=Cyc_Absyn_heap_rgn_type;void*_Tmp2=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);void*_Tmp4=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Absyn_false_type);});}
# 379
return exn_typ;}
# 382
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar (void){
static struct _tuple0*q=0;
if(q==0)
q=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Abs_n(0,0);_Tmp0->f0=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("PrintArg",sizeof(char),9U);_Tmp2;});_Tmp0->f1=_Tmp1;});_Tmp0;});
return q;}
# 388
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar (void){
static struct _tuple0*q=0;
if(q==0)
q=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Abs_n(0,0);_Tmp0->f0=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("ScanfArg",sizeof(char),9U);_Tmp2;});_Tmp0->f1=_Tmp1;});_Tmp0;});
return q;}
# 416 "absyn.cyc"
struct _tuple0*Cyc_Absyn_uniqueaqual_qvar (void){
static struct _tuple0*q=0;
if(q==0)
q=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Abs_n(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=_tag_fat("Core",sizeof(char),5U);_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}),0);_Tmp0->f0=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("unique_qual",sizeof(char),12U);_Tmp2;});_Tmp0->f1=_Tmp1;});_Tmp0;});
return q;}
# 422
struct Cyc_Absyn_Exp*Cyc_Absyn_uniqueaqual_exp (void){
void*t=Cyc_Absyn_aqual_handle_type(Cyc_Absyn_un_qual_type);
struct Cyc_Absyn_Vardecl*vd=({struct _tuple0*_Tmp0=Cyc_Absyn_uniqueaqual_qvar();Cyc_Absyn_new_vardecl(0U,_Tmp0,t,0,0);});
vd->sc=3U;{
struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=t,_Tmp0->loc=0U,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,({
void*_Tmp1=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp2->tag=1,({void*_Tmp3=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_Tmp4->tag=1,_Tmp4->f1=vd;_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->r=_Tmp1;});return _Tmp0;}}
# 431
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo s){
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_Tmp0->tag=3,_Tmp0->f1=s;_Tmp0;});}
# 435
void*Cyc_Absyn_fatptr_type(void*t,void*r,void*aq,struct Cyc_Absyn_Tqual tq,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp0;_Tmp0.elt_type=t,_Tmp0.elt_tq=tq,_Tmp0.ptr_atts.eff=r,_Tmp0.ptr_atts.nullable=Cyc_Absyn_true_type,_Tmp0.ptr_atts.bounds=Cyc_Absyn_fat_bound_type,_Tmp0.ptr_atts.zero_term=zt,_Tmp0.ptr_atts.ptrloc=0,_Tmp0.ptr_atts.autoreleased=rel,_Tmp0.ptr_atts.aqual=aq;_Tmp0;}));}
# 442
void*Cyc_Absyn_starb_type(void*t,void*r,void*aq,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp0;_Tmp0.elt_type=t,_Tmp0.elt_tq=tq,_Tmp0.ptr_atts.eff=r,_Tmp0.ptr_atts.nullable=Cyc_Absyn_true_type,_Tmp0.ptr_atts.bounds=b,_Tmp0.ptr_atts.zero_term=zt,_Tmp0.ptr_atts.ptrloc=0,_Tmp0.ptr_atts.autoreleased=rel,_Tmp0.ptr_atts.aqual=aq;_Tmp0;}));}
# 448
void*Cyc_Absyn_atb_type(void*t,void*r,void*aq,struct Cyc_Absyn_Tqual tq,void*b,void*zt,void*rel){
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp0;_Tmp0.elt_type=t,_Tmp0.elt_tq=tq,_Tmp0.ptr_atts.eff=r,_Tmp0.ptr_atts.nullable=Cyc_Absyn_false_type,_Tmp0.ptr_atts.bounds=b,_Tmp0.ptr_atts.zero_term=zt,_Tmp0.ptr_atts.ptrloc=0,_Tmp0.ptr_atts.autoreleased=rel,_Tmp0.ptr_atts.aqual=aq;_Tmp0;}));}
# 454
void*Cyc_Absyn_star_type(void*t,void*r,void*aq,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
void*_Tmp0=t;void*_Tmp1=r;void*_Tmp2=aq;struct Cyc_Absyn_Tqual _Tmp3=tq;void*_Tmp4=Cyc_Absyn_bounds_one();void*_Tmp5=zeroterm;return Cyc_Absyn_starb_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,rel);}
# 457
void*Cyc_Absyn_cstar_type(void*t,struct Cyc_Absyn_Tqual tq){
return Cyc_Absyn_star_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}
# 460
void*Cyc_Absyn_at_type(void*t,void*r,void*aq,struct Cyc_Absyn_Tqual tq,void*zeroterm,void*rel){
void*_Tmp0=t;void*_Tmp1=r;void*_Tmp2=aq;struct Cyc_Absyn_Tqual _Tmp3=tq;void*_Tmp4=Cyc_Absyn_bounds_one();void*_Tmp5=zeroterm;return Cyc_Absyn_atb_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,rel);}
# 463
void*Cyc_Absyn_string_type(void*rgn,void*aq){
void*_Tmp0=Cyc_Absyn_char_type;void*_Tmp1=rgn;void*_Tmp2=aq;struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);void*_Tmp4=Cyc_Absyn_fat_bound_type;void*_Tmp5=Cyc_Absyn_true_type;return Cyc_Absyn_starb_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);}
# 466
void*Cyc_Absyn_const_string_type(void*rgn,void*aq){
void*_Tmp0=Cyc_Absyn_char_type;void*_Tmp1=rgn;void*_Tmp2=aq;struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_const_tqual(0U);void*_Tmp4=Cyc_Absyn_fat_bound_type;void*_Tmp5=Cyc_Absyn_true_type;return Cyc_Absyn_starb_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);}
# 470
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc){
# 472
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp0->tag=4,_Tmp0->f1.elt_type=elt_type,_Tmp0->f1.tq=tq,_Tmp0->f1.num_elts=num_elts,_Tmp0->f1.zero_term=zero_term,_Tmp0->f1.zt_loc=ztloc;_Tmp0;});}
# 475
void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_TypeofType_Absyn_Type_struct));_Tmp0->tag=10,_Tmp0->f1=e;_Tmp0;});}
# 480
void*Cyc_Absyn_typedef_type(struct _tuple0*n,struct Cyc_List_List*args,struct Cyc_Absyn_Typedefdecl*d,void*defn){
# 482
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_Absyn_Type_struct));_Tmp0->tag=7,_Tmp0->f1=n,_Tmp0->f2=args,_Tmp0->f3=d,_Tmp0->f4=defn;_Tmp0;});}
# 488
static void*Cyc_Absyn_aggregate_type(enum Cyc_Absyn_AggrKind k,struct _fat_ptr*name){
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp0->tag=0,({void*_Tmp1=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp2->tag=24,({union Cyc_Absyn_AggrInfo _Tmp3=({enum Cyc_Absyn_AggrKind _Tmp4=k;Cyc_Absyn_UnknownAggr(_Tmp4,({struct _tuple0*_Tmp5=_cycalloc(sizeof(struct _tuple0));_Tmp5->f0=Cyc_Absyn_rel_ns_null,_Tmp5->f1=name;_Tmp5;}),0);});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 491
void*Cyc_Absyn_strct(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(0U,name);}
void*Cyc_Absyn_union_typ(struct _fat_ptr*name){return Cyc_Absyn_aggregate_type(1U,name);}
# 494
void*Cyc_Absyn_strctq(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(0U,name,0),0);}
# 497
void*Cyc_Absyn_unionq_type(struct _tuple0*name){
return Cyc_Absyn_aggr_type(Cyc_Absyn_UnknownAggr(1U,name,0),0);}
# 502
void*Cyc_Absyn_compress(void*t){
void*_Tmp0;switch(*((int*)t)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2==0)
goto _LL4;else{_Tmp0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_LL8: {void**t2opt_ref=_Tmp0;
# 508
void*ta=_check_null(*t2opt_ref);
void*t2=Cyc_Absyn_compress(ta);
if(t2!=ta)
*t2opt_ref=t2;
return t2;}}case 7: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4==0){_LL4:
# 505
 return t;}else{_Tmp0=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{void**topt_ref=_Tmp0;
_Tmp0=topt_ref;goto _LL8;}}case 8: _Tmp0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
# 515
Cyc_Evexp_eval_const_uint_exp(e);
# 517
CAST_LOOP: {
void*_Tmp1=e->r;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp1)){case 38: _Tmp3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp1)->f1;{void*t2=_Tmp3;
return Cyc_Absyn_compress(t2);}case 14: _Tmp3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp1)->f2;{void*t2=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 521
void*_Tmp4=Cyc_Absyn_compress(t2);if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1==Cyc_Absyn_Unsigned)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f2){case Cyc_Absyn_Int_sz:
 goto _LL20;case Cyc_Absyn_Long_sz: _LL20: {
# 524
void*_Tmp5=e2->r;if(*((int*)_Tmp5)==38){
e=e2;goto CAST_LOOP;}else{
return t;};}default: goto _LL21;}else{goto _LL21;}}else{goto _LL21;}}else{_LL21:
# 528
 return t;};}default:
# 530
 return t;};}}case 10: _Tmp0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
# 533
void*t2=e->topt;
if(t2!=0)return t2;else{return t;}}case 9: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f2!=0){_Tmp0=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f2;{void*t=_Tmp0;
return Cyc_Absyn_compress(t);}}else{goto _LL13;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 15: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*tv=_Tmp0;
# 537
void*ctv=Cyc_Absyn_compress(tv);
if(ctv==tv)
return t;
{void*_Tmp1;switch(*((int*)ctv)){case 3: _Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)ctv)->f1.ptr_atts.aqual;{void*aq=_Tmp1;
# 542
return Cyc_Absyn_compress(aq);}case 2:
 goto _LL2E;case 1: _LL2E:
# 545
 goto _LL28;default:
# 547
 return Cyc_Absyn_al_qual_type;}_LL28:;}
# 549
return Cyc_Absyn_aqualsof_type(ctv);}}else{goto _LL13;}case 17: _Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*tv_bnd=_Tmp0;
# 551
void*comp=Cyc_Absyn_compress((void*)_check_null(tv_bnd)->hd);
void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;switch(*((int*)comp)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)comp)->f1)){case 16: _Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)comp)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_Tmp2;
# 554
return comp;}case 15: _LL39:
# 558
 if(comp==(void*)tv_bnd->hd)
return t;{
void*_Tmp3=comp;return Cyc_Absyn_aqual_var_type(_Tmp3,Cyc_Absyn_compress((void*)_check_null(tv_bnd->tl)->hd));}case 17: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)comp)->f2;{struct Cyc_List_List*tvb2=_Tmp1;
# 562
return Cyc_Absyn_compress(comp);}default: goto _LL3C;}case 2:
# 555
 goto _LL37;case 1: _LL37:
 goto _LL39;default: _LL3C:
# 564
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("Unexpected type within AqualVar: ",sizeof(char),34U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)comp;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});};}default: goto _LL13;}default: _LL13:
# 566
 return t;};}
# 571
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Char_c.tag=2U,_Tmp0.Char_c.val.f0=sn,_Tmp0.Char_c.val.f1=c;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _fat_ptr s){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Wchar_c.tag=3U,_Tmp0.Wchar_c.val=s;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Short_c.tag=4U,_Tmp0.Short_c.val.f0=sn,_Tmp0.Short_c.val.f1=s;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Int_c.tag=5U,_Tmp0.Int_c.val.f0=sn,_Tmp0.Int_c.val.f1=i;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.LongLong_c.tag=6U,_Tmp0.LongLong_c.val.f0=sn,_Tmp0.LongLong_c.val.f1=l;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _fat_ptr s,int i){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Float_c.tag=7U,_Tmp0.Float_c.val.f0=s,_Tmp0.Float_c.val.f1=i;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _fat_ptr s){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.String_c.tag=8U,_Tmp0.String_c.val=s;return _Tmp0;}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _fat_ptr s){union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Wstring_c.tag=9U,_Tmp0.Wstring_c.val=s;return _Tmp0;}
# 581
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned loc){
struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));_Tmp0->topt=0,_Tmp0->r=r,_Tmp0->loc=loc,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;return _Tmp0;}
# 584
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*qual_hdl,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct));_Tmp1->tag=16,_Tmp1->f1=rgn_handle,_Tmp1->f2=e,_Tmp1->f3=qual_hdl;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 587
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Exp));*_Tmp0=*e;return _Tmp0;}
# 590
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct));_Tmp1->tag=0,_Tmp1->f1=c;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 593
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 597
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned seg){union Cyc_Absyn_Cnst _Tmp0=Cyc_Absyn_Int_c(s,i);return Cyc_Absyn_const_exp(_Tmp0,seg);}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{.f0=0U,.f1=0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{.f0=0U,.f1=1}}}};
if(i==0)return Cyc_Absyn_new_exp((void*)& szero,loc);
if(i==1)return Cyc_Absyn_new_exp((void*)& sone,loc);
return Cyc_Absyn_int_exp(0U,i,loc);}
# 605
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned i,unsigned loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{.f0=1U,.f1=0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{.f0=1U,.f1=1}}}};
if(i==0U)return Cyc_Absyn_new_exp((void*)& uzero,loc);
if(i==1U)return Cyc_Absyn_new_exp((void*)& uone,loc);
return Cyc_Absyn_int_exp(1U,(int)i,loc);}
# 612
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned loc){union Cyc_Absyn_Cnst _Tmp0=Cyc_Absyn_Char_c(2U,c);return Cyc_Absyn_const_exp(_Tmp0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr f,int i,unsigned loc){
union Cyc_Absyn_Cnst _Tmp0=Cyc_Absyn_Float_c(f,i);return Cyc_Absyn_const_exp(_Tmp0,loc);}
# 619
static struct Cyc_Absyn_Exp*Cyc_Absyn_str2exp(union Cyc_Absyn_Cnst(*f)(struct _fat_ptr),struct _fat_ptr s,unsigned loc){
union Cyc_Absyn_Cnst _Tmp0=f(s);return Cyc_Absyn_const_exp(_Tmp0,loc);}
# 622
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wchar_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_String_c,s,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr s,unsigned loc){return Cyc_Absyn_str2exp(Cyc_Absyn_Wstring_c,s,loc);}
# 626
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp1->tag=1,({void*_Tmp2=(void*)({struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_Tmp3->tag=0,_Tmp3->f1=q;_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*b,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct));_Tmp1->tag=1,_Tmp1->f1=b;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 633
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned loc){
return Cyc_Absyn_var_exp(q,loc);}
# 636
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr s,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct));_Tmp1->tag=2,_Tmp1->f1=s;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 639
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct));_Tmp1->tag=3,_Tmp1->f1=p,_Tmp1->f2=es;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 642
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned loc){
enum Cyc_Absyn_Primop _Tmp0=p;struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=e,_Tmp2->tl=0;_Tmp2;});return Cyc_Absyn_primop_exp(_Tmp0,_Tmp1,loc);}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
enum Cyc_Absyn_Primop _Tmp0=p;struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=e1,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=e2,_Tmp4->tl=0;_Tmp4;});_Tmp2->tl=_Tmp3;});_Tmp2;});return Cyc_Absyn_primop_exp(_Tmp0,_Tmp1,loc);}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_tagof_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
return Cyc_Absyn_prim1_exp(19U,e,loc);}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct));_Tmp1->tag=34,_Tmp1->f1=e1,_Tmp1->f2=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 654
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(0U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(1U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(3U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(5U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(6U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(7U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(8U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(9U,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){return Cyc_Absyn_prim2_exp(10U,e1,e2,loc);}
# 664
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct));_Tmp1->tag=4,_Tmp1->f1=e1,_Tmp1->f2=popt,_Tmp1->f3=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 667
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 670
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp1->tag=5,_Tmp1->f1=e,_Tmp1->f2=i;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 673
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct));_Tmp1->tag=6,_Tmp1->f1=e1,_Tmp1->f2=e2,_Tmp1->f3=e3;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 676
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct));_Tmp1->tag=7,_Tmp1->f1=e1,_Tmp1->f2=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 679
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct));_Tmp1->tag=8,_Tmp1->f1=e1,_Tmp1->f2=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 682
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct));_Tmp1->tag=9,_Tmp1->f1=e1,_Tmp1->f2=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 685
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp1->tag=10,_Tmp1->f1=e,_Tmp1->f2=es,_Tmp1->f3=0,_Tmp1->f4=0;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 688
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct));_Tmp1->tag=10,_Tmp1->f1=e,_Tmp1->f2=es,_Tmp1->f3=0,_Tmp1->f4=1;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 691
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct));_Tmp1->tag=12,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 694
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct));_Tmp1->tag=13,_Tmp1->f1=e,_Tmp1->f2=ts;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 697
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct));_Tmp1->tag=14,_Tmp1->f1=t,_Tmp1->f2=e,_Tmp1->f3=user_cast,_Tmp1->f4=c;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 700
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_Tmp1->tag=11,_Tmp1->f1=e,_Tmp1->f2=0;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 703
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_Tmp1->tag=11,_Tmp1->f1=e,_Tmp1->f2=1;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 706
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned loc){void*_Tmp0=(void*)({struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct));_Tmp1->tag=15,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct));_Tmp1->tag=17,_Tmp1->f1=t;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 710
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct));_Tmp1->tag=18,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 713
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct));_Tmp1->tag=19,_Tmp1->f1=t,_Tmp1->f2=ofs;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 716
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned loc){void*_Tmp0=(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct));_Tmp1->tag=20,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_Tmp1->tag=21,_Tmp1->f1=e,_Tmp1->f2=n,_Tmp1->f3=0,_Tmp1->f4=0;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 720
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*n,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct));_Tmp1->tag=22,_Tmp1->f1=e,_Tmp1->f2=n,_Tmp1->f3=0,_Tmp1->f4=0;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 723
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct));_Tmp1->tag=23,_Tmp1->f1=e1,_Tmp1->f2=e2;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 726
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int i){
static struct Cyc_Xarray_Xarray*x=0;
if(x==0)x=Cyc_Xarray_create_empty();
while(({int _Tmp0=i;_Tmp0 >= Cyc_Xarray_length(_check_null(x));})){
{struct _fat_ptr s=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)i;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("f%d",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
struct _fat_ptr*n;n=_cycalloc(sizeof(struct _fat_ptr)),*n=s;
({int(*_Tmp0)(struct Cyc_Xarray_Xarray*,struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)=({int(*_Tmp1)(struct Cyc_Xarray_Xarray*,struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)=(int(*)(struct Cyc_Xarray_Xarray*,struct Cyc_Absyn_FieldName_Absyn_Designator_struct*))Cyc_Xarray_add_ind;_Tmp1;});struct Cyc_Xarray_Xarray*_Tmp1=_check_null(x);_Tmp0(_Tmp1,({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp2->tag=1,_Tmp2->f1=n;_Tmp2;}));});}
# 730
1U;}
# 734
return({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*(*_Tmp0)(struct Cyc_Xarray_Xarray*,int)=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;_Tmp0;})(_check_null(x),i);}struct _tuple13{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 736
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*dles=0;
{unsigned i=0U;for(0;es!=0;(es=es->tl,++ i)){
struct _tuple13*dle;dle=_cycalloc(sizeof(struct _tuple13)),({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)Cyc_Absyn_tuple_field_designator((int)i);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;});dle->f0=_Tmp0;}),dle->f1=(struct Cyc_Absyn_Exp*)es->hd;
# 741
dles=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=dle,_Tmp0->tl=dles;_Tmp0;});}}
# 743
dles=Cyc_List_imp_rev(dles);{
void*_Tmp0=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp1->tag=29,_Tmp1->f1=0,_Tmp1->f2=1,_Tmp1->f3=dles;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}}struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;};
# 746
void*Cyc_Absyn_tuple_type(struct Cyc_List_List*tqts){
struct Cyc_List_List*fs=0;
{int i=0;for(0;tqts!=0;(tqts=tqts->tl,++ i)){
struct _tuple14*_Tmp0=(struct _tuple14*)tqts->hd;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Tqual tq=_Tmp2;void*t=_Tmp1;
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=Cyc_Absyn_tuple_field_designator(i);void*_Tmp4;_Tmp4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp3)->f1;{struct _fat_ptr*n=_Tmp4;
struct Cyc_Absyn_Aggrfield*af;af=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),af->name=n,af->tq=tq,af->type=t,af->width=0,af->attributes=0,af->requires_clause=0;
fs=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=af,_Tmp5->tl=fs;_Tmp5;});}}}}
# 754
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp0->tag=6,_Tmp0->f1=0U,_Tmp0->f2=1,({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(fs);_Tmp0->f3=_Tmp1;});_Tmp0;});}
# 756
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_Tmp1->tag=36,_Tmp1->f1=s;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 759
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp1->tag=38,_Tmp1->f1=t;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 763
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr tmpl,struct Cyc_List_List*outs,struct Cyc_List_List*ins,struct Cyc_List_List*clobs,unsigned loc){
# 766
void*_Tmp0=(void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp1->tag=39,_Tmp1->f1=volatile_kw,_Tmp1->f2=tmpl,_Tmp1->f3=outs,_Tmp1->f4=ins,_Tmp1->f5=clobs;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 768
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct));_Tmp1->tag=40,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 771
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct));_Tmp1->tag=41,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 774
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct));_Tmp1->tag=42,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 778
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned loc){
struct Cyc_List_List*dles=0;
for(1;es!=0;es=es->tl){
dles=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple13*_Tmp1=({struct _tuple13*_Tmp2=_cycalloc(sizeof(struct _tuple13));_Tmp2->f0=0,_Tmp2->f1=(struct Cyc_Absyn_Exp*)es->hd;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=dles;_Tmp0;});}{
void*_Tmp0=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp1->tag=25,({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(dles);_Tmp1->f1=_Tmp2;});_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}}
# 784
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned loc){
# 787
void*_Tmp0=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp1->tag=35,_Tmp1->f1=n,_Tmp1->f2=dles;_Tmp1;});return Cyc_Absyn_new_exp(_Tmp0,loc);}
# 790
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned loc){
struct Cyc_Absyn_Stmt*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Stmt));_Tmp0->r=s,_Tmp0->loc=loc,_Tmp0->annot=(void*)& Cyc_Absyn_EmptyAnnot_val;return _Tmp0;}
# 793
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
static struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct Cyc_Absyn_Break_s_val={6};
static struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct Cyc_Absyn_Continue_s_val={7};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Break_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Continue_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){void*_Tmp0=(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct));_Tmp1->tag=1,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned loc){void*_Tmp0=(void*)({struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct));_Tmp1->tag=3,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned loc){
if(ss==0)return Cyc_Absyn_skip_stmt(loc);
if(ss->tl==0)return(struct Cyc_Absyn_Stmt*)ss->hd;{
struct Cyc_Absyn_Stmt*_Tmp0=(struct Cyc_Absyn_Stmt*)ss->hd;struct Cyc_Absyn_Stmt*_Tmp1=Cyc_Absyn_seq_stmts(ss->tl,loc);return Cyc_Absyn_seq_stmt(_Tmp0,_Tmp1,loc);}}struct _tuple15{void*f0;void*f1;};
# 806
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=s1->r,_Tmp1.f1=s2->r;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return s2;else{if(*((int*)_Tmp0.f1)==0)
return s1;else{
void*_Tmp1=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_Tmp2->tag=2,_Tmp2->f1=s1,_Tmp2->f2=s2;_Tmp2;});return Cyc_Absyn_new_stmt(_Tmp1,loc);}};}
# 813
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct));_Tmp1->tag=4,_Tmp1->f1=e,_Tmp1->f2=s1,_Tmp1->f3=s2;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 816
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned loc){
# 823
s=({struct Cyc_Absyn_Stmt*_Tmp0=s;struct Cyc_Absyn_Stmt*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_uint_exp(1U,s->loc);Cyc_Absyn_exp_stmt(_Tmp2,s->loc);});Cyc_Absyn_seq_stmt(_Tmp0,_Tmp1,s->loc);});{
void*_Tmp0=(void*)({struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct));_Tmp1->tag=5,_Tmp1->f1.f0=e,({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_skip_stmt(e->loc);_Tmp1->f1.f1=_Tmp2;}),_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}}
# 826
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct));_Tmp1->tag=9,_Tmp1->f1=e1,_Tmp1->f2.f0=e2,({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_skip_stmt(e3->loc);_Tmp1->f2.f1=_Tmp2;}),
_Tmp1->f3.f0=e3,({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_skip_stmt(e3->loc);_Tmp1->f3.f1=_Tmp2;}),_Tmp1->f4=s;_Tmp1;});
# 827
return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 831
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct));_Tmp1->tag=14,_Tmp1->f1=s,_Tmp1->f2.f0=e,({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_skip_stmt(e->loc);_Tmp1->f2.f1=_Tmp2;});_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 834
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct));_Tmp1->tag=10,_Tmp1->f1=e,_Tmp1->f2=scs,_Tmp1->f3=0;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 837
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct));_Tmp1->tag=15,_Tmp1->f1=s,_Tmp1->f2=scs,_Tmp1->f3=0;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 840
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct));_Tmp1->tag=11,_Tmp1->f1=el,_Tmp1->f2=0;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 843
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*lab,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct));_Tmp1->tag=8,_Tmp1->f1=lab;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 846
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _fat_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp1->tag=13,_Tmp1->f1=v,_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 849
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 852
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned loc){
struct Cyc_Absyn_Decl*d=({void*_Tmp0=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Vardecl*_Tmp2=Cyc_Absyn_new_vardecl(0U,x,t,init,0);_Tmp1->f1=_Tmp2;});_Tmp1;});Cyc_Absyn_new_decl(_Tmp0,loc);});
void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,loc);}
# 856
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned loc){
struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_assign_exp(e1,e2,loc);return Cyc_Absyn_exp_stmt(_Tmp0,loc);}
# 860
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned s){struct Cyc_Absyn_Pat*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Pat));_Tmp0->r=p,_Tmp0->topt=0,_Tmp0->loc=s;return _Tmp0;}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){void*_Tmp0=(void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct));_Tmp1->tag=16,_Tmp1->f1=e;_Tmp1;});return Cyc_Absyn_new_pat(_Tmp0,e->loc);}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={8};
# 866
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned loc){struct Cyc_Absyn_Decl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp0->r=r,_Tmp0->loc=loc;return _Tmp0;}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct));_Tmp1->tag=2,_Tmp1->f1=p,_Tmp1->f2=0,_Tmp1->f3=e,_Tmp1->f4=0;_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 870
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct));_Tmp1->tag=3,_Tmp1->f1=vds;_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 873
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*open_exp,unsigned loc){
void*_Tmp0=(void*)({struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct));_Tmp1->tag=4,_Tmp1->f1=tv,_Tmp1->f2=vd,_Tmp1->f3=open_exp;_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 876
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e,unsigned loc){
# 878
void*_Tmp0=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct));_Tmp1->tag=2,({struct Cyc_Absyn_Pat*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp4->tag=2,_Tmp4->f1=tv,_Tmp4->f2=vd;_Tmp4;});Cyc_Absyn_new_pat(_Tmp3,loc);});_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0,_Tmp1->f3=e,_Tmp1->f4=0;_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 881
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Exp*rename){
struct Cyc_Absyn_Vardecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_Tmp0->sc=2U,_Tmp0->name=x,_Tmp0->varloc=varloc,({
struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);_Tmp0->tq=_Tmp1;}),_Tmp0->type=t,_Tmp0->initializer=init,_Tmp0->rgn=0,_Tmp0->attributes=0,_Tmp0->escapes=0,_Tmp0->is_proto=0,_Tmp0->rename=rename;return _Tmp0;}
# 887
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*ans=Cyc_Absyn_new_vardecl(0U,x,t,init,0);
ans->sc=0U;
return ans;}
# 892
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*ec,struct Cyc_List_List*qb,struct Cyc_List_List*fs,int tagged){
# 898
struct Cyc_Absyn_AggrdeclImpl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_AggrdeclImpl));_Tmp0->exist_vars=exists,_Tmp0->qual_bnd=qb,_Tmp0->fields=fs,_Tmp0->tagged=tagged,_Tmp0->effconstr=ec;return _Tmp0;}
# 901
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 904
void*_Tmp0=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp1->tag=5,({struct Cyc_Absyn_Aggrdecl*_Tmp2=({struct Cyc_Absyn_Aggrdecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp3->kind=k,_Tmp3->sc=s,_Tmp3->name=n,_Tmp3->tvs=ts,_Tmp3->impl=i,_Tmp3->attributes=atts,_Tmp3->expected_mem_kind=0;_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 909
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 912
struct Cyc_Absyn_TypeDecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=0,({struct Cyc_Absyn_Aggrdecl*_Tmp3=({struct Cyc_Absyn_Aggrdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp4->kind=k,_Tmp4->sc=s,_Tmp4->name=n,_Tmp4->tvs=ts,_Tmp4->impl=i,_Tmp4->attributes=atts,_Tmp4->expected_mem_kind=0;_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->r=_Tmp1;}),_Tmp0->loc=loc;return _Tmp0;}
# 918
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 921
return Cyc_Absyn_aggr_decl(0U,s,n,ts,i,atts,loc);}
# 923
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned loc){
# 926
return Cyc_Absyn_aggr_decl(1U,s,n,ts,i,atts,loc);}
# 928
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 931
void*_Tmp0=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp1->tag=6,({struct Cyc_Absyn_Datatypedecl*_Tmp2=({struct Cyc_Absyn_Datatypedecl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp3->sc=s,_Tmp3->name=n,_Tmp3->tvs=ts,_Tmp3->fields=fs,_Tmp3->is_extensible=is_extensible;_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;});return Cyc_Absyn_new_decl(_Tmp0,loc);}
# 934
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned loc){
# 937
struct Cyc_Absyn_TypeDecl*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));({void*_Tmp1=(void*)({struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=2,({struct Cyc_Absyn_Datatypedecl*_Tmp3=({struct Cyc_Absyn_Datatypedecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp4->sc=s,_Tmp4->name=n,_Tmp4->tvs=ts,_Tmp4->fields=fs,_Tmp4->is_extensible=is_extensible;_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->r=_Tmp1;}),_Tmp0->loc=loc;return _Tmp0;}
# 949 "absyn.cyc"
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_type,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*effc,struct Cyc_List_List*qb,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens,struct Cyc_Absyn_Exp*thrws){
# 957
{struct Cyc_List_List*args2=args;for(0;args2!=0;args2=args2->tl){
({void*_Tmp0=Cyc_Absyn_pointer_expand((*((struct _tuple8*)args2->hd)).f2,1);(*((struct _tuple8*)args2->hd)).f2=_Tmp0;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp0->tag=5,_Tmp0->f1.tvars=tvs,_Tmp0->f1.ret_tqual=ret_tqual,({
# 961
void*_Tmp1=Cyc_Absyn_pointer_expand(ret_type,0);_Tmp0->f1.ret_type=_Tmp1;}),_Tmp0->f1.effect=eff_type,_Tmp0->f1.args=args,_Tmp0->f1.c_varargs=c_varargs,_Tmp0->f1.cyc_varargs=cyc_varargs,_Tmp0->f1.qual_bnd=qb,_Tmp0->f1.attributes=atts,_Tmp0->f1.requires_clause=req,_Tmp0->f1.requires_assn=0,_Tmp0->f1.ensures_clause=ens,_Tmp0->f1.ensures_assn=0,_Tmp0->f1.throws_clause=thrws,_Tmp0->f1.throws_assn=0,_Tmp0->f1.arg_vardecls=0,_Tmp0->f1.return_value=0,_Tmp0->f1.effconstr=effc;_Tmp0;});}
# 982
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==5){
# 985
void*rtyp=fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=& Cyc_Kinds_ek;_Tmp1;}),0): Cyc_Absyn_heap_rgn_type;
void*_Tmp1=t;void*_Tmp2=rtyp;void*_Tmp3=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp4=Cyc_Absyn_empty_tqual(0U);void*_Tmp5=Cyc_Absyn_false_type;return Cyc_Absyn_at_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);}else{
return t;};}
# 1001 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 2:
 return 0;case 1: _Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LL6;}case 4: _Tmp1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1006
void*_Tmp2=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp2)==4)
return 0;else{
return 1;};}default:
# 1010
 goto _LLA;}case 22: _LLA:
 goto _LLC;case 20: _LLC:
 goto _LLE;case 23: _LLE:
 return 1;case 21: _Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_is_lvalue(e1);}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_is_lvalue(e1);}case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_is_lvalue(e1);}default:
 return 0;};}
# 1021
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _fat_ptr*v){
{struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fs->hd)->name,v)==0)
return(struct Cyc_Absyn_Aggrfield*)fs->hd;}}
return 0;}
# 1027
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*v){
if(ad->impl==0)return 0;else{return Cyc_Absyn_lookup_field(ad->impl->fields,v);}}
# 1031
struct _tuple14*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(1;i!=0 && ts!=0;(-- i,ts=ts->tl)){
;}
if(ts==0)return 0;else{return(struct _tuple14*)ts->hd;}}
# 1037
struct _fat_ptr*Cyc_Absyn_decl_name(struct Cyc_Absyn_Decl*decl){
void*_Tmp0=decl->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp1;
return(*x->name).f1;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp1;
return(*x->name).f1;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp1;
return(*x->name).f1;}case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
return(*x->name).f1;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp1;
return(*x->name).f1;}case 13:
 goto _LLE;case 14: _LLE:
 goto _LL10;case 15: _LL10:
 goto _LL12;case 16: _LL12:
 goto _LL14;case 2: _LL14:
 goto _LL16;case 6: _LL16:
 goto _LL18;case 3: _LL18:
 goto _LL1A;case 9: _LL1A:
 goto _LL1C;case 10: _LL1C:
 goto _LL1E;case 11: _LL1E:
 goto _LL20;case 12: _LL20:
 goto _LL22;default: _LL22:
 return 0;};}
# 1060
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name){
for(1;decls!=0;decls=decls->tl){
struct _fat_ptr*dname=Cyc_Absyn_decl_name((struct Cyc_Absyn_Decl*)decls->hd);
if((unsigned)dname && !Cyc_strptrcmp(dname,name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 1066
return 0;}static char _TmpG5[3U]="f0";
# 1069
struct _fat_ptr*Cyc_Absyn_fieldname(int i){
# 1071
static struct _fat_ptr f0={_TmpG5,_TmpG5,_TmpG5 + 3U};
static struct _fat_ptr*field_names_v[1U]={& f0};
static struct _fat_ptr field_names={(void*)field_names_v,(void*)field_names_v,(void*)(field_names_v + 1U)};
unsigned fsz=_get_fat_size(field_names,sizeof(struct _fat_ptr*));
if((unsigned)i >= fsz)
field_names=({unsigned _Tmp0=(unsigned)(i + 1);
_tag_fat(({struct _fat_ptr**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct _fat_ptr*)));({{unsigned _Tmp2=(unsigned)(i + 1);unsigned j;for(j=0;j < _Tmp2;++ j){j < fsz?_Tmp1[j]=*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),(int)j)):({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)j);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("f%d",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});*_Tmp4=_Tmp5;});_Tmp4;});_Tmp1[j]=_Tmp3;});}}0;});_Tmp1;}),sizeof(struct _fat_ptr*),_Tmp0);});
# 1079
return*((struct _fat_ptr**)_check_fat_subscript(field_names,sizeof(struct _fat_ptr*),i));}struct _tuple16{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;};
# 1082
struct _tuple16 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo info){
void*_Tmp0;enum Cyc_Absyn_AggrKind _Tmp1;if(info.UnknownAggr.tag==1){_Tmp1=info.UnknownAggr.val.f0;_Tmp0=info.UnknownAggr.val.f1;{enum Cyc_Absyn_AggrKind ak=_Tmp1;struct _tuple0*n=_Tmp0;
struct _tuple16 _Tmp2;_Tmp2.f0=ak,_Tmp2.f1=n;return _Tmp2;}}else{_Tmp1=(*info.KnownAggr.val)->kind;_Tmp0=(*info.KnownAggr.val)->name;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct _tuple0*n=_Tmp0;
struct _tuple16 _Tmp2;_Tmp2.f0=k,_Tmp2.f1=n;return _Tmp2;}};}
# 1088
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo info){
void*_Tmp0;if(info.UnknownAggr.tag==1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("unchecked aggrdecl",sizeof(char),19U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp3;})(_tag_fat(_Tmp2,sizeof(void*),1));});else{_Tmp0=*info.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
return ad;}};}
# 1094
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 6: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1==Cyc_Absyn_UnionA){_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_List_List*fs=_Tmp2;
# 1097
return fs==0 ||((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause==0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
# 1099
int _Tmp3;enum Cyc_Absyn_AggrKind _Tmp4;void*_Tmp5;if(info.KnownAggr.tag==2){_Tmp5=*info.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp5;
# 1101
if((int)ad->kind!=1)return 0;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if(_check_null(impl)->tagged)return 0;{
struct Cyc_List_List*fields=impl->fields;
return fields==0 ||((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause==0;}}}}else{if(info.UnknownAggr.val.f2==0){_Tmp4=info.UnknownAggr.val.f0;{enum Cyc_Absyn_AggrKind k=_Tmp4;
return(int)k==1;}}else{_Tmp4=info.UnknownAggr.val.f0;_Tmp3=(int)info.UnknownAggr.val.f2->v;{enum Cyc_Absyn_AggrKind k=_Tmp4;int b=_Tmp3;
return(int)k==1 && !b;}}};}}else{goto _LL5;}default: _LL5:
# 1109
 return 0;};}
# 1112
int Cyc_Absyn_is_require_union_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);union Cyc_Absyn_AggrInfo _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 6: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1==Cyc_Absyn_UnionA){_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_List_List*fs=_Tmp2;
# 1115
return fs!=0 &&((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause!=0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp1;
# 1117
enum Cyc_Absyn_AggrKind _Tmp3;void*_Tmp4;if(info.KnownAggr.tag==2){_Tmp4=*info.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1119
if((int)ad->kind!=1)return 0;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if(_check_null(impl)->tagged)return 0;{
struct Cyc_List_List*fields=impl->fields;
return fields!=0 &&((struct Cyc_Absyn_Aggrfield*)fields->hd)->requires_clause!=0;}}}}else{_Tmp3=info.UnknownAggr.val.f0;{enum Cyc_Absyn_AggrKind k=_Tmp3;
return 0;}};}}else{goto _LL5;}default: _LL5:
# 1126
 return 0;};}
# 1130
struct _tuple0*Cyc_Absyn_binding2qvar(void*b){
void*_Tmp0;switch(*((int*)b)){case 0: _Tmp0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)b)->f1;{struct _tuple0*qv=_Tmp0;
return qv;}case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
_Tmp0=vd;goto _LL6;}case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp0;
_Tmp0=vd;goto _LL8;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_Tmp0;
_Tmp0=vd;goto _LLA;}case 5: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd->name;}default: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp0;
return fd->name;}};}
# 1141
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*ds){
if(ds==0 || ds->tl!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("designator list not of length 1",sizeof(char),32U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});{
void*_Tmp0=(void*)ds->hd;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp0)->f1;{struct _fat_ptr*f=_Tmp1;
return f;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("array designator in struct",sizeof(char),27U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}}
# 1150
int Cyc_Absyn_type2bool(int def,void*t){
void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 11:
 return 1;case 12:
 return 0;default: goto _LL5;}else{_LL5:
 return def;};}
# 1160
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct _tuple17{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
void Cyc_Absyn_visit_exp(int(*f1)(void*,struct Cyc_Absyn_Exp*),int(*f2)(void*,struct Cyc_Absyn_Stmt*),void*env,struct Cyc_Absyn_Exp*e){
if(!f1(env,e))
return;{
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 31: _LL8:
 goto _LLA;case 32: _LLA:
 goto _LLC;case 38: _LLC:
 goto _LLE;case 19: _LLE:
 goto _LL10;case 17: _LL10:
 goto _LL0;case 40: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp3;
# 1174
_Tmp3=e1;goto _LL14;}case 41: _Tmp3=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL16;}case 42: _Tmp3=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL18;}case 5: _Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1A;}case 11: _Tmp3=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1C;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1E;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL20;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL22;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL24;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL26;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL28;}case 27: _Tmp3=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL2A;}case 37: _Tmp3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL2C;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL2E;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp3;
Cyc_Absyn_visit_exp(f1,f2,env,e1);return;}case 4: _Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 1190
_Tmp3=e1;_Tmp2=e2;goto _LL32;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL34;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL36;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL38;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL38: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL3A;}case 34: _Tmp3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL3A: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL3C;}case 26: _Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL3C: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 1197
Cyc_Absyn_visit_exp(f1,f2,env,e1);Cyc_Absyn_visit_exp(f1,f2,env,e2);return;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 1200
Cyc_Absyn_visit_exp(f1,f2,env,e1);Cyc_Absyn_visit_exp(f1,f2,env,e2);Cyc_Absyn_visit_exp(f1,f2,env,e3);
return;}case 10: _Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_List_List*lexp=_Tmp2;
# 1204
for(1;lexp!=0;lexp=lexp->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(struct Cyc_Absyn_Exp*)lexp->hd);}
Cyc_Absyn_visit_exp(f1,f2,env,e1);
goto _LL0;}case 25: _Tmp3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*ldt=_Tmp3;
# 1209
_Tmp3=ldt;goto _LL44;}case 24: _Tmp3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL44: {struct Cyc_List_List*ldt=_Tmp3;
_Tmp3=ldt;goto _LL46;}case 35: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL46: {struct Cyc_List_List*ldt=_Tmp3;
_Tmp3=ldt;goto _LL48;}case 28: _Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL48: {struct Cyc_List_List*ldt=_Tmp3;
_Tmp3=ldt;goto _LL4A;}case 29: _Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL4A: {struct Cyc_List_List*ldt=_Tmp3;
# 1214
for(1;ldt!=0;ldt=ldt->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(*((struct _tuple13*)ldt->hd)).f1);}
goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*lexp=_Tmp3;
# 1218
_Tmp3=lexp;goto _LL4E;}case 30: _Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL4E: {struct Cyc_List_List*lexp=_Tmp3;
# 1220
for(1;lexp!=0;lexp=lexp->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(struct Cyc_Absyn_Exp*)lexp->hd);}
goto _LL0;}case 33: _Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;{struct Cyc_Absyn_Exp*e1o=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3o=_Tmp1;
# 1224
_Tmp3=e1o;_Tmp2=e2;_Tmp1=e3o;goto _LL52;}case 16: _Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL52: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 1228
if(e1!=0)Cyc_Absyn_visit_exp(f1,f2,env,e1);
if(e3!=0)Cyc_Absyn_visit_exp(f1,f2,env,e3);
Cyc_Absyn_visit_exp(f1,f2,env,e2);
goto _LL0;}case 36: _Tmp3=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
# 1233
Cyc_Absyn_visit_stmt(f1,f2,env,s);goto _LL0;}default: _Tmp3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_List_List*sl1=_Tmp3;struct Cyc_List_List*sl2=_Tmp2;
# 1236
for(1;sl1!=0;sl1=sl1->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(*((struct _tuple17*)sl1->hd)).f1);}
for(1;sl2!=0;sl2=sl2->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(*((struct _tuple17*)sl2->hd)).f1);}
goto _LL0;}}_LL0:;}}
# 1243
static void Cyc_Absyn_visit_scs(int(*f1)(void*,struct Cyc_Absyn_Exp*),int(*f2)(void*,struct Cyc_Absyn_Stmt*),void*env,struct Cyc_List_List*scs){
# 1245
for(1;scs!=0;scs=scs->tl){
if(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause!=0)
Cyc_Absyn_visit_exp(f1,f2,env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause);
Cyc_Absyn_visit_stmt(f1,f2,env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}}
# 1251
void Cyc_Absyn_visit_stmt(int(*f1)(void*,struct Cyc_Absyn_Exp*),int(*f2)(void*,struct Cyc_Absyn_Stmt*),void*env,struct Cyc_Absyn_Stmt*s){
if(!f2(env,s))
return;{
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 goto _LLA;case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){_LLA:
 goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
_Tmp4=e;goto _LLE;}}case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyn_visit_exp(f1,f2,env,e);goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
Cyc_Absyn_visit_exp(f1,f2,env,e1);_Tmp4=s1;_Tmp3=s2;goto _LL12;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
Cyc_Absyn_visit_stmt(f1,f2,env,s1);Cyc_Absyn_visit_stmt(f1,f2,env,s2);goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
_Tmp4=s1;_Tmp3=e;goto _LL16;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_LL16: {struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Absyn_visit_exp(f1,f2,env,e);Cyc_Absyn_visit_stmt(f1,f2,env,s1);goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 1267
Cyc_Absyn_visit_exp(f1,f2,env,e1);Cyc_Absyn_visit_exp(f1,f2,env,e2);Cyc_Absyn_visit_exp(f1,f2,env,e3);
Cyc_Absyn_visit_stmt(f1,f2,env,s1);
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 1271
for(1;es!=0;es=es->tl){
Cyc_Absyn_visit_exp(f1,f2,env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1275
{void*_Tmp5=d->r;void*_Tmp6;switch(*((int*)_Tmp5)){case 0: _Tmp6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1277
if(vd->initializer!=0)
Cyc_Absyn_visit_exp(f1,f2,env,vd->initializer);
goto _LL23;}case 1: _Tmp6=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp6;
Cyc_Absyn_visit_stmt(f1,f2,env,fd->body);goto _LL23;}case 2: _Tmp6=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;
Cyc_Absyn_visit_exp(f1,f2,env,e);goto _LL23;}case 4: _Tmp6=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp5)->f3;{struct Cyc_Absyn_Exp*eo=_Tmp6;
if((unsigned)eo)Cyc_Absyn_visit_exp(f1,f2,env,eo);goto _LL23;}default:
 goto _LL23;}_LL23:;}
# 1285
Cyc_Absyn_visit_stmt(f1,f2,env,s1);
goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;
Cyc_Absyn_visit_stmt(f1,f2,env,s1);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
# 1289
Cyc_Absyn_visit_exp(f1,f2,env,e);
Cyc_Absyn_visit_scs(f1,f2,env,scs);
goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
# 1293
Cyc_Absyn_visit_stmt(f1,f2,env,s1);
Cyc_Absyn_visit_scs(f1,f2,env,scs);
goto _LL0;}}_LL0:;}}
# 1301
static int Cyc_Absyn_no_side_effects_f1(int*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 10:
 goto _LL4;case 4: _LL4:
 goto _LL6;case 34: _LL6:
 goto _LL8;case 39: _LL8:
 goto _LLA;case 36: _LLA:
 goto _LLC;case 5: _LLC:
*env=0;return 0;case 18:
 return 0;default:
 return 1;};}
# 1313
static int Cyc_Absyn_no_side_effects_f2(int*env,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Absyn::no_side_effects looking at a statement",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1316
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*e){
int ans=1;
({void(*_Tmp0)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_Absyn_no_side_effects_f1,Cyc_Absyn_no_side_effects_f2,& ans,e);
return ans;}struct _tuple18{struct _tuple0*f0;int f1;};
# 1324
static int Cyc_Absyn_var_may_appear_f1(struct _tuple18*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 1327
if(({struct _tuple0*_Tmp2=Cyc_Absyn_binding2qvar(b);Cyc_Absyn_qvar_cmp(_Tmp2,(*env).f0);})==0)
(*env).f1=1;
return 0;}case 39:
 goto _LL6;case 36: _LL6:
# 1332
(*env).f1=1;
return 0;case 26: _Tmp1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
return Cyc_Absyn_qvar_cmp(vd->name,(*env).f0)!=0;}default:
 return 1;};}
# 1338
static int Cyc_Absyn_var_may_appear_f2(struct _tuple18*env,struct Cyc_Absyn_Stmt*e){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Absyn::no_side_effects looking at a statement",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1341
int Cyc_Absyn_var_may_appear_exp(struct _tuple0*v,struct Cyc_Absyn_Exp*e){
struct _tuple18 env=({struct _tuple18 _Tmp0;_Tmp0.f0=v,_Tmp0.f1=0;_Tmp0;});
({void(*_Tmp0)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_Absyn_var_may_appear_f1,Cyc_Absyn_var_may_appear_f2,& env,e);
return env.f1;}struct Cyc_Absyn_NestedStmtEnv{void(*f)(void*,struct Cyc_Absyn_Stmt*);void*env;int szeof;};
# 1352
static int Cyc_Absyn_do_nested_stmt_f1(struct Cyc_Absyn_NestedStmtEnv*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;if(*((int*)_Tmp0)==18)
return env->szeof;else{
return 1;};}
# 1358
static int Cyc_Absyn_do_nested_stmt_f2(struct Cyc_Absyn_NestedStmtEnv*env,struct Cyc_Absyn_Stmt*s){
# 1360
env->f(env->env,s);
return 0;}
# 1363
void Cyc_Absyn_do_nested_statement(struct Cyc_Absyn_Exp*e,void*env,void(*f)(void*,struct Cyc_Absyn_Stmt*),int szeof){
struct Cyc_Absyn_NestedStmtEnv nested_env=({struct Cyc_Absyn_NestedStmtEnv _Tmp0;_Tmp0.f=f,_Tmp0.env=env,_Tmp0.szeof=szeof;_Tmp0;});
({void(*_Tmp0)(int(*)(struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Absyn_NestedStmtEnv*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_Absyn_do_nested_stmt_f1,Cyc_Absyn_do_nested_stmt_f2,& nested_env,e);}
# 1368
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={13};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={14};
struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val={15};
struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val={16};
