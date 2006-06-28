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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 920 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 928
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 1237
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 79 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_RenameTemps_Env{struct Cyc_Dict_Dict d;int next_num;};
# 36 "rename_temps.cyc"
static int Cyc_RenameTemps_is_temp_var(struct _tuple0*qv){struct _tuple0*_T0;struct _tuple0 _T1;struct _fat_ptr*_T2;int _T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;struct _fat_ptr _TB;unsigned char*_TC;const char*_TD;char _TE;int _TF;struct _fat_ptr _T10;unsigned char*_T11;const char*_T12;char _T13;int _T14;struct _fat_ptr _T15;unsigned char*_T16;const char*_T17;char _T18;int _T19;_T0=qv;_T1=*_T0;_T2=_T1.f1;{
struct _fat_ptr v=*_T2;_T4=v;_T5=
_get_fat_size(_T4,sizeof(char));if(_T5 <= 4U)goto _TL0;_T6=v;_T7=_T6.curr;_T8=(const char*)_T7;_T9=_T8[0];_TA=(int)_T9;if(_TA!=95)goto _TL0;_TB=v;_TC=_TB.curr;_TD=(const char*)_TC;_TE=_TD[1];_TF=(int)_TE;if(_TF!=116)goto _TL0;_T10=v;_T11=_T10.curr;_T12=(const char*)_T11;_T13=_T12[2];_T14=(int)_T13;if(_T14!=109)goto _TL0;_T15=v;_T16=_T15.curr;_T17=(const char*)_T16;_T18=_T17[3];_T19=(int)_T18;_T3=_T19==112;goto _TL1;_TL0: _T3=0;_TL1: return _T3;}}
# 41
static void*Cyc_RenameTemps_toplevel_temp_var(int i){struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T0;struct _tuple0*_T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;struct Cyc_Int_pa_PrintArg_struct _T4;int _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;void*_T8;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T9=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_T9->tag=0;{struct _tuple0*_TA=_cycalloc(sizeof(struct _tuple0));
_TA->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_TB=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TC;_TC.tag=1;_T5=i;_TC.f1=(unsigned)_T5;_T4=_TC;}{struct Cyc_Int_pa_PrintArg_struct _TC=_T4;void*_TD[1];_TD[0]=& _TC;_T6=_tag_fat("_TmpG%X",sizeof(char),8U);_T7=_tag_fat(_TD,sizeof(void*),1);_T3=Cyc_aprintf(_T6,_T7);}*_TB=_T3;_T2=(struct _fat_ptr*)_TB;}_TA->f1=_T2;_T1=(struct _tuple0*)_TA;}_T9->f1=_T1;_T0=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T9;}_T8=(void*)_T0;return _T8;}
# 44
static void*Cyc_RenameTemps_local_temp_var(int i){void**_T0;int _T1;char*_T2;void**_T3;void*_T4;void**_T5;int _T6;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T7;struct _tuple0*_T8;struct _fat_ptr*_T9;struct _fat_ptr _TA;struct Cyc_Int_pa_PrintArg_struct _TB;int _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;void**_TF;int _T10;void*_T11;void*_T12;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T13;struct _tuple0*_T14;struct _fat_ptr*_T15;struct _fat_ptr _T16;struct Cyc_Int_pa_PrintArg_struct _T17;int _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;void*_T1B;
static void*memo_arr[256U]={};
if(i >= 256)goto _TL2;_T0=memo_arr;_T1=i;_T2=_check_known_subscript_notnull(_T0,256U,sizeof(void*),_T1);_T3=(void**)_T2;_T4=*_T3;
if(_T4!=0)goto _TL4;_T5=memo_arr;_T6=i;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T1C=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_T1C->tag=0;{struct _tuple0*_T1D=_cycalloc(sizeof(struct _tuple0));
_T1D->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1E=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _T1F;_T1F.tag=1;_TC=i;_T1F.f1=(unsigned)_TC;_TB=_T1F;}{struct Cyc_Int_pa_PrintArg_struct _T1F=_TB;void*_T20[1];_T20[0]=& _T1F;_TD=_tag_fat("_T%X",sizeof(char),5U);_TE=_tag_fat(_T20,sizeof(void*),1);_TA=Cyc_aprintf(_TD,_TE);}*_T1E=_TA;_T9=(struct _fat_ptr*)_T1E;}_T1D->f1=_T9;_T8=(struct _tuple0*)_T1D;}_T1C->f1=_T8;_T7=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T1C;}_T5[_T6]=(void*)_T7;goto _TL5;_TL4: _TL5: _TF=memo_arr;_T10=i;_T11=_TF[_T10];_T12=
_check_null(_T11);return _T12;_TL2:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T1C=_cycalloc(sizeof(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct));_T1C->tag=0;{struct _tuple0*_T1D=_cycalloc(sizeof(struct _tuple0));
# 51
_T1D->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1E=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _T1F;_T1F.tag=1;_T18=i;_T1F.f1=(unsigned)_T18;_T17=_T1F;}{struct Cyc_Int_pa_PrintArg_struct _T1F=_T17;void*_T20[1];_T20[0]=& _T1F;_T19=_tag_fat("_T%X",sizeof(char),5U);_T1A=_tag_fat(_T20,sizeof(void*),1);_T16=Cyc_aprintf(_T19,_T1A);}*_T1E=_T16;_T15=(struct _fat_ptr*)_T1E;}_T1D->f1=_T15;_T14=(struct _tuple0*)_T1D;}_T1C->f1=_T14;_T13=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T1C;}_T1B=(void*)_T13;return _T1B;}
# 54
static void Cyc_RenameTemps_rename_exp(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*);
static void Cyc_RenameTemps_rename_stmt(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*);
static int Cyc_RenameTemps_rename_visit_f1(struct Cyc_RenameTemps_Env*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void**_T4;void**_T5;void*_T6;int _T7;struct _handler_cons*_T8;int _T9;void**_TA;void*(*_TB)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_TC)(struct Cyc_Dict_Dict,void*);struct Cyc_RenameTemps_Env*_TD;struct Cyc_Dict_Dict _TE;struct _tuple0*_TF;void*_T10;struct Cyc_String_pa_PrintArg_struct _T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;_T0=e;{
void*_T15=_T0->r;void*_T16;_T1=(int*)_T15;_T2=*_T1;if(_T2!=1)goto _TL6;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T15;_T4=& _T3->f1;_T16=(void**)_T4;}{void**b=(void**)_T16;_T5=b;_T6=*_T5;{
# 59
struct _tuple0*qv=Cyc_Absyn_binding2qvar(_T6);_T7=
Cyc_RenameTemps_is_temp_var(qv);if(!_T7)goto _TL8;{struct _handler_cons _T17;_T8=& _T17;_push_handler(_T8);{int _T18=0;_T9=setjmp(_T17.handler);if(!_T9)goto _TLA;_T18=1;goto _TLB;_TLA: _TLB: if(_T18)goto _TLC;else{goto _TLE;}_TLE: _TA=b;_TC=Cyc_Dict_lookup;{
# 62
void*(*_T19)(struct Cyc_Dict_Dict,struct _tuple0*)=(void*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_TC;_TB=_T19;}_TD=env;_TE=_TD->d;_TF=qv;*_TA=_TB(_TE,_TF);_pop_handler();goto _TLD;_TLC: _T10=Cyc_Core_get_exn_thrown();{void*_T19=(void*)_T10;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 65
_T1A.f1=Cyc_Absynpp_exp2string(e);_T11=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T11;void*_T1B[1];_T1B[0]=& _T1A;_T12=Cyc_stderr;_T13=_tag_fat("can't find var %s!!!\n",sizeof(char),22U);_T14=_tag_fat(_T1B,sizeof(void*),1);Cyc_fprintf(_T12,_T13,_T14);}goto _LL5;_LL5:;}_TLD:;}}goto _TL9;_TL8: _TL9:
# 69
 return 0;}}_TL6:
 return 1;;}}
# 77
static int Cyc_RenameTemps_rename_visit_f2(struct Cyc_RenameTemps_Env*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_RenameTemps_Env*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_Absyn_Vardecl*_T7;struct _tuple0*_T8;int _T9;struct Cyc_RenameTemps_Env*_TA;int _TB;int _TC;struct Cyc_RenameTemps_Env*_TD;struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct _tuple0*,void*);struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_RenameTemps_Env*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_Absyn_Vardecl*_T12;struct _tuple0*_T13;void*_T14;struct Cyc_Absyn_Vardecl*_T15;struct Cyc_Absyn_Decl*_T16;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T17;struct Cyc_Absyn_Vardecl*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Exp*_T1A;unsigned _T1B;struct Cyc_RenameTemps_Env*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_RenameTemps_Env*_T1F;struct Cyc_Absyn_Fndecl*_T20;struct Cyc_Absyn_Stmt*_T21;struct Cyc_Warn_String_Warn_Warg_struct _T22;int(*_T23)(struct _fat_ptr);void*(*_T24)(struct _fat_ptr);struct _fat_ptr _T25;struct Cyc_Absyn_Stmt*_T26;int*_T27;int _T28;void(*_T29)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*);void(*_T2A)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_RenameTemps_Env*_T2B;struct Cyc_Absyn_Stmt*_T2C;struct Cyc_RenameTemps_Env*_T2D;struct Cyc_Absyn_Stmt*_T2E;int*_T2F;unsigned _T30;_T0=s;{
void*_T31=_T0->r;struct Cyc_Absyn_Stmt*_T32;struct Cyc_Absyn_Stmt*_T33;struct Cyc_Absyn_Decl*_T34;_T1=(int*)_T31;_T2=*_T1;switch(_T2){case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T35=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T31;_T34=_T35->f1;_T33=_T35->f2;}{struct Cyc_Absyn_Decl*d=_T34;struct Cyc_Absyn_Stmt*s1=_T33;_T3=env;{
# 80
struct Cyc_RenameTemps_Env old_env=*_T3;
_TL10: if(1)goto _TL11;else{goto _TL12;}
_TL11: _T4=d;{void*_T35=_T4->r;struct Cyc_Absyn_Fndecl*_T36;struct Cyc_Absyn_Vardecl*_T37;_T5=(int*)_T35;_T6=*_T5;switch(_T6){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T38=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T35;_T37=_T38->f1;}{struct Cyc_Absyn_Vardecl*vd=_T37;_T7=vd;_T8=_T7->name;_T9=
# 84
Cyc_RenameTemps_is_temp_var(_T8);if(!_T9)goto _TL14;_TA=env;_TB=_TA->next_num;
_TA->next_num=_TB + 1;_TC=_TB;{void*v=Cyc_RenameTemps_local_temp_var(_TC);_TD=env;_TF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T38)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))_TF;_TE=_T38;}_T10=env;_T11=_T10->d;_T12=vd;_T13=_T12->name;_T14=v;_TD->d=_TE(_T11,_T13,_T14);_T15=vd;{
struct Cyc_Absyn_Vardecl vd2=*_T15;
vd2.name=Cyc_Absyn_binding2qvar(v);_T16=d;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T38=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T38->tag=0;{struct Cyc_Absyn_Vardecl*_T39=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));
*_T39=vd2;_T18=(struct Cyc_Absyn_Vardecl*)_T39;}_T38->f1=_T18;_T17=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T38;}_T16->r=(void*)_T17;}}goto _TL15;_TL14: _TL15: _T19=vd;_T1A=_T19->initializer;_T1B=(unsigned)_T1A;
# 92
if(!_T1B)goto _TL16;_T1C=env;_T1D=vd;_T1E=_T1D->initializer;
Cyc_RenameTemps_rename_exp(_T1C,_T1E);goto _TL17;_TL16: _TL17: goto _LL7;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T38=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T35;_T36=_T38->f1;}{struct Cyc_Absyn_Fndecl*fd=_T36;_T1F=env;_T20=fd;_T21=_T20->body;
# 97
Cyc_RenameTemps_rename_stmt(_T1F,_T21);goto _LL7;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T38;_T38.tag=0;
_T38.f1=_tag_fat("RenameTemps: local decl not a var or function",sizeof(char),46U);_T22=_T38;}{struct Cyc_Warn_String_Warn_Warg_struct _T38=_T22;void*_T39[1];_T39[0]=& _T38;_T24=Cyc_Warn_impos2;{int(*_T3A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T24;_T23=_T3A;}_T25=_tag_fat(_T39,sizeof(void*),1);_T23(_T25);}}_LL7:;}_T26=s1;{
# 100
void*_T35=_T26->r;struct Cyc_Absyn_Stmt*_T36;struct Cyc_Absyn_Decl*_T37;_T27=(int*)_T35;_T28=*_T27;if(_T28!=12)goto _TL18;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T38=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T35;_T37=_T38->f1;_T36=_T38->f2;}{struct Cyc_Absyn_Decl*d2=_T37;struct Cyc_Absyn_Stmt*s2=_T36;
d=d2;s1=s2;goto _TL10;}_TL18: _T2A=Cyc_Absyn_visit_stmt;{
# 103
void(*_T38)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*))_T2A;_T29=_T38;}_T2B=env;_T2C=s1;_T29(Cyc_RenameTemps_rename_visit_f1,Cyc_RenameTemps_rename_visit_f2,_T2B,_T2C);_T2D=env;
*_T2D=old_env;
return 0;;}goto _TL10;_TL12:;}}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T35=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T31;_T33=_T35->f1;_T32=_T35->f2;}{struct Cyc_Absyn_Stmt*s1=_T33;struct Cyc_Absyn_Stmt*s2=_T32;
# 109
_TL1A: if(1)goto _TL1B;else{goto _TL1C;}
_TL1B: Cyc_RenameTemps_rename_stmt(env,s1);
LOOP2: _T2E=s2;{
void*_T35=_T2E->r;struct Cyc_Absyn_Stmt*_T36;struct Cyc_Absyn_Stmt*_T37;_T2F=(int*)_T35;_T30=*_T2F;switch(_T30){case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T38=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T35;_T37=_T38->f1;_T36=_T38->f2;}{struct Cyc_Absyn_Stmt*s3=_T37;struct Cyc_Absyn_Stmt*s4=_T36;
# 114
s1=s3;s2=s4;goto _TL1A;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T38=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T35;_T37=_T38->f2;}{struct Cyc_Absyn_Stmt*s3=_T37;
# 116
s2=s3;goto LOOP2;}default:
 Cyc_RenameTemps_rename_stmt(env,s2);return 0;};}goto _TL1A;_TL1C:;}default:
# 120
 return 1;};}}
# 123
static void Cyc_RenameTemps_rename_exp(struct Cyc_RenameTemps_Env*env,struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_RenameTemps_Env*_T2;struct Cyc_Absyn_Exp*_T3;_T1=Cyc_Absyn_visit_exp;{
void(*_T4)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*))_T1;_T0=_T4;}_T2=env;_T3=e;_T0(Cyc_RenameTemps_rename_visit_f1,Cyc_RenameTemps_rename_visit_f2,_T2,_T3);}
# 126
static void Cyc_RenameTemps_rename_stmt(struct Cyc_RenameTemps_Env*env,struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_RenameTemps_Env*_T2;struct Cyc_Absyn_Stmt*_T3;_T1=Cyc_Absyn_visit_stmt;{
void(*_T4)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*),struct Cyc_RenameTemps_Env*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T4;}_T2=env;_T3=s;_T0(Cyc_RenameTemps_rename_visit_f1,Cyc_RenameTemps_rename_visit_f2,_T2,_T3);}
# 130
struct Cyc_List_List*Cyc_RenameTemps_rename(struct Cyc_List_List*tds){struct Cyc_RenameTemps_Env _T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _tuple0*,struct _tuple0*);struct Cyc_List_List*_T4;void*_T5;struct Cyc_Absyn_Decl*_T6;int*_T7;unsigned _T8;struct Cyc_Absyn_Vardecl*_T9;struct _tuple0*_TA;int _TB;int _TC;int _TD;struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct _tuple0*,void*);struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_RenameTemps_Env _T10;struct Cyc_Dict_Dict _T11;struct Cyc_Absyn_Vardecl*_T12;struct _tuple0*_T13;void*_T14;struct Cyc_Absyn_Vardecl*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Decl*_T18;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_Absyn_Vardecl*_T1B;struct Cyc_Absyn_Exp*_T1C;unsigned _T1D;struct Cyc_RenameTemps_Env*_T1E;struct Cyc_RenameTemps_Env*_T1F;struct Cyc_Absyn_Vardecl*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_RenameTemps_Env*_T22;struct Cyc_RenameTemps_Env*_T23;struct Cyc_Absyn_Fndecl*_T24;struct Cyc_Absyn_Stmt*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;{struct Cyc_RenameTemps_Env _T28;_T2=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T29)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T2;_T1=_T29;}_T3=Cyc_Absyn_qvar_cmp;_T28.d=_T1(_T3);_T28.next_num=0;_T0=_T28;}{struct Cyc_RenameTemps_Env env=_T0;{
struct Cyc_List_List*tds2=tds;_TL21: if(tds2!=0)goto _TL1F;else{goto _TL20;}
_TL1F: _T4=tds2;_T5=_T4->hd;_T6=(struct Cyc_Absyn_Decl*)_T5;{void*_T28=_T6->r;struct Cyc_Absyn_Fndecl*_T29;struct Cyc_Absyn_Vardecl*_T2A;_T7=(int*)_T28;_T8=*_T7;switch(_T8){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T28;_T2A=_T2B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2A;_T9=vd;_TA=_T9->name;_TB=
# 135
Cyc_RenameTemps_is_temp_var(_TA);if(!_TB)goto _TL23;_TC=env.next_num;
env.next_num=_TC + 1;_TD=_TC;{void*v=Cyc_RenameTemps_toplevel_temp_var(_TD);_TF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T2B)(struct Cyc_Dict_Dict,struct _tuple0*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,void*))_TF;_TE=_T2B;}_T10=env;_T11=_T10.d;_T12=vd;_T13=_T12->name;_T14=v;env.d=_TE(_T11,_T13,_T14);_T15=vd;{
struct Cyc_Absyn_Vardecl vd2=*_T15;
vd2.name=Cyc_Absyn_binding2qvar(v);_T16=tds2;_T17=_T16->hd;_T18=(struct Cyc_Absyn_Decl*)_T17;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2B=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_T2B->tag=0;{struct Cyc_Absyn_Vardecl*_T2C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));
*_T2C=vd2;_T1A=(struct Cyc_Absyn_Vardecl*)_T2C;}_T2B->f1=_T1A;_T19=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2B;}_T18->r=(void*)_T19;}}goto _TL24;_TL23: _TL24: _T1B=vd;_T1C=_T1B->initializer;_T1D=(unsigned)_T1C;
# 143
if(!_T1D)goto _TL25;_T1E=& env;_T1F=(struct Cyc_RenameTemps_Env*)_T1E;_T20=vd;_T21=_T20->initializer;
Cyc_RenameTemps_rename_exp(_T1F,_T21);goto _TL26;_TL25: _TL26: goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2B=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T28;_T29=_T2B->f1;}{struct Cyc_Absyn_Fndecl*fd=_T29;
# 148
struct Cyc_RenameTemps_Env env2=env;
env.next_num=0;_T22=& env;_T23=(struct Cyc_RenameTemps_Env*)_T22;_T24=fd;_T25=_T24->body;
Cyc_RenameTemps_rename_stmt(_T23,_T25);
env=env2;goto _LL0;}default: goto _LL0;}_LL0:;}_T26=tds2;
# 132
tds2=_T26->tl;goto _TL21;_TL20:;}_T27=tds;
# 155
return _T27;}}
