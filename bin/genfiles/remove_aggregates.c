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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 929 "absyn.h"
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 998
void*Cyc_Absyn_strctq(struct _tuple0*);
void*Cyc_Absyn_unionq_type(struct _tuple0*);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1107
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1110
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1196
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1199
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 41 "toc.h"
struct _tuple0*Cyc_Toc_temp_var (void);extern char Cyc_Toc_NewInfo[8U];struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;void*f2;};
# 42 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 48 "remove_aggregates.cyc"
static void Cyc_RemoveAggrs_massage_toplevel_aggr(struct Cyc_Absyn_Exp*e){;}
# 54
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,unsigned loc){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==20){_Tmp1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Absyn_aggrarrow_exp(e1,f,loc);}}else{
return Cyc_Absyn_aggrmember_exp(e,f,loc);};}
# 63
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer =0U,Cyc_RemoveAggrs_NewDest =1U,Cyc_RemoveAggrs_AggrField =2U,Cyc_RemoveAggrs_Other =3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 74
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_no_init(struct Cyc_RemoveAggrs_Env env){
if((int)env.ctxt==0)return Cyc_RemoveAggrs_other_env;else{return env;}}
# 79
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray =0U,Cyc_RemoveAggrs_UsedInitializer =1U,Cyc_RemoveAggrs_OtherRes =2U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 89
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*);
# 92
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
struct Cyc_RemoveAggrs_Result _Tmp0=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);enum Cyc_RemoveAggrs_ExpResult _Tmp1;_Tmp1=_Tmp0.res;{enum Cyc_RemoveAggrs_ExpResult r=_Tmp1;
if((int)r==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("remove_aggrs_exp -- unexpected array or comprehension: ",sizeof(char),56U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=e;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});}}
# 98
static int Cyc_RemoveAggrs_can_use_initializer(struct Cyc_Absyn_Exp*e,void*t){
return((unsigned)e->topt && !Cyc_Tcutil_is_array_type(_check_null(e->topt)))&&
 Cyc_Tcutil_typecmp(_check_null(e->topt),t)==0;}struct _tuple11{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 103
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
enum Cyc_RemoveAggrs_ExpResult res=2U;
int did_assign=0;
# 107
{void*_Tmp0=e->r;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct _fat_ptr _Tmp5;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag==8){_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.val;{struct _fat_ptr s=_Tmp5;
# 110
if((int)env.ctxt==2 && Cyc_Tcutil_is_array_type(_check_null(e->topt))){
struct Cyc_List_List*dles=0;
unsigned n=_get_fat_size(s,sizeof(char));
{unsigned i=0U;for(0;i < n;++ i){
struct Cyc_Absyn_Exp*c=Cyc_Absyn_char_exp(((const char*)s.curr)[(int)i],0U);
c->topt=Cyc_Absyn_char_type;
dles=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple11*_Tmp7=({struct _tuple11*_Tmp8=_cycalloc(sizeof(struct _tuple11));_Tmp8->f0=0,_Tmp8->f1=c;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=dles;_Tmp6;});}}
# 118
dles=Cyc_List_imp_rev(dles);
({void*_Tmp6=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp7->tag=25,_Tmp7->f1=dles;_Tmp7;});e->r=_Tmp6;});
return Cyc_RemoveAggrs_remove_aggrs_exp(env,e);}
# 122
goto _LL0;}}else{
goto _LL6;}case 17: _LL6:
 goto _LL8;case 19: _LL8:
 goto _LLA;case 32: _LLA:
 goto _LLC;case 31: _LLC:
 goto _LLE;case 39: _LLE:
 goto _LL10;case 1: _LL10:
 goto _LL0;case 16: _Tmp4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 132
{void*_Tmp6=e->annot;void*_Tmp7;void*_Tmp8;if(((struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_Tmp6)->tag==Cyc_Toc_NewInfo){_Tmp8=((struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_Tmp6)->f1;_Tmp7=(void*)((struct Cyc_Toc_NewInfo_Absyn_AbsynAnnot_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Exp*mexp=_Tmp8;void*typ=_Tmp7;
# 135
if((int)env.ctxt==0 && Cyc_RemoveAggrs_can_use_initializer(_check_null(env.dest),typ)){
({struct Cyc_RemoveAggrs_Env _Tmp9=({struct Cyc_RemoveAggrs_Env _TmpA;_TmpA.ctxt=1U,_TmpA.dest=env.dest;_TmpA;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp9,e1);});
({void*_Tmp9=({struct Cyc_Absyn_Exp*_TmpA=Cyc_Absyn_assign_exp(env.dest,mexp,0U);Cyc_Absyn_seq_exp(_TmpA,e1,0U);})->r;e->r=_Tmp9;});
# 139
res=1U;
goto _LL41;}{
# 142
struct _tuple0*xvar=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*xexp=Cyc_Absyn_var_exp(xvar,0U);
({struct Cyc_RemoveAggrs_Env _Tmp9=({struct Cyc_RemoveAggrs_Env _TmpA;_TmpA.ctxt=1U,_TmpA.dest=xexp;_TmpA;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp9,e1);});{
struct Cyc_Absyn_Stmt*s=({struct _tuple0*_Tmp9=xvar;void*_TmpA=typ;struct Cyc_Absyn_Exp*_TmpB=mexp;Cyc_Absyn_declare_stmt(_Tmp9,_TmpA,_TmpB,({
struct Cyc_Absyn_Stmt*_TmpC=Cyc_Absyn_exp_stmt(e1,0U);Cyc_Absyn_seq_stmt(_TmpC,Cyc_Absyn_exp_stmt(xexp,0U),0U);}),0U);});
({void*_Tmp9=Cyc_Absyn_stmt_exp(s,0U)->r;e->r=_Tmp9;});
goto _LL41;}}}}else{
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("removeAggrs: toc did not set a new-destination",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));}_LL41:;}
# 151
goto _LL0;}case 28: _Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple0*tdn=_Tmp4;struct Cyc_List_List*dles=_Tmp3;struct Cyc_Absyn_Aggrdecl*sdopt=_Tmp2;
# 154
did_assign=1;{
int do_stmt_exp=(int)env.ctxt!=2 &&(int)env.ctxt!=1;
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;
if(do_stmt_exp){
v=({struct _tuple0**_Tmp6=_cycalloc(sizeof(struct _tuple0*));({struct _tuple0*_Tmp7=Cyc_Toc_temp_var();*_Tmp6=_Tmp7;});_Tmp6;});
dest=Cyc_Absyn_var_exp(*v,0U);}else{
# 162
v=0;
dest=_check_null(env.dest);
if((int)env.ctxt==1)
dest=Cyc_Absyn_deref_exp(dest,0U);}
# 168
{struct Cyc_List_List*dles2=dles;for(0;dles2!=0;dles2=dles2->tl){
struct _tuple11*_Tmp6=(struct _tuple11*)dles2->hd;void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6->f0;_Tmp7=_Tmp6->f1;{struct Cyc_List_List*ds=_Tmp8;struct Cyc_Absyn_Exp*field_exp=_Tmp7;
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(ds);
struct Cyc_Absyn_Exp*field_dest=({struct Cyc_Absyn_Exp*_Tmp9=Cyc_Absyn_copy_exp(dest);Cyc_RemoveAggrs_member_exp(_Tmp9,f,0U);});
({struct Cyc_RemoveAggrs_Env _Tmp9=({struct Cyc_RemoveAggrs_Env _TmpA;_TmpA.ctxt=2U,_TmpA.dest=field_dest;_TmpA;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp9,field_exp);});}}}
# 175
if(dles==0)
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("zero-field aggregate",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));{
# 178
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f1;
for(dles=dles->tl;dles!=0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f1,0U);}
if(do_stmt_exp){
void*(*f)(struct _tuple0*)=(unsigned)sdopt &&(int)sdopt->kind==1?Cyc_Absyn_unionq_type: Cyc_Absyn_strctq;
({void*_Tmp6=Cyc_Absyn_stmt_exp(({struct _tuple0*_Tmp7=*v;void*_Tmp8=f(tdn);Cyc_Absyn_declare_stmt(_Tmp7,_Tmp8,0,({
struct Cyc_Absyn_Stmt*_Tmp9=Cyc_Absyn_exp_stmt(init_e,0U);Cyc_Absyn_seq_stmt(_Tmp9,
Cyc_Absyn_exp_stmt(dest,0U),0U);}),0U);}),0U)->r;
# 183
e->r=_Tmp6;});}else{
# 187
e->r=init_e->r;
e->topt=0;}
# 190
goto _LL0;}}}case 25: _Tmp4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp4;
# 195
res=0U;
if(dles==0){
({struct Cyc_Absyn_Exp _Tmp6=*Cyc_Absyn_signed_int_exp(0,0U);*e=_Tmp6;});
goto _LL0;}
# 200
{enum Cyc_RemoveAggrs_ExpContext _Tmp6=env.ctxt;if(_Tmp6==Cyc_RemoveAggrs_Other)
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("remove-aggrs: Array_e in bad position",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));else{
goto _LL49;}_LL49:;}
# 204
did_assign=1;{
struct Cyc_Absyn_Exp*dest=_check_null(env.dest);
# 207
int i=0;
{struct Cyc_List_List*dles2=dles;for(0;dles2!=0;(dles2=dles2->tl,++ i)){
# 211
struct _tuple11*_Tmp6=(struct _tuple11*)dles2->hd;void*_Tmp7;_Tmp7=_Tmp6->f1;{struct Cyc_Absyn_Exp*field_exp=_Tmp7;
struct Cyc_Absyn_Exp*fielddest=({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_copy_exp(dest);Cyc_Absyn_subscript_exp(_Tmp8,Cyc_Absyn_signed_int_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _Tmp8=({struct Cyc_RemoveAggrs_Env _Tmp9;_Tmp9.ctxt=2U,_Tmp9.dest=fielddest;_Tmp9;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp8,field_exp);});}}}{
# 215
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple11*)dles->hd)).f1;
for(dles=dles->tl;dles!=0;dles=dles->tl){
init_e=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple11*)dles->hd)).f1,0U);}
e->r=init_e->r;
e->topt=0;
goto _LL0;}}}case 26: _Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Exp*bd=_Tmp3;struct Cyc_Absyn_Exp*body=_Tmp2;int zero_term=_Tmp1;
# 224
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _Tmp6=env.ctxt;if(_Tmp6==Cyc_RemoveAggrs_Other)
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("remove-aggrs: comprehension in bad position",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));else{
goto _LL51;}_LL51:;}{
# 230
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*i=vd->name;
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_assign_exp(_Tmp6,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_Tmp6=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_lt_exp(_Tmp6,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_increment_exp(Cyc_Absyn_var_exp(i,0U),0U,0U);
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_Tmp6=_check_null(env.dest);Cyc_Absyn_subscript_exp(_Tmp6,Cyc_Absyn_var_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _Tmp6=({struct Cyc_RemoveAggrs_Env _Tmp7;_Tmp7.ctxt=2U,({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_copy_exp(lval);_Tmp7.dest=_Tmp8;});_Tmp7;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp6,body);});{
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp6=ea;struct Cyc_Absyn_Exp*_Tmp7=eb;struct Cyc_Absyn_Exp*_Tmp8=ec;Cyc_Absyn_for_stmt(_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_exp_stmt(body,0U),0U);});
if(zero_term){
# 245
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_Tmp6=({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_copy_exp(env.dest);Cyc_Absyn_subscript_exp(_Tmp7,
Cyc_Absyn_var_exp(max,0U),0U);});
# 245
Cyc_Absyn_assign_exp(_Tmp6,
# 247
Cyc_Absyn_signed_int_exp(0,0U),0U);});
s=({struct Cyc_Absyn_Stmt*_Tmp6=s;Cyc_Absyn_seq_stmt(_Tmp6,Cyc_Absyn_exp_stmt(ex,0U),0U);});}
# 251
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,bd);
({void*_Tmp6=Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_Tmp7=({struct _tuple0*_Tmp8=max;void*_Tmp9=Cyc_Absyn_uint_type;struct Cyc_Absyn_Exp*_TmpA=bd;Cyc_Absyn_declare_stmt(_Tmp8,_Tmp9,_TmpA,
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_type,0,s,0U),0U);});
# 252
Cyc_Absyn_seq_stmt(_Tmp7,
# 254
Cyc_Absyn_exp_stmt(Cyc_Absyn_signed_int_exp(0,0U),0U),0U);}),0U)->r;
# 252
e->r=_Tmp6;});
# 255
e->topt=0;
goto _LL0;}}}case 27: _Tmp4=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*bd=_Tmp4;
# 259
did_assign=1;
res=0U;
{enum Cyc_RemoveAggrs_ExpContext _Tmp6=env.ctxt;if(_Tmp6==Cyc_RemoveAggrs_Other)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("remove-aggrs: no-init-comp in bad position",sizeof(char),43U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),1));});else{
goto _LL56;}_LL56:;}
# 266
*e=*bd;
goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 270
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 274
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
({struct Cyc_RemoveAggrs_Env _Tmp6=Cyc_RemoveAggrs_no_init(env);Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_Tmp6,e2);});
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 279
did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
({struct Cyc_RemoveAggrs_Env _Tmp6=Cyc_RemoveAggrs_no_init(env);Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_Tmp6,e2);});{
struct Cyc_RemoveAggrs_Env env3=({struct Cyc_RemoveAggrs_Env _Tmp6;_Tmp6.ctxt=env.ctxt,env.dest==0?_Tmp6.dest=0:({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_copy_exp(env.dest);_Tmp6.dest=_Tmp7;});_Tmp6;});
({struct Cyc_RemoveAggrs_Env _Tmp6=Cyc_RemoveAggrs_no_init(env3);Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_Tmp6,e3);});
goto _LL0;}}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==0){_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*es=_Tmp3;
# 287
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
_Tmp4=es;goto _LL24;}}else{goto _LL3F;}case 3: _Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL24: {struct Cyc_List_List*es=_Tmp4;
# 290
for(1;es!=0;es=es->tl){
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 23: _Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 294
_Tmp4=e1;_Tmp3=e2;goto _LL28;}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL2A;}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 297
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e2);
goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 302
_Tmp4=e1;goto _LL2E;}case 12: _Tmp4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL30;}case 14: _Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL32;}case 15: _Tmp4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL34;}case 20: _Tmp4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL36;}case 21: _Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL38;}case 22: _Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL3A;}case 18: _Tmp4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LL3C;}case 5: _Tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_Tmp4;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e1);goto _LL0;}case 36: _Tmp4=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp4;
# 313
Cyc_RemoveAggrs_remove_aggrs_stmt(s);goto _LL0;}default: _LL3F:
# 315
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("bad exp after translation to C: ",sizeof(char),33U);_Tmp7;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp8;_Tmp8.tag=4,_Tmp8.f1=e;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),2));});}_LL0:;}
# 317
if((int)env.ctxt==2 && !did_assign)
({void*_Tmp0=({struct Cyc_Absyn_Exp*_Tmp1=_check_null(env.dest);Cyc_Absyn_assign_exp(_Tmp1,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_Tmp0;});
if((int)env.ctxt==1 && !did_assign)
({void*_Tmp0=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_deref_exp(_check_null(env.dest),0U);Cyc_Absyn_assign_exp(_Tmp1,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_Tmp0;});{
struct Cyc_RemoveAggrs_Result _Tmp0;_Tmp0.res=res;return _Tmp0;}}
# 328
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f1(int ignore,struct Cyc_Absyn_Exp*e){
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,e);
return 0;}
# 332
static int Cyc_RemoveAggrs_remove_aggrs_stmt_f2(int ignore,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==12){_Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 335
Cyc_RemoveAggrs_remove_aggrs_stmt(s2);{
void*_Tmp3=d->r;void*_Tmp4;switch(*((int*)_Tmp3)){case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 338
if((int)vd->sc==0){
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);
return 0;}
# 349
if(vd->initializer!=0){
struct Cyc_Absyn_Exp*var_exp=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp5->tag=4,_Tmp5->f1=vd;_Tmp5;}),0U);
var_exp->topt=vd->type;{
struct Cyc_RemoveAggrs_Result _Tmp5=({struct Cyc_RemoveAggrs_Env _Tmp6=({struct Cyc_RemoveAggrs_Env _Tmp7;_Tmp7.ctxt=0U,_Tmp7.dest=var_exp;_Tmp7;});Cyc_RemoveAggrs_remove_aggrs_exp(_Tmp6,
_check_null(vd->initializer));});
# 352
enum Cyc_RemoveAggrs_ExpResult _Tmp6;_Tmp6=_Tmp5.res;{enum Cyc_RemoveAggrs_ExpResult r=_Tmp6;
# 354
if((int)r==0 ||(int)r==1){
({void*_Tmp7=({struct Cyc_Absyn_Decl*_Tmp8=d;Cyc_Absyn_decl_stmt(_Tmp8,({struct Cyc_Absyn_Stmt*_Tmp9=Cyc_Absyn_exp_stmt(_check_null(vd->initializer),0U);Cyc_Absyn_seq_stmt(_Tmp9,s2,0U);}),0U);})->r;s->r=_Tmp7;});
# 357
vd->initializer=0;}}}}
# 360
return 0;}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);return 0;}default:
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("bad local declaration after xlation to C",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));};}}}else{
# 364
return 1;};}
# 367
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){
({void(*_Tmp0)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int,struct Cyc_Absyn_Exp*),int(*)(int,struct Cyc_Absyn_Stmt*),int,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp0;})(Cyc_RemoveAggrs_remove_aggrs_stmt_f1,Cyc_RemoveAggrs_remove_aggrs_stmt_f2,1,s);}
# 372
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){
{struct Cyc_List_List*ds2=ds;for(0;ds2!=0;ds2=ds2->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds2->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 376
if((int)fd->orig_scope!=4)
Cyc_RemoveAggrs_remove_aggrs_stmt(fd->body);
goto _LL0;}case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
Cyc_RemoveAggrs_massage_toplevel_aggr(vd->initializer);goto _LL0;}case 5:
 goto _LL8;case 7: _LL8:
 goto _LLA;case 8: _LLA:
 goto _LL0;case 4:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 2: _LL10:
 goto _LL12;case 3: _LL12:
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Cyclone decl after xlation to C",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));case 9:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("translation unit after xlation to C",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}
# 396
return ds;}
