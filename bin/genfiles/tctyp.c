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
# 113 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 119
void*Cyc_Core_identity(void*);extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 309
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 322
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 329
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 893
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 911
void*Cyc_Absyn_compress(void*);
# 915
int Cyc_Absyn_type2bool(int,void*);
# 924
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 938
extern void*Cyc_Absyn_heap_rgn_type;
# 940
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 944
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 946
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);
# 971
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar (void);
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar (void);
# 981
void*Cyc_Absyn_bounds_one (void);
# 1010
void*Cyc_Absyn_aqualsof_type(void*);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1200
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 32 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 39
int Cyc_Tcutil_is_function_type(void*);
# 42
int Cyc_Tcutil_is_array_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 91
void*Cyc_Tcutil_copy_type(void*);
# 98
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 100
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 156
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 166
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 168
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 209
void*Cyc_Tcutil_normalize_effect(void*);
# 212
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 214
int Cyc_Tcutil_new_tvar_id (void);
# 222
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 226
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 229
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 240
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 247
int Cyc_Tcutil_force_type2bool(int,void*);
# 255
int Cyc_Tcutil_admits_zero(void*);
# 258
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 265
void*Cyc_Tcutil_lookup_aquals(struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_Absyn_Tvar*f0;int f1;};
# 268
struct _tuple12*Cyc_Tcutil_tvar_bool_pair(int,struct Cyc_Absyn_Tvar*);
# 270
int Cyc_Tcutil_type_in_effect(int,void*,void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 60 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 76 "kinds.h"
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 83
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
struct Cyc_Core_Opt Cyc_Kinds_tvar_kind_opt(struct Cyc_Absyn_Tvar*);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 97
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_bound_opt(struct Cyc_Absyn_Kind*);
int Cyc_Kinds_constrain_kinds(void*,void*);
# 68 "attributes.h"
int Cyc_Atts_fntype_att(void*);
# 72
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
# 144
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 46 "warn.h"
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 89 "flags.h"
extern int Cyc_Flags_tovc_r;
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 35
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*);
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*);struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 256 "assndef.h"
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void);
# 43 "vcgen.h"
extern struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int def_aliasable;int inst_type_evar;int generalize_evars;int allow_aqual_bounds;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;struct Cyc_List_List*free_qualvars;};
# 90 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple14{void*f0;int f1;};
# 96
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){
void*_Tmp0=Cyc_Absyn_compress(e);int _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f3;{int i=_Tmp1;
# 99
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
struct _tuple14*_Tmp2=(struct _tuple14*)es2->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=(int*)& _Tmp2->f1;{void*t=_Tmp4;int*b2=(int*)_Tmp3;
void*_Tmp5=Cyc_Absyn_compress(t);int _Tmp6;if(*((int*)_Tmp5)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp5)->f3;{int j=_Tmp6;
# 103
if(i==j){
if(b!=*b2)*b2=1;
return es;}
# 107
goto _LL8;}}else{
goto _LL8;}_LL8:;}}}{
# 111
struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp3=({struct _tuple14*_Tmp4=_cycalloc(sizeof(struct _tuple14));_Tmp4->f0=e,_Tmp4->f1=b;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=es;return _Tmp2;}}}else{
return es;};}
# 117
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bound tvar id for ",sizeof(char),19U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" is NULL",sizeof(char),9U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),3));});{
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;return _Tmp0;}}
# 122
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){
# 124
struct Cyc_List_List*r=0;
for(1;tvs!=0;tvs=tvs->tl){
struct Cyc_List_List*b=btvs;
for(1;b!=0;b=b->tl){
if(({int _Tmp0=get(tvs->hd)->identity;_Tmp0==((struct Cyc_Absyn_Tvar*)b->hd)->identity;}))
break;}
if(b==0)
r=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tvs->hd,_Tmp0->tl=r;_Tmp0;});}
# 133
r=Cyc_List_imp_rev(r);
return r;}
# 141
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){
for(1;free_evars!=0;free_evars=free_evars->tl){
void*e=(*((struct _tuple14*)free_evars->hd)).f0;
{void*_Tmp0=Cyc_Absyn_compress(e);if(*((int*)_Tmp0)==1)
goto _LL0;else{
continue;}_LL0:;}{
# 148
void*t;
struct _fat_ptr s;
{struct Cyc_Absyn_Kind*_Tmp0=Cyc_Tcutil_type_kind(e);switch((int)((struct Cyc_Absyn_Kind*)_Tmp0)->kind){case Cyc_Absyn_EffKind:
# 152
 t=Cyc_Absyn_heap_rgn_type;s=_tag_fat("{`H}",sizeof(char),5U);goto _LL5;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_false_type;s=_tag_fat("@false",sizeof(char),7U);goto _LL5;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_bounds_one();s=_tag_fat("bounds_one",sizeof(char),11U);goto _LL5;case Cyc_Absyn_AqualKind:
 t=Cyc_Absyn_al_qual_type;s=_tag_fat("aliasable",sizeof(char),10U);goto _LL5;default:
# 157
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("hidden type variable ",sizeof(char),22U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)e;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" isn't abstracted in type ",sizeof(char),27U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)in_typ;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});
return;}_LL5:;}
# 160
if(!Cyc_Unify_unify(e,t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't unify evar ",sizeof(char),18U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)e;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" with ",sizeof(char),7U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("!",sizeof(char),2U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),5));});}}}
# 171
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int allow_aqual_bounds){
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)==0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Kinds_constrain_kinds(k1,k2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" used with inconsistent kinds ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp4;_Tmp4.tag=8,_Tmp4.f1=(void*)k1;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" and ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=(void*)k2;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),6));});
if(tv->aquals_bound!=0 && !allow_aqual_bounds)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("alias bound appears on type variable ",sizeof(char),38U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" which is not in a function declaration or aggregate field",sizeof(char),59U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->aquals_bound==0)
((struct Cyc_Absyn_Tvar*)tvs2->hd)->aquals_bound=tv->aquals_bound;else{
if(tv->aquals_bound!=0 && Cyc_Tcutil_typecmp(tv->aquals_bound,((struct Cyc_Absyn_Tvar*)tvs2->hd)->aquals_bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=tv;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" used with inconsistent alias qualifier bounds ",sizeof(char),48U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)tv->aquals_bound;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" and ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp6;_Tmp6.tag=3,_Tmp6.f1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->aquals_bound;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),6));});}
if(tv->identity==-1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity!=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("same type variable has different identity!",sizeof(char),43U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});}
return tvs;}}}
# 193
({int _Tmp0=Cyc_Tcutil_new_tvar_id();tv->identity=_Tmp0;});{
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;return _Tmp0;}}
# 197
static int Cyc_Tctyp_cmp_fqv(struct _tuple14*te1,struct _tuple14*te2){
void*t1=(*te1).f0;
void*t2=(*te2).f0;
return Cyc_Tcutil_typecmp(t1,t2)==0;}
# 204
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar: bad identity in tv",sizeof(char),39U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
struct Cyc_Absyn_Tvar*tv2=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(tv2->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar: bad identity in tvs2",sizeof(char),41U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
if(tv2->identity==tv->identity)
return tvs;}}{
# 214
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=tv,_Tmp0->tl=tvs;return _Tmp0;}}
# 220
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
if(tv->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tv",sizeof(char),44U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
struct _tuple12*_Tmp0=(struct _tuple12*)tvs2->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=(int*)& _Tmp0->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp2;int*b2=(int*)_Tmp1;
if(tv2->identity==-1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tvs2",sizeof(char),46U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});
if(tv2->identity==tv->identity){
*b2=*b2 || b;
return tvs;}}}}{
# 232
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp1=({struct _tuple12*_Tmp2=_cycalloc(sizeof(struct _tuple12));_Tmp2->f0=tv,_Tmp2->f1=b;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=tvs;return _Tmp0;}}
# 239
static int Cyc_Tctyp_check_type_visit_f1(struct Cyc_Tctyp_CVTEnv*env,struct Cyc_Absyn_Exp*e){
# 242
env->loc=e->loc;
{void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 14: _Tmp1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp1;
({struct Cyc_Tctyp_CVTEnv _Tmp2=Cyc_Tctyp_i_check_type(*env,& Cyc_Kinds_ak,t,1,0);*env=_Tmp2;});goto _LL0;}case 19: _Tmp1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp1;
_Tmp1=t;goto _LL6;}case 17: _Tmp1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {void*t=_Tmp1;
({struct Cyc_Tctyp_CVTEnv _Tmp2=Cyc_Tctyp_i_check_type(*env,& Cyc_Kinds_ak,t,1,0);*env=_Tmp2;});goto _LL0;}case 38: _Tmp1=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp1;
({struct Cyc_Tctyp_CVTEnv _Tmp2=Cyc_Tctyp_i_check_type(*env,& Cyc_Kinds_ik,t,1,0);*env=_Tmp2;});goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 250
return 1;}
# 252
static int Cyc_Tctyp_check_type_visit_f2(struct Cyc_Tctyp_CVTEnv*env,struct Cyc_Absyn_Stmt*s){
({int(*_Tmp0)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp0;})(s->loc,_tag_fat("statement inside expression inside type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));}
# 255
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){
({void(*_Tmp0)(int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp0;})(Cyc_Tctyp_check_type_visit_f1,Cyc_Tctyp_check_type_visit_f2,& env,e);
return env;}
# 298 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){
# 303
struct Cyc_List_List*targs=*targsp;
int old_inst_evar=env.inst_type_evar;
env.inst_type_evar=env.generalize_evars;
for(1;targs!=0 && tvs!=0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 311
{struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Kinds_compress_kb(tv->kind);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=t;_Tmp1;});void*_Tmp1;switch(*((int*)_Tmp0.f0)){case 1: if(*((int*)_Tmp0.f1)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp1;
# 313
({struct Cyc_List_List*_Tmp2=Cyc_Tctyp_add_free_tvar(env.loc,env.kind_env,tv2,env.allow_aqual_bounds);env.kind_env=_Tmp2;});
({struct Cyc_List_List*_Tmp2=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,tv2,1);env.free_vars=_Tmp2;});
continue;}}else{goto _LL5;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f0)->f1)->kind==Cyc_Absyn_AqualKind){if(*((int*)_Tmp0.f1)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp1;
# 317
({void*_Tmp2=(void*)Cyc_Absyn_aqual_var_type(t,Cyc_Absyn_al_qual_type);targs->hd=_Tmp2;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_aqk,(void*)targs->hd,1,allow_abs_aggr);
continue;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 goto _LL0;}_LL0:;}{
# 322
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(env.loc,t);}}
# 326
if(targs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("too many type arguments for ",sizeof(char),29U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=typname;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(env.loc,_tag_fat(_Tmp2,sizeof(void*),2));});{
# 329
struct Cyc_List_List*hidden_ts=0;
for(1;tvs!=0;tvs=tvs->tl){
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=e,_Tmp0->tl=hidden_ts;_Tmp0;});{
# 334
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Kinds_bk);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}
# 337
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=*targsp;Cyc_List_imp_append(_Tmp1,Cyc_List_imp_rev(hidden_ts));});*targsp=_Tmp0;});
env.inst_type_evar=old_inst_evar;
return env;}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f0;struct Cyc_AssnDef_AssnMap f1;};
# 343
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 346
if(clause==0){
struct _tuple15 _Tmp0;_Tmp0.f0=env,({struct Cyc_AssnDef_AssnMap _Tmp1=Cyc_AssnDef_true_assnmap();_Tmp0.f1=_Tmp1;});return _Tmp0;}
Cyc_Tcexp_tcExp(env.te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=clause_name;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(" clause has type ",sizeof(char),18U);_Tmp2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3;_Tmp3.tag=3,_Tmp3.f1=(void*)clause->topt;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=
_tag_fat(" instead of integral type",sizeof(char),26U);_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(env.loc,_tag_fat(_Tmp4,sizeof(void*),4));});
env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_AssnDef_AssnMap assnmap=Cyc_Vcgen_clause2assn(clause);
struct _tuple15 _Tmp0;_Tmp0.f0=env,_Tmp0.f1=assnmap;return _Tmp0;}}
# 358
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 361
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
union Cyc_Absyn_AggrInfo _Tmp2=*info;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_AggrKind _Tmp5;if(_Tmp2.UnknownAggr.tag==1){_Tmp5=_Tmp2.UnknownAggr.val.f0;_Tmp4=_Tmp2.UnknownAggr.val.f1;_Tmp3=_Tmp2.UnknownAggr.val.f2;{enum Cyc_Absyn_AggrKind k=_Tmp5;struct _tuple1*n=_Tmp4;struct Cyc_Core_Opt*tgd=_Tmp3;
# 364
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);{
struct Cyc_Absyn_Aggrdecl*ad=*adp;
if((int)ad->kind!=(int)k){
if((int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting struct ",sizeof(char),18U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("instead of union ",sizeof(char),18U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});void*_TmpC[4];_TmpC[0]=& _Tmp8,_TmpC[1]=& _Tmp9,_TmpC[2]=& _TmpA,_TmpC[3]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),4));});else{
# 372
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting union ",sizeof(char),17U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("instead of struct ",sizeof(char),19U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});void*_TmpC[4];_TmpC[0]=& _Tmp8,_TmpC[1]=& _Tmp9,_TmpC[2]=& _TmpA,_TmpC[3]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),4));});}}
# 374
if(((unsigned)tgd &&(int)tgd->v)&&(!((unsigned)ad->impl)|| !ad->impl->tagged))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("@tagged qualfiers don't agree on union ",sizeof(char),40U);_Tmp9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=n;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),2));});
# 377
({union Cyc_Absyn_AggrInfo _Tmp8=Cyc_Absyn_KnownAggr(adp);*info=_Tmp8;});}
# 366
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp8)->tag==Cyc_Dict_Absent){
# 381
({struct Cyc_Tcenv_Tenv*_TmpA=te;unsigned _TmpB=loc;Cyc_Tc_tcAggrdecl(_TmpA,_TmpB,({struct Cyc_Absyn_Aggrdecl*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TmpC->kind=k,_TmpC->sc=3U,_TmpC->name=n,_TmpC->tvs=0,_TmpC->impl=0,_TmpC->attributes=0,_TmpC->expected_mem_kind=0;_TmpC;}));});
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);
({union Cyc_Absyn_AggrInfo _TmpA=Cyc_Absyn_KnownAggr(adp);*info=_TmpA;});
# 385
if(*targs!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("declare parameterized type ",sizeof(char),28U);_TmpB;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=n;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat(" before using",sizeof(char),14U);_TmpD;});void*_TmpD[3];_TmpD[0]=& _TmpA,_TmpD[1]=& _TmpB,_TmpD[2]=& _TmpC;Cyc_Warn_err2(loc,_tag_fat(_TmpD,sizeof(void*),3));});
return env;}
# 389
goto _LL8;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL8:;}}}
# 391
_Tmp4=*adp;goto _LL7;}}else{_Tmp4=*_Tmp2.KnownAggr.val;_LL7: {struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 393
env=Cyc_Tctyp_check_type_inst(env,targs,ad->tvs,expected_kind,allow_abs_aggr,ad->name,0);
# 395
if((allow_abs_aggr && ad->impl==0)&& !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,expected_kind))
# 400
ad->expected_mem_kind=1;
return env;}};}}
# 406
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 409
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
struct Cyc_List_List*targs=*targsp;
union Cyc_Absyn_DatatypeInfo _Tmp2=*info;int _Tmp3;void*_Tmp4;if(_Tmp2.UnknownDatatype.tag==1){_Tmp4=_Tmp2.UnknownDatatype.val.name;_Tmp3=_Tmp2.UnknownDatatype.val.is_extensible;{struct _tuple1*n=_Tmp4;int is_x=_Tmp3;
# 413
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp7)->tag==Cyc_Dict_Absent){
# 417
({struct Cyc_Tcenv_Tenv*_Tmp9=te;unsigned _TmpA=loc;Cyc_Tc_tcDatatypedecl(_Tmp9,_TmpA,({struct Cyc_Absyn_Datatypedecl*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_TmpB->sc=3U,_TmpB->name=n,_TmpB->tvs=0,_TmpB->fields=0,_TmpB->is_extensible=is_x;_TmpB;}));});
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 420
if(targs!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("declare parameterized datatype ",sizeof(char),32U);_TmpA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=n;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" before using",sizeof(char),14U);_TmpC;});void*_TmpC[3];_TmpC[0]=& _Tmp9,_TmpC[1]=& _TmpA,_TmpC[2]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),3));});
return env;}
# 424
goto _LL8;}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL8:;}}}
# 428
if(is_x && !(*tudp)->is_extensible)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("datatype ",sizeof(char),10U);_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=n;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" was not declared @extensible",sizeof(char),30U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});
({union Cyc_Absyn_DatatypeInfo _Tmp5=Cyc_Absyn_KnownDatatype(tudp);*info=_Tmp5;});
_Tmp4=*tudp;goto _LL7;}}else{_Tmp4=*_Tmp2.KnownDatatype.val;_LL7: {struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 433
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tud->name,0);}};}}
# 439
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 442
union Cyc_Absyn_DatatypeFieldInfo _Tmp0=*info;int _Tmp1;void*_Tmp2;void*_Tmp3;if(_Tmp0.UnknownDatatypefield.tag==1){_Tmp3=_Tmp0.UnknownDatatypefield.val.datatype_name;_Tmp2=_Tmp0.UnknownDatatypefield.val.field_name;_Tmp1=_Tmp0.UnknownDatatypefield.val.is_extensible;{struct _tuple1*tname=_Tmp3;struct _tuple1*fname=_Tmp2;int is_x=_Tmp1;
# 445
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(env.te,env.loc,tname);
struct Cyc_Absyn_Datatypefield*tuf;
# 450
{struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(tud->fields)->v;for(0;1;fs=fs->tl){
if(fs==0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Tcutil found a bad datatypefield",sizeof(char),33U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)fs->hd)->name,fname)==0){
tuf=(struct Cyc_Absyn_Datatypefield*)fs->hd;
break;}}}
# 457
({union Cyc_Absyn_DatatypeFieldInfo _Tmp4=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_Tmp4;});
_Tmp3=tud;_Tmp2=tuf;goto _LL4;}}else{_Tmp3=_Tmp0.KnownDatatypefield.val.f0;_Tmp2=_Tmp0.KnownDatatypefield.val.f1;_LL4: {struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;
# 460
return Cyc_Tctyp_check_type_inst(env,targsp,tud->tvs,expected_kind,allow_abs_aggr,tuf->name,0);}};}
# 465
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){
# 467
if(Cyc_List_length(ts)!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(" has wrong number of arguments",sizeof(char),31U);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp3;})(_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Tctyp_i_check_type(env,k,(void*)_check_null(ts)->hd,put_in_effect,1);}
# 472
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr,void*apptype){
# 475
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
struct Cyc_List_List*ts=*targsp;
void*_Tmp2;void*_Tmp3;switch(*((int*)c)){case 1:
# 479
 goto _LL7;case 2: _LL7: goto _LL9;case 0: _LL9: goto _LLB;case 8: _LLB:
 goto _LLD;case 7: _LLD: goto _LLF;case 6: _LLF: goto _LL11;case 12: _LL11:
 goto _LL13;case 11: _LL13: goto _LL15;case 14: _LL15: goto _LL17;case 21: _LL17:
# 483
 if(ts!=0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;})))_Tmp6=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;}));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});
return env;case 16:
# 487
 if(ts!=0)
({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,({typeof((void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;})))_Tmp6=(void*)((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_Tmp7->tag=0,_Tmp7->f1=c,_Tmp7->f2=ts;_Tmp7;}));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});
if(expected_kind!=& Cyc_Kinds_aqk)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Kind Q used where kind ",sizeof(char),24U);_Tmp5;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp6;_Tmp6.tag=9,_Tmp6.f1=expected_kind;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" was expected",sizeof(char),14U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});
return env;case 15:
# 493
 if(ts==0 || ts->tl!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("aquals expects a single argument",sizeof(char),33U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});
return Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ak,(void*)ts->hd,0,0);case 17:
# 497
 if(Cyc_List_length(ts)!=2)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("aquals expects two arguments",sizeof(char),29U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_aqk,(void*)_check_null(ts)->hd,0,0);{
void*ctx_bnd=({struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(env.te);Cyc_Tcutil_lookup_aquals(_Tmp4,(void*)ts->hd);});
if((unsigned)ctx_bnd)
_check_null(ts->tl)->hd=(void*)ctx_bnd;else{
# 505
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_aqk,(void*)ts->tl->hd,0,0);}
# 507
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp6=({struct _tuple14*_Tmp7=_cycalloc(sizeof(struct _tuple14));_Tmp7->f0=apptype,_Tmp7->f1=0;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=env.free_qualvars;_Tmp5;});env.free_qualvars=_Tmp4;});
return env;}case 3:
# 510
 for(1;ts!=0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_mk,(void*)ts->hd,1,1);}
return env;case 9:
# 514
 for(1;ts!=0;ts=ts->tl){
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ek,(void*)ts->hd,1,1);}
return env;case 19: _Tmp3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f1;_Tmp2=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f2;{struct _tuple1*n=_Tmp3;struct Cyc_Absyn_Enumdecl**edp=(struct Cyc_Absyn_Enumdecl**)_Tmp2;
# 518
if(ts!=0)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});
if(*edp==0 ||(*edp)->fields==0){
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){({struct Cyc_Absyn_Enumdecl*_Tmp6=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_Tmp6;});;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp6)->tag==Cyc_Dict_Absent){
# 523
({struct Cyc_Tcenv_Tenv*_Tmp8=te;unsigned _Tmp9=loc;Cyc_Tc_tcEnumdecl(_Tmp8,_Tmp9,({struct Cyc_Absyn_Enumdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_TmpA->sc=3U,_TmpA->name=n,_TmpA->fields=0;_TmpA;}));});
({struct Cyc_Absyn_Enumdecl*_Tmp8=*Cyc_Tcenv_lookup_enumdecl(te,loc,n);*edp=_Tmp8;});
goto _LL36;}else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LL36:;}}}
# 528
return env;}case 20: _Tmp3=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)c)->f1;{struct Cyc_List_List*fs=_Tmp3;
# 530
if(ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
# 533
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
if(({int(*_Tmp4)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp4;})(Cyc_strptrcmp,prev_fields,(*f->name).f1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("duplicate enum field name ",sizeof(char),27U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=f->name;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 540
prev_fields=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(*f->name).f1,_Tmp4->tl=prev_fields;_Tmp4;});}
if(f->tag==0)
({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_uint_exp(tag_count,f->loc);f->tag=_Tmp4;});else{
if(!Cyc_Tcutil_is_const_exp(f->tag))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("enum field ",sizeof(char),12U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=f->name;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(": expression is not constant",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});}
tag_count=1U + Cyc_Evexp_eval_const_uint_exp(_check_null(f->tag)).f0;}
# 547
return env;}}case 10:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ak,ts,1,_tag_fat("regions",sizeof(char),8U));case 4:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ek,ts,1,_tag_fat("region_t",sizeof(char),9U));case 18:
# 551
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_aqk,ts,1,_tag_fat("aqual_t",sizeof(char),8U));case 5:
# 553
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ik,ts,0,_tag_fat("tag_t",sizeof(char),6U));case 13:
 return Cyc_Tctyp_i_check_unary_app(env,& Cyc_Kinds_ik,ts,0,_tag_fat("@thin",sizeof(char),6U));case 24: _Tmp3=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo*info=_Tmp3;
# 556
return Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);}case 22: _Tmp3=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeInfo*info=_Tmp3;
# 558
return Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);}default: _Tmp3=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeFieldInfo*info=(union Cyc_Absyn_DatatypeFieldInfo*)_Tmp3;
# 560
return Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);}};}}
# 564
static int Cyc_Tctyp_bnd_qual_cmp(void*tvt1,void*tvt2){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(tvt1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(tvt2);_Tmp1.f1=_Tmp2;});_Tmp1;});int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0.f0)){case 2: switch(*((int*)_Tmp0.f1)){case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_Absyn_Tvar*tv2=_Tmp3;
# 567
return Cyc_Absyn_tvar_cmp(tv1,tv2);}case 1:
# 572
 goto _LLA;case 0: _LL10:
# 576
 goto _LL12;default: goto _LL13;}case 1: switch(*((int*)_Tmp0.f1)){case 1: _Tmp4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f1)->f3;{struct Cyc_Core_Opt*k1=_Tmp4;int i1=_Tmp2;struct Cyc_Core_Opt*k2=_Tmp3;int i2=_Tmp1;
# 569
if(i1==i2 &&({struct Cyc_Absyn_Kind*_Tmp5=(struct Cyc_Absyn_Kind*)_check_null(k1)->v;Cyc_Kinds_kind_eq(_Tmp5,(struct Cyc_Absyn_Kind*)_check_null(k2)->v);}))return 0;else{return 1;}}case 2: _LLA:
# 573
 goto _LLC;case 0: _LLC:
 goto _LLE;default: goto _LL13;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2!=0)switch(*((int*)_Tmp0.f1)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2!=0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2->hd;_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2->hd;{void*tv11=_Tmp4;void*tv21=_Tmp3;
# 571
return Cyc_Tctyp_bnd_qual_cmp(tv11,tv21);}}else{goto _LL13;}}else{goto _LL13;}case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}else{switch(*((int*)_Tmp0.f1)){case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}}}else{switch(*((int*)_Tmp0.f1)){case 1: _LLD: _LLE:
# 575
 goto _LL10;case 2: _LL11: _LL12:
# 578
 return 1;default: goto _LL13;}}default: _LL13:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Expect only tvars, evars or aquals in qualifier bounds: ",sizeof(char),57U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)tvt1;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(",",sizeof(char),2U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)tvt2;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),4));});};}
# 584
static int Cyc_Tctyp_aqualvar_unconstrained(struct _tuple14*t){
void*_Tmp0=Cyc_Absyn_compress((*t).f0);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 16:
# 587
 return 0;case 17:
# 589
 return 1;default: goto _LL5;}else{_LL5:
# 591
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Expect only Aqual types -- got ",sizeof(char),32U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)(*t).f0;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),2));});};}
# 595
static int Cyc_Tctyp_aqualsof_var_bnd(void*bndv,void*var){
void*_Tmp0;if(*((int*)bndv)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)bndv)->f1)==15){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)bndv)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)bndv)->f2->hd;{void*vv=_Tmp0;
# 598
return Cyc_Tcutil_typecmp(var,vv);}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 600
 return 1;};}
# 605
static void Cyc_Tctyp_constrain_vars(unsigned loc,struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List*vars,struct Cyc_List_List**qb,void*def,int make_ex_aliasable){
# 609
for(1;vars!=0;vars=vars->tl){
struct _tuple14*_Tmp0=(struct _tuple14*)vars->hd;int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*v=_Tmp2;int existential=_Tmp1;
if(!make_ex_aliasable)
existential=0;{
void*adef=def;
int explicit_bound_set=0;
void*_Tmp3;switch(*((int*)v)){case 2: _Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)v)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
# 617
if(tv->aquals_bound!=0){
adef=tv->aquals_bound;
explicit_bound_set=1;}{
# 621
struct Cyc_Core_Opt ko=Cyc_Kinds_tvar_kind_opt(tv);
_Tmp3=(unsigned)((struct Cyc_Absyn_Kind*)ko.v)?(struct Cyc_Absyn_Kind*)ko.v:& Cyc_Kinds_ek;goto _LL7;}}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)v)->f1!=0){_Tmp3=(struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)v)->f1->v;_LL7: {struct Cyc_Absyn_Kind*k=_Tmp3;
# 624
if(((int)k->kind==2 ||(int)k->kind==0)||(int)k->kind==1){
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_aqualsof_var_bnd,*qb,v);
if(explicit_bound_set && Cyc_Tcutil_typecmp(adef,xbnd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)v;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_warn2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});}
# 626
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found){
# 631
void*aqt=Cyc_Absyn_aqualsof_type(v);
if(existential && !explicit_bound_set)
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpA=({struct _tuple0*_TmpB=_cycalloc(sizeof(struct _tuple0));_TmpB->f0=aqt,_TmpB->f1=Cyc_Absyn_al_qual_type;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=*qb;_Tmp9;});*qb=_Tmp8;});else{
# 635
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpA=({struct _tuple0*_TmpB=_cycalloc(sizeof(struct _tuple0));_TmpB->f0=aqt,_TmpB->f1=adef;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=*qb;_Tmp9;});*qb=_Tmp8;});}
goto _LLC;}else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LLC:;}}}else{
# 639
if((int)k->kind==7){
struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,*qb,v);
if(explicit_bound_set && Cyc_Tcutil_typecmp(adef,xbnd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)v;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_warn2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});}
# 641
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found){
# 646
if(existential && !explicit_bound_set)
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpA=({struct _tuple0*_TmpB=_cycalloc(sizeof(struct _tuple0));_TmpB->f0=v,_TmpB->f1=Cyc_Absyn_al_qual_type;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=*qb;_Tmp9;});*qb=_Tmp8;});else{
# 649
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpA=({struct _tuple0*_TmpB=_cycalloc(sizeof(struct _tuple0));_TmpB->f0=v,_TmpB->f1=adef;_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=*qb;_Tmp9;});*qb=_Tmp8;});}
goto _LL11;}else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LL11:;}}}}
# 653
goto _LL3;}}else{goto _LLA;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)v)->f1)==17){_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)v)->f2;{struct Cyc_List_List*tv_bnd=_Tmp3;
# 655
void*tv=(void*)_check_null(tv_bnd)->hd;
{void*_Tmp4;if(*((int*)tv)==2){_Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)tv)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 658
if((unsigned)tv->aquals_bound){
adef=tv->aquals_bound;
explicit_bound_set=1;}
# 662
goto _LL16;}}else{
# 664
goto _LL16;}_LL16:;}
# 666
{struct _handler_cons _Tmp4;_push_handler(& _Tmp4);{int _Tmp5=0;if(setjmp(_Tmp4.handler))_Tmp5=1;if(!_Tmp5){
{void*x_bnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,*qb,tv);
if(explicit_bound_set && Cyc_Tcutil_typecmp(x_bnd,adef))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)v;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_warn2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});
_check_null(tv_bnd->tl)->hd=(void*)x_bnd;}
# 667
;_pop_handler();}else{void*_Tmp6=(void*)Cyc_Core_get_exn_thrown();void*_Tmp7;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp6)->tag==Cyc_Core_Not_found){
# 674
{void*_Tmp8=Cyc_Absyn_compress(tv);switch(*((int*)_Tmp8)){case 1:
 goto _LL24;case 2: _LL24:
# 677
 if(existential && !explicit_bound_set){
({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpB=({struct _tuple0*_TmpC=_cycalloc(sizeof(struct _tuple0));_TmpC->f0=tv,_TmpC->f1=Cyc_Absyn_al_qual_type;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=*qb;_TmpA;});*qb=_Tmp9;});
_check_null(tv_bnd->tl)->hd=(void*)Cyc_Absyn_al_qual_type;}else{
# 682
({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_TmpB=({struct _tuple0*_TmpC=_cycalloc(sizeof(struct _tuple0));_TmpC->f0=tv,_TmpC->f1=adef;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=*qb;_TmpA;});*qb=_Tmp9;});
_check_null(tv_bnd->tl)->hd=(void*)adef;}
# 685
goto _LL20;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)==15)
# 687
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("aquals bounds must be present",sizeof(char),30U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});else{goto _LL27;}default: _LL27:
# 689
 goto _LL20;}_LL20:;}
# 691
goto _LL1B;}else{_Tmp7=_Tmp6;{void*exn=_Tmp7;_rethrow(exn);}}_LL1B:;}}}
# 693
goto _LL3;}}else{goto _LLA;}default: _LLA:
# 696
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Expect only tvars, evars and aqual_vars -- got ",sizeof(char),48U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)v;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});
goto _LL3;}_LL3:;}}}}
# 702
static struct _tuple14*Cyc_Tctyp_tvar2type(struct _tuple12*in){
int _Tmp0;void*_Tmp1;_Tmp1=in->f0;_Tmp0=in->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;int b=_Tmp0;
void*t=Cyc_Absyn_var_type(tv);
struct _tuple14*_Tmp2=_cycalloc(sizeof(struct _tuple14));_Tmp2->f0=t,_Tmp2->f1=b;return _Tmp2;}}
# 708
static int Cyc_Tctyp_evar_unconstrained(struct _tuple14*evb){
void*_Tmp0=Cyc_Absyn_compress((*evb).f0);if(*((int*)_Tmp0)==1)
return 1;else{
return 0;};}
# 715
static void Cyc_Tctyp_update_qualifier_bounds(unsigned loc,struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List*flagged_tvars,struct Cyc_List_List**qb,void*def){
# 719
struct Cyc_List_List*freeqvs=env.free_qualvars;
struct Cyc_List_List*freeevs=env.free_evars;
({unsigned _Tmp0=loc;struct Cyc_Tctyp_CVTEnv _Tmp1=env;struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Tctyp_tvar2type,flagged_tvars);struct Cyc_List_List**_Tmp3=qb;Cyc_Tctyp_constrain_vars(_Tmp0,_Tmp1,_Tmp2,_Tmp3,def,1);});
({unsigned _Tmp0=loc;struct Cyc_Tctyp_CVTEnv _Tmp1=env;struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Tctyp_tvar2type,env.free_vars);struct Cyc_List_List**_Tmp3=qb;Cyc_Tctyp_constrain_vars(_Tmp0,_Tmp1,_Tmp2,_Tmp3,def,0);});
({unsigned _Tmp0=loc;struct Cyc_Tctyp_CVTEnv _Tmp1=env;struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(int(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_filter;_Tmp3;})(Cyc_Tctyp_evar_unconstrained,freeevs);struct Cyc_List_List**_Tmp3=qb;Cyc_Tctyp_constrain_vars(_Tmp0,_Tmp1,_Tmp2,_Tmp3,def,0);});
({unsigned _Tmp0=loc;struct Cyc_Tctyp_CVTEnv _Tmp1=env;struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(int(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_filter;_Tmp3;})(Cyc_Tctyp_aqualvar_unconstrained,freeqvs);struct Cyc_List_List**_Tmp3=qb;Cyc_Tctyp_constrain_vars(_Tmp0,_Tmp1,_Tmp2,_Tmp3,def,0);});}struct _tuple16{enum Cyc_Absyn_Format_Type f0;void*f1;};struct _tuple17{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 727
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 729
void*_Tmp0;unsigned _Tmp1;_Tmp1=env.loc;_Tmp0=env.te;{unsigned loc=_Tmp1;struct Cyc_Tcenv_Tenv*te=_Tmp0;
{void*_Tmp2=Cyc_Absyn_compress(t);enum Cyc_Absyn_AggrKind _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;int _TmpB;unsigned _TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;void*_Tmp11;void*_Tmp12;void*_Tmp13;void*_Tmp14;switch(*((int*)_Tmp2)){case 0: _Tmp14=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp13=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{void*c=_Tmp14;struct Cyc_List_List**targsp=_Tmp13;
# 732
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr,t);
# 734
goto _LL3;}case 1: _Tmp14=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f1;_Tmp13=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f2;{struct Cyc_Core_Opt**k=_Tmp14;void**ref=_Tmp13;
# 737
if(*k==0 ||
 Cyc_Kinds_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)(*k)->v)&& !
Cyc_Kinds_kind_leq((struct Cyc_Absyn_Kind*)_check_null(*k)->v,expected_kind))
({struct Cyc_Core_Opt*_Tmp15=Cyc_Kinds_kind_to_opt(expected_kind);*k=_Tmp15;});
# 742
if(env.fn_result && env.generalize_evars){
if((int)expected_kind->kind==3 && !te->tempest_generalize){
*ref=Cyc_Absyn_heap_rgn_type;
goto _LL3;}else{
# 747
if((int)expected_kind->kind==7 && !te->tempest_generalize){
*ref=Cyc_Absyn_al_qual_type;
goto _LL3;}}}
# 753
if((env.inst_type_evar &&(int)expected_kind->kind==7)&& !te->tempest_generalize){
# 756
*ref=Cyc_Absyn_al_qual_type;
goto _LL3;}
# 760
if((env.generalize_evars &&(int)expected_kind->kind!=5)&&(int)expected_kind->kind!=6){
# 763
struct Cyc_Absyn_Tvar*v=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp15->tag=2,_Tmp15->f1=0,_Tmp15->f2=expected_kind;_Tmp15;}));
({void*_Tmp15=Cyc_Absyn_var_type(v);*ref=_Tmp15;});
_Tmp14=v;goto _LL9;}
# 767
({struct Cyc_List_List*_Tmp15=Cyc_Tctyp_add_free_evar(env.free_evars,t,put_in_effect);env.free_evars=_Tmp15;});
goto _LL3;}case 2: _Tmp14=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2)->f1;_LL9: {struct Cyc_Absyn_Tvar*v=_Tmp14;
# 770
{void*_Tmp15=Cyc_Kinds_compress_kb(v->kind);void*_Tmp16;if(*((int*)_Tmp15)==1){_Tmp16=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp15)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp16;
({struct Cyc_Core_Opt*_Tmp17=({struct Cyc_Core_Opt*_Tmp18=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp19=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp1A=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp1A->tag=2,_Tmp1A->f1=0,_Tmp1A->f2=expected_kind;_Tmp1A;});_Tmp18->v=_Tmp19;});_Tmp18;});*f=_Tmp17;});goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}
# 776
({struct Cyc_List_List*_Tmp15=Cyc_Tctyp_add_free_tvar(loc,env.kind_env,v,env.allow_aqual_bounds);env.kind_env=_Tmp15;});
# 779
({struct Cyc_List_List*_Tmp15=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,v,put_in_effect);env.free_vars=_Tmp15;});
# 781
{void*_Tmp15=Cyc_Kinds_compress_kb(v->kind);void*_Tmp16;void*_Tmp17;if(*((int*)_Tmp15)==2){_Tmp17=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp15)->f1;_Tmp16=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp15)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp17;struct Cyc_Absyn_Kind*k=_Tmp16;
# 783
if(Cyc_Kinds_kind_leq(expected_kind,k))
({struct Cyc_Core_Opt*_Tmp18=({struct Cyc_Core_Opt*_Tmp19=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp1A=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp1B=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp1B->tag=2,_Tmp1B->f1=0,_Tmp1B->f2=expected_kind;_Tmp1B;});_Tmp19->v=_Tmp1A;});_Tmp19;});*f=_Tmp18;});
goto _LL1F;}}else{
goto _LL1F;}_LL1F:;}
# 788
goto _LL3;}case 9: _Tmp14=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f1->r;_Tmp13=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp2)->f2;{void*td=_Tmp14;void***topt=_Tmp13;
# 793
void*new_typ=Cyc_Tcutil_copy_type(Cyc_Absyn_compress(t));
{void*_Tmp15;switch(*((int*)td)){case 0: _Tmp15=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp15;
# 796
if(env.te->in_extern_c_include)
ad->sc=4U;
Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL24;}case 1: _Tmp15=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp15;
# 800
if(env.te->in_extern_c_include)
ed->sc=4U;
Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL24;}default: _Tmp15=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)td)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp15;
# 804
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL24;}}_LL24:;}
# 806
({void**_Tmp15=({void**_Tmp16=_cycalloc(sizeof(void*));*_Tmp16=new_typ;_Tmp16;});*topt=_Tmp15;});
return Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);}case 3: _Tmp14=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;_Tmp13=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.elt_tq;_Tmp12=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.eff;_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.nullable;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.bounds;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.zero_term;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.autoreleased;_TmpD=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.aqual;{void*t1=_Tmp14;struct Cyc_Absyn_Tqual*tqp=_Tmp13;void**eff_type=_Tmp12;void*nullable=_Tmp11;void*b=_Tmp10;void*zt=_TmpF;void*rel=_TmpE;void**aq=_TmpD;
# 817
void*override_aq=*aq;
{void*_Tmp15=*eff_type;if(*((int*)_Tmp15)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f1)){case 7:
# 820
*eff_type=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_un_qual_type;
goto _LL2B;case 8:
# 824
*eff_type=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_rc_qual_type;
goto _LL2B;default: goto _LL30;}else{_LL30:
# 828
 goto _LL2B;}_LL2B:;}
# 843 "tctyp.cyc"
if(override_aq!=*aq){
{void*_Tmp15=*aq;void*_Tmp16;if(*((int*)_Tmp15)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f1)==17){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2)->hd)==1){_Tmp16=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2->tl;{struct Cyc_List_List*bnd=_Tmp16;
# 846
goto _LL32;}}else{goto _LL35;}}else{goto _LL35;}}else{goto _LL35;}}else{_LL35:
# 848
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("`U and `RC shorthands override explicit aqual",sizeof(char),46U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_warn2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});}_LL32:;}
# 850
*aq=override_aq;}
# 852
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ak,t1,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ek,*eff_type,1,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,zt,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,nullable,0,1);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_ptrbk,b,0,allow_abs_aggr);
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_boolk,rel,0,1);
{void*_Tmp15=*aq;void*_Tmp16;if(*((int*)_Tmp15)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f1)==17){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2)->hd)==1){_Tmp16=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp15)->f2->tl;{struct Cyc_List_List*bnd=_Tmp16;
# 860
if(env.def_aliasable)
*aq=Cyc_Absyn_al_qual_type;else{
# 863
_check_null(bnd)->hd=(void*)Cyc_Absyn_rtd_qual_type;}
goto _LL37;}}else{goto _LL3A;}}else{goto _LL3A;}}else{goto _LL3A;}}else{_LL3A:
# 866
 goto _LL37;}_LL37:;}
# 869
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_aqk,*aq,0,1);
({int _Tmp15=Cyc_Tcutil_extract_const_from_typedef(env.loc,tqp->print_const,t1);tqp->real_const=_Tmp15;});
# 874
({void*_Tmp15=zt;Cyc_Unify_unify(_Tmp15,Cyc_Tcutil_is_char_type(t1)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);
if(is_zero_terminated && !Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("cannot have a pointer to zero-terminated ",sizeof(char),42U);_Tmp16;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp17;_Tmp17.tag=2,_Tmp17.f1=(void*)t1;_Tmp17;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat(" elements",sizeof(char),10U);_Tmp18;});void*_Tmp18[3];_Tmp18[0]=& _Tmp15,_Tmp18[1]=& _Tmp16,_Tmp18[2]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),3));});{
# 880
struct Cyc_Absyn_Exp*e=({void*_Tmp15=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp15,b);});
if(e!=0){
struct _tuple13 _Tmp15=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp16;unsigned _Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{unsigned sz=_Tmp17;int known=_Tmp16;
if(is_zero_terminated &&(!known || sz < 1U))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=_tag_fat("zero-terminated pointer cannot point to empty sequence",sizeof(char),55U);_Tmp19;});void*_Tmp19[1];_Tmp19[0]=& _Tmp18;Cyc_Warn_err2(loc,_tag_fat(_Tmp19,sizeof(void*),1));});}}
# 886
goto _LL3;}}}case 8: _Tmp14=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp14;
# 891
({struct Cyc_Tcenv_Tenv*_Tmp15=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp15,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("valueof_t requires an int expression",sizeof(char),37U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);
goto _LL3;}case 10: _Tmp14=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp14;
# 900
({struct Cyc_Tcenv_Tenv*_Tmp15=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp15,0,e);});
goto _LL3;}case 4: _Tmp14=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.elt_type;_Tmp13=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.tq;_Tmp12=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.num_elts;_Tmp11=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.zero_term;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1.zt_loc;{void*t1=_Tmp14;struct Cyc_Absyn_Tqual*tqp=_Tmp13;struct Cyc_Absyn_Exp**eptr=_Tmp12;void*zt=_Tmp11;unsigned ztl=_TmpC;
# 905
struct Cyc_Absyn_Exp*e=*eptr;
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_mk,t1,1,allow_abs_aggr);
({int _Tmp15=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t1);tqp->real_const=_Tmp15;});{
# 909
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);
if(is_zero_terminated){
# 912
if(!Cyc_Tcutil_admits_zero(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("cannot have a zero-terminated array of ",sizeof(char),40U);_Tmp16;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp17;_Tmp17.tag=2,_Tmp17.f1=(void*)t1;_Tmp17;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat(" elements",sizeof(char),10U);_Tmp18;});void*_Tmp18[3];_Tmp18[0]=& _Tmp15,_Tmp18[1]=& _Tmp16,_Tmp18[2]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),3));});}
# 917
if(e==0){
# 920
if(!is_zero_terminated)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("array bound defaults to 1 here",sizeof(char),31U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_warn2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp15=e=Cyc_Absyn_uint_exp(1U,0U);*eptr=_Tmp15;});}
# 924
({struct Cyc_Tcenv_Tenv*_Tmp15=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp15,0,e);});
if(!Cyc_Tcutil_coerce_uint_type(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("array bounds expression is not an unsigned int",sizeof(char),47U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple13 _Tmp15=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp16;unsigned _Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{unsigned sz=_Tmp17;int known=_Tmp16;
# 930
if((is_zero_terminated && known)&& sz < 1U)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=_tag_fat("zero terminated array cannot have zero elements",sizeof(char),48U);_Tmp19;});void*_Tmp19[1];_Tmp19[0]=& _Tmp18;Cyc_Warn_warn2(loc,_tag_fat(_Tmp19,sizeof(void*),1));});
# 933
if((known && sz < 1U)&& Cyc_Flags_tovc_r){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=_tag_fat("0-element arrays are supported only with gcc, changing to 1",sizeof(char),60U);_Tmp19;});void*_Tmp19[1];_Tmp19[0]=& _Tmp18;Cyc_Warn_warn2(loc,_tag_fat(_Tmp19,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp18=Cyc_Absyn_uint_exp(1U,0U);*eptr=_Tmp18;});}
# 937
goto _LL3;}}}}case 5: _Tmp14=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.tvars;_Tmp13=(void**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.effect;_Tmp12=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ret_tqual;_Tmp11=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ret_type;_Tmp10=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.args;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.c_varargs;_TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.cyc_varargs;_TmpE=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.qual_bnd;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.attributes;_TmpA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.requires_clause;_Tmp9=(struct Cyc_AssnDef_AssnMap**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.requires_assn;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ensures_clause;_Tmp7=(struct Cyc_AssnDef_AssnMap**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.ensures_assn;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.throws_clause;_Tmp5=(struct Cyc_AssnDef_AssnMap**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.throws_assn;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp2)->f1.effconstr;{struct Cyc_List_List**btvs=_Tmp14;void**eff=_Tmp13;struct Cyc_Absyn_Tqual*rtq=_Tmp12;void*tr=_Tmp11;struct Cyc_List_List*args=_Tmp10;int c_vararg=_TmpB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpF;struct Cyc_List_List**qb=(struct Cyc_List_List**)_TmpE;struct Cyc_List_List*atts=_TmpD;struct Cyc_Absyn_Exp*req=_TmpA;struct Cyc_AssnDef_AssnMap**req_relns=(struct Cyc_AssnDef_AssnMap**)_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;struct Cyc_AssnDef_AssnMap**ens_relns=(struct Cyc_AssnDef_AssnMap**)_Tmp7;struct Cyc_Absyn_Exp*thrws=_Tmp6;struct Cyc_AssnDef_AssnMap**thrws_relns=(struct Cyc_AssnDef_AssnMap**)_Tmp5;struct Cyc_List_List*effc=_Tmp4;
# 946
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(1;atts!=0;atts=atts->tl){
void*_Tmp15=(void*)atts->hd;int _Tmp16;int _Tmp17;enum Cyc_Absyn_Format_Type _Tmp18;switch(*((int*)_Tmp15)){case 1:
 is_stdcall=1;goto _LL42;case 2:
 is_cdecl=1;goto _LL42;case 3:
 is_fastcall=1;goto _LL42;case 19: _Tmp18=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp15)->f1;_Tmp17=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp15)->f2;_Tmp16=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp15)->f3;{enum Cyc_Absyn_Format_Type fmttype=_Tmp18;int i=_Tmp17;int j=_Tmp16;
# 959
++ num_formats;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;
goto _LL42;}default:
# 965
 if(!Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("bad function type attribute ",sizeof(char),29U);_Tmp1A;});struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=10,_Tmp1B.f1=(void*)atts->hd;_Tmp1B;});void*_Tmp1B[2];_Tmp1B[0]=& _Tmp19,_Tmp1B[1]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),2));});}_LL42:;}
# 968
if((is_stdcall + is_cdecl)+ is_fastcall > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("function can't have multiple calling conventions",sizeof(char),49U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
if(num_formats > 1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("function can't have multiple format attributes",sizeof(char),47U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});
# 975
Cyc_Tcutil_check_unique_tvars(loc,*btvs);
{struct Cyc_List_List*b=*btvs;for(0;b!=0;b=b->tl){
({int _Tmp15=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_Tmp15;});
({struct Cyc_List_List*_Tmp15=Cyc_Tctyp_add_bound_tvar(env.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);env.kind_env=_Tmp15;});{
void*_Tmp15=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);if(*((int*)_Tmp15)==0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp15)->f1)->kind==Cyc_Absyn_MemKind){
# 981
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("function abstracts Mem type variable ",sizeof(char),38U);_Tmp17;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp18;_Tmp18.tag=7,_Tmp18.f1=(struct Cyc_Absyn_Tvar*)b->hd;_Tmp18;});void*_Tmp18[2];_Tmp18[0]=& _Tmp16,_Tmp18[1]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),2));});goto _LL4D;}else{goto _LL50;}}else{_LL50:
 goto _LL4D;}_LL4D:;}}}{
# 987
struct Cyc_Tctyp_CVTEnv new_env=({struct Cyc_Tctyp_CVTEnv _Tmp15;_Tmp15.loc=loc,_Tmp15.te=te,_Tmp15.kind_env=env.kind_env,_Tmp15.fn_result=1,_Tmp15.def_aliasable=1,_Tmp15.inst_type_evar=0,_Tmp15.generalize_evars=env.generalize_evars,_Tmp15.allow_aqual_bounds=1,_Tmp15.free_vars=0,_Tmp15.free_evars=0,_Tmp15.free_qualvars=0;_Tmp15;});
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_mk,tr,1,1);
({int _Tmp15=Cyc_Tcutil_extract_const_from_typedef(loc,rtq->print_const,tr);rtq->real_const=_Tmp15;});
new_env.fn_result=0;
# 995
{struct Cyc_List_List*a=args;for(0;a!=0;a=a->tl){
struct _tuple9*trip=(struct _tuple9*)a->hd;
void*t=(*trip).f2;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_mk,t,1,1);{
int ec=Cyc_Tcutil_extract_const_from_typedef(loc,(*trip).f1.print_const,t);
(*trip).f1.real_const=ec;
# 1003
if(Cyc_Tcutil_is_array_type(t)){
# 1005
void*ptr_rgn=Cyc_Absyn_new_evar(0,0);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,ptr_rgn,1,1);{
void*aqt=({void*_Tmp15=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,0);Cyc_Absyn_aqual_var_type(_Tmp15,Cyc_Absyn_al_qual_type);});
# 1009
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_aqk,aqt,1,1);
({void*_Tmp15=Cyc_Tcutil_promote_array(t,ptr_rgn,aqt,0);(*trip).f2=_Tmp15;});}}}}}
# 1014
if(cyc_vararg!=0){
if(c_vararg)({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("both c_vararg and cyc_vararg",sizeof(char),29U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;({int(*_Tmp17)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp17;})(_tag_fat(_Tmp16,sizeof(void*),1));});{
struct Cyc_Absyn_VarargInfo _Tmp15=*cyc_vararg;int _Tmp16;void*_Tmp17;_Tmp17=_Tmp15.type;_Tmp16=_Tmp15.inject;{void*vt=_Tmp17;int vi=_Tmp16;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_mk,vt,1,1);
({int _Tmp18=Cyc_Tcutil_extract_const_from_typedef(loc,cyc_vararg->tq.print_const,vt);cyc_vararg->tq.real_const=_Tmp18;});
# 1020
if(vi){
void*_Tmp18=Cyc_Absyn_compress(vt);void*_Tmp19;void*_Tmp1A;void*_Tmp1B;if(*((int*)_Tmp18)==3){_Tmp1B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp18)->f1.elt_type;_Tmp1A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp18)->f1.ptr_atts.bounds;_Tmp19=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp18)->f1.ptr_atts.zero_term;{void*et=_Tmp1B;void*bs=_Tmp1A;void*zt=_Tmp19;
# 1023
{void*_Tmp1C=Cyc_Absyn_compress(et);if(*((int*)_Tmp1C)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1C)->f1)==22){
# 1025
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=_tag_fat("can't inject into a zeroterm pointer",sizeof(char),37U);_Tmp1E;});void*_Tmp1E[1];_Tmp1E[0]=& _Tmp1D;Cyc_Warn_err2(loc,_tag_fat(_Tmp1E,sizeof(void*),1));});
if(!({void*_Tmp1D=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_Tmp1D,bs);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=_tag_fat("can't inject into a fat pointer to datatype",sizeof(char),44U);_Tmp1E;});void*_Tmp1E[1];_Tmp1E[0]=& _Tmp1D;Cyc_Warn_err2(loc,_tag_fat(_Tmp1E,sizeof(void*),1));});
goto _LL5A;}else{goto _LL5D;}}else{_LL5D:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=_tag_fat("can't inject a non-datatype type",sizeof(char),33U);_Tmp1E;});void*_Tmp1E[1];_Tmp1E[0]=& _Tmp1D;Cyc_Warn_err2(loc,_tag_fat(_Tmp1E,sizeof(void*),1));});goto _LL5A;}_LL5A:;}
# 1032
goto _LL55;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("expecting a datatype pointer type",sizeof(char),34U);_Tmp1D;});void*_Tmp1D[1];_Tmp1D[0]=& _Tmp1C;Cyc_Warn_err2(loc,_tag_fat(_Tmp1D,sizeof(void*),1));});goto _LL55;}_LL55:;}}}}
# 1037
if(num_formats > 0){
int num_args=Cyc_List_length(args);
if((((fmt_desc_arg < 0 || fmt_desc_arg > num_args)|| fmt_arg_start < 0)||
# 1041
(cyc_vararg==0 && !c_vararg)&& fmt_arg_start!=0)||
(cyc_vararg!=0 || c_vararg)&& fmt_arg_start!=num_args + 1)
# 1044
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat("bad format descriptor",sizeof(char),22U);_Tmp16;});void*_Tmp16[1];_Tmp16[0]=& _Tmp15;Cyc_Warn_err2(loc,_tag_fat(_Tmp16,sizeof(void*),1));});else{
# 1047
struct _tuple9 _Tmp15=*({struct _tuple9*(*_Tmp16)(struct Cyc_List_List*,int)=(struct _tuple9*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp16;})(args,fmt_desc_arg - 1);void*_Tmp16;_Tmp16=_Tmp15.f2;{void*t=_Tmp16;
# 1049
{void*_Tmp17=Cyc_Absyn_compress(t);void*_Tmp18;void*_Tmp19;if(*((int*)_Tmp17)==3){_Tmp19=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp17)->f1.elt_type;_Tmp18=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp17)->f1.ptr_atts.bounds;{void*et=_Tmp19;void*b=_Tmp18;
# 1052
if(!Cyc_Tcutil_is_char_type(et))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("format descriptor is not a string",sizeof(char),34U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});else{
# 1055
struct Cyc_Absyn_Exp*e=({void*_Tmp1A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp1A,b);});
if(e==0 && c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});else{
if(e!=0 && !c_vararg)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});}}
# 1061
goto _LL62;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat("format descriptor is not a string type",sizeof(char),39U);_Tmp1B;});void*_Tmp1B[1];_Tmp1B[0]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),1));});goto _LL62;}_LL62:;}
# 1064
if(fmt_arg_start!=0 && !c_vararg){
# 1068
int problem;
{struct _tuple16 _Tmp17=({struct _tuple16 _Tmp18;_Tmp18.f0=ft,({void*_Tmp19=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(_check_null(cyc_vararg)->type));_Tmp18.f1=_Tmp19;});_Tmp18;});void*_Tmp18;switch((int)_Tmp17.f0){case Cyc_Absyn_Printf_ft: if(*((int*)_Tmp17.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)->f1.KnownDatatype.tag==2){_Tmp18=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp18;
# 1071
problem=({struct _tuple1*_Tmp19=tud->name;Cyc_Absyn_qvar_cmp(_Tmp19,Cyc_Absyn_datatype_print_arg_qvar());})!=0;
goto _LL67;}}else{goto _LL6C;}}else{goto _LL6C;}}else{goto _LL6C;}case Cyc_Absyn_Scanf_ft: if(*((int*)_Tmp17.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)->f1.KnownDatatype.tag==2){_Tmp18=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp17.f1)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp18;
# 1074
problem=({struct _tuple1*_Tmp19=tud->name;Cyc_Absyn_qvar_cmp(_Tmp19,Cyc_Absyn_datatype_scanf_arg_qvar());})!=0;
goto _LL67;}}else{goto _LL6C;}}else{goto _LL6C;}}else{goto _LL6C;}default: _LL6C:
# 1077
 problem=1;
goto _LL67;}_LL67:;}
# 1080
if(problem)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("format attribute and vararg types don't match",sizeof(char),46U);_Tmp18;});void*_Tmp18[1];_Tmp18[0]=& _Tmp17;Cyc_Warn_err2(loc,_tag_fat(_Tmp18,sizeof(void*),1));});}}}}
# 1086
{struct Cyc_List_List*qbit=*qb;for(0;qbit!=0;qbit=qbit->tl){
struct _tuple0*_Tmp15=(struct _tuple0*)qbit->hd;void*_Tmp16;void*_Tmp17;_Tmp17=_Tmp15->f0;_Tmp16=_Tmp15->f1;{void*tv=_Tmp17;void*bnd=_Tmp16;
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_aqk,bnd,0,0);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_aqk,tv,0,0);}}}
# 1092
{struct Cyc_List_List*ec=effc;for(0;ec!=0;ec=ec->tl){
void*_Tmp15=(void*)ec->hd;void*_Tmp16;void*_Tmp17;switch(*((int*)_Tmp15)){case 0: _Tmp17=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_Tmp15)->f1;{void*t=_Tmp17;
# 1095
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,t,1,1);
goto _LL71;}case 1: _Tmp17=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp15)->f1;_Tmp16=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp15)->f2;{void*t1=_Tmp17;void*t2=_Tmp16;
_Tmp17=t1;_Tmp16=t2;goto _LL77;}default: _Tmp17=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp15)->f1;_Tmp16=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp15)->f2;_LL77: {void*t1=_Tmp17;void*t2=_Tmp16;
# 1099
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,t1,1,1);
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,t2,1,1);
goto _LL71;}}_LL71:;}}{
# 1108
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_bogus_fenv(tr,args);
struct Cyc_Tcenv_Tenv*old_te=new_env.te;
({struct Cyc_Tcenv_Tenv*_Tmp15=({struct Cyc_Tcenv_Tenv*_Tmp16=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_Tmp16->ns=te->ns,_Tmp16->ae=te->ae,_Tmp16->le=fenv,_Tmp16->allow_valueof=1,_Tmp16->in_extern_c_include=te->in_extern_c_include,_Tmp16->in_tempest=te->in_tempest,_Tmp16->tempest_generalize=te->tempest_generalize,_Tmp16->in_extern_c_inc_repeat=te->in_extern_c_inc_repeat;_Tmp16;});new_env.te=_Tmp15;});
# 1114
if(req!=0){
struct _tuple15 _Tmp15=Cyc_Tctyp_check_clause(new_env,_tag_fat("@requires",sizeof(char),10U),req);struct Cyc_AssnDef_AssnMap _Tmp16;struct Cyc_Tctyp_CVTEnv _Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{struct Cyc_Tctyp_CVTEnv nenv=_Tmp17;struct Cyc_AssnDef_AssnMap req_assn=_Tmp16;
new_env=nenv;
({struct Cyc_AssnDef_AssnMap*_Tmp18=({struct Cyc_AssnDef_AssnMap*_Tmp19=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_Tmp19=req_assn;_Tmp19;});*req_relns=_Tmp18;});}}
# 1119
if(ens!=0){
struct _tuple15 _Tmp15=Cyc_Tctyp_check_clause(new_env,_tag_fat("@ensures",sizeof(char),9U),ens);struct Cyc_AssnDef_AssnMap _Tmp16;struct Cyc_Tctyp_CVTEnv _Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{struct Cyc_Tctyp_CVTEnv nenv=_Tmp17;struct Cyc_AssnDef_AssnMap ens_assn=_Tmp16;
new_env=nenv;
({struct Cyc_AssnDef_AssnMap*_Tmp18=({struct Cyc_AssnDef_AssnMap*_Tmp19=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_Tmp19=ens_assn;_Tmp19;});*ens_relns=_Tmp18;});}}
# 1124
if(thrws!=0){
struct _tuple15 _Tmp15=Cyc_Tctyp_check_clause(new_env,_tag_fat("@throws",sizeof(char),8U),thrws);struct Cyc_AssnDef_AssnMap _Tmp16;struct Cyc_Tctyp_CVTEnv _Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{struct Cyc_Tctyp_CVTEnv nenv=_Tmp17;struct Cyc_AssnDef_AssnMap thrws_assn=_Tmp16;
new_env=nenv;
({struct Cyc_AssnDef_AssnMap*_Tmp18=({struct Cyc_AssnDef_AssnMap*_Tmp19=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_Tmp19=thrws_assn;_Tmp19;});*thrws_relns=_Tmp18;});}}
# 1129
new_env.te=old_te;
# 1131
if(*eff!=0){
new_env=Cyc_Tctyp_i_check_type(new_env,& Cyc_Kinds_ek,*eff,1,1);
if(!Cyc_Tcutil_type_in_effect(0,Cyc_Absyn_heap_rgn_type,_check_null(*eff)))
({void*_Tmp15=Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_Tmp16[2];_Tmp16[0]=Cyc_Absyn_heap_rgn_type,_Tmp16[1]=_check_null(*eff);Cyc_List_list(_tag_fat(_Tmp16,sizeof(void*),2));})));*eff=_Tmp15;});}else{
# 1138
struct Cyc_List_List*effect;effect=_cycalloc(sizeof(struct Cyc_List_List)),effect->hd=Cyc_Absyn_heap_rgn_type,effect->tl=0;
# 1143
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs!=0;tvs=tvs->tl){
struct _tuple12 _Tmp15=*((struct _tuple12*)tvs->hd);int _Tmp16;void*_Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{struct Cyc_Absyn_Tvar*tv=_Tmp17;int put_in_eff=_Tmp16;
if(!put_in_eff)continue;{
void*_Tmp18=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp19;switch(*((int*)_Tmp18)){case 2: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp18)->f2)->kind){case Cyc_Absyn_BoolKind:
 goto _LL88;case Cyc_Absyn_PtrBndKind: _LL88:
 goto _LL8A;case Cyc_Absyn_IntKind: _LL8A:
 goto _LL8C;case Cyc_Absyn_EffKind: _Tmp19=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp18)->f1;{struct Cyc_Core_Opt**f=_Tmp19;
# 1154
({struct Cyc_Core_Opt*_Tmp1A=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ek);*f=_Tmp1A;});goto _LL94;}default: goto _LL97;}case 0: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp18)->f1)->kind){case Cyc_Absyn_BoolKind: _LL8C:
# 1150
 goto _LL8E;case Cyc_Absyn_PtrBndKind: _LL8E:
 goto _LL90;case Cyc_Absyn_IntKind: _LL90:
 goto _LL84;case Cyc_Absyn_EffKind: _LL94:
# 1156
 effect=({struct Cyc_List_List*_Tmp1A=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1B=Cyc_Absyn_var_type(tv);_Tmp1A->hd=_Tmp1B;}),_Tmp1A->tl=effect;_Tmp1A;});goto _LL84;default: _LL97: _LL98:
# 1161
 effect=({struct Cyc_List_List*_Tmp1A=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1B=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(tv));_Tmp1A->hd=_Tmp1B;}),_Tmp1A->tl=effect;_Tmp1A;});goto _LL84;}default: _Tmp19=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp18)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp19;
# 1158
({struct Cyc_Core_Opt*_Tmp1A=({struct Cyc_Core_Opt*_Tmp1B=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp1C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp1D=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp1D->tag=2,_Tmp1D->f1=0,_Tmp1D->f2=& Cyc_Kinds_ak;_Tmp1D;});_Tmp1B->v=_Tmp1C;});_Tmp1B;});*f=_Tmp1A;});goto _LL98;}}_LL84:;}}}}
# 1165
{struct Cyc_List_List*ts=new_env.free_evars;for(0;ts!=0;ts=ts->tl){
struct _tuple14 _Tmp15=*((struct _tuple14*)ts->hd);int _Tmp16;void*_Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{void*tv=_Tmp17;int put_in_eff=_Tmp16;
if(!put_in_eff)continue;{
struct Cyc_Absyn_Kind*_Tmp18=Cyc_Tcutil_type_kind(tv);switch((int)((struct Cyc_Absyn_Kind*)_Tmp18)->kind){case Cyc_Absyn_EffKind:
# 1170
 effect=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=tv,_Tmp19->tl=effect;_Tmp19;});goto _LL9C;case Cyc_Absyn_IntKind:
 goto _LL9C;default:
 effect=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1A=Cyc_Absyn_regionsof_eff(tv);_Tmp19->hd=_Tmp1A;}),_Tmp19->tl=effect;_Tmp19;});goto _LL9C;}_LL9C:;}}}}
# 1175
({void*_Tmp15=Cyc_Absyn_join_eff(effect);*eff=_Tmp15;});}
# 1181
if(*btvs!=0){
struct Cyc_List_List*bs=*btvs;for(0;bs!=0;bs=bs->tl){
void*_Tmp15=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_Tmp16;void*_Tmp17;switch(*((int*)_Tmp15)){case 1: _Tmp17=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp15)->f1;{struct Cyc_Core_Opt**f=_Tmp17;
# 1185
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp19;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=7,_Tmp1A.f1=(struct Cyc_Absyn_Tvar*)bs->hd;_Tmp1A;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=_tag_fat(" unconstrained, assuming boxed",sizeof(char),31U);_Tmp1B;});void*_Tmp1B[3];_Tmp1B[0]=& _Tmp18,_Tmp1B[1]=& _Tmp19,_Tmp1B[2]=& _Tmp1A;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1B,sizeof(void*),3));});
({struct Cyc_Core_Opt*_Tmp18=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp18;});
goto _LLA3;}case 2: _Tmp17=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp15)->f1;_Tmp16=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp15)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp17;struct Cyc_Absyn_Kind*k=_Tmp16;
# 1189
switch((int)((struct Cyc_Absyn_Kind*)k)->kind){case Cyc_Absyn_BoxKind:
 goto _LLB0;case Cyc_Absyn_MemKind: _LLB0:
 goto _LLB2;case Cyc_Absyn_AnyKind: _LLB2:
# 1193
({struct Cyc_Core_Opt*_Tmp18=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp18;});goto _LLAC;case Cyc_Absyn_EffKind:
# 1195
({struct Cyc_Core_Opt*_Tmp18=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_ek);*f=_Tmp18;});goto _LLAC;default:
# 1197
({struct Cyc_Core_Opt*_Tmp18=Cyc_Kinds_kind_to_bound_opt(k);*f=_Tmp18;});goto _LLAC;}_LLAC:;
# 1199
goto _LLA3;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp15)->f1)->kind==Cyc_Absyn_MemKind){
# 1201
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=_tag_fat("functions can't abstract M types",sizeof(char),33U);_Tmp19;});void*_Tmp19[1];_Tmp19[0]=& _Tmp18;Cyc_Warn_err2(loc,_tag_fat(_Tmp19,sizeof(void*),1));});goto _LLA3;}else{
goto _LLA3;}}_LLA3:;}}
# 1205
({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*(*_Tmp16)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars;_Tmp16;})(new_env.kind_env,*btvs,({struct Cyc_Absyn_Tvar*(*_Tmp16)(struct Cyc_Absyn_Tvar*)=(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))Cyc_Core_identity;_Tmp16;}));env.kind_env=_Tmp15;});
({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*(*_Tmp16)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_Tmp16;})(new_env.free_vars,*btvs,({struct Cyc_Absyn_Tvar*(*_Tmp16)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp16;}));new_env.free_vars=_Tmp15;});
# 1208
{struct Cyc_List_List*tvs=new_env.free_vars;for(0;tvs!=0;tvs=tvs->tl){
struct _tuple12 _Tmp15=*((struct _tuple12*)tvs->hd);int _Tmp16;void*_Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{struct Cyc_Absyn_Tvar*t=_Tmp17;int b=_Tmp16;
({struct Cyc_List_List*_Tmp18=Cyc_Tctyp_fast_add_free_tvar_bool(env.free_vars,t,b);env.free_vars=_Tmp18;});}}}
# 1213
{struct Cyc_List_List*evs=new_env.free_evars;for(0;evs!=0;evs=evs->tl){
struct _tuple14 _Tmp15=*((struct _tuple14*)evs->hd);int _Tmp16;void*_Tmp17;_Tmp17=_Tmp15.f0;_Tmp16=_Tmp15.f1;{void*e=_Tmp17;int b=_Tmp16;
({struct Cyc_List_List*_Tmp18=Cyc_Tctyp_add_free_evar(env.free_evars,e,b);env.free_evars=_Tmp18;});}}}
# 1217
if((unsigned)env.free_qualvars){
# 1219
struct Cyc_List_List*it=new_env.free_qualvars;for(0;it!=0;it=it->tl){
if(!({int(*_Tmp15)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*)=(int(*)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*))Cyc_List_exists_c;_Tmp15;})(Cyc_Tctyp_cmp_fqv,(struct _tuple14*)it->hd,env.free_qualvars))
({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));_Tmp16->hd=(struct _tuple14*)it->hd,_Tmp16->tl=env.free_qualvars;_Tmp16;});env.free_qualvars=_Tmp15;});}}else{
# 1225
env.free_qualvars=new_env.free_qualvars;}
goto _LL3;}}}case 6: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f1;_TmpB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f2;_Tmp14=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp3;int is_tuple=_TmpB;struct Cyc_List_List*fs=_Tmp14;
# 1230
struct Cyc_List_List*prev_fields=0;
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_Tmp15=(struct Cyc_Absyn_Aggrfield*)fs->hd;void*_Tmp16;void*_Tmp17;void*_Tmp18;void*_Tmp19;void*_Tmp1A;void*_Tmp1B;_Tmp1B=_Tmp15->name;_Tmp1A=(struct Cyc_Absyn_Tqual*)& _Tmp15->tq;_Tmp19=_Tmp15->type;_Tmp18=_Tmp15->width;_Tmp17=_Tmp15->attributes;_Tmp16=_Tmp15->requires_clause;{struct _fat_ptr*fn=_Tmp1B;struct Cyc_Absyn_Tqual*tqp=(struct Cyc_Absyn_Tqual*)_Tmp1A;void*t=_Tmp19;struct Cyc_Absyn_Exp*width=_Tmp18;struct Cyc_List_List*atts=_Tmp17;struct Cyc_Absyn_Exp*requires_clause=_Tmp16;
if(({int(*_Tmp1C)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp1C;})(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("duplicate field ",sizeof(char),17U);_Tmp1D;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=*fn;_Tmp1E;});void*_Tmp1E[2];_Tmp1E[0]=& _Tmp1C,_Tmp1E[1]=& _Tmp1D;Cyc_Warn_err2(loc,_tag_fat(_Tmp1E,sizeof(void*),2));});
if(Cyc_strcmp(*fn,_tag_fat("",sizeof(char),1U))!=0)
prev_fields=({struct Cyc_List_List*_Tmp1C=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1C->hd=fn,_Tmp1C->tl=prev_fields;_Tmp1C;});
env=Cyc_Tctyp_i_check_type(env,& Cyc_Kinds_mk,t,1,0);
({int _Tmp1C=Cyc_Tcutil_extract_const_from_typedef(loc,tqp->print_const,t);tqp->real_const=_Tmp1C;});
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause!=0){
# 1243
if((int)k!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("@requires clause is only allowed on union members",sizeof(char),50U);_Tmp1D;});void*_Tmp1D[1];_Tmp1D[0]=& _Tmp1C;Cyc_Warn_err2(loc,_tag_fat(_Tmp1D,sizeof(void*),1));});
({struct Cyc_Tcenv_Tenv*_Tmp1C=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_Tmp1C,0,requires_clause);});
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("@requires clause has type ",sizeof(char),27U);_Tmp1D;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=3,_Tmp1E.f1=(void*)requires_clause->topt;_Tmp1E;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=0,_Tmp1F.f1=
_tag_fat(" instead of integral type",sizeof(char),26U);_Tmp1F;});void*_Tmp1F[3];_Tmp1F[0]=& _Tmp1C,_Tmp1F[1]=& _Tmp1D,_Tmp1F[2]=& _Tmp1E;Cyc_Warn_err2(loc,_tag_fat(_Tmp1F,sizeof(void*),3));});
env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);}}}
# 1252
goto _LL3;}default: _Tmp14=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f1;_Tmp13=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f2;_Tmp12=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f3;_Tmp11=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp2)->f4;{struct _tuple1*tdn=_Tmp14;struct Cyc_List_List**targs_ref=(struct Cyc_List_List**)_Tmp13;struct Cyc_Absyn_Typedefdecl**tdopt=(struct Cyc_Absyn_Typedefdecl**)_Tmp12;void**toptp=(void**)_Tmp11;
# 1256
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _Tmp15;_push_handler(& _Tmp15);{int _Tmp16=0;if(setjmp(_Tmp15.handler))_Tmp16=1;if(!_Tmp16){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);;_pop_handler();}else{void*_Tmp17=(void*)Cyc_Core_get_exn_thrown();void*_Tmp18;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp17)->tag==Cyc_Dict_Absent){
# 1260
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("unbound typedef name ",sizeof(char),22U);_Tmp1A;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=1,_Tmp1B.f1=tdn;_Tmp1B;});void*_Tmp1B[2];_Tmp1B[0]=& _Tmp19,_Tmp1B[1]=& _Tmp1A;Cyc_Warn_err2(loc,_tag_fat(_Tmp1B,sizeof(void*),2));});
return env;}else{_Tmp18=_Tmp17;{void*exn=_Tmp18;_rethrow(exn);}};}}}
# 1263
*tdopt=td;
env=Cyc_Tctyp_check_type_inst(env,targs_ref,td->tvs,expected_kind,allow_abs_aggr,tdn,td);
# 1266
if(td->defn!=0){
struct Cyc_List_List*inst=0;
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=*targs_ref;
for(1;tvs!=0;(tvs=tvs->tl,ts=ts->tl)){
inst=({struct Cyc_List_List*_Tmp15=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp16=({struct _tuple17*_Tmp17=_cycalloc(sizeof(struct _tuple17));_Tmp17->f0=(struct Cyc_Absyn_Tvar*)tvs->hd,_Tmp17->f1=(void*)_check_null(ts)->hd;_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=inst;_Tmp15;});}
({void*_Tmp15=Cyc_Tcutil_substitute(inst,_check_null(td->defn));*toptp=_Tmp15;});}
# 1274
goto _LL3;}}_LL3:;}
# 1276
if(!({struct Cyc_Absyn_Kind*_Tmp2=Cyc_Tcutil_type_kind(t);Cyc_Kinds_kind_leq(_Tmp2,expected_kind);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("type ",sizeof(char),6U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" has kind ",sizeof(char),11U);_Tmp5;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp6;_Tmp6.tag=9,({struct Cyc_Absyn_Kind*_Tmp7=Cyc_Tcutil_type_kind(t);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" but as used here needs kind ",sizeof(char),30U);_Tmp7;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp8;_Tmp8.tag=9,_Tmp8.f1=expected_kind;_Tmp8;});void*_Tmp8[6];_Tmp8[0]=& _Tmp2,_Tmp8[1]=& _Tmp3,_Tmp8[2]=& _Tmp4,_Tmp8[3]=& _Tmp5,_Tmp8[4]=& _Tmp6,_Tmp8[5]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),6));});
return env;}}
# 1282
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 1284
struct Cyc_List_List*kind_env=cvt.kind_env;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);
# 1287
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs!=0;vs=vs->tl){
({struct Cyc_List_List*_Tmp0=Cyc_Tctyp_fast_add_free_tvar(kind_env,(*((struct _tuple12*)vs->hd)).f0);cvt.kind_env=_Tmp0;});}}
# 1293
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs!=0;evs=evs->tl){
struct _tuple14 _Tmp0=*((struct _tuple14*)evs->hd);void*_Tmp1;_Tmp1=_Tmp0.f0;{void*e=_Tmp1;
void*_Tmp2=Cyc_Absyn_compress(e);void*_Tmp3;if(*((int*)_Tmp2)==1){_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp2)->f4;{struct Cyc_Core_Opt**s=(struct Cyc_Core_Opt**)_Tmp3;
# 1297
if(*s==0)
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=kind_env;_Tmp5;});*s=_Tmp4;});else{
# 1301
struct Cyc_List_List*result=0;
{struct Cyc_List_List*tvs=(struct Cyc_List_List*)(*s)->v;for(0;tvs!=0;tvs=tvs->tl){
if(({int(*_Tmp4)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;_Tmp4;})(Cyc_Tcutil_fast_tvar_cmp,kind_env,(struct Cyc_Absyn_Tvar*)tvs->hd))
result=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_Tmp4->tl=result;_Tmp4;});}}
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=result;_Tmp5;});*s=_Tmp4;});}
# 1307
goto _LL3;}}else{
goto _LL3;}_LL3:;}}}
# 1311
return cvt;}
# 1318
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=
Cyc_Tcutil_is_function_type(t)&&(!te->in_tempest || te->tempest_generalize);
struct Cyc_Absyn_Kind*expected_kind=Cyc_Tcutil_is_function_type(t)?& Cyc_Kinds_ak:& Cyc_Kinds_mk;
struct Cyc_List_List*te_vars=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Tctyp_CVTEnv cvt=({struct Cyc_Tctyp_CVTEnv _Tmp0=({struct Cyc_Tctyp_CVTEnv _Tmp1;_Tmp1.loc=loc,_Tmp1.te=te,_Tmp1.kind_env=te_vars,_Tmp1.fn_result=0,_Tmp1.def_aliasable=0,_Tmp1.inst_type_evar=0,_Tmp1.generalize_evars=generalize_evars,_Tmp1.allow_aqual_bounds=1,_Tmp1.free_vars=0,_Tmp1.free_evars=0,_Tmp1.free_qualvars=0;_Tmp1;});struct Cyc_Absyn_Kind*_Tmp1=expected_kind;Cyc_Tctyp_check_valid_type(_Tmp0,_Tmp1,1,t);});
# 1326
struct Cyc_List_List*free_tvars=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({struct Cyc_Absyn_Tvar*(*_Tmp0)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp0;}),cvt.free_vars);
struct Cyc_List_List*free_evars=cvt.free_evars;
struct Cyc_List_List*free_qualvars=cvt.free_qualvars;
# 1330
if(te_vars!=0){
struct Cyc_List_List*res=0;
{struct Cyc_List_List*fs=free_tvars;for(0;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*ts=te_vars;
for(1;ts!=0;ts=ts->tl){
if(Cyc_Absyn_tvar_cmp((struct Cyc_Absyn_Tvar*)fs->hd,(struct Cyc_Absyn_Tvar*)ts->hd)==0)
break;}
if(ts==0)
res=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct Cyc_Absyn_Tvar*)fs->hd,_Tmp0->tl=res;_Tmp0;});}}
# 1340
free_tvars=Cyc_List_imp_rev(res);}
# 1345
{struct Cyc_List_List*x=free_tvars;for(0;x!=0;x=x->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);enum Cyc_Absyn_AliasHint _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp4;});goto _LL0;}case 2: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp3;struct Cyc_Absyn_Kind*k=_Tmp2;
# 1349
switch((int)((struct Cyc_Absyn_Kind*)k)->kind){case Cyc_Absyn_BoxKind:
 goto _LLD;case Cyc_Absyn_MemKind: _LLD:
# 1352
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(& Cyc_Kinds_bk);*f=_Tmp4;});goto _LL9;default:
# 1354
({struct Cyc_Core_Opt*_Tmp4=Cyc_Kinds_kind_to_bound_opt(k);*f=_Tmp4;});goto _LL9;}_LL9:;
# 1356
goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_MemKind){_Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliashint;{enum Cyc_Absyn_AliasHint a=_Tmp1;
# 1358
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp5;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=7,_Tmp6.f1=(struct Cyc_Absyn_Tvar*)x->hd;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" cannot have kind ",sizeof(char),19U);_Tmp7;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp8;_Tmp8.tag=9,({struct Cyc_Absyn_Kind*_Tmp9=({struct Cyc_Absyn_Kind*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpA->kind=1U,_TmpA->aliashint=a;_TmpA;});_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),4));});
goto _LL0;}}else{
goto _LL0;}}_LL0:;}}
# 1364
if((free_tvars!=0 || free_evars!=0)|| free_qualvars!=0){
{void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==5){_Tmp2=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.qual_bnd;{struct Cyc_List_List**btvs=(struct Cyc_List_List**)_Tmp2;struct Cyc_List_List**qb=(struct Cyc_List_List**)_Tmp1;
# 1368
({unsigned _Tmp3=loc;struct Cyc_Tctyp_CVTEnv _Tmp4=cvt;struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*(*_Tmp6)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_Tcutil_tvar_bool_pair,0,*btvs);struct Cyc_List_List**_Tmp6=qb;Cyc_Tctyp_update_qualifier_bounds(_Tmp3,_Tmp4,_Tmp5,_Tmp6,Cyc_Absyn_al_qual_type);});
# 1370
if(*btvs==0){
# 1372
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_filter;_Tmp4;})(Cyc_CurRgn_is_not_curr_rgn,free_tvars);*btvs=_Tmp3;});
free_tvars=0;}
# 1375
goto _LL10;}}else{
goto _LL10;}_LL10:;}
# 1378
if(free_tvars!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=(struct Cyc_Absyn_Tvar*)free_tvars->hd;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" when checking ",sizeof(char),16U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
if(!Cyc_Tcutil_is_function_type(t)|| !te->in_tempest)
Cyc_Tctyp_check_free_evars(free_evars,t,loc);}{
# 1384
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.tvars;{struct Cyc_List_List*btvs=_Tmp1;
# 1386
if(({int(*_Tmp2)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_exists;_Tmp2;})(Cyc_CurRgn_is_curr_rgn,btvs))
# 1388
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("cannot abstract current region ",sizeof(char),32U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_CurRgn_curr_rgn_name;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" in ",sizeof(char),5U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),4));});
goto _LL15;}}else{
goto _LL15;}_LL15:;}}
# 1398
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1401
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
{void*_Tmp0=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp1;
# 1404
struct Cyc_List_List*atts=fd->i.attributes;
fd->i=i;
fd->i.attributes=atts;
({int _Tmp2=
Cyc_Tcutil_extract_const_from_typedef(loc,i.ret_tqual.print_const,i.ret_type);
# 1407
fd->i.ret_tqual.real_const=_Tmp2;});
# 1409
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("check_fndecl_valid_type: not a FnType",sizeof(char),38U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}{
# 1412
struct Cyc_List_List*argnames=0;
{struct Cyc_List_List*args=fd->i.args;for(0;args!=0;args=args->tl){
argnames=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=_check_null((*((struct _tuple9*)args->hd)).f0),_Tmp0->tl=argnames;_Tmp0;});}}
Cyc_Tcutil_check_unique_vars(argnames,loc,_tag_fat("function declaration has repeated parameter",sizeof(char),44U));
# 1417
fd->cached_type=t;}}
# 1425
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1428
struct Cyc_Tctyp_CVTEnv cvt=({struct Cyc_Tctyp_CVTEnv _Tmp0=({struct Cyc_Tctyp_CVTEnv _Tmp1;_Tmp1.loc=loc,_Tmp1.te=te,_Tmp1.kind_env=bound_tvars,_Tmp1.fn_result=0,_Tmp1.def_aliasable=0,_Tmp1.inst_type_evar=0,_Tmp1.generalize_evars=0,_Tmp1.allow_aqual_bounds=0,_Tmp1.free_vars=0,_Tmp1.free_evars=0,_Tmp1.free_qualvars=0;_Tmp1;});struct Cyc_Absyn_Kind*_Tmp1=expected_kind;int _Tmp2=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_Tmp0,_Tmp1,_Tmp2,t);});
# 1430
struct Cyc_List_List*free_tvs=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_Tmp0;})(cvt.free_vars,bound_tvars,({struct Cyc_Absyn_Tvar*(*_Tmp0)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp0;}));
for(1;free_tvs!=0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=(*((struct _tuple12*)free_tvs->hd)).f0;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" in type ",sizeof(char),10U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);}
# 1437
void Cyc_Tctyp_check_type_with_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*flagged_tvars,struct Cyc_List_List**qual_bnd,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 1441
struct Cyc_List_List*bound_tvars=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({struct Cyc_Absyn_Tvar*(*_Tmp0)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp0;}),flagged_tvars);
struct Cyc_Tctyp_CVTEnv cvt=({struct Cyc_Tctyp_CVTEnv _Tmp0=({struct Cyc_Tctyp_CVTEnv _Tmp1;_Tmp1.loc=loc,_Tmp1.te=te,_Tmp1.kind_env=bound_tvars,_Tmp1.fn_result=0,_Tmp1.def_aliasable=0,_Tmp1.inst_type_evar=0,_Tmp1.generalize_evars=0,_Tmp1.allow_aqual_bounds=1,_Tmp1.free_vars=0,_Tmp1.free_evars=0,_Tmp1.free_qualvars=0;_Tmp1;});struct Cyc_Absyn_Kind*_Tmp1=expected_kind;int _Tmp2=allow_abs_aggr;Cyc_Tctyp_check_valid_type(_Tmp0,_Tmp1,_Tmp2,t);});
# 1446
{struct Cyc_List_List*qbit=*qual_bnd;for(0;qbit!=0;qbit=qbit->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)qbit->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*tv=_Tmp2;void*bnd=_Tmp1;
cvt=Cyc_Tctyp_check_valid_type(cvt,& Cyc_Kinds_aqk,allow_abs_aggr,tv);
cvt=Cyc_Tctyp_check_valid_type(cvt,& Cyc_Kinds_aqk,allow_abs_aggr,bnd);}}}{
# 1452
struct Cyc_List_List*free_tvs=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_Tmp0;})(cvt.free_vars,bound_tvars,({struct Cyc_Absyn_Tvar*(*_Tmp0)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp0;}));
for(1;free_tvs!=0;free_tvs=free_tvs->tl){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=(*((struct _tuple12*)free_tvs->hd)).f0;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" in type ",sizeof(char),10U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}
if(!allow_evars)
Cyc_Tctyp_check_free_evars(cvt.free_evars,t,loc);
Cyc_Tctyp_update_qualifier_bounds(loc,cvt,flagged_tvars,qual_bnd,Cyc_Absyn_rtd_qual_type);}}
