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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};
# 547 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 912 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 929
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 933
void*Cyc_Absyn_compress(void*);
# 937
int Cyc_Absyn_type2bool(int,void*);
# 968
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 1003
void*Cyc_Absyn_bounds_one (void);
# 1006
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
# 37
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
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
int Cyc_Unify_unify_c(void*,void*,int(*)(void*,void*,void*),void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 36
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 65 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Set_Set;
# 185 "assndef.h"
extern void*Cyc_AssnDef_fresh_var(void*);struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};
# 229 "assndef.h"
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};
# 279
extern void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*,struct Cyc_List_List*);
# 308
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{enum Cyc_Absyn_Coercion f0;enum Cyc_Absyn_Coercion f1;};
# 39 "subtype.cyc"
static enum Cyc_Absyn_Coercion Cyc_Subtype_join_coercion(enum Cyc_Absyn_Coercion c1,enum Cyc_Absyn_Coercion c2){
if((int)c1==(int)c2)return c1;{
struct _tuple11 _Tmp0=({struct _tuple11 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});enum Cyc_Absyn_Coercion _Tmp1;if(_Tmp0.f0==Cyc_Absyn_Unknown_coercion)
goto _LL4;else{if(_Tmp0.f1==Cyc_Absyn_Unknown_coercion){_LL4:
 return 0U;}else{if(_Tmp0.f0==Cyc_Absyn_No_coercion){_Tmp1=_Tmp0.f1;{enum Cyc_Absyn_Coercion o=_Tmp1;
_Tmp1=o;goto _LL8;}}else{if(_Tmp0.f1==Cyc_Absyn_No_coercion){_Tmp1=_Tmp0.f0;_LL8: {enum Cyc_Absyn_Coercion o=_Tmp1;
return o;}}else{switch((int)_Tmp0.f0){case Cyc_Absyn_Null_to_NonNull: if(_Tmp0.f1==Cyc_Absyn_Other_coercion)
goto _LLC;else{goto _LLD;}case Cyc_Absyn_Other_coercion: if(_Tmp0.f1==Cyc_Absyn_Null_to_NonNull){_LLC:
 return 2U;}else{goto _LLD;}default: _LLD:
 return 0U;}}}}};}}struct _tuple12{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;};
# 56
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*,void*,void*);
# 58
static struct _tuple12 Cyc_Subtype_effect_subtype(void*,void*,int);struct _tuple13{enum Cyc_Absyn_Coercion f0;struct Cyc_List_List*f1;int f2;};
static struct _tuple13 Cyc_Subtype_constraint_subtype(void*,void*,int(*)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int,int);
# 65
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*,int,void*);struct _tuple14{struct Cyc_List_List*f0;struct _RegionHandle*f1;int f2;};struct _tuple15{struct Cyc_Absyn_Tqual f0;void*f1;};
# 67
static struct Cyc_List_List*Cyc_Subtype_flatten_type_f(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*x){
# 70
struct _tuple14 _Tmp0=*env;int _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*inst=_Tmp3;struct _RegionHandle*r=_Tmp2;int flatten=_Tmp1;
void*t=inst==0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);
struct Cyc_List_List*ts=Cyc_Subtype_flatten_type(r,flatten,t);
if(Cyc_List_length(ts)==1){
struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp5=({struct _tuple15*_Tmp6=_region_malloc(r,0U,sizeof(struct _tuple15));_Tmp6->f0=x->tq,_Tmp6->f1=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;return _Tmp4;}
return ts;}}
# 78
static struct Cyc_List_List*Cyc_Subtype_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 80
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_Tmp0;void*_Tmp1;switch(*((int*)t1)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)){case 0:
 return 0;case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f1)->f1.KnownAggr.val;_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t1)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 86
if((((int)ad->kind==1 || ad->impl==0)|| ad->impl->exist_vars!=0)|| ad->impl->effconstr!=0){
# 88
struct Cyc_List_List*_Tmp2=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp3=({struct _tuple15*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple15));({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);_Tmp4->f0=_Tmp5;}),_Tmp4->f1=t1;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;return _Tmp2;}{
struct Cyc_List_List*inst=Cyc_List_rzip(r,r,ad->tvs,ts);
struct _tuple14 env=({struct _tuple14 _Tmp2;_Tmp2.f0=inst,_Tmp2.f1=r,_Tmp2.f2=flatten;_Tmp2;});
struct Cyc_List_List*_Tmp2=ad->impl->fields;void*_Tmp3;void*_Tmp4;if(_Tmp2==0)
return 0;else{_Tmp4=(struct Cyc_Absyn_Aggrfield*)_Tmp2->hd;_Tmp3=_Tmp2->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp4;struct Cyc_List_List*tl=_Tmp3;
# 94
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(& env,hd);
env.f2=0;{
struct Cyc_List_List*tl2=({struct Cyc_List_List*(*_Tmp5)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp5;})(r,Cyc_Subtype_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}}else{goto _LL7;}default: goto _LL7;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f1==Cyc_Absyn_StructA){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f3;{struct Cyc_List_List*fs=_Tmp1;
# 101
struct _tuple14 env=({struct _tuple14 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=r,_Tmp2.f2=flatten;_Tmp2;});
void*_Tmp2;void*_Tmp3;if(fs==0)
return 0;else{_Tmp3=(struct Cyc_Absyn_Aggrfield*)fs->hd;_Tmp2=fs->tl;{struct Cyc_Absyn_Aggrfield*hd=_Tmp3;struct Cyc_List_List*tl=_Tmp2;
# 105
struct Cyc_List_List*hd2=Cyc_Subtype_flatten_type_f(& env,hd);
env.f2=0;{
struct Cyc_List_List*tl2=({struct Cyc_List_List*(*_Tmp4)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp4;})(r,Cyc_Subtype_flatten_type_f,& env,tl);
struct Cyc_List_List*tts;tts=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),tts->hd=hd2,tts->tl=tl2;
return Cyc_List_rflatten(r,tts);}}};}}else{goto _LL7;}default: _LL7:
# 111
 goto _LL0;}_LL0:;}}{
# 114
struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp1=({struct _tuple15*_Tmp2=_region_malloc(r,0U,sizeof(struct _tuple15));({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual(0U);_Tmp2->f0=_Tmp3;}),_Tmp2->f1=t1;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;return _Tmp0;}}struct _tuple16{void*f0;void*f1;};
# 119
int Cyc_Subtype_check_aqual_bounds(struct Cyc_List_List*aquals_bnd,void*aq,void*bnd){
# 121
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(bnd);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;enum Cyc_Absyn_AliasQualVal _Tmp2;enum Cyc_Absyn_AliasQualVal _Tmp3;if(*((int*)_Tmp0.f0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 16: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp2;
# 123
return(int)v_sup==(int)v_sub ||(int)v_sup==3;}}else{goto _LL7;}}else{goto _LL7;}case 17: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_List_List*tv_sub=_Tmp1;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 125
return Cyc_Subtype_check_aqual_bounds(aquals_bnd,(void*)_check_null(_check_null(tv_sub)->tl)->hd,bnd);}}else{goto _LL7;}}else{goto _LL7;}case 15: if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==16){_Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sup=_Tmp3;
# 127
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*aq_bound=Cyc_List_assoc_cmp(Cyc_Tcutil_typecmp,aquals_bnd,aq);
int _Tmp6=Cyc_Subtype_check_aqual_bounds(aquals_bnd,aq_bound,bnd);_npop_handler(0);return _Tmp6;}
# 128
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found)
# 132
return 0;else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}};}}}}else{goto _LL7;}}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 135
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("check_aqual_bounds expects a constant bound; got ",sizeof(char),50U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aq;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(",",sizeof(char),2U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)bnd;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),4));});};}
# 143
int Cyc_Subtype_alias_qual_subtype(void*aq_sub,void*aq_sup){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(aq_sub);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(aq_sup);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_AliasQualVal _Tmp5;enum Cyc_Absyn_AliasQualVal _Tmp6;if(*((int*)_Tmp0.f0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 16: if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 16: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp5=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp5;
# 146
return((int)v_sup==(int)v_sub ||(int)v_sup==3)||
(int)v_sup==0 &&(((int)v_sub==1 ||(int)v_sub==2)||(int)v_sub==3);}case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 2: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 168
return 0;}case 1: _Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{enum Cyc_Absyn_AliasQualVal v_sub=_Tmp6;struct Cyc_Core_Opt*k=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;
# 170
return Cyc_Kinds_kind_eq((struct Cyc_Absyn_Kind*)_check_null(k)->v,& Cyc_Kinds_aqk)&& Cyc_Subtype_alias_qual_subtype(aq_sub,(void*)_check_null(bnd)->hd);}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{goto _LL15;}case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)){case 2: if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_Absyn_Tvar*tv2=_Tmp3;
# 149
return tv2->identity==tv1->identity;}case 1: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 154
return({struct Cyc_Absyn_Kind*_Tmp7=Cyc_Kinds_tvar_kind(tv1,& Cyc_Kinds_aqk);Cyc_Kinds_kind_eq(_Tmp7,(struct Cyc_Absyn_Kind*)_check_null(k)->v);})&&({void*_Tmp7=(void*)_check_null(bnd1)->hd;Cyc_Subtype_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd);});}default: goto _LL15;}else{goto _LL15;}case 16: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 163
return Cyc_Subtype_alias_qual_subtype((void*)_check_null(bnd)->hd,aq_sup);}default: goto _LL15;}else{goto _LL15;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2)->hd)->f2)->tl==0){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==17){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)->f2)->tl==0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f2->hd;_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f2->hd;{void*tv1=_Tmp4;void*tv2=_Tmp3;
# 152
return Cyc_Tcutil_typecmp(tv1,tv2)==0;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}}else{goto _LL15;}case 1: if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0)switch(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2)->hd)){case 2: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Absyn_Tvar*tv2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 156
return({struct Cyc_Absyn_Kind*_Tmp7=(struct Cyc_Absyn_Kind*)_check_null(k1)->v;Cyc_Kinds_kind_eq(_Tmp7,Cyc_Kinds_tvar_kind(tv2,& Cyc_Kinds_aqk));})&&({void*_Tmp7=(void*)_check_null(bnd2)->hd;Cyc_Subtype_check_aqual_bounds(0,_Tmp7,(void*)_check_null(bnd1)->hd);});}case 1: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->tl;{struct Cyc_Core_Opt*k1=_Tmp4;struct Cyc_List_List*bnd1=_Tmp3;struct Cyc_Core_Opt*k2=_Tmp2;struct Cyc_List_List*bnd2=_Tmp1;
# 158
return({struct Cyc_Absyn_Kind*_Tmp7=(struct Cyc_Absyn_Kind*)_check_null(k1)->v;Cyc_Kinds_kind_eq(_Tmp7,(struct Cyc_Absyn_Kind*)_check_null(k2)->v);})&&({void*_Tmp7=(void*)_check_null(bnd1)->hd;Cyc_Subtype_alias_qual_subtype(_Tmp7,(void*)_check_null(bnd2)->hd);});}default: goto _LL15;}else{goto _LL15;}case 16: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->tl;_Tmp6=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;{struct Cyc_Core_Opt*k=_Tmp4;struct Cyc_List_List*bnd=_Tmp3;enum Cyc_Absyn_AliasQualVal v_sup=_Tmp6;
# 165
return Cyc_Kinds_kind_eq((struct Cyc_Absyn_Kind*)_check_null(k)->v,& Cyc_Kinds_aqk)&& Cyc_Subtype_check_aqual_bounds(0,aq_sup,(void*)_check_null(bnd)->hd);}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{goto _LL15;}default: goto _LL15;}else{_LL15:
# 172
 return 0;};}
# 176
static int Cyc_Subtype_isomorphic(void*t1,void*t2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});enum Cyc_Absyn_Size_of _Tmp1;enum Cyc_Absyn_Size_of _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==1){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){_Tmp2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f2;_Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f2;{enum Cyc_Absyn_Size_of b1=_Tmp2;enum Cyc_Absyn_Size_of b2=_Tmp1;
# 179
return((int)b1==(int)b2 ||(int)b1==2 &&(int)b2==3)||
(int)b1==3 &&(int)b2==2;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 188
static struct _tuple12 Cyc_Subtype_effect_inst_subtype(struct Cyc_List_List*l1,struct Cyc_List_List*l2){
enum Cyc_Absyn_Coercion subtypes=1U;
struct Cyc_List_List*retc=0;
for(1;(l1!=0 && l2!=0)&&(int)subtypes;(l1=l1->tl,l2=l2->tl)){
if(Cyc_Tcutil_type_kind((void*)l1->hd)==& Cyc_Kinds_ek && Cyc_Tcutil_type_kind((void*)l2->hd)==& Cyc_Kinds_ek)
subtypes=(enum Cyc_Absyn_Coercion)Cyc_Tcutil_subset_effect(0,(void*)l1->hd,(void*)l2->hd);else{
# 196
struct _tuple12 _Tmp0=Cyc_Subtype_effect_subtype((void*)l1->hd,(void*)l2->hd,0);void*_Tmp1;enum Cyc_Absyn_Coercion _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{enum Cyc_Absyn_Coercion s=_Tmp2;struct Cyc_List_List*c=_Tmp1;
subtypes=s;
retc=Cyc_List_imp_append(retc,c);}}}
# 201
if(l1==0 ^ l2==0)
subtypes=0U;{
struct _tuple12 _Tmp0;_Tmp0.f0=subtypes,_Tmp0.f1=retc;return _Tmp0;}}
# 207
static struct _tuple12 Cyc_Subtype_effect_subtype(void*t1,void*t2,int top_level){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;switch(*((int*)_Tmp0.f0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownAggr.tag==2){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownAggr.tag==2){_TmpE=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownAggr.val;_TmpD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpC=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownAggr.val;_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Aggrdecl*ad1=_TmpE;struct Cyc_List_List*l1=_TmpD;struct Cyc_Absyn_Aggrdecl*ad2=_TmpC;struct Cyc_List_List*l2=_TmpB;
# 210
if(Cyc_Absyn_qvar_cmp(ad1->name,ad2->name)==0)return Cyc_Subtype_effect_inst_subtype(l1,l2);else{struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatype.tag==2){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.tag==2){_TmpE=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatype.val;_TmpD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpC=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.val;_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_TmpE;struct Cyc_List_List*l1=_TmpD;struct Cyc_Absyn_Datatypedecl*dd2=_TmpC;struct Cyc_List_List*l2=_TmpB;
# 212
if(Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)==0)return Cyc_Subtype_effect_inst_subtype(l1,l2);else{struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.tag==2){if(*((int*)_Tmp0.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.tag==2){_TmpE=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f0;_TmpD=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f1;_TmpC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpB=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.val.f0;_TmpA=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatypefield.val.f1;_Tmp9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_TmpE;struct Cyc_Absyn_Datatypefield*df1=_TmpD;struct Cyc_List_List*l1=_TmpC;struct Cyc_Absyn_Datatypedecl*dd2=_TmpB;struct Cyc_Absyn_Datatypefield*df2=_TmpA;struct Cyc_List_List*l2=_Tmp9;
# 215
if(Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)==0 && Cyc_Absyn_qvar_cmp(df1->name,df2->name)==0)return Cyc_Subtype_effect_inst_subtype(l1,l2);else{struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.tag==2){_TmpE=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f0;_TmpD=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f1;_TmpC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpB=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.val;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_TmpE;struct Cyc_Absyn_Datatypefield*df1=_TmpD;struct Cyc_List_List*l1=_TmpC;struct Cyc_Absyn_Datatypedecl*dd2=_TmpB;struct Cyc_List_List*l2=_TmpA;
# 218
if(top_level &&(dd1==dd2 || Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)==0)){
struct _tuple12 _TmpF=Cyc_Subtype_effect_inst_subtype(l1,l2);void*_Tmp10;enum Cyc_Absyn_Coercion _Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{enum Cyc_Absyn_Coercion s=_Tmp11;struct Cyc_List_List*c=_Tmp10;
if((int)s!=0){
struct _tuple12 _Tmp12;_Tmp12.f0=3U,_Tmp12.f1=c;return _Tmp12;}}}{
# 224
struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}default: goto _LLD;}else{goto _LLD;}}else{goto _LLD;}default: goto _LLD;}case 8: if(*((int*)_Tmp0.f1)==8){_TmpE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f0)->f1;_TmpD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f1;_TmpB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct _tuple0*n1=_TmpE;struct Cyc_List_List*l1=_TmpD;struct _tuple0*n2=_TmpC;struct Cyc_List_List*l2=_TmpB;
# 226
if(Cyc_Absyn_qvar_cmp(n1,n2)==0)return Cyc_Subtype_effect_inst_subtype(l1,l2);else{struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}case 4: if(*((int*)_Tmp0.f1)==4){_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t_a=_TmpE;struct Cyc_Absyn_Tqual q_a=_Tmp8;void*eff_a=_TmpD;void*null_a=_TmpC;void*b_a=_TmpB;void*zt_a=_TmpA;void*aq_a=_Tmp9;void*t_b=_Tmp7;struct Cyc_Absyn_Tqual q_b=_Tmp6;void*eff_b=_Tmp5;void*null_b=_Tmp4;void*b_b=_Tmp3;void*zt_b=_Tmp2;void*aq_b=_Tmp1;
# 229
if(Cyc_Tcutil_subset_effect(0,eff_a,eff_b)){
struct _tuple12 _TmpF=Cyc_Subtype_effect_subtype(t_a,t_b,0);void*_Tmp10;enum Cyc_Absyn_Coercion _Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{enum Cyc_Absyn_Coercion s=_Tmp11;struct Cyc_List_List*c=_Tmp10;
if((int)s!=0){
if(((Cyc_Absyn_equal_tqual(q_a,q_b)&& Cyc_Unify_unify(null_a,null_b))&&
 Cyc_Unify_unify(zt_a,zt_b))&& Cyc_Unify_unify(aq_a,aq_b)){
if(Cyc_Unify_unify(b_a,b_b)){
struct _tuple12 _Tmp12;_Tmp12.f0=1U,_Tmp12.f1=c;return _Tmp12;}else{
# 238
void*cba=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_a);
void*cbb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);
if((unsigned)cba &&(unsigned)cbb){
struct _tuple12 _Tmp12;_Tmp12.f0=1U,({struct Cyc_List_List*_Tmp13=({struct Cyc_List_List*_Tmp14=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp15=Cyc_BansheeIf_equality_constraint(cba,cbb);_Tmp14->hd=_Tmp15;}),_Tmp14->tl=c;_Tmp14;});_Tmp12.f1=_Tmp13;});return _Tmp12;}}}}}}{
# 247
struct _tuple12 _TmpF;_TmpF.f0=0U,_TmpF.f1=0;return _TmpF;}}}else{goto _LLD;}default: _LLD: {
# 249
struct _tuple12 _TmpF;Cyc_Tcutil_typecmp(t1,t2)==0?_TmpF.f0=1U:(_TmpF.f0=0U),_TmpF.f1=0;return _TmpF;}};}
# 253
static struct _tuple12 Cyc_Subtype_force_equivalence(void*,void*);
# 255
static struct _tuple12 Cyc_Subtype_force_pointer_equivalence(void*t1,void*t2){
# 257
struct Cyc_List_List*retc=0;
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;struct Cyc_Absyn_Tqual _TmpF;void*_Tmp10;if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.autoreleased;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t1a=_Tmp10;struct Cyc_Absyn_Tqual tqual1a=_TmpF;void*eff1=_TmpE;void*null1a=_TmpD;void*b1=_TmpC;void*zt1=_TmpB;void*rel1=_TmpA;void*aq1=_Tmp9;void*t2a=_Tmp8;struct Cyc_Absyn_Tqual tqual2a=_Tmp7;void*eff2=_Tmp6;void*null2a=_Tmp5;void*b2=_Tmp4;void*zt2=_Tmp3;void*rel2=_Tmp2;void*aq2=_Tmp1;
# 261
if(Cyc_Unify_unify(b1,b2)){
# 266
if(((((Cyc_Absyn_equal_tqual(tqual1a,tqual2a)&& Cyc_Unify_unify(eff1,eff2))&&
 Cyc_Unify_unify(null1a,null2a))&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Unify_unify(rel1,rel2))&& Cyc_Unify_unify(aq1,aq2)){
if(Cyc_Unify_unify(t1a,t2a)){
struct _tuple12 _Tmp11;_Tmp11.f0=1U,_Tmp11.f1=0;return _Tmp11;}
return Cyc_Subtype_force_equivalence(t1a,t2a);}{
# 274
struct _tuple12 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0;return _Tmp11;}}else{
# 277
void*bb1=Cyc_Tcutil_ptrbnd_cvar_equivalent(b1);
void*bb2=Cyc_Tcutil_ptrbnd_cvar_equivalent(b2);
if((unsigned)bb1 &&(unsigned)bb2){
if(((((Cyc_Absyn_equal_tqual(tqual1a,tqual2a)&& Cyc_Unify_unify(eff1,eff2))&&
 Cyc_Unify_unify(null1a,null2a))&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Unify_unify(rel1,rel2))&& Cyc_Unify_unify(aq1,aq2)){
void*bbc=Cyc_BansheeIf_equality_constraint(bb1,bb2);
if(Cyc_Unify_unify(t1a,t2a)){
struct _tuple12 _Tmp11;_Tmp11.f0=1U,({struct Cyc_List_List*_Tmp12=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));_Tmp13->hd=bbc,_Tmp13->tl=0;_Tmp13;});_Tmp11.f1=_Tmp12;});return _Tmp11;}{
struct _tuple12 _Tmp11=Cyc_Subtype_force_equivalence(t1a,t2a);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion c=_Tmp13;struct Cyc_List_List*retc1=_Tmp12;
if((int)c!=0){
struct _tuple12 _Tmp14;_Tmp14.f0=1U,({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=bbc,_Tmp16->tl=retc1;_Tmp16;});_Tmp14.f1=_Tmp15;});return _Tmp14;}}}}}{
# 291
struct _tuple12 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0;return _Tmp11;}}}}else{goto _LL3;}}else{_LL3: {
# 294
struct _tuple12 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0;return _Tmp11;}};}
# 298
static int Cyc_Subtype_unify_cvar(struct Cyc_List_List**env,void*t1,void*t2){
if(Cyc_Tcutil_is_cvar_type(t1)|| Cyc_Tcutil_is_cvar_type(t2)){
void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
if((unsigned)cv1 &&(unsigned)cv2){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_BansheeIf_equality_constraint(cv1,cv2);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*_check_null(env);_Tmp1;});*env=_Tmp0;});
return 1;}}
# 307
return 0;}
# 310
static struct _tuple12 Cyc_Subtype_force_equivalence(void*t1,void*t2){
# 312
struct Cyc_List_List*retc=0;
if(({int(*_Tmp0)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**)=(int(*)(void*,void*,int(*)(struct Cyc_List_List**,void*,void*),struct Cyc_List_List**))Cyc_Unify_unify_c;_Tmp0;})(t1,t2,Cyc_Subtype_unify_cvar,& retc)){
struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=retc;return _Tmp0;}{
# 316
struct _tuple12 _Tmp0;_Tmp0.f0=0U,_Tmp0.f1=0;return _Tmp0;}}
# 319
static int Cyc_Subtype_gen_equality_constraint(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*);struct _tuple17{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 351 "subtype.cyc"
static struct _tuple12 Cyc_Subtype_subtype_impl(struct Cyc_List_List*assume,void*t1,void*t2){
# 353
struct Cyc_List_List*retc=0;
if(Cyc_Unify_unify(t1,t2)){struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0;return _Tmp0;}
{struct Cyc_List_List*a=assume;for(0;a!=0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple16*)a->hd)).f0)&& Cyc_Unify_unify(t2,(*((struct _tuple16*)a->hd)).f1)){
struct _tuple12 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0;return _Tmp0;}}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});struct Cyc_Absyn_FnInfo _Tmp1;struct Cyc_Absyn_FnInfo _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp12=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.autoreleased;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t_a=_Tmp12;struct Cyc_Absyn_Tqual q_a=_Tmp11;void*rt_a=_Tmp10;void*null_a=_TmpF;void*b_a=_TmpE;void*zt_a=_TmpD;void*rel_a=_TmpC;void*aq_a=_TmpB;void*t_b=_TmpA;struct Cyc_Absyn_Tqual q_b=_Tmp9;void*rt_b=_Tmp8;void*null_b=_Tmp7;void*b_b=_Tmp6;void*zt_b=_Tmp5;void*rel_b=_Tmp4;void*aq_b=_Tmp3;
# 365
enum Cyc_Absyn_Coercion coerce=1U;
# 367
if(q_a.real_const && !q_b.real_const){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 370
if(!Cyc_Unify_unify(null_a,null_b)){
if(Cyc_Absyn_type2bool(0,null_a)&& !Cyc_Absyn_type2bool(0,null_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}
# 376
if((!Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 380
if((Cyc_Unify_unify(rel_a,rel_b)&& !
Cyc_Absyn_type2bool(0,rel_a))&& Cyc_Absyn_type2bool(0,rel_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 384
if((!Cyc_Unify_unify(rt_a,rt_b)&& !((int)Cyc_Subtype_subtype_impl(assume,rt_a,rt_b).f0))&& !
Cyc_Tcutil_subset_effect(0,rt_a,rt_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 388
if((!Cyc_Unify_unify(aq_a,aq_b)&& !Cyc_Subtype_alias_qual_subtype(aq_a,aq_b))&& !((int)
Cyc_Subtype_subtype_impl(assume,aq_a,aq_b).f0)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 393
if(!Cyc_Unify_unify(b_a,b_b)){
if(!Cyc_Tcutil_is_cvar_type(b_a)&& !Cyc_Tcutil_is_cvar_type(b_b)){
struct Cyc_Absyn_Exp*e1=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_a);});
struct Cyc_Absyn_Exp*e2=({void*_Tmp13=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp13,b_b);});
if(e1!=e2){
if((e1==0 || e2==0)|| !Cyc_Evexp_lte_const_exp(e2,e1)){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}else{
# 405
void*cva=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_a);
void*cvb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);
if((unsigned)cva &&(unsigned)cvb)
retc=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp14=Cyc_BansheeIf_equality_constraint(cva,cvb);_Tmp13->hd=_Tmp14;}),_Tmp13->tl=retc;_Tmp13;});else{
# 411
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}
# 416
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b))){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}
# 421
if(Cyc_Subtype_isomorphic(t_a,t_b)){
struct _tuple12 _Tmp13;_Tmp13.f0=coerce,_Tmp13.f1=retc;return _Tmp13;}
# 424
{
struct _tuple13 _Tmp13=Cyc_Subtype_constraint_subtype(t_a,t_b,Cyc_Subtype_gen_equality_constraint,0,0);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s==1){
retc=Cyc_List_imp_append(retc,c);{
struct _tuple12 _Tmp16;_Tmp16.f0=1U,_Tmp16.f1=retc;return _Tmp16;}}}}{
# 432
int deep_subtype=!Cyc_Tcutil_force_type2bool(0,zt_b);
if(deep_subtype && !({void*_Tmp13=b_b;Cyc_Unify_unify(_Tmp13,Cyc_Absyn_bounds_one());})){
coerce=Cyc_Subtype_join_coercion(coerce,3U);{
void*cbb=Cyc_Tcutil_ptrbnd_cvar_equivalent(b_b);
if((unsigned)cbb)
retc=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp14=({void*_Tmp15=cbb;Cyc_BansheeIf_equality_constraint(_Tmp15,Cyc_Absyn_thinconst());});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=retc;_Tmp13;});else{
# 440
deep_subtype=0;}}}
# 442
if(deep_subtype){
struct _tuple12 _Tmp13=({struct Cyc_List_List*_Tmp14=({struct Cyc_List_List*_Tmp15=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp16=({struct _tuple16*_Tmp17=_cycalloc(sizeof(struct _tuple16));_Tmp17->f0=t1,_Tmp17->f1=t2;_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=assume;_Tmp15;});void*_Tmp15=t_a;Cyc_Subtype_ptrsubtype(_Tmp14,_Tmp15,t_b);});void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s==1){
struct _tuple12 _Tmp16;_Tmp16.f0=1U,({struct Cyc_List_List*_Tmp17=Cyc_List_imp_append(retc,c);_Tmp16.f1=_Tmp17;});return _Tmp16;}}}else{
# 449
struct _tuple12 _Tmp13=Cyc_Subtype_force_equivalence(t_a,t_b);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s!=0){
struct _tuple12 _Tmp16;_Tmp16.f0=1U,({struct Cyc_List_List*_Tmp17=Cyc_List_imp_append(retc,c);_Tmp16.f1=_Tmp17;});return _Tmp16;}}}{
# 453
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}}}}else{goto _LL7;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==23){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.tag==2){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.tag==2){_Tmp12=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f0;_Tmp10=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1.KnownDatatypefield.val.f1;_TmpF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpE=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1.KnownDatatype.val;_TmpD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Datatypedecl*dd1=_Tmp12;struct Cyc_Absyn_Datatypefield*df=_Tmp10;struct Cyc_List_List*ts1=_TmpF;struct Cyc_Absyn_Datatypedecl*dd2=_TmpE;struct Cyc_List_List*ts2=_TmpD;
# 459
if(dd1!=dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!=0){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
# 461
if(({int _Tmp13=Cyc_List_length(ts1);_Tmp13!=Cyc_List_length(ts2);})){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
for(1;ts1!=0;(ts1=ts1->tl,ts2=ts2->tl)){
struct _tuple13 _Tmp13=Cyc_Subtype_constraint_subtype((void*)ts1->hd,(void*)ts2->hd,Cyc_Subtype_gen_equality_constraint,0,0);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s!=0)
retc=Cyc_List_imp_append(retc,c);else{
# 469
struct _tuple12 _Tmp16=Cyc_Subtype_effect_subtype(t1,t2,1);void*_Tmp17;enum Cyc_Absyn_Coercion _Tmp18;_Tmp18=_Tmp16.f0;_Tmp17=_Tmp16.f1;{enum Cyc_Absyn_Coercion es=_Tmp18;struct Cyc_List_List*ec=_Tmp17;
if((int)es==0){
struct _tuple12 _Tmp19;_Tmp19.f0=0U,_Tmp19.f1=0;return _Tmp19;}
retc=Cyc_List_imp_append(retc,ec);}}}}{
# 475
struct _tuple12 _Tmp13;_Tmp13.f0=3U,_Tmp13.f1=retc;return _Tmp13;}}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_FnInfo f1=_Tmp2;struct Cyc_Absyn_FnInfo f2=_Tmp1;
# 477
if(f1.tvars!=0 || f2.tvars!=0){
struct Cyc_List_List*tvs1=f1.tvars;
struct Cyc_List_List*tvs2=f2.tvars;
if(({int _Tmp13=Cyc_List_length(tvs1);_Tmp13!=Cyc_List_length(tvs2);})){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}{
struct Cyc_List_List*inst=0;
while(tvs1!=0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind)){struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
inst=({struct Cyc_List_List*_Tmp13=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp14=({struct _tuple17*_Tmp15=_cycalloc(sizeof(struct _tuple17));_Tmp15->f0=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_Tmp16=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_Tmp15->f1=_Tmp16;});_Tmp15;});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=inst;_Tmp13;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;
# 483
1U;}
# 488
if(tvs2!=0){
struct _tuple12 _Tmp13;_Tmp13.f0=0U,_Tmp13.f1=0;return _Tmp13;}
if(inst!=0){
f1.tvars=0;
f2.tvars=0;{
void*newftype2=({struct _RegionHandle*_Tmp13=Cyc_Core_heap_region;struct Cyc_List_List*_Tmp14=inst;Cyc_Tcutil_rsubstitute(_Tmp13,_Tmp14,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp15->tag=6,_Tmp15->f1=f2;_Tmp15;}));});
struct Cyc_List_List*_Tmp13=assume;void*_Tmp14=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp15->tag=6,_Tmp15->f1=f1;_Tmp15;});return Cyc_Subtype_subtype_impl(_Tmp13,_Tmp14,newftype2);}}}}{
# 498
struct _tuple12 _Tmp13=Cyc_Subtype_subtype_impl(assume,f1.ret_type,f2.ret_type);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if(!((int)s)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
retc=Cyc_List_imp_append(retc,c);{
struct Cyc_List_List*args1=f1.args;
struct Cyc_List_List*args2=f2.args;
# 505
if(({int _Tmp16=Cyc_List_length(args1);_Tmp16!=Cyc_List_length(args2);})){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 507
for(1;args1!=0;(args1=args1->tl,args2=args2->tl)){
struct _tuple8 _Tmp16=*((struct _tuple8*)args1->hd);void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;_Tmp18=_Tmp16.f1;_Tmp17=_Tmp16.f2;{struct Cyc_Absyn_Tqual tq1=_Tmp18;void*t1=_Tmp17;
struct _tuple8 _Tmp19=*((struct _tuple8*)args2->hd);void*_Tmp1A;struct Cyc_Absyn_Tqual _Tmp1B;_Tmp1B=_Tmp19.f1;_Tmp1A=_Tmp19.f2;{struct Cyc_Absyn_Tqual tq2=_Tmp1B;void*t2=_Tmp1A;
# 511
if(tq2.real_const && !tq1.real_const){
struct _tuple12 _Tmp1C;_Tmp1C.f0=0U,_Tmp1C.f1=0;return _Tmp1C;}{
# 515
struct _tuple12 _Tmp1C=Cyc_Subtype_subtype_impl(0,t2,t1);void*_Tmp1D;enum Cyc_Absyn_Coercion _Tmp1E;_Tmp1E=_Tmp1C.f0;_Tmp1D=_Tmp1C.f1;{enum Cyc_Absyn_Coercion s=_Tmp1E;struct Cyc_List_List*c=_Tmp1D;
if((int)s==0){
struct _tuple12 _Tmp1F;_Tmp1F.f0=0U,_Tmp1F.f1=0;return _Tmp1F;}
retc=Cyc_List_imp_append(retc,c);}}}}}
# 521
if(f1.c_varargs!=f2.c_varargs){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
if(f1.cyc_varargs!=0 && f2.cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo v1=*f1.cyc_varargs;
struct Cyc_Absyn_VarargInfo v2=*f2.cyc_varargs;
# 526
if(v2.tq.real_const && !v1.tq.real_const){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}{
struct _tuple12 _Tmp16=Cyc_Subtype_subtype_impl(assume,v2.type,v1.type);void*_Tmp17;enum Cyc_Absyn_Coercion _Tmp18;_Tmp18=_Tmp16.f0;_Tmp17=_Tmp16.f1;{enum Cyc_Absyn_Coercion s=_Tmp18;struct Cyc_List_List*c=_Tmp17;
if((int)s==0){
struct _tuple12 _Tmp19;_Tmp19.f0=0U,_Tmp19.f1=0;return _Tmp19;}
retc=Cyc_List_imp_append(retc,c);}}}else{
if(f1.cyc_varargs!=0 || f2.cyc_varargs!=0){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}
# 534
if(!Cyc_Tcutil_subset_effect(0,f1.effect,f2.effect)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 536
if(Cyc_Tcutil_cmp_effect_constraints(f1.effconstr,f2.effconstr)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}
# 538
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes)){struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}{
# 540
struct Cyc_List_List*terms=0;
# 542
{struct Cyc_List_List*vds=f1.arg_vardecls;for(0;vds!=0;vds=vds->tl){
terms=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp17=Cyc_AssnDef_fresh_var((struct Cyc_Absyn_Vardecl*)vds->hd==0?0:((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_Tmp16->hd=_Tmp17;}),_Tmp16->tl=terms;_Tmp16;});}}
# 545
terms=Cyc_List_imp_rev(terms);
terms=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp17=Cyc_AssnDef_fresh_var(0);_Tmp16->hd=_Tmp17;}),_Tmp16->tl=terms;_Tmp16;});
# 554
if(f1.requires_assn!=0){
void*req1=Cyc_AssnDef_existassnfn2assn(f1.requires_assn,terms);
void*req2=(unsigned)f2.requires_assn?Cyc_AssnDef_existassnfn2assn(f2.requires_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(req2,req1)&& !
Cyc_PrattProver_constraint_prove(req2,req1)){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}
# 564
if(f2.ensures_assn!=0){
void*ret_value=Cyc_AssnDef_fresh_var(f1.return_value->type);
struct Cyc_List_List*_Tmp16;_Tmp16=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp16->hd=ret_value,_Tmp16->tl=terms;{struct Cyc_List_List*terms=_Tmp16;
void*ens2=Cyc_AssnDef_existassnfn2assn(f2.ensures_assn,terms);
void*ens1=(unsigned)f1.ensures_assn?Cyc_AssnDef_existassnfn2assn(f1.ensures_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(ens1,ens2)&& !
Cyc_PrattProver_constraint_prove(ens1,ens2)){
struct _tuple12 _Tmp17;_Tmp17.f0=0U,_Tmp17.f1=0;return _Tmp17;}}}
# 574
if(f2.throws_assn!=0){
void*thrws2=Cyc_AssnDef_existassnfn2assn(f2.throws_assn,terms);
void*thrws1=(unsigned)f1.throws_assn?Cyc_AssnDef_existassnfn2assn(f1.throws_assn,terms):(void*)& Cyc_AssnDef_true_assn;
if(!Cyc_AssnDef_simple_prove(thrws1,thrws2)&& !
Cyc_PrattProver_constraint_prove(thrws1,thrws2)){
struct _tuple12 _Tmp16;_Tmp16.f0=0U,_Tmp16.f1=0;return _Tmp16;}}{
# 582
struct _tuple12 _Tmp16;_Tmp16.f0=1U,_Tmp16.f1=retc;return _Tmp16;}}}}}}}else{goto _LL7;}default: _LL7: {
# 584
struct _tuple12 _Tmp13=Cyc_Subtype_effect_subtype(t1,t2,1);void*_Tmp14;enum Cyc_Absyn_Coercion _Tmp15;_Tmp15=_Tmp13.f0;_Tmp14=_Tmp13.f1;{enum Cyc_Absyn_Coercion s=_Tmp15;struct Cyc_List_List*c=_Tmp14;
if((int)s!=0){
struct _tuple12 _Tmp16;_Tmp16.f0=s,_Tmp16.f1=c;return _Tmp16;}
return Cyc_Subtype_force_equivalence(t1,t2);}}};}}
# 591
int Cyc_Subtype_subtype(unsigned loc,struct Cyc_List_List*assume,void*t1,void*t2){
# 593
struct _tuple12 _Tmp0=Cyc_Subtype_subtype_impl(assume,t1,t2);void*_Tmp1;enum Cyc_Absyn_Coercion _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{enum Cyc_Absyn_Coercion r=_Tmp2;struct Cyc_List_List*c=_Tmp1;
if((int)r==1)
return({int(*_Tmp3)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_forall_c;_Tmp3;})(Cyc_BansheeIf_add_constraint,loc,c);
# 597
return 0;}}
# 607 "subtype.cyc"
static struct _tuple12 Cyc_Subtype_ptrsubtype(struct Cyc_List_List*assume,void*t1,void*t2){
# 609
struct Cyc_List_List*tqs1=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Subtype_flatten_type(Cyc_Core_heap_region,1,t2);
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;
for(1;tqs2!=0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1==0){struct _tuple12 _Tmp0;_Tmp0.f0=0U,_Tmp0.f1=0;return _Tmp0;}{
struct _tuple15*_Tmp0=(struct _tuple15*)tqs1->hd;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Tqual tq1=_Tmp2;void*t1a=_Tmp1;
struct _tuple15*_Tmp3=(struct _tuple15*)tqs2->hd;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp3->f0;_Tmp4=_Tmp3->f1;{struct Cyc_Absyn_Tqual tq2=_Tmp5;void*t2a=_Tmp4;
if(tq1.real_const && !tq2.real_const){struct _tuple12 _Tmp6;_Tmp6.f0=0U,_Tmp6.f1=0;return _Tmp6;}
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Subtype_isomorphic(t1a,t2a))
continue;
if(tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a))){
struct _tuple12 _Tmp6=Cyc_Subtype_subtype_impl(assume,t1a,t2a);void*_Tmp7;enum Cyc_Absyn_Coercion _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{enum Cyc_Absyn_Coercion s=_Tmp8;struct Cyc_List_List*c=_Tmp7;
coerce=Cyc_Subtype_join_coercion(s,coerce);
if((int)coerce!=0){
retc=Cyc_List_imp_append(retc,c);
continue;}}}else{
# 629
struct _tuple12 _Tmp6=Cyc_Subtype_force_equivalence(t1a,t2a);void*_Tmp7;enum Cyc_Absyn_Coercion _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{enum Cyc_Absyn_Coercion s=_Tmp8;struct Cyc_List_List*c=_Tmp7;
coerce=Cyc_Subtype_join_coercion(s,coerce);
if((int)coerce!=0){
retc=Cyc_List_imp_append(retc,c);
continue;}}}{
# 636
struct _tuple12 _Tmp6;_Tmp6.f0=0U,_Tmp6.f1=0;return _Tmp6;}}}}}{
# 638
struct _tuple12 _Tmp0;_Tmp0.f0=coerce,_Tmp0.f1=retc;return _Tmp0;}}
# 645
static int Cyc_Subtype_gen_default_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){
if(k==Cyc_Absyn_PtrBndKind){
# 648
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(_t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(_t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;{void*t1=_Tmp4;void*z1=_Tmp3;void*t2=_Tmp2;void*z2=_Tmp1;
# 650
t1=Cyc_Absyn_compress(t1);t2=Cyc_Absyn_compress(t2);
if(!Cyc_Tcutil_is_cvar_type(t1)&& !Cyc_Tcutil_is_cvar_type(t2))
return 0;{
void*ct1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*ct2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
# 661
void*c=0;
if(((unsigned)ct1 &&(unsigned)ct2)&&({void*_Tmp5=ct2;_Tmp5!=Cyc_Absyn_thinconst();})){
# 667
if(({void*_Tmp5=ct1;_Tmp5!=Cyc_Absyn_thinconst();})|| !Cyc_Absyn_type2bool(0,z1))
c=({void*_Tmp5=({void*_Tmp6=ct2;Cyc_BansheeIf_cmpeq_constraint(_Tmp6,Cyc_Absyn_fatconst());});Cyc_BansheeIf_implication_constraint(_Tmp5,
Cyc_BansheeIf_equality_constraint(ct1,ct2));});}else{
# 682
if(!((unsigned)ct2))
c=({void*_Tmp5=_check_null(ct1);Cyc_BansheeIf_equality_constraint(_Tmp5,Cyc_Absyn_fatconst());});}
# 686
if((unsigned)c)
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=c,_Tmp6->tl=*retc;_Tmp6;});*retc=_Tmp5;});
# 690
return 1;}}}else{goto _LL8;}}else{_LL8:
# 692
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Non pointer type in gen constraint",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}else{
# 695
return 0;};}
# 699
static int Cyc_Subtype_gen_equality_constraint(struct Cyc_List_List**retc,enum Cyc_Absyn_KindQual k,void*_t1,void*_t2){
# 702
if(k==Cyc_Absyn_PtrBndKind){
# 704
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=_t1,_Tmp1.f1=_t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;{void*t1=_Tmp2;void*t2=_Tmp1;
# 706
void*cv1=Cyc_Tcutil_ptrbnd_cvar_equivalent(t1);
void*cv2=Cyc_Tcutil_ptrbnd_cvar_equivalent(t2);
if((unsigned)cv1 &&(unsigned)cv2){
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp5=Cyc_BansheeIf_equality_constraint(cv1,cv2);_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*retc;_Tmp4;});*retc=_Tmp3;});
return 1;}
# 712
return 0;}}else{goto _LL8;}}else{_LL8:
# 714
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Non pointer type in gen constraint",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}else{
# 717
return 0;};}
# 725
static struct _tuple13 Cyc_Subtype_constraint_subtype(void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_deep_subtype,int allow_coercion){
# 733
if(Cyc_Unify_unify(t1,t2)){
struct _tuple13 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0,_Tmp0.f2=1;return _Tmp0;}
# 736
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(*((int*)t2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)){case 0:  {
struct _tuple13 _Tmp0;_Tmp0.f0=1U,_Tmp0.f1=0,_Tmp0.f2=1;return _Tmp0;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2){case Cyc_Absyn_Int_sz:
# 741
 goto _LL6;case Cyc_Absyn_Long_sz: _LL6:
# 743
 if((int)Cyc_Tcutil_type_kind(t1)->kind==2){struct _tuple13 _Tmp0;_Tmp0.f0=3U,_Tmp0.f1=0,_Tmp0.f2=0;return _Tmp0;}
if(Cyc_Tcutil_is_pointer_type(t1)){
void*cv1=Cyc_Tcutil_get_pointer_bounds(t1);
if(Cyc_Tcutil_is_cvar_type(cv1)){
struct _tuple13 _Tmp0;_Tmp0.f0=3U,({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=({void*_Tmp4=Cyc_Tcutil_ptrbnd_cvar_equivalent(cv1);Cyc_BansheeIf_equality_constraint(_Tmp4,Cyc_Absyn_thinconst());});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});_Tmp0.f1=_Tmp1;}),_Tmp0.f2=0;return _Tmp0;}}
# 750
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;{
# 754
struct Cyc_List_List*retc=0;
enum Cyc_Absyn_Coercion coerce=1U;
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;struct Cyc_Absyn_Tqual _TmpF;void*_Tmp10;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_tq;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.nullable;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.zero_term;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.autoreleased;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_tq;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.eff;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*t1a=_Tmp10;struct Cyc_Absyn_Tqual tqual1a=_TmpF;void*eff1=_TmpE;void*null1a=_TmpD;void*b1=_TmpC;void*zt1=_TmpB;void*rel1=_TmpA;void*aq1=_Tmp9;void*t2a=_Tmp8;struct Cyc_Absyn_Tqual tqual2a=_Tmp7;void*eff2=_Tmp6;void*null2a=_Tmp5;void*b2=_Tmp4;void*zt2=_Tmp3;void*rel2=_Tmp2;void*aq2=_Tmp1;
# 758
if((!Cyc_Unify_unify(null1a,null2a)&& Cyc_Tcutil_force_type2bool(0,null1a))&& !Cyc_Tcutil_force_type2bool(0,null2a)){
if(allow_coercion)
coerce=2U;else{
# 762
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}}
# 764
if(!tqual2a.real_const && tqual1a.real_const){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 767
if((!Cyc_Unify_unify(eff1,eff2)&& !
Cyc_Tcutil_subset_effect(0,eff1,eff2))&& !
genconstr(& retc,3U,t1,t2)){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 773
if((!Cyc_Unify_unify(aq1,aq2)&& !Cyc_Subtype_alias_qual_subtype(aq1,aq2))&& !
genconstr(& retc,7U,t1,t2)){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 777
if(!Cyc_Unify_unify(rel1,rel2)&& !Cyc_Absyn_type2bool(0,rel2)){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
# 785
if((!Cyc_Unify_unify(zt1,zt2)&& Cyc_Absyn_type2bool(1,zt2))&& !genconstr(& retc,5U,t1,t2)){
# 796 "subtype.cyc"
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}{
# 798
int silent=1;
if(!Cyc_Unify_unify(b1,b2)){
if(!Cyc_Tcutil_is_cvar_type(b1)&& !Cyc_Tcutil_is_cvar_type(b2)){
struct Cyc_Absyn_Exp*e1=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp11,b1);});
struct Cyc_Absyn_Exp*e2=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp11,b2);});
if((unsigned)e1 &&(unsigned)e2){
if(!Cyc_Evexp_lte_const_exp(e2,e1)){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
silent=1;}else{
# 808
if((unsigned)e2)
silent=0;}}else{
# 813
if(!genconstr(& retc,6U,t1,t2)){
struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}}
# 818
coerce=Cyc_Subtype_join_coercion(coerce,3U);}
# 820
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Subtype_isomorphic(t1a,t2a)){
struct _tuple13 _Tmp11;_Tmp11.f0=coerce,_Tmp11.f1=retc,_Tmp11.f2=silent;return _Tmp11;}
if(((Cyc_Tcutil_is_bits_only_type(t1a)&& Cyc_Tcutil_is_char_type(t2a))&& !
Cyc_Tcutil_force_type2bool(0,zt2))&&(
tqual2a.real_const || !tqual1a.real_const)){
struct _tuple13 _Tmp11;_Tmp11.f0=coerce,_Tmp11.f1=retc,_Tmp11.f2=silent;return _Tmp11;}{
struct _tuple12 _Tmp11=Cyc_Subtype_effect_subtype(t1a,t2a,1);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
if((int)s!=0){
struct _tuple13 _Tmp14;({enum Cyc_Absyn_Coercion _Tmp15=Cyc_Subtype_join_coercion(s,coerce);_Tmp14.f0=_Tmp15;}),({struct Cyc_List_List*_Tmp15=Cyc_List_imp_append(retc,c);_Tmp14.f1=_Tmp15;}),_Tmp14.f2=silent;return _Tmp14;}
# 830
if(allow_deep_subtype){
struct _tuple12 _Tmp14=Cyc_Subtype_ptrsubtype(0,t1a,t2a);void*_Tmp15;enum Cyc_Absyn_Coercion _Tmp16;_Tmp16=_Tmp14.f0;_Tmp15=_Tmp14.f1;{enum Cyc_Absyn_Coercion s=_Tmp16;struct Cyc_List_List*c=_Tmp15;
if((int)s!=0 && !Cyc_Absyn_type2bool(1,zt2)){
# 835
if(({void*_Tmp17=b2;Cyc_Unify_unify(_Tmp17,Cyc_Absyn_bounds_one());})){
struct _tuple13 _Tmp17;({enum Cyc_Absyn_Coercion _Tmp18=Cyc_Subtype_join_coercion(s,coerce);_Tmp17.f0=_Tmp18;}),({struct Cyc_List_List*_Tmp18=Cyc_List_imp_append(retc,c);_Tmp17.f1=_Tmp18;}),_Tmp17.f2=1;return _Tmp17;}
# 838
if(Cyc_Tcutil_is_cvar_type(b2)){
void*cb2=Cyc_Tcutil_ptrbnd_cvar_equivalent(b2);
if((unsigned)cb2){
c=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp18=({void*_Tmp19=cb2;Cyc_BansheeIf_equality_constraint(_Tmp19,Cyc_Absyn_thinconst());});_Tmp17->hd=_Tmp18;}),_Tmp17->tl=c;_Tmp17;});{
struct _tuple13 _Tmp17;({enum Cyc_Absyn_Coercion _Tmp18=Cyc_Subtype_join_coercion(coerce,3U);_Tmp17.f0=_Tmp18;}),({struct Cyc_List_List*_Tmp18=Cyc_List_imp_append(retc,c);_Tmp17.f1=_Tmp18;}),_Tmp17.f2=silent;return _Tmp17;}}}}}}{
# 847
struct _tuple13 _Tmp14;_Tmp14.f0=0U,_Tmp14.f1=0,_Tmp14.f2=0;return _Tmp14;}}}}}}else{goto _LL16;}case 5: if(*((int*)_Tmp0.f1)==5){_Tmp10=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.tq;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.num_elts;_TmpD=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.zero_term;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.tq;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.num_elts;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1.zero_term;{void*t1a=_Tmp10;struct Cyc_Absyn_Tqual tq1a=_TmpF;struct Cyc_Absyn_Exp*e1=_TmpE;void*zt1=_TmpD;void*t2a=_TmpC;struct Cyc_Absyn_Tqual tq2a=_Tmp7;struct Cyc_Absyn_Exp*e2=_TmpB;void*zt2=_TmpA;
# 849
if(!Cyc_Unify_unify(zt1,zt2)){struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
if(e1==0 || e2==0){struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
if(!Cyc_Evexp_same_uint_const_exp(e1,e2)){struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}
if(!Cyc_Unify_unify(t1a,t2a)){struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}{
# 854
struct _tuple13 _Tmp11;_Tmp11.f0=(enum Cyc_Absyn_Coercion)(!tq1a.real_const || tq2a.real_const),_Tmp11.f1=retc,_Tmp11.f2=1;return _Tmp11;}}}else{goto _LL16;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)){case 19: _Tmp10=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed1=_Tmp10;
# 858
{void*_Tmp11;if(*((int*)t2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)==19){_Tmp11=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t2)->f1)->f2;{struct Cyc_Absyn_Enumdecl*ed2=_Tmp11;
# 860
if((ed1->fields!=0 && ed2->fields!=0)&&({
int _Tmp12=Cyc_List_length((struct Cyc_List_List*)ed1->fields->v);_Tmp12 >= Cyc_List_length((struct Cyc_List_List*)ed2->fields->v);})){
struct _tuple13 _Tmp12;_Tmp12.f0=1U,_Tmp12.f1=0,_Tmp12.f2=1;return _Tmp12;}
goto _LL1E;}}else{goto _LL21;}}else{_LL21:
 goto _LL1E;}_LL1E:;}
# 866
goto _LL11;}case 1: _LL11:
 goto _LL13;case 2: _LL13:
# 869
 if(Cyc_Tcutil_is_strict_arithmetic_type(t2)){struct _tuple13 _Tmp11;_Tmp11.f0=1U,_Tmp11.f1=0,_Tmp11.f2=1;return _Tmp11;}else{struct _tuple13 _Tmp11;_Tmp11.f0=0U,_Tmp11.f1=0,_Tmp11.f2=0;return _Tmp11;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2!=0){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0){_Tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd;_TmpE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd;{void*r1=_Tmp10;void*r2=_TmpE;
# 872
if(Cyc_Tcutil_subset_effect(0,r1,r2)){
struct _tuple13 _Tmp11;_Tmp11.f0=1U,_Tmp11.f1=0,_Tmp11.f2=1;return _Tmp11;}
# 875
goto _LL17;}}else{goto _LL16;}}else{goto _LL16;}}else{goto _LL16;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17: {
# 879
struct _tuple12 _Tmp11=Cyc_Subtype_force_equivalence(t1,t2);void*_Tmp12;enum Cyc_Absyn_Coercion _Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{enum Cyc_Absyn_Coercion s=_Tmp13;struct Cyc_List_List*c=_Tmp12;
struct _tuple13 _Tmp14;_Tmp14.f0=s,_Tmp14.f1=c,_Tmp14.f2=1;return _Tmp14;}}};}}
# 885
static enum Cyc_Absyn_Coercion Cyc_Subtype_internal_coercible_c(unsigned loc,int warn,void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_deep_subtype,int allow_coercion){
# 891
struct _tuple13 _Tmp0=Cyc_Subtype_constraint_subtype(t1,t2,genconstr,allow_deep_subtype,allow_coercion);int _Tmp1;void*_Tmp2;enum Cyc_Absyn_Coercion _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{enum Cyc_Absyn_Coercion s=_Tmp3;struct Cyc_List_List*c=_Tmp2;int b=_Tmp1;
if((int)s==0)
return s;
if(({int(*_Tmp4)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(int(*)(int(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_forall_c;_Tmp4;})(Cyc_BansheeIf_add_constraint,loc,c)){
if(!b && warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("implicit cast from ",sizeof(char),20U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t2;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;Cyc_Warn_warn2(loc,_tag_fat(_Tmp8,sizeof(void*),4));});
return s;}
# 899
return 0U;}}
# 902
static enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_c(unsigned loc,void*t1,void*t2,int(*genconstr)(struct Cyc_List_List**,enum Cyc_Absyn_KindQual,void*,void*),int allow_deep_subtype){
# 907
return Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,genconstr,allow_deep_subtype,1);}
# 910
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_default_constraint,1,1);}
# 914
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_warn(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,1,t1,t2,Cyc_Subtype_gen_default_constraint,1,1);}
# 918
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned loc,void*t1,void*t2){
return Cyc_Subtype_internal_coercible_c(loc,0,t1,t2,Cyc_Subtype_gen_equality_constraint,0,0);}
