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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned char _AliasQualHandle_t;


struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
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
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_region_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_region_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 172 "list.h"
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
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
# 149 "dict.h"
extern void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 257
extern struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 158
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 293 "absyn.h"
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 306
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 313
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 505 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 519
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 681 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 864
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 866
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 871
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 874
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 911
extern void*Cyc_Absyn_heap_rgn_type;
# 913
extern void*Cyc_Absyn_al_qual_type;
# 917
extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;
# 938
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 965
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1087
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1096
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1164
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 37 "flags.h"
extern int Cyc_Flags_warn_override;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 58 "binding.cyc"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*data=mkdata(env);
return({struct Cyc_Binding_NSCtxt*_Tmp0=_cycalloc(sizeof(struct Cyc_Binding_NSCtxt));_Tmp0->curr_ns=0,({
struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Binding_Namespace_Binding_NSDirective_struct));_Tmp4->tag=0,_Tmp4->f1=0;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;});_Tmp0->availables=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*,void*))Cyc_Dict_singleton;})(Cyc_Absyn_varlist_cmp,0,data);_Tmp0->ns_data=_Tmp1;});_Tmp0;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
void*_Tmp0;switch(abs_ns.Abs_n.tag){case 3: _Tmp0=abs_ns.C_n.val;{struct Cyc_List_List*vs=_Tmp0;
_Tmp0=vs;goto _LL4;}case 2: _Tmp0=abs_ns.Abs_n.val;_LL4: {struct Cyc_List_List*vs=_Tmp0;
return({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(ctxt->ns_data,vs);}default:
 Cyc_Warn_impos(_tag_fat("Binding:get_ns_data: relative ns",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));};}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*fullname=Cyc_List_append(ctxt->curr_ns,rel_ns);
if(({(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member;})(ctxt->ns_data,fullname))
return fullname;
{struct Cyc_List_List*as=ctxt->availables;for(0;as!=0;as=as->tl){
void*_Tmp0=(void*)as->hd;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_Tmp0)->f1;{struct Cyc_List_List*ns=_Tmp1;
_Tmp1=ns;goto _LL4;}}else{_Tmp1=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_Tmp0)->f1;_LL4: {struct Cyc_List_List*ns=_Tmp1;
# 92
struct Cyc_List_List*fullname=Cyc_List_append(ns,rel_ns);
if(({(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member;})(ctxt->ns_data,fullname))
return fullname;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("namespace ",sizeof(char),11U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_str_sepstr(rel_ns,_tag_fat("::",sizeof(char),3U));_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" not found",sizeof(char),11U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp0->tag=Cyc_Binding_BindingError;_Tmp0;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=qv->f1;_Tmp0=qv->f2;{union Cyc_Absyn_Nmspace ns=_Tmp1;struct _fat_ptr*v=_Tmp0;
void*_Tmp2;switch(ns.Abs_n.tag){case 1: if(ns.Rel_n.val==0){
# 106
{struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){{void*_Tmp5=({void*(*_Tmp6)(void*,struct _fat_ptr*)=lookup;void*_Tmp7=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(ctxt->ns_data,ctxt->curr_ns);_Tmp6(_Tmp7,v);});_npop_handler(0);return _Tmp5;};_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp5)->tag==Cyc_Dict_Absent)
goto _LLE;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}}_LLE:;}}}
{struct Cyc_List_List*as=ctxt->availables;for(0;as!=0;as=as->tl){
void*_Tmp3=(void*)as->hd;void*_Tmp4;if(*((int*)_Tmp3)==1){_Tmp4=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_Tmp3)->f1;{struct Cyc_List_List*ns=_Tmp4;
_Tmp4=ns;goto _LL17;}}else{_Tmp4=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_Tmp3)->f1;_LL17: {struct Cyc_List_List*ns=_Tmp4;
# 112
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){{void*_Tmp7=({void*(*_Tmp8)(void*,struct _fat_ptr*)=lookup;void*_Tmp9=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(ctxt->ns_data,ns);_Tmp8(_Tmp9,v);});_npop_handler(0);return _Tmp7;};_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent)
goto _LL18;else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL18:;}}}
goto _LL13;}}_LL13:;}}
# 116
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp3->tag=Cyc_Binding_BindingError;_Tmp3;}));}else{_Tmp2=ns.Rel_n.val;{struct Cyc_List_List*ns=_Tmp2;
# 118
struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
{struct Cyc_List_List*abs_ns=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);
void*_Tmp5=({void*(*_Tmp6)(void*,struct _fat_ptr*)=lookup;void*_Tmp7=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(ctxt->ns_data,abs_ns);_Tmp6(_Tmp7,v);});_npop_handler(0);return _Tmp5;}
# 119
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp5)->tag==Cyc_Dict_Absent)
# 121
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp7->tag=Cyc_Binding_BindingError;_Tmp7;}));else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}}}case 3: _Tmp2=ns.C_n.val;{struct Cyc_List_List*ns=_Tmp2;
_Tmp2=ns;goto _LLB;}case 2: _Tmp2=ns.Abs_n.val;_LLB: {struct Cyc_List_List*ns=_Tmp2;
# 124
struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){{void*_Tmp5=({void*(*_Tmp6)(void*,struct _fat_ptr*)=lookup;void*_Tmp7=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(ctxt->ns_data,ns);_Tmp6(_Tmp7,v);});_npop_handler(0);return _Tmp5;};_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp5)->tag==Cyc_Dict_Absent)
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp7->tag=Cyc_Binding_BindingError;_Tmp7;}));else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}}default:
 Cyc_Warn_impos(_tag_fat("lookup local in global",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));};}}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*ns=ctxt->curr_ns;
struct Cyc_List_List*ns2=({struct Cyc_List_List*_Tmp0=ns;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=subname,_Tmp1->tl=0;_Tmp1;}));});
if(!({(int(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_member;})(ctxt->ns_data,ns2))
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict _Tmp1=ctxt->ns_data;struct Cyc_List_List*_Tmp2=ns2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*,void*))Cyc_Dict_insert;})(_Tmp1,_Tmp2,mkdata(env));});ctxt->ns_data=_Tmp0;});
ctxt->curr_ns=ns2;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Binding_Namespace_Binding_NSDirective_struct));_Tmp3->tag=0,_Tmp3->f1=ns2;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ctxt->availables;_Tmp1;});ctxt->availables=_Tmp0;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables==0)
Cyc_Warn_impos(_tag_fat("leaving topmost namespace",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
void*_Tmp0=(void*)_check_null(ctxt->availables)->hd;if(*((int*)_Tmp0)==0){
# 143
ctxt->availables=_check_null(ctxt->availables)->tl;
({struct Cyc_List_List*_Tmp1=Cyc_List_rev(_check_null(Cyc_List_rev(ctxt->curr_ns))->tl);ctxt->curr_ns=_Tmp1;});
goto _LL0;}else{
# 147
Cyc_Warn_impos(_tag_fat("leaving using as namespace",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=usename->f1;_Tmp0=usename->f2;{union Cyc_Absyn_Nmspace nsl=_Tmp1;struct _fat_ptr*v=_Tmp0;
struct Cyc_List_List*ns;
{void*_Tmp2;switch(nsl.Loc_n.tag){case 1: _Tmp2=nsl.Rel_n.val;{struct Cyc_List_List*vs=_Tmp2;
# 155
ns=({unsigned _Tmp3=loc;struct Cyc_Binding_NSCtxt*_Tmp4=ctxt;Cyc_Binding_resolve_rel_ns(_Tmp3,_Tmp4,({struct Cyc_List_List*_Tmp5=vs;Cyc_List_append(_Tmp5,({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=v,_Tmp6->tl=0;_Tmp6;}));}));});{
struct Cyc_List_List*abs_vs=Cyc_List_rev(_check_null(Cyc_List_rev(ns))->tl);
({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Abs_n(abs_vs,0);(*usename).f1=_Tmp3;});
goto _LL3;}}case 2: _Tmp2=nsl.Abs_n.val;{struct Cyc_List_List*vs=_Tmp2;
# 160
ns=({struct Cyc_List_List*_Tmp3=vs;Cyc_List_append(_Tmp3,({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=v,_Tmp4->tl=0;_Tmp4;}));});
goto _LL3;}case 4:
 Cyc_Warn_impos(_tag_fat("enter_using, Loc_n",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));default:
 Cyc_Warn_impos(_tag_fat("enter_using, C_n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));}_LL3:;}
# 165
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Binding_Using_Binding_NSDirective_struct));_Tmp5->tag=1,_Tmp5->f1=ns;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=ctxt->availables;_Tmp3;});ctxt->availables=_Tmp2;});
return ns;}}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables==0)
Cyc_Warn_impos(_tag_fat("leaving topmost namespace as a using",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));{
void*_Tmp0=(void*)_check_null(ctxt->availables)->hd;if(*((int*)_Tmp0)==1){
ctxt->availables=_check_null(ctxt->availables)->tl;goto _LL0;}else{
Cyc_Warn_impos(_tag_fat("leaving namespace as using",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;int in_cyc_override;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 213 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 216
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars==0;}
# 219
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_Tmp0=_cycalloc(sizeof(struct Cyc_Binding_ResolveNSEnv));({struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);_Tmp0->aggrdecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);_Tmp0->datatypedecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);_Tmp0->enumdecls=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);_Tmp0->typedefs=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);_Tmp0->ordinaries=_Tmp1;});_Tmp0;});}
# 227
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return({(struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(renv->aggrdecls,v);}
# 230
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return({(struct Cyc_List_List*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(renv->datatypedecls,v);}
# 233
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return({(struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(renv->enumdecls,v);}
# 236
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return({(struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(renv->typedefs,v);}
# 239
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(renv->ordinaries,v);}
# 242
static void*Cyc_Binding_lookup_ordinary(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=qv->f1;_Tmp0=qv->f2;{union Cyc_Absyn_Nmspace nsl=_Tmp1;struct _fat_ptr*v=_Tmp0;
switch(nsl.Rel_n.tag){case 4:
# 246
 if(Cyc_Binding_at_toplevel(env)|| !({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(*_check_null(env->local_vars),v))
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp2->tag=Cyc_Binding_BindingError;_Tmp2;}));
return({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(*_check_null(env->local_vars),v);case 1: if(nsl.Rel_n.val==0){
# 250
if(!Cyc_Binding_at_toplevel(env)&&({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(*_check_null(env->local_vars),v))
return({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(*_check_null(env->local_vars),v);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 254
 return({(void*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,void*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);};}}
# 258
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_qualbnd(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
# 275
void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _Tmp0=(*qv).f1;switch(_Tmp0.Abs_n.tag){case 1: if(_Tmp0.Rel_n.val==0){
# 278
if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _Tmp1=({struct Cyc_List_List*_Tmp2=env->ns->curr_ns;Cyc_Absyn_Abs_n(_Tmp2,Cyc_Binding_in_cinclude(env)||(int)sc==4);});(*qv).f1=_Tmp1;});else{
# 281
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4:
 goto _LL0;case 2:
# 285
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("qualified names in declarations unimplemented (",sizeof(char),48U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=qv;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(")",sizeof(char),2U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),3));});}_LL0:;}
# 294
void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*old_locals=env->local_vars;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=old_locals;}
# 301
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{void*_Tmp2=Cyc_Binding_lookup_ordinary(loc,env,q);void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_Tmp2)->f1;{void*b=_Tmp3;
# 306
if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0);return;}
switch(*((int*)b)){case 1:
 hides_what=_tag_fat("global variable",sizeof(char),16U);goto _LLB;case 4:
 hides_what=_tag_fat("local variable",sizeof(char),15U);goto _LLB;case 3:
 hides_what=_tag_fat("parameter",sizeof(char),10U);goto _LLB;case 5:
 hides_what=_tag_fat("pattern variable",sizeof(char),17U);goto _LLB;case 2:
 hides_what=_tag_fat("function",sizeof(char),9U);goto _LLB;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("shadowing free variable!",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 316
goto _LL0;}case 1:
 hides_what=_tag_fat("struct constructor",sizeof(char),19U);goto _LL0;case 2:
 hides_what=_tag_fat("datatype constructor",sizeof(char),21U);goto _LL0;case 3:
 goto _LLA;default: _LLA:
 hides_what=_tag_fat("enum tag",sizeof(char),9U);goto _LL0;}_LL0:;}
# 322
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("declaration hides ",sizeof(char),19U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=hides_what;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_warn2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
_npop_handler(0);return;
# 304
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp2)->tag==Cyc_Binding_BindingError)
# 324
return;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 327
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_Tmp0=vd->name;void*_Tmp1;union Cyc_Absyn_Nmspace _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp2;struct _fat_ptr*decl_name=_Tmp1;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict _Tmp4=*_check_null(env->local_vars);struct _fat_ptr*_Tmp5=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp4,_Tmp5,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_Tmp6->tag=0,({void*_Tmp7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp8->tag=4,_Tmp8->f1=vd;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;}));});
# 334
*_check_null(env->local_vars)=_Tmp3;});else{
# 336
if(!env->in_cyc_override){
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict _Tmp4=decl_ns_data->ordinaries;struct _fat_ptr*_Tmp5=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp4,_Tmp5,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
# 340
_Tmp6->tag=0,({void*_Tmp7=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_Tmp8->tag=1,_Tmp8->f1=vd;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;}));});
# 338
decl_ns_data->ordinaries=_Tmp3;});}}}}}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 344
static int Cyc_Binding_resolve_visit_f1(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**b=_Tmp4;
# 347
void*_Tmp5=*b;void*_Tmp6;if(*((int*)_Tmp5)==0){_Tmp6=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_Tmp5)->f1;{struct _tuple0*q=_Tmp6;
# 349
{struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){{void*_Tmp9=Cyc_Binding_lookup_ordinary(e->loc,env,q);void*_TmpA;void*_TmpB;switch(*((int*)_Tmp9)){case 0: _TmpB=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_Tmp9)->f1;{void*bnd=_TmpB;
*b=bnd;goto _LL24;}case 2: _TmpB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_TmpB;struct Cyc_Absyn_Datatypefield*tuf=_TmpA;
({void*_TmpC=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TmpD->tag=31,_TmpD->f1=0,_TmpD->f2=tud,_TmpD->f3=tuf;_TmpD;});e->r=_TmpC;});goto _LL24;}case 3: _TmpB=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Enumdecl*ed=_TmpB;struct Cyc_Absyn_Enumfield*ef=_TmpA;
({void*_TmpC=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_TmpD->tag=32,_TmpD->f1=ed,_TmpD->f2=ef;_TmpD;});e->r=_TmpC;});goto _LL24;}case 4: _TmpB=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_Tmp9)->f2;{void*t=_TmpB;struct Cyc_Absyn_Enumfield*ef=_TmpA;
({void*_TmpC=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_TmpD->tag=33,_TmpD->f1=t,_TmpD->f2=ef;_TmpD;});e->r=_TmpC;});goto _LL24;}default: _TmpB=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_Tmp9)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_TmpB;
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("bad occurrence of type name ",sizeof(char),29U);_TmpD;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpD=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=q;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Warn_err2(e->loc,_tag_fat(_TmpE,sizeof(void*),2));});goto _LL24;}}_LL24:;}
# 349
;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp9)->tag==Cyc_Binding_BindingError){
# 357
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_TmpC;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=q;_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_Warn_err2(e->loc,_tag_fat(_TmpD,sizeof(void*),2));});goto _LL2F;}else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}}_LL2F:;}}}
# 359
return 0;}}else{
return 0;};}case 10: _Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_List_List*es=_Tmp3;int*b=_Tmp2;
# 363
*b=1;
# 365
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es2->hd);}}{
void*_Tmp5=e1->r;void*_Tmp6;if(*((int*)_Tmp5)==1){_Tmp6=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp5)->f1;{void**b=(void**)_Tmp6;
# 369
void*_Tmp7=*b;void*_Tmp8;if(*((int*)_Tmp7)==0){_Tmp8=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_Tmp7)->f1;{struct _tuple0*q=_Tmp8;
# 371
struct _handler_cons _Tmp9;_push_handler(& _Tmp9);{int _TmpA=0;if(setjmp(_Tmp9.handler))_TmpA=1;if(!_TmpA){{void*_TmpB=Cyc_Binding_lookup_ordinary(e1->loc,env,q);void*_TmpC;void*_TmpD;switch(*((int*)_TmpB)){case 0: _TmpD=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_TmpB)->f1;{void*bnd=_TmpD;
*b=bnd;{int _TmpE=0;_npop_handler(0);return _TmpE;}}case 1: _TmpD=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_TmpB)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_TmpD;
# 374
struct Cyc_List_List*dles=0;
for(1;es!=0;es=es->tl){
dles=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple11*_TmpF=({struct _tuple11*_Tmp10=_cycalloc(sizeof(struct _tuple11));_Tmp10->f1=0,_Tmp10->f2=(struct Cyc_Absyn_Exp*)es->hd;_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=dles;_TmpE;});}
({void*_TmpE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_TmpF->tag=29,_TmpF->f1=ad->name,_TmpF->f2=0,({struct Cyc_List_List*_Tmp10=Cyc_List_imp_rev(dles);_TmpF->f3=_Tmp10;}),_TmpF->f4=ad;_TmpF;});e->r=_TmpE;});{
int _TmpE=0;_npop_handler(0);return _TmpE;}}case 2: _TmpD=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TmpB)->f1;_TmpC=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_TmpB)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_TmpD;struct Cyc_Absyn_Datatypefield*tuf=_TmpC;
# 380
if(tuf->typs==0)
({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=tuf->name;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" is a constant, not a function",sizeof(char),31U);_Tmp10;});void*_Tmp10[2];_Tmp10[0]=& _TmpE,_Tmp10[1]=& _TmpF;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp10,sizeof(void*),2));});
({void*_TmpE=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct));_TmpF->tag=31,_TmpF->f1=es,_TmpF->f2=tud,_TmpF->f3=tuf;_TmpF;});e->r=_TmpE;});{
int _TmpE=0;_npop_handler(0);return _TmpE;}}case 4:
 goto _LL48;default: _LL48:
# 386
({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=q;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" is an enum constructor, not a function",sizeof(char),40U);_Tmp10;});void*_Tmp10[2];_Tmp10[0]=& _TmpE,_Tmp10[1]=& _TmpF;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp10,sizeof(void*),2));});{int _TmpE=0;_npop_handler(0);return _TmpE;}};}
# 371
;_pop_handler();}else{void*_TmpB=(void*)Cyc_Core_get_exn_thrown();void*_TmpC;if(((struct Cyc_Binding_BindingError_exn_struct*)_TmpB)->tag==Cyc_Binding_BindingError){
# 388
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("undeclared identifier ",sizeof(char),23U);_TmpE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=q;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_Warn_err2(e->loc,_tag_fat(_TmpF,sizeof(void*),2));});return 0;}else{_TmpC=_TmpB;{void*exn=_TmpC;_rethrow(exn);}};}}}}else{
# 390
return 0;};}}else{
# 392
Cyc_Binding_resolve_exp(env,e1);return 0;};}}case 29: _Tmp4=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple0**tn=_Tmp4;struct Cyc_List_List*ts=_Tmp3;struct Cyc_List_List*dles=_Tmp2;struct Cyc_Absyn_Aggrdecl**adopt=(struct Cyc_Absyn_Aggrdecl**)_Tmp1;
# 395
struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){
({struct Cyc_Absyn_Aggrdecl*_Tmp7=({(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(e->loc,env->ns,*tn,Cyc_Binding_lookup_aggrdecl);*adopt=_Tmp7;});
*tn=_check_null(*adopt)->name;{
int _Tmp7=1;_npop_handler(0);return _Tmp7;}
# 396
;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp7)->tag==Cyc_Binding_BindingError){
# 400
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_TmpA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=*tn;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_err2(e->loc,_tag_fat(_TmpB,sizeof(void*),2));});
return 1;}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}};}}}case 27: _Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 404
Cyc_Binding_resolve_exp(env,e1);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_Tmp5=({struct Cyc_Dict_Dict*_Tmp6=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp7=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);*_Tmp6=_Tmp7;});_Tmp6;});env->local_vars=_Tmp5;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
env->local_vars=0;
return 0;}{
# 412
struct Cyc_Dict_Dict old_locals=*_check_null(env->local_vars);
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
*_check_null(env->local_vars)=old_locals;
return 0;}}case 13: _Tmp4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 419
for(1;ts!=0;ts=ts->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)ts->hd);}
return 1;}case 34: _Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;{void**to=_Tmp4;
# 423
if(to!=0){_Tmp4=*to;goto _LLE;}else{return 1;}}case 30: _Tmp4=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {void*t=_Tmp4;
_Tmp4=t;goto _LL10;}case 33: _Tmp4=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {void*t=_Tmp4;
_Tmp4=t;goto _LL12;}case 25: _Tmp4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1->f3;_LL12: {void*t=_Tmp4;
_Tmp4=t;goto _LL14;}case 39: _Tmp4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {void*t=_Tmp4;
_Tmp4=t;goto _LL16;}case 19: _Tmp4=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {void*t=_Tmp4;
_Tmp4=t;goto _LL18;}case 17: _Tmp4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {void*t=_Tmp4;
_Tmp4=t;goto _LL1A;}case 28: _Tmp4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {void*t=_Tmp4;
_Tmp4=t;goto _LL1C;}case 14: _Tmp4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1C: {void*t=_Tmp4;
# 432
Cyc_Binding_resolve_type(e->loc,env,t);
return 1;}default:
# 435
 return 1;};}
# 438
int Cyc_Binding_resolve_visit_f2(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 12: _Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 441
struct Cyc_Dict_Dict old_locals=*_check_null(env->local_vars);
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);
*_check_null(env->local_vars)=old_locals;
return 0;}case 10: _Tmp2=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp2;struct Cyc_List_List*scs=_Tmp1;
# 447
Cyc_Binding_resolve_exp(env,e);
Cyc_Binding_resolve_scs(env,scs);
return 0;}case 15: _Tmp2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp2;struct Cyc_List_List*scs=_Tmp1;
# 451
Cyc_Binding_resolve_stmt(env,s1);
Cyc_Binding_resolve_scs(env,scs);
return 0;}default:
 return 1;};}
# 457
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
({(void(*)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;})(Cyc_Binding_resolve_visit_f1,Cyc_Binding_resolve_visit_f2,env,s);}
# 460
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
({(void(*)(int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;})(Cyc_Binding_resolve_visit_f1,Cyc_Binding_resolve_visit_f2,env,e);}
# 464
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict old_locals=*_check_null(env->local_vars);
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_Tmp0=(struct Cyc_Absyn_Switch_clause*)scs->hd;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->pattern;_Tmp2=_Tmp0->where_clause;_Tmp1=_Tmp0->body;{struct Cyc_Absyn_Pat*pattern=_Tmp3;struct Cyc_Absyn_Exp*where_clause=_Tmp2;struct Cyc_Absyn_Stmt*body=_Tmp1;
Cyc_Binding_resolve_pat(env,pattern);
if(where_clause!=0)
Cyc_Binding_resolve_exp(env,where_clause);
Cyc_Binding_resolve_stmt(env,body);
*_check_null(env->local_vars)=old_locals;}}
# 474
return;}
# 476
void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_Tmp0=(struct Cyc_Absyn_Aggrfield*)fs->hd;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;_Tmp6=_Tmp0->name;_Tmp5=_Tmp0->tq;_Tmp4=_Tmp0->type;_Tmp3=_Tmp0->width;_Tmp2=_Tmp0->attributes;_Tmp1=_Tmp0->requires_clause;{struct _fat_ptr*fn=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_Absyn_Exp*width=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*requires_clause=_Tmp1;
Cyc_Binding_resolve_type(loc,env,t);
if(width!=0)
Cyc_Binding_resolve_exp(env,width);
if(requires_clause!=0)
Cyc_Binding_resolve_exp(env,requires_clause);}}
# 485
return;}
# 488
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 494
struct Cyc_List_List*vds=0;
for(1;args!=0;args=args->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)args->hd;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{struct _fat_ptr*a=_Tmp3;struct Cyc_Absyn_Tqual tq=_Tmp2;void*t=_Tmp1;
if(a==0)
continue;{
struct Cyc_Absyn_Vardecl*vd=({struct _tuple0*_Tmp4=({struct _tuple0*_Tmp5=_cycalloc(sizeof(struct _tuple0));_Tmp5->f1=Cyc_Absyn_Loc_n,_Tmp5->f2=a;_Tmp5;});Cyc_Absyn_new_vardecl(0U,_Tmp4,t,0);});
vd->tq=tq;
vds=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd,_Tmp4->tl=vds;_Tmp4;});}}}
# 503
if(vai!=0){
struct Cyc_Absyn_VarargInfo _Tmp0=*vai;int _Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp0.name;_Tmp3=_Tmp0.tq;_Tmp2=_Tmp0.type;_Tmp1=_Tmp0.inject;{struct _fat_ptr*v=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;int i=_Tmp1;
if(v==0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("missing name for vararg",sizeof(char),24U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});}else{
# 510
void*typ=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);
struct Cyc_Absyn_Vardecl*vd=({struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,_Tmp6->f2=v;_Tmp6;});Cyc_Absyn_new_vardecl(0U,_Tmp5,typ,0);});
vds=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=vd,_Tmp5->tl=vds;_Tmp5;});}}}
# 515
vds=Cyc_List_imp_rev(vds);
return vds;}
# 519
static void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo*i){
if(i->effect!=0)
Cyc_Binding_resolve_type(loc,env,_check_null(i->effect));
Cyc_Binding_resolve_type(loc,env,i->ret_type);
{struct Cyc_List_List*args=i->args;for(0;args!=0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)args->hd)).f3);}}
if(i->cyc_varargs!=0)
Cyc_Binding_resolve_type(loc,env,_check_null(i->cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,i->rgn_po);
Cyc_Binding_resolve_qualbnd(loc,env,i->qual_bnd);
if(i->requires_clause!=0)
Cyc_Binding_resolve_exp(env,_check_null(i->requires_clause));
if(i->ensures_clause!=0){
# 533
struct Cyc_Dict_Dict locs=*_check_null(env->local_vars);
struct _fat_ptr*v;v=_cycalloc(sizeof(struct _fat_ptr)),*v=_tag_fat("return_value",sizeof(char),13U);{
struct Cyc_Absyn_Vardecl*vd=({struct _tuple0*_Tmp0=({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));_Tmp1->f1=Cyc_Absyn_Loc_n,_Tmp1->f2=v;_Tmp1;});Cyc_Absyn_new_vardecl(0U,_Tmp0,i->ret_type,0);});
i->return_value=vd;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict _Tmp1=*_check_null(env->local_vars);struct _fat_ptr*_Tmp2=v;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp1,_Tmp2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_Tmp3->tag=0,({void*_Tmp4=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp5->tag=4,_Tmp5->f1=vd;_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;}));});
# 537
*_check_null(env->local_vars)=_Tmp0;});
# 539
Cyc_Binding_resolve_exp(env,_check_null(i->ensures_clause));
*_check_null(env->local_vars)=locs;}}}struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 549
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 23: _Tmp2=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeInfo*info=_Tmp2;struct Cyc_List_List*targs=_Tmp1;
# 552
for(1;targs!=0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeInfo _Tmp3=*info;int _Tmp4;void*_Tmp5;if(_Tmp3.UnknownDatatype.tag==1){_Tmp5=_Tmp3.UnknownDatatype.val.name;_Tmp4=_Tmp3.UnknownDatatype.val.is_extensible;{struct _tuple0*qv=_Tmp5;int b=_Tmp4;
# 556
struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{struct Cyc_Absyn_Datatypedecl*tud=(struct Cyc_Absyn_Datatypedecl*)({(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,qv,Cyc_Binding_lookup_datatypedecl)->hd;
({union Cyc_Absyn_DatatypeInfo _Tmp8=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp9;_Tmp9.name=tud->name,_Tmp9.is_extensible=b;_Tmp9;}));*info=_Tmp8;});
_npop_handler(0);return;}
# 557
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp8)->tag==Cyc_Binding_BindingError){
# 561
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}};}}}}else{
return;};}}case 24: _Tmp2=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeFieldInfo*info=_Tmp2;struct Cyc_List_List*targs=_Tmp1;
# 565
for(1;targs!=0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _Tmp3=*info;int _Tmp4;void*_Tmp5;void*_Tmp6;if(_Tmp3.UnknownDatatypefield.tag==1){_Tmp6=_Tmp3.UnknownDatatypefield.val.datatype_name;_Tmp5=_Tmp3.UnknownDatatypefield.val.field_name;_Tmp4=_Tmp3.UnknownDatatypefield.val.is_extensible;{struct _tuple0*qvd=_Tmp6;struct _tuple0*qvf=_Tmp5;int b=_Tmp4;
# 571
{struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){
{void*_Tmp9=Cyc_Binding_lookup_ordinary(loc,env,qvf);void*_TmpA;void*_TmpB;if(*((int*)_Tmp9)==2){_TmpB=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_TmpB;struct Cyc_Absyn_Datatypefield*tuf=_TmpA;
# 574
struct Cyc_Absyn_Datatypedecl*tud2=(struct Cyc_Absyn_Datatypedecl*)({(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,qvd,Cyc_Binding_lookup_datatypedecl)->hd;
if(Cyc_Absyn_qvar_cmp(tud->name,tud2->name)!=0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=tuf->name;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat(" is a variant of ",sizeof(char),18U);_TmpE;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=tud2->name;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" not ",sizeof(char),6U);_Tmp10;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp11;_Tmp11.tag=1,_Tmp11.f1=tud->name;_Tmp11;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=
_tag_fat(" (shadowing not yet implemented properly)",sizeof(char),42U);_Tmp12;});void*_Tmp12[6];_Tmp12[0]=& _TmpC,_Tmp12[1]=& _TmpD,_Tmp12[2]=& _TmpE,_Tmp12[3]=& _TmpF,_Tmp12[4]=& _Tmp10,_Tmp12[5]=& _Tmp11;Cyc_Warn_err2(loc,_tag_fat(_Tmp12,sizeof(void*),6));});
_npop_handler(0);return;}
# 580
({union Cyc_Absyn_DatatypeFieldInfo _TmpC=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _TmpD;_TmpD.datatype_name=tud->name,_TmpD.field_name=tuf->name,_TmpD.is_extensible=b;_TmpD;}));*info=_TmpC;});
# 582
_npop_handler(0);return;}}else{
goto _LL34;}_LL34:;}
# 572
;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp9)->tag==Cyc_Binding_BindingError)
# 585
goto _LL39;else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}}_LL39:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=qvf;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" is not a datatype field",sizeof(char),25U);_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),2));});return;}}else{
return;};}}case 25: _Tmp2=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_AggrInfo*info=_Tmp2;struct Cyc_List_List*targs=_Tmp1;
# 591
for(1;targs!=0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_AggrInfo _Tmp3=*info;void*_Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;if(_Tmp3.UnknownAggr.tag==1){_Tmp6=_Tmp3.UnknownAggr.val.f1;_Tmp5=_Tmp3.UnknownAggr.val.f2;_Tmp4=_Tmp3.UnknownAggr.val.f3;{enum Cyc_Absyn_AggrKind ak=_Tmp6;struct _tuple0*qv=_Tmp5;struct Cyc_Core_Opt*bo=_Tmp4;
# 595
struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){
{struct Cyc_Absyn_Aggrdecl*ad=({(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,qv,Cyc_Binding_lookup_aggrdecl);
if((int)ad->kind!=(int)ak)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("struct vs. union mismatch with earlier declaration",sizeof(char),51U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),1));});
if((((int)ak==1 && bo!=0)&& ad->impl!=0)&&
 _check_null(ad->impl)->tagged!=(int)bo->v)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("@tagged mismatch with earlier declaration",sizeof(char),42U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),1));});
({union Cyc_Absyn_AggrInfo _Tmp9=Cyc_Absyn_UnknownAggr(ak,ad->name,bo);*info=_Tmp9;});
_npop_handler(0);return;}
# 596
;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp9)->tag==Cyc_Binding_BindingError){
# 605
Cyc_Binding_absolutize_topdecl(loc,env,qv,2U);return;}else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}};}}}}else{
return;};}}case 20: _Tmp2=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{struct _tuple0**qv=_Tmp2;struct Cyc_Absyn_Enumdecl*edo=_Tmp1;
# 609
if(edo!=0)
return;{
struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
{struct Cyc_Absyn_Enumdecl*ed=({(struct Cyc_Absyn_Enumdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,*qv,Cyc_Binding_lookup_enumdecl);
*qv=ed->name;
_npop_handler(0);return;}
# 612
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp5)->tag==Cyc_Binding_BindingError){
# 616
Cyc_Binding_absolutize_topdecl(loc,env,*qv,2U);return;}else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}}}case 21: _Tmp2=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct Cyc_List_List*fs=_Tmp2;
# 654
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({struct Cyc_Binding_NSCtxt*_Tmp3=env->ns;({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(_Tmp3,({struct Cyc_List_List*_Tmp4=env->ns->curr_ns;Cyc_Absyn_Abs_n(_Tmp4,
Cyc_Binding_in_cinclude(env));}));});
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
if(f->tag!=0)
Cyc_Binding_resolve_exp(env,_check_null(f->tag));
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict _Tmp4=decl_ns_data->ordinaries;struct _fat_ptr*_Tmp5=(*f->name).f2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp4,_Tmp5,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct));_Tmp6->tag=4,_Tmp6->f1=t,_Tmp6->f2=f;_Tmp6;}));});decl_ns_data->ordinaries=_Tmp3;});}
# 662
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2==0){_LL16:
# 666
 return;}else{_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*c=_Tmp2;struct Cyc_List_List*ts=_Tmp1;
# 668
for(1;ts!=0;ts=ts->tl){
Cyc_Binding_resolve_type(loc,env,(void*)ts->hd);}
return;}}}case 8: _Tmp2=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp0=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;{struct _tuple0**tdn=_Tmp2;struct Cyc_List_List*targs=_Tmp1;struct Cyc_Absyn_Typedefdecl**tdo=_Tmp0;
# 619
for(1;targs!=0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}
{struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
{struct Cyc_Absyn_Typedefdecl*td=({(struct Cyc_Absyn_Typedefdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,*tdn,Cyc_Binding_lookup_typedefdecl);
# 624
*tdn=td->name;
_npop_handler(0);return;}
# 622
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp5)->tag==Cyc_Binding_BindingError)
# 626
goto _LL4D;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}}_LL4D:;}}}
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("unbound typdef name ",sizeof(char),21U);_Tmp4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=*tdn;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),2));});
return;}case 10: _Tmp2=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1;_Tmp1=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp2;void***to=_Tmp1;
# 632
struct Cyc_Dict_Dict*old_locals=env->local_vars;
env->local_vars=0;
{void*_Tmp3=td->r;void*_Tmp4;switch(*((int*)_Tmp3)){case 0: _Tmp4=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 636
({struct Cyc_Binding_Env*_Tmp5=env;Cyc_Binding_resolve_decl(_Tmp5,({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp8->tag=5,_Tmp8->f1=ad;_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=td->loc;_Tmp6;}));});goto _LL52;}case 1: _Tmp4=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
# 638
({struct Cyc_Binding_Env*_Tmp5=env;Cyc_Binding_resolve_decl(_Tmp5,({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp8->tag=7,_Tmp8->f1=ed;_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=td->loc;_Tmp6;}));});goto _LL52;}default: _Tmp4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 640
({struct Cyc_Binding_Env*_Tmp5=env;Cyc_Binding_resolve_decl(_Tmp5,({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp8->tag=6,_Tmp8->f1=tud;_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=td->loc;_Tmp6;}));});goto _LL52;}}_LL52:;}
# 642
env->local_vars=old_locals;
return;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*fs=_Tmp2;
# 649
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 1: _Tmp2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;{void*to=_Tmp2;
# 663
if(to!=0)Cyc_Binding_resolve_type(loc,env,to);return;}case 2:
# 665
 goto _LL16;case 3: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;{void*t1=_Tmp2;void*t2=_Tmp1;void*bds=_Tmp0;
# 673
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 6: _Tmp2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)t)->f1;{struct Cyc_List_List*tqts=_Tmp2;
# 679
for(1;tqts!=0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)tqts->hd)).f2);}
return;}case 4: _Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;{void*t1=_Tmp2;struct Cyc_Absyn_Exp*eo=_Tmp1;
# 683
Cyc_Binding_resolve_type(loc,env,t1);
if(eo!=0)Cyc_Binding_resolve_exp(env,eo);
return;}case 5: _Tmp2=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*i=_Tmp2;
# 690
struct Cyc_List_List*vds=Cyc_Binding_get_fun_vardecls(0,loc,env,i->args,i->cyc_varargs);
i->arg_vardecls=vds;{
struct Cyc_Dict_Dict*old_locals=env->local_vars;
if(old_locals!=0)
({struct Cyc_Dict_Dict*_Tmp3=({struct Cyc_Dict_Dict*_Tmp4=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp4=*_check_null(env->local_vars);_Tmp4;});env->local_vars=_Tmp3;});else{
# 696
({struct Cyc_Dict_Dict*_Tmp3=({struct Cyc_Dict_Dict*_Tmp4=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp5=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);*_Tmp4=_Tmp5;});_Tmp4;});env->local_vars=_Tmp3;});}
{struct Cyc_List_List*vds1=vds;for(0;vds1!=0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict _Tmp4=*_check_null(env->local_vars);struct _fat_ptr*_Tmp5=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp4,_Tmp5,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_Tmp6->tag=0,({void*_Tmp7=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_Tmp8->tag=3,_Tmp8->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;}));});
# 698
*_check_null(env->local_vars)=_Tmp3;});}}
# 701
Cyc_Binding_resolve_function_stuff(loc,env,i);
env->local_vars=old_locals;
return;}}case 9: _Tmp2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 705
_Tmp2=e;goto _LL24;}default: _Tmp2=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_Tmp2;
Cyc_Binding_resolve_exp(env,e);return;}};}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 710
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
unsigned _Tmp0;void*_Tmp1;_Tmp1=p->r;_Tmp0=p->loc;{void*r=_Tmp1;unsigned loc=_Tmp0;
void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)r)){case 15: _Tmp5=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)r)->f1;{struct _tuple0*qv=_Tmp5;
# 714
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){{void*_Tmp8=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp8)){case 0:
 goto _LL2C;case 1:
# 717
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("struct tag used without arguments in pattern",sizeof(char),45U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),1));});_npop_handler(0);return;case 2: _TmpA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp8)->f1;_Tmp9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp8)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_TmpA;struct Cyc_Absyn_Datatypefield*tuf=_Tmp9;
# 719
({void*_TmpB=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_TmpC->tag=8,_TmpC->f1=tud,_TmpC->f2=tuf,_TmpC->f3=0,_TmpC->f4=0;_TmpC;});p->r=_TmpB;});_npop_handler(0);return;}case 3: _TmpA=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_Tmp8)->f1;_Tmp9=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_Tmp8)->f2;{struct Cyc_Absyn_Enumdecl*ed=_TmpA;struct Cyc_Absyn_Enumfield*ef=_Tmp9;
# 721
({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct));_TmpC->tag=13,_TmpC->f1=ed,_TmpC->f2=ef;_TmpC;});p->r=_TmpB;});_npop_handler(0);return;}default: _TmpA=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_Tmp8)->f1;_Tmp9=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_Tmp8)->f2;{void*t=_TmpA;struct Cyc_Absyn_Enumfield*ef=_Tmp9;
# 723
({void*_TmpB=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct));_TmpC->tag=14,_TmpC->f1=t,_TmpC->f2=ef;_TmpC;});p->r=_TmpB;});_npop_handler(0);return;}}_LL2C:;}
# 714
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp8)->tag==Cyc_Binding_BindingError)
# 724
goto _LL37;else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL37:;}}}{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
({void*_Tmp6=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp7->tag=1,_Tmp7->f1=vd,({struct Cyc_Absyn_Pat*_Tmp8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_Tmp7->f2=_Tmp8;});_Tmp7;});p->r=_Tmp6;});
return;}}case 16: _Tmp5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)r)->f1;_Tmp4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)r)->f2;_Tmp3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)r)->f3;{struct _tuple0*qv=_Tmp5;struct Cyc_List_List*ps=_Tmp4;int dots=_Tmp3;
# 731
{struct Cyc_List_List*ps2=ps;for(0;ps2!=0;ps2=ps2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps2->hd);}}
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){{void*_Tmp8=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp8)){case 0:
 goto _LL3C;case 1: _TmpA=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_TmpA;
# 736
struct Cyc_List_List*new_ps=0;
for(1;ps!=0;ps=ps->tl){
new_ps=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple13*_TmpC=({struct _tuple13*_TmpD=_cycalloc(sizeof(struct _tuple13));_TmpD->f1=0,_TmpD->f2=(struct Cyc_Absyn_Pat*)ps->hd;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=new_ps;_TmpB;});}
({void*_TmpB=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_TmpC->tag=7,({union Cyc_Absyn_AggrInfo*_TmpD=({union Cyc_Absyn_AggrInfo*_TmpE=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));({union Cyc_Absyn_AggrInfo _TmpF=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp10=ad;_Tmp10;}));*_TmpE=_TmpF;});_TmpE;});_TmpC->f1=_TmpD;}),_TmpC->f2=0,({struct Cyc_List_List*_TmpD=Cyc_List_imp_rev(new_ps);_TmpC->f3=_TmpD;}),_TmpC->f4=dots;_TmpC;});p->r=_TmpB;});
_npop_handler(0);return;}case 2: _TmpA=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp8)->f1;_Tmp9=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_Tmp8)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_TmpA;struct Cyc_Absyn_Datatypefield*tuf=_Tmp9;
# 742
({void*_TmpB=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct));_TmpC->tag=8,_TmpC->f1=tud,_TmpC->f2=tuf,_TmpC->f3=ps,_TmpC->f4=dots;_TmpC;});p->r=_TmpB;});_npop_handler(0);return;}case 3:
 goto _LL46;default: _LL46:
# 745
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("enum tag used with arguments in pattern",sizeof(char),40U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),1));});_npop_handler(0);return;}_LL3C:;}
# 733
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp8)->tag==Cyc_Binding_BindingError)
# 746
goto _LL47;else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL47:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=qv;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" is not a constructor in pattern",sizeof(char),33U);_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});return;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f1!=0){if(((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f1)->UnknownAggr.tag==1){_Tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f1->UnknownAggr.val.f2;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f3;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f4;{struct _tuple0*n=_Tmp5;struct Cyc_List_List*exist_ts=_Tmp4;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp3;
# 750
{struct Cyc_List_List*dps2=dps;for(0;dps2!=0;dps2=dps2->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)dps2->hd)).f2);}}
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{struct Cyc_Absyn_Aggrdecl*ad=({(struct Cyc_Absyn_Aggrdecl*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,n,Cyc_Binding_lookup_aggrdecl);
({void*_Tmp8=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp9->tag=7,({union Cyc_Absyn_AggrInfo*_TmpA=({union Cyc_Absyn_AggrInfo*_TmpB=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));({union Cyc_Absyn_AggrInfo _TmpC=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_TmpD=ad;_TmpD;}));*_TmpB=_TmpC;});_TmpB;});_Tmp9->f1=_TmpA;}),_Tmp9->f2=exist_ts,_Tmp9->f3=dps,_Tmp9->f4=dots;_Tmp9;});p->r=_Tmp8;});}
# 753
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp8)->tag==Cyc_Binding_BindingError){
# 756
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("non-aggregate name has designator patterns",sizeof(char),43U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(loc,_tag_fat(_TmpB,sizeof(void*),1));});return;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}};}}}
# 758
return;}}else{_Tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f3;_LL25: {struct Cyc_List_List*dps=_Tmp5;
# 787
for(1;dps!=0;dps=dps->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple13*)dps->hd)).f2);}
return;}}}else{_Tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)r)->f3;{struct Cyc_List_List*dps=_Tmp5;
# 785
_Tmp5=dps;goto _LL25;}}case 0:
# 760
 return;case 3: _Tmp5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)r)->f1;_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)r)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp4;
# 762
_Tmp5=vd;_Tmp4=p2;goto _LLF;}case 1: _Tmp5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)r)->f1;_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)r)->f2;_LLF: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp4;
# 764
Cyc_Binding_resolve_pat(env,p2);
_Tmp5=vd;goto _LL11;}case 4: _Tmp5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)r)->f2;_LL11: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;
_Tmp5=vd;goto _LL13;}case 2: _Tmp5=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)r)->f2;_LL13: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 768
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 6: _Tmp5=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)r)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp5;
# 770
Cyc_Binding_resolve_pat(env,p2);return;}case 5: _Tmp5=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)r)->f1;{struct Cyc_List_List*ps=_Tmp5;
# 772
for(1;ps!=0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
goto _LL3;}case 17: _Tmp5=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)r)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
# 776
Cyc_Binding_resolve_exp(env,e);return;}case 9:
# 778
 goto _LL1D;case 10: _LL1D:
 goto _LL1F;case 11: _LL1F:
 goto _LL21;case 12: _LL21:
 return;case 8: _Tmp5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)r)->f3;{struct Cyc_List_List*ps=_Tmp5;
# 791
for(1;ps!=0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 13:
 goto _LL2B;default: _LL2B:
 return;}_LL3:;}}struct _tuple14{void*f1;void*f2;};
# 799
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(1;po!=0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)po->hd)).f2);}}
# 806
void Cyc_Binding_resolve_qualbnd(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*qb){
for(1;qb!=0;qb=qb->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)qb->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple14*)qb->hd)).f2);}}struct _tuple15{struct Cyc_List_List**f1;struct Cyc_Dict_Dict*f2;struct Cyc_Binding_Env*f3;struct _tuple10*f4;};struct _tuple16{unsigned f1;struct _tuple0*f2;int f3;};
# 815
void Cyc_Binding_export_all_symbols(struct _tuple15*cenv,struct _fat_ptr*name,void*res){
# 817
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=cenv->f1;_Tmp3=cenv->f2;_Tmp2=cenv->f3;_Tmp1=cenv->f4->f1;_Tmp0=cenv->f4->f2;{struct Cyc_List_List**exlist_ptr=_Tmp4;struct Cyc_Dict_Dict*out_dict=_Tmp3;struct Cyc_Binding_Env*env=_Tmp2;unsigned wcloc=_Tmp1;struct Cyc_List_List*hidelist=_Tmp0;
void*_Tmp5;void*_Tmp6;switch(*((int*)res)){case 0: _Tmp6=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)res)->f1;{void*bnd=_Tmp6;
# 820
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp7=Cyc_Absyn_Rel_n(0);qv->f1=_Tmp7;}),qv->f2=name;
if(({(int(*)(int(*)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*,struct _tuple0*))Cyc_List_mem;})(Cyc_Absyn_qvar_cmp,hidelist,qv))
# 823
return;
# 825
Cyc_Binding_absolutize_decl(wcloc,env,qv,4U);{
struct _tuple16*ex_sym;ex_sym=_cycalloc(sizeof(struct _tuple16)),ex_sym->f1=wcloc,ex_sym->f2=qv,ex_sym->f3=0;
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=ex_sym,_Tmp8->tl=*exlist_ptr;_Tmp8;});*exlist_ptr=_Tmp7;});
({struct Cyc_Dict_Dict _Tmp7=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(*out_dict,name,res);*out_dict=_Tmp7;});
goto _LL3;}}case 3: _Tmp6=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)res)->f1;_Tmp5=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)res)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp6;struct Cyc_Absyn_Enumfield*ef=_Tmp5;
# 831
({struct Cyc_Dict_Dict _Tmp7=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(*out_dict,name,res);*out_dict=_Tmp7;});
goto _LL3;}case 1: _Tmp6=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)res)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp6;
# 835
({struct Cyc_Dict_Dict _Tmp7=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(*out_dict,name,res);*out_dict=_Tmp7;});
goto _LL3;}case 4: _Tmp6=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)res)->f1;_Tmp5=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)res)->f2;{void*t=_Tmp6;struct Cyc_Absyn_Enumfield*ef=_Tmp5;
# 838
goto _LL3;}default: _Tmp6=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)res)->f1;_Tmp5=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)res)->f2;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp6;struct Cyc_Absyn_Datatypefield*df=_Tmp5;
# 840
Cyc_Warn_err(wcloc,_tag_fat("Unexpected binding from extern C Include",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));
goto _LL3;}}_LL3:;}}
# 853 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 856
unsigned loc=d->loc;
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 860
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
# 865
if(vd->initializer!=0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,_check_null(vd->initializer));
goto _LL0;}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 870
if(env->in_cyc_override)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Function declaration not permitted within cyclone_override block",sizeof(char),65U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
Cyc_Binding_absolutize_decl(loc,env,fd->name,fd->sc);{
struct _tuple0*_Tmp5=fd->name;void*_Tmp6;union Cyc_Absyn_Nmspace _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp7;struct _fat_ptr*decl_name=_Tmp6;
struct Cyc_List_List*argit=fd->i.args;
struct Cyc_List_List*vds=Cyc_Binding_get_fun_vardecls(1,loc,env,fd->i.args,fd->i.cyc_varargs);
({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=vds;_Tmp9;});fd->param_vardecls=_Tmp8;});
fd->i.arg_vardecls=vds;
if(Cyc_Flags_warn_override)
Cyc_Binding_check_warn_override(loc,env,fd->name);{
# 881
struct Cyc_Dict_Dict*old_locals=env->local_vars;
if(old_locals!=0)
({struct Cyc_Dict_Dict*_Tmp8=({struct Cyc_Dict_Dict*_Tmp9=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp9=*_check_null(env->local_vars);_Tmp9;});env->local_vars=_Tmp8;});else{
# 885
({struct Cyc_Dict_Dict*_Tmp8=({struct Cyc_Dict_Dict*_Tmp9=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _TmpA=({(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;})(Cyc_strptrcmp);*_Tmp9=_TmpA;});_Tmp9;});env->local_vars=_Tmp8;});}
{struct Cyc_List_List*vds1=vds;for(0;vds1!=0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=*_check_null(env->local_vars);struct _fat_ptr*_TmpA=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_TmpB->tag=0,({void*_TmpC=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_Absyn_Binding_struct));_TmpD->tag=3,_TmpD->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;}));});
# 887
*_check_null(env->local_vars)=_Tmp8;});}}
# 890
Cyc_Binding_resolve_function_stuff(loc,env,& fd->i);
# 892
if(old_locals!=0){
# 894
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=*old_locals;struct _fat_ptr*_TmpA=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_TmpB->tag=0,({void*_TmpC=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_TmpD->tag=2,_TmpD->f1=fd;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;}));});
# 894
*old_locals=_Tmp8;});
# 896
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=*_check_null(env->local_vars);struct _fat_ptr*_TmpA=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
_TmpB->tag=0,({void*_TmpC=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_TmpD->tag=2,_TmpD->f1=fd;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;}));});
# 896
*_check_null(env->local_vars)=_Tmp8;});}else{
# 899
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=decl_ns_data->ordinaries;struct _fat_ptr*_TmpA=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_VarRes_Binding_Resolved_struct));
# 902
_TmpB->tag=0,({void*_TmpC=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_TmpD->tag=2,_TmpD->f1=fd;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;}));});
# 900
decl_ns_data->ordinaries=_Tmp8;});}
# 905
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,fd->body);
# 908
env->local_vars=old_locals;
goto _LL0;}}}}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*exp=_Tmp3;
# 912
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("let not allowed at top-level",sizeof(char),29U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}
# 916
Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
# 921
for(1;vds!=0;vds=vds->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)vds->hd);}
goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 926
if(open_exp_opt!=0)
Cyc_Binding_resolve_exp(env,open_exp_opt);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 932
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}
# 936
Cyc_Binding_absolutize_decl(loc,env,td->name,td->extern_c?4U: 2U);{
struct _tuple0*_Tmp5=td->name;void*_Tmp6;union Cyc_Absyn_Nmspace _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp7;struct _fat_ptr*decl_name=_Tmp6;
# 939
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
if(td->defn!=0)
Cyc_Binding_resolve_type(loc,env,_check_null(td->defn));
# 943
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _Tmp8=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert;})(decl_ns_data->typedefs,decl_name,td);decl_ns_data->typedefs=_Tmp8;});
# 947
goto _LL0;}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 950
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}
# 954
Cyc_Binding_absolutize_decl(loc,env,ad->name,ad->sc);{
struct _tuple0*_Tmp5=ad->name;void*_Tmp6;union Cyc_Absyn_Nmspace _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp7;struct _fat_ptr*decl_name=_Tmp6;
# 957
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
# 959
if(({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(decl_ns_data->aggrdecls,decl_name)&& ad->impl==0)
goto _LL0;
# 962
if(!env->in_cyc_override){
({struct Cyc_Dict_Dict _Tmp8=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Aggrdecl*))Cyc_Dict_insert;})(decl_ns_data->aggrdecls,decl_name,ad);decl_ns_data->aggrdecls=_Tmp8;});
# 965
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=decl_ns_data->ordinaries;struct _fat_ptr*_TmpA=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_AggrRes_Binding_Resolved_struct));
_TmpB->tag=1,_TmpB->f1=ad;_TmpB;}));});
# 965
decl_ns_data->ordinaries=_Tmp8;});}
# 968
if(ad->impl!=0){
struct Cyc_Absyn_AggrdeclImpl*_Tmp8=_check_null(ad->impl);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8->rgn_po;_TmpA=_Tmp8->qual_bnd;_Tmp9=_Tmp8->fields;{struct Cyc_List_List*rpo=_TmpB;struct Cyc_List_List*qb=_TmpA;struct Cyc_List_List*fs=_Tmp9;
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
Cyc_Binding_resolve_qualbnd(loc,env,qb);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}
# 974
goto _LL0;}}}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
# 977
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("nested type definitions are not yet supported",sizeof(char),46U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}
# 981
Cyc_Binding_absolutize_decl(loc,env,ed->name,ed->sc);{
struct _tuple0*_Tmp5=ed->name;void*_Tmp6;union Cyc_Absyn_Nmspace _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp7;struct _fat_ptr*decl_name=_Tmp6;
# 984
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
# 986
if(({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(decl_ns_data->enumdecls,decl_name)&& ed->fields==0)
goto _LL0;
# 990
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _Tmp8=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Enumdecl*))Cyc_Dict_insert;})(decl_ns_data->enumdecls,decl_name,ed);decl_ns_data->enumdecls=_Tmp8;});
# 993
if(ed->fields!=0){
# 995
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(ed->fields)->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Binding_absolutize_decl(f->loc,env,f->name,ed->sc);
if(f->tag!=0)
Cyc_Binding_resolve_exp(env,_check_null(f->tag));
if(!env->in_cyc_override)
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=decl_ns_data->ordinaries;struct _fat_ptr*_TmpA=(*f->name).f2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_EnumRes_Binding_Resolved_struct));
# 1003
_TmpB->tag=3,_TmpB->f1=ed,_TmpB->f2=f;_TmpB;}));});
# 1001
decl_ns_data->ordinaries=_Tmp8;});}}
# 1005
goto _LL0;}}}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 1009
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){
{struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*,struct Cyc_List_List*(*)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup;})(loc,env->ns,tud->name,Cyc_Binding_lookup_datatypedecl);
struct Cyc_Absyn_Datatypedecl*last_decl=(struct Cyc_Absyn_Datatypedecl*)decls->hd;
if(!last_decl->is_extensible)
_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Binding_BindingError_exn_struct));_Tmp7->tag=Cyc_Binding_BindingError;_Tmp7;}));
tud->name=last_decl->name;
tud->is_extensible=1;{
struct _tuple0*_Tmp7=tud->name;void*_Tmp8;union Cyc_Absyn_Nmspace _Tmp9;_Tmp9=_Tmp7->f1;_Tmp8=_Tmp7->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp9;struct _fat_ptr*decl_name=_Tmp8;
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
({struct Cyc_Dict_Dict _TmpA=({struct Cyc_Dict_Dict _TmpB=decl_ns_data->datatypedecls;struct _fat_ptr*_TmpC=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert;})(_TmpB,_TmpC,({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));
_TmpD->hd=tud,_TmpD->tl=decls;_TmpD;}));});
# 1018
decl_ns_data->datatypedecls=_TmpA;});}}}
# 1010
;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Binding_BindingError_exn_struct*)_Tmp7)->tag==Cyc_Binding_BindingError){
# 1022
Cyc_Binding_absolutize_topdecl(loc,env,tud->name,tud->sc);{
struct _tuple0*_Tmp9=tud->name;void*_TmpA;union Cyc_Absyn_Nmspace _TmpB;_TmpB=_Tmp9->f1;_TmpA=_Tmp9->f2;{union Cyc_Absyn_Nmspace decl_ns=_TmpB;struct _fat_ptr*decl_name=_TmpA;
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,decl_ns);
# 1026
if(({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(decl_ns_data->datatypedecls,decl_name)&& tud->fields==0)
# 1028
goto _LL35;
({struct Cyc_Dict_Dict _TmpC=({struct Cyc_Dict_Dict _TmpD=decl_ns_data->datatypedecls;struct _fat_ptr*_TmpE=decl_name;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_List_List*))Cyc_Dict_insert;})(_TmpD,_TmpE,({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));
_TmpF->hd=tud,_TmpF->tl=0;_TmpF;}));});
# 1029
decl_ns_data->datatypedecls=_TmpC;});
# 1031
goto _LL35;}}}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL35:;}}}{
# 1034
struct _tuple0*_Tmp5=tud->name;void*_Tmp6;union Cyc_Absyn_Nmspace _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{union Cyc_Absyn_Nmspace decl_ns=_Tmp7;struct _fat_ptr*decl_name=_Tmp6;
if(tud->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(tud->fields)->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
{struct Cyc_List_List*tqts=f->typs;for(0;tqts!=0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple12*)tqts->hd)).f2);}}
{union Cyc_Absyn_Nmspace _Tmp8=(*f->name).f1;switch(_Tmp8.Abs_n.tag){case 1: if(_Tmp8.Rel_n.val==0){
# 1042
if(tud->is_extensible)
({union Cyc_Absyn_Nmspace _Tmp9=Cyc_Absyn_Abs_n(env->ns->curr_ns,0);(*f->name).f1=_Tmp9;});else{
# 1045
(*f->name).f1=(*tud->name).f1;}
goto _LL40;}else{
# 1048
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("qualified datatype field declarations not allowed",sizeof(char),50U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(f->loc,_tag_fat(_TmpA,sizeof(void*),1));});
return;}case 2:
 goto _LL40;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("datatype field Loc_n or C_n",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));}_LL40:;}{
# 1053
struct Cyc_Binding_ResolveNSEnv*ns_data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*,union Cyc_Absyn_Nmspace))Cyc_Binding_get_ns_data;})(env->ns,(*f->name).f1);
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict _Tmp9=ns_data->ordinaries;struct _fat_ptr*_TmpA=(*f->name).f2;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp9,_TmpA,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct));
# 1056
_TmpB->tag=2,_TmpB->f1=tud,_TmpB->f2=f;_TmpB;}));});
# 1054
ns_data->ordinaries=_Tmp8;});}}}
# 1058
goto _LL0;}}}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*ds2=_Tmp3;
# 1061
({(void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_enter_ns;})(env->ns,v,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);
Cyc_Binding_leave_ns(env->ns);
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple0*qv=_Tmp4;struct Cyc_List_List*ds2=_Tmp3;
# 1066
Cyc_Binding_enter_using(d->loc,env->ns,qv);
Cyc_Binding_resolve_decls(env,ds2);
Cyc_Binding_leave_using(env->ns);
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp4;
# 1072
int old=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cinclude=old;
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*ds2=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List**exports=(struct Cyc_List_List**)_Tmp2;struct _tuple10*hides=_Tmp1;
# 1083
{struct Cyc_List_List*exs=*exports;for(0;exs!=0;exs=exs->tl){
struct _tuple16*_Tmp5=(struct _tuple16*)exs->hd;void*_Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2;{unsigned loc=_Tmp7;struct _tuple0*qv=_Tmp6;
Cyc_Binding_absolutize_decl(loc,env,qv,4U);}}}
# 1091
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("extern \"C include\" not at toplevel",sizeof(char),35U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}{
# 1095
struct Cyc_Binding_ResolveNSEnv*data=({(struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;})(env->ns->ns_data,env->ns->curr_ns);
struct Cyc_Dict_Dict old_dict=data->ordinaries;
int old=env->in_cinclude;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cyc_override=1;
Cyc_Binding_resolve_decls(env,ovrs);
env->in_cyc_override=0;{
struct Cyc_Dict_Dict new_dict=data->ordinaries;
struct Cyc_Dict_Dict out_dict=old_dict;
if((*hides).f1 > 0U){
if((unsigned)*exports)
Cyc_Warn_err((*hides).f1,_tag_fat("export wildcard expects empty export list",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));
env->in_cinclude=old;
({struct _tuple15*_Tmp5=({struct _tuple15*_Tmp6=_cycalloc(sizeof(struct _tuple15));_Tmp6->f1=exports,_Tmp6->f2=& out_dict,_Tmp6->f3=env,_Tmp6->f4=hides;_Tmp6;});({(void(*)(void(*)(struct _tuple15*,struct _fat_ptr*,void*),struct _tuple15*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Binding_export_all_symbols,_Tmp5,
Cyc_Dict_difference(new_dict,old_dict));});}else{
# 1112
struct Cyc_List_List*exs=*exports;for(0;exs!=0;exs=exs->tl){
struct _tuple16*_Tmp5=(struct _tuple16*)exs->hd;void*_Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5->f1;_Tmp6=_Tmp5->f2->f2;{unsigned loc=_Tmp7;struct _fat_ptr*v=_Tmp6;
if(!({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(new_dict,v)||
({(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;})(old_dict,v)&&({
void*_Tmp8=({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(old_dict,v);_Tmp8==({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(new_dict,v);}))
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err(loc,_tag_fat("%s is exported but not defined",sizeof(char),31U),_tag_fat(_Tmp9,sizeof(void*),1));});
out_dict=({struct Cyc_Dict_Dict _Tmp8=out_dict;struct _fat_ptr*_Tmp9=v;({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;})(_Tmp8,_Tmp9,({(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;})(new_dict,v));});}}}
# 1121
data->ordinaries=out_dict;
env->in_cinclude=old;
goto _LL0;}}}case 13:
# 1125
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 goto _LL0;}_LL0:;}
# 1132
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(1;tds!=0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1137
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*env;env=_cycalloc(sizeof(struct Cyc_Binding_Env)),env->in_cinclude=0,env->in_cyc_override=0,({struct Cyc_Binding_NSCtxt*_Tmp0=({(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Binding_ResolveNSEnv*(*)(int)))Cyc_Binding_mt_nsctxt;})(1,Cyc_Binding_mt_renv);env->ns=_Tmp0;}),env->local_vars=0;
({struct Cyc_Binding_Env*_Tmp0=env;Cyc_Binding_resolve_decl(_Tmp0,({struct Cyc_Absyn_Decl*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp2=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_Tmp3->tag=6,({struct Cyc_Absyn_Datatypedecl*_Tmp4=Cyc_Absyn_exn_tud();_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->r=_Tmp2;}),_Tmp1->loc=0U;_Tmp1;}));});
Cyc_Binding_resolve_decls(env,tds);}
