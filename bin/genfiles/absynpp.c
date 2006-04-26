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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);struct _tuple0{void*f0;void*f1;};
# 115
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 387
extern int Cyc_List_list_prefix(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 450 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;struct Cyc_Absyn_Exp*f10;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 529
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 568
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 736 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 923
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 938
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 944
void*Cyc_Absyn_compress(void*);
# 948
int Cyc_Absyn_type2bool(int,void*);
# 957
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 1114
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1229
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1237
struct _tuple1*Cyc_Absyn_binding2qvar(void*);
# 1247
void Cyc_Absyn_visit_type(int(*)(void*,void*),void*,void*);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 100 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 223 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 32 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 81 "kinds.h"
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 40 "flags.h"
extern int Cyc_Flags_interproc;
# 42
extern int Cyc_Flags_no_merge;
# 99
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 103
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 50
extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*,int,struct Cyc___cycFILE*);
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);
# 67 "pp.h"
extern struct Cyc_PP_Doc*Cyc_PP_nil_doc (void);
# 69
extern struct Cyc_PP_Doc*Cyc_PP_blank_doc (void);
# 72
extern struct Cyc_PP_Doc*Cyc_PP_line_doc (void);
# 78
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr);
# 80
extern struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*);
# 83
extern struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr,int);
# 91
extern struct Cyc_PP_Doc*Cyc_PP_nest(int,struct Cyc_PP_Doc*);
# 94
extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 108
extern struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr,struct Cyc_List_List*);
# 112
extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 117
extern struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr,struct Cyc_List_List*);
# 120
extern struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 123
extern struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);
# 129
extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 66
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 80
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;};struct _tuple15{int f0;struct Cyc_List_List*f1;};struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 38 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple16*);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int,struct Cyc_List_List*,int);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int,struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple1*);
# 47
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int,enum Cyc_Absyn_Primop,struct Cyc_List_List*);struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple17*);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual,void*,struct Cyc_Core_Opt*);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*,int);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*);
# 67
static int Cyc_Absynpp_expand_typedefs;
# 71
static int Cyc_Absynpp_qvar_to_Cids;static char _TmpG0[4U]="Cyc";
# 73
struct _fat_ptr Cyc_Absynpp_cyc_string={_TmpG0,_TmpG0,_TmpG0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static char _TmpG1[14U]="__NoCycPrefix";
# 77
static struct _fat_ptr Cyc_Absynpp_nocyc_str={_TmpG1,_TmpG1,_TmpG1 + 14U};
static struct _fat_ptr*Cyc_Absynpp_nocyc_strptr=& Cyc_Absynpp_nocyc_str;
# 81
static int Cyc_Absynpp_add_cyc_prefix;
# 85
static int Cyc_Absynpp_to_VC;
# 88
static int Cyc_Absynpp_decls_first;
# 92
static int Cyc_Absynpp_rewrite_temp_tvars;
# 95
static int Cyc_Absynpp_print_all_tvars;
# 98
static int Cyc_Absynpp_print_all_kinds;
# 101
static int Cyc_Absynpp_print_all_effects;
# 104
static int Cyc_Absynpp_print_using_stmts;
# 109
static int Cyc_Absynpp_print_externC_stmts;
# 113
static int Cyc_Absynpp_print_full_evars;
static int Cyc_Absynpp_gen_clean_cyclone;
static int Cyc_Absynpp_inside_function_type=0;
# 118
static int Cyc_Absynpp_generate_line_directives;
# 121
static int Cyc_Absynpp_use_curr_namespace;
# 124
static int Cyc_Absynpp_print_zeroterm;
# 127
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 130
int Cyc_Absynpp_print_for_cycdoc=0;
# 152
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;
Cyc_Absynpp_gen_clean_cyclone=fs->gen_clean_cyclone;}
# 172
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0,0};
# 193
struct Cyc_Absynpp_Params Cyc_Absynpp_cycinf_params_r={0,0,0,0,0,1,0,0,1,1,1,0,1,0,1,0,1};
# 214
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0,0};
# 235
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,0};
# 256
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0};
# 277
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_Tmp0=Cyc_Absynpp_curr_namespace;Cyc_List_imp_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=0;_Tmp1;}));});}
# 281
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(_check_null(*l)->tl==0)
*l=0;else{
# 285
Cyc_Absynpp_suppr_last(&(*l)->tl);}}
# 287
static void Cyc_Absynpp_curr_namespace_drop (void){
Cyc_Absynpp_suppr_last(& Cyc_Absynpp_curr_namespace);}
# 291
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
switch((int)c){case 7:
 return _tag_fat("\\a",sizeof(char),3U);case 8:
 return _tag_fat("\\b",sizeof(char),3U);case 12:
 return _tag_fat("\\f",sizeof(char),3U);case 10:
 return _tag_fat("\\n",sizeof(char),3U);case 13:
 return _tag_fat("\\r",sizeof(char),3U);case 9:
 return _tag_fat("\\t",sizeof(char),3U);case 11:
 return _tag_fat("\\v",sizeof(char),3U);case 92:
 return _tag_fat("\\\\",sizeof(char),3U);case 34:
 return _tag_fat("\"",sizeof(char),2U);case 39:
 return _tag_fat("\\'",sizeof(char),3U);default:
# 304
 if((int)c >= 32 &&(int)c <= 126){
struct _fat_ptr t=Cyc_Core_new_string(2U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return t;}{
# 309
struct _fat_ptr t=Cyc_Core_new_string(5U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)((unsigned char)c >> 6)& 3));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),2);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)(c >> 3)& 7));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),3);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)c & 7));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return t;}};}
# 318
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- 1U);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= 32 ||(int)c >= 126)||(int)c==34)||(int)c==92)
return 1;}}
# 325
return 0;}
# 328
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 331
int n=(int)(_get_fat_size(s,sizeof(char))- 1U);
# 333
if(n > 0 &&(int)((const char*)s.curr)[n]==0)-- n;{
# 335
int len=0;
{int i=0;for(0;i <= n;++ i){
char _Tmp0=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _Tmp1;switch((int)_Tmp0){case 7:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 12: _LL6:
 goto _LL8;case 10: _LL8:
 goto _LLA;case 13: _LLA:
 goto _LLC;case 9: _LLC:
 goto _LLE;case 11: _LLE:
 goto _LL10;case 92: _LL10:
 goto _LL12;case 34: _LL12:
 len +=2;goto _LL0;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 348
if((int)c >= 32 &&(int)c <= 126)++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 353
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _Tmp0=((const char*)s.curr)[i];char _Tmp1;switch((int)_Tmp0){case 7:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='a';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 8:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='b';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 12:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='f';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 10:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='n';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 13:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='r';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 9:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='t';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 11:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='v';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 92:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 34:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='"';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 367
if((int)c >= 32 &&(int)c <= 126)({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=c;if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});else{
# 369
unsigned char uc=(unsigned char)c;
# 372
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)(uc >> 6)& 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)(uc >> 3)& 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc & 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});}
# 377
goto _LL15;}}_LL15:;}}
# 379
return t;}}}}static char _TmpG2[6U]="const";static char _TmpG3[9U]="volatile";static char _TmpG4[9U]="restrict";
# 382
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
static struct _fat_ptr restrict_string={_TmpG4,_TmpG4,_TmpG4 + 9U};
static struct _fat_ptr volatile_string={_TmpG3,_TmpG3,_TmpG3 + 9U};
static struct _fat_ptr const_string={_TmpG2,_TmpG2,_TmpG2 + 6U};
struct Cyc_List_List*l=0;
if(tq.q_restrict)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& restrict_string,_Tmp0->tl=l;_Tmp0;});
if(tq.q_volatile)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& volatile_string,_Tmp0->tl=l;_Tmp0;});
if(tq.print_const)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& const_string,_Tmp0->tl=l;_Tmp0;});{
struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat(" ",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(" ",sizeof(char),2U);return Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_PP_textptr,l));}}
# 393
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){return Cyc_Kinds_kind2string(ka);}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 396
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_Tmp0=Cyc_Kinds_compress_kb(c);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_Absynpp_kind2string(k);}case 1:
# 401
 if(Cyc_PP_tex_output)return _tag_fat("{?}",sizeof(char),4U);else{return _tag_fat("?",sizeof(char),2U);}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_kind2string(k);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;return Cyc_aprintf(_tag_fat("<=%s",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));}};}
# 406
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_Tmp0=Cyc_Kinds_compress_kb(c);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1:
# 411
 if(Cyc_PP_tex_output)return Cyc_PP_text(_tag_fat("{?}",sizeof(char),4U));else{return Cyc_PP_text(_tag_fat("?",sizeof(char),2U));}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}};}
# 416
static int Cyc_Absynpp_emptydoc(struct Cyc_PP_Doc*a){
return Cyc_strlen(Cyc_PP_string_of_doc(a,72))==0U;}
# 421
static struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
if(Cyc_Absynpp_gen_clean_cyclone && !Cyc_Absynpp_inside_function_type){
int foundEvar=0;
struct Cyc_List_List*cleanTs=0;
{struct Cyc_List_List*ts2=ts;for(0;!foundEvar &&(unsigned)ts2;ts2=ts2->tl){
void*_Tmp0=(void*)ts2->hd;if(*((int*)_Tmp0)==1){
# 428
foundEvar=1;
goto _LL0;}else{
# 434
cleanTs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)ts2->hd,_Tmp1->tl=cleanTs;_Tmp1;});
goto _LL0;}_LL0:;}}
# 438
ts=Cyc_List_imp_rev(cleanTs);}{
# 440
struct Cyc_List_List*l=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_Absynpp_typ2doc,ts);
if(Cyc_Absynpp_gen_clean_cyclone){
struct Cyc_List_List*prev=0;
int foundEmptyDoc=0;
{struct Cyc_List_List*ll=l;for(0;(unsigned)ll;ll=ll->tl){
if(!Cyc_Absynpp_emptydoc((struct Cyc_PP_Doc*)ll->hd)){
if(foundEmptyDoc)
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Found non-empty doc after an empty doc",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));
# 449
prev=ll;}else{
# 452
foundEmptyDoc=1;}}}
# 455
if(prev==0)
return Cyc_PP_nil_doc();
prev->tl=0;}
# 459
return Cyc_PP_egroup(_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U),l);}}
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
# 466
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
struct _fat_ptr kstring=_tag_fat("K",sizeof(char),2U);
{void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 473
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=kstring;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=_fat_ptr_plus(n,sizeof(char),1);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("`G%s%s",sizeof(char),7U),_tag_fat(_Tmp2,sizeof(void*),2));}));}
# 475
return Cyc_PP_text(n);}
# 478
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 return Cyc_Absynpp_tvar2doc(tv);case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
# 482
_Tmp1=k;goto _LL6;}default: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Kind*k=_Tmp1;
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_tvar2doc(tv);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat("::",sizeof(char),3U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_kind2doc(k);_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}};}
# 487
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
struct _fat_ptr _Tmp0=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);return Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Absynpp_ktvar2doc,tvs));}
# 490
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*)=(Cyc_Absynpp_print_all_kinds || Cyc_Absynpp_gen_clean_cyclone)?Cyc_Absynpp_ktvar2doc: Cyc_Absynpp_tvar2doc;
struct _fat_ptr _Tmp0=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);return Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(f,tvs));}struct _tuple18{struct Cyc_Absyn_Tqual f0;void*f1;};
# 495
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple18*t){
return Cyc_Absynpp_tqtd2doc((*t).f0,(*t).f1,0);}
# 498
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
struct _fat_ptr _Tmp0=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);return Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(struct _tuple18*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Absynpp_arg2doc,ts));}
# 502
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
switch(*((int*)att)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 return Cyc_PP_nil_doc();default:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));};}
# 511
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 1:
 return Cyc_PP_text(_tag_fat(" _stdcall ",sizeof(char),11U));case 2:
 return Cyc_PP_text(_tag_fat(" _cdecl ",sizeof(char),9U));case 3:
 return Cyc_PP_text(_tag_fat(" _fastcall ",sizeof(char),12U));default:
 goto _LL0;}_LL0:;}
# 519
return Cyc_PP_nil_doc();}
# 521
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2!=0;atts2=atts2->tl){
void*_Tmp0=(void*)atts2->hd;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 530
if(!hasatt)
return Cyc_PP_nil_doc();{
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" __declspec(",sizeof(char),13U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp4=_tag_fat(" ",sizeof(char),2U);Cyc_PP_group(_Tmp2,_Tmp3,_Tmp4,({struct Cyc_List_List*(*_Tmp5)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp5;})(Cyc_Absynpp_noncallatt2doc,atts));});_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}}
# 537
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
int _Tmp0;struct _fat_ptr _Tmp1;void*_Tmp2;int _Tmp3;switch(*((int*)a)){case 0: _Tmp3=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)a)->f1;{int i=_Tmp3;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("regparm(%d)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));}case 1:
 return _tag_fat("stdcall",sizeof(char),8U);case 2:
 return _tag_fat("cdecl",sizeof(char),6U);case 3:
 return _tag_fat("fastcall",sizeof(char),9U);case 4:
 return _tag_fat("noreturn",sizeof(char),9U);case 5:
 return _tag_fat("const",sizeof(char),6U);case 6: _Tmp2=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)a)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 546
if(e==0)return _tag_fat("aligned",sizeof(char),8U);
switch((int)Cyc_Flags_c_compiler){case Cyc_Flags_Gcc_c:  {
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("aligned(%s)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));}case Cyc_Flags_Vc_c:
 goto _LL41;default: _LL41: {struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("align(%s)",sizeof(char),10U),_tag_fat(_Tmp5,sizeof(void*),1));}};}case 7:
# 551
 return _tag_fat("packed",sizeof(char),7U);case 8: _Tmp1=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("section(\"%s\")",sizeof(char),14U),_tag_fat(_Tmp5,sizeof(void*),1));}case 9:
 return _tag_fat("nocommon",sizeof(char),9U);case 10:
 return _tag_fat("shared",sizeof(char),7U);case 11:
 return _tag_fat("unused",sizeof(char),7U);case 12:
 return _tag_fat("weak",sizeof(char),5U);case 13:
 return _tag_fat("dllimport",sizeof(char),10U);case 14:
 return _tag_fat("dllexport",sizeof(char),10U);case 15:
 return _tag_fat("no_instrument_function",sizeof(char),23U);case 16:
 return _tag_fat("constructor",sizeof(char),12U);case 17:
 return _tag_fat("destructor",sizeof(char),11U);case 18:
 return _tag_fat("no_check_memory_usage",sizeof(char),22U);case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f1==Cyc_Absyn_Printf_ft){_Tmp3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f2;_Tmp0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f3;{int n=_Tmp3;int m=_Tmp0;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)n;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)m;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat("format(printf,%u,%u)",sizeof(char),21U),_tag_fat(_Tmp6,sizeof(void*),2));}}else{_Tmp3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f2;_Tmp0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f3;{int n=_Tmp3;int m=_Tmp0;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)n;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)m;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat("format(scanf,%u,%u)",sizeof(char),20U),_tag_fat(_Tmp6,sizeof(void*),2));}}case 20: _Tmp3=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("initializes(%d)",sizeof(char),16U),_tag_fat(_Tmp5,sizeof(void*),1));}case 21: _Tmp3=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("noliveunique(%d)",sizeof(char),17U),_tag_fat(_Tmp5,sizeof(void*),1));}case 22: _Tmp3=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("consume(%d)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));}case 23:
 return _tag_fat("pure",sizeof(char),5U);case 26:
 return _tag_fat("always_inline",sizeof(char),14U);case 24: _Tmp1=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("__mode__(\"%s\")",sizeof(char),15U),_tag_fat(_Tmp5,sizeof(void*),1));}case 25: _Tmp1=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("alias(\"%s\")",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));}default:
 return _tag_fat("no_throw",sizeof(char),9U);};}
# 576
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 578
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts==0)return Cyc_PP_nil_doc();
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c)
return Cyc_Absynpp_noncallconv2doc(atts);else{
struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" __attribute__",sizeof(char),15U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("((",sizeof(char),3U);struct _fat_ptr _Tmp3=_tag_fat("))",sizeof(char),3U);struct _fat_ptr _Tmp4=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp2,_Tmp3,_Tmp4,({struct Cyc_List_List*(*_Tmp5)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp5;})(Cyc_Absynpp_att2doc,atts));});_Tmp0[1]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));};}
# 587
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms==0)return 0;{
void*_Tmp0=(void*)tms->hd;switch(*((int*)_Tmp0)){case 2:
 return 1;case 5:
# 592
 if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c)
return 0;else{
return Cyc_Absynpp_next_is_pointer(tms->tl);};default:
# 596
 return 0;};}}
# 600
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
static struct Cyc_PP_Doc*cache_question=0;
if(!((unsigned)cache_question))
cache_question=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{?}",sizeof(char),4U),1): Cyc_PP_text(_tag_fat("?",sizeof(char),2U));
return cache_question;}
# 606
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
static struct Cyc_PP_Doc*cache_lb=0;
if(!((unsigned)cache_lb))
cache_lb=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{\\lb}",sizeof(char),6U),1): Cyc_PP_text(_tag_fat("{",sizeof(char),2U));
return cache_lb;}
# 612
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
static struct Cyc_PP_Doc*cache_rb=0;
if(!((unsigned)cache_rb))
cache_rb=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{\\rb}",sizeof(char),6U),1): Cyc_PP_text(_tag_fat("}",sizeof(char),2U));
return cache_rb;}
# 618
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
static struct Cyc_PP_Doc*cache_dollar=0;
if(!((unsigned)cache_dollar))
cache_dollar=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("\\$",sizeof(char),3U),1): Cyc_PP_text(_tag_fat("$",sizeof(char),2U));
return cache_dollar;}
# 625
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seq(sep,ss);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}
# 630
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(1;tms!=0;tms=tms->tl){
void*_Tmp0=(void*)tms->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 0:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Carray_mod ",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,_tag_fat("ConstArray_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)==1){_Tmp1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)->f1;{struct Cyc_List_List*ys=_Tmp1;
# 636
Cyc_fprintf(Cyc_stderr,_tag_fat("Function_mod(",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));
for(1;ys!=0;ys=ys->tl){
struct _fat_ptr*v=(*((struct _tuple9*)ys->hd)).f0;
if(v==0)Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
Cyc_fprintf(Cyc_stderr,*v,_tag_fat(0U,sizeof(void*),0));}
if(ys->tl!=0)Cyc_fprintf(Cyc_stderr,_tag_fat(",",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 643
Cyc_fprintf(Cyc_stderr,_tag_fat(") ",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}else{
Cyc_fprintf(Cyc_stderr,_tag_fat("Function_mod()",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 5:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Attributes_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 4:
 Cyc_fprintf(Cyc_stderr,_tag_fat("TypeParams_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Pointer_mod ",sizeof(char),13U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 650
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 653
static struct Cyc_PP_Doc*const(*Cyc_Absynpp_rgn2doc)(void*)=(struct Cyc_PP_Doc*const(*)(void*))Cyc_Absynpp_ntyp2doc;
# 655
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch((int)sc){case Cyc_Absyn_Static:
 return Cyc_PP_text(_tag_fat("static ",sizeof(char),8U));case Cyc_Absyn_Public:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
 return Cyc_PP_text(_tag_fat("extern ",sizeof(char),8U));case Cyc_Absyn_ExternC:
 return Cyc_PP_text(_tag_fat("extern \"C\" ",sizeof(char),12U));case Cyc_Absyn_Abstract:
 return Cyc_PP_text(_tag_fat("abstract ",sizeof(char),10U));case Cyc_Absyn_Register:
 return Cyc_PP_text(_tag_fat("register ",sizeof(char),10U));default:
 return Cyc_PP_nil_doc();};}
# 668
static struct Cyc_PP_Doc*Cyc_Absynpp_aqual_val2doc(enum Cyc_Absyn_AliasQualVal aqv){
switch((int)aqv){case Cyc_Absyn_Aliasable_qual:
 return Cyc_PP_text(_tag_fat("ALIASABLE",sizeof(char),10U));case Cyc_Absyn_Unique_qual:
 return Cyc_PP_text(_tag_fat("UNIQUE",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Refcnt_qual:
 return Cyc_PP_text(_tag_fat("REFCNT",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Restricted_qual:
 return Cyc_PP_text(_tag_fat("RESTRICTED",sizeof(char),11U));goto _LL0;default:
 _throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Absynpp::aqual_val2doc unrecognized value",sizeof(char),42U);_Tmp0;}));}_LL0:;}
# 678
static int Cyc_Absynpp_is_function_tms(struct Cyc_List_List*tms){
if(tms==0)return 0;{
void*_Tmp0=(void*)tms->hd;if(*((int*)_Tmp0)==3)
return 1;else{
return Cyc_Absynpp_is_function_tms(tms->tl);};}}
# 686
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms==0)return d;{
int save_ift=Cyc_Absynpp_inside_function_type;
if(Cyc_Absynpp_gen_clean_cyclone && Cyc_Absynpp_is_function_tms(tms))
Cyc_Absynpp_inside_function_type=1;{
# 692
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=rest,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
struct Cyc_PP_Doc*s;
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;unsigned _Tmp7;void*_Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 0: _Tmp9=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*zeroterm=_Tmp9;
# 697
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
s=({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[]@zeroterm",sizeof(char),12U));_TmpA[1]=_TmpB;}):({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[]",sizeof(char),3U));_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;return s;}case 1: _Tmp9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp8=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;void*zeroterm=_Tmp8;
# 701
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
s=({struct Cyc_PP_Doc*_TmpA[4];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[",sizeof(char),2U));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[2]=_TmpB;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("]@zeroterm",sizeof(char),11U));_TmpA[3]=_TmpB;}):({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});
Cyc_Absynpp_inside_function_type=save_ift;return s;}case 3: _Tmp9=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp9;
# 706
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;void*_Tmp11;void*_Tmp12;int _Tmp13;void*_Tmp14;if(*((int*)args)==1){_Tmp14=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_Tmp13=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_Tmp12=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_Tmp11=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_Tmp10=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f9;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f10;{struct Cyc_List_List*args2=_Tmp14;int c_varargs=_Tmp13;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp12;void*effopt=_Tmp11;struct Cyc_List_List*effc=_Tmp10;struct Cyc_List_List*qb=_TmpF;struct Cyc_Absyn_Exp*chk=_TmpE;struct Cyc_Absyn_Exp*req=_TmpD;struct Cyc_Absyn_Exp*ens=_TmpC;struct Cyc_Absyn_Exp*thrws=_TmpB;
# 709
s=({struct Cyc_PP_Doc*_Tmp15[2];_Tmp15[0]=rest,({struct Cyc_PP_Doc*_Tmp16=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,effc,qb,chk,req,ens,thrws);_Tmp15[1]=_Tmp16;});Cyc_PP_cat(_tag_fat(_Tmp15,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;return s;}}else{_Tmp14=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;_TmpA=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{struct Cyc_List_List*sl=_Tmp14;unsigned loc=_TmpA;
# 712
s=({struct Cyc_PP_Doc*_Tmp15[2];_Tmp15[0]=rest,({struct Cyc_PP_Doc*_Tmp16=({struct _fat_ptr _Tmp17=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _Tmp18=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _Tmp19=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp17,_Tmp18,_Tmp19,({struct Cyc_List_List*(*_Tmp1A)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;_Tmp1A;})(Cyc_PP_textptr,sl));});_Tmp15[1]=_Tmp16;});Cyc_PP_cat(_tag_fat(_Tmp15,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;return s;}};}}case 5: _Tmp9=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*atts=_Tmp9;
# 716
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 718
if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
s=({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_atts2doc(atts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;return s;}else{
# 723
if(Cyc_Absynpp_next_is_pointer(tms->tl)){
s=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_callconv2doc(atts);_TmpA[0]=_TmpB;}),_TmpA[1]=rest;Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;
return s;}
# 728
Cyc_Absynpp_inside_function_type=save_ift;return rest;};}case 4: _Tmp9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_List_List*ts=_Tmp9;unsigned loc=_Tmp7;int print_kinds=_Tmp6;
# 731
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds){
s=({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_ktvars2doc(ts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;
return s;}
# 737
s=({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_tvars2doc(ts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
Cyc_Absynpp_inside_function_type=save_ift;
return s;}default: _Tmp9=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.eff;_Tmp8=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.nullable;_Tmp5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.bounds;_Tmp4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.zero_term;_Tmp3=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.autoreleased;_Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.aqual;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*rgn=_Tmp9;void*nullable=_Tmp8;void*bd=_Tmp5;void*zt=_Tmp4;void*rel=_Tmp3;void*aq=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 743
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*aqd=mt;
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*reld=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_TmpA=Cyc_Absyn_compress(bd);void*_TmpB;if(*((int*)_TmpA)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)){case 14:
 ptr=Cyc_Absynpp_question();goto _LL17;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2!=0){_TmpB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2->hd;{void*targ=_TmpB;
# 753
{void*_TmpC;if(*((int*)targ)==9){_TmpC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)targ)->f1;{struct Cyc_Absyn_Exp*e=_TmpC;
# 755
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));{
struct _tuple13 _TmpD=Cyc_Evexp_eval_const_uint_exp(e);int _TmpE;unsigned _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{unsigned val=_TmpF;int known=_TmpE;
if(!known || val!=1U)
ptr=({struct Cyc_PP_Doc*_Tmp10[4];_Tmp10[0]=ptr,({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_lb();_Tmp10[1]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_exp2doc(e);_Tmp10[2]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_rb();_Tmp10[3]=_Tmp11;});Cyc_PP_cat(_tag_fat(_Tmp10,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}}}}else{
# 761
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));
ptr=({struct Cyc_PP_Doc*_TmpD[4];_TmpD[0]=ptr,({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc(Cyc_Absyn_compress(targ));_TmpD[2]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rb();_TmpD[3]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}_LL1E:;}
# 765
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C:
# 767
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));
ptr=({struct Cyc_PP_Doc*_TmpC[4];_TmpC[0]=ptr,({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(bd);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL17;}_LL17:;}
# 771
if(Cyc_Absynpp_print_zeroterm){
{void*_TmpA=Cyc_Absyn_compress(aq);void*_TmpB;enum Cyc_Absyn_AliasQualVal _TmpC;switch(*((int*)_TmpA)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)){case 16: _TmpC=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_TmpC;
# 774
if(aqv==Cyc_Absyn_Aliasable_qual)
goto _LL2F;else{
# 777
aqd=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_aqual_val2doc(aqv);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2F;}_LL2F:;
# 780
goto _LL26;}case 17: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2;{struct Cyc_List_List*tv=_TmpB;
# 782
if(Cyc_Absynpp_gen_clean_cyclone){
void*_TmpD=Cyc_Absyn_compress((void*)_check_null(tv)->hd);if(*((int*)_TmpD)==1)
goto _LL34;else{
# 786
aqd=({struct Cyc_PP_Doc*_TmpE[3];({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpE[0]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_typ2doc(Cyc_Absyn_compress((void*)tv->hd));_TmpE[1]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpE[2]=_TmpF;});Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL34;}_LL34:;}else{
# 791
aqd=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc(Cyc_Absyn_compress((void*)_check_null(tv)->hd));_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});}
# 793
goto _LL26;}default: goto _LL2D;}case 1:
# 795
 if(!Cyc_Absynpp_gen_clean_cyclone)
aqd=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc(aq);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL26;default: _LL2D:
# 803
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("unexpected alias qualifier",sizeof(char),27U);_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)aq;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;({int(*_Tmp10)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp10;})(_tag_fat(_TmpF,sizeof(void*),2));});}_LL26:;}
# 805
if((Cyc_Absynpp_gen_clean_cyclone || !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(_tag_fat("@zeroterm",sizeof(char),10U));else{
if((Cyc_Absynpp_gen_clean_cyclone || is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(_tag_fat("@nozeroterm",sizeof(char),12U));}
if(Cyc_Absyn_type2bool(0,rel))
reld=Cyc_PP_text(_tag_fat("@autoreleased",sizeof(char),14U));}{
# 812
int was_evar=0;
if(Cyc_Absynpp_gen_clean_cyclone && !Cyc_Absynpp_inside_function_type){
if(*((int*)rgn)==1){
# 816
was_evar=1;
goto _LL39;}else{
# 819
goto _LL39;}_LL39:;}
# 822
{void*_TmpA=Cyc_Absyn_compress(rgn);switch(*((int*)_TmpA)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)==6){
# 824
if(Cyc_Flags_interproc && Cyc_Flags_no_merge)
rgd=Cyc_PP_text(_tag_fat("`H",sizeof(char),3U));
# 827
goto _LL3E;}else{goto _LL43;}case 1: if(
Cyc_Absynpp_print_for_cycdoc || Cyc_Absynpp_gen_clean_cyclone)goto _LL3E;else{goto _LL43;}default: _LL43:
# 830
 if(!was_evar)
rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL3E:;}{
# 833
struct Cyc_PP_Doc*spacer0=reld!=mt && ztd!=mt?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
struct Cyc_PP_Doc*spacer1=tqd!=mt &&((ztd!=mt || rgd!=mt)|| reld!=mt)?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
struct Cyc_PP_Doc*spacer2=rest!=mt?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
s=({struct Cyc_PP_Doc*_TmpA[10];_TmpA[0]=ptr,_TmpA[1]=aqd,_TmpA[2]=ztd,_TmpA[3]=spacer0,_TmpA[4]=reld,_TmpA[5]=rgd,_TmpA[6]=spacer1,_TmpA[7]=tqd,_TmpA[8]=spacer2,_TmpA[9]=rest;Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),10));});
Cyc_Absynpp_inside_function_type=save_ift;
return s;}}}};}}}
# 842
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 845
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==9){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
# 849
for(1;ts!=0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_typ2doc(t);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=*effects;_Tmp3;});*effects=_Tmp2;});goto _LL0;}_LL0:;}
# 855
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=Cyc_List_imp_rev(rgions);
effects=Cyc_List_imp_rev(effects);
if(rgions==0 && effects!=0)
return Cyc_PP_group(_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat("+",sizeof(char),2U),effects);{
struct Cyc_PP_Doc*doc1=Cyc_Absynpp_group_braces(_tag_fat(",",sizeof(char),2U),rgions);
struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat("+",sizeof(char),2U);return Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*_Tmp3=effects;Cyc_List_imp_append(_Tmp3,({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=doc1,_Tmp4->tl=0;_Tmp4;}));}));}}
# 867
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
if((int)k==0)return Cyc_PP_text(_tag_fat("struct ",sizeof(char),8U));else{return Cyc_PP_text(_tag_fat("union ",sizeof(char),7U));}}
# 871
static struct _tuple18*Cyc_Absynpp_aggrfield2arg(struct Cyc_Absyn_Aggrfield*f){
struct _tuple18*_Tmp0=_cycalloc(sizeof(struct _tuple18));_Tmp0->f0=f->tq,_Tmp0->f1=f->type;return _Tmp0;}
# 876
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
enum Cyc_Absyn_AggrKind _Tmp0;void*_Tmp1;const char*_Tmp2;void*_Tmp3;enum Cyc_Absyn_AliasQualVal _Tmp4;struct _fat_ptr _Tmp5;union Cyc_Absyn_AggrInfo _Tmp6;int _Tmp7;enum Cyc_Absyn_Size_of _Tmp8;enum Cyc_Absyn_Sign _Tmp9;union Cyc_Absyn_DatatypeFieldInfo _TmpA;void*_TmpB;union Cyc_Absyn_DatatypeInfo _TmpC;switch(*((int*)t)){case 5:
# 879
 return Cyc_PP_text(_tag_fat("[[[array]]]",sizeof(char),12U));case 6:
 return Cyc_PP_nil_doc();case 4:
 return Cyc_PP_nil_doc();case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 0:
# 883
 return Cyc_PP_text(_tag_fat("void",sizeof(char),5U));case 22: _TmpC=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeInfo tu_info=_TmpC;struct Cyc_List_List*ts=_TmpB;
# 909
int _TmpD;void*_TmpE;if(tu_info.UnknownDatatype.tag==1){_TmpE=tu_info.UnknownDatatype.val.name;_TmpD=tu_info.UnknownDatatype.val.is_extensible;{struct _tuple1*n=_TmpE;int is_x=_TmpD;
_TmpE=n;_TmpD=is_x;goto _LL53;}}else{_TmpE=(*tu_info.KnownDatatype.val)->name;_TmpD=(*tu_info.KnownDatatype.val)->is_extensible;_LL53: {struct _tuple1*n=_TmpE;int is_x=_TmpD;
# 912
struct Cyc_PP_Doc*kw=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));
struct Cyc_PP_Doc*ext=is_x?Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U)): Cyc_PP_nil_doc();
struct Cyc_PP_Doc*_TmpF[4];_TmpF[0]=ext,_TmpF[1]=kw,({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(n);_TmpF[2]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_tps2doc(ts);_TmpF[3]=_Tmp10;});return Cyc_PP_cat(_tag_fat(_TmpF,sizeof(struct Cyc_PP_Doc*),4));}};}case 23: _TmpA=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_TmpA;struct Cyc_List_List*ts=_TmpB;
# 917
int _TmpD;void*_TmpE;void*_TmpF;if(tuf_info.UnknownDatatypefield.tag==1){_TmpF=tuf_info.UnknownDatatypefield.val.datatype_name;_TmpE=tuf_info.UnknownDatatypefield.val.field_name;_TmpD=tuf_info.UnknownDatatypefield.val.is_extensible;{struct _tuple1*tname=_TmpF;struct _tuple1*fname=_TmpE;int is_x=_TmpD;
# 919
_TmpF=tname;_TmpD=is_x;_TmpE=fname;goto _LL58;}}else{_TmpF=tuf_info.KnownDatatypefield.val.f0->name;_TmpD=tuf_info.KnownDatatypefield.val.f0->is_extensible;_TmpE=tuf_info.KnownDatatypefield.val.f1->name;_LL58: {struct _tuple1*tname=_TmpF;int is_x=_TmpD;struct _tuple1*fname=_TmpE;
# 922
struct Cyc_PP_Doc*kw=Cyc_PP_text(is_x?_tag_fat("@extensible datatype ",sizeof(char),22U): _tag_fat("datatype ",sizeof(char),10U));
struct Cyc_PP_Doc*_Tmp10[4];_Tmp10[0]=kw,({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_qvar2doc(tname);_Tmp10[1]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_Tmp10[2]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_qvar2doc(fname);_Tmp10[3]=_Tmp11;});return Cyc_PP_cat(_tag_fat(_Tmp10,sizeof(struct Cyc_PP_Doc*),4));}};}case 1: _Tmp9=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sn=_Tmp9;enum Cyc_Absyn_Size_of sz=_Tmp8;
# 926
struct _fat_ptr sns;
struct _fat_ptr ts;
switch((int)sn){case Cyc_Absyn_None:
 goto _LL5D;case Cyc_Absyn_Signed: _LL5D:
 sns=_tag_fat("",sizeof(char),1U);goto _LL59;default:
 sns=_tag_fat("unsigned ",sizeof(char),10U);goto _LL59;}_LL59:;
# 933
switch((int)sz){case Cyc_Absyn_Char_sz:
# 935
 switch((int)sn){case Cyc_Absyn_None:
 sns=_tag_fat("",sizeof(char),1U);goto _LL6D;case Cyc_Absyn_Signed:
 sns=_tag_fat("signed ",sizeof(char),8U);goto _LL6D;default:
 sns=_tag_fat("unsigned ",sizeof(char),10U);goto _LL6D;}_LL6D:;
# 940
ts=_tag_fat("char",sizeof(char),5U);
goto _LL60;case Cyc_Absyn_Short_sz:
 ts=_tag_fat("short",sizeof(char),6U);goto _LL60;case Cyc_Absyn_Int_sz:
 ts=_tag_fat("int",sizeof(char),4U);goto _LL60;case Cyc_Absyn_Long_sz:
 ts=_tag_fat("long",sizeof(char),5U);goto _LL60;case Cyc_Absyn_LongLong_sz:
 goto _LL6C;default: _LL6C:
# 947
 if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
ts=_tag_fat("long long",sizeof(char),10U);goto _LL74;}else{
ts=_tag_fat("__int64",sizeof(char),8U);goto _LL74;}_LL74:;
# 951
goto _LL60;}_LL60:;
# 953
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=sns;_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=ts;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_aprintf(_tag_fat("%s%s",sizeof(char),5U),_tag_fat(_TmpF,sizeof(void*),2));}));}case 2: _Tmp7=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{int i=_Tmp7;
# 955
switch((int)i){case 0:
 return Cyc_PP_text(_tag_fat("float",sizeof(char),6U));case 1:
 return Cyc_PP_text(_tag_fat("double",sizeof(char),7U));default:
 return Cyc_PP_text(_tag_fat("long double",sizeof(char),12U));};}case 3: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 960
struct Cyc_PP_Doc*_TmpD[2];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("_Complex ",sizeof(char),10U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_ntyp2doc((void*)_check_null(ts)->hd);_TmpD[1]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),2));}case 24: _Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp6;struct Cyc_List_List*ts=_TmpB;
# 962
struct _tuple12 _TmpD=Cyc_Absyn_aggr_kinded_name(info);void*_TmpE;enum Cyc_Absyn_AggrKind _TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{enum Cyc_Absyn_AggrKind k=_TmpF;struct _tuple1*n=_TmpE;
struct Cyc_PP_Doc*_Tmp10[3];({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_aggr_kind2doc(k);_Tmp10[0]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_qvar2doc(n);_Tmp10[1]=_Tmp11;}),({struct Cyc_PP_Doc*_Tmp11=Cyc_Absynpp_tps2doc(ts);_Tmp10[2]=_Tmp11;});return Cyc_PP_cat(_tag_fat(_Tmp10,sizeof(struct Cyc_PP_Doc*),3));}}case 20: _TmpB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct Cyc_List_List*fs=_TmpB;
# 970
struct Cyc_PP_Doc*_TmpD[4];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_TmpD[2]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rb();_TmpD[3]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));}case 19: _TmpB=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _tuple1*n=_TmpB;
struct Cyc_PP_Doc*_TmpD[2];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_qvar2doc(n);_TmpD[1]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),2));}case 21: _Tmp5=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _fat_ptr t=_Tmp5;
# 974
return Cyc_PP_text(t);}case 4: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 983
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("region_t<",sizeof(char),10U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rgn2doc((void*)_check_null(ts)->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 18: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 985
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("aqual_t<",sizeof(char),9U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc((void*)_check_null(ts)->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 17: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 987
return Cyc_Absynpp_typ2doc((void*)_check_null(ts)->hd);goto _LL0;}case 5: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 989
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("tag_t<",sizeof(char),7U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc((void*)_check_null(ts)->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 10: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpB;
# 991
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("regions(",sizeof(char),9U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc((void*)_check_null(ts)->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 6:
 return Cyc_PP_text(_tag_fat("`H",sizeof(char),3U));case 7:
 return Cyc_PP_text(_tag_fat("`U",sizeof(char),3U));case 8:
 return Cyc_PP_text(_tag_fat("`RC",sizeof(char),4U));case 9:
# 996
 return Cyc_Absynpp_eff2doc(t);case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){_TmpB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_TmpB;
# 1001
struct Cyc_PP_Doc*_TmpD[4];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@thin @numelts",sizeof(char),15U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc(t);_TmpD[2]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rb();_TmpD[3]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));}}else{
# 1004
return Cyc_PP_text(_tag_fat("@thin",sizeof(char),6U));}case 16: _Tmp4=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_Tmp4;
# 1002
return Cyc_Absynpp_aqual_val2doc(aqv);}case 15: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*qtv=_TmpB;
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("aquals(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc((void*)_check_null(qtv)->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 14:
# 1005
 return Cyc_PP_text(_tag_fat("@fat",sizeof(char),5U));case 11:
 return Cyc_PP_text(_tag_fat("@true",sizeof(char),6U));default:
 return Cyc_PP_text(_tag_fat("@false",sizeof(char),7U));}case 3: _TmpB=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f2;_Tmp7=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f3;_Tmp2=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f5;{struct Cyc_Core_Opt*k=_TmpB;void*topt=_Tmp3;int i=_Tmp7;const char*name=_Tmp2;
# 887
struct _fat_ptr kindstring=k==0?_tag_fat("K",sizeof(char),2U): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
if((unsigned)name)
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=kindstring;_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10=name;_tag_fat((void*)_Tmp10,sizeof(char),_get_zero_arr_size_char((void*)_Tmp10,1U));});_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_aprintf(_tag_fat("`C_%s_%s",sizeof(char),9U),_tag_fat(_TmpF,sizeof(void*),2));}));else{
# 892
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=kindstring;_TmpE;});struct Cyc_Int_pa_PrintArg_struct _TmpE=({struct Cyc_Int_pa_PrintArg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=(unsigned long)i;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_aprintf(_tag_fat("`C_%s_%d",sizeof(char),9U),_tag_fat(_TmpF,sizeof(void*),2));}));}}case 1: _TmpB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_Tmp7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f4;{struct Cyc_Core_Opt*k=_TmpB;void*topt=_Tmp3;int i=_Tmp7;struct Cyc_Core_Opt*tvs=_Tmp1;
# 896
if(topt!=0)
# 898
return Cyc_Absynpp_ntyp2doc(topt);
if(Cyc_Absynpp_gen_clean_cyclone)
return Cyc_PP_nil_doc();{
struct _fat_ptr kindstring=k==0?_tag_fat("K",sizeof(char),2U): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=kindstring;_TmpE;});struct Cyc_Int_pa_PrintArg_struct _TmpE=({struct Cyc_Int_pa_PrintArg_struct _TmpF;_TmpF.tag=1,_TmpF.f1=(unsigned long)i;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_aprintf(_tag_fat("`E%s%d",sizeof(char),7U),_tag_fat(_TmpF,sizeof(void*),2));}));}}case 2: _TmpB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_TmpB;
# 904
struct Cyc_PP_Doc*s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_TmpD[3];_TmpD[0]=s,({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("::",sizeof(char),3U));_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_kindbound2doc(tv->kind);_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});
return s;}case 7: _Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f1;_Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;_TmpB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp0;int is_tuple=_Tmp7;struct Cyc_List_List*fs=_TmpB;
# 965
if(is_tuple){
struct Cyc_PP_Doc*_TmpD[2];({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_dollar();_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_args2doc(({struct Cyc_List_List*(*_TmpF)(struct _tuple18*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map;_TmpF;})(Cyc_Absynpp_aggrfield2arg,fs));_TmpD[1]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),2));}else{
# 968
struct Cyc_PP_Doc*_TmpD[4];({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_aggr_kind2doc(k);_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_TmpD[2]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_rb();_TmpD[3]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));}}case 9: _TmpB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_TmpB;
# 972
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("valueof_t(",sizeof(char),11U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_exp2doc(e);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 11: _TmpB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_TmpB;
struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("typeof(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_exp2doc(e);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));}case 8: _TmpB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;{struct _tuple1*n=_TmpB;struct Cyc_List_List*ts=_Tmp3;struct Cyc_Absyn_Typedefdecl*kopt=_Tmp1;
# 981
struct Cyc_PP_Doc*_TmpD[2];({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_qvar2doc(n);_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_tps2doc(ts);_TmpD[1]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),2));}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1)->r)){case 0: _TmpB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_TmpB;
# 997
return Cyc_Absynpp_aggrdecl2doc(d);}case 1: _TmpB=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_TmpB;
return Cyc_Absynpp_enumdecl2doc(d);}default: _TmpB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_TmpB;
return Cyc_Absynpp_datatypedecl2doc(d);}}}_LL0:;}
# 1011
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple0*cmp){
void*_Tmp0;void*_Tmp1;_Tmp1=cmp->f0;_Tmp0=cmp->f1;{void*r1=_Tmp1;void*r2=_Tmp0;
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgn2doc(r1);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" > ",sizeof(char),4U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgn2doc(r2);_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}}
# 1015
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;_Tmp0;})(Cyc_Absynpp_rgn_cmp2doc,_tag_fat(",",sizeof(char),2U),po);}
# 1018
struct Cyc_PP_Doc*Cyc_Absynpp_one_ec2doc(void*ec){
void*_Tmp0;void*_Tmp1;switch(*((int*)ec)){case 2: _Tmp1=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)ec)->f1;_Tmp0=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)ec)->f2;{void*e1=_Tmp1;void*e2=_Tmp0;
# 1021
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_eff2doc(e1);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" <= ",sizeof(char),5U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_eff2doc(e2);_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}case 1: _Tmp1=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)ec)->f1;_Tmp0=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)ec)->f2;{void*e1=_Tmp1;void*e2=_Tmp0;
# 1023
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_eff2doc(e1);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" | ",sizeof(char),4U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_eff2doc(e2);_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}default: _Tmp1=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)ec)->f1;{void*e=_Tmp1;
# 1025
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat("single(",sizeof(char),8U));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_eff2doc(e);_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}};}
# 1028
struct Cyc_PP_Doc*Cyc_Absynpp_effconstr2doc(struct Cyc_List_List*effc){
return Cyc_PP_ppseq(Cyc_Absynpp_one_ec2doc,_tag_fat(",",sizeof(char),2U),effc);}
# 1032
struct Cyc_PP_Doc*Cyc_Absynpp_qb_cmp2doc(struct _tuple0*cmp){
void*_Tmp0;void*_Tmp1;_Tmp1=cmp->f0;_Tmp0=cmp->f1;{void*t1=_Tmp1;void*t2=_Tmp0;
struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_ntyp2doc(t2);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" >= ",sizeof(char),5U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_ntyp2doc(t1);_Tmp2[2]=_Tmp3;});return Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));}}
# 1036
struct Cyc_PP_Doc*Cyc_Absynpp_qualbnd2doc(struct Cyc_List_List*qb){
struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);return Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Absynpp_qb_cmp2doc,qb));}
# 1040
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple9*t){
struct _fat_ptr*vo=(*t).f0;
struct Cyc_Core_Opt*dopt=vo==0?0:({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*vo);_Tmp0->v=_Tmp1;});_Tmp0;});
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,dopt);}
# 1046
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*effc,struct Cyc_List_List*qb,struct Cyc_Absyn_Exp*chk,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens,struct Cyc_Absyn_Exp*thrws){
# 1053
struct Cyc_List_List*arg_docs=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct _tuple9*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat("...",sizeof(char),4U));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));});else{
if(cyc_varargs!=0){
struct Cyc_PP_Doc*varargs_doc=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("...",sizeof(char),4U));_Tmp0[0]=_Tmp1;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" inject ",sizeof(char),9U));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_funarg2doc(({struct _tuple9*_Tmp2=_cycalloc(sizeof(struct _tuple9));_Tmp2->f0=cyc_varargs->name,_Tmp2->f1=cyc_varargs->tq,_Tmp2->f2=cyc_varargs->type;_Tmp2;}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
# 1062
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=varargs_doc,_Tmp1->tl=0;_Tmp1;}));});}}{
# 1064
struct Cyc_PP_Doc*arg_doc=Cyc_PP_group(_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat(",",sizeof(char),2U),arg_docs);
if(effopt!=0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_eff2doc(effopt);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(effc!=0)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_effconstr2doc(effc);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(qb!=0){
struct Cyc_PP_Doc*sep=effc==0?Cyc_PP_text(_tag_fat(":",sizeof(char),2U)): Cyc_PP_text(_tag_fat(",",sizeof(char),2U));
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,_Tmp0[1]=sep,({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qualbnd2doc(qb);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}{
# 1073
struct Cyc_PP_Doc*res=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(chk!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @checks(",sizeof(char),10U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(chk);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(req!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @requires(",sizeof(char),12U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(ens!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @ensures(",sizeof(char),11U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(ens);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(thrws!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @throws(",sizeof(char),10U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(thrws);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
return res;}}}
# 1085
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*q){
struct Cyc_List_List*prefix=0;
int match;
{union Cyc_Absyn_Nmspace _Tmp0=(*q).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 _Tmp1=0;goto _LL4;case 1: _Tmp1=_Tmp0.Rel_n.val;_LL4: {struct Cyc_List_List*x=_Tmp1;
# 1091
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 1095
prefix=x;}
goto _LL0;}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*x=_Tmp1;
# 1098
if(Cyc_Absynpp_gen_clean_cyclone){
_Tmp1=x;goto _LL8;}else{
# 1101
match=Cyc_Absynpp_use_curr_namespace &&({int(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;_Tmp2;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);}
# 1103
goto _LL0;}default: _Tmp1=_Tmp0.Abs_n.val;_LL8: {struct Cyc_List_List*x=_Tmp1;
# 1105
match=Cyc_Absynpp_use_curr_namespace &&({int(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;_Tmp2;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 1109
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=Cyc_Absynpp_cyc_stringptr,_Tmp2->tl=x;_Tmp2;}): x;}
goto _LL0;}}_LL0:;}
# 1112
if(Cyc_Absynpp_qvar_to_Cids){
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f1,_Tmp2->tl=0;_Tmp2;}));});return Cyc_str_sepstr(_Tmp0,_tag_fat("_",sizeof(char),2U));}
if(match)
return*(*q).f1;{
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f1,_Tmp2->tl=0;_Tmp2;}));});return Cyc_str_sepstr(_Tmp0,_tag_fat("::",sizeof(char),3U));}}
# 1119
struct _fat_ptr Cyc_Absynpp_fullqvar2string(struct _tuple1*q){
struct Cyc_List_List*prefix=0;
{union Cyc_Absyn_Nmspace _Tmp0=(*q).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 _Tmp1=0;goto _LL4;case 1: _Tmp1=_Tmp0.Rel_n.val;_LL4: {struct Cyc_List_List*x=_Tmp1;
# 1124
prefix=x;
goto _LL0;}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*x=_Tmp1;
# 1127
goto _LL0;}default: _Tmp1=_Tmp0.Abs_n.val;{struct Cyc_List_List*x=_Tmp1;
# 1129
prefix=x;
goto _LL0;}}_LL0:;}{
# 1132
struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f1,_Tmp2->tl=0;_Tmp2;}));});return Cyc_str_sepstr(_Tmp0,_tag_fat("_",sizeof(char),2U));}}
# 1136
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple1*v){
# 1138
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 1141
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _Tmp0=(*v).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 goto _LL4;case 1: if(_Tmp0.Rel_n.val==0){_LL4:
 return*(*v).f1;}else{_LLA: {
# 1150
struct _fat_ptr _Tmp2=_tag_fat("/* bad namespace : */ ",sizeof(char),23U);return Cyc_strconcat(_Tmp2,Cyc_Absynpp_qvar2string(v));}}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*l=_Tmp1;
# 1145
_Tmp1=l;goto _LL8;}default: _Tmp1=_Tmp0.Abs_n.val;_LL8: {struct Cyc_List_List*l=_Tmp1;
# 1147
if(({int(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;_Tmp2;})(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)==0)
return*(*v).f1;
goto _LLA;}};}
# 1153
return*(*v).f1;}
# 1156
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple1*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple1*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 1159
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple1*q){
struct _fat_ptr qs=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=Cyc_strconcat(_tag_fat("\\textbf{",sizeof(char),9U),qs);Cyc_strconcat(_Tmp1,_tag_fat("}",sizeof(char),2U));});return Cyc_PP_text_width(_Tmp0,(int)Cyc_strlen(qs));}
return Cyc_PP_text(qs);}
# 1166
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple1*v){
struct _fat_ptr vs=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=Cyc_strconcat(_tag_fat("\\textbf{",sizeof(char),9U),vs);Cyc_strconcat(_Tmp1,_tag_fat("}",sizeof(char),2U));});return Cyc_PP_text_width(_Tmp0,(int)Cyc_strlen(vs));}
return Cyc_PP_text(vs);}
# 1173
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);return Cyc_Absynpp_tqtd2doc(_Tmp0,t,0);}
# 1177
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_Tmp0;_Tmp0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)f)->f1;{struct _fat_ptr*n=_Tmp0;
return Cyc_PP_textptr(n);};}
# 1190 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 return 10000;case 3: _Tmp2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{enum Cyc_Absyn_Primop p=_Tmp2;
# 1195
switch((int)p){case Cyc_Absyn_Plus:
 return 100;case Cyc_Absyn_Times:
 return 110;case Cyc_Absyn_Minus:
 return 100;case Cyc_Absyn_Div:
 goto _LL61;case Cyc_Absyn_UDiv: _LL61:
 goto _LL63;case Cyc_Absyn_Mod: _LL63:
 goto _LL65;case Cyc_Absyn_UMod: _LL65:
 return 110;case Cyc_Absyn_Eq:
 goto _LL69;case Cyc_Absyn_Neq: _LL69:
 return 70;case Cyc_Absyn_Gt:
 goto _LL6D;case Cyc_Absyn_Lt: _LL6D:
 goto _LL6F;case Cyc_Absyn_Gte: _LL6F:
 goto _LL71;case Cyc_Absyn_Lte: _LL71:
 goto _LL73;case Cyc_Absyn_UGt: _LL73:
 goto _LL75;case Cyc_Absyn_ULt: _LL75:
 goto _LL77;case Cyc_Absyn_UGte: _LL77:
 goto _LL79;case Cyc_Absyn_ULte: _LL79:
 return 80;case Cyc_Absyn_Not:
 goto _LL7D;case Cyc_Absyn_Bitnot: _LL7D:
 return 130;case Cyc_Absyn_Bitand:
 return 60;case Cyc_Absyn_Bitor:
 return 40;case Cyc_Absyn_Bitxor:
 return 50;case Cyc_Absyn_Bitlshift:
 return 90;case Cyc_Absyn_Bitlrshift:
 return 80;case Cyc_Absyn_Numelts:
 return 140;case Cyc_Absyn_Tagof:
 return 140;default:
 return 140;};}case 4:
# 1224
 return 20;case 5:
 return 130;case 6:
 return 30;case 7:
 return 35;case 8:
 return 30;case 9:
 return -10;case 10:
 return 140;case 2:
 return 140;case 11:
 return 130;case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Absynpp_exp_prec(e2);}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Absynpp_exp_prec(e2);}case 14:
 return 120;case 16:
 return 15;case 15:
 goto _LL24;case 17: _LL24:
 goto _LL26;case 18: _LL26:
 goto _LL28;case 38: _LL28:
 goto _LL2A;case 39: _LL2A:
 goto _LL2C;case 37: _LL2C:
 goto _LL2E;case 19: _LL2E:
 goto _LL30;case 20: _LL30:
 goto _LL32;case 41: _LL32:
 goto _LL34;case 42: _LL34:
 goto _LL36;case 40: _LL36:
 return 130;case 21:
 goto _LL3A;case 22: _LL3A:
 goto _LL3C;case 23: _LL3C:
 return 140;case 24:
 goto _LL40;case 25: _LL40:
 goto _LL42;case 26: _LL42:
 goto _LL44;case 27: _LL44:
 goto _LL46;case 28: _LL46:
 goto _LL48;case 29: _LL48:
 goto _LL4A;case 30: _LL4A:
 goto _LL4C;case 31: _LL4C:
 goto _LL4E;case 32: _LL4E:
 goto _LL50;case 33: _LL50:
 goto _LL52;case 34: _LL52:
 goto _LL54;case 35: _LL54:
 return 140;default:
 return 10000;};}struct _tuple19{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
# 1267
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
{struct _tuple19*_Tmp0=(struct _tuple19*)o->hd;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct _fat_ptr c=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
s=({struct Cyc_PP_Doc*_Tmp3[6];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(c);_Tmp3[2]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("\" (",sizeof(char),4U));_Tmp3[3]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_exp2doc(e);_Tmp3[4]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp3[5]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),6));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp3[2];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_Tmp3[1]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1270
1U;}
# 1276
return s;}
# 1279
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(o);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": : ",sizeof(char),5U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1287
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1289
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,ncol==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": : :",sizeof(char),6U));_Tmp0[1]=_Tmp1;}):(ncol==1?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : : ",sizeof(char),6U));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}));Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
while(cl!=0){
s=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(", ",sizeof(char),3U));_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
# 1293
1U;}}
# 1299
return s;}char Cyc_Absynpp_Implicit_region[16U]="Implicit_region";struct Cyc_Absynpp_Implicit_region_exn_struct{char*tag;};
# 1303
struct Cyc_Absynpp_Implicit_region_exn_struct Cyc_Absynpp_Implicit_region_val={Cyc_Absynpp_Implicit_region};
# 1305
static int Cyc_Absynpp_may_contain_implicit_regions(int*res,void*t){
if(Cyc_Tcutil_type_kind(t)==& Cyc_Kinds_ek){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 1309
if(Cyc_Tcutil_is_temp_tvar(tv)){
*_check_null(res)=1;
_throw(& Cyc_Absynpp_Implicit_region_val);}
# 1313
goto _LL0;}}else{
# 1315
goto _LL0;}_LL0:;}
# 1318
return 1;}
# 1321
static int Cyc_Absynpp_is_clean_cast(void*nt,struct Cyc_Absyn_Exp*e){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=nt,_Tmp1.f1=e->topt;_Tmp1;});struct Cyc_Absyn_PtrInfo _Tmp1;if(*((int*)_Tmp0.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==1){if(_Tmp0.f1!=0){if(*((int*)_Tmp0.f1)==4){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_Tmp1;
# 1324
void*_Tmp2=p1.ptr_atts.bounds;switch(*((int*)_Tmp2)){case 3:
 goto _LL9;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==14){_LL9:
# 1327
 return 0;}else{goto _LLA;}default: _LLA:
# 1331
 return 1;};}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: {
# 1334
void*_Tmp2=e->r;if(*((int*)_Tmp2)==12)
# 1336
return 0;else{
# 1338
int br=0;
struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
({void(*_Tmp5)(int(*)(int*,void*),int*,void*)=(void(*)(int(*)(int*,void*),int*,void*))Cyc_Absyn_visit_type;_Tmp5;})(Cyc_Absynpp_may_contain_implicit_regions,& br,nt);{
int _Tmp5=1;_npop_handler(0);return _Tmp5;}
# 1340
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Absynpp_Implicit_region_exn_struct*)_Tmp5)->tag==Cyc_Absynpp_Implicit_region)
# 1345
return 0;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}};}};}
# 1351
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_Tmp0=e->r;void*_Tmp1;enum Cyc_Absyn_MallocKind _Tmp2;enum Cyc_Absyn_Coercion _Tmp3;int _Tmp4;enum Cyc_Absyn_Incrementor _Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Primop _Tmp8;struct _fat_ptr _Tmp9;void*_TmpA;union Cyc_Absyn_Cnst _TmpB;switch(*((int*)_Tmp0)){case 0: _TmpB=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_TmpB;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1: _TmpA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_TmpA;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _Tmp9=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp9;
# 1358
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("__cyclone_pragma__",sizeof(char),19U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(p);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 3: _Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp8;struct Cyc_List_List*es=_TmpA;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4: _TmpA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Core_Opt*popt=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1361
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_TmpC[1]=_TmpD;}),
popt==0?({struct Cyc_PP_Doc*_TmpD=Cyc_PP_nil_doc();_TmpC[2]=_TmpD;}):({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("= ",sizeof(char),3U));_TmpC[3]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _TmpA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_TmpA;enum Cyc_Absyn_Incrementor i=_Tmp5;
# 1368
struct Cyc_PP_Doc*es=Cyc_Absynpp_exp2doc_prec(myprec,e2);
switch((int)i){case Cyc_Absyn_PreInc:
 s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("++",sizeof(char),3U));_TmpC[0]=_TmpD;}),_TmpC[1]=es;Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PreDec:
 s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("--",sizeof(char),3U));_TmpC[0]=_TmpD;}),_TmpC[1]=es;Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostInc:
 s=({struct Cyc_PP_Doc*_TmpC[2];_TmpC[0]=es,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("++",sizeof(char),3U));_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;case Cyc_Absyn_PostDec:
 goto _LL61;default: _LL61:
 s=({struct Cyc_PP_Doc*_TmpC[2];_TmpC[0]=es,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("--",sizeof(char),3U));_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL57;}_LL57:;
# 1376
goto _LL0;}case 6: _TmpA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;struct Cyc_Absyn_Exp*e3=_Tmp6;
# 1378
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" ? ",sizeof(char),4U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(0,e2);_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e3);_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 7: _TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1382
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" && ",sizeof(char),5U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 8: _TmpA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1385
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" || ",sizeof(char),5U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 9: _TmpA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1390
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(", ",sizeof(char),3U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 10: _TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp7;
# 1393
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exps2doc_prec(20,es);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 11: _TmpA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("throw ",sizeof(char),7U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 12: _TmpA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 1397
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);
if(Cyc_Absynpp_gen_clean_cyclone)
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),_TmpC[1]=s,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("<>",sizeof(char),3U));_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
# 1401
goto _LL0;}case 13: _TmpA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*ts=_Tmp7;
# 1403
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);
if(Cyc_Absynpp_gen_clean_cyclone){
struct Cyc_PP_Doc*inst=Cyc_Absynpp_tps2doc(ts);
if(!Cyc_Absynpp_emptydoc(inst))
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),_TmpC[1]=s,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("@",sizeof(char),2U));_TmpC[3]=_TmpD;}),_TmpC[4]=inst;Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});}
# 1409
goto _LL0;}case 14: _TmpA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;int u=_Tmp4;enum Cyc_Absyn_Coercion c=_Tmp3;
# 1411
struct Cyc_PP_Doc*castStr=Cyc_PP_nil_doc();
int printType=1;
if(Cyc_Absynpp_gen_clean_cyclone && !u)
printType=Cyc_Absynpp_is_clean_cast(t,e1);
# 1416
if(printType)
castStr=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
s=({struct Cyc_PP_Doc*_TmpC[2];_TmpC[0]=castStr,({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 15: _TmpA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("&",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 16: _TmpA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*ropt=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*qual=_Tmp6;
# 1422
if(ropt==0){
struct Cyc_PP_Doc*qd=Cyc_PP_nil_doc();
if((unsigned)qual)
qd=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(qual);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
# 1427
s=({struct Cyc_PP_Doc*_TmpC[3];qual==0?({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("new ",sizeof(char),5U));_TmpC[0]=_TmpD;}):({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("qnew ",sizeof(char),6U));_TmpC[0]=_TmpD;}),_TmpC[1]=qd,({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1430
struct Cyc_PP_Doc*qd=Cyc_PP_nil_doc();
if((unsigned)qual)
qd=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(qual);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});
# 1434
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("rnew(",sizeof(char),6U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(ropt);_TmpC[1]=_TmpD;}),_TmpC[2]=qd,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});}
# 1436
goto _LL0;}case 17: _TmpA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("sizeof(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 18: _TmpA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("sizeof(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e1);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 41: _TmpA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;int static_only=_Tmp4;
# 1440
if(static_only)
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("@assert(",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1443
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("assert(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL0;}case 42: _TmpA=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("@assert_false(",sizeof(char),15U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 40: _TmpA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 1447
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("__extension__(",sizeof(char),15U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 38: _TmpA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("valueof(",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 39: _Tmp4=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_TmpA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp6=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int vol=_Tmp4;struct _fat_ptr t=_Tmp9;struct Cyc_List_List*o=_TmpA;struct Cyc_List_List*i=_Tmp7;struct Cyc_List_List*cl=_Tmp6;
# 1450
if(vol)
s=({struct Cyc_PP_Doc*_TmpC[7];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("__asm__",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" volatile (",sizeof(char),12U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpC[4]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpC[5]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[6]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1453
s=({struct Cyc_PP_Doc*_TmpC[6];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("__asm__(",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpC[4]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[5]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),6));});}
goto _LL0;}case 37: _TmpA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;struct _fat_ptr*f=_Tmp7;
# 1456
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("tagcheck(",sizeof(char),10U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_textptr(f);_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 19: _TmpA=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_TmpA;struct Cyc_List_List*l=_Tmp7;
# 1459
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("offsetof(",sizeof(char),10U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=({struct _fat_ptr _TmpE=_tag_fat(".",sizeof(char),2U);Cyc_PP_seq(_TmpE,({struct Cyc_List_List*(*_TmpF)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_TmpF;})(Cyc_Absynpp_offsetof_field_to_doc,l));});_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 20: _TmpA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 21: _TmpA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*n=_Tmp7;
# 1464
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_textptr(n);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 22: _TmpA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*n=_Tmp7;
# 1466
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("->",sizeof(char),3U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_textptr(n);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 23: _TmpA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1468
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("[",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e2);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 24: _TmpA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple9*vat=_TmpA;struct Cyc_List_List*des=_Tmp7;
# 1470
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((*vat).f2);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=({struct _fat_ptr _TmpE=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpE,({struct Cyc_List_List*(*_TmpF)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpF;})(Cyc_Absynpp_de2doc,des));});_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 25: _TmpA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_TmpA;
s=({struct _fat_ptr _TmpC=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpC,({struct Cyc_List_List*(*_TmpD)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 26: _TmpA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1475
s=({struct Cyc_PP_Doc*_TmpC[8];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("for ",sizeof(char),5U));_TmpC[1]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(*(*vd->name).f1);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" < ",sizeof(char),4U));_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e1);_TmpC[4]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpC[5]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e2);_TmpC[6]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[7]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 27: _TmpA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;void*t=_Tmp7;
# 1480
s=({struct Cyc_PP_Doc*_TmpC[7];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("for x ",sizeof(char),7U));_TmpC[1]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" < ",sizeof(char),4U));_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[3]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpC[4]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[5]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[6]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 28: _TmpA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct _tuple1*n=_TmpA;struct Cyc_List_List*ts=_Tmp7;struct Cyc_List_List*des=_Tmp6;
# 1488
struct Cyc_List_List*des_doc=({struct Cyc_List_List*(*_TmpC)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpC;})(Cyc_Absynpp_de2doc,des);
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(n);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({
# 1491
struct Cyc_PP_Doc*_TmpD=Cyc_PP_seq(_tag_fat(",",sizeof(char),2U),des_doc);_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
# 1495
goto _LL0;}case 29: _Tmp4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_TmpA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{int is_tuple=_Tmp4;struct Cyc_List_List*des=_TmpA;
# 1497
if(is_tuple)
s=({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_dollar();_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exps2doc_prec(20,({struct Cyc_List_List*(*_TmpE)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(({struct Cyc_Absyn_Exp*(*_TmpE)(struct _tuple17*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd;_TmpE;}),des));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1500
s=({struct _fat_ptr _TmpC=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpC,({struct Cyc_List_List*(*_TmpD)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(Cyc_Absynpp_de2doc,des));});}
goto _LL0;}case 30: _TmpA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;
# 1504
s=({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(tuf->name);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=({struct _fat_ptr _TmpE=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpF=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _Tmp10=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpE,_TmpF,_Tmp10,({struct Cyc_List_List*(*_Tmp11)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;_Tmp11;})(Cyc_Absynpp_exp2doc,es));});_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 31: _TmpA=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_TmpA;
_TmpA=ef;goto _LL4E;}case 32: _TmpA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4E: {struct Cyc_Absyn_Enumfield*ef=_TmpA;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 33: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_TmpA=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp7=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp2;struct Cyc_Absyn_Exp*rgnopt=_TmpA;struct Cyc_Absyn_Exp*aqopt=_Tmp7;void**topt=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp1;int inline_call=_Tmp4;
# 1511
if((int)mknd==1){
# 1513
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U);
if(rgnopt==0)
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("calloc(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(st);_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1518
s=({struct Cyc_PP_Doc*_TmpC[7];aqopt!=0?({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("qcalloc(",sizeof(char),9U));_TmpC[0]=_TmpD;}):({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("rcalloc(",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(rgnopt);_TmpC[1]=_TmpD;}),
aqopt!=0?({struct Cyc_PP_Doc*_TmpD=({struct Cyc_PP_Doc*_TmpE[3];({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpE[0]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_exp2doc(aqopt);_TmpE[1]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpE[2]=_TmpF;});Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),3));});_TmpC[2]=_TmpD;}):({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[3]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[4]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(st);_TmpC[5]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[6]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1523
struct Cyc_PP_Doc*new_e=Cyc_Absynpp_exp2doc(e);
(unsigned)topt?0:({int(*_TmpC)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_TmpC;})(_tag_fat("topt",sizeof(char),5U),_tag_fat("absynpp.cyc",sizeof(char),12U),1524U);{
# 1526
struct _fat_ptr fn_name=_tag_fat("malloc(",sizeof(char),8U);
if(rgnopt==0)
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(fn_name);_TmpC[0]=_TmpD;}),_TmpC[1]=new_e,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1531
if((int)mknd==2)fn_name=_tag_fat("rvmalloc(",sizeof(char),10U);else{
if(inline_call)fn_name=_tag_fat("rmalloc_inline(",sizeof(char),16U);else{
if(aqopt!=0)fn_name=_tag_fat("qmalloc(",sizeof(char),9U);else{
fn_name=_tag_fat("rmalloc(",sizeof(char),9U);}}}
s=({struct Cyc_PP_Doc*_TmpC[5];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(fn_name);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(rgnopt);_TmpC[1]=_TmpD;}),
aqopt!=0?({struct Cyc_PP_Doc*_TmpD=({struct Cyc_PP_Doc*_TmpE[3];({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpE[0]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_exp2doc(aqopt);_TmpE[1]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpE[2]=_TmpF;});Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),3));});_TmpC[2]=_TmpD;}):({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpC[2]=_TmpD;}),_TmpC[3]=new_e,({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[4]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),5));});}}}
# 1540
goto _LL0;}case 34: _TmpA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1542
s=({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(" :=: ",sizeof(char),6U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 35: _TmpA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*n=_TmpA;struct Cyc_List_List*des=_Tmp7;
# 1545
s=({struct _fat_ptr _TmpC=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpC,({struct Cyc_List_List*(*_TmpD)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _TmpA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s2=_TmpA;
# 1548
s=({struct Cyc_PP_Doc*_TmpC[7];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_blank_doc();_TmpC[2]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0,1));_TmpC[3]=_TmpD;}),({
struct Cyc_PP_Doc*_TmpD=Cyc_PP_blank_doc();_TmpC[4]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[5]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[6]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}}_LL0:;}
# 1553
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1557
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1561
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_Tmp0;if(*((int*)d)==0){_Tmp0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)d)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
struct Cyc_PP_Doc*_Tmp1[3];({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat(".[",sizeof(char),3U));_Tmp1[0]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_exp2doc(e);_Tmp1[1]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_Tmp1[2]=_Tmp2;});return Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),3));}}else{_Tmp0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)d)->f1;{struct _fat_ptr*v=_Tmp0;
struct Cyc_PP_Doc*_Tmp1[2];({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_Tmp1[0]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_textptr(v);_Tmp1[1]=_Tmp2;});return Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),2));}};}
# 1568
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple17*de){
if((*de).f0==0)return Cyc_Absynpp_exp2doc((*de).f1);{
struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("=",sizeof(char),2U);struct _fat_ptr _Tmp4=_tag_fat("=",sizeof(char),2U);Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({struct Cyc_List_List*(*_Tmp5)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp5;})(Cyc_Absynpp_designator2doc,(*de).f0));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((*de).f1);_Tmp0[1]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}}
# 1574
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);return Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*(*_Tmp3)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp3;})(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1578
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=_tag_fat(({unsigned _Tmp0=28U + 1U;({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=28U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='z';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;});}),sizeof(char),29U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),27);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),26);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),25);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),24);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='U';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),23);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});{
int index=23;
while(i!=0U){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),index --);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48U + i % 10U);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
i=i / 10U;
# 1587
1U;}
# 1590
return _fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1594
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
long long _Tmp0;int _Tmp1;short _Tmp2;struct _fat_ptr _Tmp3;char _Tmp4;enum Cyc_Absyn_Sign _Tmp5;switch(c.String_c.tag){case 2: _Tmp5=c.Char_c.val.f0;_Tmp4=c.Char_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;char ch=_Tmp4;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_char_escape(ch);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("'%s'",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 3: _Tmp3=c.Wchar_c.val;{struct _fat_ptr s=_Tmp3;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("L'%s'",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 4: _Tmp5=c.Short_c.val.f0;_Tmp2=c.Short_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;short s=_Tmp2;
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)s);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 5: _Tmp5=c.Int_c.val.f0;_Tmp1=c.Int_c.val.f1;{enum Cyc_Absyn_Sign sn=_Tmp5;int i=_Tmp1;
# 1600
if((int)sn==1)return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%uU",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));}));
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 6: _Tmp5=c.LongLong_c.val.f0;_Tmp0=c.LongLong_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;long long i=_Tmp0;
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7: _Tmp3=c.Float_c.val.f0;{struct _fat_ptr x=_Tmp3;
return Cyc_PP_text(x);}case 1:
 return Cyc_PP_text(_tag_fat("NULL",sizeof(char),5U));case 8: _Tmp3=c.String_c.val;{struct _fat_ptr s=_Tmp3;
struct Cyc_PP_Doc*_Tmp6[3];({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_Tmp6[1]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[2]=_Tmp7;});return Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),3));}default: _Tmp3=c.Wstring_c.val;{struct _fat_ptr s=_Tmp3;
struct Cyc_PP_Doc*_Tmp6[3];({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("L\"",sizeof(char),3U));_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(s);_Tmp6[1]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[2]=_Tmp7;});return Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),3));}};}
# 1610
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p==18){
if(es==0 || es->tl!=0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_PP_string_of_doc(ps,72);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({
# 1614
int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Absynpp::primapp2doc Numelts: %s with bad args",sizeof(char),47U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1616
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("numelts(",sizeof(char),9U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}}else{
if((int)p==19){
if(es==0 || es->tl!=0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_PP_string_of_doc(ps,72);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({
# 1619
int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Absynpp::primapp2doc Tagof: %s with bad args",sizeof(char),45U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1621
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("tagof(",sizeof(char),7U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}}}{
# 1623
struct Cyc_List_List*ds=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp0;})(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds==0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_PP_string_of_doc(ps,72);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Absynpp::primapp2doc: %s with no args",sizeof(char),38U),_tag_fat(_Tmp1,sizeof(void*),1));});
if(ds->tl==0){
struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=(struct Cyc_PP_Doc*)ds->hd;return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}
if(ds->tl->tl!=0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_PP_string_of_doc(ps,72);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({
# 1629
int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Absynpp::primapp2doc: %s with more than 2 args",sizeof(char),47U),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1631
struct Cyc_PP_Doc*_Tmp0[5];_Tmp0[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[3]=_Tmp1;}),_Tmp0[4]=(struct Cyc_PP_Doc*)_check_null(ds->tl)->hd;return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));}}}
# 1634
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return _tag_fat("+",sizeof(char),2U);case Cyc_Absyn_Times:
 return _tag_fat("*",sizeof(char),2U);case Cyc_Absyn_Minus:
 return _tag_fat("-",sizeof(char),2U);case Cyc_Absyn_UDiv:
 goto _LLA;case Cyc_Absyn_Div: _LLA:
 return _tag_fat("/",sizeof(char),2U);case Cyc_Absyn_UMod:
 goto _LLE;case Cyc_Absyn_Mod: _LLE:
 if(Cyc_Absynpp_print_for_cycdoc)return _tag_fat("\\%",sizeof(char),3U);else{return _tag_fat("%",sizeof(char),2U);}case Cyc_Absyn_Eq:
 return _tag_fat("==",sizeof(char),3U);case Cyc_Absyn_Neq:
 return _tag_fat("!=",sizeof(char),3U);case Cyc_Absyn_UGt:
 goto _LL16;case Cyc_Absyn_Gt: _LL16:
 return _tag_fat(">",sizeof(char),2U);case Cyc_Absyn_ULt:
 goto _LL1A;case Cyc_Absyn_Lt: _LL1A:
 return _tag_fat("<",sizeof(char),2U);case Cyc_Absyn_UGte:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1E:
 return _tag_fat(">=",sizeof(char),3U);case Cyc_Absyn_ULte:
 goto _LL22;case Cyc_Absyn_Lte: _LL22:
 return _tag_fat("<=",sizeof(char),3U);case Cyc_Absyn_Not:
 return _tag_fat("!",sizeof(char),2U);case Cyc_Absyn_Bitnot:
 return _tag_fat("~",sizeof(char),2U);case Cyc_Absyn_Bitand:
 return _tag_fat("&",sizeof(char),2U);case Cyc_Absyn_Bitor:
 return _tag_fat("|",sizeof(char),2U);case Cyc_Absyn_Bitxor:
 return _tag_fat("^",sizeof(char),2U);case Cyc_Absyn_Bitlshift:
 return _tag_fat("<<",sizeof(char),3U);case Cyc_Absyn_Bitlrshift:
 return _tag_fat(">>",sizeof(char),3U);case Cyc_Absyn_Numelts:
 return _tag_fat("numelts",sizeof(char),8U);case Cyc_Absyn_Tagof:
 return _tag_fat("tagof",sizeof(char),6U);default:
 return _tag_fat("?",sizeof(char),2U);};}
# 1665
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1669
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;if(*((int*)_Tmp0)==12)
return 1;else{
return 0;};}
# 1675
static int Cyc_Absynpp_is_region_decl(struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;if(*((int*)_Tmp0)==4)
return 1;else{
return 0;};}
# 1690 "absynpp.cyc"
struct _tuple15 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_Tmp0=d->r;void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1693
if(({int(*_Tmp2)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*,struct _tuple1*)=(int(*)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*,struct _tuple1*))Cyc_List_mem;_Tmp2;})(Cyc_Absyn_qvar_cmp,varsinblock,vd->name)){
struct _tuple15 _Tmp2;_Tmp2.f0=1,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=0;_Tmp4;});_Tmp2.f1=_Tmp3;});return _Tmp2;}{
struct _tuple15 _Tmp2;_Tmp2.f0=0,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=varsinblock;_Tmp4;});_Tmp2.f1=_Tmp3;});return _Tmp2;}}}else{
struct _tuple15 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=varsinblock;return _Tmp2;};}
# 1700
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp){
struct Cyc_PP_Doc*_Tmp0[8];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[5]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp0[6]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[7]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),8));}{
struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[5]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));}}
# 1707
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock,int prevdecl){
void*_Tmp0=st->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return Cyc_PP_text(_tag_fat(";",sizeof(char),2U));case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 1712
if(!Cyc_Absynpp_decls_first){
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,0,prevdecl);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,0,0);_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}
if(Cyc_Absynpp_is_declaration(s1)){
struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,prevdecl));_Tmp5[0]=_Tmp6;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0,0));});_Tmp5[1]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock,0);_Tmp5[1]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));}
if(Cyc_Absynpp_is_declaration(s2)){
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock,prevdecl);_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0,0));});_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}{
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock,prevdecl);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock,0);_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
# 1726
if(eopt==0)
return Cyc_PP_text(_tag_fat("return;",sizeof(char),8U));{
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("return ",sizeof(char),8U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(eopt);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 1730
int print_else;
{void*_Tmp5=s2->r;if(*((int*)_Tmp5)==0){
print_else=0;goto _LL23;}else{
print_else=1;goto _LL23;}_LL23:;}{
# 1735
struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("if (",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,1));_Tmp5[3]=_Tmp6;}),
print_else?({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat("else ",sizeof(char),6U));_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0,1));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 1740
_Tmp5[4]=_Tmp6;}):({
# 1743
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[4]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));}}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1745
struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("while (",sizeof(char),8U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,1));_Tmp5[3]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));}case 6:
 return Cyc_PP_text(_tag_fat("break;",sizeof(char),7U));case 7:
 return Cyc_PP_text(_tag_fat("continue;",sizeof(char),10U));case 8: _Tmp4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct _fat_ptr*x=_Tmp4;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*x;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("goto %s;",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),1));}));}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 1751
struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("for(",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e1);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("; ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e2);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("; ",sizeof(char),3U));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e3);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,1));_Tmp5[7]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1760
struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("switch (",sizeof(char),9U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_switchclauses2doc(ss);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0)
return Cyc_PP_text(_tag_fat("fallthru;",sizeof(char),10U));else{_Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 1770
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("fallthru(",sizeof(char),10U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exps2doc_prec(20,es);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1772
struct _tuple15 _Tmp5=Cyc_Absynpp_shadows(d,varsinblock);void*_Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int newblock=_Tmp7;struct Cyc_List_List*newvarsinblock=_Tmp6;
struct Cyc_PP_Doc*s=({struct Cyc_PP_Doc*_Tmp8[3];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_decl2doc(d);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_line_doc();_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock,1);_Tmp8[2]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),3));});
if(newblock || Cyc_Absynpp_gen_clean_cyclone)s=Cyc_Absynpp_block(stmtexp,s);
# 1776
return s;}}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*x=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1778
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1)){
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s1,stmtexp,0,0));});_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}{
struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock,0);_Tmp5[2]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));}}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1782
struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("do ",sizeof(char),4U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,1));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" while (",sizeof(char),9U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp5[4]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1788
struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("try ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0,1));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" catch ",sizeof(char),8U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_Tmp5[3]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));}};}
# 1795
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct _fat_ptr _Tmp5;char _Tmp6;int _Tmp7;enum Cyc_Absyn_Sign _Tmp8;switch(*((int*)_Tmp0)){case 0:
 return Cyc_PP_text(_tag_fat("_",sizeof(char),2U));case 8:
 return Cyc_PP_text(_tag_fat("NULL",sizeof(char),5U));case 9: _Tmp8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign sg=_Tmp8;int i=_Tmp7;
# 1800
if((int)sg!=1)
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)i;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_TmpA,sizeof(void*),1));}));
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned)i;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("%u",sizeof(char),3U),_tag_fat(_TmpA,sizeof(void*),1));}));}case 10: _Tmp6=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char ch=_Tmp6;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Absynpp_char_escape(ch);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("'%s'",sizeof(char),5U),_tag_fat(_TmpA,sizeof(void*),1));}));}case 11: _Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr x=_Tmp5;
return Cyc_PP_text(x);}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return Cyc_Absynpp_qvar2doc(vd->name);}}else{_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
struct Cyc_PP_Doc*_Tmp9[3];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" as ",sizeof(char),5U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[2]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),3));}}case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1808
struct Cyc_PP_Doc*_Tmp9[5];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("alias",sizeof(char),6U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" <",sizeof(char),3U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_tvar2doc(tv);_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("> ",sizeof(char),3U));_Tmp9[3]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_vardecl2doc(vd,0);_Tmp9[4]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),5));}case 4: _Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1810
struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("<",sizeof(char),2U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_tvar2doc(tv);_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_Tmp9[3]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));}case 5: _Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("&",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[1]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 1813
struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[1]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));}}else{_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 1815
struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" as ",sizeof(char),5U));_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[3]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));}}case 14: _Tmp4=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple1*q=_Tmp4;
return Cyc_Absynpp_qvar2doc(q);}case 15: _Tmp4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct _tuple1*q=_Tmp4;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1818
struct _fat_ptr term=dots?_tag_fat(", ...)",sizeof(char),7U): _tag_fat(")",sizeof(char),2U);
struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(q);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpB,_TmpC,_TmpD,({struct Cyc_List_List*(*_TmpE)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(Cyc_Absynpp_pat2doc,ps));});_Tmp9[1]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));}case 6: _Tmp4=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{void*topt=_Tmp4;int is_tuple=_Tmp7;struct Cyc_List_List*exists=_Tmp3;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp1;
# 1821
if(is_tuple){
struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_dollar();_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct Cyc_PP_Doc*(*_TmpB)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*)=({struct Cyc_PP_Doc*(*_TmpC)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;_TmpC;});struct _fat_ptr _TmpC=_tag_fat(",",sizeof(char),2U);_TmpB(Cyc_Absynpp_pat2doc,_TmpC,({struct Cyc_List_List*(*_TmpD)(struct Cyc_Absyn_Pat*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Pat*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(({struct Cyc_Absyn_Pat*(*_TmpD)(struct _tuple16*)=(struct Cyc_Absyn_Pat*(*)(struct _tuple16*))Cyc_Core_snd;_TmpD;}),dps));});_Tmp9[2]=_TmpA;}),
dots?({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(", ...)",sizeof(char),7U));_Tmp9[3]=_TmpA;}):({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp9[3]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));}else{
# 1825
struct _fat_ptr term=dots?_tag_fat(", ...}",sizeof(char),7U): _tag_fat("}",sizeof(char),2U);
if(topt!=0){
void*_Tmp9=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _TmpA;if(*((int*)_Tmp9)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)==24){_TmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_TmpA;
# 1829
struct _tuple12 _TmpB=Cyc_Absyn_aggr_kinded_name(info);void*_TmpC;_TmpC=_TmpB.f1;{struct _tuple1*n=_TmpC;
struct Cyc_PP_Doc*_TmpD[4];({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_qvar2doc(n);_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({
struct Cyc_PP_Doc*_TmpE=({struct _fat_ptr _TmpF=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp10=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp11=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpF,_Tmp10,_Tmp11,({struct Cyc_List_List*(*_Tmp12)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;_Tmp12;})(Cyc_Absynpp_tvar2doc,exists));});_TmpD[2]=_TmpE;}),({
struct Cyc_PP_Doc*_TmpE=({struct _fat_ptr _TmpF=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp10=term;struct _fat_ptr _Tmp11=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpF,_Tmp10,_Tmp11,({struct Cyc_List_List*(*_Tmp12)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_Tmp12;})(Cyc_Absynpp_dp2doc,dps));});_TmpD[3]=_TmpE;});return Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));}}}else{goto _LL2C;}}else{_LL2C:
 goto _LL29;}_LL29:;}{
# 1836
struct Cyc_PP_Doc*_Tmp9[3];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_lb();_Tmp9[0]=_TmpA;}),({
struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _TmpC=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpB,_TmpC,_TmpD,({struct Cyc_List_List*(*_TmpE)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(Cyc_Absynpp_tvar2doc,exists));});_Tmp9[1]=_TmpA;}),({
struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("",sizeof(char),1U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpB,_TmpC,_TmpD,({struct Cyc_List_List*(*_TmpE)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(Cyc_Absynpp_dp2doc,dps));});_Tmp9[2]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),3));}}}case 12: _Tmp4=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp4;
# 1840
_Tmp4=ef;goto _LL22;}case 13: _Tmp4=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL22: {struct Cyc_Absyn_Enumfield*ef=_Tmp4;
return Cyc_Absynpp_qvar2doc(ef->name);}case 7: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
return Cyc_Absynpp_qvar2doc(tuf->name);}}else{_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1844
struct _fat_ptr term=dots?_tag_fat(", ...)",sizeof(char),7U): _tag_fat(")",sizeof(char),2U);
struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(tuf->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpB,_TmpC,_TmpD,({struct Cyc_List_List*(*_TmpE)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(Cyc_Absynpp_pat2doc,ps));});_Tmp9[1]=_TmpA;});return Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));}}default: _Tmp4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_Absynpp_exp2doc(e);}};}
# 1850
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple16*dp){
struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("=",sizeof(char),2U);struct _fat_ptr _Tmp4=_tag_fat("=",sizeof(char),2U);Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({struct Cyc_List_List*(*_Tmp5)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp5;})(Cyc_Absynpp_designator2doc,(*dp).f0));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc((*dp).f1);_Tmp0[1]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}
# 1855
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1857
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause==0 && c->pattern->r==(void*)& Cyc_Absyn_Wild_p_val){
struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("default: ",sizeof(char),10U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[1]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}
if(c->where_clause==0){
struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("case ",sizeof(char),6U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[3]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));}{
struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("case ",sizeof(char),6U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" && ",sizeof(char),5U));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(c->where_clause));_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[4]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[5]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));}}
# 1876
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp0;})(Cyc_Absynpp_switchclause2doc,_tag_fat("",sizeof(char),1U),cs);}
# 1880
static struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag==0)
return Cyc_Absynpp_qvar2doc(f->name);{
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2doc(f->name);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(f->tag));_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}}
# 1885
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp0;})(Cyc_Absynpp_enumfield2doc,_tag_fat(",",sizeof(char),2U),fs);}
# 1889
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1892
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;_Tmp0;})(Cyc_Absynpp_id2doc,_tag_fat(",",sizeof(char),2U),vds);}
# 1897
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd,int add_semicolon){
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;unsigned _Tmp5;void*_Tmp6;enum Cyc_Absyn_Scope _Tmp7;_Tmp7=vd->sc;_Tmp6=vd->name;_Tmp5=vd->varloc;_Tmp4=vd->tq;_Tmp3=vd->type;_Tmp2=vd->initializer;_Tmp1=vd->attributes;_Tmp0=vd->rename;{enum Cyc_Absyn_Scope sc=_Tmp7;struct _tuple1*name=_Tmp6;unsigned varloc=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*type=_Tmp3;struct Cyc_Absyn_Exp*initializer=_Tmp2;struct Cyc_List_List*atts=_Tmp1;struct Cyc_Absyn_Exp*rename=_Tmp0;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL3;}else{
# 1906
{void*_Tmp8=Cyc_Absyn_compress(type);void*_Tmp9;if(*((int*)_Tmp8)==6){_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp8)->f1.attributes;{struct Cyc_List_List*atts2=_Tmp9;
# 1908
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{
beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1912
goto _LL3;}_LL3:;{
# 1915
struct Cyc_PP_Doc*tmp_doc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{
tmp_doc=attsdoc;goto _LLD;}_LLD:;
# 1920
s=({struct Cyc_PP_Doc*_Tmp8[6];_Tmp8[0]=tmp_doc,({
struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_scope2doc(sc);_Tmp8[1]=_Tmp9;}),({
struct Cyc_PP_Doc*_Tmp9=({struct Cyc_Absyn_Tqual _TmpA=tq;void*_TmpB=type;Cyc_Absynpp_tqtd2doc(_TmpA,_TmpB,({struct Cyc_Core_Opt*_TmpC=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_TmpD=({struct Cyc_PP_Doc*_TmpE[2];_TmpE[0]=beforenamedoc,_TmpE[1]=sn;Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),2));});_TmpC->v=_TmpD;});_TmpC;}));});_Tmp8[2]=_Tmp9;}),
rename==0?({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_nil_doc();_Tmp8[3]=_Tmp9;}):({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_exp2doc(rename);_Tmp8[3]=_Tmp9;}),
initializer==0?({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_nil_doc();_Tmp8[4]=_Tmp9;}):({struct Cyc_PP_Doc*_Tmp9=({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_TmpA[0]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(initializer);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});_Tmp8[4]=_Tmp9;}),
add_semicolon?({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp8[5]=_Tmp9;}):({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_nil_doc();_Tmp8[5]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),6));});
return s;}}}struct _tuple20{unsigned f0;struct _tuple1*f1;int f2;};
# 1929
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
return Cyc_Absynpp_qvar2doc((*x).f1);}
# 1933
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl==0){
struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[3]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));}{
struct Cyc_PP_Doc*_Tmp0[14];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),
_check_null(ad->impl)->tagged?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("@tagged ",sizeof(char),9U));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[4]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[5]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[6]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(_check_null(ad->impl)->exist_vars);_Tmp0[7]=_Tmp1;}),
_check_null(ad->impl)->effconstr==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[8]=_Tmp1;}):({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_effconstr2doc(_check_null(ad->impl)->effconstr);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});_Tmp0[8]=_Tmp1;}),
_check_null(ad->impl)->qual_bnd==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[9]=_Tmp1;}):({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(ad->impl->effconstr==0?_tag_fat(":",sizeof(char),2U): _tag_fat(",",sizeof(char),2U));_Tmp2[0]=_Tmp3;}),({
struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_qualbnd2doc(_check_null(ad->impl)->qual_bnd);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});
# 1949
_Tmp0[9]=_Tmp1;}),({
# 1951
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_aggrfields2doc(_check_null(ad->impl)->fields);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[10]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[11]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[12]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(ad->attributes);_Tmp0[13]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),14));}}
# 1957
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Scope _Tmp4;_Tmp4=dd->sc;_Tmp3=dd->name;_Tmp2=dd->tvs;_Tmp1=dd->fields;_Tmp0=dd->is_extensible;{enum Cyc_Absyn_Scope sc=_Tmp4;struct _tuple1*name=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_Core_Opt*fields=_Tmp1;int is_x=_Tmp0;
if(fields==0){
struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_scope2doc(sc);_Tmp5[0]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U));_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));_Tmp5[2]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2bolddoc(name);_Tmp5[3]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ktvars2doc(tvs);_Tmp5[4]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));}{
struct Cyc_PP_Doc*_Tmp5[10];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_scope2doc(sc);_Tmp5[0]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U));_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));_Tmp5[2]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2bolddoc(name);_Tmp5[3]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ktvars2doc(tvs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp7[2];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_Tmp7[1]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[8]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[9]=_Tmp6;});return Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),10));}}}
# 1976
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=ed->sc;_Tmp1=ed->name;_Tmp0=ed->fields;{enum Cyc_Absyn_Scope sc=_Tmp2;struct _tuple1*n=_Tmp1;struct Cyc_Core_Opt*fields=_Tmp0;
if(fields==0){
struct Cyc_PP_Doc*_Tmp3[3];({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_scope2doc(sc);_Tmp3[0]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_typedef_name2bolddoc(n);_Tmp3[2]=_Tmp4;});return Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),3));}{
struct Cyc_PP_Doc*_Tmp3[8];({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_scope2doc(sc);_Tmp3[0]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_Tmp3[1]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_qvar2bolddoc(n);_Tmp3[2]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_blank_doc();_Tmp3[3]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_lb();_Tmp3[4]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp3[5]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_line_doc();_Tmp3[6]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rb();_Tmp3[7]=_Tmp4;});return Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),8));}}}
# 1989
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1993
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=6,_Tmp5->f1=type_info;_Tmp5;});
if(fd->cached_type!=0){
void*_Tmp5=Cyc_Absyn_compress(fd->cached_type);struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)==6){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 1999
({struct Cyc_List_List*_Tmp7=Cyc_List_append(fd->i.attributes,i.attributes);fd->i.attributes=_Tmp7;});goto _LL23;}}else{
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("function has non-function type",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));}_LL23:;}{
# 2002
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(fd->i.attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
inlinedoc=Cyc_PP_text(_tag_fat("inline ",sizeof(char),8U));goto _LL28;}else{
inlinedoc=Cyc_PP_text(_tag_fat("__inline ",sizeof(char),10U));goto _LL28;}_LL28:;}else{
# 2010
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL2D;}else{
beforenamedoc=Cyc_Absynpp_callconv2doc(fd->i.attributes);goto _LL2D;}_LL2D:;{
# 2017
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);void*_Tmp6=t;Cyc_Absynpp_tqtd2doc(_Tmp5,_Tmp6,({struct Cyc_Core_Opt*_Tmp7=_cycalloc(sizeof(struct Cyc_Core_Opt));({
struct Cyc_PP_Doc*_Tmp8=({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=beforenamedoc,_Tmp9[1]=namedoc;Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});_Tmp7->v=_Tmp8;});_Tmp7;}));});
# 2025
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp7[2];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_stmt2doc(fd->body,0,0,1);_Tmp7[1]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
s=({struct Cyc_PP_Doc*_Tmp5[4];_Tmp5[0]=inlinedoc,_Tmp5[1]=scopedoc,_Tmp5[2]=tqtddoc,_Tmp5[3]=bodydoc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c){
s=({struct Cyc_PP_Doc*_Tmp5[2];_Tmp5[0]=attsdoc,_Tmp5[1]=s;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL32;}else{
goto _LL32;}_LL32:;
# 2034
goto _LL0;}}}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 2036
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_aggrdecl2doc(ad);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
s=Cyc_Absynpp_vardecl2doc(vd,1);goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_datatypedecl2doc(dd);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("let ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ids2doc(vds);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_enumdecl2doc(ed);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 2042
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("region",sizeof(char),7U));_Tmp5[0]=_Tmp6;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat("<",sizeof(char),2U));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_tvar2doc(tv);_Tmp7[1]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(vd->name);_Tmp5[3]=_Tmp6;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(" = open(",sizeof(char),9U));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_exp2doc(open_exp_opt);_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 2046
_Tmp5[4]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[5]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});
goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 2051
s=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("let ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_pat2doc(p);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 2054
void*t=td->defn!=0?td->defn: Cyc_Absyn_new_evar(td->kind,0);
s=({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("typedef ",sizeof(char),9U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_Absyn_Tqual _Tmp7=td->tq;void*_Tmp8=t;Cyc_Absynpp_tqtd2doc(_Tmp7,_Tmp8,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));({
# 2058
struct Cyc_PP_Doc*_TmpA=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typedef_name2bolddoc(td->name);_TmpB[0]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_tvars2doc(td->tvs);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
# 2058
_Tmp9->v=_TmpA;});_Tmp9;}));});
# 2056
_Tmp5[1]=_Tmp6;}),({
# 2061
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_atts2doc(td->atts);_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
# 2064
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 2066
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("namespace ",sizeof(char),11U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(v);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple1*q=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 2075
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("using ",sizeof(char),7U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});else{
# 2082
s=({struct Cyc_PP_Doc*_Tmp5[11];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* using ",sizeof(char),10U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[8]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[9]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[10]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*tdl=_Tmp4;
# 2089
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("extern \"C\" ",sizeof(char),12U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[5]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});else{
# 2095
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* extern \"C\" ",sizeof(char),15U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[6]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*tdl=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple11*wc=_Tmp1;
# 2102
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs!=0)
exs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" export ",sizeof(char),9U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_export2doc,_tag_fat(",",sizeof(char),2U),exs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 2110
exs_doc=Cyc_Absynpp_rb();}
if(ovrs!=0)
ovrs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" cycdef ",sizeof(char),9U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),ovrs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 2116
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("extern \"C include\" ",sizeof(char),20U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),_Tmp5[5]=exs_doc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 2123
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* extern \"C include\" ",sizeof(char),23U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*(*_Tmp7)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp7;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[6]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 13:
# 2130
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__cyclone_port_on__;",sizeof(char),21U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;case 14:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__cyclone_port_off__;",sizeof(char),22U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;case 15:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__tempest_on__;",sizeof(char),16U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;default:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__tempest_off__;",sizeof(char),17U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}_LL0:;}
# 2135
return s;}
# 2139
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==9){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 2151
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 7:
 goto _LL4;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==20){_LL4:
 return 1;}else{goto _LL7;}case 8: _Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;if((void*)_Tmp0!=0){struct Cyc_Absyn_Typedefdecl*td=_Tmp1;void*x=_Tmp0;
# 2158
return Cyc_Absynpp_is_anon_aggrtype(x);}else{goto _LL7;}default: _LL7:
 return 0;};}
# 2165
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 2168
if(tms!=0 && tms->tl!=0){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=(void*)tms->hd,_Tmp1.f1=(void*)tms->tl->hd;_Tmp1;});if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==3){
# 2171
struct Cyc_List_List*_Tmp1=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)tms->tl->hd,({struct Cyc_List_List*_Tmp4=Cyc_Absynpp_bubble_attributes(r,atts,tms->tl->tl);_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1->tl=_Tmp2;});return _Tmp1;}else{goto _LL3;}}else{_LL3: {
struct Cyc_List_List*_Tmp1=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=atts,_Tmp1->tl=tms;return _Tmp1;}};}{
# 2174
struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=atts,_Tmp0->tl=tms;return _Tmp0;}}
# 2177
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr s=Cyc_strconcat(_tag_fat("`",sizeof(char),2U),*t->name);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='t';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=(struct _fat_ptr)s;_Tmp1;});t->name=_Tmp0;});}}
# 2186
struct _tuple14 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2188
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;int _Tmp8;void*_Tmp9;struct Cyc_Absyn_PtrAtts _TmpA;unsigned _TmpB;void*_TmpC;void*_TmpD;struct Cyc_Absyn_Tqual _TmpE;void*_TmpF;switch(*((int*)t)){case 5: _TmpF=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_TmpD=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*t2=_TmpF;struct Cyc_Absyn_Tqual tq2=_TmpE;struct Cyc_Absyn_Exp*e=_TmpD;void*zeroterm=_TmpC;unsigned ztl=_TmpB;
# 2191
struct _tuple14 _Tmp10=Cyc_Absynpp_to_tms(r,tq2,t2);void*_Tmp11;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;_Tmp13=_Tmp10.f0;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp13;void*t3=_Tmp12;struct Cyc_List_List*tml3=_Tmp11;
void*tm;
if(e==0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp14->tag=0,_Tmp14->f1=zeroterm,_Tmp14->f2=ztl;_Tmp14;});else{
# 2196
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp14->tag=1,_Tmp14->f1=e,_Tmp14->f2=zeroterm,_Tmp14->f3=ztl;_Tmp14;});}{
struct _tuple14 _Tmp14;_Tmp14.f0=tq3,_Tmp14.f1=t3,({struct Cyc_List_List*_Tmp15=({struct Cyc_List_List*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp16->hd=tm,_Tmp16->tl=tml3;_Tmp16;});_Tmp14.f2=_Tmp15;});return _Tmp14;}}}case 4: _TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts;{void*t2=_TmpF;struct Cyc_Absyn_Tqual tq2=_TmpE;struct Cyc_Absyn_PtrAtts ptratts=_TmpA;
# 2200
struct _tuple14 _Tmp10=Cyc_Absynpp_to_tms(r,tq2,t2);void*_Tmp11;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;_Tmp13=_Tmp10.f0;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp13;void*t3=_Tmp12;struct Cyc_List_List*tml3=_Tmp11;
tml3=({struct Cyc_List_List*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp16->tag=2,_Tmp16->f1=ptratts,_Tmp16->f2=tq;_Tmp16;});_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tml3;_Tmp14;});{
struct _tuple14 _Tmp14;_Tmp14.f0=tq3,_Tmp14.f1=t3,_Tmp14.f2=tml3;return _Tmp14;}}}case 6: _TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.tvars;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.effect;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_tqual;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_type;_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.args;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.c_varargs;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.cyc_varargs;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.qual_bnd;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.attributes;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.checks_clause;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.requires_clause;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ensures_clause;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.throws_clause;_Tmp0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.effconstr;{struct Cyc_List_List*typvars=_TmpF;void*effopt=_TmpD;struct Cyc_Absyn_Tqual t2qual=_TmpE;void*t2=_TmpC;struct Cyc_List_List*args=_Tmp9;int c_varargs=_Tmp8;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp7;struct Cyc_List_List*qb=_Tmp6;struct Cyc_List_List*fn_atts=_Tmp5;struct Cyc_Absyn_Exp*chk=_Tmp4;struct Cyc_Absyn_Exp*req=_Tmp3;struct Cyc_Absyn_Exp*ens=_Tmp2;struct Cyc_Absyn_Exp*thrws=_Tmp1;struct Cyc_List_List*effc=_Tmp0;
# 2206
if(!Cyc_Absynpp_print_all_tvars){
# 2210
if(effopt==0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 2214
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2217
({void(*_Tmp10)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter;_Tmp10;})(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 2220
struct _tuple14 _Tmp10=Cyc_Absynpp_to_tms(r,t2qual,t2);void*_Tmp11;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;_Tmp13=_Tmp10.f0;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp13;void*t3=_Tmp12;struct Cyc_List_List*tml3=_Tmp11;
struct Cyc_List_List*tms=tml3;
# 2231 "absynpp.cyc"
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 2235
if(fn_atts!=0)
tms=({struct _RegionHandle*_Tmp14=r;void*_Tmp15=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp16->tag=5,_Tmp16->f1=0U,_Tmp16->f2=fn_atts;_Tmp16;});Cyc_Absynpp_bubble_attributes(_Tmp14,_Tmp15,tms);});
tms=({struct Cyc_List_List*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp16->tag=3,({void*_Tmp17=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp18=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp18->tag=1,_Tmp18->f1=args,_Tmp18->f2=c_varargs,_Tmp18->f3=cyc_varargs,_Tmp18->f4=effopt,_Tmp18->f5=effc,_Tmp18->f6=qb,_Tmp18->f7=chk,_Tmp18->f8=req,_Tmp18->f9=ens,_Tmp18->f10=thrws;_Tmp18;});_Tmp16->f1=_Tmp17;});_Tmp16;});
# 2237
_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tms;_Tmp14;});
# 2241
goto _LL16;}else{
# 2243
tms=({struct Cyc_List_List*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp16->tag=3,({void*_Tmp17=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp18=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp18->tag=1,_Tmp18->f1=args,_Tmp18->f2=c_varargs,_Tmp18->f3=cyc_varargs,_Tmp18->f4=effopt,_Tmp18->f5=effc,_Tmp18->f6=qb,_Tmp18->f7=chk,_Tmp18->f8=req,_Tmp18->f9=ens,_Tmp18->f10=thrws;_Tmp18;});_Tmp16->f1=_Tmp17;});_Tmp16;});
# 2243
_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tms;_Tmp14;});
# 2247
for(1;fn_atts!=0;fn_atts=fn_atts->tl){
void*_Tmp14=(void*)fn_atts->hd;switch(*((int*)_Tmp14)){case 1:
 goto _LL1F;case 2: _LL1F:
 goto _LL21;case 3: _LL21:
# 2252
 tms=({struct Cyc_List_List*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp16=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp17=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp17->tag=5,_Tmp17->f1=0U,({struct Cyc_List_List*_Tmp18=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=(void*)fn_atts->hd,_Tmp19->tl=0;_Tmp19;});_Tmp17->f2=_Tmp18;});_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=tms;_Tmp15;});
goto AfterAtts;default:
 goto _LL1B;}_LL1B:;}
# 2256
goto _LL16;}_LL16:;
# 2259
AfterAtts:
 if(typvars!=0)
tms=({struct Cyc_List_List*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp16->tag=4,_Tmp16->f1=typvars,_Tmp16->f2=0U,_Tmp16->f3=1;_Tmp16;});_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tms;_Tmp14;});{
struct _tuple14 _Tmp14;_Tmp14.f0=tq3,_Tmp14.f1=t3,_Tmp14.f2=tms;return _Tmp14;}}}}case 1: _TmpF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_TmpD=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_Tmp8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;{struct Cyc_Core_Opt*k=_TmpF;void*topt=_TmpD;int i=_Tmp8;
# 2265
if(topt==0){struct _tuple14 _Tmp10;_Tmp10.f0=tq,_Tmp10.f1=t,_Tmp10.f2=0;return _Tmp10;}else{return Cyc_Absynpp_to_tms(r,tq,topt);}}case 8: _TmpF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_TmpD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_TmpC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp9=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct _tuple1*n=_TmpF;struct Cyc_List_List*ts=_TmpD;struct Cyc_Absyn_Typedefdecl*td=_TmpC;void*topt=_Tmp9;
# 2271
if(topt==0 || !Cyc_Absynpp_expand_typedefs){
struct _tuple14 _Tmp10;_Tmp10.f0=tq,_Tmp10.f1=t,_Tmp10.f2=0;return _Tmp10;}
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}default:  {
# 2277
struct _tuple14 _Tmp10;_Tmp10.f0=tq,_Tmp10.f1=t,_Tmp10.f2=0;return _Tmp10;}};}
# 2281
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2283
void*_Tmp0;switch(*((int*)t)){case 1: _Tmp0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;if((void*)_Tmp0!=0){void*def=_Tmp0;
return Cyc_Absynpp_is_char_ptr(def);}else{goto _LL5;}case 4: _Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*elt_typ=_Tmp0;
# 2286
while(1){
{void*_Tmp1;switch(*((int*)elt_typ)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)elt_typ)->f2;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;continue;}else{goto _LLE;}case 8: _Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)elt_typ)->f4;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;continue;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)->f2==Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;};}
# 2287
1U;}}default: _LL5:
# 2293
 return 0;};}
# 2297
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct _tuple14 _Tmp1=Cyc_Absynpp_to_tms(temp,tq,typ);void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;struct Cyc_List_List*tms=_Tmp2;
tms=Cyc_List_imp_rev(tms);
if(tms==0 && dopt==0){
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[2];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _Tmp5;}{
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[4];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp6[2]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=({int _Tmp8=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_Tmp9=dopt==0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_Tmp8,_Tmp9,tms);});_Tmp6[3]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _Tmp5;}}}
# 2299
;_pop_region();}
# 2309
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Exp*req=f->requires_clause;
struct Cyc_Absyn_Exp*wid=f->width;
struct Cyc_PP_Doc*requires_doc=(unsigned)req?({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("@requires ",sizeof(char),11U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}): Cyc_PP_nil_doc();
struct Cyc_PP_Doc*width_doc=(unsigned)wid?({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(wid);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}): Cyc_PP_nil_doc();
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 2316
struct Cyc_PP_Doc*_Tmp0[5];({struct Cyc_PP_Doc*_Tmp1=({struct Cyc_Absyn_Tqual _Tmp2=f->tq;void*_Tmp3=f->type;Cyc_Absynpp_tqtd2doc(_Tmp2,_Tmp3,({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp5=Cyc_PP_textptr(f->name);_Tmp4->v=_Tmp5;});_Tmp4;}));});_Tmp0[0]=_Tmp1;}),_Tmp0[1]=width_doc,({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(f->attributes);_Tmp0[2]=_Tmp1;}),_Tmp0[3]=requires_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[4]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));}else{
# 2319
struct Cyc_PP_Doc*_Tmp0[5];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(f->attributes);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_Absyn_Tqual _Tmp2=f->tq;void*_Tmp3=f->type;Cyc_Absynpp_tqtd2doc(_Tmp2,_Tmp3,({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp5=Cyc_PP_textptr(f->name);_Tmp4->v=_Tmp5;});_Tmp4;}));});_Tmp0[1]=_Tmp1;}),_Tmp0[2]=width_doc,_Tmp0[3]=requires_doc,({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[4]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));};}
# 2324
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp0;})(Cyc_Absynpp_aggrfield2doc,_tag_fat("",sizeof(char),1U),fields);}
# 2328
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(f->sc);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_typedef_name2doc(f->name);_Tmp0[1]=_Tmp1;}),
f->typs==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[2]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_args2doc(f->typs);_Tmp0[2]=_Tmp1;});return Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));}
# 2332
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({struct Cyc_PP_Doc*(*_Tmp0)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*)=(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;_Tmp0;})(Cyc_Absynpp_datatypefield2doc,_tag_fat(",",sizeof(char),2U),fields);}
# 2337
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(1;tdl!=0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_Tmp0=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_Tmp0,72,f);});
Cyc_fprintf(f,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}}
# 2344
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);Cyc_PP_seql(_Tmp0,({struct Cyc_List_List*(*_Tmp1)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2347
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0,0,0),72);}
struct _fat_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_rgnpo2string(struct Cyc_List_List*po){
return Cyc_PP_string_of_doc(Cyc_Absynpp_rgnpo2doc(po),72);}
# 2354
struct _fat_ptr Cyc_Absynpp_effconstr2string(struct Cyc_List_List*effc){
return Cyc_PP_string_of_doc(Cyc_Absynpp_effconstr2doc(effc),72);}
# 2357
struct _fat_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _fat_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;}}
# 2367
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}
