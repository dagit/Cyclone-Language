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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 941 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 959
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 961
extern void*Cyc_Absyn_sint_type;
# 1027
void*Cyc_Absyn_cstar_type(void*,struct Cyc_Absyn_Tqual);
# 1032
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1097
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1102
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1109
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1114
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1119
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1136
struct Cyc_Absyn_Exp*Cyc_Absyn_set_type(struct Cyc_Absyn_Exp*,void*);
# 1141
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1149
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1152
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1240
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1243
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
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_String_c _T5;unsigned _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_String_c _T8;struct Cyc_RemoveAggrs_Env _T9;enum Cyc_RemoveAggrs_ExpContext _TA;int _TB;struct Cyc_Absyn_Exp*_TC;void*_TD;void*_TE;int _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;unsigned char*_T12;const char*_T13;unsigned _T14;int _T15;char _T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_List_List*_T18;struct _tuple11*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T1B;struct Cyc_RemoveAggrs_Result _T1C;struct Cyc_Absyn_Exp*_T1D;struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T1E;char*_T1F;char*_T20;void*_T21;struct Cyc_RemoveAggrs_Env _T22;enum Cyc_RemoveAggrs_ExpContext _T23;int _T24;struct Cyc_RemoveAggrs_Env _T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_Absyn_Exp*_T27;void*_T28;int _T29;struct Cyc_RemoveAggrs_Env _T2A;struct Cyc_RemoveAggrs_Env _T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Exp*_T2D;struct Cyc_RemoveAggrs_Env _T2E;struct Cyc_Absyn_Exp*_T2F;struct Cyc_Absyn_Exp*_T30;struct Cyc_Absyn_Exp*_T31;struct Cyc_RemoveAggrs_Env _T32;struct Cyc_Absyn_Exp*_T33;void*_T34;void*_T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Exp*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Exp*_T3A;struct Cyc_Absyn_Exp*_T3B;void*_T3C;struct Cyc_RemoveAggrs_Env _T3D;struct Cyc_Absyn_Exp*_T3E;struct Cyc_Absyn_Exp*_T3F;void*_T40;struct Cyc_Absyn_Exp*_T41;void*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Exp*_T44;struct Cyc_Absyn_Exp*_T45;void*_T46;void*_T47;struct _tuple0*_T48;void*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_Absyn_Stmt*_T4B;struct Cyc_Absyn_Stmt*_T4C;struct Cyc_Absyn_Stmt*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_Absyn_Exp*_T4F;int(*_T50)(struct _fat_ptr,struct _fat_ptr);void*(*_T51)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T52;struct _fat_ptr _T53;int _T54;struct Cyc_RemoveAggrs_Env _T55;enum Cyc_RemoveAggrs_ExpContext _T56;int _T57;struct Cyc_RemoveAggrs_Env _T58;enum Cyc_RemoveAggrs_ExpContext _T59;int _T5A;void*(*_T5B)(struct _tuple0*);struct Cyc_Absyn_Aggrdecl*_T5C;unsigned _T5D;struct Cyc_Absyn_Aggrdecl*_T5E;enum Cyc_Absyn_AggrKind _T5F;int _T60;int _T61;struct _tuple0**_T62;struct _tuple0**_T63;struct _tuple0*_T64;struct Cyc_Absyn_Exp*_T65;void*_T66;struct Cyc_RemoveAggrs_Env _T67;struct Cyc_Absyn_Exp*_T68;struct Cyc_RemoveAggrs_Env _T69;enum Cyc_RemoveAggrs_ExpContext _T6A;int _T6B;struct Cyc_Absyn_Exp*_T6C;void*_T6D;struct Cyc_List_List*_T6E;void*_T6F;struct Cyc_Absyn_Exp*_T70;struct _fat_ptr*_T71;struct Cyc_Absyn_Exp*_T72;struct Cyc_Absyn_Exp*_T73;void*_T74;void*_T75;struct Cyc_RemoveAggrs_Env _T76;struct Cyc_Absyn_Exp*_T77;struct Cyc_List_List*_T78;int(*_T79)(struct _fat_ptr,struct _fat_ptr);void*(*_T7A)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7B;struct _fat_ptr _T7C;struct Cyc_List_List*_T7D;void*_T7E;struct _tuple11*_T7F;struct _tuple11 _T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;void*_T83;struct _tuple11*_T84;struct _tuple11 _T85;struct Cyc_Absyn_Exp*_T86;void*_T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;struct Cyc_Warn_Exp_Warn_Warg_struct _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;int(*_T8B)(struct _fat_ptr);void*(*_T8C)(struct _fat_ptr);struct _fat_ptr _T8D;struct Cyc_Absyn_Exp*_T8E;struct Cyc_Absyn_Exp*_T8F;void*_T90;void*_T91;struct Cyc_List_List*_T92;int _T93;struct Cyc_Absyn_Exp*_T94;struct _tuple0**_T95;struct _tuple0*_T96;void*_T97;struct Cyc_Absyn_Stmt*_T98;struct Cyc_Absyn_Stmt*_T99;struct Cyc_Absyn_Stmt*_T9A;struct Cyc_Absyn_Stmt*_T9B;struct Cyc_Absyn_Exp*_T9C;struct Cyc_Absyn_Exp*_T9D;struct Cyc_Absyn_Exp*_T9E;struct Cyc_Absyn_Exp*_T9F;struct Cyc_Absyn_Exp*_TA0;struct Cyc_Absyn_Exp*_TA1;struct Cyc_Absyn_Exp*_TA2;void*_TA3;struct Cyc_Absyn_Exp*_TA4;struct Cyc_RemoveAggrs_Env _TA5;int(*_TA6)(struct _fat_ptr,struct _fat_ptr);void*(*_TA7)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA8;struct _fat_ptr _TA9;struct Cyc_RemoveAggrs_Env _TAA;struct Cyc_Absyn_Exp*_TAB;struct Cyc_Absyn_Exp*_TAC;void*_TAD;void*_TAE;int*_TAF;int _TB0;struct Cyc_Absyn_ArrayInfo _TB1;void*_TB2;struct Cyc_Absyn_ArrayInfo _TB3;struct Cyc_Absyn_Tqual _TB4;struct Cyc_Absyn_Exp*_TB5;void*_TB6;struct Cyc_List_List*_TB7;void*_TB8;struct Cyc_Absyn_Exp*_TB9;struct Cyc_Absyn_Exp*_TBA;void*_TBB;struct Cyc_Absyn_Exp*_TBC;struct Cyc_Absyn_Exp*_TBD;void*_TBE;struct Cyc_RemoveAggrs_Env _TBF;struct Cyc_Absyn_Exp*_TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;void*_TC3;struct _tuple11*_TC4;struct _tuple11 _TC5;struct Cyc_List_List*_TC6;struct Cyc_List_List*_TC7;void*_TC8;struct _tuple11*_TC9;struct _tuple11 _TCA;struct Cyc_Absyn_Exp*_TCB;struct Cyc_Absyn_Exp*_TCC;void*_TCD;void*_TCE;struct Cyc_List_List*_TCF;struct Cyc_Absyn_Exp*_TD0;struct Cyc_Absyn_Exp*_TD1;struct Cyc_Absyn_Exp*_TD2;struct Cyc_Absyn_Exp*_TD3;struct Cyc_RemoveAggrs_Env _TD4;int(*_TD5)(struct _fat_ptr,struct _fat_ptr);void*(*_TD6)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD7;struct _fat_ptr _TD8;struct Cyc_Absyn_Vardecl*_TD9;struct Cyc_Absyn_Exp*_TDA;void*_TDB;struct Cyc_Absyn_Exp*_TDC;struct Cyc_Absyn_Exp*_TDD;void*_TDE;struct Cyc_Absyn_Exp*_TDF;struct Cyc_Absyn_Exp*_TE0;void*_TE1;struct Cyc_Absyn_Exp*_TE2;struct Cyc_Absyn_Exp*_TE3;void*_TE4;struct Cyc_Absyn_Exp*_TE5;struct Cyc_Absyn_Exp*_TE6;void*_TE7;struct Cyc_Absyn_Exp*_TE8;struct Cyc_RemoveAggrs_Env _TE9;struct Cyc_Absyn_Exp*_TEA;struct Cyc_Absyn_Exp*_TEB;void*_TEC;void*_TED;int*_TEE;int _TEF;struct Cyc_Absyn_ArrayInfo _TF0;void*_TF1;struct Cyc_Absyn_ArrayInfo _TF2;struct Cyc_Absyn_Tqual _TF3;struct Cyc_Absyn_Exp*_TF4;void*_TF5;struct Cyc_RemoveAggrs_Env _TF6;struct Cyc_Absyn_Exp*_TF7;struct Cyc_Absyn_Exp*_TF8;void*_TF9;struct Cyc_Absyn_Exp*_TFA;struct Cyc_RemoveAggrs_Env _TFB;struct Cyc_Absyn_Exp*_TFC;struct Cyc_Absyn_Exp*_TFD;struct Cyc_Absyn_Exp*_TFE;struct Cyc_Absyn_Exp*_TFF;struct Cyc_Absyn_Stmt*_T100;int _T101;struct Cyc_RemoveAggrs_Env _T102;struct Cyc_Absyn_Exp*_T103;struct Cyc_Absyn_Exp*_T104;struct Cyc_Absyn_Exp*_T105;void*_T106;struct Cyc_Absyn_Exp*_T107;struct Cyc_Absyn_Exp*_T108;struct Cyc_Absyn_Exp*_T109;void*_T10A;struct Cyc_Absyn_Exp*_T10B;struct Cyc_Absyn_Stmt*_T10C;struct Cyc_Absyn_Stmt*_T10D;struct Cyc_Absyn_Exp*_T10E;struct _tuple0*_T10F;void*_T110;struct Cyc_Absyn_Exp*_T111;struct Cyc_Absyn_Stmt*_T112;struct Cyc_Absyn_Stmt*_T113;struct Cyc_Absyn_Exp*_T114;void*_T115;struct Cyc_Absyn_Exp*_T116;struct Cyc_Absyn_Stmt*_T117;struct Cyc_Absyn_Stmt*_T118;struct Cyc_Absyn_Exp*_T119;struct Cyc_Absyn_Exp*_T11A;struct Cyc_RemoveAggrs_Env _T11B;struct Cyc_Warn_String_Warn_Warg_struct _T11C;int(*_T11D)(struct _fat_ptr);void*(*_T11E)(struct _fat_ptr);struct _fat_ptr _T11F;struct Cyc_Absyn_Exp*_T120;struct Cyc_Absyn_Exp*_T121;struct Cyc_RemoveAggrs_Env _T122;struct Cyc_Absyn_Exp*_T123;struct Cyc_RemoveAggrs_Env _T124;struct Cyc_Absyn_Exp*_T125;struct Cyc_RemoveAggrs_Env _T126;struct Cyc_RemoveAggrs_Env _T127;struct Cyc_RemoveAggrs_Env _T128;struct Cyc_Absyn_Exp*_T129;struct Cyc_RemoveAggrs_Env _T12A;struct Cyc_Absyn_Exp*_T12B;struct Cyc_RemoveAggrs_Env _T12C;struct Cyc_Absyn_Exp*_T12D;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T12E;struct Cyc_Absyn_VarargCallInfo*_T12F;struct Cyc_RemoveAggrs_Env _T130;struct Cyc_List_List*_T131;void*_T132;struct Cyc_Absyn_Exp*_T133;struct Cyc_List_List*_T134;struct Cyc_Warn_String_Warn_Warg_struct _T135;struct Cyc_Warn_Exp_Warn_Warg_struct _T136;int(*_T137)(struct _fat_ptr);void*(*_T138)(struct _fat_ptr);struct _fat_ptr _T139;struct Cyc_RemoveAggrs_Env _T13A;enum Cyc_RemoveAggrs_ExpContext _T13B;int _T13C;int _T13D;struct Cyc_Absyn_Exp*_T13E;struct Cyc_RemoveAggrs_Env _T13F;struct Cyc_Absyn_Exp*_T140;struct Cyc_Absyn_Exp*_T141;struct Cyc_Absyn_Exp*_T142;struct Cyc_Absyn_Exp*_T143;struct Cyc_RemoveAggrs_Env _T144;enum Cyc_RemoveAggrs_ExpContext _T145;int _T146;int _T147;struct Cyc_Absyn_Exp*_T148;struct Cyc_RemoveAggrs_Env _T149;struct Cyc_Absyn_Exp*_T14A;struct Cyc_Absyn_Exp*_T14B;struct Cyc_Absyn_Exp*_T14C;struct Cyc_Absyn_Exp*_T14D;void*_T14E;void*_T14F;struct Cyc_Absyn_Exp*_T150;struct Cyc_Absyn_Exp*_T151;struct Cyc_Absyn_Exp*_T152;struct Cyc_RemoveAggrs_Result _T153;
enum Cyc_RemoveAggrs_ExpResult res=2U;
int did_assign=0;_T0=e;{
# 110
void*_T154=_T0->r;struct Cyc_Absyn_Stmt*_T155;struct Cyc_Absyn_Exp*_T156;int _T157;struct Cyc_Absyn_Exp*_T158;struct Cyc_Absyn_Vardecl*_T159;struct Cyc_Absyn_Aggrdecl*_T15A;struct Cyc_List_List*_T15B;struct _tuple0*_T15C;struct Cyc_Absyn_Exp*_T15D;struct _fat_ptr _T15E;_T1=(int*)_T154;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T154;_T4=_T3->f1;_T5=_T4.String_c;_T6=_T5.tag;if(_T6!=8)goto _TLA;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T154;_T7=_T15F->f1;_T8=_T7.String_c;_T15E=_T8.val;}{struct _fat_ptr s=_T15E;_T9=env;_TA=_T9.ctxt;_TB=(int)_TA;
# 113
if(_TB!=2)goto _TLC;_TC=e;_TD=_TC->topt;_TE=_check_null(_TD);_TF=Cyc_Tcutil_is_array_type(_TE);if(!_TF)goto _TLC;{
struct Cyc_List_List*dles=0;_T10=s;{
unsigned n=_get_fat_size(_T10,sizeof(char));{
unsigned i=0U;_TL11: if(i < n)goto _TLF;else{goto _TL10;}
_TLF: _T11=s;_T12=_T11.curr;_T13=(const char*)_T12;_T14=i;_T15=(int)_T14;_T16=_T13[_T15];{struct Cyc_Absyn_Exp*c=Cyc_Absyn_char_exp(_T16,0U);_T17=c;
_T17->topt=Cyc_Absyn_char_type;{struct Cyc_List_List*_T15F=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T160=_cycalloc(sizeof(struct _tuple11));
_T160->f0=0;_T160->f1=c;_T19=(struct _tuple11*)_T160;}_T15F->hd=_T19;_T15F->tl=dles;_T18=(struct Cyc_List_List*)_T15F;}dles=_T18;}
# 116
i=i + 1;goto _TL11;_TL10:;}
# 121
dles=Cyc_List_imp_rev(dles);_T1A=e;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T15F=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_T15F->tag=25;
_T15F->f1=dles;_T1B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T15F;}_T1A->r=(void*)_T1B;_T1C=
Cyc_RemoveAggrs_remove_aggrs_exp(env,e);return _T1C;}}_TLC: goto _LL0;}_TLA: goto _LL6;case 17: _LL6: goto _LL8;case 19: _LL8: goto _LLA;case 32: _LLA: goto _LLC;case 31: _LLC: goto _LLE;case 39: _LLE: goto _LL10;case 1: _LL10: goto _LL0;case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f2;}{struct Cyc_Absyn_Exp*e1=_T15D;_T1D=e;{
# 135
void*_T15F=_T1D->annot;void*_T160;struct Cyc_Absyn_Exp*_T161;_T1E=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T15F;_T1F=_T1E->tag;_T20=Cyc_Toc_NewInfo;if(_T1F!=_T20)goto _TL12;{struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*_T162=(struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_T15F;_T161=_T162->f1;_T21=_T162->f2;_T160=(void*)_T21;}{struct Cyc_Absyn_Exp*mexp=_T161;void*typ=_T160;_T22=env;_T23=_T22.ctxt;_T24=(int)_T23;
# 138
if(_T24!=0)goto _TL14;_T25=env;_T26=_T25.dest;_T27=_check_null(_T26);_T28=typ;_T29=Cyc_RemoveAggrs_can_use_initializer(_T27,_T28);if(!_T29)goto _TL14;{struct Cyc_RemoveAggrs_Env _T162;
_T162.ctxt=1U;_T2B=env;_T162.dest=_T2B.dest;_T2A=_T162;}_T2C=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T2A,_T2C);_T2D=e;_T2E=env;_T2F=_T2E.dest;_T30=mexp;_T31=
Cyc_Absyn_assign_exp(_T2F,_T30,0U);_T32=env;_T33=_T32.dest;_T34=_T33->topt;_T35=
_check_null(_T34);_T36=
# 140
Cyc_Absyn_set_type(_T31,_T35);_T37=e1;_T38=Cyc_Absyn_seq_exp(_T36,_T37,0U);_T2D->r=_T38->r;_T39=e;_T3A=e1;
# 143
_T39->topt=_T3A->topt;
res=1U;goto _LL41;_TL14: {
# 147
struct _tuple0*xvar=Cyc_Toc_temp_var();_T3B=
Cyc_Absyn_var_exp(xvar,0U);_T3C=typ;{struct Cyc_Absyn_Exp*xexp=Cyc_Absyn_set_type(_T3B,_T3C);{struct Cyc_RemoveAggrs_Env _T162;
_T162.ctxt=1U;_T162.dest=xexp;_T3D=_T162;}_T3E=e1;Cyc_RemoveAggrs_remove_aggrs_exp(_T3D,_T3E);{
struct Cyc_Absyn_Exp*body_exp=xexp;_T3F=e;_T40=_T3F->topt;
if(_T40==0)goto _TL16;_T41=e;_T42=_T41->topt;_T43=xexp;_T44=
Cyc_Absyn_cast_exp(_T42,_T43,0,1U,0U);_T45=e;_T46=_T45->topt;_T47=
_check_null(_T46);
# 152
body_exp=Cyc_Absyn_set_type(_T44,_T47);goto _TL17;_TL16: _TL17: _T48=xvar;_T49=typ;_T4A=mexp;_T4B=
# 155
Cyc_Absyn_exp_stmt(e1,0U);_T4C=Cyc_Absyn_exp_stmt(body_exp,0U);_T4D=Cyc_Absyn_seq_stmt(_T4B,_T4C,0U);{
# 154
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_declare_stmt(_T48,_T49,_T4A,_T4D,0U);_T4E=e;_T4F=
# 156
Cyc_Absyn_stmt_exp(s,0U);_T4E->r=_T4F->r;goto _LL41;}}}}}_TL12: _T51=Cyc_Warn_impos;{
# 158
int(*_T162)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T51;_T50=_T162;}_T52=_tag_fat("removeAggrs: toc did not set a new-destination",sizeof(char),47U);_T53=_tag_fat(0U,sizeof(void*),0);_T50(_T52,_T53);_LL41:;}goto _LL0;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T154;_T15C=_T15F->f1;_T15B=_T15F->f3;_T15A=_T15F->f4;}{struct _tuple0*tdn=_T15C;struct Cyc_List_List*dles=_T15B;struct Cyc_Absyn_Aggrdecl*sdopt=_T15A;
# 163
did_assign=1;_T55=env;_T56=_T55.ctxt;_T57=(int)_T56;
if(_T57==2)goto _TL18;_T58=env;_T59=_T58.ctxt;_T5A=(int)_T59;_T54=_T5A!=1;goto _TL19;_TL18: _T54=0;_TL19: {int do_stmt_exp=_T54;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;_T5C=sdopt;_T5D=(unsigned)_T5C;
if(!_T5D)goto _TL1A;_T5E=sdopt;_T5F=_T5E->kind;_T60=(int)_T5F;if(_T60!=1)goto _TL1A;_T5B=Cyc_Absyn_unionq_type;goto _TL1B;_TL1A: _T5B=Cyc_Absyn_strctq;_TL1B: {void*(*f)(struct _tuple0*)=_T5B;
void*ty=f(tdn);_T61=do_stmt_exp;
if(!_T61)goto _TL1C;{struct _tuple0**_T15F=_cycalloc(sizeof(struct _tuple0*));
*_T15F=Cyc_Toc_temp_var();_T62=(struct _tuple0**)_T15F;}v=_T62;_T63=v;_T64=*_T63;_T65=
Cyc_Absyn_var_exp(_T64,0U);_T66=ty;dest=Cyc_Absyn_set_type(_T65,_T66);goto _TL1D;
# 173
_TL1C: v=0;_T67=env;_T68=_T67.dest;
dest=_check_null(_T68);_T69=env;_T6A=_T69.ctxt;_T6B=(int)_T6A;
if(_T6B!=1)goto _TL1E;_T6C=
Cyc_Absyn_deref_exp(dest,0U);_T6D=ty;dest=Cyc_Absyn_set_type(_T6C,_T6D);goto _TL1F;_TL1E: _TL1F: _TL1D:{
# 179
struct Cyc_List_List*dles2=dles;_TL23: if(dles2!=0)goto _TL21;else{goto _TL22;}
_TL21: _T6E=dles2;_T6F=_T6E->hd;{struct _tuple11*_T15F=(struct _tuple11*)_T6F;struct Cyc_Absyn_Exp*_T160;struct Cyc_List_List*_T161;{struct _tuple11 _T162=*_T15F;_T161=_T162.f0;_T160=_T162.f1;}{struct Cyc_List_List*ds=_T161;struct Cyc_Absyn_Exp*field_exp=_T160;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);_T70=
Cyc_RemoveAggrs_deep_copy(dest);_T71=f;_T72=Cyc_RemoveAggrs_member_exp(_T70,_T71,0U);_T73=field_exp;_T74=_T73->topt;_T75=
_check_null(_T74);{
# 182
struct Cyc_Absyn_Exp*field_dest=Cyc_Absyn_set_type(_T72,_T75);{struct Cyc_RemoveAggrs_Env _T162;
# 184
_T162.ctxt=2U;_T162.dest=field_dest;_T76=_T162;}_T77=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_T76,_T77);}}}_T78=dles2;
# 179
dles2=_T78->tl;goto _TL23;_TL22:;}
# 187
if(dles!=0)goto _TL24;_T7A=Cyc_Warn_impos;{
int(*_T15F)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7A;_T79=_T15F;}_T7B=_tag_fat("zero-field aggregate",sizeof(char),21U);_T7C=_tag_fat(0U,sizeof(void*),0);_T79(_T7B,_T7C);goto _TL25;_TL24: _TL25: _T7D=dles;_T7E=_T7D->hd;_T7F=(struct _tuple11*)_T7E;_T80=*_T7F;{
# 190
struct Cyc_Absyn_Exp*init_e=_T80.f1;_T81=dles;
dles=_T81->tl;_TL29: if(dles!=0)goto _TL27;else{goto _TL28;}
_TL27: _T82=dles;_T83=_T82->hd;_T84=(struct _tuple11*)_T83;_T85=*_T84;{struct Cyc_Absyn_Exp*e2=_T85.f1;_T86=e2;_T87=_T86->topt;
if(_T87!=0)goto _TL2A;{struct Cyc_Warn_String_Warn_Warg_struct _T15F;_T15F.tag=0;
_T15F.f1=_tag_fat("expression ",sizeof(char),12U);_T88=_T15F;}{struct Cyc_Warn_String_Warn_Warg_struct _T15F=_T88;{struct Cyc_Warn_Exp_Warn_Warg_struct _T160;_T160.tag=4;_T160.f1=e2;_T89=_T160;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T160=_T89;{struct Cyc_Warn_String_Warn_Warg_struct _T161;_T161.tag=0;_T161.f1=_tag_fat(" missing type!",sizeof(char),15U);_T8A=_T161;}{struct Cyc_Warn_String_Warn_Warg_struct _T161=_T8A;void*_T162[3];_T162[0]=& _T15F;_T162[1]=& _T160;_T162[2]=& _T161;_T8C=Cyc_Warn_impos2;{int(*_T163)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T8C;_T8B=_T163;}_T8D=_tag_fat(_T162,sizeof(void*),3);_T8B(_T8D);}}}goto _TL2B;_TL2A: _TL2B: _T8E=
Cyc_Absyn_seq_exp(init_e,e2,0U);_T8F=e2;_T90=_T8F->topt;_T91=_check_null(_T90);init_e=Cyc_Absyn_set_type(_T8E,_T91);}_T92=dles;
# 191
dles=_T92->tl;goto _TL29;_TL28: _T93=do_stmt_exp;
# 197
if(!_T93)goto _TL2C;_T94=e;_T95=
_check_null(v);_T96=*_T95;_T97=ty;_T98=
Cyc_Absyn_exp_stmt(init_e,0U);_T99=
Cyc_Absyn_exp_stmt(dest,0U);_T9A=
# 199
Cyc_Absyn_seq_stmt(_T98,_T99,0U);_T9B=
# 198
Cyc_Absyn_declare_stmt(_T96,_T97,0,_T9A,0U);_T9C=Cyc_Absyn_stmt_exp(_T9B,0U);_T94->r=_T9C->r;goto _TL2D;
# 202
_TL2C: _T9D=e;_T9E=init_e;_T9D->r=_T9E->r;_T9F=e;_TA0=init_e;
_T9F->topt=_TA0->topt;_TL2D: goto _LL0;}}}}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T154;_T15B=_T15F->f1;}{struct Cyc_List_List*dles=_T15B;
# 211
res=0U;
if(dles!=0)goto _TL2E;_TA1=e;_TA2=
Cyc_Absyn_signed_int_exp(0,0U);_TA3=Cyc_Absyn_sint_type;_TA4=Cyc_Absyn_set_type(_TA2,_TA3);*_TA1=*_TA4;goto _LL0;_TL2E: _TA5=env;{
# 216
enum Cyc_RemoveAggrs_ExpContext _T15F=_TA5.ctxt;if(_T15F!=Cyc_RemoveAggrs_Other)goto _TL30;_TA7=Cyc_Warn_impos;{
int(*_T160)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TA7;_TA6=_T160;}_TA8=_tag_fat("remove-aggrs: Array_e in bad position",sizeof(char),38U);_TA9=_tag_fat(0U,sizeof(void*),0);_TA6(_TA8,_TA9);goto _TL31;_TL30: goto _LL49;_TL31: _LL49:;}
# 220
did_assign=1;_TAA=env;_TAB=_TAA.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TAB);
void*dest_type;_TAC=dest;_TAD=_TAC->topt;_TAE=
_check_null(_TAD);{void*_T15F=Cyc_Absyn_compress(_TAE);struct Cyc_Absyn_ArrayInfo _T160;_TAF=(int*)_T15F;_TB0=*_TAF;if(_TB0!=5)goto _TL32;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T161=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T15F;_T160=_T161->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T160;_TB1=ai;_TB2=_TB1.elt_type;_TB3=ai;_TB4=_TB3.tq;
dest_type=Cyc_Absyn_cstar_type(_TB2,_TB4);goto _LL4E;}_TL32: _TB5=dest;_TB6=_TB5->topt;
dest_type=_check_null(_TB6);goto _LL4E;_LL4E:;}{
# 228
int i=0;{
struct Cyc_List_List*dles2=dles;_TL37: if(dles2!=0)goto _TL35;else{goto _TL36;}
# 232
_TL35: _TB7=dles2;_TB8=_TB7->hd;{struct _tuple11*_T15F=(struct _tuple11*)_TB8;struct Cyc_Absyn_Exp*_T160;{struct _tuple11 _T161=*_T15F;_T160=_T161.f1;}{struct Cyc_Absyn_Exp*field_exp=_T160;_TB9=
Cyc_RemoveAggrs_deep_copy(dest);_TBA=
Cyc_Absyn_signed_int_exp(i,0U);_TBB=Cyc_Absyn_sint_type;_TBC=Cyc_Absyn_set_type(_TBA,_TBB);_TBD=
# 233
Cyc_Absyn_subscript_exp(_TB9,_TBC,0U);_TBE=dest_type;{struct Cyc_Absyn_Exp*fielddest=Cyc_Absyn_set_type(_TBD,_TBE);{struct Cyc_RemoveAggrs_Env _T161;
# 237
_T161.ctxt=2U;_T161.dest=fielddest;_TBF=_T161;}_TC0=field_exp;Cyc_RemoveAggrs_remove_aggrs_exp(_TBF,_TC0);}}}_TC1=dles2;
# 229
dles2=_TC1->tl;i=i + 1;goto _TL37;_TL36:;}_TC2=dles;_TC3=_TC2->hd;_TC4=(struct _tuple11*)_TC3;_TC5=*_TC4;{
# 239
struct Cyc_Absyn_Exp*init_e=_TC5.f1;_TC6=dles;
dles=_TC6->tl;_TL3B: if(dles!=0)goto _TL39;else{goto _TL3A;}
_TL39: _TC7=dles;_TC8=_TC7->hd;_TC9=(struct _tuple11*)_TC8;_TCA=*_TC9;{struct Cyc_Absyn_Exp*e2=_TCA.f1;_TCB=
Cyc_Absyn_seq_exp(init_e,e2,0U);_TCC=e2;_TCD=_TCC->topt;_TCE=_check_null(_TCD);init_e=Cyc_Absyn_set_type(_TCB,_TCE);}_TCF=dles;
# 240
dles=_TCF->tl;goto _TL3B;_TL3A: _TD0=e;_TD1=init_e;
# 244
_TD0->r=_TD1->r;_TD2=e;_TD3=init_e;
_TD2->topt=_TD3->topt;goto _LL0;}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T154;_T159=_T15F->f1;_T15D=_T15F->f2;_T158=_T15F->f3;_T157=_T15F->f4;}{struct Cyc_Absyn_Vardecl*vd=_T159;struct Cyc_Absyn_Exp*bd=_T15D;struct Cyc_Absyn_Exp*body=_T158;int zero_term=_T157;
# 251
did_assign=1;
res=0U;_TD4=env;{
enum Cyc_RemoveAggrs_ExpContext _T15F=_TD4.ctxt;if(_T15F!=Cyc_RemoveAggrs_Other)goto _TL3C;_TD6=Cyc_Warn_impos;{
int(*_T160)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD6;_TD5=_T160;}_TD7=_tag_fat("remove-aggrs: comprehension in bad position",sizeof(char),44U);_TD8=_tag_fat(0U,sizeof(void*),0);_TD5(_TD7,_TD8);goto _TL3D;_TL3C: goto _LL56;_TL3D: _LL56:;}{
# 257
struct _tuple0*max=Cyc_Toc_temp_var();_TD9=vd;{
struct _tuple0*i=_TD9->name;_TDA=
Cyc_Absyn_var_exp(i,0U);_TDB=Cyc_Absyn_sint_type;_TDC=Cyc_Absyn_set_type(_TDA,_TDB);_TDD=
Cyc_Absyn_signed_int_exp(0,0U);_TDE=Cyc_Absyn_sint_type;_TDF=Cyc_Absyn_set_type(_TDD,_TDE);{
# 259
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(_TDC,_TDF,0U);_TE0=
# 261
Cyc_Absyn_var_exp(i,0U);_TE1=Cyc_Absyn_uint_type;_TE2=Cyc_Absyn_set_type(_TE0,_TE1);_TE3=
Cyc_Absyn_var_exp(max,0U);_TE4=Cyc_Absyn_uint_type;_TE5=Cyc_Absyn_set_type(_TE3,_TE4);{
# 261
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(_TE2,_TE5,0U);_TE6=
# 263
Cyc_Absyn_var_exp(i,0U);_TE7=Cyc_Absyn_uint_type;_TE8=Cyc_Absyn_set_type(_TE6,_TE7);{struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(_TE8,0U,0U);_TE9=env;_TEA=_TE9.dest;{
struct Cyc_Absyn_Exp*dest=_check_null(_TEA);
void*dest_type;_TEB=dest;_TEC=_TEB->topt;_TED=
_check_null(_TEC);{void*_T15F=Cyc_Absyn_compress(_TED);struct Cyc_Absyn_ArrayInfo _T160;_TEE=(int*)_T15F;_TEF=*_TEE;if(_TEF!=5)goto _TL3E;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T161=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T15F;_T160=_T161->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T160;_TF0=ai;_TF1=_TF0.elt_type;_TF2=ai;_TF3=_TF2.tq;
dest_type=Cyc_Absyn_cstar_type(_TF1,_TF3);goto _LL5B;}_TL3E: _TF4=dest;_TF5=_TF4->topt;
dest_type=_check_null(_TF5);_LL5B:;}_TF6=env;_TF7=_TF6.dest;_TF8=
# 270
Cyc_Absyn_var_exp(i,0U);_TF9=Cyc_Absyn_uint_type;_TFA=Cyc_Absyn_set_type(_TF8,_TF9);{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(_TF7,_TFA,0U);
Cyc_Absyn_set_type(lval,dest_type);{struct Cyc_RemoveAggrs_Env _T15F;
_T15F.ctxt=2U;_T15F.dest=Cyc_RemoveAggrs_deep_copy(lval);_TFB=_T15F;}_TFC=body;Cyc_RemoveAggrs_remove_aggrs_exp(_TFB,_TFC);_TFD=ea;_TFE=eb;_TFF=ec;_T100=
Cyc_Absyn_exp_stmt(body,0U);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TFD,_TFE,_TFF,_T100,0U);_T101=zero_term;
if(!_T101)goto _TL40;_T102=env;_T103=_T102.dest;_T104=
# 281
Cyc_RemoveAggrs_deep_copy(_T103);_T105=
Cyc_Absyn_var_exp(max,0U);_T106=Cyc_Absyn_sint_type;_T107=Cyc_Absyn_set_type(_T105,_T106);_T108=
# 281
Cyc_Absyn_subscript_exp(_T104,_T107,0U);_T109=
# 283
Cyc_Absyn_signed_int_exp(0,0U);_T10A=Cyc_Absyn_sint_type;_T10B=Cyc_Absyn_set_type(_T109,_T10A);{
# 281
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(_T108,_T10B,0U);_T10C=s;_T10D=
# 284
Cyc_Absyn_exp_stmt(ex,0U);s=Cyc_Absyn_seq_stmt(_T10C,_T10D,0U);}goto _TL41;_TL40: _TL41:
# 287
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);_T10E=e;_T10F=max;_T110=Cyc_Absyn_uint_type;_T111=bd;_T112=
# 289
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U);_T113=
# 288
Cyc_Absyn_declare_stmt(_T10F,_T110,_T111,_T112,0U);_T114=
# 290
Cyc_Absyn_signed_int_exp(0,0U);_T115=Cyc_Absyn_sint_type;_T116=Cyc_Absyn_set_type(_T114,_T115);_T117=Cyc_Absyn_exp_stmt(_T116,0U);_T118=
# 288
Cyc_Absyn_seq_stmt(_T113,_T117,0U);_T119=Cyc_Absyn_stmt_exp(_T118,0U);_T10E->r=_T119->r;_T11A=e;
# 291
_T11A->topt=Cyc_Absyn_sint_type;goto _LL0;}}}}}}}}}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}{struct Cyc_Absyn_Exp*bd=_T15D;
# 295
did_assign=1;
res=0U;_T11B=env;{
enum Cyc_RemoveAggrs_ExpContext _T15F=_T11B.ctxt;if(_T15F!=Cyc_RemoveAggrs_Other)goto _TL42;{struct Cyc_Warn_String_Warn_Warg_struct _T160;_T160.tag=0;
_T160.f1=_tag_fat("remove-aggrs: no-init-comp in bad position",sizeof(char),43U);_T11C=_T160;}{struct Cyc_Warn_String_Warn_Warg_struct _T160=_T11C;void*_T161[1];_T161[0]=& _T160;_T11E=Cyc_Warn_impos2;{int(*_T162)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T11E;_T11D=_T162;}_T11F=_tag_fat(_T161,sizeof(void*),1);_T11D(_T11F);}goto _TL43;_TL42: goto _LL60;_TL43: _LL60:;}_T120=e;_T121=bd;
# 302
*_T120=*_T121;goto _LL0;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f3;}{struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;
# 306
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f2;}{struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;
# 310
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T122=
Cyc_RemoveAggrs_no_init(env);_T123=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T122,_T123);goto _LL0;}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f2;_T156=_T15F->f3;}{struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;struct Cyc_Absyn_Exp*e3=_T156;
# 315
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);_T124=
Cyc_RemoveAggrs_no_init(env);_T125=e2;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T124,_T125);{struct Cyc_RemoveAggrs_Env _T15F;_T127=env;
_T15F.ctxt=_T127.ctxt;_T128=env;_T129=_T128.dest;if(_T129!=0)goto _TL44;_T15F.dest=0;goto _TL45;_TL44: _T12A=env;_T12B=_T12A.dest;_T15F.dest=Cyc_RemoveAggrs_deep_copy(_T12B);_TL45: _T126=_T15F;}{struct Cyc_RemoveAggrs_Env env3=_T126;_T12C=
Cyc_RemoveAggrs_no_init(env3);_T12D=e3;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T12C,_T12D);goto _LL0;}}case 10: _T12E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T154;_T12F=_T12E->f3;if(_T12F!=0)goto _TL46;{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T15B=_T15F->f2;}{struct Cyc_Absyn_Exp*e=_T15D;struct Cyc_List_List*es=_T15B;
# 323
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);_T15B=es;goto _LL24;}_TL46: goto _LL3F;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T154;_T15B=_T15F->f2;}_LL24: {struct Cyc_List_List*es=_T15B;
# 326
_TL4B: if(es!=0)goto _TL49;else{goto _TL4A;}
_TL49: _T130=Cyc_RemoveAggrs_other_env;_T131=es;_T132=_T131->hd;_T133=(struct Cyc_Absyn_Exp*)_T132;Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_T130,_T133);_T134=es;
# 326
es=_T134->tl;goto _TL4B;_TL4A: goto _LL0;}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f2;}{struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;_T15D=e1;_T158=e2;goto _LL28;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f2;}_LL28: {struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;_T15D=e1;_T158=e2;goto _LL2A;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;_T158=_T15F->f2;}_LL2A: {struct Cyc_Absyn_Exp*e1=_T15D;struct Cyc_Absyn_Exp*e2=_T158;
# 333
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);goto _LL0;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}{struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL2E;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL2E: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL30;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f2;}_LL30: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL32;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL32: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL34;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL34: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL36;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL36: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL38;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL38: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL3A;}case 18:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL3A: {struct Cyc_Absyn_Exp*e1=_T15D;_T15D=e1;goto _LL3C;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T154;_T15D=_T15F->f1;}_LL3C: {struct Cyc_Absyn_Exp*e1=_T15D;
# 346
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T15F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T154;_T155=_T15F->f1;}{struct Cyc_Absyn_Stmt*s=_T155;
# 349
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:{struct Cyc_Warn_String_Warn_Warg_struct _T15F;_T15F.tag=0;
# 351
_T15F.f1=_tag_fat("bad exp after translation to C: ",sizeof(char),33U);_T135=_T15F;}{struct Cyc_Warn_String_Warn_Warg_struct _T15F=_T135;{struct Cyc_Warn_Exp_Warn_Warg_struct _T160;_T160.tag=4;_T160.f1=e;_T136=_T160;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T160=_T136;void*_T161[2];_T161[0]=& _T15F;_T161[1]=& _T160;_T138=Cyc_Warn_impos2;{int(*_T162)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T138;_T137=_T162;}_T139=_tag_fat(_T161,sizeof(void*),2);_T137(_T139);}}}_LL0:;}_T13A=env;_T13B=_T13A.ctxt;_T13C=(int)_T13B;
# 353
if(_T13C!=2)goto _TL4C;_T13D=did_assign;if(_T13D)goto _TL4C;else{goto _TL4E;}
_TL4E: _T13E=e;_T13F=env;_T140=_T13F.dest;_T141=_check_null(_T140);_T142=Cyc_RemoveAggrs_deep_copy(e);_T143=Cyc_Absyn_assign_exp(_T141,_T142,0U);_T13E->r=_T143->r;goto _TL4D;_TL4C: _TL4D: _T144=env;_T145=_T144.ctxt;_T146=(int)_T145;
if(_T146!=1)goto _TL4F;_T147=did_assign;if(_T147)goto _TL4F;else{goto _TL51;}
_TL51: _T148=e;_T149=env;_T14A=_T149.dest;_T14B=_check_null(_T14A);_T14C=Cyc_Absyn_deref_exp(_T14B,0U);_T14D=e;_T14E=_T14D->topt;_T14F=_check_null(_T14E);_T150=Cyc_Absyn_set_type(_T14C,_T14F);_T151=
Cyc_RemoveAggrs_deep_copy(e);_T152=
# 356
Cyc_Absyn_assign_exp(_T150,_T151,0U);_T148->r=_T152->r;goto _TL50;_TL4F: _TL50:{struct Cyc_RemoveAggrs_Result _T154;
# 359
_T154.res=res;_T153=_T154;}return _T153;}
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
