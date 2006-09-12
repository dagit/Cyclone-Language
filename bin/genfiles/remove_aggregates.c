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
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 942 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 960
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 962
extern void*Cyc_Absyn_sint_type;
# 1028
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1033
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1090
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1098
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1103
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1110
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1115
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1120
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1137
struct Cyc_Absyn_Exp*Cyc_Absyn_set_type(struct Cyc_Absyn_Exp*,void*);
# 1142
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1150
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1153
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1241
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1244
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};
# 42 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 95
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 50 "remove_aggregates.cyc"
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_deep_copy(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;_T0=
Cyc_Tcutil_deep_copy_exp(1,e);return _T0;}
# 56
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Absyn_Exp*_T4;_T0=e;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;_T1=(int*)_T5;_T2=*_T1;if(_T2!=20)goto _TL0;{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T7=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T5;_T6=_T7->f1;}{struct Cyc_Absyn_Exp*e1=_T6;_T3=
Cyc_Absyn_aggrarrow_exp(e1,f,loc);return _T3;}_TL0: _T4=
Cyc_Absyn_aggrmember_exp(e,f,loc);return _T4;;}}
# 65
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer =0U,Cyc_RemoveAggrs_NewDest =1U,Cyc_RemoveAggrs_AggrField =2U,Cyc_RemoveAggrs_Other =3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 76
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 78
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_no_init(struct Cyc_RemoveAggrs_Env env){struct Cyc_RemoveAggrs_Env _T0;struct Cyc_RemoveAggrs_Env _T1;enum Cyc_RemoveAggrs_ExpContext _T2;int _T3;_T1=env;_T2=_T1.ctxt;_T3=(int)_T2;
if(_T3!=0)goto _TL2;_T0=Cyc_RemoveAggrs_other_env;goto _TL3;_TL2: _T0=env;_TL3: return _T0;}
# 82
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray =0U,Cyc_RemoveAggrs_UsedInitializer =1U,Cyc_RemoveAggrs_OtherRes =2U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 92
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*);
# 95
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){enum Cyc_RemoveAggrs_ExpResult _T0;int _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;struct Cyc_Warn_Exp_Warn_Warg_struct _T3;int(*_T4)(struct _fat_ptr);void*(*_T5)(struct _fat_ptr);struct _fat_ptr _T6;
struct Cyc_RemoveAggrs_Result _T7=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);enum Cyc_RemoveAggrs_ExpResult _T8;_T8=_T7.res;{enum Cyc_RemoveAggrs_ExpResult r=_T8;_T0=r;_T1=(int)_T0;
if(_T1!=0)goto _TL4;{struct Cyc_Warn_String_Warn_Warg_struct _T9;_T9.tag=0;
_T9.f1=_tag_fat("remove_aggrs_exp -- unexpected array or comprehension: ",sizeof(char),56U);_T2=_T9;}{struct Cyc_Warn_String_Warn_Warg_struct _T9=_T2;{struct Cyc_Warn_Exp_Warn_Warg_struct _TA;_TA.tag=4;_TA.f1=e;_T3=_TA;}{struct Cyc_Warn_Exp_Warn_Warg_struct _TA=_T3;void*_TB[2];_TB[0]=& _T9;_TB[1]=& _TA;_T5=Cyc_Warn_impos2;{int(*_TC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5;_T4=_TC;}_T6=_tag_fat(_TB,sizeof(void*),2);_T4(_T6);}}goto _TL5;_TL4: _TL5:;}}
# 101
static int Cyc_RemoveAggrs_can_use_initializer(struct Cyc_Absyn_Exp*e,void*t){int _T0;struct Cyc_Absyn_Exp*_T1;void*_T2;unsigned _T3;struct Cyc_Absyn_Exp*_T4;void*_T5;int _T6;struct Cyc_Absyn_Exp*_T7;void*_T8;void*_T9;void*_TA;int _TB;_T1=e;_T2=_T1->topt;_T3=(unsigned)_T2;
if(!_T3)goto _TL6;_T4=e;_T5=_T4->topt;_T6=Cyc_Tcutil_is_array_type(_T5);if(_T6)goto _TL6;else{goto _TL8;}_TL8: _T7=e;_T8=_T7->topt;_T9=
_check_null(_T8);_TA=t;_TB=Cyc_Tcutil_typecmp(_T9,_TA);_T0=_TB==0;goto _TL7;_TL6: _T0=0;_TL7:
# 102
 return _T0;}struct _tuple11{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 106
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_String_c _T5;unsigned _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_String_c _T8;struct Cyc_RemoveAggrs_Env _T9;enum Cyc_RemoveAggrs_ExpContext _TA;int _TB;struct Cyc_Absyn_Exp*_TC;void*_TD;void*_TE;int _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;unsigned char*_T12;const char*_T13;const char*_T14;unsigned _T15;int _T16;char _T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_List_List*_T19;struct _tuple11*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T1C;struct Cyc_RemoveAggrs_Result _T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T1F;char*_T20;char*_T21;void*_T22;struct Cyc_RemoveAggrs_Env _T23;enum Cyc_RemoveAggrs_ExpContext _T24;int _T25;struct Cyc_RemoveAggrs_Env _T26;struct Cyc_Absyn_Exp*_T27;struct Cyc_Absyn_Exp*_T28;void*_T29;int _T2A;struct Cyc_RemoveAggrs_Env _T2B;struct Cyc_RemoveAggrs_Env _T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_Absyn_Exp*_T2E;struct Cyc_RemoveAggrs_Env _T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_RemoveAggrs_Env _T33;struct Cyc_Absyn_Exp*_T34;void*_T35;void*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;struct Cyc_Absyn_Exp*_T3C;void*_T3D;struct Cyc_RemoveAggrs_Env _T3E;struct Cyc_Absyn_Exp*_T3F;struct Cyc_Absyn_Exp*_T40;void*_T41;struct Cyc_Absyn_Exp*_T42;void*_T43;struct Cyc_Absyn_Exp*_T44;struct Cyc_Absyn_Exp*_T45;struct Cyc_Absyn_Exp*_T46;void*_T47;void*_T48;struct _tuple0*_T49;void*_T4A;struct Cyc_Absyn_Exp*_T4B;struct Cyc_Absyn_Stmt*_T4C;struct Cyc_Absyn_Stmt*_T4D;struct Cyc_Absyn_Stmt*_T4E;struct Cyc_Absyn_Exp*_T4F;struct Cyc_Absyn_Exp*_T50;int(*_T51)(struct _fat_ptr,struct _fat_ptr);void*(*_T52)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T53;struct _fat_ptr _T54;int _T55;struct Cyc_RemoveAggrs_Env _T56;enum Cyc_RemoveAggrs_ExpContext _T57;int _T58;struct Cyc_RemoveAggrs_Env _T59;enum Cyc_RemoveAggrs_ExpContext _T5A;int _T5B;void*(*_T5C)(struct _tuple0*);struct Cyc_Absyn_Aggrdecl*_T5D;unsigned _T5E;struct Cyc_Absyn_Aggrdecl*_T5F;enum Cyc_Absyn_AggrKind _T60;int _T61;int _T62;struct _tuple0**_T63;struct _tuple0**_T64;struct _tuple0*_T65;struct Cyc_Absyn_Exp*_T66;void*_T67;struct Cyc_RemoveAggrs_Env _T68;struct Cyc_Absyn_Exp*_T69;struct Cyc_RemoveAggrs_Env _T6A;enum Cyc_RemoveAggrs_ExpContext _T6B;int _T6C;struct Cyc_Absyn_Exp*_T6D;void*_T6E;struct Cyc_List_List*_T6F;void*_T70;struct Cyc_Absyn_Exp*_T71;struct _fat_ptr*_T72;struct Cyc_Absyn_Exp*_T73;struct Cyc_Absyn_Exp*_T74;void*_T75;void*_T76;struct Cyc_RemoveAggrs_Env _T77;struct Cyc_Absyn_Exp*_T78;struct Cyc_List_List*_T79;int(*_T7A)(struct _fat_ptr,struct _fat_ptr);void*(*_T7B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7C;struct _fat_ptr _T7D;struct Cyc_List_List*_T7E;void*_T7F;struct _tuple11*_T80;struct _tuple11 _T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;void*_T84;struct _tuple11*_T85;struct _tuple11 _T86;struct Cyc_Absyn_Exp*_T87;void*_T88;struct Cyc_Warn_String_Warn_Warg_struct _T89;struct Cyc_Warn_Exp_Warn_Warg_struct _T8A;struct Cyc_Warn_String_Warn_Warg_struct _T8B;int(*_T8C)(struct _fat_ptr);void*(*_T8D)(struct _fat_ptr);struct _fat_ptr _T8E;struct Cyc_Absyn_Exp*_T8F;struct Cyc_Absyn_Exp*_T90;void*_T91;void*_T92;struct Cyc_List_List*_T93;int _T94;struct Cyc_Absyn_Exp*_T95;struct _tuple0**_T96;struct _tuple0*_T97;void*_T98;struct Cyc_Absyn_Stmt*_T99;struct Cyc_Absyn_Stmt*_T9A;struct Cyc_Absyn_Stmt*_T9B;struct Cyc_Absyn_Stmt*_T9C;struct Cyc_Absyn_Exp*_T9D;struct Cyc_Absyn_Exp*_T9E;struct Cyc_Absyn_Exp*_T9F;struct Cyc_Absyn_Exp*_TA0;struct Cyc_Absyn_Exp*_TA1;struct Cyc_Absyn_Exp*_TA2;struct Cyc_Absyn_Exp*_TA3;void*_TA4;struct Cyc_Absyn_Exp*_TA5;struct Cyc_RemoveAggrs_Env _TA6;int(*_TA7)(struct _fat_ptr,struct _fat_ptr);void*(*_TA8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA9;struct _fat_ptr _TAA;struct Cyc_RemoveAggrs_Env _TAB;struct Cyc_Absyn_Exp*_TAC;struct Cyc_Absyn_Exp*_TAD;void*_TAE;void*_TAF;int*_TB0;int _TB1;struct Cyc_Absyn_ArrayInfo _TB2;void*_TB3;struct Cyc_Absyn_ArrayInfo _TB4;struct Cyc_Absyn_Tqual _TB5;struct Cyc_Absyn_Exp*_TB6;void*_TB7;struct Cyc_List_List*_TB8;void*_TB9;struct Cyc_Absyn_Exp*_TBA;struct Cyc_Absyn_Exp*_TBB;void*_TBC;struct Cyc_Absyn_Exp*_TBD;struct Cyc_Absyn_Exp*_TBE;void*_TBF;struct Cyc_RemoveAggrs_Env _TC0;struct Cyc_Absyn_Exp*_TC1;struct Cyc_List_List*_TC2;struct Cyc_List_List*_TC3;void*_TC4;struct _tuple11*_TC5;struct _tuple11 _TC6;struct Cyc_List_List*_TC7;struct Cyc_List_List*_TC8;void*_TC9;struct _tuple11*_TCA;struct _tuple11 _TCB;struct Cyc_Absyn_Exp*_TCC;struct Cyc_Absyn_Exp*_TCD;void*_TCE;void*_TCF;struct Cyc_List_List*_TD0;struct Cyc_Absyn_Exp*_TD1;struct Cyc_Absyn_Exp*_TD2;struct Cyc_Absyn_Exp*_TD3;struct Cyc_Absyn_Exp*_TD4;struct Cyc_RemoveAggrs_Env _TD5;int(*_TD6)(struct _fat_ptr,struct _fat_ptr);void*(*_TD7)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD8;struct _fat_ptr _TD9;struct Cyc_Absyn_Vardecl*_TDA;struct Cyc_Absyn_Exp*_TDB;void*_TDC;struct Cyc_Absyn_Exp*_TDD;struct Cyc_Absyn_Exp*_TDE;void*_TDF;struct Cyc_Absyn_Exp*_TE0;struct Cyc_Absyn_Exp*_TE1;void*_TE2;struct Cyc_Absyn_Exp*_TE3;struct Cyc_Absyn_Exp*_TE4;void*_TE5;struct Cyc_Absyn_Exp*_TE6;struct Cyc_Absyn_Exp*_TE7;void*_TE8;struct Cyc_Absyn_Exp*_TE9;struct Cyc_RemoveAggrs_Env _TEA;struct Cyc_Absyn_Exp*_TEB;struct Cyc_Absyn_Exp*_TEC;void*_TED;void*_TEE;int*_TEF;int _TF0;struct Cyc_Absyn_ArrayInfo _TF1;void*_TF2;struct Cyc_Absyn_ArrayInfo _TF3;struct Cyc_Absyn_Tqual _TF4;struct Cyc_Absyn_Exp*_TF5;void*_TF6;struct Cyc_RemoveAggrs_Env _TF7;struct Cyc_Absyn_Exp*_TF8;struct Cyc_Absyn_Exp*_TF9;void*_TFA;struct Cyc_Absyn_Exp*_TFB;struct Cyc_RemoveAggrs_Env _TFC;struct Cyc_Absyn_Exp*_TFD;struct Cyc_Absyn_Exp*_TFE;struct Cyc_Absyn_Exp*_TFF;struct Cyc_Absyn_Exp*_T100;struct Cyc_Absyn_Stmt*_T101;int _T102;struct Cyc_RemoveAggrs_Env _T103;struct Cyc_Absyn_Exp*_T104;struct Cyc_Absyn_Exp*_T105;struct Cyc_Absyn_Exp*_T106;void*_T107;struct Cyc_Absyn_Exp*_T108;struct Cyc_Absyn_Exp*_T109;struct Cyc_Absyn_Exp*_T10A;void*_T10B;struct Cyc_Absyn_Exp*_T10C;struct Cyc_Absyn_Stmt*_T10D;struct Cyc_Absyn_Stmt*_T10E;struct Cyc_Absyn_Exp*_T10F;struct _tuple0*_T110;void*_T111;struct Cyc_Absyn_Exp*_T112;struct Cyc_Absyn_Stmt*_T113;struct Cyc_Absyn_Stmt*_T114;struct Cyc_Absyn_Exp*_T115;void*_T116;struct Cyc_Absyn_Exp*_T117;struct Cyc_Absyn_Stmt*_T118;struct Cyc_Absyn_Stmt*_T119;struct Cyc_Absyn_Exp*_T11A;struct Cyc_Absyn_Exp*_T11B;struct Cyc_RemoveAggrs_Env _T11C;struct Cyc_Warn_String_Warn_Warg_struct _T11D;int(*_T11E)(struct _fat_ptr);void*(*_T11F)(struct _fat_ptr);struct _fat_ptr _T120;struct Cyc_Absyn_Exp*_T121;struct Cyc_Absyn_Exp*_T122;struct Cyc_RemoveAggrs_Env _T123;struct Cyc_Absyn_Exp*_T124;struct Cyc_RemoveAggrs_Env _T125;struct Cyc_Absyn_Exp*_T126;struct Cyc_RemoveAggrs_Env _T127;struct Cyc_RemoveAggrs_Env _T128;struct Cyc_RemoveAggrs_Env _T129;struct Cyc_Absyn_Exp*_T12A;struct Cyc_RemoveAggrs_Env _T12B;struct Cyc_Absyn_Exp*_T12C;struct Cyc_RemoveAggrs_Env _T12D;struct Cyc_Absyn_Exp*_T12E;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T12F;struct Cyc_Absyn_VarargCallInfo*_T130;struct Cyc_RemoveAggrs_Env _T131;struct Cyc_List_List*_T132;void*_T133;struct Cyc_Absyn_Exp*_T134;struct Cyc_List_List*_T135;struct Cyc_Warn_String_Warn_Warg_struct _T136;struct Cyc_Warn_Exp_Warn_Warg_struct _T137;int(*_T138)(struct _fat_ptr);void*(*_T139)(struct _fat_ptr);struct _fat_ptr _T13A;struct Cyc_RemoveAggrs_Env _T13B;enum Cyc_RemoveAggrs_ExpContext _T13C;int _T13D;int _T13E;struct Cyc_Absyn_Exp*_T13F;struct Cyc_RemoveAggrs_Env _T140;struct Cyc_Absyn_Exp*_T141;struct Cyc_Absyn_Exp*_T142;struct Cyc_Absyn_Exp*_T143;struct Cyc_Absyn_Exp*_T144;struct Cyc_RemoveAggrs_Env _T145;enum Cyc_RemoveAggrs_ExpContext _T146;int _T147;int _T148;struct Cyc_Absyn_Exp*_T149;struct Cyc_RemoveAggrs_Env _T14A;struct Cyc_Absyn_Exp*_T14B;struct Cyc_Absyn_Exp*_T14C;struct Cyc_Absyn_Exp*_T14D;struct Cyc_Absyn_Exp*_T14E;void*_T14F;void*_T150;struct Cyc_Absyn_Exp*_T151;struct Cyc_Absyn_Exp*_T152;struct Cyc_Absyn_Exp*_T153;struct Cyc_RemoveAggrs_Result _T154;
enum Cyc_RemoveAggrs_ExpResult res=2U;
int did_assign=0;_T0=e;{
# 110
void*_T155=_T0->r;struct Cyc_Absyn_Stmt*_T156;struct Cyc_Absyn_Exp*_T157;int _T158;struct Cyc_Absyn_Exp*_T159;struct Cyc_Absyn_Vardecl*_T15A;struct Cyc_Absyn_Aggrdecl*_T15B;struct Cyc_List_List*_T15C;struct _tuple0*_T15D;struct Cyc_Absyn_Exp*_T15E;struct _fat_ptr _T15F;_T1=(int*)_T155;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T155;_T4=_T3->f1;_T5=_T4.String_c;_T6=_T5.tag;if(_T6!=8)goto _TLA;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T155;_T7=_T160->f1;_T8=_T7.String_c;_T15F=_T8.val;}{struct _fat_ptr s=_T15F;_T9=env;_TA=_T9.ctxt;_TB=(int)_TA;
# 113
if(_TB!=2)goto _TLC;_TC=e;_TD=_TC->topt;_TE=_check_null(_TD);_TF=Cyc_Tcutil_is_array_type(_TE);if(!_TF)goto _TLC;{
struct Cyc_List_List*dles=0;_T10=s;{
unsigned n=_get_fat_size(_T10,sizeof(char));{
unsigned i=0U;_TL11: if(i < n)goto _TLF;else{goto _TL10;}
_TLF: _T11=s;_T12=_T11.curr;_T13=(const char*)_T12;_T14=_check_null(_T13);_T15=i;_T16=(int)_T15;_T17=_T14[_T16];{struct Cyc_Absyn_Exp*c=Cyc_Absyn_char_exp(_T17,0U);_T18=c;
_T18->topt=Cyc_Absyn_char_type;{struct Cyc_List_List*_T160=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T161=_cycalloc(sizeof(struct _tuple11));
_T161->f0=0;_T161->f1=c;_T1A=(struct _tuple11*)_T161;}_T160->hd=_T1A;_T160->tl=dles;_T19=(struct Cyc_List_List*)_T160;}dles=_T19;}
# 116
i=i + 1;goto _TL11;_TL10:;}
# 121
dles=Cyc_List_imp_rev(dles);_T1B=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T160=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T160->tag=25;
_T160->f1=dles;_T1C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T160;}_T1B->r=(void*)_T1C;_T1D=
Cyc_RemoveAggrs_remove_aggrs_exp(env,e);return _T1D;}}_TLC: goto _LL0;}_TLA: goto _LL6;case 17: _LL6: goto _LL8;case 19: _LL8: goto _LLA;case 32: _LLA: goto _LLC;case 31: _LLC: goto _LLE;case 39: _LLE: goto _LL10;case 1: _LL10: goto _LL0;case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f2;}{struct Cyc_Absyn_Exp*e1=_T15E;_T1E=e;{
# 135
void*_T160=_T1E->annot;void*_T161;struct Cyc_Absyn_Exp*_T162;_T1F=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T160;_T20=_T1F->tag;_T21=Cyc_Toc_NewInfo;if(_T20!=_T21)goto _TL12;{struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T163=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T160;_T162=_T163->f1;_T22=_T163->f2;_T161=(void*)_T22;}{struct Cyc_Absyn_Exp*mexp=_T162;void*typ=_T161;_T23=env;_T24=_T23.ctxt;_T25=(int)_T24;
# 138
if(_T25!=0)goto _TL14;_T26=env;_T27=_T26.dest;_T28=_check_null(_T27);_T29=typ;_T2A=Cyc_RemoveAggrs_can_use_initializer(_T28,_T29);if(!_T2A)goto _TL14;{struct Cyc_RemoveAggrs_Env _T163;
_T163.ctxt=1U;_T2C=env;_T163.dest=_T2C.dest;_T2B=_T163;}_T2D=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T2B,_T2D);_T2E=e;_T2F=env;_T30=_T2F.dest;_T31=mexp;_T32=
Cyc_Absyn_assign_exp(_T30,_T31,0U);_T33=env;_T34=_T33.dest;_T35=_T34->topt;_T36=
_check_null(_T35);_T37=
# 140
Cyc_Absyn_set_type(_T32,_T36);_T38=e1;_T39=Cyc_Absyn_seq_exp(_T37,_T38,0U);_T2E->r=_T39->r;_T3A=e;_T3B=e1;
# 143
_T3A->topt=_T3B->topt;
res=1U;goto _LL41;_TL14: {
# 147
struct _tuple0*xvar=Cyc_Toc_temp_var();_T3C=
Cyc_Absyn_var_exp(xvar,0U);_T3D=typ;{struct Cyc_Absyn_Exp*xexp=Cyc_Absyn_set_type(_T3C,_T3D);{struct Cyc_RemoveAggrs_Env _T163;
_T163.ctxt=1U;_T163.dest=xexp;_T3E=_T163;}_T3F=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T3E,_T3F);{
struct Cyc_Absyn_Exp*body_exp=xexp;_T40=e;_T41=_T40->topt;
if(_T41==0)goto _TL16;_T42=e;_T43=_T42->topt;_T44=xexp;_T45=
Cyc_Absyn_cast_exp(_T43,_T44,0,1U,0U);_T46=e;_T47=_T46->topt;_T48=
_check_null(_T47);
# 152
body_exp=Cyc_Absyn_set_type(_T45,_T48);goto _TL17;_TL16: _TL17: _T49=xvar;_T4A=typ;_T4B=mexp;_T4C=
# 155
Cyc_Absyn_exp_stmt(e1,0U);_T4D=Cyc_Absyn_exp_stmt(body_exp,0U);_T4E=Cyc_Absyn_seq_stmt(_T4C,_T4D,0U);{
# 154
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T49,_T4A,_T4B,_T4E,0U);_T4F=e;_T50=
# 156
Cyc_Absyn_stmt_exp(s,0U);_T4F->r=_T50->r;goto _LL41;}}}}}_TL12: _T52=Cyc_Warn_impos;{
# 158
int(*_T163)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T52;_T51=_T163;}_T53=_tag_fat("removeAggrs: toc did not set a new-destination",sizeof(char),47U);_T54=_tag_fat(0U,sizeof(void*),0);_T51(_T53,_T54);_LL41:;}goto _LL0;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T155;_T15D=_T160->f1;_T15C=_T160->f3;_T15B=_T160->f4;}{struct _tuple0*tdn=_T15D;struct Cyc_List_List*dles=_T15C;struct Cyc_Absyn_Aggrdecl*sdopt=_T15B;
# 163
did_assign=1;_T56=env;_T57=_T56.ctxt;_T58=(int)_T57;
if(_T58==2)goto _TL18;_T59=env;_T5A=_T59.ctxt;_T5B=(int)_T5A;_T55=_T5B!=1;goto _TL19;_TL18: _T55=0;_TL19: {int do_stmt_exp=_T55;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;_T5D=sdopt;_T5E=(unsigned)_T5D;
if(!_T5E)goto _TL1A;_T5F=sdopt;_T60=_T5F->kind;_T61=(int)_T60;if(_T61!=1)goto _TL1A;_T5C=Cyc_Absyn_unionq_type;goto _TL1B;_TL1A: _T5C=Cyc_Absyn_strctq;_TL1B: {void*(*f)(struct _tuple0*)=_T5C;
void*ty=f(tdn);_T62=do_stmt_exp;
if(!_T62)goto _TL1C;{struct _tuple0**_T160=_cycalloc(sizeof(struct _tuple0*));
*_T160=Cyc_Toc_temp_var();_T63=(struct _tuple0**)_T160;}v=_T63;_T64=v;_T65=*_T64;_T66=
Cyc_Absyn_var_exp(_T65,0U);_T67=ty;dest=Cyc_Absyn_set_type(_T66,_T67);goto _TL1D;
# 173
_TL1C: v=0;_T68=env;_T69=_T68.dest;
dest=_check_null(_T69);_T6A=env;_T6B=_T6A.ctxt;_T6C=(int)_T6B;
if(_T6C!=1)goto _TL1E;_T6D=
Cyc_Absyn_deref_exp(dest,0U);_T6E=ty;dest=Cyc_Absyn_set_type(_T6D,_T6E);goto _TL1F;_TL1E: _TL1F: _TL1D:{
# 179
struct Cyc_List_List*dles2=dles;_TL23: if(dles2!=0)goto _TL21;else{goto _TL22;}
_TL21: _T6F=dles2;_T70=_T6F->hd;{struct _tuple11*_T160=(struct _tuple11*)_T70;struct Cyc_Absyn_Exp*_T161;struct Cyc_List_List*_T162;{struct _tuple11 _T163=*_T160;_T162=_T163.f0;_T161=_T163.f1;}{struct Cyc_List_List*ds=_T162;struct Cyc_Absyn_Exp*field_exp=_T161;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);_T71=
Cyc_RemoveAggrs_deep_copy(dest);_T72=f;_T73=Cyc_RemoveAggrs_member_exp(_T71,_T72,0U);_T74=field_exp;_T75=_T74->topt;_T76=
_check_null(_T75);{
# 182
struct Cyc_Absyn_Exp*field_dest=Cyc_Absyn_set_type(_T73,_T76);{struct Cyc_RemoveAggrs_Env _T163;
# 184
_T163.ctxt=2U;_T163.dest=field_dest;_T77=_T163;}_T78=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_T77,_T78);}}}_T79=dles2;
# 179
dles2=_T79->tl;goto _TL23;_TL22:;}
# 187
if(dles!=0)goto _TL24;_T7B=Cyc_Warn_impos;{
int(*_T160)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7B;_T7A=_T160;}_T7C=_tag_fat("zero-field aggregate",sizeof(char),21U);_T7D=_tag_fat(0U,sizeof(void*),0);_T7A(_T7C,_T7D);goto _TL25;_TL24: _TL25: _T7E=dles;_T7F=_T7E->hd;_T80=(struct _tuple11*)_T7F;_T81=*_T80;{
# 190
struct Cyc_Absyn_Exp*init_e=_T81.f1;_T82=dles;
dles=_T82->tl;_TL29: if(dles!=0)goto _TL27;else{goto _TL28;}
_TL27: _T83=dles;_T84=_T83->hd;_T85=(struct _tuple11*)_T84;_T86=*_T85;{struct Cyc_Absyn_Exp*e2=_T86.f1;_T87=e2;_T88=_T87->topt;
if(_T88!=0)goto _TL2A;{struct Cyc_Warn_String_Warn_Warg_struct _T160;_T160.tag=0;
_T160.f1=_tag_fat("expression ",sizeof(char),12U);_T89=_T160;}{struct Cyc_Warn_String_Warn_Warg_struct _T160=_T89;{struct Cyc_Warn_Exp_Warn_Warg_struct _T161;_T161.tag=4;_T161.f1=e2;_T8A=_T161;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T161=_T8A;{struct Cyc_Warn_String_Warn_Warg_struct _T162;_T162.tag=0;_T162.f1=_tag_fat(" missing type!",sizeof(char),15U);_T8B=_T162;}{struct Cyc_Warn_String_Warn_Warg_struct _T162=_T8B;void*_T163[3];_T163[0]=& _T160;_T163[1]=& _T161;_T163[2]=& _T162;_T8D=Cyc_Warn_impos2;{int(*_T164)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T8D;_T8C=_T164;}_T8E=_tag_fat(_T163,sizeof(void*),3);_T8C(_T8E);}}}goto _TL2B;_TL2A: _TL2B: _T8F=
Cyc_Absyn_seq_exp(init_e,e2,0U);_T90=e2;_T91=_T90->topt;_T92=_check_null(_T91);init_e=Cyc_Absyn_set_type(_T8F,_T92);}_T93=dles;
# 191
dles=_T93->tl;goto _TL29;_TL28: _T94=do_stmt_exp;
# 197
if(!_T94)goto _TL2C;_T95=e;_T96=
_check_null(v);_T97=*_T96;_T98=ty;_T99=
Cyc_Absyn_exp_stmt(init_e,0U);_T9A=
Cyc_Absyn_exp_stmt(dest,0U);_T9B=
# 199
Cyc_Absyn_seq_stmt(_T99,_T9A,0U);_T9C=
# 198
Cyc_Absyn_declare_stmt(_T97,_T98,0,_T9B,0U);_T9D=Cyc_Absyn_stmt_exp(_T9C,0U);_T95->r=_T9D->r;goto _TL2D;
# 202
_TL2C: _T9E=e;_T9F=init_e;_T9E->r=_T9F->r;_TA0=e;_TA1=init_e;
_TA0->topt=_TA1->topt;_TL2D: goto _LL0;}}}}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T155;_T15C=_T160->f1;}{struct Cyc_List_List*dles=_T15C;
# 211
res=0U;
if(dles!=0)goto _TL2E;_TA2=e;_TA3=
Cyc_Absyn_signed_int_exp(0,0U);_TA4=Cyc_Absyn_sint_type;_TA5=Cyc_Absyn_set_type(_TA3,_TA4);*_TA2=*_TA5;goto _LL0;_TL2E: _TA6=env;{
# 216
enum Cyc_RemoveAggrs_ExpContext _T160=_TA6.ctxt;if(_T160!=Cyc_RemoveAggrs_Other)goto _TL30;_TA8=Cyc_Warn_impos;{
int(*_T161)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TA8;_TA7=_T161;}_TA9=_tag_fat("remove-aggrs: Array_e in bad position",sizeof(char),38U);_TAA=_tag_fat(0U,sizeof(void*),0);_TA7(_TA9,_TAA);goto _TL31;_TL30: goto _LL49;_TL31: _LL49:;}
# 220
did_assign=1;_TAB=env;_TAC=_TAB.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TAC);
void*dest_type;_TAD=dest;_TAE=_TAD->topt;_TAF=
_check_null(_TAE);{void*_T160=Cyc_Absyn_compress(_TAF);struct Cyc_Absyn_ArrayInfo _T161;_TB0=(int*)_T160;_TB1=*_TB0;if(_TB1!=5)goto _TL32;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T162=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T160;_T161=_T162->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T161;_TB2=ai;_TB3=_TB2.elt_type;_TB4=ai;_TB5=_TB4.tq;
dest_type=Cyc_Absyn_cstar_type(_TB3,_TB5);goto _LL4E;}_TL32: _TB6=dest;_TB7=_TB6->topt;
dest_type=_check_null(_TB7);goto _LL4E;_LL4E:;}{
# 228
int i=0;{
struct Cyc_List_List*dles2=dles;_TL37: if(dles2!=0)goto _TL35;else{goto _TL36;}
# 232
_TL35: _TB8=dles2;_TB9=_TB8->hd;{struct _tuple11*_T160=(struct _tuple11*)_TB9;struct Cyc_Absyn_Exp*_T161;{struct _tuple11 _T162=*_T160;_T161=_T162.f1;}{struct Cyc_Absyn_Exp*field_exp=_T161;_TBA=
Cyc_RemoveAggrs_deep_copy(dest);_TBB=
Cyc_Absyn_signed_int_exp(i,0U);_TBC=Cyc_Absyn_sint_type;_TBD=Cyc_Absyn_set_type(_TBB,_TBC);_TBE=
# 233
Cyc_Absyn_subscript_exp(_TBA,_TBD,0U);_TBF=dest_type;{struct Cyc_Absyn_Exp*fielddest=Cyc_Absyn_set_type(_TBE,_TBF);{struct Cyc_RemoveAggrs_Env _T162;
# 237
_T162.ctxt=2U;_T162.dest=fielddest;_TC0=_T162;}_TC1=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_TC0,_TC1);}}}_TC2=dles2;
# 229
dles2=_TC2->tl;i=i + 1;goto _TL37;_TL36:;}_TC3=dles;_TC4=_TC3->hd;_TC5=(struct _tuple11*)_TC4;_TC6=*_TC5;{
# 239
struct Cyc_Absyn_Exp*init_e=_TC6.f1;_TC7=dles;
dles=_TC7->tl;_TL3B: if(dles!=0)goto _TL39;else{goto _TL3A;}
_TL39: _TC8=dles;_TC9=_TC8->hd;_TCA=(struct _tuple11*)_TC9;_TCB=*_TCA;{struct Cyc_Absyn_Exp*e2=_TCB.f1;_TCC=
Cyc_Absyn_seq_exp(init_e,e2,0U);_TCD=e2;_TCE=_TCD->topt;_TCF=_check_null(_TCE);init_e=Cyc_Absyn_set_type(_TCC,_TCF);}_TD0=dles;
# 240
dles=_TD0->tl;goto _TL3B;_TL3A: _TD1=e;_TD2=init_e;
# 244
_TD1->r=_TD2->r;_TD3=e;_TD4=init_e;
_TD3->topt=_TD4->topt;goto _LL0;}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T155;_T15A=_T160->f1;_T15E=_T160->f2;_T159=_T160->f3;_T158=_T160->f4;}{struct Cyc_Absyn_Vardecl*vd=_T15A;struct Cyc_Absyn_Exp*bd=_T15E;struct Cyc_Absyn_Exp*body=_T159;int zero_term=_T158;
# 251
did_assign=1;
res=0U;_TD5=env;{
enum Cyc_RemoveAggrs_ExpContext _T160=_TD5.ctxt;if(_T160!=Cyc_RemoveAggrs_Other)goto _TL3C;_TD7=Cyc_Warn_impos;{
int(*_T161)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD7;_TD6=_T161;}_TD8=_tag_fat("remove-aggrs: comprehension in bad position",sizeof(char),44U);_TD9=_tag_fat(0U,sizeof(void*),0);_TD6(_TD8,_TD9);goto _TL3D;_TL3C: goto _LL56;_TL3D: _LL56:;}{
# 257
struct _tuple0*max=Cyc_Toc_temp_var();_TDA=vd;{
struct _tuple0*i=_TDA->name;_TDB=
Cyc_Absyn_var_exp(i,0U);_TDC=Cyc_Absyn_sint_type;_TDD=Cyc_Absyn_set_type(_TDB,_TDC);_TDE=
Cyc_Absyn_signed_int_exp(0,0U);_TDF=Cyc_Absyn_sint_type;_TE0=Cyc_Absyn_set_type(_TDE,_TDF);{
# 259
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(_TDD,_TE0,0U);_TE1=
# 261
Cyc_Absyn_var_exp(i,0U);_TE2=Cyc_Absyn_uint_type;_TE3=Cyc_Absyn_set_type(_TE1,_TE2);_TE4=
Cyc_Absyn_var_exp(max,0U);_TE5=Cyc_Absyn_uint_type;_TE6=Cyc_Absyn_set_type(_TE4,_TE5);{
# 261
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(_TE3,_TE6,0U);_TE7=
# 263
Cyc_Absyn_var_exp(i,0U);_TE8=Cyc_Absyn_uint_type;_TE9=Cyc_Absyn_set_type(_TE7,_TE8);{struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(_TE9,0U,0U);_TEA=env;_TEB=_TEA.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TEB);
void*dest_type;_TEC=dest;_TED=_TEC->topt;_TEE=
_check_null(_TED);{void*_T160=Cyc_Absyn_compress(_TEE);struct Cyc_Absyn_ArrayInfo _T161;_TEF=(int*)_T160;_TF0=*_TEF;if(_TF0!=5)goto _TL3E;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T162=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T160;_T161=_T162->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T161;_TF1=ai;_TF2=_TF1.elt_type;_TF3=ai;_TF4=_TF3.tq;
dest_type=Cyc_Absyn_cstar_type(_TF2,_TF4);goto _LL5B;}_TL3E: _TF5=dest;_TF6=_TF5->topt;
dest_type=_check_null(_TF6);_LL5B:;}_TF7=env;_TF8=_TF7.dest;_TF9=
# 270
Cyc_Absyn_var_exp(i,0U);_TFA=Cyc_Absyn_uint_type;_TFB=Cyc_Absyn_set_type(_TF9,_TFA);{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(_TF8,_TFB,0U);
Cyc_Absyn_set_type(lval,dest_type);{struct Cyc_RemoveAggrs_Env _T160;
_T160.ctxt=2U;_T160.dest=Cyc_RemoveAggrs_deep_copy(lval);_TFC=_T160;}_TFD=body;Cyc_RemoveAggrs_remove_aggrs_exp(_TFC,_TFD);_TFE=ea;_TFF=eb;_T100=ec;_T101=
Cyc_Absyn_exp_stmt(body,0U);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TFE,_TFF,_T100,_T101,0U);_T102=zero_term;
if(!_T102)goto _TL40;_T103=env;_T104=_T103.dest;_T105=
# 281
Cyc_RemoveAggrs_deep_copy(_T104);_T106=
Cyc_Absyn_var_exp(max,0U);_T107=Cyc_Absyn_sint_type;_T108=Cyc_Absyn_set_type(_T106,_T107);_T109=
# 281
Cyc_Absyn_subscript_exp(_T105,_T108,0U);_T10A=
# 283
Cyc_Absyn_signed_int_exp(0,0U);_T10B=Cyc_Absyn_sint_type;_T10C=Cyc_Absyn_set_type(_T10A,_T10B);{
# 281
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(_T109,_T10C,0U);_T10D=s;_T10E=
# 284
Cyc_Absyn_exp_stmt(ex,0U);s=Cyc_Absyn_seq_stmt(_T10D,_T10E,0U);}goto _TL41;_TL40: _TL41:
# 287
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);_T10F=e;_T110=max;_T111=Cyc_Absyn_uint_type;_T112=bd;_T113=
# 289
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U);_T114=
# 288
Cyc_Absyn_declare_stmt(_T110,_T111,_T112,_T113,0U);_T115=
# 290
Cyc_Absyn_signed_int_exp(0,0U);_T116=Cyc_Absyn_sint_type;_T117=Cyc_Absyn_set_type(_T115,_T116);_T118=Cyc_Absyn_exp_stmt(_T117,0U);_T119=
# 288
Cyc_Absyn_seq_stmt(_T114,_T118,0U);_T11A=Cyc_Absyn_stmt_exp(_T119,0U);_T10F->r=_T11A->r;_T11B=e;
# 291
_T11B->topt=Cyc_Absyn_sint_type;goto _LL0;}}}}}}}}}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}{struct Cyc_Absyn_Exp*bd=_T15E;
# 295
did_assign=1;
res=0U;_T11C=env;{
enum Cyc_RemoveAggrs_ExpContext _T160=_T11C.ctxt;if(_T160!=Cyc_RemoveAggrs_Other)goto _TL42;{struct Cyc_Warn_String_Warn_Warg_struct _T161;_T161.tag=0;
_T161.f1=_tag_fat("remove-aggrs: no-init-comp in bad position",sizeof(char),43U);_T11D=_T161;}{struct Cyc_Warn_String_Warn_Warg_struct _T161=_T11D;void*_T162[1];_T162[0]=& _T161;_T11F=Cyc_Warn_impos2;{int(*_T163)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T11F;_T11E=_T163;}_T120=_tag_fat(_T162,sizeof(void*),1);_T11E(_T120);}goto _TL43;_TL42: goto _LL60;_TL43: _LL60:;}_T121=e;_T122=bd;
# 302
*_T121=*_T122;goto _LL0;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f3;}{struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;
# 306
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f2;}{struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;
# 310
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T123=
Cyc_RemoveAggrs_no_init(env);_T124=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T123,_T124);goto _LL0;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f2;_T157=_T160->f3;}{struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;struct Cyc_Absyn_Exp*e3=_T157;
# 315
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T125=
Cyc_RemoveAggrs_no_init(env);_T126=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T125,_T126);{struct Cyc_RemoveAggrs_Env _T160;_T128=env;
_T160.ctxt=_T128.ctxt;_T129=env;_T12A=_T129.dest;if(_T12A!=0)goto _TL44;_T160.dest=0;goto _TL45;_TL44: _T12B=env;_T12C=_T12B.dest;_T160.dest=Cyc_RemoveAggrs_deep_copy(_T12C);_TL45: _T127=_T160;}{struct Cyc_RemoveAggrs_Env env3=_T127;_T12D=
Cyc_RemoveAggrs_no_init(env3);_T12E=e3;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T12D,_T12E);goto _LL0;}}case 10: _T12F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T155;_T130=_T12F->f3;if(_T130!=0)goto _TL46;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T15C=_T160->f2;}{struct Cyc_Absyn_Exp*e=_T15E;struct Cyc_List_List*es=_T15C;
# 323
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);_T15C=es;goto _LL24;}_TL46: goto _LL3F;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T155;_T15C=_T160->f2;}_LL24: {struct Cyc_List_List*es=_T15C;
# 326
_TL4B: if(es!=0)goto _TL49;else{goto _TL4A;}
_TL49: _T131=Cyc_RemoveAggrs_other_env;_T132=es;_T133=_T132->hd;_T134=(struct Cyc_Absyn_Exp*)_T133;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T131,_T134);_T135=es;
# 326
es=_T135->tl;goto _TL4B;_TL4A: goto _LL0;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f2;}{struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;_T15E=e1;_T159=e2;goto _LL28;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f2;}_LL28: {struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;_T15E=e1;_T159=e2;goto _LL2A;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;_T159=_T160->f2;}_LL2A: {struct Cyc_Absyn_Exp*e1=_T15E;struct Cyc_Absyn_Exp*e2=_T159;
# 333
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}{struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL2E;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL2E: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL30;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f2;}_LL30: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL32;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL32: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL34;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL34: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL36;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL36: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL38;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL38: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL3A;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL3A: {struct Cyc_Absyn_Exp*e1=_T15E;_T15E=e1;goto _LL3C;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T155;_T15E=_T160->f1;}_LL3C: {struct Cyc_Absyn_Exp*e1=_T15E;
# 346
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T160=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T155;_T156=_T160->f1;}{struct Cyc_Absyn_Stmt*s=_T156;
# 349
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:{struct Cyc_Warn_String_Warn_Warg_struct _T160;_T160.tag=0;
# 351
_T160.f1=_tag_fat("bad exp after translation to C: ",sizeof(char),33U);_T136=_T160;}{struct Cyc_Warn_String_Warn_Warg_struct _T160=_T136;{struct Cyc_Warn_Exp_Warn_Warg_struct _T161;_T161.tag=4;_T161.f1=e;_T137=_T161;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T161=_T137;void*_T162[2];_T162[0]=& _T160;_T162[1]=& _T161;_T139=Cyc_Warn_impos2;{int(*_T163)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T139;_T138=_T163;}_T13A=_tag_fat(_T162,sizeof(void*),2);_T138(_T13A);}}}_LL0:;}_T13B=env;_T13C=_T13B.ctxt;_T13D=(int)_T13C;
# 353
if(_T13D!=2)goto _TL4C;_T13E=did_assign;if(_T13E)goto _TL4C;else{goto _TL4E;}
_TL4E: _T13F=e;_T140=env;_T141=_T140.dest;_T142=_check_null(_T141);_T143=Cyc_RemoveAggrs_deep_copy(e);_T144=Cyc_Absyn_assign_exp(_T142,_T143,0U);_T13F->r=_T144->r;goto _TL4D;_TL4C: _TL4D: _T145=env;_T146=_T145.ctxt;_T147=(int)_T146;
if(_T147!=1)goto _TL4F;_T148=did_assign;if(_T148)goto _TL4F;else{goto _TL51;}
_TL51: _T149=e;_T14A=env;_T14B=_T14A.dest;_T14C=_check_null(_T14B);_T14D=Cyc_Absyn_deref_exp(_T14C,0U);_T14E=e;_T14F=_T14E->topt;_T150=_check_null(_T14F);_T151=Cyc_Absyn_set_type(_T14D,_T150);_T152=
Cyc_RemoveAggrs_deep_copy(e);_T153=
# 356
Cyc_Absyn_assign_exp(_T151,_T152,0U);_T149->r=_T153->r;goto _TL50;_TL4F: _TL50:{struct Cyc_RemoveAggrs_Result _T155;
# 359
_T155.res=res;_T154=_T155;}return _T154;}
# 366
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f1(int bogus,struct Cyc_Absyn_Exp*e){
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
return 0;}
# 370
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f2(int bogus,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;int _T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Vardecl*_T6;enum Cyc_Absyn_Scope _T7;int _T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_TB;void*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Vardecl*_TE;struct Cyc_RemoveAggrs_Env _TF;struct Cyc_Absyn_Vardecl*_T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Exp*_T12;enum Cyc_RemoveAggrs_ExpResult _T13;int _T14;enum Cyc_RemoveAggrs_ExpResult _T15;int _T16;struct Cyc_Absyn_Stmt*_T17;struct Cyc_Absyn_Decl*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_Absyn_Stmt*_T1C;struct Cyc_Absyn_Stmt*_T1D;struct Cyc_Absyn_Stmt*_T1E;struct Cyc_Absyn_Stmt*_T1F;struct Cyc_Absyn_Vardecl*_T20;struct Cyc_Absyn_Fndecl*_T21;struct Cyc_Absyn_Stmt*_T22;int(*_T23)(struct _fat_ptr,struct _fat_ptr);void*(*_T24)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T25;struct _fat_ptr _T26;_T0=s;{
void*_T27=_T0->r;struct Cyc_Absyn_Stmt*_T28;struct Cyc_Absyn_Decl*_T29;_T1=(int*)_T27;_T2=*_T1;if(_T2!=12)goto _TL52;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T2A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T27;_T29=_T2A->f1;_T28=_T2A->f2;}{struct Cyc_Absyn_Decl*d=_T29;struct Cyc_Absyn_Stmt*s2=_T28;
# 373
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);_T3=d;{
void*_T2A=_T3->r;struct Cyc_Absyn_Fndecl*_T2B;struct Cyc_Absyn_Vardecl*_T2C;_T4=(int*)_T2A;_T5=*_T4;switch(_T5){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2A;_T2C=_T2D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2C;_T6=vd;_T7=_T6->sc;_T8=(int)_T7;
# 376
if(_T8!=0)goto _TL55;return 0;_TL55: _T9=vd;_TA=_T9->initializer;
# 384
if(_TA==0)goto _TL57;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2D=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T2D->tag=4;
_T2D->f1=vd;_TB=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T2D;}_TC=(void*)_TB;{struct Cyc_Absyn_Exp*var_exp=Cyc_Absyn_varb_exp(_TC,0U);_TD=var_exp;_TE=vd;
_TD->topt=_TE->type;{struct Cyc_RemoveAggrs_Env _T2D;
_T2D.ctxt=0U;_T2D.dest=var_exp;_TF=_T2D;}_T10=vd;_T11=_T10->initializer;_T12=
_check_null(_T11);{
# 387
struct Cyc_RemoveAggrs_Result _T2D=Cyc_RemoveAggrs_remove_aggrs_exp(_TF,_T12);enum Cyc_RemoveAggrs_ExpResult _T2E;_T2E=_T2D.res;{enum Cyc_RemoveAggrs_ExpResult r=_T2E;_T13=r;_T14=(int)_T13;
# 389
if(_T14==0)goto _TL5B;else{goto _TL5C;}_TL5C: _T15=r;_T16=(int)_T15;if(_T16==1)goto _TL5B;else{goto _TL59;}
_TL5B: _T17=s;_T18=d;_T19=vd;_T1A=_T19->initializer;_T1B=_check_null(_T1A);_T1C=Cyc_Absyn_exp_stmt(_T1B,0U);_T1D=s2;_T1E=Cyc_Absyn_seq_stmt(_T1C,_T1D,0U);_T1F=Cyc_Absyn_decl_stmt(_T18,_T1E,0U);_T17->r=_T1F->r;_T20=vd;
# 392
_T20->initializer=0;goto _TL5A;_TL59: _TL5A:;}}}goto _TL58;_TL57: _TL58:
# 395
 return 0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2A;_T2B=_T2D->f1;}{struct Cyc_Absyn_Fndecl*fd=_T2B;_T21=fd;_T22=_T21->body;
Cyc_RemoveAggrs_remove_aggrs_stmt(_T22);return 0;}default: _T24=Cyc_Warn_impos;{
int(*_T2D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T24;_T23=_T2D;}_T25=_tag_fat("bad local declaration after xlation to C",sizeof(char),41U);_T26=_tag_fat(0U,sizeof(void*),0);_T23(_T25,_T26);};}}goto _TL53;_TL52:
# 399
 return 1;_TL53:;}}
# 402
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Absyn_Stmt*_T2;_T1=Cyc_Absyn_visit_stmt;{
void(*_T3)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))_T1;_T0=_T3;}_T2=s;_T0(Cyc_RemoveAggrs_remove_aggrs_stmt_f1,Cyc_RemoveAggrs_remove_aggrs_stmt_f2,0,_T2);}
# 406
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Fndecl*_T5;enum Cyc_Absyn_Scope _T6;int _T7;struct Cyc_Absyn_Fndecl*_T8;struct Cyc_Absyn_Stmt*_T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;{
struct Cyc_List_List*ds2=ds;_TL60: if(ds2!=0)goto _TL5E;else{goto _TL5F;}
_TL5E: _T0=ds2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_T14=_T2->r;struct Cyc_Absyn_Fndecl*_T15;_T3=(int*)_T14;_T4=*_T3;switch(_T4){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T14;_T15=_T16->f1;}{struct Cyc_Absyn_Fndecl*fd=_T15;_T5=fd;_T6=_T5->orig_scope;_T7=(int)_T6;
# 410
if(_T7==4)goto _TL62;_T8=fd;_T9=_T8->body;
Cyc_RemoveAggrs_remove_aggrs_stmt(_T9);goto _TL63;_TL62: _TL63: goto _LL0;}case 0: goto _LL6;case 5: _LL6: goto _LL8;case 7: _LL8: goto _LLA;case 8: _LLA: goto _LL0;case 4: goto _LLE;case 6: _LLE: goto _LL10;case 2: _LL10: goto _LL12;case 3: _LL12: _TB=Cyc_Warn_impos;{
# 420
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T16;}_TC=_tag_fat("Cyclone decl after xlation to C",sizeof(char),32U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);case 9: goto _LL16;case 10: _LL16: goto _LL18;case 11: _LL18: goto _LL1A;case 12: _LL1A: goto _LL1C;case 13: _LL1C: goto _LL1E;case 14: _LL1E: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: _TF=Cyc_Warn_impos;{
# 428
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T16;}_T10=_tag_fat("translation unit after xlation to C",sizeof(char),36U);_T11=_tag_fat(0U,sizeof(void*),0);_TE(_T10,_T11);}_LL0:;}_T12=ds2;
# 407
ds2=_T12->tl;goto _TL60;_TL5F:;}_T13=ds;
# 430
return _T13;}
