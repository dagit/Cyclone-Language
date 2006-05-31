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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 172 "list.h"
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 101
extern struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*)(void*,void*),void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 147 "dict.h"
extern void Cyc_Dict_iter(void(*)(void*,void*),struct Cyc_Dict_Dict);
# 149
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 257
extern struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 312 "absyn.h"
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 325
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 332
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 563
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 920
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 926
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 939
void*Cyc_Absyn_compress(void*);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_void_type;
# 996
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1027
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1040
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1159
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 43 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "flags.h"
extern int Cyc_Flags_resolve;
# 49
extern int Cyc_Flags_warn_override;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 35 "bansheeif.h"
void Cyc_BansheeIf_register_toplevel_cvar(void*);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 60 "binding.cyc"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T2;struct Cyc_Dict_Dict(*_T3)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*);struct Cyc_Dict_Dict(*_T4)(int(*)(void*,void*),void*,void*);int(*_T5)(struct Cyc_List_List*,struct Cyc_List_List*);void*_T6;
void*data=mkdata(env);{struct Cyc_Binding_NSCtxt*_T7=_cycalloc(sizeof(struct Cyc_Binding_NSCtxt));
_T7->curr_ns=0;{struct Cyc_List_List*_T8=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T9=_cycalloc(sizeof(struct Cyc_Binding_Namespace_Binding_NSDirective_struct));_T9->tag=0;
_T9->f1=0;_T2=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T9;}_T8->hd=(void*)_T2;_T8->tl=0;_T1=(struct Cyc_List_List*)_T8;}_T7->availables=_T1;_T4=Cyc_Dict_singleton;{
struct Cyc_Dict_Dict(*_T8)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*)=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*))_T4;_T3=_T8;}_T5=Cyc_Absyn_varlist_cmp;_T6=data;_T7->ns_data=_T3(_T5,0,_T6);_T0=(struct Cyc_Binding_NSCtxt*)_T7;}
# 62
return _T0;}
# 66
static void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){union Cyc_Absyn_Nmspace _T0;struct _union_Nmspace_Abs_n _T1;unsigned _T2;union Cyc_Absyn_Nmspace _T3;struct _union_Nmspace_C_n _T4;union Cyc_Absyn_Nmspace _T5;struct _union_Nmspace_Abs_n _T6;void*(*_T7)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T8)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T9;struct Cyc_Dict_Dict _TA;struct Cyc_List_List*_TB;void*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_List_List*_TF;_T0=abs_ns;_T1=_T0.Abs_n;_T2=_T1.tag;switch(_T2){case 3: _T3=abs_ns;_T4=_T3.C_n;_TF=_T4.val;{struct Cyc_List_List*vs=_TF;_TF=vs;goto _LL4;}case 1: _T5=abs_ns;_T6=_T5.Abs_n;_TF=_T6.val;_LL4: {struct Cyc_List_List*vs=_TF;_T8=Cyc_Dict_lookup;{
# 69
void*(*_T10)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T8;_T7=_T10;}_T9=ctxt;_TA=_T9->ns_data;_TB=vs;_TC=_T7(_TA,_TB);return _TC;}default: _TD=
_tag_fat("Binding:get_ns_data: relative ns",sizeof(char),33U);_TE=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_TD,_TE);};}
# 84 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;int(*_T3)(struct Cyc_Dict_Dict,struct Cyc_List_List*);int(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_List_List*_T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_Binding_NSCtxt*_TA;struct Cyc_List_List*_TB;int*_TC;int _TD;int(*_TE)(struct Cyc_Dict_Dict,struct Cyc_List_List*);int(*_TF)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_List_List*_T12;int _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_List_List*_T18;struct _fat_ptr _T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;void**_T1B;void**_T1C;void**_T1D;unsigned _T1E;struct _fat_ptr _T1F;struct Cyc_Binding_BindingError_exn_struct*_T20;void*_T21;_T0=ctxt;_T1=_T0->curr_ns;_T2=rel_ns;{
# 87
struct Cyc_List_List*fullname=Cyc_List_append(_T1,_T2);_T4=Cyc_Dict_member;{
int(*_T22)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T4;_T3=_T22;}_T5=ctxt;_T6=_T5->ns_data;_T7=fullname;_T8=_T3(_T6,_T7);if(!_T8)goto _TL1;_T9=fullname;
return _T9;_TL1: _TA=ctxt;{
struct Cyc_List_List*as=_TA->availables;_TL6: if(as!=0)goto _TL4;else{goto _TL5;}
_TL4: _TB=as;{void*_T22=_TB->hd;struct Cyc_List_List*_T23;_TC=(int*)_T22;_TD=*_TC;if(_TD!=1)goto _TL7;{struct Cyc_Binding_Using_Binding_NSDirective_struct*_T24=(struct Cyc_Binding_Using_Binding_NSDirective_struct*)_T22;_T23=_T24->f1;}{struct Cyc_List_List*ns=_T23;_T23=ns;goto _LL4;}_TL7:{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T24=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T22;_T23=_T24->f1;}_LL4:{struct Cyc_List_List*ns=_T23;
# 94
struct Cyc_List_List*fullname=Cyc_List_append(ns,rel_ns);_TF=Cyc_Dict_member;{
int(*_T24)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_TF;_TE=_T24;}_T10=ctxt;_T11=_T10->ns_data;_T12=fullname;_T13=_TE(_T11,_T12);if(!_T13)goto _TL9;_T14=fullname;
return _T14;_TL9: goto _LL0;}_LL0:;}_T15=as;
# 90
as=_T15->tl;goto _TL6;_TL5:;}{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
# 99
_T22.f1=_tag_fat("namespace ",sizeof(char),11U);_T16=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T16;{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;_T18=rel_ns;_T19=_tag_fat("::",sizeof(char),3U);_T23.f1=Cyc_str_sepstr(_T18,_T19);_T17=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_T17;{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=_tag_fat(" not found",sizeof(char),11U);_T1A=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T1A;void*_T25[3];_T1B=_T25 + 0;*_T1B=& _T22;_T1C=_T25 + 1;*_T1C=& _T23;_T1D=_T25 + 2;*_T1D=& _T24;_T1E=loc;_T1F=_tag_fat(_T25,sizeof(void*),3);Cyc_Warn_err2(_T1E,_T1F);}}}{struct Cyc_Binding_BindingError_exn_struct*_T22=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T22->tag=Cyc_Binding_BindingError;_T20=(struct Cyc_Binding_BindingError_exn_struct*)_T22;}_T21=(void*)_T20;_throw(_T21);}}
# 104
static void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){struct _tuple0*_T0;union Cyc_Absyn_Nmspace _T1;struct _union_Nmspace_Abs_n _T2;unsigned _T3;union Cyc_Absyn_Nmspace _T4;struct _union_Nmspace_Rel_n _T5;struct Cyc_List_List*_T6;struct _handler_cons*_T7;int*_T8;int _T9;void*(*_TA)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_TB)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_TC;struct Cyc_Dict_Dict _TD;struct Cyc_Binding_NSCtxt*_TE;struct Cyc_List_List*_TF;void*_T10;struct _fat_ptr*_T11;void*_T12;struct Cyc_Dict_Absent_exn_struct*_T13;char*_T14;char*_T15;struct Cyc_Binding_NSCtxt*_T16;struct Cyc_List_List*_T17;int*_T18;int _T19;struct _handler_cons*_T1A;int*_T1B;int _T1C;void*(*_T1D)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1E)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T1F;struct Cyc_Dict_Dict _T20;struct Cyc_List_List*_T21;void*_T22;struct _fat_ptr*_T23;void*_T24;struct Cyc_Dict_Absent_exn_struct*_T25;char*_T26;char*_T27;struct Cyc_List_List*_T28;struct Cyc_Binding_BindingError_exn_struct*_T29;void*_T2A;union Cyc_Absyn_Nmspace _T2B;struct _union_Nmspace_Rel_n _T2C;struct _handler_cons*_T2D;int*_T2E;int _T2F;void*(*_T30)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T31)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T32;struct Cyc_Dict_Dict _T33;struct Cyc_List_List*_T34;void*_T35;struct _fat_ptr*_T36;void*_T37;struct Cyc_Dict_Absent_exn_struct*_T38;char*_T39;char*_T3A;struct Cyc_Binding_BindingError_exn_struct*_T3B;void*_T3C;union Cyc_Absyn_Nmspace _T3D;struct _union_Nmspace_C_n _T3E;union Cyc_Absyn_Nmspace _T3F;struct _union_Nmspace_Abs_n _T40;struct _handler_cons*_T41;int*_T42;int _T43;void*(*_T44)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T45)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T46;struct Cyc_Dict_Dict _T47;struct Cyc_List_List*_T48;void*_T49;struct _fat_ptr*_T4A;void*_T4B;struct Cyc_Dict_Absent_exn_struct*_T4C;char*_T4D;char*_T4E;struct Cyc_Binding_BindingError_exn_struct*_T4F;void*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct _fat_ptr*_T53;union Cyc_Absyn_Nmspace _T54;_T0=qv;{struct _tuple0 _T55=*_T0;_T54=_T55.f0;_T53=_T55.f1;}{union Cyc_Absyn_Nmspace ns=_T54;struct _fat_ptr*v=_T53;struct Cyc_List_List*_T55;_T1=ns;_T2=_T1.Abs_n;_T3=_T2.tag;switch(_T3){case 2: _T4=ns;_T5=_T4.Rel_n;_T6=_T5.val;if(_T6!=0)goto _TLC;{struct _handler_cons _T56;_T7=& _T56;_push_handler(_T7);{int _T57=0;_T8=_T56.handler;_T9=setjmp(_T8);if(!_T9)goto _TLE;_T57=1;goto _TLF;_TLE: _TLF: if(_T57)goto _TL10;else{goto _TL12;}_TL12: _TB=Cyc_Dict_lookup;{
# 108
void*(*_T58)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_TB;_TA=_T58;}_TC=ctxt;_TD=_TC->ns_data;_TE=ctxt;_TF=_TE->curr_ns;_T10=_TA(_TD,_TF);_T11=v;{void*_T58=lookup(_T10,_T11);_npop_handler(0);return _T58;}_pop_handler();goto _TL11;_TL10: _T12=Cyc_Core_get_exn_thrown();{void*_T58=(void*)_T12;void*_T59;_T13=(struct Cyc_Dict_Absent_exn_struct*)_T58;_T14=_T13->tag;_T15=Cyc_Dict_Absent;if(_T14!=_T15)goto _TL13;goto _LLE;_TL13: _T59=_T58;{void*exn=_T59;_rethrow(exn);}_LLE:;}_TL11:;}}_T16=ctxt;{
# 110
struct Cyc_List_List*as=_T16->availables;_TL18: if(as!=0)goto _TL16;else{goto _TL17;}
_TL16: _T17=as;{void*_T56=_T17->hd;struct Cyc_List_List*_T57;_T18=(int*)_T56;_T19=*_T18;if(_T19!=1)goto _TL19;{struct Cyc_Binding_Using_Binding_NSDirective_struct*_T58=(struct Cyc_Binding_Using_Binding_NSDirective_struct*)_T56;_T57=_T58->f1;}{struct Cyc_List_List*ns=_T57;_T57=ns;goto _LL17;}_TL19:{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T58=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T56;_T57=_T58->f1;}_LL17:{struct Cyc_List_List*ns=_T57;{struct _handler_cons _T58;_T1A=& _T58;_push_handler(_T1A);{int _T59=0;_T1B=_T58.handler;_T1C=setjmp(_T1B);if(!_T1C)goto _TL1B;_T59=1;goto _TL1C;_TL1B: _TL1C: if(_T59)goto _TL1D;else{goto _TL1F;}_TL1F: _T1E=Cyc_Dict_lookup;{
# 114
void*(*_T5A)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1E;_T1D=_T5A;}_T1F=ctxt;_T20=_T1F->ns_data;_T21=ns;_T22=_T1D(_T20,_T21);_T23=v;{void*_T5A=lookup(_T22,_T23);_npop_handler(0);return _T5A;}_pop_handler();goto _TL1E;_TL1D: _T24=Cyc_Core_get_exn_thrown();{void*_T5A=(void*)_T24;void*_T5B;_T25=(struct Cyc_Dict_Absent_exn_struct*)_T5A;_T26=_T25->tag;_T27=Cyc_Dict_Absent;if(_T26!=_T27)goto _TL20;goto _LL18;_TL20: _T5B=_T5A;{void*exn=_T5B;_rethrow(exn);}_LL18:;}_TL1E:;}}goto _LL13;}_LL13:;}_T28=as;
# 110
as=_T28->tl;goto _TL18;_TL17:;}{struct Cyc_Binding_BindingError_exn_struct*_T56=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
# 118
_T56->tag=Cyc_Binding_BindingError;_T29=(struct Cyc_Binding_BindingError_exn_struct*)_T56;}_T2A=(void*)_T29;_throw(_T2A);goto _TLD;_TLC: _T2B=ns;_T2C=_T2B.Rel_n;_T55=_T2C.val;{struct Cyc_List_List*ns=_T55;struct _handler_cons _T56;_T2D=& _T56;_push_handler(_T2D);{int _T57=0;_T2E=_T56.handler;_T2F=setjmp(_T2E);if(!_T2F)goto _TL22;_T57=1;goto _TL23;_TL22: _TL23: if(_T57)goto _TL24;else{goto _TL26;}_TL26:{
# 121
struct Cyc_List_List*abs_ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);_T31=Cyc_Dict_lookup;{
void*(*_T58)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T31;_T30=_T58;}_T32=ctxt;_T33=_T32->ns_data;_T34=abs_ns;_T35=_T30(_T33,_T34);_T36=v;{void*_T58=lookup(_T35,_T36);_npop_handler(0);return _T58;}}_pop_handler();goto _TL25;_TL24: _T37=Cyc_Core_get_exn_thrown();{void*_T58=(void*)_T37;void*_T59;_T38=(struct Cyc_Dict_Absent_exn_struct*)_T58;_T39=_T38->tag;_T3A=Cyc_Dict_Absent;if(_T39!=_T3A)goto _TL27;{struct Cyc_Binding_BindingError_exn_struct*_T5A=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T5A->tag=Cyc_Binding_BindingError;_T3B=(struct Cyc_Binding_BindingError_exn_struct*)_T5A;}_T3C=(void*)_T3B;_throw(_T3C);goto _TL28;_TL27: _T59=_T58;{void*exn=_T59;_rethrow(exn);}_TL28:;}_TL25:;}}_TLD:;case 3: _T3D=ns;_T3E=_T3D.C_n;_T55=_T3E.val;{struct Cyc_List_List*ns=_T55;_T55=ns;goto _LLB;}case 1: _T3F=ns;_T40=_T3F.Abs_n;_T55=_T40.val;_LLB: {struct Cyc_List_List*ns=_T55;struct _handler_cons _T56;_T41=& _T56;_push_handler(_T41);{int _T57=0;_T42=_T56.handler;_T43=setjmp(_T42);if(!_T43)goto _TL29;_T57=1;goto _TL2A;_TL29: _TL2A: if(_T57)goto _TL2B;else{goto _TL2D;}_TL2D: _T45=Cyc_Dict_lookup;{
# 126
void*(*_T58)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T45;_T44=_T58;}_T46=ctxt;_T47=_T46->ns_data;_T48=ns;_T49=_T44(_T47,_T48);_T4A=v;{void*_T58=lookup(_T49,_T4A);_npop_handler(0);return _T58;}_pop_handler();goto _TL2C;_TL2B: _T4B=Cyc_Core_get_exn_thrown();{void*_T58=(void*)_T4B;void*_T59;_T4C=(struct Cyc_Dict_Absent_exn_struct*)_T58;_T4D=_T4C->tag;_T4E=Cyc_Dict_Absent;if(_T4D!=_T4E)goto _TL2E;{struct Cyc_Binding_BindingError_exn_struct*_T5A=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T5A->tag=Cyc_Binding_BindingError;_T4F=(struct Cyc_Binding_BindingError_exn_struct*)_T5A;}_T50=(void*)_T4F;_throw(_T50);goto _TL2F;_TL2E: _T59=_T58;{void*exn=_T59;_rethrow(exn);}_TL2F:;}_TL2C:;}}default: _T51=
_tag_fat("lookup local in global",sizeof(char),23U);_T52=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T51,_T52);};}}
# 132
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;int(*_T3)(struct Cyc_Dict_Dict,struct Cyc_List_List*);int(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_List_List*_T7;int _T8;struct Cyc_Binding_NSCtxt*_T9;struct Cyc_Dict_Dict(*_TA)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*);struct Cyc_Dict_Dict(*_TB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_NSCtxt*_TC;struct Cyc_Dict_Dict _TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Binding_NSCtxt*_T10;struct Cyc_Binding_NSCtxt*_T11;struct Cyc_List_List*_T12;struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T13;struct Cyc_Binding_NSCtxt*_T14;_T0=ctxt;{
struct Cyc_List_List*ns=_T0->curr_ns;_T1=ns;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));
_T15->hd=subname;_T15->tl=0;_T2=(struct Cyc_List_List*)_T15;}{struct Cyc_List_List*ns2=Cyc_List_append(_T1,_T2);_T4=Cyc_Dict_member;{
int(*_T15)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T4;_T3=_T15;}_T5=ctxt;_T6=_T5->ns_data;_T7=ns2;_T8=_T3(_T6,_T7);if(_T8)goto _TL30;else{goto _TL32;}
_TL32: _T9=ctxt;_TB=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*))_TB;_TA=_T15;}_TC=ctxt;_TD=_TC->ns_data;_TE=ns2;_TF=mkdata(env);_T9->ns_data=_TA(_TD,_TE,_TF);goto _TL31;_TL30: _TL31: _T10=ctxt;
_T10->curr_ns=ns2;_T11=ctxt;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T16=_cycalloc(sizeof(struct Cyc_Binding_Namespace_Binding_NSDirective_struct));_T16->tag=0;
_T16->f1=ns2;_T13=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T16;}_T15->hd=(void*)_T13;_T14=ctxt;_T15->tl=_T14->availables;_T12=(struct Cyc_List_List*)_T15;}_T11->availables=_T12;}}}
# 140
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Binding_NSCtxt*_T4;struct Cyc_List_List*_T5;int*_T6;int _T7;struct Cyc_Binding_NSCtxt*_T8;struct Cyc_Binding_NSCtxt*_T9;struct Cyc_List_List*_TA;struct Cyc_Binding_NSCtxt*_TB;struct Cyc_Binding_NSCtxt*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;_T0=ctxt;_T1=_T0->availables;
if(_T1!=0)goto _TL33;_T2=
_tag_fat("leaving topmost namespace",sizeof(char),26U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T2,_T3);goto _TL34;_TL33: _TL34: _T4=ctxt;_T5=_T4->availables;{
void*_T13=_T5->hd;_T6=(int*)_T13;_T7=*_T6;if(_T7!=0)goto _TL35;_T8=ctxt;_T9=ctxt;_TA=_T9->availables;
# 145
_T8->availables=_TA->tl;_TB=ctxt;_TC=ctxt;_TD=_TC->curr_ns;_TE=
Cyc_List_rev(_TD);_TF=_check_null(_TE);_T10=_TF->tl;_TB->curr_ns=Cyc_List_rev(_T10);goto _LL0;_TL35: _T11=
# 149
_tag_fat("leaving using as namespace",sizeof(char),27U);_T12=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T11,_T12);_LL0:;}}
# 152
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){struct _tuple0*_T0;union Cyc_Absyn_Nmspace _T1;struct _union_Nmspace_Loc_n _T2;unsigned _T3;union Cyc_Absyn_Nmspace _T4;struct _union_Nmspace_Rel_n _T5;unsigned _T6;struct Cyc_Binding_NSCtxt*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct _tuple0*_TE;union Cyc_Absyn_Nmspace _TF;struct _union_Nmspace_Abs_n _T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Binding_NSCtxt*_T17;struct Cyc_List_List*_T18;struct Cyc_Binding_Using_Binding_NSDirective_struct*_T19;struct Cyc_Binding_NSCtxt*_T1A;struct Cyc_List_List*_T1B;struct _fat_ptr*_T1C;union Cyc_Absyn_Nmspace _T1D;_T0=usename;{struct _tuple0 _T1E=*_T0;_T1D=_T1E.f0;_T1C=_T1E.f1;}{union Cyc_Absyn_Nmspace nsl=_T1D;struct _fat_ptr*v=_T1C;
# 154
struct Cyc_List_List*ns;{struct Cyc_List_List*_T1E;_T1=nsl;_T2=_T1.Loc_n;_T3=_T2.tag;switch(_T3){case 2: _T4=nsl;_T5=_T4.Rel_n;_T1E=_T5.val;{struct Cyc_List_List*vs=_T1E;_T6=loc;_T7=ctxt;_T8=vs;{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));
# 157
_T1F->hd=v;_T1F->tl=0;_T9=(struct Cyc_List_List*)_T1F;}_TA=Cyc_List_append(_T8,_T9);ns=Cyc_Binding_resolve_rel_ns(_T6,_T7,_TA);_TB=
Cyc_List_rev(ns);_TC=_check_null(_TB);_TD=_TC->tl;{struct Cyc_List_List*abs_vs=Cyc_List_rev(_TD);_TE=usename;
(*_TE).f0=Cyc_Absyn_Abs_n(abs_vs,0);goto _LL3;}}case 1: _TF=nsl;_T10=_TF.Abs_n;_T1E=_T10.val;{struct Cyc_List_List*vs=_T1E;_T11=vs;{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));
# 162
_T1F->hd=v;_T1F->tl=0;_T12=(struct Cyc_List_List*)_T1F;}ns=Cyc_List_append(_T11,_T12);goto _LL3;}case 4: _T13=
# 164
_tag_fat("enter_using, Loc_n",sizeof(char),19U);_T14=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T13,_T14);default: _T15=
_tag_fat("enter_using, C_n",sizeof(char),17U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T15,_T16);}_LL3:;}_T17=ctxt;{struct Cyc_List_List*_T1E=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Binding_Using_Binding_NSDirective_struct*_T1F=_cycalloc(sizeof(struct Cyc_Binding_Using_Binding_NSDirective_struct));_T1F->tag=1;
# 167
_T1F->f1=ns;_T19=(struct Cyc_Binding_Using_Binding_NSDirective_struct*)_T1F;}_T1E->hd=(void*)_T19;_T1A=ctxt;_T1E->tl=_T1A->availables;_T18=(struct Cyc_List_List*)_T1E;}_T17->availables=_T18;_T1B=ns;
return _T1B;}}
# 170
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Binding_NSCtxt*_T4;struct Cyc_List_List*_T5;int*_T6;int _T7;struct Cyc_Binding_NSCtxt*_T8;struct Cyc_Binding_NSCtxt*_T9;struct Cyc_List_List*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;_T0=ctxt;_T1=_T0->availables;
if(_T1!=0)goto _TL38;_T2=
_tag_fat("leaving topmost namespace as a using",sizeof(char),37U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T2,_T3);goto _TL39;_TL38: _TL39: _T4=ctxt;_T5=_T4->availables;{
void*_TD=_T5->hd;_T6=(int*)_TD;_T7=*_T6;if(_T7!=1)goto _TL3A;_T8=ctxt;_T9=ctxt;_TA=_T9->availables;
_T8->availables=_TA->tl;goto _LL0;_TL3A: _TB=
_tag_fat("leaving namespace as using",sizeof(char),27U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_TB,_TC);_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;int in_cyc_override;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 215 "binding.cyc"
static void Cyc_Binding_dump_local_entry(struct _fat_ptr*v,void*r){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;{struct Cyc_String_pa_PrintArg_struct _T5;_T5.tag=0;_T1=v;
_T5.f1=*_T1;_T0=_T5;}{struct Cyc_String_pa_PrintArg_struct _T5=_T0;void*_T6[1];_T2=_T6 + 0;*_T2=& _T5;_T3=_tag_fat("%s resolved\n",sizeof(char),13U);_T4=_tag_fat(_T6,sizeof(void*),1);Cyc_printf(_T3,_T4);}}
# 218
static void Cyc_Binding_dump_local_dict(struct Cyc_Binding_Env*env){void(*_T0)(void(*)(struct _fat_ptr*,void*),struct Cyc_Dict_Dict);void(*_T1)(void(*)(void*,void*),struct Cyc_Dict_Dict);struct Cyc_Binding_Env*_T2;struct Cyc_Dict_Dict*_T3;struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict _T5;_T1=Cyc_Dict_iter;{
void(*_T6)(void(*)(struct _fat_ptr*,void*),struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _fat_ptr*,void*),struct Cyc_Dict_Dict))_T1;_T0=_T6;}_T2=env;_T3=_T2->local_vars;_T4=_check_null(_T3);_T5=*_T4;_T0(Cyc_Binding_dump_local_entry,_T5);}
# 221
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){struct Cyc_Binding_Env*_T0;int _T1;_T0=env;_T1=_T0->in_cinclude;
return _T1;}
# 224
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){struct Cyc_Binding_Env*_T0;struct Cyc_Dict_Dict*_T1;int _T2;_T0=env;_T1=_T0->local_vars;_T2=_T1==0;
return _T2;}
# 227
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){struct Cyc_Binding_ResolveNSEnv*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Dict_Dict(*_T4)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T5)(int(*)(void*,void*));int(*_T6)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Dict_Dict(*_T7)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T8)(int(*)(void*,void*));int(*_T9)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Dict_Dict(*_TA)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_TB)(int(*)(void*,void*));int(*_TC)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Dict_Dict(*_TD)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_TE)(int(*)(void*,void*));int(*_TF)(struct _fat_ptr*,struct _fat_ptr*);{struct Cyc_Binding_ResolveNSEnv*_T10=_cycalloc(sizeof(struct Cyc_Binding_ResolveNSEnv));_T2=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T2;_T1=_T11;}_T3=Cyc_strptrcmp;_T10->aggrdecls=_T1(_T3);_T5=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T5;_T4=_T11;}_T6=Cyc_strptrcmp;_T10->datatypedecls=_T4(_T6);_T8=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T8;_T7=_T11;}_T9=Cyc_strptrcmp;_T10->enumdecls=_T7(_T9);_TB=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_TB;_TA=_T11;}_TC=Cyc_strptrcmp;_T10->typedefs=_TA(_TC);_TE=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_TE;_TD=_T11;}_TF=Cyc_strptrcmp;_T10->ordinaries=_TD(_TF);_T0=(struct Cyc_Binding_ResolveNSEnv*)_T10;}
# 228
return _T0;}
# 235
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){struct Cyc_Absyn_Aggrdecl*(*_T0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T2;struct Cyc_Dict_Dict _T3;struct _fat_ptr*_T4;struct Cyc_Absyn_Aggrdecl*_T5;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Aggrdecl*(*_T6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1;_T0=_T6;}_T2=renv;_T3=_T2->aggrdecls;_T4=v;_T5=_T0(_T3,_T4);return _T5;}
# 238
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){struct Cyc_List_List*(*_T0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T2;struct Cyc_Dict_Dict _T3;struct _fat_ptr*_T4;struct Cyc_List_List*_T5;_T1=Cyc_Dict_lookup;{
struct Cyc_List_List*(*_T6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_List_List*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1;_T0=_T6;}_T2=renv;_T3=_T2->datatypedecls;_T4=v;_T5=_T0(_T3,_T4);return _T5;}
# 241
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){struct Cyc_Absyn_Enumdecl*(*_T0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T2;struct Cyc_Dict_Dict _T3;struct _fat_ptr*_T4;struct Cyc_Absyn_Enumdecl*_T5;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Enumdecl*(*_T6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1;_T0=_T6;}_T2=renv;_T3=_T2->enumdecls;_T4=v;_T5=_T0(_T3,_T4);return _T5;}
# 244
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){struct Cyc_Absyn_Typedefdecl*(*_T0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T2;struct Cyc_Dict_Dict _T3;struct _fat_ptr*_T4;struct Cyc_Absyn_Typedefdecl*_T5;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Typedefdecl*(*_T6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1;_T0=_T6;}_T2=renv;_T3=_T2->typedefs;_T4=v;_T5=_T0(_T3,_T4);return _T5;}
# 247
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){void*(*_T0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T2;struct Cyc_Dict_Dict _T3;struct _fat_ptr*_T4;void*_T5;_T1=Cyc_Dict_lookup;{
void*(*_T6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1;_T0=_T6;}_T2=renv;_T3=_T2->ordinaries;_T4=v;_T5=_T0(_T3,_T4);return _T5;}
# 250
static void*Cyc_Binding_lookup_ordinary(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){struct _tuple0*_T0;union Cyc_Absyn_Nmspace _T1;struct _union_Nmspace_Rel_n _T2;unsigned _T3;int _T4;int(*_T5)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T6)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T7;struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict*_T9;struct Cyc_Dict_Dict _TA;struct _fat_ptr*_TB;int _TC;struct Cyc_Binding_BindingError_exn_struct*_TD;void*_TE;void*(*_TF)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T10)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T11;struct Cyc_Dict_Dict*_T12;struct Cyc_Dict_Dict*_T13;struct Cyc_Dict_Dict _T14;struct _fat_ptr*_T15;void*_T16;union Cyc_Absyn_Nmspace _T17;struct _union_Nmspace_Rel_n _T18;struct Cyc_List_List*_T19;int _T1A;int(*_T1B)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1C)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T1D;struct Cyc_Dict_Dict*_T1E;struct Cyc_Dict_Dict _T1F;struct _fat_ptr*_T20;int _T21;void*(*_T22)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T23)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T24;struct Cyc_Dict_Dict*_T25;struct Cyc_Dict_Dict _T26;struct _fat_ptr*_T27;void*_T28;void*(*_T29)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T2A;struct Cyc_Binding_Env*_T2B;struct Cyc_Binding_NSCtxt*_T2C;struct _tuple0*_T2D;void*_T2E;struct _fat_ptr*_T2F;union Cyc_Absyn_Nmspace _T30;_T0=qv;{struct _tuple0 _T31=*_T0;_T30=_T31.f0;_T2F=_T31.f1;}{union Cyc_Absyn_Nmspace nsl=_T30;struct _fat_ptr*v=_T2F;_T1=nsl;_T2=_T1.Rel_n;_T3=_T2.tag;switch(_T3){case 4: _T4=
# 254
Cyc_Binding_at_toplevel(env);if(_T4)goto _TL3F;else{goto _TL40;}_TL40: _T6=Cyc_Dict_member;{int(*_T31)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T6;_T5=_T31;}_T7=env;_T8=_T7->local_vars;_T9=_check_null(_T8);_TA=*_T9;_TB=v;_TC=_T5(_TA,_TB);if(_TC)goto _TL3D;else{goto _TL3F;}
_TL3F:{struct Cyc_Binding_BindingError_exn_struct*_T31=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_T31->tag=Cyc_Binding_BindingError;_TD=(struct Cyc_Binding_BindingError_exn_struct*)_T31;}_TE=(void*)_TD;_throw(_TE);goto _TL3E;_TL3D: _TL3E: _T10=Cyc_Dict_lookup;{
void*(*_T31)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T10;_TF=_T31;}_T11=env;_T12=_T11->local_vars;_T13=_check_null(_T12);_T14=*_T13;_T15=v;_T16=_TF(_T14,_T15);return _T16;case 2: _T17=nsl;_T18=_T17.Rel_n;_T19=_T18.val;if(_T19!=0)goto _TL41;_T1A=
# 258
Cyc_Binding_at_toplevel(env);if(_T1A)goto _TL43;else{goto _TL45;}_TL45: _T1C=Cyc_Dict_member;{int(*_T31)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1C;_T1B=_T31;}_T1D=env;_T1E=_T1D->local_vars;_T1F=*_T1E;_T20=v;_T21=_T1B(_T1F,_T20);if(!_T21)goto _TL43;_T23=Cyc_Dict_lookup;{
void*(*_T31)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T23;_T22=_T31;}_T24=env;_T25=_T24->local_vars;_T26=*_T25;_T27=v;_T28=_T22(_T26,_T27);return _T28;_TL43: goto _LL9;_TL41: goto _LL8;default: _LL8: _LL9:{
# 262
void*(*_T31)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(void*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T29=_T31;}_T2A=loc;_T2B=env;_T2C=_T2B->ns;_T2D=qv;_T2E=_T29(_T2A,_T2C,_T2D,Cyc_Binding_lookup_ordinary_global);return _T2E;};}}
# 266
static void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
static void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
static void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
static void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
static void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_Binding_resolve_type(unsigned,struct Cyc_Binding_Env*,void*);
# 273
static void Cyc_Binding_resolve_effconstr(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
static void Cyc_Binding_resolve_qualbnd(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
static void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
# 284
static void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){struct _tuple0*_T0;struct _tuple0 _T1;struct _union_Nmspace_Abs_n _T2;unsigned _T3;struct _union_Nmspace_Rel_n _T4;struct Cyc_List_List*_T5;int _T6;struct _tuple0*_T7;struct Cyc_Binding_Env*_T8;struct Cyc_Binding_NSCtxt*_T9;struct Cyc_List_List*_TA;int _TB;int _TC;enum Cyc_Absyn_Scope _TD;int _TE;struct _tuple0*_TF;int _T10;struct Cyc_Warn_String_Warn_Warg_struct _T11;struct Cyc_Warn_Qvar_Warn_Warg_struct _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;void**_T15;void**_T16;unsigned _T17;struct _fat_ptr _T18;_T0=qv;_T1=*_T0;{
union Cyc_Absyn_Nmspace _T19=_T1.f0;_T2=_T19.Abs_n;_T3=_T2.tag;switch(_T3){case 2: _T4=_T19.Rel_n;_T5=_T4.val;if(_T5!=0)goto _TL47;_T6=
# 287
Cyc_Binding_at_toplevel(env);if(!_T6)goto _TL49;_T7=qv;_T8=env;_T9=_T8->ns;_TA=_T9->curr_ns;_TC=
Cyc_Binding_in_cinclude(env);if(!_TC)goto _TL4B;_TB=1;goto _TL4C;_TL4B: _TD=sc;_TE=(int)_TD;_TB=_TE==4;_TL4C:(*_T7).f0=Cyc_Absyn_Abs_n(_TA,_TB);goto _TL4A;
# 290
_TL49: _TF=qv;(*_TF).f0=Cyc_Absyn_Loc_n();_TL4A: goto _LL0;_TL47: goto _LL7;case 4: goto _LL0;case 1: _T10=
# 294
Cyc_Binding_at_toplevel(env);if(_T10)goto _TL4D;else{goto _TL4F;}
_TL4F: goto _LL8;_TL4D: goto _LL0;default: _LL7: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 297
_T1A.f1=_tag_fat("qualified names in declarations unimplemented (",sizeof(char),48U);_T11=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T11;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1B;_T1B.tag=1;_T1B.f1=qv;_T12=_T1B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1B=_T12;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=_tag_fat(")",sizeof(char),2U);_T13=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T13;void*_T1D[3];_T14=_T1D + 0;*_T14=& _T1A;_T15=_T1D + 1;*_T15=& _T1B;_T16=_T1D + 2;*_T16=& _T1C;_T17=loc;_T18=_tag_fat(_T1D,sizeof(void*),3);Cyc_Warn_err2(_T17,_T18);}}}}_LL0:;}}
# 303
static void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;_T0=env;{
struct Cyc_Dict_Dict*old_locals=_T0->local_vars;_T1=env;
_T1->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);_T2=env;
_T2->local_vars=old_locals;}}
# 310
static void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){struct _handler_cons*_T0;int*_T1;int _T2;int*_T3;unsigned _T4;void*_T5;int _T6;void*_T7;int*_T8;unsigned _T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;void**_T10;void**_T11;unsigned _T12;struct _fat_ptr _T13;void*_T14;struct Cyc_Binding_BindingError_exn_struct*_T15;char*_T16;char*_T17;
struct _fat_ptr hides_what;struct _handler_cons _T18;_T0=& _T18;_push_handler(_T0);{int _T19=0;_T1=_T18.handler;_T2=setjmp(_T1);if(!_T2)goto _TL50;_T19=1;goto _TL51;_TL50: _TL51: if(_T19)goto _TL52;else{goto _TL54;}_TL54:{
# 313
void*_T1A=Cyc_Binding_lookup_ordinary(loc,env,q);void*_T1B;_T3=(int*)_T1A;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1C=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1A;_T5=_T1C->f1;_T1B=(void*)_T5;}{void*b=_T1B;_T6=
# 315
Cyc_Binding_at_toplevel(env);if(!_T6)goto _TL56;_npop_handler(0);return;_TL56: _T7=b;_T8=(int*)_T7;_T9=*_T8;switch(_T9){case 1:
# 318
 hides_what=_tag_fat("global variable",sizeof(char),16U);goto _LLB;case 4:
 hides_what=_tag_fat("local variable",sizeof(char),15U);goto _LLB;case 3:
 hides_what=_tag_fat("parameter",sizeof(char),10U);goto _LLB;case 5:
 hides_what=_tag_fat("pattern variable",sizeof(char),17U);goto _LLB;case 2:
 hides_what=_tag_fat("function",sizeof(char),9U);goto _LLB;default: _TB=Cyc_Warn_impos;{
int(*_T1C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T1C;}_TC=_tag_fat("shadowing free variable!",sizeof(char),25U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);}_LLB: goto _LL0;}case 1:
# 326
 hides_what=_tag_fat("struct constructor",sizeof(char),19U);goto _LL0;case 2:
 hides_what=_tag_fat("datatype constructor",sizeof(char),21U);goto _LL0;case 3: goto _LLA;default: _LLA:
# 329
 hides_what=_tag_fat("enum tag",sizeof(char),9U);goto _LL0;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 331
_T1A.f1=_tag_fat("declaration hides ",sizeof(char),19U);_TE=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;_T1B.f1=hides_what;_TF=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_TF;void*_T1C[2];_T10=_T1C + 0;*_T10=& _T1A;_T11=_T1C + 1;*_T11=& _T1B;_T12=loc;_T13=_tag_fat(_T1C,sizeof(void*),2);Cyc_Warn_warn2(_T12,_T13);}}_npop_handler(0);return;_pop_handler();goto _TL53;_TL52: _T14=Cyc_Core_get_exn_thrown();{void*_T1A=(void*)_T14;void*_T1B;_T15=(struct Cyc_Binding_BindingError_exn_struct*)_T1A;_T16=_T15->tag;_T17=Cyc_Binding_BindingError;if(_T16!=_T17)goto _TL59;
# 333
return;_TL59: _T1B=_T1A;{void*exn=_T1B;_rethrow(exn);};}_TL53:;}}
# 336
static void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){unsigned _T0;struct Cyc_Binding_Env*_T1;struct Cyc_Absyn_Vardecl*_T2;struct _tuple0*_T3;struct Cyc_Absyn_Vardecl*_T4;enum Cyc_Absyn_Scope _T5;struct Cyc_Absyn_Vardecl*_T6;unsigned _T7;struct Cyc_Binding_Env*_T8;struct Cyc_Absyn_Vardecl*_T9;void*_TA;int _TB;unsigned _TC;struct Cyc_Binding_Env*_TD;struct Cyc_Absyn_Vardecl*_TE;struct _tuple0*_TF;int _T10;struct Cyc_Binding_Env*_T11;struct Cyc_Dict_Dict*_T12;struct Cyc_Dict_Dict*_T13;struct Cyc_Dict_Dict(*_T14)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T16;struct Cyc_Dict_Dict*_T17;struct Cyc_Dict_Dict*_T18;struct Cyc_Dict_Dict _T19;struct _fat_ptr*_T1A;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T1C;void*_T1D;struct Cyc_Binding_Env*_T1E;int _T1F;struct Cyc_Binding_ResolveNSEnv*(*_T20)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T21;struct Cyc_Binding_NSCtxt*_T22;union Cyc_Absyn_Nmspace _T23;struct Cyc_Binding_ResolveNSEnv*_T24;struct Cyc_Dict_Dict(*_T25)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T27;struct Cyc_Dict_Dict _T28;struct _fat_ptr*_T29;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T2A;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T2B;void*_T2C;_T0=loc;_T1=env;_T2=vd;_T3=_T2->name;_T4=vd;_T5=_T4->sc;
Cyc_Binding_absolutize_decl(_T0,_T1,_T3,_T5);_T6=vd;{
struct _tuple0*_T2D=_T6->name;struct _fat_ptr*_T2E;union Cyc_Absyn_Nmspace _T2F;{struct _tuple0 _T30=*_T2D;_T2F=_T30.f0;_T2E=_T30.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T2F;struct _fat_ptr*decl_name=_T2E;_T7=loc;_T8=env;_T9=vd;_TA=_T9->type;
Cyc_Binding_resolve_type(_T7,_T8,_TA);_TB=Cyc_Flags_warn_override;
if(!_TB)goto _TL5B;_TC=loc;_TD=env;_TE=vd;_TF=_TE->name;
Cyc_Binding_check_warn_override(_TC,_TD,_TF);goto _TL5C;_TL5B: _TL5C: _T10=
Cyc_Binding_at_toplevel(env);if(_T10)goto _TL5D;else{goto _TL5F;}
_TL5F: _T11=env;_T12=_T11->local_vars;_T13=_check_null(_T12);_T15=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T30)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T15;_T14=_T30;}_T16=env;_T17=_T16->local_vars;_T18=_check_null(_T17);_T19=*_T18;_T1A=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T30=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T30->tag=0;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T31=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T31->tag=4;
_T31->f1=vd;_T1C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T31;}_T30->f1=(void*)_T1C;_T1B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T30;}_T1D=(void*)_T1B;
# 343
*_T13=_T14(_T19,_T1A,_T1D);goto _TL5E;
# 345
_TL5D: _T1E=env;_T1F=_T1E->in_cyc_override;if(_T1F)goto _TL60;else{goto _TL62;}
_TL62:{struct Cyc_Binding_ResolveNSEnv*(*_T30)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T20=_T30;}_T21=env;_T22=_T21->ns;_T23=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T20(_T22,_T23);_T24=decl_ns_data;_T26=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T30)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T26;_T25=_T30;}_T27=decl_ns_data;_T28=_T27->ordinaries;_T29=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T30=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T30->tag=0;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T31=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_T31->tag=1;
# 349
_T31->f1=vd;_T2B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T31;}_T30->f1=(void*)_T2B;_T2A=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T30;}_T2C=(void*)_T2A;
# 347
_T24->ordinaries=_T25(_T28,_T29,_T2C);}goto _TL61;_TL60: _TL61: _TL5E:;}}}
# 353
void Cyc_Binding_interproc_binding_handler(unsigned,struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);struct _tuple11{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 355
static int Cyc_Binding_resolve_visit_f1(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void**_T4;void**_T5;int*_T6;int _T7;struct _handler_cons*_T8;int*_T9;int _TA;struct Cyc_Absyn_Exp*_TB;unsigned _TC;struct Cyc_Binding_Env*_TD;struct _tuple0*_TE;int*_TF;unsigned _T10;void*_T11;void**_T12;struct Cyc_Absyn_Exp*_T13;unsigned _T14;struct Cyc_Binding_Env*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T1A;void*_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T1D;struct Cyc_Warn_String_Warn_Warg_struct _T1E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1F;void**_T20;void**_T21;struct Cyc_Absyn_Exp*_T22;unsigned _T23;struct _fat_ptr _T24;void*_T25;struct Cyc_Binding_BindingError_exn_struct*_T26;char*_T27;char*_T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Qvar_Warn_Warg_struct _T2A;void**_T2B;void**_T2C;struct Cyc_Absyn_Exp*_T2D;unsigned _T2E;struct _fat_ptr _T2F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T30;int*_T31;int*_T32;struct Cyc_Binding_Env*_T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_List_List*_T37;struct Cyc_Absyn_Exp*_T38;int*_T39;int _T3A;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3B;void**_T3C;void**_T3D;int*_T3E;int _T3F;struct _handler_cons*_T40;int*_T41;int _T42;struct Cyc_Absyn_Exp*_T43;unsigned _T44;struct Cyc_Binding_Env*_T45;struct _tuple0*_T46;int*_T47;unsigned _T48;void*_T49;void**_T4A;struct Cyc_Absyn_Exp*_T4B;unsigned _T4C;struct Cyc_Binding_Env*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_List_List*_T4F;struct _tuple11*_T50;struct Cyc_List_List*_T51;void*_T52;struct Cyc_List_List*_T53;struct Cyc_Absyn_Exp*_T54;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T55;struct Cyc_Absyn_Aggrdecl*_T56;struct Cyc_Absyn_Datatypefield*_T57;struct Cyc_List_List*_T58;struct Cyc_Warn_Qvar_Warn_Warg_struct _T59;struct Cyc_Absyn_Datatypefield*_T5A;struct Cyc_Warn_String_Warn_Warg_struct _T5B;void**_T5C;void**_T5D;struct Cyc_Absyn_Exp*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct Cyc_Absyn_Exp*_T61;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T62;struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;void**_T65;void**_T66;struct Cyc_Absyn_Exp*_T67;unsigned _T68;struct _fat_ptr _T69;void*_T6A;struct Cyc_Binding_BindingError_exn_struct*_T6B;char*_T6C;char*_T6D;struct Cyc_Absyn_Exp*_T6E;unsigned _T6F;struct Cyc_Binding_Env*_T70;struct Cyc_Absyn_Exp*_T71;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T72;struct _tuple0**_T73;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T74;struct Cyc_Absyn_Aggrdecl**_T75;struct _handler_cons*_T76;int*_T77;int _T78;struct Cyc_Absyn_Aggrdecl**_T79;struct Cyc_Absyn_Aggrdecl*(*_T7A)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));struct Cyc_Absyn_Exp*_T7B;unsigned _T7C;struct Cyc_Binding_Env*_T7D;struct Cyc_Binding_NSCtxt*_T7E;struct _tuple0**_T7F;struct _tuple0*_T80;struct _tuple0**_T81;struct Cyc_Absyn_Aggrdecl**_T82;struct Cyc_Absyn_Aggrdecl*_T83;struct Cyc_Absyn_Aggrdecl*_T84;void*_T85;struct Cyc_Binding_BindingError_exn_struct*_T86;char*_T87;char*_T88;struct Cyc_Warn_String_Warn_Warg_struct _T89;struct Cyc_Warn_Qvar_Warn_Warg_struct _T8A;struct _tuple0**_T8B;void**_T8C;void**_T8D;struct Cyc_Absyn_Exp*_T8E;unsigned _T8F;struct _fat_ptr _T90;int _T91;struct Cyc_Binding_Env*_T92;struct Cyc_Dict_Dict*_T93;struct Cyc_Dict_Dict(*_T94)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T95)(int(*)(void*,void*));int(*_T96)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Absyn_Exp*_T97;unsigned _T98;struct Cyc_Binding_Env*_T99;struct Cyc_Absyn_Vardecl*_T9A;struct Cyc_Binding_Env*_T9B;struct Cyc_Binding_Env*_T9C;struct Cyc_Dict_Dict*_T9D;struct Cyc_Dict_Dict*_T9E;struct Cyc_Absyn_Exp*_T9F;unsigned _TA0;struct Cyc_Binding_Env*_TA1;struct Cyc_Absyn_Vardecl*_TA2;struct Cyc_Binding_Env*_TA3;struct Cyc_Dict_Dict*_TA4;struct Cyc_Dict_Dict*_TA5;struct Cyc_Absyn_Exp*_TA6;unsigned _TA7;struct Cyc_Binding_Env*_TA8;struct Cyc_List_List*_TA9;void*_TAA;struct Cyc_List_List*_TAB;struct Cyc_Absyn_MallocInfo _TAC;void**_TAD;void*_TAE;void*_TAF;struct _tuple8*_TB0;void*_TB1;void*_TB2;void*_TB3;void*_TB4;void*_TB5;struct Cyc_Absyn_Exp*_TB6;unsigned _TB7;struct Cyc_Binding_Env*_TB8;void*_TB9;_T0=e;{
void*_TBA=_T0->r;struct Cyc_Absyn_Exp*_TBB;struct Cyc_Absyn_Exp*_TBC;struct Cyc_List_List*_TBD;void*_TBE;struct Cyc_List_List*_TBF;void*_TC0;_T1=(int*)_TBA;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TBA;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TBA;_T4=& _T3->f1;_TC0=(void**)_T4;}{void**b=_TC0;_T5=b;{
# 358
void*_TC1=*_T5;struct _tuple0*_TC2;_T6=(int*)_TC1;_T7=*_T6;if(_T7!=0)goto _TL64;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TC3=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TC1;_TC2=_TC3->f1;}{struct _tuple0*q=_TC2;{struct _handler_cons _TC3;_T8=& _TC3;_push_handler(_T8);{int _TC4=0;_T9=_TC3.handler;_TA=setjmp(_T9);if(!_TA)goto _TL66;_TC4=1;goto _TL67;_TL66: _TL67: if(_TC4)goto _TL68;else{goto _TL6A;}_TL6A: _TB=e;_TC=_TB->loc;_TD=env;_TE=q;{
# 360
void*_TC5=Cyc_Binding_lookup_ordinary(_TC,_TD,_TE);struct Cyc_Absyn_Aggrdecl*_TC6;struct Cyc_Absyn_Enumfield*_TC7;struct Cyc_Absyn_Datatypefield*_TC8;void*_TC9;_TF=(int*)_TC5;_T10=*_TF;switch(_T10){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TCA=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TC5;_T11=_TCA->f1;_TC9=(void*)_T11;}{void*bnd=_TC9;_T12=b;
*_T12=bnd;_T13=e;_T14=_T13->loc;_T15=env;_T16=e;Cyc_Binding_interproc_binding_handler(_T14,_T15,_T16);goto _LL24;}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TCA=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TC5;_TC9=_TCA->f1;_TC8=_TCA->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TC9;struct Cyc_Absyn_Datatypefield*tuf=_TC8;_T17=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TCA=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TCA->tag=30;
_TCA->f1=0;_TCA->f2=tud;_TCA->f3=tuf;_T18=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TCA;}_T17->r=(void*)_T18;goto _LL24;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_TCA=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_TC5;_TC9=_TCA->f1;_TC7=_TCA->f2;}{struct Cyc_Absyn_Enumdecl*ed=_TC9;struct Cyc_Absyn_Enumfield*ef=_TC7;_T19=e;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_TCA=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_TCA->tag=31;
_TCA->f1=ed;_TCA->f2=ef;_T1A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_TCA;}_T19->r=(void*)_T1A;goto _LL24;}case 4:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TCA=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_TC5;_T1B=_TCA->f1;_TC9=(void*)_T1B;_TC7=_TCA->f2;}{void*t=_TC9;struct Cyc_Absyn_Enumfield*ef=_TC7;_T1C=e;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TCA=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_TCA->tag=32;
_TCA->f1=t;_TCA->f2=ef;_T1D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TCA;}_T1C->r=(void*)_T1D;goto _LL24;}default:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TCA=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TC5;_TC6=_TCA->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TC6;{struct Cyc_Warn_String_Warn_Warg_struct _TCA;_TCA.tag=0;
_TCA.f1=_tag_fat("bad occurrence of type name ",sizeof(char),29U);_T1E=_TCA;}{struct Cyc_Warn_String_Warn_Warg_struct _TCA=_T1E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCB;_TCB.tag=1;_TCB.f1=q;_T1F=_TCB;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCB=_T1F;void*_TCC[2];_T20=_TCC + 0;*_T20=& _TCA;_T21=_TCC + 1;*_T21=& _TCB;_T22=e;_T23=_T22->loc;_T24=_tag_fat(_TCC,sizeof(void*),2);Cyc_Warn_err2(_T23,_T24);}}goto _LL24;}}_LL24:;}_pop_handler();goto _TL69;_TL68: _T25=Cyc_Core_get_exn_thrown();{void*_TC5=(void*)_T25;void*_TC6;_T26=(struct Cyc_Binding_BindingError_exn_struct*)_TC5;_T27=_T26->tag;_T28=Cyc_Binding_BindingError;if(_T27!=_T28)goto _TL6C;{struct Cyc_Warn_String_Warn_Warg_struct _TC7;_TC7.tag=0;
# 368
_TC7.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T29=_TC7;}{struct Cyc_Warn_String_Warn_Warg_struct _TC7=_T29;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC8;_TC8.tag=1;_TC8.f1=q;_T2A=_TC8;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC8=_T2A;void*_TC9[2];_T2B=_TC9 + 0;*_T2B=& _TC7;_T2C=_TC9 + 1;*_T2C=& _TC8;_T2D=e;_T2E=_T2D->loc;_T2F=_tag_fat(_TC9,sizeof(void*),2);Cyc_Warn_err2(_T2E,_T2F);}}goto _LL2F;_TL6C: _TC6=_TC5;{void*exn=_TC6;_rethrow(exn);}_LL2F:;}_TL69:;}}
# 370
return 0;}_TL64:
 return 0;;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TBA;_TC0=_TC1->f1;_TBF=_TC1->f2;_T30=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TBA;_T31=& _T30->f4;_TBE=(int*)_T31;}{struct Cyc_Absyn_Exp*e1=_TC0;struct Cyc_List_List*es=_TBF;int*b=_TBE;_T32=b;
# 374
*_T32=1;{
# 376
struct Cyc_List_List*es2=es;_TL71: if(es2!=0)goto _TL6F;else{goto _TL70;}
_TL6F: _T33=env;_T34=es2;_T35=_T34->hd;_T36=(struct Cyc_Absyn_Exp*)_T35;Cyc_Binding_resolve_exp(_T33,_T36);_T37=es2;
# 376
es2=_T37->tl;goto _TL71;_TL70:;}_T38=e1;{
# 378
void*_TC1=_T38->r;void*_TC2;_T39=(int*)_TC1;_T3A=*_T39;if(_T3A!=1)goto _TL72;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TC3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TC1;_T3B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TC1;_T3C=& _T3B->f1;_TC2=(void**)_T3C;}{void**b=(void**)_TC2;_T3D=b;{
# 380
void*_TC3=*_T3D;struct _tuple0*_TC4;_T3E=(int*)_TC3;_T3F=*_T3E;if(_T3F!=0)goto _TL74;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TC5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TC3;_TC4=_TC5->f1;}{struct _tuple0*q=_TC4;struct _handler_cons _TC5;_T40=& _TC5;_push_handler(_T40);{int _TC6=0;_T41=_TC5.handler;_T42=setjmp(_T41);if(!_T42)goto _TL76;_TC6=1;goto _TL77;_TL76: _TL77: if(_TC6)goto _TL78;else{goto _TL7A;}_TL7A: _T43=e1;_T44=_T43->loc;_T45=env;_T46=q;{
# 382
void*_TC7=Cyc_Binding_lookup_ordinary(_T44,_T45,_T46);struct Cyc_Absyn_Datatypefield*_TC8;struct Cyc_Absyn_Datatypedecl*_TC9;struct Cyc_Absyn_Aggrdecl*_TCA;void*_TCB;_T47=(int*)_TC7;_T48=*_T47;switch(_T48){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TCC=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TC7;_T49=_TCC->f1;_TCB=(void*)_T49;}{void*bnd=_TCB;_T4A=b;
# 384
*_T4A=bnd;_T4B=e1;_T4C=_T4B->loc;_T4D=env;_T4E=e1;
Cyc_Binding_interproc_binding_handler(_T4C,_T4D,_T4E);{int _TCC=0;_npop_handler(0);return _TCC;}}case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TCC=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TC7;_TCA=_TCC->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TCA;
# 388
struct Cyc_List_List*dles=0;
_TL7F: if(es!=0)goto _TL7D;else{goto _TL7E;}
_TL7D:{struct Cyc_List_List*_TCC=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_TCD=_cycalloc(sizeof(struct _tuple11));_TCD->f0=0;_T51=es;_T52=_T51->hd;_TCD->f1=(struct Cyc_Absyn_Exp*)_T52;_T50=(struct _tuple11*)_TCD;}_TCC->hd=_T50;_TCC->tl=dles;_T4F=(struct Cyc_List_List*)_TCC;}dles=_T4F;_T53=es;
# 389
es=_T53->tl;goto _TL7F;_TL7E: _T54=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TCC=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TCC->tag=28;_T56=ad;
# 391
_TCC->f1=_T56->name;_TCC->f2=0;_TCC->f3=Cyc_List_imp_rev(dles);_TCC->f4=ad;_T55=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TCC;}_T54->r=(void*)_T55;{int _TCC=0;_npop_handler(0);return _TCC;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TCC=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TC7;_TC9=_TCC->f1;_TC8=_TCC->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TC9;struct Cyc_Absyn_Datatypefield*tuf=_TC8;_T57=tuf;_T58=_T57->typs;
# 394
if(_T58!=0)goto _TL80;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCC;_TCC.tag=1;_T5A=tuf;
_TCC.f1=_T5A->name;_T59=_TCC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCC=_T59;{struct Cyc_Warn_String_Warn_Warg_struct _TCD;_TCD.tag=0;_TCD.f1=_tag_fat(" is a constant, not a function",sizeof(char),31U);_T5B=_TCD;}{struct Cyc_Warn_String_Warn_Warg_struct _TCD=_T5B;void*_TCE[2];_T5C=_TCE + 0;*_T5C=& _TCC;_T5D=_TCE + 1;*_T5D=& _TCD;_T5E=e;_T5F=_T5E->loc;_T60=_tag_fat(_TCE,sizeof(void*),2);Cyc_Warn_err2(_T5F,_T60);}}goto _TL81;_TL80: _TL81: _T61=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TCC=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TCC->tag=30;
_TCC->f1=es;_TCC->f2=tud;_TCC->f3=tuf;_T62=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TCC;}_T61->r=(void*)_T62;{int _TCC=0;_npop_handler(0);return _TCC;}}case 4: goto _LL48;default: _LL48:{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCC;_TCC.tag=1;
# 400
_TCC.f1=q;_T63=_TCC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TCC=_T63;{struct Cyc_Warn_String_Warn_Warg_struct _TCD;_TCD.tag=0;_TCD.f1=_tag_fat(" is an enum constructor, not a function",sizeof(char),40U);_T64=_TCD;}{struct Cyc_Warn_String_Warn_Warg_struct _TCD=_T64;void*_TCE[2];_T65=_TCE + 0;*_T65=& _TCC;_T66=_TCE + 1;*_T66=& _TCD;_T67=e;_T68=_T67->loc;_T69=_tag_fat(_TCE,sizeof(void*),2);Cyc_Warn_err2(_T68,_T69);}}{int _TCC=0;_npop_handler(0);return _TCC;}};}_pop_handler();goto _TL79;_TL78: _T6A=Cyc_Core_get_exn_thrown();{void*_TC7=(void*)_T6A;void*_TC8;_T6B=(struct Cyc_Binding_BindingError_exn_struct*)_TC7;_T6C=_T6B->tag;_T6D=Cyc_Binding_BindingError;if(_T6C!=_T6D)goto _TL82;_T6E=e1;_T6F=_T6E->loc;_T70=env;_T71=e1;
# 403
Cyc_Binding_interproc_binding_handler(_T6F,_T70,_T71);
return 0;_TL82: _TC8=_TC7;{void*exn=_TC8;_rethrow(exn);};}_TL79:;}}goto _TL75;_TL74:
# 406
 return 0;_TL75:;}}goto _TL73;_TL72:
# 408
 Cyc_Binding_resolve_exp(env,e1);return 0;_TL73:;}}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TBA;_T72=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TBA;_T73=& _T72->f1;_TC0=(struct _tuple0**)_T73;_TBF=_TC1->f2;_TBD=_TC1->f3;_T74=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TBA;_T75=& _T74->f4;_TBE=(struct Cyc_Absyn_Aggrdecl**)_T75;}{struct _tuple0**tn=_TC0;struct Cyc_List_List*ts=_TBF;struct Cyc_List_List*dles=_TBD;struct Cyc_Absyn_Aggrdecl**adopt=(struct Cyc_Absyn_Aggrdecl**)_TBE;struct _handler_cons _TC1;_T76=& _TC1;_push_handler(_T76);{int _TC2=0;_T77=_TC1.handler;_T78=setjmp(_T77);if(!_T78)goto _TL84;_TC2=1;goto _TL85;_TL84: _TL85: if(_TC2)goto _TL86;else{goto _TL88;}_TL88: _T79=adopt;{
# 412
struct Cyc_Absyn_Aggrdecl*(*_TC3)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T7A=_TC3;}_T7B=e;_T7C=_T7B->loc;_T7D=env;_T7E=_T7D->ns;_T7F=tn;_T80=*_T7F;*_T79=_T7A(_T7C,_T7E,_T80,Cyc_Binding_lookup_aggrdecl);_T81=tn;_T82=adopt;_T83=*_T82;_T84=
_check_null(_T83);*_T81=_T84->name;{int _TC3=1;_npop_handler(0);return _TC3;}_pop_handler();goto _TL87;_TL86: _T85=Cyc_Core_get_exn_thrown();{void*_TC3=(void*)_T85;void*_TC4;_T86=(struct Cyc_Binding_BindingError_exn_struct*)_TC3;_T87=_T86->tag;_T88=Cyc_Binding_BindingError;if(_T87!=_T88)goto _TL89;{struct Cyc_Warn_String_Warn_Warg_struct _TC5;_TC5.tag=0;
# 416
_TC5.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_T89=_TC5;}{struct Cyc_Warn_String_Warn_Warg_struct _TC5=_T89;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC6;_TC6.tag=1;_T8B=tn;_TC6.f1=*_T8B;_T8A=_TC6;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC6=_T8A;void*_TC7[2];_T8C=_TC7 + 0;*_T8C=& _TC5;_T8D=_TC7 + 1;*_T8D=& _TC6;_T8E=e;_T8F=_T8E->loc;_T90=_tag_fat(_TC7,sizeof(void*),2);Cyc_Warn_err2(_T8F,_T90);}}
return 1;_TL89: _TC4=_TC3;{void*exn=_TC4;_rethrow(exn);};}_TL87:;}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TBA;_TC0=_TC1->f1;_TBC=_TC1->f2;_TBB=_TC1->f3;}{struct Cyc_Absyn_Vardecl*vd=_TC0;struct Cyc_Absyn_Exp*e1=_TBC;struct Cyc_Absyn_Exp*e2=_TBB;
# 420
Cyc_Binding_resolve_exp(env,e1);_T91=
Cyc_Binding_at_toplevel(env);if(!_T91)goto _TL8B;_T92=env;{struct Cyc_Dict_Dict*_TC1=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T95=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_TC2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T95;_T94=_TC2;}_T96=Cyc_strptrcmp;*_TC1=_T94(_T96);_T93=(struct Cyc_Dict_Dict*)_TC1;}_T92->local_vars=_T93;_T97=e;_T98=_T97->loc;_T99=env;_T9A=vd;
Cyc_Binding_resolve_and_add_vardecl(_T98,_T99,_T9A);
Cyc_Binding_resolve_exp(env,e2);_T9B=env;
_T9B->local_vars=0;
return 0;_TL8B: _T9C=env;_T9D=_T9C->local_vars;_T9E=
# 428
_check_null(_T9D);{struct Cyc_Dict_Dict old_locals=*_T9E;_T9F=e;_TA0=_T9F->loc;_TA1=env;_TA2=vd;
Cyc_Binding_resolve_and_add_vardecl(_TA0,_TA1,_TA2);
Cyc_Binding_resolve_exp(env,e2);_TA3=env;_TA4=_TA3->local_vars;_TA5=
_check_null(_TA4);*_TA5=old_locals;
return 0;}}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TBA;_TC0=_TC1->f1;_TBF=_TC1->f2;}{struct Cyc_Absyn_Exp*e1=_TC0;struct Cyc_List_List*ts=_TBF;
# 435
_TL90: if(ts!=0)goto _TL8E;else{goto _TL8F;}
_TL8E: _TA6=e;_TA7=_TA6->loc;_TA8=env;_TA9=ts;_TAA=_TA9->hd;Cyc_Binding_resolve_type(_TA7,_TA8,_TAA);_TAB=ts;
# 435
ts=_TAB->tl;goto _TL90;_TL8F:
# 437
 return 1;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_TBA;_TAC=_TC1->f1;_TC0=_TAC.elt_type;}{void**to=_TC0;
# 439
if(to==0)goto _TL91;_TAD=to;_TC0=*_TAD;goto _LLE;_TL91: return 1;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_TBA;_TAE=_TC1->f1;_TC0=(void*)_TAE;}_LLE: {void*topt=_TC0;
# 441
if(topt!=0)goto _TL93;return 1;_TL93: _TC0=topt;goto _LL10;}case 32:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TBA;_TAF=_TC1->f1;_TC0=(void*)_TAF;}_LL10: {void*t=_TC0;_TC0=t;goto _LL12;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_TBA;_TB0=_TC1->f1;{struct _tuple8 _TC2=*_TB0;_TC0=_TC2.f2;}}_LL12: {void*t=_TC0;_TC0=t;goto _LL14;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_TBA;_TB1=_TC1->f1;_TC0=(void*)_TB1;}_LL14: {void*t=_TC0;_TC0=t;goto _LL16;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_TBA;_TB2=_TC1->f1;_TC0=(void*)_TB2;}_LL16: {void*t=_TC0;_TC0=t;goto _LL18;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_TBA;_TB3=_TC1->f1;_TC0=(void*)_TB3;}_LL18: {void*t=_TC0;_TC0=t;goto _LL1A;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TBA;_TB4=_TC1->f2;_TC0=(void*)_TB4;}_LL1A: {void*t=_TC0;_TC0=t;goto _LL1C;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TC1=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TBA;_TB5=_TC1->f1;_TC0=(void*)_TB5;}_LL1C: {void*t=_TC0;_TB6=e;_TB7=_TB6->loc;_TB8=env;_TB9=t;
# 450
Cyc_Binding_resolve_type(_TB7,_TB8,_TB9);
return 1;}default:
# 453
 return 1;};}}
# 456
static int Cyc_Binding_resolve_visit_f2(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Binding_Env*_T3;struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict*_T5;struct Cyc_Binding_Env*_T6;struct Cyc_Dict_Dict*_T7;struct Cyc_Dict_Dict*_T8;_T0=s;{
void*_T9=_T0->r;struct Cyc_List_List*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Stmt*_TC;struct Cyc_Absyn_Decl*_TD;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_TE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T9;_TD=_TE->f1;_TC=_TE->f2;}{struct Cyc_Absyn_Decl*d=_TD;struct Cyc_Absyn_Stmt*s1=_TC;_T3=env;_T4=_T3->local_vars;_T5=
# 459
_check_null(_T4);{struct Cyc_Dict_Dict old_locals=*_T5;
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);_T6=env;_T7=_T6->local_vars;_T8=
_check_null(_T7);*_T8=old_locals;
return 0;}}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_TE=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T9;_TB=_TE->f1;_TA=_TE->f2;}{struct Cyc_Absyn_Exp*e=_TB;struct Cyc_List_List*scs=_TA;
# 465
Cyc_Binding_resolve_exp(env,e);
Cyc_Binding_resolve_scs(env,scs);
return 0;}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_TE=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T9;_TC=_TE->f1;_TA=_TE->f2;}{struct Cyc_Absyn_Stmt*s1=_TC;struct Cyc_List_List*scs=_TA;
# 469
Cyc_Binding_resolve_stmt(env,s1);
Cyc_Binding_resolve_scs(env,scs);
return 0;}default:
 return 1;};}}
# 475
static void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Binding_Env*_T2;struct Cyc_Absyn_Stmt*_T3;_T1=Cyc_Absyn_visit_stmt;{
void(*_T4)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T4;}_T2=env;_T3=s;_T0(Cyc_Binding_resolve_visit_f1,Cyc_Binding_resolve_visit_f2,_T2,_T3);}
# 478
static void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Binding_Env*_T2;struct Cyc_Absyn_Exp*_T3;_T1=Cyc_Absyn_visit_exp;{
void(*_T4)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*))_T1;_T0=_T4;}_T2=env;_T3=e;_T0(Cyc_Binding_resolve_visit_f1,Cyc_Binding_resolve_visit_f2,_T2,_T3);}
# 482
static void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){struct Cyc_Binding_Env*_T0;struct Cyc_Dict_Dict*_T1;struct Cyc_Dict_Dict*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Binding_Env*_T5;struct Cyc_Dict_Dict*_T6;struct Cyc_Dict_Dict*_T7;struct Cyc_List_List*_T8;_T0=env;_T1=_T0->local_vars;_T2=
_check_null(_T1);{struct Cyc_Dict_Dict old_locals=*_T2;
_TL99: if(scs!=0)goto _TL97;else{goto _TL98;}
_TL97: _T3=scs;_T4=_T3->hd;{struct Cyc_Absyn_Switch_clause*_T9=(struct Cyc_Absyn_Switch_clause*)_T4;struct Cyc_Absyn_Stmt*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Pat*_TC;{struct Cyc_Absyn_Switch_clause _TD=*_T9;_TC=_TD.pattern;_TB=_TD.where_clause;_TA=_TD.body;}{struct Cyc_Absyn_Pat*pattern=_TC;struct Cyc_Absyn_Exp*where_clause=_TB;struct Cyc_Absyn_Stmt*body=_TA;
Cyc_Binding_resolve_pat(env,pattern);
if(where_clause==0)goto _TL9A;
Cyc_Binding_resolve_exp(env,where_clause);goto _TL9B;_TL9A: _TL9B:
 Cyc_Binding_resolve_stmt(env,body);_T5=env;_T6=_T5->local_vars;_T7=
_check_null(_T6);*_T7=old_locals;}}_T8=scs;
# 484
scs=_T8->tl;goto _TL99;_TL98:
# 492
 return;}}
# 494
static void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;
_TL9F: if(fs!=0)goto _TL9D;else{goto _TL9E;}
_TL9D: _T0=fs;_T1=_T0->hd;{struct Cyc_Absyn_Aggrfield*_T3=(struct Cyc_Absyn_Aggrfield*)_T1;struct Cyc_Absyn_Exp*_T4;struct Cyc_List_List*_T5;struct Cyc_Absyn_Exp*_T6;void*_T7;struct Cyc_Absyn_Tqual _T8;struct _fat_ptr*_T9;{struct Cyc_Absyn_Aggrfield _TA=*_T3;_T9=_TA.name;_T8=_TA.tq;_T7=_TA.type;_T6=_TA.width;_T5=_TA.attributes;_T4=_TA.requires_clause;}{struct _fat_ptr*fn=_T9;struct Cyc_Absyn_Tqual tq=_T8;void*t=_T7;struct Cyc_Absyn_Exp*width=_T6;struct Cyc_List_List*atts=_T5;struct Cyc_Absyn_Exp*requires_clause=_T4;
Cyc_Binding_resolve_type(loc,env,t);
if(width==0)goto _TLA0;
Cyc_Binding_resolve_exp(env,width);goto _TLA1;_TLA0: _TLA1:
 if(requires_clause==0)goto _TLA2;
Cyc_Binding_resolve_exp(env,requires_clause);goto _TLA3;_TLA2: _TLA3:;}}_T2=fs;
# 495
fs=_T2->tl;goto _TL9F;_TL9E:
# 503
 return;}
# 506
static struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;struct _tuple0*_T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_VarargInfo*_T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;void**_TB;unsigned _TC;struct _fat_ptr _TD;struct _tuple0*_TE;void*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;
# 512
struct Cyc_List_List*vds=0;
_TLA7: if(args!=0)goto _TLA5;else{goto _TLA6;}
_TLA5: _T0=args;_T1=_T0->hd;{struct _tuple8*_T12=(struct _tuple8*)_T1;void*_T13;struct Cyc_Absyn_Tqual _T14;struct _fat_ptr*_T15;{struct _tuple8 _T16=*_T12;_T15=_T16.f0;_T14=_T16.f1;_T13=_T16.f2;}{struct _fat_ptr*a=_T15;struct Cyc_Absyn_Tqual tq=_T14;void*t=_T13;
if(a!=0)goto _TLA8;{struct Cyc_List_List*_T16=_cycalloc(sizeof(struct Cyc_List_List));
_T16->hd=0;_T16->tl=vds;_T2=(struct Cyc_List_List*)_T16;}vds=_T2;goto _TLA4;_TLA8:{struct _tuple0*_T16=_cycalloc(sizeof(struct _tuple0));
# 519
_T16->f0=Cyc_Absyn_Loc_n();_T16->f1=a;_T3=(struct _tuple0*)_T16;}_T4=t;{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_T3,_T4,0,0);_T5=vd;
_T5->tq=tq;{struct Cyc_List_List*_T16=_cycalloc(sizeof(struct Cyc_List_List));
_T16->hd=vd;_T16->tl=vds;_T6=(struct Cyc_List_List*)_T16;}vds=_T6;}}}_TLA4: _T7=args;
# 513
args=_T7->tl;goto _TLA7;_TLA6:
# 523
 if(vai==0)goto _TLAA;_T8=vai;{
struct Cyc_Absyn_VarargInfo _T12=*_T8;int _T13;void*_T14;struct Cyc_Absyn_Tqual _T15;struct _fat_ptr*_T16;_T16=_T12.name;_T15=_T12.tq;_T14=_T12.type;_T13=_T12.inject;{struct _fat_ptr*v=_T16;struct Cyc_Absyn_Tqual tq=_T15;void*t=_T14;int i=_T13;
if(v!=0)goto _TLAC;_T9=need_va_name;
if(!_T9)goto _TLAE;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
_T17.f1=_tag_fat("missing name for vararg",sizeof(char),24U);_TA=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TA;void*_T18[1];_TB=_T18 + 0;*_TB=& _T17;_TC=loc;_TD=_tag_fat(_T18,sizeof(void*),1);Cyc_Warn_err2(_TC,_TD);}goto _TLAF;_TLAE: _TLAF: goto _TLAD;
# 530
_TLAC:{void*typ=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);{struct _tuple0*_T17=_cycalloc(sizeof(struct _tuple0));
_T17->f0=Cyc_Absyn_Loc_n();_T17->f1=v;_TE=(struct _tuple0*)_T17;}_TF=typ;{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_TE,_TF,0,0);{struct Cyc_List_List*_T17=_cycalloc(sizeof(struct Cyc_List_List));
_T17->hd=vd;_T17->tl=vds;_T10=(struct Cyc_List_List*)_T17;}vds=_T10;}}_TLAD:;}}goto _TLAB;_TLAA: _TLAB:
# 535
 vds=Cyc_List_imp_rev(vds);_T11=vds;
return _T11;}
# 539
static void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo*i){struct Cyc_Absyn_FnInfo*_T0;void*_T1;unsigned _T2;struct Cyc_Binding_Env*_T3;struct Cyc_Absyn_FnInfo*_T4;void*_T5;unsigned _T6;struct Cyc_Binding_Env*_T7;struct Cyc_Absyn_FnInfo*_T8;void*_T9;struct Cyc_Absyn_FnInfo*_TA;unsigned _TB;struct Cyc_Binding_Env*_TC;struct Cyc_List_List*_TD;void*_TE;struct _tuple8*_TF;struct _tuple8 _T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_Absyn_FnInfo*_T13;struct Cyc_Absyn_VarargInfo*_T14;unsigned _T15;struct Cyc_Binding_Env*_T16;struct Cyc_Absyn_FnInfo*_T17;struct Cyc_Absyn_VarargInfo*_T18;void*_T19;unsigned _T1A;struct Cyc_Binding_Env*_T1B;struct Cyc_Absyn_FnInfo*_T1C;struct Cyc_List_List*_T1D;unsigned _T1E;struct Cyc_Binding_Env*_T1F;struct Cyc_Absyn_FnInfo*_T20;struct Cyc_List_List*_T21;struct Cyc_Absyn_FnInfo*_T22;struct Cyc_Absyn_Exp*_T23;struct Cyc_Binding_Env*_T24;struct Cyc_Absyn_FnInfo*_T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_Absyn_FnInfo*_T27;struct Cyc_Absyn_Exp*_T28;struct Cyc_Binding_Env*_T29;struct Cyc_Absyn_FnInfo*_T2A;struct Cyc_Absyn_Exp*_T2B;struct Cyc_Absyn_FnInfo*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Absyn_FnInfo*_T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Binding_Env*_T30;struct Cyc_Dict_Dict*_T31;struct Cyc_Dict_Dict*_T32;struct _fat_ptr*_T33;struct _tuple0*_T34;struct Cyc_Absyn_FnInfo*_T35;void*_T36;struct Cyc_Absyn_FnInfo*_T37;struct Cyc_Binding_Env*_T38;struct Cyc_Dict_Dict*_T39;struct Cyc_Dict_Dict*_T3A;struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T3C)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T3D;struct Cyc_Dict_Dict*_T3E;struct Cyc_Dict_Dict*_T3F;struct Cyc_Dict_Dict _T40;struct _fat_ptr*_T41;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T42;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T43;void*_T44;struct Cyc_Absyn_FnInfo*_T45;struct Cyc_Absyn_Exp*_T46;struct Cyc_Binding_Env*_T47;struct Cyc_Absyn_FnInfo*_T48;struct Cyc_Absyn_Exp*_T49;struct Cyc_Absyn_FnInfo*_T4A;struct Cyc_Absyn_Exp*_T4B;struct Cyc_Binding_Env*_T4C;struct Cyc_Absyn_FnInfo*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_Binding_Env*_T4F;struct Cyc_Dict_Dict*_T50;struct Cyc_Dict_Dict*_T51;_T0=i;_T1=_T0->effect;
if(_T1==0)goto _TLB0;_T2=loc;_T3=env;_T4=i;_T5=_T4->effect;
Cyc_Binding_resolve_type(_T2,_T3,_T5);goto _TLB1;_TLB0: _TLB1: _T6=loc;_T7=env;_T8=i;_T9=_T8->ret_type;
Cyc_Binding_resolve_type(_T6,_T7,_T9);_TA=i;{
struct Cyc_List_List*args=_TA->args;_TLB5: if(args!=0)goto _TLB3;else{goto _TLB4;}
_TLB3: _TB=loc;_TC=env;_TD=args;_TE=_TD->hd;_TF=(struct _tuple8*)_TE;_T10=*_TF;_T11=_T10.f2;Cyc_Binding_resolve_type(_TB,_TC,_T11);_T12=args;
# 543
args=_T12->tl;goto _TLB5;_TLB4:;}_T13=i;_T14=_T13->cyc_varargs;
# 545
if(_T14==0)goto _TLB6;_T15=loc;_T16=env;_T17=i;_T18=_T17->cyc_varargs;_T19=_T18->type;
Cyc_Binding_resolve_type(_T15,_T16,_T19);goto _TLB7;_TLB6: _TLB7: _T1A=loc;_T1B=env;_T1C=i;_T1D=_T1C->effconstr;
Cyc_Binding_resolve_effconstr(_T1A,_T1B,_T1D);_T1E=loc;_T1F=env;_T20=i;_T21=_T20->qual_bnd;
# 549
Cyc_Binding_resolve_qualbnd(_T1E,_T1F,_T21);_T22=i;_T23=_T22->checks_clause;
if(_T23==0)goto _TLB8;_T24=env;_T25=i;_T26=_T25->checks_clause;
Cyc_Binding_resolve_exp(_T24,_T26);goto _TLB9;_TLB8: _TLB9: _T27=i;_T28=_T27->requires_clause;
if(_T28==0)goto _TLBA;_T29=env;_T2A=i;_T2B=_T2A->requires_clause;
Cyc_Binding_resolve_exp(_T29,_T2B);goto _TLBB;_TLBA: _TLBB: _T2C=i;_T2D=_T2C->ensures_clause;
if(_T2D!=0)goto _TLBE;else{goto _TLBF;}_TLBF: _T2E=i;_T2F=_T2E->throws_clause;if(_T2F!=0)goto _TLBE;else{goto _TLBC;}
# 556
_TLBE: _T30=env;_T31=_T30->local_vars;_T32=_check_null(_T31);{struct Cyc_Dict_Dict locs=*_T32;
struct _fat_ptr*v;v=_cycalloc(sizeof(struct _fat_ptr));_T33=v;*_T33=_tag_fat("return_value",sizeof(char),13U);{struct _tuple0*_T52=_cycalloc(sizeof(struct _tuple0));
_T52->f0=Cyc_Absyn_Loc_n();_T52->f1=v;_T34=(struct _tuple0*)_T52;}_T35=i;_T36=_T35->ret_type;{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_T34,_T36,0,0);_T37=i;
_T37->return_value=vd;_T38=env;_T39=_T38->local_vars;_T3A=
_check_null(_T39);_T3C=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T52)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T3C;_T3B=_T52;}_T3D=env;_T3E=_T3D->local_vars;_T3F=_check_null(_T3E);_T40=*_T3F;_T41=v;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T52=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T52->tag=0;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T53=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T53->tag=4;
_T53->f1=vd;_T43=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T53;}_T52->f1=(void*)_T43;_T42=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T52;}_T44=(void*)_T42;
# 560
*_T3A=_T3B(_T40,_T41,_T44);_T45=i;_T46=_T45->ensures_clause;
# 562
if(_T46==0)goto _TLC0;_T47=env;_T48=i;_T49=_T48->ensures_clause;
Cyc_Binding_resolve_exp(_T47,_T49);goto _TLC1;_TLC0: _TLC1: _T4A=i;_T4B=_T4A->throws_clause;
# 565
if(_T4B==0)goto _TLC2;_T4C=env;_T4D=i;_T4E=_T4D->throws_clause;
Cyc_Binding_resolve_exp(_T4C,_T4E);goto _TLC3;_TLC2: _TLC3: _T4F=env;_T50=_T4F->local_vars;_T51=
# 568
_check_null(_T50);*_T51=locs;}}goto _TLBD;_TLBC: _TLBD:;}
# 577
static void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD;union Cyc_Absyn_DatatypeInfo*_TE;unsigned _TF;struct Cyc_Binding_Env*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_List_List*_T13;union Cyc_Absyn_DatatypeInfo*_T14;struct _union_DatatypeInfo_UnknownDatatype _T15;unsigned _T16;struct _union_DatatypeInfo_UnknownDatatype _T17;struct Cyc_Absyn_UnknownDatatypeInfo _T18;struct _union_DatatypeInfo_UnknownDatatype _T19;struct Cyc_Absyn_UnknownDatatypeInfo _T1A;struct _handler_cons*_T1B;int*_T1C;int _T1D;struct Cyc_List_List*(*_T1E)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T1F;struct Cyc_Binding_Env*_T20;struct Cyc_Binding_NSCtxt*_T21;struct _tuple0*_T22;struct Cyc_List_List*_T23;void*_T24;union Cyc_Absyn_DatatypeInfo*_T25;struct Cyc_Absyn_UnknownDatatypeInfo _T26;struct Cyc_Absyn_Datatypedecl*_T27;void*_T28;struct Cyc_Binding_BindingError_exn_struct*_T29;char*_T2A;char*_T2B;void*_T2C;void*_T2D;void*_T2E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2F;void*_T30;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T31;union Cyc_Absyn_DatatypeFieldInfo*_T32;unsigned _T33;struct Cyc_Binding_Env*_T34;struct Cyc_List_List*_T35;void*_T36;struct Cyc_List_List*_T37;union Cyc_Absyn_DatatypeFieldInfo*_T38;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T39;unsigned _T3A;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3C;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3D;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3E;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3F;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T40;struct _handler_cons*_T41;int*_T42;int _T43;int*_T44;int _T45;struct Cyc_List_List*(*_T46)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T47;struct Cyc_Binding_Env*_T48;struct Cyc_Binding_NSCtxt*_T49;struct _tuple0*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_Absyn_Datatypedecl*_T4D;struct _tuple0*_T4E;struct Cyc_Absyn_Datatypedecl*_T4F;struct _tuple0*_T50;int _T51;struct Cyc_Warn_Qvar_Warn_Warg_struct _T52;struct Cyc_Absyn_Datatypefield*_T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;struct Cyc_Warn_Qvar_Warn_Warg_struct _T55;struct Cyc_Absyn_Datatypedecl*_T56;struct Cyc_Warn_String_Warn_Warg_struct _T57;struct Cyc_Warn_Qvar_Warn_Warg_struct _T58;struct Cyc_Absyn_Datatypedecl*_T59;struct Cyc_Warn_String_Warn_Warg_struct _T5A;void**_T5B;void**_T5C;void**_T5D;void**_T5E;void**_T5F;void**_T60;unsigned _T61;struct _fat_ptr _T62;union Cyc_Absyn_DatatypeFieldInfo*_T63;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T64;struct Cyc_Absyn_Datatypedecl*_T65;struct Cyc_Absyn_Datatypefield*_T66;void*_T67;struct Cyc_Binding_BindingError_exn_struct*_T68;char*_T69;char*_T6A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T6B;struct Cyc_Warn_String_Warn_Warg_struct _T6C;void**_T6D;void**_T6E;unsigned _T6F;struct _fat_ptr _T70;void*_T71;void*_T72;void*_T73;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74;void*_T75;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T76;union Cyc_Absyn_AggrInfo*_T77;unsigned _T78;struct Cyc_Binding_Env*_T79;struct Cyc_List_List*_T7A;void*_T7B;struct Cyc_List_List*_T7C;union Cyc_Absyn_AggrInfo*_T7D;struct _union_AggrInfo_UnknownAggr _T7E;unsigned _T7F;struct _union_AggrInfo_UnknownAggr _T80;struct _tuple2 _T81;struct _union_AggrInfo_UnknownAggr _T82;struct _tuple2 _T83;struct _union_AggrInfo_UnknownAggr _T84;struct _tuple2 _T85;struct _handler_cons*_T86;int*_T87;int _T88;struct Cyc_Absyn_Aggrdecl*(*_T89)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T8A;struct Cyc_Binding_Env*_T8B;struct Cyc_Binding_NSCtxt*_T8C;struct _tuple0*_T8D;struct Cyc_Absyn_Aggrdecl*_T8E;enum Cyc_Absyn_AggrKind _T8F;int _T90;enum Cyc_Absyn_AggrKind _T91;int _T92;struct Cyc_Warn_String_Warn_Warg_struct _T93;void**_T94;unsigned _T95;struct _fat_ptr _T96;enum Cyc_Absyn_AggrKind _T97;int _T98;struct Cyc_Absyn_Aggrdecl*_T99;struct Cyc_Absyn_AggrdeclImpl*_T9A;struct Cyc_Absyn_Aggrdecl*_T9B;struct Cyc_Absyn_AggrdeclImpl*_T9C;int _T9D;struct Cyc_Core_Opt*_T9E;void*_T9F;int _TA0;struct Cyc_Warn_String_Warn_Warg_struct _TA1;void**_TA2;unsigned _TA3;struct _fat_ptr _TA4;union Cyc_Absyn_AggrInfo*_TA5;enum Cyc_Absyn_AggrKind _TA6;struct Cyc_Absyn_Aggrdecl*_TA7;struct _tuple0*_TA8;struct Cyc_Core_Opt*_TA9;void*_TAA;struct Cyc_Binding_BindingError_exn_struct*_TAB;char*_TAC;char*_TAD;void*_TAE;void*_TAF;void*_TB0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB1;void*_TB2;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TB3;struct _tuple0**_TB4;struct _handler_cons*_TB5;int*_TB6;int _TB7;struct Cyc_Absyn_Enumdecl*(*_TB8)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TB9;struct Cyc_Binding_Env*_TBA;struct Cyc_Binding_NSCtxt*_TBB;struct _tuple0**_TBC;struct _tuple0*_TBD;struct _tuple0**_TBE;struct Cyc_Absyn_Enumdecl*_TBF;void*_TC0;struct Cyc_Binding_BindingError_exn_struct*_TC1;char*_TC2;char*_TC3;unsigned _TC4;struct Cyc_Binding_Env*_TC5;struct _tuple0**_TC6;struct _tuple0*_TC7;void*_TC8;void*_TC9;struct Cyc_Binding_ResolveNSEnv*(*_TCA)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TCB;struct Cyc_Binding_NSCtxt*_TCC;struct Cyc_Binding_Env*_TCD;struct Cyc_Binding_NSCtxt*_TCE;struct Cyc_List_List*_TCF;int _TD0;union Cyc_Absyn_Nmspace _TD1;struct Cyc_List_List*_TD2;void*_TD3;struct Cyc_Absyn_Enumfield*_TD4;struct Cyc_Absyn_Exp*_TD5;struct Cyc_Binding_Env*_TD6;struct Cyc_Absyn_Enumfield*_TD7;struct Cyc_Absyn_Exp*_TD8;struct Cyc_Binding_ResolveNSEnv*_TD9;struct Cyc_Dict_Dict(*_TDA)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TDB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TDC;struct Cyc_Dict_Dict _TDD;struct Cyc_Absyn_Enumfield*_TDE;struct _tuple0*_TDF;struct _tuple0 _TE0;struct _fat_ptr*_TE1;struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TE2;void*_TE3;struct Cyc_List_List*_TE4;void*_TE5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE6;struct Cyc_List_List*_TE7;void*_TE8;void*_TE9;unsigned _TEA;struct Cyc_Binding_Env*_TEB;struct Cyc_List_List*_TEC;void*_TED;struct Cyc_List_List*_TEE;void*_TEF;void*_TF0;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TF1;struct _tuple0**_TF2;void*_TF3;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TF4;struct Cyc_Absyn_Typedefdecl**_TF5;unsigned _TF6;struct Cyc_Binding_Env*_TF7;struct Cyc_List_List*_TF8;void*_TF9;struct Cyc_List_List*_TFA;struct _handler_cons*_TFB;int*_TFC;int _TFD;struct Cyc_Absyn_Typedefdecl*(*_TFE)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TFF;struct Cyc_Binding_Env*_T100;struct Cyc_Binding_NSCtxt*_T101;struct _tuple0**_T102;struct _tuple0*_T103;struct _tuple0**_T104;struct Cyc_Absyn_Typedefdecl*_T105;void*_T106;struct Cyc_Binding_BindingError_exn_struct*_T107;char*_T108;char*_T109;struct Cyc_Warn_String_Warn_Warg_struct _T10A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T10B;struct _tuple0**_T10C;void**_T10D;void**_T10E;unsigned _T10F;struct _fat_ptr _T110;void*_T111;void*_T112;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T113;void***_T114;struct Cyc_Binding_Env*_T115;struct Cyc_Binding_Env*_T116;struct Cyc_Absyn_TypeDecl*_T117;int*_T118;unsigned _T119;struct Cyc_Binding_Env*_T11A;struct Cyc_Absyn_Decl*_T11B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T11C;struct Cyc_Absyn_TypeDecl*_T11D;struct Cyc_Binding_Env*_T11E;struct Cyc_Absyn_Decl*_T11F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T120;struct Cyc_Absyn_TypeDecl*_T121;struct Cyc_Binding_Env*_T122;struct Cyc_Absyn_Decl*_T123;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T124;struct Cyc_Absyn_TypeDecl*_T125;struct Cyc_Binding_Env*_T126;void*_T127;void*_T128;void*_T129;int _T12A;void*_T12B;void*_T12C;void*_T12D;struct Cyc_Absyn_PtrInfo _T12E;struct Cyc_Absyn_PtrInfo _T12F;struct Cyc_Absyn_PtrAtts _T130;struct Cyc_Absyn_PtrInfo _T131;struct Cyc_Absyn_PtrAtts _T132;void*_T133;struct Cyc_Absyn_ArrayInfo _T134;struct Cyc_Absyn_ArrayInfo _T135;void*_T136;void*_T137;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T138;struct Cyc_Absyn_FnInfo*_T139;unsigned _T13A;struct Cyc_Binding_Env*_T13B;struct Cyc_Absyn_FnInfo*_T13C;struct Cyc_List_List*_T13D;struct Cyc_Absyn_FnInfo*_T13E;struct Cyc_Absyn_VarargInfo*_T13F;struct Cyc_Absyn_FnInfo*_T140;struct Cyc_Binding_Env*_T141;struct Cyc_Binding_Env*_T142;struct Cyc_Dict_Dict*_T143;struct Cyc_Binding_Env*_T144;struct Cyc_Dict_Dict*_T145;struct Cyc_Binding_Env*_T146;struct Cyc_Dict_Dict*_T147;struct Cyc_Dict_Dict(*_T148)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T149)(int(*)(void*,void*));int(*_T14A)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T14B;void*_T14C;struct Cyc_Absyn_Vardecl*_T14D;struct Cyc_Binding_Env*_T14E;struct Cyc_Dict_Dict*_T14F;struct Cyc_Dict_Dict*_T150;struct Cyc_Dict_Dict(*_T151)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T152)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T153;struct Cyc_Dict_Dict*_T154;struct Cyc_Dict_Dict*_T155;struct Cyc_Dict_Dict _T156;struct Cyc_List_List*_T157;void*_T158;struct Cyc_Absyn_Vardecl*_T159;struct _tuple0*_T15A;struct _fat_ptr*_T15B;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T15C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T15D;struct Cyc_List_List*_T15E;void*_T15F;void*_T160;struct Cyc_List_List*_T161;struct Cyc_Binding_Env*_T162;void*_T163;void*_T164;struct Cyc_Absyn_Exp*_T165;void*_T166;void*_T167;void*_T168;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 22: _T8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T9=_T169->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T16A=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T9;_TA=t;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC;_TE=& _TD->f1;_T168=(union Cyc_Absyn_DatatypeInfo*)_TE;}_T167=_T169->f2;}{union Cyc_Absyn_DatatypeInfo*info=_T168;struct Cyc_List_List*targs=_T167;
# 580
_TLC9: if(targs!=0)goto _TLC7;else{goto _TLC8;}
_TLC7: _TF=loc;_T10=env;_T11=targs;_T12=_T11->hd;Cyc_Binding_resolve_type(_TF,_T10,_T12);_T13=targs;
# 580
targs=_T13->tl;goto _TLC9;_TLC8: _T14=info;{
# 582
union Cyc_Absyn_DatatypeInfo _T169=*_T14;int _T16A;struct _tuple0*_T16B;_T15=_T169.UnknownDatatype;_T16=_T15.tag;if(_T16!=1)goto _TLCA;_T17=_T169.UnknownDatatype;_T18=_T17.val;_T16B=_T18.name;_T19=_T169.UnknownDatatype;_T1A=_T19.val;_T16A=_T1A.is_extensible;{struct _tuple0*qv=_T16B;int b=_T16A;struct _handler_cons _T16C;_T1B=& _T16C;_push_handler(_T1B);{int _T16D=0;_T1C=_T16C.handler;_T1D=setjmp(_T1C);if(!_T1D)goto _TLCC;_T16D=1;goto _TLCD;_TLCC: _TLCD: if(_T16D)goto _TLCE;else{goto _TLD0;}_TLD0:{
# 585
struct Cyc_List_List*(*_T16E)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T1E=_T16E;}_T1F=loc;_T20=env;_T21=_T20->ns;_T22=qv;_T23=_T1E(_T1F,_T21,_T22,Cyc_Binding_lookup_datatypedecl);_T24=_T23->hd;{struct Cyc_Absyn_Datatypedecl*tud=(struct Cyc_Absyn_Datatypedecl*)_T24;_T25=info;{struct Cyc_Absyn_UnknownDatatypeInfo _T16E;_T27=tud;
_T16E.name=_T27->name;_T16E.is_extensible=b;_T26=_T16E;}*_T25=Cyc_Absyn_UnknownDatatype(_T26);_npop_handler(0);return;}_pop_handler();goto _TLCF;_TLCE: _T28=Cyc_Core_get_exn_thrown();{void*_T16E=(void*)_T28;void*_T16F;_T29=(struct Cyc_Binding_BindingError_exn_struct*)_T16E;_T2A=_T29->tag;_T2B=Cyc_Binding_BindingError;if(_T2A!=_T2B)goto _TLD1;
# 589
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLD1: _T16F=_T16E;{void*exn=_T16F;_rethrow(exn);};}_TLCF:;}}goto _TLCB;_TLCA:
 return;_TLCB:;}}case 23: _T2C=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2C;_T2D=_T169->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T16A=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T2D;_T2E=t;_T2F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2E;_T30=_T2F->f1;_T31=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T30;_T32=& _T31->f1;_T168=(union Cyc_Absyn_DatatypeFieldInfo*)_T32;}_T167=_T169->f2;}{union Cyc_Absyn_DatatypeFieldInfo*info=_T168;struct Cyc_List_List*targs=_T167;
# 593
_TLD6: if(targs!=0)goto _TLD4;else{goto _TLD5;}
_TLD4: _T33=loc;_T34=env;_T35=targs;_T36=_T35->hd;Cyc_Binding_resolve_type(_T33,_T34,_T36);_T37=targs;
# 593
targs=_T37->tl;goto _TLD6;_TLD5: _T38=info;{
# 595
union Cyc_Absyn_DatatypeFieldInfo _T169=*_T38;int _T16A;struct _tuple0*_T16B;struct _tuple0*_T16C;_T39=_T169.UnknownDatatypefield;_T3A=_T39.tag;if(_T3A!=1)goto _TLD7;_T3B=_T169.UnknownDatatypefield;_T3C=_T3B.val;_T16C=_T3C.datatype_name;_T3D=_T169.UnknownDatatypefield;_T3E=_T3D.val;_T16B=_T3E.field_name;_T3F=_T169.UnknownDatatypefield;_T40=_T3F.val;_T16A=_T40.is_extensible;{struct _tuple0*qvd=_T16C;struct _tuple0*qvf=_T16B;int b=_T16A;{struct _handler_cons _T16D;_T41=& _T16D;_push_handler(_T41);{int _T16E=0;_T42=_T16D.handler;_T43=setjmp(_T42);if(!_T43)goto _TLD9;_T16E=1;goto _TLDA;_TLD9: _TLDA: if(_T16E)goto _TLDB;else{goto _TLDD;}_TLDD:{
# 600
void*_T16F=Cyc_Binding_lookup_ordinary(loc,env,qvf);struct Cyc_Absyn_Datatypefield*_T170;struct Cyc_Absyn_Datatypedecl*_T171;_T44=(int*)_T16F;_T45=*_T44;if(_T45!=2)goto _TLDE;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T172=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T16F;_T171=_T172->f1;_T170=_T172->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T171;struct Cyc_Absyn_Datatypefield*tuf=_T170;{
# 602
struct Cyc_List_List*(*_T172)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T46=_T172;}_T47=loc;_T48=env;_T49=_T48->ns;_T4A=qvd;_T4B=_T46(_T47,_T49,_T4A,Cyc_Binding_lookup_datatypedecl);_T4C=_T4B->hd;{struct Cyc_Absyn_Datatypedecl*tud2=(struct Cyc_Absyn_Datatypedecl*)_T4C;_T4D=tud;_T4E=_T4D->name;_T4F=tud2;_T50=_T4F->name;_T51=
Cyc_Absyn_qvar_cmp(_T4E,_T50);if(_T51==0)goto _TLE0;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T172;_T172.tag=1;_T53=tuf;
_T172.f1=_T53->name;_T52=_T172;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T172=_T52;{struct Cyc_Warn_String_Warn_Warg_struct _T173;_T173.tag=0;_T173.f1=_tag_fat(" is a variant of ",sizeof(char),18U);_T54=_T173;}{struct Cyc_Warn_String_Warn_Warg_struct _T173=_T54;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T174;_T174.tag=1;_T56=tud2;_T174.f1=_T56->name;_T55=_T174;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T174=_T55;{struct Cyc_Warn_String_Warn_Warg_struct _T175;_T175.tag=0;_T175.f1=_tag_fat(" not ",sizeof(char),6U);_T57=_T175;}{struct Cyc_Warn_String_Warn_Warg_struct _T175=_T57;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T176;_T176.tag=1;_T59=tud;
_T176.f1=_T59->name;_T58=_T176;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T176=_T58;{struct Cyc_Warn_String_Warn_Warg_struct _T177;_T177.tag=0;_T177.f1=_tag_fat(" (shadowing not yet implemented properly)",sizeof(char),42U);_T5A=_T177;}{struct Cyc_Warn_String_Warn_Warg_struct _T177=_T5A;void*_T178[6];_T5B=_T178 + 0;*_T5B=& _T172;_T5C=_T178 + 1;*_T5C=& _T173;_T5D=_T178 + 2;*_T5D=& _T174;_T5E=_T178 + 3;*_T5E=& _T175;_T5F=_T178 + 4;*_T5F=& _T176;_T60=_T178 + 5;*_T60=& _T177;_T61=loc;_T62=_tag_fat(_T178,sizeof(void*),6);Cyc_Warn_err2(_T61,_T62);}}}}}}_npop_handler(0);return;_TLE0: _T63=info;{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T172;_T65=tud;
# 608
_T172.datatype_name=_T65->name;_T66=tuf;
_T172.field_name=_T66->name;_T172.is_extensible=b;_T64=_T172;}
# 608
*_T63=Cyc_Absyn_UnknownDatatypefield(_T64);_npop_handler(0);return;}}_TLDE: goto _LL34;_LL34:;}_pop_handler();goto _TLDC;_TLDB: _T67=Cyc_Core_get_exn_thrown();{void*_T16F=(void*)_T67;void*_T170;_T68=(struct Cyc_Binding_BindingError_exn_struct*)_T16F;_T69=_T68->tag;_T6A=Cyc_Binding_BindingError;if(_T69!=_T6A)goto _TLE2;goto _LL39;_TLE2: _T170=_T16F;{void*exn=_T170;_rethrow(exn);}_LL39:;}_TLDC:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16D;_T16D.tag=1;
# 614
_T16D.f1=qvf;_T6B=_T16D;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16D=_T6B;{struct Cyc_Warn_String_Warn_Warg_struct _T16E;_T16E.tag=0;_T16E.f1=_tag_fat(" is not a datatype field",sizeof(char),25U);_T6C=_T16E;}{struct Cyc_Warn_String_Warn_Warg_struct _T16E=_T6C;void*_T16F[2];_T6D=_T16F + 0;*_T6D=& _T16D;_T6E=_T16F + 1;*_T6E=& _T16E;_T6F=loc;_T70=_tag_fat(_T16F,sizeof(void*),2);Cyc_Warn_err2(_T6F,_T70);}}return;}_TLD7:
 return;;}}case 24: _T71=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T71;_T72=_T169->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T16A=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T72;_T73=t;_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T73;_T75=_T74->f1;_T76=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T75;_T77=& _T76->f1;_T168=(union Cyc_Absyn_AggrInfo*)_T77;}_T167=_T169->f2;}{union Cyc_Absyn_AggrInfo*info=_T168;struct Cyc_List_List*targs=_T167;
# 619
_TLE7: if(targs!=0)goto _TLE5;else{goto _TLE6;}
_TLE5: _T78=loc;_T79=env;_T7A=targs;_T7B=_T7A->hd;Cyc_Binding_resolve_type(_T78,_T79,_T7B);_T7C=targs;
# 619
targs=_T7C->tl;goto _TLE7;_TLE6: _T7D=info;{
# 621
union Cyc_Absyn_AggrInfo _T169=*_T7D;struct Cyc_Core_Opt*_T16A;struct _tuple0*_T16B;enum Cyc_Absyn_AggrKind _T16C;_T7E=_T169.UnknownAggr;_T7F=_T7E.tag;if(_T7F!=1)goto _TLE8;_T80=_T169.UnknownAggr;_T81=_T80.val;_T16C=_T81.f0;_T82=_T169.UnknownAggr;_T83=_T82.val;_T16B=_T83.f1;_T84=_T169.UnknownAggr;_T85=_T84.val;_T16A=_T85.f2;{enum Cyc_Absyn_AggrKind ak=_T16C;struct _tuple0*qv=_T16B;struct Cyc_Core_Opt*bo=_T16A;struct _handler_cons _T16D;_T86=& _T16D;_push_handler(_T86);{int _T16E=0;_T87=_T16D.handler;_T88=setjmp(_T87);if(!_T88)goto _TLEA;_T16E=1;goto _TLEB;_TLEA: _TLEB: if(_T16E)goto _TLEC;else{goto _TLEE;}_TLEE:{
# 624
struct Cyc_Absyn_Aggrdecl*(*_T16F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T89=_T16F;}_T8A=loc;_T8B=env;_T8C=_T8B->ns;_T8D=qv;{struct Cyc_Absyn_Aggrdecl*ad=_T89(_T8A,_T8C,_T8D,Cyc_Binding_lookup_aggrdecl);_T8E=ad;_T8F=_T8E->kind;_T90=(int)_T8F;_T91=ak;_T92=(int)_T91;
if(_T90==_T92)goto _TLEF;{struct Cyc_Warn_String_Warn_Warg_struct _T16F;_T16F.tag=0;
_T16F.f1=_tag_fat("struct vs. union mismatch with earlier declaration",sizeof(char),51U);_T93=_T16F;}{struct Cyc_Warn_String_Warn_Warg_struct _T16F=_T93;void*_T170[1];_T94=_T170 + 0;*_T94=& _T16F;_T95=loc;_T96=_tag_fat(_T170,sizeof(void*),1);Cyc_Warn_err2(_T95,_T96);}goto _TLF0;_TLEF: _TLF0: _T97=ak;_T98=(int)_T97;
if(_T98!=1)goto _TLF1;if(bo==0)goto _TLF1;_T99=ad;_T9A=_T99->impl;if(_T9A==0)goto _TLF1;_T9B=ad;_T9C=_T9B->impl;_T9D=_T9C->tagged;_T9E=bo;_T9F=_T9E->v;_TA0=(int)_T9F;if(_T9D==_TA0)goto _TLF1;{struct Cyc_Warn_String_Warn_Warg_struct _T16F;_T16F.tag=0;
# 629
_T16F.f1=_tag_fat("@tagged mismatch with earlier declaration",sizeof(char),42U);_TA1=_T16F;}{struct Cyc_Warn_String_Warn_Warg_struct _T16F=_TA1;void*_T170[1];_TA2=_T170 + 0;*_TA2=& _T16F;_TA3=loc;_TA4=_tag_fat(_T170,sizeof(void*),1);Cyc_Warn_err2(_TA3,_TA4);}goto _TLF2;_TLF1: _TLF2: _TA5=info;_TA6=ak;_TA7=ad;_TA8=_TA7->name;_TA9=bo;
*_TA5=Cyc_Absyn_UnknownAggr(_TA6,_TA8,_TA9);_npop_handler(0);return;}_pop_handler();goto _TLED;_TLEC: _TAA=Cyc_Core_get_exn_thrown();{void*_T16F=(void*)_TAA;void*_T170;_TAB=(struct Cyc_Binding_BindingError_exn_struct*)_T16F;_TAC=_TAB->tag;_TAD=Cyc_Binding_BindingError;if(_TAC!=_TAD)goto _TLF3;
# 633
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLF3: _T170=_T16F;{void*exn=_T170;_rethrow(exn);};}_TLED:;}}goto _TLE9;_TLE8:
 return;_TLE9:;}}case 19: _TAE=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAE;_TAF=_T169->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T16A=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TAF;_TB0=t;_TB1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB0;_TB2=_TB1->f1;_TB3=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TB2;_TB4=& _TB3->f1;_T168=(struct _tuple0**)_TB4;_T167=_T16A->f2;}}{struct _tuple0**qv=_T168;struct Cyc_Absyn_Enumdecl*edo=_T167;
# 637
if(edo==0)goto _TLF5;
return;_TLF5: {struct _handler_cons _T169;_TB5=& _T169;_push_handler(_TB5);{int _T16A=0;_TB6=_T169.handler;_TB7=setjmp(_TB6);if(!_TB7)goto _TLF7;_T16A=1;goto _TLF8;_TLF7: _TLF8: if(_T16A)goto _TLF9;else{goto _TLFB;}_TLFB:{
# 640
struct Cyc_Absyn_Enumdecl*(*_T16B)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TB8=_T16B;}_TB9=loc;_TBA=env;_TBB=_TBA->ns;_TBC=qv;_TBD=*_TBC;{struct Cyc_Absyn_Enumdecl*ed=_TB8(_TB9,_TBB,_TBD,Cyc_Binding_lookup_enumdecl);_TBE=qv;_TBF=ed;
*_TBE=_TBF->name;_npop_handler(0);return;}_pop_handler();goto _TLFA;_TLF9: _TC0=Cyc_Core_get_exn_thrown();{void*_T16B=(void*)_TC0;void*_T16C;_TC1=(struct Cyc_Binding_BindingError_exn_struct*)_T16B;_TC2=_TC1->tag;_TC3=Cyc_Binding_BindingError;if(_TC2!=_TC3)goto _TLFC;_TC4=loc;_TC5=env;_TC6=qv;_TC7=*_TC6;
# 644
Cyc_Binding_absolutize_topdecl(_TC4,_TC5,_TC7,2U);return;_TLFC: _T16C=_T16B;{void*exn=_T16C;_rethrow(exn);};}_TLFA:;}}}case 20: _TC8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC8;_TC9=_T169->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_T16A=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_TC9;_T168=_T16A->f1;}}{struct Cyc_List_List*fs=_T168;{
# 682
struct Cyc_Binding_ResolveNSEnv*(*_T169)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TCA=_T169;}_TCB=env;_TCC=_TCB->ns;_TCD=env;_TCE=_TCD->ns;_TCF=_TCE->curr_ns;_TD0=
Cyc_Binding_in_cinclude(env);_TD1=
# 682
Cyc_Absyn_Abs_n(_TCF,_TD0);{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TCA(_TCC,_TD1);
# 684
_TL101: if(fs!=0)goto _TLFF;else{goto _TL100;}
_TLFF: _TD2=fs;_TD3=_TD2->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TD3;_TD4=f;_TD5=_TD4->tag;
if(_TD5==0)goto _TL102;_TD6=env;_TD7=f;_TD8=_TD7->tag;
Cyc_Binding_resolve_exp(_TD6,_TD8);goto _TL103;_TL102: _TL103: _TD9=decl_ns_data;_TDB=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T169)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TDB;_TDA=_T169;}_TDC=decl_ns_data;_TDD=_TDC->ordinaries;_TDE=f;_TDF=_TDE->name;_TE0=*_TDF;_TE1=_TE0.f1;{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T169=_cycalloc(sizeof(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct));_T169->tag=4;_T169->f1=t;_T169->f2=f;_TE2=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T169;}_TE3=(void*)_TE2;_TD9->ordinaries=_TDA(_TDD,_TE1,_TE3);}_TE4=fs;
# 684
fs=_TE4->tl;goto _TL101;_TL100:
# 690
 return;}}default: _TE5=t;_TE6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE5;_TE7=_TE6->f2;if(_TE7!=0)goto _TL104;_LL18:
# 700
 return;_TL104: _TE8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE8;_TE9=_T169->f1;_T168=(void*)_TE9;_T167=_T169->f2;}{void*c=_T168;struct Cyc_List_List*ts=_T167;
# 702
_TL109: if(ts!=0)goto _TL107;else{goto _TL108;}
_TL107: _TEA=loc;_TEB=env;_TEC=ts;_TED=_TEC->hd;Cyc_Binding_resolve_type(_TEA,_TEB,_TED);_TEE=ts;
# 702
ts=_TEE->tl;goto _TL109;_TL108:
# 704
 return;}};case 8: _TEF=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TEF;_TF0=t;_TF1=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TF0;_TF2=& _TF1->f1;_T168=(struct _tuple0**)_TF2;_T167=_T169->f2;_TF3=t;_TF4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TF3;_TF5=& _TF4->f3;_T166=(struct Cyc_Absyn_Typedefdecl**)_TF5;}{struct _tuple0**tdn=_T168;struct Cyc_List_List*targs=_T167;struct Cyc_Absyn_Typedefdecl**tdo=_T166;
# 647
_TL10D: if(targs!=0)goto _TL10B;else{goto _TL10C;}
_TL10B: _TF6=loc;_TF7=env;_TF8=targs;_TF9=_TF8->hd;Cyc_Binding_resolve_type(_TF6,_TF7,_TF9);_TFA=targs;
# 647
targs=_TFA->tl;goto _TL10D;_TL10C:{struct _handler_cons _T169;_TFB=& _T169;_push_handler(_TFB);{int _T16A=0;_TFC=_T169.handler;_TFD=setjmp(_TFC);if(!_TFD)goto _TL10E;_T16A=1;goto _TL10F;_TL10E: _TL10F: if(_T16A)goto _TL110;else{goto _TL112;}_TL112:{
# 650
struct Cyc_Absyn_Typedefdecl*(*_T16B)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TFE=_T16B;}_TFF=loc;_T100=env;_T101=_T100->ns;_T102=tdn;_T103=*_T102;{struct Cyc_Absyn_Typedefdecl*td=_TFE(_TFF,_T101,_T103,Cyc_Binding_lookup_typedefdecl);_T104=tdn;_T105=td;
# 652
*_T104=_T105->name;_npop_handler(0);return;}_pop_handler();goto _TL111;_TL110: _T106=Cyc_Core_get_exn_thrown();{void*_T16B=(void*)_T106;void*_T16C;_T107=(struct Cyc_Binding_BindingError_exn_struct*)_T16B;_T108=_T107->tag;_T109=Cyc_Binding_BindingError;if(_T108!=_T109)goto _TL113;goto _LL4D;_TL113: _T16C=_T16B;{void*exn=_T16C;_rethrow(exn);}_LL4D:;}_TL111:;}}{struct Cyc_Warn_String_Warn_Warg_struct _T169;_T169.tag=0;
# 655
_T169.f1=_tag_fat("unbound typdef name ",sizeof(char),21U);_T10A=_T169;}{struct Cyc_Warn_String_Warn_Warg_struct _T169=_T10A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16A;_T16A.tag=1;_T10C=tdn;_T16A.f1=*_T10C;_T10B=_T16A;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16A=_T10B;void*_T16B[2];_T10D=_T16B + 0;*_T10D=& _T169;_T10E=_T16B + 1;*_T10E=& _T16A;_T10F=loc;_T110=_tag_fat(_T16B,sizeof(void*),2);Cyc_Warn_err2(_T10F,_T110);}}
return;}case 10: _T111=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T111;_T168=_T169->f1;_T112=t;_T113=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T112;_T114=& _T113->f2;_T167=(void***)_T114;}{struct Cyc_Absyn_TypeDecl*td=_T168;void***to=_T167;_T115=env;{
# 660
struct Cyc_Dict_Dict*old_locals=_T115->local_vars;_T116=env;
_T116->local_vars=0;_T117=td;{
void*_T169=_T117->r;struct Cyc_Absyn_Datatypedecl*_T16A;struct Cyc_Absyn_Enumdecl*_T16B;struct Cyc_Absyn_Aggrdecl*_T16C;_T118=(int*)_T169;_T119=*_T118;switch(_T119){case 0:{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T16D=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T169;_T16C=_T16D->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T16C;_T11A=env;{struct Cyc_Absyn_Decl*_T16D=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T16E=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T16E->tag=5;
# 664
_T16E->f1=ad;_T11C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T16E;}_T16D->r=(void*)_T11C;_T11D=td;_T16D->loc=_T11D->loc;_T11B=(struct Cyc_Absyn_Decl*)_T16D;}Cyc_Binding_resolve_decl(_T11A,_T11B);goto _LL52;}case 1:{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T16D=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T169;_T16B=_T16D->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T16B;_T11E=env;{struct Cyc_Absyn_Decl*_T16D=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T16E=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T16E->tag=7;
# 666
_T16E->f1=ed;_T120=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T16E;}_T16D->r=(void*)_T120;_T121=td;_T16D->loc=_T121->loc;_T11F=(struct Cyc_Absyn_Decl*)_T16D;}Cyc_Binding_resolve_decl(_T11E,_T11F);goto _LL52;}default:{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T16D=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T169;_T16A=_T16D->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T16A;_T122=env;{struct Cyc_Absyn_Decl*_T16D=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T16E=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T16E->tag=6;
# 668
_T16E->f1=tud;_T124=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T16E;}_T16D->r=(void*)_T124;_T125=td;_T16D->loc=_T125->loc;_T123=(struct Cyc_Absyn_Decl*)_T16D;}Cyc_Binding_resolve_decl(_T122,_T123);goto _LL52;}}_LL52:;}_T126=env;
# 670
_T126->local_vars=old_locals;
return;}}case 7: _T127=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T127;_T168=_T169->f3;}{struct Cyc_List_List*fs=_T168;
# 677
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 3: _T128=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T169=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T128;_T129=_T169->f2;_T168=(void*)_T129;}{void*to=_T168;_T12A=Cyc_Flags_resolve;
# 692
if(!_T12A)goto _TL116;
Cyc_BansheeIf_register_toplevel_cvar(t);goto _LL0;
# 697
_TL116: _T168=to;goto _LL14;}case 1: _T12B=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T169=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T12B;_T12C=_T169->f2;_T168=(void*)_T12C;}_LL14: {void*to=_T168;
if(to==0)goto _TL118;Cyc_Binding_resolve_type(loc,env,to);goto _TL119;_TL118: _TL119: return;}case 2: goto _LL18;case 4: _T12D=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T12D;_T12E=_T169->f1;_T168=_T12E.elt_type;_T12F=_T169->f1;_T130=_T12F.ptr_atts;_T167=_T130.eff;_T131=_T169->f1;_T132=_T131.ptr_atts;_T166=_T132.bounds;}{void*t1=_T168;void*t2=_T167;void*bds=_T166;
# 707
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 5: _T133=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T133;_T134=_T169->f1;_T168=_T134.elt_type;_T135=_T169->f1;_T165=_T135.num_elts;}{void*t1=_T168;struct Cyc_Absyn_Exp*eo=_T165;
# 713
Cyc_Binding_resolve_type(loc,env,t1);
if(eo==0)goto _TL11A;Cyc_Binding_resolve_exp(env,eo);goto _TL11B;_TL11A: _TL11B:
 return;}case 6: _T136=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T136;_T137=t;_T138=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T137;_T139=& _T138->f1;_T168=(struct Cyc_Absyn_FnInfo*)_T139;}{struct Cyc_Absyn_FnInfo*i=(struct Cyc_Absyn_FnInfo*)_T168;_T13A=loc;_T13B=env;_T13C=i;_T13D=_T13C->args;_T13E=i;_T13F=_T13E->cyc_varargs;{
# 720
struct Cyc_List_List*vds=Cyc_Binding_get_fun_vardecls(0,_T13A,_T13B,_T13D,_T13F);_T140=i;
_T140->arg_vardecls=vds;_T141=env;{
struct Cyc_Dict_Dict*old_locals=_T141->local_vars;
if(old_locals==0)goto _TL11C;_T142=env;{struct Cyc_Dict_Dict*_T169=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T144=env;_T145=_T144->local_vars;
*_T169=*_T145;_T143=(struct Cyc_Dict_Dict*)_T169;}_T142->local_vars=_T143;goto _TL11D;
# 726
_TL11C: _T146=env;{struct Cyc_Dict_Dict*_T169=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T149=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T16A)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T149;_T148=_T16A;}_T14A=Cyc_strptrcmp;*_T169=_T148(_T14A);_T147=(struct Cyc_Dict_Dict*)_T169;}_T146->local_vars=_T147;_TL11D:{
struct Cyc_List_List*vds1=vds;_TL121: if(vds1!=0)goto _TL11F;else{goto _TL120;}
_TL11F: _T14B=vds1;_T14C=_T14B->hd;_T14D=(struct Cyc_Absyn_Vardecl*)_T14C;if(_T14D==0)goto _TL122;_T14E=env;_T14F=_T14E->local_vars;_T150=
_check_null(_T14F);_T152=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T169)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T152;_T151=_T169;}_T153=env;_T154=_T153->local_vars;_T155=_check_null(_T154);_T156=*_T155;_T157=vds1;_T158=_T157->hd;_T159=(struct Cyc_Absyn_Vardecl*)_T158;_T15A=_T159->name;_T15B=_T15A->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T169=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T169->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T16A=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T16A->tag=3;_T15E=vds1;_T15F=_T15E->hd;
_T16A->f1=(struct Cyc_Absyn_Vardecl*)_T15F;_T15D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T16A;}_T169->f1=(void*)_T15D;_T15C=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T169;}_T160=(void*)_T15C;
# 729
*_T150=_T151(_T156,_T15B,_T160);goto _TL123;_TL122: _TL123: _T161=vds1;
# 727
vds1=_T161->tl;goto _TL121;_TL120:;}
# 732
Cyc_Binding_resolve_function_stuff(loc,env,i);_T162=env;
_T162->local_vars=old_locals;
return;}}}case 9: _T163=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T163;_T165=_T169->f1;}{struct Cyc_Absyn_Exp*e=_T165;_T165=e;goto _LL24;}default: _T164=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T169=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T164;_T165=_T169->f1;}_LL24: {struct Cyc_Absyn_Exp*e=_T165;
# 737
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple12{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 741
static void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;void*_T1;int*_T2;unsigned _T3;void*_T4;struct _handler_cons*_T5;int*_T6;int _T7;int*_T8;unsigned _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;void**_TB;unsigned _TC;struct _fat_ptr _TD;struct Cyc_Absyn_Pat*_TE;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TF;struct Cyc_Absyn_Pat*_T10;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T11;void*_T12;struct Cyc_Absyn_Pat*_T13;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T14;void*_T15;struct Cyc_Binding_BindingError_exn_struct*_T16;char*_T17;char*_T18;struct Cyc_Absyn_Pat*_T19;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1C;void*_T1D;void*_T1E;struct Cyc_Binding_Env*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Pat*_T22;struct Cyc_List_List*_T23;struct _handler_cons*_T24;int*_T25;int _T26;int*_T27;unsigned _T28;struct Cyc_List_List*_T29;struct _tuple12*_T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_List_List*_T2D;struct Cyc_Absyn_Aggrdecl**_T2E;union Cyc_Absyn_AggrInfo _T2F;struct Cyc_Absyn_Pat*_T30;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T31;struct Cyc_Absyn_Pat*_T32;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T33;struct Cyc_Warn_String_Warn_Warg_struct _T34;void**_T35;unsigned _T36;struct _fat_ptr _T37;void*_T38;struct Cyc_Binding_BindingError_exn_struct*_T39;char*_T3A;char*_T3B;struct Cyc_Warn_Qvar_Warn_Warg_struct _T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;void**_T3E;void**_T3F;unsigned _T40;struct _fat_ptr _T41;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;void*_T47;void*_T48;void*_T49;int*_T4A;int _T4B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4C;void*_T4D;int*_T4E;int _T4F;void*_T50;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T51;void*_T52;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T53;union Cyc_Absyn_AggrInfo*_T54;union Cyc_Absyn_AggrInfo*_T55;struct _union_AggrInfo_UnknownAggr _T56;unsigned _T57;struct _union_AggrInfo_UnknownAggr _T58;struct _tuple2 _T59;struct _union_AggrInfo_UnknownAggr _T5A;struct _tuple2 _T5B;struct _handler_cons*_T5C;int*_T5D;int _T5E;struct Cyc_Absyn_Aggrdecl*(*_T5F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T60;struct Cyc_Binding_Env*_T61;struct Cyc_Binding_NSCtxt*_T62;struct _tuple0*_T63;union Cyc_Absyn_AggrInfo*_T64;struct Cyc_Absyn_Aggrdecl**_T65;void*_T66;struct Cyc_Binding_BindingError_exn_struct*_T67;char*_T68;char*_T69;struct Cyc_Warn_Qvar_Warn_Warg_struct _T6A;struct Cyc_Warn_String_Warn_Warg_struct _T6B;void**_T6C;void**_T6D;struct Cyc_Absyn_Pat*_T6E;unsigned _T6F;struct _fat_ptr _T70;struct Cyc_Absyn_Pat*_T71;unsigned _T72;struct Cyc_Binding_Env*_T73;void*_T74;struct Cyc_Binding_Env*_T75;struct Cyc_List_List*_T76;void*_T77;struct _tuple12*_T78;struct _tuple12 _T79;struct Cyc_Absyn_Pat*_T7A;struct Cyc_List_List*_T7B;void*_T7C;struct Cyc_Binding_Env*_T7D;struct Cyc_List_List*_T7E;void*_T7F;struct Cyc_Absyn_Pat*_T80;struct Cyc_List_List*_T81;unsigned _T82;void*_T83;_T0=p;{struct Cyc_Absyn_Pat _T84=*_T0;_T83=_T84.r;_T82=_T84.loc;}{void*r=_T83;unsigned loc=_T82;struct Cyc_Absyn_Pat*_T84;int _T85;struct Cyc_List_List*_T86;void*_T87;_T1=r;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 14: _T4=r;{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T4;_T87=_T88->f1;}{struct _tuple0*qv=_T87;{struct _handler_cons _T88;_T5=& _T88;_push_handler(_T5);{int _T89=0;_T6=_T88.handler;_T7=setjmp(_T6);if(!_T7)goto _TL125;_T89=1;goto _TL126;_TL125: _TL126: if(_T89)goto _TL127;else{goto _TL129;}_TL129:{
# 745
void*_T8A=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Enumfield*_T8B;struct Cyc_Absyn_Datatypefield*_T8C;void*_T8D;_T8=(int*)_T8A;_T9=*_T8;switch(_T9){case 0: goto _LL26;case 1:{struct Cyc_Warn_String_Warn_Warg_struct _T8E;_T8E.tag=0;
# 748
_T8E.f1=_tag_fat("struct tag used without arguments in pattern",sizeof(char),45U);_TA=_T8E;}{struct Cyc_Warn_String_Warn_Warg_struct _T8E=_TA;void*_T8F[1];_TB=_T8F + 0;*_TB=& _T8E;_TC=loc;_TD=_tag_fat(_T8F,sizeof(void*),1);Cyc_Warn_err2(_TC,_TD);}_npop_handler(0);return;case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T8E=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T8A;_T8D=_T8E->f1;_T8C=_T8E->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T8D;struct Cyc_Absyn_Datatypefield*tuf=_T8C;_TE=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T8E=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T8E->tag=7;
# 750
_T8E->f1=tud;_T8E->f2=tuf;_T8E->f3=0;_T8E->f4=0;_TF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T8E;}_TE->r=(void*)_TF;_npop_handler(0);return;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T8E=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T8A;_T8D=_T8E->f1;_T8B=_T8E->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T8D;struct Cyc_Absyn_Enumfield*ef=_T8B;_T10=p;{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T8E=_cycalloc(sizeof(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct));_T8E->tag=12;
# 752
_T8E->f1=ed;_T8E->f2=ef;_T11=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T8E;}_T10->r=(void*)_T11;_npop_handler(0);return;}default:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T8E=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T8A;_T12=_T8E->f1;_T8D=(void*)_T12;_T8B=_T8E->f2;}{void*t=_T8D;struct Cyc_Absyn_Enumfield*ef=_T8B;_T13=p;{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T8E=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct));_T8E->tag=13;
# 754
_T8E->f1=t;_T8E->f2=ef;_T14=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T8E;}_T13->r=(void*)_T14;_npop_handler(0);return;}}_LL26:;}_pop_handler();goto _TL128;_TL127: _T15=Cyc_Core_get_exn_thrown();{void*_T8A=(void*)_T15;void*_T8B;_T16=(struct Cyc_Binding_BindingError_exn_struct*)_T8A;_T17=_T16->tag;_T18=Cyc_Binding_BindingError;if(_T17!=_T18)goto _TL12B;goto _LL31;_TL12B: _T8B=_T8A;{void*exn=_T8B;_rethrow(exn);}_LL31:;}_TL128:;}}{
# 756
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0,0);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T19=p;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T88->tag=1;
_T88->f1=vd;_T1B=& Cyc_Absyn_Wild_p_val;_T1C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1B;_T1D=(void*)_T1C;_T88->f2=Cyc_Absyn_new_pat(_T1D,0U);_T1A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T88;}_T19->r=(void*)_T1A;
return;}}case 15: _T1E=r;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T1E;_T87=_T88->f1;_T86=_T88->f2;_T85=_T88->f3;}{struct _tuple0*qv=_T87;struct Cyc_List_List*ps=_T86;int dots=_T85;{
# 762
struct Cyc_List_List*ps2=ps;_TL130: if(ps2!=0)goto _TL12E;else{goto _TL12F;}
_TL12E: _T1F=env;_T20=ps2;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Pat*)_T21;Cyc_Binding_resolve_pat(_T1F,_T22);_T23=ps2;
# 762
ps2=_T23->tl;goto _TL130;_TL12F:;}{struct _handler_cons _T88;_T24=& _T88;_push_handler(_T24);{int _T89=0;_T25=_T88.handler;_T26=setjmp(_T25);if(!_T26)goto _TL131;_T89=1;goto _TL132;_TL131: _TL132: if(_T89)goto _TL133;else{goto _TL135;}_TL135:{
# 764
void*_T8A=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Datatypefield*_T8B;struct Cyc_Absyn_Datatypedecl*_T8C;struct Cyc_Absyn_Aggrdecl*_T8D;_T27=(int*)_T8A;_T28=*_T27;switch(_T28){case 0: goto _LL36;case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T8E=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T8A;_T8D=_T8E->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T8D;
# 767
struct Cyc_List_List*new_ps=0;
_TL13A: if(ps!=0)goto _TL138;else{goto _TL139;}
_TL138:{struct Cyc_List_List*_T8E=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T8F=_cycalloc(sizeof(struct _tuple12));_T8F->f0=0;_T2B=ps;_T2C=_T2B->hd;_T8F->f1=(struct Cyc_Absyn_Pat*)_T2C;_T2A=(struct _tuple12*)_T8F;}_T8E->hd=_T2A;_T8E->tl=new_ps;_T29=(struct Cyc_List_List*)_T8E;}new_ps=_T29;_T2D=ps;
# 768
ps=_T2D->tl;goto _TL13A;_TL139:{struct Cyc_Absyn_Aggrdecl**_T8E=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 770
*_T8E=ad;_T2E=(struct Cyc_Absyn_Aggrdecl**)_T8E;}_T2F=Cyc_Absyn_KnownAggr(_T2E);{void*atype=Cyc_Absyn_aggr_type(_T2F,0);_T30=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8E=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T8E->tag=6;
_T8E->f1=atype;_T8E->f2=0;_T8E->f3=0;_T8E->f4=Cyc_List_imp_rev(new_ps);_T8E->f5=dots;_T31=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8E;}_T30->r=(void*)_T31;_npop_handler(0);return;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T8E=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T8A;_T8C=_T8E->f1;_T8B=_T8E->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T8C;struct Cyc_Absyn_Datatypefield*tuf=_T8B;_T32=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T8E=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T8E->tag=7;
# 774
_T8E->f1=tud;_T8E->f2=tuf;_T8E->f3=ps;_T8E->f4=dots;_T33=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T8E;}_T32->r=(void*)_T33;_npop_handler(0);return;}case 3: goto _LL40;default: _LL40:{struct Cyc_Warn_String_Warn_Warg_struct _T8E;_T8E.tag=0;
# 777
_T8E.f1=_tag_fat("enum tag used with arguments in pattern",sizeof(char),40U);_T34=_T8E;}{struct Cyc_Warn_String_Warn_Warg_struct _T8E=_T34;void*_T8F[1];_T35=_T8F + 0;*_T35=& _T8E;_T36=loc;_T37=_tag_fat(_T8F,sizeof(void*),1);Cyc_Warn_err2(_T36,_T37);}_npop_handler(0);return;}_LL36:;}_pop_handler();goto _TL134;_TL133: _T38=Cyc_Core_get_exn_thrown();{void*_T8A=(void*)_T38;void*_T8B;_T39=(struct Cyc_Binding_BindingError_exn_struct*)_T8A;_T3A=_T39->tag;_T3B=Cyc_Binding_BindingError;if(_T3A!=_T3B)goto _TL13B;goto _LL41;_TL13B: _T8B=_T8A;{void*exn=_T8B;_rethrow(exn);}_LL41:;}_TL134:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T88;_T88.tag=1;
# 779
_T88.f1=qv;_T3C=_T88;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T88=_T3C;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat(" is not a constructor in pattern",sizeof(char),33U);_T3D=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T3D;void*_T8A[2];_T3E=_T8A + 0;*_T3E=& _T88;_T3F=_T8A + 1;*_T3F=& _T89;_T40=loc;_T41=_tag_fat(_T8A,sizeof(void*),2);Cyc_Warn_err2(_T40,_T41);}}return;}case 0:
# 781
 return;case 3: _T42=r;{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T42;_T87=_T88->f1;_T84=_T88->f2;}{struct Cyc_Absyn_Vardecl*vd=_T87;struct Cyc_Absyn_Pat*p2=_T84;_T87=vd;_T84=p2;goto _LLD;}case 1: _T43=r;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T43;_T87=_T88->f1;_T84=_T88->f2;}_LLD: {struct Cyc_Absyn_Vardecl*vd=_T87;struct Cyc_Absyn_Pat*p2=_T84;
# 785
Cyc_Binding_resolve_pat(env,p2);_T87=vd;goto _LLF;}case 4: _T44=r;{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T44;_T87=_T88->f2;}_LLF: {struct Cyc_Absyn_Vardecl*vd=_T87;_T87=vd;goto _LL11;}case 2: _T45=r;{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T45;_T87=_T88->f2;}_LL11: {struct Cyc_Absyn_Vardecl*vd=_T87;
# 789
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 5: _T46=r;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T46;_T87=_T88->f1;}{struct Cyc_Absyn_Pat*p2=_T87;
# 791
Cyc_Binding_resolve_pat(env,p2);return;}case 16: _T47=r;{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T47;_T87=_T88->f1;}{struct Cyc_Absyn_Exp*e=_T87;
Cyc_Binding_resolve_exp(env,e);return;}case 8: goto _LL19;case 9: _LL19: goto _LL1B;case 10: _LL1B: goto _LL1D;case 11: _LL1D:
# 797
 return;case 6: _T48=r;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T48;_T49=_T88->f1;_T87=(void*)_T49;_T86=_T88->f4;}{void*topt=_T87;struct Cyc_List_List*dps=_T86;
# 800
if(topt==0)goto _TL13D;{
void*t=topt;
void*_T88=Cyc_Absyn_compress(t);void*_T89;_T4A=(int*)_T88;_T4B=*_T4A;if(_T4B!=0)goto _TL13F;_T4C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T88;_T4D=_T4C->f1;_T4E=(int*)_T4D;_T4F=*_T4E;if(_T4F!=24)goto _TL141;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T88;_T50=_T8A->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T8B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T50;_T51=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T88;_T52=_T51->f1;_T53=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T52;_T54=& _T53->f1;_T89=(union Cyc_Absyn_AggrInfo*)_T54;}}{union Cyc_Absyn_AggrInfo*info=(union Cyc_Absyn_AggrInfo*)_T89;_T55=info;{
# 804
union Cyc_Absyn_AggrInfo _T8A=*_T55;struct _tuple0*_T8B;enum Cyc_Absyn_AggrKind _T8C;_T56=_T8A.UnknownAggr;_T57=_T56.tag;if(_T57!=1)goto _TL143;_T58=_T8A.UnknownAggr;_T59=_T58.val;_T8C=_T59.f0;_T5A=_T8A.UnknownAggr;_T5B=_T5A.val;_T8B=_T5B.f1;{enum Cyc_Absyn_AggrKind ak=_T8C;struct _tuple0*n=_T8B;{struct _handler_cons _T8D;_T5C=& _T8D;_push_handler(_T5C);{int _T8E=0;_T5D=_T8D.handler;_T5E=setjmp(_T5D);if(!_T5E)goto _TL145;_T8E=1;goto _TL146;_TL145: _TL146: if(_T8E)goto _TL147;else{goto _TL149;}_TL149:{
# 807
struct Cyc_Absyn_Aggrdecl*(*_T8F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T5F=_T8F;}_T60=loc;_T61=env;_T62=_T61->ns;_T63=n;{struct Cyc_Absyn_Aggrdecl*ad=_T5F(_T60,_T62,_T63,Cyc_Binding_lookup_aggrdecl);_T64=info;{struct Cyc_Absyn_Aggrdecl**_T8F=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_T8F=ad;_T65=(struct Cyc_Absyn_Aggrdecl**)_T8F;}*_T64=Cyc_Absyn_KnownAggr(_T65);}_pop_handler();goto _TL148;_TL147: _T66=Cyc_Core_get_exn_thrown();{void*_T8F=(void*)_T66;void*_T90;_T67=(struct Cyc_Binding_BindingError_exn_struct*)_T8F;_T68=_T67->tag;_T69=Cyc_Binding_BindingError;if(_T68!=_T69)goto _TL14A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T91;_T91.tag=1;
# 811
_T91.f1=n;_T6A=_T91;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T91=_T6A;{struct Cyc_Warn_String_Warn_Warg_struct _T92;_T92.tag=0;_T92.f1=_tag_fat(" is an undefined aggregate",sizeof(char),27U);_T6B=_T92;}{struct Cyc_Warn_String_Warn_Warg_struct _T92=_T6B;void*_T93[2];_T6C=_T93 + 0;*_T6C=& _T91;_T6D=_T93 + 1;*_T6D=& _T92;_T6E=p;_T6F=_T6E->loc;_T70=_tag_fat(_T93,sizeof(void*),2);Cyc_Warn_err2(_T6F,_T70);}}goto _LL50;_TL14A: _T90=_T8F;{void*exn=_T90;_rethrow(exn);}_LL50:;}_TL148:;}}goto _LL4B;}_TL143: goto _LL4B;_LL4B:;}goto _LL46;}_TL141: goto _LL49;_TL13F: _LL49: _T71=p;_T72=_T71->loc;_T73=env;_T74=topt;
# 818
Cyc_Binding_resolve_type(_T72,_T73,_T74);goto _LL46;_LL46:;}goto _TL13E;_TL13D: _TL13E:
# 821
 _TL14F: if(dps!=0)goto _TL14D;else{goto _TL14E;}
_TL14D: _T75=env;_T76=dps;_T77=_T76->hd;_T78=(struct _tuple12*)_T77;_T79=*_T78;_T7A=_T79.f1;Cyc_Binding_resolve_pat(_T75,_T7A);_T7B=dps;
# 821
dps=_T7B->tl;goto _TL14F;_TL14E:
# 823
 return;}case 7: _T7C=r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T88=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T7C;_T86=_T88->f3;}{struct Cyc_List_List*ps=_T86;
# 825
_TL153: if(ps!=0)goto _TL151;else{goto _TL152;}
_TL151: _T7D=env;_T7E=ps;_T7F=_T7E->hd;_T80=(struct Cyc_Absyn_Pat*)_T7F;Cyc_Binding_resolve_pat(_T7D,_T80);_T81=ps;
# 825
ps=_T81->tl;goto _TL153;_TL152:
# 827
 return;}case 12: goto _LL25;default: _LL25:
# 829
 return;}_LL3:;}}struct _tuple13{void*f0;void*f1;};
# 833
static void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){unsigned _T0;struct Cyc_Binding_Env*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple13*_T4;struct _tuple13 _T5;void*_T6;unsigned _T7;struct Cyc_Binding_Env*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple13*_TB;struct _tuple13 _TC;void*_TD;struct Cyc_List_List*_TE;
_TL157: if(po!=0)goto _TL155;else{goto _TL156;}
_TL155: _T0=loc;_T1=env;_T2=po;_T3=_T2->hd;_T4=(struct _tuple13*)_T3;_T5=*_T4;_T6=_T5.f0;Cyc_Binding_resolve_type(_T0,_T1,_T6);_T7=loc;_T8=env;_T9=po;_TA=_T9->hd;_TB=(struct _tuple13*)_TA;_TC=*_TB;_TD=_TC.f1;
Cyc_Binding_resolve_type(_T7,_T8,_TD);_TE=po;
# 834
po=_TE->tl;goto _TL157;_TL156:;}
# 840
static void Cyc_Binding_resolve_effconstr(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*effc){struct Cyc_List_List*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;struct Cyc_List_List*_T8;
_TL15B: if(effc!=0)goto _TL159;else{goto _TL15A;}
_TL159: _T0=effc;{void*_T9=_T0->hd;void*_TA;void*_TB;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T9;_T3=_TC->f1;_TB=(void*)_T3;}{void*e=_TB;
Cyc_Binding_resolve_type(loc,env,e);goto _LL0;}case 1:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T9;_T4=_TC->f1;_TB=(void*)_T4;_T5=_TC->f2;_TA=(void*)_T5;}{void*e1=_TB;void*e2=_TA;_TB=e1;_TA=e2;goto _LL6;}default:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T9;_T6=_TC->f1;_TB=(void*)_T6;_T7=_TC->f2;_TA=(void*)_T7;}_LL6: {void*e1=_TB;void*e2=_TA;
# 846
Cyc_Binding_resolve_type(loc,env,e1);
Cyc_Binding_resolve_type(loc,env,e2);goto _LL0;}}_LL0:;}_T8=effc;
# 841
effc=_T8->tl;goto _TL15B;_TL15A:;}
# 852
static void Cyc_Binding_resolve_qualbnd(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*qb){unsigned _T0;struct Cyc_Binding_Env*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple13*_T4;struct _tuple13 _T5;void*_T6;unsigned _T7;struct Cyc_Binding_Env*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple13*_TB;struct _tuple13 _TC;void*_TD;struct Cyc_List_List*_TE;
_TL160: if(qb!=0)goto _TL15E;else{goto _TL15F;}
_TL15E: _T0=loc;_T1=env;_T2=qb;_T3=_T2->hd;_T4=(struct _tuple13*)_T3;_T5=*_T4;_T6=_T5.f0;Cyc_Binding_resolve_type(_T0,_T1,_T6);_T7=loc;_T8=env;_T9=qb;_TA=_T9->hd;_TB=(struct _tuple13*)_TA;_TC=*_TB;_TD=_TC.f1;
Cyc_Binding_resolve_type(_T7,_T8,_TD);_TE=qb;
# 853
qb=_TE->tl;goto _TL160;_TL15F:;}struct _tuple14{struct Cyc_List_List**f0;struct Cyc_Dict_Dict*f1;struct Cyc_Binding_Env*f2;struct _tuple10*f3;};struct _tuple15{unsigned f0;struct _tuple0*f1;int f2;};
# 861
static void Cyc_Binding_export_all_symbols(struct _tuple14*cenv,struct _fat_ptr*name,void*res){struct _tuple14*_T0;struct _tuple10*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;struct _tuple0*_T7;struct _tuple0*_T8;int(*_T9)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*);int(*_TA)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_TB)(struct _tuple0*,struct _tuple0*);struct Cyc_List_List*_TC;struct _tuple0*_TD;int _TE;struct _tuple15*_TF;struct _tuple15*_T10;struct _tuple15*_T11;struct Cyc_List_List**_T12;struct Cyc_List_List*_T13;struct Cyc_List_List**_T14;struct Cyc_Dict_Dict*_T15;struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T17)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T18;struct Cyc_Dict_Dict _T19;struct _fat_ptr*_T1A;void*_T1B;void*_T1C;struct Cyc_Dict_Dict*_T1D;struct Cyc_Dict_Dict(*_T1E)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T1F)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T20;struct Cyc_Dict_Dict _T21;struct _fat_ptr*_T22;void*_T23;void*_T24;struct Cyc_Dict_Dict*_T25;struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T28;struct Cyc_Dict_Dict _T29;struct _fat_ptr*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_List_List*_T32;unsigned _T33;struct Cyc_Binding_Env*_T34;struct Cyc_Dict_Dict*_T35;struct Cyc_List_List**_T36;_T0=cenv;{struct _tuple14 _T37=*_T0;_T36=_T37.f0;_T35=_T37.f1;_T34=_T37.f2;_T1=_T37.f3;{struct _tuple10 _T38=*_T1;_T33=_T38.f0;_T32=_T38.f1;}}{struct Cyc_List_List**exlist_ptr=_T36;struct Cyc_Dict_Dict*out_dict=_T35;struct Cyc_Binding_Env*env=_T34;unsigned wcloc=_T33;struct Cyc_List_List*hidelist=_T32;struct Cyc_Absyn_Datatypefield*_T37;struct Cyc_Absyn_Datatypedecl*_T38;struct Cyc_Absyn_Enumfield*_T39;void*_T3A;_T2=res;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 0: _T5=res;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T5;_T6=_T3B->f1;_T3A=(void*)_T6;}{void*bnd=_T3A;
# 866
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T7=qv;_T7->f0=Cyc_Absyn_Rel_n(0);_T8=qv;_T8->f1=name;_TA=Cyc_List_mem;{
int(*_T3B)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*)=(int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))_TA;_T9=_T3B;}_TB=Cyc_Absyn_qvar_cmp;_TC=hidelist;_TD=qv;_TE=_T9(_TB,_TC,_TD);if(!_TE)goto _TL162;
# 869
return;_TL162:
# 871
 Cyc_Binding_absolutize_decl(wcloc,env,qv,4U);{
struct _tuple15*ex_sym;ex_sym=_cycalloc(sizeof(struct _tuple15));_TF=ex_sym;_TF->f0=wcloc;_T10=ex_sym;_T10->f1=qv;_T11=ex_sym;_T11->f2=0;_T12=exlist_ptr;{struct Cyc_List_List*_T3B=_cycalloc(sizeof(struct Cyc_List_List));
_T3B->hd=ex_sym;_T14=exlist_ptr;_T3B->tl=*_T14;_T13=(struct Cyc_List_List*)_T3B;}*_T12=_T13;_T15=out_dict;_T17=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T17;_T16=_T3B;}_T18=out_dict;_T19=*_T18;_T1A=name;_T1B=res;*_T15=_T16(_T19,_T1A,_T1B);goto _LL3;}}case 3: _T1C=res;{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T1C;_T3A=_T3B->f1;_T39=_T3B->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T3A;struct Cyc_Absyn_Enumfield*ef=_T39;_T1D=out_dict;_T1F=Cyc_Dict_insert;{
# 877
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T1F;_T1E=_T3B;}_T20=out_dict;_T21=*_T20;_T22=name;_T23=res;*_T1D=_T1E(_T21,_T22,_T23);goto _LL3;}case 1: _T24=res;{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T24;_T3A=_T3B->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T3A;_T25=out_dict;_T27=Cyc_Dict_insert;{
# 881
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T27;_T26=_T3B;}_T28=out_dict;_T29=*_T28;_T2A=name;_T2B=res;*_T25=_T26(_T29,_T2A,_T2B);goto _LL3;}case 4: _T2C=res;{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T2C;_T2D=_T3B->f1;_T3A=(void*)_T2D;_T39=_T3B->f2;}{void*t=_T3A;struct Cyc_Absyn_Enumfield*ef=_T39;goto _LL3;}default: _T2E=res;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T2E;_T38=_T3B->f1;_T37=_T3B->f2;}{struct Cyc_Absyn_Datatypedecl*dd=_T38;struct Cyc_Absyn_Datatypefield*df=_T37;_T2F=wcloc;_T30=
# 886
_tag_fat("Unexpected binding from extern C Include",sizeof(char),41U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2F,_T30,_T31);goto _LL3;}}_LL3:;}}struct _tuple16{struct Cyc_Absyn_Tqual f0;void*f1;};
# 899 "binding.cyc"
static void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;struct Cyc_Absyn_Decl*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Vardecl*_T4;struct Cyc_Absyn_Exp*_T5;int _T6;struct Cyc_Binding_Env*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Binding_Env*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;void**_TD;unsigned _TE;struct _fat_ptr _TF;unsigned _T10;struct Cyc_Binding_Env*_T11;struct Cyc_Absyn_Fndecl*_T12;struct _tuple0*_T13;struct Cyc_Absyn_Fndecl*_T14;enum Cyc_Absyn_Scope _T15;struct Cyc_Absyn_Fndecl*_T16;struct Cyc_Absyn_Fndecl*_T17;struct Cyc_Absyn_FnInfo _T18;unsigned _T19;struct Cyc_Binding_Env*_T1A;struct Cyc_Absyn_Fndecl*_T1B;struct Cyc_Absyn_FnInfo _T1C;struct Cyc_List_List*_T1D;struct Cyc_Absyn_Fndecl*_T1E;struct Cyc_Absyn_FnInfo _T1F;struct Cyc_Absyn_VarargInfo*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Core_Opt*_T29;struct Cyc_Absyn_Fndecl*_T2A;int _T2B;unsigned _T2C;struct Cyc_Binding_Env*_T2D;struct Cyc_Absyn_Fndecl*_T2E;struct _tuple0*_T2F;struct Cyc_Binding_Env*_T30;struct Cyc_Binding_Env*_T31;struct Cyc_Dict_Dict*_T32;struct Cyc_Binding_Env*_T33;struct Cyc_Dict_Dict*_T34;struct Cyc_Binding_Env*_T35;struct Cyc_Dict_Dict*_T36;struct Cyc_Dict_Dict(*_T37)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T38)(int(*)(void*,void*));int(*_T39)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T3A;void*_T3B;struct Cyc_Absyn_Vardecl*_T3C;struct Cyc_Binding_Env*_T3D;struct Cyc_Dict_Dict*_T3E;struct Cyc_Dict_Dict*_T3F;struct Cyc_Dict_Dict(*_T40)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T41)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T42;struct Cyc_Dict_Dict*_T43;struct Cyc_Dict_Dict*_T44;struct Cyc_Dict_Dict _T45;struct Cyc_List_List*_T46;void*_T47;struct Cyc_Absyn_Vardecl*_T48;struct _tuple0*_T49;struct _fat_ptr*_T4A;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T4B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T4C;struct Cyc_List_List*_T4D;void*_T4E;void*_T4F;struct Cyc_List_List*_T50;unsigned _T51;struct Cyc_Binding_Env*_T52;struct Cyc_Absyn_Fndecl*_T53;struct Cyc_Absyn_FnInfo*_T54;struct Cyc_Absyn_FnInfo*_T55;struct Cyc_Dict_Dict*_T56;struct Cyc_Dict_Dict(*_T57)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T58)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T59;struct Cyc_Dict_Dict _T5A;struct _fat_ptr*_T5B;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T5C;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T5D;void*_T5E;struct Cyc_Binding_Env*_T5F;struct Cyc_Dict_Dict*_T60;struct Cyc_Dict_Dict*_T61;struct Cyc_Dict_Dict(*_T62)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T63)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T64;struct Cyc_Dict_Dict*_T65;struct Cyc_Dict_Dict*_T66;struct Cyc_Dict_Dict _T67;struct _fat_ptr*_T68;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T69;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T6A;void*_T6B;struct Cyc_Binding_ResolveNSEnv*(*_T6C)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T6D;struct Cyc_Binding_NSCtxt*_T6E;union Cyc_Absyn_Nmspace _T6F;struct Cyc_Binding_ResolveNSEnv*_T70;struct Cyc_Dict_Dict(*_T71)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T72)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T73;struct Cyc_Dict_Dict _T74;struct _fat_ptr*_T75;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T76;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T77;void*_T78;int _T79;struct Cyc_Binding_Env*_T7A;struct Cyc_Absyn_Fndecl*_T7B;struct Cyc_Absyn_Stmt*_T7C;struct Cyc_Binding_Env*_T7D;int _T7E;struct Cyc_Warn_String_Warn_Warg_struct _T7F;void**_T80;unsigned _T81;struct _fat_ptr _T82;unsigned _T83;struct Cyc_Binding_Env*_T84;struct Cyc_List_List*_T85;void*_T86;struct Cyc_Absyn_Vardecl*_T87;struct Cyc_List_List*_T88;int _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;void**_T8B;unsigned _T8C;struct _fat_ptr _T8D;unsigned _T8E;struct Cyc_Binding_Env*_T8F;struct Cyc_Absyn_Typedefdecl*_T90;struct _tuple0*_T91;enum Cyc_Absyn_Scope _T92;struct Cyc_Absyn_Typedefdecl*_T93;int _T94;struct Cyc_Absyn_Typedefdecl*_T95;struct Cyc_Binding_ResolveNSEnv*(*_T96)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T97;struct Cyc_Binding_NSCtxt*_T98;union Cyc_Absyn_Nmspace _T99;struct Cyc_Absyn_Typedefdecl*_T9A;void*_T9B;unsigned _T9C;struct Cyc_Binding_Env*_T9D;struct Cyc_Absyn_Typedefdecl*_T9E;void*_T9F;struct Cyc_Binding_Env*_TA0;int _TA1;struct Cyc_Binding_ResolveNSEnv*_TA2;struct Cyc_Dict_Dict(*_TA3)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_TA4)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TA5;struct Cyc_Dict_Dict _TA6;struct _fat_ptr*_TA7;struct Cyc_Absyn_Typedefdecl*_TA8;int _TA9;struct Cyc_Warn_String_Warn_Warg_struct _TAA;void**_TAB;unsigned _TAC;struct _fat_ptr _TAD;unsigned _TAE;struct Cyc_Binding_Env*_TAF;struct Cyc_Absyn_Aggrdecl*_TB0;struct _tuple0*_TB1;struct Cyc_Absyn_Aggrdecl*_TB2;enum Cyc_Absyn_Scope _TB3;struct Cyc_Absyn_Aggrdecl*_TB4;struct Cyc_Binding_ResolveNSEnv*(*_TB5)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TB6;struct Cyc_Binding_NSCtxt*_TB7;union Cyc_Absyn_Nmspace _TB8;int(*_TB9)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TBA)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TBB;struct Cyc_Dict_Dict _TBC;struct _fat_ptr*_TBD;int _TBE;struct Cyc_Absyn_Aggrdecl*_TBF;struct Cyc_Absyn_AggrdeclImpl*_TC0;struct Cyc_Binding_Env*_TC1;int _TC2;struct Cyc_Binding_ResolveNSEnv*_TC3;struct Cyc_Dict_Dict(*_TC4)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_TC5)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TC6;struct Cyc_Dict_Dict _TC7;struct _fat_ptr*_TC8;struct Cyc_Absyn_Aggrdecl*_TC9;struct Cyc_Binding_ResolveNSEnv*_TCA;struct Cyc_Dict_Dict(*_TCB)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TCC)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TCD;struct Cyc_Dict_Dict _TCE;struct _fat_ptr*_TCF;struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TD0;void*_TD1;struct Cyc_Absyn_Aggrdecl*_TD2;struct Cyc_Absyn_AggrdeclImpl*_TD3;struct Cyc_Absyn_Aggrdecl*_TD4;int _TD5;struct Cyc_Warn_String_Warn_Warg_struct _TD6;void**_TD7;unsigned _TD8;struct _fat_ptr _TD9;unsigned _TDA;struct Cyc_Binding_Env*_TDB;struct Cyc_Absyn_Enumdecl*_TDC;struct _tuple0*_TDD;struct Cyc_Absyn_Enumdecl*_TDE;enum Cyc_Absyn_Scope _TDF;struct Cyc_Absyn_Enumdecl*_TE0;struct Cyc_Binding_ResolveNSEnv*(*_TE1)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TE2;struct Cyc_Binding_NSCtxt*_TE3;union Cyc_Absyn_Nmspace _TE4;int(*_TE5)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TE6)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TE7;struct Cyc_Dict_Dict _TE8;struct _fat_ptr*_TE9;int _TEA;struct Cyc_Absyn_Enumdecl*_TEB;struct Cyc_Core_Opt*_TEC;struct Cyc_Binding_Env*_TED;int _TEE;struct Cyc_Binding_ResolveNSEnv*_TEF;struct Cyc_Dict_Dict(*_TF0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_TF1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TF2;struct Cyc_Dict_Dict _TF3;struct _fat_ptr*_TF4;struct Cyc_Absyn_Enumdecl*_TF5;struct Cyc_Absyn_Enumdecl*_TF6;struct Cyc_Core_Opt*_TF7;struct Cyc_Absyn_Enumdecl*_TF8;struct Cyc_Core_Opt*_TF9;void*_TFA;struct Cyc_List_List*_TFB;void*_TFC;struct Cyc_Absyn_Enumfield*_TFD;unsigned _TFE;struct Cyc_Binding_Env*_TFF;struct Cyc_Absyn_Enumfield*_T100;struct _tuple0*_T101;struct Cyc_Absyn_Enumdecl*_T102;enum Cyc_Absyn_Scope _T103;struct Cyc_Absyn_Enumfield*_T104;struct Cyc_Absyn_Exp*_T105;struct Cyc_Binding_Env*_T106;struct Cyc_Absyn_Enumfield*_T107;struct Cyc_Absyn_Exp*_T108;struct Cyc_Binding_Env*_T109;int _T10A;struct Cyc_Binding_ResolveNSEnv*_T10B;struct Cyc_Dict_Dict(*_T10C)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T10D)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T10E;struct Cyc_Dict_Dict _T10F;struct Cyc_Absyn_Enumfield*_T110;struct _tuple0*_T111;struct _tuple0 _T112;struct _fat_ptr*_T113;struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T114;void*_T115;struct Cyc_List_List*_T116;struct _handler_cons*_T117;int*_T118;int _T119;struct Cyc_List_List*(*_T11A)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T11B;struct Cyc_Binding_Env*_T11C;struct Cyc_Binding_NSCtxt*_T11D;struct Cyc_Absyn_Datatypedecl*_T11E;struct _tuple0*_T11F;struct Cyc_List_List*_T120;void*_T121;struct Cyc_Absyn_Datatypedecl*_T122;int _T123;struct Cyc_Binding_BindingError_exn_struct*_T124;void*_T125;struct Cyc_Absyn_Datatypedecl*_T126;struct Cyc_Absyn_Datatypedecl*_T127;struct Cyc_Absyn_Datatypedecl*_T128;struct Cyc_Absyn_Datatypedecl*_T129;struct Cyc_Binding_ResolveNSEnv*(*_T12A)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T12B;struct Cyc_Binding_NSCtxt*_T12C;union Cyc_Absyn_Nmspace _T12D;struct Cyc_Binding_ResolveNSEnv*_T12E;struct Cyc_Dict_Dict(*_T12F)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T130)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T131;struct Cyc_Dict_Dict _T132;struct _fat_ptr*_T133;struct Cyc_List_List*_T134;void*_T135;struct Cyc_Binding_BindingError_exn_struct*_T136;char*_T137;char*_T138;unsigned _T139;struct Cyc_Binding_Env*_T13A;struct Cyc_Absyn_Datatypedecl*_T13B;struct _tuple0*_T13C;struct Cyc_Absyn_Datatypedecl*_T13D;enum Cyc_Absyn_Scope _T13E;struct Cyc_Absyn_Datatypedecl*_T13F;struct Cyc_Binding_ResolveNSEnv*(*_T140)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T141;struct Cyc_Binding_NSCtxt*_T142;union Cyc_Absyn_Nmspace _T143;int(*_T144)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T145)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T146;struct Cyc_Dict_Dict _T147;struct _fat_ptr*_T148;int _T149;struct Cyc_Absyn_Datatypedecl*_T14A;struct Cyc_Core_Opt*_T14B;struct Cyc_Binding_ResolveNSEnv*_T14C;struct Cyc_Dict_Dict(*_T14D)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T14E)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T14F;struct Cyc_Dict_Dict _T150;struct _fat_ptr*_T151;struct Cyc_List_List*_T152;struct Cyc_Absyn_Datatypedecl*_T153;struct Cyc_Absyn_Datatypedecl*_T154;struct Cyc_Core_Opt*_T155;struct Cyc_Absyn_Datatypedecl*_T156;struct Cyc_Core_Opt*_T157;void*_T158;struct Cyc_List_List*_T159;void*_T15A;struct Cyc_Absyn_Datatypefield*_T15B;unsigned _T15C;struct Cyc_Binding_Env*_T15D;struct Cyc_List_List*_T15E;void*_T15F;struct _tuple16*_T160;struct _tuple16 _T161;void*_T162;struct Cyc_List_List*_T163;struct Cyc_Absyn_Datatypefield*_T164;struct _tuple0*_T165;struct _tuple0 _T166;struct _union_Nmspace_Abs_n _T167;unsigned _T168;struct _union_Nmspace_Rel_n _T169;struct Cyc_List_List*_T16A;struct Cyc_Absyn_Datatypedecl*_T16B;int _T16C;struct Cyc_Absyn_Datatypefield*_T16D;struct _tuple0*_T16E;struct Cyc_Binding_Env*_T16F;struct Cyc_Binding_NSCtxt*_T170;struct Cyc_List_List*_T171;struct Cyc_Absyn_Datatypefield*_T172;struct _tuple0*_T173;struct Cyc_Absyn_Datatypedecl*_T174;struct _tuple0*_T175;struct _tuple0 _T176;struct Cyc_Warn_String_Warn_Warg_struct _T177;void**_T178;struct Cyc_Absyn_Datatypefield*_T179;unsigned _T17A;struct _fat_ptr _T17B;int(*_T17C)(struct _fat_ptr,struct _fat_ptr);void*(*_T17D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T17E;struct _fat_ptr _T17F;struct Cyc_Binding_ResolveNSEnv*(*_T180)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T181;struct Cyc_Binding_NSCtxt*_T182;struct Cyc_Absyn_Datatypefield*_T183;struct _tuple0*_T184;struct _tuple0 _T185;union Cyc_Absyn_Nmspace _T186;struct Cyc_Binding_ResolveNSEnv*_T187;struct Cyc_Dict_Dict(*_T188)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T189)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T18A;struct Cyc_Dict_Dict _T18B;struct Cyc_Absyn_Datatypefield*_T18C;struct _tuple0*_T18D;struct _tuple0 _T18E;struct _fat_ptr*_T18F;struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T190;void*_T191;struct Cyc_List_List*_T192;void(*_T193)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T194;struct Cyc_Binding_NSCtxt*_T195;struct _fat_ptr*_T196;struct Cyc_Binding_Env*_T197;struct Cyc_Binding_NSCtxt*_T198;struct Cyc_Absyn_Decl*_T199;unsigned _T19A;struct Cyc_Binding_Env*_T19B;struct Cyc_Binding_NSCtxt*_T19C;struct _tuple0*_T19D;struct Cyc_Binding_Env*_T19E;struct Cyc_Binding_NSCtxt*_T19F;struct Cyc_Binding_Env*_T1A0;struct Cyc_Binding_Env*_T1A1;struct Cyc_Binding_Env*_T1A2;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1A3;struct Cyc_List_List**_T1A4;struct Cyc_List_List**_T1A5;struct Cyc_List_List*_T1A6;void*_T1A7;struct Cyc_List_List*_T1A8;int _T1A9;struct Cyc_Warn_String_Warn_Warg_struct _T1AA;void**_T1AB;unsigned _T1AC;struct _fat_ptr _T1AD;struct Cyc_Binding_ResolveNSEnv*(*_T1AE)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1AF)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T1B0;struct Cyc_Binding_NSCtxt*_T1B1;struct Cyc_Dict_Dict _T1B2;struct Cyc_Binding_Env*_T1B3;struct Cyc_Binding_NSCtxt*_T1B4;struct Cyc_List_List*_T1B5;struct Cyc_Binding_ResolveNSEnv*_T1B6;struct Cyc_Binding_Env*_T1B7;struct Cyc_Binding_Env*_T1B8;struct Cyc_Binding_Env*_T1B9;struct Cyc_Binding_Env*_T1BA;struct Cyc_Binding_ResolveNSEnv*_T1BB;struct _tuple10*_T1BC;struct _tuple10 _T1BD;unsigned _T1BE;struct Cyc_List_List**_T1BF;struct Cyc_List_List*_T1C0;unsigned _T1C1;struct _tuple10*_T1C2;struct _tuple10 _T1C3;unsigned _T1C4;struct _fat_ptr _T1C5;struct _fat_ptr _T1C6;struct Cyc_Binding_Env*_T1C7;void(*_T1C8)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict);void(*_T1C9)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple14*_T1CA;struct Cyc_Dict_Dict*_T1CB;struct Cyc_Dict_Dict _T1CC;struct Cyc_List_List**_T1CD;struct Cyc_List_List*_T1CE;void*_T1CF;struct _tuple0*_T1D0;int(*_T1D1)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1D2)(struct Cyc_Dict_Dict,void*);int _T1D3;int(*_T1D4)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1D5)(struct Cyc_Dict_Dict,void*);int _T1D6;void*(*_T1D7)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1D8)(struct Cyc_Dict_Dict,void*);void*_T1D9;void*(*_T1DA)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1DB)(struct Cyc_Dict_Dict,void*);void*_T1DC;struct Cyc_String_pa_PrintArg_struct _T1DD;struct _fat_ptr*_T1DE;void**_T1DF;unsigned _T1E0;struct _fat_ptr _T1E1;struct _fat_ptr _T1E2;struct Cyc_Dict_Dict(*_T1E3)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T1E4)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict _T1E5;struct _fat_ptr*_T1E6;void*(*_T1E7)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1E8)(struct Cyc_Dict_Dict,void*);void*_T1E9;struct Cyc_List_List*_T1EA;struct Cyc_Binding_ResolveNSEnv*_T1EB;struct Cyc_Binding_Env*_T1EC;_T0=d;{
# 902
unsigned loc=_T0->loc;_T1=d;{
void*_T1ED=_T1->r;struct _tuple10*_T1EE;struct Cyc_List_List*_T1EF;struct Cyc_List_List*_T1F0;struct Cyc_Absyn_Vardecl*_T1F1;struct Cyc_Absyn_Exp*_T1F2;void*_T1F3;_T2=(int*)_T1ED;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1F3;
# 906
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T4=vd;_T5=_T4->initializer;
# 911
if(_T5==0)goto _TL165;_T6=Cyc_Binding_in_cinclude(env);if(_T6)goto _TL165;else{goto _TL167;}
_TL167: _T7=env;_T8=vd;_T9=_T8->initializer;Cyc_Binding_resolve_exp(_T7,_T9);goto _TL166;_TL165: _TL166: goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Fndecl*fd=_T1F3;_TA=env;_TB=_TA->in_cyc_override;
# 916
if(!_TB)goto _TL168;{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;
_T1F4.f1=_tag_fat("Function declaration not permitted within cyclone_override block",sizeof(char),65U);_TC=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_TC;void*_T1F5[1];_TD=_T1F5 + 0;*_TD=& _T1F4;_TE=loc;_TF=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_TE,_TF);}goto _TL169;_TL168: _TL169: _T10=loc;_T11=env;_T12=fd;_T13=_T12->name;_T14=fd;_T15=_T14->sc;
Cyc_Binding_absolutize_decl(_T10,_T11,_T13,_T15);_T16=fd;{
struct _tuple0*_T1F4=_T16->name;struct _fat_ptr*_T1F5;union Cyc_Absyn_Nmspace _T1F6;{struct _tuple0 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F6;struct _fat_ptr*decl_name=_T1F5;_T17=fd;_T18=_T17->i;{
struct Cyc_List_List*argit=_T18.args;_T19=loc;_T1A=env;_T1B=fd;_T1C=_T1B->i;_T1D=_T1C.args;_T1E=fd;_T1F=_T1E->i;_T20=_T1F.cyc_varargs;{
struct Cyc_List_List*vdsopts=Cyc_Binding_get_fun_vardecls(1,_T19,_T1A,_T1D,_T20);
struct Cyc_List_List*vds=0;{
struct Cyc_List_List*v=vdsopts;_TL16D: if(v!=0)goto _TL16B;else{goto _TL16C;}
_TL16B: _T21=v;_T22=_T21->hd;_T23=(struct Cyc_Absyn_Vardecl*)_T22;if(_T23==0)goto _TL16E;{struct Cyc_List_List*_T1F7=_cycalloc(sizeof(struct Cyc_List_List));_T25=v;_T26=_T25->hd;_T1F7->hd=(struct Cyc_Absyn_Vardecl*)_T26;_T1F7->tl=vds;_T24=(struct Cyc_List_List*)_T1F7;}vds=_T24;goto _TL16F;_TL16E: _TL16F: _T27=v;
# 923
v=_T27->tl;goto _TL16D;_TL16C:;}
# 925
vds=Cyc_List_imp_rev(vds);_T28=fd;{struct Cyc_Core_Opt*_T1F7=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T1F7->v=vds;_T29=(struct Cyc_Core_Opt*)_T1F7;}_T28->param_vardecls=_T29;_T2A=fd;
_T2A->i.arg_vardecls=vdsopts;_T2B=Cyc_Flags_warn_override;
if(!_T2B)goto _TL170;_T2C=loc;_T2D=env;_T2E=fd;_T2F=_T2E->name;
Cyc_Binding_check_warn_override(_T2C,_T2D,_T2F);goto _TL171;_TL170: _TL171: _T30=env;{
# 931
struct Cyc_Dict_Dict*old_locals=_T30->local_vars;
if(old_locals==0)goto _TL172;_T31=env;{struct Cyc_Dict_Dict*_T1F7=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T33=env;_T34=_T33->local_vars;
*_T1F7=*_T34;_T32=(struct Cyc_Dict_Dict*)_T1F7;}_T31->local_vars=_T32;goto _TL173;
# 935
_TL172: _T35=env;{struct Cyc_Dict_Dict*_T1F7=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T38=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T1F8)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T38;_T37=_T1F8;}_T39=Cyc_strptrcmp;*_T1F7=_T37(_T39);_T36=(struct Cyc_Dict_Dict*)_T1F7;}_T35->local_vars=_T36;_TL173:{
struct Cyc_List_List*vds1=vds;_TL177: if(vds1!=0)goto _TL175;else{goto _TL176;}
_TL175: _T3A=vds1;_T3B=_T3A->hd;_T3C=(struct Cyc_Absyn_Vardecl*)_T3B;if(_T3C==0)goto _TL178;_T3D=env;_T3E=_T3D->local_vars;_T3F=
_check_null(_T3E);_T41=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T41;_T40=_T1F7;}_T42=env;_T43=_T42->local_vars;_T44=_check_null(_T43);_T45=*_T44;_T46=vds1;_T47=_T46->hd;_T48=(struct Cyc_Absyn_Vardecl*)_T47;_T49=_T48->name;_T4A=_T49->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1F7->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T1F8=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T1F8->tag=3;_T4D=vds1;_T4E=_T4D->hd;
_T1F8->f1=(struct Cyc_Absyn_Vardecl*)_T4E;_T4C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T1F8;}_T1F7->f1=(void*)_T4C;_T4B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1F7;}_T4F=(void*)_T4B;
# 938
*_T3F=_T40(_T45,_T4A,_T4F);goto _TL179;_TL178: _TL179: _T50=vds1;
# 936
vds1=_T50->tl;goto _TL177;_TL176:;}_T51=loc;_T52=env;_T53=fd;_T54=& _T53->i;_T55=(struct Cyc_Absyn_FnInfo*)_T54;
# 941
Cyc_Binding_resolve_function_stuff(_T51,_T52,_T55);
# 943
if(old_locals==0)goto _TL17A;_T56=old_locals;_T58=Cyc_Dict_insert;{
# 945
struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T58;_T57=_T1F7;}_T59=old_locals;_T5A=*_T59;_T5B=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1F7->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F8=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F8->tag=2;
_T1F8->f1=fd;_T5D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F8;}_T1F7->f1=(void*)_T5D;_T5C=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1F7;}_T5E=(void*)_T5C;
# 945
*_T56=_T57(_T5A,_T5B,_T5E);_T5F=env;_T60=_T5F->local_vars;_T61=
# 947
_check_null(_T60);_T63=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T63;_T62=_T1F7;}_T64=env;_T65=_T64->local_vars;_T66=_check_null(_T65);_T67=*_T66;_T68=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1F7->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F8=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F8->tag=2;
_T1F8->f1=fd;_T6A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F8;}_T1F7->f1=(void*)_T6A;_T69=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1F7;}_T6B=(void*)_T69;
# 947
*_T61=_T62(_T67,_T68,_T6B);goto _TL17B;
# 950
_TL17A:{struct Cyc_Binding_ResolveNSEnv*(*_T1F7)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T6C=_T1F7;}_T6D=env;_T6E=_T6D->ns;_T6F=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T6C(_T6E,_T6F);_T70=decl_ns_data;_T72=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T72;_T71=_T1F7;}_T73=decl_ns_data;_T74=_T73->ordinaries;_T75=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1F7->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F8=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F8->tag=2;
# 953
_T1F8->f1=fd;_T77=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F8;}_T1F7->f1=(void*)_T77;_T76=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1F7;}_T78=(void*)_T76;
# 951
_T70->ordinaries=_T71(_T74,_T75,_T78);}_TL17B: _T79=
# 956
Cyc_Binding_in_cinclude(env);if(_T79)goto _TL17C;else{goto _TL17E;}
_TL17E: _T7A=env;_T7B=fd;_T7C=_T7B->body;Cyc_Binding_resolve_stmt(_T7A,_T7C);goto _TL17D;_TL17C: _TL17D: _T7D=env;
# 959
_T7D->local_vars=old_locals;goto _LL0;}}}}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;_T1F2=_T1F4->f3;}{struct Cyc_Absyn_Pat*p=_T1F3;struct Cyc_Absyn_Exp*exp=_T1F2;_T7E=
# 963
Cyc_Binding_at_toplevel(env);if(!_T7E)goto _TL17F;{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;
_T1F4.f1=_tag_fat("let not allowed at top-level",sizeof(char),29U);_T7F=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_T7F;void*_T1F5[1];_T80=_T1F5 + 0;*_T80=& _T1F4;_T81=loc;_T82=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_T81,_T82);}goto _LL0;_TL17F:
# 967
 Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);goto _LL0;}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_List_List*vds=_T1F3;
# 972
_TL184: if(vds!=0)goto _TL182;else{goto _TL183;}
_TL182: _T83=loc;_T84=env;_T85=vds;_T86=_T85->hd;_T87=(struct Cyc_Absyn_Vardecl*)_T86;Cyc_Binding_resolve_and_add_vardecl(_T83,_T84,_T87);_T88=vds;
# 972
vds=_T88->tl;goto _TL184;_TL183: goto _LL0;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;_T1F1=_T1F4->f2;_T1F2=_T1F4->f3;}{struct Cyc_Absyn_Tvar*tv=_T1F3;struct Cyc_Absyn_Vardecl*vd=_T1F1;struct Cyc_Absyn_Exp*open_exp_opt=_T1F2;
# 977
if(open_exp_opt==0)goto _TL185;
Cyc_Binding_resolve_exp(env,open_exp_opt);goto _TL186;_TL185: _TL186:
 Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T1F3;_T89=
# 983
Cyc_Binding_at_toplevel(env);if(_T89)goto _TL187;else{goto _TL189;}
_TL189:{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;_T1F4.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_T8A=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_T8A;void*_T1F5[1];_T8B=_T1F5 + 0;*_T8B=& _T1F4;_T8C=loc;_T8D=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_T8C,_T8D);}goto _LL0;_TL187: _T8E=loc;_T8F=env;_T90=td;_T91=_T90->name;_T93=td;_T94=_T93->extern_c;
# 987
if(!_T94)goto _TL18A;_T92=4U;goto _TL18B;_TL18A: _T92=2U;_TL18B: Cyc_Binding_absolutize_decl(_T8E,_T8F,_T91,_T92);_T95=td;{
struct _tuple0*_T1F4=_T95->name;struct _fat_ptr*_T1F5;union Cyc_Absyn_Nmspace _T1F6;{struct _tuple0 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F6;struct _fat_ptr*decl_name=_T1F5;{
# 990
struct Cyc_Binding_ResolveNSEnv*(*_T1F7)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T96=_T1F7;}_T97=env;_T98=_T97->ns;_T99=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T96(_T98,_T99);_T9A=td;_T9B=_T9A->defn;
if(_T9B==0)goto _TL18C;_T9C=loc;_T9D=env;_T9E=td;_T9F=_T9E->defn;
Cyc_Binding_resolve_type(_T9C,_T9D,_T9F);goto _TL18D;_TL18C: _TL18D: _TA0=env;_TA1=_TA0->in_cyc_override;
# 994
if(_TA1)goto _TL18E;else{goto _TL190;}
_TL190: _TA2=decl_ns_data;_TA4=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))_TA4;_TA3=_T1F7;}_TA5=decl_ns_data;_TA6=_TA5->typedefs;_TA7=decl_name;_TA8=td;_TA2->typedefs=_TA3(_TA6,_TA7,_TA8);goto _TL18F;_TL18E: _TL18F: goto _LL0;}}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T1F3;_TA9=
# 1001
Cyc_Binding_at_toplevel(env);if(_TA9)goto _TL191;else{goto _TL193;}
_TL193:{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;_T1F4.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TAA=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_TAA;void*_T1F5[1];_TAB=_T1F5 + 0;*_TAB=& _T1F4;_TAC=loc;_TAD=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_TAC,_TAD);}goto _LL0;_TL191: _TAE=loc;_TAF=env;_TB0=ad;_TB1=_TB0->name;_TB2=ad;_TB3=_TB2->sc;
# 1005
Cyc_Binding_absolutize_decl(_TAE,_TAF,_TB1,_TB3);_TB4=ad;{
struct _tuple0*_T1F4=_TB4->name;struct _fat_ptr*_T1F5;union Cyc_Absyn_Nmspace _T1F6;{struct _tuple0 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F6;struct _fat_ptr*decl_name=_T1F5;{
# 1008
struct Cyc_Binding_ResolveNSEnv*(*_T1F7)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TB5=_T1F7;}_TB6=env;_TB7=_TB6->ns;_TB8=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TB5(_TB7,_TB8);_TBA=Cyc_Dict_member;{
# 1010
int(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TBA;_TB9=_T1F7;}_TBB=decl_ns_data;_TBC=_TBB->aggrdecls;_TBD=decl_name;_TBE=_TB9(_TBC,_TBD);if(!_TBE)goto _TL194;_TBF=ad;_TC0=_TBF->impl;if(_TC0!=0)goto _TL194;goto _LL0;_TL194: _TC1=env;_TC2=_TC1->in_cyc_override;
# 1013
if(_TC2)goto _TL196;else{goto _TL198;}
_TL198: _TC3=decl_ns_data;_TC5=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))_TC5;_TC4=_T1F7;}_TC6=decl_ns_data;_TC7=_TC6->aggrdecls;_TC8=decl_name;_TC9=ad;_TC3->aggrdecls=_TC4(_TC7,_TC8,_TC9);_TCA=decl_ns_data;_TCC=Cyc_Dict_insert;{
# 1016
struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TCC;_TCB=_T1F7;}_TCD=decl_ns_data;_TCE=_TCD->ordinaries;_TCF=decl_name;{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_AggrRes_Binding_Resolved_struct));_T1F7->tag=1;
_T1F7->f1=ad;_TD0=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T1F7;}_TD1=(void*)_TD0;
# 1016
_TCA->ordinaries=_TCB(_TCE,_TCF,_TD1);goto _TL197;_TL196: _TL197: _TD2=ad;_TD3=_TD2->impl;
# 1019
if(_TD3==0)goto _TL199;_TD4=ad;{
struct Cyc_Absyn_AggrdeclImpl*_T1F7=_TD4->impl;struct Cyc_List_List*_T1F8;struct Cyc_List_List*_T1F9;struct Cyc_List_List*_T1FA;{struct Cyc_Absyn_AggrdeclImpl _T1FB=*_T1F7;_T1FA=_T1FB.qual_bnd;_T1F9=_T1FB.fields;_T1F8=_T1FB.effconstr;}{struct Cyc_List_List*qb=_T1FA;struct Cyc_List_List*fs=_T1F9;struct Cyc_List_List*fc=_T1F8;
Cyc_Binding_resolve_effconstr(loc,env,fc);
# 1023
Cyc_Binding_resolve_qualbnd(loc,env,qb);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}goto _TL19A;_TL199: _TL19A: goto _LL0;}}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T1F3;_TD5=
# 1029
Cyc_Binding_at_toplevel(env);if(_TD5)goto _TL19B;else{goto _TL19D;}
_TL19D:{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;_T1F4.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TD6=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_TD6;void*_T1F5[1];_TD7=_T1F5 + 0;*_TD7=& _T1F4;_TD8=loc;_TD9=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_TD8,_TD9);}goto _LL0;_TL19B: _TDA=loc;_TDB=env;_TDC=ed;_TDD=_TDC->name;_TDE=ed;_TDF=_TDE->sc;
# 1033
Cyc_Binding_absolutize_decl(_TDA,_TDB,_TDD,_TDF);_TE0=ed;{
struct _tuple0*_T1F4=_TE0->name;struct _fat_ptr*_T1F5;union Cyc_Absyn_Nmspace _T1F6;{struct _tuple0 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F6;struct _fat_ptr*decl_name=_T1F5;{
# 1036
struct Cyc_Binding_ResolveNSEnv*(*_T1F7)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TE1=_T1F7;}_TE2=env;_TE3=_TE2->ns;_TE4=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TE1(_TE3,_TE4);_TE6=Cyc_Dict_member;{
# 1038
int(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TE6;_TE5=_T1F7;}_TE7=decl_ns_data;_TE8=_TE7->enumdecls;_TE9=decl_name;_TEA=_TE5(_TE8,_TE9);if(!_TEA)goto _TL19E;_TEB=ed;_TEC=_TEB->fields;if(_TEC!=0)goto _TL19E;goto _LL0;_TL19E: _TED=env;_TEE=_TED->in_cyc_override;
# 1042
if(_TEE)goto _TL1A0;else{goto _TL1A2;}
_TL1A2: _TEF=decl_ns_data;_TF1=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))_TF1;_TF0=_T1F7;}_TF2=decl_ns_data;_TF3=_TF2->enumdecls;_TF4=decl_name;_TF5=ed;_TEF->enumdecls=_TF0(_TF3,_TF4,_TF5);goto _TL1A1;_TL1A0: _TL1A1: _TF6=ed;_TF7=_TF6->fields;
# 1045
if(_TF7==0)goto _TL1A3;_TF8=ed;_TF9=_TF8->fields;_TFA=_TF9->v;{
# 1047
struct Cyc_List_List*fs=(struct Cyc_List_List*)_TFA;_TL1A8: if(fs!=0)goto _TL1A6;else{goto _TL1A7;}
_TL1A6: _TFB=fs;_TFC=_TFB->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TFC;_TFD=f;_TFE=_TFD->loc;_TFF=env;_T100=f;_T101=_T100->name;_T102=ed;_T103=_T102->sc;
Cyc_Binding_absolutize_decl(_TFE,_TFF,_T101,_T103);_T104=f;_T105=_T104->tag;
if(_T105==0)goto _TL1A9;_T106=env;_T107=f;_T108=_T107->tag;
Cyc_Binding_resolve_exp(_T106,_T108);goto _TL1AA;_TL1A9: _TL1AA: _T109=env;_T10A=_T109->in_cyc_override;
if(_T10A)goto _TL1AB;else{goto _TL1AD;}
_TL1AD: _T10B=decl_ns_data;_T10D=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T10D;_T10C=_T1F7;}_T10E=decl_ns_data;_T10F=_T10E->ordinaries;_T110=f;_T111=_T110->name;_T112=*_T111;_T113=_T112.f1;{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_EnumRes_Binding_Resolved_struct));_T1F7->tag=3;
# 1055
_T1F7->f1=ed;_T1F7->f2=f;_T114=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T1F7;}_T115=(void*)_T114;
# 1053
_T10B->ordinaries=_T10C(_T10F,_T113,_T115);goto _TL1AC;_TL1AB: _TL1AC:;}_T116=fs;
# 1047
fs=_T116->tl;goto _TL1A8;_TL1A7:;}goto _TL1A4;_TL1A3: _TL1A4: goto _LL0;}}}}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T1F3;{struct _handler_cons _T1F4;_T117=& _T1F4;_push_handler(_T117);{int _T1F5=0;_T118=_T1F4.handler;_T119=setjmp(_T118);if(!_T119)goto _TL1AE;_T1F5=1;goto _TL1AF;_TL1AE: _TL1AF: if(_T1F5)goto _TL1B0;else{goto _TL1B2;}_TL1B2:{
# 1062
struct Cyc_List_List*(*_T1F6)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T11A=_T1F6;}_T11B=loc;_T11C=env;_T11D=_T11C->ns;_T11E=tud;_T11F=_T11E->name;{struct Cyc_List_List*decls=_T11A(_T11B,_T11D,_T11F,Cyc_Binding_lookup_datatypedecl);_T120=decls;_T121=_T120->hd;{
struct Cyc_Absyn_Datatypedecl*last_decl=(struct Cyc_Absyn_Datatypedecl*)_T121;_T122=last_decl;_T123=_T122->is_extensible;
if(_T123)goto _TL1B3;else{goto _TL1B5;}
_TL1B5:{struct Cyc_Binding_BindingError_exn_struct*_T1F6=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_T1F6->tag=Cyc_Binding_BindingError;_T124=(struct Cyc_Binding_BindingError_exn_struct*)_T1F6;}_T125=(void*)_T124;_throw(_T125);goto _TL1B4;_TL1B3: _TL1B4: _T126=tud;_T127=last_decl;
_T126->name=_T127->name;_T128=tud;
_T128->is_extensible=1;_T129=tud;{
struct _tuple0*_T1F6=_T129->name;struct _fat_ptr*_T1F7;union Cyc_Absyn_Nmspace _T1F8;{struct _tuple0 _T1F9=*_T1F6;_T1F8=_T1F9.f0;_T1F7=_T1F9.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F8;struct _fat_ptr*decl_name=_T1F7;{
struct Cyc_Binding_ResolveNSEnv*(*_T1F9)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T12A=_T1F9;}_T12B=env;_T12C=_T12B->ns;_T12D=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T12A(_T12C,_T12D);_T12E=decl_ns_data;_T130=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F9)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T130;_T12F=_T1F9;}_T131=decl_ns_data;_T132=_T131->datatypedecls;_T133=decl_name;{struct Cyc_List_List*_T1F9=_cycalloc(sizeof(struct Cyc_List_List));
_T1F9->hd=tud;_T1F9->tl=decls;_T134=(struct Cyc_List_List*)_T1F9;}
# 1070
_T12E->datatypedecls=_T12F(_T132,_T133,_T134);}}}}}_pop_handler();goto _TL1B1;_TL1B0: _T135=Cyc_Core_get_exn_thrown();{void*_T1F6=(void*)_T135;void*_T1F7;_T136=(struct Cyc_Binding_BindingError_exn_struct*)_T1F6;_T137=_T136->tag;_T138=Cyc_Binding_BindingError;if(_T137!=_T138)goto _TL1B6;_T139=loc;_T13A=env;_T13B=tud;_T13C=_T13B->name;_T13D=tud;_T13E=_T13D->sc;
# 1074
Cyc_Binding_absolutize_topdecl(_T139,_T13A,_T13C,_T13E);_T13F=tud;{
struct _tuple0*_T1F8=_T13F->name;struct _fat_ptr*_T1F9;union Cyc_Absyn_Nmspace _T1FA;{struct _tuple0 _T1FB=*_T1F8;_T1FA=_T1FB.f0;_T1F9=_T1FB.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1FA;struct _fat_ptr*decl_name=_T1F9;{
struct Cyc_Binding_ResolveNSEnv*(*_T1FB)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T140=_T1FB;}_T141=env;_T142=_T141->ns;_T143=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T140(_T142,_T143);_T145=Cyc_Dict_member;{
# 1078
int(*_T1FB)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T145;_T144=_T1FB;}_T146=decl_ns_data;_T147=_T146->datatypedecls;_T148=decl_name;_T149=_T144(_T147,_T148);if(!_T149)goto _TL1B8;_T14A=tud;_T14B=_T14A->fields;if(_T14B!=0)goto _TL1B8;goto _LL35;_TL1B8: _T14C=decl_ns_data;_T14E=Cyc_Dict_insert;{
# 1081
struct Cyc_Dict_Dict(*_T1FB)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T14E;_T14D=_T1FB;}_T14F=decl_ns_data;_T150=_T14F->datatypedecls;_T151=decl_name;{struct Cyc_List_List*_T1FB=_cycalloc(sizeof(struct Cyc_List_List));
_T1FB->hd=tud;_T1FB->tl=0;_T152=(struct Cyc_List_List*)_T1FB;}
# 1081
_T14C->datatypedecls=_T14D(_T150,_T151,_T152);goto _LL35;}}}_TL1B6: _T1F7=_T1F6;{void*exn=_T1F7;_rethrow(exn);}_LL35:;}_TL1B1:;}}_T153=tud;{
# 1086
struct _tuple0*_T1F4=_T153->name;struct _fat_ptr*_T1F5;union Cyc_Absyn_Nmspace _T1F6;{struct _tuple0 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F6;struct _fat_ptr*decl_name=_T1F5;_T154=tud;_T155=_T154->fields;
if(_T155==0)goto _TL1BA;_T156=tud;_T157=_T156->fields;_T158=_T157->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T158;_TL1BF: if(fs!=0)goto _TL1BD;else{goto _TL1BE;}
_TL1BD: _T159=fs;_T15A=_T159->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T15A;_T15B=f;{
struct Cyc_List_List*tqts=_T15B->typs;_TL1C3: if(tqts!=0)goto _TL1C1;else{goto _TL1C2;}
_TL1C1: _T15C=loc;_T15D=env;_T15E=tqts;_T15F=_T15E->hd;_T160=(struct _tuple16*)_T15F;_T161=*_T160;_T162=_T161.f1;Cyc_Binding_resolve_type(_T15C,_T15D,_T162);_T163=tqts;
# 1090
tqts=_T163->tl;goto _TL1C3;_TL1C2:;}_T164=f;_T165=_T164->name;_T166=*_T165;{
# 1092
union Cyc_Absyn_Nmspace _T1F7=_T166.f0;_T167=_T1F7.Abs_n;_T168=_T167.tag;switch(_T168){case 2: _T169=_T1F7.Rel_n;_T16A=_T169.val;if(_T16A!=0)goto _TL1C5;_T16B=tud;_T16C=_T16B->is_extensible;
# 1094
if(!_T16C)goto _TL1C7;_T16D=f;_T16E=_T16D->name;_T16F=env;_T170=_T16F->ns;_T171=_T170->curr_ns;
(*_T16E).f0=Cyc_Absyn_Abs_n(_T171,0);goto _TL1C8;
# 1097
_TL1C7: _T172=f;_T173=_T172->name;_T174=tud;_T175=_T174->name;_T176=*_T175;(*_T173).f0=_T176.f0;_TL1C8: goto _LL40;_TL1C5:{struct Cyc_Warn_String_Warn_Warg_struct _T1F8;_T1F8.tag=0;
# 1100
_T1F8.f1=_tag_fat("qualified datatype field declarations not allowed",sizeof(char),50U);_T177=_T1F8;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F8=_T177;void*_T1F9[1];_T178=_T1F9 + 0;*_T178=& _T1F8;_T179=f;_T17A=_T179->loc;_T17B=_tag_fat(_T1F9,sizeof(void*),1);Cyc_Warn_err2(_T17A,_T17B);}
return;case 1: goto _LL40;default: _T17D=Cyc_Warn_impos;{
# 1103
int(*_T1F8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T17D;_T17C=_T1F8;}_T17E=_tag_fat("datatype field Loc_n or C_n",sizeof(char),28U);_T17F=_tag_fat(0U,sizeof(void*),0);_T17C(_T17E,_T17F);}_LL40:;}{
# 1105
struct Cyc_Binding_ResolveNSEnv*(*_T1F7)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T180=_T1F7;}_T181=env;_T182=_T181->ns;_T183=f;_T184=_T183->name;_T185=*_T184;_T186=_T185.f0;{struct Cyc_Binding_ResolveNSEnv*ns_data=_T180(_T182,_T186);_T187=ns_data;_T189=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T189;_T188=_T1F7;}_T18A=ns_data;_T18B=_T18A->ordinaries;_T18C=f;_T18D=_T18C->name;_T18E=*_T18D;_T18F=_T18E.f1;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T1F7=_cycalloc(sizeof(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct));_T1F7->tag=2;
# 1108
_T1F7->f1=tud;_T1F7->f2=f;_T190=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T1F7;}_T191=(void*)_T190;
# 1106
_T187->ordinaries=_T188(_T18B,_T18F,_T191);}}_T192=fs;
# 1088
fs=_T192->tl;goto _TL1BF;_TL1BE:;}goto _TL1BB;_TL1BA: _TL1BB: goto _LL0;}}}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;_T1F0=_T1F4->f2;}{struct _fat_ptr*v=_T1F3;struct Cyc_List_List*ds2=_T1F0;{
# 1113
void(*_T1F4)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns;_T193=_T1F4;}_T194=env;_T195=_T194->ns;_T196=v;_T193(_T195,_T196,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);_T197=env;_T198=_T197->ns;
Cyc_Binding_leave_ns(_T198);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1ED;_T1F3=_T1F4->f1;_T1F0=_T1F4->f2;}{struct _tuple0*qv=_T1F3;struct Cyc_List_List*ds2=_T1F0;_T199=d;_T19A=_T199->loc;_T19B=env;_T19C=_T19B->ns;_T19D=qv;
# 1118
Cyc_Binding_enter_using(_T19A,_T19C,_T19D);
Cyc_Binding_resolve_decls(env,ds2);_T19E=env;_T19F=_T19E->ns;
Cyc_Binding_leave_using(_T19F);goto _LL0;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T1ED;_T1F0=_T1F4->f1;}{struct Cyc_List_List*ds2=_T1F0;_T1A0=env;{
# 1124
int old=_T1A0->in_cinclude;_T1A1=env;
_T1A1->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T1A2=env;
_T1A2->in_cinclude=old;goto _LL0;}}case 12:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1F4=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1ED;_T1F0=_T1F4->f1;_T1EF=_T1F4->f2;_T1A3=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1ED;_T1A4=& _T1A3->f3;_T1F3=(struct Cyc_List_List**)_T1A4;_T1EE=_T1F4->f4;}{struct Cyc_List_List*ds2=_T1F0;struct Cyc_List_List*ovrs=_T1EF;struct Cyc_List_List**exports=(struct Cyc_List_List**)_T1F3;struct _tuple10*hides=_T1EE;_T1A5=exports;{
# 1135
struct Cyc_List_List*exs=*_T1A5;_TL1CC: if(exs!=0)goto _TL1CA;else{goto _TL1CB;}
_TL1CA: _T1A6=exs;_T1A7=_T1A6->hd;{struct _tuple15*_T1F4=(struct _tuple15*)_T1A7;struct _tuple0*_T1F5;unsigned _T1F6;{struct _tuple15 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1F5=_T1F7.f1;}{unsigned loc=_T1F6;struct _tuple0*qv=_T1F5;
Cyc_Binding_absolutize_decl(loc,env,qv,4U);}}_T1A8=exs;
# 1135
exs=_T1A8->tl;goto _TL1CC;_TL1CB:;}_T1A9=
# 1143
Cyc_Binding_at_toplevel(env);if(_T1A9)goto _TL1CD;else{goto _TL1CF;}
_TL1CF:{struct Cyc_Warn_String_Warn_Warg_struct _T1F4;_T1F4.tag=0;_T1F4.f1=_tag_fat("extern \"C include\" not at toplevel",sizeof(char),35U);_T1AA=_T1F4;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F4=_T1AA;void*_T1F5[1];_T1AB=_T1F5 + 0;*_T1AB=& _T1F4;_T1AC=loc;_T1AD=_tag_fat(_T1F5,sizeof(void*),1);Cyc_Warn_err2(_T1AC,_T1AD);}goto _LL0;_TL1CD: _T1AF=Cyc_Dict_lookup;{
# 1147
struct Cyc_Binding_ResolveNSEnv*(*_T1F4)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1AF;_T1AE=_T1F4;}_T1B0=env;_T1B1=_T1B0->ns;_T1B2=_T1B1->ns_data;_T1B3=env;_T1B4=_T1B3->ns;_T1B5=_T1B4->curr_ns;{struct Cyc_Binding_ResolveNSEnv*data=_T1AE(_T1B2,_T1B5);_T1B6=data;{
struct Cyc_Dict_Dict old_dict=_T1B6->ordinaries;_T1B7=env;{
int old=_T1B7->in_cinclude;_T1B8=env;
_T1B8->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T1B9=env;
_T1B9->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);_T1BA=env;
_T1BA->in_cyc_override=0;_T1BB=data;{
struct Cyc_Dict_Dict new_dict=_T1BB->ordinaries;
struct Cyc_Dict_Dict out_dict=old_dict;_T1BC=hides;_T1BD=*_T1BC;_T1BE=_T1BD.f0;
if(_T1BE <= 0U)goto _TL1D0;_T1BF=exports;_T1C0=*_T1BF;_T1C1=(unsigned)_T1C0;
if(!_T1C1)goto _TL1D2;_T1C2=hides;_T1C3=*_T1C2;_T1C4=_T1C3.f0;_T1C5=
_tag_fat("export wildcard expects empty export list",sizeof(char),42U);_T1C6=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1C4,_T1C5,_T1C6);goto _TL1D3;_TL1D2: _TL1D3: _T1C7=env;
_T1C7->in_cinclude=old;_T1C9=Cyc_Dict_iter_c;{
void(*_T1F4)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict))_T1C9;_T1C8=_T1F4;}{struct _tuple14*_T1F4=_cycalloc(sizeof(struct _tuple14));_T1F4->f0=exports;_T1CB=& out_dict;_T1F4->f1=(struct Cyc_Dict_Dict*)_T1CB;_T1F4->f2=env;_T1F4->f3=hides;_T1CA=(struct _tuple14*)_T1F4;}_T1CC=
Cyc_Dict_difference(new_dict,old_dict);
# 1161
_T1C8(Cyc_Binding_export_all_symbols,_T1CA,_T1CC);goto _TL1D1;
# 1164
_TL1D0: _T1CD=exports;{struct Cyc_List_List*exs=*_T1CD;_TL1D7: if(exs!=0)goto _TL1D5;else{goto _TL1D6;}
_TL1D5: _T1CE=exs;_T1CF=_T1CE->hd;{struct _tuple15*_T1F4=(struct _tuple15*)_T1CF;struct _fat_ptr*_T1F5;unsigned _T1F6;{struct _tuple15 _T1F7=*_T1F4;_T1F6=_T1F7.f0;_T1D0=_T1F7.f1;{struct _tuple0 _T1F8=*_T1D0;_T1F5=_T1F8.f1;}}{unsigned loc=_T1F6;struct _fat_ptr*v=_T1F5;_T1D2=Cyc_Dict_member;{
int(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D2;_T1D1=_T1F7;}_T1D3=_T1D1(new_dict,v);if(_T1D3)goto _TL1DB;else{goto _TL1DA;}_TL1DB: _T1D5=Cyc_Dict_member;{
int(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D5;_T1D4=_T1F7;}_T1D6=_T1D4(old_dict,v);
# 1166
if(_T1D6)goto _TL1DC;else{goto _TL1D8;}_TL1DC: _T1D8=Cyc_Dict_lookup;{
# 1168
void*(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D8;_T1D7=_T1F7;}_T1D9=_T1D7(old_dict,v);_T1DB=Cyc_Dict_lookup;{void*(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1DB;_T1DA=_T1F7;}_T1DC=_T1DA(new_dict,v);
# 1166
if(_T1D9==_T1DC)goto _TL1DA;else{goto _TL1D8;}
# 1169
_TL1DA:{struct Cyc_String_pa_PrintArg_struct _T1F7;_T1F7.tag=0;_T1DE=v;_T1F7.f1=*_T1DE;_T1DD=_T1F7;}{struct Cyc_String_pa_PrintArg_struct _T1F7=_T1DD;void*_T1F8[1];_T1DF=_T1F8 + 0;*_T1DF=& _T1F7;_T1E0=loc;_T1E1=_tag_fat("%s is exported but not defined",sizeof(char),31U);_T1E2=_tag_fat(_T1F8,sizeof(void*),1);Cyc_Warn_err(_T1E0,_T1E1,_T1E2);}goto _TL1D9;_TL1D8: _TL1D9: _T1E4=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T1E4;_T1E3=_T1F7;}_T1E5=out_dict;_T1E6=v;_T1E8=Cyc_Dict_lookup;{void*(*_T1F7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1E8;_T1E7=_T1F7;}_T1E9=_T1E7(new_dict,v);out_dict=_T1E3(_T1E5,_T1E6,_T1E9);}}_T1EA=exs;
# 1164
exs=_T1EA->tl;goto _TL1D7;_TL1D6:;}_TL1D1: _T1EB=data;
# 1173
_T1EB->ordinaries=out_dict;_T1EC=env;
_T1EC->in_cinclude=old;goto _LL0;}}}}}case 13: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: goto _LL0;}_LL0:;}}}
# 1184
static void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Decl*_T3;struct Cyc_List_List*_T4;
_TL1E0: if(tds!=0)goto _TL1DE;else{goto _TL1DF;}
_TL1DE: _T0=env;_T1=tds;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Decl*)_T2;Cyc_Binding_resolve_decl(_T0,_T3);_T4=tds;
# 1185
tds=_T4->tl;goto _TL1E0;_TL1DF:;}
# 1189
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;struct Cyc_Binding_NSCtxt*(*_T3)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T4;struct Cyc_Binding_Env*_T5;struct Cyc_Absyn_Decl*_T6;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T7;
struct Cyc_Binding_Env*env;env=_cycalloc(sizeof(struct Cyc_Binding_Env));_T0=env;_T0->in_cinclude=0;_T1=env;_T1->in_cyc_override=0;_T2=env;{struct Cyc_Binding_NSCtxt*(*_T8)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt;_T3=_T8;}_T2->ns=_T3(1,Cyc_Binding_mt_renv);_T4=env;_T4->local_vars=0;_T5=env;{struct Cyc_Absyn_Decl*_T8=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T9=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T9->tag=6;
_T9->f1=Cyc_Absyn_exn_tud();_T7=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T9;}_T8->r=(void*)_T7;_T8->loc=0U;_T6=(struct Cyc_Absyn_Decl*)_T8;}Cyc_Binding_resolve_decl(_T5,_T6);
Cyc_Binding_resolve_decls(env,tds);}
# 1195
static struct Cyc_List_List*Cyc_Binding_extern_functions;
static int Cyc_Binding_enable_interproc=0;
# 1198
void Cyc_Binding_interproc_binding_handler(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;struct Cyc_Warn_Qvar_Warn_Warg_struct _T9;void**_TA;void**_TB;struct Cyc_Absyn_Exp*_TC;unsigned _TD;struct _fat_ptr _TE;void*_TF;int _T10;struct Cyc_Absyn_Vardecl*_T11;void*_T12;int _T13;struct Cyc_List_List*_T14;int _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_Exp_Warn_Warg_struct _T17;void**_T18;void**_T19;int(*_T1A)(struct _fat_ptr);void*(*_T1B)(struct _fat_ptr);struct _fat_ptr _T1C;_T0=e;{
void*_T1D=_T0->r;void*_T1E;_T1=(int*)_T1D;_T2=*_T1;if(_T2!=1)goto _TL1E1;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T1D;_T3=_T1F->f1;_T1E=(void*)_T3;}{void*b=_T1E;{struct Cyc_Absyn_Vardecl*_T1F;struct _tuple0*_T20;_T4=b;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 0: _T7=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T21=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T7;_T20=_T21->f1;}{struct _tuple0*q=_T20;{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;
# 1206
_T21.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T8=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_T8;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22;_T22.tag=1;_T22.f1=q;_T9=_T22;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T22=_T9;void*_T23[2];_TA=_T23 + 0;*_TA=& _T21;_TB=_T23 + 1;*_TB=& _T22;_TC=e;_TD=_TC->loc;_TE=_tag_fat(_T23,sizeof(void*),2);Cyc_Warn_err2(_TD,_TE);}}
return;}case 1: _TF=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T21=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TF;_T1F=_T21->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1F;_T10=Cyc_Binding_enable_interproc;
# 1209
if(!_T10)goto _TL1E4;_T11=vd;_T12=_T11->type;_T13=Cyc_Tcutil_is_function_type(_T12);if(!_T13)goto _TL1E4;{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
_T21->hd=e;_T21->tl=Cyc_Binding_extern_functions;_T14=(struct Cyc_List_List*)_T21;}Cyc_Binding_extern_functions=_T14;goto _TL1E5;_TL1E4: _TL1E5: goto _LL5;}case 2: goto _LLD;case 3: _LLD: goto _LLF;case 4: _LLF: goto _LL11;default: _LL11: goto _LL5;}_LL5:;}goto _LL0;}_TL1E1: _T15=Cyc_Binding_enable_interproc;
# 1221
if(!_T15)goto _TL1E6;{struct Cyc_Warn_String_Warn_Warg_struct _T1F;_T1F.tag=0;
_T1F.f1=_tag_fat("Interproc binding got unsupported expression: ",sizeof(char),47U);_T16=_T1F;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F=_T16;{struct Cyc_Warn_Exp_Warn_Warg_struct _T20;_T20.tag=4;_T20.f1=e;_T17=_T20;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T20=_T17;void*_T21[2];_T18=_T21 + 0;*_T18=& _T1F;_T19=_T21 + 1;*_T19=& _T20;_T1B=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1B;_T1A=_T22;}_T1C=_tag_fat(_T21,sizeof(void*),2);_T1A(_T1C);}}goto _TL1E7;_TL1E6: _TL1E7: goto _LL0;_LL0:;}}
# 1227
struct Cyc_List_List*Cyc_Binding_resolve_all_interproc(struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;struct Cyc_Binding_NSCtxt*(*_T3)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T4;struct Cyc_Binding_Env*_T5;struct Cyc_Absyn_Decl*_T6;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T7;struct Cyc_List_List*_T8;
Cyc_Binding_enable_interproc=1;
Cyc_Binding_extern_functions=0;{
struct Cyc_Binding_Env*env;env=_cycalloc(sizeof(struct Cyc_Binding_Env));_T0=env;_T0->in_cinclude=0;_T1=env;_T1->in_cyc_override=0;_T2=env;{struct Cyc_Binding_NSCtxt*(*_T9)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt;_T3=_T9;}_T2->ns=_T3(1,Cyc_Binding_mt_renv);_T4=env;_T4->local_vars=0;_T5=env;{struct Cyc_Absyn_Decl*_T9=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TA=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TA->tag=6;
_TA->f1=Cyc_Absyn_exn_tud();_T7=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TA;}_T9->r=(void*)_T7;_T9->loc=0U;_T6=(struct Cyc_Absyn_Decl*)_T9;}Cyc_Binding_resolve_decl(_T5,_T6);
Cyc_Binding_resolve_decls(env,tds);{
struct Cyc_List_List*ret=0;{
struct Cyc_List_List*_T9=ret;struct Cyc_List_List*_TA=Cyc_Binding_extern_functions;ret=_TA;Cyc_Binding_extern_functions=_T9;}_T8=ret;
return _T8;}}}
