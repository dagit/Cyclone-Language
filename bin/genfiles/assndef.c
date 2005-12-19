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
extern void*Cyc_List_nth(struct Cyc_List_List*,int);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 167
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 522 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};
# 927 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 933
void*Cyc_Absyn_compress(void*);
# 951
extern void*Cyc_Absyn_uint_type;
# 953
extern void*Cyc_Absyn_sint_type;
# 960
extern void*Cyc_Absyn_heap_rgn_type;
# 962
extern void*Cyc_Absyn_al_qual_type;
# 966
extern void*Cyc_Absyn_false_type;
# 991
void*Cyc_Absyn_exn_type (void);
# 1017
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 74
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 42
int Cyc_Tcutil_is_pointer_type(void*);
# 60
void*Cyc_Tcutil_pointer_elt_type(void*);
# 129 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 40 "warn.h"
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
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 77
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 81
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 87
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 116
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);
# 92
extern void Cyc_Hashtable_iter_c(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 149 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
# 163
void*Cyc_AssnDef_select(void*,void*,void*);
# 167
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 169
void*Cyc_AssnDef_aggr(int,unsigned,struct Cyc_List_List*,void*);
# 171
void*Cyc_AssnDef_proj(void*,unsigned,void*);
void*Cyc_AssnDef_aggr_update(void*,unsigned,void*);
# 175
void*Cyc_AssnDef_offsetf(void*,unsigned,void*);
# 180
void*Cyc_AssnDef_plus(void*,void*,void*);struct _tuple12{void*f0;struct Cyc_List_List*f1;};
# 200
void*Cyc_AssnDef_get_term_type(void*);
# 213 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 225
int Cyc_AssnDef_assncmp(void*,void*);
# 229
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 234
void*Cyc_AssnDef_not(void*);struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};struct _tuple13{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 289
struct _tuple13 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*,struct Cyc_AssnDef_AssnMap);struct _tuple14{void*f0;void*f1;struct Cyc_Dict_Dict f2;};
# 351 "assndef.h"
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 71 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_typopt2string(void*tp){
if(tp==0)return _tag_fat("NULL",sizeof(char),5U);
return Cyc_Absynpp_typ2string(tp);}static char _TmpG0[2U]=",";static char _TmpG1[2U]="}";static char _TmpG2[2U]="{";
# 76
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{enum Cyc_Absyn_Primop _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)e);_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s[%d]",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 15: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("tagof_tm(%s)",sizeof(char),13U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 6: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*tp=_Tmp2;
# 83
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_typopt2string(tp);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Sel(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 86
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_term2string(t3);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 2: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp1;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}}else{_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp1;
# 90
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}case 3: _Tmp0=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;
# 92
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_prim2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t1);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 4: _Tmp0=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;void*t2=_Tmp3;
# 94
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_prim2string(p);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=Cyc_AssnDef_term2string(t2);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*tm=_Tmp3;
# 98
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_typ2string(tp);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(tm);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp1;unsigned tag=_Tmp5;struct Cyc_List_List*tms=_Tmp4;
# 100
static struct _fat_ptr lb={_TmpG2,_TmpG2,_TmpG2 + 2U};
static struct _fat_ptr rb={_TmpG1,_TmpG1,_TmpG1 + 2U};
static struct _fat_ptr comma={_TmpG0,_TmpG0,_TmpG0 + 2U};
struct Cyc_List_List*ss;ss=_cycalloc(sizeof(struct Cyc_List_List)),ss->hd=& lb,ss->tl=0;
for(1;tms!=0;tms=tms->tl){
struct _fat_ptr s=Cyc_AssnDef_term2string((void*)tms->hd);
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=s;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=ss;_Tmp6;});
if(tms->tl!=0)ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& comma,_Tmp6->tl=ss;_Tmp6;});}
# 109
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& rb,_Tmp6->tl=ss;_Tmp6;});
ss=Cyc_List_imp_rev(ss);
res=Cyc_strconcat_l(ss);
if(tag!=4294967295U)res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)tag);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=res;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("tag(%d)%s",sizeof(char),10U),_tag_fat(_Tmp8,sizeof(void*),2));});
if(is_union)res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=res;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("union%s",sizeof(char),8U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp3;
# 116
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)i);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("#%d(%s)",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 10: _Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*a=_Tmp4;unsigned i=_Tmp5;void*v=_Tmp3;
# 119
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(a);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)i);_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_term2string(v);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Aupd(%s,%d,%s)",sizeof(char),15U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 11: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;void*invoke=_Tmp3;
# 122
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(invoke);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned)vd;_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("&%s_%s_%x",sizeof(char),10U),_tag_fat(_Tmp9,sizeof(void*),3));});
# 125
goto _LL0;}case 12: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;int id=_Tmp1;void*nelts=_Tmp3;void*invoke=_Tmp2;
# 127
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)e);_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)id;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_term2string(nelts);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_AssnDef_term2string(invoke);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[4];_TmpA[0]=& _Tmp6,_TmpA[1]=& _Tmp7,_TmpA[2]=& _Tmp8,_TmpA[3]=& _Tmp9;Cyc_aprintf(_tag_fat("Alloc(%d,%d,%s,%s)",sizeof(char),19U),_tag_fat(_TmpA,sizeof(void*),4));});
goto _LL0;}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp3;
# 130
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)i);_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_typopt2string(tp);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Offsetf(%s,%d,%s)",sizeof(char),18U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 133
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s.+.%s",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}}_LL0:;}
# 136
return res;}
# 139
static int Cyc_AssnDef_hash_const(union Cyc_Absyn_Cnst c){
struct _fat_ptr _Tmp0;long long _Tmp1;int _Tmp2;short _Tmp3;char _Tmp4;switch(c.String_c.tag){case 1:
 return 0;case 2: _Tmp4=c.Char_c.val.f1;{char c=_Tmp4;
return(int)c;}case 4: _Tmp3=c.Short_c.val.f1;{short c=_Tmp3;
return(int)c;}case 5: _Tmp2=c.Int_c.val.f1;{int i=_Tmp2;
return i;}case 6: _Tmp1=c.LongLong_c.val.f1;{long long i=_Tmp1;
return(int)i;}case 7: _Tmp0=c.Float_c.val.f0;{struct _fat_ptr s=_Tmp0;
_Tmp0=s;goto _LLE;}case 3: _Tmp0=c.Wchar_c.val;_LLE: {struct _fat_ptr s=_Tmp0;
_Tmp0=s;goto _LL10;}case 8: _Tmp0=c.String_c.val;_LL10: {struct _fat_ptr s=_Tmp0;
_Tmp0=s;goto _LL12;}default: _Tmp0=c.Wstring_c.val;_LL12: {struct _fat_ptr s=_Tmp0;
# 150
return Cyc_Hashtable_hash_string(s);}};}
# 154
static int Cyc_AssnDef_hash_binding(void*b){
void*_Tmp0;switch(*((int*)b)){case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return(int)(((unsigned)vd >> 4U)+ 21U);}case 5: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return(int)(((unsigned)vd >> 4U)+ 23U);}case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return(int)(((unsigned)vd >> 4U)+ 27U);}case 2: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*vd=_Tmp0;
return(int)(((unsigned)vd >> 4U)+ 29U);}case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return(int)(((unsigned)vd >> 4U)+ 31U);}default: _Tmp0=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)b)->f1;{struct _tuple0*q=_Tmp0;
return 1;}};}
# 165
static int Cyc_AssnDef_hash_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;union Cyc_Absyn_Cnst _Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_Tmp2;
return Cyc_AssnDef_hash_const(c);}case 1: _Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
return Cyc_AssnDef_hash_binding(b);}default:
 return(int)(*((const unsigned*)e->r)+ 1U);};}
# 173
int Cyc_AssnDef_termhash(void*t){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(int)i;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_AssnDef_hash_exp(e)+ 61;}case 2: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;
return(int)(((unsigned)vd >> 4U)+ (unsigned)(i + 1));}case 3: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;
return(int)(((unsigned)p + ((unsigned)t1 >> 4U))+ 1U);}case 4: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;void*t2=_Tmp1;
# 180
return(int)((unsigned)p + ((unsigned)t1 + (unsigned)t2 >> 4U));}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;
return Cyc_AssnDef_termhash(t)+ 1;}case 6: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp1;
# 183
return(int)(((unsigned)t1 + (unsigned)t2 >> 4U)+ 2U);}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp1;void*t3=_Tmp0;
# 185
return(int)((((unsigned)t1 + (unsigned)t2)+ (unsigned)t3 >> 4U)+ 3U);}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
return(int)((((unsigned)t >> 4U)+ i)+ 23U);}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp3;unsigned tag=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 188
unsigned res=(37U + tag)+ (unsigned)is_union;
for(1;ts!=0;ts=ts->tl){
res=res + ((unsigned)((void*)ts->hd)>> 4U);}
return(int)res;}case 10: _Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*a=_Tmp4;unsigned i=_Tmp5;void*v=_Tmp1;
# 193
return(int)((((unsigned)a + (unsigned)v >> 4U)+ i)+ 17U);}case 11: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;void*invok=_Tmp1;
# 195
return(int)(((unsigned)invok + (unsigned)vd >> 4U)+ 7U);}case 12: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;int i=_Tmp3;void*n=_Tmp1;void*invok=_Tmp0;
# 197
return(int)((((unsigned)e + (unsigned)n)+ (unsigned)invok >> 4U)+ (unsigned)i);}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
return(int)((((unsigned)t >> 4U)+ i)+ 11U);}case 14: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;void*index=_Tmp1;
# 200
return(int)(((unsigned)index + (unsigned)t >> 4U)+ 13U);}default: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(int)(((unsigned)t >> 4U)+ 741U);}};}struct _tuple15{void*f0;void*f1;};
# 205
int Cyc_AssnDef_termcmp(void*_t1,void*_t2){
void*t1=_t1;
void*t2=_t2;
LOOP: {
int c=(int)(*((const unsigned*)t1)- *((const unsigned*)t2));
if(c!=0)return c;
if(t1==t2)return 0;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Primop _Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;enum Cyc_Absyn_Primop _TmpC;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_TmpC=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp8=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_TmpC;void*t11=_TmpB;void*t12=_TmpA;void*tp1=_Tmp9;enum Cyc_Absyn_Primop p2=_Tmp8;void*t21=_Tmp7;void*t22=_Tmp6;void*tp2=_Tmp5;
# 214
int c=(int)p1 - (int)p2;
c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
t1=t12;
t2=t22;
goto LOOP;}}else{goto _LL21;}case 2: if(*((int*)_Tmp0.f1)==2){_TmpB=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*vd1=_TmpB;int i=_Tmp4;struct Cyc_Absyn_Vardecl*vd2=_TmpA;int j=_Tmp3;
# 221
int c=i - j;
if(c!=0)return c;
return(int)vd1 - (int)vd2;}}else{goto _LL21;}case 0: if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i1=_Tmp2;unsigned i2=_Tmp1;
return(int)(i1 - i2);}}else{goto _LL21;}case 1: if(*((int*)_Tmp0.f1)==1){_TmpB=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_TmpB;struct Cyc_Absyn_Exp*e2=_TmpA;
# 226
if(Cyc_Evexp_exp_cmp(e1,e2))return(int)e1 - (int)e2;else{return 0;}}}else{goto _LL21;}case 11: if(*((int*)_Tmp0.f1)==11){_TmpB=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*vd1=_TmpB;void*i1=_TmpA;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;void*i2=_Tmp7;
# 228
int c=(int)vd1 - (int)vd2;
if(c!=0)return c;
t1=i1;
t2=i2;
goto LOOP;}}else{goto _LL21;}case 3: if(*((int*)_Tmp0.f1)==3){_TmpC=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_Primop p1=_TmpC;void*t1a=_TmpB;enum Cyc_Absyn_Primop p2=_Tmp8;void*t2a=_TmpA;
# 234
int c=(int)p1 - (int)p2;
if(c!=0)return c;
t1=t1a;
t2=t2a;
goto LOOP;}}else{goto _LL21;}case 5: if(*((int*)_Tmp0.f1)==5){_TmpB=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_TmpB;void*tm1=_TmpA;void*tp2=_Tmp9;void*tm2=_Tmp7;
# 240
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL21;}case 6: if(*((int*)_Tmp0.f1)==6){_TmpB=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t11=_TmpB;void*t12=_TmpA;void*t21=_Tmp9;void*t22=_Tmp7;
# 246
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
t1=t12;
t2=t22;
goto LOOP;}}else{goto _LL21;}case 7: if(*((int*)_Tmp0.f1)==7){_TmpB=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*t11=_TmpB;void*t12=_TmpA;void*t13=_Tmp9;void*t21=_Tmp7;void*t22=_Tmp6;void*t23=_Tmp5;
# 252
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(t12,t22);
if(c!=0)return c;
t1=t13;
t2=t23;
goto LOOP;}}else{goto _LL21;}case 15: if(*((int*)_Tmp0.f1)==15){_TmpB=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm1=_TmpB;void*tm2=_TmpA;
# 260
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL21;}case 9: if(*((int*)_Tmp0.f1)==9){_TmpB=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpB;unsigned i1=_Tmp2;void*tm2=_TmpA;unsigned i2=_Tmp1;
# 264
int c=(int)(i1 - i2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL21;}case 10: if(*((int*)_Tmp0.f1)==10){_TmpB=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*tm1=_TmpB;unsigned i1=_Tmp2;void*v1=_TmpA;void*tm2=_Tmp9;unsigned i2=_Tmp1;void*v2=_Tmp7;
# 270
int c=(int)(i1 - i2);
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(tm1,tm2);
if(c!=0)return c;
t1=v1;
t2=v2;
goto LOOP;}}else{goto _LL21;}case 12: if(*((int*)_Tmp0.f1)==12){_TmpB=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp7=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*e1=_TmpB;int i1=_Tmp4;void*n1=_TmpA;void*v1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;int i2=_Tmp3;void*n2=_Tmp6;void*v2=_Tmp5;
# 278
int c=(int)e1 - (int)e2;
if(c!=0)return c;
c=i1 - i2;
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(n1,n2);
if(c!=0)return c;
t1=v1;
t2=v2;
goto LOOP;}}else{goto _LL21;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpB=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpB;unsigned i1=_Tmp2;void*tm2=_TmpA;unsigned i2=_Tmp1;
# 288
int c=(int)(i1 - i2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL21;}case 14: if(*((int*)_Tmp0.f1)==14){_TmpB=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpB;void*i1=_TmpA;void*tm2=_Tmp9;void*i2=_Tmp7;
# 294
int c=Cyc_AssnDef_termcmp(i1,i2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL21;}default: if(*((int*)_Tmp0.f1)==8){_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpB=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f2;_TmpA=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f3;{int is_union1=_Tmp4;unsigned tag1=_Tmp2;struct Cyc_List_List*ts1=_TmpB;int is_union2=_Tmp3;unsigned tag2=_Tmp1;struct Cyc_List_List*ts2=_TmpA;
# 300
int c=(int)(tag1 - tag2);
if(c!=0)return c;
c=is_union2 - is_union1;
if(c!=0)return c;
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
c=Cyc_AssnDef_termcmp((void*)ts1->hd,(void*)ts2->hd);
if(c!=0)return c;}
# 308
if(ts1==ts2)return 0;
if(ts1==0)return -1;
return 1;}}else{_LL21:
({int(*_TmpD)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpD;})(_tag_fat("different terms but same codes!",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));}};}}}
# 315
static void*Cyc_AssnDef_copy_term(void*t){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;unsigned _Tmp6;switch(*((int*)t)){case 0: _Tmp6=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f2;{unsigned i=_Tmp6;void*tp=_Tmp5;
return(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp7->tag=0,_Tmp7->f1=i,_Tmp7->f2=tp;_Tmp7;});}case 1: _Tmp5=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_Tmp7->tag=1,_Tmp7->f1=e;_Tmp7;});}case 11: _Tmp5=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;void*t=_Tmp4;void*tp=_Tmp3;
return(void*)({struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Addr_AssnDef_Term_struct));_Tmp7->tag=11,_Tmp7->f1=vd,_Tmp7->f2=t,_Tmp7->f3=tp;_Tmp7;});}case 2: _Tmp5=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;int i=_Tmp2;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp7->tag=2,_Tmp7->f1=vd,_Tmp7->f2=i,_Tmp7->f3=tp;_Tmp7;});}case 4: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp3;
return(void*)({struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Binop_AssnDef_Term_struct));_Tmp7->tag=4,_Tmp7->f1=p,_Tmp7->f2=t1,_Tmp7->f3=t2,_Tmp7->f4=tp;_Tmp7;});}case 3: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t=_Tmp5;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Unop_AssnDef_Term_struct));_Tmp7->tag=3,_Tmp7->f1=p,_Tmp7->f2=t,_Tmp7->f3=tp;_Tmp7;});}case 5: _Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp5;void*t=_Tmp4;
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_Tmp7->tag=5,_Tmp7->f1=tp,_Tmp7->f2=t;_Tmp7;});}case 6: _Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp3;
return(void*)({struct Cyc_AssnDef_Select_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Select_AssnDef_Term_struct));_Tmp7->tag=6,_Tmp7->f1=t1,_Tmp7->f2=t2,_Tmp7->f3=tp;_Tmp7;});}case 7: _Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*t3=_Tmp3;
return(void*)({struct Cyc_AssnDef_Update_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Update_AssnDef_Term_struct));_Tmp7->tag=7,_Tmp7->f1=t1,_Tmp7->f2=t2,_Tmp7->f3=t3;_Tmp7;});}case 9: _Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp6=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp5;unsigned i=_Tmp6;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Proj_AssnDef_Term_struct));_Tmp7->tag=9,_Tmp7->f1=t,_Tmp7->f2=i,_Tmp7->f3=tp;_Tmp7;});}case 10: _Tmp5=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp6=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp5;unsigned i=_Tmp6;void*v=_Tmp4;
return(void*)({struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct));_Tmp7->tag=10,_Tmp7->f1=t,_Tmp7->f2=i,_Tmp7->f3=v;_Tmp7;});}case 8: _Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp6=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int is_union=_Tmp2;unsigned tag=_Tmp6;struct Cyc_List_List*ts=_Tmp5;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct));_Tmp7->tag=8,_Tmp7->f1=is_union,_Tmp7->f2=tag,_Tmp7->f3=ts,_Tmp7->f4=tp;_Tmp7;});}case 12: _Tmp5=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;_Tmp0=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f5;{struct Cyc_Absyn_Exp*e=_Tmp5;int i=_Tmp2;void*n=_Tmp4;void*v=_Tmp3;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct));_Tmp7->tag=12,_Tmp7->f1=e,_Tmp7->f2=i,_Tmp7->f3=n,_Tmp7->f4=v,_Tmp7->f5=tp;_Tmp7;});}case 13: _Tmp5=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp6=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp5;unsigned i=_Tmp6;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct));_Tmp7->tag=13,_Tmp7->f1=t,_Tmp7->f2=i,_Tmp7->f3=tp;_Tmp7;});}case 14: _Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp5;void*i=_Tmp4;void*tp=_Tmp3;
return(void*)({struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct));_Tmp7->tag=14,_Tmp7->f1=t,_Tmp7->f2=i,_Tmp7->f3=tp;_Tmp7;});}default: _Tmp5=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp5;
return(void*)({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct));_Tmp7->tag=15,_Tmp7->f1=t;_Tmp7;});}};}
# 336
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_zero_value={0,0U,0};
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_one_value={0,1U,0};
# 339
static struct Cyc_Set_Set*Cyc_AssnDef_empty_term_set (void){
static struct Cyc_Set_Set**term_set=0;
if(term_set==0)
term_set=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=Cyc_Set_empty(Cyc_AssnDef_termcmp);*_Tmp0=_Tmp1;});_Tmp0;});
# 344
return*term_set;}struct Cyc_AssnDef_TermHashedInfo{void*term;struct Cyc_Set_Set**free_logicvar_set;};
# 353
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 356
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_term_hash_cons_table (void){
# 358
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table==0){
h=Cyc_Hashtable_create(221,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;{
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_zero_value,({struct Cyc_AssnDef_TermHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp2->term=(void*)& Cyc_AssnDef_zero_value,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_one_value,({struct Cyc_AssnDef_TermHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp2->term=(void*)& Cyc_AssnDef_one_value,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});}}else{
# 366
h=Cyc_AssnDef_term_hash_cons_table;}
# 368
return h;}
# 371
static void*Cyc_AssnDef_hash_cons_term(void*t){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();
struct Cyc_AssnDef_TermHashedInfo**resopt=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(h,t,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
if(resopt==0){
void*t2=Cyc_AssnDef_copy_term(t);
# 377
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=t2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_TermHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp3->term=t2,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
# 380
return t2;}else{
# 382
return(*resopt)->term;}}
# 388
struct Cyc_Set_Set*Cyc_AssnDef_term_fr_logicvar(void*t){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();
struct Cyc_AssnDef_TermHashedInfo**thinfo=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,t);
# 392
if(thinfo==0){
t=Cyc_AssnDef_hash_cons_term(t);
thinfo=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,t);}
# 396
if((*_check_null(thinfo))->free_logicvar_set!=0)
return*(*thinfo)->free_logicvar_set;{
struct Cyc_Set_Set*res;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
# 402
 res=Cyc_AssnDef_empty_term_set();goto _LL0;case 2:
# 404
 res=Cyc_Set_singleton(Cyc_AssnDef_termcmp,t);
goto _LL0;case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{void*t1=_Tmp2;void*t2=_Tmp1;
# 407
_Tmp2=t1;_Tmp1=t2;goto _LLA;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_LLA: {void*t1=_Tmp2;void*t2=_Tmp1;
# 409
_Tmp2=t1;_Tmp1=t2;goto _LLC;}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;_LLC: {void*t1=_Tmp2;void*t2=_Tmp1;
# 411
_Tmp2=t1;_Tmp1=t2;goto _LLE;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_LLE: {void*t1=_Tmp2;void*t2=_Tmp1;
# 413
_Tmp2=t1;_Tmp1=t2;goto _LL10;}case 14: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_LL10: {void*t1=_Tmp2;void*t2=_Tmp1;
# 415
res=({struct Cyc_Set_Set*_Tmp3=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp3,Cyc_AssnDef_term_fr_logicvar(t2));});
goto _LL0;}case 7: _Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;void*t3=_Tmp0;
# 418
res=({struct Cyc_Set_Set*_Tmp3=({struct Cyc_Set_Set*_Tmp4=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp4,
Cyc_AssnDef_term_fr_logicvar(t2));});
# 418
Cyc_Set_union_two(_Tmp3,
# 420
Cyc_AssnDef_term_fr_logicvar(t3));});
goto _LL0;}case 11: _Tmp2=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp2;
# 423
_Tmp2=t1;goto _LL16;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_LL16: {void*t1=_Tmp2;
# 425
_Tmp2=t1;goto _LL18;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_LL18: {void*t1=_Tmp2;
# 427
_Tmp2=t1;goto _LL1A;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_LL1A: {void*t1=_Tmp2;
# 429
_Tmp2=t1;goto _LL1C;}case 15: _Tmp2=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;_LL1C: {void*t1=_Tmp2;
# 431
_Tmp2=t1;goto _LL1E;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;_LL1E: {void*t1=_Tmp2;
# 433
res=Cyc_AssnDef_term_fr_logicvar(t1);
goto _LL0;}default: _Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{struct Cyc_List_List*ts=_Tmp2;
# 436
struct Cyc_List_List*tsets=({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_AssnDef_term_fr_logicvar,ts);
res=({struct Cyc_Set_Set*(*_Tmp3)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*)=({struct Cyc_Set_Set*(*_Tmp4)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp4;});struct Cyc_Set_Set*_Tmp4=Cyc_AssnDef_empty_term_set();_Tmp3(Cyc_Set_union_two,_Tmp4,tsets);});
goto _LL0;}}_LL0:;}
# 440
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=res;_Tmp1;});(*thinfo)->free_logicvar_set=_Tmp0;});
return res;}}
# 444
static int Cyc_AssnDef_logicvar_in_term(void*lvar,void*t){
struct Cyc_Set_Set*lvset=Cyc_AssnDef_term_fr_logicvar(t);
return Cyc_Set_member(lvset,lvar);}
# 449
void*Cyc_AssnDef_uint(unsigned i){
struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=({struct Cyc_AssnDef_Uint_AssnDef_Term_struct _Tmp0;_Tmp0.tag=0,_Tmp0.f1=i,_Tmp0.f2=0;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 453
void*Cyc_AssnDef_zero (void){
return(void*)& Cyc_AssnDef_zero_value;}
# 456
void*Cyc_AssnDef_one (void){
return(void*)& Cyc_AssnDef_one_value;}
# 459
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned c=_Tmp2;int known=_Tmp1;
if(known)return Cyc_AssnDef_uint(c);{
struct Cyc_AssnDef_Const_AssnDef_Term_struct t=({struct Cyc_AssnDef_Const_AssnDef_Term_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=e;_Tmp3;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}}
# 465
void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*vd,void*invok){
void*tp=vd->type;
{void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==5)
goto _LL0;else{
# 470
void*rgn=(unsigned)vd->rgn?vd->rgn: Cyc_Absyn_heap_rgn_type;
tp=({void*_Tmp1=tp;void*_Tmp2=rgn;void*_Tmp3=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp4=Cyc_Absyn_empty_tqual(0U);void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);});}_LL0:;}{
# 473
struct Cyc_AssnDef_Addr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=11,_Tmp0.f1=vd,_Tmp0.f2=invok,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 476
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){
struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=vd,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 480
void*Cyc_AssnDef_update(void*t1,void*t2,void*t3){
LOOP1: {
# 488
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;if(*((int*)t2)==13){_Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t2)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t2)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t2)->f3;{void*a=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
# 490
t2=a;{
void*tpopt=0;
if(tp!=0 && Cyc_Tcutil_is_pointer_type(tp))
tpopt=Cyc_Tcutil_pointer_elt_type(tp);
t3=({void*_Tmp3=Cyc_AssnDef_select(t1,a,tp);unsigned _Tmp4=i;Cyc_AssnDef_aggr_update(_Tmp3,_Tmp4,t3);});
goto LOOP1;}}}else{
# 497
goto _LL0;}_LL0:;}
# 500
LOOP2: {
void*_Tmp0;void*_Tmp1;if(*((int*)t1)==7){_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t1)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t1)->f2;if((void*)_Tmp0==t2){void*m=_Tmp1;void*t=_Tmp0;
# 503
t1=m;
goto LOOP2;}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}{
# 507
struct Cyc_AssnDef_Update_AssnDef_Term_struct t=({struct Cyc_AssnDef_Update_AssnDef_Term_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=t1,_Tmp0.f2=t2,_Tmp0.f3=t3;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 511
void*Cyc_AssnDef_aggr_update(void*t1,unsigned i,void*t2){
LOOP: {
int _Tmp0;void*_Tmp1;unsigned _Tmp2;void*_Tmp3;switch(*((int*)t1)){case 10: _Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp2=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f3;{void*t=_Tmp3;unsigned j=_Tmp2;void*t3=_Tmp1;
# 515
if(i==j){
# 517
t1=t;
goto LOOP;}
# 520
if(i < j){
# 523
void*_Tmp4=Cyc_AssnDef_aggr_update(t,i,t2);unsigned _Tmp5=j;return Cyc_AssnDef_aggr_update(_Tmp4,_Tmp5,t3);}
# 525
goto _LL0;}case 8: _Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f4;{int is_union=_Tmp0;unsigned tag=_Tmp2;struct Cyc_List_List*ts=_Tmp3;void*tp=_Tmp1;
# 528
if(is_union){int _Tmp4=is_union;unsigned _Tmp5=i;struct Cyc_List_List*_Tmp6=({void*_Tmp7[1];_Tmp7[0]=t2;Cyc_List_list(_tag_fat(_Tmp7,sizeof(void*),1));});return Cyc_AssnDef_aggr(_Tmp4,_Tmp5,_Tmp6,tp);}{
# 530
struct Cyc_List_List*revnewts=0;
for(1;i!=0U;-- i){
if(ts==0)goto _LL6;
revnewts=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(void*)ts->hd,_Tmp4->tl=revnewts;_Tmp4;});
ts=ts->tl;}
# 536
if(ts==0)goto _LL6;{
struct Cyc_List_List*newts=({struct Cyc_List_List*_Tmp4=revnewts;Cyc_List_revappend(_Tmp4,({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=t2,_Tmp5->tl=ts->tl;_Tmp5;}));});
void*res=Cyc_AssnDef_aggr(is_union,tag,newts,tp);
return res;}}}default: _LL6:
# 541
 goto _LL0;}_LL0:;}{
# 543
struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct t=({struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct _Tmp0;_Tmp0.tag=10,_Tmp0.f1=t1,_Tmp0.f2=i,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 547
static int Cyc_AssnDef_address_disjoint(void*,void*);
# 550
static int Cyc_AssnDef_term_neq(void*t1,void*t2){
LOOP: {
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;unsigned _Tmp6;switch(*((int*)_Tmp0.f0)){case 0: switch(*((int*)_Tmp0.f1)){case 0: _Tmp6=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp6;unsigned j=_Tmp5;
return i!=j;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus)goto _LL7;else{goto _LLD;}case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}default: goto _LLD;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Plus){if(*((int*)_Tmp0.f1)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((void*)_Tmp4==(void*)_Tmp2){void*t11=_Tmp4;void*t12=_Tmp3;void*t21=_Tmp2;void*t22=_Tmp1;
# 556
t1=t12;
t2=t22;
goto LOOP;}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp2=_Tmp0.f1;if((void*)_Tmp4==(void*)_Tmp2){void*t11=_Tmp4;void*t12=_Tmp3;void*t21=_Tmp2;
# 560
t1=t12;
t2=Cyc_AssnDef_zero();
goto LOOP;}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}}else{switch(*((int*)_Tmp0.f1)){case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus)goto _LL7;else{goto _LLD;}case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}default: goto _LLD;}}default: if(*((int*)_Tmp0.f1)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus){_LL7: _Tmp4=_Tmp0.f0;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((void*)_Tmp4==(void*)_Tmp3){void*t11=_Tmp4;void*t21=_Tmp3;void*t22=_Tmp2;
# 564
t1=Cyc_AssnDef_zero();
t2=t22;
goto LOOP;}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot)goto _LL9;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot)goto _LL9;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot){_LL9: _Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=_Tmp0.f1;if((void*)_Tmp4==(void*)_Tmp3){void*t1=_Tmp4;void*t2=_Tmp3;
return 1;}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot){_LLB: _Tmp4=_Tmp0.f0;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;if((void*)_Tmp4==(void*)_Tmp3){void*t1=_Tmp4;void*t2=_Tmp3;
return 1;}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
# 571
 return Cyc_AssnDef_address_disjoint(t1,t2);}}}};}}
# 577
static int Cyc_AssnDef_address_disjoint(void*t1,void*t2){
LOOP:
 if(t1==t2)return 0;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp0.f0)){case 11: switch(*((int*)_Tmp0.f1)){case 11: _TmpA=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*x=_TmpA;void*nv1=_Tmp9;struct Cyc_Absyn_Vardecl*y=_Tmp8;void*nv2=_Tmp7;
# 583
return x!=y;}case 12:
 return 1;case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;}case 12: switch(*((int*)_Tmp0.f1)){case 11:
 return 1;case 12: _TmpA=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp7=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*exp1=_TmpA;int i1=_Tmp6;void*tm1=_Tmp9;void*nv1=_Tmp8;struct Cyc_Absyn_Exp*exp2=_Tmp7;int i2=_Tmp5;void*tm2=_Tmp4;void*nv2=_Tmp3;
# 590
return exp1!=exp2 || i1!=i2;}case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpA;unsigned i=_Tmp2;void*tm2=_Tmp9;unsigned j=_Tmp1;
# 594
if(tm1==tm2 && i!=j)return 1;
t1=tm1;
t2=tm2;
goto LOOP;}}else{_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*tm1=_TmpA;
# 606
t1=tm1;
goto LOOP;}}case 14: switch(*((int*)_Tmp0.f1)){case 14: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpA;void*i1=_Tmp9;void*tm2=_Tmp8;void*i2=_Tmp7;
# 599
if(tm1==tm2 && Cyc_AssnDef_term_neq(i1,i2))return 1;
t1=tm1;
t2=tm2;
goto LOOP;}case 13: goto _LLF;default: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*tm1=_TmpA;
# 612
t1=tm1;
goto LOOP;}}default: switch(*((int*)_Tmp0.f1)){case 13: _LLF: _TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm2=_TmpA;
# 609
t2=tm2;
goto LOOP;}case 14: _LL13: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm2=_TmpA;
# 615
t2=tm2;
goto LOOP;}default: _LL15:
 return 0;}};}}struct _tuple16{int f0;void*f1;};
# 621
struct _tuple12 Cyc_AssnDef_split_addr(void*a){
struct Cyc_List_List*fields=0;
while(1){
{void*_Tmp0;unsigned _Tmp1;void*_Tmp2;if(*((int*)a)==13){_Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f3;{void*r=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
# 626
fields=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp4=({struct _tuple16*_Tmp5=_cycalloc(sizeof(struct _tuple16));_Tmp5->f0=(int)i,_Tmp5->f1=tp;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=fields;_Tmp3;});
a=r;
continue;}}else{
# 630
struct _tuple12 _Tmp3;_Tmp3.f0=a,_Tmp3.f1=fields;return _Tmp3;};}
# 624
1U;}}
# 635
void*Cyc_AssnDef_apply_aggr_update(void*a,struct Cyc_List_List*fields,void*v){
if(fields==0)return v;{
struct _tuple16*_Tmp0=(struct _tuple16*)fields->hd;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{int i=_Tmp2;void*tpopt=_Tmp1;
void*tp=0;
if(tpopt!=0 && Cyc_Tcutil_is_pointer_type(tpopt))
tp=Cyc_Tcutil_pointer_elt_type(tpopt);{
# 642
void*res=({void*_Tmp3=a;unsigned _Tmp4=(unsigned)i;Cyc_AssnDef_aggr_update(_Tmp3,_Tmp4,({
void*_Tmp5=Cyc_AssnDef_proj(a,(unsigned)i,tp);struct Cyc_List_List*_Tmp6=fields->tl;Cyc_AssnDef_apply_aggr_update(_Tmp5,_Tmp6,v);}));});
return res;}}}}
# 647
static void*Cyc_AssnDef_proj_fields(void*v,struct Cyc_List_List*fs){
for(1;fs!=0;fs=fs->tl){
struct _tuple16*_Tmp0=(struct _tuple16*)fs->hd;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{int i=_Tmp2;void*tpopt=_Tmp1;
if(tpopt!=0 && Cyc_Tcutil_is_pointer_type(tpopt))
v=({void*_Tmp3=v;unsigned _Tmp4=(unsigned)i;Cyc_AssnDef_proj(_Tmp3,_Tmp4,Cyc_Tcutil_pointer_elt_type(tpopt));});else{
# 653
v=Cyc_AssnDef_proj(v,(unsigned)i,0);}}}
# 655
return v;}
# 658
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){
LOOP: {
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==7){_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=_Tmp0.f1;{void*m=_Tmp4;void*a1=_Tmp3;void*v=_Tmp2;void*a2=_Tmp1;
# 662
if(a1==a2)return v;
if(Cyc_AssnDef_address_disjoint(a1,a2)){
t1=m;
goto LOOP;}{
# 671
struct _tuple12 _Tmp5=Cyc_AssnDef_split_addr(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*root=_Tmp7;struct Cyc_List_List*fields=_Tmp6;
if(a1==root)
return Cyc_AssnDef_proj_fields(v,fields);
goto _LL4;}}}}else{_LL4: {
# 676
struct Cyc_AssnDef_Select_AssnDef_Term_struct t=({struct Cyc_AssnDef_Select_AssnDef_Term_struct _Tmp5;_Tmp5.tag=6,_Tmp5.f1=t1,_Tmp5.f2=t2,_Tmp5.f3=tp;_Tmp5;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}}
# 688 "assndef.cyc"
void*Cyc_AssnDef_proj(void*t1,unsigned i,void*tp){
LOOP: {
void*_Tmp0;void*_Tmp1;void*_Tmp2;unsigned _Tmp3;int _Tmp4;switch(*((int*)t1)){case 8: _Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;{int is_union=_Tmp4;unsigned tag=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 692
int n=Cyc_List_length(ts);
if(!is_union && i < (unsigned)n)
return Cyc_List_nth(ts,(int)i);else{
if(tag==i && n >= 1)
return(void*)_check_null(ts)->hd;else{
goto _LL0;}}}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp3=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f3;{void*t=_Tmp2;unsigned j=_Tmp3;void*t2=_Tmp1;
# 699
if(i==j)return t2;{
# 702
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0)goto NOREDUCE;
{void*_Tmp5=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _Tmp6;switch(*((int*)_Tmp5)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==24){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp6;
# 706
{void*_Tmp7;if(ai.KnownAggr.tag==2){_Tmp7=*ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 708
if((int)ad->kind==1)goto NOREDUCE;goto _LL10;}}else{if(ai.UnknownAggr.val.f0==Cyc_Absyn_UnionA)
goto NOREDUCE;else{
goto _LL10;}}_LL10:;}
# 712
goto _LL9;}}else{goto _LLE;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp5)->f1==Cyc_Absyn_UnionA)
goto NOREDUCE;else{goto _LLE;}default: _LLE:
 goto _LL9;}_LL9:;}
# 716
t1=t;
goto LOOP;}}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f3;{void*m=_Tmp2;void*a=_Tmp1;void*tp2=_Tmp0;
void*_Tmp5=m;void*_Tmp6=Cyc_AssnDef_offsetf(a,i,tp2);return Cyc_AssnDef_select(_Tmp5,_Tmp6,tp);}default:
 goto _LL0;}_LL0:;}
# 721
NOREDUCE: {
struct Cyc_AssnDef_Proj_AssnDef_Term_struct t=({struct Cyc_AssnDef_Proj_AssnDef_Term_struct _Tmp0;_Tmp0.tag=9,_Tmp0.f1=t1,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 726
void*Cyc_AssnDef_aggr(int is_union,unsigned tag,struct Cyc_List_List*ts,void*tp){
# 729
struct Cyc_AssnDef_Aggr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=is_union,_Tmp0.f2=tag,_Tmp0.f3=ts,_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 732
void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,-1,ts,tp);}
# 735
void*Cyc_AssnDef_datatype_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,tag,ts,tp);}
# 738
void*Cyc_AssnDef_union_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(1,tag,ts,tp);}
# 741
void*Cyc_AssnDef_tagof_tm(void*t1){
{void*_Tmp0;unsigned _Tmp1;switch(*((int*)t1)){case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;{unsigned tag=_Tmp1;
return Cyc_AssnDef_uint(tag);}case 10: _Tmp0=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp1=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;{void*t=_Tmp0;unsigned j=_Tmp1;
# 746
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0)goto _LL0;
{void*_Tmp2=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _Tmp3;switch(*((int*)_Tmp2)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==24){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp3;
# 750
{void*_Tmp4;if(ai.KnownAggr.tag==2){_Tmp4=*ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 752
if((int)ad->kind==1)return Cyc_AssnDef_uint(j);
goto _LLE;}}else{if(ai.UnknownAggr.val.f0==Cyc_Absyn_UnionA)
return Cyc_AssnDef_uint(j);else{
goto _LLE;}}_LLE:;}
# 757
goto _LL7;}}else{goto _LLC;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f1==Cyc_Absyn_UnionA)
return Cyc_AssnDef_uint(j);else{goto _LLC;}default: _LLC:
 goto _LL7;}_LL7:;}
# 761
goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 764
struct Cyc_AssnDef_Tagof_AssnDef_Term_struct t=({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _Tmp0;_Tmp0.tag=15,_Tmp0.f1=t1;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 768
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 773
void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*e,void*nelts,void*invok,void*tp){
static int counter=0;
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct t=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp0;_Tmp0.tag=12,_Tmp0.f1=e,_Tmp0.f2=counter,_Tmp0.f3=nelts,_Tmp0.f4=invok,_Tmp0.f5=tp;_Tmp0;});
++ counter;
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 780
void*Cyc_AssnDef_offsetf(void*tm,unsigned i,void*tp){
struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct _Tmp0;_Tmp0.tag=13,_Tmp0.f1=tm,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 785
void*Cyc_AssnDef_offseti(void*tm,void*i,void*tp){
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=tm,_Tmp1.f1=i;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0.f1)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1==0)
return tm;else{if(*((int*)_Tmp0.f0)==14)goto _LL3;else{goto _LL5;}}}else{if(*((int*)_Tmp0.f0)==14){_LL3: _Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f3;{void*t=_Tmp3;void*j=_Tmp2;void*tp2=_Tmp1;
# 789
if((tp!=0 && tp2!=0)&& Cyc_Unify_unify(tp,tp2)){
void*_Tmp4=t;void*_Tmp5=Cyc_AssnDef_plus(i,j,Cyc_Absyn_sint_type);return Cyc_AssnDef_offseti(_Tmp4,_Tmp5,tp);}
goto _LL6;}}else{_LL5: _LL6: {
# 793
struct Cyc_AssnDef_Offseti_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offseti_AssnDef_Term_struct _Tmp4;_Tmp4.tag=14,_Tmp4.f1=tm,_Tmp4.f2=i,_Tmp4.f3=tp;_Tmp4;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}};}
# 797
void*Cyc_AssnDef_fresh_var(void*tp){
static int counter=0;
void*t=Cyc_AssnDef_logicvar(0,counter,tp);
++ counter;
return t;}
# 804
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 807
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 811
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 813
if((int)p==0)return t;
# 815
if((int)p==2){void*_Tmp0=t;void*_Tmp1=Cyc_AssnDef_uint(-1);return Cyc_AssnDef_binop(1U,_Tmp0,_Tmp1,tp);}{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 820
void*Cyc_AssnDef_numelts_term(void*t){
return Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);}static char _TmpG3[5U]="#Mem";
# 826
static struct _fat_ptr Cyc_AssnDef_memory_var={_TmpG3,_TmpG3,_TmpG3 + 5U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)& Cyc_AssnDef_void_con,0};
# 831
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0,0};
# 844
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=& Cyc_AssnDef_memory_struct;static char _TmpG4[5U]="#Exn";
# 846
static struct _fat_ptr Cyc_AssnDef_exception_var={_TmpG4,_TmpG4,_TmpG4 + 5U};
static struct _tuple0 Cyc_AssnDef_exnvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_exception_var};
# 849
struct Cyc_Absyn_Vardecl Cyc_AssnDef_exception_struct={Cyc_Absyn_Public,& Cyc_AssnDef_exnvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0,0};
# 863
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void){
struct Cyc_Absyn_Vardecl*vd=& Cyc_AssnDef_exception_struct;
({void*_Tmp0=Cyc_Absyn_exn_type();vd->type=_Tmp0;});
return vd;}struct _tuple17{struct Cyc_List_List*f0;int f1;};
# 880
struct _tuple17 Cyc_AssnDef_get_coefficient_term_list(void*t){
struct Cyc_List_List*coefficient_term_list=0;
int res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;unsigned _Tmp2;switch(*((int*)t)){case 0: _Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp2;
res=(int)i;goto _LL0;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned i=_Tmp2;
# 887
res=(int)i;
t=t1;
goto LOOP;}}else{_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 891
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t2)==4)switch((int)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f1){case Cyc_Absyn_Times: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 893
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_cycalloc(sizeof(struct _tuple16));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}}else{goto _LLE;}case Cyc_Absyn_Plus:
# 896
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Plus in the right branch, not a normal form",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
goto _LL9;default: goto _LLE;}else{_LLE:
# 899
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_cycalloc(sizeof(struct _tuple16));_Tmp7->f0=1,_Tmp7->f1=t2;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}_LL9:;}
# 902
t=t1;
goto LOOP;}}}else{goto _LL7;}default: _LL7:
# 905
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Times){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 907
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_cycalloc(sizeof(struct _tuple16));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 910
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_cycalloc(sizeof(struct _tuple16));_Tmp7->f0=1,_Tmp7->f1=t;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}_LL10:;}
# 913
goto _LL0;}_LL0:;}{
# 915
struct _tuple17 _Tmp0;_Tmp0.f0=coefficient_term_list,_Tmp0.f1=res;return _Tmp0;}}
# 925 "assndef.cyc"
static struct Cyc_List_List*Cyc_AssnDef_imp_merge_coefficient_term_list(int(*less_eq)(void*,void*),struct Cyc_List_List*termlist1,struct Cyc_List_List*termlist2){
# 929
struct Cyc_List_List*res_tail=0;
struct Cyc_List_List*res=0;
if(termlist1==0)return termlist2;
if(termlist2==0)return termlist1;{
int cmp_result=less_eq(((struct _tuple16*)termlist1->hd)->f1,((struct _tuple16*)termlist2->hd)->f1);
while(termlist1!=0 && termlist2!=0){
{int cmp_result=less_eq(((struct _tuple16*)termlist1->hd)->f1,((struct _tuple16*)termlist2->hd)->f1);
if(cmp_result < 0){
if(res_tail==0){
res_tail=termlist1;
termlist1=termlist1->tl;
res=res_tail;}else{
# 942
res_tail->tl=termlist1;
res_tail=termlist1;
termlist1=termlist1->tl;}}else{
# 946
if(cmp_result > 0){
if(res_tail==0){
res_tail=termlist2;
termlist2=termlist2->tl;
res=res_tail;}else{
# 952
res_tail->tl=termlist2;
res_tail=termlist2;
termlist2=termlist2->tl;}}else{
# 958
int i=((struct _tuple16*)termlist1->hd)->f0 + ((struct _tuple16*)termlist2->hd)->f0;
if(i!=0){
((struct _tuple16*)termlist1->hd)->f0=i;
if(res_tail==0){
res_tail=termlist1;
res=res_tail;}else{
# 965
res_tail->tl=termlist1;
res_tail=termlist1;}}
# 969
termlist1=termlist1->tl;
termlist2=termlist2->tl;}}}
# 935
1U;}
# 973
if(res_tail==0){
res_tail=termlist1==0?termlist2: termlist1;
res=res_tail;}else{
# 977
res_tail->tl=termlist1==0?termlist2: termlist1;}
# 979
return res;}}
# 985
inline static unsigned Cyc_AssnDef_id_of_primop(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return 0U;case Cyc_Absyn_Times:
 return 1U;case Cyc_Absyn_Bitand:
 return 4294967295U;case Cyc_Absyn_Bitor:
 return 0U;case Cyc_Absyn_Bitxor:
 return 0U;default:
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Absynpp_prim2string(p);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("no id for primop %s",sizeof(char),20U),_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 999
static void*Cyc_AssnDef_termlist2term(struct Cyc_List_List*termlist,int newc,enum Cyc_Absyn_Primop p,void*tp){
unsigned id=Cyc_AssnDef_id_of_primop(p);
if(termlist==0)return Cyc_AssnDef_uint((unsigned)newc);{
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=(void*)termlist->hd,_Tmp0.f4=tp;_Tmp0;});
t=Cyc_AssnDef_hash_cons_term((void*)& newterm);
termlist=termlist->tl;}
# 1005
1U;}
# 1009
if(id==(unsigned)newc)return t;else{
# 1023 "assndef.cyc"
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=p,_Tmp0.f2=t,({void*_Tmp1=Cyc_AssnDef_uint((unsigned)newc);_Tmp0.f3=_Tmp1;}),_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& newterm);}}}
# 1030
static void*Cyc_AssnDef_compute_factor(void*tp,struct _tuple16*coefficient_term){
void*_Tmp0;int _Tmp1;_Tmp1=coefficient_term->f0;_Tmp0=coefficient_term->f1;{int c=_Tmp1;void*t=_Tmp0;
if(c==1)return t;else{
# 1034
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newfactor=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=1U,_Tmp2.f2=t,({void*_Tmp3=Cyc_AssnDef_uint((unsigned)c);_Tmp2.f3=_Tmp3;}),_Tmp2.f4=tp;_Tmp2;});
return Cyc_AssnDef_hash_cons_term((void*)& newfactor);}}}
# 1039
static int Cyc_AssnDef_inv_termcmp(void*t1,void*t2){
return - Cyc_AssnDef_termcmp(t1,t2);}
# 1050 "assndef.cyc"
static void*Cyc_AssnDef_normalize_plus(void*t1,void*t2,void*tp){
struct _tuple17 _Tmp0=Cyc_AssnDef_get_coefficient_term_list(t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*coefficient_term_list1=_Tmp2;int i1=_Tmp1;
struct _tuple17 _Tmp3=Cyc_AssnDef_get_coefficient_term_list(t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*coefficient_term_list2=_Tmp5;int i2=_Tmp4;
struct Cyc_List_List*coefficient_term_list=
Cyc_AssnDef_imp_merge_coefficient_term_list(Cyc_AssnDef_inv_termcmp,coefficient_term_list1,coefficient_term_list2);
# 1058
struct Cyc_List_List*term_list=({struct Cyc_List_List*(*_Tmp6)(void*(*)(void*,struct _tuple16*),void*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(void*,struct _tuple16*),void*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_AssnDef_compute_factor,tp,coefficient_term_list);
void*res=Cyc_AssnDef_termlist2term(term_list,i1 + i2,0U,tp);
return res;}}}
# 1068
static struct _tuple17 Cyc_AssnDef_term2termlist(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
int res=(int)Cyc_AssnDef_id_of_primop(p);
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=(int)i;goto _LL0;}case 4: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 1075
res=(int)i;
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 1079
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 1083
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}{
# 1086
struct _tuple17 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;return _Tmp0;}}
# 1090
inline static unsigned Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return i + j;case Cyc_Absyn_Times:
 return i * j;case Cyc_Absyn_Bitand:
 return i & j;case Cyc_Absyn_Bitor:
 return i | j;case Cyc_Absyn_Bitxor:
 return i ^ j;default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 1102
static void*Cyc_AssnDef_normalize_assoc_commu(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
struct _tuple17 _Tmp0=Cyc_AssnDef_term2termlist(p,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*term_list1=_Tmp2;int c1=_Tmp1;
struct _tuple17 _Tmp3=Cyc_AssnDef_term2termlist(p,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*term_list2=_Tmp5;int c2=_Tmp4;
struct Cyc_List_List*term_list=Cyc_List_imp_merge(Cyc_AssnDef_inv_termcmp,term_list1,term_list2);
unsigned c=Cyc_AssnDef_eval_binop(p,(unsigned)c1,(unsigned)c2);
return Cyc_AssnDef_termlist2term(term_list,(int)c,p,tp);}}}
# 1110
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1114
 if(tp!=0){
void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==2)
return 1;else{goto _LLE;}}else{_LLE:
 return 0;};}
# 1120
return 0;case Cyc_Absyn_Eq:
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 return 1;default:
 return 0;};}
# 1127
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1131
 if(tp!=0){
void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==1)
# 1134
return 1;else{goto _LL10;}}else{_LL10:
 return 0;};}
# 1138
return 0;case Cyc_Absyn_Bitand:
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}struct _tuple18{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 1147
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
{struct _tuple18 _Tmp0=({struct _tuple18 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 1151
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL7;}_LL7:;
# 1161
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL3;else{goto _LL5;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){_LL3: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 1163
if(tp!=0){if(*((int*)tp)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)tp)->f1)==1){
# 1166
if(Cyc_AssnDef_termcmp(t1,t2)==0)return Cyc_AssnDef_uint(0U);
# 1168
t2=({void*_Tmp5=t2;void*_Tmp6=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp5,_Tmp6,tp);});
return Cyc_AssnDef_binop(0U,t1,t2,tp);}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B:
 goto _LL18;}_LL18:;
# 1172
goto _LL0;}}else{_LL5:
# 1177
 if(Cyc_AssnDef_is_associative_commutative(p,tp)){
# 1182
if((int)p==0)return Cyc_AssnDef_normalize_plus(t1,t2,tp);else{
return Cyc_AssnDef_normalize_assoc_commu(p,t1,t2,tp);}}
# 1185
if(Cyc_AssnDef_is_nonassociative_commutative(p,tp)){
if(Cyc_AssnDef_termcmp(t1,t2)> 0){
void*tmpt=t1;
t1=t2;
t2=tmpt;}}else{
# 1191
if((int)p==0){
struct _tuple15 _Tmp5=({struct _tuple15 _Tmp6;_Tmp6.f0=t1,_Tmp6.f1=t2;_Tmp6;});unsigned _Tmp6;void*_Tmp7;unsigned _Tmp8;void*_Tmp9;if(*((int*)_Tmp5.f0)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f3)==0){if(*((int*)_Tmp5.f1)==0){_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f2;_Tmp8=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f3)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f4;_Tmp6=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp5.f1)->f1;{void*t1=_Tmp9;unsigned i=_Tmp8;void*tp2=_Tmp7;unsigned j=_Tmp6;
# 1194
if((tp!=0 && tp2!=0)&& Cyc_Unify_unify(tp,tp2)){
void*_TmpA=t1;void*_TmpB=Cyc_AssnDef_uint(i + j);return Cyc_AssnDef_plus(_TmpA,_TmpB,tp);}
goto _LL1D;}}else{goto _LL20;}}else{goto _LL20;}}else{goto _LL20;}}else{_LL20:
 goto _LL1D;}_LL1D:;}}
# 1200
goto _LL0;}}_LL0:;}{
# 1202
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 1206
void*Cyc_AssnDef_get_term_type(void*t){
LOOP: {
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;switch(*((int*)t)){case 0: _Tmp2=(void*)((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp2;
return tp;}case 1: _Tmp2=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
return e->topt;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp2;
return topt;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*tp=_Tmp2;
return tp;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 8: _Tmp2=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
return tp;}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp2;
t=t1;goto LOOP;}case 11: _Tmp2=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*tp=_Tmp0;
return tp;}case 7:
 return 0;case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f5;{void*tp=_Tmp2;
return tp;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 14: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}default:
 return Cyc_Absyn_uint_type;};}}
# 1228
static void*Cyc_AssnDef_subst_term(struct Cyc_Dict_Dict dict,void*t,struct Cyc_Hashtable_Table*term_h){
# 1230
void**t1=Cyc_Hashtable_lookup_opt(term_h,t);
if(t1!=0)
# 1236
return*t1;{
# 1238
void*res;
{unsigned _Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
# 1242
 res=t;goto _LL0;case 11: _Tmp6=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;void*t1=_Tmp5;void*tp=_Tmp4;
# 1244
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
struct Cyc_AssnDef_Addr_AssnDef_Term_struct addr_term=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp7;_Tmp7.tag=11,_Tmp7.f1=vd,_Tmp7.f2=s1,_Tmp7.f3=tp;_Tmp7;});
res=Cyc_AssnDef_hash_cons_term((void*)& addr_term);
goto _LL0;}case 12: _Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f5;{struct Cyc_Absyn_Exp*e1=_Tmp6;int i=_Tmp3;void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp2;
# 1249
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct alloc_term=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=e1,_Tmp7.f2=i,_Tmp7.f3=s1,_Tmp7.f4=s2,_Tmp7.f5=tp;_Tmp7;});
res=Cyc_AssnDef_hash_cons_term((void*)& alloc_term);
goto _LL0;}case 6: _Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1255
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_select(s1,s2,tp);
goto _LL0;}case 7: _Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*t3=_Tmp4;
# 1260
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
void*s3=Cyc_AssnDef_subst_term(dict,t3,term_h);
res=Cyc_AssnDef_update(s1,s2,s3);
goto _LL0;}case 2:  {
# 1266
void**t1=Cyc_Dict_lookup_opt(dict,t);
if(t1!=0)res=*t1;else{
res=t;}
goto _LL0;}case 3: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*tp=_Tmp5;
# 1271
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_unop(p,s1,tp);
goto _LL0;}case 4: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1275
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_binop(p,s1,s2,tp);
goto _LL0;}case 5: _Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp6;void*t1=_Tmp5;
# 1280
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_cast(tp,s1);
goto _LL0;}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp6=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int b=_Tmp3;unsigned tag=_Tmp0;struct Cyc_List_List*tlist=_Tmp6;void*tp=_Tmp5;
# 1284
struct Cyc_List_List*slist=0;
for(1;tlist!=0;tlist=tlist->tl){
slist=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp8=Cyc_AssnDef_subst_term(dict,(void*)tlist->hd,term_h);_Tmp7->hd=_Tmp8;}),_Tmp7->tl=slist;_Tmp7;});}
# 1288
Cyc_List_imp_rev(slist);
res=Cyc_AssnDef_aggr(b,tag,slist,tp);
goto _LL0;}case 9: _Tmp6=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 1292
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_proj(s1,i,tp);
goto _LL0;}case 10: _Tmp6=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*t2=_Tmp5;
# 1296
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_aggr_update(s1,i,s2);
goto _LL0;}case 13: _Tmp6=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 1301
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_offsetf(s1,i,tp);
goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1305
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_offseti(s1,s2,tp);
goto _LL0;}default: _Tmp6=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp6;
# 1310
res=Cyc_AssnDef_tagof_tm(Cyc_AssnDef_subst_term(dict,t1,term_h));
goto _LL0;}}_LL0:;}
# 1313
Cyc_Hashtable_insert(term_h,t,res);
return res;}}
# 1317
static int Cyc_AssnDef_hash_ptr(void*s){return(int)s;}
# 1320
void*Cyc_AssnDef_subst_t(struct Cyc_Dict_Dict dict,void*t){
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_term(dict,t,term_h);}struct _tuple19{struct _fat_ptr f0;unsigned f1;};
# 1327
static struct _tuple19 Cyc_AssnDef_a2dag(void*a,int*ctr,struct Cyc_Hashtable_Table*t,struct Cyc_List_List**decls){
# 1331
struct _tuple19**resopt=({struct _tuple19**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple19**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(t,a);
if(resopt!=0)
return*(*resopt);{
# 1335
int c=*ctr;
*ctr=c + 1;{
struct _fat_ptr name=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)c;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("A%d",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
struct _fat_ptr s;
unsigned size=1U;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1344
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 1354
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=ps;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(t2);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1357
struct _tuple19 _Tmp3=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _Tmp4;struct _fat_ptr _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _fat_ptr s1=_Tmp5;unsigned n1=_Tmp4;
struct _tuple19 _Tmp6=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct _fat_ptr s2=_Tmp8;unsigned n2=_Tmp7;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=s1;_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s2;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_TmpB,sizeof(void*),2));});
size +=n1 + n2;
goto _LL0;}}}default: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1363
struct _tuple19 _Tmp3=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _Tmp4;struct _fat_ptr _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _fat_ptr s1=_Tmp5;unsigned n1=_Tmp4;
struct _tuple19 _Tmp6=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct _fat_ptr s2=_Tmp8;unsigned n2=_Tmp7;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=s1;_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s2;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_TmpB,sizeof(void*),2));});
size +=n1 + n2;
goto _LL0;}}}}_LL0:;}
# 1369
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=name;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("%s = %s\n",sizeof(char),9U),_tag_fat(_Tmp7,sizeof(void*),2));});*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*decls;_Tmp1;});*decls=_Tmp0;});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple19*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=t;void*_Tmp2=a;_Tmp0(_Tmp1,_Tmp2,({struct _tuple19*_Tmp3=_cycalloc(sizeof(struct _tuple19));_Tmp3->f0=name,_Tmp3->f1=size;_Tmp3;}));});{
struct _tuple19 _Tmp0;_Tmp0.f0=name,_Tmp0.f1=size;return _Tmp0;}}}}
# 1374
int Cyc_AssnDef_assnhash(void*);
# 1376
struct _fat_ptr Cyc_AssnDef_assn2dag(void*a){
struct Cyc_List_List*decls=0;
struct Cyc_Hashtable_Table*t=
Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
int ctr=0;
struct _tuple19 _Tmp0=Cyc_AssnDef_a2dag(a,& ctr,t,& decls);unsigned _Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct _fat_ptr s=_Tmp2;unsigned n=_Tmp1;
decls=Cyc_List_imp_rev(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=s;_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)((int)n);_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_aprintf(_tag_fat("in %s (%d nodes as tree)\n",sizeof(char),26U),_tag_fat(_Tmp9,sizeof(void*),2));});*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=decls;_Tmp3;}));
return Cyc_strconcat_l(decls);}}
# 1389
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int inprec){
int myprec=10;
struct _fat_ptr s;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1396
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 1406
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=ps;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(t2);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1409
myprec=5;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_a2string(a1,myprec);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a2,myprec);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1413
myprec=10;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_a2string(a1,10);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a2,10);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL0;}}_LL0:;}
# 1417
if(myprec < inprec)
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("(%s)",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));});
return s;}
# 1423
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
return Cyc_AssnDef_a2string(a,10);}
# 1435 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1440
int _Tmp3=({int _Tmp4=Cyc_AssnDef_termhash(t1);_Tmp4 + (int)p;});return _Tmp3 + Cyc_AssnDef_termhash(t2);}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1442
return(int)(((unsigned)a1 + (unsigned)a2 >> 4U)+ 3U);}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1444
return(int)(((unsigned)a2 + (unsigned)a1 >> 4U)+ 5U);}};}
# 1450
int Cyc_AssnDef_assncmp(void*_a1,void*_a2){
void*a1=_a1;
void*a2=_a2;
LOOP:
 if(a1==a2)return 0;{
int c=(int)(*((const unsigned*)a2)- *((const unsigned*)a1));
if(c!=0)return c;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});enum Cyc_AssnDef_Primreln _Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==3){_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
_Tmp6=a11;_Tmp5=a12;_Tmp4=a21;_Tmp3=a22;goto _LL4;}}else{goto _LL7;}case 4: if(*((int*)_Tmp0.f1)==4){_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL4: {void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
# 1460
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}}else{goto _LL7;}case 2: if(*((int*)_Tmp0.f1)==2){_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp6;enum Cyc_AssnDef_Primreln p1=_Tmp2;void*t12=_Tmp5;void*t21=_Tmp4;enum Cyc_AssnDef_Primreln p2=_Tmp1;void*t22=_Tmp3;
# 1466
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_termcmp(t12,t22);}}else{goto _LL7;}default: _LL7:
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("assncmp:  tagof failure",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}}}
# 1478
static void*Cyc_AssnDef_copy_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
# 1481
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("True assertion should be in the table",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));case 1:
# 1483
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("False assertion should be in the table",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=p,_Tmp3->f3=t2;_Tmp3;});}};}
# 1492
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 1500
static void*Cyc_AssnDef_neg_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1507
switch((int)p){case Cyc_AssnDef_Eq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=1U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_Neq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=0U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_SLt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=3U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_SLte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=2U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=5U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=4U,_Tmp3->f3=t1;_Tmp3;});default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("undefined prim relation",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}};}struct Cyc_AssnDef_AssnHashedInfo{void*assn;void*negation;struct Cyc_Set_Set**widened_set;struct Cyc_Set_Set**free_logicvar_set;};
# 1530
static struct Cyc_Hashtable_Table*Cyc_AssnDef_assn_hash_cons_table;
# 1532
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_assn_hash_cons_table (void){
# 1534
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_assn_hash_cons_table==0){
h=Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_assn_hash_cons_table=h;{
struct Cyc_Set_Set*ast=Cyc_Set_empty(Cyc_AssnDef_assncmp);
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_true_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_true_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=ast;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_false_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_false_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=0;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});}}else{
# 1543
h=Cyc_AssnDef_assn_hash_cons_table;}
# 1545
return h;}
# 1547
static void*Cyc_AssnDef_hash_cons_assn(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**resopt=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
# 1553
void*a2=Cyc_AssnDef_copy_assn(a);
void*nega2=Cyc_AssnDef_neg_assn(a);
# 1556
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=a2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=a2,_Tmp3->negation=nega2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=nega2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=nega2,_Tmp3->negation=a2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
# 1560
return a2;}else{
# 1562
return(*resopt)->assn;}}
# 1565
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 1575
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1580
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
# 1583
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 1591
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1605 "assndef.cyc"
static void*Cyc_AssnDef_reinsert(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1611
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1618
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}default:
# 1625
 return Cyc_AssnDef_hash_cons_assn(a);};}
# 1631
struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1638
if((*_check_null(ahinfo))->free_logicvar_set!=0)
return*(*ahinfo)->free_logicvar_set;{
struct Cyc_Set_Set*res;
{void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1644
 res=Cyc_AssnDef_empty_term_set();
goto _LL0;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1647
_Tmp1=a1;_Tmp0=a2;goto _LL8;}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LL8: {void*a1=_Tmp1;void*a2=_Tmp0;
# 1649
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_assn_fr_logicvar(a1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_assn_fr_logicvar(a2));});
goto _LL0;}default: _Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 1652
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_term_fr_logicvar(t2));});
goto _LL0;}}_LL0:;}
# 1655
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=res;_Tmp1;});(*ahinfo)->free_logicvar_set=_Tmp0;});
return res;}}
# 1660
static int Cyc_AssnDef_logicvar_in_assn(void*lvar,void*a){
struct Cyc_Set_Set*lvset=Cyc_AssnDef_assn_fr_logicvar(a);
return Cyc_Set_member(lvset,lvar);}
# 1667
void*Cyc_AssnDef_not(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1674
return(*_check_null(ahinfo))->negation;}
# 1677
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons_assn((void*)& ptr);}
# 1682
void*Cyc_AssnDef_eq(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;
if(Cyc_AssnDef_term_neq(t1,t2))return(void*)& Cyc_AssnDef_false_assn;
if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1688
return Cyc_AssnDef_prim(t1,0U,t2);}
# 1691
void*Cyc_AssnDef_slt(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1695
if((int)i < (int)j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,2U,t2);};}}
# 1700
void*Cyc_AssnDef_slte(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1704
if((int)i <= (int)j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,3U,t2);};}}
# 1709
void*Cyc_AssnDef_ult(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1713
if(i < j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,4U,t2);};}}
# 1718
void*Cyc_AssnDef_ulte(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1722
if(i <= j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,5U,t2);};}}
# 1727
void*Cyc_AssnDef_neq(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;
if(Cyc_AssnDef_term_neq(t1,t2))return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;if(*((int*)_Tmp0.f0)==0){_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;if((unsigned)_Tmp1==0U){unsigned i=_Tmp1;
# 1732
void*_Tmp2=Cyc_AssnDef_zero();return Cyc_AssnDef_ult(_Tmp2,t2);}else{if(*((int*)_Tmp0.f1)==0)goto _LL3;else{goto _LL5;}}}else{if(*((int*)_Tmp0.f1)==0){_LL3: _Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;if((unsigned)_Tmp1==0U){unsigned j=_Tmp1;
# 1734
void*_Tmp2=Cyc_AssnDef_zero();return Cyc_AssnDef_ult(_Tmp2,t1);}else{goto _LL5;}}else{_LL5:
# 1736
 if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp2=t1;void*_Tmp3=t2;t1=_Tmp3;t2=_Tmp2;});
# 1739
return Cyc_AssnDef_prim(t1,1U,t2);}};}}
# 1743
struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1750
if((*_check_null(ahinfo))->widened_set!=0)return*(*ahinfo)->widened_set;{
struct Cyc_Set_Set*s;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1755
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("true and false should have widened_set",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1757
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0 || s2==0)
s=0;else{
# 1762
s=Cyc_Set_union_two(s1,s2);}
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1765
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)
s=s2;else{
if(s2==0)
s=s1;else{
# 1772
s=Cyc_Set_intersect(s1,s2);}}
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1775
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,a);
switch((int)p){case Cyc_AssnDef_Eq:  {
# 1781
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 1790
goto _LLB;}case Cyc_AssnDef_SLt:
# 1792
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
goto _LLB;case Cyc_AssnDef_ULt:
# 1795
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
goto _LLB;default:
 goto _LLB;}_LLB:;
# 1799
goto _LL0;}}_LL0:;}
# 1801
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=s;_Tmp1;});(*ahinfo)->widened_set=_Tmp0;});
return s;}}
# 1806
void*Cyc_AssnDef_widen(void*a){
# 1808
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);
return res;}}
# 1814
static void*Cyc_AssnDef_subst_assn(struct Cyc_Dict_Dict dict,void*a,struct Cyc_Hashtable_Table*assn_h,struct Cyc_Hashtable_Table*term_h){
# 1817
void**a1=Cyc_Hashtable_lookup_opt(assn_h,a);
if(a1!=0)
# 1823
return*a1;{
# 1825
void*res;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 res=a;goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1830
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_prim(s1,p,s2);
goto _LL0;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1835
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_and(b1,b2);
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1840
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_or(b1,b2);
goto _LL0;}}_LL0:;}
# 1845
Cyc_Hashtable_insert(assn_h,a,res);
return res;}}
# 1849
void*Cyc_AssnDef_subst_a(struct Cyc_Dict_Dict dict,void*a){
# 1852
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_assn(dict,a,assn_h,term_h);}
# 1869 "assndef.cyc"
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){
while(1){
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1873
if(Cyc_Set_member(ctxt,a))return 1;
# 1880
switch((int)p){case Cyc_AssnDef_Neq:
# 1882
 return((({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t2,t1));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t2,t1));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));});case Cyc_AssnDef_SLte:
# 1887
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});case Cyc_AssnDef_ULte:
# 1890
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});default:
 return 0;};}case 0:
# 1894
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1897
if(!Cyc_AssnDef_simple_prv(ctxt,a1))
return 0;
# 1903
a=a2;
continue;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1906
if(Cyc_AssnDef_simple_prv(ctxt,a1))return 1;
a=a2;
continue;}};}
# 1871
1U;}}
# 1916
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
if(ctxt==a)return 1;{
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt==0)return 1;
return Cyc_AssnDef_simple_prv(sopt,a);}}
# 1923
struct Cyc_Set_Set*Cyc_AssnDef_assnfn_fr_logicvar(struct Cyc_AssnDef_AssnFn*af){
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assn_fr_logicvar(af->assn);
{struct Cyc_List_List*as=af->actuals;for(0;as!=0;as=as->tl){
fvs=Cyc_Set_delete(fvs,(void*)as->hd);}}
return fvs;}
# 1931
struct Cyc_AssnDef_AssnFn*Cyc_AssnDef_apply_term_assnfn(struct Cyc_AssnDef_AssnFn*af,void*t){
void*assn=af->assn;
struct Cyc_List_List*actuals=af->actuals;
struct Cyc_AssnDef_AssnFn*newaf;newaf=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn)),newaf->actuals=_check_null(actuals)->tl,newaf->assn=assn;
# 1936
if(!({struct Cyc_Set_Set*_Tmp0=Cyc_AssnDef_assnfn_fr_logicvar(newaf);Cyc_Set_member(_Tmp0,(void*)actuals->hd);}))return newaf;{
# 1938
struct Cyc_List_List*newactuals=0;
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
# 1941
dict=Cyc_Dict_insert(dict,(void*)actuals->hd,t);
actuals=actuals->tl;{
# 1945
struct Cyc_Set_Set*fvs=Cyc_AssnDef_term_fr_logicvar(t);
for(1;actuals!=0;actuals=actuals->tl){
if(Cyc_Set_member(fvs,(void*)actuals->hd)){
void*v=Cyc_AssnDef_fresh_var(Cyc_AssnDef_get_term_type((void*)actuals->hd));
dict=Cyc_Dict_insert(dict,(void*)actuals->hd,v);
newactuals=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=v,_Tmp0->tl=newactuals;_Tmp0;});}else{
# 1952
newactuals=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)actuals->hd,_Tmp0->tl=newactuals;_Tmp0;});}}{
# 1955
void*newassn=Cyc_AssnDef_subst_a(dict,assn);
struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(newactuals);struct Cyc_List_List*newactuals=_Tmp0;
struct Cyc_AssnDef_AssnFn*_Tmp1=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_Tmp1->actuals=newactuals,_Tmp1->assn=newassn;return _Tmp1;}}}}
# 1961
static struct Cyc_AssnDef_AssnFn*Cyc_AssnDef_apply_terms_assnfn(struct Cyc_AssnDef_AssnFn*af,struct Cyc_List_List*ts){
for(1;ts!=0;ts=ts->tl){
af=Cyc_AssnDef_apply_term_assnfn(af,(void*)ts->hd);}
return af;}
# 1967
static struct _fat_ptr Cyc_AssnDef_assnfn2string(struct Cyc_AssnDef_AssnFn*af){
if(af->actuals==0)return Cyc_AssnDef_assn2string(af->assn);{
struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string((void*)af->actuals->hd);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_AssnDef_assnfn2string(({struct Cyc_AssnDef_AssnFn*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_Tmp4->actuals=af->actuals->tl,_Tmp4->assn=af->assn;_Tmp4;}));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_aprintf(_tag_fat("/\\ %s .\n%s",sizeof(char),11U),_tag_fat(_Tmp2,sizeof(void*),2));}}
# 1971
struct _fat_ptr Cyc_AssnDef_existassnfn2string(struct Cyc_AssnDef_ExistAssnFn*eaf){
return Cyc_AssnDef_assnfn2string(eaf->af);}
# 1977
struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_assnmap2existassnfn(struct Cyc_AssnDef_AssnMap am,struct Cyc_List_List*vds){
struct Cyc_List_List*actuals=0;
for(1;vds!=0;vds=vds->tl){
if((struct Cyc_Absyn_Vardecl*)vds->hd!=0){
struct _tuple13 _Tmp0=Cyc_AssnDef_lookup_var_map((struct Cyc_Absyn_Vardecl*)vds->hd,am);void*_Tmp1;_Tmp1=_Tmp0.f0;{void*v=_Tmp1;
if(*((int*)v)==2)
goto _LL3;else{
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("term to be bound is not a logic var",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));}_LL3:;
# 1986
actuals=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=v,_Tmp2->tl=actuals;_Tmp2;});}}else{
# 1988
actuals=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1=Cyc_AssnDef_fresh_var((struct Cyc_Absyn_Vardecl*)vds->hd==0?0:((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=actuals;_Tmp0;});}}
# 1991
actuals=Cyc_List_imp_rev(actuals);{
# 1993
struct Cyc_AssnDef_AssnFn*af;af=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn)),af->actuals=actuals,af->assn=am.assn;{
# 1995
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assnfn_fr_logicvar(af);
struct Cyc_AssnDef_ExistAssnFn*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_ExistAssnFn));_Tmp0->af=af,_Tmp0->existvars=fvs;return _Tmp0;}}}
# 2003
struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_bound_ts_in_assn(void*a,struct Cyc_List_List*ts){
# 2005
struct Cyc_List_List*vs=0;
for(1;ts!=0;ts=ts->tl){
void*_Tmp0=(void*)ts->hd;if(*((int*)_Tmp0)==2){
# 2011
int occured=0;
{struct Cyc_List_List*xs=vs;for(0;xs!=0;xs=xs->tl){
if(Cyc_AssnDef_termcmp((void*)ts->hd,(void*)xs->hd)==0){
occured=1;
break;}}}
# 2018
if(!occured)vs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)ts->hd,_Tmp1->tl=vs;_Tmp1;});else{
# 2020
void*v=Cyc_AssnDef_fresh_var(Cyc_AssnDef_get_term_type((void*)ts->hd));
a=({void*_Tmp1=a;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,(void*)ts->hd));});
vs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=vs;_Tmp1;});}
# 2024
goto _LL0;}else{
# 2026
void*v=Cyc_AssnDef_fresh_var(Cyc_AssnDef_get_term_type((void*)ts->hd));
a=({void*_Tmp1=a;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,(void*)ts->hd));});
vs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=vs;_Tmp1;});
goto _LL0;}_LL0:;}
# 2032
vs=Cyc_List_imp_rev(vs);{
# 2034
struct Cyc_AssnDef_AssnFn*af;af=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn)),af->actuals=vs,af->assn=a;{
# 2036
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assnfn_fr_logicvar(af);
struct Cyc_AssnDef_ExistAssnFn*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_ExistAssnFn));_Tmp0->af=af,_Tmp0->existvars=fvs;return _Tmp0;}}}
# 2040
static struct Cyc_Dict_Dict*Cyc_AssnDef_insertnewmap(void*t,struct Cyc_Dict_Dict*dict){
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict _Tmp1=*dict;void*_Tmp2=t;Cyc_Dict_insert(_Tmp1,_Tmp2,Cyc_AssnDef_fresh_var(Cyc_AssnDef_get_term_type(t)));});*dict=_Tmp0;});
return dict;}
# 2047
static struct Cyc_AssnDef_AssnFn*Cyc_AssnDef_remove_exist_existassnfn(struct Cyc_AssnDef_ExistAssnFn*eaf){
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
dict=*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(void*,struct Cyc_Dict_Dict*),struct Cyc_Set_Set*,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(void*,struct Cyc_Dict_Dict*),struct Cyc_Set_Set*,struct Cyc_Dict_Dict*))Cyc_Set_fold;_Tmp0;})(Cyc_AssnDef_insertnewmap,eaf->existvars,& dict);{
struct Cyc_AssnDef_AssnFn*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_AssnFn));_Tmp0->actuals=eaf->af->actuals,({void*_Tmp1=Cyc_AssnDef_subst_a(dict,eaf->af->assn);_Tmp0->assn=_Tmp1;});return _Tmp0;}}
# 2053
void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*eaf,struct Cyc_List_List*ts){
struct Cyc_AssnDef_AssnFn*af=Cyc_AssnDef_remove_exist_existassnfn(eaf);
af=Cyc_AssnDef_apply_terms_assnfn(af,ts);
if(af->actuals==0)return af->assn;
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("not enough ts to be applied to assnfn",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));}
# 2061
static struct Cyc_List_List*Cyc_AssnDef_m2string(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_List_List*stringlist){
struct _fat_ptr res=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Absynpp_qvar2string(vd->name);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_AssnDef_term2string(value);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s == %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=res;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=stringlist;return _Tmp0;}
# 2066
struct _fat_ptr Cyc_AssnDef_map2string(struct Cyc_Dict_Dict m){
struct Cyc_List_List*stringlist=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_m2string,m,0);
struct _fat_ptr s;
if(stringlist==0)return Cyc_aprintf(_tag_fat(" ",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*((struct _fat_ptr*)stringlist->hd);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
while(stringlist->tl!=0){
stringlist=stringlist->tl;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*((struct _fat_ptr*)stringlist->hd);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
# 2072
1U;}
# 2075
return s;}
# 2079
struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap am){
struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_map2string(am.map);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_AssnDef_assn2string(am.assn);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_aprintf(_tag_fat("{%s}&&\n%s",sizeof(char),10U),_tag_fat(_Tmp2,sizeof(void*),2));}
# 2083
struct _fat_ptr Cyc_AssnDef_assnmap2dag(struct Cyc_AssnDef_AssnMap am){
struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_map2string(am.map);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_AssnDef_assn2dag(am.assn);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_aprintf(_tag_fat("{%s}\n%s",sizeof(char),8U),_tag_fat(_Tmp2,sizeof(void*),2));}
# 2087
static struct Cyc_Dict_Dict Cyc_AssnDef_empty_map (void){
static struct Cyc_Dict_Dict*mt=0;
if(mt==0){
struct Cyc_Dict_Dict d=({struct Cyc_Dict_Dict(*_Tmp0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;_Tmp0;})(({int(*_Tmp0)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;_Tmp0;}));
mt=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp0=d;_Tmp0;});
return d;}else{
# 2094
return*mt;}}
# 2097
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void){
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});return _Tmp0;}
# 2101
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void){
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});return _Tmp0;}
# 2108
struct _tuple13 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*vd,struct Cyc_AssnDef_AssnMap am){
void**t=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,vd);
if(t!=0){struct _tuple13 _Tmp0;_Tmp0.f0=*t,_Tmp0.f1=am;return _Tmp0;}{
void*newt=Cyc_AssnDef_fresh_var(vd->type);
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
struct _tuple13 _Tmp0;_Tmp0.f0=newt,_Tmp0.f1.assn=am.assn,_Tmp0.f1.map=newm;return _Tmp0;}}
# 2118
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*vd,void*newt,struct Cyc_AssnDef_AssnMap am){
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=am.assn,_Tmp0.map=newm;return _Tmp0;}
# 2124
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap am){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_widen(am.assn);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 2129
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_or(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 2134
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_and(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 2138
void*Cyc_AssnDef_merge(struct Cyc_Absyn_Vardecl*vd,void*t1,void*t2){
if(t1==t2)return t1;else{
return Cyc_AssnDef_fresh_var(vd->type);}}struct _tuple20{struct Cyc_Hashtable_Table*f0;struct Cyc_Dict_Dict f1;};
# 2143
static void*Cyc_AssnDef_subst_term_internal(struct _tuple20*env,void*t){
return Cyc_AssnDef_subst_term(env->f1,t,env->f0);}
# 2147
static struct Cyc_AssnDef_AssnMap Cyc_AssnDef_subst_am(struct Cyc_Dict_Dict dict,struct Cyc_AssnDef_AssnMap am){
# 2150
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
void*assn=Cyc_AssnDef_subst_assn(dict,am.assn,assn_h,term_h);
struct Cyc_Dict_Dict map=({struct Cyc_Dict_Dict(*_Tmp0)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict)=({struct Cyc_Dict_Dict(*_Tmp1)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict))Cyc_Dict_map_c;_Tmp1;});struct _tuple20*_Tmp1=({struct _tuple20*_Tmp2=_cycalloc(sizeof(struct _tuple20));_Tmp2->f0=term_h,_Tmp2->f1=dict;_Tmp2;});_Tmp0(Cyc_AssnDef_subst_term_internal,_Tmp1,am.map);});
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=assn,_Tmp0.map=map;return _Tmp0;}struct Cyc_AssnDef_Accum{void*a1;void*a2;struct Cyc_Dict_Dict m1;struct Cyc_Dict_Dict m2;struct Cyc_Dict_Dict m;};
# 2164
static int Cyc_AssnDef_logicvar_in_map_internal(void*lvar,struct Cyc_Absyn_Vardecl*dummy,void*value,int in_map){
return in_map || Cyc_AssnDef_logicvar_in_term(lvar,value);}
# 2168
static int Cyc_AssnDef_logicvar_in_map(void*lv,struct Cyc_Dict_Dict m){
return({int(*_Tmp0)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int)=(int(*)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int))Cyc_Dict_fold_c;_Tmp0;})(Cyc_AssnDef_logicvar_in_map_internal,lv,m,0);}
# 2178
static void*Cyc_AssnDef_unify_term(struct Cyc_AssnDef_Accum*accum,void*ta,void*tb){
if(ta==tb)return ta;
{struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=ta,_Tmp1.f1=tb;_Tmp1;});unsigned _Tmp1;int _Tmp2;unsigned _Tmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;enum Cyc_Absyn_Primop _Tmp9;void*_TmpA;void*_TmpB;enum Cyc_Absyn_Primop _TmpC;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==3){_TmpC=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp8=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((int)((enum Cyc_Absyn_Primop)_TmpC)==(int)((enum Cyc_Absyn_Primop)_Tmp9)){enum Cyc_Absyn_Primop pa=_TmpC;void*ta1=_TmpB;void*topta=_TmpA;enum Cyc_Absyn_Primop pb=_Tmp9;void*tb1=_Tmp8;void*toptb=_Tmp7;
# 2182
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);
return Cyc_AssnDef_unop(pa,t1,topta);}else{goto _LL19;}}else{goto _LL19;}case 4: if(*((int*)_Tmp0.f1)==4){_TmpC=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp9=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;if((int)((enum Cyc_Absyn_Primop)_TmpC)==(int)((enum Cyc_Absyn_Primop)_Tmp9)){enum Cyc_Absyn_Primop pa=_TmpC;void*ta1=_TmpB;void*ta2=_TmpA;void*topta=_Tmp8;enum Cyc_Absyn_Primop pb=_Tmp9;void*tb1=_Tmp7;void*tb2=_Tmp6;void*toptb=_Tmp5;
# 2185
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);
void*t2=Cyc_AssnDef_unify_term(accum,ta2,tb2);
return Cyc_AssnDef_binop(pa,t1,t2,topta);}else{goto _LL19;}}else{goto _LL19;}case 5: if(*((int*)_Tmp0.f1)==5){_TmpB=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*atype=_TmpB;void*ta1=_TmpA;void*btype=_Tmp8;void*tb1=_Tmp7;
# 2189
if(!Cyc_Unify_unify(atype,btype))goto _LL0;{
void*t1=Cyc_AssnDef_unify_term(accum,ta1,tb1);
return Cyc_AssnDef_cast(atype,t1);}}}else{goto _LL19;}case 6: if(*((int*)_Tmp0.f1)==6){_TmpB=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*ma=_TmpB;void*aa=_TmpA;void*ta=_Tmp8;void*mb=_Tmp7;void*ab=_Tmp6;void*tb=_Tmp5;
# 2193
void*m=Cyc_AssnDef_unify_term(accum,ma,mb);
void*a=Cyc_AssnDef_unify_term(accum,aa,ab);
return Cyc_AssnDef_select(m,a,ta);}}else{goto _LL19;}case 7: if(*((int*)_Tmp0.f1)==7){_TmpB=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*ma=_TmpB;void*aa=_TmpA;void*va=_Tmp8;void*mb=_Tmp7;void*ab=_Tmp6;void*vb=_Tmp5;
# 2197
if(aa==ab){
void*m=Cyc_AssnDef_unify_term(accum,ma,mb);
void*v=Cyc_AssnDef_unify_term(accum,va,vb);
return Cyc_AssnDef_update(m,aa,v);}else{
# 2202
void*t1=({void*_TmpD=({void*_TmpE=ma;void*_TmpF=ab;Cyc_AssnDef_update(_TmpE,_TmpF,({void*_Tmp10=ma;void*_Tmp11=ab;Cyc_AssnDef_select(_Tmp10,_Tmp11,Cyc_AssnDef_get_term_type(vb));}));});void*_TmpE=aa;Cyc_AssnDef_update(_TmpD,_TmpE,va);});
void*t2=({void*_TmpD=Cyc_AssnDef_update(mb,ab,vb);void*_TmpE=aa;Cyc_AssnDef_update(_TmpD,_TmpE,({void*_TmpF=Cyc_AssnDef_update(mb,ab,vb);void*_Tmp10=aa;Cyc_AssnDef_select(_TmpF,_Tmp10,Cyc_AssnDef_get_term_type(va));}));});
return Cyc_AssnDef_unify_term(accum,t1,t2);}}}else{goto _LL19;}case 8: if(*((int*)_Tmp0.f1)==8){_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpB=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f3;_TmpA=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f4;{int aisu=_Tmp4;unsigned atag=_Tmp3;struct Cyc_List_List*ats=_TmpB;void*atopt=_TmpA;int bisu=_Tmp2;unsigned btag=_Tmp1;struct Cyc_List_List*bts=_Tmp8;void*btopt=_Tmp7;
# 2213
if((aisu!=bisu || atag!=btag)||({int _TmpD=Cyc_List_length(ats);_TmpD!=Cyc_List_length(bts);}))goto _LL0;{
struct Cyc_List_List*ts=0;
for(1;ats!=0 && bts!=0;(ats=ats->tl,bts=bts->tl)){
ts=({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpE=Cyc_AssnDef_unify_term(accum,(void*)ats->hd,(void*)bts->hd);_TmpD->hd=_TmpE;}),_TmpD->tl=ts;_TmpD;});}{
int _TmpD=aisu;unsigned _TmpE=atag;struct Cyc_List_List*_TmpF=Cyc_List_imp_rev(ts);return Cyc_AssnDef_aggr(_TmpD,_TmpE,_TmpF,atopt);}}}}else{goto _LL19;}case 9: if(*((int*)_Tmp0.f1)==9){_TmpB=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((unsigned)_Tmp3==(unsigned)_Tmp1){void*at=_TmpB;unsigned ai=_Tmp3;void*atopt=_TmpA;void*bt=_Tmp8;unsigned bi=_Tmp1;void*btopt=_Tmp7;
# 2219
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
return Cyc_AssnDef_proj(t,ai,atopt);}else{goto _LL19;}}else{goto _LL19;}case 10: if(*((int*)_Tmp0.f1)==10){_TmpB=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*at=_TmpB;unsigned ai=_Tmp3;void*av=_TmpA;void*bt=_Tmp8;unsigned bi=_Tmp1;void*bv=_Tmp7;
# 2222
if(ai==bi){
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
void*v=Cyc_AssnDef_unify_term(accum,av,bv);
return Cyc_AssnDef_aggr_update(t,ai,v);}else{
# 2227
void*t1=({void*_TmpD=({void*_TmpE=ta;unsigned _TmpF=bi;Cyc_AssnDef_aggr_update(_TmpE,_TmpF,({void*_Tmp10=ta;unsigned _Tmp11=bi;Cyc_AssnDef_proj(_Tmp10,_Tmp11,Cyc_AssnDef_get_term_type(bv));}));});unsigned _TmpE=ai;Cyc_AssnDef_aggr_update(_TmpD,_TmpE,av);});
void*t2=({void*_TmpD=Cyc_AssnDef_aggr_update(tb,bi,bv);unsigned _TmpE=ai;Cyc_AssnDef_aggr_update(_TmpD,_TmpE,({void*_TmpF=Cyc_AssnDef_aggr_update(tb,bi,bv);unsigned _Tmp10=ai;Cyc_AssnDef_proj(_TmpF,_Tmp10,Cyc_AssnDef_get_term_type(av));}));});
return Cyc_AssnDef_unify_term(accum,t1,t2);}}}else{goto _LL19;}case 11: if(*((int*)_Tmp0.f1)==11){_TmpB=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((int)((struct Cyc_Absyn_Vardecl*)_TmpB)==(int)((struct Cyc_Absyn_Vardecl*)_Tmp7)){struct Cyc_Absyn_Vardecl*ax=_TmpB;void*ai=_TmpA;void*atopt=_Tmp8;struct Cyc_Absyn_Vardecl*bx=_Tmp7;void*bi=_Tmp6;void*btopt=_Tmp5;
# 2232
void*i=Cyc_AssnDef_unify_term(accum,ai,bi);
return Cyc_AssnDef_addr(ax,i);}else{goto _LL19;}}else{goto _LL19;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpB=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((unsigned)_Tmp3==(unsigned)_Tmp1){void*at=_TmpB;unsigned ai=_Tmp3;void*atopt=_TmpA;void*bt=_Tmp8;unsigned bi=_Tmp1;void*btopt=_Tmp7;
# 2238
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
return Cyc_AssnDef_offsetf(t,ai,atopt);}else{goto _LL19;}}else{goto _LL19;}case 14: if(*((int*)_Tmp0.f1)==14){_TmpB=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*at=_TmpB;void*ai=_TmpA;void*atopt=_Tmp8;void*bt=_Tmp7;void*bi=_Tmp6;void*btopt=_Tmp5;
# 2241
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
void*i=Cyc_AssnDef_unify_term(accum,ai,bi);
return Cyc_AssnDef_offseti(t,i,atopt);}}else{goto _LL19;}case 15: if(*((int*)_Tmp0.f1)==15){_TmpB=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*at=_TmpB;void*bt=_TmpA;
# 2245
void*t=Cyc_AssnDef_unify_term(accum,at,bt);
return Cyc_AssnDef_tagof_tm(t);}}else{goto _LL19;}default: _LL19:
 goto _LL0;}_LL0:;}{
# 2249
void*topta=Cyc_AssnDef_get_term_type(ta);
void*toptb=Cyc_AssnDef_get_term_type(tb);
void*topt=0;
if((topta!=0 && toptb!=0)&& Cyc_Unify_unify(topta,toptb))
# 2254
topt=topta;{
void*v=Cyc_AssnDef_fresh_var(topt);
({void*_Tmp0=({void*_Tmp1=accum->a1;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,ta));});accum->a1=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=accum->a2;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,tb));});accum->a2=_Tmp0;});
return v;}}}
# 2265
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm1(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
void**t2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(accum->m2,vd);
# 2269
if(t2==0){
if(*((int*)t)==2){
# 2272
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,t);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 2277
goto _LL0;}else{
# 2279
goto _LL0;}_LL0:;{
# 2281
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=accum->a1;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,t));});accum->a1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,v);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,v);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;}}
# 2289
if(t==*t2){
# 2292
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 2297
{struct _tuple15 _Tmp0=({struct _tuple15 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=*t2;_Tmp1;});if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){
# 2299
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 2301
({void*_Tmp1=({void*_Tmp2=accum->a2;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(t,*t2));});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}else{
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 2307
({void*_Tmp1=({void*_Tmp2=accum->a1;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(t,*t2));});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});
return accum;}}
# 2312
goto _LL5;}else{
# 2314
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 2316
({void*_Tmp1=({void*_Tmp2=accum->a2;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(t,*t2));});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}
# 2321
goto _LL5;}}else{if(*((int*)_Tmp0.f1)==2){
# 2323
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 2325
({void*_Tmp1=({void*_Tmp2=accum->a1;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(t,*t2));});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});
return accum;}
# 2330
goto _LL5;}else{
# 2332
goto _LL5;}}_LL5:;}{
# 2334
void*newt=Cyc_AssnDef_unify_term(accum,t,*t2);
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,newt);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,newt);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,newt);accum->m=_Tmp0;});
return accum;}}
# 2345
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm2(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
{
# 2351
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,t);Cyc_AssnDef_and(_Tmp1,accum->a2);});accum->a2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;};}
# 2358
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void){
static struct Cyc_Dict_Dict*term_dict=0;
if(term_dict==0)
term_dict=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_empty(Cyc_Core_ptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
# 2363
return*term_dict;}
# 2371
struct _tuple14 Cyc_AssnDef_unify_var_maps(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple14 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;return _Tmp0;}
if(am2.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple14 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;return _Tmp0;}{
struct Cyc_AssnDef_Accum accum=({struct Cyc_AssnDef_Accum _Tmp0;_Tmp0.a1=am1.assn,_Tmp0.a2=am2.assn,_Tmp0.m1=am1.map,_Tmp0.m2=am2.map,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.m=_Tmp1;});_Tmp0;});
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_foldm1,am1.map,& accum);
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=({struct Cyc_AssnDef_Accum*(*_Tmp1)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp1;});_Tmp0(Cyc_AssnDef_foldm2,Cyc_Dict_difference(am2.map,am1.map),& accum);});{
struct _tuple14 _Tmp0;_Tmp0.f0=accum.a1,_Tmp0.f1=accum.a2,_Tmp0.f2=accum.m;return _Tmp0;}}}
# 2381
static struct Cyc_Dict_Dict*Cyc_AssnDef_collect_subst(struct Cyc_Dict_Dict*map2,struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_Dict_Dict*dict){
if(*((int*)value)==2){
# 2387
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(*map2,vd);
# 2390
if(Cyc_Dict_lookup_opt(*dict,value)==0){
if(v!=0 &&*v==value){
struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_insert(*dict,value,*v);*_Tmp0=_Tmp1;});return _Tmp0;}{
struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict _Tmp2=*dict;void*_Tmp3=value;Cyc_Dict_insert(_Tmp2,_Tmp3,Cyc_AssnDef_fresh_var(vd->type));});*_Tmp0=_Tmp1;});return _Tmp0;}}
# 2395
goto _LL0;}else{
# 2397
goto _LL0;}_LL0:;
# 2399
return dict;}
# 2409 "assndef.cyc"
struct _tuple14 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple14 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;return _Tmp0;}
if(am2.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple14 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;return _Tmp0;}{
# 2415
struct Cyc_Dict_Dict subst_dict=*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp1;});struct Cyc_Dict_Dict*_Tmp1=& am2.map;struct Cyc_Dict_Dict _Tmp2=am1.map;_Tmp0(Cyc_AssnDef_collect_subst,_Tmp1,_Tmp2,({struct Cyc_Dict_Dict*_Tmp3=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp4=Cyc_AssnDef_empty_term_dict();*_Tmp3=_Tmp4;});_Tmp3;}));});
struct Cyc_AssnDef_AssnMap _Tmp0=Cyc_AssnDef_subst_am(subst_dict,am2);struct Cyc_AssnDef_AssnMap am2=_Tmp0;
return Cyc_AssnDef_unify_var_maps(am1,am2);}}
# 2420
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
struct _tuple14 _Tmp0=Cyc_AssnDef_unify_var_maps(am1,am2);struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*a1=_Tmp3;void*a2=_Tmp2;struct Cyc_Dict_Dict map=_Tmp1;
struct Cyc_AssnDef_AssnMap _Tmp4;({void*_Tmp5=Cyc_AssnDef_or(a1,a2);_Tmp4.assn=_Tmp5;}),_Tmp4.map=map;return _Tmp4;}}struct Cyc_AssnDef_Canonical_am_accum{void*assn;struct Cyc_Dict_Dict map;struct Cyc_Set_Set*used_lvars;};
# 2432
static struct Cyc_AssnDef_Canonical_am_accum*Cyc_AssnDef_fold_canonical(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_AssnDef_Canonical_am_accum*accum){
if(*((int*)value)==2){
# 2435
if(!Cyc_Set_member(accum->used_lvars,value)){
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,value);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,value);accum->map=_Tmp0;});
return accum;}
# 2440
goto _LL0;}else{
# 2442
goto _LL0;}_LL0:;{
# 2444
void*v=Cyc_AssnDef_fresh_var(vd->type);
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,v);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,v);accum->map=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=accum->assn;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,value));});accum->assn=_Tmp0;});
return accum;}}
# 2455
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap am){
struct Cyc_AssnDef_Canonical_am_accum*accum;accum=_cycalloc(sizeof(struct Cyc_AssnDef_Canonical_am_accum)),accum->assn=am.assn,({struct Cyc_Dict_Dict _Tmp0=Cyc_AssnDef_empty_map();accum->map=_Tmp0;}),({struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(Cyc_Core_ptrcmp);accum->used_lvars=_Tmp0;});
({struct Cyc_AssnDef_Canonical_am_accum*(*_Tmp0)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*)=(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_fold_canonical,am.map,accum);{
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=accum->assn,_Tmp0.map=accum->map;return _Tmp0;}}
# 2468 "assndef.cyc"
void*Cyc_AssnDef_merge_assnmaps(struct Cyc_List_List*vds1,struct Cyc_AssnDef_AssnMap a1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap a2){
# 2470
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
# 2472
void**v1=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a1.map,Cyc_AssnDef_memory);
void**v2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a2.map,Cyc_AssnDef_memory);
if(v1!=0 && v2!=0)
dict=Cyc_Dict_insert(dict,*v2,*v1);
# 2477
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v1=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a1.map,(struct Cyc_Absyn_Vardecl*)vds1->hd);
void**v2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a2.map,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd));
if(v1!=0 && v2!=0)
dict=Cyc_Dict_insert(dict,*v2,*v1);}}
# 2492
if(Cyc_Dict_is_empty(dict))return a2.assn;else{
return Cyc_AssnDef_subst_a(dict,a2.assn);}}
# 2504 "assndef.cyc"
extern struct Cyc_AssnDef_AssnMap*Cyc_AssnDef_subst_vardecls(struct Cyc_List_List*vds1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap*a){
# 2507
if(a==0)return 0;{
struct Cyc_Dict_Dict newmap=Cyc_AssnDef_empty_map();
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
# 2511
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a->map,Cyc_AssnDef_memory);
if(v!=0){
void*u=Cyc_AssnDef_fresh_var(0);
newmap=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(newmap,Cyc_AssnDef_memory,u);
dict=Cyc_Dict_insert(dict,*v,u);}
# 2517
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a->map,(struct Cyc_Absyn_Vardecl*)vds1->hd);
if(v!=0){
void*u=Cyc_AssnDef_fresh_var(_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd)->type);
# 2523
newmap=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(newmap,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd),u);
# 2525
dict=Cyc_Dict_insert(dict,*v,u);}}}{
# 2530
void*newa=Cyc_AssnDef_subst_a(dict,a->assn);
struct Cyc_AssnDef_AssnMap*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));_Tmp0->assn=newa,_Tmp0->map=newmap;return _Tmp0;}}}
# 2539
void*Cyc_AssnDef_subst_args(struct Cyc_List_List*args,struct Cyc_Absyn_Vardecl*res,struct Cyc_List_List*actual_args,void*actual_res,void*mem,struct Cyc_AssnDef_AssnMap am){
# 2543
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
void**mem_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,Cyc_AssnDef_memory);
if(mem_val!=0)
dict=Cyc_Dict_insert(dict,*mem_val,mem);
if(res!=0 && actual_res!=0){
void**res_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,res);
if(res_val!=0)
dict=Cyc_Dict_insert(dict,*res_val,actual_res);}
# 2552
for(1;args!=0 && actual_args!=0;(args=args->tl,actual_args=actual_args->tl)){
if((struct Cyc_Absyn_Vardecl*)args->hd!=0 &&(void*)actual_args->hd!=0){
void**arg_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,(struct Cyc_Absyn_Vardecl*)args->hd);
if(arg_val!=0)
dict=Cyc_Dict_insert(dict,*arg_val,(void*)actual_args->hd);}}
# 2559
return Cyc_AssnDef_subst_a(dict,am.assn);}
# 2562
void Cyc_AssnDef_reset_hash_cons_table (void){
# 2565
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_assn_hash_cons_table=0;}
# 2569
static void Cyc_AssnDef_f_sizeof_hashtable(void*key,void*value,int*counter){
++(*counter);}
# 2573
int Cyc_AssnDef_sizeof_hash_cons_table (void){
int size=0;
if(Cyc_AssnDef_term_hash_cons_table!=0)
({void(*_Tmp0)(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*)=(void(*)(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*))Cyc_Hashtable_iter_c;_Tmp0;})(({void(*_Tmp0)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*)=(void(*)(void*,struct Cyc_AssnDef_TermHashedInfo*,int*))Cyc_AssnDef_f_sizeof_hashtable;_Tmp0;}),Cyc_AssnDef_term_hash_cons_table,& size);
# 2578
if(Cyc_AssnDef_assn_hash_cons_table!=0)
({void(*_Tmp0)(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*)=(void(*)(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*),struct Cyc_Hashtable_Table*,int*))Cyc_Hashtable_iter_c;_Tmp0;})(({void(*_Tmp0)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*)=(void(*)(void*,struct Cyc_AssnDef_AssnHashedInfo*,int*))Cyc_AssnDef_f_sizeof_hashtable;_Tmp0;}),Cyc_AssnDef_assn_hash_cons_table,& size);
# 2581
return size;}
# 2584
struct Cyc_Hashtable_Table*Cyc_AssnDef_empty_term_table (void){
return Cyc_Hashtable_create(33,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);}
# 2588
void*Cyc_AssnDef_subst_table(struct Cyc_Hashtable_Table*table,void*t){
void**sopt=Cyc_Hashtable_lookup_opt(table,t);
if(sopt!=0)return*sopt;
{unsigned _Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)t)){case 2:
 goto _LL0;case 0:
 goto _LL0;case 1:
 goto _LL0;case 11: _Tmp6=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;void*t1=_Tmp5;void*tp=_Tmp4;
# 2596
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1){
struct Cyc_AssnDef_Addr_AssnDef_Term_struct a=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp7;_Tmp7.tag=11,_Tmp7.f1=vd,_Tmp7.f2=s1,_Tmp7.f3=tp;_Tmp7;});
t=Cyc_AssnDef_hash_cons_term((void*)& a);}
# 2601
goto _LL0;}case 12: _Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f5;{struct Cyc_Absyn_Exp*e1=_Tmp6;int i=_Tmp3;void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp2;
# 2603
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1 || s2!=t2){
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct a=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=e1,_Tmp7.f2=i,_Tmp7.f3=s1,_Tmp7.f4=s2,_Tmp7.f5=tp;_Tmp7;});
t=Cyc_AssnDef_hash_cons_term((void*)& a);}
# 2609
goto _LL0;}case 6: _Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 2611
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1 || s2!=t2)t=Cyc_AssnDef_select(s1,s2,tp);
goto _LL0;}case 7: _Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*t3=_Tmp4;
# 2616
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
void*s3=Cyc_AssnDef_subst_table(table,t3);
if((s1!=t1 || s2!=t2)|| s3!=t3)t=Cyc_AssnDef_update(s1,s2,s3);
goto _LL0;}case 3: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*tp=_Tmp5;
# 2622
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)t=Cyc_AssnDef_unop(p,s1,tp);
goto _LL0;}case 4: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 2626
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1 || s2!=t2)t=Cyc_AssnDef_binop(p,s1,s2,tp);
goto _LL0;}case 5: _Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp6;void*t1=_Tmp5;
# 2631
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)t=Cyc_AssnDef_cast(tp,s1);
goto _LL0;}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp6=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int b=_Tmp3;unsigned tag=_Tmp0;struct Cyc_List_List*tlist=_Tmp6;void*tp=_Tmp5;
# 2635
struct Cyc_List_List*slist=0;
int changed=0;
for(1;tlist!=0;tlist=tlist->tl){
void*t1=(void*)tlist->hd;
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)changed=1;
slist=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=s1,_Tmp7->tl=slist;_Tmp7;});}
# 2643
if(changed){
Cyc_List_imp_rev(slist);
t=Cyc_AssnDef_aggr(b,tag,slist,tp);}
# 2647
goto _LL0;}case 9: _Tmp6=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 2649
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)t=Cyc_AssnDef_proj(s1,i,tp);
goto _LL0;}case 10: _Tmp6=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*t2=_Tmp5;
# 2653
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1 || s2!=t2)t=Cyc_AssnDef_aggr_update(s1,i,s2);
goto _LL0;}case 13: _Tmp6=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 2658
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)t=Cyc_AssnDef_offsetf(s1,i,tp);
goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 2662
void*s1=Cyc_AssnDef_subst_table(table,t1);
void*s2=Cyc_AssnDef_subst_table(table,t2);
if(s1!=t1 || s2!=t2)t=Cyc_AssnDef_offseti(s1,s2,tp);
goto _LL0;}default: _Tmp6=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp6;
# 2667
void*s1=Cyc_AssnDef_subst_table(table,t1);
if(s1!=t1)t=Cyc_AssnDef_tagof_tm(s1);
goto _LL0;}}_LL0:;}
# 2671
return t;}
