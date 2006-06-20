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
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 1055 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned);
# 1117
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1139
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1151
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 79 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);
# 226 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 39 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 41
struct _tuple1*Cyc_Toc_temp_var (void);
# 32 "toseqc.cyc"
enum Cyc_Toseqc_SideEffect{Cyc_Toseqc_Const =0U,Cyc_Toseqc_NoEffect =1U,Cyc_Toseqc_ExnEffect =2U,Cyc_Toseqc_AnyEffect =3U};struct _tuple12{enum Cyc_Toseqc_SideEffect f0;enum Cyc_Toseqc_SideEffect f1;};
# 34
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_join_side_effect(enum Cyc_Toseqc_SideEffect e1,enum Cyc_Toseqc_SideEffect e2){struct _tuple12 _T0;enum Cyc_Toseqc_SideEffect _T1;enum Cyc_Toseqc_SideEffect _T2;enum Cyc_Toseqc_SideEffect _T3;enum Cyc_Toseqc_SideEffect _T4;enum Cyc_Toseqc_SideEffect _T5;enum Cyc_Toseqc_SideEffect _T6;enum Cyc_Toseqc_SideEffect _T7;enum Cyc_Toseqc_SideEffect _T8;enum Cyc_Toseqc_SideEffect _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;int(*_TB)(struct _fat_ptr);void*(*_TC)(struct _fat_ptr);struct _fat_ptr _TD;{struct _tuple12 _TE;
_TE.f0=e1;_TE.f1=e2;_T0=_TE;}{struct _tuple12 _TE=_T0;_T1=_TE.f0;if(_T1!=Cyc_Toseqc_AnyEffect)goto _TL0;goto _LL4;_TL0: _T2=_TE.f1;if(_T2!=Cyc_Toseqc_AnyEffect)goto _TL2;_LL4:
# 37
 return 3U;_TL2: _T3=_TE.f0;if(_T3!=Cyc_Toseqc_ExnEffect)goto _TL4;_T4=_TE.f1;if(_T4!=Cyc_Toseqc_ExnEffect)goto _TL6;
# 39
return 3U;_TL6: goto _LLA;_TL4: _T5=_TE.f1;if(_T5!=Cyc_Toseqc_ExnEffect)goto _TL8;_LLA:
# 41
 return 2U;_TL8: _T6=_TE.f0;if(_T6!=Cyc_Toseqc_NoEffect)goto _TLA;goto _LLE;_TLA: _T7=_TE.f1;if(_T7!=Cyc_Toseqc_NoEffect)goto _TLC;_LLE:
# 43
 return 1U;_TLC: _T8=_TE.f0;if(_T8!=Cyc_Toseqc_Const)goto _TLE;_T9=_TE.f1;if(_T9!=Cyc_Toseqc_Const)goto _TL10;
return 0U;_TL10: goto _LL11;_TLE: _LL11:{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;
_TF.f1=_tag_fat("join_side_effect",sizeof(char),17U);_TA=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_TA;void*_T10[1];_T10[0]=& _TF;_TC=Cyc_Warn_impos2;{int(*_T11)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TC;_TB=_T11;}_TD=_tag_fat(_T10,sizeof(void*),1);_TB(_TD);};}}static char _TmpG0[14U]="_get_fat_size";static char _TmpG1[9U]="_tag_fat";static char _TmpG2[24U]="_get_zero_arr_size_char";static char _TmpG3[25U]="_get_zero_arr_size_short";static char _TmpG4[23U]="_get_zero_arr_size_int";static char _TmpG5[25U]="_get_zero_arr_size_float";static char _TmpG6[26U]="_get_zero_arr_size_double";static char _TmpG7[30U]="_get_zero_arr_size_longdouble";static char _TmpG8[28U]="_get_zero_arr_size_voidstar";
# 50
static struct _fat_ptr Cyc_Toseqc_pure_funs[9U]={{(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 14U},{(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 9U},{(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 24U},{(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 25U},{(unsigned char*)_TmpG4,(unsigned char*)_TmpG4,(unsigned char*)_TmpG4 + 23U},{(unsigned char*)_TmpG5,(unsigned char*)_TmpG5,(unsigned char*)_TmpG5 + 25U},{(unsigned char*)_TmpG6,(unsigned char*)_TmpG6,(unsigned char*)_TmpG6 + 26U},{(unsigned char*)_TmpG7,(unsigned char*)_TmpG7,(unsigned char*)_TmpG7 + 30U},{(unsigned char*)_TmpG8,(unsigned char*)_TmpG8,(unsigned char*)_TmpG8 + 28U}};static char _TmpG9[12U]="_check_null";static char _TmpGA[28U]="_check_known_subscript_null";static char _TmpGB[31U]="_check_known_subscript_notnull";static char _TmpGC[21U]="_check_fat_subscript";static char _TmpGD[19U]="_check_fat_at_base";static char _TmpGE[15U]="_untag_fat_ptr";static char _TmpGF[15U]="_region_malloc";
# 62
static struct _fat_ptr Cyc_Toseqc_exn_effect_funs[7U]={{(unsigned char*)_TmpG9,(unsigned char*)_TmpG9,(unsigned char*)_TmpG9 + 12U},{(unsigned char*)_TmpGA,(unsigned char*)_TmpGA,(unsigned char*)_TmpGA + 28U},{(unsigned char*)_TmpGB,(unsigned char*)_TmpGB,(unsigned char*)_TmpGB + 31U},{(unsigned char*)_TmpGC,(unsigned char*)_TmpGC,(unsigned char*)_TmpGC + 21U},{(unsigned char*)_TmpGD,(unsigned char*)_TmpGD,(unsigned char*)_TmpGD + 19U},{(unsigned char*)_TmpGE,(unsigned char*)_TmpGE,(unsigned char*)_TmpGE + 15U},{(unsigned char*)_TmpGF,(unsigned char*)_TmpGF,(unsigned char*)_TmpGF + 15U}};
# 71
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_fun_effect(struct _tuple1*qv){struct _tuple1*_T0;struct _tuple1*_T1;union Cyc_Absyn_Nmspace _T2;struct _union_Nmspace_Loc_n _T3;unsigned _T4;struct _tuple1*_T5;int _T6;unsigned _T7;struct _fat_ptr*_T8;int _T9;struct _fat_ptr _TA;struct _fat_ptr*_TB;struct _fat_ptr _TC;int _TD;int _TE;unsigned _TF;struct _fat_ptr*_T10;int _T11;struct _fat_ptr _T12;struct _fat_ptr*_T13;struct _fat_ptr _T14;int _T15;struct _fat_ptr*_T16;_T0=qv;_T1=(struct _tuple1*)_T0;_T2=_T1->f0;_T3=_T2.Loc_n;_T4=_T3.tag;if(_T4!=4)goto _TL12;_T5=qv;{struct _tuple1 _T17=*_T5;_T16=_T17.f1;}{struct _fat_ptr*fn=_T16;{
# 74
int i=0;_TL17: _T6=i;_T7=(unsigned)_T6;if(_T7 < 9U)goto _TL15;else{goto _TL16;}
_TL15: _T8=Cyc_Toseqc_pure_funs;_T9=i;_TA=_T8[_T9];_TB=fn;_TC=*_TB;_TD=Cyc_strcmp(_TA,_TC);if(_TD)goto _TL18;else{goto _TL1A;}
_TL1A: return 1U;_TL18:
# 74
 i=i + 1;goto _TL17;_TL16:;}{
# 77
int i=0;_TL1E: _TE=i;_TF=(unsigned)_TE;if(_TF < 7U)goto _TL1C;else{goto _TL1D;}
_TL1C: _T10=Cyc_Toseqc_exn_effect_funs;_T11=i;_T12=_T10[_T11];_T13=fn;_T14=*_T13;_T15=Cyc_strcmp(_T12,_T14);if(_T15)goto _TL1F;else{goto _TL21;}
_TL21: return 2U;_TL1F:
# 77
 i=i + 1;goto _TL1E;_TL1D:;}
# 80
return 3U;}_TL12:
 return 3U;;}
# 85
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*);
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exps_effect(struct Cyc_List_List*es){enum Cyc_Toseqc_SideEffect _T0;int _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Exp*_T4;enum Cyc_Toseqc_SideEffect _T5;enum Cyc_Toseqc_SideEffect _T6;struct Cyc_List_List*_T7;enum Cyc_Toseqc_SideEffect _T8;
enum Cyc_Toseqc_SideEffect res=0U;
_TL25: if(es!=0)goto _TL26;else{goto _TL24;}_TL26: _T0=res;_T1=(int)_T0;if(_T1!=3)goto _TL23;else{goto _TL24;}
_TL23: _T2=es;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Exp*)_T3;_T5=Cyc_Toseqc_exp_effect(_T4);_T6=res;res=Cyc_Toseqc_join_side_effect(_T5,_T6);_T7=es;
# 89
es=_T7->tl;goto _TL25;_TL24: _T8=res;
# 91
return _T8;}
# 94
static void*Cyc_Toseqc_unexpected_form(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct Cyc_Warn_Exp_Warn_Warg_struct _T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;struct _fat_ptr _T8;
struct _fat_ptr bad_form;_T0=e;{
void*_T9=_T0->r;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 2:
 bad_form=_tag_fat("Pragma_e",sizeof(char),9U);goto _LL0;case 11:
 bad_form=_tag_fat("Throw_e",sizeof(char),8U);goto _LL0;case 16:
 bad_form=_tag_fat("New_e",sizeof(char),6U);goto _LL0;case 24:
 bad_form=_tag_fat("CompoundLit_e",sizeof(char),14U);goto _LL0;case 25:
 bad_form=_tag_fat("Array_e",sizeof(char),8U);goto _LL0;case 26:
 bad_form=_tag_fat("Comprehension_e",sizeof(char),16U);goto _LL0;case 27:
 bad_form=_tag_fat("ComprehensionNoinit_e",sizeof(char),22U);goto _LL0;case 28:
 bad_form=_tag_fat("Aggregate_e",sizeof(char),12U);goto _LL0;case 29:
 bad_form=_tag_fat("AnonStruct_e",sizeof(char),13U);goto _LL0;case 30:
 bad_form=_tag_fat("Datatype_e",sizeof(char),11U);goto _LL0;case 33:
 bad_form=_tag_fat("Malloc_e",sizeof(char),9U);goto _LL0;case 34:
 bad_form=_tag_fat("Swap_e",sizeof(char),7U);goto _LL0;case 37:
 bad_form=_tag_fat("Tagcheck_e",sizeof(char),11U);goto _LL0;case 38:
 bad_form=_tag_fat("Valueof_e",sizeof(char),10U);goto _LL0;case 41:
 bad_form=_tag_fat("Assert_e",sizeof(char),9U);goto _LL0;default:
 bad_form=_tag_fat("huh??? Unexpected-form",sizeof(char),23U);goto _LL0;}_LL0:;}{struct Cyc_Warn_String_Warn_Warg_struct _T9;_T9.tag=0;
# 114
_T9.f1=_tag_fat("bad exp form ",sizeof(char),14U);_T3=_T9;}{struct Cyc_Warn_String_Warn_Warg_struct _T9=_T3;{struct Cyc_Warn_String_Warn_Warg_struct _TA;_TA.tag=0;_TA.f1=bad_form;_T4=_TA;}{struct Cyc_Warn_String_Warn_Warg_struct _TA=_T4;{struct Cyc_Warn_String_Warn_Warg_struct _TB;_TB.tag=0;_TB.f1=_tag_fat(" (exp=|",sizeof(char),8U);_T5=_TB;}{struct Cyc_Warn_String_Warn_Warg_struct _TB=_T5;{struct Cyc_Warn_Exp_Warn_Warg_struct _TC;_TC.tag=4;_TC.f1=e;_T6=_TC;}{struct Cyc_Warn_Exp_Warn_Warg_struct _TC=_T6;{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat("|) after xlation to C",sizeof(char),22U);_T7=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T7;void*_TE[5];_TE[0]=& _T9;_TE[1]=& _TA;_TE[2]=& _TB;_TE[3]=& _TC;_TE[4]=& _TD;_T8=_tag_fat(_TE,sizeof(void*),5);Cyc_Warn_impos2(_T8);}}}}}}struct _tuple13{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 118
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_exp_effect(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp*_T3;int*_T4;int _T5;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T6;void*_T7;int*_T8;unsigned _T9;void*_TA;struct Cyc_Absyn_Fndecl*_TB;struct _tuple1*_TC;enum Cyc_Toseqc_SideEffect _TD;void*_TE;struct Cyc_Absyn_Vardecl*_TF;struct _tuple1*_T10;enum Cyc_Toseqc_SideEffect _T11;void*_T12;enum Cyc_Toseqc_SideEffect _T13;struct Cyc_List_List*(*_T14)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*);struct Cyc_List_List*(*_T15)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T16)(struct _tuple13*);void*(*_T17)(struct _tuple0*);struct Cyc_List_List*_T18;enum Cyc_Toseqc_SideEffect _T19;struct Cyc_List_List*_T1A;struct _fat_ptr _T1B;enum Cyc_Toseqc_SideEffect _T1C;struct Cyc_List_List*_T1D;struct _fat_ptr _T1E;enum Cyc_Toseqc_SideEffect _T1F;enum Cyc_Toseqc_SideEffect _T20;enum Cyc_Toseqc_SideEffect _T21;int(*_T22)(struct Cyc_Absyn_Exp*);
struct _fat_ptr bad_form;_T0=e;{
void*_T23=_T0->r;struct Cyc_Absyn_Stmt*_T24;struct Cyc_Absyn_Exp*_T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Exp*_T28;_T1=(int*)_T23;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 18: _LL4: goto _LL6;case 17: _LL6: goto _LL8;case 19: _LL8: goto _LLA;case 32: _LLA: goto _LLC;case 31: _LLC:
# 126
 return 0U;case 1:
 return 1U;case 4: goto _LL12;case 39: _LL12:
# 130
 return 3U;case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}{struct Cyc_Absyn_Exp*e=_T28;_T3=e;{
# 133
void*_T29=_T3->r;struct _tuple1*_T2A;struct Cyc_Absyn_Vardecl*_T2B;struct Cyc_Absyn_Fndecl*_T2C;_T4=(int*)_T29;_T5=*_T4;if(_T5!=1)goto _TL29;_T6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T29;_T7=_T6->f1;_T8=(int*)_T7;_T9=*_T8;switch(_T9){case 2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T29;_TA=_T2D->f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T2E=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_TA;_T2C=_T2E->f1;}}{struct Cyc_Absyn_Fndecl*d=_T2C;_TB=d;_TC=_TB->name;_TD=
Cyc_Toseqc_fun_effect(_TC);return _TD;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T29;_TE=_T2D->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T2E=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TE;_T2B=_T2E->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T2B;_TF=vd;_T10=_TF->name;_T11=
Cyc_Toseqc_fun_effect(_T10);return _T11;}case 0:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T29;_T12=_T2D->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T2E=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T12;_T2A=_T2E->f1;}}{struct _tuple1*v=_T2A;_T13=
Cyc_Toseqc_fun_effect(v);return _T13;}default: goto _LL40;}goto _TL2A;_TL29: _LL40:
 return 3U;_TL2A:;}}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T23;_T27=_T29->f2;}{struct Cyc_List_List*dles=_T27;_T15=Cyc_List_map;{
# 140
struct Cyc_List_List*(*_T29)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))_T15;_T14=_T29;}_T17=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T29)(struct _tuple13*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple13*))_T17;_T16=_T29;}_T18=dles;_T27=_T14(_T16,_T18);goto _LL18;}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T23;_T27=_T29->f2;}_LL18: {struct Cyc_List_List*es=_T27;_T19=
Cyc_Toseqc_exps_effect(es);return _T19;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;_T26=_T29->f2;_T25=_T29->f3;}{struct Cyc_Absyn_Exp*e1=_T28;struct Cyc_Absyn_Exp*e2=_T26;struct Cyc_Absyn_Exp*e3=_T25;{struct Cyc_Absyn_Exp*_T29[3];
# 143
_T29[0]=e1;_T29[1]=e2;_T29[2]=e3;_T1B=_tag_fat(_T29,sizeof(struct Cyc_Absyn_Exp*),3);_T1A=Cyc_List_list(_T1B);}_T1C=Cyc_Toseqc_exps_effect(_T1A);return _T1C;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;_T26=_T29->f2;}{struct Cyc_Absyn_Exp*e1=_T28;struct Cyc_Absyn_Exp*e2=_T26;_T28=e1;_T26=e2;goto _LL1E;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;_T26=_T29->f2;}_LL1E: {struct Cyc_Absyn_Exp*e1=_T28;struct Cyc_Absyn_Exp*e2=_T26;_T28=e1;_T26=e2;goto _LL20;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;_T26=_T29->f2;}_LL20: {struct Cyc_Absyn_Exp*e1=_T28;struct Cyc_Absyn_Exp*e2=_T26;_T28=e1;_T26=e2;goto _LL22;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;_T26=_T29->f2;}_LL22: {struct Cyc_Absyn_Exp*e1=_T28;struct Cyc_Absyn_Exp*e2=_T26;{struct Cyc_Absyn_Exp*_T29[2];
# 148
_T29[0]=e1;_T29[1]=e2;_T1E=_tag_fat(_T29,sizeof(struct Cyc_Absyn_Exp*),2);_T1D=Cyc_List_list(_T1E);}_T1F=Cyc_Toseqc_exps_effect(_T1D);return _T1F;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}{struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL26;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL26: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL28;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL28: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL2A;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f2;}_LL2A: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL2C;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL2C: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL2E;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL2E: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL30;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL30: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL32;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL32: {struct Cyc_Absyn_Exp*e1=_T28;_T28=e1;goto _LL34;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T23;_T28=_T29->f1;}_LL34: {struct Cyc_Absyn_Exp*e1=_T28;_T20=
# 158
Cyc_Toseqc_exp_effect(e1);return _T20;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T29=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T23;_T24=_T29->f1;}{struct Cyc_Absyn_Stmt*s=_T24;_T21=
# 160
Cyc_Toseqc_stmt_effect(s);return _T21;}default:{
# 162
int(*_T29)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_Toseqc_unexpected_form;_T22=_T29;}_T22(e);};}}
# 165
static enum Cyc_Toseqc_SideEffect Cyc_Toseqc_stmt_effect(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;enum Cyc_Toseqc_SideEffect _T3;enum Cyc_Toseqc_SideEffect _T4;enum Cyc_Toseqc_SideEffect _T5;enum Cyc_Toseqc_SideEffect _T6;enum Cyc_Toseqc_SideEffect _T7;enum Cyc_Toseqc_SideEffect _T8;struct Cyc_Absyn_Decl*_T9;int*_TA;int _TB;struct Cyc_Absyn_Vardecl*_TC;struct Cyc_Absyn_Exp*_TD;unsigned _TE;enum Cyc_Toseqc_SideEffect _TF;struct Cyc_Absyn_Vardecl*_T10;struct Cyc_Absyn_Exp*_T11;enum Cyc_Toseqc_SideEffect _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;
enum Cyc_Toseqc_SideEffect res=0U;
_TL2C: if(1)goto _TL2D;else{goto _TL2E;}
_TL2D: _T0=s;{void*_T17=_T0->r;struct Cyc_Absyn_Decl*_T18;struct Cyc_Absyn_Stmt*_T19;struct Cyc_Absyn_Stmt*_T1A;struct Cyc_Absyn_Exp*_T1B;_T1=(int*)_T17;_T2=*_T1;switch(_T2){case 0: _T3=res;
return _T3;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T1C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T17;_T1B=_T1C->f1;}{struct Cyc_Absyn_Exp*e=_T1B;_T4=res;_T5=
Cyc_Toseqc_exp_effect(e);_T6=Cyc_Toseqc_join_side_effect(_T4,_T5);return _T6;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1C=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T17;_T1A=_T1C->f2;}{struct Cyc_Absyn_Stmt*s2=_T1A;
# 172
s=s2;goto _TL2C;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T1C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T17;_T1A=_T1C->f1;_T19=_T1C->f2;}{struct Cyc_Absyn_Stmt*s1=_T1A;struct Cyc_Absyn_Stmt*s2=_T19;_T7=res;_T8=
# 174
Cyc_Toseqc_stmt_effect(s1);res=Cyc_Toseqc_join_side_effect(_T7,_T8);
s=s2;goto _TL2C;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T1C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T17;_T18=_T1C->f1;_T1A=_T1C->f2;}{struct Cyc_Absyn_Decl*d=_T18;struct Cyc_Absyn_Stmt*s1=_T1A;_T9=d;{
# 179
void*_T1C=_T9->r;struct Cyc_Absyn_Vardecl*_T1D;_TA=(int*)_T1C;_TB=*_TA;if(_TB!=0)goto _TL30;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T1E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1C;_T1D=_T1E->f1;}{struct Cyc_Absyn_Vardecl*vd=_T1D;_TC=vd;_TD=_TC->initializer;_TE=(unsigned)_TD;
# 181
if(!_TE)goto _TL32;_TF=res;_T10=vd;_T11=_T10->initializer;_T12=
Cyc_Toseqc_exp_effect(_T11);res=Cyc_Toseqc_join_side_effect(_TF,_T12);goto _TL33;_TL32: _TL33:
 s=s1;goto _TL2C;}_TL30:
# 185
 return 3U;;}}case 6: goto _LLE;case 7: _LLE: goto _LL10;case 8: _LL10: goto _LL12;case 3: _LL12: goto _LL14;case 4: _LL14: goto _LL16;case 5: _LL16: goto _LL18;case 9: _LL18: goto _LL1A;case 14: _LL1A: goto _LL1C;case 10: _LL1C:
# 196
 return 3U;default:{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;
_T1C.f1=_tag_fat("bad stmt after xlation to C",sizeof(char),28U);_T13=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T13;void*_T1D[1];_T1D[0]=& _T1C;_T15=Cyc_Warn_impos2;{int(*_T1E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_T1E;}_T16=_tag_fat(_T1D,sizeof(void*),1);_T14(_T16);}};}goto _TL2C;_TL2E:;}
# 203
static int Cyc_Toseqc_is_toc_var(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;int _T6;void*_T7;_T0=e;{
void*_T8=_T0->r;struct _tuple1*_T9;_T1=(int*)_T8;_T2=*_T1;if(_T2!=1)goto _TL34;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T8;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL36;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T8;_T7=_TA->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_TB=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T7;_T9=_TB->f1;}}{struct _tuple1*v=_T9;
return 1;}_TL36: goto _LL3;_TL34: _LL3:
 return 0;;}}
# 210
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*);
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*);
# 228 "toseqc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toseqc_exps_to_seqc(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){void(*_T0)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);void(*_T1)(void(*)(void*),struct Cyc_List_List*);void(*_T2)(struct Cyc_Absyn_Exp*);struct Cyc_List_List*_T3;int _T4;enum Cyc_Toseqc_SideEffect _T5;int _T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_Exp*_T8;unsigned _T9;struct Cyc_List_List*_TA;void*_TB;int _TC;int _TD;int _TE;struct _tuple1*_TF;struct Cyc_Absyn_Exp*_T10;unsigned _T11;struct Cyc_Absyn_Exp*_T12;void*_T13;struct Cyc_Absyn_Exp*_T14;unsigned _T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Exp*_T19;void*_T1A;struct Cyc_Absyn_Exp*_T1B;void*_T1C;struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T1D;struct Cyc_Absyn_Exp*_T1E;void*_T1F;struct _tuple1*_T20;void*_T21;struct Cyc_Absyn_Exp*_T22;struct Cyc_Absyn_Stmt*_T23;struct Cyc_Absyn_Exp*_T24;unsigned _T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_List_List*_T28;int _T29;struct Cyc_Absyn_Exp*_T2A;struct Cyc_Absyn_Stmt*_T2B;struct Cyc_Absyn_Exp*_T2C;unsigned _T2D;struct Cyc_Absyn_Exp*_T2E;struct Cyc_Absyn_Stmt*_T2F;_T1=Cyc_List_iter;{
# 230
void(*_T30)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T1;_T0=_T30;}_T2=Cyc_Toseqc_exp_to_seqc;_T3=es;_T0(_T2,_T3);_T4=
# 233
Cyc_List_length(es);if(_T4 <= 1)goto _TL3A;else{goto _TL3B;}_TL3B: _T5=Cyc_Toseqc_exps_effect(es);_T6=(int)_T5;if(_T6!=3)goto _TL3A;else{goto _TL38;}_TL3A: return 0;_TL38: _T7=
# 238
Cyc_Absyn_copy_exp(e);_T8=e;_T9=_T8->loc;{struct Cyc_Absyn_Stmt*stmt=Cyc_Absyn_exp_stmt(_T7,_T9);
struct Cyc_Absyn_Stmt*laststmt=stmt;
int did_skip=0;
int did_convert=0;{
struct Cyc_List_List*x=Cyc_List_rev(es);_TL3F: if(x!=0)goto _TL3D;else{goto _TL3E;}
_TL3D: _TA=x;_TB=_TA->hd;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_TB;_TC=
# 246
Cyc_Tcutil_is_const_exp(e1);if(_TC)goto _TL42;else{goto _TL43;}_TL43: _TD=Cyc_Toseqc_is_toc_var(e1);if(_TD)goto _TL42;else{goto _TL40;}_TL42: goto _TL3C;_TL40: _TE=did_skip;
# 251
if(_TE)goto _TL44;else{goto _TL46;}
_TL46: did_skip=1;goto _TL3C;_TL44:
# 255
 did_convert=1;{
# 258
struct _tuple1*v=Cyc_Toc_temp_var();_TF=v;_T10=e1;_T11=_T10->loc;{
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_var_exp(_TF,_T11);_T12=e1;_T13=_T12->r;_T14=e1;_T15=_T14->loc;{
# 262
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_new_exp(_T13,_T15);_T16=e2;_T17=e1;
_T16->annot=_T17->annot;_T18=e2;_T19=e1;
_T18->topt=_T19->topt;_T1B=e2;_T1C=_T1B->topt;
# 266
if(_T1C!=0)goto _TL47;{struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_T30=_cycalloc(sizeof(struct Cyc_Absyn_TypeofType_Absyn_Type_struct));_T30->tag=11;_T30->f1=e2;_T1D=(struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_T30;}_T1A=(void*)_T1D;goto _TL48;_TL47: _T1E=e2;_T1F=_T1E->topt;_T1A=Cyc_Toc_typ_to_c(_T1F);_TL48: {void*t=_T1A;_T20=v;_T21=t;_T22=e2;_T23=stmt;_T24=e;_T25=_T24->loc;
stmt=Cyc_Absyn_declare_stmt(_T20,_T21,_T22,_T23,_T25);_T26=e1;_T27=ve;
# 270
_T26->r=_T27->r;}}}}}_TL3C: _T28=x;
# 242
x=_T28->tl;goto _TL3F;_TL3E:;}_T29=did_convert;
# 274
if(!_T29)goto _TL49;_T2A=e;_T2B=stmt;_T2C=e;_T2D=_T2C->loc;_T2E=
Cyc_Absyn_stmt_exp(_T2B,_T2D);_T2A->r=_T2E->r;_T2F=laststmt;
return _T2F;_TL49:
# 278
 return 0;}}
# 281
static void Cyc_Toseqc_exp_to_seqc(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_List_List*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_List_List*_T5;struct _fat_ptr _T6;struct Cyc_Absyn_Exp*_T7;int*_T8;int _T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_List_List*_TB;struct _fat_ptr _TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_List_List*(*_TE)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*);struct Cyc_List_List*(*_TF)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T10)(struct _tuple13*);void*(*_T11)(struct _tuple0*);struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;int(*_T14)(struct Cyc_Absyn_Exp*);_T0=e;{
void*_T15=_T0->r;struct Cyc_Absyn_Stmt*_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_Core_Opt*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_Exp*_T1B;_T1=(int*)_T15;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 1: _LL4:
# 284
 return;case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T1A=_T1C->f2;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_List_List*es=_T1A;{struct Cyc_List_List*_T1C=_cycalloc(sizeof(struct Cyc_List_List));
# 286
_T1C->hd=e1;_T1C->tl=es;_T3=(struct Cyc_List_List*)_T1C;}_T1A=_T3;goto _LL8;}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T15;_T1A=_T1C->f2;}_LL8: {struct Cyc_List_List*es=_T1A;
Cyc_Toseqc_exps_to_seqc(e,es);return;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T19=_T1C->f2;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T19;_T4=e;{struct Cyc_Absyn_Exp*_T1C[2];
_T1C[0]=e1;_T1C[1]=e2;_T6=_tag_fat(_T1C,sizeof(struct Cyc_Absyn_Exp*),2);_T5=Cyc_List_list(_T6);}Cyc_Toseqc_exps_to_seqc(_T4,_T5);return;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T18=_T1C->f2;_T19=_T1C->f3;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Core_Opt*p=_T18;struct Cyc_Absyn_Exp*e2=_T19;
# 291
if(p!=0)goto _TL4C;_T7=e1;{
void*_T1C=_T7->r;_T8=(int*)_T1C;_T9=*_T8;if(_T9!=1)goto _TL4E;
Cyc_Toseqc_exp_to_seqc(e2);return;_TL4E: goto _LL39;_LL39:;}goto _TL4D;_TL4C: _TL4D: _TA=e;{struct Cyc_Absyn_Exp*_T1C[2];
# 297
_T1C[0]=e2;_T1C[1]=e1;_TC=_tag_fat(_T1C,sizeof(struct Cyc_Absyn_Exp*),2);_TB=Cyc_List_list(_TC);}Cyc_Toseqc_exps_to_seqc(_TA,_TB);
return;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T19=_T1C->f2;_T17=_T1C->f3;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T19;struct Cyc_Absyn_Exp*e3=_T17;
# 300
Cyc_Toseqc_exp_to_seqc(e3);_T1B=e1;_T19=e2;goto _LL10;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T19=_T1C->f2;}_LL10: {struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T19;_T1B=e1;_T19=e2;goto _LL12;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T19=_T1C->f2;}_LL12: {struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T19;_T1B=e1;_T19=e2;goto _LL14;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;_T19=_T1C->f2;}_LL14: {struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T19;
# 303
Cyc_Toseqc_exp_to_seqc(e2);_T1B=e1;goto _LL16;}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL16: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL18;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL18: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL1A;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL1A: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL1C;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f2;}_LL1C: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL1E;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL1E: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL20;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL20: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL22;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL22: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL24;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL24: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL26;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL26: {struct Cyc_Absyn_Exp*e1=_T1B;_T1B=e1;goto _LL28;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T15;_T1B=_T1C->f1;}_LL28: {struct Cyc_Absyn_Exp*e1=_T1B;
# 314
Cyc_Toseqc_exp_to_seqc(e1);return;}case 17: goto _LL2C;case 19: _LL2C: goto _LL2E;case 32: _LL2E: goto _LL30;case 31: _LL30:
# 319
 return;case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T15;_T1A=_T1C->f2;}{struct Cyc_List_List*dles=_T1A;_TD=e;_TF=Cyc_List_map;{
# 321
struct Cyc_List_List*(*_T1C)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))_TF;_TE=_T1C;}_T11=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T1C)(struct _tuple13*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple13*))_T11;_T10=_T1C;}_T12=dles;_T13=_TE(_T10,_T12);Cyc_Toseqc_exps_to_seqc(_TD,_T13);return;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T15;_T16=_T1C->f1;}{struct Cyc_Absyn_Stmt*s=_T16;
# 323
Cyc_Toseqc_stmt_to_seqc(s);return;}case 39:
# 325
 return;default:{
int(*_T1C)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_Toseqc_unexpected_form;_T14=_T1C;}_T14(e);};}}
# 330
static void Cyc_Toseqc_stmt_to_seqc(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct _tuple10 _T3;struct _tuple10 _T4;struct _tuple10 _T5;struct _tuple10 _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_Absyn_Switch_clause*_T9;struct Cyc_Absyn_Stmt*_TA;struct Cyc_List_List*_TB;struct Cyc_Absyn_Decl*_TC;int*_TD;int _TE;struct Cyc_Absyn_Vardecl*_TF;struct Cyc_Absyn_Exp*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_Absyn_Exp*_T12;int*_T13;int _T14;struct Cyc_Absyn_Vardecl*_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_List_List*(*_T17)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*);struct Cyc_List_List*(*_T18)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T19)(struct _tuple13*);void*(*_T1A)(struct _tuple0*);struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Stmt*_T1D;int*_T1E;int _T1F;struct Cyc_Absyn_Vardecl*_T20;struct Cyc_Absyn_Exp*_T21;struct Cyc_Absyn_Exp*_T22;int*_T23;int _T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Stmt*_T26;struct Cyc_Absyn_Stmt*_T27;struct Cyc_Absyn_Stmt*_T28;struct Cyc_Absyn_Stmt*_T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;struct Cyc_Warn_Exp_Warn_Warg_struct _T2B;struct Cyc_Absyn_Vardecl*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Warn_String_Warn_Warg_struct _T2E;int(*_T2F)(struct _fat_ptr);void*(*_T30)(struct _fat_ptr);struct _fat_ptr _T31;struct Cyc_Warn_String_Warn_Warg_struct _T32;struct Cyc_Warn_Stmt_Warn_Warg_struct _T33;struct Cyc_Warn_String_Warn_Warg_struct _T34;int(*_T35)(struct _fat_ptr);void*(*_T36)(struct _fat_ptr);struct _fat_ptr _T37;struct Cyc_Absyn_Vardecl*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;int(*_T3C)(struct _fat_ptr);void*(*_T3D)(struct _fat_ptr);struct _fat_ptr _T3E;
# 332
_TL50: if(1)goto _TL51;else{goto _TL52;}
_TL51: _T0=s;{void*_T3F=_T0->r;struct Cyc_Absyn_Decl*_T40;struct Cyc_List_List*_T41;struct Cyc_Absyn_Exp*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Stmt*_T44;struct Cyc_Absyn_Exp*_T45;struct Cyc_Absyn_Stmt*_T46;_T1=(int*)_T3F;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 6: _LL4: goto _LL6;case 7: _LL6: goto _LL8;case 8: _LL8:
# 337
 return;case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T3F;_T46=_T47->f2;}{struct Cyc_Absyn_Stmt*s2=_T46;
s=s2;goto _TL50;}case 3:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T3F;_T45=_T47->f1;}{struct Cyc_Absyn_Exp*eopt=_T45;
# 340
if(eopt!=0)goto _TL54;
return;_TL54: _T45=eopt;goto _LLE;}case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T3F;_T45=_T47->f1;}_LLE: {struct Cyc_Absyn_Exp*e=_T45;
# 344
Cyc_Toseqc_exp_to_seqc(e);
# 352
return;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T3F;_T45=_T47->f1;_T46=_T47->f2;_T44=_T47->f3;}{struct Cyc_Absyn_Exp*e=_T45;struct Cyc_Absyn_Stmt*s1=_T46;struct Cyc_Absyn_Stmt*s2=_T44;
# 354
Cyc_Toseqc_exp_to_seqc(e);_T46=s1;_T44=s2;goto _LL12;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T3F;_T46=_T47->f1;_T44=_T47->f2;}_LL12: {struct Cyc_Absyn_Stmt*s1=_T46;struct Cyc_Absyn_Stmt*s2=_T44;
# 357
Cyc_Toseqc_stmt_to_seqc(s1);
s=s2;goto _TL50;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T3F;_T46=_T47->f1;_T3=_T47->f2;_T45=_T3.f0;}{struct Cyc_Absyn_Stmt*s2=_T46;struct Cyc_Absyn_Exp*e=_T45;_T45=e;_T46=s2;goto _LL16;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T3F;_T4=_T47->f1;_T45=_T4.f0;_T46=_T47->f2;}_LL16: {struct Cyc_Absyn_Exp*e=_T45;struct Cyc_Absyn_Stmt*s2=_T46;
# 362
Cyc_Toseqc_exp_to_seqc(e);
s=s2;goto _TL50;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T3F;_T45=_T47->f1;_T5=_T47->f2;_T43=_T5.f0;_T6=_T47->f3;_T42=_T6.f0;_T46=_T47->f4;}{struct Cyc_Absyn_Exp*e1=_T45;struct Cyc_Absyn_Exp*e2=_T43;struct Cyc_Absyn_Exp*e3=_T42;struct Cyc_Absyn_Stmt*s2=_T46;
# 366
Cyc_Toseqc_exp_to_seqc(e1);
Cyc_Toseqc_exp_to_seqc(e2);
Cyc_Toseqc_exp_to_seqc(e3);
s=s2;goto _TL50;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T3F;_T45=_T47->f1;_T41=_T47->f2;}{struct Cyc_Absyn_Exp*e=_T45;struct Cyc_List_List*scs=_T41;
# 374
Cyc_Toseqc_exp_to_seqc(e);
_TL59: if(scs!=0)goto _TL57;else{goto _TL58;}
_TL57: _T7=scs;_T8=_T7->hd;_T9=(struct Cyc_Absyn_Switch_clause*)_T8;_TA=_T9->body;Cyc_Toseqc_stmt_to_seqc(_TA);_TB=scs;
# 375
scs=_TB->tl;goto _TL59;_TL58:
# 377
 return;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T47=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T3F;_T40=_T47->f1;_T46=_T47->f2;}{struct Cyc_Absyn_Decl*d=_T40;struct Cyc_Absyn_Stmt*s2=_T46;_TC=d;{
# 379
void*_T47=_TC->r;struct Cyc_Absyn_Vardecl*_T48;_TD=(int*)_T47;_TE=*_TD;if(_TE!=0)goto _TL5A;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T49=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T47;_T48=_T49->f1;}{struct Cyc_Absyn_Vardecl*vd=_T48;_TF=vd;_T10=_TF->initializer;
# 381
if(_T10==0)goto _TL5C;_T11=vd;_T12=_T11->initializer;{
# 384
void*_T49=_T12->r;struct Cyc_List_List*_T4A;_T13=(int*)_T49;_T14=*_T13;if(_T14!=35)goto _TL5E;{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T49;_T4A=_T4B->f2;}{struct Cyc_List_List*dles=_T4A;_T15=vd;_T16=_T15->initializer;_T18=Cyc_List_map;{
# 398 "toseqc.cyc"
struct Cyc_List_List*(*_T4B)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple13*),struct Cyc_List_List*))_T18;_T17=_T4B;}_T1A=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T4B)(struct _tuple13*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple13*))_T1A;_T19=_T4B;}_T1B=dles;_T1C=_T17(_T19,_T1B);{
# 397
struct Cyc_Absyn_Stmt*laststmt=Cyc_Toseqc_exps_to_seqc(_T16,_T1C);
# 400
if(laststmt!=0)goto _TL60;goto _LL24;_TL60: _T1D=laststmt;{
# 406
void*_T4B=_T1D->r;struct Cyc_Absyn_Exp*_T4C;_T1E=(int*)_T4B;_T1F=*_T1E;if(_T1F!=1)goto _TL62;{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T4D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T4B;_T4C=_T4D->f1;}{struct Cyc_Absyn_Exp*initexp=_T4C;_T20=vd;_T21=_T20->initializer;_T22=
# 408
_check_null(_T21);{void*_T4D=_T22->r;struct Cyc_Absyn_Stmt*_T4E;_T23=(int*)_T4D;_T24=*_T23;if(_T24!=36)goto _TL64;{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T4F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T4D;_T4E=_T4F->f1;}{struct Cyc_Absyn_Stmt*s3=_T4E;_T25=vd;
# 410
_T25->initializer=initexp;_T26=laststmt;_T27=s;
_T26->r=_T27->r;_T28=s;_T29=s3;
_T28->r=_T29->r;goto _LL2E;}_TL64:{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;
# 415
_T4F.f1=_tag_fat("exps_to_seqc updated to non-stmt-exp |",sizeof(char),39U);_T2A=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T2A;{struct Cyc_Warn_Exp_Warn_Warg_struct _T50;_T50.tag=4;_T2C=vd;_T2D=_T2C->initializer;
_T50.f1=_check_null(_T2D);_T2B=_T50;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T50=_T2B;{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;_T51.f1=_tag_fat("|",sizeof(char),2U);_T2E=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T2E;void*_T52[3];_T52[0]=& _T4F;_T52[1]=& _T50;_T52[2]=& _T51;_T30=Cyc_Warn_impos2;{
# 415
int(*_T53)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T30;_T2F=_T53;}_T31=_tag_fat(_T52,sizeof(void*),3);_T2F(_T31);}}}_LL2E:;}goto _LL29;}_TL62:{struct Cyc_Warn_String_Warn_Warg_struct _T4D;_T4D.tag=0;
# 420
_T4D.f1=_tag_fat("exps_to_seqc returned non-exp-stmt |",sizeof(char),37U);_T32=_T4D;}{struct Cyc_Warn_String_Warn_Warg_struct _T4D=_T32;{struct Cyc_Warn_Stmt_Warn_Warg_struct _T4E;_T4E.tag=5;
_T4E.f1=laststmt;_T33=_T4E;}{struct Cyc_Warn_Stmt_Warn_Warg_struct _T4E=_T33;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;_T4F.f1=_tag_fat("|",sizeof(char),2U);_T34=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T34;void*_T50[3];_T50[0]=& _T4D;_T50[1]=& _T4E;_T50[2]=& _T4F;_T36=Cyc_Warn_impos2;{
# 420
int(*_T51)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T36;_T35=_T51;}_T37=_tag_fat(_T50,sizeof(void*),3);_T35(_T37);}}}_LL29:;}goto _LL24;}}_TL5E: _T38=vd;_T39=_T38->initializer;_T3A=
# 426
_check_null(_T39);Cyc_Toseqc_exp_to_seqc(_T3A);goto _LL24;_LL24:;}goto _TL5D;_TL5C: _TL5D:
# 429
 s=s2;goto _TL50;}_TL5A: goto _LL1F;_LL1F:;}goto _LL1E;}default: _LL1E:{struct Cyc_Warn_String_Warn_Warg_struct _T47;_T47.tag=0;
# 434
_T47.f1=_tag_fat("bad stmt after xlation to C",sizeof(char),28U);_T3B=_T47;}{struct Cyc_Warn_String_Warn_Warg_struct _T47=_T3B;void*_T48[1];_T48[0]=& _T47;_T3D=Cyc_Warn_impos2;{int(*_T49)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3D;_T3C=_T49;}_T3E=_tag_fat(_T48,sizeof(void*),1);_T3C(_T3E);}};}goto _TL50;_TL52:;}
# 438
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*ds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Fndecl*_T5;enum Cyc_Absyn_Scope _T6;int _T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_Stmt*_T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;int(*_TB)(struct _fat_ptr);void*(*_TC)(struct _fat_ptr);struct _fat_ptr _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;{
struct Cyc_List_List*old_ds=ds;_TL69: if(old_ds!=0)goto _TL67;else{goto _TL68;}
_TL67: _T0=old_ds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T10=_T2->r;struct Cyc_Absyn_Fndecl*_T11;_T3=(int*)_T10;_T4=*_T3;switch(_T4){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T12=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T10;_T11=_T12->f1;}{struct Cyc_Absyn_Fndecl*fd=_T11;_T5=fd;_T6=_T5->orig_scope;_T7=(int)_T6;
# 442
if(_T7==4)goto _TL6B;_T8=fd;_T9=_T8->body;
Cyc_Toseqc_stmt_to_seqc(_T9);goto _TL6C;_TL6B: _TL6C: goto _LL0;}case 0: goto _LL6;case 5: _LL6: goto _LL8;case 7: _LL8: goto _LLA;case 8: _LLA: goto _LL0;case 6: goto _LLE;case 2: _LLE: goto _LL10;case 3: _LL10: goto _LL12;case 4: _LL12: goto _LL14;case 9: _LL14: goto _LL16;case 10: _LL16: goto _LL18;case 11: _LL18: goto _LL1A;case 12: _LL1A: goto _LL1C;case 13: _LL1C: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22:{struct Cyc_Warn_String_Warn_Warg_struct _T12;_T12.tag=0;
# 460
_T12.f1=_tag_fat("unexpected toplevel decl in toseqc",sizeof(char),35U);_TA=_T12;}{struct Cyc_Warn_String_Warn_Warg_struct _T12=_TA;void*_T13[1];_T13[0]=& _T12;_TC=Cyc_Warn_impos2;{int(*_T14)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TC;_TB=_T14;}_TD=_tag_fat(_T13,sizeof(void*),1);_TB(_TD);}}_LL0:;}_TE=old_ds;
# 439
old_ds=_TE->tl;goto _TL69;_TL68:;}_TF=ds;
# 462
return _TF;}
