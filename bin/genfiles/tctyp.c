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

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 114
void*Cyc_Core_identity(void*);
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 134 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 157
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 160
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 165
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 273
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 286
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 293
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 381 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};
# 454
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 479
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 835 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 838
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 855
void*Cyc_Absyn_compress(void*);
# 859
int Cyc_Absyn_type2bool(int,void*);
# 868
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 882
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;
# 884
extern void*Cyc_Absyn_empty_effect;
# 886
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 888
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);
# 907
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar (void);
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar (void);
# 917
void*Cyc_Absyn_bounds_one (void);
# 967
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 32 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 38
int Cyc_Tcutil_is_function_type(void*);
# 41
int Cyc_Tcutil_is_array_type(void*);
# 58
void*Cyc_Tcutil_pointer_elt_type(void*);
# 66
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 78
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 81
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 88
void*Cyc_Tcutil_copy_type(void*);
# 95
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 97
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 123
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 141
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 151
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 153
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 195
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 197
int Cyc_Tcutil_new_tvar_id (void);
# 205
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 209
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 212
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 223
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 230
int Cyc_Tcutil_force_type2bool(int,void*);
# 238
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 241
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 42
extern struct Cyc_Absyn_Kind Cyc_Kinds_urk;
# 44
extern struct Cyc_Absyn_Kind Cyc_Kinds_ubk;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 86
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*);
int Cyc_Kinds_constrain_kinds(void*,void*);
# 68 "attributes.h"
int Cyc_Atts_fntype_att(void*);
# 72
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 73 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*,struct Cyc_List_List*);
# 77
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 80
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};
# 67 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 80 "flags.h"
extern int Cyc_Flags_tovc_r;
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 35
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*);
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int generalize_evars;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;};
# 84 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple13{void*f1;int f2;};
# 90
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
void*_Tmp0=Cyc_Absyn_compress(e);int _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;{int i=_Tmp1;
# 93
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
struct _tuple13*_Tmp2=(struct _tuple13*)es2->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=(int*)& _Tmp2->f2;{void*t=_Tmp4;int*b2=(int*)_Tmp3;
void*_Tmp5=Cyc_Absyn_compress(t);int _Tmp6;if(*((int*)_Tmp5)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f3;{int j=_Tmp6;
# 97
if(i==j){
if(b!=*b2)*b2=1;
return es;}
# 101
goto _LL8;}}else{
goto _LL8;}_LL8:;}}}
# 105
return({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple13*_Tmp3=({struct _tuple13*_Tmp4=_cycalloc(sizeof(struct _tuple13));_Tmp4->f1=e,_Tmp4->f2=b;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=es;_Tmp2;});}}else{
return es;};}
# 111
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bound tvar id for ",sizeof(char),19U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" is NULL",sizeof(char),9U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),3));});
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;_Tmp0;});}
# 116
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){
# 118
struct Cyc_List_List*r=0;
for(1;tvs!=0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(1;b!=0;b=b->tl){
if(({int _Tmp0=get(tvs->hd)->identity;_Tmp0==((struct Cyc_Absyn_Tvar*)b->hd)->identity;}))
break;}
if(b==0)
r=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tvs->hd,_Tmp0->tl=r;_Tmp0;});}
# 127
r=Cyc_List_imp_rev(r);
return r;}
# 135
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(1;free_evars!=0;free_evars=free_evars->tl){
void*e=(*((struct _tuple13*)free_evars->hd)).f1;
{void*_Tmp0=Cyc_Absyn_compress(e);if(*((int*)_Tmp0)==1)
goto _LL0;else{
continue;}_LL0:;}{
# 142
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_Tmp0=Cyc_Tcutil_type_kind(e);switch((int)((struct Cyc_Absyn_Kind*)_Tmp0)->kind){case Cyc_Absyn_RgnKind: switch((int)((struct Cyc_Absyn_Kind*)_Tmp0)->aliasqual){case Cyc_Absyn_Unique:
 t=Cyc_Absyn_unique_rgn_type;s=_tag_fat("unique region",sizeof(char),14U);goto _LL5;case Cyc_Absyn_Aliasable:
 goto _LLB;case Cyc_Absyn_Top: _LLB:
 t=Cyc_Absyn_heap_rgn_type;s=_tag_fat("heap",sizeof(char),5U);goto _LL5;default: goto _LL12;}case Cyc_Absyn_EffKind:
 t=Cyc_Absyn_empty_effect;s=_tag_fat("{}",sizeof(char),3U);goto _LL5;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_false_type;s=_tag_fat("@false",sizeof(char),7U);goto _LL5;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_bounds_one();s=_tag_fat("bounds_one",sizeof(char),11U);goto _LL5;default: _LL12:
# 152
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("hidden type variable ",sizeof(char),22U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)e;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" isn't abstracted in type ",sizeof(char),27U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)in_typ;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});
return;}_LL5:;}
# 155
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't unify evar ",sizeof(char),18U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)e;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" with ",sizeof(char),7U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("!",sizeof(char),2U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),5));});}}}
# 166
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)==0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Kinds_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" used with inconsistent kinds ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp4;_Tmp4.tag=8,_Tmp4.f1=(void*)k1;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" and ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=(void*)k2;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),6));});
if(tv->identity==-1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity!=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("same type variable has different identity!",sizeof(char),43U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}
return tvs;}}}
# 180
({int _Tmp0=Cyc_Tcutil_new_tvar_id();tv->identity=_Tmp0;});
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;_Tmp0;});}
# 186
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar: bad identity in tv",sizeof(char),39U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*tv2=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(tv2->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar: bad identity in tvs2",sizeof(char),41U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
if(tv2->identity==tv->identity)
return tvs;}}
# 196
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;_Tmp0;});}struct _tuple14{struct Cyc_Absyn_Tvar*f1;int f2;};
# 202
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tv",sizeof(char),44U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
struct _tuple14*_Tmp0=(struct _tuple14*)tvs2->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=(int*)& _Tmp0->f2;{struct Cyc_Absyn_Tvar*tv2=_Tmp2;int*b2=(int*)_Tmp1;
if(tv2->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tvs2",sizeof(char),46U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),1));});
if(tv2->identity==tv->identity){
*b2=*b2 || b;
return tvs;}}}}
# 214
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp1=({struct _tuple14*_Tmp2=_cycalloc(sizeof(struct _tuple14));_Tmp2->f1=tv,_Tmp2->f2=b;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tvs;_Tmp0;});}
# 221
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
env.loc=e->loc;
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 32: _LL4:
 goto _LL6;case 33: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 1: _LLA:
 goto _LL0;case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp3;
# 230
for(1;es!=0;es=es->tl){
env=Cyc_Tctyp_i_check_type_level_exp((struct Cyc_Absyn_Exp*)es->hd,env);}
goto _LL0;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 234
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
_Tmp3=e2;_Tmp2=e3;goto _LL10;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL12;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL14;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 239
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type_level_exp(e2,env);
goto _LL0;}case 14: _Tmp3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp3;struct Cyc_Absyn_Exp*e1=_Tmp2;
# 243
env=Cyc_Tctyp_i_check_type_level_exp(e1,env);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t,1,0);
goto _LL0;}case 19: _Tmp3=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp3;
_Tmp3=t;goto _LL1A;}case 17: _Tmp3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {void*t=_Tmp3;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t,1,0);goto _LL0;}case 39: _Tmp3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp3;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ik,t,1,0);goto _LL0;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp3=e;goto _LL20;}case 41: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_Tmp3;
env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL0;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad expression in Tctyp::i_check_type_level_exp",sizeof(char),48U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL0:;}
# 253
return env;}
# 256
static int Cyc_Tctyp_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td==0)
return 1;
if(td->defn==0)
return 0;{
void*_Tmp0=Cyc_Absyn_compress(_check_null(td->defn));void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.rgn;{void*r=_Tmp1;
# 263
void*_Tmp2=Cyc_Absyn_compress(r);void*_Tmp3;if(*((int*)_Tmp2)==2){_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
return Cyc_Absyn_tvar_cmp(tvar,tv)==0;}}else{
return 0;};}}else{
# 267
return 0;};}}
# 271
static struct Cyc_Absyn_Kind*Cyc_Tctyp_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 273
void*_Tmp0=Cyc_Kinds_compress_kb(tvar->kind);switch(*((int*)_Tmp0)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->kind==Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->aliasqual==Cyc_Absyn_Top)
goto _LL4;else{goto _LL5;}}else{goto _LL5;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->aliasqual==Cyc_Absyn_Top){_LL4: {
# 282
enum Cyc_Absyn_KindQual k=expected_kind->kind;
if((((int)k==2 ||(int)k==1)||(int)k==0)&&
 Cyc_Tctyp_typedef_tvar_is_ptr_rgn(tvar,td)){
if((int)expected_kind->aliasqual==0)
return& Cyc_Kinds_rk;
if((int)expected_kind->aliasqual==1)
return& Cyc_Kinds_urk;}
# 290
return& Cyc_Kinds_trk;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 return Cyc_Kinds_tvar_kind(tvar,def_kind);};}
# 294
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){
# 299
struct Cyc_List_List*targs=*targsp;
for(1;targs!=0 && tvs!=0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 305
{struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Kinds_compress_kb(tv->kind);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=t;_Tmp1;});void*_Tmp1;if(*((int*)_Tmp0.f1)==1){if(*((int*)_Tmp0.f2)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp1;
# 307
({struct Cyc_List_List*_Tmp2=Cyc_Tctyp_add_free_tvar(env.loc,env.kind_env,tv2);env.kind_env=_Tmp2;});
({struct Cyc_List_List*_Tmp2=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_Tmp2;});
continue;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 312
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(env.loc,t);}}
# 316
if(targs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("too many type arguments for ",sizeof(char),29U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=typname;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(env.loc,_tag_fat(_Tmp2,sizeof(void*),2));});{
# 319
struct Cyc_List_List*hidden_ts=0;
for(1;tvs!=0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=e,_Tmp0->tl=hidden_ts;_Tmp0;});{
struct Cyc_Absyn_Kind*k=Cyc_Tctyp_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Kinds_bk,expected_kind,td);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}
# 326
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=*targsp;Cyc_List_imp_append(_Tmp1,Cyc_List_imp_rev(hidden_ts));});*targsp=_Tmp0;});
return env;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f1;struct Cyc_List_List*f2;};
# 331
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 334
if(clause==0)
return({struct _tuple15 _Tmp0;_Tmp0.f1=env,_Tmp0.f2=0;_Tmp0;});
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=clause_name;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(" clause has type ",sizeof(char),18U);_Tmp2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3;_Tmp3.tag=3,_Tmp3.f1=(void*)clause->topt;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=
_tag_fat(" instead of integral type",sizeof(char),26U);_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(env.loc,_tag_fat(_Tmp4,sizeof(void*),4));});
env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_List_List*relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
# 344
return({struct _tuple15 _Tmp0;_Tmp0.f1=env,_Tmp0.f2=relns;_Tmp0;});}}
# 348
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 351
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
union Cyc_Absyn_AggrInfo _Tmp2=*info;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_AggrKind _Tmp5;if(_Tmp2.UnknownAggr.tag==1){_Tmp5=_Tmp2.UnknownAggr.val.f1;_Tmp4=_Tmp2.UnknownAggr.val.f2;_Tmp3=_Tmp2.UnknownAggr.val.f3;{enum Cyc_Absyn_AggrKind k=_Tmp5;struct _tuple1*n=_Tmp4;struct Cyc_Core_Opt*tgd=_Tmp3;
# 354
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*ad=*adp;
if((int)ad->kind!=(int)k){
if((int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting struct ",sizeof(char),18U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("instead of union ",sizeof(char),18U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});void*_TmpC[4];_TmpC[0]=& _Tmp8,_TmpC[1]=& _Tmp9,_TmpC[2]=& _TmpA,_TmpC[3]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),4));});else{
# 362
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting union ",sizeof(char),17U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("instead of struct ",sizeof(char),19U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});void*_TmpC[4];_TmpC[0]=& _Tmp8,_TmpC[1]=& _Tmp9,_TmpC[2]=& _TmpA,_TmpC[3]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),4));});}}
# 364
if(((unsigned)tgd &&(int)tgd->v)&&(!((unsigned)ad->impl)|| !_check_null(ad->impl)->tagged))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("@tagged qualfiers don't agree on union ",sizeof(char),40U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),2));});
# 367
({union Cyc_Absyn_AggrInfo _Tmp8=Cyc_Absyn_KnownAggr(adp);*info=_Tmp8;});}
# 356
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp8)->tag==Cyc_Dict_Absent){
# 371
({struct Cyc_Tcenv_Tenv*_TmpA=te;unsigned _TmpB=loc;Cyc_Tc_tcAggrdecl(_TmpA,_TmpB,({struct Cyc_Absyn_Aggrdecl*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TmpC->kind=k,_TmpC->sc=3U,_TmpC->name=n,_TmpC->tvs=0,_TmpC->impl=0,_TmpC->attributes=0,_TmpC->expected_mem_kind=0;_TmpC;}));});
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _TmpA=Cyc_Absyn_KnownAggr(adp);*info=_TmpA;});
# 375
if(*targs!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("declare parameterized type ",sizeof(char),28U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat(" before using",sizeof(char),14U);_TmpD;});void*_TmpD[3];_TmpD[0]=& _TmpA,_TmpD[1]=& _TmpB,_TmpD[2]=& _TmpC;Cyc_Warn_err2(loc,_tag_fat(_TmpD,sizeof(void*),3));});
return env;}
# 379
goto _LL8;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL8:;}}}
# 381
_Tmp4=*adp;goto _LL7;}}else{_Tmp4=*_Tmp2.KnownAggr.val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 383
env=Cyc_Tctyp_check_type_inst(env,targs,ad->tvs,expected_kind,allow_abs_aggr,ad->name,0);
# 385
if((allow_abs_aggr && ad->impl==0)&& !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,expected_kind))
# 390
ad->expected_mem_kind=1;
return env;}};}}
# 396
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 399
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
struct Cyc_List_List*targs=*targsp;
union Cyc_Absyn_DatatypeInfo _Tmp2=*info;int _Tmp3;void*_Tmp4;if(_Tmp2.UnknownDatatype.tag==1){_Tmp4=_Tmp2.UnknownDatatype.val.name;_Tmp3=_Tmp2.UnknownDatatype.val.is_extensible;{struct _tuple1*n=_Tmp4;int is_x=_Tmp3;
# 403
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent){
# 407
({struct Cyc_Tcenv_Tenv*_Tmp9=te;unsigned _TmpA=loc;Cyc_Tc_tcDatatypedecl(_Tmp9,_TmpA,({struct Cyc_Absyn_Datatypedecl*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_TmpB->sc=3U,_TmpB->name=n,_TmpB->tvs=0,_TmpB->fields=0,_TmpB->is_extensible=is_x;_TmpB;}));});
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 410
if(targs!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("declare parameterized datatype ",sizeof(char),32U);_TmpA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=n;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" before using",sizeof(char),14U);_TmpC;});void*_TmpC[3];_TmpC[0]=& _Tmp9,_TmpC[1]=& _TmpA,_TmpC[2]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),3));});
return env;}
# 414
goto _LL8;}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL8:;}}}
# 418
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("datatype ",sizeof(char),10U);_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=n;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" was not declared @extensible",sizeof(char),30U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});
({union Cyc_Absyn_DatatypeInfo _Tmp5=Cyc_Absyn_KnownDatatype(tudp);*info=_Tmp5;});
_Tmp4=*tudp;goto _LL7;}}else{_Tmp4=*_Tmp2.KnownDatatype.val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 423
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tud->name,0);}};}}
# 429
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 432
union Cyc_Absyn_DatatypeFieldInfo _Tmp0=*info;int _Tmp1;void*_Tmp2;void*_Tmp3;if(_Tmp0.UnknownDatatypefield.tag==1){_Tmp3=_Tmp0.UnknownDatatypefield.val.datatype_name;_Tmp2=_Tmp0.UnknownDatatypefield.val.field_name;_Tmp1=_Tmp0.UnknownDatatypefield.val.is_extensible;{struct _tuple1*tname=_Tmp3;struct _tuple1*fname=_Tmp2;int is_x=_Tmp1;
# 435
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(env.te,env.loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 440
{struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(tud->fields)->v;for(0;1;fs=fs->tl){
if(fs==0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Tcutil found a bad datatypefield",sizeof(char),33U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)==0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 447
({union Cyc_Absyn_DatatypeFieldInfo _Tmp4=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_Tmp4;});
_Tmp3=tud;_Tmp2=tuf;goto _LL4;}}else{_Tmp3=_Tmp0.KnownDatatypefield.val.f1;_Tmp2=_Tmp0.KnownDatatypefield.val.f2;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;
# 450
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tuf->name,0);}};}
# 455
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){
# 457
if(Cyc_List_length(ts)!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(" has wrong number of arguments",sizeof(char),31U);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Tctyp_i_check_type(env,k,(void*)_check_null(ts)->hd,put_in_effect,1);}
# 462
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 465
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
struct Cyc_List_List*ts=*targsp;
void*_Tmp2;void*_Tmp3;switch(*((int*)c)){case 1:
# 469
 goto _LL7;case 2: _LL7: goto _LL9;case 0: _LL9: goto _LLB;case 8: _LLB:
 goto _LLD;case 7: _LLD: goto _LLF;case 6: _LLF: goto _LL11;case 13: _LL11:
 goto _LL13;case 12: _LL13: goto _LL15;case 15: _LL15: goto _LL17;case 18: _LL17:
# 473
 if(ts!=0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;})))_Tmp6=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;}));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),2));});
return env;case 3:
# 477
 for(1;ts!=0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_mk,(void*)ts->hd,1,1);}
return env;case 10:
# 481
 for(1;ts!=0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ek,(void*)ts->hd,1,1);}
return env;case 16: _Tmp3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f1;_Tmp2=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f2;{struct _tuple1*n=_Tmp3;struct Cyc_Absyn_Enumdecl**edp=(struct Cyc_Absyn_Enumdecl**)_Tmp2;
# 485
if(ts!=0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(*edp==0 || _check_null(*edp)->fields==0){
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){({struct Cyc_Absyn_Enumdecl*_Tmp6=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_Tmp6;});;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp6)->tag==Cyc_Dict_Absent){
# 489
({struct Cyc_Tcenv_Tenv*_Tmp8=te;unsigned _Tmp9=loc;Cyc_Tc_tcEnumdecl(_Tmp8,_Tmp9,({struct Cyc_Absyn_Enumdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_TmpA->sc=3U,_TmpA->name=n,_TmpA->fields=0;_TmpA;}));});
({struct Cyc_Absyn_Enumdecl*_Tmp8=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_Tmp8;});
goto _LL30;}else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LL30:;}}}
# 494
return env;}case 17: _Tmp3=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)c)->f1;{struct Cyc_List_List*fs=_Tmp3;
# 496
if(ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
# 499
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,prev_fields,(*f->name).f2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("duplicate enum field name ",sizeof(char),27U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=f->name;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 506
prev_fields=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(*f->name).f2,_Tmp4->tl=prev_fields;_Tmp4;});}
if(f->tag==0)
({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_Tmp4;});else{
if(!Cyc_Tcutil_is_const_exp(_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum field ",sizeof(char),12U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=f->name;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(": expression is not constant",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});}
tag_count=1U + Cyc_Evexp_eval_const_uint_exp(_check_null(f->tag)).f1;}
# 513
return env;}}case 11:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_tak,ts,1,_tag_fat("regions",sizeof(char),8U));case 4:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_trk,ts,1,_tag_fat("region_t",sizeof(char),9U));case 9:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_trk,ts,1,_tag_fat("access",sizeof(char),7U));case 5:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ik,ts,0,_tag_fat("tag_t",sizeof(char),6U));case 14:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ik,ts,0,_tag_fat("@thin",sizeof(char),6U));case 21: _Tmp3=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo*info=_Tmp3;
# 520
return Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 19: _Tmp3=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeInfo*info=_Tmp3;
# 522
return Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _Tmp3=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeFieldInfo*info=(union Cyc_Absyn_DatatypeFieldInfo*)_Tmp3;
# 524
return Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);}};}}struct _tuple16{enum Cyc_Absyn_Format_Type f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 529
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 531
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
{void*_Tmp2=Cyc_Absyn_compress(t);enum Cyc_Absyn_AggrKind _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;int _Tmp9;unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;void*_Tmp11;switch(*((int*)_Tmp2)){case 0: _Tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp10=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{void*c=_Tmp11;struct Cyc_List_List**targsp=_Tmp10;
# 534
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr);
# 536
goto _LL3;}case 1: _Tmp11=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f1;_Tmp10=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f2;{struct Cyc_Core_Opt**k=_Tmp11;void**ref=_Tmp10;
# 539
if(*k==0 ||
 Cyc_Kinds_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)_check_null(*k)->v)&& !
Cyc_Kinds_kind_leq((struct Cyc_Absyn_Kind*)_check_null(*k)->v,expected_kind))
({struct Cyc_Core_Opt*_Tmp12=Cyc_Kinds_kind_to_opt(expected_kind);*k=_Tmp12;});
if(((env.fn_result && env.generalize_evars)&&(int)expected_kind->kind==3)&& !te->tempest_generalize){
# 545
if((int)expected_kind->aliasqual==1)
*ref=Cyc_Absyn_unique_rgn_type;else{
# 548
*ref=Cyc_Absyn_heap_rgn_type;}}else{
if((env.generalize_evars &&(int)expected_kind->kind!=6)&&(int)expected_kind->kind!=7){
# 552
struct Cyc_Absyn_Tvar*v=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp12->tag=2,_Tmp12->f1=0,_Tmp12->f2=expected_kind;_Tmp12;}));
({void*_Tmp12=Cyc_Absyn_var_type(v);*ref=_Tmp12;});
_Tmp11=v;goto _LL9;}else{
# 556
({struct Cyc_List_List*_Tmp12=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_Tmp12;});}}
# 558
goto _LL3;}case 2: _Tmp11=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_Tmp11;
# 560
{void*_Tmp12=Cyc_Kinds_compress_kb(v->kind);void*_Tmp13;if(*((int*)_Tmp12)==1){_Tmp13=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp12)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp13;
({struct Cyc_Core_Opt*_Tmp14=({struct Cyc_Core_Opt*_Tmp15=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp16=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp17=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp17->tag=2,_Tmp17->f1=0,_Tmp17->f2=expected_kind;_Tmp17;});_Tmp15->v=_Tmp16;});_Tmp15;});*f=_Tmp14;});goto _LL1C;}}else{
goto _LL1C;}_LL1C:;}
# 566
({struct Cyc_List_List*_Tmp12=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v);env.kind_env=_Tmp12;});
# 569
({struct Cyc_List_List*_Tmp12=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_Tmp12;});
# 571
{void*_Tmp12=Cyc_Kinds_compress_kb(v->kind);void*_Tmp13;void*_Tmp14;if(*((int*)_Tmp12)==2){_Tmp14=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp12)->f1;_Tmp13=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp12)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp14;struct Cyc_Absyn_Kind*k=_Tmp13;
# 573
if(Cyc_Kinds_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_Tmp15=({struct Cyc_Core_Opt*_Tmp16=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp17=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp18=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp18->tag=2,_Tmp18->f1=0,_Tmp18->f2=expected_kind;_Tmp18;});_Tmp16->v=_Tmp17;});_Tmp16;});*f=_Tmp15;});
goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 578
goto _LL3;}case 10: _Tmp11=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1->r;_Tmp10=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f2;{void*td=_Tmp11;void***topt=_Tmp10;
# 583
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Absyn_compress(t));
{void*_Tmp12;switch(*((int*)td)){case 0: _Tmp12=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp12;
# 586
if(env.te->in_extern_c_include)
ad->sc=4U;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1: _Tmp12=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp12;
# 590
if(env.te->in_extern_c_include)
ed->sc=4U;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _Tmp12=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp12;
# 594
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}
# 596
({void**_Tmp12=({void**_Tmp13=_cycalloc(sizeof(void*));*_Tmp13=new_typ;_Tmp13;});*topt=_Tmp12;});
return Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3: _Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;_Tmp10=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.elt_tq;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.rgn;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.nullable;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.bounds;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.zero_term;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.autoreleased;{void*t1=_Tmp11;struct Cyc_Absyn_Tqual*tqp=_Tmp10;void*rgn_type=_TmpF;void*nullable=_TmpE;void*b=_TmpD;void*zt=_TmpC;void*rel=_TmpB;
# 601
struct Cyc_Absyn_Kind*rgnk;
{enum Cyc_Absyn_AliasQual _Tmp12=expected_kind->aliasqual;switch((int)_Tmp12){case Cyc_Absyn_Aliasable:
 rgnk=& Cyc_Kinds_rk;goto _LL2D;case Cyc_Absyn_Unique:
 rgnk=& Cyc_Kinds_urk;goto _LL2D;case Cyc_Absyn_Top:
 goto _LL35;default: _LL35: rgnk=& Cyc_Kinds_trk;goto _LL2D;}_LL2D:;}
# 607
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tak,t1,1,1);
env=Cyc_Tctyp_i_check_type(env,rgnk,rgn_type,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,nullable,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ptrbk,b,0,allow_abs_aggr);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,rel,0,1);
# 614
({int _Tmp12=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_Tmp12;});
# 618
({void*_Tmp12=zt;Cyc_Unify_unify(_Tmp12,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated && !Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("cannot have a pointer to zero-terminated ",sizeof(char),42U);_Tmp13;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14;_Tmp14.tag=2,_Tmp14.f1=(void*)t1;_Tmp14;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15;_Tmp15.tag=0,_Tmp15.f1=_tag_fat(" elements",sizeof(char),10U);_Tmp15;});void*_Tmp15[3];_Tmp15[0]=& _Tmp12,_Tmp15[1]=& _Tmp13,_Tmp15[2]=& _Tmp14;Cyc_Warn_err2(loc,_tag_fat(_Tmp15,sizeof(void*),3));});{
# 624
struct Cyc_Absyn_Exp*e=({void*_Tmp12=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp12,b);});
if(e!=0){
struct _tuple12 _Tmp12=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp13;unsigned _Tmp14;_Tmp14=_Tmp12.f1;_Tmp13=_Tmp12.f2;{unsigned sz=_Tmp14;int known=_Tmp13;
if(is_zero_terminated &&(!known || sz < 1U))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("zero-terminated pointer cannot point to empty sequence",sizeof(char),55U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});}}
# 630
goto _LL3;}}}case 9: _Tmp11=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp11;
# 635
({struct Cyc_Tcenv_Tenv*_Tmp12=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp12,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("valueof_t requires an int expression",sizeof(char),37U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);
goto _LL3;}case 11: _Tmp11=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp11;
# 644
({struct Cyc_Tcenv_Tenv*_Tmp12=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp12,0,e);});
goto _LL3;}case 4: _Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;_Tmp10=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.tq;_TmpF=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.num_elts;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.zero_term;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.zt_loc;{void*t1=_Tmp11;struct Cyc_Absyn_Tqual*tqp=_Tmp10;struct Cyc_Absyn_Exp**eptr=_TmpF;void*zt=_TmpE;unsigned ztl=_TmpA;
# 649
struct Cyc_Absyn_Exp*e=*eptr;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,t1,1,allow_abs_aggr);
({int _Tmp12=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_Tmp12;});{
# 653
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 656
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("cannot have a zero-terminated array of ",sizeof(char),40U);_Tmp13;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14;_Tmp14.tag=2,_Tmp14.f1=(void*)t1;_Tmp14;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15;_Tmp15.tag=0,_Tmp15.f1=_tag_fat(" elements",sizeof(char),10U);_Tmp15;});void*_Tmp15[3];_Tmp15[0]=& _Tmp12,_Tmp15[1]=& _Tmp13,_Tmp15[2]=& _Tmp14;Cyc_Warn_err2(loc,_tag_fat(_Tmp15,sizeof(void*),3));});}
# 661
if(e==0){
# 664
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("array bound defaults to 1 here",sizeof(char),31U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_warn2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp12=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_Tmp12;});}
# 668
({struct Cyc_Tcenv_Tenv*_Tmp12=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp12,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("array bounds expression is not an unsigned int",sizeof(char),47U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple12 _Tmp12=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp13;unsigned _Tmp14;_Tmp14=_Tmp12.f1;_Tmp13=_Tmp12.f2;{unsigned sz=_Tmp14;int known=_Tmp13;
# 674
if((is_zero_terminated && known)&& sz < 1U)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("zero terminated array cannot have zero elements",sizeof(char),48U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_warn2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
# 677
if((known && sz < 1U)&& Cyc_Flags_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("0-element arrays are supported only with gcc, changing to 1",sizeof(char),60U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_warn2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp15=Cyc_Absyn_uint_exp(1U,0U);*eptr=_Tmp15;});}
# 681
goto _LL3;}}}}case 5: _Tmp11=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.tvars;_Tmp10=(void**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.effect;_TmpF=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ret_tqual;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ret_type;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.args;_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.c_varargs;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.cyc_varargs;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.rgn_po;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.attributes;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.requires_clause;_Tmp6=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.requires_relns;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ensures_clause;_Tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ensures_relns;{struct Cyc_List_List**btvs=_Tmp11;void**eff=_Tmp10;struct Cyc_Absyn_Tqual*rtq=_TmpF;void*tr=_TmpE;struct Cyc_List_List*args=_TmpD;int c_vararg=_Tmp9;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpC;struct Cyc_List_List*rgn_po=_TmpB;struct Cyc_List_List*atts=_Tmp8;struct Cyc_Absyn_Exp*req=_Tmp7;struct Cyc_List_List**req_relns=(struct Cyc_List_List**)_Tmp6;struct Cyc_Absyn_Exp*ens=_Tmp5;struct Cyc_List_List**ens_relns=(struct Cyc_List_List**)_Tmp4;
# 689
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(1;atts!=0;atts=atts->tl){
void*_Tmp12=(void*)atts->hd;int _Tmp13;int _Tmp14;enum Cyc_Absyn_Format_Type _Tmp15;switch(*((int*)_Tmp12)){case 1:
 is_stdcall=1;goto _LL3C;case 2:
 is_cdecl=1;goto _LL3C;case 3:
 is_fastcall=1;goto _LL3C;case 19: _Tmp15=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp12)->f1;_Tmp14=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp12)->f2;_Tmp13=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp12)->f3;{enum Cyc_Absyn_Format_Type fmttype=_Tmp15;int i=_Tmp14;int j=_Tmp13;
# 702
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL3C;}default:
# 708
 if(!Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("bad function type attribute ",sizeof(char),29U);_Tmp17;});struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp18;_Tmp18.tag=10,_Tmp18.f1=(void*)atts->hd;_Tmp18;});void*_Tmp18[2];_Tmp18[0]=& _Tmp16,_Tmp18[1]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),2));});}_LL3C:;}
# 711
if((is_stdcall + is_cdecl)+ is_fastcall > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("function can't have multiple calling conventions",sizeof(char),49U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("function can't have multiple format attributes",sizeof(char),47U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});
# 718
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b!=0;b=b->tl){
({int _Tmp12=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_Tmp12;});
({struct Cyc_List_List*_Tmp12=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_Tmp12;});{
void*_Tmp12=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);if(*((int*)_Tmp12)==0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp12)->f1)->kind==Cyc_Absyn_MemKind){
# 724
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=_tag_fat("function abstracts Mem type variable ",sizeof(char),38U);_Tmp14;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp15;_Tmp15.tag=7,_Tmp15.f1=(struct Cyc_Absyn_Tvar*)b->hd;_Tmp15;});void*_Tmp15[2];_Tmp15[0]=& _Tmp13,_Tmp15[1]=& _Tmp14;Cyc_Warn_err2(loc,_tag_fat(_Tmp15,sizeof(void*),2));});goto _LL47;}else{goto _LL4A;}}else{_LL4A:
 goto _LL47;}_LL47:;}}}{
# 730
struct Cyc_Tctyp_CVTEnv new_env=({struct Cyc_Tctyp_CVTEnv _Tmp12;_Tmp12.loc=loc,_Tmp12.te=te,_Tmp12.kind_env=env.kind_env,_Tmp12.fn_result=1,_Tmp12.generalize_evars=env.generalize_evars,_Tmp12.free_vars=0,_Tmp12.free_evars=0;_Tmp12;});
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,tr,1,1);
({int _Tmp12=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_Tmp12;});
new_env.fn_result=0;
# 736
{struct Cyc_List_List*a=args;for(0;a!=0;a=a->tl){
struct _tuple9*trip=(struct _tuple9*)a->hd;
void*t=(*trip).f3;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,t,1,1);{
int ec=Cyc_Tcutil_extract_const_from_typedef(loc,(*trip).f2.print_const,t);
(*trip).f2.real_const=ec;
# 744
if(Cyc_Tcutil_is_array_type(t)){
# 746
void*ptr_rgn=Cyc_Absyn_new_evar(0,0);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_rk,ptr_rgn,1,1);
({void*_Tmp12=Cyc_Tcutil_promote_array(t,ptr_rgn,0);(*trip).f3=_Tmp12;});}}}}
# 752
if(cyc_vararg!=0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("both c_vararg and cyc_vararg",sizeof(char),29U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp13,sizeof(void*),1));});{
struct Cyc_Absyn_VarargInfo _Tmp12=*cyc_vararg;int _Tmp13;void*_Tmp14;_Tmp14=_Tmp12.type;_Tmp13=_Tmp12.inject;{void*vt=_Tmp14;int vi=_Tmp13;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_tmk,vt,1,1);
({int _Tmp15=Cyc_Tcutil_extract_const_from_typedef(loc,cyc_vararg->tq.print_const,vt);cyc_vararg->tq.real_const=_Tmp15;});
# 758
if(vi){
void*_Tmp15=Cyc_Absyn_compress(vt);void*_Tmp16;void*_Tmp17;void*_Tmp18;if(*((int*)_Tmp15)==3){_Tmp18=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp15)->f1.elt_type;_Tmp17=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp15)->f1.ptr_atts.bounds;_Tmp16=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp15)->f1.ptr_atts.zero_term;{void*et=_Tmp18;void*bs=_Tmp17;void*zt=_Tmp16;
# 761
{void*_Tmp19=Cyc_Absyn_compress(et);if(*((int*)_Tmp19)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp19)->f1)==19){
# 763
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("can't inject into a zeroterm pointer",sizeof(char),37U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});
if(!({void*_Tmp1A=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_Tmp1A,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("can't inject into a fat pointer to datatype",sizeof(char),44U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});
goto _LL54;}else{goto _LL57;}}else{_LL57:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("can't inject a non-datatype type",sizeof(char),33U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});goto _LL54;}_LL54:;}
# 770
goto _LL4F;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("expecting a datatype pointer type",sizeof(char),34U);_Tmp1A;});void*_Tmp1A[1];_Tmp1A[0]=& _Tmp19;Cyc_Warn_err2(loc,_tag_fat(_Tmp1A,sizeof(void*),1));});goto _LL4F;}_LL4F:;}}}}
# 775
if(num_formats > 0){
int num_args=Cyc_List_length(args);
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 779
(cyc_vararg==0 && !c_vararg)&& fmt_arg_start!=0)||
(cyc_vararg!=0 || c_vararg)&& fmt_arg_start!=num_args + 1)
# 782
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("bad format descriptor",sizeof(char),22U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(loc,_tag_fat(_Tmp13,sizeof(void*),1));});else{
# 785
struct _tuple9 _Tmp12=*({(struct _tuple9*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(args,fmt_desc_arg - 1);void*_Tmp13;_Tmp13=_Tmp12.f3;{void*t=_Tmp13;
# 787
{void*_Tmp14=Cyc_Absyn_compress(t);void*_Tmp15;void*_Tmp16;if(*((int*)_Tmp14)==3){_Tmp16=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp14)->f1.elt_type;_Tmp15=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp14)->f1.ptr_atts.bounds;{void*et=_Tmp16;void*b=_Tmp15;
# 790
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("format descriptor is not a string",sizeof(char),34U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});else{
# 793
struct Cyc_Absyn_Exp*e=({void*_Tmp17=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp17,b);});
if(e==0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});else{
if(e!=0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});}}
# 799
goto _LL5C;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("format descriptor is not a string type",sizeof(char),39U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});goto _LL5C;}_LL5C:;}
# 802
if(fmt_arg_start!=0 && !c_vararg){
# 806
int problem;
{struct _tuple16 _Tmp14=({struct _tuple16 _Tmp15;_Tmp15.f1=ft,({void*_Tmp16=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(_check_null(cyc_vararg)->type));_Tmp15.f2=_Tmp16;});_Tmp15;});void*_Tmp15;switch((int)_Tmp14.f1){case Cyc_Absyn_Printf_ft: if(*((int*)_Tmp14.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)==19){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)->f1.KnownDatatype.tag==2){_Tmp15=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp15;
# 809
problem=({struct _tuple1*_Tmp16=tud->name;Cyc_Absyn_qvar_cmp(_Tmp16,Cyc_Absyn_datatype_print_arg_qvar());})!=0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}case Cyc_Absyn_Scanf_ft: if(*((int*)_Tmp14.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)==19){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)->f1.KnownDatatype.tag==2){_Tmp15=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp14.f2)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp15;
# 812
problem=({struct _tuple1*_Tmp16=tud->name;Cyc_Absyn_qvar_cmp(_Tmp16,Cyc_Absyn_datatype_scanf_arg_qvar());})!=0;
goto _LL61;}}else{goto _LL66;}}else{goto _LL66;}}else{goto _LL66;}default: _LL66:
# 815
 problem=1;
goto _LL61;}_LL61:;}
# 818
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15;_Tmp15.tag=0,_Tmp15.f1=_tag_fat("format attribute and vararg types don't match",sizeof(char),46U);_Tmp15;});void*_Tmp15[1];_Tmp15[0]=& _Tmp14;Cyc_Warn_err2(loc,_tag_fat(_Tmp15,sizeof(void*),1));});}}}}
# 826
{struct Cyc_List_List*rpo=rgn_po;for(0;rpo!=0;rpo=rpo->tl){
struct _tuple0*_Tmp12=(struct _tuple0*)rpo->hd;void*_Tmp13;void*_Tmp14;_Tmp14=_Tmp12->f1;_Tmp13=_Tmp12->f2;{void*r1=_Tmp14;void*r2=_Tmp13;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,r1,1,1);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_trk,r2,1,1);}}}{
# 834
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_bogus_fenv(tr,args);
struct Cyc_Tcenv_Tenv*old_te=new_env.te;
({struct Cyc_Tcenv_Tenv*_Tmp12=({struct Cyc_Tcenv_Tenv*_Tmp13=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_Tmp13->ns=te->ns,_Tmp13->ae=te->ae,_Tmp13->le=fenv,_Tmp13->allow_valueof=1,_Tmp13->in_extern_c_include=te->in_extern_c_include,_Tmp13->in_tempest=te->in_tempest,_Tmp13->tempest_generalize=te->tempest_generalize,_Tmp13->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_Tmp13;});new_env.te=_Tmp12;});{
# 840
struct _tuple15 _Tmp12=Cyc_Tctyp_check_clause(new_env,_tag_fat("@requires",sizeof(char),10U),req);void*_Tmp13;struct Cyc_Tctyp_CVTEnv _Tmp14;_Tmp14=_Tmp12.f1;_Tmp13=_Tmp12.f2;{struct Cyc_Tctyp_CVTEnv nenv=_Tmp14;struct Cyc_List_List*req_rs=_Tmp13;
new_env=nenv;
*req_relns=req_rs;
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Tcutil_replace_rops,args,req_rs);{
struct _tuple15 _Tmp15=Cyc_Tctyp_check_clause(new_env,_tag_fat("@ensures",sizeof(char),9U),ens);void*_Tmp16;struct Cyc_Tctyp_CVTEnv _Tmp17;_Tmp17=_Tmp15.f1;_Tmp16=_Tmp15.f2;{struct Cyc_Tctyp_CVTEnv nenv=_Tmp17;struct Cyc_List_List*ens_rs=_Tmp16;
new_env=nenv;
*ens_relns=ens_rs;
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Tcutil_replace_rops,args,ens_rs);
new_env.te=old_te;
# 850
if(*eff!=0)
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,_check_null(*eff),1,1);else{
# 853
struct Cyc_List_List*effect=0;
# 858
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs!=0;tvs=tvs->tl){
struct _tuple14 _Tmp18=*((struct _tuple14*)tvs->hd);int _Tmp19;void*_Tmp1A;_Tmp1A=_Tmp18.f1;_Tmp19=_Tmp18.f2;{struct Cyc_Absyn_Tvar*tv=_Tmp1A;int put_in_eff=_Tmp19;
if(!put_in_eff)continue;{
void*_Tmp1B=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1C;void*_Tmp1D;switch(*((int*)_Tmp1B)){case 2: _Tmp1D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp1B)->f1;_Tmp1C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp1B)->f2;if((int)((struct Cyc_Absyn_Kind*)_Tmp1C)->kind==3){struct Cyc_Core_Opt**f=_Tmp1D;struct Cyc_Absyn_Kind*r=_Tmp1C;
# 864
if((int)r->aliasqual==2)
({struct Cyc_Core_Opt*_Tmp1E=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_Tmp1E;});else{
# 867
({struct Cyc_Core_Opt*_Tmp1E=Cyc_Kinds_kind_to_bound_opt(r);*f=_Tmp1E;});}
_Tmp1D=r;goto _LL78;}else{switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp1B)->f2)->kind){case Cyc_Absyn_BoolKind:
# 871
 goto _LL7C;case Cyc_Absyn_PtrBndKind: _LL7C:
 goto _LL7E;case Cyc_Absyn_IntKind: _LL7E:
 goto _LL80;case Cyc_Absyn_EffKind: _Tmp1D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp1B)->f1;{struct Cyc_Core_Opt**f=_Tmp1D;
# 878
({struct Cyc_Core_Opt*_Tmp1E=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ek);*f=_Tmp1E;});goto _LL88;}default: goto _LL8B;}}case 0: _Tmp1D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp1B)->f1;if((int)((struct Cyc_Absyn_Kind*)_Tmp1D)->kind==3){_LL78: {struct Cyc_Absyn_Kind*r=_Tmp1D;
# 870
effect=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1F=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(tv));_Tmp1E->hd=_Tmp1F;}),_Tmp1E->tl=effect;_Tmp1E;});goto _LL74;}}else{switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp1B)->f1)->kind){case Cyc_Absyn_BoolKind: _LL80:
# 874
 goto _LL82;case Cyc_Absyn_PtrBndKind: _LL82:
 goto _LL84;case Cyc_Absyn_IntKind: _LL84:
 goto _LL74;case Cyc_Absyn_EffKind: _LL88:
# 880
 effect=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1F=Cyc_Absyn_var_type(tv);_Tmp1E->hd=_Tmp1F;}),_Tmp1E->tl=effect;_Tmp1E;});goto _LL74;default: _LL8B: _LL8C:
# 885
 effect=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1F=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_Tmp1E->hd=_Tmp1F;}),_Tmp1E->tl=effect;_Tmp1E;});goto _LL74;}}default: _Tmp1D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp1B)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp1D;
# 882
({struct Cyc_Core_Opt*_Tmp1E=({struct Cyc_Core_Opt*_Tmp1F=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp20=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp21=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp21->tag=2,_Tmp21->f1=0,_Tmp21->f2=& Cyc_Kinds_ak;_Tmp21;});_Tmp1F->v=_Tmp20;});_Tmp1F;});*f=_Tmp1E;});goto _LL8C;}}_LL74:;}}}}
# 889
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts!=0;ts=ts->tl){
struct _tuple13 _Tmp18=*((struct _tuple13*)ts->hd);int _Tmp19;void*_Tmp1A;_Tmp1A=_Tmp18.f1;_Tmp19=_Tmp18.f2;{void*tv=_Tmp1A;int put_in_eff=_Tmp19;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_Tmp1B=Cyc_Tcutil_type_kind(tv);switch((int)((struct Cyc_Absyn_Kind*)_Tmp1B)->kind){case Cyc_Absyn_RgnKind:
 effect=({struct Cyc_List_List*_Tmp1C=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1D=Cyc_Absyn_access_eff(tv);_Tmp1C->hd=_Tmp1D;}),_Tmp1C->tl=effect;_Tmp1C;});goto _LL90;case Cyc_Absyn_EffKind:
 effect=({struct Cyc_List_List*_Tmp1C=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1C->hd=tv,_Tmp1C->tl=effect;_Tmp1C;});goto _LL90;case Cyc_Absyn_IntKind:
 goto _LL90;default:
 effect=({struct Cyc_List_List*_Tmp1C=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1D=Cyc_Absyn_regionsof_eff(tv);_Tmp1C->hd=_Tmp1D;}),_Tmp1C->tl=effect;_Tmp1C;});goto _LL90;}_LL90:;}}}}
# 899
({void*_Tmp18=Cyc_Absyn_join_eff(effect);*eff=_Tmp18;});}
# 905
if(*btvs!=0){
struct Cyc_List_List*bs=*btvs;for(0;bs!=0;bs=bs->tl){
void*_Tmp18=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_Tmp19;void*_Tmp1A;switch(*((int*)_Tmp18)){case 1: _Tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp18)->f1;{struct Cyc_Core_Opt**f=_Tmp1A;
# 909
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp1C;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=7,_Tmp1D.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_Tmp1D;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=_tag_fat(" unconstrained, assuming boxed",sizeof(char),31U);_Tmp1E;});void*_Tmp1E[3];_Tmp1E[0]=& _Tmp1B,_Tmp1E[1]=& _Tmp1C,_Tmp1E[2]=& _Tmp1D;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1E,sizeof(void*),3));});
({struct Cyc_Core_Opt*_Tmp1B=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp1B;});
goto _LL99;}case 2: _Tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp18)->f1;_Tmp19=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp18)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp1A;struct Cyc_Absyn_Kind*k=_Tmp19;
# 913
switch((int)((struct Cyc_Absyn_Kind*)k)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)k)->aliasqual==Cyc_Absyn_Top)
goto _LLA6;else{goto _LLB3;}case Cyc_Absyn_MemKind: switch((int)((struct Cyc_Absyn_Kind*)k)->aliasqual){case Cyc_Absyn_Top: _LLA6:
 goto _LLA8;case Cyc_Absyn_Aliasable: _LLA8:
 goto _LLAA;case Cyc_Absyn_Unique:
# 920
 goto _LLB0;default: goto _LLB3;}case Cyc_Absyn_AnyKind: switch((int)((struct Cyc_Absyn_Kind*)k)->aliasqual){case Cyc_Absyn_Top: _LLAA:
# 917
 goto _LLAC;case Cyc_Absyn_Aliasable: _LLAC:
# 919
({struct Cyc_Core_Opt*_Tmp1B=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp1B;});goto _LLA2;case Cyc_Absyn_Unique: _LLB0:
# 922
({struct Cyc_Core_Opt*_Tmp1B=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ubk);*f=_Tmp1B;});goto _LLA2;default: goto _LLB3;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)k)->aliasqual==Cyc_Absyn_Top){
# 924
({struct Cyc_Core_Opt*_Tmp1B=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_Tmp1B;});goto _LLA2;}else{goto _LLB3;}default: _LLB3:
# 926
({struct Cyc_Core_Opt*_Tmp1B=Cyc_Kinds_kind_to_bound_opt(k);*f=_Tmp1B;});goto _LLA2;}_LLA2:;
# 928
goto _LL99;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp18)->f1)->kind==Cyc_Absyn_MemKind){
# 930
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("functions can't abstract M types",sizeof(char),33U);_Tmp1C;});void*_Tmp1C[1];_Tmp1C[0]=& _Tmp1B;Cyc_Warn_err2(loc,_tag_fat(_Tmp1C,sizeof(void*),1));});goto _LL99;}else{
goto _LL99;}}_LL99:;}}
# 934
({struct Cyc_List_List*_Tmp18=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars;})(new_env.kind_env,*btvs,({(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))Cyc_Core_identity;}));env.kind_env=_Tmp18;});
({struct Cyc_List_List*_Tmp18=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars;})(new_env.free_vars,*btvs,({(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst;}));new_env.free_vars=_Tmp18;});
# 937
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs!=0;tvs=tvs->tl){
struct _tuple14 _Tmp18=*((struct _tuple14*)tvs->hd);int _Tmp19;void*_Tmp1A;_Tmp1A=_Tmp18.f1;_Tmp19=_Tmp18.f2;{struct Cyc_Absyn_Tvar*t=_Tmp1A;int b=_Tmp19;
({struct Cyc_List_List*_Tmp1B=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_Tmp1B;});}}}
# 942
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs!=0;evs=evs->tl){
struct _tuple13 _Tmp18=*((struct _tuple13*)evs->hd);int _Tmp19;void*_Tmp1A;_Tmp1A=_Tmp18.f1;_Tmp19=_Tmp18.f2;{void*e=_Tmp1A;int b=_Tmp19;
({struct Cyc_List_List*_Tmp1B=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_Tmp1B;});}}}
# 946
goto _LL3;}}}}}}}case 6: _Tmp11=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_List_List*tq_ts=_Tmp11;
# 949
for(1;tq_ts!=0;tq_ts=tq_ts->tl){
struct _tuple17*p=(struct _tuple17*)tq_ts->hd;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,(*p).f2,1,0);
({int _Tmp12=
Cyc_Tcutil_extract_const_from_typedef(loc,(*p).f1.print_const,(*p).f2);
# 952
(*p).f1.real_const=_Tmp12;});}
# 955
goto _LL3;}case 7: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp11=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp3;struct Cyc_List_List*fs=_Tmp11;
# 959
struct Cyc_List_List*prev_fields=0;
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_Tmp12=(struct Cyc_Absyn_Aggrfield*)fs->hd;void*_Tmp13;void*_Tmp14;void*_Tmp15;void*_Tmp16;void*_Tmp17;void*_Tmp18;_Tmp18=_Tmp12->name;_Tmp17=(struct Cyc_Absyn_Tqual*)& _Tmp12->tq;_Tmp16=_Tmp12->type;_Tmp15=_Tmp12->width;_Tmp14=_Tmp12->attributes;_Tmp13=_Tmp12->requires_clause;{struct _fat_ptr*fn=_Tmp18;struct Cyc_Absyn_Tqual*tqp=(struct Cyc_Absyn_Tqual*)_Tmp17;void*t=_Tmp16;struct Cyc_Absyn_Exp*width=_Tmp15;struct Cyc_List_List*atts=_Tmp14;struct Cyc_Absyn_Exp*requires_clause=_Tmp13;
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("duplicate field ",sizeof(char),17U);_Tmp1A;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=*fn;_Tmp1B;});void*_Tmp1B[2];_Tmp1B[0]=& _Tmp19,_Tmp1B[1]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),2));});
if(Cyc_strcmp(*fn,_tag_fat("",sizeof(char),1U))!=0)
prev_fields=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=fn,_Tmp19->tl=prev_fields;_Tmp19;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_tmk,t,1,0);
({int _Tmp19=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_Tmp19;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause!=0){
# 972
if((int)k!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("@requires clause is only allowed on union members",sizeof(char),50U);_Tmp1A;});void*_Tmp1A[1];_Tmp1A[0]=& _Tmp19;Cyc_Warn_err2(loc,_tag_fat(_Tmp1A,sizeof(void*),1));});
({struct Cyc_Tcenv_Tenv*_Tmp19=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp19,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("@requires clause has type ",sizeof(char),27U);_Tmp1A;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=3,_Tmp1B.f1=(void*)requires_clause->topt;_Tmp1B;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=
_tag_fat(" instead of integral type",sizeof(char),26U);_Tmp1C;});void*_Tmp1C[3];_Tmp1C[0]=& _Tmp19,_Tmp1C[1]=& _Tmp1A,_Tmp1C[2]=& _Tmp1B;Cyc_Warn_err2(loc,_tag_fat(_Tmp1C,sizeof(void*),3));});
env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);}}}
# 981
goto _LL3;}default: _Tmp11=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp10=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f2;_TmpF=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f3;_TmpE=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f4;{struct _tuple1*tdn=_Tmp11;struct Cyc_List_List**targs_ref=(struct Cyc_List_List**)_Tmp10;struct Cyc_Absyn_Typedefdecl**tdopt=(struct Cyc_Absyn_Typedefdecl**)_TmpF;void**toptp=(void**)_TmpE;
# 985
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _Tmp12;_push_handler(& _Tmp12);{int _Tmp13=0;if(setjmp(_Tmp12.handler))_Tmp13=1;if(!_Tmp13){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_Tmp14=(void*)Cyc_Core_get_exn_thrown();void*_Tmp15;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp14)->tag==Cyc_Dict_Absent){
# 988
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("unbound typedef name ",sizeof(char),22U);_Tmp17;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp18;_Tmp18.tag=1,_Tmp18.f1=tdn;_Tmp18;});void*_Tmp18[2];_Tmp18[0]=& _Tmp16,_Tmp18[1]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),2));});
return env;}else{_Tmp15=_Tmp14;{void*exn=_Tmp15;_rethrow(exn);}};}}}
# 991
*tdopt=td;
env=Cyc_Tctyp_check_type_inst(env,targs_ref,td->tvs,expected_kind,allow_abs_aggr,tdn,td);
# 994
if(td->defn!=0){
struct Cyc_List_List*inst=0;
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=*targs_ref;
for(1;tvs!=0;(tvs=tvs->tl,ts=ts->tl)){
inst=({struct Cyc_List_List*_Tmp12=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp13=({struct _tuple18*_Tmp14=_cycalloc(sizeof(struct _tuple18));_Tmp14->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_Tmp14->f2=(void*)_check_null(ts)->hd;_Tmp14;});_Tmp12->hd=_Tmp13;}),_Tmp12->tl=inst;_Tmp12;});}
({void*_Tmp12=Cyc_Tcutil_substitute(inst,_check_null(td->defn));*toptp=_Tmp12;});}
# 1002
goto _LL3;}}_LL3:;}
# 1004
if(!({struct Cyc_Absyn_Kind*_Tmp2=Cyc_Tcutil_type_kind(t);Cyc_Kinds_kind_leq(_Tmp2,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("type ",sizeof(char),6U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" has kind ",sizeof(char),11U);_Tmp5;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp6;_Tmp6.tag=9,({struct Cyc_Absyn_Kind*_Tmp7=Cyc_Tcutil_type_kind(t);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" but as used here needs kind ",sizeof(char),30U);_Tmp7;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp8;_Tmp8.tag=9,_Tmp8.f1=expected_kind;_Tmp8;});void*_Tmp8[6];_Tmp8[0]=& _Tmp2,_Tmp8[1]=& _Tmp3,_Tmp8[2]=& _Tmp4,_Tmp8[3]=& _Tmp5,_Tmp8[4]=& _Tmp6,_Tmp8[5]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),6));});
return env;}}
# 1010
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1012
struct Cyc_List_List*kind_env=cvt.kind_env;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1015
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs!=0;vs=vs->tl){
({struct Cyc_List_List*_Tmp0=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple14*)vs->hd)).f1);cvt.kind_env=_Tmp0;});}}
# 1021
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs!=0;evs=evs->tl){
struct _tuple13 _Tmp0=*((struct _tuple13*)evs->hd);void*_Tmp1;_Tmp1=_Tmp0.f1;{void*e=_Tmp1;
void*_Tmp2=Cyc_Absyn_compress(e);void*_Tmp3;if(*((int*)_Tmp2)==1){_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f4;{struct Cyc_Core_Opt**s=(struct Cyc_Core_Opt**)_Tmp3;
# 1025
if(*s==0)
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=kind_env;_Tmp5;});*s=_Tmp4;});else{
# 1029
struct Cyc_List_List*result=0;
{struct Cyc_List_List*tvs=(struct Cyc_List_List*)_check_null(*s)->v;for(0;tvs!=0;tvs=tvs->tl){
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_Tmp4->tl=result;_Tmp4;});}}
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=result;_Tmp5;});*s=_Tmp4;});}
# 1035
goto _LL3;}}else{
goto _LL3;}_LL3:;}}}
# 1039
return cvt;}
# 1046
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Kinds_tak:& Cyc_Kinds_tmk;
struct Cyc_List_List*te_vars=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Tctyp_CVTEnv cvt=({struct Cyc_Tctyp_CVTEnv _Tmp0=({struct Cyc_Tctyp_CVTEnv _Tmp1;_Tmp1.loc=loc,_Tmp1.te=te,_Tmp1.kind_env=te_vars,_Tmp1.fn_result=0,_Tmp1.generalize_evars=generalize_evars,_Tmp1.free_vars=0,_Tmp1.free_evars=0;_Tmp1;});struct Cyc_Absyn_Kind*_Tmp1=expected_kind;Cyc_Tctyp_check_valid_type(_Tmp0,_Tmp1,1,t);});
# 1054
struct Cyc_List_List*free_tvars=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst;}),cvt.free_vars);
struct Cyc_List_List*free_evars=cvt.free_evars;
# 1057
if(te_vars!=0){
struct Cyc_List_List*res=0;
{struct Cyc_List_List*fs=free_tvars;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*ts=te_vars;
for(1;ts!=0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)==0)
break;}
if(ts==0)
res=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_Tmp0->tl=res;_Tmp0;});}}
# 1067
free_tvars=Cyc_List_imp_rev(res);}
# 1072
{struct Cyc_List_List*x=free_tvars;for(0;x!=0;x=x->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);enum Cyc_Absyn_AliasQual _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp4;});goto _LL0;}case 2: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp3;struct Cyc_Absyn_Kind*k=_Tmp2;
# 1076
switch((int)((struct Cyc_Absyn_Kind*)k)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)k)->aliasqual==Cyc_Absyn_Top)
goto _LLD;else{goto _LL14;}case Cyc_Absyn_MemKind: switch((int)((struct Cyc_Absyn_Kind*)k)->aliasqual){case Cyc_Absyn_Top: _LLD:
 goto _LLF;case Cyc_Absyn_Aliasable: _LLF:
# 1080
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp4;});goto _LL9;case Cyc_Absyn_Unique:
# 1082
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ubk);*f=_Tmp4;});goto _LL9;default: goto _LL14;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)k)->aliasqual==Cyc_Absyn_Top){
# 1084
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_rk);*f=_Tmp4;});goto _LL9;}else{goto _LL14;}default: _LL14:
# 1086
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(k);*f=_Tmp4;});goto _LL9;}_LL9:;
# 1088
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_MemKind){_Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliasqual;{enum Cyc_Absyn_AliasQual a=_Tmp1;
# 1090
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp5;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=7,_Tmp6.f1=(struct Cyc_Absyn_Tvar*)x->hd;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" cannot have kind ",sizeof(char),19U);_Tmp7;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp8;_Tmp8.tag=9,({struct Cyc_Absyn_Kind*_Tmp9=({struct Cyc_Absyn_Kind*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpA->kind=1U,_TmpA->aliasqual=a;_TmpA;});_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),4));});
goto _LL0;}}else{
goto _LL0;}}_LL0:;}}
# 1096
if(free_tvars!=0 || free_evars!=0){
{void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;{struct Cyc_List_List**btvs=(struct Cyc_List_List**)_Tmp1;
# 1099
if(*btvs==0){
# 1101
({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_CurRgn_is_not_curr_rgn,free_tvars);*btvs=_Tmp2;});
free_tvars=0;}
# 1104
goto _LL16;}}else{
goto _LL16;}_LL16:;}
# 1107
if(free_tvars!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" when checking ",sizeof(char),16U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}{
# 1113
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;{struct Cyc_List_List*btvs=_Tmp1;
# 1115
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_exists;})(Cyc_CurRgn_is_curr_rgn,btvs))
# 1117
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("cannot abstract current region ",sizeof(char),32U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_CurRgn_curr_rgn_name;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" in ",sizeof(char),5U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),4));});
goto _LL1B;}}else{
goto _LL1B;}_LL1B:;}}
# 1127
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1130
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_Tmp0=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp1;
# 1133
struct Cyc_List_List*atts=fd->i.attributes;
fd->i=i;
fd->i.attributes=atts;
({int _Tmp2=
Cyc_Tcutil_extract_const_from_typedef(loc,i.ret_tqual.print_const,i.ret_type);
# 1136
fd->i.ret_tqual.real_const=_Tmp2;});
# 1138
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("check_fndecl_valid_type: not a FnType",sizeof(char),38U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}{
# 1141
struct Cyc_List_List*argnames=0;
{struct Cyc_List_List*args=fd->i.args;for(0;args!=0;args=args->tl){
argnames=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=_check_null((*((struct _tuple9*)args->hd)).f1),_Tmp0->tl=argnames;_Tmp0;});}}
Cyc_Tcutil_check_unique_vars(argnames,loc,_tag_fat("function declaration has repeated parameter",sizeof(char),44U));
# 1146
fd->cached_type=t;}}
# 1151
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1154
struct Cyc_Tctyp_CVTEnv cvt=({struct Cyc_Tctyp_CVTEnv _Tmp0=({struct Cyc_Tctyp_CVTEnv _Tmp1;_Tmp1.loc=loc,_Tmp1.te=te,_Tmp1.kind_env=bound_tvars,_Tmp1.fn_result=0,_Tmp1.generalize_evars=0,_Tmp1.free_vars=0,_Tmp1.free_evars=0;_Tmp1;});struct Cyc_Absyn_Kind*_Tmp1=expected_kind;int _Tmp2=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_Tmp0,_Tmp1,_Tmp2,t);});
# 1156
struct Cyc_List_List*free_tvs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple14*)))Cyc_Tctyp_remove_bound_tvars;})(cvt.free_vars,bound_tvars,({(struct Cyc_Absyn_Tvar*(*)(struct _tuple14*))Cyc_Core_fst;}));
for(1;free_tvs!=0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=(*((struct _tuple14*)free_tvs->hd)).f1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" in type ",sizeof(char),10U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}
