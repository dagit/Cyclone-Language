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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 933
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 935
int Cyc_Absyn_equal_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
int Cyc_Unify_unify_kindbound(void*,void*);
# 33
int Cyc_Unify_unify_and_constrain(unsigned,void*,void*);
# 131 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 138
struct Cyc_List_List*Cyc_Tcutil_rsubst_effconstr(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 274
int Cyc_Tcutil_cmp_effect_constraints(struct Cyc_List_List*,struct Cyc_List_List*);
# 90 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_force_kb(void*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 76
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);char Cyc_Tcdecl_Incompatible[13U]="Incompatible";
# 38 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
void Cyc_Tcdecl_merr(unsigned loc,struct _fat_ptr*msg1,struct _fat_ptr fmt,struct _fat_ptr ap){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned long _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;unsigned _T9;void**_TA;unsigned _TB;struct _fat_ptr _TC;unsigned _TD;void**_TE;unsigned _TF;struct Cyc_String_pa_PrintArg_struct*_T10;struct _fat_ptr*_T11;struct _fat_ptr _T12;void**_T13;unsigned _T14;struct _fat_ptr _T15;unsigned char*_T16;void**_T17;unsigned _T18;int _T19;
# 42
struct _fat_ptr fmt2;_T0=
_check_null(msg1);_T1=*_T0;_T2=Cyc_strlen(_T1);if(_T2!=0U)goto _TL0;_T3=
_tag_fat("%s",sizeof(char),3U);_T4=fmt;fmt2=Cyc_strconcat(_T3,_T4);goto _TL1;
# 46
_TL0: _T5=_tag_fat("%s ",sizeof(char),4U);_T6=fmt;fmt2=Cyc_strconcat(_T5,_T6);_TL1: _T8=ap;_T9=
_get_fat_size(_T8,sizeof(void*));{unsigned _T1A=_T9 + 1U;_TB=_check_times(_T1A,sizeof(void*));{void**_T1B=_cycalloc(_TB);_TC=ap;_TD=_get_fat_size(_TC,sizeof(void*));{unsigned _T1C=_TD + 1U;unsigned i;i=0;_TL5: if(i < _T1C)goto _TL3;else{goto _TL4;}_TL3:
 if(i!=0U)goto _TL6;_TF=i;_TE=_T1B + _TF;{struct Cyc_String_pa_PrintArg_struct*_T1D=_cycalloc(sizeof(struct Cyc_String_pa_PrintArg_struct));_T1D->tag=0;_T11=msg1;_T12=*_T11;_T1D->f1=_T12;_T10=(struct Cyc_String_pa_PrintArg_struct*)_T1D;}*_TE=(void*)_T10;goto _TL7;_TL6: _T14=i;_T13=_T1B + _T14;_T15=ap;_T16=_T15.curr;_T17=(void**)_T16;_T18=i - 1U;_T19=(int)_T18;*_T13=_T17[_T19];_TL7: i=i + 1;goto _TL5;_TL4:;}_TA=(void**)_T1B;}_T7=
# 47
_tag_fat(_TA,sizeof(void*),_T1A);}{struct _fat_ptr ap2=_T7;
# 49
Cyc_Warn_verr(loc,fmt2,ap2);}}
# 54
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){struct _fat_ptr _T0;enum Cyc_Absyn_Scope _T1;int _T2;struct _fat_ptr _T3;enum Cyc_Absyn_Scope _T4;int _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct Cyc_String_pa_PrintArg_struct _T9;void**_TA;void**_TB;void**_TC;void**_TD;unsigned _TE;struct _fat_ptr*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;_T1=s0;_T2=(int)_T1;
# 56
if(_T2!=2)goto _TL8;_T0=_tag_fat("public ",sizeof(char),8U);goto _TL9;_TL8: _T0=Cyc_Absynpp_scope2string(s0);_TL9: {struct _fat_ptr s0_str=_T0;_T4=s1;_T5=(int)_T4;
if(_T5!=2)goto _TLA;_T3=_tag_fat("public ",sizeof(char),8U);goto _TLB;_TLA: _T3=Cyc_Absynpp_scope2string(s1);_TLB: {struct _fat_ptr s1_str=_T3;{struct Cyc_String_pa_PrintArg_struct _T12;_T12.tag=0;
# 59
_T12.f1=t;_T6=_T12;}{struct Cyc_String_pa_PrintArg_struct _T12=_T6;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;_T13.f1=v;_T7=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_T7;{struct Cyc_String_pa_PrintArg_struct _T14;_T14.tag=0;_T14.f1=s1_str;_T8=_T14;}{struct Cyc_String_pa_PrintArg_struct _T14=_T8;{struct Cyc_String_pa_PrintArg_struct _T15;_T15.tag=0;_T15.f1=s0_str;_T9=_T15;}{struct Cyc_String_pa_PrintArg_struct _T15=_T9;void*_T16[4];_TA=_T16 + 0;*_TA=& _T12;_TB=_T16 + 1;*_TB=& _T13;_TC=_T16 + 2;*_TC=& _T14;_TD=_T16 + 3;*_TD=& _T15;_TE=loc;_TF=msg;_T10=
# 58
_tag_fat("%s %s is %sbut expected scope is %s",sizeof(char),36U);_T11=_tag_fat(_T16,sizeof(void*),4);Cyc_Tcdecl_merr(_TE,_TF,_T10,_T11);}}}}}}}struct _tuple11{enum Cyc_Absyn_Scope f0;int f1;};struct _tuple12{enum Cyc_Absyn_Scope f0;enum Cyc_Absyn_Scope f1;};
# 62
struct _tuple11 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg,int externCmerge){struct _tuple12 _T0;enum Cyc_Absyn_Scope _T1;int _T2;enum Cyc_Absyn_Scope _T3;int _T4;enum Cyc_Absyn_Scope _T5;int _T6;enum Cyc_Absyn_Scope _T7;int _T8;int _T9;enum Cyc_Absyn_Scope _TA;int _TB;struct _tuple11 _TC;enum Cyc_Absyn_Scope _TD;int _TE;enum Cyc_Absyn_Scope _TF;enum Cyc_Absyn_Scope _T10;enum Cyc_Absyn_Scope _T11;struct _tuple11 _T12;struct _tuple11 _T13;{struct _tuple12 _T14;
# 64
_T14.f0=s0;_T14.f1=s1;_T0=_T14;}{struct _tuple12 _T14=_T0;_T1=_T14.f0;_T2=(int)_T1;switch(_T2){case Cyc_Absyn_ExternC: _T3=_T14.f1;_T4=(int)_T3;switch(_T4){case Cyc_Absyn_ExternC: goto _LL0;case Cyc_Absyn_Public: goto _LL6;case Cyc_Absyn_Extern: _LL6: goto _LL8;default: _LLC: goto _LLE;};case Cyc_Absyn_Public: _T5=_T14.f1;_T6=(int)_T5;switch(_T6){case Cyc_Absyn_ExternC: _LL8: goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL16: goto _LL18;default: goto _LL1B;};case Cyc_Absyn_Extern: _T7=_T14.f1;_T8=(int)_T7;switch(_T8){case Cyc_Absyn_ExternC: _LLA: _T9=externCmerge;
# 71
if(!_T9)goto _TL10;goto _LL0;_TL10: goto _LLC;case Cyc_Absyn_Extern: goto _LLF;default: goto _LL0;};default: _TA=_T14.f1;_TB=(int)_TA;switch(_TB){case Cyc_Absyn_ExternC: _LLE:
# 73
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple11 _T15;_T15.f0=s1;_T15.f1=0;_TC=_T15;}return _TC;case Cyc_Absyn_Extern: _LLF:
# 75
 s1=s0;goto _LL0;default: _TD=_T14.f0;_TE=(int)_TD;switch(_TE){case Cyc_Absyn_Static: _TF=_T14.f1;if(_TF!=Cyc_Absyn_Static)goto _TL14;goto _LL16;_TL14: goto _LL1B;case Cyc_Absyn_Abstract: _T10=_T14.f1;if(_T10!=Cyc_Absyn_Abstract)goto _TL16;_LL18: goto _LL1A;_TL16: goto _LL1B;case Cyc_Absyn_Register: _T11=_T14.f1;if(_T11!=Cyc_Absyn_Register)goto _TL18;_LL1A: goto _LL0;_TL18: goto _LL1B;default: _LL1B:
# 82
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple11 _T15;_T15.f0=s1;_T15.f1=0;_T12=_T15;}return _T12;};};}_LL0:;}{struct _tuple11 _T14;
# 84
_T14.f0=s1;_T14.f1=1;_T13=_T14;}return _T13;}
# 87
static int Cyc_Tcdecl_check_type(unsigned loc,void*t0,void*t1){int _T0;_T0=
# 89
Cyc_Unify_unify_and_constrain(loc,t0,t1);return _T0;}
# 93
static unsigned Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_Int_c _T5;unsigned _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_Int_c _T8;struct _tuple5 _T9;int _TA;unsigned _TB;struct Cyc_Core_Invalid_argument_exn_struct*_TC;void*_TD;_T0=e;{
void*_TE=_T0->r;int _TF;_T1=(int*)_TE;_T2=*_T1;if(_T2!=0)goto _TL1A;_T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TE;_T4=_T3->f1;_T5=_T4.Int_c;_T6=_T5.tag;if(_T6!=5)goto _TL1C;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T10=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TE;_T7=_T10->f1;_T8=_T7.Int_c;_T9=_T8.val;_TF=_T9.f1;}{int x=_TF;_TA=x;_TB=(unsigned)_TA;
return _TB;}_TL1C: goto _LL3;_TL1A: _LL3:{struct Cyc_Core_Invalid_argument_exn_struct*_T10=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T10->tag=Cyc_Core_Invalid_argument;
_T10->f1=_tag_fat("Tcdecl::get_uint_const_value",sizeof(char),29U);_TC=(struct Cyc_Core_Invalid_argument_exn_struct*)_T10;}_TD=(void*)_TC;_throw(_TD);;}}struct _tuple13{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 100
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*_T0;struct _tuple13*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;
# 102
struct Cyc_List_List*inst=0;
_TL21: if(tvs0!=0)goto _TL1F;else{goto _TL20;}
_TL1F:{struct Cyc_List_List*_TA=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple13*_TB=_cycalloc(sizeof(struct _tuple13));_T2=_check_null(tvs1);_T3=_T2->hd;_TB->f0=(struct Cyc_Absyn_Tvar*)_T3;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TC=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TC->tag=2;_T5=tvs0;_T6=_T5->hd;_TC->f1=(struct Cyc_Absyn_Tvar*)_T6;_T4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TC;}_TB->f1=(void*)_T4;_T1=(struct _tuple13*)_TB;}_TA->hd=_T1;_TA->tl=inst;_T0=(struct Cyc_List_List*)_TA;}inst=_T0;_T7=tvs0;
# 103
tvs0=_T7->tl;_T8=tvs1;tvs1=_T8->tl;goto _TL21;_TL20: _T9=inst;
# 105
return _T9;}
# 108
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){int _T0;int _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_String_pa_PrintArg_struct _T3;void**_T4;void**_T5;unsigned _T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Absyn_Tvar*_TC;void*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Absyn_Tvar*_T10;void*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Absyn_Tvar*_T14;void*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Tvar*_T18;void*_T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_Absyn_Tvar*_T20;struct _fat_ptr*_T21;struct Cyc_String_pa_PrintArg_struct _T22;void**_T23;void**_T24;void**_T25;void**_T26;void**_T27;unsigned _T28;struct _fat_ptr*_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;_T0=
# 111
Cyc_List_length(tvs0);_T1=Cyc_List_length(tvs1);if(_T0==_T1)goto _TL22;{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
_T2E.f1=t;_T2=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T2;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=v;_T3=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T3;void*_T30[2];_T4=_T30 + 0;*_T4=& _T2E;_T5=_T30 + 1;*_T5=& _T2F;_T6=loc;_T7=msg;_T8=_tag_fat("%s %s has a different number of type parameters",sizeof(char),48U);_T9=_tag_fat(_T30,sizeof(void*),2);Cyc_Tcdecl_merr(_T6,_T7,_T8,_T9);}}
return 0;_TL22: {
# 115
struct Cyc_List_List*x0=tvs0;
struct Cyc_List_List*x1=tvs1;
_TL27: if(x0!=0)goto _TL25;else{goto _TL26;}
_TL25: _TA=x0;_TB=_TA->hd;_TC=(struct Cyc_Absyn_Tvar*)_TB;_TD=_TC->kind;_TE=_check_null(x1);_TF=_TE->hd;_T10=(struct Cyc_Absyn_Tvar*)_TF;_T11=_T10->kind;Cyc_Unify_unify_kindbound(_TD,_T11);_T12=x0;_T13=_T12->hd;_T14=(struct Cyc_Absyn_Tvar*)_T13;_T15=_T14->kind;{
struct Cyc_Absyn_Kind*c0=Cyc_Kinds_force_kb(_T15);_T16=x1;_T17=_T16->hd;_T18=(struct Cyc_Absyn_Tvar*)_T17;_T19=_T18->kind;{
struct Cyc_Absyn_Kind*c1=Cyc_Kinds_force_kb(_T19);
if(c0!=c1)goto _TL28;goto _TL24;_TL28:{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
# 123
_T2E.f1=t;_T1A=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T1A;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=v;_T1B=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T1B;{struct Cyc_String_pa_PrintArg_struct _T30;_T30.tag=0;_T30.f1=Cyc_Absynpp_kind2string(c0);_T1C=_T30;}{struct Cyc_String_pa_PrintArg_struct _T30=_T1C;{struct Cyc_String_pa_PrintArg_struct _T31;_T31.tag=0;_T1E=x0;_T1F=_T1E->hd;_T20=(struct Cyc_Absyn_Tvar*)_T1F;_T21=_T20->name;_T31.f1=*_T21;_T1D=_T31;}{struct Cyc_String_pa_PrintArg_struct _T31=_T1D;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;_T32.f1=Cyc_Absynpp_kind2string(c1);_T22=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T22;void*_T33[5];_T23=_T33 + 0;*_T23=& _T2E;_T24=_T33 + 1;*_T24=& _T2F;_T25=_T33 + 2;*_T25=& _T30;_T26=_T33 + 3;*_T26=& _T31;_T27=_T33 + 4;*_T27=& _T32;_T28=loc;_T29=msg;_T2A=
# 122
_tag_fat("%s %s has a different kind (%s) for type parameter %s (%s)",sizeof(char),59U);_T2B=_tag_fat(_T33,sizeof(void*),5);Cyc_Tcdecl_merr(_T28,_T29,_T2A,_T2B);}}}}}
# 124
return 0;}}_TL24: _T2C=x0;
# 117
x0=_T2C->tl;_T2D=x1;x1=_T2D->tl;goto _TL27;_TL26:
# 126
 return 1;}}
# 129
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){int _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_String_pa_PrintArg_struct _T2;void**_T3;void**_T4;unsigned _T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;_T0=
# 131
Cyc_Atts_same_atts(atts0,atts1);if(!_T0)goto _TL2A;
return 1;_TL2A:{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;
_T9.f1=t;_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_TA.f1=v;_T2=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T2;void*_TB[2];_T3=_TB + 0;*_T3=& _T9;_T4=_TB + 1;*_T4=& _TA;_T5=loc;_T6=msg;_T7=_tag_fat("%s %s has different attributes",sizeof(char),31U);_T8=_tag_fat(_TB,sizeof(void*),2);Cyc_Tcdecl_merr(_T5,_T6,_T7,_T8);}}
return 0;}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f0;struct Cyc_Absyn_AggrdeclImpl*f1;};
# 138
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Absyn_Aggrdecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Aggrdecl*_T2;enum Cyc_Absyn_AggrKind _T3;int _T4;struct Cyc_Absyn_Aggrdecl*_T5;enum Cyc_Absyn_AggrKind _T6;int _T7;struct Cyc_Absyn_Aggrdecl*_T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_Aggrdecl*_TA;struct Cyc_List_List*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;unsigned _TE;struct _fat_ptr*_TF;int _T10;struct Cyc_Absyn_Aggrdecl*_T11;enum Cyc_Absyn_Scope _T12;struct Cyc_Absyn_Aggrdecl*_T13;enum Cyc_Absyn_Scope _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;unsigned _T17;struct _fat_ptr*_T18;int _T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_Aggrdecl*_T1C;struct Cyc_List_List*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;unsigned _T20;struct _fat_ptr*_T21;int _T22;struct _tuple14 _T23;struct Cyc_Absyn_Aggrdecl*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct Cyc_Absyn_AggrdeclImpl*_T26;struct Cyc_Absyn_AggrdeclImpl*_T27;struct Cyc_Absyn_AggrdeclImpl*_T28;struct Cyc_Absyn_AggrdeclImpl*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;unsigned _T2E;struct _fat_ptr*_T2F;int _T30;struct Cyc_Absyn_Aggrdecl*_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Aggrdecl*_T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;int _T39;unsigned _T3A;struct _fat_ptr*_T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;struct Cyc_List_List*_T3E;void*_T3F;struct Cyc_List_List*_T40;void*_T41;int _T42;struct Cyc_String_pa_PrintArg_struct _T43;struct Cyc_String_pa_PrintArg_struct _T44;struct Cyc_String_pa_PrintArg_struct _T45;struct _fat_ptr*_T46;struct Cyc_String_pa_PrintArg_struct _T47;struct _fat_ptr*_T48;void**_T49;void**_T4A;void**_T4B;void**_T4C;unsigned _T4D;struct _fat_ptr*_T4E;struct _fat_ptr _T4F;struct _fat_ptr _T50;int _T51;struct Cyc_String_pa_PrintArg_struct _T52;struct Cyc_String_pa_PrintArg_struct _T53;struct Cyc_String_pa_PrintArg_struct _T54;struct _fat_ptr*_T55;void**_T56;void**_T57;void**_T58;unsigned _T59;struct _fat_ptr*_T5A;struct _fat_ptr _T5B;struct _fat_ptr _T5C;int _T5D;struct Cyc_String_pa_PrintArg_struct _T5E;struct Cyc_String_pa_PrintArg_struct _T5F;struct Cyc_String_pa_PrintArg_struct _T60;struct _fat_ptr*_T61;void**_T62;void**_T63;void**_T64;unsigned _T65;struct _fat_ptr*_T66;struct _fat_ptr _T67;struct _fat_ptr _T68;unsigned _T69;unsigned _T6A;struct Cyc_String_pa_PrintArg_struct _T6B;struct Cyc_String_pa_PrintArg_struct _T6C;struct Cyc_String_pa_PrintArg_struct _T6D;struct _fat_ptr*_T6E;void**_T6F;void**_T70;void**_T71;unsigned _T72;struct _fat_ptr*_T73;struct _fat_ptr _T74;struct _fat_ptr _T75;struct Cyc_List_List*_T76;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T77;void*_T78;unsigned _T79;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T7A;void*_T7B;void*_T7C;int _T7D;struct Cyc_String_pa_PrintArg_struct _T7E;struct Cyc_String_pa_PrintArg_struct _T7F;struct Cyc_String_pa_PrintArg_struct _T80;void**_T81;void**_T82;void**_T83;unsigned _T84;struct _fat_ptr*_T85;struct _fat_ptr _T86;struct _fat_ptr _T87;struct Cyc_String_pa_PrintArg_struct _T88;void**_T89;unsigned _T8A;struct _fat_ptr*_T8B;struct _fat_ptr _T8C;struct _fat_ptr _T8D;int _T8E;struct Cyc_String_pa_PrintArg_struct _T8F;struct Cyc_String_pa_PrintArg_struct _T90;struct _fat_ptr*_T91;struct Cyc_String_pa_PrintArg_struct _T92;struct Cyc_String_pa_PrintArg_struct _T93;void**_T94;void**_T95;void**_T96;void**_T97;unsigned _T98;struct _fat_ptr*_T99;struct _fat_ptr _T9A;struct _fat_ptr _T9B;struct Cyc_List_List*_T9C;struct Cyc_List_List*_T9D;struct Cyc_String_pa_PrintArg_struct _T9E;struct Cyc_String_pa_PrintArg_struct _T9F;struct Cyc_List_List*_TA0;void*_TA1;struct Cyc_Absyn_Aggrfield*_TA2;struct _fat_ptr*_TA3;void**_TA4;void**_TA5;unsigned _TA6;struct _fat_ptr*_TA7;struct _fat_ptr _TA8;struct _fat_ptr _TA9;struct Cyc_String_pa_PrintArg_struct _TAA;struct Cyc_String_pa_PrintArg_struct _TAB;struct Cyc_List_List*_TAC;void*_TAD;struct Cyc_Absyn_Aggrfield*_TAE;struct _fat_ptr*_TAF;void**_TB0;void**_TB1;unsigned _TB2;struct _fat_ptr*_TB3;struct _fat_ptr _TB4;struct _fat_ptr _TB5;struct Cyc_String_pa_PrintArg_struct _TB6;void**_TB7;unsigned _TB8;struct _fat_ptr*_TB9;struct _fat_ptr _TBA;struct _fat_ptr _TBB;int _TBC;enum Cyc_Absyn_Scope _TBD;int _TBE;struct Cyc_Absyn_Aggrdecl*_TBF;enum Cyc_Absyn_Scope _TC0;int _TC1;struct Cyc_Absyn_Aggrdecl*_TC2;struct Cyc_Absyn_Aggrdecl*_TC3;struct Cyc_Absyn_Aggrdecl*_TC4;struct Cyc_Absyn_Aggrdecl*_TC5;struct Cyc_Absyn_Aggrdecl*_TC6;_T0=d0;_T1=_T0->name;{
# 140
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T1);
int res=1;_T2=d0;_T3=_T2->kind;_T4=(int)_T3;_T5=d1;_T6=_T5->kind;_T7=(int)_T6;
# 146
if(_T4==_T7)goto _TL2C;else{goto _TL2E;}_TL2E: return 0;_TL2C: _T8=d0;_T9=_T8->tvs;_TA=d1;_TB=_TA->tvs;_TC=
# 149
_tag_fat("type",sizeof(char),5U);_TD=v;_TE=loc;_TF=msg;_T10=Cyc_Tcdecl_check_tvs(_T9,_TB,_TC,_TD,_TE,_TF);if(_T10)goto _TL2F;else{goto _TL31;}_TL31: return 0;_TL2F: _T11=d0;_T12=_T11->sc;_T13=d1;_T14=_T13->sc;_T15=
# 153
_tag_fat("type",sizeof(char),5U);_T16=v;_T17=loc;_T18=msg;{
# 152
struct _tuple11 _TC7=
Cyc_Tcdecl_merge_scope(_T12,_T14,_T15,_T16,_T17,_T18,1);int _TC8;enum Cyc_Absyn_Scope _TC9;_TC9=_TC7.f0;_TC8=_TC7.f1;{enum Cyc_Absyn_Scope new_scope=_TC9;int res_scope=_TC8;_T19=res_scope;
if(_T19)goto _TL32;else{goto _TL34;}_TL34: res=0;goto _TL33;_TL32: _TL33: _T1A=d0;_T1B=_T1A->attributes;_T1C=d1;_T1D=_T1C->attributes;_T1E=
# 157
_tag_fat("type",sizeof(char),5U);_T1F=v;_T20=loc;_T21=msg;_T22=Cyc_Tcdecl_check_atts(_T1B,_T1D,_T1E,_T1F,_T20,_T21);if(_T22)goto _TL35;else{goto _TL37;}_TL37: res=0;goto _TL36;_TL35: _TL36: {
# 159
struct Cyc_Absyn_Aggrdecl*d2;{struct _tuple14 _TCA;_T24=d0;
# 161
_TCA.f0=_T24->impl;_T25=d1;_TCA.f1=_T25->impl;_T23=_TCA;}{struct _tuple14 _TCA=_T23;struct Cyc_List_List*_TCB;int _TCC;struct Cyc_List_List*_TCD;struct Cyc_List_List*_TCE;struct Cyc_List_List*_TCF;struct Cyc_List_List*_TD0;int _TD1;struct Cyc_List_List*_TD2;struct Cyc_List_List*_TD3;struct Cyc_List_List*_TD4;_T26=_TCA.f1;if(_T26!=0)goto _TL38;
d2=d0;goto _LL3;_TL38: _T27=_TCA.f0;if(_T27!=0)goto _TL3A;
d2=d1;goto _LL3;_TL3A: _T28=_TCA.f0;{struct Cyc_Absyn_AggrdeclImpl _TD5=*_T28;_TD4=_TD5.exist_vars;_TD3=_TD5.qual_bnd;_TD2=_TD5.fields;_TD1=_TD5.tagged;_TD0=_TD5.effconstr;}_T29=_TCA.f1;{struct Cyc_Absyn_AggrdeclImpl _TD5=*_T29;_TCF=_TD5.exist_vars;_TCE=_TD5.qual_bnd;_TCD=_TD5.fields;_TCC=_TD5.tagged;_TCB=_TD5.effconstr;}{struct Cyc_List_List*exist_vars0=_TD4;struct Cyc_List_List*qb0=_TD3;struct Cyc_List_List*f0s=_TD2;int tagged0=_TD1;struct Cyc_List_List*ec0=_TD0;struct Cyc_List_List*exist_vars1=_TCF;struct Cyc_List_List*qb1=_TCE;struct Cyc_List_List*f1s=_TCD;int tagged1=_TCC;struct Cyc_List_List*ec1=_TCB;_T2A=exist_vars0;_T2B=exist_vars1;_T2C=
# 168
_tag_fat("type",sizeof(char),5U);_T2D=v;_T2E=loc;_T2F=msg;_T30=Cyc_Tcdecl_check_tvs(_T2A,_T2B,_T2C,_T2D,_T2E,_T2F);if(_T30)goto _TL3C;else{goto _TL3E;}
_TL3E: return 0;_TL3C: _T31=d0;_T32=_T31->tvs;_T33=exist_vars0;_T34=
# 172
Cyc_List_append(_T32,_T33);_T35=d1;_T36=_T35->tvs;_T37=exist_vars1;_T38=
Cyc_List_append(_T36,_T37);{
# 172
struct Cyc_List_List*inst=Cyc_Tcdecl_build_tvs_map(_T34,_T38);
# 182 "tcdecl.cyc"
struct Cyc_List_List*ec11=Cyc_Tcutil_rsubst_effconstr(Cyc_Core_heap_region,inst,ec1);_T39=
Cyc_Tcutil_cmp_effect_constraints(ec0,ec11);if(!_T39)goto _TL3F;_T3A=loc;_T3B=msg;_T3C=
_tag_fat("Effect constraints do not match",sizeof(char),32U);_T3D=_tag_fat(0U,sizeof(void*),0);Cyc_Tcdecl_merr(_T3A,_T3B,_T3C,_T3D);goto _TL40;_TL3F: _TL40:
# 188
 _TL44: if(f0s!=0)goto _TL45;else{goto _TL43;}_TL45: if(f1s!=0)goto _TL42;else{goto _TL43;}
_TL42: _T3E=f0s;_T3F=_T3E->hd;{struct Cyc_Absyn_Aggrfield*_TD5=(struct Cyc_Absyn_Aggrfield*)_T3F;struct Cyc_Absyn_Exp*_TD6;struct Cyc_List_List*_TD7;struct Cyc_Absyn_Exp*_TD8;void*_TD9;struct Cyc_Absyn_Tqual _TDA;struct _fat_ptr*_TDB;{struct Cyc_Absyn_Aggrfield _TDC=*_TD5;_TDB=_TDC.name;_TDA=_TDC.tq;_TD9=_TDC.type;_TD8=_TDC.width;_TD7=_TDC.attributes;_TD6=_TDC.requires_clause;}{struct _fat_ptr*fn0=_TDB;struct Cyc_Absyn_Tqual tq0=_TDA;void*t0=_TD9;struct Cyc_Absyn_Exp*width0=_TD8;struct Cyc_List_List*atts0=_TD7;struct Cyc_Absyn_Exp*req0=_TD6;_T40=f1s;_T41=_T40->hd;{
struct Cyc_Absyn_Aggrfield*_TDC=(struct Cyc_Absyn_Aggrfield*)_T41;struct Cyc_Absyn_Exp*_TDD;struct Cyc_List_List*_TDE;struct Cyc_Absyn_Exp*_TDF;void*_TE0;struct Cyc_Absyn_Tqual _TE1;struct _fat_ptr*_TE2;{struct Cyc_Absyn_Aggrfield _TE3=*_TDC;_TE2=_TE3.name;_TE1=_TE3.tq;_TE0=_TE3.type;_TDF=_TE3.width;_TDE=_TE3.attributes;_TDD=_TE3.requires_clause;}{struct _fat_ptr*fn1=_TE2;struct Cyc_Absyn_Tqual tq1=_TE1;void*t1=_TE0;struct Cyc_Absyn_Exp*width1=_TDF;struct Cyc_List_List*atts1=_TDE;struct Cyc_Absyn_Exp*req1=_TDD;_T42=
# 192
Cyc_strptrcmp(fn0,fn1);if(_T42==0)goto _TL46;{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;
_TE3.f1=_tag_fat("type",sizeof(char),5U);_T43=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T43;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;
_TE4.f1=v;_T44=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T44;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_T46=fn0;_TE5.f1=*_T46;_T45=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T45;{struct Cyc_String_pa_PrintArg_struct _TE6;_TE6.tag=0;_T48=fn1;_TE6.f1=*_T48;_T47=_TE6;}{struct Cyc_String_pa_PrintArg_struct _TE6=_T47;void*_TE7[4];_T49=_TE7 + 0;*_T49=& _TE3;_T4A=_TE7 + 1;*_T4A=& _TE4;_T4B=_TE7 + 2;*_T4B=& _TE5;_T4C=_TE7 + 3;*_T4C=& _TE6;_T4D=loc;_T4E=msg;_T4F=
# 193
_tag_fat("%s %s : field name mismatch %s != %s",sizeof(char),37U);_T50=_tag_fat(_TE7,sizeof(void*),4);Cyc_Tcdecl_merr(_T4D,_T4E,_T4F,_T50);}}}}
# 195
return 0;_TL46: _T51=
# 197
Cyc_Atts_same_atts(atts0,atts1);if(_T51)goto _TL48;else{goto _TL4A;}
_TL4A:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("type",sizeof(char),5U);_T52=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T52;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;_TE4.f1=v;_T53=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T53;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_T55=fn0;_TE5.f1=*_T55;_T54=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T54;void*_TE6[3];_T56=_TE6 + 0;*_T56=& _TE3;_T57=_TE6 + 1;*_T57=& _TE4;_T58=_TE6 + 2;*_T58=& _TE5;_T59=loc;_T5A=msg;_T5B=_tag_fat("%s %s : attribute mismatch on field %s",sizeof(char),39U);_T5C=_tag_fat(_TE6,sizeof(void*),3);Cyc_Tcdecl_merr(_T59,_T5A,_T5B,_T5C);}}}
res=0;goto _TL49;_TL48: _TL49: _T5D=
# 201
Cyc_Absyn_equal_tqual(tq0,tq1);if(_T5D)goto _TL4B;else{goto _TL4D;}
_TL4D:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("type",sizeof(char),5U);_T5E=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T5E;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;_TE4.f1=v;_T5F=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T5F;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_T61=fn0;_TE5.f1=*_T61;_T60=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T60;void*_TE6[3];_T62=_TE6 + 0;*_T62=& _TE3;_T63=_TE6 + 1;*_T63=& _TE4;_T64=_TE6 + 2;*_T64=& _TE5;_T65=loc;_T66=msg;_T67=_tag_fat("%s %s : qualifier mismatch on field %s",sizeof(char),39U);_T68=_tag_fat(_TE6,sizeof(void*),3);Cyc_Tcdecl_merr(_T65,_T66,_T67,_T68);}}}
res=0;goto _TL4C;_TL4B: _TL4C:
# 205
 if(width0!=0)goto _TL54;else{goto _TL52;}_TL54: if(width1!=0)goto _TL53;else{goto _TL52;}_TL53: _T69=
Cyc_Tcdecl_get_uint_const_value(width0);_T6A=
Cyc_Tcdecl_get_uint_const_value(width1);
# 205
if(_T69!=_T6A)goto _TL50;else{goto _TL52;}_TL52: if(width0==0)goto _TL55;else{goto _TL51;}_TL55: if(width1!=0)goto _TL50;else{goto _TL51;}_TL51: if(width0!=0)goto _TL56;else{goto _TL4E;}_TL56: if(width1==0)goto _TL50;else{goto _TL4E;}
# 210
_TL50:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;_TE3.f1=_tag_fat("type",sizeof(char),5U);_T6B=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T6B;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;_TE4.f1=v;_T6C=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T6C;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_T6E=fn0;_TE5.f1=*_T6E;_T6D=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T6D;void*_TE6[3];_T6F=_TE6 + 0;*_T6F=& _TE3;_T70=_TE6 + 1;*_T70=& _TE4;_T71=_TE6 + 2;*_T71=& _TE5;_T72=loc;_T73=msg;_T74=_tag_fat("%s %s : bitfield mismatch on field %s",sizeof(char),38U);_T75=_tag_fat(_TE6,sizeof(void*),3);Cyc_Tcdecl_merr(_T72,_T73,_T74,_T75);}}}
res=0;goto _TL4F;_TL4E: _TL4F:
# 213
 if(req0==0)goto _TL57;if(req1==0)goto _TL57;_T76=inst;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TE3=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TE3->tag=9;
_TE3->f1=req1;_T77=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TE3;}_T78=(void*)_T77;{void*subst_w1=Cyc_Tcutil_substitute(_T76,_T78);_T79=loc;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TE3=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TE3->tag=9;
_TE3->f1=req0;_T7A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TE3;}_T7B=(void*)_T7A;_T7C=subst_w1;_T7D=Cyc_Tcdecl_check_type(_T79,_T7B,_T7C);if(_T7D)goto _TL59;else{goto _TL5B;}
_TL5B:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;
_TE3.f1=v;_T7E=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T7E;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;_TE4.f1=Cyc_Absynpp_exp2string(req0);_T7F=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T7F;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_TE5.f1=Cyc_Absynpp_exp2string(req1);_T80=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T80;void*_TE6[3];_T81=_TE6 + 0;*_T81=& _TE3;_T82=_TE6 + 1;*_T82=& _TE4;_T83=_TE6 + 2;*_T83=& _TE5;_T84=loc;_T85=msg;_T86=
# 216
_tag_fat("type %s : mismatch on @requires clauses: %s != %s",sizeof(char),50U);_T87=_tag_fat(_TE6,sizeof(void*),3);Cyc_Tcdecl_merr(_T84,_T85,_T86,_T87);}}}
# 218
res=0;goto _TL5A;_TL59: _TL5A:;}goto _TL58;
# 220
_TL57: if(req0==0)goto _TL60;else{goto _TL5F;}_TL60: if(req1!=0)goto _TL5E;else{goto _TL5F;}_TL5F: if(req0!=0)goto _TL61;else{goto _TL5C;}_TL61: if(req1==0)goto _TL5E;else{goto _TL5C;}
# 222
_TL5E:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;_TE3.f1=v;_T88=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T88;void*_TE4[1];_T89=_TE4 + 0;*_T89=& _TE3;_T8A=loc;_T8B=msg;_T8C=_tag_fat("type %s : mismatch on @requires clauses",sizeof(char),40U);_T8D=_tag_fat(_TE4,sizeof(void*),1);Cyc_Tcdecl_merr(_T8A,_T8B,_T8C,_T8D);}
res=0;goto _TL5D;_TL5C: _TL5D: _TL58: {
# 226
void*subst_t1=Cyc_Tcutil_substitute(inst,t1);_T8E=
Cyc_Tcdecl_check_type(loc,t0,subst_t1);if(_T8E)goto _TL62;else{goto _TL64;}
_TL64:{struct Cyc_String_pa_PrintArg_struct _TE3;_TE3.tag=0;
_TE3.f1=v;_T8F=_TE3;}{struct Cyc_String_pa_PrintArg_struct _TE3=_T8F;{struct Cyc_String_pa_PrintArg_struct _TE4;_TE4.tag=0;_T91=fn0;_TE4.f1=*_T91;_T90=_TE4;}{struct Cyc_String_pa_PrintArg_struct _TE4=_T90;{struct Cyc_String_pa_PrintArg_struct _TE5;_TE5.tag=0;_TE5.f1=Cyc_Absynpp_typ2string(t0);_T92=_TE5;}{struct Cyc_String_pa_PrintArg_struct _TE5=_T92;{struct Cyc_String_pa_PrintArg_struct _TE6;_TE6.tag=0;_TE6.f1=Cyc_Absynpp_typ2string(subst_t1);_T93=_TE6;}{struct Cyc_String_pa_PrintArg_struct _TE6=_T93;void*_TE7[4];_T94=_TE7 + 0;*_T94=& _TE3;_T95=_TE7 + 1;*_T95=& _TE4;_T96=_TE7 + 2;*_T96=& _TE5;_T97=_TE7 + 3;*_T97=& _TE6;_T98=loc;_T99=msg;_T9A=
# 228
_tag_fat("type %s : type mismatch on field %s: %s != %s",sizeof(char),46U);_T9B=_tag_fat(_TE7,sizeof(void*),4);Cyc_Tcdecl_merr(_T98,_T99,_T9A,_T9B);}}}}
# 230
Cyc_Unify_explain_failure();
res=0;goto _TL63;_TL62: _TL63:;}}}}}_T9C=f0s;
# 188
f0s=_T9C->tl;_T9D=f1s;f1s=_T9D->tl;goto _TL44;_TL43:
# 235
 if(f0s==0)goto _TL65;{struct Cyc_String_pa_PrintArg_struct _TD5;_TD5.tag=0;
_TD5.f1=v;_T9E=_TD5;}{struct Cyc_String_pa_PrintArg_struct _TD5=_T9E;{struct Cyc_String_pa_PrintArg_struct _TD6;_TD6.tag=0;_TA0=f0s;_TA1=_TA0->hd;_TA2=(struct Cyc_Absyn_Aggrfield*)_TA1;_TA3=_TA2->name;_TD6.f1=*_TA3;_T9F=_TD6;}{struct Cyc_String_pa_PrintArg_struct _TD6=_T9F;void*_TD7[2];_TA4=_TD7 + 0;*_TA4=& _TD5;_TA5=_TD7 + 1;*_TA5=& _TD6;_TA6=loc;_TA7=msg;_TA8=_tag_fat("type %s is missing field %s",sizeof(char),28U);_TA9=_tag_fat(_TD7,sizeof(void*),2);Cyc_Tcdecl_merr(_TA6,_TA7,_TA8,_TA9);}}
res=0;goto _TL66;_TL65: _TL66:
# 239
 if(f1s==0)goto _TL67;{struct Cyc_String_pa_PrintArg_struct _TD5;_TD5.tag=0;
_TD5.f1=v;_TAA=_TD5;}{struct Cyc_String_pa_PrintArg_struct _TD5=_TAA;{struct Cyc_String_pa_PrintArg_struct _TD6;_TD6.tag=0;_TAC=f1s;_TAD=_TAC->hd;_TAE=(struct Cyc_Absyn_Aggrfield*)_TAD;_TAF=_TAE->name;_TD6.f1=*_TAF;_TAB=_TD6;}{struct Cyc_String_pa_PrintArg_struct _TD6=_TAB;void*_TD7[2];_TB0=_TD7 + 0;*_TB0=& _TD5;_TB1=_TD7 + 1;*_TB1=& _TD6;_TB2=loc;_TB3=msg;_TB4=_tag_fat("type %s has extra field %s",sizeof(char),27U);_TB5=_tag_fat(_TD7,sizeof(void*),2);Cyc_Tcdecl_merr(_TB2,_TB3,_TB4,_TB5);}}
res=0;goto _TL68;_TL67: _TL68:
# 244
 if(tagged0==tagged1)goto _TL69;{struct Cyc_String_pa_PrintArg_struct _TD5;_TD5.tag=0;
_TD5.f1=v;_TB6=_TD5;}{struct Cyc_String_pa_PrintArg_struct _TD5=_TB6;void*_TD6[1];_TB7=_TD6 + 0;*_TB7=& _TD5;_TB8=loc;_TB9=msg;_TBA=_tag_fat("%s : previous declaration disagrees with @tagged qualifier",sizeof(char),59U);_TBB=_tag_fat(_TD6,sizeof(void*),1);Cyc_Tcdecl_merr(_TB8,_TB9,_TBA,_TBB);}
res=0;goto _TL6A;_TL69: _TL6A:
# 249
 d2=d0;goto _LL3;}}_LL3:;}_TBC=res;
# 253
if(_TBC)goto _TL6B;else{goto _TL6D;}_TL6D: return 0;_TL6B: _TBD=new_scope;_TBE=(int)_TBD;_TBF=d2;_TC0=_TBF->sc;_TC1=(int)_TC0;
if(_TBE!=_TC1)goto _TL6E;_TC2=d2;return _TC2;_TL6E:{struct Cyc_Absyn_Aggrdecl*_TCA=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TC4=d2;
*_TCA=*_TC4;_TC3=(struct Cyc_Absyn_Aggrdecl*)_TCA;}d2=_TC3;_TC5=d2;
_TC5->sc=new_scope;_TC6=d2;
return _TC6;}}}}}
# 260
inline static struct _fat_ptr Cyc_Tcdecl_is_x2string(int is_x){struct _fat_ptr _T0;int _T1;_T1=is_x;
if(!_T1)goto _TL70;_T0=_tag_fat("@extensible datatype",sizeof(char),21U);goto _TL71;_TL70: _T0=_tag_fat("datatype",sizeof(char),9U);_TL71: return _T0;}struct _tuple15{struct Cyc_Absyn_Tqual f0;void*f1;};
# 266
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _fat_ptr t,struct _fat_ptr v,struct _fat_ptr*msg){struct Cyc_Absyn_Datatypefield*_T0;struct Cyc_Absyn_Datatypefield*_T1;struct _tuple0*_T2;struct _tuple0 _T3;struct _fat_ptr*_T4;struct Cyc_Absyn_Datatypefield*_T5;struct _tuple0*_T6;struct _tuple0 _T7;struct _fat_ptr*_T8;int _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_String_pa_PrintArg_struct _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_Absyn_Datatypefield*_TD;struct _tuple0*_TE;struct _tuple0 _TF;struct _fat_ptr*_T10;struct Cyc_String_pa_PrintArg_struct _T11;struct Cyc_Absyn_Datatypefield*_T12;struct _tuple0*_T13;struct _tuple0 _T14;struct _fat_ptr*_T15;void**_T16;void**_T17;void**_T18;void**_T19;unsigned _T1A;struct _fat_ptr*_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;struct Cyc_Absyn_Datatypefield*_T1E;struct _tuple0*_T1F;struct _tuple0 _T20;struct _fat_ptr*_T21;struct Cyc_Absyn_Datatypefield*_T22;enum Cyc_Absyn_Scope _T23;struct Cyc_Absyn_Datatypefield*_T24;enum Cyc_Absyn_Scope _T25;struct _fat_ptr _T26;struct Cyc_String_pa_PrintArg_struct _T27;struct Cyc_String_pa_PrintArg_struct _T28;void**_T29;void**_T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;unsigned _T2E;struct _fat_ptr*_T2F;struct Cyc_Absyn_Datatypefield*_T30;struct Cyc_Absyn_Datatypefield*_T31;int _T32;int _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct Cyc_String_pa_PrintArg_struct _T35;struct Cyc_String_pa_PrintArg_struct _T36;void**_T37;void**_T38;void**_T39;unsigned _T3A;struct _fat_ptr*_T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;struct Cyc_List_List*_T3E;void*_T3F;struct _tuple15*_T40;struct _tuple15 _T41;struct Cyc_Absyn_Tqual _T42;struct Cyc_List_List*_T43;void*_T44;struct _tuple15*_T45;struct _tuple15 _T46;struct Cyc_Absyn_Tqual _T47;int _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct Cyc_String_pa_PrintArg_struct _T4A;struct Cyc_String_pa_PrintArg_struct _T4B;void**_T4C;void**_T4D;void**_T4E;unsigned _T4F;struct _fat_ptr*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;void*_T55;struct _tuple15*_T56;struct _tuple15 _T57;void*_T58;unsigned _T59;struct Cyc_List_List*_T5A;void*_T5B;struct _tuple15*_T5C;struct _tuple15 _T5D;void*_T5E;void*_T5F;int _T60;struct Cyc_String_pa_PrintArg_struct _T61;struct Cyc_String_pa_PrintArg_struct _T62;struct Cyc_String_pa_PrintArg_struct _T63;struct Cyc_String_pa_PrintArg_struct _T64;struct Cyc_List_List*_T65;void*_T66;struct _tuple15*_T67;struct _tuple15 _T68;void*_T69;struct Cyc_String_pa_PrintArg_struct _T6A;void**_T6B;void**_T6C;void**_T6D;void**_T6E;void**_T6F;unsigned _T70;struct _fat_ptr*_T71;struct _fat_ptr _T72;struct _fat_ptr _T73;struct Cyc_List_List*_T74;struct Cyc_List_List*_T75;int _T76;struct Cyc_Absyn_Datatypefield*_T77;enum Cyc_Absyn_Scope _T78;int _T79;enum Cyc_Absyn_Scope _T7A;int _T7B;struct Cyc_Absyn_Datatypefield*_T7C;struct Cyc_Absyn_Datatypefield*_T7D;struct Cyc_Absyn_Datatypefield*_T7E;struct Cyc_Absyn_Datatypefield*_T7F;struct Cyc_Absyn_Datatypefield*_T80;_T0=f1;{
# 270
unsigned loc=_T0->loc;_T1=f0;_T2=_T1->name;_T3=*_T2;_T4=_T3.f1;_T5=f1;_T6=_T5->name;_T7=*_T6;_T8=_T7.f1;_T9=
# 273
Cyc_strptrcmp(_T4,_T8);if(_T9==0)goto _TL72;{struct Cyc_String_pa_PrintArg_struct _T81;_T81.tag=0;
# 275
_T81.f1=t;_TA=_T81;}{struct Cyc_String_pa_PrintArg_struct _T81=_TA;{struct Cyc_String_pa_PrintArg_struct _T82;_T82.tag=0;_T82.f1=v;_TB=_T82;}{struct Cyc_String_pa_PrintArg_struct _T82=_TB;{struct Cyc_String_pa_PrintArg_struct _T83;_T83.tag=0;_TD=f1;_TE=_TD->name;_TF=*_TE;_T10=_TF.f1;_T83.f1=*_T10;_TC=_T83;}{struct Cyc_String_pa_PrintArg_struct _T83=_TC;{struct Cyc_String_pa_PrintArg_struct _T84;_T84.tag=0;_T12=f0;_T13=_T12->name;_T14=*_T13;_T15=_T14.f1;_T84.f1=*_T15;_T11=_T84;}{struct Cyc_String_pa_PrintArg_struct _T84=_T11;void*_T85[4];_T16=_T85 + 0;*_T16=& _T81;_T17=_T85 + 1;*_T17=& _T82;_T18=_T85 + 2;*_T18=& _T83;_T19=_T85 + 3;*_T19=& _T84;_T1A=loc;_T1B=msg;_T1C=
# 274
_tag_fat("%s %s: field name mismatch %s != %s",sizeof(char),36U);_T1D=_tag_fat(_T85,sizeof(void*),4);Cyc_Tcdecl_merr(_T1A,_T1B,_T1C,_T1D);}}}}
# 276
return 0;_TL72: _T1E=f0;_T1F=_T1E->name;_T20=*_T1F;_T21=_T20.f1;{
# 279
struct _fat_ptr name=*_T21;_T22=f0;_T23=_T22->sc;_T24=f1;_T25=_T24->sc;{struct Cyc_String_pa_PrintArg_struct _T81;_T81.tag=0;
# 283
_T81.f1=t;_T27=_T81;}{struct Cyc_String_pa_PrintArg_struct _T81=_T27;{struct Cyc_String_pa_PrintArg_struct _T82;_T82.tag=0;_T82.f1=v;_T28=_T82;}{struct Cyc_String_pa_PrintArg_struct _T82=_T28;void*_T83[2];_T29=_T83 + 0;*_T29=& _T81;_T2A=_T83 + 1;*_T2A=& _T82;_T2B=_tag_fat("in %s %s, field",sizeof(char),16U);_T2C=_tag_fat(_T83,sizeof(void*),2);_T26=Cyc_aprintf(_T2B,_T2C);}}_T2D=name;_T2E=loc;_T2F=msg;{
# 282
struct _tuple11 _T81=
Cyc_Tcdecl_merge_scope(_T23,_T25,_T26,_T2D,_T2E,_T2F,0);int _T82;enum Cyc_Absyn_Scope _T83;_T83=_T81.f0;_T82=_T81.f1;{enum Cyc_Absyn_Scope new_scope=_T83;int res=_T82;_T30=f0;{
# 286
struct Cyc_List_List*typs0=_T30->typs;_T31=f1;{
struct Cyc_List_List*typs1=_T31->typs;_T32=
Cyc_List_length(typs0);_T33=Cyc_List_length(typs1);if(_T32==_T33)goto _TL74;{struct Cyc_String_pa_PrintArg_struct _T84;_T84.tag=0;
_T84.f1=t;_T34=_T84;}{struct Cyc_String_pa_PrintArg_struct _T84=_T34;{struct Cyc_String_pa_PrintArg_struct _T85;_T85.tag=0;_T85.f1=v;_T35=_T85;}{struct Cyc_String_pa_PrintArg_struct _T85=_T35;{struct Cyc_String_pa_PrintArg_struct _T86;_T86.tag=0;_T86.f1=name;_T36=_T86;}{struct Cyc_String_pa_PrintArg_struct _T86=_T36;void*_T87[3];_T37=_T87 + 0;*_T37=& _T84;_T38=_T87 + 1;*_T38=& _T85;_T39=_T87 + 2;*_T39=& _T86;_T3A=loc;_T3B=msg;_T3C=_tag_fat("%s %s, field %s: parameter number mismatch",sizeof(char),43U);_T3D=_tag_fat(_T87,sizeof(void*),3);Cyc_Tcdecl_merr(_T3A,_T3B,_T3C,_T3D);}}}
res=0;goto _TL75;_TL74: _TL75:
# 293
 _TL79: if(typs0!=0)goto _TL7A;else{goto _TL78;}_TL7A: if(typs1!=0)goto _TL77;else{goto _TL78;}
# 295
_TL77: _T3E=typs0;_T3F=_T3E->hd;_T40=(struct _tuple15*)_T3F;_T41=*_T40;_T42=_T41.f0;_T43=typs1;_T44=_T43->hd;_T45=(struct _tuple15*)_T44;_T46=*_T45;_T47=_T46.f0;_T48=Cyc_Absyn_equal_tqual(_T42,_T47);if(_T48)goto _TL7B;else{goto _TL7D;}
_TL7D:{struct Cyc_String_pa_PrintArg_struct _T84;_T84.tag=0;_T84.f1=t;_T49=_T84;}{struct Cyc_String_pa_PrintArg_struct _T84=_T49;{struct Cyc_String_pa_PrintArg_struct _T85;_T85.tag=0;_T85.f1=v;_T4A=_T85;}{struct Cyc_String_pa_PrintArg_struct _T85=_T4A;{struct Cyc_String_pa_PrintArg_struct _T86;_T86.tag=0;_T86.f1=name;_T4B=_T86;}{struct Cyc_String_pa_PrintArg_struct _T86=_T4B;void*_T87[3];_T4C=_T87 + 0;*_T4C=& _T84;_T4D=_T87 + 1;*_T4D=& _T85;_T4E=_T87 + 2;*_T4E=& _T86;_T4F=loc;_T50=msg;_T51=_tag_fat("%s %s, field %s: parameter qualifier",sizeof(char),37U);_T52=_tag_fat(_T87,sizeof(void*),3);Cyc_Tcdecl_merr(_T4F,_T50,_T51,_T52);}}}
res=0;goto _TL7C;_TL7B: _TL7C: _T53=inst;_T54=typs1;_T55=_T54->hd;_T56=(struct _tuple15*)_T55;_T57=*_T56;_T58=_T57.f1;{
# 299
void*subst_t1=Cyc_Tcutil_substitute(_T53,_T58);_T59=loc;_T5A=typs0;_T5B=_T5A->hd;_T5C=(struct _tuple15*)_T5B;_T5D=*_T5C;_T5E=_T5D.f1;_T5F=subst_t1;_T60=
# 301
Cyc_Tcdecl_check_type(_T59,_T5E,_T5F);if(_T60)goto _TL7E;else{goto _TL80;}
_TL80:{struct Cyc_String_pa_PrintArg_struct _T84;_T84.tag=0;
_T84.f1=t;_T61=_T84;}{struct Cyc_String_pa_PrintArg_struct _T84=_T61;{struct Cyc_String_pa_PrintArg_struct _T85;_T85.tag=0;_T85.f1=v;_T62=_T85;}{struct Cyc_String_pa_PrintArg_struct _T85=_T62;{struct Cyc_String_pa_PrintArg_struct _T86;_T86.tag=0;_T86.f1=name;_T63=_T86;}{struct Cyc_String_pa_PrintArg_struct _T86=_T63;{struct Cyc_String_pa_PrintArg_struct _T87;_T87.tag=0;_T65=typs0;_T66=_T65->hd;_T67=(struct _tuple15*)_T66;_T68=*_T67;_T69=_T68.f1;_T87.f1=Cyc_Absynpp_typ2string(_T69);_T64=_T87;}{struct Cyc_String_pa_PrintArg_struct _T87=_T64;{struct Cyc_String_pa_PrintArg_struct _T88;_T88.tag=0;_T88.f1=Cyc_Absynpp_typ2string(subst_t1);_T6A=_T88;}{struct Cyc_String_pa_PrintArg_struct _T88=_T6A;void*_T89[5];_T6B=_T89 + 0;*_T6B=& _T84;_T6C=_T89 + 1;*_T6C=& _T85;_T6D=_T89 + 2;*_T6D=& _T86;_T6E=_T89 + 3;*_T6E=& _T87;_T6F=_T89 + 4;*_T6F=& _T88;_T70=loc;_T71=msg;_T72=
# 302
_tag_fat("%s %s, field %s: parameter type mismatch %s != %s",sizeof(char),50U);_T73=_tag_fat(_T89,sizeof(void*),5);Cyc_Tcdecl_merr(_T70,_T71,_T72,_T73);}}}}}
# 304
Cyc_Unify_explain_failure();
res=0;goto _TL7F;_TL7E: _TL7F:;}_T74=typs0;
# 293
typs0=_T74->tl;_T75=typs1;typs1=_T75->tl;goto _TL79;_TL78: _T76=res;
# 309
if(_T76)goto _TL81;else{goto _TL83;}_TL83: return 0;_TL81: _T77=f0;_T78=_T77->sc;_T79=(int)_T78;_T7A=new_scope;_T7B=(int)_T7A;
if(_T79!=_T7B)goto _TL84;_T7C=f0;return _T7C;_TL84: {
struct Cyc_Absyn_Datatypefield*f2;f2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T7D=f2;_T7E=f0;*_T7D=*_T7E;_T7F=f2;
_T7F->sc=new_scope;_T80=f2;
return _T80;}}}}}}}}
# 316
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){struct _tuple15*_T0;struct _tuple15*_T1;void*_T2;struct Cyc_Absyn_Tqual _T3;_T0=x;{struct _tuple15 _T4=*_T0;_T3=_T4.f0;_T2=_T4.f1;}{struct Cyc_Absyn_Tqual tq=_T3;void*t=_T2;{struct _tuple15*_T4=_cycalloc(sizeof(struct _tuple15));
# 320
_T4->f0=tq;_T4->f1=Cyc_Tcutil_substitute(inst,t);_T1=(struct _tuple15*)_T4;}return _T1;}}
# 325
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){struct Cyc_Absyn_Datatypefield*_T0;struct Cyc_Absyn_Datatypefield*_T1;struct Cyc_Absyn_Datatypefield*_T2;struct Cyc_List_List*(*_T3)(struct _tuple15*(*)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T4)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T5;struct Cyc_Absyn_Datatypefield*_T6;struct Cyc_List_List*_T7;struct Cyc_Absyn_Datatypefield*_T8;
# 327
struct Cyc_Absyn_Datatypefield*f0;f0=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T0=f0;_T1=f1;*_T0=*_T1;_T2=f0;_T4=Cyc_List_map_c;{
struct Cyc_List_List*(*_T9)(struct _tuple15*(*)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple15*(*)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*,struct Cyc_List_List*))_T4;_T3=_T9;}_T5=inst1;_T6=f1;_T7=_T6->typs;_T2->typs=_T3(Cyc_Tcdecl_substitute_datatypefield_f2,_T5,_T7);_T8=f0;
# 330
return _T8;}
# 337
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){struct Cyc_List_List**_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Datatypefield*_T3;struct _tuple0*_T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_Absyn_Datatypefield*_T7;struct _tuple0*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List**_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;int*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Datatypefield*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List**_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_Absyn_Datatypefield*_T1C;struct Cyc_List_List*_T1D;void*_T1E;struct Cyc_Absyn_Datatypefield*_T1F;struct Cyc_List_List*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct _fat_ptr*_T23;struct Cyc_Absyn_Datatypefield*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_Absyn_Datatypefield*_T27;int*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List**_T2B;struct Cyc_List_List*_T2C;int*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;int*_T30;struct Cyc_List_List**_T31;struct Cyc_List_List**_T32;struct Cyc_List_List**_T33;struct Cyc_List_List*_T34;
# 346
struct Cyc_List_List**f2sp;f2sp=_cycalloc(sizeof(struct Cyc_List_List*));_T0=f2sp;*_T0=0;{
struct Cyc_List_List**next=f2sp;
int cmp=-1;
# 350
_TL89: if(f0s!=0)goto _TL8A;else{goto _TL88;}_TL8A: if(f1s!=0)goto _TL87;else{goto _TL88;}
_TL87: _TL8B: if(f0s!=0)goto _TL8E;else{goto _TL8D;}_TL8E: _T1=f0s;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Datatypefield*)_T2;_T4=_T3->name;_T5=f1s;_T6=_T5->hd;_T7=(struct Cyc_Absyn_Datatypefield*)_T6;_T8=_T7->name;cmp=Cyc_Absyn_qvar_cmp(_T4,_T8);_T9=cmp;if(_T9 < 0)goto _TL8C;else{goto _TL8D;}
_TL8C:{struct Cyc_List_List*node;node=_cycalloc(sizeof(struct Cyc_List_List));_TA=node;_TB=f0s;_TC=_TB->hd;_TA->hd=(struct Cyc_Absyn_Datatypefield*)_TC;_TD=node;_TD->tl=0;_TE=next;
*_TE=node;_TF=node;
next=& _TF->tl;_T10=f0s;
f0s=_T10->tl;}goto _TL8B;_TL8D:
# 357
 if(f0s==0)goto _TL91;else{goto _TL92;}_TL92: if(cmp > 0)goto _TL91;else{goto _TL8F;}
# 359
_TL91: _T11=incl;*_T11=0;{
struct Cyc_List_List*node;node=_cycalloc(sizeof(struct Cyc_List_List));_T12=node;_T13=inst;_T14=f1s;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Datatypefield*)_T15;_T12->hd=Cyc_Tcdecl_substitute_datatypefield(_T13,_T16);_T17=node;_T17->tl=0;_T18=next;
*_T18=node;_T19=node;
next=& _T19->tl;}goto _TL90;
# 365
_TL8F: _T1A=f0s;_T1B=_T1A->hd;_T1C=(struct Cyc_Absyn_Datatypefield*)_T1B;_T1D=f1s;_T1E=_T1D->hd;_T1F=(struct Cyc_Absyn_Datatypefield*)_T1E;_T20=inst;_T21=t;_T22=v;_T23=msg;{struct Cyc_Absyn_Datatypefield*f2=Cyc_Tcdecl_merge_datatypefield(_T1C,_T1F,_T20,_T21,_T22,_T23);
if(f2==0)goto _TL93;_T24=f2;_T25=f0s;_T26=_T25->hd;_T27=(struct Cyc_Absyn_Datatypefield*)_T26;
if(_T24==_T27)goto _TL95;_T28=incl;*_T28=0;goto _TL96;_TL95: _TL96:{
struct Cyc_List_List*node;node=_cycalloc(sizeof(struct Cyc_List_List));_T29=node;_T29->hd=f2;_T2A=node;_T2A->tl=0;_T2B=next;
*_T2B=node;_T2C=node;
next=& _T2C->tl;}goto _TL94;
# 372
_TL93: _T2D=res;*_T2D=0;_TL94: _T2E=f0s;
# 374
f0s=_T2E->tl;}_TL90: _T2F=f1s;
# 350
f1s=_T2F->tl;goto _TL89;_TL88:
# 379
 if(f1s==0)goto _TL97;_T30=incl;
*_T30=0;_T31=next;
*_T31=f1s;goto _TL98;
# 383
_TL97: _T32=next;*_T32=f0s;_TL98: _T33=f2sp;_T34=*_T33;
# 385
return _T34;}}struct _tuple16{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 388
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){struct _tuple16 _T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct _tuple16 _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct _tuple16 _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;
if(f!=0)goto _TL99;{struct _tuple16 _TE;_TE.f0=0;_TE.f1=0;_T0=_TE;}return _T0;_TL99: _T1=f;_T2=_T1->tl;
if(_T2!=0)goto _TL9B;{struct _tuple16 _TE;_TE.f0=f;_TE.f1=0;_T3=_TE;}return _T3;_TL9B: _T4=f;_T5=_T4->tl;_T6=_T5->tl;{
struct _tuple16 _TE=Cyc_Tcdecl_split(_T6);struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;_T10=_TE.f0;_TF=_TE.f1;{struct Cyc_List_List*f1=_T10;struct Cyc_List_List*f2=_TF;{struct _tuple16 _T11;{struct Cyc_List_List*_T12=_cycalloc(sizeof(struct Cyc_List_List));_T9=f;
_T12->hd=_T9->hd;_T12->tl=f1;_T8=(struct Cyc_List_List*)_T12;}_T11.f0=_T8;{struct Cyc_List_List*_T12=_cycalloc(sizeof(struct Cyc_List_List));_TB=f;_TC=_TB->tl;_TD=_check_null(_TC);_T12->hd=_TD->hd;_T12->tl=f2;_TA=(struct Cyc_List_List*)_T12;}_T11.f1=_TA;_T7=_T11;}return _T7;}}}
# 397
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _fat_ptr*v,unsigned loc,struct _fat_ptr*msg){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;int*_T6;int*_T7;struct _fat_ptr _T8;struct _fat_ptr*_T9;struct _fat_ptr _TA;unsigned _TB;struct _fat_ptr*_TC;struct Cyc_List_List*_TD;
# 400
struct _tuple16 _TE=Cyc_Tcdecl_split(f);struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;_T10=_TE.f0;_TF=_TE.f1;{struct Cyc_List_List*f1=_T10;struct Cyc_List_List*f2=_TF;
if(f1==0)goto _TL9D;_T0=f1;_T1=_T0->tl;if(_T1==0)goto _TL9D;
f1=Cyc_Tcdecl_sort_xdatatype_fields(f1,res,v,loc,msg);goto _TL9E;_TL9D: _TL9E:
 if(f2==0)goto _TL9F;_T2=f2;_T3=_T2->tl;if(_T3==0)goto _TL9F;
f2=Cyc_Tcdecl_sort_xdatatype_fields(f2,res,v,loc,msg);goto _TLA0;_TL9F: _TLA0: _T4=f1;_T5=f2;_T6=res;{int*_T11=_cycalloc_atomic(sizeof(int));
*_T11=1;_T7=(int*)_T11;}_T8=
_tag_fat("@extensible datatype",sizeof(char),21U);_T9=v;_TA=*_T9;_TB=loc;_TC=msg;_TD=
# 405
Cyc_Tcdecl_merge_xdatatype_fields(_T4,_T5,0,0,0,_T6,_T7,_T8,_TA,_TB,_TC);return _TD;}}struct _tuple17{struct Cyc_Core_Opt*f0;struct Cyc_Core_Opt*f1;};
# 409
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Absyn_Datatypedecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Datatypedecl*_T2;int _T3;struct Cyc_Absyn_Datatypedecl*_T4;int _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct Cyc_Absyn_Datatypedecl*_T8;int _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_Absyn_Datatypedecl*_TB;int _TC;void**_TD;void**_TE;void**_TF;unsigned _T10;struct _fat_ptr*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_Absyn_Datatypedecl*_T14;int _T15;struct Cyc_Absyn_Datatypedecl*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Datatypedecl*_T18;struct Cyc_List_List*_T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;unsigned _T1C;struct _fat_ptr*_T1D;int _T1E;struct Cyc_Absyn_Datatypedecl*_T1F;enum Cyc_Absyn_Scope _T20;struct Cyc_Absyn_Datatypedecl*_T21;enum Cyc_Absyn_Scope _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;unsigned _T25;struct _fat_ptr*_T26;int _T27;struct _tuple17 _T28;struct Cyc_Absyn_Datatypedecl*_T29;struct Cyc_Absyn_Datatypedecl*_T2A;struct Cyc_Core_Opt*_T2B;struct Cyc_Core_Opt*_T2C;struct Cyc_Core_Opt*_T2D;void*_T2E;struct Cyc_Core_Opt*_T2F;void*_T30;struct Cyc_Absyn_Datatypedecl*_T31;struct Cyc_List_List*_T32;struct Cyc_Absyn_Datatypedecl*_T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Datatypedecl*_T35;int _T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;struct Cyc_Absyn_Datatypedecl*_T3A;struct Cyc_List_List*_T3B;struct Cyc_Absyn_Datatypedecl*_T3C;struct Cyc_List_List*_T3D;int*_T3E;int*_T3F;struct _fat_ptr _T40;struct _fat_ptr _T41;unsigned _T42;struct _fat_ptr*_T43;int _T44;struct Cyc_Absyn_Datatypedecl*_T45;struct Cyc_Absyn_Datatypedecl*_T46;struct Cyc_Absyn_Datatypedecl*_T47;struct Cyc_Absyn_Datatypedecl*_T48;struct Cyc_Core_Opt*_T49;struct Cyc_List_List*_T4A;void*_T4B;struct Cyc_Absyn_Datatypefield*_T4C;struct Cyc_List_List*_T4D;void*_T4E;struct Cyc_Absyn_Datatypefield*_T4F;struct Cyc_List_List*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct _fat_ptr*_T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;struct Cyc_String_pa_PrintArg_struct _T56;struct Cyc_String_pa_PrintArg_struct _T57;struct Cyc_String_pa_PrintArg_struct _T58;struct Cyc_List_List*_T59;void*_T5A;struct Cyc_Absyn_Datatypefield*_T5B;struct _tuple0*_T5C;struct _tuple0 _T5D;struct _fat_ptr*_T5E;void**_T5F;void**_T60;void**_T61;unsigned _T62;struct _fat_ptr*_T63;struct _fat_ptr _T64;struct _fat_ptr _T65;struct Cyc_String_pa_PrintArg_struct _T66;struct Cyc_String_pa_PrintArg_struct _T67;struct Cyc_String_pa_PrintArg_struct _T68;struct Cyc_List_List*_T69;void*_T6A;struct Cyc_Absyn_Datatypefield*_T6B;struct _tuple0*_T6C;struct _tuple0 _T6D;struct _fat_ptr*_T6E;void**_T6F;void**_T70;void**_T71;unsigned _T72;struct _fat_ptr*_T73;struct _fat_ptr _T74;struct _fat_ptr _T75;int _T76;enum Cyc_Absyn_Scope _T77;int _T78;struct Cyc_Absyn_Datatypedecl*_T79;enum Cyc_Absyn_Scope _T7A;int _T7B;struct Cyc_Absyn_Datatypedecl*_T7C;struct Cyc_Absyn_Datatypedecl*_T7D;struct Cyc_Absyn_Datatypedecl*_T7E;struct Cyc_Absyn_Datatypedecl*_T7F;struct Cyc_Absyn_Datatypedecl*_T80;_T0=d0;_T1=_T0->name;{
# 411
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T1);
struct _fat_ptr t=_tag_fat("datatype",sizeof(char),9U);
int res=1;_T2=d0;_T3=_T2->is_extensible;_T4=d1;_T5=_T4->is_extensible;
# 418
if(_T3==_T5)goto _TLA1;{struct Cyc_String_pa_PrintArg_struct _T81;_T81.tag=0;
# 420
_T81.f1=v;_T6=_T81;}{struct Cyc_String_pa_PrintArg_struct _T81=_T6;{struct Cyc_String_pa_PrintArg_struct _T82;_T82.tag=0;_T8=d0;_T9=_T8->is_extensible;_T82.f1=Cyc_Tcdecl_is_x2string(_T9);_T7=_T82;}{struct Cyc_String_pa_PrintArg_struct _T82=_T7;{struct Cyc_String_pa_PrintArg_struct _T83;_T83.tag=0;_TB=d1;_TC=_TB->is_extensible;_T83.f1=Cyc_Tcdecl_is_x2string(_TC);_TA=_T83;}{struct Cyc_String_pa_PrintArg_struct _T83=_TA;void*_T84[3];_TD=_T84 + 0;*_TD=& _T81;_TE=_T84 + 1;*_TE=& _T82;_TF=_T84 + 2;*_TF=& _T83;_T10=loc;_T11=msg;_T12=
# 419
_tag_fat("expected %s to be a %s instead of a %s",sizeof(char),39U);_T13=_tag_fat(_T84,sizeof(void*),3);Cyc_Tcdecl_merr(_T10,_T11,_T12,_T13);}}}
# 421
res=0;goto _TLA2;
# 423
_TLA1: _T14=d0;_T15=_T14->is_extensible;t=Cyc_Tcdecl_is_x2string(_T15);_TLA2: _T16=d0;_T17=_T16->tvs;_T18=d1;_T19=_T18->tvs;_T1A=t;_T1B=v;_T1C=loc;_T1D=msg;_T1E=
# 426
Cyc_Tcdecl_check_tvs(_T17,_T19,_T1A,_T1B,_T1C,_T1D);if(_T1E)goto _TLA3;else{goto _TLA5;}_TLA5: return 0;_TLA3: _T1F=d0;_T20=_T1F->sc;_T21=d1;_T22=_T21->sc;_T23=t;_T24=v;_T25=loc;_T26=msg;{
# 429
struct _tuple11 _T81=Cyc_Tcdecl_merge_scope(_T20,_T22,_T23,_T24,_T25,_T26,0);int _T82;enum Cyc_Absyn_Scope _T83;_T83=_T81.f0;_T82=_T81.f1;{enum Cyc_Absyn_Scope new_scope=_T83;int res_scope=_T82;_T27=res_scope;
if(_T27)goto _TLA6;else{goto _TLA8;}_TLA8: res=0;goto _TLA7;_TLA6: _TLA7: {
# 432
struct Cyc_Absyn_Datatypedecl*d2;{struct _tuple17 _T84;_T29=d0;
# 435
_T84.f0=_T29->fields;_T2A=d1;_T84.f1=_T2A->fields;_T28=_T84;}{struct _tuple17 _T84=_T28;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;_T2B=_T84.f1;if(_T2B!=0)goto _TLA9;
d2=d0;goto _LL3;_TLA9: _T2C=_T84.f0;if(_T2C!=0)goto _TLAB;
d2=d1;goto _LL3;_TLAB: _T2D=_T84.f0;{struct Cyc_Core_Opt _T87=*_T2D;_T2E=_T87.v;_T86=(struct Cyc_List_List*)_T2E;}_T2F=_T84.f1;{struct Cyc_Core_Opt _T87=*_T2F;_T30=_T87.v;_T85=(struct Cyc_List_List*)_T30;}{struct Cyc_List_List*f0s=_T86;struct Cyc_List_List*f1s=_T85;_T31=d0;_T32=_T31->tvs;_T33=d1;_T34=_T33->tvs;{
# 449 "tcdecl.cyc"
struct Cyc_List_List*inst=Cyc_Tcdecl_build_tvs_map(_T32,_T34);_T35=d0;_T36=_T35->is_extensible;
# 452
if(!_T36)goto _TLAD;{
# 456
int incl=1;_T37=f0s;_T38=f1s;_T39=inst;_T3A=d0;_T3B=_T3A->tvs;_T3C=d1;_T3D=_T3C->tvs;_T3E=& res;_T3F=& incl;_T40=t;_T41=v;_T42=loc;_T43=msg;{
struct Cyc_List_List*f2s=Cyc_Tcdecl_merge_xdatatype_fields(_T37,_T38,_T39,_T3B,_T3D,_T3E,_T3F,_T40,_T41,_T42,_T43);_T44=incl;
# 460
if(!_T44)goto _TLAF;
d2=d0;goto _TLB0;
# 463
_TLAF:{struct Cyc_Absyn_Datatypedecl*_T87=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T46=d0;*_T87=*_T46;_T45=(struct Cyc_Absyn_Datatypedecl*)_T87;}d2=_T45;_T47=d2;
_T47->sc=new_scope;_T48=d2;{struct Cyc_Core_Opt*_T87=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T87->v=f2s;_T49=(struct Cyc_Core_Opt*)_T87;}_T48->fields=_T49;_TLB0:;}}goto _TLAE;
# 469
_TLAD: _TLB4: if(f0s!=0)goto _TLB5;else{goto _TLB3;}_TLB5: if(f1s!=0)goto _TLB2;else{goto _TLB3;}
_TLB2: _T4A=f0s;_T4B=_T4A->hd;_T4C=(struct Cyc_Absyn_Datatypefield*)_T4B;_T4D=f1s;_T4E=_T4D->hd;_T4F=(struct Cyc_Absyn_Datatypefield*)_T4E;_T50=inst;_T51=t;_T52=v;_T53=msg;Cyc_Tcdecl_merge_datatypefield(_T4C,_T4F,_T50,_T51,_T52,_T53);_T54=f0s;
# 469
f0s=_T54->tl;_T55=f1s;f1s=_T55->tl;goto _TLB4;_TLB3:
# 471
 if(f0s==0)goto _TLB6;{struct Cyc_String_pa_PrintArg_struct _T87;_T87.tag=0;
_T87.f1=t;_T56=_T87;}{struct Cyc_String_pa_PrintArg_struct _T87=_T56;{struct Cyc_String_pa_PrintArg_struct _T88;_T88.tag=0;_T88.f1=v;_T57=_T88;}{struct Cyc_String_pa_PrintArg_struct _T88=_T57;{struct Cyc_String_pa_PrintArg_struct _T89;_T89.tag=0;_T59=f0s;_T5A=_T59->hd;_T5B=(struct Cyc_Absyn_Datatypefield*)_T5A;_T5C=_T5B->name;_T5D=*_T5C;_T5E=_T5D.f1;_T89.f1=*_T5E;_T58=_T89;}{struct Cyc_String_pa_PrintArg_struct _T89=_T58;void*_T8A[3];_T5F=_T8A + 0;*_T5F=& _T87;_T60=_T8A + 1;*_T60=& _T88;_T61=_T8A + 2;*_T61=& _T89;_T62=loc;_T63=msg;_T64=_tag_fat("%s %s is missing field %s",sizeof(char),26U);_T65=_tag_fat(_T8A,sizeof(void*),3);Cyc_Tcdecl_merr(_T62,_T63,_T64,_T65);}}}
res=0;goto _TLB7;_TLB6: _TLB7:
# 475
 if(f1s==0)goto _TLB8;{struct Cyc_String_pa_PrintArg_struct _T87;_T87.tag=0;
_T87.f1=t;_T66=_T87;}{struct Cyc_String_pa_PrintArg_struct _T87=_T66;{struct Cyc_String_pa_PrintArg_struct _T88;_T88.tag=0;_T88.f1=v;_T67=_T88;}{struct Cyc_String_pa_PrintArg_struct _T88=_T67;{struct Cyc_String_pa_PrintArg_struct _T89;_T89.tag=0;_T69=f1s;_T6A=_T69->hd;_T6B=(struct Cyc_Absyn_Datatypefield*)_T6A;_T6C=_T6B->name;_T6D=*_T6C;_T6E=_T6D.f1;_T89.f1=*_T6E;_T68=_T89;}{struct Cyc_String_pa_PrintArg_struct _T89=_T68;void*_T8A[3];_T6F=_T8A + 0;*_T6F=& _T87;_T70=_T8A + 1;*_T70=& _T88;_T71=_T8A + 2;*_T71=& _T89;_T72=loc;_T73=msg;_T74=_tag_fat("%s %s has extra field %s",sizeof(char),25U);_T75=_tag_fat(_T8A,sizeof(void*),3);Cyc_Tcdecl_merr(_T72,_T73,_T74,_T75);}}}
res=0;goto _TLB9;_TLB8: _TLB9:
# 479
 d2=d0;_TLAE: goto _LL3;}}_LL3:;}_T76=res;
# 484
if(_T76)goto _TLBA;else{goto _TLBC;}_TLBC: return 0;_TLBA: _T77=new_scope;_T78=(int)_T77;_T79=d2;_T7A=_T79->sc;_T7B=(int)_T7A;
if(_T78!=_T7B)goto _TLBD;_T7C=d2;return _T7C;_TLBD:{struct Cyc_Absyn_Datatypedecl*_T84=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T7E=d2;
*_T84=*_T7E;_T7D=(struct Cyc_Absyn_Datatypedecl*)_T84;}d2=_T7D;_T7F=d2;
_T7F->sc=new_scope;_T80=d2;
return _T80;}}}}}
# 491
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Absyn_Enumdecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Enumdecl*_T2;enum Cyc_Absyn_Scope _T3;struct Cyc_Absyn_Enumdecl*_T4;enum Cyc_Absyn_Scope _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;unsigned _T8;struct _fat_ptr*_T9;int _TA;struct _tuple17 _TB;struct Cyc_Absyn_Enumdecl*_TC;struct Cyc_Absyn_Enumdecl*_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Core_Opt*_TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_Core_Opt*_T12;void*_T13;struct Cyc_List_List*_T14;void*_T15;struct _tuple0*_T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple0*_T19;int _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct _fat_ptr*_T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct _fat_ptr*_T1F;void**_T20;void**_T21;void**_T22;unsigned _T23;struct _fat_ptr*_T24;struct _fat_ptr _T25;struct _fat_ptr _T26;struct Cyc_Absyn_Exp*_T27;unsigned _T28;struct Cyc_Absyn_Exp*_T29;unsigned _T2A;struct Cyc_String_pa_PrintArg_struct _T2B;struct Cyc_String_pa_PrintArg_struct _T2C;struct _fat_ptr*_T2D;void**_T2E;void**_T2F;unsigned _T30;struct _fat_ptr*_T31;struct _fat_ptr _T32;struct _fat_ptr _T33;struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;struct Cyc_String_pa_PrintArg_struct _T36;void**_T37;unsigned _T38;struct _fat_ptr*_T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;int _T3C;struct Cyc_Absyn_Enumdecl*_T3D;enum Cyc_Absyn_Scope _T3E;int _T3F;enum Cyc_Absyn_Scope _T40;int _T41;struct Cyc_Absyn_Enumdecl*_T42;struct Cyc_Absyn_Enumdecl*_T43;struct Cyc_Absyn_Enumdecl*_T44;struct Cyc_Absyn_Enumdecl*_T45;struct Cyc_Absyn_Enumdecl*_T46;_T0=d0;_T1=_T0->name;{
# 493
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T1);
int res=1;_T2=d0;_T3=_T2->sc;_T4=d1;_T5=_T4->sc;_T6=
# 497
_tag_fat("enum",sizeof(char),5U);_T7=v;_T8=loc;_T9=msg;{struct _tuple11 _T47=Cyc_Tcdecl_merge_scope(_T3,_T5,_T6,_T7,_T8,_T9,1);int _T48;enum Cyc_Absyn_Scope _T49;_T49=_T47.f0;_T48=_T47.f1;{enum Cyc_Absyn_Scope new_scope=_T49;int res_scope=_T48;_TA=res_scope;
if(_TA)goto _TLBF;else{goto _TLC1;}_TLC1: res=0;goto _TLC0;_TLBF: _TLC0: {
# 500
struct Cyc_Absyn_Enumdecl*d2;{struct _tuple17 _T4A;_TC=d0;
# 503
_T4A.f0=_TC->fields;_TD=d1;_T4A.f1=_TD->fields;_TB=_T4A;}{struct _tuple17 _T4A=_TB;struct Cyc_List_List*_T4B;struct Cyc_List_List*_T4C;_TE=_T4A.f1;if(_TE!=0)goto _TLC2;
d2=d0;goto _LL3;_TLC2: _TF=_T4A.f0;if(_TF!=0)goto _TLC4;
d2=d1;goto _LL3;_TLC4: _T10=_T4A.f0;{struct Cyc_Core_Opt _T4D=*_T10;_T11=_T4D.v;_T4C=(struct Cyc_List_List*)_T11;}_T12=_T4A.f1;{struct Cyc_Core_Opt _T4D=*_T12;_T13=_T4D.v;_T4B=(struct Cyc_List_List*)_T13;}{struct Cyc_List_List*f0s=_T4C;struct Cyc_List_List*f1s=_T4B;
# 508
_TLC9: if(f0s!=0)goto _TLCA;else{goto _TLC8;}_TLCA: if(f1s!=0)goto _TLC7;else{goto _TLC8;}
_TLC7: _T14=f0s;_T15=_T14->hd;{struct Cyc_Absyn_Enumfield*_T4D=(struct Cyc_Absyn_Enumfield*)_T15;unsigned _T4E;struct Cyc_Absyn_Exp*_T4F;struct _fat_ptr*_T50;{struct Cyc_Absyn_Enumfield _T51=*_T4D;_T16=_T51.name;{struct _tuple0 _T52=*_T16;_T50=_T52.f1;}_T4F=_T51.tag;_T4E=_T51.loc;}{struct _fat_ptr*fname0=_T50;struct Cyc_Absyn_Exp*tag0=_T4F;unsigned loc0=_T4E;_T17=f1s;_T18=_T17->hd;{
struct Cyc_Absyn_Enumfield*_T51=(struct Cyc_Absyn_Enumfield*)_T18;unsigned _T52;struct Cyc_Absyn_Exp*_T53;struct _fat_ptr*_T54;{struct Cyc_Absyn_Enumfield _T55=*_T51;_T19=_T55.name;{struct _tuple0 _T56=*_T19;_T54=_T56.f1;}_T53=_T55.tag;_T52=_T55.loc;}{struct _fat_ptr*fname1=_T54;struct Cyc_Absyn_Exp*tag1=_T53;unsigned loc1=_T52;_T1A=
# 512
Cyc_strptrcmp(fname0,fname1);if(_T1A==0)goto _TLCB;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;
_T55.f1=v;_T1B=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T1B;{struct Cyc_String_pa_PrintArg_struct _T56;_T56.tag=0;_T1D=fname0;_T56.f1=*_T1D;_T1C=_T56;}{struct Cyc_String_pa_PrintArg_struct _T56=_T1C;{struct Cyc_String_pa_PrintArg_struct _T57;_T57.tag=0;_T1F=fname1;_T57.f1=*_T1F;_T1E=_T57;}{struct Cyc_String_pa_PrintArg_struct _T57=_T1E;void*_T58[3];_T20=_T58 + 0;*_T20=& _T55;_T21=_T58 + 1;*_T21=& _T56;_T22=_T58 + 2;*_T22=& _T57;_T23=loc;_T24=msg;_T25=_tag_fat("enum %s: field name mismatch %s != %s",sizeof(char),38U);_T26=_tag_fat(_T58,sizeof(void*),3);Cyc_Tcdecl_merr(_T23,_T24,_T25,_T26);}}}
res=0;goto _TLCC;_TLCB: _TLCC: _T27=
# 516
_check_null(tag0);_T28=Cyc_Tcdecl_get_uint_const_value(_T27);_T29=_check_null(tag1);_T2A=Cyc_Tcdecl_get_uint_const_value(_T29);if(_T28==_T2A)goto _TLCD;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;
_T55.f1=v;_T2B=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T2B;{struct Cyc_String_pa_PrintArg_struct _T56;_T56.tag=0;_T2D=fname1;_T56.f1=*_T2D;_T2C=_T56;}{struct Cyc_String_pa_PrintArg_struct _T56=_T2C;void*_T57[2];_T2E=_T57 + 0;*_T2E=& _T55;_T2F=_T57 + 1;*_T2F=& _T56;_T30=loc;_T31=msg;_T32=_tag_fat("enum %s, field %s, value mismatch",sizeof(char),34U);_T33=_tag_fat(_T57,sizeof(void*),2);Cyc_Tcdecl_merr(_T30,_T31,_T32,_T33);}}
res=0;goto _TLCE;_TLCD: _TLCE:;}}}}_T34=f0s;
# 508
f0s=_T34->tl;_T35=f1s;f1s=_T35->tl;goto _TLC9;_TLC8:
# 521
 if(f0s!=0)goto _TLD1;else{goto _TLD2;}_TLD2: if(f1s!=0)goto _TLD1;else{goto _TLCF;}
_TLD1:{struct Cyc_String_pa_PrintArg_struct _T4D;_T4D.tag=0;_T4D.f1=v;_T36=_T4D;}{struct Cyc_String_pa_PrintArg_struct _T4D=_T36;void*_T4E[1];_T37=_T4E + 0;*_T37=& _T4D;_T38=loc;_T39=msg;_T3A=_tag_fat("enum %s declarations: number of fields differs",sizeof(char),47U);_T3B=_tag_fat(_T4E,sizeof(void*),1);Cyc_Tcdecl_merr(_T38,_T39,_T3A,_T3B);}
res=0;goto _TLD0;_TLCF: _TLD0:
# 525
 d2=d0;goto _LL3;}_LL3:;}_T3C=res;
# 529
if(_T3C)goto _TLD3;else{goto _TLD5;}_TLD5: return 0;_TLD3: _T3D=d2;_T3E=_T3D->sc;_T3F=(int)_T3E;_T40=new_scope;_T41=(int)_T40;
if(_T3F!=_T41)goto _TLD6;_T42=d2;return _T42;_TLD6:{struct Cyc_Absyn_Enumdecl*_T4A=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T44=d2;
*_T4A=*_T44;_T43=(struct Cyc_Absyn_Enumdecl*)_T4A;}d2=_T43;_T45=d2;
_T45->sc=new_scope;_T46=d2;
return _T46;}}}}}
# 536
static struct _tuple11 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _fat_ptr t,struct _fat_ptr v,unsigned loc,struct _fat_ptr*msg){int _T0;int _T1;unsigned long _T2;unsigned long _T3;unsigned long _T4;unsigned long _T5;unsigned long _T6;unsigned long _T7;unsigned long _T8;struct _fat_ptr _T9;unsigned long _TA;unsigned long _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct Cyc_String_pa_PrintArg_struct _TF;void**_T10;void**_T11;void**_T12;void**_T13;unsigned _T14;struct _fat_ptr*_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_String_pa_PrintArg_struct _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;void**_T1C;void**_T1D;void**_T1E;void**_T1F;unsigned _T20;struct _fat_ptr*_T21;struct _fat_ptr _T22;struct _fat_ptr _T23;int _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc_String_pa_PrintArg_struct _T26;void**_T27;void**_T28;unsigned _T29;struct _fat_ptr*_T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;int _T2D;struct Cyc_String_pa_PrintArg_struct _T2E;struct Cyc_String_pa_PrintArg_struct _T2F;void**_T30;void**_T31;unsigned _T32;struct _fat_ptr*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct Cyc___cycFILE*_T36;struct _fat_ptr _T37;struct _fat_ptr _T38;struct Cyc_String_pa_PrintArg_struct _T39;struct Cyc_List_List*_T3A;void*_T3B;void**_T3C;struct Cyc___cycFILE*_T3D;struct _fat_ptr _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*_T40;struct Cyc___cycFILE*_T41;struct _fat_ptr _T42;struct _fat_ptr _T43;struct Cyc_String_pa_PrintArg_struct _T44;struct Cyc_List_List*_T45;void*_T46;void**_T47;struct Cyc___cycFILE*_T48;struct _fat_ptr _T49;struct _fat_ptr _T4A;struct Cyc_List_List*_T4B;struct Cyc___cycFILE*_T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;struct _tuple11 _T4F;
# 540
int res=1;
# 543
struct _tuple11 _T50=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);int _T51;enum Cyc_Absyn_Scope _T52;_T52=_T50.f0;_T51=_T50.f1;{enum Cyc_Absyn_Scope new_scope=_T52;int res_scope=_T51;_T0=res_scope;
if(_T0)goto _TLD8;else{goto _TLDA;}_TLDA: res=0;goto _TLD9;_TLD8: _TLD9: _T1=
# 547
Cyc_Tcdecl_check_type(loc,t0,t1);if(_T1)goto _TLDB;else{goto _TLDD;}
_TLDD:{struct _fat_ptr s1=Cyc_Absynpp_typ2string(t0);
struct _fat_ptr s2=Cyc_Absynpp_typ2string(t1);_T2=
Cyc_strlen(s1);_T3=Cyc_strlen(s2);_T4=_T2 + _T3;_T5=Cyc_strlen(t);_T6=_T4 + _T5;_T7=Cyc_strlen(v);_T8=_T6 + _T7;_T9=
_tag_fat("  was declared  instead of ",sizeof(char),28U);_TA=Cyc_strlen(_T9);_TB=_T8 + _TA;
# 550
if(_TB <= 70U)goto _TLDE;{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;
# 553
_T53.f1=t;_TC=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_TC;{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T54.f1=v;_TD=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_TD;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;_T55.f1=s2;_TE=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_TE;{struct Cyc_String_pa_PrintArg_struct _T56;_T56.tag=0;_T56.f1=s1;_TF=_T56;}{struct Cyc_String_pa_PrintArg_struct _T56=_TF;void*_T57[4];_T10=_T57 + 0;*_T10=& _T53;_T11=_T57 + 1;*_T11=& _T54;_T12=_T57 + 2;*_T12=& _T55;_T13=_T57 + 3;*_T13=& _T56;_T14=loc;_T15=msg;_T16=
# 552
_tag_fat("%s %s was declared \n\t%s\n instead of \n\t%s",sizeof(char),41U);_T17=_tag_fat(_T57,sizeof(void*),4);Cyc_Tcdecl_merr(_T14,_T15,_T16,_T17);}}}}goto _TLDF;
# 555
_TLDE:{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;
_T53.f1=t;_T18=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_T18;{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T54.f1=v;_T19=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T19;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;_T55.f1=s2;_T1A=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T1A;{struct Cyc_String_pa_PrintArg_struct _T56;_T56.tag=0;_T56.f1=s1;_T1B=_T56;}{struct Cyc_String_pa_PrintArg_struct _T56=_T1B;void*_T57[4];_T1C=_T57 + 0;*_T1C=& _T53;_T1D=_T57 + 1;*_T1D=& _T54;_T1E=_T57 + 2;*_T1E=& _T55;_T1F=_T57 + 3;*_T1F=& _T56;_T20=loc;_T21=msg;_T22=
# 555
_tag_fat("%s %s was declared %s instead of %s",sizeof(char),36U);_T23=_tag_fat(_T57,sizeof(void*),4);Cyc_Tcdecl_merr(_T20,_T21,_T22,_T23);}}}}_TLDF:
# 557
 Cyc_Unify_explain_failure();
res=0;}goto _TLDC;_TLDB: _TLDC: _T24=
# 562
Cyc_Absyn_equal_tqual(tq0,tq1);if(_T24)goto _TLE0;else{goto _TLE2;}
_TLE2:{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;_T53.f1=t;_T25=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_T25;{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T54.f1=v;_T26=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T26;void*_T55[2];_T27=_T55 + 0;*_T27=& _T53;_T28=_T55 + 1;*_T28=& _T54;_T29=loc;_T2A=msg;_T2B=_tag_fat("%s %s has different type qualifiers",sizeof(char),36U);_T2C=_tag_fat(_T55,sizeof(void*),2);Cyc_Tcdecl_merr(_T29,_T2A,_T2B,_T2C);}}
res=0;goto _TLE1;_TLE0: _TLE1: _T2D=
# 568
Cyc_Atts_same_atts(atts0,atts1);if(_T2D)goto _TLE3;else{goto _TLE5;}
_TLE5:{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;_T53.f1=t;_T2E=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_T2E;{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T54.f1=v;_T2F=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T2F;void*_T55[2];_T30=_T55 + 0;*_T30=& _T53;_T31=_T55 + 1;*_T31=& _T54;_T32=loc;_T33=msg;_T34=_tag_fat("%s %s has different attributes",sizeof(char),31U);_T35=_tag_fat(_T55,sizeof(void*),2);Cyc_Tcdecl_merr(_T32,_T33,_T34,_T35);}}_T36=Cyc_stderr;_T37=
_tag_fat("\tprevious attributes: ",sizeof(char),23U);_T38=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T36,_T37,_T38);
_TLE9: if(atts0!=0)goto _TLE7;else{goto _TLE8;}
_TLE7:{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;_T3A=atts0;_T3B=_T3A->hd;_T53.f1=Cyc_Absynpp_attribute2string(_T3B);_T39=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_T39;void*_T54[1];_T3C=_T54 + 0;*_T3C=& _T53;_T3D=Cyc_stderr;_T3E=_tag_fat("%s ",sizeof(char),4U);_T3F=_tag_fat(_T54,sizeof(void*),1);Cyc_fprintf(_T3D,_T3E,_T3F);}_T40=atts0;
# 571
atts0=_T40->tl;goto _TLE9;_TLE8: _T41=Cyc_stderr;_T42=
# 573
_tag_fat("\n\tcurrent attributes: ",sizeof(char),23U);_T43=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T41,_T42,_T43);
_TLED: if(atts1!=0)goto _TLEB;else{goto _TLEC;}
_TLEB:{struct Cyc_String_pa_PrintArg_struct _T53;_T53.tag=0;_T45=atts1;_T46=_T45->hd;_T53.f1=Cyc_Absynpp_attribute2string(_T46);_T44=_T53;}{struct Cyc_String_pa_PrintArg_struct _T53=_T44;void*_T54[1];_T47=_T54 + 0;*_T47=& _T53;_T48=Cyc_stderr;_T49=_tag_fat("%s ",sizeof(char),4U);_T4A=_tag_fat(_T54,sizeof(void*),1);Cyc_fprintf(_T48,_T49,_T4A);}_T4B=atts1;
# 574
atts1=_T4B->tl;goto _TLED;_TLEC: _T4C=Cyc_stderr;_T4D=
# 576
_tag_fat("\n",sizeof(char),2U);_T4E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4C,_T4D,_T4E);
res=0;goto _TLE4;_TLE3: _TLE4:{struct _tuple11 _T53;
# 580
_T53.f0=new_scope;_T53.f1=res;_T4F=_T53;}return _T4F;}}
# 583
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Absyn_Vardecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Vardecl*_T2;enum Cyc_Absyn_Scope _T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Tqual _T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_Vardecl*_TA;enum Cyc_Absyn_Scope _TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_Absyn_Vardecl*_TE;struct Cyc_Absyn_Tqual _TF;struct Cyc_Absyn_Vardecl*_T10;struct Cyc_List_List*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;unsigned _T14;struct _fat_ptr*_T15;int _T16;struct Cyc_Absyn_Vardecl*_T17;enum Cyc_Absyn_Scope _T18;int _T19;enum Cyc_Absyn_Scope _T1A;int _T1B;struct Cyc_Absyn_Vardecl*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Absyn_Vardecl*_T1E;struct Cyc_Absyn_Vardecl*_T1F;struct Cyc_Absyn_Vardecl*_T20;_T0=d0;_T1=_T0->name;{
# 585
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T1);_T2=d0;_T3=_T2->sc;_T4=d0;_T5=_T4->type;_T6=d0;_T7=_T6->tq;_T8=d0;_T9=_T8->attributes;_TA=d1;_TB=_TA->sc;_TC=d1;_TD=_TC->type;_TE=d1;_TF=_TE->tq;_T10=d1;_T11=_T10->attributes;_T12=
# 591
_tag_fat("variable",sizeof(char),9U);_T13=v;_T14=loc;_T15=msg;{
# 588
struct _tuple11 _T21=
Cyc_Tcdecl_check_var_or_fn_decl(_T3,_T5,_T7,_T9,_TB,_TD,_TF,_T11,_T12,_T13,_T14,_T15);int _T22;enum Cyc_Absyn_Scope _T23;_T23=_T21.f0;_T22=_T21.f1;{enum Cyc_Absyn_Scope new_scope=_T23;int res=_T22;_T16=res;
# 593
if(_T16)goto _TLEE;else{goto _TLF0;}_TLF0: return 0;_TLEE: _T17=d0;_T18=_T17->sc;_T19=(int)_T18;_T1A=new_scope;_T1B=(int)_T1A;
if(_T19!=_T1B)goto _TLF1;_T1C=d0;return _T1C;_TLF1: {
struct Cyc_Absyn_Vardecl*d2;d2=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T1D=d2;_T1E=d0;*_T1D=*_T1E;_T1F=d2;
_T1F->sc=new_scope;_T20=d2;
return _T20;}}}}}
# 600
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Absyn_Typedefdecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Typedefdecl*_T2;struct Cyc_List_List*_T3;struct Cyc_Absyn_Typedefdecl*_T4;struct Cyc_List_List*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;unsigned _T8;struct _fat_ptr*_T9;int _TA;struct Cyc_Absyn_Typedefdecl*_TB;struct Cyc_List_List*_TC;struct Cyc_Absyn_Typedefdecl*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Typedefdecl*_TF;void*_T10;struct Cyc_Absyn_Typedefdecl*_T11;void*_T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_Typedefdecl*_T14;void*_T15;unsigned _T16;struct Cyc_Absyn_Typedefdecl*_T17;void*_T18;void*_T19;void*_T1A;int _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct Cyc_Absyn_Typedefdecl*_T1F;void*_T20;void*_T21;void**_T22;void**_T23;void**_T24;unsigned _T25;struct _fat_ptr*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc_Absyn_Typedefdecl*_T29;_T0=d0;_T1=_T0->name;{
# 602
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T1);_T2=d0;_T3=_T2->tvs;_T4=d1;_T5=_T4->tvs;_T6=
# 605
_tag_fat("typedef",sizeof(char),8U);_T7=v;_T8=loc;_T9=msg;_TA=Cyc_Tcdecl_check_tvs(_T3,_T5,_T6,_T7,_T8,_T9);if(_TA)goto _TLF3;else{goto _TLF5;}_TLF5: return 0;_TLF3: _TB=d0;_TC=_TB->tvs;_TD=d1;_TE=_TD->tvs;{
# 608
struct Cyc_List_List*inst=Cyc_Tcdecl_build_tvs_map(_TC,_TE);_TF=d0;_T10=_TF->defn;
# 611
if(_T10==0)goto _TLF6;_T11=d1;_T12=_T11->defn;if(_T12==0)goto _TLF6;_T13=inst;_T14=d1;_T15=_T14->defn;{
void*subst_defn1=Cyc_Tcutil_substitute(_T13,_T15);_T16=loc;_T17=d0;_T18=_T17->defn;_T19=
_check_null(_T18);_T1A=subst_defn1;_T1B=Cyc_Tcdecl_check_type(_T16,_T19,_T1A);if(_T1B)goto _TLF8;else{goto _TLFA;}
_TLFA:{struct Cyc_String_pa_PrintArg_struct _T2A;_T2A.tag=0;
_T2A.f1=v;_T1C=_T2A;}{struct Cyc_String_pa_PrintArg_struct _T2A=_T1C;{struct Cyc_String_pa_PrintArg_struct _T2B;_T2B.tag=0;_T2B.f1=Cyc_Absynpp_typ2string(subst_defn1);_T1D=_T2B;}{struct Cyc_String_pa_PrintArg_struct _T2B=_T1D;{struct Cyc_String_pa_PrintArg_struct _T2C;_T2C.tag=0;_T1F=d0;_T20=_T1F->defn;_T21=_check_null(_T20);_T2C.f1=Cyc_Absynpp_typ2string(_T21);_T1E=_T2C;}{struct Cyc_String_pa_PrintArg_struct _T2C=_T1E;void*_T2D[3];_T22=_T2D + 0;*_T22=& _T2A;_T23=_T2D + 1;*_T23=& _T2B;_T24=_T2D + 2;*_T24=& _T2C;_T25=loc;_T26=msg;_T27=
# 614
_tag_fat("typedef %s does not refer to the same type: %s != %s",sizeof(char),53U);_T28=_tag_fat(_T2D,sizeof(void*),3);Cyc_Tcdecl_merr(_T25,_T26,_T27,_T28);}}}
# 616
return 0;_TLF8:;}goto _TLF7;_TLF6: _TLF7: _T29=d0;
# 619
return _T29;}}}struct _tuple18{void*f0;void*f1;};
# 622
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned loc,struct _fat_ptr*msg){struct _tuple18 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TB;void*_TC;void*_TD;void*_TE;struct Cyc_Absyn_Vardecl*_TF;enum Cyc_Absyn_Scope _T10;struct Cyc_Absyn_Vardecl*_T11;void*_T12;struct Cyc_Absyn_Vardecl*_T13;struct Cyc_Absyn_Tqual _T14;struct Cyc_Absyn_Vardecl*_T15;struct Cyc_List_List*_T16;struct Cyc_Absyn_Fndecl*_T17;enum Cyc_Absyn_Scope _T18;struct Cyc_Absyn_Fndecl*_T19;void*_T1A;void*_T1B;struct Cyc_Absyn_Tqual _T1C;struct Cyc_Absyn_Fndecl*_T1D;struct Cyc_Absyn_FnInfo _T1E;struct Cyc_List_List*_T1F;struct _fat_ptr _T20;struct Cyc_Absyn_Vardecl*_T21;struct _tuple0*_T22;struct _fat_ptr _T23;unsigned _T24;struct _fat_ptr*_T25;int _T26;struct Cyc_Absyn_Vardecl*_T27;void*_T28;void*_T29;int*_T2A;unsigned _T2B;void*_T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct Cyc_Absyn_Fndecl*_T2E;struct _tuple0*_T2F;void**_T30;unsigned _T31;struct _fat_ptr*_T32;struct _fat_ptr _T33;struct _fat_ptr _T34;void*_T35;void*_T36;struct Cyc_Absyn_Fndecl*_T37;enum Cyc_Absyn_Scope _T38;struct Cyc_Absyn_Fndecl*_T39;void*_T3A;void*_T3B;struct Cyc_Absyn_Tqual _T3C;struct Cyc_Absyn_Fndecl*_T3D;struct Cyc_Absyn_FnInfo _T3E;struct Cyc_List_List*_T3F;struct Cyc_Absyn_Vardecl*_T40;enum Cyc_Absyn_Scope _T41;struct Cyc_Absyn_Vardecl*_T42;void*_T43;struct Cyc_Absyn_Vardecl*_T44;struct Cyc_Absyn_Tqual _T45;struct Cyc_Absyn_Vardecl*_T46;struct Cyc_List_List*_T47;struct _fat_ptr _T48;struct Cyc_Absyn_Fndecl*_T49;struct _tuple0*_T4A;struct _fat_ptr _T4B;unsigned _T4C;struct _fat_ptr*_T4D;int _T4E;struct Cyc_Absyn_Vardecl*_T4F;void*_T50;{struct _tuple18 _T51;
# 624
_T51.f0=b0;_T51.f1=b1;_T0=_T51;}{struct _tuple18 _T51=_T0;struct Cyc_Absyn_Fndecl*_T52;struct Cyc_Absyn_Vardecl*_T53;struct Cyc_Absyn_Vardecl*_T54;_T1=_T51.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 1: _T4=_T51.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 1: _T7=_T51.f0;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T7;_T54=_T55->f1;}_T8=_T51.f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T8;_T53=_T55->f1;}{struct Cyc_Absyn_Vardecl*d0=_T54;struct Cyc_Absyn_Vardecl*d1=_T53;
# 626
struct Cyc_Absyn_Vardecl*d2=Cyc_Tcdecl_merge_vardecl(d0,d1,loc,msg);
if(d2!=0)goto _TLFD;return 0;_TLFD:
 if(d2!=d0)goto _TLFF;_T9=b0;return _T9;_TLFF:
 if(d2!=d1)goto _TL101;_TA=b1;return _TA;_TL101:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_T55->tag=1;
_T55->f1=d2;_TB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T55;}_TC=(void*)_TB;return _TC;}case 2: _TD=_T51.f0;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TD;_T54=_T55->f1;}_TE=_T51.f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_TE;_T52=_T55->f1;}{struct Cyc_Absyn_Vardecl*d0=_T54;struct Cyc_Absyn_Fndecl*d1=_T52;_TF=d0;_T10=_TF->sc;_T11=d0;_T12=_T11->type;_T13=d0;_T14=_T13->tq;_T15=d0;_T16=_T15->attributes;_T17=d1;_T18=_T17->sc;_T19=d1;_T1A=_T19->cached_type;_T1B=
# 633
_check_null(_T1A);_T1C=Cyc_Absyn_empty_tqual(0U);_T1D=d1;_T1E=_T1D->i;_T1F=_T1E.attributes;_T20=
_tag_fat("function",sizeof(char),9U);_T21=d0;_T22=_T21->name;_T23=
Cyc_Absynpp_qvar2string(_T22);_T24=loc;_T25=msg;{
# 632
struct _tuple11 _T55=Cyc_Tcdecl_check_var_or_fn_decl(_T10,_T12,_T14,_T16,_T18,_T1B,_T1C,_T1F,_T20,_T23,_T24,_T25);int _T56;_T56=_T55.f1;{int res=_T56;_T26=res;
# 636
if(_T26)goto _TL103;else{goto _TL105;}_TL105: return 0;_TL103: _T27=d0;
_T27->is_proto=1;_T28=b1;
return _T28;}}}default: goto _LL9;};case 2: _T29=_T51.f1;_T2A=(int*)_T29;_T2B=*_T2A;switch(_T2B){case 2: _T2C=_T51.f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T2C;_T52=_T55->f1;}{struct Cyc_Absyn_Fndecl*d1=_T52;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;_T2E=d1;_T2F=_T2E->name;
# 640
_T55.f1=Cyc_Absynpp_qvar2string(_T2F);_T2D=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T2D;void*_T56[1];_T30=_T56 + 0;*_T30=& _T55;_T31=loc;_T32=msg;_T33=_tag_fat("redefinition of function %s",sizeof(char),28U);_T34=_tag_fat(_T56,sizeof(void*),1);Cyc_Tcdecl_merr(_T31,_T32,_T33,_T34);}
return 0;}case 1: _T35=_T51.f0;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T35;_T52=_T55->f1;}_T36=_T51.f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T36;_T54=_T55->f1;}{struct Cyc_Absyn_Fndecl*d0=_T52;struct Cyc_Absyn_Vardecl*d1=_T54;_T37=d0;_T38=_T37->sc;_T39=d0;_T3A=_T39->cached_type;_T3B=
# 643
_check_null(_T3A);_T3C=Cyc_Absyn_empty_tqual(0U);_T3D=d0;_T3E=_T3D->i;_T3F=_T3E.attributes;_T40=d1;_T41=_T40->sc;_T42=d1;_T43=_T42->type;_T44=d1;_T45=_T44->tq;_T46=d1;_T47=_T46->attributes;_T48=
# 645
_tag_fat("variable",sizeof(char),9U);_T49=d0;_T4A=_T49->name;_T4B=
Cyc_Absynpp_qvar2string(_T4A);_T4C=loc;_T4D=msg;{
# 643
struct _tuple11 _T55=Cyc_Tcdecl_check_var_or_fn_decl(_T38,_T3B,_T3C,_T3F,_T41,_T43,_T45,_T47,_T48,_T4B,_T4C,_T4D);int _T56;_T56=_T55.f1;{int res=_T56;_T4E=res;
# 647
if(_T4E)goto _TL107;else{goto _TL109;}_TL109: return 0;_TL107: _T4F=d1;
_T4F->is_proto=1;_T50=b0;
return _T50;}}}default: goto _LL9;};default: _LL9:
 return 0;};}}
# 654
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned loc,struct _fat_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl*_T0;struct Cyc_Tcdecl_Xdatatypefielddecl*_T1;struct Cyc_Absyn_Datatypefield*_T2;struct _tuple0*_T3;struct Cyc_Absyn_Datatypedecl*_T4;struct _tuple0*_T5;struct Cyc_Absyn_Datatypedecl*_T6;struct _tuple0*_T7;int _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc_Absyn_Datatypedecl*_TA;struct _tuple0*_TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_Absyn_Datatypedecl*_TD;struct _tuple0*_TE;struct Cyc_String_pa_PrintArg_struct _TF;void**_T10;void**_T11;void**_T12;unsigned _T13;struct _fat_ptr*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Absyn_Datatypedecl*_T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Datatypedecl*_T19;struct Cyc_List_List*_T1A;struct _fat_ptr _T1B;struct Cyc_Absyn_Datatypedecl*_T1C;struct _tuple0*_T1D;struct _fat_ptr _T1E;unsigned _T1F;struct _fat_ptr*_T20;int _T21;struct Cyc_Absyn_Datatypedecl*_T22;struct Cyc_List_List*_T23;struct Cyc_Absyn_Datatypedecl*_T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Datatypefield*_T26;struct Cyc_Absyn_Datatypefield*_T27;struct Cyc_List_List*_T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;struct _fat_ptr*_T2B;struct Cyc_Tcdecl_Xdatatypefielddecl*_T2C;struct Cyc_Tcdecl_Xdatatypefielddecl*_T2D;struct Cyc_Absyn_Datatypefield*_T2E;struct Cyc_Absyn_Datatypedecl*_T2F;_T0=d0;{struct Cyc_Tcdecl_Xdatatypefielddecl _T30=*_T0;_T2F=_T30.base;_T2E=_T30.field;}{struct Cyc_Absyn_Datatypedecl*b0=_T2F;struct Cyc_Absyn_Datatypefield*f0=_T2E;struct Cyc_Absyn_Datatypefield*_T30;struct Cyc_Absyn_Datatypedecl*_T31;_T1=d1;{struct Cyc_Tcdecl_Xdatatypefielddecl _T32=*_T1;_T31=_T32.base;_T30=_T32.field;}{struct Cyc_Absyn_Datatypedecl*b1=_T31;struct Cyc_Absyn_Datatypefield*f1=_T30;_T2=f0;_T3=_T2->name;{
# 659
struct _fat_ptr v=Cyc_Absynpp_qvar2string(_T3);_T4=b0;_T5=_T4->name;_T6=b1;_T7=_T6->name;_T8=
# 662
Cyc_Absyn_qvar_cmp(_T5,_T7);if(_T8==0)goto _TL10A;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;_TA=b0;_TB=_TA->name;
# 664
_T32.f1=Cyc_Absynpp_qvar2string(_TB);_T9=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T9;{struct Cyc_String_pa_PrintArg_struct _T33;_T33.tag=0;_TD=b1;_TE=_TD->name;_T33.f1=Cyc_Absynpp_qvar2string(_TE);_TC=_T33;}{struct Cyc_String_pa_PrintArg_struct _T33=_TC;{struct Cyc_String_pa_PrintArg_struct _T34;_T34.tag=0;_T34.f1=v;_TF=_T34;}{struct Cyc_String_pa_PrintArg_struct _T34=_TF;void*_T35[3];_T10=_T35 + 0;*_T10=& _T32;_T11=_T35 + 1;*_T11=& _T33;_T12=_T35 + 2;*_T12=& _T34;_T13=loc;_T14=msg;_T15=
# 663
_tag_fat("@extensible datatype %s and %s both have a field named %s",sizeof(char),58U);_T16=_tag_fat(_T35,sizeof(void*),3);Cyc_Tcdecl_merr(_T13,_T14,_T15,_T16);}}}
# 665
return 0;_TL10A: _T17=b0;_T18=_T17->tvs;_T19=b1;_T1A=_T19->tvs;_T1B=
# 669
_tag_fat("@extensible datatype",sizeof(char),21U);_T1C=b0;_T1D=_T1C->name;_T1E=Cyc_Absynpp_qvar2string(_T1D);_T1F=loc;_T20=msg;_T21=Cyc_Tcdecl_check_tvs(_T18,_T1A,_T1B,_T1E,_T1F,_T20);if(_T21)goto _TL10C;else{goto _TL10E;}
_TL10E: return 0;_TL10C: _T22=b0;_T23=_T22->tvs;_T24=b1;_T25=_T24->tvs;{
# 672
struct Cyc_List_List*inst1=Cyc_Tcdecl_build_tvs_map(_T23,_T25);_T26=f0;_T27=f1;_T28=inst1;_T29=
# 675
_tag_fat("@extensible datatype field",sizeof(char),27U);_T2A=v;_T2B=msg;{struct Cyc_Absyn_Datatypefield*f2=Cyc_Tcdecl_merge_datatypefield(_T26,_T27,_T28,_T29,_T2A,_T2B);
if(f2!=0)goto _TL10F;return 0;_TL10F:
 if(f2!=f0)goto _TL111;_T2C=d0;return _T2C;_TL111:{struct Cyc_Tcdecl_Xdatatypefielddecl*_T32=_cycalloc(sizeof(struct Cyc_Tcdecl_Xdatatypefielddecl));
_T32->base=b0;_T32->field=f2;_T2D=(struct Cyc_Tcdecl_Xdatatypefielddecl*)_T32;}return _T2D;}}}}}}
