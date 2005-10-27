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
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 938
extern void*Cyc_Absyn_heap_rgn_type;
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_false_type;
# 946
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 967
extern struct _tuple0*Cyc_Absyn_exn_name;
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud (void);
# 995
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 143
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 145
int Cyc_Tcutil_check_eff_in_cap(unsigned,int,void*,struct Cyc_List_List*,void*);
# 148
int Cyc_Tcutil_satisfies_effect_constraints(unsigned,struct Cyc_List_List*,struct Cyc_List_List*);
# 169
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 212
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 214
int Cyc_Tcutil_new_tvar_id (void);
# 216
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 264
struct Cyc_List_List*Cyc_Tcutil_get_aquals_bounds(struct Cyc_Absyn_Fndecl*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 67 "warn.h"
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
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 158
extern void*Cyc_Tcenv_glob_curr_rgn;
# 30 "currgn.h"
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void);
void*Cyc_CurRgn_curr_rgn_type (void);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 95 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_effconstr2string(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 52 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 54
void*Cyc_Tcenv_env_err(struct _fat_ptr msg){
_throw(& Cyc_Tcenv_Env_error_val);}struct _tuple11{struct Cyc_Absyn_Switch_clause*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct{int tag;void*f1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_List_List*aquals_bounds;const struct _tuple11*ctrl_env;void*capability;void*curr_rgn;void*curr_lifo_rgn;struct Cyc_Tcenv_FenvFlags flags;void*lastconstraint;struct Cyc_List_List*effconstr;};
# 133 "tcenv.cyc"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void){
# 135
struct Cyc_Tcenv_Genv*ae;ae=_cycalloc(sizeof(struct Cyc_Tcenv_Genv)),({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);ae->aggrdecls=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);ae->datatypedecls=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);ae->enumdecls=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);ae->typedefs=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);ae->ordinaries=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**)=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;_Tmp2;});struct Cyc_Dict_Dict _Tmp2=ae->datatypedecls;struct _tuple0*_Tmp3=Cyc_Absyn_exn_name;_Tmp1(_Tmp2,_Tmp3,({struct Cyc_Absyn_Datatypedecl**_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));({struct Cyc_Absyn_Datatypedecl*_Tmp5=Cyc_Absyn_exn_tud();*_Tmp4=_Tmp5;});_Tmp4;}));});ae->datatypedecls=_Tmp0;});{
struct Cyc_Tcenv_Tenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_Tmp0->ns=0,_Tmp0->ae=ae,_Tmp0->le=0,_Tmp0->allow_valueof=0,_Tmp0->in_extern_c_include=0,_Tmp0->in_tempest=0,_Tmp0->tempest_generalize=0,_Tmp0->in_extern_c_inc_repeat=0;return _Tmp0;}}struct _tuple12{void*f0;int f1;};
# 143
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q,int is_use){
struct _tuple12*ans=({struct _tuple12*(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(te->ae->ordinaries,q);
if(is_use)
(*ans).f1=1;
return(*ans).f0;}
# 149
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Aggrdecl**(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(te->ae->aggrdecls,q);}
# 152
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl**(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(te->ae->datatypedecls,q);}
# 155
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
struct Cyc_Absyn_Datatypedecl***_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Datatypedecl**));({struct Cyc_Absyn_Datatypedecl**_Tmp1=({struct Cyc_Absyn_Datatypedecl**(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp2;})(te->ae->datatypedecls,q);*_Tmp0=_Tmp1;});return _Tmp0;}
# 158
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Enumdecl**(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(te->ae->enumdecls,q);}
# 161
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _tuple0*q){
return({struct Cyc_Absyn_Typedefdecl*(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(te->ae->typedefs,q);}
# 167
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr err_msg){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)({int(*_Tmp0)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;_Tmp0;})(err_msg);
return le;}
# 172
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le==0)({int(*_Tmp0)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Tcenv_env_err;_Tmp0;})(_tag_fat("put_fenv",sizeof(char),9U));{
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->le=fe;
return ans;}}
# 178
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->le=0;
return ans;}
# 184
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return Cyc_Tcenv_get_fenv(te,_tag_fat("return_typ",sizeof(char),11U))->shared->return_typ;}
# 188
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(te->le==0)return 0;
return le->type_vars;}
# 193
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp1=Cyc_Tcenv_lookup_type_vars(te);_Tmp0->v=_Tmp1;});return _Tmp0;}
# 197
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*fe;fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("add_type_vars",sizeof(char),14U));*fe=_Tmp0;});
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*new_tvs=Cyc_List_append(tvs,fe->type_vars);
Cyc_Tcutil_check_unique_tvars(loc,new_tvs);
fe->type_vars=new_tvs;
return Cyc_Tcenv_put_fenv(te,fe);}}
# 206
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_new=status;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 213
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0U;
return le->flags.in_new;}
# 218
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->flags.abstract_ok;}
# 223
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 230
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 237
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 244
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 251
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->flags.in_notreadctxt;}
# 256
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_lhs=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 263
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_lhs=0;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 270
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->flags.in_lhs;}
# 275
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),*ans=*le;
ans->flags.in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 282
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->flags.in_stmt_exp;}
# 289
const struct _tuple11*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 291
const struct _tuple11*ans=Cyc_Tcenv_get_fenv(te,_tag_fat("process_fallthru",sizeof(char),17U))->ctrl_env;
if(ans!=(const struct _tuple11*)0)
({struct Cyc_Absyn_Switch_clause**_Tmp0=({struct Cyc_Absyn_Switch_clause**_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause*));*_Tmp1=(*ans).f0;_Tmp1;});*clauseopt=_Tmp0;});
return ans;}
# 297
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 299
struct Cyc_List_List*ft_typ=0;
for(1;vds!=0;vds=vds->tl){
ft_typ=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,_Tmp0->tl=ft_typ;_Tmp0;});}{
struct _tuple11*new_ctrl_env;new_ctrl_env=_cycalloc(sizeof(struct _tuple11)),new_ctrl_env->f0=clause,new_ctrl_env->f1=new_tvs,({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(ft_typ);new_ctrl_env->f2=_Tmp0;});{
struct Cyc_Tcenv_Fenv*new_fe;new_fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("set_fallthru",sizeof(char),13U));*new_fe=_Tmp0;});
new_fe->ctrl_env=(const struct _tuple11*)new_ctrl_env;
return Cyc_Tcenv_put_fenv(te,new_fe);}}}
# 308
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe;fe=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("clear_fallthru",sizeof(char),15U));*fe=_Tmp0;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 314
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=te->le==0?0:({struct Cyc_Tcenv_Fenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));*_Tmp0=*te->le;_Tmp0;});
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->allow_valueof=1;
return ans;}
# 320
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=te->le==0?0:({struct Cyc_Tcenv_Fenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));*_Tmp0=*te->le;_Tmp0;});
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->in_extern_c_include=1;
return ans;}
# 326
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=te->le==0?0:({struct Cyc_Tcenv_Fenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));*_Tmp0=*te->le;_Tmp0;});
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->in_tempest=1;
return ans;}
# 332
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=te->le==0?0:({struct Cyc_Tcenv_Fenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));*_Tmp0=*te->le;_Tmp0;});
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
ans->in_tempest=0;
return ans;}
# 338
int Cyc_Tcenv_in_tempest(struct Cyc_Tcenv_Tenv*te){
return te->in_tempest;}
# 341
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened,int lifo){
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("add_region",sizeof(char),11U));*ans=_Tmp0;});
({void*_Tmp0=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=rgn,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=ans->capability,_Tmp3->tl=0;_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;}));
# 343
ans->capability=_Tmp0;});
# 346
if(lifo){
ans->curr_lifo_rgn=rgn;
if(!opened){
# 351
void*dj=0;
{void*_Tmp0=ans->lastconstraint;void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)_Tmp0)->f1;{struct Cyc_List_List*l=_Tmp1;
# 354
dj=(void*)({struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_Tmp2->tag=1,_Tmp2->f1=rgn,({void*_Tmp3=Cyc_Absyn_join_eff(l);_Tmp2->f2=_Tmp3;});_Tmp2;});
({void*_Tmp2=(void*)({struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct));_Tmp3->tag=1,_Tmp3->f1=dj;_Tmp3;});ans->lastconstraint=_Tmp2;});
goto _LL0;}}else{_Tmp1=(void**)&((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)_Tmp0)->f1;{void**c=(void**)_Tmp1;
# 358
{void*_Tmp2=*c;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp2)==1){if(*((int*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp2)->f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp2)->f2)->f1)==9){_Tmp4=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp2)->f1;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp2)->f2)->f2;{void*e=_Tmp4;struct Cyc_List_List*l=_Tmp3;
# 360
dj=(void*)({struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_Tmp5->tag=1,_Tmp5->f1=rgn,({void*_Tmp6=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=e,_Tmp7->tl=l;_Tmp7;}));_Tmp5->f2=_Tmp6;});_Tmp5;});
*c=dj;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 364
 goto _LL5;}_LL5:;}
# 366
goto _LL0;}}_LL0:;}
# 368
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_Tmp3->tag=0,_Tmp3->f1=rgn;_Tmp3;});_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=_check_null(dj),_Tmp3->tl=ans->effconstr;_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;});
# 368
ans->effconstr=_Tmp0;});}}
# 373
return Cyc_Tcenv_put_fenv(te,ans);}
# 376
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("new_named_block",sizeof(char),16U));*ans=_Tmp0;});{
struct Cyc_Absyn_VarType_Absyn_Type_struct*block_typ;block_typ=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct)),block_typ->tag=2,block_typ->f1=block_rgn;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=block_rgn,_Tmp1->tl=ans->type_vars;_Tmp1;});ans->type_vars=_Tmp0;});
Cyc_Tcutil_check_unique_tvars(loc,ans->type_vars);
# 382
({void*_Tmp0=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)block_typ,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=ans->capability,_Tmp3->tl=0;_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;}));
# 382
ans->capability=_Tmp0;});
# 384
ans->curr_rgn=(void*)block_typ;
return Cyc_Tcenv_put_fenv(te,ans);}}
# 388
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_eff_kb={0,& Cyc_Kinds_ek};
# 390
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_eff_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}
# 396
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_effect_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ecs,unsigned loc){
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("new_effect_equalities",sizeof(char),22U));*ans=_Tmp0;});
({struct Cyc_List_List*_Tmp0=Cyc_List_append(ans->effconstr,ecs);ans->effconstr=_Tmp0;});
return Cyc_Tcenv_put_fenv(te,ans);}
# 402
void*Cyc_Tcenv_glob_curr_rgn=0;
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0){Cyc_Tcenv_glob_curr_rgn=Cyc_Absyn_heap_rgn_type;return Cyc_Absyn_heap_rgn_type;}
Cyc_Tcenv_glob_curr_rgn=le->curr_rgn;return le->curr_rgn;}
# 408
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return Cyc_Absyn_heap_rgn_type;
return le->curr_lifo_rgn;}
# 414
struct Cyc_List_List*Cyc_Tcenv_curr_effect_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->effconstr;}
# 419
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*le=te->le;
if(le==0)return 0;
return le->aquals_bounds;}struct _tuple13{void*f0;void*f1;};
# 424
static void Cyc_Tcenv_print_aquals_bound(struct Cyc_List_List*aqb){
struct Cyc_List_List*i=aqb;for(0;i!=0;i=i->tl){
struct _tuple13*_Tmp0=(struct _tuple13*)i->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*tv=_Tmp2;void*b=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_typ2string(tv);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_typ2string(b);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat(" (%s, %s) ",sizeof(char),11U),_tag_fat(_Tmp5,sizeof(void*),2));});}}}
# 430
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_aquals_bound(struct Cyc_Tcenv_Tenv*te,void*aq,void*bnd){
struct Cyc_Tcenv_Fenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv)),({struct Cyc_Tcenv_Fenv _Tmp0=*Cyc_Tcenv_get_fenv(te,_tag_fat("add_aquals_bound",sizeof(char),17U));*ans=_Tmp0;});{
struct Cyc_List_List*aqb=ans->aquals_bounds;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{void*found=Cyc_List_assoc_cmp(Cyc_Tcutil_typecmp,aqb,aq);
struct Cyc_Tcenv_Tenv*_Tmp2=te;_npop_handler(0);return _Tmp2;}
# 434
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 438
goto _LL0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}
# 440
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple13*_Tmp2=({struct _tuple13*_Tmp3=_cycalloc(sizeof(struct _tuple13));_Tmp3->f0=aq,_Tmp3->f1=bnd;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=aqb;_Tmp1;});ans->aquals_bounds=_Tmp0;});
return Cyc_Tcenv_put_fenv(te,ans);}}
# 447
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*rgn){
# 449
if(te->allow_valueof)return;{
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,_tag_fat("check_rgn_accessible",sizeof(char),21U));
if(Cyc_Tcutil_subset_effect(0,rgn,fe->capability)||
 Cyc_Tcutil_subset_effect(1,rgn,fe->capability))
return;
# 457
if(Cyc_Tcutil_check_eff_in_cap(loc,0,rgn,fe->effconstr,fe->capability)||
 Cyc_Tcutil_check_eff_in_cap(loc,1,rgn,fe->effconstr,fe->capability))
return;
# 462
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Expression accesses unavailable region ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)rgn;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});}}struct _tuple14{void*f0;void*f1;struct Cyc_List_List*f2;unsigned f3;};
# 467
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*eff){
# 469
if(te->allow_valueof)return;{
struct Cyc_Tcenv_Fenv*_Tmp0=
Cyc_Tcenv_get_fenv(te,_tag_fat("check_effect_accessible",sizeof(char),24U));
# 470
void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->capability;_Tmp2=_Tmp0->shared;_Tmp1=_Tmp0->effconstr;{void*capability=_Tmp3;struct Cyc_Tcenv_SharedFenv*shared=_Tmp2;struct Cyc_List_List*effc=_Tmp1;
# 472
if(Cyc_Tcutil_subset_effect(0,eff,capability))
return;
if(Cyc_Tcutil_check_eff_in_cap(loc,0,eff,effc,capability))
return;
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple14*_Tmp6=({struct _tuple14*_Tmp7=_cycalloc(sizeof(struct _tuple14));_Tmp7->f0=capability,_Tmp7->f1=eff,_Tmp7->f2=effc,_Tmp7->f3=loc;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=shared->delayed_effect_checks;_Tmp5;});
# 476
shared->delayed_effect_checks=_Tmp4;});}}}
# 479
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*checks=
Cyc_Tcenv_get_fenv(te,_tag_fat("check_delayed_constraints",sizeof(char),26U))->shared->delayed_effect_checks;
for(1;checks!=0;checks=checks->tl){
struct _tuple14*_Tmp0=(struct _tuple14*)checks->hd;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0->f0;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{void*capability=_Tmp4;void*eff=_Tmp3;struct Cyc_List_List*constr=_Tmp2;unsigned loc=_Tmp1;
if(Cyc_Tcutil_subset_effect(1,eff,capability))
continue;
if(Cyc_Tcutil_check_eff_in_cap(loc,1,eff,constr,capability))
continue;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Capability \n",sizeof(char),13U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)capability;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat("\ndoes not cover function's effect\n",sizeof(char),35U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)eff;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});}}}struct _tuple15{struct Cyc_List_List*f0;struct Cyc_List_List*f1;unsigned f2;};
# 493
void Cyc_Tcenv_check_effect_constraints(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_List_List*ec){
if(te->le==0){
if(!Cyc_Tcutil_satisfies_effect_constraints(loc,ec,0))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("the effect constraints are not satisfied here",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 498
return;}{
# 500
struct Cyc_Tcenv_Fenv*_Tmp0=te->le;void*_Tmp1;void*_Tmp2;if(_Tmp0!=0){_Tmp2=_Tmp0->effconstr;_Tmp1=_Tmp0->shared;{struct Cyc_List_List*env_ec=_Tmp2;struct Cyc_Tcenv_SharedFenv*shared=_Tmp1;
if(!Cyc_Tcutil_satisfies_effect_constraints(loc,ec,env_ec))
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple15*_Tmp5=({struct _tuple15*_Tmp6=_cycalloc(sizeof(struct _tuple15));_Tmp6->f0=ec,_Tmp6->f1=env_ec,_Tmp6->f2=loc;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=shared->delayed_constraint_checks;_Tmp4;});
# 502
shared->delayed_constraint_checks=_Tmp3;});}}else{_throw_match();}}}
# 507
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*checks=
Cyc_Tcenv_get_fenv(te,_tag_fat("check_delayed_constraints",sizeof(char),26U))->shared->delayed_constraint_checks;
for(1;checks!=0;checks=checks->tl){
struct _tuple15*_Tmp0=(struct _tuple15*)checks->hd;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f0;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*env_ec=_Tmp2;unsigned loc=_Tmp1;
if(!Cyc_Tcutil_satisfies_effect_constraints(loc,ec,env_ec))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("the effect constraints are not satisfied here",sizeof(char),46U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});}}}
# 517
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
struct Cyc_Tcenv_SharedFenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_SharedFenv));_Tmp0->return_typ=ret,_Tmp0->delayed_effect_checks=0,_Tmp0->delayed_constraint_checks=0;return _Tmp0;}
# 523
void*Cyc_Tcenv_make_init_store_constraint(struct Cyc_List_List*tvs,void*sc){
struct Cyc_List_List*es=0;
for(1;tvs!=0;tvs=tvs->tl){
struct Cyc_Core_Opt _Tmp0=Cyc_Kinds_tvar_kind_opt((struct Cyc_Absyn_Tvar*)tvs->hd);void*_Tmp1;_Tmp1=(struct Cyc_Absyn_Kind*)_Tmp0.v;{struct Cyc_Absyn_Kind*k=_Tmp1;
if(k==& Cyc_Kinds_ek)
es=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs->hd);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=es;_Tmp2;});}}
# 530
{void*_Tmp0;void*_Tmp1;if(sc!=0){if(*((int*)sc)==0){_Tmp1=((struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1;{struct Cyc_List_List*ll=_Tmp1;
# 532
es=Cyc_List_imp_append(es,ll);
goto _LL3;}}else{if(*((int*)((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1)==1){if(*((int*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1)->f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1)->f2)->f1)==9){_Tmp1=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1)->f1;_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)((struct Cyc_Tcenv_LastConstraint_Tcenv_StoreConstraintEnv_struct*)sc)->f1)->f2)->f2;{void*r=_Tmp1;struct Cyc_List_List*l=_Tmp0;
# 535
es=Cyc_List_imp_append(es,l);
es=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=r,_Tmp2->tl=es;_Tmp2;});
goto _LL3;}}else{goto _LL8;}}else{goto _LL8;}}else{goto _LL8;}}}else{_LL8:
# 539
 goto _LL3;}_LL3:;}
# 541
return(void*)({struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct));_Tmp0->tag=0,_Tmp0->f1=es;_Tmp0;});}struct _tuple16{struct Cyc_List_List*f0;void*f1;void*f2;};
# 544
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0;
rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)*(*fd->name).f1;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp4,sizeof(void*),1));});*_Tmp1=_Tmp2;});_Tmp1;});rgn0->name=_Tmp0;}),({
int _Tmp0=Cyc_Tcutil_new_tvar_id();rgn0->identity=_Tmp0;}),rgn0->kind=(void*)& Cyc_Tcenv_eff_kb,rgn0->aquals_bound=0;{
# 549
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Tvar*_Tmp4=Cyc_CurRgn_curr_rgn_tvar();_Tmp3->hd=_Tmp4;}),_Tmp3->tl=fd->i.tvars;_Tmp3;});_Tmp1.f0=_Tmp2;}),({
void*_Tmp2=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=Cyc_CurRgn_curr_rgn_type();_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=_check_null(fd->i.effect),_Tmp5->tl=0;_Tmp5;});_Tmp3->tl=_Tmp4;});_Tmp3;}));
# 550
_Tmp1.f1=_Tmp2;}),_Tmp1.f2=fd->i.ret_type;_Tmp1;});
# 549
void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*tvs1=_Tmp3;void*cap=_Tmp2;void*ret_t=_Tmp1;
# 553
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List)),tvs->hd=rgn0,tvs->tl=tvs1;
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*init_store_constraint=Cyc_Tcenv_make_init_store_constraint(tvs,0);
# 557
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp4->tag=2,_Tmp4->f1=rgn0;_Tmp4;});
{struct Cyc_List_List*vds=(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v;for(0;vds!=0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if(fd->i.cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo _Tmp4=*fd->i.cyc_varargs;int _Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;_Tmp8=_Tmp4.name;_Tmp7=_Tmp4.tq;_Tmp6=_Tmp4.type;_Tmp5=_Tmp4.inject;{struct _fat_ptr*n=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp7;void*t=_Tmp6;int i=_Tmp5;
# 563
struct Cyc_List_List*vds=(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v;for(0;vds!=0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)vds->hd;
if(Cyc_strptrcmp((*vd->name).f1,_check_null(n))==0){
({void*_Tmp9=Cyc_Absyn_fatptr_type(t,param_rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);vd->type=_Tmp9;});
break;}}}}{
# 572
struct Cyc_Tcenv_Fenv*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
({struct Cyc_Tcenv_SharedFenv*_Tmp5=Cyc_Tcenv_new_shared_fenv(ret_t);_Tmp4->shared=_Tmp5;}),_Tmp4->type_vars=tvs,({
# 576
struct Cyc_List_List*_Tmp5=Cyc_Tcutil_get_aquals_bounds(fd);_Tmp4->aquals_bounds=_Tmp5;}),_Tmp4->ctrl_env=0,({
# 578
void*_Tmp5=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=param_rgn,({
struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=cap,_Tmp8->tl=0;_Tmp8;});_Tmp6->tl=_Tmp7;});_Tmp6;}));
# 578
_Tmp4->capability=_Tmp5;}),_Tmp4->curr_rgn=param_rgn,({
# 581
void*_Tmp5=Cyc_CurRgn_curr_rgn_type();_Tmp4->curr_lifo_rgn=_Tmp5;}),
_Tmp4->flags.in_new=0U,_Tmp4->flags.in_notreadctxt=0,_Tmp4->flags.in_lhs=0,_Tmp4->flags.abstract_ok=0,_Tmp4->flags.in_stmt_exp=0,_Tmp4->lastconstraint=init_store_constraint,_Tmp4->effconstr=fd->i.effconstr;return _Tmp4;}}}}}
# 588
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=old_fenv->aquals_bounds;_Tmp3=old_fenv->type_vars;_Tmp2=old_fenv->shared;_Tmp1=old_fenv->lastconstraint;_Tmp0=old_fenv->effconstr;{struct Cyc_List_List*aqb=_Tmp4;struct Cyc_List_List*type_vars=_Tmp3;struct Cyc_Tcenv_SharedFenv*shared=_Tmp2;void*lc=_Tmp1;struct Cyc_List_List*ec_old=_Tmp0;
# 592
struct Cyc_Absyn_Tvar*rgn0;
rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=(struct _fat_ptr)*(*fd->name).f1;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp9,sizeof(void*),1));});*_Tmp6=_Tmp7;});_Tmp6;});rgn0->name=_Tmp5;}),({
int _Tmp5=Cyc_Tcutil_new_tvar_id();rgn0->identity=_Tmp5;}),rgn0->kind=(void*)& Cyc_Tcenv_eff_kb,rgn0->aquals_bound=0;{
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List)),tvs->hd=rgn0,({struct Cyc_List_List*_Tmp5=Cyc_List_append(fd->i.tvars,type_vars);tvs->tl=_Tmp5;});
Cyc_Tcutil_check_unique_tvars(loc,tvs);{
void*initconstr=Cyc_Tcenv_make_init_store_constraint(tvs,lc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp5->tag=2,_Tmp5->f1=rgn0;_Tmp5;});
{struct Cyc_List_List*vds=(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v;for(0;vds!=0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=param_rgn;}}
if(fd->i.cyc_varargs!=0){
struct Cyc_Absyn_VarargInfo _Tmp5=*fd->i.cyc_varargs;int _Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;_Tmp9=_Tmp5.name;_Tmp8=_Tmp5.tq;_Tmp7=_Tmp5.type;_Tmp6=_Tmp5.inject;{struct _fat_ptr*n=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*t=_Tmp7;int i=_Tmp6;
# 604
struct Cyc_List_List*vds=(struct Cyc_List_List*)_check_null(fd->param_vardecls)->v;for(0;vds!=0;vds=vds->tl){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)vds->hd;
if(Cyc_strptrcmp((*vd->name).f1,_check_null(n))==0){
({void*_TmpA=Cyc_Absyn_fatptr_type(t,param_rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);vd->type=_TmpA;});
break;}}}}{
# 612
struct Cyc_Tcenv_Fenv*_Tmp5=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
({struct Cyc_Tcenv_SharedFenv*_Tmp6=Cyc_Tcenv_new_shared_fenv(fd->i.ret_type);_Tmp5->shared=_Tmp6;}),_Tmp5->type_vars=tvs,_Tmp5->aquals_bounds=aqb,_Tmp5->ctrl_env=0,({
# 618
void*_Tmp6=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=param_rgn,({
struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=_check_null(fd->i.effect),_Tmp9->tl=0;_Tmp9;});_Tmp7->tl=_Tmp8;});_Tmp7;}));
# 618
_Tmp5->capability=_Tmp6;}),_Tmp5->curr_rgn=param_rgn,({
# 621
void*_Tmp6=Cyc_CurRgn_curr_rgn_type();_Tmp5->curr_lifo_rgn=_Tmp6;}),
_Tmp5->flags.in_new=0U,_Tmp5->flags.in_notreadctxt=0,_Tmp5->flags.in_lhs=0,_Tmp5->flags.abstract_ok=0,_Tmp5->flags.in_stmt_exp=0,_Tmp5->lastconstraint=initconstr,({
# 624
struct Cyc_List_List*_Tmp6=Cyc_List_append(fd->i.effconstr,ec_old);_Tmp5->effconstr=_Tmp6;});return _Tmp5;}}}}}
# 627
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 629
struct Cyc_Absyn_Tvar*rgn0;rgn0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=_tag_fat("bogus",sizeof(char),6U);_Tmp1;});rgn0->name=_Tmp0;}),({int _Tmp0=Cyc_Tcutil_new_tvar_id();rgn0->identity=_Tmp0;}),rgn0->kind=(void*)& Cyc_Tcenv_eff_kb,rgn0->aquals_bound=0;{
struct Cyc_List_List*tvs;tvs=_cycalloc(sizeof(struct Cyc_List_List)),tvs->hd=rgn0,tvs->tl=0;{
# 632
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp0->tag=2,_Tmp0->f1=rgn0;_Tmp0;});
struct Cyc_Tcenv_Fenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
({struct Cyc_Tcenv_SharedFenv*_Tmp1=Cyc_Tcenv_new_shared_fenv(ret_type);_Tmp0->shared=_Tmp1;}),_Tmp0->type_vars=tvs,_Tmp0->aquals_bounds=0,_Tmp0->ctrl_env=0,({
# 639
void*_Tmp1=Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=param_rgn,_Tmp2->tl=0;_Tmp2;}));_Tmp0->capability=_Tmp1;}),_Tmp0->curr_rgn=param_rgn,({
# 641
void*_Tmp1=Cyc_CurRgn_curr_rgn_type();_Tmp0->curr_lifo_rgn=_Tmp1;}),
_Tmp0->flags.in_new=0U,_Tmp0->flags.in_notreadctxt=0,_Tmp0->flags.in_lhs=0,_Tmp0->flags.abstract_ok=0,_Tmp0->flags.in_stmt_exp=0,({
void*_Tmp1=(void*)({struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Tcenv_InitialConstraint_Tcenv_StoreConstraintEnv_struct));_Tmp2->tag=0,_Tmp2->f1=0;_Tmp2;});_Tmp0->lastconstraint=_Tmp1;}),_Tmp0->effconstr=0;return _Tmp0;}}}
# 647
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*ans;ans=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),*ans=*te;
({struct Cyc_Tcenv_Genv*_Tmp0=({struct Cyc_Tcenv_Genv*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcenv_Genv));*_Tmp1=*te->ae;_Tmp1;});ans->ae=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_copy(te->ae->aggrdecls);ans->ae->aggrdecls=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_copy(te->ae->enumdecls);ans->ae->enumdecls=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_copy(te->ae->datatypedecls);ans->ae->datatypedecls=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_copy(te->ae->typedefs);ans->ae->typedefs=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_copy(te->ae->ordinaries);ans->ae->ordinaries=_Tmp0;});
ans->le=0;
return ans;}
# 659
void Cyc_Tcenv_print_delayed_checks(struct Cyc_List_List*dcc){
struct Cyc_List_List*c=dcc;for(0;c!=0;c=c->tl){
struct _tuple15*_Tmp0=(struct _tuple15*)c->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_List_List*ec1=_Tmp2;struct Cyc_List_List*env=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_effconstr2string(ec1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_effconstr2string(env);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat(" (%s , %s)\n",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),2));});}}}
