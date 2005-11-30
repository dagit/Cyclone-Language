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
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 522 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 529
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 547
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 561
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 915 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 933
void*Cyc_Absyn_compress(void*);
# 968
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 29 "currgn.h"
extern struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 40 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 132 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*,void*);
# 218
int Cyc_Tcutil_new_tvar_id (void);
# 32 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 77 "kinds.h"
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 33 "currgn.cyc"
static struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_tvar=0;
static void*Cyc_CurRgn_curr_rgn_typ=0;static char _TmpG0[3U]="`C";
# 36
struct _fat_ptr Cyc_CurRgn_curr_rgn_name={_TmpG0,_TmpG0,_TmpG0 + 3U};
# 38
struct Cyc_Absyn_Tvar*Cyc_CurRgn_curr_rgn_tvar (void){
if(Cyc_CurRgn_curr_tvar==0){
Cyc_CurRgn_curr_tvar=({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=Cyc_CurRgn_curr_rgn_name;_Tmp2;});_Tmp0->name=_Tmp1;}),({
int _Tmp1=Cyc_Tcutil_new_tvar_id();_Tmp0->identity=_Tmp1;}),({
void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);_Tmp0->kind=_Tmp1;}),_Tmp0->aquals_bound=0;_Tmp0;});
# 44
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type(Cyc_CurRgn_curr_tvar);}
# 46
return _check_null(Cyc_CurRgn_curr_tvar);}
# 49
void*Cyc_CurRgn_curr_rgn_type (void){
Cyc_CurRgn_curr_rgn_tvar();
Cyc_CurRgn_curr_rgn_typ=Cyc_Absyn_var_type(_check_null(Cyc_CurRgn_curr_tvar));
return Cyc_CurRgn_curr_rgn_typ;}struct _tuple11{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 55
void Cyc_CurRgn_subst_inst(struct Cyc_List_List*inst,struct Cyc_Absyn_Tvar*tv,void*t){
for(1;inst!=0;inst->tl){
struct _tuple11*_Tmp0=(struct _tuple11*)inst->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(struct Cyc_Absyn_Tvar**)& _Tmp0->f0;_Tmp1=(void**)& _Tmp0->f1;{struct Cyc_Absyn_Tvar**tvi=(struct Cyc_Absyn_Tvar**)_Tmp2;void**ti=(void**)_Tmp1;
if(Cyc_Absyn_tvar_cmp(*tvi,tv)==0){
*ti=t;
return;}}}}
# 68
static struct _tuple8*Cyc_CurRgn_argtype(struct _tuple8*arg){
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;_Tmp2=arg->f0;_Tmp1=arg->f1;_Tmp0=arg->f2;{struct _fat_ptr*vopt=_Tmp2;struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
{void*_Tmp3;if(*((int*)t)==4){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*t1=_Tmp3;
# 72
if(!Cyc_Tcutil_is_function_type(t1))return arg;
goto _LL3;}}else{
return arg;}_LL3:;}{
# 76
void*t2=Cyc_Tcutil_copy_type(t);
void*_Tmp3=Cyc_Absyn_compress(t2);void*_Tmp4;if(*((int*)_Tmp3)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;{void*t1=_Tmp4;
# 79
void*_Tmp5=Cyc_Absyn_compress(t1);void*_Tmp6;if(*((int*)_Tmp5)==6){_Tmp6=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1.tvars;{struct Cyc_List_List**tvars=(struct Cyc_List_List**)_Tmp6;
# 83
struct Cyc_List_List*tvars2=*tvars;
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=_check_null(Cyc_CurRgn_curr_tvar),_Tmp8->tl=tvars2;_Tmp8;});*tvars=_Tmp7;});{
struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f0=vopt,_Tmp7->f1=tq,_Tmp7->f2=t2;return _Tmp7;}}}else{
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("internal compiler error: not a function type",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));};}}else{
# 88
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("internal compiler error: not a pointer type",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));};}}}
# 93
void*Cyc_CurRgn_instantiate(void*fntype,void*currgn){
void*_Tmp0;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)fntype)==6){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype)->f1.tvars;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype)->f1.effect;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype)->f1.ret_tqual;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype)->f1.ret_type;_Tmp0=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype)->f1.args;{struct Cyc_List_List*tvars=_Tmp4;void*eff=_Tmp3;struct Cyc_Absyn_Tqual res_tq=_Tmp2;void*res_typ=_Tmp1;struct Cyc_List_List**args_info=(struct Cyc_List_List**)_Tmp0;
# 99
struct Cyc_List_List*old_args_info=*args_info;
# 101
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*(*_Tmp6)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;_Tmp6;})(Cyc_CurRgn_argtype,*args_info);*args_info=_Tmp5;});{
# 103
struct Cyc_List_List*inst;inst=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple11*_Tmp5=({struct _tuple11*_Tmp6=_cycalloc(sizeof(struct _tuple11));_Tmp6->f0=_check_null(Cyc_CurRgn_curr_tvar),_Tmp6->f1=currgn;_Tmp6;});inst->hd=_Tmp5;}),inst->tl=0;{
void*fntype2=Cyc_Tcutil_substitute(inst,fntype);
# 106
{void*_Tmp5;void*_Tmp6;if(*((int*)fntype2)==6){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype2)->f1.tvars;_Tmp5=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fntype2)->f1.args;{struct Cyc_List_List*tvars=_Tmp6;struct Cyc_List_List**args_info2=(struct Cyc_List_List**)_Tmp5;
# 108
*args_info2=old_args_info;{
struct Cyc_List_List*tvars2=tvars;
goto _LL5;}}}else{
goto _LL5;}_LL5:;}
# 113
*args_info=old_args_info;
return fntype2;}}}}else{
return fntype;};}
# 120
int Cyc_CurRgn_is_not_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,_check_null(Cyc_CurRgn_curr_tvar))!=0;}
# 124
int Cyc_CurRgn_is_curr_rgn(struct Cyc_Absyn_Tvar*v){
return Cyc_Absyn_tvar_cmp(v,_check_null(Cyc_CurRgn_curr_tvar))==0;}struct Cyc_CurRgn_rgnsubenv{struct Cyc_List_List*inst;int in_externC;int toplevel;};
# 134
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Stmt*);
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Exp*);
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
static void Cyc_CurRgn_rgnsubdtdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Datatypedecl*);
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_Absyn_Aggrdecl*);
# 142
static void Cyc_CurRgn_check_tvar(unsigned loc,struct Cyc_Absyn_Tvar*tv){
if(Cyc_Absyn_tvar_cmp(tv,Cyc_CurRgn_curr_tvar)==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Cannot abstract special type variable ",sizeof(char),39U);_Tmp1;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=Cyc_CurRgn_curr_tvar;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 146
static void Cyc_CurRgn_check_tvars(unsigned loc,struct Cyc_List_List*tvs){
for(1;tvs!=0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}}
# 151
static void Cyc_CurRgn_rgnsubtypes(unsigned,struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
# 154
static void Cyc_CurRgn_rgnsubtype(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,void*type){
void*_Tmp0=Cyc_Absyn_compress(type);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 2: _Tmp5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*v=_Tmp5;
# 157
if(env.toplevel)Cyc_CurRgn_check_tvar(loc,v);return;}case 8: _Tmp5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 5: _Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.num_elts;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*t1=_Tmp5;struct Cyc_Absyn_Exp*e=_Tmp4;void*zt=_Tmp3;
# 160
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubexpopt(env,e);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
return;}case 4: _Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.eff;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.autoreleased;{void*t1=_Tmp5;void*r=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 165
Cyc_CurRgn_rgnsubtype(loc,env,t1);
Cyc_CurRgn_rgnsubtype(loc,env,r);
Cyc_CurRgn_rgnsubtype(loc,env,b);
Cyc_CurRgn_rgnsubtype(loc,env,zt);
Cyc_CurRgn_rgnsubtype(loc,env,rel);
return;}case 6:
# 172
 return;case 7: _Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_List_List*fs=_Tmp5;
# 174
for(1;fs!=0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}
# 178
return;}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f2;{void*r=_Tmp5;
_Tmp5=r;goto _LL10;}case 1: _Tmp5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_LL10: {void*r=_Tmp5;
# 181
if(r!=0)Cyc_CurRgn_rgnsubtype(loc,env,r);
return;}case 0: _Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
Cyc_CurRgn_rgnsubtypes(loc,env,ts);return;}case 9: _Tmp5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);return;}case 11: _Tmp5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);return;}default: _Tmp5=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp5;void**topt=_Tmp4;
# 187
topt==0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("topt == NULL",sizeof(char),13U),_tag_fat("currgn.cyc",sizeof(char),11U),187U);
{void*_Tmp6=td->r;void*_Tmp7;switch(*((int*)_Tmp6)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
Cyc_CurRgn_rgnsubaggrdecl(loc,env,ad);return;}case 2: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Datatypedecl*dtd=_Tmp7;
Cyc_CurRgn_rgnsubdtdecl(loc,env,dtd);return;}default:
 return;};}
# 193
return;}};}
# 197
static void Cyc_CurRgn_rgnsubtypes(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ts){
# 199
for(1;ts!=0;ts=ts->tl){Cyc_CurRgn_rgnsubtype(loc,env,(void*)ts->hd);}}
# 204
static void Cyc_CurRgn_rgnsubexp(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 31: _LL4:
 goto _LL6;case 32: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 39: _LLA:
 goto _LLC;case 1: _LLC:
 goto _LL0;case 19: _Tmp5=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void**t=_Tmp5;struct Cyc_List_List*f=_Tmp4;
# 213
_Tmp5=t;goto _LL10;}case 17: _Tmp5=(void**)&((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {void**t=_Tmp5;
# 215
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp6;});
goto _LL0;}case 6: _Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 221
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);Cyc_CurRgn_rgnsubexp(env,e3);goto _LL0;}case 26: _Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;int res=_Tmp2;
# 223
_Tmp5=e1;_Tmp4=e2;goto _LL16;}case 4: _Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL18;}case 34: _Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1A;}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1C;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL1E;}case 23: _Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL20;}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
Cyc_CurRgn_rgnsubexp(env,e1);Cyc_CurRgn_rgnsubexp(env,e2);goto _LL0;}case 30: _Tmp5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp5;struct Cyc_Absyn_Datatypedecl*dtd=_Tmp4;struct Cyc_Absyn_Datatypefield*dtfd=_Tmp3;
# 231
_Tmp5=es;goto _LL24;}case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL24: {struct Cyc_List_List*es=_Tmp5;
# 234
for(1;es!=0;es=es->tl){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 27: _Tmp5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(void**)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;void**t=_Tmp4;
_Tmp5=t;_Tmp4=e1;goto _LL28;}case 14: _Tmp5=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL28: {void**t=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;
# 239
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp6;});
_Tmp5=e1;goto _LL2A;}case 21: _Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 244
_Tmp5=e1;goto _LL2C;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL2E;}case 37: _Tmp5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL30;}case 5: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL32;}case 41: _Tmp5=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL34;}case 42: _Tmp5=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL36;}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL38;}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL38: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL3A;}case 18: _Tmp5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3A: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL3C;}case 40: _Tmp5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3C: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL3E;}case 11: _Tmp5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3E: {struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL40;}case 12: _Tmp5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL40: {struct Cyc_Absyn_Exp*e1=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 38: _Tmp5=(void**)&((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**t=_Tmp5;
# 257
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel){
void*new_typ=Cyc_Tcutil_substitute_nofun(env.inst,*t);
if(new_typ==*t)goto _LL0;{
# 262
void*_Tmp6=Cyc_Absyn_compress(new_typ);void*_Tmp7;if(*((int*)_Tmp6)==9){_Tmp7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp6)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp7;
# 264
e->r=e1->r;
e->topt=e1->topt;
e->annot=e1->annot;
goto _LL57;}}else{
*t=new_typ;goto _LL57;}_LL57:;}}
# 271
goto _LL0;}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_List_List*es=_Tmp4;struct Cyc_Absyn_VarargCallInfo*vararg=_Tmp3;int resv=_Tmp2;
# 273
while(es!=0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;
# 274
1U;}
# 277
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 13: _Tmp5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 280
while(ts!=0){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_Tmp6=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_Tmp6;});
ts=ts->tl;
# 281
1U;}
# 286
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}case 16: _Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eopt=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*qopt=_Tmp3;
# 290
Cyc_CurRgn_rgnsubexpopt(env,eopt);Cyc_CurRgn_rgnsubexpopt(env,qopt);Cyc_CurRgn_rgnsubexp(env,e1);goto _LL0;}case 24: _Tmp5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple8*i=_Tmp5;struct Cyc_List_List*ds=_Tmp4;
# 293
Cyc_CurRgn_rgnsubtype(e->loc,env,(*i).f2);
if(!env.toplevel)
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,(*i).f2);(*i).f2=_Tmp6;});
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 35: _Tmp5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds=_Tmp5;
# 299
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 25: _Tmp5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp5;
Cyc_CurRgn_rgnsubds(env,ds);goto _LL0;}case 28: _Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*ts=_Tmp5;struct Cyc_List_List*ds=_Tmp4;
# 303
for(1;ts!=0;ts=ts->tl){
Cyc_CurRgn_rgnsubtype(e->loc,env,(void*)ts->hd);
if(!env.toplevel)
({void*_Tmp6=(void*)Cyc_Tcutil_substitute_nofun(env.inst,(void*)ts->hd);ts->hd=_Tmp6;});}
# 308
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 29: _Tmp5=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{void**t=_Tmp5;struct Cyc_List_List*ds=_Tmp4;
# 312
if(*t!=0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*t);
if(!env.toplevel)
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp6;});}
# 317
Cyc_CurRgn_rgnsubds(env,ds);
goto _LL0;}case 33: _Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*rgnopt=_Tmp5;struct Cyc_Absyn_Exp*aqopt=_Tmp4;void**elt_t=_Tmp3;struct Cyc_Absyn_Exp*e1=_Tmp1;
# 321
!env.toplevel?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("!env.toplevel",sizeof(char),14U),_tag_fat("currgn.cyc",sizeof(char),11U),321U);
Cyc_CurRgn_rgnsubexpopt(env,rgnopt);
Cyc_CurRgn_rgnsubexpopt(env,aqopt);
if(elt_t!=0){
Cyc_CurRgn_rgnsubtype(e->loc,env,*elt_t);
({void*_Tmp6=Cyc_Tcutil_substitute_nofun(env.inst,*elt_t);*elt_t=_Tmp6;});}
# 328
Cyc_CurRgn_rgnsubexp(env,e1);
goto _LL0;}default: _Tmp5=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp5;
# 331
Cyc_CurRgn_rgnsubstmt(env,s);goto _LL0;}}_LL0:;}
# 333
if(e->topt!=0){
Cyc_CurRgn_rgnsubtype(e->loc,env,e->topt);
if(!env.toplevel)
({void*_Tmp0=Cyc_Tcutil_substitute_nofun(env.inst,e->topt);e->topt=_Tmp0;});}}
# 340
static void Cyc_CurRgn_rgnsubexpopt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Exp*e){
if(e==0)return;else{
Cyc_CurRgn_rgnsubexp(env,e);}}struct _tuple12{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 345
static void Cyc_CurRgn_rgnsubds(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
# 347
for(1;ds!=0;ds=ds->tl){
Cyc_CurRgn_rgnsubexp(env,(*((struct _tuple12*)ds->hd)).f1);}}struct _tuple13{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 351
static void Cyc_CurRgn_rgnsubpat(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Pat*p){
!env.toplevel?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("!env.toplevel",sizeof(char),14U),_tag_fat("currgn.cyc",sizeof(char),11U),352U);{
void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0:
 return;case 1: _Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Pat*p1=_Tmp1;
# 356
_Tmp2=vd;_Tmp1=p1;goto _LL6;}case 3: _Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;struct Cyc_Absyn_Pat*p1=_Tmp1;
# 359
if(vd->type!=0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 363
Cyc_CurRgn_rgnsubpat(env,p1);
return;}case 2: _Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp2;struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 367
Cyc_CurRgn_check_tvar(p->loc,tv);
if(vd->type!=0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 372
return;}case 4: _Tmp2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp2;struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 376
if(vd->type!=0){
Cyc_CurRgn_rgnsubtype(p->loc,env,vd->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,vd->type);vd->type=_Tmp3;});}
# 380
Cyc_CurRgn_check_tvar(p->loc,tv);
return;}case 5: _Tmp2=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p1=_Tmp2;
# 383
Cyc_CurRgn_rgnsubpat(env,p1);return;}case 6: _Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*ds=_Tmp1;
# 386
for(1;tvs!=0;tvs=tvs->tl){
Cyc_CurRgn_check_tvar(p->loc,(struct Cyc_Absyn_Tvar*)tvs->hd);}
for(1;ds!=0;ds=ds->tl){
Cyc_CurRgn_rgnsubpat(env,(*((struct _tuple13*)ds->hd)).f1);}
return;}case 7: _Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp2;
# 393
_Tmp2=ps;goto _LL12;}case 15: _Tmp2=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL12: {struct Cyc_List_List*ps=_Tmp2;
# 395
for(1;ps!=0;ps=ps->tl){
Cyc_CurRgn_rgnsubpat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 8:
# 399
 goto _LL16;case 9: _LL16:
 goto _LL18;case 10: _LL18:
 goto _LL1A;case 11: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL1E;case 13: _LL1E:
 goto _LL20;case 14: _LL20:
 return;default: _Tmp2=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 407
Cyc_CurRgn_rgnsubexp(env,e);return;}};}}
# 411
static void Cyc_CurRgn_rgnsubaggrdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Aggrdecl*ad){
Cyc_CurRgn_check_tvars(loc,ad->tvs);
if(ad->impl!=0){
Cyc_CurRgn_check_tvars(loc,ad->impl->exist_vars);{
struct Cyc_List_List*fc=ad->impl->effconstr;
for(1;fc!=0;fc=fc->tl){
void*_Tmp0=(void*)fc->hd;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f1;{void*e=_Tmp2;
Cyc_CurRgn_rgnsubtype(loc,env,e);goto _LL0;}case 1: _Tmp2=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f2;{void*e1=_Tmp2;void*e2=_Tmp1;
_Tmp2=e1;_Tmp1=e2;goto _LL6;}default: _Tmp2=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f2;_LL6: {void*e1=_Tmp2;void*e2=_Tmp1;
Cyc_CurRgn_rgnsubtype(loc,env,e1);Cyc_CurRgn_rgnsubtype(loc,env,e2);goto _LL0;}}_LL0:;}{
# 423
struct Cyc_List_List*fs=ad->impl->fields;
for(1;fs!=0;fs=fs->tl){
Cyc_CurRgn_rgnsubtype(loc,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
Cyc_CurRgn_rgnsubexpopt(env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->requires_clause);}}}}}struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;};
# 431
static void Cyc_CurRgn_rgnsubdtdecl(unsigned loc,struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Datatypedecl*tud){
Cyc_CurRgn_check_tvars(loc,tud->tvs);
if(tud->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)tud->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_List_List*ts=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;
while(ts!=0){
{struct _tuple14*_Tmp0=(struct _tuple14*)ts->hd;void*_Tmp1;_Tmp1=(void**)& _Tmp0->f1;{void**typ=(void**)_Tmp1;
Cyc_CurRgn_rgnsubtype(loc,env,*typ);
ts=ts->tl;}}
# 437
1U;}}}}
# 445
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv,struct Cyc_List_List*);
static void Cyc_CurRgn_rgnsubdecl(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 449
void*_Tmp3;void*_Tmp4;_Tmp4=(void**)& vd->type;_Tmp3=vd->initializer;{void**t=(void**)_Tmp4;struct Cyc_Absyn_Exp*initializer=_Tmp3;
Cyc_CurRgn_rgnsubexpopt(env,initializer);
Cyc_CurRgn_rgnsubtype(d->loc,env,*t);
if(!env.toplevel)
({void*_Tmp5=Cyc_Tcutil_substitute_nofun(env.inst,*t);*t=_Tmp5;});
return;}}case 1: _Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp2;
# 462
env.toplevel=0;
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple11*_Tmp5=({struct _tuple11*_Tmp6=_cycalloc(sizeof(struct _tuple11));_Tmp6->f0=Cyc_CurRgn_curr_tvar,_Tmp6->f1=Cyc_CurRgn_curr_rgn_typ;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});env.inst=_Tmp3;});
# 465
Cyc_CurRgn_rgnsubstmt(env,fd->body);
# 468
return;}case 8: _Tmp2=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp2;
# 471
env.toplevel?0:({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp3;})(_tag_fat("env.toplevel",sizeof(char),13U),_tag_fat("currgn.cyc",sizeof(char),11U),471U);
Cyc_CurRgn_check_tvars(d->loc,td->tvs);
if(td->defn!=0)Cyc_CurRgn_rgnsubtype(d->loc,env,td->defn);
return;}case 5: _Tmp2=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp2;
# 477
env.toplevel?0:({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp3;})(_tag_fat("env.toplevel",sizeof(char),13U),_tag_fat("currgn.cyc",sizeof(char),11U),477U);
Cyc_CurRgn_rgnsubaggrdecl(d->loc,env,ad);
return;}case 6: _Tmp2=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp2;
# 482
env.toplevel?0:({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp3;})(_tag_fat("env.toplevel",sizeof(char),13U),_tag_fat("currgn.cyc",sizeof(char),11U),482U);
Cyc_CurRgn_rgnsubdtdecl(d->loc,env,tud);
return;}case 11:
# 488
 goto _LLE;case 12: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 13: _LL12:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 15: _LL16:
 goto _LL18;case 16: _LL18:
 return;case 9: _Tmp2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp2;struct Cyc_List_List*ds2=_Tmp1;
# 496
_Tmp2=ds2;goto _LL1C;}case 10: _Tmp2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_List_List*ds2=_Tmp2;
Cyc_CurRgn_rgnsubdecls(env,ds2);return;}case 2: _Tmp2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
# 500
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexp(env,e);
return;}case 3: _Tmp2=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp2;
# 505
!env.toplevel?0:({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp3;})(_tag_fat("!env.toplevel",sizeof(char),14U),_tag_fat("currgn.cyc",sizeof(char),11U),505U);
for(1;vds!=0;vds=vds->tl){
Cyc_CurRgn_rgnsubtype(d->loc,env,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);
({void*_Tmp3=Cyc_Tcutil_substitute_nofun(env.inst,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);((struct Cyc_Absyn_Vardecl*)vds->hd)->type=_Tmp3;});}
# 510
return;}default:
# 512
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("internal compiler error: region or pool at toplevel",sizeof(char),52U),_tag_fat(0U,sizeof(void*),0));};}
# 516
static void Cyc_CurRgn_rgnsubstmt(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 6: _LL4:
 goto _LL6;case 7: _LL6:
 goto _LL8;case 8: _LL8:
 return;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 523
Cyc_CurRgn_rgnsubexp(env,e);return;}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 3: _Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp6;
Cyc_CurRgn_rgnsubexpopt(env,eopt);return;}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 528
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);Cyc_CurRgn_rgnsubstmt(env,s2);return;}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;
# 530
_Tmp6=e;_Tmp5=s;goto _LL14;}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LL14: {struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
Cyc_CurRgn_rgnsubexp(env,e);Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*guard_s=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*cont_s=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 534
Cyc_CurRgn_rgnsubexp(env,e1);
Cyc_CurRgn_rgnsubexp(env,e2);
Cyc_CurRgn_rgnsubexp(env,e3);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;}case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*l=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 540
Cyc_CurRgn_rgnsubstmt(env,s1);return;}case 11: _Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp6;
# 543
while(es!=0){
Cyc_CurRgn_rgnsubexp(env,(struct Cyc_Absyn_Exp*)es->hd);
es=es->tl;
# 544
1U;}
# 547
return;}case 10: _Tmp6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 550
Cyc_CurRgn_rgnsubexp(env,e);
while(scs!=0){
{struct Cyc_Absyn_Switch_clause*_Tmp7=(struct Cyc_Absyn_Switch_clause*)scs->hd;void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->pattern;_Tmp9=_Tmp7->where_clause;_Tmp8=_Tmp7->body;{struct Cyc_Absyn_Pat*p=_TmpA;struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Stmt*s1=_Tmp8;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 552
1U;}
# 558
return;}case 15: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 561
while(scs!=0){
{struct Cyc_Absyn_Switch_clause*_Tmp7=(struct Cyc_Absyn_Switch_clause*)scs->hd;void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->pattern;_Tmp9=_Tmp7->where_clause;_Tmp8=_Tmp7->body;{struct Cyc_Absyn_Pat*p=_TmpA;struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Stmt*s1=_Tmp8;
Cyc_CurRgn_rgnsubpat(env,p);
Cyc_CurRgn_rgnsubexpopt(env,eopt);
Cyc_CurRgn_rgnsubstmt(env,s1);
scs=scs->tl;}}
# 562
1U;}
# 568
return;}default: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 571
void*_Tmp7=d->r;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp7)==4){_TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f1;_Tmp9=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f2;_Tmp8=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp7)->f3;{struct Cyc_Absyn_Tvar*tv=_TmpA;struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp8;
# 573
Cyc_CurRgn_check_tvar(d->loc,tv);
Cyc_CurRgn_rgnsubexpopt(env,open_exp_opt);
# 578
if(open_exp_opt==0){
({struct Cyc_List_List*_TmpB=env.inst;struct Cyc_Absyn_Tvar*_TmpC=Cyc_CurRgn_curr_tvar;Cyc_CurRgn_subst_inst(_TmpB,_TmpC,Cyc_Absyn_var_type(tv));});
Cyc_CurRgn_rgnsubstmt(env,s1);}else{
# 583
Cyc_CurRgn_rgnsubstmt(env,s1);}
return;}}else{
# 587
Cyc_CurRgn_rgnsubdecl(env,d);
Cyc_CurRgn_rgnsubstmt(env,s1);
return;};}};}
# 594
static void Cyc_CurRgn_rgnsubdecls(struct Cyc_CurRgn_rgnsubenv env,struct Cyc_List_List*ds){
for(1;ds!=0;ds=ds->tl){
Cyc_CurRgn_rgnsubdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}}
# 599
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds){
Cyc_CurRgn_curr_rgn_type();{
struct Cyc_CurRgn_rgnsubenv env=({struct Cyc_CurRgn_rgnsubenv _Tmp0;_Tmp0.inst=0,_Tmp0.in_externC=0,_Tmp0.toplevel=1;_Tmp0;});
Cyc_CurRgn_rgnsubdecls(env,tds);}}
