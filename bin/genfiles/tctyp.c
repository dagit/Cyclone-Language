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
static struct Cyc_List_List*Cyc_Tctyp_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_T0;int _T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;struct Cyc_Warn_Tvar_Warn_Warg_struct _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;int(*_T6)(struct _fat_ptr);void*(*_T7)(struct _fat_ptr);struct _fat_ptr _T8;struct Cyc_List_List*_T9;_T0=tv;_T1=_T0->identity;_T2=- 1;
if(_T1!=_T2)goto _TLC;{struct Cyc_Warn_String_Warn_Warg_struct _TA;_TA.tag=0;
_TA.f1=_tag_fat("bound tvar id for ",sizeof(char),19U);_T3=_TA;}{struct Cyc_Warn_String_Warn_Warg_struct _TA=_T3;{struct Cyc_Warn_Tvar_Warn_Warg_struct _TB;_TB.tag=7;_TB.f1=tv;_T4=_TB;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _TB=_T4;{struct Cyc_Warn_String_Warn_Warg_struct _TC;_TC.tag=0;_TC.f1=_tag_fat(" is NULL",sizeof(char),9U);_T5=_TC;}{struct Cyc_Warn_String_Warn_Warg_struct _TC=_T5;void*_TD[3];_TD[0]=& _TA;_TD[1]=& _TB;_TD[2]=& _TC;_T7=Cyc_Warn_impos2;{int(*_TE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T7;_T6=_TE;}_T8=_tag_fat(_TD,sizeof(void*),3);_T6(_T8);}}}goto _TLD;_TLC: _TLD:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));
_TA->hd=tv;_TA->tl=tvs;_T9=(struct Cyc_List_List*)_TA;}return _T9;}
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
static void Cyc_Tctyp_check_free_evars(struct Cyc_List_List*free_evars,void*in_typ,unsigned loc){struct Cyc_List_List*_T0;void*_T1;struct _tuple14*_T2;struct _tuple14 _T3;int*_T4;int _T5;struct Cyc_Absyn_Kind*_T6;enum Cyc_Absyn_KindQual _T7;int _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;struct Cyc_Warn_Typ_Warn_Warg_struct _TC;unsigned _TD;struct _fat_ptr _TE;int _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;int(*_T15)(struct _fat_ptr);void*(*_T16)(struct _fat_ptr);struct _fat_ptr _T17;struct Cyc_List_List*_T18;
_TL1D: if(free_evars!=0)goto _TL1B;else{goto _TL1C;}
_TL1B: _T0=free_evars;_T1=_T0->hd;_T2=(struct _tuple14*)_T1;_T3=*_T2;{void*e=_T3.f0;{
void*_T19=Cyc_Absyn_compress(e);_T4=(int*)_T19;_T5=*_T4;if(_T5!=1)goto _TL1E;goto _LL0;_TL1E: goto _TL1A;_LL0:;}{
# 149
void*t;
struct _fat_ptr s;{
struct Cyc_Absyn_Kind*_T19=Cyc_Tcutil_type_kind(e);_T6=(struct Cyc_Absyn_Kind*)_T19;_T7=_T6->kind;_T8=(int)_T7;switch(_T8){case Cyc_Absyn_EffKind:
# 153
 t=Cyc_Absyn_heap_rgn_type;s=_tag_fat("{`H}",sizeof(char),5U);goto _LL5;case Cyc_Absyn_BoolKind:
 t=Cyc_Absyn_false_type;s=_tag_fat("@false",sizeof(char),7U);goto _LL5;case Cyc_Absyn_PtrBndKind:
 t=Cyc_Absyn_bounds_one();s=_tag_fat("bounds_one",sizeof(char),11U);goto _LL5;case Cyc_Absyn_AqualKind:
 t=Cyc_Absyn_al_qual_type;s=_tag_fat("aliasable",sizeof(char),10U);goto _LL5;default:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 158
_T1A.f1=_tag_fat("hidden type variable ",sizeof(char),22U);_T9=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B;_T1B.tag=2;_T1B.f1=e;_TA=_T1B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1B=_TA;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=_tag_fat(" isn't abstracted in type ",sizeof(char),27U);_TB=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_TB;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1D;_T1D.tag=2;_T1D.f1=in_typ;_TC=_T1D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1D=_TC;void*_T1E[4];_T1E[0]=& _T1A;_T1E[1]=& _T1B;_T1E[2]=& _T1C;_T1E[3]=& _T1D;_TD=loc;_TE=_tag_fat(_T1E,sizeof(void*),4);Cyc_Warn_err2(_TD,_TE);}}}}
return;}_LL5:;}_TF=
# 161
Cyc_Unify_unify(e,t);if(_TF)goto _TL21;else{goto _TL23;}
_TL23:{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;_T19.f1=_tag_fat("can't unify evar ",sizeof(char),18U);_T10=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1A;_T1A.tag=2;_T1A.f1=e;_T11=_T1A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1A=_T11;{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;_T1B.f1=_tag_fat(" with ",sizeof(char),7U);_T12=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T12;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=s;_T13=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T13;{struct Cyc_Warn_String_Warn_Warg_struct _T1D;_T1D.tag=0;_T1D.f1=_tag_fat("!",sizeof(char),2U);_T14=_T1D;}{struct Cyc_Warn_String_Warn_Warg_struct _T1D=_T14;void*_T1E[5];_T1E[0]=& _T19;_T1E[1]=& _T1A;_T1E[2]=& _T1B;_T1E[3]=& _T1C;_T1E[4]=& _T1D;_T16=Cyc_Warn_impos2;{int(*_T1F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T16;_T15=_T1F;}_T17=_tag_fat(_T1E,sizeof(void*),5);_T15(_T17);}}}}}goto _TL22;_TL21: _TL22:;}}_TL1A: _T18=free_evars;
# 143
free_evars=_T18->tl;goto _TL1D;_TL1C:;}
# 172
static struct Cyc_List_List*Cyc_Tctyp_add_free_tvar(unsigned loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int allow_aqual_bounds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;struct _fat_ptr*_T3;struct Cyc_Absyn_Tvar*_T4;struct _fat_ptr*_T5;int _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;struct Cyc_Absyn_Tvar*_TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_Tvar_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_KindBound_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_KindBound_Warn_Warg_struct _T11;unsigned _T12;struct _fat_ptr _T13;struct Cyc_Absyn_Tvar*_T14;void*_T15;int _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_Tvar_Warn_Warg_struct _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;unsigned _T1A;struct _fat_ptr _T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Tvar*_T1E;void*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Tvar*_T22;struct Cyc_Absyn_Tvar*_T23;struct Cyc_Absyn_Tvar*_T24;void*_T25;struct Cyc_Absyn_Tvar*_T26;void*_T27;struct Cyc_List_List*_T28;void*_T29;struct Cyc_Absyn_Tvar*_T2A;void*_T2B;int _T2C;struct Cyc_Warn_String_Warn_Warg_struct _T2D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T2E;struct Cyc_Warn_String_Warn_Warg_struct _T2F;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T30;struct Cyc_Absyn_Tvar*_T31;struct Cyc_Warn_String_Warn_Warg_struct _T32;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T33;struct Cyc_List_List*_T34;void*_T35;struct Cyc_Absyn_Tvar*_T36;unsigned _T37;struct _fat_ptr _T38;struct Cyc_Absyn_Tvar*_T39;int _T3A;int _T3B;struct Cyc_Absyn_Tvar*_T3C;struct Cyc_List_List*_T3D;void*_T3E;struct Cyc_Absyn_Tvar*_T3F;struct Cyc_Absyn_Tvar*_T40;int _T41;struct Cyc_List_List*_T42;void*_T43;struct Cyc_Absyn_Tvar*_T44;int _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;int(*_T47)(struct _fat_ptr);void*(*_T48)(struct _fat_ptr);struct _fat_ptr _T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;struct Cyc_Absyn_Tvar*_T4C;struct Cyc_List_List*_T4D;{
struct Cyc_List_List*tvs2=tvs;_TL27: if(tvs2!=0)goto _TL25;else{goto _TL26;}
_TL25: _T0=tvs2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Tvar*)_T1;_T3=_T2->name;_T4=tv;_T5=_T4->name;_T6=Cyc_strptrcmp(_T3,_T5);if(_T6!=0)goto _TL28;_T7=tvs2;_T8=_T7->hd;_T9=(struct Cyc_Absyn_Tvar*)_T8;{
void*k1=_T9->kind;_TA=tv;{
void*k2=_TA->kind;_TB=
Cyc_Kinds_constrain_kinds(k1,k2);if(_TB)goto _TL2A;else{goto _TL2C;}
_TL2C:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("type variable ",sizeof(char),15U);_TC=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_TC;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F;_T4F.tag=7;_T4F.f1=tv;_TD=_T4F;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;_T50.f1=_tag_fat(" used with inconsistent kinds ",sizeof(char),31U);_TE=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_TE;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T51;_T51.tag=8;
_T51.f1=k1;_TF=_T51;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T51=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat(" and ",sizeof(char),6U);_T10=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T10;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T53;_T53.tag=8;_T53.f1=k2;_T11=_T53;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T53=_T11;void*_T54[6];_T54[0]=& _T4E;_T54[1]=& _T4F;_T54[2]=& _T50;_T54[3]=& _T51;_T54[4]=& _T52;_T54[5]=& _T53;_T12=loc;_T13=_tag_fat(_T54,sizeof(void*),6);Cyc_Warn_err2(_T12,_T13);}}}}}}goto _TL2B;_TL2A: _TL2B: _T14=tv;_T15=_T14->aquals_bound;
if(_T15==0)goto _TL2D;_T16=allow_aqual_bounds;if(_T16)goto _TL2D;else{goto _TL2F;}
_TL2F:{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;_T4E.f1=_tag_fat("alias bound appears on type variable ",sizeof(char),38U);_T17=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T17;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F;_T4F.tag=7;_T4F.f1=tv;_T18=_T4F;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F=_T18;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;
_T50.f1=_tag_fat(" which is not in a function declaration or aggregate field",sizeof(char),59U);_T19=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_T19;void*_T51[3];_T51[0]=& _T4E;_T51[1]=& _T4F;_T51[2]=& _T50;_T1A=loc;_T1B=_tag_fat(_T51,sizeof(void*),3);Cyc_Warn_err2(_T1A,_T1B);}}}goto _TL2E;_TL2D: _TL2E: _T1C=tvs2;_T1D=_T1C->hd;_T1E=(struct Cyc_Absyn_Tvar*)_T1D;_T1F=_T1E->aquals_bound;
if(_T1F!=0)goto _TL30;_T20=tvs2;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Tvar*)_T21;_T23=tv;
_T22->aquals_bound=_T23->aquals_bound;goto _TL31;
_TL30: _T24=tv;_T25=_T24->aquals_bound;if(_T25==0)goto _TL32;_T26=tv;_T27=_T26->aquals_bound;_T28=tvs2;_T29=_T28->hd;_T2A=(struct Cyc_Absyn_Tvar*)_T29;_T2B=_T2A->aquals_bound;_T2C=Cyc_Tcutil_typecmp(_T27,_T2B);if(!_T2C)goto _TL32;{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;
_T4E.f1=_tag_fat("type variable ",sizeof(char),15U);_T2D=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T2D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F;_T4F.tag=7;_T4F.f1=tv;_T2E=_T4F;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T4F=_T2E;{struct Cyc_Warn_String_Warn_Warg_struct _T50;_T50.tag=0;_T50.f1=_tag_fat(" used with inconsistent alias qualifier bounds ",sizeof(char),48U);_T2F=_T50;}{struct Cyc_Warn_String_Warn_Warg_struct _T50=_T2F;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T51;_T51.tag=3;_T31=tv;
_T51.f1=_T31->aquals_bound;_T30=_T51;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T51=_T30;{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;_T52.f1=_tag_fat(" and ",sizeof(char),6U);_T32=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_T32;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T53;_T53.tag=3;_T34=tvs2;_T35=_T34->hd;_T36=(struct Cyc_Absyn_Tvar*)_T35;_T53.f1=_T36->aquals_bound;_T33=_T53;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T53=_T33;void*_T54[6];_T54[0]=& _T4E;_T54[1]=& _T4F;_T54[2]=& _T50;_T54[3]=& _T51;_T54[4]=& _T52;_T54[5]=& _T53;_T37=loc;_T38=_tag_fat(_T54,sizeof(void*),6);Cyc_Warn_err2(_T37,_T38);}}}}}}goto _TL33;_TL32: _TL33: _TL31: _T39=tv;_T3A=_T39->identity;_T3B=- 1;
if(_T3A!=_T3B)goto _TL34;_T3C=tv;_T3D=tvs2;_T3E=_T3D->hd;_T3F=(struct Cyc_Absyn_Tvar*)_T3E;
_T3C->identity=_T3F->identity;goto _TL35;
_TL34: _T40=tv;_T41=_T40->identity;_T42=tvs2;_T43=_T42->hd;_T44=(struct Cyc_Absyn_Tvar*)_T43;_T45=_T44->identity;if(_T41==_T45)goto _TL36;{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;
_T4E.f1=_tag_fat("same type variable has different identity!",sizeof(char),43U);_T46=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T46;void*_T4F[1];_T4F[0]=& _T4E;_T48=Cyc_Warn_impos2;{int(*_T50)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T48;_T47=_T50;}_T49=_tag_fat(_T4F,sizeof(void*),1);_T47(_T49);}goto _TL37;_TL36: _TL37: _TL35: _T4A=tvs;
return _T4A;}}_TL28: _T4B=tvs2;
# 173
tvs2=_T4B->tl;goto _TL27;_TL26:;}_T4C=tv;
# 194
_T4C->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*_T4E=_cycalloc(sizeof(struct Cyc_List_List));
_T4E->hd=tv;_T4E->tl=tvs;_T4D=(struct Cyc_List_List*)_T4E;}return _T4D;}
# 198
static int Cyc_Tctyp_cmp_fqv(struct _tuple14*te1,struct _tuple14*te2){struct _tuple14*_T0;struct _tuple14 _T1;struct _tuple14*_T2;struct _tuple14 _T3;int _T4;int _T5;_T0=te1;_T1=*_T0;{
void*t1=_T1.f0;_T2=te2;_T3=*_T2;{
void*t2=_T3.f0;_T4=
Cyc_Tcutil_typecmp(t1,t2);_T5=_T4==0;return _T5;}}}
# 205
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_T0;int _T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;int(*_T4)(struct _fat_ptr);void*(*_T5)(struct _fat_ptr);struct _fat_ptr _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;int _TA;int _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;int(*_TD)(struct _fat_ptr);void*(*_TE)(struct _fat_ptr);struct _fat_ptr _TF;struct Cyc_Absyn_Tvar*_T10;int _T11;struct Cyc_Absyn_Tvar*_T12;int _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;_T0=tv;_T1=_T0->identity;_T2=- 1;
if(_T1!=_T2)goto _TL38;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
_T17.f1=_tag_fat("fast_add_free_tvar: bad identity in tv",sizeof(char),39U);_T3=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_T3;void*_T18[1];_T18[0]=& _T17;_T5=Cyc_Warn_impos2;{int(*_T19)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5;_T4=_T19;}_T6=_tag_fat(_T18,sizeof(void*),1);_T4(_T6);}goto _TL39;_TL38: _TL39:{
struct Cyc_List_List*tvs2=tvs;_TL3D: if(tvs2!=0)goto _TL3B;else{goto _TL3C;}
_TL3B: _T7=tvs2;_T8=_T7->hd;{struct Cyc_Absyn_Tvar*tv2=(struct Cyc_Absyn_Tvar*)_T8;_T9=tv2;_TA=_T9->identity;_TB=- 1;
if(_TA!=_TB)goto _TL3E;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
_T17.f1=_tag_fat("fast_add_free_tvar: bad identity in tvs2",sizeof(char),41U);_TC=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TC;void*_T18[1];_T18[0]=& _T17;_TE=Cyc_Warn_impos2;{int(*_T19)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TE;_TD=_T19;}_TF=_tag_fat(_T18,sizeof(void*),1);_TD(_TF);}goto _TL3F;_TL3E: _TL3F: _T10=tv2;_T11=_T10->identity;_T12=tv;_T13=_T12->identity;
if(_T11!=_T13)goto _TL40;_T14=tvs;
return _T14;_TL40:;}_T15=tvs2;
# 208
tvs2=_T15->tl;goto _TL3D;_TL3C:;}{struct Cyc_List_List*_T17=_cycalloc(sizeof(struct Cyc_List_List));
# 215
_T17->hd=tv;_T17->tl=tvs;_T16=(struct Cyc_List_List*)_T17;}return _T16;}
# 221
static struct Cyc_List_List*Cyc_Tctyp_fast_add_free_tvar_bool(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){struct Cyc_Absyn_Tvar*_T0;int _T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;int(*_T4)(struct _fat_ptr);void*(*_T5)(struct _fat_ptr);struct _fat_ptr _T6;struct Cyc_List_List*_T7;void*_T8;int*_T9;struct Cyc_Absyn_Tvar*_TA;int _TB;int _TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;int(*_TE)(struct _fat_ptr);void*(*_TF)(struct _fat_ptr);struct _fat_ptr _T10;struct Cyc_Absyn_Tvar*_T11;int _T12;struct Cyc_Absyn_Tvar*_T13;int _T14;int*_T15;int _T16;int*_T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct _tuple12*_T1C;_T0=tv;_T1=_T0->identity;_T2=- 1;
if(_T1!=_T2)goto _TL42;{struct Cyc_Warn_String_Warn_Warg_struct _T1D;_T1D.tag=0;
_T1D.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tv",sizeof(char),44U);_T3=_T1D;}{struct Cyc_Warn_String_Warn_Warg_struct _T1D=_T3;void*_T1E[1];_T1E[0]=& _T1D;_T5=Cyc_Warn_impos2;{int(*_T1F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5;_T4=_T1F;}_T6=_tag_fat(_T1E,sizeof(void*),1);_T4(_T6);}goto _TL43;_TL42: _TL43:{
struct Cyc_List_List*tvs2=tvs;_TL47: if(tvs2!=0)goto _TL45;else{goto _TL46;}
_TL45: _T7=tvs2;_T8=_T7->hd;{struct _tuple12*_T1D=(struct _tuple12*)_T8;void*_T1E;struct Cyc_Absyn_Tvar*_T1F;{struct _tuple12 _T20=*_T1D;_T1F=_T20.f0;_T9=& _T1D->f1;_T1E=(int*)_T9;}{struct Cyc_Absyn_Tvar*tv2=_T1F;int*b2=(int*)_T1E;_TA=tv2;_TB=_TA->identity;_TC=- 1;
if(_TB!=_TC)goto _TL48;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
_T20.f1=_tag_fat("fast_add_free_tvar_bool: bad identity in tvs2",sizeof(char),46U);_TD=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_TD;void*_T21[1];_T21[0]=& _T20;_TF=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TF;_TE=_T22;}_T10=_tag_fat(_T21,sizeof(void*),1);_TE(_T10);}goto _TL49;_TL48: _TL49: _T11=tv2;_T12=_T11->identity;_T13=tv;_T14=_T13->identity;
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
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_check_type_inst(struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List**targsp,struct Cyc_List_List*tvs,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,struct _tuple1*typname,struct Cyc_Absyn_Typedefdecl*td){struct Cyc_List_List**_T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_Tctyp_CVTEnv _T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple0 _T6;struct Cyc_Absyn_Tvar*_T7;void*_T8;void*_T9;int*_TA;unsigned _TB;void*_TC;int*_TD;int _TE;void*_TF;struct Cyc_Tctyp_CVTEnv _T10;unsigned _T11;struct Cyc_Tctyp_CVTEnv _T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_Tvar*_T14;struct Cyc_Tctyp_CVTEnv _T15;int _T16;struct Cyc_Tctyp_CVTEnv _T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Tvar*_T19;void*_T1A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1B;struct Cyc_Absyn_Kind*_T1C;struct Cyc_Absyn_Kind*_T1D;enum Cyc_Absyn_KindQual _T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Tctyp_CVTEnv _T25;struct Cyc_Absyn_Kind*_T26;struct Cyc_Absyn_Kind*_T27;struct Cyc_List_List*_T28;void*_T29;int _T2A;struct Cyc_Absyn_Tvar*_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Tctyp_CVTEnv _T2E;unsigned _T2F;void*_T30;struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;struct Cyc_Warn_String_Warn_Warg_struct _T33;struct Cyc_Warn_Qvar_Warn_Warg_struct _T34;struct Cyc_Tctyp_CVTEnv _T35;unsigned _T36;struct _fat_ptr _T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;void*_T3A;struct Cyc_Absyn_Tvar*_T3B;struct Cyc_Absyn_Kind*_T3C;struct Cyc_Absyn_Kind*_T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List**_T3F;struct Cyc_List_List**_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;struct Cyc_Tctyp_CVTEnv _T43;_T0=targsp;{
# 304
struct Cyc_List_List*targs=*_T0;_T1=env;{
int old_inst_evar=_T1.inst_type_evar;_T2=env;
env.inst_type_evar=_T2.generalize_evars;
_TL52: if(targs!=0)goto _TL53;else{goto _TL51;}_TL53: if(tvs!=0)goto _TL50;else{goto _TL51;}
_TL50: _T3=targs;{void*t=_T3->hd;_T4=tvs;_T5=_T4->hd;{
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T5;{struct _tuple0 _T44;_T7=tv;_T8=_T7->kind;
# 312
_T44.f0=Cyc_Kinds_compress_kb(_T8);_T44.f1=t;_T6=_T44;}{struct _tuple0 _T44=_T6;struct Cyc_Absyn_Tvar*_T45;_T9=_T44.f0;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 1: _TC=_T44.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=2)goto _TL55;_TF=_T44.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TF;_T45=_T46->f1;}{struct Cyc_Absyn_Tvar*tv2=_T45;_T10=env;_T11=_T10.loc;_T12=env;_T13=_T12.kind_env;_T14=tv2;_T15=env;_T16=_T15.allow_aqual_bounds;
# 314
env.kind_env=Cyc_Tctyp_add_free_tvar(_T11,_T13,_T14,_T16);_T17=env;_T18=_T17.free_vars;_T19=tv2;
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T18,_T19,1);goto _TL4F;}_TL55: goto _LL5;case 0: _T1A=_T44.f0;_T1B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1A;_T1C=_T1B->f1;_T1D=(struct Cyc_Absyn_Kind*)_T1C;_T1E=_T1D->kind;if(_T1E!=Cyc_Absyn_AqualKind)goto _TL57;_T1F=_T44.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=2)goto _TL59;_T22=_T44.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T22;_T45=_T46->f1;}{struct Cyc_Absyn_Tvar*tv2=_T45;_T23=targs;_T24=
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
 if(targs==0)goto _TL5B;{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;
_T44.f1=_tag_fat("too many type arguments for ",sizeof(char),29U);_T33=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T33;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T45;_T45.tag=1;_T45.f1=typname;_T34=_T45;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T45=_T34;void*_T46[2];_T46[0]=& _T44;_T46[1]=& _T45;_T35=env;_T36=_T35.loc;_T37=_tag_fat(_T46,sizeof(void*),2);Cyc_Warn_err2(_T36,_T37);}}goto _TL5C;_TL5B: _TL5C: {
# 330
struct Cyc_List_List*hidden_ts=0;
_TL60: if(tvs!=0)goto _TL5E;else{goto _TL5F;}
_TL5E:{void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_T44=_cycalloc(sizeof(struct Cyc_List_List));
_T44->hd=e;_T44->tl=hidden_ts;_T38=(struct Cyc_List_List*)_T44;}hidden_ts=_T38;_T39=tvs;_T3A=_T39->hd;_T3B=(struct Cyc_Absyn_Tvar*)_T3A;_T3C=& Cyc_Kinds_bk;_T3D=(struct Cyc_Absyn_Kind*)_T3C;{
# 335
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind(_T3B,_T3D);
env=Cyc_Tctyp_i_check_type(env,k,e,1,allow_abs_aggr);}}_T3E=tvs;
# 331
tvs=_T3E->tl;goto _TL60;_TL5F: _T3F=targsp;_T40=targsp;_T41=*_T40;_T42=
# 338
Cyc_List_imp_rev(hidden_ts);*_T3F=Cyc_List_imp_append(_T41,_T42);
env.inst_type_evar=old_inst_evar;_T43=env;
return _T43;}}}}struct _tuple15{struct Cyc_Tctyp_CVTEnv f0;struct Cyc_AssnDef_AssnMap f1;};
# 344
static struct _tuple15 Cyc_Tctyp_check_clause(struct Cyc_Tctyp_CVTEnv env,struct _fat_ptr clause_name,struct Cyc_Absyn_Exp*clause){struct _tuple15 _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Absyn_Exp*_T3;int _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Tctyp_CVTEnv _TA;unsigned _TB;struct _fat_ptr _TC;struct _tuple15 _TD;
# 347
if(clause!=0)goto _TL61;{struct _tuple15 _TE;
_TE.f0=env;_TE.f1=Cyc_AssnDef_true_assnmap();_T0=_TE;}return _T0;_TL61: _T1=env;_T2=_T1.te;_T3=clause;
Cyc_Tcexp_tcExp(_T2,0,_T3);_T4=
Cyc_Tcutil_is_integral(clause);if(_T4)goto _TL63;else{goto _TL65;}
_TL65:{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;_TE.f1=clause_name;_T5=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T5;{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;_TF.f1=_tag_fat(" clause has type ",sizeof(char),18U);_T6=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_T6;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T10;_T10.tag=3;_T8=clause;_T10.f1=_T8->topt;_T7=_T10;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T10=_T7;{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;
_T11.f1=_tag_fat(" instead of integral type",sizeof(char),26U);_T9=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T9;void*_T12[4];_T12[0]=& _TE;_T12[1]=& _TF;_T12[2]=& _T10;_T12[3]=& _T11;_TA=env;_TB=_TA.loc;_TC=_tag_fat(_T12,sizeof(void*),4);Cyc_Warn_err2(_TB,_TC);}}}}goto _TL64;_TL63: _TL64:
 env=Cyc_Tctyp_i_check_type_level_exp(clause,env);{
struct Cyc_AssnDef_AssnMap assnmap=Cyc_Vcgen_clause2assn(clause);{struct _tuple15 _TE;
_TE.f0=env;_TE.f1=assnmap;_TD=_TE;}return _TD;}}
# 359
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_aggr(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;union Cyc_Absyn_AggrInfo*_T2;struct _union_AggrInfo_UnknownAggr _T3;unsigned _T4;struct _union_AggrInfo_UnknownAggr _T5;struct _tuple3 _T6;struct _union_AggrInfo_UnknownAggr _T7;struct _tuple3 _T8;struct _union_AggrInfo_UnknownAggr _T9;struct _tuple3 _TA;struct _handler_cons*_TB;int*_TC;int _TD;struct Cyc_Absyn_Aggrdecl**_TE;struct Cyc_Absyn_Aggrdecl*_TF;enum Cyc_Absyn_AggrKind _T10;int _T11;enum Cyc_Absyn_AggrKind _T12;int _T13;struct Cyc_Absyn_Aggrdecl*_T14;enum Cyc_Absyn_AggrKind _T15;int _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;struct Cyc_Warn_Qvar_Warn_Warg_struct _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1A;unsigned _T1B;struct _fat_ptr _T1C;struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;struct Cyc_Warn_Qvar_Warn_Warg_struct _T20;unsigned _T21;struct _fat_ptr _T22;struct Cyc_Core_Opt*_T23;unsigned _T24;struct Cyc_Core_Opt*_T25;void*_T26;int _T27;struct Cyc_Absyn_Aggrdecl*_T28;struct Cyc_Absyn_AggrdeclImpl*_T29;unsigned _T2A;struct Cyc_Absyn_Aggrdecl*_T2B;struct Cyc_Absyn_AggrdeclImpl*_T2C;int _T2D;struct Cyc_Warn_String_Warn_Warg_struct _T2E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T2F;unsigned _T30;struct _fat_ptr _T31;union Cyc_Absyn_AggrInfo*_T32;void*_T33;struct Cyc_Dict_Absent_exn_struct*_T34;char*_T35;char*_T36;struct Cyc_Tcenv_Tenv*_T37;unsigned _T38;struct Cyc_Absyn_Aggrdecl*_T39;union Cyc_Absyn_AggrInfo*_T3A;struct Cyc_List_List**_T3B;struct Cyc_List_List*_T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T3E;struct Cyc_Warn_String_Warn_Warg_struct _T3F;unsigned _T40;struct _fat_ptr _T41;struct Cyc_Tctyp_CVTEnv _T42;struct Cyc_Absyn_Aggrdecl**_T43;struct _union_AggrInfo_KnownAggr _T44;struct Cyc_Absyn_Aggrdecl**_T45;struct Cyc_Tctyp_CVTEnv _T46;struct Cyc_List_List**_T47;struct Cyc_Absyn_Aggrdecl*_T48;struct Cyc_List_List*_T49;struct Cyc_Absyn_Kind*_T4A;int _T4B;struct Cyc_Absyn_Aggrdecl*_T4C;struct _tuple1*_T4D;int _T4E;struct Cyc_Absyn_Aggrdecl*_T4F;struct Cyc_Absyn_AggrdeclImpl*_T50;struct Cyc_Absyn_Kind*_T51;struct Cyc_Absyn_Kind*_T52;struct Cyc_Absyn_Kind*_T53;int _T54;struct Cyc_Absyn_Aggrdecl*_T55;struct Cyc_Tctyp_CVTEnv _T56;struct Cyc_Tcenv_Tenv*_T57;unsigned _T58;_T0=env;_T58=_T0.loc;_T1=env;_T57=_T1.te;{unsigned loc=_T58;struct Cyc_Tcenv_Tenv*te=_T57;_T2=info;{
# 363
union Cyc_Absyn_AggrInfo _T59=*_T2;struct Cyc_Absyn_Aggrdecl*_T5A;struct Cyc_Core_Opt*_T5B;struct _tuple1*_T5C;enum Cyc_Absyn_AggrKind _T5D;_T3=_T59.UnknownAggr;_T4=_T3.tag;if(_T4!=1)goto _TL66;_T5=_T59.UnknownAggr;_T6=_T5.val;_T5D=_T6.f0;_T7=_T59.UnknownAggr;_T8=_T7.val;_T5C=_T8.f1;_T9=_T59.UnknownAggr;_TA=_T9.val;_T5B=_TA.f2;{enum Cyc_Absyn_AggrKind k=_T5D;struct _tuple1*n=_T5C;struct Cyc_Core_Opt*tgd=_T5B;
# 365
struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _T5E;_TB=& _T5E;_push_handler(_TB);{int _T5F=0;_TC=_T5E.handler;_TD=setjmp(_TC);if(!_TD)goto _TL68;_T5F=1;goto _TL69;_TL68: _TL69: if(_T5F)goto _TL6A;else{goto _TL6C;}_TL6C:
# 367
 adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);_TE=adp;{
struct Cyc_Absyn_Aggrdecl*ad=*_TE;_TF=ad;_T10=_TF->kind;_T11=(int)_T10;_T12=k;_T13=(int)_T12;
if(_T11==_T13)goto _TL6D;_T14=ad;_T15=_T14->kind;_T16=(int)_T15;
if(_T16!=0)goto _TL6F;{struct Cyc_Warn_String_Warn_Warg_struct _T60;_T60.tag=0;
_T60.f1=_tag_fat("expecting struct ",sizeof(char),18U);_T17=_T60;}{struct Cyc_Warn_String_Warn_Warg_struct _T60=_T17;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61;_T61.tag=1;_T61.f1=n;_T18=_T61;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61=_T18;{struct Cyc_Warn_String_Warn_Warg_struct _T62;_T62.tag=0;_T62.f1=_tag_fat("instead of union ",sizeof(char),18U);_T19=_T62;}{struct Cyc_Warn_String_Warn_Warg_struct _T62=_T19;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;_T63.tag=1;_T63.f1=n;_T1A=_T63;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63=_T1A;void*_T64[4];_T64[0]=& _T60;_T64[1]=& _T61;_T64[2]=& _T62;_T64[3]=& _T63;_T1B=loc;_T1C=_tag_fat(_T64,sizeof(void*),4);Cyc_Warn_err2(_T1B,_T1C);}}}}goto _TL70;
# 373
_TL6F:{struct Cyc_Warn_String_Warn_Warg_struct _T60;_T60.tag=0;_T60.f1=_tag_fat("expecting union ",sizeof(char),17U);_T1D=_T60;}{struct Cyc_Warn_String_Warn_Warg_struct _T60=_T1D;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61;_T61.tag=1;_T61.f1=n;_T1E=_T61;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61=_T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T62;_T62.tag=0;_T62.f1=_tag_fat("instead of struct ",sizeof(char),19U);_T1F=_T62;}{struct Cyc_Warn_String_Warn_Warg_struct _T62=_T1F;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;_T63.tag=1;_T63.f1=n;_T20=_T63;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63=_T20;void*_T64[4];_T64[0]=& _T60;_T64[1]=& _T61;_T64[2]=& _T62;_T64[3]=& _T63;_T21=loc;_T22=_tag_fat(_T64,sizeof(void*),4);Cyc_Warn_err2(_T21,_T22);}}}}_TL70: goto _TL6E;_TL6D: _TL6E: _T23=tgd;_T24=(unsigned)_T23;
# 375
if(!_T24)goto _TL71;_T25=tgd;_T26=_T25->v;_T27=(int)_T26;if(!_T27)goto _TL71;_T28=ad;_T29=_T28->impl;_T2A=(unsigned)_T29;if(_T2A)goto _TL74;else{goto _TL73;}_TL74: _T2B=ad;_T2C=_T2B->impl;_T2D=_T2C->tagged;if(_T2D)goto _TL71;else{goto _TL73;}
_TL73:{struct Cyc_Warn_String_Warn_Warg_struct _T60;_T60.tag=0;_T60.f1=_tag_fat("@tagged qualfiers don't agree on union ",sizeof(char),40U);_T2E=_T60;}{struct Cyc_Warn_String_Warn_Warg_struct _T60=_T2E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61;_T61.tag=1;_T61.f1=n;_T2F=_T61;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T61=_T2F;void*_T62[2];_T62[0]=& _T60;_T62[1]=& _T61;_T30=loc;_T31=_tag_fat(_T62,sizeof(void*),2);Cyc_Warn_err2(_T30,_T31);}}goto _TL72;_TL71: _TL72: _T32=info;
# 378
*_T32=Cyc_Absyn_KnownAggr(adp);}_pop_handler();goto _TL6B;_TL6A: _T33=Cyc_Core_get_exn_thrown();{void*_T60=(void*)_T33;void*_T61;_T34=(struct Cyc_Dict_Absent_exn_struct*)_T60;_T35=_T34->tag;_T36=Cyc_Dict_Absent;if(_T35!=_T36)goto _TL75;_T37=te;_T38=loc;{struct Cyc_Absyn_Aggrdecl*_T62=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));
# 382
_T62->kind=k;_T62->sc=3U;_T62->name=n;_T62->tvs=0;_T62->impl=0;_T62->attributes=0;_T62->expected_mem_kind=0;_T39=(struct Cyc_Absyn_Aggrdecl*)_T62;}Cyc_Tc_tcAggrdecl(_T37,_T38,_T39);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,n);_T3A=info;
*_T3A=Cyc_Absyn_KnownAggr(adp);_T3B=targs;_T3C=*_T3B;
# 386
if(_T3C==0)goto _TL77;{struct Cyc_Warn_String_Warn_Warg_struct _T62;_T62.tag=0;
_T62.f1=_tag_fat("declare parameterized type ",sizeof(char),28U);_T3D=_T62;}{struct Cyc_Warn_String_Warn_Warg_struct _T62=_T3D;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63;_T63.tag=1;_T63.f1=n;_T3E=_T63;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T63=_T3E;{struct Cyc_Warn_String_Warn_Warg_struct _T64;_T64.tag=0;_T64.f1=_tag_fat(" before using",sizeof(char),14U);_T3F=_T64;}{struct Cyc_Warn_String_Warn_Warg_struct _T64=_T3F;void*_T65[3];_T65[0]=& _T62;_T65[1]=& _T63;_T65[2]=& _T64;_T40=loc;_T41=_tag_fat(_T65,sizeof(void*),3);Cyc_Warn_err2(_T40,_T41);}}}_T42=env;
return _T42;_TL77: goto _LL8;_TL75: _T61=_T60;{void*exn=_T61;_rethrow(exn);}_LL8:;}_TL6B:;}}_T43=adp;_T5A=*_T43;goto _LL7;}_TL66: _T44=_T59.KnownAggr;_T45=_T44.val;{struct Cyc_Absyn_Aggrdecl*_T5E=*_T45;_T5A=_T5E;}_LL7:{struct Cyc_Absyn_Aggrdecl*ad=_T5A;_T46=env;_T47=targs;_T48=ad;_T49=_T48->tvs;_T4A=expected_kind;_T4B=allow_abs_aggr;_T4C=ad;_T4D=_T4C->name;
# 394
env=Cyc_Tctyp_check_type_inst(_T46,_T47,_T49,_T4A,_T4B,_T4D,0);_T4E=allow_abs_aggr;
# 396
if(!_T4E)goto _TL79;_T4F=ad;_T50=_T4F->impl;if(_T50!=0)goto _TL79;_T51=& Cyc_Kinds_ak;_T52=(struct Cyc_Absyn_Kind*)_T51;_T53=expected_kind;_T54=
Cyc_Kinds_kind_leq(_T52,_T53);
# 396
if(_T54)goto _TL79;else{goto _TL7B;}
# 401
_TL7B: _T55=ad;_T55->expected_mem_kind=1;goto _TL7A;_TL79: _TL7A: _T56=env;
return _T56;};}}}
# 407
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_List_List**_T2;union Cyc_Absyn_DatatypeInfo*_T3;struct _union_DatatypeInfo_UnknownDatatype _T4;unsigned _T5;struct _union_DatatypeInfo_UnknownDatatype _T6;struct Cyc_Absyn_UnknownDatatypeInfo _T7;struct _union_DatatypeInfo_UnknownDatatype _T8;struct Cyc_Absyn_UnknownDatatypeInfo _T9;struct _handler_cons*_TA;int*_TB;int _TC;void*_TD;struct Cyc_Dict_Absent_exn_struct*_TE;char*_TF;char*_T10;struct Cyc_Tcenv_Tenv*_T11;unsigned _T12;struct Cyc_Absyn_Datatypedecl*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Qvar_Warn_Warg_struct _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;unsigned _T17;struct _fat_ptr _T18;struct Cyc_Tctyp_CVTEnv _T19;int _T1A;struct Cyc_Absyn_Datatypedecl**_T1B;struct Cyc_Absyn_Datatypedecl*_T1C;int _T1D;struct Cyc_Warn_String_Warn_Warg_struct _T1E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1F;struct Cyc_Warn_String_Warn_Warg_struct _T20;unsigned _T21;struct _fat_ptr _T22;union Cyc_Absyn_DatatypeInfo*_T23;struct Cyc_Absyn_Datatypedecl**_T24;struct _union_DatatypeInfo_KnownDatatype _T25;struct Cyc_Absyn_Datatypedecl**_T26;struct Cyc_Tctyp_CVTEnv _T27;struct Cyc_List_List**_T28;struct Cyc_Absyn_Datatypedecl*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_Kind*_T2B;int _T2C;struct Cyc_Absyn_Datatypedecl*_T2D;struct _tuple1*_T2E;struct Cyc_Tctyp_CVTEnv _T2F;struct Cyc_Tcenv_Tenv*_T30;unsigned _T31;_T0=env;_T31=_T0.loc;_T1=env;_T30=_T1.te;{unsigned loc=_T31;struct Cyc_Tcenv_Tenv*te=_T30;_T2=targsp;{
# 411
struct Cyc_List_List*targs=*_T2;_T3=info;{
union Cyc_Absyn_DatatypeInfo _T32=*_T3;struct Cyc_Absyn_Datatypedecl*_T33;int _T34;struct _tuple1*_T35;_T4=_T32.UnknownDatatype;_T5=_T4.tag;if(_T5!=1)goto _TL7C;_T6=_T32.UnknownDatatype;_T7=_T6.val;_T35=_T7.name;_T8=_T32.UnknownDatatype;_T9=_T8.val;_T34=_T9.is_extensible;{struct _tuple1*n=_T35;int is_x=_T34;
# 414
struct Cyc_Absyn_Datatypedecl**tudp;{struct _handler_cons _T36;_TA=& _T36;_push_handler(_TA);{int _T37=0;_TB=_T36.handler;_TC=setjmp(_TB);if(!_TC)goto _TL7E;_T37=1;goto _TL7F;_TL7E: _TL7F: if(_T37)goto _TL80;else{goto _TL82;}_TL82:
 tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);_pop_handler();goto _TL81;_TL80: _TD=Cyc_Core_get_exn_thrown();{void*_T38=(void*)_TD;void*_T39;_TE=(struct Cyc_Dict_Absent_exn_struct*)_T38;_TF=_TE->tag;_T10=Cyc_Dict_Absent;if(_TF!=_T10)goto _TL83;_T11=te;_T12=loc;{struct Cyc_Absyn_Datatypedecl*_T3A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));
# 418
_T3A->sc=3U;_T3A->name=n;_T3A->tvs=0;_T3A->fields=0;_T3A->is_extensible=is_x;_T13=(struct Cyc_Absyn_Datatypedecl*)_T3A;}Cyc_Tc_tcDatatypedecl(_T11,_T12,_T13);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,n);
# 421
if(targs==0)goto _TL85;{struct Cyc_Warn_String_Warn_Warg_struct _T3A;_T3A.tag=0;
_T3A.f1=_tag_fat("declare parameterized datatype ",sizeof(char),32U);_T14=_T3A;}{struct Cyc_Warn_String_Warn_Warg_struct _T3A=_T14;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3B;_T3B.tag=1;_T3B.f1=n;_T15=_T3B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3B=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T3C;_T3C.tag=0;_T3C.f1=_tag_fat(" before using",sizeof(char),14U);_T16=_T3C;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C=_T16;void*_T3D[3];_T3D[0]=& _T3A;_T3D[1]=& _T3B;_T3D[2]=& _T3C;_T17=loc;_T18=_tag_fat(_T3D,sizeof(void*),3);Cyc_Warn_err2(_T17,_T18);}}}_T19=env;
return _T19;_TL85: goto _LL8;_TL83: _T39=_T38;{void*exn=_T39;_rethrow(exn);}_LL8:;}_TL81:;}}_T1A=is_x;
# 429
if(!_T1A)goto _TL87;_T1B=tudp;_T1C=*_T1B;_T1D=_T1C->is_extensible;if(_T1D)goto _TL87;else{goto _TL89;}
_TL89:{struct Cyc_Warn_String_Warn_Warg_struct _T36;_T36.tag=0;_T36.f1=_tag_fat("datatype ",sizeof(char),10U);_T1E=_T36;}{struct Cyc_Warn_String_Warn_Warg_struct _T36=_T1E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T37;_T37.tag=1;_T37.f1=n;_T1F=_T37;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T37=_T1F;{struct Cyc_Warn_String_Warn_Warg_struct _T38;_T38.tag=0;_T38.f1=_tag_fat(" was not declared @extensible",sizeof(char),30U);_T20=_T38;}{struct Cyc_Warn_String_Warn_Warg_struct _T38=_T20;void*_T39[3];_T39[0]=& _T36;_T39[1]=& _T37;_T39[2]=& _T38;_T21=loc;_T22=_tag_fat(_T39,sizeof(void*),3);Cyc_Warn_err2(_T21,_T22);}}}goto _TL88;_TL87: _TL88: _T23=info;
*_T23=Cyc_Absyn_KnownDatatype(tudp);_T24=tudp;_T33=*_T24;goto _LL7;}_TL7C: _T25=_T32.KnownDatatype;_T26=_T25.val;{struct Cyc_Absyn_Datatypedecl*_T36=*_T26;_T33=_T36;}_LL7:{struct Cyc_Absyn_Datatypedecl*tud=_T33;_T27=env;_T28=targsp;_T29=tud;_T2A=_T29->tvs;_T2B=expected_kind;_T2C=allow_abs_aggr;_T2D=tud;_T2E=_T2D->name;_T2F=
# 434
Cyc_Tctyp_check_type_inst(_T27,_T28,_T2A,_T2B,_T2C,_T2E,0);return _T2F;};}}}}
# 440
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_datatype_field(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){union Cyc_Absyn_DatatypeFieldInfo*_T0;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T1;unsigned _T2;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T3;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T4;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T5;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T7;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T8;struct Cyc_Tctyp_CVTEnv _T9;struct Cyc_Tcenv_Tenv*_TA;struct Cyc_Tctyp_CVTEnv _TB;unsigned _TC;struct _tuple1*_TD;struct Cyc_Absyn_Datatypedecl**_TE;struct Cyc_Absyn_Datatypedecl*_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Core_Opt*_T11;void*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Datatypefield*_T19;struct _tuple1*_T1A;struct _tuple1*_T1B;int _T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_List_List*_T1F;union Cyc_Absyn_DatatypeFieldInfo*_T20;struct _union_DatatypeFieldInfo_KnownDatatypefield _T21;struct _tuple2 _T22;struct _union_DatatypeFieldInfo_KnownDatatypefield _T23;struct _tuple2 _T24;struct Cyc_Tctyp_CVTEnv _T25;struct Cyc_List_List**_T26;struct Cyc_Absyn_Datatypedecl*_T27;struct Cyc_List_List*_T28;struct Cyc_Absyn_Kind*_T29;int _T2A;struct Cyc_Absyn_Datatypefield*_T2B;struct _tuple1*_T2C;struct Cyc_Tctyp_CVTEnv _T2D;_T0=info;{
# 443
union Cyc_Absyn_DatatypeFieldInfo _T2E=*_T0;struct Cyc_Absyn_Datatypefield*_T2F;struct Cyc_Absyn_Datatypedecl*_T30;int _T31;struct _tuple1*_T32;struct _tuple1*_T33;_T1=_T2E.UnknownDatatypefield;_T2=_T1.tag;if(_T2!=1)goto _TL8A;_T3=_T2E.UnknownDatatypefield;_T4=_T3.val;_T33=_T4.datatype_name;_T5=_T2E.UnknownDatatypefield;_T6=_T5.val;_T32=_T6.field_name;_T7=_T2E.UnknownDatatypefield;_T8=_T7.val;_T31=_T8.is_extensible;{struct _tuple1*tname=_T33;struct _tuple1*fname=_T32;int is_x=_T31;_T9=env;_TA=_T9.te;_TB=env;_TC=_TB.loc;_TD=tname;_TE=
# 446
Cyc_Tcenv_lookup_datatypedecl(_TA,_TC,_TD);{struct Cyc_Absyn_Datatypedecl*tud=*_TE;
struct Cyc_Absyn_Datatypefield*tuf;_TF=tud;_T10=_TF->fields;_T11=
# 451
_check_null(_T10);_T12=_T11->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T12;_TL8F: if(1)goto _TL8D;else{goto _TL8E;}
_TL8D: if(fs!=0)goto _TL90;{struct Cyc_Warn_String_Warn_Warg_struct _T34;_T34.tag=0;_T34.f1=_tag_fat("Tcutil found a bad datatypefield",sizeof(char),33U);_T13=_T34;}{struct Cyc_Warn_String_Warn_Warg_struct _T34=_T13;void*_T35[1];_T35[0]=& _T34;_T15=Cyc_Warn_impos2;{int(*_T36)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_T36;}_T16=_tag_fat(_T35,sizeof(void*),1);_T14(_T16);}goto _TL91;_TL90: _TL91: _T17=fs;_T18=_T17->hd;_T19=(struct Cyc_Absyn_Datatypefield*)_T18;_T1A=_T19->name;_T1B=fname;_T1C=
Cyc_Absyn_qvar_cmp(_T1A,_T1B);if(_T1C!=0)goto _TL92;_T1D=fs;_T1E=_T1D->hd;
tuf=(struct Cyc_Absyn_Datatypefield*)_T1E;goto _TL8E;_TL92: _T1F=fs;
# 451
fs=_T1F->tl;goto _TL8F;_TL8E:;}_T20=info;
# 458
*_T20=Cyc_Absyn_KnownDatatypefield(tud,tuf);_T30=tud;_T2F=tuf;goto _LL4;}}_TL8A: _T21=_T2E.KnownDatatypefield;_T22=_T21.val;_T30=_T22.f0;_T23=_T2E.KnownDatatypefield;_T24=_T23.val;_T2F=_T24.f1;_LL4:{struct Cyc_Absyn_Datatypedecl*tud=_T30;struct Cyc_Absyn_Datatypefield*tuf=_T2F;_T25=env;_T26=targsp;_T27=tud;_T28=_T27->tvs;_T29=expected_kind;_T2A=allow_abs_aggr;_T2B=tuf;_T2C=_T2B->name;_T2D=
# 461
Cyc_Tctyp_check_type_inst(_T25,_T26,_T28,_T29,_T2A,_T2C,0);return _T2D;};}}
# 466
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_unary_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*k,struct Cyc_List_List*ts,int put_in_effect,struct _fat_ptr s){int _T0;struct Cyc_Warn_String_Warn_Warg_struct _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;int(*_T3)(struct _fat_ptr);void*(*_T4)(struct _fat_ptr);struct _fat_ptr _T5;struct Cyc_Tctyp_CVTEnv _T6;struct Cyc_Absyn_Kind*_T7;struct Cyc_List_List*_T8;void*_T9;int _TA;struct Cyc_Tctyp_CVTEnv _TB;_T0=
# 468
Cyc_List_length(ts);if(_T0==1)goto _TL94;{struct Cyc_Warn_String_Warn_Warg_struct _TC;_TC.tag=0;
_TC.f1=s;_T1=_TC;}{struct Cyc_Warn_String_Warn_Warg_struct _TC=_T1;{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat(" has wrong number of arguments",sizeof(char),31U);_T2=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T2;void*_TE[2];_TE[0]=& _TC;_TE[1]=& _TD;_T4=Cyc_Warn_impos2;{int(*_TF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T4;_T3=_TF;}_T5=_tag_fat(_TE,sizeof(void*),2);_T3(_T5);}}goto _TL95;_TL94: _TL95: _T6=env;_T7=k;_T8=
_check_null(ts);_T9=_T8->hd;_TA=put_in_effect;_TB=Cyc_Tctyp_i_check_type(_T6,_T7,_T9,_TA,1);return _TB;}
# 473
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type_app(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr,void*apptype){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;struct Cyc_List_List**_T2;void*_T3;int*_T4;unsigned _T5;struct Cyc_Warn_Typ_Warn_Warg_struct _T6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;int(*_T9)(struct _fat_ptr);void*(*_TA)(struct _fat_ptr);struct _fat_ptr _TB;struct Cyc_Tctyp_CVTEnv _TC;struct Cyc_Warn_Typ_Warn_Warg_struct _TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;int(*_T10)(struct _fat_ptr);void*(*_T11)(struct _fat_ptr);struct _fat_ptr _T12;struct Cyc_Absyn_Kind*_T13;struct Cyc_Absyn_Kind*_T14;struct Cyc_Absyn_Kind*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_Kind_Warn_Warg_struct _T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;unsigned _T19;struct _fat_ptr _T1A;struct Cyc_Tctyp_CVTEnv _T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_Warn_String_Warn_Warg_struct _T1E;int(*_T1F)(struct _fat_ptr);void*(*_T20)(struct _fat_ptr);struct _fat_ptr _T21;struct Cyc_Tctyp_CVTEnv _T22;struct Cyc_Absyn_Kind*_T23;struct Cyc_Absyn_Kind*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_Tctyp_CVTEnv _T27;int _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;int(*_T2A)(struct _fat_ptr);void*(*_T2B)(struct _fat_ptr);struct _fat_ptr _T2C;struct Cyc_Tctyp_CVTEnv _T2D;struct Cyc_Absyn_Kind*_T2E;struct Cyc_Absyn_Kind*_T2F;struct Cyc_List_List*_T30;void*_T31;struct Cyc_Tctyp_CVTEnv _T32;struct Cyc_Tcenv_Tenv*_T33;struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;void*_T36;void*_T37;unsigned _T38;struct Cyc_List_List*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;void*_T3C;struct Cyc_Tctyp_CVTEnv _T3D;struct Cyc_Absyn_Kind*_T3E;struct Cyc_Absyn_Kind*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;void*_T43;struct Cyc_List_List*_T44;struct _tuple14*_T45;struct Cyc_Tctyp_CVTEnv _T46;struct Cyc_Tctyp_CVTEnv _T47;struct Cyc_Tctyp_CVTEnv _T48;struct Cyc_Absyn_Kind*_T49;struct Cyc_Absyn_Kind*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct Cyc_Tctyp_CVTEnv _T4E;struct Cyc_Tctyp_CVTEnv _T4F;struct Cyc_Absyn_Kind*_T50;struct Cyc_Absyn_Kind*_T51;struct Cyc_List_List*_T52;void*_T53;struct Cyc_List_List*_T54;struct Cyc_Tctyp_CVTEnv _T55;void*_T56;void*_T57;struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_T58;struct Cyc_Absyn_Enumdecl**_T59;struct Cyc_Warn_String_Warn_Warg_struct _T5A;int(*_T5B)(struct _fat_ptr);void*(*_T5C)(struct _fat_ptr);struct _fat_ptr _T5D;struct Cyc_Absyn_Enumdecl**_T5E;struct Cyc_Absyn_Enumdecl*_T5F;struct Cyc_Absyn_Enumdecl**_T60;struct Cyc_Absyn_Enumdecl*_T61;struct Cyc_Core_Opt*_T62;struct _handler_cons*_T63;int*_T64;int _T65;struct Cyc_Absyn_Enumdecl**_T66;struct Cyc_Absyn_Enumdecl**_T67;void*_T68;struct Cyc_Dict_Absent_exn_struct*_T69;char*_T6A;char*_T6B;struct Cyc_Tcenv_Tenv*_T6C;unsigned _T6D;struct Cyc_Absyn_Enumdecl*_T6E;struct Cyc_Absyn_Enumdecl**_T6F;struct Cyc_Absyn_Enumdecl**_T70;struct Cyc_Tctyp_CVTEnv _T71;void*_T72;struct Cyc_Warn_String_Warn_Warg_struct _T73;int(*_T74)(struct _fat_ptr);void*(*_T75)(struct _fat_ptr);struct _fat_ptr _T76;struct Cyc_List_List*_T77;void*_T78;int(*_T79)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T7A)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T7B)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T7C;struct Cyc_Absyn_Enumfield*_T7D;struct _tuple1*_T7E;struct _tuple1 _T7F;struct _fat_ptr*_T80;int _T81;struct Cyc_Warn_String_Warn_Warg_struct _T82;struct Cyc_Warn_Qvar_Warn_Warg_struct _T83;struct Cyc_Absyn_Enumfield*_T84;struct Cyc_Absyn_Enumfield*_T85;unsigned _T86;struct _fat_ptr _T87;struct Cyc_List_List*_T88;struct Cyc_Absyn_Enumfield*_T89;struct _tuple1*_T8A;struct _tuple1 _T8B;struct Cyc_Absyn_Enumfield*_T8C;struct Cyc_Absyn_Exp*_T8D;struct Cyc_Absyn_Enumfield*_T8E;unsigned _T8F;struct Cyc_Absyn_Enumfield*_T90;unsigned _T91;struct Cyc_Absyn_Enumfield*_T92;struct Cyc_Absyn_Exp*_T93;int _T94;struct Cyc_Warn_String_Warn_Warg_struct _T95;struct Cyc_Warn_Qvar_Warn_Warg_struct _T96;struct Cyc_Absyn_Enumfield*_T97;struct Cyc_Warn_String_Warn_Warg_struct _T98;unsigned _T99;struct _fat_ptr _T9A;struct Cyc_Absyn_Enumfield*_T9B;struct Cyc_Absyn_Exp*_T9C;struct Cyc_Absyn_Exp*_T9D;struct _tuple13 _T9E;unsigned _T9F;struct Cyc_List_List*_TA0;struct Cyc_Tctyp_CVTEnv _TA1;struct Cyc_Tctyp_CVTEnv _TA2;struct Cyc_Absyn_Kind*_TA3;struct Cyc_Absyn_Kind*_TA4;struct Cyc_List_List*_TA5;struct _fat_ptr _TA6;struct Cyc_Tctyp_CVTEnv _TA7;struct Cyc_Tctyp_CVTEnv _TA8;struct Cyc_Absyn_Kind*_TA9;struct Cyc_Absyn_Kind*_TAA;struct Cyc_List_List*_TAB;struct _fat_ptr _TAC;struct Cyc_Tctyp_CVTEnv _TAD;struct Cyc_Tctyp_CVTEnv _TAE;struct Cyc_Absyn_Kind*_TAF;struct Cyc_Absyn_Kind*_TB0;struct Cyc_List_List*_TB1;struct _fat_ptr _TB2;struct Cyc_Tctyp_CVTEnv _TB3;struct Cyc_Tctyp_CVTEnv _TB4;struct Cyc_Absyn_Kind*_TB5;struct Cyc_Absyn_Kind*_TB6;struct Cyc_List_List*_TB7;struct _fat_ptr _TB8;struct Cyc_Tctyp_CVTEnv _TB9;struct Cyc_Tctyp_CVTEnv _TBA;struct Cyc_Absyn_Kind*_TBB;struct Cyc_Absyn_Kind*_TBC;struct Cyc_List_List*_TBD;struct _fat_ptr _TBE;struct Cyc_Tctyp_CVTEnv _TBF;void*_TC0;void*_TC1;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TC2;union Cyc_Absyn_AggrInfo*_TC3;struct Cyc_Tctyp_CVTEnv _TC4;void*_TC5;void*_TC6;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TC7;union Cyc_Absyn_DatatypeInfo*_TC8;struct Cyc_Tctyp_CVTEnv _TC9;void*_TCA;void*_TCB;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TCC;union Cyc_Absyn_DatatypeFieldInfo*_TCD;struct Cyc_Tctyp_CVTEnv _TCE;struct Cyc_Tcenv_Tenv*_TCF;unsigned _TD0;_T0=env;_TD0=_T0.loc;_T1=env;_TCF=_T1.te;{unsigned loc=_TD0;struct Cyc_Tcenv_Tenv*te=_TCF;_T2=targsp;{
# 477
struct Cyc_List_List*ts=*_T2;void*_TD1;void*_TD2;_T3=c;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 1: goto _LL7;case 2: _LL7: goto _LL9;case 0: _LL9: goto _LLB;case 8: _LLB: goto _LLD;case 7: _LLD: goto _LLF;case 6: _LLF: goto _LL11;case 12: _LL11: goto _LL13;case 11: _LL13: goto _LL15;case 14: _LL15: goto _LL17;case 21: _LL17:
# 484
 if(ts==0)goto _TL97;{struct Cyc_Warn_Typ_Warn_Warg_struct _TD3;_TD3.tag=2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD4=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_TD4->tag=0;
_TD4->f1=c;_TD4->f2=ts;_T7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD4;}_TD3.f1=(void*)_T7;_T6=_TD3;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TD3=_T6;{struct Cyc_Warn_String_Warn_Warg_struct _TD4;_TD4.tag=0;_TD4.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_T8=_TD4;}{struct Cyc_Warn_String_Warn_Warg_struct _TD4=_T8;void*_TD5[2];_TD5[0]=& _TD3;_TD5[1]=& _TD4;_TA=Cyc_Warn_impos2;{int(*_TD6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TA;_T9=_TD6;}_TB=_tag_fat(_TD5,sizeof(void*),2);_T9(_TB);}}goto _TL98;_TL97: _TL98: _TC=env;
return _TC;case 16:
# 488
 if(ts==0)goto _TL99;{struct Cyc_Warn_Typ_Warn_Warg_struct _TD3;_TD3.tag=2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD4=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_TD4->tag=0;
_TD4->f1=c;_TD4->f2=ts;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD4;}_TD3.f1=(void*)_TE;_TD=_TD3;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TD3=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _TD4;_TD4.tag=0;_TD4.f1=_tag_fat(" applied to argument(s)",sizeof(char),24U);_TF=_TD4;}{struct Cyc_Warn_String_Warn_Warg_struct _TD4=_TF;void*_TD5[2];_TD5[0]=& _TD3;_TD5[1]=& _TD4;_T11=Cyc_Warn_impos2;{int(*_TD6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T11;_T10=_TD6;}_T12=_tag_fat(_TD5,sizeof(void*),2);_T10(_T12);}}goto _TL9A;_TL99: _TL9A: _T13=expected_kind;_T14=& Cyc_Kinds_aqk;_T15=(struct Cyc_Absyn_Kind*)_T14;
if(_T13==_T15)goto _TL9B;{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;
_TD3.f1=_tag_fat("Kind Q used where kind ",sizeof(char),24U);_T16=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T16;{struct Cyc_Warn_Kind_Warn_Warg_struct _TD4;_TD4.tag=9;_TD4.f1=expected_kind;_T17=_TD4;}{struct Cyc_Warn_Kind_Warn_Warg_struct _TD4=_T17;{struct Cyc_Warn_String_Warn_Warg_struct _TD5;_TD5.tag=0;_TD5.f1=_tag_fat(" was expected",sizeof(char),14U);_T18=_TD5;}{struct Cyc_Warn_String_Warn_Warg_struct _TD5=_T18;void*_TD6[3];_TD6[0]=& _TD3;_TD6[1]=& _TD4;_TD6[2]=& _TD5;_T19=loc;_T1A=_tag_fat(_TD6,sizeof(void*),3);Cyc_Warn_err2(_T19,_T1A);}}}goto _TL9C;_TL9B: _TL9C: _T1B=env;
return _T1B;case 15:
# 494
 if(ts==0)goto _TL9F;else{goto _TLA0;}_TLA0: _T1C=ts;_T1D=_T1C->tl;if(_T1D!=0)goto _TL9F;else{goto _TL9D;}
_TL9F:{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;_TD3.f1=_tag_fat("aquals expects a single argument",sizeof(char),33U);_T1E=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T1E;void*_TD4[1];_TD4[0]=& _TD3;_T20=Cyc_Warn_impos2;{int(*_TD5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T20;_T1F=_TD5;}_T21=_tag_fat(_TD4,sizeof(void*),1);_T1F(_T21);}goto _TL9E;_TL9D: _TL9E: _T22=env;_T23=& Cyc_Kinds_ak;_T24=(struct Cyc_Absyn_Kind*)_T23;_T25=ts;_T26=_T25->hd;_T27=
Cyc_Tctyp_i_check_type(_T22,_T24,_T26,0,0);return _T27;case 17: _T28=
# 498
Cyc_List_length(ts);if(_T28==2)goto _TLA1;{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;
_TD3.f1=_tag_fat("aquals expects two arguments",sizeof(char),29U);_T29=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T29;void*_TD4[1];_TD4[0]=& _TD3;_T2B=Cyc_Warn_impos2;{int(*_TD5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2B;_T2A=_TD5;}_T2C=_tag_fat(_TD4,sizeof(void*),1);_T2A(_T2C);}goto _TLA2;_TLA1: _TLA2: _T2D=env;_T2E=& Cyc_Kinds_aqk;_T2F=(struct Cyc_Absyn_Kind*)_T2E;_T30=
_check_null(ts);_T31=_T30->hd;env=Cyc_Tctyp_i_check_type(_T2D,_T2F,_T31,0,0);_T32=env;_T33=_T32.te;_T34=
Cyc_Tcenv_curr_aquals_bounds(_T33);_T35=ts;_T36=_T35->hd;{void*ctx_bnd=Cyc_Tcutil_lookup_aquals_opt(_T34,_T36);_T37=ctx_bnd;_T38=(unsigned)_T37;
if(!_T38)goto _TLA3;_T39=ts;_T3A=_T39->tl;_T3B=
_check_null(_T3A);_T3C=ctx_bnd;_T3B->hd=(void*)_T3C;goto _TLA4;
# 506
_TLA3: _T3D=env;_T3E=& Cyc_Kinds_aqk;_T3F=(struct Cyc_Absyn_Kind*)_T3E;_T40=ts;_T41=_T40->tl;_T42=_check_null(_T41);_T43=_T42->hd;env=Cyc_Tctyp_i_check_type(_T3D,_T3F,_T43,0,0);_TLA4:{struct Cyc_List_List*_TD3=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple14*_TD4=_cycalloc(sizeof(struct _tuple14));
# 508
_TD4->f0=apptype;_TD4->f1=0;_T45=(struct _tuple14*)_TD4;}_TD3->hd=_T45;_T46=env;_TD3->tl=_T46.free_qualvars;_T44=(struct Cyc_List_List*)_TD3;}env.free_qualvars=_T44;_T47=env;
return _T47;}case 3:
# 511
 _TLA8: if(ts!=0)goto _TLA6;else{goto _TLA7;}
_TLA6: _T48=env;_T49=& Cyc_Kinds_mk;_T4A=(struct Cyc_Absyn_Kind*)_T49;_T4B=ts;_T4C=_T4B->hd;env=Cyc_Tctyp_i_check_type(_T48,_T4A,_T4C,1,1);_T4D=ts;
# 511
ts=_T4D->tl;goto _TLA8;_TLA7: _T4E=env;
# 513
return _T4E;case 9:
# 515
 _TLAC: if(ts!=0)goto _TLAA;else{goto _TLAB;}
_TLAA: _T4F=env;_T50=& Cyc_Kinds_ek;_T51=(struct Cyc_Absyn_Kind*)_T50;_T52=ts;_T53=_T52->hd;env=Cyc_Tctyp_i_check_type(_T4F,_T51,_T53,1,1);_T54=ts;
# 515
ts=_T54->tl;goto _TLAC;_TLAB: _T55=env;
# 517
return _T55;case 19: _T56=c;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TD3=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T56;_TD2=_TD3->f1;_T57=c;_T58=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T57;_T59=& _T58->f2;_TD1=(struct Cyc_Absyn_Enumdecl**)_T59;}{struct _tuple1*n=_TD2;struct Cyc_Absyn_Enumdecl**edp=(struct Cyc_Absyn_Enumdecl**)_TD1;
# 519
if(ts==0)goto _TLAD;{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;_TD3.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_T5A=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T5A;void*_TD4[1];_TD4[0]=& _TD3;_T5C=Cyc_Warn_impos2;{int(*_TD5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5C;_T5B=_TD5;}_T5D=_tag_fat(_TD4,sizeof(void*),1);_T5B(_T5D);}goto _TLAE;_TLAD: _TLAE: _T5E=edp;_T5F=*_T5E;
if(_T5F==0)goto _TLB1;else{goto _TLB2;}_TLB2: _T60=edp;_T61=*_T60;_T62=_T61->fields;if(_T62==0)goto _TLB1;else{goto _TLAF;}
_TLB1:{struct _handler_cons _TD3;_T63=& _TD3;_push_handler(_T63);{int _TD4=0;_T64=_TD3.handler;_T65=setjmp(_T64);if(!_T65)goto _TLB3;_TD4=1;goto _TLB4;_TLB3: _TLB4: if(_TD4)goto _TLB5;else{goto _TLB7;}_TLB7: _T66=edp;_T67=Cyc_Tcenv_lookup_enumdecl(te,loc,n);*_T66=*_T67;_pop_handler();goto _TLB6;_TLB5: _T68=Cyc_Core_get_exn_thrown();{void*_TD5=(void*)_T68;void*_TD6;_T69=(struct Cyc_Dict_Absent_exn_struct*)_TD5;_T6A=_T69->tag;_T6B=Cyc_Dict_Absent;if(_T6A!=_T6B)goto _TLB8;_T6C=te;_T6D=loc;{struct Cyc_Absyn_Enumdecl*_TD7=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
# 524
_TD7->sc=3U;_TD7->name=n;_TD7->fields=0;_T6E=(struct Cyc_Absyn_Enumdecl*)_TD7;}Cyc_Tc_tcEnumdecl(_T6C,_T6D,_T6E);_T6F=edp;_T70=
Cyc_Tcenv_lookup_enumdecl(te,loc,n);*_T6F=*_T70;goto _LL36;_TLB8: _TD6=_TD5;{void*exn=_TD6;_rethrow(exn);}_LL36:;}_TLB6:;}}goto _TLB0;_TLAF: _TLB0: _T71=env;
# 529
return _T71;}case 20: _T72=c;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_TD3=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T72;_TD2=_TD3->f1;}{struct Cyc_List_List*fs=_TD2;
# 531
if(ts==0)goto _TLBA;{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;
_TD3.f1=_tag_fat("enum applied to argument(s)",sizeof(char),28U);_T73=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T73;void*_TD4[1];_TD4[0]=& _TD3;_T75=Cyc_Warn_impos2;{int(*_TD5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T75;_T74=_TD5;}_T76=_tag_fat(_TD4,sizeof(void*),1);_T74(_T76);}goto _TLBB;_TLBA: _TLBB: {
# 534
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
_TLBF: if(fs!=0)goto _TLBD;else{goto _TLBE;}
_TLBD: _T77=fs;_T78=_T77->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T78;_T7A=Cyc_List_mem;{
int(*_TD3)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T7A;_T79=_TD3;}_T7B=Cyc_strptrcmp;_T7C=prev_fields;_T7D=f;_T7E=_T7D->name;_T7F=*_T7E;_T80=_T7F.f1;_T81=_T79(_T7B,_T7C,_T80);if(!_T81)goto _TLC0;{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;
_TD3.f1=_tag_fat("duplicate enum field name ",sizeof(char),27U);_T82=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T82;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TD4;_TD4.tag=1;_T84=f;_TD4.f1=_T84->name;_T83=_TD4;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TD4=_T83;void*_TD5[2];_TD5[0]=& _TD3;_TD5[1]=& _TD4;_T85=f;_T86=_T85->loc;_T87=_tag_fat(_TD5,sizeof(void*),2);Cyc_Warn_err2(_T86,_T87);}}goto _TLC1;
# 541
_TLC0:{struct Cyc_List_List*_TD3=_cycalloc(sizeof(struct Cyc_List_List));_T89=f;_T8A=_T89->name;_T8B=*_T8A;_TD3->hd=_T8B.f1;_TD3->tl=prev_fields;_T88=(struct Cyc_List_List*)_TD3;}prev_fields=_T88;_TLC1: _T8C=f;_T8D=_T8C->tag;
if(_T8D!=0)goto _TLC2;_T8E=f;_T8F=tag_count;_T90=f;_T91=_T90->loc;
_T8E->tag=Cyc_Absyn_uint_exp(_T8F,_T91);goto _TLC3;
_TLC2: _T92=f;_T93=_T92->tag;_T94=Cyc_Tcutil_is_const_exp(_T93);if(_T94)goto _TLC4;else{goto _TLC6;}
_TLC6:{struct Cyc_Warn_String_Warn_Warg_struct _TD3;_TD3.tag=0;_TD3.f1=_tag_fat("enum field ",sizeof(char),12U);_T95=_TD3;}{struct Cyc_Warn_String_Warn_Warg_struct _TD3=_T95;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TD4;_TD4.tag=1;_T97=f;_TD4.f1=_T97->name;_T96=_TD4;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TD4=_T96;{struct Cyc_Warn_String_Warn_Warg_struct _TD5;_TD5.tag=0;_TD5.f1=_tag_fat(": expression is not constant",sizeof(char),29U);_T98=_TD5;}{struct Cyc_Warn_String_Warn_Warg_struct _TD5=_T98;void*_TD6[3];_TD6[0]=& _TD3;_TD6[1]=& _TD4;_TD6[2]=& _TD5;_T99=loc;_T9A=_tag_fat(_TD6,sizeof(void*),3);Cyc_Warn_err2(_T99,_T9A);}}}goto _TLC5;_TLC4: _TLC5: _TLC3: _T9B=f;_T9C=_T9B->tag;_T9D=
_check_null(_T9C);_T9E=Cyc_Evexp_eval_const_uint_exp(_T9D);_T9F=_T9E.f0;tag_count=1U + _T9F;}_TA0=fs;
# 536
fs=_TA0->tl;goto _TLBF;_TLBE: _TA1=env;
# 548
return _TA1;}}case 10: _TA2=env;_TA3=& Cyc_Kinds_ak;_TA4=(struct Cyc_Absyn_Kind*)_TA3;_TA5=ts;_TA6=
_tag_fat("regions",sizeof(char),8U);_TA7=Cyc_Tctyp_i_check_unary_app(_TA2,_TA4,_TA5,1,_TA6);return _TA7;case 4: _TA8=env;_TA9=& Cyc_Kinds_ek;_TAA=(struct Cyc_Absyn_Kind*)_TA9;_TAB=ts;_TAC=
_tag_fat("region_t",sizeof(char),9U);_TAD=Cyc_Tctyp_i_check_unary_app(_TA8,_TAA,_TAB,1,_TAC);return _TAD;case 18: _TAE=env;_TAF=& Cyc_Kinds_aqk;_TB0=(struct Cyc_Absyn_Kind*)_TAF;_TB1=ts;_TB2=
# 552
_tag_fat("aqual_t",sizeof(char),8U);_TB3=Cyc_Tctyp_i_check_unary_app(_TAE,_TB0,_TB1,1,_TB2);return _TB3;case 5: _TB4=env;_TB5=& Cyc_Kinds_ik;_TB6=(struct Cyc_Absyn_Kind*)_TB5;_TB7=ts;_TB8=
# 554
_tag_fat("tag_t",sizeof(char),6U);_TB9=Cyc_Tctyp_i_check_unary_app(_TB4,_TB6,_TB7,0,_TB8);return _TB9;case 13: _TBA=env;_TBB=& Cyc_Kinds_ik;_TBC=(struct Cyc_Absyn_Kind*)_TBB;_TBD=ts;_TBE=
_tag_fat("@thin",sizeof(char),6U);_TBF=Cyc_Tctyp_i_check_unary_app(_TBA,_TBC,_TBD,0,_TBE);return _TBF;case 24: _TC0=c;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TD3=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC0;_TC1=c;_TC2=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC1;_TC3=& _TC2->f1;_TD2=(union Cyc_Absyn_AggrInfo*)_TC3;}{union Cyc_Absyn_AggrInfo*info=_TD2;_TC4=
# 557
Cyc_Tctyp_i_check_aggr(env,expected_kind,info,targsp,allow_abs_aggr);return _TC4;}case 22: _TC5=c;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD3=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC5;_TC6=c;_TC7=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC6;_TC8=& _TC7->f1;_TD2=(union Cyc_Absyn_DatatypeInfo*)_TC8;}{union Cyc_Absyn_DatatypeInfo*info=_TD2;_TC9=
# 559
Cyc_Tctyp_i_check_datatype(env,expected_kind,info,targsp,allow_abs_aggr);return _TC9;}default: _TCA=c;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TD3=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TCA;_TCB=c;_TCC=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TCB;_TCD=& _TCC->f1;_TD2=(union Cyc_Absyn_DatatypeFieldInfo*)_TCD;}{union Cyc_Absyn_DatatypeFieldInfo*info=(union Cyc_Absyn_DatatypeFieldInfo*)_TD2;_TCE=
# 561
Cyc_Tctyp_i_check_datatype_field(env,expected_kind,info,targsp,allow_abs_aggr);return _TCE;}};}}}
# 565
static int Cyc_Tctyp_bnd_qual_cmp(void*tvt1,void*tvt2){struct _tuple0 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;int _T9;void*_TA;int*_TB;unsigned _TC;void*_TD;void*_TE;int _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Core_Opt*_T13;void*_T14;struct Cyc_Absyn_Kind*_T15;int _T16;void*_T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;struct Cyc_List_List*_T1E;void*_T1F;int*_T20;unsigned _T21;void*_T22;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T23;void*_T24;int*_T25;int _T26;void*_T27;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_List_List*_T2B;void*_T2C;void*_T2D;struct Cyc_List_List*_T2E;void*_T2F;int _T30;void*_T31;int*_T32;unsigned _T33;void*_T34;int*_T35;unsigned _T36;struct Cyc_Warn_String_Warn_Warg_struct _T37;struct Cyc_Warn_Typ_Warn_Warg_struct _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;struct Cyc_Warn_Typ_Warn_Warg_struct _T3A;int(*_T3B)(struct _fat_ptr);void*(*_T3C)(struct _fat_ptr);struct _fat_ptr _T3D;{struct _tuple0 _T3E;
_T3E.f0=Cyc_Absyn_compress(tvt1);_T3E.f1=Cyc_Absyn_compress(tvt2);_T0=_T3E;}{struct _tuple0 _T3E=_T0;int _T3F;int _T40;void*_T41;void*_T42;_T1=_T3E.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 2: _T4=_T3E.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 2: _T7=_T3E.f0;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T7;_T42=_T43->f1;}_T8=_T3E.f1;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T8;_T41=_T43->f1;}{struct Cyc_Absyn_Tvar*tv1=_T42;struct Cyc_Absyn_Tvar*tv2=_T41;_T9=
# 568
Cyc_Absyn_tvar_cmp(tv1,tv2);return _T9;}case 1: goto _LLA;case 0: _LL10: goto _LL12;default: goto _LL13;};case 1: _TA=_T3E.f1;_TB=(int*)_TA;_TC=*_TB;switch(_TC){case 1: _TD=_T3E.f0;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TD;_T42=_T43->f1;_T40=_T43->f3;}_TE=_T3E.f1;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_TE;_T41=_T43->f1;_T3F=_T43->f3;}{struct Cyc_Core_Opt*k1=_T42;int i1=_T40;struct Cyc_Core_Opt*k2=_T41;int i2=_T3F;
# 570
if(i1!=i2)goto _TLCA;_T10=_check_null(k1);_T11=_T10->v;_T12=(struct Cyc_Absyn_Kind*)_T11;_T13=_check_null(k2);_T14=_T13->v;_T15=(struct Cyc_Absyn_Kind*)_T14;_T16=Cyc_Kinds_kind_eq(_T12,_T15);if(!_T16)goto _TLCA;_TF=0;goto _TLCB;_TLCA: _TF=1;_TLCB: return _TF;}case 2: _LLA: goto _LLC;case 0: _LLC: goto _LLE;default: goto _LL13;};case 0: _T17=_T3E.f0;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=15)goto _TLCC;_T1C=_T3E.f0;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f2;if(_T1E==0)goto _TLCE;_T1F=_T3E.f1;_T20=(int*)_T1F;_T21=*_T20;switch(_T21){case 0: _T22=_T3E.f1;_T23=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T24=_T23->f1;_T25=(int*)_T24;_T26=*_T25;if(_T26!=15)goto _TLD1;_T27=_T3E.f1;_T28=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27;_T29=_T28->f2;if(_T29==0)goto _TLD3;_T2A=_T3E.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2A;_T2B=_T43->f2;{struct Cyc_List_List _T44=*_T2B;_T2C=_T44.hd;_T42=(void*)_T2C;}}_T2D=_T3E.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T2E=_T43->f2;{struct Cyc_List_List _T44=*_T2E;_T2F=_T44.hd;_T41=(void*)_T2F;}}{void*tv11=_T42;void*tv21=_T41;_T30=
# 572
Cyc_Tctyp_bnd_qual_cmp(tv11,tv21);return _T30;}_TLD3: goto _LL13;_TLD1: goto _LL13;case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}goto _TLCF;_TLCE: _T31=_T3E.f1;_T32=(int*)_T31;_T33=*_T32;switch(_T33){case 1: goto _LLD;case 2: goto _LL11;default: goto _LL13;}_TLCF: goto _TLCD;_TLCC: _T34=_T3E.f1;_T35=(int*)_T34;_T36=*_T35;switch(_T36){case 1: _LLD: _LLE: goto _LL10;case 2: _LL11: _LL12:
# 579
 return 1;default: goto _LL13;}_TLCD:;default: _LL13:{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;
_T43.f1=_tag_fat("Expect only tvars, evars or aquals in qualifier bounds: ",sizeof(char),57U);_T37=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T37;{struct Cyc_Warn_Typ_Warn_Warg_struct _T44;_T44.tag=2;_T44.f1=tvt1;_T38=_T44;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T44=_T38;{struct Cyc_Warn_String_Warn_Warg_struct _T45;_T45.tag=0;_T45.f1=_tag_fat(",",sizeof(char),2U);_T39=_T45;}{struct Cyc_Warn_String_Warn_Warg_struct _T45=_T39;{struct Cyc_Warn_Typ_Warn_Warg_struct _T46;_T46.tag=2;_T46.f1=tvt2;_T3A=_T46;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T46=_T3A;void*_T47[4];_T47[0]=& _T43;_T47[1]=& _T44;_T47[2]=& _T45;_T47[3]=& _T46;_T3C=Cyc_Warn_impos2;{int(*_T48)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3C;_T3B=_T48;}_T3D=_tag_fat(_T47,sizeof(void*),4);_T3B(_T3D);}}}}};}}
# 585
static int Cyc_Tctyp_aqualvar_unconstrained(struct _tuple14*t){struct _tuple14*_T0;struct _tuple14 _T1;void*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;struct _tuple14*_TB;struct _tuple14 _TC;int(*_TD)(struct _fat_ptr);void*(*_TE)(struct _fat_ptr);struct _fat_ptr _TF;_T0=t;_T1=*_T0;_T2=_T1.f0;{
void*_T10=Cyc_Absyn_compress(_T2);_T3=(int*)_T10;_T4=*_T3;if(_T4!=0)goto _TLD7;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 16:
# 588
 return 0;case 17:
# 590
 return 1;default: goto _LL5;}goto _TLD8;_TLD7: _LL5:{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;
# 592
_T11.f1=_tag_fat("Expect only Aqual types -- got ",sizeof(char),32U);_T9=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T12;_T12.tag=2;_TB=t;_TC=*_TB;_T12.f1=_TC.f0;_TA=_T12;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T12=_TA;void*_T13[2];_T13[0]=& _T11;_T13[1]=& _T12;_TE=Cyc_Warn_impos2;{int(*_T14)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TE;_TD=_T14;}_TF=_tag_fat(_T13,sizeof(void*),2);_TD(_TF);}}_TLD8:;}}
# 596
static int Cyc_Tctyp_aqualsof_var_bnd(void*bndv,void*var){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;int _TE;void*_TF;_T0=bndv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TLDA;_T3=bndv;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=15)goto _TLDC;_T8=bndv;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_TA=_T9->f2;if(_TA==0)goto _TLDE;_TB=bndv;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB;_TC=_T10->f2;{struct Cyc_List_List _T11=*_TC;_TD=_T11.hd;_TF=(void*)_TD;}}{void*vv=_TF;_TE=
# 599
Cyc_Tcutil_typecmp(var,vv);return _TE;}_TLDE: goto _LL3;_TLDC: goto _LL3;_TLDA: _LL3:
# 601
 return 1;;}
# 606
static void Cyc_Tctyp_constrain_vars(unsigned loc,struct Cyc_Tctyp_CVTEnv env,struct Cyc_List_List*vars,struct Cyc_List_List**qb,void*def,int make_ex_aliasable){struct Cyc_List_List*_T0;void*_T1;int _T2;void*_T3;int*_T4;unsigned _T5;void*_T6;struct Cyc_Absyn_Tvar*_T7;void*_T8;struct Cyc_Absyn_Tvar*_T9;struct Cyc_Absyn_Kind*_TA;struct Cyc_Core_Opt _TB;void*_TC;struct Cyc_Absyn_Kind*_TD;unsigned _TE;struct Cyc_Core_Opt _TF;void*_T10;struct Cyc_Absyn_Kind*_T11;void*_T12;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T13;struct Cyc_Core_Opt*_T14;void*_T15;struct Cyc_Core_Opt*_T16;void*_T17;struct Cyc_Absyn_Kind*_T18;enum Cyc_Absyn_KindQual _T19;int _T1A;struct Cyc_Absyn_Kind*_T1B;enum Cyc_Absyn_KindQual _T1C;int _T1D;struct Cyc_Absyn_Kind*_T1E;enum Cyc_Absyn_KindQual _T1F;int _T20;struct _handler_cons*_T21;int*_T22;int _T23;struct Cyc_List_List**_T24;struct Cyc_List_List*_T25;void*_T26;int _T27;int _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Typ_Warn_Warg_struct _T2A;unsigned _T2B;struct _fat_ptr _T2C;void*_T2D;struct Cyc_Core_Not_found_exn_struct*_T2E;char*_T2F;char*_T30;int _T31;int _T32;struct Cyc_List_List**_T33;struct Cyc_List_List*_T34;struct _tuple0*_T35;struct Cyc_List_List**_T36;struct Cyc_List_List**_T37;struct Cyc_List_List*_T38;struct _tuple0*_T39;struct Cyc_List_List**_T3A;struct Cyc_Absyn_Kind*_T3B;enum Cyc_Absyn_KindQual _T3C;int _T3D;struct _handler_cons*_T3E;int*_T3F;int _T40;struct Cyc_List_List**_T41;struct Cyc_List_List*_T42;void*_T43;int _T44;int _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Warn_Typ_Warn_Warg_struct _T47;unsigned _T48;struct _fat_ptr _T49;void*_T4A;struct Cyc_Core_Not_found_exn_struct*_T4B;char*_T4C;char*_T4D;int _T4E;int _T4F;struct Cyc_List_List**_T50;struct Cyc_List_List*_T51;struct _tuple0*_T52;struct Cyc_List_List**_T53;struct Cyc_List_List**_T54;struct Cyc_List_List*_T55;struct _tuple0*_T56;struct Cyc_List_List**_T57;void*_T58;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T59;void*_T5A;int*_T5B;int _T5C;void*_T5D;struct Cyc_List_List*_T5E;void*_T5F;int*_T60;int _T61;void*_T62;struct Cyc_Absyn_Tvar*_T63;void*_T64;unsigned _T65;struct Cyc_Absyn_Tvar*_T66;struct _handler_cons*_T67;int*_T68;int _T69;struct Cyc_List_List**_T6A;struct Cyc_List_List*_T6B;void*_T6C;int _T6D;int _T6E;struct Cyc_Warn_String_Warn_Warg_struct _T6F;struct Cyc_Warn_Typ_Warn_Warg_struct _T70;unsigned _T71;struct _fat_ptr _T72;struct Cyc_List_List*_T73;struct Cyc_List_List*_T74;struct Cyc_List_List*_T75;void*_T76;void*_T77;struct Cyc_Core_Not_found_exn_struct*_T78;char*_T79;char*_T7A;int*_T7B;unsigned _T7C;int _T7D;int _T7E;struct Cyc_List_List**_T7F;struct Cyc_List_List*_T80;struct _tuple0*_T81;struct Cyc_List_List**_T82;struct Cyc_List_List*_T83;struct Cyc_List_List*_T84;struct Cyc_List_List*_T85;void*_T86;struct Cyc_List_List**_T87;struct Cyc_List_List*_T88;struct _tuple0*_T89;struct Cyc_List_List**_T8A;struct Cyc_List_List*_T8B;struct Cyc_List_List*_T8C;struct Cyc_List_List*_T8D;void*_T8E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8F;void*_T90;int*_T91;int _T92;struct Cyc_Warn_String_Warn_Warg_struct _T93;int(*_T94)(struct _fat_ptr);void*(*_T95)(struct _fat_ptr);struct _fat_ptr _T96;struct Cyc_Warn_String_Warn_Warg_struct _T97;struct Cyc_Warn_Typ_Warn_Warg_struct _T98;int(*_T99)(struct _fat_ptr);void*(*_T9A)(struct _fat_ptr);struct _fat_ptr _T9B;struct Cyc_List_List*_T9C;
# 610
_TLE3: if(vars!=0)goto _TLE1;else{goto _TLE2;}
_TLE1: _T0=vars;_T1=_T0->hd;{struct _tuple14*_T9D=(struct _tuple14*)_T1;int _T9E;void*_T9F;{struct _tuple14 _TA0=*_T9D;_T9F=_TA0.f0;_T9E=_TA0.f1;}{void*v=_T9F;int existential=_T9E;_T2=make_ex_aliasable;
if(_T2)goto _TLE4;else{goto _TLE6;}
_TLE6: existential=0;goto _TLE5;_TLE4: _TLE5: {
void*adef=def;
int explicit_bound_set=0;struct Cyc_List_List*_TA0;struct Cyc_Absyn_Kind*_TA1;struct Cyc_Absyn_Tvar*_TA2;_T3=v;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 2: _T6=v;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TA3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6;_TA2=_TA3->f1;}{struct Cyc_Absyn_Tvar*tv=_TA2;_T7=tv;_T8=_T7->aquals_bound;
# 618
if(_T8==0)goto _TLE8;_T9=tv;
adef=_T9->aquals_bound;
explicit_bound_set=1;goto _TLE9;_TLE8: _TLE9: {
# 622
struct Cyc_Core_Opt ko=Cyc_Kinds_tvar_kind_opt(tv);_TB=ko;_TC=_TB.v;_TD=(struct Cyc_Absyn_Kind*)_TC;_TE=(unsigned)_TD;
if(!_TE)goto _TLEA;_TF=ko;_T10=_TF.v;_TA=(struct Cyc_Absyn_Kind*)_T10;goto _TLEB;_TLEA: _T11=& Cyc_Kinds_ek;_TA=(struct Cyc_Absyn_Kind*)_T11;_TLEB: _TA1=_TA;goto _LL7;}}case 1: _T12=v;_T13=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T12;_T14=_T13->f1;if(_T14==0)goto _TLEC;_T15=v;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_TA3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T15;_T16=_TA3->f1;{struct Cyc_Core_Opt _TA4=*_T16;_T17=_TA4.v;_TA1=(struct Cyc_Absyn_Kind*)_T17;}}_LL7:{struct Cyc_Absyn_Kind*k=_TA1;_T18=k;_T19=_T18->kind;_T1A=(int)_T19;
# 625
if(_T1A==2)goto _TLF0;else{goto _TLF2;}_TLF2: _T1B=k;_T1C=_T1B->kind;_T1D=(int)_T1C;if(_T1D==0)goto _TLF0;else{goto _TLF1;}_TLF1: _T1E=k;_T1F=_T1E->kind;_T20=(int)_T1F;if(_T20==1)goto _TLF0;else{goto _TLEE;}
_TLF0:{struct _handler_cons _TA3;_T21=& _TA3;_push_handler(_T21);{int _TA4=0;_T22=_TA3.handler;_T23=setjmp(_T22);if(!_T23)goto _TLF3;_TA4=1;goto _TLF4;_TLF3: _TLF4: if(_TA4)goto _TLF5;else{goto _TLF7;}_TLF7: _T24=qb;_T25=*_T24;_T26=v;{
void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_aqualsof_var_bnd,_T25,_T26);_T27=explicit_bound_set;
if(!_T27)goto _TLF8;_T28=Cyc_Tcutil_typecmp(adef,xbnd);if(!_T28)goto _TLF8;{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;
_TA5.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T29=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_T29;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6;_TA6.tag=2;_TA6.f1=v;_T2A=_TA6;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6=_T2A;void*_TA7[2];_TA7[0]=& _TA5;_TA7[1]=& _TA6;_T2B=loc;_T2C=_tag_fat(_TA7,sizeof(void*),2);Cyc_Warn_warn2(_T2B,_T2C);}}goto _TLF9;_TLF8: _TLF9:;}_pop_handler();goto _TLF6;_TLF5: _T2D=Cyc_Core_get_exn_thrown();{void*_TA5=(void*)_T2D;void*_TA6;_T2E=(struct Cyc_Core_Not_found_exn_struct*)_TA5;_T2F=_T2E->tag;_T30=Cyc_Core_Not_found;if(_T2F!=_T30)goto _TLFA;{
# 632
void*aqt=Cyc_Absyn_aqualsof_type(v);_T31=existential;
if(!_T31)goto _TLFC;_T32=explicit_bound_set;if(_T32)goto _TLFC;else{goto _TLFE;}
_TLFE: _T33=qb;{struct Cyc_List_List*_TA7=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA8=_cycalloc(sizeof(struct _tuple0));_TA8->f0=aqt;_TA8->f1=Cyc_Absyn_al_qual_type;_T35=(struct _tuple0*)_TA8;}_TA7->hd=_T35;_T36=qb;_TA7->tl=*_T36;_T34=(struct Cyc_List_List*)_TA7;}*_T33=_T34;goto _TLFD;
# 636
_TLFC: _T37=qb;{struct Cyc_List_List*_TA7=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA8=_cycalloc(sizeof(struct _tuple0));_TA8->f0=aqt;_TA8->f1=adef;_T39=(struct _tuple0*)_TA8;}_TA7->hd=_T39;_T3A=qb;_TA7->tl=*_T3A;_T38=(struct Cyc_List_List*)_TA7;}*_T37=_T38;_TLFD: goto _LLC;}_TLFA: _TA6=_TA5;{void*exn=_TA6;_rethrow(exn);}_LLC:;}_TLF6:;}}goto _TLEF;
# 640
_TLEE: _T3B=k;_T3C=_T3B->kind;_T3D=(int)_T3C;if(_T3D!=7)goto _TLFF;{struct _handler_cons _TA3;_T3E=& _TA3;_push_handler(_T3E);{int _TA4=0;_T3F=_TA3.handler;_T40=setjmp(_T3F);if(!_T40)goto _TL101;_TA4=1;goto _TL102;_TL101: _TL102: if(_TA4)goto _TL103;else{goto _TL105;}_TL105: _T41=qb;_T42=*_T41;_T43=v;{
# 642
void*xbnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,_T42,_T43);_T44=explicit_bound_set;
if(!_T44)goto _TL106;_T45=Cyc_Tcutil_typecmp(adef,xbnd);if(!_T45)goto _TL106;{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;
_TA5.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T46=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_T46;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6;_TA6.tag=2;_TA6.f1=v;_T47=_TA6;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6=_T47;void*_TA7[2];_TA7[0]=& _TA5;_TA7[1]=& _TA6;_T48=loc;_T49=_tag_fat(_TA7,sizeof(void*),2);Cyc_Warn_warn2(_T48,_T49);}}goto _TL107;_TL106: _TL107:;}_pop_handler();goto _TL104;_TL103: _T4A=Cyc_Core_get_exn_thrown();{void*_TA5=(void*)_T4A;void*_TA6;_T4B=(struct Cyc_Core_Not_found_exn_struct*)_TA5;_T4C=_T4B->tag;_T4D=Cyc_Core_Not_found;if(_T4C!=_T4D)goto _TL108;_T4E=existential;
# 647
if(!_T4E)goto _TL10A;_T4F=explicit_bound_set;if(_T4F)goto _TL10A;else{goto _TL10C;}
_TL10C: _T50=qb;{struct Cyc_List_List*_TA7=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA8=_cycalloc(sizeof(struct _tuple0));_TA8->f0=v;_TA8->f1=Cyc_Absyn_al_qual_type;_T52=(struct _tuple0*)_TA8;}_TA7->hd=_T52;_T53=qb;_TA7->tl=*_T53;_T51=(struct Cyc_List_List*)_TA7;}*_T50=_T51;goto _TL10B;
# 650
_TL10A: _T54=qb;{struct Cyc_List_List*_TA7=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA8=_cycalloc(sizeof(struct _tuple0));_TA8->f0=v;_TA8->f1=adef;_T56=(struct _tuple0*)_TA8;}_TA7->hd=_T56;_T57=qb;_TA7->tl=*_T57;_T55=(struct Cyc_List_List*)_TA7;}*_T54=_T55;_TL10B: goto _LL11;_TL108: _TA6=_TA5;{void*exn=_TA6;_rethrow(exn);}_LL11:;}_TL104:;}}goto _TL100;_TLFF: _TL100: _TLEF: goto _LL3;}_TLEC: goto _LLA;case 0: _T58=v;_T59=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T58;_T5A=_T59->f1;_T5B=(int*)_T5A;_T5C=*_T5B;if(_T5C!=17)goto _TL10D;_T5D=v;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5D;_TA0=_TA3->f2;}{struct Cyc_List_List*tv_bnd=_TA0;_T5E=
# 656
_check_null(tv_bnd);{void*tv=_T5E->hd;{struct Cyc_Absyn_Tvar*_TA3;_T5F=tv;_T60=(int*)_T5F;_T61=*_T60;if(_T61!=2)goto _TL10F;_T62=tv;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T62;_TA3=_TA4->f1;}{struct Cyc_Absyn_Tvar*tv=_TA3;_T63=tv;_T64=_T63->aquals_bound;_T65=(unsigned)_T64;
# 659
if(!_T65)goto _TL111;_T66=tv;
adef=_T66->aquals_bound;
explicit_bound_set=1;goto _TL112;_TL111: _TL112: goto _LL16;}_TL10F: goto _LL16;_LL16:;}{struct _handler_cons _TA3;_T67=& _TA3;_push_handler(_T67);{int _TA4=0;_T68=_TA3.handler;_T69=setjmp(_T68);if(!_T69)goto _TL113;_TA4=1;goto _TL114;_TL113: _TL114: if(_TA4)goto _TL115;else{goto _TL117;}_TL117: _T6A=qb;_T6B=*_T6A;_T6C=tv;{
# 668
void*x_bnd=Cyc_List_assoc_cmp(Cyc_Tctyp_bnd_qual_cmp,_T6B,_T6C);_T6D=explicit_bound_set;
if(!_T6D)goto _TL118;_T6E=Cyc_Tcutil_typecmp(x_bnd,adef);if(!_T6E)goto _TL118;{struct Cyc_Warn_String_Warn_Warg_struct _TA5;_TA5.tag=0;
_TA5.f1=_tag_fat("Inconsistent aqual bounds on type ",sizeof(char),35U);_T6F=_TA5;}{struct Cyc_Warn_String_Warn_Warg_struct _TA5=_T6F;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6;_TA6.tag=2;_TA6.f1=v;_T70=_TA6;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA6=_T70;void*_TA7[2];_TA7[0]=& _TA5;_TA7[1]=& _TA6;_T71=loc;_T72=_tag_fat(_TA7,sizeof(void*),2);Cyc_Warn_warn2(_T71,_T72);}}goto _TL119;_TL118: _TL119: _T73=tv_bnd;_T74=_T73->tl;_T75=
_check_null(_T74);_T76=x_bnd;_T75->hd=(void*)_T76;}_pop_handler();goto _TL116;_TL115: _T77=Cyc_Core_get_exn_thrown();{void*_TA5=(void*)_T77;void*_TA6;_T78=(struct Cyc_Core_Not_found_exn_struct*)_TA5;_T79=_T78->tag;_T7A=Cyc_Core_Not_found;if(_T79!=_T7A)goto _TL11A;{
# 675
void*_TA7=Cyc_Absyn_compress(tv);_T7B=(int*)_TA7;_T7C=*_T7B;switch(_T7C){case 1: goto _LL24;case 2: _LL24: _T7D=existential;
# 678
if(!_T7D)goto _TL11D;_T7E=explicit_bound_set;if(_T7E)goto _TL11D;else{goto _TL11F;}
_TL11F: _T7F=qb;{struct Cyc_List_List*_TA8=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA9=_cycalloc(sizeof(struct _tuple0));_TA9->f0=tv;_TA9->f1=Cyc_Absyn_al_qual_type;_T81=(struct _tuple0*)_TA9;}_TA8->hd=_T81;_T82=qb;_TA8->tl=*_T82;_T80=(struct Cyc_List_List*)_TA8;}*_T7F=_T80;_T83=tv_bnd;_T84=_T83->tl;_T85=
_check_null(_T84);_T86=Cyc_Absyn_al_qual_type;_T85->hd=(void*)_T86;goto _TL11E;
# 683
_TL11D: _T87=qb;{struct Cyc_List_List*_TA8=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_TA9=_cycalloc(sizeof(struct _tuple0));_TA9->f0=tv;_TA9->f1=adef;_T89=(struct _tuple0*)_TA9;}_TA8->hd=_T89;_T8A=qb;_TA8->tl=*_T8A;_T88=(struct Cyc_List_List*)_TA8;}*_T87=_T88;_T8B=tv_bnd;_T8C=_T8B->tl;_T8D=
_check_null(_T8C);_T8E=adef;_T8D->hd=(void*)_T8E;_TL11E: goto _LL20;case 0: _T8F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA7;_T90=_T8F->f1;_T91=(int*)_T90;_T92=*_T91;if(_T92!=15)goto _TL120;{struct Cyc_Warn_String_Warn_Warg_struct _TA8;_TA8.tag=0;
# 688
_TA8.f1=_tag_fat("aquals bounds must be present",sizeof(char),30U);_T93=_TA8;}{struct Cyc_Warn_String_Warn_Warg_struct _TA8=_T93;void*_TA9[1];_TA9[0]=& _TA8;_T95=Cyc_Warn_impos2;{int(*_TAA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T95;_T94=_TAA;}_T96=_tag_fat(_TA9,sizeof(void*),1);_T94(_T96);}goto _TL121;_TL120: goto _LL27;_TL121:;default: _LL27: goto _LL20;}_LL20:;}goto _LL1B;_TL11A: _TA6=_TA5;{void*exn=_TA6;_rethrow(exn);}_LL1B:;}_TL116:;}}goto _LL3;}}_TL10D: goto _LLA;default: _LLA:{struct Cyc_Warn_String_Warn_Warg_struct _TA3;_TA3.tag=0;
# 697
_TA3.f1=_tag_fat("Expect only tvars, evars and aqual_vars -- got ",sizeof(char),48U);_T97=_TA3;}{struct Cyc_Warn_String_Warn_Warg_struct _TA3=_T97;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA4;_TA4.tag=2;_TA4.f1=v;_T98=_TA4;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA4=_T98;void*_TA5[2];_TA5[0]=& _TA3;_TA5[1]=& _TA4;_T9A=Cyc_Warn_impos2;{int(*_TA6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T9A;_T99=_TA6;}_T9B=_tag_fat(_TA5,sizeof(void*),2);_T99(_T9B);}}goto _LL3;}_LL3:;}}}_T9C=vars;
# 610
vars=_T9C->tl;goto _TLE3;_TLE2:;}
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
static void Cyc_Tctyp_check_constrain_ptrbnd(unsigned loc,void*t,struct Cyc_Absyn_Kind*expected_kind){struct Cyc_Absyn_Kind*_T0;enum Cyc_Absyn_KindQual _T1;int _T2;int*_T3;int _T4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T5;void**_T6;void**_T7;void*_T8;int _T9;unsigned _TA;void**_TB;void*_TC;void*_TD;void*_TE;int _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;struct Cyc_Warn_String_Warn_Warg_struct _T12;struct Cyc_Warn_Kind_Warn_Warg_struct _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Kind_Warn_Warg_struct _T15;unsigned _T16;struct _fat_ptr _T17;void**_T18;void*_T19;void*_T1A;void*_T1B;_T0=expected_kind;_T1=_T0->kind;_T2=(int)_T1;
if(_T2!=2)goto _TL124;{
void*_T1C=Cyc_Absyn_compress(t);void*_T1D;_T3=(int*)_T1C;_T4=*_T3;if(_T4!=4)goto _TL126;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1C;_T5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T1C;_T6=& _T5->f1.ptr_atts.bounds;_T1D=(void**)_T6;}{void**b=(void**)_T1D;_T7=b;_T8=*_T7;_T9=
# 733
Cyc_Tcutil_is_cvar_type(_T8);if(!_T9)goto _TL128;_TA=loc;_TB=b;_TC=*_TB;_TD=
Cyc_Absyn_thinconst();_TE=Cyc_BansheeIf_equality_constraint(_TC,_TD);_TF=Cyc_BansheeIf_add_constraint(_TA,_TE);if(_TF)goto _TL12A;else{goto _TL12C;}
_TL12C:{struct Cyc_Warn_String_Warn_Warg_struct _T1E;_T1E.tag=0;_T1E.f1=_tag_fat("type ",sizeof(char),6U);_T10=_T1E;}{struct Cyc_Warn_String_Warn_Warg_struct _T1E=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F;_T1F.tag=2;_T1F.f1=t;_T11=_T1F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F=_T11;{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;_T20.f1=_tag_fat(" has kind ",sizeof(char),11U);_T12=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T12;{struct Cyc_Warn_Kind_Warn_Warg_struct _T21;_T21.tag=9;_T21.f1=Cyc_Tcutil_type_kind(t);_T13=_T21;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T21=_T13;{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
_T22.f1=_tag_fat(" but as used here needs kind ",sizeof(char),30U);_T14=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T14;{struct Cyc_Warn_Kind_Warn_Warg_struct _T23;_T23.tag=9;_T23.f1=expected_kind;_T15=_T23;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T23=_T15;void*_T24[6];_T24[0]=& _T1E;_T24[1]=& _T1F;_T24[2]=& _T20;_T24[3]=& _T21;_T24[4]=& _T22;_T24[5]=& _T23;_T16=loc;_T17=_tag_fat(_T24,sizeof(void*),6);Cyc_Warn_err2(_T16,_T17);}}}}}}goto _TL12B;
# 739
_TL12A: _T18=b;_T19=*_T18;_T1A=Cyc_Absyn_compress(_T19);_T1B=Cyc_Absyn_bounds_one();if(_T1A!=_T1B)goto _TL12D;goto _TL12E;_TL12D: _throw_assert();_TL12E: _TL12B: goto _TL129;_TL128: _TL129: goto _LL0;}_TL126: goto _LL0;_LL0:;}goto _TL125;_TL124: _TL125:;}struct _tuple16{enum Cyc_Absyn_Format_Type f0;void*f1;};struct _tuple17{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 749
static struct Cyc_Tctyp_CVTEnv Cyc_Tctyp_i_check_type(struct Cyc_Tctyp_CVTEnv env,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Tctyp_CVTEnv _T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;struct Cyc_List_List**_T6;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T7;struct Cyc_Core_Opt**_T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;struct Cyc_Warn_Typ_Warn_Warg_struct _TA;unsigned _TB;struct _fat_ptr _TC;struct Cyc_Core_Opt**_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Absyn_Kind*_TF;struct Cyc_Core_Opt**_T10;struct Cyc_Core_Opt*_T11;void*_T12;struct Cyc_Absyn_Kind*_T13;int _T14;struct Cyc_Core_Opt**_T15;struct Cyc_Core_Opt*_T16;struct Cyc_Core_Opt*_T17;void*_T18;struct Cyc_Absyn_Kind*_T19;struct Cyc_Absyn_Kind*_T1A;int _T1B;struct Cyc_Core_Opt**_T1C;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1D;struct Cyc_Core_Opt**_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;void**_T20;struct Cyc_Core_Opt**_T21;struct Cyc_Core_Opt*_T22;struct Cyc_Absyn_Kind*_T23;struct Cyc_Core_Opt**_T24;struct Cyc_Core_Opt*_T25;void*_T26;struct Cyc_Absyn_Kind*_T27;int _T28;struct Cyc_Core_Opt**_T29;struct Cyc_Core_Opt*_T2A;struct Cyc_Core_Opt*_T2B;void*_T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Absyn_Kind*_T2E;int _T2F;struct Cyc_Core_Opt**_T30;struct Cyc_Tctyp_CVTEnv _T31;int _T32;struct Cyc_Tctyp_CVTEnv _T33;int _T34;struct Cyc_Absyn_Kind*_T35;enum Cyc_Absyn_KindQual _T36;int _T37;struct Cyc_Tcenv_Tenv*_T38;int _T39;void**_T3A;struct Cyc_Absyn_Kind*_T3B;enum Cyc_Absyn_KindQual _T3C;int _T3D;struct Cyc_Tcenv_Tenv*_T3E;int _T3F;void**_T40;struct Cyc_Tctyp_CVTEnv _T41;int _T42;struct Cyc_Absyn_Kind*_T43;enum Cyc_Absyn_KindQual _T44;int _T45;struct Cyc_Tcenv_Tenv*_T46;int _T47;void**_T48;struct Cyc_Tctyp_CVTEnv _T49;int _T4A;struct Cyc_Absyn_Kind*_T4B;enum Cyc_Absyn_KindQual _T4C;int _T4D;struct Cyc_Absyn_Kind*_T4E;enum Cyc_Absyn_KindQual _T4F;int _T50;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T51;void*_T52;void**_T53;struct Cyc_Tctyp_CVTEnv _T54;struct Cyc_List_List*_T55;void*_T56;int _T57;struct Cyc_Absyn_Tvar*_T58;void*_T59;int*_T5A;int _T5B;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T5C;struct Cyc_Core_Opt**_T5D;struct Cyc_Core_Opt**_T5E;struct Cyc_Core_Opt*_T5F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T60;unsigned _T61;struct Cyc_Tctyp_CVTEnv _T62;struct Cyc_List_List*_T63;struct Cyc_Absyn_Tvar*_T64;struct Cyc_Tctyp_CVTEnv _T65;int _T66;struct Cyc_Tctyp_CVTEnv _T67;struct Cyc_List_List*_T68;struct Cyc_Absyn_Tvar*_T69;int _T6A;struct Cyc_Absyn_Tvar*_T6B;void*_T6C;int*_T6D;int _T6E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T6F;struct Cyc_Core_Opt**_T70;int _T71;struct Cyc_Core_Opt**_T72;struct Cyc_Core_Opt*_T73;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T74;struct Cyc_Absyn_TypeDecl*_T75;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T76;void***_T77;void*_T78;void*_T79;int*_T7A;unsigned _T7B;void*_T7C;struct Cyc_Tctyp_CVTEnv _T7D;struct Cyc_Tcenv_Tenv*_T7E;int _T7F;struct Cyc_Absyn_Aggrdecl*_T80;void*_T81;struct Cyc_Tctyp_CVTEnv _T82;struct Cyc_Tcenv_Tenv*_T83;int _T84;struct Cyc_Absyn_Enumdecl*_T85;void*_T86;void***_T87;void**_T88;struct Cyc_Tctyp_CVTEnv _T89;struct Cyc_Absyn_PtrInfo _T8A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8B;struct Cyc_Absyn_Tqual*_T8C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8D;void**_T8E;struct Cyc_Absyn_PtrInfo _T8F;struct Cyc_Absyn_PtrAtts _T90;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T91;void**_T92;struct Cyc_Absyn_PtrInfo _T93;struct Cyc_Absyn_PtrAtts _T94;struct Cyc_Absyn_PtrInfo _T95;struct Cyc_Absyn_PtrAtts _T96;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T97;void**_T98;void**_T99;void**_T9A;int*_T9B;int _T9C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9D;void*_T9E;int*_T9F;unsigned _TA0;void**_TA1;void**_TA2;void*_TA3;void**_TA4;void*_TA5;void**_TA6;int*_TA7;int _TA8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA9;void*_TAA;int*_TAB;int _TAC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAD;struct Cyc_List_List*_TAE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAF;struct Cyc_List_List*_TB0;struct Cyc_List_List*_TB1;void*_TB2;int*_TB3;int _TB4;struct Cyc_List_List*_TB5;struct Cyc_Warn_String_Warn_Warg_struct _TB6;unsigned _TB7;struct _fat_ptr _TB8;void**_TB9;struct Cyc_Tctyp_CVTEnv _TBA;struct Cyc_Absyn_Kind*_TBB;struct Cyc_Absyn_Kind*_TBC;void*_TBD;struct Cyc_Tctyp_CVTEnv _TBE;struct Cyc_Absyn_Kind*_TBF;struct Cyc_Absyn_Kind*_TC0;void**_TC1;void*_TC2;struct Cyc_Tctyp_CVTEnv _TC3;struct Cyc_Absyn_Kind*_TC4;struct Cyc_Absyn_Kind*_TC5;void*_TC6;struct Cyc_Tctyp_CVTEnv _TC7;struct Cyc_Absyn_Kind*_TC8;struct Cyc_Absyn_Kind*_TC9;void*_TCA;struct Cyc_Tctyp_CVTEnv _TCB;struct Cyc_Absyn_Kind*_TCC;struct Cyc_Absyn_Kind*_TCD;void**_TCE;void*_TCF;int _TD0;struct Cyc_Tctyp_CVTEnv _TD1;struct Cyc_Absyn_Kind*_TD2;struct Cyc_Absyn_Kind*_TD3;void*_TD4;void**_TD5;int*_TD6;int _TD7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD8;void*_TD9;int*_TDA;int _TDB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDC;struct Cyc_List_List*_TDD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDE;struct Cyc_List_List*_TDF;struct Cyc_List_List*_TE0;void*_TE1;int*_TE2;int _TE3;struct Cyc_List_List*_TE4;struct Cyc_Tctyp_CVTEnv _TE5;int _TE6;void**_TE7;struct Cyc_List_List*_TE8;void*_TE9;struct Cyc_Tctyp_CVTEnv _TEA;struct Cyc_Absyn_Kind*_TEB;struct Cyc_Absyn_Kind*_TEC;void**_TED;void*_TEE;struct Cyc_Absyn_Tqual*_TEF;struct Cyc_Tctyp_CVTEnv _TF0;unsigned _TF1;struct Cyc_Absyn_Tqual*_TF2;int _TF3;void*_TF4;void*_TF5;void*_TF6;int _TF7;int _TF8;int _TF9;struct Cyc_Warn_String_Warn_Warg_struct _TFA;struct Cyc_Warn_Typ_Warn_Warg_struct _TFB;struct Cyc_Warn_String_Warn_Warg_struct _TFC;unsigned _TFD;struct _fat_ptr _TFE;void**_TFF;void*_T100;int _T101;void*_T102;void**_T103;void*_T104;int _T105;int _T106;struct Cyc_Warn_String_Warn_Warg_struct _T107;unsigned _T108;struct _fat_ptr _T109;struct Cyc_Tcenv_Tenv*_T10A;struct Cyc_Absyn_Exp*_T10B;int _T10C;struct Cyc_Warn_String_Warn_Warg_struct _T10D;unsigned _T10E;struct _fat_ptr _T10F;struct Cyc_Tcenv_Tenv*_T110;struct Cyc_Absyn_Exp*_T111;struct Cyc_Absyn_ArrayInfo _T112;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T113;struct Cyc_Absyn_Tqual*_T114;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T115;struct Cyc_Absyn_Exp**_T116;struct Cyc_Absyn_ArrayInfo _T117;struct Cyc_Absyn_ArrayInfo _T118;struct Cyc_Absyn_Exp**_T119;struct Cyc_Tctyp_CVTEnv _T11A;struct Cyc_Absyn_Kind*_T11B;struct Cyc_Absyn_Kind*_T11C;void*_T11D;int _T11E;struct Cyc_Absyn_Tqual*_T11F;unsigned _T120;struct Cyc_Absyn_Tqual*_T121;int _T122;void*_T123;int _T124;int _T125;struct Cyc_Warn_String_Warn_Warg_struct _T126;struct Cyc_Warn_Typ_Warn_Warg_struct _T127;struct Cyc_Warn_String_Warn_Warg_struct _T128;unsigned _T129;struct _fat_ptr _T12A;int _T12B;struct Cyc_Warn_String_Warn_Warg_struct _T12C;unsigned _T12D;struct _fat_ptr _T12E;struct Cyc_Absyn_Exp**_T12F;struct Cyc_Tcenv_Tenv*_T130;struct Cyc_Absyn_Exp*_T131;int _T132;struct Cyc_Warn_String_Warn_Warg_struct _T133;unsigned _T134;struct _fat_ptr _T135;int _T136;int _T137;struct Cyc_Warn_String_Warn_Warg_struct _T138;unsigned _T139;struct _fat_ptr _T13A;int _T13B;int _T13C;struct Cyc_Warn_String_Warn_Warg_struct _T13D;unsigned _T13E;struct _fat_ptr _T13F;struct Cyc_Absyn_Exp**_T140;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T141;struct Cyc_List_List**_T142;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T143;void**_T144;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T145;struct Cyc_Absyn_Tqual*_T146;struct Cyc_Absyn_FnInfo _T147;struct Cyc_Absyn_FnInfo _T148;struct Cyc_Absyn_FnInfo _T149;struct Cyc_Absyn_FnInfo _T14A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T14B;struct Cyc_List_List**_T14C;struct Cyc_Absyn_FnInfo _T14D;struct Cyc_Absyn_FnInfo _T14E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T14F;struct Cyc_AssnDef_ExistAssnFn**_T150;struct Cyc_Absyn_FnInfo _T151;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T152;struct Cyc_AssnDef_ExistAssnFn**_T153;struct Cyc_Absyn_FnInfo _T154;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T155;struct Cyc_AssnDef_ExistAssnFn**_T156;struct Cyc_Absyn_FnInfo _T157;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T158;struct Cyc_AssnDef_ExistAssnFn**_T159;struct Cyc_Absyn_FnInfo _T15A;struct Cyc_Absyn_FnInfo _T15B;struct Cyc_Absyn_FnInfo _T15C;struct Cyc_List_List*_T15D;int*_T15E;unsigned _T15F;struct Cyc_List_List*_T160;void*_T161;int _T162;struct Cyc_Warn_String_Warn_Warg_struct _T163;struct Cyc_Warn_Attribute_Warn_Warg_struct _T164;struct Cyc_List_List*_T165;unsigned _T166;struct _fat_ptr _T167;struct Cyc_List_List*_T168;int _T169;int _T16A;int _T16B;struct Cyc_Warn_String_Warn_Warg_struct _T16C;unsigned _T16D;struct _fat_ptr _T16E;struct Cyc_Warn_String_Warn_Warg_struct _T16F;unsigned _T170;struct _fat_ptr _T171;unsigned _T172;struct Cyc_List_List**_T173;struct Cyc_List_List*_T174;struct Cyc_List_List**_T175;struct Cyc_List_List*_T176;void*_T177;struct Cyc_Absyn_Tvar*_T178;struct Cyc_Tctyp_CVTEnv _T179;struct Cyc_List_List*_T17A;struct Cyc_List_List*_T17B;void*_T17C;struct Cyc_Absyn_Tvar*_T17D;struct Cyc_List_List*_T17E;void*_T17F;struct Cyc_Absyn_Tvar*_T180;void*_T181;int*_T182;int _T183;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T184;struct Cyc_Absyn_Kind*_T185;struct Cyc_Absyn_Kind*_T186;enum Cyc_Absyn_KindQual _T187;struct Cyc_Warn_String_Warn_Warg_struct _T188;struct Cyc_Warn_Tvar_Warn_Warg_struct _T189;struct Cyc_List_List*_T18A;void*_T18B;unsigned _T18C;struct _fat_ptr _T18D;struct Cyc_List_List*_T18E;struct Cyc_Tctyp_CVTEnv _T18F;struct Cyc_Tctyp_CVTEnv _T190;struct Cyc_Tctyp_CVTEnv _T191;struct Cyc_Tctyp_CVTEnv _T192;struct Cyc_Absyn_Kind*_T193;struct Cyc_Absyn_Kind*_T194;void*_T195;struct Cyc_Absyn_Tqual*_T196;unsigned _T197;struct Cyc_Absyn_Tqual*_T198;int _T199;void*_T19A;struct Cyc_List_List*_T19B;void*_T19C;struct _tuple9*_T19D;struct _tuple9 _T19E;struct Cyc_Tctyp_CVTEnv _T19F;struct Cyc_Absyn_Kind*_T1A0;struct Cyc_Absyn_Kind*_T1A1;void*_T1A2;unsigned _T1A3;struct _tuple9*_T1A4;struct _tuple9 _T1A5;struct Cyc_Absyn_Tqual _T1A6;int _T1A7;void*_T1A8;struct _tuple9*_T1A9;int _T1AA;struct Cyc_Tctyp_CVTEnv _T1AB;struct Cyc_Absyn_Kind*_T1AC;struct Cyc_Absyn_Kind*_T1AD;void*_T1AE;struct Cyc_Core_Opt*_T1AF;struct Cyc_Core_Opt*_T1B0;void*_T1B1;void*_T1B2;struct Cyc_Tctyp_CVTEnv _T1B3;struct Cyc_Absyn_Kind*_T1B4;struct Cyc_Absyn_Kind*_T1B5;void*_T1B6;struct _tuple9*_T1B7;struct Cyc_List_List*_T1B8;int _T1B9;struct Cyc_Warn_String_Warn_Warg_struct _T1BA;int(*_T1BB)(struct _fat_ptr);void*(*_T1BC)(struct _fat_ptr);struct _fat_ptr _T1BD;struct Cyc_Absyn_VarargInfo*_T1BE;struct Cyc_Tctyp_CVTEnv _T1BF;struct Cyc_Absyn_Kind*_T1C0;struct Cyc_Absyn_Kind*_T1C1;void*_T1C2;struct Cyc_Absyn_VarargInfo*_T1C3;unsigned _T1C4;struct Cyc_Absyn_VarargInfo*_T1C5;struct Cyc_Absyn_Tqual _T1C6;int _T1C7;void*_T1C8;int _T1C9;int*_T1CA;int _T1CB;struct Cyc_Absyn_PtrInfo _T1CC;struct Cyc_Absyn_PtrInfo _T1CD;struct Cyc_Absyn_PtrAtts _T1CE;struct Cyc_Absyn_PtrInfo _T1CF;struct Cyc_Absyn_PtrAtts _T1D0;int*_T1D1;int _T1D2;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D3;void*_T1D4;int*_T1D5;int _T1D6;int _T1D7;struct Cyc_Warn_String_Warn_Warg_struct _T1D8;unsigned _T1D9;struct _fat_ptr _T1DA;void*_T1DB;void*_T1DC;int _T1DD;int _T1DE;unsigned _T1DF;void*_T1E0;void*_T1E1;void*_T1E2;int _T1E3;struct Cyc_Warn_String_Warn_Warg_struct _T1E4;unsigned _T1E5;struct _fat_ptr _T1E6;struct Cyc_Warn_String_Warn_Warg_struct _T1E7;unsigned _T1E8;struct _fat_ptr _T1E9;struct Cyc_Warn_String_Warn_Warg_struct _T1EA;unsigned _T1EB;struct _fat_ptr _T1EC;int _T1ED;int _T1EE;int _T1EF;int _T1F0;struct Cyc_Warn_String_Warn_Warg_struct _T1F1;unsigned _T1F2;struct _fat_ptr _T1F3;struct _tuple9*(*_T1F4)(struct Cyc_List_List*,int);void*(*_T1F5)(struct Cyc_List_List*,int);struct Cyc_List_List*_T1F6;int _T1F7;struct _tuple9*_T1F8;int*_T1F9;int _T1FA;struct Cyc_Absyn_PtrInfo _T1FB;struct Cyc_Absyn_PtrInfo _T1FC;struct Cyc_Absyn_PtrAtts _T1FD;int _T1FE;struct Cyc_Warn_String_Warn_Warg_struct _T1FF;unsigned _T200;struct _fat_ptr _T201;int _T202;void*_T203;void*_T204;int _T205;struct Cyc_Warn_String_Warn_Warg_struct _T206;unsigned _T207;struct _fat_ptr _T208;int _T209;struct Cyc_Warn_String_Warn_Warg_struct _T20A;unsigned _T20B;struct _fat_ptr _T20C;int _T20D;unsigned _T20E;void*_T20F;void*_T210;void*_T211;void*_T212;int _T213;struct Cyc_Warn_String_Warn_Warg_struct _T214;unsigned _T215;struct _fat_ptr _T216;unsigned _T217;void*_T218;void*_T219;void*_T21A;void*_T21B;int _T21C;struct Cyc_Warn_String_Warn_Warg_struct _T21D;unsigned _T21E;struct _fat_ptr _T21F;struct Cyc_Warn_String_Warn_Warg_struct _T220;unsigned _T221;struct _fat_ptr _T222;int _T223;struct _tuple16 _T224;struct Cyc_Absyn_VarargInfo*_T225;void*_T226;void*_T227;enum Cyc_Absyn_Format_Type _T228;int _T229;void*_T22A;int*_T22B;int _T22C;void*_T22D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22E;void*_T22F;int*_T230;int _T231;void*_T232;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T233;void*_T234;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T235;union Cyc_Absyn_DatatypeInfo _T236;struct _union_DatatypeInfo_KnownDatatype _T237;unsigned _T238;void*_T239;void*_T23A;union Cyc_Absyn_DatatypeInfo _T23B;struct _union_DatatypeInfo_KnownDatatype _T23C;struct Cyc_Absyn_Datatypedecl**_T23D;struct Cyc_Absyn_Datatypedecl*_T23E;struct _tuple1*_T23F;struct _tuple1*_T240;int _T241;void*_T242;int*_T243;int _T244;void*_T245;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T246;void*_T247;int*_T248;int _T249;void*_T24A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24B;void*_T24C;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T24D;union Cyc_Absyn_DatatypeInfo _T24E;struct _union_DatatypeInfo_KnownDatatype _T24F;unsigned _T250;void*_T251;void*_T252;union Cyc_Absyn_DatatypeInfo _T253;struct _union_DatatypeInfo_KnownDatatype _T254;struct Cyc_Absyn_Datatypedecl**_T255;struct Cyc_Absyn_Datatypedecl*_T256;struct _tuple1*_T257;struct _tuple1*_T258;int _T259;int _T25A;struct Cyc_Warn_String_Warn_Warg_struct _T25B;unsigned _T25C;struct _fat_ptr _T25D;struct Cyc_List_List**_T25E;struct Cyc_List_List*_T25F;void*_T260;struct Cyc_Tctyp_CVTEnv _T261;struct Cyc_Absyn_Kind*_T262;struct Cyc_Absyn_Kind*_T263;void*_T264;struct Cyc_Tctyp_CVTEnv _T265;struct Cyc_Absyn_Kind*_T266;struct Cyc_Absyn_Kind*_T267;void*_T268;struct Cyc_List_List*_T269;struct Cyc_List_List*_T26A;int*_T26B;unsigned _T26C;void*_T26D;struct Cyc_Tctyp_CVTEnv _T26E;struct Cyc_Absyn_Kind*_T26F;struct Cyc_Absyn_Kind*_T270;void*_T271;void*_T272;void*_T273;void*_T274;void*_T275;struct Cyc_Tctyp_CVTEnv _T276;struct Cyc_Absyn_Kind*_T277;struct Cyc_Absyn_Kind*_T278;void*_T279;struct Cyc_Tctyp_CVTEnv _T27A;struct Cyc_Absyn_Kind*_T27B;struct Cyc_Absyn_Kind*_T27C;void*_T27D;struct Cyc_List_List*_T27E;struct Cyc_Tctyp_CVTEnv _T27F;struct Cyc_Tcenv_Tenv*_T280;struct Cyc_Tcenv_Tenv*_T281;struct Cyc_Tcenv_Tenv*_T282;struct Cyc_Tcenv_Tenv*_T283;struct Cyc_Tcenv_Tenv*_T284;struct Cyc_Tcenv_Tenv*_T285;struct Cyc_Tcenv_Tenv*_T286;struct Cyc_List_List*_T287;struct Cyc_List_List*_T288;int _T289;struct Cyc_Absyn_VarargInfo*_T28A;unsigned _T28B;struct Cyc_Warn_String_Warn_Warg_struct _T28C;unsigned _T28D;struct _fat_ptr _T28E;struct Cyc_Tctyp_CVTEnv _T28F;struct _fat_ptr _T290;struct Cyc_Absyn_Exp*_T291;struct Cyc_AssnDef_ExistAssnFn**_T292;struct Cyc_Tctyp_CVTEnv _T293;struct _fat_ptr _T294;struct Cyc_Absyn_Exp*_T295;struct Cyc_AssnDef_ExistAssnFn**_T296;struct Cyc_List_List*_T297;struct Cyc_List_List*_T298;struct Cyc_List_List*_T299;struct Cyc_Tctyp_CVTEnv _T29A;struct _fat_ptr _T29B;struct Cyc_Absyn_Exp*_T29C;struct Cyc_AssnDef_ExistAssnFn**_T29D;struct Cyc_AssnDef_AssnMap _T29E;struct Cyc_List_List*_T29F;struct Cyc_Tctyp_CVTEnv _T2A0;struct _fat_ptr _T2A1;struct Cyc_Absyn_Exp*_T2A2;struct Cyc_AssnDef_ExistAssnFn**_T2A3;void**_T2A4;void*_T2A5;struct Cyc_Tctyp_CVTEnv _T2A6;struct Cyc_Absyn_Kind*_T2A7;struct Cyc_Absyn_Kind*_T2A8;void**_T2A9;void*_T2AA;void*_T2AB;void**_T2AC;void*_T2AD;void*_T2AE;int _T2AF;void**_T2B0;struct Cyc_List_List*_T2B1;void**_T2B2;void*_T2B3;struct _fat_ptr _T2B4;void*_T2B5;struct Cyc_List_List*_T2B6;struct Cyc_List_List*_T2B7;struct Cyc_Tctyp_CVTEnv _T2B8;struct Cyc_List_List*_T2B9;void*_T2BA;struct _tuple12*_T2BB;int _T2BC;struct Cyc_Absyn_Tvar*_T2BD;void*_T2BE;int*_T2BF;unsigned _T2C0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2C1;struct Cyc_Absyn_Kind*_T2C2;struct Cyc_Absyn_Kind*_T2C3;enum Cyc_Absyn_KindQual _T2C4;int _T2C5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2C6;struct Cyc_Core_Opt**_T2C7;struct Cyc_Core_Opt**_T2C8;struct Cyc_Absyn_Kind*_T2C9;struct Cyc_Absyn_Kind*_T2CA;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T2CB;struct Cyc_Absyn_Kind*_T2CC;struct Cyc_Absyn_Kind*_T2CD;enum Cyc_Absyn_KindQual _T2CE;int _T2CF;struct Cyc_List_List*_T2D0;struct Cyc_List_List*_T2D1;void*_T2D2;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T2D3;struct Cyc_Core_Opt**_T2D4;struct Cyc_Core_Opt**_T2D5;struct Cyc_Core_Opt*_T2D6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2D7;struct Cyc_Absyn_Kind*_T2D8;struct Cyc_List_List*_T2D9;struct Cyc_Tctyp_CVTEnv _T2DA;struct Cyc_List_List*_T2DB;void*_T2DC;struct _tuple14*_T2DD;int _T2DE;struct Cyc_Absyn_Kind*_T2DF;enum Cyc_Absyn_KindQual _T2E0;int _T2E1;struct Cyc_List_List*_T2E2;struct Cyc_List_List*_T2E3;struct Cyc_List_List*_T2E4;void**_T2E5;struct Cyc_List_List**_T2E6;struct Cyc_List_List*_T2E7;struct Cyc_List_List**_T2E8;struct Cyc_List_List*_T2E9;void*_T2EA;struct Cyc_Absyn_Tvar*_T2EB;void*_T2EC;int*_T2ED;unsigned _T2EE;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T2EF;struct Cyc_Core_Opt**_T2F0;struct Cyc_Warn_String_Warn_Warg_struct _T2F1;struct Cyc_Warn_Tvar_Warn_Warg_struct _T2F2;struct Cyc_List_List*_T2F3;void*_T2F4;struct Cyc_Warn_String_Warn_Warg_struct _T2F5;unsigned _T2F6;struct _fat_ptr _T2F7;struct Cyc_Core_Opt**_T2F8;struct Cyc_Absyn_Kind*_T2F9;struct Cyc_Absyn_Kind*_T2FA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2FB;struct Cyc_Core_Opt**_T2FC;struct Cyc_Absyn_Kind*_T2FD;struct Cyc_Absyn_Kind*_T2FE;enum Cyc_Absyn_KindQual _T2FF;int _T300;struct Cyc_Core_Opt**_T301;struct Cyc_Absyn_Kind*_T302;struct Cyc_Absyn_Kind*_T303;struct Cyc_Core_Opt**_T304;struct Cyc_Absyn_Kind*_T305;struct Cyc_Absyn_Kind*_T306;struct Cyc_Core_Opt**_T307;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T308;struct Cyc_Absyn_Kind*_T309;struct Cyc_Absyn_Kind*_T30A;enum Cyc_Absyn_KindQual _T30B;struct Cyc_Warn_String_Warn_Warg_struct _T30C;unsigned _T30D;struct _fat_ptr _T30E;struct Cyc_List_List*_T30F;struct Cyc_List_List*(*_T310)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*));struct Cyc_Tctyp_CVTEnv _T311;struct Cyc_List_List*_T312;struct Cyc_List_List**_T313;struct Cyc_List_List*_T314;struct Cyc_Absyn_Tvar*(*_T315)(struct Cyc_Absyn_Tvar*);void*(*_T316)(void*);struct Cyc_List_List*(*_T317)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T318;struct Cyc_List_List*_T319;struct Cyc_List_List**_T31A;struct Cyc_List_List*_T31B;struct Cyc_Absyn_Tvar*(*_T31C)(struct _tuple12*);void*(*_T31D)(struct _tuple0*);struct Cyc_Tctyp_CVTEnv _T31E;struct Cyc_List_List*_T31F;void*_T320;struct _tuple12*_T321;struct Cyc_Tctyp_CVTEnv _T322;struct Cyc_List_List*_T323;struct Cyc_Absyn_Tvar*_T324;int _T325;struct Cyc_List_List*_T326;struct Cyc_Tctyp_CVTEnv _T327;struct Cyc_List_List*_T328;void*_T329;struct _tuple14*_T32A;struct Cyc_Tctyp_CVTEnv _T32B;struct Cyc_List_List*_T32C;void*_T32D;int _T32E;struct Cyc_List_List*_T32F;struct Cyc_Tctyp_CVTEnv _T330;struct Cyc_List_List*_T331;unsigned _T332;struct Cyc_Tctyp_CVTEnv _T333;int(*_T334)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*);int(*_T335)(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T336;void*_T337;struct _tuple14*_T338;struct Cyc_Tctyp_CVTEnv _T339;struct Cyc_List_List*_T33A;int _T33B;struct Cyc_List_List*_T33C;struct Cyc_List_List*_T33D;void*_T33E;struct Cyc_Tctyp_CVTEnv _T33F;struct Cyc_List_List*_T340;struct Cyc_Tctyp_CVTEnv _T341;struct Cyc_List_List*_T342;void*_T343;struct Cyc_Absyn_Tqual*_T344;int(*_T345)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T346)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T347)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T348;struct _fat_ptr*_T349;int _T34A;struct Cyc_Warn_String_Warn_Warg_struct _T34B;struct Cyc_Warn_String_Warn_Warg_struct _T34C;struct _fat_ptr*_T34D;unsigned _T34E;struct _fat_ptr _T34F;struct _fat_ptr*_T350;struct _fat_ptr _T351;struct _fat_ptr _T352;int _T353;struct Cyc_List_List*_T354;struct Cyc_Tctyp_CVTEnv _T355;struct Cyc_Absyn_Kind*_T356;struct Cyc_Absyn_Kind*_T357;void*_T358;struct Cyc_Absyn_Tqual*_T359;unsigned _T35A;struct Cyc_Absyn_Tqual*_T35B;int _T35C;void*_T35D;enum Cyc_Absyn_AggrKind _T35E;int _T35F;struct Cyc_Warn_String_Warn_Warg_struct _T360;unsigned _T361;struct _fat_ptr _T362;struct Cyc_Tcenv_Tenv*_T363;struct Cyc_Absyn_Exp*_T364;int _T365;struct Cyc_Warn_String_Warn_Warg_struct _T366;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T367;struct Cyc_Absyn_Exp*_T368;struct Cyc_Warn_String_Warn_Warg_struct _T369;unsigned _T36A;struct _fat_ptr _T36B;struct Cyc_List_List*_T36C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T36D;struct Cyc_List_List**_T36E;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T36F;struct Cyc_Absyn_Typedefdecl**_T370;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T371;void**_T372;struct _handler_cons*_T373;int*_T374;int _T375;void*_T376;struct Cyc_Dict_Absent_exn_struct*_T377;char*_T378;char*_T379;struct Cyc_Warn_String_Warn_Warg_struct _T37A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T37B;unsigned _T37C;struct _fat_ptr _T37D;struct Cyc_Tctyp_CVTEnv _T37E;struct Cyc_Absyn_Typedefdecl**_T37F;struct Cyc_Tctyp_CVTEnv _T380;struct Cyc_List_List**_T381;struct Cyc_Absyn_Typedefdecl*_T382;struct Cyc_List_List*_T383;struct Cyc_Absyn_Kind*_T384;int _T385;struct _tuple1*_T386;struct Cyc_Absyn_Typedefdecl*_T387;struct Cyc_Absyn_Typedefdecl*_T388;void*_T389;unsigned _T38A;struct Cyc_Absyn_Typedefdecl*_T38B;void*_T38C;struct Cyc_Absyn_Kind*_T38D;struct Cyc_Absyn_Typedefdecl*_T38E;struct Cyc_List_List**_T38F;struct Cyc_List_List*_T390;struct _tuple17*_T391;struct Cyc_List_List*_T392;void*_T393;struct Cyc_List_List*_T394;struct Cyc_List_List*_T395;struct Cyc_List_List*_T396;void**_T397;struct Cyc_List_List*_T398;struct Cyc_Absyn_Typedefdecl*_T399;void*_T39A;void*_T39B;struct Cyc_Absyn_Kind*_T39C;struct Cyc_Absyn_Kind*_T39D;int _T39E;struct Cyc_Warn_String_Warn_Warg_struct _T39F;struct Cyc_Warn_Typ_Warn_Warg_struct _T3A0;struct Cyc_Warn_String_Warn_Warg_struct _T3A1;struct Cyc_Warn_Kind_Warn_Warg_struct _T3A2;struct Cyc_Warn_String_Warn_Warg_struct _T3A3;struct Cyc_Warn_Kind_Warn_Warg_struct _T3A4;unsigned _T3A5;struct _fat_ptr _T3A6;struct Cyc_Tctyp_CVTEnv _T3A7;struct Cyc_Tcenv_Tenv*_T3A8;unsigned _T3A9;_T0=env;_T3A9=_T0.loc;_T1=env;_T3A8=_T1.te;{unsigned loc=_T3A9;struct Cyc_Tcenv_Tenv*te=_T3A8;{
# 752
void*_T3AA=Cyc_Absyn_compress(t);struct _tuple1*_T3AB;enum Cyc_Absyn_AggrKind _T3AC;struct Cyc_List_List*_T3AD;struct Cyc_List_List*_T3AE;struct Cyc_Absyn_Vardecl*_T3AF;void*_T3B0;struct Cyc_Absyn_Exp*_T3B1;struct Cyc_Absyn_Exp*_T3B2;struct Cyc_Absyn_Exp*_T3B3;struct Cyc_Absyn_Exp*_T3B4;struct Cyc_List_List*_T3B5;struct Cyc_Absyn_VarargInfo*_T3B6;int _T3B7;struct Cyc_List_List*_T3B8;unsigned _T3B9;void*_T3BA;void*_T3BB;void*_T3BC;void*_T3BD;void*_T3BE;void*_T3BF;const char*_T3C0;void*_T3C1;void*_T3C2;_T2=(int*)_T3AA;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3AA;_T4=_T3C3->f1;_T3C2=(void*)_T4;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3AA;_T6=& _T5->f2;_T3C1=(struct Cyc_List_List**)_T6;}{void*c=_T3C2;struct Cyc_List_List**targsp=_T3C1;
# 754
env=Cyc_Tctyp_i_check_type_app(env,expected_kind,c,targsp,put_in_effect,allow_abs_aggr,t);goto _LL3;}case 3:{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3AA;_T7=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3AA;_T8=& _T7->f1;_T3C2=(struct Cyc_Core_Opt**)_T8;_T3C0=_T3C3->f5;}{struct Cyc_Core_Opt**k=_T3C2;const char*name=_T3C0;
# 758
if(name!=0)goto _TL130;{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;
_T3C3.f1=_tag_fat("Unnamed Cvar ",sizeof(char),14U);_T9=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T9;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4;_T3C4.tag=2;_T3C4.f1=t;_TA=_T3C4;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4=_TA;void*_T3C5[2];_T3C5[0]=& _T3C3;_T3C5[1]=& _T3C4;_TB=loc;_TC=_tag_fat(_T3C5,sizeof(void*),2);Cyc_Warn_err2(_TB,_TC);}}goto _TL131;_TL130: _TL131: _TD=k;_TE=*_TD;
# 761
if(_TE==0)goto _TL134;else{goto _TL135;}_TL135: _TF=expected_kind;_T10=k;_T11=*_T10;_T12=_T11->v;_T13=(struct Cyc_Absyn_Kind*)_T12;_T14=
Cyc_Kinds_kind_leq(_TF,_T13);
# 761
if(_T14)goto _TL136;else{goto _TL132;}_TL136: _T15=k;_T16=*_T15;_T17=
# 763
_check_null(_T16);_T18=_T17->v;_T19=(struct Cyc_Absyn_Kind*)_T18;_T1A=expected_kind;_T1B=Cyc_Kinds_kind_leq(_T19,_T1A);
# 761
if(_T1B)goto _TL132;else{goto _TL134;}
# 764
_TL134: _T1C=k;*_T1C=Cyc_Kinds_kind_to_opt(expected_kind);goto _TL133;_TL132: _TL133: goto _LL3;}case 1:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3AA;_T1D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3AA;_T1E=& _T1D->f1;_T3C2=(struct Cyc_Core_Opt**)_T1E;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T3AA;_T20=& _T1F->f2;_T3C1=(void**)_T20;}{struct Cyc_Core_Opt**k=_T3C2;void**ref=_T3C1;_T21=k;_T22=*_T21;
# 768
if(_T22==0)goto _TL139;else{goto _TL13A;}_TL13A: _T23=expected_kind;_T24=k;_T25=*_T24;_T26=_T25->v;_T27=(struct Cyc_Absyn_Kind*)_T26;_T28=
Cyc_Kinds_kind_leq(_T23,_T27);
# 768
if(_T28)goto _TL13B;else{goto _TL137;}_TL13B: _T29=k;_T2A=*_T29;_T2B=
# 770
_check_null(_T2A);_T2C=_T2B->v;_T2D=(struct Cyc_Absyn_Kind*)_T2C;_T2E=expected_kind;_T2F=Cyc_Kinds_kind_leq(_T2D,_T2E);
# 768
if(_T2F)goto _TL137;else{goto _TL139;}
# 771
_TL139: _T30=k;*_T30=Cyc_Kinds_kind_to_opt(expected_kind);goto _TL138;_TL137: _TL138: _T31=env;_T32=_T31.fn_result;
# 773
if(!_T32)goto _TL13C;_T33=env;_T34=_T33.generalize_evars;if(!_T34)goto _TL13C;_T35=expected_kind;_T36=_T35->kind;_T37=(int)_T36;
if(_T37!=3)goto _TL13E;_T38=te;_T39=_T38->tempest_generalize;if(_T39)goto _TL13E;else{goto _TL140;}
_TL140: _T3A=ref;*_T3A=Cyc_Absyn_heap_rgn_type;goto _LL3;
# 778
_TL13E: _T3B=expected_kind;_T3C=_T3B->kind;_T3D=(int)_T3C;if(_T3D!=7)goto _TL141;_T3E=te;_T3F=_T3E->tempest_generalize;if(_T3F)goto _TL141;else{goto _TL143;}
_TL143: _T40=ref;*_T40=Cyc_Absyn_al_qual_type;goto _LL3;_TL141: goto _TL13D;_TL13C: _TL13D: _T41=env;_T42=_T41.inst_type_evar;
# 784
if(!_T42)goto _TL144;_T43=expected_kind;_T44=_T43->kind;_T45=(int)_T44;if(_T45!=7)goto _TL144;_T46=te;_T47=_T46->tempest_generalize;if(_T47)goto _TL144;else{goto _TL146;}
# 787
_TL146: _T48=ref;*_T48=Cyc_Absyn_al_qual_type;goto _LL3;_TL144: _T49=env;_T4A=_T49.generalize_evars;
# 791
if(!_T4A)goto _TL147;_T4B=expected_kind;_T4C=_T4B->kind;_T4D=(int)_T4C;if(_T4D==5)goto _TL147;_T4E=expected_kind;_T4F=_T4E->kind;_T50=(int)_T4F;if(_T50==6)goto _TL147;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C3=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3C3->tag=2;
# 794
_T3C3->f1=0;_T3C3->f2=expected_kind;_T51=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C3;}_T52=(void*)_T51;{struct Cyc_Absyn_Tvar*v=Cyc_Tcutil_new_tvar(_T52);_T53=ref;
*_T53=Cyc_Absyn_var_type(v);_T3C2=v;goto _LLB;}_TL147: _T54=env;_T55=_T54.free_evars;_T56=t;_T57=put_in_effect;
# 798
env.free_evars=Cyc_Tctyp_add_free_evar(_T55,_T56,_T57);goto _LL3;}case 2:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3AA;_T3C2=_T3C3->f1;}_LLB: {struct Cyc_Absyn_Tvar*v=_T3C2;_T58=v;_T59=_T58->kind;{
# 801
void*_T3C3=Cyc_Kinds_compress_kb(_T59);void*_T3C4;_T5A=(int*)_T3C3;_T5B=*_T5A;if(_T5B!=1)goto _TL149;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3C5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C3;_T5C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C3;_T5D=& _T5C->f1;_T3C4=(struct Cyc_Core_Opt**)_T5D;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3C4;_T5E=f;{struct Cyc_Core_Opt*_T3C5=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C6=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3C6->tag=2;
_T3C6->f1=0;_T3C6->f2=expected_kind;_T60=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C6;}_T3C5->v=(void*)_T60;_T5F=(struct Cyc_Core_Opt*)_T3C5;}*_T5E=_T5F;goto _LL1C;}_TL149: goto _LL1C;_LL1C:;}_T61=loc;_T62=env;_T63=_T62.kind_env;_T64=v;_T65=env;_T66=_T65.allow_aqual_bounds;
# 807
env.kind_env=Cyc_Tctyp_add_free_tvar(_T61,_T63,_T64,_T66);_T67=env;_T68=_T67.free_vars;_T69=v;_T6A=put_in_effect;
# 810
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T68,_T69,_T6A);_T6B=v;_T6C=_T6B->kind;{
# 812
void*_T3C3=Cyc_Kinds_compress_kb(_T6C);struct Cyc_Absyn_Kind*_T3C4;void*_T3C5;_T6D=(int*)_T3C3;_T6E=*_T6D;if(_T6E!=2)goto _TL14B;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C3;_T6F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C3;_T70=& _T6F->f1;_T3C5=(struct Cyc_Core_Opt**)_T70;_T3C4=_T3C6->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3C5;struct Cyc_Absyn_Kind*k=_T3C4;_T71=
# 814
Cyc_Kinds_kind_leq(expected_kind,k);if(!_T71)goto _TL14D;_T72=f;{struct Cyc_Core_Opt*_T3C6=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C7=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3C7->tag=2;
_T3C7->f1=0;_T3C7->f2=expected_kind;_T74=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C7;}_T3C6->v=(void*)_T74;_T73=(struct Cyc_Core_Opt*)_T3C6;}*_T72=_T73;goto _TL14E;_TL14D: _TL14E: goto _LL21;}_TL14B: goto _LL21;_LL21:;}goto _LL3;}case 10:{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T3AA;_T75=_T3C3->f1;{struct Cyc_Absyn_TypeDecl _T3C4=*_T75;_T3C2=_T3C4.r;}_T76=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T3AA;_T77=& _T76->f2;_T3C1=(void***)_T77;}{void*td=_T3C2;void***topt=_T3C1;_T78=
# 824
Cyc_Absyn_compress(t);{void*new_typ=Cyc_Tcutil_copy_type(_T78);{struct Cyc_Absyn_Datatypedecl*_T3C3;struct Cyc_Absyn_Enumdecl*_T3C4;struct Cyc_Absyn_Aggrdecl*_T3C5;_T79=td;_T7A=(int*)_T79;_T7B=*_T7A;switch(_T7B){case 0: _T7C=td;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T3C6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T7C;_T3C5=_T3C6->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T3C5;_T7D=env;_T7E=_T7D.te;_T7F=_T7E->in_extern_c_include;
# 827
if(!_T7F)goto _TL150;_T80=ad;
_T80->sc=4U;goto _TL151;_TL150: _TL151:
 Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL26;}case 1: _T81=td;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T3C6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T81;_T3C4=_T3C6->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T3C4;_T82=env;_T83=_T82.te;_T84=_T83->in_extern_c_include;
# 831
if(!_T84)goto _TL152;_T85=ed;
_T85->sc=4U;goto _TL153;_TL152: _TL153:
 Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL26;}default: _T86=td;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T3C6=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T86;_T3C3=_T3C6->f1;}{struct Cyc_Absyn_Datatypedecl*dd=_T3C3;
# 835
Cyc_Tc_tcDatatypedecl(te,loc,dd);goto _LL26;}}_LL26:;}_T87=topt;{void**_T3C3=_cycalloc(sizeof(void*));
# 837
*_T3C3=new_typ;_T88=(void**)_T3C3;}*_T87=_T88;_T89=
Cyc_Tctyp_i_check_type(env,expected_kind,new_typ,put_in_effect,allow_abs_aggr);return _T89;}}case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3AA;_T8A=_T3C3->f1;_T3C2=_T8A.elt_type;_T8B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3AA;_T8C=& _T8B->f1.elt_tq;_T3C1=(struct Cyc_Absyn_Tqual*)_T8C;_T8D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3AA;_T8E=& _T8D->f1.ptr_atts.eff;_T3BF=(void**)_T8E;_T8F=_T3C3->f1;_T90=_T8F.ptr_atts;_T3BE=_T90.nullable;_T91=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3AA;_T92=& _T91->f1.ptr_atts.bounds;_T3BD=(void**)_T92;_T93=_T3C3->f1;_T94=_T93.ptr_atts;_T3BC=_T94.zero_term;_T95=_T3C3->f1;_T96=_T95.ptr_atts;_T3BB=_T96.autoreleased;_T97=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3AA;_T98=& _T97->f1.ptr_atts.aqual;_T3BA=(void**)_T98;}{void*t1=_T3C2;struct Cyc_Absyn_Tqual*tqp=_T3C1;void**eff_type=_T3BF;void*nullable=_T3BE;void**b=_T3BD;void*zt=_T3BC;void*rel=_T3BB;void**aq=_T3BA;_T99=aq;{
# 842
void*override_aq=*_T99;_T9A=eff_type;{
void*_T3C3=*_T9A;_T9B=(int*)_T3C3;_T9C=*_T9B;if(_T9C!=0)goto _TL154;_T9D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_T9E=_T9D->f1;_T9F=(int*)_T9E;_TA0=*_T9F;switch(_TA0){case 7: _TA1=eff_type;
# 845
*_TA1=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_un_qual_type;goto _LL2D;case 8: _TA2=eff_type;
# 849
*_TA2=Cyc_Absyn_heap_rgn_type;
override_aq=Cyc_Absyn_rc_qual_type;goto _LL2D;default: goto _LL32;}goto _TL155;_TL154: _LL32: goto _LL2D;_TL155: _LL2D:;}_TA3=override_aq;_TA4=aq;_TA5=*_TA4;
# 855
if(_TA3==_TA5)goto _TL157;_TA6=aq;{
void*_T3C3=*_TA6;struct Cyc_List_List*_T3C4;_TA7=(int*)_T3C3;_TA8=*_TA7;if(_TA8!=0)goto _TL159;_TA9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TAA=_TA9->f1;_TAB=(int*)_TAA;_TAC=*_TAB;if(_TAC!=17)goto _TL15B;_TAD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TAE=_TAD->f2;if(_TAE==0)goto _TL15D;_TAF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TB0=_TAF->f2;_TB1=(struct Cyc_List_List*)_TB0;_TB2=_TB1->hd;_TB3=(int*)_TB2;_TB4=*_TB3;if(_TB4!=1)goto _TL15F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TB5=_T3C5->f2;{struct Cyc_List_List _T3C6=*_TB5;_T3C4=_T3C6.tl;}}{struct Cyc_List_List*bnd=_T3C4;goto _LL34;}_TL15F: goto _LL37;_TL15D: goto _LL37;_TL15B: goto _LL37;_TL159: _LL37:{struct Cyc_Warn_String_Warn_Warg_struct _T3C5;_T3C5.tag=0;
# 860
_T3C5.f1=_tag_fat("`U and `RC shorthands override explicit aqual",sizeof(char),46U);_TB6=_T3C5;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C5=_TB6;void*_T3C6[1];_T3C6[0]=& _T3C5;_TB7=loc;_TB8=_tag_fat(_T3C6,sizeof(void*),1);Cyc_Warn_warn2(_TB7,_TB8);}_LL34:;}_TB9=aq;
# 862
*_TB9=override_aq;goto _TL158;_TL157: _TL158:
# 864
 Cyc_Tctyp_check_constrain_ptrbnd(loc,t,expected_kind);_TBA=env;_TBB=& Cyc_Kinds_ak;_TBC=(struct Cyc_Absyn_Kind*)_TBB;_TBD=t1;
env=Cyc_Tctyp_i_check_type(_TBA,_TBC,_TBD,1,1);_TBE=env;_TBF=& Cyc_Kinds_ek;_TC0=(struct Cyc_Absyn_Kind*)_TBF;_TC1=eff_type;_TC2=*_TC1;
env=Cyc_Tctyp_i_check_type(_TBE,_TC0,_TC2,1,1);_TC3=env;_TC4=& Cyc_Kinds_boolk;_TC5=(struct Cyc_Absyn_Kind*)_TC4;_TC6=zt;
env=Cyc_Tctyp_i_check_type(_TC3,_TC5,_TC6,0,1);_TC7=env;_TC8=& Cyc_Kinds_boolk;_TC9=(struct Cyc_Absyn_Kind*)_TC8;_TCA=nullable;
env=Cyc_Tctyp_i_check_type(_TC7,_TC9,_TCA,0,1);_TCB=env;_TCC=& Cyc_Kinds_ptrbk;_TCD=(struct Cyc_Absyn_Kind*)_TCC;_TCE=b;_TCF=*_TCE;_TD0=allow_abs_aggr;
env=Cyc_Tctyp_i_check_type(_TCB,_TCD,_TCF,0,_TD0);_TD1=env;_TD2=& Cyc_Kinds_boolk;_TD3=(struct Cyc_Absyn_Kind*)_TD2;_TD4=rel;
env=Cyc_Tctyp_i_check_type(_TD1,_TD3,_TD4,0,1);_TD5=aq;{
void*_T3C3=*_TD5;struct Cyc_List_List*_T3C4;_TD6=(int*)_T3C3;_TD7=*_TD6;if(_TD7!=0)goto _TL161;_TD8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TD9=_TD8->f1;_TDA=(int*)_TD9;_TDB=*_TDA;if(_TDB!=17)goto _TL163;_TDC=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TDD=_TDC->f2;if(_TDD==0)goto _TL165;_TDE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TDF=_TDE->f2;_TE0=(struct Cyc_List_List*)_TDF;_TE1=_TE0->hd;_TE2=(int*)_TE1;_TE3=*_TE2;if(_TE3!=1)goto _TL167;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C3;_TE4=_T3C5->f2;{struct Cyc_List_List _T3C6=*_TE4;_T3C4=_T3C6.tl;}}{struct Cyc_List_List*bnd=_T3C4;_TE5=env;_TE6=_TE5.def_aliasable;
# 873
if(!_TE6)goto _TL169;_TE7=aq;
*_TE7=Cyc_Absyn_al_qual_type;goto _TL16A;
# 876
_TL169: _TE8=_check_null(bnd);_TE9=Cyc_Absyn_rtd_qual_type;_TE8->hd=(void*)_TE9;_TL16A: goto _LL39;}_TL167: goto _LL3C;_TL165: goto _LL3C;_TL163: goto _LL3C;_TL161: _LL3C: goto _LL39;_LL39:;}_TEA=env;_TEB=& Cyc_Kinds_aqk;_TEC=(struct Cyc_Absyn_Kind*)_TEB;_TED=aq;_TEE=*_TED;
# 882
env=Cyc_Tctyp_i_check_type(_TEA,_TEC,_TEE,0,1);_TEF=tqp;_TF0=env;_TF1=_TF0.loc;_TF2=tqp;_TF3=_TF2->print_const;_TF4=t1;
_TEF->real_const=Cyc_Tcutil_extract_const_from_typedef(_TF1,_TF3,_TF4);_TF5=zt;_TF7=
# 887
Cyc_Tcutil_is_char_type(t1);if(!_TF7)goto _TL16B;_TF6=Cyc_Absyn_true_type;goto _TL16C;_TL16B: _TF6=Cyc_Absyn_false_type;_TL16C: Cyc_Unify_unify(_TF5,_TF6);{
int is_zero_terminated=Cyc_Absyn_type2bool(0,zt);_TF8=is_zero_terminated;
if(!_TF8)goto _TL16D;_TF9=Cyc_Tcutil_admits_zero(t1);if(_TF9)goto _TL16D;else{goto _TL16F;}
_TL16F:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("cannot have a pointer to zero-terminated ",sizeof(char),42U);_TFA=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_TFA;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4;_T3C4.tag=2;_T3C4.f1=t1;_TFB=_T3C4;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4=_TFB;{struct Cyc_Warn_String_Warn_Warg_struct _T3C5;_T3C5.tag=0;_T3C5.f1=_tag_fat(" elements",sizeof(char),10U);_TFC=_T3C5;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C5=_TFC;void*_T3C6[3];_T3C6[0]=& _T3C3;_T3C6[1]=& _T3C4;_T3C6[2]=& _T3C5;_TFD=loc;_TFE=_tag_fat(_T3C6,sizeof(void*),3);Cyc_Warn_err2(_TFD,_TFE);}}}goto _TL16E;_TL16D: _TL16E: _TFF=b;_T100=*_TFF;_T101=
# 893
Cyc_Tcutil_is_cvar_type(_T100);if(_T101)goto _TL170;else{goto _TL172;}
_TL172: _T102=Cyc_Absyn_bounds_one();_T103=b;_T104=*_T103;{struct Cyc_Absyn_Exp*e=Cyc_Tcutil_get_bounds_exp(_T102,_T104);
if(e==0)goto _TL173;{
struct _tuple13 _T3C3=Cyc_Evexp_eval_const_uint_exp(e);int _T3C4;unsigned _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{unsigned sz=_T3C5;int known=_T3C4;_T105=is_zero_terminated;
if(!_T105)goto _TL175;_T106=known;if(_T106)goto _TL178;else{goto _TL177;}_TL178: if(sz < 1U)goto _TL177;else{goto _TL175;}
_TL177:{struct Cyc_Warn_String_Warn_Warg_struct _T3C6;_T3C6.tag=0;_T3C6.f1=_tag_fat("zero-terminated pointer cannot point to empty sequence",sizeof(char),55U);_T107=_T3C6;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C6=_T107;void*_T3C7[1];_T3C7[0]=& _T3C6;_T108=loc;_T109=_tag_fat(_T3C7,sizeof(void*),1);Cyc_Warn_err2(_T108,_T109);}goto _TL176;_TL175: _TL176:;}}goto _TL174;_TL173: _TL174:;}goto _TL171;_TL170: _TL171: goto _LL3;}}}case 9:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T3AA;_T3C2=_T3C3->f1;}{struct Cyc_Absyn_Exp*e=_T3C2;_T10A=
# 906
Cyc_Tcenv_allow_valueof(te);_T10B=e;Cyc_Tcexp_tcExp(_T10A,0,_T10B);_T10C=
Cyc_Tcutil_coerce_uint_type(e);if(_T10C)goto _TL179;else{goto _TL17B;}
_TL17B:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("valueof_t requires an int expression",sizeof(char),37U);_T10D=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T10D;void*_T3C4[1];_T3C4[0]=& _T3C3;_T10E=loc;_T10F=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T10E,_T10F);}goto _TL17A;_TL179: _TL17A:
 env=Cyc_Tctyp_i_check_type_level_exp(e,env);goto _LL3;}case 11:{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T3AA;_T3C2=_T3C3->f1;}{struct Cyc_Absyn_Exp*e=_T3C2;_T110=
# 915
Cyc_Tcenv_allow_valueof(te);_T111=e;Cyc_Tcexp_tcExp(_T110,0,_T111);goto _LL3;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3AA;_T112=_T3C3->f1;_T3C2=_T112.elt_type;_T113=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3AA;_T114=& _T113->f1.tq;_T3C1=(struct Cyc_Absyn_Tqual*)_T114;_T115=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3AA;_T116=& _T115->f1.num_elts;_T3BF=(struct Cyc_Absyn_Exp**)_T116;_T117=_T3C3->f1;_T3BE=_T117.zero_term;_T118=_T3C3->f1;_T3B9=_T118.zt_loc;}{void*t1=_T3C2;struct Cyc_Absyn_Tqual*tqp=_T3C1;struct Cyc_Absyn_Exp**eptr=_T3BF;void*zt=_T3BE;unsigned ztl=_T3B9;_T119=eptr;{
# 920
struct Cyc_Absyn_Exp*e=*_T119;_T11A=env;_T11B=& Cyc_Kinds_mk;_T11C=(struct Cyc_Absyn_Kind*)_T11B;_T11D=t1;_T11E=allow_abs_aggr;
env=Cyc_Tctyp_i_check_type(_T11A,_T11C,_T11D,1,_T11E);_T11F=tqp;_T120=loc;_T121=tqp;_T122=_T121->print_const;_T123=t1;
_T11F->real_const=Cyc_Tcutil_extract_const_from_typedef(_T120,_T122,_T123);{
# 924
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,zt);_T124=is_zero_terminated;
if(!_T124)goto _TL17C;_T125=
# 927
Cyc_Tcutil_admits_zero(t1);if(_T125)goto _TL17E;else{goto _TL180;}
_TL180:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("cannot have a zero-terminated array of ",sizeof(char),40U);_T126=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T126;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4;_T3C4.tag=2;_T3C4.f1=t1;_T127=_T3C4;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3C4=_T127;{struct Cyc_Warn_String_Warn_Warg_struct _T3C5;_T3C5.tag=0;_T3C5.f1=_tag_fat(" elements",sizeof(char),10U);_T128=_T3C5;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C5=_T128;void*_T3C6[3];_T3C6[0]=& _T3C3;_T3C6[1]=& _T3C4;_T3C6[2]=& _T3C5;_T129=loc;_T12A=_tag_fat(_T3C6,sizeof(void*),3);Cyc_Warn_err2(_T129,_T12A);}}}goto _TL17F;_TL17E: _TL17F: goto _TL17D;_TL17C: _TL17D:
# 932
 if(e!=0)goto _TL181;_T12B=is_zero_terminated;
# 935
if(_T12B)goto _TL183;else{goto _TL185;}
_TL185:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("array bound defaults to 1 here",sizeof(char),31U);_T12C=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T12C;void*_T3C4[1];_T3C4[0]=& _T3C3;_T12D=loc;_T12E=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_warn2(_T12D,_T12E);}goto _TL184;_TL183: _TL184: _T12F=eptr;
e=Cyc_Absyn_uint_exp(1U,0U);*_T12F=e;goto _TL182;_TL181: _TL182: _T130=
# 939
Cyc_Tcenv_allow_valueof(te);_T131=e;Cyc_Tcexp_tcExp(_T130,0,_T131);_T132=
Cyc_Tcutil_coerce_uint_type(e);if(_T132)goto _TL186;else{goto _TL188;}
_TL188:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("array bounds expression is not an unsigned int",sizeof(char),47U);_T133=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T133;void*_T3C4[1];_T3C4[0]=& _T3C3;_T134=loc;_T135=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T134,_T135);}goto _TL187;_TL186: _TL187:
 env=Cyc_Tctyp_i_check_type_level_exp(e,env);{
struct _tuple13 _T3C3=Cyc_Evexp_eval_const_uint_exp(e);int _T3C4;unsigned _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{unsigned sz=_T3C5;int known=_T3C4;_T136=is_zero_terminated;
# 945
if(!_T136)goto _TL189;_T137=known;if(!_T137)goto _TL189;if(sz >= 1U)goto _TL189;{struct Cyc_Warn_String_Warn_Warg_struct _T3C6;_T3C6.tag=0;
_T3C6.f1=_tag_fat("zero terminated array cannot have zero elements",sizeof(char),48U);_T138=_T3C6;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C6=_T138;void*_T3C7[1];_T3C7[0]=& _T3C6;_T139=loc;_T13A=_tag_fat(_T3C7,sizeof(void*),1);Cyc_Warn_warn2(_T139,_T13A);}goto _TL18A;_TL189: _TL18A: _T13B=known;
# 948
if(!_T13B)goto _TL18B;if(sz >= 1U)goto _TL18B;_T13C=Cyc_Flags_tovc_r;if(!_T13C)goto _TL18B;{struct Cyc_Warn_String_Warn_Warg_struct _T3C6;_T3C6.tag=0;
_T3C6.f1=_tag_fat("0-element arrays are supported only with gcc, changing to 1",sizeof(char),60U);_T13D=_T3C6;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C6=_T13D;void*_T3C7[1];_T3C7[0]=& _T3C6;_T13E=loc;_T13F=_tag_fat(_T3C7,sizeof(void*),1);Cyc_Warn_warn2(_T13E,_T13F);}_T140=eptr;
*_T140=Cyc_Absyn_uint_exp(1U,0U);goto _TL18C;_TL18B: _TL18C: goto _LL3;}}}}}case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T141=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T142=& _T141->f1.tvars;_T3C2=(struct Cyc_List_List**)_T142;_T143=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T144=& _T143->f1.effect;_T3C1=(void**)_T144;_T145=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T146=& _T145->f1.ret_tqual;_T3BF=(struct Cyc_Absyn_Tqual*)_T146;_T147=_T3C3->f1;_T3BE=_T147.ret_type;_T148=_T3C3->f1;_T3B8=_T148.args;_T149=_T3C3->f1;_T3B7=_T149.c_varargs;_T14A=_T3C3->f1;_T3B6=_T14A.cyc_varargs;_T14B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T14C=& _T14B->f1.qual_bnd;_T3BD=(struct Cyc_List_List**)_T14C;_T14D=_T3C3->f1;_T3B5=_T14D.attributes;_T14E=_T3C3->f1;_T3B4=_T14E.checks_clause;_T14F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T150=& _T14F->f1.checks_assn;_T3BC=(struct Cyc_AssnDef_ExistAssnFn**)_T150;_T151=_T3C3->f1;_T3B3=_T151.requires_clause;_T152=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T153=& _T152->f1.requires_assn;_T3BB=(struct Cyc_AssnDef_ExistAssnFn**)_T153;_T154=_T3C3->f1;_T3B2=_T154.ensures_clause;_T155=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T156=& _T155->f1.ensures_assn;_T3BA=(struct Cyc_AssnDef_ExistAssnFn**)_T156;_T157=_T3C3->f1;_T3B1=_T157.throws_clause;_T158=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3AA;_T159=& _T158->f1.throws_assn;_T3B0=(struct Cyc_AssnDef_ExistAssnFn**)_T159;_T15A=_T3C3->f1;_T3AF=_T15A.return_value;_T15B=_T3C3->f1;_T3AE=_T15B.arg_vardecls;_T15C=_T3C3->f1;_T3AD=_T15C.effconstr;}{struct Cyc_List_List**btvs=_T3C2;void**eff=_T3C1;struct Cyc_Absyn_Tqual*rtq=_T3BF;void*tr=_T3BE;struct Cyc_List_List*args=_T3B8;int c_vararg=_T3B7;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T3B6;struct Cyc_List_List**qb=(struct Cyc_List_List**)_T3BD;struct Cyc_List_List*atts=_T3B5;struct Cyc_Absyn_Exp*chk=_T3B4;struct Cyc_AssnDef_ExistAssnFn**chk_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3BC;struct Cyc_Absyn_Exp*req=_T3B3;struct Cyc_AssnDef_ExistAssnFn**req_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3BB;struct Cyc_Absyn_Exp*ens=_T3B2;struct Cyc_AssnDef_ExistAssnFn**ens_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3BA;struct Cyc_Absyn_Exp*thrws=_T3B1;struct Cyc_AssnDef_ExistAssnFn**thrws_relns=(struct Cyc_AssnDef_ExistAssnFn**)_T3B0;struct Cyc_Absyn_Vardecl*return_vd=_T3AF;struct Cyc_List_List*arg_vds=_T3AE;struct Cyc_List_List*effc=_T3AD;
# 961
int num_formats=0;
int is_cdecl=0;
int is_stdcall=0;
int is_fastcall=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=- 1;
int fmt_arg_start=- 1;
_TL190: if(atts!=0)goto _TL18E;else{goto _TL18F;}
_TL18E: _T15D=atts;{void*_T3C3=_T15D->hd;int _T3C4;int _T3C5;enum Cyc_Absyn_Format_Type _T3C6;_T15E=(int*)_T3C3;_T15F=*_T15E;switch(_T15F){case 1:
 is_stdcall=1;goto _LL44;case 2:
 is_cdecl=1;goto _LL44;case 3:
 is_fastcall=1;goto _LL44;case 19:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_T3C7=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_T3C3;_T3C6=_T3C7->f1;_T3C5=_T3C7->f2;_T3C4=_T3C7->f3;}{enum Cyc_Absyn_Format_Type fmttype=_T3C6;int i=_T3C5;int j=_T3C4;
# 974
num_formats=num_formats + 1;
ft=fmttype;
fmt_desc_arg=i;
fmt_arg_start=j;goto _LL44;}default: _T160=atts;_T161=_T160->hd;_T162=
# 980
Cyc_Atts_fntype_att(_T161);if(_T162)goto _TL192;else{goto _TL194;}
_TL194:{struct Cyc_Warn_String_Warn_Warg_struct _T3C7;_T3C7.tag=0;_T3C7.f1=_tag_fat("bad function type attribute ",sizeof(char),29U);_T163=_T3C7;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C7=_T163;{struct Cyc_Warn_Attribute_Warn_Warg_struct _T3C8;_T3C8.tag=10;_T165=atts;_T3C8.f1=_T165->hd;_T164=_T3C8;}{struct Cyc_Warn_Attribute_Warn_Warg_struct _T3C8=_T164;void*_T3C9[2];_T3C9[0]=& _T3C7;_T3C9[1]=& _T3C8;_T166=loc;_T167=_tag_fat(_T3C9,sizeof(void*),2);Cyc_Warn_err2(_T166,_T167);}}goto _TL193;_TL192: _TL193:;}_LL44:;}_T168=atts;
# 968
atts=_T168->tl;goto _TL190;_TL18F: _T169=is_stdcall + is_cdecl;_T16A=is_fastcall;_T16B=_T169 + _T16A;
# 983
if(_T16B <= 1)goto _TL195;{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;
_T3C3.f1=_tag_fat("function can't have multiple calling conventions",sizeof(char),49U);_T16C=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T16C;void*_T3C4[1];_T3C4[0]=& _T3C3;_T16D=loc;_T16E=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T16D,_T16E);}goto _TL196;_TL195: _TL196:
 if(num_formats <= 1)goto _TL197;{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;
_T3C3.f1=_tag_fat("function can't have multiple format attributes",sizeof(char),47U);_T16F=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T16F;void*_T3C4[1];_T3C4[0]=& _T3C3;_T170=loc;_T171=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T170,_T171);}goto _TL198;_TL197: _TL198: _T172=loc;_T173=btvs;_T174=*_T173;
# 990
Cyc_Tcutil_check_unique_tvars(_T172,_T174);_T175=btvs;{
struct Cyc_List_List*b=*_T175;_TL19C: if(b!=0)goto _TL19A;else{goto _TL19B;}
_TL19A: _T176=b;_T177=_T176->hd;_T178=(struct Cyc_Absyn_Tvar*)_T177;_T178->identity=Cyc_Tcutil_new_tvar_id();_T179=env;_T17A=_T179.kind_env;_T17B=b;_T17C=_T17B->hd;_T17D=(struct Cyc_Absyn_Tvar*)_T17C;
env.kind_env=Cyc_Tctyp_add_bound_tvar(_T17A,_T17D);_T17E=b;_T17F=_T17E->hd;_T180=(struct Cyc_Absyn_Tvar*)_T17F;_T181=_T180->kind;{
void*_T3C3=Cyc_Kinds_compress_kb(_T181);_T182=(int*)_T3C3;_T183=*_T182;if(_T183!=0)goto _TL19D;_T184=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3C3;_T185=_T184->f1;_T186=(struct Cyc_Absyn_Kind*)_T185;_T187=_T186->kind;if(_T187!=Cyc_Absyn_MemKind)goto _TL19F;{struct Cyc_Warn_String_Warn_Warg_struct _T3C4;_T3C4.tag=0;
# 996
_T3C4.f1=_tag_fat("function abstracts Mem type variable ",sizeof(char),38U);_T188=_T3C4;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C4=_T188;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3C5;_T3C5.tag=7;_T18A=b;_T18B=_T18A->hd;_T3C5.f1=(struct Cyc_Absyn_Tvar*)_T18B;_T189=_T3C5;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3C5=_T189;void*_T3C6[2];_T3C6[0]=& _T3C4;_T3C6[1]=& _T3C5;_T18C=loc;_T18D=_tag_fat(_T3C6,sizeof(void*),2);Cyc_Warn_err2(_T18C,_T18D);}}goto _LL4F;_TL19F: goto _LL52;_TL19D: _LL52: goto _LL4F;_LL4F:;}_T18E=b;
# 991
b=_T18E->tl;goto _TL19C;_TL19B:;}{struct Cyc_Tctyp_CVTEnv _T3C3;
# 1002
_T3C3.loc=loc;_T3C3.te=te;_T190=env;_T3C3.kind_env=_T190.kind_env;_T3C3.fn_result=1;_T3C3.def_aliasable=1;_T3C3.inst_type_evar=0;_T191=env;_T3C3.generalize_evars=_T191.generalize_evars;_T3C3.allow_aqual_bounds=1;_T3C3.free_vars=0;_T3C3.free_evars=0;_T3C3.free_qualvars=0;_T18F=_T3C3;}{struct Cyc_Tctyp_CVTEnv new_env=_T18F;_T192=new_env;_T193=& Cyc_Kinds_mk;_T194=(struct Cyc_Absyn_Kind*)_T193;_T195=tr;
new_env=Cyc_Tctyp_i_check_type(_T192,_T194,_T195,1,1);_T196=rtq;_T197=loc;_T198=rtq;_T199=_T198->print_const;_T19A=tr;
_T196->real_const=Cyc_Tcutil_extract_const_from_typedef(_T197,_T199,_T19A);
new_env.fn_result=0;{
# 1010
struct Cyc_List_List*a=args;_TL1A4: if(a!=0)goto _TL1A2;else{goto _TL1A3;}
_TL1A2: _T19B=a;_T19C=_T19B->hd;{struct _tuple9*trip=(struct _tuple9*)_T19C;_T19D=trip;_T19E=*_T19D;{
void*t=_T19E.f2;_T19F=new_env;_T1A0=& Cyc_Kinds_mk;_T1A1=(struct Cyc_Absyn_Kind*)_T1A0;_T1A2=t;
new_env=Cyc_Tctyp_i_check_type(_T19F,_T1A1,_T1A2,1,1);_T1A3=loc;_T1A4=trip;_T1A5=*_T1A4;_T1A6=_T1A5.f1;_T1A7=_T1A6.print_const;_T1A8=t;{
int ec=Cyc_Tcutil_extract_const_from_typedef(_T1A3,_T1A7,_T1A8);_T1A9=trip;
(*_T1A9).f1.real_const=ec;_T1AA=
# 1018
Cyc_Tcutil_is_array_type(t);if(!_T1AA)goto _TL1A5;{
# 1020
void*ptr_rgn=Cyc_Absyn_new_evar(0,0);_T1AB=new_env;_T1AC=& Cyc_Kinds_ek;_T1AD=(struct Cyc_Absyn_Kind*)_T1AC;_T1AE=ptr_rgn;
new_env=Cyc_Tctyp_i_check_type(_T1AB,_T1AD,_T1AE,1,1);_T1AF=& Cyc_Kinds_aqko;_T1B0=(struct Cyc_Core_Opt*)_T1AF;_T1B1=
Cyc_Absyn_new_evar(_T1B0,0);_T1B2=Cyc_Absyn_al_qual_type;{void*aqt=Cyc_Absyn_aqual_var_type(_T1B1,_T1B2);_T1B3=new_env;_T1B4=& Cyc_Kinds_aqk;_T1B5=(struct Cyc_Absyn_Kind*)_T1B4;_T1B6=aqt;
# 1024
new_env=Cyc_Tctyp_i_check_type(_T1B3,_T1B5,_T1B6,1,1);_T1B7=trip;
(*_T1B7).f2=Cyc_Tcutil_promote_array(t,ptr_rgn,aqt,0);}}goto _TL1A6;_TL1A5: _TL1A6:;}}}_T1B8=a;
# 1010
a=_T1B8->tl;goto _TL1A4;_TL1A3:;}
# 1029
if(cyc_vararg==0)goto _TL1A7;_T1B9=c_vararg;
if(!_T1B9)goto _TL1A9;{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("both c_vararg and cyc_vararg",sizeof(char),29U);_T1BA=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T1BA;void*_T3C4[1];_T3C4[0]=& _T3C3;_T1BC=Cyc_Warn_impos2;{int(*_T3C5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1BC;_T1BB=_T3C5;}_T1BD=_tag_fat(_T3C4,sizeof(void*),1);_T1BB(_T1BD);}goto _TL1AA;_TL1A9: _TL1AA: _T1BE=cyc_vararg;{
struct Cyc_Absyn_VarargInfo _T3C3=*_T1BE;int _T3C4;void*_T3C5;_T3C5=_T3C3.type;_T3C4=_T3C3.inject;{void*vt=_T3C5;int vi=_T3C4;_T1BF=new_env;_T1C0=& Cyc_Kinds_mk;_T1C1=(struct Cyc_Absyn_Kind*)_T1C0;_T1C2=vt;
new_env=Cyc_Tctyp_i_check_type(_T1BF,_T1C1,_T1C2,1,1);_T1C3=cyc_vararg;_T1C4=loc;_T1C5=cyc_vararg;_T1C6=_T1C5->tq;_T1C7=_T1C6.print_const;_T1C8=vt;
_T1C3->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T1C4,_T1C7,_T1C8);_T1C9=vi;
# 1035
if(!_T1C9)goto _TL1AB;{
void*_T3C6=Cyc_Absyn_compress(vt);void*_T3C7;void*_T3C8;void*_T3C9;_T1CA=(int*)_T3C6;_T1CB=*_T1CA;if(_T1CB!=4)goto _TL1AD;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3CA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3C6;_T1CC=_T3CA->f1;_T3C9=_T1CC.elt_type;_T1CD=_T3CA->f1;_T1CE=_T1CD.ptr_atts;_T3C8=_T1CE.bounds;_T1CF=_T3CA->f1;_T1D0=_T1CF.ptr_atts;_T3C7=_T1D0.zero_term;}{void*et=_T3C9;void*bs=_T3C8;void*zt=_T3C7;{
# 1038
void*_T3CA=Cyc_Absyn_compress(et);_T1D1=(int*)_T3CA;_T1D2=*_T1D1;if(_T1D2!=0)goto _TL1AF;_T1D3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3CA;_T1D4=_T1D3->f1;_T1D5=(int*)_T1D4;_T1D6=*_T1D5;if(_T1D6!=22)goto _TL1B1;_T1D7=
# 1040
Cyc_Tcutil_force_type2bool(0,zt);if(!_T1D7)goto _TL1B3;{struct Cyc_Warn_String_Warn_Warg_struct _T3CB;_T3CB.tag=0;
_T3CB.f1=_tag_fat("can't inject into a zeroterm pointer",sizeof(char),37U);_T1D8=_T3CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CB=_T1D8;void*_T3CC[1];_T3CC[0]=& _T3CB;_T1D9=loc;_T1DA=_tag_fat(_T3CC,sizeof(void*),1);Cyc_Warn_err2(_T1D9,_T1DA);}goto _TL1B4;_TL1B3: _TL1B4: _T1DB=
Cyc_Absyn_bounds_one();_T1DC=bs;_T1DD=Cyc_Unify_unify(_T1DB,_T1DC);if(_T1DD)goto _TL1B5;else{goto _TL1B7;}
_TL1B7: _T1DE=Cyc_Tcutil_is_cvar_type(bs);if(_T1DE)goto _TL1BB;else{goto _TL1BA;}_TL1BB: _T1DF=loc;_T1E0=bs;_T1E1=
Cyc_Absyn_thinconst();_T1E2=Cyc_BansheeIf_equality_constraint(_T1E0,_T1E1);_T1E3=Cyc_BansheeIf_add_constraint(_T1DF,_T1E2);
# 1043
if(_T1E3)goto _TL1B8;else{goto _TL1BA;}
# 1045
_TL1BA:{struct Cyc_Warn_String_Warn_Warg_struct _T3CB;_T3CB.tag=0;_T3CB.f1=_tag_fat("can't inject into a fat pointer to datatype",sizeof(char),44U);_T1E4=_T3CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CB=_T1E4;void*_T3CC[1];_T3CC[0]=& _T3CB;_T1E5=loc;_T1E6=_tag_fat(_T3CC,sizeof(void*),1);Cyc_Warn_err2(_T1E5,_T1E6);}goto _TL1B9;_TL1B8: _TL1B9: goto _TL1B6;_TL1B5: _TL1B6: goto _LL5C;_TL1B1: goto _LL5F;_TL1AF: _LL5F:{struct Cyc_Warn_String_Warn_Warg_struct _T3CB;_T3CB.tag=0;
# 1049
_T3CB.f1=_tag_fat("can't inject a non-datatype type",sizeof(char),33U);_T1E7=_T3CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CB=_T1E7;void*_T3CC[1];_T3CC[0]=& _T3CB;_T1E8=loc;_T1E9=_tag_fat(_T3CC,sizeof(void*),1);Cyc_Warn_err2(_T1E8,_T1E9);}goto _LL5C;_LL5C:;}goto _LL57;}_TL1AD:{struct Cyc_Warn_String_Warn_Warg_struct _T3CA;_T3CA.tag=0;
# 1052
_T3CA.f1=_tag_fat("expecting a datatype pointer type",sizeof(char),34U);_T1EA=_T3CA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CA=_T1EA;void*_T3CB[1];_T3CB[0]=& _T3CA;_T1EB=loc;_T1EC=_tag_fat(_T3CB,sizeof(void*),1);Cyc_Warn_err2(_T1EB,_T1EC);}goto _LL57;_LL57:;}goto _TL1AC;_TL1AB: _TL1AC:;}}goto _TL1A8;_TL1A7: _TL1A8:
# 1056
 if(num_formats <= 0)goto _TL1BC;{
int num_args=Cyc_List_length(args);
if(fmt_desc_arg < 0)goto _TL1C0;else{goto _TL1C4;}_TL1C4: if(fmt_desc_arg > num_args)goto _TL1C0;else{goto _TL1C3;}_TL1C3: if(fmt_arg_start < 0)goto _TL1C0;else{goto _TL1C2;}_TL1C2: if(cyc_vararg==0)goto _TL1C6;else{goto _TL1C1;}_TL1C6: _T1ED=c_vararg;if(_T1ED)goto _TL1C1;else{goto _TL1C5;}_TL1C5: if(fmt_arg_start!=0)goto _TL1C0;else{goto _TL1C1;}_TL1C1: if(cyc_vararg!=0)goto _TL1C7;else{goto _TL1C8;}_TL1C8: _T1EE=c_vararg;if(_T1EE)goto _TL1C7;else{goto _TL1BE;}_TL1C7: _T1EF=fmt_arg_start;_T1F0=num_args + 1;if(_T1EF!=_T1F0)goto _TL1C0;else{goto _TL1BE;}
# 1063
_TL1C0:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("bad format descriptor",sizeof(char),22U);_T1F1=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T1F1;void*_T3C4[1];_T3C4[0]=& _T3C3;_T1F2=loc;_T1F3=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T1F2,_T1F3);}goto _TL1BF;
# 1066
_TL1BE: _T1F5=Cyc_List_nth;{struct _tuple9*(*_T3C3)(struct Cyc_List_List*,int)=(struct _tuple9*(*)(struct Cyc_List_List*,int))_T1F5;_T1F4=_T3C3;}_T1F6=args;_T1F7=fmt_desc_arg - 1;_T1F8=_T1F4(_T1F6,_T1F7);{struct _tuple9 _T3C3=*_T1F8;void*_T3C4;_T3C4=_T3C3.f2;{void*t=_T3C4;{
# 1068
void*_T3C5=Cyc_Absyn_compress(t);void*_T3C6;void*_T3C7;_T1F9=(int*)_T3C5;_T1FA=*_T1F9;if(_T1FA!=4)goto _TL1C9;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3C5;_T1FB=_T3C8->f1;_T3C7=_T1FB.elt_type;_T1FC=_T3C8->f1;_T1FD=_T1FC.ptr_atts;_T3C6=_T1FD.bounds;}{void*et=_T3C7;void*b=_T3C6;_T1FE=
# 1071
Cyc_Tcutil_is_char_type(et);if(_T1FE)goto _TL1CB;else{goto _TL1CD;}
_TL1CD:{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;_T3C8.f1=_tag_fat("format descriptor is not a string",sizeof(char),34U);_T1FF=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T1FF;void*_T3C9[1];_T3C9[0]=& _T3C8;_T200=loc;_T201=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T200,_T201);}goto _TL1CC;
# 1074
_TL1CB: _T202=Cyc_Tcutil_is_cvar_type(b);if(_T202)goto _TL1CE;else{goto _TL1D0;}
_TL1D0: _T203=Cyc_Absyn_bounds_one();_T204=b;{struct Cyc_Absyn_Exp*e=Cyc_Tcutil_get_bounds_exp(_T203,_T204);
if(e!=0)goto _TL1D1;_T205=c_vararg;if(!_T205)goto _TL1D1;{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;
_T3C8.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_T206=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T206;void*_T3C9[1];_T3C9[0]=& _T3C8;_T207=loc;_T208=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T207,_T208);}goto _TL1D2;
_TL1D1: if(e==0)goto _TL1D3;_T209=c_vararg;if(_T209)goto _TL1D3;else{goto _TL1D5;}
_TL1D5:{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;_T3C8.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_T20A=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T20A;void*_T3C9[1];_T3C9[0]=& _T3C8;_T20B=loc;_T20C=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T20B,_T20C);}goto _TL1D4;_TL1D3: _TL1D4: _TL1D2:;}goto _TL1CF;
# 1082
_TL1CE: _T20D=c_vararg;if(!_T20D)goto _TL1D6;_T20E=loc;_T20F=
Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T210=_check_null(_T20F);_T211=
Cyc_Absyn_thinconst();_T212=
# 1083
Cyc_BansheeIf_equality_constraint(_T210,_T211);_T213=Cyc_BansheeIf_add_constraint(_T20E,_T212);if(_T213)goto _TL1D8;else{goto _TL1DA;}
# 1085
_TL1DA:{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;_T3C8.f1=_tag_fat("format descriptor is not a char ? type",sizeof(char),39U);_T214=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T214;void*_T3C9[1];_T3C9[0]=& _T3C8;_T215=loc;_T216=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T215,_T216);}goto _TL1D9;_TL1D8: _TL1D9: goto _TL1D7;
# 1089
_TL1D6: _T217=loc;_T218=Cyc_Tcutil_ptrbnd_cvar_equivalent(b);_T219=_check_null(_T218);_T21A=
Cyc_Absyn_fatconst();_T21B=
# 1089
Cyc_BansheeIf_equality_constraint(_T219,_T21A);_T21C=Cyc_BansheeIf_add_constraint(_T217,_T21B);if(_T21C)goto _TL1DB;else{goto _TL1DD;}
# 1091
_TL1DD:{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;_T3C8.f1=_tag_fat("format descriptor is not a char * type",sizeof(char),39U);_T21D=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T21D;void*_T3C9[1];_T3C9[0]=& _T3C8;_T21E=loc;_T21F=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T21E,_T21F);}goto _TL1DC;_TL1DB: _TL1DC: _TL1D7: _TL1CF: _TL1CC: goto _LL64;}_TL1C9:{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;
# 1097
_T3C8.f1=_tag_fat("format descriptor is not a string type",sizeof(char),39U);_T220=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T220;void*_T3C9[1];_T3C9[0]=& _T3C8;_T221=loc;_T222=_tag_fat(_T3C9,sizeof(void*),1);Cyc_Warn_err2(_T221,_T222);}goto _LL64;_LL64:;}
# 1099
if(fmt_arg_start==0)goto _TL1DE;_T223=c_vararg;if(_T223)goto _TL1DE;else{goto _TL1E0;}
# 1103
_TL1E0:{int problem;{struct _tuple16 _T3C5;
_T3C5.f0=ft;_T225=_check_null(cyc_vararg);_T226=_T225->type;_T227=Cyc_Tcutil_pointer_elt_type(_T226);_T3C5.f1=Cyc_Absyn_compress(_T227);_T224=_T3C5;}{struct _tuple16 _T3C5=_T224;struct Cyc_Absyn_Datatypedecl*_T3C6;_T228=_T3C5.f0;_T229=(int)_T228;switch(_T229){case Cyc_Absyn_Printf_ft: _T22A=_T3C5.f1;_T22B=(int*)_T22A;_T22C=*_T22B;if(_T22C!=0)goto _TL1E2;_T22D=_T3C5.f1;_T22E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22D;_T22F=_T22E->f1;_T230=(int*)_T22F;_T231=*_T230;if(_T231!=22)goto _TL1E4;_T232=_T3C5.f1;_T233=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T232;_T234=_T233->f1;_T235=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T234;_T236=_T235->f1;_T237=_T236.KnownDatatype;_T238=_T237.tag;if(_T238!=2)goto _TL1E6;_T239=_T3C5.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T239;_T23A=_T3C7->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T3C8=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T23A;_T23B=_T3C8->f1;_T23C=_T23B.KnownDatatype;_T23D=_T23C.val;{struct Cyc_Absyn_Datatypedecl*_T3C9=*_T23D;_T3C6=_T3C9;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T3C6;_T23E=tud;_T23F=_T23E->name;_T240=
# 1106
Cyc_Absyn_datatype_print_arg_qvar();_T241=Cyc_Absyn_qvar_cmp(_T23F,_T240);problem=_T241!=0;goto _LL69;}_TL1E6: goto _LL6E;_TL1E4: goto _LL6E;_TL1E2: goto _LL6E;case Cyc_Absyn_Scanf_ft: _T242=_T3C5.f1;_T243=(int*)_T242;_T244=*_T243;if(_T244!=0)goto _TL1E8;_T245=_T3C5.f1;_T246=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T245;_T247=_T246->f1;_T248=(int*)_T247;_T249=*_T248;if(_T249!=22)goto _TL1EA;_T24A=_T3C5.f1;_T24B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24A;_T24C=_T24B->f1;_T24D=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T24C;_T24E=_T24D->f1;_T24F=_T24E.KnownDatatype;_T250=_T24F.tag;if(_T250!=2)goto _TL1EC;_T251=_T3C5.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T251;_T252=_T3C7->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T3C8=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T252;_T253=_T3C8->f1;_T254=_T253.KnownDatatype;_T255=_T254.val;{struct Cyc_Absyn_Datatypedecl*_T3C9=*_T255;_T3C6=_T3C9;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T3C6;_T256=tud;_T257=_T256->name;_T258=
# 1109
Cyc_Absyn_datatype_scanf_arg_qvar();_T259=Cyc_Absyn_qvar_cmp(_T257,_T258);problem=_T259!=0;goto _LL69;}_TL1EC: goto _LL6E;_TL1EA: goto _LL6E;_TL1E8: goto _LL6E;default: _LL6E:
# 1112
 problem=1;goto _LL69;}_LL69:;}_T25A=problem;
# 1115
if(!_T25A)goto _TL1EE;{struct Cyc_Warn_String_Warn_Warg_struct _T3C5;_T3C5.tag=0;
_T3C5.f1=_tag_fat("format attribute and vararg types don't match",sizeof(char),46U);_T25B=_T3C5;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C5=_T25B;void*_T3C6[1];_T3C6[0]=& _T3C5;_T25C=loc;_T25D=_tag_fat(_T3C6,sizeof(void*),1);Cyc_Warn_err2(_T25C,_T25D);}goto _TL1EF;_TL1EE: _TL1EF:;}goto _TL1DF;_TL1DE: _TL1DF:;}}_TL1BF:;}goto _TL1BD;_TL1BC: _TL1BD: _T25E=qb;{
# 1121
struct Cyc_List_List*qbit=*_T25E;_TL1F3: if(qbit!=0)goto _TL1F1;else{goto _TL1F2;}
_TL1F1: _T25F=qbit;_T260=_T25F->hd;{struct _tuple0*_T3C3=(struct _tuple0*)_T260;void*_T3C4;void*_T3C5;{struct _tuple0 _T3C6=*_T3C3;_T3C5=_T3C6.f0;_T3C4=_T3C6.f1;}{void*tv=_T3C5;void*bnd=_T3C4;_T261=new_env;_T262=& Cyc_Kinds_aqk;_T263=(struct Cyc_Absyn_Kind*)_T262;_T264=bnd;
new_env=Cyc_Tctyp_i_check_type(_T261,_T263,_T264,0,0);_T265=new_env;_T266=& Cyc_Kinds_aqk;_T267=(struct Cyc_Absyn_Kind*)_T266;_T268=tv;
new_env=Cyc_Tctyp_i_check_type(_T265,_T267,_T268,0,0);}}_T269=qbit;
# 1121
qbit=_T269->tl;goto _TL1F3;_TL1F2:;}{
# 1127
struct Cyc_List_List*ec=effc;_TL1F7: if(ec!=0)goto _TL1F5;else{goto _TL1F6;}
_TL1F5: _T26A=ec;{void*_T3C3=_T26A->hd;void*_T3C4;void*_T3C5;_T26B=(int*)_T3C3;_T26C=*_T26B;switch(_T26C){case 0:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T3C6=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T3C3;_T26D=_T3C6->f1;_T3C5=(void*)_T26D;}{void*t=_T3C5;_T26E=new_env;_T26F=& Cyc_Kinds_ek;_T270=(struct Cyc_Absyn_Kind*)_T26F;_T271=t;
# 1130
new_env=Cyc_Tctyp_i_check_type(_T26E,_T270,_T271,1,1);goto _LL73;}case 1:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T3C6=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T3C3;_T272=_T3C6->f1;_T3C5=(void*)_T272;_T273=_T3C6->f2;_T3C4=(void*)_T273;}{void*t1=_T3C5;void*t2=_T3C4;_T3C5=t1;_T3C4=t2;goto _LL79;}default:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T3C6=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T3C3;_T274=_T3C6->f1;_T3C5=(void*)_T274;_T275=_T3C6->f2;_T3C4=(void*)_T275;}_LL79: {void*t1=_T3C5;void*t2=_T3C4;_T276=new_env;_T277=& Cyc_Kinds_ek;_T278=(struct Cyc_Absyn_Kind*)_T277;_T279=t1;
# 1134
new_env=Cyc_Tctyp_i_check_type(_T276,_T278,_T279,1,1);_T27A=new_env;_T27B=& Cyc_Kinds_ek;_T27C=(struct Cyc_Absyn_Kind*)_T27B;_T27D=t2;
new_env=Cyc_Tctyp_i_check_type(_T27A,_T27C,_T27D,1,1);goto _LL73;}}_LL73:;}_T27E=ec;
# 1127
ec=_T27E->tl;goto _TL1F7;_TL1F6:;}{
# 1143
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_bogus_fenv(tr,args);_T27F=new_env;{
struct Cyc_Tcenv_Tenv*old_te=_T27F.te;{struct Cyc_Tcenv_Tenv*_T3C3=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T281=te;
_T3C3->ns=_T281->ns;_T282=te;_T3C3->ae=_T282->ae;_T3C3->le=fenv;_T3C3->allow_valueof=1;_T283=te;
_T3C3->in_extern_c_include=_T283->in_extern_c_include;_T284=te;
_T3C3->in_tempest=_T284->in_tempest;_T285=te;_T3C3->tempest_generalize=_T285->tempest_generalize;_T286=te;
_T3C3->in_extern_c_inc_repeat=_T286->in_extern_c_inc_repeat;_T280=(struct Cyc_Tcenv_Tenv*)_T3C3;}
# 1145
new_env.te=_T280;{
# 1149
struct Cyc_List_List*vds;vds=_cycalloc(sizeof(struct Cyc_List_List));_T287=vds;_T287->hd=Cyc_AssnDef_memory;_T288=vds;_T288->tl=arg_vds;
if(chk==0)goto _TL1F9;_T289=c_vararg;
if(_T289)goto _TL1FD;else{goto _TL1FE;}_TL1FE: _T28A=cyc_vararg;_T28B=(unsigned)_T28A;if(_T28B)goto _TL1FD;else{goto _TL1FB;}
_TL1FD:{struct Cyc_Warn_String_Warn_Warg_struct _T3C3;_T3C3.tag=0;_T3C3.f1=_tag_fat("we do not yet support @checks on vararg functions",sizeof(char),50U);_T28C=_T3C3;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C3=_T28C;void*_T3C4[1];_T3C4[0]=& _T3C3;_T28D=loc;_T28E=_tag_fat(_T3C4,sizeof(void*),1);Cyc_Warn_err2(_T28D,_T28E);}goto _TL1FC;_TL1FB: _TL1FC: _T28F=new_env;_T290=
_tag_fat("@checks",sizeof(char),8U);_T291=chk;{struct _tuple15 _T3C3=Cyc_Tctyp_check_clause(_T28F,_T290,_T291);struct Cyc_AssnDef_AssnMap _T3C4;struct Cyc_Tctyp_CVTEnv _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3C5;struct Cyc_AssnDef_AssnMap chk_assn=_T3C4;
new_env=nenv;_T292=chk_relns;
*_T292=Cyc_AssnDef_assnmap2existassnfn(chk_assn,vds);}}goto _TL1FA;_TL1F9: _TL1FA:
# 1157
 if(req==0)goto _TL1FF;_T293=new_env;_T294=
_tag_fat("@requires",sizeof(char),10U);_T295=req;{struct _tuple15 _T3C3=Cyc_Tctyp_check_clause(_T293,_T294,_T295);struct Cyc_AssnDef_AssnMap _T3C4;struct Cyc_Tctyp_CVTEnv _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3C5;struct Cyc_AssnDef_AssnMap req_assn=_T3C4;
new_env=nenv;_T296=req_relns;
*_T296=Cyc_AssnDef_assnmap2existassnfn(req_assn,vds);}}goto _TL200;_TL1FF: _TL200: {
# 1163
struct Cyc_List_List*vds;vds=_cycalloc(sizeof(struct Cyc_List_List));_T297=vds;_T297->hd=Cyc_AssnDef_memory;_T298=vds;{struct Cyc_List_List*_T3C3=_cycalloc(sizeof(struct Cyc_List_List));_T3C3->hd=Cyc_AssnDef_pre_memory;_T3C3->tl=arg_vds;_T299=(struct Cyc_List_List*)_T3C3;}_T298->tl=_T299;
if(ens==0)goto _TL201;_T29A=new_env;_T29B=
_tag_fat("@ensures",sizeof(char),9U);_T29C=ens;{struct _tuple15 _T3C3=Cyc_Tctyp_check_clause(_T29A,_T29B,_T29C);struct Cyc_AssnDef_AssnMap _T3C4;struct Cyc_Tctyp_CVTEnv _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3C5;struct Cyc_AssnDef_AssnMap ens_assn=_T3C4;
new_env=nenv;_T29D=ens_relns;_T29E=ens_assn;{struct Cyc_List_List*_T3C6=_cycalloc(sizeof(struct Cyc_List_List));
_T3C6->hd=return_vd;_T3C6->tl=vds;_T29F=(struct Cyc_List_List*)_T3C6;}*_T29D=Cyc_AssnDef_assnmap2existassnfn(_T29E,_T29F);}}goto _TL202;_TL201: _TL202:
# 1169
 if(thrws==0)goto _TL203;_T2A0=new_env;_T2A1=
_tag_fat("@throws",sizeof(char),8U);_T2A2=thrws;{struct _tuple15 _T3C3=Cyc_Tctyp_check_clause(_T2A0,_T2A1,_T2A2);struct Cyc_AssnDef_AssnMap _T3C4;struct Cyc_Tctyp_CVTEnv _T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Tctyp_CVTEnv nenv=_T3C5;struct Cyc_AssnDef_AssnMap thrws_assn=_T3C4;
new_env=nenv;_T2A3=thrws_relns;
*_T2A3=Cyc_AssnDef_assnmap2existassnfn(thrws_assn,vds);}}goto _TL204;_TL203: _TL204:
# 1174
 new_env.te=old_te;_T2A4=eff;_T2A5=*_T2A4;
# 1176
if(_T2A5==0)goto _TL205;_T2A6=new_env;_T2A7=& Cyc_Kinds_ek;_T2A8=(struct Cyc_Absyn_Kind*)_T2A7;_T2A9=eff;_T2AA=*_T2A9;
new_env=Cyc_Tctyp_i_check_type(_T2A6,_T2A8,_T2AA,1,1);_T2AB=Cyc_Absyn_heap_rgn_type;_T2AC=eff;_T2AD=*_T2AC;_T2AE=
_check_null(_T2AD);_T2AF=Cyc_Tcutil_type_in_effect(0,_T2AB,_T2AE);if(_T2AF)goto _TL207;else{goto _TL209;}
_TL209: _T2B0=eff;{void*_T3C3[2];_T3C3[0]=Cyc_Absyn_heap_rgn_type;_T2B2=eff;_T2B3=*_T2B2;_T3C3[1]=_check_null(_T2B3);_T2B4=_tag_fat(_T3C3,sizeof(void*),2);_T2B1=Cyc_List_list(_T2B4);}_T2B5=Cyc_Absyn_join_eff(_T2B1);*_T2B0=Cyc_Tcutil_normalize_effect(_T2B5);goto _TL208;_TL207: _TL208: goto _TL206;
# 1183
_TL205:{struct Cyc_List_List*effect;effect=_cycalloc(sizeof(struct Cyc_List_List));_T2B6=effect;_T2B6->hd=Cyc_Absyn_heap_rgn_type;_T2B7=effect;_T2B7->tl=0;_T2B8=new_env;{
# 1188
struct Cyc_List_List*tvs=_T2B8.free_vars;_TL20D: if(tvs!=0)goto _TL20B;else{goto _TL20C;}
_TL20B: _T2B9=tvs;_T2BA=_T2B9->hd;_T2BB=(struct _tuple12*)_T2BA;{struct _tuple12 _T3C3=*_T2BB;int _T3C4;struct Cyc_Absyn_Tvar*_T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Absyn_Tvar*tv=_T3C5;int put_in_eff=_T3C4;_T2BC=put_in_eff;
if(_T2BC)goto _TL20E;else{goto _TL210;}_TL210: goto _TL20A;_TL20E: _T2BD=tv;_T2BE=_T2BD->kind;{
void*_T3C6=Cyc_Kinds_compress_kb(_T2BE);void*_T3C7;_T2BF=(int*)_T3C6;_T2C0=*_T2BF;switch(_T2C0){case 2: _T2C1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C6;_T2C2=_T2C1->f2;_T2C3=(struct Cyc_Absyn_Kind*)_T2C2;_T2C4=_T2C3->kind;_T2C5=(int)_T2C4;switch(_T2C5){case Cyc_Absyn_BoolKind: goto _LL8D;case Cyc_Absyn_PtrBndKind: _LL8D: goto _LL8F;case Cyc_Absyn_IntKind: _LL8F: goto _LL91;case Cyc_Absyn_AqualKind: _LL91: goto _LL93;case Cyc_Absyn_EffKind:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C6;_T2C6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C6;_T2C7=& _T2C6->f1;_T3C7=(struct Cyc_Core_Opt**)_T2C7;}{struct Cyc_Core_Opt**f=_T3C7;_T2C8=f;_T2C9=& Cyc_Kinds_ek;_T2CA=(struct Cyc_Absyn_Kind*)_T2C9;
# 1201
*_T2C8=Cyc_Kinds_kind_to_bound_opt(_T2CA);goto _LL9D;}default: goto _LLA0;};case 0: _T2CB=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3C6;_T2CC=_T2CB->f1;_T2CD=(struct Cyc_Absyn_Kind*)_T2CC;_T2CE=_T2CD->kind;_T2CF=(int)_T2CE;switch(_T2CF){case Cyc_Absyn_BoolKind: _LL93: goto _LL95;case Cyc_Absyn_PtrBndKind: _LL95: goto _LL97;case Cyc_Absyn_IntKind: _LL97: goto _LL99;case Cyc_Absyn_AqualKind: _LL99: goto _LL89;case Cyc_Absyn_EffKind: _LL9D:{struct Cyc_List_List*_T3C8=_cycalloc(sizeof(struct Cyc_List_List));
# 1203
_T3C8->hd=Cyc_Absyn_var_type(tv);_T3C8->tl=effect;_T2D0=(struct Cyc_List_List*)_T3C8;}effect=_T2D0;goto _LL89;default: _LLA0: _LLA1:{struct Cyc_List_List*_T3C8=_cycalloc(sizeof(struct Cyc_List_List));_T2D2=
# 1208
Cyc_Absyn_var_type(tv);_T3C8->hd=Cyc_Absyn_regionsof_eff(_T2D2);_T3C8->tl=effect;_T2D1=(struct Cyc_List_List*)_T3C8;}effect=_T2D1;goto _LL89;};default:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3C8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C6;_T2D3=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C6;_T2D4=& _T2D3->f1;_T3C7=(struct Cyc_Core_Opt**)_T2D4;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3C7;_T2D5=f;{struct Cyc_Core_Opt*_T3C8=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C9=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T3C9->tag=2;
# 1205
_T3C9->f1=0;_T2D8=& Cyc_Kinds_ak;_T3C9->f2=(struct Cyc_Absyn_Kind*)_T2D8;_T2D7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C9;}_T3C8->v=(void*)_T2D7;_T2D6=(struct Cyc_Core_Opt*)_T3C8;}*_T2D5=_T2D6;goto _LLA1;}}_LL89:;}}}_TL20A: _T2D9=tvs;
# 1188
tvs=_T2D9->tl;goto _TL20D;_TL20C:;}_T2DA=new_env;{
# 1212
struct Cyc_List_List*ts=_T2DA.free_evars;_TL217: if(ts!=0)goto _TL215;else{goto _TL216;}
_TL215: _T2DB=ts;_T2DC=_T2DB->hd;_T2DD=(struct _tuple14*)_T2DC;{struct _tuple14 _T3C3=*_T2DD;int _T3C4;void*_T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{void*tv=_T3C5;int put_in_eff=_T3C4;_T2DE=put_in_eff;
if(_T2DE)goto _TL218;else{goto _TL21A;}_TL21A: goto _TL214;_TL218: {
struct Cyc_Absyn_Kind*_T3C6=Cyc_Tcutil_type_kind(tv);_T2DF=(struct Cyc_Absyn_Kind*)_T3C6;_T2E0=_T2DF->kind;_T2E1=(int)_T2E0;switch(_T2E1){case Cyc_Absyn_IntKind: goto _LLA9;case Cyc_Absyn_BoolKind: _LLA9: goto _LLAB;case Cyc_Absyn_PtrBndKind: _LLAB: goto _LLAD;case Cyc_Absyn_AqualKind: _LLAD: goto _LLA5;case Cyc_Absyn_EffKind:{struct Cyc_List_List*_T3C7=_cycalloc(sizeof(struct Cyc_List_List));
# 1222
_T3C7->hd=tv;_T3C7->tl=effect;_T2E2=(struct Cyc_List_List*)_T3C7;}effect=_T2E2;goto _LLA5;default:{struct Cyc_List_List*_T3C7=_cycalloc(sizeof(struct Cyc_List_List));
_T3C7->hd=Cyc_Absyn_regionsof_eff(tv);_T3C7->tl=effect;_T2E3=(struct Cyc_List_List*)_T3C7;}effect=_T2E3;goto _LLA5;}_LLA5:;}}}_TL214: _T2E4=ts;
# 1212
ts=_T2E4->tl;goto _TL217;_TL216:;}_T2E5=eff;
# 1226
*_T2E5=Cyc_Absyn_join_eff(effect);}_TL206: _T2E6=btvs;_T2E7=*_T2E6;
# 1232
if(_T2E7==0)goto _TL21C;_T2E8=btvs;{
struct Cyc_List_List*bs=*_T2E8;_TL221: if(bs!=0)goto _TL21F;else{goto _TL220;}
_TL21F: _T2E9=bs;_T2EA=_T2E9->hd;_T2EB=(struct Cyc_Absyn_Tvar*)_T2EA;_T2EC=_T2EB->kind;{void*_T3C3=Cyc_Kinds_compress_kb(_T2EC);struct Cyc_Absyn_Kind*_T3C4;void*_T3C5;_T2ED=(int*)_T3C3;_T2EE=*_T2ED;switch(_T2EE){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3C6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C3;_T2EF=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3C3;_T2F0=& _T2EF->f1;_T3C5=(struct Cyc_Core_Opt**)_T2F0;}{struct Cyc_Core_Opt**f=_T3C5;{struct Cyc_Warn_String_Warn_Warg_struct _T3C6;_T3C6.tag=0;
# 1236
_T3C6.f1=_tag_fat("type variable ",sizeof(char),15U);_T2F1=_T3C6;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C6=_T2F1;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3C7;_T3C7.tag=7;_T2F3=bs;_T2F4=_T2F3->hd;_T3C7.f1=(struct Cyc_Absyn_Tvar*)_T2F4;_T2F2=_T3C7;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T3C7=_T2F2;{struct Cyc_Warn_String_Warn_Warg_struct _T3C8;_T3C8.tag=0;_T3C8.f1=_tag_fat(" unconstrained, assuming boxed",sizeof(char),31U);_T2F5=_T3C8;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C8=_T2F5;void*_T3C9[3];_T3C9[0]=& _T3C6;_T3C9[1]=& _T3C7;_T3C9[2]=& _T3C8;_T2F6=loc;_T2F7=_tag_fat(_T3C9,sizeof(void*),3);Cyc_Warn_warn2(_T2F6,_T2F7);}}}_T2F8=f;_T2F9=& Cyc_Kinds_bk;_T2FA=(struct Cyc_Absyn_Kind*)_T2F9;
*_T2F8=Cyc_Kinds_kind_to_bound_opt(_T2FA);goto _LLB2;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3C6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C3;_T2FB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3C3;_T2FC=& _T2FB->f1;_T3C5=(struct Cyc_Core_Opt**)_T2FC;_T3C4=_T3C6->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3C5;struct Cyc_Absyn_Kind*k=_T3C4;_T2FD=k;_T2FE=(struct Cyc_Absyn_Kind*)_T2FD;_T2FF=_T2FE->kind;_T300=(int)_T2FF;switch(_T300){case Cyc_Absyn_BoxKind: goto _LLBF;case Cyc_Absyn_MemKind: _LLBF: goto _LLC1;case Cyc_Absyn_AnyKind: _LLC1: _T301=f;_T302=& Cyc_Kinds_bk;_T303=(struct Cyc_Absyn_Kind*)_T302;
# 1244
*_T301=Cyc_Kinds_kind_to_bound_opt(_T303);goto _LLBB;case Cyc_Absyn_EffKind: _T304=f;_T305=& Cyc_Kinds_ek;_T306=(struct Cyc_Absyn_Kind*)_T305;
# 1246
*_T304=Cyc_Kinds_kind_to_bound_opt(_T306);goto _LLBB;default: _T307=f;
# 1248
*_T307=Cyc_Kinds_kind_to_bound_opt(k);goto _LLBB;}_LLBB: goto _LLB2;}default: _T308=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3C3;_T309=_T308->f1;_T30A=(struct Cyc_Absyn_Kind*)_T309;_T30B=_T30A->kind;if(_T30B!=Cyc_Absyn_MemKind)goto _TL224;{struct Cyc_Warn_String_Warn_Warg_struct _T3C6;_T3C6.tag=0;
# 1252
_T3C6.f1=_tag_fat("functions can't abstract M types",sizeof(char),33U);_T30C=_T3C6;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C6=_T30C;void*_T3C7[1];_T3C7[0]=& _T3C6;_T30D=loc;_T30E=_tag_fat(_T3C7,sizeof(void*),1);Cyc_Warn_err2(_T30D,_T30E);}goto _LLB2;_TL224: goto _LLB2;}_LLB2:;}_T30F=bs;
# 1233
bs=_T30F->tl;goto _TL221;_TL220:;}goto _TL21D;_TL21C: _TL21D:{
# 1256
struct Cyc_List_List*(*_T3C3)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*)))Cyc_Tctyp_remove_bound_tvars;_T310=_T3C3;}_T311=new_env;_T312=_T311.kind_env;_T313=btvs;_T314=*_T313;_T316=Cyc_Core_identity;{struct Cyc_Absyn_Tvar*(*_T3C3)(struct Cyc_Absyn_Tvar*)=(struct Cyc_Absyn_Tvar*(*)(struct Cyc_Absyn_Tvar*))_T316;_T315=_T3C3;}env.kind_env=_T310(_T312,_T314,_T315);{
struct Cyc_List_List*(*_T3C3)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T317=_T3C3;}_T318=new_env;_T319=_T318.free_vars;_T31A=btvs;_T31B=*_T31A;_T31D=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T3C3)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T31D;_T31C=_T3C3;}new_env.free_vars=_T317(_T319,_T31B,_T31C);_T31E=new_env;{
# 1259
struct Cyc_List_List*tvs=_T31E.free_vars;_TL229: if(tvs!=0)goto _TL227;else{goto _TL228;}
_TL227: _T31F=tvs;_T320=_T31F->hd;_T321=(struct _tuple12*)_T320;{struct _tuple12 _T3C3=*_T321;int _T3C4;struct Cyc_Absyn_Tvar*_T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{struct Cyc_Absyn_Tvar*t=_T3C5;int b=_T3C4;_T322=env;_T323=_T322.free_vars;_T324=t;_T325=b;
env.free_vars=Cyc_Tctyp_fast_add_free_tvar_bool(_T323,_T324,_T325);}}_T326=tvs;
# 1259
tvs=_T326->tl;goto _TL229;_TL228:;}_T327=new_env;{
# 1264
struct Cyc_List_List*evs=_T327.free_evars;_TL22D: if(evs!=0)goto _TL22B;else{goto _TL22C;}
_TL22B: _T328=evs;_T329=_T328->hd;_T32A=(struct _tuple14*)_T329;{struct _tuple14 _T3C3=*_T32A;int _T3C4;void*_T3C5;_T3C5=_T3C3.f0;_T3C4=_T3C3.f1;{void*e=_T3C5;int b=_T3C4;_T32B=env;_T32C=_T32B.free_evars;_T32D=e;_T32E=b;
env.free_evars=Cyc_Tctyp_add_free_evar(_T32C,_T32D,_T32E);}}_T32F=evs;
# 1264
evs=_T32F->tl;goto _TL22D;_TL22C:;}_T330=env;_T331=_T330.free_qualvars;_T332=(unsigned)_T331;
# 1268
if(!_T332)goto _TL22E;_T333=new_env;{
# 1270
struct Cyc_List_List*it=_T333.free_qualvars;_TL233: if(it!=0)goto _TL231;else{goto _TL232;}
_TL231: _T335=Cyc_List_exists_c;{int(*_T3C3)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*)=(int(*)(int(*)(struct _tuple14*,struct _tuple14*),struct _tuple14*,struct Cyc_List_List*))_T335;_T334=_T3C3;}_T336=it;_T337=_T336->hd;_T338=(struct _tuple14*)_T337;_T339=env;_T33A=_T339.free_qualvars;_T33B=_T334(Cyc_Tctyp_cmp_fqv,_T338,_T33A);if(_T33B)goto _TL234;else{goto _TL236;}
_TL236:{struct Cyc_List_List*_T3C3=_cycalloc(sizeof(struct Cyc_List_List));_T33D=it;_T33E=_T33D->hd;_T3C3->hd=(struct _tuple14*)_T33E;_T33F=env;_T3C3->tl=_T33F.free_qualvars;_T33C=(struct Cyc_List_List*)_T3C3;}env.free_qualvars=_T33C;goto _TL235;_TL234: _TL235: _T340=it;
# 1270
it=_T340->tl;goto _TL233;_TL232:;}goto _TL22F;
# 1276
_TL22E: _T341=new_env;env.free_qualvars=_T341.free_qualvars;_TL22F: goto _LL3;}}}}}}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T3AA;_T3AC=_T3C3->f1;_T3B7=_T3C3->f2;_T3C2=_T3C3->f3;}{enum Cyc_Absyn_AggrKind k=_T3AC;int is_tuple=_T3B7;struct Cyc_List_List*fs=_T3C2;
# 1281
struct Cyc_List_List*prev_fields=0;
_TL23A: if(fs!=0)goto _TL238;else{goto _TL239;}
_TL238: _T342=fs;_T343=_T342->hd;{struct Cyc_Absyn_Aggrfield*_T3C3=(struct Cyc_Absyn_Aggrfield*)_T343;struct Cyc_Absyn_Exp*_T3C4;struct Cyc_List_List*_T3C5;struct Cyc_Absyn_Exp*_T3C6;void*_T3C7;void*_T3C8;struct _fat_ptr*_T3C9;{struct Cyc_Absyn_Aggrfield _T3CA=*_T3C3;_T3C9=_T3CA.name;_T344=& _T3C3->tq;_T3C8=(struct Cyc_Absyn_Tqual*)_T344;_T3C7=_T3CA.type;_T3C6=_T3CA.width;_T3C5=_T3CA.attributes;_T3C4=_T3CA.requires_clause;}{struct _fat_ptr*fn=_T3C9;struct Cyc_Absyn_Tqual*tqp=(struct Cyc_Absyn_Tqual*)_T3C8;void*t=_T3C7;struct Cyc_Absyn_Exp*width=_T3C6;struct Cyc_List_List*atts=_T3C5;struct Cyc_Absyn_Exp*requires_clause=_T3C4;_T346=Cyc_List_mem;{
int(*_T3CA)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T346;_T345=_T3CA;}_T347=Cyc_strptrcmp;_T348=prev_fields;_T349=fn;_T34A=_T345(_T347,_T348,_T349);if(!_T34A)goto _TL23B;{struct Cyc_Warn_String_Warn_Warg_struct _T3CA;_T3CA.tag=0;
_T3CA.f1=_tag_fat("duplicate field ",sizeof(char),17U);_T34B=_T3CA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CA=_T34B;{struct Cyc_Warn_String_Warn_Warg_struct _T3CB;_T3CB.tag=0;_T34D=fn;_T3CB.f1=*_T34D;_T34C=_T3CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CB=_T34C;void*_T3CC[2];_T3CC[0]=& _T3CA;_T3CC[1]=& _T3CB;_T34E=loc;_T34F=_tag_fat(_T3CC,sizeof(void*),2);Cyc_Warn_err2(_T34E,_T34F);}}goto _TL23C;_TL23B: _TL23C: _T350=fn;_T351=*_T350;_T352=
_tag_fat("",sizeof(char),1U);_T353=Cyc_strcmp(_T351,_T352);if(_T353==0)goto _TL23D;{struct Cyc_List_List*_T3CA=_cycalloc(sizeof(struct Cyc_List_List));
_T3CA->hd=fn;_T3CA->tl=prev_fields;_T354=(struct Cyc_List_List*)_T3CA;}prev_fields=_T354;goto _TL23E;_TL23D: _TL23E: _T355=env;_T356=& Cyc_Kinds_mk;_T357=(struct Cyc_Absyn_Kind*)_T356;_T358=t;
env=Cyc_Tctyp_i_check_type(_T355,_T357,_T358,1,0);_T359=tqp;_T35A=loc;_T35B=tqp;_T35C=_T35B->print_const;_T35D=t;
_T359->real_const=Cyc_Tcutil_extract_const_from_typedef(_T35A,_T35C,_T35D);
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
Cyc_Atts_check_field_atts(loc,fn,atts);
if(requires_clause==0)goto _TL23F;_T35E=k;_T35F=(int)_T35E;
# 1294
if(_T35F==1)goto _TL241;{struct Cyc_Warn_String_Warn_Warg_struct _T3CA;_T3CA.tag=0;
_T3CA.f1=_tag_fat("@requires clause is only allowed on union members",sizeof(char),50U);_T360=_T3CA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CA=_T360;void*_T3CB[1];_T3CB[0]=& _T3CA;_T361=loc;_T362=_tag_fat(_T3CB,sizeof(void*),1);Cyc_Warn_err2(_T361,_T362);}goto _TL242;_TL241: _TL242: _T363=
Cyc_Tcenv_allow_valueof(te);_T364=requires_clause;Cyc_Tcexp_tcExp(_T363,0,_T364);_T365=
Cyc_Tcutil_is_integral(requires_clause);if(_T365)goto _TL243;else{goto _TL245;}
_TL245:{struct Cyc_Warn_String_Warn_Warg_struct _T3CA;_T3CA.tag=0;_T3CA.f1=_tag_fat("@requires clause has type ",sizeof(char),27U);_T366=_T3CA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CA=_T366;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3CB;_T3CB.tag=3;_T368=requires_clause;_T3CB.f1=_T368->topt;_T367=_T3CB;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T3CB=_T367;{struct Cyc_Warn_String_Warn_Warg_struct _T3CC;_T3CC.tag=0;
_T3CC.f1=_tag_fat(" instead of integral type",sizeof(char),26U);_T369=_T3CC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3CC=_T369;void*_T3CD[3];_T3CD[0]=& _T3CA;_T3CD[1]=& _T3CB;_T3CD[2]=& _T3CC;_T36A=loc;_T36B=_tag_fat(_T3CD,sizeof(void*),3);Cyc_Warn_err2(_T36A,_T36B);}}}goto _TL244;_TL243: _TL244:
 env=Cyc_Tctyp_i_check_type_level_exp(requires_clause,env);goto _TL240;_TL23F: _TL240:;}}_T36C=fs;
# 1282
fs=_T36C->tl;goto _TL23A;_TL239: goto _LL3;}default:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T3C3=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3AA;_T3AB=_T3C3->f1;_T36D=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3AA;_T36E=& _T36D->f2;_T3C2=(struct Cyc_List_List**)_T36E;_T36F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3AA;_T370=& _T36F->f3;_T3C1=(struct Cyc_Absyn_Typedefdecl**)_T370;_T371=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3AA;_T372=& _T371->f4;_T3BF=(void**)_T372;}{struct _tuple1*tdn=_T3AB;struct Cyc_List_List**targs_ref=(struct Cyc_List_List**)_T3C2;struct Cyc_Absyn_Typedefdecl**tdopt=(struct Cyc_Absyn_Typedefdecl**)_T3C1;void**toptp=(void**)_T3BF;
# 1307
struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _T3C3;_T373=& _T3C3;_push_handler(_T373);{int _T3C4=0;_T374=_T3C3.handler;_T375=setjmp(_T374);if(!_T375)goto _TL246;_T3C4=1;goto _TL247;_TL246: _TL247: if(_T3C4)goto _TL248;else{goto _TL24A;}_TL24A:
 td=Cyc_Tcenv_lookup_typedefdecl(te,loc,tdn);_pop_handler();goto _TL249;_TL248: _T376=Cyc_Core_get_exn_thrown();{void*_T3C5=(void*)_T376;void*_T3C6;_T377=(struct Cyc_Dict_Absent_exn_struct*)_T3C5;_T378=_T377->tag;_T379=Cyc_Dict_Absent;if(_T378!=_T379)goto _TL24B;{struct Cyc_Warn_String_Warn_Warg_struct _T3C7;_T3C7.tag=0;
# 1311
_T3C7.f1=_tag_fat("unbound typedef name ",sizeof(char),22U);_T37A=_T3C7;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C7=_T37A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3C8;_T3C8.tag=1;_T3C8.f1=tdn;_T37B=_T3C8;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3C8=_T37B;void*_T3C9[2];_T3C9[0]=& _T3C7;_T3C9[1]=& _T3C8;_T37C=loc;_T37D=_tag_fat(_T3C9,sizeof(void*),2);Cyc_Warn_err2(_T37C,_T37D);}}_T37E=env;
return _T37E;_TL24B: _T3C6=_T3C5;{void*exn=_T3C6;_rethrow(exn);};}_TL249:;}}_T37F=tdopt;
# 1314
*_T37F=td;_T380=env;_T381=targs_ref;_T382=td;_T383=_T382->tvs;_T384=expected_kind;_T385=allow_abs_aggr;_T386=tdn;_T387=td;
env=Cyc_Tctyp_check_type_inst(_T380,_T381,_T383,_T384,_T385,_T386,_T387);_T388=td;_T389=_T388->defn;
# 1317
if(_T389==0)goto _TL24D;_T38A=loc;_T38B=td;_T38C=_T38B->defn;_T38D=expected_kind;
Cyc_Tctyp_check_constrain_ptrbnd(_T38A,_T38C,_T38D);{
struct Cyc_List_List*inst=0;_T38E=td;{
struct Cyc_List_List*tvs=_T38E->tvs;_T38F=targs_ref;{
struct Cyc_List_List*ts=*_T38F;
_TL252: if(tvs!=0)goto _TL250;else{goto _TL251;}
_TL250:{struct Cyc_List_List*_T3C3=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T3C4=_cycalloc(sizeof(struct _tuple17));_T392=tvs;_T393=_T392->hd;_T3C4->f0=(struct Cyc_Absyn_Tvar*)_T393;_T394=_check_null(ts);_T3C4->f1=_T394->hd;_T391=(struct _tuple17*)_T3C4;}_T3C3->hd=_T391;_T3C3->tl=inst;_T390=(struct Cyc_List_List*)_T3C3;}inst=_T390;_T395=tvs;
# 1322
tvs=_T395->tl;_T396=ts;ts=_T396->tl;goto _TL252;_TL251: _T397=toptp;_T398=inst;_T399=td;_T39A=_T399->defn;_T39B=
# 1324
_check_null(_T39A);*_T397=Cyc_Tcutil_substitute(_T398,_T39B);}}}goto _TL24E;_TL24D: _TL24E: goto _LL3;}}_LL3:;}_T39C=
# 1328
Cyc_Tcutil_type_kind(t);_T39D=expected_kind;_T39E=Cyc_Kinds_kind_leq(_T39C,_T39D);if(_T39E)goto _TL253;else{goto _TL255;}
_TL255:{struct Cyc_Warn_String_Warn_Warg_struct _T3AA;_T3AA.tag=0;_T3AA.f1=_tag_fat("type ",sizeof(char),6U);_T39F=_T3AA;}{struct Cyc_Warn_String_Warn_Warg_struct _T3AA=_T39F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3AB;_T3AB.tag=2;_T3AB.f1=t;_T3A0=_T3AB;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3AB=_T3A0;{struct Cyc_Warn_String_Warn_Warg_struct _T3AC;_T3AC.tag=0;_T3AC.f1=_tag_fat(" has kind ",sizeof(char),11U);_T3A1=_T3AC;}{struct Cyc_Warn_String_Warn_Warg_struct _T3AC=_T3A1;{struct Cyc_Warn_Kind_Warn_Warg_struct _T3AD;_T3AD.tag=9;_T3AD.f1=Cyc_Tcutil_type_kind(t);_T3A2=_T3AD;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T3AD=_T3A2;{struct Cyc_Warn_String_Warn_Warg_struct _T3AE;_T3AE.tag=0;
_T3AE.f1=_tag_fat(" but as used here needs kind ",sizeof(char),30U);_T3A3=_T3AE;}{struct Cyc_Warn_String_Warn_Warg_struct _T3AE=_T3A3;{struct Cyc_Warn_Kind_Warn_Warg_struct _T3AF;_T3AF.tag=9;_T3AF.f1=expected_kind;_T3A4=_T3AF;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T3AF=_T3A4;void*_T3B0[6];_T3B0[0]=& _T3AA;_T3B0[1]=& _T3AB;_T3B0[2]=& _T3AC;_T3B0[3]=& _T3AD;_T3B0[4]=& _T3AE;_T3B0[5]=& _T3AF;_T3A5=loc;_T3A6=_tag_fat(_T3B0,sizeof(void*),6);Cyc_Warn_err2(_T3A5,_T3A6);}}}}}}goto _TL254;_TL253: _TL254: _T3A7=env;
# 1332
return _T3A7;}}
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
void Cyc_Tctyp_check_valid_toplevel_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*t){int _T0;int _T1;int _T2;struct Cyc_Tcenv_Tenv*_T3;int _T4;struct Cyc_Tcenv_Tenv*_T5;struct Cyc_Absyn_Kind*_T6;int _T7;struct Cyc_Absyn_Kind*_T8;struct Cyc_Absyn_Kind*_T9;struct Cyc_Tctyp_CVTEnv _TA;struct Cyc_Absyn_Kind*_TB;void*_TC;struct Cyc_List_List*(*_TD)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_TE)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TF)(struct _tuple12*);void*(*_T10)(struct _tuple0*);struct Cyc_Tctyp_CVTEnv _T11;struct Cyc_List_List*_T12;struct Cyc_Tctyp_CVTEnv _T13;struct Cyc_Tctyp_CVTEnv _T14;struct Cyc_List_List*_T15;unsigned _T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Tvar*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_Absyn_Tvar*_T1C;int _T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Absyn_Tvar*_T25;void*_T26;int*_T27;unsigned _T28;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T29;struct Cyc_Core_Opt**_T2A;struct Cyc_Core_Opt**_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2E;struct Cyc_Core_Opt**_T2F;struct Cyc_Absyn_Kind*_T30;struct Cyc_Absyn_Kind*_T31;enum Cyc_Absyn_KindQual _T32;int _T33;struct Cyc_Core_Opt**_T34;struct Cyc_Absyn_Kind*_T35;struct Cyc_Absyn_Kind*_T36;struct Cyc_Core_Opt**_T37;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T38;struct Cyc_Absyn_Kind*_T39;struct Cyc_Absyn_Kind*_T3A;enum Cyc_Absyn_KindQual _T3B;struct Cyc_Absyn_Kind*_T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_Warn_String_Warn_Warg_struct _T41;struct Cyc_Warn_Kind_Warn_Warg_struct _T42;struct Cyc_Absyn_Kind*_T43;unsigned _T44;struct _fat_ptr _T45;struct Cyc_List_List*_T46;int*_T47;int _T48;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T49;struct Cyc_List_List**_T4A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4B;struct Cyc_List_List**_T4C;unsigned _T4D;struct Cyc_Tctyp_CVTEnv _T4E;struct Cyc_List_List*(*_T4F)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T50)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T51)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List**_T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;struct Cyc_List_List**_T55;void*_T56;struct Cyc_List_List**_T57;struct Cyc_List_List*_T58;struct Cyc_List_List**_T59;struct Cyc_List_List*(*_T5A)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);struct Cyc_List_List*(*_T5B)(int(*)(void*),struct Cyc_List_List*);int(*_T5C)(struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T5D;struct Cyc_Warn_String_Warn_Warg_struct _T5E;struct Cyc_Warn_Tvar_Warn_Warg_struct _T5F;struct Cyc_List_List*_T60;void*_T61;struct Cyc_Warn_String_Warn_Warg_struct _T62;struct Cyc_Warn_Typ_Warn_Warg_struct _T63;unsigned _T64;struct _fat_ptr _T65;int _T66;struct Cyc_Tcenv_Tenv*_T67;int _T68;int*_T69;int _T6A;struct Cyc_Absyn_FnInfo _T6B;int(*_T6C)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);int(*_T6D)(int(*)(void*),struct Cyc_List_List*);int(*_T6E)(struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T6F;int _T70;struct Cyc_Warn_String_Warn_Warg_struct _T71;struct Cyc_Warn_String_Warn_Warg_struct _T72;struct Cyc_Warn_String_Warn_Warg_struct _T73;struct Cyc_Warn_Typ_Warn_Warg_struct _T74;unsigned _T75;struct _fat_ptr _T76;_T1=
# 1373
Cyc_Tcutil_is_function_type(t);if(!_T1)goto _TL268;_T3=te;_T4=_T3->in_tempest;if(_T4)goto _TL26A;else{goto _TL26C;}_TL26C: _T2=1;goto _TL26B;_TL26A: _T5=te;_T2=_T5->tempest_generalize;_TL26B: _T0=_T2;goto _TL269;_TL268: _T0=0;_TL269: {
# 1372
int generalize_evars=_T0;_T7=
# 1374
Cyc_Tcutil_is_function_type(t);if(!_T7)goto _TL26D;_T8=& Cyc_Kinds_ak;_T6=(struct Cyc_Absyn_Kind*)_T8;goto _TL26E;_TL26D: _T9=& Cyc_Kinds_mk;_T6=(struct Cyc_Absyn_Kind*)_T9;_TL26E: {struct Cyc_Absyn_Kind*expected_kind=_T6;
struct Cyc_List_List*te_vars=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Tctyp_CVTEnv _T77;
_T77.loc=loc;_T77.te=te;
_T77.kind_env=te_vars;_T77.fn_result=0;_T77.def_aliasable=0;_T77.inst_type_evar=0;_T77.generalize_evars=generalize_evars;_T77.allow_aqual_bounds=1;_T77.free_vars=0;_T77.free_evars=0;_T77.free_qualvars=0;_TA=_T77;}_TB=expected_kind;_TC=t;{
# 1376
struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_TA,_TB,1,_TC);_TE=Cyc_List_map;{
# 1379
struct Cyc_List_List*(*_T77)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))_TE;_TD=_T77;}_T10=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T77)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T10;_TF=_T77;}_T11=cvt;_T12=_T11.free_vars;{struct Cyc_List_List*free_tvars=_TD(_TF,_T12);_T13=cvt;{
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
 if(ts!=0)goto _TL27B;{struct Cyc_List_List*_T77=_cycalloc(sizeof(struct Cyc_List_List));_T20=fs;_T21=_T20->hd;
_T77->hd=(struct Cyc_Absyn_Tvar*)_T21;_T77->tl=res;_T1F=(struct Cyc_List_List*)_T77;}res=_T1F;goto _TL27C;_TL27B: _TL27C:;}_T22=fs;
# 1385
fs=_T22->tl;goto _TL274;_TL273:;}
# 1393
free_tvars=Cyc_List_imp_rev(res);}goto _TL270;_TL26F: _TL270:{
# 1398
struct Cyc_List_List*x=free_tvars;_TL280: if(x!=0)goto _TL27E;else{goto _TL27F;}
_TL27E: _T23=x;_T24=_T23->hd;_T25=(struct Cyc_Absyn_Tvar*)_T24;_T26=_T25->kind;{void*_T77=Cyc_Kinds_compress_kb(_T26);enum Cyc_Absyn_AliasHint _T78;struct Cyc_Absyn_Kind*_T79;void*_T7A;_T27=(int*)_T77;_T28=*_T27;switch(_T28){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T7B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T77;_T29=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T77;_T2A=& _T29->f1;_T7A=(struct Cyc_Core_Opt**)_T2A;}{struct Cyc_Core_Opt**f=_T7A;_T2B=f;_T2C=& Cyc_Kinds_bk;_T2D=(struct Cyc_Absyn_Kind*)_T2C;
*_T2B=Cyc_Kinds_kind_to_bound_opt(_T2D);goto _LL0;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T7B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T77;_T2E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T77;_T2F=& _T2E->f1;_T7A=(struct Cyc_Core_Opt**)_T2F;_T79=_T7B->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T7A;struct Cyc_Absyn_Kind*k=_T79;_T30=k;_T31=(struct Cyc_Absyn_Kind*)_T30;_T32=_T31->kind;_T33=(int)_T32;switch(_T33){case Cyc_Absyn_BoxKind: goto _LLD;case Cyc_Absyn_MemKind: _LLD: _T34=f;_T35=& Cyc_Kinds_bk;_T36=(struct Cyc_Absyn_Kind*)_T35;
# 1405
*_T34=Cyc_Kinds_kind_to_bound_opt(_T36);goto _LL9;default: _T37=f;
# 1407
*_T37=Cyc_Kinds_kind_to_bound_opt(k);goto _LL9;}_LL9: goto _LL0;}default: _T38=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T77;_T39=_T38->f1;_T3A=(struct Cyc_Absyn_Kind*)_T39;_T3B=_T3A->kind;if(_T3B!=Cyc_Absyn_MemKind)goto _TL283;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T7B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T77;_T3C=_T7B->f1;{struct Cyc_Absyn_Kind _T7C=*_T3C;_T78=_T7C.aliashint;}}{enum Cyc_Absyn_AliasHint a=_T78;{struct Cyc_Warn_String_Warn_Warg_struct _T7B;_T7B.tag=0;
# 1411
_T7B.f1=_tag_fat("type variable ",sizeof(char),15U);_T3D=_T7B;}{struct Cyc_Warn_String_Warn_Warg_struct _T7B=_T3D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T7C;_T7C.tag=7;_T3F=x;_T40=_T3F->hd;_T7C.f1=(struct Cyc_Absyn_Tvar*)_T40;_T3E=_T7C;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T7C=_T3E;{struct Cyc_Warn_String_Warn_Warg_struct _T7D;_T7D.tag=0;_T7D.f1=_tag_fat(" cannot have kind ",sizeof(char),19U);_T41=_T7D;}{struct Cyc_Warn_String_Warn_Warg_struct _T7D=_T41;{struct Cyc_Warn_Kind_Warn_Warg_struct _T7E;_T7E.tag=9;{struct Cyc_Absyn_Kind*_T7F=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_T7F->kind=1U;_T7F->aliashint=a;_T43=(struct Cyc_Absyn_Kind*)_T7F;}_T7E.f1=_T43;_T42=_T7E;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T7E=_T42;void*_T7F[4];_T7F[0]=& _T7B;_T7F[1]=& _T7C;_T7F[2]=& _T7D;_T7F[3]=& _T7E;_T44=loc;_T45=_tag_fat(_T7F,sizeof(void*),4);Cyc_Warn_err2(_T44,_T45);}}}}goto _LL0;}_TL283: goto _LL0;}_LL0:;}_T46=x;
# 1398
x=_T46->tl;goto _TL280;_TL27F:;}
# 1417
if(free_tvars!=0)goto _TL287;else{goto _TL289;}_TL289: if(free_evars!=0)goto _TL287;else{goto _TL288;}_TL288: if(free_qualvars!=0)goto _TL287;else{goto _TL285;}
_TL287:{void*_T77=Cyc_Absyn_compress(t);void*_T78;void*_T79;_T47=(int*)_T77;_T48=*_T47;if(_T48!=6)goto _TL28A;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T7A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T77;_T49=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T77;_T4A=& _T49->f1.tvars;_T79=(struct Cyc_List_List**)_T4A;_T4B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T77;_T4C=& _T4B->f1.qual_bnd;_T78=(struct Cyc_List_List**)_T4C;}{struct Cyc_List_List**btvs=(struct Cyc_List_List**)_T79;struct Cyc_List_List**qb=(struct Cyc_List_List**)_T78;_T4D=loc;_T4E=cvt;_T50=Cyc_List_map_c;{
# 1421
struct Cyc_List_List*(*_T7A)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T50;_T4F=_T7A;}_T51=Cyc_Tcutil_tvar_bool_pair;_T52=btvs;_T53=*_T52;_T54=_T4F(_T51,0,_T53);_T55=qb;_T56=Cyc_Absyn_al_qual_type;Cyc_Tctyp_update_qualifier_bounds(_T4D,_T4E,_T54,_T55,_T56);_T57=btvs;_T58=*_T57;
# 1423
if(_T58!=0)goto _TL28C;_T59=btvs;_T5B=Cyc_List_filter;{
# 1425
struct Cyc_List_List*(*_T7A)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T5B;_T5A=_T7A;}_T5C=Cyc_CurRgn_is_not_curr_rgn;_T5D=free_tvars;*_T59=_T5A(_T5C,_T5D);
free_tvars=0;goto _TL28D;_TL28C: _TL28D: goto _LL10;}_TL28A: goto _LL10;_LL10:;}
# 1431
if(free_tvars==0)goto _TL28E;{struct Cyc_Warn_String_Warn_Warg_struct _T77;_T77.tag=0;
_T77.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_T5E=_T77;}{struct Cyc_Warn_String_Warn_Warg_struct _T77=_T5E;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T78;_T78.tag=7;_T60=free_tvars;_T61=_T60->hd;_T78.f1=(struct Cyc_Absyn_Tvar*)_T61;_T5F=_T78;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T78=_T5F;{struct Cyc_Warn_String_Warn_Warg_struct _T79;_T79.tag=0;_T79.f1=_tag_fat(" when checking ",sizeof(char),16U);_T62=_T79;}{struct Cyc_Warn_String_Warn_Warg_struct _T79=_T62;{struct Cyc_Warn_Typ_Warn_Warg_struct _T7A;_T7A.tag=2;_T7A.f1=t;_T63=_T7A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T7A=_T63;void*_T7B[4];_T7B[0]=& _T77;_T7B[1]=& _T78;_T7B[2]=& _T79;_T7B[3]=& _T7A;_T64=loc;_T65=_tag_fat(_T7B,sizeof(void*),4);Cyc_Warn_err2(_T64,_T65);}}}}goto _TL28F;_TL28E: _TL28F: _T66=
Cyc_Tcutil_is_function_type(t);if(_T66)goto _TL293;else{goto _TL292;}_TL293: _T67=te;_T68=_T67->in_tempest;if(_T68)goto _TL290;else{goto _TL292;}
_TL292: Cyc_Tctyp_check_free_evars(free_evars,t,loc);goto _TL291;_TL290: _TL291: goto _TL286;_TL285: _TL286: {
# 1437
void*_T77=Cyc_Absyn_compress(t);struct Cyc_List_List*_T78;_T69=(int*)_T77;_T6A=*_T69;if(_T6A!=6)goto _TL294;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T79=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T77;_T6B=_T79->f1;_T78=_T6B.tvars;}{struct Cyc_List_List*btvs=_T78;_T6D=Cyc_List_exists;{
# 1439
int(*_T79)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T6D;_T6C=_T79;}_T6E=Cyc_CurRgn_is_curr_rgn;_T6F=btvs;_T70=_T6C(_T6E,_T6F);if(!_T70)goto _TL296;{struct Cyc_Warn_String_Warn_Warg_struct _T79;_T79.tag=0;
# 1441
_T79.f1=_tag_fat("cannot abstract current region ",sizeof(char),32U);_T71=_T79;}{struct Cyc_Warn_String_Warn_Warg_struct _T79=_T71;{struct Cyc_Warn_String_Warn_Warg_struct _T7A;_T7A.tag=0;_T7A.f1=Cyc_CurRgn_curr_rgn_name;_T72=_T7A;}{struct Cyc_Warn_String_Warn_Warg_struct _T7A=_T72;{struct Cyc_Warn_String_Warn_Warg_struct _T7B;_T7B.tag=0;_T7B.f1=_tag_fat(" in ",sizeof(char),5U);_T73=_T7B;}{struct Cyc_Warn_String_Warn_Warg_struct _T7B=_T73;{struct Cyc_Warn_Typ_Warn_Warg_struct _T7C;_T7C.tag=2;_T7C.f1=t;_T74=_T7C;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T7C=_T74;void*_T7D[4];_T7D[0]=& _T79;_T7D[1]=& _T7A;_T7D[2]=& _T7B;_T7D[3]=& _T7C;_T75=loc;_T76=_tag_fat(_T7D,sizeof(void*),4);Cyc_Warn_err2(_T75,_T76);}}}}goto _TL297;_TL296: _TL297: goto _LL15;}_TL294: goto _LL15;_LL15:;}}}}}}}}
# 1451
void Cyc_Tctyp_check_fndecl_valid_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){int*_T0;int _T1;struct Cyc_Absyn_Fndecl*_T2;struct Cyc_Absyn_FnInfo _T3;struct Cyc_Absyn_Fndecl*_T4;struct Cyc_Absyn_Fndecl*_T5;struct Cyc_Absyn_Fndecl*_T6;unsigned _T7;struct Cyc_Absyn_FnInfo _T8;struct Cyc_Absyn_Tqual _T9;int _TA;struct Cyc_Absyn_FnInfo _TB;void*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;int(*_TE)(struct _fat_ptr);void*(*_TF)(struct _fat_ptr);struct _fat_ptr _T10;struct Cyc_Absyn_Fndecl*_T11;struct Cyc_Absyn_FnInfo _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;struct _tuple9*_T16;struct _tuple9 _T17;struct _fat_ptr*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;unsigned _T1B;struct _fat_ptr _T1C;struct Cyc_Absyn_Fndecl*_T1D;
void*t=Cyc_Tcutil_fndecl2type(fd);
# 1454
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);{
void*_T1E=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _T1F;_T0=(int*)_T1E;_T1=*_T0;if(_T1!=6)goto _TL298;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T20=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1E;_T1F=_T20->f1;}{struct Cyc_Absyn_FnInfo i=_T1F;_T2=fd;_T3=_T2->i;{
# 1457
struct Cyc_List_List*atts=_T3.attributes;_T4=fd;
_T4->i=i;_T5=fd;
_T5->i.attributes=atts;_T6=fd;_T7=loc;_T8=i;_T9=_T8.ret_tqual;_TA=_T9.print_const;_TB=i;_TC=_TB.ret_type;
_T6->i.ret_tqual.real_const=
Cyc_Tcutil_extract_const_from_typedef(_T7,_TA,_TC);goto _LL0;}}_TL298:{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
# 1463
_T20.f1=_tag_fat("check_fndecl_valid_type: not a FnType",sizeof(char),38U);_TD=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_TD;void*_T21[1];_T21[0]=& _T20;_TF=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TF;_TE=_T22;}_T10=_tag_fat(_T21,sizeof(void*),1);_TE(_T10);}_LL0:;}{
# 1465
struct Cyc_List_List*argnames=0;_T11=fd;_T12=_T11->i;{
struct Cyc_List_List*args=_T12.args;_TL29D: if(args!=0)goto _TL29B;else{goto _TL29C;}
_TL29B:{struct Cyc_List_List*_T1E=_cycalloc(sizeof(struct Cyc_List_List));_T14=args;_T15=_T14->hd;_T16=(struct _tuple9*)_T15;_T17=*_T16;_T18=_T17.f0;_T1E->hd=_check_null(_T18);_T1E->tl=argnames;_T13=(struct Cyc_List_List*)_T1E;}argnames=_T13;_T19=args;
# 1466
args=_T19->tl;goto _TL29D;_TL29C:;}_T1A=argnames;_T1B=loc;_T1C=
# 1468
_tag_fat("function declaration has repeated parameter",sizeof(char),44U);Cyc_Tcutil_check_unique_vars(_T1A,_T1B,_T1C);_T1D=fd;
# 1470
_T1D->cached_type=t;}}
# 1478
void Cyc_Tctyp_check_type(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){struct Cyc_Tctyp_CVTEnv _T0;struct Cyc_Absyn_Kind*_T1;int _T2;void*_T3;struct Cyc_List_List*(*_T4)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Tvar*(*_T8)(struct _tuple12*);void*(*_T9)(struct _tuple0*);struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Tvar_Warn_Warg_struct _TB;struct Cyc_List_List*_TC;void*_TD;struct _tuple12*_TE;struct _tuple12 _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Warn_Typ_Warn_Warg_struct _T11;unsigned _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;int _T15;struct Cyc_Tctyp_CVTEnv _T16;struct Cyc_List_List*_T17;void*_T18;unsigned _T19;{struct Cyc_Tctyp_CVTEnv _T1A;
# 1481
_T1A.loc=loc;_T1A.te=te;_T1A.kind_env=bound_tvars;_T1A.fn_result=0;_T1A.def_aliasable=0;_T1A.inst_type_evar=0;_T1A.generalize_evars=0;_T1A.allow_aqual_bounds=0;_T1A.free_vars=0;_T1A.free_evars=0;_T1A.free_qualvars=0;_T0=_T1A;}_T1=expected_kind;_T2=allow_abs_aggr;_T3=t;{struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_T0,_T1,_T2,_T3);{
# 1483
struct Cyc_List_List*(*_T1A)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T4=_T1A;}_T5=cvt;_T6=_T5.free_vars;_T7=bound_tvars;_T9=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T1A)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T9;_T8=_T1A;}{struct Cyc_List_List*free_tvs=_T4(_T6,_T7,_T8);
_TL2A1: if(free_tvs!=0)goto _TL29F;else{goto _TL2A0;}
_TL29F:{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;_T1A.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_TA=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_TA;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T1B;_T1B.tag=7;_TC=free_tvs;_TD=_TC->hd;_TE=(struct _tuple12*)_TD;_TF=*_TE;_T1B.f1=_TF.f0;_TB=_T1B;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T1B=_TB;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=_tag_fat(" in type ",sizeof(char),10U);_T10=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T10;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1D;_T1D.tag=2;_T1D.f1=t;_T11=_T1D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1D=_T11;void*_T1E[4];_T1E[0]=& _T1A;_T1E[1]=& _T1B;_T1E[2]=& _T1C;_T1E[3]=& _T1D;_T12=loc;_T13=_tag_fat(_T1E,sizeof(void*),4);Cyc_Warn_err2(_T12,_T13);}}}}_T14=free_tvs;
# 1484
free_tvs=_T14->tl;goto _TL2A1;_TL2A0: _T15=allow_evars;
# 1486
if(_T15)goto _TL2A2;else{goto _TL2A4;}
_TL2A4: _T16=cvt;_T17=_T16.free_evars;_T18=t;_T19=loc;Cyc_Tctyp_check_free_evars(_T17,_T18,_T19);goto _TL2A3;_TL2A2: _TL2A3:;}}}
# 1490
void Cyc_Tctyp_check_type_with_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*flagged_tvars,struct Cyc_List_List**qual_bnd,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){struct Cyc_List_List*(*_T0)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T2)(struct _tuple12*);void*(*_T3)(struct _tuple0*);struct Cyc_List_List*_T4;struct Cyc_Tctyp_CVTEnv _T5;struct Cyc_Absyn_Kind*_T6;int _T7;void*_T8;struct Cyc_List_List**_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Tctyp_CVTEnv _TC;struct Cyc_Absyn_Kind*_TD;struct Cyc_Absyn_Kind*_TE;int _TF;void*_T10;struct Cyc_Tctyp_CVTEnv _T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Absyn_Kind*_T13;int _T14;void*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*(*_T17)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*));struct Cyc_Tctyp_CVTEnv _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_Tvar*(*_T1B)(struct _tuple12*);void*(*_T1C)(struct _tuple0*);struct Cyc_Warn_String_Warn_Warg_struct _T1D;struct Cyc_Warn_Tvar_Warn_Warg_struct _T1E;struct Cyc_List_List*_T1F;void*_T20;struct _tuple12*_T21;struct _tuple12 _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_Typ_Warn_Warg_struct _T24;unsigned _T25;struct _fat_ptr _T26;struct Cyc_List_List*_T27;int _T28;struct Cyc_Tctyp_CVTEnv _T29;struct Cyc_List_List*_T2A;void*_T2B;unsigned _T2C;_T1=Cyc_List_map;{
# 1494
struct Cyc_List_List*(*_T2D)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))_T1;_T0=_T2D;}_T3=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2D)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T3;_T2=_T2D;}_T4=flagged_tvars;{struct Cyc_List_List*bound_tvars=_T0(_T2,_T4);{struct Cyc_Tctyp_CVTEnv _T2D;
_T2D.loc=loc;_T2D.te=te;_T2D.kind_env=bound_tvars;_T2D.fn_result=0;_T2D.def_aliasable=0;_T2D.inst_type_evar=0;_T2D.generalize_evars=0;_T2D.allow_aqual_bounds=1;_T2D.free_vars=0;_T2D.free_evars=0;_T2D.free_qualvars=0;_T5=_T2D;}_T6=expected_kind;_T7=allow_abs_aggr;_T8=t;{struct Cyc_Tctyp_CVTEnv cvt=Cyc_Tctyp_check_valid_type(_T5,_T6,_T7,_T8);_T9=qual_bnd;{
# 1499
struct Cyc_List_List*qbit=*_T9;_TL2A8: if(qbit!=0)goto _TL2A6;else{goto _TL2A7;}
_TL2A6: _TA=qbit;_TB=_TA->hd;{struct _tuple0*_T2D=(struct _tuple0*)_TB;void*_T2E;void*_T2F;{struct _tuple0 _T30=*_T2D;_T2F=_T30.f0;_T2E=_T30.f1;}{void*tv=_T2F;void*bnd=_T2E;_TC=cvt;_TD=& Cyc_Kinds_aqk;_TE=(struct Cyc_Absyn_Kind*)_TD;_TF=allow_abs_aggr;_T10=tv;
cvt=Cyc_Tctyp_check_valid_type(_TC,_TE,_TF,_T10);_T11=cvt;_T12=& Cyc_Kinds_aqk;_T13=(struct Cyc_Absyn_Kind*)_T12;_T14=allow_abs_aggr;_T15=bnd;
cvt=Cyc_Tctyp_check_valid_type(_T11,_T13,_T14,_T15);}}_T16=qbit;
# 1499
qbit=_T16->tl;goto _TL2A8;_TL2A7:;}{
# 1505
struct Cyc_List_List*(*_T2D)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))=(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Tvar*(*)(struct _tuple12*)))Cyc_Tctyp_remove_bound_tvars;_T17=_T2D;}_T18=cvt;_T19=_T18.free_vars;_T1A=bound_tvars;_T1C=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2D)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T1C;_T1B=_T2D;}{struct Cyc_List_List*free_tvs=_T17(_T19,_T1A,_T1B);
_TL2AC: if(free_tvs!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA:{struct Cyc_Warn_String_Warn_Warg_struct _T2D;_T2D.tag=0;_T2D.f1=_tag_fat("unbound type variable ",sizeof(char),23U);_T1D=_T2D;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D=_T1D;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T2E;_T2E.tag=7;_T1F=free_tvs;_T20=_T1F->hd;_T21=(struct _tuple12*)_T20;_T22=*_T21;_T2E.f1=_T22.f0;_T1E=_T2E;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T2E=_T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T2F;_T2F.tag=0;_T2F.f1=_tag_fat(" in type ",sizeof(char),10U);_T23=_T2F;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F=_T23;{struct Cyc_Warn_Typ_Warn_Warg_struct _T30;_T30.tag=2;_T30.f1=t;_T24=_T30;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T30=_T24;void*_T31[4];_T31[0]=& _T2D;_T31[1]=& _T2E;_T31[2]=& _T2F;_T31[3]=& _T30;_T25=loc;_T26=_tag_fat(_T31,sizeof(void*),4);Cyc_Warn_err2(_T25,_T26);}}}}_T27=free_tvs;
# 1506
free_tvs=_T27->tl;goto _TL2AC;_TL2AB: _T28=allow_evars;
# 1508
if(_T28)goto _TL2AD;else{goto _TL2AF;}
_TL2AF: _T29=cvt;_T2A=_T29.free_evars;_T2B=t;_T2C=loc;Cyc_Tctyp_check_free_evars(_T2A,_T2B,_T2C);goto _TL2AE;_TL2AD: _TL2AE:
 Cyc_Tctyp_update_qualifier_bounds(loc,cvt,flagged_tvars,qual_bnd,Cyc_Absyn_rtd_qual_type);}}}}
