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
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_SubsetType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_AssnDef_ExistAssnFn*f3;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 551 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};
# 566
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 734 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 921
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 923
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 932
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 942
void*Cyc_Absyn_compress(void*);
# 969
extern void*Cyc_Absyn_heap_rgn_type;
# 971
extern void*Cyc_Absyn_al_qual_type;
# 975
extern void*Cyc_Absyn_false_type;
# 977
extern void*Cyc_Absyn_void_type;
# 999
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1030
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1043
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1162
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1171
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1244
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
static void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){struct _tuple0*_T0;union Cyc_Absyn_Nmspace _T1;struct _union_Nmspace_Abs_n _T2;unsigned _T3;union Cyc_Absyn_Nmspace _T4;struct _union_Nmspace_Rel_n _T5;struct Cyc_List_List*_T6;struct _handler_cons*_T7;int _T8;void*(*_T9)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_TA)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_TB;struct Cyc_Dict_Dict _TC;struct Cyc_Binding_NSCtxt*_TD;struct Cyc_List_List*_TE;void*_TF;struct _fat_ptr*_T10;void*_T11;struct Cyc_Dict_Absent_exn_struct*_T12;char*_T13;char*_T14;struct Cyc_Binding_NSCtxt*_T15;struct Cyc_List_List*_T16;int*_T17;int _T18;struct _handler_cons*_T19;int _T1A;void*(*_T1B)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1C)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T1D;struct Cyc_Dict_Dict _T1E;struct Cyc_List_List*_T1F;void*_T20;struct _fat_ptr*_T21;void*_T22;struct Cyc_Dict_Absent_exn_struct*_T23;char*_T24;char*_T25;struct Cyc_List_List*_T26;struct Cyc_Binding_BindingError_exn_struct*_T27;void*_T28;union Cyc_Absyn_Nmspace _T29;struct _union_Nmspace_Rel_n _T2A;struct _handler_cons*_T2B;int _T2C;void*(*_T2D)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T2E)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T2F;struct Cyc_Dict_Dict _T30;struct Cyc_List_List*_T31;void*_T32;struct _fat_ptr*_T33;void*_T34;struct Cyc_Dict_Absent_exn_struct*_T35;char*_T36;char*_T37;struct Cyc_Binding_BindingError_exn_struct*_T38;void*_T39;union Cyc_Absyn_Nmspace _T3A;struct _union_Nmspace_C_n _T3B;union Cyc_Absyn_Nmspace _T3C;struct _union_Nmspace_Abs_n _T3D;struct _handler_cons*_T3E;int _T3F;void*(*_T40)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T41)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_NSCtxt*_T42;struct Cyc_Dict_Dict _T43;struct Cyc_List_List*_T44;void*_T45;struct _fat_ptr*_T46;void*_T47;struct Cyc_Dict_Absent_exn_struct*_T48;char*_T49;char*_T4A;struct Cyc_Binding_BindingError_exn_struct*_T4B;void*_T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;struct _fat_ptr*_T4F;union Cyc_Absyn_Nmspace _T50;_T0=qv;{struct _tuple0 _T51=*_T0;_T50=_T51.f0;_T4F=_T51.f1;}{union Cyc_Absyn_Nmspace ns=_T50;struct _fat_ptr*v=_T4F;struct Cyc_List_List*_T51;_T1=ns;_T2=_T1.Abs_n;_T3=_T2.tag;switch(_T3){case 2: _T4=ns;_T5=_T4.Rel_n;_T6=_T5.val;if(_T6!=0)goto _TLC;{struct _handler_cons _T52;_T7=& _T52;_push_handler(_T7);{int _T53=0;_T8=setjmp(_T52.handler);if(!_T8)goto _TLE;_T53=1;goto _TLF;_TLE: _TLF: if(_T53)goto _TL10;else{goto _TL12;}_TL12: _TA=Cyc_Dict_lookup;{
# 108
void*(*_T54)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_TA;_T9=_T54;}_TB=ctxt;_TC=_TB->ns_data;_TD=ctxt;_TE=_TD->curr_ns;_TF=_T9(_TC,_TE);_T10=v;{void*_T54=lookup(_TF,_T10);_npop_handler(0);return _T54;}_pop_handler();goto _TL11;_TL10: _T11=Cyc_Core_get_exn_thrown();{void*_T54=(void*)_T11;void*_T55;_T12=(struct Cyc_Dict_Absent_exn_struct*)_T54;_T13=_T12->tag;_T14=Cyc_Dict_Absent;if(_T13!=_T14)goto _TL13;goto _LLE;_TL13: _T55=_T54;{void*exn=_T55;_rethrow(exn);}_LLE:;}_TL11:;}}_T15=ctxt;{
# 110
struct Cyc_List_List*as=_T15->availables;_TL18: if(as!=0)goto _TL16;else{goto _TL17;}
_TL16: _T16=as;{void*_T52=_T16->hd;struct Cyc_List_List*_T53;_T17=(int*)_T52;_T18=*_T17;if(_T18!=1)goto _TL19;{struct Cyc_Binding_Using_Binding_NSDirective_struct*_T54=(struct Cyc_Binding_Using_Binding_NSDirective_struct*)_T52;_T53=_T54->f1;}{struct Cyc_List_List*ns=_T53;_T53=ns;goto _LL17;}_TL19:{struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_T54=(struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_T52;_T53=_T54->f1;}_LL17:{struct Cyc_List_List*ns=_T53;{struct _handler_cons _T54;_T19=& _T54;_push_handler(_T19);{int _T55=0;_T1A=setjmp(_T54.handler);if(!_T1A)goto _TL1B;_T55=1;goto _TL1C;_TL1B: _TL1C: if(_T55)goto _TL1D;else{goto _TL1F;}_TL1F: _T1C=Cyc_Dict_lookup;{
# 114
void*(*_T56)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1C;_T1B=_T56;}_T1D=ctxt;_T1E=_T1D->ns_data;_T1F=ns;_T20=_T1B(_T1E,_T1F);_T21=v;{void*_T56=lookup(_T20,_T21);_npop_handler(0);return _T56;}_pop_handler();goto _TL1E;_TL1D: _T22=Cyc_Core_get_exn_thrown();{void*_T56=(void*)_T22;void*_T57;_T23=(struct Cyc_Dict_Absent_exn_struct*)_T56;_T24=_T23->tag;_T25=Cyc_Dict_Absent;if(_T24!=_T25)goto _TL20;goto _LL18;_TL20: _T57=_T56;{void*exn=_T57;_rethrow(exn);}_LL18:;}_TL1E:;}}goto _LL13;}_LL13:;}_T26=as;
# 110
as=_T26->tl;goto _TL18;_TL17:;}{struct Cyc_Binding_BindingError_exn_struct*_T52=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
# 118
_T52->tag=Cyc_Binding_BindingError;_T27=(struct Cyc_Binding_BindingError_exn_struct*)_T52;}_T28=(void*)_T27;_throw(_T28);goto _TLD;_TLC: _T29=ns;_T2A=_T29.Rel_n;_T51=_T2A.val;{struct Cyc_List_List*ns=_T51;struct _handler_cons _T52;_T2B=& _T52;_push_handler(_T2B);{int _T53=0;_T2C=setjmp(_T52.handler);if(!_T2C)goto _TL22;_T53=1;goto _TL23;_TL22: _TL23: if(_T53)goto _TL24;else{goto _TL26;}_TL26:{
# 121
struct Cyc_List_List*abs_ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);_T2E=Cyc_Dict_lookup;{
void*(*_T54)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T2E;_T2D=_T54;}_T2F=ctxt;_T30=_T2F->ns_data;_T31=abs_ns;_T32=_T2D(_T30,_T31);_T33=v;{void*_T54=lookup(_T32,_T33);_npop_handler(0);return _T54;}}_pop_handler();goto _TL25;_TL24: _T34=Cyc_Core_get_exn_thrown();{void*_T54=(void*)_T34;void*_T55;_T35=(struct Cyc_Dict_Absent_exn_struct*)_T54;_T36=_T35->tag;_T37=Cyc_Dict_Absent;if(_T36!=_T37)goto _TL27;{struct Cyc_Binding_BindingError_exn_struct*_T56=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T56->tag=Cyc_Binding_BindingError;_T38=(struct Cyc_Binding_BindingError_exn_struct*)_T56;}_T39=(void*)_T38;_throw(_T39);goto _TL28;_TL27: _T55=_T54;{void*exn=_T55;_rethrow(exn);}_TL28:;}_TL25:;}}_TLD:;case 3: _T3A=ns;_T3B=_T3A.C_n;_T51=_T3B.val;{struct Cyc_List_List*ns=_T51;_T51=ns;goto _LLB;}case 1: _T3C=ns;_T3D=_T3C.Abs_n;_T51=_T3D.val;_LLB: {struct Cyc_List_List*ns=_T51;struct _handler_cons _T52;_T3E=& _T52;_push_handler(_T3E);{int _T53=0;_T3F=setjmp(_T52.handler);if(!_T3F)goto _TL29;_T53=1;goto _TL2A;_TL29: _TL2A: if(_T53)goto _TL2B;else{goto _TL2D;}_TL2D: _T41=Cyc_Dict_lookup;{
# 126
void*(*_T54)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T41;_T40=_T54;}_T42=ctxt;_T43=_T42->ns_data;_T44=ns;_T45=_T40(_T43,_T44);_T46=v;{void*_T54=lookup(_T45,_T46);_npop_handler(0);return _T54;}_pop_handler();goto _TL2C;_TL2B: _T47=Cyc_Core_get_exn_thrown();{void*_T54=(void*)_T47;void*_T55;_T48=(struct Cyc_Dict_Absent_exn_struct*)_T54;_T49=_T48->tag;_T4A=Cyc_Dict_Absent;if(_T49!=_T4A)goto _TL2E;{struct Cyc_Binding_BindingError_exn_struct*_T56=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));
_T56->tag=Cyc_Binding_BindingError;_T4B=(struct Cyc_Binding_BindingError_exn_struct*)_T56;}_T4C=(void*)_T4B;_throw(_T4C);goto _TL2F;_TL2E: _T55=_T54;{void*exn=_T55;_rethrow(exn);}_TL2F:;}_TL2C:;}}default: _T4D=
_tag_fat("lookup local in global",sizeof(char),23U);_T4E=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_impos(_T4D,_T4E);};}}
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
static void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){struct _handler_cons*_T0;int _T1;int*_T2;unsigned _T3;void*_T4;int _T5;void*_T6;int*_T7;unsigned _T8;int(*_T9)(struct _fat_ptr,struct _fat_ptr);void*(*_TA)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;unsigned _TF;struct _fat_ptr _T10;void*_T11;struct Cyc_Binding_BindingError_exn_struct*_T12;char*_T13;char*_T14;
struct _fat_ptr hides_what;struct _handler_cons _T15;_T0=& _T15;_push_handler(_T0);{int _T16=0;_T1=setjmp(_T15.handler);if(!_T1)goto _TL50;_T16=1;goto _TL51;_TL50: _TL51: if(_T16)goto _TL52;else{goto _TL54;}_TL54:{
# 313
void*_T17=Cyc_Binding_lookup_ordinary(loc,env,q);void*_T18;_T2=(int*)_T17;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T19=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T17;_T4=_T19->f1;_T18=(void*)_T4;}{void*b=_T18;_T5=
# 315
Cyc_Binding_at_toplevel(env);if(!_T5)goto _TL56;_npop_handler(0);return;_TL56: _T6=b;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 1:
# 318
 hides_what=_tag_fat("global variable",sizeof(char),16U);goto _LLB;case 4:
 hides_what=_tag_fat("local variable",sizeof(char),15U);goto _LLB;case 3:
 hides_what=_tag_fat("parameter",sizeof(char),10U);goto _LLB;case 5:
 hides_what=_tag_fat("pattern variable",sizeof(char),17U);goto _LLB;case 2:
 hides_what=_tag_fat("function",sizeof(char),9U);goto _LLB;default: _TA=Cyc_Warn_impos;{
int(*_T19)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TA;_T9=_T19;}_TB=_tag_fat("shadowing free variable!",sizeof(char),25U);_TC=_tag_fat(0U,sizeof(void*),0);_T9(_TB,_TC);}_LLB: goto _LL0;}case 1:
# 326
 hides_what=_tag_fat("struct constructor",sizeof(char),19U);goto _LL0;case 2:
 hides_what=_tag_fat("datatype constructor",sizeof(char),21U);goto _LL0;case 3: goto _LLA;default: _LLA:
# 329
 hides_what=_tag_fat("enum tag",sizeof(char),9U);goto _LL0;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
# 331
_T17.f1=_tag_fat("declaration hides ",sizeof(char),19U);_TD=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;_T18.f1=hides_what;_TE=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_TE;void*_T19[2];_T19[0]=& _T17;_T19[1]=& _T18;_TF=loc;_T10=_tag_fat(_T19,sizeof(void*),2);Cyc_Warn_warn2(_TF,_T10);}}_npop_handler(0);return;_pop_handler();goto _TL53;_TL52: _T11=Cyc_Core_get_exn_thrown();{void*_T17=(void*)_T11;void*_T18;_T12=(struct Cyc_Binding_BindingError_exn_struct*)_T17;_T13=_T12->tag;_T14=Cyc_Binding_BindingError;if(_T13!=_T14)goto _TL59;
# 333
return;_TL59: _T18=_T17;{void*exn=_T18;_rethrow(exn);};}_TL53:;}}
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
static int Cyc_Binding_resolve_visit_f1(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void**_T4;void**_T5;int*_T6;int _T7;struct _handler_cons*_T8;int _T9;struct Cyc_Absyn_Exp*_TA;unsigned _TB;struct Cyc_Binding_Env*_TC;struct _tuple0*_TD;int*_TE;unsigned _TF;void*_T10;void**_T11;struct Cyc_Absyn_Exp*_T12;unsigned _T13;struct Cyc_Binding_Env*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T19;void*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E;struct Cyc_Absyn_Exp*_T1F;unsigned _T20;struct _fat_ptr _T21;void*_T22;struct Cyc_Binding_BindingError_exn_struct*_T23;char*_T24;char*_T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Warn_Qvar_Warn_Warg_struct _T27;struct Cyc_Absyn_Exp*_T28;unsigned _T29;struct _fat_ptr _T2A;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T2B;int*_T2C;int*_T2D;struct Cyc_Binding_Env*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_List_List*_T32;struct Cyc_Absyn_Exp*_T33;int*_T34;int _T35;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T36;void**_T37;void**_T38;int*_T39;int _T3A;struct _handler_cons*_T3B;int _T3C;struct Cyc_Absyn_Exp*_T3D;unsigned _T3E;struct Cyc_Binding_Env*_T3F;struct _tuple0*_T40;int*_T41;unsigned _T42;void*_T43;void**_T44;struct Cyc_Absyn_Exp*_T45;unsigned _T46;struct Cyc_Binding_Env*_T47;struct Cyc_Absyn_Exp*_T48;struct Cyc_List_List*_T49;struct _tuple11*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T4F;struct Cyc_Absyn_Aggrdecl*_T50;struct Cyc_Absyn_Datatypefield*_T51;struct Cyc_List_List*_T52;struct Cyc_Warn_Qvar_Warn_Warg_struct _T53;struct Cyc_Absyn_Datatypefield*_T54;struct Cyc_Warn_String_Warn_Warg_struct _T55;struct Cyc_Absyn_Exp*_T56;unsigned _T57;struct _fat_ptr _T58;struct Cyc_Absyn_Exp*_T59;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T5A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T5B;struct Cyc_Warn_String_Warn_Warg_struct _T5C;struct Cyc_Absyn_Exp*_T5D;unsigned _T5E;struct _fat_ptr _T5F;void*_T60;struct Cyc_Binding_BindingError_exn_struct*_T61;char*_T62;char*_T63;struct Cyc_Absyn_Exp*_T64;unsigned _T65;struct Cyc_Binding_Env*_T66;struct Cyc_Absyn_Exp*_T67;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T68;struct _tuple0**_T69;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T6A;struct Cyc_Absyn_Aggrdecl**_T6B;struct _handler_cons*_T6C;int _T6D;struct Cyc_Absyn_Aggrdecl**_T6E;struct Cyc_Absyn_Aggrdecl*(*_T6F)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));struct Cyc_Absyn_Exp*_T70;unsigned _T71;struct Cyc_Binding_Env*_T72;struct Cyc_Binding_NSCtxt*_T73;struct _tuple0**_T74;struct _tuple0*_T75;struct _tuple0**_T76;struct Cyc_Absyn_Aggrdecl**_T77;struct Cyc_Absyn_Aggrdecl*_T78;struct Cyc_Absyn_Aggrdecl*_T79;void*_T7A;struct Cyc_Binding_BindingError_exn_struct*_T7B;char*_T7C;char*_T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T7F;struct _tuple0**_T80;struct Cyc_Absyn_Exp*_T81;unsigned _T82;struct _fat_ptr _T83;int _T84;struct Cyc_Binding_Env*_T85;struct Cyc_Dict_Dict*_T86;struct Cyc_Dict_Dict(*_T87)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T88)(int(*)(void*,void*));int(*_T89)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Absyn_Exp*_T8A;unsigned _T8B;struct Cyc_Binding_Env*_T8C;struct Cyc_Absyn_Vardecl*_T8D;struct Cyc_Binding_Env*_T8E;struct Cyc_Binding_Env*_T8F;struct Cyc_Dict_Dict*_T90;struct Cyc_Dict_Dict*_T91;struct Cyc_Absyn_Exp*_T92;unsigned _T93;struct Cyc_Binding_Env*_T94;struct Cyc_Absyn_Vardecl*_T95;struct Cyc_Binding_Env*_T96;struct Cyc_Dict_Dict*_T97;struct Cyc_Dict_Dict*_T98;struct Cyc_Absyn_Exp*_T99;unsigned _T9A;struct Cyc_Binding_Env*_T9B;struct Cyc_List_List*_T9C;void*_T9D;struct Cyc_List_List*_T9E;struct Cyc_Absyn_MallocInfo _T9F;void**_TA0;void*_TA1;void*_TA2;struct _tuple8*_TA3;void*_TA4;void*_TA5;void*_TA6;void*_TA7;void*_TA8;struct Cyc_Absyn_Exp*_TA9;unsigned _TAA;struct Cyc_Binding_Env*_TAB;void*_TAC;_T0=e;{
void*_TAD=_T0->r;struct Cyc_Absyn_Exp*_TAE;struct Cyc_Absyn_Exp*_TAF;struct Cyc_List_List*_TB0;void*_TB1;struct Cyc_List_List*_TB2;void*_TB3;_T1=(int*)_TAD;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAD;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TAD;_T4=& _T3->f1;_TB3=(void**)_T4;}{void**b=_TB3;_T5=b;{
# 358
void*_TB4=*_T5;struct _tuple0*_TB5;_T6=(int*)_TB4;_T7=*_T6;if(_T7!=0)goto _TL64;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TB6=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TB4;_TB5=_TB6->f1;}{struct _tuple0*q=_TB5;{struct _handler_cons _TB6;_T8=& _TB6;_push_handler(_T8);{int _TB7=0;_T9=setjmp(_TB6.handler);if(!_T9)goto _TL66;_TB7=1;goto _TL67;_TL66: _TL67: if(_TB7)goto _TL68;else{goto _TL6A;}_TL6A: _TA=e;_TB=_TA->loc;_TC=env;_TD=q;{
# 360
void*_TB8=Cyc_Binding_lookup_ordinary(_TB,_TC,_TD);struct Cyc_Absyn_Aggrdecl*_TB9;struct Cyc_Absyn_Enumfield*_TBA;struct Cyc_Absyn_Datatypefield*_TBB;void*_TBC;_TE=(int*)_TB8;_TF=*_TE;switch(_TF){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TBD=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TB8;_T10=_TBD->f1;_TBC=(void*)_T10;}{void*bnd=_TBC;_T11=b;
*_T11=bnd;_T12=e;_T13=_T12->loc;_T14=env;_T15=e;Cyc_Binding_interproc_binding_handler(_T13,_T14,_T15);goto _LL24;}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TBD=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TB8;_TBC=_TBD->f1;_TBB=_TBD->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TBC;struct Cyc_Absyn_Datatypefield*tuf=_TBB;_T16=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TBD=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TBD->tag=30;
_TBD->f1=0;_TBD->f2=tud;_TBD->f3=tuf;_T17=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TBD;}_T16->r=(void*)_T17;goto _LL24;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_TBD=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_TB8;_TBC=_TBD->f1;_TBA=_TBD->f2;}{struct Cyc_Absyn_Enumdecl*ed=_TBC;struct Cyc_Absyn_Enumfield*ef=_TBA;_T18=e;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_TBD=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_TBD->tag=31;
_TBD->f1=ed;_TBD->f2=ef;_T19=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_TBD;}_T18->r=(void*)_T19;goto _LL24;}case 4:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TBD=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_TB8;_T1A=_TBD->f1;_TBC=(void*)_T1A;_TBA=_TBD->f2;}{void*t=_TBC;struct Cyc_Absyn_Enumfield*ef=_TBA;_T1B=e;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TBD=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_TBD->tag=32;
_TBD->f1=t;_TBD->f2=ef;_T1C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TBD;}_T1B->r=(void*)_T1C;goto _LL24;}default:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TBD=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TB8;_TB9=_TBD->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TB9;{struct Cyc_Warn_String_Warn_Warg_struct _TBD;_TBD.tag=0;
_TBD.f1=_tag_fat("bad occurrence of type name ",sizeof(char),29U);_T1D=_TBD;}{struct Cyc_Warn_String_Warn_Warg_struct _TBD=_T1D;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBE;_TBE.tag=1;_TBE.f1=q;_T1E=_TBE;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBE=_T1E;void*_TBF[2];_TBF[0]=& _TBD;_TBF[1]=& _TBE;_T1F=e;_T20=_T1F->loc;_T21=_tag_fat(_TBF,sizeof(void*),2);Cyc_Warn_err2(_T20,_T21);}}goto _LL24;}}_LL24:;}_pop_handler();goto _TL69;_TL68: _T22=Cyc_Core_get_exn_thrown();{void*_TB8=(void*)_T22;void*_TB9;_T23=(struct Cyc_Binding_BindingError_exn_struct*)_TB8;_T24=_T23->tag;_T25=Cyc_Binding_BindingError;if(_T24!=_T25)goto _TL6C;{struct Cyc_Warn_String_Warn_Warg_struct _TBA;_TBA.tag=0;
# 368
_TBA.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T26=_TBA;}{struct Cyc_Warn_String_Warn_Warg_struct _TBA=_T26;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBB;_TBB.tag=1;_TBB.f1=q;_T27=_TBB;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBB=_T27;void*_TBC[2];_TBC[0]=& _TBA;_TBC[1]=& _TBB;_T28=e;_T29=_T28->loc;_T2A=_tag_fat(_TBC,sizeof(void*),2);Cyc_Warn_err2(_T29,_T2A);}}goto _LL2F;_TL6C: _TB9=_TB8;{void*exn=_TB9;_rethrow(exn);}_LL2F:;}_TL69:;}}
# 370
return 0;}_TL64:
 return 0;;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TAD;_TB3=_TB4->f1;_TB2=_TB4->f2;_T2B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TAD;_T2C=& _T2B->f4;_TB1=(int*)_T2C;}{struct Cyc_Absyn_Exp*e1=_TB3;struct Cyc_List_List*es=_TB2;int*b=_TB1;_T2D=b;
# 374
*_T2D=1;{
# 376
struct Cyc_List_List*es2=es;_TL71: if(es2!=0)goto _TL6F;else{goto _TL70;}
_TL6F: _T2E=env;_T2F=es2;_T30=_T2F->hd;_T31=(struct Cyc_Absyn_Exp*)_T30;Cyc_Binding_resolve_exp(_T2E,_T31);_T32=es2;
# 376
es2=_T32->tl;goto _TL71;_TL70:;}_T33=e1;{
# 378
void*_TB4=_T33->r;void*_TB5;_T34=(int*)_TB4;_T35=*_T34;if(_T35!=1)goto _TL72;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TB6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB4;_T36=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TB4;_T37=& _T36->f1;_TB5=(void**)_T37;}{void**b=(void**)_TB5;_T38=b;{
# 380
void*_TB6=*_T38;struct _tuple0*_TB7;_T39=(int*)_TB6;_T3A=*_T39;if(_T3A!=0)goto _TL74;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TB8=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_TB6;_TB7=_TB8->f1;}{struct _tuple0*q=_TB7;struct _handler_cons _TB8;_T3B=& _TB8;_push_handler(_T3B);{int _TB9=0;_T3C=setjmp(_TB8.handler);if(!_T3C)goto _TL76;_TB9=1;goto _TL77;_TL76: _TL77: if(_TB9)goto _TL78;else{goto _TL7A;}_TL7A: _T3D=e1;_T3E=_T3D->loc;_T3F=env;_T40=q;{
# 382
void*_TBA=Cyc_Binding_lookup_ordinary(_T3E,_T3F,_T40);struct Cyc_Absyn_Datatypefield*_TBB;struct Cyc_Absyn_Datatypedecl*_TBC;struct Cyc_Absyn_Aggrdecl*_TBD;void*_TBE;_T41=(int*)_TBA;_T42=*_T41;switch(_T42){case 0:{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TBF=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TBA;_T43=_TBF->f1;_TBE=(void*)_T43;}{void*bnd=_TBE;_T44=b;
# 384
*_T44=bnd;_T45=e1;_T46=_T45->loc;_T47=env;_T48=e1;
Cyc_Binding_interproc_binding_handler(_T46,_T47,_T48);{int _TBF=0;_npop_handler(0);return _TBF;}}case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TBF=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TBA;_TBD=_TBF->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TBD;
# 388
struct Cyc_List_List*dles=0;
_TL7F: if(es!=0)goto _TL7D;else{goto _TL7E;}
_TL7D:{struct Cyc_List_List*_TBF=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_TC0=_cycalloc(sizeof(struct _tuple11));_TC0->f0=0;_T4B=es;_T4C=_T4B->hd;_TC0->f1=(struct Cyc_Absyn_Exp*)_T4C;_T4A=(struct _tuple11*)_TC0;}_TBF->hd=_T4A;_TBF->tl=dles;_T49=(struct Cyc_List_List*)_TBF;}dles=_T49;_T4D=es;
# 389
es=_T4D->tl;goto _TL7F;_TL7E: _T4E=e;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TBF=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TBF->tag=28;_T50=ad;
# 391
_TBF->f1=_T50->name;_TBF->f2=0;_TBF->f3=Cyc_List_imp_rev(dles);_TBF->f4=ad;_T4F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TBF;}_T4E->r=(void*)_T4F;{int _TBF=0;_npop_handler(0);return _TBF;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TBF=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TBA;_TBC=_TBF->f1;_TBB=_TBF->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_TBC;struct Cyc_Absyn_Datatypefield*tuf=_TBB;_T51=tuf;_T52=_T51->typs;
# 394
if(_T52!=0)goto _TL80;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBF;_TBF.tag=1;_T54=tuf;
_TBF.f1=_T54->name;_T53=_TBF;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBF=_T53;{struct Cyc_Warn_String_Warn_Warg_struct _TC0;_TC0.tag=0;_TC0.f1=_tag_fat(" is a constant, not a function",sizeof(char),31U);_T55=_TC0;}{struct Cyc_Warn_String_Warn_Warg_struct _TC0=_T55;void*_TC1[2];_TC1[0]=& _TBF;_TC1[1]=& _TC0;_T56=e;_T57=_T56->loc;_T58=_tag_fat(_TC1,sizeof(void*),2);Cyc_Warn_err2(_T57,_T58);}}goto _TL81;_TL80: _TL81: _T59=e;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TBF=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TBF->tag=30;
_TBF->f1=es;_TBF->f2=tud;_TBF->f3=tuf;_T5A=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TBF;}_T59->r=(void*)_T5A;{int _TBF=0;_npop_handler(0);return _TBF;}}case 4: goto _LL48;default: _LL48:{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBF;_TBF.tag=1;
# 400
_TBF.f1=q;_T5B=_TBF;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TBF=_T5B;{struct Cyc_Warn_String_Warn_Warg_struct _TC0;_TC0.tag=0;_TC0.f1=_tag_fat(" is an enum constructor, not a function",sizeof(char),40U);_T5C=_TC0;}{struct Cyc_Warn_String_Warn_Warg_struct _TC0=_T5C;void*_TC1[2];_TC1[0]=& _TBF;_TC1[1]=& _TC0;_T5D=e;_T5E=_T5D->loc;_T5F=_tag_fat(_TC1,sizeof(void*),2);Cyc_Warn_err2(_T5E,_T5F);}}{int _TBF=0;_npop_handler(0);return _TBF;}};}_pop_handler();goto _TL79;_TL78: _T60=Cyc_Core_get_exn_thrown();{void*_TBA=(void*)_T60;void*_TBB;_T61=(struct Cyc_Binding_BindingError_exn_struct*)_TBA;_T62=_T61->tag;_T63=Cyc_Binding_BindingError;if(_T62!=_T63)goto _TL82;_T64=e1;_T65=_T64->loc;_T66=env;_T67=e1;
# 403
Cyc_Binding_interproc_binding_handler(_T65,_T66,_T67);
return 0;_TL82: _TBB=_TBA;{void*exn=_TBB;_rethrow(exn);};}_TL79:;}}goto _TL75;_TL74:
# 406
 return 0;_TL75:;}}goto _TL73;_TL72:
# 408
 Cyc_Binding_resolve_exp(env,e1);return 0;_TL73:;}}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TAD;_T68=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TAD;_T69=& _T68->f1;_TB3=(struct _tuple0**)_T69;_TB2=_TB4->f2;_TB0=_TB4->f3;_T6A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TAD;_T6B=& _T6A->f4;_TB1=(struct Cyc_Absyn_Aggrdecl**)_T6B;}{struct _tuple0**tn=_TB3;struct Cyc_List_List*ts=_TB2;struct Cyc_List_List*dles=_TB0;struct Cyc_Absyn_Aggrdecl**adopt=(struct Cyc_Absyn_Aggrdecl**)_TB1;struct _handler_cons _TB4;_T6C=& _TB4;_push_handler(_T6C);{int _TB5=0;_T6D=setjmp(_TB4.handler);if(!_T6D)goto _TL84;_TB5=1;goto _TL85;_TL84: _TL85: if(_TB5)goto _TL86;else{goto _TL88;}_TL88: _T6E=adopt;{
# 412
struct Cyc_Absyn_Aggrdecl*(*_TB6)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T6F=_TB6;}_T70=e;_T71=_T70->loc;_T72=env;_T73=_T72->ns;_T74=tn;_T75=*_T74;*_T6E=_T6F(_T71,_T73,_T75,Cyc_Binding_lookup_aggrdecl);_T76=tn;_T77=adopt;_T78=*_T77;_T79=
_check_null(_T78);*_T76=_T79->name;{int _TB6=1;_npop_handler(0);return _TB6;}_pop_handler();goto _TL87;_TL86: _T7A=Cyc_Core_get_exn_thrown();{void*_TB6=(void*)_T7A;void*_TB7;_T7B=(struct Cyc_Binding_BindingError_exn_struct*)_TB6;_T7C=_T7B->tag;_T7D=Cyc_Binding_BindingError;if(_T7C!=_T7D)goto _TL89;{struct Cyc_Warn_String_Warn_Warg_struct _TB8;_TB8.tag=0;
# 416
_TB8.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_T7E=_TB8;}{struct Cyc_Warn_String_Warn_Warg_struct _TB8=_T7E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TB9;_TB9.tag=1;_T80=tn;_TB9.f1=*_T80;_T7F=_TB9;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TB9=_T7F;void*_TBA[2];_TBA[0]=& _TB8;_TBA[1]=& _TB9;_T81=e;_T82=_T81->loc;_T83=_tag_fat(_TBA,sizeof(void*),2);Cyc_Warn_err2(_T82,_T83);}}
return 1;_TL89: _TB7=_TB6;{void*exn=_TB7;_rethrow(exn);};}_TL87:;}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TAD;_TB3=_TB4->f1;_TAF=_TB4->f2;_TAE=_TB4->f3;}{struct Cyc_Absyn_Vardecl*vd=_TB3;struct Cyc_Absyn_Exp*e1=_TAF;struct Cyc_Absyn_Exp*e2=_TAE;
# 420
Cyc_Binding_resolve_exp(env,e1);_T84=
Cyc_Binding_at_toplevel(env);if(!_T84)goto _TL8B;_T85=env;{struct Cyc_Dict_Dict*_TB4=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T88=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_TB5)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T88;_T87=_TB5;}_T89=Cyc_strptrcmp;*_TB4=_T87(_T89);_T86=(struct Cyc_Dict_Dict*)_TB4;}_T85->local_vars=_T86;_T8A=e;_T8B=_T8A->loc;_T8C=env;_T8D=vd;
Cyc_Binding_resolve_and_add_vardecl(_T8B,_T8C,_T8D);
Cyc_Binding_resolve_exp(env,e2);_T8E=env;
_T8E->local_vars=0;
return 0;_TL8B: _T8F=env;_T90=_T8F->local_vars;_T91=
# 428
_check_null(_T90);{struct Cyc_Dict_Dict old_locals=*_T91;_T92=e;_T93=_T92->loc;_T94=env;_T95=vd;
Cyc_Binding_resolve_and_add_vardecl(_T93,_T94,_T95);
Cyc_Binding_resolve_exp(env,e2);_T96=env;_T97=_T96->local_vars;_T98=
_check_null(_T97);*_T98=old_locals;
return 0;}}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TAD;_TB3=_TB4->f1;_TB2=_TB4->f2;}{struct Cyc_Absyn_Exp*e1=_TB3;struct Cyc_List_List*ts=_TB2;
# 435
_TL90: if(ts!=0)goto _TL8E;else{goto _TL8F;}
_TL8E: _T99=e;_T9A=_T99->loc;_T9B=env;_T9C=ts;_T9D=_T9C->hd;Cyc_Binding_resolve_type(_T9A,_T9B,_T9D);_T9E=ts;
# 435
ts=_T9E->tl;goto _TL90;_TL8F:
# 437
 return 1;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_TAD;_T9F=_TB4->f1;_TB3=_T9F.elt_type;}{void**to=_TB3;
# 439
if(to==0)goto _TL91;_TA0=to;_TB3=*_TA0;goto _LLE;_TL91: return 1;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_TAD;_TA1=_TB4->f1;_TB3=(void*)_TA1;}_LLE: {void*topt=_TB3;
# 441
if(topt!=0)goto _TL93;return 1;_TL93: _TB3=topt;goto _LL10;}case 32:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_TAD;_TA2=_TB4->f1;_TB3=(void*)_TA2;}_LL10: {void*t=_TB3;_TB3=t;goto _LL12;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_TAD;_TA3=_TB4->f1;{struct _tuple8 _TB5=*_TA3;_TB3=_TB5.f2;}}_LL12: {void*t=_TB3;_TB3=t;goto _LL14;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_TAD;_TA4=_TB4->f1;_TB3=(void*)_TA4;}_LL14: {void*t=_TB3;_TB3=t;goto _LL16;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_TAD;_TA5=_TB4->f1;_TB3=(void*)_TA5;}_LL16: {void*t=_TB3;_TB3=t;goto _LL18;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_TAD;_TA6=_TB4->f1;_TB3=(void*)_TA6;}_LL18: {void*t=_TB3;_TB3=t;goto _LL1A;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TAD;_TA7=_TB4->f2;_TB3=(void*)_TA7;}_LL1A: {void*t=_TB3;_TB3=t;goto _LL1C;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TB4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TAD;_TA8=_TB4->f1;_TB3=(void*)_TA8;}_LL1C: {void*t=_TB3;_TA9=e;_TAA=_TA9->loc;_TAB=env;_TAC=t;
# 450
Cyc_Binding_resolve_type(_TAA,_TAB,_TAC);
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
static void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD;union Cyc_Absyn_DatatypeInfo*_TE;unsigned _TF;struct Cyc_Binding_Env*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_List_List*_T13;union Cyc_Absyn_DatatypeInfo*_T14;struct _union_DatatypeInfo_UnknownDatatype _T15;unsigned _T16;struct _union_DatatypeInfo_UnknownDatatype _T17;struct Cyc_Absyn_UnknownDatatypeInfo _T18;struct _union_DatatypeInfo_UnknownDatatype _T19;struct Cyc_Absyn_UnknownDatatypeInfo _T1A;struct _handler_cons*_T1B;int _T1C;struct Cyc_List_List*(*_T1D)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T1E;struct Cyc_Binding_Env*_T1F;struct Cyc_Binding_NSCtxt*_T20;struct _tuple0*_T21;struct Cyc_List_List*_T22;void*_T23;union Cyc_Absyn_DatatypeInfo*_T24;struct Cyc_Absyn_UnknownDatatypeInfo _T25;struct Cyc_Absyn_Datatypedecl*_T26;void*_T27;struct Cyc_Binding_BindingError_exn_struct*_T28;char*_T29;char*_T2A;void*_T2B;void*_T2C;void*_T2D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2E;void*_T2F;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T30;union Cyc_Absyn_DatatypeFieldInfo*_T31;unsigned _T32;struct Cyc_Binding_Env*_T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_List_List*_T36;union Cyc_Absyn_DatatypeFieldInfo*_T37;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T38;unsigned _T39;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3A;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3B;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3C;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3D;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3E;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T3F;struct _handler_cons*_T40;int _T41;int*_T42;int _T43;struct Cyc_List_List*(*_T44)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T45;struct Cyc_Binding_Env*_T46;struct Cyc_Binding_NSCtxt*_T47;struct _tuple0*_T48;struct Cyc_List_List*_T49;void*_T4A;struct Cyc_Absyn_Datatypedecl*_T4B;struct _tuple0*_T4C;struct Cyc_Absyn_Datatypedecl*_T4D;struct _tuple0*_T4E;int _T4F;struct Cyc_Warn_Qvar_Warn_Warg_struct _T50;struct Cyc_Absyn_Datatypefield*_T51;struct Cyc_Warn_String_Warn_Warg_struct _T52;struct Cyc_Warn_Qvar_Warn_Warg_struct _T53;struct Cyc_Absyn_Datatypedecl*_T54;struct Cyc_Warn_String_Warn_Warg_struct _T55;struct Cyc_Warn_Qvar_Warn_Warg_struct _T56;struct Cyc_Absyn_Datatypedecl*_T57;struct Cyc_Warn_String_Warn_Warg_struct _T58;unsigned _T59;struct _fat_ptr _T5A;union Cyc_Absyn_DatatypeFieldInfo*_T5B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T5C;struct Cyc_Absyn_Datatypedecl*_T5D;struct Cyc_Absyn_Datatypefield*_T5E;void*_T5F;struct Cyc_Binding_BindingError_exn_struct*_T60;char*_T61;char*_T62;struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;unsigned _T65;struct _fat_ptr _T66;void*_T67;void*_T68;void*_T69;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6A;void*_T6B;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6C;union Cyc_Absyn_AggrInfo*_T6D;unsigned _T6E;struct Cyc_Binding_Env*_T6F;struct Cyc_List_List*_T70;void*_T71;struct Cyc_List_List*_T72;union Cyc_Absyn_AggrInfo*_T73;struct _union_AggrInfo_UnknownAggr _T74;unsigned _T75;struct _union_AggrInfo_UnknownAggr _T76;struct _tuple2 _T77;struct _union_AggrInfo_UnknownAggr _T78;struct _tuple2 _T79;struct _union_AggrInfo_UnknownAggr _T7A;struct _tuple2 _T7B;struct _handler_cons*_T7C;int _T7D;struct Cyc_Absyn_Aggrdecl*(*_T7E)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T7F;struct Cyc_Binding_Env*_T80;struct Cyc_Binding_NSCtxt*_T81;struct _tuple0*_T82;struct Cyc_Absyn_Aggrdecl*_T83;enum Cyc_Absyn_AggrKind _T84;int _T85;enum Cyc_Absyn_AggrKind _T86;int _T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;unsigned _T89;struct _fat_ptr _T8A;enum Cyc_Absyn_AggrKind _T8B;int _T8C;struct Cyc_Absyn_Aggrdecl*_T8D;struct Cyc_Absyn_AggrdeclImpl*_T8E;struct Cyc_Absyn_Aggrdecl*_T8F;struct Cyc_Absyn_AggrdeclImpl*_T90;int _T91;struct Cyc_Core_Opt*_T92;void*_T93;int _T94;struct Cyc_Warn_String_Warn_Warg_struct _T95;unsigned _T96;struct _fat_ptr _T97;union Cyc_Absyn_AggrInfo*_T98;enum Cyc_Absyn_AggrKind _T99;struct Cyc_Absyn_Aggrdecl*_T9A;struct _tuple0*_T9B;struct Cyc_Core_Opt*_T9C;void*_T9D;struct Cyc_Binding_BindingError_exn_struct*_T9E;char*_T9F;char*_TA0;void*_TA1;void*_TA2;void*_TA3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA4;void*_TA5;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TA6;struct _tuple0**_TA7;struct _handler_cons*_TA8;int _TA9;struct Cyc_Absyn_Enumdecl*(*_TAA)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TAB;struct Cyc_Binding_Env*_TAC;struct Cyc_Binding_NSCtxt*_TAD;struct _tuple0**_TAE;struct _tuple0*_TAF;struct _tuple0**_TB0;struct Cyc_Absyn_Enumdecl*_TB1;void*_TB2;struct Cyc_Binding_BindingError_exn_struct*_TB3;char*_TB4;char*_TB5;unsigned _TB6;struct Cyc_Binding_Env*_TB7;struct _tuple0**_TB8;struct _tuple0*_TB9;void*_TBA;void*_TBB;struct Cyc_Binding_ResolveNSEnv*(*_TBC)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TBD;struct Cyc_Binding_NSCtxt*_TBE;struct Cyc_Binding_Env*_TBF;struct Cyc_Binding_NSCtxt*_TC0;struct Cyc_List_List*_TC1;int _TC2;union Cyc_Absyn_Nmspace _TC3;struct Cyc_List_List*_TC4;void*_TC5;struct Cyc_Absyn_Enumfield*_TC6;struct Cyc_Absyn_Exp*_TC7;struct Cyc_Binding_Env*_TC8;struct Cyc_Absyn_Enumfield*_TC9;struct Cyc_Absyn_Exp*_TCA;struct Cyc_Binding_ResolveNSEnv*_TCB;struct Cyc_Dict_Dict(*_TCC)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TCD)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TCE;struct Cyc_Dict_Dict _TCF;struct Cyc_Absyn_Enumfield*_TD0;struct _tuple0*_TD1;struct _tuple0 _TD2;struct _fat_ptr*_TD3;struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_TD4;void*_TD5;struct Cyc_List_List*_TD6;void*_TD7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD8;struct Cyc_List_List*_TD9;void*_TDA;void*_TDB;unsigned _TDC;struct Cyc_Binding_Env*_TDD;struct Cyc_List_List*_TDE;void*_TDF;struct Cyc_List_List*_TE0;void*_TE1;void*_TE2;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TE3;struct _tuple0**_TE4;void*_TE5;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TE6;struct Cyc_Absyn_Typedefdecl**_TE7;unsigned _TE8;struct Cyc_Binding_Env*_TE9;struct Cyc_List_List*_TEA;void*_TEB;struct Cyc_List_List*_TEC;struct _handler_cons*_TED;int _TEE;struct Cyc_Absyn_Typedefdecl*(*_TEF)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _TF0;struct Cyc_Binding_Env*_TF1;struct Cyc_Binding_NSCtxt*_TF2;struct _tuple0**_TF3;struct _tuple0*_TF4;struct _tuple0**_TF5;struct Cyc_Absyn_Typedefdecl*_TF6;void*_TF7;struct Cyc_Binding_BindingError_exn_struct*_TF8;char*_TF9;char*_TFA;struct Cyc_Warn_String_Warn_Warg_struct _TFB;struct Cyc_Warn_Qvar_Warn_Warg_struct _TFC;struct _tuple0**_TFD;unsigned _TFE;struct _fat_ptr _TFF;void*_T100;void*_T101;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T102;void***_T103;struct Cyc_Binding_Env*_T104;struct Cyc_Binding_Env*_T105;struct Cyc_Absyn_TypeDecl*_T106;int*_T107;unsigned _T108;struct Cyc_Binding_Env*_T109;struct Cyc_Absyn_Decl*_T10A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T10B;struct Cyc_Absyn_TypeDecl*_T10C;struct Cyc_Binding_Env*_T10D;struct Cyc_Absyn_Decl*_T10E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T10F;struct Cyc_Absyn_TypeDecl*_T110;struct Cyc_Binding_Env*_T111;struct Cyc_Absyn_Decl*_T112;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T113;struct Cyc_Absyn_TypeDecl*_T114;struct Cyc_Binding_Env*_T115;void*_T116;void*_T117;void*_T118;int _T119;void*_T11A;void*_T11B;void*_T11C;struct Cyc_Absyn_PtrInfo _T11D;struct Cyc_Absyn_PtrInfo _T11E;struct Cyc_Absyn_PtrAtts _T11F;struct Cyc_Absyn_PtrInfo _T120;struct Cyc_Absyn_PtrAtts _T121;void*_T122;struct Cyc_Absyn_ArrayInfo _T123;struct Cyc_Absyn_ArrayInfo _T124;void*_T125;void*_T126;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T127;struct Cyc_Absyn_FnInfo*_T128;unsigned _T129;struct Cyc_Binding_Env*_T12A;struct Cyc_Absyn_FnInfo*_T12B;struct Cyc_List_List*_T12C;struct Cyc_Absyn_FnInfo*_T12D;struct Cyc_Absyn_VarargInfo*_T12E;struct Cyc_Absyn_FnInfo*_T12F;struct Cyc_Binding_Env*_T130;struct Cyc_Binding_Env*_T131;struct Cyc_Dict_Dict*_T132;struct Cyc_Binding_Env*_T133;struct Cyc_Dict_Dict*_T134;struct Cyc_Binding_Env*_T135;struct Cyc_Dict_Dict*_T136;struct Cyc_Dict_Dict(*_T137)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T138)(int(*)(void*,void*));int(*_T139)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T13A;void*_T13B;struct Cyc_Absyn_Vardecl*_T13C;struct Cyc_Binding_Env*_T13D;struct Cyc_Dict_Dict*_T13E;struct Cyc_Dict_Dict*_T13F;struct Cyc_Dict_Dict(*_T140)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T141)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T142;struct Cyc_Dict_Dict*_T143;struct Cyc_Dict_Dict*_T144;struct Cyc_Dict_Dict _T145;struct Cyc_List_List*_T146;void*_T147;struct Cyc_Absyn_Vardecl*_T148;struct _tuple0*_T149;struct _fat_ptr*_T14A;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T14B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T14C;struct Cyc_List_List*_T14D;void*_T14E;void*_T14F;struct Cyc_List_List*_T150;struct Cyc_Binding_Env*_T151;void*_T152;unsigned _T153;struct Cyc_Binding_Env*_T154;struct Cyc_Absyn_Vardecl*_T155;void*_T156;struct Cyc_Binding_Env*_T157;struct Cyc_Binding_Env*_T158;struct Cyc_Dict_Dict*_T159;struct Cyc_Binding_Env*_T15A;struct Cyc_Dict_Dict*_T15B;struct Cyc_Binding_Env*_T15C;struct Cyc_Dict_Dict*_T15D;struct Cyc_Dict_Dict(*_T15E)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T15F)(int(*)(void*,void*));int(*_T160)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Binding_Env*_T161;struct Cyc_Dict_Dict*_T162;struct Cyc_Dict_Dict*_T163;struct Cyc_Dict_Dict(*_T164)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T165)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T166;struct Cyc_Dict_Dict*_T167;struct Cyc_Dict_Dict _T168;struct Cyc_Absyn_Vardecl*_T169;struct _tuple0*_T16A;struct _fat_ptr*_T16B;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T16C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T16D;void*_T16E;struct Cyc_Binding_Env*_T16F;void*_T170;void*_T171;struct Cyc_Absyn_Vardecl*_T172;struct Cyc_Absyn_Exp*_T173;void*_T174;void*_T175;void*_T176;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;switch(_T7){case 22: _T8=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T9=_T177->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T178=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T9;_TA=t;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC;_TE=& _TD->f1;_T176=(union Cyc_Absyn_DatatypeInfo*)_TE;}_T175=_T177->f2;}{union Cyc_Absyn_DatatypeInfo*info=_T176;struct Cyc_List_List*targs=_T175;
# 580
_TLC9: if(targs!=0)goto _TLC7;else{goto _TLC8;}
_TLC7: _TF=loc;_T10=env;_T11=targs;_T12=_T11->hd;Cyc_Binding_resolve_type(_TF,_T10,_T12);_T13=targs;
# 580
targs=_T13->tl;goto _TLC9;_TLC8: _T14=info;{
# 582
union Cyc_Absyn_DatatypeInfo _T177=*_T14;int _T178;struct _tuple0*_T179;_T15=_T177.UnknownDatatype;_T16=_T15.tag;if(_T16!=1)goto _TLCA;_T17=_T177.UnknownDatatype;_T18=_T17.val;_T179=_T18.name;_T19=_T177.UnknownDatatype;_T1A=_T19.val;_T178=_T1A.is_extensible;{struct _tuple0*qv=_T179;int b=_T178;struct _handler_cons _T17A;_T1B=& _T17A;_push_handler(_T1B);{int _T17B=0;_T1C=setjmp(_T17A.handler);if(!_T1C)goto _TLCC;_T17B=1;goto _TLCD;_TLCC: _TLCD: if(_T17B)goto _TLCE;else{goto _TLD0;}_TLD0:{
# 585
struct Cyc_List_List*(*_T17C)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T1D=_T17C;}_T1E=loc;_T1F=env;_T20=_T1F->ns;_T21=qv;_T22=_T1D(_T1E,_T20,_T21,Cyc_Binding_lookup_datatypedecl);_T23=_T22->hd;{struct Cyc_Absyn_Datatypedecl*tud=(struct Cyc_Absyn_Datatypedecl*)_T23;_T24=info;{struct Cyc_Absyn_UnknownDatatypeInfo _T17C;_T26=tud;
_T17C.name=_T26->name;_T17C.is_extensible=b;_T25=_T17C;}*_T24=Cyc_Absyn_UnknownDatatype(_T25);_npop_handler(0);return;}_pop_handler();goto _TLCF;_TLCE: _T27=Cyc_Core_get_exn_thrown();{void*_T17C=(void*)_T27;void*_T17D;_T28=(struct Cyc_Binding_BindingError_exn_struct*)_T17C;_T29=_T28->tag;_T2A=Cyc_Binding_BindingError;if(_T29!=_T2A)goto _TLD1;
# 589
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLD1: _T17D=_T17C;{void*exn=_T17D;_rethrow(exn);};}_TLCF:;}}goto _TLCB;_TLCA:
 return;_TLCB:;}}case 23: _T2B=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2B;_T2C=_T177->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T178=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T2C;_T2D=t;_T2E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T2F=_T2E->f1;_T30=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_T2F;_T31=& _T30->f1;_T176=(union Cyc_Absyn_DatatypeFieldInfo*)_T31;}_T175=_T177->f2;}{union Cyc_Absyn_DatatypeFieldInfo*info=_T176;struct Cyc_List_List*targs=_T175;
# 593
_TLD6: if(targs!=0)goto _TLD4;else{goto _TLD5;}
_TLD4: _T32=loc;_T33=env;_T34=targs;_T35=_T34->hd;Cyc_Binding_resolve_type(_T32,_T33,_T35);_T36=targs;
# 593
targs=_T36->tl;goto _TLD6;_TLD5: _T37=info;{
# 595
union Cyc_Absyn_DatatypeFieldInfo _T177=*_T37;int _T178;struct _tuple0*_T179;struct _tuple0*_T17A;_T38=_T177.UnknownDatatypefield;_T39=_T38.tag;if(_T39!=1)goto _TLD7;_T3A=_T177.UnknownDatatypefield;_T3B=_T3A.val;_T17A=_T3B.datatype_name;_T3C=_T177.UnknownDatatypefield;_T3D=_T3C.val;_T179=_T3D.field_name;_T3E=_T177.UnknownDatatypefield;_T3F=_T3E.val;_T178=_T3F.is_extensible;{struct _tuple0*qvd=_T17A;struct _tuple0*qvf=_T179;int b=_T178;{struct _handler_cons _T17B;_T40=& _T17B;_push_handler(_T40);{int _T17C=0;_T41=setjmp(_T17B.handler);if(!_T41)goto _TLD9;_T17C=1;goto _TLDA;_TLD9: _TLDA: if(_T17C)goto _TLDB;else{goto _TLDD;}_TLDD:{
# 600
void*_T17D=Cyc_Binding_lookup_ordinary(loc,env,qvf);struct Cyc_Absyn_Datatypefield*_T17E;struct Cyc_Absyn_Datatypedecl*_T17F;_T42=(int*)_T17D;_T43=*_T42;if(_T43!=2)goto _TLDE;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T180=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T17D;_T17F=_T180->f1;_T17E=_T180->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T17F;struct Cyc_Absyn_Datatypefield*tuf=_T17E;{
# 602
struct Cyc_List_List*(*_T180)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T44=_T180;}_T45=loc;_T46=env;_T47=_T46->ns;_T48=qvd;_T49=_T44(_T45,_T47,_T48,Cyc_Binding_lookup_datatypedecl);_T4A=_T49->hd;{struct Cyc_Absyn_Datatypedecl*tud2=(struct Cyc_Absyn_Datatypedecl*)_T4A;_T4B=tud;_T4C=_T4B->name;_T4D=tud2;_T4E=_T4D->name;_T4F=
Cyc_Absyn_qvar_cmp(_T4C,_T4E);if(_T4F==0)goto _TLE0;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T180;_T180.tag=1;_T51=tuf;
_T180.f1=_T51->name;_T50=_T180;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T180=_T50;{struct Cyc_Warn_String_Warn_Warg_struct _T181;_T181.tag=0;_T181.f1=_tag_fat(" is a variant of ",sizeof(char),18U);_T52=_T181;}{struct Cyc_Warn_String_Warn_Warg_struct _T181=_T52;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T182;_T182.tag=1;_T54=tud2;_T182.f1=_T54->name;_T53=_T182;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T182=_T53;{struct Cyc_Warn_String_Warn_Warg_struct _T183;_T183.tag=0;_T183.f1=_tag_fat(" not ",sizeof(char),6U);_T55=_T183;}{struct Cyc_Warn_String_Warn_Warg_struct _T183=_T55;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T184;_T184.tag=1;_T57=tud;
_T184.f1=_T57->name;_T56=_T184;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T184=_T56;{struct Cyc_Warn_String_Warn_Warg_struct _T185;_T185.tag=0;_T185.f1=_tag_fat(" (shadowing not yet implemented properly)",sizeof(char),42U);_T58=_T185;}{struct Cyc_Warn_String_Warn_Warg_struct _T185=_T58;void*_T186[6];_T186[0]=& _T180;_T186[1]=& _T181;_T186[2]=& _T182;_T186[3]=& _T183;_T186[4]=& _T184;_T186[5]=& _T185;_T59=loc;_T5A=_tag_fat(_T186,sizeof(void*),6);Cyc_Warn_err2(_T59,_T5A);}}}}}}_npop_handler(0);return;_TLE0: _T5B=info;{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T180;_T5D=tud;
# 608
_T180.datatype_name=_T5D->name;_T5E=tuf;
_T180.field_name=_T5E->name;_T180.is_extensible=b;_T5C=_T180;}
# 608
*_T5B=Cyc_Absyn_UnknownDatatypefield(_T5C);_npop_handler(0);return;}}_TLDE: goto _LL36;_LL36:;}_pop_handler();goto _TLDC;_TLDB: _T5F=Cyc_Core_get_exn_thrown();{void*_T17D=(void*)_T5F;void*_T17E;_T60=(struct Cyc_Binding_BindingError_exn_struct*)_T17D;_T61=_T60->tag;_T62=Cyc_Binding_BindingError;if(_T61!=_T62)goto _TLE2;goto _LL3B;_TLE2: _T17E=_T17D;{void*exn=_T17E;_rethrow(exn);}_LL3B:;}_TLDC:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T17B;_T17B.tag=1;
# 614
_T17B.f1=qvf;_T63=_T17B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T17B=_T63;{struct Cyc_Warn_String_Warn_Warg_struct _T17C;_T17C.tag=0;_T17C.f1=_tag_fat(" is not a datatype field",sizeof(char),25U);_T64=_T17C;}{struct Cyc_Warn_String_Warn_Warg_struct _T17C=_T64;void*_T17D[2];_T17D[0]=& _T17B;_T17D[1]=& _T17C;_T65=loc;_T66=_tag_fat(_T17D,sizeof(void*),2);Cyc_Warn_err2(_T65,_T66);}}return;}_TLD7:
 return;;}}case 24: _T67=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T67;_T68=_T177->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T178=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T68;_T69=t;_T6A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T69;_T6B=_T6A->f1;_T6C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6B;_T6D=& _T6C->f1;_T176=(union Cyc_Absyn_AggrInfo*)_T6D;}_T175=_T177->f2;}{union Cyc_Absyn_AggrInfo*info=_T176;struct Cyc_List_List*targs=_T175;
# 619
_TLE7: if(targs!=0)goto _TLE5;else{goto _TLE6;}
_TLE5: _T6E=loc;_T6F=env;_T70=targs;_T71=_T70->hd;Cyc_Binding_resolve_type(_T6E,_T6F,_T71);_T72=targs;
# 619
targs=_T72->tl;goto _TLE7;_TLE6: _T73=info;{
# 621
union Cyc_Absyn_AggrInfo _T177=*_T73;struct Cyc_Core_Opt*_T178;struct _tuple0*_T179;enum Cyc_Absyn_AggrKind _T17A;_T74=_T177.UnknownAggr;_T75=_T74.tag;if(_T75!=1)goto _TLE8;_T76=_T177.UnknownAggr;_T77=_T76.val;_T17A=_T77.f0;_T78=_T177.UnknownAggr;_T79=_T78.val;_T179=_T79.f1;_T7A=_T177.UnknownAggr;_T7B=_T7A.val;_T178=_T7B.f2;{enum Cyc_Absyn_AggrKind ak=_T17A;struct _tuple0*qv=_T179;struct Cyc_Core_Opt*bo=_T178;struct _handler_cons _T17B;_T7C=& _T17B;_push_handler(_T7C);{int _T17C=0;_T7D=setjmp(_T17B.handler);if(!_T7D)goto _TLEA;_T17C=1;goto _TLEB;_TLEA: _TLEB: if(_T17C)goto _TLEC;else{goto _TLEE;}_TLEE:{
# 624
struct Cyc_Absyn_Aggrdecl*(*_T17D)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T7E=_T17D;}_T7F=loc;_T80=env;_T81=_T80->ns;_T82=qv;{struct Cyc_Absyn_Aggrdecl*ad=_T7E(_T7F,_T81,_T82,Cyc_Binding_lookup_aggrdecl);_T83=ad;_T84=_T83->kind;_T85=(int)_T84;_T86=ak;_T87=(int)_T86;
if(_T85==_T87)goto _TLEF;{struct Cyc_Warn_String_Warn_Warg_struct _T17D;_T17D.tag=0;
_T17D.f1=_tag_fat("struct vs. union mismatch with earlier declaration",sizeof(char),51U);_T88=_T17D;}{struct Cyc_Warn_String_Warn_Warg_struct _T17D=_T88;void*_T17E[1];_T17E[0]=& _T17D;_T89=loc;_T8A=_tag_fat(_T17E,sizeof(void*),1);Cyc_Warn_err2(_T89,_T8A);}goto _TLF0;_TLEF: _TLF0: _T8B=ak;_T8C=(int)_T8B;
if(_T8C!=1)goto _TLF1;if(bo==0)goto _TLF1;_T8D=ad;_T8E=_T8D->impl;if(_T8E==0)goto _TLF1;_T8F=ad;_T90=_T8F->impl;_T91=_T90->tagged;_T92=bo;_T93=_T92->v;_T94=(int)_T93;if(_T91==_T94)goto _TLF1;{struct Cyc_Warn_String_Warn_Warg_struct _T17D;_T17D.tag=0;
# 629
_T17D.f1=_tag_fat("@tagged mismatch with earlier declaration",sizeof(char),42U);_T95=_T17D;}{struct Cyc_Warn_String_Warn_Warg_struct _T17D=_T95;void*_T17E[1];_T17E[0]=& _T17D;_T96=loc;_T97=_tag_fat(_T17E,sizeof(void*),1);Cyc_Warn_err2(_T96,_T97);}goto _TLF2;_TLF1: _TLF2: _T98=info;_T99=ak;_T9A=ad;_T9B=_T9A->name;_T9C=bo;
*_T98=Cyc_Absyn_UnknownAggr(_T99,_T9B,_T9C);_npop_handler(0);return;}_pop_handler();goto _TLED;_TLEC: _T9D=Cyc_Core_get_exn_thrown();{void*_T17D=(void*)_T9D;void*_T17E;_T9E=(struct Cyc_Binding_BindingError_exn_struct*)_T17D;_T9F=_T9E->tag;_TA0=Cyc_Binding_BindingError;if(_T9F!=_TA0)goto _TLF3;
# 633
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;_TLF3: _T17E=_T17D;{void*exn=_T17E;_rethrow(exn);};}_TLED:;}}goto _TLE9;_TLE8:
 return;_TLE9:;}}case 19: _TA1=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA1;_TA2=_T177->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T178=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TA2;_TA3=t;_TA4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA3;_TA5=_TA4->f1;_TA6=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_TA5;_TA7=& _TA6->f1;_T176=(struct _tuple0**)_TA7;_T175=_T178->f2;}}{struct _tuple0**qv=_T176;struct Cyc_Absyn_Enumdecl*edo=_T175;
# 637
if(edo==0)goto _TLF5;
return;_TLF5: {struct _handler_cons _T177;_TA8=& _T177;_push_handler(_TA8);{int _T178=0;_TA9=setjmp(_T177.handler);if(!_TA9)goto _TLF7;_T178=1;goto _TLF8;_TLF7: _TLF8: if(_T178)goto _TLF9;else{goto _TLFB;}_TLFB:{
# 640
struct Cyc_Absyn_Enumdecl*(*_T179)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TAA=_T179;}_TAB=loc;_TAC=env;_TAD=_TAC->ns;_TAE=qv;_TAF=*_TAE;{struct Cyc_Absyn_Enumdecl*ed=_TAA(_TAB,_TAD,_TAF,Cyc_Binding_lookup_enumdecl);_TB0=qv;_TB1=ed;
*_TB0=_TB1->name;_npop_handler(0);return;}_pop_handler();goto _TLFA;_TLF9: _TB2=Cyc_Core_get_exn_thrown();{void*_T179=(void*)_TB2;void*_T17A;_TB3=(struct Cyc_Binding_BindingError_exn_struct*)_T179;_TB4=_TB3->tag;_TB5=Cyc_Binding_BindingError;if(_TB4!=_TB5)goto _TLFC;_TB6=loc;_TB7=env;_TB8=qv;_TB9=*_TB8;
# 644
Cyc_Binding_absolutize_topdecl(_TB6,_TB7,_TB9,2U);return;_TLFC: _T17A=_T179;{void*exn=_T17A;_rethrow(exn);};}_TLFA:;}}}case 20: _TBA=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TBA;_TBB=_T177->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_T178=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_TBB;_T176=_T178->f1;}}{struct Cyc_List_List*fs=_T176;{
# 682
struct Cyc_Binding_ResolveNSEnv*(*_T177)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TBC=_T177;}_TBD=env;_TBE=_TBD->ns;_TBF=env;_TC0=_TBF->ns;_TC1=_TC0->curr_ns;_TC2=
Cyc_Binding_in_cinclude(env);_TC3=
# 682
Cyc_Absyn_Abs_n(_TC1,_TC2);{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TBC(_TBE,_TC3);
# 684
_TL101: if(fs!=0)goto _TLFF;else{goto _TL100;}
_TLFF: _TC4=fs;_TC5=_TC4->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TC5;_TC6=f;_TC7=_TC6->tag;
if(_TC7==0)goto _TL102;_TC8=env;_TC9=f;_TCA=_TC9->tag;
Cyc_Binding_resolve_exp(_TC8,_TCA);goto _TL103;_TL102: _TL103: _TCB=decl_ns_data;_TCD=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T177)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TCD;_TCC=_T177;}_TCE=decl_ns_data;_TCF=_TCE->ordinaries;_TD0=f;_TD1=_TD0->name;_TD2=*_TD1;_TD3=_TD2.f1;{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T177=_cycalloc(sizeof(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct));_T177->tag=4;_T177->f1=t;_T177->f2=f;_TD4=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T177;}_TD5=(void*)_TD4;_TCB->ordinaries=_TCC(_TCF,_TD3,_TD5);}_TD6=fs;
# 684
fs=_TD6->tl;goto _TL101;_TL100:
# 690
 return;}}default: _TD7=t;_TD8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD7;_TD9=_TD8->f2;if(_TD9!=0)goto _TL104;_LL18:
# 700
 return;_TL104: _TDA=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TDA;_TDB=_T177->f1;_T176=(void*)_TDB;_T175=_T177->f2;}{void*c=_T176;struct Cyc_List_List*ts=_T175;
# 702
_TL109: if(ts!=0)goto _TL107;else{goto _TL108;}
_TL107: _TDC=loc;_TDD=env;_TDE=ts;_TDF=_TDE->hd;Cyc_Binding_resolve_type(_TDC,_TDD,_TDF);_TE0=ts;
# 702
ts=_TE0->tl;goto _TL109;_TL108:
# 704
 return;}};case 8: _TE1=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE1;_TE2=t;_TE3=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE2;_TE4=& _TE3->f1;_T176=(struct _tuple0**)_TE4;_T175=_T177->f2;_TE5=t;_TE6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TE5;_TE7=& _TE6->f3;_T174=(struct Cyc_Absyn_Typedefdecl**)_TE7;}{struct _tuple0**tdn=_T176;struct Cyc_List_List*targs=_T175;struct Cyc_Absyn_Typedefdecl**tdo=_T174;
# 647
_TL10D: if(targs!=0)goto _TL10B;else{goto _TL10C;}
_TL10B: _TE8=loc;_TE9=env;_TEA=targs;_TEB=_TEA->hd;Cyc_Binding_resolve_type(_TE8,_TE9,_TEB);_TEC=targs;
# 647
targs=_TEC->tl;goto _TL10D;_TL10C:{struct _handler_cons _T177;_TED=& _T177;_push_handler(_TED);{int _T178=0;_TEE=setjmp(_T177.handler);if(!_TEE)goto _TL10E;_T178=1;goto _TL10F;_TL10E: _TL10F: if(_T178)goto _TL110;else{goto _TL112;}_TL112:{
# 650
struct Cyc_Absyn_Typedefdecl*(*_T179)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_TEF=_T179;}_TF0=loc;_TF1=env;_TF2=_TF1->ns;_TF3=tdn;_TF4=*_TF3;{struct Cyc_Absyn_Typedefdecl*td=_TEF(_TF0,_TF2,_TF4,Cyc_Binding_lookup_typedefdecl);_TF5=tdn;_TF6=td;
# 652
*_TF5=_TF6->name;_npop_handler(0);return;}_pop_handler();goto _TL111;_TL110: _TF7=Cyc_Core_get_exn_thrown();{void*_T179=(void*)_TF7;void*_T17A;_TF8=(struct Cyc_Binding_BindingError_exn_struct*)_T179;_TF9=_TF8->tag;_TFA=Cyc_Binding_BindingError;if(_TF9!=_TFA)goto _TL113;goto _LL4F;_TL113: _T17A=_T179;{void*exn=_T17A;_rethrow(exn);}_LL4F:;}_TL111:;}}{struct Cyc_Warn_String_Warn_Warg_struct _T177;_T177.tag=0;
# 655
_T177.f1=_tag_fat("unbound typdef name ",sizeof(char),21U);_TFB=_T177;}{struct Cyc_Warn_String_Warn_Warg_struct _T177=_TFB;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T178;_T178.tag=1;_TFD=tdn;_T178.f1=*_TFD;_TFC=_T178;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T178=_TFC;void*_T179[2];_T179[0]=& _T177;_T179[1]=& _T178;_TFE=loc;_TFF=_tag_fat(_T179,sizeof(void*),2);Cyc_Warn_err2(_TFE,_TFF);}}
return;}case 10: _T100=t;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T100;_T176=_T177->f1;_T101=t;_T102=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T101;_T103=& _T102->f2;_T175=(void***)_T103;}{struct Cyc_Absyn_TypeDecl*td=_T176;void***to=_T175;_T104=env;{
# 660
struct Cyc_Dict_Dict*old_locals=_T104->local_vars;_T105=env;
_T105->local_vars=0;_T106=td;{
void*_T177=_T106->r;struct Cyc_Absyn_Datatypedecl*_T178;struct Cyc_Absyn_Enumdecl*_T179;struct Cyc_Absyn_Aggrdecl*_T17A;_T107=(int*)_T177;_T108=*_T107;switch(_T108){case 0:{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T17B=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T177;_T17A=_T17B->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T17A;_T109=env;{struct Cyc_Absyn_Decl*_T17B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T17C=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_T17C->tag=5;
# 664
_T17C->f1=ad;_T10B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T17C;}_T17B->r=(void*)_T10B;_T10C=td;_T17B->loc=_T10C->loc;_T10A=(struct Cyc_Absyn_Decl*)_T17B;}Cyc_Binding_resolve_decl(_T109,_T10A);goto _LL54;}case 1:{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T17B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T177;_T179=_T17B->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T179;_T10D=env;{struct Cyc_Absyn_Decl*_T17B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T17C=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_T17C->tag=7;
# 666
_T17C->f1=ed;_T10F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T17C;}_T17B->r=(void*)_T10F;_T110=td;_T17B->loc=_T110->loc;_T10E=(struct Cyc_Absyn_Decl*)_T17B;}Cyc_Binding_resolve_decl(_T10D,_T10E);goto _LL54;}default:{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T17B=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T177;_T178=_T17B->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T178;_T111=env;{struct Cyc_Absyn_Decl*_T17B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T17C=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T17C->tag=6;
# 668
_T17C->f1=tud;_T113=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T17C;}_T17B->r=(void*)_T113;_T114=td;_T17B->loc=_T114->loc;_T112=(struct Cyc_Absyn_Decl*)_T17B;}Cyc_Binding_resolve_decl(_T111,_T112);goto _LL54;}}_LL54:;}_T115=env;
# 670
_T115->local_vars=old_locals;
return;}}case 7: _T116=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T116;_T176=_T177->f3;}{struct Cyc_List_List*fs=_T176;
# 677
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 3: _T117=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T177=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T117;_T118=_T177->f2;_T176=(void*)_T118;}{void*to=_T176;_T119=Cyc_Flags_resolve;
# 692
if(!_T119)goto _TL116;
Cyc_BansheeIf_register_toplevel_cvar(t);goto _LL0;
# 697
_TL116: _T176=to;goto _LL14;}case 1: _T11A=t;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T177=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T11A;_T11B=_T177->f2;_T176=(void*)_T11B;}_LL14: {void*to=_T176;
if(to==0)goto _TL118;Cyc_Binding_resolve_type(loc,env,to);goto _TL119;_TL118: _TL119: return;}case 2: goto _LL18;case 4: _T11C=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T11C;_T11D=_T177->f1;_T176=_T11D.elt_type;_T11E=_T177->f1;_T11F=_T11E.ptr_atts;_T175=_T11F.eff;_T120=_T177->f1;_T121=_T120.ptr_atts;_T174=_T121.bounds;}{void*t1=_T176;void*t2=_T175;void*bds=_T174;
# 707
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 5: _T122=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T122;_T123=_T177->f1;_T176=_T123.elt_type;_T124=_T177->f1;_T173=_T124.num_elts;}{void*t1=_T176;struct Cyc_Absyn_Exp*eo=_T173;
# 713
Cyc_Binding_resolve_type(loc,env,t1);
if(eo==0)goto _TL11A;Cyc_Binding_resolve_exp(env,eo);goto _TL11B;_TL11A: _TL11B:
 return;}case 6: _T125=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T125;_T126=t;_T127=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T126;_T128=& _T127->f1;_T176=(struct Cyc_Absyn_FnInfo*)_T128;}{struct Cyc_Absyn_FnInfo*i=(struct Cyc_Absyn_FnInfo*)_T176;_T129=loc;_T12A=env;_T12B=i;_T12C=_T12B->args;_T12D=i;_T12E=_T12D->cyc_varargs;{
# 720
struct Cyc_List_List*vds=Cyc_Binding_get_fun_vardecls(0,_T129,_T12A,_T12C,_T12E);_T12F=i;
_T12F->arg_vardecls=vds;_T130=env;{
struct Cyc_Dict_Dict*old_locals=_T130->local_vars;
if(old_locals==0)goto _TL11C;_T131=env;{struct Cyc_Dict_Dict*_T177=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T133=env;_T134=_T133->local_vars;
*_T177=*_T134;_T132=(struct Cyc_Dict_Dict*)_T177;}_T131->local_vars=_T132;goto _TL11D;
# 726
_TL11C: _T135=env;{struct Cyc_Dict_Dict*_T177=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T138=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T178)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T138;_T137=_T178;}_T139=Cyc_strptrcmp;*_T177=_T137(_T139);_T136=(struct Cyc_Dict_Dict*)_T177;}_T135->local_vars=_T136;_TL11D:{
struct Cyc_List_List*vds1=vds;_TL121: if(vds1!=0)goto _TL11F;else{goto _TL120;}
_TL11F: _T13A=vds1;_T13B=_T13A->hd;_T13C=(struct Cyc_Absyn_Vardecl*)_T13B;if(_T13C==0)goto _TL122;_T13D=env;_T13E=_T13D->local_vars;_T13F=
_check_null(_T13E);_T141=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T177)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T141;_T140=_T177;}_T142=env;_T143=_T142->local_vars;_T144=_check_null(_T143);_T145=*_T144;_T146=vds1;_T147=_T146->hd;_T148=(struct Cyc_Absyn_Vardecl*)_T147;_T149=_T148->name;_T14A=_T149->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T177=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T177->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T178=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T178->tag=3;_T14D=vds1;_T14E=_T14D->hd;
_T178->f1=(struct Cyc_Absyn_Vardecl*)_T14E;_T14C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T178;}_T177->f1=(void*)_T14C;_T14B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T177;}_T14F=(void*)_T14B;
# 729
*_T13F=_T140(_T145,_T14A,_T14F);goto _TL123;_TL122: _TL123: _T150=vds1;
# 727
vds1=_T150->tl;goto _TL121;_TL120:;}
# 732
Cyc_Binding_resolve_function_stuff(loc,env,i);_T151=env;
_T151->local_vars=old_locals;
return;}}}case 12: _T152=t;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T152;_T172=_T177->f1;_T173=_T177->f2;}{struct Cyc_Absyn_Vardecl*vd=_T172;struct Cyc_Absyn_Exp*e=_T173;_T153=loc;_T154=env;_T155=vd;_T156=_T155->type;
# 736
Cyc_Binding_resolve_type(_T153,_T154,_T156);_T157=env;{
struct Cyc_Dict_Dict*old_locals=_T157->local_vars;
if(old_locals==0)goto _TL124;_T158=env;{struct Cyc_Dict_Dict*_T177=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T15A=env;_T15B=_T15A->local_vars;
*_T177=*_T15B;_T159=(struct Cyc_Dict_Dict*)_T177;}_T158->local_vars=_T159;goto _TL125;
# 741
_TL124: _T15C=env;{struct Cyc_Dict_Dict*_T177=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T15F=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T178)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T15F;_T15E=_T178;}_T160=Cyc_strptrcmp;*_T177=_T15E(_T160);_T15D=(struct Cyc_Dict_Dict*)_T177;}_T15C->local_vars=_T15D;_TL125: _T161=env;_T162=_T161->local_vars;_T163=
_check_null(_T162);_T165=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T177)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T165;_T164=_T177;}_T166=env;_T167=_T166->local_vars;_T168=*_T167;_T169=vd;_T16A=_T169->name;_T16B=_T16A->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T177=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T177->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T178=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T178->tag=3;_T178->f1=vd;_T16D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T178;}_T177->f1=(void*)_T16D;_T16C=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T177;}_T16E=(void*)_T16C;
# 742
*_T163=
_T164(_T168,_T16B,_T16E);
Cyc_Binding_resolve_exp(env,e);_T16F=env;
_T16F->local_vars=old_locals;
return;}}case 9: _T170=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T170;_T173=_T177->f1;}{struct Cyc_Absyn_Exp*e=_T173;_T173=e;goto _LL26;}default: _T171=t;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T177=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T171;_T173=_T177->f1;}_LL26: {struct Cyc_Absyn_Exp*e=_T173;
# 748
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple12{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 752
static void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;void*_T1;int*_T2;unsigned _T3;void*_T4;struct _handler_cons*_T5;int _T6;int*_T7;unsigned _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;unsigned _TA;struct _fat_ptr _TB;struct Cyc_Absyn_Pat*_TC;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TD;struct Cyc_Absyn_Pat*_TE;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_TF;void*_T10;struct Cyc_Absyn_Pat*_T11;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T12;void*_T13;struct Cyc_Binding_BindingError_exn_struct*_T14;char*_T15;char*_T16;struct Cyc_Absyn_Pat*_T17;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T18;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T19;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1A;void*_T1B;void*_T1C;struct Cyc_Binding_Env*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Pat*_T20;struct Cyc_List_List*_T21;struct _handler_cons*_T22;int _T23;int*_T24;unsigned _T25;struct Cyc_List_List*_T26;struct _tuple12*_T27;struct Cyc_List_List*_T28;void*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_Aggrdecl**_T2B;union Cyc_Absyn_AggrInfo _T2C;struct Cyc_Absyn_Pat*_T2D;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T2E;struct Cyc_Absyn_Pat*_T2F;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;unsigned _T32;struct _fat_ptr _T33;void*_T34;struct Cyc_Binding_BindingError_exn_struct*_T35;char*_T36;char*_T37;struct Cyc_Warn_Qvar_Warn_Warg_struct _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;unsigned _T3A;struct _fat_ptr _T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;void*_T42;void*_T43;int*_T44;int _T45;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T46;void*_T47;int*_T48;int _T49;void*_T4A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4B;void*_T4C;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T4D;union Cyc_Absyn_AggrInfo*_T4E;union Cyc_Absyn_AggrInfo*_T4F;struct _union_AggrInfo_UnknownAggr _T50;unsigned _T51;struct _union_AggrInfo_UnknownAggr _T52;struct _tuple2 _T53;struct _union_AggrInfo_UnknownAggr _T54;struct _tuple2 _T55;struct _handler_cons*_T56;int _T57;struct Cyc_Absyn_Aggrdecl*(*_T58)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T59;struct Cyc_Binding_Env*_T5A;struct Cyc_Binding_NSCtxt*_T5B;struct _tuple0*_T5C;union Cyc_Absyn_AggrInfo*_T5D;struct Cyc_Absyn_Aggrdecl**_T5E;void*_T5F;struct Cyc_Binding_BindingError_exn_struct*_T60;char*_T61;char*_T62;struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Absyn_Pat*_T65;unsigned _T66;struct _fat_ptr _T67;struct Cyc_Absyn_Pat*_T68;unsigned _T69;struct Cyc_Binding_Env*_T6A;void*_T6B;struct Cyc_Binding_Env*_T6C;struct Cyc_List_List*_T6D;void*_T6E;struct _tuple12*_T6F;struct _tuple12 _T70;struct Cyc_Absyn_Pat*_T71;struct Cyc_List_List*_T72;void*_T73;struct Cyc_Binding_Env*_T74;struct Cyc_List_List*_T75;void*_T76;struct Cyc_Absyn_Pat*_T77;struct Cyc_List_List*_T78;unsigned _T79;void*_T7A;_T0=p;{struct Cyc_Absyn_Pat _T7B=*_T0;_T7A=_T7B.r;_T79=_T7B.loc;}{void*r=_T7A;unsigned loc=_T79;struct Cyc_Absyn_Pat*_T7B;int _T7C;struct Cyc_List_List*_T7D;void*_T7E;_T1=r;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 14: _T4=r;{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T4;_T7E=_T7F->f1;}{struct _tuple0*qv=_T7E;{struct _handler_cons _T7F;_T5=& _T7F;_push_handler(_T5);{int _T80=0;_T6=setjmp(_T7F.handler);if(!_T6)goto _TL127;_T80=1;goto _TL128;_TL127: _TL128: if(_T80)goto _TL129;else{goto _TL12B;}_TL12B:{
# 756
void*_T81=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Enumfield*_T82;struct Cyc_Absyn_Datatypefield*_T83;void*_T84;_T7=(int*)_T81;_T8=*_T7;switch(_T8){case 0: goto _LL26;case 1:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 759
_T85.f1=_tag_fat("struct tag used without arguments in pattern",sizeof(char),45U);_T9=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T9;void*_T86[1];_T86[0]=& _T85;_TA=loc;_TB=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_TA,_TB);}_npop_handler(0);return;case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T85=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T81;_T84=_T85->f1;_T83=_T85->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T84;struct Cyc_Absyn_Datatypefield*tuf=_T83;_TC=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T85->tag=7;
# 761
_T85->f1=tud;_T85->f2=tuf;_T85->f3=0;_T85->f4=0;_TD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T85;}_TC->r=(void*)_TD;_npop_handler(0);return;}case 3:{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T85=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T81;_T84=_T85->f1;_T82=_T85->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T84;struct Cyc_Absyn_Enumfield*ef=_T82;_TE=p;{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct));_T85->tag=12;
# 763
_T85->f1=ed;_T85->f2=ef;_TF=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T85;}_TE->r=(void*)_TF;_npop_handler(0);return;}default:{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T85=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T81;_T10=_T85->f1;_T84=(void*)_T10;_T82=_T85->f2;}{void*t=_T84;struct Cyc_Absyn_Enumfield*ef=_T82;_T11=p;{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct));_T85->tag=13;
# 765
_T85->f1=t;_T85->f2=ef;_T12=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T85;}_T11->r=(void*)_T12;_npop_handler(0);return;}}_LL26:;}_pop_handler();goto _TL12A;_TL129: _T13=Cyc_Core_get_exn_thrown();{void*_T81=(void*)_T13;void*_T82;_T14=(struct Cyc_Binding_BindingError_exn_struct*)_T81;_T15=_T14->tag;_T16=Cyc_Binding_BindingError;if(_T15!=_T16)goto _TL12D;goto _LL31;_TL12D: _T82=_T81;{void*exn=_T82;_rethrow(exn);}_LL31:;}_TL12A:;}}{
# 767
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0,0);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T17=p;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T7F=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T7F->tag=1;
_T7F->f1=vd;_T19=& Cyc_Absyn_Wild_p_val;_T1A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T19;_T1B=(void*)_T1A;_T7F->f2=Cyc_Absyn_new_pat(_T1B,0U);_T18=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T7F;}_T17->r=(void*)_T18;
return;}}case 15: _T1C=r;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T1C;_T7E=_T7F->f1;_T7D=_T7F->f2;_T7C=_T7F->f3;}{struct _tuple0*qv=_T7E;struct Cyc_List_List*ps=_T7D;int dots=_T7C;{
# 773
struct Cyc_List_List*ps2=ps;_TL132: if(ps2!=0)goto _TL130;else{goto _TL131;}
_TL130: _T1D=env;_T1E=ps2;_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Pat*)_T1F;Cyc_Binding_resolve_pat(_T1D,_T20);_T21=ps2;
# 773
ps2=_T21->tl;goto _TL132;_TL131:;}{struct _handler_cons _T7F;_T22=& _T7F;_push_handler(_T22);{int _T80=0;_T23=setjmp(_T7F.handler);if(!_T23)goto _TL133;_T80=1;goto _TL134;_TL133: _TL134: if(_T80)goto _TL135;else{goto _TL137;}_TL137:{
# 775
void*_T81=Cyc_Binding_lookup_ordinary(loc,env,qv);struct Cyc_Absyn_Datatypefield*_T82;struct Cyc_Absyn_Datatypedecl*_T83;struct Cyc_Absyn_Aggrdecl*_T84;_T24=(int*)_T81;_T25=*_T24;switch(_T25){case 0: goto _LL36;case 1:{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T85=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T81;_T84=_T85->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T84;
# 778
struct Cyc_List_List*new_ps=0;
_TL13C: if(ps!=0)goto _TL13A;else{goto _TL13B;}
_TL13A:{struct Cyc_List_List*_T85=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T86=_cycalloc(sizeof(struct _tuple12));_T86->f0=0;_T28=ps;_T29=_T28->hd;_T86->f1=(struct Cyc_Absyn_Pat*)_T29;_T27=(struct _tuple12*)_T86;}_T85->hd=_T27;_T85->tl=new_ps;_T26=(struct Cyc_List_List*)_T85;}new_ps=_T26;_T2A=ps;
# 779
ps=_T2A->tl;goto _TL13C;_TL13B:{struct Cyc_Absyn_Aggrdecl**_T85=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 781
*_T85=ad;_T2B=(struct Cyc_Absyn_Aggrdecl**)_T85;}_T2C=Cyc_Absyn_KnownAggr(_T2B);{void*atype=Cyc_Absyn_aggr_type(_T2C,0);_T2D=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T85->tag=6;
_T85->f1=atype;_T85->f2=0;_T85->f3=0;_T85->f4=Cyc_List_imp_rev(new_ps);_T85->f5=dots;_T2E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T85;}_T2D->r=(void*)_T2E;_npop_handler(0);return;}}case 2:{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T85=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T81;_T83=_T85->f1;_T82=_T85->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T83;struct Cyc_Absyn_Datatypefield*tuf=_T82;_T2F=p;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_T85->tag=7;
# 785
_T85->f1=tud;_T85->f2=tuf;_T85->f3=ps;_T85->f4=dots;_T30=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T85;}_T2F->r=(void*)_T30;_npop_handler(0);return;}case 3: goto _LL40;default: _LL40:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 788
_T85.f1=_tag_fat("enum tag used with arguments in pattern",sizeof(char),40U);_T31=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T31;void*_T86[1];_T86[0]=& _T85;_T32=loc;_T33=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_T32,_T33);}_npop_handler(0);return;}_LL36:;}_pop_handler();goto _TL136;_TL135: _T34=Cyc_Core_get_exn_thrown();{void*_T81=(void*)_T34;void*_T82;_T35=(struct Cyc_Binding_BindingError_exn_struct*)_T81;_T36=_T35->tag;_T37=Cyc_Binding_BindingError;if(_T36!=_T37)goto _TL13D;goto _LL41;_TL13D: _T82=_T81;{void*exn=_T82;_rethrow(exn);}_LL41:;}_TL136:;}}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T7F;_T7F.tag=1;
# 790
_T7F.f1=qv;_T38=_T7F;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T7F=_T38;{struct Cyc_Warn_String_Warn_Warg_struct _T80;_T80.tag=0;_T80.f1=_tag_fat(" is not a constructor in pattern",sizeof(char),33U);_T39=_T80;}{struct Cyc_Warn_String_Warn_Warg_struct _T80=_T39;void*_T81[2];_T81[0]=& _T7F;_T81[1]=& _T80;_T3A=loc;_T3B=_tag_fat(_T81,sizeof(void*),2);Cyc_Warn_err2(_T3A,_T3B);}}return;}case 0:
# 792
 return;case 3: _T3C=r;{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3C;_T7E=_T7F->f1;_T7B=_T7F->f2;}{struct Cyc_Absyn_Vardecl*vd=_T7E;struct Cyc_Absyn_Pat*p2=_T7B;_T7E=vd;_T7B=p2;goto _LLD;}case 1: _T3D=r;{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T3D;_T7E=_T7F->f1;_T7B=_T7F->f2;}_LLD: {struct Cyc_Absyn_Vardecl*vd=_T7E;struct Cyc_Absyn_Pat*p2=_T7B;
# 796
Cyc_Binding_resolve_pat(env,p2);_T7E=vd;goto _LLF;}case 4: _T3E=r;{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T3E;_T7E=_T7F->f2;}_LLF: {struct Cyc_Absyn_Vardecl*vd=_T7E;_T7E=vd;goto _LL11;}case 2: _T3F=r;{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T3F;_T7E=_T7F->f2;}_LL11: {struct Cyc_Absyn_Vardecl*vd=_T7E;
# 800
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 5: _T40=r;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T40;_T7E=_T7F->f1;}{struct Cyc_Absyn_Pat*p2=_T7E;
# 802
Cyc_Binding_resolve_pat(env,p2);return;}case 16: _T41=r;{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T41;_T7E=_T7F->f1;}{struct Cyc_Absyn_Exp*e=_T7E;
Cyc_Binding_resolve_exp(env,e);return;}case 8: goto _LL19;case 9: _LL19: goto _LL1B;case 10: _LL1B: goto _LL1D;case 11: _LL1D:
# 808
 return;case 6: _T42=r;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T42;_T43=_T7F->f1;_T7E=(void*)_T43;_T7D=_T7F->f4;}{void*topt=_T7E;struct Cyc_List_List*dps=_T7D;
# 811
if(topt==0)goto _TL13F;{
void*t=topt;
void*_T7F=Cyc_Absyn_compress(t);void*_T80;_T44=(int*)_T7F;_T45=*_T44;if(_T45!=0)goto _TL141;_T46=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7F;_T47=_T46->f1;_T48=(int*)_T47;_T49=*_T48;if(_T49!=24)goto _TL143;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T81=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7F;_T4A=_T81->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T82=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4A;_T4B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7F;_T4C=_T4B->f1;_T4D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4C;_T4E=& _T4D->f1;_T80=(union Cyc_Absyn_AggrInfo*)_T4E;}}{union Cyc_Absyn_AggrInfo*info=(union Cyc_Absyn_AggrInfo*)_T80;_T4F=info;{
# 815
union Cyc_Absyn_AggrInfo _T81=*_T4F;struct _tuple0*_T82;enum Cyc_Absyn_AggrKind _T83;_T50=_T81.UnknownAggr;_T51=_T50.tag;if(_T51!=1)goto _TL145;_T52=_T81.UnknownAggr;_T53=_T52.val;_T83=_T53.f0;_T54=_T81.UnknownAggr;_T55=_T54.val;_T82=_T55.f1;{enum Cyc_Absyn_AggrKind ak=_T83;struct _tuple0*n=_T82;{struct _handler_cons _T84;_T56=& _T84;_push_handler(_T56);{int _T85=0;_T57=setjmp(_T84.handler);if(!_T57)goto _TL147;_T85=1;goto _TL148;_TL147: _TL148: if(_T85)goto _TL149;else{goto _TL14B;}_TL14B:{
# 818
struct Cyc_Absyn_Aggrdecl*(*_T86)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T58=_T86;}_T59=loc;_T5A=env;_T5B=_T5A->ns;_T5C=n;{struct Cyc_Absyn_Aggrdecl*ad=_T58(_T59,_T5B,_T5C,Cyc_Binding_lookup_aggrdecl);_T5D=info;{struct Cyc_Absyn_Aggrdecl**_T86=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
*_T86=ad;_T5E=(struct Cyc_Absyn_Aggrdecl**)_T86;}*_T5D=Cyc_Absyn_KnownAggr(_T5E);}_pop_handler();goto _TL14A;_TL149: _T5F=Cyc_Core_get_exn_thrown();{void*_T86=(void*)_T5F;void*_T87;_T60=(struct Cyc_Binding_BindingError_exn_struct*)_T86;_T61=_T60->tag;_T62=Cyc_Binding_BindingError;if(_T61!=_T62)goto _TL14C;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T88;_T88.tag=1;
# 822
_T88.f1=n;_T63=_T88;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T88=_T63;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat(" is an undefined aggregate",sizeof(char),27U);_T64=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T64;void*_T8A[2];_T8A[0]=& _T88;_T8A[1]=& _T89;_T65=p;_T66=_T65->loc;_T67=_tag_fat(_T8A,sizeof(void*),2);Cyc_Warn_err2(_T66,_T67);}}goto _LL50;_TL14C: _T87=_T86;{void*exn=_T87;_rethrow(exn);}_LL50:;}_TL14A:;}}goto _LL4B;}_TL145: goto _LL4B;_LL4B:;}goto _LL46;}_TL143: goto _LL49;_TL141: _LL49: _T68=p;_T69=_T68->loc;_T6A=env;_T6B=topt;
# 829
Cyc_Binding_resolve_type(_T69,_T6A,_T6B);goto _LL46;_LL46:;}goto _TL140;_TL13F: _TL140:
# 832
 _TL151: if(dps!=0)goto _TL14F;else{goto _TL150;}
_TL14F: _T6C=env;_T6D=dps;_T6E=_T6D->hd;_T6F=(struct _tuple12*)_T6E;_T70=*_T6F;_T71=_T70.f1;Cyc_Binding_resolve_pat(_T6C,_T71);_T72=dps;
# 832
dps=_T72->tl;goto _TL151;_TL150:
# 834
 return;}case 7: _T73=r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T7F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T73;_T7D=_T7F->f3;}{struct Cyc_List_List*ps=_T7D;
# 836
_TL155: if(ps!=0)goto _TL153;else{goto _TL154;}
_TL153: _T74=env;_T75=ps;_T76=_T75->hd;_T77=(struct Cyc_Absyn_Pat*)_T76;Cyc_Binding_resolve_pat(_T74,_T77);_T78=ps;
# 836
ps=_T78->tl;goto _TL155;_TL154:
# 838
 return;}case 12: goto _LL25;default: _LL25:
# 840
 return;}_LL3:;}}struct _tuple13{void*f0;void*f1;};
# 844
static void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){unsigned _T0;struct Cyc_Binding_Env*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple13*_T4;struct _tuple13 _T5;void*_T6;unsigned _T7;struct Cyc_Binding_Env*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple13*_TB;struct _tuple13 _TC;void*_TD;struct Cyc_List_List*_TE;
_TL159: if(po!=0)goto _TL157;else{goto _TL158;}
_TL157: _T0=loc;_T1=env;_T2=po;_T3=_T2->hd;_T4=(struct _tuple13*)_T3;_T5=*_T4;_T6=_T5.f0;Cyc_Binding_resolve_type(_T0,_T1,_T6);_T7=loc;_T8=env;_T9=po;_TA=_T9->hd;_TB=(struct _tuple13*)_TA;_TC=*_TB;_TD=_TC.f1;
Cyc_Binding_resolve_type(_T7,_T8,_TD);_TE=po;
# 845
po=_TE->tl;goto _TL159;_TL158:;}
# 851
static void Cyc_Binding_resolve_effconstr(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*effc){struct Cyc_List_List*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;struct Cyc_List_List*_T8;
_TL15D: if(effc!=0)goto _TL15B;else{goto _TL15C;}
_TL15B: _T0=effc;{void*_T9=_T0->hd;void*_TA;void*_TB;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T9;_T3=_TC->f1;_TB=(void*)_T3;}{void*e=_TB;
Cyc_Binding_resolve_type(loc,env,e);goto _LL0;}case 1:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T9;_T4=_TC->f1;_TB=(void*)_T4;_T5=_TC->f2;_TA=(void*)_T5;}{void*e1=_TB;void*e2=_TA;_TB=e1;_TA=e2;goto _LL6;}default:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TC=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T9;_T6=_TC->f1;_TB=(void*)_T6;_T7=_TC->f2;_TA=(void*)_T7;}_LL6: {void*e1=_TB;void*e2=_TA;
# 857
Cyc_Binding_resolve_type(loc,env,e1);
Cyc_Binding_resolve_type(loc,env,e2);goto _LL0;}}_LL0:;}_T8=effc;
# 852
effc=_T8->tl;goto _TL15D;_TL15C:;}
# 863
static void Cyc_Binding_resolve_qualbnd(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*qb){unsigned _T0;struct Cyc_Binding_Env*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple13*_T4;struct _tuple13 _T5;void*_T6;unsigned _T7;struct Cyc_Binding_Env*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple13*_TB;struct _tuple13 _TC;void*_TD;struct Cyc_List_List*_TE;
_TL162: if(qb!=0)goto _TL160;else{goto _TL161;}
_TL160: _T0=loc;_T1=env;_T2=qb;_T3=_T2->hd;_T4=(struct _tuple13*)_T3;_T5=*_T4;_T6=_T5.f0;Cyc_Binding_resolve_type(_T0,_T1,_T6);_T7=loc;_T8=env;_T9=qb;_TA=_T9->hd;_TB=(struct _tuple13*)_TA;_TC=*_TB;_TD=_TC.f1;
Cyc_Binding_resolve_type(_T7,_T8,_TD);_TE=qb;
# 864
qb=_TE->tl;goto _TL162;_TL161:;}struct _tuple14{struct Cyc_List_List**f0;struct Cyc_Dict_Dict*f1;struct Cyc_Binding_Env*f2;struct _tuple10*f3;};struct _tuple15{unsigned f0;struct _tuple0*f1;int f2;};
# 872
static void Cyc_Binding_export_all_symbols(struct _tuple14*cenv,struct _fat_ptr*name,void*res){struct _tuple14*_T0;struct _tuple10*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;struct _tuple0*_T7;struct _tuple0*_T8;int(*_T9)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*);int(*_TA)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_TB)(struct _tuple0*,struct _tuple0*);struct Cyc_List_List*_TC;struct _tuple0*_TD;int _TE;struct _tuple15*_TF;struct _tuple15*_T10;struct _tuple15*_T11;struct Cyc_List_List**_T12;struct Cyc_List_List*_T13;struct Cyc_List_List**_T14;struct Cyc_Dict_Dict*_T15;struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T17)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T18;struct Cyc_Dict_Dict _T19;struct _fat_ptr*_T1A;void*_T1B;void*_T1C;struct Cyc_Dict_Dict*_T1D;struct Cyc_Dict_Dict(*_T1E)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T1F)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T20;struct Cyc_Dict_Dict _T21;struct _fat_ptr*_T22;void*_T23;void*_T24;struct Cyc_Dict_Dict*_T25;struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T28;struct Cyc_Dict_Dict _T29;struct _fat_ptr*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_List_List*_T32;unsigned _T33;struct Cyc_Binding_Env*_T34;struct Cyc_Dict_Dict*_T35;struct Cyc_List_List**_T36;_T0=cenv;{struct _tuple14 _T37=*_T0;_T36=_T37.f0;_T35=_T37.f1;_T34=_T37.f2;_T1=_T37.f3;{struct _tuple10 _T38=*_T1;_T33=_T38.f0;_T32=_T38.f1;}}{struct Cyc_List_List**exlist_ptr=_T36;struct Cyc_Dict_Dict*out_dict=_T35;struct Cyc_Binding_Env*env=_T34;unsigned wcloc=_T33;struct Cyc_List_List*hidelist=_T32;struct Cyc_Absyn_Datatypefield*_T37;struct Cyc_Absyn_Datatypedecl*_T38;struct Cyc_Absyn_Enumfield*_T39;void*_T3A;_T2=res;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 0: _T5=res;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T5;_T6=_T3B->f1;_T3A=(void*)_T6;}{void*bnd=_T3A;
# 877
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T7=qv;_T7->f0=Cyc_Absyn_Rel_n(0);_T8=qv;_T8->f1=name;_TA=Cyc_List_mem;{
int(*_T3B)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*)=(int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))_TA;_T9=_T3B;}_TB=Cyc_Absyn_qvar_cmp;_TC=hidelist;_TD=qv;_TE=_T9(_TB,_TC,_TD);if(!_TE)goto _TL164;
# 880
return;_TL164:
# 882
 Cyc_Binding_absolutize_decl(wcloc,env,qv,4U);{
struct _tuple15*ex_sym;ex_sym=_cycalloc(sizeof(struct _tuple15));_TF=ex_sym;_TF->f0=wcloc;_T10=ex_sym;_T10->f1=qv;_T11=ex_sym;_T11->f2=0;_T12=exlist_ptr;{struct Cyc_List_List*_T3B=_cycalloc(sizeof(struct Cyc_List_List));
_T3B->hd=ex_sym;_T14=exlist_ptr;_T3B->tl=*_T14;_T13=(struct Cyc_List_List*)_T3B;}*_T12=_T13;_T15=out_dict;_T17=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T17;_T16=_T3B;}_T18=out_dict;_T19=*_T18;_T1A=name;_T1B=res;*_T15=_T16(_T19,_T1A,_T1B);goto _LL3;}}case 3: _T1C=res;{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T1C;_T3A=_T3B->f1;_T39=_T3B->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T3A;struct Cyc_Absyn_Enumfield*ef=_T39;_T1D=out_dict;_T1F=Cyc_Dict_insert;{
# 888
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T1F;_T1E=_T3B;}_T20=out_dict;_T21=*_T20;_T22=name;_T23=res;*_T1D=_T1E(_T21,_T22,_T23);goto _LL3;}case 1: _T24=res;{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T24;_T3A=_T3B->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T3A;_T25=out_dict;_T27=Cyc_Dict_insert;{
# 892
struct Cyc_Dict_Dict(*_T3B)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T27;_T26=_T3B;}_T28=out_dict;_T29=*_T28;_T2A=name;_T2B=res;*_T25=_T26(_T29,_T2A,_T2B);goto _LL3;}case 4: _T2C=res;{struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_T2C;_T2D=_T3B->f1;_T3A=(void*)_T2D;_T39=_T3B->f2;}{void*t=_T3A;struct Cyc_Absyn_Enumfield*ef=_T39;goto _LL3;}default: _T2E=res;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T3B=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T2E;_T38=_T3B->f1;_T37=_T3B->f2;}{struct Cyc_Absyn_Datatypedecl*dd=_T38;struct Cyc_Absyn_Datatypefield*df=_T37;_T2F=wcloc;_T30=
# 897
_tag_fat("Unexpected binding from extern C Include",sizeof(char),41U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2F,_T30,_T31);goto _LL3;}}_LL3:;}}struct _tuple16{struct Cyc_Absyn_Tqual f0;void*f1;};
# 910 "binding.cyc"
static void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;struct Cyc_Absyn_Decl*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Vardecl*_T4;struct Cyc_Absyn_Exp*_T5;int _T6;struct Cyc_Binding_Env*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Binding_Env*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;unsigned _TD;struct _fat_ptr _TE;unsigned _TF;struct Cyc_Binding_Env*_T10;struct Cyc_Absyn_Fndecl*_T11;struct _tuple0*_T12;struct Cyc_Absyn_Fndecl*_T13;enum Cyc_Absyn_Scope _T14;struct Cyc_Absyn_Fndecl*_T15;struct Cyc_Absyn_Fndecl*_T16;struct Cyc_Absyn_FnInfo _T17;unsigned _T18;struct Cyc_Binding_Env*_T19;struct Cyc_Absyn_Fndecl*_T1A;struct Cyc_Absyn_FnInfo _T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Fndecl*_T1D;struct Cyc_Absyn_FnInfo _T1E;struct Cyc_Absyn_VarargInfo*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Vardecl*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_List_List*_T26;struct Cyc_Absyn_Fndecl*_T27;struct Cyc_Core_Opt*_T28;struct Cyc_Absyn_Fndecl*_T29;int _T2A;unsigned _T2B;struct Cyc_Binding_Env*_T2C;struct Cyc_Absyn_Fndecl*_T2D;struct _tuple0*_T2E;struct Cyc_Binding_Env*_T2F;struct Cyc_Binding_Env*_T30;struct Cyc_Dict_Dict*_T31;struct Cyc_Binding_Env*_T32;struct Cyc_Dict_Dict*_T33;struct Cyc_Binding_Env*_T34;struct Cyc_Dict_Dict*_T35;struct Cyc_Dict_Dict(*_T36)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T37)(int(*)(void*,void*));int(*_T38)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T39;void*_T3A;struct Cyc_Absyn_Vardecl*_T3B;struct Cyc_Binding_Env*_T3C;struct Cyc_Dict_Dict*_T3D;struct Cyc_Dict_Dict*_T3E;struct Cyc_Dict_Dict(*_T3F)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T40)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T41;struct Cyc_Dict_Dict*_T42;struct Cyc_Dict_Dict*_T43;struct Cyc_Dict_Dict _T44;struct Cyc_List_List*_T45;void*_T46;struct Cyc_Absyn_Vardecl*_T47;struct _tuple0*_T48;struct _fat_ptr*_T49;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T4A;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T4B;struct Cyc_List_List*_T4C;void*_T4D;void*_T4E;struct Cyc_List_List*_T4F;unsigned _T50;struct Cyc_Binding_Env*_T51;struct Cyc_Absyn_Fndecl*_T52;struct Cyc_Absyn_FnInfo*_T53;struct Cyc_Absyn_FnInfo*_T54;struct Cyc_Dict_Dict*_T55;struct Cyc_Dict_Dict(*_T56)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T57)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T58;struct Cyc_Dict_Dict _T59;struct _fat_ptr*_T5A;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T5B;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T5C;void*_T5D;struct Cyc_Binding_Env*_T5E;struct Cyc_Dict_Dict*_T5F;struct Cyc_Dict_Dict*_T60;struct Cyc_Dict_Dict(*_T61)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T62)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_Env*_T63;struct Cyc_Dict_Dict*_T64;struct Cyc_Dict_Dict*_T65;struct Cyc_Dict_Dict _T66;struct _fat_ptr*_T67;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T68;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T69;void*_T6A;struct Cyc_Binding_ResolveNSEnv*(*_T6B)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T6C;struct Cyc_Binding_NSCtxt*_T6D;union Cyc_Absyn_Nmspace _T6E;struct Cyc_Binding_ResolveNSEnv*_T6F;struct Cyc_Dict_Dict(*_T70)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T71)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T72;struct Cyc_Dict_Dict _T73;struct _fat_ptr*_T74;struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T75;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T76;void*_T77;int _T78;struct Cyc_Binding_Env*_T79;struct Cyc_Absyn_Fndecl*_T7A;struct Cyc_Absyn_Stmt*_T7B;struct Cyc_Binding_Env*_T7C;int _T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;unsigned _T7F;struct _fat_ptr _T80;unsigned _T81;struct Cyc_Binding_Env*_T82;struct Cyc_List_List*_T83;void*_T84;struct Cyc_Absyn_Vardecl*_T85;struct Cyc_List_List*_T86;int _T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;unsigned _T89;struct _fat_ptr _T8A;unsigned _T8B;struct Cyc_Binding_Env*_T8C;struct Cyc_Absyn_Typedefdecl*_T8D;struct _tuple0*_T8E;enum Cyc_Absyn_Scope _T8F;struct Cyc_Absyn_Typedefdecl*_T90;int _T91;struct Cyc_Absyn_Typedefdecl*_T92;struct Cyc_Binding_ResolveNSEnv*(*_T93)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T94;struct Cyc_Binding_NSCtxt*_T95;union Cyc_Absyn_Nmspace _T96;struct Cyc_Absyn_Typedefdecl*_T97;void*_T98;unsigned _T99;struct Cyc_Binding_Env*_T9A;struct Cyc_Absyn_Typedefdecl*_T9B;void*_T9C;struct Cyc_Binding_Env*_T9D;int _T9E;struct Cyc_Binding_ResolveNSEnv*_T9F;struct Cyc_Dict_Dict(*_TA0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_TA1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TA2;struct Cyc_Dict_Dict _TA3;struct _fat_ptr*_TA4;struct Cyc_Absyn_Typedefdecl*_TA5;int _TA6;struct Cyc_Warn_String_Warn_Warg_struct _TA7;unsigned _TA8;struct _fat_ptr _TA9;unsigned _TAA;struct Cyc_Binding_Env*_TAB;struct Cyc_Absyn_Aggrdecl*_TAC;struct _tuple0*_TAD;struct Cyc_Absyn_Aggrdecl*_TAE;enum Cyc_Absyn_Scope _TAF;struct Cyc_Absyn_Aggrdecl*_TB0;struct Cyc_Binding_ResolveNSEnv*(*_TB1)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TB2;struct Cyc_Binding_NSCtxt*_TB3;union Cyc_Absyn_Nmspace _TB4;int(*_TB5)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TB6)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TB7;struct Cyc_Dict_Dict _TB8;struct _fat_ptr*_TB9;int _TBA;struct Cyc_Absyn_Aggrdecl*_TBB;struct Cyc_Absyn_AggrdeclImpl*_TBC;struct Cyc_Binding_Env*_TBD;int _TBE;struct Cyc_Binding_ResolveNSEnv*_TBF;struct Cyc_Dict_Dict(*_TC0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*);struct Cyc_Dict_Dict(*_TC1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TC2;struct Cyc_Dict_Dict _TC3;struct _fat_ptr*_TC4;struct Cyc_Absyn_Aggrdecl*_TC5;struct Cyc_Binding_ResolveNSEnv*_TC6;struct Cyc_Dict_Dict(*_TC7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TC8)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TC9;struct Cyc_Dict_Dict _TCA;struct _fat_ptr*_TCB;struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TCC;void*_TCD;struct Cyc_Absyn_Aggrdecl*_TCE;struct Cyc_Absyn_AggrdeclImpl*_TCF;struct Cyc_Absyn_Aggrdecl*_TD0;int _TD1;struct Cyc_Warn_String_Warn_Warg_struct _TD2;unsigned _TD3;struct _fat_ptr _TD4;unsigned _TD5;struct Cyc_Binding_Env*_TD6;struct Cyc_Absyn_Enumdecl*_TD7;struct _tuple0*_TD8;struct Cyc_Absyn_Enumdecl*_TD9;enum Cyc_Absyn_Scope _TDA;struct Cyc_Absyn_Enumdecl*_TDB;struct Cyc_Binding_ResolveNSEnv*(*_TDC)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_TDD;struct Cyc_Binding_NSCtxt*_TDE;union Cyc_Absyn_Nmspace _TDF;int(*_TE0)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_TE1)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_TE2;struct Cyc_Dict_Dict _TE3;struct _fat_ptr*_TE4;int _TE5;struct Cyc_Absyn_Enumdecl*_TE6;struct Cyc_Core_Opt*_TE7;struct Cyc_Binding_Env*_TE8;int _TE9;struct Cyc_Binding_ResolveNSEnv*_TEA;struct Cyc_Dict_Dict(*_TEB)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*);struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_TED;struct Cyc_Dict_Dict _TEE;struct _fat_ptr*_TEF;struct Cyc_Absyn_Enumdecl*_TF0;struct Cyc_Absyn_Enumdecl*_TF1;struct Cyc_Core_Opt*_TF2;struct Cyc_Absyn_Enumdecl*_TF3;struct Cyc_Core_Opt*_TF4;void*_TF5;struct Cyc_List_List*_TF6;void*_TF7;struct Cyc_Absyn_Enumfield*_TF8;unsigned _TF9;struct Cyc_Binding_Env*_TFA;struct Cyc_Absyn_Enumfield*_TFB;struct _tuple0*_TFC;struct Cyc_Absyn_Enumdecl*_TFD;enum Cyc_Absyn_Scope _TFE;struct Cyc_Absyn_Enumfield*_TFF;struct Cyc_Absyn_Exp*_T100;struct Cyc_Binding_Env*_T101;struct Cyc_Absyn_Enumfield*_T102;struct Cyc_Absyn_Exp*_T103;struct Cyc_Binding_Env*_T104;int _T105;struct Cyc_Binding_ResolveNSEnv*_T106;struct Cyc_Dict_Dict(*_T107)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T108)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T109;struct Cyc_Dict_Dict _T10A;struct Cyc_Absyn_Enumfield*_T10B;struct _tuple0*_T10C;struct _tuple0 _T10D;struct _fat_ptr*_T10E;struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T10F;void*_T110;struct Cyc_List_List*_T111;struct _handler_cons*_T112;int _T113;struct Cyc_List_List*(*_T114)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*));unsigned _T115;struct Cyc_Binding_Env*_T116;struct Cyc_Binding_NSCtxt*_T117;struct Cyc_Absyn_Datatypedecl*_T118;struct _tuple0*_T119;struct Cyc_List_List*_T11A;void*_T11B;struct Cyc_Absyn_Datatypedecl*_T11C;int _T11D;struct Cyc_Binding_BindingError_exn_struct*_T11E;void*_T11F;struct Cyc_Absyn_Datatypedecl*_T120;struct Cyc_Absyn_Datatypedecl*_T121;struct Cyc_Absyn_Datatypedecl*_T122;struct Cyc_Absyn_Datatypedecl*_T123;struct Cyc_Binding_ResolveNSEnv*(*_T124)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T125;struct Cyc_Binding_NSCtxt*_T126;union Cyc_Absyn_Nmspace _T127;struct Cyc_Binding_ResolveNSEnv*_T128;struct Cyc_Dict_Dict(*_T129)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T12A)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T12B;struct Cyc_Dict_Dict _T12C;struct _fat_ptr*_T12D;struct Cyc_List_List*_T12E;void*_T12F;struct Cyc_Binding_BindingError_exn_struct*_T130;char*_T131;char*_T132;unsigned _T133;struct Cyc_Binding_Env*_T134;struct Cyc_Absyn_Datatypedecl*_T135;struct _tuple0*_T136;struct Cyc_Absyn_Datatypedecl*_T137;enum Cyc_Absyn_Scope _T138;struct Cyc_Absyn_Datatypedecl*_T139;struct Cyc_Binding_ResolveNSEnv*(*_T13A)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T13B;struct Cyc_Binding_NSCtxt*_T13C;union Cyc_Absyn_Nmspace _T13D;int(*_T13E)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T13F)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_ResolveNSEnv*_T140;struct Cyc_Dict_Dict _T141;struct _fat_ptr*_T142;int _T143;struct Cyc_Absyn_Datatypedecl*_T144;struct Cyc_Core_Opt*_T145;struct Cyc_Binding_ResolveNSEnv*_T146;struct Cyc_Dict_Dict(*_T147)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*);struct Cyc_Dict_Dict(*_T148)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T149;struct Cyc_Dict_Dict _T14A;struct _fat_ptr*_T14B;struct Cyc_List_List*_T14C;struct Cyc_Absyn_Datatypedecl*_T14D;struct Cyc_Absyn_Datatypedecl*_T14E;struct Cyc_Core_Opt*_T14F;struct Cyc_Absyn_Datatypedecl*_T150;struct Cyc_Core_Opt*_T151;void*_T152;struct Cyc_List_List*_T153;void*_T154;struct Cyc_Absyn_Datatypefield*_T155;unsigned _T156;struct Cyc_Binding_Env*_T157;struct Cyc_List_List*_T158;void*_T159;struct _tuple16*_T15A;struct _tuple16 _T15B;void*_T15C;struct Cyc_List_List*_T15D;struct Cyc_Absyn_Datatypefield*_T15E;struct _tuple0*_T15F;struct _tuple0 _T160;struct _union_Nmspace_Abs_n _T161;unsigned _T162;struct _union_Nmspace_Rel_n _T163;struct Cyc_List_List*_T164;struct Cyc_Absyn_Datatypedecl*_T165;int _T166;struct Cyc_Absyn_Datatypefield*_T167;struct _tuple0*_T168;struct Cyc_Binding_Env*_T169;struct Cyc_Binding_NSCtxt*_T16A;struct Cyc_List_List*_T16B;struct Cyc_Absyn_Datatypefield*_T16C;struct _tuple0*_T16D;struct Cyc_Absyn_Datatypedecl*_T16E;struct _tuple0*_T16F;struct _tuple0 _T170;struct Cyc_Warn_String_Warn_Warg_struct _T171;struct Cyc_Absyn_Datatypefield*_T172;unsigned _T173;struct _fat_ptr _T174;int(*_T175)(struct _fat_ptr,struct _fat_ptr);void*(*_T176)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T177;struct _fat_ptr _T178;struct Cyc_Binding_ResolveNSEnv*(*_T179)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace);struct Cyc_Binding_Env*_T17A;struct Cyc_Binding_NSCtxt*_T17B;struct Cyc_Absyn_Datatypefield*_T17C;struct _tuple0*_T17D;struct _tuple0 _T17E;union Cyc_Absyn_Nmspace _T17F;struct Cyc_Binding_ResolveNSEnv*_T180;struct Cyc_Dict_Dict(*_T181)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T182)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Binding_ResolveNSEnv*_T183;struct Cyc_Dict_Dict _T184;struct Cyc_Absyn_Datatypefield*_T185;struct _tuple0*_T186;struct _tuple0 _T187;struct _fat_ptr*_T188;struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T189;void*_T18A;struct Cyc_List_List*_T18B;void(*_T18C)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T18D;struct Cyc_Binding_NSCtxt*_T18E;struct _fat_ptr*_T18F;struct Cyc_Binding_Env*_T190;struct Cyc_Binding_NSCtxt*_T191;struct Cyc_Absyn_Decl*_T192;unsigned _T193;struct Cyc_Binding_Env*_T194;struct Cyc_Binding_NSCtxt*_T195;struct _tuple0*_T196;struct Cyc_Binding_Env*_T197;struct Cyc_Binding_NSCtxt*_T198;struct Cyc_Binding_Env*_T199;struct Cyc_Binding_Env*_T19A;struct Cyc_Binding_Env*_T19B;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T19C;struct Cyc_List_List**_T19D;struct Cyc_List_List**_T19E;struct Cyc_List_List*_T19F;void*_T1A0;struct Cyc_List_List*_T1A1;int _T1A2;struct Cyc_Warn_String_Warn_Warg_struct _T1A3;unsigned _T1A4;struct _fat_ptr _T1A5;struct Cyc_Binding_ResolveNSEnv*(*_T1A6)(struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1A7)(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Env*_T1A8;struct Cyc_Binding_NSCtxt*_T1A9;struct Cyc_Dict_Dict _T1AA;struct Cyc_Binding_Env*_T1AB;struct Cyc_Binding_NSCtxt*_T1AC;struct Cyc_List_List*_T1AD;struct Cyc_Binding_ResolveNSEnv*_T1AE;struct Cyc_Binding_Env*_T1AF;struct Cyc_Binding_Env*_T1B0;struct Cyc_Binding_Env*_T1B1;struct Cyc_Binding_Env*_T1B2;struct Cyc_Binding_ResolveNSEnv*_T1B3;struct _tuple10*_T1B4;struct _tuple10 _T1B5;unsigned _T1B6;struct Cyc_List_List**_T1B7;struct Cyc_List_List*_T1B8;unsigned _T1B9;struct _tuple10*_T1BA;struct _tuple10 _T1BB;unsigned _T1BC;struct _fat_ptr _T1BD;struct _fat_ptr _T1BE;struct Cyc_Binding_Env*_T1BF;void(*_T1C0)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict);void(*_T1C1)(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple14*_T1C2;struct Cyc_Dict_Dict*_T1C3;struct Cyc_Dict_Dict _T1C4;struct Cyc_List_List**_T1C5;struct Cyc_List_List*_T1C6;void*_T1C7;struct _tuple0*_T1C8;int(*_T1C9)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1CA)(struct Cyc_Dict_Dict,void*);int _T1CB;int(*_T1CC)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T1CD)(struct Cyc_Dict_Dict,void*);int _T1CE;void*(*_T1CF)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1D0)(struct Cyc_Dict_Dict,void*);void*_T1D1;void*(*_T1D2)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1D3)(struct Cyc_Dict_Dict,void*);void*_T1D4;struct Cyc_String_pa_PrintArg_struct _T1D5;struct _fat_ptr*_T1D6;unsigned _T1D7;struct _fat_ptr _T1D8;struct _fat_ptr _T1D9;struct Cyc_Dict_Dict(*_T1DA)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_T1DB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict _T1DC;struct _fat_ptr*_T1DD;void*(*_T1DE)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T1DF)(struct Cyc_Dict_Dict,void*);void*_T1E0;struct Cyc_List_List*_T1E1;struct Cyc_Binding_ResolveNSEnv*_T1E2;struct Cyc_Binding_Env*_T1E3;_T0=d;{
# 913
unsigned loc=_T0->loc;_T1=d;{
void*_T1E4=_T1->r;struct _tuple10*_T1E5;struct Cyc_List_List*_T1E6;struct Cyc_List_List*_T1E7;struct Cyc_Absyn_Vardecl*_T1E8;struct Cyc_Absyn_Exp*_T1E9;void*_T1EA;_T2=(int*)_T1E4;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1EA;
# 917
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);_T4=vd;_T5=_T4->initializer;
# 922
if(_T5==0)goto _TL167;_T6=Cyc_Binding_in_cinclude(env);if(_T6)goto _TL167;else{goto _TL169;}
_TL169: _T7=env;_T8=vd;_T9=_T8->initializer;Cyc_Binding_resolve_exp(_T7,_T9);goto _TL168;_TL167: _TL168: goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Fndecl*fd=_T1EA;_TA=env;_TB=_TA->in_cyc_override;
# 927
if(!_TB)goto _TL16A;{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;
_T1EB.f1=_tag_fat("Function declaration not permitted within cyclone_override block",sizeof(char),65U);_TC=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_TC;void*_T1EC[1];_T1EC[0]=& _T1EB;_TD=loc;_TE=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_TD,_TE);}goto _TL16B;_TL16A: _TL16B: _TF=loc;_T10=env;_T11=fd;_T12=_T11->name;_T13=fd;_T14=_T13->sc;
Cyc_Binding_absolutize_decl(_TF,_T10,_T12,_T14);_T15=fd;{
struct _tuple0*_T1EB=_T15->name;struct _fat_ptr*_T1EC;union Cyc_Absyn_Nmspace _T1ED;{struct _tuple0 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1ED;struct _fat_ptr*decl_name=_T1EC;_T16=fd;_T17=_T16->i;{
struct Cyc_List_List*argit=_T17.args;_T18=loc;_T19=env;_T1A=fd;_T1B=_T1A->i;_T1C=_T1B.args;_T1D=fd;_T1E=_T1D->i;_T1F=_T1E.cyc_varargs;{
struct Cyc_List_List*vdsopts=Cyc_Binding_get_fun_vardecls(1,_T18,_T19,_T1C,_T1F);
struct Cyc_List_List*vds=0;{
struct Cyc_List_List*v=vdsopts;_TL16F: if(v!=0)goto _TL16D;else{goto _TL16E;}
_TL16D: _T20=v;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Vardecl*)_T21;if(_T22==0)goto _TL170;{struct Cyc_List_List*_T1EE=_cycalloc(sizeof(struct Cyc_List_List));_T24=v;_T25=_T24->hd;_T1EE->hd=(struct Cyc_Absyn_Vardecl*)_T25;_T1EE->tl=vds;_T23=(struct Cyc_List_List*)_T1EE;}vds=_T23;goto _TL171;_TL170: _TL171: _T26=v;
# 934
v=_T26->tl;goto _TL16F;_TL16E:;}
# 936
vds=Cyc_List_imp_rev(vds);_T27=fd;{struct Cyc_Core_Opt*_T1EE=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T1EE->v=vds;_T28=(struct Cyc_Core_Opt*)_T1EE;}_T27->param_vardecls=_T28;_T29=fd;
_T29->i.arg_vardecls=vdsopts;_T2A=Cyc_Flags_warn_override;
if(!_T2A)goto _TL172;_T2B=loc;_T2C=env;_T2D=fd;_T2E=_T2D->name;
Cyc_Binding_check_warn_override(_T2B,_T2C,_T2E);goto _TL173;_TL172: _TL173: _T2F=env;{
# 942
struct Cyc_Dict_Dict*old_locals=_T2F->local_vars;
if(old_locals==0)goto _TL174;_T30=env;{struct Cyc_Dict_Dict*_T1EE=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T32=env;_T33=_T32->local_vars;
*_T1EE=*_T33;_T31=(struct Cyc_Dict_Dict*)_T1EE;}_T30->local_vars=_T31;goto _TL175;
# 946
_TL174: _T34=env;{struct Cyc_Dict_Dict*_T1EE=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T37=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T1EF)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T37;_T36=_T1EF;}_T38=Cyc_strptrcmp;*_T1EE=_T36(_T38);_T35=(struct Cyc_Dict_Dict*)_T1EE;}_T34->local_vars=_T35;_TL175:{
struct Cyc_List_List*vds1=vds;_TL179: if(vds1!=0)goto _TL177;else{goto _TL178;}
_TL177: _T39=vds1;_T3A=_T39->hd;_T3B=(struct Cyc_Absyn_Vardecl*)_T3A;if(_T3B==0)goto _TL17A;_T3C=env;_T3D=_T3C->local_vars;_T3E=
_check_null(_T3D);_T40=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T40;_T3F=_T1EE;}_T41=env;_T42=_T41->local_vars;_T43=_check_null(_T42);_T44=*_T43;_T45=vds1;_T46=_T45->hd;_T47=(struct Cyc_Absyn_Vardecl*)_T46;_T48=_T47->name;_T49=_T48->f1;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EE->tag=0;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_T1EF->tag=3;_T4C=vds1;_T4D=_T4C->hd;
_T1EF->f1=(struct Cyc_Absyn_Vardecl*)_T4D;_T4B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T1EF;}_T1EE->f1=(void*)_T4B;_T4A=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EE;}_T4E=(void*)_T4A;
# 949
*_T3E=_T3F(_T44,_T49,_T4E);goto _TL17B;_TL17A: _TL17B: _T4F=vds1;
# 947
vds1=_T4F->tl;goto _TL179;_TL178:;}_T50=loc;_T51=env;_T52=fd;_T53=& _T52->i;_T54=(struct Cyc_Absyn_FnInfo*)_T53;
# 952
Cyc_Binding_resolve_function_stuff(_T50,_T51,_T54);
# 954
if(old_locals==0)goto _TL17C;_T55=old_locals;_T57=Cyc_Dict_insert;{
# 956
struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T57;_T56=_T1EE;}_T58=old_locals;_T59=*_T58;_T5A=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EE->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1EF->tag=2;
_T1EF->f1=fd;_T5C=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1EF;}_T1EE->f1=(void*)_T5C;_T5B=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EE;}_T5D=(void*)_T5B;
# 956
*_T55=_T56(_T59,_T5A,_T5D);_T5E=env;_T5F=_T5E->local_vars;_T60=
# 958
_check_null(_T5F);_T62=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T62;_T61=_T1EE;}_T63=env;_T64=_T63->local_vars;_T65=_check_null(_T64);_T66=*_T65;_T67=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EE->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1EF->tag=2;
_T1EF->f1=fd;_T69=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1EF;}_T1EE->f1=(void*)_T69;_T68=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EE;}_T6A=(void*)_T68;
# 958
*_T60=_T61(_T66,_T67,_T6A);goto _TL17D;
# 961
_TL17C:{struct Cyc_Binding_ResolveNSEnv*(*_T1EE)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T6B=_T1EE;}_T6C=env;_T6D=_T6C->ns;_T6E=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T6B(_T6D,_T6E);_T6F=decl_ns_data;_T71=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T71;_T70=_T1EE;}_T72=decl_ns_data;_T73=_T72->ordinaries;_T74=decl_name;{struct Cyc_Binding_VarRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));_T1EE->tag=0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1EF=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T1EF->tag=2;
# 964
_T1EF->f1=fd;_T76=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T1EF;}_T1EE->f1=(void*)_T76;_T75=(struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_T1EE;}_T77=(void*)_T75;
# 962
_T6F->ordinaries=_T70(_T73,_T74,_T77);}_TL17D: _T78=
# 967
Cyc_Binding_in_cinclude(env);if(_T78)goto _TL17E;else{goto _TL180;}
_TL180: _T79=env;_T7A=fd;_T7B=_T7A->body;Cyc_Binding_resolve_stmt(_T79,_T7B);goto _TL17F;_TL17E: _TL17F: _T7C=env;
# 970
_T7C->local_vars=old_locals;goto _LL0;}}}}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;_T1E9=_T1EB->f3;}{struct Cyc_Absyn_Pat*p=_T1EA;struct Cyc_Absyn_Exp*exp=_T1E9;_T7D=
# 974
Cyc_Binding_at_toplevel(env);if(!_T7D)goto _TL181;{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;
_T1EB.f1=_tag_fat("let not allowed at top-level",sizeof(char),29U);_T7E=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_T7E;void*_T1EC[1];_T1EC[0]=& _T1EB;_T7F=loc;_T80=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_T7F,_T80);}goto _LL0;_TL181:
# 978
 Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);goto _LL0;}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_List_List*vds=_T1EA;
# 983
_TL186: if(vds!=0)goto _TL184;else{goto _TL185;}
_TL184: _T81=loc;_T82=env;_T83=vds;_T84=_T83->hd;_T85=(struct Cyc_Absyn_Vardecl*)_T84;Cyc_Binding_resolve_and_add_vardecl(_T81,_T82,_T85);_T86=vds;
# 983
vds=_T86->tl;goto _TL186;_TL185: goto _LL0;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;_T1E8=_T1EB->f2;_T1E9=_T1EB->f3;}{struct Cyc_Absyn_Tvar*tv=_T1EA;struct Cyc_Absyn_Vardecl*vd=_T1E8;struct Cyc_Absyn_Exp*open_exp_opt=_T1E9;
# 988
if(open_exp_opt==0)goto _TL187;
Cyc_Binding_resolve_exp(env,open_exp_opt);goto _TL188;_TL187: _TL188:
 Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T1EA;_T87=
# 994
Cyc_Binding_at_toplevel(env);if(_T87)goto _TL189;else{goto _TL18B;}
_TL18B:{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;_T1EB.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_T88=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_T88;void*_T1EC[1];_T1EC[0]=& _T1EB;_T89=loc;_T8A=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_T89,_T8A);}goto _LL0;_TL189: _T8B=loc;_T8C=env;_T8D=td;_T8E=_T8D->name;_T90=td;_T91=_T90->extern_c;
# 998
if(!_T91)goto _TL18C;_T8F=4U;goto _TL18D;_TL18C: _T8F=2U;_TL18D: Cyc_Binding_absolutize_decl(_T8B,_T8C,_T8E,_T8F);_T92=td;{
struct _tuple0*_T1EB=_T92->name;struct _fat_ptr*_T1EC;union Cyc_Absyn_Nmspace _T1ED;{struct _tuple0 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1ED;struct _fat_ptr*decl_name=_T1EC;{
# 1001
struct Cyc_Binding_ResolveNSEnv*(*_T1EE)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T93=_T1EE;}_T94=env;_T95=_T94->ns;_T96=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T93(_T95,_T96);_T97=td;_T98=_T97->defn;
if(_T98==0)goto _TL18E;_T99=loc;_T9A=env;_T9B=td;_T9C=_T9B->defn;
Cyc_Binding_resolve_type(_T99,_T9A,_T9C);goto _TL18F;_TL18E: _TL18F: _T9D=env;_T9E=_T9D->in_cyc_override;
# 1005
if(_T9E)goto _TL190;else{goto _TL192;}
_TL192: _T9F=decl_ns_data;_TA1=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))_TA1;_TA0=_T1EE;}_TA2=decl_ns_data;_TA3=_TA2->typedefs;_TA4=decl_name;_TA5=td;_T9F->typedefs=_TA0(_TA3,_TA4,_TA5);goto _TL191;_TL190: _TL191: goto _LL0;}}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T1EA;_TA6=
# 1012
Cyc_Binding_at_toplevel(env);if(_TA6)goto _TL193;else{goto _TL195;}
_TL195:{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;_T1EB.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TA7=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_TA7;void*_T1EC[1];_T1EC[0]=& _T1EB;_TA8=loc;_TA9=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_TA8,_TA9);}goto _LL0;_TL193: _TAA=loc;_TAB=env;_TAC=ad;_TAD=_TAC->name;_TAE=ad;_TAF=_TAE->sc;
# 1016
Cyc_Binding_absolutize_decl(_TAA,_TAB,_TAD,_TAF);_TB0=ad;{
struct _tuple0*_T1EB=_TB0->name;struct _fat_ptr*_T1EC;union Cyc_Absyn_Nmspace _T1ED;{struct _tuple0 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1ED;struct _fat_ptr*decl_name=_T1EC;{
# 1019
struct Cyc_Binding_ResolveNSEnv*(*_T1EE)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TB1=_T1EE;}_TB2=env;_TB3=_TB2->ns;_TB4=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TB1(_TB3,_TB4);_TB6=Cyc_Dict_member;{
# 1021
int(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TB6;_TB5=_T1EE;}_TB7=decl_ns_data;_TB8=_TB7->aggrdecls;_TB9=decl_name;_TBA=_TB5(_TB8,_TB9);if(!_TBA)goto _TL196;_TBB=ad;_TBC=_TBB->impl;if(_TBC!=0)goto _TL196;goto _LL0;_TL196: _TBD=env;_TBE=_TBD->in_cyc_override;
# 1024
if(_TBE)goto _TL198;else{goto _TL19A;}
_TL19A: _TBF=decl_ns_data;_TC1=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))_TC1;_TC0=_T1EE;}_TC2=decl_ns_data;_TC3=_TC2->aggrdecls;_TC4=decl_name;_TC5=ad;_TBF->aggrdecls=_TC0(_TC3,_TC4,_TC5);_TC6=decl_ns_data;_TC8=Cyc_Dict_insert;{
# 1027
struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TC8;_TC7=_T1EE;}_TC9=decl_ns_data;_TCA=_TC9->ordinaries;_TCB=decl_name;{struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_AggrRes_Binding_Resolved_struct));_T1EE->tag=1;
_T1EE->f1=ad;_TCC=(struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_T1EE;}_TCD=(void*)_TCC;
# 1027
_TC6->ordinaries=_TC7(_TCA,_TCB,_TCD);goto _TL199;_TL198: _TL199: _TCE=ad;_TCF=_TCE->impl;
# 1030
if(_TCF==0)goto _TL19B;_TD0=ad;{
struct Cyc_Absyn_AggrdeclImpl*_T1EE=_TD0->impl;struct Cyc_List_List*_T1EF;struct Cyc_List_List*_T1F0;struct Cyc_List_List*_T1F1;{struct Cyc_Absyn_AggrdeclImpl _T1F2=*_T1EE;_T1F1=_T1F2.qual_bnd;_T1F0=_T1F2.fields;_T1EF=_T1F2.effconstr;}{struct Cyc_List_List*qb=_T1F1;struct Cyc_List_List*fs=_T1F0;struct Cyc_List_List*fc=_T1EF;
Cyc_Binding_resolve_effconstr(loc,env,fc);
# 1034
Cyc_Binding_resolve_qualbnd(loc,env,qb);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}goto _TL19C;_TL19B: _TL19C: goto _LL0;}}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T1EA;_TD1=
# 1040
Cyc_Binding_at_toplevel(env);if(_TD1)goto _TL19D;else{goto _TL19F;}
_TL19F:{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;_T1EB.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_TD2=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_TD2;void*_T1EC[1];_T1EC[0]=& _T1EB;_TD3=loc;_TD4=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_TD3,_TD4);}goto _LL0;_TL19D: _TD5=loc;_TD6=env;_TD7=ed;_TD8=_TD7->name;_TD9=ed;_TDA=_TD9->sc;
# 1044
Cyc_Binding_absolutize_decl(_TD5,_TD6,_TD8,_TDA);_TDB=ed;{
struct _tuple0*_T1EB=_TDB->name;struct _fat_ptr*_T1EC;union Cyc_Absyn_Nmspace _T1ED;{struct _tuple0 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1ED;struct _fat_ptr*decl_name=_T1EC;{
# 1047
struct Cyc_Binding_ResolveNSEnv*(*_T1EE)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_TDC=_T1EE;}_TDD=env;_TDE=_TDD->ns;_TDF=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_TDC(_TDE,_TDF);_TE1=Cyc_Dict_member;{
# 1049
int(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_TE1;_TE0=_T1EE;}_TE2=decl_ns_data;_TE3=_TE2->enumdecls;_TE4=decl_name;_TE5=_TE0(_TE3,_TE4);if(!_TE5)goto _TL1A0;_TE6=ed;_TE7=_TE6->fields;if(_TE7!=0)goto _TL1A0;goto _LL0;_TL1A0: _TE8=env;_TE9=_TE8->in_cyc_override;
# 1053
if(_TE9)goto _TL1A2;else{goto _TL1A4;}
_TL1A4: _TEA=decl_ns_data;_TEC=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))_TEC;_TEB=_T1EE;}_TED=decl_ns_data;_TEE=_TED->enumdecls;_TEF=decl_name;_TF0=ed;_TEA->enumdecls=_TEB(_TEE,_TEF,_TF0);goto _TL1A3;_TL1A2: _TL1A3: _TF1=ed;_TF2=_TF1->fields;
# 1056
if(_TF2==0)goto _TL1A5;_TF3=ed;_TF4=_TF3->fields;_TF5=_TF4->v;{
# 1058
struct Cyc_List_List*fs=(struct Cyc_List_List*)_TF5;_TL1AA: if(fs!=0)goto _TL1A8;else{goto _TL1A9;}
_TL1A8: _TF6=fs;_TF7=_TF6->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_TF7;_TF8=f;_TF9=_TF8->loc;_TFA=env;_TFB=f;_TFC=_TFB->name;_TFD=ed;_TFE=_TFD->sc;
Cyc_Binding_absolutize_decl(_TF9,_TFA,_TFC,_TFE);_TFF=f;_T100=_TFF->tag;
if(_T100==0)goto _TL1AB;_T101=env;_T102=f;_T103=_T102->tag;
Cyc_Binding_resolve_exp(_T101,_T103);goto _TL1AC;_TL1AB: _TL1AC: _T104=env;_T105=_T104->in_cyc_override;
if(_T105)goto _TL1AD;else{goto _TL1AF;}
_TL1AF: _T106=decl_ns_data;_T108=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T108;_T107=_T1EE;}_T109=decl_ns_data;_T10A=_T109->ordinaries;_T10B=f;_T10C=_T10B->name;_T10D=*_T10C;_T10E=_T10D.f1;{struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_EnumRes_Binding_Resolved_struct));_T1EE->tag=3;
# 1066
_T1EE->f1=ed;_T1EE->f2=f;_T10F=(struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_T1EE;}_T110=(void*)_T10F;
# 1064
_T106->ordinaries=_T107(_T10A,_T10E,_T110);goto _TL1AE;_TL1AD: _TL1AE:;}_T111=fs;
# 1058
fs=_T111->tl;goto _TL1AA;_TL1A9:;}goto _TL1A6;_TL1A5: _TL1A6: goto _LL0;}}}}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T1EA;{struct _handler_cons _T1EB;_T112=& _T1EB;_push_handler(_T112);{int _T1EC=0;_T113=setjmp(_T1EB.handler);if(!_T113)goto _TL1B0;_T1EC=1;goto _TL1B1;_TL1B0: _TL1B1: if(_T1EC)goto _TL1B2;else{goto _TL1B4;}_TL1B4:{
# 1073
struct Cyc_List_List*(*_T1ED)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*))=(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;_T114=_T1ED;}_T115=loc;_T116=env;_T117=_T116->ns;_T118=tud;_T119=_T118->name;{struct Cyc_List_List*decls=_T114(_T115,_T117,_T119,Cyc_Binding_lookup_datatypedecl);_T11A=decls;_T11B=_T11A->hd;{
struct Cyc_Absyn_Datatypedecl*last_decl=(struct Cyc_Absyn_Datatypedecl*)_T11B;_T11C=last_decl;_T11D=_T11C->is_extensible;
if(_T11D)goto _TL1B5;else{goto _TL1B7;}
_TL1B7:{struct Cyc_Binding_BindingError_exn_struct*_T1ED=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_T1ED->tag=Cyc_Binding_BindingError;_T11E=(struct Cyc_Binding_BindingError_exn_struct*)_T1ED;}_T11F=(void*)_T11E;_throw(_T11F);goto _TL1B6;_TL1B5: _TL1B6: _T120=tud;_T121=last_decl;
_T120->name=_T121->name;_T122=tud;
_T122->is_extensible=1;_T123=tud;{
struct _tuple0*_T1ED=_T123->name;struct _fat_ptr*_T1EE;union Cyc_Absyn_Nmspace _T1EF;{struct _tuple0 _T1F0=*_T1ED;_T1EF=_T1F0.f0;_T1EE=_T1F0.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1EF;struct _fat_ptr*decl_name=_T1EE;{
struct Cyc_Binding_ResolveNSEnv*(*_T1F0)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T124=_T1F0;}_T125=env;_T126=_T125->ns;_T127=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T124(_T126,_T127);_T128=decl_ns_data;_T12A=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1F0)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T12A;_T129=_T1F0;}_T12B=decl_ns_data;_T12C=_T12B->datatypedecls;_T12D=decl_name;{struct Cyc_List_List*_T1F0=_cycalloc(sizeof(struct Cyc_List_List));
_T1F0->hd=tud;_T1F0->tl=decls;_T12E=(struct Cyc_List_List*)_T1F0;}
# 1081
_T128->datatypedecls=_T129(_T12C,_T12D,_T12E);}}}}}_pop_handler();goto _TL1B3;_TL1B2: _T12F=Cyc_Core_get_exn_thrown();{void*_T1ED=(void*)_T12F;void*_T1EE;_T130=(struct Cyc_Binding_BindingError_exn_struct*)_T1ED;_T131=_T130->tag;_T132=Cyc_Binding_BindingError;if(_T131!=_T132)goto _TL1B8;_T133=loc;_T134=env;_T135=tud;_T136=_T135->name;_T137=tud;_T138=_T137->sc;
# 1085
Cyc_Binding_absolutize_topdecl(_T133,_T134,_T136,_T138);_T139=tud;{
struct _tuple0*_T1EF=_T139->name;struct _fat_ptr*_T1F0;union Cyc_Absyn_Nmspace _T1F1;{struct _tuple0 _T1F2=*_T1EF;_T1F1=_T1F2.f0;_T1F0=_T1F2.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1F1;struct _fat_ptr*decl_name=_T1F0;{
struct Cyc_Binding_ResolveNSEnv*(*_T1F2)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T13A=_T1F2;}_T13B=env;_T13C=_T13B->ns;_T13D=decl_ns;{struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_T13A(_T13C,_T13D);_T13F=Cyc_Dict_member;{
# 1089
int(*_T1F2)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T13F;_T13E=_T1F2;}_T140=decl_ns_data;_T141=_T140->datatypedecls;_T142=decl_name;_T143=_T13E(_T141,_T142);if(!_T143)goto _TL1BA;_T144=tud;_T145=_T144->fields;if(_T145!=0)goto _TL1BA;goto _LL35;_TL1BA: _T146=decl_ns_data;_T148=Cyc_Dict_insert;{
# 1092
struct Cyc_Dict_Dict(*_T1F2)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))_T148;_T147=_T1F2;}_T149=decl_ns_data;_T14A=_T149->datatypedecls;_T14B=decl_name;{struct Cyc_List_List*_T1F2=_cycalloc(sizeof(struct Cyc_List_List));
_T1F2->hd=tud;_T1F2->tl=0;_T14C=(struct Cyc_List_List*)_T1F2;}
# 1092
_T146->datatypedecls=_T147(_T14A,_T14B,_T14C);goto _LL35;}}}_TL1B8: _T1EE=_T1ED;{void*exn=_T1EE;_rethrow(exn);}_LL35:;}_TL1B3:;}}_T14D=tud;{
# 1097
struct _tuple0*_T1EB=_T14D->name;struct _fat_ptr*_T1EC;union Cyc_Absyn_Nmspace _T1ED;{struct _tuple0 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{union Cyc_Absyn_Nmspace decl_ns=_T1ED;struct _fat_ptr*decl_name=_T1EC;_T14E=tud;_T14F=_T14E->fields;
if(_T14F==0)goto _TL1BC;_T150=tud;_T151=_T150->fields;_T152=_T151->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T152;_TL1C1: if(fs!=0)goto _TL1BF;else{goto _TL1C0;}
_TL1BF: _T153=fs;_T154=_T153->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T154;_T155=f;{
struct Cyc_List_List*tqts=_T155->typs;_TL1C5: if(tqts!=0)goto _TL1C3;else{goto _TL1C4;}
_TL1C3: _T156=loc;_T157=env;_T158=tqts;_T159=_T158->hd;_T15A=(struct _tuple16*)_T159;_T15B=*_T15A;_T15C=_T15B.f1;Cyc_Binding_resolve_type(_T156,_T157,_T15C);_T15D=tqts;
# 1101
tqts=_T15D->tl;goto _TL1C5;_TL1C4:;}_T15E=f;_T15F=_T15E->name;_T160=*_T15F;{
# 1103
union Cyc_Absyn_Nmspace _T1EE=_T160.f0;_T161=_T1EE.Abs_n;_T162=_T161.tag;switch(_T162){case 2: _T163=_T1EE.Rel_n;_T164=_T163.val;if(_T164!=0)goto _TL1C7;_T165=tud;_T166=_T165->is_extensible;
# 1105
if(!_T166)goto _TL1C9;_T167=f;_T168=_T167->name;_T169=env;_T16A=_T169->ns;_T16B=_T16A->curr_ns;
(*_T168).f0=Cyc_Absyn_Abs_n(_T16B,0);goto _TL1CA;
# 1108
_TL1C9: _T16C=f;_T16D=_T16C->name;_T16E=tud;_T16F=_T16E->name;_T170=*_T16F;(*_T16D).f0=_T170.f0;_TL1CA: goto _LL40;_TL1C7:{struct Cyc_Warn_String_Warn_Warg_struct _T1EF;_T1EF.tag=0;
# 1111
_T1EF.f1=_tag_fat("qualified datatype field declarations not allowed",sizeof(char),50U);_T171=_T1EF;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EF=_T171;void*_T1F0[1];_T1F0[0]=& _T1EF;_T172=f;_T173=_T172->loc;_T174=_tag_fat(_T1F0,sizeof(void*),1);Cyc_Warn_err2(_T173,_T174);}
return;case 1: goto _LL40;default: _T176=Cyc_Warn_impos;{
# 1114
int(*_T1EF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T176;_T175=_T1EF;}_T177=_tag_fat("datatype field Loc_n or C_n",sizeof(char),28U);_T178=_tag_fat(0U,sizeof(void*),0);_T175(_T177,_T178);}_LL40:;}{
# 1116
struct Cyc_Binding_ResolveNSEnv*(*_T1EE)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;_T179=_T1EE;}_T17A=env;_T17B=_T17A->ns;_T17C=f;_T17D=_T17C->name;_T17E=*_T17D;_T17F=_T17E.f0;{struct Cyc_Binding_ResolveNSEnv*ns_data=_T179(_T17B,_T17F);_T180=ns_data;_T182=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T182;_T181=_T1EE;}_T183=ns_data;_T184=_T183->ordinaries;_T185=f;_T186=_T185->name;_T187=*_T186;_T188=_T187.f1;{struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_T1EE=_cycalloc(sizeof(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct));_T1EE->tag=2;
# 1119
_T1EE->f1=tud;_T1EE->f2=f;_T189=(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_T1EE;}_T18A=(void*)_T189;
# 1117
_T180->ordinaries=_T181(_T184,_T188,_T18A);}}_T18B=fs;
# 1099
fs=_T18B->tl;goto _TL1C1;_TL1C0:;}goto _TL1BD;_TL1BC: _TL1BD: goto _LL0;}}}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;_T1E7=_T1EB->f2;}{struct _fat_ptr*v=_T1EA;struct Cyc_List_List*ds2=_T1E7;{
# 1124
void(*_T1EB)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns;_T18C=_T1EB;}_T18D=env;_T18E=_T18D->ns;_T18F=v;_T18C(_T18E,_T18F,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);_T190=env;_T191=_T190->ns;
Cyc_Binding_leave_ns(_T191);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1E4;_T1EA=_T1EB->f1;_T1E7=_T1EB->f2;}{struct _tuple0*qv=_T1EA;struct Cyc_List_List*ds2=_T1E7;_T192=d;_T193=_T192->loc;_T194=env;_T195=_T194->ns;_T196=qv;
# 1129
Cyc_Binding_enter_using(_T193,_T195,_T196);
Cyc_Binding_resolve_decls(env,ds2);_T197=env;_T198=_T197->ns;
Cyc_Binding_leave_using(_T198);goto _LL0;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T1E4;_T1E7=_T1EB->f1;}{struct Cyc_List_List*ds2=_T1E7;_T199=env;{
# 1135
int old=_T199->in_cinclude;_T19A=env;
_T19A->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T19B=env;
_T19B->in_cinclude=old;goto _LL0;}}case 12:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1EB=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1E4;_T1E7=_T1EB->f1;_T1E6=_T1EB->f2;_T19C=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1E4;_T19D=& _T19C->f3;_T1EA=(struct Cyc_List_List**)_T19D;_T1E5=_T1EB->f4;}{struct Cyc_List_List*ds2=_T1E7;struct Cyc_List_List*ovrs=_T1E6;struct Cyc_List_List**exports=(struct Cyc_List_List**)_T1EA;struct _tuple10*hides=_T1E5;_T19E=exports;{
# 1146
struct Cyc_List_List*exs=*_T19E;_TL1CE: if(exs!=0)goto _TL1CC;else{goto _TL1CD;}
_TL1CC: _T19F=exs;_T1A0=_T19F->hd;{struct _tuple15*_T1EB=(struct _tuple15*)_T1A0;struct _tuple0*_T1EC;unsigned _T1ED;{struct _tuple15 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1EC=_T1EE.f1;}{unsigned loc=_T1ED;struct _tuple0*qv=_T1EC;
Cyc_Binding_absolutize_decl(loc,env,qv,4U);}}_T1A1=exs;
# 1146
exs=_T1A1->tl;goto _TL1CE;_TL1CD:;}_T1A2=
# 1154
Cyc_Binding_at_toplevel(env);if(_T1A2)goto _TL1CF;else{goto _TL1D1;}
_TL1D1:{struct Cyc_Warn_String_Warn_Warg_struct _T1EB;_T1EB.tag=0;_T1EB.f1=_tag_fat("extern \"C include\" not at toplevel",sizeof(char),35U);_T1A3=_T1EB;}{struct Cyc_Warn_String_Warn_Warg_struct _T1EB=_T1A3;void*_T1EC[1];_T1EC[0]=& _T1EB;_T1A4=loc;_T1A5=_tag_fat(_T1EC,sizeof(void*),1);Cyc_Warn_err2(_T1A4,_T1A5);}goto _LL0;_TL1CF: _T1A7=Cyc_Dict_lookup;{
# 1158
struct Cyc_Binding_ResolveNSEnv*(*_T1EB)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1A7;_T1A6=_T1EB;}_T1A8=env;_T1A9=_T1A8->ns;_T1AA=_T1A9->ns_data;_T1AB=env;_T1AC=_T1AB->ns;_T1AD=_T1AC->curr_ns;{struct Cyc_Binding_ResolveNSEnv*data=_T1A6(_T1AA,_T1AD);_T1AE=data;{
struct Cyc_Dict_Dict old_dict=_T1AE->ordinaries;_T1AF=env;{
int old=_T1AF->in_cinclude;_T1B0=env;
_T1B0->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);_T1B1=env;
_T1B1->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);_T1B2=env;
_T1B2->in_cyc_override=0;_T1B3=data;{
struct Cyc_Dict_Dict new_dict=_T1B3->ordinaries;
struct Cyc_Dict_Dict out_dict=old_dict;_T1B4=hides;_T1B5=*_T1B4;_T1B6=_T1B5.f0;
if(_T1B6 <= 0U)goto _TL1D2;_T1B7=exports;_T1B8=*_T1B7;_T1B9=(unsigned)_T1B8;
if(!_T1B9)goto _TL1D4;_T1BA=hides;_T1BB=*_T1BA;_T1BC=_T1BB.f0;_T1BD=
_tag_fat("export wildcard expects empty export list",sizeof(char),42U);_T1BE=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1BC,_T1BD,_T1BE);goto _TL1D5;_TL1D4: _TL1D5: _T1BF=env;
_T1BF->in_cinclude=old;_T1C1=Cyc_Dict_iter_c;{
void(*_T1EB)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple14*,struct _fat_ptr*,void*),struct _tuple14*,struct Cyc_Dict_Dict))_T1C1;_T1C0=_T1EB;}{struct _tuple14*_T1EB=_cycalloc(sizeof(struct _tuple14));_T1EB->f0=exports;_T1C3=& out_dict;_T1EB->f1=(struct Cyc_Dict_Dict*)_T1C3;_T1EB->f2=env;_T1EB->f3=hides;_T1C2=(struct _tuple14*)_T1EB;}_T1C4=
Cyc_Dict_difference(new_dict,old_dict);
# 1172
_T1C0(Cyc_Binding_export_all_symbols,_T1C2,_T1C4);goto _TL1D3;
# 1175
_TL1D2: _T1C5=exports;{struct Cyc_List_List*exs=*_T1C5;_TL1D9: if(exs!=0)goto _TL1D7;else{goto _TL1D8;}
_TL1D7: _T1C6=exs;_T1C7=_T1C6->hd;{struct _tuple15*_T1EB=(struct _tuple15*)_T1C7;struct _fat_ptr*_T1EC;unsigned _T1ED;{struct _tuple15 _T1EE=*_T1EB;_T1ED=_T1EE.f0;_T1C8=_T1EE.f1;{struct _tuple0 _T1EF=*_T1C8;_T1EC=_T1EF.f1;}}{unsigned loc=_T1ED;struct _fat_ptr*v=_T1EC;_T1CA=Cyc_Dict_member;{
int(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1CA;_T1C9=_T1EE;}_T1CB=_T1C9(new_dict,v);if(_T1CB)goto _TL1DD;else{goto _TL1DC;}_TL1DD: _T1CD=Cyc_Dict_member;{
int(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1CD;_T1CC=_T1EE;}_T1CE=_T1CC(old_dict,v);
# 1177
if(_T1CE)goto _TL1DE;else{goto _TL1DA;}_TL1DE: _T1D0=Cyc_Dict_lookup;{
# 1179
void*(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D0;_T1CF=_T1EE;}_T1D1=_T1CF(old_dict,v);_T1D3=Cyc_Dict_lookup;{void*(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1D3;_T1D2=_T1EE;}_T1D4=_T1D2(new_dict,v);
# 1177
if(_T1D1==_T1D4)goto _TL1DC;else{goto _TL1DA;}
# 1180
_TL1DC:{struct Cyc_String_pa_PrintArg_struct _T1EE;_T1EE.tag=0;_T1D6=v;_T1EE.f1=*_T1D6;_T1D5=_T1EE;}{struct Cyc_String_pa_PrintArg_struct _T1EE=_T1D5;void*_T1EF[1];_T1EF[0]=& _T1EE;_T1D7=loc;_T1D8=_tag_fat("%s is exported but not defined",sizeof(char),31U);_T1D9=_tag_fat(_T1EF,sizeof(void*),1);Cyc_Warn_err(_T1D7,_T1D8,_T1D9);}goto _TL1DB;_TL1DA: _TL1DB: _T1DB=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_T1DB;_T1DA=_T1EE;}_T1DC=out_dict;_T1DD=v;_T1DF=Cyc_Dict_lookup;{void*(*_T1EE)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T1DF;_T1DE=_T1EE;}_T1E0=_T1DE(new_dict,v);out_dict=_T1DA(_T1DC,_T1DD,_T1E0);}}_T1E1=exs;
# 1175
exs=_T1E1->tl;goto _TL1D9;_TL1D8:;}_TL1D3: _T1E2=data;
# 1184
_T1E2->ordinaries=out_dict;_T1E3=env;
_T1E3->in_cinclude=old;goto _LL0;}}}}}case 13: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: goto _LL0;}_LL0:;}}}
# 1195
static void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Decl*_T3;struct Cyc_List_List*_T4;
_TL1E2: if(tds!=0)goto _TL1E0;else{goto _TL1E1;}
_TL1E0: _T0=env;_T1=tds;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Decl*)_T2;Cyc_Binding_resolve_decl(_T0,_T3);_T4=tds;
# 1196
tds=_T4->tl;goto _TL1E2;_TL1E1:;}
# 1200
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;struct Cyc_Binding_NSCtxt*(*_T3)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T4;struct Cyc_Binding_Env*_T5;struct Cyc_Absyn_Decl*_T6;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T7;
struct Cyc_Binding_Env*env;env=_cycalloc(sizeof(struct Cyc_Binding_Env));_T0=env;_T0->in_cinclude=0;_T1=env;_T1->in_cyc_override=0;_T2=env;{struct Cyc_Binding_NSCtxt*(*_T8)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt;_T3=_T8;}_T2->ns=_T3(1,Cyc_Binding_mt_renv);_T4=env;_T4->local_vars=0;_T5=env;{struct Cyc_Absyn_Decl*_T8=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T9=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_T9->tag=6;
_T9->f1=Cyc_Absyn_exn_tud();_T7=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T9;}_T8->r=(void*)_T7;_T8->loc=0U;_T6=(struct Cyc_Absyn_Decl*)_T8;}Cyc_Binding_resolve_decl(_T5,_T6);
Cyc_Binding_resolve_decls(env,tds);}
# 1206
static struct Cyc_List_List*Cyc_Binding_extern_functions;
static int Cyc_Binding_enable_interproc=0;
# 1209
void Cyc_Binding_interproc_binding_handler(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;struct Cyc_Warn_Qvar_Warn_Warg_struct _T9;struct Cyc_Absyn_Exp*_TA;unsigned _TB;struct _fat_ptr _TC;void*_TD;int _TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;int _T11;struct Cyc_List_List*_T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Exp_Warn_Warg_struct _T15;int(*_T16)(struct _fat_ptr);void*(*_T17)(struct _fat_ptr);struct _fat_ptr _T18;_T0=e;{
void*_T19=_T0->r;void*_T1A;_T1=(int*)_T19;_T2=*_T1;if(_T2!=1)goto _TL1E3;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T19;_T3=_T1B->f1;_T1A=(void*)_T3;}{void*b=_T1A;{struct Cyc_Absyn_Vardecl*_T1B;struct _tuple0*_T1C;_T4=b;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 0: _T7=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T7;_T1C=_T1D->f1;}{struct _tuple0*q=_T1C;{struct Cyc_Warn_String_Warn_Warg_struct _T1D;_T1D.tag=0;
# 1217
_T1D.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_T8=_T1D;}{struct Cyc_Warn_String_Warn_Warg_struct _T1D=_T8;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E;_T1E.tag=1;_T1E.f1=q;_T9=_T1E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E=_T9;void*_T1F[2];_T1F[0]=& _T1D;_T1F[1]=& _T1E;_TA=e;_TB=_TA->loc;_TC=_tag_fat(_T1F,sizeof(void*),2);Cyc_Warn_err2(_TB,_TC);}}
return;}case 1: _TD=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TD;_T1B=_T1D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1B;_TE=Cyc_Binding_enable_interproc;
# 1220
if(!_TE)goto _TL1E6;_TF=vd;_T10=_TF->type;_T11=Cyc_Tcutil_is_function_type(_T10);if(!_T11)goto _TL1E6;{struct Cyc_List_List*_T1D=_cycalloc(sizeof(struct Cyc_List_List));
_T1D->hd=e;_T1D->tl=Cyc_Binding_extern_functions;_T12=(struct Cyc_List_List*)_T1D;}Cyc_Binding_extern_functions=_T12;goto _TL1E7;_TL1E6: _TL1E7: goto _LL5;}case 2: goto _LLD;case 3: _LLD: goto _LLF;case 4: _LLF: goto _LL11;default: _LL11: goto _LL5;}_LL5:;}goto _LL0;}_TL1E3: _T13=Cyc_Binding_enable_interproc;
# 1232
if(!_T13)goto _TL1E8;{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
_T1B.f1=_tag_fat("Interproc binding got unsupported expression: ",sizeof(char),47U);_T14=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T14;{struct Cyc_Warn_Exp_Warn_Warg_struct _T1C;_T1C.tag=4;_T1C.f1=e;_T15=_T1C;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T1C=_T15;void*_T1D[2];_T1D[0]=& _T1B;_T1D[1]=& _T1C;_T17=Cyc_Warn_impos2;{int(*_T1E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T17;_T16=_T1E;}_T18=_tag_fat(_T1D,sizeof(void*),2);_T16(_T18);}}goto _TL1E9;_TL1E8: _TL1E9: goto _LL0;_LL0:;}}
# 1238
struct Cyc_List_List*Cyc_Binding_resolve_all_interproc(struct Cyc_List_List*tds){struct Cyc_Binding_Env*_T0;struct Cyc_Binding_Env*_T1;struct Cyc_Binding_Env*_T2;struct Cyc_Binding_NSCtxt*(*_T3)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int));struct Cyc_Binding_Env*_T4;struct Cyc_Binding_Env*_T5;struct Cyc_Absyn_Decl*_T6;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T7;struct Cyc_List_List*_T8;
Cyc_Binding_enable_interproc=1;
Cyc_Binding_extern_functions=0;{
struct Cyc_Binding_Env*env;env=_cycalloc(sizeof(struct Cyc_Binding_Env));_T0=env;_T0->in_cinclude=0;_T1=env;_T1->in_cyc_override=0;_T2=env;{struct Cyc_Binding_NSCtxt*(*_T9)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int))=(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt;_T3=_T9;}_T2->ns=_T3(1,Cyc_Binding_mt_renv);_T4=env;_T4->local_vars=0;_T5=env;{struct Cyc_Absyn_Decl*_T9=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TA=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TA->tag=6;
_TA->f1=Cyc_Absyn_exn_tud();_T7=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TA;}_T9->r=(void*)_T7;_T9->loc=0U;_T6=(struct Cyc_Absyn_Decl*)_T9;}Cyc_Binding_resolve_decl(_T5,_T6);
Cyc_Binding_resolve_decls(env,tds);{
struct Cyc_List_List*ret=0;{
struct Cyc_List_List*_T9=ret;struct Cyc_List_List*_TA=Cyc_Binding_extern_functions;ret=_TA;Cyc_Binding_extern_functions=_T9;}_T8=ret;
return _T8;}}}
