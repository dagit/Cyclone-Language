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
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 72 "list.h"
extern struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 71
extern struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*,struct Cyc_Set_Set*,void*);
# 143
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*,struct Cyc_Set_Set*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 68 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 277 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};
# 529 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 923 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 944
void*Cyc_Absyn_compress(void*);
# 979
extern void*Cyc_Absyn_void_type;
# 1000
extern struct _tuple0*Cyc_Absyn_exn_name;
# 1030
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1100
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1102
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1105
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1107
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1113
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1117
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1120
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1139
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
# 1141
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1152
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1165
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1237
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 160 "tcutil.h"
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);
extern struct _fat_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 73 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple11{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 101
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
{void*_Tmp0=e->r;void*_Tmp1;int _Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 105
struct _tuple0*_Tmp6=Cyc_Absyn_binding2qvar(b);void*_Tmp7;if(((struct _tuple0*)_Tmp6)->f0.Loc_n.tag==4){_Tmp7=_Tmp6->f1;{struct _fat_ptr*v=_Tmp7;
# 109
if(({int(*_Tmp8)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;_Tmp8;})(env.varmap,v)){
({void*_Tmp8=({struct Cyc_Absyn_Exp*(*_Tmp9)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;_Tmp9;})(env.varmap,v)->r;e->r=_Tmp8;});
continue;}
# 114
{int i=0;for(0;(unsigned)i < _get_fat_size(Cyc_Toc_globals,sizeof(struct _fat_ptr*));++ i){
if(Cyc_strptrcmp(v,((struct _fat_ptr**)Cyc_Toc_globals.curr)[i])==0)
return 0;}}
# 118
for(1;env.boundvars!=0;env.boundvars=env.boundvars->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)env.boundvars->hd)->name).f1)==0)
return 0;}
# 122
for(1;env.all_locals!=0;env.all_locals=env.all_locals->tl){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)env.all_locals->hd;
if(Cyc_strptrcmp(v,(*vd->name).f1)==0){
({struct Cyc_Set_Set*_Tmp8=({struct Cyc_Set_Set*(*_Tmp9)(struct _RegionHandle*,struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))Cyc_Set_rinsert;_Tmp9;})(env.rgn,*env.freevars,vd);*env.freevars=_Tmp8;});{
# 127
void*_Tmp8=vd->type;if(*((int*)_Tmp8)==5)
return 0;else{
return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);};}}}
# 136
for(1;env.all_locals!=0;env.all_locals=env.all_locals->tl){
if(Cyc_strptrcmp(v,(*((struct Cyc_Absyn_Vardecl*)env.all_locals->hd)->name).f1)==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("unbound variable ",sizeof(char),18U);_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=*v;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat(" found in box_free_vars_exp",sizeof(char),28U);_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),3));});}
# 142
return 0;}}else{
return 0;};}case 23: _Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 149
struct Cyc_Absyn_Exp*e1b=Cyc_Tovc_box_free_vars_exp(env,e1);
struct Cyc_Absyn_Exp*e2b=Cyc_Tovc_box_free_vars_exp(env,e2);
if(e1b!=0){
if(e2b!=0){
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_subscript_exp(e1b,e2b,e->loc);
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}else{
# 159
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_subscript_exp(e1b,e2,e->loc);
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}else{
# 164
if(e2b!=0){
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_subscript_exp(e1,e2b,e->loc);
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}}
# 170
return 0;}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 173
struct Cyc_Absyn_Exp*e1b=Cyc_Tovc_box_free_vars_exp(env,e1);
if(e1b!=0){
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_address_exp(e1b,e->loc);
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 180
return 0;}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 183
struct Cyc_Absyn_Exp*e1b=Cyc_Tovc_box_free_vars_exp(env,e1);
if(e1b!=0){
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_deref_exp(e1b,e->loc);
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 190
return 0;}case 21: _Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*fn=_Tmp4;int ist=_Tmp3;int isr=_Tmp2;
# 193
struct Cyc_Absyn_Exp*e1b=Cyc_Tovc_box_free_vars_exp(env,e1);
if(e1b!=0){
struct Cyc_Absyn_Exp*eb=({void*_Tmp6=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_Tmp7->tag=21,_Tmp7->f1=e1b,_Tmp7->f2=fn,_Tmp7->f3=ist,_Tmp7->f4=isr;_Tmp7;});Cyc_Absyn_new_exp(_Tmp6,e->loc);});
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 200
return 0;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*fn=_Tmp4;int ist=_Tmp3;int isr=_Tmp2;
# 203
struct Cyc_Absyn_Exp*e1b=Cyc_Tovc_box_free_vars_exp(env,e1);
if(e1b!=0){
struct Cyc_Absyn_Exp*eb=({void*_Tmp6=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct));_Tmp7->tag=22,_Tmp7->f1=e1b,_Tmp7->f2=fn,_Tmp7->f3=ist,_Tmp7->f4=isr;_Tmp7;});Cyc_Absyn_new_exp(_Tmp6,e->loc);});
eb->topt=e->topt;
eb->annot=e->annot;
return eb;}
# 210
return 0;}case 0:
# 213
 return 0;case 10: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_List_List*es=_Tmp4;
# 215
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e1);if(__ei!=0)*e1=__ei;}
_Tmp5=es;goto _LL12;}case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_List_List*es=_Tmp5;
# 218
for(1;es!=0;es=es->tl){
struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)es->hd);if(__ei!=0)es->hd=(void*)__ei;}
# 221
return 0;}case 6: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_Absyn_Exp**e2=_Tmp4;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_Tmp1;
# 224
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e3);if(__ei!=0)*e3=__ei;}
_Tmp5=e1;_Tmp4=e2;goto _LL16;}case 7: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL16: {struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_Absyn_Exp**e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL18;}case 8: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL18: {struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_Absyn_Exp**e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1A;}case 9: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_Absyn_Exp**e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1C;}case 4: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL1C: {struct Cyc_Absyn_Exp**e1=_Tmp5;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_Tmp4;
# 230
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e2);if(__ei!=0)*e2=__ei;}
_Tmp5=e1;goto _LL1E;}case 12: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp**e1=_Tmp5;
# 233
_Tmp5=e1;goto _LL20;}case 13: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp**e1=_Tmp5;
_Tmp5=e1;goto _LL22;}case 14: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL22: {struct Cyc_Absyn_Exp**e1=_Tmp5;
_Tmp5=e1;goto _LL24;}case 18: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp**e1=_Tmp5;
_Tmp5=e1;goto _LL26;}case 5: _Tmp5=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_Absyn_Exp**e1=_Tmp5;
# 238
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e1);if(__ei!=0)*e1=__ei;}
return 0;}case 17:
# 241
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 31: _LL2C:
 return 0;case 35: _Tmp5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp5;
# 246
for(1;dles!=0;dles=dles->tl){
struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple11*)dles->hd)).f1);if(__ei!=0)(*((struct _tuple11*)dles->hd)).f1=__ei;}
# 249
return 0;}default:
# 251
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("bad exp form in Tocv::box_free_vars_exp",sizeof(char),40U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});};}
# 103
1U;}}
# 255
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 259
while(1){
{void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return;case 6:
# 263
 Cyc_Warn_warn(s->loc,_tag_fat("break in eliminated statement expression",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));
return;case 7:
# 266
 Cyc_Warn_warn(s->loc,_tag_fat("continue in eliminated statement expression",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
# 269
return;case 8:
# 271
 Cyc_Warn_warn(s->loc,_tag_fat("goto in eliminated statement expression",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
return;case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 274
s=s2;
continue;}case 3: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp**eopt=_Tmp4;
# 277
Cyc_Warn_warn(s->loc,_tag_fat("return in eliminated statement expression",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));
# 279
if(*eopt!=0){
struct Cyc_Absyn_Exp*ei=Cyc_Tovc_box_free_vars_exp(env,*eopt);
if(ei!=0)*eopt=ei;}
# 283
return;}case 1: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp**e=_Tmp4;
# 285
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e);if(__ei!=0)*e=__ei;}
return;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 288
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 4: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 292
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e);if(__ei!=0)*e=__ei;}
Cyc_Tovc_box_free_vars_stmt(env,s1);
s=s2;
continue;}case 5: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp**e=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 297
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e);if(__ei!=0)*e=__ei;}
s=s2;
continue;}case 9: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp**e1=_Tmp4;struct Cyc_Absyn_Exp**e2=_Tmp3;struct Cyc_Absyn_Exp**e3=(struct Cyc_Absyn_Exp**)_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 301
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e1);if(__ei!=0)*e1=__ei;}
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e2);if(__ei!=0)*e2=__ei;}
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e3);if(__ei!=0)*e3=__ei;}
s=s2;
continue;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s2=_Tmp4;struct Cyc_Absyn_Exp**e=_Tmp3;
# 307
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e);if(__ei!=0)*e=__ei;}
s=s2;
continue;}case 10: _Tmp4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp**e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
# 312
{struct Cyc_Absyn_Exp*__ei=Cyc_Tovc_box_free_vars_exp(env,*e);if(__ei!=0)*e=__ei;}
for(1;scs!=0;scs=scs->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 317
{void*_Tmp5=d->r;void*_Tmp6;if(*((int*)_Tmp5)==0){_Tmp6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 319
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_region_malloc(env.rgn,0U,sizeof(struct Cyc_List_List));_Tmp8->hd=vd,_Tmp8->tl=env.boundvars;_Tmp8;});env.boundvars=_Tmp7;});
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_rdelete_same;_Tmp8;})(env.varmap,(*vd->name).f1);env.varmap=_Tmp7;});
if(vd->initializer!=0){
struct Cyc_Absyn_Exp*ei=Cyc_Tovc_box_free_vars_exp(env,vd->initializer);
if(ei!=0)vd->initializer=ei;}
# 325
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 329
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad stmt after xlation to C",sizeof(char),28U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),1));});};}
# 260
1U;}}
# 340
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 344
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 347
struct Cyc_Set_Set*freevars=({struct Cyc_Set_Set*(*_Tmp0)(struct _RegionHandle*,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty;_Tmp0;})(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv boxenv=({struct Cyc_Tovc_BoxingEnv _Tmp0;_Tmp0.rgn=env.rgn,_Tmp0.all_locals=env.all_locals,_Tmp0.varmap=env.varmap,_Tmp0.boundvars=0,_Tmp0.freevars=& freevars;_Tmp0;});
Cyc_Tovc_box_free_vars_stmt(boxenv,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 353
struct Cyc_Iter_Iter iter=Cyc_Set_make_iter(Cyc_Core_heap_region,freevars);
# 355
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),vd->sc=2U,vd->name=Cyc_Absyn_exn_name,vd->varloc=0U,({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);vd->tq=_Tmp0;}),vd->type=Cyc_Absyn_void_type,vd->initializer=0,vd->rgn=0,vd->attributes=0,vd->escapes=0,vd->is_proto=0,vd->rename=0;
# 358
while(({int(*_Tmp0)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**)=(int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next;_Tmp0;})(iter,& vd)){
{void*_Tmp0=vd->type;void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*elt_type=_Tmp1;
# 361
params=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp3=({struct _tuple8*_Tmp4=_cycalloc(sizeof(struct _tuple8));_Tmp4->f0=(*vd->name).f1,_Tmp4->f1=vd->tq,({
void*_Tmp5=({void*_Tmp6=elt_type;Cyc_Absyn_cstar_type(_Tmp6,Cyc_Absyn_empty_tqual(0U));});_Tmp4->f2=_Tmp5;});_Tmp4;});
# 361
_Tmp2->hd=_Tmp3;}),_Tmp2->tl=params;_Tmp2;});
# 364
args=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp3=Cyc_Absyn_var_exp(vd->name,0U);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=args;_Tmp2;});
goto _LL0;}}else{
# 367
params=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp3=({struct _tuple8*_Tmp4=_cycalloc(sizeof(struct _tuple8));_Tmp4->f0=(*vd->name).f1,_Tmp4->f1=vd->tq,({
void*_Tmp5=({void*_Tmp6=vd->type;Cyc_Absyn_cstar_type(_Tmp6,Cyc_Absyn_empty_tqual(0U));});_Tmp4->f2=_Tmp5;});_Tmp4;});
# 367
_Tmp2->hd=_Tmp3;}),_Tmp2->tl=params;_Tmp2;});
# 370
args=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp3=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(vd->name,0U),0U);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=args;_Tmp2;});
goto _LL0;}_LL0:;}
# 359
1U;}{
# 373
struct _tuple0*funname=Cyc_Toc_temp_var();
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));
({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=({struct Cyc_Absyn_Fndecl*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp5->sc=0U,_Tmp5->is_inline=0,_Tmp5->name=funname,_Tmp5->body=s,
_Tmp5->i.tvars=0,_Tmp5->i.effect=0,({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);_Tmp5->i.ret_tqual=_Tmp6;}),_Tmp5->i.ret_type=rettype,_Tmp5->i.args=params,_Tmp5->i.c_varargs=0,_Tmp5->i.cyc_varargs=0,_Tmp5->i.qual_bnd=0,_Tmp5->i.attributes=0,_Tmp5->i.checks_clause=0,_Tmp5->i.checks_assn=0,_Tmp5->i.requires_clause=0,_Tmp5->i.requires_assn=0,_Tmp5->i.ensures_clause=0,_Tmp5->i.ensures_assn=0,_Tmp5->i.throws_clause=0,_Tmp5->i.throws_assn=0,_Tmp5->i.return_value=0,_Tmp5->i.arg_vardecls=0,_Tmp5->i.effconstr=0,_Tmp5->cached_type=0,_Tmp5->param_vardecls=0,_Tmp5->fn_vardecl=0,_Tmp5->orig_scope=0U,_Tmp5->escapes=0;_Tmp5;});
# 375
_Tmp3->f1=_Tmp4;});_Tmp3;}),0U);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*env.gen_ds;_Tmp1;});
# 374
*env.gen_ds=_Tmp0;});{
# 383
struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_var_exp(funname,0U);return Cyc_Absyn_fncall_exp(_Tmp0,args,0U);}}}}
# 386
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp3;
# 389
struct _tuple0*_Tmp4=Cyc_Absyn_binding2qvar(b);void*_Tmp5;if(((struct _tuple0*)_Tmp4)->f0.Loc_n.tag==4){_Tmp5=_Tmp4->f1;{struct _fat_ptr*v=_Tmp5;
# 391
if(({int(*_Tmp6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;_Tmp6;})(varmap,v))
({void*_Tmp6=({struct Cyc_Absyn_Exp*(*_Tmp7)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;_Tmp7;})(varmap,v)->r;e->r=_Tmp6;});
return;}}else{
return;};}case 0:
# 398
 return;case 10: _Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_List_List*es=_Tmp2;
# 400
Cyc_Tovc_apply_varmap(varmap,e1);_Tmp3=es;goto _LL8;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List*es=_Tmp3;
# 402
for(1;es!=0;es=es->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 407
Cyc_Tovc_apply_varmap(varmap,e3);_Tmp3=e1;_Tmp2=e2;goto _LLC;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLE;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL10;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL12;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL14;}case 4: _Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 413
Cyc_Tovc_apply_varmap(varmap,e2);_Tmp3=e1;goto _LL16;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp3;
# 415
_Tmp3=e1;goto _LL18;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1A;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1C;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1E;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL20;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL22;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL24;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL26;}case 5: _Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_Tmp3;
# 424
Cyc_Tovc_apply_varmap(varmap,e1);
return;}case 17:
# 427
 goto _LL2A;case 19: _LL2A:
 goto _LL2C;case 31: _LL2C:
 return;case 35: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp3;
# 432
for(1;dles!=0;dles=dles->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple11*)dles->hd)).f1);}
return;}default:
# 436
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad exp form in Tocv::apply_varmap",sizeof(char),35U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});};}
# 440
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 return Cyc_Absyn_true_exp(0U);case 1: _Tmp3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 444
Cyc_Tovc_apply_varmap(env.varmap,e);
return e;}case 2: _Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 448
struct Cyc_Absyn_Exp*s1_new=Cyc_Tovc_stmt_to_exp(env,s1);
struct Cyc_Absyn_Exp*s2_new=Cyc_Tovc_stmt_to_exp(env,s2);
return Cyc_Absyn_seq_exp(s1_new,s2_new,0U);}case 4: _Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp3;struct Cyc_Absyn_Stmt*s1=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 452
Cyc_Tovc_apply_varmap(env.varmap,e);{
# 454
struct Cyc_Absyn_Exp*s1_new=Cyc_Tovc_stmt_to_exp(env,s1);
struct Cyc_Absyn_Exp*s2_new=Cyc_Tovc_stmt_to_exp(env,s2);
return Cyc_Absyn_conditional_exp(e,s1_new,s2_new,0U);}}case 12: _Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 458
void*_Tmp4=d->r;void*_Tmp5;if(*((int*)_Tmp4)==0){_Tmp5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 461
struct _fat_ptr*oldname=(*vd->name).f1;
struct _tuple0*newqvar=Cyc_Toc_temp_var();
vd->name=newqvar;
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*)=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,struct Cyc_Absyn_Exp*))Cyc_Dict_insert;_Tmp8;});struct Cyc_Dict_Dict _Tmp8=env.varmap;struct _fat_ptr*_Tmp9=oldname;_Tmp7(_Tmp8,_Tmp9,Cyc_Absyn_var_exp(newqvar,0U));});env.varmap=_Tmp6;});{
struct Cyc_List_List*unresolvedmem_inits=0;
if(vd->initializer!=0){
Cyc_Tovc_apply_varmap(env.varmap,vd->initializer);{
void*_Tmp6=_check_null(vd->initializer)->r;void*_Tmp7;if(*((int*)_Tmp6)==35){_Tmp7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp6)->f2;{struct Cyc_List_List*dles=_Tmp7;
# 480 "tovc.cyc"
struct Cyc_Absyn_Exp*var=Cyc_Absyn_var_exp(vd->name,0U);
{unsigned i=0U;for(0;dles!=0;(dles=dles->tl,++ i)){
unresolvedmem_inits=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp9=({struct Cyc_Absyn_Exp*_TmpA=({struct Cyc_Absyn_Exp*_TmpB=var;Cyc_Absyn_subscript_exp(_TmpB,Cyc_Absyn_uint_exp(i,0U),0U);});Cyc_Absyn_assign_exp(_TmpA,(*((struct _tuple11*)dles->hd)).f1,0U);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=unresolvedmem_inits;_Tmp8;});}}
# 486
vd->initializer=0;
goto _LL12;}}else{
goto _LL12;}_LL12:;}}
# 491
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_region_malloc(env.rgn,0U,sizeof(struct Cyc_List_List));_Tmp7->hd=vd,_Tmp7->tl=env.all_locals;_Tmp7;});env.all_locals=_Tmp6;});
# 493
({void*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=d;Cyc_Absyn_decl_stmt(_Tmp7,Cyc_Absyn_new_stmt(env.encloser->r,0U),0U);})->r;env.encloser->r=_Tmp6;});{
struct Cyc_Absyn_Exp*e=Cyc_Tovc_stmt_to_exp(env,s2);
if(vd->initializer!=0){
e=({struct Cyc_Absyn_Exp*_Tmp6=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_assign_exp(_Tmp7,_check_null(vd->initializer),0U);});Cyc_Absyn_seq_exp(_Tmp6,e,0U);});
# 498
vd->initializer=0;}
# 500
for(1;unresolvedmem_inits!=0;unresolvedmem_inits=unresolvedmem_inits->tl){
e=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)unresolvedmem_inits->hd,e,0U);}
return e;}}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("bad local decl in Tovc::stmt_to_exp",sizeof(char),36U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});};}default:
# 506
 return Cyc_Tovc_stmt_to_fun(env,s,Cyc_Absyn_void_type);};}
# 510
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv,struct Cyc_Absyn_Stmt*);
# 512
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _fat_ptr bad_form=_tag_fat("",sizeof(char),1U);
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0:
 return;case 1:
 return;case 10: _Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_List_List*es=_Tmp2;
# 519
Cyc_Tovc_exp_to_vc(env,e1);_Tmp3=es;goto _LL8;}case 3: _Tmp3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List*es=_Tmp3;
# 521
for(1;es!=0;es=es->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 6: _Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 526
Cyc_Tovc_exp_to_vc(env,e3);_Tmp3=e1;_Tmp2=e2;goto _LLC;}case 7: _Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LLE;}case 8: _Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL10;}case 23: _Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL12;}case 9: _Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL14;}case 4: _Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 532
Cyc_Tovc_exp_to_vc(env,e2);_Tmp3=e1;goto _LL16;}case 40: _Tmp3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp3;
# 534
_Tmp3=e1;goto _LL18;}case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1A;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1C;}case 14: _Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL1E;}case 15: _Tmp3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL20;}case 18: _Tmp3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL22;}case 20: _Tmp3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL24;}case 21: _Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL26;}case 22: _Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_Tmp3;
_Tmp3=e1;goto _LL28;}case 5: _Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_Tmp3;
# 544
Cyc_Tovc_exp_to_vc(env,e1);
return;}case 17:
# 547
 goto _LL2C;case 19: _LL2C:
 goto _LL2E;case 32: _LL2E:
 goto _LL30;case 31: _LL30:
 return;case 35: _Tmp3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp3;
# 556
for(1;dles!=0;dles=dles->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple11*)dles->hd)).f1);}
return;}case 36: _Tmp3=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
# 564
Cyc_Tovc_stmt_to_vc(env,s);
# 566
({void*_Tmp4=Cyc_Tovc_stmt_to_exp(env,s)->r;e->r=_Tmp4;});
return;}case 39:
# 569
 return;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad expression after translation to C: ",sizeof(char),40U);_Tmp5;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp6;_Tmp6.tag=4,_Tmp6.f1=e;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});};}
# 574
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 576
while(1){
env.encloser=s;{
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 584
s=s2;
continue;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
# 587
if(eopt==0)
return;
_Tmp4=eopt;goto _LLE;}case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e=_Tmp4;
# 591
Cyc_Tovc_exp_to_vc(env,e);
return;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 594
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 598
Cyc_Tovc_exp_to_vc(env,e);
Cyc_Tovc_stmt_to_vc(env,s1);
s=s2;
continue;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 603
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 607
Cyc_Tovc_exp_to_vc(env,e1);
Cyc_Tovc_exp_to_vc(env,e2);
Cyc_Tovc_exp_to_vc(env,e3);
s=s2;
continue;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s2=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 613
Cyc_Tovc_exp_to_vc(env,e);
s=s2;
continue;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
# 619
Cyc_Tovc_exp_to_vc(env,e);
for(1;scs!=0;scs=scs->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 624
{void*_Tmp5=d->r;void*_Tmp6;if(*((int*)_Tmp5)==0){_Tmp6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 626
if(Cyc_Tovc_elim_array_initializers){
# 631
void*_Tmp7=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp7)==5){
# 633
if(vd->initializer!=0){
void*_Tmp8=vd->initializer->r;void*_Tmp9;if(*((int*)_Tmp8)==35){_Tmp9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp8)->f2;{struct Cyc_List_List*dles=_Tmp9;
# 636
vd->initializer=0;{
# 638
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(vd->name,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(s2->r,0U);
{unsigned i=0U;for(0;dles!=0;(dles=dles->tl,++ i)){
s3=({struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_TmpB=({struct Cyc_Absyn_Exp*_TmpC=arr;Cyc_Absyn_subscript_exp(_TmpC,
Cyc_Absyn_uint_exp(i,0U),0U);});
# 641
Cyc_Absyn_assign_exp(_TmpB,(*((struct _tuple11*)dles->hd)).f1,0U);}),0U);Cyc_Absyn_seq_stmt(_TmpA,s3,0U);});}}
# 646
s2->r=s3->r;
goto _LL29;}}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("|",sizeof(char),2U);_TmpB;});struct Cyc_Warn_Stmt_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Stmt_Warn_Warg_struct _TmpC;_TmpC.tag=5,_TmpC.f1=s;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("|: array type bad initializer",sizeof(char),30U);_TmpD;});void*_TmpD[3];_TmpD[0]=& _TmpA,_TmpD[1]=& _TmpB,_TmpD[2]=& _TmpC;({int(*_TmpE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),3));});}_LL29:;}
# 650
goto _LL24;}else{
goto _LL24;}_LL24:;}
# 654
if(vd->initializer!=0)
Cyc_Tovc_exp_to_vc(env,vd->initializer);
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_region_malloc(env.rgn,0U,sizeof(struct Cyc_List_List));_Tmp8->hd=vd,_Tmp8->tl=env.all_locals;_Tmp8;});env.all_locals=_Tmp7;});
s=s2;
continue;}}else{
goto _LL1F;}_LL1F:;}
# 661
goto _LL1E;}default: _LL1E:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad stmt after xlation to C",sizeof(char),28U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),1));});};}
# 577
1U;}}
# 667
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*new_ds=0;
for(1;old_ds!=0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*next_d=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_Tmp0=next_d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 676
fd->cached_type=0;
# 682
if(fd->param_vardecls!=0){
struct _RegionHandle _Tmp2=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _Tmp2;_push_region(rgn);
{struct Cyc_List_List**gen_ds;gen_ds=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List*)),*gen_ds=0;
({struct Cyc_Tovc_ToExpEnv _Tmp3=({struct Cyc_Tovc_ToExpEnv _Tmp4;_Tmp4.rgn=rgn,({
struct Cyc_List_List*_Tmp5=Cyc_List_rcopy(rgn,(struct Cyc_List_List*)fd->param_vardecls->v);_Tmp4.all_locals=_Tmp5;}),({
struct Cyc_Dict_Dict _Tmp5=({struct Cyc_Dict_Dict(*_Tmp6)(struct _RegionHandle*,int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_rempty;_Tmp6;})(rgn,Cyc_strptrcmp);_Tmp4.varmap=_Tmp5;}),_Tmp4.encloser=fd->body,_Tmp4.gen_ds=gen_ds;_Tmp4;});
# 685
Cyc_Tovc_stmt_to_vc(_Tmp3,fd->body);});
# 691
if(*gen_ds!=0){
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),vd->sc=fd->sc,vd->name=fd->name,vd->varloc=0U,({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_const_tqual(0U);vd->tq=_Tmp3;}),({
void*_Tmp3=Cyc_Tcutil_fndecl2type(fd);vd->type=_Tmp3;}),vd->initializer=0,vd->rgn=0,vd->attributes=fd->i.attributes,vd->escapes=0,vd->is_proto=fd->body!=0,vd->rename=0;
# 696
new_ds=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp4=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp5->tag=0,_Tmp5->f1=vd;_Tmp5;}),0U);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=new_ds;_Tmp3;});
new_ds=Cyc_List_append(*gen_ds,new_ds);}}
# 684
;_pop_region();}
# 700
goto _LL4;}case 0: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 new_ds=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=next_d,_Tmp2->tl=new_ds;_Tmp2;});goto _LL0;case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
# 717
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("nested translation unit after translation to C",sizeof(char),47U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}
# 720
return Cyc_List_imp_rev(new_ds);}
