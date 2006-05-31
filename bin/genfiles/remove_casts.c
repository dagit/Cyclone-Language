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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};
# 549 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 939 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 1142
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 54 "warn.h"
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);
# 29 "remove_casts.cyc"
static int Cyc_RemoveCasts_remove_visit_f1(int,struct Cyc_Absyn_Exp*);
# 31
static int Cyc_RemoveCasts_remove_visit_f2(int ignore,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp*_T3;int*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Exp*_TC;void*_TD;int*_TE;unsigned _TF;struct Cyc_Absyn_Exp*_T10;void*_T11;struct Cyc_Absyn_Stmt*_T12;struct Cyc_Absyn_Stmt*_T13;struct Cyc_Absyn_Stmt*_T14;int*_T15;unsigned _T16;struct Cyc_Absyn_Stmt*_T17;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T18;int(*_T19)(unsigned,struct _fat_ptr,struct _fat_ptr);void*(*_T1A)(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Absyn_Stmt*_T1B;unsigned _T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;void(*_T1F)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T20)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);int(*_T21)(int,struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Stmt*_T22;struct Cyc_Absyn_Exp*_T23;void*_T24;struct Cyc_Absyn_Stmt*_T25;struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T26;void(*_T27)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T28)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);int(*_T29)(int,struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Stmt*_T2A;_T0=s;{
void*_T2B=_T0->r;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Absyn_Stmt*_T2E;struct Cyc_Absyn_Exp*_T2F;_T1=(int*)_T2B;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T30=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T2B;_T2F=_T30->f1;}{struct Cyc_Absyn_Exp*e=_T2F;_T3=e;{
# 34
void*_T30=_T3->r;struct Cyc_Absyn_Exp*_T31;_T4=(int*)_T30;_T5=*_T4;if(_T5!=14)goto _TL1;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T32=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T30;_T31=_T32->f2;}{struct Cyc_Absyn_Exp*e1=_T31;_T6=e;_T7=e1;
*_T6=*_T7;goto _LL9;}_TL1: goto _LL9;_LL9:;}
# 38
return 1;}case 3: _T8=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2B;_T9=_T8->f1;if(_T9==0)goto _TL3;_TA=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2B;_TB=_TA->f1;_TC=(struct Cyc_Absyn_Exp*)_TB;_TD=_TC->r;_TE=(int*)_TD;_TF=*_TE;switch(_TF){case 36:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T30=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2B;_T10=_T30->f1;{struct Cyc_Absyn_Exp _T31=*_T10;_T11=_T31.r;{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T32=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T11;_T2E=_T32->f1;}}}{struct Cyc_Absyn_Stmt*s1=_T2E;_T12=s;_T13=s1;
# 41
_T12->r=_T13->r;
_TL6: if(1)goto _TL7;else{goto _TL8;}
_TL7: _T14=s1;{void*_T30=_T14->r;struct Cyc_Absyn_Stmt*_T31;struct Cyc_Absyn_Exp*_T32;_T15=(int*)_T30;_T16=*_T15;switch(_T16){case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T33=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T30;_T32=_T33->f1;}{struct Cyc_Absyn_Exp*e=_T32;_T17=s1;{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T33=_cycalloc(sizeof(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct));_T33->tag=3;
# 45
_T33->f1=e;_T18=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T33;}_T17->r=(void*)_T18;goto _LLE;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T33=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T30;_T31=_T33->f2;}{struct Cyc_Absyn_Stmt*s2=_T31;
s1=s2;goto _TL6;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T33=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T30;_T31=_T33->f2;}{struct Cyc_Absyn_Stmt*s2=_T31;
s1=s2;goto _TL6;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T33=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T30;_T31=_T33->f2;}{struct Cyc_Absyn_Stmt*s2=_T31;
s1=s2;goto _TL6;}default: _T1A=Cyc_Warn_impos_loc;{
int(*_T33)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))_T1A;_T19=_T33;}_T1B=s1;_T1C=_T1B->loc;_T1D=_tag_fat("pushing return: no expression statement!",sizeof(char),41U);_T1E=_tag_fat(0U,sizeof(void*),0);_T19(_T1C,_T1D,_T1E);}_LLE:;}goto _TL8;goto _TL6;_TL8: _T20=Cyc_Absyn_visit_stmt;{
# 53
void(*_T30)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T20;_T1F=_T30;}_T21=Cyc_RemoveCasts_remove_visit_f1;_T22=s1;_T1F(_T21,Cyc_RemoveCasts_remove_visit_f2,1,_T22);
return 0;}case 6:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T30=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2B;_T23=_T30->f1;{struct Cyc_Absyn_Exp _T31=*_T23;_T24=_T31.r;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T32=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T24;_T2F=_T32->f1;_T2D=_T32->f2;_T2C=_T32->f3;}}}{struct Cyc_Absyn_Exp*e1=_T2F;struct Cyc_Absyn_Exp*e2=_T2D;struct Cyc_Absyn_Exp*e3=_T2C;_T25=s;{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T30=_cycalloc(sizeof(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct));_T30->tag=4;
# 56
_T30->f1=e1;_T30->f2=Cyc_Absyn_return_stmt(e2,0U);_T30->f3=Cyc_Absyn_return_stmt(e3,0U);_T26=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T30;}_T25->r=(void*)_T26;_T28=Cyc_Absyn_visit_stmt;{
void(*_T30)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T28;_T27=_T30;}_T29=Cyc_RemoveCasts_remove_visit_f1;_T2A=s;_T27(_T29,Cyc_RemoveCasts_remove_visit_f2,1,_T2A);
return 0;}default: goto _LL7;}goto _TL4;_TL3: goto _LL7;_TL4:;default: _LL7:
 return 1;};}}
# 62
static int Cyc_RemoveCasts_remove_visit_f1(int ignore,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;void*_T3;void(*_T4)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*);void(*_T5)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);int _T6;struct Cyc_Absyn_Exp*_T7;int*_T8;int _T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;int*_TC;int _TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Stmt*_T12;int*_T13;unsigned _T14;void(*_T15)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*);void(*_T16)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*_T17;void(*_T18)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T19)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Absyn_Stmt*_T1A;_T0=e;{
void*_T1B=_T0->r;struct Cyc_Absyn_Stmt*_T1C;struct Cyc_Absyn_Exp*_T1D;void*_T1E;_T1=(int*)_T1B;_T2=*_T1;switch(_T2){case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T1B;_T3=_T1F->f1;_T1E=(void*)_T3;_T1D=_T1F->f2;}{void*t=_T1E;struct Cyc_Absyn_Exp*e1=_T1D;_T5=Cyc_Absyn_visit_exp;{
# 66
void(*_T1F)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*))_T5;_T4=_T1F;}_T6=ignore;_T7=e1;_T4(Cyc_RemoveCasts_remove_visit_f1,Cyc_RemoveCasts_remove_visit_f2,_T6,_T7);{
void*_T1F=Cyc_Absyn_compress(t);_T8=(int*)_T1F;_T9=*_T8;if(_T9!=0)goto _TLB;_TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1F;_TB=_TA->f1;_TC=(int*)_TB;_TD=*_TC;if(_TD!=24)goto _TLD;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1F;_TF=_TE->f2;if(_TF!=0)goto _TLF;_T10=e;_T11=e1;
*_T10=*_T11;return 0;_TLF: goto _LLA;_TLD: goto _LLA;_TLB: _LLA:
 return 0;;}}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T1F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T1B;_T1C=_T1F->f1;}{struct Cyc_Absyn_Stmt*s=_T1C;
# 72
_TL11: if(1)goto _TL12;else{goto _TL13;}
_TL12: _T12=s;{void*_T1F=_T12->r;struct Cyc_Absyn_Stmt*_T20;struct Cyc_Absyn_Stmt*_T21;struct Cyc_Absyn_Exp*_T22;_T13=(int*)_T1F;_T14=*_T13;switch(_T14){case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T23=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T1F;_T22=_T23->f1;}{struct Cyc_Absyn_Exp*e=_T22;_T16=Cyc_Absyn_visit_exp;{
# 75
void(*_T23)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Exp*))_T16;_T15=_T23;}_T17=e;_T15(Cyc_RemoveCasts_remove_visit_f1,Cyc_RemoveCasts_remove_visit_f2,1,_T17);
return 0;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T23=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T1F;_T21=_T23->f1;_T20=_T23->f2;}{struct Cyc_Absyn_Stmt*s1=_T21;struct Cyc_Absyn_Stmt*s2=_T20;_T19=Cyc_Absyn_visit_stmt;{
# 78
void(*_T23)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T19;_T18=_T23;}_T1A=s1;_T18(Cyc_RemoveCasts_remove_visit_f1,Cyc_RemoveCasts_remove_visit_f2,1,_T1A);
s=s2;goto _TL11;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T23=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T1F;_T21=_T23->f2;}{struct Cyc_Absyn_Stmt*s1=_T21;
# 81
s=s1;goto _TL11;}default:
# 86
 return 0;};}goto _TL11;_TL13:
# 88
 return 0;}default:
 return 1;};}}
# 92
struct Cyc_List_List*Cyc_RemoveCasts_remove_casts(struct Cyc_List_List*tds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;int _T4;void(*_T5)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T6)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Absyn_Fndecl*_T7;struct Cyc_Absyn_Stmt*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;{
struct Cyc_List_List*tds2=tds;_TL18: if(tds2!=0)goto _TL16;else{goto _TL17;}
_TL16: _T0=tds2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_TB=_T2->r;struct Cyc_Absyn_Fndecl*_TC;_T3=(int*)_TB;_T4=*_T3;if(_T4!=1)goto _TL19;{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TD=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_TB;_TC=_TD->f1;}{struct Cyc_Absyn_Fndecl*fd=_TC;_T6=Cyc_Absyn_visit_stmt;{
# 96
void(*_TD)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T6;_T5=_TD;}_T7=fd;_T8=_T7->body;_T5(Cyc_RemoveCasts_remove_visit_f1,Cyc_RemoveCasts_remove_visit_f2,1,_T8);goto _LL0;}_TL19: goto _LL0;_LL0:;}_T9=tds2;
# 93
tds2=_T9->tl;goto _TL18;_TL17:;}_TA=tds;
# 99
return _TA;}
