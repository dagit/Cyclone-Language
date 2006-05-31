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
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 313
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 326
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 333
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 445 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 921
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 939
void*Cyc_Absyn_compress(void*);
# 943
int Cyc_Absyn_type2bool(int,void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 968
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 972
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);
# 999
struct _tuple1*Cyc_Absyn_datatype_print_arg_qvar (void);
struct _tuple1*Cyc_Absyn_datatype_scanf_arg_qvar (void);
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1012
void*Cyc_Absyn_fatconst (void);
void*Cyc_Absyn_thinconst (void);
# 1042
void*Cyc_Absyn_aqualsof_type(void*);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1242
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 31 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
# 39
int Cyc_Tcutil_is_function_type(void*);
# 42
int Cyc_Tcutil_is_array_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 82
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 85
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 92
void*Cyc_Tcutil_copy_type(void*);
# 99
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 101
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 131
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 169
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 171
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 212
void*Cyc_Tcutil_normalize_effect(void*);
# 215
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 217
int Cyc_Tcutil_new_tvar_id (void);
# 226
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 232
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 235
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 246
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 261
int Cyc_Tcutil_admits_zero(void*);
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 269
void*Cyc_Tcutil_lookup_aquals_opt(struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_Absyn_Tvar*f0;int f1;};
# 273
struct _tuple12*Cyc_Tcutil_tvar_bool_pair(int,struct Cyc_Absyn_Tvar*);
# 275
int Cyc_Tcutil_type_in_effect(int,void*,void*);
# 277
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
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
# 72 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*,struct Cyc_List_List*);
# 76
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 79
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 36 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_Absyn_Enumdecl*);
# 54 "warn.h"
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 98 "flags.h"
extern int Cyc_Flags_tovc_r;
# 29 "currgn.h"
extern struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 35
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*);
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*);struct Cyc_Set_Set;
# 151 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_pre_memory;struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 277 "assndef.h"
extern struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_assnmap2existassnfn(struct Cyc_AssnDef_AssnMap,struct Cyc_List_List*);
# 287
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void);
# 45 "vcgen.h"
extern struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
# 28
int Cyc_BansheeIf_add_constraint(unsigned,void*);struct Cyc_Tctyp_CVTEnv{unsigned loc;struct Cyc_Tcenv_Tenv*te;struct Cyc_List_List*kind_env;int fn_result;int def_aliasable;int inst_type_evar;int generalize_evars;int allow_aqual_bounds;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;struct Cyc_List_List*free_qualvars;};
# 91 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv,struct Cyc_Absyn_Kind*,void*,int,int);struct _tuple14{void*f0;int f1;};
# 97
static struct Cyc_List_List*Cyc_Tctyp_add_free_evar(struct Cyc_List_List*es,void*e,int b){int*_T0;int _T1;struct Cyc_List_List*_T2;void*_T3;int*_T4;int*_T5;int _T6;int _T7;int*_T8;int _T9;int*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct _tuple14*_TE;struct Cyc_List_List*_TF;
void*_T10=Cyc_Absyn_compress(e);int _T11;_T0=(int*)_T10;_T1=*_T0;if(_T1!=1)goto _TL0;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T12=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T10;_T11=_T12->f3;}{int i=_T11;{
# 100
struct Cyc_List_List*es2=es;_TL5: if(es2!=0)goto _TL3;else{goto _TL4;}
_TL3: _T2=es2;_T3=_T2->hd;{struct _tuple14*_T12=(struct _tuple14*)_T3;void*_T13;void*_T14;{struct _tuple14 _T15=*_T12;_T14=_T15.f0;_T4=& _T12->f1;_T13=(int*)_T4;}{void*t=_T14;int*b2=(int*)_T13;
void*_T15=Cyc_Absyn_compress(t);int _T16;_T5=(int*)_T15;_T6=*_T5;if(_T6!=1)goto _TL6;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T17=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T15;_T16=_T17->f3;}{int j=_T16;
# 104
if(i!=j)goto _TL8;_T7=b;_T8=b2;_T9=*_T8;
if(_T7==_T9)goto _TLA;_TA=b2;*_TA=1;goto _TLB;_TLA: _TLB: _TB=es;
return _TB;_TL8: goto _LL8;}_TL6: goto _LL8;_LL8:;}}_TC=es2;
# 100
es2=_TC->tl;goto _TL5;_TL4:;}{struct Cyc_List_List*_T12=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple14*_T13=_cycalloc(sizeof(struct _tuple14));
# 112
_T13->f0=e;_T13->f1=b;_TE=(struct _tuple14*)_T13;}_T12->hd=_TE;_T12->tl=es;_TD=(struct Cyc_List_List*)_T12;}return _TD;}_TL0: _TF=es;
return _TF;;}
# 118
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;struct Cyc_Warn_Tvar_Warn_Warg_struct _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;void**_T5;void**_T6;void**_T7;int(*_T8)(struct _fat_ptr);void*(*_T9)(struct _fat_ptr);struct _fat_ptr _TA;struct Cyc_List_List*_TB;_T0=tv;_T1=_T0->identity;
if(_T1!=-1)goto _TLC;{struct Cyc_Warn_String_Warn_Warg_struct _TC;_TC.tag=0;
_TC.f1=_tag_fat("bound tvar id for ",sizeof(char),19U);_T2=_TC;}{struct Cyc_Warn_String_Warn_Warg_struct _TC=_T2;{struct Cyc_Warn_Tvar_Warn_Warg_struct _TD;_TD.tag=7;_TD.f1=tv;_T3=_TD;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _TD=_T3;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;_TE.f1=_tag_fat(" is NULL",sizeof(char),9U);_T4=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T4;void*_TF[3];_T5=_TF + 0;*_T5=& _TC;_T6=_TF + 1;*_T6=& _TD;_T7=_TF + 2;*_T7=& _TE;_T9=Cyc_Warn_impos2;{int(*_T10)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T9;_T8=_T10;}_TA=_tag_fat(_TF,sizeof(void*),3);_T8(_TA);}}}goto _TLD;_TLC: _TLD:{struct Cyc_List_List*_TC=_cycalloc(sizeof(struct Cyc_List_List));
_TC->hd=tv;_TC->tl=tvs;_TB=(struct Cyc_List_List*)_TC;}return _TB;}
# 123
static struct Cyc_List_List*Cyc_Tctyp_remove_bound_tvars(struct Cyc_List_List*tvs,struct Cyc_List_List*btvs,struct Cyc_Absyn_Tvar*(*get)(void*)){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;int _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_Absyn_Tvar*_T6;int _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;
# 125
struct Cyc_List_List*r=0;
_TL11: if(tvs!=0)goto _TLF;else{goto _TL10;}
_TLF:{struct Cyc_List_List*b=btvs;
_TL15: if(b!=0)goto _TL13;else{goto _TL14;}
_TL13: _T0=tvs;_T1=_T0->hd;_T2=get(_T1);_T3=_T2->identity;_T4=b;_T5=_T4->hd;_T6=(struct Cyc_Absyn_Tvar*)_T5;_T7=_T6->identity;if(_T3!=_T7)goto _TL16;goto _TL14;_TL16: _T8=b;
# 128
b=_T8->tl;goto _TL15;_TL14:
# 131
 if(b!=0)goto _TL18;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));_TA=tvs;
_TD->hd=_TA->hd;_TD->tl=r;_T9=(struct Cyc_List_List*)_TD;}r=_T9;goto _TL19;_TL18: _TL19:;}_TB=tvs;
# 126
tvs=_TB->tl;goto _TL11;_TL10:
# 134
 r=Cyc_List_imp_rev(r);_TC=r;
return _TC;}
# 142
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){struct Cyc_List_List*_T0;void*_T1;struct _tuple14*_T2;struct _tuple14 _T3;int*_T4;int _T5;struct Cyc_Absyn_Kind*_T6;enum Cyc_Absyn_KindQual _T7;int _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;struct Cyc_Warn_Typ_Warn_Warg_struct _TC;void**_TD;void**_TE;void**_TF;void**_T10;unsigned _T11;struct _fat_ptr _T12;int _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Typ_Warn_Warg_struct _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;void**_T19;void**_T1A;void**_T1B;void**_T1C;void**_T1D;int(*_T1E)(struct _fat_ptr);void*(*_T1F)(struct _fat_ptr);struct _fat_ptr _T20;struct Cyc_List_List*_T21;
_TL1D: if(free_evars!=0)goto _TL1B;else{goto _TL1C;}
_TL1B: _T0=free_evars;_T1=_T0->hd;_T2=(struct _tuple14*)_T1;_T3=*_T2;{void*e=_T3.f0;{
void*_T22=Cyc_Absyn_compress(e);_T4=(int*)_T22;_T5=*_T4;if(_T5!=1)goto _TL1E;goto _LL0;_TL1E: goto _TL1A;_LL0:;}{
# 149
void*t;
struct _fat_ptr s;{
struct Cyc_Absyn_Kind*_T22=Cyc_Tcutil_type_kind(e);_T6=(struct Cyc_Absyn_Kind*)_T22;_T7=_T6->kind;_T8=(int)_T7;switch(_T8){case Cyc_Absyn_EffKind:
# 153
 t=Cyc_Absyn_heap_rgn_type;s=_tag_fat("{`H}",sizeof(char),5U);goto _LL5;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_false_type;s=_tag_fat("@false",sizeof(char),7U);goto _LL5;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_bounds_one();s=_tag_fat("bounds_one",sizeof(char),11U);goto _LL5;case Cyc_Absyn_AqualKind:
 t=Cyc_Absyn_al_qual_type;s=_tag_fat("aliasable",sizeof(char),10U);goto _LL5;default:{struct Cyc_Warn_String_Warn_Warg_struct _T23;_T23.tag=0;
# 158
_T23.f1=_tag_fat("hidden type variable ",sizeof(char),22U);_T9=_T23;}{struct Cyc_Warn_String_Warn_Warg_struct _T23=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T24;_T24.tag=2;_T24.f1=e;_TA=_T24;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T24=_TA;{struct Cyc_Warn_String_Warn_Warg_struct _T25;_T25.tag=0;_T25.f1=_tag_fat(" isn't abstracted in type ",sizeof(char),27U);_TB=_T25;}{struct Cyc_Warn_String_Warn_Warg_struct _T25=_TB;{struct Cyc_Warn_Typ_Warn_Warg_struct _T26;_T26.tag=2;_T26.f1=in_typ;_TC=_T26;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T26=_TC;void*_T27[4];_TD=_T27 + 0;*_TD=& _T23;_TE=_T27 + 1;*_TE=& _T24;_TF=_T27 + 2;*_TF=& _T25;_T10=_T27 + 3;*_T10=& _T26;_T11=loc;_T12=_tag_fat(_T27,sizeof(void*),4);Cyc_Warn_err2(_T11,_T12);}}}}
return;}_LL5:;}_T13=
# 161
Cyc_Unify_unify(e,t);if(_T13)goto _TL21;else{goto _TL23;}
_TL23:{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;_T22.f1=_tag_fat("can't unify evar ",sizeof(char),18U);_T14=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T14;{struct Cyc_Warn_Typ_Warn_Warg_struct _T23;_T23.tag=2;_T23.f1=e;_T15=_T23;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T23=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=_tag_fat(" with ",sizeof(char),7U);_T16=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T16;{struct Cyc_Warn_String_Warn_Warg_struct _T25;_T25.tag=0;_T25.f1=s;_T17=_T25;}{struct Cyc_Warn_String_Warn_Warg_struct _T25=_T17;{struct Cyc_Warn_String_Warn_Warg_struct _T26;_T26.tag=0;_T26.f1=_tag_fat("!",sizeof(char),2U);_T18=_T26;}{struct Cyc_Warn_String_Warn_Warg_struct _T26=_T18;void*_T27[5];_T19=_T27 + 0;*_T19=& _T22;_T1A=_T27 + 1;*_T1A=& _T23;_T1B=_T27 + 2;*_T1B=& _T24;_T1C=_T27 + 3;*_T1C=& _T25;_T1D=_T27 + 4;*_T1D=& _T26;_T1F=Cyc_Warn_impos2;{int(*_T28)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1F;_T1E=_T28;}_T20=_tag_fat(_T27,sizeof(void*),5);_T1E(_T20);}}}}}goto _TL22;_TL21: _TL22:;}}_TL1A: _T21=free_evars;
# 143
free_evars=_T21->tl;goto _TL1D;_TL1C:;}
# 172
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int allow_aqual_bounds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;struct _fat_ptr*_T3;struct Cyc_Absyn_Tvar*_T4;struct _fat_ptr*_T5;int _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;struct Cyc_Absyn_Tvar*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_Tvar_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_KindBound_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_KindBound_Warn_Warg_struct _T11;void**_T12;void**_T13;void**_T14;void**_T15;void**_T16;void**_T17;unsigned _T18;struct _fat_ptr _T19;struct Cyc_Absyn_Tvar*_T1A;void*_T1B;int _T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;void**_T20;void**_T21;void**_T22;unsigned _T23;struct _fat_ptr _T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_Absyn_Tvar*_T27;void*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Tvar*_T2B;struct Cyc_Absyn_Tvar*_T2C;struct Cyc_Absyn_Tvar*_T2D;void*_T2E;struct Cyc_Absyn_Tvar*_T2F;void*_T30;struct Cyc_List_List*_T31;void*_T32;struct Cyc_Absyn_Tvar*_T33;void*_T34;int _T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Tvar_Warn_Warg_struct _T37;struct Cyc_Warn_String_Warn_Warg_struct _T38;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T39;struct Cyc_Absyn_Tvar*_T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3C;struct Cyc_List_List*_T3D;void*_T3E;struct Cyc_Absyn_Tvar*_T3F;void**_T40;void**_T41;void**_T42;void**_T43;void**_T44;void**_T45;unsigned _T46;struct _fat_ptr _T47;struct Cyc_Absyn_Tvar*_T48;int _T49;struct Cyc_Absyn_Tvar*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_Absyn_Tvar*_T4D;struct Cyc_Absyn_Tvar*_T4E;int _T4F;struct Cyc_List_List*_T50;void*_T51;struct Cyc_Absyn_Tvar*_T52;int _T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;void**_T55;int(*_T56)(struct _fat_ptr);void*(*_T57)(struct _fat_ptr);struct _fat_ptr _T58;struct Cyc_List_List*_T59;struct Cyc_List_List*_T5A;struct Cyc_Absyn_Tvar*_T5B;struct Cyc_List_List*_T5C;{
struct Cyc_List_List*tvs2=tvs;_TL27: if(tvs2!=0)goto _TL25;else{goto _TL26;}
_TL25: _T0=tvs2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Tvar*)_T1;_T3=_T2->name;_T4=tv;_T5=_T4->name;_T6=Cyc_strptrcmp(_T3,_T5);if(_T6!=0)goto _TL28;_T7=tvs2;_T8=_T7->hd;_T9=(struct Cyc_Absyn_Tvar*)_T8;{
void*k1=_T9->kind;_TA=tv;{
void*k2=_TA->kind;_TB=
Cyc_Kinds_constrain_kinds(k1,k2);if(_TB)goto _TL2A;else{goto _TL2C;}
_TL2C:{struct Cyc_Warn_String_Warn_Warg_struct _T5D;_T5D.tag=0;_T5D.f1=_tag_fat("type variable ",sizeof(char),15U);_TC=_T5D;}{struct Cyc_Warn_String_Warn_Warg_struct _T5D=_TC;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E;_T5E.tag=7;_T5E.f1=tv;_TD=_T5E;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T5F;_T5F.tag=0;_T5F.f1=_tag_fat(" used with inconsistent kinds ",sizeof(char),31U);_TE=_T5F;}{struct Cyc_Warn_String_Warn_Warg_struct _T5F=_TE;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T60;_T60.tag=8;
_T60.f1=k1;_TF=_T60;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T60=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T61;_T61.tag=0;_T61.f1=_tag_fat(" and ",sizeof(char),6U);_T10=_T61;}{struct Cyc_Warn_String_Warn_Warg_struct _T61=_T10;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T62;_T62.tag=8;_T62.f1=k2;_T11=_T62;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T62=_T11;void*_T63[6];_T12=_T63 + 0;*_T12=& _T5D;_T13=_T63 + 1;*_T13=& _T5E;_T14=_T63 + 2;*_T14=& _T5F;_T15=_T63 + 3;*_T15=& _T60;_T16=_T63 + 4;*_T16=& _T61;_T17=_T63 + 5;*_T17=& _T62;_T18=loc;_T19=_tag_fat(_T63,sizeof(void*),6);Cyc_Warn_err2(_T18,_T19);}}}}}}goto _TL2B;_TL2A: _TL2B: _T1A=tv;_T1B=_T1A->aquals_bound;
if(_T1B==0)goto _TL2D;_T1C=allow_aqual_bounds;if(_T1C)goto _TL2D;else{goto _TL2F;}
_TL2F:{struct Cyc_Warn_String_Warn_Warg_struct _T5D;_T5D.tag=0;_T5D.f1=_tag_fat("alias bound appears on type variable ",sizeof(char),38U);_T1D=_T5D;}{struct Cyc_Warn_String_Warn_Warg_struct _T5D=_T1D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E;_T5E.tag=7;_T5E.f1=tv;_T1E=_T5E;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E=_T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T5F;_T5F.tag=0;
_T5F.f1=_tag_fat(" which is not in a function declaration or aggregate field",sizeof(char),59U);_T1F=_T5F;}{struct Cyc_Warn_String_Warn_Warg_struct _T5F=_T1F;void*_T60[3];_T20=_T60 + 0;*_T20=& _T5D;_T21=_T60 + 1;*_T21=& _T5E;_T22=_T60 + 2;*_T22=& _T5F;_T23=loc;_T24=_tag_fat(_T60,sizeof(void*),3);Cyc_Warn_err2(_T23,_T24);}}}goto _TL2E;_TL2D: _TL2E: _T25=tvs2;_T26=_T25->hd;_T27=(struct Cyc_Absyn_Tvar*)_T26;_T28=_T27->aquals_bound;
if(_T28!=0)goto _TL30;_T29=tvs2;_T2A=_T29->hd;_T2B=(struct Cyc_Absyn_Tvar*)_T2A;_T2C=tv;
_T2B->aquals_bound=_T2C->aquals_bound;goto _TL31;
_TL30: _T2D=tv;_T2E=_T2D->aquals_bound;if(_T2E==0)goto _TL32;_T2F=tv;_T30=_T2F->aquals_bound;_T31=tvs2;_T32=_T31->hd;_T33=(struct Cyc_Absyn_Tvar*)_T32;_T34=_T33->aquals_bound;_T35=Cyc_Tcutil_typecmp(_T30,_T34);if(!_T35)goto _TL32;{struct Cyc_Warn_String_Warn_Warg_struct _T5D;_T5D.tag=0;
_T5D.f1=_tag_fat("type variable ",sizeof(char),15U);_T36=_T5D;}{struct Cyc_Warn_String_Warn_Warg_struct _T5D=_T36;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E;_T5E.tag=7;_T5E.f1=tv;_T37=_T5E;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T5E=_T37;{struct Cyc_Warn_String_Warn_Warg_struct _T5F;_T5F.tag=0;_T5F.f1=_tag_fat(" used with inconsistent alias qualifier bounds ",sizeof(char),48U);_T38=_T5F;}{struct Cyc_Warn_String_Warn_Warg_struct _T5F=_T38;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T60;_T60.tag=3;_T3A=tv;
_T60.f1=_T3A->aquals_bound;_T39=_T60;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T60=_T39;{struct Cyc_Warn_String_Warn_Warg_struct _T61;_T61.tag=0;_T61.f1=_tag_fat(" and ",sizeof(char),6U);_T3B=_T61;}{struct Cyc_Warn_String_Warn_Warg_struct _T61=_T3B;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T62;_T62.tag=3;_T3D=tvs2;_T3E=_T3D->hd;_T3F=(struct Cyc_Absyn_Tvar*)_T3E;_T62.f1=_T3F->aquals_bound;_T3C=_T62;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T62=_T3C;void*_T63[6];_T40=_T63 + 0;*_T40=& _T5D;_T41=_T63 + 1;*_T41=& _T5E;_T42=_T63 + 2;*_T42=& _T5F;_T43=_T63 + 3;*_T43=& _T60;_T44=_T63 + 4;*_T44=& _T61;_T45=_T63 + 5;*_T45=& _T62;_T46=loc;_T47=_tag_fat(_T63,sizeof(void*),6);Cyc_Warn_err2(_T46,_T47);}}}}}}goto _TL33;_TL32: _TL33: _TL31: _T48=tv;_T49=_T48->identity;
if(_T49!=-1)goto _TL34;_T4A=tv;_T4B=tvs2;_T4C=_T4B->hd;_T4D=(struct Cyc_Absyn_Tvar*)_T4C;
_T4A->identity=_T4D->identity;goto _TL35;
_TL34: _T4E=tv;_T4F=_T4E->identity;_T50=tvs2;_T51=_T50->hd;_T52=(struct Cyc_Absyn_Tvar*)_T51;_T53=_T52->identity;if(_T4F==_T53)goto _TL36;{struct Cyc_Warn_String_Warn_Warg_struct _T5D;_T5D.tag=0;
_T5D.f1=_tag_fat("same type variable has different identity!",sizeof(char),43U);_T54=_T5D;}{struct Cyc_Warn_String_Warn_Warg_struct _T5D=_T54;void*_T5E[1];_T55=_T5E + 0;*_T55=& _T5D;_T57=Cyc_Warn_impos2;{int(*_T5F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T57;_T56=_T5F;}_T58=_tag_fat(_T5E,sizeof(void*),1);_T56(_T58);}goto _TL37;_TL36: _TL37: _TL35: _T59=tvs;
return _T59;}}_TL28: _T5A=tvs2;
# 173
tvs2=_T5A->tl;goto _TL27;_TL26:;}_T5B=tv;
# 194
_T5B->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));
_T5D->hd=tv;_T5D->tl=tvs;_T5C=(struct Cyc_List_List*)_T5D;}return _T5C;}
# 198
static int Cyc_Tctyp_cmp_fqv(struct _tuple14*te1,struct _tuple14*te2){struct _tuple14*_T0;struct _tuple14 _T1;struct _tuple14*_T2;struct _tuple14 _T3;int _T4;int _T5;_T0=te1;_T1=*_T0;{
void*t1=_T1.f0;_T2=te2;_T3=*_T2;{
void*t2=_T3.f0;_T4=
Cyc_Tcutil_typecmp(t1,t2);_T5=_T4==0;return _T5;}}}
# 205
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;void**_T3;int(*_T4)(struct _fat_ptr);void*(*_T5)(struct _fat_ptr);struct _fat_ptr _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;int _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;void**_TC;int(*_TD)(struct _fat_ptr);void*(*_TE)(struct _fat_ptr);struct _fat_ptr _TF;struct Cyc_Absyn_Tvar*_T10;int _T11;struct Cyc_Absyn_Tvar*_T12;int _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;_T0=tv;_T1=_T0->identity;
if(_T1!=-1)goto _TL38;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
_T17.f1=_tag_fat("fast_add_free_tvar: bad identity in tv",sizeof(char),39U);_T2=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_T2;void*_T18[1];_T3=_T18 + 0;*_T3=& _T17;_T5=Cyc_Warn_impos2;{int(*_T19)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5;_T4=_T19;}_T6=_tag_fat(_T18,sizeof(void*),1);_T4(_T6);}goto _TL39;_TL38: _TL39:{
struct Cyc_List_List*tvs2=tvs;_TL3D: if(tvs2!=0)goto _TL3B;else{goto _TL3C;}
_TL3B: _T7=tvs2;_T8=_T7->hd;{struct Cyc_Absyn_Tvar*tv2=(struct Cyc_Absyn_Tvar*)_T8;_T9=tv2;_TA=_T9->identity;
if(_TA!=-1)goto _TL3E;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
_T17.f1=_tag_fat("fast_add_free_tvar: bad identity in tvs2",sizeof(char),41U);_TB=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TB;void*_T18[1];_TC=_T18 + 0;*_TC=& _T17;_TE=Cyc_Warn_impos2;{int(*_T19)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TE;_TD=_T19;}_TF=_tag_fat(_T18,sizeof(void*),1);_TD(_TF);}goto _TL3F;_TL3E: _TL3F: _T10=tv2;_T11=_T10->identity;_T12=tv;_T13=_T12->identity;
if(_T11!=_T13)goto _TL40;_T14=tvs;
return _T14;_TL40:;}_T15=tvs2;
# 208
tvs2=_T15->tl;goto _TL3D;_TL3C:;}{struct Cyc_List_List*_T17=_cycalloc(sizeof(struct Cyc_List_List));
# 215
_T17->hd=tv;_T17->tl=tvs;_T16=(struct Cyc_List_List*)_T17;}return _T16;}
# 221
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){struct Cyc_Absyn_Tvar*_T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;void**_T3;int(*_T4)(struct _fat_ptr);void*(*_T5)(struct _fat_ptr);struct _fat_ptr _T6;struct Cyc_List_List*_T7;void*_T8;int*_T9;struct Cyc_Absyn_Tvar*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;void**_TD;int(*_TE)(struct _fat_ptr);void*(*_TF)(struct _fat_ptr);struct _fat_ptr _T10;struct Cyc_Absyn_Tvar*_T11;int _T12;struct Cyc_Absyn_Tvar*_T13;int _T14;int*_T15;int _T16;int*_T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct _tuple12*_T1C;_T0=tv;_T1=_T0->identity;
if(_T1!=-1)goto _TL42;{struct Cyc_Warn_String_Warn_Warg_struct _T1D;_T1D.tag=0;
_T1D.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tv",sizeof(char),44U);_T2=_T1D;}{struct Cyc_Warn_String_Warn_Warg_struct _T1D=_T2;void*_T1E[1];_T3=_T1E + 0;*_T3=& _T1D;_T5=Cyc_Warn_impos2;{int(*_T1F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5;_T4=_T1F;}_T6=_tag_fat(_T1E,sizeof(void*),1);_T4(_T6);}goto _TL43;_TL42: _TL43:{
struct Cyc_List_List*tvs2=tvs;_TL47: if(tvs2!=0)goto _TL45;else{goto _TL46;}
_TL45: _T7=tvs2;_T8=_T7->hd;{struct _tuple12*_T1D=(struct _tuple12*)_T8;void*_T1E;struct Cyc_Absyn_Tvar*_T1F;{struct _tuple12 _T20=*_T1D;_T1F=_T20.f0;_T9=& _T1D->f1;_T1E=(int*)_T9;}{struct Cyc_Absyn_Tvar*tv2=_T1F;int*b2=(int*)_T1E;_TA=tv2;_TB=_TA->identity;
if(_TB!=-1)goto _TL48;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
_T20.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tvs2",sizeof(char),46U);_TC=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_TC;void*_T21[1];_TD=_T21 + 0;*_TD=& _T20;_TF=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TF;_TE=_T22;}_T10=_tag_fat(_T21,sizeof(void*),1);_TE(_T10);}goto _TL49;_TL48: _TL49: _T11=tv2;_T12=_T11->identity;_T13=tv;_T14=_T13->identity;
if(_T12!=_T14)goto _TL4A;_T15=b2;_T17=b2;_T18=*_T17;
if(!_T18)goto _TL4C;_T16=1;goto _TL4D;_TL4C: _T16=b;_TL4D:*_T15=_T16;_T19=tvs;
return _T19;_TL4A:;}}_T1A=tvs2;
# 224
tvs2=_T1A->tl;goto _TL47;_TL46:;}{struct Cyc_List_List*_T1D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T1E=_cycalloc(sizeof(struct _tuple12));
# 233
_T1E->f0=tv;_T1E->f1=b;_T1C=(struct _tuple12*)_T1E;}_T1D->hd=_T1C;_T1D->tl=tvs;_T1B=(struct Cyc_List_List*)_T1D;}return _T1B;}
# 240
static int Cyc_Tctyp_check_type_visit_f1(struct Cyc_Tctyp_CVTEnv*env,struct Cyc_Absyn_Exp*e){struct Cyc_Tctyp_CVTEnv*_T0;struct Cyc_Absyn_Exp*_T1;struct Cyc_Absyn_Exp*_T2;int*_T3;unsigned _T4;void*_T5;struct Cyc_Tctyp_CVTEnv*_T6;struct Cyc_Tctyp_CVTEnv*_T7;struct Cyc_Tctyp_CVTEnv _T8;struct Cyc_Absyn_Kind*_T9;struct Cyc_Absyn_Kind*_TA;void*_TB;void*_TC;void*_TD;struct Cyc_Tctyp_CVTEnv*_TE;struct Cyc_Tctyp_CVTEnv*_TF;struct Cyc_Tctyp_CVTEnv _T10;struct Cyc_Absyn_Kind*_T11;struct Cyc_Absyn_Kind*_T12;void*_T13;void*_T14;struct Cyc_Tctyp_CVTEnv*_T15;struct Cyc_Tctyp_CVTEnv*_T16;struct Cyc_Tctyp_CVTEnv _T17;struct Cyc_Absyn_Kind*_T18;struct Cyc_Absyn_Kind*_T19;void*_T1A;_T0=env;_T1=e;
# 243
_T0->loc=_T1->loc;_T2=e;{
void*_T1B=_T2->r;void*_T1C;_T3=(int*)_T1B;_T4=*_T3;switch(_T4){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T5=_T1D->f1;_T1C=(void*)_T5;}{void*t=_T1C;_T6=env;_T7=env;_T8=*_T7;_T9=& Cyc_Kinds_ak;_TA=(struct Cyc_Absyn_Kind*)_T9;_TB=t;
*_T6=Cyc_Tctyp_i_check_type(_T8,_TA,_TB,1,0);goto _LL0;}case 19:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T1B;_TC=_T1D->f1;_T1C=(void*)_TC;}{void*t=_T1C;_T1C=t;goto _LL6;}case 17:{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T1B;_TD=_T1D->f1;_T1C=(void*)_TD;}_LL6: {void*t=_T1C;_TE=env;_TF=env;_T10=*_TF;_T11=& Cyc_Kinds_ak;_T12=(struct Cyc_Absyn_Kind*)_T11;_T13=t;
# 247
*_TE=Cyc_Tctyp_i_check_type(_T10,_T12,_T13,1,0);goto _LL0;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T1B;_T14=_T1D->f1;_T1C=(void*)_T14;}{void*t=_T1C;_T15=env;_T16=env;_T17=*_T16;_T18=& Cyc_Kinds_ik;_T19=(struct Cyc_Absyn_Kind*)_T18;_T1A=t;
*_T15=Cyc_Tctyp_i_check_type(_T17,_T19,_T1A,1,0);goto _LL0;}default: goto _LL0;}_LL0:;}
# 251
return 1;}
# 253
static int Cyc_Tctyp_check_type_visit_f2(struct Cyc_Tctyp_CVTEnv*env,struct Cyc_Absyn_Stmt*s){int(*_T0)(unsigned,struct _fat_ptr,struct _fat_ptr);void*(*_T1)(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Absyn_Stmt*_T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;_T1=Cyc_Warn_impos_loc;{
int(*_T6)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))_T1;_T0=_T6;}_T2=s;_T3=_T2->loc;_T4=_tag_fat("statement inside expression inside type",sizeof(char),40U);_T5=_tag_fat(0U,sizeof(void*),0);_T0(_T3,_T4,_T5);}
# 256
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tctyp_CVTEnv env){void(*_T0)(int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Tctyp_CVTEnv*_T2;struct Cyc_Tctyp_CVTEnv*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Tctyp_CVTEnv _T5;_T1=Cyc_Absyn_visit_exp;{
void(*_T6)(int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Stmt*),struct Cyc_Tctyp_CVTEnv*,struct Cyc_Absyn_Exp*))_T1;_T0=_T6;}_T2=& env;_T3=(struct Cyc_Tctyp_CVTEnv*)_T2;_T4=e;_T0(Cyc_Tctyp_check_type_visit_f1,Cyc_Tctyp_check_type_visit_f2,_T3,_T4);_T5=env;
return _T5;}
# 299 "tctyp.cyc"
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){struct Cyc_List_List**_T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_Tctyp_CVTEnv _T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple0 _T6;struct Cyc_Absyn_Tvar*_T7;void*_T8;void*_T9;int*_TA;unsigned _TB;void*_TC;int*_TD;int _TE;void*_TF;struct Cyc_Tctyp_CVTEnv _T10;unsigned _T11;struct Cyc_Tctyp_CVTEnv _T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_Tvar*_T14;struct Cyc_Tctyp_CVTEnv _T15;int _T16;struct Cyc_Tctyp_CVTEnv _T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Tvar*_T19;void*_T1A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1B;struct Cyc_Absyn_Kind*_T1C;struct Cyc_Absyn_Kind*_T1D;enum Cyc_Absyn_KindQual _T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Tctyp_CVTEnv _T25;struct Cyc_Absyn_Kind*_T26;struct Cyc_Absyn_Kind*_T27;struct Cyc_List_List*_T28;void*_T29;int _T2A;struct Cyc_Absyn_Tvar*_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Tctyp_CVTEnv _T2E;unsigned _T2F;void*_T30;struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;struct Cyc_Warn_String_Warn_Warg_struct _T33;struct Cyc_Warn_Qvar_Warn_Warg_struct _T34;void**_T35;void**_T36;struct Cyc_Tctyp_CVTEnv _T37;unsigned _T38;struct _fat_ptr _T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;void*_T3C;struct Cyc_Absyn_Tvar*_T3D;struct Cyc_Absyn_Kind*_T3E;struct Cyc_Absyn_Kind*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List**_T41;struct Cyc_List_List**_T42;struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;struct Cyc_Tctyp_CVTEnv _T45;_T0=targsp;{
# 304
struct Cyc_List_List*targs=*_T0;_T1=env;{
int old_inst_evar=_T1.inst_type_evar;_T2=env;
env.inst_type_evar=_T2.generalize_evars;
_TL52: if(targs!=0)goto _TL53;else{goto _TL51;}_TL53: if(tvs!=0)goto _TL50;else{goto _TL51;}
_TL50: _T3=targs;{void*t=_T3->hd;_T4=tvs;_T5=_T4->hd;{
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T5;{struct _tuple0 _T46;_T7=tv;_T8=_T7->kind;
# 312
_T46.f0=Cyc_Kinds_compress_kb(_T8);_T46.f1=t;_T6=_T46;}{struct _tuple0 _T46=_T6;struct Cyc_Absyn_Tvar*_T47;_T9=_T46.f0;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 1: _TC=_T46.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=2)goto _TL55;_TF=_T46.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T48=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TF;_T47=_T48->f1;}{struct Cyc_Absyn_Tvar*tv2=_T47;_T10=env;_T11=_T10.loc;_T12=env;_T13=_T12.kind_env;_T14=tv2;_T15=env;_T16=_T15.allow_aqual_bounds;
# 314
env.kind_env=Cyc_Tctyp_add_free_tvar(_T11,_T13,_T14,_T16);_T17=env;_T18=_T17.free_vars;_T19=tv2;
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T18,_T19,1);goto _TL4F;}_TL55: goto _LL5;case 0: _T1A=_T46.f0;_T1B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1A;_T1C=_T1B->f1;_T1D=(struct Cyc_Absyn_Kind*)_T1C;_T1E=_T1D->kind;if(_T1E!=Cyc_Absyn_AqualKind)goto _TL57;_T1F=_T46.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=2)goto _TL59;_T22=_T46.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T48=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T22;_T47=_T48->f1;}{struct Cyc_Absyn_Tvar*tv2=_T47;_T23=targs;_T24=
# 318
Cyc_Absyn_aqual_var_type(t,Cyc_Absyn_al_qual_type);_T23->hd=(void*)_T24;_T25=env;_T26=& Cyc_Kinds_aqk;_T27=(struct Cyc_Absyn_Kind*)_T26;_T28=targs;_T29=_T28->hd;_T2A=allow_abs_aggr;
env=Cyc_Tctyp_i_check_type(_T25,_T27,_T29,1,_T2A);goto _TL4F;}_TL59: goto _LL5;_TL57: goto _LL5;default: _LL5: goto _LL0;}_LL0:;}_T2B=tv;_T2C=& Cyc_Kinds_bk;_T2D=(struct Cyc_Absyn_Kind*)_T2C;{
# 323
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(_T2B,_T2D);
env=Cyc_Tctyp_i_check_type(env,k,t,1,allow_abs_aggr);_T2E=env;_T2F=_T2E.loc;_T30=t;
Cyc_Tcutil_check_no_qual(_T2F,_T30);}}}_TL4F: _T31=targs;
# 307
targs=_T31->tl;_T32=tvs;tvs=_T32->tl;goto _TL52;_TL51:
# 327
 if(targs==0)goto _TL5B;{struct Cyc_Warn_String_Warn_Warg_struct _T46;_T46.tag=0;
_T46.f1=_tag_fat("too many type arguments for ",sizeof(char),29U);_T33=_T46;}{struct Cyc_Warn_String_Warn_Warg_struct _T46=_T33;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T47;_T47.tag=1;_T47.f1=typname;_T34=_T47;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T47=_T34;void*_T48[2];_T35=_T48 + 0;*_T35=& _T46;_T36=_T48 + 1;*_T36=& _T47;_T37=env;_T38=_T37.loc;_T39=_tag_fat(_T48,sizeof(void*),2);Cyc_Warn_err2(_T38,_T39);}}goto _TL5C;_TL5B: _TL5C: {
# 330
struct Cyc_List_List*hidden_ts=0;
_TL60: if(tvs!=0)goto _TL5E;else{goto _TL5F;}
_TL5E:{void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_T46=_cycalloc(sizeof(struct Cyc_List_List));
_T46->hd=e;_T46->tl=hidden_ts;_T3A=(struct Cyc_List_List*)_T46;}hidden_ts=_T3A;_T3B=tvs;_T3C=_T3B->hd;_T3D=(struct Cyc_Absyn_Tvar*)_T3C;_T3E=& Cyc_Kinds_bk;_T3F=(struct Cyc_Absyn_Kind*)_T3E;{
# 335
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(_T3D,_T3F);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}_T40=tvs;
# 331
tvs=_T40->tl;goto _TL60;_TL5F: _T41=targsp;_T42=targsp;_T43=*_T42;_T44=
# 338
Cyc_List_imp_rev(hidden_ts);*_T41=Cyc_List_imp_append(_T43,_T44);
env.inst_type_evar=old_inst_evar;_T45=env;
return _T45;}}}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f0;struct Cyc_AssnDef_AssnMap f1;};
# 344
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){struct _tuple15 _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Absyn_Exp*_T3;int _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;void**_TA;void**_TB;void**_TC;void**_TD;struct Cyc_Tctyp_CVTEnv _TE;unsigned _TF;struct _fat_ptr _T10;struct _tuple15 _T11;
# 347
if(clause!=0)goto _TL61;{struct _tuple15 _T12;
_T12.f0=env;_T12.f1=Cyc_AssnDef_true_assnmap();_T0=_T12;}return _T0;_TL61: _T1=env;_T2=_T1.te;_T3=clause;
Cyc_Tcexp_tcExp(_T2,0,_T3);_T4=
Cyc_Tcutil_is_integral(clause);if(_T4)goto _TL63;else{goto _TL65;}
_TL65:{struct Cyc_Warn_String_Warn_Warg_struct _T12;_T12.tag=0;_T12.f1=clause_name;_T5=_T12;}{struct Cyc_Warn_String_Warn_Warg_struct _T12=_T5;{struct Cyc_Warn_String_Warn_Warg_struct _T13;_T13.tag=0;_T13.f1=_tag_fat(" clause has type ",sizeof(char),18U);_T6=_T13;}{struct Cyc_Warn_String_Warn_Warg_struct _T13=_T6;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T14;_T14.tag=3;_T8=clause;_T14.f1=_T8->topt;_T7=_T14;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T14=_T7;{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;
_T15.f1=_tag_fat(" instead of integral type",sizeof(char),26U);_T9=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_T9;void*_T16[4];_TA=_T16 + 0;*_TA=& _T12;_TB=_T16 + 1;*_TB=& _T13;_TC=_T16 + 2;*_TC=& _T14;_TD=_T16 + 3;*_TD=& _T15;_TE=env;_TF=_TE.loc;_T10=_tag_fat(_T16,sizeof(void*),4);Cyc_Warn_err2(_TF,_T10);}}}}goto _TL64;_TL63: _TL64:
 env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_AssnDef_AssnMap assnmap=Cyc_Vcgen_clause2assn(clause);{struct _tuple15 _T12;
_T12.f0=env;_T12.f1=assnmap;_T11=_T12;}return _T11;}}
# 359
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;union Cyc_Absyn_AggrInfo*_T2;struct _union_AggrInfo_UnknownAggr _T3;unsigned _T4;struct _union_AggrInfo_UnknownAggr _T5;struct _tuple3 _T6;struct _union_AggrInfo_UnknownAggr _T7;struct _tuple3 _T8;struct _union_AggrInfo_UnknownAggr _T9;struct _tuple3 _TA;struct _handler_cons*_TB;int*_TC;int _TD;struct Cyc_Absyn_Aggrdecl**_TE;struct Cyc_Absyn_Aggrdecl*_TF;enum Cyc_Absyn_AggrKind _T10;int _T11;enum Cyc_Absyn_AggrKind _T12;int _T13;struct Cyc_Absyn_Aggrdecl*_T14;enum Cyc_Absyn_AggrKind _T15;int _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_Qvar_Warn_Warg_struct _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1A;void**_T1B;void**_T1C;void**_T1D;void**_T1E;unsigned _T1F;struct _fat_ptr _T20;struct Cyc_Warn_String_Warn_Warg_struct _T21;struct Cyc_Warn_Qvar_Warn_Warg_struct _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_Qvar_Warn_Warg_struct _T24;void**_T25;void**_T26;void**_T27;void**_T28;unsigned _T29;struct _fat_ptr _T2A;struct Cyc_Core_Opt*_T2B;unsigned _T2C;struct Cyc_Core_Opt*_T2D;void*_T2E;int _T2F;struct Cyc_Absyn_Aggrdecl*_T30;struct Cyc_Absyn_AggrdeclImpl*_T31;unsigned _T32;struct Cyc_Absyn_Aggrdecl*_T33;struct Cyc_Absyn_AggrdeclImpl*_T34;int _T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Qvar_Warn_Warg_struct _T37;void**_T38;void**_T39;unsigned _T3A;struct _fat_ptr _T3B;union Cyc_Absyn_AggrInfo*_T3C;void*_T3D;struct Cyc_Dict_Absent_exn_struct*_T3E;char*_T3F;char*_T40;struct Cyc_Tcenv_Tenv*_T41;unsigned _T42;struct Cyc_Absyn_Aggrdecl*_T43;union Cyc_Absyn_AggrInfo*_T44;struct Cyc_List_List**_T45;struct Cyc_List_List*_T46;struct Cyc_Warn_String_Warn_Warg_struct _T47;struct Cyc_Warn_Qvar_Warn_Warg_struct _T48;struct Cyc_Warn_String_Warn_Warg_struct _T49;void**_T4A;void**_T4B;void**_T4C;unsigned _T4D;struct _fat_ptr _T4E;struct Cyc_Tctyp_CVTEnv _T4F;struct Cyc_Absyn_Aggrdecl**_T50;struct _union_AggrInfo_KnownAggr _T51;struct Cyc_Absyn_Aggrdecl**_T52;struct Cyc_Tctyp_CVTEnv _T53;struct Cyc_List_List**_T54;struct Cyc_Absyn_Aggrdecl*_T55;struct Cyc_List_List*_T56;struct Cyc_Absyn_Kind*_T57;int _T58;struct Cyc_Absyn_Aggrdecl*_T59;struct _tuple1*_T5A;int _T5B;struct Cyc_Absyn_Aggrdecl*_T5C;struct Cyc_Absyn_AggrdeclImpl*_T5D;struct Cyc_Absyn_Kind*_T5E;struct Cyc_Absyn_Kind*_T5F;struct Cyc_Absyn_Kind*_T60;int _T61;struct Cyc_Absyn_Aggrdecl*_T62;struct Cyc_Tctyp_CVTEnv _T63;struct Cyc_Tcenv_Tenv*_T64;unsigned _T65;_T0=env;_T65=_T0.loc;_T1=env;_T64=_T1.te;{unsigned loc=_T65;struct Cyc_Tcenv_Tenv*te=_T64;_T2=info;{
# 363
union Cyc_Absyn_AggrInfo _T66=*_T2;struct Cyc_Absyn_Aggrdecl*_T67;struct Cyc_Core_Opt*_T68;struct _tuple1*_T69;enum Cyc_Absyn_AggrKind _T6A;_T3=_T66.UnknownAggr;_T4=_T3.tag;if(_T4!=1)goto _TL66;_T5=_T66.UnknownAggr;_T6=_T5.val;_T6A=_T6.f0;_T7=_T66.UnknownAggr;_T8=_T7.val;_T69=_T8.f1;_T9=_T66.UnknownAggr;_TA=_T9.val;_T68=_TA.f2;{enum Cyc_Absyn_AggrKind k=_T6A;struct _tuple1*n=_T69;struct Cyc_Core_Opt*tgd=_T68;
# 365
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _T6B;_TB=& _T6B;_push_handler(_TB);{int _T6C=0;_TC=_T6B.handler;_TD=setjmp(_TC);if(!_TD)goto _TL68;_T6C=1;goto _TL69;_TL68: _TL69: if(_T6C)goto _TL6A;else{goto _TL6C;}_TL6C:
# 367
 adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);_TE=adp;{
struct Cyc_Absyn_Aggrdecl*ad=*_TE;_TF=ad;_T10=_TF->kind;_T11=(int)_T10;_T12=k;_T13=(int)_T12;
if(_T11==_T13)goto _TL6D;_T14=ad;_T15=_T14->kind;_T16=(int)_T15;
if(_T16!=0)goto _TL6F;{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;
_T6D.f1=_tag_fat("expecting struct ",sizeof(char),18U);_T17=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T17;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E;_T6E.tag=1;_T6E.f1=n;_T18=_T6E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E=_T18;{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;_T6F.f1=_tag_fat("instead of union ",sizeof(char),18U);_T19=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T19;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70;_T70.tag=1;_T70.f1=n;_T1A=_T70;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70=_T1A;void*_T71[4];_T1B=_T71 + 0;*_T1B=& _T6D;_T1C=_T71 + 1;*_T1C=& _T6E;_T1D=_T71 + 2;*_T1D=& _T6F;_T1E=_T71 + 3;*_T1E=& _T70;_T1F=loc;_T20=_tag_fat(_T71,sizeof(void*),4);Cyc_Warn_err2(_T1F,_T20);}}}}goto _TL70;
# 373
_TL6F:{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;_T6D.f1=_tag_fat("expecting union ",sizeof(char),17U);_T21=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T21;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E;_T6E.tag=1;_T6E.f1=n;_T22=_T6E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E=_T22;{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;_T6F.f1=_tag_fat("instead of struct ",sizeof(char),19U);_T23=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T23;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70;_T70.tag=1;_T70.f1=n;_T24=_T70;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70=_T24;void*_T71[4];_T25=_T71 + 0;*_T25=& _T6D;_T26=_T71 + 1;*_T26=& _T6E;_T27=_T71 + 2;*_T27=& _T6F;_T28=_T71 + 3;*_T28=& _T70;_T29=loc;_T2A=_tag_fat(_T71,sizeof(void*),4);Cyc_Warn_err2(_T29,_T2A);}}}}_TL70: goto _TL6E;_TL6D: _TL6E: _T2B=tgd;_T2C=(unsigned)_T2B;
# 375
if(!_T2C)goto _TL71;_T2D=tgd;_T2E=_T2D->v;_T2F=(int)_T2E;if(!_T2F)goto _TL71;_T30=ad;_T31=_T30->impl;_T32=(unsigned)_T31;if(_T32)goto _TL74;else{goto _TL73;}_TL74: _T33=ad;_T34=_T33->impl;_T35=_T34->tagged;if(_T35)goto _TL71;else{goto _TL73;}
_TL73:{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;_T6D.f1=_tag_fat("@tagged qualfiers don't agree on union ",sizeof(char),40U);_T36=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T36;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E;_T6E.tag=1;_T6E.f1=n;_T37=_T6E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T6E=_T37;void*_T6F[2];_T38=_T6F + 0;*_T38=& _T6D;_T39=_T6F + 1;*_T39=& _T6E;_T3A=loc;_T3B=_tag_fat(_T6F,sizeof(void*),2);Cyc_Warn_err2(_T3A,_T3B);}}goto _TL72;_TL71: _TL72: _T3C=info;
# 378
*_T3C=Cyc_Absyn_KnownAggr(adp);}_pop_handler();goto _TL6B;_TL6A: _T3D=Cyc_Core_get_exn_thrown();{void*_T6D=(void*)_T3D;void*_T6E;_T3E=(struct Cyc_Dict_Absent_exn_struct*)_T6D;_T3F=_T3E->tag;_T40=Cyc_Dict_Absent;if(_T3F!=_T40)goto _TL75;_T41=te;_T42=loc;{struct Cyc_Absyn_Aggrdecl*_T6F=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));
# 382
_T6F->kind=k;_T6F->sc=3U;_T6F->name=n;_T6F->tvs=0;_T6F->impl=0;_T6F->attributes=0;_T6F->expected_mem_kind=0;_T43=(struct Cyc_Absyn_Aggrdecl*)_T6F;}Cyc_Tc_tcAggrdecl(_T41,_T42,_T43);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);_T44=info;
*_T44=Cyc_Absyn_KnownAggr(adp);_T45=targs;_T46=*_T45;
# 386
if(_T46==0)goto _TL77;{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;
_T6F.f1=_tag_fat("declare parameterized type ",sizeof(char),28U);_T47=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T47;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70;_T70.tag=1;_T70.f1=n;_T48=_T70;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70=_T48;{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;_T71.f1=_tag_fat(" before using",sizeof(char),14U);_T49=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T49;void*_T72[3];_T4A=_T72 + 0;*_T4A=& _T6F;_T4B=_T72 + 1;*_T4B=& _T70;_T4C=_T72 + 2;*_T4C=& _T71;_T4D=loc;_T4E=_tag_fat(_T72,sizeof(void*),3);Cyc_Warn_err2(_T4D,_T4E);}}}_T4F=env;
return _T4F;_TL77: goto _LL8;_TL75: _T6E=_T6D;{void*exn=_T6E;_rethrow(exn);}_LL8:;}_TL6B:;}}_T50=adp;_T67=*_T50;goto _LL7;}_TL66: _T51=_T66.KnownAggr;_T52=_T51.val;{struct Cyc_Absyn_Aggrdecl*_T6B=*_T52;_T67=_T6B;}_LL7:{struct Cyc_Absyn_Aggrdecl*ad=_T67;_T53=env;_T54=targs;_T55=ad;_T56=_T55->tvs;_T57=expected_kind;_T58=allow_abs_aggr;_T59=ad;_T5A=_T59->name;
# 394
env=Cyc_Tctyp_check_type_inst(_T53,_T54,_T56,_T57,_T58,_T5A,0);_T5B=allow_abs_aggr;
# 396
if(!_T5B)goto _TL79;_T5C=ad;_T5D=_T5C->impl;if(_T5D!=0)goto _TL79;_T5E=& Cyc_Kinds_ak;_T5F=(struct Cyc_Absyn_Kind*)_T5E;_T60=expected_kind;_T61=
Cyc_Kinds_kind_leq(_T5F,_T60);
# 396
if(_T61)goto _TL79;else{goto _TL7B;}
# 401
_TL7B: _T62=ad;_T62->expected_mem_kind=1;goto _TL7A;_TL79: _TL7A: _T63=env;
return _T63;};}}}
# 407
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_List_List**_T2;union Cyc_Absyn_DatatypeInfo*_T3;struct _union_DatatypeInfo_UnknownDatatype _T4;unsigned _T5;struct _union_DatatypeInfo_UnknownDatatype _T6;struct Cyc_Absyn_UnknownDatatypeInfo _T7;struct _union_DatatypeInfo_UnknownDatatype _T8;struct Cyc_Absyn_UnknownDatatypeInfo _T9;struct _handler_cons*_TA;int*_TB;int _TC;void*_TD;struct Cyc_Dict_Absent_exn_struct*_TE;char*_TF;char*_T10;struct Cyc_Tcenv_Tenv*_T11;unsigned _T12;struct Cyc_Absyn_Datatypedecl*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Qvar_Warn_Warg_struct _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;void**_T17;void**_T18;void**_T19;unsigned _T1A;struct _fat_ptr _T1B;struct Cyc_Tctyp_CVTEnv _T1C;int _T1D;struct Cyc_Absyn_Datatypedecl**_T1E;struct Cyc_Absyn_Datatypedecl*_T1F;int _T20;struct Cyc_Warn_String_Warn_Warg_struct _T21;struct Cyc_Warn_Qvar_Warn_Warg_struct _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;void**_T24;void**_T25;void**_T26;unsigned _T27;struct _fat_ptr _T28;union Cyc_Absyn_DatatypeInfo*_T29;struct Cyc_Absyn_Datatypedecl**_T2A;struct _union_DatatypeInfo_KnownDatatype _T2B;struct Cyc_Absyn_Datatypedecl**_T2C;struct Cyc_Tctyp_CVTEnv _T2D;struct Cyc_List_List**_T2E;struct Cyc_Absyn_Datatypedecl*_T2F;struct Cyc_List_List*_T30;struct Cyc_Absyn_Kind*_T31;int _T32;struct Cyc_Absyn_Datatypedecl*_T33;struct _tuple1*_T34;struct Cyc_Tctyp_CVTEnv _T35;struct Cyc_Tcenv_Tenv*_T36;unsigned _T37;_T0=env;_T37=_T0.loc;_T1=env;_T36=_T1.te;{unsigned loc=_T37;struct Cyc_Tcenv_Tenv*te=_T36;_T2=targsp;{
# 411
struct Cyc_List_List*targs=*_T2;_T3=info;{
union Cyc_Absyn_DatatypeInfo _T38=*_T3;struct Cyc_Absyn_Datatypedecl*_T39;int _T3A;struct _tuple1*_T3B;_T4=_T38.UnknownDatatype;_T5=_T4.tag;if(_T5!=1)goto _TL7C;_T6=_T38.UnknownDatatype;_T7=_T6.val;_T3B=_T7.name;_T8=_T38.UnknownDatatype;_T9=_T8.val;_T3A=_T9.is_extensible;{struct _tuple1*n=_T3B;int is_x=_T3A;
# 414
struct Cyc_Absyn_Datatypedecl**tudp;{struct _handler_cons _T3C;_TA=& _T3C;_push_handler(_TA);{int _T3D=0;_TB=_T3C.handler;_TC=setjmp(_TB);if(!_TC)goto _TL7E;_T3D=1;goto _TL7F;_TL7E: _TL7F: if(_T3D)goto _TL80;else{goto _TL82;}_TL82:
 tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);_pop_handler();goto _TL81;_TL80: _TD=Cyc_Core_get_exn_thrown();{void*_T3E=(void*)_TD;void*_T3F;_TE=(struct Cyc_Dict_Absent_exn_struct*)_T3E;_TF=_TE->tag;_T10=Cyc_Dict_Absent;if(_TF!=_T10)goto _TL83;_T11=te;_T12=loc;{struct Cyc_Absyn_Datatypedecl*_T40=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));
# 418
_T40->sc=3U;_T40->name=n;_T40->tvs=0;_T40->fields=0;_T40->is_extensible=is_x;_T13=(struct Cyc_Absyn_Datatypedecl*)_T40;}Cyc_Tc_tcDatatypedecl(_T11,_T12,_T13);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 421
if(targs==0)goto _TL85;{struct Cyc_Warn_String_Warn_Warg_struct _T40;_T40.tag=0;
_T40.f1=_tag_fat("declare parameterized datatype ",sizeof(char),32U);_T14=_T40;}{struct Cyc_Warn_String_Warn_Warg_struct _T40=_T14;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T41;_T41.tag=1;_T41.f1=n;_T15=_T41;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T41=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T42;_T42.tag=0;_T42.f1=_tag_fat(" before using",sizeof(char),14U);_T16=_T42;}{struct Cyc_Warn_String_Warn_Warg_struct _T42=_T16;void*_T43[3];_T17=_T43 + 0;*_T17=& _T40;_T18=_T43 + 1;*_T18=& _T41;_T19=_T43 + 2;*_T19=& _T42;_T1A=loc;_T1B=_tag_fat(_T43,sizeof(void*),3);Cyc_Warn_err2(_T1A,_T1B);}}}_T1C=env;
return _T1C;_TL85: goto _LL8;_TL83: _T3F=_T3E;{void*exn=_T3F;_rethrow(exn);}_LL8:;}_TL81:;}}_T1D=is_x;
# 429
if(!_T1D)goto _TL87;_T1E=tudp;_T1F=*_T1E;_T20=_T1F->is_extensible;if(_T20)goto _TL87;else{goto _TL89;}
_TL89:{struct Cyc_Warn_String_Warn_Warg_struct _T3C;_T3C.tag=0;_T3C.f1=_tag_fat("datatype ",sizeof(char),10U);_T21=_T3C;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C=_T21;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3D;_T3D.tag=1;_T3D.f1=n;_T22=_T3D;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3D=_T22;{struct Cyc_Warn_String_Warn_Warg_struct _T3E;_T3E.tag=0;_T3E.f1=_tag_fat(" was not declared @extensible",sizeof(char),30U);_T23=_T3E;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E=_T23;void*_T3F[3];_T24=_T3F + 0;*_T24=& _T3C;_T25=_T3F + 1;*_T25=& _T3D;_T26=_T3F + 2;*_T26=& _T3E;_T27=loc;_T28=_tag_fat(_T3F,sizeof(void*),3);Cyc_Warn_err2(_T27,_T28);}}}goto _TL88;_TL87: _TL88: _T29=info;
*_T29=Cyc_Absyn_KnownDatatype(tudp);_T2A=tudp;_T39=*_T2A;goto _LL7;}_TL7C: _T2B=_T38.KnownDatatype;_T2C=_T2B.val;{struct Cyc_Absyn_Datatypedecl*_T3C=*_T2C;_T39=_T3C;}_LL7:{struct Cyc_Absyn_Datatypedecl*tud=_T39;_T2D=env;_T2E=targsp;_T2F=tud;_T30=_T2F->tvs;_T31=expected_kind;_T32=allow_abs_aggr;_T33=tud;_T34=_T33->name;_T35=
# 434
Cyc_Tctyp_check_type_inst(_T2D,_T2E,_T30,_T31,_T32,_T34,0);return _T35;};}}}}
# 440
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){union Cyc_Absyn_DatatypeFieldInfo*_T0;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T1;unsigned _T2;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T4;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T5;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T7;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T8;struct Cyc_Tctyp_CVTEnv _T9;struct Cyc_Tcenv_Tenv*_TA;struct Cyc_Tctyp_CVTEnv _TB;unsigned _TC;struct _tuple1*_TD;struct Cyc_Absyn_Datatypedecl**_TE;struct Cyc_Absyn_Datatypedecl*_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Core_Opt*_T11;void*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;int(*_T15)(struct _fat_ptr);void*(*_T16)(struct _fat_ptr);struct _fat_ptr _T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Absyn_Datatypefield*_T1A;struct _tuple1*_T1B;struct _tuple1*_T1C;int _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_List_List*_T20;union Cyc_Absyn_DatatypeFieldInfo*_T21;struct _union_DatatypeFieldInfo_KnownDatatypefield _T22;struct _tuple2 _T23;struct _union_DatatypeFieldInfo_KnownDatatypefield _T24;struct _tuple2 _T25;struct Cyc_Tctyp_CVTEnv _T26;struct Cyc_List_List**_T27;struct Cyc_Absyn_Datatypedecl*_T28;struct Cyc_List_List*_T29;struct Cyc_Absyn_Kind*_T2A;int _T2B;struct Cyc_Absyn_Datatypefield*_T2C;struct _tuple1*_T2D;struct Cyc_Tctyp_CVTEnv _T2E;_T0=info;{
# 443
union Cyc_Absyn_DatatypeFieldInfo _T2F=*_T0;struct Cyc_Absyn_Datatypefield*_T30;struct Cyc_Absyn_Datatypedecl*_T31;int _T32;struct _tuple1*_T33;struct _tuple1*_T34;_T1=_T2F.UnknownDatatypefield;_T2=_T1.tag;if(_T2!=1)goto _TL8A;_T3=_T2F.UnknownDatatypefield;_T4=_T3.val;_T34=_T4.datatype_name;_T5=_T2F.UnknownDatatypefield;_T6=_T5.val;_T33=_T6.field_name;_T7=_T2F.UnknownDatatypefield;_T8=_T7.val;_T32=_T8.is_extensible;{struct _tuple1*tname=_T34;struct _tuple1*fname=_T33;int is_x=_T32;_T9=env;_TA=_T9.te;_TB=env;_TC=_TB.loc;_TD=tname;_TE=
# 446
Cyc_Tcenv_lookup_datatypedecl(_TA,_TC,_TD);{struct Cyc_Absyn_Datatypedecl*tud=*_TE;
struct Cyc_Absyn_Datatypefield*tuf;_TF=tud;_T10=_TF->fields;_T11=
# 451
_check_null(_T10);_T12=_T11->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T12;_TL8F: if(1)goto _TL8D;else{goto _TL8E;}
_TL8D: if(fs!=0)goto _TL90;{struct Cyc_Warn_String_Warn_Warg_struct _T35;_T35.tag=0;_T35.f1=_tag_fat("Tcutil found a bad datatypefield",sizeof(char),33U);_T13=_T35;}{struct Cyc_Warn_String_Warn_Warg_struct _T35=_T13;void*_T36[1];_T14=_T36 + 0;*_T14=& _T35;_T16=Cyc_Warn_impos2;{int(*_T37)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T16;_T15=_T37;}_T17=_tag_fat(_T36,sizeof(void*),1);_T15(_T17);}goto _TL91;_TL90: _TL91: _T18=fs;_T19=_T18->hd;_T1A=(struct Cyc_Absyn_Datatypefield*)_T19;_T1B=_T1A->name;_T1C=fname;_T1D=
Cyc_Absyn_qvar_cmp(_T1B,_T1C);if(_T1D!=0)goto _TL92;_T1E=fs;_T1F=_T1E->hd;
tuf=(struct Cyc_Absyn_Datatypefield*)_T1F;goto _TL8E;_TL92: _T20=fs;
# 451
fs=_T20->tl;goto _TL8F;_TL8E:;}_T21=info;
# 458
*_T21=Cyc_Absyn_KnownDatatypefield(tud,tuf);_T31=tud;_T30=tuf;goto _LL4;}}_TL8A: _T22=_T2F.KnownDatatypefield;_T23=_T22.val;_T31=_T23.f0;_T24=_T2F.KnownDatatypefield;_T25=_T24.val;_T30=_T25.f1;_LL4:{struct Cyc_Absyn_Datatypedecl*tud=_T31;struct Cyc_Absyn_Datatypefield*tuf=_T30;_T26=env;_T27=targsp;_T28=tud;_T29=_T28->tvs;_T2A=expected_kind;_T2B=allow_abs_aggr;_T2C=tuf;_T2D=_T2C->name;_T2E=
# 461
Cyc_Tctyp_check_type_inst(_T26,_T27,_T29,_T2A,_T2B,_T2D,0);return _T2E;};}}
# 466
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){int _T0;struct Cyc_Warn_String_Warn_Warg_struct _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;void**_T3;void**_T4;int(*_T5)(struct _fat_ptr);void*(*_T6)(struct _fat_ptr);struct _fat_ptr _T7;struct Cyc_Tctyp_CVTEnv _T8;struct Cyc_Absyn_Kind*_T9;struct Cyc_List_List*_TA;void*_TB;int _TC;struct Cyc_Tctyp_CVTEnv _TD;_T0=
# 468
Cyc_List_length(ts);if(_T0==1)goto _TL94;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;
_TE.f1=s;_T1=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T1;{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;_TF.f1=_tag_fat(" has wrong number of arguments",sizeof(char),31U);_T2=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_T2;void*_T10[2];_T3=_T10 + 0;*_T3=& _TE;_T4=_T10 + 1;*_T4=& _TF;_T6=Cyc_Warn_impos2;{int(*_T11)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6;_T5=_T11;}_T7=_tag_fat(_T10,sizeof(void*),2);_T5(_T7);}}goto _TL95;_TL94: _TL95: _T8=env;_T9=k;_TA=
_check_null(ts);_TB=_TA->hd;_TC=put_in_effect;_TD=Cyc_Tctyp_i_check_type(_T8,_T9,_TB,_TC,1);return _TD;}
# 473
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr,void*apptype){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_List_List**_T2;void*_T3;int*_T4;unsigned _T5;struct Cyc_Warn_Typ_Warn_Warg_struct _T6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;void**_T9;void**_TA;int(*_TB)(struct _fat_ptr);void*(*_TC)(struct _fat_ptr);struct _fat_ptr _TD;struct Cyc_Tctyp_CVTEnv _TE;struct Cyc_Warn_Typ_Warn_Warg_struct _TF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10;struct Cyc_Warn_String_Warn_Warg_struct _T11;void**_T12;void**_T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;struct Cyc_Absyn_Kind*_T17;struct Cyc_Absyn_Kind*_T18;struct Cyc_Absyn_Kind*_T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;struct Cyc_Warn_Kind_Warn_Warg_struct _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;void**_T1D;void**_T1E;void**_T1F;unsigned _T20;struct _fat_ptr _T21;struct Cyc_Tctyp_CVTEnv _T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_Warn_String_Warn_Warg_struct _T25;void**_T26;int(*_T27)(struct _fat_ptr);void*(*_T28)(struct _fat_ptr);struct _fat_ptr _T29;struct Cyc_Tctyp_CVTEnv _T2A;struct Cyc_Absyn_Kind*_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_List_List*_T2D;void*_T2E;struct Cyc_Tctyp_CVTEnv _T2F;int _T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;void**_T32;int(*_T33)(struct _fat_ptr);void*(*_T34)(struct _fat_ptr);struct _fat_ptr _T35;struct Cyc_Tctyp_CVTEnv _T36;struct Cyc_Absyn_Kind*_T37;struct Cyc_Absyn_Kind*_T38;struct Cyc_List_List*_T39;void*_T3A;struct Cyc_Tctyp_CVTEnv _T3B;struct Cyc_Tcenv_Tenv*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;void*_T3F;void*_T40;unsigned _T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;void*_T45;struct Cyc_Tctyp_CVTEnv _T46;struct Cyc_Absyn_Kind*_T47;struct Cyc_Absyn_Kind*_T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct _tuple14*_T4E;struct Cyc_Tctyp_CVTEnv _T4F;struct Cyc_Tctyp_CVTEnv _T50;struct Cyc_Tctyp_CVTEnv _T51;struct Cyc_Absyn_Kind*_T52;struct Cyc_Absyn_Kind*_T53;struct Cyc_List_List*_T54;void*_T55;struct Cyc_List_List*_T56;struct Cyc_Tctyp_CVTEnv _T57;struct Cyc_Tctyp_CVTEnv _T58;struct Cyc_Absyn_Kind*_T59;struct Cyc_Absyn_Kind*_T5A;struct Cyc_List_List*_T5B;void*_T5C;struct Cyc_List_List*_T5D;struct Cyc_Tctyp_CVTEnv _T5E;void*_T5F;void*_T60;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T61;struct Cyc_Absyn_Enumdecl**_T62;struct Cyc_Warn_String_Warn_Warg_struct _T63;void**_T64;int(*_T65)(struct _fat_ptr);void*(*_T66)(struct _fat_ptr);struct _fat_ptr _T67;struct Cyc_Absyn_Enumdecl**_T68;struct Cyc_Absyn_Enumdecl*_T69;struct Cyc_Absyn_Enumdecl**_T6A;struct Cyc_Absyn_Enumdecl*_T6B;struct Cyc_Core_Opt*_T6C;struct _handler_cons*_T6D;int*_T6E;int _T6F;struct Cyc_Absyn_Enumdecl**_T70;struct Cyc_Absyn_Enumdecl**_T71;void*_T72;struct Cyc_Dict_Absent_exn_struct*_T73;char*_T74;char*_T75;struct Cyc_Tcenv_Tenv*_T76;unsigned _T77;struct Cyc_Absyn_Enumdecl*_T78;struct Cyc_Absyn_Enumdecl**_T79;struct Cyc_Absyn_Enumdecl**_T7A;struct Cyc_Tctyp_CVTEnv _T7B;void*_T7C;struct Cyc_Warn_String_Warn_Warg_struct _T7D;void**_T7E;int(*_T7F)(struct _fat_ptr);void*(*_T80)(struct _fat_ptr);struct _fat_ptr _T81;struct Cyc_List_List*_T82;void*_T83;int(*_T84)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T85)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T86)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T87;struct Cyc_Absyn_Enumfield*_T88;struct _tuple1*_T89;struct _tuple1 _T8A;struct _fat_ptr*_T8B;int _T8C;struct Cyc_Warn_String_Warn_Warg_struct _T8D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T8E;struct Cyc_Absyn_Enumfield*_T8F;void**_T90;void**_T91;struct Cyc_Absyn_Enumfield*_T92;unsigned _T93;struct _fat_ptr _T94;struct Cyc_List_List*_T95;struct Cyc_Absyn_Enumfield*_T96;struct _tuple1*_T97;struct _tuple1 _T98;struct Cyc_Absyn_Enumfield*_T99;struct Cyc_Absyn_Exp*_T9A;struct Cyc_Absyn_Enumfield*_T9B;unsigned _T9C;struct Cyc_Absyn_Enumfield*_T9D;unsigned _T9E;struct Cyc_Absyn_Enumfield*_T9F;struct Cyc_Absyn_Exp*_TA0;int _TA1;struct Cyc_Warn_String_Warn_Warg_struct _TA2;struct Cyc_Warn_Qvar_Warn_Warg_struct _TA3;struct Cyc_Absyn_Enumfield*_TA4;struct Cyc_Warn_String_Warn_Warg_struct _TA5;void**_TA6;void**_TA7;void**_TA8;unsigned _TA9;struct _fat_ptr _TAA;struct Cyc_Absyn_Enumfield*_TAB;struct Cyc_Absyn_Exp*_TAC;struct Cyc_Absyn_Exp*_TAD;struct _tuple13 _TAE;unsigned _TAF;struct Cyc_List_List*_TB0;struct Cyc_Tctyp_CVTEnv _TB1;struct Cyc_Tctyp_CVTEnv _TB2;struct Cyc_Absyn_Kind*_TB3;struct Cyc_Absyn_Kind*_TB4;struct Cyc_List_List*_TB5;struct _fat_ptr _TB6;struct Cyc_Tctyp_CVTEnv _TB7;struct Cyc_Tctyp_CVTEnv _TB8;struct Cyc_Absyn_Kind*_TB9;struct Cyc_Absyn_Kind*_TBA;struct Cyc_List_List*_TBB;struct _fat_ptr _TBC;struct Cyc_Tctyp_CVTEnv _TBD;struct Cyc_Tctyp_CVTEnv _TBE;struct Cyc_Absyn_Kind*_TBF;struct Cyc_Absyn_Kind*_TC0;struct Cyc_List_List*_TC1;struct _fat_ptr _TC2;struct Cyc_Tctyp_CVTEnv _TC3;struct Cyc_Tctyp_CVTEnv _TC4;struct Cyc_Absyn_Kind*_TC5;struct Cyc_Absyn_Kind*_TC6;struct Cyc_List_List*_TC7;struct _fat_ptr _TC8;struct Cyc_Tctyp_CVTEnv _TC9;struct Cyc_Tctyp_CVTEnv _TCA;struct Cyc_Absyn_Kind*_TCB;struct Cyc_Absyn_Kind*_TCC;struct Cyc_List_List*_TCD;struct _fat_ptr _TCE;struct Cyc_Tctyp_CVTEnv _TCF;void*_TD0;void*_TD1;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TD2;union Cyc_Absyn_AggrInfo*_TD3;struct Cyc_Tctyp_CVTEnv _TD4;void*_TD5;void*_TD6;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD7;union Cyc_Absyn_DatatypeInfo*_TD8;struct Cyc_Tctyp_CVTEnv _TD9;void*_TDA;void*_TDB;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TDC;union Cyc_Absyn_DatatypeFieldInfo*_TDD;struct Cyc_Tctyp_CVTEnv _TDE;struct Cyc_Tcenv_Tenv*_TDF;unsigned _TE0;_T0=env;_TE0=_T0.loc;_T1=env;_TDF=_T1.te;{unsigned loc=_TE0;struct Cyc_Tcenv_Tenv*te=_TDF;_T2=targsp;{
# 477
struct Cyc_List_List*ts=*_T2;void*_TE1;void*_TE2;_T3=c;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 1: goto _LL7;case 2: _LL7: goto _LL9;case 0: _LL9: goto _LLB;case 8: _LLB: goto _LLD;case 7: _LLD: goto _LLF;case 6: _LLF: goto _LL11;case 12: _LL11: goto _LL13;case 11: _LL13: goto _LL15;case 14: _LL15: goto _LL17;case 21: _LL17:
# 484
 if(ts==0)goto _TL97;{struct Cyc_Warn_Typ_Warn_Warg_struct _TE3;_TE3.tag=2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE4=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_TE4->tag=0;
_TE4->f1=c;_TE4->f2=ts;_T7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE4;}_TE3.f1=(void*)_T7;_T6=_TE3;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TE3=_T6;{struct Cyc_Warn_String_Warn_Warg_struct _TE4;_TE4.tag=0;_TE4.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_T8=_TE4;}{struct Cyc_Warn_String_Warn_Warg_struct _TE4=_T8;void*_TE5[2];_T9=_TE5 + 0;*_T9=& _TE3;_TA=_TE5 + 1;*_TA=& _TE4;_TC=Cyc_Warn_impos2;{int(*_TE6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TC;_TB=_TE6;}_TD=_tag_fat(_TE5,sizeof(void*),2);_TB(_TD);}}goto _TL98;_TL97: _TL98: _TE=env;
return _TE;case 16:
# 488
 if(ts==0)goto _TL99;{struct Cyc_Warn_Typ_Warn_Warg_struct _TE3;_TE3.tag=2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE4=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_TE4->tag=0;
_TE4->f1=c;_TE4->f2=ts;_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE4;}_TE3.f1=(void*)_T10;_TF=_TE3;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TE3=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _TE4;_TE4.tag=0;_TE4.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_T11=_TE4;}{struct Cyc_Warn_String_Warn_Warg_struct _TE4=_T11;void*_TE5[2];_T12=_TE5 + 0;*_T12=& _TE3;_T13=_TE5 + 1;*_T13=& _TE4;_T15=Cyc_Warn_impos2;{int(*_TE6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_TE6;}_T16=_tag_fat(_TE5,sizeof(void*),2);_T14(_T16);}}goto _TL9A;_TL99: _TL9A: _T17=expected_kind;_T18=& Cyc_Kinds_aqk;_T19=(struct Cyc_Absyn_Kind*)_T18;
if(_T17==_T19)goto _TL9B;{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;
_TE3.f1=_tag_fat("Kind Q used where kind ",sizeof(char),24U);_T1A=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T1A;{struct Cyc_Warn_Kind_Warn_Warg_struct _TE4;_TE4.tag=9;_TE4.f1=expected_kind;_T1B=_TE4;}{struct Cyc_Warn_Kind_Warn_Warg_struct _TE4=_T1B;{struct Cyc_Warn_String_Warn_Warg_struct _TE5;_TE5.tag=0;_TE5.f1=_tag_fat(" was expected",sizeof(char),14U);_T1C=_TE5;}{struct Cyc_Warn_String_Warn_Warg_struct _TE5=_T1C;void*_TE6[3];_T1D=_TE6 + 0;*_T1D=& _TE3;_T1E=_TE6 + 1;*_T1E=& _TE4;_T1F=_TE6 + 2;*_T1F=& _TE5;_T20=loc;_T21=_tag_fat(_TE6,sizeof(void*),3);Cyc_Warn_err2(_T20,_T21);}}}goto _TL9C;_TL9B: _TL9C: _T22=env;
return _T22;case 15:
# 494
 if(ts==0)goto _TL9F;else{goto _TLA0;}_TLA0: _T23=ts;_T24=_T23->tl;if(_T24!=0)goto _TL9F;else{goto _TL9D;}
_TL9F:{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("aquals expects a single argument",sizeof(char),33U);_T25=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T25;void*_TE4[1];_T26=_TE4 + 0;*_T26=& _TE3;_T28=Cyc_Warn_impos2;{int(*_TE5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T28;_T27=_TE5;}_T29=_tag_fat(_TE4,sizeof(void*),1);_T27(_T29);}goto _TL9E;_TL9D: _TL9E: _T2A=env;_T2B=& Cyc_Kinds_ak;_T2C=(struct Cyc_Absyn_Kind*)_T2B;_T2D=ts;_T2E=_T2D->hd;_T2F=
Cyc_Tctyp_i_check_type(_T2A,_T2C,_T2E,0,0);return _T2F;case 17: _T30=
# 498
Cyc_List_length(ts);if(_T30==2)goto _TLA1;{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;
_TE3.f1=_tag_fat("aquals expects two arguments",sizeof(char),29U);_T31=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T31;void*_TE4[1];_T32=_TE4 + 0;*_T32=& _TE3;_T34=Cyc_Warn_impos2;{int(*_TE5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T34;_T33=_TE5;}_T35=_tag_fat(_TE4,sizeof(void*),1);_T33(_T35);}goto _TLA2;_TLA1: _TLA2: _T36=env;_T37=& Cyc_Kinds_aqk;_T38=(struct Cyc_Absyn_Kind*)_T37;_T39=
_check_null(ts);_T3A=_T39->hd;env=Cyc_Tctyp_i_check_type(_T36,_T38,_T3A,0,0);_T3B=env;_T3C=_T3B.te;_T3D=
Cyc_Tcenv_curr_aquals_bounds(_T3C);_T3E=ts;_T3F=_T3E->hd;{void*ctx_bnd=Cyc_Tcutil_lookup_aquals_opt(_T3D,_T3F);_T40=ctx_bnd;_T41=(unsigned)_T40;
if(!_T41)goto _TLA3;_T42=ts;_T43=_T42->tl;_T44=
_check_null(_T43);_T45=ctx_bnd;_T44->hd=(void*)_T45;goto _TLA4;
# 506
_TLA3: _T46=env;_T47=& Cyc_Kinds_aqk;_T48=(struct Cyc_Absyn_Kind*)_T47;_T49=ts;_T4A=_T49->tl;_T4B=_check_null(_T4A);_T4C=_T4B->hd;env=Cyc_Tctyp_i_check_type(_T46,_T48,_T4C,0,0);_TLA4:{struct Cyc_List_List*_TE3=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple14*_TE4=_cycalloc(sizeof(struct _tuple14));
# 508
_TE4->f0=apptype;_TE4->f1=0;_T4E=(struct _tuple14*)_TE4;}_TE3->hd=_T4E;_T4F=env;_TE3->tl=_T4F.free_qualvars;_T4D=(struct Cyc_List_List*)_TE3;}env.free_qualvars=_T4D;_T50=env;
return _T50;}case 3:
# 511
 _TLA8: if(ts!=0)goto _TLA6;else{goto _TLA7;}
_TLA6: _T51=env;_T52=& Cyc_Kinds_mk;_T53=(struct Cyc_Absyn_Kind*)_T52;_T54=ts;_T55=_T54->hd;env=Cyc_Tctyp_i_check_type(_T51,_T53,_T55,1,1);_T56=ts;
# 511
ts=_T56->tl;goto _TLA8;_TLA7: _T57=env;
# 513
return _T57;case 9:
# 515
 _TLAC: if(ts!=0)goto _TLAA;else{goto _TLAB;}
_TLAA: _T58=env;_T59=& Cyc_Kinds_ek;_T5A=(struct Cyc_Absyn_Kind*)_T59;_T5B=ts;_T5C=_T5B->hd;env=Cyc_Tctyp_i_check_type(_T58,_T5A,_T5C,1,1);_T5D=ts;
# 515
ts=_T5D->tl;goto _TLAC;_TLAB: _T5E=env;
# 517
return _T5E;case 19: _T5F=c;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TE3=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T5F;_TE2=_TE3->f1;_T60=c;_T61=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T60;_T62=& _T61->f2;_TE1=(struct Cyc_Absyn_Enumdecl**)_T62;}{struct _tuple1*n=_TE2;struct Cyc_Absyn_Enumdecl**edp=(struct Cyc_Absyn_Enumdecl**)_TE1;
# 519
if(ts==0)goto _TLAD;{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_T63=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T63;void*_TE4[1];_T64=_TE4 + 0;*_T64=& _TE3;_T66=Cyc_Warn_impos2;{int(*_TE5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T66;_T65=_TE5;}_T67=_tag_fat(_TE4,sizeof(void*),1);_T65(_T67);}goto _TLAE;_TLAD: _TLAE: _T68=edp;_T69=*_T68;
if(_T69==0)goto _TLB1;else{goto _TLB2;}_TLB2: _T6A=edp;_T6B=*_T6A;_T6C=_T6B->fields;if(_T6C==0)goto _TLB1;else{goto _TLAF;}
_TLB1:{struct _handler_cons _TE3;_T6D=& _TE3;_push_handler(_T6D);{int _TE4=0;_T6E=_TE3.handler;_T6F=setjmp(_T6E);if(!_T6F)goto _TLB3;_TE4=1;goto _TLB4;_TLB3: _TLB4: if(_TE4)goto _TLB5;else{goto _TLB7;}_TLB7: _T70=edp;_T71=Cyc_Tcenv_lookup_enumdecl(te,loc,n);*_T70=*_T71;_pop_handler();goto _TLB6;_TLB5: _T72=Cyc_Core_get_exn_thrown();{void*_TE5=(void*)_T72;void*_TE6;_T73=(struct Cyc_Dict_Absent_exn_struct*)_TE5;_T74=_T73->tag;_T75=Cyc_Dict_Absent;if(_T74!=_T75)goto _TLB8;_T76=te;_T77=loc;{struct Cyc_Absyn_Enumdecl*_TE7=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
# 524
_TE7->sc=3U;_TE7->name=n;_TE7->fields=0;_T78=(struct Cyc_Absyn_Enumdecl*)_TE7;}Cyc_Tc_tcEnumdecl(_T76,_T77,_T78);_T79=edp;_T7A=
Cyc_Tcenv_lookup_enumdecl(te,loc,n);*_T79=*_T7A;goto _LL36;_TLB8: _TE6=_TE5;{void*exn=_TE6;_rethrow(exn);}_LL36:;}_TLB6:;}}goto _TLB0;_TLAF: _TLB0: _T7B=env;
# 529
return _T7B;}case 20: _T7C=c;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_TE3=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T7C;_TE2=_TE3->f1;}{struct Cyc_List_List*fs=_TE2;
# 531
if(ts==0)goto _TLBA;{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;
_TE3.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_T7D=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T7D;void*_TE4[1];_T7E=_TE4 + 0;*_T7E=& _TE3;_T80=Cyc_Warn_impos2;{int(*_TE5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T80;_T7F=_TE5;}_T81=_tag_fat(_TE4,sizeof(void*),1);_T7F(_T81);}goto _TLBB;_TLBA: _TLBB: {
# 534
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
_TLBF: if(fs!=0)goto _TLBD;else{goto _TLBE;}
_TLBD: _T82=fs;_T83=_T82->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T83;_T85=Cyc_List_mem;{
int(*_TE3)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T85;_T84=_TE3;}_T86=Cyc_strptrcmp;_T87=prev_fields;_T88=f;_T89=_T88->name;_T8A=*_T89;_T8B=_T8A.f1;_T8C=_T84(_T86,_T87,_T8B);if(!_T8C)goto _TLC0;{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;
_TE3.f1=_tag_fat("duplicate enum field name ",sizeof(char),27U);_T8D=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_T8D;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TE4;_TE4.tag=1;_T8F=f;_TE4.f1=_T8F->name;_T8E=_TE4;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TE4=_T8E;void*_TE5[2];_T90=_TE5 + 0;*_T90=& _TE3;_T91=_TE5 + 1;*_T91=& _TE4;_T92=f;_T93=_T92->loc;_T94=_tag_fat(_TE5,sizeof(void*),2);Cyc_Warn_err2(_T93,_T94);}}goto _TLC1;
# 541
_TLC0:{struct Cyc_List_List*_TE3=_cycalloc(sizeof(struct Cyc_List_List));_T96=f;_T97=_T96->name;_T98=*_T97;_TE3->hd=_T98.f1;_TE3->tl=prev_fields;_T95=(struct Cyc_List_List*)_TE3;}prev_fields=_T95;_TLC1: _T99=f;_T9A=_T99->tag;
if(_T9A!=0)goto _TLC2;_T9B=f;_T9C=tag_count;_T9D=f;_T9E=_T9D->loc;
_T9B->tag=Cyc_Absyn_uint_exp(_T9C,_T9E);goto _TLC3;
_TLC2: _T9F=f;_TA0=_T9F->tag;_TA1=Cyc_Tcutil_is_const_exp(_TA0);if(_TA1)goto _TLC4;else{goto _TLC6;}
_TLC6:{struct Cyc_Warn_String_Warn_Warg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("enum field ",sizeof(char),12U);_TA2=_TE3;}{struct Cyc_Warn_String_Warn_Warg_struct _TE3=_TA2;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TE4;_TE4.tag=1;_TA4=f;_TE4.f1=_TA4->name;_TA3=_TE4;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TE4=_TA3;{struct Cyc_Warn_String_Warn_Warg_struct _TE5;_TE5.tag=0;_TE5.f1=_tag_fat(": expression is not constant",sizeof(char),29U);_TA5=_TE5;}{struct Cyc_Warn_String_Warn_Warg_struct _TE5=_TA5;void*_TE6[3];_TA6=_TE6 + 0;*_TA6=& _TE3;_TA7=_TE6 + 1;*_TA7=& _TE4;_TA8=_TE6 + 2;*_TA8=& _TE5;_TA9=loc;_TAA=_tag_fat(_TE6,sizeof(void*),3);Cyc_Warn_err2(_TA9,_TAA);}}}goto _TLC5;_TLC4: _TLC5: _TLC3: _TAB=f;_TAC=_TAB->tag;_TAD=
_check_null(_TAC);_TAE=Cyc_Evexp_eval_const_uint_exp(_TAD);_TAF=_TAE.f0;tag_count=1U + _TAF;}_TB0=fs;
# 536
fs=_TB0->tl;goto _TLBF;_TLBE: _TB1=env;
# 548
return _TB1;}}case 10: _TB2=env;_TB3=& Cyc_Kinds_ak;_TB4=(struct Cyc_Absyn_Kind*)_TB3;_TB5=ts;_TB6=
_tag_fat("regions",sizeof(char),8U);_TB7=Cyc_Tctyp_i_check_unary_app(_TB2,_TB4,_TB5,1,_TB6);return _TB7;case 4: _TB8=env;_TB9=& Cyc_Kinds_ek;_TBA=(struct Cyc_Absyn_Kind*)_TB9;_TBB=ts;_TBC=
_tag_fat("region_t",sizeof(char),9U);_TBD=Cyc_Tctyp_i_check_unary_app(_TB8,_TBA,_TBB,1,_TBC);return _TBD;case 18: _TBE=env;_TBF=& Cyc_Kinds_aqk;_TC0=(struct Cyc_Absyn_Kind*)_TBF;_TC1=ts;_TC2=
# 552
_tag_fat("aqual_t",sizeof(char),8U);_TC3=Cyc_Tctyp_i_check_unary_app(_TBE,_TC0,_TC1,1,_TC2);return _TC3;case 5: _TC4=env;_TC5=& Cyc_Kinds_ik;_TC6=(struct Cyc_Absyn_Kind*)_TC5;_TC7=ts;_TC8=
# 554
_tag_fat("tag_t",sizeof(char),6U);_TC9=Cyc_Tctyp_i_check_unary_app(_TC4,_TC6,_TC7,0,_TC8);return _TC9;case 13: _TCA=env;_TCB=& Cyc_Kinds_ik;_TCC=(struct Cyc_Absyn_Kind*)_TCB;_TCD=ts;_TCE=
_tag_fat("@thin",sizeof(char),6U);_TCF=Cyc_Tctyp_i_check_unary_app(_TCA,_TCC,_TCD,0,_TCE);return _TCF;case 24: _TD0=c;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TE3=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TD0;_TD1=c;_TD2=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TD1;_TD3=& _TD2->f1;_TE2=(union Cyc_Absyn_AggrInfo*)_TD3;}{union Cyc_Absyn_AggrInfo*info=_TE2;_TD4=
# 557
Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);return _TD4;}case 22: _TD5=c;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TE3=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TD5;_TD6=c;_TD7=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TD6;_TD8=& _TD7->f1;_TE2=(union Cyc_Absyn_DatatypeInfo*)_TD8;}{union Cyc_Absyn_DatatypeInfo*info=_TE2;_TD9=
# 559
Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);return _TD9;}default: _TDA=c;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TE3=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TDA;_TDB=c;_TDC=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TDB;_TDD=& _TDC->f1;_TE2=(union Cyc_Absyn_DatatypeFieldInfo*)_TDD;}{union Cyc_Absyn_DatatypeFieldInfo*info=(union Cyc_Absyn_DatatypeFieldInfo*)_TE2;_TDE=
# 561
Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);return _TDE;}};}}}
# 565
static int Cyc_Tctyp_bnd_qual_cmp(void*tvt1,void*tvt2){struct _tuple0 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;int _T9;void*_TA;int*_TB;unsigned _TC;void*_TD;void*_TE;int _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Core_Opt*_T13;void*_T14;struct Cyc_Absyn_Kind*_T15;int _T16;void*_T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;struct Cyc_List_List*_T1E;void*_T1F;int*_T20;unsigned _T21;void*_T22;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T23;void*_T24;int*_T25;int _T26;void*_T27;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_List_List*_T2B;void*_T2C;void*_T2D;struct Cyc_List_List*_T2E;void*_T2F;int _T30;void*_T31;int*_T32;unsigned _T33;void*_T34;int*_T35;unsigned _T36;struct Cyc_Warn_String_Warn_Warg_struct _T37;struct Cyc_Warn_Typ_Warn_Warg_struct _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;struct Cyc_Warn_Typ_Warn_Warg_struct _T3A;void**_T3B;void**_T3C;void**_T3D;void**_T3E;int(*_T3F)(struct _fat_ptr);void*(*_T40)(struct _fat_ptr);struct _fat_ptr _T41;{struct _tuple0 _T42;
_T42.f0=Cyc_Absyn_compress(tvt1);_T42.f1=Cyc_Absyn_compress(tvt2);_T0=_T42;}{struct _tuple0 _T42=_T0;int _T43;int _T44;void*_T45;void*_T46;_T1=_T42.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 2: _T4=_T42.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 2: _T7=_T42.f0;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T7;_T46=_T47->f1;}_T8=_T42.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T8;_T45=_T47->f1;}{struct Cyc_Absyn_Tvar*tv1=_T46;struct Cyc_Absyn_Tvar*tv2=_T45;_T9=
# 568
Cyc_Absyn_tvar_cmp(tv1,tv2);return _T9;}case 1: goto _LLA;case 0: _LL10: goto _LL12;default: goto _LL13;};case 1: _TA=_T42.f1;_TB=(int*)_TA;_TC=*_TB;switch(_TC){case 1: _TD=_T42.f0;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T47=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TD;_T46=_T47->f1;_T44=_T47->f3;}_TE=_T42.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T47=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TE;_T45=_T47->f1;_T43=_T47->f3;}{struct Cyc_Core_Opt*k1=_T46;int i1=_T44;struct Cyc_Core_Opt*k2=_T45;int i2=_T43;
# 570
if(i1!=i2)goto _TLCA;_T10=_check_null(k1);_T11=_T10->v;_T12=(struct Cyc_Absyn_Kind*)_T11;_T13=_check_null(k2);_T14=_T13->v;_T15=(struct Cyc_Absyn_Kind*)_T14;_T16=Cyc_Kinds_kind_eq(_T12,_T15);if(!_T16)goto _TLCA;_TF=0;goto _TLCB;_TLCA: _TF=1;_TLCB: return _TF;}case 2: _LLA: goto _LLC;case 0: _LLC: goto _LLE;default: goto _LL13;};case 0: _T17=_T42.f0;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=15)goto _TLCC;_T1C=_T42.f0;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f2;if(_T1E==0)goto _TLCE;_T1F=_T42.f1;_T20=(int*)_T1F;_T21=*_T20;switch(_T21){case 0: _T22=_T42.f1;_T23=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T24=_T23->f1;_T25=(int*)_T24;_T26=*_T25;if(_T26!=15)goto _TLD1;_T27=_T42.f1;_T28=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27;_T29=_T28->f2;if(_T29==0)goto _TLD3;_T2A=_T42.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2A;_T2B=_T47->f2;{struct Cyc_List_List _T48=*_T2B;_T2C=_T48.hd;_T46=(void*)_T2C;}}_T2D=_T42.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T47=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T2E=_T47->f2;{struct Cyc_List_List _T48=*_T2E;_T2F=_T48.hd;_T45=(void*)_T2F;}}{void*tv11=_T46;void*tv21=_T45;_T30=
# 572
Cyc_Tctyp_bnd_qual_cmp(tv11,tv21);return _T30;}_TLD3: goto _LL13;_TLD1: goto _LL13;case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}goto _TLCF;_TLCE: _T31=_T42.f1;_T32=(int*)_T31;_T33=*_T32;switch(_T33){case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}_TLCF: goto _TLCD;_TLCC: _T34=_T42.f1;_T35=(int*)_T34;_T36=*_T35;switch(_T36){case 1: _LLD: _LLE: goto _LL10;case 2: _LL11: _LL12:
# 579
 return 1;default: goto _LL13;}_TLCD:;default: _LL13:{struct Cyc_Warn_String_Warn_Warg_struct _T47;_T47.tag=0;
_T47.f1=_tag_fat("Expect only tvars, evars or aquals in qualifier bounds: ",sizeof(char),57U);_T37=_T47;}{struct Cyc_Warn_String_Warn_Warg_struct _T47=_T37;{struct Cyc_Warn_Typ_Warn_Warg_struct _T48;_T48.tag=2;_T48.f1=tvt1;_T38=_T48;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T48=_T38;{struct Cyc_Warn_String_Warn_Warg_struct _T49;_T49.tag=0;_T49.f1=_tag_fat(",",sizeof(char),2U);_T39=_T49;}{struct Cyc_Warn_String_Warn_Warg_struct _T49=_T39;{struct Cyc_Warn_Typ_Warn_Warg_struct _T4A;_T4A.tag=2;_T4A.f1=tvt2;_T3A=_T4A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T4A=_T3A;void*_T4B[4];_T3B=_T4B + 0;*_T3B=& _T47;_T3C=_T4B + 1;*_T3C=& _T48;_T3D=_T4B + 2;*_T3D=& _T49;_T3E=_T4B + 3;*_T3E=& _T4A;_T40=Cyc_Warn_impos2;{int(*_T4C)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T40;_T3F=_T4C;}_T41=_tag_fat(_T4B,sizeof(void*),4);_T3F(_T41);}}}}};}}
# 585
static int Cyc_Tctyp_aqualvar_unconstrained(struct _tuple14*t){struct _tuple14*_T0;struct _tuple14 _T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;struct _tuple14*_TB;struct _tuple14 _TC;void**_TD;void**_TE;int(*_TF)(struct _fat_ptr);void*(*_T10)(struct _fat_ptr);struct _fat_ptr _T11;_T0=t;_T1=*_T0;_T2=_T1.f0;{
void*_T12=Cyc_Absyn_compress(_T2);_T3=(int*)_T12;_T4=*_T3;if(_T4!=0)goto _TLD7;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 16:
# 588
 return 0;case 17:
# 590
 return 1;default: goto _LL5;}goto _TLD8;_TLD7: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T13;_T13.tag=0;
# 592
_T13.f1=_tag_fat("Expect only Aqual types -- got ",sizeof(char),32U);_T9=_T13;}{struct Cyc_Warn_String_Warn_Warg_struct _T13=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T14;_T14.tag=2;_TB=t;_TC=*_TB;_T14.f1=_TC.f0;_TA=_T14;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T14=_TA;void*_T15[2];_TD=_T15 + 0;*_TD=& _T13;_TE=_T15 + 1;*_TE=& _T14;_T10=Cyc_Warn_impos2;{int(*_T16)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T10;_TF=_T16;}_T11=_tag_fat(_T15,sizeof(void*),2);_TF(_T11);}}_TLD8:;}}
# 596
static int Cyc_Tctyp_aqualsof_var_bnd(void*bndv,void*var){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;int _TE;void*_TF;_T0=bndv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TLDA;_T3=bndv;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=15)goto _TLDC;_T8=bndv;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_TA=_T9->f2;if(_TA==0)goto _TLDE;_TB=bndv;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB;_TC=_T10->f2;{struct Cyc_List_List _T11=*_TC;_TD=_T11.hd;_TF=(void*)_TD;}}{void*vv=_TF;_TE=
# 599
Cyc_Tcutil_typecmp(var,vv);return _TE;}_TLDE: goto _LL3;_TLDC: goto _LL3;_TLDA: _LL3:
# 601
 return 1;;}
# 606
static void Cyc_Tctyp_constrain_vars(unsigned loc,struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List*vars,struct Cyc_List_List**qb,void*def,int make_ex_aliasable){struct Cyc_List_List*_T0;void*_T1;int _T2;void*_T3;int*_T4;unsigned _T5;void*_T6;struct Cyc_Absyn_Tvar*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;struct Cyc_Absyn_Kind*_TA;struct Cyc_Core_Opt _TB;void*_TC;struct Cyc_Absyn_Kind*_TD;unsigned _TE;struct Cyc_Core_Opt _TF;void*_T10;struct Cyc_Absyn_Kind*_T11;void*_T12;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T13;struct Cyc_Core_Opt*_T14;void*_T15;struct Cyc_Core_Opt*_T16;void*_T17;struct Cyc_Absyn_Kind*_T18;enum Cyc_Absyn_KindQual _T19;int _T1A;struct Cyc_Absyn_Kind*_T1B;enum Cyc_Absyn_KindQual _T1C;int _T1D;struct Cyc_Absyn_Kind*_T1E;enum Cyc_Absyn_KindQual _T1F;int _T20;struct _handler_cons*_T21;int*_T22;int _T23;struct Cyc_List_List**_T24;struct Cyc_List_List*_T25;void*_T26;int _T27;int _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Typ_Warn_Warg_struct _T2A;void**_T2B;void**_T2C;unsigned _T2D;struct _fat_ptr _T2E;void*_T2F;struct Cyc_Core_Not_found_exn_struct*_T30;char*_T31;char*_T32;int _T33;int _T34;struct Cyc_List_List**_T35;struct Cyc_List_List*_T36;struct _tuple0*_T37;struct Cyc_List_List**_T38;struct Cyc_List_List**_T39;struct Cyc_List_List*_T3A;struct _tuple0*_T3B;struct Cyc_List_List**_T3C;struct Cyc_Absyn_Kind*_T3D;enum Cyc_Absyn_KindQual _T3E;int _T3F;struct _handler_cons*_T40;int*_T41;int _T42;struct Cyc_List_List**_T43;struct Cyc_List_List*_T44;void*_T45;int _T46;int _T47;struct Cyc_Warn_String_Warn_Warg_struct _T48;struct Cyc_Warn_Typ_Warn_Warg_struct _T49;void**_T4A;void**_T4B;unsigned _T4C;struct _fat_ptr _T4D;void*_T4E;struct Cyc_Core_Not_found_exn_struct*_T4F;char*_T50;char*_T51;int _T52;int _T53;struct Cyc_List_List**_T54;struct Cyc_List_List*_T55;struct _tuple0*_T56;struct Cyc_List_List**_T57;struct Cyc_List_List**_T58;struct Cyc_List_List*_T59;struct _tuple0*_T5A;struct Cyc_List_List**_T5B;void*_T5C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5D;void*_T5E;int*_T5F;int _T60;void*_T61;struct Cyc_List_List*_T62;void*_T63;int*_T64;int _T65;void*_T66;struct Cyc_Absyn_Tvar*_T67;void*_T68;unsigned _T69;struct Cyc_Absyn_Tvar*_T6A;struct _handler_cons*_T6B;int*_T6C;int _T6D;struct Cyc_List_List**_T6E;struct Cyc_List_List*_T6F;void*_T70;int _T71;int _T72;struct Cyc_Warn_String_Warn_Warg_struct _T73;struct Cyc_Warn_Typ_Warn_Warg_struct _T74;void**_T75;void**_T76;unsigned _T77;struct _fat_ptr _T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;struct Cyc_List_List*_T7B;void*_T7C;void*_T7D;struct Cyc_Core_Not_found_exn_struct*_T7E;char*_T7F;char*_T80;int*_T81;unsigned _T82;int _T83;int _T84;struct Cyc_List_List**_T85;struct Cyc_List_List*_T86;struct _tuple0*_T87;struct Cyc_List_List**_T88;struct Cyc_List_List*_T89;struct Cyc_List_List*_T8A;struct Cyc_List_List*_T8B;void*_T8C;struct Cyc_List_List**_T8D;struct Cyc_List_List*_T8E;struct _tuple0*_T8F;struct Cyc_List_List**_T90;struct Cyc_List_List*_T91;struct Cyc_List_List*_T92;struct Cyc_List_List*_T93;void*_T94;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T95;void*_T96;int*_T97;int _T98;struct Cyc_Warn_String_Warn_Warg_struct _T99;void**_T9A;int(*_T9B)(struct _fat_ptr);void*(*_T9C)(struct _fat_ptr);struct _fat_ptr _T9D;struct Cyc_Warn_String_Warn_Warg_struct _T9E;struct Cyc_Warn_Typ_Warn_Warg_struct _T9F;void**_TA0;void**_TA1;int(*_TA2)(struct _fat_ptr);void*(*_TA3)(struct _fat_ptr);struct _fat_ptr _TA4;struct Cyc_List_List*_TA5;
# 610
_TLE3: if(vars!=0)goto _TLE1;else{goto _TLE2;}
_TLE1: _T0=vars;_T1=_T0->hd;{struct _tuple14*_TA6=(struct _tuple14*)_T1;int _TA7;void*_TA8;{struct _tuple14 _TA9=*_TA6;_TA8=_TA9.f0;_TA7=_TA9.f1;}{void*v=_TA8;int existential=_TA7;_T2=make_ex_aliasable;
if(_T2)goto _TLE4;else{goto _TLE6;}
_TLE6: existential=0;goto _TLE5;_TLE4: _TLE5: {
void*adef=def;
int explicit_bound_set=0;struct Cyc_List_List*_TA9;struct Cyc_Absyn_Kind*_TAA;struct Cyc_Absyn_Tvar*_TAB;_T3=v;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 2: _T6=v;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6;_TAB=_TAC->f1;}{struct Cyc_Absyn_Tvar*tv=_TAB;_T7=tv;_T8=_T7->aquals_bound;
# 618
if(_T8==0)goto _TLE8;_T9=tv;
adef=_T9->aquals_bound;
explicit_bound_set=1;goto _TLE9;_TLE8: _TLE9: {
# 622
struct Cyc_Core_Opt ko=Cyc_Kinds_tvar_kind_opt(tv);_TB=ko;_TC=_TB.v;_TD=(struct Cyc_Absyn_Kind*)_TC;_TE=(unsigned)_TD;
if(!_TE)goto _TLEA;_TF=ko;_T10=_TF.v;_TA=(struct Cyc_Absyn_Kind*)_T10;goto _TLEB;_TLEA: _T11=& Cyc_Kinds_ek;_TA=(struct Cyc_Absyn_Kind*)_T11;_TLEB: _TAA=_TA;goto _LL7;}}case 1: _T12=v;_T13=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T12;_T14=_T13->f1;if(_T14==0)goto _TLEC;_T15=v;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TAC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T15;_T16=_TAC->f1;{struct Cyc_Core_Opt _TAD=*_T16;_T17=_TAD.v;_TAA=(struct Cyc_Absyn_Kind*)_T17;}}_LL7:{struct Cyc_Absyn_Kind*k=_TAA;_T18=k;_T19=_T18->kind;_T1A=(int)_T19;
# 625
if(_T1A==2)goto _TLF0;else{goto _TLF2;}_TLF2: _T1B=k;_T1C=_T1B->kind;_T1D=(int)_T1C;if(_T1D==0)goto _TLF0;else{goto _TLF1;}_TLF1: _T1E=k;_T1F=_T1E->kind;_T20=(int)_T1F;if(_T20==1)goto _TLF0;else{goto _TLEE;}
_TLF0:{struct _handler_cons _TAC;_T21=& _TAC;_push_handler(_T21);{int _TAD=0;_T22=_TAC.handler;_T23=setjmp(_T22);if(!_T23)goto _TLF3;_TAD=1;goto _TLF4;_TLF3: _TLF4: if(_TAD)goto _TLF5;else{goto _TLF7;}_TLF7: _T24=qb;_T25=*_T24;_T26=v;{
void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_aqualsof_var_bnd,_T25,_T26);_T27=explicit_bound_set;
if(!_T27)goto _TLF8;_T28=Cyc_Tcutil_typecmp(adef,xbnd);if(!_T28)goto _TLF8;{struct Cyc_Warn_String_Warn_Warg_struct _TAE;_TAE.tag=0;
_TAE.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T29=_TAE;}{struct Cyc_Warn_String_Warn_Warg_struct _TAE=_T29;{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF;_TAF.tag=2;_TAF.f1=v;_T2A=_TAF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF=_T2A;void*_TB0[2];_T2B=_TB0 + 0;*_T2B=& _TAE;_T2C=_TB0 + 1;*_T2C=& _TAF;_T2D=loc;_T2E=_tag_fat(_TB0,sizeof(void*),2);Cyc_Warn_warn2(_T2D,_T2E);}}goto _TLF9;_TLF8: _TLF9:;}_pop_handler();goto _TLF6;_TLF5: _T2F=Cyc_Core_get_exn_thrown();{void*_TAE=(void*)_T2F;void*_TAF;_T30=(struct Cyc_Core_Not_found_exn_struct*)_TAE;_T31=_T30->tag;_T32=Cyc_Core_Not_found;if(_T31!=_T32)goto _TLFA;{
# 632
void*aqt=Cyc_Absyn_aqualsof_type(v);_T33=existential;
if(!_T33)goto _TLFC;_T34=explicit_bound_set;if(_T34)goto _TLFC;else{goto _TLFE;}
_TLFE: _T35=qb;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB1=_cycalloc(sizeof(struct _tuple0));_TB1->f0=aqt;_TB1->f1=Cyc_Absyn_al_qual_type;_T37=(struct _tuple0*)_TB1;}_TB0->hd=_T37;_T38=qb;_TB0->tl=*_T38;_T36=(struct Cyc_List_List*)_TB0;}*_T35=_T36;goto _TLFD;
# 636
_TLFC: _T39=qb;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB1=_cycalloc(sizeof(struct _tuple0));_TB1->f0=aqt;_TB1->f1=adef;_T3B=(struct _tuple0*)_TB1;}_TB0->hd=_T3B;_T3C=qb;_TB0->tl=*_T3C;_T3A=(struct Cyc_List_List*)_TB0;}*_T39=_T3A;_TLFD: goto _LLC;}_TLFA: _TAF=_TAE;{void*exn=_TAF;_rethrow(exn);}_LLC:;}_TLF6:;}}goto _TLEF;
# 640
_TLEE: _T3D=k;_T3E=_T3D->kind;_T3F=(int)_T3E;if(_T3F!=7)goto _TLFF;{struct _handler_cons _TAC;_T40=& _TAC;_push_handler(_T40);{int _TAD=0;_T41=_TAC.handler;_T42=setjmp(_T41);if(!_T42)goto _TL101;_TAD=1;goto _TL102;_TL101: _TL102: if(_TAD)goto _TL103;else{goto _TL105;}_TL105: _T43=qb;_T44=*_T43;_T45=v;{
# 642
void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,_T44,_T45);_T46=explicit_bound_set;
if(!_T46)goto _TL106;_T47=Cyc_Tcutil_typecmp(adef,xbnd);if(!_T47)goto _TL106;{struct Cyc_Warn_String_Warn_Warg_struct _TAE;_TAE.tag=0;
_TAE.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T48=_TAE;}{struct Cyc_Warn_String_Warn_Warg_struct _TAE=_T48;{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF;_TAF.tag=2;_TAF.f1=v;_T49=_TAF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF=_T49;void*_TB0[2];_T4A=_TB0 + 0;*_T4A=& _TAE;_T4B=_TB0 + 1;*_T4B=& _TAF;_T4C=loc;_T4D=_tag_fat(_TB0,sizeof(void*),2);Cyc_Warn_warn2(_T4C,_T4D);}}goto _TL107;_TL106: _TL107:;}_pop_handler();goto _TL104;_TL103: _T4E=Cyc_Core_get_exn_thrown();{void*_TAE=(void*)_T4E;void*_TAF;_T4F=(struct Cyc_Core_Not_found_exn_struct*)_TAE;_T50=_T4F->tag;_T51=Cyc_Core_Not_found;if(_T50!=_T51)goto _TL108;_T52=existential;
# 647
if(!_T52)goto _TL10A;_T53=explicit_bound_set;if(_T53)goto _TL10A;else{goto _TL10C;}
_TL10C: _T54=qb;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB1=_cycalloc(sizeof(struct _tuple0));_TB1->f0=v;_TB1->f1=Cyc_Absyn_al_qual_type;_T56=(struct _tuple0*)_TB1;}_TB0->hd=_T56;_T57=qb;_TB0->tl=*_T57;_T55=(struct Cyc_List_List*)_TB0;}*_T54=_T55;goto _TL10B;
# 650
_TL10A: _T58=qb;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB1=_cycalloc(sizeof(struct _tuple0));_TB1->f0=v;_TB1->f1=adef;_T5A=(struct _tuple0*)_TB1;}_TB0->hd=_T5A;_T5B=qb;_TB0->tl=*_T5B;_T59=(struct Cyc_List_List*)_TB0;}*_T58=_T59;_TL10B: goto _LL11;_TL108: _TAF=_TAE;{void*exn=_TAF;_rethrow(exn);}_LL11:;}_TL104:;}}goto _TL100;_TLFF: _TL100: _TLEF: goto _LL3;}_TLEC: goto _LLA;case 0: _T5C=v;_T5D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5C;_T5E=_T5D->f1;_T5F=(int*)_T5E;_T60=*_T5F;if(_T60!=17)goto _TL10D;_T61=v;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T61;_TA9=_TAC->f2;}{struct Cyc_List_List*tv_bnd=_TA9;_T62=
# 656
_check_null(tv_bnd);{void*tv=_T62->hd;{struct Cyc_Absyn_Tvar*_TAC;_T63=tv;_T64=(int*)_T63;_T65=*_T64;if(_T65!=2)goto _TL10F;_T66=tv;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TAD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T66;_TAC=_TAD->f1;}{struct Cyc_Absyn_Tvar*tv=_TAC;_T67=tv;_T68=_T67->aquals_bound;_T69=(unsigned)_T68;
# 659
if(!_T69)goto _TL111;_T6A=tv;
adef=_T6A->aquals_bound;
explicit_bound_set=1;goto _TL112;_TL111: _TL112: goto _LL16;}_TL10F: goto _LL16;_LL16:;}{struct _handler_cons _TAC;_T6B=& _TAC;_push_handler(_T6B);{int _TAD=0;_T6C=_TAC.handler;_T6D=setjmp(_T6C);if(!_T6D)goto _TL113;_TAD=1;goto _TL114;_TL113: _TL114: if(_TAD)goto _TL115;else{goto _TL117;}_TL117: _T6E=qb;_T6F=*_T6E;_T70=tv;{
# 668
void*x_bnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,_T6F,_T70);_T71=explicit_bound_set;
if(!_T71)goto _TL118;_T72=Cyc_Tcutil_typecmp(x_bnd,adef);if(!_T72)goto _TL118;{struct Cyc_Warn_String_Warn_Warg_struct _TAE;_TAE.tag=0;
_TAE.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T73=_TAE;}{struct Cyc_Warn_String_Warn_Warg_struct _TAE=_T73;{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF;_TAF.tag=2;_TAF.f1=v;_T74=_TAF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TAF=_T74;void*_TB0[2];_T75=_TB0 + 0;*_T75=& _TAE;_T76=_TB0 + 1;*_T76=& _TAF;_T77=loc;_T78=_tag_fat(_TB0,sizeof(void*),2);Cyc_Warn_warn2(_T77,_T78);}}goto _TL119;_TL118: _TL119: _T79=tv_bnd;_T7A=_T79->tl;_T7B=
_check_null(_T7A);_T7C=x_bnd;_T7B->hd=(void*)_T7C;}_pop_handler();goto _TL116;_TL115: _T7D=Cyc_Core_get_exn_thrown();{void*_TAE=(void*)_T7D;void*_TAF;_T7E=(struct Cyc_Core_Not_found_exn_struct*)_TAE;_T7F=_T7E->tag;_T80=Cyc_Core_Not_found;if(_T7F!=_T80)goto _TL11A;{
# 675
void*_TB0=Cyc_Absyn_compress(tv);_T81=(int*)_TB0;_T82=*_T81;switch(_T82){case 1: goto _LL24;case 2: _LL24: _T83=existential;
# 678
if(!_T83)goto _TL11D;_T84=explicit_bound_set;if(_T84)goto _TL11D;else{goto _TL11F;}
_TL11F: _T85=qb;{struct Cyc_List_List*_TB1=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB2=_cycalloc(sizeof(struct _tuple0));_TB2->f0=tv;_TB2->f1=Cyc_Absyn_al_qual_type;_T87=(struct _tuple0*)_TB2;}_TB1->hd=_T87;_T88=qb;_TB1->tl=*_T88;_T86=(struct Cyc_List_List*)_TB1;}*_T85=_T86;_T89=tv_bnd;_T8A=_T89->tl;_T8B=
_check_null(_T8A);_T8C=Cyc_Absyn_al_qual_type;_T8B->hd=(void*)_T8C;goto _TL11E;
# 683
_TL11D: _T8D=qb;{struct Cyc_List_List*_TB1=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TB2=_cycalloc(sizeof(struct _tuple0));_TB2->f0=tv;_TB2->f1=adef;_T8F=(struct _tuple0*)_TB2;}_TB1->hd=_T8F;_T90=qb;_TB1->tl=*_T90;_T8E=(struct Cyc_List_List*)_TB1;}*_T8D=_T8E;_T91=tv_bnd;_T92=_T91->tl;_T93=
_check_null(_T92);_T94=adef;_T93->hd=(void*)_T94;_TL11E: goto _LL20;case 0: _T95=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB0;_T96=_T95->f1;_T97=(int*)_T96;_T98=*_T97;if(_T98!=15)goto _TL120;{struct Cyc_Warn_String_Warn_Warg_struct _TB1;_TB1.tag=0;
# 688
_TB1.f1=_tag_fat("aquals bounds must be present",sizeof(char),30U);_T99=_TB1;}{struct Cyc_Warn_String_Warn_Warg_struct _TB1=_T99;void*_TB2[1];_T9A=_TB2 + 0;*_T9A=& _TB1;_T9C=Cyc_Warn_impos2;{int(*_TB3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T9C;_T9B=_TB3;}_T9D=_tag_fat(_TB2,sizeof(void*),1);_T9B(_T9D);}goto _TL121;_TL120: goto _LL27;_TL121:;default: _LL27: goto _LL20;}_LL20:;}goto _LL1B;_TL11A: _TAF=_TAE;{void*exn=_TAF;_rethrow(exn);}_LL1B:;}_TL116:;}}goto _LL3;}}_TL10D: goto _LLA;default: _LLA:{struct Cyc_Warn_String_Warn_Warg_struct _TAC;_TAC.tag=0;
# 697
_TAC.f1=_tag_fat("Expect only tvars, evars and aqual_vars -- got ",sizeof(char),48U);_T9E=_TAC;}{struct Cyc_Warn_String_Warn_Warg_struct _TAC=_T9E;{struct Cyc_Warn_Typ_Warn_Warg_struct _TAD;_TAD.tag=2;_TAD.f1=v;_T9F=_TAD;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TAD=_T9F;void*_TAE[2];_TA0=_TAE + 0;*_TA0=& _TAC;_TA1=_TAE + 1;*_TA1=& _TAD;_TA3=Cyc_Warn_impos2;{int(*_TAF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TA3;_TA2=_TAF;}_TA4=_tag_fat(_TAE,sizeof(void*),2);_TA2(_TA4);}}goto _LL3;}_LL3:;}}}_TA5=vars;
# 610
vars=_TA5->tl;goto _TLE3;_TLE2:;}
# 703
static struct _tuple14*Cyc_Tctyp_tvar2type(struct _tuple12*in){struct _tuple12*_T0;struct _tuple14*_T1;int _T2;struct Cyc_Absyn_Tvar*_T3;_T0=in;{struct _tuple12 _T4=*_T0;_T3=_T4.f0;_T2=_T4.f1;}{struct Cyc_Absyn_Tvar*tv=_T3;int b=_T2;
# 705
void*t=Cyc_Absyn_var_type(tv);{struct _tuple14*_T4=_cycalloc(sizeof(struct _tuple14));
_T4->f0=t;_T4->f1=b;_T1=(struct _tuple14*)_T4;}return _T1;}}
# 709
static int Cyc_Tctyp_evar_unconstrained(struct _tuple14*evb){struct _tuple14*_T0;struct _tuple14 _T1;void*_T2;int*_T3;int _T4;_T0=evb;_T1=*_T0;_T2=_T1.f0;{
void*_T5=Cyc_Absyn_compress(_T2);_T3=(int*)_T5;_T4=*_T3;if(_T4!=1)goto _TL122;
return 1;_TL122:
 return 0;;}}
# 716
static void Cyc_Tctyp_update_qualifier_bounds(unsigned loc,struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List*flagged_tvars,struct Cyc_List_List**qb,void*def){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;unsigned _T2;struct Cyc_Tctyp_CVTEnv _T3;struct Cyc_List_List*(*_T4)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_T5)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List**_T8;void*_T9;unsigned _TA;struct Cyc_Tctyp_CVTEnv _TB;struct Cyc_List_List*(*_TC)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_TD)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Tctyp_CVTEnv _TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List**_T11;void*_T12;unsigned _T13;struct Cyc_Tctyp_CVTEnv _T14;struct Cyc_List_List*(*_T15)(int(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T16)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List**_T19;void*_T1A;unsigned _T1B;struct Cyc_Tctyp_CVTEnv _T1C;struct Cyc_List_List*(*_T1D)(int(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1E)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List**_T21;void*_T22;_T0=env;{
# 720
struct Cyc_List_List*freeqvs=_T0.free_qualvars;_T1=env;{
struct Cyc_List_List*freeevs=_T1.free_evars;_T2=loc;_T3=env;_T5=Cyc_List_map;{
struct Cyc_List_List*(*_T23)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*))_T5;_T4=_T23;}_T6=flagged_tvars;_T7=_T4(Cyc_Tctyp_tvar2type,_T6);_T8=qb;_T9=def;Cyc_Tctyp_constrain_vars(_T2,_T3,_T7,_T8,_T9,1);_TA=loc;_TB=env;_TD=Cyc_List_map;{
struct Cyc_List_List*(*_T23)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple12*),struct Cyc_List_List*))_TD;_TC=_T23;}_TE=env;_TF=_TE.free_vars;_T10=_TC(Cyc_Tctyp_tvar2type,_TF);_T11=qb;_T12=def;Cyc_Tctyp_constrain_vars(_TA,_TB,_T10,_T11,_T12,0);_T13=loc;_T14=env;_T16=Cyc_List_filter;{
struct Cyc_List_List*(*_T23)(int(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple14*),struct Cyc_List_List*))_T16;_T15=_T23;}_T17=freeevs;_T18=_T15(Cyc_Tctyp_evar_unconstrained,_T17);_T19=qb;_T1A=def;Cyc_Tctyp_constrain_vars(_T13,_T14,_T18,_T19,_T1A,0);_T1B=loc;_T1C=env;_T1E=Cyc_List_filter;{
struct Cyc_List_List*(*_T23)(int(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple14*),struct Cyc_List_List*))_T1E;_T1D=_T23;}_T1F=freeqvs;_T20=_T1D(Cyc_Tctyp_aqualvar_unconstrained,_T1F);_T21=qb;_T22=def;Cyc_Tctyp_constrain_vars(_T1B,_T1C,_T20,_T21,_T22,0);}}}
# 729
static void Cyc_Tctyp_check_constrain_ptrbnd(unsigned loc,void*t,struct Cyc_Absyn_Kind*expected_kind){struct Cyc_Absyn_Kind*_T0;enum Cyc_Absyn_KindQual _T1;int _T2;int*_T3;int _T4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T5;void**_T6;void**_T7;void*_T8;int _T9;unsigned _TA;void**_TB;void*_TC;void*_TD;void*_TE;int _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;struct Cyc_Warn_Kind_Warn_Warg_struct _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Kind_Warn_Warg_struct _T15;void**_T16;void**_T17;void**_T18;void**_T19;void**_T1A;void**_T1B;unsigned _T1C;struct _fat_ptr _T1D;void**_T1E;void*_T1F;void*_T20;void*_T21;_T0=expected_kind;_T1=_T0->kind;_T2=(int)_T1;
if(_T2!=2)goto _TL124;{
void*_T22=Cyc_Absyn_compress(t);void*_T23;_T3=(int*)_T22;_T4=*_T3;if(_T4!=4)goto _TL126;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T24=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T22;_T5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T22;_T6=& _T5->f1.ptr_atts.bounds;_T23=(void**)_T6;}{void**b=(void**)_T23;_T7=b;_T8=*_T7;_T9=
# 733
Cyc_Tcutil_is_cvar_type(_T8);if(!_T9)goto _TL128;_TA=loc;_TB=b;_TC=*_TB;_TD=
Cyc_Absyn_thinconst();_TE=Cyc_BansheeIf_equality_constraint(_TC,_TD);_TF=Cyc_BansheeIf_add_constraint(_TA,_TE);if(_TF)goto _TL12A;else{goto _TL12C;}
_TL12C:{struct Cyc_Warn_String_Warn_Warg_struct _T24;_T24.tag=0;_T24.f1=_tag_fat("type ",sizeof(char),6U);_T10=_T24;}{struct Cyc_Warn_String_Warn_Warg_struct _T24=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T25;_T25.tag=2;_T25.f1=t;_T11=_T25;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T25=_T11;{struct Cyc_Warn_String_Warn_Warg_struct _T26;_T26.tag=0;_T26.f1=_tag_fat(" has kind ",sizeof(char),11U);_T12=_T26;}{struct Cyc_Warn_String_Warn_Warg_struct _T26=_T12;{struct Cyc_Warn_Kind_Warn_Warg_struct _T27;_T27.tag=9;_T27.f1=Cyc_Tcutil_type_kind(t);_T13=_T27;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T27=_T13;{struct Cyc_Warn_String_Warn_Warg_struct _T28;_T28.tag=0;
_T28.f1=_tag_fat(" but as used here needs kind ",sizeof(char),30U);_T14=_T28;}{struct Cyc_Warn_String_Warn_Warg_struct _T28=_T14;{struct Cyc_Warn_Kind_Warn_Warg_struct _T29;_T29.tag=9;_T29.f1=expected_kind;_T15=_T29;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T29=_T15;void*_T2A[6];_T16=_T2A + 0;*_T16=& _T24;_T17=_T2A + 1;*_T17=& _T25;_T18=_T2A + 2;*_T18=& _T26;_T19=_T2A + 3;*_T19=& _T27;_T1A=_T2A + 4;*_T1A=& _T28;_T1B=_T2A + 5;*_T1B=& _T29;_T1C=loc;_T1D=_tag_fat(_T2A,sizeof(void*),6);Cyc_Warn_err2(_T1C,_T1D);}}}}}}goto _TL12B;
# 739
_TL12A: _T1E=b;_T1F=*_T1E;_T20=Cyc_Absyn_compress(_T1F);_T21=Cyc_Absyn_bounds_one();if(_T20!=_T21)goto _TL12D;goto _TL12E;_TL12D: _throw_assert();_TL12E: _TL12B: goto _TL129;_TL128: _TL129: goto _LL0;}_TL126: goto _LL0;_LL0:;}goto _TL125;_TL124: _TL125:;}struct _tuple16{enum Cyc_Absyn_Format_Type f0;void*f1;};struct _tuple17{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 749
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;struct Cyc_List_List**_T6;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T7;struct Cyc_Core_Opt**_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;void**_TB;void**_TC;unsigned _TD;struct _fat_ptr _TE;struct Cyc_Core_Opt**_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Absyn_Kind*_T11;struct Cyc_Core_Opt**_T12;struct Cyc_Core_Opt*_T13;void*_T14;struct Cyc_Absyn_Kind*_T15;int _T16;struct Cyc_Core_Opt**_T17;struct Cyc_Core_Opt*_T18;struct Cyc_Core_Opt*_T19;void*_T1A;struct Cyc_Absyn_Kind*_T1B;struct Cyc_Absyn_Kind*_T1C;int _T1D;struct Cyc_Core_Opt**_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;struct Cyc_Core_Opt**_T20;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T21;void**_T22;struct Cyc_Core_Opt**_T23;struct Cyc_Core_Opt*_T24;struct Cyc_Absyn_Kind*_T25;struct Cyc_Core_Opt**_T26;struct Cyc_Core_Opt*_T27;void*_T28;struct Cyc_Absyn_Kind*_T29;int _T2A;struct Cyc_Core_Opt**_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Core_Opt*_T2D;void*_T2E;struct Cyc_Absyn_Kind*_T2F;struct Cyc_Absyn_Kind*_T30;int _T31;struct Cyc_Core_Opt**_T32;struct Cyc_Tctyp_CVTEnv _T33;int _T34;struct Cyc_Tctyp_CVTEnv _T35;int _T36;struct Cyc_Absyn_Kind*_T37;enum Cyc_Absyn_KindQual _T38;int _T39;struct Cyc_Tcenv_Tenv*_T3A;int _T3B;void**_T3C;struct Cyc_Absyn_Kind*_T3D;enum Cyc_Absyn_KindQual _T3E;int _T3F;struct Cyc_Tcenv_Tenv*_T40;int _T41;void**_T42;struct Cyc_Tctyp_CVTEnv _T43;int _T44;struct Cyc_Absyn_Kind*_T45;enum Cyc_Absyn_KindQual _T46;int _T47;struct Cyc_Tcenv_Tenv*_T48;int _T49;void**_T4A;struct Cyc_Tctyp_CVTEnv _T4B;int _T4C;struct Cyc_Absyn_Kind*_T4D;enum Cyc_Absyn_KindQual _T4E;int _T4F;struct Cyc_Absyn_Kind*_T50;enum Cyc_Absyn_KindQual _T51;int _T52;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T53;void*_T54;void**_T55;struct Cyc_Tctyp_CVTEnv _T56;struct Cyc_List_List*_T57;void*_T58;int _T59;struct Cyc_Absyn_Tvar*_T5A;void*_T5B;int*_T5C;int _T5D;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T5E;struct Cyc_Core_Opt**_T5F;struct Cyc_Core_Opt**_T60;struct Cyc_Core_Opt*_T61;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T62;unsigned _T63;struct Cyc_Tctyp_CVTEnv _T64;struct Cyc_List_List*_T65;struct Cyc_Absyn_Tvar*_T66;struct Cyc_Tctyp_CVTEnv _T67;int _T68;struct Cyc_Tctyp_CVTEnv _T69;struct Cyc_List_List*_T6A;struct Cyc_Absyn_Tvar*_T6B;int _T6C;struct Cyc_Absyn_Tvar*_T6D;void*_T6E;int*_T6F;int _T70;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T71;struct Cyc_Core_Opt**_T72;int _T73;struct Cyc_Core_Opt**_T74;struct Cyc_Core_Opt*_T75;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T76;struct Cyc_Absyn_TypeDecl*_T77;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T78;void***_T79;void*_T7A;void*_T7B;int*_T7C;unsigned _T7D;void*_T7E;struct Cyc_Tctyp_CVTEnv _T7F;struct Cyc_Tcenv_Tenv*_T80;int _T81;struct Cyc_Absyn_Aggrdecl*_T82;void*_T83;struct Cyc_Tctyp_CVTEnv _T84;struct Cyc_Tcenv_Tenv*_T85;int _T86;struct Cyc_Absyn_Enumdecl*_T87;void*_T88;void***_T89;void**_T8A;struct Cyc_Tctyp_CVTEnv _T8B;struct Cyc_Absyn_PtrInfo _T8C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8D;struct Cyc_Absyn_Tqual*_T8E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8F;void**_T90;struct Cyc_Absyn_PtrInfo _T91;struct Cyc_Absyn_PtrAtts _T92;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T93;void**_T94;struct Cyc_Absyn_PtrInfo _T95;struct Cyc_Absyn_PtrAtts _T96;struct Cyc_Absyn_PtrInfo _T97;struct Cyc_Absyn_PtrAtts _T98;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T99;void**_T9A;void**_T9B;void**_T9C;int*_T9D;int _T9E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9F;void*_TA0;int*_TA1;unsigned _TA2;void**_TA3;void**_TA4;void*_TA5;void**_TA6;void*_TA7;void**_TA8;int*_TA9;int _TAA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAB;void*_TAC;int*_TAD;int _TAE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAF;struct Cyc_List_List*_TB0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB1;struct Cyc_List_List*_TB2;struct Cyc_List_List*_TB3;void*_TB4;int*_TB5;int _TB6;struct Cyc_List_List*_TB7;struct Cyc_Warn_String_Warn_Warg_struct _TB8;void**_TB9;unsigned _TBA;struct _fat_ptr _TBB;void**_TBC;struct Cyc_Tctyp_CVTEnv _TBD;struct Cyc_Absyn_Kind*_TBE;struct Cyc_Absyn_Kind*_TBF;void*_TC0;struct Cyc_Tctyp_CVTEnv _TC1;struct Cyc_Absyn_Kind*_TC2;struct Cyc_Absyn_Kind*_TC3;void**_TC4;void*_TC5;struct Cyc_Tctyp_CVTEnv _TC6;struct Cyc_Absyn_Kind*_TC7;struct Cyc_Absyn_Kind*_TC8;void*_TC9;struct Cyc_Tctyp_CVTEnv _TCA;struct Cyc_Absyn_Kind*_TCB;struct Cyc_Absyn_Kind*_TCC;void*_TCD;struct Cyc_Tctyp_CVTEnv _TCE;struct Cyc_Absyn_Kind*_TCF;struct Cyc_Absyn_Kind*_TD0;void**_TD1;void*_TD2;int _TD3;struct Cyc_Tctyp_CVTEnv _TD4;struct Cyc_Absyn_Kind*_TD5;struct Cyc_Absyn_Kind*_TD6;void*_TD7;void**_TD8;int*_TD9;int _TDA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDB;void*_TDC;int*_TDD;int _TDE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDF;struct Cyc_List_List*_TE0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE1;struct Cyc_List_List*_TE2;struct Cyc_List_List*_TE3;void*_TE4;int*_TE5;int _TE6;struct Cyc_List_List*_TE7;struct Cyc_Tctyp_CVTEnv _TE8;int _TE9;void**_TEA;struct Cyc_List_List*_TEB;void*_TEC;struct Cyc_Tctyp_CVTEnv _TED;struct Cyc_Absyn_Kind*_TEE;struct Cyc_Absyn_Kind*_TEF;void**_TF0;void*_TF1;struct Cyc_Absyn_Tqual*_TF2;struct Cyc_Tctyp_CVTEnv _TF3;unsigned _TF4;struct Cyc_Absyn_Tqual*_TF5;int _TF6;void*_TF7;void*_TF8;void*_TF9;int _TFA;int _TFB;int _TFC;struct Cyc_Warn_String_Warn_Warg_struct _TFD;struct Cyc_Warn_Typ_Warn_Warg_struct _TFE;struct Cyc_Warn_String_Warn_Warg_struct _TFF;void**_T100;void**_T101;void**_T102;unsigned _T103;struct _fat_ptr _T104;void**_T105;void*_T106;int _T107;void*_T108;void**_T109;void*_T10A;int _T10B;int _T10C;struct Cyc_Warn_String_Warn_Warg_struct _T10D;void**_T10E;unsigned _T10F;struct _fat_ptr _T110;struct Cyc_Tcenv_Tenv*_T111;struct Cyc_Absyn_Exp*_T112;int _T113;struct Cyc_Warn_String_Warn_Warg_struct _T114;void**_T115;unsigned _T116;struct _fat_ptr _T117;struct Cyc_Tcenv_Tenv*_T118;struct Cyc_Absyn_Exp*_T119;struct Cyc_Absyn_ArrayInfo _T11A;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T11B;struct Cyc_Absyn_Tqual*_T11C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T11D;struct Cyc_Absyn_Exp**_T11E;struct Cyc_Absyn_ArrayInfo _T11F;struct Cyc_Absyn_ArrayInfo _T120;struct Cyc_Absyn_Exp**_T121;struct Cyc_Tctyp_CVTEnv _T122;struct Cyc_Absyn_Kind*_T123;struct Cyc_Absyn_Kind*_T124;void*_T125;int _T126;struct Cyc_Absyn_Tqual*_T127;unsigned _T128;struct Cyc_Absyn_Tqual*_T129;int _T12A;void*_T12B;int _T12C;int _T12D;struct Cyc_Warn_String_Warn_Warg_struct _T12E;struct Cyc_Warn_Typ_Warn_Warg_struct _T12F;struct Cyc_Warn_String_Warn_Warg_struct _T130;void**_T131;void**_T132;void**_T133;unsigned _T134;struct _fat_ptr _T135;int _T136;struct Cyc_Warn_String_Warn_Warg_struct _T137;void**_T138;unsigned _T139;struct _fat_ptr _T13A;struct Cyc_Absyn_Exp**_T13B;struct Cyc_Tcenv_Tenv*_T13C;struct Cyc_Absyn_Exp*_T13D;int _T13E;struct Cyc_Warn_String_Warn_Warg_struct _T13F;void**_T140;unsigned _T141;struct _fat_ptr _T142;int _T143;int _T144;struct Cyc_Warn_String_Warn_Warg_struct _T145;void**_T146;unsigned _T147;struct _fat_ptr _T148;int _T149;int _T14A;struct Cyc_Warn_String_Warn_Warg_struct _T14B;void**_T14C;unsigned _T14D;struct _fat_ptr _T14E;struct Cyc_Absyn_Exp**_T14F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T150;struct Cyc_List_List**_T151;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T152;void**_T153;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T154;struct Cyc_Absyn_Tqual*_T155;struct Cyc_Absyn_FnInfo _T156;struct Cyc_Absyn_FnInfo _T157;struct Cyc_Absyn_FnInfo _T158;struct Cyc_Absyn_FnInfo _T159;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T15A;struct Cyc_List_List**_T15B;struct Cyc_Absyn_FnInfo _T15C;struct Cyc_Absyn_FnInfo _T15D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T15E;struct Cyc_AssnDef_ExistAssnFn**_T15F;struct Cyc_Absyn_FnInfo _T160;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T161;struct Cyc_AssnDef_ExistAssnFn**_T162;struct Cyc_Absyn_FnInfo _T163;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T164;struct Cyc_AssnDef_ExistAssnFn**_T165;struct Cyc_Absyn_FnInfo _T166;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T167;struct Cyc_AssnDef_ExistAssnFn**_T168;struct Cyc_Absyn_FnInfo _T169;struct Cyc_Absyn_FnInfo _T16A;struct Cyc_Absyn_FnInfo _T16B;struct Cyc_List_List*_T16C;int*_T16D;unsigned _T16E;struct Cyc_List_List*_T16F;void*_T170;int _T171;struct Cyc_Warn_String_Warn_Warg_struct _T172;struct Cyc_Warn_Attribute_Warn_Warg_struct _T173;struct Cyc_List_List*_T174;void**_T175;void**_T176;unsigned _T177;struct _fat_ptr _T178;struct Cyc_List_List*_T179;int _T17A;int _T17B;int _T17C;struct Cyc_Warn_String_Warn_Warg_struct _T17D;void**_T17E;unsigned _T17F;struct _fat_ptr _T180;struct Cyc_Warn_String_Warn_Warg_struct _T181;void**_T182;unsigned _T183;struct _fat_ptr _T184;unsigned _T185;struct Cyc_List_List**_T186;struct Cyc_List_List*_T187;struct Cyc_List_List**_T188;struct Cyc_List_List*_T189;void*_T18A;struct Cyc_Absyn_Tvar*_T18B;struct Cyc_Tctyp_CVTEnv _T18C;struct Cyc_List_List*_T18D;struct Cyc_List_List*_T18E;void*_T18F;struct Cyc_Absyn_Tvar*_T190;struct Cyc_List_List*_T191;void*_T192;struct Cyc_Absyn_Tvar*_T193;void*_T194;int*_T195;int _T196;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T197;struct Cyc_Absyn_Kind*_T198;struct Cyc_Absyn_Kind*_T199;enum Cyc_Absyn_KindQual _T19A;struct Cyc_Warn_String_Warn_Warg_struct _T19B;struct Cyc_Warn_Tvar_Warn_Warg_struct _T19C;struct Cyc_List_List*_T19D;void*_T19E;void**_T19F;void**_T1A0;unsigned _T1A1;struct _fat_ptr _T1A2;struct Cyc_List_List*_T1A3;struct Cyc_Tctyp_CVTEnv _T1A4;struct Cyc_Tctyp_CVTEnv _T1A5;struct Cyc_Tctyp_CVTEnv _T1A6;struct Cyc_Tctyp_CVTEnv _T1A7;struct Cyc_Absyn_Kind*_T1A8;struct Cyc_Absyn_Kind*_T1A9;void*_T1AA;struct Cyc_Absyn_Tqual*_T1AB;unsigned _T1AC;struct Cyc_Absyn_Tqual*_T1AD;int _T1AE;void*_T1AF;struct Cyc_List_List*_T1B0;void*_T1B1;struct _tuple9*_T1B2;struct _tuple9 _T1B3;struct Cyc_Tctyp_CVTEnv _T1B4;struct Cyc_Absyn_Kind*_T1B5;struct Cyc_Absyn_Kind*_T1B6;void*_T1B7;unsigned _T1B8;struct _tuple9*_T1B9;struct _tuple9 _T1BA;struct Cyc_Absyn_Tqual _T1BB;int _T1BC;void*_T1BD;struct _tuple9*_T1BE;int _T1BF;struct Cyc_Tctyp_CVTEnv _T1C0;struct Cyc_Absyn_Kind*_T1C1;struct Cyc_Absyn_Kind*_T1C2;void*_T1C3;struct Cyc_Core_Opt*_T1C4;struct Cyc_Core_Opt*_T1C5;void*_T1C6;void*_T1C7;struct Cyc_Tctyp_CVTEnv _T1C8;struct Cyc_Absyn_Kind*_T1C9;struct Cyc_Absyn_Kind*_T1CA;void*_T1CB;struct _tuple9*_T1CC;struct Cyc_List_List*_T1CD;int _T1CE;struct Cyc_Warn_String_Warn_Warg_struct _T1CF;void**_T1D0;int(*_T1D1)(struct _fat_ptr);void*(*_T1D2)(struct _fat_ptr);struct _fat_ptr _T1D3;struct Cyc_Absyn_VarargInfo*_T1D4;struct Cyc_Tctyp_CVTEnv _T1D5;struct Cyc_Absyn_Kind*_T1D6;struct Cyc_Absyn_Kind*_T1D7;void*_T1D8;struct Cyc_Absyn_VarargInfo*_T1D9;unsigned _T1DA;struct Cyc_Absyn_VarargInfo*_T1DB;struct Cyc_Absyn_Tqual _T1DC;int _T1DD;void*_T1DE;int _T1DF;int*_T1E0;int _T1E1;struct Cyc_Absyn_PtrInfo _T1E2;struct Cyc_Absyn_PtrInfo _T1E3;struct Cyc_Absyn_PtrAtts _T1E4;struct Cyc_Absyn_PtrInfo _T1E5;struct Cyc_Absyn_PtrAtts _T1E6;int*_T1E7;int _T1E8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1E9;void*_T1EA;int*_T1EB;int _T1EC;int _T1ED;struct Cyc_Warn_String_Warn_Warg_struct _T1EE;void**_T1EF;unsigned _T1F0;struct _fat_ptr _T1F1;void*_T1F2;void*_T1F3;int _T1F4;int _T1F5;unsigned _T1F6;void*_T1F7;void*_T1F8;void*_T1F9;int _T1FA;struct Cyc_Warn_String_Warn_Warg_struct _T1FB;void**_T1FC;unsigned _T1FD;struct _fat_ptr _T1FE;struct Cyc_Warn_String_Warn_Warg_struct _T1FF;void**_T200;unsigned _T201;struct _fat_ptr _T202;struct Cyc_Warn_String_Warn_Warg_struct _T203;void**_T204;unsigned _T205;struct _fat_ptr _T206;int _T207;int _T208;int _T209;int _T20A;struct Cyc_Warn_String_Warn_Warg_struct _T20B;void**_T20C;unsigned _T20D;struct _fat_ptr _T20E;struct _tuple9*(*_T20F)(struct Cyc_List_List*,int);void*(*_T210)(struct Cyc_List_List*,int);struct Cyc_List_List*_T211;int _T212;struct _tuple9*_T213;int*_T214;int _T215;struct Cyc_Absyn_PtrInfo _T216;struct Cyc_Absyn_PtrInfo _T217;struct Cyc_Absyn_PtrAtts _T218;int _T219;struct Cyc_Warn_String_Warn_Warg_struct _T21A;void**_T21B;unsigned _T21C;struct _fat_ptr _T21D;int _T21E;void*_T21F;void*_T220;int _T221;struct Cyc_Warn_String_Warn_Warg_struct _T222;void**_T223;unsigned _T224;struct _fat_ptr _T225;int _T226;struct Cyc_Warn_String_Warn_Warg_struct _T227;void**_T228;unsigned _T229;struct _fat_ptr _T22A;int _T22B;unsigned _T22C;void*_T22D;void*_T22E;void*_T22F;void*_T230;int _T231;struct Cyc_Warn_String_Warn_Warg_struct _T232;void**_T233;unsigned _T234;struct _fat_ptr _T235;unsigned _T236;void*_T237;void*_T238;void*_T239;void*_T23A;int _T23B;struct Cyc_Warn_String_Warn_Warg_struct _T23C;void**_T23D;unsigned _T23E;struct _fat_ptr _T23F;struct Cyc_Warn_String_Warn_Warg_struct _T240;void**_T241;unsigned _T242;struct _fat_ptr _T243;int _T244;struct _tuple16 _T245;struct Cyc_Absyn_VarargInfo*_T246;void*_T247;void*_T248;enum Cyc_Absyn_Format_Type _T249;int _T24A;void*_T24B;int*_T24C;int _T24D;void*_T24E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24F;void*_T250;int*_T251;int _T252;void*_T253;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T254;void*_T255;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T256;union Cyc_Absyn_DatatypeInfo _T257;struct _union_DatatypeInfo_KnownDatatype _T258;unsigned _T259;void*_T25A;void*_T25B;union Cyc_Absyn_DatatypeInfo _T25C;struct _union_DatatypeInfo_KnownDatatype _T25D;struct Cyc_Absyn_Datatypedecl**_T25E;struct Cyc_Absyn_Datatypedecl*_T25F;struct _tuple1*_T260;struct _tuple1*_T261;int _T262;void*_T263;int*_T264;int _T265;void*_T266;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T267;void*_T268;int*_T269;int _T26A;void*_T26B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26C;void*_T26D;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T26E;union Cyc_Absyn_DatatypeInfo _T26F;struct _union_DatatypeInfo_KnownDatatype _T270;unsigned _T271;void*_T272;void*_T273;union Cyc_Absyn_DatatypeInfo _T274;struct _union_DatatypeInfo_KnownDatatype _T275;struct Cyc_Absyn_Datatypedecl**_T276;struct Cyc_Absyn_Datatypedecl*_T277;struct _tuple1*_T278;struct _tuple1*_T279;int _T27A;int _T27B;struct Cyc_Warn_String_Warn_Warg_struct _T27C;void**_T27D;unsigned _T27E;struct _fat_ptr _T27F;struct Cyc_List_List**_T280;struct Cyc_List_List*_T281;void*_T282;struct Cyc_Tctyp_CVTEnv _T283;struct Cyc_Absyn_Kind*_T284;struct Cyc_Absyn_Kind*_T285;void*_T286;struct Cyc_Tctyp_CVTEnv _T287;struct Cyc_Absyn_Kind*_T288;struct Cyc_Absyn_Kind*_T289;void*_T28A;struct Cyc_List_List*_T28B;struct Cyc_List_List*_T28C;int*_T28D;unsigned _T28E;void*_T28F;struct Cyc_Tctyp_CVTEnv _T290;struct Cyc_Absyn_Kind*_T291;struct Cyc_Absyn_Kind*_T292;void*_T293;void*_T294;void*_T295;void*_T296;void*_T297;struct Cyc_Tctyp_CVTEnv _T298;struct Cyc_Absyn_Kind*_T299;struct Cyc_Absyn_Kind*_T29A;void*_T29B;struct Cyc_Tctyp_CVTEnv _T29C;struct Cyc_Absyn_Kind*_T29D;struct Cyc_Absyn_Kind*_T29E;void*_T29F;struct Cyc_List_List*_T2A0;struct Cyc_Tctyp_CVTEnv _T2A1;struct Cyc_Tcenv_Tenv*_T2A2;struct Cyc_Tcenv_Tenv*_T2A3;struct Cyc_Tcenv_Tenv*_T2A4;struct Cyc_Tcenv_Tenv*_T2A5;struct Cyc_Tcenv_Tenv*_T2A6;struct Cyc_Tcenv_Tenv*_T2A7;struct Cyc_Tcenv_Tenv*_T2A8;struct Cyc_List_List*_T2A9;struct Cyc_List_List*_T2AA;int _T2AB;struct Cyc_Absyn_VarargInfo*_T2AC;unsigned _T2AD;struct Cyc_Warn_String_Warn_Warg_struct _T2AE;void**_T2AF;unsigned _T2B0;struct _fat_ptr _T2B1;struct Cyc_Tctyp_CVTEnv _T2B2;struct _fat_ptr _T2B3;struct Cyc_Absyn_Exp*_T2B4;struct Cyc_AssnDef_ExistAssnFn**_T2B5;struct Cyc_Tctyp_CVTEnv _T2B6;struct _fat_ptr _T2B7;struct Cyc_Absyn_Exp*_T2B8;struct Cyc_AssnDef_ExistAssnFn**_T2B9;struct Cyc_List_List*_T2BA;struct Cyc_List_List*_T2BB;struct Cyc_List_List*_T2BC;struct Cyc_Tctyp_CVTEnv _T2BD;struct _fat_ptr _T2BE;struct Cyc_Absyn_Exp*_T2BF;struct Cyc_AssnDef_ExistAssnFn**_T2C0;struct Cyc_AssnDef_AssnMap _T2C1;struct Cyc_List_List*_T2C2;struct Cyc_Tctyp_CVTEnv _T2C3;struct _fat_ptr _T2C4;struct Cyc_Absyn_Exp*_T2C5;struct Cyc_AssnDef_ExistAssnFn**_T2C6;void**_T2C7;void*_T2C8;struct Cyc_Tctyp_CVTEnv _T2C9;struct Cyc_Absyn_Kind*_T2CA;struct Cyc_Absyn_Kind*_T2CB;void**_T2CC;void*_T2CD;void*_T2CE;void**_T2CF;void*_T2D0;void*_T2D1;int _T2D2;void**_T2D3;struct Cyc_List_List*_T2D4;void**_T2D5;void**_T2D6;void**_T2D7;void*_T2D8;struct _fat_ptr _T2D9;void*_T2DA;struct Cyc_List_List*_T2DB;struct Cyc_List_List*_T2DC;struct Cyc_Tctyp_CVTEnv _T2DD;struct Cyc_List_List*_T2DE;void*_T2DF;struct _tuple12*_T2E0;int _T2E1;struct Cyc_Absyn_Tvar*_T2E2;void*_T2E3;int*_T2E4;unsigned _T2E5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2E6;struct Cyc_Absyn_Kind*_T2E7;struct Cyc_Absyn_Kind*_T2E8;enum Cyc_Absyn_KindQual _T2E9;int _T2EA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2EB;struct Cyc_Core_Opt**_T2EC;struct Cyc_Core_Opt**_T2ED;struct Cyc_Absyn_Kind*_T2EE;struct Cyc_Absyn_Kind*_T2EF;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T2F0;struct Cyc_Absyn_Kind*_T2F1;struct Cyc_Absyn_Kind*_T2F2;enum Cyc_Absyn_KindQual _T2F3;int _T2F4;struct Cyc_List_List*_T2F5;struct Cyc_List_List*_T2F6;void*_T2F7;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T2F8;struct Cyc_Core_Opt**_T2F9;struct Cyc_Core_Opt**_T2FA;struct Cyc_Core_Opt*_T2FB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2FC;struct Cyc_Absyn_Kind*_T2FD;struct Cyc_List_List*_T2FE;struct Cyc_Tctyp_CVTEnv _T2FF;struct Cyc_List_List*_T300;void*_T301;struct _tuple14*_T302;int _T303;struct Cyc_Absyn_Kind*_T304;enum Cyc_Absyn_KindQual _T305;int _T306;struct Cyc_List_List*_T307;struct Cyc_List_List*_T308;struct Cyc_List_List*_T309;void**_T30A;struct Cyc_List_List**_T30B;struct Cyc_List_List*_T30C;struct Cyc_List_List**_T30D;struct Cyc_List_List*_T30E;void*_T30F;struct Cyc_Absyn_Tvar*_T310;void*_T311;int*_T312;unsigned _T313;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T314;struct Cyc_Core_Opt**_T315;struct Cyc_Warn_String_Warn_Warg_struct _T316;struct Cyc_Warn_Tvar_Warn_Warg_struct _T317;struct Cyc_List_List*_T318;void*_T319;struct Cyc_Warn_String_Warn_Warg_struct _T31A;void**_T31B;void**_T31C;void**_T31D;unsigned _T31E;struct _fat_ptr _T31F;struct Cyc_Core_Opt**_T320;struct Cyc_Absyn_Kind*_T321;struct Cyc_Absyn_Kind*_T322;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T323;struct Cyc_Core_Opt**_T324;struct Cyc_Absyn_Kind*_T325;struct Cyc_Absyn_Kind*_T326;enum Cyc_Absyn_KindQual _T327;int _T328;struct Cyc_Core_Opt**_T329;struct Cyc_Absyn_Kind*_T32A;struct Cyc_Absyn_Kind*_T32B;struct Cyc_Core_Opt**_T32C;struct Cyc_Absyn_Kind*_T32D;struct Cyc_Absyn_Kind*_T32E;struct Cyc_Core_Opt**_T32F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T330;struct Cyc_Absyn_Kind*_T331;struct Cyc_Absyn_Kind*_T332;enum Cyc_Absyn_KindQual _T333;struct Cyc_Warn_String_Warn_Warg_struct _T334;void**_T335;unsigned _T336;struct _fat_ptr _T337;struct Cyc_List_List*_T338;struct Cyc_List_List*(*_T339)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*));struct Cyc_Tctyp_CVTEnv _T33A;struct Cyc_List_List*_T33B;struct Cyc_List_List**_T33C;struct Cyc_List_List*_T33D;struct Cyc_Absyn_Tvar*(*_T33E)(struct Cyc_Absyn_Tvar*);void*(*_T33F)(void*);struct Cyc_List_List*(*_T340)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T341;struct Cyc_List_List*_T342;struct Cyc_List_List**_T343;struct Cyc_List_List*_T344;struct Cyc_Absyn_Tvar*(*_T345)(struct _tuple12*);void*(*_T346)(struct _tuple0*);struct Cyc_Tctyp_CVTEnv _T347;struct Cyc_List_List*_T348;void*_T349;struct _tuple12*_T34A;struct Cyc_Tctyp_CVTEnv _T34B;struct Cyc_List_List*_T34C;struct Cyc_Absyn_Tvar*_T34D;int _T34E;struct Cyc_List_List*_T34F;struct Cyc_Tctyp_CVTEnv _T350;struct Cyc_List_List*_T351;void*_T352;struct _tuple14*_T353;struct Cyc_Tctyp_CVTEnv _T354;struct Cyc_List_List*_T355;void*_T356;int _T357;struct Cyc_List_List*_T358;struct Cyc_Tctyp_CVTEnv _T359;struct Cyc_List_List*_T35A;unsigned _T35B;struct Cyc_Tctyp_CVTEnv _T35C;int(*_T35D)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*);int(*_T35E)(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T35F;void*_T360;struct _tuple14*_T361;struct Cyc_Tctyp_CVTEnv _T362;struct Cyc_List_List*_T363;int _T364;struct Cyc_List_List*_T365;struct Cyc_List_List*_T366;void*_T367;struct Cyc_Tctyp_CVTEnv _T368;struct Cyc_List_List*_T369;struct Cyc_Tctyp_CVTEnv _T36A;struct Cyc_List_List*_T36B;void*_T36C;struct Cyc_Absyn_Tqual*_T36D;int(*_T36E)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T36F)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T370)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T371;struct _fat_ptr*_T372;int _T373;struct Cyc_Warn_String_Warn_Warg_struct _T374;struct Cyc_Warn_String_Warn_Warg_struct _T375;struct _fat_ptr*_T376;void**_T377;void**_T378;unsigned _T379;struct _fat_ptr _T37A;struct _fat_ptr*_T37B;struct _fat_ptr _T37C;struct _fat_ptr _T37D;int _T37E;struct Cyc_List_List*_T37F;struct Cyc_Tctyp_CVTEnv _T380;struct Cyc_Absyn_Kind*_T381;struct Cyc_Absyn_Kind*_T382;void*_T383;struct Cyc_Absyn_Tqual*_T384;unsigned _T385;struct Cyc_Absyn_Tqual*_T386;int _T387;void*_T388;enum Cyc_Absyn_AggrKind _T389;int _T38A;struct Cyc_Warn_String_Warn_Warg_struct _T38B;void**_T38C;unsigned _T38D;struct _fat_ptr _T38E;struct Cyc_Tcenv_Tenv*_T38F;struct Cyc_Absyn_Exp*_T390;int _T391;struct Cyc_Warn_String_Warn_Warg_struct _T392;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T393;struct Cyc_Absyn_Exp*_T394;struct Cyc_Warn_String_Warn_Warg_struct _T395;void**_T396;void**_T397;void**_T398;unsigned _T399;struct _fat_ptr _T39A;struct Cyc_List_List*_T39B;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T39C;struct Cyc_List_List**_T39D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T39E;struct Cyc_Absyn_Typedefdecl**_T39F;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T3A0;void**_T3A1;struct _handler_cons*_T3A2;int*_T3A3;int _T3A4;void*_T3A5;struct Cyc_Dict_Absent_exn_struct*_T3A6;char*_T3A7;char*_T3A8;struct Cyc_Warn_String_Warn_Warg_struct _T3A9;struct Cyc_Warn_Qvar_Warn_Warg_struct _T3AA;void**_T3AB;void**_T3AC;unsigned _T3AD;struct _fat_ptr _T3AE;struct Cyc_Tctyp_CVTEnv _T3AF;struct Cyc_Absyn_Typedefdecl**_T3B0;struct Cyc_Tctyp_CVTEnv _T3B1;struct Cyc_List_List**_T3B2;struct Cyc_Absyn_Typedefdecl*_T3B3;struct Cyc_List_List*_T3B4;struct Cyc_Absyn_Kind*_T3B5;int _T3B6;struct _tuple1*_T3B7;struct Cyc_Absyn_Typedefdecl*_T3B8;struct Cyc_Absyn_Typedefdecl*_T3B9;void*_T3BA;unsigned _T3BB;struct Cyc_Absyn_Typedefdecl*_T3BC;void*_T3BD;struct Cyc_Absyn_Kind*_T3BE;struct Cyc_Absyn_Typedefdecl*_T3BF;struct Cyc_List_List**_T3C0;struct Cyc_List_List*_T3C1;struct _tuple17*_T3C2;struct Cyc_List_List*_T3C3;void*_T3C4;struct Cyc_List_List*_T3C5;struct Cyc_List_List*_T3C6;struct Cyc_List_List*_T3C7;void**_T3C8;struct Cyc_List_List*_T3C9;struct Cyc_Absyn_Typedefdecl*_T3CA;void*_T3CB;void*_T3CC;struct Cyc_Absyn_Kind*_T3CD;struct Cyc_Absyn_Kind*_T3CE;int _T3CF;struct Cyc_Warn_String_Warn_Warg_struct _T3D0;struct Cyc_Warn_Typ_Warn_Warg_struct _T3D1;struct Cyc_Warn_String_Warn_Warg_struct _T3D2;struct Cyc_Warn_Kind_Warn_Warg_struct _T3D3;struct Cyc_Warn_String_Warn_Warg_struct _T3D4;struct Cyc_Warn_Kind_Warn_Warg_struct _T3D5;void**_T3D6;void**_T3D7;void**_T3D8;void**_T3D9;void**_T3DA;void**_T3DB;unsigned _T3DC;struct _fat_ptr _T3DD;struct Cyc_Tctyp_CVTEnv _T3DE;struct Cyc_Tcenv_Tenv*_T3DF;unsigned _T3E0;_T0=env;_T3E0=_T0.loc;_T1=env;_T3DF=_T1.te;{unsigned loc=_T3E0;struct Cyc_Tcenv_Tenv*te=_T3DF;{
# 752
void*_T3E1=Cyc_Absyn_compress(t);struct _tuple1*_T3E2;enum Cyc_Absyn_AggrKind _T3E3;struct Cyc_List_List*_T3E4;struct Cyc_List_List*_T3E5;struct Cyc_Absyn_Vardecl*_T3E6;void*_T3E7;struct Cyc_Absyn_Exp*_T3E8;struct Cyc_Absyn_Exp*_T3E9;struct Cyc_Absyn_Exp*_T3EA;struct Cyc_Absyn_Exp*_T3EB;struct Cyc_List_List*_T3EC;struct Cyc_Absyn_VarargInfo*_T3ED;int _T3EE;struct Cyc_List_List*_T3EF;unsigned _T3F0;void*_T3F1;void*_T3F2;void*_T3F3;void*_T3F4;void*_T3F5;void*_T3F6;const char*_T3F7;void*_T3F8;void*_T3F9;_T2=(int*)_T3E1;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E1;_T4=_T3FA->f1;_T3F9=(void*)_T4;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E1;_T6=& _T5->f2;_T3F8=(struct Cyc_List_List**)_T6;}{void*c=_T3F9;struct Cyc_List_List**targsp=_T3F8;
# 754
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr,t);goto _LL3;}case 3:{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3E1;_T7=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3E1;_T8=& _T7->f1;_T3F9=(struct Cyc_Core_Opt**)_T8;_T3F7=_T3FA->f5;}{struct Cyc_Core_Opt**k=_T3F9;const char*name=_T3F7;
# 758
if(name!=0)goto _TL130;{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;
_T3FA.f1=_tag_fat("Unnamed Cvar ",sizeof(char),14U);_T9=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB;_T3FB.tag=2;_T3FB.f1=t;_TA=_T3FB;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB=_TA;void*_T3FC[2];_TB=_T3FC + 0;*_TB=& _T3FA;_TC=_T3FC + 1;*_TC=& _T3FB;_TD=loc;_TE=_tag_fat(_T3FC,sizeof(void*),2);Cyc_Warn_err2(_TD,_TE);}}goto _TL131;_TL130: _TL131: _TF=k;_T10=*_TF;
# 761
if(_T10==0)goto _TL134;else{goto _TL135;}_TL135: _T11=expected_kind;_T12=k;_T13=*_T12;_T14=_T13->v;_T15=(struct Cyc_Absyn_Kind*)_T14;_T16=
Cyc_Kinds_kind_leq(_T11,_T15);
# 761
if(_T16)goto _TL136;else{goto _TL132;}_TL136: _T17=k;_T18=*_T17;_T19=
# 763
_check_null(_T18);_T1A=_T19->v;_T1B=(struct Cyc_Absyn_Kind*)_T1A;_T1C=expected_kind;_T1D=Cyc_Kinds_kind_leq(_T1B,_T1C);
# 761
if(_T1D)goto _TL132;else{goto _TL134;}
# 764
_TL134: _T1E=k;*_T1E=Cyc_Kinds_kind_to_opt(expected_kind);goto _TL133;_TL132: _TL133: goto _LL3;}case 1:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3E1;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3E1;_T20=& _T1F->f1;_T3F9=(struct Cyc_Core_Opt**)_T20;_T21=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3E1;_T22=& _T21->f2;_T3F8=(void**)_T22;}{struct Cyc_Core_Opt**k=_T3F9;void**ref=_T3F8;_T23=k;_T24=*_T23;
# 768
if(_T24==0)goto _TL139;else{goto _TL13A;}_TL13A: _T25=expected_kind;_T26=k;_T27=*_T26;_T28=_T27->v;_T29=(struct Cyc_Absyn_Kind*)_T28;_T2A=
Cyc_Kinds_kind_leq(_T25,_T29);
# 768
if(_T2A)goto _TL13B;else{goto _TL137;}_TL13B: _T2B=k;_T2C=*_T2B;_T2D=
# 770
_check_null(_T2C);_T2E=_T2D->v;_T2F=(struct Cyc_Absyn_Kind*)_T2E;_T30=expected_kind;_T31=Cyc_Kinds_kind_leq(_T2F,_T30);
# 768
if(_T31)goto _TL137;else{goto _TL139;}
# 771
_TL139: _T32=k;*_T32=Cyc_Kinds_kind_to_opt(expected_kind);goto _TL138;_TL137: _TL138: _T33=env;_T34=_T33.fn_result;
# 773
if(!_T34)goto _TL13C;_T35=env;_T36=_T35.generalize_evars;if(!_T36)goto _TL13C;_T37=expected_kind;_T38=_T37->kind;_T39=(int)_T38;
if(_T39!=3)goto _TL13E;_T3A=te;_T3B=_T3A->tempest_generalize;if(_T3B)goto _TL13E;else{goto _TL140;}
_TL140: _T3C=ref;*_T3C=Cyc_Absyn_heap_rgn_type;goto _LL3;
# 778
_TL13E: _T3D=expected_kind;_T3E=_T3D->kind;_T3F=(int)_T3E;if(_T3F!=7)goto _TL141;_T40=te;_T41=_T40->tempest_generalize;if(_T41)goto _TL141;else{goto _TL143;}
_TL143: _T42=ref;*_T42=Cyc_Absyn_al_qual_type;goto _LL3;_TL141: goto _TL13D;_TL13C: _TL13D: _T43=env;_T44=_T43.inst_type_evar;
# 784
if(!_T44)goto _TL144;_T45=expected_kind;_T46=_T45->kind;_T47=(int)_T46;if(_T47!=7)goto _TL144;_T48=te;_T49=_T48->tempest_generalize;if(_T49)goto _TL144;else{goto _TL146;}
# 787
_TL146: _T4A=ref;*_T4A=Cyc_Absyn_al_qual_type;goto _LL3;_TL144: _T4B=env;_T4C=_T4B.generalize_evars;
# 791
if(!_T4C)goto _TL147;_T4D=expected_kind;_T4E=_T4D->kind;_T4F=(int)_T4E;if(_T4F==5)goto _TL147;_T50=expected_kind;_T51=_T50->kind;_T52=(int)_T51;if(_T52==6)goto _TL147;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FA=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3FA->tag=2;
# 794
_T3FA->f1=0;_T3FA->f2=expected_kind;_T53=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FA;}_T54=(void*)_T53;{struct Cyc_Absyn_Tvar*v=Cyc_Tcutil_new_tvar(_T54);_T55=ref;
*_T55=Cyc_Absyn_var_type(v);_T3F9=v;goto _LLB;}_TL147: _T56=env;_T57=_T56.free_evars;_T58=t;_T59=put_in_effect;
# 798
env.free_evars=Cyc_Tctyp_add_free_evar(_T57,_T58,_T59);goto _LL3;}case 2:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3E1;_T3F9=_T3FA->f1;}_LLB: {struct Cyc_Absyn_Tvar*v=_T3F9;_T5A=v;_T5B=_T5A->kind;{
# 801
void*_T3FA=Cyc_Kinds_compress_kb(_T5B);void*_T3FB;_T5C=(int*)_T3FA;_T5D=*_T5C;if(_T5D!=1)goto _TL149;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3FC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FA;_T5E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FA;_T5F=& _T5E->f1;_T3FB=(struct Cyc_Core_Opt**)_T5F;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3FB;_T60=f;{struct Cyc_Core_Opt*_T3FC=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FD=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3FD->tag=2;
_T3FD->f1=0;_T3FD->f2=expected_kind;_T62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FD;}_T3FC->v=(void*)_T62;_T61=(struct Cyc_Core_Opt*)_T3FC;}*_T60=_T61;goto _LL1C;}_TL149: goto _LL1C;_LL1C:;}_T63=loc;_T64=env;_T65=_T64.kind_env;_T66=v;_T67=env;_T68=_T67.allow_aqual_bounds;
# 807
env.kind_env=Cyc_Tctyp_add_free_tvar(_T63,_T65,_T66,_T68);_T69=env;_T6A=_T69.free_vars;_T6B=v;_T6C=put_in_effect;
# 810
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T6A,_T6B,_T6C);_T6D=v;_T6E=_T6D->kind;{
# 812
void*_T3FA=Cyc_Kinds_compress_kb(_T6E);struct Cyc_Absyn_Kind*_T3FB;void*_T3FC;_T6F=(int*)_T3FA;_T70=*_T6F;if(_T70!=2)goto _TL14B;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FA;_T71=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FA;_T72=& _T71->f1;_T3FC=(struct Cyc_Core_Opt**)_T72;_T3FB=_T3FD->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3FC;struct Cyc_Absyn_Kind*k=_T3FB;_T73=
# 814
Cyc_Kinds_kind_leq(expected_kind,k);if(!_T73)goto _TL14D;_T74=f;{struct Cyc_Core_Opt*_T3FD=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FE=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3FE->tag=2;
_T3FE->f1=0;_T3FE->f2=expected_kind;_T76=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FE;}_T3FD->v=(void*)_T76;_T75=(struct Cyc_Core_Opt*)_T3FD;}*_T74=_T75;goto _TL14E;_TL14D: _TL14E: goto _LL21;}_TL14B: goto _LL21;_LL21:;}goto _LL3;}case 10:{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T3E1;_T77=_T3FA->f1;{struct Cyc_Absyn_TypeDecl _T3FB=*_T77;_T3F9=_T3FB.r;}_T78=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T3E1;_T79=& _T78->f2;_T3F8=(void***)_T79;}{void*td=_T3F9;void***topt=_T3F8;_T7A=
# 824
Cyc_Absyn_compress(t);{void*new_typ=Cyc_Tcutil_copy_type(_T7A);{struct Cyc_Absyn_Datatypedecl*_T3FA;struct Cyc_Absyn_Enumdecl*_T3FB;struct Cyc_Absyn_Aggrdecl*_T3FC;_T7B=td;_T7C=(int*)_T7B;_T7D=*_T7C;switch(_T7D){case 0: _T7E=td;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T3FD=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T7E;_T3FC=_T3FD->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T3FC;_T7F=env;_T80=_T7F.te;_T81=_T80->in_extern_c_include;
# 827
if(!_T81)goto _TL150;_T82=ad;
_T82->sc=4U;goto _TL151;_TL150: _TL151:
 Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1: _T83=td;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T3FD=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T83;_T3FB=_T3FD->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T3FB;_T84=env;_T85=_T84.te;_T86=_T85->in_extern_c_include;
# 831
if(!_T86)goto _TL152;_T87=ed;
_T87->sc=4U;goto _TL153;_TL152: _TL153:
 Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _T88=td;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T3FD=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T88;_T3FA=_T3FD->f1;}{struct Cyc_Absyn_Datatypedecl*dd=_T3FA;
# 835
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}_T89=topt;{void**_T3FA=_cycalloc(sizeof(void*));
# 837
*_T3FA=new_typ;_T8A=(void**)_T3FA;}*_T89=_T8A;_T8B=
Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);return _T8B;}}case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3E1;_T8C=_T3FA->f1;_T3F9=_T8C.elt_type;_T8D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3E1;_T8E=& _T8D->f1.elt_tq;_T3F8=(struct Cyc_Absyn_Tqual*)_T8E;_T8F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3E1;_T90=& _T8F->f1.ptr_atts.eff;_T3F6=(void**)_T90;_T91=_T3FA->f1;_T92=_T91.ptr_atts;_T3F5=_T92.nullable;_T93=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3E1;_T94=& _T93->f1.ptr_atts.bounds;_T3F4=(void**)_T94;_T95=_T3FA->f1;_T96=_T95.ptr_atts;_T3F3=_T96.zero_term;_T97=_T3FA->f1;_T98=_T97.ptr_atts;_T3F2=_T98.autoreleased;_T99=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3E1;_T9A=& _T99->f1.ptr_atts.aqual;_T3F1=(void**)_T9A;}{void*t1=_T3F9;struct Cyc_Absyn_Tqual*tqp=_T3F8;void**eff_type=_T3F6;void*nullable=_T3F5;void**b=_T3F4;void*zt=_T3F3;void*rel=_T3F2;void**aq=_T3F1;_T9B=aq;{
# 842
void*override_aq=*_T9B;_T9C=eff_type;{
void*_T3FA=*_T9C;_T9D=(int*)_T3FA;_T9E=*_T9D;if(_T9E!=0)goto _TL154;_T9F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TA0=_T9F->f1;_TA1=(int*)_TA0;_TA2=*_TA1;switch(_TA2){case 7: _TA3=eff_type;
# 845
*_TA3=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_un_qual_type;goto _LL2D;case 8: _TA4=eff_type;
# 849
*_TA4=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_rc_qual_type;goto _LL2D;default: goto _LL32;}goto _TL155;_TL154: _LL32: goto _LL2D;_TL155: _LL2D:;}_TA5=override_aq;_TA6=aq;_TA7=*_TA6;
# 855
if(_TA5==_TA7)goto _TL157;_TA8=aq;{
void*_T3FA=*_TA8;struct Cyc_List_List*_T3FB;_TA9=(int*)_T3FA;_TAA=*_TA9;if(_TAA!=0)goto _TL159;_TAB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TAC=_TAB->f1;_TAD=(int*)_TAC;_TAE=*_TAD;if(_TAE!=17)goto _TL15B;_TAF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TB0=_TAF->f2;if(_TB0==0)goto _TL15D;_TB1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TB2=_TB1->f2;_TB3=(struct Cyc_List_List*)_TB2;_TB4=_TB3->hd;_TB5=(int*)_TB4;_TB6=*_TB5;if(_TB6!=1)goto _TL15F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3FC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TB7=_T3FC->f2;{struct Cyc_List_List _T3FD=*_TB7;_T3FB=_T3FD.tl;}}{struct Cyc_List_List*bnd=_T3FB;goto _LL34;}_TL15F: goto _LL37;_TL15D: goto _LL37;_TL15B: goto _LL37;_TL159: _LL37:{struct Cyc_Warn_String_Warn_Warg_struct _T3FC;_T3FC.tag=0;
# 860
_T3FC.f1=_tag_fat("`U and `RC shorthands override explicit aqual",sizeof(char),46U);_TB8=_T3FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FC=_TB8;void*_T3FD[1];_TB9=_T3FD + 0;*_TB9=& _T3FC;_TBA=loc;_TBB=_tag_fat(_T3FD,sizeof(void*),1);Cyc_Warn_warn2(_TBA,_TBB);}_LL34:;}_TBC=aq;
# 862
*_TBC=override_aq;goto _TL158;_TL157: _TL158:
# 864
 Cyc_Tctyp_check_constrain_ptrbnd(loc,t,expected_kind);_TBD=env;_TBE=& Cyc_Kinds_ak;_TBF=(struct Cyc_Absyn_Kind*)_TBE;_TC0=t1;
env=Cyc_Tctyp_i_check_type(_TBD,_TBF,_TC0,1,1);_TC1=env;_TC2=& Cyc_Kinds_ek;_TC3=(struct Cyc_Absyn_Kind*)_TC2;_TC4=eff_type;_TC5=*_TC4;
env=Cyc_Tctyp_i_check_type(_TC1,_TC3,_TC5,1,1);_TC6=env;_TC7=& Cyc_Kinds_boolk;_TC8=(struct Cyc_Absyn_Kind*)_TC7;_TC9=zt;
env=Cyc_Tctyp_i_check_type(_TC6,_TC8,_TC9,0,1);_TCA=env;_TCB=& Cyc_Kinds_boolk;_TCC=(struct Cyc_Absyn_Kind*)_TCB;_TCD=nullable;
env=Cyc_Tctyp_i_check_type(_TCA,_TCC,_TCD,0,1);_TCE=env;_TCF=& Cyc_Kinds_ptrbk;_TD0=(struct Cyc_Absyn_Kind*)_TCF;_TD1=b;_TD2=*_TD1;_TD3=allow_abs_aggr;
env=Cyc_Tctyp_i_check_type(_TCE,_TD0,_TD2,0,_TD3);_TD4=env;_TD5=& Cyc_Kinds_boolk;_TD6=(struct Cyc_Absyn_Kind*)_TD5;_TD7=rel;
env=Cyc_Tctyp_i_check_type(_TD4,_TD6,_TD7,0,1);_TD8=aq;{
void*_T3FA=*_TD8;struct Cyc_List_List*_T3FB;_TD9=(int*)_T3FA;_TDA=*_TD9;if(_TDA!=0)goto _TL161;_TDB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TDC=_TDB->f1;_TDD=(int*)_TDC;_TDE=*_TDD;if(_TDE!=17)goto _TL163;_TDF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TE0=_TDF->f2;if(_TE0==0)goto _TL165;_TE1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TE2=_TE1->f2;_TE3=(struct Cyc_List_List*)_TE2;_TE4=_TE3->hd;_TE5=(int*)_TE4;_TE6=*_TE5;if(_TE6!=1)goto _TL167;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3FC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3FA;_TE7=_T3FC->f2;{struct Cyc_List_List _T3FD=*_TE7;_T3FB=_T3FD.tl;}}{struct Cyc_List_List*bnd=_T3FB;_TE8=env;_TE9=_TE8.def_aliasable;
# 873
if(!_TE9)goto _TL169;_TEA=aq;
*_TEA=Cyc_Absyn_al_qual_type;goto _TL16A;
# 876
_TL169: _TEB=_check_null(bnd);_TEC=Cyc_Absyn_rtd_qual_type;_TEB->hd=(void*)_TEC;_TL16A: goto _LL39;}_TL167: goto _LL3C;_TL165: goto _LL3C;_TL163: goto _LL3C;_TL161: _LL3C: goto _LL39;_LL39:;}_TED=env;_TEE=& Cyc_Kinds_aqk;_TEF=(struct Cyc_Absyn_Kind*)_TEE;_TF0=aq;_TF1=*_TF0;
# 882
env=Cyc_Tctyp_i_check_type(_TED,_TEF,_TF1,0,1);_TF2=tqp;_TF3=env;_TF4=_TF3.loc;_TF5=tqp;_TF6=_TF5->print_const;_TF7=t1;
_TF2->real_const=Cyc_Tcutil_extract_const_from_typedef(_TF4,_TF6,_TF7);_TF8=zt;_TFA=
# 887
Cyc_Tcutil_is_char_type(t1);if(!_TFA)goto _TL16B;_TF9=Cyc_Absyn_true_type;goto _TL16C;_TL16B: _TF9=Cyc_Absyn_false_type;_TL16C: Cyc_Unify_unify(_TF8,_TF9);{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);_TFB=is_zero_terminated;
if(!_TFB)goto _TL16D;_TFC=Cyc_Tcutil_admits_zero(t1);if(_TFC)goto _TL16D;else{goto _TL16F;}
_TL16F:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("cannot have a pointer to zero-terminated ",sizeof(char),42U);_TFD=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_TFD;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB;_T3FB.tag=2;_T3FB.f1=t1;_TFE=_T3FB;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB=_TFE;{struct Cyc_Warn_String_Warn_Warg_struct _T3FC;_T3FC.tag=0;_T3FC.f1=_tag_fat(" elements",sizeof(char),10U);_TFF=_T3FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FC=_TFF;void*_T3FD[3];_T100=_T3FD + 0;*_T100=& _T3FA;_T101=_T3FD + 1;*_T101=& _T3FB;_T102=_T3FD + 2;*_T102=& _T3FC;_T103=loc;_T104=_tag_fat(_T3FD,sizeof(void*),3);Cyc_Warn_err2(_T103,_T104);}}}goto _TL16E;_TL16D: _TL16E: _T105=b;_T106=*_T105;_T107=
# 893
Cyc_Tcutil_is_cvar_type(_T106);if(_T107)goto _TL170;else{goto _TL172;}
_TL172: _T108=Cyc_Absyn_bounds_one();_T109=b;_T10A=*_T109;{struct Cyc_Absyn_Exp*e=Cyc_Tcutil_get_bounds_exp(_T108,_T10A);
if(e==0)goto _TL173;{
struct _tuple13 _T3FA=Cyc_Evexp_eval_const_uint_exp(e);int _T3FB;unsigned _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{unsigned sz=_T3FC;int known=_T3FB;_T10B=is_zero_terminated;
if(!_T10B)goto _TL175;_T10C=known;if(_T10C)goto _TL178;else{goto _TL177;}_TL178: if(sz < 1U)goto _TL177;else{goto _TL175;}
_TL177:{struct Cyc_Warn_String_Warn_Warg_struct _T3FD;_T3FD.tag=0;_T3FD.f1=_tag_fat("zero-terminated pointer cannot point to empty sequence",sizeof(char),55U);_T10D=_T3FD;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FD=_T10D;void*_T3FE[1];_T10E=_T3FE + 0;*_T10E=& _T3FD;_T10F=loc;_T110=_tag_fat(_T3FE,sizeof(void*),1);Cyc_Warn_err2(_T10F,_T110);}goto _TL176;_TL175: _TL176:;}}goto _TL174;_TL173: _TL174:;}goto _TL171;_TL170: _TL171: goto _LL3;}}}case 9:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T3E1;_T3F9=_T3FA->f1;}{struct Cyc_Absyn_Exp*e=_T3F9;_T111=
# 906
Cyc_Tcenv_allow_valueof(te);_T112=e;Cyc_Tcexp_tcExp(_T111,0,_T112);_T113=
Cyc_Tcutil_coerce_uint_type(e);if(_T113)goto _TL179;else{goto _TL17B;}
_TL17B:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("valueof_t requires an int expression",sizeof(char),37U);_T114=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T114;void*_T3FB[1];_T115=_T3FB + 0;*_T115=& _T3FA;_T116=loc;_T117=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T116,_T117);}goto _TL17A;_TL179: _TL17A:
 env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL3;}case 11:{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T3E1;_T3F9=_T3FA->f1;}{struct Cyc_Absyn_Exp*e=_T3F9;_T118=
# 915
Cyc_Tcenv_allow_valueof(te);_T119=e;Cyc_Tcexp_tcExp(_T118,0,_T119);goto _LL3;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3E1;_T11A=_T3FA->f1;_T3F9=_T11A.elt_type;_T11B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3E1;_T11C=& _T11B->f1.tq;_T3F8=(struct Cyc_Absyn_Tqual*)_T11C;_T11D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3E1;_T11E=& _T11D->f1.num_elts;_T3F6=(struct Cyc_Absyn_Exp**)_T11E;_T11F=_T3FA->f1;_T3F5=_T11F.zero_term;_T120=_T3FA->f1;_T3F0=_T120.zt_loc;}{void*t1=_T3F9;struct Cyc_Absyn_Tqual*tqp=_T3F8;struct Cyc_Absyn_Exp**eptr=_T3F6;void*zt=_T3F5;unsigned ztl=_T3F0;_T121=eptr;{
# 920
struct Cyc_Absyn_Exp*e=*_T121;_T122=env;_T123=& Cyc_Kinds_mk;_T124=(struct Cyc_Absyn_Kind*)_T123;_T125=t1;_T126=allow_abs_aggr;
env=Cyc_Tctyp_i_check_type(_T122,_T124,_T125,1,_T126);_T127=tqp;_T128=loc;_T129=tqp;_T12A=_T129->print_const;_T12B=t1;
_T127->real_const=Cyc_Tcutil_extract_const_from_typedef(_T128,_T12A,_T12B);{
# 924
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);_T12C=is_zero_terminated;
if(!_T12C)goto _TL17C;_T12D=
# 927
Cyc_Tcutil_admits_zero(t1);if(_T12D)goto _TL17E;else{goto _TL180;}
_TL180:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("cannot have a zero-terminated array of ",sizeof(char),40U);_T12E=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T12E;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB;_T3FB.tag=2;_T3FB.f1=t1;_T12F=_T3FB;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3FB=_T12F;{struct Cyc_Warn_String_Warn_Warg_struct _T3FC;_T3FC.tag=0;_T3FC.f1=_tag_fat(" elements",sizeof(char),10U);_T130=_T3FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FC=_T130;void*_T3FD[3];_T131=_T3FD + 0;*_T131=& _T3FA;_T132=_T3FD + 1;*_T132=& _T3FB;_T133=_T3FD + 2;*_T133=& _T3FC;_T134=loc;_T135=_tag_fat(_T3FD,sizeof(void*),3);Cyc_Warn_err2(_T134,_T135);}}}goto _TL17F;_TL17E: _TL17F: goto _TL17D;_TL17C: _TL17D:
# 932
 if(e!=0)goto _TL181;_T136=is_zero_terminated;
# 935
if(_T136)goto _TL183;else{goto _TL185;}
_TL185:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("array bound defaults to 1 here",sizeof(char),31U);_T137=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T137;void*_T3FB[1];_T138=_T3FB + 0;*_T138=& _T3FA;_T139=loc;_T13A=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_warn2(_T139,_T13A);}goto _TL184;_TL183: _TL184: _T13B=eptr;
e=Cyc_Absyn_uint_exp(1U,0U);*_T13B=e;goto _TL182;_TL181: _TL182: _T13C=
# 939
Cyc_Tcenv_allow_valueof(te);_T13D=e;Cyc_Tcexp_tcExp(_T13C,0,_T13D);_T13E=
Cyc_Tcutil_coerce_uint_type(e);if(_T13E)goto _TL186;else{goto _TL188;}
_TL188:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("array bounds expression is not an unsigned int",sizeof(char),47U);_T13F=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T13F;void*_T3FB[1];_T140=_T3FB + 0;*_T140=& _T3FA;_T141=loc;_T142=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T141,_T142);}goto _TL187;_TL186: _TL187:
 env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple13 _T3FA=Cyc_Evexp_eval_const_uint_exp(e);int _T3FB;unsigned _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{unsigned sz=_T3FC;int known=_T3FB;_T143=is_zero_terminated;
# 945
if(!_T143)goto _TL189;_T144=known;if(!_T144)goto _TL189;if(sz >= 1U)goto _TL189;{struct Cyc_Warn_String_Warn_Warg_struct _T3FD;_T3FD.tag=0;
_T3FD.f1=_tag_fat("zero terminated array cannot have zero elements",sizeof(char),48U);_T145=_T3FD;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FD=_T145;void*_T3FE[1];_T146=_T3FE + 0;*_T146=& _T3FD;_T147=loc;_T148=_tag_fat(_T3FE,sizeof(void*),1);Cyc_Warn_warn2(_T147,_T148);}goto _TL18A;_TL189: _TL18A: _T149=known;
# 948
if(!_T149)goto _TL18B;if(sz >= 1U)goto _TL18B;_T14A=Cyc_Flags_tovc_r;if(!_T14A)goto _TL18B;{struct Cyc_Warn_String_Warn_Warg_struct _T3FD;_T3FD.tag=0;
_T3FD.f1=_tag_fat("0-element arrays are supported only with gcc, changing to 1",sizeof(char),60U);_T14B=_T3FD;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FD=_T14B;void*_T3FE[1];_T14C=_T3FE + 0;*_T14C=& _T3FD;_T14D=loc;_T14E=_tag_fat(_T3FE,sizeof(void*),1);Cyc_Warn_warn2(_T14D,_T14E);}_T14F=eptr;
*_T14F=Cyc_Absyn_uint_exp(1U,0U);goto _TL18C;_TL18B: _TL18C: goto _LL3;}}}}}case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T150=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T151=& _T150->f1.tvars;_T3F9=(struct Cyc_List_List**)_T151;_T152=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T153=& _T152->f1.effect;_T3F8=(void**)_T153;_T154=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T155=& _T154->f1.ret_tqual;_T3F6=(struct Cyc_Absyn_Tqual*)_T155;_T156=_T3FA->f1;_T3F5=_T156.ret_type;_T157=_T3FA->f1;_T3EF=_T157.args;_T158=_T3FA->f1;_T3EE=_T158.c_varargs;_T159=_T3FA->f1;_T3ED=_T159.cyc_varargs;_T15A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T15B=& _T15A->f1.qual_bnd;_T3F4=(struct Cyc_List_List**)_T15B;_T15C=_T3FA->f1;_T3EC=_T15C.attributes;_T15D=_T3FA->f1;_T3EB=_T15D.checks_clause;_T15E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T15F=& _T15E->f1.checks_assn;_T3F3=(struct Cyc_AssnDef_ExistAssnFn**)_T15F;_T160=_T3FA->f1;_T3EA=_T160.requires_clause;_T161=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T162=& _T161->f1.requires_assn;_T3F2=(struct Cyc_AssnDef_ExistAssnFn**)_T162;_T163=_T3FA->f1;_T3E9=_T163.ensures_clause;_T164=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T165=& _T164->f1.ensures_assn;_T3F1=(struct Cyc_AssnDef_ExistAssnFn**)_T165;_T166=_T3FA->f1;_T3E8=_T166.throws_clause;_T167=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3E1;_T168=& _T167->f1.throws_assn;_T3E7=(struct Cyc_AssnDef_ExistAssnFn**)_T168;_T169=_T3FA->f1;_T3E6=_T169.return_value;_T16A=_T3FA->f1;_T3E5=_T16A.arg_vardecls;_T16B=_T3FA->f1;_T3E4=_T16B.effconstr;}{struct Cyc_List_List**btvs=_T3F9;void**eff=_T3F8;struct Cyc_Absyn_Tqual*rtq=_T3F6;void*tr=_T3F5;struct Cyc_List_List*args=_T3EF;int c_vararg=_T3EE;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T3ED;struct Cyc_List_List**qb=(struct Cyc_List_List**)_T3F4;struct Cyc_List_List*atts=_T3EC;struct Cyc_Absyn_Exp*chk=_T3EB;struct Cyc_AssnDef_ExistAssnFn**chk_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3F3;struct Cyc_Absyn_Exp*req=_T3EA;struct Cyc_AssnDef_ExistAssnFn**req_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3F2;struct Cyc_Absyn_Exp*ens=_T3E9;struct Cyc_AssnDef_ExistAssnFn**ens_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3F1;struct Cyc_Absyn_Exp*thrws=_T3E8;struct Cyc_AssnDef_ExistAssnFn**thrws_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3E7;struct Cyc_Absyn_Vardecl*return_vd=_T3E6;struct Cyc_List_List*arg_vds=_T3E5;struct Cyc_List_List*effc=_T3E4;
# 961
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
_TL190: if(atts!=0)goto _TL18E;else{goto _TL18F;}
_TL18E: _T16C=atts;{void*_T3FA=_T16C->hd;int _T3FB;int _T3FC;enum Cyc_Absyn_Format_Type _T3FD;_T16D=(int*)_T3FA;_T16E=*_T16D;switch(_T16E){case 1:
 is_stdcall=1;goto _LL44;case 2:
 is_cdecl=1;goto _LL44;case 3:
 is_fastcall=1;goto _LL44;case 19:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_T3FE=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_T3FA;_T3FD=_T3FE->f1;_T3FC=_T3FE->f2;_T3FB=_T3FE->f3;}{enum Cyc_Absyn_Format_Type fmttype=_T3FD;int i=_T3FC;int j=_T3FB;
# 974
num_formats=num_formats + 1;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;goto _LL44;}default: _T16F=atts;_T170=_T16F->hd;_T171=
# 980
Cyc_Atts_fntype_att(_T170);if(_T171)goto _TL192;else{goto _TL194;}
_TL194:{struct Cyc_Warn_String_Warn_Warg_struct _T3FE;_T3FE.tag=0;_T3FE.f1=_tag_fat("bad function type attribute ",sizeof(char),29U);_T172=_T3FE;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FE=_T172;{struct Cyc_Warn_Attribute_Warn_Warg_struct _T3FF;_T3FF.tag=10;_T174=atts;_T3FF.f1=_T174->hd;_T173=_T3FF;}{struct Cyc_Warn_Attribute_Warn_Warg_struct _T3FF=_T173;void*_T400[2];_T175=_T400 + 0;*_T175=& _T3FE;_T176=_T400 + 1;*_T176=& _T3FF;_T177=loc;_T178=_tag_fat(_T400,sizeof(void*),2);Cyc_Warn_err2(_T177,_T178);}}goto _TL193;_TL192: _TL193:;}_LL44:;}_T179=atts;
# 968
atts=_T179->tl;goto _TL190;_TL18F: _T17A=is_stdcall + is_cdecl;_T17B=is_fastcall;_T17C=_T17A + _T17B;
# 983
if(_T17C <= 1)goto _TL195;{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;
_T3FA.f1=_tag_fat("function can't have multiple calling conventions",sizeof(char),49U);_T17D=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T17D;void*_T3FB[1];_T17E=_T3FB + 0;*_T17E=& _T3FA;_T17F=loc;_T180=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T17F,_T180);}goto _TL196;_TL195: _TL196:
 if(num_formats <= 1)goto _TL197;{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;
_T3FA.f1=_tag_fat("function can't have multiple format attributes",sizeof(char),47U);_T181=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T181;void*_T3FB[1];_T182=_T3FB + 0;*_T182=& _T3FA;_T183=loc;_T184=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T183,_T184);}goto _TL198;_TL197: _TL198: _T185=loc;_T186=btvs;_T187=*_T186;
# 990
Cyc_Tcutil_check_unique_tvars(_T185,_T187);_T188=btvs;{
struct Cyc_List_List*b=*_T188;_TL19C: if(b!=0)goto _TL19A;else{goto _TL19B;}
_TL19A: _T189=b;_T18A=_T189->hd;_T18B=(struct Cyc_Absyn_Tvar*)_T18A;_T18B->identity=Cyc_Tcutil_new_tvar_id();_T18C=env;_T18D=_T18C.kind_env;_T18E=b;_T18F=_T18E->hd;_T190=(struct Cyc_Absyn_Tvar*)_T18F;
env.kind_env=Cyc_Tctyp_add_bound_tvar(_T18D,_T190);_T191=b;_T192=_T191->hd;_T193=(struct Cyc_Absyn_Tvar*)_T192;_T194=_T193->kind;{
void*_T3FA=Cyc_Kinds_compress_kb(_T194);_T195=(int*)_T3FA;_T196=*_T195;if(_T196!=0)goto _TL19D;_T197=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3FA;_T198=_T197->f1;_T199=(struct Cyc_Absyn_Kind*)_T198;_T19A=_T199->kind;if(_T19A!=Cyc_Absyn_MemKind)goto _TL19F;{struct Cyc_Warn_String_Warn_Warg_struct _T3FB;_T3FB.tag=0;
# 996
_T3FB.f1=_tag_fat("function abstracts Mem type variable ",sizeof(char),38U);_T19B=_T3FB;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FB=_T19B;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3FC;_T3FC.tag=7;_T19D=b;_T19E=_T19D->hd;_T3FC.f1=(struct Cyc_Absyn_Tvar*)_T19E;_T19C=_T3FC;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3FC=_T19C;void*_T3FD[2];_T19F=_T3FD + 0;*_T19F=& _T3FB;_T1A0=_T3FD + 1;*_T1A0=& _T3FC;_T1A1=loc;_T1A2=_tag_fat(_T3FD,sizeof(void*),2);Cyc_Warn_err2(_T1A1,_T1A2);}}goto _LL4F;_TL19F: goto _LL52;_TL19D: _LL52: goto _LL4F;_LL4F:;}_T1A3=b;
# 991
b=_T1A3->tl;goto _TL19C;_TL19B:;}{struct Cyc_Tctyp_CVTEnv _T3FA;
# 1002
_T3FA.loc=loc;_T3FA.te=te;_T1A5=env;_T3FA.kind_env=_T1A5.kind_env;_T3FA.fn_result=1;_T3FA.def_aliasable=1;_T3FA.inst_type_evar=0;_T1A6=env;_T3FA.generalize_evars=_T1A6.generalize_evars;_T3FA.allow_aqual_bounds=1;_T3FA.free_vars=0;_T3FA.free_evars=0;_T3FA.free_qualvars=0;_T1A4=_T3FA;}{struct Cyc_Tctyp_CVTEnv new_env=_T1A4;_T1A7=new_env;_T1A8=& Cyc_Kinds_mk;_T1A9=(struct Cyc_Absyn_Kind*)_T1A8;_T1AA=tr;
new_env=Cyc_Tctyp_i_check_type(_T1A7,_T1A9,_T1AA,1,1);_T1AB=rtq;_T1AC=loc;_T1AD=rtq;_T1AE=_T1AD->print_const;_T1AF=tr;
_T1AB->real_const=Cyc_Tcutil_extract_const_from_typedef(_T1AC,_T1AE,_T1AF);
new_env.fn_result=0;{
# 1010
struct Cyc_List_List*a=args;_TL1A4: if(a!=0)goto _TL1A2;else{goto _TL1A3;}
_TL1A2: _T1B0=a;_T1B1=_T1B0->hd;{struct _tuple9*trip=(struct _tuple9*)_T1B1;_T1B2=trip;_T1B3=*_T1B2;{
void*t=_T1B3.f2;_T1B4=new_env;_T1B5=& Cyc_Kinds_mk;_T1B6=(struct Cyc_Absyn_Kind*)_T1B5;_T1B7=t;
new_env=Cyc_Tctyp_i_check_type(_T1B4,_T1B6,_T1B7,1,1);_T1B8=loc;_T1B9=trip;_T1BA=*_T1B9;_T1BB=_T1BA.f1;_T1BC=_T1BB.print_const;_T1BD=t;{
int ec=Cyc_Tcutil_extract_const_from_typedef(_T1B8,_T1BC,_T1BD);_T1BE=trip;
(*_T1BE).f1.real_const=ec;_T1BF=
# 1018
Cyc_Tcutil_is_array_type(t);if(!_T1BF)goto _TL1A5;{
# 1020
void*ptr_rgn=Cyc_Absyn_new_evar(0,0);_T1C0=new_env;_T1C1=& Cyc_Kinds_ek;_T1C2=(struct Cyc_Absyn_Kind*)_T1C1;_T1C3=ptr_rgn;
new_env=Cyc_Tctyp_i_check_type(_T1C0,_T1C2,_T1C3,1,1);_T1C4=& Cyc_Kinds_aqko;_T1C5=(struct Cyc_Core_Opt*)_T1C4;_T1C6=
Cyc_Absyn_new_evar(_T1C5,0);_T1C7=Cyc_Absyn_al_qual_type;{void*aqt=Cyc_Absyn_aqual_var_type(_T1C6,_T1C7);_T1C8=new_env;_T1C9=& Cyc_Kinds_aqk;_T1CA=(struct Cyc_Absyn_Kind*)_T1C9;_T1CB=aqt;
# 1024
new_env=Cyc_Tctyp_i_check_type(_T1C8,_T1CA,_T1CB,1,1);_T1CC=trip;
(*_T1CC).f2=Cyc_Tcutil_promote_array(t,ptr_rgn,aqt,0);}}goto _TL1A6;_TL1A5: _TL1A6:;}}}_T1CD=a;
# 1010
a=_T1CD->tl;goto _TL1A4;_TL1A3:;}
# 1029
if(cyc_vararg==0)goto _TL1A7;_T1CE=c_vararg;
if(!_T1CE)goto _TL1A9;{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("both c_vararg and cyc_vararg",sizeof(char),29U);_T1CF=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T1CF;void*_T3FB[1];_T1D0=_T3FB + 0;*_T1D0=& _T3FA;_T1D2=Cyc_Warn_impos2;{int(*_T3FC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1D2;_T1D1=_T3FC;}_T1D3=_tag_fat(_T3FB,sizeof(void*),1);_T1D1(_T1D3);}goto _TL1AA;_TL1A9: _TL1AA: _T1D4=cyc_vararg;{
struct Cyc_Absyn_VarargInfo _T3FA=*_T1D4;int _T3FB;void*_T3FC;_T3FC=_T3FA.type;_T3FB=_T3FA.inject;{void*vt=_T3FC;int vi=_T3FB;_T1D5=new_env;_T1D6=& Cyc_Kinds_mk;_T1D7=(struct Cyc_Absyn_Kind*)_T1D6;_T1D8=vt;
new_env=Cyc_Tctyp_i_check_type(_T1D5,_T1D7,_T1D8,1,1);_T1D9=cyc_vararg;_T1DA=loc;_T1DB=cyc_vararg;_T1DC=_T1DB->tq;_T1DD=_T1DC.print_const;_T1DE=vt;
_T1D9->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T1DA,_T1DD,_T1DE);_T1DF=vi;
# 1035
if(!_T1DF)goto _TL1AB;{
void*_T3FD=Cyc_Absyn_compress(vt);void*_T3FE;void*_T3FF;void*_T400;_T1E0=(int*)_T3FD;_T1E1=*_T1E0;if(_T1E1!=4)goto _TL1AD;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T401=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3FD;_T1E2=_T401->f1;_T400=_T1E2.elt_type;_T1E3=_T401->f1;_T1E4=_T1E3.ptr_atts;_T3FF=_T1E4.bounds;_T1E5=_T401->f1;_T1E6=_T1E5.ptr_atts;_T3FE=_T1E6.zero_term;}{void*et=_T400;void*bs=_T3FF;void*zt=_T3FE;{
# 1038
void*_T401=Cyc_Absyn_compress(et);_T1E7=(int*)_T401;_T1E8=*_T1E7;if(_T1E8!=0)goto _TL1AF;_T1E9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T401;_T1EA=_T1E9->f1;_T1EB=(int*)_T1EA;_T1EC=*_T1EB;if(_T1EC!=22)goto _TL1B1;_T1ED=
# 1040
Cyc_Tcutil_force_type2bool(0,zt);if(!_T1ED)goto _TL1B3;{struct Cyc_Warn_String_Warn_Warg_struct _T402;_T402.tag=0;
_T402.f1=_tag_fat("can't inject into a zeroterm pointer",sizeof(char),37U);_T1EE=_T402;}{struct Cyc_Warn_String_Warn_Warg_struct _T402=_T1EE;void*_T403[1];_T1EF=_T403 + 0;*_T1EF=& _T402;_T1F0=loc;_T1F1=_tag_fat(_T403,sizeof(void*),1);Cyc_Warn_err2(_T1F0,_T1F1);}goto _TL1B4;_TL1B3: _TL1B4: _T1F2=
Cyc_Absyn_bounds_one();_T1F3=bs;_T1F4=Cyc_Unify_unify(_T1F2,_T1F3);if(_T1F4)goto _TL1B5;else{goto _TL1B7;}
_TL1B7: _T1F5=Cyc_Tcutil_is_cvar_type(bs);if(_T1F5)goto _TL1BB;else{goto _TL1BA;}_TL1BB: _T1F6=loc;_T1F7=bs;_T1F8=
Cyc_Absyn_thinconst();_T1F9=Cyc_BansheeIf_equality_constraint(_T1F7,_T1F8);_T1FA=Cyc_BansheeIf_add_constraint(_T1F6,_T1F9);
# 1043
if(_T1FA)goto _TL1B8;else{goto _TL1BA;}
# 1045
_TL1BA:{struct Cyc_Warn_String_Warn_Warg_struct _T402;_T402.tag=0;_T402.f1=_tag_fat("can't inject into a fat pointer to datatype",sizeof(char),44U);_T1FB=_T402;}{struct Cyc_Warn_String_Warn_Warg_struct _T402=_T1FB;void*_T403[1];_T1FC=_T403 + 0;*_T1FC=& _T402;_T1FD=loc;_T1FE=_tag_fat(_T403,sizeof(void*),1);Cyc_Warn_err2(_T1FD,_T1FE);}goto _TL1B9;_TL1B8: _TL1B9: goto _TL1B6;_TL1B5: _TL1B6: goto _LL5C;_TL1B1: goto _LL5F;_TL1AF: _LL5F:{struct Cyc_Warn_String_Warn_Warg_struct _T402;_T402.tag=0;
# 1049
_T402.f1=_tag_fat("can't inject a non-datatype type",sizeof(char),33U);_T1FF=_T402;}{struct Cyc_Warn_String_Warn_Warg_struct _T402=_T1FF;void*_T403[1];_T200=_T403 + 0;*_T200=& _T402;_T201=loc;_T202=_tag_fat(_T403,sizeof(void*),1);Cyc_Warn_err2(_T201,_T202);}goto _LL5C;_LL5C:;}goto _LL57;}_TL1AD:{struct Cyc_Warn_String_Warn_Warg_struct _T401;_T401.tag=0;
# 1052
_T401.f1=_tag_fat("expecting a datatype pointer type",sizeof(char),34U);_T203=_T401;}{struct Cyc_Warn_String_Warn_Warg_struct _T401=_T203;void*_T402[1];_T204=_T402 + 0;*_T204=& _T401;_T205=loc;_T206=_tag_fat(_T402,sizeof(void*),1);Cyc_Warn_err2(_T205,_T206);}goto _LL57;_LL57:;}goto _TL1AC;_TL1AB: _TL1AC:;}}goto _TL1A8;_TL1A7: _TL1A8:
# 1056
 if(num_formats <= 0)goto _TL1BC;{
int num_args=Cyc_List_length(args);
if(fmt_desc_arg < 0)goto _TL1C0;else{goto _TL1C4;}_TL1C4: if(fmt_desc_arg > num_args)goto _TL1C0;else{goto _TL1C3;}_TL1C3: if(fmt_arg_start < 0)goto _TL1C0;else{goto _TL1C2;}_TL1C2: if(cyc_vararg==0)goto _TL1C6;else{goto _TL1C1;}_TL1C6: _T207=c_vararg;if(_T207)goto _TL1C1;else{goto _TL1C5;}_TL1C5: if(fmt_arg_start!=0)goto _TL1C0;else{goto _TL1C1;}_TL1C1: if(cyc_vararg!=0)goto _TL1C7;else{goto _TL1C8;}_TL1C8: _T208=c_vararg;if(_T208)goto _TL1C7;else{goto _TL1BE;}_TL1C7: _T209=fmt_arg_start;_T20A=num_args + 1;if(_T209!=_T20A)goto _TL1C0;else{goto _TL1BE;}
# 1063
_TL1C0:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("bad format descriptor",sizeof(char),22U);_T20B=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T20B;void*_T3FB[1];_T20C=_T3FB + 0;*_T20C=& _T3FA;_T20D=loc;_T20E=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T20D,_T20E);}goto _TL1BF;
# 1066
_TL1BE: _T210=Cyc_List_nth;{struct _tuple9*(*_T3FA)(struct Cyc_List_List*,int)=(struct _tuple9*(*)(struct Cyc_List_List*,int))_T210;_T20F=_T3FA;}_T211=args;_T212=fmt_desc_arg - 1;_T213=_T20F(_T211,_T212);{struct _tuple9 _T3FA=*_T213;void*_T3FB;_T3FB=_T3FA.f2;{void*t=_T3FB;{
# 1068
void*_T3FC=Cyc_Absyn_compress(t);void*_T3FD;void*_T3FE;_T214=(int*)_T3FC;_T215=*_T214;if(_T215!=4)goto _TL1C9;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3FC;_T216=_T3FF->f1;_T3FE=_T216.elt_type;_T217=_T3FF->f1;_T218=_T217.ptr_atts;_T3FD=_T218.bounds;}{void*et=_T3FE;void*b=_T3FD;_T219=
# 1071
Cyc_Tcutil_is_char_type(et);if(_T219)goto _TL1CB;else{goto _TL1CD;}
_TL1CD:{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;_T3FF.f1=_tag_fat("format descriptor is not a string",sizeof(char),34U);_T21A=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T21A;void*_T400[1];_T21B=_T400 + 0;*_T21B=& _T3FF;_T21C=loc;_T21D=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T21C,_T21D);}goto _TL1CC;
# 1074
_TL1CB: _T21E=Cyc_Tcutil_is_cvar_type(b);if(_T21E)goto _TL1CE;else{goto _TL1D0;}
_TL1D0: _T21F=Cyc_Absyn_bounds_one();_T220=b;{struct Cyc_Absyn_Exp*e=Cyc_Tcutil_get_bounds_exp(_T21F,_T220);
if(e!=0)goto _TL1D1;_T221=c_vararg;if(!_T221)goto _TL1D1;{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;
_T3FF.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_T222=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T222;void*_T400[1];_T223=_T400 + 0;*_T223=& _T3FF;_T224=loc;_T225=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T224,_T225);}goto _TL1D2;
_TL1D1: if(e==0)goto _TL1D3;_T226=c_vararg;if(_T226)goto _TL1D3;else{goto _TL1D5;}
_TL1D5:{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;_T3FF.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_T227=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T227;void*_T400[1];_T228=_T400 + 0;*_T228=& _T3FF;_T229=loc;_T22A=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T229,_T22A);}goto _TL1D4;_TL1D3: _TL1D4: _TL1D2:;}goto _TL1CF;
# 1082
_TL1CE: _T22B=c_vararg;if(!_T22B)goto _TL1D6;_T22C=loc;_T22D=
Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T22E=_check_null(_T22D);_T22F=
Cyc_Absyn_thinconst();_T230=
# 1083
Cyc_BansheeIf_equality_constraint(_T22E,_T22F);_T231=Cyc_BansheeIf_add_constraint(_T22C,_T230);if(_T231)goto _TL1D8;else{goto _TL1DA;}
# 1085
_TL1DA:{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;_T3FF.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_T232=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T232;void*_T400[1];_T233=_T400 + 0;*_T233=& _T3FF;_T234=loc;_T235=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T234,_T235);}goto _TL1D9;_TL1D8: _TL1D9: goto _TL1D7;
# 1089
_TL1D6: _T236=loc;_T237=Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T238=_check_null(_T237);_T239=
Cyc_Absyn_fatconst();_T23A=
# 1089
Cyc_BansheeIf_equality_constraint(_T238,_T239);_T23B=Cyc_BansheeIf_add_constraint(_T236,_T23A);if(_T23B)goto _TL1DB;else{goto _TL1DD;}
# 1091
_TL1DD:{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;_T3FF.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_T23C=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T23C;void*_T400[1];_T23D=_T400 + 0;*_T23D=& _T3FF;_T23E=loc;_T23F=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T23E,_T23F);}goto _TL1DC;_TL1DB: _TL1DC: _TL1D7: _TL1CF: _TL1CC: goto _LL64;}_TL1C9:{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;
# 1097
_T3FF.f1=_tag_fat("format descriptor is not a string type",sizeof(char),39U);_T240=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T240;void*_T400[1];_T241=_T400 + 0;*_T241=& _T3FF;_T242=loc;_T243=_tag_fat(_T400,sizeof(void*),1);Cyc_Warn_err2(_T242,_T243);}goto _LL64;_LL64:;}
# 1099
if(fmt_arg_start==0)goto _TL1DE;_T244=c_vararg;if(_T244)goto _TL1DE;else{goto _TL1E0;}
# 1103
_TL1E0:{int problem;{struct _tuple16 _T3FC;
_T3FC.f0=ft;_T246=_check_null(cyc_vararg);_T247=_T246->type;_T248=Cyc_Tcutil_pointer_elt_type(_T247);_T3FC.f1=Cyc_Absyn_compress(_T248);_T245=_T3FC;}{struct _tuple16 _T3FC=_T245;struct Cyc_Absyn_Datatypedecl*_T3FD;_T249=_T3FC.f0;_T24A=(int)_T249;switch(_T24A){case Cyc_Absyn_Printf_ft: _T24B=_T3FC.f1;_T24C=(int*)_T24B;_T24D=*_T24C;if(_T24D!=0)goto _TL1E2;_T24E=_T3FC.f1;_T24F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24E;_T250=_T24F->f1;_T251=(int*)_T250;_T252=*_T251;if(_T252!=22)goto _TL1E4;_T253=_T3FC.f1;_T254=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T253;_T255=_T254->f1;_T256=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T255;_T257=_T256->f1;_T258=_T257.KnownDatatype;_T259=_T258.tag;if(_T259!=2)goto _TL1E6;_T25A=_T3FC.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3FE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25A;_T25B=_T3FE->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T3FF=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T25B;_T25C=_T3FF->f1;_T25D=_T25C.KnownDatatype;_T25E=_T25D.val;{struct Cyc_Absyn_Datatypedecl*_T400=*_T25E;_T3FD=_T400;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T3FD;_T25F=tud;_T260=_T25F->name;_T261=
# 1106
Cyc_Absyn_datatype_print_arg_qvar();_T262=Cyc_Absyn_qvar_cmp(_T260,_T261);problem=_T262!=0;goto _LL69;}_TL1E6: goto _LL6E;_TL1E4: goto _LL6E;_TL1E2: goto _LL6E;case Cyc_Absyn_Scanf_ft: _T263=_T3FC.f1;_T264=(int*)_T263;_T265=*_T264;if(_T265!=0)goto _TL1E8;_T266=_T3FC.f1;_T267=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T266;_T268=_T267->f1;_T269=(int*)_T268;_T26A=*_T269;if(_T26A!=22)goto _TL1EA;_T26B=_T3FC.f1;_T26C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T26B;_T26D=_T26C->f1;_T26E=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T26D;_T26F=_T26E->f1;_T270=_T26F.KnownDatatype;_T271=_T270.tag;if(_T271!=2)goto _TL1EC;_T272=_T3FC.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3FE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T272;_T273=_T3FE->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T3FF=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T273;_T274=_T3FF->f1;_T275=_T274.KnownDatatype;_T276=_T275.val;{struct Cyc_Absyn_Datatypedecl*_T400=*_T276;_T3FD=_T400;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T3FD;_T277=tud;_T278=_T277->name;_T279=
# 1109
Cyc_Absyn_datatype_scanf_arg_qvar();_T27A=Cyc_Absyn_qvar_cmp(_T278,_T279);problem=_T27A!=0;goto _LL69;}_TL1EC: goto _LL6E;_TL1EA: goto _LL6E;_TL1E8: goto _LL6E;default: _LL6E:
# 1112
 problem=1;goto _LL69;}_LL69:;}_T27B=problem;
# 1115
if(!_T27B)goto _TL1EE;{struct Cyc_Warn_String_Warn_Warg_struct _T3FC;_T3FC.tag=0;
_T3FC.f1=_tag_fat("format attribute and vararg types don't match",sizeof(char),46U);_T27C=_T3FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FC=_T27C;void*_T3FD[1];_T27D=_T3FD + 0;*_T27D=& _T3FC;_T27E=loc;_T27F=_tag_fat(_T3FD,sizeof(void*),1);Cyc_Warn_err2(_T27E,_T27F);}goto _TL1EF;_TL1EE: _TL1EF:;}goto _TL1DF;_TL1DE: _TL1DF:;}}_TL1BF:;}goto _TL1BD;_TL1BC: _TL1BD: _T280=qb;{
# 1121
struct Cyc_List_List*qbit=*_T280;_TL1F3: if(qbit!=0)goto _TL1F1;else{goto _TL1F2;}
_TL1F1: _T281=qbit;_T282=_T281->hd;{struct _tuple0*_T3FA=(struct _tuple0*)_T282;void*_T3FB;void*_T3FC;{struct _tuple0 _T3FD=*_T3FA;_T3FC=_T3FD.f0;_T3FB=_T3FD.f1;}{void*tv=_T3FC;void*bnd=_T3FB;_T283=new_env;_T284=& Cyc_Kinds_aqk;_T285=(struct Cyc_Absyn_Kind*)_T284;_T286=bnd;
new_env=Cyc_Tctyp_i_check_type(_T283,_T285,_T286,0,0);_T287=new_env;_T288=& Cyc_Kinds_aqk;_T289=(struct Cyc_Absyn_Kind*)_T288;_T28A=tv;
new_env=Cyc_Tctyp_i_check_type(_T287,_T289,_T28A,0,0);}}_T28B=qbit;
# 1121
qbit=_T28B->tl;goto _TL1F3;_TL1F2:;}{
# 1127
struct Cyc_List_List*ec=effc;_TL1F7: if(ec!=0)goto _TL1F5;else{goto _TL1F6;}
_TL1F5: _T28C=ec;{void*_T3FA=_T28C->hd;void*_T3FB;void*_T3FC;_T28D=(int*)_T3FA;_T28E=*_T28D;switch(_T28E){case 0:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T3FD=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T3FA;_T28F=_T3FD->f1;_T3FC=(void*)_T28F;}{void*t=_T3FC;_T290=new_env;_T291=& Cyc_Kinds_ek;_T292=(struct Cyc_Absyn_Kind*)_T291;_T293=t;
# 1130
new_env=Cyc_Tctyp_i_check_type(_T290,_T292,_T293,1,1);goto _LL73;}case 1:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T3FD=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T3FA;_T294=_T3FD->f1;_T3FC=(void*)_T294;_T295=_T3FD->f2;_T3FB=(void*)_T295;}{void*t1=_T3FC;void*t2=_T3FB;_T3FC=t1;_T3FB=t2;goto _LL79;}default:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T3FD=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T3FA;_T296=_T3FD->f1;_T3FC=(void*)_T296;_T297=_T3FD->f2;_T3FB=(void*)_T297;}_LL79: {void*t1=_T3FC;void*t2=_T3FB;_T298=new_env;_T299=& Cyc_Kinds_ek;_T29A=(struct Cyc_Absyn_Kind*)_T299;_T29B=t1;
# 1134
new_env=Cyc_Tctyp_i_check_type(_T298,_T29A,_T29B,1,1);_T29C=new_env;_T29D=& Cyc_Kinds_ek;_T29E=(struct Cyc_Absyn_Kind*)_T29D;_T29F=t2;
new_env=Cyc_Tctyp_i_check_type(_T29C,_T29E,_T29F,1,1);goto _LL73;}}_LL73:;}_T2A0=ec;
# 1127
ec=_T2A0->tl;goto _TL1F7;_TL1F6:;}{
# 1143
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_bogus_fenv(tr,args);_T2A1=new_env;{
struct Cyc_Tcenv_Tenv*old_te=_T2A1.te;{struct Cyc_Tcenv_Tenv*_T3FA=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T2A3=te;
_T3FA->ns=_T2A3->ns;_T2A4=te;_T3FA->ae=_T2A4->ae;_T3FA->le=fenv;_T3FA->allow_valueof=1;_T2A5=te;
_T3FA->in_extern_c_include=_T2A5->in_extern_c_include;_T2A6=te;
_T3FA->in_tempest=_T2A6->in_tempest;_T2A7=te;_T3FA->tempest_generalize=_T2A7->tempest_generalize;_T2A8=te;
_T3FA->in_extern_c_inc_repeat=_T2A8->in_extern_c_inc_repeat;_T2A2=(struct Cyc_Tcenv_Tenv*)_T3FA;}
# 1145
new_env.te=_T2A2;{
# 1149
struct Cyc_List_List*vds;vds=_cycalloc(sizeof(struct Cyc_List_List));_T2A9=vds;_T2A9->hd=Cyc_AssnDef_memory;_T2AA=vds;_T2AA->tl=arg_vds;
if(chk==0)goto _TL1F9;_T2AB=c_vararg;
if(_T2AB)goto _TL1FD;else{goto _TL1FE;}_TL1FE: _T2AC=cyc_vararg;_T2AD=(unsigned)_T2AC;if(_T2AD)goto _TL1FD;else{goto _TL1FB;}
_TL1FD:{struct Cyc_Warn_String_Warn_Warg_struct _T3FA;_T3FA.tag=0;_T3FA.f1=_tag_fat("we do not yet support @checks on vararg functions",sizeof(char),50U);_T2AE=_T3FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FA=_T2AE;void*_T3FB[1];_T2AF=_T3FB + 0;*_T2AF=& _T3FA;_T2B0=loc;_T2B1=_tag_fat(_T3FB,sizeof(void*),1);Cyc_Warn_err2(_T2B0,_T2B1);}goto _TL1FC;_TL1FB: _TL1FC: _T2B2=new_env;_T2B3=
_tag_fat("@checks",sizeof(char),8U);_T2B4=chk;{struct _tuple15 _T3FA=Cyc_Tctyp_check_clause(_T2B2,_T2B3,_T2B4);struct Cyc_AssnDef_AssnMap _T3FB;struct Cyc_Tctyp_CVTEnv _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3FC;struct Cyc_AssnDef_AssnMap chk_assn=_T3FB;
new_env=nenv;_T2B5=chk_relns;
*_T2B5=Cyc_AssnDef_assnmap2existassnfn(chk_assn,vds);}}goto _TL1FA;_TL1F9: _TL1FA:
# 1157
 if(req==0)goto _TL1FF;_T2B6=new_env;_T2B7=
_tag_fat("@requires",sizeof(char),10U);_T2B8=req;{struct _tuple15 _T3FA=Cyc_Tctyp_check_clause(_T2B6,_T2B7,_T2B8);struct Cyc_AssnDef_AssnMap _T3FB;struct Cyc_Tctyp_CVTEnv _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3FC;struct Cyc_AssnDef_AssnMap req_assn=_T3FB;
new_env=nenv;_T2B9=req_relns;
*_T2B9=Cyc_AssnDef_assnmap2existassnfn(req_assn,vds);}}goto _TL200;_TL1FF: _TL200: {
# 1163
struct Cyc_List_List*vds;vds=_cycalloc(sizeof(struct Cyc_List_List));_T2BA=vds;_T2BA->hd=Cyc_AssnDef_memory;_T2BB=vds;{struct Cyc_List_List*_T3FA=_cycalloc(sizeof(struct Cyc_List_List));_T3FA->hd=Cyc_AssnDef_pre_memory;_T3FA->tl=arg_vds;_T2BC=(struct Cyc_List_List*)_T3FA;}_T2BB->tl=_T2BC;
if(ens==0)goto _TL201;_T2BD=new_env;_T2BE=
_tag_fat("@ensures",sizeof(char),9U);_T2BF=ens;{struct _tuple15 _T3FA=Cyc_Tctyp_check_clause(_T2BD,_T2BE,_T2BF);struct Cyc_AssnDef_AssnMap _T3FB;struct Cyc_Tctyp_CVTEnv _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3FC;struct Cyc_AssnDef_AssnMap ens_assn=_T3FB;
new_env=nenv;_T2C0=ens_relns;_T2C1=ens_assn;{struct Cyc_List_List*_T3FD=_cycalloc(sizeof(struct Cyc_List_List));
_T3FD->hd=return_vd;_T3FD->tl=vds;_T2C2=(struct Cyc_List_List*)_T3FD;}*_T2C0=Cyc_AssnDef_assnmap2existassnfn(_T2C1,_T2C2);}}goto _TL202;_TL201: _TL202:
# 1169
 if(thrws==0)goto _TL203;_T2C3=new_env;_T2C4=
_tag_fat("@throws",sizeof(char),8U);_T2C5=thrws;{struct _tuple15 _T3FA=Cyc_Tctyp_check_clause(_T2C3,_T2C4,_T2C5);struct Cyc_AssnDef_AssnMap _T3FB;struct Cyc_Tctyp_CVTEnv _T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3FC;struct Cyc_AssnDef_AssnMap thrws_assn=_T3FB;
new_env=nenv;_T2C6=thrws_relns;
*_T2C6=Cyc_AssnDef_assnmap2existassnfn(thrws_assn,vds);}}goto _TL204;_TL203: _TL204:
# 1174
 new_env.te=old_te;_T2C7=eff;_T2C8=*_T2C7;
# 1176
if(_T2C8==0)goto _TL205;_T2C9=new_env;_T2CA=& Cyc_Kinds_ek;_T2CB=(struct Cyc_Absyn_Kind*)_T2CA;_T2CC=eff;_T2CD=*_T2CC;
new_env=Cyc_Tctyp_i_check_type(_T2C9,_T2CB,_T2CD,1,1);_T2CE=Cyc_Absyn_heap_rgn_type;_T2CF=eff;_T2D0=*_T2CF;_T2D1=
_check_null(_T2D0);_T2D2=Cyc_Tcutil_type_in_effect(0,_T2CE,_T2D1);if(_T2D2)goto _TL207;else{goto _TL209;}
_TL209: _T2D3=eff;{void*_T3FA[2];_T2D5=_T3FA + 0;*_T2D5=Cyc_Absyn_heap_rgn_type;_T2D6=_T3FA + 1;_T2D7=eff;_T2D8=*_T2D7;*_T2D6=_check_null(_T2D8);_T2D9=_tag_fat(_T3FA,sizeof(void*),2);_T2D4=Cyc_List_list(_T2D9);}_T2DA=Cyc_Absyn_join_eff(_T2D4);*_T2D3=Cyc_Tcutil_normalize_effect(_T2DA);goto _TL208;_TL207: _TL208: goto _TL206;
# 1183
_TL205:{struct Cyc_List_List*effect;effect=_cycalloc(sizeof(struct Cyc_List_List));_T2DB=effect;_T2DB->hd=Cyc_Absyn_heap_rgn_type;_T2DC=effect;_T2DC->tl=0;_T2DD=new_env;{
# 1188
struct Cyc_List_List*tvs=_T2DD.free_vars;_TL20D: if(tvs!=0)goto _TL20B;else{goto _TL20C;}
_TL20B: _T2DE=tvs;_T2DF=_T2DE->hd;_T2E0=(struct _tuple12*)_T2DF;{struct _tuple12 _T3FA=*_T2E0;int _T3FB;struct Cyc_Absyn_Tvar*_T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Absyn_Tvar*tv=_T3FC;int put_in_eff=_T3FB;_T2E1=put_in_eff;
if(_T2E1)goto _TL20E;else{goto _TL210;}_TL210: goto _TL20A;_TL20E: _T2E2=tv;_T2E3=_T2E2->kind;{
void*_T3FD=Cyc_Kinds_compress_kb(_T2E3);void*_T3FE;_T2E4=(int*)_T3FD;_T2E5=*_T2E4;switch(_T2E5){case 2: _T2E6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FD;_T2E7=_T2E6->f2;_T2E8=(struct Cyc_Absyn_Kind*)_T2E7;_T2E9=_T2E8->kind;_T2EA=(int)_T2E9;switch(_T2EA){case Cyc_Absyn_BoolKind: goto _LL8D;case Cyc_Absyn_PtrBndKind: _LL8D: goto _LL8F;case Cyc_Absyn_IntKind: _LL8F: goto _LL91;case Cyc_Absyn_AqualKind: _LL91: goto _LL93;case Cyc_Absyn_EffKind:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FD;_T2EB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FD;_T2EC=& _T2EB->f1;_T3FE=(struct Cyc_Core_Opt**)_T2EC;}{struct Cyc_Core_Opt**f=_T3FE;_T2ED=f;_T2EE=& Cyc_Kinds_ek;_T2EF=(struct Cyc_Absyn_Kind*)_T2EE;
# 1201
*_T2ED=Cyc_Kinds_kind_to_bound_opt(_T2EF);goto _LL9D;}default: goto _LLA0;};case 0: _T2F0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3FD;_T2F1=_T2F0->f1;_T2F2=(struct Cyc_Absyn_Kind*)_T2F1;_T2F3=_T2F2->kind;_T2F4=(int)_T2F3;switch(_T2F4){case Cyc_Absyn_BoolKind: _LL93: goto _LL95;case Cyc_Absyn_PtrBndKind: _LL95: goto _LL97;case Cyc_Absyn_IntKind: _LL97: goto _LL99;case Cyc_Absyn_AqualKind: _LL99: goto _LL89;case Cyc_Absyn_EffKind: _LL9D:{struct Cyc_List_List*_T3FF=_cycalloc(sizeof(struct Cyc_List_List));
# 1203
_T3FF->hd=Cyc_Absyn_var_type(tv);_T3FF->tl=effect;_T2F5=(struct Cyc_List_List*)_T3FF;}effect=_T2F5;goto _LL89;default: _LLA0: _LLA1:{struct Cyc_List_List*_T3FF=_cycalloc(sizeof(struct Cyc_List_List));_T2F7=
# 1208
Cyc_Absyn_var_type(tv);_T3FF->hd=Cyc_Absyn_regionsof_eff(_T2F7);_T3FF->tl=effect;_T2F6=(struct Cyc_List_List*)_T3FF;}effect=_T2F6;goto _LL89;};default:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3FF=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FD;_T2F8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FD;_T2F9=& _T2F8->f1;_T3FE=(struct Cyc_Core_Opt**)_T2F9;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3FE;_T2FA=f;{struct Cyc_Core_Opt*_T3FF=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T400=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T400->tag=2;
# 1205
_T400->f1=0;_T2FD=& Cyc_Kinds_ak;_T400->f2=(struct Cyc_Absyn_Kind*)_T2FD;_T2FC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T400;}_T3FF->v=(void*)_T2FC;_T2FB=(struct Cyc_Core_Opt*)_T3FF;}*_T2FA=_T2FB;goto _LLA1;}}_LL89:;}}}_TL20A: _T2FE=tvs;
# 1188
tvs=_T2FE->tl;goto _TL20D;_TL20C:;}_T2FF=new_env;{
# 1212
struct Cyc_List_List*ts=_T2FF.free_evars;_TL217: if(ts!=0)goto _TL215;else{goto _TL216;}
_TL215: _T300=ts;_T301=_T300->hd;_T302=(struct _tuple14*)_T301;{struct _tuple14 _T3FA=*_T302;int _T3FB;void*_T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{void*tv=_T3FC;int put_in_eff=_T3FB;_T303=put_in_eff;
if(_T303)goto _TL218;else{goto _TL21A;}_TL21A: goto _TL214;_TL218: {
struct Cyc_Absyn_Kind*_T3FD=Cyc_Tcutil_type_kind(tv);_T304=(struct Cyc_Absyn_Kind*)_T3FD;_T305=_T304->kind;_T306=(int)_T305;switch(_T306){case Cyc_Absyn_IntKind: goto _LLA9;case Cyc_Absyn_BoolKind: _LLA9: goto _LLAB;case Cyc_Absyn_PtrBndKind: _LLAB: goto _LLAD;case Cyc_Absyn_AqualKind: _LLAD: goto _LLA5;case Cyc_Absyn_EffKind:{struct Cyc_List_List*_T3FE=_cycalloc(sizeof(struct Cyc_List_List));
# 1222
_T3FE->hd=tv;_T3FE->tl=effect;_T307=(struct Cyc_List_List*)_T3FE;}effect=_T307;goto _LLA5;default:{struct Cyc_List_List*_T3FE=_cycalloc(sizeof(struct Cyc_List_List));
_T3FE->hd=Cyc_Absyn_regionsof_eff(tv);_T3FE->tl=effect;_T308=(struct Cyc_List_List*)_T3FE;}effect=_T308;goto _LLA5;}_LLA5:;}}}_TL214: _T309=ts;
# 1212
ts=_T309->tl;goto _TL217;_TL216:;}_T30A=eff;
# 1226
*_T30A=Cyc_Absyn_join_eff(effect);}_TL206: _T30B=btvs;_T30C=*_T30B;
# 1232
if(_T30C==0)goto _TL21C;_T30D=btvs;{
struct Cyc_List_List*bs=*_T30D;_TL221: if(bs!=0)goto _TL21F;else{goto _TL220;}
_TL21F: _T30E=bs;_T30F=_T30E->hd;_T310=(struct Cyc_Absyn_Tvar*)_T30F;_T311=_T310->kind;{void*_T3FA=Cyc_Kinds_compress_kb(_T311);struct Cyc_Absyn_Kind*_T3FB;void*_T3FC;_T312=(int*)_T3FA;_T313=*_T312;switch(_T313){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3FD=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FA;_T314=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3FA;_T315=& _T314->f1;_T3FC=(struct Cyc_Core_Opt**)_T315;}{struct Cyc_Core_Opt**f=_T3FC;{struct Cyc_Warn_String_Warn_Warg_struct _T3FD;_T3FD.tag=0;
# 1236
_T3FD.f1=_tag_fat("type variable ",sizeof(char),15U);_T316=_T3FD;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FD=_T316;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3FE;_T3FE.tag=7;_T318=bs;_T319=_T318->hd;_T3FE.f1=(struct Cyc_Absyn_Tvar*)_T319;_T317=_T3FE;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3FE=_T317;{struct Cyc_Warn_String_Warn_Warg_struct _T3FF;_T3FF.tag=0;_T3FF.f1=_tag_fat(" unconstrained, assuming boxed",sizeof(char),31U);_T31A=_T3FF;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FF=_T31A;void*_T400[3];_T31B=_T400 + 0;*_T31B=& _T3FD;_T31C=_T400 + 1;*_T31C=& _T3FE;_T31D=_T400 + 2;*_T31D=& _T3FF;_T31E=loc;_T31F=_tag_fat(_T400,sizeof(void*),3);Cyc_Warn_warn2(_T31E,_T31F);}}}_T320=f;_T321=& Cyc_Kinds_bk;_T322=(struct Cyc_Absyn_Kind*)_T321;
*_T320=Cyc_Kinds_kind_to_bound_opt(_T322);goto _LLB2;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3FD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FA;_T323=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3FA;_T324=& _T323->f1;_T3FC=(struct Cyc_Core_Opt**)_T324;_T3FB=_T3FD->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3FC;struct Cyc_Absyn_Kind*k=_T3FB;_T325=k;_T326=(struct Cyc_Absyn_Kind*)_T325;_T327=_T326->kind;_T328=(int)_T327;switch(_T328){case Cyc_Absyn_BoxKind: goto _LLBF;case Cyc_Absyn_MemKind: _LLBF: goto _LLC1;case Cyc_Absyn_AnyKind: _LLC1: _T329=f;_T32A=& Cyc_Kinds_bk;_T32B=(struct Cyc_Absyn_Kind*)_T32A;
# 1244
*_T329=Cyc_Kinds_kind_to_bound_opt(_T32B);goto _LLBB;case Cyc_Absyn_EffKind: _T32C=f;_T32D=& Cyc_Kinds_ek;_T32E=(struct Cyc_Absyn_Kind*)_T32D;
# 1246
*_T32C=Cyc_Kinds_kind_to_bound_opt(_T32E);goto _LLBB;default: _T32F=f;
# 1248
*_T32F=Cyc_Kinds_kind_to_bound_opt(k);goto _LLBB;}_LLBB: goto _LLB2;}default: _T330=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3FA;_T331=_T330->f1;_T332=(struct Cyc_Absyn_Kind*)_T331;_T333=_T332->kind;if(_T333!=Cyc_Absyn_MemKind)goto _TL224;{struct Cyc_Warn_String_Warn_Warg_struct _T3FD;_T3FD.tag=0;
# 1252
_T3FD.f1=_tag_fat("functions can't abstract M types",sizeof(char),33U);_T334=_T3FD;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FD=_T334;void*_T3FE[1];_T335=_T3FE + 0;*_T335=& _T3FD;_T336=loc;_T337=_tag_fat(_T3FE,sizeof(void*),1);Cyc_Warn_err2(_T336,_T337);}goto _LLB2;_TL224: goto _LLB2;}_LLB2:;}_T338=bs;
# 1233
bs=_T338->tl;goto _TL221;_TL220:;}goto _TL21D;_TL21C: _TL21D:{
# 1256
struct Cyc_List_List*(*_T3FA)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars;_T339=_T3FA;}_T33A=new_env;_T33B=_T33A.kind_env;_T33C=btvs;_T33D=*_T33C;_T33F=Cyc_Core_identity;{struct Cyc_Absyn_Tvar*(*_T3FA)(struct Cyc_Absyn_Tvar*)=(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))_T33F;_T33E=_T3FA;}env.kind_env=_T339(_T33B,_T33D,_T33E);{
struct Cyc_List_List*(*_T3FA)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T340=_T3FA;}_T341=new_env;_T342=_T341.free_vars;_T343=btvs;_T344=*_T343;_T346=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T3FA)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T346;_T345=_T3FA;}new_env.free_vars=_T340(_T342,_T344,_T345);_T347=new_env;{
# 1259
struct Cyc_List_List*tvs=_T347.free_vars;_TL229: if(tvs!=0)goto _TL227;else{goto _TL228;}
_TL227: _T348=tvs;_T349=_T348->hd;_T34A=(struct _tuple12*)_T349;{struct _tuple12 _T3FA=*_T34A;int _T3FB;struct Cyc_Absyn_Tvar*_T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{struct Cyc_Absyn_Tvar*t=_T3FC;int b=_T3FB;_T34B=env;_T34C=_T34B.free_vars;_T34D=t;_T34E=b;
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T34C,_T34D,_T34E);}}_T34F=tvs;
# 1259
tvs=_T34F->tl;goto _TL229;_TL228:;}_T350=new_env;{
# 1264
struct Cyc_List_List*evs=_T350.free_evars;_TL22D: if(evs!=0)goto _TL22B;else{goto _TL22C;}
_TL22B: _T351=evs;_T352=_T351->hd;_T353=(struct _tuple14*)_T352;{struct _tuple14 _T3FA=*_T353;int _T3FB;void*_T3FC;_T3FC=_T3FA.f0;_T3FB=_T3FA.f1;{void*e=_T3FC;int b=_T3FB;_T354=env;_T355=_T354.free_evars;_T356=e;_T357=b;
env.free_evars=Cyc_Tctyp_add_free_evar(_T355,_T356,_T357);}}_T358=evs;
# 1264
evs=_T358->tl;goto _TL22D;_TL22C:;}_T359=env;_T35A=_T359.free_qualvars;_T35B=(unsigned)_T35A;
# 1268
if(!_T35B)goto _TL22E;_T35C=new_env;{
# 1270
struct Cyc_List_List*it=_T35C.free_qualvars;_TL233: if(it!=0)goto _TL231;else{goto _TL232;}
_TL231: _T35E=Cyc_List_exists_c;{int(*_T3FA)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*)=(int(*)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*))_T35E;_T35D=_T3FA;}_T35F=it;_T360=_T35F->hd;_T361=(struct _tuple14*)_T360;_T362=env;_T363=_T362.free_qualvars;_T364=_T35D(Cyc_Tctyp_cmp_fqv,_T361,_T363);if(_T364)goto _TL234;else{goto _TL236;}
_TL236:{struct Cyc_List_List*_T3FA=_cycalloc(sizeof(struct Cyc_List_List));_T366=it;_T367=_T366->hd;_T3FA->hd=(struct _tuple14*)_T367;_T368=env;_T3FA->tl=_T368.free_qualvars;_T365=(struct Cyc_List_List*)_T3FA;}env.free_qualvars=_T365;goto _TL235;_TL234: _TL235: _T369=it;
# 1270
it=_T369->tl;goto _TL233;_TL232:;}goto _TL22F;
# 1276
_TL22E: _T36A=new_env;env.free_qualvars=_T36A.free_qualvars;_TL22F: goto _LL3;}}}}}}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T3E1;_T3E3=_T3FA->f1;_T3EE=_T3FA->f2;_T3F9=_T3FA->f3;}{enum Cyc_Absyn_AggrKind k=_T3E3;int is_tuple=_T3EE;struct Cyc_List_List*fs=_T3F9;
# 1281
struct Cyc_List_List*prev_fields=0;
_TL23A: if(fs!=0)goto _TL238;else{goto _TL239;}
_TL238: _T36B=fs;_T36C=_T36B->hd;{struct Cyc_Absyn_Aggrfield*_T3FA=(struct Cyc_Absyn_Aggrfield*)_T36C;struct Cyc_Absyn_Exp*_T3FB;struct Cyc_List_List*_T3FC;struct Cyc_Absyn_Exp*_T3FD;void*_T3FE;void*_T3FF;struct _fat_ptr*_T400;{struct Cyc_Absyn_Aggrfield _T401=*_T3FA;_T400=_T401.name;_T36D=& _T3FA->tq;_T3FF=(struct Cyc_Absyn_Tqual*)_T36D;_T3FE=_T401.type;_T3FD=_T401.width;_T3FC=_T401.attributes;_T3FB=_T401.requires_clause;}{struct _fat_ptr*fn=_T400;struct Cyc_Absyn_Tqual*tqp=(struct Cyc_Absyn_Tqual*)_T3FF;void*t=_T3FE;struct Cyc_Absyn_Exp*width=_T3FD;struct Cyc_List_List*atts=_T3FC;struct Cyc_Absyn_Exp*requires_clause=_T3FB;_T36F=Cyc_List_mem;{
int(*_T401)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T36F;_T36E=_T401;}_T370=Cyc_strptrcmp;_T371=prev_fields;_T372=fn;_T373=_T36E(_T370,_T371,_T372);if(!_T373)goto _TL23B;{struct Cyc_Warn_String_Warn_Warg_struct _T401;_T401.tag=0;
_T401.f1=_tag_fat("duplicate field ",sizeof(char),17U);_T374=_T401;}{struct Cyc_Warn_String_Warn_Warg_struct _T401=_T374;{struct Cyc_Warn_String_Warn_Warg_struct _T402;_T402.tag=0;_T376=fn;_T402.f1=*_T376;_T375=_T402;}{struct Cyc_Warn_String_Warn_Warg_struct _T402=_T375;void*_T403[2];_T377=_T403 + 0;*_T377=& _T401;_T378=_T403 + 1;*_T378=& _T402;_T379=loc;_T37A=_tag_fat(_T403,sizeof(void*),2);Cyc_Warn_err2(_T379,_T37A);}}goto _TL23C;_TL23B: _TL23C: _T37B=fn;_T37C=*_T37B;_T37D=
_tag_fat("",sizeof(char),1U);_T37E=Cyc_strcmp(_T37C,_T37D);if(_T37E==0)goto _TL23D;{struct Cyc_List_List*_T401=_cycalloc(sizeof(struct Cyc_List_List));
_T401->hd=fn;_T401->tl=prev_fields;_T37F=(struct Cyc_List_List*)_T401;}prev_fields=_T37F;goto _TL23E;_TL23D: _TL23E: _T380=env;_T381=& Cyc_Kinds_mk;_T382=(struct Cyc_Absyn_Kind*)_T381;_T383=t;
env=Cyc_Tctyp_i_check_type(_T380,_T382,_T383,1,0);_T384=tqp;_T385=loc;_T386=tqp;_T387=_T386->print_const;_T388=t;
_T384->real_const=Cyc_Tcutil_extract_const_from_typedef(_T385,_T387,_T388);
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause==0)goto _TL23F;_T389=k;_T38A=(int)_T389;
# 1294
if(_T38A==1)goto _TL241;{struct Cyc_Warn_String_Warn_Warg_struct _T401;_T401.tag=0;
_T401.f1=_tag_fat("@requires clause is only allowed on union members",sizeof(char),50U);_T38B=_T401;}{struct Cyc_Warn_String_Warn_Warg_struct _T401=_T38B;void*_T402[1];_T38C=_T402 + 0;*_T38C=& _T401;_T38D=loc;_T38E=_tag_fat(_T402,sizeof(void*),1);Cyc_Warn_err2(_T38D,_T38E);}goto _TL242;_TL241: _TL242: _T38F=
Cyc_Tcenv_allow_valueof(te);_T390=requires_clause;Cyc_Tcexp_tcExp(_T38F,0,_T390);_T391=
Cyc_Tcutil_is_integral(requires_clause);if(_T391)goto _TL243;else{goto _TL245;}
_TL245:{struct Cyc_Warn_String_Warn_Warg_struct _T401;_T401.tag=0;_T401.f1=_tag_fat("@requires clause has type ",sizeof(char),27U);_T392=_T401;}{struct Cyc_Warn_String_Warn_Warg_struct _T401=_T392;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T402;_T402.tag=3;_T394=requires_clause;_T402.f1=_T394->topt;_T393=_T402;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T402=_T393;{struct Cyc_Warn_String_Warn_Warg_struct _T403;_T403.tag=0;
_T403.f1=_tag_fat(" instead of integral type",sizeof(char),26U);_T395=_T403;}{struct Cyc_Warn_String_Warn_Warg_struct _T403=_T395;void*_T404[3];_T396=_T404 + 0;*_T396=& _T401;_T397=_T404 + 1;*_T397=& _T402;_T398=_T404 + 2;*_T398=& _T403;_T399=loc;_T39A=_tag_fat(_T404,sizeof(void*),3);Cyc_Warn_err2(_T399,_T39A);}}}goto _TL244;_TL243: _TL244:
 env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);goto _TL240;_TL23F: _TL240:;}}_T39B=fs;
# 1282
fs=_T39B->tl;goto _TL23A;_TL239: goto _LL3;}default:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T3FA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3E1;_T3E2=_T3FA->f1;_T39C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3E1;_T39D=& _T39C->f2;_T3F9=(struct Cyc_List_List**)_T39D;_T39E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3E1;_T39F=& _T39E->f3;_T3F8=(struct Cyc_Absyn_Typedefdecl**)_T39F;_T3A0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3E1;_T3A1=& _T3A0->f4;_T3F6=(void**)_T3A1;}{struct _tuple1*tdn=_T3E2;struct Cyc_List_List**targs_ref=(struct Cyc_List_List**)_T3F9;struct Cyc_Absyn_Typedefdecl**tdopt=(struct Cyc_Absyn_Typedefdecl**)_T3F8;void**toptp=(void**)_T3F6;
# 1307
struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _T3FA;_T3A2=& _T3FA;_push_handler(_T3A2);{int _T3FB=0;_T3A3=_T3FA.handler;_T3A4=setjmp(_T3A3);if(!_T3A4)goto _TL246;_T3FB=1;goto _TL247;_TL246: _TL247: if(_T3FB)goto _TL248;else{goto _TL24A;}_TL24A:
 td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);_pop_handler();goto _TL249;_TL248: _T3A5=Cyc_Core_get_exn_thrown();{void*_T3FC=(void*)_T3A5;void*_T3FD;_T3A6=(struct Cyc_Dict_Absent_exn_struct*)_T3FC;_T3A7=_T3A6->tag;_T3A8=Cyc_Dict_Absent;if(_T3A7!=_T3A8)goto _TL24B;{struct Cyc_Warn_String_Warn_Warg_struct _T3FE;_T3FE.tag=0;
# 1311
_T3FE.f1=_tag_fat("unbound typedef name ",sizeof(char),22U);_T3A9=_T3FE;}{struct Cyc_Warn_String_Warn_Warg_struct _T3FE=_T3A9;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3FF;_T3FF.tag=1;_T3FF.f1=tdn;_T3AA=_T3FF;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3FF=_T3AA;void*_T400[2];_T3AB=_T400 + 0;*_T3AB=& _T3FE;_T3AC=_T400 + 1;*_T3AC=& _T3FF;_T3AD=loc;_T3AE=_tag_fat(_T400,sizeof(void*),2);Cyc_Warn_err2(_T3AD,_T3AE);}}_T3AF=env;
return _T3AF;_TL24B: _T3FD=_T3FC;{void*exn=_T3FD;_rethrow(exn);};}_TL249:;}}_T3B0=tdopt;
# 1314
*_T3B0=td;_T3B1=env;_T3B2=targs_ref;_T3B3=td;_T3B4=_T3B3->tvs;_T3B5=expected_kind;_T3B6=allow_abs_aggr;_T3B7=tdn;_T3B8=td;
env=Cyc_Tctyp_check_type_inst(_T3B1,_T3B2,_T3B4,_T3B5,_T3B6,_T3B7,_T3B8);_T3B9=td;_T3BA=_T3B9->defn;
# 1317
if(_T3BA==0)goto _TL24D;_T3BB=loc;_T3BC=td;_T3BD=_T3BC->defn;_T3BE=expected_kind;
Cyc_Tctyp_check_constrain_ptrbnd(_T3BB,_T3BD,_T3BE);{
struct Cyc_List_List*inst=0;_T3BF=td;{
struct Cyc_List_List*tvs=_T3BF->tvs;_T3C0=targs_ref;{
struct Cyc_List_List*ts=*_T3C0;
_TL252: if(tvs!=0)goto _TL250;else{goto _TL251;}
_TL250:{struct Cyc_List_List*_T3FA=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T3FB=_cycalloc(sizeof(struct _tuple17));_T3C3=tvs;_T3C4=_T3C3->hd;_T3FB->f0=(struct Cyc_Absyn_Tvar*)_T3C4;_T3C5=_check_null(ts);_T3FB->f1=_T3C5->hd;_T3C2=(struct _tuple17*)_T3FB;}_T3FA->hd=_T3C2;_T3FA->tl=inst;_T3C1=(struct Cyc_List_List*)_T3FA;}inst=_T3C1;_T3C6=tvs;
# 1322
tvs=_T3C6->tl;_T3C7=ts;ts=_T3C7->tl;goto _TL252;_TL251: _T3C8=toptp;_T3C9=inst;_T3CA=td;_T3CB=_T3CA->defn;_T3CC=
# 1324
_check_null(_T3CB);*_T3C8=Cyc_Tcutil_substitute(_T3C9,_T3CC);}}}goto _TL24E;_TL24D: _TL24E: goto _LL3;}}_LL3:;}_T3CD=
# 1328
Cyc_Tcutil_type_kind(t);_T3CE=expected_kind;_T3CF=Cyc_Kinds_kind_leq(_T3CD,_T3CE);if(_T3CF)goto _TL253;else{goto _TL255;}
_TL255:{struct Cyc_Warn_String_Warn_Warg_struct _T3E1;_T3E1.tag=0;_T3E1.f1=_tag_fat("type ",sizeof(char),6U);_T3D0=_T3E1;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E1=_T3D0;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3E2;_T3E2.tag=2;_T3E2.f1=t;_T3D1=_T3E2;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3E2=_T3D1;{struct Cyc_Warn_String_Warn_Warg_struct _T3E3;_T3E3.tag=0;_T3E3.f1=_tag_fat(" has kind ",sizeof(char),11U);_T3D2=_T3E3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E3=_T3D2;{struct Cyc_Warn_Kind_Warn_Warg_struct _T3E4;_T3E4.tag=9;_T3E4.f1=Cyc_Tcutil_type_kind(t);_T3D3=_T3E4;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T3E4=_T3D3;{struct Cyc_Warn_String_Warn_Warg_struct _T3E5;_T3E5.tag=0;
_T3E5.f1=_tag_fat(" but as used here needs kind ",sizeof(char),30U);_T3D4=_T3E5;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E5=_T3D4;{struct Cyc_Warn_Kind_Warn_Warg_struct _T3E6;_T3E6.tag=9;_T3E6.f1=expected_kind;_T3D5=_T3E6;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T3E6=_T3D5;void*_T3E7[6];_T3D6=_T3E7 + 0;*_T3D6=& _T3E1;_T3D7=_T3E7 + 1;*_T3D7=& _T3E2;_T3D8=_T3E7 + 2;*_T3D8=& _T3E3;_T3D9=_T3E7 + 3;*_T3D9=& _T3E4;_T3DA=_T3E7 + 4;*_T3DA=& _T3E5;_T3DB=_T3E7 + 5;*_T3DB=& _T3E6;_T3DC=loc;_T3DD=_tag_fat(_T3E7,sizeof(void*),6);Cyc_Warn_err2(_T3DC,_T3DD);}}}}}}goto _TL254;_TL253: _TL254: _T3DE=env;
# 1332
return _T3DE;}}
# 1335
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_valid_type(struct Cyc_Tctyp_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple12*_T5;struct _tuple12 _T6;struct Cyc_Absyn_Tvar*_T7;struct Cyc_List_List*_T8;struct Cyc_Tctyp_CVTEnv _T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple14*_TC;int*_TD;int _TE;struct Cyc_Absyn_Evar_Absyn_Type_struct*_TF;struct Cyc_Core_Opt**_T10;struct Cyc_Core_Opt**_T11;struct Cyc_Core_Opt*_T12;struct Cyc_Core_Opt**_T13;struct Cyc_Core_Opt*_T14;struct Cyc_Core_Opt**_T15;struct Cyc_Core_Opt*_T16;void*_T17;int(*_T18)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);int(*_T19)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T1A)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Tvar*_T1E;int _T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_List_List*_T23;struct Cyc_Core_Opt**_T24;struct Cyc_Core_Opt*_T25;struct Cyc_List_List*_T26;struct Cyc_Tctyp_CVTEnv _T27;_T0=cvt;{
# 1337
struct Cyc_List_List*kind_env=_T0.kind_env;
cvt=Cyc_Tctyp_i_check_type(cvt,expected_kind,t,1,allow_abs_aggr);_T1=cvt;{
# 1340
struct Cyc_List_List*vs=_T1.free_vars;_TL259: if(vs!=0)goto _TL257;else{goto _TL258;}
_TL257: _T2=kind_env;_T3=vs;_T4=_T3->hd;_T5=(struct _tuple12*)_T4;_T6=*_T5;_T7=_T6.f0;cvt.kind_env=Cyc_Tctyp_fast_add_free_tvar(_T2,_T7);_T8=vs;
# 1340
vs=_T8->tl;goto _TL259;_TL258:;}_T9=cvt;{
# 1346
struct Cyc_List_List*evs=_T9.free_evars;_TL25D: if(evs!=0)goto _TL25B;else{goto _TL25C;}
_TL25B: _TA=evs;_TB=_TA->hd;_TC=(struct _tuple14*)_TB;{struct _tuple14 _T28=*_TC;void*_T29;_T29=_T28.f0;{void*e=_T29;
void*_T2A=Cyc_Absyn_compress(e);void*_T2B;_TD=(int*)_T2A;_TE=*_TD;if(_TE!=1)goto _TL25E;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T2A;_TF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T2A;_T10=& _TF->f4;_T2B=(struct Cyc_Core_Opt**)_T10;}{struct Cyc_Core_Opt**s=(struct Cyc_Core_Opt**)_T2B;_T11=s;_T12=*_T11;
# 1350
if(_T12!=0)goto _TL260;_T13=s;{struct Cyc_Core_Opt*_T2C=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T2C->v=kind_env;_T14=(struct Cyc_Core_Opt*)_T2C;}*_T13=_T14;goto _TL261;
# 1354
_TL260:{struct Cyc_List_List*result=0;_T15=s;_T16=*_T15;_T17=_T16->v;{
struct Cyc_List_List*tvs=(struct Cyc_List_List*)_T17;_TL265: if(tvs!=0)goto _TL263;else{goto _TL264;}
_TL263: _T19=Cyc_List_mem;{int(*_T2C)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))_T19;_T18=_T2C;}_T1A=Cyc_Tcutil_fast_tvar_cmp;_T1B=kind_env;_T1C=tvs;_T1D=_T1C->hd;_T1E=(struct Cyc_Absyn_Tvar*)_T1D;_T1F=_T18(_T1A,_T1B,_T1E);if(!_T1F)goto _TL266;{struct Cyc_List_List*_T2C=_cycalloc(sizeof(struct Cyc_List_List));_T21=tvs;_T22=_T21->hd;
_T2C->hd=(struct Cyc_Absyn_Tvar*)_T22;_T2C->tl=result;_T20=(struct Cyc_List_List*)_T2C;}result=_T20;goto _TL267;_TL266: _TL267: _T23=tvs;
# 1355
tvs=_T23->tl;goto _TL265;_TL264:;}_T24=s;{struct Cyc_Core_Opt*_T2C=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 1358
_T2C->v=result;_T25=(struct Cyc_Core_Opt*)_T2C;}*_T24=_T25;}_TL261: goto _LL3;}_TL25E: goto _LL3;_LL3:;}}_T26=evs;
# 1346
evs=_T26->tl;goto _TL25D;_TL25C:;}_T27=cvt;
# 1364
return _T27;}}
# 1371
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){int _T0;int _T1;int _T2;struct Cyc_Tcenv_Tenv*_T3;int _T4;struct Cyc_Tcenv_Tenv*_T5;struct Cyc_Absyn_Kind*_T6;int _T7;struct Cyc_Absyn_Kind*_T8;struct Cyc_Absyn_Kind*_T9;struct Cyc_Tctyp_CVTEnv _TA;struct Cyc_Absyn_Kind*_TB;void*_TC;struct Cyc_List_List*(*_TD)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_TE)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TF)(struct _tuple12*);void*(*_T10)(struct _tuple0*);struct Cyc_Tctyp_CVTEnv _T11;struct Cyc_List_List*_T12;struct Cyc_Tctyp_CVTEnv _T13;struct Cyc_Tctyp_CVTEnv _T14;struct Cyc_List_List*_T15;unsigned _T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Tvar*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_Absyn_Tvar*_T1C;int _T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Absyn_Tvar*_T25;void*_T26;int*_T27;unsigned _T28;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T29;struct Cyc_Core_Opt**_T2A;struct Cyc_Core_Opt**_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2E;struct Cyc_Core_Opt**_T2F;struct Cyc_Absyn_Kind*_T30;struct Cyc_Absyn_Kind*_T31;enum Cyc_Absyn_KindQual _T32;int _T33;struct Cyc_Core_Opt**_T34;struct Cyc_Absyn_Kind*_T35;struct Cyc_Absyn_Kind*_T36;struct Cyc_Core_Opt**_T37;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T38;struct Cyc_Absyn_Kind*_T39;struct Cyc_Absyn_Kind*_T3A;enum Cyc_Absyn_KindQual _T3B;struct Cyc_Absyn_Kind*_T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_Warn_String_Warn_Warg_struct _T41;struct Cyc_Warn_Kind_Warn_Warg_struct _T42;struct Cyc_Absyn_Kind*_T43;void**_T44;void**_T45;void**_T46;void**_T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;int*_T4B;int _T4C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4D;struct Cyc_List_List**_T4E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4F;struct Cyc_List_List**_T50;unsigned _T51;struct Cyc_Tctyp_CVTEnv _T52;struct Cyc_List_List*(*_T53)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T54)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T55)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List**_T56;struct Cyc_List_List*_T57;struct Cyc_List_List*_T58;struct Cyc_List_List**_T59;void*_T5A;struct Cyc_List_List**_T5B;struct Cyc_List_List*_T5C;struct Cyc_List_List**_T5D;struct Cyc_List_List*(*_T5E)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);struct Cyc_List_List*(*_T5F)(int(*)(void*),struct Cyc_List_List*);int(*_T60)(struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T61;struct Cyc_Warn_String_Warn_Warg_struct _T62;struct Cyc_Warn_Tvar_Warn_Warg_struct _T63;struct Cyc_List_List*_T64;void*_T65;struct Cyc_Warn_String_Warn_Warg_struct _T66;struct Cyc_Warn_Typ_Warn_Warg_struct _T67;void**_T68;void**_T69;void**_T6A;void**_T6B;unsigned _T6C;struct _fat_ptr _T6D;int _T6E;struct Cyc_Tcenv_Tenv*_T6F;int _T70;int*_T71;int _T72;struct Cyc_Absyn_FnInfo _T73;int(*_T74)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);int(*_T75)(int(*)(void*),struct Cyc_List_List*);int(*_T76)(struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T77;int _T78;struct Cyc_Warn_String_Warn_Warg_struct _T79;struct Cyc_Warn_String_Warn_Warg_struct _T7A;struct Cyc_Warn_String_Warn_Warg_struct _T7B;struct Cyc_Warn_Typ_Warn_Warg_struct _T7C;void**_T7D;void**_T7E;void**_T7F;void**_T80;unsigned _T81;struct _fat_ptr _T82;_T1=
# 1373
Cyc_Tcutil_is_function_type(t);if(!_T1)goto _TL268;_T3=te;_T4=_T3->in_tempest;if(_T4)goto _TL26A;else{goto _TL26C;}_TL26C: _T2=1;goto _TL26B;_TL26A: _T5=te;_T2=_T5->tempest_generalize;_TL26B: _T0=_T2;goto _TL269;_TL268: _T0=0;_TL269: {
# 1372
int generalize_evars=_T0;_T7=
# 1374
Cyc_Tcutil_is_function_type(t);if(!_T7)goto _TL26D;_T8=& Cyc_Kinds_ak;_T6=(struct Cyc_Absyn_Kind*)_T8;goto _TL26E;_TL26D: _T9=& Cyc_Kinds_mk;_T6=(struct Cyc_Absyn_Kind*)_T9;_TL26E: {struct Cyc_Absyn_Kind*expected_kind=_T6;
struct Cyc_List_List*te_vars=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Tctyp_CVTEnv _T83;
_T83.loc=loc;_T83.te=te;
_T83.kind_env=te_vars;_T83.fn_result=0;_T83.def_aliasable=0;_T83.inst_type_evar=0;_T83.generalize_evars=generalize_evars;_T83.allow_aqual_bounds=1;_T83.free_vars=0;_T83.free_evars=0;_T83.free_qualvars=0;_TA=_T83;}_TB=expected_kind;_TC=t;{
# 1376
struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_TA,_TB,1,_TC);_TE=Cyc_List_map;{
# 1379
struct Cyc_List_List*(*_T83)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))_TE;_TD=_T83;}_T10=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T83)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T10;_TF=_T83;}_T11=cvt;_T12=_T11.free_vars;{struct Cyc_List_List*free_tvars=_TD(_TF,_T12);_T13=cvt;{
struct Cyc_List_List*free_evars=_T13.free_evars;_T14=cvt;{
struct Cyc_List_List*free_qualvars=_T14.free_qualvars;
# 1383
if(te_vars==0)goto _TL26F;{
struct Cyc_List_List*res=0;{
struct Cyc_List_List*fs=free_tvars;_TL274: _T15=fs;_T16=(unsigned)_T15;if(_T16)goto _TL272;else{goto _TL273;}
_TL272:{struct Cyc_List_List*ts=te_vars;
_TL278: if(ts!=0)goto _TL276;else{goto _TL277;}
_TL276: _T17=fs;_T18=_T17->hd;_T19=(struct Cyc_Absyn_Tvar*)_T18;_T1A=ts;_T1B=_T1A->hd;_T1C=(struct Cyc_Absyn_Tvar*)_T1B;_T1D=Cyc_Absyn_tvar_cmp(_T19,_T1C);if(_T1D!=0)goto _TL279;goto _TL277;_TL279: _T1E=ts;
# 1387
ts=_T1E->tl;goto _TL278;_TL277:
# 1390
 if(ts!=0)goto _TL27B;{struct Cyc_List_List*_T83=_cycalloc(sizeof(struct Cyc_List_List));_T20=fs;_T21=_T20->hd;
_T83->hd=(struct Cyc_Absyn_Tvar*)_T21;_T83->tl=res;_T1F=(struct Cyc_List_List*)_T83;}res=_T1F;goto _TL27C;_TL27B: _TL27C:;}_T22=fs;
# 1385
fs=_T22->tl;goto _TL274;_TL273:;}
# 1393
free_tvars=Cyc_List_imp_rev(res);}goto _TL270;_TL26F: _TL270:{
# 1398
struct Cyc_List_List*x=free_tvars;_TL280: if(x!=0)goto _TL27E;else{goto _TL27F;}
_TL27E: _T23=x;_T24=_T23->hd;_T25=(struct Cyc_Absyn_Tvar*)_T24;_T26=_T25->kind;{void*_T83=Cyc_Kinds_compress_kb(_T26);enum Cyc_Absyn_AliasHint _T84;struct Cyc_Absyn_Kind*_T85;void*_T86;_T27=(int*)_T83;_T28=*_T27;switch(_T28){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T87=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T83;_T29=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T83;_T2A=& _T29->f1;_T86=(struct Cyc_Core_Opt**)_T2A;}{struct Cyc_Core_Opt**f=_T86;_T2B=f;_T2C=& Cyc_Kinds_bk;_T2D=(struct Cyc_Absyn_Kind*)_T2C;
*_T2B=Cyc_Kinds_kind_to_bound_opt(_T2D);goto _LL0;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T83;_T2E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T83;_T2F=& _T2E->f1;_T86=(struct Cyc_Core_Opt**)_T2F;_T85=_T87->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T86;struct Cyc_Absyn_Kind*k=_T85;_T30=k;_T31=(struct Cyc_Absyn_Kind*)_T30;_T32=_T31->kind;_T33=(int)_T32;switch(_T33){case Cyc_Absyn_BoxKind: goto _LLD;case Cyc_Absyn_MemKind: _LLD: _T34=f;_T35=& Cyc_Kinds_bk;_T36=(struct Cyc_Absyn_Kind*)_T35;
# 1405
*_T34=Cyc_Kinds_kind_to_bound_opt(_T36);goto _LL9;default: _T37=f;
# 1407
*_T37=Cyc_Kinds_kind_to_bound_opt(k);goto _LL9;}_LL9: goto _LL0;}default: _T38=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T83;_T39=_T38->f1;_T3A=(struct Cyc_Absyn_Kind*)_T39;_T3B=_T3A->kind;if(_T3B!=Cyc_Absyn_MemKind)goto _TL283;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T87=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T83;_T3C=_T87->f1;{struct Cyc_Absyn_Kind _T88=*_T3C;_T84=_T88.aliashint;}}{enum Cyc_Absyn_AliasHint a=_T84;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
# 1411
_T87.f1=_tag_fat("type variable ",sizeof(char),15U);_T3D=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T3D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T88;_T88.tag=7;_T3F=x;_T40=_T3F->hd;_T88.f1=(struct Cyc_Absyn_Tvar*)_T40;_T3E=_T88;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T88=_T3E;{struct Cyc_Warn_String_Warn_Warg_struct _T89;_T89.tag=0;_T89.f1=_tag_fat(" cannot have kind ",sizeof(char),19U);_T41=_T89;}{struct Cyc_Warn_String_Warn_Warg_struct _T89=_T41;{struct Cyc_Warn_Kind_Warn_Warg_struct _T8A;_T8A.tag=9;{struct Cyc_Absyn_Kind*_T8B=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_T8B->kind=1U;_T8B->aliashint=a;_T43=(struct Cyc_Absyn_Kind*)_T8B;}_T8A.f1=_T43;_T42=_T8A;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T8A=_T42;void*_T8B[4];_T44=_T8B + 0;*_T44=& _T87;_T45=_T8B + 1;*_T45=& _T88;_T46=_T8B + 2;*_T46=& _T89;_T47=_T8B + 3;*_T47=& _T8A;_T48=loc;_T49=_tag_fat(_T8B,sizeof(void*),4);Cyc_Warn_err2(_T48,_T49);}}}}goto _LL0;}_TL283: goto _LL0;}_LL0:;}_T4A=x;
# 1398
x=_T4A->tl;goto _TL280;_TL27F:;}
# 1417
if(free_tvars!=0)goto _TL287;else{goto _TL289;}_TL289: if(free_evars!=0)goto _TL287;else{goto _TL288;}_TL288: if(free_qualvars!=0)goto _TL287;else{goto _TL285;}
_TL287:{void*_T83=Cyc_Absyn_compress(t);void*_T84;void*_T85;_T4B=(int*)_T83;_T4C=*_T4B;if(_T4C!=6)goto _TL28A;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T86=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T83;_T4D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T83;_T4E=& _T4D->f1.tvars;_T85=(struct Cyc_List_List**)_T4E;_T4F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T83;_T50=& _T4F->f1.qual_bnd;_T84=(struct Cyc_List_List**)_T50;}{struct Cyc_List_List**btvs=(struct Cyc_List_List**)_T85;struct Cyc_List_List**qb=(struct Cyc_List_List**)_T84;_T51=loc;_T52=cvt;_T54=Cyc_List_map_c;{
# 1421
struct Cyc_List_List*(*_T86)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T54;_T53=_T86;}_T55=Cyc_Tcutil_tvar_bool_pair;_T56=btvs;_T57=*_T56;_T58=_T53(_T55,0,_T57);_T59=qb;_T5A=Cyc_Absyn_al_qual_type;Cyc_Tctyp_update_qualifier_bounds(_T51,_T52,_T58,_T59,_T5A);_T5B=btvs;_T5C=*_T5B;
# 1423
if(_T5C!=0)goto _TL28C;_T5D=btvs;_T5F=Cyc_List_filter;{
# 1425
struct Cyc_List_List*(*_T86)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T5F;_T5E=_T86;}_T60=Cyc_CurRgn_is_not_curr_rgn;_T61=free_tvars;*_T5D=_T5E(_T60,_T61);
free_tvars=0;goto _TL28D;_TL28C: _TL28D: goto _LL10;}_TL28A: goto _LL10;_LL10:;}
# 1431
if(free_tvars==0)goto _TL28E;{struct Cyc_Warn_String_Warn_Warg_struct _T83;_T83.tag=0;
_T83.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_T62=_T83;}{struct Cyc_Warn_String_Warn_Warg_struct _T83=_T62;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T84;_T84.tag=7;_T64=free_tvars;_T65=_T64->hd;_T84.f1=(struct Cyc_Absyn_Tvar*)_T65;_T63=_T84;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T84=_T63;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;_T85.f1=_tag_fat(" when checking ",sizeof(char),16U);_T66=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T66;{struct Cyc_Warn_Typ_Warn_Warg_struct _T86;_T86.tag=2;_T86.f1=t;_T67=_T86;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T86=_T67;void*_T87[4];_T68=_T87 + 0;*_T68=& _T83;_T69=_T87 + 1;*_T69=& _T84;_T6A=_T87 + 2;*_T6A=& _T85;_T6B=_T87 + 3;*_T6B=& _T86;_T6C=loc;_T6D=_tag_fat(_T87,sizeof(void*),4);Cyc_Warn_err2(_T6C,_T6D);}}}}goto _TL28F;_TL28E: _TL28F: _T6E=
Cyc_Tcutil_is_function_type(t);if(_T6E)goto _TL293;else{goto _TL292;}_TL293: _T6F=te;_T70=_T6F->in_tempest;if(_T70)goto _TL290;else{goto _TL292;}
_TL292: Cyc_Tctyp_check_free_evars(free_evars,t,loc);goto _TL291;_TL290: _TL291: goto _TL286;_TL285: _TL286: {
# 1437
void*_T83=Cyc_Absyn_compress(t);struct Cyc_List_List*_T84;_T71=(int*)_T83;_T72=*_T71;if(_T72!=6)goto _TL294;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T85=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T83;_T73=_T85->f1;_T84=_T73.tvars;}{struct Cyc_List_List*btvs=_T84;_T75=Cyc_List_exists;{
# 1439
int(*_T85)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T75;_T74=_T85;}_T76=Cyc_CurRgn_is_curr_rgn;_T77=btvs;_T78=_T74(_T76,_T77);if(!_T78)goto _TL296;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 1441
_T85.f1=_tag_fat("cannot abstract current region ",sizeof(char),32U);_T79=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T79;{struct Cyc_Warn_String_Warn_Warg_struct _T86;_T86.tag=0;_T86.f1=Cyc_CurRgn_curr_rgn_name;_T7A=_T86;}{struct Cyc_Warn_String_Warn_Warg_struct _T86=_T7A;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;_T87.f1=_tag_fat(" in ",sizeof(char),5U);_T7B=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T7B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T88;_T88.tag=2;_T88.f1=t;_T7C=_T88;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T88=_T7C;void*_T89[4];_T7D=_T89 + 0;*_T7D=& _T85;_T7E=_T89 + 1;*_T7E=& _T86;_T7F=_T89 + 2;*_T7F=& _T87;_T80=_T89 + 3;*_T80=& _T88;_T81=loc;_T82=_tag_fat(_T89,sizeof(void*),4);Cyc_Warn_err2(_T81,_T82);}}}}goto _TL297;_TL296: _TL297: goto _LL15;}_TL294: goto _LL15;_LL15:;}}}}}}}}
# 1451
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){int*_T0;int _T1;struct Cyc_Absyn_Fndecl*_T2;struct Cyc_Absyn_FnInfo _T3;struct Cyc_Absyn_Fndecl*_T4;struct Cyc_Absyn_Fndecl*_T5;struct Cyc_Absyn_Fndecl*_T6;unsigned _T7;struct Cyc_Absyn_FnInfo _T8;struct Cyc_Absyn_Tqual _T9;int _TA;struct Cyc_Absyn_FnInfo _TB;void*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;void**_TE;int(*_TF)(struct _fat_ptr);void*(*_T10)(struct _fat_ptr);struct _fat_ptr _T11;struct Cyc_Absyn_Fndecl*_T12;struct Cyc_Absyn_FnInfo _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;struct _tuple9*_T17;struct _tuple9 _T18;struct _fat_ptr*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;unsigned _T1C;struct _fat_ptr _T1D;struct Cyc_Absyn_Fndecl*_T1E;
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1454
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);{
void*_T1F=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _T20;_T0=(int*)_T1F;_T1=*_T0;if(_T1!=6)goto _TL298;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T21=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1F;_T20=_T21->f1;}{struct Cyc_Absyn_FnInfo i=_T20;_T2=fd;_T3=_T2->i;{
# 1457
struct Cyc_List_List*atts=_T3.attributes;_T4=fd;
_T4->i=i;_T5=fd;
_T5->i.attributes=atts;_T6=fd;_T7=loc;_T8=i;_T9=_T8.ret_tqual;_TA=_T9.print_const;_TB=i;_TC=_TB.ret_type;
_T6->i.ret_tqual.real_const=
Cyc_Tcutil_extract_const_from_typedef(_T7,_TA,_TC);goto _LL0;}}_TL298:{struct Cyc_Warn_String_Warn_Warg_struct _T21;_T21.tag=0;
# 1463
_T21.f1=_tag_fat("check_fndecl_valid_type: not a FnType",sizeof(char),38U);_TD=_T21;}{struct Cyc_Warn_String_Warn_Warg_struct _T21=_TD;void*_T22[1];_TE=_T22 + 0;*_TE=& _T21;_T10=Cyc_Warn_impos2;{int(*_T23)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T10;_TF=_T23;}_T11=_tag_fat(_T22,sizeof(void*),1);_TF(_T11);}_LL0:;}{
# 1465
struct Cyc_List_List*argnames=0;_T12=fd;_T13=_T12->i;{
struct Cyc_List_List*args=_T13.args;_TL29D: if(args!=0)goto _TL29B;else{goto _TL29C;}
_TL29B:{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));_T15=args;_T16=_T15->hd;_T17=(struct _tuple9*)_T16;_T18=*_T17;_T19=_T18.f0;_T1F->hd=_check_null(_T19);_T1F->tl=argnames;_T14=(struct Cyc_List_List*)_T1F;}argnames=_T14;_T1A=args;
# 1466
args=_T1A->tl;goto _TL29D;_TL29C:;}_T1B=argnames;_T1C=loc;_T1D=
# 1468
_tag_fat("function declaration has repeated parameter",sizeof(char),44U);Cyc_Tcutil_check_unique_vars(_T1B,_T1C,_T1D);_T1E=fd;
# 1470
_T1E->cached_type=t;}}
# 1478
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Absyn_Kind*_T1;int _T2;void*_T3;struct Cyc_List_List*(*_T4)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Tvar*(*_T8)(struct _tuple12*);void*(*_T9)(struct _tuple0*);struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Tvar_Warn_Warg_struct _TB;struct Cyc_List_List*_TC;void*_TD;struct _tuple12*_TE;struct _tuple12 _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;void**_T12;void**_T13;void**_T14;void**_T15;unsigned _T16;struct _fat_ptr _T17;struct Cyc_List_List*_T18;int _T19;struct Cyc_Tctyp_CVTEnv _T1A;struct Cyc_List_List*_T1B;void*_T1C;unsigned _T1D;{struct Cyc_Tctyp_CVTEnv _T1E;
# 1481
_T1E.loc=loc;_T1E.te=te;_T1E.kind_env=bound_tvars;_T1E.fn_result=0;_T1E.def_aliasable=0;_T1E.inst_type_evar=0;_T1E.generalize_evars=0;_T1E.allow_aqual_bounds=0;_T1E.free_vars=0;_T1E.free_evars=0;_T1E.free_qualvars=0;_T0=_T1E;}_T1=expected_kind;_T2=allow_abs_aggr;_T3=t;{struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_T0,_T1,_T2,_T3);{
# 1483
struct Cyc_List_List*(*_T1E)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T4=_T1E;}_T5=cvt;_T6=_T5.free_vars;_T7=bound_tvars;_T9=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T1E)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T9;_T8=_T1E;}{struct Cyc_List_List*free_tvs=_T4(_T6,_T7,_T8);
_TL2A1: if(free_tvs!=0)goto _TL29F;else{goto _TL2A0;}
_TL29F:{struct Cyc_Warn_String_Warn_Warg_struct _T1E;_T1E.tag=0;_T1E.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_TA=_T1E;}{struct Cyc_Warn_String_Warn_Warg_struct _T1E=_TA;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T1F;_T1F.tag=7;_TC=free_tvs;_TD=_TC->hd;_TE=(struct _tuple12*)_TD;_TF=*_TE;_T1F.f1=_TF.f0;_TB=_T1F;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T1F=_TB;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;_T20.f1=_tag_fat(" in type ",sizeof(char),10U);_T10=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T21;_T21.tag=2;_T21.f1=t;_T11=_T21;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T21=_T11;void*_T22[4];_T12=_T22 + 0;*_T12=& _T1E;_T13=_T22 + 1;*_T13=& _T1F;_T14=_T22 + 2;*_T14=& _T20;_T15=_T22 + 3;*_T15=& _T21;_T16=loc;_T17=_tag_fat(_T22,sizeof(void*),4);Cyc_Warn_err2(_T16,_T17);}}}}_T18=free_tvs;
# 1484
free_tvs=_T18->tl;goto _TL2A1;_TL2A0: _T19=allow_evars;
# 1486
if(_T19)goto _TL2A2;else{goto _TL2A4;}
_TL2A4: _T1A=cvt;_T1B=_T1A.free_evars;_T1C=t;_T1D=loc;Cyc_Tctyp_check_free_evars(_T1B,_T1C,_T1D);goto _TL2A3;_TL2A2: _TL2A3:;}}}
# 1490
void Cyc_Tctyp_check_type_with_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*flagged_tvars,struct Cyc_List_List**qual_bnd,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){struct Cyc_List_List*(*_T0)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T2)(struct _tuple12*);void*(*_T3)(struct _tuple0*);struct Cyc_List_List*_T4;struct Cyc_Tctyp_CVTEnv _T5;struct Cyc_Absyn_Kind*_T6;int _T7;void*_T8;struct Cyc_List_List**_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Tctyp_CVTEnv _TC;struct Cyc_Absyn_Kind*_TD;struct Cyc_Absyn_Kind*_TE;int _TF;void*_T10;struct Cyc_Tctyp_CVTEnv _T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Absyn_Kind*_T13;int _T14;void*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*(*_T17)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_Tvar*(*_T1B)(struct _tuple12*);void*(*_T1C)(struct _tuple0*);struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T1E;struct Cyc_List_List*_T1F;void*_T20;struct _tuple12*_T21;struct _tuple12 _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_Typ_Warn_Warg_struct _T24;void**_T25;void**_T26;void**_T27;void**_T28;unsigned _T29;struct _fat_ptr _T2A;struct Cyc_List_List*_T2B;int _T2C;struct Cyc_Tctyp_CVTEnv _T2D;struct Cyc_List_List*_T2E;void*_T2F;unsigned _T30;_T1=Cyc_List_map;{
# 1494
struct Cyc_List_List*(*_T31)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))_T1;_T0=_T31;}_T3=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T31)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T3;_T2=_T31;}_T4=flagged_tvars;{struct Cyc_List_List*bound_tvars=_T0(_T2,_T4);{struct Cyc_Tctyp_CVTEnv _T31;
_T31.loc=loc;_T31.te=te;_T31.kind_env=bound_tvars;_T31.fn_result=0;_T31.def_aliasable=0;_T31.inst_type_evar=0;_T31.generalize_evars=0;_T31.allow_aqual_bounds=1;_T31.free_vars=0;_T31.free_evars=0;_T31.free_qualvars=0;_T5=_T31;}_T6=expected_kind;_T7=allow_abs_aggr;_T8=t;{struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_T5,_T6,_T7,_T8);_T9=qual_bnd;{
# 1499
struct Cyc_List_List*qbit=*_T9;_TL2A8: if(qbit!=0)goto _TL2A6;else{goto _TL2A7;}
_TL2A6: _TA=qbit;_TB=_TA->hd;{struct _tuple0*_T31=(struct _tuple0*)_TB;void*_T32;void*_T33;{struct _tuple0 _T34=*_T31;_T33=_T34.f0;_T32=_T34.f1;}{void*tv=_T33;void*bnd=_T32;_TC=cvt;_TD=& Cyc_Kinds_aqk;_TE=(struct Cyc_Absyn_Kind*)_TD;_TF=allow_abs_aggr;_T10=tv;
cvt=Cyc_Tctyp_check_valid_type(_TC,_TE,_TF,_T10);_T11=cvt;_T12=& Cyc_Kinds_aqk;_T13=(struct Cyc_Absyn_Kind*)_T12;_T14=allow_abs_aggr;_T15=bnd;
cvt=Cyc_Tctyp_check_valid_type(_T11,_T13,_T14,_T15);}}_T16=qbit;
# 1499
qbit=_T16->tl;goto _TL2A8;_TL2A7:;}{
# 1505
struct Cyc_List_List*(*_T31)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T17=_T31;}_T18=cvt;_T19=_T18.free_vars;_T1A=bound_tvars;_T1C=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T31)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T1C;_T1B=_T31;}{struct Cyc_List_List*free_tvs=_T17(_T19,_T1A,_T1B);
_TL2AC: if(free_tvs!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA:{struct Cyc_Warn_String_Warn_Warg_struct _T31;_T31.tag=0;_T31.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_T1D=_T31;}{struct Cyc_Warn_String_Warn_Warg_struct _T31=_T1D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T32;_T32.tag=7;_T1F=free_tvs;_T20=_T1F->hd;_T21=(struct _tuple12*)_T20;_T22=*_T21;_T32.f1=_T22.f0;_T1E=_T32;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T32=_T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T33;_T33.tag=0;_T33.f1=_tag_fat(" in type ",sizeof(char),10U);_T23=_T33;}{struct Cyc_Warn_String_Warn_Warg_struct _T33=_T23;{struct Cyc_Warn_Typ_Warn_Warg_struct _T34;_T34.tag=2;_T34.f1=t;_T24=_T34;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T34=_T24;void*_T35[4];_T25=_T35 + 0;*_T25=& _T31;_T26=_T35 + 1;*_T26=& _T32;_T27=_T35 + 2;*_T27=& _T33;_T28=_T35 + 3;*_T28=& _T34;_T29=loc;_T2A=_tag_fat(_T35,sizeof(void*),4);Cyc_Warn_err2(_T29,_T2A);}}}}_T2B=free_tvs;
# 1506
free_tvs=_T2B->tl;goto _TL2AC;_TL2AB: _T2C=allow_evars;
# 1508
if(_T2C)goto _TL2AD;else{goto _TL2AF;}
_TL2AF: _T2D=cvt;_T2E=_T2D.free_evars;_T2F=t;_T30=loc;Cyc_Tctyp_check_free_evars(_T2E,_T2F,_T30);goto _TL2AE;_TL2AD: _TL2AE:
 Cyc_Tctyp_update_qualifier_bounds(loc,cvt,flagged_tvars,qual_bnd,Cyc_Absyn_rtd_qual_type);}}}}
