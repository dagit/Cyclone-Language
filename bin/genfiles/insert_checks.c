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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 134 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 157
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
# 453 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 460
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 478
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 492
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 854 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 914
void*Cyc_Absyn_bounds_one (void);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*,union Cyc_Relations_RelnOp,enum Cyc_Relations_Relation,union Cyc_Relations_RelnOp,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 51 "tcutil.h"
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 221
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 42 "evexp.h"
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 46
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);
# 58 "flags.h"
extern int Cyc_Flags_warn_bounds_checks;
# 62
extern int Cyc_Flags_warn_all_null_deref;struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
extern struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val;
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
extern struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val;
# 95 "assndef.h"
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 132 "assndef.h"
void*Cyc_AssnDef_reduce(void*);
# 32 "vcgen.h"
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void);
extern void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table**);struct _tuple13{void*f1;void*f2;};
# 36
struct _tuple13 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**,struct Cyc_Absyn_Exp*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);char Cyc_InsertChecks_FatBound[9U]="FatBound";char Cyc_InsertChecks_NoCheck[8U]="NoCheck";char Cyc_InsertChecks_NullAndFatBound[16U]="NullAndFatBound";char Cyc_InsertChecks_NullAndThinBound[17U]="NullAndThinBound";char Cyc_InsertChecks_NullOnly[9U]="NullOnly";char Cyc_InsertChecks_ThinBound[10U]="ThinBound";
# 63 "insert_checks.cyc"
struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val={Cyc_InsertChecks_NoCheck};
struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val={Cyc_InsertChecks_NullOnly};
struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val={Cyc_InsertChecks_NullAndFatBound};
struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val={Cyc_InsertChecks_FatBound};
# 78 "insert_checks.cyc"
static struct Cyc_Absyn_Exp*Cyc_InsertChecks_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return _check_null(ans);}struct Cyc_InsertChecks_Env{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table**assn_info;int in_sizeof: 1;int at_toplevel: 1;int vcgen_r: 1;};
# 94
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_toplevel_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,int do_vcgen){
return({struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=tables,_Tmp0.assn_info=0,_Tmp0.in_sizeof=0,_Tmp0.at_toplevel=1,_Tmp0.vcgen_r=do_vcgen;_Tmp0;});}
# 98
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_fn_body_env(struct Cyc_InsertChecks_Env nv){
return({struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=nv.tables,_Tmp0.assn_info=nv.assn_info,_Tmp0.in_sizeof=0,_Tmp0.at_toplevel=0,_Tmp0.vcgen_r=nv.vcgen_r;_Tmp0;});}
# 102
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_enter_sizeof(struct Cyc_InsertChecks_Env nv){
return({struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=nv.tables,_Tmp0.assn_info=nv.assn_info,_Tmp0.in_sizeof=1,_Tmp0.at_toplevel=nv.at_toplevel,_Tmp0.vcgen_r=nv.vcgen_r;_Tmp0;});}
# 108
static int Cyc_InsertChecks_need_null_check(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e){
if(nv.in_sizeof)
return 0;
# 112
{void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*n=_Tmp1;
# 114
if(!Cyc_Tcutil_force_type2bool(0,n))
return 0;
goto _LL0;}}else{
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_Tmp2="need_null_check: non-pointer type";_tag_fat(_Tmp2,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 119
{void*_Tmp0=e->annot;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_UnknownZ)
goto _LL5;else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_NotZero)
return 0;else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_IsZero){
# 123
Cyc_Warn_err(e->loc,({const char*_Tmp1="NULL pointer check will definitely fail";_tag_fat(_Tmp1,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_Absyn_EmptyAnnot)
# 126
return 0;else{
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_Tmp1="need_null_check: unexpected annotation";_tag_fat(_Tmp1,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));}}}}_LL5:;}
# 129
if(nv.at_toplevel){
Cyc_Warn_err(e->loc,({const char*_Tmp0="cannot perform NULL-pointer check at toplevel";_tag_fat(_Tmp0,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));
return 0;}
# 133
if(Cyc_Flags_warn_all_null_deref)
Cyc_Warn_warn(e->loc,({const char*_Tmp0="inserted null check";_tag_fat(_Tmp0,sizeof(char),20U);}),_tag_fat(0U,sizeof(void*),0));
return 1;}
# 138
static struct Cyc_List_List*Cyc_InsertChecks_get_relns(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->annot;void*_Tmp1;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_UnknownZ){_Tmp1=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*r=_Tmp1;
return r;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_NotZero){_Tmp1=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*r=_Tmp1;
return r;}}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_IsZero)
return 0;else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_Absyn_EmptyAnnot)
return 0;else{
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_Tmp2="get_relns: unexpected annotation";_tag_fat(_Tmp2,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));}}}};}
# 149
static int Cyc_InsertChecks_simple_need_bounds_check(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
if(nv.in_sizeof)return 0;
# 154
if(nv.at_toplevel)
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(a->loc,({const char*_Tmp0="InsertChecks: memory read at top-level";_tag_fat(_Tmp0,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));{
# 161
struct Cyc_Absyn_Exp*bound_opt=Cyc_Tcutil_get_type_bound(_check_null(a->topt));
# 163
if(bound_opt==0)return 1;{
struct Cyc_Absyn_Exp*bound=bound_opt;
# 166
if(!Cyc_Tcutil_is_const_exp(bound)|| !Cyc_Tcutil_is_const_exp(i))return 1;{
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_uint_exp(bound);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned b=_Tmp2;int b_known=_Tmp1;
struct _tuple12 _Tmp3=Cyc_Evexp_eval_const_uint_exp(i);int _Tmp4;unsigned _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{unsigned i=_Tmp5;int i_known=_Tmp4;
# 170
if((!b_known || !i_known)|| i >= b)return 1;
# 176
return 0;}}}}}}
# 183
static int Cyc_InsertChecks_need_bounds_check(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
if(nv.in_sizeof)
return 0;
if(nv.at_toplevel)
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(a->loc,({const char*_Tmp0="InsertChecks: memory read at top-level";_tag_fat(_Tmp0,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));{
# 194
int bd_valid=0;union Cyc_Relations_RelnOp rop_bd=Cyc_Relations_RConst(0U);
int a_valid=0;union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RConst(0U);
int i_valid=0;union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int z_valid=0;union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
# 200
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_check_null(a->topt));
if(bound!=0)
bd_valid=Cyc_Relations_exp2relnop(bound,& rop_bd);
# 204
{void*_Tmp0=a->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 206
struct Cyc_Absyn_Vardecl*x=Cyc_Tcutil_nonesc_vardecl(b);
if(x!=0){
a_valid=1;
rop_a=Cyc_Relations_RNumelts(x);}
# 211
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 215
i_valid=Cyc_Relations_exp2relnop(i,& rop_i);
# 217
if(!i_valid){
inner_loop: {
void*_Tmp0=i->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
i=e2;goto inner_loop;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){_Tmp2=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp1=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 222
z_valid=Cyc_Relations_exp2relnop(e2,& rop_z);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA:
 goto _LL5;}_LL5:;}}{
# 228
union Cyc_Relations_RelnOp rop_left;
# 232
if(!bd_valid && !a_valid)
return 1;
if(bd_valid && a_valid){
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bd,2U,rop_a,relns);
rop_left=rop_a;}else{
if(a_valid)
rop_left=rop_a;else{
# 240
rop_left=rop_bd;}}
# 242
if(i_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,2U,rop_i,relns);else{
if(z_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,3U,rop_z,relns);else{
return 1;}}{
# 249
int ans=Cyc_Relations_consistent_relations(relns);
# 253
if(Cyc_Flags_warn_bounds_checks && ans)
Cyc_Warn_warn(a->loc,({const char*_Tmp0="inserted bounds check (location is pointer expression)";_tag_fat(_Tmp0,sizeof(char),55U);}),_tag_fat(0U,sizeof(void*),0));
return ans;}}}}
# 258
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Stmt*);
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Exp*);
# 261
static void Cyc_InsertChecks_do_exp_opt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*eo){
if(eo!=0)
Cyc_InsertChecks_do_exp(nv,eo);}
# 266
static int Cyc_InsertChecks_need_bounds_check_vcgen(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*a){
struct _tuple13 _Tmp0=Cyc_Vcgen_exp2ctxt_assn(nv.assn_info,a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*ctxt=_Tmp2;void*assn=_Tmp1;
return !Cyc_PrattProver_constraint_prove(ctxt,assn);}}struct _tuple14{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 271
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e){
# 273
void*_Tmp0=e->r;enum Cyc_Absyn_Coercion _Tmp1;int _Tmp2;enum Cyc_Absyn_Incrementor _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 40:
 goto _LL4;case 0: _LL4:
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 31: _Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp6;
# 279
_Tmp6=es;goto _LLC;}case 24: _Tmp6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLC: {struct Cyc_List_List*es=_Tmp6;
_Tmp6=es;goto _LLE;}case 3: _Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLE: {struct Cyc_List_List*es=_Tmp6;
# 282
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 4: _Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 289
_Tmp6=e1;_Tmp5=e2;goto _LL12;}case 9: _Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL14;}case 7: _Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL16;}case 8: _Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL16: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 293
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 6: _Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Exp*e3=_Tmp4;
# 298
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
Cyc_InsertChecks_do_exp(nv,e3);
goto _LL0;}case 5: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp6;enum Cyc_Absyn_Incrementor incr=_Tmp3;
# 309
if((Cyc_Tcutil_get_type_bound(_check_null(e2->topt))!=0 &&
 Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(e2->topt)))&&
 Cyc_InsertChecks_need_null_check(nv,e2))
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 314
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 10: _Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_List_List*es=_Tmp5;
# 319
if(Cyc_InsertChecks_need_null_check(nv,e1))
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 322
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,e1);
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp5;int user_inserted=_Tmp2;enum Cyc_Absyn_Coercion coercion=_Tmp1;
# 331
{struct _tuple13 _Tmp7=({struct _tuple13 _Tmp8;({void*_Tmp9=Cyc_Absyn_compress(_check_null(e1->topt));_Tmp8.f1=_Tmp9;}),({void*_Tmp9=Cyc_Absyn_compress(t);_Tmp8.f2=_Tmp9;});_Tmp8;});struct Cyc_Absyn_PtrInfo _Tmp8;struct Cyc_Absyn_PtrInfo _Tmp9;if(*((int*)_Tmp7.f1)==3){if(*((int*)_Tmp7.f2)==3){_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7.f1)->f1;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7.f2)->f1;{struct Cyc_Absyn_PtrInfo p1=_Tmp9;struct Cyc_Absyn_PtrInfo p2=_Tmp8;
# 333
int n1=Cyc_Tcutil_force_type2bool(0,p1.ptr_atts.nullable);
int n2=Cyc_Tcutil_force_type2bool(0,p2.ptr_atts.nullable);
struct Cyc_Absyn_Exp*b1=({void*_TmpA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpA,p1.ptr_atts.bounds);});
struct Cyc_Absyn_Exp*b2=({void*_TmpA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpA,p2.ptr_atts.bounds);});
if((n1 && !n2)&&(int)coercion!=2)
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_TmpA="null-check conversion mis-classified";_tag_fat(_TmpA,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));else{
if(((unsigned)b1 && !((unsigned)b2))&&(int)coercion==2)
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_TmpA="conversion mis-classified as null-check";_tag_fat(_TmpA,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));}
{struct _tuple14 _TmpA=({struct _tuple14 _TmpB;_TmpB.f1=b1,_TmpB.f2=b2;_TmpB;});if(_TmpA.f1!=0){if(_TmpA.f2!=0){
# 343
if(!({struct Cyc_Absyn_Exp*_TmpB=_check_null(b2);Cyc_Evexp_lte_const_exp(_TmpB,_check_null(b1));}))
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_TmpB="InsertChecks: cast to maybe-shorter pointer type";_tag_fat(_TmpB,sizeof(char),49U);}),_tag_fat(0U,sizeof(void*),0));
if((n1 && !n2)&& Cyc_InsertChecks_need_null_check(nv,e1)){
if(!user_inserted)
Cyc_Warn_warn(e->loc,({const char*_TmpB="inserted null check due to implicit cast from * to @ type";_tag_fat(_TmpB,sizeof(char),58U);}),_tag_fat(0U,sizeof(void*),0));
# 349
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 351
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5C;}else{
# 355
if(!Cyc_Evexp_c_can_eval(_check_null(b1)))
Cyc_Warn_err(e->loc,({const char*_TmpB="cannot perform coercion: numelts is statically unknown";_tag_fat(_TmpB,sizeof(char),55U);}),_tag_fat(0U,sizeof(void*),0));
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
goto _LL5C;}}else{if(_TmpA.f2!=0){
# 360
if(nv.at_toplevel)
Cyc_Warn_err(e->loc,({const char*_TmpB="cannot coerce fat-pointer to thin-pointer at toplevel";_tag_fat(_TmpB,sizeof(char),54U);}),_tag_fat(0U,sizeof(void*),0));
# 363
if(!Cyc_Evexp_c_can_eval(_check_null(b2)))
Cyc_Warn_err(e->loc,({const char*_TmpB="cannot perform coercion: numelts is statically unknown";_tag_fat(_TmpB,sizeof(char),55U);}),_tag_fat(0U,sizeof(void*),0));{
# 368
int bds_chk=0;
if(!n2 && Cyc_InsertChecks_need_null_check(nv,e1)){
if(!user_inserted)
Cyc_Warn_warn(e->loc,({const char*_TmpB="inserted null check due to implicit cast from *@fat (?) to @ type";_tag_fat(_TmpB,sizeof(char),66U);}),_tag_fat(0U,sizeof(void*),0));
# 373
e->annot=bds_chk?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 375
e->annot=bds_chk?(void*)& Cyc_InsertChecks_FatBound_val:(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5C;}}else{
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL5C;}}_LL5C:;}
# 379
goto _LL57;}}else{goto _LL5A;}}else{_LL5A:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL57;}_LL57:;}
# 382
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 22: _Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 385
_Tmp6=e1;goto _LL22;}case 20: _Tmp6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;_Tmp5=Cyc_InsertChecks_shared_zero_exp();goto _LL24;}case 23: _Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL24: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 388
{void*_Tmp7=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;struct Cyc_Absyn_Tqual _TmpC;void*_TmpD;switch(*((int*)_Tmp7)){case 6:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL65;case 3: _TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.elt_type;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.elt_tq;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.rgn;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.nullable;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.bounds;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.zero_term;{void*ta=_TmpD;struct Cyc_Absyn_Tqual tq=_TmpC;void*rgn=_TmpB;void*n=_TmpA;void*b=_Tmp9;void*zt=_Tmp8;
# 391
int bds_ck;
# 394
bds_ck=Cyc_InsertChecks_simple_need_bounds_check(nv,e1,e2);
# 398
if(bds_ck && nv.vcgen_r)
# 400
bds_ck=Cyc_InsertChecks_need_bounds_check_vcgen(nv,e1);
# 418 "insert_checks.cyc"
if(bds_ck && Cyc_Flags_warn_bounds_checks)
Cyc_Warn_warn(e1->loc,({const char*_TmpE="inserted bounds check (location is pointer expression)";_tag_fat(_TmpE,sizeof(char),55U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Exp*bd=({void*_TmpE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_TmpE,b);});
int fat_ptr=bd==0;
int null_ck;
# 425
if(fat_ptr && !bds_ck)null_ck=0;else{
null_ck=Cyc_InsertChecks_need_null_check(nv,e1);}
if(bds_ck){
if((unsigned)bd){
if(!Cyc_Evexp_c_can_eval(bd)){
struct _tuple13 _TmpE=Cyc_Vcgen_exp2ctxt_assn(nv.assn_info,e1);void*_TmpF;_TmpF=_TmpE.f1;{void*c=_TmpF;
Cyc_Warn_err(e->loc,({const char*_Tmp10="cannot determine subscript is in bounds";_tag_fat(_Tmp10,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,({
struct _fat_ptr _Tmp12=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(c));_Tmp11.f1=_Tmp12;});_Tmp11;});void*_Tmp11[1];_Tmp11[0]=& _Tmp10;Cyc_fprintf(Cyc_stderr,({const char*_Tmp12="[all that I can prove is %s";_tag_fat(_Tmp12,sizeof(char),28U);}),_tag_fat(_Tmp11,sizeof(void*),1));});
Cyc_fprintf(Cyc_stderr,({const char*_Tmp10="]\n";_tag_fat(_Tmp10,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}}
# 436
({void*_TmpE=
null_ck?(void*)({struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_TmpF=_cycalloc(sizeof(struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct));_TmpF->tag=Cyc_InsertChecks_NullAndThinBound,_TmpF->f1=bd;_TmpF;}):(void*)({struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_TmpF=_cycalloc(sizeof(struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct));_TmpF->tag=Cyc_InsertChecks_ThinBound,_TmpF->f1=bd;_TmpF;});
# 436
e->annot=_TmpE;});}else{
# 439
e->annot=null_ck?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_FatBound_val;}}else{
if(null_ck)
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 443
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}}
goto _LL65;}}default:
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_TmpE="InsertChecks: non-pointer-type on dereference";_tag_fat(_TmpE,sizeof(char),46U);}),_tag_fat(0U,sizeof(void*),0));}_LL65:;}
# 447
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 18: _Tmp6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 452
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
({struct Cyc_InsertChecks_Env _Tmp7=Cyc_InsertChecks_enter_sizeof(nv);Cyc_InsertChecks_do_exp(_Tmp7,e1);});
goto _LL0;}case 16: _Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*rgnopt=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp5;
# 457
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,rgnopt);
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 34: _Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*rgnopt=_Tmp6;struct Cyc_Absyn_Exp*sizeexp=_Tmp5;
# 463
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,rgnopt);
Cyc_InsertChecks_do_exp(nv,sizeexp);
goto _LL0;}case 41: _Tmp6=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 468
_Tmp6=e1;goto _LL2E;}case 11: _Tmp6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL30;}case 12: _Tmp6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL32;}case 13: _Tmp6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL34;}case 15: _Tmp6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL36;}case 42: _Tmp6=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp6;
# 474
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 32:
 goto _LL3A;case 33: _LL3A:
 goto _LL3C;case 17: _LL3C:
 goto _LL3E;case 19: _LL3E:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 21: _Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL42;}case 38: _Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL44;}case 28: _Tmp6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL44: {struct Cyc_Absyn_Exp*e1=_Tmp6;
# 484
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 27: _Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL48;}case 35: _Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL48: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 489
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 26: _Tmp6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp6;
_Tmp6=dles;goto _LL4C;}case 29: _Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL4C: {struct Cyc_List_List*dles=_Tmp6;
_Tmp6=dles;goto _LL4E;}case 30: _Tmp6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4E: {struct Cyc_List_List*dles=_Tmp6;
# 496
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(1;dles!=0;dles=dles->tl){
Cyc_InsertChecks_do_exp(nv,(*((struct _tuple15*)dles->hd)).f2);}
goto _LL0;}case 37: _Tmp6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;Cyc_InsertChecks_do_stmt(nv,s);goto _LL0;}case 36:
# 502
 goto _LL54;case 25: _LL54:
 goto _LL56;default: _LL56:
({(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;})(e->loc,({const char*_Tmp7="InsertChecks, unexpected exp form";_tag_fat(_Tmp7,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 508
static void Cyc_InsertChecks_do_decl(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
Cyc_InsertChecks_do_exp_opt(nv,vd->initializer);goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 512
struct Cyc_InsertChecks_Env _Tmp2=Cyc_InsertChecks_fn_body_env(nv);struct Cyc_InsertChecks_Env nv=_Tmp2;
({struct Cyc_Hashtable_Table**_Tmp3=Cyc_Vcgen_new_assn_info();nv.assn_info=_Tmp3;});
if(nv.vcgen_r)
# 521
Cyc_Vcgen_vcgen_fundecl(nv.tables,fd,nv.assn_info);
# 538 "insert_checks.cyc"
Cyc_InsertChecks_do_stmt(nv,fd->body);
# 550 "insert_checks.cyc"
goto _LL0;}case 2: _Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp1;
Cyc_InsertChecks_do_exp(nv,e);goto _LL0;}case 3:
 goto _LL0;case 4: _Tmp1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eo=_Tmp1;
Cyc_InsertChecks_do_exp_opt(nv,eo);goto _LL0;}case 5:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL0;case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL20;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL20: {struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL22;}default: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL22: {struct Cyc_List_List*ds=_Tmp1;
# 566
for(1;ds!=0;ds=ds->tl){
Cyc_InsertChecks_do_decl(nv,(struct Cyc_Absyn_Decl*)ds->hd);}}}_LL0:;}
# 571
static void Cyc_InsertChecks_do_scs(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*scs){
# 574
for(1;scs!=0;scs=scs->tl){
if(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause!=0)
Cyc_InsertChecks_do_exp(nv,_check_null(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause));
Cyc_InsertChecks_do_stmt(nv,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}}
# 581
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_InsertChecks_do_exp(nv,e);goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_stmt(nv,s2);goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
if((unsigned)eopt)Cyc_InsertChecks_do_exp(nv,eopt);goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 588
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_stmt(nv,s2);goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp4=e;_Tmp3=s1;goto _LLE;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 592
Cyc_InsertChecks_do_exp(nv,e1);Cyc_InsertChecks_do_exp(nv,e2);Cyc_InsertChecks_do_exp(nv,e3);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 6:
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 11: _Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 597
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;
Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
Cyc_InsertChecks_do_decl(nv,d);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_scs(nv,scs);goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_scs(nv,scs);goto _LL0;}}_LL0:;}
# 607
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,int do_vcgen){
# 617 "insert_checks.cyc"
struct Cyc_InsertChecks_Env nv=Cyc_InsertChecks_toplevel_env(tables,do_vcgen);
for(1;tds!=0;tds=tds->tl){
Cyc_InsertChecks_do_decl(nv,(struct Cyc_Absyn_Decl*)tds->hd);}}
