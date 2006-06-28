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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 920 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100 "cycboot.h"
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 46 "graph.h"
void Cyc_Graph_print(struct Cyc___cycFILE*,struct Cyc_Dict_Dict,void(*)(struct Cyc___cycFILE*,void*));
# 50
extern struct Cyc_Dict_Dict Cyc_Graph_empty(int(*)(void*,void*));
# 53
extern struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict,void*);
# 59
extern struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*);
# 71 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);struct _tuple11{struct Cyc_Hashtable_Table*f0;struct Cyc_Set_Set*f1;};
# 49 "callgraph.cyc"
static int Cyc_Callgraph_visit_cg_f1(struct _tuple11*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_Absyn_Fndecl*_T8;void*_T9;struct Cyc_Absyn_Vardecl*_TA;struct _tuple11*_TB;struct _handler_cons*_TC;int _TD;struct Cyc_Set_Set*(*_TE)(struct Cyc_Set_Set*,struct Cyc_Absyn_Fndecl*);struct Cyc_Set_Set*(*_TF)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T10;struct Cyc_Absyn_Fndecl*(*_T11)(struct Cyc_Hashtable_Table*,struct _tuple0*);void*(*_T12)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Absyn_Fndecl*_T13;struct _tuple11*_T14;struct _tuple11 _T15;void*_T16;struct Cyc_Core_Not_found_exn_struct*_T17;char*_T18;char*_T19;
struct _tuple0*name;_T0=e;{
void*_T1A=_T0->r;struct Cyc_Absyn_Vardecl*_T1B;struct Cyc_Absyn_Fndecl*_T1C;_T1=(int*)_T1A;_T2=*_T1;if(_T2!=1)goto _TL0;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T1A;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T1A;_T7=_T1D->f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1E=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T7;_T1C=_T1E->f1;}}{struct Cyc_Absyn_Fndecl*fd=_T1C;_T8=fd;
name=_T8->name;goto _LL0;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T1A;_T9=_T1D->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T1E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T9;_T1B=_T1E->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T1B;_TA=vd;
name=_TA->name;goto _LL0;}default: goto _LL5;}goto _TL1;_TL0: _LL5:
 return 1;_TL1: _LL0:;}{struct Cyc_Set_Set*_T1A;struct Cyc_Hashtable_Table*_T1B;_TB=env;{struct _tuple11 _T1C=*_TB;_T1B=_T1C.f0;_T1A=_T1C.f1;}{struct Cyc_Hashtable_Table*fds=_T1B;struct Cyc_Set_Set*s=_T1A;{struct _handler_cons _T1C;_TC=& _T1C;_push_handler(_TC);{int _T1D=0;_TD=setjmp(_T1C.handler);if(!_TD)goto _TL3;_T1D=1;goto _TL4;_TL3: _TL4: if(_T1D)goto _TL5;else{goto _TL7;}_TL7: _TF=Cyc_Set_insert;{
# 60
struct Cyc_Set_Set*(*_T1E)(struct Cyc_Set_Set*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Fndecl*))_TF;_TE=_T1E;}_T10=s;_T12=Cyc_Hashtable_lookup;{struct Cyc_Absyn_Fndecl*(*_T1E)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_T12;_T11=_T1E;}_T13=_T11(fds,name);s=_TE(_T10,_T13);_T14=env;{struct _tuple11 _T1E;
# 62
_T1E.f0=fds;_T1E.f1=s;_T15=_T1E;}*_T14=_T15;_pop_handler();goto _TL6;_TL5: _T16=Cyc_Core_get_exn_thrown();{void*_T1E=(void*)_T16;void*_T1F;_T17=(struct Cyc_Core_Not_found_exn_struct*)_T1E;_T18=_T17->tag;_T19=Cyc_Core_Not_found;if(_T18!=_T19)goto _TL8;goto _LLA;_TL8: _T1F=_T1E;{void*exn=_T1F;_rethrow(exn);}_LLA:;}_TL6:;}}
# 64
return 0;}}}
# 66
static int Cyc_Callgraph_visit_cg_f2(struct _tuple11*env,struct Cyc_Absyn_Stmt*s){
return 1;}
# 69
static struct Cyc_Set_Set*Cyc_Callgraph_cg_stmt(struct Cyc_Hashtable_Table*fds,struct Cyc_Absyn_Stmt*e,struct Cyc_Set_Set*s){struct _tuple11 _T0;void(*_T1)(int(*)(struct _tuple11*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple11*,struct Cyc_Absyn_Stmt*),struct _tuple11*,struct Cyc_Absyn_Stmt*);void(*_T2)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct _tuple11*_T3;struct Cyc_Absyn_Stmt*_T4;struct _tuple11 _T5;struct Cyc_Set_Set*_T6;{struct _tuple11 _T7;
_T7.f0=fds;_T7.f1=s;_T0=_T7;}{struct _tuple11 env=_T0;_T2=Cyc_Absyn_visit_stmt;{
void(*_T7)(int(*)(struct _tuple11*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple11*,struct Cyc_Absyn_Stmt*),struct _tuple11*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct _tuple11*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple11*,struct Cyc_Absyn_Stmt*),struct _tuple11*,struct Cyc_Absyn_Stmt*))_T2;_T1=_T7;}_T3=& env;_T4=e;_T1(Cyc_Callgraph_visit_cg_f1,Cyc_Callgraph_visit_cg_f2,_T3,_T4);{struct Cyc_Set_Set*_T7;_T5=env;_T7=_T5.f1;{struct Cyc_Set_Set*s=_T7;_T6=s;
# 73
return _T6;}}}}
# 76
static int Cyc_Callgraph_fndecl_cmp(struct Cyc_Absyn_Fndecl*fd1,struct Cyc_Absyn_Fndecl*fd2){struct Cyc_Absyn_Fndecl*_T0;struct _tuple0*_T1;struct Cyc_Absyn_Fndecl*_T2;struct _tuple0*_T3;int _T4;_T0=fd1;_T1=_T0->name;_T2=fd2;_T3=_T2->name;_T4=
Cyc_Absyn_qvar_cmp(_T1,_T3);return _T4;}
# 79
static int Cyc_Callgraph_hash_fndecl(struct Cyc_Absyn_Fndecl*fd){struct Cyc_Absyn_Fndecl*_T0;int _T1;_T0=fd;_T1=(int)_T0;return _T1;}
# 81
static struct Cyc_Dict_Dict Cyc_Callgraph_cg_topdecls(struct Cyc_Hashtable_Table*fds,struct Cyc_Dict_Dict cg,struct Cyc_List_List*ds){struct Cyc_Set_Set*(*_T0)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*));struct Cyc_Set_Set*(*_T1)(int(*)(void*,void*));struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_Dict_Dict(*_T7)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*,struct Cyc_Set_Set*);struct Cyc_Dict_Dict(*_T8)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*);struct Cyc_Dict_Dict _T9;struct Cyc_Absyn_Fndecl*_TA;struct Cyc_Hashtable_Table*_TB;struct Cyc_Absyn_Fndecl*_TC;struct Cyc_Absyn_Stmt*_TD;struct Cyc_Set_Set*_TE;struct Cyc_Set_Set*_TF;struct Cyc_List_List*_T10;struct Cyc_Dict_Dict _T11;_T1=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T12)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*))=(struct Cyc_Set_Set*(*)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*)))_T1;_T0=_T12;}{struct Cyc_Set_Set*mt=_T0(Cyc_Callgraph_fndecl_cmp);{
struct Cyc_List_List*_T12=ds;struct Cyc_List_List*ds=_T12;_TLD: if(ds!=0)goto _TLB;else{goto _TLC;}
_TLB: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T13=_T4->r;struct Cyc_List_List*_T14;struct Cyc_Absyn_Fndecl*_T15;_T5=(int*)_T13;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T13;_T15=_T16->f1;}{struct Cyc_Absyn_Fndecl*fd=_T15;_T8=Cyc_Graph_add_edges;{
# 86
struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*,struct Cyc_Set_Set*))_T8;_T7=_T16;}_T9=cg;_TA=fd;_TB=fds;_TC=fd;_TD=_TC->body;_TE=mt;_TF=Cyc_Callgraph_cg_stmt(_TB,_TD,_TE);cg=_T7(_T9,_TA,_TF);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T13;_T14=_T16->f2;}{struct Cyc_List_List*ds=_T14;_T14=ds;goto _LL6;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T13;_T14=_T16->f2;}_LL6: {struct Cyc_List_List*ds=_T14;
# 88
cg=Cyc_Callgraph_cg_topdecls(fds,cg,ds);goto _LL0;}default: goto _LL0;}_LL0:;}_T10=ds;
# 83
ds=_T10->tl;goto _TLD;_TLC:;}_T11=cg;
# 91
return _T11;}}
# 95
static struct Cyc_Dict_Dict Cyc_Callgraph_enter_fndecls(struct Cyc_Hashtable_Table*fds,struct Cyc_Dict_Dict cg,struct Cyc_List_List*ds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;void(*_T5)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*);void(*_T6)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T7;struct Cyc_Absyn_Fndecl*_T8;struct _tuple0*_T9;struct Cyc_Absyn_Fndecl*_TA;struct Cyc_Dict_Dict(*_TB)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*);struct Cyc_Dict_Dict(*_TC)(struct Cyc_Dict_Dict,void*);struct Cyc_List_List*_TD;struct Cyc_Dict_Dict _TE;{
# 97
struct Cyc_List_List*_TF=ds;struct Cyc_List_List*ds=_TF;_TL12: if(ds!=0)goto _TL10;else{goto _TL11;}
_TL10: _T0=ds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T10=_T2->r;struct Cyc_List_List*_T11;struct Cyc_Absyn_Fndecl*_T12;_T3=(int*)_T10;_T4=*_T3;switch(_T4){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T13=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T10;_T12=_T13->f1;}{struct Cyc_Absyn_Fndecl*f=_T12;_T6=Cyc_Hashtable_insert;{
# 100
void(*_T13)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*))_T6;_T5=_T13;}_T7=fds;_T8=f;_T9=_T8->name;_TA=f;_T5(_T7,_T9,_TA);_TC=Cyc_Graph_add_node;{
struct Cyc_Dict_Dict(*_T13)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Fndecl*))_TC;_TB=_T13;}cg=_TB(cg,f);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T13=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T10;_T11=_T13->f2;}{struct Cyc_List_List*ds=_T11;_T11=ds;goto _LL6;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T13=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T10;_T11=_T13->f2;}_LL6: {struct Cyc_List_List*ds=_T11;
# 104
cg=Cyc_Callgraph_enter_fndecls(fds,cg,ds);goto _LL0;}default: goto _LL0;}_LL0:;}_TD=ds;
# 97
ds=_TD->tl;goto _TL12;_TL11:;}_TE=cg;
# 107
return _TE;}
# 110
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*ds){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct _tuple0*,struct _tuple0*);int(*_T3)(struct _tuple0*);struct Cyc_Hashtable_Table*_T4;struct Cyc_Dict_Dict(*_T5)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*));struct Cyc_Dict_Dict(*_T6)(int(*)(void*,void*));struct Cyc_Dict_Dict _T7;struct Cyc_List_List*_T8;struct Cyc_Dict_Dict _T9;_T1=Cyc_Hashtable_create;{
# 112
struct Cyc_Hashtable_Table*(*_TA)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))_T1;_T0=_TA;}_T2=Cyc_Absyn_qvar_cmp;_T3=Cyc_Absyn_hash_qvar;{struct Cyc_Hashtable_Table*fd=_T0(51,_T2,_T3);_T4=fd;_T6=Cyc_Graph_empty;{
struct Cyc_Dict_Dict(*_TA)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*))=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Fndecl*,struct Cyc_Absyn_Fndecl*)))_T6;_T5=_TA;}_T7=_T5(Cyc_Callgraph_fndecl_cmp);_T8=ds;{struct Cyc_Dict_Dict cg=Cyc_Callgraph_enter_fndecls(_T4,_T7,_T8);
# 115
cg=Cyc_Callgraph_cg_topdecls(fd,cg,ds);_T9=cg;
return _T9;}}}
# 119
static void Cyc_Callgraph_print_fndecl(struct Cyc___cycFILE*f,struct Cyc_Absyn_Fndecl*fd){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc_Absyn_Fndecl*_T1;struct _tuple0*_T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;{struct Cyc_String_pa_PrintArg_struct _T6;_T6.tag=0;_T1=fd;_T2=_T1->name;
_T6.f1=Cyc_Absynpp_qvar2string(_T2);_T0=_T6;}{struct Cyc_String_pa_PrintArg_struct _T6=_T0;void*_T7[1];_T7[0]=& _T6;_T3=f;_T4=_tag_fat("%s ",sizeof(char),4U);_T5=_tag_fat(_T7,sizeof(void*),1);Cyc_fprintf(_T3,_T4,_T5);}}
# 123
void Cyc_Callgraph_print_callgraph(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict cg){void(*_T0)(struct Cyc___cycFILE*,struct Cyc_Dict_Dict,void(*)(struct Cyc___cycFILE*,struct Cyc_Absyn_Fndecl*));void(*_T1)(struct Cyc___cycFILE*,struct Cyc_Dict_Dict,void(*)(struct Cyc___cycFILE*,void*));struct Cyc___cycFILE*_T2;struct Cyc_Dict_Dict _T3;_T1=Cyc_Graph_print;{
void(*_T4)(struct Cyc___cycFILE*,struct Cyc_Dict_Dict,void(*)(struct Cyc___cycFILE*,struct Cyc_Absyn_Fndecl*))=(void(*)(struct Cyc___cycFILE*,struct Cyc_Dict_Dict,void(*)(struct Cyc___cycFILE*,struct Cyc_Absyn_Fndecl*)))_T1;_T0=_T4;}_T2=f;_T3=cg;_T0(_T2,_T3,Cyc_Callgraph_print_fndecl);}
