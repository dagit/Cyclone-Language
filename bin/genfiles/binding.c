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
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){struct Cyc_Binding_NSCtxt*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;int(*_T3)(struct Cyc_Dict_Dict,struct Cyc_List_List*);int(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_List_List*_T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_Binding_NSCtxt*_TA;struct Cyc_List_List*_TB;int*_TC;int _TD;int(*_TE)(struct Cyc_Dict_Dict,struct Cyc_List_List*);int(*_TF)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_List_List*_T12;int _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_List_List*_T18;struct _fat_ptr _T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;unsigned _T1B;struct _fat_ptr _T1C;struct Cyc_Binding_BindingError_exn_struct*_T1D;void*_T1E;_T0=ctxt;_T1=_T0->curr_ns;_T2=rel_ns;{
# 87
struct Cyc_List_List*fullname=Cyc_List_append(_T1,_T2);_T4=Cyc_Dict_member;{
int(*_T1F)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T4;_T3=_T1F;}_T5=ctxt;_T6=_T5->ns_data;_T7=fullname;_T8=_T3(_T6,_T7);if(!_T8)goto _TL1;_T9=fullname;
return _T9;_TL1: _TA=ctxt;{
struct Cyc_List_List*as=_TA->availables;_TL6: if(as!=0)goto _TL4;else{goto _TL5;}
_TL4: _TB=as;{void*_T1F=_TB->hd;struct Cyc_List_List*_T20;_TC=(int*)_T1F;_TD=*_TC;if(_TD!=1)goto _TL7;{struct Cyc_Binding_Using_Binding_NSDirective_struct*_T21=(struct Cyc_Binding_Using_Binding_NSDirective_struct*)_T1F;_T20=_T21->f1;}{struct Cyc_List_List*ns=_T20;_T20=ns;goto _LL4;}_TL7:{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T21=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T1F;_T20=_T21->f1;}_LL4:{struct Cyc_List_List*ns=_T20;
# 94
struct Cyc_List_List*fullname=Cyc_List_append(ns,rel_ns);_TF=Cyc_Dict_member;{
int(*_T21)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_TF;_TE=_T21;}_T10=ctxt;_T11=_T10->ns_data;_T12=fullname;_T13=_TE(_T11,_T12);if(!_T13)goto _TL9;_T14=fullname;
return _T14;_TL9: goto _LL0;}_LL0:;}_T15=as;
# 90
as=_T15->tl;goto _TL6;_TL5:;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F;_T1F.tag=0;
# 99
_T1F.f1=_tag_fat("namespace ",sizeof(char),11U);_T16=_T1F;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F=_T16;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;_T18=rel_ns;_T19=_tag_fat("::",sizeof(char),3U);_T20.f1=Cyc_str_sepstr(_T18,_T19);_T17=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T17;{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;_T21.f1=_tag_fat(" not found",sizeof(char),11U);_T1A=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_T1A;void*_T22[3];_T22[0]=& _T1F;_T22[1]=& _T20;_T22[2]=& _T21;_T1B=loc;_T1C=_tag_fat(_T22,sizeof(void*),3);Cyc_Warn_err2(_T1B,_T1C);}}}{struct Cyc_Binding_BindingError_exn_struct*_T1F=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T1F->tag=Cyc_Binding_BindingError;_T1D=(struct Cyc_Binding_BindingError_exn_struct*)_T1F;}_T1E=(void*)_T1D;_throw(_T1E);}}
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
static void Cyc_Binding_dump_local_entry(struct _fat_ptr*v,void*r){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;{struct Cyc_String_pa_PrintArg_struct _T4;_T4.tag=0;_T1=v;
_T4.f1=*_T1;_T0=_T4;}{struct Cyc_String_pa_PrintArg_struct _T4=_T0;void*_T5[1];_T5[0]=& _T4;_T2=_tag_fat("%s resolved\n",sizeof(char),13U);_T3=_tag_fat(_T5,sizeof(void*),1);Cyc_printf(_T2,_T3);}}
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
static void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){struct _tuple0*_T0;struct _tuple0 _T1;struct _union_Nmspace_Abs_n _T2;unsigned _T3;struct _union_Nmspace_Rel_n _T4;struct Cyc_List_List*_T5;int _T6;struct _tuple0*_T7;struct Cyc_Binding_Env*_T8;struct Cyc_Binding_NSCtxt*_T9;struct Cyc_List_List*_TA;int _TB;int _TC;enum Cyc_Absyn_Scope _TD;int _TE;struct _tuple0*_TF;int _T10;struct Cyc_Warn_String_Warn_Warg_struct _T11;struct Cyc_Warn_Qvar_Warn_Warg_struct _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;unsigned _T14;struct _fat_ptr _T15;_T0=qv;_T1=*_T0;{
union Cyc_Absyn_Nmspace _T16=_T1.f0;_T2=_T16.Abs_n;_T3=_T2.tag;switch(_T3){case 2: _T4=_T16.Rel_n;_T5=_T4.val;if(_T5!=0)goto _TL47;_T6=
# 287
Cyc_Binding_at_toplevel(env);if(!_T6)goto _TL49;_T7=qv;_T8=env;_T9=_T8->ns;_TA=_T9->curr_ns;_TC=
Cyc_Binding_in_cinclude(env);if(!_TC)goto _TL4B;_TB=1;goto _TL4C;_TL4B: _TD=sc;_TE=(int)_TD;_TB=_TE==4;_TL4C:(*_T7).f0=Cyc_Absyn_Abs_n(_TA,_TB);goto _TL4A;
# 290
_TL49: _TF=qv;(*_TF).f0=Cyc_Absyn_Loc_n();_TL4A: goto _LL0;_TL47: goto _LL7;case 4: goto _LL0;case 1: _T10=
# 294
Cyc_Binding_at_toplevel(env);if(_T10)goto _TL4D;else{goto _TL4F;}
_TL4F: goto _LL8;_TL4D: goto _LL0;default: _LL7: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
# 297
_T17.f1=_tag_fat("qualified names in declarations unimplemented (",sizeof(char),48U);_T11=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_T11;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T18;_T18.tag=1;_T18.f1=qv;_T12=_T18;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T18=_T12;{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;_T19.f1=_tag_fat(")",sizeof(char),2U);_T13=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_T13;void*_T1A[3];_T1A[0]=& _T17;_T1A[1]=& _T18;_T1A[2]=& _T19;_T14=loc;_T15=_tag_fat(_T1A,sizeof(void*),3);Cyc_Warn_err2(_T14,_T15);}}}}_LL0:;}}
# 303
static void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;_T0=env;{
struct Cyc_Dict_Dict*old_locals=_T0->local_vars;_T1=env;
_T1->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);_T2=env;
_T2->local_vars=old_locals;}}
# 310
static void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){struct _handler_cons*_T0;int*_T1;int _T2;int*_T3;unsigned _T4;void*_T5;int _T6;void*_T7;int*_T8;unsigned _T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;unsigned _T10;struct _fat_ptr _T11;void*_T12;struct Cyc_Binding_BindingError_exn_struct*_T13;char*_T14;char*_T15;
struct _fat_ptr hides_what;struct _handler_cons _T16;_T0=& _T16;_push_handler(_T0);{int _T17=0;_T1=_T16.handler;_T2=setjmp(_T1);if(!_T2)goto _TL50;_T17=1;goto _TL51;_TL50: _TL51: if(_T17)goto _TL52;else{goto _TL54;}_TL54:{
# 313
void*_T18=Cyc_Binding_lookup_ordinary(loc,env,q);void*_T19;_T3=(int*)_T18;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1A=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T18;_T5=_T1A->f1;_T19=(void*)_T5;}{void*b=_T19;_T6=
# 315
Cyc_Binding_at_toplevel(env);if(!_T6)goto _TL56;_npop_handler(0);return;_TL56: _T7=b;_T8=(int*)_T7;_T9=*_T8;switch(_T9){case 1:
# 318
 hides_what=_tag_fat("global variable",sizeof(char),16U);goto _LLB;case 4:
 hides_what=_tag_fat("local variable",sizeof(char),15U);goto _LLB;case 3:
 hides_what=_tag_fat("parameter",sizeof(char),10U);goto _LLB;case 5:
 hides_what=_tag_fat("pattern variable",sizeof(char),17U);goto _LLB;case 2:
 hides_what=_tag_fat("function",sizeof(char),9U);goto _LLB;default: _TB=Cyc_Warn_impos;{
int(*_T1A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T1A;}_TC=_tag_fat("shadowing free variable!",sizeof(char),25U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);}_LLB: goto _LL0;}case 1:
# 326
 hides_what=_tag_fat("struct constructor",sizeof(char),19U);goto _LL0;case 2:
 hides_what=_tag_fat("datatype constructor",sizeof(char),21U);goto _LL0;case 3: goto _LLA;default: _LLA:
# 329
 hides_what=_tag_fat("enum tag",sizeof(char),9U);goto _LL0;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;
# 331
_T18.f1=_tag_fat("declaration hides ",sizeof(char),19U);_TE=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;_T19.f1=hides_what;_TF=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_TF;void*_T1A[2];_T1A[0]=& _T18;_T1A[1]=& _T19;_T10=loc;_T11=_tag_fat(_T1A,sizeof(void*),2);Cyc_Warn_warn2(_T10,_T11);}}_npop_handler(0);return;_pop_handler();goto _TL53;_TL52: _T12=Cyc_Core_get_exn_thrown();{void*_T18=(void*)_T12;void*_T19;_T13=(struct Cyc_Binding_BindingError_exn_struct*)_T18;_T14=_T13->tag;_T15=Cyc_Binding_BindingError;if(_T14!=_T15)goto _TL59;
# 333
return;_TL59: _T19=_T18;{void*exn=_T19;_rethrow(exn);};}_TL53:;}}
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
static int Cyc_Binding_resolve_visit_f1(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void**_T4;void**_T5;int*_T6;int _T7;struct _handler_cons*_T8;int*_T9;int _TA;struct Cyc_Absyn_Exp*_TB;unsigned _TC;struct Cyc_Binding_Env*_TD;struct _tuple0*_TE;int*_TF;unsigned _T10;void*_T11;void**_T12;struct Cyc_Absyn_Exp*_T13;unsigned _T14;struct Cyc_Binding_Env*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T1A;void*_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T1D;struct Cyc_Warn_String_Warn_Warg_struct _T1E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1F;struct Cyc_Absyn_Exp*_T20;unsigned _T21;struct _fat_ptr _T22;void*_T23;struct Cyc_Binding_BindingError_exn_struct*_T24;char*_T25;char*_T26;struct Cyc_Warn_String_Warn_Warg_struct _T27;struct Cyc_Warn_Qvar_Warn_Warg_struct _T28;struct Cyc_Absyn_Exp*_T29;unsigned _T2A;struct _fat_ptr _T2B;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T2C;int*_T2D;int*_T2E;struct Cyc_Binding_Env*_T2F;struct Cyc_List_List*_T30;void*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_List_List*_T33;struct Cyc_Absyn_Exp*_T34;int*_T35;int _T36;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T37;void**_T38;void**_T39;int*_T3A;int _T3B;struct _handler_cons*_T3C;int*_T3D;int _T3E;struct Cyc_Absyn_Exp*_T3F;unsigned _T40;struct Cyc_Binding_Env*_T41;struct _tuple0*_T42;int*_T43;unsigned _T44;void*_T45;void**_T46;struct Cyc_Absyn_Exp*_T47;unsigned _T48;struct Cyc_Binding_Env*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_List_List*_T4B;struct _tuple11*_T4C;struct Cyc_List_List*_T4D;void*_T4E;struct Cyc_List_List*_T4F;struct Cyc_Absyn_Exp*_T50;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T51;struct Cyc_Absyn_Aggrdecl*_T52;struct Cyc_Absyn_Datatypefield*_T53;struct Cyc_List_List*_T54;struct Cyc_Warn_Qvar_Warn_Warg_struct _T55;struct Cyc_Absyn_Datatypefield*_T56;struct Cyc_Warn_String_Warn_Warg_struct _T57;struct Cyc_Absyn_Exp*_T58;unsigned _T59;struct _fat_ptr _T5A;struct Cyc_Absyn_Exp*_T5B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T5C;struct Cyc_Warn_Qvar_Warn_Warg_struct _T5D;struct Cyc_Warn_String_Warn_Warg_struct _T5E;struct Cyc_Absyn_Exp*_T5F;unsigned _T60;struct _fat_ptr _T61;void*_T62;struct Cyc_Binding_BindingError_exn_struct*_T63;char*_T64;char*_T65;struct Cyc_Absyn_Exp*_T66;unsigned _T67;struct Cyc_Binding_Env*_T68;struct Cyc_Absyn_Exp*_T69;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T6A;struct _tuple0**_T6B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T6C;struct Cyc_Absyn_Aggrdecl**_T6D;struct _handler_cons*_T6E;int*_T6F;int _T70;struct Cyc_Absyn_Aggrdecl**_T71;struct Cyc_Absyn_Aggrdecl*(*_T72)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));struct Cyc_Absyn_Exp*_T73;unsigned _T74;struct Cyc_Binding_Env*_T75;struct Cyc_Binding_NSCtxt*_T76;struct _tuple0**_T77;struct _tuple0*_T78;struct _tuple0**_T79;struct Cyc_Absyn_Aggrdecl**_T7A;struct Cyc_Absyn_Aggrdecl*_T7B;struct Cyc_Absyn_Aggrdecl*_T7C;void*_T7D;struct Cyc_Binding_BindingError_exn_struct*_T7E;char*_T7F;char*_T80;struct Cyc_Warn_String_Warn_Warg_struct _T81;struct Cyc_Warn_Qvar_Warn_Warg_struct _T82;struct _tuple0**_T83;struct Cyc_Absyn_Exp*_T84;unsigned _T85;struct _fat_ptr _T86;int _T87;struct Cyc_Binding_Env*_T88;struct Cyc_Dict_Dict*_T89;struct Cyc_Dict_Dict(*_T8A)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T8B)(int(*)(void*,void*));int(*_T8C)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Absyn_Exp*_T8D;unsigned _T8E;struct Cyc_Binding_Env*_T8F;struct Cyc_Absyn_Vardecl*_T90;struct Cyc_Binding_Env*_T91;struct Cyc_Binding_Env*_T92;struct Cyc_Dict_Dict*_T93;struct Cyc_Dict_Dict*_T94;struct Cyc_Absyn_Exp*_T95;unsigned _T96;struct Cyc_Binding_Env*_T97;struct Cyc_Absyn_Vardecl*_T98;struct Cyc_Binding_Env*_T99;struct Cyc_Dict_Dict*_T9A;struct Cyc_Dict_Dict*_T9B;struct Cyc_Absyn_Exp*_T9C;unsigned _T9D;struct Cyc_Binding_Env*_T9E;struct Cyc_List_List*_T9F;void*_TA0;struct Cyc_List_List*_TA1;struct Cyc_Absyn_MallocInfo _TA2;void**_TA3;void*_TA4;void*_TA5;struct _tuple8*_TA6;void*_TA7;void*_TA8;void*_TA9;void*_TAA;void*_TAB;struct Cyc_Absyn_Exp*_TAC;unsigned _TAD;struct Cyc_Binding_Env*_TAE;void*_TAF;_T0=e;{
void*_TB0=_T0->r;struct Cyc_Absyn_Exp*_TB1;struct Cyc_Absyn_Exp*_TB2;struct Cyc_List_List*_TB3;void*_TB4;struct Cyc_List_List*_TB5;void*_TB6;_T1=(int*)_TB0;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB0;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB0;_T4=& _T3->f1;_TB6=(void**)_T4;}{void**b=_TB6;_T5=b;{
# 358
void*_TB7=*_T5;struct _tuple0*_TB8;_T6=(int*)_TB7;_T7=*_T6;if(_T7!=0)goto _TL64;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TB9=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TB7;_TB8=_TB9->f1;}{struct _tuple0*q=_TB8;{struct _handler_cons _TB9;_T8=& _TB9;_push_handler(_T8);{int _TBA=0;_T9=_TB9.handler;_TA=setjmp(_T9);if(!_TA)goto _TL66;_TBA=1;goto _TL67;_TL66: _TL67: if(_TBA)goto _TL68;else{goto _TL6A;}_TL6A: _TB=e;_TC=_TB->loc;_TD=env;_TE=q;{
# 360
void*_TBB=Cyc_Binding_lookup_ordinary(_TC,_TD,_TE);struct Cyc_Absyn_Aggrdecl*_TBC;struct Cyc_Absyn_Enumfield*_TBD;struct Cyc_Absyn_Datatypefield*_TBE;void*_TBF;_TF=(int*)_TBB;_T10=*_TF;switch(_T10){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TC0=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TBB;_T11=_TC0->f1;_TBF=(void*)_T11;}{void*bnd=_TBF;_T12=b;
*_T12=bnd;_T13=e;_T14=_T13->loc;_T15=env;_T16=e;Cyc_Binding_interproc_binding_handler(_T14,_T15,_T16);goto _LL24;}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TC0=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TBB;_TBF=_TC0->f1;_TBE=_TC0->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TBF;struct Cyc_Absyn_Datatypefield*tuf=_TBE;_T17=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TC0=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TC0->tag=30;
_TC0->f1=0;_TC0->f2=tud;_TC0->f3=tuf;_T18=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TC0;}_T17->r=(void*)_T18;goto _LL24;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_TC0=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_TBB;_TBF=_TC0->f1;_TBD=_TC0->f2;}{struct Cyc_Absyn_Enumdecl*ed=_TBF;struct Cyc_Absyn_Enumfield*ef=_TBD;_T19=e;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_TC0=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_TC0->tag=31;
_TC0->f1=ed;_TC0->f2=ef;_T1A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_TC0;}_T19->r=(void*)_T1A;goto _LL24;}case 4:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TC0=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_TBB;_T1B=_TC0->f1;_TBF=(void*)_T1B;_TBD=_TC0->f2;}{void*t=_TBF;struct Cyc_Absyn_Enumfield*ef=_TBD;_T1C=e;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TC0=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_TC0->tag=32;
_TC0->f1=t;_TC0->f2=ef;_T1D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TC0;}_T1C->r=(void*)_T1D;goto _LL24;}default:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TC0=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TBB;_TBC=_TC0->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TBC;{struct Cyc_Warn_String_Warn_Warg_struct _TC0;_TC0.tag=0;
_TC0.f1=_tag_fat("bad occurrence of type name ",sizeof(char),29U);_T1E=_TC0;}{struct Cyc_Warn_String_Warn_Warg_struct _TC0=_T1E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC1;_TC1.tag=1;_TC1.f1=q;_T1F=_TC1;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC1=_T1F;void*_TC2[2];_TC2[0]=& _TC0;_TC2[1]=& _TC1;_T20=e;_T21=_T20->loc;_T22=_tag_fat(_TC2,sizeof(void*),2);Cyc_Warn_err2(_T21,_T22);}}goto _LL24;}}_LL24:;}_pop_handler();goto _TL69;_TL68: _T23=Cyc_Core_get_exn_thrown();{void*_TBB=(void*)_T23;void*_TBC;_T24=(struct Cyc_Binding_BindingError_exn_struct*)_TBB;_T25=_T24->tag;_T26=Cyc_Binding_BindingError;if(_T25!=_T26)goto _TL6C;{struct Cyc_Warn_String_Warn_Warg_struct _TBD;_TBD.tag=0;
# 368
_TBD.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T27=_TBD;}{struct Cyc_Warn_String_Warn_Warg_struct _TBD=_T27;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBE;_TBE.tag=1;_TBE.f1=q;_T28=_TBE;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBE=_T28;void*_TBF[2];_TBF[0]=& _TBD;_TBF[1]=& _TBE;_T29=e;_T2A=_T29->loc;_T2B=_tag_fat(_TBF,sizeof(void*),2);Cyc_Warn_err2(_T2A,_T2B);}}goto _LL2F;_TL6C: _TBC=_TBB;{void*exn=_TBC;_rethrow(exn);}_LL2F:;}_TL69:;}}
# 370
return 0;}_TL64:
 return 0;;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TB0;_TB6=_TB7->f1;_TB5=_TB7->f2;_T2C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TB0;_T2D=& _T2C->f4;_TB4=(int*)_T2D;}{struct Cyc_Absyn_Exp*e1=_TB6;struct Cyc_List_List*es=_TB5;int*b=_TB4;_T2E=b;
# 374
*_T2E=1;{
# 376
struct Cyc_List_List*es2=es;_TL71: if(es2!=0)goto _TL6F;else{goto _TL70;}
_TL6F: _T2F=env;_T30=es2;_T31=_T30->hd;_T32=(struct Cyc_Absyn_Exp*)_T31;Cyc_Binding_resolve_exp(_T2F,_T32);_T33=es2;
# 376
es2=_T33->tl;goto _TL71;_TL70:;}_T34=e1;{
# 378
void*_TB7=_T34->r;void*_TB8;_T35=(int*)_TB7;_T36=*_T35;if(_T36!=1)goto _TL72;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB7;_T37=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB7;_T38=& _T37->f1;_TB8=(void**)_T38;}{void**b=(void**)_TB8;_T39=b;{
# 380
void*_TB9=*_T39;struct _tuple0*_TBA;_T3A=(int*)_TB9;_T3B=*_T3A;if(_T3B!=0)goto _TL74;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TBB=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TB9;_TBA=_TBB->f1;}{struct _tuple0*q=_TBA;struct _handler_cons _TBB;_T3C=& _TBB;_push_handler(_T3C);{int _TBC=0;_T3D=_TBB.handler;_T3E=setjmp(_T3D);if(!_T3E)goto _TL76;_TBC=1;goto _TL77;_TL76: _TL77: if(_TBC)goto _TL78;else{goto _TL7A;}_TL7A: _T3F=e1;_T40=_T3F->loc;_T41=env;_T42=q;{
# 382
void*_TBD=Cyc_Binding_lookup_ordinary(_T40,_T41,_T42);struct Cyc_Absyn_Datatypefield*_TBE;struct Cyc_Absyn_Datatypedecl*_TBF;struct Cyc_Absyn_Aggrdecl*_TC0;void*_TC1;_T43=(int*)_TBD;_T44=*_T43;switch(_T44){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TC2=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TBD;_T45=_TC2->f1;_TC1=(void*)_T45;}{void*bnd=_TC1;_T46=b;
# 384
*_T46=bnd;_T47=e1;_T48=_T47->loc;_T49=env;_T4A=e1;
Cyc_Binding_interproc_binding_handler(_T48,_T49,_T4A);{int _TC2=0;_npop_handler(0);return _TC2;}}case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TC2=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TBD;_TC0=_TC2->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TC0;
# 388
struct Cyc_List_List*dles=0;
_TL7F: if(es!=0)goto _TL7D;else{goto _TL7E;}
_TL7D:{struct Cyc_List_List*_TC2=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_TC3=_cycalloc(sizeof(struct _tuple11));_TC3->f0=0;_T4D=es;_T4E=_T4D->hd;_TC3->f1=(struct Cyc_Absyn_Exp*)_T4E;_T4C=(struct _tuple11*)_TC3;}_TC2->hd=_T4C;_TC2->tl=dles;_T4B=(struct Cyc_List_List*)_TC2;}dles=_T4B;_T4F=es;
# 389
es=_T4F->tl;goto _TL7F;_TL7E: _T50=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TC2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TC2->tag=28;_T52=ad;
# 391
_TC2->f1=_T52->name;_TC2->f2=0;_TC2->f3=Cyc_List_imp_rev(dles);_TC2->f4=ad;_T51=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TC2;}_T50->r=(void*)_T51;{int _TC2=0;_npop_handler(0);return _TC2;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TC2=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TBD;_TBF=_TC2->f1;_TBE=_TC2->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TBF;struct Cyc_Absyn_Datatypefield*tuf=_TBE;_T53=tuf;_T54=_T53->typs;
# 394
if(_T54!=0)goto _TL80;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC2;_TC2.tag=1;_T56=tuf;
_TC2.f1=_T56->name;_T55=_TC2;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC2=_T55;{struct Cyc_Warn_String_Warn_Warg_struct _TC3;_TC3.tag=0;_TC3.f1=_tag_fat(" is a constant, not a function",sizeof(char),31U);_T57=_TC3;}{struct Cyc_Warn_String_Warn_Warg_struct _TC3=_T57;void*_TC4[2];_TC4[0]=& _TC2;_TC4[1]=& _TC3;_T58=e;_T59=_T58->loc;_T5A=_tag_fat(_TC4,sizeof(void*),2);Cyc_Warn_err2(_T59,_T5A);}}goto _TL81;_TL80: _TL81: _T5B=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TC2=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TC2->tag=30;
_TC2->f1=es;_TC2->f2=tud;_TC2->f3=tuf;_T5C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TC2;}_T5B->r=(void*)_T5C;{int _TC2=0;_npop_handler(0);return _TC2;}}case 4: goto _LL48;default: _LL48:{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC2;_TC2.tag=1;
# 400
_TC2.f1=q;_T5D=_TC2;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TC2=_T5D;{struct Cyc_Warn_String_Warn_Warg_struct _TC3;_TC3.tag=0;_TC3.f1=_tag_fat(" is an enum constructor, not a function",sizeof(char),40U);_T5E=_TC3;}{struct Cyc_Warn_String_Warn_Warg_struct _TC3=_T5E;void*_TC4[2];_TC4[0]=& _TC2;_TC4[1]=& _TC3;_T5F=e;_T60=_T5F->loc;_T61=_tag_fat(_TC4,sizeof(void*),2);Cyc_Warn_err2(_T60,_T61);}}{int _TC2=0;_npop_handler(0);return _TC2;}};}_pop_handler();goto _TL79;_TL78: _T62=Cyc_Core_get_exn_thrown();{void*_TBD=(void*)_T62;void*_TBE;_T63=(struct Cyc_Binding_BindingError_exn_struct*)_TBD;_T64=_T63->tag;_T65=Cyc_Binding_BindingError;if(_T64!=_T65)goto _TL82;_T66=e1;_T67=_T66->loc;_T68=env;_T69=e1;
# 403
Cyc_Binding_interproc_binding_handler(_T67,_T68,_T69);
return 0;_TL82: _TBE=_TBD;{void*exn=_TBE;_rethrow(exn);};}_TL79:;}}goto _TL75;_TL74:
# 406
 return 0;_TL75:;}}goto _TL73;_TL72:
# 408
 Cyc_Binding_resolve_exp(env,e1);return 0;_TL73:;}}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TB0;_T6A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TB0;_T6B=& _T6A->f1;_TB6=(struct _tuple0**)_T6B;_TB5=_TB7->f2;_TB3=_TB7->f3;_T6C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TB0;_T6D=& _T6C->f4;_TB4=(struct Cyc_Absyn_Aggrdecl**)_T6D;}{struct _tuple0**tn=_TB6;struct Cyc_List_List*ts=_TB5;struct Cyc_List_List*dles=_TB3;struct Cyc_Absyn_Aggrdecl**adopt=(struct Cyc_Absyn_Aggrdecl**)_TB4;struct _handler_cons _TB7;_T6E=& _TB7;_push_handler(_T6E);{int _TB8=0;_T6F=_TB7.handler;_T70=setjmp(_T6F);if(!_T70)goto _TL84;_TB8=1;goto _TL85;_TL84: _TL85: if(_TB8)goto _TL86;else{goto _TL88;}_TL88: _T71=adopt;{
# 412
struct Cyc_Absyn_Aggrdecl*(*_TB9)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T72=_TB9;}_T73=e;_T74=_T73->loc;_T75=env;_T76=_T75->ns;_T77=tn;_T78=*_T77;*_T71=_T72(_T74,_T76,_T78,Cyc_Binding_lookup_aggrdecl);_T79=tn;_T7A=adopt;_T7B=*_T7A;_T7C=
_check_null(_T7B);*_T79=_T7C->name;{int _TB9=1;_npop_handler(0);return _TB9;}_pop_handler();goto _TL87;_TL86: _T7D=Cyc_Core_get_exn_thrown();{void*_TB9=(void*)_T7D;void*_TBA;_T7E=(struct Cyc_Binding_BindingError_exn_struct*)_TB9;_T7F=_T7E->tag;_T80=Cyc_Binding_BindingError;if(_T7F!=_T80)goto _TL89;{struct Cyc_Warn_String_Warn_Warg_struct _TBB;_TBB.tag=0;
# 416
_TBB.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_T81=_TBB;}{struct Cyc_Warn_String_Warn_Warg_struct _TBB=_T81;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBC;_TBC.tag=1;_T83=tn;_TBC.f1=*_T83;_T82=_TBC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBC=_T82;void*_TBD[2];_TBD[0]=& _TBB;_TBD[1]=& _TBC;_T84=e;_T85=_T84->loc;_T86=_tag_fat(_TBD,sizeof(void*),2);Cyc_Warn_err2(_T85,_T86);}}
return 1;_TL89: _TBA=_TB9;{void*exn=_TBA;_rethrow(exn);};}_TL87:;}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TB0;_TB6=_TB7->f1;_TB2=_TB7->f2;_TB1=_TB7->f3;}{struct Cyc_Absyn_Vardecl*vd=_TB6;struct Cyc_Absyn_Exp*e1=_TB2;struct Cyc_Absyn_Exp*e2=_TB1;
# 420
Cyc_Binding_resolve_exp(env,e1);_T87=
Cyc_Binding_at_toplevel(env);if(!_T87)goto _TL8B;_T88=env;{struct Cyc_Dict_Dict*_TB7=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T8B=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_TB8)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T8B;_T8A=_TB8;}_T8C=Cyc_strptrcmp;*_TB7=_T8A(_T8C);_T89=(struct Cyc_Dict_Dict*)_TB7;}_T88->local_vars=_T89;_T8D=e;_T8E=_T8D->loc;_T8F=env;_T90=vd;
Cyc_Binding_resolve_and_add_vardecl(_T8E,_T8F,_T90);
Cyc_Binding_resolve_exp(env,e2);_T91=env;
_T91->local_vars=0;
return 0;_TL8B: _T92=env;_T93=_T92->local_vars;_T94=
# 428
_check_null(_T93);{struct Cyc_Dict_Dict old_locals=*_T94;_T95=e;_T96=_T95->loc;_T97=env;_T98=vd;
Cyc_Binding_resolve_and_add_vardecl(_T96,_T97,_T98);
Cyc_Binding_resolve_exp(env,e2);_T99=env;_T9A=_T99->local_vars;_T9B=
_check_null(_T9A);*_T9B=old_locals;
return 0;}}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TB0;_TB6=_TB7->f1;_TB5=_TB7->f2;}{struct Cyc_Absyn_Exp*e1=_TB6;struct Cyc_List_List*ts=_TB5;
# 435
_TL90: if(ts!=0)goto _TL8E;else{goto _TL8F;}
_TL8E: _T9C=e;_T9D=_T9C->loc;_T9E=env;_T9F=ts;_TA0=_T9F->hd;Cyc_Binding_resolve_type(_T9D,_T9E,_TA0);_TA1=ts;
# 435
ts=_TA1->tl;goto _TL90;_TL8F:
# 437
 return 1;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_TB0;_TA2=_TB7->f1;_TB6=_TA2.elt_type;}{void**to=_TB6;
# 439
if(to==0)goto _TL91;_TA3=to;_TB6=*_TA3;goto _LLE;_TL91: return 1;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_TB0;_TA4=_TB7->f1;_TB6=(void*)_TA4;}_LLE: {void*topt=_TB6;
# 441
if(topt!=0)goto _TL93;return 1;_TL93: _TB6=topt;goto _LL10;}case 32:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TB0;_TA5=_TB7->f1;_TB6=(void*)_TA5;}_LL10: {void*t=_TB6;_TB6=t;goto _LL12;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_TB0;_TA6=_TB7->f1;{struct _tuple8 _TB8=*_TA6;_TB6=_TB8.f2;}}_LL12: {void*t=_TB6;_TB6=t;goto _LL14;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_TB0;_TA7=_TB7->f1;_TB6=(void*)_TA7;}_LL14: {void*t=_TB6;_TB6=t;goto _LL16;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_TB0;_TA8=_TB7->f1;_TB6=(void*)_TA8;}_LL16: {void*t=_TB6;_TB6=t;goto _LL18;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_TB0;_TA9=_TB7->f1;_TB6=(void*)_TA9;}_LL18: {void*t=_TB6;_TB6=t;goto _LL1A;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TB0;_TAA=_TB7->f2;_TB6=(void*)_TAA;}_LL1A: {void*t=_TB6;_TB6=t;goto _LL1C;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TB7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TB0;_TAB=_TB7->f1;_TB6=(void*)_TAB;}_LL1C: {void*t=_TB6;_TAC=e;_TAD=_TAC->loc;_TAE=env;_TAF=t;
# 450
Cyc_Binding_resolve_type(_TAD,_TAE,_TAF);
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
static struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;struct _tuple0*_T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_VarargInfo*_T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;unsigned _TB;struct _fat_ptr _TC;struct _tuple0*_TD;void*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;
# 512
struct Cyc_List_List*vds=0;
_TLA7: if(args!=0)goto _TLA5;else{goto _TLA6;}
_TLA5: _T0=args;_T1=_T0->hd;{struct _tuple8*_T11=(struct _tuple8*)_T1;void*_T12;struct Cyc_Absyn_Tqual _T13;struct _fat_ptr*_T14;{struct _tuple8 _T15=*_T11;_T14=_T15.f0;_T13=_T15.f1;_T12=_T15.f2;}{struct _fat_ptr*a=_T14;struct Cyc_Absyn_Tqual tq=_T13;void*t=_T12;
if(a!=0)goto _TLA8;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));
_T15->hd=0;_T15->tl=vds;_T2=(struct Cyc_List_List*)_T15;}vds=_T2;goto _TLA4;_TLA8:{struct _tuple0*_T15=_cycalloc(sizeof(struct _tuple0));
# 519
_T15->f0=Cyc_Absyn_Loc_n();_T15->f1=a;_T3=(struct _tuple0*)_T15;}_T4=t;{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_T3,_T4,0,0);_T5=vd;
_T5->tq=tq;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));
_T15->hd=vd;_T15->tl=vds;_T6=(struct Cyc_List_List*)_T15;}vds=_T6;}}}_TLA4: _T7=args;
# 513
args=_T7->tl;goto _TLA7;_TLA6:
# 523
 if(vai==0)goto _TLAA;_T8=vai;{
struct Cyc_Absyn_VarargInfo _T11=*_T8;int _T12;void*_T13;struct Cyc_Absyn_Tqual _T14;struct _fat_ptr*_T15;_T15=_T11.name;_T14=_T11.tq;_T13=_T11.type;_T12=_T11.inject;{struct _fat_ptr*v=_T15;struct Cyc_Absyn_Tqual tq=_T14;void*t=_T13;int i=_T12;
if(v!=0)goto _TLAC;_T9=need_va_name;
if(!_T9)goto _TLAE;{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;
_T16.f1=_tag_fat("missing name for vararg",sizeof(char),24U);_TA=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_TA;void*_T17[1];_T17[0]=& _T16;_TB=loc;_TC=_tag_fat(_T17,sizeof(void*),1);Cyc_Warn_err2(_TB,_TC);}goto _TLAF;_TLAE: _TLAF: goto _TLAD;
# 530
_TLAC:{void*typ=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);{struct _tuple0*_T16=_cycalloc(sizeof(struct _tuple0));
_T16->f0=Cyc_Absyn_Loc_n();_T16->f1=v;_TD=(struct _tuple0*)_T16;}_TE=typ;{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_TD,_TE,0,0);{struct Cyc_List_List*_T16=_cycalloc(sizeof(struct Cyc_List_List));
_T16->hd=vd;_T16->tl=vds;_TF=(struct Cyc_List_List*)_T16;}vds=_TF;}}_TLAD:;}}goto _TLAB;_TLAA: _TLAB:
# 535
 vds=Cyc_List_imp_rev(vds);_T10=vds;
return _T10;}
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
static void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD;union Cyc_Absyn_DatatypeInfo*_TE;unsigned _TF;struct Cyc_Binding_Env*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_List_List*_T13;union Cyc_Absyn_DatatypeInfo*_T14;struct _union_DatatypeInfo_UnknownDatatype _T15;unsigned _T16;struct _union_DatatypeInfo_UnknownDatatype _T17;struct Cyc_Absyn_UnknownDatatypeInfo _T18;struct _union_DatatypeInfo_UnknownDatatype _T19;struct Cyc_Absyn_UnknownDatatypeInfo _T1A;struct _handler_cons*_T1B;int*_T1C;int _T1D;struct Cyc_List_List*(*_T1E)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T1F;struct Cyc_Binding_Env*_T20;struct Cyc_Binding_NSCtxt*_T21;struct _tuple0*_T22;struct Cyc_List_List*_T23;void*_T24;union Cyc_Absyn_DatatypeInfo*_T25;struct Cyc_Absyn_UnknownDatatypeInfo _T26;struct Cyc_Absyn_Datatypedecl*_T27;void*_T28;struct Cyc_Binding_BindingError_exn_struct*_T29;char*_T2A;char*_T2B;void*_T2C;void*_T2D;void*_T2E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2F;void*_T30;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T31;union Cyc_Absyn_DatatypeFieldInfo*_T32;unsigned _T33;struct Cyc_Binding_Env*_T34;struct Cyc_List_List*_T35;void*_T36;struct Cyc_List_List*_T37;union Cyc_Absyn_DatatypeFieldInfo*_T38;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T39;unsigned _T3A;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3C;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3D;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3E;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3F;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T40;struct _handler_cons*_T41;int*_T42;int _T43;int*_T44;int _T45;struct Cyc_List_List*(*_T46)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T47;struct Cyc_Binding_Env*_T48;struct Cyc_Binding_NSCtxt*_T49;struct _tuple0*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_Absyn_Datatypedecl*_T4D;struct _tuple0*_T4E;struct Cyc_Absyn_Datatypedecl*_T4F;struct _tuple0*_T50;int _T51;struct Cyc_Warn_Qvar_Warn_Warg_struct _T52;struct Cyc_Absyn_Datatypefield*_T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;struct Cyc_Warn_Qvar_Warn_Warg_struct _T55;struct Cyc_Absyn_Datatypedecl*_T56;struct Cyc_Warn_String_Warn_Warg_struct _T57;struct Cyc_Warn_Qvar_Warn_Warg_struct _T58;struct Cyc_Absyn_Datatypedecl*_T59;struct Cyc_Warn_String_Warn_Warg_struct _T5A;unsigned _T5B;struct _fat_ptr _T5C;union Cyc_Absyn_DatatypeFieldInfo*_T5D;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T5E;struct Cyc_Absyn_Datatypedecl*_T5F;struct Cyc_Absyn_Datatypefield*_T60;void*_T61;struct Cyc_Binding_BindingError_exn_struct*_T62;char*_T63;char*_T64;struct Cyc_Warn_Qvar_Warn_Warg_struct _T65;struct Cyc_Warn_String_Warn_Warg_struct _T66;unsigned _T67;struct _fat_ptr _T68;void*_T69;void*_T6A;void*_T6B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6C;void*_T6D;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6E;union Cyc_Absyn_AggrInfo*_T6F;unsigned _T70;struct Cyc_Binding_Env*_T71;struct Cyc_List_List*_T72;void*_T73;struct Cyc_List_List*_T74;union Cyc_Absyn_AggrInfo*_T75;struct _union_AggrInfo_UnknownAggr _T76;unsigned _T77;struct _union_AggrInfo_UnknownAggr _T78;struct _tuple2 _T79;struct _union_AggrInfo_UnknownAggr _T7A;struct _tuple2 _T7B;struct _union_AggrInfo_UnknownAggr _T7C;struct _tuple2 _T7D;struct _handler_cons*_T7E;int*_T7F;int _T80;struct Cyc_Absyn_Aggrdecl*(*_T81)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T82;struct Cyc_Binding_Env*_T83;struct Cyc_Binding_NSCtxt*_T84;struct _tuple0*_T85;struct Cyc_Absyn_Aggrdecl*_T86;enum Cyc_Absyn_AggrKind _T87;int _T88;enum Cyc_Absyn_AggrKind _T89;int _T8A;struct Cyc_Warn_String_Warn_Warg_struct _T8B;unsigned _T8C;struct _fat_ptr _T8D;enum Cyc_Absyn_AggrKind _T8E;int _T8F;struct Cyc_Absyn_Aggrdecl*_T90;struct Cyc_Absyn_AggrdeclImpl*_T91;struct Cyc_Absyn_Aggrdecl*_T92;struct Cyc_Absyn_AggrdeclImpl*_T93;int _T94;struct Cyc_Core_Opt*_T95;void*_T96;int _T97;struct Cyc_Warn_String_Warn_Warg_struct _T98;unsigned _T99;struct _fat_ptr _T9A;union Cyc_Absyn_AggrInfo*_T9B;enum Cyc_Absyn_AggrKind _T9C;struct Cyc_Absyn_Aggrdecl*_T9D;struct _tuple0*_T9E;struct Cyc_Core_Opt*_T9F;void*_TA0;struct Cyc_Binding_BindingError_exn_struct*_TA1;char*_TA2;char*_TA3;void*_TA4;void*_TA5;void*_TA6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7;void*_TA8;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TA9;struct _tuple0**_TAA;struct _handler_cons*_TAB;int*_TAC;int _TAD;struct Cyc_Absyn_Enumdecl*(*_TAE)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TAF;struct Cyc_Binding_Env*_TB0;struct Cyc_Binding_NSCtxt*_TB1;struct _tuple0**_TB2;struct _tuple0*_TB3;struct _tuple0**_TB4;struct Cyc_Absyn_Enumdecl*_TB5;void*_TB6;struct Cyc_Binding_BindingError_exn_struct*_TB7;char*_TB8;char*_TB9;unsigned _TBA;struct Cyc_Binding_Env*_TBB;struct _tuple0**_TBC;struct _tuple0*_TBD;void*_TBE;void*_TBF;struct Cyc_Binding_ResolveNSEnv*(*_TC0)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TC1;struct Cyc_Binding_NSCtxt*_TC2;struct Cyc_Binding_Env*_TC3;struct Cyc_Binding_NSCtxt*_TC4;struct Cyc_List_List*_TC5;int _TC6;union Cyc_Absyn_Nmspace _TC7;struct Cyc_List_List*_TC8;void*_TC9;struct Cyc_Absyn_Enumfield*_TCA;struct Cyc_Absyn_Exp*_TCB;struct Cyc_Binding_Env*_TCC;struct Cyc_Absyn_Enumfield*_TCD;struct Cyc_Absyn_Exp*_TCE;struct Cyc_Binding_ResolveNSEnv*_TCF;struct Cyc_Dict_Dict(*_TD0)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TD1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TD2;struct Cyc_Dict_Dict _TD3;struct Cyc_Absyn_Enumfield*_TD4;struct _tuple0*_TD5;struct _tuple0 _TD6;struct _fat_ptr*_TD7;struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TD8;void*_TD9;struct Cyc_List_List*_TDA;void*_TDB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDC;struct Cyc_List_List*_TDD;void*_TDE;void*_TDF;unsigned _TE0;struct Cyc_Binding_Env*_TE1;struct Cyc_List_List*_TE2;void*_TE3;struct Cyc_List_List*_TE4;void*_TE5;void*_TE6;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TE7;struct _tuple0**_TE8;void*_TE9;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TEA;struct Cyc_Absyn_Typedefdecl**_TEB;unsigned _TEC;struct Cyc_Binding_Env*_TED;struct Cyc_List_List*_TEE;void*_TEF;struct Cyc_List_List*_TF0;struct _handler_cons*_TF1;int*_TF2;int _TF3;struct Cyc_Absyn_Typedefdecl*(*_TF4)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TF5;struct Cyc_Binding_Env*_TF6;struct Cyc_Binding_NSCtxt*_TF7;struct _tuple0**_TF8;struct _tuple0*_TF9;struct _tuple0**_TFA;struct Cyc_Absyn_Typedefdecl*_TFB;void*_TFC;struct Cyc_Binding_BindingError_exn_struct*_TFD;char*_TFE;char*_TFF;struct Cyc_Warn_String_Warn_Warg_struct _T100;struct Cyc_Warn_Qvar_Warn_Warg_struct _T101;struct _tuple0**_T102;unsigned _T103;struct _fat_ptr _T104;void*_T105;void*_T106;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T107;void***_T108;struct Cyc_Binding_Env*_T109;struct Cyc_Binding_Env*_T10A;struct Cyc_Absyn_TypeDecl*_T10B;int*_T10C;unsigned _T10D;struct Cyc_Binding_Env*_T10E;struct Cyc_Absyn_Decl*_T10F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T110;struct Cyc_Absyn_TypeDecl*_T111;struct Cyc_Binding_Env*_T112;struct Cyc_Absyn_Decl*_T113;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T114;struct Cyc_Absyn_TypeDecl*_T115;struct Cyc_Binding_Env*_T116;struct Cyc_Absyn_Decl*_T117;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T118;struct Cyc_Absyn_TypeDecl*_T119;struct Cyc_Binding_Env*_T11A;void*_T11B;void*_T11C;void*_T11D;int _T11E;void*_T11F;void*_T120;void*_T121;struct Cyc_Absyn_PtrInfo _T122;struct Cyc_Absyn_PtrInfo _T123;struct Cyc_Absyn_PtrAtts _T124;struct Cyc_Absyn_PtrInfo _T125;struct Cyc_Absyn_PtrAtts _T126;void*_T127;struct Cyc_Absyn_ArrayInfo _T128;struct Cyc_Absyn_ArrayInfo _T129;void*_T12A;void*_T12B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T12C;struct Cyc_Absyn_FnInfo*_T12D;unsigned _T12E;struct Cyc_Binding_Env*_T12F;struct Cyc_Absyn_FnInfo*_T130;struct Cyc_List_List*_T131;struct Cyc_Absyn_FnInfo*_T132;struct Cyc_Absyn_VarargInfo*_T133;struct Cyc_Absyn_FnInfo*_T134;struct Cyc_Binding_Env*_T135;struct Cyc_Binding_Env*_T136;struct Cyc_Dict_Dict*_T137;struct Cyc_Binding_Env*_T138;struct Cyc_Dict_Dict*_T139;struct Cyc_Binding_Env*_T13A;struct Cyc_Dict_Dict*_T13B;struct Cyc_Dict_Dict(*_T13C)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T13D)(int(*)(void*,void*));int(*_T13E)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T13F;void*_T140;struct Cyc_Absyn_Vardecl*_T141;struct Cyc_Binding_Env*_T142;struct Cyc_Dict_Dict*_T143;struct Cyc_Dict_Dict*_T144;struct Cyc_Dict_Dict(*_T145)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T146)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T147;struct Cyc_Dict_Dict*_T148;struct Cyc_Dict_Dict*_T149;struct Cyc_Dict_Dict _T14A;struct Cyc_List_List*_T14B;void*_T14C;struct Cyc_Absyn_Vardecl*_T14D;struct _tuple0*_T14E;struct _fat_ptr*_T14F;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T150;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T151;struct Cyc_List_List*_T152;void*_T153;void*_T154;struct Cyc_List_List*_T155;struct Cyc_Binding_Env*_T156;void*_T157;void*_T158;struct Cyc_Absyn_Exp*_T159;void*_T15A;void*_T15B;void*_T15C;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 22: _T8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T9=_T15D->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T15E=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T9;_TA=t;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC;_TE=& _TD->f1;_T15C=(union Cyc_Absyn_DatatypeInfo*)_TE;}_T15B=_T15D->f2;}{union Cyc_Absyn_DatatypeInfo*info=_T15C;struct Cyc_List_List*targs=_T15B;
# 580
_TLC9: if(targs!=0)goto _TLC7;else{goto _TLC8;}
_TLC7: _TF=loc;_T10=env;_T11=targs;_T12=_T11->hd;Cyc_Binding_resolve_type(_TF,_T10,_T12);_T13=targs;
# 580
targs=_T13->tl;goto _TLC9;_TLC8: _T14=info;{
# 582
union Cyc_Absyn_DatatypeInfo _T15D=*_T14;int _T15E;struct _tuple0*_T15F;_T15=_T15D.UnknownDatatype;_T16=_T15.tag;if(_T16!=1)goto _TLCA;_T17=_T15D.UnknownDatatype;_T18=_T17.val;_T15F=_T18.name;_T19=_T15D.UnknownDatatype;_T1A=_T19.val;_T15E=_T1A.is_extensible;{struct _tuple0*qv=_T15F;int b=_T15E;struct _handler_cons _T160;_T1B=& _T160;_push_handler(_T1B);{int _T161=0;_T1C=_T160.handler;_T1D=setjmp(_T1C);if(!_T1D)goto _TLCC;_T161=1;goto _TLCD;_TLCC: _TLCD: if(_T161)goto _TLCE;else{goto _TLD0;}_TLD0:{
# 585
struct Cyc_List_List*(*_T162)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T1E=_T162;}_T1F=loc;_T20=env;_T21=_T20->ns;_T22=qv;_T23=_T1E(_T1F,_T21,_T22,Cyc_Binding_lookup_datatypedecl);_T24=_T23->hd;{struct Cyc_Absyn_Datatypedecl*tud=(struct Cyc_Absyn_Datatypedecl*)_T24;_T25=info;{struct Cyc_Absyn_UnknownDatatypeInfo _T162;_T27=tud;
_T162.name=_T27->name;_T162.is_extensible=b;_T26=_T162;}*_T25=Cyc_Absyn_UnknownDatatype(_T26);_npop_handler(0);return;}_pop_handler();goto _TLCF;_TLCE: _T28=Cyc_Core_get_exn_thrown();{void*_T162=(void*)_T28;void*_T163;_T29=(struct Cyc_Binding_BindingError_exn_struct*)_T162;_T2A=_T29->tag;_T2B=Cyc_Binding_BindingError;if(_T2A!=_T2B)goto _TLD1;
# 589
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLD1: _T163=_T162;{void*exn=_T163;_rethrow(exn);};}_TLCF:;}}goto _TLCB;_TLCA:
 return;_TLCB:;}}case 23: _T2C=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2C;_T2D=_T15D->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T15E=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T2D;_T2E=t;_T2F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2E;_T30=_T2F->f1;_T31=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T30;_T32=& _T31->f1;_T15C=(union Cyc_Absyn_DatatypeFieldInfo*)_T32;}_T15B=_T15D->f2;}{union Cyc_Absyn_DatatypeFieldInfo*info=_T15C;struct Cyc_List_List*targs=_T15B;
# 593
_TLD6: if(targs!=0)goto _TLD4;else{goto _TLD5;}
_TLD4: _T33=loc;_T34=env;_T35=targs;_T36=_T35->hd;Cyc_Binding_resolve_type(_T33,_T34,_T36);_T37=targs;
# 593
targs=_T37->tl;goto _TLD6;_TLD5: _T38=info;{
# 595
union Cyc_Absyn_DatatypeFieldInfo _T15D=*_T38;int _T15E;struct _tuple0*_T15F;struct _tuple0*_T160;_T39=_T15D.UnknownDatatypefield;_T3A=_T39.tag;if(_T3A!=1)goto _TLD7;_T3B=_T15D.UnknownDatatypefield;_T3C=_T3B.val;_T160=_T3C.datatype_name;_T3D=_T15D.UnknownDatatypefield;_T3E=_T3D.val;_T15F=_T3E.field_name;_T3F=_T15D.UnknownDatatypefield;_T40=_T3F.val;_T15E=_T40.is_extensible;{struct _tuple0*qvd=_T160;struct _tuple0*qvf=_T15F;int b=_T15E;{struct _handler_cons _T161;_T41=& _T161;_push_handler(_T41);{int _T162=0;_T42=_T161.handler;_T43=setjmp(_T42);if(!_T43)goto _TLD9;_T162=1;goto _TLDA;_TLD9: _TLDA: if(_T162)goto _TLDB;else{goto _TLDD;}_TLDD:{
# 600
void*_T163=Cyc_Binding_lookup_ordinary(loc,env,qvf);struct Cyc_Absyn_Datatypefield*_T164;struct Cyc_Absyn_Datatypedecl*_T165;_T44=(int*)_T163;_T45=*_T44;if(_T45!=2)goto _TLDE;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T166=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T163;_T165=_T166->f1;_T164=_T166->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T165;struct Cyc_Absyn_Datatypefield*tuf=_T164;{
# 602
struct Cyc_List_List*(*_T166)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T46=_T166;}_T47=loc;_T48=env;_T49=_T48->ns;_T4A=qvd;_T4B=_T46(_T47,_T49,_T4A,Cyc_Binding_lookup_datatypedecl);_T4C=_T4B->hd;{struct Cyc_Absyn_Datatypedecl*tud2=(struct Cyc_Absyn_Datatypedecl*)_T4C;_T4D=tud;_T4E=_T4D->name;_T4F=tud2;_T50=_T4F->name;_T51=
Cyc_Absyn_qvar_cmp(_T4E,_T50);if(_T51==0)goto _TLE0;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T166;_T166.tag=1;_T53=tuf;
_T166.f1=_T53->name;_T52=_T166;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T166=_T52;{struct Cyc_Warn_String_Warn_Warg_struct _T167;_T167.tag=0;_T167.f1=_tag_fat(" is a variant of ",sizeof(char),18U);_T54=_T167;}{struct Cyc_Warn_String_Warn_Warg_struct _T167=_T54;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T168;_T168.tag=1;_T56=tud2;_T168.f1=_T56->name;_T55=_T168;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T168=_T55;{struct Cyc_Warn_String_Warn_Warg_struct _T169;_T169.tag=0;_T169.f1=_tag_fat(" not ",sizeof(char),6U);_T57=_T169;}{struct Cyc_Warn_String_Warn_Warg_struct _T169=_T57;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16A;_T16A.tag=1;_T59=tud;
_T16A.f1=_T59->name;_T58=_T16A;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T16A=_T58;{struct Cyc_Warn_String_Warn_Warg_struct _T16B;_T16B.tag=0;_T16B.f1=_tag_fat(" (shadowing not yet implemented properly)",sizeof(char),42U);_T5A=_T16B;}{struct Cyc_Warn_String_Warn_Warg_struct _T16B=_T5A;void*_T16C[6];_T16C[0]=& _T166;_T16C[1]=& _T167;_T16C[2]=& _T168;_T16C[3]=& _T169;_T16C[4]=& _T16A;_T16C[5]=& _T16B;_T5B=loc;_T5C=_tag_fat(_T16C,sizeof(void*),6);Cyc_Warn_err2(_T5B,_T5C);}}}}}}_npop_handler(0);return;_TLE0: _T5D=info;{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T166;_T5F=tud;
# 608
_T166.datatype_name=_T5F->name;_T60=tuf;
_T166.field_name=_T60->name;_T166.is_extensible=b;_T5E=_T166;}
# 608
*_T5D=Cyc_Absyn_UnknownDatatypefield(_T5E);_npop_handler(0);return;}}_TLDE: goto _LL34;_LL34:;}_pop_handler();goto _TLDC;_TLDB: _T61=Cyc_Core_get_exn_thrown();{void*_T163=(void*)_T61;void*_T164;_T62=(struct Cyc_Binding_BindingError_exn_struct*)_T163;_T63=_T62->tag;_T64=Cyc_Binding_BindingError;if(_T63!=_T64)goto _TLE2;goto _LL39;_TLE2: _T164=_T163;{void*exn=_T164;_rethrow(exn);}_LL39:;}_TLDC:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T161;_T161.tag=1;
# 614
_T161.f1=qvf;_T65=_T161;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T161=_T65;{struct Cyc_Warn_String_Warn_Warg_struct _T162;_T162.tag=0;_T162.f1=_tag_fat(" is not a datatype field",sizeof(char),25U);_T66=_T162;}{struct Cyc_Warn_String_Warn_Warg_struct _T162=_T66;void*_T163[2];_T163[0]=& _T161;_T163[1]=& _T162;_T67=loc;_T68=_tag_fat(_T163,sizeof(void*),2);Cyc_Warn_err2(_T67,_T68);}}return;}_TLD7:
 return;;}}case 24: _T69=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T69;_T6A=_T15D->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T15E=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6A;_T6B=t;_T6C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6B;_T6D=_T6C->f1;_T6E=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6D;_T6F=& _T6E->f1;_T15C=(union Cyc_Absyn_AggrInfo*)_T6F;}_T15B=_T15D->f2;}{union Cyc_Absyn_AggrInfo*info=_T15C;struct Cyc_List_List*targs=_T15B;
# 619
_TLE7: if(targs!=0)goto _TLE5;else{goto _TLE6;}
_TLE5: _T70=loc;_T71=env;_T72=targs;_T73=_T72->hd;Cyc_Binding_resolve_type(_T70,_T71,_T73);_T74=targs;
# 619
targs=_T74->tl;goto _TLE7;_TLE6: _T75=info;{
# 621
union Cyc_Absyn_AggrInfo _T15D=*_T75;struct Cyc_Core_Opt*_T15E;struct _tuple0*_T15F;enum Cyc_Absyn_AggrKind _T160;_T76=_T15D.UnknownAggr;_T77=_T76.tag;if(_T77!=1)goto _TLE8;_T78=_T15D.UnknownAggr;_T79=_T78.val;_T160=_T79.f0;_T7A=_T15D.UnknownAggr;_T7B=_T7A.val;_T15F=_T7B.f1;_T7C=_T15D.UnknownAggr;_T7D=_T7C.val;_T15E=_T7D.f2;{enum Cyc_Absyn_AggrKind ak=_T160;struct _tuple0*qv=_T15F;struct Cyc_Core_Opt*bo=_T15E;struct _handler_cons _T161;_T7E=& _T161;_push_handler(_T7E);{int _T162=0;_T7F=_T161.handler;_T80=setjmp(_T7F);if(!_T80)goto _TLEA;_T162=1;goto _TLEB;_TLEA: _TLEB: if(_T162)goto _TLEC;else{goto _TLEE;}_TLEE:{
# 624
struct Cyc_Absyn_Aggrdecl*(*_T163)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T81=_T163;}_T82=loc;_T83=env;_T84=_T83->ns;_T85=qv;{struct Cyc_Absyn_Aggrdecl*ad=_T81(_T82,_T84,_T85,Cyc_Binding_lookup_aggrdecl);_T86=ad;_T87=_T86->kind;_T88=(int)_T87;_T89=ak;_T8A=(int)_T89;
if(_T88==_T8A)goto _TLEF;{struct Cyc_Warn_String_Warn_Warg_struct _T163;_T163.tag=0;
_T163.f1=_tag_fat("struct vs. union mismatch with earlier declaration",sizeof(char),51U);_T8B=_T163;}{struct Cyc_Warn_String_Warn_Warg_struct _T163=_T8B;void*_T164[1];_T164[0]=& _T163;_T8C=loc;_T8D=_tag_fat(_T164,sizeof(void*),1);Cyc_Warn_err2(_T8C,_T8D);}goto _TLF0;_TLEF: _TLF0: _T8E=ak;_T8F=(int)_T8E;
if(_T8F!=1)goto _TLF1;if(bo==0)goto _TLF1;_T90=ad;_T91=_T90->impl;if(_T91==0)goto _TLF1;_T92=ad;_T93=_T92->impl;_T94=_T93->tagged;_T95=bo;_T96=_T95->v;_T97=(int)_T96;if(_T94==_T97)goto _TLF1;{struct Cyc_Warn_String_Warn_Warg_struct _T163;_T163.tag=0;
# 629
_T163.f1=_tag_fat("@tagged mismatch with earlier declaration",sizeof(char),42U);_T98=_T163;}{struct Cyc_Warn_String_Warn_Warg_struct _T163=_T98;void*_T164[1];_T164[0]=& _T163;_T99=loc;_T9A=_tag_fat(_T164,sizeof(void*),1);Cyc_Warn_err2(_T99,_T9A);}goto _TLF2;_TLF1: _TLF2: _T9B=info;_T9C=ak;_T9D=ad;_T9E=_T9D->name;_T9F=bo;
*_T9B=Cyc_Absyn_UnknownAggr(_T9C,_T9E,_T9F);_npop_handler(0);return;}_pop_handler();goto _TLED;_TLEC: _TA0=Cyc_Core_get_exn_thrown();{void*_T163=(void*)_TA0;void*_T164;_TA1=(struct Cyc_Binding_BindingError_exn_struct*)_T163;_TA2=_TA1->tag;_TA3=Cyc_Binding_BindingError;if(_TA2!=_TA3)goto _TLF3;
# 633
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLF3: _T164=_T163;{void*exn=_T164;_rethrow(exn);};}_TLED:;}}goto _TLE9;_TLE8:
 return;_TLE9:;}}case 19: _TA4=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA4;_TA5=_T15D->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T15E=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TA5;_TA6=t;_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA6;_TA8=_TA7->f1;_TA9=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TA8;_TAA=& _TA9->f1;_T15C=(struct _tuple0**)_TAA;_T15B=_T15E->f2;}}{struct _tuple0**qv=_T15C;struct Cyc_Absyn_Enumdecl*edo=_T15B;
# 637
if(edo==0)goto _TLF5;
return;_TLF5: {struct _handler_cons _T15D;_TAB=& _T15D;_push_handler(_TAB);{int _T15E=0;_TAC=_T15D.handler;_TAD=setjmp(_TAC);if(!_TAD)goto _TLF7;_T15E=1;goto _TLF8;_TLF7: _TLF8: if(_T15E)goto _TLF9;else{goto _TLFB;}_TLFB:{
# 640
struct Cyc_Absyn_Enumdecl*(*_T15F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TAE=_T15F;}_TAF=loc;_TB0=env;_TB1=_TB0->ns;_TB2=qv;_TB3=*_TB2;{struct Cyc_Absyn_Enumdecl*ed=_TAE(_TAF,_TB1,_TB3,Cyc_Binding_lookup_enumdecl);_TB4=qv;_TB5=ed;
*_TB4=_TB5->name;_npop_handler(0);return;}_pop_handler();goto _TLFA;_TLF9: _TB6=Cyc_Core_get_exn_thrown();{void*_T15F=(void*)_TB6;void*_T160;_TB7=(struct Cyc_Binding_BindingError_exn_struct*)_T15F;_TB8=_TB7->tag;_TB9=Cyc_Binding_BindingError;if(_TB8!=_TB9)goto _TLFC;_TBA=loc;_TBB=env;_TBC=qv;_TBD=*_TBC;
# 644
Cyc_Binding_absolutize_topdecl(_TBA,_TBB,_TBD,2U);return;_TLFC: _T160=_T15F;{void*exn=_T160;_rethrow(exn);};}_TLFA:;}}}case 20: _TBE=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TBE;_TBF=_T15D->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_T15E=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_TBF;_T15C=_T15E->f1;}}{struct Cyc_List_List*fs=_T15C;{
# 682
struct Cyc_Binding_ResolveNSEnv*(*_T15D)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TC0=_T15D;}_TC1=env;_TC2=_TC1->ns;_TC3=env;_TC4=_TC3->ns;_TC5=_TC4->curr_ns;_TC6=
Cyc_Binding_in_cinclude(env);_TC7=
# 682
Cyc_Absyn_Abs_n(_TC5,_TC6);{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TC0(_TC2,_TC7);
# 684
_TL101: if(fs!=0)goto _TLFF;else{goto _TL100;}
_TLFF: _TC8=fs;_TC9=_TC8->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TC9;_TCA=f;_TCB=_TCA->tag;
if(_TCB==0)goto _TL102;_TCC=env;_TCD=f;_TCE=_TCD->tag;
Cyc_Binding_resolve_exp(_TCC,_TCE);goto _TL103;_TL102: _TL103: _TCF=decl_ns_data;_TD1=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T15D)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TD1;_TD0=_T15D;}_TD2=decl_ns_data;_TD3=_TD2->ordinaries;_TD4=f;_TD5=_TD4->name;_TD6=*_TD5;_TD7=_TD6.f1;{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T15D=_cycalloc(sizeof(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct));_T15D->tag=4;_T15D->f1=t;_T15D->f2=f;_TD8=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T15D;}_TD9=(void*)_TD8;_TCF->ordinaries=_TD0(_TD3,_TD7,_TD9);}_TDA=fs;
# 684
fs=_TDA->tl;goto _TL101;_TL100:
# 690
 return;}}default: _TDB=t;_TDC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDB;_TDD=_TDC->f2;if(_TDD!=0)goto _TL104;_LL18:
# 700
 return;_TL104: _TDE=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDE;_TDF=_T15D->f1;_T15C=(void*)_TDF;_T15B=_T15D->f2;}{void*c=_T15C;struct Cyc_List_List*ts=_T15B;
# 702
_TL109: if(ts!=0)goto _TL107;else{goto _TL108;}
_TL107: _TE0=loc;_TE1=env;_TE2=ts;_TE3=_TE2->hd;Cyc_Binding_resolve_type(_TE0,_TE1,_TE3);_TE4=ts;
# 702
ts=_TE4->tl;goto _TL109;_TL108:
# 704
 return;}};case 8: _TE5=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE5;_TE6=t;_TE7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE6;_TE8=& _TE7->f1;_T15C=(struct _tuple0**)_TE8;_T15B=_T15D->f2;_TE9=t;_TEA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE9;_TEB=& _TEA->f3;_T15A=(struct Cyc_Absyn_Typedefdecl**)_TEB;}{struct _tuple0**tdn=_T15C;struct Cyc_List_List*targs=_T15B;struct Cyc_Absyn_Typedefdecl**tdo=_T15A;
# 647
_TL10D: if(targs!=0)goto _TL10B;else{goto _TL10C;}
_TL10B: _TEC=loc;_TED=env;_TEE=targs;_TEF=_TEE->hd;Cyc_Binding_resolve_type(_TEC,_TED,_TEF);_TF0=targs;
# 647
targs=_TF0->tl;goto _TL10D;_TL10C:{struct _handler_cons _T15D;_TF1=& _T15D;_push_handler(_TF1);{int _T15E=0;_TF2=_T15D.handler;_TF3=setjmp(_TF2);if(!_TF3)goto _TL10E;_T15E=1;goto _TL10F;_TL10E: _TL10F: if(_T15E)goto _TL110;else{goto _TL112;}_TL112:{
# 650
struct Cyc_Absyn_Typedefdecl*(*_T15F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TF4=_T15F;}_TF5=loc;_TF6=env;_TF7=_TF6->ns;_TF8=tdn;_TF9=*_TF8;{struct Cyc_Absyn_Typedefdecl*td=_TF4(_TF5,_TF7,_TF9,Cyc_Binding_lookup_typedefdecl);_TFA=tdn;_TFB=td;
# 652
*_TFA=_TFB->name;_npop_handler(0);return;}_pop_handler();goto _TL111;_TL110: _TFC=Cyc_Core_get_exn_thrown();{void*_T15F=(void*)_TFC;void*_T160;_TFD=(struct Cyc_Binding_BindingError_exn_struct*)_T15F;_TFE=_TFD->tag;_TFF=Cyc_Binding_BindingError;if(_TFE!=_TFF)goto _TL113;goto _LL4D;_TL113: _T160=_T15F;{void*exn=_T160;_rethrow(exn);}_LL4D:;}_TL111:;}}{struct Cyc_Warn_String_Warn_Warg_struct _T15D;_T15D.tag=0;
# 655
_T15D.f1=_tag_fat("unbound typdef name ",sizeof(char),21U);_T100=_T15D;}{struct Cyc_Warn_String_Warn_Warg_struct _T15D=_T100;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T15E;_T15E.tag=1;_T102=tdn;_T15E.f1=*_T102;_T101=_T15E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T15E=_T101;void*_T15F[2];_T15F[0]=& _T15D;_T15F[1]=& _T15E;_T103=loc;_T104=_tag_fat(_T15F,sizeof(void*),2);Cyc_Warn_err2(_T103,_T104);}}
return;}case 10: _T105=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T105;_T15C=_T15D->f1;_T106=t;_T107=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T106;_T108=& _T107->f2;_T15B=(void***)_T108;}{struct Cyc_Absyn_TypeDecl*td=_T15C;void***to=_T15B;_T109=env;{
# 660
struct Cyc_Dict_Dict*old_locals=_T109->local_vars;_T10A=env;
_T10A->local_vars=0;_T10B=td;{
void*_T15D=_T10B->r;struct Cyc_Absyn_Datatypedecl*_T15E;struct Cyc_Absyn_Enumdecl*_T15F;struct Cyc_Absyn_Aggrdecl*_T160;_T10C=(int*)_T15D;_T10D=*_T10C;switch(_T10D){case 0:{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T161=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T15D;_T160=_T161->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T160;_T10E=env;{struct Cyc_Absyn_Decl*_T161=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T162=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T162->tag=5;
# 664
_T162->f1=ad;_T110=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T162;}_T161->r=(void*)_T110;_T111=td;_T161->loc=_T111->loc;_T10F=(struct Cyc_Absyn_Decl*)_T161;}Cyc_Binding_resolve_decl(_T10E,_T10F);goto _LL52;}case 1:{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T161=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T15D;_T15F=_T161->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T15F;_T112=env;{struct Cyc_Absyn_Decl*_T161=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T162=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T162->tag=7;
# 666
_T162->f1=ed;_T114=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T162;}_T161->r=(void*)_T114;_T115=td;_T161->loc=_T115->loc;_T113=(struct Cyc_Absyn_Decl*)_T161;}Cyc_Binding_resolve_decl(_T112,_T113);goto _LL52;}default:{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T161=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T15D;_T15E=_T161->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T15E;_T116=env;{struct Cyc_Absyn_Decl*_T161=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T162=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T162->tag=6;
# 668
_T162->f1=tud;_T118=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T162;}_T161->r=(void*)_T118;_T119=td;_T161->loc=_T119->loc;_T117=(struct Cyc_Absyn_Decl*)_T161;}Cyc_Binding_resolve_decl(_T116,_T117);goto _LL52;}}_LL52:;}_T11A=env;
# 670
_T11A->local_vars=old_locals;
return;}}case 7: _T11B=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T11B;_T15C=_T15D->f3;}{struct Cyc_List_List*fs=_T15C;
# 677
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 3: _T11C=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T11C;_T11D=_T15D->f2;_T15C=(void*)_T11D;}{void*to=_T15C;_T11E=Cyc_Flags_resolve;
# 692
if(!_T11E)goto _TL116;
Cyc_BansheeIf_register_toplevel_cvar(t);goto _LL0;
# 697
_TL116: _T15C=to;goto _LL14;}case 1: _T11F=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T11F;_T120=_T15D->f2;_T15C=(void*)_T120;}_LL14: {void*to=_T15C;
if(to==0)goto _TL118;Cyc_Binding_resolve_type(loc,env,to);goto _TL119;_TL118: _TL119: return;}case 2: goto _LL18;case 4: _T121=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T121;_T122=_T15D->f1;_T15C=_T122.elt_type;_T123=_T15D->f1;_T124=_T123.ptr_atts;_T15B=_T124.eff;_T125=_T15D->f1;_T126=_T125.ptr_atts;_T15A=_T126.bounds;}{void*t1=_T15C;void*t2=_T15B;void*bds=_T15A;
# 707
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 5: _T127=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T127;_T128=_T15D->f1;_T15C=_T128.elt_type;_T129=_T15D->f1;_T159=_T129.num_elts;}{void*t1=_T15C;struct Cyc_Absyn_Exp*eo=_T159;
# 713
Cyc_Binding_resolve_type(loc,env,t1);
if(eo==0)goto _TL11A;Cyc_Binding_resolve_exp(env,eo);goto _TL11B;_TL11A: _TL11B:
 return;}case 6: _T12A=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T12A;_T12B=t;_T12C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T12B;_T12D=& _T12C->f1;_T15C=(struct Cyc_Absyn_FnInfo*)_T12D;}{struct Cyc_Absyn_FnInfo*i=(struct Cyc_Absyn_FnInfo*)_T15C;_T12E=loc;_T12F=env;_T130=i;_T131=_T130->args;_T132=i;_T133=_T132->cyc_varargs;{
# 720
struct Cyc_List_List*vds=Cyc_Binding_get_fun_vardecls(0,_T12E,_T12F,_T131,_T133);_T134=i;
_T134->arg_vardecls=vds;_T135=env;{
struct Cyc_Dict_Dict*old_locals=_T135->local_vars;
if(old_locals==0)goto _TL11C;_T136=env;{struct Cyc_Dict_Dict*_T15D=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T138=env;_T139=_T138->local_vars;
*_T15D=*_T139;_T137=(struct Cyc_Dict_Dict*)_T15D;}_T136->local_vars=_T137;goto _TL11D;
# 726
_TL11C: _T13A=env;{struct Cyc_Dict_Dict*_T15D=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T13D=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T15E)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T13D;_T13C=_T15E;}_T13E=Cyc_strptrcmp;*_T15D=_T13C(_T13E);_T13B=(struct Cyc_Dict_Dict*)_T15D;}_T13A->local_vars=_T13B;_TL11D:{
struct Cyc_List_List*vds1=vds;_TL121: if(vds1!=0)goto _TL11F;else{goto _TL120;}
_TL11F: _T13F=vds1;_T140=_T13F->hd;_T141=(struct Cyc_Absyn_Vardecl*)_T140;if(_T141==0)goto _TL122;_T142=env;_T143=_T142->local_vars;_T144=
_check_null(_T143);_T146=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T15D)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T146;_T145=_T15D;}_T147=env;_T148=_T147->local_vars;_T149=_check_null(_T148);_T14A=*_T149;_T14B=vds1;_T14C=_T14B->hd;_T14D=(struct Cyc_Absyn_Vardecl*)_T14C;_T14E=_T14D->name;_T14F=_T14E->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T15D=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T15D->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T15E=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T15E->tag=3;_T152=vds1;_T153=_T152->hd;
_T15E->f1=(struct Cyc_Absyn_Vardecl*)_T153;_T151=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T15E;}_T15D->f1=(void*)_T151;_T150=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T15D;}_T154=(void*)_T150;
# 729
*_T144=_T145(_T14A,_T14F,_T154);goto _TL123;_TL122: _TL123: _T155=vds1;
# 727
vds1=_T155->tl;goto _TL121;_TL120:;}
# 732
Cyc_Binding_resolve_function_stuff(loc,env,i);_T156=env;
_T156->local_vars=old_locals;
return;}}}case 9: _T157=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T157;_T159=_T15D->f1;}{struct Cyc_Absyn_Exp*e=_T159;_T159=e;goto _LL24;}default: _T158=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T15D=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T158;_T159=_T15D->f1;}_LL24: {struct Cyc_Absyn_Exp*e=_T159;
# 737
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple12{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 741
static void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;void*_T1;int*_T2;unsigned _T3;void*_T4;struct _handler_cons*_T5;int*_T6;int _T7;int*_T8;unsigned _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;unsigned _TB;struct _fat_ptr _TC;struct Cyc_Absyn_Pat*_TD;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TE;struct Cyc_Absyn_Pat*_TF;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T10;void*_T11;struct Cyc_Absyn_Pat*_T12;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T13;void*_T14;struct Cyc_Binding_BindingError_exn_struct*_T15;char*_T16;char*_T17;struct Cyc_Absyn_Pat*_T18;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T19;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1B;void*_T1C;void*_T1D;struct Cyc_Binding_Env*_T1E;struct Cyc_List_List*_T1F;void*_T20;struct Cyc_Absyn_Pat*_T21;struct Cyc_List_List*_T22;struct _handler_cons*_T23;int*_T24;int _T25;int*_T26;unsigned _T27;struct Cyc_List_List*_T28;struct _tuple12*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_List_List*_T2C;struct Cyc_Absyn_Aggrdecl**_T2D;union Cyc_Absyn_AggrInfo _T2E;struct Cyc_Absyn_Pat*_T2F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T30;struct Cyc_Absyn_Pat*_T31;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T32;struct Cyc_Warn_String_Warn_Warg_struct _T33;unsigned _T34;struct _fat_ptr _T35;void*_T36;struct Cyc_Binding_BindingError_exn_struct*_T37;char*_T38;char*_T39;struct Cyc_Warn_Qvar_Warn_Warg_struct _T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;unsigned _T3C;struct _fat_ptr _T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;void*_T42;void*_T43;void*_T44;void*_T45;int*_T46;int _T47;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T48;void*_T49;int*_T4A;int _T4B;void*_T4C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4D;void*_T4E;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T4F;union Cyc_Absyn_AggrInfo*_T50;union Cyc_Absyn_AggrInfo*_T51;struct _union_AggrInfo_UnknownAggr _T52;unsigned _T53;struct _union_AggrInfo_UnknownAggr _T54;struct _tuple2 _T55;struct _union_AggrInfo_UnknownAggr _T56;struct _tuple2 _T57;struct _handler_cons*_T58;int*_T59;int _T5A;struct Cyc_Absyn_Aggrdecl*(*_T5B)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T5C;struct Cyc_Binding_Env*_T5D;struct Cyc_Binding_NSCtxt*_T5E;struct _tuple0*_T5F;union Cyc_Absyn_AggrInfo*_T60;struct Cyc_Absyn_Aggrdecl**_T61;void*_T62;struct Cyc_Binding_BindingError_exn_struct*_T63;char*_T64;char*_T65;struct Cyc_Warn_Qvar_Warn_Warg_struct _T66;struct Cyc_Warn_String_Warn_Warg_struct _T67;struct Cyc_Absyn_Pat*_T68;unsigned _T69;struct _fat_ptr _T6A;struct Cyc_Absyn_Pat*_T6B;unsigned _T6C;struct Cyc_Binding_Env*_T6D;void*_T6E;struct Cyc_Binding_Env*_T6F;struct Cyc_List_List*_T70;void*_T71;struct _tuple12*_T72;struct _tuple12 _T73;struct Cyc_Absyn_Pat*_T74;struct Cyc_List_List*_T75;void*_T76;struct Cyc_Binding_Env*_T77;struct Cyc_List_List*_T78;void*_T79;struct Cyc_Absyn_Pat*_T7A;struct Cyc_List_List*_T7B;unsigned _T7C;void*_T7D;_T0=p;{struct Cyc_Absyn_Pat _T7E=*_T0;_T7D=_T7E.r;_T7C=_T7E.loc;}{void*r=_T7D;unsigned loc=_T7C;struct Cyc_Absyn_Pat*_T7E;int _T7F;struct Cyc_List_List*_T80;void*_T81;_T1=r;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 14: _T4=r;{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T4;_T81=_T82->f1;}{struct _tuple0*qv=_T81;{struct _handler_cons _T82;_T5=& _T82;_push_handler(_T5);{int _T83=0;_T6=_T82.handler;_T7=setjmp(_T6);if(!_T7)goto _TL125;_T83=1;goto _TL126;_TL125: _TL126: if(_T83)goto _TL127;else{goto _TL129;}_TL129:{
# 745
void*_T84=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Enumfield*_T85;struct Cyc_Absyn_Datatypefield*_T86;void*_T87;_T8=(int*)_T84;_T9=*_T8;switch(_T9){case 0: goto _LL26;case 1:{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;
# 748
_T88.f1=_tag_fat("struct tag used without arguments in pattern",sizeof(char),45U);_TA=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_TA;void*_T89[1];_T89[0]=& _T88;_TB=loc;_TC=_tag_fat(_T89,sizeof(void*),1);Cyc_Warn_err2(_TB,_TC);}_npop_handler(0);return;case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T88=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T84;_T87=_T88->f1;_T86=_T88->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T87;struct Cyc_Absyn_Datatypefield*tuf=_T86;_TD=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T88->tag=7;
# 750
_T88->f1=tud;_T88->f2=tuf;_T88->f3=0;_T88->f4=0;_TE=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T88;}_TD->r=(void*)_TE;_npop_handler(0);return;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T88=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T84;_T87=_T88->f1;_T85=_T88->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T87;struct Cyc_Absyn_Enumfield*ef=_T85;_TF=p;{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct));_T88->tag=12;
# 752
_T88->f1=ed;_T88->f2=ef;_T10=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T88;}_TF->r=(void*)_T10;_npop_handler(0);return;}default:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T88=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T84;_T11=_T88->f1;_T87=(void*)_T11;_T85=_T88->f2;}{void*t=_T87;struct Cyc_Absyn_Enumfield*ef=_T85;_T12=p;{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct));_T88->tag=13;
# 754
_T88->f1=t;_T88->f2=ef;_T13=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T88;}_T12->r=(void*)_T13;_npop_handler(0);return;}}_LL26:;}_pop_handler();goto _TL128;_TL127: _T14=Cyc_Core_get_exn_thrown();{void*_T84=(void*)_T14;void*_T85;_T15=(struct Cyc_Binding_BindingError_exn_struct*)_T84;_T16=_T15->tag;_T17=Cyc_Binding_BindingError;if(_T16!=_T17)goto _TL12B;goto _LL31;_TL12B: _T85=_T84;{void*exn=_T85;_rethrow(exn);}_LL31:;}_TL128:;}}{
# 756
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0,0);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T18=p;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T82=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T82->tag=1;
_T82->f1=vd;_T1A=& Cyc_Absyn_Wild_p_val;_T1B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1A;_T1C=(void*)_T1B;_T82->f2=Cyc_Absyn_new_pat(_T1C,0U);_T19=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T82;}_T18->r=(void*)_T19;
return;}}case 15: _T1D=r;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T1D;_T81=_T82->f1;_T80=_T82->f2;_T7F=_T82->f3;}{struct _tuple0*qv=_T81;struct Cyc_List_List*ps=_T80;int dots=_T7F;{
# 762
struct Cyc_List_List*ps2=ps;_TL130: if(ps2!=0)goto _TL12E;else{goto _TL12F;}
_TL12E: _T1E=env;_T1F=ps2;_T20=_T1F->hd;_T21=(struct Cyc_Absyn_Pat*)_T20;Cyc_Binding_resolve_pat(_T1E,_T21);_T22=ps2;
# 762
ps2=_T22->tl;goto _TL130;_TL12F:;}{struct _handler_cons _T82;_T23=& _T82;_push_handler(_T23);{int _T83=0;_T24=_T82.handler;_T25=setjmp(_T24);if(!_T25)goto _TL131;_T83=1;goto _TL132;_TL131: _TL132: if(_T83)goto _TL133;else{goto _TL135;}_TL135:{
# 764
void*_T84=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Datatypefield*_T85;struct Cyc_Absyn_Datatypedecl*_T86;struct Cyc_Absyn_Aggrdecl*_T87;_T26=(int*)_T84;_T27=*_T26;switch(_T27){case 0: goto _LL36;case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T88=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T84;_T87=_T88->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T87;
# 767
struct Cyc_List_List*new_ps=0;
_TL13A: if(ps!=0)goto _TL138;else{goto _TL139;}
_TL138:{struct Cyc_List_List*_T88=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T89=_cycalloc(sizeof(struct _tuple12));_T89->f0=0;_T2A=ps;_T2B=_T2A->hd;_T89->f1=(struct Cyc_Absyn_Pat*)_T2B;_T29=(struct _tuple12*)_T89;}_T88->hd=_T29;_T88->tl=new_ps;_T28=(struct Cyc_List_List*)_T88;}new_ps=_T28;_T2C=ps;
# 768
ps=_T2C->tl;goto _TL13A;_TL139:{struct Cyc_Absyn_Aggrdecl**_T88=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 770
*_T88=ad;_T2D=(struct Cyc_Absyn_Aggrdecl**)_T88;}_T2E=Cyc_Absyn_KnownAggr(_T2D);{void*atype=Cyc_Absyn_aggr_type(_T2E,0);_T2F=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T88->tag=6;
_T88->f1=atype;_T88->f2=0;_T88->f3=0;_T88->f4=Cyc_List_imp_rev(new_ps);_T88->f5=dots;_T30=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T88;}_T2F->r=(void*)_T30;_npop_handler(0);return;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T88=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T84;_T86=_T88->f1;_T85=_T88->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T86;struct Cyc_Absyn_Datatypefield*tuf=_T85;_T31=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T88=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T88->tag=7;
# 774
_T88->f1=tud;_T88->f2=tuf;_T88->f3=ps;_T88->f4=dots;_T32=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T88;}_T31->r=(void*)_T32;_npop_handler(0);return;}case 3: goto _LL40;default: _LL40:{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;
# 777
_T88.f1=_tag_fat("enum tag used with arguments in pattern",sizeof(char),40U);_T33=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T33;void*_T89[1];_T89[0]=& _T88;_T34=loc;_T35=_tag_fat(_T89,sizeof(void*),1);Cyc_Warn_err2(_T34,_T35);}_npop_handler(0);return;}_LL36:;}_pop_handler();goto _TL134;_TL133: _T36=Cyc_Core_get_exn_thrown();{void*_T84=(void*)_T36;void*_T85;_T37=(struct Cyc_Binding_BindingError_exn_struct*)_T84;_T38=_T37->tag;_T39=Cyc_Binding_BindingError;if(_T38!=_T39)goto _TL13B;goto _LL41;_TL13B: _T85=_T84;{void*exn=_T85;_rethrow(exn);}_LL41:;}_TL134:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T82;_T82.tag=1;
# 779
_T82.f1=qv;_T3A=_T82;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T82=_T3A;{struct Cyc_Warn_String_Warn_Warg_struct _T83;_T83.tag=0;_T83.f1=_tag_fat(" is not a constructor in pattern",sizeof(char),33U);_T3B=_T83;}{struct Cyc_Warn_String_Warn_Warg_struct _T83=_T3B;void*_T84[2];_T84[0]=& _T82;_T84[1]=& _T83;_T3C=loc;_T3D=_tag_fat(_T84,sizeof(void*),2);Cyc_Warn_err2(_T3C,_T3D);}}return;}case 0:
# 781
 return;case 3: _T3E=r;{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3E;_T81=_T82->f1;_T7E=_T82->f2;}{struct Cyc_Absyn_Vardecl*vd=_T81;struct Cyc_Absyn_Pat*p2=_T7E;_T81=vd;_T7E=p2;goto _LLD;}case 1: _T3F=r;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T3F;_T81=_T82->f1;_T7E=_T82->f2;}_LLD: {struct Cyc_Absyn_Vardecl*vd=_T81;struct Cyc_Absyn_Pat*p2=_T7E;
# 785
Cyc_Binding_resolve_pat(env,p2);_T81=vd;goto _LLF;}case 4: _T40=r;{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T40;_T81=_T82->f2;}_LLF: {struct Cyc_Absyn_Vardecl*vd=_T81;_T81=vd;goto _LL11;}case 2: _T41=r;{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T41;_T81=_T82->f2;}_LL11: {struct Cyc_Absyn_Vardecl*vd=_T81;
# 789
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 5: _T42=r;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T42;_T81=_T82->f1;}{struct Cyc_Absyn_Pat*p2=_T81;
# 791
Cyc_Binding_resolve_pat(env,p2);return;}case 16: _T43=r;{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T43;_T81=_T82->f1;}{struct Cyc_Absyn_Exp*e=_T81;
Cyc_Binding_resolve_exp(env,e);return;}case 8: goto _LL19;case 9: _LL19: goto _LL1B;case 10: _LL1B: goto _LL1D;case 11: _LL1D:
# 797
 return;case 6: _T44=r;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T44;_T45=_T82->f1;_T81=(void*)_T45;_T80=_T82->f4;}{void*topt=_T81;struct Cyc_List_List*dps=_T80;
# 800
if(topt==0)goto _TL13D;{
void*t=topt;
void*_T82=Cyc_Absyn_compress(t);void*_T83;_T46=(int*)_T82;_T47=*_T46;if(_T47!=0)goto _TL13F;_T48=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T49=_T48->f1;_T4A=(int*)_T49;_T4B=*_T4A;if(_T4B!=24)goto _TL141;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T84=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T4C=_T84->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T85=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4C;_T4D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T4E=_T4D->f1;_T4F=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4E;_T50=& _T4F->f1;_T83=(union Cyc_Absyn_AggrInfo*)_T50;}}{union Cyc_Absyn_AggrInfo*info=(union Cyc_Absyn_AggrInfo*)_T83;_T51=info;{
# 804
union Cyc_Absyn_AggrInfo _T84=*_T51;struct _tuple0*_T85;enum Cyc_Absyn_AggrKind _T86;_T52=_T84.UnknownAggr;_T53=_T52.tag;if(_T53!=1)goto _TL143;_T54=_T84.UnknownAggr;_T55=_T54.val;_T86=_T55.f0;_T56=_T84.UnknownAggr;_T57=_T56.val;_T85=_T57.f1;{enum Cyc_Absyn_AggrKind ak=_T86;struct _tuple0*n=_T85;{struct _handler_cons _T87;_T58=& _T87;_push_handler(_T58);{int _T88=0;_T59=_T87.handler;_T5A=setjmp(_T59);if(!_T5A)goto _TL145;_T88=1;goto _TL146;_TL145: _TL146: if(_T88)goto _TL147;else{goto _TL149;}_TL149:{
# 807
struct Cyc_Absyn_Aggrdecl*(*_T89)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T5B=_T89;}_T5C=loc;_T5D=env;_T5E=_T5D->ns;_T5F=n;{struct Cyc_Absyn_Aggrdecl*ad=_T5B(_T5C,_T5E,_T5F,Cyc_Binding_lookup_aggrdecl);_T60=info;{struct Cyc_Absyn_Aggrdecl**_T89=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_T89=ad;_T61=(struct Cyc_Absyn_Aggrdecl**)_T89;}*_T60=Cyc_Absyn_KnownAggr(_T61);}_pop_handler();goto _TL148;_TL147: _T62=Cyc_Core_get_exn_thrown();{void*_T89=(void*)_T62;void*_T8A;_T63=(struct Cyc_Binding_BindingError_exn_struct*)_T89;_T64=_T63->tag;_T65=Cyc_Binding_BindingError;if(_T64!=_T65)goto _TL14A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T8B;_T8B.tag=1;
# 811
_T8B.f1=n;_T66=_T8B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T8B=_T66;{struct Cyc_Warn_String_Warn_Warg_struct _T8C;_T8C.tag=0;_T8C.f1=_tag_fat(" is an undefined aggregate",sizeof(char),27U);_T67=_T8C;}{struct Cyc_Warn_String_Warn_Warg_struct _T8C=_T67;void*_T8D[2];_T8D[0]=& _T8B;_T8D[1]=& _T8C;_T68=p;_T69=_T68->loc;_T6A=_tag_fat(_T8D,sizeof(void*),2);Cyc_Warn_err2(_T69,_T6A);}}goto _LL50;_TL14A: _T8A=_T89;{void*exn=_T8A;_rethrow(exn);}_LL50:;}_TL148:;}}goto _LL4B;}_TL143: goto _LL4B;_LL4B:;}goto _LL46;}_TL141: goto _LL49;_TL13F: _LL49: _T6B=p;_T6C=_T6B->loc;_T6D=env;_T6E=topt;
# 818
Cyc_Binding_resolve_type(_T6C,_T6D,_T6E);goto _LL46;_LL46:;}goto _TL13E;_TL13D: _TL13E:
# 821
 _TL14F: if(dps!=0)goto _TL14D;else{goto _TL14E;}
_TL14D: _T6F=env;_T70=dps;_T71=_T70->hd;_T72=(struct _tuple12*)_T71;_T73=*_T72;_T74=_T73.f1;Cyc_Binding_resolve_pat(_T6F,_T74);_T75=dps;
# 821
dps=_T75->tl;goto _TL14F;_TL14E:
# 823
 return;}case 7: _T76=r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T82=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T76;_T80=_T82->f3;}{struct Cyc_List_List*ps=_T80;
# 825
_TL153: if(ps!=0)goto _TL151;else{goto _TL152;}
_TL151: _T77=env;_T78=ps;_T79=_T78->hd;_T7A=(struct Cyc_Absyn_Pat*)_T79;Cyc_Binding_resolve_pat(_T77,_T7A);_T7B=ps;
# 825
ps=_T7B->tl;goto _TL153;_TL152:
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
static void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;struct Cyc_Absyn_Decl*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Vardecl*_T4;struct Cyc_Absyn_Exp*_T5;int _T6;struct Cyc_Binding_Env*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Binding_Env*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;unsigned _TD;struct _fat_ptr _TE;unsigned _TF;struct Cyc_Binding_Env*_T10;struct Cyc_Absyn_Fndecl*_T11;struct _tuple0*_T12;struct Cyc_Absyn_Fndecl*_T13;enum Cyc_Absyn_Scope _T14;struct Cyc_Absyn_Fndecl*_T15;struct Cyc_Absyn_Fndecl*_T16;struct Cyc_Absyn_FnInfo _T17;unsigned _T18;struct Cyc_Binding_Env*_T19;struct Cyc_Absyn_Fndecl*_T1A;struct Cyc_Absyn_FnInfo _T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Fndecl*_T1D;struct Cyc_Absyn_FnInfo _T1E;struct Cyc_Absyn_VarargInfo*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Vardecl*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_List_List*_T26;struct Cyc_Absyn_Fndecl*_T27;struct Cyc_Core_Opt*_T28;struct Cyc_Absyn_Fndecl*_T29;int _T2A;unsigned _T2B;struct Cyc_Binding_Env*_T2C;struct Cyc_Absyn_Fndecl*_T2D;struct _tuple0*_T2E;struct Cyc_Binding_Env*_T2F;struct Cyc_Binding_Env*_T30;struct Cyc_Dict_Dict*_T31;struct Cyc_Binding_Env*_T32;struct Cyc_Dict_Dict*_T33;struct Cyc_Binding_Env*_T34;struct Cyc_Dict_Dict*_T35;struct Cyc_Dict_Dict(*_T36)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T37)(int(*)(void*,void*));int(*_T38)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T39;void*_T3A;struct Cyc_Absyn_Vardecl*_T3B;struct Cyc_Binding_Env*_T3C;struct Cyc_Dict_Dict*_T3D;struct Cyc_Dict_Dict*_T3E;struct Cyc_Dict_Dict(*_T3F)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T40)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T41;struct Cyc_Dict_Dict*_T42;struct Cyc_Dict_Dict*_T43;struct Cyc_Dict_Dict _T44;struct Cyc_List_List*_T45;void*_T46;struct Cyc_Absyn_Vardecl*_T47;struct _tuple0*_T48;struct _fat_ptr*_T49;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T4A;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T4B;struct Cyc_List_List*_T4C;void*_T4D;void*_T4E;struct Cyc_List_List*_T4F;unsigned _T50;struct Cyc_Binding_Env*_T51;struct Cyc_Absyn_Fndecl*_T52;struct Cyc_Absyn_FnInfo*_T53;struct Cyc_Absyn_FnInfo*_T54;struct Cyc_Dict_Dict*_T55;struct Cyc_Dict_Dict(*_T56)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T57)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T58;struct Cyc_Dict_Dict _T59;struct _fat_ptr*_T5A;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T5B;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T5C;void*_T5D;struct Cyc_Binding_Env*_T5E;struct Cyc_Dict_Dict*_T5F;struct Cyc_Dict_Dict*_T60;struct Cyc_Dict_Dict(*_T61)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T62)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T63;struct Cyc_Dict_Dict*_T64;struct Cyc_Dict_Dict*_T65;struct Cyc_Dict_Dict _T66;struct _fat_ptr*_T67;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T68;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T69;void*_T6A;struct Cyc_Binding_ResolveNSEnv*(*_T6B)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T6C;struct Cyc_Binding_NSCtxt*_T6D;union Cyc_Absyn_Nmspace _T6E;struct Cyc_Binding_ResolveNSEnv*_T6F;struct Cyc_Dict_Dict(*_T70)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T71)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T72;struct Cyc_Dict_Dict _T73;struct _fat_ptr*_T74;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T75;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T76;void*_T77;int _T78;struct Cyc_Binding_Env*_T79;struct Cyc_Absyn_Fndecl*_T7A;struct Cyc_Absyn_Stmt*_T7B;struct Cyc_Binding_Env*_T7C;int _T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;unsigned _T7F;struct _fat_ptr _T80;unsigned _T81;struct Cyc_Binding_Env*_T82;struct Cyc_List_List*_T83;void*_T84;struct Cyc_Absyn_Vardecl*_T85;struct Cyc_List_List*_T86;int _T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;unsigned _T89;struct _fat_ptr _T8A;unsigned _T8B;struct Cyc_Binding_Env*_T8C;struct Cyc_Absyn_Typedefdecl*_T8D;struct _tuple0*_T8E;enum Cyc_Absyn_Scope _T8F;struct Cyc_Absyn_Typedefdecl*_T90;int _T91;struct Cyc_Absyn_Typedefdecl*_T92;struct Cyc_Binding_ResolveNSEnv*(*_T93)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T94;struct Cyc_Binding_NSCtxt*_T95;union Cyc_Absyn_Nmspace _T96;struct Cyc_Absyn_Typedefdecl*_T97;void*_T98;unsigned _T99;struct Cyc_Binding_Env*_T9A;struct Cyc_Absyn_Typedefdecl*_T9B;void*_T9C;struct Cyc_Binding_Env*_T9D;int _T9E;struct Cyc_Binding_ResolveNSEnv*_T9F;struct Cyc_Dict_Dict(*_TA0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_TA1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TA2;struct Cyc_Dict_Dict _TA3;struct _fat_ptr*_TA4;struct Cyc_Absyn_Typedefdecl*_TA5;int _TA6;struct Cyc_Warn_String_Warn_Warg_struct _TA7;unsigned _TA8;struct _fat_ptr _TA9;unsigned _TAA;struct Cyc_Binding_Env*_TAB;struct Cyc_Absyn_Aggrdecl*_TAC;struct _tuple0*_TAD;struct Cyc_Absyn_Aggrdecl*_TAE;enum Cyc_Absyn_Scope _TAF;struct Cyc_Absyn_Aggrdecl*_TB0;struct Cyc_Binding_ResolveNSEnv*(*_TB1)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TB2;struct Cyc_Binding_NSCtxt*_TB3;union Cyc_Absyn_Nmspace _TB4;int(*_TB5)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TB6)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TB7;struct Cyc_Dict_Dict _TB8;struct _fat_ptr*_TB9;int _TBA;struct Cyc_Absyn_Aggrdecl*_TBB;struct Cyc_Absyn_AggrdeclImpl*_TBC;struct Cyc_Binding_Env*_TBD;int _TBE;struct Cyc_Binding_ResolveNSEnv*_TBF;struct Cyc_Dict_Dict(*_TC0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_TC1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TC2;struct Cyc_Dict_Dict _TC3;struct _fat_ptr*_TC4;struct Cyc_Absyn_Aggrdecl*_TC5;struct Cyc_Binding_ResolveNSEnv*_TC6;struct Cyc_Dict_Dict(*_TC7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TC8)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TC9;struct Cyc_Dict_Dict _TCA;struct _fat_ptr*_TCB;struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TCC;void*_TCD;struct Cyc_Absyn_Aggrdecl*_TCE;struct Cyc_Absyn_AggrdeclImpl*_TCF;struct Cyc_Absyn_Aggrdecl*_TD0;int _TD1;struct Cyc_Warn_String_Warn_Warg_struct _TD2;unsigned _TD3;struct _fat_ptr _TD4;unsigned _TD5;struct Cyc_Binding_Env*_TD6;struct Cyc_Absyn_Enumdecl*_TD7;struct _tuple0*_TD8;struct Cyc_Absyn_Enumdecl*_TD9;enum Cyc_Absyn_Scope _TDA;struct Cyc_Absyn_Enumdecl*_TDB;struct Cyc_Binding_ResolveNSEnv*(*_TDC)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TDD;struct Cyc_Binding_NSCtxt*_TDE;union Cyc_Absyn_Nmspace _TDF;int(*_TE0)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TE1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TE2;struct Cyc_Dict_Dict _TE3;struct _fat_ptr*_TE4;int _TE5;struct Cyc_Absyn_Enumdecl*_TE6;struct Cyc_Core_Opt*_TE7;struct Cyc_Binding_Env*_TE8;int _TE9;struct Cyc_Binding_ResolveNSEnv*_TEA;struct Cyc_Dict_Dict(*_TEB)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TED;struct Cyc_Dict_Dict _TEE;struct _fat_ptr*_TEF;struct Cyc_Absyn_Enumdecl*_TF0;struct Cyc_Absyn_Enumdecl*_TF1;struct Cyc_Core_Opt*_TF2;struct Cyc_Absyn_Enumdecl*_TF3;struct Cyc_Core_Opt*_TF4;void*_TF5;struct Cyc_List_List*_TF6;void*_TF7;struct Cyc_Absyn_Enumfield*_TF8;unsigned _TF9;struct Cyc_Binding_Env*_TFA;struct Cyc_Absyn_Enumfield*_TFB;struct _tuple0*_TFC;struct Cyc_Absyn_Enumdecl*_TFD;enum Cyc_Absyn_Scope _TFE;struct Cyc_Absyn_Enumfield*_TFF;struct Cyc_Absyn_Exp*_T100;struct Cyc_Binding_Env*_T101;struct Cyc_Absyn_Enumfield*_T102;struct Cyc_Absyn_Exp*_T103;struct Cyc_Binding_Env*_T104;int _T105;struct Cyc_Binding_ResolveNSEnv*_T106;struct Cyc_Dict_Dict(*_T107)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T108)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T109;struct Cyc_Dict_Dict _T10A;struct Cyc_Absyn_Enumfield*_T10B;struct _tuple0*_T10C;struct _tuple0 _T10D;struct _fat_ptr*_T10E;struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T10F;void*_T110;struct Cyc_List_List*_T111;struct _handler_cons*_T112;int*_T113;int _T114;struct Cyc_List_List*(*_T115)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T116;struct Cyc_Binding_Env*_T117;struct Cyc_Binding_NSCtxt*_T118;struct Cyc_Absyn_Datatypedecl*_T119;struct _tuple0*_T11A;struct Cyc_List_List*_T11B;void*_T11C;struct Cyc_Absyn_Datatypedecl*_T11D;int _T11E;struct Cyc_Binding_BindingError_exn_struct*_T11F;void*_T120;struct Cyc_Absyn_Datatypedecl*_T121;struct Cyc_Absyn_Datatypedecl*_T122;struct Cyc_Absyn_Datatypedecl*_T123;struct Cyc_Absyn_Datatypedecl*_T124;struct Cyc_Binding_ResolveNSEnv*(*_T125)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T126;struct Cyc_Binding_NSCtxt*_T127;union Cyc_Absyn_Nmspace _T128;struct Cyc_Binding_ResolveNSEnv*_T129;struct Cyc_Dict_Dict(*_T12A)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T12B)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T12C;struct Cyc_Dict_Dict _T12D;struct _fat_ptr*_T12E;struct Cyc_List_List*_T12F;void*_T130;struct Cyc_Binding_BindingError_exn_struct*_T131;char*_T132;char*_T133;unsigned _T134;struct Cyc_Binding_Env*_T135;struct Cyc_Absyn_Datatypedecl*_T136;struct _tuple0*_T137;struct Cyc_Absyn_Datatypedecl*_T138;enum Cyc_Absyn_Scope _T139;struct Cyc_Absyn_Datatypedecl*_T13A;struct Cyc_Binding_ResolveNSEnv*(*_T13B)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T13C;struct Cyc_Binding_NSCtxt*_T13D;union Cyc_Absyn_Nmspace _T13E;int(*_T13F)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T140)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T141;struct Cyc_Dict_Dict _T142;struct _fat_ptr*_T143;int _T144;struct Cyc_Absyn_Datatypedecl*_T145;struct Cyc_Core_Opt*_T146;struct Cyc_Binding_ResolveNSEnv*_T147;struct Cyc_Dict_Dict(*_T148)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T149)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T14A;struct Cyc_Dict_Dict _T14B;struct _fat_ptr*_T14C;struct Cyc_List_List*_T14D;struct Cyc_Absyn_Datatypedecl*_T14E;struct Cyc_Absyn_Datatypedecl*_T14F;struct Cyc_Core_Opt*_T150;struct Cyc_Absyn_Datatypedecl*_T151;struct Cyc_Core_Opt*_T152;void*_T153;struct Cyc_List_List*_T154;void*_T155;struct Cyc_Absyn_Datatypefield*_T156;unsigned _T157;struct Cyc_Binding_Env*_T158;struct Cyc_List_List*_T159;void*_T15A;struct _tuple16*_T15B;struct _tuple16 _T15C;void*_T15D;struct Cyc_List_List*_T15E;struct Cyc_Absyn_Datatypefield*_T15F;struct _tuple0*_T160;struct _tuple0 _T161;struct _union_Nmspace_Abs_n _T162;unsigned _T163;struct _union_Nmspace_Rel_n _T164;struct Cyc_List_List*_T165;struct Cyc_Absyn_Datatypedecl*_T166;int _T167;struct Cyc_Absyn_Datatypefield*_T168;struct _tuple0*_T169;struct Cyc_Binding_Env*_T16A;struct Cyc_Binding_NSCtxt*_T16B;struct Cyc_List_List*_T16C;struct Cyc_Absyn_Datatypefield*_T16D;struct _tuple0*_T16E;struct Cyc_Absyn_Datatypedecl*_T16F;struct _tuple0*_T170;struct _tuple0 _T171;struct Cyc_Warn_String_Warn_Warg_struct _T172;struct Cyc_Absyn_Datatypefield*_T173;unsigned _T174;struct _fat_ptr _T175;int(*_T176)(struct _fat_ptr,struct _fat_ptr);void*(*_T177)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T178;struct _fat_ptr _T179;struct Cyc_Binding_ResolveNSEnv*(*_T17A)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T17B;struct Cyc_Binding_NSCtxt*_T17C;struct Cyc_Absyn_Datatypefield*_T17D;struct _tuple0*_T17E;struct _tuple0 _T17F;union Cyc_Absyn_Nmspace _T180;struct Cyc_Binding_ResolveNSEnv*_T181;struct Cyc_Dict_Dict(*_T182)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T183)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T184;struct Cyc_Dict_Dict _T185;struct Cyc_Absyn_Datatypefield*_T186;struct _tuple0*_T187;struct _tuple0 _T188;struct _fat_ptr*_T189;struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T18A;void*_T18B;struct Cyc_List_List*_T18C;void(*_T18D)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T18E;struct Cyc_Binding_NSCtxt*_T18F;struct _fat_ptr*_T190;struct Cyc_Binding_Env*_T191;struct Cyc_Binding_NSCtxt*_T192;struct Cyc_Absyn_Decl*_T193;unsigned _T194;struct Cyc_Binding_Env*_T195;struct Cyc_Binding_NSCtxt*_T196;struct _tuple0*_T197;struct Cyc_Binding_Env*_T198;struct Cyc_Binding_NSCtxt*_T199;struct Cyc_Binding_Env*_T19A;struct Cyc_Binding_Env*_T19B;struct Cyc_Binding_Env*_T19C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T19D;struct Cyc_List_List**_T19E;struct Cyc_List_List**_T19F;struct Cyc_List_List*_T1A0;void*_T1A1;struct Cyc_List_List*_T1A2;int _T1A3;struct Cyc_Warn_String_Warn_Warg_struct _T1A4;unsigned _T1A5;struct _fat_ptr _T1A6;struct Cyc_Binding_ResolveNSEnv*(*_T1A7)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1A8)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T1A9;struct Cyc_Binding_NSCtxt*_T1AA;struct Cyc_Dict_Dict _T1AB;struct Cyc_Binding_Env*_T1AC;struct Cyc_Binding_NSCtxt*_T1AD;struct Cyc_List_List*_T1AE;struct Cyc_Binding_ResolveNSEnv*_T1AF;struct Cyc_Binding_Env*_T1B0;struct Cyc_Binding_Env*_T1B1;struct Cyc_Binding_Env*_T1B2;struct Cyc_Binding_Env*_T1B3;struct Cyc_Binding_ResolveNSEnv*_T1B4;struct _tuple10*_T1B5;struct _tuple10 _T1B6;unsigned _T1B7;struct Cyc_List_List**_T1B8;struct Cyc_List_List*_T1B9;unsigned _T1BA;struct _tuple10*_T1BB;struct _tuple10 _T1BC;unsigned _T1BD;struct _fat_ptr _T1BE;struct _fat_ptr _T1BF;struct Cyc_Binding_Env*_T1C0;void(*_T1C1)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict);void(*_T1C2)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple14*_T1C3;struct Cyc_Dict_Dict*_T1C4;struct Cyc_Dict_Dict _T1C5;struct Cyc_List_List**_T1C6;struct Cyc_List_List*_T1C7;void*_T1C8;struct _tuple0*_T1C9;int(*_T1CA)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1CB)(struct Cyc_Dict_Dict,void*);int _T1CC;int(*_T1CD)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1CE)(struct Cyc_Dict_Dict,void*);int _T1CF;void*(*_T1D0)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1D1)(struct Cyc_Dict_Dict,void*);void*_T1D2;void*(*_T1D3)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1D4)(struct Cyc_Dict_Dict,void*);void*_T1D5;struct Cyc_String_pa_PrintArg_struct _T1D6;struct _fat_ptr*_T1D7;unsigned _T1D8;struct _fat_ptr _T1D9;struct _fat_ptr _T1DA;struct Cyc_Dict_Dict(*_T1DB)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T1DC)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict _T1DD;struct _fat_ptr*_T1DE;void*(*_T1DF)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1E0)(struct Cyc_Dict_Dict,void*);void*_T1E1;struct Cyc_List_List*_T1E2;struct Cyc_Binding_ResolveNSEnv*_T1E3;struct Cyc_Binding_Env*_T1E4;_T0=d;{
# 902
unsigned loc=_T0->loc;_T1=d;{
void*_T1E5=_T1->r;struct _tuple10*_T1E6;struct Cyc_List_List*_T1E7;struct Cyc_List_List*_T1E8;struct Cyc_Absyn_Vardecl*_T1E9;struct Cyc_Absyn_Exp*_T1EA;void*_T1EB;_T2=(int*)_T1E5;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1EB;
# 906
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T4=vd;_T5=_T4->initializer;
# 911
if(_T5==0)goto _TL165;_T6=Cyc_Binding_in_cinclude(env);if(_T6)goto _TL165;else{goto _TL167;}
_TL167: _T7=env;_T8=vd;_T9=_T8->initializer;Cyc_Binding_resolve_exp(_T7,_T9);goto _TL166;_TL165: _TL166: goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Fndecl*fd=_T1EB;_TA=env;_TB=_TA->in_cyc_override;
# 916
if(!_TB)goto _TL168;{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;
_T1EC.f1=_tag_fat("Function declaration not permitted within cyclone_override block",sizeof(char),65U);_TC=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_TC;void*_T1ED[1];_T1ED[0]=& _T1EC;_TD=loc;_TE=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_TD,_TE);}goto _TL169;_TL168: _TL169: _TF=loc;_T10=env;_T11=fd;_T12=_T11->name;_T13=fd;_T14=_T13->sc;
Cyc_Binding_absolutize_decl(_TF,_T10,_T12,_T14);_T15=fd;{
struct _tuple0*_T1EC=_T15->name;struct _fat_ptr*_T1ED;union Cyc_Absyn_Nmspace _T1EE;{struct _tuple0 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EE;struct _fat_ptr*decl_name=_T1ED;_T16=fd;_T17=_T16->i;{
struct Cyc_List_List*argit=_T17.args;_T18=loc;_T19=env;_T1A=fd;_T1B=_T1A->i;_T1C=_T1B.args;_T1D=fd;_T1E=_T1D->i;_T1F=_T1E.cyc_varargs;{
struct Cyc_List_List*vdsopts=Cyc_Binding_get_fun_vardecls(1,_T18,_T19,_T1C,_T1F);
struct Cyc_List_List*vds=0;{
struct Cyc_List_List*v=vdsopts;_TL16D: if(v!=0)goto _TL16B;else{goto _TL16C;}
_TL16B: _T20=v;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Vardecl*)_T21;if(_T22==0)goto _TL16E;{struct Cyc_List_List*_T1EF=_cycalloc(sizeof(struct Cyc_List_List));_T24=v;_T25=_T24->hd;_T1EF->hd=(struct Cyc_Absyn_Vardecl*)_T25;_T1EF->tl=vds;_T23=(struct Cyc_List_List*)_T1EF;}vds=_T23;goto _TL16F;_TL16E: _TL16F: _T26=v;
# 923
v=_T26->tl;goto _TL16D;_TL16C:;}
# 925
vds=Cyc_List_imp_rev(vds);_T27=fd;{struct Cyc_Core_Opt*_T1EF=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T1EF->v=vds;_T28=(struct Cyc_Core_Opt*)_T1EF;}_T27->param_vardecls=_T28;_T29=fd;
_T29->i.arg_vardecls=vdsopts;_T2A=Cyc_Flags_warn_override;
if(!_T2A)goto _TL170;_T2B=loc;_T2C=env;_T2D=fd;_T2E=_T2D->name;
Cyc_Binding_check_warn_override(_T2B,_T2C,_T2E);goto _TL171;_TL170: _TL171: _T2F=env;{
# 931
struct Cyc_Dict_Dict*old_locals=_T2F->local_vars;
if(old_locals==0)goto _TL172;_T30=env;{struct Cyc_Dict_Dict*_T1EF=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T32=env;_T33=_T32->local_vars;
*_T1EF=*_T33;_T31=(struct Cyc_Dict_Dict*)_T1EF;}_T30->local_vars=_T31;goto _TL173;
# 935
_TL172: _T34=env;{struct Cyc_Dict_Dict*_T1EF=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T37=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T1F0)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T37;_T36=_T1F0;}_T38=Cyc_strptrcmp;*_T1EF=_T36(_T38);_T35=(struct Cyc_Dict_Dict*)_T1EF;}_T34->local_vars=_T35;_TL173:{
struct Cyc_List_List*vds1=vds;_TL177: if(vds1!=0)goto _TL175;else{goto _TL176;}
_TL175: _T39=vds1;_T3A=_T39->hd;_T3B=(struct Cyc_Absyn_Vardecl*)_T3A;if(_T3B==0)goto _TL178;_T3C=env;_T3D=_T3C->local_vars;_T3E=
_check_null(_T3D);_T40=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T40;_T3F=_T1EF;}_T41=env;_T42=_T41->local_vars;_T43=_check_null(_T42);_T44=*_T43;_T45=vds1;_T46=_T45->hd;_T47=(struct Cyc_Absyn_Vardecl*)_T46;_T48=_T47->name;_T49=_T48->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EF->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T1F0=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T1F0->tag=3;_T4C=vds1;_T4D=_T4C->hd;
_T1F0->f1=(struct Cyc_Absyn_Vardecl*)_T4D;_T4B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T1F0;}_T1EF->f1=(void*)_T4B;_T4A=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EF;}_T4E=(void*)_T4A;
# 938
*_T3E=_T3F(_T44,_T49,_T4E);goto _TL179;_TL178: _TL179: _T4F=vds1;
# 936
vds1=_T4F->tl;goto _TL177;_TL176:;}_T50=loc;_T51=env;_T52=fd;_T53=& _T52->i;_T54=(struct Cyc_Absyn_FnInfo*)_T53;
# 941
Cyc_Binding_resolve_function_stuff(_T50,_T51,_T54);
# 943
if(old_locals==0)goto _TL17A;_T55=old_locals;_T57=Cyc_Dict_insert;{
# 945
struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T57;_T56=_T1EF;}_T58=old_locals;_T59=*_T58;_T5A=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EF->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F0=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F0->tag=2;
_T1F0->f1=fd;_T5C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F0;}_T1EF->f1=(void*)_T5C;_T5B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EF;}_T5D=(void*)_T5B;
# 945
*_T55=_T56(_T59,_T5A,_T5D);_T5E=env;_T5F=_T5E->local_vars;_T60=
# 947
_check_null(_T5F);_T62=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T62;_T61=_T1EF;}_T63=env;_T64=_T63->local_vars;_T65=_check_null(_T64);_T66=*_T65;_T67=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EF->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F0=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F0->tag=2;
_T1F0->f1=fd;_T69=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F0;}_T1EF->f1=(void*)_T69;_T68=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EF;}_T6A=(void*)_T68;
# 947
*_T60=_T61(_T66,_T67,_T6A);goto _TL17B;
# 950
_TL17A:{struct Cyc_Binding_ResolveNSEnv*(*_T1EF)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T6B=_T1EF;}_T6C=env;_T6D=_T6C->ns;_T6E=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T6B(_T6D,_T6E);_T6F=decl_ns_data;_T71=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T71;_T70=_T1EF;}_T72=decl_ns_data;_T73=_T72->ordinaries;_T74=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EF->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1F0=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1F0->tag=2;
# 953
_T1F0->f1=fd;_T76=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1F0;}_T1EF->f1=(void*)_T76;_T75=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EF;}_T77=(void*)_T75;
# 951
_T6F->ordinaries=_T70(_T73,_T74,_T77);}_TL17B: _T78=
# 956
Cyc_Binding_in_cinclude(env);if(_T78)goto _TL17C;else{goto _TL17E;}
_TL17E: _T79=env;_T7A=fd;_T7B=_T7A->body;Cyc_Binding_resolve_stmt(_T79,_T7B);goto _TL17D;_TL17C: _TL17D: _T7C=env;
# 959
_T7C->local_vars=old_locals;goto _LL0;}}}}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;_T1EA=_T1EC->f3;}{struct Cyc_Absyn_Pat*p=_T1EB;struct Cyc_Absyn_Exp*exp=_T1EA;_T7D=
# 963
Cyc_Binding_at_toplevel(env);if(!_T7D)goto _TL17F;{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;
_T1EC.f1=_tag_fat("let not allowed at top-level",sizeof(char),29U);_T7E=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_T7E;void*_T1ED[1];_T1ED[0]=& _T1EC;_T7F=loc;_T80=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_T7F,_T80);}goto _LL0;_TL17F:
# 967
 Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);goto _LL0;}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_List_List*vds=_T1EB;
# 972
_TL184: if(vds!=0)goto _TL182;else{goto _TL183;}
_TL182: _T81=loc;_T82=env;_T83=vds;_T84=_T83->hd;_T85=(struct Cyc_Absyn_Vardecl*)_T84;Cyc_Binding_resolve_and_add_vardecl(_T81,_T82,_T85);_T86=vds;
# 972
vds=_T86->tl;goto _TL184;_TL183: goto _LL0;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;_T1E9=_T1EC->f2;_T1EA=_T1EC->f3;}{struct Cyc_Absyn_Tvar*tv=_T1EB;struct Cyc_Absyn_Vardecl*vd=_T1E9;struct Cyc_Absyn_Exp*open_exp_opt=_T1EA;
# 977
if(open_exp_opt==0)goto _TL185;
Cyc_Binding_resolve_exp(env,open_exp_opt);goto _TL186;_TL185: _TL186:
 Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T1EB;_T87=
# 983
Cyc_Binding_at_toplevel(env);if(_T87)goto _TL187;else{goto _TL189;}
_TL189:{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;_T1EC.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_T88=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_T88;void*_T1ED[1];_T1ED[0]=& _T1EC;_T89=loc;_T8A=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_T89,_T8A);}goto _LL0;_TL187: _T8B=loc;_T8C=env;_T8D=td;_T8E=_T8D->name;_T90=td;_T91=_T90->extern_c;
# 987
if(!_T91)goto _TL18A;_T8F=4U;goto _TL18B;_TL18A: _T8F=2U;_TL18B: Cyc_Binding_absolutize_decl(_T8B,_T8C,_T8E,_T8F);_T92=td;{
struct _tuple0*_T1EC=_T92->name;struct _fat_ptr*_T1ED;union Cyc_Absyn_Nmspace _T1EE;{struct _tuple0 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EE;struct _fat_ptr*decl_name=_T1ED;{
# 990
struct Cyc_Binding_ResolveNSEnv*(*_T1EF)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T93=_T1EF;}_T94=env;_T95=_T94->ns;_T96=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T93(_T95,_T96);_T97=td;_T98=_T97->defn;
if(_T98==0)goto _TL18C;_T99=loc;_T9A=env;_T9B=td;_T9C=_T9B->defn;
Cyc_Binding_resolve_type(_T99,_T9A,_T9C);goto _TL18D;_TL18C: _TL18D: _T9D=env;_T9E=_T9D->in_cyc_override;
# 994
if(_T9E)goto _TL18E;else{goto _TL190;}
_TL190: _T9F=decl_ns_data;_TA1=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))_TA1;_TA0=_T1EF;}_TA2=decl_ns_data;_TA3=_TA2->typedefs;_TA4=decl_name;_TA5=td;_T9F->typedefs=_TA0(_TA3,_TA4,_TA5);goto _TL18F;_TL18E: _TL18F: goto _LL0;}}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T1EB;_TA6=
# 1001
Cyc_Binding_at_toplevel(env);if(_TA6)goto _TL191;else{goto _TL193;}
_TL193:{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;_T1EC.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TA7=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_TA7;void*_T1ED[1];_T1ED[0]=& _T1EC;_TA8=loc;_TA9=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_TA8,_TA9);}goto _LL0;_TL191: _TAA=loc;_TAB=env;_TAC=ad;_TAD=_TAC->name;_TAE=ad;_TAF=_TAE->sc;
# 1005
Cyc_Binding_absolutize_decl(_TAA,_TAB,_TAD,_TAF);_TB0=ad;{
struct _tuple0*_T1EC=_TB0->name;struct _fat_ptr*_T1ED;union Cyc_Absyn_Nmspace _T1EE;{struct _tuple0 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EE;struct _fat_ptr*decl_name=_T1ED;{
# 1008
struct Cyc_Binding_ResolveNSEnv*(*_T1EF)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TB1=_T1EF;}_TB2=env;_TB3=_TB2->ns;_TB4=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TB1(_TB3,_TB4);_TB6=Cyc_Dict_member;{
# 1010
int(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TB6;_TB5=_T1EF;}_TB7=decl_ns_data;_TB8=_TB7->aggrdecls;_TB9=decl_name;_TBA=_TB5(_TB8,_TB9);if(!_TBA)goto _TL194;_TBB=ad;_TBC=_TBB->impl;if(_TBC!=0)goto _TL194;goto _LL0;_TL194: _TBD=env;_TBE=_TBD->in_cyc_override;
# 1013
if(_TBE)goto _TL196;else{goto _TL198;}
_TL198: _TBF=decl_ns_data;_TC1=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))_TC1;_TC0=_T1EF;}_TC2=decl_ns_data;_TC3=_TC2->aggrdecls;_TC4=decl_name;_TC5=ad;_TBF->aggrdecls=_TC0(_TC3,_TC4,_TC5);_TC6=decl_ns_data;_TC8=Cyc_Dict_insert;{
# 1016
struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TC8;_TC7=_T1EF;}_TC9=decl_ns_data;_TCA=_TC9->ordinaries;_TCB=decl_name;{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_AggrRes_Binding_Resolved_struct));_T1EF->tag=1;
_T1EF->f1=ad;_TCC=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T1EF;}_TCD=(void*)_TCC;
# 1016
_TC6->ordinaries=_TC7(_TCA,_TCB,_TCD);goto _TL197;_TL196: _TL197: _TCE=ad;_TCF=_TCE->impl;
# 1019
if(_TCF==0)goto _TL199;_TD0=ad;{
struct Cyc_Absyn_AggrdeclImpl*_T1EF=_TD0->impl;struct Cyc_List_List*_T1F0;struct Cyc_List_List*_T1F1;struct Cyc_List_List*_T1F2;{struct Cyc_Absyn_AggrdeclImpl _T1F3=*_T1EF;_T1F2=_T1F3.qual_bnd;_T1F1=_T1F3.fields;_T1F0=_T1F3.effconstr;}{struct Cyc_List_List*qb=_T1F2;struct Cyc_List_List*fs=_T1F1;struct Cyc_List_List*fc=_T1F0;
Cyc_Binding_resolve_effconstr(loc,env,fc);
# 1023
Cyc_Binding_resolve_qualbnd(loc,env,qb);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}goto _TL19A;_TL199: _TL19A: goto _LL0;}}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T1EB;_TD1=
# 1029
Cyc_Binding_at_toplevel(env);if(_TD1)goto _TL19B;else{goto _TL19D;}
_TL19D:{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;_T1EC.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TD2=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_TD2;void*_T1ED[1];_T1ED[0]=& _T1EC;_TD3=loc;_TD4=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_TD3,_TD4);}goto _LL0;_TL19B: _TD5=loc;_TD6=env;_TD7=ed;_TD8=_TD7->name;_TD9=ed;_TDA=_TD9->sc;
# 1033
Cyc_Binding_absolutize_decl(_TD5,_TD6,_TD8,_TDA);_TDB=ed;{
struct _tuple0*_T1EC=_TDB->name;struct _fat_ptr*_T1ED;union Cyc_Absyn_Nmspace _T1EE;{struct _tuple0 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EE;struct _fat_ptr*decl_name=_T1ED;{
# 1036
struct Cyc_Binding_ResolveNSEnv*(*_T1EF)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TDC=_T1EF;}_TDD=env;_TDE=_TDD->ns;_TDF=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TDC(_TDE,_TDF);_TE1=Cyc_Dict_member;{
# 1038
int(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TE1;_TE0=_T1EF;}_TE2=decl_ns_data;_TE3=_TE2->enumdecls;_TE4=decl_name;_TE5=_TE0(_TE3,_TE4);if(!_TE5)goto _TL19E;_TE6=ed;_TE7=_TE6->fields;if(_TE7!=0)goto _TL19E;goto _LL0;_TL19E: _TE8=env;_TE9=_TE8->in_cyc_override;
# 1042
if(_TE9)goto _TL1A0;else{goto _TL1A2;}
_TL1A2: _TEA=decl_ns_data;_TEC=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))_TEC;_TEB=_T1EF;}_TED=decl_ns_data;_TEE=_TED->enumdecls;_TEF=decl_name;_TF0=ed;_TEA->enumdecls=_TEB(_TEE,_TEF,_TF0);goto _TL1A1;_TL1A0: _TL1A1: _TF1=ed;_TF2=_TF1->fields;
# 1045
if(_TF2==0)goto _TL1A3;_TF3=ed;_TF4=_TF3->fields;_TF5=_TF4->v;{
# 1047
struct Cyc_List_List*fs=(struct Cyc_List_List*)_TF5;_TL1A8: if(fs!=0)goto _TL1A6;else{goto _TL1A7;}
_TL1A6: _TF6=fs;_TF7=_TF6->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TF7;_TF8=f;_TF9=_TF8->loc;_TFA=env;_TFB=f;_TFC=_TFB->name;_TFD=ed;_TFE=_TFD->sc;
Cyc_Binding_absolutize_decl(_TF9,_TFA,_TFC,_TFE);_TFF=f;_T100=_TFF->tag;
if(_T100==0)goto _TL1A9;_T101=env;_T102=f;_T103=_T102->tag;
Cyc_Binding_resolve_exp(_T101,_T103);goto _TL1AA;_TL1A9: _TL1AA: _T104=env;_T105=_T104->in_cyc_override;
if(_T105)goto _TL1AB;else{goto _TL1AD;}
_TL1AD: _T106=decl_ns_data;_T108=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T108;_T107=_T1EF;}_T109=decl_ns_data;_T10A=_T109->ordinaries;_T10B=f;_T10C=_T10B->name;_T10D=*_T10C;_T10E=_T10D.f1;{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_EnumRes_Binding_Resolved_struct));_T1EF->tag=3;
# 1055
_T1EF->f1=ed;_T1EF->f2=f;_T10F=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T1EF;}_T110=(void*)_T10F;
# 1053
_T106->ordinaries=_T107(_T10A,_T10E,_T110);goto _TL1AC;_TL1AB: _TL1AC:;}_T111=fs;
# 1047
fs=_T111->tl;goto _TL1A8;_TL1A7:;}goto _TL1A4;_TL1A3: _TL1A4: goto _LL0;}}}}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T1EB;{struct _handler_cons _T1EC;_T112=& _T1EC;_push_handler(_T112);{int _T1ED=0;_T113=_T1EC.handler;_T114=setjmp(_T113);if(!_T114)goto _TL1AE;_T1ED=1;goto _TL1AF;_TL1AE: _TL1AF: if(_T1ED)goto _TL1B0;else{goto _TL1B2;}_TL1B2:{
# 1062
struct Cyc_List_List*(*_T1EE)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T115=_T1EE;}_T116=loc;_T117=env;_T118=_T117->ns;_T119=tud;_T11A=_T119->name;{struct Cyc_List_List*decls=_T115(_T116,_T118,_T11A,Cyc_Binding_lookup_datatypedecl);_T11B=decls;_T11C=_T11B->hd;{
struct Cyc_Absyn_Datatypedecl*last_decl=(struct Cyc_Absyn_Datatypedecl*)_T11C;_T11D=last_decl;_T11E=_T11D->is_extensible;
if(_T11E)goto _TL1B3;else{goto _TL1B5;}
_TL1B5:{struct Cyc_Binding_BindingError_exn_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_T1EE->tag=Cyc_Binding_BindingError;_T11F=(struct Cyc_Binding_BindingError_exn_struct*)_T1EE;}_T120=(void*)_T11F;_throw(_T120);goto _TL1B4;_TL1B3: _TL1B4: _T121=tud;_T122=last_decl;
_T121->name=_T122->name;_T123=tud;
_T123->is_extensible=1;_T124=tud;{
struct _tuple0*_T1EE=_T124->name;struct _fat_ptr*_T1EF;union Cyc_Absyn_Nmspace _T1F0;{struct _tuple0 _T1F1=*_T1EE;_T1F0=_T1F1.f0;_T1EF=_T1F1.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F0;struct _fat_ptr*decl_name=_T1EF;{
struct Cyc_Binding_ResolveNSEnv*(*_T1F1)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T125=_T1F1;}_T126=env;_T127=_T126->ns;_T128=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T125(_T127,_T128);_T129=decl_ns_data;_T12B=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F1)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T12B;_T12A=_T1F1;}_T12C=decl_ns_data;_T12D=_T12C->datatypedecls;_T12E=decl_name;{struct Cyc_List_List*_T1F1=_cycalloc(sizeof(struct Cyc_List_List));
_T1F1->hd=tud;_T1F1->tl=decls;_T12F=(struct Cyc_List_List*)_T1F1;}
# 1070
_T129->datatypedecls=_T12A(_T12D,_T12E,_T12F);}}}}}_pop_handler();goto _TL1B1;_TL1B0: _T130=Cyc_Core_get_exn_thrown();{void*_T1EE=(void*)_T130;void*_T1EF;_T131=(struct Cyc_Binding_BindingError_exn_struct*)_T1EE;_T132=_T131->tag;_T133=Cyc_Binding_BindingError;if(_T132!=_T133)goto _TL1B6;_T134=loc;_T135=env;_T136=tud;_T137=_T136->name;_T138=tud;_T139=_T138->sc;
# 1074
Cyc_Binding_absolutize_topdecl(_T134,_T135,_T137,_T139);_T13A=tud;{
struct _tuple0*_T1F0=_T13A->name;struct _fat_ptr*_T1F1;union Cyc_Absyn_Nmspace _T1F2;{struct _tuple0 _T1F3=*_T1F0;_T1F2=_T1F3.f0;_T1F1=_T1F3.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F2;struct _fat_ptr*decl_name=_T1F1;{
struct Cyc_Binding_ResolveNSEnv*(*_T1F3)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T13B=_T1F3;}_T13C=env;_T13D=_T13C->ns;_T13E=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T13B(_T13D,_T13E);_T140=Cyc_Dict_member;{
# 1078
int(*_T1F3)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T140;_T13F=_T1F3;}_T141=decl_ns_data;_T142=_T141->datatypedecls;_T143=decl_name;_T144=_T13F(_T142,_T143);if(!_T144)goto _TL1B8;_T145=tud;_T146=_T145->fields;if(_T146!=0)goto _TL1B8;goto _LL35;_TL1B8: _T147=decl_ns_data;_T149=Cyc_Dict_insert;{
# 1081
struct Cyc_Dict_Dict(*_T1F3)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T149;_T148=_T1F3;}_T14A=decl_ns_data;_T14B=_T14A->datatypedecls;_T14C=decl_name;{struct Cyc_List_List*_T1F3=_cycalloc(sizeof(struct Cyc_List_List));
_T1F3->hd=tud;_T1F3->tl=0;_T14D=(struct Cyc_List_List*)_T1F3;}
# 1081
_T147->datatypedecls=_T148(_T14B,_T14C,_T14D);goto _LL35;}}}_TL1B6: _T1EF=_T1EE;{void*exn=_T1EF;_rethrow(exn);}_LL35:;}_TL1B1:;}}_T14E=tud;{
# 1086
struct _tuple0*_T1EC=_T14E->name;struct _fat_ptr*_T1ED;union Cyc_Absyn_Nmspace _T1EE;{struct _tuple0 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EE;struct _fat_ptr*decl_name=_T1ED;_T14F=tud;_T150=_T14F->fields;
if(_T150==0)goto _TL1BA;_T151=tud;_T152=_T151->fields;_T153=_T152->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T153;_TL1BF: if(fs!=0)goto _TL1BD;else{goto _TL1BE;}
_TL1BD: _T154=fs;_T155=_T154->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T155;_T156=f;{
struct Cyc_List_List*tqts=_T156->typs;_TL1C3: if(tqts!=0)goto _TL1C1;else{goto _TL1C2;}
_TL1C1: _T157=loc;_T158=env;_T159=tqts;_T15A=_T159->hd;_T15B=(struct _tuple16*)_T15A;_T15C=*_T15B;_T15D=_T15C.f1;Cyc_Binding_resolve_type(_T157,_T158,_T15D);_T15E=tqts;
# 1090
tqts=_T15E->tl;goto _TL1C3;_TL1C2:;}_T15F=f;_T160=_T15F->name;_T161=*_T160;{
# 1092
union Cyc_Absyn_Nmspace _T1EF=_T161.f0;_T162=_T1EF.Abs_n;_T163=_T162.tag;switch(_T163){case 2: _T164=_T1EF.Rel_n;_T165=_T164.val;if(_T165!=0)goto _TL1C5;_T166=tud;_T167=_T166->is_extensible;
# 1094
if(!_T167)goto _TL1C7;_T168=f;_T169=_T168->name;_T16A=env;_T16B=_T16A->ns;_T16C=_T16B->curr_ns;
(*_T169).f0=Cyc_Absyn_Abs_n(_T16C,0);goto _TL1C8;
# 1097
_TL1C7: _T16D=f;_T16E=_T16D->name;_T16F=tud;_T170=_T16F->name;_T171=*_T170;(*_T16E).f0=_T171.f0;_TL1C8: goto _LL40;_TL1C5:{struct Cyc_Warn_String_Warn_Warg_struct _T1F0;_T1F0.tag=0;
# 1100
_T1F0.f1=_tag_fat("qualified datatype field declarations not allowed",sizeof(char),50U);_T172=_T1F0;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F0=_T172;void*_T1F1[1];_T1F1[0]=& _T1F0;_T173=f;_T174=_T173->loc;_T175=_tag_fat(_T1F1,sizeof(void*),1);Cyc_Warn_err2(_T174,_T175);}
return;case 1: goto _LL40;default: _T177=Cyc_Warn_impos;{
# 1103
int(*_T1F0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T177;_T176=_T1F0;}_T178=_tag_fat("datatype field Loc_n or C_n",sizeof(char),28U);_T179=_tag_fat(0U,sizeof(void*),0);_T176(_T178,_T179);}_LL40:;}{
# 1105
struct Cyc_Binding_ResolveNSEnv*(*_T1EF)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T17A=_T1EF;}_T17B=env;_T17C=_T17B->ns;_T17D=f;_T17E=_T17D->name;_T17F=*_T17E;_T180=_T17F.f0;{struct Cyc_Binding_ResolveNSEnv*ns_data=_T17A(_T17C,_T180);_T181=ns_data;_T183=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T183;_T182=_T1EF;}_T184=ns_data;_T185=_T184->ordinaries;_T186=f;_T187=_T186->name;_T188=*_T187;_T189=_T188.f1;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct));_T1EF->tag=2;
# 1108
_T1EF->f1=tud;_T1EF->f2=f;_T18A=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T1EF;}_T18B=(void*)_T18A;
# 1106
_T181->ordinaries=_T182(_T185,_T189,_T18B);}}_T18C=fs;
# 1088
fs=_T18C->tl;goto _TL1BF;_TL1BE:;}goto _TL1BB;_TL1BA: _TL1BB: goto _LL0;}}}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;_T1E8=_T1EC->f2;}{struct _fat_ptr*v=_T1EB;struct Cyc_List_List*ds2=_T1E8;{
# 1113
void(*_T1EC)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns;_T18D=_T1EC;}_T18E=env;_T18F=_T18E->ns;_T190=v;_T18D(_T18F,_T190,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);_T191=env;_T192=_T191->ns;
Cyc_Binding_leave_ns(_T192);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1E5;_T1EB=_T1EC->f1;_T1E8=_T1EC->f2;}{struct _tuple0*qv=_T1EB;struct Cyc_List_List*ds2=_T1E8;_T193=d;_T194=_T193->loc;_T195=env;_T196=_T195->ns;_T197=qv;
# 1118
Cyc_Binding_enter_using(_T194,_T196,_T197);
Cyc_Binding_resolve_decls(env,ds2);_T198=env;_T199=_T198->ns;
Cyc_Binding_leave_using(_T199);goto _LL0;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T1E5;_T1E8=_T1EC->f1;}{struct Cyc_List_List*ds2=_T1E8;_T19A=env;{
# 1124
int old=_T19A->in_cinclude;_T19B=env;
_T19B->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T19C=env;
_T19C->in_cinclude=old;goto _LL0;}}case 12:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1EC=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1E5;_T1E8=_T1EC->f1;_T1E7=_T1EC->f2;_T19D=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1E5;_T19E=& _T19D->f3;_T1EB=(struct Cyc_List_List**)_T19E;_T1E6=_T1EC->f4;}{struct Cyc_List_List*ds2=_T1E8;struct Cyc_List_List*ovrs=_T1E7;struct Cyc_List_List**exports=(struct Cyc_List_List**)_T1EB;struct _tuple10*hides=_T1E6;_T19F=exports;{
# 1135
struct Cyc_List_List*exs=*_T19F;_TL1CC: if(exs!=0)goto _TL1CA;else{goto _TL1CB;}
_TL1CA: _T1A0=exs;_T1A1=_T1A0->hd;{struct _tuple15*_T1EC=(struct _tuple15*)_T1A1;struct _tuple0*_T1ED;unsigned _T1EE;{struct _tuple15 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1ED=_T1EF.f1;}{unsigned loc=_T1EE;struct _tuple0*qv=_T1ED;
Cyc_Binding_absolutize_decl(loc,env,qv,4U);}}_T1A2=exs;
# 1135
exs=_T1A2->tl;goto _TL1CC;_TL1CB:;}_T1A3=
# 1143
Cyc_Binding_at_toplevel(env);if(_T1A3)goto _TL1CD;else{goto _TL1CF;}
_TL1CF:{struct Cyc_Warn_String_Warn_Warg_struct _T1EC;_T1EC.tag=0;_T1EC.f1=_tag_fat("extern \"C include\" not at toplevel",sizeof(char),35U);_T1A4=_T1EC;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EC=_T1A4;void*_T1ED[1];_T1ED[0]=& _T1EC;_T1A5=loc;_T1A6=_tag_fat(_T1ED,sizeof(void*),1);Cyc_Warn_err2(_T1A5,_T1A6);}goto _LL0;_TL1CD: _T1A8=Cyc_Dict_lookup;{
# 1147
struct Cyc_Binding_ResolveNSEnv*(*_T1EC)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1A8;_T1A7=_T1EC;}_T1A9=env;_T1AA=_T1A9->ns;_T1AB=_T1AA->ns_data;_T1AC=env;_T1AD=_T1AC->ns;_T1AE=_T1AD->curr_ns;{struct Cyc_Binding_ResolveNSEnv*data=_T1A7(_T1AB,_T1AE);_T1AF=data;{
struct Cyc_Dict_Dict old_dict=_T1AF->ordinaries;_T1B0=env;{
int old=_T1B0->in_cinclude;_T1B1=env;
_T1B1->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T1B2=env;
_T1B2->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);_T1B3=env;
_T1B3->in_cyc_override=0;_T1B4=data;{
struct Cyc_Dict_Dict new_dict=_T1B4->ordinaries;
struct Cyc_Dict_Dict out_dict=old_dict;_T1B5=hides;_T1B6=*_T1B5;_T1B7=_T1B6.f0;
if(_T1B7 <= 0U)goto _TL1D0;_T1B8=exports;_T1B9=*_T1B8;_T1BA=(unsigned)_T1B9;
if(!_T1BA)goto _TL1D2;_T1BB=hides;_T1BC=*_T1BB;_T1BD=_T1BC.f0;_T1BE=
_tag_fat("export wildcard expects empty export list",sizeof(char),42U);_T1BF=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1BD,_T1BE,_T1BF);goto _TL1D3;_TL1D2: _TL1D3: _T1C0=env;
_T1C0->in_cinclude=old;_T1C2=Cyc_Dict_iter_c;{
void(*_T1EC)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict))_T1C2;_T1C1=_T1EC;}{struct _tuple14*_T1EC=_cycalloc(sizeof(struct _tuple14));_T1EC->f0=exports;_T1C4=& out_dict;_T1EC->f1=(struct Cyc_Dict_Dict*)_T1C4;_T1EC->f2=env;_T1EC->f3=hides;_T1C3=(struct _tuple14*)_T1EC;}_T1C5=
Cyc_Dict_difference(new_dict,old_dict);
# 1161
_T1C1(Cyc_Binding_export_all_symbols,_T1C3,_T1C5);goto _TL1D1;
# 1164
_TL1D0: _T1C6=exports;{struct Cyc_List_List*exs=*_T1C6;_TL1D7: if(exs!=0)goto _TL1D5;else{goto _TL1D6;}
_TL1D5: _T1C7=exs;_T1C8=_T1C7->hd;{struct _tuple15*_T1EC=(struct _tuple15*)_T1C8;struct _fat_ptr*_T1ED;unsigned _T1EE;{struct _tuple15 _T1EF=*_T1EC;_T1EE=_T1EF.f0;_T1C9=_T1EF.f1;{struct _tuple0 _T1F0=*_T1C9;_T1ED=_T1F0.f1;}}{unsigned loc=_T1EE;struct _fat_ptr*v=_T1ED;_T1CB=Cyc_Dict_member;{
int(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1CB;_T1CA=_T1EF;}_T1CC=_T1CA(new_dict,v);if(_T1CC)goto _TL1DB;else{goto _TL1DA;}_TL1DB: _T1CE=Cyc_Dict_member;{
int(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1CE;_T1CD=_T1EF;}_T1CF=_T1CD(old_dict,v);
# 1166
if(_T1CF)goto _TL1DC;else{goto _TL1D8;}_TL1DC: _T1D1=Cyc_Dict_lookup;{
# 1168
void*(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D1;_T1D0=_T1EF;}_T1D2=_T1D0(old_dict,v);_T1D4=Cyc_Dict_lookup;{void*(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D4;_T1D3=_T1EF;}_T1D5=_T1D3(new_dict,v);
# 1166
if(_T1D2==_T1D5)goto _TL1DA;else{goto _TL1D8;}
# 1169
_TL1DA:{struct Cyc_String_pa_PrintArg_struct _T1EF;_T1EF.tag=0;_T1D7=v;_T1EF.f1=*_T1D7;_T1D6=_T1EF;}{struct Cyc_String_pa_PrintArg_struct _T1EF=_T1D6;void*_T1F0[1];_T1F0[0]=& _T1EF;_T1D8=loc;_T1D9=_tag_fat("%s is exported but not defined",sizeof(char),31U);_T1DA=_tag_fat(_T1F0,sizeof(void*),1);Cyc_Warn_err(_T1D8,_T1D9,_T1DA);}goto _TL1D9;_TL1D8: _TL1D9: _T1DC=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T1DC;_T1DB=_T1EF;}_T1DD=out_dict;_T1DE=v;_T1E0=Cyc_Dict_lookup;{void*(*_T1EF)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1E0;_T1DF=_T1EF;}_T1E1=_T1DF(new_dict,v);out_dict=_T1DB(_T1DD,_T1DE,_T1E1);}}_T1E2=exs;
# 1164
exs=_T1E2->tl;goto _TL1D7;_TL1D6:;}_TL1D1: _T1E3=data;
# 1173
_T1E3->ordinaries=out_dict;_T1E4=env;
_T1E4->in_cinclude=old;goto _LL0;}}}}}case 13: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: goto _LL0;}_LL0:;}}}
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
void Cyc_Binding_interproc_binding_handler(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;struct Cyc_Warn_Qvar_Warn_Warg_struct _T9;struct Cyc_Absyn_Exp*_TA;unsigned _TB;struct _fat_ptr _TC;void*_TD;int _TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;int _T11;struct Cyc_List_List*_T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Exp_Warn_Warg_struct _T15;int(*_T16)(struct _fat_ptr);void*(*_T17)(struct _fat_ptr);struct _fat_ptr _T18;_T0=e;{
void*_T19=_T0->r;void*_T1A;_T1=(int*)_T19;_T2=*_T1;if(_T2!=1)goto _TL1E1;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T19;_T3=_T1B->f1;_T1A=(void*)_T3;}{void*b=_T1A;{struct Cyc_Absyn_Vardecl*_T1B;struct _tuple0*_T1C;_T4=b;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 0: _T7=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T7;_T1C=_T1D->f1;}{struct _tuple0*q=_T1C;{struct Cyc_Warn_String_Warn_Warg_struct _T1D;_T1D.tag=0;
# 1206
_T1D.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T8=_T1D;}{struct Cyc_Warn_String_Warn_Warg_struct _T1D=_T8;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E;_T1E.tag=1;_T1E.f1=q;_T9=_T1E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E=_T9;void*_T1F[2];_T1F[0]=& _T1D;_T1F[1]=& _T1E;_TA=e;_TB=_TA->loc;_TC=_tag_fat(_T1F,sizeof(void*),2);Cyc_Warn_err2(_TB,_TC);}}
return;}case 1: _TD=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TD;_T1B=_T1D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1B;_TE=Cyc_Binding_enable_interproc;
# 1209
if(!_TE)goto _TL1E4;_TF=vd;_T10=_TF->type;_T11=Cyc_Tcutil_is_function_type(_T10);if(!_T11)goto _TL1E4;{struct Cyc_List_List*_T1D=_cycalloc(sizeof(struct Cyc_List_List));
_T1D->hd=e;_T1D->tl=Cyc_Binding_extern_functions;_T12=(struct Cyc_List_List*)_T1D;}Cyc_Binding_extern_functions=_T12;goto _TL1E5;_TL1E4: _TL1E5: goto _LL5;}case 2: goto _LLD;case 3: _LLD: goto _LLF;case 4: _LLF: goto _LL11;default: _LL11: goto _LL5;}_LL5:;}goto _LL0;}_TL1E1: _T13=Cyc_Binding_enable_interproc;
# 1221
if(!_T13)goto _TL1E6;{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
_T1B.f1=_tag_fat("Interproc binding got unsupported expression: ",sizeof(char),47U);_T14=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T14;{struct Cyc_Warn_Exp_Warn_Warg_struct _T1C;_T1C.tag=4;_T1C.f1=e;_T15=_T1C;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T1C=_T15;void*_T1D[2];_T1D[0]=& _T1B;_T1D[1]=& _T1C;_T17=Cyc_Warn_impos2;{int(*_T1E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T17;_T16=_T1E;}_T18=_tag_fat(_T1D,sizeof(void*),2);_T16(_T18);}}goto _TL1E7;_TL1E6: _TL1E7: goto _LL0;_LL0:;}}
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
