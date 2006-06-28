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
# 128 "core.h"
int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 234
extern struct Cyc_List_List*Cyc_List_imp_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 935 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 941
void*Cyc_Absyn_compress(void*);
# 959
extern void*Cyc_Absyn_uint_type;
# 961
extern void*Cyc_Absyn_sint_type;
# 968
extern void*Cyc_Absyn_heap_rgn_type;
# 970
extern void*Cyc_Absyn_al_qual_type;
# 974
extern void*Cyc_Absyn_false_type;
# 999
void*Cyc_Absyn_exn_type (void);
# 1025
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 74
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 168 "flags.h"
extern int Cyc_Flags_better_widen;struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 34 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 131
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 77
extern int Cyc_Dict_is_empty(struct Cyc_Dict_Dict);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);
# 131
extern void*Cyc_Dict_fold(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);
# 135
extern void*Cyc_Dict_fold_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,void*);
# 184
extern struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*)(void*,void*),void*,struct Cyc_Dict_Dict);
# 257
extern struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 64 "string.h"
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 57
extern struct Cyc_Set_Set*Cyc_Set_singleton(int(*)(void*,void*),void*);
# 60
extern struct Cyc_Set_Set*Cyc_Set_from_list(int(*)(void*,void*),struct Cyc_List_List*);
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 77
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 81
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 84
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 87
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 99
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 116
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);
# 92
extern void Cyc_Hashtable_iter_c(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 142 "assndef.h"
int Cyc_AssnDef_termcmp(void*,void*);
# 144
unsigned Cyc_AssnDef_term_size(void*);
# 151
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
# 166
void*Cyc_AssnDef_select(void*,void*,void*);
# 170
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 172
void*Cyc_AssnDef_aggr(int,unsigned,struct Cyc_List_List*,void*);
# 174
void*Cyc_AssnDef_proj(void*,unsigned,void*);
void*Cyc_AssnDef_aggr_update(void*,unsigned,void*);
# 184
void*Cyc_AssnDef_plus(void*,void*,void*);struct _tuple12{void*f0;struct Cyc_List_List*f1;};
# 204
void*Cyc_AssnDef_get_term_type(void*);
# 217 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 229
int Cyc_AssnDef_assncmp(void*,void*);
# 233
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 238
void*Cyc_AssnDef_not(void*);struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};struct _tuple13{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 293
struct _tuple13 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*,struct Cyc_AssnDef_AssnMap);struct _tuple14{void*f0;void*f1;struct Cyc_Dict_Dict f2;};
# 359 "assndef.h"
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
struct Cyc_Set_Set*Cyc_PrattProver_filter_implies(struct Cyc_Set_Set*,struct Cyc_Set_Set*);struct Cyc_Hashtable_Cell{void*key;void*value;struct Cyc_Hashtable_Cell*next;};struct Cyc_Hashtable_Bucket{unsigned length;struct Cyc_Hashtable_Cell*cells;};struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 74 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_typopt2string(void*tp){struct _fat_ptr _T0;struct _fat_ptr _T1;
if(tp!=0)goto _TL0;_T0=_tag_fat("NULL",sizeof(char),5U);return _T0;_TL0: _T1=
Cyc_Absynpp_typ2string(tp);return _T1;}static char _TmpG0[2U]=",";static char _TmpG1[2U]="}";static char _TmpG2[2U]="{";
# 79
static struct _fat_ptr Cyc_AssnDef_term2string_i(void*t,struct Cyc_Set_Set**seen,int*depth){int*_T0;int _T1;struct _fat_ptr _T2;int*_T3;int*_T4;int _T5;struct Cyc_Set_Set**_T6;struct Cyc_Set_Set*_T7;void*_T8;int _T9;struct Cyc_Set_Set**_TA;struct Cyc_Set_Set**_TB;void*_TC;int*_TD;unsigned _TE;void*_TF;struct _fat_ptr _T10;struct Cyc_Int_pa_PrintArg_struct _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;void*_T14;struct _fat_ptr _T15;struct Cyc_String_pa_PrintArg_struct _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;void*_T19;void*_T1A;struct _fat_ptr _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;void*_T1F;void*_T20;void*_T21;void*_T22;struct _fat_ptr _T23;struct Cyc_String_pa_PrintArg_struct _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct _fat_ptr _T26;struct _fat_ptr _T27;void*_T28;void*_T29;void*_T2A;void*_T2B;struct _fat_ptr _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct Cyc_String_pa_PrintArg_struct _T2E;struct Cyc_String_pa_PrintArg_struct _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;void*_T32;struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T33;struct Cyc_Absyn_Vardecl*_T34;void*_T35;void*_T36;struct _fat_ptr _T37;struct Cyc_Int_pa_PrintArg_struct _T38;int _T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;void*_T3C;void*_T3D;struct _fat_ptr _T3E;struct Cyc_String_pa_PrintArg_struct _T3F;struct Cyc_Absyn_Vardecl*_T40;struct _tuple0*_T41;struct Cyc_Int_pa_PrintArg_struct _T42;int _T43;struct _fat_ptr _T44;struct _fat_ptr _T45;void*_T46;void*_T47;struct _fat_ptr _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct Cyc_String_pa_PrintArg_struct _T4A;struct _fat_ptr _T4B;struct _fat_ptr _T4C;void*_T4D;void*_T4E;void*_T4F;struct _fat_ptr _T50;struct Cyc_String_pa_PrintArg_struct _T51;struct Cyc_String_pa_PrintArg_struct _T52;struct Cyc_String_pa_PrintArg_struct _T53;struct _fat_ptr _T54;struct _fat_ptr _T55;void*_T56;void*_T57;void*_T58;struct _fat_ptr _T59;struct Cyc_String_pa_PrintArg_struct _T5A;struct Cyc_String_pa_PrintArg_struct _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;void*_T5E;struct Cyc_List_List*_T5F;struct _fat_ptr*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List*_T62;void*_T63;struct Cyc_Set_Set**_T64;int*_T65;struct Cyc_List_List*_T66;struct _fat_ptr*_T67;struct Cyc_List_List*_T68;struct Cyc_List_List*_T69;struct Cyc_List_List*_T6A;struct _fat_ptr*_T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;struct _fat_ptr*_T6E;struct _fat_ptr _T6F;struct Cyc_Int_pa_PrintArg_struct _T70;unsigned _T71;int _T72;struct Cyc_String_pa_PrintArg_struct _T73;struct _fat_ptr _T74;struct _fat_ptr _T75;int _T76;struct _fat_ptr _T77;struct Cyc_String_pa_PrintArg_struct _T78;struct _fat_ptr _T79;struct _fat_ptr _T7A;void*_T7B;void*_T7C;void*_T7D;struct _fat_ptr _T7E;struct Cyc_Int_pa_PrintArg_struct _T7F;unsigned _T80;int _T81;struct Cyc_String_pa_PrintArg_struct _T82;struct _fat_ptr _T83;struct _fat_ptr _T84;void*_T85;void*_T86;void*_T87;struct _fat_ptr _T88;struct Cyc_String_pa_PrintArg_struct _T89;struct Cyc_Int_pa_PrintArg_struct _T8A;unsigned _T8B;int _T8C;struct Cyc_String_pa_PrintArg_struct _T8D;struct _fat_ptr _T8E;struct _fat_ptr _T8F;void*_T90;void*_T91;struct _fat_ptr _T92;struct Cyc_String_pa_PrintArg_struct _T93;struct Cyc_Absyn_Vardecl*_T94;struct _tuple0*_T95;struct _fat_ptr _T96;struct _fat_ptr _T97;void*_T98;void*_T99;void*_T9A;struct _fat_ptr _T9B;struct Cyc_Int_pa_PrintArg_struct _T9C;struct Cyc_Absyn_Exp*_T9D;int _T9E;struct Cyc_Int_pa_PrintArg_struct _T9F;int _TA0;struct Cyc_String_pa_PrintArg_struct _TA1;struct Cyc_String_pa_PrintArg_struct _TA2;struct _fat_ptr _TA3;struct _fat_ptr _TA4;void*_TA5;void*_TA6;void*_TA7;struct _fat_ptr _TA8;struct Cyc_String_pa_PrintArg_struct _TA9;struct Cyc_Int_pa_PrintArg_struct _TAA;unsigned _TAB;int _TAC;struct Cyc_String_pa_PrintArg_struct _TAD;struct _fat_ptr _TAE;struct _fat_ptr _TAF;void*_TB0;void*_TB1;void*_TB2;struct _fat_ptr _TB3;struct Cyc_String_pa_PrintArg_struct _TB4;struct Cyc_String_pa_PrintArg_struct _TB5;struct _fat_ptr _TB6;struct _fat_ptr _TB7;struct Cyc_Set_Set**_TB8;int*_TB9;int*_TBA;int _TBB;int _TBC;struct _fat_ptr _TBD;struct Cyc_String_pa_PrintArg_struct _TBE;struct _fat_ptr _TBF;struct _fat_ptr _TC0;struct _fat_ptr _TC1;
struct _fat_ptr res;
int seen_it=0;_T0=depth;_T1=*_T0;
if(_T1!=0)goto _TL2;_T2=
_tag_fat("<depth>",sizeof(char),8U);return _T2;_TL2: _T3=depth;_T4=depth;_T5=*_T4;
*_T3=_T5 - 1;_T6=seen;_T7=*_T6;_T8=t;_T9=
Cyc_Set_member(_T7,_T8);if(!_T9)goto _TL4;
seen_it=1;goto _TL5;_TL4: _TL5: _TA=seen;{
# 88
struct Cyc_Set_Set*old_seen=*_TA;_TB=seen;
*_TB=Cyc_Set_insert(old_seen,t);{enum Cyc_Absyn_Primop _TC2;int _TC3;void*_TC4;void*_TC5;void*_TC6;unsigned _TC7;_TC=t;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 0: _TF=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TF;_TC7=_TC8->f1;}{unsigned i=_TC7;{struct Cyc_Int_pa_PrintArg_struct _TC8;_TC8.tag=1;
# 91
_TC8.f1=i;_T11=_TC8;}{struct Cyc_Int_pa_PrintArg_struct _TC8=_T11;void*_TC9[1];_TC9[0]=& _TC8;_T12=_tag_fat("0x%x",sizeof(char),5U);_T13=_tag_fat(_TC9,sizeof(void*),1);_T10=Cyc_aprintf(_T12,_T13);}res=_T10;goto _LL0;}case 1: _T14=t;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T14;_TC6=_TC8->f1;}{struct Cyc_Absyn_Exp*e=_TC6;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
_TC8.f1=Cyc_Absynpp_exp2string(e);_T16=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T16;void*_TC9[1];_TC9[0]=& _TC8;_T17=_tag_fat("%s",sizeof(char),3U);_T18=_tag_fat(_TC9,sizeof(void*),1);_T15=Cyc_aprintf(_T17,_T18);}res=_T15;goto _LL0;}case 15: _T19=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T19;_T1A=_TC8->f1;_TC6=(void*)_T1A;}{void*t=_TC6;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
_TC8.f1=Cyc_AssnDef_term2string_i(t,seen,depth);_T1C=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T1C;void*_TC9[1];_TC9[0]=& _TC8;_T1D=_tag_fat("tagof_tm(%s)",sizeof(char),13U);_T1E=_tag_fat(_TC9,sizeof(void*),1);_T1B=Cyc_aprintf(_T1D,_T1E);}res=_T1B;goto _LL0;}case 6: _T1F=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T1F;_T20=_TC8->f1;_TC6=(void*)_T20;_T21=_TC8->f2;_TC5=(void*)_T21;_T22=_TC8->f3;_TC4=(void*)_T22;}{void*t1=_TC6;void*t2=_TC5;void*tp=_TC4;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 96
_TC8.f1=Cyc_AssnDef_term2string_i(t1,seen,depth);_T24=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T24;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(t2,seen,depth);_T25=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T25;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T26=_tag_fat("Sel(%s,%s)",sizeof(char),11U);_T27=_tag_fat(_TCA,sizeof(void*),2);_T23=Cyc_aprintf(_T26,_T27);}}res=_T23;goto _LL0;}case 7: _T28=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T28;_T29=_TC8->f1;_TC6=(void*)_T29;_T2A=_TC8->f2;_TC5=(void*)_T2A;_T2B=_TC8->f3;_TC4=(void*)_T2B;}{void*t1=_TC6;void*t2=_TC5;void*t3=_TC4;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 99
_TC8.f1=Cyc_AssnDef_term2string_i(t1,seen,depth);_T2D=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T2D;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(t2,seen,depth);_T2E=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T2E;{struct Cyc_String_pa_PrintArg_struct _TCA;_TCA.tag=0;_TCA.f1=Cyc_AssnDef_term2string_i(t3,seen,depth);_T2F=_TCA;}{struct Cyc_String_pa_PrintArg_struct _TCA=_T2F;void*_TCB[3];_TCB[0]=& _TC8;_TCB[1]=& _TC9;_TCB[2]=& _TCA;_T30=_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U);_T31=_tag_fat(_TCB,sizeof(void*),3);_T2C=Cyc_aprintf(_T30,_T31);}}}res=_T2C;goto _LL0;}case 2: _T32=t;_T33=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T32;_T34=_T33->f1;if(_T34!=0)goto _TL7;_T35=t;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T35;_TC3=_TC8->f2;_T36=_TC8->f3;_TC6=(void*)_T36;}{int i=_TC3;void*topt=_TC6;{struct Cyc_Int_pa_PrintArg_struct _TC8;_TC8.tag=1;_T39=i;
# 105
_TC8.f1=(unsigned long)_T39;_T38=_TC8;}{struct Cyc_Int_pa_PrintArg_struct _TC8=_T38;void*_TC9[1];_TC9[0]=& _TC8;_T3A=_tag_fat("_X%d",sizeof(char),5U);_T3B=_tag_fat(_TC9,sizeof(void*),1);_T37=Cyc_aprintf(_T3A,_T3B);}res=_T37;goto _LL0;}_TL7: _T3C=t;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T3C;_TC6=_TC8->f1;_TC3=_TC8->f2;_T3D=_TC8->f3;_TC5=(void*)_T3D;}{struct Cyc_Absyn_Vardecl*vd=_TC6;int i=_TC3;void*topt=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;_T40=vd;_T41=_T40->name;
# 111
_TC8.f1=Cyc_Absynpp_qvar2string(_T41);_T3F=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T3F;{struct Cyc_Int_pa_PrintArg_struct _TC9;_TC9.tag=1;_T43=i;_TC9.f1=(unsigned long)_T43;_T42=_TC9;}{struct Cyc_Int_pa_PrintArg_struct _TC9=_T42;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T44=_tag_fat("_%s%d",sizeof(char),6U);_T45=_tag_fat(_TCA,sizeof(void*),2);_T3E=Cyc_aprintf(_T44,_T45);}}res=_T3E;goto _LL0;}case 3: _T46=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T46;_TC2=_TC8->f1;_T47=_TC8->f2;_TC6=(void*)_T47;}{enum Cyc_Absyn_Primop p=_TC2;void*t1=_TC6;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 113
_TC8.f1=Cyc_Absynpp_prim2string(p);_T49=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T49;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(t1,seen,depth);_T4A=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T4A;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T4B=_tag_fat("%s(%s)",sizeof(char),7U);_T4C=_tag_fat(_TCA,sizeof(void*),2);_T48=Cyc_aprintf(_T4B,_T4C);}}res=_T48;goto _LL0;}case 4: _T4D=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T4D;_TC2=_TC8->f1;_T4E=_TC8->f2;_TC6=(void*)_T4E;_T4F=_TC8->f3;_TC5=(void*)_T4F;}{enum Cyc_Absyn_Primop p=_TC2;void*t1=_TC6;void*t2=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 115
_TC8.f1=Cyc_AssnDef_term2string_i(t1,seen,depth);_T51=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T51;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_Absynpp_prim2string(p);_T52=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T52;{struct Cyc_String_pa_PrintArg_struct _TCA;_TCA.tag=0;
_TCA.f1=Cyc_AssnDef_term2string_i(t2,seen,depth);_T53=_TCA;}{struct Cyc_String_pa_PrintArg_struct _TCA=_T53;void*_TCB[3];_TCB[0]=& _TC8;_TCB[1]=& _TC9;_TCB[2]=& _TCA;_T54=
# 115
_tag_fat("(%s%s%s)",sizeof(char),9U);_T55=_tag_fat(_TCB,sizeof(void*),3);_T50=Cyc_aprintf(_T54,_T55);}}}res=_T50;goto _LL0;}case 5: _T56=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T56;_T57=_TC8->f1;_TC6=(void*)_T57;_T58=_TC8->f2;_TC5=(void*)_T58;}{void*tp=_TC6;void*tm=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 119
_TC8.f1=Cyc_Absynpp_typ2string(tp);_T5A=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T5A;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(tm,seen,depth);_T5B=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T5B;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T5C=_tag_fat("(%s)%s",sizeof(char),7U);_T5D=_tag_fat(_TCA,sizeof(void*),2);_T59=Cyc_aprintf(_T5C,_T5D);}}res=_T59;goto _LL0;}case 8: _T5E=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T5E;_TC3=_TC8->f1;_TC7=_TC8->f2;_TC6=_TC8->f3;}{int is_union=_TC3;unsigned tag=_TC7;struct Cyc_List_List*tms=_TC6;
# 121
static struct _fat_ptr lb={(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 2U};
static struct _fat_ptr rb={(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 2U};
static struct _fat_ptr comma={(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 2U};
struct Cyc_List_List*ss;ss=_cycalloc(sizeof(struct Cyc_List_List));_T5F=ss;_T60=& lb;_T5F->hd=(struct _fat_ptr*)_T60;_T61=ss;_T61->tl=0;
_TLC: if(tms!=0)goto _TLA;else{goto _TLB;}
_TLA: _T62=tms;_T63=_T62->hd;_T64=seen;_T65=depth;{struct _fat_ptr s=Cyc_AssnDef_term2string_i(_T63,_T64,_T65);{struct Cyc_List_List*_TC8=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TC9=_cycalloc(sizeof(struct _fat_ptr));
*_TC9=s;_T67=(struct _fat_ptr*)_TC9;}_TC8->hd=_T67;_TC8->tl=ss;_T66=(struct Cyc_List_List*)_TC8;}ss=_T66;_T68=tms;_T69=_T68->tl;
if(_T69==0)goto _TLD;{struct Cyc_List_List*_TC8=_cycalloc(sizeof(struct Cyc_List_List));_T6B=& comma;_TC8->hd=(struct _fat_ptr*)_T6B;_TC8->tl=ss;_T6A=(struct Cyc_List_List*)_TC8;}ss=_T6A;goto _TLE;_TLD: _TLE:;}_T6C=tms;
# 125
tms=_T6C->tl;goto _TLC;_TLB:{struct Cyc_List_List*_TC8=_cycalloc(sizeof(struct Cyc_List_List));_T6E=& rb;
# 130
_TC8->hd=(struct _fat_ptr*)_T6E;_TC8->tl=ss;_T6D=(struct Cyc_List_List*)_TC8;}ss=_T6D;
ss=Cyc_List_imp_rev(ss);
res=Cyc_strconcat_l(ss);
if(tag==4294967295U)goto _TLF;{struct Cyc_Int_pa_PrintArg_struct _TC8;_TC8.tag=1;_T71=tag;_T72=(int)_T71;_TC8.f1=(unsigned long)_T72;_T70=_TC8;}{struct Cyc_Int_pa_PrintArg_struct _TC8=_T70;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=res;_T73=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T73;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T74=_tag_fat("tag(%d)%s",sizeof(char),10U);_T75=_tag_fat(_TCA,sizeof(void*),2);_T6F=Cyc_aprintf(_T74,_T75);}}res=_T6F;goto _TL10;_TLF: _TL10: _T76=is_union;
if(!_T76)goto _TL11;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;_TC8.f1=res;_T78=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T78;void*_TC9[1];_TC9[0]=& _TC8;_T79=_tag_fat("union%s",sizeof(char),8U);_T7A=_tag_fat(_TC9,sizeof(void*),1);_T77=Cyc_aprintf(_T79,_T7A);}res=_T77;goto _TL12;_TL11: _TL12: goto _LL0;}case 9: _T7B=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T7B;_T7C=_TC8->f1;_TC6=(void*)_T7C;_TC7=_TC8->f2;_T7D=_TC8->f3;_TC5=(void*)_T7D;}{void*t=_TC6;unsigned i=_TC7;void*tp=_TC5;{struct Cyc_Int_pa_PrintArg_struct _TC8;_TC8.tag=1;_T80=i;_T81=(int)_T80;
# 137
_TC8.f1=(unsigned long)_T81;_T7F=_TC8;}{struct Cyc_Int_pa_PrintArg_struct _TC8=_T7F;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(t,seen,depth);_T82=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_T82;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_T83=_tag_fat("#%d(%s)",sizeof(char),8U);_T84=_tag_fat(_TCA,sizeof(void*),2);_T7E=Cyc_aprintf(_T83,_T84);}}res=_T7E;goto _LL0;}case 10: _T85=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T85;_T86=_TC8->f1;_TC6=(void*)_T86;_TC7=_TC8->f2;_T87=_TC8->f3;_TC5=(void*)_T87;}{void*a=_TC6;unsigned i=_TC7;void*v=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 140
_TC8.f1=Cyc_AssnDef_term2string_i(a,seen,depth);_T89=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T89;{struct Cyc_Int_pa_PrintArg_struct _TC9;_TC9.tag=1;_T8B=i;_T8C=(int)_T8B;_TC9.f1=(unsigned long)_T8C;_T8A=_TC9;}{struct Cyc_Int_pa_PrintArg_struct _TC9=_T8A;{struct Cyc_String_pa_PrintArg_struct _TCA;_TCA.tag=0;_TCA.f1=Cyc_AssnDef_term2string_i(v,seen,depth);_T8D=_TCA;}{struct Cyc_String_pa_PrintArg_struct _TCA=_T8D;void*_TCB[3];_TCB[0]=& _TC8;_TCB[1]=& _TC9;_TCB[2]=& _TCA;_T8E=_tag_fat("Aupd(%s,%d,%s)",sizeof(char),15U);_T8F=_tag_fat(_TCB,sizeof(void*),3);_T88=Cyc_aprintf(_T8E,_T8F);}}}res=_T88;goto _LL0;}case 11: _T90=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T90;_TC6=_TC8->f1;_T91=_TC8->f2;_TC5=(void*)_T91;}{struct Cyc_Absyn_Vardecl*vd=_TC6;void*invoke=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;_T94=vd;_T95=_T94->name;
# 145
_TC8.f1=Cyc_Absynpp_qvar2string(_T95);_T93=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_T93;void*_TC9[1];_TC9[0]=& _TC8;_T96=_tag_fat("&%s",sizeof(char),4U);_T97=_tag_fat(_TC9,sizeof(void*),1);_T92=Cyc_aprintf(_T96,_T97);}res=_T92;goto _LL0;}case 12: _T98=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T98;_TC6=_TC8->f1;_TC3=_TC8->f2;_T99=_TC8->f3;_TC5=(void*)_T99;_T9A=_TC8->f4;_TC4=(void*)_T9A;}{struct Cyc_Absyn_Exp*e=_TC6;int id=_TC3;void*nelts=_TC5;void*invoke=_TC4;{struct Cyc_Int_pa_PrintArg_struct _TC8;_TC8.tag=1;_T9D=e;_T9E=(int)_T9D;
# 148
_TC8.f1=(unsigned long)_T9E;_T9C=_TC8;}{struct Cyc_Int_pa_PrintArg_struct _TC8=_T9C;{struct Cyc_Int_pa_PrintArg_struct _TC9;_TC9.tag=1;_TA0=id;_TC9.f1=(unsigned long)_TA0;_T9F=_TC9;}{struct Cyc_Int_pa_PrintArg_struct _TC9=_T9F;{struct Cyc_String_pa_PrintArg_struct _TCA;_TCA.tag=0;_TCA.f1=Cyc_AssnDef_term2string_i(nelts,seen,depth);_TA1=_TCA;}{struct Cyc_String_pa_PrintArg_struct _TCA=_TA1;{struct Cyc_String_pa_PrintArg_struct _TCB;_TCB.tag=0;_TCB.f1=Cyc_AssnDef_term2string_i(invoke,seen,depth);_TA2=_TCB;}{struct Cyc_String_pa_PrintArg_struct _TCB=_TA2;void*_TCC[4];_TCC[0]=& _TC8;_TCC[1]=& _TC9;_TCC[2]=& _TCA;_TCC[3]=& _TCB;_TA3=_tag_fat("Alloc(%d,%d,%s,%s)",sizeof(char),19U);_TA4=_tag_fat(_TCC,sizeof(void*),4);_T9B=Cyc_aprintf(_TA3,_TA4);}}}}res=_T9B;goto _LL0;}case 13: _TA5=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_TA5;_TA6=_TC8->f1;_TC6=(void*)_TA6;_TC7=_TC8->f2;_TA7=_TC8->f3;_TC5=(void*)_TA7;}{void*t=_TC6;unsigned i=_TC7;void*tp=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 151
_TC8.f1=Cyc_AssnDef_term2string_i(t,seen,depth);_TA9=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_TA9;{struct Cyc_Int_pa_PrintArg_struct _TC9;_TC9.tag=1;_TAB=i;_TAC=(int)_TAB;_TC9.f1=(unsigned long)_TAC;_TAA=_TC9;}{struct Cyc_Int_pa_PrintArg_struct _TC9=_TAA;{struct Cyc_String_pa_PrintArg_struct _TCA;_TCA.tag=0;_TCA.f1=Cyc_AssnDef_typopt2string(tp);_TAD=_TCA;}{struct Cyc_String_pa_PrintArg_struct _TCA=_TAD;void*_TCB[3];_TCB[0]=& _TC8;_TCB[1]=& _TC9;_TCB[2]=& _TCA;_TAE=_tag_fat("Offsetf(%s,%d,%s)",sizeof(char),18U);_TAF=_tag_fat(_TCB,sizeof(void*),3);_TA8=Cyc_aprintf(_TAE,_TAF);}}}res=_TA8;goto _LL0;}default: _TB0=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TC8=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_TB0;_TB1=_TC8->f1;_TC6=(void*)_TB1;_TB2=_TC8->f2;_TC5=(void*)_TB2;}{void*t1=_TC6;void*t2=_TC5;{struct Cyc_String_pa_PrintArg_struct _TC8;_TC8.tag=0;
# 154
_TC8.f1=Cyc_AssnDef_term2string_i(t1,seen,depth);_TB4=_TC8;}{struct Cyc_String_pa_PrintArg_struct _TC8=_TB4;{struct Cyc_String_pa_PrintArg_struct _TC9;_TC9.tag=0;_TC9.f1=Cyc_AssnDef_term2string_i(t2,seen,depth);_TB5=_TC9;}{struct Cyc_String_pa_PrintArg_struct _TC9=_TB5;void*_TCA[2];_TCA[0]=& _TC8;_TCA[1]=& _TC9;_TB6=_tag_fat("%s.+.%s",sizeof(char),8U);_TB7=_tag_fat(_TCA,sizeof(void*),2);_TB3=Cyc_aprintf(_TB6,_TB7);}}res=_TB3;goto _LL0;}}_LL0:;}_TB8=seen;
# 157
*_TB8=old_seen;_TB9=depth;_TBA=depth;_TBB=*_TBA;
*_TB9=_TBB + 1;_TBC=seen_it;
if(!_TBC)goto _TL13;{struct Cyc_String_pa_PrintArg_struct _TC2;_TC2.tag=0;_TC2.f1=res;_TBE=_TC2;}{struct Cyc_String_pa_PrintArg_struct _TC2=_TBE;void*_TC3[1];_TC3[0]=& _TC2;_TBF=_tag_fat("<loop: %s>\n",sizeof(char),12U);_TC0=_tag_fat(_TC3,sizeof(void*),1);_TBD=Cyc_aprintf(_TBF,_TC0);}res=_TBD;goto _TL14;_TL13: _TL14: _TC1=res;
return _TC1;}}
# 163
struct _fat_ptr Cyc_AssnDef_term2string(void*t){int(*_T0)(void*,void*);void*_T1;struct Cyc_Set_Set**_T2;int*_T3;struct _fat_ptr _T4;_T0=Cyc_AssnDef_termcmp;{
struct Cyc_Set_Set*s=Cyc_Set_empty(_T0);
int d=100;_T1=t;_T2=& s;_T3=& d;_T4=
Cyc_AssnDef_term2string_i(_T1,_T2,_T3);return _T4;}}
# 169
static int Cyc_AssnDef_hash_const(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_String_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Char_c _T4;struct _tuple3 _T5;char _T6;int _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Short_c _T9;struct _tuple4 _TA;short _TB;int _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_Int_c _TE;struct _tuple5 _TF;int _T10;union Cyc_Absyn_Cnst _T11;struct _union_Cnst_LongLong_c _T12;struct _tuple6 _T13;long long _T14;int _T15;union Cyc_Absyn_Cnst _T16;struct _union_Cnst_Float_c _T17;struct _tuple7 _T18;union Cyc_Absyn_Cnst _T19;struct _union_Cnst_Wchar_c _T1A;union Cyc_Absyn_Cnst _T1B;struct _union_Cnst_String_c _T1C;union Cyc_Absyn_Cnst _T1D;struct _union_Cnst_Wstring_c _T1E;int _T1F;struct _fat_ptr _T20;long long _T21;int _T22;short _T23;char _T24;_T0=c;_T1=_T0.String_c;_T2=_T1.tag;switch(_T2){case 1:
# 171
 return 0;case 2: _T3=c;_T4=_T3.Char_c;_T5=_T4.val;_T24=_T5.f1;{char c=_T24;_T6=c;_T7=(int)_T6;
return _T7;}case 4: _T8=c;_T9=_T8.Short_c;_TA=_T9.val;_T23=_TA.f1;{short c=_T23;_TB=c;_TC=(int)_TB;
return _TC;}case 5: _TD=c;_TE=_TD.Int_c;_TF=_TE.val;_T22=_TF.f1;{int i=_T22;_T10=i;
return _T10;}case 6: _T11=c;_T12=_T11.LongLong_c;_T13=_T12.val;_T21=_T13.f1;{long long i=_T21;_T14=i;_T15=(int)_T14;
return _T15;}case 7: _T16=c;_T17=_T16.Float_c;_T18=_T17.val;_T20=_T18.f0;{struct _fat_ptr s=_T20;_T20=s;goto _LLE;}case 3: _T19=c;_T1A=_T19.Wchar_c;_T20=_T1A.val;_LLE: {struct _fat_ptr s=_T20;_T20=s;goto _LL10;}case 8: _T1B=c;_T1C=_T1B.String_c;_T20=_T1C.val;_LL10: {struct _fat_ptr s=_T20;_T20=s;goto _LL12;}default: _T1D=c;_T1E=_T1D.Wstring_c;_T20=_T1E.val;_LL12: {struct _fat_ptr s=_T20;_T1F=
# 180
Cyc_Hashtable_hash_string(s);return _T1F;}};}
# 184
static int Cyc_AssnDef_hash_binding(void*b){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;unsigned _T5;unsigned _T6;unsigned _T7;int _T8;void*_T9;struct Cyc_Absyn_Vardecl*_TA;unsigned _TB;unsigned _TC;unsigned _TD;int _TE;void*_TF;struct Cyc_Absyn_Vardecl*_T10;unsigned _T11;unsigned _T12;unsigned _T13;int _T14;void*_T15;struct Cyc_Absyn_Fndecl*_T16;unsigned _T17;unsigned _T18;unsigned _T19;int _T1A;void*_T1B;struct Cyc_Absyn_Vardecl*_T1C;unsigned _T1D;unsigned _T1E;unsigned _T1F;int _T20;void*_T21;struct _tuple0*_T22;struct Cyc_Absyn_Fndecl*_T23;struct Cyc_Absyn_Vardecl*_T24;_T0=b;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 4: _T3=b;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T3;_T24=_T25->f1;}{struct Cyc_Absyn_Vardecl*vd=_T24;_T4=vd;_T5=(unsigned)_T4;_T6=_T5 >> 4;_T7=_T6 + 21U;_T8=(int)_T7;
# 186
return _T8;}case 5: _T9=b;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T9;_T24=_T25->f1;}{struct Cyc_Absyn_Vardecl*vd=_T24;_TA=vd;_TB=(unsigned)_TA;_TC=_TB >> 4;_TD=_TC + 23U;_TE=(int)_TD;
return _TE;}case 3: _TF=b;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_TF;_T24=_T25->f1;}{struct Cyc_Absyn_Vardecl*vd=_T24;_T10=vd;_T11=(unsigned)_T10;_T12=_T11 >> 4;_T13=_T12 + 27U;_T14=(int)_T13;
return _T14;}case 2: _T15=b;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T15;_T23=_T25->f1;}{struct Cyc_Absyn_Fndecl*vd=_T23;_T16=vd;_T17=(unsigned)_T16;_T18=_T17 >> 4;_T19=_T18 + 29U;_T1A=(int)_T19;
return _T1A;}case 1: _T1B=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T1B;_T24=_T25->f1;}{struct Cyc_Absyn_Vardecl*vd=_T24;_T1C=vd;_T1D=(unsigned)_T1C;_T1E=_T1D >> 4;_T1F=_T1E + 31U;_T20=(int)_T1F;
return _T20;}default: _T21=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T21;_T22=_T25->f1;}{struct _tuple0*q=_T22;
return 1;}};}
# 195
static int Cyc_AssnDef_hash_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int _T3;void*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;void*_T7;const unsigned*_T8;unsigned _T9;unsigned _TA;int _TB;_T0=e;{
void*_TC=_T0->r;void*_TD;union Cyc_Absyn_Cnst _TE;_T1=(int*)_TC;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_TF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TC;_TE=_TF->f1;}{union Cyc_Absyn_Cnst c=_TE;_T3=
Cyc_AssnDef_hash_const(c);return _T3;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TC;_T4=_TF->f1;_TD=(void*)_T4;}{void*b=_TD;_T5=
Cyc_AssnDef_hash_binding(b);return _T5;}default: _T6=e;_T7=_T6->r;_T8=(const unsigned*)_T7;_T9=*_T8;_TA=_T9 + 1U;_TB=(int)_TA;
return _TB;};}}
# 205
unsigned Cyc_AssnDef_combine(unsigned k1,unsigned k2){unsigned _T0;unsigned _T1;unsigned _T2;unsigned _T3;unsigned _T4;unsigned _T5;unsigned _T6;unsigned _T7;unsigned _T8;unsigned _T9;unsigned _TA;unsigned _TB;unsigned _TC;unsigned _TD;unsigned _TE;unsigned _TF;unsigned _T10;unsigned _T11;unsigned _T12;unsigned _T13;unsigned _T14;unsigned _T15;unsigned _T16;unsigned _T17;unsigned _T18;unsigned _T19;unsigned _T1A;unsigned _T1B;unsigned _T1C;
# 207
register unsigned a;register unsigned b;register unsigned c;register unsigned len;
# 209
b=2654435769U;a=b;
c=k2 + 4U;_T0=k1;
a=a + _T0;_T1=b;
a=a - _T1;_T2=c;a=a - _T2;_T3=c >> 13;a=a ^ _T3;_T4=c;
b=b - _T4;_T5=a;b=b - _T5;_T6=a << 8;b=b ^ _T6;_T7=a;
c=c - _T7;_T8=b;c=c - _T8;_T9=b >> 13;c=c ^ _T9;_TA=b;
a=a - _TA;_TB=c;a=a - _TB;_TC=c >> 12;a=a ^ _TC;_TD=c;
b=b - _TD;_TE=a;b=b - _TE;_TF=a << 16;b=b ^ _TF;_T10=a;
c=c - _T10;_T11=b;c=c - _T11;_T12=b >> 5;c=c ^ _T12;_T13=b;
a=a - _T13;_T14=c;a=a - _T14;_T15=c >> 3;a=a ^ _T15;_T16=c;
b=b - _T16;_T17=a;b=b - _T17;_T18=a << 10;b=b ^ _T18;_T19=a;
c=c - _T19;_T1A=b;c=c - _T1A;_T1B=b >> 15;c=c ^ _T1B;_T1C=c;
return _T1C;}
# 224
int Cyc_AssnDef_termhash(void*t){void*_T0;const unsigned*_T1;unsigned _T2;void*_T3;int*_T4;unsigned _T5;void*_T6;unsigned _T7;int _T8;void*_T9;int _TA;unsigned _TB;unsigned _TC;unsigned _TD;int _TE;void*_TF;struct Cyc_Absyn_Vardecl*_T10;unsigned _T11;unsigned _T12;int _T13;unsigned _T14;unsigned _T15;unsigned _T16;int _T17;void*_T18;void*_T19;void*_T1A;unsigned _T1B;enum Cyc_Absyn_Primop _T1C;unsigned _T1D;unsigned _T1E;unsigned _T1F;unsigned _T20;int _T21;void*_T22;void*_T23;void*_T24;void*_T25;unsigned _T26;void*_T27;unsigned _T28;enum Cyc_Absyn_Primop _T29;unsigned _T2A;unsigned _T2B;unsigned _T2C;unsigned _T2D;unsigned _T2E;int _T2F;void*_T30;void*_T31;void*_T32;unsigned _T33;unsigned _T34;unsigned _T35;int _T36;void*_T37;void*_T38;void*_T39;void*_T3A;unsigned _T3B;void*_T3C;unsigned _T3D;unsigned _T3E;unsigned _T3F;unsigned _T40;int _T41;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;unsigned _T47;void*_T48;unsigned _T49;void*_T4A;unsigned _T4B;unsigned _T4C;unsigned _T4D;unsigned _T4E;unsigned _T4F;int _T50;void*_T51;void*_T52;void*_T53;unsigned _T54;unsigned _T55;unsigned _T56;int _T57;void*_T58;unsigned _T59;int _T5A;unsigned _T5B;struct Cyc_List_List*_T5C;void*_T5D;unsigned _T5E;unsigned _T5F;struct Cyc_List_List*_T60;unsigned _T61;int _T62;void*_T63;void*_T64;void*_T65;void*_T66;unsigned _T67;void*_T68;unsigned _T69;unsigned _T6A;unsigned _T6B;unsigned _T6C;int _T6D;void*_T6E;void*_T6F;void*_T70;unsigned _T71;struct Cyc_Absyn_Vardecl*_T72;unsigned _T73;unsigned _T74;unsigned _T75;unsigned _T76;int _T77;void*_T78;void*_T79;void*_T7A;struct Cyc_Absyn_Exp*_T7B;unsigned _T7C;void*_T7D;unsigned _T7E;void*_T7F;unsigned _T80;unsigned _T81;int _T82;unsigned _T83;unsigned _T84;unsigned _T85;unsigned _T86;unsigned _T87;int _T88;void*_T89;void*_T8A;void*_T8B;unsigned _T8C;unsigned _T8D;unsigned _T8E;int _T8F;void*_T90;void*_T91;void*_T92;void*_T93;unsigned _T94;void*_T95;unsigned _T96;unsigned _T97;unsigned _T98;unsigned _T99;int _T9A;void*_T9B;void*_T9C;void*_T9D;unsigned _T9E;unsigned _T9F;unsigned _TA0;int _TA1;_T0=t;_T1=(const unsigned*)_T0;_T2=*_T1;{
unsigned k=_T2 << 16;void*_TA2;void*_TA3;enum Cyc_Absyn_Primop _TA4;int _TA5;void*_TA6;unsigned _TA7;_T3=t;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 0: _T6=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T6;_TA7=_TA8->f1;}{unsigned i=_TA7;_T7=
# 227
Cyc_AssnDef_combine(k,i);_T8=(int)_T7;return _T8;}case 1: _T9=t;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T9;_TA6=_TA8->f1;}{struct Cyc_Absyn_Exp*e=_TA6;_TA=
Cyc_AssnDef_hash_exp(e);_TB=(unsigned)_TA;_TC=k;_TD=Cyc_AssnDef_combine(_TB,_TC);_TE=(int)_TD;return _TE;}case 2: _TF=t;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_TF;_TA6=_TA8->f1;_TA5=_TA8->f2;}{struct Cyc_Absyn_Vardecl*vd=_TA6;int i=_TA5;_T10=vd;_T11=(unsigned)_T10;_T12=k;_T13=i;_T14=(unsigned)_T13;_T15=
# 230
Cyc_AssnDef_combine(_T12,_T14);_T16=Cyc_AssnDef_combine(_T11,_T15);_T17=(int)_T16;return _T17;}case 3: _T18=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T18;_TA4=_TA8->f1;_T19=_TA8->f2;_TA6=(void*)_T19;}{enum Cyc_Absyn_Primop p=_TA4;void*t1=_TA6;_T1A=t1;_T1B=(unsigned)_T1A;_T1C=p;_T1D=(unsigned)_T1C;_T1E=k;_T1F=_T1D + _T1E;_T20=
# 232
Cyc_AssnDef_combine(_T1B,_T1F);_T21=(int)_T20;return _T21;}case 4: _T22=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T22;_TA4=_TA8->f1;_T23=_TA8->f2;_TA6=(void*)_T23;_T24=_TA8->f3;_TA3=(void*)_T24;}{enum Cyc_Absyn_Primop p=_TA4;void*t1=_TA6;void*t2=_TA3;_T25=t1;_T26=(unsigned)_T25;_T27=t2;_T28=(unsigned)_T27;_T29=p;_T2A=(unsigned)_T29;_T2B=k;_T2C=_T2A + _T2B;_T2D=
# 234
Cyc_AssnDef_combine(_T28,_T2C);_T2E=Cyc_AssnDef_combine(_T26,_T2D);_T2F=(int)_T2E;return _T2F;}case 5: _T30=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T30;_T31=_TA8->f2;_TA6=(void*)_T31;}{void*t=_TA6;_T32=t;_T33=(unsigned)_T32;_T34=k;_T35=
# 236
Cyc_AssnDef_combine(_T33,_T34);_T36=(int)_T35;return _T36;}case 6: _T37=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T37;_T38=_TA8->f1;_TA6=(void*)_T38;_T39=_TA8->f2;_TA3=(void*)_T39;}{void*t1=_TA6;void*t2=_TA3;_T3A=t1;_T3B=(unsigned)_T3A;_T3C=t2;_T3D=(unsigned)_T3C;_T3E=k;_T3F=
# 238
Cyc_AssnDef_combine(_T3D,_T3E);_T40=Cyc_AssnDef_combine(_T3B,_T3F);_T41=(int)_T40;return _T41;}case 7: _T42=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T42;_T43=_TA8->f1;_TA6=(void*)_T43;_T44=_TA8->f2;_TA3=(void*)_T44;_T45=_TA8->f3;_TA2=(void*)_T45;}{void*t1=_TA6;void*t2=_TA3;void*t3=_TA2;_T46=t1;_T47=(unsigned)_T46;_T48=t2;_T49=(unsigned)_T48;_T4A=t3;_T4B=(unsigned)_T4A;_T4C=k;_T4D=
# 241
Cyc_AssnDef_combine(_T4B,_T4C);_T4E=
# 240
Cyc_AssnDef_combine(_T49,_T4D);_T4F=Cyc_AssnDef_combine(_T47,_T4E);_T50=(int)_T4F;return _T50;}case 9: _T51=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T51;_T52=_TA8->f1;_TA6=(void*)_T52;_TA7=_TA8->f2;}{void*t=_TA6;unsigned i=_TA7;_T53=t;_T54=(unsigned)_T53;_T55=k + i;_T56=
# 243
Cyc_AssnDef_combine(_T54,_T55);_T57=(int)_T56;return _T57;}case 8: _T58=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T58;_TA5=_TA8->f1;_TA7=_TA8->f2;_TA6=_TA8->f3;}{int is_union=_TA5;unsigned tag=_TA7;struct Cyc_List_List*ts=_TA6;_T59=k + tag;_T5A=is_union;_T5B=(unsigned)_T5A;{
# 245
unsigned res=_T59 + _T5B;
_TL1C: if(ts!=0)goto _TL1A;else{goto _TL1B;}
_TL1A: _T5C=ts;_T5D=_T5C->hd;_T5E=(unsigned)_T5D;_T5F=res;res=Cyc_AssnDef_combine(_T5E,_T5F);_T60=ts;
# 246
ts=_T60->tl;goto _TL1C;_TL1B: _T61=res;_T62=(int)_T61;
# 248
return _T62;}}case 10: _T63=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T63;_T64=_TA8->f1;_TA6=(void*)_T64;_TA7=_TA8->f2;_T65=_TA8->f3;_TA3=(void*)_T65;}{void*a=_TA6;unsigned i=_TA7;void*v=_TA3;_T66=a;_T67=(unsigned)_T66;_T68=v;_T69=(unsigned)_T68;_T6A=i + k;_T6B=
# 250
Cyc_AssnDef_combine(_T69,_T6A);_T6C=Cyc_AssnDef_combine(_T67,_T6B);_T6D=(int)_T6C;return _T6D;}case 11: _T6E=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T6E;_TA6=_TA8->f1;_T6F=_TA8->f2;_TA3=(void*)_T6F;}{struct Cyc_Absyn_Vardecl*vd=_TA6;void*invok=_TA3;_T70=invok;_T71=(unsigned)_T70;_T72=vd;_T73=(unsigned)_T72;_T74=k;_T75=
# 252
Cyc_AssnDef_combine(_T73,_T74);_T76=Cyc_AssnDef_combine(_T71,_T75);_T77=(int)_T76;return _T77;}case 12: _T78=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T78;_TA6=_TA8->f1;_TA5=_TA8->f2;_T79=_TA8->f3;_TA3=(void*)_T79;_T7A=_TA8->f4;_TA2=(void*)_T7A;}{struct Cyc_Absyn_Exp*e=_TA6;int i=_TA5;void*n=_TA3;void*invok=_TA2;_T7B=e;_T7C=(unsigned)_T7B;_T7D=n;_T7E=(unsigned)_T7D;_T7F=invok;_T80=(unsigned)_T7F;_T81=k;_T82=i;_T83=(unsigned)_T82;_T84=_T81 + _T83;_T85=
# 255
Cyc_AssnDef_combine(_T80,_T84);_T86=
# 254
Cyc_AssnDef_combine(_T7E,_T85);_T87=Cyc_AssnDef_combine(_T7C,_T86);_T88=(int)_T87;return _T88;}case 13: _T89=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T89;_T8A=_TA8->f1;_TA6=(void*)_T8A;_TA7=_TA8->f2;}{void*t=_TA6;unsigned i=_TA7;_T8B=t;_T8C=(unsigned)_T8B;_T8D=k + i;_T8E=
# 257
Cyc_AssnDef_combine(_T8C,_T8D);_T8F=(int)_T8E;return _T8F;}case 14: _T90=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T90;_T91=_TA8->f1;_TA6=(void*)_T91;_T92=_TA8->f2;_TA3=(void*)_T92;}{void*t=_TA6;void*index=_TA3;_T93=t;_T94=(unsigned)_T93;_T95=index;_T96=(unsigned)_T95;_T97=k;_T98=
# 259
Cyc_AssnDef_combine(_T96,_T97);_T99=Cyc_AssnDef_combine(_T94,_T98);_T9A=(int)_T99;return _T9A;}default: _T9B=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TA8=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T9B;_T9C=_TA8->f1;_TA6=(void*)_T9C;}{void*t=_TA6;_T9D=t;_T9E=(unsigned)_T9D;_T9F=k;_TA0=
# 261
Cyc_AssnDef_combine(_T9E,_T9F);_TA1=(int)_TA0;return _TA1;}};}}struct _tuple15{void*f0;void*f1;};
# 265
int Cyc_AssnDef_termcmp(void*_t1,void*_t2){void*_T0;const unsigned*_T1;unsigned _T2;void*_T3;const unsigned*_T4;unsigned _T5;unsigned _T6;int _T7;struct _tuple15 _T8;void*_T9;int*_TA;unsigned _TB;void*_TC;int*_TD;int _TE;void*_TF;void*_T10;int _T11;struct Cyc_Absyn_Vardecl*_T12;int _T13;struct Cyc_Absyn_Vardecl*_T14;int _T15;int _T16;void*_T17;int*_T18;int _T19;void*_T1A;void*_T1B;unsigned _T1C;int _T1D;void*_T1E;int*_T1F;int _T20;void*_T21;void*_T22;void*_T23;void*_T24;unsigned _T25;int _T26;void*_T27;int*_T28;int _T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;enum Cyc_Absyn_Primop _T32;int _T33;enum Cyc_Absyn_Primop _T34;int _T35;int _T36;int _T37;void*_T38;int*_T39;int _T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;int _T41;void*_T42;int*_T43;int _T44;void*_T45;void*_T46;void*_T47;void*_T48;enum Cyc_Absyn_Primop _T49;int _T4A;enum Cyc_Absyn_Primop _T4B;int _T4C;int _T4D;void*_T4E;int*_T4F;int _T50;void*_T51;void*_T52;int _T53;void*_T54;int*_T55;int _T56;void*_T57;void*_T58;void*_T59;void*_T5A;struct Cyc_Absyn_Vardecl*_T5B;int _T5C;struct Cyc_Absyn_Vardecl*_T5D;int _T5E;int _T5F;void*_T60;int*_T61;int _T62;void*_T63;void*_T64;void*_T65;void*_T66;void*_T67;void*_T68;int _T69;void*_T6A;int*_T6B;int _T6C;void*_T6D;void*_T6E;void*_T6F;void*_T70;void*_T71;void*_T72;void*_T73;void*_T74;int _T75;int _T76;void*_T77;int*_T78;int _T79;void*_T7A;void*_T7B;void*_T7C;void*_T7D;void*_T7E;void*_T7F;unsigned _T80;int _T81;int _T82;void*_T83;int*_T84;int _T85;void*_T86;void*_T87;void*_T88;void*_T89;void*_T8A;int*_T8B;int _T8C;void*_T8D;void*_T8E;void*_T8F;void*_T90;unsigned _T91;int _T92;void*_T93;int*_T94;int _T95;void*_T96;void*_T97;unsigned _T98;int _T99;int _T9A;struct Cyc_List_List*_T9B;void*_T9C;struct Cyc_List_List*_T9D;void*_T9E;int _T9F;struct Cyc_List_List*_TA0;struct Cyc_List_List*_TA1;int _TA2;void*_TA3;int*_TA4;int _TA5;void*_TA6;void*_TA7;void*_TA8;void*_TA9;void*_TAA;void*_TAB;struct Cyc_Absyn_Exp*_TAC;int _TAD;struct Cyc_Absyn_Exp*_TAE;int _TAF;int _TB0;int _TB1;int _TB2;void*_TB3;int*_TB4;int _TB5;void*_TB6;void*_TB7;void*_TB8;void*_TB9;void*_TBA;void*_TBB;int _TBC;int(*_TBD)(struct _fat_ptr,struct _fat_ptr);void*(*_TBE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TBF;struct _fat_ptr _TC0;
void*t1=_t1;
void*t2=_t2;
LOOP: _T0=t1;_T1=(const unsigned*)_T0;_T2=*_T1;_T3=t2;_T4=(const unsigned*)_T3;_T5=*_T4;_T6=_T2 - _T5;{
int c=(int)_T6;
if(c==0)goto _TL1D;_T7=c;return _T7;_TL1D:
 if(t1!=t2)goto _TL1F;return 0;_TL1F:{struct _tuple15 _TC1;
_TC1.f0=t1;_TC1.f1=t2;_T8=_TC1;}{struct _tuple15 _TC1=_T8;void*_TC2;void*_TC3;void*_TC4;enum Cyc_Absyn_Primop _TC5;void*_TC6;enum Cyc_Absyn_Primop _TC7;unsigned _TC8;unsigned _TC9;int _TCA;void*_TCB;int _TCC;void*_TCD;_T9=_TC1.f0;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 2: _TC=_TC1.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=2)goto _TL22;_TF=_TC1.f0;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_TF;_TCD=_TCE->f1;_TCC=_TCE->f2;}_T10=_TC1.f1;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T10;_TCB=_TCE->f1;_TCA=_TCE->f2;}{struct Cyc_Absyn_Vardecl*vd1=_TCD;int i=_TCC;struct Cyc_Absyn_Vardecl*vd2=_TCB;int j=_TCA;
# 274
int c=i - j;
if(c==0)goto _TL24;_T11=c;return _T11;_TL24: _T12=vd1;_T13=(int)_T12;_T14=vd2;_T15=(int)_T14;_T16=_T13 - _T15;
return _T16;}_TL22: goto _LL21;case 0: _T17=_TC1.f1;_T18=(int*)_T17;_T19=*_T18;if(_T19!=0)goto _TL26;_T1A=_TC1.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T1A;_TC9=_TCE->f1;}_T1B=_TC1.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T1B;_TC8=_TCE->f1;}{unsigned i1=_TC9;unsigned i2=_TC8;_T1C=i1 - i2;_T1D=(int)_T1C;
# 278
return _T1D;}_TL26: goto _LL21;case 9: _T1E=_TC1.f1;_T1F=(int*)_T1E;_T20=*_T1F;if(_T20!=9)goto _TL28;_T21=_TC1.f0;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T21;_T22=_TCE->f1;_TCD=(void*)_T22;_TC9=_TCE->f2;}_T23=_TC1.f1;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T23;_T24=_TCE->f1;_TCB=(void*)_T24;_TC8=_TCE->f2;}{void*tm1=_TCD;unsigned i1=_TC9;void*tm2=_TCB;unsigned i2=_TC8;_T25=i1 - i2;{
# 280
int c=(int)_T25;
if(c==0)goto _TL2A;_T26=c;return _T26;_TL2A:
 t1=tm1;
t2=tm2;goto LOOP;}}_TL28: goto _LL21;case 4: _T27=_TC1.f1;_T28=(int*)_T27;_T29=*_T28;if(_T29!=4)goto _TL2C;_T2A=_TC1.f0;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T2A;_TC7=_TCE->f1;_T2B=_TCE->f2;_TCD=(void*)_T2B;_T2C=_TCE->f3;_TCB=(void*)_T2C;_T2D=_TCE->f4;_TC6=(void*)_T2D;}_T2E=_TC1.f1;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T2E;_TC5=_TCE->f1;_T2F=_TCE->f2;_TC4=(void*)_T2F;_T30=_TCE->f3;_TC3=(void*)_T30;_T31=_TCE->f4;_TC2=(void*)_T31;}{enum Cyc_Absyn_Primop p1=_TC7;void*t11=_TCD;void*t12=_TCB;void*tp1=_TC6;enum Cyc_Absyn_Primop p2=_TC5;void*t21=_TC4;void*t22=_TC3;void*tp2=_TC2;_T32=p1;_T33=(int)_T32;_T34=p2;_T35=(int)_T34;{
# 286
int c=_T33 - _T35;
if(c==0)goto _TL2E;_T36=c;return _T36;_TL2E:
 c=Cyc_AssnDef_termcmp(t11,t21);
if(c==0)goto _TL30;_T37=c;return _T37;_TL30:
 t1=t12;
t2=t22;goto LOOP;}}_TL2C: goto _LL21;case 6: _T38=_TC1.f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=6)goto _TL32;_T3B=_TC1.f0;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T3B;_T3C=_TCE->f1;_TCD=(void*)_T3C;_T3D=_TCE->f2;_TCB=(void*)_T3D;}_T3E=_TC1.f1;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T3E;_T3F=_TCE->f1;_TC6=(void*)_T3F;_T40=_TCE->f2;_TC4=(void*)_T40;}{void*t11=_TCD;void*t12=_TCB;void*t21=_TC6;void*t22=_TC4;
# 294
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c==0)goto _TL34;_T41=c;return _T41;_TL34:
 t1=t12;
t2=t22;goto LOOP;}_TL32: goto _LL21;case 3: _T42=_TC1.f1;_T43=(int*)_T42;_T44=*_T43;if(_T44!=3)goto _TL36;_T45=_TC1.f0;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T45;_TC7=_TCE->f1;_T46=_TCE->f2;_TCD=(void*)_T46;}_T47=_TC1.f1;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T47;_TC5=_TCE->f1;_T48=_TCE->f2;_TCB=(void*)_T48;}{enum Cyc_Absyn_Primop p1=_TC7;void*t1a=_TCD;enum Cyc_Absyn_Primop p2=_TC5;void*t2a=_TCB;_T49=p1;_T4A=(int)_T49;_T4B=p2;_T4C=(int)_T4B;{
# 300
int c=_T4A - _T4C;
if(c==0)goto _TL38;_T4D=c;return _T4D;_TL38:
 t1=t1a;
t2=t2a;goto LOOP;}}_TL36: goto _LL21;case 1: _T4E=_TC1.f1;_T4F=(int*)_T4E;_T50=*_T4F;if(_T50!=1)goto _TL3A;_T51=_TC1.f0;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T51;_TCD=_TCE->f1;}_T52=_TC1.f1;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T52;_TCB=_TCE->f1;}{struct Cyc_Absyn_Exp*e1=_TCD;struct Cyc_Absyn_Exp*e2=_TCB;_T53=
# 306
Cyc_Evexp_exp_cmp(e1,e2);return _T53;}_TL3A: goto _LL21;case 11: _T54=_TC1.f1;_T55=(int*)_T54;_T56=*_T55;if(_T56!=11)goto _TL3C;_T57=_TC1.f0;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T57;_TCD=_TCE->f1;_T58=_TCE->f2;_TCB=(void*)_T58;}_T59=_TC1.f1;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T59;_TC6=_TCE->f1;_T5A=_TCE->f2;_TC4=(void*)_T5A;}{struct Cyc_Absyn_Vardecl*vd1=_TCD;void*i1=_TCB;struct Cyc_Absyn_Vardecl*vd2=_TC6;void*i2=_TC4;_T5B=vd1;_T5C=(int)_T5B;_T5D=vd2;_T5E=(int)_T5D;{
# 308
int c=_T5C - _T5E;
if(c==0)goto _TL3E;_T5F=c;return _T5F;_TL3E:
 t1=i1;
t2=i2;goto LOOP;}}_TL3C: goto _LL21;case 14: _T60=_TC1.f1;_T61=(int*)_T60;_T62=*_T61;if(_T62!=14)goto _TL40;_T63=_TC1.f0;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T63;_T64=_TCE->f1;_TCD=(void*)_T64;_T65=_TCE->f2;_TCB=(void*)_T65;}_T66=_TC1.f1;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T66;_T67=_TCE->f1;_TC6=(void*)_T67;_T68=_TCE->f2;_TC4=(void*)_T68;}{void*tm1=_TCD;void*i1=_TCB;void*tm2=_TC6;void*i2=_TC4;
# 314
int c=Cyc_AssnDef_termcmp(i1,i2);
if(c==0)goto _TL42;_T69=c;return _T69;_TL42:
 t1=tm1;
t2=tm2;goto LOOP;}_TL40: goto _LL21;case 7: _T6A=_TC1.f1;_T6B=(int*)_T6A;_T6C=*_T6B;if(_T6C!=7)goto _TL44;_T6D=_TC1.f0;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T6D;_T6E=_TCE->f1;_TCD=(void*)_T6E;_T6F=_TCE->f2;_TCB=(void*)_T6F;_T70=_TCE->f3;_TC6=(void*)_T70;}_T71=_TC1.f1;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T71;_T72=_TCE->f1;_TC4=(void*)_T72;_T73=_TCE->f2;_TC3=(void*)_T73;_T74=_TCE->f3;_TC2=(void*)_T74;}{void*t11=_TCD;void*t12=_TCB;void*t13=_TC6;void*t21=_TC4;void*t22=_TC3;void*t23=_TC2;
# 320
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c==0)goto _TL46;_T75=c;return _T75;_TL46:
 c=Cyc_AssnDef_termcmp(t12,t22);
if(c==0)goto _TL48;_T76=c;return _T76;_TL48:
 t1=t13;
t2=t23;goto LOOP;}_TL44: goto _LL21;case 10: _T77=_TC1.f1;_T78=(int*)_T77;_T79=*_T78;if(_T79!=10)goto _TL4A;_T7A=_TC1.f0;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T7A;_T7B=_TCE->f1;_TCD=(void*)_T7B;_TC9=_TCE->f2;_T7C=_TCE->f3;_TCB=(void*)_T7C;}_T7D=_TC1.f1;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T7D;_T7E=_TCE->f1;_TC6=(void*)_T7E;_TC8=_TCE->f2;_T7F=_TCE->f3;_TC4=(void*)_T7F;}{void*tm1=_TCD;unsigned i1=_TC9;void*v1=_TCB;void*tm2=_TC6;unsigned i2=_TC8;void*v2=_TC4;_T80=i1 - i2;{
# 328
int c=(int)_T80;
if(c==0)goto _TL4C;_T81=c;return _T81;_TL4C:
 c=Cyc_AssnDef_termcmp(tm1,tm2);
if(c==0)goto _TL4E;_T82=c;return _T82;_TL4E:
 t1=v1;
t2=v2;goto LOOP;}}_TL4A: goto _LL21;case 15: _T83=_TC1.f1;_T84=(int*)_T83;_T85=*_T84;if(_T85!=15)goto _TL50;_T86=_TC1.f0;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T86;_T87=_TCE->f1;_TCD=(void*)_T87;}_T88=_TC1.f1;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T88;_T89=_TCE->f1;_TCB=(void*)_T89;}{void*tm1=_TCD;void*tm2=_TCB;
# 336
t1=tm1;
t2=tm2;goto LOOP;}_TL50: goto _LL21;case 13: _T8A=_TC1.f1;_T8B=(int*)_T8A;_T8C=*_T8B;if(_T8C!=13)goto _TL52;_T8D=_TC1.f0;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T8D;_T8E=_TCE->f1;_TCD=(void*)_T8E;_TC9=_TCE->f2;}_T8F=_TC1.f1;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T8F;_T90=_TCE->f1;_TCB=(void*)_T90;_TC8=_TCE->f2;}{void*tm1=_TCD;unsigned i1=_TC9;void*tm2=_TCB;unsigned i2=_TC8;_T91=i1 - i2;{
# 340
int c=(int)_T91;
if(c==0)goto _TL54;_T92=c;return _T92;_TL54:
 t1=tm1;
t2=tm2;goto LOOP;}}_TL52: goto _LL21;case 8: _T93=_TC1.f1;_T94=(int*)_T93;_T95=*_T94;if(_T95!=8)goto _TL56;_T96=_TC1.f0;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T96;_TCC=_TCE->f1;_TC9=_TCE->f2;_TCD=_TCE->f3;}_T97=_TC1.f1;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T97;_TCA=_TCE->f1;_TC8=_TCE->f2;_TCB=_TCE->f3;}{int is_union1=_TCC;unsigned tag1=_TC9;struct Cyc_List_List*ts1=_TCD;int is_union2=_TCA;unsigned tag2=_TC8;struct Cyc_List_List*ts2=_TCB;_T98=tag1 - tag2;{
# 346
int c=(int)_T98;
if(c==0)goto _TL58;_T99=c;return _T99;_TL58:
 c=is_union2 - is_union1;
if(c==0)goto _TL5A;_T9A=c;return _T9A;_TL5A:
 _TL5F: if(ts1!=0)goto _TL60;else{goto _TL5E;}_TL60: if(ts2!=0)goto _TL5D;else{goto _TL5E;}
_TL5D: _T9B=ts1;_T9C=_T9B->hd;_T9D=ts2;_T9E=_T9D->hd;c=Cyc_AssnDef_termcmp(_T9C,_T9E);
if(c==0)goto _TL61;_T9F=c;return _T9F;_TL61: _TA0=ts1;
# 350
ts1=_TA0->tl;_TA1=ts2;ts2=_TA1->tl;goto _TL5F;_TL5E:
# 354
 if(ts1!=ts2)goto _TL63;return 0;_TL63:
 if(ts1!=0)goto _TL65;_TA2=- 1;return _TA2;_TL65:
 return 1;}}_TL56: goto _LL21;case 12: _TA3=_TC1.f1;_TA4=(int*)_TA3;_TA5=*_TA4;if(_TA5!=12)goto _TL67;_TA6=_TC1.f0;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TA6;_TCD=_TCE->f1;_TCC=_TCE->f2;_TA7=_TCE->f3;_TCB=(void*)_TA7;_TA8=_TCE->f4;_TC6=(void*)_TA8;}_TA9=_TC1.f1;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TA9;_TC4=_TCE->f1;_TCA=_TCE->f2;_TAA=_TCE->f3;_TC3=(void*)_TAA;_TAB=_TCE->f4;_TC2=(void*)_TAB;}{struct Cyc_Absyn_Exp*e1=_TCD;int i1=_TCC;void*n1=_TCB;void*v1=_TC6;struct Cyc_Absyn_Exp*e2=_TC4;int i2=_TCA;void*n2=_TC3;void*v2=_TC2;_TAC=e1;_TAD=(int)_TAC;_TAE=e2;_TAF=(int)_TAE;{
# 358
int c=_TAD - _TAF;
if(c==0)goto _TL69;_TB0=c;return _TB0;_TL69:
 c=i1 - i2;
if(c==0)goto _TL6B;_TB1=c;return _TB1;_TL6B:
 c=Cyc_AssnDef_termcmp(n1,n2);
if(c==0)goto _TL6D;_TB2=c;return _TB2;_TL6D:
 t1=v1;
t2=v2;goto LOOP;}}_TL67: goto _LL21;default: _TB3=_TC1.f1;_TB4=(int*)_TB3;_TB5=*_TB4;if(_TB5!=5)goto _TL6F;_TB6=_TC1.f0;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_TB6;_TB7=_TCE->f1;_TCD=(void*)_TB7;_TB8=_TCE->f2;_TCB=(void*)_TB8;}_TB9=_TC1.f1;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TCE=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_TB9;_TBA=_TCE->f1;_TC6=(void*)_TBA;_TBB=_TCE->f2;_TC4=(void*)_TBB;}{void*tp1=_TCD;void*tm1=_TCB;void*tp2=_TC6;void*tm2=_TC4;
# 368
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c==0)goto _TL71;_TBC=c;return _TBC;_TL71:
 t1=tm1;
t2=tm2;goto LOOP;}_TL6F: _LL21: _TBE=Cyc_Warn_impos;{
# 373
int(*_TCE)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TBE;_TBD=_TCE;}_TBF=_tag_fat("different terms but same codes!",sizeof(char),32U);_TC0=_tag_fat(0U,sizeof(void*),0);_TBD(_TBF,_TC0);;};}}}
# 377
static void*Cyc_AssnDef_copy_term(void*t,unsigned*size){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;unsigned*_T5;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T6;void*_T7;void*_T8;unsigned*_T9;struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TA;void*_TB;void*_TC;void*_TD;void*_TE;unsigned*_TF;unsigned _T10;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T11;void*_T12;void*_T13;void*_T14;unsigned*_T15;struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;unsigned*_T1C;unsigned _T1D;unsigned _T1E;unsigned _T1F;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T20;void*_T21;void*_T22;void*_T23;void*_T24;unsigned*_T25;unsigned _T26;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;unsigned*_T2C;unsigned _T2D;struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;unsigned*_T34;unsigned _T35;unsigned _T36;unsigned _T37;struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;void*_T3D;unsigned*_T3E;unsigned _T3F;unsigned _T40;unsigned _T41;unsigned _T42;unsigned _T43;struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T44;void*_T45;void*_T46;void*_T47;void*_T48;unsigned*_T49;unsigned _T4A;struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;unsigned*_T50;unsigned _T51;unsigned _T52;unsigned _T53;struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T54;void*_T55;void*_T56;void*_T57;unsigned*_T58;unsigned*_T59;struct Cyc_List_List*_T5A;void*_T5B;unsigned _T5C;struct Cyc_List_List*_T5D;struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T5E;void*_T5F;void*_T60;void*_T61;void*_T62;void*_T63;unsigned*_T64;unsigned _T65;unsigned _T66;unsigned _T67;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T68;void*_T69;void*_T6A;void*_T6B;void*_T6C;unsigned*_T6D;unsigned _T6E;struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T6F;void*_T70;void*_T71;void*_T72;void*_T73;void*_T74;unsigned*_T75;unsigned _T76;unsigned _T77;unsigned _T78;struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T79;void*_T7A;void*_T7B;void*_T7C;unsigned*_T7D;unsigned _T7E;struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T7F;void*_T80;void*_T81;enum Cyc_Absyn_Primop _T82;int _T83;void*_T84;void*_T85;void*_T86;unsigned _T87;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T3;_T87=_T88->f1;_T4=_T88->f2;_T86=(void*)_T4;}{unsigned i=_T87;void*tp=_T86;_T5=size;
# 379
*_T5=1U;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_T88->tag=0;_T88->f1=i;_T88->f2=tp;_T6=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T88;}_T7=(void*)_T6;return _T7;}case 1: _T8=t;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T8;_T86=_T88->f1;}{struct Cyc_Absyn_Exp*e=_T86;_T9=size;
*_T9=1U;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_T88->tag=1;_T88->f1=e;_TA=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T88;}_TB=(void*)_TA;return _TB;}case 11: _TC=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_TC;_T86=_T88->f1;_TD=_T88->f2;_T85=(void*)_TD;_TE=_T88->f3;_T84=(void*)_TE;}{struct Cyc_Absyn_Vardecl*vd=_T86;void*t=_T85;void*tp=_T84;_TF=size;_T10=
# 382
Cyc_AssnDef_term_size(t);*_TF=1U + _T10;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Addr_AssnDef_Term_struct));_T88->tag=11;
_T88->f1=vd;_T88->f2=t;_T88->f3=tp;_T11=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T88;}_T12=(void*)_T11;return _T12;}case 2: _T13=t;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T13;_T86=_T88->f1;_T83=_T88->f2;_T14=_T88->f3;_T85=(void*)_T14;}{struct Cyc_Absyn_Vardecl*vd=_T86;int i=_T83;void*tp=_T85;_T15=size;
# 385
*_T15=1U;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_T88->tag=2;
_T88->f1=vd;_T88->f2=i;_T88->f3=tp;_T16=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T88;}_T17=(void*)_T16;return _T17;}case 4: _T18=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T18;_T82=_T88->f1;_T19=_T88->f2;_T86=(void*)_T19;_T1A=_T88->f3;_T85=(void*)_T1A;_T1B=_T88->f4;_T84=(void*)_T1B;}{enum Cyc_Absyn_Primop p=_T82;void*t1=_T86;void*t2=_T85;void*tp=_T84;_T1C=size;_T1D=
# 388
Cyc_AssnDef_term_size(t1);_T1E=1U + _T1D;_T1F=Cyc_AssnDef_term_size(t2);*_T1C=_T1E + _T1F;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Binop_AssnDef_Term_struct));_T88->tag=4;
_T88->f1=p;_T88->f2=t1;_T88->f3=t2;_T88->f4=tp;_T20=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T88;}_T21=(void*)_T20;return _T21;}case 3: _T22=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T22;_T82=_T88->f1;_T23=_T88->f2;_T86=(void*)_T23;_T24=_T88->f3;_T85=(void*)_T24;}{enum Cyc_Absyn_Primop p=_T82;void*t=_T86;void*tp=_T85;_T25=size;_T26=
# 391
Cyc_AssnDef_term_size(t);*_T25=1U + _T26;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Unop_AssnDef_Term_struct));_T88->tag=3;
_T88->f1=p;_T88->f2=t;_T88->f3=tp;_T27=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T88;}_T28=(void*)_T27;return _T28;}case 5: _T29=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T29;_T2A=_T88->f1;_T86=(void*)_T2A;_T2B=_T88->f2;_T85=(void*)_T2B;}{void*tp=_T86;void*t=_T85;_T2C=size;_T2D=
# 394
Cyc_AssnDef_term_size(t);*_T2C=1U + _T2D;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_T88->tag=5;
_T88->f1=tp;_T88->f2=t;_T2E=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T88;}_T2F=(void*)_T2E;return _T2F;}case 6: _T30=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T30;_T31=_T88->f1;_T86=(void*)_T31;_T32=_T88->f2;_T85=(void*)_T32;_T33=_T88->f3;_T84=(void*)_T33;}{void*t1=_T86;void*t2=_T85;void*tp=_T84;_T34=size;_T35=
# 397
Cyc_AssnDef_term_size(t1);_T36=1U + _T35;_T37=Cyc_AssnDef_term_size(t2);*_T34=_T36 + _T37;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Select_AssnDef_Term_struct));_T88->tag=6;
_T88->f1=t1;_T88->f2=t2;_T88->f3=tp;_T38=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T88;}_T39=(void*)_T38;return _T39;}case 7: _T3A=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T3A;_T3B=_T88->f1;_T86=(void*)_T3B;_T3C=_T88->f2;_T85=(void*)_T3C;_T3D=_T88->f3;_T84=(void*)_T3D;}{void*t1=_T86;void*t2=_T85;void*t3=_T84;_T3E=size;_T3F=
# 400
Cyc_AssnDef_term_size(t1);_T40=1U + _T3F;_T41=Cyc_AssnDef_term_size(t2);_T42=_T40 + _T41;_T43=Cyc_AssnDef_term_size(t3);*_T3E=_T42 + _T43;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Update_AssnDef_Term_struct));_T88->tag=7;
_T88->f1=t1;_T88->f2=t2;_T88->f3=t3;_T44=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T88;}_T45=(void*)_T44;return _T45;}case 9: _T46=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T46;_T47=_T88->f1;_T86=(void*)_T47;_T87=_T88->f2;_T48=_T88->f3;_T85=(void*)_T48;}{void*t=_T86;unsigned i=_T87;void*tp=_T85;_T49=size;_T4A=
# 403
Cyc_AssnDef_term_size(t);*_T49=1U + _T4A;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Proj_AssnDef_Term_struct));_T88->tag=9;
_T88->f1=t;_T88->f2=i;_T88->f3=tp;_T4B=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T88;}_T4C=(void*)_T4B;return _T4C;}case 10: _T4D=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T4D;_T4E=_T88->f1;_T86=(void*)_T4E;_T87=_T88->f2;_T4F=_T88->f3;_T85=(void*)_T4F;}{void*t=_T86;unsigned i=_T87;void*v=_T85;_T50=size;_T51=
# 406
Cyc_AssnDef_term_size(t);_T52=1U + _T51;_T53=Cyc_AssnDef_term_size(v);*_T50=_T52 + _T53;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct));_T88->tag=10;
_T88->f1=t;_T88->f2=i;_T88->f3=v;_T54=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T88;}_T55=(void*)_T54;return _T55;}case 8: _T56=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T56;_T83=_T88->f1;_T87=_T88->f2;_T86=_T88->f3;_T57=_T88->f4;_T85=(void*)_T57;}{int is_union=_T83;unsigned tag=_T87;struct Cyc_List_List*ts=_T86;void*tp=_T85;_T58=size;
# 409
*_T58=1U;{
struct Cyc_List_List*p=ts;_TL77: if(p!=0)goto _TL75;else{goto _TL76;}_TL75: _T59=size;_T5A=p;_T5B=_T5A->hd;_T5C=Cyc_AssnDef_term_size(_T5B);*_T59=*_T59 + _T5C;_T5D=p;p=_T5D->tl;goto _TL77;_TL76:;}{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct));_T88->tag=8;
_T88->f1=is_union;_T88->f2=tag;_T88->f3=ts;_T88->f4=tp;_T5E=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T88;}_T5F=(void*)_T5E;return _T5F;}case 12: _T60=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T60;_T86=_T88->f1;_T83=_T88->f2;_T61=_T88->f3;_T85=(void*)_T61;_T62=_T88->f4;_T84=(void*)_T62;_T63=_T88->f5;_T81=(void*)_T63;}{struct Cyc_Absyn_Exp*e=_T86;int i=_T83;void*n=_T85;void*v=_T84;void*tp=_T81;_T64=size;_T65=
# 413
Cyc_AssnDef_term_size(v);_T66=1U + _T65;_T67=Cyc_AssnDef_term_size(n);*_T64=_T66 + _T67;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct));_T88->tag=12;
_T88->f1=e;_T88->f2=i;_T88->f3=n;_T88->f4=v;_T88->f5=tp;_T68=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T88;}_T69=(void*)_T68;return _T69;}case 13: _T6A=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T6A;_T6B=_T88->f1;_T86=(void*)_T6B;_T87=_T88->f2;_T6C=_T88->f3;_T85=(void*)_T6C;}{void*t=_T86;unsigned i=_T87;void*tp=_T85;_T6D=size;_T6E=
# 416
Cyc_AssnDef_term_size(t);*_T6D=1U + _T6E;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct));_T88->tag=13;
_T88->f1=t;_T88->f2=i;_T88->f3=tp;_T6F=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T88;}_T70=(void*)_T6F;return _T70;}case 14: _T71=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T71;_T72=_T88->f1;_T86=(void*)_T72;_T73=_T88->f2;_T85=(void*)_T73;_T74=_T88->f3;_T84=(void*)_T74;}{void*t=_T86;void*i=_T85;void*tp=_T84;_T75=size;_T76=
# 419
Cyc_AssnDef_term_size(t);_T77=1U + _T76;_T78=Cyc_AssnDef_term_size(i);*_T75=_T77 + _T78;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct));_T88->tag=14;
_T88->f1=t;_T88->f2=i;_T88->f3=tp;_T79=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T88;}_T7A=(void*)_T79;return _T7A;}default: _T7B=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T88=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T7B;_T7C=_T88->f1;_T86=(void*)_T7C;}{void*t=_T86;_T7D=size;_T7E=
# 422
Cyc_AssnDef_term_size(t);*_T7D=1U + _T7E;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T88=_cycalloc(sizeof(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct));_T88->tag=15;
_T88->f1=t;_T7F=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T88;}_T80=(void*)_T7F;return _T80;}};}
# 427
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_zero_value={0,0U,0};
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_one_value={0,1U,0};
# 430
static struct Cyc_Set_Set*Cyc_AssnDef_empty_term_set (void){struct Cyc_Set_Set**_T0;struct Cyc_Set_Set**_T1;struct Cyc_Set_Set*_T2;
static struct Cyc_Set_Set**term_set=0;
if(term_set!=0)goto _TL78;{struct Cyc_Set_Set**_T3=_cycalloc(sizeof(struct Cyc_Set_Set*));
*_T3=Cyc_Set_empty(Cyc_AssnDef_termcmp);_T0=(struct Cyc_Set_Set**)_T3;}term_set=_T0;goto _TL79;_TL78: _TL79: _T1=term_set;_T2=*_T1;
# 435
return _T2;}struct Cyc_AssnDef_TermHashedInfo{void*term;unsigned size;struct Cyc_Set_Set**free_logicvar_set;};
# 445
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 448
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_term_hash_cons_table (void){void(*_T0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T2;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T3;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T4;void*_T5;struct Cyc_AssnDef_TermHashedInfo*_T6;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T7;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T8;struct Cyc_Set_Set**_T9;void(*_TA)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*);void(*_TB)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_TC;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TD;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_TE;void*_TF;struct Cyc_AssnDef_TermHashedInfo*_T10;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T11;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T12;struct Cyc_Set_Set**_T13;struct Cyc_Hashtable_Table*_T14;
# 450
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table!=0)goto _TL7A;
h=Cyc_Hashtable_create(221,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;{
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();_T1=Cyc_Hashtable_insert;{
void(*_T15)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))_T1;_T0=_T15;}_T2=h;_T3=& Cyc_AssnDef_zero_value;_T4=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T3;_T5=(void*)_T4;{struct Cyc_AssnDef_TermHashedInfo*_T15=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_T7=& Cyc_AssnDef_zero_value;_T8=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T7;_T15->term=(void*)_T8;_T15->size=1U;{struct Cyc_Set_Set**_T16=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T16=tst;_T9=(struct Cyc_Set_Set**)_T16;}_T15->free_logicvar_set=_T9;_T6=(struct Cyc_AssnDef_TermHashedInfo*)_T15;}_T0(_T2,_T5,_T6);_TB=Cyc_Hashtable_insert;{
void(*_T15)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))_TB;_TA=_T15;}_TC=h;_TD=& Cyc_AssnDef_one_value;_TE=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TD;_TF=(void*)_TE;{struct Cyc_AssnDef_TermHashedInfo*_T15=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_T11=& Cyc_AssnDef_one_value;_T12=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T11;_T15->term=(void*)_T12;_T15->size=1U;{struct Cyc_Set_Set**_T16=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T16=tst;_T13=(struct Cyc_Set_Set**)_T16;}_T15->free_logicvar_set=_T13;_T10=(struct Cyc_AssnDef_TermHashedInfo*)_T15;}_TA(_TC,_TF,_T10);}goto _TL7B;
# 458
_TL7A: h=Cyc_AssnDef_term_hash_cons_table;_TL7B: _T14=h;
# 460
return _T14;}
# 463
static void*Cyc_AssnDef_hash_cons_term(void*t){struct Cyc_AssnDef_TermHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));void**(*_T1)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_Hashtable_Table*_T2;void*_T3;void*_T4;unsigned*_T5;void(*_T6)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*);void(*_T7)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T8;void*_T9;struct Cyc_AssnDef_TermHashedInfo*_TA;void*_TB;struct Cyc_AssnDef_TermHashedInfo**_TC;struct Cyc_AssnDef_TermHashedInfo*_TD;void*_TE;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();_T1=Cyc_Hashtable_lookup_other_opt;{
struct Cyc_AssnDef_TermHashedInfo**(*_TF)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))_T1;_T0=_TF;}_T2=h;_T3=t;{struct Cyc_AssnDef_TermHashedInfo**resopt=_T0(_T2,_T3,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
if(resopt!=0)goto _TL7C;{
unsigned size=0U;_T4=t;_T5=& size;{
void*t2=Cyc_AssnDef_copy_term(_T4,_T5);_T7=Cyc_Hashtable_insert;{
# 481 "assndef.cyc"
void(*_TF)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))_T7;_T6=_TF;}_T8=h;_T9=t2;{struct Cyc_AssnDef_TermHashedInfo*_TF=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_TF->term=t2;_TF->size=size;_TF->free_logicvar_set=0;_TA=(struct Cyc_AssnDef_TermHashedInfo*)_TF;}_T6(_T8,_T9,_TA);_TB=t2;
return _TB;}}
# 484
_TL7C: _TC=resopt;_TD=*_TC;_TE=_TD->term;return _TE;}}
# 487
static void**Cyc_AssnDef_lookup_term_opt(struct Cyc_Hashtable_Table*h,void*t){struct Cyc_Hashtable_Table*_T0;struct _fat_ptr _T1;unsigned char*_T2;struct Cyc_Hashtable_Bucket*_T3;int _T4;unsigned _T5;struct _fat_ptr _T6;unsigned _T7;unsigned _T8;int _T9;struct Cyc_Hashtable_Bucket _TA;struct Cyc_Hashtable_Cell*_TB;void*_TC;void*_TD;int _TE;struct Cyc_Hashtable_Cell*_TF;void**_T10;struct Cyc_Hashtable_Cell*_T11;_T0=h;{
struct _fat_ptr tab=_T0->tab;_T1=tab;_T2=_T1.curr;_T3=(struct Cyc_Hashtable_Bucket*)_T2;_T4=
Cyc_AssnDef_termhash(t);_T5=(unsigned)_T4;_T6=tab;_T7=_get_fat_size(_T6,sizeof(struct Cyc_Hashtable_Bucket));_T8=_T5 % _T7;_T9=(int)_T8;_TA=_T3[_T9];{struct Cyc_Hashtable_Cell*p=_TA.cells;
_TL81: if(p!=0)goto _TL7F;else{goto _TL80;}
_TL7F: _TB=p;_TC=_TB->key;_TD=t;_TE=Cyc_AssnDef_termcmp(_TC,_TD);if(_TE!=0)goto _TL82;_TF=p;_T10=& _TF->value;return _T10;_TL82: _T11=p;
# 490
p=_T11->next;goto _TL81;_TL80:
# 493
 return 0;}}}
# 497
unsigned Cyc_AssnDef_term_size(void*t){struct Cyc_AssnDef_TermHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*);unsigned _T1;struct Cyc_AssnDef_TermHashedInfo**_T2;struct Cyc_AssnDef_TermHashedInfo*_T3;unsigned _T4;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();{
struct Cyc_AssnDef_TermHashedInfo**(*_T5)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_AssnDef_lookup_term_opt;_T0=_T5;}{struct Cyc_AssnDef_TermHashedInfo**resopt=_T0(h,t);
if(resopt!=0)goto _TL84;
Cyc_AssnDef_hash_cons_term(t);_T1=
Cyc_AssnDef_term_size(t);return _T1;_TL84: _T2=resopt;{
# 504
struct Cyc_AssnDef_TermHashedInfo*res=*_T2;_T3=res;_T4=_T3->size;
return _T4;}}}
# 511
struct Cyc_Set_Set*Cyc_AssnDef_term_fr_logicvar(void*t){struct Cyc_AssnDef_TermHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_TermHashedInfo**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_TermHashedInfo**_T2;struct Cyc_AssnDef_TermHashedInfo*_T3;struct Cyc_Set_Set**_T4;struct Cyc_AssnDef_TermHashedInfo**_T5;struct Cyc_AssnDef_TermHashedInfo*_T6;struct Cyc_Set_Set**_T7;struct Cyc_Set_Set*_T8;void*_T9;int*_TA;unsigned _TB;void*_TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;struct Cyc_Set_Set*_T1C;struct Cyc_Set_Set*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;struct Cyc_Set_Set*_T22;struct Cyc_Set_Set*_T23;struct Cyc_Set_Set*_T24;struct Cyc_Set_Set*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;struct Cyc_List_List*(*_T33)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T34)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T35;struct Cyc_Set_Set*(*_T36)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*);void*(*_T37)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Set_Set*(*_T38)(struct Cyc_Set_Set*,struct Cyc_Set_Set*);struct Cyc_Set_Set*_T39;struct Cyc_List_List*_T3A;struct Cyc_AssnDef_TermHashedInfo**_T3B;struct Cyc_AssnDef_TermHashedInfo*_T3C;struct Cyc_Set_Set**_T3D;struct Cyc_Set_Set*_T3E;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();{
struct Cyc_AssnDef_TermHashedInfo**(*_T3F)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_AssnDef_lookup_term_opt;_T0=_T3F;}{struct Cyc_AssnDef_TermHashedInfo**thinfo=_T0(h,t);
# 515
if(thinfo!=0)goto _TL86;
t=Cyc_AssnDef_hash_cons_term(t);{
struct Cyc_AssnDef_TermHashedInfo**(*_T3F)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_AssnDef_lookup_term_opt;_T1=_T3F;}thinfo=_T1(h,t);goto _TL87;_TL86: _TL87: _T2=
# 519
_check_null(thinfo);_T3=*_T2;_T4=_T3->free_logicvar_set;if(_T4==0)goto _TL88;_T5=thinfo;_T6=*_T5;_T7=_T6->free_logicvar_set;_T8=*_T7;
return _T8;_TL88: {
struct Cyc_Set_Set*res;{struct Cyc_List_List*_T3F;void*_T40;void*_T41;void*_T42;_T9=t;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 0: goto _LL4;case 1: _LL4:
# 525
 res=Cyc_AssnDef_empty_term_set();goto _LL0;case 2: _TC=t;
# 527
res=Cyc_Set_singleton(Cyc_AssnDef_termcmp,_TC);goto _LL0;case 12: _TD=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TD;_TE=_T43->f3;_T42=(void*)_TE;_TF=_T43->f4;_T41=(void*)_TF;}{void*t1=_T42;void*t2=_T41;_T42=t1;_T41=t2;goto _LLA;}case 6: _T10=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T10;_T11=_T43->f1;_T42=(void*)_T11;_T12=_T43->f2;_T41=(void*)_T12;}_LLA: {void*t1=_T42;void*t2=_T41;_T42=t1;_T41=t2;goto _LLC;}case 10: _T13=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T13;_T14=_T43->f1;_T42=(void*)_T14;_T15=_T43->f3;_T41=(void*)_T15;}_LLC: {void*t1=_T42;void*t2=_T41;_T42=t1;_T41=t2;goto _LLE;}case 4: _T16=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T16;_T17=_T43->f2;_T42=(void*)_T17;_T18=_T43->f3;_T41=(void*)_T18;}_LLE: {void*t1=_T42;void*t2=_T41;_T42=t1;_T41=t2;goto _LL10;}case 14: _T19=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T19;_T1A=_T43->f1;_T42=(void*)_T1A;_T1B=_T43->f2;_T41=(void*)_T1B;}_LL10: {void*t1=_T42;void*t2=_T41;_T1C=
# 538
Cyc_AssnDef_term_fr_logicvar(t1);_T1D=Cyc_AssnDef_term_fr_logicvar(t2);res=Cyc_Set_union_two(_T1C,_T1D);goto _LL0;}case 7: _T1E=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T1E;_T1F=_T43->f1;_T42=(void*)_T1F;_T20=_T43->f2;_T41=(void*)_T20;_T21=_T43->f3;_T40=(void*)_T21;}{void*t1=_T42;void*t2=_T41;void*t3=_T40;_T22=
# 541
Cyc_AssnDef_term_fr_logicvar(t1);_T23=
Cyc_AssnDef_term_fr_logicvar(t2);_T24=
# 541
Cyc_Set_union_two(_T22,_T23);_T25=
# 543
Cyc_AssnDef_term_fr_logicvar(t3);
# 541
res=Cyc_Set_union_two(_T24,_T25);goto _LL0;}case 11: _T26=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T26;_T27=_T43->f2;_T42=(void*)_T27;}{void*t1=_T42;_T42=t1;goto _LL16;}case 3: _T28=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T28;_T29=_T43->f2;_T42=(void*)_T29;}_LL16: {void*t1=_T42;_T42=t1;goto _LL18;}case 9: _T2A=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T2A;_T2B=_T43->f1;_T42=(void*)_T2B;}_LL18: {void*t1=_T42;_T42=t1;goto _LL1A;}case 13: _T2C=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T2C;_T2D=_T43->f1;_T42=(void*)_T2D;}_LL1A: {void*t1=_T42;_T42=t1;goto _LL1C;}case 15: _T2E=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T2E;_T2F=_T43->f1;_T42=(void*)_T2F;}_LL1C: {void*t1=_T42;_T42=t1;goto _LL1E;}case 5: _T30=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T30;_T31=_T43->f2;_T42=(void*)_T31;}_LL1E: {void*t1=_T42;
# 556
res=Cyc_AssnDef_term_fr_logicvar(t1);goto _LL0;}default: _T32=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T32;_T3F=_T43->f3;}{struct Cyc_List_List*ts=_T3F;_T34=Cyc_List_map;{
# 559
struct Cyc_List_List*(*_T43)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*))_T34;_T33=_T43;}_T35=ts;{struct Cyc_List_List*tsets=_T33(Cyc_AssnDef_term_fr_logicvar,_T35);_T37=Cyc_List_fold_left;{
struct Cyc_Set_Set*(*_T43)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*))_T37;_T36=_T43;}_T38=Cyc_Set_union_two;_T39=Cyc_AssnDef_empty_term_set();_T3A=tsets;res=_T36(_T38,_T39,_T3A);goto _LL0;}}}_LL0:;}_T3B=thinfo;_T3C=*_T3B;{struct Cyc_Set_Set**_T3F=_cycalloc(sizeof(struct Cyc_Set_Set*));
# 563
*_T3F=res;_T3D=(struct Cyc_Set_Set**)_T3F;}_T3C->free_logicvar_set=_T3D;_T3E=res;
return _T3E;}}}
# 567
static int Cyc_AssnDef_logicvar_in_term(void*lvar,void*t){int _T0;
struct Cyc_Set_Set*lvset=Cyc_AssnDef_term_fr_logicvar(t);_T0=
Cyc_Set_member(lvset,lvar);return _T0;}
# 572
void*Cyc_AssnDef_uint(unsigned i){struct Cyc_AssnDef_Uint_AssnDef_Term_struct _T0;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct _T5;_T5.tag=0;
_T5.f1=i;_T5.f2=0;_T0=_T5;}{struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 576
void*Cyc_AssnDef_zero (void){struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T0;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1;void*_T2;_T0=& Cyc_AssnDef_zero_value;_T1=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T0;_T2=(void*)_T1;
return _T2;}
# 579
void*Cyc_AssnDef_one (void){struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T0;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1;void*_T2;_T0=& Cyc_AssnDef_one_value;_T1=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T0;_T2=(void*)_T1;
return _T2;}
# 582
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){int _T0;void*_T1;struct Cyc_AssnDef_Const_AssnDef_Term_struct _T2;struct Cyc_AssnDef_Const_AssnDef_Term_struct*_T3;struct Cyc_AssnDef_Const_AssnDef_Term_struct*_T4;void*_T5;void*_T6;
struct _tuple11 _T7=Cyc_Evexp_eval_const_uint_exp(e);int _T8;unsigned _T9;_T9=_T7.f0;_T8=_T7.f1;{unsigned c=_T9;int known=_T8;_T0=known;
if(!_T0)goto _TL8B;_T1=Cyc_AssnDef_uint(c);return _T1;_TL8B:{struct Cyc_AssnDef_Const_AssnDef_Term_struct _TA;_TA.tag=1;
_TA.f1=e;_T2=_TA;}{struct Cyc_AssnDef_Const_AssnDef_Term_struct t=_T2;_T3=& t;_T4=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T3;_T5=(void*)_T4;_T6=
Cyc_AssnDef_hash_cons_term(_T5);return _T6;}}}
# 588
void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*vd,void*invok){struct Cyc_Absyn_Vardecl*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;unsigned _T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_Tqual _TB;void*_TC;void*_TD;struct Cyc_AssnDef_Addr_AssnDef_Term_struct _TE;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TF;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T10;void*_T11;void*_T12;_T0=vd;{
void*tp=_T0->type;{
void*_T13=Cyc_Absyn_compress(tp);_T1=(int*)_T13;_T2=*_T1;if(_T2!=5)goto _TL8D;goto _LL0;_TL8D: _T4=vd;_T5=_T4->rgn;_T6=(unsigned)_T5;
# 593
if(!_T6)goto _TL8F;_T7=vd;_T3=_T7->rgn;goto _TL90;_TL8F: _T3=Cyc_Absyn_heap_rgn_type;_TL90:{void*rgn=_T3;_T8=tp;_T9=rgn;_TA=Cyc_Absyn_al_qual_type;_TB=
Cyc_Absyn_empty_tqual(0U);_TC=Cyc_Absyn_false_type;_TD=Cyc_Absyn_false_type;tp=Cyc_Absyn_at_type(_T8,_T9,_TA,_TB,_TC,_TD);}_LL0:;}{struct Cyc_AssnDef_Addr_AssnDef_Term_struct _T13;_T13.tag=11;
# 596
_T13.f1=vd;_T13.f2=invok;_T13.f3=tp;_TE=_T13;}{struct Cyc_AssnDef_Addr_AssnDef_Term_struct t=_TE;_TF=& t;_T10=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_TF;_T11=(void*)_T10;_T12=
Cyc_AssnDef_hash_cons_term(_T11);return _T12;}}}
# 599
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _T0;struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _T5;_T5.tag=2;
_T5.f1=vd;_T5.f2=i;_T5.f3=tp;_T0=_T5;}{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 603
void*Cyc_AssnDef_update(void*t1,void*t2,void*t3){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;void*_T5;int _T6;void*_T7;unsigned _T8;void*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;struct Cyc_AssnDef_Update_AssnDef_Term_struct _T12;struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T13;struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T14;void*_T15;void*_T16;
LOOP1:{void*_T17;unsigned _T18;void*_T19;_T0=t2;_T1=(int*)_T0;_T2=*_T1;if(_T2!=13)goto _TL91;_T3=t2;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T1A=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T3;_T4=_T1A->f1;_T19=(void*)_T4;_T18=_T1A->f2;_T5=_T1A->f3;_T17=(void*)_T5;}{void*a=_T19;unsigned i=_T18;void*tp=_T17;
# 613
t2=a;{
void*tpopt=0;
if(tp==0)goto _TL93;_T6=Cyc_Tcutil_is_pointer_type(tp);if(!_T6)goto _TL93;
tpopt=Cyc_Tcutil_pointer_elt_type(tp);goto _TL94;_TL93: _TL94: _T7=
Cyc_AssnDef_select(t1,a,tp);_T8=i;_T9=t3;t3=Cyc_AssnDef_aggr_update(_T7,_T8,_T9);goto LOOP1;}}_TL91: goto _LL0;_LL0:;}
# 623
LOOP2:{void*_T17;void*_T18;_TA=t1;_TB=(int*)_TA;_TC=*_TB;if(_TC!=7)goto _TL95;_TD=t1;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T19=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_TD;_TE=_T19->f1;_T18=(void*)_TE;_TF=_T19->f2;_T17=(void*)_TF;}_T10=(void*)_T17;_T11=t2;if(_T10!=_T11)goto _TL97;{void*m=_T18;void*t=_T17;
# 626
t1=m;goto LOOP2;}_TL97: goto _LL8;_TL95: _LL8: goto _LL5;_LL5:;}{struct Cyc_AssnDef_Update_AssnDef_Term_struct _T17;_T17.tag=7;
# 630
_T17.f1=t1;_T17.f2=t2;_T17.f3=t3;_T12=_T17;}{struct Cyc_AssnDef_Update_AssnDef_Term_struct t=_T12;_T13=& t;_T14=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T13;_T15=(void*)_T14;_T16=
Cyc_AssnDef_hash_cons_term(_T15);return _T16;}}
# 634
void*Cyc_AssnDef_aggr_update(void*t1,unsigned i,void*t2){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;void*_TB;int _TC;int _TD;unsigned _TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;void*_T11;void*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct _T1A;struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T1B;struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T1C;void*_T1D;void*_T1E;
LOOP:{struct Cyc_List_List*_T1F;int _T20;void*_T21;unsigned _T22;void*_T23;_T0=t1;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 10: _T3=t1;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T24=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T3;_T4=_T24->f1;_T23=(void*)_T4;_T22=_T24->f2;_T5=_T24->f3;_T21=(void*)_T5;}{void*t=_T23;unsigned j=_T22;void*t3=_T21;
# 638
if(i!=j)goto _TL9A;
# 640
t1=t;goto LOOP;_TL9A:
# 643
 if(i >= j)goto _TL9C;_T6=
# 646
Cyc_AssnDef_aggr_update(t,i,t2);_T7=j;_T8=t3;_T9=Cyc_AssnDef_aggr_update(_T6,_T7,_T8);return _T9;_TL9C: goto _LL0;}case 8: _TA=t1;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T24=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_TA;_T20=_T24->f1;_T22=_T24->f2;_T1F=_T24->f3;_TB=_T24->f4;_T23=(void*)_TB;}{int is_union=_T20;unsigned tag=_T22;struct Cyc_List_List*ts=_T1F;void*tp=_T23;_TC=is_union;
# 651
if(!_TC)goto _TL9E;_TD=is_union;_TE=i;{void*_T24[1];_T24[0]=t2;_T10=_tag_fat(_T24,sizeof(void*),1);_TF=Cyc_List_list(_T10);}_T11=tp;_T12=Cyc_AssnDef_aggr(_TD,_TE,_TF,_T11);return _T12;_TL9E: {
# 653
struct Cyc_List_List*revnewts=0;
_TLA3: if(i!=0U)goto _TLA1;else{goto _TLA2;}
_TLA1: if(ts!=0)goto _TLA4;goto _LL6;_TLA4:{struct Cyc_List_List*_T24=_cycalloc(sizeof(struct Cyc_List_List));_T14=ts;
_T24->hd=_T14->hd;_T24->tl=revnewts;_T13=(struct Cyc_List_List*)_T24;}revnewts=_T13;_T15=ts;
ts=_T15->tl;
# 654
i=i + -1;goto _TLA3;_TLA2:
# 659
 if(ts!=0)goto _TLA6;goto _LL6;_TLA6: _T16=revnewts;{struct Cyc_List_List*_T24=_cycalloc(sizeof(struct Cyc_List_List));
_T24->hd=t2;_T18=ts;_T24->tl=_T18->tl;_T17=(struct Cyc_List_List*)_T24;}{struct Cyc_List_List*newts=Cyc_List_revappend(_T16,_T17);
void*res=Cyc_AssnDef_aggr(is_union,tag,newts,tp);_T19=res;
return _T19;}}}default: _LL6: goto _LL0;}_LL0:;}{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct _T1F;_T1F.tag=10;
# 666
_T1F.f1=t1;_T1F.f2=i;_T1F.f3=t2;_T1A=_T1F;}{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct t=_T1A;_T1B=& t;_T1C=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T1B;_T1D=(void*)_T1C;_T1E=
Cyc_AssnDef_hash_cons_term(_T1D);return _T1E;}}
# 670
static int Cyc_AssnDef_address_disjoint(void*,void*);
# 673
static int Cyc_AssnDef_term_neq(void*t1,void*t2){struct _tuple15 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;int _T9;void*_TA;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TB;enum Cyc_Absyn_Primop _TC;void*_TD;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TE;enum Cyc_Absyn_Primop _TF;void*_T10;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T11;enum Cyc_Absyn_Primop _T12;void*_T13;int*_T14;int _T15;void*_T16;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T17;enum Cyc_Absyn_Primop _T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;int*_T27;int _T28;void*_T29;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T2A;enum Cyc_Absyn_Primop _T2B;void*_T2C;int*_T2D;unsigned _T2E;void*_T2F;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T30;enum Cyc_Absyn_Primop _T31;void*_T32;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T33;enum Cyc_Absyn_Primop _T34;void*_T35;int*_T36;int _T37;void*_T38;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T39;enum Cyc_Absyn_Primop _T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;int*_T41;int _T42;void*_T43;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T44;enum Cyc_Absyn_Primop _T45;void*_T46;int*_T47;int _T48;void*_T49;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T4A;enum Cyc_Absyn_Primop _T4B;void*_T4C;int*_T4D;int _T4E;void*_T4F;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T50;enum Cyc_Absyn_Primop _T51;void*_T52;void*_T53;void*_T54;void*_T55;void*_T56;int*_T57;int _T58;void*_T59;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T5A;enum Cyc_Absyn_Primop _T5B;void*_T5C;int*_T5D;int _T5E;void*_T5F;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T60;enum Cyc_Absyn_Primop _T61;void*_T62;int*_T63;int _T64;void*_T65;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T66;enum Cyc_Absyn_Primop _T67;void*_T68;void*_T69;void*_T6A;void*_T6B;int _T6C;
LOOP:{struct _tuple15 _T6D;
_T6D.f0=t1;_T6D.f1=t2;_T0=_T6D;}{struct _tuple15 _T6D=_T0;void*_T6E;void*_T6F;void*_T70;void*_T71;unsigned _T72;unsigned _T73;_T1=_T6D.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=_T6D.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 0: _T7=_T6D.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T7;_T73=_T74->f1;}_T8=_T6D.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T8;_T72=_T74->f1;}{unsigned i=_T73;unsigned j=_T72;_T9=i!=j;
return _T9;}case 4: _TA=_T6D.f1;_TB=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_TA;_TC=_TB->f1;if(_TC!=Cyc_Absyn_Plus)goto _TLAA;goto _LL7;_TLAA: goto _LLD;case 3: _TD=_T6D.f1;_TE=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_TD;_TF=_TE->f1;if(_TF!=Cyc_Absyn_Bitnot)goto _TLAC;goto _LLB;_TLAC: goto _LLD;default: goto _LLD;};case 4: _T10=_T6D.f0;_T11=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T10;_T12=_T11->f1;if(_T12!=Cyc_Absyn_Plus)goto _TLAE;_T13=_T6D.f1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=4)goto _TLB0;_T16=_T6D.f1;_T17=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T16;_T18=_T17->f1;if(_T18!=Cyc_Absyn_Plus)goto _TLB2;_T19=_T6D.f0;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T19;_T1A=_T74->f2;_T71=(void*)_T1A;_T1B=_T74->f3;_T70=(void*)_T1B;}_T1C=_T6D.f1;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T1C;_T1D=_T74->f2;_T6F=(void*)_T1D;_T1E=_T74->f3;_T6E=(void*)_T1E;}_T1F=(void*)_T71;_T20=(void*)_T6F;if(_T1F!=_T20)goto _TLB4;{void*t11=_T71;void*t12=_T70;void*t21=_T6F;void*t22=_T6E;
# 679
t1=t12;
t2=t22;goto LOOP;}_TLB4: goto _LL5;_TLB2: goto _LL5;_TLB0: _LL5: _T21=_T6D.f0;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T21;_T22=_T74->f2;_T71=(void*)_T22;_T23=_T74->f3;_T70=(void*)_T23;}_T6F=_T6D.f1;_T24=(void*)_T71;_T25=(void*)_T6F;if(_T24!=_T25)goto _TLB6;{void*t11=_T71;void*t12=_T70;void*t21=_T6F;
# 683
t1=t12;
t2=Cyc_AssnDef_zero();goto LOOP;}_TLB6: _T26=_T6D.f1;_T27=(int*)_T26;_T28=*_T27;if(_T28!=3)goto _TLB8;_T29=_T6D.f1;_T2A=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T29;_T2B=_T2A->f1;if(_T2B!=Cyc_Absyn_Bitnot)goto _TLBA;goto _LLB;_TLBA: goto _LLD;_TLB8: goto _LLD;_TLAE: _T2C=_T6D.f1;_T2D=(int*)_T2C;_T2E=*_T2D;switch(_T2E){case 4: _T2F=_T6D.f1;_T30=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T2F;_T31=_T30->f1;if(_T31!=Cyc_Absyn_Plus)goto _TLBD;goto _LL7;_TLBD: goto _LLD;case 3: _T32=_T6D.f1;_T33=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T32;_T34=_T33->f1;if(_T34!=Cyc_Absyn_Bitnot)goto _TLBF;goto _LLB;_TLBF: goto _LLD;default: goto _LLD;};default: _T35=_T6D.f1;_T36=(int*)_T35;_T37=*_T36;if(_T37!=4)goto _TLC1;_T38=_T6D.f1;_T39=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T38;_T3A=_T39->f1;if(_T3A!=Cyc_Absyn_Plus)goto _TLC3;_LL7: _T71=_T6D.f0;_T3B=_T6D.f1;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T3B;_T3C=_T74->f2;_T70=(void*)_T3C;_T3D=_T74->f3;_T6F=(void*)_T3D;}_T3E=(void*)_T71;_T3F=(void*)_T70;if(_T3E!=_T3F)goto _TLC5;{void*t11=_T71;void*t21=_T70;void*t22=_T6F;
# 687
t1=Cyc_AssnDef_zero();
t2=t22;goto LOOP;}_TLC5: _T40=_T6D.f0;_T41=(int*)_T40;_T42=*_T41;if(_T42!=3)goto _TLC7;_T43=_T6D.f0;_T44=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T43;_T45=_T44->f1;if(_T45!=Cyc_Absyn_Bitnot)goto _TLC9;goto _LL9;_TLC9: goto _LLD;_TLC7: goto _LLD;_TLC3: _T46=_T6D.f0;_T47=(int*)_T46;_T48=*_T47;if(_T48!=3)goto _TLCB;_T49=_T6D.f0;_T4A=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T49;_T4B=_T4A->f1;if(_T4B!=Cyc_Absyn_Bitnot)goto _TLCD;goto _LL9;_TLCD: goto _LLD;_TLCB: goto _LLD;_TLC1: _T4C=_T6D.f0;_T4D=(int*)_T4C;_T4E=*_T4D;if(_T4E!=3)goto _TLCF;_T4F=_T6D.f0;_T50=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T4F;_T51=_T50->f1;if(_T51!=Cyc_Absyn_Bitnot)goto _TLD1;_LL9: _T52=_T6D.f0;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T52;_T53=_T74->f2;_T71=(void*)_T53;}_T70=_T6D.f1;_T54=(void*)_T71;_T55=(void*)_T70;if(_T54!=_T55)goto _TLD3;{void*t1=_T71;void*t2=_T70;
# 690
return 1;}_TLD3: _T56=_T6D.f1;_T57=(int*)_T56;_T58=*_T57;if(_T58!=3)goto _TLD5;_T59=_T6D.f1;_T5A=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T59;_T5B=_T5A->f1;if(_T5B!=Cyc_Absyn_Bitnot)goto _TLD7;goto _LLB;_TLD7: goto _LLD;_TLD5: goto _LLD;_TLD1: _T5C=_T6D.f1;_T5D=(int*)_T5C;_T5E=*_T5D;if(_T5E!=3)goto _TLD9;_T5F=_T6D.f1;_T60=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T5F;_T61=_T60->f1;if(_T61!=Cyc_Absyn_Bitnot)goto _TLDB;goto _LLB;_TLDB: goto _LLD;_TLD9: goto _LLD;_TLCF: _T62=_T6D.f1;_T63=(int*)_T62;_T64=*_T63;if(_T64!=3)goto _TLDD;_T65=_T6D.f1;_T66=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T65;_T67=_T66->f1;if(_T67!=Cyc_Absyn_Bitnot)goto _TLDF;_LLB: _T71=_T6D.f0;_T68=_T6D.f1;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T74=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T68;_T69=_T74->f2;_T70=(void*)_T69;}_T6A=(void*)_T71;_T6B=(void*)_T70;if(_T6A!=_T6B)goto _TLE1;{void*t1=_T71;void*t2=_T70;
return 1;}_TLE1: goto _LLD;_TLDF: goto _LLD;_TLDD: _LLD: _T6C=
# 694
Cyc_AssnDef_address_disjoint(t1,t2);return _T6C;};}}
# 700
static int Cyc_AssnDef_address_disjoint(void*t1,void*t2){struct _tuple15 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;void*_T9;void*_TA;int _TB;void*_TC;int*_TD;unsigned _TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;int _T15;void*_T16;int*_T17;int _T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;int*_T20;unsigned _T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;int _T28;void*_T29;void*_T2A;void*_T2B;int*_T2C;unsigned _T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;
LOOP:
 if(t1!=t2)goto _TLE3;return 0;_TLE3:{struct _tuple15 _T32;
_T32.f0=t1;_T32.f1=t2;_T0=_T32;}{struct _tuple15 _T32=_T0;unsigned _T33;unsigned _T34;void*_T35;void*_T36;int _T37;int _T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;_T1=_T32.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 11: _T4=_T32.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 11: _T7=_T32.f0;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T7;_T3C=_T3D->f1;_T8=_T3D->f2;_T3B=(void*)_T8;}_T9=_T32.f1;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T9;_T3A=_T3D->f1;_TA=_T3D->f2;_T39=(void*)_TA;}{struct Cyc_Absyn_Vardecl*x=_T3C;void*nv1=_T3B;struct Cyc_Absyn_Vardecl*y=_T3A;void*nv2=_T39;_TB=x!=y;
# 706
return _TB;}case 12:
 return 1;case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;};case 12: _TC=_T32.f1;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 11:
 return 1;case 12: _TF=_T32.f0;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TF;_T3C=_T3D->f1;_T38=_T3D->f2;_T10=_T3D->f3;_T3B=(void*)_T10;_T11=_T3D->f4;_T3A=(void*)_T11;}_T12=_T32.f1;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T12;_T39=_T3D->f1;_T37=_T3D->f2;_T13=_T3D->f3;_T36=(void*)_T13;_T14=_T3D->f4;_T35=(void*)_T14;}{struct Cyc_Absyn_Exp*exp1=_T3C;int i1=_T38;void*tm1=_T3B;void*nv1=_T3A;struct Cyc_Absyn_Exp*exp2=_T39;int i2=_T37;void*tm2=_T36;void*nv2=_T35;
# 713
if(exp1==exp2)goto _TLE8;_T15=1;goto _TLE9;_TLE8: _T15=i1!=i2;_TLE9: return _T15;}case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;};case 13: _T16=_T32.f1;_T17=(int*)_T16;_T18=*_T17;if(_T18!=13)goto _TLEA;_T19=_T32.f0;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T19;_T1A=_T3D->f1;_T3C=(void*)_T1A;_T34=_T3D->f2;}_T1B=_T32.f1;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T1B;_T1C=_T3D->f1;_T3B=(void*)_T1C;_T33=_T3D->f2;}{void*tm1=_T3C;unsigned i=_T34;void*tm2=_T3B;unsigned j=_T33;
# 717
if(tm1!=tm2)goto _TLEC;if(i==j)goto _TLEC;return 1;_TLEC:
 t1=tm1;
t2=tm2;goto LOOP;}_TLEA: _T1D=_T32.f0;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T1D;_T1E=_T3D->f1;_T3C=(void*)_T1E;}{void*tm1=_T3C;
# 729
t1=tm1;goto LOOP;}case 14: _T1F=_T32.f1;_T20=(int*)_T1F;_T21=*_T20;switch(_T21){case 14: _T22=_T32.f0;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T22;_T23=_T3D->f1;_T3C=(void*)_T23;_T24=_T3D->f2;_T3B=(void*)_T24;}_T25=_T32.f1;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T25;_T26=_T3D->f1;_T3A=(void*)_T26;_T27=_T3D->f2;_T39=(void*)_T27;}{void*tm1=_T3C;void*i1=_T3B;void*tm2=_T3A;void*i2=_T39;
# 722
if(tm1!=tm2)goto _TLEF;_T28=Cyc_AssnDef_term_neq(i1,i2);if(!_T28)goto _TLEF;return 1;_TLEF:
 t1=tm1;
t2=tm2;goto LOOP;}case 13: goto _LLF;default: _T29=_T32.f0;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T29;_T2A=_T3D->f1;_T3C=(void*)_T2A;}{void*tm1=_T3C;
# 735
t1=tm1;goto LOOP;}};default: _T2B=_T32.f1;_T2C=(int*)_T2B;_T2D=*_T2C;switch(_T2D){case 13: _LLF: _T2E=_T32.f1;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T2E;_T2F=_T3D->f1;_T3C=(void*)_T2F;}{void*tm2=_T3C;
# 732
t2=tm2;goto LOOP;}case 14: _LL13: _T30=_T32.f1;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T3D=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T30;_T31=_T3D->f1;_T3C=(void*)_T31;}{void*tm2=_T3C;
# 738
t2=tm2;goto LOOP;}default: _LL15:
# 740
 return 0;};};}}struct _tuple16{int f0;void*f1;};
# 744
struct _tuple12 Cyc_AssnDef_split_addr(void*a){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;void*_T5;struct Cyc_List_List*_T6;struct _tuple16*_T7;unsigned _T8;struct _tuple12 _T9;
struct Cyc_List_List*fields=0;
_TLF2: if(1)goto _TLF3;else{goto _TLF4;}
_TLF3:{void*_TA;unsigned _TB;void*_TC;_T0=a;_T1=(int*)_T0;_T2=*_T1;if(_T2!=13)goto _TLF5;_T3=a;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TD=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T3;_T4=_TD->f1;_TC=(void*)_T4;_TB=_TD->f2;_T5=_TD->f3;_TA=(void*)_T5;}{void*r=_TC;unsigned i=_TB;void*tp=_TA;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_TE=_cycalloc(sizeof(struct _tuple16));_T8=i;
# 749
_TE->f0=(int)_T8;_TE->f1=tp;_T7=(struct _tuple16*)_TE;}_TD->hd=_T7;_TD->tl=fields;_T6=(struct Cyc_List_List*)_TD;}fields=_T6;
a=r;goto _TLF2;}_TLF5:{struct _tuple12 _TD;
# 753
_TD.f0=a;_TD.f1=fields;_T9=_TD;}return _T9;;}goto _TLF2;_TLF4:;}
# 758
void*Cyc_AssnDef_apply_aggr_update(void*a,struct Cyc_List_List*fields,void*v){void*_T0;struct Cyc_List_List*_T1;void*_T2;int _T3;void*_T4;int _T5;unsigned _T6;void*_T7;int _T8;unsigned _T9;void*_TA;void*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;void*_TE;void*_TF;void*_T10;
if(fields!=0)goto _TLF7;_T0=v;return _T0;_TLF7: _T1=fields;_T2=_T1->hd;{
struct _tuple16*_T11=(struct _tuple16*)_T2;void*_T12;int _T13;{struct _tuple16 _T14=*_T11;_T13=_T14.f0;_T12=_T14.f1;}{int i=_T13;void*tpopt=_T12;
void*tp=0;
if(tpopt==0)goto _TLF9;_T3=Cyc_Tcutil_is_pointer_type(tpopt);if(!_T3)goto _TLF9;
tp=Cyc_Tcutil_pointer_elt_type(tpopt);goto _TLFA;_TLF9: _TLFA: _T4=a;_T5=i;_T6=(unsigned)_T5;_T7=a;_T8=i;_T9=(unsigned)_T8;_TA=tp;_TB=
# 766
Cyc_AssnDef_proj(_T7,_T9,_TA);_TC=fields;_TD=_TC->tl;_TE=v;_TF=Cyc_AssnDef_apply_aggr_update(_TB,_TD,_TE);{
# 765
void*res=Cyc_AssnDef_aggr_update(_T4,_T6,_TF);_T10=res;
# 767
return _T10;}}}}
# 770
static void*Cyc_AssnDef_proj_fields(void*v,struct Cyc_List_List*fs){struct Cyc_List_List*_T0;void*_T1;int _T2;void*_T3;int _T4;unsigned _T5;void*_T6;void*_T7;int _T8;unsigned _T9;struct Cyc_List_List*_TA;void*_TB;
_TLFE: if(fs!=0)goto _TLFC;else{goto _TLFD;}
_TLFC: _T0=fs;_T1=_T0->hd;{struct _tuple16*_TC=(struct _tuple16*)_T1;void*_TD;int _TE;{struct _tuple16 _TF=*_TC;_TE=_TF.f0;_TD=_TF.f1;}{int i=_TE;void*tpopt=_TD;
if(tpopt==0)goto _TLFF;_T2=Cyc_Tcutil_is_pointer_type(tpopt);if(!_T2)goto _TLFF;_T3=v;_T4=i;_T5=(unsigned)_T4;_T6=
Cyc_Tcutil_pointer_elt_type(tpopt);v=Cyc_AssnDef_proj(_T3,_T5,_T6);goto _TL100;
# 776
_TLFF: _T7=v;_T8=i;_T9=(unsigned)_T8;v=Cyc_AssnDef_proj(_T7,_T9,0);_TL100:;}}_TA=fs;
# 771
fs=_TA->tl;goto _TLFE;_TLFD: _TB=v;
# 778
return _TB;}
# 783
static int Cyc_AssnDef_only_pointers(void*t){int*_T0;unsigned _T1;struct Cyc_Absyn_ArrayInfo _T2;void*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;void*_TA;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TB;union Cyc_Absyn_AggrInfo _TC;struct _union_AggrInfo_KnownAggr _TD;unsigned _TE;void*_TF;union Cyc_Absyn_AggrInfo _T10;struct _union_AggrInfo_KnownAggr _T11;struct Cyc_Absyn_Aggrdecl**_T12;struct Cyc_Absyn_Aggrdecl*_T13;struct Cyc_Absyn_AggrdeclImpl*_T14;struct Cyc_Absyn_Aggrdecl*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Aggrdecl*_T18;struct Cyc_Absyn_AggrdeclImpl*_T19;struct Cyc_Absyn_AggrdeclImpl*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Aggrfield*_T1E;void*_T1F;int _T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Aggrfield*_T24;void*_T25;int _T26;struct Cyc_List_List*_T27;
void*_T28=Cyc_Absyn_compress(t);struct Cyc_List_List*_T29;struct Cyc_Absyn_Aggrdecl*_T2A;struct Cyc_Absyn_ArrayInfo _T2B;_T0=(int*)_T28;_T1=*_T0;switch(_T1){case 4:
 return 1;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T28;_T2B=_T2C->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T2B;_T2=ai;_T3=_T2.elt_type;_T4=
Cyc_AssnDef_only_pointers(_T3);return _T4;}case 0: _T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=24)goto _TL102;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_TA=_T9->f1;_TB=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TA;_TC=_TB->f1;_TD=_TC.KnownAggr;_TE=_TD.tag;if(_TE!=2)goto _TL104;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_TF=_T2C->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T2D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TF;_T10=_T2D->f1;_T11=_T10.KnownAggr;_T12=_T11.val;{struct Cyc_Absyn_Aggrdecl*_T2E=*_T12;_T2A=_T2E;}}_T29=_T2C->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T2A;struct Cyc_List_List*ts=_T29;_T13=ad;_T14=_T13->impl;
# 788
if(_T14!=0)goto _TL106;return 0;_TL106: _T15=ad;_T16=_T15->tvs;_T17=ts;{
struct Cyc_List_List*subst=Cyc_List_zip(_T16,_T17);_T18=ad;_T19=_T18->impl;_T1A=
_check_null(_T19);{struct Cyc_List_List*fs=_T1A->fields;_TL10B: if(fs!=0)goto _TL109;else{goto _TL10A;}
_TL109: _T1B=subst;_T1C=fs;_T1D=_T1C->hd;_T1E=(struct Cyc_Absyn_Aggrfield*)_T1D;_T1F=_T1E->type;{void*t=Cyc_Tcutil_substitute(_T1B,_T1F);_T20=
Cyc_AssnDef_only_pointers(t);if(_T20)goto _TL10C;else{goto _TL10E;}_TL10E: return 0;_TL10C:;}_T21=fs;
# 790
fs=_T21->tl;goto _TL10B;_TL10A:;}
# 794
return 1;}}_TL104: goto _LL9;_TL102: goto _LL9;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T28;_T29=_T2C->f3;}{struct Cyc_List_List*fs=_T29;
# 796
_TL112: if(fs!=0)goto _TL110;else{goto _TL111;}
_TL110: _T22=fs;_T23=_T22->hd;_T24=(struct Cyc_Absyn_Aggrfield*)_T23;_T25=_T24->type;_T26=Cyc_AssnDef_only_pointers(_T25);if(_T26)goto _TL113;else{goto _TL115;}_TL115: return 0;_TL113: _T27=fs;
# 796
fs=_T27->tl;goto _TL112;_TL111:
# 798
 return 1;}default: _LL9:
 return 0;};}
# 804
static int Cyc_AssnDef_only_bits(void*t){int*_T0;unsigned _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6;void*_T7;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T8;union Cyc_Absyn_AggrInfo _T9;struct _union_AggrInfo_KnownAggr _TA;unsigned _TB;void*_TC;union Cyc_Absyn_AggrInfo _TD;struct _union_AggrInfo_KnownAggr _TE;struct Cyc_Absyn_Aggrdecl**_TF;struct Cyc_Absyn_Aggrdecl*_T10;struct Cyc_Absyn_AggrdeclImpl*_T11;struct Cyc_Absyn_Aggrdecl*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_Aggrdecl*_T15;struct Cyc_Absyn_AggrdeclImpl*_T16;struct Cyc_Absyn_AggrdeclImpl*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_Absyn_Aggrfield*_T1B;void*_T1C;int _T1D;struct Cyc_List_List*_T1E;struct Cyc_Absyn_ArrayInfo _T1F;void*_T20;int _T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Aggrfield*_T24;void*_T25;int _T26;struct Cyc_List_List*_T27;
void*_T28=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_Aggrdecl*_T2B;_T0=(int*)_T28;_T1=*_T0;switch(_T1){case 0: _T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 0: goto _LL4;case 1: _LL4: goto _LL6;case 2: _LL6: goto _LL8;case 19: _LL8: goto _LLA;case 20: _LLA: goto _LLC;case 5: _LLC: goto _LLE;case 3: _LLE:
# 812
 return 1;case 24: _T6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_T7=_T6->f1;_T8=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T7;_T9=_T8->f1;_TA=_T9.KnownAggr;_TB=_TA.tag;if(_TB!=2)goto _TL118;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T28;_TC=_T2C->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T2D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TC;_TD=_T2D->f1;_TE=_TD.KnownAggr;_TF=_TE.val;{struct Cyc_Absyn_Aggrdecl*_T2E=*_TF;_T2B=_T2E;}}_T2A=_T2C->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T2B;struct Cyc_List_List*ts=_T2A;_T10=ad;_T11=_T10->impl;
# 815
if(_T11!=0)goto _TL11A;return 0;_TL11A: _T12=ad;_T13=_T12->tvs;_T14=ts;{
struct Cyc_List_List*subst=Cyc_List_zip(_T13,_T14);_T15=ad;_T16=_T15->impl;_T17=
_check_null(_T16);{struct Cyc_List_List*fs=_T17->fields;_TL11F: if(fs!=0)goto _TL11D;else{goto _TL11E;}
_TL11D: _T18=subst;_T19=fs;_T1A=_T19->hd;_T1B=(struct Cyc_Absyn_Aggrfield*)_T1A;_T1C=_T1B->type;{void*t=Cyc_Tcutil_substitute(_T18,_T1C);_T1D=
Cyc_AssnDef_only_bits(t);if(_T1D)goto _TL120;else{goto _TL122;}_TL122: return 0;_TL120:;}_T1E=fs;
# 817
fs=_T1E->tl;goto _TL11F;_TL11E:;}
# 821
return 1;}}_TL118: goto _LL15;default: goto _LL15;};case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T28;_T29=_T2C->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T29;_T1F=ai;_T20=_T1F.elt_type;_T21=
# 813
Cyc_AssnDef_only_bits(_T20);return _T21;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T28;_T2A=_T2C->f3;}{struct Cyc_List_List*fs=_T2A;
# 823
_TL126: if(fs!=0)goto _TL124;else{goto _TL125;}
_TL124: _T22=fs;_T23=_T22->hd;_T24=(struct Cyc_Absyn_Aggrfield*)_T23;_T25=_T24->type;_T26=Cyc_AssnDef_only_bits(_T25);if(_T26)goto _TL127;else{goto _TL129;}_TL129: return 0;_TL127: _T27=fs;
# 823
fs=_T27->tl;goto _TL126;_TL125:
# 825
 return 1;}default: _LL15:
 return 0;};}
# 830
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;int _T8;int _T9;int _TA;int _TB;int _TC;int _TD;void*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_AssnDef_Select_AssnDef_Term_struct _T11;struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T12;struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T13;void*_T14;void*_T15;
LOOP:{void*_T16;void*_T17;void*_T18;_T0=t1;_T1=(int*)_T0;_T2=*_T1;if(_T2!=7)goto _TL12A;_T3=t1;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T19=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T3;_T4=_T19->f1;_T18=(void*)_T4;_T5=_T19->f2;_T17=(void*)_T5;_T6=_T19->f3;_T16=(void*)_T6;}{void*m=_T18;void*a1=_T17;void*v=_T16;
# 834
if(a1!=t2)goto _TL12C;_T7=v;return _T7;_TL12C: _T8=
Cyc_AssnDef_address_disjoint(a1,t2);if(!_T8)goto _TL12E;
t1=m;goto LOOP;_TL12E:
# 846
 if(tp==0)goto _TL130;{
void*_T19=tp;void*tp=_T19;_T9=
Cyc_AssnDef_only_pointers(tp);if(!_T9)goto _TL132;{
void*vtopt=Cyc_AssnDef_get_term_type(v);
if(vtopt==0)goto _TL134;{
void*vt=vtopt;_TA=
Cyc_AssnDef_only_bits(vt);if(!_TA)goto _TL136;
t1=m;goto LOOP;_TL136:;}goto _TL135;_TL134: _TL135: {
# 857
void*a1opt=Cyc_AssnDef_get_term_type(a1);
if(a1opt==0)goto _TL138;_TB=Cyc_Tcutil_is_pointer_type(a1opt);if(!_TB)goto _TL138;{
void*vt=Cyc_Tcutil_pointer_elt_type(a1opt);_TC=
Cyc_AssnDef_only_bits(vt);if(!_TC)goto _TL13A;
t1=m;goto LOOP;_TL13A:;}goto _TL139;_TL138: _TL139:;}}goto _TL133;_TL132: _TL133:;}goto _TL131;_TL130: _TL131: goto _LL0;}_TL12A: goto _LL0;_LL0:;}{
# 870
struct _tuple12 _T16=Cyc_AssnDef_split_addr(t2);struct Cyc_List_List*_T17;void*_T18;_T18=_T16.f0;_T17=_T16.f1;{void*root=_T18;struct Cyc_List_List*fields=_T17;
if(fields==0)goto _TL13C;{
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0)goto _TL13E;_TD=Cyc_Tcutil_is_pointer_type(topt);if(!_TD)goto _TL13E;
topt=Cyc_Tcutil_pointer_elt_type(topt);goto _TL13F;_TL13E: _TL13F: _TE=
# 876
Cyc_AssnDef_select(t1,root,topt);_TF=fields;_T10=Cyc_AssnDef_proj_fields(_TE,_TF);return _T10;}_TL13C:{struct Cyc_AssnDef_Select_AssnDef_Term_struct _T19;_T19.tag=6;
# 878
_T19.f1=t1;_T19.f2=t2;_T19.f3=tp;_T11=_T19;}{struct Cyc_AssnDef_Select_AssnDef_Term_struct t=_T11;_T12=& t;_T13=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T12;_T14=(void*)_T13;_T15=
Cyc_AssnDef_hash_cons_term(_T14);return _T15;}}}}
# 889 "assndef.cyc"
void*Cyc_AssnDef_proj(void*t1,unsigned i,void*tp){void*_T0;int*_T1;unsigned _T2;void*_T3;int _T4;unsigned _T5;int _T6;unsigned _T7;struct Cyc_List_List*_T8;unsigned _T9;int _TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;int*_T12;unsigned _T13;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T14;void*_T15;int*_T16;int _T17;void*_T18;union Cyc_Absyn_AggrInfo _T19;struct _union_AggrInfo_KnownAggr _T1A;unsigned _T1B;union Cyc_Absyn_AggrInfo _T1C;struct _union_AggrInfo_KnownAggr _T1D;struct Cyc_Absyn_Aggrdecl**_T1E;struct Cyc_Absyn_Aggrdecl*_T1F;enum Cyc_Absyn_AggrKind _T20;int _T21;union Cyc_Absyn_AggrInfo _T22;struct _union_AggrInfo_UnknownAggr _T23;struct _tuple2 _T24;enum Cyc_Absyn_AggrKind _T25;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T26;enum Cyc_Absyn_AggrKind _T27;struct Cyc_AssnDef_Proj_AssnDef_Term_struct _T28;struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T29;struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T2A;void*_T2B;void*_T2C;
LOOP:{void*_T2D;void*_T2E;unsigned _T2F;int _T30;_T0=t1;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8: _T3=t1;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T3;_T30=_T31->f1;_T2F=_T31->f2;_T2E=_T31->f3;}{int is_union=_T30;unsigned tag=_T2F;struct Cyc_List_List*ts=_T2E;
# 893
int n=Cyc_List_length(ts);_T4=is_union;
if(_T4)goto _TL141;else{goto _TL143;}_TL143: _T5=i;_T6=n;_T7=(unsigned)_T6;if(_T5 >= _T7)goto _TL141;_T8=ts;_T9=i;_TA=(int)_T9;_TB=
Cyc_List_nth(_T8,_TA);return _TB;
_TL141: if(tag!=i)goto _TL144;if(n < 1)goto _TL144;_TC=
_check_null(ts);_TD=_TC->hd;return _TD;
_TL144: goto _LL0;}case 10: _TE=t1;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_TE;_TF=_T31->f1;_T2E=(void*)_TF;_T2F=_T31->f2;_T10=_T31->f3;_T2D=(void*)_T10;}{void*t=_T2E;unsigned j=_T2F;void*t2=_T2D;
# 900
if(i!=j)goto _TL146;_T11=t2;return _T11;_TL146: {
# 903
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt!=0)goto _TL148;goto NOREDUCE;_TL148:{
void*_T31=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _T32;_T12=(int*)_T31;_T13=*_T12;switch(_T13){case 0: _T14=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T31;_T15=_T14->f1;_T16=(int*)_T15;_T17=*_T16;if(_T17!=24)goto _TL14B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T33=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T31;_T18=_T33->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T34=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T18;_T32=_T34->f1;}}{union Cyc_Absyn_AggrInfo ai=_T32;{struct Cyc_Absyn_Aggrdecl*_T33;_T19=ai;_T1A=_T19.KnownAggr;_T1B=_T1A.tag;if(_T1B!=2)goto _TL14D;_T1C=ai;_T1D=_T1C.KnownAggr;_T1E=_T1D.val;{struct Cyc_Absyn_Aggrdecl*_T34=*_T1E;_T33=_T34;}{struct Cyc_Absyn_Aggrdecl*ad=_T33;_T1F=ad;_T20=_T1F->kind;_T21=(int)_T20;
# 909
if(_T21!=1)goto _TL14F;goto NOREDUCE;_TL14F: goto _LLE;}_TL14D: _T22=ai;_T23=_T22.UnknownAggr;_T24=_T23.val;_T25=_T24.f0;if(_T25!=Cyc_Absyn_UnionA)goto _TL151;goto NOREDUCE;_TL151: goto _LLE;_LLE:;}goto _LL7;}_TL14B: goto _LLC;case 7: _T26=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T31;_T27=_T26->f1;if(_T27!=Cyc_Absyn_UnionA)goto _TL153;goto NOREDUCE;_TL153: goto _LLC;default: _LLC: goto _LL7;}_LL7:;}
# 917
t1=t;goto LOOP;}}default: goto _LL0;}_LL0:;}
# 922
NOREDUCE:{struct Cyc_AssnDef_Proj_AssnDef_Term_struct _T2D;_T2D.tag=9;
_T2D.f1=t1;_T2D.f2=i;_T2D.f3=tp;_T28=_T2D;}{struct Cyc_AssnDef_Proj_AssnDef_Term_struct t=_T28;_T29=& t;_T2A=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T29;_T2B=(void*)_T2A;_T2C=
Cyc_AssnDef_hash_cons_term(_T2B);return _T2C;}}
# 927
void*Cyc_AssnDef_aggr(int is_union,unsigned tag,struct Cyc_List_List*ts,void*tp){struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _T0;struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _T5;_T5.tag=8;
# 930
_T5.f1=is_union;_T5.f2=tag;_T5.f3=ts;_T5.f4=tp;_T0=_T5;}{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 933
void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*ts,void*tp){unsigned _T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;_T0=- 1U;_T1=ts;_T2=tp;_T3=
Cyc_AssnDef_aggr(0,_T0,_T1,_T2);return _T3;}
# 936
void*Cyc_AssnDef_datatype_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){void*_T0;_T0=
Cyc_AssnDef_aggr(0,tag,ts,tp);return _T0;}
# 939
void*Cyc_AssnDef_union_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){void*_T0;_T0=
Cyc_AssnDef_aggr(1,tag,ts,tp);return _T0;}
# 942
void*Cyc_AssnDef_tagof_tm(void*t1){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;int*_T7;unsigned _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;void*_TA;int*_TB;int _TC;void*_TD;union Cyc_Absyn_AggrInfo _TE;struct _union_AggrInfo_KnownAggr _TF;unsigned _T10;union Cyc_Absyn_AggrInfo _T11;struct _union_AggrInfo_KnownAggr _T12;struct Cyc_Absyn_Aggrdecl**_T13;struct Cyc_Absyn_Aggrdecl*_T14;enum Cyc_Absyn_AggrKind _T15;int _T16;void*_T17;union Cyc_Absyn_AggrInfo _T18;struct _union_AggrInfo_UnknownAggr _T19;struct _tuple2 _T1A;enum Cyc_Absyn_AggrKind _T1B;void*_T1C;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T1D;enum Cyc_Absyn_AggrKind _T1E;void*_T1F;struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _T20;struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T21;struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T22;void*_T23;void*_T24;{void*_T25;unsigned _T26;_T0=t1;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8: _T3=t1;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T27=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T3;_T26=_T27->f2;}{unsigned tag=_T26;_T4=
# 944
Cyc_AssnDef_uint(tag);return _T4;}case 10: _T5=t1;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T27=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T5;_T6=_T27->f1;_T25=(void*)_T6;_T26=_T27->f2;}{void*t=_T25;unsigned j=_T26;
# 947
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt!=0)goto _TL156;goto _LL0;_TL156:{
void*_T27=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _T28;_T7=(int*)_T27;_T8=*_T7;switch(_T8){case 0: _T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27;_TA=_T9->f1;_TB=(int*)_TA;_TC=*_TB;if(_TC!=24)goto _TL159;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T29=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27;_TD=_T29->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T2A=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TD;_T28=_T2A->f1;}}{union Cyc_Absyn_AggrInfo ai=_T28;{struct Cyc_Absyn_Aggrdecl*_T29;_TE=ai;_TF=_TE.KnownAggr;_T10=_TF.tag;if(_T10!=2)goto _TL15B;_T11=ai;_T12=_T11.KnownAggr;_T13=_T12.val;{struct Cyc_Absyn_Aggrdecl*_T2A=*_T13;_T29=_T2A;}{struct Cyc_Absyn_Aggrdecl*ad=_T29;_T14=ad;_T15=_T14->kind;_T16=(int)_T15;
# 953
if(_T16!=1)goto _TL15D;_T17=Cyc_AssnDef_uint(j);return _T17;_TL15D: goto _LLE;}_TL15B: _T18=ai;_T19=_T18.UnknownAggr;_T1A=_T19.val;_T1B=_T1A.f0;if(_T1B!=Cyc_Absyn_UnionA)goto _TL15F;_T1C=
# 955
Cyc_AssnDef_uint(j);return _T1C;_TL15F: goto _LLE;_LLE:;}goto _LL7;}_TL159: goto _LLC;case 7: _T1D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T27;_T1E=_T1D->f1;if(_T1E!=Cyc_Absyn_UnionA)goto _TL161;_T1F=
# 959
Cyc_AssnDef_uint(j);return _T1F;_TL161: goto _LLC;default: _LLC: goto _LL7;}_LL7:;}goto _LL0;}default: goto _LL0;}_LL0:;}{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _T25;_T25.tag=15;
# 965
_T25.f1=t1;_T20=_T25;}{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct t=_T20;_T21=& t;_T22=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T21;_T23=(void*)_T22;_T24=
Cyc_AssnDef_hash_cons_term(_T23);return _T24;}}
# 969
void*Cyc_AssnDef_cast(void*tp,void*tm){struct Cyc_AssnDef_Cast_AssnDef_Term_struct _T0;struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct _T5;_T5.tag=5;
_T5.f1=tp;_T5.f2=tm;_T0=_T5;}{struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 974
void*Cyc_AssnDef_raw_alloc(struct Cyc_Absyn_Exp*e,int id,void*nelts,void*invok,void*tp){struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T0;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T5;_T5.tag=12;
_T5.f1=e;_T5.f2=id;_T5.f3=nelts;_T5.f4=invok;_T5.f5=tp;_T0=_T5;}{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 979
void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*e,void*nelts,void*invok,void*tp){struct Cyc_Absyn_Exp*_T0;int _T1;int _T2;void*_T3;void*_T4;void*_T5;void*_T6;
static int counter=0;_T0=e;_T1=counter;
counter=_T1 + 1;_T2=_T1;_T3=nelts;_T4=invok;_T5=tp;_T6=Cyc_AssnDef_raw_alloc(_T0,_T2,_T3,_T4,_T5);return _T6;}
# 984
void*Cyc_AssnDef_offsetf(void*tm,unsigned i,void*tp){struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct _T0;struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T1;struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct _T5;_T5.tag=13;
_T5.f1=tm;_T5.f2=i;_T5.f3=tp;_T0=_T5;}{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct t=_T0;_T1=& t;_T2=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_term(_T3);return _T4;}}
# 989
void*Cyc_AssnDef_offseti(void*tm,void*i,void*tp){struct _tuple15 _T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T5;unsigned _T6;void*_T7;void*_T8;int*_T9;int _TA;void*_TB;int*_TC;int _TD;void*_TE;void*_TF;void*_T10;void*_T11;int _T12;void*_T13;void*_T14;void*_T15;void*_T16;struct Cyc_AssnDef_Offseti_AssnDef_Term_struct _T17;struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T18;struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T19;void*_T1A;void*_T1B;{struct _tuple15 _T1C;
_T1C.f0=tm;_T1C.f1=i;_T0=_T1C;}{struct _tuple15 _T1C=_T0;void*_T1D;void*_T1E;void*_T1F;_T1=_T1C.f1;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL163;_T4=_T1C.f1;_T5=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T4;_T6=_T5->f1;if(_T6!=0)goto _TL165;_T7=tm;
return _T7;_TL165: _T8=_T1C.f0;_T9=(int*)_T8;_TA=*_T9;if(_TA!=14)goto _TL167;goto _LL3;_TL167: goto _LL5;_TL163: _TB=_T1C.f0;_TC=(int*)_TB;_TD=*_TC;if(_TD!=14)goto _TL169;_LL3: _TE=_T1C.f0;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T20=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_TE;_TF=_T20->f1;_T1F=(void*)_TF;_T10=_T20->f2;_T1E=(void*)_T10;_T11=_T20->f3;_T1D=(void*)_T11;}{void*t=_T1F;void*j=_T1E;void*tp2=_T1D;
# 993
if(tp==0)goto _TL16B;if(tp2==0)goto _TL16B;_T12=Cyc_Unify_unify(tp,tp2);if(!_T12)goto _TL16B;_T13=t;_T14=
Cyc_AssnDef_plus(i,j,Cyc_Absyn_sint_type);_T15=tp;_T16=Cyc_AssnDef_offseti(_T13,_T14,_T15);return _T16;_TL16B: goto _LL6;}_TL169: _LL5: _LL6:{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct _T20;_T20.tag=14;
# 997
_T20.f1=tm;_T20.f2=i;_T20.f3=tp;_T17=_T20;}{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct t=_T17;_T18=& t;_T19=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T18;_T1A=(void*)_T19;_T1B=
Cyc_AssnDef_hash_cons_term(_T1A);return _T1B;};}}
# 1001
void*Cyc_AssnDef_fresh_var(struct Cyc_Absyn_Vardecl*vd,void*tp){void*_T0;
static int counter=0;
void*t=Cyc_AssnDef_logicvar(vd,counter,tp);
counter=counter + 1;_T0=t;
return _T0;}
# 1008
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){void*_T0;_T0=
Cyc_AssnDef_binop(0U,t1,t2,tp);return _T0;}
# 1011
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){void*_T0;_T0=
Cyc_AssnDef_binop(2U,t1,t2,tp);return _T0;}
# 1015
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){enum Cyc_Absyn_Primop _T0;int _T1;void*_T2;enum Cyc_Absyn_Primop _T3;int _T4;void*_T5;unsigned _T6;void*_T7;void*_T8;void*_T9;struct Cyc_AssnDef_Unop_AssnDef_Term_struct _TA;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TB;struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TC;void*_TD;void*_TE;_T0=p;_T1=(int)_T0;
# 1017
if(_T1!=0)goto _TL16D;_T2=t;return _T2;_TL16D: _T3=p;_T4=(int)_T3;
# 1019
if(_T4!=2)goto _TL16F;_T5=t;_T6=- 1U;_T7=Cyc_AssnDef_uint(_T6);_T8=tp;_T9=Cyc_AssnDef_binop(1U,_T5,_T7,_T8);return _T9;_TL16F:{struct Cyc_AssnDef_Unop_AssnDef_Term_struct _TF;_TF.tag=3;
_TF.f1=p;_TF.f2=t;_TF.f3=tp;_TA=_TF;}{struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=_TA;_TB=& ptr;_TC=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_TB;_TD=(void*)_TC;_TE=
Cyc_AssnDef_hash_cons_term(_TD);return _TE;}}
# 1024
void*Cyc_AssnDef_numelts_term(void*t){void*_T0;_T0=
Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);return _T0;}static char _TmpG3[5U]="#Mem";
# 1030
static struct _fat_ptr Cyc_AssnDef_memory_var={(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 5U};static char _TmpG4[9U]="#Pre_Mem";
static struct _fat_ptr Cyc_AssnDef_pre_memory_var={(unsigned char*)_TmpG4,(unsigned char*)_TmpG4,(unsigned char*)_TmpG4 + 9U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_AssnDef_memory_var};
static struct _tuple0 Cyc_AssnDef_pre_memvar={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_AssnDef_pre_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)& Cyc_AssnDef_void_con),0};
# 1037
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)& Cyc_AssnDef_vd_type),0,0,0,0,0,0};
# 1050
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=(struct Cyc_Absyn_Vardecl*)& Cyc_AssnDef_memory_struct;
struct Cyc_Absyn_Vardecl Cyc_AssnDef_pre_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_pre_memvar,0U,{0,0,0,0,0U},(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)& Cyc_AssnDef_vd_type),0,0,0,0,0,0};
# 1064
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_pre_memory=(struct Cyc_Absyn_Vardecl*)& Cyc_AssnDef_pre_memory_struct;static char _TmpG5[5U]="#Exn";
# 1066
static struct _fat_ptr Cyc_AssnDef_exception_var={(unsigned char*)_TmpG5,(unsigned char*)_TmpG5,(unsigned char*)_TmpG5 + 5U};
static struct _tuple0 Cyc_AssnDef_exnvar={.f0={.Abs_n={1,0}},.f1=(struct _fat_ptr*)& Cyc_AssnDef_exception_var};
# 1069
struct Cyc_Absyn_Vardecl Cyc_AssnDef_exception_struct={Cyc_Absyn_Public,& Cyc_AssnDef_exnvar,0U,{0,0,0,0,0U},(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)& Cyc_AssnDef_vd_type),0,0,0,0,0,0};
# 1083
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void){struct Cyc_Absyn_Vardecl*_T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_Absyn_Vardecl*_T2;_T0=& Cyc_AssnDef_exception_struct;{
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T0;_T1=vd;
_T1->type=Cyc_Absyn_exn_type();_T2=vd;
return _T2;}}struct _tuple17{struct Cyc_List_List*f0;int f1;};
# 1100
struct _tuple17 Cyc_AssnDef_get_coefficient_term_list(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;unsigned _T4;void*_T5;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T6;enum Cyc_Absyn_Primop _T7;void*_T8;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;void*_TF;unsigned _T10;void*_T11;void*_T12;void*_T13;void*_T14;int*_T15;int _T16;void*_T17;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T18;enum Cyc_Absyn_Primop _T19;int _T1A;void*_T1B;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T1C;void*_T1D;int*_T1E;int _T1F;void*_T20;void*_T21;void*_T22;struct Cyc_List_List*_T23;struct _tuple16*_T24;unsigned _T25;int(*_T26)(struct _fat_ptr,struct _fat_ptr);void*(*_T27)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T28;struct _fat_ptr _T29;struct Cyc_List_List*_T2A;struct _tuple16*_T2B;void*_T2C;int*_T2D;int _T2E;void*_T2F;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T30;enum Cyc_Absyn_Primop _T31;void*_T32;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T33;void*_T34;int*_T35;int _T36;void*_T37;void*_T38;void*_T39;struct Cyc_List_List*_T3A;struct _tuple16*_T3B;unsigned _T3C;struct Cyc_List_List*_T3D;struct _tuple16*_T3E;struct _tuple17 _T3F;
struct Cyc_List_List*coefficient_term_list=0;
int res=0;
LOOP:{void*_T40;void*_T41;unsigned _T42;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T3;_T42=_T43->f1;}{unsigned i=_T42;_T4=i;
# 1105
res=(int)_T4;goto _LL0;}case 4: _T5=t;_T6=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T5;_T7=_T6->f1;if(_T7!=Cyc_Absyn_Plus)goto _TL172;_T8=t;_T9=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T8;_TA=_T9->f3;_TB=(int*)_TA;_TC=*_TB;if(_TC!=0)goto _TL174;_TD=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_TD;_TE=_T43->f2;_T41=(void*)_TE;_TF=_T43->f3;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TF;_T42=_T44->f1;}}{void*t1=_T41;unsigned i=_T42;_T10=i;
# 1107
res=(int)_T10;
t=t1;goto LOOP;}_TL174: _T11=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T11;_T12=_T43->f2;_T41=(void*)_T12;_T13=_T43->f3;_T40=(void*)_T13;}{void*t1=_T41;void*t2=_T40;{unsigned _T43;void*_T44;_T14=t2;_T15=(int*)_T14;_T16=*_T15;if(_T16!=4)goto _TL176;_T17=t2;_T18=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T17;_T19=_T18->f1;_T1A=(int)_T19;switch(_T1A){case Cyc_Absyn_Times: _T1B=t2;_T1C=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T1B;_T1D=_T1C->f3;_T1E=(int*)_T1D;_T1F=*_T1E;if(_T1F!=0)goto _TL179;_T20=t2;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T45=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T20;_T21=_T45->f2;_T44=(void*)_T21;_T22=_T45->f3;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T46=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T22;_T43=_T46->f1;}}{void*t21=_T44;unsigned c2=_T43;{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_T46=_cycalloc(sizeof(struct _tuple16));_T25=c2;
# 1113
_T46->f0=(int)_T25;_T46->f1=t21;_T24=(struct _tuple16*)_T46;}_T45->hd=_T24;_T45->tl=coefficient_term_list;_T23=(struct Cyc_List_List*)_T45;}coefficient_term_list=_T23;goto _LL9;}_TL179: goto _LLE;case Cyc_Absyn_Plus: _T27=Cyc_Warn_impos;{
# 1116
int(*_T45)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T27;_T26=_T45;}_T28=_tag_fat("Plus in the right branch, not a normal form",sizeof(char),44U);_T29=_tag_fat(0U,sizeof(void*),0);_T26(_T28,_T29);goto _LL9;default: goto _LLE;}goto _TL177;_TL176: _LLE:{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_T46=_cycalloc(sizeof(struct _tuple16));
# 1119
_T46->f0=1;_T46->f1=t2;_T2B=(struct _tuple16*)_T46;}_T45->hd=_T2B;_T45->tl=coefficient_term_list;_T2A=(struct Cyc_List_List*)_T45;}coefficient_term_list=_T2A;goto _LL9;_TL177: _LL9:;}
# 1122
t=t1;goto LOOP;}_TL172: goto _LL7;default: _LL7:{unsigned _T43;void*_T44;_T2C=t;_T2D=(int*)_T2C;_T2E=*_T2D;if(_T2E!=4)goto _TL17B;_T2F=t;_T30=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T2F;_T31=_T30->f1;if(_T31!=Cyc_Absyn_Times)goto _TL17D;_T32=t;_T33=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T32;_T34=_T33->f3;_T35=(int*)_T34;_T36=*_T35;if(_T36!=0)goto _TL17F;_T37=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T45=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T37;_T38=_T45->f2;_T44=(void*)_T38;_T39=_T45->f3;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T46=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T39;_T43=_T46->f1;}}{void*t21=_T44;unsigned c2=_T43;{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_T46=_cycalloc(sizeof(struct _tuple16));_T3C=c2;
# 1127
_T46->f0=(int)_T3C;_T46->f1=t21;_T3B=(struct _tuple16*)_T46;}_T45->hd=_T3B;_T45->tl=coefficient_term_list;_T3A=(struct Cyc_List_List*)_T45;}coefficient_term_list=_T3A;goto _LL10;}_TL17F: goto _LL13;_TL17D: goto _LL13;_TL17B: _LL13:{struct Cyc_List_List*_T45=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_T46=_cycalloc(sizeof(struct _tuple16));
# 1130
_T46->f0=1;_T46->f1=t;_T3E=(struct _tuple16*)_T46;}_T45->hd=_T3E;_T45->tl=coefficient_term_list;_T3D=(struct Cyc_List_List*)_T45;}coefficient_term_list=_T3D;goto _LL10;_LL10:;}goto _LL0;}_LL0:;}{struct _tuple17 _T40;
# 1135
_T40.f0=coefficient_term_list;_T40.f1=res;_T3F=_T40;}return _T3F;}
# 1145 "assndef.cyc"
static struct Cyc_List_List*Cyc_AssnDef_imp_merge_coefficient_term_list(int(*less_eq)(void*,void*),struct Cyc_List_List*termlist1,struct Cyc_List_List*termlist2){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple16*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct _tuple16*_T8;void*_T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple16*_TC;void*_TD;struct Cyc_List_List*_TE;void*_TF;struct _tuple16*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;void*_T19;struct _tuple16*_T1A;int _T1B;struct Cyc_List_List*_T1C;void*_T1D;struct _tuple16*_T1E;int _T1F;struct Cyc_List_List*_T20;void*_T21;struct _tuple16*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;
# 1149
struct Cyc_List_List*res_tail=0;
struct Cyc_List_List*res=0;
if(termlist1!=0)goto _TL181;_T0=termlist2;return _T0;_TL181:
 if(termlist2!=0)goto _TL183;_T1=termlist1;return _T1;_TL183: _T2=termlist1;_T3=_T2->hd;_T4=(struct _tuple16*)_T3;_T5=_T4->f1;_T6=termlist2;_T7=_T6->hd;_T8=(struct _tuple16*)_T7;_T9=_T8->f1;{
int cmp_result=less_eq(_T5,_T9);
_TL185: if(termlist1!=0)goto _TL188;else{goto _TL187;}_TL188: if(termlist2!=0)goto _TL186;else{goto _TL187;}
_TL186: _TA=termlist1;_TB=_TA->hd;_TC=(struct _tuple16*)_TB;_TD=_TC->f1;_TE=termlist2;_TF=_TE->hd;_T10=(struct _tuple16*)_TF;_T11=_T10->f1;{int cmp_result=less_eq(_TD,_T11);
if(cmp_result >= 0)goto _TL189;
if(res_tail!=0)goto _TL18B;
res_tail=termlist1;_T12=termlist1;
termlist1=_T12->tl;
res=res_tail;goto _TL18C;
# 1162
_TL18B: _T13=res_tail;_T13->tl=termlist1;
res_tail=termlist1;_T14=termlist1;
termlist1=_T14->tl;_TL18C: goto _TL18A;
# 1166
_TL189: if(cmp_result <= 0)goto _TL18D;
if(res_tail!=0)goto _TL18F;
res_tail=termlist2;_T15=termlist2;
termlist2=_T15->tl;
res=res_tail;goto _TL190;
# 1172
_TL18F: _T16=res_tail;_T16->tl=termlist2;
res_tail=termlist2;_T17=termlist2;
termlist2=_T17->tl;_TL190: goto _TL18E;
# 1178
_TL18D: _T18=termlist1;_T19=_T18->hd;_T1A=(struct _tuple16*)_T19;_T1B=_T1A->f0;_T1C=termlist2;_T1D=_T1C->hd;_T1E=(struct _tuple16*)_T1D;_T1F=_T1E->f0;{int i=_T1B + _T1F;
if(i==0)goto _TL191;_T20=termlist1;_T21=_T20->hd;_T22=(struct _tuple16*)_T21;
_T22->f0=i;
if(res_tail!=0)goto _TL193;
res_tail=termlist1;
res=res_tail;goto _TL194;
# 1185
_TL193: _T23=res_tail;_T23->tl=termlist1;
res_tail=termlist1;_TL194: goto _TL192;_TL191: _TL192: _T24=termlist1;
# 1189
termlist1=_T24->tl;_T25=termlist2;
termlist2=_T25->tl;}_TL18E: _TL18A:;}goto _TL185;_TL187:
# 1193
 if(res_tail!=0)goto _TL195;
if(termlist1!=0)goto _TL197;_T26=termlist2;goto _TL198;_TL197: _T26=termlist1;_TL198: res_tail=_T26;
res=res_tail;goto _TL196;
# 1197
_TL195: _T27=res_tail;if(termlist1!=0)goto _TL199;_T28=termlist2;goto _TL19A;_TL199: _T28=termlist1;_TL19A: _T27->tl=_T28;_TL196: _T29=res;
# 1199
return _T29;}}
# 1205
inline static unsigned Cyc_AssnDef_id_of_primop(enum Cyc_Absyn_Primop p){enum Cyc_Absyn_Primop _T0;int _T1;struct Cyc_String_pa_PrintArg_struct _T2;int(*_T3)(struct _fat_ptr,struct _fat_ptr);void*(*_T4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5;struct _fat_ptr _T6;_T0=p;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Plus:
# 1207
 return 0U;case Cyc_Absyn_Times:
 return 1U;case Cyc_Absyn_Bitand:
 return 4294967295U;case Cyc_Absyn_Bitor:
 return 0U;case Cyc_Absyn_Bitxor:
 return 0U;default:{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;
_T7.f1=Cyc_Absynpp_prim2string(p);_T2=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T2;void*_T8[1];_T8[0]=& _T7;_T4=Cyc_Warn_impos;{int(*_T9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4;_T3=_T9;}_T5=_tag_fat("no id for primop %s",sizeof(char),20U);_T6=_tag_fat(_T8,sizeof(void*),1);_T3(_T5,_T6);}};}
# 1219
static void*Cyc_AssnDef_termlist2term(struct Cyc_List_List*termlist,int newc,enum Cyc_Absyn_Primop p,void*tp){int _T0;unsigned _T1;void*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T5;struct Cyc_List_List*_T6;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T7;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T8;void*_T9;struct Cyc_List_List*_TA;unsigned _TB;int _TC;unsigned _TD;void*_TE;struct Cyc_AssnDef_Binop_AssnDef_Term_struct _TF;int _T10;unsigned _T11;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T12;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T13;void*_T14;void*_T15;
unsigned id=Cyc_AssnDef_id_of_primop(p);
if(termlist!=0)goto _TL19C;_T0=newc;_T1=(unsigned)_T0;_T2=Cyc_AssnDef_uint(_T1);return _T2;_TL19C: _T3=termlist;{
void*t=_T3->hd;_T4=termlist;
termlist=_T4->tl;
_TL19E: if(termlist!=0)goto _TL19F;else{goto _TL1A0;}
_TL19F:{struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T16;_T16.tag=4;_T16.f1=p;_T16.f2=t;_T6=termlist;_T16.f3=_T6->hd;_T16.f4=tp;_T5=_T16;}{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=_T5;_T7=& newterm;_T8=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T7;_T9=(void*)_T8;
t=Cyc_AssnDef_hash_cons_term(_T9);_TA=termlist;
termlist=_TA->tl;}goto _TL19E;_TL1A0: _TB=id;_TC=newc;_TD=(unsigned)_TC;
# 1229
if(_TB!=_TD)goto _TL1A1;_TE=t;return _TE;
# 1243 "assndef.cyc"
_TL1A1:{struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T16;_T16.tag=4;_T16.f1=p;_T16.f2=t;_T10=newc;_T11=(unsigned)_T10;_T16.f3=Cyc_AssnDef_uint(_T11);_T16.f4=tp;_TF=_T16;}{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=_TF;_T12=& newterm;_T13=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T12;_T14=(void*)_T13;_T15=
Cyc_AssnDef_hash_cons_term(_T14);return _T15;}}}
# 1250
static void*Cyc_AssnDef_compute_factor(void*tp,struct _tuple16*coefficient_term){struct _tuple16*_T0;void*_T1;struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T2;int _T3;unsigned _T4;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T5;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T6;void*_T7;void*_T8;void*_T9;int _TA;_T0=coefficient_term;{struct _tuple16 _TB=*_T0;_TA=_TB.f0;_T9=_TB.f1;}{int c=_TA;void*t=_T9;
# 1252
if(c!=1)goto _TL1A3;_T1=t;return _T1;
# 1254
_TL1A3:{struct Cyc_AssnDef_Binop_AssnDef_Term_struct _TB;_TB.tag=4;_TB.f1=1U;_TB.f2=t;_T3=c;_T4=(unsigned)_T3;_TB.f3=Cyc_AssnDef_uint(_T4);_TB.f4=tp;_T2=_TB;}{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newfactor=_T2;_T5=& newfactor;_T6=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T5;_T7=(void*)_T6;_T8=
Cyc_AssnDef_hash_cons_term(_T7);return _T8;}}}
# 1259
static int Cyc_AssnDef_inv_termcmp(void*t1,void*t2){int _T0;int _T1;_T0=
Cyc_AssnDef_termcmp(t1,t2);_T1=- _T0;return _T1;}
# 1270 "assndef.cyc"
static void*Cyc_AssnDef_normalize_plus(void*t1,void*t2,void*tp){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*(*_T2)(void*(*)(void*,struct _tuple16*),void*,struct Cyc_List_List*);struct Cyc_List_List*(*_T3)(void*(*)(void*,void*),void*,struct Cyc_List_List*);void*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;int _T7;void*_T8;void*_T9;
struct _tuple17 _TA=Cyc_AssnDef_get_coefficient_term_list(t1);int _TB;struct Cyc_List_List*_TC;_TC=_TA.f0;_TB=_TA.f1;{struct Cyc_List_List*coefficient_term_list1=_TC;int i1=_TB;
struct _tuple17 _TD=Cyc_AssnDef_get_coefficient_term_list(t2);int _TE;struct Cyc_List_List*_TF;_TF=_TD.f0;_TE=_TD.f1;{struct Cyc_List_List*coefficient_term_list2=_TF;int i2=_TE;_T0=coefficient_term_list1;_T1=coefficient_term_list2;{
struct Cyc_List_List*coefficient_term_list=
Cyc_AssnDef_imp_merge_coefficient_term_list(Cyc_AssnDef_inv_termcmp,_T0,_T1);_T3=Cyc_List_map_c;{
# 1278
struct Cyc_List_List*(*_T10)(void*(*)(void*,struct _tuple16*),void*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(void*,struct _tuple16*),void*,struct Cyc_List_List*))_T3;_T2=_T10;}_T4=tp;_T5=coefficient_term_list;{struct Cyc_List_List*term_list=_T2(Cyc_AssnDef_compute_factor,_T4,_T5);_T6=term_list;_T7=i1 + i2;_T8=tp;{
void*res=Cyc_AssnDef_termlist2term(_T6,_T7,0U,_T8);_T9=res;
return _T9;}}}}}}
# 1288
static struct _tuple17 Cyc_AssnDef_term2termlist(enum Cyc_Absyn_Primop p,void*t){unsigned _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;unsigned _T5;void*_T6;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T7;void*_T8;int*_T9;int _TA;void*_TB;void*_TC;void*_TD;enum Cyc_Absyn_Primop _TE;int _TF;enum Cyc_Absyn_Primop _T10;int _T11;unsigned _T12;void*_T13;void*_T14;void*_T15;enum Cyc_Absyn_Primop _T16;int _T17;enum Cyc_Absyn_Primop _T18;int _T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct _tuple17 _T1C;
struct Cyc_List_List*termlist=0;_T0=
Cyc_AssnDef_id_of_primop(p);{int res=(int)_T0;
LOOP:{void*_T1D;void*_T1E;enum Cyc_Absyn_Primop _T1F;unsigned _T20;_T1=t;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T21=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T4;_T20=_T21->f1;}{unsigned i=_T20;_T5=i;
# 1293
res=(int)_T5;goto _LL0;}case 4: _T6=t;_T7=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T6;_T8=_T7->f3;_T9=(int*)_T8;_TA=*_T9;if(_TA!=0)goto _TL1A6;_TB=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T21=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_TB;_T1F=_T21->f1;_TC=_T21->f2;_T1E=(void*)_TC;_TD=_T21->f3;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T22=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TD;_T20=_T22->f1;}}_TE=(enum Cyc_Absyn_Primop)_T1F;_TF=(int)_TE;_T10=p;_T11=(int)_T10;if(_TF!=_T11)goto _TL1A8;{enum Cyc_Absyn_Primop p1=_T1F;void*t1=_T1E;unsigned i=_T20;_T12=i;
# 1295
res=(int)_T12;
t=t1;goto LOOP;}_TL1A8: goto _LL5;_TL1A6: _LL5: _T13=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T21=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T13;_T1F=_T21->f1;_T14=_T21->f2;_T1E=(void*)_T14;_T15=_T21->f3;_T1D=(void*)_T15;}_T16=(enum Cyc_Absyn_Primop)_T1F;_T17=(int)_T16;_T18=p;_T19=(int)_T18;if(_T17!=_T19)goto _TL1AA;{enum Cyc_Absyn_Primop p1=_T1F;void*t1=_T1E;void*t2=_T1D;{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
# 1299
_T21->hd=t2;_T21->tl=termlist;_T1A=(struct Cyc_List_List*)_T21;}termlist=_T1A;
t=t1;goto LOOP;}_TL1AA: goto _LL7;default: _LL7:{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
# 1303
_T21->hd=t;_T21->tl=termlist;_T1B=(struct Cyc_List_List*)_T21;}termlist=_T1B;goto _LL0;}_LL0:;}{struct _tuple17 _T1D;
# 1306
_T1D.f0=termlist;_T1D.f1=res;_T1C=_T1D;}return _T1C;}}
# 1310
inline static unsigned Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){enum Cyc_Absyn_Primop _T0;int _T1;unsigned _T2;unsigned _T3;unsigned _T4;unsigned _T5;unsigned _T6;int(*_T7)(struct _fat_ptr,struct _fat_ptr);void*(*_T8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T9;struct _fat_ptr _TA;_T0=p;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Plus: _T2=i + j;
# 1312
return _T2;case Cyc_Absyn_Times: _T3=i * j;
return _T3;case Cyc_Absyn_Bitand: _T4=i & j;
return _T4;case Cyc_Absyn_Bitor: _T5=i | j;
return _T5;case Cyc_Absyn_Bitxor: _T6=i ^ j;
return _T6;default: _T8=Cyc_Warn_impos;{
int(*_TB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T8;_T7=_TB;}_T9=_tag_fat("Error in evaluating binop",sizeof(char),26U);_TA=_tag_fat(0U,sizeof(void*),0);_T7(_T9,_TA);};}
# 1322
static void*Cyc_AssnDef_normalize_assoc_commu(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;enum Cyc_Absyn_Primop _T2;int _T3;unsigned _T4;int _T5;unsigned _T6;struct Cyc_List_List*_T7;unsigned _T8;int _T9;enum Cyc_Absyn_Primop _TA;void*_TB;void*_TC;
struct _tuple17 _TD=Cyc_AssnDef_term2termlist(p,t1);int _TE;struct Cyc_List_List*_TF;_TF=_TD.f0;_TE=_TD.f1;{struct Cyc_List_List*term_list1=_TF;int c1=_TE;
struct _tuple17 _T10=Cyc_AssnDef_term2termlist(p,t2);int _T11;struct Cyc_List_List*_T12;_T12=_T10.f0;_T11=_T10.f1;{struct Cyc_List_List*term_list2=_T12;int c2=_T11;_T0=term_list1;_T1=term_list2;{
struct Cyc_List_List*term_list=Cyc_List_imp_merge(Cyc_AssnDef_inv_termcmp,_T0,_T1);_T2=p;_T3=c1;_T4=(unsigned)_T3;_T5=c2;_T6=(unsigned)_T5;{
unsigned c=Cyc_AssnDef_eval_binop(_T2,_T4,_T6);_T7=term_list;_T8=c;_T9=(int)_T8;_TA=p;_TB=tp;_TC=
Cyc_AssnDef_termlist2term(_T7,_T9,_TA,_TB);return _TC;}}}}}
# 1330
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p,void*tp){enum Cyc_Absyn_Primop _T0;int _T1;int*_T2;int _T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;int _T7;_T0=p;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Plus: goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1334
 if(tp==0)goto _TL1AE;{
void*_T8=Cyc_Absyn_compress(tp);_T2=(int*)_T8;_T3=*_T2;if(_T3!=0)goto _TL1B0;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=2)goto _TL1B2;
return 1;_TL1B2: goto _LLE;_TL1B0: _LLE:
 return 0;;}goto _TL1AF;_TL1AE: _TL1AF:
# 1340
 return 0;case Cyc_Absyn_Eq: goto _LL8;case Cyc_Absyn_Neq: _LL8:
# 1342
 return 1;default:
 return 0;};}
# 1347
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p,void*tp){enum Cyc_Absyn_Primop _T0;int _T1;int*_T2;int _T3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4;void*_T5;int*_T6;int _T7;_T0=p;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Plus: goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1351
 if(tp==0)goto _TL1B5;{
void*_T8=Cyc_Absyn_compress(tp);_T2=(int*)_T8;_T3=*_T2;if(_T3!=0)goto _TL1B7;_T4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=1)goto _TL1B9;
# 1354
return 1;_TL1B9: goto _LL10;_TL1B7: _LL10:
 return 0;;}goto _TL1B6;_TL1B5: _TL1B6:
# 1358
 return 0;case Cyc_Absyn_Bitand: goto _LL8;case Cyc_Absyn_Bitor: _LL8: goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
# 1361
 return 1;default:
 return 0;};}struct _tuple18{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 1367
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){struct _tuple18 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;enum Cyc_Absyn_Primop _T9;int _TA;unsigned _TB;void*_TC;unsigned _TD;void*_TE;unsigned _TF;void*_T10;unsigned _T11;void*_T12;unsigned _T13;void*_T14;unsigned _T15;void*_T16;unsigned _T17;void*_T18;unsigned _T19;int _T1A;unsigned _T1B;int _T1C;int _T1D;unsigned _T1E;void*_T1F;unsigned _T20;void*_T21;unsigned _T22;int _T23;unsigned _T24;int _T25;int _T26;unsigned _T27;void*_T28;unsigned _T29;void*_T2A;unsigned _T2B;void*_T2C;enum Cyc_Absyn_Primop _T2D;enum Cyc_Absyn_Primop _T2E;void*_T2F;int*_T30;int _T31;void*_T32;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T33;void*_T34;int*_T35;int _T36;int _T37;void*_T38;void*_T39;unsigned _T3A;void*_T3B;void*_T3C;void*_T3D;int _T3E;enum Cyc_Absyn_Primop _T3F;int _T40;void*_T41;void*_T42;int _T43;int _T44;enum Cyc_Absyn_Primop _T45;int _T46;struct _tuple15 _T47;void*_T48;int*_T49;int _T4A;void*_T4B;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T4C;enum Cyc_Absyn_Primop _T4D;void*_T4E;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T4F;void*_T50;int*_T51;int _T52;void*_T53;int*_T54;int _T55;void*_T56;void*_T57;void*_T58;void*_T59;void*_T5A;int _T5B;void*_T5C;unsigned _T5D;void*_T5E;void*_T5F;void*_T60;struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T61;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T62;struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T63;void*_T64;void*_T65;{struct _tuple18 _T66;
_T66.f0=p;_T66.f1=t1;_T66.f2=t2;_T0=_T66;}{struct _tuple18 _T66=_T0;void*_T67;void*_T68;unsigned _T69;unsigned _T6A;_T1=_T66.f1;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL1BB;_T4=_T66.f2;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL1BD;_T7=_T66.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T6B=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T7;_T6A=_T6B->f1;}_T8=_T66.f2;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T6B=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T8;_T69=_T6B->f1;}{unsigned i=_T6A;unsigned j=_T69;_T9=p;_TA=(int)_T9;switch(_TA){case Cyc_Absyn_Plus: _TB=i + j;_TC=
# 1372
Cyc_AssnDef_uint(_TB);return _TC;case Cyc_Absyn_Minus: _TD=i - j;_TE=
Cyc_AssnDef_uint(_TD);return _TE;case Cyc_Absyn_Bitand: _TF=i & j;_T10=
Cyc_AssnDef_uint(_TF);return _T10;case Cyc_Absyn_Bitor: _T11=i | j;_T12=
Cyc_AssnDef_uint(_T11);return _T12;case Cyc_Absyn_Bitxor: _T13=i ^ j;_T14=
Cyc_AssnDef_uint(_T13);return _T14;case Cyc_Absyn_Bitlshift: _T15=i << j;_T16=
Cyc_AssnDef_uint(_T15);return _T16;case Cyc_Absyn_Bitlrshift: _T17=i >> j;_T18=
Cyc_AssnDef_uint(_T17);return _T18;case Cyc_Absyn_Div:
 if(j==0U)goto _TL1C0;_T19=i;_T1A=(int)_T19;_T1B=j;_T1C=(int)_T1B;_T1D=_T1A / _T1C;_T1E=(unsigned)_T1D;_T1F=Cyc_AssnDef_uint(_T1E);return _T1F;_TL1C0: goto _LL7;case Cyc_Absyn_UDiv:
 if(j==0U)goto _TL1C2;_T20=i / j;_T21=Cyc_AssnDef_uint(_T20);return _T21;_TL1C2: goto _LL7;case Cyc_Absyn_Mod:
 if(j==0U)goto _TL1C4;_T22=i;_T23=(int)_T22;_T24=j;_T25=(int)_T24;_T26=_T23 % _T25;_T27=(unsigned)_T26;_T28=Cyc_AssnDef_uint(_T27);return _T28;_TL1C4: goto _LL7;case Cyc_Absyn_UMod:
 if(j==0U)goto _TL1C6;_T29=i % j;_T2A=Cyc_AssnDef_uint(_T29);return _T2A;_TL1C6: goto _LL7;case Cyc_Absyn_Times: _T2B=i * j;_T2C=
Cyc_AssnDef_uint(_T2B);return _T2C;default: goto _LL7;}_LL7: goto _LL0;}_TL1BD: _T2D=_T66.f0;if(_T2D!=Cyc_Absyn_Minus)goto _TL1C8;goto _LL3;_TL1C8: goto _LL5;_TL1BB: _T2E=_T66.f0;if(_T2E!=Cyc_Absyn_Minus)goto _TL1CA;_LL3: _T68=_T66.f1;_T67=_T66.f2;{void*t1=_T68;void*t2=_T67;if(tp==0)goto _TL1CC;_T2F=tp;_T30=(int*)_T2F;_T31=*_T30;if(_T31!=0)goto _TL1CE;_T32=tp;_T33=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T32;_T34=_T33->f1;_T35=(int*)_T34;_T36=*_T35;if(_T36!=1)goto _TL1D0;_T37=
# 1391
Cyc_AssnDef_termcmp(t1,t2);if(_T37!=0)goto _TL1D2;_T38=Cyc_AssnDef_uint(0U);return _T38;_TL1D2: _T39=t2;_T3A=- 1U;_T3B=
# 1393
Cyc_AssnDef_uint(_T3A);_T3C=tp;t2=Cyc_AssnDef_binop(1U,_T39,_T3B,_T3C);_T3D=
Cyc_AssnDef_binop(0U,t1,t2,tp);return _T3D;_TL1D0: goto _LL25;_TL1CE: goto _LL25;_TL1CC: _LL25: goto _LL22;_LL22: goto _LL0;}_TL1CA: _LL5: _T3E=
# 1402
Cyc_AssnDef_is_associative_commutative(p,tp);if(!_T3E)goto _TL1D4;_T3F=p;_T40=(int)_T3F;
# 1407
if(_T40!=0)goto _TL1D6;_T41=Cyc_AssnDef_normalize_plus(t1,t2,tp);return _T41;
_TL1D6: _T42=Cyc_AssnDef_normalize_assoc_commu(p,t1,t2,tp);return _T42;_TL1D4: _T43=
# 1410
Cyc_AssnDef_is_nonassociative_commutative(p,tp);if(!_T43)goto _TL1D8;_T44=
Cyc_AssnDef_termcmp(t1,t2);if(_T44 <= 0)goto _TL1DA;{
void*tmpt=t1;
t1=t2;
t2=tmpt;}goto _TL1DB;_TL1DA: _TL1DB: goto _TL1D9;
# 1416
_TL1D8: _T45=p;_T46=(int)_T45;if(_T46!=0)goto _TL1DC;{struct _tuple15 _T6B;
_T6B.f0=t1;_T6B.f1=t2;_T47=_T6B;}{struct _tuple15 _T6B=_T47;unsigned _T6C;void*_T6D;unsigned _T6E;void*_T6F;_T48=_T6B.f0;_T49=(int*)_T48;_T4A=*_T49;if(_T4A!=4)goto _TL1DE;_T4B=_T6B.f0;_T4C=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T4B;_T4D=_T4C->f1;if(_T4D!=Cyc_Absyn_Plus)goto _TL1E0;_T4E=_T6B.f0;_T4F=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T4E;_T50=_T4F->f3;_T51=(int*)_T50;_T52=*_T51;if(_T52!=0)goto _TL1E2;_T53=_T6B.f1;_T54=(int*)_T53;_T55=*_T54;if(_T55!=0)goto _TL1E4;_T56=_T6B.f0;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T70=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T56;_T57=_T70->f2;_T6F=(void*)_T57;_T58=_T70->f3;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T71=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T58;_T6E=_T71->f1;}_T59=_T70->f4;_T6D=(void*)_T59;}_T5A=_T6B.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T70=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T5A;_T6C=_T70->f1;}{void*t1=_T6F;unsigned i=_T6E;void*tp2=_T6D;unsigned j=_T6C;
# 1419
if(tp==0)goto _TL1E6;if(tp2==0)goto _TL1E6;_T5B=Cyc_Unify_unify(tp,tp2);if(!_T5B)goto _TL1E6;_T5C=t1;_T5D=i + j;_T5E=
Cyc_AssnDef_uint(_T5D);_T5F=tp;_T60=Cyc_AssnDef_plus(_T5C,_T5E,_T5F);return _T60;_TL1E6: goto _LL27;}_TL1E4: goto _LL2A;_TL1E2: goto _LL2A;_TL1E0: goto _LL2A;_TL1DE: _LL2A: goto _LL27;_LL27:;}goto _TL1DD;_TL1DC: _TL1DD: _TL1D9: goto _LL0;_LL0:;}{struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T66;_T66.tag=4;
# 1427
_T66.f1=p;_T66.f2=t1;_T66.f3=t2;_T66.f4=tp;_T61=_T66;}{struct Cyc_AssnDef_Binop_AssnDef_Term_struct _T66=_T61;struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_T66;_T62=& p;_T63=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T62;_T64=(void*)_T63;_T65=
Cyc_AssnDef_hash_cons_term(_T64);return _T65;}}
# 1431
void*Cyc_AssnDef_get_term_type(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;struct Cyc_Absyn_Exp*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;
LOOP: {void*_T2E;unsigned _T2F;void*_T30;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=t;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T3;_T4=_T31->f2;_T30=(void*)_T4;}{void*tp=_T30;_T5=tp;
# 1434
return _T5;}case 1: _T6=t;{struct Cyc_AssnDef_Const_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_T6;_T30=_T31->f1;}{struct Cyc_Absyn_Exp*e=_T30;_T7=e;_T8=_T7->topt;
return _T8;}case 2: _T9=t;{struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_T9;_TA=_T31->f3;_T30=(void*)_TA;}{void*topt=_T30;_TB=topt;
return _TB;}case 4: _TC=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_TC;_TD=_T31->f4;_T30=(void*)_TD;}{void*tp=_T30;_TE=tp;
return _TE;}case 3: _TF=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_TF;_T10=_T31->f3;_T30=(void*)_T10;}{void*tp=_T30;_T11=tp;
return _T11;}case 5: _T12=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T12;_T13=_T31->f1;_T30=(void*)_T13;}{void*tp=_T30;_T14=tp;
return _T14;}case 6: _T15=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T15;_T16=_T31->f3;_T30=(void*)_T16;}{void*tp=_T30;_T17=tp;
return _T17;}case 8: _T18=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T18;_T19=_T31->f4;_T30=(void*)_T19;}{void*tp=_T30;_T1A=tp;
return _T1A;}case 9: _T1B=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T1B;_T1C=_T31->f1;_T30=(void*)_T1C;_T2F=_T31->f2;_T1D=_T31->f3;_T2E=(void*)_T1D;}{void*t1=_T30;unsigned i=_T2F;void*tp=_T2E;_T1E=tp;
return _T1E;}case 10: _T1F=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T1F;_T20=_T31->f1;_T30=(void*)_T20;}{void*t1=_T30;
t=t1;goto LOOP;}case 11: _T21=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T21;_T30=_T31->f1;_T22=_T31->f3;_T2E=(void*)_T22;}{struct Cyc_Absyn_Vardecl*vd=_T30;void*tp=_T2E;_T23=tp;
return _T23;}case 7:
 return 0;case 12: _T24=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T24;_T25=_T31->f5;_T30=(void*)_T25;}{void*tp=_T30;_T26=tp;
return _T26;}case 13: _T27=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T27;_T28=_T31->f3;_T30=(void*)_T28;}{void*tp=_T30;_T29=tp;
return _T29;}case 14: _T2A=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T31=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T2A;_T2B=_T31->f3;_T30=(void*)_T2B;}{void*tp=_T30;_T2C=tp;
return _T2C;}default: _T2D=Cyc_Absyn_uint_type;
return _T2D;};}}
# 1453
static void*Cyc_AssnDef_subst_term(struct Cyc_Dict_Dict dict,void*t,struct Cyc_Hashtable_Table*term_h){void**_T0;void*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;void*_T7;struct Cyc_AssnDef_Addr_AssnDef_Term_struct _T8;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T9;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TA;void*_TB;void*_TC;void*_TD;void*_TE;void*_TF;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T10;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T11;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void**_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;void*_T28;struct Cyc_List_List*_T29;struct Cyc_Dict_Dict _T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_Hashtable_Table*_T2D;struct Cyc_List_List*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;
# 1455
void**t1=Cyc_Hashtable_lookup_opt(term_h,t);
if(t1==0)goto _TL1E9;_T0=t1;_T1=*_T0;
# 1461
return _T1;_TL1E9: {
# 1463
void*res;{unsigned _T40;enum Cyc_Absyn_Primop _T41;void*_T42;int _T43;void*_T44;void*_T45;void*_T46;_T2=t;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 0: goto _LL4;case 1: _LL4:
# 1467
 res=t;goto _LL0;case 11: _T5=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T5;_T46=_T47->f1;_T6=_T47->f2;_T45=(void*)_T6;_T7=_T47->f3;_T44=(void*)_T7;}{struct Cyc_Absyn_Vardecl*vd=_T46;void*t1=_T45;void*tp=_T44;
# 1469
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);{struct Cyc_AssnDef_Addr_AssnDef_Term_struct _T47;_T47.tag=11;
_T47.f1=vd;_T47.f2=s1;_T47.f3=tp;_T8=_T47;}{struct Cyc_AssnDef_Addr_AssnDef_Term_struct addr_term=_T8;_T9=& addr_term;_TA=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T9;_TB=(void*)_TA;
res=Cyc_AssnDef_hash_cons_term(_TB);goto _LL0;}}case 12: _TC=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TC;_T46=_T47->f1;_T43=_T47->f2;_TD=_T47->f3;_T45=(void*)_TD;_TE=_T47->f4;_T44=(void*)_TE;_TF=_T47->f5;_T42=(void*)_TF;}{struct Cyc_Absyn_Exp*e1=_T46;int i=_T43;void*t1=_T45;void*t2=_T44;void*tp=_T42;
# 1474
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T47;_T47.tag=12;
_T47.f1=e1;_T47.f2=i;_T47.f3=s1;_T47.f4=s2;_T47.f5=tp;_T10=_T47;}{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct alloc_term=_T10;_T11=& alloc_term;_T12=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T11;_T13=(void*)_T12;
res=Cyc_AssnDef_hash_cons_term(_T13);goto _LL0;}}case 6: _T14=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T14;_T15=_T47->f1;_T46=(void*)_T15;_T16=_T47->f2;_T45=(void*)_T16;_T17=_T47->f3;_T44=(void*)_T17;}{void*t1=_T46;void*t2=_T45;void*tp=_T44;
# 1480
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_select(s1,s2,tp);goto _LL0;}case 7: _T18=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T18;_T19=_T47->f1;_T46=(void*)_T19;_T1A=_T47->f2;_T45=(void*)_T1A;_T1B=_T47->f3;_T44=(void*)_T1B;}{void*t1=_T46;void*t2=_T45;void*t3=_T44;
# 1485
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
void*s3=Cyc_AssnDef_subst_term(dict,t3,term_h);
res=Cyc_AssnDef_update(s1,s2,s3);goto _LL0;}case 2:  {
# 1491
void**t1=Cyc_Dict_lookup_opt(dict,t);
if(t1==0)goto _TL1EC;_T1C=t1;res=*_T1C;goto _TL1ED;
_TL1EC: res=t;_TL1ED: goto _LL0;}case 3: _T1D=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T1D;_T41=_T47->f1;_T1E=_T47->f2;_T46=(void*)_T1E;_T1F=_T47->f3;_T45=(void*)_T1F;}{enum Cyc_Absyn_Primop p=_T41;void*t1=_T46;void*tp=_T45;
# 1496
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_unop(p,s1,tp);goto _LL0;}case 4: _T20=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T20;_T41=_T47->f1;_T21=_T47->f2;_T46=(void*)_T21;_T22=_T47->f3;_T45=(void*)_T22;_T23=_T47->f4;_T44=(void*)_T23;}{enum Cyc_Absyn_Primop p=_T41;void*t1=_T46;void*t2=_T45;void*tp=_T44;
# 1500
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_binop(p,s1,s2,tp);goto _LL0;}case 5: _T24=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T24;_T25=_T47->f1;_T46=(void*)_T25;_T26=_T47->f2;_T45=(void*)_T26;}{void*tp=_T46;void*t1=_T45;
# 1505
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_cast(tp,s1);goto _LL0;}case 8: _T27=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T27;_T43=_T47->f1;_T40=_T47->f2;_T46=_T47->f3;_T28=_T47->f4;_T45=(void*)_T28;}{int b=_T43;unsigned tag=_T40;struct Cyc_List_List*tlist=_T46;void*tp=_T45;
# 1509
struct Cyc_List_List*slist=0;
_TL1F1: if(tlist!=0)goto _TL1EF;else{goto _TL1F0;}
_TL1EF:{struct Cyc_List_List*_T47=_cycalloc(sizeof(struct Cyc_List_List));_T2A=dict;_T2B=tlist;_T2C=_T2B->hd;_T2D=term_h;_T47->hd=Cyc_AssnDef_subst_term(_T2A,_T2C,_T2D);_T47->tl=slist;_T29=(struct Cyc_List_List*)_T47;}slist=_T29;_T2E=tlist;
# 1510
tlist=_T2E->tl;goto _TL1F1;_TL1F0:
# 1513
 Cyc_List_imp_rev(slist);
res=Cyc_AssnDef_aggr(b,tag,slist,tp);goto _LL0;}case 9: _T2F=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T2F;_T30=_T47->f1;_T46=(void*)_T30;_T40=_T47->f2;_T31=_T47->f3;_T45=(void*)_T31;}{void*t1=_T46;unsigned i=_T40;void*tp=_T45;
# 1517
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_proj(s1,i,tp);goto _LL0;}case 10: _T32=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T32;_T33=_T47->f1;_T46=(void*)_T33;_T40=_T47->f2;_T34=_T47->f3;_T45=(void*)_T34;}{void*t1=_T46;unsigned i=_T40;void*t2=_T45;
# 1521
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_aggr_update(s1,i,s2);goto _LL0;}case 13: _T35=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T35;_T36=_T47->f1;_T46=(void*)_T36;_T40=_T47->f2;_T37=_T47->f3;_T45=(void*)_T37;}{void*t1=_T46;unsigned i=_T40;void*tp=_T45;
# 1526
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_offsetf(s1,i,tp);goto _LL0;}case 14: _T38=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T38;_T39=_T47->f1;_T46=(void*)_T39;_T3A=_T47->f2;_T45=(void*)_T3A;_T3B=_T47->f3;_T44=(void*)_T3B;}{void*t1=_T46;void*t2=_T45;void*tp=_T44;
# 1530
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_offseti(s1,s2,tp);goto _LL0;}default: _T3C=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T47=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T3C;_T3D=_T47->f1;_T46=(void*)_T3D;}{void*t1=_T46;_T3E=
# 1535
Cyc_AssnDef_subst_term(dict,t1,term_h);res=Cyc_AssnDef_tagof_tm(_T3E);goto _LL0;}}_LL0:;}
# 1538
Cyc_Hashtable_insert(term_h,t,res);_T3F=res;
return _T3F;}}
# 1542
static int Cyc_AssnDef_hash_ptr(void*s){void*_T0;int _T1;_T0=s;_T1=(int)_T0;return _T1;}
# 1544
void*Cyc_AssnDef_subst_t(struct Cyc_Dict_Dict dict,void*t){int(*_T0)(void*,void*);void*_T1;_T0=Cyc_Core_ptrcmp;{
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,_T0,Cyc_AssnDef_hash_ptr);_T1=
Cyc_AssnDef_subst_term(dict,t,term_h);return _T1;}}struct _tuple19{struct _fat_ptr f0;unsigned f1;};
# 1551
static struct _tuple19 Cyc_AssnDef_a2dag(void*a,int*ctr,struct Cyc_Hashtable_Table*t,struct Cyc_List_List**decls){struct _tuple19**(*_T0)(struct Cyc_Hashtable_Table*,void*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct _tuple19**_T2;struct _tuple19*_T3;struct _tuple19 _T4;int*_T5;int*_T6;struct _fat_ptr _T7;struct Cyc_Int_pa_PrintArg_struct _T8;int _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;void*_TC;int*_TD;unsigned _TE;void*_TF;void*_T10;void*_T11;enum Cyc_AssnDef_Primreln _T12;int _T13;int(*_T14)(struct _fat_ptr,struct _fat_ptr);void*(*_T15)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;void*_T1E;void*_T1F;void*_T20;struct _fat_ptr _T21;struct Cyc_String_pa_PrintArg_struct _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;unsigned _T26;void*_T27;void*_T28;void*_T29;struct _fat_ptr _T2A;struct Cyc_String_pa_PrintArg_struct _T2B;struct Cyc_String_pa_PrintArg_struct _T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;unsigned _T2F;struct Cyc_List_List**_T30;struct Cyc_List_List*_T31;struct _fat_ptr*_T32;struct _fat_ptr _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct Cyc_String_pa_PrintArg_struct _T35;struct _fat_ptr _T36;struct _fat_ptr _T37;struct Cyc_List_List**_T38;void(*_T39)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*);void(*_T3A)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T3B;void*_T3C;struct _tuple19*_T3D;struct _tuple19 _T3E;_T1=Cyc_Hashtable_lookup_opt;{
# 1555
struct _tuple19**(*_T3F)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple19**(*)(struct Cyc_Hashtable_Table*,void*))_T1;_T0=_T3F;}{struct _tuple19**resopt=_T0(t,a);
if(resopt==0)goto _TL1F2;_T2=resopt;_T3=*_T2;_T4=*_T3;
return _T4;_TL1F2: _T5=ctr;{
# 1559
int c=*_T5;_T6=ctr;
*_T6=c + 1;{struct Cyc_Int_pa_PrintArg_struct _T3F;_T3F.tag=1;_T9=c;
_T3F.f1=(unsigned long)_T9;_T8=_T3F;}{struct Cyc_Int_pa_PrintArg_struct _T3F=_T8;void*_T40[1];_T40[0]=& _T3F;_TA=_tag_fat("A%d",sizeof(char),4U);_TB=_tag_fat(_T40,sizeof(void*),1);_T7=Cyc_aprintf(_TA,_TB);}{struct _fat_ptr name=_T7;
struct _fat_ptr s;
unsigned size=1U;{void*_T3F;enum Cyc_AssnDef_Primreln _T40;void*_T41;_TC=a;_TD=(int*)_TC;_TE=*_TD;switch(_TE){case 0:
# 1565
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _TF=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T42=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_TF;_T10=_T42->f1;_T41=(void*)_T10;_T40=_T42->f2;_T11=_T42->f3;_T3F=(void*)_T11;}{void*t1=_T41;enum Cyc_AssnDef_Primreln p=_T40;void*t2=_T3F;
# 1568
struct _fat_ptr ps;_T12=p;_T13=(int)_T12;switch(_T13){case Cyc_AssnDef_Eq:
# 1570
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default: _T15=Cyc_Warn_impos;{
int(*_T42)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T15;_T14=_T42;}_T16=_tag_fat("assn2string primop",sizeof(char),19U);_T17=_tag_fat(0U,sizeof(void*),0);_T14(_T16,_T17);}_LLB:{struct Cyc_String_pa_PrintArg_struct _T42;_T42.tag=0;
# 1578
_T42.f1=Cyc_AssnDef_term2string(t1);_T19=_T42;}{struct Cyc_String_pa_PrintArg_struct _T42=_T19;{struct Cyc_String_pa_PrintArg_struct _T43;_T43.tag=0;_T43.f1=ps;_T1A=_T43;}{struct Cyc_String_pa_PrintArg_struct _T43=_T1A;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;_T44.f1=Cyc_AssnDef_term2string(t2);_T1B=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T1B;void*_T45[3];_T45[0]=& _T42;_T45[1]=& _T43;_T45[2]=& _T44;_T1C=_tag_fat("%s%s%s",sizeof(char),7U);_T1D=_tag_fat(_T45,sizeof(void*),3);_T18=Cyc_aprintf(_T1C,_T1D);}}}s=_T18;goto _LL0;}case 4: _T1E=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T42=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T1E;_T1F=_T42->f1;_T41=(void*)_T1F;_T20=_T42->f2;_T3F=(void*)_T20;}{void*a1=_T41;void*a2=_T3F;
# 1581
struct _tuple19 _T42=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _T43;struct _fat_ptr _T44;_T44=_T42.f0;_T43=_T42.f1;{struct _fat_ptr s1=_T44;unsigned n1=_T43;
struct _tuple19 _T45=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _T46;struct _fat_ptr _T47;_T47=_T45.f0;_T46=_T45.f1;{struct _fat_ptr s2=_T47;unsigned n2=_T46;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;
_T48.f1=s1;_T22=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T22;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=s2;_T23=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T23;void*_T4A[2];_T4A[0]=& _T48;_T4A[1]=& _T49;_T24=_tag_fat("%s || %s",sizeof(char),9U);_T25=_tag_fat(_T4A,sizeof(void*),2);_T21=Cyc_aprintf(_T24,_T25);}}s=_T21;_T26=n1 + n2;
size=size + _T26;goto _LL0;}}}default: _T27=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T42=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T27;_T28=_T42->f1;_T41=(void*)_T28;_T29=_T42->f2;_T3F=(void*)_T29;}{void*a1=_T41;void*a2=_T3F;
# 1587
struct _tuple19 _T42=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _T43;struct _fat_ptr _T44;_T44=_T42.f0;_T43=_T42.f1;{struct _fat_ptr s1=_T44;unsigned n1=_T43;
struct _tuple19 _T45=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _T46;struct _fat_ptr _T47;_T47=_T45.f0;_T46=_T45.f1;{struct _fat_ptr s2=_T47;unsigned n2=_T46;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;
_T48.f1=s1;_T2B=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T2B;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=s2;_T2C=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T2C;void*_T4A[2];_T4A[0]=& _T48;_T4A[1]=& _T49;_T2D=_tag_fat("%s && %s",sizeof(char),9U);_T2E=_tag_fat(_T4A,sizeof(void*),2);_T2A=Cyc_aprintf(_T2D,_T2E);}}s=_T2A;_T2F=n1 + n2;
size=size + _T2F;goto _LL0;}}}}_LL0:;}_T30=decls;{struct Cyc_List_List*_T3F=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T40=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T41;_T41.tag=0;
# 1593
_T41.f1=name;_T34=_T41;}{struct Cyc_String_pa_PrintArg_struct _T41=_T34;{struct Cyc_String_pa_PrintArg_struct _T42;_T42.tag=0;_T42.f1=s;_T35=_T42;}{struct Cyc_String_pa_PrintArg_struct _T42=_T35;void*_T43[2];_T43[0]=& _T41;_T43[1]=& _T42;_T36=_tag_fat("%s = %s\n",sizeof(char),9U);_T37=_tag_fat(_T43,sizeof(void*),2);_T33=Cyc_aprintf(_T36,_T37);}}*_T40=_T33;_T32=(struct _fat_ptr*)_T40;}_T3F->hd=_T32;_T38=decls;_T3F->tl=*_T38;_T31=(struct Cyc_List_List*)_T3F;}*_T30=_T31;_T3A=Cyc_Hashtable_insert;{
void(*_T3F)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*))_T3A;_T39=_T3F;}_T3B=t;_T3C=a;{struct _tuple19*_T3F=_cycalloc(sizeof(struct _tuple19));_T3F->f0=name;_T3F->f1=size;_T3D=(struct _tuple19*)_T3F;}_T39(_T3B,_T3C,_T3D);{struct _tuple19 _T3F;
_T3F.f0=name;_T3F.f1=size;_T3E=_T3F;}return _T3E;}}}}
# 1598
int Cyc_AssnDef_assnhash(void*);
# 1600
struct _fat_ptr Cyc_AssnDef_assn2dag(void*a){int(*_T0)(void*,void*);int(*_T1)(void*);void*_T2;int*_T3;struct Cyc_Hashtable_Table*_T4;struct Cyc_List_List**_T5;struct Cyc_List_List*_T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc_Int_pa_PrintArg_struct _TA;unsigned _TB;int _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;
struct Cyc_List_List*decls=0;_T0=Cyc_AssnDef_assncmp;_T1=Cyc_AssnDef_assnhash;{
struct Cyc_Hashtable_Table*t=
Cyc_Hashtable_create(221,_T0,_T1);
int ctr=0;_T2=a;_T3=& ctr;_T4=t;_T5=& decls;{
struct _tuple19 _T10=Cyc_AssnDef_a2dag(_T2,_T3,_T4,_T5);unsigned _T11;struct _fat_ptr _T12;_T12=_T10.f0;_T11=_T10.f1;{struct _fat_ptr s=_T12;unsigned n=_T11;{struct Cyc_List_List*_T13=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T14=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T15;_T15.tag=0;
_T15.f1=s;_T9=_T15;}{struct Cyc_String_pa_PrintArg_struct _T15=_T9;{struct Cyc_Int_pa_PrintArg_struct _T16;_T16.tag=1;_TB=n;_TC=(int)_TB;_T16.f1=(unsigned long)_TC;_TA=_T16;}{struct Cyc_Int_pa_PrintArg_struct _T16=_TA;void*_T17[2];_T17[0]=& _T15;_T17[1]=& _T16;_TD=_tag_fat("in %s (%d nodes as tree)\n",sizeof(char),26U);_TE=_tag_fat(_T17,sizeof(void*),2);_T8=Cyc_aprintf(_TD,_TE);}}*_T14=_T8;_T7=(struct _fat_ptr*)_T14;}_T13->hd=_T7;_T13->tl=decls;_T6=(struct Cyc_List_List*)_T13;}decls=Cyc_List_imp_rev(_T6);_TF=
Cyc_strconcat_l(decls);return _TF;}}}}
# 1613
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int inprec){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;enum Cyc_AssnDef_Primreln _T6;int _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr);void*(*_T9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct Cyc_String_pa_PrintArg_struct _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;void*_T12;void*_T13;void*_T14;struct _fat_ptr _T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc_String_pa_PrintArg_struct _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;void*_T1A;void*_T1B;void*_T1C;struct _fat_ptr _T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct _fat_ptr _T26;
int myprec=10;
struct _fat_ptr s;{void*_T27;enum Cyc_AssnDef_Primreln _T28;void*_T29;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0:
# 1617
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _T3=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2A=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T3;_T4=_T2A->f1;_T29=(void*)_T4;_T28=_T2A->f2;_T5=_T2A->f3;_T27=(void*)_T5;}{void*t1=_T29;enum Cyc_AssnDef_Primreln p=_T28;void*t2=_T27;
# 1620
struct _fat_ptr ps;_T6=p;_T7=(int)_T6;switch(_T7){case Cyc_AssnDef_Eq:
# 1622
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default: _T9=Cyc_Warn_impos;{
int(*_T2A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T9;_T8=_T2A;}_TA=_tag_fat("assn2string primop",sizeof(char),19U);_TB=_tag_fat(0U,sizeof(void*),0);_T8(_TA,_TB);}_LLB:{struct Cyc_String_pa_PrintArg_struct _T2A;_T2A.tag=0;
# 1630
_T2A.f1=Cyc_AssnDef_term2string(t1);_TD=_T2A;}{struct Cyc_String_pa_PrintArg_struct _T2A=_TD;{struct Cyc_String_pa_PrintArg_struct _T2B;_T2B.tag=0;_T2B.f1=ps;_TE=_T2B;}{struct Cyc_String_pa_PrintArg_struct _T2B=_TE;{struct Cyc_String_pa_PrintArg_struct _T2C;_T2C.tag=0;_T2C.f1=Cyc_AssnDef_term2string(t2);_TF=_T2C;}{struct Cyc_String_pa_PrintArg_struct _T2C=_TF;void*_T2D[3];_T2D[0]=& _T2A;_T2D[1]=& _T2B;_T2D[2]=& _T2C;_T10=_tag_fat("%s%s%s",sizeof(char),7U);_T11=_tag_fat(_T2D,sizeof(void*),3);_TC=Cyc_aprintf(_T10,_T11);}}}s=_TC;goto _LL0;}case 4: _T12=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T2A=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T12;_T13=_T2A->f1;_T29=(void*)_T13;_T14=_T2A->f2;_T27=(void*)_T14;}{void*a1=_T29;void*a2=_T27;
# 1633
myprec=5;{struct Cyc_String_pa_PrintArg_struct _T2A;_T2A.tag=0;
_T2A.f1=Cyc_AssnDef_a2string(a1,myprec);_T16=_T2A;}{struct Cyc_String_pa_PrintArg_struct _T2A=_T16;{struct Cyc_String_pa_PrintArg_struct _T2B;_T2B.tag=0;_T2B.f1=Cyc_AssnDef_a2string(a2,myprec);_T17=_T2B;}{struct Cyc_String_pa_PrintArg_struct _T2B=_T17;void*_T2C[2];_T2C[0]=& _T2A;_T2C[1]=& _T2B;_T18=_tag_fat("%s || %s",sizeof(char),9U);_T19=_tag_fat(_T2C,sizeof(void*),2);_T15=Cyc_aprintf(_T18,_T19);}}s=_T15;goto _LL0;}default: _T1A=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T2A=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T1A;_T1B=_T2A->f1;_T29=(void*)_T1B;_T1C=_T2A->f2;_T27=(void*)_T1C;}{void*a1=_T29;void*a2=_T27;
# 1637
myprec=10;{struct Cyc_String_pa_PrintArg_struct _T2A;_T2A.tag=0;
_T2A.f1=Cyc_AssnDef_a2string(a1,10);_T1E=_T2A;}{struct Cyc_String_pa_PrintArg_struct _T2A=_T1E;{struct Cyc_String_pa_PrintArg_struct _T2B;_T2B.tag=0;_T2B.f1=Cyc_AssnDef_a2string(a2,10);_T1F=_T2B;}{struct Cyc_String_pa_PrintArg_struct _T2B=_T1F;void*_T2C[2];_T2C[0]=& _T2A;_T2C[1]=& _T2B;_T20=_tag_fat("%s && %s",sizeof(char),9U);_T21=_tag_fat(_T2C,sizeof(void*),2);_T1D=Cyc_aprintf(_T20,_T21);}}s=_T1D;goto _LL0;}}_LL0:;}
# 1641
if(myprec >= inprec)goto _TL1F8;{struct Cyc_String_pa_PrintArg_struct _T27;_T27.tag=0;
_T27.f1=s;_T23=_T27;}{struct Cyc_String_pa_PrintArg_struct _T27=_T23;void*_T28[1];_T28[0]=& _T27;_T24=_tag_fat("(%s)",sizeof(char),5U);_T25=_tag_fat(_T28,sizeof(void*),1);_T22=Cyc_aprintf(_T24,_T25);}s=_T22;goto _TL1F9;_TL1F8: _TL1F9: _T26=s;
return _T26;}
# 1647
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){struct _fat_ptr _T0;_T0=
Cyc_AssnDef_a2string(a,10);return _T0;}
# 1659 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;int _T6;unsigned _T7;int _T8;unsigned _T9;enum Cyc_AssnDef_Primreln _TA;unsigned _TB;unsigned _TC;unsigned _TD;int _TE;void*_TF;void*_T10;void*_T11;void*_T12;unsigned _T13;void*_T14;unsigned _T15;unsigned _T16;unsigned _T17;int _T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;unsigned _T1D;void*_T1E;unsigned _T1F;unsigned _T20;int _T21;void*_T22;enum Cyc_AssnDef_Primreln _T23;void*_T24;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1:
# 1661
 return 0;case 0:
 return 1;case 2: _T3=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T25=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T3;_T4=_T25->f1;_T24=(void*)_T4;_T23=_T25->f2;_T5=_T25->f3;_T22=(void*)_T5;}{void*t1=_T24;enum Cyc_AssnDef_Primreln p=_T23;void*t2=_T22;_T6=
# 1664
Cyc_AssnDef_termhash(t1);_T7=(unsigned)_T6;_T8=Cyc_AssnDef_termhash(t2);_T9=(unsigned)_T8;_TA=p;_TB=(unsigned)_TA;_TC=Cyc_AssnDef_combine(_T9,_TB);_TD=Cyc_AssnDef_combine(_T7,_TC);_TE=(int)_TD;return _TE;}case 3: _TF=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T25=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_TF;_T10=_T25->f1;_T24=(void*)_T10;_T11=_T25->f2;_T22=(void*)_T11;}{void*a1=_T24;void*a2=_T22;_T12=a1;_T13=(unsigned)_T12;_T14=a2;_T15=(unsigned)_T14;_T16=_T15 + 1U;_T17=
# 1666
Cyc_AssnDef_combine(_T13,_T16);_T18=(int)_T17;return _T18;}default: _T19=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T25=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T19;_T1A=_T25->f1;_T24=(void*)_T1A;_T1B=_T25->f2;_T22=(void*)_T1B;}{void*a1=_T24;void*a2=_T22;_T1C=a1;_T1D=(unsigned)_T1C;_T1E=a2;_T1F=(unsigned)_T1E;_T20=
# 1668
Cyc_AssnDef_combine(_T1D,_T1F);_T21=(int)_T20;return _T21;}};}
# 1674
int Cyc_AssnDef_assncmp(void*_a1,void*_a2){void*_T0;const unsigned*_T1;unsigned _T2;void*_T3;const unsigned*_T4;unsigned _T5;unsigned _T6;int _T7;struct _tuple15 _T8;void*_T9;int*_TA;unsigned _TB;void*_TC;int*_TD;int _TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;int*_T16;int _T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;int _T1E;void*_T1F;int*_T20;int _T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;enum Cyc_AssnDef_Primreln _T28;int _T29;enum Cyc_AssnDef_Primreln _T2A;int _T2B;int _T2C;int _T2D;int _T2E;int(*_T2F)(struct _fat_ptr,struct _fat_ptr);void*(*_T30)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T31;struct _fat_ptr _T32;
void*a1=_a1;
void*a2=_a2;
LOOP:
 if(a1!=a2)goto _TL1FB;return 0;_TL1FB: _T0=a2;_T1=(const unsigned*)_T0;_T2=*_T1;_T3=a1;_T4=(const unsigned*)_T3;_T5=*_T4;_T6=_T2 - _T5;{
int c=(int)_T6;
if(c==0)goto _TL1FD;_T7=c;return _T7;_TL1FD:{struct _tuple15 _T33;
_T33.f0=a1;_T33.f1=a2;_T8=_T33;}{struct _tuple15 _T33=_T8;enum Cyc_AssnDef_Primreln _T34;enum Cyc_AssnDef_Primreln _T35;void*_T36;void*_T37;void*_T38;void*_T39;_T9=_T33.f0;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 3: _TC=_T33.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=3)goto _TL200;_TF=_T33.f0;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_TF;_T10=_T3A->f1;_T39=(void*)_T10;_T11=_T3A->f2;_T38=(void*)_T11;}_T12=_T33.f1;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T12;_T13=_T3A->f1;_T37=(void*)_T13;_T14=_T3A->f2;_T36=(void*)_T14;}{void*a11=_T39;void*a12=_T38;void*a21=_T37;void*a22=_T36;_T39=a11;_T38=a12;_T37=a21;_T36=a22;goto _LL4;}_TL200: goto _LL7;case 4: _T15=_T33.f1;_T16=(int*)_T15;_T17=*_T16;if(_T17!=4)goto _TL202;_T18=_T33.f0;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T18;_T19=_T3A->f1;_T39=(void*)_T19;_T1A=_T3A->f2;_T38=(void*)_T1A;}_T1B=_T33.f1;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T1B;_T1C=_T3A->f1;_T37=(void*)_T1C;_T1D=_T3A->f2;_T36=(void*)_T1D;}_LL4:{void*a11=_T39;void*a12=_T38;void*a21=_T37;void*a22=_T36;
# 1684
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c==0)goto _TL204;_T1E=c;return _T1E;_TL204:
 a1=a12;
a2=a22;goto LOOP;}_TL202: goto _LL7;case 2: _T1F=_T33.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=2)goto _TL206;_T22=_T33.f0;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T22;_T23=_T3A->f1;_T39=(void*)_T23;_T35=_T3A->f2;_T24=_T3A->f3;_T38=(void*)_T24;}_T25=_T33.f1;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T25;_T26=_T3A->f1;_T37=(void*)_T26;_T34=_T3A->f2;_T27=_T3A->f3;_T36=(void*)_T27;}{void*t11=_T39;enum Cyc_AssnDef_Primreln p1=_T35;void*t12=_T38;void*t21=_T37;enum Cyc_AssnDef_Primreln p2=_T34;void*t22=_T36;_T28=p1;_T29=(int)_T28;_T2A=p2;_T2B=(int)_T2A;{
# 1690
int c=_T29 - _T2B;
if(c==0)goto _TL208;_T2C=c;return _T2C;_TL208:
 c=Cyc_AssnDef_termcmp(t11,t21);
if(c==0)goto _TL20A;_T2D=c;return _T2D;_TL20A: _T2E=
Cyc_AssnDef_termcmp(t12,t22);return _T2E;}}_TL206: goto _LL7;default: _LL7: _T30=Cyc_Warn_impos;{
int(*_T3A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T30;_T2F=_T3A;}_T31=_tag_fat("assncmp:  tagof failure",sizeof(char),24U);_T32=_tag_fat(0U,sizeof(void*),0);_T2F(_T31,_T32);};}}}
# 1702
static void*Cyc_AssnDef_copy_assn(void*a){void*_T0;int*_T1;unsigned _T2;int(*_T3)(struct _fat_ptr,struct _fat_ptr);void*(*_T4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5;struct _fat_ptr _T6;int(*_T7)(struct _fat_ptr,struct _fat_ptr);void*(*_T8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T9;struct _fat_ptr _TA;void*_TB;void*_TC;void*_TD;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_TE;void*_TF;void*_T10;void*_T11;void*_T12;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T13;void*_T14;void*_T15;void*_T16;void*_T17;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T18;void*_T19;enum Cyc_AssnDef_Primreln _T1A;void*_T1B;void*_T1C;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T4=Cyc_Warn_impos;{
# 1705
int(*_T1D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4;_T3=_T1D;}_T5=_tag_fat("True assertion should be in the table",sizeof(char),38U);_T6=_tag_fat(0U,sizeof(void*),0);_T3(_T5,_T6);case 1: _T8=Cyc_Warn_impos;{
# 1707
int(*_T1D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T8;_T7=_T1D;}_T9=_tag_fat("False assertion should be in the table",sizeof(char),39U);_TA=_tag_fat(0U,sizeof(void*),0);_T7(_T9,_TA);case 3: _TB=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T1D=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_TB;_TC=_T1D->f1;_T1C=(void*)_TC;_TD=_T1D->f2;_T1B=(void*)_TD;}{void*a1=_T1C;void*a2=_T1B;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T1D=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_T1D->tag=3;
_T1D->f1=a1;_T1D->f2=a2;_TE=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T1D;}_TF=(void*)_TE;return _TF;}case 4: _T10=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T1D=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T10;_T11=_T1D->f1;_T1C=(void*)_T11;_T12=_T1D->f2;_T1B=(void*)_T12;}{void*a1=_T1C;void*a2=_T1B;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T1D=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_T1D->tag=4;
_T1D->f1=a1;_T1D->f2=a2;_T13=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T1D;}_T14=(void*)_T13;return _T14;}default: _T15=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1D=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T15;_T16=_T1D->f1;_T1C=(void*)_T16;_T1A=_T1D->f2;_T17=_T1D->f3;_T1B=(void*)_T17;}{void*t1=_T1C;enum Cyc_AssnDef_Primreln p=_T1A;void*t2=_T1B;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1D=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T1D->tag=2;
_T1D->f1=t1;_T1D->f2=p;_T1D->f3=t2;_T18=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T1D;}_T19=(void*)_T18;return _T19;}};}
# 1716
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 1724
static void*Cyc_AssnDef_neg_assn(void*a){void*_T0;int*_T1;unsigned _T2;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T3;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T4;void*_T5;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T7;void*_T8;void*_T9;void*_TA;void*_TB;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_TC;void*_TD;void*_TE;void*_TF;void*_T10;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T11;void*_T12;void*_T13;void*_T14;void*_T15;enum Cyc_AssnDef_Primreln _T16;int _T17;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T18;void*_T19;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1A;void*_T1B;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1C;void*_T1D;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1E;void*_T1F;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T20;void*_T21;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T22;void*_T23;int(*_T24)(struct _fat_ptr,struct _fat_ptr);void*(*_T25)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T26;struct _fat_ptr _T27;enum Cyc_AssnDef_Primreln _T28;void*_T29;void*_T2A;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=& Cyc_AssnDef_false_assn;_T4=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T3;_T5=(void*)_T4;
# 1726
return _T5;case 1: _T6=& Cyc_AssnDef_true_assn;_T7=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T6;_T8=(void*)_T7;
return _T8;case 3: _T9=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T2B=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T9;_TA=_T2B->f1;_T2A=(void*)_TA;_TB=_T2B->f2;_T29=(void*)_TB;}{void*a1=_T2A;void*a2=_T29;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_T2B->tag=4;
_T2B->f1=Cyc_AssnDef_not(a1);_T2B->f2=Cyc_AssnDef_not(a2);_TC=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T2B;}_TD=(void*)_TC;return _TD;}case 4: _TE=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T2B=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_TE;_TF=_T2B->f1;_T2A=(void*)_TF;_T10=_T2B->f2;_T29=(void*)_T10;}{void*a1=_T2A;void*a2=_T29;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_T2B->tag=3;
_T2B->f1=Cyc_AssnDef_not(a1);_T2B->f2=Cyc_AssnDef_not(a2);_T11=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T2B;}_T12=(void*)_T11;return _T12;}default: _T13=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T13;_T14=_T2B->f1;_T2A=(void*)_T14;_T28=_T2B->f2;_T15=_T2B->f3;_T29=(void*)_T15;}{void*t1=_T2A;enum Cyc_AssnDef_Primreln p=_T28;void*t2=_T29;_T16=p;_T17=(int)_T16;switch(_T17){case Cyc_AssnDef_Eq:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
# 1732
_T2B->f1=t1;_T2B->f2=1U;_T2B->f3=t2;_T18=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T19=(void*)_T18;return _T19;case Cyc_AssnDef_Neq:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
_T2B->f1=t1;_T2B->f2=0U;_T2B->f3=t2;_T1A=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T1B=(void*)_T1A;return _T1B;case Cyc_AssnDef_SLt:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
_T2B->f1=t2;_T2B->f2=3U;_T2B->f3=t1;_T1C=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T1D=(void*)_T1C;return _T1D;case Cyc_AssnDef_SLte:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
_T2B->f1=t2;_T2B->f2=2U;_T2B->f3=t1;_T1E=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T1F=(void*)_T1E;return _T1F;case Cyc_AssnDef_ULt:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
_T2B->f1=t2;_T2B->f2=5U;_T2B->f3=t1;_T20=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T21=(void*)_T20;return _T21;case Cyc_AssnDef_ULte:{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2B=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_T2B->tag=2;
_T2B->f1=t2;_T2B->f2=4U;_T2B->f3=t1;_T22=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T2B;}_T23=(void*)_T22;return _T23;default: _T25=Cyc_Warn_impos;{
int(*_T2B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T25;_T24=_T2B;}_T26=_tag_fat("undefined prim relation",sizeof(char),24U);_T27=_tag_fat(0U,sizeof(void*),0);_T24(_T26,_T27);};}};}struct Cyc_AssnDef_AssnHashedInfo{void*assn;void*negation;struct Cyc_Set_Set**widened_set;struct Cyc_Set_Set**free_logicvar_set;};
# 1754
static struct Cyc_Hashtable_Table*Cyc_AssnDef_assn_hash_cons_table;
# 1756
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_assn_hash_cons_table (void){void(*_T0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T2;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T4;void*_T5;struct Cyc_AssnDef_AssnHashedInfo*_T6;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T7;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T8;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TA;struct Cyc_Set_Set**_TB;struct Cyc_Set_Set**_TC;void(*_TD)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*);void(*_TE)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_TF;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T10;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T11;void*_T12;struct Cyc_AssnDef_AssnHashedInfo*_T13;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T14;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T15;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T16;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T17;struct Cyc_Set_Set**_T18;struct Cyc_Set_Set**_T19;struct Cyc_Hashtable_Table*_T1A;
# 1758
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_assn_hash_cons_table!=0)goto _TL20F;
h=Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_assn_hash_cons_table=h;{
struct Cyc_Set_Set*ast=Cyc_Set_empty(Cyc_AssnDef_assncmp);
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();_T1=Cyc_Hashtable_insert;{
void(*_T1B)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))_T1;_T0=_T1B;}_T2=h;_T3=& Cyc_AssnDef_true_assn;_T4=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T3;_T5=(void*)_T4;{struct Cyc_AssnDef_AssnHashedInfo*_T1B=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_T7=& Cyc_AssnDef_true_assn;_T8=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T7;_T1B->assn=(void*)_T8;_T9=& Cyc_AssnDef_false_assn;_TA=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T9;_T1B->negation=(void*)_TA;{struct Cyc_Set_Set**_T1C=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T1C=ast;_TB=(struct Cyc_Set_Set**)_T1C;}_T1B->widened_set=_TB;{struct Cyc_Set_Set**_T1C=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T1C=tst;_TC=(struct Cyc_Set_Set**)_T1C;}_T1B->free_logicvar_set=_TC;_T6=(struct Cyc_AssnDef_AssnHashedInfo*)_T1B;}_T0(_T2,_T5,_T6);_TE=Cyc_Hashtable_insert;{
void(*_T1B)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))_TE;_TD=_T1B;}_TF=h;_T10=& Cyc_AssnDef_false_assn;_T11=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T10;_T12=(void*)_T11;{struct Cyc_AssnDef_AssnHashedInfo*_T1B=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_T14=& Cyc_AssnDef_false_assn;_T15=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T14;_T1B->assn=(void*)_T15;_T16=& Cyc_AssnDef_true_assn;_T17=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T16;_T1B->negation=(void*)_T17;{struct Cyc_Set_Set**_T1C=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T1C=0;_T18=(struct Cyc_Set_Set**)_T1C;}_T1B->widened_set=_T18;{struct Cyc_Set_Set**_T1C=_cycalloc(sizeof(struct Cyc_Set_Set*));*_T1C=tst;_T19=(struct Cyc_Set_Set**)_T1C;}_T1B->free_logicvar_set=_T19;_T13=(struct Cyc_AssnDef_AssnHashedInfo*)_T1B;}_TD(_TF,_T12,_T13);}goto _TL210;
# 1767
_TL20F: h=Cyc_AssnDef_assn_hash_cons_table;_TL210: _T1A=h;
# 1769
return _T1A;}
# 1771
static void*Cyc_AssnDef_hash_cons_assn(void*a){struct Cyc_AssnDef_AssnHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));void**(*_T1)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_Hashtable_Table*_T2;void*_T3;void(*_T4)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*);void(*_T5)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T6;void*_T7;struct Cyc_AssnDef_AssnHashedInfo*_T8;void(*_T9)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*);void(*_TA)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_TB;void*_TC;struct Cyc_AssnDef_AssnHashedInfo*_TD;void*_TE;struct Cyc_AssnDef_AssnHashedInfo**_TF;struct Cyc_AssnDef_AssnHashedInfo*_T10;void*_T11;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();_T1=Cyc_Hashtable_lookup_other_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T12)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))_T1;_T0=_T12;}_T2=h;_T3=a;{struct Cyc_AssnDef_AssnHashedInfo**resopt=_T0(_T2,_T3,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt!=0)goto _TL211;{
# 1777
void*a2=Cyc_AssnDef_copy_assn(a);
void*nega2=Cyc_AssnDef_neg_assn(a);_T5=Cyc_Hashtable_insert;{
# 1780
void(*_T12)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))_T5;_T4=_T12;}_T6=h;_T7=a2;{struct Cyc_AssnDef_AssnHashedInfo*_T12=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_T12->assn=a2;_T12->negation=nega2;_T12->widened_set=0;_T12->free_logicvar_set=0;_T8=(struct Cyc_AssnDef_AssnHashedInfo*)_T12;}_T4(_T6,_T7,_T8);_TA=Cyc_Hashtable_insert;{
void(*_T12)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))_TA;_T9=_T12;}_TB=h;_TC=nega2;{struct Cyc_AssnDef_AssnHashedInfo*_T12=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_T12->assn=nega2;_T12->negation=a2;_T12->widened_set=0;_T12->free_logicvar_set=0;_TD=(struct Cyc_AssnDef_AssnHashedInfo*)_T12;}_T9(_TB,_TC,_TD);_TE=a2;
# 1784
return _TE;}
# 1786
_TL211: _TF=resopt;_T10=*_TF;_T11=_T10->assn;return _T11;}}
# 1789
void*Cyc_AssnDef_and(void*a1,void*a2){void*_T0;struct _tuple15 _T1;void*_T2;int*_T3;int _T4;void*_T5;void*_T6;int*_T7;int _T8;void*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;int*_TF;int _T10;void*_T11;struct Cyc_AssnDef_And_AssnDef_Assn_struct _T12;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T13;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T14;void*_T15;void*_T16;
if(a1!=a2)goto _TL213;_T0=a1;return _T0;_TL213:{struct _tuple15 _T17;
_T17.f0=a1;_T17.f1=a2;_T1=_T17;}{struct _tuple15 _T17=_T1;_T2=_T17.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=0)goto _TL215;_T5=a2;
return _T5;_TL215: _T6=_T17.f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=0)goto _TL217;_T9=a1;
return _T9;_TL217: _TA=_T17.f0;_TB=(int*)_TA;_TC=*_TB;if(_TC!=1)goto _TL219;_TD=a1;
return _TD;_TL219: _TE=_T17.f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=1)goto _TL21B;_T11=a2;
return _T11;_TL21B:{struct Cyc_AssnDef_And_AssnDef_Assn_struct _T18;_T18.tag=3;
# 1799
_T18.f1=a1;_T18.f2=a2;_T12=_T18;}{struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_T12;_T13=& a;_T14=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T13;_T15=(void*)_T14;_T16=
Cyc_AssnDef_hash_cons_assn(_T15);return _T16;};}}
# 1804
void*Cyc_AssnDef_or(void*a1,void*a2){void*_T0;struct _tuple15 _T1;void*_T2;int*_T3;int _T4;void*_T5;void*_T6;int*_T7;int _T8;void*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;int*_TF;int _T10;void*_T11;struct Cyc_AssnDef_Or_AssnDef_Assn_struct _T12;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T13;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T14;void*_T15;void*_T16;
if(a1!=a2)goto _TL21D;_T0=a1;return _T0;_TL21D:{struct _tuple15 _T17;
# 1807
_T17.f0=a1;_T17.f1=a2;_T1=_T17;}{struct _tuple15 _T17=_T1;_T2=_T17.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=0)goto _TL21F;_T5=a1;
return _T5;_TL21F: _T6=_T17.f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=0)goto _TL221;_T9=a2;
return _T9;_TL221: _TA=_T17.f0;_TB=(int*)_TA;_TC=*_TB;if(_TC!=1)goto _TL223;_TD=a2;
return _TD;_TL223: _TE=_T17.f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=1)goto _TL225;_T11=a1;
return _T11;_TL225:{struct Cyc_AssnDef_Or_AssnDef_Assn_struct _T18;_T18.tag=4;
# 1815
_T18.f1=a1;_T18.f2=a2;_T12=_T18;}{struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_T12;_T13=& a;_T14=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T13;_T15=(void*)_T14;_T16=
Cyc_AssnDef_hash_cons_assn(_T15);return _T16;};}}
# 1829 "assndef.cyc"
static void*Cyc_AssnDef_reinsert(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;struct Cyc_AssnDef_AssnHashedInfo**(*_T7)(struct Cyc_Hashtable_Table*,void*);void**(*_T8)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T9;struct Cyc_AssnDef_AssnHashedInfo**(*_TA)(struct Cyc_Hashtable_Table*,void*);void**(*_TB)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_TC;struct Cyc_AssnDef_And_AssnDef_Assn_struct _TD;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_TE;struct Cyc_AssnDef_And_AssnDef_Assn_struct*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;struct Cyc_AssnDef_AssnHashedInfo**(*_T15)(struct Cyc_Hashtable_Table*,void*);void**(*_T16)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T17;struct Cyc_AssnDef_AssnHashedInfo**(*_T18)(struct Cyc_Hashtable_Table*,void*);void**(*_T19)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T1A;struct Cyc_AssnDef_Or_AssnDef_Assn_struct _T1B;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T1C;struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T1D;void*_T1E;void*_T1F;void*_T20;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();void*_T21;void*_T22;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 1: _LL4: _T3=a;
# 1833
return _T3;case 3: _T4=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T23=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T4;_T5=_T23->f1;_T22=(void*)_T5;_T6=_T23->f2;_T21=(void*)_T6;}{void*a1=_T22;void*a2=_T21;_T8=Cyc_Hashtable_lookup_opt;{
# 1835
struct Cyc_AssnDef_AssnHashedInfo**(*_T23)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T8;_T7=_T23;}_T9=_T7(h,a1);if(_T9!=0)goto _TL228;
a1=Cyc_AssnDef_reinsert(a1);goto _TL229;_TL228: _TL229: _TB=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T23)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_TB;_TA=_T23;}_TC=_TA(h,a2);if(_TC!=0)goto _TL22A;
a2=Cyc_AssnDef_reinsert(a2);goto _TL22B;_TL22A: _TL22B:{struct Cyc_AssnDef_And_AssnDef_Assn_struct _T23;_T23.tag=3;
_T23.f1=a1;_T23.f2=a2;_TD=_T23;}{struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_TD;_TE=& a;_TF=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_TE;_T10=(void*)_TF;_T11=
Cyc_AssnDef_hash_cons_assn(_T10);return _T11;}}case 4: _T12=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T23=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T12;_T13=_T23->f1;_T22=(void*)_T13;_T14=_T23->f2;_T21=(void*)_T14;}{void*a1=_T22;void*a2=_T21;_T16=Cyc_Hashtable_lookup_opt;{
# 1842
struct Cyc_AssnDef_AssnHashedInfo**(*_T23)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T16;_T15=_T23;}_T17=_T15(h,a1);if(_T17!=0)goto _TL22C;
a1=Cyc_AssnDef_reinsert(a1);goto _TL22D;_TL22C: _TL22D: _T19=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T23)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T19;_T18=_T23;}_T1A=_T18(h,a2);if(_T1A!=0)goto _TL22E;
a2=Cyc_AssnDef_reinsert(a2);goto _TL22F;_TL22E: _TL22F:{struct Cyc_AssnDef_Or_AssnDef_Assn_struct _T23;_T23.tag=4;
_T23.f1=a1;_T23.f2=a2;_T1B=_T23;}{struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_T1B;_T1C=& a;_T1D=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T1C;_T1E=(void*)_T1D;_T1F=
Cyc_AssnDef_hash_cons_assn(_T1E);return _T1F;}}default: _T20=
# 1849
Cyc_AssnDef_hash_cons_assn(a);return _T20;};}
# 1855
struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*a){struct Cyc_AssnDef_AssnHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**(*_T2)(struct Cyc_Hashtable_Table*,void*);void**(*_T3)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T4;struct Cyc_AssnDef_AssnHashedInfo*_T5;struct Cyc_Set_Set**_T6;struct Cyc_AssnDef_AssnHashedInfo**_T7;struct Cyc_AssnDef_AssnHashedInfo*_T8;struct Cyc_Set_Set**_T9;struct Cyc_Set_Set*_TA;void*_TB;int*_TC;unsigned _TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;struct Cyc_Set_Set*_T14;struct Cyc_Set_Set*_T15;void*_T16;void*_T17;void*_T18;struct Cyc_Set_Set*_T19;struct Cyc_Set_Set*_T1A;struct Cyc_AssnDef_AssnHashedInfo**_T1B;struct Cyc_AssnDef_AssnHashedInfo*_T1C;struct Cyc_Set_Set**_T1D;struct Cyc_Set_Set*_T1E;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T1F)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T1;_T0=_T1F;}{struct Cyc_AssnDef_AssnHashedInfo**ahinfo=_T0(h,a);
if(ahinfo!=0)goto _TL230;
a=Cyc_AssnDef_reinsert(a);_T3=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T1F)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T3;_T2=_T1F;}ahinfo=_T2(h,a);goto _TL231;_TL230: _TL231: _T4=
# 1862
_check_null(ahinfo);_T5=*_T4;_T6=_T5->free_logicvar_set;if(_T6==0)goto _TL232;_T7=ahinfo;_T8=*_T7;_T9=_T8->free_logicvar_set;_TA=*_T9;
return _TA;_TL232: {
struct Cyc_Set_Set*res;{void*_T1F;void*_T20;_TB=a;_TC=(int*)_TB;_TD=*_TC;switch(_TD){case 0: goto _LL4;case 1: _LL4:
# 1868
 res=Cyc_AssnDef_empty_term_set();goto _LL0;case 3: _TE=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T21=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_TE;_TF=_T21->f1;_T20=(void*)_TF;_T10=_T21->f2;_T1F=(void*)_T10;}{void*a1=_T20;void*a2=_T1F;_T20=a1;_T1F=a2;goto _LL8;}case 4: _T11=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T21=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T11;_T12=_T21->f1;_T20=(void*)_T12;_T13=_T21->f2;_T1F=(void*)_T13;}_LL8: {void*a1=_T20;void*a2=_T1F;_T14=
# 1873
Cyc_AssnDef_assn_fr_logicvar(a1);_T15=Cyc_AssnDef_assn_fr_logicvar(a2);res=Cyc_Set_union_two(_T14,_T15);goto _LL0;}default: _T16=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T21=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T16;_T17=_T21->f1;_T20=(void*)_T17;_T18=_T21->f3;_T1F=(void*)_T18;}{void*t1=_T20;void*t2=_T1F;_T19=
# 1876
Cyc_AssnDef_term_fr_logicvar(t1);_T1A=Cyc_AssnDef_term_fr_logicvar(t2);res=Cyc_Set_union_two(_T19,_T1A);goto _LL0;}}_LL0:;}_T1B=ahinfo;_T1C=*_T1B;{struct Cyc_Set_Set**_T1F=_cycalloc(sizeof(struct Cyc_Set_Set*));
# 1879
*_T1F=res;_T1D=(struct Cyc_Set_Set**)_T1F;}_T1C->free_logicvar_set=_T1D;_T1E=res;
return _T1E;}}}
# 1884
static int Cyc_AssnDef_logicvar_in_assn(void*lvar,void*a){int _T0;
struct Cyc_Set_Set*lvset=Cyc_AssnDef_assn_fr_logicvar(a);_T0=
Cyc_Set_member(lvset,lvar);return _T0;}
# 1891
void*Cyc_AssnDef_not(void*a){struct Cyc_AssnDef_AssnHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**(*_T2)(struct Cyc_Hashtable_Table*,void*);void**(*_T3)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T4;struct Cyc_AssnDef_AssnHashedInfo*_T5;void*_T6;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T7)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T1;_T0=_T7;}{struct Cyc_AssnDef_AssnHashedInfo**ahinfo=_T0(h,a);
if(ahinfo!=0)goto _TL235;
a=Cyc_AssnDef_reinsert(a);_T3=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T7)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T3;_T2=_T7;}ahinfo=_T2(h,a);goto _TL236;_TL235: _TL236: _T4=
# 1898
_check_null(ahinfo);_T5=*_T4;_T6=_T5->negation;return _T6;}}
# 1901
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _T0;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T1;struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2;void*_T3;void*_T4;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _T5;_T5.tag=2;
_T5.f1=t1;_T5.f2=p;_T5.f3=t2;_T0=_T5;}{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=_T0;_T1=& ptr;_T2=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T1;_T3=(void*)_T2;_T4=
Cyc_AssnDef_hash_cons_assn(_T3);return _T4;}}
# 1906
void*Cyc_AssnDef_eq(void*t1,void*t2){struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1;void*_T2;int _T3;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T4;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T5;void*_T6;int _T7;void*_T8;
if(t1!=t2)goto _TL237;_T0=& Cyc_AssnDef_true_assn;_T1=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL237: _T3=
Cyc_AssnDef_term_neq(t1,t2);if(!_T3)goto _TL239;_T4=& Cyc_AssnDef_false_assn;_T5=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T4;_T6=(void*)_T5;return _T6;_TL239: _T7=
Cyc_AssnDef_termcmp(t1,t2);if(_T7 <= 0)goto _TL23B;{
void*_T9=t1;void*_TA=t2;t1=_TA;t2=_T9;}goto _TL23C;_TL23B: _TL23C: _T8=
# 1912
Cyc_AssnDef_prim(t1,0U,t2);return _T8;}
# 1915
void*Cyc_AssnDef_slt(void*t1,void*t2){struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1;void*_T2;struct _tuple15 _T3;void*_T4;int*_T5;int _T6;void*_T7;int*_T8;int _T9;void*_TA;void*_TB;unsigned _TC;int _TD;unsigned _TE;int _TF;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T10;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T11;void*_T12;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T13;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T14;void*_T15;void*_T16;
if(t1!=t2)goto _TL23D;_T0=& Cyc_AssnDef_false_assn;_T1=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL23D:{struct _tuple15 _T17;
_T17.f0=t1;_T17.f1=t2;_T3=_T17;}{struct _tuple15 _T17=_T3;unsigned _T18;unsigned _T19;_T4=_T17.f0;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL23F;_T7=_T17.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL241;_TA=_T17.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1A=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TA;_T19=_T1A->f1;}_TB=_T17.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1A=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TB;_T18=_T1A->f1;}{unsigned i=_T19;unsigned j=_T18;_TC=i;_TD=(int)_TC;_TE=j;_TF=(int)_TE;
# 1919
if(_TD >= _TF)goto _TL243;_T10=& Cyc_AssnDef_true_assn;_T11=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T10;_T12=(void*)_T11;return _T12;_TL243: _T13=& Cyc_AssnDef_false_assn;_T14=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T13;_T15=(void*)_T14;return _T15;}_TL241: goto _LL3;_TL23F: _LL3: _T16=
Cyc_AssnDef_prim(t1,2U,t2);return _T16;;}}
# 1924
void*Cyc_AssnDef_slte(void*t1,void*t2){struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1;void*_T2;struct _tuple15 _T3;void*_T4;int*_T5;int _T6;void*_T7;int*_T8;int _T9;void*_TA;void*_TB;unsigned _TC;int _TD;unsigned _TE;int _TF;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T10;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T11;void*_T12;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T13;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T14;void*_T15;void*_T16;
if(t1!=t2)goto _TL245;_T0=& Cyc_AssnDef_true_assn;_T1=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL245:{struct _tuple15 _T17;
_T17.f0=t1;_T17.f1=t2;_T3=_T17;}{struct _tuple15 _T17=_T3;unsigned _T18;unsigned _T19;_T4=_T17.f0;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL247;_T7=_T17.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL249;_TA=_T17.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1A=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TA;_T19=_T1A->f1;}_TB=_T17.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1A=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TB;_T18=_T1A->f1;}{unsigned i=_T19;unsigned j=_T18;_TC=i;_TD=(int)_TC;_TE=j;_TF=(int)_TE;
# 1928
if(_TD > _TF)goto _TL24B;_T10=& Cyc_AssnDef_true_assn;_T11=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T10;_T12=(void*)_T11;return _T12;_TL24B: _T13=& Cyc_AssnDef_false_assn;_T14=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T13;_T15=(void*)_T14;return _T15;}_TL249: goto _LL3;_TL247: _LL3: _T16=
Cyc_AssnDef_prim(t1,3U,t2);return _T16;;}}
# 1933
void*Cyc_AssnDef_ult(void*t1,void*t2){struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1;void*_T2;struct _tuple15 _T3;void*_T4;int*_T5;int _T6;void*_T7;int*_T8;int _T9;void*_TA;void*_TB;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TC;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TD;void*_TE;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TF;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T10;void*_T11;void*_T12;
if(t1!=t2)goto _TL24D;_T0=& Cyc_AssnDef_false_assn;_T1=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL24D:{struct _tuple15 _T13;
_T13.f0=t1;_T13.f1=t2;_T3=_T13;}{struct _tuple15 _T13=_T3;unsigned _T14;unsigned _T15;_T4=_T13.f0;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL24F;_T7=_T13.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL251;_TA=_T13.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T16=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TA;_T15=_T16->f1;}_TB=_T13.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T16=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TB;_T14=_T16->f1;}{unsigned i=_T15;unsigned j=_T14;
# 1937
if(i >= j)goto _TL253;_TC=& Cyc_AssnDef_true_assn;_TD=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_TC;_TE=(void*)_TD;return _TE;_TL253: _TF=& Cyc_AssnDef_false_assn;_T10=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TF;_T11=(void*)_T10;return _T11;}_TL251: goto _LL3;_TL24F: _LL3: _T12=
Cyc_AssnDef_prim(t1,4U,t2);return _T12;;}}
# 1942
void*Cyc_AssnDef_ulte(void*t1,void*t2){struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1;void*_T2;struct _tuple15 _T3;void*_T4;int*_T5;int _T6;void*_T7;int*_T8;int _T9;void*_TA;void*_TB;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TC;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TD;void*_TE;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TF;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T10;void*_T11;void*_T12;
if(t1!=t2)goto _TL255;_T0=& Cyc_AssnDef_true_assn;_T1=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL255:{struct _tuple15 _T13;
_T13.f0=t1;_T13.f1=t2;_T3=_T13;}{struct _tuple15 _T13=_T3;unsigned _T14;unsigned _T15;_T4=_T13.f0;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL257;_T7=_T13.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL259;_TA=_T13.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T16=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TA;_T15=_T16->f1;}_TB=_T13.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T16=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TB;_T14=_T16->f1;}{unsigned i=_T15;unsigned j=_T14;
# 1946
if(i > j)goto _TL25B;_TC=& Cyc_AssnDef_true_assn;_TD=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_TC;_TE=(void*)_TD;return _TE;_TL25B: _TF=& Cyc_AssnDef_false_assn;_T10=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TF;_T11=(void*)_T10;return _T11;}_TL259: goto _LL3;_TL257: _LL3: _T12=
Cyc_AssnDef_prim(t1,5U,t2);return _T12;;}}
# 1951
void*Cyc_AssnDef_neq(void*t1,void*t2){struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1;void*_T2;int _T3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T4;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T5;void*_T6;struct _tuple15 _T7;void*_T8;int*_T9;int _TA;void*_TB;unsigned _TC;void*_TD;void*_TE;void*_TF;void*_T10;int*_T11;int _T12;void*_T13;int*_T14;int _T15;void*_T16;unsigned _T17;void*_T18;void*_T19;void*_T1A;int _T1B;void*_T1C;
if(t1!=t2)goto _TL25D;_T0=& Cyc_AssnDef_false_assn;_T1=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL25D: _T3=
Cyc_AssnDef_term_neq(t1,t2);if(!_T3)goto _TL25F;_T4=& Cyc_AssnDef_true_assn;_T5=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T4;_T6=(void*)_T5;return _T6;_TL25F:{struct _tuple15 _T1D;
_T1D.f0=t1;_T1D.f1=t2;_T7=_T1D;}{struct _tuple15 _T1D=_T7;unsigned _T1E;_T8=_T1D.f0;_T9=(int*)_T8;_TA=*_T9;if(_TA!=0)goto _TL261;_TB=_T1D.f0;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1F=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_TB;_T1E=_T1F->f1;}_TC=(unsigned)_T1E;if(_TC!=0U)goto _TL263;{unsigned i=_T1E;_TD=
# 1956
Cyc_AssnDef_zero();_TE=t2;_TF=Cyc_AssnDef_ult(_TD,_TE);return _TF;}_TL263: _T10=_T1D.f1;_T11=(int*)_T10;_T12=*_T11;if(_T12!=0)goto _TL265;goto _LL3;_TL265: goto _LL5;_TL261: _T13=_T1D.f1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=0)goto _TL267;_LL3: _T16=_T1D.f1;{struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T1F=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T16;_T1E=_T1F->f1;}_T17=(unsigned)_T1E;if(_T17!=0U)goto _TL269;{unsigned j=_T1E;_T18=
# 1958
Cyc_AssnDef_zero();_T19=t1;_T1A=Cyc_AssnDef_ult(_T18,_T19);return _T1A;}_TL269: goto _LL5;_TL267: _LL5: _T1B=
# 1960
Cyc_AssnDef_termcmp(t1,t2);if(_T1B <= 0)goto _TL26B;{
void*_T1F=t1;void*_T20=t2;t1=_T20;t2=_T1F;}goto _TL26C;_TL26B: _TL26C: _T1C=
# 1963
Cyc_AssnDef_prim(t1,1U,t2);return _T1C;;}}
# 1967
struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){struct Cyc_AssnDef_AssnHashedInfo**(*_T0)(struct Cyc_Hashtable_Table*,void*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**(*_T2)(struct Cyc_Hashtable_Table*,void*);void**(*_T3)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo**_T4;struct Cyc_AssnDef_AssnHashedInfo*_T5;struct Cyc_Set_Set**_T6;struct Cyc_AssnDef_AssnHashedInfo**_T7;struct Cyc_AssnDef_AssnHashedInfo*_T8;struct Cyc_Set_Set**_T9;struct Cyc_Set_Set*_TA;void*_TB;int*_TC;unsigned _TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;int _T18;int _T19;struct Cyc_AssnDef_AssnHashedInfo*(*_T1A)(struct Cyc_Hashtable_Table*,void*);void*(*_T1B)(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_AssnHashedInfo*_T1C;struct Cyc_Set_Set**_T1D;struct Cyc_Set_Set**_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;enum Cyc_AssnDef_Primreln _T23;int _T24;int _T25;int _T26;struct Cyc_Set_Set*_T27;void*_T28;struct Cyc_Set_Set*_T29;void*_T2A;struct Cyc_Set_Set*_T2B;void*_T2C;struct Cyc_Set_Set*_T2D;void*_T2E;struct Cyc_Set_Set*_T2F;void*_T30;struct Cyc_Set_Set*_T31;void*_T32;struct Cyc_AssnDef_AssnHashedInfo**_T33;struct Cyc_AssnDef_AssnHashedInfo*_T34;struct Cyc_Set_Set**_T35;struct Cyc_Set_Set*_T36;
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T37)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T1;_T0=_T37;}{struct Cyc_AssnDef_AssnHashedInfo**ahinfo=_T0(h,a);
if(ahinfo!=0)goto _TL26D;
a=Cyc_AssnDef_reinsert(a);_T3=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnHashedInfo**(*_T37)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))_T3;_T2=_T37;}ahinfo=_T2(h,a);goto _TL26E;_TL26D: _TL26E: _T4=
# 1974
_check_null(ahinfo);_T5=*_T4;_T6=_T5->widened_set;if(_T6==0)goto _TL26F;_T7=ahinfo;_T8=*_T7;_T9=_T8->widened_set;_TA=*_T9;return _TA;_TL26F: {
struct Cyc_Set_Set*s;{enum Cyc_AssnDef_Primreln _T37;void*_T38;void*_T39;_TB=a;_TC=(int*)_TB;_TD=*_TC;switch(_TD){case 0: goto _LL4;case 1: _LL4: _TF=Cyc_Warn_impos;{
# 1979
int(*_T3A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T3A;}_T10=_tag_fat("true and false should have widened_set",sizeof(char),39U);_T11=_tag_fat(0U,sizeof(void*),0);_TE(_T10,_T11);case 3: _T12=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T12;_T13=_T3A->f1;_T39=(void*)_T13;_T14=_T3A->f2;_T38=(void*)_T14;}{void*a1=_T39;void*a2=_T38;
# 1981
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)goto _TL274;else{goto _TL275;}_TL275: if(s2==0)goto _TL274;else{goto _TL272;}
_TL274: s=0;goto _TL273;
# 1986
_TL272: s=Cyc_Set_union_two(s1,s2);_TL273: goto _LL0;}case 4: _T15=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T15;_T16=_T3A->f1;_T39=(void*)_T16;_T17=_T3A->f2;_T38=(void*)_T17;}{void*a1=_T39;void*a2=_T38;
# 1989
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1!=0)goto _TL276;
s=s2;goto _TL277;
_TL276: if(s2!=0)goto _TL278;
s=s1;goto _TL279;
# 1997
_TL278: s=Cyc_Set_intersect(s1,s2);_T18=Cyc_Flags_better_widen;
# 2000
if(!_T18)goto _TL27A;{
struct Cyc_Set_Set*s1diff=Cyc_Set_diff(s1,s);_T19=
Cyc_Set_is_empty(s1diff);if(_T19)goto _TL27C;else{goto _TL27E;}
_TL27E:{struct Cyc_Set_Set*other_s=Cyc_PrattProver_filter_implies(s2,s1);
if(other_s==0)goto _TL27F;{
struct Cyc_Set_Set*diff=Cyc_Set_diff(s1,other_s);
s=Cyc_Set_union_two(s,diff);}goto _TL280;
# 2010
_TL27F: _T1B=Cyc_Hashtable_lookup;{struct Cyc_AssnDef_AssnHashedInfo*(*_T3A)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo*(*)(struct Cyc_Hashtable_Table*,void*))_T1B;_T1A=_T3A;}{struct Cyc_AssnDef_AssnHashedInfo*a2hinfo=_T1A(h,a2);_T1C=a2hinfo;_T1D=_T1C->widened_set;_T1E=
_check_null(_T1D);*_T1E=0;
s=s1;}_TL280:;}goto _TL27D;_TL27C: _TL27D:;}goto _TL27B;_TL27A: _TL27B: _TL279: _TL277: goto _LL0;}default: _T1F=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T3A=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T1F;_T20=_T3A->f1;_T39=(void*)_T20;_T37=_T3A->f2;_T21=_T3A->f3;_T38=(void*)_T21;}{void*t1=_T39;enum Cyc_AssnDef_Primreln p=_T37;void*t2=_T38;_T22=a;
# 2019
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,_T22);_T23=p;_T24=(int)_T23;switch(_T24){case Cyc_AssnDef_Eq:  {
# 2025
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0)goto _TL286;else{goto _TL285;}_TL286: _T25=Cyc_Tcutil_is_integral_type(topt1);if(_T25)goto _TL284;else{goto _TL285;}_TL285: if(topt2!=0)goto _TL287;else{goto _TL282;}_TL287: _T26=
Cyc_Tcutil_is_integral_type(topt2);
# 2027
if(_T26)goto _TL284;else{goto _TL282;}
# 2029
_TL284: _T27=s;_T28=Cyc_AssnDef_slte(t1,t2);s=Cyc_Set_insert(_T27,_T28);_T29=s;_T2A=
Cyc_AssnDef_slte(t2,t1);s=Cyc_Set_insert(_T29,_T2A);_T2B=s;_T2C=
Cyc_AssnDef_ulte(t1,t2);s=Cyc_Set_insert(_T2B,_T2C);_T2D=s;_T2E=
Cyc_AssnDef_ulte(t2,t1);s=Cyc_Set_insert(_T2D,_T2E);goto _TL283;_TL282: _TL283: goto _LLB;}case Cyc_AssnDef_SLt: _T2F=s;_T30=
# 2036
Cyc_AssnDef_slte(t1,t2);s=Cyc_Set_insert(_T2F,_T30);goto _LLB;case Cyc_AssnDef_ULt: _T31=s;_T32=
# 2039
Cyc_AssnDef_ulte(t1,t2);s=Cyc_Set_insert(_T31,_T32);goto _LLB;default: goto _LLB;}_LLB: goto _LL0;}}_LL0:;}_T33=ahinfo;_T34=*_T33;{struct Cyc_Set_Set**_T37=_cycalloc(sizeof(struct Cyc_Set_Set*));
# 2045
*_T37=s;_T35=(struct Cyc_Set_Set**)_T37;}_T34->widened_set=_T35;_T36=s;
return _T36;}}}
# 2050
void*Cyc_AssnDef_widen(void*a){struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T0;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1;void*_T2;struct Cyc_Set_Set*_T3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T4;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T5;void*_T6;void*_T7;
# 2052
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt!=0)goto _TL288;_T0=& Cyc_AssnDef_false_assn;_T1=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T0;_T2=(void*)_T1;return _T2;_TL288: _T3=sopt;_T4=& Cyc_AssnDef_true_assn;_T5=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T4;_T6=(void*)_T5;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,_T3,_T6);_T7=res;
return _T7;}}
# 2058
static void*Cyc_AssnDef_subst_assn(struct Cyc_Dict_Dict dict,void*a,struct Cyc_Hashtable_Table*assn_h,struct Cyc_Hashtable_Table*term_h){void**_T0;void*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;void*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;void*_TD;void*_TE;
# 2061
void**a1=Cyc_Hashtable_lookup_opt(assn_h,a);
if(a1==0)goto _TL28A;_T0=a1;_T1=*_T0;
# 2067
return _T1;_TL28A: {
# 2069
void*res;{void*_TF;enum Cyc_AssnDef_Primreln _T10;void*_T11;_T2=a;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 0: goto _LL4;case 1: _LL4:
# 2072
 res=a;goto _LL0;case 2: _T5=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T12=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T5;_T6=_T12->f1;_T11=(void*)_T6;_T10=_T12->f2;_T7=_T12->f3;_TF=(void*)_T7;}{void*t1=_T11;enum Cyc_AssnDef_Primreln p=_T10;void*t2=_TF;
# 2074
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_prim(s1,p,s2);goto _LL0;}case 3: _T8=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T12=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T8;_T9=_T12->f1;_T11=(void*)_T9;_TA=_T12->f2;_TF=(void*)_TA;}{void*a1=_T11;void*a2=_TF;
# 2079
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_and(b1,b2);goto _LL0;}default: _TB=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T12=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_TB;_TC=_T12->f1;_T11=(void*)_TC;_TD=_T12->f2;_TF=(void*)_TD;}{void*a1=_T11;void*a2=_TF;
# 2084
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_or(b1,b2);goto _LL0;}}_LL0:;}
# 2089
Cyc_Hashtable_insert(assn_h,a,res);_TE=res;
return _TE;}}
# 2093
void*Cyc_AssnDef_subst_a(struct Cyc_Dict_Dict dict,void*a){int(*_T0)(void*,void*);int(*_T1)(void*,void*);void*_T2;_T0=Cyc_Core_ptrcmp;{
# 2096
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,_T0,Cyc_AssnDef_hash_ptr);_T1=Cyc_Core_ptrcmp;{
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,_T1,Cyc_AssnDef_hash_ptr);_T2=
Cyc_AssnDef_subst_assn(dict,a,assn_h,term_h);return _T2;}}}
# 2113 "assndef.cyc"
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;int _T6;enum Cyc_AssnDef_Primreln _T7;int _T8;int _T9;struct Cyc_Set_Set*_TA;void*_TB;int _TC;struct Cyc_Set_Set*_TD;void*_TE;int _TF;struct Cyc_Set_Set*_T10;void*_T11;int _T12;struct Cyc_Set_Set*_T13;void*_T14;int _T15;struct Cyc_Set_Set*_T16;void*_T17;int _T18;struct Cyc_Set_Set*_T19;void*_T1A;int _T1B;struct Cyc_Set_Set*_T1C;void*_T1D;int _T1E;struct Cyc_Set_Set*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;int _T24;void*_T25;void*_T26;void*_T27;int _T28;
_TL28D: if(1)goto _TL28E;else{goto _TL28F;}
_TL28E:{void*_T29;enum Cyc_AssnDef_Primreln _T2A;void*_T2B;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=a;{struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_T2C=(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_T3;_T4=_T2C->f1;_T2B=(void*)_T4;_T2A=_T2C->f2;_T5=_T2C->f3;_T29=(void*)_T5;}{void*t1=_T2B;enum Cyc_AssnDef_Primreln p=_T2A;void*t2=_T29;_T6=
# 2117
Cyc_Set_member(ctxt,a);if(!_T6)goto _TL291;return 1;_TL291: _T7=p;_T8=(int)_T7;switch(_T8){case Cyc_AssnDef_Neq: _TA=ctxt;_TB=
# 2126
Cyc_AssnDef_ult(t2,t1);_TC=Cyc_Set_member(_TA,_TB);if(_TC)goto _TL296;else{goto _TL298;}_TL298: _TD=ctxt;_TE=
Cyc_AssnDef_ult(t1,t2);_TF=Cyc_Set_member(_TD,_TE);
# 2126
if(_TF)goto _TL296;else{goto _TL297;}_TL297: _T10=ctxt;_T11=
# 2128
Cyc_AssnDef_slt(t2,t1);_T12=Cyc_Set_member(_T10,_T11);
# 2126
if(_T12)goto _TL296;else{goto _TL294;}_TL296: _T9=1;goto _TL295;_TL294: _T13=ctxt;_T14=
# 2129
Cyc_AssnDef_slt(t1,t2);_T9=Cyc_Set_member(_T13,_T14);_TL295:
# 2126
 return _T9;case Cyc_AssnDef_SLte: _T16=ctxt;_T17=
# 2131
Cyc_AssnDef_slt(t1,t2);_T18=Cyc_Set_member(_T16,_T17);if(!_T18)goto _TL299;_T15=1;goto _TL29A;_TL299: _T19=ctxt;_T1A=
Cyc_AssnDef_eq(t1,t2);_T15=Cyc_Set_member(_T19,_T1A);_TL29A:
# 2131
 return _T15;case Cyc_AssnDef_ULte: _T1C=ctxt;_T1D=
# 2134
Cyc_AssnDef_ult(t1,t2);_T1E=Cyc_Set_member(_T1C,_T1D);if(!_T1E)goto _TL29B;_T1B=1;goto _TL29C;_TL29B: _T1F=ctxt;_T20=
Cyc_AssnDef_eq(t1,t2);_T1B=Cyc_Set_member(_T1F,_T20);_TL29C:
# 2134
 return _T1B;default:
# 2136
 return 0;};}case 0:
# 2138
 return 1;case 1:
 return 0;case 3: _T21=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_T2C=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T21;_T22=_T2C->f1;_T2B=(void*)_T22;_T23=_T2C->f2;_T29=(void*)_T23;}{void*a1=_T2B;void*a2=_T29;_T24=
# 2141
Cyc_AssnDef_simple_prv(ctxt,a1);if(_T24)goto _TL29D;else{goto _TL29F;}
_TL29F: return 0;_TL29D:
# 2147
 a=a2;goto _TL28D;}default: _T25=a;{struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_T2C=(struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_T25;_T26=_T2C->f1;_T2B=(void*)_T26;_T27=_T2C->f2;_T29=(void*)_T27;}{void*a1=_T2B;void*a2=_T29;_T28=
# 2150
Cyc_AssnDef_simple_prv(ctxt,a1);if(!_T28)goto _TL2A0;return 1;_TL2A0:
 a=a2;goto _TL28D;}};}goto _TL28D;_TL28F:;}
# 2160
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){int _T0;
if(ctxt!=a)goto _TL2A2;return 1;_TL2A2: {
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt!=0)goto _TL2A4;return 1;_TL2A4: _T0=
Cyc_AssnDef_simple_prv(sopt,a);return _T0;}}
# 2167
struct Cyc_Set_Set*Cyc_AssnDef_assnfn_fr_logicvar(struct Cyc_AssnDef_AssnFn*af){struct Cyc_AssnDef_AssnFn*_T0;void*_T1;struct Cyc_AssnDef_AssnFn*_T2;struct Cyc_Set_Set*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_Set_Set*_T7;_T0=af;_T1=_T0->assn;{
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assn_fr_logicvar(_T1);_T2=af;{
struct Cyc_List_List*as=_T2->actuals;_TL2A9: if(as!=0)goto _TL2A7;else{goto _TL2A8;}
_TL2A7: _T3=fvs;_T4=as;_T5=_T4->hd;fvs=Cyc_Set_delete(_T3,_T5);_T6=as;
# 2169
as=_T6->tl;goto _TL2A9;_TL2A8:;}_T7=fvs;
# 2171
return _T7;}}
# 2174
static struct _fat_ptr Cyc_AssnDef_assnfn2string(struct Cyc_AssnDef_AssnFn*af){struct Cyc_AssnDef_AssnFn*_T0;struct Cyc_List_List*_T1;struct Cyc_AssnDef_AssnFn*_T2;void*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_AssnDef_AssnFn*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_AssnDef_AssnFn*_TB;struct Cyc_AssnDef_AssnFn*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_AssnDef_AssnFn*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;_T0=af;_T1=_T0->actuals;
if(_T1!=0)goto _TL2AA;_T2=af;_T3=_T2->assn;_T4=Cyc_AssnDef_assn2string(_T3);return _T4;_TL2AA:{struct Cyc_String_pa_PrintArg_struct _T12;_T12.tag=0;_T7=af;_T8=_T7->actuals;_T9=_T8->hd;
_T12.f1=Cyc_AssnDef_term2string(_T9);_T6=_T12;}{struct Cyc_String_pa_PrintArg_struct _T12=_T6;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;{struct Cyc_AssnDef_AssnFn*_T14=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_TC=af;_TD=_TC->actuals;_TE=_check_null(_TD);_T14->actuals=_TE->tl;_TF=af;_T14->assn=_TF->assn;_TB=(struct Cyc_AssnDef_AssnFn*)_T14;}_T13.f1=Cyc_AssnDef_assnfn2string(_TB);_TA=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_TA;void*_T14[2];_T14[0]=& _T12;_T14[1]=& _T13;_T10=_tag_fat("/\\ %s .\n%s",sizeof(char),11U);_T11=_tag_fat(_T14,sizeof(void*),2);_T5=Cyc_aprintf(_T10,_T11);}}return _T5;}
# 2178
struct _fat_ptr Cyc_AssnDef_existassnfn2string(struct Cyc_AssnDef_ExistAssnFn*eaf){struct Cyc_AssnDef_ExistAssnFn*_T0;struct Cyc_AssnDef_AssnFn*_T1;struct _fat_ptr _T2;_T0=eaf;_T1=_T0->af;_T2=
Cyc_AssnDef_assnfn2string(_T1);return _T2;}
# 2184
struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_assnmap2existassnfn(struct Cyc_AssnDef_AssnMap am,struct Cyc_List_List*vds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Vardecl*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_AssnDef_AssnMap _T6;void*_T7;int*_T8;int _T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Absyn_Vardecl*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_Absyn_Vardecl*_T17;void*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_AssnDef_AssnFn*_T1B;struct Cyc_AssnDef_AssnFn*_T1C;struct Cyc_AssnDef_AssnMap _T1D;struct Cyc_AssnDef_ExistAssnFn*_T1E;
struct Cyc_List_List*actuals=0;
_TL2AF: if(vds!=0)goto _TL2AD;else{goto _TL2AE;}
_TL2AD: _T0=vds;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Vardecl*)_T1;if(_T2==0)goto _TL2B0;_T3=vds;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Vardecl*)_T4;_T6=am;{
struct _tuple13 _T1F=Cyc_AssnDef_lookup_var_map(_T5,_T6);void*_T20;_T20=_T1F.f0;{void*v=_T20;_T7=v;_T8=(int*)_T7;_T9=*_T8;if(_T9!=2)goto _TL2B2;goto _LL3;_TL2B2: _TB=Cyc_Warn_impos;{
# 2191
int(*_T21)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T21;}_TC=_tag_fat("term to be bound is not a logic var",sizeof(char),36U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);_LL3:{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
# 2193
_T21->hd=v;_T21->tl=actuals;_TE=(struct Cyc_List_List*)_T21;}actuals=_TE;}}goto _TL2B1;
# 2195
_TL2B0:{void*x;_TF=vds;_T10=_TF->hd;_T11=(struct Cyc_Absyn_Vardecl*)_T10;
if(_T11!=0)goto _TL2B4;
x=Cyc_AssnDef_fresh_var(0,0);goto _TL2B5;
# 2199
_TL2B4: _T12=vds;_T13=_T12->hd;_T14=(struct Cyc_Absyn_Vardecl*)_T13;_T15=vds;_T16=_T15->hd;_T17=(struct Cyc_Absyn_Vardecl*)_T16;_T18=_T17->type;x=Cyc_AssnDef_fresh_var(_T14,_T18);_TL2B5:{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));
_T1F->hd=x;_T1F->tl=actuals;_T19=(struct Cyc_List_List*)_T1F;}actuals=_T19;}_TL2B1: _T1A=vds;
# 2186
vds=_T1A->tl;goto _TL2AF;_TL2AE:
# 2203
 actuals=Cyc_List_imp_rev(actuals);{
# 2205
struct Cyc_AssnDef_AssnFn*af;af=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_T1B=af;_T1B->actuals=actuals;_T1C=af;_T1D=am;_T1C->assn=_T1D.assn;{
# 2207
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assnfn_fr_logicvar(af);{struct Cyc_AssnDef_ExistAssnFn*_T1F=_cycalloc(sizeof(struct Cyc_AssnDef_ExistAssnFn));
_T1F->af=af;_T1F->existvars=fvs;_T1E=(struct Cyc_AssnDef_ExistAssnFn*)_T1F;}return _T1E;}}}
# 2215
struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_bound_ts_in_assn(void*a,struct Cyc_List_List*ts){struct Cyc_List_List*_T0;int*_T1;int _T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_List_List*_T5;void*_T6;int _T7;struct Cyc_List_List*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;void*_TF;void*_T10;struct Cyc_List_List*_T11;void*_T12;void*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;void*_T17;void*_T18;void*_T19;struct Cyc_List_List*_T1A;void*_T1B;void*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_AssnDef_AssnFn*_T1F;struct Cyc_AssnDef_AssnFn*_T20;struct Cyc_AssnDef_ExistAssnFn*_T21;
# 2217
struct Cyc_List_List*vs=0;
_TL2B9: if(ts!=0)goto _TL2B7;else{goto _TL2B8;}
_TL2B7: _T0=ts;{void*_T22=_T0->hd;_T1=(int*)_T22;_T2=*_T1;if(_T2!=2)goto _TL2BA;{
# 2223
int occured=0;{
struct Cyc_List_List*xs=vs;_TL2BF: if(xs!=0)goto _TL2BD;else{goto _TL2BE;}
_TL2BD: _T3=ts;_T4=_T3->hd;_T5=xs;_T6=_T5->hd;_T7=Cyc_AssnDef_termcmp(_T4,_T6);if(_T7!=0)goto _TL2C0;
occured=1;goto _TL2BE;_TL2C0: _T8=xs;
# 2224
xs=_T8->tl;goto _TL2BF;_TL2BE:;}_T9=occured;
# 2230
if(_T9)goto _TL2C2;else{goto _TL2C4;}_TL2C4:{struct Cyc_List_List*_T23=_cycalloc(sizeof(struct Cyc_List_List));_TB=ts;_T23->hd=_TB->hd;_T23->tl=vs;_TA=(struct Cyc_List_List*)_T23;}vs=_TA;goto _TL2C3;
# 2232
_TL2C2: _TC=ts;_TD=_TC->hd;_TE=Cyc_AssnDef_get_term_type(_TD);{void*v=Cyc_AssnDef_fresh_var(0,_TE);_TF=a;_T10=v;_T11=ts;_T12=_T11->hd;_T13=
Cyc_AssnDef_eq(_T10,_T12);a=Cyc_AssnDef_and(_TF,_T13);{struct Cyc_List_List*_T23=_cycalloc(sizeof(struct Cyc_List_List));
_T23->hd=v;_T23->tl=vs;_T14=(struct Cyc_List_List*)_T23;}vs=_T14;}_TL2C3: goto _LL0;}_TL2BA: _T15=ts;_T16=_T15->hd;_T17=
# 2238
Cyc_AssnDef_get_term_type(_T16);{void*v=Cyc_AssnDef_fresh_var(0,_T17);_T18=a;_T19=v;_T1A=ts;_T1B=_T1A->hd;_T1C=
Cyc_AssnDef_eq(_T19,_T1B);a=Cyc_AssnDef_and(_T18,_T1C);{struct Cyc_List_List*_T23=_cycalloc(sizeof(struct Cyc_List_List));
_T23->hd=v;_T23->tl=vs;_T1D=(struct Cyc_List_List*)_T23;}vs=_T1D;goto _LL0;}_LL0:;}_T1E=ts;
# 2218
ts=_T1E->tl;goto _TL2B9;_TL2B8:
# 2244
 vs=Cyc_List_imp_rev(vs);{
# 2246
struct Cyc_AssnDef_AssnFn*af;af=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_T1F=af;_T1F->actuals=vs;_T20=af;_T20->assn=a;{
# 2248
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assnfn_fr_logicvar(af);{struct Cyc_AssnDef_ExistAssnFn*_T22=_cycalloc(sizeof(struct Cyc_AssnDef_ExistAssnFn));
_T22->af=af;_T22->existvars=fvs;_T21=(struct Cyc_AssnDef_ExistAssnFn*)_T22;}return _T21;}}}
# 2252
static struct Cyc_Dict_Dict*Cyc_AssnDef_insertnewmap(void*t,struct Cyc_Dict_Dict*dict){struct Cyc_Dict_Dict*_T0;struct Cyc_Dict_Dict*_T1;struct Cyc_Dict_Dict _T2;void*_T3;void*_T4;void*_T5;struct Cyc_Dict_Dict*_T6;_T0=dict;_T1=dict;_T2=*_T1;_T3=t;_T4=
Cyc_AssnDef_get_term_type(t);_T5=Cyc_AssnDef_fresh_var(0,_T4);*_T0=Cyc_Dict_insert(_T2,_T3,_T5);_T6=dict;
return _T6;}
# 2259
void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*eaf,struct Cyc_List_List*ts){struct Cyc_Dict_Dict*(*_T0)(struct Cyc_Dict_Dict*(*)(void*,struct Cyc_Dict_Dict*),struct Cyc_Set_Set*,struct Cyc_Dict_Dict*);void*(*_T1)(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_ExistAssnFn*_T2;struct Cyc_Set_Set*_T3;struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict*_T5;struct Cyc_Dict_Dict*_T6;struct Cyc_AssnDef_ExistAssnFn*_T7;struct Cyc_AssnDef_AssnFn*_T8;struct Cyc_AssnDef_ExistAssnFn*_T9;struct Cyc_AssnDef_AssnFn*_TA;struct Cyc_Dict_Dict _TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;int(*_T12)(struct _fat_ptr,struct _fat_ptr);void*(*_T13)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T14;struct _fat_ptr _T15;void*_T16;
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();_T1=Cyc_Set_fold;{
# 2262
struct Cyc_Dict_Dict*(*_T17)(struct Cyc_Dict_Dict*(*)(void*,struct Cyc_Dict_Dict*),struct Cyc_Set_Set*,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(void*,struct Cyc_Dict_Dict*),struct Cyc_Set_Set*,struct Cyc_Dict_Dict*))_T1;_T0=_T17;}_T2=eaf;_T3=_T2->existvars;_T4=& dict;_T5=(struct Cyc_Dict_Dict*)_T4;_T6=_T0(Cyc_AssnDef_insertnewmap,_T3,_T5);dict=*_T6;_T7=eaf;_T8=_T7->af;{
struct Cyc_List_List*vs=_T8->actuals;_T9=eaf;_TA=_T9->af;{
void*a=_TA->assn;
# 2266
_TL2C8: if(vs!=0)goto _TL2C9;else{goto _TL2C7;}_TL2C9: if(ts!=0)goto _TL2C6;else{goto _TL2C7;}
_TL2C6: _TB=dict;_TC=vs;_TD=_TC->hd;_TE=ts;_TF=_TE->hd;dict=Cyc_Dict_insert(_TB,_TD,_TF);_T10=vs;
# 2266
vs=_T10->tl;_T11=ts;ts=_T11->tl;goto _TL2C8;_TL2C7:
# 2268
 if(vs!=0)goto _TL2CC;else{goto _TL2CD;}_TL2CD: if(ts!=0)goto _TL2CC;else{goto _TL2CA;}
_TL2CC: _T13=Cyc_Warn_impos;{int(*_T17)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T13;_T12=_T17;}_T14=_tag_fat("wrong number of arguments to existassnfn2assn",sizeof(char),46U);_T15=_tag_fat(0U,sizeof(void*),0);_T12(_T14,_T15);goto _TL2CB;_TL2CA: _TL2CB: _T16=
# 2272
Cyc_AssnDef_subst_a(dict,a);return _T16;}}}
# 2276
static struct Cyc_List_List*Cyc_AssnDef_m2string(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_List_List*stringlist){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_Absyn_Vardecl*_T2;struct _tuple0*_T3;struct Cyc_String_pa_PrintArg_struct _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;struct _fat_ptr*_T8;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T2=vd;_T3=_T2->name;
_T9.f1=Cyc_Absynpp_qvar2string(_T3);_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_TA.f1=Cyc_AssnDef_term2string(value);_T4=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T4;void*_TB[2];_TB[0]=& _T9;_TB[1]=& _TA;_T5=_tag_fat("%s == %s",sizeof(char),9U);_T6=_tag_fat(_TB,sizeof(void*),2);_T0=Cyc_aprintf(_T5,_T6);}}{struct _fat_ptr res=_T0;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TA=_cycalloc(sizeof(struct _fat_ptr));
*_TA=res;_T8=(struct _fat_ptr*)_TA;}_T9->hd=_T8;_T9->tl=stringlist;_T7=(struct Cyc_List_List*)_T9;}return _T7;}}
# 2281
struct _fat_ptr Cyc_AssnDef_map2string(struct Cyc_Dict_Dict m){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*);void*(*_T1)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct Cyc_List_List*_T8;void*_T9;struct _fat_ptr*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;struct Cyc_String_pa_PrintArg_struct _T11;struct Cyc_String_pa_PrintArg_struct _T12;struct Cyc_List_List*_T13;void*_T14;struct _fat_ptr*_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;_T1=Cyc_Dict_fold;{
struct Cyc_List_List*(*_T19)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))_T1;_T0=_T19;}_T2=m;{struct Cyc_List_List*stringlist=_T0(Cyc_AssnDef_m2string,_T2,0);
struct _fat_ptr s;
if(stringlist!=0)goto _TL2CE;_T3=_tag_fat(" ",sizeof(char),2U);_T4=_tag_fat(0U,sizeof(void*),0);_T5=Cyc_aprintf(_T3,_T4);return _T5;
_TL2CE:{struct Cyc_String_pa_PrintArg_struct _T19;_T19.tag=0;_T8=stringlist;_T9=_T8->hd;_TA=(struct _fat_ptr*)_T9;_T19.f1=*_TA;_T7=_T19;}{struct Cyc_String_pa_PrintArg_struct _T19=_T7;void*_T1A[1];_T1A[0]=& _T19;_TB=_tag_fat("%s",sizeof(char),3U);_TC=_tag_fat(_T1A,sizeof(void*),1);_T6=Cyc_aprintf(_TB,_TC);}s=_T6;
_TL2D0: _TD=stringlist;_TE=_TD->tl;if(_TE!=0)goto _TL2D1;else{goto _TL2D2;}
_TL2D1: _TF=stringlist;stringlist=_TF->tl;{struct Cyc_String_pa_PrintArg_struct _T19;_T19.tag=0;
_T19.f1=s;_T11=_T19;}{struct Cyc_String_pa_PrintArg_struct _T19=_T11;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;_T13=stringlist;_T14=_T13->hd;_T15=(struct _fat_ptr*)_T14;_T1A.f1=*_T15;_T12=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T12;void*_T1B[2];_T1B[0]=& _T19;_T1B[1]=& _T1A;_T16=_tag_fat("%s && %s",sizeof(char),9U);_T17=_tag_fat(_T1B,sizeof(void*),2);_T10=Cyc_aprintf(_T16,_T17);}}s=_T10;goto _TL2D0;_TL2D2: _T18=s;
# 2290
return _T18;}}
# 2294
struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap am){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_AssnDef_AssnMap _T5;void*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T2=am;_T3=_T2.map;
_T9.f1=Cyc_AssnDef_map2string(_T3);_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_T5=am;_T6=_T5.assn;_TA.f1=Cyc_AssnDef_assn2string(_T6);_T4=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T4;void*_TB[2];_TB[0]=& _T9;_TB[1]=& _TA;_T7=_tag_fat("{%s}&&\n%s",sizeof(char),10U);_T8=_tag_fat(_TB,sizeof(void*),2);_T0=Cyc_aprintf(_T7,_T8);}}return _T0;}
# 2298
struct _fat_ptr Cyc_AssnDef_assnmap2dag(struct Cyc_AssnDef_AssnMap am){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_AssnDef_AssnMap _T5;void*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T2=am;_T3=_T2.map;
_T9.f1=Cyc_AssnDef_map2string(_T3);_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_T5=am;_T6=_T5.assn;_TA.f1=Cyc_AssnDef_assn2dag(_T6);_T4=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T4;void*_TB[2];_TB[0]=& _T9;_TB[1]=& _TA;_T7=_tag_fat("{%s}\n%s",sizeof(char),8U);_T8=_tag_fat(_TB,sizeof(void*),2);_T0=Cyc_aprintf(_T7,_T8);}}return _T0;}
# 2302
static struct Cyc_Dict_Dict Cyc_AssnDef_empty_map (void){struct Cyc_Dict_Dict(*_T0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*));struct Cyc_Dict_Dict(*_T1)(int(*)(void*,void*));int(*_T2)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*);int(*_T3)(void*,void*);struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict _T5;struct Cyc_Dict_Dict*_T6;struct Cyc_Dict_Dict _T7;
static struct Cyc_Dict_Dict*mt=0;
if(mt!=0)goto _TL2D3;_T1=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T8)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))_T1;_T0=_T8;}_T3=Cyc_Core_ptrcmp;{int(*_T8)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))_T3;_T2=_T8;}{struct Cyc_Dict_Dict d=_T0(_T2);{struct Cyc_Dict_Dict*_T8=_cycalloc(sizeof(struct Cyc_Dict_Dict));
*_T8=d;_T4=(struct Cyc_Dict_Dict*)_T8;}mt=_T4;_T5=d;
return _T5;}
# 2309
_TL2D3: _T6=mt;_T7=*_T6;return _T7;}
# 2312
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void){struct Cyc_AssnDef_AssnMap _T0;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2;{struct Cyc_AssnDef_AssnMap _T3;_T1=& Cyc_AssnDef_false_assn;_T2=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T1;
_T3.assn=(void*)_T2;_T3.map=Cyc_AssnDef_empty_map();_T0=_T3;}return _T0;}
# 2316
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void){struct Cyc_AssnDef_AssnMap _T0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T1;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2;{struct Cyc_AssnDef_AssnMap _T3;_T1=& Cyc_AssnDef_true_assn;_T2=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T1;
_T3.assn=(void*)_T2;_T3.map=Cyc_AssnDef_empty_map();_T0=_T3;}return _T0;}
# 2323
struct _tuple13 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*vd,struct Cyc_AssnDef_AssnMap am){void**(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;struct _tuple13 _T5;void**_T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Absyn_Vardecl*_T8;void*_T9;struct Cyc_Dict_Dict(*_TA)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_AssnMap _TC;struct Cyc_Dict_Dict _TD;struct Cyc_Absyn_Vardecl*_TE;void*_TF;struct _tuple13 _T10;struct Cyc_AssnDef_AssnMap _T11;_T1=Cyc_Dict_lookup_opt;{
void**(*_T12)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T12;}_T2=am;_T3=_T2.map;_T4=vd;{void**t=_T0(_T3,_T4);
if(t==0)goto _TL2D5;{struct _tuple13 _T12;_T6=t;_T12.f0=*_T6;_T12.f1=am;_T5=_T12;}return _T5;_TL2D5: _T7=vd;_T8=vd;_T9=_T8->type;{
void*newt=Cyc_AssnDef_fresh_var(_T7,_T9);_TB=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T12)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TB;_TA=_T12;}_TC=am;_TD=_TC.map;_TE=vd;_TF=newt;{struct Cyc_Dict_Dict newm=_TA(_TD,_TE,_TF);{struct _tuple13 _T12;
_T12.f0=newt;_T11=am;_T12.f1.assn=_T11.assn;_T12.f1.map=newm;_T10=_T12;}return _T10;}}}}
# 2333
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*vd,void*newt,struct Cyc_AssnDef_AssnMap am){struct Cyc_Dict_Dict(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T1)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;struct Cyc_AssnDef_AssnMap _T6;struct Cyc_AssnDef_AssnMap _T7;_T1=Cyc_Dict_insert;{
# 2335
struct Cyc_Dict_Dict(*_T8)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T1;_T0=_T8;}_T2=am;_T3=_T2.map;_T4=vd;_T5=newt;{struct Cyc_Dict_Dict newm=_T0(_T3,_T4,_T5);{struct Cyc_AssnDef_AssnMap _T8;_T7=am;
_T8.assn=_T7.assn;_T8.map=newm;_T6=_T8;}return _T6;}}
# 2353 "assndef.cyc"
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap am){struct Cyc_AssnDef_AssnMap _T0;struct Cyc_AssnDef_AssnMap _T1;void*_T2;struct Cyc_AssnDef_AssnMap _T3;{struct Cyc_AssnDef_AssnMap _T4;_T1=am;_T2=_T1.assn;
_T4.assn=Cyc_AssnDef_widen(_T2);_T3=am;_T4.map=_T3.map;_T0=_T4;}return _T0;}
# 2358
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){struct Cyc_AssnDef_AssnMap _T0;struct Cyc_AssnDef_AssnMap _T1;void*_T2;void*_T3;struct Cyc_AssnDef_AssnMap _T4;{struct Cyc_AssnDef_AssnMap _T5;_T1=am;_T2=_T1.assn;_T3=a;
_T5.assn=Cyc_AssnDef_or(_T2,_T3);_T4=am;_T5.map=_T4.map;_T0=_T5;}return _T0;}
# 2363
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){struct Cyc_AssnDef_AssnMap _T0;struct Cyc_AssnDef_AssnMap _T1;void*_T2;void*_T3;struct Cyc_AssnDef_AssnMap _T4;{struct Cyc_AssnDef_AssnMap _T5;_T1=am;_T2=_T1.assn;_T3=a;
_T5.assn=Cyc_AssnDef_and(_T2,_T3);_T4=am;_T5.map=_T4.map;_T0=_T5;}return _T0;}
# 2367
void*Cyc_AssnDef_merge(struct Cyc_Absyn_Vardecl*vd,void*t1,void*t2){void*_T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_Absyn_Vardecl*_T2;void*_T3;void*_T4;
if(t1!=t2)goto _TL2D7;_T0=t1;return _T0;
_TL2D7: _T1=vd;_T2=vd;_T3=_T2->type;_T4=Cyc_AssnDef_fresh_var(_T1,_T3);return _T4;}struct _tuple20{struct Cyc_Hashtable_Table*f0;struct Cyc_Dict_Dict f1;};
# 2372
static void*Cyc_AssnDef_subst_term_internal(struct _tuple20*env,void*t){struct _tuple20*_T0;struct Cyc_Dict_Dict _T1;void*_T2;struct _tuple20*_T3;struct Cyc_Hashtable_Table*_T4;void*_T5;_T0=env;_T1=_T0->f1;_T2=t;_T3=env;_T4=_T3->f0;_T5=
Cyc_AssnDef_subst_term(_T1,_T2,_T4);return _T5;}
# 2376
static struct Cyc_AssnDef_AssnMap Cyc_AssnDef_subst_am(struct Cyc_Dict_Dict dict,struct Cyc_AssnDef_AssnMap am){int(*_T0)(void*,void*);int(*_T1)(void*,void*);struct Cyc_Dict_Dict _T2;struct Cyc_AssnDef_AssnMap _T3;void*_T4;struct Cyc_Hashtable_Table*_T5;struct Cyc_Hashtable_Table*_T6;struct Cyc_Dict_Dict(*_T7)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict(*_T8)(void*(*)(void*,void*),void*,struct Cyc_Dict_Dict);struct _tuple20*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_Dict_Dict _TB;struct Cyc_AssnDef_AssnMap _TC;_T0=Cyc_Core_ptrcmp;{
# 2379
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,_T0,Cyc_AssnDef_hash_ptr);_T1=Cyc_Core_ptrcmp;{
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,_T1,Cyc_AssnDef_hash_ptr);_T2=dict;_T3=am;_T4=_T3.assn;_T5=assn_h;_T6=term_h;{
void*assn=Cyc_AssnDef_subst_assn(_T2,_T4,_T5,_T6);_T8=Cyc_Dict_map_c;{
struct Cyc_Dict_Dict(*_TD)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict))_T8;_T7=_TD;}{struct _tuple20*_TD=_cycalloc(sizeof(struct _tuple20));_TD->f0=term_h;_TD->f1=dict;_T9=(struct _tuple20*)_TD;}_TA=am;_TB=_TA.map;{struct Cyc_Dict_Dict map=_T7(Cyc_AssnDef_subst_term_internal,_T9,_TB);{struct Cyc_AssnDef_AssnMap _TD;
_TD.assn=assn;_TD.map=map;_TC=_TD;}return _TC;}}}}}struct Cyc_AssnDef_Accum{void*a1;void*a2;struct Cyc_Dict_Dict m1;struct Cyc_Dict_Dict m2;struct Cyc_Dict_Dict m;};
# 2393
static int Cyc_AssnDef_logicvar_in_map_internal(void*lvar,struct Cyc_Absyn_Vardecl*dummy,void*value,int in_map){int _T0;int _T1;_T1=in_map;
if(!_T1)goto _TL2D9;_T0=1;goto _TL2DA;_TL2D9: _T0=Cyc_AssnDef_logicvar_in_term(lvar,value);_TL2DA: return _T0;}
# 2397
static int Cyc_AssnDef_logicvar_in_map(void*lv,struct Cyc_Dict_Dict m){int(*_T0)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int);void*(*_T1)(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,void*);void*_T2;struct Cyc_Dict_Dict _T3;int _T4;_T1=Cyc_Dict_fold_c;{
int(*_T5)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int)=(int(*)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int))_T1;_T0=_T5;}_T2=lv;_T3=m;_T4=_T0(Cyc_AssnDef_logicvar_in_map_internal,_T2,_T3,0);return _T4;}
# 2407
static void*Cyc_AssnDef_unify_term(struct Cyc_AssnDef_Accum*accum,void*ta,void*tb){void*_T0;struct _tuple15 _T1;void*_T2;int*_T3;unsigned _T4;void*_T5;int*_T6;int _T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;void*_TD;enum Cyc_Absyn_Primop _TE;int _TF;enum Cyc_Absyn_Primop _T10;int _T11;void*_T12;void*_T13;int*_T14;int _T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;enum Cyc_Absyn_Primop _T1E;int _T1F;enum Cyc_Absyn_Primop _T20;int _T21;void*_T22;void*_T23;int*_T24;int _T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;int _T2C;void*_T2D;void*_T2E;int*_T2F;int _T30;void*_T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;void*_T37;void*_T38;void*_T39;void*_T3A;int*_T3B;int _T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;void*_T47;void*_T48;void*_T49;void*_T4A;void*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;void*_T50;void*_T51;void*_T52;void*_T53;void*_T54;void*_T55;void*_T56;int*_T57;int _T58;void*_T59;void*_T5A;void*_T5B;void*_T5C;int _T5D;int _T5E;struct Cyc_List_List*_T5F;struct Cyc_AssnDef_Accum*_T60;struct Cyc_List_List*_T61;void*_T62;struct Cyc_List_List*_T63;void*_T64;struct Cyc_List_List*_T65;struct Cyc_List_List*_T66;int _T67;unsigned _T68;struct Cyc_List_List*_T69;void*_T6A;void*_T6B;void*_T6C;int*_T6D;int _T6E;void*_T6F;void*_T70;void*_T71;void*_T72;void*_T73;void*_T74;unsigned _T75;unsigned _T76;void*_T77;void*_T78;int*_T79;int _T7A;void*_T7B;void*_T7C;void*_T7D;void*_T7E;void*_T7F;void*_T80;void*_T81;void*_T82;unsigned _T83;void*_T84;unsigned _T85;void*_T86;void*_T87;void*_T88;unsigned _T89;void*_T8A;void*_T8B;unsigned _T8C;void*_T8D;unsigned _T8E;void*_T8F;void*_T90;void*_T91;void*_T92;int*_T93;int _T94;void*_T95;void*_T96;void*_T97;void*_T98;void*_T99;void*_T9A;struct Cyc_Absyn_Vardecl*_T9B;int _T9C;struct Cyc_Absyn_Vardecl*_T9D;int _T9E;void*_T9F;void*_TA0;int*_TA1;int _TA2;void*_TA3;void*_TA4;void*_TA5;void*_TA6;void*_TA7;void*_TA8;unsigned _TA9;unsigned _TAA;void*_TAB;void*_TAC;int*_TAD;int _TAE;void*_TAF;void*_TB0;void*_TB1;void*_TB2;void*_TB3;void*_TB4;void*_TB5;void*_TB6;void*_TB7;void*_TB8;int*_TB9;int _TBA;void*_TBB;void*_TBC;void*_TBD;void*_TBE;void*_TBF;int _TC0;struct Cyc_AssnDef_Accum*_TC1;struct Cyc_AssnDef_Accum*_TC2;void*_TC3;void*_TC4;struct Cyc_AssnDef_Accum*_TC5;struct Cyc_AssnDef_Accum*_TC6;void*_TC7;void*_TC8;void*_TC9;
if(ta!=tb)goto _TL2DB;_T0=ta;return _T0;_TL2DB:{struct _tuple15 _TCA;
_TCA.f0=ta;_TCA.f1=tb;_T1=_TCA;}{struct _tuple15 _TCA=_T1;unsigned _TCB;int _TCC;unsigned _TCD;int _TCE;void*_TCF;void*_TD0;void*_TD1;void*_TD2;enum Cyc_Absyn_Primop _TD3;void*_TD4;void*_TD5;enum Cyc_Absyn_Primop _TD6;_T2=_TCA.f0;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 3: _T5=_TCA.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=3)goto _TL2DE;_T8=_TCA.f0;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T8;_TD6=_TD7->f1;_T9=_TD7->f2;_TD5=(void*)_T9;_TA=_TD7->f3;_TD4=(void*)_TA;}_TB=_TCA.f1;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_TB;_TD3=_TD7->f1;_TC=_TD7->f2;_TD2=(void*)_TC;_TD=_TD7->f3;_TD1=(void*)_TD;}_TE=(enum Cyc_Absyn_Primop)_TD6;_TF=(int)_TE;_T10=(enum Cyc_Absyn_Primop)_TD3;_T11=(int)_T10;if(_TF!=_T11)goto _TL2E0;{enum Cyc_Absyn_Primop pa=_TD6;void*ta1=_TD5;void*topta=_TD4;enum Cyc_Absyn_Primop pb=_TD3;void*tb1=_TD2;void*toptb=_TD1;
# 2411
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);_T12=
Cyc_AssnDef_unop(pa,t1,topta);return _T12;}_TL2E0: goto _LL19;_TL2DE: goto _LL19;case 4: _T13=_TCA.f1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=4)goto _TL2E2;_T16=_TCA.f0;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T16;_TD6=_TD7->f1;_T17=_TD7->f2;_TD5=(void*)_T17;_T18=_TD7->f3;_TD4=(void*)_T18;_T19=_TD7->f4;_TD2=(void*)_T19;}_T1A=_TCA.f1;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T1A;_TD3=_TD7->f1;_T1B=_TD7->f2;_TD1=(void*)_T1B;_T1C=_TD7->f3;_TD0=(void*)_T1C;_T1D=_TD7->f4;_TCF=(void*)_T1D;}_T1E=(enum Cyc_Absyn_Primop)_TD6;_T1F=(int)_T1E;_T20=(enum Cyc_Absyn_Primop)_TD3;_T21=(int)_T20;if(_T1F!=_T21)goto _TL2E4;{enum Cyc_Absyn_Primop pa=_TD6;void*ta1=_TD5;void*ta2=_TD4;void*topta=_TD2;enum Cyc_Absyn_Primop pb=_TD3;void*tb1=_TD1;void*tb2=_TD0;void*toptb=_TCF;
# 2414
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);
void*t2=Cyc_AssnDef_unify_term(accum,ta2,tb2);_T22=
Cyc_AssnDef_binop(pa,t1,t2,topta);return _T22;}_TL2E4: goto _LL19;_TL2E2: goto _LL19;case 5: _T23=_TCA.f1;_T24=(int*)_T23;_T25=*_T24;if(_T25!=5)goto _TL2E6;_T26=_TCA.f0;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T26;_T27=_TD7->f1;_TD5=(void*)_T27;_T28=_TD7->f2;_TD4=(void*)_T28;}_T29=_TCA.f1;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T29;_T2A=_TD7->f1;_TD2=(void*)_T2A;_T2B=_TD7->f2;_TD1=(void*)_T2B;}{void*atype=_TD5;void*ta1=_TD4;void*btype=_TD2;void*tb1=_TD1;_T2C=
# 2418
Cyc_Unify_unify(atype,btype);if(_T2C)goto _TL2E8;else{goto _TL2EA;}_TL2EA: goto _LL0;_TL2E8: {
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);_T2D=
Cyc_AssnDef_cast(atype,t1);return _T2D;}}_TL2E6: goto _LL19;case 6: _T2E=_TCA.f1;_T2F=(int*)_T2E;_T30=*_T2F;if(_T30!=6)goto _TL2EB;_T31=_TCA.f0;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T31;_T32=_TD7->f1;_TD5=(void*)_T32;_T33=_TD7->f2;_TD4=(void*)_T33;_T34=_TD7->f3;_TD2=(void*)_T34;}_T35=_TCA.f1;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T35;_T36=_TD7->f1;_TD1=(void*)_T36;_T37=_TD7->f2;_TD0=(void*)_T37;_T38=_TD7->f3;_TCF=(void*)_T38;}{void*ma=_TD5;void*aa=_TD4;void*ta=_TD2;void*mb=_TD1;void*ab=_TD0;void*tb=_TCF;
# 2422
void*m=Cyc_AssnDef_unify_term(accum,ma,mb);
void*a=Cyc_AssnDef_unify_term(accum,aa,ab);_T39=
Cyc_AssnDef_select(m,a,ta);return _T39;}_TL2EB: goto _LL19;case 7: _T3A=_TCA.f1;_T3B=(int*)_T3A;_T3C=*_T3B;if(_T3C!=7)goto _TL2ED;_T3D=_TCA.f0;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T3D;_T3E=_TD7->f1;_TD5=(void*)_T3E;_T3F=_TD7->f2;_TD4=(void*)_T3F;_T40=_TD7->f3;_TD2=(void*)_T40;}_T41=_TCA.f1;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T41;_T42=_TD7->f1;_TD1=(void*)_T42;_T43=_TD7->f2;_TD0=(void*)_T43;_T44=_TD7->f3;_TCF=(void*)_T44;}{void*ma=_TD5;void*aa=_TD4;void*va=_TD2;void*mb=_TD1;void*ab=_TD0;void*vb=_TCF;
# 2426
if(aa!=ab)goto _TL2EF;{
void*m=Cyc_AssnDef_unify_term(accum,ma,mb);
void*v=Cyc_AssnDef_unify_term(accum,va,vb);_T45=
Cyc_AssnDef_update(m,aa,v);return _T45;}
# 2431
_TL2EF: _T46=ma;_T47=ab;_T48=ma;_T49=ab;_T4A=Cyc_AssnDef_get_term_type(vb);_T4B=Cyc_AssnDef_select(_T48,_T49,_T4A);_T4C=Cyc_AssnDef_update(_T46,_T47,_T4B);_T4D=aa;_T4E=va;{void*t1=Cyc_AssnDef_update(_T4C,_T4D,_T4E);_T4F=
Cyc_AssnDef_update(mb,ab,vb);_T50=aa;_T51=Cyc_AssnDef_update(mb,ab,vb);_T52=aa;_T53=Cyc_AssnDef_get_term_type(va);_T54=Cyc_AssnDef_select(_T51,_T52,_T53);{void*t2=Cyc_AssnDef_update(_T4F,_T50,_T54);_T55=
Cyc_AssnDef_unify_term(accum,t1,t2);return _T55;}}}_TL2ED: goto _LL19;case 8: _T56=_TCA.f1;_T57=(int*)_T56;_T58=*_T57;if(_T58!=8)goto _TL2F1;_T59=_TCA.f0;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T59;_TCE=_TD7->f1;_TCD=_TD7->f2;_TD5=_TD7->f3;_T5A=_TD7->f4;_TD4=(void*)_T5A;}_T5B=_TCA.f1;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T5B;_TCC=_TD7->f1;_TCB=_TD7->f2;_TD2=_TD7->f3;_T5C=_TD7->f4;_TD1=(void*)_T5C;}{int aisu=_TCE;unsigned atag=_TCD;struct Cyc_List_List*ats=_TD5;void*atopt=_TD4;int bisu=_TCC;unsigned btag=_TCB;struct Cyc_List_List*bts=_TD2;void*btopt=_TD1;
# 2442
if(aisu!=bisu)goto _TL2F5;else{goto _TL2F7;}_TL2F7: if(atag!=btag)goto _TL2F5;else{goto _TL2F6;}_TL2F6: _T5D=Cyc_List_length(ats);_T5E=Cyc_List_length(bts);if(_T5D!=_T5E)goto _TL2F5;else{goto _TL2F3;}_TL2F5: goto _LL0;_TL2F3: {
struct Cyc_List_List*ts=0;
_TL2FB: if(ats!=0)goto _TL2FC;else{goto _TL2FA;}_TL2FC: if(bts!=0)goto _TL2F9;else{goto _TL2FA;}
_TL2F9:{struct Cyc_List_List*_TD7=_cycalloc(sizeof(struct Cyc_List_List));_T60=accum;_T61=ats;_T62=_T61->hd;_T63=bts;_T64=_T63->hd;_TD7->hd=Cyc_AssnDef_unify_term(_T60,_T62,_T64);_TD7->tl=ts;_T5F=(struct Cyc_List_List*)_TD7;}ts=_T5F;_T65=ats;
# 2444
ats=_T65->tl;_T66=bts;bts=_T66->tl;goto _TL2FB;_TL2FA: _T67=aisu;_T68=atag;_T69=
# 2446
Cyc_List_imp_rev(ts);_T6A=atopt;_T6B=Cyc_AssnDef_aggr(_T67,_T68,_T69,_T6A);return _T6B;}}_TL2F1: goto _LL19;case 9: _T6C=_TCA.f1;_T6D=(int*)_T6C;_T6E=*_T6D;if(_T6E!=9)goto _TL2FD;_T6F=_TCA.f0;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T6F;_T70=_TD7->f1;_TD5=(void*)_T70;_TCD=_TD7->f2;_T71=_TD7->f3;_TD4=(void*)_T71;}_T72=_TCA.f1;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T72;_T73=_TD7->f1;_TD2=(void*)_T73;_TCB=_TD7->f2;_T74=_TD7->f3;_TD1=(void*)_T74;}_T75=(unsigned)_TCD;_T76=(unsigned)_TCB;if(_T75!=_T76)goto _TL2FF;{void*at=_TD5;unsigned ai=_TCD;void*atopt=_TD4;void*bt=_TD2;unsigned bi=_TCB;void*btopt=_TD1;
# 2448
void*t=Cyc_AssnDef_unify_term(accum,at,bt);_T77=
Cyc_AssnDef_proj(t,ai,atopt);return _T77;}_TL2FF: goto _LL19;_TL2FD: goto _LL19;case 10: _T78=_TCA.f1;_T79=(int*)_T78;_T7A=*_T79;if(_T7A!=10)goto _TL301;_T7B=_TCA.f0;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T7B;_T7C=_TD7->f1;_TD5=(void*)_T7C;_TCD=_TD7->f2;_T7D=_TD7->f3;_TD4=(void*)_T7D;}_T7E=_TCA.f1;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T7E;_T7F=_TD7->f1;_TD2=(void*)_T7F;_TCB=_TD7->f2;_T80=_TD7->f3;_TD1=(void*)_T80;}{void*at=_TD5;unsigned ai=_TCD;void*av=_TD4;void*bt=_TD2;unsigned bi=_TCB;void*bv=_TD1;
# 2451
if(ai!=bi)goto _TL303;{
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
void*v=Cyc_AssnDef_unify_term(accum,av,bv);_T81=
Cyc_AssnDef_aggr_update(t,ai,v);return _T81;}
# 2456
_TL303: _T82=ta;_T83=bi;_T84=ta;_T85=bi;_T86=Cyc_AssnDef_get_term_type(bv);_T87=Cyc_AssnDef_proj(_T84,_T85,_T86);_T88=Cyc_AssnDef_aggr_update(_T82,_T83,_T87);_T89=ai;_T8A=av;{void*t1=Cyc_AssnDef_aggr_update(_T88,_T89,_T8A);_T8B=
Cyc_AssnDef_aggr_update(tb,bi,bv);_T8C=ai;_T8D=Cyc_AssnDef_aggr_update(tb,bi,bv);_T8E=ai;_T8F=Cyc_AssnDef_get_term_type(av);_T90=Cyc_AssnDef_proj(_T8D,_T8E,_T8F);{void*t2=Cyc_AssnDef_aggr_update(_T8B,_T8C,_T90);_T91=
Cyc_AssnDef_unify_term(accum,t1,t2);return _T91;}}}_TL301: goto _LL19;case 11: _T92=_TCA.f1;_T93=(int*)_T92;_T94=*_T93;if(_T94!=11)goto _TL305;_T95=_TCA.f0;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T95;_TD5=_TD7->f1;_T96=_TD7->f2;_TD4=(void*)_T96;_T97=_TD7->f3;_TD2=(void*)_T97;}_T98=_TCA.f1;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T98;_TD1=_TD7->f1;_T99=_TD7->f2;_TD0=(void*)_T99;_T9A=_TD7->f3;_TCF=(void*)_T9A;}_T9B=(struct Cyc_Absyn_Vardecl*)_TD5;_T9C=(int)_T9B;_T9D=(struct Cyc_Absyn_Vardecl*)_TD1;_T9E=(int)_T9D;if(_T9C!=_T9E)goto _TL307;{struct Cyc_Absyn_Vardecl*ax=_TD5;void*ai=_TD4;void*atopt=_TD2;struct Cyc_Absyn_Vardecl*bx=_TD1;void*bi=_TD0;void*btopt=_TCF;
# 2461
void*i=Cyc_AssnDef_unify_term(accum,ai,bi);_T9F=
Cyc_AssnDef_addr(ax,i);return _T9F;}_TL307: goto _LL19;_TL305: goto _LL19;case 13: _TA0=_TCA.f1;_TA1=(int*)_TA0;_TA2=*_TA1;if(_TA2!=13)goto _TL309;_TA3=_TCA.f0;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_TA3;_TA4=_TD7->f1;_TD5=(void*)_TA4;_TCD=_TD7->f2;_TA5=_TD7->f3;_TD4=(void*)_TA5;}_TA6=_TCA.f1;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_TA6;_TA7=_TD7->f1;_TD2=(void*)_TA7;_TCB=_TD7->f2;_TA8=_TD7->f3;_TD1=(void*)_TA8;}_TA9=(unsigned)_TCD;_TAA=(unsigned)_TCB;if(_TA9!=_TAA)goto _TL30B;{void*at=_TD5;unsigned ai=_TCD;void*atopt=_TD4;void*bt=_TD2;unsigned bi=_TCB;void*btopt=_TD1;
# 2467
void*t=Cyc_AssnDef_unify_term(accum,at,bt);_TAB=
Cyc_AssnDef_offsetf(t,ai,atopt);return _TAB;}_TL30B: goto _LL19;_TL309: goto _LL19;case 14: _TAC=_TCA.f1;_TAD=(int*)_TAC;_TAE=*_TAD;if(_TAE!=14)goto _TL30D;_TAF=_TCA.f0;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_TAF;_TB0=_TD7->f1;_TD5=(void*)_TB0;_TB1=_TD7->f2;_TD4=(void*)_TB1;_TB2=_TD7->f3;_TD2=(void*)_TB2;}_TB3=_TCA.f1;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_TB3;_TB4=_TD7->f1;_TD1=(void*)_TB4;_TB5=_TD7->f2;_TD0=(void*)_TB5;_TB6=_TD7->f3;_TCF=(void*)_TB6;}{void*at=_TD5;void*ai=_TD4;void*atopt=_TD2;void*bt=_TD1;void*bi=_TD0;void*btopt=_TCF;
# 2470
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
void*i=Cyc_AssnDef_unify_term(accum,ai,bi);_TB7=
Cyc_AssnDef_offseti(t,i,atopt);return _TB7;}_TL30D: goto _LL19;case 15: _TB8=_TCA.f1;_TB9=(int*)_TB8;_TBA=*_TB9;if(_TBA!=15)goto _TL30F;_TBB=_TCA.f0;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_TBB;_TBC=_TD7->f1;_TD5=(void*)_TBC;}_TBD=_TCA.f1;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_TD7=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_TBD;_TBE=_TD7->f1;_TD4=(void*)_TBE;}{void*at=_TD5;void*bt=_TD4;
# 2474
void*t=Cyc_AssnDef_unify_term(accum,at,bt);_TBF=
Cyc_AssnDef_tagof_tm(t);return _TBF;}_TL30F: goto _LL19;default: _LL19: goto _LL0;}_LL0:;}{
# 2478
void*topta=Cyc_AssnDef_get_term_type(ta);
void*toptb=Cyc_AssnDef_get_term_type(tb);
void*topt=0;
if(topta==0)goto _TL311;if(toptb==0)goto _TL311;_TC0=Cyc_Unify_unify(topta,toptb);if(!_TC0)goto _TL311;
# 2483
topt=topta;goto _TL312;_TL311: _TL312: {
void*v=Cyc_AssnDef_fresh_var(0,topt);_TC1=accum;_TC2=accum;_TC3=_TC2->a1;_TC4=
Cyc_AssnDef_eq(v,ta);_TC1->a1=Cyc_AssnDef_and(_TC3,_TC4);_TC5=accum;_TC6=accum;_TC7=_TC6->a2;_TC8=
Cyc_AssnDef_eq(v,tb);_TC5->a2=Cyc_AssnDef_and(_TC7,_TC8);_TC9=v;
return _TC9;}}}
# 2494
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm1(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){void**(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_Accum*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_AssnDef_Accum*_T9;void*_TA;int _TB;void*_TC;struct Cyc_AssnDef_Accum*_TD;struct Cyc_Dict_Dict _TE;int _TF;struct Cyc_AssnDef_Accum*_T10;struct Cyc_Dict_Dict(*_T11)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T12)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T13;struct Cyc_Dict_Dict _T14;struct Cyc_Absyn_Vardecl*_T15;void*_T16;struct Cyc_AssnDef_Accum*_T17;struct Cyc_Dict_Dict(*_T18)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T19)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T1A;struct Cyc_Dict_Dict _T1B;struct Cyc_Absyn_Vardecl*_T1C;void*_T1D;struct Cyc_AssnDef_Accum*_T1E;struct Cyc_Absyn_Vardecl*_T1F;struct Cyc_Absyn_Vardecl*_T20;void*_T21;struct Cyc_AssnDef_Accum*_T22;struct Cyc_AssnDef_Accum*_T23;void*_T24;void*_T25;struct Cyc_AssnDef_Accum*_T26;struct Cyc_Dict_Dict(*_T27)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T28)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T29;struct Cyc_Dict_Dict _T2A;struct Cyc_Absyn_Vardecl*_T2B;void*_T2C;struct Cyc_AssnDef_Accum*_T2D;struct Cyc_Dict_Dict(*_T2E)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T2F)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T30;struct Cyc_Dict_Dict _T31;struct Cyc_Absyn_Vardecl*_T32;void*_T33;struct Cyc_AssnDef_Accum*_T34;struct Cyc_Dict_Dict(*_T35)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T36)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T37;struct Cyc_Dict_Dict _T38;struct Cyc_Absyn_Vardecl*_T39;void*_T3A;struct Cyc_AssnDef_Accum*_T3B;void*_T3C;void**_T3D;void*_T3E;struct Cyc_AssnDef_Accum*_T3F;struct Cyc_Dict_Dict(*_T40)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T41)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T42;struct Cyc_Dict_Dict _T43;struct Cyc_Absyn_Vardecl*_T44;void*_T45;struct Cyc_AssnDef_Accum*_T46;struct _tuple15 _T47;void**_T48;void*_T49;int*_T4A;int _T4B;void*_T4C;int*_T4D;int _T4E;void*_T4F;struct Cyc_AssnDef_Accum*_T50;void*_T51;int _T52;void*_T53;struct Cyc_AssnDef_Accum*_T54;struct Cyc_Dict_Dict _T55;int _T56;struct Cyc_AssnDef_Accum*_T57;struct Cyc_AssnDef_Accum*_T58;void*_T59;void*_T5A;void**_T5B;void*_T5C;void*_T5D;struct Cyc_AssnDef_Accum*_T5E;struct Cyc_Dict_Dict(*_T5F)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T60)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T61;struct Cyc_Dict_Dict _T62;struct Cyc_Absyn_Vardecl*_T63;void*_T64;struct Cyc_AssnDef_Accum*_T65;struct Cyc_Dict_Dict(*_T66)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T67)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T68;struct Cyc_Dict_Dict _T69;struct Cyc_Absyn_Vardecl*_T6A;void*_T6B;struct Cyc_AssnDef_Accum*_T6C;void**_T6D;void*_T6E;struct Cyc_AssnDef_Accum*_T6F;void*_T70;int _T71;void**_T72;void*_T73;struct Cyc_AssnDef_Accum*_T74;struct Cyc_Dict_Dict _T75;int _T76;struct Cyc_AssnDef_Accum*_T77;struct Cyc_AssnDef_Accum*_T78;void*_T79;void*_T7A;void**_T7B;void*_T7C;void*_T7D;struct Cyc_AssnDef_Accum*_T7E;struct Cyc_Dict_Dict(*_T7F)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T80)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T81;struct Cyc_Dict_Dict _T82;struct Cyc_Absyn_Vardecl*_T83;void**_T84;void*_T85;struct Cyc_AssnDef_Accum*_T86;struct Cyc_Dict_Dict(*_T87)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T88)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_T89;struct Cyc_Dict_Dict _T8A;struct Cyc_Absyn_Vardecl*_T8B;void**_T8C;void*_T8D;struct Cyc_AssnDef_Accum*_T8E;void*_T8F;struct Cyc_AssnDef_Accum*_T90;void*_T91;int _T92;void*_T93;struct Cyc_AssnDef_Accum*_T94;struct Cyc_Dict_Dict _T95;int _T96;struct Cyc_AssnDef_Accum*_T97;struct Cyc_AssnDef_Accum*_T98;void*_T99;void*_T9A;void**_T9B;void*_T9C;void*_T9D;struct Cyc_AssnDef_Accum*_T9E;struct Cyc_Dict_Dict(*_T9F)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TA0)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TA1;struct Cyc_Dict_Dict _TA2;struct Cyc_Absyn_Vardecl*_TA3;void*_TA4;struct Cyc_AssnDef_Accum*_TA5;struct Cyc_Dict_Dict(*_TA6)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TA7)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TA8;struct Cyc_Dict_Dict _TA9;struct Cyc_Absyn_Vardecl*_TAA;void*_TAB;struct Cyc_AssnDef_Accum*_TAC;void*_TAD;int*_TAE;int _TAF;void**_TB0;void*_TB1;struct Cyc_AssnDef_Accum*_TB2;void*_TB3;int _TB4;void**_TB5;void*_TB6;struct Cyc_AssnDef_Accum*_TB7;struct Cyc_Dict_Dict _TB8;int _TB9;struct Cyc_AssnDef_Accum*_TBA;struct Cyc_AssnDef_Accum*_TBB;void*_TBC;void*_TBD;void**_TBE;void*_TBF;void*_TC0;struct Cyc_AssnDef_Accum*_TC1;struct Cyc_Dict_Dict(*_TC2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TC3)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TC4;struct Cyc_Dict_Dict _TC5;struct Cyc_Absyn_Vardecl*_TC6;void**_TC7;void*_TC8;struct Cyc_AssnDef_Accum*_TC9;struct Cyc_Dict_Dict(*_TCA)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TCB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TCC;struct Cyc_Dict_Dict _TCD;struct Cyc_Absyn_Vardecl*_TCE;void**_TCF;void*_TD0;struct Cyc_AssnDef_Accum*_TD1;struct Cyc_AssnDef_Accum*_TD2;void*_TD3;void**_TD4;void*_TD5;struct Cyc_AssnDef_Accum*_TD6;struct Cyc_Dict_Dict(*_TD7)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TD8)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TD9;struct Cyc_Dict_Dict _TDA;struct Cyc_Absyn_Vardecl*_TDB;void*_TDC;struct Cyc_AssnDef_Accum*_TDD;struct Cyc_Dict_Dict(*_TDE)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TDF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TE0;struct Cyc_Dict_Dict _TE1;struct Cyc_Absyn_Vardecl*_TE2;void*_TE3;struct Cyc_AssnDef_Accum*_TE4;struct Cyc_Dict_Dict(*_TE5)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TE6)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TE7;struct Cyc_Dict_Dict _TE8;struct Cyc_Absyn_Vardecl*_TE9;void*_TEA;struct Cyc_AssnDef_Accum*_TEB;_T1=Cyc_Dict_lookup_opt;{
void**(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1;_T0=_TEC;}_T2=accum;_T3=_T2->m2;_T4=vd;{void**t2=_T0(_T3,_T4);
# 2498
if(t2!=0)goto _TL313;_T5=t;_T6=(int*)_T5;_T7=*_T6;if(_T7!=2)goto _TL315;_T8=t;_T9=accum;_TA=_T9->a2;_TB=
# 2501
Cyc_AssnDef_logicvar_in_assn(_T8,_TA);if(_TB)goto _TL317;else{goto _TL319;}_TL319: _TC=t;_TD=accum;_TE=_TD->m2;_TF=Cyc_AssnDef_logicvar_in_map(_TC,_TE);if(_TF)goto _TL317;else{goto _TL31A;}
_TL31A: _T10=accum;_T12=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T12;_T11=_TEC;}_T13=accum;_T14=_T13->m2;_T15=vd;_T16=t;_T10->m2=_T11(_T14,_T15,_T16);_T17=accum;_T19=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T19;_T18=_TEC;}_T1A=accum;_T1B=_T1A->m;_T1C=vd;_T1D=t;_T17->m=_T18(_T1B,_T1C,_T1D);_T1E=accum;
return _T1E;_TL317: goto _LL0;_TL315: goto _LL0;_LL0: _T1F=vd;_T20=vd;_T21=_T20->type;{
# 2510
void*v=Cyc_AssnDef_fresh_var(_T1F,_T21);_T22=accum;_T23=accum;_T24=_T23->a1;_T25=
Cyc_AssnDef_eq(v,t);_T22->a1=Cyc_AssnDef_and(_T24,_T25);_T26=accum;_T28=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T28;_T27=_TEC;}_T29=accum;_T2A=_T29->m1;_T2B=vd;_T2C=v;_T26->m1=_T27(_T2A,_T2B,_T2C);_T2D=accum;_T2F=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T2F;_T2E=_TEC;}_T30=accum;_T31=_T30->m2;_T32=vd;_T33=v;_T2D->m2=_T2E(_T31,_T32,_T33);_T34=accum;_T36=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T36;_T35=_TEC;}_T37=accum;_T38=_T37->m;_T39=vd;_T3A=v;_T34->m=_T35(_T38,_T39,_T3A);_T3B=accum;
return _T3B;}_TL313: _T3C=t;_T3D=t2;_T3E=*_T3D;
# 2518
if(_T3C!=_T3E)goto _TL31B;_T3F=accum;_T41=Cyc_Dict_insert;{
# 2521
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T41;_T40=_TEC;}_T42=accum;_T43=_T42->m;_T44=vd;_T45=t;_T3F->m=_T40(_T43,_T44,_T45);_T46=accum;
return _T46;_TL31B:{struct _tuple15 _TEC;
# 2526
_TEC.f0=t;_T48=t2;_TEC.f1=*_T48;_T47=_TEC;}{struct _tuple15 _TEC=_T47;_T49=_TEC.f0;_T4A=(int*)_T49;_T4B=*_T4A;if(_T4B!=2)goto _TL31D;_T4C=_TEC.f1;_T4D=(int*)_T4C;_T4E=*_T4D;if(_T4E!=2)goto _TL31F;_T4F=t;_T50=accum;_T51=_T50->a2;_T52=
# 2528
Cyc_AssnDef_logicvar_in_assn(_T4F,_T51);if(_T52)goto _TL321;else{goto _TL323;}_TL323: _T53=t;_T54=accum;_T55=_T54->m2;_T56=Cyc_AssnDef_logicvar_in_map(_T53,_T55);if(_T56)goto _TL321;else{goto _TL324;}
# 2530
_TL324: _T57=accum;_T58=accum;_T59=_T58->a2;_T5A=t;_T5B=t2;_T5C=*_T5B;_T5D=Cyc_AssnDef_eq(_T5A,_T5C);_T57->a2=Cyc_AssnDef_and(_T59,_T5D);_T5E=accum;_T60=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T60;_T5F=_TED;}_T61=accum;_T62=_T61->m2;_T63=vd;_T64=t;_T5E->m2=_T5F(_T62,_T63,_T64);_T65=accum;_T67=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T67;_T66=_TED;}_T68=accum;_T69=_T68->m;_T6A=vd;_T6B=t;_T65->m=_T66(_T69,_T6A,_T6B);_T6C=accum;
return _T6C;
_TL321: _T6D=t2;_T6E=*_T6D;_T6F=accum;_T70=_T6F->a1;_T71=Cyc_AssnDef_logicvar_in_assn(_T6E,_T70);if(_T71)goto _TL325;else{goto _TL327;}_TL327: _T72=t2;_T73=*_T72;_T74=accum;_T75=_T74->m1;_T76=Cyc_AssnDef_logicvar_in_map(_T73,_T75);if(_T76)goto _TL325;else{goto _TL328;}
# 2536
_TL328: _T77=accum;_T78=accum;_T79=_T78->a1;_T7A=t;_T7B=t2;_T7C=*_T7B;_T7D=Cyc_AssnDef_eq(_T7A,_T7C);_T77->a1=Cyc_AssnDef_and(_T79,_T7D);_T7E=accum;_T80=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T80;_T7F=_TED;}_T81=accum;_T82=_T81->m1;_T83=vd;_T84=t2;_T85=*_T84;_T7E->m1=_T7F(_T82,_T83,_T85);_T86=accum;_T88=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T88;_T87=_TED;}_T89=accum;_T8A=_T89->m;_T8B=vd;_T8C=t2;_T8D=*_T8C;_T86->m=_T87(_T8A,_T8B,_T8D);_T8E=accum;
return _T8E;_TL325: goto _LL5;_TL31F: _T8F=t;_T90=accum;_T91=_T90->a2;_T92=
# 2543
Cyc_AssnDef_logicvar_in_assn(_T8F,_T91);if(_T92)goto _TL329;else{goto _TL32B;}_TL32B: _T93=t;_T94=accum;_T95=_T94->m2;_T96=Cyc_AssnDef_logicvar_in_map(_T93,_T95);if(_T96)goto _TL329;else{goto _TL32C;}
# 2545
_TL32C: _T97=accum;_T98=accum;_T99=_T98->a2;_T9A=t;_T9B=t2;_T9C=*_T9B;_T9D=Cyc_AssnDef_eq(_T9A,_T9C);_T97->a2=Cyc_AssnDef_and(_T99,_T9D);_T9E=accum;_TA0=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TA0;_T9F=_TED;}_TA1=accum;_TA2=_TA1->m2;_TA3=vd;_TA4=t;_T9E->m2=_T9F(_TA2,_TA3,_TA4);_TA5=accum;_TA7=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TA7;_TA6=_TED;}_TA8=accum;_TA9=_TA8->m;_TAA=vd;_TAB=t;_TA5->m=_TA6(_TA9,_TAA,_TAB);_TAC=accum;
return _TAC;_TL329: goto _LL5;_TL31D: _TAD=_TEC.f1;_TAE=(int*)_TAD;_TAF=*_TAE;if(_TAF!=2)goto _TL32D;_TB0=t2;_TB1=*_TB0;_TB2=accum;_TB3=_TB2->a1;_TB4=
# 2552
Cyc_AssnDef_logicvar_in_assn(_TB1,_TB3);if(_TB4)goto _TL32F;else{goto _TL331;}_TL331: _TB5=t2;_TB6=*_TB5;_TB7=accum;_TB8=_TB7->m1;_TB9=Cyc_AssnDef_logicvar_in_map(_TB6,_TB8);if(_TB9)goto _TL32F;else{goto _TL332;}
# 2554
_TL332: _TBA=accum;_TBB=accum;_TBC=_TBB->a1;_TBD=t;_TBE=t2;_TBF=*_TBE;_TC0=Cyc_AssnDef_eq(_TBD,_TBF);_TBA->a1=Cyc_AssnDef_and(_TBC,_TC0);_TC1=accum;_TC3=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TC3;_TC2=_TED;}_TC4=accum;_TC5=_TC4->m1;_TC6=vd;_TC7=t2;_TC8=*_TC7;_TC1->m1=_TC2(_TC5,_TC6,_TC8);_TC9=accum;_TCB=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TED)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TCB;_TCA=_TED;}_TCC=accum;_TCD=_TCC->m;_TCE=vd;_TCF=t2;_TD0=*_TCF;_TC9->m=_TCA(_TCD,_TCE,_TD0);_TD1=accum;
return _TD1;_TL32F: goto _LL5;_TL32D: goto _LL5;_LL5:;}_TD2=accum;_TD3=t;_TD4=t2;_TD5=*_TD4;{
# 2563
void*newt=Cyc_AssnDef_unify_term(_TD2,_TD3,_TD5);_TD6=accum;_TD8=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TD8;_TD7=_TEC;}_TD9=accum;_TDA=_TD9->m1;_TDB=vd;_TDC=newt;_TD6->m1=_TD7(_TDA,_TDB,_TDC);_TDD=accum;_TDF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TDF;_TDE=_TEC;}_TE0=accum;_TE1=_TE0->m2;_TE2=vd;_TE3=newt;_TDD->m2=_TDE(_TE1,_TE2,_TE3);_TE4=accum;_TE6=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TEC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TE6;_TE5=_TEC;}_TE7=accum;_TE8=_TE7->m;_TE9=vd;_TEA=newt;_TE4->m=_TE5(_TE8,_TE9,_TEA);_TEB=accum;
return _TEB;}}}
# 2574
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm2(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){struct Cyc_Absyn_Vardecl*_T0;struct Cyc_Absyn_Vardecl*_T1;void*_T2;struct Cyc_AssnDef_Accum*_T3;void*_T4;struct Cyc_AssnDef_Accum*_T5;void*_T6;struct Cyc_AssnDef_Accum*_T7;struct Cyc_Dict_Dict(*_T8)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T9)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Accum*_TA;struct Cyc_Dict_Dict _TB;struct Cyc_Absyn_Vardecl*_TC;void*_TD;struct Cyc_AssnDef_Accum*_TE;_T0=vd;_T1=vd;_T2=_T1->type;{
# 2580
void*v=Cyc_AssnDef_fresh_var(_T0,_T2);_T3=accum;_T4=
Cyc_AssnDef_eq(v,t);_T5=accum;_T6=_T5->a2;_T3->a2=Cyc_AssnDef_and(_T4,_T6);_T7=accum;_T9=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T9;_T8=_TF;}_TA=accum;_TB=_TA->m;_TC=vd;_TD=v;_T7->m=_T8(_TB,_TC,_TD);_TE=accum;
return _TE;};}
# 2587
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void){struct Cyc_Dict_Dict*_T0;int(*_T1)(void*,void*);struct Cyc_Dict_Dict*_T2;struct Cyc_Dict_Dict _T3;
static struct Cyc_Dict_Dict*term_dict=0;
if(term_dict!=0)goto _TL333;{struct Cyc_Dict_Dict*_T4=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T1=Cyc_Core_ptrcmp;
*_T4=Cyc_Dict_empty(_T1);_T0=(struct Cyc_Dict_Dict*)_T4;}term_dict=_T0;goto _TL334;_TL333: _TL334: _T2=term_dict;_T3=*_T2;
# 2592
return _T3;}
# 2600
struct _tuple14 Cyc_AssnDef_unify_var_maps(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){struct Cyc_AssnDef_AssnMap _T0;void*_T1;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T3;void*_T4;struct _tuple14 _T5;struct Cyc_AssnDef_AssnMap _T6;struct Cyc_AssnDef_AssnMap _T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_AssnDef_AssnMap _T9;void*_TA;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TB;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TC;void*_TD;struct _tuple14 _TE;struct Cyc_AssnDef_AssnMap _TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;struct Cyc_AssnDef_Accum _T12;struct Cyc_AssnDef_AssnMap _T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_AssnDef_Accum*(*_T17)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*);void*(*_T18)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T19;struct Cyc_Dict_Dict _T1A;struct Cyc_AssnDef_Accum*_T1B;struct Cyc_AssnDef_Accum*_T1C;struct Cyc_AssnDef_Accum*(*_T1D)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*);void*(*_T1E)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T1F;struct Cyc_Dict_Dict _T20;struct Cyc_AssnDef_AssnMap _T21;struct Cyc_Dict_Dict _T22;struct Cyc_Dict_Dict _T23;struct Cyc_AssnDef_Accum*_T24;struct Cyc_AssnDef_Accum*_T25;struct _tuple14 _T26;struct Cyc_AssnDef_Accum _T27;struct Cyc_AssnDef_Accum _T28;struct Cyc_AssnDef_Accum _T29;_T0=am1;_T1=_T0.assn;_T2=& Cyc_AssnDef_false_assn;_T3=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T2;_T4=(void*)_T3;
if(_T1!=_T4)goto _TL335;{struct _tuple14 _T2A;_T6=am1;_T2A.f0=_T6.assn;_T7=am2;_T2A.f1=_T7.assn;_T8=am2;_T2A.f2=_T8.map;_T5=_T2A;}return _T5;_TL335: _T9=am2;_TA=_T9.assn;_TB=& Cyc_AssnDef_false_assn;_TC=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TB;_TD=(void*)_TC;
if(_TA!=_TD)goto _TL337;{struct _tuple14 _T2A;_TF=am1;_T2A.f0=_TF.assn;_T10=am2;_T2A.f1=_T10.assn;_T11=am1;_T2A.f2=_T11.map;_TE=_T2A;}return _TE;_TL337:{struct Cyc_AssnDef_Accum _T2A;_T13=am1;
_T2A.a1=_T13.assn;_T14=am2;_T2A.a2=_T14.assn;_T15=am1;_T2A.m1=_T15.map;_T16=am2;_T2A.m2=_T16.map;_T2A.m=Cyc_AssnDef_empty_map();_T12=_T2A;}{struct Cyc_AssnDef_Accum accum=_T12;_T18=Cyc_Dict_fold;{
struct Cyc_AssnDef_Accum*(*_T2A)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))_T18;_T17=_T2A;}_T19=am1;_T1A=_T19.map;_T1B=& accum;_T1C=(struct Cyc_AssnDef_Accum*)_T1B;_T17(Cyc_AssnDef_foldm1,_T1A,_T1C);_T1E=Cyc_Dict_fold;{
struct Cyc_AssnDef_Accum*(*_T2A)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))_T1E;_T1D=_T2A;}_T1F=am2;_T20=_T1F.map;_T21=am1;_T22=_T21.map;_T23=Cyc_Dict_difference(_T20,_T22);_T24=& accum;_T25=(struct Cyc_AssnDef_Accum*)_T24;_T1D(Cyc_AssnDef_foldm2,_T23,_T25);{struct _tuple14 _T2A;_T27=accum;
_T2A.f0=_T27.a1;_T28=accum;_T2A.f1=_T28.a2;_T29=accum;_T2A.f2=_T29.m;_T26=_T2A;}return _T26;}}
# 2610
static struct Cyc_Dict_Dict*Cyc_AssnDef_collect_subst(struct Cyc_Dict_Dict*map2,struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_Dict_Dict*dict){void*_T0;int*_T1;int _T2;void**(*_T3)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T5;struct Cyc_Dict_Dict _T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict _T9;void*_TA;void**_TB;void**_TC;void*_TD;void*_TE;struct Cyc_Dict_Dict*_TF;struct Cyc_Dict_Dict*_T10;struct Cyc_Dict_Dict _T11;void*_T12;void**_T13;void*_T14;struct Cyc_Dict_Dict*_T15;struct Cyc_Dict_Dict*_T16;struct Cyc_Dict_Dict _T17;void*_T18;struct Cyc_Absyn_Vardecl*_T19;struct Cyc_Absyn_Vardecl*_T1A;void*_T1B;void*_T1C;struct Cyc_Dict_Dict*_T1D;_T0=value;_T1=(int*)_T0;_T2=*_T1;if(_T2!=2)goto _TL339;_T4=Cyc_Dict_lookup_opt;{
# 2616
void**(*_T1E)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T4;_T3=_T1E;}_T5=map2;_T6=*_T5;_T7=vd;{void**v=_T3(_T6,_T7);_T8=dict;_T9=*_T8;_TA=value;_TB=
# 2619
Cyc_Dict_lookup_opt(_T9,_TA);if(_TB!=0)goto _TL33B;
if(v==0)goto _TL33D;_TC=v;_TD=*_TC;_TE=value;if(_TD!=_TE)goto _TL33D;{struct Cyc_Dict_Dict*_T1E=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T10=dict;_T11=*_T10;_T12=value;_T13=v;_T14=*_T13;
*_T1E=Cyc_Dict_insert(_T11,_T12,_T14);_TF=(struct Cyc_Dict_Dict*)_T1E;}return _TF;_TL33D:{struct Cyc_Dict_Dict*_T1E=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T16=dict;_T17=*_T16;_T18=value;_T19=vd;_T1A=vd;_T1B=_T1A->type;_T1C=
Cyc_AssnDef_fresh_var(_T19,_T1B);*_T1E=Cyc_Dict_insert(_T17,_T18,_T1C);_T15=(struct Cyc_Dict_Dict*)_T1E;}return _T15;_TL33B: goto _LL0;}_TL339: goto _LL0;_LL0: _T1D=dict;
# 2628
return _T1D;}
# 2638 "assndef.cyc"
struct _tuple14 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){struct Cyc_AssnDef_AssnMap _T0;void*_T1;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T3;void*_T4;struct _tuple14 _T5;struct Cyc_AssnDef_AssnMap _T6;struct Cyc_AssnDef_AssnMap _T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_AssnDef_AssnMap _T9;void*_TA;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TB;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TC;void*_TD;struct _tuple14 _TE;struct Cyc_AssnDef_AssnMap _TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;struct Cyc_Dict_Dict*(*_T12)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*);void*(*_T13)(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T14;struct Cyc_Dict_Dict*_T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_Dict_Dict _T17;struct Cyc_Dict_Dict*_T18;struct Cyc_Dict_Dict*_T19;struct _tuple14 _T1A;_T0=am1;_T1=_T0.assn;_T2=& Cyc_AssnDef_false_assn;_T3=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T2;_T4=(void*)_T3;
if(_T1!=_T4)goto _TL33F;{struct _tuple14 _T1B;_T6=am1;_T1B.f0=_T6.assn;_T7=am2;_T1B.f1=_T7.assn;_T8=am2;_T1B.f2=_T8.map;_T5=_T1B;}return _T5;_TL33F: _T9=am2;_TA=_T9.assn;_TB=& Cyc_AssnDef_false_assn;_TC=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TB;_TD=(void*)_TC;
if(_TA!=_TD)goto _TL341;{struct _tuple14 _T1B;_TF=am1;_T1B.f0=_TF.assn;_T10=am2;_T1B.f1=_T10.assn;_T11=am1;_T1B.f2=_T11.map;_TE=_T1B;}return _TE;_TL341: _T13=Cyc_Dict_fold_c;{
# 2644
struct Cyc_Dict_Dict*(*_T1B)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))_T13;_T12=_T1B;}_T14=& am2.map;_T15=(struct Cyc_Dict_Dict*)_T14;_T16=am1;_T17=_T16.map;{struct Cyc_Dict_Dict*_T1B=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_T1B=Cyc_AssnDef_empty_term_dict();_T18=(struct Cyc_Dict_Dict*)_T1B;}_T19=_T12(Cyc_AssnDef_collect_subst,_T15,_T17,_T18);{struct Cyc_Dict_Dict subst_dict=*_T19;
struct Cyc_AssnDef_AssnMap _T1B=Cyc_AssnDef_subst_am(subst_dict,am2);struct Cyc_AssnDef_AssnMap am2=_T1B;_T1A=
Cyc_AssnDef_unify_var_maps(am1,am2);return _T1A;}}
# 2649
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){struct Cyc_AssnDef_AssnMap _T0;
struct _tuple14 _T1=Cyc_AssnDef_unify_var_maps(am1,am2);struct Cyc_Dict_Dict _T2;void*_T3;void*_T4;_T4=_T1.f0;_T3=_T1.f1;_T2=_T1.f2;{void*a1=_T4;void*a2=_T3;struct Cyc_Dict_Dict map=_T2;{struct Cyc_AssnDef_AssnMap _T5;
_T5.assn=Cyc_AssnDef_or(a1,a2);_T5.map=map;_T0=_T5;}return _T0;}}struct Cyc_AssnDef_Canonical_am_accum{void*assn;struct Cyc_Dict_Dict map;struct Cyc_Set_Set*used_lvars;};
# 2661
static struct Cyc_AssnDef_Canonical_am_accum*Cyc_AssnDef_fold_canonical(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_AssnDef_Canonical_am_accum*accum){void*_T0;int*_T1;int _T2;struct Cyc_AssnDef_Canonical_am_accum*_T3;struct Cyc_Set_Set*_T4;void*_T5;int _T6;struct Cyc_AssnDef_Canonical_am_accum*_T7;struct Cyc_AssnDef_Canonical_am_accum*_T8;struct Cyc_Set_Set*_T9;void*_TA;struct Cyc_AssnDef_Canonical_am_accum*_TB;struct Cyc_Dict_Dict(*_TC)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TD)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Canonical_am_accum*_TE;struct Cyc_Dict_Dict _TF;struct Cyc_Absyn_Vardecl*_T10;void*_T11;struct Cyc_AssnDef_Canonical_am_accum*_T12;struct Cyc_Absyn_Vardecl*_T13;struct Cyc_Absyn_Vardecl*_T14;void*_T15;struct Cyc_AssnDef_Canonical_am_accum*_T16;struct Cyc_AssnDef_Canonical_am_accum*_T17;struct Cyc_Set_Set*_T18;void*_T19;struct Cyc_AssnDef_Canonical_am_accum*_T1A;struct Cyc_Dict_Dict(*_T1B)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_AssnDef_Canonical_am_accum*_T1D;struct Cyc_Dict_Dict _T1E;struct Cyc_Absyn_Vardecl*_T1F;void*_T20;struct Cyc_AssnDef_Canonical_am_accum*_T21;struct Cyc_AssnDef_Canonical_am_accum*_T22;void*_T23;void*_T24;struct Cyc_AssnDef_Canonical_am_accum*_T25;_T0=value;_T1=(int*)_T0;_T2=*_T1;if(_T2!=2)goto _TL343;_T3=accum;_T4=_T3->used_lvars;_T5=value;_T6=
# 2664
Cyc_Set_member(_T4,_T5);if(_T6)goto _TL345;else{goto _TL347;}
_TL347: _T7=accum;_T8=accum;_T9=_T8->used_lvars;_TA=value;_T7->used_lvars=Cyc_Set_insert(_T9,_TA);_TB=accum;_TD=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TD;_TC=_T26;}_TE=accum;_TF=_TE->map;_T10=vd;_T11=value;_TB->map=_TC(_TF,_T10,_T11);_T12=accum;
return _T12;_TL345: goto _LL0;_TL343: goto _LL0;_LL0: _T13=vd;_T14=vd;_T15=_T14->type;{
# 2673
void*v=Cyc_AssnDef_fresh_var(_T13,_T15);_T16=accum;_T17=accum;_T18=_T17->used_lvars;_T19=v;
_T16->used_lvars=Cyc_Set_insert(_T18,_T19);_T1A=accum;_T1C=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T26)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T1C;_T1B=_T26;}_T1D=accum;_T1E=_T1D->map;_T1F=vd;_T20=v;_T1A->map=_T1B(_T1E,_T1F,_T20);_T21=accum;_T22=accum;_T23=_T22->assn;_T24=
Cyc_AssnDef_eq(v,value);_T21->assn=Cyc_AssnDef_and(_T23,_T24);_T25=accum;
return _T25;}}
# 2684
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap am){struct Cyc_AssnDef_Canonical_am_accum*_T0;struct Cyc_AssnDef_AssnMap _T1;struct Cyc_AssnDef_Canonical_am_accum*_T2;struct Cyc_AssnDef_Canonical_am_accum*_T3;int(*_T4)(void*,void*);struct Cyc_AssnDef_Canonical_am_accum*(*_T5)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*);void*(*_T6)(void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T7;struct Cyc_Dict_Dict _T8;struct Cyc_AssnDef_Canonical_am_accum*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_AssnDef_Canonical_am_accum*_TB;struct Cyc_AssnDef_Canonical_am_accum*_TC;
struct Cyc_AssnDef_Canonical_am_accum*accum;accum=_cycalloc(sizeof(struct Cyc_AssnDef_Canonical_am_accum));_T0=accum;_T1=am;_T0->assn=_T1.assn;_T2=accum;_T2->map=Cyc_AssnDef_empty_map();_T3=accum;_T4=Cyc_Core_ptrcmp;_T3->used_lvars=Cyc_Set_empty(_T4);_T6=Cyc_Dict_fold;{
struct Cyc_AssnDef_Canonical_am_accum*(*_TD)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*)=(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*))_T6;_T5=_TD;}_T7=am;_T8=_T7.map;_T9=accum;_T5(Cyc_AssnDef_fold_canonical,_T8,_T9);{struct Cyc_AssnDef_AssnMap _TD;_TB=accum;
_TD.assn=_TB->assn;_TC=accum;_TD.map=_TC->map;_TA=_TD;}return _TA;}struct _tuple21{void*f0;struct Cyc_Set_Set*f1;};
# 2690
static struct Cyc_Dict_Dict*Cyc_AssnDef_force_it(struct _tuple21*env,struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_Dict_Dict*accum){int(*_T0)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);int(*_T1)(struct Cyc_Set_Set*,void*);struct _tuple21*_T2;struct Cyc_Set_Set*_T3;struct Cyc_Absyn_Vardecl*_T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;struct _tuple21*_T9;struct _tuple21*_TA;void*_TB;void*_TC;struct Cyc_Dict_Dict*_TD;struct Cyc_Dict_Dict(*_TE)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_TF)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T10;struct Cyc_Dict_Dict _T11;struct Cyc_Absyn_Vardecl*_T12;void*_T13;struct Cyc_Dict_Dict*_T14;struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_T17;struct Cyc_Dict_Dict _T18;struct Cyc_Absyn_Vardecl*_T19;void*_T1A;struct Cyc_Dict_Dict*_T1B;_T1=Cyc_Set_member;{
int(*_T1C)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T1C;}_T2=env;_T3=_T2->f1;_T4=vd;_T5=_T0(_T3,_T4);if(!_T5)goto _TL348;_T6=vd;_T7=vd;_T8=_T7->type;{
void*new_term=Cyc_AssnDef_fresh_var(_T6,_T8);_T9=env;_TA=env;_TB=_TA->f0;_TC=
Cyc_AssnDef_eq(new_term,t);_T9->f0=Cyc_AssnDef_and(_TB,_TC);_TD=accum;_TF=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_TF;_TE=_T1C;}_T10=accum;_T11=*_T10;_T12=vd;_T13=new_term;*_TD=_TE(_T11,_T12,_T13);}goto _TL349;
# 2696
_TL348: _T14=accum;_T16=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T1C)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T16;_T15=_T1C;}_T17=accum;_T18=*_T17;_T19=vd;_T1A=t;*_T14=_T15(_T18,_T19,_T1A);_TL349: _T1B=accum;
# 2698
return _T1B;}
# 2702
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_force_canonical(struct Cyc_Set_Set*vds,struct Cyc_AssnDef_AssnMap am){struct Cyc_Set_Set*(*_T0)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T1)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*(*_T2)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T3)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_AssnDef_AssnMap _T6;struct _tuple21 _T7;struct Cyc_Dict_Dict*(*_T8)(struct Cyc_Dict_Dict*(*)(struct _tuple21*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct _tuple21*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*);void*(*_T9)(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,void*);struct _tuple21*_TA;struct Cyc_AssnDef_AssnMap _TB;struct Cyc_Dict_Dict _TC;struct Cyc_Dict_Dict*_TD;struct Cyc_Dict_Dict*_TE;struct Cyc_AssnDef_AssnMap _TF;struct _tuple21 _T10;_T1=Cyc_Set_insert;{
struct Cyc_Set_Set*(*_T11)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T11;}vds=_T0(vds,Cyc_AssnDef_memory);_T3=Cyc_Set_insert;{
struct Cyc_Set_Set*(*_T11)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T3;_T2=_T11;}_T4=vds;_T5=Cyc_AssnDef_exception_vardecl();vds=_T2(_T4,_T5);_T6=am;{
void*assn=_T6.assn;{struct _tuple21 _T11;
_T11.f0=assn;_T11.f1=vds;_T7=_T11;}{struct _tuple21 env=_T7;
struct Cyc_Dict_Dict map=Cyc_AssnDef_empty_map();_T9=Cyc_Dict_fold_c;{
struct Cyc_Dict_Dict*(*_T11)(struct Cyc_Dict_Dict*(*)(struct _tuple21*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct _tuple21*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple21*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct _tuple21*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))_T9;_T8=_T11;}_TA=& env;_TB=am;_TC=_TB.map;_TD=& map;_TE=(struct Cyc_Dict_Dict*)_TD;_T8(Cyc_AssnDef_force_it,_TA,_TC,_TE);{struct Cyc_AssnDef_AssnMap _T11;_T10=env;
_T11.assn=_T10.f0;_T11.map=map;_TF=_T11;}return _TF;}}}
# 2720 "assndef.cyc"
void*Cyc_AssnDef_merge_assnmaps(struct Cyc_List_List*vds1,struct Cyc_AssnDef_AssnMap a1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap a2){void**(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;void**(*_T5)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T6)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T7;struct Cyc_Dict_Dict _T8;struct Cyc_Absyn_Vardecl*_T9;struct Cyc_Dict_Dict _TA;void**_TB;void*_TC;void**_TD;void*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Absyn_Vardecl*_T14;void**(*_T15)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T16)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T17;struct Cyc_Dict_Dict _T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_Absyn_Vardecl*_T1B;void**(*_T1C)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1D)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T1E;struct Cyc_Dict_Dict _T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_Vardecl*_T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_Dict_Dict _T24;void**_T25;void*_T26;void**_T27;void*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;int _T2B;struct Cyc_AssnDef_AssnMap _T2C;void*_T2D;struct Cyc_Dict_Dict _T2E;struct Cyc_AssnDef_AssnMap _T2F;void*_T30;void*_T31;
# 2722
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();_T1=Cyc_Dict_lookup_opt;{
# 2724
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T32;}_T2=a1;_T3=_T2.map;_T4=Cyc_AssnDef_memory;{void**v1=_T0(_T3,_T4);_T6=Cyc_Dict_lookup_opt;{
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T6;_T5=_T32;}_T7=a2;_T8=_T7.map;_T9=Cyc_AssnDef_memory;{void**v2=_T5(_T8,_T9);
if(v1==0)goto _TL34A;if(v2==0)goto _TL34A;_TA=dict;_TB=v2;_TC=*_TB;_TD=v1;_TE=*_TD;
dict=Cyc_Dict_insert(_TA,_TC,_TE);goto _TL34B;_TL34A: _TL34B:
# 2729
 _TL34F: if(vds1!=0)goto _TL350;else{goto _TL34E;}_TL350: if(vds2!=0)goto _TL34D;else{goto _TL34E;}
_TL34D: _TF=vds1;_T10=_TF->hd;_T11=(struct Cyc_Absyn_Vardecl*)_T10;if(_T11==0)goto _TL351;_T12=vds2;_T13=_T12->hd;_T14=(struct Cyc_Absyn_Vardecl*)_T13;if(_T14==0)goto _TL351;_T16=Cyc_Dict_lookup_opt;{
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T16;_T15=_T32;}_T17=a1;_T18=_T17.map;_T19=vds1;_T1A=_T19->hd;_T1B=(struct Cyc_Absyn_Vardecl*)_T1A;{void**v1=_T15(_T18,_T1B);_T1D=Cyc_Dict_lookup_opt;{
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1D;_T1C=_T32;}_T1E=a2;_T1F=_T1E.map;_T20=vds2;_T21=_T20->hd;_T22=(struct Cyc_Absyn_Vardecl*)_T21;_T23=_check_null(_T22);{void**v2=_T1C(_T1F,_T23);
if(v1==0)goto _TL353;if(v2==0)goto _TL353;_T24=dict;_T25=v2;_T26=*_T25;_T27=v1;_T28=*_T27;
dict=Cyc_Dict_insert(_T24,_T26,_T28);goto _TL354;_TL353: _TL354:;}}goto _TL352;_TL351: _TL352: _T29=vds1;
# 2729
vds1=_T29->tl;_T2A=vds2;vds2=_T2A->tl;goto _TL34F;_TL34E: _T2B=
# 2744
Cyc_Dict_is_empty(dict);if(!_T2B)goto _TL355;_T2C=a2;_T2D=_T2C.assn;return _T2D;
_TL355: _T2E=dict;_T2F=a2;_T30=_T2F.assn;_T31=Cyc_AssnDef_subst_a(_T2E,_T30);return _T31;}}}
# 2756 "assndef.cyc"
extern struct Cyc_AssnDef_AssnMap*Cyc_AssnDef_subst_vardecls(struct Cyc_List_List*vds1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap*a){void**(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap*_T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;struct Cyc_Dict_Dict(*_T5)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T6)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict _T7;void**_T8;void*_T9;void*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_Absyn_Vardecl*_T10;void**(*_T11)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T12)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap*_T13;struct Cyc_Dict_Dict _T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_Absyn_Vardecl*_T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_List_List*_T1B;void*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Absyn_Vardecl*_T1E;void*_T1F;struct Cyc_Dict_Dict(*_T20)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*);struct Cyc_Dict_Dict(*_T21)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict _T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Vardecl*_T26;void*_T27;struct Cyc_Dict_Dict _T28;void**_T29;void*_T2A;void*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_Dict_Dict _T2E;struct Cyc_AssnDef_AssnMap*_T2F;void*_T30;struct Cyc_AssnDef_AssnMap*_T31;
# 2759
if(a!=0)goto _TL357;return 0;_TL357: {
struct Cyc_Dict_Dict newmap=Cyc_AssnDef_empty_map();
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();_T1=Cyc_Dict_lookup_opt;{
# 2763
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T32;}_T2=a;_T3=_T2->map;_T4=Cyc_AssnDef_memory;{void**v=_T0(_T3,_T4);
if(v==0)goto _TL359;{
void*u=Cyc_AssnDef_fresh_var(Cyc_AssnDef_memory,0);_T6=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T6;_T5=_T32;}newmap=_T5(newmap,Cyc_AssnDef_memory,u);_T7=dict;_T8=v;_T9=*_T8;_TA=u;
dict=Cyc_Dict_insert(_T7,_T9,_TA);}goto _TL35A;_TL359: _TL35A:
# 2769
 _TL35E: if(vds1!=0)goto _TL35F;else{goto _TL35D;}_TL35F: if(vds2!=0)goto _TL35C;else{goto _TL35D;}
_TL35C: _TB=vds1;_TC=_TB->hd;_TD=(struct Cyc_Absyn_Vardecl*)_TC;if(_TD==0)goto _TL360;_TE=vds2;_TF=_TE->hd;_T10=(struct Cyc_Absyn_Vardecl*)_TF;if(_T10==0)goto _TL360;_T12=Cyc_Dict_lookup_opt;{
void**(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T12;_T11=_T32;}_T13=a;_T14=_T13->map;_T15=vds1;_T16=_T15->hd;_T17=(struct Cyc_Absyn_Vardecl*)_T16;{void**v=_T11(_T14,_T17);
if(v==0)goto _TL362;_T18=vds2;_T19=_T18->hd;_T1A=(struct Cyc_Absyn_Vardecl*)_T19;_T1B=vds2;_T1C=_T1B->hd;_T1D=(struct Cyc_Absyn_Vardecl*)_T1C;_T1E=
_check_null(_T1D);_T1F=_T1E->type;{void*u=Cyc_AssnDef_fresh_var(_T1A,_T1F);_T21=Cyc_Dict_insert;{
# 2775
struct Cyc_Dict_Dict(*_T32)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))_T21;_T20=_T32;}_T22=newmap;_T23=vds2;_T24=_T23->hd;_T25=(struct Cyc_Absyn_Vardecl*)_T24;_T26=_check_null(_T25);_T27=u;newmap=_T20(_T22,_T26,_T27);_T28=dict;_T29=v;_T2A=*_T29;_T2B=u;
# 2777
dict=Cyc_Dict_insert(_T28,_T2A,_T2B);}goto _TL363;_TL362: _TL363:;}goto _TL361;_TL360: _TL361: _T2C=vds1;
# 2769
vds1=_T2C->tl;_T2D=vds2;vds2=_T2D->tl;goto _TL35E;_TL35D: _T2E=dict;_T2F=a;_T30=_T2F->assn;{
# 2782
void*newa=Cyc_AssnDef_subst_a(_T2E,_T30);{struct Cyc_AssnDef_AssnMap*_T32=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
_T32->assn=newa;_T32->map=newmap;_T31=(struct Cyc_AssnDef_AssnMap*)_T32;}return _T31;}}}}
# 2791
void*Cyc_AssnDef_subst_args(struct Cyc_List_List*args,struct Cyc_Absyn_Vardecl*res,struct Cyc_List_List*actual_args,void*actual_res,void*mem,struct Cyc_AssnDef_AssnMap am){void**(*_T0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T2;struct Cyc_Dict_Dict _T3;struct Cyc_Absyn_Vardecl*_T4;struct Cyc_Dict_Dict _T5;void**_T6;void*_T7;void*_T8;void**(*_T9)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_TA)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _TB;struct Cyc_Dict_Dict _TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_Dict_Dict _TE;void**_TF;void*_T10;void*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Absyn_Vardecl*_T14;struct Cyc_List_List*_T15;void*_T16;void**(*_T17)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*);void**(*_T18)(struct Cyc_Dict_Dict,void*);struct Cyc_AssnDef_AssnMap _T19;struct Cyc_Dict_Dict _T1A;struct Cyc_List_List*_T1B;void*_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Dict_Dict _T1E;void**_T1F;void*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_Dict_Dict _T25;struct Cyc_AssnDef_AssnMap _T26;void*_T27;void*_T28;
# 2795
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();_T1=Cyc_Dict_lookup_opt;{
void**(*_T29)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T1;_T0=_T29;}_T2=am;_T3=_T2.map;_T4=Cyc_AssnDef_memory;{void**mem_val=_T0(_T3,_T4);
if(mem_val==0)goto _TL364;_T5=dict;_T6=mem_val;_T7=*_T6;_T8=mem;
dict=Cyc_Dict_insert(_T5,_T7,_T8);goto _TL365;_TL364: _TL365:
 if(res==0)goto _TL366;if(actual_res==0)goto _TL366;_TA=Cyc_Dict_lookup_opt;{
void**(*_T29)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_TA;_T9=_T29;}_TB=am;_TC=_TB.map;_TD=res;{void**res_val=_T9(_TC,_TD);
if(res_val==0)goto _TL368;_TE=dict;_TF=res_val;_T10=*_TF;_T11=actual_res;
dict=Cyc_Dict_insert(_TE,_T10,_T11);goto _TL369;_TL368: _TL369:;}goto _TL367;_TL366: _TL367:
# 2804
 _TL36D: if(args!=0)goto _TL36E;else{goto _TL36C;}_TL36E: if(actual_args!=0)goto _TL36B;else{goto _TL36C;}
_TL36B: _T12=args;_T13=_T12->hd;_T14=(struct Cyc_Absyn_Vardecl*)_T13;if(_T14==0)goto _TL36F;_T15=actual_args;_T16=_T15->hd;if(_T16==0)goto _TL36F;_T18=Cyc_Dict_lookup_opt;{
void**(*_T29)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))_T18;_T17=_T29;}_T19=am;_T1A=_T19.map;_T1B=args;_T1C=_T1B->hd;_T1D=(struct Cyc_Absyn_Vardecl*)_T1C;{void**arg_val=_T17(_T1A,_T1D);
if(arg_val==0)goto _TL371;_T1E=dict;_T1F=arg_val;_T20=*_T1F;_T21=actual_args;_T22=_T21->hd;
dict=Cyc_Dict_insert(_T1E,_T20,_T22);goto _TL372;_TL371: _TL372:;}goto _TL370;_TL36F: _TL370: _T23=args;
# 2804
args=_T23->tl;_T24=actual_args;actual_args=_T24->tl;goto _TL36D;_TL36C: _T25=dict;_T26=am;_T27=_T26.assn;_T28=
# 2811
Cyc_AssnDef_subst_a(_T25,_T27);return _T28;}}
# 2814
void Cyc_AssnDef_reset_hash_cons_table (void){
# 2818
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_assn_hash_cons_table=0;}
# 2822
static void Cyc_AssnDef_f_sizeof_hashtable(void*key,void*value,int*counter){int*_T0;_T0=counter;
*_T0=*_T0 + 1;}
# 2826
int Cyc_AssnDef_sizeof_hash_cons_table (void){void(*_T0)(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*);void(*_T1)(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);void(*_T2)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*);struct Cyc_Hashtable_Table*_T3;int*_T4;void(*_T5)(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*);void(*_T6)(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);void(*_T7)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*);struct Cyc_Hashtable_Table*_T8;int*_T9;int _TA;
int size=0;
if(Cyc_AssnDef_term_hash_cons_table==0)goto _TL373;_T1=Cyc_Hashtable_iter_c;{
void(*_TB)(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*)=(void(*)(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*))_T1;_T0=_TB;}{void(*_TB)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*)=(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*))Cyc_AssnDef_f_sizeof_hashtable;_T2=_TB;}_T3=Cyc_AssnDef_term_hash_cons_table;_T4=& size;_T0(_T2,_T3,_T4);goto _TL374;_TL373: _TL374:
# 2831
 if(Cyc_AssnDef_assn_hash_cons_table==0)goto _TL375;_T6=Cyc_Hashtable_iter_c;{
void(*_TB)(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*)=(void(*)(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*))_T6;_T5=_TB;}{void(*_TB)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*)=(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*))Cyc_AssnDef_f_sizeof_hashtable;_T7=_TB;}_T8=Cyc_AssnDef_assn_hash_cons_table;_T9=& size;_T5(_T7,_T8,_T9);goto _TL376;_TL375: _TL376: _TA=size;
# 2834
return _TA;}
# 2837
struct Cyc_Hashtable_Table*Cyc_AssnDef_empty_term_table (void){struct Cyc_Hashtable_Table*_T0;_T0=
Cyc_Hashtable_create(33,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);return _T0;}
# 2841
void*Cyc_AssnDef_subst_table(struct Cyc_Hashtable_Table*table,void*t){void**_T0;void*_T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;void*_T7;struct Cyc_AssnDef_Addr_AssnDef_Term_struct _T8;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T9;struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_TA;void*_TB;void*_TC;void*_TD;void*_TE;void*_TF;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T10;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T11;struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;int _T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;
void**sopt=Cyc_Hashtable_lookup_opt(table,t);
void*told=t;
if(sopt==0)goto _TL377;_T0=sopt;_T1=*_T0;return _T1;_TL377:{unsigned _T3C;enum Cyc_Absyn_Primop _T3D;void*_T3E;int _T3F;void*_T40;void*_T41;void*_T42;_T2=t;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 2: goto _LL0;case 0: goto _LL0;case 1: goto _LL0;case 11: _T5=t;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T5;_T42=_T43->f1;_T6=_T43->f2;_T41=(void*)_T6;_T7=_T43->f3;_T40=(void*)_T7;}{struct Cyc_Absyn_Vardecl*vd=_T42;void*t1=_T41;void*tp=_T40;
# 2850
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL37A;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct _T43;_T43.tag=11;
_T43.f1=vd;_T43.f2=s1;_T43.f3=tp;_T8=_T43;}{struct Cyc_AssnDef_Addr_AssnDef_Term_struct a=_T8;_T9=& a;_TA=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T9;_TB=(void*)_TA;
t=Cyc_AssnDef_hash_cons_term(_TB);}goto _TL37B;_TL37A: _TL37B: goto _LL0;}case 12: _TC=t;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_TC;_T42=_T43->f1;_T3F=_T43->f2;_TD=_T43->f3;_T41=(void*)_TD;_TE=_T43->f4;_T40=(void*)_TE;_TF=_T43->f5;_T3E=(void*)_TF;}{struct Cyc_Absyn_Exp*e1=_T42;int i=_T3F;void*t1=_T41;void*t2=_T40;void*tp=_T3E;
# 2857
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1)goto _TL37E;else{goto _TL37F;}_TL37F: if(s2!=t2)goto _TL37E;else{goto _TL37C;}
_TL37E:{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _T43;_T43.tag=12;_T43.f1=e1;_T43.f2=i;_T43.f3=s1;_T43.f4=s2;_T43.f5=tp;_T10=_T43;}{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct a=_T10;_T11=& a;_T12=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T11;_T13=(void*)_T12;
t=Cyc_AssnDef_hash_cons_term(_T13);}goto _TL37D;_TL37C: _TL37D: goto _LL0;}case 6: _T14=t;{struct Cyc_AssnDef_Select_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_T14;_T15=_T43->f1;_T42=(void*)_T15;_T16=_T43->f2;_T41=(void*)_T16;_T17=_T43->f3;_T40=(void*)_T17;}{void*t1=_T42;void*t2=_T41;void*tp=_T40;
# 2865
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1)goto _TL382;else{goto _TL383;}_TL383: if(s2!=t2)goto _TL382;else{goto _TL380;}_TL382: t=Cyc_AssnDef_select(s1,s2,tp);goto _TL381;_TL380: _TL381: goto _LL0;}case 7: _T18=t;{struct Cyc_AssnDef_Update_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_T18;_T19=_T43->f1;_T42=(void*)_T19;_T1A=_T43->f2;_T41=(void*)_T1A;_T1B=_T43->f3;_T40=(void*)_T1B;}{void*t1=_T42;void*t2=_T41;void*t3=_T40;
# 2870
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
void*s3=Cyc_AssnDef_subst_table(table,t3);
if(s1!=t1)goto _TL386;else{goto _TL388;}_TL388: if(s2!=t2)goto _TL386;else{goto _TL387;}_TL387: if(s3!=t3)goto _TL386;else{goto _TL384;}_TL386: t=Cyc_AssnDef_update(s1,s2,s3);goto _TL385;_TL384: _TL385: goto _LL0;}case 3: _T1C=t;{struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_T1C;_T3D=_T43->f1;_T1D=_T43->f2;_T42=(void*)_T1D;_T1E=_T43->f3;_T41=(void*)_T1E;}{enum Cyc_Absyn_Primop p=_T3D;void*t1=_T42;void*tp=_T41;
# 2876
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL389;t=Cyc_AssnDef_unop(p,s1,tp);goto _TL38A;_TL389: _TL38A: goto _LL0;}case 4: _T1F=t;{struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_T1F;_T3D=_T43->f1;_T20=_T43->f2;_T42=(void*)_T20;_T21=_T43->f3;_T41=(void*)_T21;_T22=_T43->f4;_T40=(void*)_T22;}{enum Cyc_Absyn_Primop p=_T3D;void*t1=_T42;void*t2=_T41;void*tp=_T40;
# 2880
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1)goto _TL38D;else{goto _TL38E;}_TL38E: if(s2!=t2)goto _TL38D;else{goto _TL38B;}_TL38D: t=Cyc_AssnDef_binop(p,s1,s2,tp);goto _TL38C;_TL38B: _TL38C: goto _LL0;}case 5: _T23=t;{struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_T23;_T24=_T43->f1;_T42=(void*)_T24;_T25=_T43->f2;_T41=(void*)_T25;}{void*tp=_T42;void*t1=_T41;
# 2885
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL38F;t=Cyc_AssnDef_cast(tp,s1);goto _TL390;_TL38F: _TL390: goto _LL0;}case 8: _T26=t;{struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_T26;_T3F=_T43->f1;_T3C=_T43->f2;_T42=_T43->f3;_T27=_T43->f4;_T41=(void*)_T27;}{int b=_T3F;unsigned tag=_T3C;struct Cyc_List_List*tlist=_T42;void*tp=_T41;
# 2889
struct Cyc_List_List*slist=0;
int changed=0;
_TL394: if(tlist!=0)goto _TL392;else{goto _TL393;}
_TL392: _T28=tlist;{void*t1=_T28->hd;
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL395;changed=1;goto _TL396;_TL395: _TL396:{struct Cyc_List_List*_T43=_cycalloc(sizeof(struct Cyc_List_List));
_T43->hd=s1;_T43->tl=slist;_T29=(struct Cyc_List_List*)_T43;}slist=_T29;}_T2A=tlist;
# 2891
tlist=_T2A->tl;goto _TL394;_TL393: _T2B=changed;
# 2897
if(!_T2B)goto _TL397;
Cyc_List_imp_rev(slist);
t=Cyc_AssnDef_aggr(b,tag,slist,tp);goto _TL398;_TL397: _TL398: goto _LL0;}case 9: _T2C=t;{struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_T2C;_T2D=_T43->f1;_T42=(void*)_T2D;_T3C=_T43->f2;_T2E=_T43->f3;_T41=(void*)_T2E;}{void*t1=_T42;unsigned i=_T3C;void*tp=_T41;
# 2903
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL399;t=Cyc_AssnDef_proj(s1,i,tp);goto _TL39A;_TL399: _TL39A: goto _LL0;}case 10: _T2F=t;{struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_T2F;_T30=_T43->f1;_T42=(void*)_T30;_T3C=_T43->f2;_T31=_T43->f3;_T41=(void*)_T31;}{void*t1=_T42;unsigned i=_T3C;void*t2=_T41;
# 2907
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1)goto _TL39D;else{goto _TL39E;}_TL39E: if(s2!=t2)goto _TL39D;else{goto _TL39B;}_TL39D: t=Cyc_AssnDef_aggr_update(s1,i,s2);goto _TL39C;_TL39B: _TL39C: goto _LL0;}case 13: _T32=t;{struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_T32;_T33=_T43->f1;_T42=(void*)_T33;_T3C=_T43->f2;_T34=_T43->f3;_T41=(void*)_T34;}{void*t1=_T42;unsigned i=_T3C;void*tp=_T41;
# 2912
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL39F;t=Cyc_AssnDef_offsetf(s1,i,tp);goto _TL3A0;_TL39F: _TL3A0: goto _LL0;}case 14: _T35=t;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T35;_T36=_T43->f1;_T42=(void*)_T36;_T37=_T43->f2;_T41=(void*)_T37;_T38=_T43->f3;_T40=(void*)_T38;}{void*t1=_T42;void*t2=_T41;void*tp=_T40;
# 2916
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1)goto _TL3A3;else{goto _TL3A4;}_TL3A4: if(s2!=t2)goto _TL3A3;else{goto _TL3A1;}_TL3A3: t=Cyc_AssnDef_offseti(s1,s2,tp);goto _TL3A2;_TL3A1: _TL3A2: goto _LL0;}default: _T39=t;{struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_T43=(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_T39;_T3A=_T43->f1;_T42=(void*)_T3A;}{void*t1=_T42;
# 2921
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1==t1)goto _TL3A5;t=Cyc_AssnDef_tagof_tm(s1);goto _TL3A6;_TL3A5: _TL3A6: goto _LL0;}}_LL0:;}
# 2925
Cyc_Hashtable_insert(table,told,t);_T3B=t;
return _T3B;}
# 2934
static void Cyc_AssnDef_changed_lhs(struct Cyc_Set_Set**vds,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;void*_T9;struct Cyc_Set_Set**_TA;struct Cyc_Set_Set*(*_TB)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_TC)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_TD;struct Cyc_Set_Set*_TE;struct Cyc_Absyn_Vardecl*_TF;_T0=e;{
void*_T10=_T0->r;struct Cyc_Absyn_Vardecl*_T11;_T1=(int*)_T10;_T2=*_T1;if(_T2!=1)goto _TL3A7;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T10;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T10;_T7=_T12->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T13=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T7;_T11=_T13->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T11;_T11=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T10;_T8=_T12->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T13=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T8;_T11=_T13->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T11;_T11=vd;goto _LL6;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T12=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T10;_T9=_T12->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T13=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T9;_T11=_T13->f1;}}_LL6: {struct Cyc_Absyn_Vardecl*vd=_T11;_TA=vds;_TC=Cyc_Set_insert;{
# 2939
struct Cyc_Set_Set*(*_T12)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_TC;_TB=_T12;}_TD=vds;_TE=*_TD;_TF=vd;*_TA=_TB(_TE,_TF);goto _LL0;}default: goto _LL7;}goto _TL3A8;_TL3A7: _LL7: goto _LL0;_TL3A8: _LL0:;}}struct _tuple22{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 2945
static void Cyc_AssnDef_changed_pat(struct Cyc_Set_Set**vds,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Set_Set*(*_T3)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T4)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T5;struct Cyc_Set_Set*_T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Set_Set*(*_T8)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T9)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_TA;struct Cyc_Set_Set*_TB;struct Cyc_Absyn_Vardecl*_TC;struct Cyc_Set_Set**_TD;struct Cyc_List_List*_TE;void*_TF;struct _tuple22*_T10;struct Cyc_Absyn_Pat*_T11;struct Cyc_List_List*_T12;struct Cyc_Set_Set**_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Pat*_T16;struct Cyc_List_List*_T17;
LOOP: _T0=p;{
void*_T18=_T0->r;struct Cyc_List_List*_T19;struct Cyc_Absyn_Pat*_T1A;struct Cyc_Absyn_Vardecl*_T1B;_T1=(int*)_T18;_T2=*_T1;switch(_T2){case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T18;_T1B=_T1C->f1;_T1A=_T1C->f2;}{struct Cyc_Absyn_Vardecl*vd=_T1B;struct Cyc_Absyn_Pat*p1=_T1A;_T1B=vd;_T1A=p1;goto _LL4;}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T18;_T1B=_T1C->f1;_T1A=_T1C->f2;}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T1B;struct Cyc_Absyn_Pat*p1=_T1A;_T4=Cyc_Set_insert;{
# 2949
struct Cyc_Set_Set*(*_T1C)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T4;_T3=_T1C;}_T5=vds;_T6=*_T5;_T7=vd;_T3(_T6,_T7);p=p1;goto LOOP;}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T18;_T1B=_T1C->f2;}{struct Cyc_Absyn_Vardecl*vd=_T1B;_T1B=vd;goto _LL8;}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T18;_T1B=_T1C->f2;}_LL8: {struct Cyc_Absyn_Vardecl*vd=_T1B;_T9=Cyc_Set_insert;{
# 2951
struct Cyc_Set_Set*(*_T1C)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T9;_T8=_T1C;}_TA=vds;_TB=*_TA;_TC=vd;_T8(_TB,_TC);goto _LL0;}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T18;_T1A=_T1C->f1;}{struct Cyc_Absyn_Pat*p1=_T1A;
p=p1;goto LOOP;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T18;_T19=_T1C->f4;}{struct Cyc_List_List*dlps=_T19;
# 2954
_TL3AE: if(dlps!=0)goto _TL3AC;else{goto _TL3AD;}
_TL3AC: _TD=vds;_TE=dlps;_TF=_TE->hd;_T10=(struct _tuple22*)_TF;_T11=_T10->f1;Cyc_AssnDef_changed_pat(_TD,_T11);_T12=dlps;
# 2954
dlps=_T12->tl;goto _TL3AE;_TL3AD: goto _LL0;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T1C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T18;_T19=_T1C->f3;}{struct Cyc_List_List*ps=_T19;
# 2958
_TL3B2: if(ps!=0)goto _TL3B0;else{goto _TL3B1;}
_TL3B0: _T13=vds;_T14=ps;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Pat*)_T15;Cyc_AssnDef_changed_pat(_T13,_T16);_T17=ps;
# 2958
ps=_T17->tl;goto _TL3B2;_TL3B1: goto _LL0;}default: goto _LL0;}_LL0:;}}
# 2965
static int Cyc_AssnDef_changed_exp(struct Cyc_Set_Set**vds,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Set_Set**_T3;struct Cyc_Set_Set*(*_T4)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T5)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T6;struct Cyc_Set_Set*_T7;struct Cyc_Absyn_Vardecl*_T8;_T0=e;{
void*_T9=_T0->r;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Exp*_TC;_T1=(int*)_T9;_T2=*_T1;switch(_T2){case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_TD=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T9;_TC=_TD->f1;}{struct Cyc_Absyn_Exp*e=_TC;_TC=e;goto _LL4;}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_TD=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T9;_TC=_TD->f1;}_LL4: {struct Cyc_Absyn_Exp*e=_TC;
# 2969
Cyc_AssnDef_changed_lhs(vds,e);goto _LL0;}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T9;_TB=_TD->f1;}{struct Cyc_Absyn_Vardecl*vd=_TB;_T3=vds;_T5=Cyc_Set_insert;{
# 2972
struct Cyc_Set_Set*(*_TD)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T5;_T4=_TD;}_T6=vds;_T7=*_T6;_T8=vd;*_T3=_T4(_T7,_T8);goto _LL0;}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_TD=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T9;_TC=_TD->f1;_TA=_TD->f2;}{struct Cyc_Absyn_Exp*e1=_TC;struct Cyc_Absyn_Exp*e2=_TA;
# 2975
Cyc_AssnDef_changed_lhs(vds,e1);
Cyc_AssnDef_changed_lhs(vds,e2);goto _LL0;}default: goto _LL0;}_LL0:;}
# 2980
return 1;}
# 2983
static int Cyc_AssnDef_changed_stmt(struct Cyc_Set_Set**vds,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_Set_Set**_T6;struct Cyc_Set_Set*(*_T7)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*);struct Cyc_Set_Set*(*_T8)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set**_T9;struct Cyc_Set_Set*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Set_Set**_TC;struct Cyc_Set_Set**_TD;struct Cyc_Set_Set*_TE;struct Cyc_Set_Set*(*_TF)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*);struct Cyc_Set_Set*(*_T10)(int(*)(void*,void*),struct Cyc_List_List*);int(*_T11)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*);int(*_T12)(void*,void*);struct Cyc_List_List*_T13;struct Cyc_Set_Set*_T14;struct Cyc_Set_Set**_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Switch_clause*_T18;struct Cyc_Absyn_Pat*_T19;struct Cyc_List_List*_T1A;_T0=s;{
void*_T1B=_T0->r;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Decl*_T1D;_T1=(int*)_T1B;_T2=*_T1;switch(_T2){case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T1E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T1B;_T1D=_T1E->f1;}{struct Cyc_Absyn_Decl*d=_T1D;_T3=d;{
# 2986
void*_T1E=_T3->r;struct Cyc_List_List*_T1F;struct Cyc_Absyn_Pat*_T20;struct Cyc_Absyn_Vardecl*_T21;_T4=(int*)_T1E;_T5=*_T4;switch(_T5){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T1E;_T21=_T22->f1;}{struct Cyc_Absyn_Vardecl*vd=_T21;_T21=vd;goto _LLD;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T1E;_T21=_T22->f2;}_LLD: {struct Cyc_Absyn_Vardecl*vd=_T21;_T6=vds;_T8=Cyc_Set_insert;{
# 2988
struct Cyc_Set_Set*(*_T22)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Absyn_Vardecl*))_T8;_T7=_T22;}_T9=vds;_TA=*_T9;_TB=vd;*_T6=_T7(_TA,_TB);goto _LL9;}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T1E;_T20=_T22->f1;}{struct Cyc_Absyn_Pat*p=_T20;
# 2990
Cyc_AssnDef_changed_pat(vds,p);goto _LL9;}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T22=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T1E;_T1F=_T22->f1;}{struct Cyc_List_List*vds2=_T1F;_TC=vds;_TD=vds;_TE=*_TD;_T10=Cyc_Set_from_list;{
# 2992
struct Cyc_Set_Set*(*_T22)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*)=(struct Cyc_Set_Set*(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))_T10;_TF=_T22;}_T12=Cyc_Core_ptrcmp;{int(*_T22)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))_T12;_T11=_T22;}_T13=vds2;_T14=_TF(_T11,_T13);*_TC=Cyc_Set_union_two(_TE,_T14);goto _LL9;}default: goto _LL9;}_LL9:;}goto _LL0;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T1E=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T1B;_T1C=_T1E->f2;}{struct Cyc_List_List*scs=_T1C;_T1C=scs;goto _LL6;}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T1E=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T1B;_T1C=_T1E->f2;}_LL6: {struct Cyc_List_List*scs=_T1C;
# 2998
_TL3B9: if(scs!=0)goto _TL3B7;else{goto _TL3B8;}
_TL3B7: _T15=vds;_T16=scs;_T17=_T16->hd;_T18=(struct Cyc_Absyn_Switch_clause*)_T17;_T19=_T18->pattern;Cyc_AssnDef_changed_pat(_T15,_T19);_T1A=scs;
# 2998
scs=_T1A->tl;goto _TL3B9;_TL3B8: goto _LL0;}default: goto _LL0;}_LL0:;}
# 3003
return 1;}
# 3006
struct Cyc_Set_Set*Cyc_AssnDef_calc_changed_vars_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Set_Set*(*_T0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*));struct Cyc_Set_Set*(*_T1)(int(*)(void*,void*));int(*_T2)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*);int(*_T3)(void*,void*);void(*_T4)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*);void(*_T5)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_Set_Set**_T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Set_Set*_T8;_T1=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T9)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Set_Set*(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))_T1;_T0=_T9;}_T3=Cyc_Core_ptrcmp;{int(*_T9)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))_T3;_T2=_T9;}{struct Cyc_Set_Set*vds=_T0(_T2);_T5=Cyc_Absyn_visit_exp;{
void(*_T9)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*))_T5;_T4=_T9;}_T6=& vds;_T7=e;_T4(Cyc_AssnDef_changed_exp,Cyc_AssnDef_changed_stmt,_T6,_T7);_T8=vds;
return _T8;}}
# 3011
struct Cyc_Set_Set*Cyc_AssnDef_calc_changed_vars_stmt(struct Cyc_Absyn_Stmt*s){struct Cyc_Set_Set*(*_T0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*));struct Cyc_Set_Set*(*_T1)(int(*)(void*,void*));int(*_T2)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*);int(*_T3)(void*,void*);void(*_T4)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*);void(*_T5)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set**_T6;struct Cyc_Absyn_Stmt*_T7;struct Cyc_Set_Set*_T8;_T1=Cyc_Set_empty;{
struct Cyc_Set_Set*(*_T9)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Set_Set*(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))_T1;_T0=_T9;}_T3=Cyc_Core_ptrcmp;{int(*_T9)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))_T3;_T2=_T9;}{struct Cyc_Set_Set*vds=_T0(_T2);_T5=Cyc_Absyn_visit_stmt;{
void(*_T9)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*),struct Cyc_Set_Set**,struct Cyc_Absyn_Stmt*))_T5;_T4=_T9;}_T6=& vds;_T7=s;_T4(Cyc_AssnDef_changed_exp,Cyc_AssnDef_changed_stmt,_T6,_T7);_T8=vds;
return _T8;}}
# 3020
static struct Cyc_Set_Set*Cyc_AssnDef_flatten_it(void*a){void*_T0;int*_T1;unsigned _T2;struct Cyc_Set_Set*_T3;void*_T4;void*_T5;void*_T6;struct Cyc_Set_Set*_T7;void*_T8;struct Cyc_Set_Set*_T9;void*_TA;void*_TB;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=
# 3022
Cyc_Set_empty(Cyc_AssnDef_assncmp);return _T3;case 1:
 return 0;case 3: _T4=a;{struct Cyc_AssnDef_And_AssnDef_Assn_struct*_TC=(struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_T4;_T5=_TC->f1;_TB=(void*)_T5;_T6=_TC->f2;_TA=(void*)_T6;}{void*a1=_TB;void*a2=_TA;
# 3025
struct Cyc_Set_Set*s1=Cyc_AssnDef_flatten_it(a1);
if(s1!=0)goto _TL3BB;return 0;_TL3BB: {
struct Cyc_Set_Set*s2=Cyc_AssnDef_flatten_it(a2);
if(s2!=0)goto _TL3BD;return 0;_TL3BD: _T7=
Cyc_Set_union_two(s1,s2);return _T7;}}default: _T8=a;_T9=
# 3031
Cyc_Set_singleton(Cyc_AssnDef_assncmp,_T8);return _T9;};}
# 3039
int Cyc_AssnDef_better_prove(void*a1,void*a2){int _T0;struct Cyc_Set_Set*_T1;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3;void*_T4;int _T5;
struct Cyc_Set_Set*w1=Cyc_AssnDef_widen_it(a1);
if(w1!=0)goto _TL3BF;return 1;_TL3BF: {
struct Cyc_Set_Set*w2=Cyc_AssnDef_flatten_it(a2);
if(w2==0)goto _TL3C1;{
struct Cyc_Set_Set*diff=Cyc_PrattProver_filter_implies(w1,w2);
if(diff==0)goto _TL3C5;else{goto _TL3C6;}_TL3C6: _T0=Cyc_Set_is_empty(diff);if(_T0)goto _TL3C5;else{goto _TL3C3;}_TL3C5: return 1;_TL3C3: _T1=diff;_T2=& Cyc_AssnDef_true_assn;_T3=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T2;_T4=(void*)_T3;
a2=Cyc_Set_fold(Cyc_AssnDef_and,_T1,_T4);}goto _TL3C2;_TL3C1: _TL3C2: _T5=
# 3048
Cyc_PrattProver_constraint_prove(a1,a2);return _T5;}}
