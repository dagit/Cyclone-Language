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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 921 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 969
extern void*Cyc_Absyn_heap_rgn_type;
# 971
extern void*Cyc_Absyn_al_qual_type;
# 975
extern void*Cyc_Absyn_false_type;
# 977
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 998
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 1030
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 146
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 148
int Cyc_Tcutil_check_eff_in_cap(unsigned,int,void*,struct Cyc_List_List*,void*);
# 151
int Cyc_Tcutil_satisfies_effect_constraints(unsigned,struct Cyc_List_List*,struct Cyc_List_List*);
# 172
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 215
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 217
int Cyc_Tcutil_new_tvar_id (void);
# 219
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 268
struct Cyc_List_List*Cyc_Tcutil_get_aquals_bounds(struct Cyc_Absyn_Fndecl*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 75 "warn.h"
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 32 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 84 "kinds.h"
struct Cyc_Core_Opt Cyc_Kinds_tvar_kind_opt(struct Cyc_Absyn_Tvar*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 173 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 88 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 157
extern void*Cyc_Tcenv_glob_curr_rgn;
# 30 "currgn.h"
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 97 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_effconstr2string(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 53 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 55
void*Cyc_Tcenv_env_err(struct _fat_ptr msg){struct Cyc_Tcenv_Env_error_exn_struct*_T0;struct Cyc_Tcenv_Env_error_exn_struct*_T1;_T0=& Cyc_Tcenv_Env_error_val;_T1=(struct Cyc_Tcenv_Env_error_exn_struct*)_T0;_throw(_T1);}struct _tuple11{struct Cyc_Absyn_Switch_clause*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct{int tag;void*f1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_List_List*aquals_bounds;const struct _tuple11*ctrl_env;void*capability;void*curr_rgn;void*curr_lifo_rgn;struct Cyc_Tcenv_FenvFlags flags;void*lastconstraint;struct Cyc_List_List*effconstr;};
# 134 "tcenv.cyc"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void){struct Cyc_Tcenv_Genv*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _tuple0*,struct _tuple0*);struct Cyc_Tcenv_Genv*_T4;struct Cyc_Dict_Dict(*_T5)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T6)(int(*)(void*,void*));int(*_T7)(struct _tuple0*,struct _tuple0*);struct Cyc_Tcenv_Genv*_T8;struct Cyc_Dict_Dict(*_T9)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_TA)(int(*)(void*,void*));int(*_TB)(struct _tuple0*,struct _tuple0*);struct Cyc_Tcenv_Genv*_TC;struct Cyc_Dict_Dict(*_TD)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_TE)(int(*)(void*,void*));int(*_TF)(struct _tuple0*,struct _tuple0*);struct Cyc_Tcenv_Genv*_T10;struct Cyc_Dict_Dict(*_T11)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T12)(int(*)(void*,void*));int(*_T13)(struct _tuple0*,struct _tuple0*);struct Cyc_Tcenv_Genv*_T14;struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**);struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Genv*_T17;struct Cyc_Dict_Dict _T18;struct _tuple0*_T19;struct Cyc_Absyn_Datatypedecl**_T1A;struct Cyc_Tcenv_Tenv*_T1B;
# 136
struct Cyc_Tcenv_Genv*ae;ae=_cycalloc(sizeof(struct Cyc_Tcenv_Genv));_T0=ae;_T2=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T1C)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T2;_T1=_T1C;}_T3=Cyc_Absyn_qvar_cmp;_T0->aggrdecls=_T1(_T3);_T4=ae;_T6=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1C)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T6;_T5=_T1C;}_T7=Cyc_Absyn_qvar_cmp;_T4->datatypedecls=_T5(_T7);_T8=ae;_TA=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1C)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TA;_T9=_T1C;}_TB=Cyc_Absyn_qvar_cmp;_T8->enumdecls=_T9(_TB);_TC=ae;_TE=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1C)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TE;_TD=_T1C;}_TF=Cyc_Absyn_qvar_cmp;_TC->typedefs=_TD(_TF);_T10=ae;_T12=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1C)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T12;_T11=_T1C;}_T13=Cyc_Absyn_qvar_cmp;_T10->ordinaries=_T11(_T13);_T14=ae;_T16=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))_T16;_T15=_T1C;}_T17=ae;_T18=_T17->datatypedecls;_T19=Cyc_Absyn_exn_name;{struct Cyc_Absyn_Datatypedecl**_T1C=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_T1C=Cyc_Absyn_exn_tud();_T1A=(struct Cyc_Absyn_Datatypedecl**)_T1C;}_T14->datatypedecls=_T15(_T18,_T19,_T1A);{struct Cyc_Tcenv_Tenv*_T1C=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));
_T1C->ns=0;_T1C->ae=ae;_T1C->le=0;_T1C->allow_valueof=0;_T1C->in_extern_c_include=0;_T1C->in_tempest=0;_T1C->tempest_generalize=0;_T1C->in_extern_c_inc_repeat=0;_T1B=(struct Cyc_Tcenv_Tenv*)_T1C;}return _T1B;}struct _tuple12{void*f0;int f1;};
# 144
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){struct _tuple12*(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;int _T6;struct _tuple12*_T7;struct _tuple12*_T8;struct _tuple12 _T9;void*_TA;_T1=Cyc_Dict_lookup;{
struct _tuple12*(*_TB)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_TB;}_T2=te;_T3=_T2->ae;_T4=_T3->ordinaries;_T5=q;{struct _tuple12*ans=_T0(_T4,_T5);_T6=is_use;
if(!_T6)goto _TL0;_T7=ans;
(*_T7).f1=1;goto _TL1;_TL0: _TL1: _T8=ans;_T9=*_T8;_TA=_T9.f0;
return _TA;}}
# 150
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){struct Cyc_Absyn_Aggrdecl**(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct Cyc_Absyn_Aggrdecl**_T6;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Aggrdecl**(*_T7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T7;}_T2=te;_T3=_T2->ae;_T4=_T3->aggrdecls;_T5=q;_T6=_T0(_T4,_T5);return _T6;}
# 153
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){struct Cyc_Absyn_Datatypedecl**(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct Cyc_Absyn_Datatypedecl**_T6;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Datatypedecl**(*_T7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T7;}_T2=te;_T3=_T2->ae;_T4=_T3->datatypedecls;_T5=q;_T6=_T0(_T4,_T5);return _T6;}
# 156
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){struct Cyc_Absyn_Datatypedecl***_T0;struct _RegionHandle*_T1;struct Cyc_Absyn_Datatypedecl**(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Genv*_T5;struct Cyc_Dict_Dict _T6;struct _tuple0*_T7;_T1=r;{struct Cyc_Absyn_Datatypedecl***_T8=_region_malloc(_T1,0U,sizeof(struct Cyc_Absyn_Datatypedecl**));_T3=Cyc_Dict_lookup;{
struct Cyc_Absyn_Datatypedecl**(*_T9)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T9;}_T4=te;_T5=_T4->ae;_T6=_T5->datatypedecls;_T7=q;*_T8=_T2(_T6,_T7);_T0=(struct Cyc_Absyn_Datatypedecl***)_T8;}return _T0;}
# 159
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){struct Cyc_Absyn_Enumdecl**(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct Cyc_Absyn_Enumdecl**_T6;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Enumdecl**(*_T7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T7;}_T2=te;_T3=_T2->ae;_T4=_T3->enumdecls;_T5=q;_T6=_T0(_T4,_T5);return _T6;}
# 162
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){struct Cyc_Absyn_Typedefdecl*(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct Cyc_Absyn_Typedefdecl*_T6;_T1=Cyc_Dict_lookup;{
struct Cyc_Absyn_Typedefdecl*(*_T7)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T7;}_T2=te;_T3=_T2->ae;_T4=_T3->typedefs;_T5=q;_T6=_T0(_T4,_T5);return _T6;}
# 168
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr err_msg){struct Cyc_Tcenv_Tenv*_T0;int(*_T1)(struct _fat_ptr);struct Cyc_Tcenv_Fenv*_T2;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL2;{int(*_T3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;_T1=_T3;}_T1(err_msg);goto _TL3;_TL2: _TL3: _T2=le;
return _T2;}}
# 173
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;int(*_T2)(struct _fat_ptr);struct _fat_ptr _T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Tenv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Tenv*_T7;_T0=te;_T1=_T0->le;
if(_T1!=0)goto _TL4;{int(*_T8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;_T2=_T8;}_T3=_tag_fat("put_fenv",sizeof(char),9U);_T2(_T3);goto _TL5;_TL4: _TL5: {
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T4=ans;_T5=te;*_T4=*_T5;_T6=ans;
_T6->le=fe;_T7=ans;
return _T7;}}
# 179
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Tenv*_T3;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T0=ans;_T1=te;*_T0=*_T1;_T2=ans;
_T2->le=0;_T3=ans;
return _T3;}
# 185
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct _fat_ptr _T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_SharedFenv*_T3;void*_T4;_T0=te;_T1=
_tag_fat("return_typ",sizeof(char),11U);_T2=Cyc_Tcenv_get_fenv(_T0,_T1);_T3=_T2->shared;_T4=_T3->return_typ;return _T4;}
# 189
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_List_List*_T4;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;_T1=te;_T2=_T1->le;
if(_T2!=0)goto _TL6;return 0;_TL6: _T3=le;_T4=_T3->type_vars;
return _T4;}}
# 194
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_T0;{struct Cyc_Core_Opt*_T1=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T1->v=Cyc_Tcenv_lookup_type_vars(te);_T0=(struct Cyc_Core_Opt*)_T1;}return _T0;}
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_List_List*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_List_List*_T6;struct Cyc_Tcenv_Fenv*_T7;struct Cyc_Tcenv_Tenv*_T8;
struct Cyc_Tcenv_Fenv*fe;fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=fe;_T1=te;_T2=_tag_fat("add_type_vars",sizeof(char),14U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;
Cyc_Tcutil_add_tvar_identities(tvs);_T4=tvs;_T5=fe;_T6=_T5->type_vars;{
struct Cyc_List_List*new_tvs=Cyc_List_append(_T4,_T6);
Cyc_Tcutil_check_unique_tvars(loc,new_tvs);_T7=fe;
_T7->type_vars=new_tvs;_T8=
Cyc_Tcenv_put_fenv(te,fe);return _T8;}}
# 207
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL8;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL8: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_new=status;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 214
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_Tcenv_FenvFlags _T2;enum Cyc_Tcenv_NewStatus _T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TLA;return 0U;_TLA: _T1=le;_T2=_T1->flags;_T3=_T2.in_new;
return _T3;}}
# 219
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_Tcenv_FenvFlags _T2;int _T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TLC;return 0;_TLC: _T1=le;_T2=_T1->flags;_T3=_T2.abstract_ok;
return _T3;}}
# 224
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TLE;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TLE: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.abstract_ok=1;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 231
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL10;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL10: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.abstract_ok=0;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 238
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL12;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL12: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_notreadctxt=1;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 245
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL14;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL14: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_notreadctxt=0;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 252
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_Tcenv_FenvFlags _T2;int _T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL16;return 0;_TL16: _T1=le;_T2=_T1->flags;_T3=_T2.in_notreadctxt;
return _T3;}}
# 257
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL18;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL18: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_lhs=1;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 264
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL1A;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL1A: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_lhs=0;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 271
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_Tcenv_FenvFlags _T2;int _T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL1C;return 0;_TL1C: _T1=le;_T2=_T1->flags;_T3=_T2.in_lhs;
return _T3;}}
# 276
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL1E;_T1=Cyc_Tcenv_put_emptyfenv(te);return _T1;_TL1E: {
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T2=ans;_T3=le;*_T2=*_T3;_T4=ans;
_T4->flags.in_stmt_exp=1;_T5=
Cyc_Tcenv_put_fenv(te,ans);return _T5;}}}
# 283
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_Tcenv_FenvFlags _T2;int _T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL20;return 0;_TL20: _T1=le;_T2=_T1->flags;_T3=_T2.in_stmt_exp;
return _T3;}}
# 290
const struct _tuple11*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Tenv*_T0;struct _fat_ptr _T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Absyn_Switch_clause***_T3;struct Cyc_Absyn_Switch_clause**_T4;const struct _tuple11*_T5;struct _tuple11 _T6;const struct _tuple11*_T7;_T0=te;_T1=
# 292
_tag_fat("process_fallthru",sizeof(char),17U);_T2=Cyc_Tcenv_get_fenv(_T0,_T1);{const struct _tuple11*ans=_T2->ctrl_env;
if(ans==0)goto _TL22;_T3=clauseopt;{struct Cyc_Absyn_Switch_clause**_T8=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause*));_T5=ans;_T6=*_T5;
*_T8=_T6.f0;_T4=(struct Cyc_Absyn_Switch_clause**)_T8;}*_T3=_T4;goto _TL23;_TL22: _TL23: _T7=ans;
return _T7;}}
# 298
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Vardecl*_T3;struct Cyc_List_List*_T4;struct _tuple11*_T5;struct _tuple11*_T6;struct _tuple11*_T7;struct Cyc_Tcenv_Fenv*_T8;struct Cyc_Tcenv_Tenv*_T9;struct _fat_ptr _TA;struct Cyc_Tcenv_Fenv*_TB;struct Cyc_Tcenv_Fenv*_TC;struct Cyc_Tcenv_Tenv*_TD;
# 300
struct Cyc_List_List*ft_typ=0;
_TL27: if(vds!=0)goto _TL25;else{goto _TL26;}
_TL25:{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));_T1=vds;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Vardecl*)_T2;_TE->hd=_T3->type;_TE->tl=ft_typ;_T0=(struct Cyc_List_List*)_TE;}ft_typ=_T0;_T4=vds;
# 301
vds=_T4->tl;goto _TL27;_TL26: {
# 303
struct _tuple11*new_ctrl_env;new_ctrl_env=_cycalloc(sizeof(struct _tuple11));_T5=new_ctrl_env;_T5->f0=clause;_T6=new_ctrl_env;_T6->f1=new_tvs;_T7=new_ctrl_env;_T7->f2=Cyc_List_imp_rev(ft_typ);{
struct Cyc_Tcenv_Fenv*new_fe;new_fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T8=new_fe;_T9=te;_TA=_tag_fat("set_fallthru",sizeof(char),13U);_TB=Cyc_Tcenv_get_fenv(_T9,_TA);*_T8=*_TB;_TC=new_fe;
_TC->ctrl_env=new_ctrl_env;_TD=
Cyc_Tcenv_put_fenv(te,new_fe);return _TD;}}}
# 309
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Tenv*_T5;
struct Cyc_Tcenv_Fenv*fe;fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=fe;_T1=te;_T2=_tag_fat("clear_fallthru",sizeof(char),15U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;_T4=fe;
_T4->ctrl_env=0;_T5=
Cyc_Tcenv_put_fenv(te,fe);return _T5;}
# 315
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Tenv*_T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Tcenv_Tenv*_T9;_T1=te;_T2=_T1->le;
if(_T2!=0)goto _TL28;_T0=0;goto _TL29;_TL28:{struct Cyc_Tcenv_Fenv*_TA=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T4=te;_T5=_T4->le;*_TA=*_T5;_T3=(struct Cyc_Tcenv_Fenv*)_TA;}_T0=_T3;_TL29: {struct Cyc_Tcenv_Fenv*fe=_T0;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T6=ans;_T7=te;*_T6=*_T7;_T8=ans;
_T8->allow_valueof=1;_T9=ans;
return _T9;}}
# 321
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Tenv*_T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Tcenv_Tenv*_T9;_T1=te;_T2=_T1->le;
if(_T2!=0)goto _TL2A;_T0=0;goto _TL2B;_TL2A:{struct Cyc_Tcenv_Fenv*_TA=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T4=te;_T5=_T4->le;*_TA=*_T5;_T3=(struct Cyc_Tcenv_Fenv*)_TA;}_T0=_T3;_TL2B: {struct Cyc_Tcenv_Fenv*fe=_T0;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T6=ans;_T7=te;*_T6=*_T7;_T8=ans;
_T8->in_extern_c_include=1;_T9=ans;
return _T9;}}
# 327
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Tenv*_T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Tcenv_Tenv*_T9;_T1=te;_T2=_T1->le;
if(_T2!=0)goto _TL2C;_T0=0;goto _TL2D;_TL2C:{struct Cyc_Tcenv_Fenv*_TA=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T4=te;_T5=_T4->le;*_TA=*_T5;_T3=(struct Cyc_Tcenv_Fenv*)_TA;}_T0=_T3;_TL2D: {struct Cyc_Tcenv_Fenv*fe=_T0;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T6=ans;_T7=te;*_T6=*_T7;_T8=ans;
_T8->in_tempest=1;_T9=ans;
return _T9;}}
# 333
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Tenv*_T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Tcenv_Tenv*_T9;_T1=te;_T2=_T1->le;
if(_T2!=0)goto _TL2E;_T0=0;goto _TL2F;_TL2E:{struct Cyc_Tcenv_Fenv*_TA=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T4=te;_T5=_T4->le;*_TA=*_T5;_T3=(struct Cyc_Tcenv_Fenv*)_TA;}_T0=_T3;_TL2F: {struct Cyc_Tcenv_Fenv*fe=_T0;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T6=ans;_T7=te;*_T6=*_T7;_T8=ans;
_T8->in_tempest=0;_T9=ans;
return _T9;}}
# 339
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;int _T1;_T0=te;_T1=_T0->in_tempest;
return _T1;}
# 342
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened,int lifo){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_Tcenv_Fenv*_T7;int _T8;struct Cyc_Tcenv_Fenv*_T9;int _TA;struct Cyc_Tcenv_Fenv*_TB;int*_TC;int _TD;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TE;struct Cyc_Tcenv_Fenv*_TF;struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T10;struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T11;void**_T12;void**_T13;int*_T14;int _T15;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T16;void*_T17;int*_T18;int _T19;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T1A;void*_T1B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1C;void*_T1D;int*_T1E;int _T1F;void*_T20;void*_T21;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T22;struct Cyc_List_List*_T23;void**_T24;struct Cyc_Tcenv_Fenv*_T25;struct Cyc_List_List*_T26;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T27;struct Cyc_List_List*_T28;struct Cyc_Tcenv_Fenv*_T29;struct Cyc_Tcenv_Tenv*_T2A;
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=ans;_T1=te;_T2=_tag_fat("add_region",sizeof(char),11U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;_T4=ans;{struct Cyc_List_List*_T2B=_cycalloc(sizeof(struct Cyc_List_List));
_T2B->hd=rgn;{struct Cyc_List_List*_T2C=_cycalloc(sizeof(struct Cyc_List_List));_T7=ans;
_T2C->hd=_T7->capability;_T2C->tl=0;_T6=(struct Cyc_List_List*)_T2C;}_T2B->tl=_T6;_T5=(struct Cyc_List_List*)_T2B;}
# 344
_T4->capability=Cyc_Absyn_join_eff(_T5);_T8=lifo;
# 347
if(!_T8)goto _TL30;_T9=ans;
_T9->curr_lifo_rgn=rgn;_TA=opened;
if(_TA)goto _TL32;else{goto _TL34;}
# 352
_TL34:{void*dj=0;_TB=ans;{
void*_T2B=_TB->lastconstraint;void*_T2C;_TC=(int*)_T2B;_TD=*_TC;if(_TD!=0)goto _TL35;{struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_T2D=(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)_T2B;_T2C=_T2D->f1;}{struct Cyc_List_List*l=_T2C;{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T2D->tag=1;
# 355
_T2D->f1=rgn;_T2D->f2=Cyc_Absyn_join_eff(l);_TE=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T2D;}dj=(void*)_TE;_TF=ans;{struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T2D=_cycalloc(sizeof(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct));_T2D->tag=1;
_T2D->f1=dj;_T10=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T2D;}_TF->lastconstraint=(void*)_T10;goto _LL0;}_TL35:{struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T2D=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T2B;_T11=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T2B;_T12=& _T11->f1;_T2C=(void**)_T12;}{void**c=(void**)_T2C;_T13=c;{
# 359
void*_T2D=*_T13;struct Cyc_List_List*_T2E;void*_T2F;_T14=(int*)_T2D;_T15=*_T14;if(_T15!=1)goto _TL37;_T16=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T2D;_T17=_T16->f2;_T18=(int*)_T17;_T19=*_T18;if(_T19!=0)goto _TL39;_T1A=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T2D;_T1B=_T1A->f2;_T1C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1B;_T1D=_T1C->f1;_T1E=(int*)_T1D;_T1F=*_T1E;if(_T1F!=9)goto _TL3B;{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T30=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T2D;_T20=_T30->f1;_T2F=(void*)_T20;_T21=_T30->f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T31=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T21;_T2E=_T31->f2;}}{void*e=_T2F;struct Cyc_List_List*l=_T2E;{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T30=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T30->tag=1;
# 361
_T30->f1=rgn;{struct Cyc_List_List*_T31=_cycalloc(sizeof(struct Cyc_List_List));_T31->hd=e;_T31->tl=l;_T23=(struct Cyc_List_List*)_T31;}_T30->f2=Cyc_Absyn_join_eff(_T23);_T22=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T30;}dj=(void*)_T22;_T24=c;
*_T24=dj;goto _LL5;}_TL3B: goto _LL8;_TL39: goto _LL8;_TL37: _LL8: goto _LL5;_LL5:;}goto _LL0;}_LL0:;}_T25=ans;{struct Cyc_List_List*_T2B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T2C=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T2C->tag=0;
# 369
_T2C->f1=rgn;_T27=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T2C;}_T2B->hd=(void*)_T27;{struct Cyc_List_List*_T2C=_cycalloc(sizeof(struct Cyc_List_List));
_T2C->hd=_check_null(dj);_T29=ans;
_T2C->tl=_T29->effconstr;_T28=(struct Cyc_List_List*)_T2C;}
# 370
_T2B->tl=_T28;_T26=(struct Cyc_List_List*)_T2B;}
# 369
_T25->effconstr=_T26;}goto _TL33;_TL32: _TL33: goto _TL31;_TL30: _TL31: _T2A=
# 374
Cyc_Tcenv_put_fenv(te,ans);return _T2A;}
# 377
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T5;struct Cyc_Tcenv_Fenv*_T6;struct Cyc_List_List*_T7;struct Cyc_Tcenv_Fenv*_T8;unsigned _T9;struct Cyc_Tcenv_Fenv*_TA;struct Cyc_List_List*_TB;struct Cyc_Tcenv_Fenv*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct Cyc_Tcenv_Fenv*_T10;struct Cyc_Tcenv_Fenv*_T11;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T12;struct Cyc_Tcenv_Tenv*_T13;
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=ans;_T1=te;_T2=_tag_fat("new_named_block",sizeof(char),16U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;{
struct Cyc_Absyn_VarType_Absyn_Type_struct*block_typ;block_typ=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T4=block_typ;_T4->tag=2;_T5=block_typ;_T5->f1=block_rgn;_T6=ans;{struct Cyc_List_List*_T14=_cycalloc(sizeof(struct Cyc_List_List));
_T14->hd=block_rgn;_T8=ans;_T14->tl=_T8->type_vars;_T7=(struct Cyc_List_List*)_T14;}_T6->type_vars=_T7;_T9=loc;_TA=ans;_TB=_TA->type_vars;
Cyc_Tcutil_check_unique_tvars(_T9,_TB);_TC=ans;{struct Cyc_List_List*_T14=_cycalloc(sizeof(struct Cyc_List_List));_TE=block_typ;
# 383
_T14->hd=(void*)_TE;{struct Cyc_List_List*_T15=_cycalloc(sizeof(struct Cyc_List_List));_T10=ans;
_T15->hd=_T10->capability;_T15->tl=0;_TF=(struct Cyc_List_List*)_T15;}_T14->tl=_TF;_TD=(struct Cyc_List_List*)_T14;}
# 383
_TC->capability=Cyc_Absyn_join_eff(_TD);_T11=ans;_T12=block_typ;
# 385
_T11->curr_rgn=(void*)_T12;_T13=
Cyc_Tcenv_put_fenv(te,ans);return _T13;}}
# 389
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_eff_kb={0,(struct Cyc_Absyn_Kind*)& Cyc_Kinds_ek};
# 391
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T0;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1;void*_T2;struct Cyc_Tcenv_Tenv*_T3;_T0=& Cyc_Tcenv_eff_kb;_T1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T0;_T2=(void*)_T1;{
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar(_T2);
Cyc_Tcutil_add_tvar_identity(t);_T3=
Cyc_Tcenv_new_named_block(loc,te,t);return _T3;}}
# 397
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_effect_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ecs,unsigned loc){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct Cyc_Tcenv_Fenv*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Tcenv_Tenv*_T8;
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=ans;_T1=te;_T2=_tag_fat("new_effect_equalities",sizeof(char),22U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;_T4=ans;_T5=ans;_T6=_T5->effconstr;_T7=ecs;
_T4->effconstr=Cyc_List_append(_T6,_T7);_T8=
Cyc_Tcenv_put_fenv(te,ans);return _T8;}
# 403
void*Cyc_Tcenv_glob_curr_rgn=0;
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;void*_T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_Fenv*_T3;void*_T4;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL3D;Cyc_Tcenv_glob_curr_rgn=Cyc_Absyn_heap_rgn_type;_T1=Cyc_Absyn_heap_rgn_type;return _T1;_TL3D: _T2=le;
Cyc_Tcenv_glob_curr_rgn=_T2->curr_rgn;_T3=le;_T4=_T3->curr_rgn;return _T4;}}
# 409
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;void*_T1;struct Cyc_Tcenv_Fenv*_T2;void*_T3;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL3F;_T1=Cyc_Absyn_heap_rgn_type;return _T1;_TL3F: _T2=le;_T3=_T2->curr_lifo_rgn;
return _T3;}}
# 415
struct Cyc_List_List*Cyc_Tcenv_curr_effect_constraints(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_List_List*_T2;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL41;return 0;_TL41: _T1=le;_T2=_T1->effconstr;
return _T2;}}
# 420
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;struct Cyc_List_List*_T2;_T0=te;{
struct Cyc_Tcenv_Fenv*le=_T0->le;
if(le!=0)goto _TL43;return 0;_TL43: _T1=le;_T2=_T1->aquals_bounds;
return _T2;}}struct _tuple13{void*f0;void*f1;};
# 425
static void Cyc_Tcenv_print_aquals_bound(struct Cyc_List_List*aqb){struct Cyc_List_List*_T0;void*_T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;
struct Cyc_List_List*i=aqb;_TL48: if(i!=0)goto _TL46;else{goto _TL47;}
_TL46: _T0=i;_T1=_T0->hd;{struct _tuple13*_T8=(struct _tuple13*)_T1;void*_T9;void*_TA;{struct _tuple13 _TB=*_T8;_TA=_TB.f0;_T9=_TB.f1;}{void*tv=_TA;void*b=_T9;{struct Cyc_String_pa_PrintArg_struct _TB;_TB.tag=0;
_TB.f1=Cyc_Absynpp_typ2string(tv);_T2=_TB;}{struct Cyc_String_pa_PrintArg_struct _TB=_T2;{struct Cyc_String_pa_PrintArg_struct _TC;_TC.tag=0;_TC.f1=Cyc_Absynpp_typ2string(b);_T3=_TC;}{struct Cyc_String_pa_PrintArg_struct _TC=_T3;void*_TD[2];_TD[0]=& _TB;_TD[1]=& _TC;_T4=Cyc_stderr;_T5=_tag_fat(" (%s, %s) ",sizeof(char),11U);_T6=_tag_fat(_TD,sizeof(void*),2);Cyc_fprintf(_T4,_T5,_T6);}}}}_T7=i;
# 426
i=_T7->tl;goto _TL48;_TL47:;}
# 431
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_aquals_bound(struct Cyc_Tcenv_Tenv*te,void*aq,void*bnd){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct _fat_ptr _T2;struct Cyc_Tcenv_Fenv*_T3;struct Cyc_Tcenv_Fenv*_T4;struct _handler_cons*_T5;int _T6;int(*_T7)(void*,void*);struct Cyc_List_List*_T8;void*_T9;void*_TA;struct Cyc_Core_Not_found_exn_struct*_TB;char*_TC;char*_TD;struct Cyc_Tcenv_Fenv*_TE;struct Cyc_List_List*_TF;struct _tuple13*_T10;struct Cyc_Tcenv_Tenv*_T11;
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T0=ans;_T1=te;_T2=_tag_fat("add_aquals_bound",sizeof(char),17U);_T3=Cyc_Tcenv_get_fenv(_T1,_T2);*_T0=*_T3;_T4=ans;{
struct Cyc_List_List*aqb=_T4->aquals_bounds;{struct _handler_cons _T12;_T5=& _T12;_push_handler(_T5);{int _T13=0;_T6=setjmp(_T12.handler);if(!_T6)goto _TL49;_T13=1;goto _TL4A;_TL49: _TL4A: if(_T13)goto _TL4B;else{goto _TL4D;}_TL4D: _T7=Cyc_Tcutil_typecmp;_T8=aqb;_T9=aq;{
# 435
void*found=Cyc_List_assoc_cmp(_T7,_T8,_T9);struct Cyc_Tcenv_Tenv*_T14=te;_npop_handler(0);return _T14;}_pop_handler();goto _TL4C;_TL4B: _TA=Cyc_Core_get_exn_thrown();{void*_T14=(void*)_TA;void*_T15;_TB=(struct Cyc_Core_Not_found_exn_struct*)_T14;_TC=_TB->tag;_TD=Cyc_Core_Not_found;if(_TC!=_TD)goto _TL4E;goto _LL0;_TL4E: _T15=_T14;{void*exn=_T15;_rethrow(exn);}_LL0:;}_TL4C:;}}_TE=ans;{struct Cyc_List_List*_T12=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple13*_T13=_cycalloc(sizeof(struct _tuple13));
# 441
_T13->f0=aq;_T13->f1=bnd;_T10=(struct _tuple13*)_T13;}_T12->hd=_T10;_T12->tl=aqb;_TF=(struct Cyc_List_List*)_T12;}_TE->aquals_bounds=_TF;_T11=
Cyc_Tcenv_put_fenv(te,ans);return _T11;}}
# 448
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){struct Cyc_Tcenv_Tenv*_T0;int _T1;struct Cyc_Tcenv_Tenv*_T2;struct _fat_ptr _T3;void*_T4;struct Cyc_Tcenv_Fenv*_T5;void*_T6;int _T7;void*_T8;struct Cyc_Tcenv_Fenv*_T9;void*_TA;int _TB;unsigned _TC;void*_TD;struct Cyc_Tcenv_Fenv*_TE;struct Cyc_List_List*_TF;struct Cyc_Tcenv_Fenv*_T10;void*_T11;int _T12;unsigned _T13;void*_T14;struct Cyc_Tcenv_Fenv*_T15;struct Cyc_List_List*_T16;struct Cyc_Tcenv_Fenv*_T17;void*_T18;int _T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;struct Cyc_Warn_Typ_Warn_Warg_struct _T1B;unsigned _T1C;struct _fat_ptr _T1D;_T0=te;_T1=_T0->allow_valueof;
# 450
if(!_T1)goto _TL50;return;_TL50: _T2=te;_T3=
_tag_fat("check_rgn_accessible",sizeof(char),21U);{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(_T2,_T3);_T4=rgn;_T5=fe;_T6=_T5->capability;_T7=
Cyc_Tcutil_subset_effect(0,_T4,_T6);if(_T7)goto _TL54;else{goto _TL55;}_TL55: _T8=rgn;_T9=fe;_TA=_T9->capability;_TB=
Cyc_Tcutil_subset_effect(1,_T8,_TA);
# 452
if(_TB)goto _TL54;else{goto _TL52;}
# 454
_TL54: return;_TL52: _TC=loc;_TD=rgn;_TE=fe;_TF=_TE->effconstr;_T10=fe;_T11=_T10->capability;_T12=
# 458
Cyc_Tcutil_check_eff_in_cap(_TC,0,_TD,_TF,_T11);if(_T12)goto _TL58;else{goto _TL59;}_TL59: _T13=loc;_T14=rgn;_T15=fe;_T16=_T15->effconstr;_T17=fe;_T18=_T17->capability;_T19=
Cyc_Tcutil_check_eff_in_cap(_T13,1,_T14,_T16,_T18);
# 458
if(_T19)goto _TL58;else{goto _TL56;}
# 460
_TL58: return;_TL56:{struct Cyc_Warn_String_Warn_Warg_struct _T1E;_T1E.tag=0;
# 463
_T1E.f1=_tag_fat("Expression accesses unavailable region ",sizeof(char),40U);_T1A=_T1E;}{struct Cyc_Warn_String_Warn_Warg_struct _T1E=_T1A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F;_T1F.tag=2;_T1F.f1=rgn;_T1B=_T1F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F=_T1B;void*_T20[2];_T20[0]=& _T1E;_T20[1]=& _T1F;_T1C=loc;_T1D=_tag_fat(_T20,sizeof(void*),2);Cyc_Warn_err2(_T1C,_T1D);}}}}struct _tuple14{void*f0;void*f1;struct Cyc_List_List*f2;unsigned f3;};
# 468
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){struct Cyc_Tcenv_Tenv*_T0;int _T1;struct Cyc_Tcenv_Tenv*_T2;struct _fat_ptr _T3;int _T4;int _T5;struct Cyc_Tcenv_SharedFenv*_T6;struct Cyc_List_List*_T7;struct _tuple14*_T8;struct Cyc_Tcenv_SharedFenv*_T9;_T0=te;_T1=_T0->allow_valueof;
# 470
if(!_T1)goto _TL5A;return;_TL5A: _T2=te;_T3=
# 472
_tag_fat("check_effect_accessible",sizeof(char),24U);{
# 471
struct Cyc_Tcenv_Fenv*_TA=
Cyc_Tcenv_get_fenv(_T2,_T3);struct Cyc_List_List*_TB;struct Cyc_Tcenv_SharedFenv*_TC;void*_TD;{struct Cyc_Tcenv_Fenv _TE=*_TA;_TD=_TE.capability;_TC=_TE.shared;_TB=_TE.effconstr;}{void*capability=_TD;struct Cyc_Tcenv_SharedFenv*shared=_TC;struct Cyc_List_List*effc=_TB;_T4=
Cyc_Tcutil_subset_effect(0,eff,capability);if(!_T4)goto _TL5C;
return;_TL5C: _T5=
Cyc_Tcutil_check_eff_in_cap(loc,0,eff,effc,capability);if(!_T5)goto _TL5E;
return;_TL5E: _T6=shared;{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple14*_TF=_cycalloc(sizeof(struct _tuple14));
# 478
_TF->f0=capability;_TF->f1=eff;_TF->f2=effc;_TF->f3=loc;_T8=(struct _tuple14*)_TF;}_TE->hd=_T8;_T9=shared;_TE->tl=_T9->delayed_effect_checks;_T7=(struct Cyc_List_List*)_TE;}
# 477
_T6->delayed_effect_checks=_T7;}}}
# 480
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct _fat_ptr _T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_SharedFenv*_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;int _T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;struct Cyc_Warn_Typ_Warn_Warg_struct _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;struct Cyc_Warn_Typ_Warn_Warg_struct _TB;unsigned _TC;struct _fat_ptr _TD;struct Cyc_List_List*_TE;_T0=te;_T1=
# 482
_tag_fat("check_delayed_constraints",sizeof(char),26U);_T2=Cyc_Tcenv_get_fenv(_T0,_T1);_T3=_T2->shared;{
# 481
struct Cyc_List_List*checks=_T3->delayed_effect_checks;
# 483
_TL63: if(checks!=0)goto _TL61;else{goto _TL62;}
_TL61: _T4=checks;_T5=_T4->hd;{struct _tuple14*_TF=(struct _tuple14*)_T5;unsigned _T10;struct Cyc_List_List*_T11;void*_T12;void*_T13;{struct _tuple14 _T14=*_TF;_T13=_T14.f0;_T12=_T14.f1;_T11=_T14.f2;_T10=_T14.f3;}{void*capability=_T13;void*eff=_T12;struct Cyc_List_List*constr=_T11;unsigned loc=_T10;_T6=
Cyc_Tcutil_subset_effect(1,eff,capability);if(!_T6)goto _TL64;goto _TL60;_TL64: _T7=
# 487
Cyc_Tcutil_check_eff_in_cap(loc,1,eff,constr,capability);if(!_T7)goto _TL66;goto _TL60;_TL66:{struct Cyc_Warn_String_Warn_Warg_struct _T14;_T14.tag=0;
# 489
_T14.f1=_tag_fat("Capability \n",sizeof(char),13U);_T8=_T14;}{struct Cyc_Warn_String_Warn_Warg_struct _T14=_T8;{struct Cyc_Warn_Typ_Warn_Warg_struct _T15;_T15.tag=2;_T15.f1=capability;_T9=_T15;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T15=_T9;{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;
_T16.f1=_tag_fat("\ndoes not cover function's effect\n",sizeof(char),35U);_TA=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_TA;{struct Cyc_Warn_Typ_Warn_Warg_struct _T17;_T17.tag=2;_T17.f1=eff;_TB=_T17;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T17=_TB;void*_T18[4];_T18[0]=& _T14;_T18[1]=& _T15;_T18[2]=& _T16;_T18[3]=& _T17;_TC=loc;_TD=_tag_fat(_T18,sizeof(void*),4);Cyc_Warn_err2(_TC,_TD);}}}}}}_TL60: _TE=checks;
# 483
checks=_TE->tl;goto _TL63;_TL62:;}}struct _tuple15{struct Cyc_List_List*f0;struct Cyc_List_List*f1;unsigned f2;};
# 494
void Cyc_Tcenv_check_effect_constraints(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ec){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Fenv*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Fenv*_T7;int _T8;struct Cyc_Tcenv_SharedFenv*_T9;struct Cyc_List_List*_TA;struct _tuple15*_TB;struct Cyc_Tcenv_SharedFenv*_TC;_T0=te;_T1=_T0->le;
if(_T1!=0)goto _TL68;_T2=
Cyc_Tcutil_satisfies_effect_constraints(loc,ec,0);if(_T2)goto _TL6A;else{goto _TL6C;}
_TL6C:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat("the effect constraints are not satisfied here",sizeof(char),46U);_T3=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T3;void*_TE[1];_TE[0]=& _TD;_T4=loc;_T5=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_err2(_T4,_T5);}goto _TL6B;_TL6A: _TL6B:
# 499
 return;_TL68: _T6=te;_T7=_T6->le;{
# 501
struct Cyc_Tcenv_Fenv _TD=*_T7;struct Cyc_Tcenv_SharedFenv*_TE;struct Cyc_List_List*_TF;_TF=_TD.effconstr;_TE=_TD.shared;{struct Cyc_List_List*env_ec=_TF;struct Cyc_Tcenv_SharedFenv*shared=_TE;_T8=
Cyc_Tcutil_satisfies_effect_constraints(loc,ec,env_ec);if(_T8)goto _TL6D;else{goto _TL6F;}
_TL6F: _T9=shared;{struct Cyc_List_List*_T10=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple15*_T11=_cycalloc(sizeof(struct _tuple15));
_T11->f0=ec;_T11->f1=env_ec;_T11->f2=loc;_TB=(struct _tuple15*)_T11;}_T10->hd=_TB;_TC=shared;_T10->tl=_TC->delayed_constraint_checks;_TA=(struct Cyc_List_List*)_T10;}
# 503
_T9->delayed_constraint_checks=_TA;goto _TL6E;_TL6D: _TL6E:;}}}
# 508
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct _fat_ptr _T1;struct Cyc_Tcenv_Fenv*_T2;struct Cyc_Tcenv_SharedFenv*_T3;struct Cyc_List_List*_T4;void*_T5;int _T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;unsigned _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;_T0=te;_T1=
# 510
_tag_fat("check_delayed_constraints",sizeof(char),26U);_T2=Cyc_Tcenv_get_fenv(_T0,_T1);_T3=_T2->shared;{
# 509
struct Cyc_List_List*checks=_T3->delayed_constraint_checks;
# 511
_TL73: if(checks!=0)goto _TL71;else{goto _TL72;}
_TL71: _T4=checks;_T5=_T4->hd;{struct _tuple15*_TB=(struct _tuple15*)_T5;unsigned _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;{struct _tuple15 _TF=*_TB;_TE=_TF.f0;_TD=_TF.f1;_TC=_TF.f2;}{struct Cyc_List_List*ec=_TE;struct Cyc_List_List*env_ec=_TD;unsigned loc=_TC;_T6=
Cyc_Tcutil_satisfies_effect_constraints(loc,ec,env_ec);if(_T6)goto _TL74;else{goto _TL76;}
_TL76:{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;_TF.f1=_tag_fat("the effect constraints are not satisfied here",sizeof(char),46U);_T7=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_T7;void*_T10[1];_T10[0]=& _TF;_T8=loc;_T9=_tag_fat(_T10,sizeof(void*),1);Cyc_Warn_err2(_T8,_T9);}goto _TL75;_TL74: _TL75:;}}_TA=checks;
# 511
checks=_TA->tl;goto _TL73;_TL72:;}}
# 518
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){struct Cyc_Tcenv_SharedFenv*_T0;{struct Cyc_Tcenv_SharedFenv*_T1=_cycalloc(sizeof(struct Cyc_Tcenv_SharedFenv));
_T1->return_typ=ret;
_T1->delayed_effect_checks=0;
_T1->delayed_constraint_checks=0;_T0=(struct Cyc_Tcenv_SharedFenv*)_T1;}
# 519
return _T0;}
# 524
void*Cyc_Tcenv_make_init_store_constraint(struct Cyc_List_List*tvs,void*sc){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;void*_T3;struct Cyc_Absyn_Kind*_T4;struct Cyc_Absyn_Kind*_T5;struct Cyc_Absyn_Kind*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_Absyn_Tvar*_TA;struct Cyc_List_List*_TB;void*_TC;int*_TD;int _TE;void*_TF;void*_T10;struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T11;void*_T12;int*_T13;int _T14;void*_T15;struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T16;void*_T17;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T1D;void*_T1E;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T1F;void*_T20;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T21;void*_T22;int*_T23;int _T24;void*_T25;void*_T26;void*_T27;void*_T28;struct Cyc_List_List*_T29;struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_T2A;void*_T2B;
struct Cyc_List_List*es=0;
_TL7A: if(tvs!=0)goto _TL78;else{goto _TL79;}
_TL78: _T0=tvs;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Tvar*)_T1;{struct Cyc_Core_Opt _T2C=Cyc_Kinds_tvar_kind_opt(_T2);struct Cyc_Absyn_Kind*_T2D;_T3=_T2C.v;_T2D=(struct Cyc_Absyn_Kind*)_T3;{struct Cyc_Absyn_Kind*k=_T2D;_T4=k;_T5=& Cyc_Kinds_ek;_T6=(struct Cyc_Absyn_Kind*)_T5;
if(_T4!=_T6)goto _TL7B;{struct Cyc_List_List*_T2E=_cycalloc(sizeof(struct Cyc_List_List));_T8=tvs;_T9=_T8->hd;_TA=(struct Cyc_Absyn_Tvar*)_T9;
_T2E->hd=Cyc_Absyn_var_type(_TA);_T2E->tl=es;_T7=(struct Cyc_List_List*)_T2E;}es=_T7;goto _TL7C;_TL7B: _TL7C:;}}_TB=tvs;
# 526
tvs=_TB->tl;goto _TL7A;_TL79:{struct Cyc_List_List*_T2C;void*_T2D;if(sc==0)goto _TL7D;_TC=sc;_TD=(int*)_TC;_TE=*_TD;if(_TE!=0)goto _TL7F;_TF=sc;{struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_T2E=(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)_TF;_T2D=_T2E->f1;}{struct Cyc_List_List*ll=_T2D;
# 533
es=Cyc_List_imp_append(es,ll);goto _LL3;}_TL7F: _T10=sc;_T11=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T10;_T12=_T11->f1;_T13=(int*)_T12;_T14=*_T13;if(_T14!=1)goto _TL81;_T15=sc;_T16=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T15;_T17=_T16->f1;_T18=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T17;_T19=_T18->f2;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=0)goto _TL83;_T1C=sc;_T1D=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T1C;_T1E=_T1D->f1;_T1F=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T1E;_T20=_T1F->f2;_T21=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T20;_T22=_T21->f1;_T23=(int*)_T22;_T24=*_T23;if(_T24!=9)goto _TL85;_T25=sc;{struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_T2E=(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_T25;_T26=_T2E->f1;{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T2F=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T26;_T27=_T2F->f1;_T2D=(void*)_T27;_T28=_T2F->f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T30=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_T2C=_T30->f2;}}}{void*r=_T2D;struct Cyc_List_List*l=_T2C;
# 536
es=Cyc_List_imp_append(es,l);{struct Cyc_List_List*_T2E=_cycalloc(sizeof(struct Cyc_List_List));
_T2E->hd=r;_T2E->tl=es;_T29=(struct Cyc_List_List*)_T2E;}es=_T29;goto _LL3;}_TL85: goto _LL8;_TL83: goto _LL8;_TL81: goto _LL8;_TL7D: _LL8: goto _LL3;_LL3:;}{struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_T2C=_cycalloc(sizeof(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct));_T2C->tag=0;
# 542
_T2C->f1=es;_T2A=(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)_T2C;}_T2B=(void*)_T2A;return _T2B;}struct _tuple16{struct Cyc_List_List*f0;void*f1;void*f2;};
# 545
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Absyn_Tvar*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc_Absyn_Fndecl*_T4;struct _tuple0*_T5;struct _tuple0 _T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_Absyn_Tvar*_TA;struct Cyc_Absyn_Tvar*_TB;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TC;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TD;struct Cyc_Absyn_Tvar*_TE;struct _tuple16 _TF;struct Cyc_List_List*_T10;struct Cyc_Absyn_Fndecl*_T11;struct Cyc_Absyn_FnInfo _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_Fndecl*_T15;struct Cyc_Absyn_FnInfo _T16;void*_T17;struct Cyc_Absyn_Fndecl*_T18;struct Cyc_Absyn_FnInfo _T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T1C;struct Cyc_Absyn_Fndecl*_T1D;struct Cyc_Core_Opt*_T1E;struct Cyc_Core_Opt*_T1F;void*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_List_List*_T24;struct Cyc_Absyn_Fndecl*_T25;struct Cyc_Absyn_FnInfo _T26;struct Cyc_Absyn_VarargInfo*_T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Absyn_FnInfo _T29;struct Cyc_Absyn_VarargInfo*_T2A;struct Cyc_Absyn_Fndecl*_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Core_Opt*_T2D;void*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct Cyc_Absyn_Vardecl*_T31;struct _tuple0*_T32;struct _tuple0 _T33;struct _fat_ptr*_T34;struct _fat_ptr*_T35;int _T36;struct Cyc_Absyn_Vardecl*_T37;struct Cyc_List_List*_T38;struct Cyc_Tcenv_Fenv*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Fndecl*_T3C;struct Cyc_Absyn_FnInfo _T3D;
struct Cyc_Absyn_Tvar*rgn0;rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T0=rgn0;{struct _fat_ptr*_T3E=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T3F;_T3F.tag=0;_T4=fd;_T5=_T4->name;_T6=*_T5;_T7=_T6.f1;
_T3F.f1=*_T7;_T3=_T3F;}{struct Cyc_String_pa_PrintArg_struct _T3F=_T3;void*_T40[1];_T40[0]=& _T3F;_T8=_tag_fat("`%s",sizeof(char),4U);_T9=_tag_fat(_T40,sizeof(void*),1);_T2=Cyc_aprintf(_T8,_T9);}*_T3E=_T2;_T1=(struct _fat_ptr*)_T3E;}_T0->name=_T1;_TA=rgn0;
_TA->identity=Cyc_Tcutil_new_tvar_id();_TB=rgn0;_TC=& Cyc_Tcenv_eff_kb;_TD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_TC;_TB->kind=(void*)_TD;_TE=rgn0;_TE->aquals_bound=0;{struct _tuple16 _T3E;{struct Cyc_List_List*_T3F=_cycalloc(sizeof(struct Cyc_List_List));
# 550
_T3F->hd=Cyc_CurRgn_curr_rgn_tvar();_T11=fd;_T12=_T11->i;_T3F->tl=_T12.tvars;_T10=(struct Cyc_List_List*)_T3F;}_T3E.f0=_T10;{struct Cyc_List_List*_T3F=_cycalloc(sizeof(struct Cyc_List_List));
_T3F->hd=Cyc_CurRgn_curr_rgn_type();{struct Cyc_List_List*_T40=_cycalloc(sizeof(struct Cyc_List_List));_T15=fd;_T16=_T15->i;_T17=_T16.effect;
_T40->hd=_check_null(_T17);_T40->tl=0;_T14=(struct Cyc_List_List*)_T40;}_T3F->tl=_T14;_T13=(struct Cyc_List_List*)_T3F;}
# 551
_T3E.f1=Cyc_Absyn_join_eff(_T13);_T18=fd;_T19=_T18->i;
# 553
_T3E.f2=_T19.ret_type;_TF=_T3E;}{
# 550
struct _tuple16 _T3E=_TF;void*_T3F;void*_T40;struct Cyc_List_List*_T41;_T41=_T3E.f0;_T40=_T3E.f1;_T3F=_T3E.f2;{struct Cyc_List_List*tvs1=_T41;void*cap=_T40;void*ret_t=_T3F;
# 554
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List));_T1A=tvs;_T1A->hd=rgn0;_T1B=tvs;_T1B->tl=tvs1;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*init_store_constraint=Cyc_Tcenv_make_init_store_constraint(tvs,0);{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T42=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T42->tag=2;
# 558
_T42->f1=rgn0;_T1C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T42;}{void*param_rgn=(void*)_T1C;_T1D=fd;_T1E=_T1D->param_vardecls;_T1F=
_check_null(_T1E);_T20=_T1F->v;{struct Cyc_List_List*vds=(struct Cyc_List_List*)_T20;_TL8A: if(vds!=0)goto _TL88;else{goto _TL89;}
_TL88: _T21=vds;_T22=_T21->hd;_T23=(struct Cyc_Absyn_Vardecl*)_T22;_T23->rgn=param_rgn;_T24=vds;
# 559
vds=_T24->tl;goto _TL8A;_TL89:;}_T25=fd;_T26=_T25->i;_T27=_T26.cyc_varargs;
# 561
if(_T27==0)goto _TL8B;_T28=fd;_T29=_T28->i;_T2A=_T29.cyc_varargs;{
struct Cyc_Absyn_VarargInfo _T42=*_T2A;int _T43;void*_T44;struct Cyc_Absyn_Tqual _T45;struct _fat_ptr*_T46;_T46=_T42.name;_T45=_T42.tq;_T44=_T42.type;_T43=_T42.inject;{struct _fat_ptr*n=_T46;struct Cyc_Absyn_Tqual tq=_T45;void*t=_T44;int i=_T43;_T2B=fd;_T2C=_T2B->param_vardecls;_T2D=
# 564
_check_null(_T2C);_T2E=_T2D->v;{struct Cyc_List_List*vds=(struct Cyc_List_List*)_T2E;_TL90: if(vds!=0)goto _TL8E;else{goto _TL8F;}
_TL8E: _T2F=vds;_T30=_T2F->hd;{struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T30;_T31=vd;_T32=_T31->name;_T33=*_T32;_T34=_T33.f1;_T35=
_check_null(n);_T36=Cyc_strptrcmp(_T34,_T35);if(_T36!=0)goto _TL91;_T37=vd;
_T37->type=Cyc_Absyn_fatptr_type(t,param_rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);goto _TL8F;_TL91:;}_T38=vds;
# 564
vds=_T38->tl;goto _TL90;_TL8F:;}}}goto _TL8C;_TL8B: _TL8C:{struct Cyc_Tcenv_Fenv*_T42=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
# 574
_T42->shared=Cyc_Tcenv_new_shared_fenv(ret_t);
_T42->type_vars=tvs;
# 577
_T42->aquals_bounds=Cyc_Tcutil_get_aquals_bounds(fd);
_T42->ctrl_env=0;{struct Cyc_List_List*_T43=_cycalloc(sizeof(struct Cyc_List_List));
_T43->hd=param_rgn;{struct Cyc_List_List*_T44=_cycalloc(sizeof(struct Cyc_List_List));
_T44->hd=cap;_T44->tl=0;_T3B=(struct Cyc_List_List*)_T44;}_T43->tl=_T3B;_T3A=(struct Cyc_List_List*)_T43;}
# 579
_T42->capability=Cyc_Absyn_join_eff(_T3A);
# 581
_T42->curr_rgn=param_rgn;
_T42->curr_lifo_rgn=Cyc_CurRgn_curr_rgn_type();
_T42->flags.in_new=0U;_T42->flags.in_notreadctxt=0;_T42->flags.in_lhs=0;_T42->flags.abstract_ok=0;_T42->flags.in_stmt_exp=0;
_T42->lastconstraint=init_store_constraint;_T3C=fd;_T3D=_T3C->i;
_T42->effconstr=_T3D.effconstr;_T39=(struct Cyc_Tcenv_Fenv*)_T42;}
# 573
return _T39;}}}}}
# 589
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv*_T0;struct Cyc_Absyn_Tvar*_T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_Absyn_Fndecl*_T5;struct _tuple0*_T6;struct _tuple0 _T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc_Absyn_Tvar*_TB;struct Cyc_Absyn_Tvar*_TC;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TD;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TE;struct Cyc_Absyn_Tvar*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_Absyn_Fndecl*_T12;struct Cyc_Absyn_FnInfo _T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T16;struct Cyc_Absyn_Fndecl*_T17;struct Cyc_Core_Opt*_T18;struct Cyc_Core_Opt*_T19;void*_T1A;struct Cyc_List_List*_T1B;void*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_List_List*_T1E;struct Cyc_Absyn_Fndecl*_T1F;struct Cyc_Absyn_FnInfo _T20;struct Cyc_Absyn_VarargInfo*_T21;struct Cyc_Absyn_Fndecl*_T22;struct Cyc_Absyn_FnInfo _T23;struct Cyc_Absyn_VarargInfo*_T24;struct Cyc_Absyn_Fndecl*_T25;struct Cyc_Core_Opt*_T26;struct Cyc_Core_Opt*_T27;void*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Vardecl*_T2B;struct _tuple0*_T2C;struct _tuple0 _T2D;struct _fat_ptr*_T2E;struct _fat_ptr*_T2F;int _T30;struct Cyc_Absyn_Vardecl*_T31;struct Cyc_List_List*_T32;struct Cyc_Tcenv_Fenv*_T33;struct Cyc_Absyn_Fndecl*_T34;struct Cyc_Absyn_FnInfo _T35;void*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;struct Cyc_Absyn_Fndecl*_T39;struct Cyc_Absyn_FnInfo _T3A;void*_T3B;struct Cyc_Absyn_Fndecl*_T3C;struct Cyc_Absyn_FnInfo _T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_Tcenv_SharedFenv*_T42;struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;_T0=old_fenv;{struct Cyc_Tcenv_Fenv _T45=*_T0;_T44=_T45.aquals_bounds;_T43=_T45.type_vars;_T42=_T45.shared;_T41=_T45.lastconstraint;_T40=_T45.effconstr;}{struct Cyc_List_List*aqb=_T44;struct Cyc_List_List*type_vars=_T43;struct Cyc_Tcenv_SharedFenv*shared=_T42;void*lc=_T41;struct Cyc_List_List*ec_old=_T40;
# 593
struct Cyc_Absyn_Tvar*rgn0;rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1=rgn0;{struct _fat_ptr*_T45=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;_T5=fd;_T6=_T5->name;_T7=*_T6;_T8=_T7.f1;
_T46.f1=*_T8;_T4=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T4;void*_T47[1];_T47[0]=& _T46;_T9=_tag_fat("`%s",sizeof(char),4U);_TA=_tag_fat(_T47,sizeof(void*),1);_T3=Cyc_aprintf(_T9,_TA);}*_T45=_T3;_T2=(struct _fat_ptr*)_T45;}_T1->name=_T2;_TB=rgn0;
_TB->identity=Cyc_Tcutil_new_tvar_id();_TC=rgn0;_TD=& Cyc_Tcenv_eff_kb;_TE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_TD;_TC->kind=(void*)_TE;_TF=rgn0;_TF->aquals_bound=0;{
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List));_T10=tvs;_T10->hd=rgn0;_T11=tvs;_T12=fd;_T13=_T12->i;_T14=_T13.tvars;_T15=type_vars;_T11->tl=Cyc_List_append(_T14,_T15);
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*initconstr=Cyc_Tcenv_make_init_store_constraint(tvs,lc);{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T45=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T45->tag=2;
_T45->f1=rgn0;_T16=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T45;}{void*param_rgn=(void*)_T16;_T17=fd;_T18=_T17->param_vardecls;_T19=
_check_null(_T18);_T1A=_T19->v;{struct Cyc_List_List*vds=(struct Cyc_List_List*)_T1A;_TL96: if(vds!=0)goto _TL94;else{goto _TL95;}
_TL94: _T1B=vds;_T1C=_T1B->hd;_T1D=(struct Cyc_Absyn_Vardecl*)_T1C;_T1D->rgn=param_rgn;_T1E=vds;
# 600
vds=_T1E->tl;goto _TL96;_TL95:;}_T1F=fd;_T20=_T1F->i;_T21=_T20.cyc_varargs;
# 602
if(_T21==0)goto _TL97;_T22=fd;_T23=_T22->i;_T24=_T23.cyc_varargs;{
struct Cyc_Absyn_VarargInfo _T45=*_T24;int _T46;void*_T47;struct Cyc_Absyn_Tqual _T48;struct _fat_ptr*_T49;_T49=_T45.name;_T48=_T45.tq;_T47=_T45.type;_T46=_T45.inject;{struct _fat_ptr*n=_T49;struct Cyc_Absyn_Tqual tq=_T48;void*t=_T47;int i=_T46;_T25=fd;_T26=_T25->param_vardecls;_T27=
# 605
_check_null(_T26);_T28=_T27->v;{struct Cyc_List_List*vds=(struct Cyc_List_List*)_T28;_TL9C: if(vds!=0)goto _TL9A;else{goto _TL9B;}
_TL9A: _T29=vds;_T2A=_T29->hd;{struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T2A;_T2B=vd;_T2C=_T2B->name;_T2D=*_T2C;_T2E=_T2D.f1;_T2F=
_check_null(n);_T30=Cyc_strptrcmp(_T2E,_T2F);if(_T30!=0)goto _TL9D;_T31=vd;
_T31->type=Cyc_Absyn_fatptr_type(t,param_rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);goto _TL9B;_TL9D:;}_T32=vds;
# 605
vds=_T32->tl;goto _TL9C;_TL9B:;}}}goto _TL98;_TL97: _TL98:{struct Cyc_Tcenv_Fenv*_T45=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_T34=fd;_T35=_T34->i;_T36=_T35.ret_type;
# 614
_T45->shared=Cyc_Tcenv_new_shared_fenv(_T36);
_T45->type_vars=tvs;
# 617
_T45->aquals_bounds=aqb;
_T45->ctrl_env=0;{struct Cyc_List_List*_T46=_cycalloc(sizeof(struct Cyc_List_List));
_T46->hd=param_rgn;{struct Cyc_List_List*_T47=_cycalloc(sizeof(struct Cyc_List_List));_T39=fd;_T3A=_T39->i;_T3B=_T3A.effect;
_T47->hd=_check_null(_T3B);_T47->tl=0;_T38=(struct Cyc_List_List*)_T47;}_T46->tl=_T38;_T37=(struct Cyc_List_List*)_T46;}
# 619
_T45->capability=Cyc_Absyn_join_eff(_T37);
# 621
_T45->curr_rgn=param_rgn;
_T45->curr_lifo_rgn=Cyc_CurRgn_curr_rgn_type();
_T45->flags.in_new=0U;_T45->flags.in_notreadctxt=0;_T45->flags.in_lhs=0;_T45->flags.abstract_ok=0;_T45->flags.in_stmt_exp=0;
_T45->lastconstraint=initconstr;_T3C=fd;_T3D=_T3C->i;_T3E=_T3D.effconstr;_T3F=ec_old;
_T45->effconstr=Cyc_List_append(_T3E,_T3F);_T33=(struct Cyc_Tcenv_Fenv*)_T45;}
# 613
return _T33;}}}}}
# 628
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){struct Cyc_Absyn_Tvar*_T0;struct _fat_ptr*_T1;struct Cyc_Absyn_Tvar*_T2;struct Cyc_Absyn_Tvar*_T3;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T4;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T5;struct Cyc_Absyn_Tvar*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T9;struct Cyc_Tcenv_Fenv*_TA;struct Cyc_List_List*_TB;struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_TC;
# 630
struct Cyc_Absyn_Tvar*rgn0;rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T0=rgn0;{struct _fat_ptr*_TD=_cycalloc(sizeof(struct _fat_ptr));*_TD=_tag_fat("bogus",sizeof(char),6U);_T1=(struct _fat_ptr*)_TD;}_T0->name=_T1;_T2=rgn0;_T2->identity=Cyc_Tcutil_new_tvar_id();_T3=rgn0;_T4=& Cyc_Tcenv_eff_kb;_T5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T4;_T3->kind=(void*)_T5;_T6=rgn0;_T6->aquals_bound=0;{
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List));_T7=tvs;_T7->hd=rgn0;_T8=tvs;_T8->tl=0;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TD=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TD->tag=2;
# 633
_TD->f1=rgn0;_T9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TD;}{void*param_rgn=(void*)_T9;{struct Cyc_Tcenv_Fenv*_TD=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
# 635
_TD->shared=Cyc_Tcenv_new_shared_fenv(ret_type);
_TD->type_vars=tvs;
# 638
_TD->aquals_bounds=0;
_TD->ctrl_env=0;{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));
_TE->hd=param_rgn;_TE->tl=0;_TB=(struct Cyc_List_List*)_TE;}_TD->capability=Cyc_Absyn_join_eff(_TB);
_TD->curr_rgn=param_rgn;
_TD->curr_lifo_rgn=Cyc_CurRgn_curr_rgn_type();
_TD->flags.in_new=0U;_TD->flags.in_notreadctxt=0;_TD->flags.in_lhs=0;_TD->flags.abstract_ok=0;_TD->flags.in_stmt_exp=0;{struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_TE=_cycalloc(sizeof(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct));_TE->tag=0;
_TE->f1=0;_TC=(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)_TE;}_TD->lastconstraint=(void*)_TC;
_TD->effconstr=0;_TA=(struct Cyc_Tcenv_Fenv*)_TD;}
# 634
return _TA;}}}
# 648
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Tenv*_T2;struct Cyc_Tcenv_Genv*_T3;struct Cyc_Tcenv_Tenv*_T4;struct Cyc_Tcenv_Genv*_T5;struct Cyc_Tcenv_Tenv*_T6;struct Cyc_Tcenv_Genv*_T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Tcenv_Genv*_T9;struct Cyc_Dict_Dict _TA;struct Cyc_Tcenv_Tenv*_TB;struct Cyc_Tcenv_Genv*_TC;struct Cyc_Tcenv_Tenv*_TD;struct Cyc_Tcenv_Genv*_TE;struct Cyc_Dict_Dict _TF;struct Cyc_Tcenv_Tenv*_T10;struct Cyc_Tcenv_Genv*_T11;struct Cyc_Tcenv_Tenv*_T12;struct Cyc_Tcenv_Genv*_T13;struct Cyc_Dict_Dict _T14;struct Cyc_Tcenv_Tenv*_T15;struct Cyc_Tcenv_Genv*_T16;struct Cyc_Tcenv_Tenv*_T17;struct Cyc_Tcenv_Genv*_T18;struct Cyc_Dict_Dict _T19;struct Cyc_Tcenv_Tenv*_T1A;struct Cyc_Tcenv_Genv*_T1B;struct Cyc_Tcenv_Tenv*_T1C;struct Cyc_Tcenv_Genv*_T1D;struct Cyc_Dict_Dict _T1E;struct Cyc_Tcenv_Tenv*_T1F;struct Cyc_Tcenv_Tenv*_T20;
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T0=ans;_T1=te;*_T0=*_T1;_T2=ans;{struct Cyc_Tcenv_Genv*_T21=_cycalloc(sizeof(struct Cyc_Tcenv_Genv));_T4=te;_T5=_T4->ae;
*_T21=*_T5;_T3=(struct Cyc_Tcenv_Genv*)_T21;}_T2->ae=_T3;_T6=ans;_T7=_T6->ae;_T8=te;_T9=_T8->ae;_TA=_T9->aggrdecls;
_T7->aggrdecls=Cyc_Dict_copy(_TA);_TB=ans;_TC=_TB->ae;_TD=te;_TE=_TD->ae;_TF=_TE->enumdecls;
_TC->enumdecls=Cyc_Dict_copy(_TF);_T10=ans;_T11=_T10->ae;_T12=te;_T13=_T12->ae;_T14=_T13->datatypedecls;
_T11->datatypedecls=Cyc_Dict_copy(_T14);_T15=ans;_T16=_T15->ae;_T17=te;_T18=_T17->ae;_T19=_T18->typedefs;
_T16->typedefs=Cyc_Dict_copy(_T19);_T1A=ans;_T1B=_T1A->ae;_T1C=te;_T1D=_T1C->ae;_T1E=_T1D->ordinaries;
_T1B->ordinaries=Cyc_Dict_copy(_T1E);_T1F=ans;
_T1F->le=0;_T20=ans;
return _T20;}
# 660
void Cyc_Tcenv_print_delayed_checks(struct Cyc_List_List*dcc){struct Cyc_List_List*_T0;void*_T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;
struct Cyc_List_List*c=dcc;_TLA2: if(c!=0)goto _TLA0;else{goto _TLA1;}
_TLA0: _T0=c;_T1=_T0->hd;{struct _tuple15*_T8=(struct _tuple15*)_T1;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;{struct _tuple15 _TB=*_T8;_TA=_TB.f0;_T9=_TB.f1;}{struct Cyc_List_List*ec1=_TA;struct Cyc_List_List*env=_T9;{struct Cyc_String_pa_PrintArg_struct _TB;_TB.tag=0;
# 664
_TB.f1=Cyc_Absynpp_effconstr2string(ec1);_T2=_TB;}{struct Cyc_String_pa_PrintArg_struct _TB=_T2;{struct Cyc_String_pa_PrintArg_struct _TC;_TC.tag=0;
_TC.f1=Cyc_Absynpp_effconstr2string(env);_T3=_TC;}{struct Cyc_String_pa_PrintArg_struct _TC=_T3;void*_TD[2];_TD[0]=& _TB;_TD[1]=& _TC;_T4=Cyc_stderr;_T5=
# 663
_tag_fat(" (%s , %s)\n",sizeof(char),12U);_T6=_tag_fat(_TD,sizeof(void*),2);Cyc_fprintf(_T4,_T5,_T6);}}}}_T7=c;
# 661
c=_T7->tl;goto _TLA2;_TLA1:;}
