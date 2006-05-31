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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 939 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 957
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 959
extern void*Cyc_Absyn_sint_type;
# 1025
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1030
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1095
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1100
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1107
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1112
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1117
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1134
struct Cyc_Absyn_Exp*Cyc_Absyn_set_type(struct Cyc_Absyn_Exp*,void*);
# 1139
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1147
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1150
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1238
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};
# 42 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 52 "remove_aggregates.cyc"
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Absyn_Exp*_T4;_T0=e;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;_T1=(int*)_T5;_T2=*_T1;if(_T2!=20)goto _TL0;{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T5;_T6=_T7->f1;}{struct Cyc_Absyn_Exp*e1=_T6;_T3=
Cyc_Absyn_aggrarrow_exp(e1,f,loc);return _T3;}_TL0: _T4=
Cyc_Absyn_aggrmember_exp(e,f,loc);return _T4;;}}
# 61
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer =0U,Cyc_RemoveAggrs_NewDest =1U,Cyc_RemoveAggrs_AggrField =2U,Cyc_RemoveAggrs_Other =3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 72
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 74
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_no_init(struct Cyc_RemoveAggrs_Env env){struct Cyc_RemoveAggrs_Env _T0;struct Cyc_RemoveAggrs_Env _T1;enum Cyc_RemoveAggrs_ExpContext _T2;int _T3;_T1=env;_T2=_T1.ctxt;_T3=(int)_T2;
if(_T3!=0)goto _TL2;_T0=Cyc_RemoveAggrs_other_env;goto _TL3;_TL2: _T0=env;_TL3: return _T0;}
# 78
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray =0U,Cyc_RemoveAggrs_UsedInitializer =1U,Cyc_RemoveAggrs_OtherRes =2U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 88
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*);
# 91
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){enum Cyc_RemoveAggrs_ExpResult _T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;struct Cyc_Warn_Exp_Warn_Warg_struct _T3;void**_T4;void**_T5;int(*_T6)(struct _fat_ptr);void*(*_T7)(struct _fat_ptr);struct _fat_ptr _T8;
struct Cyc_RemoveAggrs_Result _T9=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);enum Cyc_RemoveAggrs_ExpResult _TA;_TA=_T9.res;{enum Cyc_RemoveAggrs_ExpResult r=_TA;_T0=r;_T1=(int)_T0;
if(_T1!=0)goto _TL4;{struct Cyc_Warn_String_Warn_Warg_struct _TB;_TB.tag=0;
_TB.f1=_tag_fat("remove_aggrs_exp -- unexpected array or comprehension: ",sizeof(char),56U);_T2=_TB;}{struct Cyc_Warn_String_Warn_Warg_struct _TB=_T2;{struct Cyc_Warn_Exp_Warn_Warg_struct _TC;_TC.tag=4;_TC.f1=e;_T3=_TC;}{struct Cyc_Warn_Exp_Warn_Warg_struct _TC=_T3;void*_TD[2];_T4=_TD + 0;*_T4=& _TB;_T5=_TD + 1;*_T5=& _TC;_T7=Cyc_Warn_impos2;{int(*_TE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T7;_T6=_TE;}_T8=_tag_fat(_TD,sizeof(void*),2);_T6(_T8);}}goto _TL5;_TL4: _TL5:;}}
# 97
static int Cyc_RemoveAggrs_can_use_initializer(struct Cyc_Absyn_Exp*e,void*t){int _T0;struct Cyc_Absyn_Exp*_T1;void*_T2;unsigned _T3;struct Cyc_Absyn_Exp*_T4;void*_T5;int _T6;struct Cyc_Absyn_Exp*_T7;void*_T8;void*_T9;void*_TA;int _TB;_T1=e;_T2=_T1->topt;_T3=(unsigned)_T2;
if(!_T3)goto _TL6;_T4=e;_T5=_T4->topt;_T6=Cyc_Tcutil_is_array_type(_T5);if(_T6)goto _TL6;else{goto _TL8;}_TL8: _T7=e;_T8=_T7->topt;_T9=
_check_null(_T8);_TA=t;_TB=Cyc_Tcutil_typecmp(_T9,_TA);_T0=_TB==0;goto _TL7;_TL6: _T0=0;_TL7:
# 98
 return _T0;}struct _tuple11{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 102
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_String_c _T5;unsigned _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_String_c _T8;struct Cyc_RemoveAggrs_Env _T9;enum Cyc_RemoveAggrs_ExpContext _TA;int _TB;struct Cyc_Absyn_Exp*_TC;void*_TD;void*_TE;int _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;unsigned char*_T12;const char*_T13;unsigned _T14;int _T15;char _T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_List_List*_T18;struct _tuple11*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T1B;struct Cyc_RemoveAggrs_Result _T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T1E;char*_T1F;char*_T20;void*_T21;struct Cyc_RemoveAggrs_Env _T22;enum Cyc_RemoveAggrs_ExpContext _T23;int _T24;struct Cyc_RemoveAggrs_Env _T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_Absyn_Exp*_T27;void*_T28;int _T29;struct Cyc_RemoveAggrs_Env _T2A;struct Cyc_RemoveAggrs_Env _T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_RemoveAggrs_Env _T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_RemoveAggrs_Env _T32;struct Cyc_Absyn_Exp*_T33;void*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;void*_T3C;struct Cyc_RemoveAggrs_Env _T3D;struct Cyc_Absyn_Exp*_T3E;struct Cyc_Absyn_Exp*_T3F;void*_T40;struct Cyc_Absyn_Exp*_T41;void*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Exp*_T44;struct Cyc_Absyn_Exp*_T45;void*_T46;void*_T47;struct _tuple0*_T48;void*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_Absyn_Stmt*_T4B;struct Cyc_Absyn_Stmt*_T4C;struct Cyc_Absyn_Stmt*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_Absyn_Exp*_T4F;int(*_T50)(struct _fat_ptr,struct _fat_ptr);void*(*_T51)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T52;struct _fat_ptr _T53;int _T54;struct Cyc_RemoveAggrs_Env _T55;enum Cyc_RemoveAggrs_ExpContext _T56;int _T57;struct Cyc_RemoveAggrs_Env _T58;enum Cyc_RemoveAggrs_ExpContext _T59;int _T5A;void*(*_T5B)(struct _tuple0*);struct Cyc_Absyn_Aggrdecl*_T5C;unsigned _T5D;struct Cyc_Absyn_Aggrdecl*_T5E;enum Cyc_Absyn_AggrKind _T5F;int _T60;int _T61;struct _tuple0**_T62;struct _tuple0**_T63;struct _tuple0*_T64;struct Cyc_Absyn_Exp*_T65;void*_T66;struct Cyc_RemoveAggrs_Env _T67;struct Cyc_Absyn_Exp*_T68;struct Cyc_RemoveAggrs_Env _T69;enum Cyc_RemoveAggrs_ExpContext _T6A;int _T6B;struct Cyc_Absyn_Exp*_T6C;void*_T6D;struct Cyc_List_List*_T6E;void*_T6F;struct Cyc_Absyn_Exp*_T70;struct _fat_ptr*_T71;struct Cyc_Absyn_Exp*_T72;struct Cyc_Absyn_Exp*_T73;void*_T74;void*_T75;struct Cyc_RemoveAggrs_Env _T76;struct Cyc_Absyn_Exp*_T77;struct Cyc_List_List*_T78;int(*_T79)(struct _fat_ptr,struct _fat_ptr);void*(*_T7A)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7B;struct _fat_ptr _T7C;struct Cyc_List_List*_T7D;void*_T7E;struct _tuple11*_T7F;struct _tuple11 _T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;void*_T83;struct _tuple11*_T84;struct _tuple11 _T85;struct Cyc_Absyn_Exp*_T86;void*_T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;struct Cyc_Warn_Exp_Warn_Warg_struct _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;void**_T8B;void**_T8C;void**_T8D;int(*_T8E)(struct _fat_ptr);void*(*_T8F)(struct _fat_ptr);struct _fat_ptr _T90;struct Cyc_Absyn_Exp*_T91;struct Cyc_Absyn_Exp*_T92;void*_T93;void*_T94;struct Cyc_List_List*_T95;int _T96;struct Cyc_Absyn_Exp*_T97;struct _tuple0**_T98;struct _tuple0*_T99;void*_T9A;struct Cyc_Absyn_Stmt*_T9B;struct Cyc_Absyn_Stmt*_T9C;struct Cyc_Absyn_Stmt*_T9D;struct Cyc_Absyn_Stmt*_T9E;struct Cyc_Absyn_Exp*_T9F;struct Cyc_Absyn_Exp*_TA0;struct Cyc_Absyn_Exp*_TA1;struct Cyc_Absyn_Exp*_TA2;struct Cyc_Absyn_Exp*_TA3;struct Cyc_Absyn_Exp*_TA4;struct Cyc_Absyn_Exp*_TA5;void*_TA6;struct Cyc_Absyn_Exp*_TA7;struct Cyc_RemoveAggrs_Env _TA8;int(*_TA9)(struct _fat_ptr,struct _fat_ptr);void*(*_TAA)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TAB;struct _fat_ptr _TAC;struct Cyc_RemoveAggrs_Env _TAD;struct Cyc_Absyn_Exp*_TAE;struct Cyc_Absyn_Exp*_TAF;void*_TB0;void*_TB1;int*_TB2;int _TB3;struct Cyc_Absyn_ArrayInfo _TB4;void*_TB5;struct Cyc_Absyn_ArrayInfo _TB6;struct Cyc_Absyn_Tqual _TB7;struct Cyc_Absyn_Exp*_TB8;void*_TB9;struct Cyc_List_List*_TBA;void*_TBB;struct Cyc_Absyn_Exp*_TBC;struct Cyc_Absyn_Exp*_TBD;void*_TBE;struct Cyc_Absyn_Exp*_TBF;struct Cyc_Absyn_Exp*_TC0;void*_TC1;struct Cyc_RemoveAggrs_Env _TC2;struct Cyc_Absyn_Exp*_TC3;struct Cyc_List_List*_TC4;struct Cyc_List_List*_TC5;void*_TC6;struct _tuple11*_TC7;struct _tuple11 _TC8;struct Cyc_List_List*_TC9;struct Cyc_List_List*_TCA;void*_TCB;struct _tuple11*_TCC;struct _tuple11 _TCD;struct Cyc_Absyn_Exp*_TCE;struct Cyc_Absyn_Exp*_TCF;void*_TD0;void*_TD1;struct Cyc_List_List*_TD2;struct Cyc_Absyn_Exp*_TD3;struct Cyc_Absyn_Exp*_TD4;struct Cyc_Absyn_Exp*_TD5;struct Cyc_Absyn_Exp*_TD6;struct Cyc_RemoveAggrs_Env _TD7;int(*_TD8)(struct _fat_ptr,struct _fat_ptr);void*(*_TD9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TDA;struct _fat_ptr _TDB;struct Cyc_Absyn_Vardecl*_TDC;struct Cyc_Absyn_Exp*_TDD;void*_TDE;struct Cyc_Absyn_Exp*_TDF;struct Cyc_Absyn_Exp*_TE0;void*_TE1;struct Cyc_Absyn_Exp*_TE2;struct Cyc_Absyn_Exp*_TE3;void*_TE4;struct Cyc_Absyn_Exp*_TE5;struct Cyc_Absyn_Exp*_TE6;void*_TE7;struct Cyc_Absyn_Exp*_TE8;struct Cyc_Absyn_Exp*_TE9;void*_TEA;struct Cyc_Absyn_Exp*_TEB;struct Cyc_RemoveAggrs_Env _TEC;struct Cyc_Absyn_Exp*_TED;struct Cyc_Absyn_Exp*_TEE;void*_TEF;void*_TF0;int*_TF1;int _TF2;struct Cyc_Absyn_ArrayInfo _TF3;void*_TF4;struct Cyc_Absyn_ArrayInfo _TF5;struct Cyc_Absyn_Tqual _TF6;struct Cyc_Absyn_Exp*_TF7;void*_TF8;struct Cyc_RemoveAggrs_Env _TF9;struct Cyc_Absyn_Exp*_TFA;struct Cyc_Absyn_Exp*_TFB;void*_TFC;struct Cyc_Absyn_Exp*_TFD;struct Cyc_RemoveAggrs_Env _TFE;struct Cyc_Absyn_Exp*_TFF;struct Cyc_Absyn_Exp*_T100;struct Cyc_Absyn_Exp*_T101;struct Cyc_Absyn_Exp*_T102;struct Cyc_Absyn_Stmt*_T103;int _T104;struct Cyc_RemoveAggrs_Env _T105;struct Cyc_Absyn_Exp*_T106;struct Cyc_Absyn_Exp*_T107;struct Cyc_Absyn_Exp*_T108;void*_T109;struct Cyc_Absyn_Exp*_T10A;struct Cyc_Absyn_Exp*_T10B;struct Cyc_Absyn_Exp*_T10C;void*_T10D;struct Cyc_Absyn_Exp*_T10E;struct Cyc_Absyn_Stmt*_T10F;struct Cyc_Absyn_Stmt*_T110;struct Cyc_Absyn_Exp*_T111;struct _tuple0*_T112;void*_T113;struct Cyc_Absyn_Exp*_T114;struct Cyc_Absyn_Stmt*_T115;struct Cyc_Absyn_Stmt*_T116;struct Cyc_Absyn_Exp*_T117;void*_T118;struct Cyc_Absyn_Exp*_T119;struct Cyc_Absyn_Stmt*_T11A;struct Cyc_Absyn_Stmt*_T11B;struct Cyc_Absyn_Exp*_T11C;struct Cyc_Absyn_Exp*_T11D;struct Cyc_RemoveAggrs_Env _T11E;struct Cyc_Warn_String_Warn_Warg_struct _T11F;void**_T120;int(*_T121)(struct _fat_ptr);void*(*_T122)(struct _fat_ptr);struct _fat_ptr _T123;struct Cyc_Absyn_Exp*_T124;struct Cyc_Absyn_Exp*_T125;struct Cyc_RemoveAggrs_Env _T126;struct Cyc_Absyn_Exp*_T127;struct Cyc_RemoveAggrs_Env _T128;struct Cyc_Absyn_Exp*_T129;struct Cyc_RemoveAggrs_Env _T12A;struct Cyc_RemoveAggrs_Env _T12B;struct Cyc_RemoveAggrs_Env _T12C;struct Cyc_Absyn_Exp*_T12D;struct Cyc_RemoveAggrs_Env _T12E;struct Cyc_Absyn_Exp*_T12F;struct Cyc_RemoveAggrs_Env _T130;struct Cyc_Absyn_Exp*_T131;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T132;struct Cyc_Absyn_VarargCallInfo*_T133;struct Cyc_RemoveAggrs_Env _T134;struct Cyc_List_List*_T135;void*_T136;struct Cyc_Absyn_Exp*_T137;struct Cyc_List_List*_T138;struct Cyc_Warn_String_Warn_Warg_struct _T139;struct Cyc_Warn_Exp_Warn_Warg_struct _T13A;void**_T13B;void**_T13C;int(*_T13D)(struct _fat_ptr);void*(*_T13E)(struct _fat_ptr);struct _fat_ptr _T13F;struct Cyc_RemoveAggrs_Env _T140;enum Cyc_RemoveAggrs_ExpContext _T141;int _T142;int _T143;struct Cyc_Absyn_Exp*_T144;struct Cyc_RemoveAggrs_Env _T145;struct Cyc_Absyn_Exp*_T146;struct Cyc_Absyn_Exp*_T147;struct Cyc_Absyn_Exp*_T148;struct Cyc_Absyn_Exp*_T149;struct Cyc_RemoveAggrs_Env _T14A;enum Cyc_RemoveAggrs_ExpContext _T14B;int _T14C;int _T14D;struct Cyc_Absyn_Exp*_T14E;struct Cyc_RemoveAggrs_Env _T14F;struct Cyc_Absyn_Exp*_T150;struct Cyc_Absyn_Exp*_T151;struct Cyc_Absyn_Exp*_T152;struct Cyc_Absyn_Exp*_T153;void*_T154;void*_T155;struct Cyc_Absyn_Exp*_T156;struct Cyc_Absyn_Exp*_T157;struct Cyc_Absyn_Exp*_T158;struct Cyc_RemoveAggrs_Result _T159;
enum Cyc_RemoveAggrs_ExpResult res=2U;
int did_assign=0;_T0=e;{
# 106
void*_T15A=_T0->r;struct Cyc_Absyn_Stmt*_T15B;struct Cyc_Absyn_Exp*_T15C;int _T15D;struct Cyc_Absyn_Exp*_T15E;struct Cyc_Absyn_Vardecl*_T15F;struct Cyc_Absyn_Aggrdecl*_T160;struct Cyc_List_List*_T161;struct _tuple0*_T162;struct Cyc_Absyn_Exp*_T163;struct _fat_ptr _T164;_T1=(int*)_T15A;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T15A;_T4=_T3->f1;_T5=_T4.String_c;_T6=_T5.tag;if(_T6!=8)goto _TLA;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T15A;_T7=_T165->f1;_T8=_T7.String_c;_T164=_T8.val;}{struct _fat_ptr s=_T164;_T9=env;_TA=_T9.ctxt;_TB=(int)_TA;
# 109
if(_TB!=2)goto _TLC;_TC=e;_TD=_TC->topt;_TE=_check_null(_TD);_TF=Cyc_Tcutil_is_array_type(_TE);if(!_TF)goto _TLC;{
struct Cyc_List_List*dles=0;_T10=s;{
unsigned n=_get_fat_size(_T10,sizeof(char));{
unsigned i=0U;_TL11: if(i < n)goto _TLF;else{goto _TL10;}
_TLF: _T11=s;_T12=_T11.curr;_T13=(const char*)_T12;_T14=i;_T15=(int)_T14;_T16=_T13[_T15];{struct Cyc_Absyn_Exp*c=Cyc_Absyn_char_exp(_T16,0U);_T17=c;
_T17->topt=Cyc_Absyn_char_type;{struct Cyc_List_List*_T165=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T166=_cycalloc(sizeof(struct _tuple11));
_T166->f0=0;_T166->f1=c;_T19=(struct _tuple11*)_T166;}_T165->hd=_T19;_T165->tl=dles;_T18=(struct Cyc_List_List*)_T165;}dles=_T18;}
# 112
i=i + 1;goto _TL11;_TL10:;}
# 117
dles=Cyc_List_imp_rev(dles);_T1A=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T165=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T165->tag=25;
_T165->f1=dles;_T1B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T165;}_T1A->r=(void*)_T1B;_T1C=
Cyc_RemoveAggrs_remove_aggrs_exp(env,e);return _T1C;}}_TLC: goto _LL0;}_TLA: goto _LL6;case 17: _LL6: goto _LL8;case 19: _LL8: goto _LLA;case 32: _LLA: goto _LLC;case 31: _LLC: goto _LLE;case 39: _LLE: goto _LL10;case 1: _LL10: goto _LL0;case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f2;}{struct Cyc_Absyn_Exp*e1=_T163;_T1D=e;{
# 131
void*_T165=_T1D->annot;void*_T166;struct Cyc_Absyn_Exp*_T167;_T1E=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T165;_T1F=_T1E->tag;_T20=Cyc_Toc_NewInfo;if(_T1F!=_T20)goto _TL12;{struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T168=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T165;_T167=_T168->f1;_T21=_T168->f2;_T166=(void*)_T21;}{struct Cyc_Absyn_Exp*mexp=_T167;void*typ=_T166;_T22=env;_T23=_T22.ctxt;_T24=(int)_T23;
# 134
if(_T24!=0)goto _TL14;_T25=env;_T26=_T25.dest;_T27=_check_null(_T26);_T28=typ;_T29=Cyc_RemoveAggrs_can_use_initializer(_T27,_T28);if(!_T29)goto _TL14;{struct Cyc_RemoveAggrs_Env _T168;
_T168.ctxt=1U;_T2B=env;_T168.dest=_T2B.dest;_T2A=_T168;}_T2C=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T2A,_T2C);_T2D=e;_T2E=env;_T2F=_T2E.dest;_T30=mexp;_T31=
Cyc_Absyn_assign_exp(_T2F,_T30,0U);_T32=env;_T33=_T32.dest;_T34=_T33->topt;_T35=
_check_null(_T34);_T36=
# 136
Cyc_Absyn_set_type(_T31,_T35);_T37=e1;_T38=Cyc_Absyn_seq_exp(_T36,_T37,0U);_T2D->r=_T38->r;_T39=e;_T3A=e1;
# 139
_T39->topt=_T3A->topt;
res=1U;goto _LL41;_TL14: {
# 143
struct _tuple0*xvar=Cyc_Toc_temp_var();_T3B=
Cyc_Absyn_var_exp(xvar,0U);_T3C=typ;{struct Cyc_Absyn_Exp*xexp=Cyc_Absyn_set_type(_T3B,_T3C);{struct Cyc_RemoveAggrs_Env _T168;
_T168.ctxt=1U;_T168.dest=xexp;_T3D=_T168;}_T3E=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T3D,_T3E);{
struct Cyc_Absyn_Exp*body_exp=xexp;_T3F=e;_T40=_T3F->topt;
if(_T40==0)goto _TL16;_T41=e;_T42=_T41->topt;_T43=xexp;_T44=
Cyc_Absyn_cast_exp(_T42,_T43,0,1U,0U);_T45=e;_T46=_T45->topt;_T47=
_check_null(_T46);
# 148
body_exp=Cyc_Absyn_set_type(_T44,_T47);goto _TL17;_TL16: _TL17: _T48=xvar;_T49=typ;_T4A=mexp;_T4B=
# 151
Cyc_Absyn_exp_stmt(e1,0U);_T4C=Cyc_Absyn_exp_stmt(body_exp,0U);_T4D=Cyc_Absyn_seq_stmt(_T4B,_T4C,0U);{
# 150
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T48,_T49,_T4A,_T4D,0U);_T4E=e;_T4F=
# 152
Cyc_Absyn_stmt_exp(s,0U);_T4E->r=_T4F->r;goto _LL41;}}}}}_TL12: _T51=Cyc_Warn_impos;{
# 154
int(*_T168)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T51;_T50=_T168;}_T52=_tag_fat("removeAggrs: toc did not set a new-destination",sizeof(char),47U);_T53=_tag_fat(0U,sizeof(void*),0);_T50(_T52,_T53);_LL41:;}goto _LL0;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T15A;_T162=_T165->f1;_T161=_T165->f3;_T160=_T165->f4;}{struct _tuple0*tdn=_T162;struct Cyc_List_List*dles=_T161;struct Cyc_Absyn_Aggrdecl*sdopt=_T160;
# 159
did_assign=1;_T55=env;_T56=_T55.ctxt;_T57=(int)_T56;
if(_T57==2)goto _TL18;_T58=env;_T59=_T58.ctxt;_T5A=(int)_T59;_T54=_T5A!=1;goto _TL19;_TL18: _T54=0;_TL19: {int do_stmt_exp=_T54;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;_T5C=sdopt;_T5D=(unsigned)_T5C;
if(!_T5D)goto _TL1A;_T5E=sdopt;_T5F=_T5E->kind;_T60=(int)_T5F;if(_T60!=1)goto _TL1A;_T5B=Cyc_Absyn_unionq_type;goto _TL1B;_TL1A: _T5B=Cyc_Absyn_strctq;_TL1B: {void*(*f)(struct _tuple0*)=_T5B;
void*ty=f(tdn);_T61=do_stmt_exp;
if(!_T61)goto _TL1C;{struct _tuple0**_T165=_cycalloc(sizeof(struct _tuple0*));
*_T165=Cyc_Toc_temp_var();_T62=(struct _tuple0**)_T165;}v=_T62;_T63=v;_T64=*_T63;_T65=
Cyc_Absyn_var_exp(_T64,0U);_T66=ty;dest=Cyc_Absyn_set_type(_T65,_T66);goto _TL1D;
# 169
_TL1C: v=0;_T67=env;_T68=_T67.dest;
dest=_check_null(_T68);_T69=env;_T6A=_T69.ctxt;_T6B=(int)_T6A;
if(_T6B!=1)goto _TL1E;_T6C=
Cyc_Absyn_deref_exp(dest,0U);_T6D=ty;dest=Cyc_Absyn_set_type(_T6C,_T6D);goto _TL1F;_TL1E: _TL1F: _TL1D:{
# 175
struct Cyc_List_List*dles2=dles;_TL23: if(dles2!=0)goto _TL21;else{goto _TL22;}
_TL21: _T6E=dles2;_T6F=_T6E->hd;{struct _tuple11*_T165=(struct _tuple11*)_T6F;struct Cyc_Absyn_Exp*_T166;struct Cyc_List_List*_T167;{struct _tuple11 _T168=*_T165;_T167=_T168.f0;_T166=_T168.f1;}{struct Cyc_List_List*ds=_T167;struct Cyc_Absyn_Exp*field_exp=_T166;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);_T70=
Cyc_Absyn_copy_exp(dest);_T71=f;_T72=Cyc_RemoveAggrs_member_exp(_T70,_T71,0U);_T73=field_exp;_T74=_T73->topt;_T75=
_check_null(_T74);{
# 178
struct Cyc_Absyn_Exp*field_dest=Cyc_Absyn_set_type(_T72,_T75);{struct Cyc_RemoveAggrs_Env _T168;
# 180
_T168.ctxt=2U;_T168.dest=field_dest;_T76=_T168;}_T77=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_T76,_T77);}}}_T78=dles2;
# 175
dles2=_T78->tl;goto _TL23;_TL22:;}
# 183
if(dles!=0)goto _TL24;_T7A=Cyc_Warn_impos;{
int(*_T165)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7A;_T79=_T165;}_T7B=_tag_fat("zero-field aggregate",sizeof(char),21U);_T7C=_tag_fat(0U,sizeof(void*),0);_T79(_T7B,_T7C);goto _TL25;_TL24: _TL25: _T7D=dles;_T7E=_T7D->hd;_T7F=(struct _tuple11*)_T7E;_T80=*_T7F;{
# 186
struct Cyc_Absyn_Exp*init_e=_T80.f1;_T81=dles;
dles=_T81->tl;_TL29: if(dles!=0)goto _TL27;else{goto _TL28;}
_TL27: _T82=dles;_T83=_T82->hd;_T84=(struct _tuple11*)_T83;_T85=*_T84;{struct Cyc_Absyn_Exp*e2=_T85.f1;_T86=e2;_T87=_T86->topt;
if(_T87!=0)goto _TL2A;{struct Cyc_Warn_String_Warn_Warg_struct _T165;_T165.tag=0;
_T165.f1=_tag_fat("expression ",sizeof(char),12U);_T88=_T165;}{struct Cyc_Warn_String_Warn_Warg_struct _T165=_T88;{struct Cyc_Warn_Exp_Warn_Warg_struct _T166;_T166.tag=4;_T166.f1=e2;_T89=_T166;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T166=_T89;{struct Cyc_Warn_String_Warn_Warg_struct _T167;_T167.tag=0;_T167.f1=_tag_fat(" missing type!",sizeof(char),15U);_T8A=_T167;}{struct Cyc_Warn_String_Warn_Warg_struct _T167=_T8A;void*_T168[3];_T8B=_T168 + 0;*_T8B=& _T165;_T8C=_T168 + 1;*_T8C=& _T166;_T8D=_T168 + 2;*_T8D=& _T167;_T8F=Cyc_Warn_impos2;{int(*_T169)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T8F;_T8E=_T169;}_T90=_tag_fat(_T168,sizeof(void*),3);_T8E(_T90);}}}goto _TL2B;_TL2A: _TL2B: _T91=
Cyc_Absyn_seq_exp(init_e,e2,0U);_T92=e2;_T93=_T92->topt;_T94=_check_null(_T93);init_e=Cyc_Absyn_set_type(_T91,_T94);}_T95=dles;
# 187
dles=_T95->tl;goto _TL29;_TL28: _T96=do_stmt_exp;
# 193
if(!_T96)goto _TL2C;_T97=e;_T98=
_check_null(v);_T99=*_T98;_T9A=ty;_T9B=
Cyc_Absyn_exp_stmt(init_e,0U);_T9C=
Cyc_Absyn_exp_stmt(dest,0U);_T9D=
# 195
Cyc_Absyn_seq_stmt(_T9B,_T9C,0U);_T9E=
# 194
Cyc_Absyn_declare_stmt(_T99,_T9A,0,_T9D,0U);_T9F=Cyc_Absyn_stmt_exp(_T9E,0U);_T97->r=_T9F->r;goto _TL2D;
# 198
_TL2C: _TA0=e;_TA1=init_e;_TA0->r=_TA1->r;_TA2=e;_TA3=init_e;
_TA2->topt=_TA3->topt;_TL2D: goto _LL0;}}}}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T15A;_T161=_T165->f1;}{struct Cyc_List_List*dles=_T161;
# 207
res=0U;
if(dles!=0)goto _TL2E;_TA4=e;_TA5=
Cyc_Absyn_signed_int_exp(0,0U);_TA6=Cyc_Absyn_sint_type;_TA7=Cyc_Absyn_set_type(_TA5,_TA6);*_TA4=*_TA7;goto _LL0;_TL2E: _TA8=env;{
# 212
enum Cyc_RemoveAggrs_ExpContext _T165=_TA8.ctxt;if(_T165!=Cyc_RemoveAggrs_Other)goto _TL30;_TAA=Cyc_Warn_impos;{
int(*_T166)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TAA;_TA9=_T166;}_TAB=_tag_fat("remove-aggrs: Array_e in bad position",sizeof(char),38U);_TAC=_tag_fat(0U,sizeof(void*),0);_TA9(_TAB,_TAC);goto _TL31;_TL30: goto _LL49;_TL31: _LL49:;}
# 216
did_assign=1;_TAD=env;_TAE=_TAD.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TAE);
void*dest_type;_TAF=dest;_TB0=_TAF->topt;_TB1=
_check_null(_TB0);{void*_T165=Cyc_Absyn_compress(_TB1);struct Cyc_Absyn_ArrayInfo _T166;_TB2=(int*)_T165;_TB3=*_TB2;if(_TB3!=5)goto _TL32;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T167=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T165;_T166=_T167->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T166;_TB4=ai;_TB5=_TB4.elt_type;_TB6=ai;_TB7=_TB6.tq;
dest_type=Cyc_Absyn_cstar_type(_TB5,_TB7);goto _LL4E;}_TL32: _TB8=dest;_TB9=_TB8->topt;
dest_type=_check_null(_TB9);goto _LL4E;_LL4E:;}{
# 224
int i=0;{
struct Cyc_List_List*dles2=dles;_TL37: if(dles2!=0)goto _TL35;else{goto _TL36;}
# 228
_TL35: _TBA=dles2;_TBB=_TBA->hd;{struct _tuple11*_T165=(struct _tuple11*)_TBB;struct Cyc_Absyn_Exp*_T166;{struct _tuple11 _T167=*_T165;_T166=_T167.f1;}{struct Cyc_Absyn_Exp*field_exp=_T166;_TBC=
Cyc_Absyn_copy_exp(dest);_TBD=
Cyc_Absyn_signed_int_exp(i,0U);_TBE=Cyc_Absyn_sint_type;_TBF=Cyc_Absyn_set_type(_TBD,_TBE);_TC0=
# 229
Cyc_Absyn_subscript_exp(_TBC,_TBF,0U);_TC1=dest_type;{struct Cyc_Absyn_Exp*fielddest=Cyc_Absyn_set_type(_TC0,_TC1);{struct Cyc_RemoveAggrs_Env _T167;
# 233
_T167.ctxt=2U;_T167.dest=fielddest;_TC2=_T167;}_TC3=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_TC2,_TC3);}}}_TC4=dles2;
# 225
dles2=_TC4->tl;i=i + 1;goto _TL37;_TL36:;}_TC5=dles;_TC6=_TC5->hd;_TC7=(struct _tuple11*)_TC6;_TC8=*_TC7;{
# 235
struct Cyc_Absyn_Exp*init_e=_TC8.f1;_TC9=dles;
dles=_TC9->tl;_TL3B: if(dles!=0)goto _TL39;else{goto _TL3A;}
_TL39: _TCA=dles;_TCB=_TCA->hd;_TCC=(struct _tuple11*)_TCB;_TCD=*_TCC;{struct Cyc_Absyn_Exp*e2=_TCD.f1;_TCE=
Cyc_Absyn_seq_exp(init_e,e2,0U);_TCF=e2;_TD0=_TCF->topt;_TD1=_check_null(_TD0);init_e=Cyc_Absyn_set_type(_TCE,_TD1);}_TD2=dles;
# 236
dles=_TD2->tl;goto _TL3B;_TL3A: _TD3=e;_TD4=init_e;
# 240
_TD3->r=_TD4->r;_TD5=e;_TD6=init_e;
_TD5->topt=_TD6->topt;goto _LL0;}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T15A;_T15F=_T165->f1;_T163=_T165->f2;_T15E=_T165->f3;_T15D=_T165->f4;}{struct Cyc_Absyn_Vardecl*vd=_T15F;struct Cyc_Absyn_Exp*bd=_T163;struct Cyc_Absyn_Exp*body=_T15E;int zero_term=_T15D;
# 247
did_assign=1;
res=0U;_TD7=env;{
enum Cyc_RemoveAggrs_ExpContext _T165=_TD7.ctxt;if(_T165!=Cyc_RemoveAggrs_Other)goto _TL3C;_TD9=Cyc_Warn_impos;{
int(*_T166)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD9;_TD8=_T166;}_TDA=_tag_fat("remove-aggrs: comprehension in bad position",sizeof(char),44U);_TDB=_tag_fat(0U,sizeof(void*),0);_TD8(_TDA,_TDB);goto _TL3D;_TL3C: goto _LL56;_TL3D: _LL56:;}{
# 253
struct _tuple0*max=Cyc_Toc_temp_var();_TDC=vd;{
struct _tuple0*i=_TDC->name;_TDD=
Cyc_Absyn_var_exp(i,0U);_TDE=Cyc_Absyn_sint_type;_TDF=Cyc_Absyn_set_type(_TDD,_TDE);_TE0=
Cyc_Absyn_signed_int_exp(0,0U);_TE1=Cyc_Absyn_sint_type;_TE2=Cyc_Absyn_set_type(_TE0,_TE1);{
# 255
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(_TDF,_TE2,0U);_TE3=
# 257
Cyc_Absyn_var_exp(i,0U);_TE4=Cyc_Absyn_uint_type;_TE5=Cyc_Absyn_set_type(_TE3,_TE4);_TE6=
Cyc_Absyn_var_exp(max,0U);_TE7=Cyc_Absyn_uint_type;_TE8=Cyc_Absyn_set_type(_TE6,_TE7);{
# 257
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(_TE5,_TE8,0U);_TE9=
# 259
Cyc_Absyn_var_exp(i,0U);_TEA=Cyc_Absyn_uint_type;_TEB=Cyc_Absyn_set_type(_TE9,_TEA);{struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(_TEB,0U,0U);_TEC=env;_TED=_TEC.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TED);
void*dest_type;_TEE=dest;_TEF=_TEE->topt;_TF0=
_check_null(_TEF);{void*_T165=Cyc_Absyn_compress(_TF0);struct Cyc_Absyn_ArrayInfo _T166;_TF1=(int*)_T165;_TF2=*_TF1;if(_TF2!=5)goto _TL3E;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T167=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T165;_T166=_T167->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T166;_TF3=ai;_TF4=_TF3.elt_type;_TF5=ai;_TF6=_TF5.tq;
dest_type=Cyc_Absyn_cstar_type(_TF4,_TF6);goto _LL5B;}_TL3E: _TF7=dest;_TF8=_TF7->topt;
dest_type=_check_null(_TF8);_LL5B:;}_TF9=env;_TFA=_TF9.dest;_TFB=
# 266
Cyc_Absyn_var_exp(i,0U);_TFC=Cyc_Absyn_uint_type;_TFD=Cyc_Absyn_set_type(_TFB,_TFC);{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(_TFA,_TFD,0U);
Cyc_Absyn_set_type(lval,dest_type);{struct Cyc_RemoveAggrs_Env _T165;
_T165.ctxt=2U;_T165.dest=Cyc_Absyn_copy_exp(lval);_TFE=_T165;}_TFF=body;Cyc_RemoveAggrs_remove_aggrs_exp(_TFE,_TFF);_T100=ea;_T101=eb;_T102=ec;_T103=
Cyc_Absyn_exp_stmt(body,0U);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_T100,_T101,_T102,_T103,0U);_T104=zero_term;
if(!_T104)goto _TL40;_T105=env;_T106=_T105.dest;_T107=
# 277
Cyc_Absyn_copy_exp(_T106);_T108=
Cyc_Absyn_var_exp(max,0U);_T109=Cyc_Absyn_sint_type;_T10A=Cyc_Absyn_set_type(_T108,_T109);_T10B=
# 277
Cyc_Absyn_subscript_exp(_T107,_T10A,0U);_T10C=
# 279
Cyc_Absyn_signed_int_exp(0,0U);_T10D=Cyc_Absyn_sint_type;_T10E=Cyc_Absyn_set_type(_T10C,_T10D);{
# 277
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(_T10B,_T10E,0U);_T10F=s;_T110=
# 280
Cyc_Absyn_exp_stmt(ex,0U);s=Cyc_Absyn_seq_stmt(_T10F,_T110,0U);}goto _TL41;_TL40: _TL41:
# 283
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);_T111=e;_T112=max;_T113=Cyc_Absyn_uint_type;_T114=bd;_T115=
# 285
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U);_T116=
# 284
Cyc_Absyn_declare_stmt(_T112,_T113,_T114,_T115,0U);_T117=
# 286
Cyc_Absyn_signed_int_exp(0,0U);_T118=Cyc_Absyn_sint_type;_T119=Cyc_Absyn_set_type(_T117,_T118);_T11A=Cyc_Absyn_exp_stmt(_T119,0U);_T11B=
# 284
Cyc_Absyn_seq_stmt(_T116,_T11A,0U);_T11C=Cyc_Absyn_stmt_exp(_T11B,0U);_T111->r=_T11C->r;_T11D=e;
# 287
_T11D->topt=Cyc_Absyn_sint_type;goto _LL0;}}}}}}}}}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}{struct Cyc_Absyn_Exp*bd=_T163;
# 291
did_assign=1;
res=0U;_T11E=env;{
enum Cyc_RemoveAggrs_ExpContext _T165=_T11E.ctxt;if(_T165!=Cyc_RemoveAggrs_Other)goto _TL42;{struct Cyc_Warn_String_Warn_Warg_struct _T166;_T166.tag=0;
_T166.f1=_tag_fat("remove-aggrs: no-init-comp in bad position",sizeof(char),43U);_T11F=_T166;}{struct Cyc_Warn_String_Warn_Warg_struct _T166=_T11F;void*_T167[1];_T120=_T167 + 0;*_T120=& _T166;_T122=Cyc_Warn_impos2;{int(*_T168)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T122;_T121=_T168;}_T123=_tag_fat(_T167,sizeof(void*),1);_T121(_T123);}goto _TL43;_TL42: goto _LL60;_TL43: _LL60:;}_T124=e;_T125=bd;
# 298
*_T124=*_T125;goto _LL0;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f3;}{struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;
# 302
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f2;}{struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;
# 306
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T126=
Cyc_RemoveAggrs_no_init(env);_T127=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T126,_T127);goto _LL0;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f2;_T15C=_T165->f3;}{struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;struct Cyc_Absyn_Exp*e3=_T15C;
# 311
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T128=
Cyc_RemoveAggrs_no_init(env);_T129=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T128,_T129);{struct Cyc_RemoveAggrs_Env _T165;_T12B=env;
_T165.ctxt=_T12B.ctxt;_T12C=env;_T12D=_T12C.dest;if(_T12D!=0)goto _TL44;_T165.dest=0;goto _TL45;_TL44: _T12E=env;_T12F=_T12E.dest;_T165.dest=Cyc_Absyn_copy_exp(_T12F);_TL45: _T12A=_T165;}{struct Cyc_RemoveAggrs_Env env3=_T12A;_T130=
Cyc_RemoveAggrs_no_init(env3);_T131=e3;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T130,_T131);goto _LL0;}}case 10: _T132=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T15A;_T133=_T132->f3;if(_T133!=0)goto _TL46;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T161=_T165->f2;}{struct Cyc_Absyn_Exp*e=_T163;struct Cyc_List_List*es=_T161;
# 319
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);_T161=es;goto _LL24;}_TL46: goto _LL3F;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T15A;_T161=_T165->f2;}_LL24: {struct Cyc_List_List*es=_T161;
# 322
_TL4B: if(es!=0)goto _TL49;else{goto _TL4A;}
_TL49: _T134=Cyc_RemoveAggrs_other_env;_T135=es;_T136=_T135->hd;_T137=(struct Cyc_Absyn_Exp*)_T136;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T134,_T137);_T138=es;
# 322
es=_T138->tl;goto _TL4B;_TL4A: goto _LL0;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f2;}{struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;_T163=e1;_T15E=e2;goto _LL28;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f2;}_LL28: {struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;_T163=e1;_T15E=e2;goto _LL2A;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;_T15E=_T165->f2;}_LL2A: {struct Cyc_Absyn_Exp*e1=_T163;struct Cyc_Absyn_Exp*e2=_T15E;
# 329
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}{struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL2E;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL2E: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL30;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f2;}_LL30: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL32;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL32: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL34;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL34: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL36;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL36: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL38;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL38: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL3A;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL3A: {struct Cyc_Absyn_Exp*e1=_T163;_T163=e1;goto _LL3C;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T15A;_T163=_T165->f1;}_LL3C: {struct Cyc_Absyn_Exp*e1=_T163;
# 342
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T165=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T15A;_T15B=_T165->f1;}{struct Cyc_Absyn_Stmt*s=_T15B;
# 345
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:{struct Cyc_Warn_String_Warn_Warg_struct _T165;_T165.tag=0;
# 347
_T165.f1=_tag_fat("bad exp after translation to C: ",sizeof(char),33U);_T139=_T165;}{struct Cyc_Warn_String_Warn_Warg_struct _T165=_T139;{struct Cyc_Warn_Exp_Warn_Warg_struct _T166;_T166.tag=4;_T166.f1=e;_T13A=_T166;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T166=_T13A;void*_T167[2];_T13B=_T167 + 0;*_T13B=& _T165;_T13C=_T167 + 1;*_T13C=& _T166;_T13E=Cyc_Warn_impos2;{int(*_T168)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T13E;_T13D=_T168;}_T13F=_tag_fat(_T167,sizeof(void*),2);_T13D(_T13F);}}}_LL0:;}_T140=env;_T141=_T140.ctxt;_T142=(int)_T141;
# 349
if(_T142!=2)goto _TL4C;_T143=did_assign;if(_T143)goto _TL4C;else{goto _TL4E;}
_TL4E: _T144=e;_T145=env;_T146=_T145.dest;_T147=_check_null(_T146);_T148=Cyc_Absyn_copy_exp(e);_T149=Cyc_Absyn_assign_exp(_T147,_T148,0U);_T144->r=_T149->r;goto _TL4D;_TL4C: _TL4D: _T14A=env;_T14B=_T14A.ctxt;_T14C=(int)_T14B;
if(_T14C!=1)goto _TL4F;_T14D=did_assign;if(_T14D)goto _TL4F;else{goto _TL51;}
_TL51: _T14E=e;_T14F=env;_T150=_T14F.dest;_T151=_check_null(_T150);_T152=Cyc_Absyn_deref_exp(_T151,0U);_T153=e;_T154=_T153->topt;_T155=_check_null(_T154);_T156=Cyc_Absyn_set_type(_T152,_T155);_T157=
Cyc_Absyn_copy_exp(e);_T158=
# 352
Cyc_Absyn_assign_exp(_T156,_T157,0U);_T14E->r=_T158->r;goto _TL50;_TL4F: _TL50:{struct Cyc_RemoveAggrs_Result _T15A;
# 355
_T15A.res=res;_T159=_T15A;}return _T159;}
# 362
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f1(int bogus,struct Cyc_Absyn_Exp*e){
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
return 0;}
# 366
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f2(int bogus,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;int _T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Vardecl*_T6;enum Cyc_Absyn_Scope _T7;int _T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TB;void*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Vardecl*_TE;struct Cyc_RemoveAggrs_Env _TF;struct Cyc_Absyn_Vardecl*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Exp*_T12;enum Cyc_RemoveAggrs_ExpResult _T13;int _T14;enum Cyc_RemoveAggrs_ExpResult _T15;int _T16;struct Cyc_Absyn_Stmt*_T17;struct Cyc_Absyn_Decl*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_Stmt*_T1C;struct Cyc_Absyn_Stmt*_T1D;struct Cyc_Absyn_Stmt*_T1E;struct Cyc_Absyn_Stmt*_T1F;struct Cyc_Absyn_Vardecl*_T20;struct Cyc_Absyn_Fndecl*_T21;struct Cyc_Absyn_Stmt*_T22;int(*_T23)(struct _fat_ptr,struct _fat_ptr);void*(*_T24)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T25;struct _fat_ptr _T26;_T0=s;{
void*_T27=_T0->r;struct Cyc_Absyn_Stmt*_T28;struct Cyc_Absyn_Decl*_T29;_T1=(int*)_T27;_T2=*_T1;if(_T2!=12)goto _TL52;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T2A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T27;_T29=_T2A->f1;_T28=_T2A->f2;}{struct Cyc_Absyn_Decl*d=_T29;struct Cyc_Absyn_Stmt*s2=_T28;
# 369
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);_T3=d;{
void*_T2A=_T3->r;struct Cyc_Absyn_Fndecl*_T2B;struct Cyc_Absyn_Vardecl*_T2C;_T4=(int*)_T2A;_T5=*_T4;switch(_T5){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2A;_T2C=_T2D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2C;_T6=vd;_T7=_T6->sc;_T8=(int)_T7;
# 372
if(_T8!=0)goto _TL55;return 0;_TL55: _T9=vd;_TA=_T9->initializer;
# 380
if(_TA==0)goto _TL57;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T2D->tag=4;
_T2D->f1=vd;_TB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T2D;}_TC=(void*)_TB;{struct Cyc_Absyn_Exp*var_exp=Cyc_Absyn_varb_exp(_TC,0U);_TD=var_exp;_TE=vd;
_TD->topt=_TE->type;{struct Cyc_RemoveAggrs_Env _T2D;
_T2D.ctxt=0U;_T2D.dest=var_exp;_TF=_T2D;}_T10=vd;_T11=_T10->initializer;_T12=
_check_null(_T11);{
# 383
struct Cyc_RemoveAggrs_Result _T2D=Cyc_RemoveAggrs_remove_aggrs_exp(_TF,_T12);enum Cyc_RemoveAggrs_ExpResult _T2E;_T2E=_T2D.res;{enum Cyc_RemoveAggrs_ExpResult r=_T2E;_T13=r;_T14=(int)_T13;
# 385
if(_T14==0)goto _TL5B;else{goto _TL5C;}_TL5C: _T15=r;_T16=(int)_T15;if(_T16==1)goto _TL5B;else{goto _TL59;}
_TL5B: _T17=s;_T18=d;_T19=vd;_T1A=_T19->initializer;_T1B=_check_null(_T1A);_T1C=Cyc_Absyn_exp_stmt(_T1B,0U);_T1D=s2;_T1E=Cyc_Absyn_seq_stmt(_T1C,_T1D,0U);_T1F=Cyc_Absyn_decl_stmt(_T18,_T1E,0U);_T17->r=_T1F->r;_T20=vd;
# 388
_T20->initializer=0;goto _TL5A;_TL59: _TL5A:;}}}goto _TL58;_TL57: _TL58:
# 391
 return 0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2A;_T2B=_T2D->f1;}{struct Cyc_Absyn_Fndecl*fd=_T2B;_T21=fd;_T22=_T21->body;
Cyc_RemoveAggrs_remove_aggrs_stmt(_T22);return 0;}default: _T24=Cyc_Warn_impos;{
int(*_T2D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T24;_T23=_T2D;}_T25=_tag_fat("bad local declaration after xlation to C",sizeof(char),41U);_T26=_tag_fat(0U,sizeof(void*),0);_T23(_T25,_T26);};}}goto _TL53;_TL52:
# 395
 return 1;_TL53:;}}
# 398
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Absyn_Stmt*_T2;_T1=Cyc_Absyn_visit_stmt;{
void(*_T3)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T1;_T0=_T3;}_T2=s;_T0(Cyc_RemoveAggrs_remove_aggrs_stmt_f1,Cyc_RemoveAggrs_remove_aggrs_stmt_f2,0,_T2);}
# 402
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Fndecl*_T5;enum Cyc_Absyn_Scope _T6;int _T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_Stmt*_T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;{
struct Cyc_List_List*ds2=ds;_TL60: if(ds2!=0)goto _TL5E;else{goto _TL5F;}
_TL5E: _T0=ds2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T14=_T2->r;struct Cyc_Absyn_Fndecl*_T15;_T3=(int*)_T14;_T4=*_T3;switch(_T4){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T14;_T15=_T16->f1;}{struct Cyc_Absyn_Fndecl*fd=_T15;_T5=fd;_T6=_T5->orig_scope;_T7=(int)_T6;
# 406
if(_T7==4)goto _TL62;_T8=fd;_T9=_T8->body;
Cyc_RemoveAggrs_remove_aggrs_stmt(_T9);goto _TL63;_TL62: _TL63: goto _LL0;}case 0: goto _LL6;case 5: _LL6: goto _LL8;case 7: _LL8: goto _LLA;case 8: _LLA: goto _LL0;case 4: goto _LLE;case 6: _LLE: goto _LL10;case 2: _LL10: goto _LL12;case 3: _LL12: _TB=Cyc_Warn_impos;{
# 416
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T16;}_TC=_tag_fat("Cyclone decl after xlation to C",sizeof(char),32U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);case 9: goto _LL16;case 10: _LL16: goto _LL18;case 11: _LL18: goto _LL1A;case 12: _LL1A: goto _LL1C;case 13: _LL1C: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: _TF=Cyc_Warn_impos;{
# 424
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T16;}_T10=_tag_fat("translation unit after xlation to C",sizeof(char),36U);_T11=_tag_fat(0U,sizeof(void*),0);_TE(_T10,_T11);}_LL0:;}_T12=ds2;
# 403
ds2=_T12->tl;goto _TL60;_TL5F:;}_T13=ds;
# 426
return _T13;}
